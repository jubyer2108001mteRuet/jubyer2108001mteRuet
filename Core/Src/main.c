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
#include <string.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* LCD Pin Definitions (16x2) */
#define RS_PORT GPIOA
#define RS_PIN  GPIO_PIN_1
#define RW_PORT GPIOA
#define RW_PIN  GPIO_PIN_2
#define EN_PORT GPIOA
#define EN_PIN  GPIO_PIN_3
#define D4_PORT GPIOA
#define D4_PIN  GPIO_PIN_7
#define D5_PORT GPIOA
#define D5_PIN  GPIO_PIN_6
#define D6_PORT GPIOA
#define D6_PIN  GPIO_PIN_5
#define D7_PORT GPIOA
#define D7_PIN  GPIO_PIN_4

/* Keypad Pin Definitions */
#define R1_PORT GPIOA
#define R1_PIN  GPIO_PIN_9
#define R2_PORT GPIOA
#define R2_PIN  GPIO_PIN_10
#define R3_PORT GPIOA
#define R3_PIN  GPIO_PIN_11
#define R4_PORT GPIOA
#define R4_PIN  GPIO_PIN_12

#define C1_PORT GPIOB
#define C1_PIN  GPIO_PIN_12
#define C2_PORT GPIOB
#define C2_PIN  GPIO_PIN_13
#define C3_PORT GPIOB
#define C3_PIN  GPIO_PIN_14
#define C4_PORT GPIOB
#define C4_PIN  GPIO_PIN_15

/* Debug Pin */
#define DEBUG_PORT GPIOA
#define DEBUG_PIN  GPIO_PIN_8

/* Timer definition for microsecond delay */
#define timer htim1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef timer; // Defined as htim1

// Keypad layout
const char keypad[4][4] = {
    {'1', '2', '3', '+'},    // R1: PA9
    {'4', '5', '6', '*'},    // R2: PA10
    {'7', '8', '9', '-'},    // R3: PA11
    {'c', '0', '#', '='}     // R4: PA12 (B is '=')
};

// Calculator variables
int num1 = 0, num2 = 0, result = 0;
char operator = 0;
uint8_t state = 0; // 0: num1, 1: operator, 2: num2 entry, 3: result
char display_buffer[16]; // Buffer for LCD display
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
char read_keypad(void);
void calculate(char key);

/* LCD function prototypes */
void delay(uint16_t us);
void send_to_lcd(char data, int rs);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_put_cur(int row, int col);
void lcd_init(void);
void lcd_send_string(char *str);
void lcd_clear(void);

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
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();

  HAL_TIM_Base_Start(&timer); // Start TIM1 for LCD delay
  lcd_init();
  lcd_clear();
  lcd_put_cur(0, 0); // Start at row 0 for input
  lcd_put_cur(1, 0);
  lcd_send_string("Result:"); // Static label on row 1

  while (1)
  {
    HAL_GPIO_TogglePin(DEBUG_PORT, DEBUG_PIN); // Toggle PA8 to confirm loop
    HAL_Delay(500); // Blink every 500ms
    char key = read_keypad();
    if (key != 0x00) {
      calculate(key);
      HAL_Delay(200); // Debounce delay
    }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_PIN | RW_PIN | EN_PIN | D4_PIN | D5_PIN | D6_PIN | D7_PIN | DEBUG_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, R1_PIN | R2_PIN | R3_PIN | R4_PIN, GPIO_PIN_SET);

  /* Configure LCD GPIO pins */
  GPIO_InitStruct.Pin = RS_PIN | RW_PIN | EN_PIN | D4_PIN | D5_PIN | D6_PIN | D7_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Configure Keypad Row pins (Outputs, HIGH by default) */
  GPIO_InitStruct.Pin = R1_PIN | R2_PIN | R3_PIN | R4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Configure Keypad Column pins (Inputs with pull-up) */
  GPIO_InitStruct.Pin = C1_PIN | C2_PIN | C3_PIN | C4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure Debug pin (PA8) */
  GPIO_InitStruct.Pin = DEBUG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* Keypad Reading Function with Debounce */
