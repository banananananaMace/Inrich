#ifndef __UARTCOM_PROTOCOL_H
#define __UARTCOM_PROTOCOL_H

#include "sys.h"
#include "stdio.h"

#define	 COMFRAME_HEAD_FIRSTBYTE 	0xF1	//帧头第一个字节
#define	 COMFRAME_HEAD_SECONDBYTE 	0xBA	//帧头第二个字节


////////////串口协议中帧命令类型///////////////////////////////
typedef  enum
{
    Com_RdSenNodeList_All=0x01,				//读所有节点列表
    Com_RdSenNodeList_Black,				//读黑名单列表
    Com_RdSenNodeList_White,				//读白名单列表
    Com_RdSenNodeList_RegPass,				//读注册通过列表
    Com_RdSenNodeList_RegFail,				//读注册未通过列表
	Com_ClearNodeList_Save,					//清列表并保存
	
	Com_RdSenNode_Agent=0x11,				//读某个传感器参数代理	
    Com_WrSenNode_Agent=0x21,				//写某个传感器参数
    Com_ClearAllAgent,						//清除所有代理
    Com_ResetAllWhList,						//重置所有白名单（不删除，只改名单属性为未分配）
    Com_ResetAllBlList,						//重置所有黑名单（不删除，只改名单属性为未分配）
	
} ComCMD_Type;

//extern uint16_t RegList[BASICREGLIST_LENGTH];
extern uint16_t USART1_SendFlag;	//0：表示串口1空闲；1-表示已经发送，等待回应，等待回应,同时也表示重复发送次数；
extern uint16_t USART2_SendFlag;	//0：表示串口2空闲；1-表示已经发送，等待回应，等待回应,同时也表示重复发送次数；
extern uint8_t ParaSendFlag;	//参数设置标志，用于301对外（203）设置参数：0-表示未发送；1-表示已经发送；2-表示对方接收成功


extern uint8_t USART_RxData_Pro(uint8_t *Redata_pt,uint8_t comnum);
#endif
