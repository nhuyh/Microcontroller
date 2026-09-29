/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 1 Exercise 6 - 12 LEDs Clock (Quay thuận chiều)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

// Hàm tắt tất cả 12 LED (PA4 đến PA15)
void clearAllClock(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                             GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                             GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
}

// Bật 1 vị trí LED tương ứng (num từ 1 đến 12)
// Số 1 -> PA4, Số 2 -> PA5, ..., Số 12 -> PA15
void setNumberOnClock(int num) {
    if (num < 1 || num > 12) return;

    if (num == 12) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOA, (uint16_t)(1 << (num + 3)), GPIO_PIN_SET);
    }
}

// Tắt 1 vị trí LED tương ứng
void clearNumberOnClock(int num) {
    if (num < 1 || num > 12) return;

    if (num == 12) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOA, (uint16_t)(1 << (num + 3)), GPIO_PIN_RESET);
    }
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // Khởi tạo đếm từ 1
  int counter = 1;

  while (1)
  {
      /* USER CODE BEGIN WHILE */

      // Đếm tăng từ 1 đến 12 (Thuận chiều kim đồng hồ)
      while (counter <= 12) {
          clearAllClock();

          // Bật LED tại vị trí counter (1 -> 2 -> ... -> 12)
          setNumberOnClock(counter);

          counter++;
          HAL_Delay(500); // Tốc độ chuyển LED (500ms)
      }

      // Reset về lại 1 để lặp lại vòng mới
      if (counter > 12) {
          counter = 1;
      }

      /* USER CODE END WHILE */
  }
}

/**
  * @brief Cấu hình chân GPIOA từ PA4 đến PA15
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Tắt tất cả chân ban đầu */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /* Cấu hình Output cho các chân PA4 -> PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
