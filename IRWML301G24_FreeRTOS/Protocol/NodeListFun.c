//////////////////////////////////////////////////////////////////////////////////
//创建日期:2019/3/26
//版本：V1.0
//Copyright(C) Inrich
//功能描述：
//1、节点网络协议（即组网协议）所用函数与变量
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
// DESCRIPTION: RTU CRC校验的高位字节表
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
// DESCRIPTION: RTU CRC校验的低位字节表
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



///********频点列表************/
////微功率接入网控制信道频点默认编号：1
////微功率接入网业务信道频点默认编号：25
//uint32_t const Frequency_list[FREQUENCY_LIST_LENTH]=
//{   2399500000,2400500000,2401500000,2402500000,2403500000,2404500000,
//    2405500000,2406500000,2407500000,2408500000,2409500000,
//    2410500000,2411500000,2412500000,2413500000,2414500000,
//    2415500000,2416500000,2417500000,2418500000,2419500000,
//    2420500000,2421500000,2422500000,2423500000,2424500000
//};

uint32_t 	SelfNode_TimeStamp=0;  			//自身时间戳
uint32_t 	upHeartBeat_TimeStamp=0;  		//上行心跳时间戳
uint32_t 	downHeartBeat_TimeStamp=0;  	//下行心跳监测时间戳

uint8_t 	D_ID[6]= {0x01,0x02,0x03,0x00,0x05,0x06} ;  						//自身汇聚节点ID

uint8_t  	SenCTR_CH_Fre=1;	//1;				//微功率侧控制信道频点
uint8_t  	SenBS_CH1_Fre=25;  	//25				//微功率侧业务1信道频点
uint8_t  	SenBS_CH2_Fre=6;				//微功率侧业务2信道频点
uint8_t		Node_Fre=1;						//组网协议中的频点，该参数是为203模块使用的；

struct SensorNode_Str SensorNode_List; 	//名单
/*******************************************************
*Function Name 	:RTU_CRC
*Description  	:CRC校验函数
*Input			:puchMsg：数组指针；usDataLen ：数组长度
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
*Description  	:名单初始化：用于测试
*Input			:
*Output			:
*******************************************************/
void SensorNodeList_Init_test(void)
{
//    uint16_t bli;
//    SensorNode_List.Nodenum=1; //SensorLIST_MAXNUM;						//名单数量清零
//	SensorNode_List.WHNodenum=1;					//白名单数量
//	uint8_t  para[6];
//    //清ID
//	bli=0;
//    {
//		SensorNode_List.Sensor_ID[bli][0]=0xA9;
//        SensorNode_List.Sensor_ID[bli][1]=0X00;
//        SensorNode_List.Sensor_ID[bli][2]=0;
//        SensorNode_List.Sensor_ID[bli][3]=0;
//        SensorNode_List.Sensor_ID[bli][4]=0;
//        SensorNode_List.Sensor_ID[bli][5]=0; 
//		SensorNode_List.Register_State[bli]		=SenRegSt_Success;		//清注册状态		
//        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_WH;			//清黑白属性 
//		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;						//清注册失败的累计值
//		
//		para[0]=0;para[1]=0;para[2]=0x27;para[3]=0x10;	//业务周期30s
//		memcpy(&SensorNode_List.BS_Cycle[bli],para,4);					//清业务周期
//		para[0]=0;para[1]=5;  							//控制周期5		
//        memcpy(&SensorNode_List.REQ_Cycle[bli],para,2);				//清控制周期
//        SensorNode_List.ShockTime[bli]			=2;					//振荡时间,10ms(以5ms为单位)
//		para[0]=0;para[1]=0;para[2]=0x17;para[3]=0x70;		//延时6s	
//        memcpy(&SensorNode_List.DelayTime[bli],para,4); 	//延时 

//        SensorNode_List.BSCH_Fre[bli]			=0;				//频点
//		
//		SensorNode_List.SenID_LimitType[bli][0]	=0;				//门限类型
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//上门限
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][0],para,4);		//门限
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//下门限
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][1],para,4);		//门限
//		
//		SensorNode_List.SenID_LimitType[bli][1]	=0;				//门限类型
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//上门限
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][2],para,4);		//门限
//		para[0]=0;para[1]=0;para[2]=0x0;para[3]=0x0;			//下门限
//        memcpy(&SensorNode_List.SenID_SenLimit[bli][3],para,4);		//门限
//		
//        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//备用     
//    }
	
	uint16_t bli;
    SensorNode_List.Nodenum=SensorLIST_MAXNUM; //;						//名单数量清零
	SensorNode_List.WHNodenum=0;					//白名单数量
