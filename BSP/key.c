#include "key.h"

uint16_t g_key1_long_press = 0;
uint16_t g_key2_long_press = 0;
uint16_t g_key3_long_press = 0;

//Key1

//检测按键是否被按下
static uint8_t Key1_is_Press(void)
{
    if((DL_GPIO_readPins(KEY_button1_PORT,KEY_button1_PIN) & KEY_button1_PIN)!=0)//按键按下
    {
        return KEY_PRESS;//返回1
    }
    else
    {
        return KEY_RELEASE;//返回0
    }
}

//按键状态机（一直被周期性调用，10ms）
// 读取按键K1的状态，按下返回1，松开返回0.
// mode:设置模式，0：按下一直返回1；1：按下只返回一次1
uint8_t Key1_State(uint8_t mode)
{
    static uint16_t key1_state=0;
    if(Key1_is_Press()==KEY_PRESS)//按键被按下
    {
        //兼顾了消抖
        if(key1_state<(mode+1)*2)//mode=0,(mode+1)*2=2  mode=1,(mode+1)*2=4
        {
            key1_state++;
        }
    }
    else//按键没有被按下
    {
        //重置状态
        key1_state=0;
        g_key1_long_press=0;
    }
    if(key1_state==2)//mode=0&&按键按下，会一直进入，mode=1&&按键按下只会进入一次
    {
        g_key1_long_press=1;//只要松开就会跳到上面的else，把长按清零
        return KEY_PRESS;
    }
    return KEY_RELEASE;//第一次调用无效，防抖机制
}
//检测长按
// 读取按键K1的长按状态，累计达到长按时间返回1，未达到返回0.
// timeout为设置时间长度，单位为秒（因为每10ms调用一次)
uint8_t Key1_Long_Press(uint16_t timeout)
{
    if(g_key1_long_press>0)
    {
        if(g_key1_long_press<timeout*100+2)//++100次为1s
        {
            g_key1_long_press++;
            if(g_key1_long_press==timeout*100+2)
            {
                return 1;//达到计数值才返回1，代表长按
            }
            return 0;
        }
    }
    return 0;
}

// Key2


uint8_t Key2_is_Press(void)
{
    if((DL_GPIO_readPins(KEY_button2_PORT, KEY_button2_PIN) & KEY_button2_PIN) ==0)
    {
        return KEY_PRESS;
    }
    else
    {
        return KEY_RELEASE;
    }
}

uint8_t Key2_State(uint8_t mode)
{
    static uint16_t key2_state = 0;
    if(Key2_is_Press() == KEY_PRESS)
    {
        if(key2_state < (mode + 1) * 2)
        {
            key2_state++;
        }
    }
    else
    {
        key2_state = 0;
        g_key2_long_press = 0;
    }
    if(key2_state == 2)
    {
        g_key2_long_press = 1;
        return KEY_PRESS;
    }
    return KEY_RELEASE;
}

