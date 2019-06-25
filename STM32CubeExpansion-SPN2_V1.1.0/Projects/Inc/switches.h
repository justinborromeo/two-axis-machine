#include "stdint.h"

#define X_MAX_SWITCH_PIN GPIO_PIN_8
#define X_MIN_SWITCH_PIN GPIO_PIN_9
#define Y_MAX_SWITCH_PIN GPIO_PIN_1 // TODO Find correct pin
#define Y_MIN_SWITCH_PIN GPIO_PIN_2 // TODO Find correct pin

void Switch_Interrupt_Init(void);