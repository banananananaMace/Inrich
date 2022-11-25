#ifndef __UARTCOM_NETPROTOCOL_H
#define __UARTCOM_NETPROTOCOL_H

#include "sys.h"
#include "stdio.h"
#include <stdbool.h>

#define MaxSinkCnt  5
#define MaxRouteCnt 5
#define MaxUschCnt 10
struct SinkDevice
{
    uint8_t  EID[6];
    uint16_t Addr;
    uint8_t  BW;
};
struct RouteDevice
{
    uint8_t  SEID[6];
    uint8_t  MEID[6];
};
struct Channel
{
    uint16_t Addr;
    uint8_t  HeadSlot;
    uint8_t  SlotLength;
};

////////////΢���ʴ�����ע��״̬///////////////////////////////
typedef  enum
{
    RelayClient_OFF,	//δͬ������
    RelayClient_ON		//ע��ͨ��,��������
} RelayClient_RegState;


struct SetPara4
{
//    uint8_t  emptyflag           ;  // �ձ�־
//    uint32_t SerialNumber        ;
    uint8_t  RelayClient_status  ;  // 0-off;1-on;
//    uint8_t  onoff               ;  // ���п��أ�0-stop��1-run
    uint8_t  lock_config         ;  // ��������0-���ɽ��룻1-�����ò�������
    uint8_t  D_DID[6]            ;  // deviceID
    uint16_t desMID              ;  // Ŀ��master ID���ϲ�����Master��ַ��
    uint8_t  desARFCN            ;	// Ŀ��Ƶ���

    // uint16_t forbidden_List[MaxForbiddenQTY][2]; // 0-ARFCN, 1-MID
    // uint16_t forbidden_List_cnt;

    // ��Ϊrelay�ڵ��master״̬����ʱ��Ҫ�Ĳ���
    uint16_t addr               ;  // comm address, get from network master������������ڵ��ͨ�ŵ�ַ
    uint8_t  NID                 ;
    uint8_t  version             ;
    uint16_t HeadFN             ;  // number of the first frame of this relay node
    uint16_t BP                 ;   //�㲥����
    uint8_t  Deep               ;    //����

    uint8_t  DataLayerAbnormalCnt  ;

    uint8_t  GPD              ;//���б���ʱ��
    uint8_t  GPU              ;//���б���ʱ��
    uint8_t  GPDU             ;//�л�����ʱ��
    uint8_t  GPUD             ;//֡�䱣��ʱ��
    uint16_t QTY_Slot       ;//frame�е�slot����
    uint16_t QTY_Chip       ;//Slot�е�chip������һ��chip 1ms
    uint16_t QTY_Frame      ;//super frame�е�frame����
    uint16_t QTY_slotDL     ;//downlink subframe�е�slot����
    uint16_t QTY_slotUL     ;//uplink subframe�е�slot����


//    uint8_t  BCH_ARFCN_QTY;  // RF search QTY
//    uint8_t  BCH_ARFCN_List[10];  // RF search List

    uint8_t  BCH_RSSI;
	bool     En_debug;
    float    HW;
    float    FW;
//    uint8_t  SinkCnt;
//    struct   SinkDevice Sink[MaxSinkCnt];
//    uint8_t  RouteCnt;
//    struct   RouteDevice Route[MaxRouteCnt];
//    uint8_t  UschCnt;
//    struct   Channel UschChannel[MaxRouteCnt];

//    uint8_t  MaxBWforSingleDevice;
//    uint8_t  MaxActiveSlots;
//    uint8_t  DefaultBWforSink;
    uint16_t Max_USCH_Silent_Time;

    uint8_t  Channel0_ARFCN;
    uint8_t  Channel0_Profile;
    uint16_t Channel0_HeadFN;
    uint16_t Channel0_BP;
    uint8_t  Channel1_ARFCN;
    uint8_t  Channel1_Profile;
    uint16_t Channel1_HeadFN;
    uint16_t Channel1_BP;
    uint16_t tempFN;
    uint8_t  iChannel;

    uint16_t  ConfigDlTimeout;
    uint8_t   ConfigDlTimeoutMaxCnt;

