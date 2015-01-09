#include <stdio.h>
#include "main.h"

#define CAN_LIST_BUF 10

extern int ticker, downticker;
//送信されるメッセージが格納される変数
CanTxMsg can_tx_flame;
CanRxMsg can_rx_flame;

CanTxMsg can_tx_list[CAN_LIST_BUF];
CanRxMsg can_rx_list[CAN_LIST_BUF];


int can_tx_count = 0;


DualshockBotton psbutton = {
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},

};

void XXX_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	/* initialize InitTypeDef -----------------------------------------------*/
	/* Supply clock source --------------------------------------------------*/
	/* Define gpio_config ---------------------------------------------------*/
	/* Set up XXX_function --------------------------------------------------*/
}

void init(void)
{
	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//Setup SystickTimer
	if (SysTick_Config(SystemCoreClock / 1000)){ColorfulRingOfDeath();}

#ifdef USE_ADC
	ADC_Configuration();
#endif

#ifdef USE_ENCODER
	TIM_encoder_Configuration();
#endif

#ifdef USE_USART2
	USART2_Configuration();
#endif

#ifdef USE_CAN
	CAN_Configuration();
#endif

	NVIC_Configuration();
}

void CAN_Node_Check(CanRxMsg* RxMessage)
{
	uint8_t id = RxMessage->StdId & 0x1F;
	uint8_t i;

	switch(id){

	//switch(RxMessage->StdId){

	case CAN_NodeId_Dualshock3 :
		can_tx_count++;

		//CAN_Receive_Dualshock3(&RxMessage);
		for(i=0; i<RxMessage->DLC; i++){

				switch(RxMessage->Data[i]){	//i=key i+1=value //i+2=#

//				case  key:		psbutton.key.value		= RxMessage->Data[i+1];
//					break;
				case start: 	psbutton.start.value 	= RxMessage->Data[i+1];
					break;
				case select:	psbutton.select.value 	= RxMessage->Data[i+1];
					break;
				case left_x:	psbutton.left_x.value 	= RxMessage->Data[1];
					break;
				case left_y:	psbutton.left_y.value 	= RxMessage->Data[1];
					break;
				case right_x:	psbutton.right_x.value 	= RxMessage->Data[i+1];
					break;
				case right_y:	psbutton.right_y.value 	= RxMessage->Data[i+1];
					break;
				case L1:    	psbutton.L1.value 		= RxMessage->Data[i+1];
					break;
				case L2:		psbutton.L2.value 		= RxMessage->Data[i+1];
					break;
				case R1:		psbutton.R1.value 		= RxMessage->Data[i+1];
					break;
				case R2:		psbutton.R2.value 		= RxMessage->Data[i+1];
					break;
				case Triangle:	psbutton.Triangle.value = RxMessage->Data[i+1];
					break;
				case Circle:	psbutton.Circle.value 	= RxMessage->Data[i+1];
					break;
				case Cross:		psbutton.Cross.value 	= RxMessage->Data[i+1];
					break;
				case Square:	psbutton.Square.value 	= RxMessage->Data[i+1];
					break;
				case Up:		psbutton.Up.value 		= RxMessage->Data[i+1];
					break;
				case Right:		psbutton.Right.value 	= RxMessage->Data[i+1];
					break;
				case Down:		psbutton.Down.value 	= RxMessage->Data[i+1];
					break;
				case Left:		psbutton.Left.value 	= RxMessage->Data[i+1];
					break;
				default:
					break;
				}
				i +=2;
			}
		break;
	case CAN_NodeId_STM32F4_1:

		break;

	default:

		break;
	}
}

/* * * * * * *
 *  N E W !
 * * * * * * */
//入力0：送信するフレームのListを作る
//出力1：Listの次のフレームを変数に格納する
void CAN_Transmit_List_Stack(uint8_t io, CanTxMsg* TxMessage)
{
	static uint8_t index = 0;
	static uint8_t index_top = 0;

	if(io == 1){//出力の場合
		*TxMessage = can_tx_list[index];

		index++;

		if(index >= index_top){
			index = 0;
		}
	}else if(io == 0){//入力の場合
		can_tx_list[index_top] = *TxMessage;
		index_top++;
	}
}
/*
void CAN1_TX_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1,CAN_IT_TME)){//メールボックスが空になったら呼び出される　何も送ってない状態では呼び出されない
		GPIOD->BSRRL = GPIO_Pin_14;
		CAN_Transmit(CAN1, &can_tx_flame);//送信
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
	}
}
*/
//↑今までの割込み関数
//↓今回変更した割込み関数
void USB_HP_CAN1_TX_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1,CAN_IT_TME)){//メールボックスが空になったら呼び出される　何も送ってない状態では呼び出されない

		CAN_Transmit_List_Stack(1,&can_tx_flame);//can_tx_flameに次に送るデータを格納する
		CAN_Transmit(CAN1, &can_tx_flame);//送信
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
	}
}


void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)){//新しいメッセージを受信したら呼び出される
		CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信

		CAN_Node_Check(&can_rx_flame);
//		CAN_Receive_Dualshock3(&can_rx_flame);
	}
}
void CAN1_SCE_IRQHandler(void)//Status Change Error Interrupt
{

}

