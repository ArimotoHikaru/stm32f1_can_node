
#ifndef USART_H
#define USART_H
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "main.h"

//�ꕶ�����M
void transmit_uart2_c(char c);

//�����񑗐M
void transmit_uart2_s(char *s);

//USART2����M���荞�݃n���h��
void USART2_IRQHandler(void);

//USART2�������֐�
void USART2_Configuration(void);

#endif
