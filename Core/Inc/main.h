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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdbool.h"
#include <inttypes.h>

#include "cmsis_os.h"

#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_rcc.h"



#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "crc.h"



#include "cJSON.h"
#include "head_def.h"
#include "head_enum.h"
#include "head_typedef.h"
#include "head_thread.h"
#include "head_fs.h"
#include "spif.h"

#include "utilities_common.h"
#include "stm_queue.h"
#include "stm_list.h"
#include "stm_logging.h"
#include "head_trace.h"
#include "head_console.h"








#include "head_data.h"
#include "head_func.h"


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
#define YUI_M_Pin GPIO_PIN_0
#define YUI_M_GPIO_Port GPIOC
#define MUI1_Pin GPIO_PIN_1
#define MUI1_GPIO_Port GPIOC
#define MUI2_Pin GPIO_PIN_2
#define MUI2_GPIO_Port GPIOC
#define AEDT_Pin GPIO_PIN_3
#define AEDT_GPIO_Port GPIOC
#define CDS_Pin GPIO_PIN_0
#define CDS_GPIO_Port GPIOA
#define RB_TX_Pin GPIO_PIN_2
#define RB_TX_GPIO_Port GPIOA
#define RB_RX_Pin GPIO_PIN_3
#define RB_RX_GPIO_Port GPIOA
#define SENSOR_TX_EN_Pin GPIO_PIN_7
#define SENSOR_TX_EN_GPIO_Port GPIOA
#define SENSOR_TX_Pin GPIO_PIN_4
#define SENSOR_TX_GPIO_Port GPIOC
#define SENSOR_RX_Pin GPIO_PIN_5
#define SENSOR_RX_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_0
#define SPI1_CS_GPIO_Port GPIOB
#define SPI1_WP_Pin GPIO_PIN_1
#define SPI1_WP_GPIO_Port GPIOB
#define SPI1_HOLD_Pin GPIO_PIN_2
#define SPI1_HOLD_GPIO_Port GPIOB
#define EMER_BTN_Pin GPIO_PIN_8
#define EMER_BTN_GPIO_Port GPIOE
#define EMER_BTN_EXTI_IRQn EXTI9_5_IRQn
#define FIRE_DOOR_Pin GPIO_PIN_9
#define FIRE_DOOR_GPIO_Port GPIOE
#define FIRE_DOOR_EXTI_IRQn EXTI9_5_IRQn
#define AED_DOOR_Pin GPIO_PIN_10
#define AED_DOOR_GPIO_Port GPIOE
#define AED_DOOR_EXTI_IRQn EXTI15_10_IRQn
#define FLOOD_Pin GPIO_PIN_11
#define FLOOD_GPIO_Port GPIOE
#define FLOOD_EXTI_IRQn EXTI15_10_IRQn
#define HW_VER_ID3_Pin GPIO_PIN_12
#define HW_VER_ID3_GPIO_Port GPIOE
#define HW_VER_ID2_Pin GPIO_PIN_13
#define HW_VER_ID2_GPIO_Port GPIOE
#define HW_VER_ID1_Pin GPIO_PIN_14
#define HW_VER_ID1_GPIO_Port GPIOE
#define HW_VER_ID0_Pin GPIO_PIN_15
#define HW_VER_ID0_GPIO_Port GPIOE
#define LED_LIA0_Pin GPIO_PIN_12
#define LED_LIA0_GPIO_Port GPIOB
#define LED_LIA2_Pin GPIO_PIN_13
#define LED_LIA2_GPIO_Port GPIOB
#define LED_LIA1_Pin GPIO_PIN_14
#define LED_LIA1_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_8
#define FAN_GPIO_Port GPIOD
#define PTC_Pin GPIO_PIN_9
#define PTC_GPIO_Port GPIOD
#define LCD_Pin GPIO_PIN_10
#define LCD_GPIO_Port GPIOD
#define INVERTER_Pin GPIO_PIN_11
#define INVERTER_GPIO_Port GPIOD
#define SPEAKER_Pin GPIO_PIN_12
#define SPEAKER_GPIO_Port GPIOD
#define LED_PWM_Pin GPIO_PIN_13
#define LED_PWM_GPIO_Port GPIOD
#define SIREN_Pin GPIO_PIN_14
#define SIREN_GPIO_Port GPIOD
#define LTE_Pin GPIO_PIN_15
#define LTE_GPIO_Port GPIOD
#define DEBUG_TX_Pin GPIO_PIN_9
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_10
#define DEBUG_RX_GPIO_Port GPIOA
#define IAP_TX_Pin GPIO_PIN_10
#define IAP_TX_GPIO_Port GPIOC
#define IAP_RX_Pin GPIO_PIN_11
#define IAP_RX_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_0
#define LED_R_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_2
#define LED_B_GPIO_Port GPIOD
#define YUCHAR_Pin GPIO_PIN_3
#define YUCHAR_GPIO_Port GPIOD
#define MUCHAR1_Pin GPIO_PIN_4
#define MUCHAR1_GPIO_Port GPIOD
#define MUCHAR2_Pin GPIO_PIN_5
#define MUCHAR2_GPIO_Port GPIOD
#define SPI1_CLOCK__FLASH__Pin GPIO_PIN_3
#define SPI1_CLOCK__FLASH__GPIO_Port GPIOB
#define SPI1_MISO_Pin GPIO_PIN_4
#define SPI1_MISO_GPIO_Port GPIOB
#define SPI1_MOSI_Pin GPIO_PIN_5
#define SPI1_MOSI_GPIO_Port GPIOB
#define PH3_BOOT0_Pin GPIO_PIN_3
#define PH3_BOOT0_GPIO_Port GPIOH
#define LED_ACT_Pin GPIO_PIN_0
#define LED_ACT_GPIO_Port GPIOE
#define LED_COM_Pin GPIO_PIN_1
#define LED_COM_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
// *****************************************************************************
// Smart Bench LED control
// *****************************************************************************
#define led_act_on 			HAL_GPIO_WritePin(LED_ACT_GPIO_Port, LED_ACT_Pin, GPIO_PIN_RESET)
#define led_act_off 		HAL_GPIO_WritePin(LED_ACT_GPIO_Port, LED_ACT_Pin, GPIO_PIN_SET)
#define led_act_toggle 		HAL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin)
#define led_com_on 			HAL_GPIO_WritePin(LED_COM_GPIO_Port, LED_COM_Pin, GPIO_PIN_RESET)
#define led_com_off 		HAL_GPIO_WritePin(LED_COM_GPIO_Port, LED_COM_Pin, GPIO_PIN_SET)
#define led_com_toggle 		HAL_GPIO_TogglePin(LED_COM_GPIO_Port, LED_COM_Pin)

