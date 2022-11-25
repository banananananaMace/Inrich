#include "rtc.h"
RTC_HandleTypeDef RTCHandle;
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
        Send_Frame_Type = 1;//刚上电发送数据
        HAL_PWR_EnableBkUpAccess();//使能后备区域访问；
        HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,0);
        HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR5,0);
    }
    else if (READ_REG(RTC->BKP31R) == 1)//从低功耗模式中唤醒
    {
        WRITE_REG( RTC->BKP31R, 0x0 ); //RTC->BKP31R这个值用来记录是否进入低功耗
        HAL_PWR_EnableBkUpAccess();
        Message_Cycle_Count_down = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR1);
        if(Message_Cycle_Count_down==0)
        {
            WakeUpNumber = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR0);
            WakeUpNumber++;
            HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,WakeUpNumber);
            //上下限保护
            if(Message_cycle>30*60*1000)//MESSAGE上限   30分钟（测试值）；
            {
                Message_cycle=(30*60*1000);
            }

            if (WakeUpNumber%(Message_cycle/WakeUp_TimeBasis) == 0 )//每分钟发送一次Message
            {
                Send_Frame_Type = 1;
            }
            if (WakeUpNumber%((Ctrl_cycle*Message_cycle)/WakeUp_TimeBasis) == 0 ) //五分钟发一次req
            {
                Send_Frame_Type = 2;
                HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR0,0);
            }

        }
        else
        {
            Message_Cycle_Count_down--;
            HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR1,Message_Cycle_Count_down);//记录偏移时间的整数部分
        }
    }
}



