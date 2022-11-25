#include "string.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "protocol.h"
#include "flash.h"
#include "hw.h"
#include "adc.h"
#include "rtc.h"
#include "iwdg.h"
#include "rng.h"

/*----------------------宏定义----------------------*/
#define	start_up_time 15 //(调整值)值越小，启动越迟
#define MODE_LORA
#define RX_TIMEOUT_TICK_SIZE	RADIO_TICK_SIZE_1000_US
#define FLASH_OTP_ADDR ((uint32_t)0x1FFF7000)

/*----------------------32位----------------------*/
uint64_t sensor_id =0;
uint32_t WakeUpNumber = 0;
uint32_t Message_Cycle_Count_down = 0;
uint32_t Flash_Sensor_ID=0;
volatile uint32_t MainRun_Time = 0;   //偏移时间 ms单位
uint32_t sx1280_receive_time = 0; //在倒计时处理地方进行赋值递减。
uint32_t RTC_SleepTime = 0;
/*----------------------8位----------------------*/
uint8_t Buffer[10] = {0x00};//存放要发送的数据
uint8_t RspAckbuff[10] = {0x00};
uint8_t WriteBuffer[10], ReadBuffer[10];
uint8_t Send_BURST_Count = 3; //重复发送BURST次数定义
uint8_t Send_BURST_Flag = 0;
uint8_t Burst_Status = 0;
uint8_t Send_Frame_Type = 0;
uint8_t sensor_id_buf[6]= {0xA9,0x2E,0x08,0x2F,0xC0,0x00};
uint8_t otp_sensor_id_buf[6]= {0};

/*----------------------其他----------------------*/
PacketParams_t packetParams;
PacketStatus_t packetStatus;
HAL_StatusTypeDef hi2c1_status;
float Shift_time = 0.0;
float temperatrue = 0.0;//温度

/*----------------------函数----------------------*/
void STS31_GetData( void );
uint32_t Handle_RFSendflag(void);
void Enter_ShutDown_Mode(void);
extern uint16_t TxIrqMask ;
uint64_t default_sensor_id=0;
int main( void )
{
    RTC_SleepTime = WakeUp_TimeBasis;
    HwInit( );
    MX_IWDG_Init();
    Read_flash_Parameter();//读取参数、
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    SX1280_Init(Frequency_list[Frequency_point]);
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    Battery_Voltage_Measure();
    STS31_GetData();//温度
    HAL_IWDG_Refresh(&hiwdg) ;	//看门狗喂狗
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    Handle_RFSendflag();				//发送数据标志位
    Enter_ShutDown_Mode();
}

void STS31_GetData( void )
{
    WriteBuffer[0] = 0x2C;
    WriteBuffer[1] = 0x10;
    hi2c1_status = HAL_I2C_Master_Transmit(&hi2c1, ADDR_SHS31_Wirte, WriteBuffer, 2, 0xFFFF);
    hi2c1_status = HAL_I2C_Master_Receive(&hi2c1, ADDR_SHS31_Read, ReadBuffer, 3, 0xFFFF);
    if((ReadBuffer[0] == 0xFF) && (ReadBuffer[1] == 0xFF)) //防止刚上电时冲击电流造成传感器读取数据错误而发送告警帧//0xFFFF对应温度为129°
    {
        ReadBuffer[0] = 0;
        ReadBuffer[1] = 0;
    }
    temperatrue = (ReadBuffer[0] << 8) + ReadBuffer[1];
    temperatrue = ((175.0 * temperatrue / 65536) - 45);
    if ((temperatrue > Alarm_threshold)||(temperatrue < Alarm_threshold_DOWN))
    {
        Burst_Status = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR20); //查询告警帧发送的状态
        if(Burst_Status == 1) //之前状态是告警帧,不发送告警帧
        {
            Send_BURST_Flag = 0;
        }
        else
        {
            Send_BURST_Flag = 1;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 1); //设备告警状态标志位
        }
        if(Send_Frame_Type == 1)//在发送Message时如果产生告警，则不发送message
        {
            Send_BURST_Flag = 1;
            Send_Frame_Type = 0;
        }
        if(Send_Frame_Type == 2)//在发REQ如果产生告警，发REQ和BURST
        {
            Send_BURST_Flag = 1;
        }
    }
    else
    {
        HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 0);
    }
    memcpy(Buffer,&temperatrue,4);
}

void Enter_ShutDown_Mode(void)
{
    WRITE_REG( RTC->BKP31R, 0x1 );//记录RTC寄存器，马上进入关机模式
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime = ((RTC_SleepTime - MainRun_Time) * 2.048 - start_up_time);
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

uint32_t Handle_RFSendflag(void)
{
    uint8_t Return_Value = 0;
    if(Send_BURST_Flag == 1)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//控制频点
        Send_BURST_Flag = 0;
        AppState = 0;
        Send_BURST_Count = 3;
        while(Send_BURST_Count)
        {
            Send_BURST_Count--;
            Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
            SendtoStation_sx1280_frame(BURST, 15, Temperature_type, Buffer);
            sx1280_receive_time = 50;
            while(sx1280_receive_time)
            {
                if( IrqState == true)
                {
                    SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                    IrqState = false;
                }
                Return_Value = Process_Appstate_0();
                if(Return_Value == APP_RX)
                {
                    Return_Value = Handle_receiveStation_sx1280_frame();
                    if(Return_Value == ACK) //接受到应答帧确认
                    {
                        break; //跳出超时循环
                    }
                }
            }//倒计时 超时
            if(Return_Value == ACK) //接受到应答帧确认
            {
                break; //跳出重复次数循环
            }
        }//Send_BURST_Count 重复发送次数
    }
    if(Send_Frame_Type == 1)//发送message
    {
        Send_Frame_Type = 0;
        SendtoStation_sx1280_frame(MESSAGE, 15, Temperature_type, Buffer);
        HAL_Delay(40);//发送调整值
        Get_random();//随机数进行时间偏移
    }
    else if(Send_Frame_Type == 2)//发送REQ
    {
        Radio.SetRfFrequency(Frequency_list[1]);//切换控制频点
        Send_Frame_Type = 0;
        AppState = 0;
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, Temperature_type, Buffer);
        sx1280_receive_time = 100;
        while(sx1280_receive_time)
        {
            if( IrqState == true)
            {
                SX1280ProcessIrqs();//扫描各类状态，例如发送和接收，则置AppState为对应变量，然后通过下面代码处理数据
                IrqState = false;
            }
            Return_Value = Process_Appstate_0();
            if(Return_Value == APP_RX) //接收到RSP
            {
                Return_Value = Handle_receiveStation_sx1280_frame(); //分析汇聚节点回复的帧类型；
                if(Return_Value == RSP_END) //处理函数
                {
                    Write_flash_Parameter();//将接受到的配置参数写入flash中
                    Shift_time = Offset_Delay % WakeUp_TimeBasis; //偏移时间小数部分
                    RTC_SleepTime = (Shift_time + RTC_SleepTime - 23); //测试时间偏移23ms
                    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, ((Offset_Delay % Message_cycle / WakeUp_TimeBasis))); //延时一个业务周期5分钟=30个10s周期
                    Read_flash_Parameter();//读取参数
                    RspAckbuff[0] = 1;
 
                    SendtoStation_sx1280_frame(ACK, 1, RSP_END_ACK_type, RspAckbuff);
                    HAL_Delay(20);//发送调整值
                    break;//跳出超时
                }
            }//RSP_END
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}



