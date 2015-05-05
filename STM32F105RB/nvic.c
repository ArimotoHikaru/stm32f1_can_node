#include "nvic.h"

void NVIC_Configuration (void)
{
	/* Define InitTypeDef ---------------------------------------------------*/
	NVIC_InitTypeDef 	NVIC_InitStructure;

	/* Set up NVIC function --------------------------------------------------*/

#ifdef USE_INTERRUPT_UART4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel 						= UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_TIM2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_TIM3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_TIM4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_TIM5
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_CAN_RX
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_INTERRUPT_CAN_TX
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel 						= CAN2_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#endif
}
