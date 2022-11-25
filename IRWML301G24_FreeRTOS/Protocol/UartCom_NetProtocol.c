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
#include "UartCom_NetProtocol.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "limits.h"
#include "main.h"

extern QueueHandle_t Uart1Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
extern QueueHandle_t Uart2Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
extern QueueHandle_t Uart4Tx_MessageData_Q;   				//串口4发送数据的消息队列句柄
extern bool En_debugIP3 ;
extern uint8_t  UART1orUART2;

struct SetPara4  Config;
uint8_t const	CMD_Head[5]= {0x24,0x50,0x41,0x52,0x41};

//uint8_t En_ProtocolRun= 0;		//串口发送数据是否遵循协议：0-则为纯网络帧数据；1-则为按协议数据（命令字：58,59）
uint8_t DSCH_pdu_Ind = 0;     // 0 - no pdu;
uint8_t DSCH_pdu_Type = 0;    // 1 - net;0-mac;
uint8_t DSCH_payload_idx = 0;
uint16_t DSCH_payload_length = 0;
uint8_t L3_SinkID[6];
uint8_t L3_SensorID[6];
uint8_t L3_DestID[6];
uint8_t Ind_Sink,Ind_Sensor;
uint8_t MessageData_Q_Buf[USART2_TX_LEN];
uint8_t MessageData_Q_Buftemp[USART2_TX_LEN];  //该缓冲区必须是最大的

/*******************************************************
*Function Name	:Config_init
*Description  	:
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void Config_init(void)
{
//    Config.emptyflag = 0xA5;
    Config.RelayClient_status = RelayClient_ON;
    Config.desARFCN = 50;
    Config.desMID = 0xFF01;
    Config.addr = 0x0000;
    Config.D_DID[0] = 0xA9;
    Config.D_DID[1] = 0x2E;
    Config.D_DID[2] = 0x00;
    Config.D_DID[3] = 0x00;
    Config.D_DID[4] = 0xFF;
    Config.D_DID[5] = 0xFF;
//    Config.BCH_ARFCN_QTY = 0;
//    Config.BCH_ARFCN_List[0] = 50;
//    Config.BCH_ARFCN_List[1] = 40;
//    Config.BCH_ARFCN_List[2] = 20;
//    Config.BCH_ARFCN_List[3] = 30;
//    Config.BCH_ARFCN_List[4] = 40;

    Config.lock_config = 0;
    Config.NID = 0;
    Config.version = 0;
    Config.HeadFN  = 1;  // number of the first frame of this relay node
    Config.BP      = 2;   //广播周期
    Config.Deep    = 1;    //跳数,注意，Search_Beacon在初同步搜索到BCH后，会自动将Config.Deep设置为master Deep+1
    Config.tempFN  =0;

    Config.QTY_Frame  = 1000;
    Config.QTY_Slot   = 200;
    Config.QTY_Chip   = 5;         //5ms
    Config.QTY_slotDL = 100;
    Config.QTY_slotUL = 100;
    Config.GPD        = 10;        //1ms
    Config.GPU        = 10;        //1ms
    Config.GPDU       = 10;         //改为100us为单位
    Config.GPUD       = 10;         //改为100us为单位

    Config.HW = 1.0000;
    Config.FW = 2.000010;

//    Config.SinkCnt = 0;
//    for (int i=0; i<8; i++)
//    {
//        Config.Sink[i].Addr   = i + 0xFF58;
//        memcpy(Config.Sink[i].EID,Device[i],6);
//        Config.Sink[i].BW     = 8;
//    }

//    Config.RouteCnt =0;
//    Config.Route[0].SEID[0] = 1;
//    Config.Route[0].SEID[1] = 2;
//    Config.Route[0].SEID[2] = 3;
//    Config.Route[0].SEID[3] = 4;
//    Config.Route[0].SEID[4] = 5;
//    Config.Route[0].SEID[5] = 6;
//    Config.Route[0].MEID[0] = 9;
//    Config.Route[0].MEID[1] = 9;
//    Config.Route[0].MEID[2] = 9;
//    Config.Route[0].MEID[3] = 9;
//    Config.Route[0].MEID[4] = 9;
//    Config.Route[0].MEID[5] = 9;

//    Config.UschCnt = 0;
//    for (int i=0; i<8; i++)
//    {
//        Config.UschChannel[i].Addr = i + 0xFB00;
//        Config.UschChannel[i].HeadSlot = 1*i;
//        Config.UschChannel[i].SlotLength = 1;
//    }

//    Config.MaxBWforSingleDevice = 35;
//    Config.MaxActiveSlots = 90;
//    Config.DefaultBWforSink = 30;
    if (Config.iChannel == 0) Config.Max_USCH_Silent_Time = 6;
    else Config.Max_USCH_Silent_Time = 2;

    Config.iChannel = 3;		//第3通道
}

/*******************************************************
*Function Name	:NetProCom_Rx_Pro
*Description  	:串口接收数据处理任务
*Input		  	:
*Output		  	:
*Question	  	:1、可改为流解析
*******************************************************/
bool NetProCom_Rx_Pro(uint8_t *Redata_pt,uint8_t comnum)
{
    uint16_t pdu_length,signaling_length;
    int idx_offset=0;
    bool ind_signaling = false;

    pdu_length=(Redata_pt[USART2_REC_LEN-2]<<8)+(Redata_pt[USART2_REC_LEN-1]);	//默认最后两个字节是该帧长度
    if(pdu_length>(USART2_REC_LEN-2))	//超过最大长度（注意去掉长度俩字节）
        pdu_length=0;
//    if ((pdu_length >= 10) && (idx_offset == 0) && ((Redata_pt[idx_offset] == 0x24) && (Redata_pt[idx_offset+1] == 0x50) && (Redata_pt[idx_offset+2] == 0x41) && (Redata_pt[idx_offset+3] == 0x52) && (Redata_pt[idx_offset+4]== 0x41)))
    if ((pdu_length >= 10) && (idx_offset == 0) && (memcmp(Redata_pt,CMD_Head,5)==0))
    {
        signaling_length =  (Redata_pt[5]<<8) + Redata_pt[6];
        if (pdu_length == signaling_length + 7)
            ind_signaling = true;
    }
    if (ind_signaling == true) {
        int crc = RTU_CRC(Redata_pt,pdu_length - 2);
        if ( crc != (Redata_pt[pdu_length - 2] << 8) + Redata_pt[pdu_length - 1] )
        {    
			ind_signaling = false;
			printf("\r\n$NetProCom_Rx_Pro,CRC Error.");
		}
    }

    if ( ind_signaling )	//符合随路接口协议
    {
        //if ( En_debug ) printf("$ReadUART,Signaling;\r");
        if(procNorthSignaling(Redata_pt,comnum))  return true;
        else return false;
//        protocol_length = pdu_length;
    }
    else
    {        //纯数据
       
            if(proc_DSCH_pdu(Redata_pt,pdu_length,comnum))  //数据处理
                return true;
            else
                return false;
    }
}

