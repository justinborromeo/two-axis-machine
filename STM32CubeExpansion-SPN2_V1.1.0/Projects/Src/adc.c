#include "adc.h"
#include "L6470.h"
#include "example_usart.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"

PotentiometerReading_t Read_ADC(void)
{
	PotentiometerReading_t reading;
  HAL_ADC_Start(&HADC);
	if (HAL_ADC_PollForConversion(&HADC, 100) == HAL_OK) {
		reading.xPotentiometerReading = (HAL_ADC_GetValue(&HADC) & 0x0000FFFF);
  } else {
		reading.xPotentiometerReading = ADC_ERROR;
	}
	
	if (HAL_ADC_PollForConversion(&HADC, 100) == HAL_OK) {
		reading.yPotentiometerReading = (HAL_ADC_GetValue(&HADC) & 0x0000FFFF);
  } else {
		reading.xPotentiometerReading = ADC_ERROR;
	} 

  HAL_ADC_Stop(&HADC);
  return reading;
}
