
#ifndef ADC_H
#define ADC_H
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"

//ADC初期化関数
void ADC_Configuration(void);
//値取得関数
uint16_t GetAdc1Value_xch(uint8_t ch);

#endif
