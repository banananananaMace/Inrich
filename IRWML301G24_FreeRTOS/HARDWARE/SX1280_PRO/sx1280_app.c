//#include "main.h"
#include "sx1280_app.h"
#include "sx1280_hw.h"
#include "sx1280_radio.h"
#include "sx1280.h"
#include "sx1280_app.h"
//#include "SP1808.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"
//#include "led.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//brief Define the possible message type for this application/
//const uint8_t PingMsg[] = "PING:Txtimeout!";
const char PongMsg0[] = "SX_0:Timers!";
const char PongMsg1[] = "SX_1:Timer1s!";
const char PongMsg2[] = "SX_2:Timer22s!";

uint8_t SX_Sens_RecBufSize[SX1280_CH_NUM] = {SX_SENS_RECBUF_SIZE,SX_SENS_RECBUF_SIZE,SX_SENS_RECBUF_SIZE};
uint8_t SX_Sens_RecBuf[SX1280_CH_NUM][SX_SENS_RECBUF_SIZE];

uint8_t SX_Sens_SndBufSize = SX_SENS_SNDBUF_SIZE;
uint8_t SX_Sens_SndBuf[SX1280_CH_NUM][SX_SENS_SNDBUF_SIZE];

//brief Mask of IRQs to listen to in rx mode/
uint16_t RxIrqMask = IRQ_RX_DONE |IRQ_CRC_ERROR|IRQ_HEADER_ERROR ;//| IRQ_RX_TX_TIMEOUT; IRQ_CRC_ERROR
//\brief Mask of IRQs to listen to in tx mode/
uint16_t TxIrqMask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT;

uint32_t lorasteptime[3]= {0,0,0}; //ȫ�����ʱ������systick�м�һ(1ms)��

TickTime_t TTtime_RX= {RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_SINGLEMODE};  //SIZE*VALUE=
TickTime_t TTtime_TX= {TX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE};  //SIZE*VALUE=

void delay_sx1280(u32 nms)
{
    u32 i;
    for(i=0; i<nms; i++) ;
}

/*******************************************************
*Function Name 	:SX1280_Param_Init
*Description  	: sx1280��ʼ��
*Input			: sxcase��[0,3]��ʾ�ڼ���sx1280��ԭ��ƹ���4��
*Output			:
*******************************************************/
uint16_t SX1280_ID=0;
void SX1280_Param_Init(uint8_t sxcase,uint32_t frequency)
{
    uint32_t freq;
	freq = 2399500000 + frequency * 1000000;
	
	ModulationParams_t modulationParams;
    PacketParams_t packetParams;
    Radio.Init(sxcase);			//�ⲿ�ж�����
    //SX1280HalReset(sxcase);	//reset�ܽŸ�λ������������
    Radio.SetRegulatorMode( USE_LDO,sxcase); // LDO:���ʡ�ԣ��������������ɱ����ͣ���DCDC:��Ҫ��У����Ľ��ͣ��ɱ�������
    SX1280_ID=Radio.GetFirmwareVersion(sxcase);
    HAL_Delay(100);
    printf( "\r\nRadio firmware version of SX1280_[%u] is:0x%x", sxcase, Radio.GetFirmwareVersion(sxcase) );
    //LORa Mode  beging//
    modulationParams.PacketType = PACKET_TYPE_LORA;  						//����ΪRoLaģʽ
    modulationParams.Params.LoRa.SpreadingFactor =LORA_SF8;//LORA_SF10 // LORA_SF12;  	//��Ƶ����
    modulationParams.Params.LoRa.Bandwidth = LORA_BW_0800;//LORA_BW_1600;    	//����
    modulationParams.Params.LoRa.CodingRate = LORA_CR_4_5;//LORA_CR_LI_4_7;		//����
    packetParams.PacketType = PACKET_TYPE_LORA;
    packetParams.Params.LoRa.PreambleLength = 6;//12;
    packetParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH; // LORA_PACKET_FIXED_LENGTH;//LORA_PACKET_VARIABLE_LENGTH;
    packetParams.Params.LoRa.PayloadLength = SX_SENS_SNDBUF_SIZE;       //ÿ�η��͵��ֽ�����
    packetParams.Params.LoRa.CrcMode = LORA_CRC_ON;//LORA_CRC_OFF;
    packetParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;
    //LORa Mode  end//
    Radio.SetStandby( STDBY_RC,sxcase);
    Radio.SetPacketType( modulationParams.PacketType,sxcase);
    Radio.SetModulationParams( &modulationParams,sxcase );
//    Radio.WriteRegister( 0x925,0x32,sxcase );  //���ݹٷ�оƬ����P112ҳ˵����Ҫ���������겻ͬ��Ƶ���Ӻ�ִ�б��μĴ���д���
    Radio.WriteRegister( 0x925,0x37,sxcase );  //���ݹٷ�оƬ����P112ҳ˵����Ҫ���������겻ͬ��Ƶ���Ӻ�ִ�б��μĴ���д���
    Radio.SetPacketParams( &packetParams,sxcase );	
    Radio.SetRfFrequency( freq,sxcase );  //����Ƶ��
    Radio.SetBufferBaseAddresses( 0x00, 0x00,sxcase);//���ý��պͷ�����BUF�е���ʼ��ַ����Ϊ0�Ļ������ͺͽ��ն������������BUF��
    Radio.SetTxParams( TX_OUTPUT_POWER, RADIO_RAMP_02_US,sxcase );  //TX_OUTPUT_POWER����������ʱ�Ĺ���  RampTime:PA��Դ�򿪺�

    Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
    Radio.SetRx(TTtime_RX,sxcase );
    AppState[sxcase] = APP_LOWPOWER;  //��lowper?
}

