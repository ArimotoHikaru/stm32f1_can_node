#include "usart.h"

#define UART4_BAUDRATE 115200

#define UART4_TxBufferSize  200//リングバッファの要素数
#define UART4_RxBufferSize  200//リングバッファの要素数

char UART4_TxBuffer[UART4_TxBufferSize];
char UART4_RxBuffer[UART4_RxBufferSize];
int  UART4_TxPtrNow = 0,
     UART4_TxPtrEnd = 1;
int  UART4_RxPtrNow = 0,
     UART4_RxPtrEnd = 1;

void UART4_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	/* Define InitTypeDef ---------------------------------------------------*/
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;

	/* initialize InitTypeDef -----------------------------------------------*/

	/* Supply clock source --------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin  	= GPIO_Pin_10;//TX
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  	= GPIO_Pin_11;//RX
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Set up UART4_function --------------------------------------------------*/
	USART_InitStructure.USART_BaudRate 				= UART4_BAUDRATE;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 					= USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(UART4, &USART_InitStructure);

#ifdef USE_INTERRUPT_UART4
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
#endif

	USART_Cmd(UART4, ENABLE);

}

//一文字送信
void transmit_uart4_c(char c)
{

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, DISABLE);  //送信し終わった割り込みを一時的に禁止
#endif

  UART4_TxBuffer[UART4_TxPtrEnd] = c;    //リングバッファに１文字追加
  UART4_TxPtrEnd++;    //書き込みポインタを＋１

  if(UART4_TxPtrEnd > (UART4_TxBufferSize-1)) UART4_TxPtrNow=0; UART4_TxPtrEnd = 1;    //書き込みポインタがオーバーフローしたら０に戻す

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);  // 送信し終わった割り込みを許可
#endif

}
//文字列送信
void transmit_uart4_s(char *s)
{

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
#endif

  while(*s!=0) // until end of string   文字列の末尾までループ
  {
    UART4_TxBuffer[UART4_TxPtrEnd] = *(s++);
    UART4_TxPtrEnd++; //  ポインタ＋１

    if(UART4_TxPtrEnd > (UART4_TxBufferSize-1)) UART4_TxPtrEnd = 0;
  }

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);   //送信し終わった割り込みを許可
#endif

}

void UART4_IRQHandler(void)
{
	char c;
  
	//受信割り込み
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		c = (char)USART_ReceiveData(UART4);
		transmit_uart4_c(c);
	}
  
	//送信割り込み
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET){  // UART送信フラグチェック
		USART_SendData(UART4, UART4_TxBuffer[UART4_TxPtrNow++]); // １文字送信

		if(UART4_TxPtrNow > (UART4_TxBufferSize-1)) UART4_TxPtrNow=0;//ポインタオーバーフローならゼロに戻す

		if(UART4_TxPtrNow == UART4_TxPtrEnd){     //リングバッファが空か？
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE); //送信割り込みをオフ
		}
	}
}
