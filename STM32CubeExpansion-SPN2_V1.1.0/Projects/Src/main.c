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
#include "example.h"
#include "example_usart.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"
#include "params.h"
#include "stm32f4xx_it.h"
#include "switches.h"
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
void setupMotors(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

typedef enum {
	MIN = 0,
	MAX,
	MIDDLE
} platform_position_t;

volatile platform_position_t xPosition = MIDDLE, yPosition = MIDDLE;
volatile motor_direction_t xDirection = FORWARD, yDirection = FORWARD;
StepperMotorBoardHandle_t *StepperMotorBoardHandle;
MotorParameterData_t *MotorParameterDataGlobal, *MotorParameterDataSingle_X, *MotorParameterDataSingle_Y;

// Helper function to initialize motor handles

void setupMotors(void) {
	Motor_Param_Reg_Init();
	MotorParameterDataGlobal = GetMotorParameterInitData();
	uint8_t id = 0;
	StepperMotorBoardHandle = BSP_GetExpansionBoardHandle(EXPBRD_ID(id));
	StepperMotorBoardHandle->Config(MotorParameterDataGlobal);
	
	MotorParameterDataSingle_X = MotorParameterDataGlobal;
	MotorParameterDataSingle_Y = MotorParameterDataGlobal + 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == X_MAX_SWITCH_PIN) {
		xPosition = MAX;
	} else if (GPIO_Pin == X_MIN_SWITCH_PIN) {
		xPosition = MIN;
	} else if (GPIO_Pin == Y_MAX_SWITCH_PIN) {
		yPosition = MAX;
	} else if (GPIO_Pin == Y_MIN_SWITCH_PIN) {
		yPosition = MIN;
	} else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
    BSP_EmergencyStop();
  }
}

int main(void)
{
  /* NUCLEO board initialization */
	/* Init for UART, ADC, GPIO and SPI */
  NUCLEO_Board_Init();
  
  /* X-NUCLEO-IHM02A1 initialization */
  BSP_Init();
	
	setupMotors();
	#ifdef NUCLEO_USE_USART
  /* Transmit the initial message to the PC via UART */
  USART_TxWelcomeMessage();
	
	switch_interrupt_init();
	
	while (1) {
		
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
  * @brief  This function return the ADC conversion result.
  * @retval The number into the range [0, 4095] as [0, 3.3]V.
  */
uint16_t Read_ADC(void)
{
  HAL_ADC_Start(&HADC);
  HAL_ADC_PollForConversion(&HADC, 100);
  
  return HAL_ADC_GetValue(&HADC);
}

/**
  * @}
  */ /* End of MicrosteppingMotor_Example */

/**
  * @}
  */ /* End of MotionControl */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