/*******************************************************
*Function Name	:procNorthSignaling
*Description  	:参数设置
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
bool procNorthSignaling(uint8_t *NorthRxDMABuff,uint8_t comnum)
{
    uint16_t length;
    bool check=true;
//    uint16_t temp_FN; //temp_profile,temp_arfcn,temp_FN,temp_DEEP;
//    uint8_t EID_T[6];
//    if ((NorthRxDMABuff[0] == 0x24) && (NorthRxDMABuff[1] == 0x50) && (NorthRxDMABuff[2] == 0x41) && (NorthRxDMABuff[3] == 0x52) && (NorthRxDMABuff[4] == 0x41))
//    if(memcmp(NorthRxDMABuff,CMD_Head,5)==0)
    {
        switch( NorthRxDMABuff[7] )
        {
        case 0x59:		// down net data//           
                length=(NorthRxDMABuff[5]<<8) + NorthRxDMABuff[6];
                proc_DSCH_pdu(&NorthRxDMABuff[8],length,comnum);
            break;
        case Net_DL_BeatHeartACK:		//down heartbeat ack
//                printf("\r\nReceive heartbeat ack.");
            break;
        case 0x1A:      // SAVE CONFIG
            printf("$PARA,OK. \r\n");
//            Write_flash_Parameter((uint8_t *)&Config,sizeof(Config));
            break;
        case 0x12:     // SET SerialNumber
//            Config.SerialNumber = (NorthRxDMABuff[8] << 24) + (NorthRxDMABuff[9] << 16) +(NorthRxDMABuff[10] << 8) + NorthRxDMABuff[11];
            printf("$PARA,OK. \r\n");
            break;
        case 0x14:     // SET ARFCN AND MID
//            Config.desARFCN = NorthRxDMABuff[8];
//            Config.desMID   = (NorthRxDMABuff[10] << 8) + NorthRxDMABuff[11];
//            printf("$PARA,OK. \r\n");
            break;
        default:
            check=false;
            //printf("$SCMD,unsupport signaling;\r");
            break;
        }
    }
    return check;
}

/*******************************************************
*Function Name	:proc_DSCH_pdu
*Description  	:按照协议（组网新架构即Relay_M和Relay_C之间的串口协议）进行解析
*Input		  	:
*Output		  	:
*Question	  	:1、DSCH_pdu数组的第一个字节就是（节点组网协议）网络帧类型；
*******************************************************/
bool proc_DSCH_pdu(uint8_t *DSCH_pdu,uint16_t DSCH_pdulength, uint8_t comnum)
{
    uint8_t  Ind_L3BCH,Ind_L3DU,Ind_L3CD,Ind_Port;
    uint8_t  NwkCmdType;
	uint8_t  cmdresult;
    // add DSCH_pdu_proc
    if(0) {
        printf("\r\n$DL_SDU,%d:",DSCH_pdulength);
        if ( DSCH_pdulength < 30 )
            for (int i=0; i<DSCH_pdulength; i++) 	printf("%02X ",DSCH_pdu[i]);
        else 	for (int i=0; i<30; i++) 				printf("%02X ",DSCH_pdu[i]);
        //for (int i=0;i<5;i++) printf("%02X ",DSCH_pdu[i]);
    }
    DSCH_pdu_Ind = 1;
    DSCH_pdu_Type = 1;
    if (DSCH_pdu_Ind == 1) {
        //if ( En_debug ) printf("$procDSCH,rx,PDU,%d. \r\n",DSCH_pdu_Type);
        if ( DSCH_pdu_Type == 1) //net
        {
            // check，DSCH_pdu数组的第一个字节就是网络帧类型
            Ind_L3BCH  = (DSCH_pdu[0] & 0x80) >> 7;
            Ind_L3DU   = (DSCH_pdu[0] & 0x40) >> 6;
            Ind_L3CD   = (DSCH_pdu[0] & 0x20) >> 5;		//网络命令or网络数据
            Ind_Sink   = (DSCH_pdu[0] & 0x10) >> 4;
            Ind_Sensor = (DSCH_pdu[0] & 0x0C) >> 2;
            Ind_Port   = (DSCH_pdu[0] & 0x02) >> 1;
            /////根据网络帧类型定义字节：1-修改指针DSCH_payload_idx；2-实际数据长度DSCH_payload_length；3-保存EID或者端口号///////
            //修改完的DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）
            if ((Ind_L3BCH == 1) || (Ind_L3DU == 1)) { //如果为全网广播或者上行数据，则长度DSCH_payload_length=0；
                // add BCH process
                if (Ind_L3BCH == 1) printf("$net BCH. \r\n");
                if (Ind_L3DU == 1) printf("$net DL/UL ERROR. \r\n");
                DSCH_payload_idx = 0;
                DSCH_payload_length = 0;
            } else {
                DSCH_payload_idx = 1;
                if ( Ind_Port == 1 ) DSCH_payload_idx = DSCH_payload_idx + 1;
                if ( Ind_Sink == 1 ) {
                    memcpy(L3_SinkID,&DSCH_pdu[DSCH_payload_idx],6);
                    DSCH_payload_idx = DSCH_payload_idx + 6;
                }
                if ( Ind_Sensor > 0 ) {
                    memcpy(L3_SensorID,&DSCH_pdu[DSCH_payload_idx],6);
                    DSCH_payload_idx = DSCH_payload_idx + 6;
                }
                DSCH_payload_length = DSCH_pdulength - DSCH_payload_idx;
            }
            //if ( En_debug ) printf("$procDSCH,rx,NET,%d,%d. \r\n",DSCH_payload_idx,DSCH_payload_length);
        }
        else { //mac 这里不会出现mac数据
            DSCH_payload_idx = 0;
            DSCH_payload_length = DSCH_pdulength;
        }

        uint8_t netType = 0;   // 0 - local; 1 - route to sink; 2 - route to Lsensor; 3 - route to uSensor
        if (Ind_Sink == 0) //没有汇聚节点地址的情况
        {
            if (Ind_Sensor == 0) netType = 0; 		//没有汇聚EID也没有传感EID
            else
            {
                if (Ind_Sensor == 2) netType = 2;	//没有汇聚EID,但有低功率EID
                if (Ind_Sensor == 1) netType = 3;	//没有汇聚EID,但有微功率EID
                if (Ind_Sensor == 3) netType = 4;	//没有汇聚EID,但有备用EID
                memcpy(L3_DestID,L3_SensorID,6);
            }
        }
        if (Ind_Sink == 1)//有汇聚节点地址的情况
        {
            if (memcmp(L3_SinkID,Config.D_DID,6) == 0) //比较是否为本地汇聚
            {
                if (Ind_Sensor == 0) netType = 0;		//有本地汇聚EID的网络命令
                else
                {
                    if (Ind_L3CD == 0)						//网络数据
                    {
                        if (Ind_Sensor == 2) netType = 2;	//有汇聚EID,也有低功率EID
                        if (Ind_Sensor == 1) netType = 3;	//有汇聚EID,也有微功率EID
                        if (Ind_Sensor == 3) netType = 4;	//有汇聚EID,也有备用EID
                    } else {
                        netType = 5;						//有本地汇聚EID的网络命令
                    }
                    memcpy(L3_DestID,L3_SensorID,6);
                }
            }
            else
            {
                netType = 1;
                memcpy(L3_DestID,L3_SinkID,6);
            }
        }
        if ((netType == 3)||(netType == 5)||(netType == 0))			//Local or route to uSensor
        {
            if ( DSCH_payload_length > 0 )
            {
                if (( DSCH_pdu_Type == 0 ) || (( DSCH_pdu_Type == 1 ) && ( Ind_L3CD == 0 )))
                {
                    // Application layer data
                    // if ( En_debug ) printf("$procDSCH,rx,App,%d,%d. \r\n",DSCH_pdu_Type,Ind_L3CD);
                    // L4 PROC
                    printf("\r\n$net uSensor data");
                } else { // net command
                    // printf("$net local cmd \r\n");
                    NwkCmdType = DSCH_pdu[DSCH_payload_idx];
                    switch( NwkCmdType )
                    {
                    case Net_DL_SinkRouterAssign:   // Sink RouteList Assign
                        if(En_debugIP3)  printf("\r\n$Net_DL_SinkRouterAssign.");
                        // Load_NET_ULAck(NwkCmdType);
                        if (Config.iChannel == 0)
//							procNetCmdSinkRoute();
                            break;
                    case Net_DL_SensorRouteAssign:   // Sensor RouteList Assign，即下属传感器路由表下发
                        if(En_debugIP3)  printf("\r\n$Net_DL_SensorRouteAssign.");
                        cmdresult=procNetCmdSensorRoute(DSCH_pdu,comnum);
						if(cmdresult==2)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复正确应答						
						else if(cmdresult==1)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复错误应答,暂改回正确应答
                        break;
                    case Net_DL_DeviceRegAck:   // Reg Ack 下属设备注册应答
//                        if(En_debugIP3)  printf("\r\n$Net_DL_DeviceRegAck.");
                        procNetCmdRegAck(DSCH_pdu,comnum);
                        break;
                    case Net_DL_ChannelStatusQuery:   // Query Channel Status
                        if(En_debugIP3)  printf("\r\n$Net_DL_ChannelStatusQuery.");
                        // procNetCmdQueryChannelStatus();
                        break;
                    case Net_DL_ChannelConfig:   // Set Channel Configuration
                        if(En_debugIP3)  printf("\r\n$Net_DL_ChannelConfig.");
                        // procNetCmdSetChannelConfig();
                        break;
                    case Net_DL_DeviceStatusQuery:   // Query Device Status
                        if(En_debugIP3)  printf("\r\n$Net_DL_DeviceStatusQuery.");
                        // procNetCmdQueryDeviceStatus((DSCH_pdu[DSCH_payload_idx + 1] << 8)+DSCH_pdu[DSCH_payload_idx + 2]);
                        break;
                    case Net_DL_ACK:   // DL Net Ack
                        // printf("$CMD,DL_ACK,%02X,%02X.\r\n",DSCH_pdu[DSCH_payload_idx+1],DSCH_pdu[DSCH_payload_idx+2]);
                        break;
                    case Net_DL_SensorComParaConfig:   // Set Sensor Communication Parameters 下属传感器通信参数配置处理
                        if(En_debugIP3)  printf("\r\n$Net_DL_SensorComParaConfig.");
//                        if ( Config.iChannel == 2)
                        cmdresult=procNetCmdComParaConfig(DSCH_pdu,comnum);
						if(cmdresult==2)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复正确应答
						else if(cmdresult==1)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复错误应答,暂改回正确应答	
                        break;
                    case Net_DL_SensorComParaQuery:   // Query Sensor Communication Parameters 下属传感器通信参数查询处理
                        if(En_debugIP3)  printf("\r\n$Net_DL_SensorComParaQuery.");
//                        if ( Config.iChannel == 2)
                        procNetCmdQueryComPara(DSCH_pdu,comnum);
                        break;
                    case Net_DL_SensorBWListConfig:   // Set Black-White List	//黑白名单下发
//                        if(En_debugIP3)  printf("\r\n$Net_DL_SensorBWListConfig.");
                        cmdresult=procNetCmdBWList(DSCH_pdu,comnum);
						if(cmdresult==2)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复正确应答
						else if(cmdresult==1)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复错误应答,暂改回正确应答							
                        break;

                    case Net_DL_SinkControl:   //节点控制命令_C0:是否保存配置或者设备列表
                        if(En_debugIP3)  printf("\r\n$Net_DL_SinkControl.");
						cmdresult=procNetCmd_C0_SinkControl(DSCH_pdu,comnum);
						if(cmdresult>0)
							Load_NetCmd_UL_ACK(DSCH_pdu[DSCH_payload_idx],0xAA);	//回复正确应答
                        break;
                    case Net_DL_ChannelParaQuery:   //节点通道工作参数查询_C1：主要指470频点的两个通道						
                        if(En_debugIP3)  printf("\r\n$Net_DL_ChannelParaQuery.");
						procNetCmd_C1_ChannelParaQuery(DSCH_pdu,comnum);
                        break;
                    case Net_DL_ChannelParaConfig:   //节点通道工作参数配置_C2：主要指470频点的两个通道
                        if(En_debugIP3)  printf("\r\n$Net_DL_ChannelParaConfig.");
                        break;
                    case Net_DL_DeviceParaConfig:   //下属（通道）设备通信参数配置_C3：主要指470频点的两个通道
                        if(En_debugIP3)  printf("\r\n$Net_DL_DeviceParaConfig.");
                        break;
                    case Net_DL_DeviceParaQuery:   //下属（通道）设备通信参数查询_C4：主要指470频点的两个通道
                        if(En_debugIP3)  printf("\r\n$Net_DL_DeviceParaQuery.");
						procNetCmd_C4_DeviceParaQuery(DSCH_pdu,comnum);
                        break;
                    case Net_DL_DeviceListQuery:   //下属（通道）设备列表查询_C5
                        if(En_debugIP3)  printf("\r\n$Net_DL_DeviceListQuery.");
                        procNetCmd_C5_DeviceListQuery(DSCH_pdu,comnum);
                        break;
                    case Net_DL_RouteListQuery:   //下属（通道）路由表查询_C6：主要指470频点的两个通道
                        if(En_debugIP3)  printf("\r\n$Net_DL_RouteListQuery.");
                        break;
                    default:
                        break;
                    }
                }
            }
            DSCH_pdu_Ind = 0;
            DSCH_pdulength = 0;
            return true;
        }

        if (netType == 4)			// route to unknown sensor
        {
            if(En_debugIP3)
            {
                printf("\r\n$Route to unknown SensorID:");
                for (int i=0; i<6; i++) printf("%02X",L3_DestID[i]);
            }
            DSCH_pdu_Ind = 0;
            DSCH_pdulength = 0;
            return true;
        }
    }
    return true;
}

