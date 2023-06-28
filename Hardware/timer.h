#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"

void Timer_Init(void);
void Delay_ms(uint32_t milliseconds);
void TIM2_IRQHandler(void);



//void TIM3_Init(void);
#endif /* TIMER_H */
