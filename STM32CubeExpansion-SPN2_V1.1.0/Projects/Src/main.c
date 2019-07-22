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
#include "adc.h"

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

// Interrupt Handlers
/*
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
*/

// Wrapper function for the messiness of USART_Transmit
void usart_log(uint8_t* string) {
	USART_Transmit(&huart2, string);
}

void log_marijuana_readings(PotentiometerReading_t readings) {
	usart_log(num2hex(readings.xPotentiometerReading, WORD_F));
	usart_log((uint8_t*) "\t");
	usart_log(num2hex(readings.yPotentiometerReading, WORD_F));
	usart_log((uint8_t*) "\n\r");
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
	spin_motor(25, FORWARD, MotorParameterDataSingle_X, X_AXIS);
	spin_motor(25, FORWARD, MotorParameterDataSingle_Y, Y_AXIS);
	usart_log((uint8_t*) "Entering loop");
	
	while (1) {
		check_refractory_period();
		// turn both motors on to forward
		PotentiometerReading_t marijuanaReadings = Read_ADC();
		//log_marijuana_readings(marijuanaReadings);
		uint16_t xSpeed = motor_speed_from_pot_reading(marijuanaReadings.xPotentiometerReading).speed_percentage;
		uint16_t ySpeed = motor_speed_from_pot_reading(marijuanaReadings.yPotentiometerReading).speed_percentage;
		xDirection = motor_speed_from_pot_reading(marijuanaReadings.xPotentiometerReading).direction;
		yDirection = motor_speed_from_pot_reading(marijuanaReadings.yPotentiometerReading).direction;
		
		usart_log(num2hex(xSpeed, WORD_F));
		usart_log((uint8_t*) "\t");
		usart_log(num2hex(ySpeed, WORD_F));
		usart_log((uint8_t*) "\n\r");
		
		if (HAL_GPIO_ReadPin(Y_MIN_SWITCH_PORT, Y_MIN_SWITCH_PIN) == 1) {
			usart_log((uint8_t*) "Y Min Triggered");
			if (yDirection == Y_DOWN) {
				yDirection = STOP;
			}
		}
		if (HAL_GPIO_ReadPin(Y_MAX_SWITCH_PORT, Y_MAX_SWITCH_PIN) == 1) {
			usart_log((uint8_t*) "Y Max Triggered");
			if (yDirection == Y_UP) {
				yDirection = STOP;
			}
		}
		if (HAL_GPIO_ReadPin(X_MIN_SWITCH_PORT, X_MIN_SWITCH_PIN) == 1) {
			usart_log((uint8_t*) "X Min Triggered");
			if (xDirection == X_LEFT) {
				xDirection = STOP;
			}
		}
		if (HAL_GPIO_ReadPin(X_MAX_SWITCH_PORT, X_MAX_SWITCH_PIN) == 1) {
			usart_log((uint8_t*) "X Max Triggered");
			if (xDirection == X_RIGHT) {
				xDirection = STOP;
			}
		}
		spin_motor(ySpeed, yDirection, MotorParameterDataSingle_Y, Y_AXIS);
		spin_motor(xSpeed, xDirection, MotorParameterDataSingle_X, X_AXIS);
	}
	#endif
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif



/**
  * @}
  */ /* End of MicrosteppingMotor_Example */

/**
  * @}
  */ /* End of MotionControl */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
