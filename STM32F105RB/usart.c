#include "usart.h"

#define UART4_BAUDRATE 115200

#define UART4_TxBufferSize  200//�����O�o�b�t�@�̗v�f��
#define UART4_RxBufferSize  200//�����O�o�b�t�@�̗v�f��

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

//�ꕶ�����M
void transmit_uart4_c(char c)
{

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, DISABLE);  //���M���I��������荞�݂��ꎞ�I�ɋ֎~
#endif

  UART4_TxBuffer[UART4_TxPtrEnd] = c;    //�����O�o�b�t�@�ɂP�����ǉ�
  UART4_TxPtrEnd++;    //�������݃|�C���^���{�P

  if(UART4_TxPtrEnd > (UART4_TxBufferSize-1)) UART4_TxPtrNow=0; UART4_TxPtrEnd = 1;    //�������݃|�C���^���I�[�o�[�t���[������O�ɖ߂�

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);  // ���M���I��������荞�݂�����
#endif

}
//�����񑗐M
void transmit_uart4_s(char *s)
{

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
#endif

  while(*s!=0) // until end of string   ������̖����܂Ń��[�v
  {
    UART4_TxBuffer[UART4_TxPtrEnd] = *(s++);
    UART4_TxPtrEnd++; //  �|�C���^�{�P

    if(UART4_TxPtrEnd > (UART4_TxBufferSize-1)) UART4_TxPtrEnd = 0;
  }

#ifdef USE_INTERRUPT_UART4
  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);   //���M���I��������荞�݂�����
#endif

}

void UART4_IRQHandler(void)
{
	char c;
  
	//��M���荞��
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		c = (char)USART_ReceiveData(UART4);
		transmit_uart4_c(c);
	}
  
	//���M���荞��
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET){  // UART���M�t���O�`�F�b�N
		USART_SendData(UART4, UART4_TxBuffer[UART4_TxPtrNow++]); // �P�������M

		if(UART4_TxPtrNow > (UART4_TxBufferSize-1)) UART4_TxPtrNow=0;//�|�C���^�I�[�o�[�t���[�Ȃ�[���ɖ߂�

		if(UART4_TxPtrNow == UART4_TxPtrEnd){     //�����O�o�b�t�@���󂩁H
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE); //���M���荞�݂��I�t
		}
	}
}
