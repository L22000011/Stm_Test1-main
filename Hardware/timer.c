#include "timer.h"

/*
void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能TIM3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 设置定时器基本参数
    TIM_TimeBaseStructure.TIM_Period = 7200 - 1; // 计数周期为7200，即3s
    TIM_TimeBaseStructure.TIM_Prescaler = 9999; // 时钟预分频为10000，即10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 清除更新中断标志位
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

    // 使能定时器中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}*/