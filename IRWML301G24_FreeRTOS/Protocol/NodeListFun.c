//////////////////////////////////////////////////////////////////////////////////
//��������:2019/3/26
//�汾��V1.0
//Copyright(C) Inrich
//����������
//1���ڵ�����Э�飨������Э�飩���ú��������
//////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "stdio.h"
#include "malloc.h"
#include "sx1280_hw.h"
#include "sx1280_app.h"
#include "usart.h"
#include "NodeListFun.h"
#include "UartCom_Protocol.h"

extern bool En_debugIP1 ;

// -----------------------------------------------------------------------------
// DESCRIPTION: RTU CRCУ��ĸ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCHi[] = {
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40
} ;

// -----------------------------------------------------------------------------
// DESCRIPTION: RTU CRCУ��ĵ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCLo[] = {
    0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,0xCC,0x0C,0x0D,0xCD,
    0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,
    0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
    0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
    0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,
    0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
    0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,
    0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
    0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
    0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
    0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,
    0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40
};



///********Ƶ���б�************/
////΢���ʽ����������ŵ�Ƶ��Ĭ�ϱ�ţ�1
////΢���ʽ�����ҵ���ŵ�Ƶ��Ĭ�ϱ�ţ�25
//uint32_t const Frequency_list[FREQUENCY_LIST_LENTH]=
//{   2399500000,2400500000,2401500000,2402500000,2403500000,2404500000,
//    2405500000,2406500000,2407500000,2408500000,2409500000,
//    2410500000,2411500000,2412500000,2413500000,2414500000,
//    2415500000,2416500000,2417500000,2418500000,2419500000,
//    2420500000,2421500000,2422500000,2423500000,2424500000
//};

uint32_t 	SelfNode_TimeStamp=0;  			//����ʱ���
uint32_t 	upHeartBeat_TimeStamp=0;  		//��������ʱ���
uint32_t 	downHeartBeat_TimeStamp=0;  	//�����������ʱ���

uint8_t 	D_ID[6]= {0x01,0x02,0x03,0x00,0x05,0x06} ;  						//�����۽ڵ�ID

uint8_t  	SenCTR_CH_Fre=1;	//1;				//΢���ʲ�����ŵ�Ƶ��
uint8_t  	SenBS_CH1_Fre=25;  	//25				//΢���ʲ�ҵ��1�ŵ�Ƶ��
uint8_t  	SenBS_CH2_Fre=6;				//΢���ʲ�ҵ��2�ŵ�Ƶ��
uint8_t		Node_Fre=1;						//����Э���е�Ƶ�㣬�ò�����Ϊ203ģ��ʹ�õģ�

struct SensorNode_Str SensorNode_List; 	//����
/*******************************************************
*Function Name 	:RTU_CRC
*Description  	:CRCУ�麯��
*Input			:puchMsg������ָ�룻usDataLen �����鳤��
*Output			:
*******************************************************/
unsigned short RTU_CRC( unsigned char * puchMsg,unsigned short  usDataLen )
{
    unsigned char uchCRCHi;                // high byte of CRC initialized
    unsigned char uchCRCLo;                // low byte of CRC initialized
    unsigned uIndex;                       // will index into CRC lookup table

    uchCRCHi = 0xFF;
    uchCRCLo = 0xFF;

    while ( usDataLen-- )
    {
        // calculate the CRC
//      uIndex = uchCRCHi ^ (unsigned char)( *puchMsg++ );
//      uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
//      uchCRCLo = auchCRCLo[uIndex];
        uIndex = uchCRCLo ^ (unsigned char)( *puchMsg++ );
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
        uchCRCHi = auchCRCLo[uIndex];
    }
    return ( uchCRCHi << 8 | uchCRCLo );
}