/*******************************************************
*Function Name 	:SX1280_SENS_Pro
*Description  	:��1�����в��sx1280����״̬������AppState[0]�����з��ദ��
*Input			:sxcase:SX1280ͨ�����
*Output			:0:����
				 1:���յ�����
				 2:���ͳɹ�
				 3:���ճ�ʱ
				 4:���ճ���
				 5:���ͳ�ʱ
				 6:δ֪�ж�
*******************************************************/
uint8_t SX1280_SENS_Pro(uint8_t sxcase)
{
    uint8_t sx1280_state=0;	//������AppState[sxcase]��Ϊ����ֵ����Ϊ��ֵ�����ж��и��ĵ�;
    switch( AppState[sxcase] )
    {
    case APP_RX: //��ȷ���յ�����
        AppState[sxcase] = APP_LOWPOWER;
        if(Radio.GetPayload(&SX_Sens_RecBuf[sxcase][0], &SX_Sens_RecBufSize[sxcase],SX_SENS_RECBUF_SIZE,sxcase)) //ͨ������SX1280HalReadBuffer�����ѽ������ݷ���Buffer,�������ݳ��ȷ���BufferSize�����ܳ���BUFFER_SIZE
        {
            printf( "\n\r SX_0:Ping receive datatoo long!" );//�������ݳ��ȳ�����������󳤶�
        }
        else
        {
//            if(SX_Sens_RecBufSize[sxcase]!=SX_SENS_SNDBUF_SIZE)  //���Ȳ���
//            {
//                printf( "\n\r Ping receive data length is not 10!" );
//            }
//            else
//            {
//                //BSP_UART3_SendStr(SX_Sens_RecBuf,RecBufferSize);//ͨ��USRģ�鷢��
//                //WHL101_SendStr(SX_Sens_RecBuf,RecBufferSize);//ͨ��USRģ�鷢��
//                //EnQuene(SX_Sens_RecBuf);
//                //�������һ���ֽڱ�ʾ�ź�ǿ�ȣ���λ��dBm��,��������Ϊ��9D��������ʵ��Ϊ�з���������ʵ��Ϊ-99dBm
//                //SX1280_SENS_ReData_Pro(&SX_Sens_RecBuf[sxcase][0],SX_Sens_RecBufSize[sxcase],sxcase);
//                //	SX1280_SENS_ReData_Pro(u8 *Redata_pt,u8 Redata_Len,u8 sxcase)
//                //SX_Sens_RecBuf[sxcase][SX_Sens_RecBufSize[sxcase]]=Radio.GetRssiInst(sxcase);
//                //SX_Sens_RecBufSize[sxcase]++;
//                //BSP_UART1_SendStr(SX_Sens_RecBuf,RecBufferSize);//������
//            }
        }
        ////��������Ϊ����ģʽ����������Ӧ�ж�
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);
        lorasteptime[sxcase]=0;
        sx1280_state=1;
        break;
    case APP_TX: // ���ͳɹ�
        AppState[sxcase] = APP_LOWPOWER;
        //LED1(2);
        //printf( "\r\nSX_0:Send success!" );
        ////��������Ϊ����ģʽ����������Ӧ�ж�
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase );
		lorasteptime[sxcase]=0;
        sx1280_state=2;
        break;

    case APP_RX_TIMEOUT:  //���ճ�ʱ
        printf( "\r\nSX_0:RX timeout !" );
        AppState[sxcase] = APP_LOWPOWER;
		 lorasteptime[sxcase]=0;
        ////��������Ϊ����ģʽ����������Ӧ�ж�
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);//����Ϊ����ģʽ
        sx1280_state=3;
        break;

    case APP_RX_ERROR:
