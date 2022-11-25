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
#include "SenCTR_CH_Task.h"

extern bool En_debugIP2 ;
/*******************************************************
*Function Name	:SenCTR_CH_DataPro
*Description  	:΢���ʲ�����ŵ����յ����ݴ�����
*Input			:Redata_pt�����ջ�������
*Output			:0����ʾ�յ���Control_Dow֡��ʽ�����⣬�����Ǻ�������
				 0x10����ʾ�յ�REQ֡�����п��Ʋ�������
				 0x11����ʾ�յ�REQ֡��ͨ��ָ���������
				 0x12����ʾ�յ�REQ֡��ҵ����Ʋ�������
				 0x02����ʾ�յ��澯����
				 0x03����ʾ�յ�REQ_ACK
*Question   	:1��
*******************************************************/
uint8_t  SenCTR_CH_DataPro(uint8_t *Redata_pt)
{
    uint8_t datasum,sxi;
    uint8_t readlen=0;
    //uint16_t sennum;
    uint8_t SenID[6];
    //////////////���У���///////////////
    readlen=Redata_pt[1];			//��ȡMESSAGE��MACPayload����    
	if((readlen>(SX_SENS_RECBUF_SIZE-9))||(readlen==0)) 	//���ȳ������ֵ
    {
//        printf("\r\nSenCTR_CH_DataPro�н������ݳ���=%d",readlen);
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
			printf("\r΢���ʲ�����ŵ�����֡����У���ִ���,�ۼӺ�=%d\r",datasum);
			for(sxi=0;sxi<(readlen+9);sxi++)
				printf(" %02X",Redata_pt[sxi]);   
		}			
        return 0;
    }
    //ȡ�ø���Ϣ�еĴ�����ID
    //SenID=((uint32_t)Redata_pt[2]<<24)+((uint32_t)Redata_pt[3]<<16)+((uint32_t)Redata_pt[4]<<8)+Redata_pt[5];	//ȡ�ô�����ID
    SenID[0]=Redata_pt[2];
    SenID[1]=Redata_pt[3];
    SenID[2]=Redata_pt[4];
    SenID[3]=Redata_pt[5];
    SenID[4]=Redata_pt[6];
    SenID[5]=Redata_pt[7];
    ////////////��鴫����ID�Ƿ����ں�����//////////////
    if(CheckID_isBL(SenID)!=0xffff)  //������ں������У��򲻻�Ӧ
    {
        if(En_debugIP2)  printf("\r\n ΢���ʲ�����ŵ�����֡ID�ں�������");
        return 0;
    }

    ////////���ݲ�ͬ��ͷ���ͽ��д���////////////////
    datasum=0;
    switch((*Redata_pt)>>4)
    {
    case Sen_REQ:	//�����ݲ�����������������ȥ�����漰��Ϣ���еĲ�����
//        printf("\r\n���յ�REQ��������:\r\n");
//        for(sxi=0; sxi<(readlen+7); sxi++) //�ܳ���ΪMACPayload�ĳ���readlen+7
//            printf(" %d",Redata_pt[sxi]);

        //RSP_Send(SenID,15,Sensor_MESSAGE_CYCLE);		//�ظ�RSP_END֡,����ҵ������
        //delay_xms(40);
        //RSP_Send(SenID,15,Sensor_CONTROL_CYCLE);		//�ظ�RSP_END֡,���ÿ�������
        //delay_xms(40);
        //RSPEND_Send(SenID,15,Sensor_FOUR_PARAMETER);		//�ظ�RSP_END֡,����4������
        //RSPEND_Send(SenID,15,Sensor_MESSAGE_CYCLE);		//�ظ�RSP_END֡,����ҵ������
        //RSPEND_Send(SenID,15,Sensor_CONTROL_CYCLE);		//�ظ�RSP_END֡,���ÿ�������
        //RSPEND_Send(SenID,15,Sensor_DELAYTIME);			  //�ظ�RSP_END֡,������ʱʱ��
		
        switch(Redata_pt[8])
        {
        case 0:		//���п��Ʋ�����
            datasum=0x10;
            break;
        case 1:		//ͨ��ָ�������
            datasum=0x11;
            break;
        case 2:		//ҵ����Ʋ�����
            datasum=0x12;
            break;
        default:
            if(En_debugIP2)  
			{
				printf("\r\n REQ����������������=%d��",Redata_pt[8]);
				for(sxi=0;sxi<(readlen+9);sxi++)
				 printf(" %02X",Redata_pt[sxi]);
			}
            datasum=0; //����������������
            break;
        }
        break;
    case Sen_BURST:	//�澯����
//        printf("\r\n���յ�BURST��������:\r\n");
//        for(sxi=0; sxi<(readlen+9); sxi++) //�ܳ���ΪMACPayload�ĳ���readlen+7
//            printf(" %d",Redata_pt[sxi]);
//        //BURST_ACK_Send(SenID);
        datasum=0x02;
        break;
    case Sen_ACK:	//��ACK��ָ���������յ�RSP_END���͵�RSP_END_ACK
//        printf("\r\n���յ�RSP_END_ACK��������:\r\n");
//        for(sxi=0; sxi<(readlen+9); sxi++) //�ܳ���ΪMACPayload�ĳ���readlen+7
//            printf(" %d",Redata_pt[sxi]);
        datasum=0x03;
        break;
    default :
//        printf("\r\n��һ�ֽ�=%d����������REQ��BURST��ACK֡��",*Redata_pt);
//		for(sxi=0;sxi<(readlen+9);sxi++)
//			printf(" %02X",Redata_pt[sxi]);
        datasum=0;
        break;
    }
    return datasum;
}



