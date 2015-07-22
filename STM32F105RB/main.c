#include <stdio.h>
#include "main.h"

extern uint32_t ticker, downticker;
extern uint32_t tx_interval[4];
extern int32_t	TIM2_over_flow, TIM2_under_flow,
				TIM3_over_flow, TIM3_under_flow,
				TIM4_over_flow, TIM4_under_flow,
				TIM5_over_flow, TIM5_under_flow;


//送信されるメッセージが格納される変数
CanTxMsg can_tx_flame[4];
CanTxMsg can_tx_test;
CanRxMsg can_rx_flame;

int can_tx_count = 0;
unsigned short interval_time[4] = {0};//エンコーダーの数だけ用意
int can_enc_mode[4] = {0};//エンコーダーの数だけ用意

#define CANID_TIM2_TX 0x440
#define CANID_TIM3_TX 0x443
#define CANID_TIM4_TX 0x441
#define CANID_TIM5_TX 0x442

#define CANID_TIM2_OPTION 0x400
#define CANID_TIM3_OPTION 0x403
#define CANID_TIM4_OPTION 0x401
#define CANID_TIM5_OPTION 0x402

#define CANID_TIM2_REQUEST 0x440
#define CANID_TIM3_REQUEST 0x443
#define CANID_TIM4_REQUEST 0x441
#define CANID_TIM5_REQUEST 0x442


void XXX_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	/* initialize InitTypeDef -----------------------------------------------*/
	/* Supply clock source --------------------------------------------------*/
	/* Define gpio_config ---------------------------------------------------*/
	/* Set up XXX_function --------------------------------------------------*/
}

void CAN2_TX_IRQHandler(void)
{

	if (CAN_GetITStatus(CAN2,CAN_IT_TME)){//メールボックスが空になったら呼び出される　何も送ってない状態では呼び出されない
		CAN_Transmit(CAN2, &can_tx_flame);//送信
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
	}

}

void CAN2_RX0_IRQHandler(void){

	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)){//新しいメッセージを受信したら呼び出される
		CAN_Receive(CAN2, CAN_FIFO0, &can_rx_flame);//受信
		CAN_Receive_Check(&can_rx_flame);
	}

}

void Encoder_ClearCount(TIM_TypeDef* TIMx)
{
	TIMx->CNT = 0;
}

int Encoder_Count(TIM_TypeDef* TIMx)
{

	int value = 0;

	if(TIMx == TIM2){
		value = ( TIM_GetCounter(TIM2) + TIM2_over_flow * 65536 - ( TIM2_under_flow * 65536 ) );
	}else if(TIMx == TIM3){
		value = ( TIM_GetCounter(TIM3) + TIM3_over_flow * 65536 - ( TIM3_under_flow * 65536 ) );
	}else if(TIMx == TIM4){
		value = ( TIM_GetCounter(TIM4) + TIM4_over_flow * 65536 - ( TIM4_under_flow * 65536 ) );
	}else if(TIMx == TIM5){
		value = ( TIM_GetCounter(TIM5) + TIM5_over_flow * 65536 - ( TIM5_under_flow * 65536 ) );
	}

	return value;

}

void Encoder_int_to_char(TIM_TypeDef* TIMx, int value)
{
	if(TIMx == TIM2){
		can_tx_flame[0].StdId = CANID_TIM2_TX;
		can_tx_flame[0].RTR	= 0;
		can_tx_flame[0].IDE	= 0;
		can_tx_flame[0].DLC	= 4;
		can_tx_flame[0].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[0].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[0].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[0].Data[3] = (char)((value & 0xFF000000) >> 24);
	}else if(TIMx == TIM3){
		can_tx_flame[1].StdId = CANID_TIM3_TX;
		can_tx_flame[1].RTR	= 0;
		can_tx_flame[1].IDE	= 0;
		can_tx_flame[1].DLC	= 4;
		can_tx_flame[1].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[1].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[1].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[1].Data[3] = (char)((value & 0xFF000000) >> 24);
	}else if(TIMx == TIM4){
		can_tx_flame[2].StdId = CANID_TIM4_TX;
		can_tx_flame[2].RTR	= 0;
		can_tx_flame[2].IDE	= 0;
		can_tx_flame[2].DLC	= 4;
		can_tx_flame[2].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[2].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[2].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[2].Data[3] = (char)((value & 0xFF000000) >> 24);
	}else if(TIMx == TIM5){
		can_tx_flame[3].StdId = CANID_TIM5_TX;
		can_tx_flame[3].RTR	= 0;
		can_tx_flame[3].IDE	= 0;
		can_tx_flame[3].DLC	= 4;
		can_tx_flame[3].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[3].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[3].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[3].Data[3] = (char)((value & 0xFF000000) >> 24);
	}
}

void Encoder_into_CANflame(TIM_TypeDef* TIMx)
{
	Encoder_int_to_char(TIMx,Encoder_Count(TIMx));
}

