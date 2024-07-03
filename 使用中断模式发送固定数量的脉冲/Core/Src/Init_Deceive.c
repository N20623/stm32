/*
 * Init_Deceive.c
 *
 *  Created on: Jun 14, 2024
 *      Author: Lenovo
 */

#include "main.h"
#include "i2c.h"
uint8_t data =0x0C;

void Init_Ads000(){
	//			让ADS1000 复位一下
	HAL_I2C_Master_Transmit(&hi2c1,0x90,&data,1,100);

}