/*******************************************************
*Function Name	:procNetCmdSensorRoute
*Description  	:下属传感器路由表下发
*Input		  	:
*Output		  	:0:异常错误；1：命令有误；2：命令正确
*Question	  	:1、
*******************************************************/
uint8_t procNetCmdSensorRoute(uint8_t *DSCH_pdu, uint8_t comnum)
{
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    //DSCH_payload_idx+7指向指令内容的指示字节（前7个字节分别为：1字节命令字，6字节主设备EID）
    uint8_t device_type = (DSCH_pdu[DSCH_payload_idx+7]&0xC0)>>6;
    uint8_t proc_type   = (DSCH_pdu[DSCH_payload_idx+7]&0x30)>>4;
    uint8_t channel     = (DSCH_pdu[DSCH_payload_idx+7]&0x0F)>>0;
    uint8_t device_cnt  = DSCH_pdu[DSCH_payload_idx+8];
    uint16_t idx;
//    int8_t idx_route;
//    int16_t idx_DeviceList;
	int8_t procresult=0;
    if (1)
    {
        printf("\r\n$SensorRouterAssign,[");
        for (int i=0; i<6; i++) printf("%02X",DSCH_pdu[DSCH_payload_idx+1+i]);
        printf("],DT%d,PT%d,C%d,Cnt%d.",device_type,proc_type,channel,device_cnt);
    }
    if ( channel != 3 ) return 0;

    if ( device_type != DeviceType_UP ) return 0;                    // uPower
    
    if ( memcmp(Config.D_DID,&DSCH_pdu[DSCH_payload_idx+1],6) == 0 ) // master is this device
    {   
		procresult=2;
		if ( proc_type == ProcType_Reset ) 	//重置，则全部删除
		{
			// DeviceList_Init();
			printf("\r\nSensor Route Reset.");
			SensorNodeList_Init();		//初始化列表
			procresult=2;
		}
		
		// deviceList
        else if ( proc_type == ProcType_Del )                             // del deviceList
        {
            idx = DSCH_payload_idx + 9;
            for (int i=0; i<device_cnt; i++)
            {
                if(SenID_DEL(&DSCH_pdu[idx])==false)
					procresult=1;
                idx = idx + 6;
            }
        }		
        else if ( proc_type == ProcType_Add )                            // add deviceList
        {
            idx = DSCH_payload_idx + 9;
            for (int i=0; i<device_cnt; i++)
            {
                if(ADDID_RegState(&DSCH_pdu[idx],SenRegSt_Success)==false)
					procresult=1;
                idx = idx + 6;
            }
        }
    }
	else
		procresult=1;

    if(En_debugIP3)
    {
        printf("\r\nSenList length is: %d",SensorNode_List.Nodenum);
        GetNodeNum_AllType();
        printf("\r\nBLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
    }
	return procresult;
}


/*******************************************************
*Function Name	:procNetCmdRegAck
*Description  	:下属设备注册应答
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void procNetCmdRegAck(uint8_t *DSCH_pdu, uint8_t comnum)              //bool proc_DSCH_pdu(uint8_t *DSCH_pdu,uint16_t DSCH_pdulength, uint8_t comnum)
{
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t device_type = (DSCH_pdu[DSCH_payload_idx+1]&0xC0)>>6;  	//下属设备注册应答的指示字节的设备类型：
    uint8_t result = (DSCH_pdu[DSCH_payload_idx+1]&0x20)>>5;		//注册结果：0-不通过；1-通过
    uint8_t channel = (DSCH_pdu[DSCH_payload_idx+1]&0x0F)>>0;		//通道编号：应该是3-微功耗主设备通道
    uint8_t cnt = DSCH_pdu[DSCH_payload_idx+2];						//设备个数
//    uint8_t EID[6];
    uint16_t idx = 0;
//    int16_t idx_devicelist;
    idx = DSCH_payload_idx+3;	//指向第一个从设备EID的第一个字节
    if(channel!=3) {
        return;
    }

    if (device_type == DeviceType_UP)  //uPower sensor
    {
        for (uint16_t j=0; j<cnt; j++)
        {
            uPSensorRegReqResp_Pro(&DSCH_pdu[idx],result);
            idx = idx + 6;
        }
    }
}

/*******************************************************
*Function Name	:uPSensorRegReqResp_Pro
*Description  	:微功率传感器注册响应的处理
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void uPSensorRegReqResp_Pro(uint8_t *EID,uint8_t result)
{
    uint8_t senid[6];
    memcpy(senid,EID,6);
    if(result==0) //注册不成功，
    {
        ADDID_RegState(senid,SenRegSt_Fail);
//        if(En_debugIP3) printf("\r\nRegister fail, and write list EID :%02X %02X %02X %02X %02X %02X ",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
    }
    else if(result==1) //注册成功，
    {
        ADDID_RegState(senid,SenRegSt_Success);
//        if(En_debugIP3) printf("\r\nRegister succeed  and write list EID :%02X %02X %02X %02X %02X %02X ",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
    }
    else if(En_debugIP3) printf("\r\nuPSensorRegReqResp_Pro:result is error!");

//    if(En_debugIP3)
//    {
//        printf("\r\nSenList length is: %d",SensorNode_List.Nodenum);
//        GetNodeNum_AllType();
//        printf("\r\nBLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
//    }
}

/*******************************************************
*Function Name	:procNetCmdComParaConfig
*Description  	:下属传感器通信参数配置处理
*Input		  	:
*Output		  	: 0:异常错误；1：命令有误；2：命令正确
*Question	  	:1、延迟的设计需要改进
*******************************************************/
uint8_t procNetCmdComParaConfig(uint8_t *DSCH_pdu, uint8_t comnum) //
{
    if (1)
    {
        printf("$CmdComParaConfig,");
        for (int i=0; i<DSCH_payload_length; i++)
            printf("%02X ",DSCH_pdu[DSCH_payload_idx + i]);
        printf("\r\n");
    }
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];		//通道编号：应该是3-微功耗主设备通道
    uint8_t ParaCnt  = DSCH_pdu[DSCH_payload_idx + 2];
    uint16_t idx = DSCH_payload_idx + 3;	//指向参数类型1字节
    uint8_t cnt = 0;
//    uint8_t length;
    uint16_t idx_whlist;
//	uint8_t procresult=0;
	
    //printf("$CmdComParaConfig,C%d,L%d.\r\n",channel,ParaCnt);
    if (channel != 3) return 0;	//必须是3-微功耗主设备通道
    if (Ind_Sensor!=1) return 0;	//必须是有微功率EID
    idx_whlist=CheckID_isWH(L3_SensorID);
    if(idx_whlist==0xFFFF)  { //表示不名单中或者属性不为白名单
        if(En_debugIP3) printf("\r\nprocNetCmdComParaConfig:The ID is not in the whiteIDlist!");
        return 1;
    }
    while ((cnt < ParaCnt) && (idx < 250))
    {
        //组网为大端模式，故不能直接拷贝 20200116
        switch(DSCH_pdu[idx])
        {
        case 0x80: //业务周期长度，4字节
            memcpy(&SensorNode_List.BS_Cycle[idx_whlist],&DSCH_pdu[idx + 1],4);
            //WhiteSensorNode_List.BS_Cycle[idx_whlist]=(DSCH_pdu[idx + 1]<<24)+(DSCH_pdu[idx + 2]<<16)+(DSCH_pdu[idx + 3]<<8)+(DSCH_pdu[idx + 4]);
            idx = idx + 5;
            break;
        case 0x81: //控制周期长度，2字节
            memcpy(&SensorNode_List.REQ_Cycle[idx_whlist],&DSCH_pdu[idx + 1],2);
            //WhiteSensorNode_List.REQ_Cycle[idx_whlist]=(DSCH_pdu[idx + 1]<<8)+(DSCH_pdu[idx + 2]);
            idx = idx + 3;
            break;
        case 0x82: //延迟，4字节
            memcpy(&SensorNode_List.DelayTime[idx_whlist],&DSCH_pdu[idx + 1],4);
            //WhiteSensorNode_List.WhID_Slotorder[idx_whlist]=(DSCH_pdu[idx + 1]<<24)+(DSCH_pdu[idx + 2]<<16)+(DSCH_pdu[idx + 3]<<8)+(DSCH_pdu[idx + 4]);
            idx = idx + 5;
            break;
        case 0x83: //最大扰动，1字节
            memcpy(&SensorNode_List.ShockTime[idx_whlist],&DSCH_pdu[idx + 1],1);
            //WhiteSensorNode_List.ShockTime[idx_whlist]=(DSCH_pdu[idx + 1]);
            idx = idx + 2;
            break;
        case 0x84: //频点，1字节
            idx = idx + 2;
            break;
        case 0x85: //物理层参数，1字节
            idx = idx + 2;
            break;
        case 0x86: //REQ等待回复周期，1字节
            idx = idx + 2;
            break;
        case 0x87: //BURST等待回复周期，1字节
            idx = idx + 2;
            break;
        case 0x88: //组合，11字节
            memcpy(&SensorNode_List.BS_Cycle[idx_whlist],&DSCH_pdu[idx + 1],4); 	//周期
            //WhiteSensorNode_List.BS_Cycle[idx_whlist]=(DSCH_pdu[idx + 1]<<24)+(DSCH_pdu[idx + 2]<<16)+(DSCH_pdu[idx + 3]<<8)+(DSCH_pdu[idx + 4]);
            idx = idx + 5;
            memcpy(&SensorNode_List.REQ_Cycle[idx_whlist],&DSCH_pdu[idx + 1],2);	//控制
            //WhiteSensorNode_List.REQ_Cycle[idx_whlist]=(DSCH_pdu[idx + 1]<<8)+(DSCH_pdu[idx + 2]);
            idx = idx + 2;
            memcpy(&SensorNode_List.DelayTime[idx_whlist],&DSCH_pdu[idx + 1],4);	//延迟
            //WhiteSensorNode_List.WhID_Slotorder[idx_whlist]=(DSCH_pdu[idx + 1]<<24)+(DSCH_pdu[idx + 2]<<16)+(DSCH_pdu[idx + 3]<<8)+(DSCH_pdu[idx + 4]);
            idx = idx + 4;
            memcpy(&SensorNode_List.ShockTime[idx_whlist],&DSCH_pdu[idx + 1],1);	//扰动
            //WhiteSensorNode_List.ShockTime[idx_whlist]=(DSCH_pdu[idx + 1]);
            idx = idx + 1;
            break;
        default:
            break;
        }
        cnt ++;
    }

    if (0)
    {
//        printf("ParaCnt=%d,Ind=%X,SaveInd=%d,Period=%d,AveData=%d,BW=%d,Time=%d.\r\n",ParaCnt,LpConfig.Ind,LpConfig.efficientInd,LpConfig.trafficPeriod,LpConfig.trafficAveDataLength,LpConfig.BW,LpConfig.drxTime);
        printf("\r\nCmdComParaConfig success:");
        printf("\r\nEID:");
        for(uint8_t tempi=0; tempi<6; tempi++)
            printf("%02X ",L3_SensorID[tempi]);
        printf("\r\nBS_Cycle: %d ",SensorNode_List.BS_Cycle[idx_whlist]);
        printf("\r\nREQ_Cycle: %d ",SensorNode_List.REQ_Cycle[idx_whlist]);
        printf("\r\nDelayTime: %d ",SensorNode_List.DelayTime[idx_whlist]);
        printf("\r\nShockTime: %d ",SensorNode_List.ShockTime[idx_whlist]);
    }
	return 2;
}


