
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
void transmit_uart2_c(char c);

//文字列送信
void transmit_uart2_s(char *s);

//USART2送受信割り込みハンドラ
void USART2_IRQHandler(void);

//USART2初期化関数
void USART2_Configuration(void);

#endif