/*******************************************************
*Function Name	:BURST_ACK_Send
*Description 	:������ڿ����ŵ����յ�BURST֡����ظ�BURST_ACK
*Input		  	:SenID��������ID��
*Output		  	:
Question	  	:1��
*******************************************************/
void  BURST_ACK_Send(uint8_t *SenID)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0; //8+1+2=11//
    //MHDR��Ϊ����㸺��֡ͷ,8���ֽ�
    SX_Sens_SndBuf[0][ptbuf++]=Sen_ACK<<4;			//RSP_END֡����3���޼���
    SX_Sens_SndBuf[0][ptbuf++]=0;					//�ݶ�0���󲹣�payload�ĳ������ܳ���length��ȥ��ͷ��6�ֽں�MIC��1�ֽ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[0];			//������ID�ĵ�1���ֽڣ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[1];			//������ID�ĵ�2���ֽڣ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[2];			//������ID�ĵ�3���ֽڣ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[3];			//������ID�ĵ�4���ֽڣ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[4];			//������ID�ĵ�5���ֽڣ�
    SX_Sens_SndBuf[0][ptbuf++]=SenID[5];			//������ID�ĵ�6���ֽڣ�

    //MACPayload:��2���ֽ�
    SX_Sens_SndBuf[0][ptbuf++]=0x13;  					//������������1����Ƭָʾ��0��֡���ͣ�3 ��������λΪ��0001 0 011
    //pti=0xFFFC;											//�������ͣ�3FF�����ݳ���ָʾ��00
    SX_Sens_SndBuf[0][ptbuf++]=0xff;

//    SX_Sens_SndBuf[0][ptbuf++]=Sen_BURST_ACK;		//��ʾ�澯Ӧ��
//    SX_Sens_SndBuf[0][ptbuf++]=1;					//��ʾͻ��ȷ��

    //CRCУ��λ
    ptlength=ptbuf-2;
    pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
    SX_Sens_SndBuf[0][ptbuf++]=pti>>8;
    SX_Sens_SndBuf[0][ptbuf++]=pti;

    //MIC
    SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
    ptlength=ptbuf;									//�õ�֡���ܳ���
    SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
    //�����������ݵ��ۼӺ�
    for(pti=0; pti<(ptlength-1); pti++)
    {
        SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
    }
    SX1280_Data_Send(0,ptlength);					//���������ŵ����ݷ���
}