//	uint8_t  para[6];
	for(bli=0; bli<SensorLIST_MAXNUM; bli++)
    {
        SensorNode_List.Sensor_ID[bli][0]=bli;
        SensorNode_List.Sensor_ID[bli][1]=(bli>>8);
        SensorNode_List.Sensor_ID[bli][2]=0;
        SensorNode_List.Sensor_ID[bli][3]=0;
        SensorNode_List.Sensor_ID[bli][4]=0;
        SensorNode_List.Sensor_ID[bli][5]=0;
		SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//清注册状态		
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//清黑白属性
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	//清注册失败的累计值
		
		SensorNode_List.BS_Cycle[bli]			=0;						//清业务周期
        SensorNode_List.REQ_Cycle[bli]			=0;						//清控制周期
        SensorNode_List.ShockTime[bli]			=0;						//振荡时间
        SensorNode_List.DelayTime[bli]			=0;						//延时

        SensorNode_List.BSCH_Fre[bli]			=0;				//频点
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//门限
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//备用
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//门限类型
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//门限类型
    }
}

/*******************************************************
*Function Name 	:SensorNodeList_Init
*Description  	:名单初始化：如果需要存储功能,本函数还需要从flash中读取存放的路由表
*Input			:
*Output			:
*******************************************************/
void SensorNodeList_Init(void)
{
    uint16_t bli;
    SensorNode_List.Nodenum			=0;					//名单数量清零
    SensorNode_List.BLNodenum		=0;					//黑名单数量
    SensorNode_List.WHNodenum		=0;					//白名单数量
    SensorNode_List.RegSuccessNodenum=0;				//注册通过名单数量
    SensorNode_List.RegFailNodenum=0;					//注册未通过单数量
    for(bli=0; bli<SensorLIST_MAXNUM; bli++)
    {
        memset(SensorNode_List.Sensor_ID[bli],0,6);						//清ID
        SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//清注册状态
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//清黑白属性
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	//清注册失败的累计值
		
        SensorNode_List.BS_Cycle[bli]			=0;						//清业务周期
        SensorNode_List.REQ_Cycle[bli]			=0;						//清控制周期
        SensorNode_List.ShockTime[bli]			=0;						//振荡时间
        SensorNode_List.DelayTime[bli]			=0;						//延时

        SensorNode_List.BSCH_Fre[bli]			=0;				//频点
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//门限
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//备用
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//门限类型
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//门限类型
    }
}

/*******************************************************
*Function Name 	:SenIDList_Reset
*Description  	:名单属性重置：即把所有黑白名单属性改为未分配
*Input			:SenAttri==SenAttri_BL，则把黑名单属性改为未分配；SenAttri==SenAttri_WH，则把白名单属性改为未分配
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
    if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//名单数量超过最大值
    {
//        SensorNode_List.Nodenum=SensorLIST_MAXNUM;
//        RegList[BASICREGLIST_LENGTH-1]=SensorLIST_MAXNUM;	//同时赋值对应基本信息寄存器
        SensorNodeList_Init();
        return;
    }
    for(bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri)  //如果为名单属性，则改为未分配
            SensorNode_List.Attr_BLorWL[bli]=SenAttri_Undistrib;
    }
}

/*******************************************************
*Function Name 	:FixPosition_InSenList
*Description  	:查找名单中是否有这个ID,返回ID在数组中的序号
*Input			:
*Output			:0xFFFF:表示没有；其他值，则表示该ID在数组中的序列号
*******************************************************/
uint16_t FixPosition_InSenList(uint8_t *SD_ID)
{
    uint16_t bli;
    if(SensorNode_List.Nodenum==0)
    {
        //printf("the blacklist'num is zero in BlackList_CheckID!\r\n");
        return 0xffff;
    }
    if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//名单数量超过最大值
    {
        //printf("the blacklist'num is exceed MAXNUM in BlackList_CheckID!\r\n");
//        SensorNode_List.Nodenum=SensorLIST_MAXNUM;
//        RegList[BASICREGLIST_LENGTH-1]=SensorLIST_MAXNUM;	//同时赋值对应基本信息寄存器
        SensorNodeList_Init();
        return 0xffff;
    }
    for(bli=0; bli<SensorNode_List.Nodenum; bli++)
    {
        if(memcmp(SensorNode_List.Sensor_ID[bli],SD_ID,6)==0)  //比较ID相同则
            return bli;
    }
//    printf("there is no this ID in the whitelist whiteList_CheckID() !\r\n");
    return 0xffff;
}


