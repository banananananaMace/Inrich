#ifndef _SX1280_APP_H
#define _SX1280_APP_H
#include "sys.h"
#include "stdio.h"

//brief Used to display firmware version UART flow
#define MODE_LORA
#define RF_BL_ADV_CHANNEL_38                        2426000000////2426000000我 //2484000000公司 Hz
//brief Defines the nominal frequency/
#define RF_FREQUENCY                                RF_BL_ADV_CHANNEL_38 // Hz


//brief Defines the output power in dBm，//remark The range of the output power is [-18..+13] dBm/
#define TX_OUTPUT_POWER                         13
//brief Defines the buffer size, i.e. the payload size/
#define SX_SENS_RECBUF_SIZE                     250
#define SX_SENS_SNDBUF_SIZE                     250
//brief Number of tick size steps for tx timeout/
#define TX_TIMEOUT_VALUE                            1000 // 1000*1ms=1s
#define TX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_1000_US   //RADIO_TICK_SIZE_1000_US

//brief Number of tick size steps for rx timeout/
#define RX_TIMEOUT_VALUE                            5000 // 5000*1ms=5s
#define RX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_1000_US   //RADIO_TICK_SIZE_1000_US=1ms
#define RX_TIMEOUT_SINGLEMODE                       0000 // 5000*1ms=5s

#define SX1280_CH_NUM  3	//当前SX1280的最大通道数量

//各个SX1280重新设置状态的周期
#define APPTIME_0  360000
#define APPTIME_1  20000
#define APPTIME_2  20000
#define APPTIME_3  20000

extern uint8_t SX_Sens_RecBufSize[SX1280_CH_NUM];
//extern uint8_t SX_Sens_RecBuf[SX_SENS_RECBUF_SIZE];
extern uint8_t SX_Sens_RecBuf[SX1280_CH_NUM][SX_SENS_RECBUF_SIZE];
extern uint8_t SX_Sens_SndBufSize ;
extern uint8_t SX_Sens_SndBuf[SX1280_CH_NUM][SX_SENS_SNDBUF_SIZE];


//extern void SX1280_Param_Init(uint8_t sxcase);

extern void SX1280_Param_Init(uint8_t sxcase,uint32_t frequency);
extern uint8_t SX1280_SENS_Pro(uint8_t sxcase);
extern void SX1280_Data_Send( uint8_t sxcase,uint16_t send_length);
#endif
