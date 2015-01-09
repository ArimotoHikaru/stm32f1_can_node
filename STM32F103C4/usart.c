#include "usart.h"

#define USART2_BAUDRATE 19200

#define USART2_TxBufferSize  200//�����O�o�b�t�@�̗v�f��
#define USART2_RxBufferSize  200//�����O�o�b�t�@�̗v�f��

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

//�ꕶ�����M
void transmit_uart2_c(char c)
{

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);  //���M���I��������荞�݂��ꎞ�I�ɋ֎~
#endif

  USART2_TxBuffer[USART2_TxPtrEnd] = c;    //�����O�o�b�t�@�ɂP�����ǉ�
  USART2_TxPtrEnd++;    //�������݃|�C���^���{�P

  if(USART2_TxPtrEnd > (USART2_TxBufferSize-1)) USART2_TxPtrNow=0; USART2_TxPtrEnd = 1;    //�������݃|�C���^���I�[�o�[�t���[������O�ɖ߂�

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  // ���M���I��������荞�݂�����
#endif

}
//�����񑗐M
void transmit_uart2_s(char *s)
{

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE); 
#endif

  while(*s!=0) // until end of string   ������̖����܂Ń��[�v
  {
    USART2_TxBuffer[USART2_TxPtrEnd] = *(s++);
    USART2_TxPtrEnd++; //  �|�C���^�{�P

    if(USART2_TxPtrEnd > (USART2_TxBufferSize-1)) USART2_TxPtrEnd = 0;
  }

#ifdef USE_INTERRUPT_USART2
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);   //���M���I��������荞�݂�����
#endif

}

void USART2_IRQHandler(void)
{
  
	char c;
  
	//��M���荞��
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		c = (char)USART_ReceiveData(USART2);
		transmit_uart2_c(c);
	}
  
	//���M���荞��
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET){  // UART���M�t���O�`�F�b�N
		USART_SendData(USART2, USART2_TxBuffer[USART2_TxPtrNow++]); // �P�������M

		if(USART2_TxPtrNow > (USART2_TxBufferSize-1)) USART2_TxPtrNow=0;//   �|�C���^�I�[�o�[�t���[�Ȃ�[���ɖ߂�

		if(USART2_TxPtrNow == USART2_TxPtrEnd){     //�����O�o�b�t�@���󂩁H
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE); //���M���荞�݂��I�t
		}
	}
}
