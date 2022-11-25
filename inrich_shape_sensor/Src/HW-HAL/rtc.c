#include "rtc.h"
#include "adc.h"
#include "protocol.h"
RTC_HandleTypeDef RTCHandle;
uint32_t Message_Cycle_Count_down=0;
uint32_t WakeUpNumber=0;
extern float First_Shape_Change_Data;
void RTC_Config(void)
{
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
        Error_Handler();
    }
}

void Handle_WakeUpSource(void)
{
    if(READ_REG(RTC->BKP31R) == 0)//刚刚上电运行
    {
        Send_Frame_Type = 1;//刚上电不发送数据
        HAL_PWR_EnableBkUpAccess();
        ShapeChange_Measure();
        HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR18,First_Shape_Change_Data);//REPEAT_TIMES
        HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,REPEAT_TIMES);
    }
    else if (READ_REG(RTC->BKP31R) == 1)//从低功耗模式中唤醒
    {
        /* reset back-up register */
        WRITE_REG( RTC->BKP31R, 0x0 ); //RTC->BKP31R这个值用来记录是否进入低功耗
        HAL_PWR_EnableBkUpAccess();
        Message_Cycle_Count_down = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR1);
        RESET_FLAG = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR25);
        if(Message_Cycle_Count_down==0)
        {
            WakeUpNumber = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR0);
            WakeUpNumber++;
            HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,WakeUpNumber);
            if (WakeUpNumber%(Message_cycle/WakeUp_TimeBasis) == 0 )  //每隔1分钟发送一次message
            {
                Send_Frame_Type = 1;  //温度 message进行发送
            }
            if (WakeUpNumber%((Ctrl_cycle*Message_cycle)/WakeUp_TimeBasis) == 0 )  //每隔1分钟发送一次message
            {
                Send_Frame_Type = 2;
                HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,0);
            }
            
            if (WakeUpNumber%((Reset__cycle*Message_cycle)/WakeUp_TimeBasis) == 0 )  //每隔1分钟发送一次message
            {
                if(RESET_FLAG==0)
                {
                Send_Frame_Type = 3;
                }
                else
                {
                Send_Frame_Type = 1;
                }
            }
        }
        else
        {
            Message_Cycle_Count_down--;
            HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR1,Message_Cycle_Count_down);
        }
    }
}