/*******************************************************
*Function Name	:SenCTR_ComPara_REQRSP
*Description 	:�������յ�REQ֡������ͨ�Ų���
*Input		  	:SenID_Serial���ô������ڰ������б��е���ţ�
				 Rspend_type��0-����RSP_END��1-RSP
*Output		  	:
Question	  	:1��ҵ��ʱ϶��ʱ�䳤���ݶ�200ms
				 2��ȡ�õ�ǰʱ���ʱ����ȥʱ���ݶ�30MS��
*******************************************************/
bool  SenCTR_ComPara_REQRSP(uint16_t SenID_Serial,u8 Rsp_type)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0;
//    uint32_t senslot_lenth;		//ʱ϶���ȣ�ms��

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

    bs_cycle=SensorNode_List.BS_Cycle[SenID_Serial];  			//ȡ�ô�������Ӧ��ҵ�����ڣ�msΪ��λ
    req_cycle=SensorNode_List.REQ_Cycle[SenID_Serial];  		//ȡ�ô�������Ӧ�Ŀ�������
    shocktimelength=SensorNode_List.ShockTime[SenID_Serial];	//ȡ�ô�������Ӧ���Ŷ�ʱ������5msΪ��λ
	DelayTime=SensorNode_List.DelayTime[SenID_Serial];			//ȡ�ô�������Ӧ����ʱʱ������5msΪ��λ
//  bsfre=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  		//ȡ�ô�������Ӧ��ҵ��Ƶ��
//  assignslot_order=SensorNode_List.BSCH_Fre[SenID_Serial];  	//ȡ�ô�������Ӧ��ʱ϶���
	
//   senslot_lenth=200; 	//ȡ�ô�������Ӧ��ʱ϶����ms

//    limitsen[0]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][0];//����0
//    limitsen[1]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][1];//����1
//    limitsen[2]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][2];//����2
//    limitsen[3]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][3];//����3
//    limittype[0]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][0];
//    limittype[1]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][1];
//    backup = WhiteSensorNode_List.WhID_ParaBkUp[SenID_Serial];		//����

//    //���ʱ϶��Ŵ������ֵ,��ȡ����
//    if(assignslot_order>((bs_cycle)/senslot_lenth))
//        assignslot_order=0;	//assignslot_order=assignslot_order%((bs_cycle)/senslot_lenth);

    ptbuf=0;
    ptlength=0;			//
    //MHDR��Ϊ����㸺��֡ͷ
    if(Rsp_type==0)
        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP_END<<4;		//RSP_END֡����3��ͨ��ָ��ָʾ0����Ϊͨ�Ų����� ���޼���
    else
        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP<<4;			//Sen_RSP����2��ͨ��ָ��ָʾ0����Ϊͨ�Ų����� ���޼���
    SX_Sens_SndBuf[0][ptbuf++]=0;					//�ݶ�0�������ٲ���payload�ĳ������ܳ���length��ȥ��ͷ��6�ֽں�MIC��1�ֽ�
	memcpy(&SX_Sens_SndBuf[0][ptbuf],SensorNode_List.Sensor_ID[SenID_Serial],6);	//������ID
	ptbuf=ptbuf+6;
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][0];			//������ID�ĵ�1���ֽڣ������8λ
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][1];			//������ID�ĵ�2���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][2];			//������ID�ĵ�3���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][3];			//������ID�ĵ�4���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][4];			//������ID�ĵ�5���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][5];			//������ID�ĵ�6���ֽڣ������8λ
    //ͨ��ָ�����Լ������Э��涨
