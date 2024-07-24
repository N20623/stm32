/*
 * modbus.h
 *
 *  Created on: Jul 16, 2024
 *      Author: Lenovo
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_

/* RTU 应答代码 */
#define RSP_OK				  0			// 成功
#define RSP_ERR_CMD			 0x01	// 不支持的功能码
#define RSP_ERR_REG_ADDR	 0x02	// 寄存器地址错误
#define RSP_ERR_VALUE		  0x03	// 数据值域错误
#define RSP_ERR_WRITE		  0x04	// 写入失败
#define MOD_TX_RX_BUF_SIZE   255  //  接收 和发送的最大数组数

typedef struct {
//	uint8_t rxBuf[MOD_TX_RX_BUF_SIZE];  // 255
//	uint16_t rxCnt;  // 接收的计数(size)
	uint8_t txBuf[127];  // 255
	uint16_t txCnt; // 发送的计数(size)
//	uint16_t  c3_5Timeout; // modbus3.5个字符的时间，超时
	uint8_t rxBuf[16];
	uint16_t rxCnt; // 发送计数
	uint8_t rspCode; // 响应码 1表示OK
} Modbus_T;

void MODS_AnalyzeApp(uint8_t * receiveData,uint16_t Size);

#endif /* INC_MODBUS_H_ */
