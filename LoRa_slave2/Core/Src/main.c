/* SLAVE – Fast 128B RX/TX (SX1278, RA-02) */

#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "LoRa.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;

LoRa myLoRa;
uint8_t rxBuf[128];
uint8_t txBuf[128];
volatile uint8_t rxDone = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DIO0_Pin) rxDone = 1;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();

    HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE BOOT\r\n", 12, 100);

    myLoRa = newLoRa();
    myLoRa.CS_port = NSS_GPIO_Port;
    myLoRa.CS_pin  = NSS_Pin;
    myLoRa.reset_port = RST_GPIO_Port;
    myLoRa.reset_pin  = RST_Pin;
    myLoRa.DIO0_port  = DIO0_GPIO_Port;
    myLoRa.DIO0_pin   = DIO0_Pin;
    myLoRa.hSPIx = &hspi1;

    myLoRa.frequency = 433;
    myLoRa.spredingFactor = SF_7;
    myLoRa.bandWidth = BW_500KHz;
    myLoRa.crcRate = CR_4_5;
    myLoRa.power = POWER_14db;
    myLoRa.preamble = 6;

    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);

    if (LoRa_init(&myLoRa) != LORA_OK) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"LoRa init FAIL\r\n", 16, 100);
        while (1);
    }

    LoRa_setSyncWord(&myLoRa, 0xF3);
    LoRa_startReceiving(&myLoRa);

    for (int i = 0; i < 128; i++) txBuf[i] = 255 - i;

    while (1)
    {
        if (rxDone) {
            rxDone = 0;
            int len = LoRa_receive(&myLoRa, rxBuf, 128);

            char msg[40];
            int n = sprintf(msg, "SLAVE RX %d bytes\r\n", len);
            HAL_UART_Transmit(&huart1, (uint8_t*)msg, n, 100);

            //HAL_Delay(20);  // allow master RX settle

            LoRa_transmit(&myLoRa, txBuf, 128, 1000);
            LoRa_startReceiving(&myLoRa);
        }
    }
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}