/*******************************************************
*Function Name 	:GetNodeNum_AllType
*Description  	:取得各类传感器的个数，结果存放在结构体对应的变量中
*Input			:
*Output			:
*******************************************************/
void GetNodeNum_AllType(void)
{
    uint16_t bli;
    SensorNode_List.BLNodenum		=0;					//黑名单数量
    SensorNode_List.WHNodenum		=0;					//白名单数量
    SensorNode_List.RegSuccessNodenum=0;				//注册通过名单数量
    SensorNode_List.RegFailNodenum=0;					//注册未通过单数量
	if(SensorNode_List.Nodenum>SensorLIST_MAXNUM)		//名单数量超过最大值
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
*Description  	:这个ID是属于黑名单么
*Input			:
*Output			:0xffff:表示不是或者没有这个ID；其他值:表示属于黑名单，且该值是列表中的序号;
*******************************************************/
uint16_t CheckID_isBL(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_BL)//如果属性为黑名单
            return bli;
    }
    return 0xffff;
}
/*******************************************************
*Function Name 	:CheckID_isWH
*Description  	:这个ID是属于白名单么
*Input			:
*Output			:0xffff:表示不是或者没有这个ID；其他值:表示属性为白名单，且该值是列表中的序号;
*******************************************************/
uint16_t CheckID_isWH(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        if(SensorNode_List.Attr_BLorWL[bli]==SenAttri_WH)//如果属性为白名单
            return bli;
    }
    return 0xffff;
}

/*******************************************************
*Function Name 	:GetSenID_Attri
*Description  	:取得这个ID的黑白属性值
*Input			:
*Output			:0xFF:表示列表中没有这个ID；其他值，则表示该ID的黑白属性值
*******************************************************/
uint8_t GetSenID_Attri(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        return SensorNode_List.Attr_BLorWL[bli];
    }
    return 0xff;
}

/*******************************************************
*Function Name 	:GetSenID_RegState
*Description  	:取得这个ID的注册状态
*Input			:
*Output			:0xFF:表示列表中没有这个ID；其他值，则表示该ID的注册状态
*******************************************************/
uint8_t GetSenID_RegState(uint8_t *SD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        return SensorNode_List.Register_State[bli];
    }
    return 0xff;
}

/*******************************************************
*Function Name 	:ADDID_RegState
*Description  	:增加名单,同时修改注册状态
*Input			:
*Output			:true:表示成功;false:表示没有
*******************************************************/
bool ADDID_RegState(uint8_t *SD_ID,uint8_t regstate)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        SensorNode_List.Register_State[bli]=regstate; //直接修改状态
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	  //清注册失败的累计值		
        return true;
    }
    //如果不存在则添加在表尾
    if(SensorNode_List.Nodenum>(SensorLIST_MAXNUM-1))		//名单数量超过最大值
    {
        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in ADDID_RegState()!");
        return false;
    }
    memcpy(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],SD_ID,6);			//把该ID加入到该列表的末尾
    SensorNode_List.Register_State[SensorNode_List.Nodenum]=regstate; 			//直接修改状态	
	SensorNode_List.Register_FailTime[SensorNode_List.Nodenum]	=SelfNode_TimeStamp;				//清注册失败的累计值
    SensorNode_List.Nodenum++;													//名单数量加1
    return true;
}