/*******************************************************
*Function Name	:procNetCmdQueryComPara
*Description  	:下属传感器通信参数查询处理：组下属传感器通信参数上报帧
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
*******************************************************/
void procNetCmdQueryComPara(uint8_t *DSCH_pdu, uint8_t comnum) //
{

    if (0)
    {
        printf("$CmdComParaConfig,");
        for (int i=0; i<DSCH_payload_length; i++)
            printf("%02X ",DSCH_pdu[DSCH_payload_idx + i]);
        printf("\r\n");
    }
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];		//通道编号：应该是3-微功耗主设备通道
    uint8_t ParaCnt  = DSCH_pdu[DSCH_payload_idx + 2];
    uint16_t idx = DSCH_payload_idx + 3;	//指向参数类型1字节
    uint16_t cnt = 0;
//    uint8_t length;
    uint16_t idx_whlist;
//    uint8_t MessageData_Q_Buf[USART2_TX_LEN];
    uint16_t idx_QBuf=0;
    BaseType_t err;
	uint8_t ParaCnt_Useful=0;

    if (channel != 3) return;	//必须是3-微功耗主设备通道
    if (Ind_Sensor!=1) return;	//必须是有微功率EID

    idx_whlist=FixPosition_InSenList(L3_SensorID);
    if(idx_whlist==0xFFFF)  { //表示不名单中
        if(En_debugIP3) printf("\r\nprocNetCmdComParaConfig:The ID is not in the whiteIDlist!");
        return;
    }

    idx_QBuf=2;
    MessageData_Q_Buf[idx_QBuf++]=0x64; 			//网络帧类型0110 0100：端端；上行；网络层指令；无末端汇聚EID；有末端传感器EID；无端口号；保留为0
    memcpy(&MessageData_Q_Buf[idx_QBuf],L3_SensorID,6);	//拷贝末端传感器EID
    idx_QBuf=idx_QBuf+6;		//idx_QBuf=9
    MessageData_Q_Buf[idx_QBuf++]=Net_UL_SensorParaReport;	//网络帧命令：下属传感器通信参数上报
    MessageData_Q_Buf[idx_QBuf++]=3;						//通道编号：应该是3-微功耗主设备通道
    MessageData_Q_Buf[idx_QBuf++]=0;					//参数个数,此时idx_QBuf=11,后续需要更改
    while ((cnt < ParaCnt) && (idx < 250))
    {
        //组网为大端模式，微功率协议为小端模式，故不能直接拷贝
        switch(DSCH_pdu[idx])
        {
        case 0x80: //业务周期长度，4字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x80;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.BS_Cycle[idx_whlist],4);
            idx_QBuf=idx_QBuf+4;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist];
            break;
        case 0x81: //控制周期长度，2字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x81;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.REQ_Cycle[idx_whlist],2);
            idx_QBuf=idx_QBuf+2;
            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist]>>8;
            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist];
            break;
        case 0x82: //延迟，4字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x82;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.DelayTime[idx_whlist],4);
            idx_QBuf=idx_QBuf+4;
