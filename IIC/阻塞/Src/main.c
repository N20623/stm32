/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * 使用DMA发�?�不定长的数�?????????????????
  *	波特率为9600
  *
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define A0_Addr 0b10010000
#define ff 0.24325
char A0_Reset=0x0C;
char A0_ReLoad=0x06;
#define vdd 5.031
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */




/**
	HAL_I2C_Master_Receive(

	I2C_HandleTypeDef *hi2c,

	uint16_t DevAddress,

	uint8_t *pData,

	uint16_t Size,

	uint32_t Timeout);
*/



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t receiveData[265];
//uint8_t sendData[9];
uint8_t readBuffer[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	//
	if(huart == &huart2){
		 HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
	}
}
/**
 * 接收完成的中断
 */
HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
		if(huart == &huart2){
//			接收完成准备开始发送，拉高电平
			 HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
			 	 if(receiveData[0]==1){ // 分析485接收来的数据
			 		A0_Reset=receiveData[1];
			 		// IIC发送复位ADC转换芯片
			 		HAL_I2C_Master_Transmit(&hi2c1,A0_Addr, &A0_Reset,1,100);
			 	 }
			 	 if(receiveData[0]==2){
			 		 // 接收IIC芯片数据 2字节的数据 1字节的配置
			 		HAL_I2C_Master_Receive(&hi2c1,A0_Addr,&readBuffer,4,100);
			 	}
			 	 if(receiveData[0]==3){
			 		// 接收IIC芯片数据 2字节的数据 1字节的配置
			 		HAL_I2C_Master_Transmit(&hi2c1,0x00,&A0_ReLoad,1,100);
			 	}
			}
/*
	float x=123.432;
	uint8_t* tdata = (uint8_t*)(&x);
	sendData[0]=0x01;
	sendData[1]=0x03;
	sendData[2]=4;
	sendData[3]=tdata[3];
	sendData[4]=tdata[2];
	sendData[5]=tdata[1];
	sendData[6]=tdata[0];
	uint16_t crc=CRC16_Modbus(sendData,7);
	uint8_t* pcrc = (uint8_t*)(&crc);
	sendData[7]=pcrc[1];
	sendData[8]=pcrc[0];
	*/


//	int a= sizeof(x);

	if(huart == &huart2){
//		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)sendData, sizeof(sendData));
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)readBuffer, sizeof(readBuffer));
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receiveData,sizeof(receiveData));

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  // 485芯片复位拉低电平开始接收
  HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
  // 开启接收空闲的中断
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receiveData,sizeof(receiveData));
//  HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
//	HAL_I2C_Master_Transmit(&hi2c1,A0_Addr,0,1,100);

//  HAL_UART_Transmit(&huart2,  (uint8_t *)str, sizeof(str), 500);
//  I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout
//  HAL_I2C_Master_Receive(&hi2c1,A0_Addr,&readBuffer,1,HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  float a=123.3;
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

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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
