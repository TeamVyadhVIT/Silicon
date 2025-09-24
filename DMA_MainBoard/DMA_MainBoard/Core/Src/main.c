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
#include <string.h>

//for debugging
#include <stdarg.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	uint16_t type;
	uint16_t ID;
	uint8_t data[64];
} Message_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MSG_TYPE_PUBLISH 0x1000
#define MSG_TYPE_SUBSCRIBE 0x1100
#define MSG_TOTAL_SIZE sizeof(Message_t)
#define MAX_TOPICS 16


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
static uint8_t UART1_rxBuffer[MSG_TOTAL_SIZE] = {0};
static uint8_t UART2_rxBuffer[MSG_TOTAL_SIZE] = {0};
static uint8_t UART3_rxBuffer[MSG_TOTAL_SIZE] = {0};

static uint8_t UART1_txBuffer[MSG_TOTAL_SIZE] = {0};
static uint8_t UART2_txBuffer[MSG_TOTAL_SIZE] = {0};
static uint8_t UART3_txBuffer[MSG_TOTAL_SIZE] = {0};

Message_t UART1_record[MAX_TOPICS] = {0};
Message_t UART2_record[MAX_TOPICS] = {0};
Message_t UART3_record[MAX_TOPICS] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



//void storeMessage(Message_t* record, Message_t* msg) {
//	for (int i=0;i<MAX_TOPICS;i++) {
//		if (record[i].ID == 0) {
//			memcpy(&record[i], msg, MSG_TOTAL_SIZE);
//
//			//debugging
//			uint8_t debug[64] = {'\0'};
//			int n = sprintf(debug, "stored data in record index %u: ", i);
//			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
//
//			// Print data in hex format
//			for (int i = 0; i < 64; i++) {  // size of msg->data
//				n = sprintf(debug, "%02X ", &record[i]);
//				HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
//			}
//
//			n = sprintf(debug, "\n");
//			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
//
//			//------------//
//
//			break;
//		}
//	}
//}

void storeMessage(Message_t* record, Message_t* msg) {
	for (int i=0;i<MAX_TOPICS;i++) {
		if (record[i].ID == msg->ID) {
			memcpy(&record[i], msg, MSG_TOTAL_SIZE);

			//debugging
			uint8_t debug[64] = {'\0'};
			int n = sprintf(debug, "stored data in record index %u: ", i);
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			// Print data in hex format
			for (int j = 0; j < 64; j++) {  // size of msg->data
				n = sprintf(debug, "%02X ", record[i].data[j]);
				HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
			}

			n = sprintf(debug, "\n");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			//------------//

			break;
		}
		else if (record[i].ID == 0) {
			memcpy(&record[i], msg, MSG_TOTAL_SIZE);

			//debugging
			uint8_t debug[64] = {'\0'};
			int n = sprintf(debug, "stored data in record index %u: ", i);
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			// Print data in hex format
			for (int j = 0; j < 64; j++) {  // size of msg->data
				n = sprintf(debug, "%02X ", record[i].data[j]);
				HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
			}

			n = sprintf(debug, "\n");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			//------------//

			break;
		}
	}
}

Message_t* findMessage(Message_t* record, uint16_t ID) {
	for (int i=0;i<MAX_TOPICS;i++) {
		if (record[i].ID == ID) {

			//debugging
			uint8_t debug[64] = {'\0'};
			int n = sprintf(debug, "found data in record index %u: ", i);
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			// Print data in hex format
			for (int j = 0; j < 64; j++) {  // size of msg->data
				n = sprintf(debug, "%02X ", record[i].data[j]);
				HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
			}

			n = sprintf(debug, "\n");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			//------------//

			return &record[i];
		}
	}
	return NULL;
}

void processMessage(UART_HandleTypeDef* huart,
					uint8_t* rxBuff,
					uint8_t* txBuff,
					Message_t* ownRec,
					Message_t* recA,
					Message_t* recB) {

//	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	Message_t* msg = (Message_t*) rxBuff;

	//debugging
	uint8_t debug[64] = {'\0'};
	int n = sprintf(debug, "processMessage called with message type = %u\n", msg->type); //the 3rd argument is fixing endian-ness -interpreting the incoming message as big-endian
	HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

	n = sprintf(debug, "received data: ");
	HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

	// Print data in hex format
	for (int i = 0; i < 64; i++) {  // size of msg->data
		n = sprintf(debug, "%02X ", msg->data[i]);
		HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
	}

	n = sprintf(debug, "\n");
	HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

	//------------//

	if (msg->type == MSG_TYPE_PUBLISH) {

		//debugging
		uint8_t debug[64] = {'\0'};
		int n = sprintf(debug, "Publishing ID=%u\n", msg->ID);
		HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);


		storeMessage(ownRec, msg);
	}
	else if (msg->type == MSG_TYPE_SUBSCRIBE) {

		//debugging
		uint8_t debug[64] = {'\0'};
		int n = sprintf(debug, "Subscribe request ID=%u\n", msg->ID);
		HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);


		Message_t* found = findMessage(recA, msg->ID);
		if (!found) found = findMessage(recB, msg->ID);

		if (found) {

			//debugging
			uint8_t debug[64] = {'\0'};
			int n = sprintf(debug, "Response ID=%u %s\n", msg->ID, found ? "message found" : "message not found");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			memcpy(txBuff, found, MSG_TOTAL_SIZE);

			HAL_UART_Transmit_DMA(huart, txBuff, MSG_TOTAL_SIZE);

			//more debugging
			n = sprintf(debug, "transmitted data: ");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			// Print data in hex format
			for (int i = 0; i < 64; i++) {  // size of msg->data
				n = sprintf(debug, "%02X ", txBuff[i]);
				HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
			}

			n = sprintf(debug, "\n");
			HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

			//------------//
		}
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        processMessage(&huart1, UART1_rxBuffer, UART1_txBuffer, UART1_record, UART2_record, UART3_record);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1_rxBuffer, MSG_TOTAL_SIZE);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)
    }
    else if (huart->Instance == USART2) {
		//debugging
		uint8_t debug[64] = {0};
		int n = sprintf(debug, "USART2 rx complete callback. log data: Size=%u, huart->RxState=%u \n", Size, huart->RxState);
		HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

        processMessage(&huart2, UART2_rxBuffer, UART2_txBuffer, UART2_record, UART1_record, UART3_record);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2_rxBuffer, MSG_TOTAL_SIZE);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)

    }
    else if (huart->Instance == USART3) {
		//debugging
		uint8_t debug[64] = {0};
		int n = sprintf(debug, "USART3 rx complete callback. log data: Size=%u, huart->RxState=%u \n", Size, huart->RxState);
		HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);

        processMessage(&huart3, UART3_rxBuffer, UART3_txBuffer, UART3_record, UART1_record, UART2_record);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, UART3_rxBuffer, MSG_TOTAL_SIZE);
        __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)
    }

    uint8_t debug[64] = {0};
    int n = sprintf(debug, "\n");
	HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1_rxBuffer, MSG_TOTAL_SIZE);
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2_rxBuffer, MSG_TOTAL_SIZE);
  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, UART3_rxBuffer, MSG_TOTAL_SIZE);
  __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);  //disabling half-transfer interrupts (MUST be called right after initializing DMA)


	//debugging
	uint8_t debug[64] = {'\0'};
	int n = sprintf(debug, "STM32 start debug\n");
	HAL_UART_Transmit(&huart1, debug, n, HAL_MAX_DELAY);
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
