#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "OLED.h"
//#include "Serial.h"
//#include "Delay.h"
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init_DJ();
}

void Motor_SetSpeed(int8_t Speed)
{
    if (Speed > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare3(Speed);
    }
    else if (Speed < 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare3(-Speed);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare3(0);
    }
}

void Motor_SetSpeed_demo(int8_t Speed)
{
    if (Speed > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);    // 设置电机正转方向引脚为高电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);    // 设置电机反转方向引脚为低电平
        PWM_SetCompare3(Speed);
    }
    else if (Speed < 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);    // 设置电机正转方向引脚为低电平
        GPIO_SetBits(GPIOA, GPIO_Pin_5);    // 设置电机反转方向引脚为高电平
        PWM_SetCompare3(-Speed);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);    // 设置电机正转方向引脚为低电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);    // 设置电机反转方向引脚为低电平
        PWM_SetCompare3(0);
    }
}



/*
//电机的TT模式
void TTMode()
{
    Serial_SendString("已进入TT模式：Push 5s -> Pull 5s -> 停止\r\n");
		
    // Push 5s
    Motor_SetSpeed(-100);
    Serial_SendString("Push 5s...向外排风循环中...\r\n");
    Delay_ms(5000);

    // Pull 5s
    Motor_SetSpeed(50);
    Serial_SendString("Pull 5s...向内循环输送氧气中...\r\n");
    Delay_ms(5000);

    // 停止
    Motor_SetSpeed(0);
    Serial_SendString("已停止电机，循环风结束\r\n");
}
void Motor_SetDirection(int direction)
{
    // 根据direction的值设置电机的转向
    // 假设使用GPIOA4和GPIOA5控制电机转向
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    if (direction > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    }
    else if (direction < 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
    }
}*/