#include "tim_encoder.h"

void TIM_encoder_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	uint16_t PrescalerValue = 0;

	/* Define InitTypeDef ---------------------------------------------------*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef 		GPIO_InitStructure;

	/* initialize InitTypeDef -----------------------------------------------*/
	/* Supply clock source --------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Set up TIM_encoder function --------------------------------------------------*/
	/* プリスケーラの値の設定 */
	PrescalerValue = (uint16_t) ((SystemCoreClock ) / 36000000) - 1;

	/* メンバの値の設定 */
	TIM_TimeBaseStructure.TIM_Period		= 0xffff-1;
	TIM_TimeBaseStructure.TIM_Prescaler		= 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* プリスケーラの設定 */
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	/* プリスケーラの値の設定 */
	PrescalerValue = (uint16_t) ((SystemCoreClock ) / 72000000) - 1;

	/* メンバの値の設定 */
	TIM_TimeBaseStructure.TIM_Period		= 0xffff-1;
	TIM_TimeBaseStructure.TIM_Prescaler		= 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* プリスケーラの設定 */
	TIM_PrescalerConfig(TIM1, PrescalerValue, TIM_PSCReloadMode_Immediate);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	/*更新割り込み設定*/
#ifdef USE_INTERRUPT_TIM1
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE );
#endif

#ifdef USE_INTERRUPT_TIM2
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
#endif

#ifdef USE_INTERRUPT_TIM3
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
#endif

	/* TIM enable counter */
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

}

