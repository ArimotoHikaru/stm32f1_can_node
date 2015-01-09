/**
 *  Defines for your entire project at one place
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

#include "stm32f4xx_sdio.h"
#include "stm32f4xx_dma.h"
#include "misc.h"
//Use detect pin
#define FATFS_USE_DETECT_PIN			1
//Use writeprotect pin
#define FATFS_USE_WRITEPROTECT_PIN		1

//Activate SDIO 1-bit mode
//#define FATFS_SDIO_4BIT         0

//Activate SDIO 4-bit mode
#define FATFS_SDIO_4BIT         1

//If you want to overwrite default CD pin, then change this settings
/*
#define FATFS_USE_DETECT_PIN_RCC        RCC_AHB1Periph_GPIOB    
#define FATFS_USE_DETECT_PIN_PORT        GPIOB
#define FATFS_USE_DETECT_PIN_PIN        GPIO_Pin_6
*/
//If you want to overwrite default WP pin, then change this settings
/*
#define FATFS_USE_WRITEPROTECT_PIN_RCC        RCC_AHB1Periph_GPIOB    
#define FATFS_USE_WRITEPROTECT_PIN_PORT        GPIOB
#define FATFS_USE_WRITEPROTECT_PIN_PIN        GPIO_Pin_7
*/


#endif
 
