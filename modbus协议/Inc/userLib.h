/*
 * userLib.h
 *
 *  Created on: May 24, 2024
 *      Author: Lenovo
 */

#ifndef USER_USERLIB_H_
#define USER_USERLIB_H_

/* RTU 应答代码 */
#define RSP_OK				  0			// 成功
#define RSP_ERR_CMD			 0x01	// 不支持的功能码
#define RSP_ERR_REG_ADDR	 0x02	// 寄存器地址错误
#define RSP_ERR_VALUE		  0x03	// 数据值域错误
#define RSP_ERR_WRITE		  0x04	// 写入失败
#define MOD_TX_RX_BUF_SIZE   255

typedef struct {
//	uint8_t rxBuf[MOD_TX_RX_BUF_SIZE];  // 255
//	uint16_t rxCnt;  // 接收的计数(size)
	uint8_t txBuf[MOD_TX_RX_BUF_SIZE];  // 255
	uint16_t txCnt; // 发送的计数(size)
//	uint16_t  c3_5Timeout; // modbus3.5个字符的时间，超时
	uint8_t iicBuf[MOD_TX_RX_BUF_SIZE];
	uint8_t readIICLen;
	uint16_t iicCnt;
	uint8_t rspCode; // 响应码 1表示OK
} RS485Data;

void MODS_AnalyzeApp(uint8_t * receiveData,uint16_t Size);
uint16_t CRC16_Modbus(uint8_t * data, uint16_t Size);

static void MODS_01H(uint8_t * receiveData);
static void MODS_02H(uint8_t * receiveData);
static void MODS_03H(uint8_t * receiveData);
static void MODS_04H(uint8_t * receiveData);
static void MODS_05H(uint8_t * receiveData);
static void MODS_06H(uint8_t * receiveData);
static void MODS_0FH(uint8_t * receiveData);
static void MODS_10H(uint8_t * receiveData);
static void MODS_SendAckOk(uint8_t _ch);
static void MODS_SendAckErr(uint8_t * receiveData,uint16_t Size);
static void MODS_SendWithCRC(uint8_t * receiveData, uint16_t Size);
void readAnalogChannel(uint16_t Len);
#endif /* USER_USERLIB_H_ */
