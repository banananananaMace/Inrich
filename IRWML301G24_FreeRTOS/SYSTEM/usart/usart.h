#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////
//创建日期:2019/7/26
//版本：V1.0
//Copyright(C) Inrich
//功能描述：
////////////////////////////////////////////////////////////////////////////////// 	

//宏定义
#define UART1_RX_DMA_Stream		DMA2_Stream5   //Uart1_RX属于DMA2的通道4，数据流5
#define UART1_RX_DMA_Channel 	DMA_Channel_4
#define UART1_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF5	//TCIF5:表示数据流5的接收完成标志
#define UART1_TX_DMA_Stream		DMA2_Stream7   //Uart1_TX属于DMA2的通道4，数据流6
#define UART1_TX_DMA_Channel 	DMA_Channel_4
#define UART1_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF7	//TCIF7:表示数据流7的发送完成标志


#define UART2_RX_DMA_Stream		DMA1_Stream5   //Uart2_RX属于DMA1的通道4，数据流5
#define UART2_RX_DMA_Channel 	DMA_Channel_4
#define UART2_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF5	//TCIF5:表示数据流5的接收完成标志
#define UART2_TX_DMA_Stream		DMA1_Stream6   //Uart2_TX属于DMA1的通道4，数据流6
#define UART2_TX_DMA_Channel 	DMA_Channel_4
#define UART2_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF6	//TCIF6:表示数据流6的发送完成标志

#define UART4_RX_DMA_Stream		DMA1_Stream2   //Uart4_RX属于DMA1的通道4，数据流2
#define UART4_RX_DMA_Channel 	DMA_Channel_4
#define UART4_RX_DMA_FLAG_TCIF	DMA_FLAG_TCIF2	//TCIF2:表示数据流2的接收完成标志
#define UART4_TX_DMA_Stream		DMA1_Stream4   //Uart2_TX属于DMA1的通道4，数据流4
#define UART4_TX_DMA_Channel 	DMA_Channel_4
#define UART4_TX_DMA_FLAG_TCIF	DMA_FLAG_TCIF4	//TCIF4:表示数据流4的发送完成标志

//串口1,2最好设置相同长度；接收与发送需要设置为相同长度
#define SDU_MAXLenth			1400		//串口收发缓冲长度需要大于SDU的长度
#define USART1_REC_LEN  		1450		//定义最大接收字节数 200
#define USART1_TX_LEN  			1450			//定义最大接收字节数 200 需要和 SX_SENS_RECBUF_SIZE 一致
#define USART2_REC_LEN  		1450 		//定义最大接收字节数 200
#define USART2_TX_LEN  			1450  		//定义最大接收字节数 200 需要和 SX_SENS_RECBUF_SIZE 一致
#define USART4_REC_LEN  		270  		//定义最大接收字节数 200
#define USART4_TX_LEN  			270  		//定义最大接收字节数 200 需要和 SX_SENS_RECBUF_SIZE 一致

#define EN_USART1_RX 			1			//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX_TX 		1			//使能（1）/禁止（0）串口1接收
#define EN_USART4_RX_TX 		1			//使能（1）/禁止（0）串口1接收

#define COM1	1
#define COM2	2
#define COM4	4

//变量申明
extern u8  USART1_RX_BUF[USART1_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART2_RX_BUF[USART2_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART4_RX_BUF[USART4_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 

extern u8 USART1_TX_BUF[USART1_TX_LEN];   	//发送缓冲,最大USART_REC_LEN个字节.
extern u8 USART2_TX_BUF[USART2_TX_LEN];   	//发送缓冲,最大USART_REC_LEN个字节.
extern u8 USART4_TX_BUF[USART4_TX_LEN];   	//发送缓冲,最大USART_REC_LEN个字节.

extern u16 USART1_TX_Lenth;       			//每次需要发送数据的长度
extern u16 USART2_TX_Lenth;       			//每次需要发送数据的长度
extern u16 USART4_TX_Lenth;       			//每次需要发送数据的长度

//函数申明
extern void uart1_init(u32 bound);
extern void USART1_Sendbyte(u8 snddata);
extern void USART1_Sendbuf(u8 *sndbuf,u8 buflen);

extern void uart2_init(u32 bound);
extern void USART2_Sendbyte(u8 snddata);
extern void USART2_Sendbuf(u8 *sndbuf,u8 buflen);

extern void uart4_init(u32 bound);

#endif


