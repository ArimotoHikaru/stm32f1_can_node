#include <stdio.h>
#include "main.h"

extern uint32_t ticker, downticker;
extern uint32_t tx_interval[3];
extern int32_t	TIM1_over_flow, TIM1_under_flow,
				TIM2_over_flow, TIM2_under_flow,
				TIM3_over_flow, TIM3_under_flow;

//送信されるメッセージが格納される変数
CanTxMsg can_tx_flame[3];
CanRxMsg can_rx_flame;

int can_tx_count = 0;
unsigned short interval_time[3] = {0};//エンコーダーの数だけ用意
int can_enc_mode[3] = {0};//エンコーダーの数だけ用意


void XXX_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	/* initialize InitTypeDef -----------------------------------------------*/
	/* Supply clock source --------------------------------------------------*/
	/* Define gpio_config ---------------------------------------------------*/
	/* Set up XXX_function --------------------------------------------------*/
}

void USB_HP_CAN1_TX_IRQHandler(void)
{

	if (CAN_GetITStatus(CAN1,CAN_IT_TME)){//メールボックスが空になったら呼び出される　何も送ってない状態では呼び出されない
		CAN_Transmit(CAN1, &can_tx_flame);//送信
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
	}

}
void USB_LP_CAN1_RX0_IRQHandler(void){

	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)){//新しいメッセージを受信したら呼び出される
		CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信
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

	if(TIMx == TIM1){
		value = ( TIM_GetCounter(TIM1) + TIM1_over_flow * 65536 - ( TIM1_under_flow * 65536 ) );
	}else if(TIMx == TIM2){
		value = ( TIM_GetCounter(TIM2) + TIM2_over_flow * 65536 - ( TIM2_under_flow * 65536 ) );
	}else if(TIMx == TIM3){
		value = ( TIM_GetCounter(TIM3) + TIM3_over_flow * 65536 - ( TIM3_under_flow * 65536 ) );
	}

	return value;

}
#if 0
void can_txflame_configure(uint32_t stid, uint32_t exid, uint8_t ide, uint8_t rtr, uint8_t dlc)
{

}
#endif
void Encoder_int_to_char(TIM_TypeDef* TIMx, int value)
{
	if(TIMx == TIM1){
		can_tx_flame[0].StdId = 0x440;
		can_tx_flame[0].RTR	= 0;
		can_tx_flame[0].IDE	= 0;
		can_tx_flame[0].DLC	= 4;
		can_tx_flame[0].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[0].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[0].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[0].Data[3] = (char)((value & 0xFF000000) >> 24);
	}else if(TIMx == TIM2){
		can_tx_flame[1].StdId = 0x441;
		can_tx_flame[1].RTR	= 0;
		can_tx_flame[1].IDE	= 0;
		can_tx_flame[1].DLC	= 4;
		can_tx_flame[1].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[1].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[1].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[1].Data[3] = (char)((value & 0xFF000000) >> 24);
	}else if(TIMx == TIM3){
		can_tx_flame[2].StdId = 0x442;
		can_tx_flame[2].RTR	= 0;
		can_tx_flame[2].IDE	= 0;
		can_tx_flame[2].DLC	= 4;
		can_tx_flame[2].Data[0] = (char)(value & 0x000000FF);
		can_tx_flame[2].Data[1] = (char)((value & 0x0000FF00) >> 8);
		can_tx_flame[2].Data[2] = (char)((value & 0x00FF0000) >> 16);
		can_tx_flame[2].Data[3] = (char)((value & 0xFF000000) >> 24);
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
	//TIM1
	case 0x440://エンコーダー値の要求

		Encoder_into_CANflame(TIM1);
		CAN_Transmit(CAN1, &can_tx_flame[0]);//送信

		break;

	//TIM2
	case 0x441://エンコーダー値の要求

		Encoder_into_CANflame(TIM2);
		CAN_Transmit(CAN1, &can_tx_flame[1]);//送信

		break;

	//TIM3
	case 0x442://エンコーダー値の要求

		Encoder_into_CANflame(TIM3);
		CAN_Transmit(CAN1, &can_tx_flame[2]);//送信

		break;

//送信オプション変更
	//TIM1
	case 0x400://送信オプション変更

		if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
			TIM1_over_flow = 0;TIM1_under_flow = 0;
			Encoder_ClearCount(TIM1);

		}else if(RxMessage->Data[0] == 0x01){//自動送信設定
			interval_time[0] = (unsigned short)(((RxMessage->Data[1] << 8)&0xFF00) | ((RxMessage->Data[0])&0x00FF));
			if(interval_time[0] == 65535){
				can_enc_mode[0] = 0;//自動送信停止
			}else{
				can_enc_mode[0] = 1;//自動送信開始
				tx_interval[0] = 0;
			}
		}
		break;

		//TIM2
		case 0x401://送信オプション変更

			if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
				TIM2_over_flow = 0;TIM2_under_flow = 0;
				Encoder_ClearCount(TIM2);

			}else if(RxMessage->Data[0] == 0x01){//自動送信設定
				interval_time[1] = (unsigned short)(((RxMessage->Data[1] << 8)&0xFF00) | ((RxMessage->Data[0])&0x00FF));
				if(interval_time[1] == 65535){
					can_enc_mode[1] = 0;//自動送信停止
				}else{
					can_enc_mode[1] = 1;//自動送信開始
					tx_interval[1] = 0;
				}
			}
			break;

			//TIM3
			case 0x402://送信オプション変更

				if(RxMessage->Data[0] == 0x00){//エンコーダー値リセット
					TIM3_over_flow = 0;TIM3_under_flow = 0;
					Encoder_ClearCount(TIM3);

				}else if(RxMessage->Data[0] == 0x01){//自動送信設定
					interval_time[2] = (unsigned short)(((RxMessage->Data[1] << 8)&0xFF00) | ((RxMessage->Data[0])&0x00FF));
					if(interval_time[2] == 65535){
						can_enc_mode[2] = 0;//自動送信停止
					}else{
						can_enc_mode[2] = 1;//自動送信開始
						tx_interval[2] = 0;
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef USE_ADC
	ADC_Configuration();
#endif

#ifdef USE_ENCODER
	TIM_encoder_Configuration();
	TIM1_over_flow = 0;TIM1_under_flow = 0;
	Encoder_ClearCount(TIM1);
	TIM2_over_flow = 0;TIM2_under_flow = 0;
	Encoder_ClearCount(TIM2);
	TIM3_over_flow = 0;TIM3_under_flow = 0;
	Encoder_ClearCount(TIM3);
#endif

#ifdef USE_USART2
	USART2_Configuration();
#endif

#ifdef USE_CAN
	CAN_Configuration();
#endif

	NVIC_Configuration();
}


#if 1
int main(void)
{
	char str[130] = {0};
	char ledflag = 1;

	RCC_ClocksTypeDef tmp;

	init();

	while(1){

		//TIM1
		switch(can_enc_mode[0]){

		case 0:

			break;
		case 1:
			if(tx_interval[0] >= interval_time[0]){
				tx_interval[0] = 0;
				Encoder_into_CANflame(TIM1);
				CAN_Transmit(CAN1, &can_tx_flame[0]);//送信
			}
			break;
		}

		//TIM2
		switch(can_enc_mode[1]){

		case 0:

			break;
		case 1:
			if(tx_interval[1] >= interval_time[1]){
				tx_interval[1] = 0;
				Encoder_into_CANflame(TIM2);
				CAN_Transmit(CAN1, &can_tx_flame[1]);//送信
			}
			break;
		}

		//TIM3
		switch(can_enc_mode[2]){

		case 0:

			break;
		case 1:
			if(tx_interval[2] >= interval_time[2]){
				tx_interval[2] = 0;
				Encoder_into_CANflame(TIM3);
				CAN_Transmit(CAN1, &can_tx_flame[2]);//送信
			}
			break;
		}


		if(downticker == 0 ){
			downticker = 500;
			if(ledflag){
				ledflag = 0;
				GPIO_WriteBit(GPIOA,GPIO_Pin_10, 1);
			}else{
				ledflag = 1;
				GPIO_WriteBit(GPIOA,GPIO_Pin_10, 0);
			}
		}

		while(ticker > 200){
			ticker = 0;
			Encoder_into_CANflame(TIM1);
			Encoder_into_CANflame(TIM2);
			Encoder_into_CANflame(TIM3);

			RCC_GetClocksFreq(&tmp);

#ifdef USE_USART2
			sprintf(str,"TIM1:%4X %4X %4X %4X\n\r", can_tx_flame[2].Data[0],can_tx_flame[2].Data[1],can_tx_flame[2].Data[2],can_tx_flame[2].Data[3]);
/*エンコーダー*/ //sprintf(str,"TIM1:%8d O:%3d U:%3d TIM2:%8d O:%3d U:%3d TIM3:%8d O:%3d U:%3d \n\r", Encoder_Count(TIM1), TIM1_over_flow, TIM1_under_flow, Encoder_Count(TIM2), TIM2_over_flow, TIM2_under_flow, Encoder_Count(TIM3), TIM3_over_flow, TIM3_under_flow);
/*AD*/		//sprintf(str,"0:%5d 1:%5d 2:%5d 3:%5d 4:%5d 5:%5d \n\r", GetAdc1Value_xch(0), GetAdc1Value_xch(1), GetAdc1Value_xch(2), GetAdc1Value_xch(3), GetAdc1Value_xch(4), GetAdc1Value_xch(5));
/*Clock*/	//sprintf(str,"SYSCLK:%d HCLK:%X PCLK1:%X PCLK2:%X ADCCLK:%X SystemCoreClock:%X\n\r", tmp.SYSCLK_Frequency, tmp.HCLK_Frequency, tmp.PCLK1_Frequency, tmp.PCLK2_Frequency, tmp.SYSCLK_Frequency, SystemCoreClock);
			transmit_uart2_s(str);
#endif
		}
	}

}
#else






#define CAN_TX

#ifdef CAN_TX
//送信
int main(void)
{
	char str[200] = {0};
	char ledflag = 1;
	char cantxstatus;


	init();

	can_tx_flame.StdId 		= 0x001;//ID 11bit 0～0x7FF
	can_tx_flame.ExtId		= 0x0;	//拡張フレームID 29bit 0～0x1FFFFFFF
	can_tx_flame.IDE 		= 0;	//拡張フレームIDを使う場合1
	can_tx_flame.RTR		= 0;	//データフレーム:0 リモートフレーム:1
	can_tx_flame.DLC		= 3;	//送信するデータフィールドのバイト数 0~8
	can_tx_flame.Data[0]	= 0x0D;	//送信するデータフィールド
	can_tx_flame.Data[1]	= 0x0E;
	can_tx_flame.Data[2]	= 0x0F;

	sprintf(str,"CAN WAKE UP!\r\n");
	transmit_uart2_s(str);

	ticker = 0;
	while(ticker < 300);

	sprintf(str,"CAN TRANSMITTED!\r\n");
	transmit_uart2_s(str);
	CAN_Transmit(CAN1, &can_tx_flame);//送信

	ticker = 0;
	while(ticker < 100);


#if 0
	cantxstatus = CAN_TransmitStatus(CAN1,0);
	while(CANTXOK != cantxstatus){

		if(cantxstatus == CANTXFAILE){
			sprintf(str,"FAILED\r\n");
			transmit_uart2_s(str);
			ticker = 0;
			while(ticker < 100);
		}else if(cantxstatus == CANTXPENDING){

			sprintf(str,"CANTXPENDING %X\r\n", cantxstatus);
			transmit_uart2_s(str);

			ticker = 0;
			while(ticker < 100);
		}
		cantxstatus = CAN_TransmitStatus(CAN1,0);
	}
	sprintf(str,"OK!\r\n");
	transmit_uart2_s(str);

	ticker = 0;
	while(ticker < 100);

	sprintf(str,"RECEIVE...\r\n");
	transmit_uart2_s(str);

	while(CAN_MessagePending(CAN1, CAN_FIFO0) == 0){
		sprintf(str,"message_pending:%d\r\n",CAN_MessagePending(CAN1, CAN_FIFO0));
		transmit_uart2_s(str);

	}

	sprintf(str,"OK!\r\n");
	transmit_uart2_s(str);

	CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信

	ticker = 0;
	while(ticker < 100);

	sprintf(str,"StId:0x%X \n\rIDE:%d \n\rRTR:%d \n\rDLC:%X \n\rData[0]:0x%X \n\rData[1]:0x%X \n\rData[2]:0x%X \n\rRerror:%d \n\rTerror:%d \n\r\n\r"
										,can_rx_flame.StdId
										,can_rx_flame.IDE
										,can_rx_flame.RTR
										,can_rx_flame.DLC
										,can_rx_flame.Data[0]
										,can_rx_flame.Data[1]
										,can_rx_flame.Data[2]
										,CAN_GetReceiveErrorCounter(CAN1)
										,CAN_GetLSBTransmitErrorCounter(CAN1)
										);


	transmit_uart2_s(str);


	sprintf(str,"END\r\n");
	transmit_uart2_s(str);




	while(1);

#endif





    while(1)
    {

    	if(downticker == 0 ){
			downticker = 1000;
			if(ledflag){
				ledflag = 0;

				//GPIO_WriteBit(GPIOA,GPIO_Pin_10, 0);
#ifndef USE_INTERRUPT_CAN_TX
				can_tx_flame.Data[0]++;	//送信するデータフィールド
						if(can_tx_flame.Data[0]>0xff){
							can_tx_flame.Data[0]=0x00;
						}
				CAN_Transmit(CAN1, &can_tx_flame);//送信
				while(CANTXOK != CAN_TransmitStatus(CAN1,0));
				sprintf(str,"CAN TRANSMITTED!\r\n");
				transmit_uart2_s(str);

#endif

			}else{
				ledflag = 1;
				//GPIO_WriteBit(GPIOA,GPIO_Pin_10, 1 );
			}
		}

    	while(ticker >= 500){
    		ticker = 0;
/*
    		sprintf(str,"StId:0x%X \n\rIDE:%d \n\rRTR:%d \n\rDLC:%X \n\rData[0]:0x%X \n\rRerror:%d \n\rTerror:%d \n\r"
    											,can_tx_flame.StdId
    											,can_tx_flame.IDE
    											,can_tx_flame.RTR
    											,can_tx_flame.DLC
    											,can_tx_flame.Data[0]
    											,CAN_GetReceiveErrorCounter(CAN1)
    											,CAN_GetLSBTransmitErrorCounter(CAN1)
    											);
    		*/


    		sprintf(str,"StId:0x%X \n\rRTR:%d \n\rDLC:%X \n\rData[0]:0x%X \n\rData[1]:0x%X \n\rData[2]:0x%X \n\rRerror:%d \n\rTerror:%d \n\rESR:%X\n\r\n\r"
    											,can_rx_flame.StdId
    										//	,can_rx_flame.IDE
    											,can_rx_flame.RTR
    											,can_rx_flame.DLC
    											,can_rx_flame.Data[0]
    											,can_rx_flame.Data[1]
    											,can_rx_flame.Data[2]
    											,CAN_GetReceiveErrorCounter(CAN1)
    											,CAN_GetLSBTransmitErrorCounter(CAN1)
    											,CAN1 ->ESR
    											);



    		transmit_uart2_s(str);

    	}
    }
}

#else
//受信
int main(void)
{
	char str[150] = {0};

	init();

	GPIOD->BSRRL = GPIO_Pin_12;

	sprintf(str,"Start CAN!\n\r");
	transmit_uart3_s(str);

    while(1)
    {

#ifndef USE_INTERRUPT_CAN_RX
    	GPIOD->BSRRL = GPIO_Pin_13;
    	GPIOD->BSRRH = GPIO_Pin_14;

    	while(0 == CAN_MessagePending(CAN1, CAN_FIFO0));

    	CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信

    	GPIOD->BSRRL = GPIO_Pin_14;
    	GPIOD->BSRRH = GPIO_Pin_13;
#else

    	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == SET){
			GPIOD->BSRRH = GPIO_Pin_13;
		}else{
			GPIOD->BSRRL = GPIO_Pin_13;
    	}

#endif
    	while(ticker > 300){
    		ticker = 0;

    		sprintf(str,"StId:0x%X \n\rIDE:0x%X \n\rRTR:%d \n\rDLC:%X \n\rData[0]:0x%X \n\rData[1]:0x%X \n\rData[2]:0x%X \n\rData[3]:0x%X \n\rData[4]:0x%X \n\rData[5]:0x%X \n\rData[6]:0x%X \n\rData[7]:0x%X \n\rFMI:%d \n\rRerror:%d \n\rTerror:%d\n\r\n"
    											,can_rx_flame.StdId
    											,can_rx_flame.IDE
    											,can_rx_flame.RTR
    											,can_rx_flame.DLC
    											,can_rx_flame.Data[0]
    											,can_rx_flame.Data[1]
    											,can_rx_flame.Data[2]
    											,can_rx_flame.Data[3]
    											,can_rx_flame.Data[4]
    											,can_rx_flame.Data[5]
    											,can_rx_flame.Data[6]
    											,can_rx_flame.Data[7]
    											,can_rx_flame.FMI
    											,CAN_GetReceiveErrorCounter(CAN1)
    											,CAN_GetLSBTransmitErrorCounter(CAN1));


/*
    		sprintf(str," start:%X\n\r select:%X\n\r left_x:%X\n\r left_y:%X\n\r right_x:%X\n\r right_y:%X\n\r L1:%X\n\r L2:%X\n\r R1:%X\n\r R2:%X\n\r Tri:%X\n\r Cir:%X\n\r Cro:%X\n\r Squ:%X\n\r Up:%X\n\r Right:%X\n\r Dow:%X\n\r left:%X\n\r CAN_C:%d\n\r"
    				,psbutton.start.value
    				,psbutton.select.value
    				,psbutton.left_x.value
    				,psbutton.left_y.value
    				,psbutton.right_x.value
    				,psbutton.right_y.value
    				,psbutton.L1.value
    				,psbutton.L2.value
    				,psbutton.R1.value
    				,psbutton.R2.value
    				,psbutton.Triangle.value
    				,psbutton.Circle.value
    				,psbutton.Cross.value
    				,psbutton.Square.value
    				,psbutton.Up.value
    				,psbutton.Down.value
    				,psbutton.Right.value
    				,psbutton.Left.value
    				,can_tx_count);
*/
    		transmit_uart3_s(str);

    		//VCP_send_str(str);
    	}

    }
}
#endif
#endif
