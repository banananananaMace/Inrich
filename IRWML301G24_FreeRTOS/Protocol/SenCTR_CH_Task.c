//////////////////////////////////////////////////////////////////////////////////
//创建日期:2019/7/23
//版本：V1.0
//Copyright(C) Inrich
//功能描述：
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
#include "SenCTR_CH_Task.h"

extern bool En_debugIP2 ;
/*******************************************************
*Function Name	:SenCTR_CH_DataPro
*Description  	:微功率侧控制信道接收的数据处理函数
*Input			:Redata_pt：接收缓冲区；
*Output			:0：表示收到的Control_Dow帧格式有问题，或者是黑名单；
				 0x10：表示收到REQ帧的所有控制参数请求，
				 0x11：表示收到REQ帧的通信指令参数请求，
				 0x12：表示收到REQ帧的业务控制参数请求，
				 0x02：表示收到告警数据
				 0x03：表示收到REQ_ACK
*Question   	:1、
*******************************************************/
uint8_t  SenCTR_CH_DataPro(uint8_t *Redata_pt)
{
    uint8_t datasum,sxi;
    uint8_t readlen=0;
    //uint16_t sennum;
    uint8_t SenID[6];
    //////////////检查校验和///////////////
    readlen=Redata_pt[1];			//获取MESSAGE的MACPayload长度    
	if((readlen>(SX_SENS_RECBUF_SIZE-9))||(readlen==0)) 	//长度超过最大值
    {
//        printf("\r\nSenCTR_CH_DataPro中接收数据长度=%d",readlen);
        return 0;
    }
	
    datasum=0;
    //for(sxi=0; sxi<(readlen+6); sxi++)//因为readlen是MACPayload的长度，而总长度为MACPayload的长度readlen+7
    for(sxi=0; sxi<(readlen+8); sxi++)//因为readlen是MACPayload的长度，而总长度为MACPayload的长度readlen+9
    {
        datasum+=Redata_pt[sxi];		//累加和
    }
    //判断校验字节，即累加和是否一致
    //if(datasum!=Redata_pt[readlen+6]) //因为readlen是MACPayload的长度，而总长度为MACPayload的长度+7，故最后一个字节位置是：readlen+6
    if(datasum!=Redata_pt[readlen+8]) //因为readlen是MACPayload的长度，而总长度为MACPayload的长度+9，故最后一个字节位置是：readlen+8
    {
        if(En_debugIP2)  
		{
			printf("\r微功率侧控制信道接收帧数据校验字错误,累加和=%d\r",datasum);
			for(sxi=0;sxi<(readlen+9);sxi++)
				printf(" %02X",Redata_pt[sxi]);   
		}			
        return 0;
    }
    //取得该信息中的传感器ID
    //SenID=((uint32_t)Redata_pt[2]<<24)+((uint32_t)Redata_pt[3]<<16)+((uint32_t)Redata_pt[4]<<8)+Redata_pt[5];	//取得传感器ID
    SenID[0]=Redata_pt[2];
    SenID[1]=Redata_pt[3];
    SenID[2]=Redata_pt[4];
    SenID[3]=Redata_pt[5];
    SenID[4]=Redata_pt[6];
    SenID[5]=Redata_pt[7];
    ////////////检查传感器ID是否属于黑名单//////////////
    if(CheckID_isBL(SenID)!=0xffff)  //如果属于黑名单中，则不回应
    {
        if(En_debugIP2)  printf("\r\n 微功率侧控制信道接收帧ID在黑名单中");
        return 0;
    }

    ////////根据不同层头类型进行处理////////////////
    datasum=0;
    switch((*Redata_pt)>>4)
    {
    case Sen_REQ:	//这里暂不处理，放在任务函数中去处理（涉及消息队列的操作）
//        printf("\r\n接收到REQ数据如下:\r\n");
//        for(sxi=0; sxi<(readlen+7); sxi++) //总长度为MACPayload的长度readlen+7
//            printf(" %d",Redata_pt[sxi]);

        //RSP_Send(SenID,15,Sensor_MESSAGE_CYCLE);		//回复RSP_END帧,设置业务周期
        //delay_xms(40);
        //RSP_Send(SenID,15,Sensor_CONTROL_CYCLE);		//回复RSP_END帧,设置控制周期
        //delay_xms(40);
        //RSPEND_Send(SenID,15,Sensor_FOUR_PARAMETER);		//回复RSP_END帧,设置4个参数
        //RSPEND_Send(SenID,15,Sensor_MESSAGE_CYCLE);		//回复RSP_END帧,设置业务周期
        //RSPEND_Send(SenID,15,Sensor_CONTROL_CYCLE);		//回复RSP_END帧,设置控制周期
        //RSPEND_Send(SenID,15,Sensor_DELAYTIME);			  //回复RSP_END帧,设置延时时间
		
        switch(Redata_pt[8])
        {
        case 0:		//所有控制参数；
            datasum=0x10;
            break;
        case 1:		//通信指令参数；
            datasum=0x11;
            break;
        case 2:		//业务控制参数；
            datasum=0x12;
            break;
        default:
            if(En_debugIP2)  
			{
				printf("\r\n REQ的类型有误，其类型=%d！",Redata_pt[8]);
				for(sxi=0;sxi<(readlen+9);sxi++)
				 printf(" %02X",Redata_pt[sxi]);
			}
            datasum=0; //控制请求类型有误
            break;
        }
        break;
    case Sen_BURST:	//告警数据
//        printf("\r\n接收到BURST数据如下:\r\n");
//        for(sxi=0; sxi<(readlen+9); sxi++) //总长度为MACPayload的长度readlen+7
//            printf(" %d",Redata_pt[sxi]);
//        //BURST_ACK_Send(SenID);
        datasum=0x02;
        break;
    case Sen_ACK:	//该ACK是指传感器接收到RSP_END后发送的RSP_END_ACK
//        printf("\r\n接收到RSP_END_ACK数据如下:\r\n");
//        for(sxi=0; sxi<(readlen+9); sxi++) //总长度为MACPayload的长度readlen+7
//            printf(" %d",Redata_pt[sxi]);
        datasum=0x03;
        break;
    default :
//        printf("\r\n第一字节=%d：表明不是REQ或BURST或ACK帧！",*Redata_pt);
//		for(sxi=0;sxi<(readlen+9);sxi++)
//			printf(" %02X",Redata_pt[sxi]);
        datasum=0;
        break;
    }
    return datasum;
}



