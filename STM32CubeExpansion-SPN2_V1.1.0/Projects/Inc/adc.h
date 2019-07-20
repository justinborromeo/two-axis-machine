#include <stdint.h>

#define ADC_ERROR 0xFFFF

struct {
	uint16_t xPotentiometerReading;
	uint16_t yPotentiometerReading;
} typedef PotentiometerReading_t;

PotentiometerReading_t Read_ADC(void);
