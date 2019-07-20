#include "adc.h"
#include "L6470.h"
#include "example_usart.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"

uint32_t Read_ADC(void)
{
	uint32_t newReads = 0, redValue = 0;
  HAL_ADC_Start(&HADC);
	if (HAL_ADC_PollForConversion(&HADC, 100) == HAL_OK) {
		redValue = HAL_ADC_GetValue(&HADC);
    newReads = (redValue & 0x0000FFFF);
  }
	
	if (HAL_ADC_PollForConversion(&HADC, 100) == HAL_OK) {
		redValue = HAL_ADC_GetValue(&HADC);
    newReads += (redValue & 0x0000FFFF) << 16;
  }
  HAL_ADC_Stop(&HADC);
  return newReads;
}
