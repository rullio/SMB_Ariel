/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim16;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RCC global interrupt.
  */
void RCC_IRQHandler(void)
{
  /* USER CODE BEGIN RCC_IRQn 0 */
	SMBIntrObj.RCC_Intr_count++;

  /* USER CODE END RCC_IRQn 0 */
  /* USER CODE BEGIN RCC_IRQn 1 */

  /* USER CODE END RCC_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	SMBIntrObj.DMA1_Chan_1_count++;

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */
	SMBIntrObj.DMA1_Chan_3_count++;

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */
	SMBIntrObj.DMA1_Chan_4_count++;

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */
	SMBIntrObj.DMA1_Chan_6_count++;

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */
	SMBIntrObj.DMA1_Chan_7_count++;

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles ADC1 and ADC2 interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */
	SMBIntrObj.ADC1_2_count++;

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
	SMBIntrObj.EXTI9_5_count++;

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(EMER_BTN_Pin);
  HAL_GPIO_EXTI_IRQHandler(FIRE_DOOR_Pin);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */
	SMBIntrObj.TIM1_UP_16_count++;

  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
	SMBIntrObj.TIM4_count++;

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */
	SMBIntrObj.SPI1_count++;

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi1);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	SMBIntrObj.USART1_count++;

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	SMBIntrObj.USART2_count++;

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	SMBIntrObj.USART3_count++;

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	SMBIntrObj.EXTI15_10_count++;

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(AED_DOOR_Pin);
  HAL_GPIO_EXTI_IRQHandler(FLOOD_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
	SMBIntrObj.USART4_count++;

  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel3 global interrupt.
  */
void DMA2_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel3_IRQn 0 */
	SMBIntrObj.DMA2_Chan_3_count++;

  /* USER CODE END DMA2_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_tx);
  /* USER CODE BEGIN DMA2_Channel3_IRQn 1 */

  /* USER CODE END DMA2_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel5 global interrupt.
  */
void DMA2_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel5_IRQn 0 */
	SMBIntrObj.DMA2_Chan_5_count++;

  /* USER CODE END DMA2_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
  /* USER CODE BEGIN DMA2_Channel5_IRQn 1 */

  /* USER CODE END DMA2_Channel5_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/**
 * @brief  Conversion complete callback in non-blocking mode.
 * @param hadc ADC handle
 * @retval None
 */
#define		AEDT_COMPENSATION_VALUE		1
//extern uint16_t fake_temperature;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	/* Computation of ADC conversions raw data to physical values           */
	/* using LL ADC driver helper macro.                                    */
	/* Note: ADC results are transferred into array "aADCxConvertedData"    */
	/*       in the order of their rank in ADC sequencer.                   */
	// 아래 나머지 변환에서 측정된 VREF 를 고정된 VDDA_APPLI 값대신 쓰기 위해서 가장 먼저 변환함..
	SMB_adc_value.VREF 			= __LL_ADC_CALC_VREFANALOG_VOLTAGE(aADCxConvertedData[7], LL_ADC_RESOLUTION_12B);

	SMB_adc_value.YUI			= __LL_ADC_CALC_DATA_TO_VOLTAGE(SMB_adc_value.VREF, aADCxConvertedData[0], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.MUI1			= __LL_ADC_CALC_DATA_TO_VOLTAGE(SMB_adc_value.VREF, aADCxConvertedData[1], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.MUI2			= __LL_ADC_CALC_DATA_TO_VOLTAGE(SMB_adc_value.VREF, aADCxConvertedData[2], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.AEDT			= __LL_ADC_CALC_DATA_TO_TEMPERATURE(aADCxConvertedData[3]) - AEDT_COMPENSATION_VALUE;	// AED 온도 센서가 비선형이라서 송 사장님이 주신 raw data table 을 사용해서 변환하는 것으로 함
	SMB_adc_value.CDS			= __LL_ADC_CALC_DATA_TO_VOLTAGE(SMB_adc_value.VREF, aADCxConvertedData[4], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.TEMP			= __LL_ADC_CALC_TEMPERATURE(SMB_adc_value.VREF, aADCxConvertedData[5], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.VBAT 			= __LL_ADC_CALC_DATA_TO_VOLTAGE(SMB_adc_value.VREF, aADCxConvertedData[6], LL_ADC_RESOLUTION_12B);
	SMB_adc_value.yui_raw_data	= aADCxConvertedData[0];
	SMB_adc_value.mui1_raw_data	= aADCxConvertedData[1];
	SMB_adc_value.mui2_raw_data	= aADCxConvertedData[2];
	SMB_adc_value.aedt_raw_data	= aADCxConvertedData[3];
	SMB_adc_value.cds_raw_data	= aADCxConvertedData[4];

	//	SB_adc_value.AEDT = fake_temperature;
}

/**
 * @brief  Conversion DMA half-transfer callback in non-blocking mode.
 * @param hadc ADC handle
 * @retval None
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	UNUSED(hadc);
}

/**
 * @brief  Analog watchdog 1 callback in non-blocking mode.
 * @param hadc ADC handle
 * @retval None
 */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
	UNUSED(hadc);
}

/**
 * @brief  ADC error callback in non-blocking mode
 *         (ADC conversion with interruption or transfer by DMA).
 * @note   In case of error due to overrun when using ADC with DMA transfer
 *         (HAL ADC handle parameter "ErrorCode" to state "HAL_ADC_ERROR_OVR"):
 *         - Reinitialize the DMA using function "HAL_ADC_Stop_DMA()".
 *         - If needed, restart a new ADC conversion using function
 *           "HAL_ADC_Start_DMA()"
 *           (this function is also clearing overrun flag)
 * @param hadc ADC handle
 * @retval None
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	UNUSED(hadc);
}

// *****************************************************************************
// U(S)ART Interrupt callback
// *****************************************************************************

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART3) {		// 조도 센서 보드에서 수신되는 packet 용
		manager_msg_t manager_msg;
		assert (Size < IMS_RX_BUF_SIZE);
		memset (&manager_msg, 0, sizeof(manager_msg));

		manager_msg.head.type = MANAGER_MSG_IMS;
		manager_msg.head.dst = WORKM_MANAGER;
		manager_msg.head.src = WORKM_UART3;
		manager_msg.head.len = Size;

		memcpy(&manager_msg.body.Byte, (void *)ims_rx_buffer, Size);
		osMessageQueuePut(managerThreadQ, &manager_msg, 0U, 0U);
		HAL_UARTEx_ReceiveToIdle_DMA(huart, ims_rx_buffer, IMS_RX_BUF_SIZE);
	}
	else {
		assert (0 == 1);
	}
}

// *****************************************************************************
// Emergency button/AED door/Fire door/flooding interrupt
// *****************************************************************************
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	manager_msg_t manager_msg;

	memset (&manager_msg, 0, sizeof(manager_msg));

	switch (GPIO_Pin) {
	case EMER_BTN_Pin :	manager_msg.head.type = MANAGER_MSG_EMER_BTN; break;
	case FIRE_DOOR_Pin : manager_msg.head.type = MANAGER_MSG_FIRE_DOOR; break;
	case AED_DOOR_Pin :	manager_msg.head.type = MANAGER_MSG_AED_DOOR; break;
	case FLOOD_Pin : manager_msg.head.type = MANAGER_MSG_FLOODING; break;
	default :
		printf("%s() : Undefined EXTI GPIO_Pin = %d"LINE_TERM, __FUNCTION__, GPIO_Pin);
		return;
		break;
	}

	manager_msg.head.dst = WORKM_MANAGER;
	manager_msg.head.src = WORKM_EXTI;
	manager_msg.head.len = 0;

	osMessageQueuePut(managerThreadQ, &manager_msg, 0U, 0U);
}





















/* USER CODE END 1 */
