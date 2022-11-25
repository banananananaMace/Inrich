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

uint32_t lorasteptime[3]= {0,0,0}; //全局秒计时器，在systick中加一(1ms)；

TickTime_t TTtime_RX= {RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_SINGLEMODE};  //SIZE*VALUE=
TickTime_t TTtime_TX= {TX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE};  //SIZE*VALUE=

void delay_sx1280(u32 nms)
{
    u32 i;
    for(i=0; i<nms; i++) ;
}

/*******************************************************
*Function Name 	:SX1280_Param_Init
*Description  	: sx1280初始化
*Input			: sxcase：[0,3]表示第几个sx1280，原设计共有4个
*Output			:
*******************************************************/
uint16_t SX1280_ID=0;
void SX1280_Param_Init(uint8_t sxcase,uint32_t frequency)
{
    uint32_t freq;
	freq = 2399500000 + frequency * 1000000;
	
	ModulationParams_t modulationParams;
    PacketParams_t packetParams;
    Radio.Init(sxcase);			//外部中断配置
    //SX1280HalReset(sxcase);	//reset管脚复位，拉低再拉高
    Radio.SetRegulatorMode( USE_LDO,sxcase); // LDO:电感省略，但功耗上升（成本降低）；DCDC:需要电感，功耗降低（成本上升）
    SX1280_ID=Radio.GetFirmwareVersion(sxcase);
    HAL_Delay(100);
    printf( "\r\nRadio firmware version of SX1280_[%u] is:0x%x", sxcase, Radio.GetFirmwareVersion(sxcase) );
    //LORa Mode  beging//
    modulationParams.PacketType = PACKET_TYPE_LORA;  						//设置为RoLa模式
    modulationParams.Params.LoRa.SpreadingFactor =LORA_SF8;//LORA_SF10 // LORA_SF12;  	//扩频因子
    modulationParams.Params.LoRa.Bandwidth = LORA_BW_0800;//LORA_BW_1600;    	//带宽
    modulationParams.Params.LoRa.CodingRate = LORA_CR_4_5;//LORA_CR_LI_4_7;		//码率
    packetParams.PacketType = PACKET_TYPE_LORA;
    packetParams.Params.LoRa.PreambleLength = 6;//12;
    packetParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH; // LORA_PACKET_FIXED_LENGTH;//LORA_PACKET_VARIABLE_LENGTH;
    packetParams.Params.LoRa.PayloadLength = SX_SENS_SNDBUF_SIZE;       //每次发送的字节数量
    packetParams.Params.LoRa.CrcMode = LORA_CRC_ON;//LORA_CRC_OFF;
    packetParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;
    //LORa Mode  end//
    Radio.SetStandby( STDBY_RC,sxcase);
    Radio.SetPacketType( modulationParams.PacketType,sxcase);
    Radio.SetModulationParams( &modulationParams,sxcase );
//    Radio.WriteRegister( 0x925,0x32,sxcase );  //根据官方芯片资料P112页说明，要求在设置完不同扩频因子后，执行本次寄存器写命令；
    Radio.WriteRegister( 0x925,0x37,sxcase );  //根据官方芯片资料P112页说明，要求在设置完不同扩频因子后，执行本次寄存器写命令；
    Radio.SetPacketParams( &packetParams,sxcase );	
    Radio.SetRfFrequency( freq,sxcase );  //设置频点
    Radio.SetBufferBaseAddresses( 0x00, 0x00,sxcase);//设置接收和发送在BUF中的起始地址：都为0的话，发送和接收都可以最大化利用BUF；
    Radio.SetTxParams( TX_OUTPUT_POWER, RADIO_RAMP_02_US,sxcase );  //TX_OUTPUT_POWER：发射数据时的功率  RampTime:PA电源打开后

    Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
    Radio.SetRx(TTtime_RX,sxcase );
    AppState[sxcase] = APP_LOWPOWER;  //是lowper?
}