/*******************************************************
*Function Name 	:ADDID_Attrib
*Description  	:增加名单,同时修改名单属性
*Input			:
*Output			:true:表示成功;false:表示没有
*******************************************************/
bool ADDID_Attrib(uint8_t *SD_ID,uint8_t attribe)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        SensorNode_List.Attr_BLorWL[bli]=attribe; //直接修改属性
		if(attribe==SenAttri_BL)				
			SensorNode_List.Register_State[bli]=SenRegSt_UnReg;	//如果是黑名单则，注册标识清零
        return true;
    }
    //如果不存在则添加在表尾
    if(SensorNode_List.Nodenum>(SensorLIST_MAXNUM-1))		//名单数量超过最大值
    {
        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in ADDID_Attrib()!");
        return false;
    }
    memcpy(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],SD_ID,6);			//把该ID加入到该列表的末尾
    SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum]=attribe; 				//直接修改属性
	if(attribe==SenAttri_BL)        
		SensorNode_List.Register_State[SensorNode_List.Nodenum]=SenRegSt_UnReg;	//如果是黑名单则，注册标识清零
    SensorNode_List.Nodenum++;													//名单数量加1
    return true;
}


/*******************************************************
*Function Name 	:SenID_ModifyAttrib
*Description  	:修改名单属性
*Input			:
*Output			:true:表示成功;false:表示没有
*******************************************************/
bool SenID_ModifyAttrib(uint8_t *SD_ID,uint8_t attribe)
{
    uint16_t bli;
    bli=FixPosition_InSenList(SD_ID);	//判断是否在列表中
    if(bli!=0xffff)
    {
        SensorNode_List.Attr_BLorWL[bli]=attribe; //直接修改属性
        return true;
    }
    return false;
}


