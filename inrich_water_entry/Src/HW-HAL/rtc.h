#ifndef __RTC_H
#define __RTC_H
#include "hw.h"

//#define SAMPLE_INTERVAL (20*32768/16)   //(5S*32768/16)
#define WakeUp_TimeBasis 15000  //(10s)


void RTC_Config(void);
void Handle_WakeUpSource(void);



/*外部变量*/
extern RTC_HandleTypeDef RTCHandle;
extern uint32_t   Message_cycle;
extern uint16_t   Ctrl_cycle;
extern uint8_t Send_Frame_Type;


/*宏定义*/
#define REPEAT_TIMES   0
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xFF
#endif