/*******************************************************************************
 LEDBAR Object
 *******************************************************************************/
#define LED_R_Set				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)
#define LED_R_Clear				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET)

#define LED_G_Set				HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET)
#define LED_G_Clear				HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET)

#define LED_B_Set				HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET)
#define LED_B_Clear				HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// SIREN control
// *****************************************************************************
#define siren_on 			HAL_GPIO_WritePin(SIREN_GPIO_Port, SIREN_Pin, GPIO_PIN_RESET)
#define siren_off 			HAL_GPIO_WritePin(SIREN_GPIO_Port, SIREN_Pin, GPIO_PIN_SET)

// *****************************************************************************
// LTE control
// *****************************************************************************
#define lte_on 				HAL_GPIO_WritePin(LTE_GPIO_Port, LTE_Pin, GPIO_PIN_RESET)
#define lte_off 			HAL_GPIO_WritePin(LTE_GPIO_Port, LTE_Pin, GPIO_PIN_SET)

// *****************************************************************************
// PTC control
// *****************************************************************************
#define ptc_on 				HAL_GPIO_WritePin(PTC_GPIO_Port, PTC_Pin, GPIO_PIN_RESET)
#define ptc_off 			HAL_GPIO_WritePin(PTC_GPIO_Port, PTC_Pin, GPIO_PIN_SET)

// *****************************************************************************
// YUCHAR control
// *****************************************************************************
#define yuchar_on 			HAL_GPIO_WritePin(YUCHAR_GPIO_Port, YUCHAR_Pin, GPIO_PIN_SET)
#define yuchar_off 			HAL_GPIO_WritePin(YUCHAR_GPIO_Port, YUCHAR_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// MUCHAR1 control
// *****************************************************************************
#define muchar1_on 			HAL_GPIO_WritePin(MUCHAR1_GPIO_Port, MUCHAR1_Pin, GPIO_PIN_SET)
#define muchar1_off 		HAL_GPIO_WritePin(MUCHAR1_GPIO_Port, MUCHAR1_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// MUCHAR2 control
// *****************************************************************************
#define muchar2_on 			HAL_GPIO_WritePin(MUCHAR2_GPIO_Port, MUCHAR2_Pin, GPIO_PIN_SET)
#define muchar2_off 		HAL_GPIO_WritePin(MUCHAR2_GPIO_Port, MUCHAR2_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// FAN control
// *****************************************************************************
#define fan_on 				HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET)
#define fan_off 			HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// INVERTER control
// *****************************************************************************
#define inverter_on 		HAL_GPIO_WritePin(INVERTER_GPIO_Port, INVERTER_Pin, GPIO_PIN_SET)
#define inverter_off 		HAL_GPIO_WritePin(INVERTER_GPIO_Port, INVERTER_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// SPEAKER control
// *****************************************************************************
#define speaker_on 			HAL_GPIO_WritePin(SPEAKER_GPIO_Port, SPEAKER_Pin, GPIO_PIN_SET)
#define speaker_off 		HAL_GPIO_WritePin(SPEAKER_GPIO_Port, SPEAKER_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// LCD control
// *****************************************************************************
#define lcd_on 				HAL_GPIO_WritePin(LCD_GPIO_Port, LCD_Pin, GPIO_PIN_SET)
#define lcd_off 			HAL_GPIO_WritePin(LCD_GPIO_Port, LCD_Pin, GPIO_PIN_RESET)

// *****************************************************************************
// LED LIA control
// *****************************************************************************
#define led_lia0_on 		HAL_GPIO_WritePin(LED_LIA0_GPIO_Port, LED_LIA0_Pin, GPIO_PIN_SET)
#define led_lia0_off 		HAL_GPIO_WritePin(LED_LIA0_GPIO_Port, LED_LIA0_Pin, GPIO_PIN_RESET)

#define led_lia1_on 		HAL_GPIO_WritePin(LED_LIA1_GPIO_Port, LED_LIA1_Pin, GPIO_PIN_SET)
#define led_lia1_off 		HAL_GPIO_WritePin(LED_LIA1_GPIO_Port, LED_LIA1_Pin, GPIO_PIN_RESET)

#define led_lia2_on 		HAL_GPIO_WritePin(LED_LIA2_GPIO_Port, LED_LIA2_Pin, GPIO_PIN_SET)
#define led_lia2_off 		HAL_GPIO_WritePin(LED_LIA2_GPIO_Port, LED_LIA2_Pin, GPIO_PIN_RESET)



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
