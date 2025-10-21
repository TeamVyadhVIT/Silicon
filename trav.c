/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
// A single global variable for received data
uint8_t rx_data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);

static void MX_CAN_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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
  MX_TIM2_Init();
  MX_TIM3_Init();

  MX_CAN_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  char tx_data[40]="ENTER COMMAND\r\n";
  HAL_UART_Transmit(&huart1,(uint8_t*)tx_data,40,100);
//2 for each so 12
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);


  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    // Start the UART receiver in interrupt mode
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	// The main loop is empty. All logic is handled in the interrupt callback.

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

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */


  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */



  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */


/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 255;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 255;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA8 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 PB3
                           PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

  /* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	if (huart->Instance == USART2)
	{
		// A single byte 'w', 'a', 's', 'd', 'x' is expected.
		// Use the global `rx_data` variable, not a new local one.
		//channel 2 for left and channel 3 for right side
		switch (rx_data)
		{
			case 'w': // Forward
			{
				char tx[30]="working\r\n";
				HAL_UART_Transmit(&huart1, (uint8_t*)tx,30, HAL_MAX_DELAY);

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//Motor
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);//MOTOR 4
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1);//MOTOR 5

				for(int i=0;i<=180;i++){
			     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,i);//motor 1r
			     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, i);//motor 2r
			     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, i);//motor 3r

				    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, i);//motor 1l
			     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, i); //motor 2l
			     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,i);//motor 3l


				break;
				}
			}
			case 'd': // Turn right
			{
				char tx[30]="working\r\n";
								HAL_UART_Transmit(&huart1, (uint8_t*)tx,30, HAL_MAX_DELAY);
								//HAL_UART_Transmit(&huart1, (uint8_t*)tx, (tx), HAL_MAX_DELAY);

								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);//MOTOR R1
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);//MOTOR R2
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);//MOTOR R3
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);//MOTOR L1
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);//MOTOR L2
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);//MOTOR L3


				 for(int i=100;i<=180;i++){
					 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,180-i+100);//motor 1r
					 			     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 180-i+100);//motor 2r
					 			     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 180-i+100);//motor 3r

					 				    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, i);//motor 1l
					 			     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, i); //motor 2l
					 			     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,i);//motor 3l


				break;
			}
			}
			case 'x': // Stop
			{
				char tx[30]="working\r\n";
								HAL_UART_Transmit(&huart1, (uint8_t*)tx,30, HAL_MAX_DELAY);

								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);//MOTOR R1
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);//MOTOR R2
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);//MOTOR 3R
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);//MOTOR L1
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);//MOTOR L2
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);//MOTOR L3


								__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,0);//motor 1r
											     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);//motor 2r
											     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);//motor 3r

												    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);//motor 1l
											     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); //motor 2l
											     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,0);//motor 3l
			}
			case 'a': // Turn left
			{
				char tx[30]="working\r\n";
				HAL_UART_Transmit(&huart1, (uint8_t*)tx,30, HAL_MAX_DELAY);
				//HAL_UART_Transmit(&huart1, (uint8_t*)tx, strlen(tx), HAL_MAX_DELAY);

								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);//MOTOR R1
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);//MOTOR R2
								HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);//MOTOR R3
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);//MOTOR L1
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);//MOTOR L2
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1);//MOTOR L3

			for(int i=0;i<=180;i++){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,i);//motor 1r
							     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, i);//motor 2r
							     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, i);//motor 3r

								    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 180-i+100);//motor 1l
							     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 180-i+100); //motor 2l
							     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,180-i+100);//motor 3l
				break;			}
			}
			case 's': // Backward
			{
				char tx[30]="working\r\n";
								HAL_UART_Transmit(&huart1, (uint8_t*)tx,30, HAL_MAX_DELAY);
								//HAL_UART_Transmit(&huart2, (uint8_t*)tx, strlen(tx), HAL_MAX_DELAY);

												HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);//MOTOR R1
											    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);//MOTOR R2
												HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);//MOTOR R3
											    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);//MOTOR L1
												HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);//MOTOR L2
											    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);//MOTOR L3
				for(int i=100;i<=180;i++){
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,i);//motor 1r
												     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, i);//motor 2r
												     	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, i);//motor 3r

													    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,i);//motor 1l
												     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,i); //motor 2l
												     	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,i);//motor 3l


				break;
			}
			}
			default:
				break;
		}
	}
	// Re-arm the UART receiver to listen for the next byte.
	HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

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
#ifdef USE_FULL_ASSERT
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