/*******************************************************
*Function Name 	:SensorNodeList_Init_test
*Description  	:������ʼ�������ڲ���
*Input			:
*Output			:
*******************************************************/
void SensorNodeList_Init_test(void)
{
//    uint16_t bli;
//    SensorNode_List.Nodenum=1; //SensorLIST_MAXNUM;						//������������
//	SensorNode_List.WHNodenum=1;					//����������
//	uint8_t  para[6];
//    //��ID
//	bli=0;
//    {
//		SensorNode_List.Sensor_ID[bli][0]=0xA9;
//        SensorNode_List.Sensor_ID[bli][1]=0X00;
//        SensorNode_List.Sensor_ID[bli][2]=0;
//        SensorNode_List.Sensor_ID[bli][3]=0;
//        SensorNode_List.Sensor_ID[bli][4]=0;
//        SensorNode_List.Sensor_ID[bli][5]=0; 
//		SensorNode_List.Register_State[bli]		=SenRegSt_Success;		//��ע��״̬		
//        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_WH;			//��ڰ����� 
//		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;						//��ע��ʧ�ܵ��ۼ�ֵ
//		
//		para[0]=0;para[1]=0;para[2]=0x27;para[3]=0x10;	//ҵ������30s
//		memcpy(&SensorNode_List.BS_Cycle[bli],para,4);					//��ҵ������
//		para[0]=0;para[1]=5;  							//��������5		
//        memcpy(&SensorNode_List.REQ_Cycle[bli],para,2);				//���������
//        SensorNode_List.ShockTime[bli]			=2;					//��ʱ��,10ms(��5msΪ��λ)
//		para[0]=0;para[1]=0;para[2]=0x17;para[3]=0x70;		//��ʱ6s	
//        memcpy(&SensorNode_List.DelayTime[bli],para,4); 	//��ʱ 

//        SensorNode_List.BSCH_Fre[bli]			=0;				//Ƶ��
//		
//		SensorNode_List.SenID_LimitType[bli][0]	=0;				//��������
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//������
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][0],para,4);		//����
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//������
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][1],para,4);		//����
//		
//		SensorNode_List.SenID_LimitType[bli][1]	=0;				//��������
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//������
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][2],para,4);		//����
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//������
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][3],para,4);		//����
//		
//        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//����     
//    }
	
	uint16_t bli;
    SensorNode_List.Nodenum=SensorLIST_MAXNUM; //;						//������������
	SensorNode_List.WHNodenum=0;					//����������
//	uint8_t  para[6];
	for(bli=0; bli<SensorLIST_MAXNUM; bli++)
    {
        SensorNode_List.Sensor_ID[bli][0]=bli;
        SensorNode_List.Sensor_ID[bli][1]=(bli>>8);
        SensorNode_List.Sensor_ID[bli][2]=0;
        SensorNode_List.Sensor_ID[bli][3]=0;
        SensorNode_List.Sensor_ID[bli][4]=0;
        SensorNode_List.Sensor_ID[bli][5]=0;
		SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//��ע��״̬		
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//��ڰ�����
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	//��ע��ʧ�ܵ��ۼ�ֵ
		
		SensorNode_List.BS_Cycle[bli]			=0;						//��ҵ������
        SensorNode_List.REQ_Cycle[bli]			=0;						//���������
        SensorNode_List.ShockTime[bli]			=0;						//��ʱ��
        SensorNode_List.DelayTime[bli]			=0;						//��ʱ

        SensorNode_List.BSCH_Fre[bli]			=0;				//Ƶ��
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//����
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//����
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//��������
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//��������
    }
}

/*******************************************************
*Function Name 	:SensorNodeList_Init
*Description  	:������ʼ���������Ҫ�洢����,����������Ҫ��flash�ж�ȡ��ŵ�·�ɱ�
*Input			:
*Output			:
*******************************************************/
void SensorNodeList_Init(void)
{
    uint16_t bli;
    SensorNode_List.Nodenum			=0;					//������������
    SensorNode_List.BLNodenum		=0;					//����������
    SensorNode_List.WHNodenum		=0;					//����������
    SensorNode_List.RegSuccessNodenum=0;				//ע��ͨ����������
    SensorNode_List.RegFailNodenum=0;					//ע��δͨ��������
    for(bli=0; bli<SensorLIST_MAXNUM; bli++)
    {
        memset(SensorNode_List.Sensor_ID[bli],0,6);						//��ID
        SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//��ע��״̬
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//��ڰ�����
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	//��ע��ʧ�ܵ��ۼ�ֵ
		
        SensorNode_List.BS_Cycle[bli]			=0;						//��ҵ������
        SensorNode_List.REQ_Cycle[bli]			=0;						//���������
        SensorNode_List.ShockTime[bli]			=0;						//��ʱ��
        SensorNode_List.DelayTime[bli]			=0;						//��ʱ

        SensorNode_List.BSCH_Fre[bli]			=0;				//Ƶ��
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//����
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//����
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//��������
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//��������
    }
}

/*******************************************************
*Function Name 	:SenIDList_Reset
*Description  	:�����������ã��������кڰ��������Ը�Ϊδ����
*Input			:SenAttri==SenAttri_BL����Ѻ��������Ը�Ϊδ���䣻SenAttri==SenAttri_WH����Ѱ��������Ը�Ϊδ����
*Output			:
*******************************************************/
void SenIDList_AttribReset(uint8_t SenAttri )
{
    uint16_t bli;
    if(SensorNode_List.Nodenum==0)
    {
        //printf("the blacklist'num is zero in BlackList_CheckID!\r\n");
        return ;
    }
    if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//���������������ֵ
    {
//        SensorNode_List.Nodenum=SensorLIST_MAXNUM;
//        RegList[BASICREGLIST_LENGTH-1]=SensorLIST_MAXNUM;	//ͬʱ��ֵ��Ӧ������Ϣ�Ĵ���
        SensorNodeList_Init();
        return;
    }
    for(bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri)  //���Ϊ�������ԣ����Ϊδ����
            SensorNode_List.Attr_BLorWL[bli]=SenAttri_Undistrib;
    }
}

