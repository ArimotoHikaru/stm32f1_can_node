
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
void transmit_uart4_c(char c);

//�����񑗐M
void transmit_uart4_s(char *s);

//USART2����M���荞�݃n���h��
void UART4_IRQHandler(void);

//USART2�������֐�
void UART4_Configuration(void);

#endif
