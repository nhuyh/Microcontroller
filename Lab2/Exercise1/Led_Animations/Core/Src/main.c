
#include "main.h"

/* Handles */
TIM_HandleTypeDef htim2;

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void display7SEG(uint8_t num);

/* State */
static volatile uint8_t display_state = 0;
static volatile uint16_t tick_counter = 0;

/*
 * PIN MAPPING THEO SCHEMATIC
 *
 * PB0 -> segment a
 * PB1 -> segment b
 * PB2 -> segment c
 * PB3 -> segment d
 * PB4 -> segment e
 * PB5 -> segment f
 * PB6 -> segment g
 *
 * PA5 -> LED do rieng, luon sang
 * PA7 -> EN0 -> Q1 -> LED ben trai
 * PA6 -> EN1 -> Q2 -> LED ben phai
 *
 * Common anode:
 * RESET = segment ON
 * SET   = segment OFF
 *
 * PNP:
 * RESET = transistor ON
 * SET   = transistor OFF
 */

/*==========================================================
  MAIN
==========================================================*/

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();

    /*
     * BAT LED DO PA5 LIEN TUC
     * Gia dinh LED PA5 active-low.
     */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    /* Tat hai LED 7 doan luc khoi dong */
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_6 | GPIO_PIN_7,
        GPIO_PIN_SET
    );

    /* Tat tat ca segment */
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0 | GPIO_PIN_1 |
        GPIO_PIN_2 | GPIO_PIN_3 |
        GPIO_PIN_4 | GPIO_PIN_5 |
        GPIO_PIN_6,
        GPIO_PIN_SET
    );

    /*
     * Khoi dong:
     * LED ben trai hien so 1
     * PA7 = LOW de bat Q1
     */
    display7SEG(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

    display_state = 0;
    tick_counter = 0;

    /* Start TIM2 interrupt */
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    while (1)
    {
        /*
         * PA5 khong duoc dieu khien lai trong while.
         * LED do tiep tuc sang.
         *
         * LED scanning duoc xu ly trong TIM2 callback.
         */
    }
}

/*==========================================================
  DISPLAY 7-SEGMENT
==========================================================*/

void display7SEG(uint8_t num)
{
    /*
     * Bit 0 -> PB0 -> a
     * Bit 1 -> PB1 -> b
     * Bit 2 -> PB2 -> c
     * Bit 3 -> PB3 -> d
     * Bit 4 -> PB4 -> e
     * Bit 5 -> PB5 -> f
     * Bit 6 -> PB6 -> g
     *
     * Bit = 1: segment OFF
     * Bit = 0: segment ON
     */

    static const uint8_t code[10] =
    {
        0x40, /* 0 */
        0x79, /* 1 */
        0x24, /* 2 */
        0x30, /* 3 */
        0x19, /* 4 */
        0x12, /* 5 */
        0x02, /* 6 */
        0x78, /* 7 */
        0x00, /* 8 */
        0x10  /* 9 */
    };

    if (num > 9)
    {
        return;
    }

    for (uint8_t i = 0; i < 7; i++)
    {
        if ((code[num] & (1U << i)) != 0U)
        {
            HAL_GPIO_WritePin(
                GPIOB,
                (uint16_t)(1U << i),
                GPIO_PIN_SET
            );
        }
        else
        {
            HAL_GPIO_WritePin(
                GPIOB,
                (uint16_t)(1U << i),
                GPIO_PIN_RESET
            );
        }
    }
}

/*==========================================================
  TIM2 CALLBACK
  Timer interrupt = 10 ms
  500 ms / 10 ms = 50 ticks
==========================================================*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2)
    {
        return;
    }

    tick_counter++;

    if (tick_counter >= 50)
    {
        tick_counter = 0;

        if (display_state == 0)
        {
            /*
             * Current: LED trai hien so 1
             * Turn OFF LED trai (PA7)
             */
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_7,
                GPIO_PIN_SET
            );

            /* Change shared segments to number 2 */
            display7SEG(1);

            /* Turn ON LED phai through Q2 (PA6) */
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_6,
                GPIO_PIN_RESET
            );

            display_state = 1;
        }
        else
        {
            /*
             * Current: LED phai hien so 2
             * Turn OFF LED phai (PA6)
             */
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_6,
                GPIO_PIN_SET
            );

            /* Change shared segments to number 1 */
            display7SEG(2);

            /* Turn ON LED trai through Q1 (PA7) */
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_7,
                GPIO_PIN_RESET
            );

            display_state = 0;
        }
    }
}

/*==========================================================
  SYSTEM CLOCK: HSI 8 MHz
==========================================================*/

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    clk.ClockType = RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 |
                    RCC_CLOCKTYPE_PCLK2;

    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/*==========================================================
  TIM2 CONFIGURATION
  HSI = 8 MHz
  Prescaler = 7999 -> 1 kHz
  Period = 9 -> interrupt every 10 ms
==========================================================*/

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef clock_source = {0};
    TIM_MasterConfigTypeDef master_config = {0};

    /* Enable TIM2 peripheral clock */
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    clock_source.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(&htim2, &clock_source) != HAL_OK)
    {
        Error_Handler();
    }

    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(
            &htim2, &master_config) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable TIM2 interrupt in NVIC */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/*==========================================================
  GPIO CONFIGURATION
==========================================================*/

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*
     * PA5 = RESET: bat LED do neu mach active-low.
     * PA6, PA7 = SET: tat hai transistor PNP.
     */
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_5,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_6 | GPIO_PIN_7,
        GPIO_PIN_SET
    );

    /* Initial states: all segments OFF */
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0 | GPIO_PIN_1 |
        GPIO_PIN_2 | GPIO_PIN_3 |
        GPIO_PIN_4 | GPIO_PIN_5 |
        GPIO_PIN_6,
        GPIO_PIN_SET
    );

    /* PA5 = LED do, PA6 = EN1, PA7 = EN0 */
    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* PB0-PB6 = seven segment pins */
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 |
               GPIO_PIN_2 | GPIO_PIN_3 |
               GPIO_PIN_4 | GPIO_PIN_5 |
               GPIO_PIN_6;

    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);
}

/*==========================================================
  ERROR HANDLER
==========================================================*/

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
