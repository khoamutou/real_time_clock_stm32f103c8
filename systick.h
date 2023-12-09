#ifndef SYSTICK_H
#define SYSTICK_H
#include "stm32f10x.h" // Device header

static volatile uint32_t DelayTimerTick = 0;
//
static void InitSysTick(void)
{
	SysTick_Config(72000); /* Configure the SysTick timer */
}

void SysTick_Handler(void)
{
	if (DelayTimerTick < 0xFFFFFFFF)
	{
		DelayTimerTick++; /* increment timer */
	}
}

void Delay_SysTick(uint32_t Millis)
{
	DelayTimerTick = 0; /* reset timer value */
	while (DelayTimerTick < Millis)
		; /* wait for timer */
}
#endif