/*******************************************************
*Function Name 	:SenID_DEL
*Description  	:在列表中删除ID：删除过程是把列表尾的ID移到当前要被删除ID所在位置,这样可以省却列表移位的操作了；
*Input			:
*Output			:true:表示成功;false:表示没有
*******************************************************/
bool SenID_DEL(uint8_t *BSD_ID)
{
    uint16_t bli;
    bli=FixPosition_InSenList(BSD_ID);	//判断是否在列表中
    if(bli==0xffff) 	//不在列表中
    {
        return false;
    }
    if((bli+1)==SensorNode_List.Nodenum)  //正好是最后一个
    {
        memset(SensorNode_List.Sensor_ID[bli],0,6);						//清ID
        SensorNode_List.Register_State[bli]		=SenRegSt_UnReg;		//清注册状态
        SensorNode_List.Attr_BLorWL [bli]		=SenAttri_Undistrib;	//清黑白属性		
		SensorNode_List.Register_FailTime[bli]	=SelfNode_TimeStamp;	  					//清注册失败的累计值
		
        SensorNode_List.BS_Cycle[bli]			=0;						//清业务周期
        SensorNode_List.REQ_Cycle[bli]			=0;						//清控制周期
        SensorNode_List.ShockTime[bli]			=0;				//振荡时间
        SensorNode_List.DelayTime[bli]			=0;				//时隙编号

        SensorNode_List.BSCH_Fre[bli]			=0;				//频点
        SensorNode_List.SenID_SenLimit[bli][0]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][1]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][2]	=0;				//门限
        SensorNode_List.SenID_SenLimit[bli][3]	=0;				//门限
        SensorNode_List.SenID_ParaBkUp[bli]		=0;				//备用
        SensorNode_List.SenID_LimitType[bli][0]	=0;				//门限类型
        SensorNode_List.SenID_LimitType[bli][1]	=0;				//门限类型
        SensorNode_List.Nodenum--;				//名单数量减1
        if(En_debugIP1) printf("\r\n DEL list succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",BSD_ID[0],BSD_ID[1],BSD_ID[2],BSD_ID[3],BSD_ID[4],BSD_ID[5]);
        return true;
    }
    //把名单列表中最后一个ID替代当前要被删除的ID，这样就可以省却移动列表的操作了；
    SensorNode_List.Nodenum--;				//名单数量减1
    memcpy(SensorNode_List.Sensor_ID[bli],SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],6);	//替换ID
    SensorNode_List.Register_State[bli]	=SensorNode_List.Register_State[SensorNode_List.Nodenum];	//注册状态
    SensorNode_List.Attr_BLorWL[bli]	=SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum];		//黑白属性
	SensorNode_List.Register_FailTime[bli]=SensorNode_List.Register_FailTime[SensorNode_List.Nodenum];//注册失败的累计值
    SensorNode_List.BS_Cycle[bli]=SensorNode_List.BS_Cycle[SensorNode_List.Nodenum];				//业务周期
    SensorNode_List.REQ_Cycle[bli]=SensorNode_List.REQ_Cycle[SensorNode_List.Nodenum];				//控制周期
    SensorNode_List.ShockTime[bli]=SensorNode_List.ShockTime[SensorNode_List.Nodenum];				//振荡时间
    SensorNode_List.DelayTime[bli]=SensorNode_List.DelayTime[SensorNode_List.Nodenum];				//时隙编号

    SensorNode_List.BSCH_Fre[bli]=SensorNode_List.BSCH_Fre[SensorNode_List.Nodenum];						//频点
    SensorNode_List.SenID_SenLimit[bli][0]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][0];	//门限
    SensorNode_List.SenID_SenLimit[bli][1]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][1];	//门限
    SensorNode_List.SenID_SenLimit[bli][2]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][2];	//门限
    SensorNode_List.SenID_SenLimit[bli][3]	=SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][3];	//门限
    SensorNode_List.SenID_ParaBkUp[bli]	 	=SensorNode_List.SenID_ParaBkUp[SensorNode_List.Nodenum];		//备用
    SensorNode_List.SenID_LimitType[bli][0]	=SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][0];	//门限类型
    SensorNode_List.SenID_LimitType[bli][1]	=SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][1];	//门限类型

    //把列表中最后一个设备的数据清零
    memset(SensorNode_List.Sensor_ID[SensorNode_List.Nodenum],0,6);									//ID
    SensorNode_List.Register_State[SensorNode_List.Nodenum]		=SenRegSt_UnReg;		//注册状态
    SensorNode_List.Attr_BLorWL[SensorNode_List.Nodenum]		=SenAttri_Undistrib;	//黑白属性
	SensorNode_List.Register_FailTime[SensorNode_List.Nodenum]	=SelfNode_TimeStamp;	//清注册失败的累计值					
	
    SensorNode_List.BS_Cycle[SensorNode_List.Nodenum]			=0;				//业务周期
    SensorNode_List.REQ_Cycle[SensorNode_List.Nodenum]			=0;				//控制周期
    SensorNode_List.ShockTime[SensorNode_List.Nodenum]			=0;				//振荡时间
    SensorNode_List.DelayTime[SensorNode_List.Nodenum]	=0;				//时隙编号

    SensorNode_List.BSCH_Fre[SensorNode_List.Nodenum]			=0;				//频点
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][0]	=0;				//门限
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][1]	=0;				//门限
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][2]	=0;				//门限
    SensorNode_List.SenID_SenLimit[SensorNode_List.Nodenum][3]	=0;				//门限
    SensorNode_List.SenID_ParaBkUp[SensorNode_List.Nodenum]		=0;				//备用
    SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][0]	=0;				//门限类型
    SensorNode_List.SenID_LimitType[SensorNode_List.Nodenum][1]	=0;				//门限类型

    if(En_debugIP1) printf("\r\n DEL list succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",BSD_ID[0],BSD_ID[1],BSD_ID[2],BSD_ID[3],BSD_ID[4],BSD_ID[5]);
    return true;
}