    uint8_t   TxPower;
};

//���������
#define Net_UL_TopologyReport         0x01
#define Net_UL_DeviceReg              0x02
#define Net_UL_ChannelStatusReport    0x03
#define Net_UL_DeviceStatusReport     0x04
#define Net_UL_ACK                    0x05
#define Net_UL_SensorParaReport       0x06
#define Net_DL_SinkRouterAssign       0x41
#define Net_DL_SensorRouteAssign      0x42
#define Net_DL_DeviceRegAck           0x43
#define Net_DL_ChannelStatusQuery     0x44
#define Net_DL_ChannelConfig          0x45
#define Net_DL_DeviceStatusQuery      0x46
#define Net_DL_ACK                    0x47
#define Net_DL_SensorComParaConfig    0x48
#define Net_DL_SensorComParaQuery     0x49
#define Net_DL_SensorBWListConfig     0x81
#define Net_DL_SensorSyncCollect      0x82

#define Net_DL_SinkControl			  0xC0
#define Net_DL_ChannelParaQuery		  0xC1
#define Net_DL_ChannelParaConfig	  0xC2
#define Net_DL_DeviceParaConfig	  	  0xC3
#define Net_DL_DeviceParaQuery	  	  0xC4
#define Net_DL_DeviceListQuery	  	  0xC5
#define Net_DL_RouteListQuery	  	  0xC6

#define Net_UL_ChannelParaQueryAck	  0xE0
#define Net_UL_DeviceParaQueryAck	  0xE1
#define Net_UL_DeviceListQueryAck	  0xE2
#define Net_UL_RouteListQueryAck	  0xE3

#define Net_UL_BeatHeartCMD			  0x56
#define Net_DL_BeatHeartACK			  0x57

//#define DeviceFlag_None               0x00
//#define DeviceFlag_unReg              0x01
//#define DeviceFlag_RegReq             0x02
//#define DeviceFlag_Reg                0xF0
//#define DeviceFlag_Topology           0xF1
//#define DeviceFlag_AddWithoutMacReg   0xF2
//#define DeviceFlag_Black              0xFF

//�豸����
#define DeviceType_Sink 1
#define DeviceType_UP   0
#define DeviceType_LP   2

#define ProcType_Reset     0
#define ProcType_Add       1
#define ProcType_Del       2
#define ProcType_Reserved  3


extern uint8_t const	CMD_Head[5];
extern struct SetPara4  Config;

extern void Config_init(void);
extern bool NetProCom_Rx_Pro(uint8_t *Redata_pt,uint8_t comnum);
extern bool proc_DSCH_pdu(uint8_t *DSCH_pdu,uint16_t DSCH_pdulength, uint8_t comnum);
extern bool procNorthSignaling(uint8_t *NorthRxDMABuff,uint8_t comnum);
extern uint8_t procNetCmdSensorRoute(uint8_t *DSCH_pdu, uint8_t comnum);
extern void procNetCmdRegAck(uint8_t *DSCH_pdu, uint8_t comnum); 
extern void uPSensorRegReqResp_Pro(uint8_t *EID,uint8_t result);
extern uint8_t procNetCmdComParaConfig(uint8_t *DSCH_pdu, uint8_t comnum); 
extern void procNetCmdQueryComPara(uint8_t *DSCH_pdu, uint8_t comnum);
extern uint8_t procNetCmdBWList(uint8_t *DSCH_pdu, uint8_t comnum) ;
extern uint8_t procNetCmd_C0_SinkControl(uint8_t *DSCH_pdu, uint8_t comnum);
extern void procNetCmd_C1_ChannelParaQuery(uint8_t *DSCH_pdu, uint8_t comnum);
extern void procNetCmd_C4_DeviceParaQuery(uint8_t *DSCH_pdu, uint8_t comnum);
extern void procNetCmd_C5_DeviceListQuery(uint8_t *DSCH_pdu, uint8_t comnum);
extern void Load_NetCmd_UL_ACK(uint8_t NetCmd, uint8_t ackresult);
//extern void Add_Protocol_Head(uint8_t *pt_buf);
extern void Load_HeartBeat(void);
#endif
