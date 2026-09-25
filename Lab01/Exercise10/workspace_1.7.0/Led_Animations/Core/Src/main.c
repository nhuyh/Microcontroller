#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

// Mảng ánh xạ chân GPIO cho 12 vị trí LED (0h -> 11h)
// 0  -> 12h (PA4)
// 1  -> 1h  (PA5)
// ...
// 11 -> 11h (PA15)
const uint16_t LED_PINS[12] = {
    GPIO_PIN_4,  // 0: 12h
    GPIO_PIN_5,  // 1: 1h
    GPIO_PIN_6,  // 2: 2h
    GPIO_PIN_7,  // 3: 3h
    GPIO_PIN_8,  // 4: 4h
    GPIO_PIN_9,  // 5: 5h
    GPIO_PIN_10, // 6: 6h
    GPIO_PIN_11, // 7: 7h
    GPIO_PIN_12, // 8: 8h
    GPIO_PIN_13, // 9: 9h
    GPIO_PIN_14, // 10: 10h
    GPIO_PIN_15  // 11: 11h
};

// Hàm tắt tất cả 12 LED (Bài 7)
void clearAllClock(void) {
    HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
        GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
        GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
        GPIO_PIN_RESET);
}

// Hàm bật LED theo vị trí (Bài 8)
void setNumberOnClock(int num) {
    if (num < 0 || num > 11) return;
    HAL_GPIO_WritePin(GPIOA, LED_PINS[num], GPIO_PIN_SET);
}

// Hàm tắt LED theo vị trí (Bài 9)
void clearNumberOnClock(int num) {
    if (num < 0 || num > 11) return;
    HAL_GPIO_WritePin(GPIOA, LED_PINS[num], GPIO_PIN_RESET);
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  int hour = 0;   // Khởi tạo giờ (0 - 11)
  int minute = 0; // Khởi tạo phút (0 - 59)
  int second = 0; // Khởi tạo giây (0 - 59)

  while (1)
  {
      // 1. Tắt hết LED trước khi hiển thị vị trí mới
      clearAllClock();

      // 2. Quy đổi Giây, Phút, Giờ sang vị trí 12 LED (từ 0 đến 11)
      int sec_pos = second / 5;   // 60 giây chia thành 12 mốc (mỗi mốc 5s)
      int min_pos = minute / 5;   // 60 phút chia thành 12 mốc (mỗi mốc 5m)
      int hour_pos = hour % 12;   // 12 mốc giờ

      // 3. Bật 3 LED tương ứng với kim Giờ, Phút, Giây
      setNumberOnClock(sec_pos);
      setNumberOnClock(min_pos);
      setNumberOnClock(hour_pos);

      // 4. Trễ 1000ms (1 giây)
      HAL_Delay(10);

      // 5. Cập nhật đếm thời gian thực
      second++;
      if (second >= 60) {
          second = 0;
          minute++;
          if (minute >= 60) {
              minute = 0;
              hour++;
              if (hour >= 12) {
                  hour = 0;
              }
          }
      }
  }
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Đặt trạng thái ban đầu tắt tất cả LED
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  // Cấu hình PA4 -> PA15 làm Output Push-Pull
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SystemClock_Config(void) {
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
