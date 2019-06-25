#include "switches.h"

void Switch_Interrupt_Init(void) {
	GPIO_InitTypeDef XMaxSwitchInitStruct;
	GPIO_InitTypeDef XMinSwitchInitStruct;
	
	// Set up interrupts on x-axis
	XMaxSwitchInitStruct.Pin = X_MAX_SWITCH_PIN;
  XMaxSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
  XMaxSwitchInitStruct.Pull = GPIO_NOPULL;
  XMaxSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	XMinSwitchInitStruct.Pin = X_MIN_SWITCH_PIN;
	XMinSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
	XMinSwitchInitStruct.Pull = GPIO_PULLDOWN;
	XMinSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &XMaxSwitchInitStruct);
	HAL_GPIO_Init(GPIOA, &XMinSwitchInitStruct);

  HAL_NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn));
	
	GPIO_InitTypeDef XMaxSwitchInitStruct;
	GPIO_InitTypeDef XMinSwitchInitStruct;
	
	// Set up interrupts on y-axis
	YMaxSwitchInitStruct.Pin = Y_MAX_SWITCH_PIN;
  YMaxSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
  YMaxSwitchInitStruct.Pull = GPIO_NOPULL;
  YMaxSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	YMinSwitchInitStruct.Pin = Y_MIN_SWITCH_PIN;
	YMinSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
	YMinSwitchInitStruct.Pull = GPIO_PULLDOWN;
	YMinSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &YMaxSwitchInitStruct);
	HAL_GPIO_Init(GPIOA, &YMinSwitchInitStruct);

  HAL_NVIC_SetPriority((IRQn_Type)(EXTI9_5_IRQn), 0x0F, 0x00); // TODO Change to correct handler
  HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI9_5_IRQn)); // TODO Change to correct handler
}

