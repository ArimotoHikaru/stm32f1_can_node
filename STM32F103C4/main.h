/**
  ******************************************************************************
  * @file    Demonstrations/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define USE_USART2
#define USE_INTERRUPT_USART2

#define USE_ENCODER
#define USE_INTERRUPT_TIM1
#define USE_INTERRUPT_TIM2
#define USE_INTERRUPT_TIM3

#define USE_ADC

#define USE_CAN
#define USE_INTERRUPT_CAN_RX
#define USE_INTERRUPT_CAN_TX

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "stm32f10x_can.h"

/* user_hedder --------------------------------------------------------------*/
#include "adc.h"
#include "usart.h"
#include "tim_encoder.h"
#include "can.h"
#include "nvic.h"

#define CAN_NodeId_Dualshock3	0x003	//0x00~0x1F
#define CAN_NodeId_STM32F4_1	0x0F

//Dualshock3�̃f�[�^��͗p
//���̗񋓌^�̒l��Arduino���̐ݒ�ƍ��킹��
enum {
	start = 'a',
	select,
	left_x,
	left_y,
	right_x,
	right_y,
	L1,L2,
	R1,R2,
	Triangle,
	Circle,
	Cross,
	Square,
	Up,
	Right,
	Down,
	Left,
}Mnemonic;

typedef struct _Button{
	uint8_t	value;
}Button;

typedef struct _DualshockBotton{
	Button	start,
			select,
			left_x,
			left_y,
			right_x,
			right_y,
			L1,
			L2,
			R1,
			R2,
			Triangle,
			Circle,
			Cross,
			Square,
			Up,
			Right,
			Down,
			Left;
}DualshockBotton;




/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void init();
void ColorfulRingOfDeath(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/