//            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist];
            break;
        case 0x83: //最大扰动，1字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x83;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.ShockTime[idx_whlist],1);
            idx_QBuf=idx_QBuf+1;
//            MessageData_Q_Buf[idx_QBuf++]=SensorNode_List.ShockTime[idx_whlist];
            break;
        case 0x84: //业务频点，1字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x84;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SenBS_CH1_Fre,1);
            idx_QBuf=idx_QBuf+1;
            break;
        case 0x85: //物理层参数，1字节

            break;
        case 0x86: //REQ等待回复周期，1字节

            break;
        case 0x87: //BURST等待回复周期，1字节

            break;
        case 0x88: //组合，11字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x88;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.BS_Cycle[idx_whlist],4);	//周期
            idx_QBuf = idx_QBuf + 4;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.REQ_Cycle[idx_whlist],2);	//控制
            idx_QBuf = idx_QBuf + 2;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.DelayTime[idx_whlist],4);	//延迟
            idx_QBuf = idx_QBuf + 4;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.ShockTime[idx_whlist],1);	//扰动
            idx_QBuf = idx_QBuf + 1;
//            //业务周期长度
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist];
//            //控制周期长度
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist];
//            //延迟
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist];
//            //最大扰动
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.ShockTime[idx_whlist];
            break;
        default:
            break;
        }
        idx = idx + 1;
        cnt ++;
        if(idx_QBuf>(USART2_TX_LEN-12))
            return;
    }
	MessageData_Q_Buf[11]=ParaCnt_Useful;	//参数个数,此时idx_QBuf=11,后续需要更改
    MessageData_Q_Buf[0]=(idx_QBuf-2)>>8; 	//修改长度(不包括自己)
    MessageData_Q_Buf[1]=(idx_QBuf-2);   
    if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    if(err==errQUEUE_FULL)
    {
        printf("\r\n SenCTR_CH_Task2:UartTx_MessageData_Q is full and send fail!");
    }
}

