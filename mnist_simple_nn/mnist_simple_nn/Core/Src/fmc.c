/**
  ******************************************************************************
  * File Name          : FMC.c
  * Description        : This file provides code for the configuration
  *                      of the FMC peripheral.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "fmc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SRAM_HandleTypeDef hsram1;
SRAM_HandleTypeDef hsram2;

/* FMC initialization function */
void MX_FMC_Init(void)
{
  FMC_NORSRAM_TimingTypeDef Timing = {0};

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FMC_NORSRAM_DEVICE;
  hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FMC_MEMORY_TYPE_PSRAM;
  hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
  hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Perform the SRAM2 memory initialization sequence
  */
  hsram2.Instance = FMC_NORSRAM_DEVICE;
  hsram2.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram2.Init */
  hsram2.Init.NSBank = FMC_NORSRAM_BANK2;
  hsram2.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram2.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  hsram2.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram2.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram2.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram2.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram2.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  hsram2.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram2.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram2.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram2.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram2.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram2.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
  hsram2.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram2, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PD7   ------> FMC_NE1
  PB7   ------> FMC_NL
  PD0   ------> FMC_D2
  PG9   ------> FMC_NE2
  PD5   ------> FMC_NWE
  PD1   ------> FMC_D3
  PD4   ------> FMC_NOE
  PF0   ------> FMC_A0
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PG5   ------> FMC_A15
  PG4   ------> FMC_A14
  PG3   ------> FMC_A13
  PD15   ------> FMC_D1
  PG2   ------> FMC_A12
  PG1   ------> FMC_A11
  PD14   ------> FMC_D0
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE13   ------> FMC_D10
  PD12   ------> FMC_A17
  PD11   ------> FMC_A16
  PD10   ------> FMC_D15
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF14   ------> FMC_A8
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  */
  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = PSRAM_NBL1_Pin|PSRAM_NBL0_Pin|LCD_PSRAM_D10_Pin|LCD_PSRAM_D5_Pin
                          |LCD_PSRAM_D6_Pin|LCD_PSRAM_D8_Pin|LCD_PSRAM_D11_Pin|LCD_PSRAM_D4_Pin
                          |LCD_PSRAM_D7_Pin|LCD_PSRAM_D9_Pin|LCD_PSRAM_D12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = PSRAM_NE1_Pin|LCD_PSRAM_D2_Pin|LCD_PSRAM_NWE_Pin|LCD_PSRAM_D3_Pin
                          |LCD_PSRAM_NWED4_Pin|LCD_PSRAM_D1_Pin|LCD_PSRAM_D0_Pin|PSRAM_A17_Pin
                          |PSRAM_A16_Pin|LCD_PSRAM_D15_Pin|LCD_PSRAM_D14_Pin|LCD_PSRAM_D13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = NC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(NC1_GPIO_Port, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = LCD_NE_Pin|PSRAM_A15_Pin|PSRAM_A14_Pin|PSRAM_A13_Pin
                          |PSRAM_A12_Pin|PSRAM_A11_Pin|PSRAM_A10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = PSRAM_A0_Pin|PSRAM_A2_Pin|PSRAM_A1_Pin|PSRAM_A3_Pin
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A7_Pin|PSRAM_A6_Pin
                          |PSRAM_A9_Pin|PSRAM_A8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* sramHandle){
  /* USER CODE BEGIN SRAM_MspInit 0 */

  /* USER CODE END SRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SRAM_MspInit 1 */

  /* USER CODE END SRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PD7   ------> FMC_NE1
  PB7   ------> FMC_NL
  PD0   ------> FMC_D2
  PG9   ------> FMC_NE2
  PD5   ------> FMC_NWE
  PD1   ------> FMC_D3
  PD4   ------> FMC_NOE
  PF0   ------> FMC_A0
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PG5   ------> FMC_A15
  PG4   ------> FMC_A14
  PG3   ------> FMC_A13
  PD15   ------> FMC_D1
  PG2   ------> FMC_A12
  PG1   ------> FMC_A11
  PD14   ------> FMC_D0
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE13   ------> FMC_D10
  PD12   ------> FMC_A17
  PD11   ------> FMC_A16
  PD10   ------> FMC_D15
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF14   ------> FMC_A8
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  */

  HAL_GPIO_DeInit(GPIOE, PSRAM_NBL1_Pin|PSRAM_NBL0_Pin|LCD_PSRAM_D10_Pin|LCD_PSRAM_D5_Pin
                          |LCD_PSRAM_D6_Pin|LCD_PSRAM_D8_Pin|LCD_PSRAM_D11_Pin|LCD_PSRAM_D4_Pin
                          |LCD_PSRAM_D7_Pin|LCD_PSRAM_D9_Pin|LCD_PSRAM_D12_Pin);

  HAL_GPIO_DeInit(GPIOD, PSRAM_NE1_Pin|LCD_PSRAM_D2_Pin|LCD_PSRAM_NWE_Pin|LCD_PSRAM_D3_Pin
                          |LCD_PSRAM_NWED4_Pin|LCD_PSRAM_D1_Pin|LCD_PSRAM_D0_Pin|PSRAM_A17_Pin
                          |PSRAM_A16_Pin|LCD_PSRAM_D15_Pin|LCD_PSRAM_D14_Pin|LCD_PSRAM_D13_Pin);

  HAL_GPIO_DeInit(NC1_GPIO_Port, NC1_Pin);

  HAL_GPIO_DeInit(GPIOG, LCD_NE_Pin|PSRAM_A15_Pin|PSRAM_A14_Pin|PSRAM_A13_Pin
                          |PSRAM_A12_Pin|PSRAM_A11_Pin|PSRAM_A10_Pin);

  HAL_GPIO_DeInit(GPIOF, PSRAM_A0_Pin|PSRAM_A2_Pin|PSRAM_A1_Pin|PSRAM_A3_Pin
                          |PSRAM_A4_Pin|PSRAM_A5_Pin|PSRAM_A7_Pin|PSRAM_A6_Pin
                          |PSRAM_A9_Pin|PSRAM_A8_Pin);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* sramHandle){
  /* USER CODE BEGIN SRAM_MspDeInit 0 */

  /* USER CODE END SRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SRAM_MspDeInit 1 */

  /* USER CODE END SRAM_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