/*******************************************************
*Function Name	:BURST_ACK_Send
*Description 	:接入侧在控制信道接收到BURST帧后，需回复BURST_ACK
*Input		  	:SenID：传感器ID；
*Output		  	:
Question	  	:1、
*******************************************************/
void  BURST_ACK_Send(uint8_t *SenID)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0; //8+1+2=11//
    //MHDR：为物理层负载帧头,8个字节
    SX_Sens_SndBuf[0][ptbuf++]=Sen_ACK<<4;			//RSP_END帧类型3，无加密
    SX_Sens_SndBuf[0][ptbuf++]=0;					//暂定0，后补；payload的长度是总长度length减去层头的6字节和MIC的1字节
    SX_Sens_SndBuf[0][ptbuf++]=SenID[0];			//传感器ID的第1个字节，
    SX_Sens_SndBuf[0][ptbuf++]=SenID[1];			//传感器ID的第2个字节，
    SX_Sens_SndBuf[0][ptbuf++]=SenID[2];			//传感器ID的第3个字节，
    SX_Sens_SndBuf[0][ptbuf++]=SenID[3];			//传感器ID的第4个字节，
    SX_Sens_SndBuf[0][ptbuf++]=SenID[4];			//传感器ID的第5个字节，
    SX_Sens_SndBuf[0][ptbuf++]=SenID[5];			//传感器ID的第6个字节，

    //MACPayload:共2个字节
    SX_Sens_SndBuf[0][ptbuf++]=0x13;  					//物理量个数：1；分片指示：0；帧类型：3 ，故数据位为：0001 0 011
    //pti=0xFFFC;											//物理类型：3FF；数据长度指示：00
    SX_Sens_SndBuf[0][ptbuf++]=0xff;