char read_keypad(void) {
    char key = 0x00;

    HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) { key = '1'; }
    else if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) { key = '2'; }
    else if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) { key = '3'; }
    else if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) { key = '+'; }
    HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);

    if (key != 0x00) {
        while (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) || !HAL_GPIO_ReadPin(C2_PORT, C2_PIN) ||
               !HAL_GPIO_ReadPin(C3_PORT, C3_PIN) || !HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
            HAL_Delay(10);
        }
        return key;
    }

    HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) { key = '4'; }
    else if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) { key = '5'; }
    else if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) { key = '6';}
    else if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) { key = '*'; }

    HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);

    if (key != 0x00) {
        while (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) || !HAL_GPIO_ReadPin(C2_PORT, C2_PIN) ||
               !HAL_GPIO_ReadPin(C3_PORT, C3_PIN) || !HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
            HAL_Delay(10);
        }
        return key;
    }

    HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) { key = '7'; }
    else if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) { key = '8'; }
    else if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) { key = '9'; }
    else if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) { key = '-'; }// c=-

    HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);

    if (key != 0x00) {
        while (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) || !HAL_GPIO_ReadPin(C2_PORT, C2_PIN) ||
               !HAL_GPIO_ReadPin(C3_PORT, C3_PIN) || !HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
            HAL_Delay(10);
        }
        return key;
    }

    HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) { key = 'c'; }
    else if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) { key = '0'; }
    else if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) { key = '#'; }
    else if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) { key = '='; }
    HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);

    if (key != 0x00) {
        while (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) || !HAL_GPIO_ReadPin(C2_PORT, C2_PIN) ||
               !HAL_GPIO_ReadPin(C3_PORT, C3_PIN) || !HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
            HAL_Delay(10);
        }
        return key;
    }

    return 0x00;
}

/* Calculator Logic with LCD Display */
void calculate(char key) {
    static int num1 = 0, num2 = 0, result = 0;
    static char operator = 0;
    static int state = 0; // 0: num1 entry, 1: operator entry, 2: num2 entry
    static int result_displayed = 0; // Flag to track if result is displayed
    char display_buffer[16]; // Adjust size as needed for your LCD

    if (key >= '0' && key <= '9') {
        if (state == 0) { // Build and display num1
            num1 = num1 * 10 + (key - '0');
            sprintf(display_buffer, "%d", num1); // Convert num1 to string
            lcd_put_cur(0, 0); // Position cursor at start of first row
            lcd_send_string(display_buffer);
        } else if (state == 1) { // Transition to num2 entry
            state = 2;
            num2 = num2 * 10 + (key - '0'); // Start num2
            // Display operator and num2 on the first row
            sprintf(display_buffer, "%d%c%d", num1, operator, num2); // Convert num1, operator, and num2 to string
            lcd_put_cur(0, 0); // Position cursor at start of first row
            lcd_send_string(display_buffer);
        } else if (state == 2) { // Continue building num2
            num2 = num2 * 10 + (key - '0');
            sprintf(display_buffer, "%d%c%d", num1, operator, num2); // Update display with num2
            lcd_put_cur(0, 0); // Ensure num2 is displayed on the first row
            lcd_send_string(display_buffer);
        }
    }
    else if (key == '+') {
        if (state == 0) {
            state = 1; // Move to operator entry state
            operator = '+'; // Set the operator
            lcd_put_cur(0, strlen(display_buffer)); // Position cursor after num1
            lcd_send_data('+'); // Display the '+' operator
        }
    }
    else if (key == '-') {
        if (state == 0) {
            state = 1;
            operator = '-';
            lcd_put_cur(0, strlen(display_buffer));
            lcd_send_data('-');
        }
    }
    else if (key == '*') {
        if (state == 0) {
            state = 1;
            operator = '*';
            lcd_put_cur(0, strlen(display_buffer));
            lcd_send_data('*');
        }
    }
    else if (key == '=') {
        if (state == 2) { // If num2 is entered, perform calculation
            if (operator == '+') {
                result = num1 + num2;
            }
            else if (operator == '-') {
                result = num1 - num2;
            }
            else if (operator == '*') {
                result = num1 * num2;
            }

            // Convert the result to string and display it
            sprintf(display_buffer, "Result: %d", result); // Convert result to string
            lcd_put_cur(1, 0); // Position to display result on first row
            lcd_send_string(display_buffer);
            result_displayed = 1; // Set flag to indicate result is displayed

            // Reset for the next calculation
            num1 = 0;
            num2 = 0;
            operator = 0;
            state = 0;
        }
    }
    else if (key == 'c') { // Clear screen when 'c' is pressed
        if (result_displayed) {
            lcd_clear(); // Clear the display
            lcd_put_cur(1, 0); // Position to start from the first row
            lcd_send_string("Result:"); // Optionally print "Result:" again
            result_displayed = 0; // Reset the flag
        }
    }
}

