/*
 * user_lib.h
 *
 *  Created on: Jun 12, 2024
 *      Author: Lenovo
 */

#ifndef INC_USER_LIB_H_
#define INC_USER_LIB_H_
#include "main.h"
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _Len);
uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);
uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);


#endif /* INC_USER_LIB_H_ */
