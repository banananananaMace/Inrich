#ifndef __RNG_H
#define __RNG_H
#include "hw.h"



uint32_t Get_random(void);
extern RNG_HandleTypeDef RngHandle;
extern uint32_t   RTC_SleepTime;
#endif





