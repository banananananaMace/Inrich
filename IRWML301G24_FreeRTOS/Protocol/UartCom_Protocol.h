#ifndef __UARTCOM_PROTOCOL_H
#define __UARTCOM_PROTOCOL_H

#include "sys.h"
#include "stdio.h"

#define	 COMFRAME_HEAD_FIRSTBYTE 	0xF1	//֡ͷ��һ���ֽ�
#define	 COMFRAME_HEAD_SECONDBYTE 	0xBA	//֡ͷ�ڶ����ֽ�


////////////����Э����֡��������///////////////////////////////
typedef  enum
{
    Com_RdSenNodeList_All=0x01,				//�����нڵ��б�
    Com_RdSenNodeList_Black,				//���������б�
    Com_RdSenNodeList_White,				//���������б�
    Com_RdSenNodeList_RegPass,				//��ע��ͨ���б�
    Com_RdSenNodeList_RegFail,				//��ע��δͨ���б�
	Com_ClearNodeList_Save,					//���б�����
	
	Com_RdSenNode_Agent=0x11,				//��ĳ����������������	
    Com_WrSenNode_Agent=0x21,				//дĳ������������
    Com_ClearAllAgent,						//������д���
    Com_ResetAllWhList,						//�������а���������ɾ����ֻ����������Ϊδ���䣩
    Com_ResetAllBlList,						//�������к���������ɾ����ֻ����������Ϊδ���䣩
	
} ComCMD_Type;

//extern uint16_t RegList[BASICREGLIST_LENGTH];
extern uint16_t USART1_SendFlag;	//0����ʾ����1���У�1-��ʾ�Ѿ����ͣ��ȴ���Ӧ���ȴ���Ӧ,ͬʱҲ��ʾ�ظ����ʹ�����
extern uint16_t USART2_SendFlag;	//0����ʾ����2���У�1-��ʾ�Ѿ����ͣ��ȴ���Ӧ���ȴ���Ӧ,ͬʱҲ��ʾ�ظ����ʹ�����
extern uint8_t ParaSendFlag;	//�������ñ�־������301���⣨203�����ò�����0-��ʾδ���ͣ�1-��ʾ�Ѿ����ͣ�2-��ʾ�Է����ճɹ�


extern uint8_t USART_RxData_Pro(uint8_t *Redata_pt,uint8_t comnum);
#endif