//    SX_Sens_SndBuf[0][ptbuf++]=Sen_BURST_ACK;		//表示告警应答
//    SX_Sens_SndBuf[0][ptbuf++]=1;					//表示突发确认

    //CRC校验位
    ptlength=ptbuf-2;
    pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
    SX_Sens_SndBuf[0][ptbuf++]=pti>>8;
    SX_Sens_SndBuf[0][ptbuf++]=pti;

    //MIC
    SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
    ptlength=ptbuf;									//得到帧的总长度
    SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
    //计算所有数据的累加和
    for(pti=0; pti<(ptlength-1); pti++)
    {
        SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
    }
    SX1280_Data_Send(0,ptlength);					//接入侧控制信道数据发送
}

/*******************************************************
*Function Name	:SenCTR_ComPara_REQRSP
*Description 	:接入侧接收到REQ帧后，设置通信参数
*Input		  	:SenID_Serial：该传感器在白名单列表中的序号；
				 Rspend_type：0-按照RSP_END；1-RSP
*Output		  	:
Question	  	:1、业务时隙段时间长度暂定200ms
				 2、取得当前时间戳时，减去时间差，暂定30MS；
*******************************************************/
bool  SenCTR_ComPara_REQRSP(uint16_t SenID_Serial,u8 Rsp_type)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0;
//    uint32_t senslot_lenth;		//时隙长度（ms）

//    uint32_t assignslot_order;
    uint32_t DelayTime=0;
//    uint16_t curslot_order=0;
    uint32_t bs_cycle;
    uint16_t req_cycle;
    uint8_t  shocktimelength;
//    uint8_t  bsfre;
//    uint32_t limitsen[4];
//    uint16_t backup;
 //   uint8_t  limittype[2];

    bs_cycle=SensorNode_List.BS_Cycle[SenID_Serial];  			//取得传感器对应的业务周期，ms为单位
    req_cycle=SensorNode_List.REQ_Cycle[SenID_Serial];  		//取得传感器对应的控制周期
    shocktimelength=SensorNode_List.ShockTime[SenID_Serial];	//取得传感器对应的扰动时长，以5ms为单位
	DelayTime=SensorNode_List.DelayTime[SenID_Serial];			//取得传感器对应的延时时长，以5ms为单位
//  bsfre=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  		//取得传感器对应的业务频点
//  assignslot_order=SensorNode_List.BSCH_Fre[SenID_Serial];  	//取得传感器对应的时隙编号
	
//   senslot_lenth=200; 	//取得传感器对应的时隙长度ms

//    limitsen[0]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][0];//门限0
//    limitsen[1]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][1];//门限1
//    limitsen[2]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][2];//门限2
//    limitsen[3]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][3];//门限3
//    limittype[0]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][0];
//    limittype[1]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][1];
//    backup = WhiteSensorNode_List.WhID_ParaBkUp[SenID_Serial];		//备用

//    //如果时隙编号大于最大值,则取余数
//    if(assignslot_order>((bs_cycle)/senslot_lenth))
//        assignslot_order=0;	//assignslot_order=assignslot_order%((bs_cycle)/senslot_lenth);

    ptbuf=0;
    ptlength=0;			//
    //MHDR：为物理层负载帧头
    if(Rsp_type==0)
        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP_END<<4;		//RSP_END帧类型3，通信指令指示0（即为通信参数） ，无加密
    else
        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP<<4;			//Sen_RSP类型2，通信指令指示0（即为通信参数） ，无加密
    SX_Sens_SndBuf[0][ptbuf++]=0;					//暂定0，后面再补；payload的长度是总长度length减去层头的6字节和MIC的1字节
	memcpy(&SX_Sens_SndBuf[0][ptbuf],SensorNode_List.Sensor_ID[SenID_Serial],6);	//传感器ID
	ptbuf=ptbuf+6;
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][0];			//传感器ID的第1个字节，即最高8位
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][1];			//传感器ID的第2个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][2];			//传感器ID的第3个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][3];			//传感器ID的第4个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][4];			//传感器ID的第5个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][5];			//传感器ID的第6个字节，即最低8位
    //通信指令不按规约，按照协议规定
