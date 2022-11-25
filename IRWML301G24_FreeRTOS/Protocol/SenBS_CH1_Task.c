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
#include "SenBS_CH1_Task.h"

extern bool En_debugIP2 ;
/*******************************************************
*Function Name	:SenBS_CH1_DataPro
*Description  	:微功率侧控制信道接收的数据处理函数
*Input			:Redata_pt：接收缓冲区；
*Output			:0：表示收到的Message帧格式有问题（不用处理）；

*Question   	:1、
*******************************************************/
uint8_t  SenBS_CH1_DataPro(uint8_t *Redata_pt)
{
    uint8_t datasum,sxi;
    uint8_t readlen=0;
    uint8_t SenID[6];
    uint8_t Senattri,SenRegstat;

    //检测层头
    if(((*Redata_pt)>>4)!=Sen_MESSAGE)
    {
        if(En_debugIP2)  printf("\r\n第一字节=%d：表明不是MESSAGE帧！",*Redata_pt);
        return 0;
    }
    //////////////检查校验和///////////////
    readlen=Redata_pt[1];			//获取MESSAGE的MACPayload长度
    if((readlen>(SX_SENS_RECBUF_SIZE-9))||(readlen==0)) 	//长度超过最大值
    {
//        printf("\r\nSenBS_CH1_DataPro中接收数据长度=%d",readlen);
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
            printf("\r微功率侧业务信道接收帧数据校验字错误,累加和=%d \r",datasum);
            for(sxi=0; sxi<(readlen+9); sxi++)
                printf(" %02X",Redata_pt[sxi]);
        }
        return 0;
    }
    ////////////检查传感器ID是否属于黑名单//////////////
    SenID[0]=Redata_pt[2];
    SenID[1]=Redata_pt[3];
    SenID[2]=Redata_pt[4];
    SenID[3]=Redata_pt[5];
    SenID[4]=Redata_pt[6];
    SenID[5]=Redata_pt[7];
	
    Senattri=GetSenID_Attri(SenID);			//取得该传感器的黑白属性
    SenRegstat=GetSenID_RegState(SenID);	//取得该传感器的注册状态
	/**由于微功率传感器不会主动发起注册，只会是由接收到业务数据后才触发注册，故对微功率来说接收到业务数据：
	（1）不在设备列表中，且列表数量未满，则发起注册；
	（2）在设备列表中为黑名单，则不处理；
	（3）在设备列表中非黑已注册，则发送数据；
	（4）在设备列表中非黑且注册失败，则查看是否超时，如果超时，则进行重新注册，否则不处理；
	（5）在设备列表中非黑且未，则发起注册；
	**/
    if(Senattri==SenAttri_BL) 	//即（2）属于黑名单，不处理
        return 1;
    if(Senattri==0xff) 		//即不在设备名单中(1)，进行添加设备并发起注册
        return 2;

    //名单属性为未分配或者白名单
    if(SenRegstat==SenRegSt_UnReg)		//即（5）在名单中（属性为：非黑），未注册，进行注册
        return 3;
    if(SenRegstat==SenRegSt_Success) 	//即（3）在名单中（属性为：非黑），注册成功，进行数据上传
        return 4;
    if(SenRegstat==SenRegSt_Fail) 		//即（4）在名单中（属性为：非黑），注册不通过，不处理
        return 5;
	return 6;
}



