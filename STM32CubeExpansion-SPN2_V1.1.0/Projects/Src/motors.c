#include "motors.h"
#include "L6470.h"
#include "example_usart.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "microstepping_motor.h"


void spin_motor(uint8_t speed_percentage,
							 motor_direction_t direction,
							 MotorParameterData_t* MotorParameterDataSingle,
							 axis_t axis) {
	eL6470_DirId_t directionId = L6470_DIR_FWD_ID;
	if (direction == BACKWARD) {
		directionId = L6470_DIR_REV_ID;
	}
	if (direction != STOP) {	
		// 67 is approximately equal to (250e-9/2^-28)
		L6470_Run(axis, directionId, (MotorParameterDataSingle -> maxspeed) * 67 * speed_percentage / 100);
	} else {
		stop_motor(axis);
	}
}

void stop_motor(axis_t axis) {
	L6470_HardStop(axis);
}

void soft_stop_motor(axis_t axis) {
	L6470_SoftStop(axis);
}
