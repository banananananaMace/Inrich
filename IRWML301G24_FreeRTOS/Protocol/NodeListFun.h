#ifndef __NODENET_PROTOCOL_H
#define __NODENET_PROTOCOL_H

#include "sys.h"
#include "stdio.h"
#include <stdbool.h>
//
#define  en_WATCHDOG  			true
#define  WATCHDOG_MaxTime   	120  		//20��Լ2���ӣ�������ι��������ʱ����20*5s(��ʱ����ʱ��)+16=116S; //200�ӽ�20���� //120:120*5s+16=616����10������
#define  Register_FAILTIME_MAX 	120*60*1000	//ע���Ӧ����������,2Сʱ����ע��һ�£�
#define  SensorLIST_MAXNUM		700			//������󳤶�
#define  DnBeatHeartTime_MAXNUM	30*1000		//�����������ʱ��


#define	 FREQUENCY_LIST_LENTH 	26 			//Ƶ����󳤶�
////////////����Э������Ϣ����///////////////////////////////
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

////////////΢����Э����֡����///////////////////////////////
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

////////////΢����Э����Ӧ�������/////////////////////////////
typedef  enum
{
    Sen_RSP_END_ACK=1,
    Sen_BURST_ACK,
    Sen_RFU_ACK
} SensorNet_ACKType;

////////////΢����Э���п�����Ϣ��������///////////////////////////////
typedef  enum
{
    Sensor_MESSAGE_CYCLE,		//ҵ������
    Sensor_CONTROL_CYCLE,		//��������
    Sensor_DELAYTIME,			//�ӳ�ʱ��
    Sensor_SHOCKTIME,			//�������˱�ʱ��
    Sensor_BS_CH_FRE,			//ҵ��Ƶ��
	Sensor_PHYSICAL_LAYER,		//�������
	Sensor_WAITREQ_CYCLE,		//REQ֡�ȴ��ظ�����
	Sensor_WAITBURST_CYCLE,		//BURST֡�ȴ��ظ�����
	
	Sensor_Control_PARAMETER=0xFD, 	//���Ʋ�������Ҫ��ָ�����ޣ�
	Sensor_SIX_PARAMETER=0xFE,	//��ǰ5��+����+���ã�
    Sensor_FOUR_PARAMETER=0xFF	//��ǰ4����
} SensorNet_RSPENDType;

////////////΢���ʴ������ڰ�����///////////////////////////////
typedef  enum
{
    SenAttri_Undistrib,		//����δ����
	SenAttri_BL,			//����������
    SenAttri_WH				//����������
} SensorID_Attri;

////////////΢���ʴ�����ע��״̬///////////////////////////////
typedef  enum
{
    SenRegSt_UnReg,			//δע��
	SenRegSt_Fail,			//ע�᲻ͨ��
    SenRegSt_Success,		//ע��ͨ��
//	SenRegSt_RegSend		//����ע��
} SensorID_RegState;


////////////////////////////////////////////////////////////
//΢���ʴ��������ݽṹ
////////////////////////////////////////////////////////////

struct SensorNode_Str
{
	uint16_t 	Nodenum;								//��ǰ�豸������
	uint16_t    BLNodenum;								//����������
	uint16_t    WHNodenum;								//����������  �����»���δ�������Ե�
	uint16_t    RegSuccessNodenum;						//ע��ͨ�����豸����
	uint16_t    RegFailNodenum;							//ע��δͨ�����豸���������»���δ����ע����豸
	
	uint8_t		Register_State[SensorLIST_MAXNUM];		//ע��״̬��0-δע�����1-ע��ͨ����2-ע��δͨ����>2-ע��δͨ������
	uint8_t		Attr_BLorWL[SensorLIST_MAXNUM];			//�ڰ����ԣ�0-δ���䣻1-��������2-��������
	uint32_t	Register_FailTime[SensorLIST_MAXNUM];	//ע��δͨ������������һ����������Ҫ����ע�᣻
	
