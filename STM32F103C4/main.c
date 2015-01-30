#include <stdio.h>
#include "main.h"

#define CAN_LIST_BUF 10

extern uint32_t ticker, downticker;
extern int32_t	TIM1_over_flow, TIM1_under_flow,
				TIM2_over_flow, TIM2_under_flow,
				TIM3_over_flow, TIM3_under_flow;

//送信されるメッセージが格納される変数
CanTxMsg can_tx_flame;
CanRxMsg can_rx_flame;

CanTxMsg can_tx_list[CAN_LIST_BUF];
CanRxMsg can_rx_list[CAN_LIST_BUF];


int can_tx_count = 0;

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
#endif

#ifdef USE_USART2
	USART2_Configuration();
#endif

#ifdef USE_CAN
	CAN_Configuration();
#endif

	NVIC_Configuration();
}
#if 0
int main(void)
{
	char str[100] = {0};
	char ledflag = 1;
	init();

	while(1){

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

		while(ticker > 500){
			ticker = 0;

#ifdef USE_USART2
/*エンコーダー*/ //sprintf(str,"TIM1:%7d O:%3d U:%3d TIM2:%7d O:%3d U:%3d TIM3:%7d O:%3d U:%3d \n\r", TIM_GetCounter(TIM1), TIM1_over_flow, TIM1_under_flow, TIM_GetCounter(TIM2), TIM2_over_flow, TIM2_under_flow, TIM_GetCounter(TIM3), TIM3_over_flow, TIM3_under_flow);
			sprintf(str,"0:%5d 1:%5d 2:%5d 3:%5d 4:%5d 5:%5d \n\r", GetAdc1Value_xch(0), GetAdc1Value_xch(1), GetAdc1Value_xch(2), GetAdc1Value_xch(3), GetAdc1Value_xch(4), GetAdc1Value_xch(5));
			transmit_uart2_s(str);
#endif
		}
	}

}
#else



void USB_HP_CAN1_TX_IRQHandler(void)
{

	if (CAN_GetITStatus(CAN1,CAN_IT_TME)){//メールボックスが空になったら呼び出される　何も送ってない状態では呼び出されない
		can_tx_flame.Data[0]++;	//送信するデータフィールド
		if(can_tx_flame.Data[0]>0xff){
			can_tx_flame.Data[0]=0x00;
		}
		CAN_Transmit(CAN1, &can_tx_flame);//送信
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
	}

}
void USB_LP_CAN1_RX0_IRQHandler(void){
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)){//新しいメッセージを受信したら呼び出される
		CAN_Receive(CAN1, CAN_FIFO0, &can_rx_flame);//受信
	}

}


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
