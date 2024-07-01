/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l0xx_hal.h"

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
#define FINGER_INT_Pin GPIO_PIN_0
#define FINGER_INT_GPIO_Port GPIOA
#define FINGER_INT_EXTI_IRQn EXTI0_1_IRQn
#define MCU_TX_Pin GPIO_PIN_2
#define MCU_TX_GPIO_Port GPIOA
#define MCU_RX_Pin GPIO_PIN_3
#define MCU_RX_GPIO_Port GPIOA
#define ADC_CH3_Pin GPIO_PIN_5
#define ADC_CH3_GPIO_Port GPIOA
#define ADC_CH1_Pin GPIO_PIN_7
#define ADC_CH1_GPIO_Port GPIOA
#define ADC_CH2_Pin GPIO_PIN_0
#define ADC_CH2_GPIO_Port GPIOB
#define CHRG_INT_Pin GPIO_PIN_8
#define CHRG_INT_GPIO_Port GPIOA
#define CHRG_INT_EXTI_IRQn EXTI4_15_IRQn
#define FINGER_ENABLE_Pin GPIO_PIN_12
#define FINGER_ENABLE_GPIO_Port GPIOA
#define BLUE_Pin GPIO_PIN_3
#define BLUE_GPIO_Port GPIOB
#define RED_Pin GPIO_PIN_4
#define RED_GPIO_Port GPIOB
#define GREEN_Pin GPIO_PIN_5
#define GREEN_GPIO_Port GPIOB
#define CHRG_OUT_Pin GPIO_PIN_6
#define CHRG_OUT_GPIO_Port GPIOB
#define CHRG_IN_Pin GPIO_PIN_7
#define CHRG_IN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