/*******************************************************
*Function Name 	:SenID_DEL
*Description  	:打印设备列表；
*Input			:pflag:0-所有列表；1-黑名单；2-白名单；3-注册成功；4-注册未通过；5-注册成功的白名单；6-注册失败的黑名单；
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
//*Description  	:在名单中增加ID，先查表确认是否属于更新，否则再加入表尾
//*Input			:SensorAgent_Str*SenPt：代理指针
//				:addflag:0-清所有参数；1-修改参数；2-保持参数不变
//*Output			:true:表示成功;false:表示没有
//*******************************************************/
//uint16_t WhiteList_ADDID(SensorAgent_Str *SenPt,uint8_t addflag)
//{
//    uint16_t bli;
//    uint16_t tempi;
//    if(WhiteSensorNode_List.Nodenum>WHITELIST_MAXNUM)		//名单数量超过最大值
//    {
//        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM in WhiteList_ADDID()!");
//        return 0xffff;
//    }
//    //先查表确认该ID是否已经存在，如存在则更新参数
//    for(bli=0; bli<WhiteSensorNode_List.Nodenum; bli++)
//    {
//        //if(WhiteSensorNode_List.WhiteIDList[bli]==SenPt->SenID)	//ID是否相等
//        if((WhiteSensorNode_List.WhiteIDList[bli][0]==SenPt->SenID[0])&&(WhiteSensorNode_List.WhiteIDList[bli][1]==SenPt->SenID[1])&&(WhiteSensorNode_List.WhiteIDList[bli][2]==SenPt->SenID[2])&&	//ID是否相等
//                (WhiteSensorNode_List.WhiteIDList[bli][3]==SenPt->SenID[3])&&(WhiteSensorNode_List.WhiteIDList[bli][4]==SenPt->SenID[4])&&(WhiteSensorNode_List.WhiteIDList[bli][5]==SenPt->SenID[5]))	//ID是否相等
//        {
//            switch(addflag)
//            {
//            case 0:	//清参数
//                WhiteSensorNode_List.BS_Cycle[bli]=0;
//                WhiteSensorNode_List.REQ_Cycle[bli]=0;
//                WhiteSensorNode_List.WhID_Slotorder[bli]=0;
//                WhiteSensorNode_List.ShockTime[bli]=0;
//                WhiteSensorNode_List.WhID_ParaBkUp[bli]=0;					//备用参数
//                WhiteSensorNode_List.WhID_SenLimit[bli][0]=0;				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][1]=0;				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][2]=0;				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][3]=0;				//门限
//                WhiteSensorNode_List.WhID_LimitType[bli][0]=0;			//类型
//                WhiteSensorNode_List.WhID_LimitType[bli][1]=0;			//类型
//                break;
//            case 1: //修改参数
//                WhiteSensorNode_List.BS_Cycle[bli]=SenPt->SenBS_Cycle;
//                WhiteSensorNode_List.REQ_Cycle[bli]=SenPt->SenREQ_Cycle;
//                WhiteSensorNode_List.WhID_Slotorder[bli]=SenPt->SenSlotorder;
//                WhiteSensorNode_List.ShockTime[bli]=SenPt->SenShockTime;
//                WhiteSensorNode_List.WhID_ParaBkUp[bli]=SenPt->SenParaBkUp;					//备用参数
//                WhiteSensorNode_List.WhID_SenLimit[bli][0]=SenPt->SenLimit[0];				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][1]=SenPt->SenLimit[1];				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][2]=SenPt->SenLimit[2];				//门限
//                WhiteSensorNode_List.WhID_SenLimit[bli][3]=SenPt->SenLimit[3];				//门限
//                WhiteSensorNode_List.WhID_LimitType[bli][0]=SenPt->SenLimitType[0];			//类型
//                WhiteSensorNode_List.WhID_LimitType[bli][1]=SenPt->SenLimitType[1];			//类型
//                break;
//            default:
//                break;
//            }
//            return bli;
//        }
//    }
//    //如果不存在则把ID加到表格最后
//    if(WhiteSensorNode_List.Nodenum>(WHITELIST_MAXNUM-1))		//名单数量超过最大值
//    {
//        if(En_debugIP1) printf("\r\nthe list'num is exceed MAXNUM-1 in WhiteList_ADDID()!");
//        return 0xffff;
//    }
//    for(tempi=0; tempi<6; tempi++)
//        WhiteSensorNode_List.WhiteIDList[bli][tempi]=SenPt->SenID[tempi];
//    switch(addflag)
//    {
//    case 0:	//清参数
//        WhiteSensorNode_List.BS_Cycle[bli]=0;
//        WhiteSensorNode_List.REQ_Cycle[bli]=0;
//        WhiteSensorNode_List.WhID_Slotorder[bli]=0;
//        WhiteSensorNode_List.ShockTime[bli]=0;
//        WhiteSensorNode_List.WhID_ParaBkUp[bli]=0;					//备用参数
//        WhiteSensorNode_List.WhID_SenLimit[bli][0]=0;				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][1]=0;				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][2]=0;				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][3]=0;				//门限
//        WhiteSensorNode_List.WhID_LimitType[bli][0]=0;			//类型
//        WhiteSensorNode_List.WhID_LimitType[bli][1]=0;			//类型
//        break;
//    case 1: //修改参数
//        WhiteSensorNode_List.BS_Cycle[bli]=SenPt->SenBS_Cycle;
//        WhiteSensorNode_List.REQ_Cycle[bli]=SenPt->SenREQ_Cycle;
//        WhiteSensorNode_List.WhID_Slotorder[bli]=SenPt->SenSlotorder;
//        WhiteSensorNode_List.ShockTime[bli]=SenPt->SenShockTime;
//        WhiteSensorNode_List.WhID_ParaBkUp[bli]=SenPt->SenParaBkUp;					//备用参数

