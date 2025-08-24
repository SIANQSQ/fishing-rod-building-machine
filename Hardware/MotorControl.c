#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#include "stm32f10x.h"

// ????????

// GPIO??????????
void ControlSwitch_EXTI_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ??GPIOB?AFIO??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // ??PB1?PB10?PB11?????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ?PB1?PB10?PB11????????
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

    // ???????
    EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line10 | EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ?????????
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // ??NVIC
    // ??EXTI1??
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ??EXTI15_10??
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void Motor_Control_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u16 Get_Motor_Dir()
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		return 1;  //顺时针
	else return 0;   //逆时针
}

u16 Get_Motor_Run()
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
		return 0;  //停止
	else return 1;  //启动

}
u16 Get_Motor_Control()
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		return 2;  //脚动
	else return 1;  //手动
}
