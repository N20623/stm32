/*
 * userLib.h
 *
 *  Created on: May 24, 2024
 *      Author: Lenovo
 */

#ifndef USER_USERLIB_H_
#define USER_USERLIB_H_

uint16_t CRC16_Modbus(uint8_t * data, uint16_t Size);
uint16_t BEBufToUint16(uint8_t *_pBuf);
int16_t BEBufToInt16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);
int16_t LEBufToInt16(uint8_t *_pBuf);
uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);
void MODS_SendWithCRC(uint8_t *_pBuf, uint16_t _Len);

#endif /* USER_USERLIB_H_ */
