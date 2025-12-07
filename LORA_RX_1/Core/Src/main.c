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
#include "spi.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LoRa.h"
#include <stdio.h>
#include <string.h>
// no need for string.h here, we don't use strlen
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

/* USER CODE BEGIN PV */
LoRa myLoRa;
uint16_t LoRa_stat = 0;
uint8_t RxBuffer[128];
const char *debug = "lora started successfully";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
	  /* MCU Configuration--------------------------------------------------------*/
	  HAL_Init();
	  SystemClock_Config();

	  /* Initialize all configured peripherals */
	  MX_GPIO_Init();
	  MX_SPI1_Init();
	  MX_USART1_UART_Init();
  /* USER CODE END 1 */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */


  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */

	  // 🔹 1) Prove UART + code are alive
	    HAL_UART_Transmit(&huart1, (uint8_t*)"BOOT RX\r\n", 9, 100);

	    myLoRa = newLoRa();

	    myLoRa.CS_port         = NSS_GPIO_Port;
	    myLoRa.CS_pin          = NSS_Pin;
	    myLoRa.reset_port      = RST_GPIO_Port;
	    myLoRa.reset_pin       = RST_Pin;
	    myLoRa.DIO0_port       = DIO0_GPIO_Port;
	    myLoRa.DIO0_pin        = DIO0_Pin;
	    myLoRa.hSPIx           = &hspi1;

	    myLoRa.frequency             = 433;             // default = 433 MHz
	    myLoRa.spredingFactor        = SF_7;            // default = SF_7
	    myLoRa.bandWidth             = BW_125KHz;       // default = BW_125KHz
	    myLoRa.crcRate               = CR_4_5;          // default = CR_4_5
	    myLoRa.power                 = POWER_17db;      // default = 20db
	    myLoRa.overCurrentProtection = 100;             // default = 100 mA
	    myLoRa.preamble              = 8;               // default = 8;

	    if(LoRa_init(&myLoRa)==LORA_OK){
	        LoRa_stat=1;
	        HAL_UART_Transmit(&huart1, (uint8_t*)"LoRa init OK\r\n", 14, 100);
	    } else {
	        HAL_UART_Transmit(&huart1, (uint8_t*)"LoRa init FAIL\r\n", 16, 100);
	    }

	    LoRa_startReceiving(&myLoRa);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t*)"loop\r\n", 6, 100);

	      int16_t len = LoRa_receive(&myLoRa, RxBuffer, sizeof(RxBuffer));
	      if (len > 0)
	      {
	          // 1) Print the received payload
	          HAL_UART_Transmit(&huart1, (uint8_t*)"Packet: ", 8, 100);
	          HAL_UART_Transmit(&huart1, RxBuffer, len, 100);

	          // 2) Get RSSI of the last packet
	          int rssi = LoRa_getRSSI(&myLoRa);   // ✅ IMPORTANT: pass &myLoRa

	          // 3) Print RSSI as text
	          char buf[32];
	          int n = snprintf(buf, sizeof(buf), "  RSSI=%d dBm\r\n", rssi);
	          if (n > 0 && n < (int)sizeof(buf)) {
	              HAL_UART_Transmit(&huart1, (uint8_t*)buf, n, 100);
	          } else {
	              // Fallback newline if snprintf fails
	              uint8_t nl[2] = "\r\n";
	              HAL_UART_Transmit(&huart1, nl, 2, 100);
	          }
	      }

	      HAL_Delay(500);


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

// 🔴 EXTI callback no longer needed; you can delete or keep it commented
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DIO0_Pin)
    {
        int16_t len = LoRa_recieve(&myLoRa, RxBuffer, sizeof(RxBuffer));
        if (len > 0)
        {
            HAL_UART_Transmit(&huart1, RxBuffer, len, 100);
            uint8_t newline[2] = "\r\n";
            HAL_UART_Transmit(&huart1, newline, 2, 100);
        }
    }
}
*/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
