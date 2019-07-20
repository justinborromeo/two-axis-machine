#include "stdlib.h"
#include "L6470.h"
#include "xnucleoihm02a1.h"
#include "example_usart.h"
#include "stm32f4xx_hal_gpio.h"
#include "switches.h"

enum {
	FORWARD = 0, // LEFT for X, DOWN for Y
	BACKWARD = 1, // RIGHT FOR X, UP for Y
	STOP = 2
} typedef motor_direction_t;

#define X_LEFT FORWARD
#define X_RIGHT BACKWARD
#define Y_UP BACKWARD
#define Y_DOWN FORWARD

void spin_motor(uint8_t speed_percentage,
							 motor_direction_t direction,
							 MotorParameterData_t* MotorParameterDataSingle,
							 axis_t axis);

void stop_motor(axis_t axis);

#define ADC_MIN 0x0000
#define ADC_MAX 0x0FFF

struct {
	uint16_t speed_percentage;
	motor_direction_t direction;
} typedef motor_control_pair_t;

// Assumes 12 bit precision
motor_control_pair_t motor_speed_from_pot_reading(uint16_t potReading);