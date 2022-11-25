#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////
//��������:2019/7/26
//�汾��V1.0
//Copyright(C) Inrich
//����������
////////////////////////////////////////////////////////////////////////////////// 	

//�궨��
#define UART1_RX_DMA_Stream		DMA2_Stream5   //Uart1_RX����DMA2��ͨ��4��������5
#define UART1_RX_DMA_Channel 	DMA_Channel_4
#define UART1_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF5	//TCIF5:��ʾ������5�Ľ�����ɱ�־
#define UART1_TX_DMA_Stream		DMA2_Stream7   //Uart1_TX����DMA2��ͨ��4��������6
#define UART1_TX_DMA_Channel 	DMA_Channel_4
#define UART1_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF7	//TCIF7:��ʾ������7�ķ�����ɱ�־


#define UART2_RX_DMA_Stream		DMA1_Stream5   //Uart2_RX����DMA1��ͨ��4��������5
#define UART2_RX_DMA_Channel 	DMA_Channel_4
#define UART2_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF5	//TCIF5:��ʾ������5�Ľ�����ɱ�־
#define UART2_TX_DMA_Stream		DMA1_Stream6   //Uart2_TX����DMA1��ͨ��4��������6
#define UART2_TX_DMA_Channel 	DMA_Channel_4
#define UART2_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF6	//TCIF6:��ʾ������6�ķ�����ɱ�־

#define UART4_RX_DMA_Stream		DMA1_Stream2   //Uart4_RX����DMA1��ͨ��4��������2
#define UART4_RX_DMA_Channel 	DMA_Channel_4
#define UART4_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF2	//TCIF2:��ʾ������2�Ľ�����ɱ�־
#define UART4_TX_DMA_Stream		DMA1_Stream4   //Uart2_TX����DMA1��ͨ��4��������4
#define UART4_TX_DMA_Channel 	DMA_Channel_4
#define UART4_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF4	//TCIF4:��ʾ������4�ķ�����ɱ�־

//����1,2���������ͬ���ȣ������뷢����Ҫ����Ϊ��ͬ����
#define SDU_MAXLenth			1400		//�����շ����峤����Ҫ����SDU�ĳ���
#define USART1_REC_LEN  		1450		//�����������ֽ��� 200
#define USART1_TX_LEN  			1450			//�����������ֽ��� 200 ��Ҫ�� SX_SENS_RECBUF_SIZE һ��
#define USART2_REC_LEN  		1450 		//�����������ֽ��� 200
#define USART2_TX_LEN  			1450  		//�����������ֽ��� 200 ��Ҫ�� SX_SENS_RECBUF_SIZE һ��
#define USART4_REC_LEN  		270  		//�����������ֽ��� 200
#define USART4_TX_LEN  			270  		//�����������ֽ��� 200 ��Ҫ�� SX_SENS_RECBUF_SIZE һ��

#define EN_USART1_RX 			1			//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX_TX 		1			//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART4_RX_TX 		1			//ʹ�ܣ�1��/��ֹ��0������1����

#define COM1	1
#define COM2	2
#define COM4	4

//��������
extern u8  USART1_RX_BUF[USART1_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART2_RX_BUF[USART2_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART4_RX_BUF[USART4_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

extern u8 USART1_TX_BUF[USART1_TX_LEN];   	//���ͻ���,���USART_REC_LEN���ֽ�.
extern u8 USART2_TX_BUF[USART2_TX_LEN];   	//���ͻ���,���USART_REC_LEN���ֽ�.
extern u8 USART4_TX_BUF[USART4_TX_LEN];   	//���ͻ���,���USART_REC_LEN���ֽ�.

extern u16 USART1_TX_Lenth;       			//ÿ����Ҫ�������ݵĳ���
extern u16 USART2_TX_Lenth;       			//ÿ����Ҫ�������ݵĳ���
extern u16 USART4_TX_Lenth;       			//ÿ����Ҫ�������ݵĳ���

//��������
extern void uart1_init(u32 bound);
extern void USART1_Sendbyte(u8 snddata);
extern void USART1_Sendbuf(u8 *sndbuf,u8 buflen);

extern void uart2_init(u32 bound);
extern void USART2_Sendbyte(u8 snddata);
extern void USART2_Sendbuf(u8 *sndbuf,u8 buflen);

extern void uart4_init(u32 bound);

#endif