//    if((bs_cycle!=0)&&(req_cycle!=0)&&(DelayTime!=0)&&(shocktimelength!=0)) //4����������Ϊ0������Խ�����ϲ����·�
    if((bs_cycle!=0)&&(req_cycle!=0)&&(DelayTime!=0)&&(shocktimelength!=0)) //4����������Ϊ0������Խ�����ϲ����·�
	{
        SX_Sens_SndBuf[0][ptbuf++]=Sensor_FOUR_PARAMETER;	//������Ϣ���ͣ�ǰ�ĸ����
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.BS_Cycle[SenID_Serial],4);  //ҵ������4���ֽ�
		ptbuf=ptbuf+4;
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.REQ_Cycle[SenID_Serial],2);  //��������2���ֽ�
		ptbuf=ptbuf+2;
		memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.DelayTime[SenID_Serial],4);  //��ʱʱ��4���ֽ�
		ptbuf=ptbuf+4;
		SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.ShockTime[SenID_Serial];			//���ʱ���𵴷�Χ����5msΪʱ�䵥λ��
		
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//ҵ�����ڵ�4���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//ҵ�����ڵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//ҵ�����ڵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//ҵ�����ڵ�1���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//�������ڵ�2���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//�������ڵ�1���ֽڣ�
        //TimeStamp=1000*9;	//22�룬���ڲ���
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime;				//��ʱ���ڵ�4���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;			//��ʱ���ڵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;			//��ʱ���ڵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;			//��ʱ��1���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=shocktimelength;			//���ʱ���𵴷�Χ����5msΪʱ�䵥λ��
    }
    else
    {
        if(bs_cycle!=0)	//����ҵ������
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_MESSAGE_CYCLE;	//������Ϣ���ͣ�ҵ������
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.BS_Cycle[SenID_Serial],4);  //ҵ������4���ֽ�
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//ҵ�����ڵ�4���ֽڣ������8λ
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//ҵ�����ڵ�3���ֽڣ�
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//ҵ�����ڵ�2���ֽڣ�
//            SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//ҵ�����ڵ�1���ֽڣ������8λ
        }
        if(req_cycle!=0)//��������
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_CONTROL_CYCLE;	//������Ϣ���ͣ�������������
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.REQ_Cycle[SenID_Serial],2);  //��������2���ֽ�
			ptbuf=ptbuf+2;
//            SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//�������ڵ�4���ֽڣ������8λ
//            SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//�������ڵ�3���ֽڣ�
        }
        if(DelayTime!=0)//��ʱʱ�䣬/ʱ϶������1��ʼ
        {            
            //TimeStamp=1000*9;	//22�룬���ڲ���
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_DELAYTIME;			//������Ϣ���ͣ���ʱʱ��
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.DelayTime[SenID_Serial],4);  //��ʱʱ��4���ֽ�
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime;					//��ʱ���ڵ�4���ֽڣ������8λ
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;				//��ʱ���ڵ�3���ֽڣ�
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;				//��ʱ���ڵ�2���ֽڣ�
//            SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;				//��ʱ��1���ֽڣ������8λ
        }
        if(shocktimelength!=0)//��ʱ��
        {
            SX_Sens_SndBuf[0][ptbuf++]=Sensor_SHOCKTIME;						//������Ϣ���ͣ���ʱ��
            SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.ShockTime[SenID_Serial];	//���ʱ���𵴷�Χ����5msΪʱ�䵥λ��
        }
    }
//    if(ptbuf>8)	//��ʾ�в�����Ҫ����
    {
        //MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
        ptlength=ptbuf;									//�õ�֡���ܳ���
        SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
        for(pti=0; pti<(ptlength-1); pti++)			//�����������ݵ��ۼӺ�
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);				//��������REQ�ŵ����ݷ���
        return 1;	//��ʾ
    }
}

