/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS_Pin GPIO_PIN_1
#define RS_GPIO_Port GPIOA
#define RW_Pin GPIO_PIN_2
#define RW_GPIO_Port GPIOA
#define EN_Pin GPIO_PIN_3
#define EN_GPIO_Port GPIOA
#define D7_Pin GPIO_PIN_4
#define D7_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_5
#define D6_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_6
#define D5_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_0
#define R1_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_1
#define R2_GPIO_Port GPIOB
#define R3_Pin GPIO_PIN_2
#define R3_GPIO_Port GPIOB
#define R4_Pin GPIO_PIN_10
#define R4_GPIO_Port GPIOB
#define C4_Pin GPIO_PIN_9
#define C4_GPIO_Port GPIOA
#define C3_Pin GPIO_PIN_10
#define C3_GPIO_Port GPIOA
#define C2_Pin GPIO_PIN_11
#define C2_GPIO_Port GPIOA
#define C1_Pin GPIO_PIN_12
#define C1_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