void CAN_Receive_Check (CanRxMsg* RxMessage)
{
	switch(RxMessage->StdId){

//エンコーダー値の要求
	//TIM2
	case CANID_TIM2_REQUEST://エンコーダー値の要求

		Encoder_into_CANflame(TIM2);
		CAN_Transmit(CAN2, &can_tx_flame[0]);//送信

		break;
	//TIM3
	case CANID_TIM3_REQUEST://エンコーダー値の要求

		Encoder_into_CANflame(TIM3);
		CAN_Transmit(CAN2, &can_tx_flame[1]);//送信

		break;
	//TIM4
	case CANID_TIM4_REQUEST://エンコーダー値の要求

		Encoder_into_CANflame(TIM4);
		CAN_Transmit(CAN2, &can_tx_flame[2]);//送信

		break;

	//TIM5
	case CANID_TIM5_REQUEST://エンコーダー値の要求

		Encoder_into_CANflame(TIM5);
		CAN_Transmit(CAN2, &can_tx_flame[3]);//送信

		break;

//送信オプション変更
	//TIM2
	case CANID_TIM2_OPTION://送信オプション変更

		if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
			TIM2_over_flow = 0;TIM2_under_flow = 0;
			Encoder_ClearCount(TIM2);

		}else if(RxMessage->Data[0] == 0x01){//自動送信設定
			interval_time[0] = (unsigned short)(((RxMessage->Data[2] << 8)&0xFF00) | ((RxMessage->Data[1])&0x00FF));
			if(interval_time[0] == 65535){
				can_enc_mode[0] = 0;//自動送信停止
			}else{
				can_enc_mode[0] = 1;//自動送信開始
				tx_interval[0] = 0;
			}
		}
		break;

	case CANID_TIM3_OPTION://送信オプション変更

		if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
			TIM3_over_flow = 0;TIM3_under_flow = 0;
			Encoder_ClearCount(TIM3);

		}else if(RxMessage->Data[0] == 0x01){//自動送信設定
			interval_time[1] = (unsigned short)(((RxMessage->Data[2] << 8)&0xFF00) | ((RxMessage->Data[1])&0x00FF));
			if(interval_time[1] == 65535){
				can_enc_mode[1] = 0;//自動送信停止
			}else{
				can_enc_mode[1] = 1;//自動送信開始
				tx_interval[1] = 0;
			}
		}
		break;

	case CANID_TIM4_OPTION://送信オプション変更

		if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
			TIM4_over_flow = 0;TIM4_under_flow = 0;
			Encoder_ClearCount(TIM4);

		}else if(RxMessage->Data[0] == 0x01){//自動送信設定
			interval_time[2] = (unsigned short)(((RxMessage->Data[2] << 8)&0xFF00) | ((RxMessage->Data[1])&0x00FF));
			if(interval_time[2] == 65535){
				can_enc_mode[2] = 0;//自動送信停止
			}else{
				can_enc_mode[2] = 1;//自動送信開始
				tx_interval[2] = 0;
			}
		}
		break;

	case CANID_TIM5_OPTION://送信オプション変更

		if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
			TIM5_over_flow = 0;TIM5_under_flow = 0;
			Encoder_ClearCount(TIM5);

		}else if(RxMessage->Data[0] == 0x01){//自動送信設定
			interval_time[3] = (unsigned short)(((RxMessage->Data[2] << 8)&0xFF00) | ((RxMessage->Data[1])&0x00FF));
			if(interval_time[3] == 65535){
				can_enc_mode[3] = 0;//自動送信停止
			}else{
				can_enc_mode[3] = 1;//自動送信開始
				tx_interval[3] = 0;
			}
		}
		break;


	default:
		break;
	}
}

void init(void)
{
	SystemInit();

	if (SysTick_Config(SystemCoreClock / 1000)){}

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
*/
#ifdef USE_ADC
	ADC_Configuration();
#endif

#ifdef USE_ENCODER
	TIM_encoder_Configuration();

	TIM2_over_flow = 0;TIM2_under_flow = 0;
	Encoder_ClearCount(TIM2);
	TIM3_over_flow = 0;TIM3_under_flow = 0;
	Encoder_ClearCount(TIM3);
	TIM4_over_flow = 0;TIM4_under_flow = 0;
	Encoder_ClearCount(TIM4);
	TIM5_over_flow = 0;TIM5_under_flow = 0;
	Encoder_ClearCount(TIM5);
#endif

#ifdef USE_UART4
	UART4_Configuration();
#endif

#ifdef USE_CAN
	CAN_Configuration();
#endif

	NVIC_Configuration();
}


