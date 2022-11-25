//////////////////////////////////////////////////////////////////////////////////
//��������:2019/7/23
//�汾��V1.0
//Copyright(C) Inrich
//����������
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
*Description  	:΢���ʲ�����ŵ����յ����ݴ�����
*Input			:Redata_pt�����ջ�������
*Output			:0����ʾ�յ���Message֡��ʽ�����⣨���ô�����

*Question   	:1��
*******************************************************/
uint8_t  SenBS_CH1_DataPro(uint8_t *Redata_pt)
{
    uint8_t datasum,sxi;
    uint8_t readlen=0;
    uint8_t SenID[6];
    uint8_t Senattri,SenRegstat;

    //����ͷ
    if(((*Redata_pt)>>4)!=Sen_MESSAGE)
    {
        if(En_debugIP2)  printf("\r\n��һ�ֽ�=%d����������MESSAGE֡��",*Redata_pt);
        return 0;
    }
    //////////////���У���///////////////
    readlen=Redata_pt[1];			//��ȡMESSAGE��MACPayload����
    if((readlen>(SX_SENS_RECBUF_SIZE-9))||(readlen==0)) 	//���ȳ������ֵ
    {
//        printf("\r\nSenBS_CH1_DataPro�н������ݳ���=%d",readlen);
        return 0;
    }
    datasum=0;
    //for(sxi=0; sxi<(readlen+6); sxi++)//��Ϊreadlen��MACPayload�ĳ��ȣ����ܳ���ΪMACPayload�ĳ���readlen+7
    for(sxi=0; sxi<(readlen+8); sxi++)//��Ϊreadlen��MACPayload�ĳ��ȣ����ܳ���ΪMACPayload�ĳ���readlen+9
    {
        datasum+=Redata_pt[sxi];		//�ۼӺ�
    }
    //�ж�У���ֽڣ����ۼӺ��Ƿ�һ��
    //if(datasum!=Redata_pt[readlen+6]) //��Ϊreadlen��MACPayload�ĳ��ȣ����ܳ���ΪMACPayload�ĳ���+7�������һ���ֽ�λ���ǣ�readlen+6
    if(datasum!=Redata_pt[readlen+8]) //��Ϊreadlen��MACPayload�ĳ��ȣ����ܳ���ΪMACPayload�ĳ���+9�������һ���ֽ�λ���ǣ�readlen+8
    {
        if(En_debugIP2)
        {
            printf("\r΢���ʲ�ҵ���ŵ�����֡����У���ִ���,�ۼӺ�=%d \r",datasum);
            for(sxi=0; sxi<(readlen+9); sxi++)
                printf(" %02X",Redata_pt[sxi]);
        }
        return 0;
    }
    ////////////��鴫����ID�Ƿ����ں�����//////////////
    SenID[0]=Redata_pt[2];
    SenID[1]=Redata_pt[3];
    SenID[2]=Redata_pt[4];
    SenID[3]=Redata_pt[5];
    SenID[4]=Redata_pt[6];
    SenID[5]=Redata_pt[7];
	
    Senattri=GetSenID_Attri(SenID);			//ȡ�øô������ĺڰ�����
    SenRegstat=GetSenID_RegState(SenID);	//ȡ�øô�������ע��״̬
	/**����΢���ʴ�����������������ע�ᣬֻ�����ɽ��յ�ҵ�����ݺ�Ŵ���ע�ᣬ�ʶ�΢������˵���յ�ҵ�����ݣ�
	��1�������豸�б��У����б�����δ��������ע�᣻
	��2�����豸�б���Ϊ���������򲻴���
	��3�����豸�б��зǺ���ע�ᣬ�������ݣ�
	��4�����豸�б��зǺ���ע��ʧ�ܣ���鿴�Ƿ�ʱ�������ʱ�����������ע�ᣬ���򲻴���
	��5�����豸�б��зǺ���δ������ע�᣻
	**/
    if(Senattri==SenAttri_BL) 	//����2�����ں�������������
        return 1;
    if(Senattri==0xff) 		//�������豸������(1)����������豸������ע��
        return 2;

    //��������Ϊδ������߰�����
    if(SenRegstat==SenRegSt_UnReg)		//����5���������У�����Ϊ���Ǻڣ���δע�ᣬ����ע��
        return 3;
    if(SenRegstat==SenRegSt_Success) 	//����3���������У�����Ϊ���Ǻڣ���ע��ɹ������������ϴ�
        return 4;
    if(SenRegstat==SenRegSt_Fail) 		//����4���������У�����Ϊ���Ǻڣ���ע�᲻ͨ����������
        return 5;
	return 6;
}