/*******************************************************
*Function Name	:SenCTR_CtrPara_REQRSP
*Description 	:�������յ�REQ֡�����ÿ��Ʋ���
*Input		  	:SenID_Serial���ô������ڰ������б��е���ţ�
				 Rspend_type��0-����RSP_END��1-RSP
*Output		  	:
Question	  	:1��ҵ��ʱ϶��ʱ�䳤���ݶ�200ms
				 2��ȡ�õ�ǰʱ���ʱ����ȥʱ���ݶ�30MS��
*******************************************************/
bool  SenCTR_CtrPara_REQRSP(uint16_t SenID_Serial,u8 Rsp_type)
{
    uint8_t ptbuf=0;
    uint16_t pti;
    uint8_t ptlength=0;
//    uint32_t limitsen[4];
    uint16_t limittype[2];

//    limitsen[0]=SensorNode_List.SenID_SenLimit[SenID_Serial][0];//����0
//    limitsen[1]=SensorNode_List.SenID_SenLimit[SenID_Serial][1];//����1
//    limitsen[2]=SensorNode_List.SenID_SenLimit[SenID_Serial][2];//����2
//    limitsen[3]=SensorNode_List.SenID_SenLimit[SenID_Serial][3];//����3
    limittype[0]=SensorNode_List.SenID_LimitType[SenID_Serial][0];
    limittype[1]=SensorNode_List.SenID_LimitType[SenID_Serial][1];

    ptbuf=0;
    ptlength=0;			//
    //MHDR��Ϊ����㸺��֡ͷ
	//SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;										//RSP_END֡����3��ͨ��ָ��ָʾ1����Ϊ���Ʋ����� ���޼���
    if(Rsp_type==0)
        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;		//RSP_END֡����3��ͨ��ָ��ָʾ1����Ϊ���Ʋ����� ���޼���
    else
        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP<<4)|0x08;			//Sen_RSP����2�� ͨ��ָ��ָʾ1����Ϊ���Ʋ����� ���޼���
    SX_Sens_SndBuf[0][ptbuf++]=0;					//�ݶ�0�������ٲ���payload�ĳ������ܳ���length��ȥ��ͷ��6�ֽں�MIC��1�ֽ�
	
	memcpy(&SX_Sens_SndBuf[0][ptbuf],SensorNode_List.Sensor_ID[SenID_Serial],6);	//������ID
	ptbuf=ptbuf+6;
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][0];			//������ID�ĵ�1���ֽڣ������8λ
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][1];			//������ID�ĵ�2���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][2];			//������ID�ĵ�3���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][3];			//������ID�ĵ�4���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][4];			//������ID�ĵ�5���ֽڣ�
//    SX_Sens_SndBuf[0][ptbuf++]=SensorNode_List.Sensor_ID[SenID_Serial][5];			//������ID�ĵ�6���ֽڣ������8λ

    //        //MACPayload:�����ֽ�
    //�жϵ�ǰ�������õ���ֵ����m
    pti=0;
    if(limittype[0]!=0)
        pti++;
    if(limittype[1]!=0)
        pti++;
    if(pti!=0)	//����ֵ�������ã�����ֵ��������0
    {
        SX_Sens_SndBuf[0][ptbuf++]=(pti<<4)|0x04;  			//������������pti����Ƭָʾ��0��֡���ͣ�4�����Ʊ��ģ� ��������λΪ��0001 0 100
        SX_Sens_SndBuf[0][ptbuf++]=(5<<1)|0x01;				//���Ʊ�������:5���������ñ�ʶ��1�����ã�
        //�����б�
        if(limittype[0]!=0)
        {
            SX_Sens_SndBuf[0][ptbuf++]=(limittype[0]<<2)|0x00;		//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
            SX_Sens_SndBuf[0][ptbuf++]=((limittype[0]<<2)|0x00)>>8;	//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][0],4);  //����,4�ֽ�
			ptbuf=ptbuf+4;
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][1],4);  //����,4�ֽ�
			ptbuf=ptbuf+4;
			
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0];			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>8;			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>16;		//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>24;		//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1];			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>8;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>16;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>24;			//
        }
        if(limittype[1]!=0)
        {
            SX_Sens_SndBuf[0][ptbuf++]=(limittype[1]<<2)|0x00;		//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
            SX_Sens_SndBuf[0][ptbuf++]=((limittype[1]<<2)|0x00)>>8;	//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][2],4);  //����,4�ֽ�
			ptbuf=ptbuf+4;
			memcpy(&SX_Sens_SndBuf[0][ptbuf],&SensorNode_List.SenID_SenLimit[SenID_Serial][3],4);  //����,4�ֽ�
			ptbuf=ptbuf+4;
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2];			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>8;			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>16;		//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>24;		//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3];			//����
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>8;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>16;			//
//            SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>24;			//
        }
        //CRCУ��λ
        ptlength=ptbuf-2;
        pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
        SX_Sens_SndBuf[0][ptbuf++]=pti>>8;
        SX_Sens_SndBuf[0][ptbuf++]=pti;

        //MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
        ptlength=ptbuf;									//�õ�֡���ܳ���
        SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
        for(pti=0; pti<(ptlength-1); pti++)				//�����������ݵ��ۼӺ�
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);					//��������REQ�ŵ����ݷ���
        return 1;
    }
	else
	{
		//MIC
        SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
        ptlength=ptbuf;									//�õ�֡���ܳ���
        SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
        for(pti=0; pti<(ptlength-1); pti++)				//�����������ݵ��ۼӺ�
        {
            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
        }
        SX1280_Data_Send(0,ptlength);					//��������REQ�ŵ����ݷ���
		return 0;
	}    
}

