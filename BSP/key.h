#ifndef __KEY_H__
#define __KEY_H__
//按键均为上拉电阻，按下为低电平
#include "AllHeader.h"


#define KEY_PRESS      1
#define KEY_RELEASE    0

#define KEY_MODE_ONE_TIME   1
#define KEY_MODE_ALWAYS     0

#define KEY1_PRES      1
#define KEY2_PRES      2
#define KEY3_PRES      3

uint8_t Key1_State(uint8_t mode);
uint8_t Key1_Long_Press(uint16_t timeout);

uint8_t Key2_is_Press(void);
uint8_t Key2_State(uint8_t mode);
uint8_t Key2_Long_Press(uint16_t timeout);

uint8_t Key3_is_Press(void);
uint8_t Key3_State(uint8_t mode);
uint8_t Key3_Long_Press(uint16_t timeout);

uint8_t Key_Scan(void);
uint8_t switch_mode(void);

#endif