/*******************************************************
*Function Name 	:FixPosition_InSenList
*Description  	:�����������Ƿ������ID,����ID�������е����
*Input			:
*Output			:0xFFFF:��ʾû�У�����ֵ�����ʾ��ID�������е����к�
*******************************************************/
uint16_t FixPosition_InSenList(uint8_t *SD_ID)
{
    uint16_t bli;
    if(SensorNode_List.Nodenum==0)
    {
        //printf("the blacklist'num is zero in BlackList_CheckID!\r\n");
        return 0xffff;
    }
    if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//���������������ֵ
    {
        //printf("the blacklist'num is exceed MAXNUM in BlackList_CheckID!\r\n");
//        SensorNode_List.Nodenum=SensorLIST_MAXNUM;
//        RegList[BASICREGLIST_LENGTH-1]=SensorLIST_MAXNUM;	//ͬʱ��ֵ��Ӧ������Ϣ�Ĵ���
        SensorNodeList_Init();
        return 0xffff;
    }
    for(bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        if(memcmp(SensorNode_List.Sensor_ID[bli],SD_ID,6)==0)  //�Ƚ�ID��ͬ��
            return bli;
    }
//    printf("there is no this ID in the whitelist whiteList_CheckID() !\r\n");
    return 0xffff;
}


/*******************************************************
*Function Name 	:GetNodeNum_AllType
*Description  	:ȡ�ø��ഫ�����ĸ������������ڽṹ���Ӧ�ı�����
*Input			:
*Output			:
*******************************************************/
void GetNodeNum_AllType(void)
{
    uint16_t bli;
    SensorNode_List.BLNodenum		=0;					//����������
    SensorNode_List.WHNodenum		=0;					//����������
    SensorNode_List.RegSuccessNodenum=0;				//ע��ͨ����������
    SensorNode_List.RegFailNodenum=0;					//ע��δͨ��������
	if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//���������������ֵ
    {
        SensorNodeList_Init();
        return ;
    }
    for(bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_BL)
            SensorNode_List.BLNodenum++;
        else if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_WH)
            SensorNode_List.WHNodenum++;

        if(SensorNode_List.Register_State[bli]==SenRegSt_Success)
            SensorNode_List.RegSuccessNodenum++;
        else if(SensorNode_List.Register_State[bli]==SenRegSt_Fail)
            SensorNode_List.RegFailNodenum++;
    }
}

/*******************************************************
*Function Name 	:CheckID_isBL
*Description  	:���ID�����ں�����ô
*Input			:
*Output			:0xffff:��ʾ���ǻ���û�����ID������ֵ:��ʾ���ں��������Ҹ�ֵ���б��е����;
*******************************************************/
uint16_t CheckID_isBL(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_BL)//�������Ϊ������
            return bli;
    }
    return 0xffff;
}
/*******************************************************
*Function Name 	:CheckID_isWH
*Description  	:���ID�����ڰ�����ô
*Input			:
*Output			:0xffff:��ʾ���ǻ���û�����ID������ֵ:��ʾ����Ϊ���������Ҹ�ֵ���б��е����;
*******************************************************/
uint16_t CheckID_isWH(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_WH)//�������Ϊ������
            return bli;
    }
    return 0xffff;
}

/*******************************************************
*Function Name 	:GetSenID_Attri
*Description  	:ȡ�����ID�ĺڰ�����ֵ
*Input			:
*Output			:0xFF:��ʾ�б���û�����ID������ֵ�����ʾ��ID�ĺڰ�����ֵ
*******************************************************/
uint8_t GetSenID_Attri(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        return SensorNode_List.Attr_BLorWL[bli];
    }
    return 0xff;
}

/*******************************************************
*Function Name 	:GetSenID_RegState
*Description  	:ȡ�����ID��ע��״̬
*Input			:
*Output			:0xFF:��ʾ�б���û�����ID������ֵ�����ʾ��ID��ע��״̬
*******************************************************/
uint8_t GetSenID_RegState(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        return SensorNode_List.Register_State[bli];
    }
    return 0xff;
}

/*******************************************************
*Function Name 	:ADDID_RegState
*Description  	:��������,ͬʱ�޸�ע��״̬
*Input			:
*Output			:true:��ʾ�ɹ�;false:��ʾû��
*******************************************************/
bool ADDID_RegState(uint8_t *SD_ID,uint8_t regstate)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        SensorNode_List.Register_State[bli]=regstate; //ֱ���޸�״̬
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	  //��ע��ʧ�ܵ��ۼ�ֵ		
        return true;
    }
    //���������������ڱ�β
    if(SensorNode_List.Nodenum>(SensorLIST_MAXNUM-1))		//���������������ֵ
    {
        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in ADDID_RegState()!");
        return false;
    }
    memcpy(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],SD_ID,6);			//�Ѹ�ID���뵽���б��ĩβ
    SensorNode_List.Register_State[SensorNode_List.Nodenum]=regstate; 			//ֱ���޸�״̬	
	SensorNode_List.Register_FailTime[SensorNode_List.Nodenum]	=SelfNode_TimeStamp;				//��ע��ʧ�ܵ��ۼ�ֵ
    SensorNode_List.Nodenum++;													//����������1
    return true;
}

