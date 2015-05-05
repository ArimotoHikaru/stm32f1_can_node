#include "tim_encoder.h"

void TIM_encoder_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	uint16_t PrescalerValue = 0;

	/* Define InitTypeDef ---------------------------------------------------*/
	GPIO_InitTypeDef 		GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* initialize InitTypeDef -----------------------------------------------*/
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	TIM_DeInit(TIM5);

	/* Supply clock source --------------------------------------------------*/
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |	RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5, ENABLE);
	//RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);/*TIM2_CH1_ETR→PA15*TIM2_CH2→PB3*TIM2_CH3→リマップしない*TIM2_CH4→リマップしない*/
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin 	= 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed 	= 	GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= 	GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*
	 * TIM2_CH1:PA15
	 * TIM2_CH2:PB3
	 * TIM3_CH1:PA6
	 * TIM3_CH2:PA7
	 * TIM4_CH1:PB6
	 * TIM4_CH2:PB7
	 * TIM5_CH1:PA0
	 * TIM5_CH2:PA1
	 */
//2453]


	/* Set up TIM_encoder function --------------------------------------------------*/
	//プリスケーラの値の設定
	PrescalerValue = (uint16_t) ((SystemCoreClock ) / 36000000) - 1;

	//メンバの値の設定
	TIM_TimeBaseStructure.TIM_Period		= 0xffff-1;
	TIM_TimeBaseStructure.TIM_Prescaler		= 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	//プリスケーラの設定
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM5, PrescalerValue, TIM_PSCReloadMode_Immediate);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	//更新割り込み設定
#ifdef USE_INTERRUPT_TIM2
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
#endif

#ifdef USE_INTERRUPT_TIM3
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
#endif

#ifdef USE_INTERRUPT_TIM4
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );
#endif

#ifdef USE_INTERRUPT_TIM5
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE );
#endif

	/* TIM enable counter */
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);

}

