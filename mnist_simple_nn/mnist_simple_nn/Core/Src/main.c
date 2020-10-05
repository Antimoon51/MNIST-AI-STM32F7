/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "i2c.h"
#include "quadspi.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	float prob;
	uint8_t id;
}id_prob;

TS_StateTypeDef  TS_State;


ai_float in_data[28][28]= {{0}};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INPUT_IMAGE_WIDTH						28
#define INPUT_IMAGE_HEIGHT 						28
#define NUM_CLASSES								10
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
static void MPU_ConfigPSRAM(void);
void processingdata(void);
void draw_touch_position(void);
void Draw_Cluster_Image(void);
void Reset_Pred(ai_float** in_data, id_prob* first_guess, id_prob* second_guess);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Reset_Pred(ai_float** in_data, id_prob* first_guess, id_prob* second_guess){
	memset(in_data, 0.0, sizeof(in_data[0][0]) * INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT);
	second_guess->id = 0;
	second_guess->prob = 0.0;
	first_guess->id = 0;
	first_guess->prob = 0.0;

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
		MPU_ConfigPSRAM();
  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	BSP_LCD_Init();
	BSP_PB_Init(BUTTON_WAKEUP,BUTTON_MODE_GPIO);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  //MX_ADC1_Init();
  //MX_ADC2_Init();
  //MX_ADC3_Init();
  MX_CRC_Init();
  //MX_FMC_Init();
  //MX_I2C1_Init();
  //MX_I2C2_Init();
  //MX_I2C3_Init();
  //MX_QUADSPI_Init();
  //MX_SAI2_Init();
  //MX_SPI1_Init();
  //MX_SPI2_Init();
  //MX_TIM2_Init();
  //MX_TIM3_Init();
  //MX_TIM5_Init();
  //MX_TIM9_Init();
  //MX_TIM12_Init();
  //MX_UART4_Init();
  //MX_UART5_Init();
  //MX_UART7_Init();
  //MX_USART2_UART_Init();
  //MX_USART6_UART_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */
	uint8_t ts_status = 1;
	while(ts_status != 0){
		ts_status = Touchscreen_Calibration();
	}
	
	id_prob first_guess;
  id_prob second_guess;
  first_guess.prob = 0.0;
  second_guess.prob = 0.0;
  char first_guess_str[12];
  char second_guess_str[12];
  ai_float out_data[NUM_CLASSES];
	
	
	BSP_LCD_Clear(LCD_COLOR_BLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0,0,(uint8_t * ) ("Simple NN"), CENTER_MODE);
	
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillRect(22, 22, 196, 196);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		draw_touch_position();
		if(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET){
			HAL_Delay(10);
			while(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET);		// Wait for Button release (do (nothing)while(buttonpressed);)

			processingdata();
			HAL_Delay(500);
			

	
				aiRun(in_data, out_data);
				for(int i = 0; i < NUM_CLASSES; ++i){
					if(first_guess.prob < out_data[i]){
						second_guess.id = first_guess.id;
						second_guess.prob = first_guess.prob;
						first_guess.prob = out_data[i];
						first_guess.id = i;
					}else if(second_guess.prob < out_data[i]){
						second_guess.id = i;
						second_guess.prob = out_data[i];
					}
		}
		sprintf(first_guess_str ,"%d (%.4f)", first_guess.id , first_guess.prob);
		sprintf(second_guess_str,"%d (%.4f)", second_guess.id , second_guess.prob);
		BSP_LCD_SetFont(&Font12);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		BSP_LCD_DisplayStringAtLine(0, (uint8_t *) first_guess_str);
		BSP_LCD_DisplayStringAtLine(1, (uint8_t *) second_guess_str);
		Reset_Pred((ai_float ** )in_data,&first_guess,&second_guess);
		while(BSP_PB_GetState(BUTTON_WAKEUP) ==RESET){
						HAL_Delay(100);  											
					}
		while(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET);		// Wait for Button release (do (nothing)while(buttonpressed);)

		BSP_LCD_Clear(LCD_COLOR_BLUE);
		BSP_LCD_DisplayStringAt(0,0,(uint8_t * ) ("Simple NN"), CENTER_MODE);
	
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_FillRect(22, 22, 196, 196);
			}
  /* USER CODE END WHILE */
  
  //MX_X_CUBE_AI_Process();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART6
                              |RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_UART5
                              |RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_SAI2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_I2C3;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_WAKEUP) != RESET)
  {
    HAL_Delay(10);
    while (BSP_PB_GetState(BUTTON_WAKEUP) != RESET);
    return 1 ;
  }
  return 0;
}