/*******************************************************
*Function Name	:procNetCmdBWList
*Description  	:黑白名单处理，并进行网络帧回复
*Input		  	:
*Output		  	:0:异常错误；1：命令有误；2：命令正确
*Question	  	:1、临时缓冲区长度
				 2、黑白名单重置，只修改属性未删除
*******************************************************/
uint8_t procNetCmdBWList(uint8_t *DSCH_pdu, uint8_t comnum)
{

    uint8_t listType= (DSCH_pdu[DSCH_payload_idx+1]&0x80) >> 7;  	//名单类型：1-黑；0-白
    uint8_t reset   = (DSCH_pdu[DSCH_payload_idx+1]&0x40) >> 6;  	//是否重置：1-重置；0-保留
    uint8_t addDel  = (DSCH_pdu[DSCH_payload_idx+1]&0x20) >> 5;	 	//增减：1-增；0-减
    uint8_t addrType= (DSCH_pdu[DSCH_payload_idx+1]&0x10) >> 4;		//类型：1-微功率；0-低功率
    uint8_t cnt = DSCH_pdu[DSCH_payload_idx+2];						//数量
    uint8_t senid[6];
    uint16_t idx = 0;
	uint8_t procresult=0;
    idx = DSCH_payload_idx+3; //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
//    printf("$BWLIST,RESET=%d,addDel=%d,type=%d,addrtype=%d,cnt=%d.\r\n",reset,addDel,listType,addrType,cnt);
    if(addrType!=1)	//非微功耗
        return 0;
    if (reset == 1)		//重置所有名单(只修改属性，未删除)
    {
        procresult=2;
		if (listType == 0) //白名单
        {
            SenIDList_AttribReset(SenAttri_WH);
            printf("\r\n$reset whitelist.");
			return 2;
        }
        else		//黑名单
        {
            SenIDList_AttribReset(SenAttri_BL);
            printf("\r\n$reset blacklist. ");
			return 2;
        }
    }
    else	//修改部分名单
    {
        procresult=2;
		for (int j=0; j<cnt; j++)
        {
            if ( 0 )
            {
                printf("$EID:[");
                for (int i=0; i<6; i++) printf("%02X",DSCH_pdu[idx+i]);
                printf("].\r\n");
            }
            memcpy(senid,&DSCH_pdu[idx],6);
            if ( addDel == 0) //减
            {   // del
                if(listType==1) { //黑名单
                    if(SenID_ModifyAttrib(senid,SenAttri_Undistrib)==false)	//直接修改名单属性为未分配
						procresult=1;
                    if(En_debugIP3)
                        printf("\r\nDEL blacklist EID :%02X %02X %02X %02X %02X %02X",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
                }
                else {			//白名单
                    if(SenID_ModifyAttrib(senid,SenAttri_Undistrib)==false)	//直接修改名单属性为未分配
						procresult=1;
                    if(En_debugIP3)
                        printf("\r\nDEL whitelist EID :%02X %02X %02X %02X %02X %02X",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
                }

            } else  // add
            {
                if(listType==1) //黑名单
                {
                    if(ADDID_Attrib(senid,SenAttri_BL)==false)	//添加黑名单
						procresult=1;
                    if(En_debugIP3)
                        printf("\r\nADD blacklist EID :%02X %02X %02X %02X %02X %02X",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
                }
                else
                {
                    if(ADDID_Attrib(senid,SenAttri_WH)==false)		//添加白名单
						procresult=1;
//                    if(En_debugIP3)
//                        printf("\r\nADD whitelist EID :%02X %02X %02X %02X %02X %02X",senid[0],senid[1],senid[2],senid[3],senid[4],senid[5]);
                }
            }
            idx = idx + 6;
        }
    }

//    if(En_debugIP3)
//    {
//        printf("\r\nSenList length is: %d",SensorNode_List.Nodenum);
//        GetNodeNum_AllType();
//        printf("\r\nBLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
//    }
	return procresult;
}


/*******************************************************
*Function Name	:procNetCmd_C0_SinkControl
*Description  	:节点控制指令
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
				 2、
*******************************************************/
extern EventGroupHandle_t Common_EventGroupHandler;
uint8_t procNetCmd_C0_SinkControl(uint8_t *DSCH_pdu, uint8_t comnum)
{    
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];
    uint8_t ctrcmd  = DSCH_pdu[DSCH_payload_idx + 2];
//	uint8_t saveobject  = DSCH_pdu[DSCH_payload_idx + 2];
//    BaseType_t err;
//    uint16_t idx_QBuf=0;
//    uint16_t devicesum,SDUcnt,send_devicenum,devicelist_ptr;
//    uint16_t SDUSN;

    if (( channel != 3)&&(channel != 0xFF)) return 0;  //必须是3-微功耗主设备通道,或者所有通道（0xFF）
	if(ctrcmd==0x00)	//重启
	{
		 xEventGroupSetBits(Common_EventGroupHandler, EVENTBIT_SoftReset_Flag);
		return 1;
	}
	else if(ctrcmd==0x01)	//保存
	{
		switch(DSCH_pdu[DSCH_payload_idx + 3]) //命令的第一字节；
		{
			case 0xff:	//save all
			case 0x01:	//save devicelist
				STMFLASH_Write_devicelist(STM32_SAVE_BASE,(uint32_t *)&SensorNode_List,sizeof(SensorNode_List));
				break;
			case 0x00: 	//save config				
			case 0x02:	//save routelist
			default:
				break;
		}
		return 1;
	} 
	return 0;
}

/*******************************************************
*Function Name	:procNetCmd_C1_ChannelParaQuery
*Description  	:节点通道工作参数查询
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
				 2、
*******************************************************/
void procNetCmd_C1_ChannelParaQuery(uint8_t *DSCH_pdu, uint8_t comnum)
{    
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];
//    uint8_t cmdnum  = DSCH_pdu[DSCH_payload_idx + 2];	//参数个数
//	uint8_t saveobject  = DSCH_pdu[DSCH_payload_idx + 2];
    BaseType_t err;
    uint16_t idx_QBuf=0;
//    uint16_t devicesum,SDUcnt,send_devicenum,devicelist_ptr;
//    uint16_t SDUSN;
	if ( channel != 3 ) return ;  //必须是3-微功耗主设备通道
	MessageData_Q_Buf[2]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
    memcpy(&MessageData_Q_Buf[3],Config.D_DID,6);		//拷贝末端传感器EID
    MessageData_Q_Buf[9]=Net_UL_ChannelParaQueryAck;	//
    MessageData_Q_Buf[10]=3;							//通道编号：应该是3-微功耗主设备通道
    MessageData_Q_Buf[11]=2;							//参数个数:2;版本号
    idx_QBuf=12;	
	MessageData_Q_Buf[idx_QBuf++]=0x57;					//参数类型:硬件版本号
	memcpy(&MessageData_Q_Buf[idx_QBuf],HWVersion,4);	//
	idx_QBuf=idx_QBuf+4;
	MessageData_Q_Buf[idx_QBuf++]=0x58;					//参数类型:固件版本号
	memcpy(&MessageData_Q_Buf[idx_QBuf],FWVersion,4);	//
	idx_QBuf=idx_QBuf+4;	
    MessageData_Q_Buf[0]=(idx_QBuf-2)>>8; 				//修改长度(不包括自己)
    MessageData_Q_Buf[1]=(idx_QBuf-2);   
    if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    if(err==errQUEUE_FULL)
    {
        printf("\r\n procNetCmd_C4_DeviceParaQuery:UartTx_MessageData_Q is full and send fail!");
    }	
}

/*******************************************************
*Function Name	:procNetCmd_C4_DeviceParaQuery
*Description  	:下属设备通信参数查询
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
				 2、消息队列深度不足，则只是直接发送（直接发送有IO查询问题）
*******************************************************/
void procNetCmd_C4_DeviceParaQuery(uint8_t *DSCH_pdu, uint8_t comnum)
{
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];
	uint8_t Senid[6];
    BaseType_t err;
    uint16_t idx_QBuf=0;
    uint16_t ParaCnt,cnt,idx,idx_whlist;
	uint8_t ParaCnt_Useful;
    if ( channel != 3 ) return;  //必须是3-微功耗主设备通道
	memcpy(Senid,&DSCH_pdu[DSCH_payload_idx + 2],6);
	idx_whlist=FixPosition_InSenList(Senid);
	if(idx_whlist==0xffff) return;	//不在列表中
	ParaCnt=DSCH_pdu[DSCH_payload_idx + 8];	//取得参数个数
	idx = DSCH_payload_idx + 9;				//指向第一个参数类型字节的位置
	cnt=0;
	ParaCnt_Useful=0;
	
	MessageData_Q_Buf[2]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
    memcpy(&MessageData_Q_Buf[3],Config.D_DID,6);		//拷贝末端传感器EID
    MessageData_Q_Buf[9]=Net_UL_DeviceParaQueryAck;	//
    MessageData_Q_Buf[10]=3;						//通道编号：应该是3-微功耗主设备通道
	memcpy(&MessageData_Q_Buf[11],Senid,6);			//拷贝下属设备传感器EID	 
    MessageData_Q_Buf[17]=0;						//参数个数,此时idx_QBuf=17,后续需要更改	
    idx_QBuf=18;	
    while ((cnt < ParaCnt) && (idx < 250))
    {
        //组网为大端模式，微功率协议为小端模式，故不能直接拷贝
        switch(DSCH_pdu[idx])
        {
        case 0x80: //业务周期长度，4字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x80;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.BS_Cycle[idx_whlist],4);
            idx_QBuf=idx_QBuf+4;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist];
            break;
        case 0x81: //控制周期长度，2字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x81;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.REQ_Cycle[idx_whlist],2);
            idx_QBuf=idx_QBuf+2;
            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist]>>8;
            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist];
            break;
        case 0x82: //延迟，4字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x82;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.DelayTime[idx_whlist],4);
            idx_QBuf=idx_QBuf+4;
