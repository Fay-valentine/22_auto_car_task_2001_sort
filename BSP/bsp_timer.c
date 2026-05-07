#include "bsp_timer.h"

#if Timer_20ms_Switch

//20ms定时器
void Timer_20ms_Init(void)
{
    //打开20ms定时器
    NVIC_ClearPendingIRQ(TIMER_20ms_INST_INT_IRQN);
	  NVIC_EnableIRQ(TIMER_20ms_INST_INT_IRQN);
		DL_TimerG_startCounter(TIMER_20ms_INST);
}

u8 gled_cnt = 0;
//20ms定时器中断
void TIMER_20ms_INST_IRQHandler(void)
{
    //20ms归零中断触发
	if( DL_TimerG_getPendingInterrupt(TIMER_20ms_INST) == DL_TIMER_IIDX_ZERO )
	{
		IR_DataAnalysis();//八路红外模块数据处理
		Encoder_Update();//编码器更新
        Motion_Handle(); //小车驱动
        gled_cnt++;
        if(gled_cnt>=10)//200ms
        {
            gled_cnt=0;
            DL_GPIO_togglePins(LED_PORT,LED_MCU_PIN);
        }
        DL_TimerG_clearInterruptStatus(TIMER_20ms_INST, DL_TIMERG_INTERRUPT_ZERO_EVENT); // 新增
	}

}

#endif