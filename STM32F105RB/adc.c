#include "adc.h"

int16_t ADCConvertedValue[6];//これを読みだす

void ADC_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	/* initialize InitTypeDef -----------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	DMA_StructInit(&DMA_InitStructure);

	/* Supply clock source --------------------------------------------------*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1 |
							RCC_APB2Periph_GPIOA |
							RCC_APB2Periph_GPIOC, ENABLE);

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);



	/* Set up DMA_function --------------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr 		= (uint32_t)&ADCConvertedValue;
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize 			= 6;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;
	DMA_InitStructure.DMA_M2M 					= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Set up ADC_function --------------------------------------------------*/
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode 					= ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode 			= ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode 	= ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign 			= ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel 			= 6;
	ADC_Init(ADC1, &ADC_InitStructure);

	//ADC1のアナログ入力を定義する
	//ADC_Channel_11がPA1,ADC_Channel_12がPA2。マニュアル(UM1472)のピン配表を見るとわかる
	//"ADC123_IN11"という表記は、ADC1,ADC2,ADC3で使える入力でchannel_11に割り当てられる、ということ
	//使うチャネルを増やすには、ここに追記する。チャネル指定の次のパラメータは変換シーケンスの順序

	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  1, ADC_SampleTime_41Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  2, ADC_SampleTime_41Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_41Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_41Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_41Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_41Cycles5);
	//ADC1start
	ADC_DMACmd(ADC1, ENABLE);

	//ADC1start
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

uint16_t GetAdc1Value_xch(uint8_t ch)
{
	return ADCConvertedValue[ch];
}

