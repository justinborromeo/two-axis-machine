#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#define SWITCH_INTERRUPT_PRIORITY 0x01
#define INTERRUPT_REFRACTORY_PERIOD_IN_MS 0x01F4

// PA 8
#define X_MAX_SWITCH_PIN GPIO_PIN_8 
#define X_MAX_SWITCH_PORT GPIOA
#define X_MAX_SWITCH_IRQ EXTI9_5_IRQn

// PA 9
#define X_MIN_SWITCH_PIN GPIO_PIN_9
#define X_MIN_SWITCH_PORT GPIOA
#define X_MIN_SWITCH_IRQ EXTI9_5_IRQn

// PA 1
#define Y_MAX_SWITCH_PIN GPIO_PIN_1
#define Y_MAX_SWITCH_PORT GPIOA
#define Y_MAX_SWITCH_IRQ EXTI1_IRQn

// PB 4
#define Y_MIN_SWITCH_PIN GPIO_PIN_4
#define Y_MIN_SWITCH_PORT GPIOB
#define Y_MIN_SWITCH_IRQ EXTI4_IRQn

typedef bool reversal_needed_t;

enum {
	X_AXIS = 0,
	Y_AXIS = 1
} typedef axis_t;

void switch_interrupt_init(void);

reversal_needed_t handle_x_min_pressed(void);
reversal_needed_t handle_x_max_pressed(void);
reversal_needed_t handle_y_min_pressed(void);
reversal_needed_t handle_y_max_pressed(void);

void check_refractory_period(void);
