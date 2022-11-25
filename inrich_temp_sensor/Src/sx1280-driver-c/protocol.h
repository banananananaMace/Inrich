#include "main.h"
#include "stdio.h"
#include "stdint.h"


extern uint8_t Receive_BURST_ACK_flag;
extern uint8_t RecBuffer[];
extern uint8_t sensor_id_buf[6];
extern float Alarm_threshold_DOWN;
uint8_t Handle_receiveStation_sx1280_frame(void);
uint8_t SendtoStation_sx1280_frame(uint8_t send_MHDR_type, uint8_t send_Length,uint8_t Data_Type, uint8_t *data);

typedef  enum
{
    Message_cycle_type=0x00,
    Ctrl_cycle_type=0x01,
    Delay_type=0x02,
    Time_random_type=0x03,
    Frequency_point_type=0x04,
    REQ_Waiting_Reply_Cycle,
    BURST_Waiting_Reply_Cycle,
    Alarm_threshold_type=0x14,
    ALL_RSP_END_need_type=0xFF,
} RSP_END_Data_Type;

typedef  enum
{
    All_Ctrl_Para=0,	//所有控制参数	(超帧测试)
    Signal_Instruct=1,	//通信指令参数	(Message周期及其他)
    Sensor_Ctrl_Para=2,	//传感器控制参数	(告警帧)
} REQ_Info_Type;

typedef  enum
{
    MESSAGE=0,
    REQ=1,
    RSP=2,
    RSP_END=3,
    BURST=4,
    ACK=5,
    RFU_1=6,
    RFU_2
} MType;


typedef  enum
{
//    //MESSAGE    MACPayload
//    Humiture_type=1,      //温湿度
////    Temperature_type=1,  //温度电池
    Temperature_type=1,  //温度电池
//    WaterEntry_type=4,   //水浸
//    ShapeChange_type=5,   //形变
//    Angle_type=6,          //倾角
//    Shake_type=7,           //震动

} Data_Type;


typedef  enum
{
    BURST_Temperature_alarm_type,
    BURST_Humiture_alarm_type,

} BURST_Data_Type;

typedef  enum
{
    //ACK    MACPayload
    RSP_END_ACK_type=1,//RSP_END帧确认帧
    BURST_ACK_type,//BURST帧确认帧
    RFU_ACK_type,//保留备用帧的确认帧
} ACK_Data_Type;



struct MHDR
{
    uint8_t MType;
    uint8_t Length;
    uint8_t Sensor_ID[6];
    uint8_t Key_Value;
    uint8_t Data_type;
};

struct MESSAGE_MACPayload
{
    uint8_t Data_Type;
    uint8_t  data[30];
};




extern uint32_t   Message_cycle,

       Offset_Delay;

extern uint16_t    Ctrl_cycle;
extern uint8_t     Time_random;
extern uint8_t		 Frequency_point,
           REQ_Waiting_Reply,
           BURST_Waiting_Reply;
extern float		Alarm_threshold;


extern uint32_t const Frequency_list[];


