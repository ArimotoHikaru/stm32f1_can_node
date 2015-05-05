
#ifndef I2C_H
#define I2C_H

#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

void I2C_Configuration(void);

void AWrite_ATP(char slave_add, char cmd);
char ARead_ATP(char slave_add);


#endif
