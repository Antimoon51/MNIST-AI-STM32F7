/**
  ******************************************************************************
  * @file    BSP/Src/main.c 
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32412G_DISCOVERY BSP Drivers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stlogo.h"
#include "nnom.h"
#include "weights.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
TS_StateTypeDef  TS_State = {0};

typedef struct{
	float prob;
	uint8_t id;
}id_prob;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t DemoIndex = 0;
uint8_t NbLoop = 1;
uint32_t i, PsramTest = 0; 
/* Global variables ----------------------------------------------------------*/
uint8_t SDDetectIT = 0;
uint8_t* input;
/* Global extern variables ---------------------------------------------------*/
#ifndef USE_FULL_ASSERT
uint16_t ErrorCounter = 0;
#endif
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static void MPU_ConfigPSRAM(void);
static void CPU_CACHE_Enable(void);

void processingdata(void);
void draw_touch_position(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the MPU attributes for PSRAM external memory */
  MPU_ConfigPSRAM();
  
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  
  /* STM32F7xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 216 Mhz */
  SystemClock_Config();

  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6); 
 
  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
 
  /*##-1- Initialize the LCD #################################################*/
  /* Initialize the LCD */
  BSP_LCD_Init();

  /* To choose the portrait orientation uncomment the below line. Note that the
     TS should be also modified to portrait orientation and this by uncommenting */
     /* ts_status = BSP_TS_InitEx(BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), TS_ORIENTATION_PORTRAIT); */
  /* in ts_calibration.c file */
  
  /* BSP_LCD_InitEx(LCD_ORIENTATION_PORTRAIT); */
  //Display_DemoDescription();
	
	BSP_LCD_Clear(LCD_COLOR_BLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0,0,(uint8_t * ) ("densenet"), CENTER_MODE);
	
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillRect(22, 22, 196, 196);
	
	id_prob first_guess;
  id_prob second_guess;
  first_guess.prob = 0.0;
  second_guess.prob = 0.0;
  char first_guess_str[12];
  char second_guess_str[12];
	
	nnom_model_t* model;
	
	
	model = nnom_model_create();
	
  /* Wait For User inputs */
  while (1)
  {
    draw_touch_position();
		if(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET){
			HAL_Delay(10);
			while(BSP_PB_GetState(BUTTON_WAKEUP) !=RESET);		// Wait for Button release (do (nothing)while(buttonpressed);)

			processingdata();
			HAL_Delay(500);
      
			memcpy(nnom_input_data, input, sizeof(nnom_input_data));
			model_run(model);
			
			for(int i = 0; i < 10; ++i){
					if(first_guess.prob < nnom_output_data[i]){
						second_guess.id = first_guess.id;
						second_guess.prob = first_guess.prob;
						first_guess.prob = nnom_output_data[i];
						first_guess.id = i;
					}else if(second_guess.prob < nnom_output_data[i]){
						second_guess.id = i;
						second_guess.prob = nnom_output_data[i];
					}
		}
    }
    Toggle_Leds();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
    /* Insert a delay */
    HAL_Delay(50);
  }
}

/**
  * @brief  Drawing pixels on Touchscreen, where touch is detected
  * @param  None
  * @retval None
  */

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
					BSP_LCD_DisplayStringAt(0,0,(uint8_t * ) ("CNN"), CENTER_MODE);
					BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
					BSP_LCD_FillRect(22, 22, 196, 196);
					
        }
    }
}

/**
  * @brief  Reads the Touchscreen set pixels in 28x28 matrix
  * @param  None
  * @retval None
  */

void processingdata(void){
	uint16_t x = 0;		//x- und y Werte der oberen linken Ecke der einzelnen Clusterfl?chen in Pixel
	uint16_t y = 0;
	uint8_t k = 0;		//counter for Pixel / Cluster
	#define Border 22
	int l = 0;
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
			input[l] = k / 49.0f * 255;											//writing Cluster-Value to input vector.
			l++;
		
			k = 0;																																//restet counter for next Cluster
		}
				
	}
	
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
 
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
}


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
  * @brief  Toggle Leds.
  * @param  None
  * @retval None
  */
void Toggle_Leds(void)
{
  BSP_LED_Toggle(LED6);
  HAL_Delay(50);
}


/**
  * @brief EXTI line detection callbacks.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t debounce_time = 0;

  if(GPIO_Pin == BUTTON_WAKEUP)
  {
    /* Prevent debounce effect for user key */
    if((HAL_GetTick() - debounce_time) > 50)
    {
      debounce_time = HAL_GetTick();
    }  
  }
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

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