/*******************************************************
*Function Name 	:SX1280_SENS_Pro
*Description  	:第1个传感侧的sx1280根据状态变量即AppState[0]，进行分类处理
*Input			:sxcase:SX1280通道编号
*Output			:0:空闲
				 1:接收到数据
				 2:发送成功
				 3:接收超时
				 4:接收出错
				 5:发送超时
				 6:未知中断
*******************************************************/
uint8_t SX1280_SENS_Pro(uint8_t sxcase)
{
    uint8_t sx1280_state=0;	//不能用AppState[sxcase]作为返回值，因为该值是在中断中更改的;
    switch( AppState[sxcase] )
    {
    case APP_RX: //正确接收到数据
        AppState[sxcase] = APP_LOWPOWER;
        if(Radio.GetPayload(&SX_Sens_RecBuf[sxcase][0], &SX_Sens_RecBufSize[sxcase],SX_SENS_RECBUF_SIZE,sxcase)) //通过调用SX1280HalReadBuffer函数把接收数据放在Buffer,接收数据长度放在BufferSize，不能超过BUFFER_SIZE
        {
            printf( "\n\r SX_0:Ping receive datatoo long!" );//接收数据长度超过缓冲区最大长度
        }
        else
        {
//            if(SX_Sens_RecBufSize[sxcase]!=SX_SENS_SNDBUF_SIZE)  //长度不对
//            {
//                printf( "\n\r Ping receive data length is not 10!" );
//            }
//            else
//            {
//                //BSP_UART3_SendStr(SX_Sens_RecBuf,RecBufferSize);//通过USR模块发送
//                //WHL101_SendStr(SX_Sens_RecBuf,RecBufferSize);//通过USR模块发送
//                //EnQuene(SX_Sens_RecBuf);
//                //加上最后一个字节表示信号强度，单位（dBm）,例如数据为：9D，该数据实际为有符号数，则实际为-99dBm
//                //SX1280_SENS_ReData_Pro(&SX_Sens_RecBuf[sxcase][0],SX_Sens_RecBufSize[sxcase],sxcase);
//                //	SX1280_SENS_ReData_Pro(u8 *Redata_pt,u8 Redata_Len,u8 sxcase)
//                //SX_Sens_RecBuf[sxcase][SX_Sens_RecBufSize[sxcase]]=Radio.GetRssiInst(sxcase);
//                //SX_Sens_RecBufSize[sxcase]++;
//                //BSP_UART1_SendStr(SX_Sens_RecBuf,RecBufferSize);//调试用
//            }
        }
        ////继续设置为接收模式，并设置相应中断
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);
        lorasteptime[sxcase]=0;
        sx1280_state=1;
        break;
    case APP_TX: // 发送成功
        AppState[sxcase] = APP_LOWPOWER;
        //LED1(2);
        //printf( "\r\nSX_0:Send success!" );
        ////继续设置为接收模式，并设置相应中断
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase );
		lorasteptime[sxcase]=0;
        sx1280_state=2;
        break;

    case APP_RX_TIMEOUT:  //接收超时
        printf( "\r\nSX_0:RX timeout !" );
        AppState[sxcase] = APP_LOWPOWER;
		 lorasteptime[sxcase]=0;
        ////继续设置为接收模式，并设置相应中断
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);//设置为接收模式
        sx1280_state=3;
        break;

    case APP_RX_ERROR:
//        printf( "\r\nSX=%d:received a Packet with a CRC error!",sxcase);
//        delay_sx1280(1000);
        AppState[sxcase] = APP_LOWPOWER;
		 lorasteptime[sxcase]=0;
        // We have received a Packet with a CRC error, send reply as if packet was correct
        ////继续设置为接收模式，并设置相应中断
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
        Radio.SetRx(TTtime_RX,sxcase);//设置为接收模式
        sx1280_state=4;
        break;

    case APP_TX_TIMEOUT:  //发送超时
        printf( "\r\nSX_0:TX timeout !" );
        AppState[sxcase] = APP_LOWPOWER;
        //继续发送
