#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Motor.h"
#include "Key.h"
#include "string.h"
#include "Serial.h"
#include "Buzzer.h"
#include "LightSensor.h"
#include "Process.h"
int main(void)
{
	//TIM3_Init;
		Processes();  //所有操作封装在此文件
}