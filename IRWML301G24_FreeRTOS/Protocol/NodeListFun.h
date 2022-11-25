#ifndef __NODENET_PROTOCOL_H
#define __NODENET_PROTOCOL_H

#include "sys.h"
#include "stdio.h"
#include <stdbool.h>
//
#define  en_WATCHDOG  			true
#define  WATCHDOG_MaxTime   	120  		//20（约2分钟）次以内喂狗，重启时间在20*5s(延时阻塞时间)+16=116S; //200接近20分钟 //120:120*5s+16=616，即10分左右
#define  Register_FAILTIME_MAX 	120*60*1000	//注意对应变量的类型,2小时重新注册一下；
#define  SensorLIST_MAXNUM		700			//名单最大长度
#define  DnBeatHeartTime_MAXNUM	30*1000		//接收心跳间隔时间


#define	 FREQUENCY_LIST_LENTH 	26 			//频点最大长度
////////////组网协议中消息类型///////////////////////////////
typedef  enum
{
    Net_ReadBuffReq=1,
    Net_ReadRegReq,
    Net_WriteRegReq,
    Net_ReadAgentReq,
    Net_WriteAgentReq,
    Net_ReadRouteUnitReq,
    Net_WriteRouteUnitReq,
    Net_DelRouteUnitReq,
    Net_ClrRouteUnitReq,
    Net_ChannelStateInd,
    Net_AuthReq,
    Net_ReadBuffResp,
    Net_ReadRegResp,
    Net_WriteRegResp,
    Net_ReadAgentResp,
    Net_WriteAgentResp,
    Net_ReadRouteUnitResp,
    Net_WriteRouteUnitResp,
    Net_DelRouteUnitResp,
    Net_ClearRouteResp,
    Net_ChannelStateResp,
    Net_AuthResp,
    Net_GroupInd,
    Net_GroupDownInd,
    Net_RegReq,
    Net_AlarmInd,
    Net_ChannelReq,
    Net_GroupIndResp,
    Net_RegResp,
    Net_AlarmResp,
    Net_SystemInd
} NetWorking_InforType;

////////////微功率协议中帧类型///////////////////////////////
typedef  enum
{
    Sen_MESSAGE,
    Sen_REQ,
    Sen_RSP,
    Sen_RSP_END,
    Sen_BURST,
    Sen_ACK,
    Sen_RFU_1,
    Sen_RFU_2
} SensorNet_MType;

////////////微功率协议中应答的类型/////////////////////////////
typedef  enum
{
    Sen_RSP_END_ACK=1,
    Sen_BURST_ACK,
    Sen_RFU_ACK
} SensorNet_ACKType;

////////////微功率协议中控制信息参数类型///////////////////////////////
typedef  enum
{
    Sensor_MESSAGE_CYCLE,		//业务周期
    Sensor_CONTROL_CYCLE,		//控制周期
    Sensor_DELAYTIME,			//延迟时间
    Sensor_SHOCKTIME,			//最大随机退避时长
    Sensor_BS_CH_FRE,			//业务频点
	Sensor_PHYSICAL_LAYER,		//物理层数
	Sensor_WAITREQ_CYCLE,		//REQ帧等待回复周期
	Sensor_WAITBURST_CYCLE,		//BURST帧等待回复周期
	
	Sensor_Control_PARAMETER=0xFD, 	//控制参数（主要是指上下限）
	Sensor_SIX_PARAMETER=0xFE,	//（前5个+门限+备用）
    Sensor_FOUR_PARAMETER=0xFF	//（前4个）
} SensorNet_RSPENDType;

////////////微功率传感器黑白属性///////////////////////////////
typedef  enum
{
    SenAttri_Undistrib,		//属性未分配
	SenAttri_BL,			//黑名单属性
    SenAttri_WH				//白名单属性
} SensorID_Attri;

////////////微功率传感器注册状态///////////////////////////////
typedef  enum
{
    SenRegSt_UnReg,			//未注册
	SenRegSt_Fail,			//注册不通过
    SenRegSt_Success,		//注册通过
//	SenRegSt_RegSend		//正在注册
} SensorID_RegState;


////////////////////////////////////////////////////////////
//微功率传感器数据结构
////////////////////////////////////////////////////////////

struct SensorNode_Str
{
	uint16_t 	Nodenum;								//当前设备总数量
	uint16_t    BLNodenum;								//黑名单数量
	uint16_t    WHNodenum;								//百名单数量  ，余下还有未设置属性的
	uint16_t    RegSuccessNodenum;						//注册通过的设备数量
	uint16_t    RegFailNodenum;							//注册未通过的设备数量，余下还有未进行注册的设备
	
	uint8_t		Register_State[SensorLIST_MAXNUM];		//注册状态：0-未注册过；1-注册通过；2-注册未通过；>2-注册未通过次数
	uint8_t		Attr_BLorWL[SensorLIST_MAXNUM];			//黑白属性：0-未分配；1-白名单；2-黑名单；
	uint32_t	Register_FailTime[SensorLIST_MAXNUM];	//注册未通过次数，超过一定数量则需要重新注册；
	
