/* SLAVE main.c - replace your Core/Src/main.c with this file
   Assumptions:
   - MX_GPIO_Init(), MX_SPI1_Init(), MX_USART1_UART_Init() exist (CubeMX).
   - SPI1 configured Mode0, NSS software.
   - DIO0_Pin configured as EXTI in CubeMX.
   - LoRa.c / LoRa.h present and unchanged.
*/

#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "LoRa.h"
#include <string.h>
#include <stdio.h>

LoRa myLoRa;
volatile uint8_t rx_flag = 0;
uint8_t rxBuf[128];

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;

/* forward prototypes */
void SystemClock_Config(void);
void Error_Handler(void);

/* EXTI callback used by HAL when DIO0 line rises */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DIO0_Pin) {
        rx_flag = 1;
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();

    HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE BOOT\r\n", 12, 200);

    /* LoRa setup */
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
    myLoRa.bandWidth = BW_125KHz;
    myLoRa.crcRate = CR_4_5;
    myLoRa.power = POWER_14db;
   // myLoRa.overCurrentProtection = 120;
    myLoRa.preamble = 8;

    /* Reset module and diagnostic */
    HAL_GPIO_WritePin(myLoRa.reset_port, myLoRa.reset_pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(myLoRa.reset_port, myLoRa.reset_pin, GPIO_PIN_SET);
    HAL_Delay(50);

    uint8_t ver = LoRa_read(&myLoRa, RegVersion);
    {
        char out[64];
        int n = snprintf(out, sizeof(out), "SLAVE: RegVersion=0x%02X\r\n", ver);
        HAL_UART_Transmit(&huart1, (uint8_t*)out, n, 200);
    }

    if (LoRa_init(&myLoRa) == LORA_OK) {
        HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: LoRa init OK\r\n", 22, 200);
    } else {
        HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: LoRa init FAIL\r\n", 24, 200);
        /* continue for debug */
    }

    LoRa_setSyncWord(&myLoRa, 0xF3);
    LoRa_setPower(&myLoRa, POWER_14db);
    LoRa_setOCP(&myLoRa, 120);

    LoRa_startReceiving(&myLoRa);

    /* Main loop: on RX flag, read packet, print hex, delay, transmit reply twice */
    while (1)
    {
        if (rx_flag) {
            rx_flag = 0;
            memset(rxBuf, 0, sizeof(rxBuf));
            int16_t len = LoRa_receive(&myLoRa, rxBuf, (uint8_t)sizeof(rxBuf));
            if (len > 0) {
                /* Print received bytes in HEX with timestamp */
                char dbg[80];
                int m = snprintf(dbg, sizeof(dbg), "SLAVE: RX <- ts=%lu len=%d ", (unsigned long)HAL_GetTick(), len);
                HAL_UART_Transmit(&huart1, (uint8_t*)dbg, m, 200);
                for (int i = 0; i < len; ++i) {
                    char tmp[6];
                    int n = snprintf(tmp, sizeof(tmp), "%02X ", rxBuf[i]);
                    HAL_UART_Transmit(&huart1, (uint8_t*)tmp, n, 200);
                }
                HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 200);

                /* Delay to allow master to enter RX state (tune if needed) */
                HAL_Delay(200);  /* start with 200 ms; increase to 300 ms if master still misses */

                /* Build reply: header + 2-byte RPM */
                uint16_t rpm = 345; /* replace with real sensor read */
                uint8_t reply[3];
                reply[0] = 0xA1;
                reply[1] = (uint8_t)(rpm & 0xFF);
                reply[2] = (uint8_t)((rpm >> 8) & 0xFF);

                /* Send reply twice to increase chance of being received */
                HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: TX -> reply (1)\r\n", 24, 200);
                uint8_t ok = LoRa_transmit(&myLoRa, reply, 3, 500);
                if (!ok) HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: TX ERR (1)\r\n", 19, 200);

                HAL_Delay(30);

                HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: TX -> reply (2)\r\n", 24, 200);
                ok = LoRa_transmit(&myLoRa, reply, 3, 500);
                if (!ok) HAL_UART_Transmit(&huart1, (uint8_t*)"SLAVE: TX ERR (2)\r\n", 19, 200);

                /* Return to RX */
                LoRa_startReceiving(&myLoRa);
            } else {
                /* Defensive: re-enter RX if no bytes read */
                LoRa_startReceiving(&myLoRa);
            }
        }
        HAL_Delay(1);
    }
}

/* ---------- SystemClock_Config and Error_Handler ---------- */
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