///*******************************************************
//*Function Name	:RSPEND_Send
//*Description 	:�������յ�REQ֡����ظ�RSP_END���ͺ���
//*Input		  	:SenID_Serial���ô������ڰ������б��е���ţ�
//				 Rspend_type
//*Output		  	:
//Question	  	:1��ҵ��ʱ϶��ʱ�䳤���ݶ�200ms
//				 2��ȡ�õ�ǰʱ���ʱ����ȥʱ���ݶ�30MS��
//*******************************************************/
//void  SenCTR_RSPEND_Send(uint16_t SenID_Serial,u8 Rspend_type)
//{
//    uint8_t ptbuf=0;
//    uint16_t pti;
//    uint8_t ptlength=0;
//    uint32_t senslot_lenth;		//ʱ϶���ȣ�ms��

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

//    bs_cycle=WhiteSensorNode_List.BS_Cycle[SenID_Serial];  			//ȡ�ô�������Ӧ��ҵ�����ڣ�msΪ��λ
//    req_cycle=WhiteSensorNode_List.REQ_Cycle[SenID_Serial];  		//ȡ�ô�������Ӧ�Ŀ�������
////    bsfre=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  			//ȡ�ô�������Ӧ��ҵ��Ƶ��
//    assignslot_order=WhiteSensorNode_List.BSCH_Fre[SenID_Serial];  	//ȡ�ô�������Ӧ��ʱ϶���

//    //senslot_lenth=WhiteSensorNode_List.WhID_SlotLen[SenID_Serial]; 	//ȡ�ô�������Ӧ��ʱ϶����
//    senslot_lenth=200; 	//ȡ�ô�������Ӧ��ʱ϶����ms

//    shocktimelength=WhiteSensorNode_List.ShockTime[SenID_Serial];//ȡ�ô�������Ӧ���Ŷ�ʱ������5msΪ��λ
//    limitsen[0]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][0];//����0
//    limitsen[1]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][1];//����1
//    limitsen[2]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][2];//����2
//    limitsen[3]=WhiteSensorNode_List.WhID_SenLimit[SenID_Serial][3];//����3
//    limittype[0]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][0];
//    limittype[1]=WhiteSensorNode_List.WhID_LimitType[SenID_Serial][1];
////    backup = WhiteSensorNode_List.WhID_ParaBkUp[SenID_Serial];		//����

////    //����ҵ�����ڳ��ȵ���СֵΪ��1s��
////    if(bs_cycle<1000)
////        bs_cycle=10000;

////    //���ƿ������ڳ��ȵ���СֵΪ��5��ҵ������
////    if(req_cycle<5)
////        req_cycle=5;

//    //���ʱ϶��Ŵ������ֵ,��ȡ����
//    if(assignslot_order>((bs_cycle)/senslot_lenth))
//        assignslot_order=assignslot_order%((bs_cycle)/senslot_lenth);

//    switch(Rspend_type)
//    {
//    case Sensor_FOUR_PARAMETER:
//        ptbuf=0;
//        ptlength=0;			//
//        //MHDR��Ϊ����㸺��֡ͷ
//        SX_Sens_SndBuf[0][ptbuf++]=Sen_RSP_END<<4;		//RSP_END֡����3��ͨ��ָ��ָʾ0����Ϊͨ�Ų����� ���޼���
//        SX_Sens_SndBuf[0][ptbuf++]=0;					//�ݶ�0�������ٲ���payload�ĳ������ܳ���length��ȥ��ͷ��6�ֽں�MIC��1�ֽ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][0];			//������ID�ĵ�1���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][1];			//������ID�ĵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][2];			//������ID�ĵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][3];			//������ID�ĵ�4���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][4];			//������ID�ĵ�5���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][5];			//������ID�ĵ�6���ֽڣ������8λ