/*******************************************************
*Function Name 	:ADDID_Attrib
*Description  	:��������,ͬʱ�޸���������
*Input			:
*Output			:true:��ʾ�ɹ�;false:��ʾû��
*******************************************************/
bool ADDID_Attrib(uint8_t *SD_ID,uint8_t attribe)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        SensorNode_List.Attr_BLorWL[bli]=attribe; //ֱ���޸�����
		if(attribe==SenAttri_BL)				
			SensorNode_List.Register_State[bli]=SenRegSt_UnReg;	//����Ǻ�������ע���ʶ����
        return true;
    }
    //���������������ڱ�β
    if(SensorNode_List.Nodenum>(SensorLIST_MAXNUM-1))		//���������������ֵ
    {
        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in ADDID_Attrib()!");
        return false;
    }
    memcpy(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],SD_ID,6);			//�Ѹ�ID���뵽���б��ĩβ
    SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum]=attribe; 				//ֱ���޸�����
	if(attribe==SenAttri_BL)        
		SensorNode_List.Register_State[SensorNode_List.Nodenum]=SenRegSt_UnReg;	//����Ǻ�������ע���ʶ����
    SensorNode_List.Nodenum++;													//����������1
    return true;
}


/*******************************************************
*Function Name 	:SenID_ModifyAttrib
*Description  	:�޸���������
*Input			:
*Output			:true:��ʾ�ɹ�;false:��ʾû��
*******************************************************/
bool SenID_ModifyAttrib(uint8_t *SD_ID,uint8_t attribe)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//�ж��Ƿ����б���
    if(bli!=0xffff)
    {
        SensorNode_List.Attr_BLorWL[bli]=attribe; //ֱ���޸�����
        return true;
    }
    return false;
}


/*******************************************************
*Function Name 	:SenID_DEL
*Description  	:���б���ɾ��ID��ɾ�������ǰ��б�β��ID�Ƶ���ǰҪ��ɾ��ID����λ��,��������ʡȴ�б���λ�Ĳ����ˣ�
*Input			:
*Output			:true:��ʾ�ɹ�;false:��ʾû��
*******************************************************/
bool SenID_DEL(uint8_t *BSD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(BSD_ID);	//�ж��Ƿ����б���
    if(bli==0xffff) 	//�����б���
    {
        return false;
    }
    if((bli+1)==SensorNode_List.Nodenum)  //���������һ��
    {
        memset(SensorNode_List.Sensor_ID[bli],0,6);						//��ID
        SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//��ע��״̬
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//��ڰ�����		
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	  					//��ע��ʧ�ܵ��ۼ�ֵ
		
        SensorNode_List.BS_Cycle[bli]			=0;						//��ҵ������
        SensorNode_List.REQ_Cycle[bli]			=0;						//���������
        SensorNode_List.ShockTime[bli]			=0;				//��ʱ��
        SensorNode_List.DelayTime[bli]			=0;				//ʱ϶���

        SensorNode_List.BSCH_Fre[bli]			=0;				//Ƶ��
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//����
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//����
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//����
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//��������
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//��������
        SensorNode_List.Nodenum--;				//����������1
        if(En_debugIP1) printf("\r\n DEL list succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",BSD_ID[0],BSD_ID[1],BSD_ID[2],BSD_ID[3],BSD_ID[4],BSD_ID[5]);
        return true;
    }
    //�������б������һ��ID�����ǰҪ��ɾ����ID�������Ϳ���ʡȴ�ƶ��б�Ĳ����ˣ�
    SensorNode_List.Nodenum--;				//����������1
    memcpy(SensorNode_List.Sensor_ID[bli],SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],6);	//�滻ID
    SensorNode_List.Register_State[bli]	=SensorNode_List.Register_State[SensorNode_List.Nodenum];	//ע��״̬
    SensorNode_List.Attr_BLorWL[bli]	=SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum];		//�ڰ�����
	SensorNode_List.Register_FailTime[bli]=SensorNode_List.Register_FailTime[SensorNode_List.Nodenum];//ע��ʧ�ܵ��ۼ�ֵ
    SensorNode_List.BS_Cycle[bli]=SensorNode_List.BS_Cycle[SensorNode_List.Nodenum];				//ҵ������
    SensorNode_List.REQ_Cycle[bli]=SensorNode_List.REQ_Cycle[SensorNode_List.Nodenum];				//��������
    SensorNode_List.ShockTime[bli]=SensorNode_List.ShockTime[SensorNode_List.Nodenum];				//��ʱ��
    SensorNode_List.DelayTime[bli]=SensorNode_List.DelayTime[SensorNode_List.Nodenum];				//ʱ϶���

    SensorNode_List.BSCH_Fre[bli]=SensorNode_List.BSCH_Fre[SensorNode_List.Nodenum];						//Ƶ��
    SensorNode_List.SenID_SenLimit[bli][0]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][0];	//����
    SensorNode_List.SenID_SenLimit[bli][1]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][1];	//����
    SensorNode_List.SenID_SenLimit[bli][2]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][2];	//����
    SensorNode_List.SenID_SenLimit[bli][3]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][3];	//����
    SensorNode_List.SenID_ParaBkUp[bli]	 	=SensorNode_List.SenID_ParaBkUp[SensorNode_List.Nodenum];		//����
    SensorNode_List.SenID_LimitType[bli][0]	=SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][0];	//��������
    SensorNode_List.SenID_LimitType[bli][1]	=SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][1];	//��������

    //���б������һ���豸����������
    memset(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],0,6);									//ID
    SensorNode_List.Register_State[SensorNode_List.Nodenum]		=SenRegSt_UnReg;		//ע��״̬
    SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum]		=SenAttri_Undistrib;	//�ڰ�����
	SensorNode_List.Register_FailTime[SensorNode_List.Nodenum]	=SelfNode_TimeStamp;	//��ע��ʧ�ܵ��ۼ�ֵ					
	
    SensorNode_List.BS_Cycle[SensorNode_List.Nodenum]			=0;				//ҵ������
    SensorNode_List.REQ_Cycle[SensorNode_List.Nodenum]			=0;				//��������
    SensorNode_List.ShockTime[SensorNode_List.Nodenum]			=0;				//��ʱ��
    SensorNode_List.DelayTime[SensorNode_List.Nodenum]	=0;				//ʱ϶���

    SensorNode_List.BSCH_Fre[SensorNode_List.Nodenum]			=0;				//Ƶ��
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][0]	=0;				//����
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][1]	=0;				//����
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][2]	=0;				//����
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][3]	=0;				//����
    SensorNode_List.SenID_ParaBkUp[SensorNode_List.Nodenum]		=0;				//����
    SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][0]	=0;				//��������
    SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][1]	=0;				//��������

    if(En_debugIP1) printf("\r\n DEL list succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",BSD_ID[0],BSD_ID[1],BSD_ID[2],BSD_ID[3],BSD_ID[4],BSD_ID[5]);
    return true;
}


