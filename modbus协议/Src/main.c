/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "userLib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define A0_Addr 0b10010000
#define addr 0x01   // modbus地址
#define rate1 0.2456
extern  RS485Data rs485data;
 uint8_t receive485Data[20]; //接收485数据的缓冲区
extern uint8_t readIICBuffer[3]; // IIC读取数据的缓
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


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 串口发送完成回调
HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	//
	if(huart == &huart2){
		 HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
		 HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
	}
}
/**
 * 串口接收完成的回调
 */
HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if(huart == &huart2){
//		接收完成准备开始发送，拉高电平
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
//				 开始分析数据
		MODS_AnalyzeApp(&receive485Data,sizeof(receive485Data));
	}
}
// IIC的接收完成的回调
HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c==&hi2c1){
		//  处理读取来的数据
		// 将读取来的数据转换为16位有符号的整
		 int16_t Btemp=BEBufToUint16(readIICBuffer);
//		 int16_t Ltemp=LEBufToUint16(readBuffer);
//		 计算系数得到的结果
		 float resultTemp=Btemp*rate1;
//		 double resultTemp1=Btemp*rate1;
//		 交换高低字节
//		uint32_t Bresult=BEBufToUint32(&resultTemp);
		// 将float的地址转换为uint8_t类型的指针
//		uint8_t *p = (uint8_t *)&Bresult;
//		uint8_t *fp = (uint8_t *)&resultTemp;
		// 将浮点数的地址转换为uint8_t指针，并复制内存
		const uint8_t *fp = (const uint8_t *)&resultTemp;
		// 逐字节复制浮点数到数组
		for (int j = 0; j < 4; j++) {
			rs485data.iicBuf[rs485data.iicCnt++] = fp[3-j];  // 在这里调整字节顺序，以符合大端格式
 	  }
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
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  // 485芯片复位拉低电平准备开始接收
  HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_RESET);
  // 开启接收空闲的中断
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)receive485Data,sizeof(receive485Data));
//  HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	HAL_I2C_Master_Transmit(&hi2c1,A0_Addr,0x0C,1,100);  //  初始化模拟IIC芯片
//  HAL_UART_Transmit(&huart2,  (uint8_t *)str, sizeof(str), 500);
//  I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout
//  HAL_I2C_Master_Receive(&hi2c1,A0_Addr,&readBuffer,1,HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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