////        //MACPayload:�����ֽ�
////        SX_Sens_SndBuf[0][ptbuf++]=0x15;  					//������������1����Ƭָʾ��0��֡���ͣ�5��������Ӧ���ģ� ��������λΪ��0001 0 101
////        //pti=0xFFFC;											//�������ͣ�3FF�����ݳ���ָʾ��00
////        SX_Sens_SndBuf[0][ptbuf++]=0xff;
////        SX_Sens_SndBuf[0][ptbuf++]=0xfc;
//        //ͨ��ָ�����Լ������Э��涨
//        SX_Sens_SndBuf[0][ptbuf++]=0xFF;					//������Ϣ���ͣ�ǰ�ĸ����

//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle;				//ҵ�����ڵ�4���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>8;				//ҵ�����ڵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>16;			//ҵ�����ڵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=bs_cycle>>24;			//ҵ�����ڵ�1���ֽڣ������8λ

//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle;				//�������ڵ�4���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>8;			//�������ڵ�3���ֽڣ�
////        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>16;			//�������ڵ�2���ֽڣ�
////        SX_Sens_SndBuf[0][ptbuf++]=req_cycle>>24;			//�������ڵ�1���ֽڣ������8λ

//        //������ʱ����Ҫ��ʱ϶��ţ�ҵ�����ڣ�ʱ϶����3������
//        if(assignslot_order!=0)	//ʱ϶������1��ʼ
//        {
//            DelayTime=SelfNode_TimeStamp-15;					//ȡ�õ�ǰʱ϶,����15ms�ݶ���ʱ�����ֵ������1280�Ĵ���ʱ���Լ��������Ķ�ʱ���
//            curslot_order=((DelayTime%(bs_cycle))/senslot_lenth)+1;	//�������ǰʱ�䴦���ĸ�ҵ��ʱ϶�Σ�����ҵ��ʱ϶���ݶ�200ms��ʱ϶���Ǵӵ�1�ο�ʼ��
//            if(assignslot_order>curslot_order) 	//�����ǰ���ڹ㲥ʱ϶���ڷ���Ĺ㲥ʱ϶��֮ǰ
//            {
//                curslot_order=assignslot_order-curslot_order; 	//������
//                DelayTime=((uint32_t)(curslot_order))*senslot_lenth;  //������ʱʱ��
//            }
//            else if(assignslot_order<curslot_order) //�����ǰ���ڹ㲥ʱ϶���ڷ���Ĺ㲥ʱ϶��֮��
//            {
//                curslot_order=bs_cycle/senslot_lenth-(curslot_order-assignslot_order);  //  SENSOR_SLOT_ORDER_MAX; 	//������
//                DelayTime=((uint32_t)(curslot_order))*senslot_lenth;  //������ʱʱ��
//            }
//            else
//                DelayTime=0;
//        }
//        else
//            DelayTime=0;

//        //TimeStamp=1000*9;	//22�룬���ڲ���
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime;					//��ʱ���ڵ�4���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>8;				//��ʱ���ڵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>16;				//��ʱ���ڵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=DelayTime>>24;				//��ʱ��1���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=shocktimelength;				//���ʱ���𵴷�Χ����5msΪʱ�䵥λ��

////        //CRCУ��λ
////        ptlength=ptbuf-2;
////        pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
////        SX_Sens_SndBuf[0][ptbuf++]=pti;
////        SX_Sens_SndBuf[0][ptbuf++]=pti>>8;

