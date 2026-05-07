#ifndef __TASK_H__
#define __TASK_H__

#include "bsp.h"

extern uint8_t black_line_count;
extern volatile int8_t turn_adjust;
typedef void(*TaskFunc)(int arg);// 定义统一的函数指针类型

//任务结构体
typedef struct
{
    uint32_t interval;//期望的执行时间间隔,单位为ms
    uint32_t last_call;//上次调用的时间
    //void (*task)(void);//名为 task 函数指针，可以调用相同返回值，参数的函数
    TaskFunc task_func;
    int arg;
}Task;
void Reset_flag(void);
void Schedule_Run(void);
void test_turn_simple(int8_t direction, float angle);

#endif