//        WhiteSensorNode_List.WhID_SenLimit[bli][0]=SenPt->SenLimit[0];				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][1]=SenPt->SenLimit[1];				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][2]=SenPt->SenLimit[2];				//门限
//        WhiteSensorNode_List.WhID_SenLimit[bli][3]=SenPt->SenLimit[3];				//门限
//        WhiteSensorNode_List.WhID_LimitType[bli][0]=SenPt->SenLimitType[0];						//类型
//        WhiteSensorNode_List.WhID_LimitType[bli][1]=SenPt->SenLimitType[1];						//类型
//        break;
//    }
//    WhiteSensorNode_List.Nodenum++;												//名单数量加1
//    RegList[BASICREGLIST_LENGTH-2]=WhiteSensorNode_List.Nodenum;				//同时清对应基本信息寄存器
//    return bli;
//}

///*******************************************************
//*Function Name 	:WhiteList_DELID
//*Description  	:在名单中删除ID：删除过程是把列表尾的ID移到当前要被删除ID所在位置，这样可以省却列表移位的操作了；
//*Input			:
//*Output			:true:表示成功;false:表示没有
//*******************************************************/
//bool WhiteList_DELID(uint8_t *SD_ID)
//{
//    uint16_t bli;
//    uint16_t tempi;
//    if(WhiteSensorNode_List.Nodenum==0) 	//空列表
//    {
//        if(En_debugIP1) printf("\r\nthe blacklist is empty BlackList_DELID()!");
//        return false;
//    }

//    if(WhiteSensorNode_List.Nodenum==1) 	//名单中只有一个ID
//    {
//        if((WhiteSensorNode_List.WhiteIDList[0][0]==SD_ID[0])&&(WhiteSensorNode_List.WhiteIDList[0][1]==SD_ID[1])&&(WhiteSensorNode_List.WhiteIDList[0][2]==SD_ID[2])&&	//ID是否相等
//                (WhiteSensorNode_List.WhiteIDList[0][3]==SD_ID[3])&&(WhiteSensorNode_List.WhiteIDList[0][4]==SD_ID[4])&&(WhiteSensorNode_List.WhiteIDList[0][5]==SD_ID[5]))	//ID是否相等
//        {
//            WhiteSensorNode_List.Nodenum=0;					//名单数量清零
//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[0][tempi]=0;			//ID清零
//            RegList[BASICREGLIST_LENGTH-2]=0;			//同时清对应基本信息寄存器
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
//        //if(WhiteSensorNode_List.WhiteIDList[bli]==SD_ID)	//ID是否相等
//        if((WhiteSensorNode_List.WhiteIDList[bli][0]==SD_ID[0])&&(WhiteSensorNode_List.WhiteIDList[bli][1]==SD_ID[1])&&(WhiteSensorNode_List.WhiteIDList[bli][2]==SD_ID[2])&&	//ID是否相等
//                (WhiteSensorNode_List.WhiteIDList[bli][3]==SD_ID[3])&&(WhiteSensorNode_List.WhiteIDList[bli][4]==SD_ID[4])&&(WhiteSensorNode_List.WhiteIDList[bli][5]==SD_ID[5]))	//ID是否相等
//        {
//            WhiteSensorNode_List.Nodenum--;				//名单数量减1
//            RegList[BASICREGLIST_LENGTH-2]=WhiteSensorNode_List.Nodenum;			//同时清对应基本信息寄存器
//            //把名单列表中最后一个ID替代当前要被删除的ID，这样就可以省却移动列表的操作了；
//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[bli][tempi]=WhiteSensorNode_List.WhiteIDList[WhiteSensorNode_List.Nodenum][tempi];			//移动ID
//            WhiteSensorNode_List.BS_Cycle[bli]=WhiteSensorNode_List.BS_Cycle[WhiteSensorNode_List.Nodenum];					//业务周期
//            WhiteSensorNode_List.REQ_Cycle[bli]=WhiteSensorNode_List.REQ_Cycle[WhiteSensorNode_List.Nodenum];				//控制周期
//            WhiteSensorNode_List.ShockTime[bli]=WhiteSensorNode_List.ShockTime[WhiteSensorNode_List.Nodenum];				//振荡时间
//            WhiteSensorNode_List.WhID_Slotorder[bli]=WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum];		//时隙编号