int main(void)
{
	char str[150] = {0};
	char ledflag = 1;
	char TransmitMailbox,rxstatus;
	char datac = 0;

	RCC_ClocksTypeDef tmp;

	init();

	while(1){

		//TIM2
		switch(can_enc_mode[0]){

		case 0:

			break;
		case 1:
			if(tx_interval[0] >= interval_time[0]){
				tx_interval[0] = 0;
				Encoder_into_CANflame(TIM2);
				CAN_Transmit(CAN2, &can_tx_flame[0]);//送信
			}
			break;
		}

		//TIM3
		switch(can_enc_mode[1]){

		case 0:

			break;
		case 1:
			if(tx_interval[1] >= interval_time[1]){
				tx_interval[1] = 0;
				Encoder_into_CANflame(TIM3);
				CAN_Transmit(CAN2, &can_tx_flame[1]);//送信
			}
			break;
		}

		//TIM4
		switch(can_enc_mode[2]){

		case 0:

			break;
		case 1:
			if(tx_interval[2] >= interval_time[2]){
				tx_interval[2] = 0;
				Encoder_into_CANflame(TIM4);
				CAN_Transmit(CAN2, &can_tx_flame[2]);//送信
			}
			break;
		}

		//TIM5
		switch(can_enc_mode[3]){

		case 0:

			break;
		case 1:
			if(tx_interval[3] >= interval_time[3]){
				tx_interval[3] = 0;
				Encoder_into_CANflame(TIM5);
				CAN_Transmit(CAN2, &can_tx_flame[3]);//送信
			}
			break;
		}

		while(ticker > 500){
			ticker = 0;

			if(ledflag){
			ledflag = 0;
				GPIO_WriteBit(GPIOC,GPIO_Pin_13, 1);
			}else{
				ledflag = 1;
				GPIO_WriteBit(GPIOC,GPIO_Pin_13, 0);
			}


			//Encoder_into_CANflame(TIM2);
			//Encoder_into_CANflame(TIM3);
			//Encoder_into_CANflame(TIM4);
			//Encoder_into_CANflame(TIM5);
			//RCC_GetClocksFreq(&tmp);
//2453
			//tmp1 = (unsigned short)(((a[1] << 8)&0xFF00) | ((a[0])&0x00FF));

#ifdef USE_UART4
			//sprintf(str,"%d \r\n",RCC_GetSYSCLKSource());
			//sprintf(str,"test%4X %4X %4X %4X %4X\n\r", can_tx_test.StdId, can_tx_test.Data[0],can_tx_test.Data[1],can_tx_test.Data[2],can_tx_test.Data[3]);
			//sprintf(str,"ID:%X RTR:%X DLC:%X DATA:%X %X %X %X FMI:%X\n\r", can_rx_flame.StdId, can_rx_flame.RTR, can_rx_flame.DLC, can_rx_flame.Data[0], can_rx_flame.Data[1], can_rx_flame.Data[2], can_rx_flame.Data[3], can_rx_flame.FMI);
			//sprintf(str,"TIM1:%4X %4X %4X %4X\n\r", can_tx_flame[2].Data[0],can_tx_flame[2].Data[1],can_tx_flame[2].Data[2],can_tx_flame[2].Data[3]);
/*エンコーダー*/ //sprintf(str,"TIM2:%8d O:%3d U:%3d TIM3:%8d O:%3d U:%3d TIM4:%8d O:%3d U:%3d \n\r", Encoder_Count(TIM2), TIM2_over_flow, TIM2_under_flow, Encoder_Count(TIM3), TIM3_over_flow, TIM3_under_flow, Encoder_Count(TIM4), TIM4_over_flow, TIM4_under_flow);
/*TIM2*/	//sprintf(str,"TIM2:%8d %d O:%3d U:%3d \n\r", TIM_GetCounter(TIM2), Encoder_Count(TIM2), TIM2_over_flow, TIM2_under_flow);
/*TIM3*/	//sprintf(str,"TIM3:%8d %d O:%3d U:%3d \n\r", TIM_GetCounter(TIM3), Encoder_Count(TIM3), TIM3_over_flow, TIM3_under_flow);
/*TIM4*/	//sprintf(str,"TIM4:%8d %d O:%3d U:%3d \n\r", TIM_GetCounter(TIM4),  Encoder_Count(TIM4), TIM4_over_flow, TIM4_under_flow);
/*TIM5*/	//sprintf(str,"TIM5:%8d %8d O:%3d U:%3d \n\r", TIM_GetCounter(TIM5),Encoder_Count(TIM5), TIM5_over_flow, TIM5_under_flow);

/*AD*/		//sprintf(str,"0:%5d 1:%5d 2:%5d 3:%5d 4:%5d 5:%5d \n\r", GetAdc1Value_xch(0), GetAdc1Value_xch(1), GetAdc1Value_xch(2), GetAdc1Value_xch(3), GetAdc1Value_xch(4), GetAdc1Value_xch(5));
/*Clock*/	//sprintf(str,"SYSCLK:%d HCLK:%d PCLK1:%d PCLK2:%d ADCCLK:%d SystemCoreClock:%d \n\r", tmp.SYSCLK_Frequency, tmp.HCLK_Frequency, tmp.PCLK1_Frequency, tmp.PCLK2_Frequency, tmp.SYSCLK_Frequency, SystemCoreClock);
			transmit_uart4_s(str);
#endif
		}
	}

}
