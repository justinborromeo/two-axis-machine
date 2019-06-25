#include "xnucleoihm02a1.h"
#include "example.h"
#include "example_usart.h"
#include "stm32f4xx_hal_adc.h"
#include "params.h"
#include "stm32f4xx_it.h"

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




StepperMotorBoardHandle_t *StepperMotorBoardHandle;
	
void stopMotorHoriz () {
	StepperMotorBoardHandle->Command->HardStop(EXPBRD_ID(0), L6470_ID(0));
	//HAL_Delay(3000);
}

void stopMotorVert () {
	StepperMotorBoardHandle->Command->HardStop(EXPBRD_ID(1), L6470_ID(1));
	//HAL_Delay(3000);
}
	
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
	
	/* POLL */
	/*
  GPIO_InitStruct8.Pin = GPIO_PIN_8;
  GPIO_InitStruct8.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct8.Pull = GPIO_PULLDOWN; // PULLDOWN if 3.3 -> data, PULLUP if data -> ground
  GPIO_InitStruct8.Speed = GPIO_SPEED_FREQ_HIGH;
	*/
	
	/* INTERRUPT */
	
	GPIO_InitStruct8.Pin = GPIO_PIN_8;
  GPIO_InitStruct8.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct8.Pull = GPIO_NOPULL;
  GPIO_InitStruct8.Speed = GPIO_SPEED_FREQ_HIGH;
	
	GPIO_InitStruct9.Pin = GPIO_PIN_9;
	GPIO_InitStruct9.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct9.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct9.Speed = GPIO_SPEED_FREQ_HIGH;


	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct8);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct9);
  // Lowest Priority

  HAL_NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));
	
	#define MPR_1     4			  //!< Motor Movements Per Revolution 1st option
	#define MPR_2     8			  //!< Motor Movements Per Revolution 2nd option
	#define DELAY_1   5000		//!< Delay time 1st option
	#define DELAY_2   2500		//!< Delay time 2nd option
	#define DELAY_3   10000   //!< Delay time 3rd option
		
	/* Setup each X-NUCLEO-IHM02A1 Expansion Board ******************************/
  
	
	uint32_t Step;
	uint32_t Speed;
	uint8_t MovementPerRevolution;
	uint8_t i;
	uint8_t id;
	uint8_t board, device;

	MotorParameterData_t *MotorParameterDataGlobal, *MotorParameterDataSingle;
  /* Get the parameters for the motor connected with the 1st stepper motor driver of the 1st stepper motor expansion board */
  MotorParameterDataGlobal = GetMotorParameterInitData();
  
  for (id = 0; id < EXPBRD_MOUNTED_NR; id++)
  {
    StepperMotorBoardHandle = BSP_GetExpansionBoardHandle(EXPBRD_ID(id));
    MotorParameterDataSingle = MotorParameterDataGlobal+(id*L6470DAISYCHAINSIZE);
    StepperMotorBoardHandle->Config(MotorParameterDataSingle);
  }
  
  /****************************************************************************/
  
		
  while (1)
  { 
		/**** Lab 1 ****/
		/*
		GPIO_PinState pintest = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1 - pintest);
		*/
		/**** Lab 2 ****/
		/*
		GPIO_PinState pin8State = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, pin8State);
		*/


		MovementPerRevolution = MPR_1;
		board = EXPBRD_ID(0);
	
		StepperMotorBoardHandle = BSP_GetExpansionBoardHandle(board);
		// Iterating through each motor
		for (device = L6470_ID(0); device <= L6470_ID(L6470DAISYCHAINSIZE-1); device++)
		{
			/* Get the parameters for the motor connected with the actual stepper motor driver of the actual stepper motor expansion board */
			MotorParameterDataSingle = MotorParameterDataGlobal+((board*L6470DAISYCHAINSIZE)+device);
			Step = ((uint32_t)MotorParameterDataSingle->fullstepsperrevolution * pow(2, MotorParameterDataSingle->step_sel)) / MovementPerRevolution;
			
			for (i=0; i<MovementPerRevolution; i++)
			{
				StepperMotorBoardHandle->Command->Run(board, device, L6470_DIR_FWD_ID, Step);
				//HAL_Delay(4000);
				//StepperMotorBoardHandle->Command->HardStop(board, device);
			}
		}
		
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