//    if((bs_cycle!=0)&&(req_cycle!=0)&&(DelayTime!=0)&&(shocktimelength!=0)) //4个参数都不为0，则可以进行组合参数下发
    if((bs_cycle!=0)&&(req_cycle!=0)&&(DelayTime!=0)&&(shocktimelength!=0)) //4个参数都不为0，则可以进行组合参数下发
	{
        SX_Sens_SndBuf[0][ptbuf++]=Sensor_FOUR_PARAMETER;	//控制信息类型：前四个组合
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.BS_Cycle[SenID_Serial],4);  //业务周期4个字节
		ptbuf=ptbuf+4;
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.REQ_Cycle[SenID_Serial],2);  //控制周期2个字节
		ptbuf=ptbuf+2;
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.DelayTime[SenID_Serial],4);  //延时时间4个字节
		ptbuf=ptbuf+4;
		SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.ShockTime[SenID_Serial];			//随机时间震荡范围，以5ms为时间单位；
		
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//业务周期第4个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//业务周期第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//业务周期第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//业务周期第1个字节，即最高8位
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//控制周期第2个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//控制周期第1个字节，
        //TimeStamp=1000*9;	//22秒，用于测试
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime;				//延时周期第4个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;			//延时周期第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;			//延时周期第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;			//延时第1个字节，即最高8位
//        SX_Sens_SndBuf[0][ptbuf++]=shocktimelength;			//随机时间震荡范围，以5ms为时间单位；
    }
    else
    {
        if(bs_cycle!=0)	//设置业务周期
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_MESSAGE_CYCLE;	//控制信息类型：业务周期
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.BS_Cycle[SenID_Serial],4);  //业务周期4个字节
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//业务周期第4个字节，即最低8位
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//业务周期第3个字节，
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//业务周期第2个字节，
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//业务周期第1个字节，即最高8位
        }
        if(req_cycle!=0)//控制周期
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_CONTROL_CYCLE;	//控制信息类型：控制周期周期
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.REQ_Cycle[SenID_Serial],2);  //控制周期2个字节
			ptbuf=ptbuf+2;
//            SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//控制周期第4个字节，即最低8位
//            SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//控制周期第3个字节，
        }
        if(DelayTime!=0)//延时时间，/时隙编号需从1开始
        {            
            //TimeStamp=1000*9;	//22秒，用于测试
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_DELAYTIME;			//控制信息类型：延时时间
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.DelayTime[SenID_Serial],4);  //延时时间4个字节
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime;					//延时周期第4个字节，即最低8位
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;				//延时周期第3个字节，
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;				//延时周期第2个字节，
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;				//延时第1个字节，即最高8位
        }
        if(shocktimelength!=0)//振荡时间
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_SHOCKTIME;						//控制信息类型：振荡时间
            SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.ShockTime[SenID_Serial];	//随机时间震荡范围，以5ms为时间单位；
        }
    }
//    if(ptbuf>8)	//表示有参数需要设置
    {
        //MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
        ptlength=ptbuf;									//得到帧的总长度
        SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
        for(pti=0; pti<(ptlength-1); pti++)			//计算所有数据的累加和
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);				//接入侧控制REQ信道数据发送
        return 1;	//表示
    }
}

/*******************************************************
*Function Name	:SenCTR_CtrPara_REQRSP
*Description 	:接入侧接收到REQ帧后，设置控制参数
*Input		  	:SenID_Serial：该传感器在白名单列表中的序号；
				 Rspend_type：0-按照RSP_END；1-RSP
*Output		  	:
Question	  	:1、业务时隙段时间长度暂定200ms
				 2、取得当前时间戳时，减去时间差，暂定30MS；
*******************************************************/
bool  SenCTR_CtrPara_REQRSP(uint16_t SenID_Serial,u8 Rsp_type)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0;
//    uint32_t limitsen[4];
    uint16_t limittype[2];

