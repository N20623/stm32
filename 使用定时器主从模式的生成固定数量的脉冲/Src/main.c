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
// TIM_IT_UPDATE

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==&htim2){
				if(	__HAL_TIM_GET_FLAG(&htim2,TIM_FLAG_CC2)!=RESET){ // 判断从定时器是否记录到个
				__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_CC2); // 清除中断标志

				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1); //  关闭脉冲
				HAL_TIM_Base_Stop_IT(&htim2); //  关闭定时器中
				}
		}
	//  存在中断相应问题  第二个脉冲计数不会生效
	if(htim==&htim4){
			if(	__HAL_TIM_GET_FLAG(&htim4,TIM_FLAG_CC4)!=RESET){ // 判断从定时器是否记录到个
				__HAL_TIM_CLEAR_FLAG(&htim4,TIM_FLAG_CC4); // 清除中断标志
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1); //  关闭脉冲
				HAL_TIM_Base_Stop_IT(&htim4); //  关闭定时器中
			}

		}
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart== &huart2){
		if(receiveData[0]==1){
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim2); //  关闭定时器中
//			HAL_TIMEx_PWMN_Stop_IT(&htim1,TIM_CHANNEL_1);
//			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,receiveData[1]);
		}
		if(receiveData[0]==2){
			//  要使用PWM模式2才能正确的发出 脉冲数
//			 PWM模式1 会多发一个脉冲数
			__HAL_TIM_SET_AUTORELOAD(&htim2,9);
			__HAL_TIM_SET_AUTORELOAD(&htim4,9);
			HAL_TIM_Base_Start_IT(&htim2);
			HAL_TIM_Base_Start_IT(&htim4);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		}
		if(receiveData[0]==3){
			__HAL_TIM_SET_AUTORELOAD(&htim2,receiveData[1]);
			HAL_TIM_Base_Start_IT(&htim2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
		}
//			HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
//			HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_1);
//			HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
		}

//		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_UPDATE);
//		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_TRIGGER);
		HAL_UART_Receive_IT(&huart2,receiveData,1);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  /* MCU Configuration--------------------------------------------------------*/
//  rs485_Init();
//  Init_Ads000(); // 初始化设备外�????????????????????????????????????????????????????????
  /* MCU Configuration--------------------------------------------------------*/
//  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//  HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
//  HAL_TIM_Base_Start_IT(&htim1);


  HAL_UART_Receive_IT(&huart2,receiveData,1);




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