/* LCD Functions */
void delay(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&timer, 0);
    while (__HAL_TIM_GET_COUNTER(&timer) < us);
}

void send_to_lcd(char data, int rs) {
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, rs);
    HAL_GPIO_WritePin(RW_PORT, RW_PIN, 0);
    HAL_GPIO_WritePin(D7_PORT, D7_PIN, ((data >> 3) & 0x01));
    HAL_GPIO_WritePin(D6_PORT, D6_PIN, ((data >> 2) & 0x01));
    HAL_GPIO_WritePin(D5_PORT, D5_PIN, ((data >> 1) & 0x01));
    HAL_GPIO_WritePin(D4_PORT, D4_PIN, ((data >> 0) & 0x01));
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, 1);
    delay(20);
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, 0);
    delay(20);
}

void lcd_send_cmd(char cmd) {
    char datatosend;
    datatosend = ((cmd >> 4) & 0x0F);
    send_to_lcd(datatosend, 0);
    datatosend = (cmd & 0x0F);
    send_to_lcd(datatosend, 0);
    delay(100);
}

void lcd_send_data(char data) {
    char datatosend;
    datatosend = ((data >> 4) & 0x0F);
    send_to_lcd(datatosend, 1);
    datatosend = (data & 0x0F);
    send_to_lcd(datatosend, 1);
}

void lcd_put_cur(int row, int col) {
    static const uint8_t row_offsets[] = {0x00, 0x40}; // 16x2 LCD
    if (row >= 0 && row <= 1 && col >= 0 && col <= 15) {
        lcd_send_cmd(0x80 | (row_offsets[row] + col));
    }
}

void lcd_init(void) {
    HAL_Delay(50); // Initial delay for LCD power-up
    HAL_GPIO_WritePin(RW_PORT, RW_PIN, 0);
    lcd_send_cmd(0x33); // Initialize in 8-bit mode
    HAL_Delay(5);
    lcd_send_cmd(0x32); // Switch to 4-bit mode
    HAL_Delay(5);
    lcd_send_cmd(0x28); // 4-bit, 2-line, 5x8 font
    HAL_Delay(5);
    lcd_send_cmd(0x08); // Display off
    HAL_Delay(5);
    lcd_send_cmd(0x01); // Clear display
    HAL_Delay(5);
    lcd_send_cmd(0x06); // Entry mode: increment cursor
    HAL_Delay(5);
    lcd_send_cmd(0x0C); // Display on, no cursor
    HAL_Delay(5);
}

void lcd_send_string(char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(5);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  HAL_GPIO_WritePin(DEBUG_PORT, DEBUG_PIN, GPIO_PIN_SET); // Debug pin HIGH on error
  __disable_irq();
  while (1) {}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
