/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../FW_Source/Library/mpst_log.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef char boolean;

#define MID_NUMBER 	1

#define RESET 		0
#define SET   		1

#define	OFF			0
#define ON			1

#define FALSE	 	0
#define TRUE 	 	1

#define IDLE		0
#define CHARGE		1
#define DISCHARGE	2
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
#define CSBI_1_Pin GPIO_PIN_3
#define CSBI_1_GPIO_Port GPIOA
#define CSBI_2_Pin GPIO_PIN_4
#define CSBI_2_GPIO_Port GPIOA
#define u_485_TX_Pin GPIO_PIN_6
#define u_485_TX_GPIO_Port GPIOC
#define u_485_RX_Pin GPIO_PIN_7
#define u_485_RX_GPIO_Port GPIOC
#define u_485_CON_Pin GPIO_PIN_8
#define u_485_CON_GPIO_Port GPIOC
#define u_PRE_G_Pin GPIO_PIN_8
#define u_PRE_G_GPIO_Port GPIOA
#define u_CHG_G_Pin GPIO_PIN_9
#define u_CHG_G_GPIO_Port GPIOA
#define u_DSG_G_Pin GPIO_PIN_10
#define u_DSG_G_GPIO_Port GPIOA
#define u_LED_Pin GPIO_PIN_11
#define u_LED_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
//	230907
#define	FM25V02_CS_ENABLE		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
#define	FM25V02_CS_DISABLE		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
