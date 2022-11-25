//////////////////////////////////////////////////////////////////////////////////
//��������:2019/8/16
//�汾��V1.0
//Copyright(C) Inrich
//����������
//1������ͨ��Э�����ú��������
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

extern QueueHandle_t Uart1Tx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
extern QueueHandle_t Uart2Tx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
extern QueueHandle_t Uart4Tx_MessageData_Q;   				//����4�������ݵ���Ϣ���о��


///////////�Ĵ����б�������Ϣ/////////////
//RegList[0]:����
//RegList[1]:Ӳ���汾��
//RegList[2]:����汾��
//RegList[3]~[6]:���кţ���λ��ǰ
//RegList[BASICREGLIST_LENGTH-2]:����������
//RegList[BASICREGLIST_LENGTH-1]:����������
//uint16_t RegList[BASICREGLIST_LENGTH]= {0xA92E,0x0001,0x0002,0x0003,0x0004,0x0005};
uint16_t USART1_SendFlag=0;	//0����ʾ����1���У�1-��ʾ�Ѿ����ͣ��ȴ���Ӧ,ͬʱҲ��ʾ�ظ����ʹ�����
uint16_t USART2_SendFlag=0;	//0����ʾ����2���У�1-��ʾ�Ѿ����ͣ��ȴ���Ӧ,ͬʱҲ��ʾ�ظ����ʹ�����
uint8_t ParaSendFlag=0;		//�������ñ�־������301���⣨203�����ò�����0-��ʾδ���ͣ�1-��ʾ�Ѿ����ͣ�2-��ʾ�Է����ճɹ�

/*******************************************************
*Function Name	:USART2_Rx_Task
*Description  	:���ڽ������ݴ������񣨣�
*Input		  	:comnum:COM2-��ʾusart2��COM4-��ʾusart4
*Output		  	:
*Question	  	:1��
*******************************************************/
uint8_t USART_RxData_Pro(uint8_t *Redata_pt,uint8_t comnum)
{
//    uint8_t sxi;//,datasum;
//    uint16_t readadd=0;
//    uint16_t ParaFlag=0;
    uint16_t readlen=0;
    //�ж�֡ͷ�Ƿ���ȷ
    if((Redata_pt[0]!=COMFRAME_HEAD_FIRSTBYTE)||(Redata_pt[1]!=COMFRAME_HEAD_SECONDBYTE))
    {
        printf("\r\n the headword of comframe is error:comnum=%02X,firsthead=%02X,secondhead=%02X!",comnum,Redata_pt[0],Redata_pt[1]);
        return 0;
    }
    //////////////��鳤��///////////////
    readlen=Redata_pt[1];			//��ȡ����
    if(readlen>(USART1_REC_LEN-4)) 	//���ȳ������ֵ
    {
        printf("\r\n USART2_RxData_Pro�������ݳ���=%d",readlen);
        return 0;
    }
//	//////////////���У���///////////////
//	datasum=0;
//    for(sxi=0; sxi<(readlen+4); sxi++)//��Ϊreadlen���������ĳ��ȣ����ܳ���Ϊreadlen+4��������У���֣�
//    {
//        datasum+=Redata_pt[sxi];		//�ۼӺ�
//    }
//    //�ж�У���ֽڣ����ۼӺ��Ƿ�һ��
//     if(datasum!=Redata_pt[readlen+4]) //��Ϊreadlen���������ĳ��ȣ����ܳ���Ϊreadlen+4��������У���֣��������һ���ֽ�λ���ǣ�readlen+4
//    {
//        printf("\r\n ����%d����֡����У���ִ���,�ۼӺ�=%d",comnum,datasum);
//        return 0;
//    }

    ////////����֡�����ֽ��д���/////////
    switch(Redata_pt[3])
    {
	case Com_RdSenNodeList_All:					//�����нڵ��б�
        Printf_SenNodeList(1);
		break;
	case Com_RdSenNodeList_Black:				//���������б�
        Printf_SenNodeList(2);
		break;
	case Com_RdSenNodeList_White:				//���������б�
        Printf_SenNodeList(3);
		break;
	case Com_RdSenNodeList_RegPass:				//��ע��ͨ���б�
        Printf_SenNodeList(4);
		break;
	case Com_RdSenNodeList_RegFail:				//��ע��δͨ���б�
        Printf_SenNodeList(5);
		break;
	case Com_ClearNodeList_Save:				//���б�����
        SensorNodeList_Init();
		STMFLASH_Write_devicelist(STM32_SAVE_BASE,(uint32_t *)&SensorNode_List,sizeof(SensorNode_List));
		printf("\r\n Com_ClearNodeList_Save succeed!");
		break;
		
	case Com_RdSenNode_Agent:					//��ĳ����������������
        
		break;
	
	case Com_WrSenNode_Agent:					//дĳ������������
        
		break;
	case Com_ClearAllAgent:						//������д���
//        SensorNodeList_Init();
		break;
	case Com_ResetAllWhList:					//�������а���������ɾ����ֻ����������Ϊδ���䣩
//        SenIDList_AttribReset(SenAttri_WH);
		break;
	case Com_ResetAllBlList:					//�������к���������ɾ����ֻ����������Ϊδ���䣩
//        SenIDList_AttribReset(SenAttri_BL);
		break;	
    default:
//        printf("\r\n the cmdtype of comframe is error:%d;com is:%d!",Redata_pt[3],comnum);
        break;
    }
    return 1;
}



