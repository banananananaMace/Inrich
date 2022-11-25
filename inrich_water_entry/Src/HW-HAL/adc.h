#ifndef __ADC_H
#define __ADC_H
#include "hw.h"

void MX_ADC1_Init(void);
void ADC1toChanel_8(void);
void ADC1toChanel_Ref(void);
void Battery_Voltage_Measure(void);
extern uint8_t	Send_BURST_Flag;
extern RTC_HandleTypeDef RTCHandle;
extern uint8_t Send_Frame_Type;
extern uint8_t Buffer[10];













#endif













