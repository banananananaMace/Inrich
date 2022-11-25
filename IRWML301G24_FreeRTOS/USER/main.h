#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"
#include "stdio.h"
#include <stdbool.h>
//////////////////////////////////////////////////////////////////////////////////

//#define BCSCAN_WAITTIME_MAX		2000*4		//4�ι㲥����
//#define BC_SLOT_ORDER_MAX		10			//BC֡���ʱ϶�������Ժͻ�۽ڵ����൱
//#define EVENTBIT_SX1280_MAX_DELAY ( TickType_t ) 	1000   // 1s	0xffffffffUL
//#define FIRMWARE_VERSION    ( ( char* )"Firmware Version: " )
extern uint8_t FWVersion[4];
extern uint8_t HWVersion[4];
/**���в���**/
#define SX1280_RData_Q_NUM    	5  							//�ŵ��������ݶ��е���Ϣ����  
#define SX1280_RData_Q_LENTH  SX_SENS_RECBUF_SIZE  			//�ŵ��������ݶ���ÿ����Ϣ�ĳ��� 

#define Uart1Tx_MessageData_Q_NUM 	10						//����1�������ݵ���Ϣ���е���Ϣ����
#define Uart1Tx_MessageData_Q_LENTH  USART1_TX_LEN			//����1�������ݵ���Ϣ����ÿ����Ϣ�ĳ���
#define Uart1Rx_MessageData_Q_NUM 	10						//����1�������ݵ���Ϣ���е���Ϣ����
#define Uart1Rx_MessageData_Q_LENTH USART1_REC_LEN			//����1�������ݵ���Ϣ����ÿ����Ϣ�ĳ���

#define Uart2Tx_MessageData_Q_NUM 	5						//����2�������ݵ���Ϣ���е���Ϣ����
#define Uart2Tx_MessageData_Q_LENTH  USART2_TX_LEN			//����2�������ݵ���Ϣ����ÿ����Ϣ�ĳ���
#define Uart2Rx_MessageData_Q_NUM 	5						//����2�������ݵ���Ϣ���е���Ϣ����
#define Uart2Rx_MessageData_Q_LENTH USART2_REC_LEN			//����2�������ݵ���Ϣ����ÿ����Ϣ�ĳ���

#define Uart4Tx_MessageData_Q_NUM 	3						//����4�������ݵ���Ϣ���е���Ϣ����
#define Uart4Tx_MessageData_Q_LENTH  USART4_TX_LEN			//����4�������ݵ���Ϣ����ÿ����Ϣ�ĳ���
#define Uart4Rx_MessageData_Q_NUM 	3						//����4�������ݵ���Ϣ���е���Ϣ����
#define Uart4Rx_MessageData_Q_LENTH USART4_REC_LEN			//����4�������ݵ���Ϣ����ÿ����Ϣ�ĳ���

#define RegSensorID_MessageData_Q_NUM 	30					//��ע��ID����Ϣ���е���Ϣ����(���ܳ���35)
#define RegSensorID_MessageData_Q_LENTH 6					//��ע��ID����Ϣ����ÿ����Ϣ�ĳ���



//////////SX1280�����ж��¼�����¼���־λ����///////////////////////////////
//#define EVENTBIT_SX1280_NODE0_ISR (1<<0)    //SX1280_0���жϱ�־
//#define EVENTBIT_SX1280_NODE1_ISR (1<<1)    //SX1280_1���жϱ�־
//#define EVENTBIT_SX1280_NODE2_ISR (1<<2)    //SX1280_2���жϱ�־
#define EVENTBIT_SX1280_SENS0_ISR (1<<3)    //SX1280_3���жϱ�־
#define EVENTBIT_SX1280_SENS1_ISR (1<<4)    //SX1280_4���жϱ�־
#define EVENTBIT_SX1280_SENS2_ISR (1<<5)    //SX1280_5���жϱ�־
//SX1280�ж��¼��鼯��
#define EVENTBIT_SX1280_ALL	(EVENTBIT_SX1280_SENS0_ISR|EVENTBIT_SX1280_SENS1_ISR|EVENTBIT_SX1280_SENS2_ISR)
#define EVENTBIT_BC_MultiRX 			(1<<6)    //��ν��յ�BC֡��־
#define EVENTBIT_REGISTER_ACK			(1<<8)    //�����ŵ����յ�ע����Ӧ֡��־

//////////���Ź��¼�����¼���־λ����///////////////////////////////
#define EVENTBIT_SX1280RxData_TASK 	(1<<0)    //SX1280���ݽ��������־
#define EVENTBIT_SenCTR_CH_TASK 	(1<<1)    //�����ŵ������־
#define EVENTBIT_SenBS_CH1_TASK 	(1<<2)    //ҵ���ŵ������־
#define EVENTBIT_USART1_Rx_Task 	(1<<3)    //����1�����־
#define EVENTBIT_USART2_Rx_Task 	(1<<4)    //����2�����־
#define EVENTBIT_USART1_Tx_Task 	(1<<5)    //����1�����־
#define EVENTBIT_USART2_Tx_Task 	(1<<6)    //����2�����־
//SX1280�ж��¼��鼯��
#define EVENTBIT_IWDG_ALL	(EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART1_Rx_Task|EVENTBIT_USART2_Rx_Task|EVENTBIT_USART1_Tx_Task|EVENTBIT_USART2_Tx_Task)
#define EVENTBIT_IWDG_UART1 (EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART1_Tx_Task)
#define EVENTBIT_IWDG_UART2 (EVENTBIT_SenBS_CH1_TASK|EVENTBIT_USART2_Tx_Task)

//////////�����¼�����¼���־λ����///////////////////////////////
#define EVENTBIT_SoftReset_Flag	(1<<0)    	  //���յ����������־

#endif
