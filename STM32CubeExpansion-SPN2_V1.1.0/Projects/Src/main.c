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

#define TEST_MOTOR	//!< Comment out this line to test the ADC

/**
  * @defgroup   MotionControl
  * @{
  */

/**
  * @addtogroup BSP
  * @{
  */

/**
  * @}
  */ /* End of BSP */

/**
  * @addtogroup MicrosteppingMotor_Example
  * @{
  */

/**
  * @defgroup   ExampleTypes
  * @{
  */

//#define MICROSTEPPING_MOTOR_EXAMPLE        //!< Uncomment to performe the standalone example
#define MICROSTEPPING_MOTOR_USART_EXAMPLE  //!< Uncomment to performe the USART example
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
static void Error_Handler(void);
uint16_t Read_ADC(void);

/**
  * @brief The FW main module
  */
int main(void)
{
  /* NUCLEO board initialization */
	/* Init for UART, ADC, GPIO and SPI */
  NUCLEO_Board_Init();
  
  /* X-NUCLEO-IHM02A1 initialization */
  BSP_Init();
	
	#ifdef NUCLEO_USE_USART
  /* Transmit the initial message to the PC via UART */
  USART_TxWelcomeMessage();
#endif
	
#if defined (MICROSTEPPING_MOTOR_EXAMPLE)
  /* Perform a batch commands for X-NUCLEO-IHM02A1 */
  MicrosteppingMotor_Example_01();
  
  /* Infinite loop */
  while (1);
#elif defined (MICROSTEPPING_MOTOR_USART_EXAMPLE)
  /* Fill the L6470_DaisyChainMnemonic structure */
  Fill_L6470_DaisyChainMnemonic();
	
	/*Initialize the motor parameters */
	Motor_Param_Reg_Init();
	
	/**** Lab 1 ****/
	/*
  GPIO_InitTypeDef GPIO_InitStruct8;
	GPIO_InitTypeDef GPIO_InitStruct9;
	
	GPIO_InitStruct8.Pin = GPIO_PIN_8;
	GPIO_InitStruct8.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct8.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct8.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct9.Pin = GPIO_PIN_9;
	GPIO_InitStruct9.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct9.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct9.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct8);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct9);
	*/

	/**** Lab 2 ****/
	GPIO_InitTypeDef GPIO_InitStruct8;
	GPIO_InitTypeDef GPIO_InitStruct9;

  GPIO_InitStruct8.Pin = GPIO_PIN_8;
  GPIO_InitStruct8.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct8.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct8.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	GPIO_InitStruct9.Pin = GPIO_PIN_9;
	GPIO_InitStruct9.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct9.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct9.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct8);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct9);

  while (1)
  { 
		/**** Lab 1 ****/
		/*
		GPIO_PinState pintest = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1 - pintest);
		*/
		/**** Lab 2 ****/
		
		GPIO_PinState pin8State = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1 - pin8State);
		// TODO is this what it means to do to turn off the LED before the next rising edge?
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
		
/*		
#ifdef TEST_MOTOR		

    USART_CheckAppCmd();
		
#else
		
		uint16_t myADCVal;
		myADCVal = Read_ADC();
		USART_Transmit(&huart2, " ADC Read: ");
	  USART_Transmit(&huart2, num2hex(myADCVal, WORD_F));
	  USART_Transmit(&huart2, " \n\r");
#endif		
*/
  }
#endif
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while (1)
  {
  }
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