uint8_t Key2_Long_Press(uint16_t timeout)
{
    if(g_key2_long_press > 0)
    {
        if(g_key2_long_press < timeout * 100 + 2)
        {
            g_key2_long_press++;
            if(g_key2_long_press == timeout * 100 + 2)
            {
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

// Key3


uint8_t Key3_is_Press(void)
{
    if((DL_GPIO_readPins(KEY_button3_PORT, KEY_button3_PIN) & KEY_button3_PIN) ==0)
    {
        return KEY_PRESS;
    }
    else
    {
        return KEY_RELEASE;
    }
}

uint8_t Key3_State(uint8_t mode)
{
    static uint16_t key3_state = 0;
    if(Key3_is_Press() == KEY_PRESS)
    {
        if(key3_state < (mode + 1) * 2)
        {
            key3_state++;
        }
    }
    else
    {
        key3_state = 0;
        g_key3_long_press = 0;
    }
    if(key3_state == 2)
    {
        g_key3_long_press = 1;
        return KEY_PRESS;
    }
    return KEY_RELEASE;
}

uint8_t Key3_Long_Press(uint16_t timeout)
{
    if(g_key3_long_press > 0)
    {
        if(g_key3_long_press < timeout * 100 + 2)
        {
            g_key3_long_press++;
            if(g_key3_long_press == timeout * 100 + 2)
            {
                return 1;
            }
            return 0;
        }
    }
    return 0;
}


//按键扫描（用于菜单选择）
// uint8_t Key_Scan(void)//松开才返回键值版本
// {
//     //当按键松开后，才返回值，防止高频返回键值
//     static uint8_t release=1;//1:按键松开  0：按键按下
//     static volatile uint8_t key_num=0;//返回的键值
//     uint8_t ret=0;
//     //release=1时，检测到key1 or key2按下，release=0，按下状态
//     if(release==1&&(Key1_is_Press()==KEY_PRESS||Key2_is_Press()==KEY_PRESS))
//     {
//         delay_ms(20);//消抖
//         release=0;
//         if(Key1_is_Press()==KEY_PRESS)
//         {
//             //赋值而不是直接返回，等松开后再返回
//             key_num = KEY1_PRES;//值为1
//         }
//         else if(Key2_is_Press()==KEY_PRESS)
//         {
//             key_num = KEY2_PRES;//值为2
//         }
//     }
//     else if(Key1_is_Press()==KEY_RELEASE && Key2_is_Press()==KEY_RELEASE)//key1 or key2都松开
//     {
//         release=1;//按键松开
//         ret=key_num; 
//         key_num=0;//重置键值
//         return ret; 
//     }
//	 delay_ms(10);
//     return 0;
// }

uint8_t Key_Scan(void)//按下立即返回键值版本
{
    //按下后立即返回键值，随后仍保持按下会返回0，保证只返回一次keyx 键值x
    static uint8_t release=1;//初始为松开状态
    volatile uint8_t ret=0;//每次循环开始ret都归零
    //release=1时，检测到key1 or key2按下，release=0，按下状态
    if(release==1&&(Key1_is_Press()==KEY_PRESS||Key2_is_Press()==KEY_PRESS||Key3_is_Press()==KEY_PRESS))
    {
        delay_ms(10);//消抖
        release=0;
        if(Key1_is_Press()==KEY_PRESS)
        {
            //赋值而不是直接返回，等松开后再返回
            ret = KEY1_PRES;//值为1
        }
        else if(Key2_is_Press()==KEY_PRESS)
        {
            ret = KEY2_PRES;//值为2
        }
        else if(Key3_is_Press()==KEY_PRESS)
        {
            ret = KEY3_PRES;
        }
    }
    else if(Key1_is_Press()==KEY_RELEASE && Key2_is_Press()==KEY_RELEASE && Key3_is_Press()==KEY_RELEASE)//key1 or key2都松开
    {
        release=1;//设为松开状态
    }
    return ret;
}
//小车模式选择函数（OLED+key）
//uint8_t switch_mode(void)
//{
//	uint8_t	select_mode = 0, key_num = 0;
//	OLED_Clear();
//	OLED_ShowString(0, 0, "Select Mode:", 8, 1);
//	OLED_ShowNum(80,0,select_mode,1,24,1);
//	OLED_Refresh();
//	while(1)
//    {
//		key_num = Key_Scan();
//		if(key_num)
//        {
//			if(key_num == KEY1_PRES)
//            {
//				select_mode +=1;
//			}
//			else if(key_num == KEY2_PRES)
//            {
//				OLED_ShowString(92,0,"OK!!!",8,1);
//				OLED_Refresh();
//				delay_ms(700);
//				OLED_Clear();
//				break;
//			}
//			if(select_mode > 3 )
//            {
//				select_mode = 0;
//			}
//			OLED_ShowNum(80,0,select_mode,1,24,1);
//			OLED_Refresh();
//		}
//        delay_ms(10);
//	}
//	return select_mode;
//}

uint8_t switch_mode(void)
{
	uint8_t	select_mode = 1, key_num = 0;
	OLED_Clear();
	OLED_ShowString_Grid(1,0,"Select Mode:",1,1,1);
	OLED_ShowNum_Grid(1,13,select_mode,1,1,0,1);
    OLED_ShowString_Grid(2,0,"bias:",1,1,0);
    OLED_ShowSNum_Grid(2,5,turn_adjust,3,1,0,1);
	OLED_Refresh();
	
	while(1)
    {
		key_num = Key_Scan();
		if(key_num)
        {
			if(key_num == KEY1_PRES)
            {
				select_mode +=1;
				if(select_mode > 6 )
				{
					select_mode = 0;
				}
				OLED_ShowNum_Grid(1,13,select_mode,1,1,0,1);
				OLED_Refresh();
			}
			else if(key_num == KEY2_PRES)
            {
				OLED_ShowString(92,0,"OK!!!",8,1);
				OLED_Refresh();
				delay_ms(700);
				OLED_Clear();
				break;
			}
			else if(key_num == KEY3_PRES)//掉头调整值设置
            {
                if(turn_adjust>50)
                {
                    turn_adjust=-50.0f;
                }
                turn_adjust+=5.0f;
                OLED_ShowSNum_Grid(2,5,turn_adjust,3,1,0,1);
            }
		}
        delay_ms(10);
	}
	return select_mode;
}

