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
// 定义电机前进方向的引脚
#define MOTOR_FORWARD_PIN GPIO_Pin_4
#define MOTOR_FORWARD_PORT GPIOA

// 设置电机前进方向
#define MOTOR_FORWARD() GPIO_SetBits(MOTOR_FORWARD_PORT, MOTOR_FORWARD_PIN)

uint8_t KeyNum;
float Angle;
int8_t Speed = 0;
uint8_t CommandReceived = 0; // 标志位，表示是否接收到了指令
uint8_t feedingCount = 0; // 记录喂食次数
uint8_t isTimerMode = 0; // 标志位，表示是否处于定时器模式
uint8_t isMotorRunning = 0;
uint8_t tag = 1;
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
}
// TT模式函数
void TTMode()
{		
    Serial_SendString("已进入TT模式：Push 5s -> Pull 5s -> 停止\r\n");
    Buzzer_Light(); // 蜂鸣器响一声    
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
// 人为模式函数
void ManualMode()
{
    Serial_SendString("已进入人为模式：Servo人为喂食，TT模式驱动电机，Exit退出当前模式\r\n");
    feedingCount = 0; // 重置喂食次数
    while (1)
    {
			Buzzer_Light(); // 蜂鸣器响一声
        if (Serial_RxFlag == 1)
        {
            if (strcmp(Serial_RxPacket, "Servo") == 0)
            {
                Angle += 180; // 更改舵机转角度
                if (Angle > 180)
                    Angle = 0;
                Servo_SetAngle(Angle);

                // 发送舵机驱动完成信息
                Serial_SendString("{人为模式}舵机驱动完成，完成一次喂食\r\n");

                OLED_ShowNum(1, 7, Angle, 3);

                // 增加喂食计数显示
                feedingCount++;
                OLED_ShowString(2, 1, "Feeding Count:");
                OLED_ShowNum(2, 15, feedingCount, 1);
            }
            else if (strcmp(Serial_RxPacket, "TT") == 0)
            {
                // 调用TT模式驱动电机
                TTMode();
            }
                        else if (strcmp(Serial_RxPacket, "Stop") == 0)
            {
                // 停止电机
                Motor_SetSpeed(0);
                Serial_SendString("已停止循环风电机\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Push") == 0)
            {
                // 开启Speed=-100的电机旋转
                Motor_SetSpeed(-100);
                Serial_SendString("启动循环风扇，向外排风[Speed=100]\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Pull") == 0)
            {
                // 开启Speed=100的电机旋转
                Motor_SetSpeed(50);
                Serial_SendString("启动氧气循环，输送新鲜空气[Speed=-100]\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Exit") == 0)
            {
                // 退出人为模式
                Serial_SendString("已退出人为模式\r\n");
                break;
            }
            else
            {
                Serial_SendString("请输入正确的指令\r\n");
            }

            Serial_RxFlag = 0; // 重置接收标志位
        }
    }
}



void Processes(void)
{
    OLED_Init();
    Servo_Init();
    Key_Init();
    Serial_Init();
    TIM3_Init(); // 初始化定时器3
    Motor_Init();

    OLED_ShowString(1, 1, "TxPacket");
    OLED_ShowString(3, 1, "RxPacket");
    OLED_ShowString(1, 1, "Angle:");
    Buzzer_Init();
	LightSensor_Init();
    // 提示输入模式选择
    Serial_SendString("选择模式（T-自动模式，M-人为模式，TT-TT模式）,Push,Pull风扇循环风\r\n");

    while (1)
    {   
        Buzzer_Light();
        if (Serial_RxFlag == 1)
        {
            
            OLED_ShowString(4, 1, "                ");
            OLED_ShowString(4, 1, Serial_RxPacket);

            if (strcmp(Serial_RxPacket, "T") == 0)
            {
                // 进入定时器模式
                isTimerMode = 1;
                CommandReceived = 1;
                Serial_SendString("已进入自动模式\r\n");
            }
            else if (strcmp(Serial_RxPacket, "M") == 0)
            {
                // 进入人为模式
                isTimerMode = 0;
                CommandReceived = 1;
               // Serial_SendString("人为模式：Servo人为喂食，Exit退出当前模式，TT自动循环风\r\n");
            }
             else if (strcmp(Serial_RxPacket, "TT") == 0)
            {
                    Serial_SendString("已进入TT模式\r\n");
                    TTMode();
            }
						 else if (strcmp(Serial_RxPacket, "Light_on") == 0)
            {
								Buzzer_ON200();
							//Serial_SendString("海洋馆缺少光，蜂鸣器报警\r\n");
            }
						 else if (strcmp(Serial_RxPacket, "Light_off") == 0)
            {
								Buzzer_OFF();
							Serial_SendString("海洋馆光充足，关闭补充光\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Stop") == 0)
            {
                // 停止电机
                Motor_SetSpeed(0);
                Serial_SendString("已停止循环风电机\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Push") == 0)
            {
                // 开启Speed=-100的电机旋转
                Motor_SetSpeed(-100);
                Serial_SendString("启动循环风扇，向外排风[Speed=100]\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Pull") == 0)
            {
                // 开启Speed=100的电机旋转
                Motor_SetSpeed(50);
                Serial_SendString("启动氧气循环，输送新鲜空气[Speed=-100]\r\n");
            }
            else
            {
                Serial_SendString("请输入正确的指令\r\n");
            }

            Serial_RxFlag = 0; // 重置接收标志位
        }

        if (CommandReceived == 1)
        {
            if (isTimerMode)
            {
                if (feedingCount < 5)
                {
                    Angle += 180; // 更改舵机转角度
                    if (Angle > 180)
                        Angle = 0;
                    Servo_SetAngle(Angle);

                    // 发送喂食完成信息
                    char buffer[30];
                    sprintf(buffer, "{自动模式} 舵机完成第%d次喂食\r\n", feedingCount + 1);
                    Serial_SendString(buffer);

                    feedingCount++; // 喂食次数加一

                    OLED_ShowNum(1, 7, Angle, 3);

                    Delay_ms(3000); // 延时3秒，控制喂食的间隔
                }
                                else if (strcmp(Serial_RxPacket, "Stop") == 0)
            {
                // 停止电机
                Motor_SetSpeed(0);
                Serial_SendString("已停止循环风电机\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Push") == 0)
            {
                // 开启Speed=-100的电机旋转
                Motor_SetSpeed(-100);
                Serial_SendString("启动循环风扇，向外排风[Speed=100]\r\n");
            }
            else if (strcmp(Serial_RxPacket, "Pull") == 0)
            {
                // 开启Speed=100的电机旋转
                Motor_SetSpeed(50);
                Serial_SendString("启动氧气循环，输送新鲜空气[Speed=-100]\r\n");
            }
						 else if (strcmp(Serial_RxPacket, "Light_on") == 0)
            {
								Buzzer_ON200();
							Serial_SendString("海洋馆缺少光，蜂鸣器报警\r\n");
            }
						 else if (strcmp(Serial_RxPacket, "Light_off") == 0)
            {
								Buzzer_OFF();
							Serial_SendString("海洋馆光充足，关闭补充光\r\n");
            }
                else
                {
                    CommandReceived = 0; // 重置标志位
                    feedingCount = 0; // 重置喂食次数
                    Serial_SendString("定时器模式下喂食结束\r\n");
                    Serial_SendString("请再次确认输入指令选择模式【T/M】\r\n");
                }
            }
            else
            {
                ManualMode();

                // 人为模式结束后，提示重新选择模式
                Serial_SendString("请选择模式（T-自动模式，M-人为模式，TT-TT模式）,Push,Pull风扇循环风\r\n");

                CommandReceived = 0; // 重置标志位
            }
        }

        // 增加适当的延时，以等待新指令
        Delay_ms(100);
    }
}

// 定时器3中断处理函数
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if (isTimerMode && CommandReceived)
        {
            CommandReceived = 1; // 标志位置1，表示接收到定时器中断
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位
    }
}