//        memcpy( SndBuffer, PongMsg0, SND_BUFFER_SIZE );
//        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,0);
//        Radio.SendPayload( SndBuffer, SND_BUFFER_SIZE,TTtime_TX,0 );
        sx1280_state=5;
        break;

     default: // APP_LOWPOWER:
        if(lorasteptime[sxcase]>100*1000)//超过100s，重新继续设置为接收模式
        {
            lorasteptime[sxcase]=0;
            ////重新继续设置为接收模式，并设置相应中断
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
*Description  	:第1通道发送函数，发送数据提前放在第1通道的专用发送缓冲区SX_Sens_SndBuf[0]
*Input			:send_length：发送数据长度
*Output			:
*******************************************************/
void SX1280_Data_Send( uint8_t sxcase,uint16_t send_length)
{
    taskENTER_CRITICAL();
	Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,sxcase);
    SX1280HalWriteRegister( REG_LR_PAYLOADLENGTH,send_length,sxcase );//变长所需
    Radio.SendPayload(SX_Sens_SndBuf[sxcase],send_length,TTtime_TX,sxcase);
	taskEXIT_CRITICAL();
}


/*******************************************************
*Function Name 	:SX1280_SENS_Pro
*Description  	: 第二个sx1280根据状态变量即AppState[1]，进行分类处理
*Input					:
*Output					:
*******************************************************/
//void SX1280_SENS1_Pro(void)
//{
//    //uint8_t appi=0;
//    switch( AppState[1] )
//    {
//    case APP_RX: //正确接收到数据
//        AppState[1] = APP_LOWPOWER;
//        RecBufferSize=REC_BUFFER_SIZE;
//        memset( &SX_Sens_RecBuf, 0x31, RecBufferSize );
//        if(Radio.GetPayload( SX_Sens_RecBuf, &RecBufferSize, REC_BUFFER_SIZE,1)==1) //通过调用SX1280HalReadBuffer函数把接收数据放在Buffer,接收数据长度放在BufferSize，不能超过BUFFER_SIZE
//        {
//            printf( "\n\r SX_1:Ping receive datatoo long!" );//接收数据长度超过缓冲区最大长度
//        }
//        else
//        {
//            printf( "\n\rSX_1: Ping receive data is :" );
//            BSP_UART1_SendStr(SX_Sens_RecBuf,RecBufferSize);
//        }
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1);
//        break;

//    case APP_TX: // 发送成功
//        AppState[1] = APP_LOWPOWER;
//        printf( "\r\nSX_1:Send success!" );
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1 );
//        Radio.SetRx(TTtime_TX,1 );
//        break;

//    case APP_RX_TIMEOUT:  //接收超时
//        //printf( "\r\nSX_1:RX timeout !" );
//        AppState[1] = APP_LOWPOWER;
//        //继续等待接收
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1);
//        break;

//    case APP_RX_ERROR:
//        printf( "\r\nSX_1:received a Packet with a CRC error!" );
//        AppState[1] = APP_LOWPOWER;
//        // We have received a Packet with a CRC error, send reply as if packet was correct
//        //继续等待接收
//        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SetRx(TTtime_RX,1 );
//        break;

//    case APP_TX_TIMEOUT:  //发送超时
//        printf( "\r\nSX_1:TX timeout !" );
//        AppState[1] = APP_LOWPOWER;
//        //继续发送
//        memcpy( SndBuffer, PongMsg1, SND_BUFFER_SIZE );
//        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE,1);
//        Radio.SendPayload( SndBuffer, SND_BUFFER_SIZE,TTtime_TX,1 );
//        break;

//    case APP_LOWPOWER:
//        if(mainsteptime[1]>APPTIME_1)//超过10s，发送一次数据
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







