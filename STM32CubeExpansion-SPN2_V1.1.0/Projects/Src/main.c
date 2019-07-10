/**	
  ******************************************************************************	
  * File Name          : main.c	
  * Date               : 09/10/2014 11:13:03	
  * Description        : Main program body	
  ******************************************************************************	
  *	
  * COPYRIGHT(c) 2014 STMicroelectronics	
  *	
  * Redistribution and use in source and binary forms, with or without modification,	
  * are permitted provided that the following conditions are met:	
  *   1. Redistributions of source code must retain the above copyright notice,	
  *      this list of conditions and the following disclaimer.	
  *   2. Redistributions in binary form must reproduce the above copyright notice,	
  *      this list of conditions and the following disclaimer in the documentation	
  *      and/or other materials provided with the distribution.	
  *   3. Neither the name of STMicroelectronics nor the names of its contributors	
  *      may be used to endorse or promote products derived from this software	
  *      without specific prior written permission.	
  *	
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"	
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE	
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE	
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE	
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL	
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR	
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER	
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,	
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE	
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.	
  *	
  ******************************************************************************	
  */

#include "xnucleoihm02a1.h"
#include "example_usart.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"
#include "params.h"
#include "stm32f4xx_it.h"
#include "motors.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define MICROSTEPPING_MOTOR_EXAMPLE        //!< Uncomment to performe the standalone example
//#define MICROSTEPPING_MOTOR_USART_EXAMPLE  //!< Uncomment to performe the USART example
#if ((defined (MICROSTEPPING_MOTOR_EXAMPLE)) && (defined (MICROSTEPPING_MOTOR_USART_EXAMPLE)))
  #error "Please select an option only!"
#elif ((!defined (MICROSTEPPING_MOTOR_EXAMPLE)) && (!defined (MICROSTEPPING_MOTOR_USART_EXAMPLE)))
  #error "Please select an option!"
#endif
#if (defined (MICROSTEPPING_MOTOR_USART_EXAMPLE) && (!defined (NUCLEO_USE_USART)))
  #error "Please define "NUCLEO_USE_USART" in "stm32fxxx_x-nucleo-ihm02a1.h"!"
#endif

/**
  * @}
  */ /* End of ExampleTypes */
	
	/* Private Variables ----------------------*/

/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;


/* Private function prototypes -----------------------------------------------*/
//static void SystemClock_Config(void);
//static void Error_Handler(void);
uint16_t Read_ADC(void);
void setup_motors(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void usart_log(uint8_t* string);
void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI4_IRQHandler(void);

volatile motor_direction_t xDirection = X_RIGHT;
volatile motor_direction_t yDirection = Y_UP;
StepperMotorBoardHandle_t *StepperMotorBoardHandle;
MotorParameterData_t *MotorParameterDataGlobal, *MotorParameterDataSingle_X, *MotorParameterDataSingle_Y;

// Helper function to initialize motor handles

void setup_motors(void) {
	Motor_Param_Reg_Init();
	MotorParameterDataGlobal = GetMotorParameterInitData();
	uint8_t id = 0;
	StepperMotorBoardHandle = BSP_GetExpansionBoardHandle(EXPBRD_ID(id));
	StepperMotorBoardHandle->Config(MotorParameterDataGlobal);
	
	MotorParameterDataSingle_X = MotorParameterDataGlobal;
	MotorParameterDataSingle_Y = MotorParameterDataGlobal + 1;
}

/*** Interrupt Handlers ***/

void EXTI9_5_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(Y_MAX_SWITCH_PIN);
		stop_motor(Y_AXIS);
		yDirection = STOP;
		return;
	}
	if (__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(X_MIN_SWITCH_PIN);
		__HAL_GPIO_EXTI_CLEAR_IT(X_MAX_SWITCH_PIN);
		stop_motor(X_AXIS);
		xDirection = STOP;
		return;
	}
	
	
	if (__HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(X_MAX_SWITCH_PIN);
		xDirection = handle_x_max_pressed() ? X_LEFT : X_RIGHT;
	} else if (__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(X_MIN_SWITCH_PIN);
		xDirection = handle_x_min_pressed() ? X_RIGHT: X_LEFT;
	} else if (__HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(Y_MAX_SWITCH_PIN);
		yDirection = handle_y_max_pressed() ? Y_DOWN : Y_UP;
	}
	
}