//        printf( "\r\nSX=%d:received a Packet with a CRC error!",sxcase);
//        delay_sx1280(1000);
        AppState[sxcase] = APP_LOWPOWER;
		 lorasteptime[sxcase]=0;
        // We have received a Packet with a CRC error, send reply as if packet was correct
        ////��������Ϊ����ģʽ����������Ӧ�ж�
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);//����Ϊ����ģʽ
        sx1280_state=4;
        break;

    case APP_TX_TIMEOUT:  //���ͳ�ʱ
        printf( "\r\nSX_0:TX timeout !" );
        AppState[sxcase] = APP_LOWPOWER;
        //��������
//        memcpy( SndBuffer, PongMsg0, SND_BUFFER_SIZE );
//        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,0);
//        Radio.SendPayload( SndBuffer, SND_BUFFER_SIZE,TTtime_TX,0 );
        sx1280_state=5;
        break;

     default: // APP_LOWPOWER:
        if(lorasteptime[sxcase]>100*1000)//����100s�����¼�������Ϊ����ģʽ
        {
            lorasteptime[sxcase]=0;
            ////���¼�������Ϊ����ģʽ����������Ӧ�ж�
            Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
            Radio.SetRx(TTtime_RX,sxcase);
        }
//        lorasteptime[sxcase]++;
        sx1280_state=0;
        break;
    }
    return sx1280_state;
}


/*******************************************************
*Function Name 	:SX1280_Data_Send
*Description  	:��1ͨ�����ͺ���������������ǰ���ڵ�1ͨ����ר�÷��ͻ�����SX_Sens_SndBuf[0]
*Input			:send_length���������ݳ���
*Output			:
*******************************************************/
void SX1280_Data_Send( uint8_t sxcase,uint16_t send_length)
{
    taskENTER_CRITICAL();
	Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
    SX1280HalWriteRegister( REG_LR_PAYLOADLENGTH,send_length,sxcase );//�䳤����
    Radio.SendPayload(SX_Sens_SndBuf[sxcase],send_length,TTtime_TX,sxcase);
	taskEXIT_CRITICAL();
}


/*******************************************************
*Function Name 	:SX1280_SENS_Pro
*Description  	: �ڶ���sx1280����״̬������AppState[1]�����з��ദ��
*Input					:
*Output					:
*******************************************************/
//void SX1280_SENS1_Pro(void)
//{
//    //uint8_t appi=0;
//    switch( AppState[1] )
//    {
//    case APP_RX: //��ȷ���յ�����
//        AppState[1] = APP_LOWPOWER;
//        RecBufferSize=REC_BUFFER_SIZE;
//        memset( &SX_Sens_RecBuf, 0x31, RecBufferSize );
//        if(Radio.GetPayload( SX_Sens_RecBuf, &RecBufferSize, REC_BUFFER_SIZE,1)==1) //ͨ������SX1280HalReadBuffer�����ѽ������ݷ���Buffer,�������ݳ��ȷ���BufferSize�����ܳ���BUFFER_SIZE
//        {
//            printf( "\n\r SX_1:Ping receive datatoo long!" );//�������ݳ��ȳ�����������󳤶�
//        }
//        else
//        {
//            printf( "\n\rSX_1: Ping receive data is :" );
//            BSP_UART1_SendStr(SX_Sens_RecBuf,RecBufferSize);
//        }
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1);
//        break;

//    case APP_TX: // ���ͳɹ�
//        AppState[1] = APP_LOWPOWER;
//        printf( "\r\nSX_1:Send success!" );
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1 );
//        Radio.SetRx(TTtime_TX,1 );
//        break;

//    case APP_RX_TIMEOUT:  //���ճ�ʱ
//        //printf( "\r\nSX_1:RX timeout !" );
//        AppState[1] = APP_LOWPOWER;
//        //�����ȴ�����
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1);
//        break;

//    case APP_RX_ERROR:
//        printf( "\r\nSX_1:received a Packet with a CRC error!" );
//        AppState[1] = APP_LOWPOWER;
//        // We have received a Packet with a CRC error, send reply as if packet was correct
//        //�����ȴ�����
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1 );
//        break;

//    case APP_TX_TIMEOUT:  //���ͳ�ʱ
//        printf( "\r\nSX_1:TX timeout !" );
//        AppState[1] = APP_LOWPOWER;
//        //��������
//        memcpy( SndBuffer, PongMsg1, SND_BUFFER_SIZE );
//        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SendPayload( SndBuffer, SND_BUFFER_SIZE,TTtime_TX,1 );
//        break;

//    case APP_LOWPOWER:
//        if(mainsteptime[1]>APPTIME_1)//����10s������һ������
//        {
//            mainsteptime[1]=0;
//            memcpy( SndBuffer, PongMsg1, SND_BUFFER_SIZE);  //SNEDDATASIZE
//            Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//            Radio.SendPayload( SndBuffer, SND_BUFFER_SIZE,TTtime_TX,1);
//        }
//        break;
//    default:
//        // Set low power
//        break;
//    }
//}







