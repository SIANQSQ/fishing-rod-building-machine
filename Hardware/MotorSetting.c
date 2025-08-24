#include "stm32f10x.h"                  // Device header

void MotorSetting_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);  //A1控制使能 //A2控制方向  //A8输出脉冲
}

void Motor_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void Motor_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void Motor_Fan(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void Motor_Zheng(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

