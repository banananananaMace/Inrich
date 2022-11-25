#ifndef __ADC_H
#define __ADC_H
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_adc.h"
#include "hw.h"
void MX_ADC1_Init(void);
void ADC1toChanel_Ref(void);
void ADC1toChanel_8(void);
void ADC1toChanel_5(void);
void ADC1toChanel_7(void);
void Battery_Voltage_Measure(void);
void ShapeChange_Measure(void);

extern uint8_t Send_Frame_Type;
extern __IO uint16_t uhADCxRefValue;
extern __IO uint16_t uhADCxConvertedValue;
extern uint8_t Buffer[15];
extern float PA0_ADCxRValue;
extern float PA2_ADCxRValue;
extern uint8_t	Send_BURST_Flag;
extern uint8_t Send_Message_Flag;
extern RTC_HandleTypeDef RTCHandle;
extern float First_Shape_Change_Data;


#endif









