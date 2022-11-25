#ifndef __RTC_H
#define __RTC_H

#include "hw.h"

void RTC_Config(void);
void Handle_WakeUpSource(void);

extern RTC_HandleTypeDef RTCHandle;
extern uint32_t   Message_cycle;
extern uint8_t Send_Message_Flag;
extern uint8_t Send_REQ_Flag;
extern uint16_t   Ctrl_cycle;
extern uint8_t Send_Frame_Type;
//#define SAMPLE_INTERVAL (20*32768/16)   //(5S*32768/16)
#define WakeUp_TimeBasis 15000  //(10s)


#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xFF  /* 32Khz/128 - 1 */
#define REPEAT_TIMES   0







#endif












