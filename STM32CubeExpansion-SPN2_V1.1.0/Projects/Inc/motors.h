#include <stdint.h>

enum {
	FORWARD = 0,
	BACKWARD = 1
} typedef motor_direction_t;

// TODO fix speed param
void spinMotorX(uint32_t speed, motor_direction_t direction);
