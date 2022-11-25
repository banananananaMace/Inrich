#include "sys.h"
#include "main.h"
#include "exti.h"
#include "delay.h"
#include "key.h"
#include "FreeRTOS.h"
#include "event_groups.h"
//////////////////////////////////////////////////////////////////////////////////
#include "sx1280_hw-gpio.h"
//外部中断1服务程序，用于SX1280_0
#include "sx1280.h"


typedef long BaseType_t;
BaseType_t xHigherPriorityTaskWoken;
extern EventGroupHandle_t SX1280RX_EventGroupHandler;
//
void EXTI9_5_IRQHandler(void)//
{
    uint8_t Result=0;    
    if(EXTI_GetITStatus(EXTI_Line8)!=RESET)//sx1280    2号完成
    {
        //GpioLaunchIrqHandler(GPIO_Pin_5);
        //	 xTaskNotifyFromISR( SX1280_ISRTask_Handler,EVENTBIT_SX1280_NODE2_ISR,1,&xHigherPriorityTaskWoken);
        Result=xEventGroupSetBitsFromISR(SX1280RX_EventGroupHandler,EVENTBIT_SX1280_SENS2_ISR,&xHigherPriorityTaskWoken);//事件组
        if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
        EXTI_ClearITPendingBit(EXTI_Line8); //清除LINE0上的中断标志位
    }
}

void EXTI15_10_IRQHandler(void)
{
    uint8_t Result=0;
    if(EXTI_GetITStatus(EXTI_Line14)!=RESET)//0号
    {

        //Result=xTaskNotifyFromISR(SX1280RX_EventGroupHandler,EVENTBIT_SX1280_SENS0_ISR,1,&xHigherPriorityTaskWoken);
        Result=xEventGroupSetBitsFromISR(SX1280RX_EventGroupHandler,EVENTBIT_SX1280_SENS0_ISR,&xHigherPriorityTaskWoken);//事件组
        if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //		portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
        EXTI_ClearITPendingBit(EXTI_Line14); //清除LINE0上的中断标志位
    }

    if(EXTI_GetITStatus(EXTI_Line12)!=RESET)//1号  进入中断
    {


         //Result=xTaskNotifyFromISR( SX1280RX_EventGroupHandler,EVENTBIT_SX1280_SENS1_ISR,1,&xHigherPriorityTaskWoken);
        Result=xEventGroupSetBitsFromISR(SX1280RX_EventGroupHandler,EVENTBIT_SX1280_SENS1_ISR,&xHigherPriorityTaskWoken);//事件组
        if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
        EXTI_ClearITPendingBit(EXTI_Line12); //清除LINE0上的中断标志位
    }
}







