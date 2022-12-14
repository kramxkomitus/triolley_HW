/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rgb_led.h"
#include "stdlib.h"
#include "stdio.h"
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
uint32_t answer = 0;
uint8_t sim = 0;
uint8_t console_flag = 0;
uint8_t console_buff[100] = {};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  for (int i = 0; i < len; i++)
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr++, 1, 100);
  return len;
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
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_TIM_Base_Start_IT(&htim4);
  HAL_UART_Receive_IT(&huart1, &sim, 1);
  uint8_t test = 0;

  HAL_Delay(100);
  ws2812_clean();
  uint8_t color[3] = {};
  uint8_t *color_str[3] = {};
  while (1)
  {

    if (console_flag)
    {
      uint8_t j = 0;
      uint8_t flag = 0;
      console_flag = 0;
      if (*console_buff == 'w')
      {
        for (int i = 1; i < strlen(console_buff); i++)
        {
          if (console_buff[i] != ' ')
          {
            if (!flag)
            {
              color_str[j++] = &console_buff[i];
              flag = 1;
            }
          }
          else
            flag = 0;
        }
        color[0] = atoi(color_str[0]);
        color[1] = atoi(color_str[1]);
        color[2] = atoi(color_str[2]);
        //printf("dick --> R:%d; G:%d, B:%d.\r\n", color[0], color[1], color[2]);
      }
    }

    for (uint16_t i = 0; i < 22; i++)
      ws2812_pixel_set(color[0], color[1], color[2], i);
    ws2812_pixel_show();

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(100);

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    static uint8_t i = 0;
    if (sim == '\r')
    {
      console_buff[i] = '\0';
      console_flag = 1;
      i = 0;
      //HAL_UART_Transmit(&huart1, "\r\n", 2, 100);
    }
    else
    {
      console_buff[i++] = sim;
      //HAL_UART_Transmit(&huart1, &sim, 1, 100);
    }

    HAL_UART_Receive_IT(&huart1, &sim, 1);
  }
}

enum sonar_state
{
  bisy,
  trig,
  wait,
  echo
} s_state = bisy;

uint32_t count = 10 * 1000; // 10ms
uint32_t duration = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4)
  {
    switch (s_state)
    {
    case bisy:
      if (count-- == 0)
      {
        count = 10; // 10 mks
        s_state = trig;
      }
      break;

    case trig:
      HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 1);
      if (count-- == 0)
      {
        count = 10 * 1000; // 10 ms
        s_state = wait;
      }
      break;

    case wait:
      HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 0);
      if (HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == 1)
        s_state = echo;
      break;

    case echo:
      duration++;
      if (HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == 0)
      {
        s_state = bisy;
        answer = (duration / 2) / 29;
        duration = 0;
      }
      break;

    default:
      break;
    }
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
