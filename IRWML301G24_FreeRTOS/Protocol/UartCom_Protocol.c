//////////////////////////////////////////////////////////////////////////////////
//创建日期:2019/8/16
//版本：V1.0
//Copyright(C) Inrich
//功能描述：
//1、串口通信协议所用函数与变量
//////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "stdio.h"
#include "malloc.h"
#include "usart.h"
#include "stmflash.h"
#include "NodeListFun.h"
#include "UartCom_Protocol.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "limits.h"

extern QueueHandle_t Uart1Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
extern QueueHandle_t Uart2Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
extern QueueHandle_t Uart4Tx_MessageData_Q;   				//串口4发送数据的消息队列句柄


///////////寄存器列表：基本信息/////////////
//RegList[0]:厂家
//RegList[1]:硬件版本号
//RegList[2]:软件版本号
//RegList[3]~[6]:序列号，高位在前
//RegList[BASICREGLIST_LENGTH-2]:白名单数量
//RegList[BASICREGLIST_LENGTH-1]:黑名单数量
//uint16_t RegList[BASICREGLIST_LENGTH]= {0xA92E,0x0001,0x0002,0x0003,0x0004,0x0005};
uint16_t USART1_SendFlag=0;	//0：表示串口1空闲；1-表示已经发送，等待回应,同时也表示重复发送次数；
uint16_t USART2_SendFlag=0;	//0：表示串口2空闲；1-表示已经发送，等待回应,同时也表示重复发送次数；
uint8_t ParaSendFlag=0;		//参数设置标志，用于301对外（203）设置参数：0-表示未发送；1-表示已经发送；2-表示对方接收成功

/*******************************************************
*Function Name	:USART2_Rx_Task
*Description  	:串口接收数据处理任务（）
*Input		  	:comnum:COM2-表示usart2；COM4-表示usart4
*Output		  	:
*Question	  	:1、
*******************************************************/
uint8_t USART_RxData_Pro(uint8_t *Redata_pt,uint8_t comnum)
{
//    uint8_t sxi;//,datasum;
//    uint16_t readadd=0;
//    uint16_t ParaFlag=0;
    uint16_t readlen=0;
    //判断帧头是否正确
    if((Redata_pt[0]!=COMFRAME_HEAD_FIRSTBYTE)||(Redata_pt[1]!=COMFRAME_HEAD_SECONDBYTE))
    {
        printf("\r\n the headword of comframe is error:comnum=%02X,firsthead=%02X,secondhead=%02X!",comnum,Redata_pt[0],Redata_pt[1]);
        return 0;
    }
    //////////////检查长度///////////////
    readlen=Redata_pt[1];			//获取长度
    if(readlen>(USART1_REC_LEN-4)) 	//长度超过最大值
    {
        printf("\r\n USART2_RxData_Pro接收数据长度=%d",readlen);
        return 0;
    }
//	//////////////检查校验和///////////////
//	datasum=0;
//    for(sxi=0; sxi<(readlen+4); sxi++)//因为readlen是数据区的长度，而总长度为readlen+4（不包括校验字）
//    {
//        datasum+=Redata_pt[sxi];		//累加和
//    }
//    //判断校验字节，即累加和是否一致
//     if(datasum!=Redata_pt[readlen+4]) //因为readlen是数据区的长度，而总长度为readlen+4（不包括校验字），故最后一个字节位置是：readlen+4
//    {
//        printf("\r\n 串口%d接收帧数据校验字错误,累加和=%d",comnum,datasum);
//        return 0;
//    }

    ////////根据帧命令字进行处理/////////
    switch(Redata_pt[3])
    {
	case Com_RdSenNodeList_All:					//读所有节点列表
        Printf_SenNodeList(1);
		break;
	case Com_RdSenNodeList_Black:				//读黑名单列表
        Printf_SenNodeList(2);
		break;
	case Com_RdSenNodeList_White:				//读白名单列表
        Printf_SenNodeList(3);
		break;
	case Com_RdSenNodeList_RegPass:				//读注册通过列表
        Printf_SenNodeList(4);
		break;
	case Com_RdSenNodeList_RegFail:				//读注册未通过列表
        Printf_SenNodeList(5);
		break;
	case Com_ClearNodeList_Save:				//清列表并保存
        SensorNodeList_Init();
		STMFLASH_Write_devicelist(STM32_SAVE_BASE,(uint32_t *)&SensorNode_List,sizeof(SensorNode_List));
		printf("\r\n Com_ClearNodeList_Save succeed!");
		break;
		
	case Com_RdSenNode_Agent:					//读某个传感器参数代理
        
		break;
	
	case Com_WrSenNode_Agent:					//写某个传感器参数
        
		break;
	case Com_ClearAllAgent:						//清除所有代理
//        SensorNodeList_Init();
		break;
	case Com_ResetAllWhList:					//重置所有白名单（不删除，只改名单属性为未分配）
//        SenIDList_AttribReset(SenAttri_WH);
		break;
	case Com_ResetAllBlList:					//重置所有黑名单（不删除，只改名单属性为未分配）
//        SenIDList_AttribReset(SenAttri_BL);
		break;	
    default:
//        printf("\r\n the cmdtype of comframe is error:%d;com is:%d!",Redata_pt[3],comnum);
        break;
    }
    return 1;
}



