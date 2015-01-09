#include "usart.h"

#define USART2_BAUDRATE 19200

#define USART2_TxBufferSize  200//リングバッファの要素数
#define USART2_RxBufferSize  200//リングバッファの要素数

char USART2_TxBuffer[USART2_TxBufferSize];
char USART2_RxBuffer[USART2_RxBufferSize];
int  USART2_TxPtrNow = 0,
     USART2_TxPtrEnd = 1;
int  USART2_RxPtrNow = 0,
     USART2_RxPtrEnd = 1;

void USART2_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;

	/* initialize InitTypeDef -----------------------------------------------*/

	/* Supply clock source --------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin  	= GPIO_Pin_2;//TX
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  	= GPIO_Pin_3;//RX
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Set up USART2_function --------------------------------------------------*/
	USART_InitStructure.USART_BaudRate 				= USART2_BAUDRATE;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

#ifdef USE_INTERRUPT_USART2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#endif

	USART_Cmd(USART2, ENABLE);

}

//一文字送信
void transmit_uart2_c(char c)
{

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);  //送信し終わった割り込みを一時的に禁止
#endif

  USART2_TxBuffer[USART2_TxPtrEnd] = c;    //リングバッファに１文字追加
  USART2_TxPtrEnd++;    //書き込みポインタを＋１

  if(USART2_TxPtrEnd > (USART2_TxBufferSize-1)) USART2_TxPtrNow=0; USART2_TxPtrEnd = 1;    //書き込みポインタがオーバーフローしたら０に戻す

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  // 送信し終わった割り込みを許可
#endif

}
//文字列送信
void transmit_uart2_s(char *s)
{

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE); 
#endif

  while(*s!=0) // until end of string   文字列の末尾までループ
  {
    USART2_TxBuffer[USART2_TxPtrEnd] = *(s++);
    USART2_TxPtrEnd++; //  ポインタ＋１

    if(USART2_TxPtrEnd > (USART2_TxBufferSize-1)) USART2_TxPtrEnd = 0;
  }

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);   //送信し終わった割り込みを許可
#endif

}

void USART2_IRQHandler(void)
{
  
	char c;
  
	//受信割り込み
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		c = (char)USART_ReceiveData(USART2);
		transmit_uart2_c(c);
	}
  
	//送信割り込み
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET){  // UART送信フラグチェック
		USART_SendData(USART2, USART2_TxBuffer[USART2_TxPtrNow++]); // １文字送信

		if(USART2_TxPtrNow > (USART2_TxBufferSize-1)) USART2_TxPtrNow=0;//   ポインタオーバーフローならゼロに戻す

		if(USART2_TxPtrNow == USART2_TxPtrEnd){     //リングバッファが空か？
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE); //送信割り込みをオフ
		}
	}
}
