#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"
#include "stdio.h"
#include <stdbool.h>
//////////////////////////////////////////////////////////////////////////////////

//#define BCSCAN_WAITTIME_MAX		2000*4		//4次广播周期
//#define BC_SLOT_ORDER_MAX		10			//BC帧最大时隙数，可以和汇聚节点数相当
//#define EVENTBIT_SX1280_MAX_DELAY ( TickType_t ) 	1000   // 1s	0xffffffffUL
//#define FIRMWARE_VERSION    ( ( char* )"Firmware Version: " )
extern uint8_t FWVersion[4];
extern uint8_t HWVersion[4];
/**队列参数**/
#define SX1280_RData_Q_NUM    	5  							//信道接收数据队列的消息数量  
#define SX1280_RData_Q_LENTH  SX_SENS_RECBUF_SIZE  			//信道接收数据队列每个消息的长度 

#define Uart1Tx_MessageData_Q_NUM 	10						//串口1发送数据的消息队列的消息数量
#define Uart1Tx_MessageData_Q_LENTH  USART1_TX_LEN			//串口1发送数据的消息队列每个消息的长度
#define Uart1Rx_MessageData_Q_NUM 	10						//串口1接收数据的消息队列的消息数量
#define Uart1Rx_MessageData_Q_LENTH USART1_REC_LEN			//串口1接收数据的消息队列每个消息的长度

#define Uart2Tx_MessageData_Q_NUM 	5						//串口2发送数据的消息队列的消息数量
#define Uart2Tx_MessageData_Q_LENTH  USART2_TX_LEN			//串口2发送数据的消息队列每个消息的长度
#define Uart2Rx_MessageData_Q_NUM 	5						//串口2接收数据的消息队列的消息数量
#define Uart2Rx_MessageData_Q_LENTH USART2_REC_LEN			//串口2接收数据的消息队列每个消息的长度

#define Uart4Tx_MessageData_Q_NUM 	3						//串口4发送数据的消息队列的消息数量
#define Uart4Tx_MessageData_Q_LENTH  USART4_TX_LEN			//串口4发送数据的消息队列每个消息的长度
#define Uart4Rx_MessageData_Q_NUM 	3						//串口4接收数据的消息队列的消息数量
#define Uart4Rx_MessageData_Q_LENTH USART4_REC_LEN			//串口4接收数据的消息队列每个消息的长度

#define RegSensorID_MessageData_Q_NUM 	30					//需注册ID的消息队列的消息数量(不能超过35)
#define RegSensorID_MessageData_Q_LENTH 6					//需注册ID的消息队列每个消息的长度



//////////SX1280接收中断事件组的事件标志位定义///////////////////////////////
//#define EVENTBIT_SX1280_NODE0_ISR (1<<0)    //SX1280_0的中断标志
//#define EVENTBIT_SX1280_NODE1_ISR (1<<1)    //SX1280_1的中断标志
//#define EVENTBIT_SX1280_NODE2_ISR (1<<2)    //SX1280_2的中断标志
#define EVENTBIT_SX1280_SENS0_ISR (1<<3)    //SX1280_3的中断标志
#define EVENTBIT_SX1280_SENS1_ISR (1<<4)    //SX1280_4的中断标志
#define EVENTBIT_SX1280_SENS2_ISR (1<<5)    //SX1280_5的中断标志
//SX1280中断事件组集合
#define EVENTBIT_SX1280_ALL	(EVENTBIT_SX1280_SENS0_ISR|EVENTBIT_SX1280_SENS1_ISR|EVENTBIT_SX1280_SENS2_ISR)
#define EVENTBIT_BC_MultiRX 			(1<<6)    //多次接收到BC帧标志
#define EVENTBIT_REGISTER_ACK			(1<<8)    //控制信道接收到注册响应帧标志

//////////看门狗事件组的事件标志位定义///////////////////////////////
#define EVENTBIT_SX1280RxData_TASK 	(1<<0)    //SX1280数据接收任务标志
#define EVENTBIT_SenCTR_CH_TASK 	(1<<1)    //控制信道任务标志
#define EVENTBIT_SenBS_CH1_TASK 	(1<<2)    //业务信道任务标志
#define EVENTBIT_USART1_Rx_Task 	(1<<3)    //串口1任务标志
#define EVENTBIT_USART2_Rx_Task 	(1<<4)    //串口2任务标志
#define EVENTBIT_USART1_Tx_Task 	(1<<5)    //串口1任务标志
#define EVENTBIT_USART2_Tx_Task 	(1<<6)    //串口2任务标志
//SX1280中断事件组集合
#define EVENTBIT_IWDG_ALL	(EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART1_Rx_Task|EVENTBIT_USART2_Rx_Task|EVENTBIT_USART1_Tx_Task|EVENTBIT_USART2_Tx_Task)
#define EVENTBIT_IWDG_UART1 (EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART1_Tx_Task)
#define EVENTBIT_IWDG_UART2 (EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART2_Tx_Task)

//////////常用事件组的事件标志位定义///////////////////////////////
#define EVENTBIT_SoftReset_Flag	(1<<0)    	  //接收到重启命令标志

#endif
