/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"
#include "userLib.h"
#include "readAdcChannels.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define I2C_Size 2
#define S0_L HAL_GPIO_WritePin(GPIOB,S0_Pin,RESET)
#define S0_H HAL_GPIO_WritePin(GPIOB,S0_Pin,SET)
#define S1_L HAL_GPIO_WritePin(GPIOB,S1_Pin,RESET)
#define S1_H HAL_GPIO_WritePin(GPIOB,S1_Pin,SET)
static void readAnalogChannel(uint16_t Len,uint8_t* _readI2cBuffer );
static void calFloat(uint8_t * _icBuff,uint16_t _Len,float * _rate,float *_floatData);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/**
 *  00  128sps  9.62ms
 *  01  32sps	38.46ms
 *  10	16sps   76.92ms
 *  11  8sps   153.85ms
 *  adc采样的平均间隔ms
 */
uint8_t adcConfig=0b00000000;  // 配置为持续采样 并且调整采样率

uint8_t receive485Data[8]; // modbus协议接收到的字节的长度

uint8_t readI2cBuffer[24]; //读取数据
float rate[12]={
	1000.0f,1000.0f,1000.0f,1000.0f,
	1000.0f,1000.0f,1000.0f,1000.0f,
	1000.0f,1000.0f,1000.0f,1000.0f
};  // 每个通道的比例
float floatData[12]; // 12个浮点数 1浮点数=4字节
uint32_t swapFinshBuffer[12]; // 48个字节
uint8_t localAddr=1; //  modbus协议的地址

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if(huart==&huart1){
		 MODS_AnalyzeApp(receive485Data, Size);
	}else{
		// 对没用的干扰 启用接收就行
		HAL_GPIO_WritePin(ss485_GPIO_Port,ss485_Pin, RESET);
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receive485Data,sizeof(receive485Data));
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart==&huart1){
	HAL_GPIO_WritePin(ss485_GPIO_Port,ss485_Pin, RESET);
	}
}
/***
 * Len  读取的数据的长度
 * _readI2cBuffer 保存数据的数组
 *
 */
static void readAnalogChannel(uint16_t Len,uint8_t* _readI2cBuffer ){
	for(uint16_t i=0;i<Len;i++){
	//	读取IIC芯片数据
	if(0<=i && i<4 ){
		//		HAL_OK
		if(i%4==0){	S0_L; S1_L;} // 0
		if(i%4==1){	S0_H; S1_L;} // 1
		if(i%4==2){	S0_L; S1_H; } // 2
		if(i%4==3){	S0_H; S1_H; } // 3
//		uint8_t readI2cBuffer[24]; 定义在头文件中
		HAL_I2C_Master_Receive(&hi2c1,A0_Addr+(2*0),(_readI2cBuffer+2*i), I2C_Size,100);
		HAL_Delay(9);
		continue;
	}
	if(4<=i && i<8){
		if(i%4==0){	S0_L; S1_L; } // 4
		if(i%4==1){	S0_H; S1_L; } // 5
		if(i%4==2){	S0_L; S1_H; } // 6
		if(i%4==3){	S0_H; S1_H; } // 7
		HAL_I2C_Master_Receive(&hi2c1,A0_Addr+(2*1),(_readI2cBuffer+2*i), I2C_Size,100);
		HAL_Delay(9);
		continue;
	}
	if(8<=i && i<12){
		if(i%4==0){	S0_L; S1_L; } // 8
		if(i%4==1){	S0_H; S1_L; } // 9
		if(i%4==2){	S0_L; S1_H; } // 10
		if(i%4==3){	S0_H; S1_H; } // 11
		HAL_I2C_Master_Receive(&hi2c1,A0_Addr+(2*2),(_readI2cBuffer+2*i), I2C_Size,100);
		HAL_Delay(9);
		continue;
	}
 }
}

/**
 *   icBuff  24
 *
 *
 */
static void calFloat(uint8_t * _icBuff,uint16_t i2c_Len,float * _rate,float * _floatBuffer){
	uint16_t j;
	uint16_t  t_Len= i2c_Len>>1;
	for(j=0;j<t_Len;j++){
	 _floatBuffer[j]=BEBufToInt16(_icBuff+2*j)/_rate[j];
	}
}

/**
 * 将float数组进行char类型数组转换  并且4字节交换
 */
static void swapByte(float * _floatByteBuff,uint16_t _numFloats,uint32_t *_swapBuffer){
	uint16_t i;
	uint8_t *p=(uint8_t *)_floatByteBuff;
	for(i=0;i<_numFloats;i++){
		_swapBuffer[i]=BEBufToUint32(p+(4*i));
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_I2C_Master_Transmit(&hi2c1,  A0_Addr,&adcConfig,1,100); // 初始化芯片
//  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receive485Data,sizeof(receive485Data));

  /*
   * 修改芯片为单采集模式

    HAL_GPIO_WritePin(GPIOB, ss485_Pin, SET);
    HAL_I2C_Master_Receive(&hi2c1,0xD0,i2cReceiveData, 3,100);
    HAL_UART_Transmit(&huart1,i2cReceiveData,3,100);
    HAL_Delay(1000);
   */

  /*
  i2cSendData[0]=0x08;
  HAL_I2C_Master_Transmit(&hi2c1,0xD0,i2cSendData, 3,100);

  	    HAL_GPIO_WritePin(GPIOB, ss485_Pin, SET);
  	    HAL_I2C_Master_Receive(&hi2c1,0xD0,i2cReceiveData, 3,100);
  	    HAL_Delay(1000);
  	    HAL_UART_Transmit(&huart1,i2cReceiveData,3,100);
   */

  /**
   *  readAnalogChannel(12);
	  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receive485Data,sizeof(receive485Data));
   *
   *
   */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		readAnalogChannel(12,readI2cBuffer);
	//	i2cLen=sizeof(readI2cBuffer)/sizeof(readI2cBuffer[0])/2;
		calFloat(readI2cBuffer,24,rate,floatData);

		swapByte(floatData,12,swapFinshBuffer);

		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receive485Data,sizeof(receive485Data));
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
