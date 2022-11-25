#ifndef __ADC_H
#define __ADC_H
#include "hw.h"

void MX_ADC1_Init(void);
void ADC1toChanel_8(void);
void ADC1toChanel_Ref(void);
void Battery_Voltage_Measure(void);

extern uint8_t Buffer[10];
extern ADC_HandleTypeDef hadc1;









#endif