/*******************************************************
*Function Name 	:SenID_DEL
*Description  	:��ӡ�豸�б�
*Input			:pflag:0-�����б�1-��������2-��������3-ע��ɹ���4-ע��δͨ����5-ע��ɹ��İ�������6-ע��ʧ�ܵĺ�������
*Output			:
*******************************************************/
void Printf_SenNodeList(uint8_t pflag)
{
    uint16_t bli,blm;
    GetNodeNum_AllType();
    printf("\r\nNodenum=%d,",SensorNode_List.Nodenum);
    printf("BLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
	switch(pflag)
	{
		case 1:
			printf("\r\n---ALL-SenNodeList(%05d)-------",SensorNode_List.Nodenum);
			printf("\r\n   SN        ID      N/B/W  N/F/S  ");
			break;	
		case 2:
			printf("\r\n---Black-SenNodeList(%05d)-----",SensorNode_List.BLNodenum);
			printf("\r\n   SN        ID      N/B/W  N/F/S  ");
			break;	
		case 3:
			printf("\r\n---White-SenNodeList(%05d)-----",SensorNode_List.WHNodenum);
			printf("\r\n   SN        ID      N/B/W  N/F/S  ");
			break;	
		case 4:
			printf("\r\n-RegSuccess-SenNodeList(%05d)--",SensorNode_List.RegSuccessNodenum);
			printf("\r\n   SN        ID      N/B/W  N/F/S  ");
			break;		
		case 5:
			printf("\r\n---RegFail-SenNodeList(%05d)---",SensorNode_List.RegFailNodenum);
			printf("\r\n   SN        ID      N/B/W  N/F/S  ");
			break;	
		default:
			printf("\r\n-----Error CMD Byte -----------");
			break;
	}	
	
	blm=0;
    for (bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        switch(pflag)
        {
        case 1:	//all list				
            printf("\r\n % 5d  ",(bli+1));
            for (uint8_t j=0; j<6; j++) printf("%02X",SensorNode_List.Sensor_ID[bli][j]);
            printf("   %1d      %1d ",SensorNode_List.Attr_BLorWL[bli],SensorNode_List.Register_State[bli]);
            break;
        case 2:	//only blacklist
			if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_BL)
			{
				blm++;
				printf("\r\n %05d  ",blm);
				for (uint8_t j=0; j<6; j++) printf("%02X",SensorNode_List.Sensor_ID[bli][j]);
				printf("   %1d      %1d ",SensorNode_List.Attr_BLorWL[bli],SensorNode_List.Register_State[bli]);            
			}
            break;
		case 3:	//only whitelist
			if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_WH)
			{
				blm++;
				printf("\r\n %05d  ",blm);
				for (uint8_t j=0; j<6; j++) printf("%02X",SensorNode_List.Sensor_ID[bli][j]);
				printf("   %1d      %1d ",SensorNode_List.Attr_BLorWL[bli],SensorNode_List.Register_State[bli]);            
			}
			break;
		case 4:	//only regsuccess nodelist
			if(SensorNode_List.Register_State[bli]==SenRegSt_Success)
			{
				blm++;
				printf("\r\n %05d  ",blm);
				for (uint8_t j=0; j<6; j++) printf("%02X",SensorNode_List.Sensor_ID[bli][j]);
				printf("   %1d      %1d ",SensorNode_List.Attr_BLorWL[bli],SensorNode_List.Register_State[bli]);            
			}
			break;
		case 5:	//only regfail nodelist
			if(SensorNode_List.Register_State[bli]==SenRegSt_Fail)
			{
				blm++;
				printf("\r\n %05d  ",blm);
				for (uint8_t j=0; j<6; j++) printf("%02X",SensorNode_List.Sensor_ID[bli][j]);
				printf("   %1d      %1d ",SensorNode_List.Attr_BLorWL[bli],SensorNode_List.Register_State[bli]);            
			}
            break;
        }
    }
}



