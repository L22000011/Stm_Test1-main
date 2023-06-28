#ifndef __PROCESS_H
#define __PROCESS_H

void TIM3_Init(void);
void Motor_SetDirection(int direction);
void TTMode();
void ManualMode();
void Processes(void);
void TIM3_IRQHandler(void);


#endif