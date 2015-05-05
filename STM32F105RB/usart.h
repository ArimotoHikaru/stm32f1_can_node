
#ifndef USART_H
#define USART_H
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "main.h"

//一文字送信
void transmit_uart4_c(char c);

//文字列送信
void transmit_uart4_s(char *s);

//USART2送受信割り込みハンドラ
void UART4_IRQHandler(void);

//USART2初期化関数
void UART4_Configuration(void);

#endif