//        //MIC
//        SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
//        ptlength=ptbuf;									//�õ�֡���ܳ���
//        SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
//        for(pti=0; pti<(ptlength-1); pti++)			//�����������ݵ��ۼӺ�
//        {
//            SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
//        }
//        SX1280_Data_Send(0,ptlength);				//��������REQ�ŵ����ݷ���
//        break;
//    case Sensor_Control_PARAMETER:	//���ÿ��Ʋ�������Ҫ�������ޣ�
//        ptbuf=0;
//        ptlength=0;			//
//        //MHDR��Ϊ����㸺��֡ͷ
//        SX_Sens_SndBuf[0][ptbuf++]=(Sen_RSP_END<<4)|0x08;										//RSP_END֡����3��ͨ��ָ��ָʾ1����Ϊ���Ʋ����� ���޼���
//        SX_Sens_SndBuf[0][ptbuf++]=0;															//�ݶ�0�������ٲ���payload�ĳ������ܳ���length��ȥ��ͷ��6�ֽں�MIC��1�ֽ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][0];			//������ID�ĵ�1���ֽڣ������8λ
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][1];			//������ID�ĵ�2���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][2];			//������ID�ĵ�3���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][3];			//������ID�ĵ�4���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][4];			//������ID�ĵ�5���ֽڣ�
//        SX_Sens_SndBuf[0][ptbuf++]=WhiteSensorNode_List.WhiteIDList[SenID_Serial][5];			//������ID�ĵ�6���ֽڣ������8λ

//        //MACPayload:�����ֽ�
//        //�жϵ�ǰ�������õ���ֵ����m
//        pti=0;
//        if(limittype[0]!=0)
//            pti++;
//        if(limittype[1]!=0)
//            pti++;
//        if(pti!=0)	//����ֵ�������ã�����ֵ��������0
//        {
//            SX_Sens_SndBuf[0][ptbuf++]=(pti<<4)|0x04;  			//������������pti����Ƭָʾ��0��֡���ͣ�4�����Ʊ��ģ� ��������λΪ��0001 0 100
//            SX_Sens_SndBuf[0][ptbuf++]=(5<<1)|0x01;				//���Ʊ�������:5���������ñ�ʶ��1�����ã�
//            //�����б�
//            if(limittype[0]!=0)
//            {
//                SX_Sens_SndBuf[0][ptbuf++]=(limittype[0]<<2)|0x00;		//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
//				SX_Sens_SndBuf[0][ptbuf++]=((limittype[0]<<2)|0x00)>>8;	//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0];			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>8;			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>16;		//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[0]>>24;		//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1];			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>8;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>16;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[1]>>24;			//
//            }
//            if(limittype[1]!=0)
//            {
//                SX_Sens_SndBuf[0][ptbuf++]=(limittype[1]<<2)|0x00;		//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
//				SX_Sens_SndBuf[0][ptbuf++]=((limittype[1]<<2)|0x00)>>8;	//�������ͣ�limittype[0] �����ݳ���ָʾλ��0���޳����ֽڣ�Ĭ��4�ֽڣ�
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2];			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>8;			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>16;		//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[2]>>24;		//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3];			//����
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>8;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>16;			//
//                SX_Sens_SndBuf[0][ptbuf++]= limitsen[3]>>24;			//
//            }
//            //CRCУ��λ
//            ptlength=ptbuf-2;
//            pti=RTU_CRC(&(SX_Sens_SndBuf[0][2]),ptlength);		//CRC
//            SX_Sens_SndBuf[0][ptbuf++]=pti;
//            SX_Sens_SndBuf[0][ptbuf++]=pti>>8;

//            //MIC
//            SX_Sens_SndBuf[0][ptbuf++]=0;					//�ۼӺ�����
//            ptlength=ptbuf;									//�õ�֡���ܳ���
//            SX_Sens_SndBuf[0][1]=ptlength-9;				//�޸�payload�ĳ���
//            for(pti=0; pti<(ptlength-1); pti++)				//�����������ݵ��ۼӺ�
//            {
//                SX_Sens_SndBuf[0][ptlength-1]+=SX_Sens_SndBuf[0][pti];
//            }
//            SX1280_Data_Send(0,ptlength);					//��������REQ�ŵ����ݷ���
//        }
//        break;
//    }
//}







