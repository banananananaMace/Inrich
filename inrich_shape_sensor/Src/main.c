//2019-06-13  形变代码
//

//形变类型（ShapeChange_type=4）  + 一路ADC    +   一路ADC     电池电量      RSSI
//      1个字节                    2个字节        2个字节      2个字节      一个字节
//                                 Buffer[0]      Buffer[2]    Buffer[4]
//                                 Buffer[1]      Buffer[3]    Buffer[5]   Buffer[6]
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "stm32l4xx_hal.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "protocol.h"
#include "flash.h"
#include "adc.h"
#include "rtc.h"
#include "iwdg.h"
#include "rng.h"

uint64_t default_sensor_id=0;
uint64_t sensor_id = 0x1000001;//0xB5C8DD94
uint8_t otp_sensor_id_buf[6]= {0};
uint8_t sensor_id_buf[6]= {0x1D,0x9C,0x08,0x2A,0xFF,0xFC}; // {0xA9,0x2E,0x0D,0x00,0x00,0x01};
#define FLASH_OTP_ADDR ((uint32_t)0x1FFF7000)

/*宏定义*/
#define   start_up_time 16 //(调整值)值越小，启动越迟
#define ALARM_TEMPERATURE 0x0AF8B //75 degree
#define RX_TIMEOUT_TICK_SIZE	RADIO_TICK_SIZE_1000_US
#define MODE_LORA

/*32位*/
uint32_t BURST_Count_down=0;
volatile uint32_t MainRun_Time=0;     //偏移时间 ms单位
uint32_t sx1280_receive_time=0;//在倒计时处理地方进行赋值递减。
uint32_t RTC_SleepTime =0;

/*16位*/
__IO uint16_t uhADCxRefValue=0, uhADCxConvertedValue = 0;
float PA0_ADCxRValue=0, PA2_ADCxRValue = 0;
uint16_t Rssi=0;
uint16_t temperatrue=0;

/*8位*/
uint8_t Buffer[15]= {0x00};
uint8_t RspAckbuff[50]= {0x00};
uint8_t WriteBuffer[10],ReadBuffer[10];
uint8_t	Send_BURST_Count=3;//重复发送BURST次数定义
uint8_t Send_Message_Flag=0;
uint8_t Send_REQ_Flag=0;
uint8_t	Send_BURST_Flag=0;
uint8_t Send_Frame_Type=0;

/*句柄*/
SPI_HandleTypeDef hspi1;

/*其他*/
PacketParams_t packetParams;
PacketStatus_t packetStatus;
HAL_StatusTypeDef hi2c1_status;
int8_t x;
RadioStatus_t s;
float Shift_time=0.0;


/*函数*/
void STS31_GetData( void );
void Enter_Shutdown_Mode(void);
uint32_t Handle_RFSendflag(void);
extern uint16_t TxIrqMask;
extern uint32_t Flash_Sensor_ID;
int main( void )
{
    RTC_SleepTime = WakeUp_TimeBasis;
    HwInit( );
    Read_flash_Parameter();//读取参数
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    MX_IWDG_Init();
    SX1280_Init(Frequency_list[Frequency_point]);
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    Battery_Voltage_Measure();	//形变数据采集
    Handle_RFSendflag();
    HAL_IWDG_Refresh(&hiwdg);//喂狗
    Enter_Shutdown_Mode();
}


void Enter_Shutdown_Mode(void)
{
    WRITE_REG( RTC->BKP31R, 0x1 );//记录RTC寄存器，马上进入关机模式
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime=((RTC_SleepTime-MainRun_Time)*2.048-start_up_time);//5是估算程序启动时间 2是计算ms
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

uint32_t Handle_RFSendflag(void)
{
    uint8_t Return_Value=0;
    if( Send_Frame_Type == 1)
    {
        Send_Frame_Type=0; //方便理解
        SendtoStation_sx1280_frame(MESSAGE,19,ShapeChange_type,Buffer);
        HAL_Delay(35);//发送调整值
        Get_random();
    }
    else if( Send_Frame_Type == 2)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//切换控制频点
        Send_Frame_Type=0;
        AppState=0;
        sx1280_receive_time=65;//有时候接受超帧
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, ShapeChange_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState ==true)
            {
                SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                IrqState =false;
            }
            Return_Value=Process_Appstate_0();
            if(Return_Value==APP_RX)
            {
                Return_Value=Handle_receiveStation_sx1280_frame();
                if(Return_Value==RSP_END)
                {
                    Write_flash_Parameter();//将接受到的配置参数写入flash中
                    Shift_time=Offset_Delay%WakeUp_TimeBasis;
                    //串口调试使用
                    RTC_SleepTime=(Shift_time+RTC_SleepTime-23);
                    HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR1,((Offset_Delay%Message_cycle/WakeUp_TimeBasis)));//延时一个业务周期5分钟=30个10s周期
                    Read_flash_Parameter();//读取参数
                    RspAckbuff[0] = 0x01;
                    SendtoStation_sx1280_frame(ACK,1,RSP_END_ACK_type,RspAckbuff);
                    HAL_Delay(12);//发送调整值
                    break;//跳出超时
                }
            }
        }
    }
    else  if( Send_Frame_Type == 3)
    {
        Radio.SetRfFrequency(Frequency_list[20]);//切换控制频点
        Send_Frame_Type=0;
        AppState=0;
        sx1280_receive_time=100;//有时候接受超帧
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(RESET_REQ, 2, ShapeChange_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState ==true)
            {
                SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                IrqState =false;
            }
            Return_Value=Process_Appstate_0();
            if(Return_Value==APP_RX)
            {
                Return_Value=Handle_receiveStation_sx1280_frame();
                if(Return_Value==RESET_ACK)
                {
                    RspAckbuff[0] = 0x01;
                    SendtoStation_sx1280_frame(RESET_SUCCESS_ACK,1,RSP_END_ACK_type,RspAckbuff);
                    HAL_Delay(12);//发送调整值
                    break;//跳出超时
                }
            }
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}