//            //MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist];
            break;
        case 0x83: //最大扰动，1字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x83;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.ShockTime[idx_whlist],1);
            idx_QBuf=idx_QBuf+1;
            MessageData_Q_Buf[idx_QBuf++]=SensorNode_List.ShockTime[idx_whlist];
            break;
        case 0x84: //业务频点，1字节
            //MessageData_Q_Buf[idx_QBuf++]=0x84;		//参数类型
            //memcpy(&MessageData_Q_Buf[idx_QBuf],&SenBS_CH1_Fre,1);
            //idx_QBuf=idx_QBuf+1;

            break;
        case 0x85: //物理层参数，1字节

            break;
        case 0x86: //REQ等待回复周期，1字节

            break;
        case 0x87: //BURST等待回复周期，1字节

            break;
        case 0x88: //组合，11字节
			ParaCnt_Useful++;	//实际有用的参数量加1
            MessageData_Q_Buf[idx_QBuf++]=0x88;		//参数类型
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.BS_Cycle[idx_whlist],4);	//周期
            idx_QBuf = idx_QBuf + 4;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.REQ_Cycle[idx_whlist],2);	//控制
            idx_QBuf = idx_QBuf + 2;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.DelayTime[idx_whlist],4);	//延迟
            idx_QBuf = idx_QBuf + 4;
            memcpy(&MessageData_Q_Buf[idx_QBuf],&SensorNode_List.ShockTime[idx_whlist],1);	//扰动
            idx_QBuf = idx_QBuf + 1;
//            //业务周期长度
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.BS_Cycle[idx_whlist];
//            //控制周期长度
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.REQ_Cycle[idx_whlist];
//            //延迟
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>24;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>16;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist]>>8;
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.WhID_Slotorder[idx_whlist];
//            //最大扰动
//            MessageData_Q_Buf[idx_QBuf++]=WhiteSensorNode_List.ShockTime[idx_whlist];
            break;
        default:
            break;
        }
        idx = idx + 1;
        cnt ++;
        if(idx_QBuf>(USART2_TX_LEN-12))
            return;
    }
	MessageData_Q_Buf[18]=ParaCnt_Useful;	//参数个数,此时idx_QBuf=18,后续需要更改
    MessageData_Q_Buf[0]=(idx_QBuf-2)>>8; 	//修改长度(不包括自己)
    MessageData_Q_Buf[1]=(idx_QBuf-2);   
    if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    if(err==errQUEUE_FULL)
    {
        printf("\r\n procNetCmd_C4_DeviceParaQuery:UartTx_MessageData_Q is full and send fail!");
    }	
}

/*******************************************************
*Function Name	:procNetCmd_C5_DeviceListQuery
*Description  	:下属设备列表查询
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
				 2、消息队列深度不足，则只是直接发送（直接发送有IO查询问题）
*******************************************************/
#define MARGIN_BUFF 30  //是为了留出一定空间进行保护
void procNetCmd_C5_DeviceListQuery(uint8_t *DSCH_pdu, uint8_t comnum)
{
    if (0)
    {
        printf("$DeviceListQuery,");
        for (int i=0; i<DSCH_payload_length; i++)
            printf("%02X ",DSCH_pdu[DSCH_payload_idx + i]);
        printf("\r\n");
    }
    //DSCH_payload_idx指向网络层帧结构中网络层负载的第一个字节（如果是命令帧，则为命令字）；
    uint8_t channel = DSCH_pdu[DSCH_payload_idx + 1];
    uint8_t type    = DSCH_pdu[DSCH_payload_idx + 2];
    BaseType_t err;
    uint16_t idx_QBuf=0;
    uint16_t devicesum,SDUcnt,send_devicenum,devicelist_ptr;
    uint16_t SDUSN;

//    if ( channel != Config.iChannel + 1 ) return;  //必须是3-微功耗主设备通道
    if ( channel != 3 ) return;  //必须是3-微功耗主设备通道

    GetNodeNum_AllType();	//update the num of all type SenIDList
    switch(type)
    {
    case 0: //all device list
        devicesum=SensorNode_List.Nodenum;
        break;
    case 1: //register success device list
        devicesum=SensorNode_List.RegSuccessNodenum;
        break;
    case 2://black device list
        devicesum=SensorNode_List.BLNodenum;
        break;
    case 3://white device list
        devicesum=SensorNode_List.WHNodenum;
        break;
    default:
        devicesum=0;
        break;
    }
    SDUcnt=(devicesum*6)/(SDU_MAXLenth-MARGIN_BUFF)+1;		//得到需要传输的SDU个数，其中减去20是为了留出一定空间进行保护；
    devicelist_ptr=0;		//遍历列表的指针
    send_devicenum=0;		//符合发送要求并已经发送的设备总个数
    if(En_debugIP3)	 printf("\r\nDeviceListQuery:type=%d, devicesum=%d, SDUcnt=%d ",type,devicesum,SDUcnt);
    for(SDUSN=0; SDUSN<SDUcnt; SDUSN++)
    {
        //开始装载缓冲数据
//			idx_QBuf=2;
        MessageData_Q_Buf[2]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
        memcpy(&MessageData_Q_Buf[3],Config.D_DID,6);	//拷贝末端汇聚EID
        idx_QBuf=idx_QBuf+6;
        MessageData_Q_Buf[9]=Net_UL_DeviceListQueryAck;			//网络指令：下属设备列表上报
        MessageData_Q_Buf[10]=3;									//通道号
        MessageData_Q_Buf[11]=type;									//上报类型
        MessageData_Q_Buf[12]=SDUcnt;								//SDU个数
        MessageData_Q_Buf[13]=SDUSN;								//SDU序号
        MessageData_Q_Buf[14]=0;									//设备个数，后面重新修改
        idx_QBuf=15;
        while((devicelist_ptr<SensorLIST_MAXNUM)&&(send_devicenum<devicesum)) //还没有遍历全所有列表，同时还没有发送完所有符合要求的设备列表
        {
            switch(type)
            {
            case 0: //all device list
                MessageData_Q_Buf[14]++;	//本次发送设备数量加1
                send_devicenum++;			//发送设备总数量加1
                memcpy(&MessageData_Q_Buf[idx_QBuf],SensorNode_List.Sensor_ID[devicelist_ptr],6);	//拷贝ID
                idx_QBuf=idx_QBuf+6;
                break;
            case 1: //register success device list
                if(SensorNode_List.Register_State[devicelist_ptr]==SenRegSt_Success)
                {
                    MessageData_Q_Buf[14]++;	//本次发送设备数量加1
                    send_devicenum++;			//发送设备总数量加1
                    memcpy(&MessageData_Q_Buf[idx_QBuf],SensorNode_List.Sensor_ID[devicelist_ptr],6);	//拷贝ID
                    idx_QBuf=idx_QBuf+6;
                }
                break;
            case 2://black device list
                if(SensorNode_List.Attr_BLorWL[devicelist_ptr]==SenAttri_BL)
                {
                    MessageData_Q_Buf[14]++;	//本次发送设备数量加1
                    send_devicenum++;			//发送设备总数量加1
                    memcpy(&MessageData_Q_Buf[idx_QBuf],SensorNode_List.Sensor_ID[devicelist_ptr],6);	//拷贝ID
                    idx_QBuf=idx_QBuf+6;
                }
                break;
            case 3://white device list
                if(SensorNode_List.Attr_BLorWL[devicelist_ptr]==SenAttri_WH)
                {
                    MessageData_Q_Buf[14]++;	//本次发送设备数量加1
                    send_devicenum++;			//发送设备总数量加1
                    memcpy(&MessageData_Q_Buf[idx_QBuf],SensorNode_List.Sensor_ID[devicelist_ptr],6);	//拷贝ID
                    idx_QBuf=idx_QBuf+6;
                }
                break;
            default:
                devicelist_ptr=SensorLIST_MAXNUM;
                break;
            }
            devicelist_ptr++;
            if(MessageData_Q_Buf[14]>((SDU_MAXLenth-MARGIN_BUFF)/6))  //本次发送数量达到最大值
                break;
        }
        MessageData_Q_Buf[0]=(idx_QBuf-2)>>8; 	//修改长度(不包括自己)
        MessageData_Q_Buf[1]=(idx_QBuf-2);       
        if(En_debugIP3)	printf("\r\nDeviceListQuery:SDUSN=%03d,devicesum_thistime=%03d",SDUSN,MessageData_Q_Buf[14]);

        //如果消息队列深度不足，则只是直接发送
        if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
        else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
        if(err==errQUEUE_FULL)
        {
            printf("\r\n NetCmd_UL_ACK:UartTx_MessageData_Q is full and send fail!");
        }
    }
}



