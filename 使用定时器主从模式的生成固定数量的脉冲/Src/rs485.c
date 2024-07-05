/*
 * rs485.c
 *
 *  Created on: Jun 11, 2024
 *      Author: Lenovo
 */


#include "usart.h"
#include "main.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_def.h"
#include "user_lib.h"
#include "i2c.h"
//#include ""
#include "rs485.h"
//#define
#define A0_Addr 0b10010000
#define  slaveAddr  1
#define rate1 0.00025479613103190295516485865441137
uint8_t receive485Data[50];
RS485Data rs485data;
uint8_t readIICBuffer[3];
//uint8_t
 /**
 *
 * start receive rs485 data
 */
void rs485_Init(void){
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t *)receive485Data,sizeof(receive485Data));
}
/**
 * receive complete callback
 * ss485_GPIO_Port
 * ss485_Pin
 * 串口接收信息完成的回调
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if(huart== &huart2){
//		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
//		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)receive485Data,Size);
	//	HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

//				 HAL_Delay(100);
				 HAL_I2C_Master_Receive_IT(&hi2c1,A0_Addr,(uint8_t *)readIICBuffer,sizeof(readIICBuffer));
				 HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
				 HAL_UART_Transmit_DMA(&huart2,(uint8_t *)readIICBuffer,sizeof(readIICBuffer));



	}
}

/**
 * receive complete callback
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart2){

	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t *)receive485Data,sizeof(receive485Data));
	}

}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if( hi2c == &hi2c1 ){
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit_DMA(&huart2,(uint8_t *)readIICBuffer,sizeof(readIICBuffer));
	}
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
	if (slaveAddr != receiveData[0])	/* 判断主机发送的命令地址是否符合 */
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
		crc = CRC16_Modbus(_pBuf, _Len);
		_pBuf[_Len++] = crc >> 8;
		_pBuf[_Len++] = crc;
		HAL_UART_Transmit_DMA(&huart2,_pBuf, _Len); // end send & callback
}
/**
 * 错误应答返回
 *
 */
static void MODS_SendAckErr(uint8_t * receivedata,uint16_t size){

	HAL_UART_Transmit_DMA(&huart2,(uint8_t *)receivedata,size); // end send & callback

}

static void MODS_03H(uint8_t * receiveData)
{

	rs485data.txCnt=0;  // 清零发送计数
	rs485data.txBuf[rs485data.txCnt++]=receiveData[0]; // [0]地址 i++ 先赋值再自增
	rs485data.txBuf[rs485data.txCnt++]=receiveData[1]; // [1]功能码
	uint16_t startAddr;
	uint16_t Length;
	/*
			主机发送:
				11 从机地址
				03 功能码
				00 寄存器地址高字节
				6B 寄存器地址低字节
				00 寄存器数量高字节
				03 寄存器数量低字节
				76 CRC高字节
				87 CRC低字节
			从机应答: 保持寄存器的长度为2个字节。对于单个保持寄存器而言，寄存器高字节数据先被传输，
						低字节数据后被传输。保持寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
				11 从机地址
				03 功能码
				06 字节数
				00 数据1高字节(006BH)
				6B 数据1低字节(006BH)
				00 数据2高字节(006CH)
				13 数据2 低字节(006CH)
				00 数据3高字节(006DH)
				00 数据3低字节(006DH)
				38 CRC高字节
				B9 CRC低字节
		*/
//	起始数据地址
	startAddr = BEBufToUint16(&receiveData[2]);
	if(startAddr == 99){

	}
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
{
		uint16_t addr;
		uint16_t num;
		rs485data.txCnt=0;  // 清零发送计数
		rs485data.txBuf[rs485data.txCnt++]=receiveData[0]; // [0]地址 i++ 先赋值再自增
		rs485data.txBuf[rs485data.txCnt++]=receiveData[1]; // [1]功能码
		addr = BEBufToUint16(&receiveData[2]);  // 起始线圈地址
		num = BEBufToUint16(&receiveData[4]);  // 线圈数量


}
static void MODS_02H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_04H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_05H(uint8_t * receiveData)
{
	uint16_t addr;
	uint16_t value;
	uint16_t byteAddr;
	uint16_t bitOffset = 0;
	addr = BEBufToUint16(&receiveData[2]); 	// 寄存器地址
	value = BEBufToUint16(&receiveData[4]);	// 数据
	//////////////////////////////////////////////////////////////
	byteAddr = addr /8;	// 得到字节地址



	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_06H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_0FH(uint8_t * receiveData)
{
		uint16_t addr;
		uint16_t value;
		addr = BEBufToUint16(&receiveData[2]); 	// 寄存器地址
		value = BEBufToUint16(&receiveData[4]);	// 数据
	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}
static void MODS_10H(uint8_t * receiveData)
{	HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
}

void readAnalogChannel(uint16_t Len){
	if(Len>0){
	rs485data.iicCnt=0;
	for(uint16_t i=0;i<Len;i++ ){
		//		读取IIC芯片数据
//	 HAL_I2C_Master_Receive_IT(&hi2c1,A0_Addr,&readIICBuffer,sizeof(readIICBuffer));
	    HAL_I2C_Master_Receive(&hi2c1,A0_Addr,readIICBuffer,sizeof(readIICBuffer),100);
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

/*
*********************************************************************************************************
*	函 数 名: RS_CheckSum
*	功能说明: 累加和校验
*	形    参:
*	返 回 值:
*********************************************************************************************************
*/
uint8_t RS_CheckSum(uint8_t *src, uint16_t len)
{
	uint16_t i;
	uint8_t sum;

	sum = 0;
	for(i=0; i<len; i++)
	{
		sum += src[i];
	}
	return sum;
}
