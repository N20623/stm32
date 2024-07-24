/*
 * modbus.c
 *
 *  Created on: Jul 16, 2024
 *      Author: Lenovo
 */
#include "main.h"
#include "usart.h"
#include "modbus.h"
#include "userLib.h"
#include "readAdcChannels.h"


extern uint8_t swapFinshBuffer[48];
Modbus_T rs485data;  // 实列化modbus的数据结构体
extern uint8_t localAddr; //  modbus协议的地址
extern uint8_t receive485Data[8]; // modbus协议接收到的字节的长度
extern uint8_t readI2cBuffer[24]; //读取数据 --------------------------------测试要删除
extern float floatData[12]; // 12个浮点数 1浮点数=4字节


static void MODS_SendAckOk(uint8_t _ch);
static void MODS_SendAckErr(uint8_t * receive485Data,uint16_t Size);
static void MODS_01H(uint8_t * receiveData);
static void MODS_02H(uint8_t * receiveData);
static void MODS_03H(uint8_t * receiveData);
static void MODS_04H(uint8_t * receiveData);
static void MODS_05H(uint8_t * receiveData);
static void MODS_06H(uint8_t * receiveData);
static void MODS_0FH(uint8_t * receiveData);
static void MODS_10H(uint8_t * receiveData);

void MODS_AnalyzeApp(uint8_t * receiveData,uint16_t Size) {

	if (Size < 8) /* 接收到的数据小于4个字节就认为错误 */
	{	/*
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receive485Data,sizeof(receive485Data));
		*/
		return;
	}
	/* 站地址 (1字节） */
	/* 第1字节 站号 */
	if (localAddr != receiveData[0]) /* 判断主机发送的命令地址是否符合 */
	{
		if(localAddr==0xFF){
			return;
		}
		return;
	}
	/* 计算CRC校验和 */
	// 来自数据的
	uint16_t crc=BEBufToUint16(&receiveData[Size-2]); // 1 数组加括号是取值
//	uint16_t crc=BEBufToUint16(receiveData+Size-2); // 2 数组是地址加偏移 两种方法都可以
	// 本地计算得出的
	uint16_t crc1=CRC16_Modbus(receiveData, Size-2);
	if (crc1 != crc)
	{
		return;
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
				MODS_03H(receiveData );
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
				MODS_SendAckErr(receiveData,Size);	/* 告诉主机命令错误 */
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
static void MODS_03H(uint8_t * receiveData)
{

	rs485data.txCnt=0;  // 清零发送计数

	rs485data.txBuf[rs485data.txCnt++]=receiveData[0]; // [0]地址 i++ 先赋值再自增
	rs485data.txBuf[rs485data.txCnt++]=receiveData[1]; // [1]功能码

	uint16_t startAddr;
	uint16_t Length;

//	起始数据地址
	startAddr = BEBufToUint16(&receiveData[2]);
//	if(startAddr)
//	需要数据长度
	Length = BEBufToUint16(&receiveData[4]);
	rs485data.txBuf[rs485data.txCnt++]=Length *2; // [2]要发送的寄存器数量
	uint8_t *p= &swapFinshBuffer;
	/* 返回数值 */
	for (uint16_t i = 0; i < Length ; i++)
	{

//		rs485data.txBuf[rs485data.txCnt++]=swapFinshBuffer[2*i]; //0  2  4  6
//		rs485data.txBuf[rs485data.txCnt++]=swapFinshBuffer[2*i+1]; //1 3  5  7

		rs485data.txBuf[rs485data.txCnt++]=*(p+(2*i));  //0  2  4  6
		rs485data.txBuf[rs485data.txCnt++]=*(p+(2*i+1)); //1 3  5  7

	}
	MODS_SendWithCRC(rs485data.txBuf,rs485data.txCnt);
	HAL_GPIO_WritePin(ss485_GPIO_Port,ss485_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit_DMA(&huart1,rs485data.txBuf,rs485data.txCnt+2);  // CRC+2
}
static void MODS_01H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_02H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_04H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_05H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_06H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_0FH(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_10H(uint8_t * receiveData)
{
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)receive485Data,sizeof(receive485Data));
}