void EXTI4_IRQHandler(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
		yDirection = STOP;
		return;
	}
	
	if (__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET) {
		 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
		 yDirection = handle_y_min_pressed() ? Y_UP : Y_DOWN;
	}
}


// Wrapper function for the messiness of USART_Transmit
void usart_log(uint8_t* string) {
	USART_Transmit(&huart2, string);
}

void experimentA(uint16_t speed) {
	xDirection = STOP;
	yDirection = BACKWARD;
	spin_motor(speed, BACKWARD, MotorParameterDataSingle_Y, Y_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();

	while (1) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(speed, yDirection, MotorParameterDataSingle_Y, Y_AXIS);
		}
	}
}

void experimentB(uint16_t speed) {
	yDirection = FORWARD;
	xDirection = STOP;
	spin_motor(speed, FORWARD, MotorParameterDataSingle_Y, Y_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();

	while (1) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(speed, yDirection, MotorParameterDataSingle_Y, Y_AXIS);
		}
	}
}

void experimentC(uint16_t speed) {
	xDirection = BACKWARD;
	yDirection = STOP;
	spin_motor(speed, xDirection, MotorParameterDataSingle_X, X_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();

	while (1) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(speed, xDirection, MotorParameterDataSingle_X, X_AXIS);
		}
	}
}

void stopExperimentation(bool hardstop) {
	xDirection = BACKWARD;
	yDirection = STOP;
	spin_motor(100, xDirection, MotorParameterDataSingle_X, X_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();
	// 5 Seconds
	uint16_t duration = 5000;

	while (HAL_GetTick() < startTimestamp + duration) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(100, xDirection, MotorParameterDataSingle_X, X_AXIS);
		}
	}
	if (hardstop) {
		stop_motor(X_AXIS);
	} else {
		soft_stop_motor(X_AXIS);
	}
}

void brownoutTest() {
	xDirection = BACKWARD;
	yDirection = BACKWARD;
	spin_motor(100, xDirection, MotorParameterDataSingle_X, X_AXIS);
	spin_motor(100, yDirection, MotorParameterDataSingle_Y, Y_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();
	// 5 Seconds
	uint16_t duration = 3500;

	while (HAL_GetTick() < startTimestamp + duration) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(100, xDirection, MotorParameterDataSingle_X, X_AXIS);
			spin_motor(100, yDirection, MotorParameterDataSingle_Y, Y_AXIS);
		}
	}
	stop_motor(X_AXIS);
	stop_motor(Y_AXIS);
}

void speedGreaterThanMax() {
		xDirection = BACKWARD;
	yDirection = STOP;
	// Set speed to max 8-bit int
	spin_motor(244, xDirection, MotorParameterDataSingle_X, X_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	uint32_t startTimestamp = HAL_GetTick();
	// 5 Seconds
	uint16_t duration = 5000;

	while (HAL_GetTick() < startTimestamp + duration) {
		check_refractory_period();
		// turn both motors on to forward
		if ((__HAL_GPIO_EXTI_GET_IT(X_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(X_MAX_SWITCH_PIN) != RESET) ||
			(__HAL_GPIO_EXTI_GET_IT(Y_MIN_SWITCH_PIN) != RESET && __HAL_GPIO_EXTI_GET_IT(Y_MAX_SWITCH_PIN) != RESET)) {
			stop_motor(X_AXIS);
			stop_motor(Y_AXIS);
		} else {
			spin_motor(100, xDirection, MotorParameterDataSingle_X, X_AXIS);
		}
	}
	if (hardstop) {
		stop_motor(X_AXIS);
	} else {
		soft_stop_motor(X_AXIS);
	}
}

int main(void)
{
  /* NUCLEO board initialization */
	/* Init for UART, ADC, GPIO and SPI */
  NUCLEO_Board_Init();
  
  /* X-NUCLEO-IHM02A1 initialization */
  BSP_Init();
	
	setup_motors();
	#ifdef NUCLEO_USE_USART
  /* Transmit the initial message to the PC via UART */
  USART_TxWelcomeMessage();
	
	switch_interrupt_init();

	// Insert experiment function here:
	experimentA(20);
	
	#endif
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
}

#endif

/**
  * @brief  This function return the ADC conversion result.
  * @retval The number into the range [0, 4095] as [0, 3.3]V.
  */
uint16_t Read_ADC(void)
{
  HAL_ADC_Start(&HADC);
  HAL_ADC_PollForConversion(&HADC, 100);
  
  return HAL_ADC_GetValue(&HADC);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/