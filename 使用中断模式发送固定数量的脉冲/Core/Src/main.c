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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/************************************************************************/
//#include "rs485.h"

/*******************************************************************************/
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t str[]="hello";
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t sin_val[]={
		500,531,562,593,624,654,684,712,740,767,793,818,842,864,885,904,
		922,938,952,964,975,984,991,996,999,1000,999,996,991,984,975,964,
		952,938,922,904,885,864,842,818,793,767,740,712,684,654,624,593,
		562,531,500,468,437,406,375,345,315,287,259,232,206,181,157,135,
		114,95,77,61,47,35,24,15,8,3,0,0,0,3,8,15,
		24,35,47,61,77,95,114,135,157,181,206,232,259,287,315,345,
		375,406,437,468
};
uint8_t size=100;
uint8_t receiveData[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static int num=5;
/*
 static int i=0;
 // 周期中断函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(++i==size)i=0;
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,sin_val[i]);
		HAL_GPIO_WritePin(ss485_GPIO_Port, ss485_Pin, GPIO_PIN_SET);
		HAL_UART_Transmit(&huart2,str,sizeof(str),100);

}
*/
static uint16_t cnt = 0;
// 每发一个脉冲触发一次中断 会频繁调用中断
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){

	cnt++;
	if(cnt == num){
		cnt = 0;
		HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_1);
	}


}
// 串口接收完成中断  当为阻塞模式接收，按字符数计算   当为中断模式接收也是字符数吧   当为空闲中断接收   会检测到串口的空闲时间 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart== &huart2){
		if(receiveData[0]==1){

			HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_1);


//			HAL_TIMEx_PWMN_Stop_IT(&htim1,TIM_CHANNEL_1);
//			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,receiveData[1]);
		}

		if(receiveData[0]==2){
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,receiveData[1]);
			HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start_IT(&htim1,TIM_CHANNEL_1);
		}
		if(receiveData[0]==3){
			num=receiveData[1];
			HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);

//			HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
//			HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_1);
//			HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
		}
		HAL_UART_Receive_IT(&huart2,receiveData,2);
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
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* MCU Configuration--------------------------------------------------------*/
//  rs485_Init();
//  Init_Ads000(); // 初始化设备外设 模拟量的芯片
  /* MCU Configuration--------------------------------------------------------*/
//  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//  HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
//  HAL_TIM_Base_Start_IT(&htim1);
  HAL_UART_Receive_IT(&huart2,receiveData,2);




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
