#include "task.h"

#define speed_down_target     3//第几条黑线减速
#define speed_up_target     4  //第几条黑线加速

void black_control(int num);

//任务数组进行黑线计数控制
Task tasks[]=
{
    {10,0,black_control,6},
    
};

void Schedule_Run(void)
{
    uint32_t now=Get_Time();//获取当前时间
    //for循环遍历执行任务
    for(int i=0;i<sizeof(tasks)/sizeof(Task);i++)
    {
        if(now-tasks[i].last_call>=tasks[i].interval)//到达期望间隔时间
        {
            tasks[i].task_func(tasks[i].arg);//执行任务
            tasks[i].last_call=now;//更新last_call（上次执行任务的时间）
            
        }
    }
}


volatile int8_t turn_adjust=0;//转头后的调整角度
/**
 * @brief 掉头函数
 * @param direction 1: 顺时针右转掉头, -1: 逆时针左转掉头
 * @param angle 旋转的角度
 */
void test_turn_simple(int8_t direction, float angle)
{
    // 停止当前运动
    Motion_Stop(STOP_BRAKE);
    delay_ms(50);
    
    // 旋转速度（绝对值，单位 mm/s，对应 Motion_Car_Control 的 V_z 参数）
    int16_t spin_speed = 800 * direction;   // 200 是速度值，可调整
    
    float target_angle=target_yaw+(direction*angle);
    //环绕修正
    if (target_angle > 180.0f)  
    {
        target_angle -= 360.0f;
    }
    if (target_angle < -180.0f) 
    {
        target_angle += 360.0f;
    }

    // 旋转
    Motion_Car_Control(0, 0, spin_speed);

    while(true)
    {
        mpu_dmp_get_data(&pitch,&roll,&yaw);               // 更新 yaw

        // 计算最短角度差
        float diff = yaw - target_angle;

        if (diff > 180.0f)  
        {
            diff -= 360.0f;
        }
        if (diff < -180.0f) 
        {
            diff += 360.0f;
        }

        if (fabs(diff) < 8.0f)            // 误差 < 8° 即停止
        {
            break;
        }
            
        delay_ms(3);                     // 避免空转
    }
    
    // 停止
    Motion_Stop(STOP_BRAKE);
    target_yaw =target_yaw+(direction*angle)+turn_adjust;         // 更新全局目标航向，更新为初始target_yaw转180度
    if (target_yaw > 180.0f)  
    {
        target_yaw -= 360.0f;
    }
    if (target_yaw < -180.0f) 
    {
        target_yaw += 360.0f;
    }
    OLED_ShowSNum_Grid(3,11,target_yaw,4,1,0,1);//更新显示一次target_yaw
    StraightLineWalk_IMU_Reset();         // 下次直行重新锁定
}

uint8_t black_line_count=0;
static uint8_t brake_flag=0;//0:未进入刹车状态  1：进入刹车状态
static uint8_t stop_flag=0;
static uint8_t last_line=0;//0:白线  1：黑线
/**
 * @brief 重置黑线控制函数的状态
 * 
 */
void Reset_flag(void)
{
    brake_flag=0;
    stop_flag=0;
    last_line=0;
}
/**
 * @brief 遇到黑线刹车
 * 
 * @param num 遇到多少次黑线就刹车
 */
void black_control(int num)
{
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
    
    
    static uint32_t brake_delay_start=0;
    
    //8,7,6路均为黑，都可以触发
    uint8_t is_black = (x1==0 && x2==0 && x3==0 && x4==0 && 
                        x5==0 && x6==0 && x7==0 && x8==0)
                        ||(x2==0 && x3==0 && x4==0 && 
                        x5==0 && x6==0 && x7==0)||
                        (x2==0 && x3==0 && x4==0 && 
                        x5==0 && x6==0 && x7==0 && x8==0)
                        ||(x1==0 && x2==0 && x3==0 && x4==0 && 
                        x5==0 && x6==0 && x7==0);

    if(brake_flag==0)//未进入刹车状态
    {
        if(is_black==1 && last_line==0)//上次是白线且这次是黑线
        {
            black_line_count++;
            //调试显示
            OLED_ShowSNum_Grid(2,17,black_line_count,1,1,0,0);
            switch(black_line_count)//根据黑线次数决定加速和减速
            {
                case speed_down_target:
                g_IR_track_speed=125;
                break;

                case speed_up_target:
                g_IR_track_speed=250;
                break;

                default:
                break;
            }
        }
        if(black_line_count>=num)//达到次数后进入刹车状态
        {
            brake_flag=1;
            brake_delay_start=Get_Time();//记录当前时间
            black_line_count=0;//清零黑线次数
        }
        last_line=is_black;
    }

    if(brake_flag==1 && stop_flag==0)//进入掉头状态
    {
        if (Get_Time() - brake_delay_start >= 600)
        {
            black_line_count=1;//掉头时会越过一条黑线，所以加上
            brake_flag=0;
            stop_flag=1;//进入停止状态
            test_turn_simple(1,180);//开始掉头
        }
    }

    if(brake_flag==1 && stop_flag==1)//进入停止状态
    {
        if (Get_Time() - brake_delay_start >= 600)
        {
            brake_flag=0;
            stop_flag=0;
            Motion_Stop(STOP_BRAKE);
            g_IR_track_speed = 0;   // 标志停车，主循环会停止调用直行
            // while(true)
            // {
            //     delay_ms(5000);
            // }
        }
    }
    
    
}