/**
  * @brief  Configure the MPU attributes as Write Back for PSRAM mapped on FMC
  *         BANK1/BANK2. This function configures two regions.
  * @note   The Base Address is 0x64000000 (for Display purposes) and 
  *         0x60000000 (for Audio Record module).
  *         The Region Size is 512KB, it is related to PSRAM memory size.
  * @param  None
  * @retval None
  */
static void MPU_ConfigPSRAM(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes for PSRAM with recomended configurations:
     Normal memory, Shareable, write-back (Display purposes) */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x64000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  
  /* Configure the MPU attributes for PSRAM with recomended configurations:
     Normal memory, Shareable, write-back */
  MPU_InitStruct.BaseAddress = 0x60000000;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
void draw_touch_position(void) {
    /* Get X and Y position of the first touch post calibrated */
#define FootprintRadius 10
#define Border 22
    uint8_t Errortext[] = "Error: Please try again!";																	//define Errormessage for leaving the draw area
    uint8_t ts_status = BSP_TS_GetState(&TS_State);																		//check touchscreen(ts) status
    if(TS_State.touchDetected) {																											//if(touch detechtet){get koordinates of touch; store in (x,y);}

        uint16_t x = TouchScreen_Get_Calibrated_X(TS_State.touchX[0]);
        uint16_t y = TouchScreen_Get_Calibrated_Y(TS_State.touchY[0]);

        if((y > (Border + FootprintRadius)) &&																				//if(touchpoint + Footprint within Border){Draw circle @touchpont(width = Footprintradius);}
                (y < (BSP_LCD_GetYSize() - Border - FootprintRadius)) &&
                (x > (Border + FootprintRadius)) &&
                (x < (BSP_LCD_GetXSize() - Border - FootprintRadius)))
        {
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            BSP_LCD_FillCircle(x, y, FootprintRadius);
        } else {																																			//else{printf(Errormessage);}
            BSP_LCD_Clear(LCD_COLOR_RED);
            BSP_LCD_SetTextColor(LCD_COLOR_RED);
            BSP_LCD_DisplayStringAt(0, 120, Errortext, CENTER_MODE);
          while(BSP_PB_GetState(BUTTON_WAKEUP) ==RESET){
						HAL_Delay(100);  											
					}
					while(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET);		// Wait for Button release (do (nothing)while(buttonpressed);)
					
					BSP_LCD_Clear(LCD_COLOR_BLUE);
					BSP_LCD_DisplayStringAt(0,0,(uint8_t * ) ("Simple NN"), CENTER_MODE);
	
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(22, 22, 196, 196);
					
        }
    }
}

void processingdata(void){
	uint16_t x = 0;		//x- und y Werte der oberen linken Ecke der einzelnen Clusterfl�chen in Pixel
	uint16_t y = 0;
	uint8_t k = 0;		//Z�hlvariable f�r Pixel pro Cluster
	#define Border 22
	
	for(x = Border; x < 240-Border; x += 7){																	//begin @Border, end @Border, take steps of Cluster-Width
		for(y = Border; y < 240-Border; y += 7){																//begin @Border, end @Border, take steps of Cluster-Hight
			for(int i = 0; i < 7; i++){																						//count through Cluster-Width
				for(int j = 0; j < 7; j++){																					//count through Cluster Hight
						if(BSP_LCD_ReadPixel(x+i, y+j) == LCD_COLOR_BLACK){							//Get Pixel color state; if( Pixelcolorstate == Black){k++;}
						k++;
					}
				}
				
				}																																			//after each Cluster: check for Pixel Amount in Cluster, color Cluster accordingly
			if(k >= 10 && k <= 20){																									//skipping white due to default color
				BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
				BSP_LCD_FillRect(x, y, 7, 7);
			}else if(k >= 10 && k < 30){
				BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
				BSP_LCD_FillRect(x, y, 7, 7);
			}else if(k >= 10 && k < 40){
				BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
				BSP_LCD_FillRect(x, y, 7, 7);
			}else if(k >= 10 && k <= 49){
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_FillRect(x, y, 7, 7);
			}
			in_data[(y-Border)/7][(x-Border)/7] = k / 49.0f;											//writing Cluster-Value to image Matrix
		
			k = 0;																																//restet counter for next Cluster
		}
				
	}
	
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