/*******************************************************
*Function Name	:NetCmd_UL_ACK
*Description  	:网络层上行应答
*Input		  	:NetCmd-被应答的命令; ackresult-应答结果：AA（ receive correct）00（receive fail）
*Output		  	:
*Question	  	:1、临时缓冲区长度
*******************************************************/
void Load_NetCmd_UL_ACK(uint8_t NetCmd, uint8_t ackresult)
{
    BaseType_t err;
    uint16_t idx_QBuf=0;

    idx_QBuf=2;
    MessageData_Q_Buf[idx_QBuf++]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
    memcpy(&MessageData_Q_Buf[idx_QBuf],Config.D_DID,6);	//拷贝末端汇聚EID
    idx_QBuf=idx_QBuf+6;
    MessageData_Q_Buf[idx_QBuf++]=Net_UL_ACK;			//网络指令：网络层应答
    MessageData_Q_Buf[idx_QBuf++]=NetCmd;				//被应答的网络命令
    MessageData_Q_Buf[idx_QBuf++]=ackresult;			//应答结果
    MessageData_Q_Buf[0]=(idx_QBuf-2)>>8; 	//修改长度(不包括自己)
    MessageData_Q_Buf[1]=(idx_QBuf-2);
   
    if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
    if(err==errQUEUE_FULL)
    {
        printf("\r\n NetCmd_UL_ACK:UartTx_MessageData_Q is full and send fail!");
    }
}

///*******************************************************
//*Function Name	:Load_HeartBeat
//*Description  	://如果需要按照新协议传输数据则转换格式；
//*Input		  	:
//*Output		  	:
//*Question	  	:1、临时缓冲区长度
//*******************************************************/
//void Add_Protocol_Head(uint8_t *pt_buff)
//{
//    uint16_t bufflenth;
//    bufflenth=(pt_buff[0]<<8) +pt_buff[1];

//    memcpy(MessageData_Q_Buftemp,&pt_buff[2],bufflenth);

//    memcpy(&pt_buff[2],CMD_Head,5);
//    pt_buff[7] = ((bufflenth + 3) & 0xFF00) >> 8;		//长度
//    pt_buff[8] = ((bufflenth + 3) & 0x00FF) >> 0;
//    pt_buff[9] = 0x58;									//上行命令字
//    memcpy(&pt_buff[10],MessageData_Q_Buftemp,bufflenth);
//    uint16_t crc = RTU_CRC(&pt_buff[2],bufflenth + 8);
//    pt_buff[2 + 5 + 3 + bufflenth] = (crc & 0xFF00) >> 8;
//    pt_buff[2 + 5 + 3 + bufflenth + 1] = (crc & 0x00FF) >> 0;
//    pt_buff[0] = ((bufflenth + 5+3+2) & 0xFF00) >> 8;
//    pt_buff[1] = ((bufflenth + 5+3+2) & 0x00FF) >> 0;
//}

/*******************************************************
*Function Name	:Load_HeartBeat
*Description  	:load heartbeat data to uart
*Input		  	:
*Output		  	:
*Question	  	:1、临时缓冲区长度
*******************************************************/
void Load_HeartBeat(void)
{
    uint16_t idx_QBuf=0;

    BaseType_t err;
    idx_QBuf=2;
    memcpy(&MessageData_Q_Buf[2],CMD_Head,5);
    idx_QBuf=7;
    MessageData_Q_Buf[idx_QBuf++]=0x00; 			//长度高字节：即大端序
    MessageData_Q_Buf[idx_QBuf++]=0x11; 			//长度低字节，总共：17字节（数据）+2字节（长度）+5字节（帧头）=24字节
    MessageData_Q_Buf[idx_QBuf++]=Net_UL_BeatHeartCMD; 			//心跳命令
    memcpy(&MessageData_Q_Buf[idx_QBuf],Config.D_DID,6);	//拷贝末端汇聚EID
    idx_QBuf=idx_QBuf+6;
    MessageData_Q_Buf[idx_QBuf++]=Config.addr>>8;	//通信地址，
    MessageData_Q_Buf[idx_QBuf++]=Config.addr;
    MessageData_Q_Buf[idx_QBuf++]=Config.iChannel;	//通道编号
    MessageData_Q_Buf[idx_QBuf++]=SenBS_CH1_Fre;	//工作频点
    MessageData_Q_Buf[idx_QBuf++]=1;				//物理层配置
    MessageData_Q_Buf[idx_QBuf++]=0;				//广播周期
    MessageData_Q_Buf[idx_QBuf++]=0;				//起始帧号高地址
    MessageData_Q_Buf[idx_QBuf++]=0;				//起始帧号
    uint16_t crc=RTU_CRC(&(MessageData_Q_Buf[2]),idx_QBuf-2);		//CRC
    MessageData_Q_Buf[idx_QBuf++]=crc>>8;			//CRC
    MessageData_Q_Buf[idx_QBuf++]=crc;
    MessageData_Q_Buf[0]=(idx_QBuf-2)>>8;			//前两字节为帧长度（不包括自身）
    MessageData_Q_Buf[1]=(idx_QBuf-2);
    if(Config.RelayClient_status == RelayClient_ON)
    {
        if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
        else 				err=xQueueSend(Uart2Tx_MessageData_Q,MessageData_Q_Buf,0);//堵塞0ms
        if(err==errQUEUE_FULL)
        {
            printf("\r\n Load_HeartBeat:UartTx_MessageData_Q is full and send fail!");
        }
    }
}

