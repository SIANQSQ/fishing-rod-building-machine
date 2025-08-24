#include "stm32f10x.h"

// 定义最大频率，可根据步进电机和驱动器性能调整
#define MAX_FREQUENCY 50000  // 单位：Hz  //16000 -> 800xf   //

// 速度设定函数，使用百分比控制速度
void Set_Speed(uint8_t speed_percentage) {
    if (speed_percentage > 100) {
        speed_percentage = 100;
    }
    // 计算目标频率
    uint32_t target_frequency = (uint32_t)(speed_percentage * MAX_FREQUENCY / 100);
    // 计算自动重载值（ARR）
    uint16_t arr_value = (uint16_t)(SystemCoreClock / (72 * target_frequency)) - 1;
    // 更新TIM1的自动重载寄存器
    TIM1->ARR = arr_value;
}

// 初始化GPIO
void Pul_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 配置PA8为复用推挽输出，用于PWM输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

// 初始化TIM1用于PWM输出

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // TIM1基本配置
    TIM_TimeBaseStructure.TIM_Period = (SystemCoreClock / (72 * MAX_FREQUENCY)) - 1;  // 初始频率为最大频率
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // TIM1通道1 PWM配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (TIM1->ARR) / 2;  // 初始占空比为50%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    // 使能TIM1主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 使能TIM1
    TIM_Cmd(TIM1, ENABLE);
}

