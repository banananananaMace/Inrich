#ifndef __RTC_H
#define __RTC_H
#include "hw.h"





#define Wake_up_time 10000  //(10s)
#define First_wake_up_time (20*32768/16) //16位 最多30s唤醒时间
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xFF  /* 32Khz/128 - 1 */
#define SAMPLE_INTERVAL (10*32768/16)   //(5S*32768/16)


extern RTC_HandleTypeDef RTCHandle;
extern uint32_t Repeat_Count;      //唤醒次数
extern uint8_t   wake_up_flag;
extern uint32_t One_Cycle;
extern uint8_t Send_Frame_Type;
extern uint32_t   Message_cycle,
           Offset_Delay;   //暂时没有用到，地址留着备用
extern uint16_t   Ctrl_cycle;
//extern uint32_t  RTC_SleepTime;
extern uint32_t  Sample_Interval;
void RTC_Config(void);
void Handle_WakeUpSource(void);

void Wakeup_Source_Handle (void);










#endif