//    limitsen[0]=SensorNode_List.SenID_SenLimit[SenID_Serial][0];//门限0
//    limitsen[1]=SensorNode_List.SenID_SenLimit[SenID_Serial][1];//门限1
//    limitsen[2]=SensorNode_List.SenID_SenLimit[SenID_Serial][2];//门限2
//    limitsen[3]=SensorNode_List.SenID_SenLimit[SenID_Serial][3];//门限3
    limittype[0]=SensorNode_List.SenID_LimitType[SenID_Serial][0];
    limittype[1]=SensorNode_List.SenID_LimitType[SenID_Serial][1];

    ptbuf=0;
    ptlength=0;			//
    //MHDR：为物理层负载帧头
	//SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;										//RSP_END帧类型3，通信指令指示1（即为控制参数） ，无加密
    if(Rsp_type==0)
        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;		//RSP_END帧类型3，通信指令指示1（即为控制参数） ，无加密
    else
        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP<<4)|0x08;			//Sen_RSP类型2， 通信指令指示1（即为控制参数） ，无加密
    SX_Sens_SndBuf[0][ptbuf++]=0;					//暂定0，后面再补；payload的长度是总长度length减去层头的6字节和MIC的1字节
	
	memcpy(&SX_Sens_SndBuf[0][ptbuf],SensorNode_List.Sensor_ID[SenID_Serial],6);	//传感器ID
	ptbuf=ptbuf+6;
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][0];			//传感器ID的第1个字节，即最高8位
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][1];			//传感器ID的第2个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][2];			//传感器ID的第3个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][3];			//传感器ID的第4个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][4];			//传感器ID的第5个字节，
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][5];			//传感器ID的第6个字节，即最低8位

    //        //MACPayload:共个字节
    //判断当前可以设置的限值数量m
    pti=0;
    if(limittype[0]!=0)
        pti++;
    if(limittype[1]!=0)
        pti++;
    if(pti!=0)	//有限值可以设置，即限值数量不是0
    {
        SX_Sens_SndBuf[0][ptbuf++]=(pti<<4)|0x04;  			//物理量个数：pti；分片指示：0；帧类型：4（控制报文） ，故数据位为：0001 0 100
        SX_Sens_SndBuf[0][ptbuf++]=(5<<1)|0x01;				//控制报文类型:5；参数配置标识：1（设置）
        //参数列表
        if(limittype[0]!=0)
        {
            SX_Sens_SndBuf[0][ptbuf++]=(limittype[0]<<2)|0x00;		//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
            SX_Sens_SndBuf[0][ptbuf++]=((limittype[0]<<2)|0x00)>>8;	//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][0],4);  //上限,4字节
			ptbuf=ptbuf+4;
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][1],4);  //下限,4字节
			ptbuf=ptbuf+4;
			
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0];			//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>8;			//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>16;		//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>24;		//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1];			//下限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>8;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>16;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>24;			//
        }
        if(limittype[1]!=0)
        {
            SX_Sens_SndBuf[0][ptbuf++]=(limittype[1]<<2)|0x00;		//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
            SX_Sens_SndBuf[0][ptbuf++]=((limittype[1]<<2)|0x00)>>8;	//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][2],4);  //上限,4字节
			ptbuf=ptbuf+4;
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][3],4);  //下限,4字节
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2];			//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>8;			//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>16;		//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>24;		//上限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3];			//下限
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>8;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>16;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>24;			//
        }
        //CRC校验位
        ptlength=ptbuf-2;
        pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
        SX_Sens_SndBuf[0][ptbuf++]=pti>>8;
        SX_Sens_SndBuf[0][ptbuf++]=pti;

        //MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
        ptlength=ptbuf;									//得到帧的总长度
        SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
        for(pti=0; pti<(ptlength-1); pti++)				//计算所有数据的累加和
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);					//接入侧控制REQ信道数据发送
        return 1;
    }
	else
	{
		//MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
        ptlength=ptbuf;									//得到帧的总长度
        SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
        for(pti=0; pti<(ptlength-1); pti++)				//计算所有数据的累加和
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);					//接入侧控制REQ信道数据发送
		return 0;
	}    
}

///*******************************************************
//*Function Name	:RSPEND_Send
//*Description 	:接入侧接收到REQ帧后，需回复RSP_END发送函数
//*Input		  	:SenID_Serial：该传感器在白名单列表中的序号；
//				 Rspend_type
//*Output		  	:
//Question	  	:1、业务时隙段时间长度暂定200ms
//				 2、取得当前时间戳时，减去时间差，暂定30MS；
//*******************************************************/
//void  SenCTR_RSPEND_Send(uint16_t SenID_Serial,u8 Rspend_type)
//{
//    uint8_t ptbuf=0;
//    uint16_t pti;
//    uint8_t ptlength=0;
//    uint32_t senslot_lenth;		//时隙长度（ms）

//    uint32_t assignslot_order;
//    uint32_t DelayTime=0;
//    uint16_t curslot_order=0;
////    uint32_t SenID;
//    uint32_t bs_cycle;
//    uint16_t req_cycle;
//    uint8_t  shocktimelength;
////    uint8_t  bsfre;
//    uint32_t limitsen[4];
////    uint16_t backup;
//    uint8_t  limittype[2];