	//以下六个参数是放在串口协议中代理里面
    uint8_t 	Sensor_ID[SensorLIST_MAXNUM][6]; 		//白名单ID列表，就是一个数组	
	uint32_t	BS_Cycle[SensorLIST_MAXNUM];			//业务周期，s为单位
	uint16_t	REQ_Cycle[SensorLIST_MAXNUM];			//控制周期，以业务周期为单位
	uint8_t 	ShockTime[SensorLIST_MAXNUM]; 			//振荡时间，以5ms为单位
//	uint32_t 	SenID_Slotorder[SensorLIST_MAXNUM];		//每个ID对应的延时时间，4字节，其实是时隙编号：最小值1，最大值Sensor_Business_Cycle/SENSOR_SLOT_TIME_LEN，其中Sensor_Business_Cycle为业务周期，SENSOR_SLOT_TIME_LEN为时隙时长（ms）;
	uint32_t 	DelayTime[SensorLIST_MAXNUM];		//每个ID对应的延时时间，4字节，其实是时隙编号：最小值1，最大值Sensor_Business_Cycle/SENSOR_SLOT_TIME_LEN，其中Sensor_Business_Cycle为业务周期，SENSOR_SLOT_TIME_LEN为时隙时长（ms）;
	uint16_t	SenID_ParaBkUp[SensorLIST_MAXNUM];		//备用参数，2个字节；
	
	//	uint16_t	WhID_SlotLen[WHITELIST_MAXNUM];		//该传感器的时隙长度（ms）
	uint8_t		BSCH_Fre[SensorLIST_MAXNUM];			//业务频点	
	int32_t 	SenID_SenLimit[SensorLIST_MAXNUM][4];	//门限值,14字节：[0]：是第一物理量的上限；[1]：是下限；[2]：是第二物理量的上限；[3]：是下限；
	uint16_t 	SenID_LimitType[SensorLIST_MAXNUM][2];	//限值类型：[0]：是指第一物理量的类型；[1]：第二物理量的类型；如果是0的话表示没有上下限
} ;

//传感器代理的数据结构，与白名单一致
#define AGENT_BYTENUM	19		//代理的字节数：暂时没有包括门限值
typedef struct
{	
    uint8_t 	SenID[6]; 							//ID，6字节
	uint32_t	SenBS_Cycle	;						//业务周期，ms为单位，4字节
	uint16_t	SenREQ_Cycle;						//控制周期，以业务周期为单位，2字	
	uint8_t 	SenShockTime;						//振荡时间，1字节，以5ms为单位
	uint32_t 	SenSlotorder;						//每个ID对应的延时时间，4字节，其实是时隙编号
	uint16_t	SenParaBkUp;						//备用参数，2字节；
	int32_t 	SenLimit[4];						//门限值,16字节；
	uint16_t    SenLimitType[2];					//限值类型：[0]：是指第一物理量的类型；[1]：第二物理量的类型；如果是0的话表示没有上下限	
} SensorAgent_Str;


//变量申明
//extern uint32_t const Frequency_list[FREQUENCY_LIST_LENTH];
extern uint32_t 	SelfNode_TimeStamp;  			//自身时间戳
extern uint32_t 	upHeartBeat_TimeStamp;  		//心跳时间戳
extern uint32_t 	downHeartBeat_TimeStamp;  		//下行心跳监测时间戳
extern uint8_t 	D_ID[6] ;  							//自身汇聚节点ID


extern uint8_t  	SenCTR_CH_Fre;					//微功率侧控制信道频点
extern uint8_t  	SenBS_CH1_Fre;					//微功率侧业务2信道频点
extern uint8_t  	SenBS_CH2_Fre;					//微功率侧业务2信道频点
extern uint8_t		Node_Fre;						//组网协议中的频点，该参数是为203模块使用的；

extern struct SensorNode_Str SensorNode_List;

//函数申明

extern unsigned short RTU_CRC( unsigned char * puchMsg,unsigned short  usDataLen );
extern void SensorNodeList_Init_test(void);
extern void SensorNodeList_Init(void);
extern void SenIDList_AttribReset(uint8_t SenAttri );
extern uint16_t FixPosition_InSenList(uint8_t *SD_ID);
extern uint16_t CheckID_isBL(uint8_t *SD_ID);
extern uint16_t CheckID_isWH(uint8_t *SD_ID);
extern uint8_t GetSenID_Attri(uint8_t *SD_ID);
extern uint8_t GetSenID_RegState(uint8_t *SD_ID);
extern bool ADDID_RegState(uint8_t *SD_ID,uint8_t regstate);
extern bool ADDID_Attrib(uint8_t *SD_ID,uint8_t attribe);
extern bool SenID_ModifyAttrib(uint8_t *SD_ID,uint8_t attribe);
extern bool SenID_DEL(uint8_t *BSD_ID);
extern void GetNodeNum_AllType(void);
extern void Printf_SenNodeList(uint8_t pflag);
#endif