///*******************************************************
//*Function Name	:WhiteList_ADDID
//*Description  	:������������ID���Ȳ��ȷ���Ƿ����ڸ��£������ټ����β
//*Input			:SensorAgent_Str*SenPt������ָ��
//				:addflag:0-�����в�����1-�޸Ĳ�����2-���ֲ�������
//*Output			:true:��ʾ�ɹ�;false:��ʾû��
//*******************************************************/
//uint16_t WhiteList_ADDID(SensorAgent_Str *SenPt,uint8_t addflag)
//{
//    uint16_t bli;
//    uint16_t tempi;
//    if(WhiteSensorNode_List.Nodenum>WHITELIST_MAXNUM)		//���������������ֵ
//    {
//        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM in WhiteList_ADDID()!");
//        return 0xffff;
//    }
//    //�Ȳ��ȷ�ϸ�ID�Ƿ��Ѿ����ڣ����������²���
//    for(bli=0; bli<WhiteSensorNode_List.Nodenum; bli++)
//    {
//        //if(WhiteSensorNode_List.WhiteIDList[bli]==SenPt->SenID)	//ID�Ƿ����
//        if((WhiteSensorNode_List.WhiteIDList[bli][0]==SenPt->SenID[0])&&(WhiteSensorNode_List.WhiteIDList[bli][1]==SenPt->SenID[1])&&(WhiteSensorNode_List.WhiteIDList[bli][2]==SenPt->SenID[2])&&	//ID�Ƿ����
//                (WhiteSensorNode_List.WhiteIDList[bli][3]==SenPt->SenID[3])&&(WhiteSensorNode_List.WhiteIDList[bli][4]==SenPt->SenID[4])&&(WhiteSensorNode_List.WhiteIDList[bli][5]==SenPt->SenID[5]))	//ID�Ƿ����
//        {
//            switch(addflag)
//            {
//            case 0:	//�����
//                WhiteSensorNode_List.BS_Cycle[bli]=0;
//                WhiteSensorNode_List.REQ_Cycle[bli]=0;
//                WhiteSensorNode_List.WhID_Slotorder[bli]=0;
//                WhiteSensorNode_List.ShockTime[bli]=0;
//                WhiteSensorNode_List.WhID_ParaBkUp[bli]=0;					//���ò���
//                WhiteSensorNode_List.WhID_SenLimit[bli][0]=0;				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][1]=0;				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][2]=0;				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][3]=0;				//����
//                WhiteSensorNode_List.WhID_LimitType[bli][0]=0;			//����
//                WhiteSensorNode_List.WhID_LimitType[bli][1]=0;			//����
//                break;
//            case 1: //�޸Ĳ���
//                WhiteSensorNode_List.BS_Cycle[bli]=SenPt->SenBS_Cycle;
//                WhiteSensorNode_List.REQ_Cycle[bli]=SenPt->SenREQ_Cycle;
//                WhiteSensorNode_List.WhID_Slotorder[bli]=SenPt->SenSlotorder;
//                WhiteSensorNode_List.ShockTime[bli]=SenPt->SenShockTime;
//                WhiteSensorNode_List.WhID_ParaBkUp[bli]=SenPt->SenParaBkUp;					//���ò���
//                WhiteSensorNode_List.WhID_SenLimit[bli][0]=SenPt->SenLimit[0];				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][1]=SenPt->SenLimit[1];				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][2]=SenPt->SenLimit[2];				//����
//                WhiteSensorNode_List.WhID_SenLimit[bli][3]=SenPt->SenLimit[3];				//����
//                WhiteSensorNode_List.WhID_LimitType[bli][0]=SenPt->SenLimitType[0];			//����
//                WhiteSensorNode_List.WhID_LimitType[bli][1]=SenPt->SenLimitType[1];			//����
//                break;
//            default:
//                break;
//            }
//            return bli;
//        }
//    }
//    //������������ID�ӵ�������
//    if(WhiteSensorNode_List.Nodenum>(WHITELIST_MAXNUM-1))		//���������������ֵ
//    {
//        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in WhiteList_ADDID()!");
//        return 0xffff;
//    }
//    for(tempi=0; tempi<6; tempi++)
//        WhiteSensorNode_List.WhiteIDList[bli][tempi]=SenPt->SenID[tempi];
//    switch(addflag)
//    {
//    case 0:	//�����
//        WhiteSensorNode_List.BS_Cycle[bli]=0;
//        WhiteSensorNode_List.REQ_Cycle[bli]=0;
//        WhiteSensorNode_List.WhID_Slotorder[bli]=0;
//        WhiteSensorNode_List.ShockTime[bli]=0;
//        WhiteSensorNode_List.WhID_ParaBkUp[bli]=0;					//���ò���
//        WhiteSensorNode_List.WhID_SenLimit[bli][0]=0;				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][1]=0;				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][2]=0;				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][3]=0;				//����
//        WhiteSensorNode_List.WhID_LimitType[bli][0]=0;			//����
//        WhiteSensorNode_List.WhID_LimitType[bli][1]=0;			//����
//        break;
//    case 1: //�޸Ĳ���
//        WhiteSensorNode_List.BS_Cycle[bli]=SenPt->SenBS_Cycle;
//        WhiteSensorNode_List.REQ_Cycle[bli]=SenPt->SenREQ_Cycle;
//        WhiteSensorNode_List.WhID_Slotorder[bli]=SenPt->SenSlotorder;
//        WhiteSensorNode_List.ShockTime[bli]=SenPt->SenShockTime;
//        WhiteSensorNode_List.WhID_ParaBkUp[bli]=SenPt->SenParaBkUp;					//���ò���

