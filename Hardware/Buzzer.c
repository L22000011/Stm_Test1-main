#include "stm32f10x.h"                  // Device header
#include "LightSensor.h"
#include "Delay.h"
#include "Serial.h"
void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_ON(void)
{
	Serial_SendString("海洋馆缺少光，蜂鸣器报警\r\n");
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
}
//*******
void Buzzer_Light()
{
	if (LightSensor_Get() == 1)
        {
            Buzzer_OFF();
        }
        else
        {
						Serial_SendString("海洋馆缺少光，蜂鸣器报警\r\n");
            Buzzer_ON();
            Delay_ms(200);
            Buzzer_OFF();
        }
}

void Buzzer_ON200()
{
	Serial_SendString("海洋馆缺少光，蜂鸣器报警\r\n");
	Buzzer_ON();
  Delay_ms(400);
  Buzzer_OFF();
	Delay_ms(100);
	Buzzer_ON();
  Delay_ms(200);
  Buzzer_OFF();
	Delay_ms(200);
	Buzzer_ON();
	Delay_ms(100);
	Buzzer_OFF();
}