	//�������������Ƿ��ڴ���Э���д�������
    uint8_t 	Sensor_ID[SensorLIST_MAXNUM][6]; 		//������ID�б�����һ������	
	uint32_t	BS_Cycle[SensorLIST_MAXNUM];			//ҵ�����ڣ�sΪ��λ
	uint16_t	REQ_Cycle[SensorLIST_MAXNUM];			//�������ڣ���ҵ������Ϊ��λ
	uint8_t 	ShockTime[SensorLIST_MAXNUM]; 			//��ʱ�䣬��5msΪ��λ
//	uint32_t 	SenID_Slotorder[SensorLIST_MAXNUM];		//ÿ��ID��Ӧ����ʱʱ�䣬4�ֽڣ���ʵ��ʱ϶��ţ���Сֵ1�����ֵSensor_Business_Cycle/SENSOR_SLOT_TIME_LEN������Sensor_Business_CycleΪҵ�����ڣ�SENSOR_SLOT_TIME_LENΪʱ϶ʱ����ms��;
	uint32_t 	DelayTime[SensorLIST_MAXNUM];		//ÿ��ID��Ӧ����ʱʱ�䣬4�ֽڣ���ʵ��ʱ϶��ţ���Сֵ1�����ֵSensor_Business_Cycle/SENSOR_SLOT_TIME_LEN������Sensor_Business_CycleΪҵ�����ڣ�SENSOR_SLOT_TIME_LENΪʱ϶ʱ����ms��;
	uint16_t	SenID_ParaBkUp[SensorLIST_MAXNUM];		//���ò�����2���ֽڣ�
	
	//	uint16_t	WhID_SlotLen[WHITELIST_MAXNUM];		//�ô�������ʱ϶���ȣ�ms��
	uint8_t		BSCH_Fre[SensorLIST_MAXNUM];			//ҵ��Ƶ��	
	int32_t 	SenID_SenLimit[SensorLIST_MAXNUM][4];	//����ֵ,14�ֽڣ�[0]���ǵ�һ�����������ޣ�[1]�������ޣ�[2]���ǵڶ������������ޣ�[3]�������ޣ�
	uint16_t 	SenID_LimitType[SensorLIST_MAXNUM][2];	//��ֵ���ͣ�[0]����ָ��һ�����������ͣ�[1]���ڶ������������ͣ������0�Ļ���ʾû��������
} ;

//��������������ݽṹ���������һ��
#define AGENT_BYTENUM	19		//������ֽ�������ʱû�а�������ֵ
typedef struct
{	
    uint8_t 	SenID[6]; 							//ID��6�ֽ�
	uint32_t	SenBS_Cycle	;						//ҵ�����ڣ�msΪ��λ��4�ֽ�
	uint16_t	SenREQ_Cycle;						//�������ڣ���ҵ������Ϊ��λ��2��	
	uint8_t 	SenShockTime;						//��ʱ�䣬1�ֽڣ���5msΪ��λ
	uint32_t 	SenSlotorder;						//ÿ��ID��Ӧ����ʱʱ�䣬4�ֽڣ���ʵ��ʱ϶���
	uint16_t	SenParaBkUp;						//���ò�����2�ֽڣ�
	int32_t 	SenLimit[4];						//����ֵ,16�ֽڣ�
	uint16_t    SenLimitType[2];					//��ֵ���ͣ�[0]����ָ��һ�����������ͣ�[1]���ڶ������������ͣ������0�Ļ���ʾû��������	
} SensorAgent_Str;


//��������
//extern uint32_t const Frequency_list[FREQUENCY_LIST_LENTH];
extern uint32_t 	SelfNode_TimeStamp;  			//����ʱ���
extern uint32_t 	upHeartBeat_TimeStamp;  		//����ʱ���
extern uint32_t 	downHeartBeat_TimeStamp;  		//�����������ʱ���
extern uint8_t 	D_ID[6] ;  							//�����۽ڵ�ID


extern uint8_t  	SenCTR_CH_Fre;					//΢���ʲ�����ŵ�Ƶ��
extern uint8_t  	SenBS_CH1_Fre;					//΢���ʲ�ҵ��2�ŵ�Ƶ��
extern uint8_t  	SenBS_CH2_Fre;					//΢���ʲ�ҵ��2�ŵ�Ƶ��
extern uint8_t		Node_Fre;						//����Э���е�Ƶ�㣬�ò�����Ϊ203ģ��ʹ�õģ�

extern struct SensorNode_Str SensorNode_List;

//��������

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