//        WhiteSensorNode_List.WhID_SenLimit[bli][0]=SenPt->SenLimit[0];				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][1]=SenPt->SenLimit[1];				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][2]=SenPt->SenLimit[2];				//����
//        WhiteSensorNode_List.WhID_SenLimit[bli][3]=SenPt->SenLimit[3];				//����
//        WhiteSensorNode_List.WhID_LimitType[bli][0]=SenPt->SenLimitType[0];						//����
//        WhiteSensorNode_List.WhID_LimitType[bli][1]=SenPt->SenLimitType[1];						//����
//        break;
//    }
//    WhiteSensorNode_List.Nodenum++;												//����������1
//    RegList[BASICREGLIST_LENGTH-2]=WhiteSensorNode_List.Nodenum;				//ͬʱ���Ӧ������Ϣ�Ĵ���
//    return bli;
//}

///*******************************************************
//*Function Name 	:WhiteList_DELID
//*Description  	:��������ɾ��ID��ɾ�������ǰ��б�β��ID�Ƶ���ǰҪ��ɾ��ID����λ�ã���������ʡȴ�б���λ�Ĳ����ˣ�
//*Input			:
//*Output			:true:��ʾ�ɹ�;false:��ʾû��
//*******************************************************/
//bool WhiteList_DELID(uint8_t *SD_ID)
//{
//    uint16_t bli;
//    uint16_t tempi;
//    if(WhiteSensorNode_List.Nodenum==0) 	//���б�
//    {
//        if(En_debugIP1) printf("\r\nthe blacklist is empty BlackList_DELID()!");
//        return false;
//    }

