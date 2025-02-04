/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
extern I2C_HandleTypeDef hi2c1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
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
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_6
#define LED5_GPIO_Port GPIOE
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOE
#define TX_EN_Pin GPIO_PIN_15
#define TX_EN_GPIO_Port GPIOE
#define PWR_SIM_Pin GPIO_PIN_8
#define PWR_SIM_GPIO_Port GPIOA
#define Button_on_Pin GPIO_PIN_15
#define Button_on_GPIO_Port GPIOA
#define Button_on_EXTI_IRQn EXTI15_10_IRQn
#define Button_off_Pin GPIO_PIN_10
#define Button_off_GPIO_Port GPIOC
#define Button_off_EXTI_IRQn EXTI15_10_IRQn
#define SIGNAL_CHECK_Pin GPIO_PIN_11
#define SIGNAL_CHECK_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