//            WhiteSensorNode_List.BSCH_Fre[bli]=WhiteSensorNode_List.BSCH_Fre[WhiteSensorNode_List.Nodenum];					//频点
//            WhiteSensorNode_List.WhID_SenLimit[bli][0]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][0];		//门限
//            WhiteSensorNode_List.WhID_SenLimit[bli][1]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][1];		//门限
//            WhiteSensorNode_List.WhID_SenLimit[bli][2]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][2];		//门限
//            WhiteSensorNode_List.WhID_SenLimit[bli][3]=WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][3];		//门限
//            WhiteSensorNode_List.WhID_ParaBkUp[bli]=WhiteSensorNode_List.WhID_ParaBkUp[WhiteSensorNode_List.Nodenum];			//备用
//            WhiteSensorNode_List.WhID_LimitType[bli][0]=WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][0];			//门限类型
//            WhiteSensorNode_List.WhID_LimitType[bli][1]=WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][1];			//门限类型


////			for( tempi=0;tempi<WhID_Para_MAXNUM;tempi++)
////				WhiteSensorNode_List.WhID_Para[bli][tempi]=WhiteSensorNode_List.WhID_Para[WhiteSensorNode_List.Nodenum][tempi];				//移动参数

//            for(tempi=0; tempi<6; tempi++)
//                WhiteSensorNode_List.WhiteIDList[WhiteSensorNode_List.Nodenum][tempi]=0;			//原列表尾的ID清零
//            WhiteSensorNode_List.BS_Cycle[WhiteSensorNode_List.Nodenum]=0;				//业务周期清零
//            WhiteSensorNode_List.REQ_Cycle[WhiteSensorNode_List.Nodenum]=0;				//控制周期清零
//            WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum]=0;		//延时时间清零
//            WhiteSensorNode_List.ShockTime[WhiteSensorNode_List.Nodenum]=0;				//原列表尾的振荡时间
//            WhiteSensorNode_List.WhID_Slotorder[WhiteSensorNode_List.Nodenum]=0;		//原列表尾的时隙编号清零

//            WhiteSensorNode_List.BSCH_Fre[WhiteSensorNode_List.Nodenum]=0;
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][0]=0;		//原列表尾的门限清零
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][1]=0;		//原列表尾的门限清零
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][2]=0;		//原列表尾的门限清零
//            WhiteSensorNode_List.WhID_SenLimit[WhiteSensorNode_List.Nodenum][3]=0;		//原列表尾的门限清零
//            WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][0]=0;		//门限类型
//            WhiteSensorNode_List.WhID_LimitType[WhiteSensorNode_List.Nodenum][1]=0;		//门限类型
//            if(En_debugIP1) printf("\r\n DEL whilelist succeed,ID=%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//            return true;
//        }
//    }
//    if(En_debugIP1) printf("\r\n DEL whilelist fail,there is no thisID in whitelist:%02X, %02X, %02X, %02X, %02X, %02X!",SD_ID[0],SD_ID[1],SD_ID[2],SD_ID[3],SD_ID[4],SD_ID[5]);
//    return false;
//}


