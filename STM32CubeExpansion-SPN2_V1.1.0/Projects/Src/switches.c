#include "switches.h"
#include "xnucleoihm02a1.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"

void Switch_Interrupt_Init(void) {
	// X Max Switch Interrupt
	GPIO_InitTypeDef XMaxSwitchInitStruct;
	XMaxSwitchInitStruct.Pin = X_MAX_SWITCH_PIN;
  XMaxSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
  XMaxSwitchInitStruct.Pull = GPIO_NOPULL;
  XMaxSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(X_MAX_SWITCH_PORT, &XMaxSwitchInitStruct);
	
	HAL_NVIC_SetPriority((IRQn_Type)(X_MAX_SWITCH_IRQ), SWITCH_INTERRUPT_PRIORITY, SWITCH_INTERRUPT_PRIORITY);
  HAL_NVIC_EnableIRQ((IRQn_Type)(X_MAX_SWITCH_IRQ));
	
	// X Min Switch Interrupt
	GPIO_InitTypeDef XMinSwitchInitStruct;
	XMinSwitchInitStruct.Pin = X_MIN_SWITCH_PIN;
	XMinSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
	XMinSwitchInitStruct.Pull = GPIO_NOPULL;
	XMinSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(X_MIN_SWITCH_PORT, &XMinSwitchInitStruct);

  HAL_NVIC_SetPriority((IRQn_Type)(X_MIN_SWITCH_IRQ), SWITCH_INTERRUPT_PRIORITY, SWITCH_INTERRUPT_PRIORITY);
  HAL_NVIC_EnableIRQ((IRQn_Type)(X_MIN_SWITCH_IRQ));
	
	// Y Max Switch Interrupt
	GPIO_InitTypeDef YMaxSwitchInitStruct;
	YMaxSwitchInitStruct.Pin = Y_MAX_SWITCH_PIN;
  YMaxSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
  YMaxSwitchInitStruct.Pull = GPIO_NOPULL;
  YMaxSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(Y_MAX_SWITCH_PORT, &YMaxSwitchInitStruct);
	HAL_NVIC_SetPriority((IRQn_Type)(Y_MAX_SWITCH_IRQ), SWITCH_INTERRUPT_PRIORITY, SWITCH_INTERRUPT_PRIORITY);
  HAL_NVIC_EnableIRQ((IRQn_Type)(Y_MAX_SWITCH_IRQ));
	
	// Y Min Switch Interrupt
	GPIO_InitTypeDef YMinSwitchInitStruct;
	YMinSwitchInitStruct.Pin = Y_MIN_SWITCH_PIN;
	YMinSwitchInitStruct.Mode = GPIO_MODE_IT_RISING;
	YMinSwitchInitStruct.Pull = GPIO_NOPULL;
	YMinSwitchInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(Y_MIN_SWITCH_PORT, &YMinSwitchInitStruct);
  HAL_NVIC_SetPriority((IRQn_Type)(Y_MIN_SWITCH_IRQ), SWITCH_INTERRUPT_PRIORITY, SWITCH_INTERRUPT_PRIORITY);
  HAL_NVIC_EnableIRQ((IRQn_Type)(Y_MIN_SWITCH_IRQ));
}
