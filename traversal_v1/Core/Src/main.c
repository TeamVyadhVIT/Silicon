/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @brief   CAN RX -> UART (prints "+200-200\\n"), STM32F103 (bxCAN)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include <string.h>

/* Handles -------------------------------------------------------------------*/
CAN_HandleTypeDef  hcan;
UART_HandleTypeDef huart1;

/* Globals (ISR -> main) -----------------------------------------------------*/
volatile uint8_t       rx_ready = 0;     // set in ISR, consumed in main()
CAN_RxHeaderTypeDef    canRx;
uint8_t                can_data[8];

/* Prototypes ----------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);

/* -------------------------------------------------------------------------- */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();

  /* -------- Filter: accept ONLY StdID 0x100 into FIFO0 (bxCAN <<5 packing) - */
  {
    CAN_FilterTypeDef f = {0};
    const uint32_t id   = (0x100U << 5);
    const uint32_t mask = (0x7FFU << 5);

    f.FilterBank           = 0;
    f.FilterMode           = CAN_FILTERMODE_IDMASK;
    f.FilterScale          = CAN_FILTERSCALE_32BIT;
    f.FilterIdHigh         = (uint16_t)(id   >> 16);
    f.FilterIdLow          = (uint16_t)(id   & 0xFFFF);
    f.FilterMaskIdHigh     = (uint16_t)(mask >> 16);
    f.FilterMaskIdLow      = (uint16_t)(mask & 0xFFFF);
    f.FilterFIFOAssignment = CAN_FILTER_FIFO0;   // correct enum for bxCAN
    f.FilterActivation     = ENABLE;
    HAL_CAN_ConfigFilter(&hcan, &f);
  }

  /* ----------------------- Start CAN + enable RX interrupt ------------------ */
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

  /* Optional banner to verify UART first */
  const char hello[] = "UART OK, waiting CAN ID 0x100...\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)hello, sizeof hello - 1, HAL_MAX_DELAY);

  /* --------------------------------- Main loop ------------------------------ */
  while (1)
  {
    if (rx_ready) {
      rx_ready = 0;

      /* Only print exactly 8 ASCII bytes, then newline */
      if (canRx.IDE == CAN_ID_STD && canRx.StdId == 0x100 && canRx.DLC == 8) {
        HAL_UART_Transmit(&huart1, can_data, 8, HAL_MAX_DELAY);     // e.g. "+200-200"
        const uint8_t nl = '\n';
        HAL_UART_Transmit(&huart1, &nl, 1, HAL_MAX_DELAY);
      }
    }
  }
}

/* ============================= Clock config ================================ */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* HSE 8 MHz, PLL x9 -> SYSCLK 72 MHz, APB1 36 MHz, APB2 72 MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                     RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  /* 36 MHz */
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  /* 72 MHz (USART1 clock) */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

/* =============================== CAN init ================================= */
static void MX_CAN_Init(void)
{
  hcan.Instance = CAN1;

  /* 1 Mbps @ APB1=36 MHz:
     36 / (Presc * (1 + BS1 + BS2)) = 1e6
     Presc=2, BS1=15TQ, BS2=2TQ -> 36/(2*(1+15+2)) = 1 MHz */
  hcan.Init.Prescaler            = 2;
  hcan.Init.Mode                 = CAN_MODE_NORMAL;   /* Use LOOPBACK for self-test */
  hcan.Init.SyncJumpWidth        = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1             = CAN_BS1_15TQ;
  hcan.Init.TimeSeg2             = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode    = DISABLE;
  hcan.Init.AutoBusOff           = ENABLE;
  hcan.Init.AutoWakeUp           = ENABLE;
  hcan.Init.AutoRetransmission   = ENABLE;
  hcan.Init.ReceiveFifoLocked    = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK) { Error_Handler(); }

  /* NVIC for CAN1 RX0 IRQ (FIFO0 message pending) */
  HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

/* ============================== USART1 init =============================== */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance        = USART1;
  huart1.Init.BaudRate   = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits   = UART_STOPBITS_1;
  huart1.Init.Parity     = UART_PARITY_NONE;
  huart1.Init.Mode       = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) { Error_Handler(); }
}

/* ================================ GPIO init =============================== */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* PA11 (CAN_RX), PA12 (CAN_TX) are configured in HAL MSP for CAN. */
}

/* ===================== CAN RX callback (ISR context) ====================== */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *h)
{
  if (HAL_CAN_GetRxMessage(h, CAN_RX_FIFO0, &canRx, can_data) == HAL_OK) {
    /* Only flag when it’s exactly the expected payload size; main will print */
    if (canRx.DLC == 8) {
      rx_ready = 1;
    }
  }
}

/* ================================ Error handler =========================== */
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file; (void)line;
}
#endif
