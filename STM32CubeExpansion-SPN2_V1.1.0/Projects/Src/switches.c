#include "switches.h"
#include "xnucleoihm02a1.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"

// These should not be 0 during each axis' refractory period
volatile uint32_t lastXMinPressedTimestamp = 0,
									lastXMaxPressedTimestamp = 0, 
									lastYMinPressedTimestamp = 0,
									lastYMaxPressedTimestamp = 0;

void switch_interrupt_init(void) {
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

reversal_needed_t handle_x_min_pressed(void) {
	if (lastXMinPressedTimestamp == 0) {
		// Reversal needed
		lastXMinPressedTimestamp = HAL_GetTick();
		return true;
	}
	return false;
}

reversal_needed_t handle_x_max_pressed(void) {
	if (lastXMaxPressedTimestamp == 0) {
		// Reversal needed
		lastXMaxPressedTimestamp = HAL_GetTick();
		return true;
	}
	return false;
}

reversal_needed_t handle_y_min_pressed(void) {
	if (lastYMinPressedTimestamp == 0) {
		// Reversal needed
		lastYMinPressedTimestamp = HAL_GetTick();
		return true;
	}
	return false;
}

reversal_needed_t handle_y_max_pressed(void) {
	if (lastYMaxPressedTimestamp == 0) {
		// Reversal needed
		lastYMaxPressedTimestamp = HAL_GetTick();
		return true;
	}
	return false;
}

// Check if the refractory period has elapsed since initial press.  If so, set the corresponding pressed timestamp to 0
void check_refractory_period(void) {
	uint32_t currentTimestamp = HAL_GetTick();
	if (lastXMinPressedTimestamp != 0 && lastXMinPressedTimestamp < currentTimestamp - INTERRUPT_REFRACTORY_PERIOD_IN_MS) {
		lastXMinPressedTimestamp = 0;
	}
	if (lastYMinPressedTimestamp != 0 && lastYMinPressedTimestamp < currentTimestamp - INTERRUPT_REFRACTORY_PERIOD_IN_MS) {
		lastYMinPressedTimestamp = 0;
	} 
	if (lastXMaxPressedTimestamp != 0 && lastXMaxPressedTimestamp < currentTimestamp - INTERRUPT_REFRACTORY_PERIOD_IN_MS) {
		lastXMaxPressedTimestamp = 0;
	} 
	if (lastYMaxPressedTimestamp != 0 && lastYMaxPressedTimestamp < currentTimestamp - INTERRUPT_REFRACTORY_PERIOD_IN_MS) {
		lastYMaxPressedTimestamp = 0;
	}
}