//    bs_cycle=WhiteSensorNode_List.BS_Cycle[SenID_Serial];  			//取得传感器对应的业务周期，ms为单位
//    req_cycle=WhiteSensorNode_List.REQ_Cycle[SenID_Serial];  		//取得传感器对应的控制周期
////    bsfre=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  			//取得传感器对应的业务频点
//    assignslot_order=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  	//取得传感器对应的时隙编号

//    //senslot_lenth=WhiteSensorNode_List.WhID_SlotLen[SenID_Serial]; 	//取得传感器对应的时隙长度
//    senslot_lenth=200; 	//取得传感器对应的时隙长度ms

//    shocktimelength=WhiteSensorNode_List.ShockTime[SenID_Serial];//取得传感器对应的扰动时长，以5ms为单位
//    limitsen[0]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][0];//门限0
//    limitsen[1]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][1];//门限1
//    limitsen[2]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][2];//门限2
//    limitsen[3]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][3];//门限3
//    limittype[0]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][0];
//    limittype[1]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][1];
////    backup = WhiteSensorNode_List.WhID_ParaBkUp[SenID_Serial];		//备用

////    //限制业务周期长度的最小值为：1s，
////    if(bs_cycle<1000)
////        bs_cycle=10000;

////    //限制控制周期长度的最小值为：5个业务周期
////    if(req_cycle<5)
////        req_cycle=5;

//    //如果时隙编号大于最大值,则取余数
//    if(assignslot_order>((bs_cycle)/senslot_lenth))
//        assignslot_order=assignslot_order%((bs_cycle)/senslot_lenth);

//    switch(Rspend_type)
//    {
//    case Sensor_FOUR_PARAMETER:
//        ptbuf=0;
//        ptlength=0;			//
//        //MHDR：为物理层负载帧头
//        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP_END<<4;		//RSP_END帧类型3，通信指令指示0（即为通信参数） ，无加密
//        SX_Sens_SndBuf[0][ptbuf++]=0;					//暂定0，后面再补；payload的长度是总长度length减去层头的6字节和MIC的1字节
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][0];			//传感器ID的第1个字节，即最高8位
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][1];			//传感器ID的第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][2];			//传感器ID的第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][3];			//传感器ID的第4个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][4];			//传感器ID的第5个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][5];			//传感器ID的第6个字节，即最低8位

////        //MACPayload:共个字节
////        SX_Sens_SndBuf[0][ptbuf++]=0x15;  					//物理量个数：1；分片指示：0；帧类型：5（控制响应报文） ，故数据位为：0001 0 101
////        //pti=0xFFFC;											//物理类型：3FF；数据长度指示：00
////        SX_Sens_SndBuf[0][ptbuf++]=0xff;
////        SX_Sens_SndBuf[0][ptbuf++]=0xfc;
//        //通信指令不按规约，按照协议规定
//        SX_Sens_SndBuf[0][ptbuf++]=0xFF;					//控制信息类型：前四个组合

//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//业务周期第4个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//业务周期第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//业务周期第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//业务周期第1个字节，即最高8位

//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//控制周期第4个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//控制周期第3个字节，
////        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>16;			//控制周期第2个字节，
////        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>24;			//控制周期第1个字节，即最高8位

//        //计算延时：需要用时隙编号，业务周期，时隙长度3个参数
//        if(assignslot_order!=0)	//时隙编号需从1开始
//        {
//            DelayTime=SelfNode_TimeStamp-15;					//取得当前时隙,其中15ms暂定算时间误差值：包括1280的传输时间以及传感器的定时误差
//            curslot_order=((DelayTime%(bs_cycle))/senslot_lenth)+1;	//计算出当前时间处于哪个业务时隙段，其中业务时隙段暂定200ms，时隙数是从第1段开始；
//            if(assignslot_order>curslot_order) 	//如果当前所在广播时隙段在分配的广播时隙段之前
//            {
//                curslot_order=assignslot_order-curslot_order; 	//计算差几段
//                DelayTime=((uint32_t)(curslot_order))*senslot_lenth;  //计算延时时间
//            }
//            else if(assignslot_order<curslot_order) //如果当前所在广播时隙段在分配的广播时隙段之后
//            {
//                curslot_order=bs_cycle/senslot_lenth-(curslot_order-assignslot_order);  //  SENSOR_SLOT_ORDER_MAX; 	//计算差几段
//                DelayTime=((uint32_t)(curslot_order))*senslot_lenth;  //计算延时时间
//            }
//            else
//                DelayTime=0;
//        }
//        else
//            DelayTime=0;

