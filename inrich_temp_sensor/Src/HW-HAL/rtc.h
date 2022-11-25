#ifndef __RTC_H
#define __RTC_H
#include "hw.h"

#define WakeUp_TimeBasis 15000  //(15s) 每次唤醒时间

//#define SAMPLE_INTERVAL (20*32768/16)   //(5S*32768/16)
void RTC_Config(void);
void Handle_WakeUpSource(void);
void Handle_Burst_Source(void);

extern RTC_HandleTypeDef RTCHandle;
extern uint32_t Message_Cycle_Count_down;
extern uint32_t WakeUpNumber;
extern uint32_t Message_cycle;
extern uint16_t Ctrl_cycle;
extern uint8_t Send_Frame_Type;
extern float Alarm_threshold;
extern float Alarm_threshold_DOWN;
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xFF


#endif