//    if(WhiteSensorNode_List.Nodenum==1) 	//������ֻ��һ��ID
//    {
//        if((WhiteSensorNode_List.WhiteIDList[0][0]==SD_ID[0])&&(WhiteSensorNode_List.WhiteIDList[0][1]==SD_ID[1])&&(WhiteSensorNode_List.WhiteIDList[0][2]==SD_ID[2])&&	//ID�Ƿ����
//                (WhiteSensorNode_List.WhiteIDList[0][3]==SD_ID[3])&&(WhiteSensorNode_List.WhiteIDList[0][4]==SD_ID[4])&&(WhiteSensorNode_List.WhiteIDList[0][5]==SD_ID[5]))	//ID�Ƿ����
//        {
//            WhiteSensorNode_List.Nodenum=0;					//������������
//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[0][tempi]=0;			//ID����
//            RegList[BASICREGLIST_LENGTH-2]=0;			//ͬʱ���Ӧ������Ϣ�Ĵ���
//            if(En_debugIP1) printf("\r\n DEL first whilelist succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//            return true;
//        }
//        else
//        {
//            if(En_debugIP1) printf("\r\n DEL whilelist fail,there is no thisID in whitelist:%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//            return false;
//        }
//    }
//    for(bli=0; bli<WhiteSensorNode_List.Nodenum; bli++)
//    {
//        //if(WhiteSensorNode_List.WhiteIDList[bli]==SD_ID)	//ID�Ƿ����
//        if((WhiteSensorNode_List.WhiteIDList[bli][0]==SD_ID[0])&&(WhiteSensorNode_List.WhiteIDList[bli][1]==SD_ID[1])&&(WhiteSensorNode_List.WhiteIDList[bli][2]==SD_ID[2])&&	//ID�Ƿ����
//                (WhiteSensorNode_List.WhiteIDList[bli][3]==SD_ID[3])&&(WhiteSensorNode_List.WhiteIDList[bli][4]==SD_ID[4])&&(WhiteSensorNode_List.WhiteIDList[bli][5]==SD_ID[5]))	//ID�Ƿ����
//        {
//            WhiteSensorNode_List.Nodenum--;				//����������1
//            RegList[BASICREGLIST_LENGTH-2]=WhiteSensorNode_List.Nodenum;			//ͬʱ���Ӧ������Ϣ�Ĵ���
//            //�������б������һ��ID�����ǰҪ��ɾ����ID�������Ϳ���ʡȴ�ƶ��б�Ĳ����ˣ�
//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[bli][tempi]=WhiteSensorNode_List.WhiteIDList[WhiteSensorNode_List.Nodenum][tempi];			//�ƶ�ID
//            WhiteSensorNode_List.BS_Cycle[bli]=WhiteSensorNode_List.BS_Cycle[WhiteSensorNode_List.Nodenum];					//ҵ������
//            WhiteSensorNode_List.REQ_Cycle[bli]=WhiteSensorNode_List.REQ_Cycle[WhiteSensorNode_List.Nodenum];				//��������
//            WhiteSensorNode_List.ShockTime[bli]=WhiteSensorNode_List.ShockTime[WhiteSensorNode_List.Nodenum];				//��ʱ��
//            WhiteSensorNode_List.WhID_Slotorder[bli]=WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum];		//ʱ϶���

//            WhiteSensorNode_List.BSCH_Fre[bli]=WhiteSensorNode_List.BSCH_Fre[WhiteSensorNode_List.Nodenum];					//Ƶ��
//            WhiteSensorNode_List.WhID_SenLimit[bli][0]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][0];		//����
//            WhiteSensorNode_List.WhID_SenLimit[bli][1]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][1];		//����
//            WhiteSensorNode_List.WhID_SenLimit[bli][2]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][2];		//����
//            WhiteSensorNode_List.WhID_SenLimit[bli][3]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][3];		//����
//            WhiteSensorNode_List.WhID_ParaBkUp[bli]=WhiteSensorNode_List.WhID_ParaBkUp[WhiteSensorNode_List.Nodenum];			//����
//            WhiteSensorNode_List.WhID_LimitType[bli][0]=WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][0];			//��������
//            WhiteSensorNode_List.WhID_LimitType[bli][1]=WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][1];			//��������


////			for( tempi=0;tempi<WhID_Para_MAXNUM;tempi++)
////				WhiteSensorNode_List.WhID_Para[bli][tempi]=WhiteSensorNode_List.WhID_Para[WhiteSensorNode_List.Nodenum][tempi];				//�ƶ�����

//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[WhiteSensorNode_List.Nodenum][tempi]=0;			//ԭ�б�β��ID����
//            WhiteSensorNode_List.BS_Cycle[WhiteSensorNode_List.Nodenum]=0;				//ҵ����������
//            WhiteSensorNode_List.REQ_Cycle[WhiteSensorNode_List.Nodenum]=0;				//������������
//            WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum]=0;		//��ʱʱ������
//            WhiteSensorNode_List.ShockTime[WhiteSensorNode_List.Nodenum]=0;				//ԭ�б�β����ʱ��
//            WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum]=0;		//ԭ�б�β��ʱ϶�������

//            WhiteSensorNode_List.BSCH_Fre[WhiteSensorNode_List.Nodenum]=0;
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][0]=0;		//ԭ�б�β����������
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][1]=0;		//ԭ�б�β����������
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][2]=0;		//ԭ�б�β����������
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][3]=0;		//ԭ�б�β����������
//            WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][0]=0;		//��������
//            WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][1]=0;		//��������
//            if(En_debugIP1) printf("\r\n DEL whilelist succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//            return true;
//        }
//    }
//    if(En_debugIP1) printf("\r\n DEL whilelist fail,there is no thisID in whitelist:%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//    return false;
//}


