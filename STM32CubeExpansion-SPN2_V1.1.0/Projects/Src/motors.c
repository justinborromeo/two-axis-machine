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

motor_control_pair_t motor_speed_from_pot_reading(uint16_t potReading) {
	motor_control_pair_t returnedMotorControl;
	uint16_t adcRangeStep = (ADC_MAX - ADC_MIN) / 5;
	if (potReading < adcRangeStep) {
		// Reverse, 25 - 100
		returnedMotorControl.direction = BACKWARD;
		returnedMotorControl.speed_percentage = (-7500 / adcRangeStep) * potReading / 100 + 100;
	} else if (potReading < adcRangeStep * 2) {
		// Reverse, 0 - 25
		returnedMotorControl.direction = BACKWARD;
		returnedMotorControl.speed_percentage = (-2500 / adcRangeStep) * potReading / 100 + 50;
	} else if (potReading < adcRangeStep * 3) {
		// Stop
		returnedMotorControl.direction = STOP;
		returnedMotorControl.speed_percentage = 0;
	} else if (potReading < adcRangeStep * 4) {
		// Forward, 0 - 25
		returnedMotorControl.direction = FORWARD;
		returnedMotorControl.speed_percentage = (2500 / adcRangeStep) * potReading / 100 - 75;
	} else { // This is borked (25 - 93) but shouldn't really matter
		// Forward, 25 - 100
		returnedMotorControl.direction = FORWARD;
		returnedMotorControl.speed_percentage = (7500 / adcRangeStep) * potReading / 100 - 275;
	}
	
	return returnedMotorControl;
}
