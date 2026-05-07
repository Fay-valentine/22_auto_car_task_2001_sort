/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AllHeader.h"

//app_irtracking_eight设置了巡线速度

//转向偏移值为+10走的比较直

//主要使用到的全局变量：
//Vz_Bias
//target_yaw
//black_line_count
//turn_adjust
//yaw
int main(void)
{
    bsp_Init();//初始化
    delay_ms(500);
    uart0_send_string("$0,0,1#");//八路红外模块启动指令
    key_choose:
    uint8_t mode=switch_mode();//模式选择，turn_adjust值（转头后的补偿值）调整
	OLED_ShowString_Grid(1,0,"yaw:",1,1,1);//原始yaw
    OLED_ShowString_Grid(2,0,"black_line_count:",1,1,1);//黑线计数
    OLED_ShowString_Grid(3,0,"target_yaw:",1,1,1);//目标yaw
    
    while (1) 
    {
        if(mode==1)
        {
            Schedule_Run();
            if (g_IR_track_speed != 0)// 速度不为0说明未停车，继续IMU直行
            {
                StraightLineWalk_IMU();
            }
            //按下key2表示到终点，黑线计数为6，用于调试
            if(Key2_is_Press() == KEY_PRESS) 
            {
                black_line_count=6;
            }
            if(Key3_is_Press() == KEY_PRESS) //按下key3重置状态，避免复位需要等待MPU6050零飘
            {
                Motion_Stop(STOP_BRAKE);
                g_IR_track_speed = 250;          // 恢复默认前进速度
                black_line_count=0;//恢复黑线次数
                Reset_flag();
                StraightLineWalk_IMU_Reset();
                Yaw_lock();
                delay_ms(200);
                goto key_choose;
            }
            // 所有模式的公共低频刷新
            static uint32_t last_oled = 0;
            if (Get_Time() - last_oled > 200)
            {
                OLED_ShowSNum_Grid(1,4,yaw,4,1,0,0);//刷新yaw
                OLED_Refresh();
                last_oled = Get_Time();
            }
        }

        
    }
}

