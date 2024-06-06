/*
 * UserLib.c
 *
 *  Created on: May 24, 2024
 *      Author: Lenovo
 */
#include "main.h"
#include "i2c.h"
#include "userLib.h"
#include "usart.h"
#include "usart.h"
#define A0_Addr 0b10010000 // IIC地址
#define addr 0x01   // modbus地址
extern uint8_t receive485Data[20];
RS485Data rs485data;
#define rate1 0.2456
uint8_t readIICBuffer[3]; // IIC读取数据的缓
// float f[] = {8.5f, -3.2f, 0.75f};  // 示例浮点数数组

// CRC 高位字节值表
static const uint8_t s_CRCHi[] = {
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
// CRC 低位字节值表
const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen)
{
	uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint16_t usIndex;  /* CRC循环中的索引 */
    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((int16_t)_pBuf[0] << 8) | _pBuf[1]);
}
/*
*********************************************************************************************************
*	函 数 名: LEBufToUint16
*	功能说明: 将2字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((int16_t)_pBuf[1] << 8) | _pBuf[0]);
}
/*
*********************************************************************************************************
*	函 数 名: BEBufToUint32
*	功能说明: 将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}
/*
*********************************************************************************************************
*	函 数 名: LEBufToUint32
*	功能说明: 将4字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}
 void MODS_AnalyzeApp(uint8_t * receiveData,uint16_t Size) {
//	uint16_t crc1;  // 计算CRC校验和
	if (Size < 4) /* 接收到的数据小于4个字节就认为错误 */
	{
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
	}
	/* 计算CRC校验和 */
	/*
	crc1 = CRC16_Modbus(receiveData, Size);
	if (crc1 != 0)
	{return;}
	*/
	/* 站地址 (1字节） */
	/* 第1字节 站号 */
	if (addr != receiveData[0])	/* 判断主机发送的命令地址是否符合 */
	{
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
	}
	switch (receiveData[1])	// 第2个字节 功能码
		{
			case 0x01:		/* 读取线圈状态*/
			{
				MODS_01H(receiveData);
				break;
			}
			case 0x02:	/* 读取输入状态*/
			{
				MODS_02H(receiveData);
				break;
			}
			case 0x03:	/* 读取保持寄存器*/
			{
				MODS_03H(receiveData);
				break;
			}
			case 0x04:	/* 读取输入寄存器*/
			{
				MODS_04H(receiveData);
				break;
			}
			case 0x05:	/* 强制单线圈*/
			{
				MODS_05H(receiveData);
				break;
			}
			case 0x06:	/* 写单个保存寄存器*/
			{
				MODS_06H(receiveData);
				break;
			}
			case 0x0F:	/* 写多个线圈*/
			{
				MODS_0FH(receiveData);
				break;
			}
			case 0x10:	/* 写多个保存寄存器*/
			{
				MODS_10H(receiveData);
				break;
			}
			default:
			{
				MODS_SendAckErr( receiveData,Size);	/* 告诉主机命令错误 */
				break;
			}
		}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckOk(uint8_t _ch)
{
	uint8_t i;
	if(_ch < 2)
//	gModS[_ch].txCnt =0;
	for (i = 0; i < 6; i++)
	{
//		gModS[_ch].txBuf[gModS[_ch].txCnt ++] = gModS[_ch].rxBuf[i];
	}
	if(_ch < 2){}
//		MODS_SendWithCRC(_ch,gModS[_ch].txBuf, gModS[_ch].txCnt);

}
/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t * receiveData,uint16_t Size)
{
	uint8_t _ucErrCode;
	_ucErrCode += receiveData[0];			/* 485地址 */
	_ucErrCode += receiveData[1] | 0x80;	/* 异常的功能码 */
		 /* 错误代码(01,02,03,04) */
}
/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint16_t _Len)
{
	uint16_t crc;
	crc = CRC16_Modbus(_pBuf,  _Len);

	_pBuf[_Len++] = crc >> 8;
	_pBuf[_Len++] = crc;
	HAL_UART_Transmit_DMA(&huart2,_pBuf, _Len);
}
static void MODS_03H(uint8_t * receiveData)
{

	rs485data.txCnt=0;  // 清零发送计数
	rs485data.txBuf[rs485data.txCnt++]=receiveData[0]; // [0]地址 i++ 先赋值再自增
	rs485data.txBuf[rs485data.txCnt++]=receiveData[1]; // [1]功能码
	uint16_t startAddr;
	uint16_t Length;

//	起始数据地址
	startAddr = BEBufToUint16(&receiveData[2]);
//	需要数据长度
	Length = BEBufToUint16(&receiveData[4]);
	rs485data.txBuf[rs485data.txCnt++]=Length *2; // [2]要发送的寄存器数量
	rs485data.readIICLen=Length/4;
	readAnalogChannel(rs485data.readIICLen);
	/* 返回数值 */
	for (uint8_t i = 0; i < Length ; i++)
	{
		rs485data.txBuf[rs485data.txCnt++]=rs485data.iicBuf[2*i] ;  //txfBuf[3] iicBuf[0]
		rs485data.txBuf[rs485data.txCnt++]= rs485data.iicBuf[2*i+1];    //txfBuf[4]  iicBuf[1]

	}
	MODS_SendWithCRC(rs485data.txBuf,rs485data.txCnt);
}

static void MODS_01H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_02H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_04H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_05H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_06H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_0FH(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}
static void MODS_10H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));}

void readAnalogChannel(uint16_t Len){
	if(Len>0){
	rs485data.iicCnt=0;
	for(uint16_t i=0;i<Len;i++ ){
		//		读取IIC芯片数据
//	 HAL_I2C_Master_Receive_IT(&hi2c1,A0_Addr,&readIICBuffer,sizeof(readIICBuffer));
	    HAL_I2C_Master_Receive(&hi2c1,A0_Addr,&readIICBuffer,sizeof(readIICBuffer),100);
	    int16_t Btemp=BEBufToUint16(readIICBuffer);
		float resultTemp=Btemp*rate1;
//		const uint8_t *fp = (const uint8_t *)&resultTemp;
	    uint8_t *fp = ( uint8_t *)&resultTemp;
			// 逐字节复制浮点数到数组
		for (int j = 0; j < 4; j++) {
		  rs485data.iicBuf[rs485data.iicCnt++] = fp[3-j];  // 在这里调整字节顺序，以符合大端格式
	}
}
	}else{
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
	}

}


