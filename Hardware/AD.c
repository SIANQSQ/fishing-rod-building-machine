#include "stm32f10x.h"

// ????AD???????
volatile uint16_t AD_Value[2];

// ???GPIO
void AD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ??GPIOA??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // ??PA4?PA5?????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure;

    // ??ADC1??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // ADC??
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 2;
    ADC_Init(ADC1, &ADC_InitStructure);

    // ??ADC??4?5
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);

    // ??ADC?DMA??
    ADC_DMACmd(ADC1, ENABLE);

    // ??ADC
    ADC_Cmd(ADC1, ENABLE);

    // ???????
    ADC_ResetCalibration(ADC1);
    // ???????????
    while(ADC_GetResetCalibrationStatus(ADC1));
    // ????
    ADC_StartCalibration(ADC1);
    // ??????
    while(ADC_GetCalibrationStatus(ADC1));

    DMA_InitTypeDef DMA_InitStructure;

    // ??DMA1??
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA??
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 2;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // ??DMA??
    DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