//        //TimeStamp=1000*9;	//22秒，用于测试
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime;					//延时周期第4个字节，即最低8位
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;				//延时周期第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;				//延时周期第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;				//延时第1个字节，即最高8位
//        SX_Sens_SndBuf[0][ptbuf++]=shocktimelength;				//随机时间震荡范围，以5ms为时间单位；

////        //CRC校验位
////        ptlength=ptbuf-2;
////        pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
////        SX_Sens_SndBuf[0][ptbuf++]=pti;
////        SX_Sens_SndBuf[0][ptbuf++]=pti>>8;

//        //MIC
//        SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
//        ptlength=ptbuf;									//得到帧的总长度
//        SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
//        for(pti=0; pti<(ptlength-1); pti++)			//计算所有数据的累加和
//        {
//            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
//        }
//        SX1280_Data_Send(0,ptlength);				//接入侧控制REQ信道数据发送
//        break;
//    case Sensor_Control_PARAMETER:	//设置控制参数（主要是上下限）
//        ptbuf=0;
//        ptlength=0;			//
//        //MHDR：为物理层负载帧头
//        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;										//RSP_END帧类型3，通信指令指示1（即为控制参数） ，无加密
//        SX_Sens_SndBuf[0][ptbuf++]=0;															//暂定0，后面再补；payload的长度是总长度length减去层头的6字节和MIC的1字节
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][0];			//传感器ID的第1个字节，即最高8位
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][1];			//传感器ID的第2个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][2];			//传感器ID的第3个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][3];			//传感器ID的第4个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][4];			//传感器ID的第5个字节，
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][5];			//传感器ID的第6个字节，即最低8位

//        //MACPayload:共个字节
//        //判断当前可以设置的限值数量m
//        pti=0;
//        if(limittype[0]!=0)
//            pti++;
//        if(limittype[1]!=0)
//            pti++;
//        if(pti!=0)	//有限值可以设置，即限值数量不是0
//        {
//            SX_Sens_SndBuf[0][ptbuf++]=(pti<<4)|0x04;  			//物理量个数：pti；分片指示：0；帧类型：4（控制报文） ，故数据位为：0001 0 100
//            SX_Sens_SndBuf[0][ptbuf++]=(5<<1)|0x01;				//控制报文类型:5；参数配置标识：1（设置）
//            //参数列表
//            if(limittype[0]!=0)
//            {
//                SX_Sens_SndBuf[0][ptbuf++]=(limittype[0]<<2)|0x00;		//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
//				SX_Sens_SndBuf[0][ptbuf++]=((limittype[0]<<2)|0x00)>>8;	//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0];			//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>8;			//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>16;		//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>24;		//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1];			//下限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>8;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>16;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>24;			//
//            }
//            if(limittype[1]!=0)
//            {
//                SX_Sens_SndBuf[0][ptbuf++]=(limittype[1]<<2)|0x00;		//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
//				SX_Sens_SndBuf[0][ptbuf++]=((limittype[1]<<2)|0x00)>>8;	//参数类型：limittype[0] ；数据长度指示位：0（无长度字节，默认4字节）
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2];			//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>8;			//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>16;		//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>24;		//上限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3];			//下限
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>8;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>16;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>24;			//
//            }
//            //CRC校验位
//            ptlength=ptbuf-2;
//            pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
//            SX_Sens_SndBuf[0][ptbuf++]=pti;
//            SX_Sens_SndBuf[0][ptbuf++]=pti>>8;

//            //MIC
//            SX_Sens_SndBuf[0][ptbuf++]=0;					//累加和清零
//            ptlength=ptbuf;									//得到帧的总长度
//            SX_Sens_SndBuf[0][1]=ptlength-9;				//修改payload的长度
//            for(pti=0; pti<(ptlength-1); pti++)				//计算所有数据的累加和
//            {
//                SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
//            }
//            SX1280_Data_Send(0,ptlength);					//接入侧控制REQ信道数据发送
//        }
//        break;
//    }
//}







