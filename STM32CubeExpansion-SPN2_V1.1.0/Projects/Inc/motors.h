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