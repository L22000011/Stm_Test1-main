#include "stm32f10x.h"                  // Device header
#include "PWM.h"
//#include "Serial.h"
//#include "string.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle / 180 * 2000 + 500);
}