int main(void)
{
	char str[70] = {0};

	init();

	while(1){

		while(ticker > 300){
			ticker = 0;

			sprintf(str,"Hello!\r\n");
			transmit_uart2_s(str);
		}
	}

}


//#define CAN_TX

#ifdef CAN_TX
//送信
int main(void)
{
	char str[70] = {0};
	int a;

	init();

	can_tx_flame.StdId 		= 0x00A;//ID 11bit 0～0x7FF
	can_tx_flame.ExtId		= 0x0;	//拡張フレームID 29bit 0～0x1FFFFFFF
	can_tx_flame.IDE 		= 0;	//拡張フレームIDを使う場合1
	can_tx_flame.RTR		= 0;	//データフレーム:0 リモートフレーム:1
	can_tx_flame.DLC		= 3;	//送信するデータフィールドのバイト数 0~8
	can_tx_flame.Data[0]	= 0xAA;	//送信するデータフィールド
	can_tx_flame.Data[1]	= 0xAB;
	can_tx_flame.Data[2]	= 0xAC;


	CAN_Transmit_List_Stack(0, &can_tx_flame);//0x00AというIDのフレームをリストに追加
	can_tx_flame.StdId 		= 0x00B;
	CAN_Transmit_List_Stack(0, &can_tx_flame);//0x00BというIDのフレームをリストに追加
	can_tx_flame.StdId 		= 0x00C;
	CAN_Transmit_List_Stack(0, &can_tx_flame);//0x00CというIDのフレームをリストに追加
	can_tx_flame.StdId 		= 0x00D;
	CAN_Transmit_List_Stack(0, &can_tx_flame);//0x00DというIDのフレームをリストに追加

	//以降割込みによって0x00Aから0x00Dのフレームが繰り返し送信される


	while(ticker < 1000);

	//CAN_Transmit(CAN1, &can_tx_flame);//送信

	GPIOD->BSRRL = GPIO_Pin_12;

	//while(CANTXOK != CAN_TransmitStatus(CAN1,0));

	//GPIOD->BSRRL = GPIO_Pin_13;
/*
	while(0 == CAN_MessagePending(CAN1, CAN_FIFO0));

	GPIOD->BSRRL = GPIO_Pin_14;

	CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信

	GPIOD->BSRRL = GPIO_Pin_15;
*/
	CAN_Transmit(CAN1, &can_tx_flame);//送信

    while(1)
    {

    	while(ticker > 300){
    		ticker = 0;

    		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == SET){
/*
    			can_tx_flame.Data[0]	= can_tx_flame.Data[0] + 1;
    			if(can_tx_flame.Data[0]>0xFF){
    				can_tx_flame.Data[0] = 0x00;
    			}
*/
    			GPIOD->BSRRH = GPIO_Pin_13;
#ifndef USE_INTERRUPT_CAN_TX
    			CAN_Transmit(CAN1, &can_tx_flame);//送信

    			while(CANTXOK != CAN_TransmitStatus(CAN1,0));
#endif
    		}else{
    			GPIOD->BSRRL = GPIO_Pin_13;
    		}

    		sprintf(str,"StId:0x%X \n\rIDE:%d \n\rRTR:%d \n\rDLC:%X \n\rData[0]:0x%X \n\rRerror:%d \n\rTerror:%d\n\r\n"
    											,can_tx_flame.StdId
    											,can_tx_flame.IDE
    											,can_tx_flame.RTR
    											,can_tx_flame.DLC
    											,can_tx_flame.Data[0]
    											//,can_tx_flame.FMI
    											,CAN_GetReceiveErrorCounter(CAN1)
    											,CAN_GetLSBTransmitErrorCounter(CAN1));

    		transmit_uart3_s(str);

    	}
    }
}

//#else
//受信
int main(void)
{
	char str[150] = {0};

	init();

	GPIOD->BSRRL = GPIO_Pin_12;

/*
	can_tx_flame.StdId 		= 0x00F;//ID 11bit 0～0x7FF
	can_tx_flame.ExtId		= 0x0;	//拡張フレームID 28bit 0～0x1FFFFFFF
	can_tx_flame.IDE 		= 0;	//拡張フレームIDを使う場合1
	can_tx_flame.RTR		= 0;	//データフレーム:0 リモートフレーム:1
	can_tx_flame.DLC		= 1;	//送信するデータフィールドのバイト数
	can_tx_flame.Data[0]	= 0xCC;	//送信するデータフィールド

	ticker = 0;
	while(ticker<1000);

	CAN_Transmit(CAN1, &can_tx_flame);//送信

	GPIOD->BSRRL = GPIO_Pin_12;

	while(CANTXOK != CAN_TransmitStatus(CAN1,0));

	GPIOD->BSRRL = GPIO_Pin_13;

	while(0 == CAN_MessagePending(CAN1, CAN_FIFO0));

	GPIOD->BSRRL = GPIO_Pin_14;

	CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信

	GPIOD->BSRRL = GPIO_Pin_15;
	*/



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
