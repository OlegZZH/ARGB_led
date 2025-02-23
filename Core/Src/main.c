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
#include "ARGB.h"
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
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
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
  MX_DMA_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  ARGB_Init();  // Initialization
  ARGB_Clear(); // Clear stirp
  while (!ARGB_Show());


  int button_curent_state = 0;
  int button_color_state =0;
  int button_brightness_state =0;

  ARGB_MODE current_MODE = STATIC;
  int color_R = 255;
  int color_G = 255;
  int color_B = 255;
  int brightness = 100;
  int brightness_direction = 1;
  int brightness_step =2;


  ARGB_SetBrightness(brightness);  // Set global brightness to 40%


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	button_curent_state=HAL_GPIO_ReadPin(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin);
	button_color_state=HAL_GPIO_ReadPin(COLOR_BUTTON_GPIO_Port, COLOR_BUTTON_Pin);
	button_brightness_state=HAL_GPIO_ReadPin(BRIGHTNESS_BUTTON_GPIO_Port, BRIGHTNESS_BUTTON_Pin);


	if (button_curent_state){
		if (current_MODE < ITERATION) {
		    current_MODE++;
		}
		else{
			current_MODE=0;
		}
		HAL_Delay(300);
	}else if (button_color_state){
		ARGB_Clear();
		while (!ARGB_Show());
		calculateRainbowColor(&color_R, &color_G, &color_B);
		ARGB_FillRGB(color_R, color_G, color_B);
		while (!ARGB_Show());
	}else if (button_brightness_state){
		ARGB_Clear();
		while (!ARGB_Show());
        brightness += brightness_step * brightness_direction;

        if (brightness >= 100) {
            brightness = 100;
            brightness_direction = -1;
        } else if (brightness <= 0) {
            brightness = 0;
            brightness_direction = 1;
        }
		ARGB_SetBrightness(brightness);
		ARGB_FillRGB(color_R, color_G, color_B);
		while (!ARGB_Show());
	}else{

		switch(current_MODE){
		case STATIC:
			ARGB_Clear();
			while (!ARGB_Show());
			ARGB_FillRGB(color_R, color_G, color_B);
			while (!ARGB_Show());
			break;
		case RAINBOW:
			ARGB_Clear();
			while (!ARGB_Show());
			calculateRainbowColor(&color_R, &color_G, &color_B);
			ARGB_FillRGB(color_R, color_G, color_B);
			while (!ARGB_Show());
			break;
		case SLOW_BLINK:
			ARGB_Clear();
			while (!ARGB_Show());
            brightness += brightness_step * brightness_direction;

            if (brightness >= 100) {
                brightness = 100;
                brightness_direction = -1;
            } else if (brightness <= 0) {
                brightness = 0;
                brightness_direction = 1;
            }
			ARGB_SetBrightness(brightness);
			ARGB_FillRGB(color_R, color_G, color_B);
			while (!ARGB_Show());
			break;
		case ITERATION:
			for (uint8_t i = 0; i < NUM_PIXELS; i++){
				button_curent_state=HAL_GPIO_ReadPin(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin);
				button_color_state=HAL_GPIO_ReadPin(COLOR_BUTTON_GPIO_Port, COLOR_BUTTON_Pin);
				button_brightness_state=HAL_GPIO_ReadPin(BRIGHTNESS_BUTTON_GPIO_Port, BRIGHTNESS_BUTTON_Pin);
				if (button_curent_state || button_color_state || button_brightness_state){
					break;
				}
				ARGB_Clear();
			  	while (!ARGB_Show());
			  	ARGB_SetRGB(i,color_R, color_G, color_B);
			  	while (!ARGB_Show());
			  	HAL_Delay(100);
			}
			break;
		}
	}


	HAL_Delay(50);

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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : BRIGHTNESS_BUTTON_Pin COLOR_BUTTON_Pin MODE_BUTTON_Pin */
  GPIO_InitStruct.Pin = BRIGHTNESS_BUTTON_Pin|COLOR_BUTTON_Pin|MODE_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void calculateRainbowColor(int *color_R, int *color_G, int *color_B) {
    static int color_mode = 0;
    static int step = 5;

    switch (color_mode) {
    case 0: // Red to Yellow (Increase Green)
		*color_G += 5;
		if (*color_G >= 255) {
			*color_G = 255;
			color_mode = 1;
		}
		break;
	case 1: // Yellow to Green (Decrease Red)
		*color_R -= 5;
		if (*color_R <= 0) {
			*color_R = 0;
			color_mode = 2;
		}
		break;
	case 2: // Green to Cyan (Increase Blue)
		*color_B += 5;
		if (*color_B >= 255) {
			*color_B = 255;
			color_mode = 3;
		}
		break;
	case 3: // Cyan to Blue (Decrease Green)
		*color_G -= 5;
		if (*color_G <= 0) {
			*color_G = 0;
			color_mode = 4;
		}
		break;
	case 4: // Blue to Magenta (Increase Red)
		*color_R += 5;
		if (*color_R >= 255) {
			*color_R = 255;
			color_mode = 5;
		}
		break;
	case 5: // Magenta to Red (Decrease Blue)
		*color_B -= 5;
		if (*color_B <= 0) {
			*color_B = 0;
			color_mode = 0;
		}
		break;
    }
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
