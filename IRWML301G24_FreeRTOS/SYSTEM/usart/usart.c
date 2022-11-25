#include "sys.h"
#include "usart.h"
#include "string.h"
//#include "absacc.h"

//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用
#include "queue.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
//创建日期:2019/7/26
//版本：V1.0
//Copyright(C) Inrich
//功能描述：
////////////////////////////////////////////////////////////////////////////////// 
extern QueueHandle_t Uart1Rx_MessageData_Q;   				//串口1接收数据的消息队列句柄
extern QueueHandle_t Uart2Rx_MessageData_Q;   				//串口2接收数据的消息队列句柄
extern QueueHandle_t Uart4Rx_MessageData_Q;   				//串口4接收数据的消息队列句柄

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((UART4->SR&0X40)==0);//循环发送,直到发送完毕
		UART4->DR = (u8) ch;

//  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//	USART_SendData(USART1, snddata);         //向串口1发送数据
    return ch;
}
#endif


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART1_RX_BUF[USART1_REC_LEN]  ;     //接收缓冲,最大USART_REC_LEN个字节.  __attribute__((at(0x20010000)))  volatile
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART4_RX_BUF[USART4_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

u8 USART1_TX_BUF[USART1_TX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART2_TX_BUF[USART2_TX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART4_TX_BUF[USART4_TX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

u16 USART1_TX_Lenth=0;       		//每次需要发送数据的长度
u16 USART2_TX_Lenth=0;       		//每次需要发送数据的长度
u16 USART4_TX_Lenth=0;       		//每次需要发送数据的长度

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
//u16 USART1_RX_STA=0;       //接收状态标记
//u16 USART2_RX_STA=0;       //接收状态标记



/*******************************************************
*Function Name	:uart1_init
*Description  	:初始化串口1
*Input		  	:bound:波特率
*Output		  	:
*Question	  	:1、
*******************************************************/
void uart1_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; 					//GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//推挽复用输出,RX口应该设置为输入：GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//初始化PA9，PA3
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; 				//GPIOA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1

#if EN_USART1_RX
	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;//抢占优先级7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 			//开启总线空闲中断
#endif
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  		//开启串口DMA接收	
	USART_Cmd(USART1, ENABLE);  //使能串口1
}

/*******************************************************
*Function Name	:USART1_IRQHandler
*Description  	:串口1中断函数
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0; 
	
    //接收总线空闲中断，即可以
    if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
    {
        Res = USART1->SR;
        Res =USART_ReceiveData(USART1) ; 							//读数据，同时清USART_IT_IDLE标志，USART1->DR
        DMA_Cmd(UART1_RX_DMA_Stream,DISABLE);    					//关闭DMA
		Res = USART1_REC_LEN -  DMA_GetCurrDataCounter(UART1_RX_DMA_Stream);  //得到真正接收数据个数        
		USART1_RX_BUF[USART1_REC_LEN-2]=Res>>8;						//在数组的最后2个字节保存该次串口接收数据长度；
		USART1_RX_BUF[USART1_REC_LEN-1]=Res&0xFF;						//在数组的最后2个字节保存该次串口接收数据长度；
		Result=xQueueSendFromISR(Uart1Rx_MessageData_Q,USART1_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART1_RX_BUF,0,USART1_REC_LEN);
        DMA_ClearFlag(UART1_RX_DMA_Stream,UART1_RX_DMA_FLAG_TCIF);		//清除接收完成标志,必须清除，否则下次DMA不会接收
		//DMA_SetCurrDataCounter(UART1_RX_DMA_Stream,USART_REC_LEN);  	//重新设置接收数据个数 :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART1_RX_DMA_Stream,ENABLE);  							//开启DMA
        // receive_flag = 1;           								//接收数据标志位置1
		if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}
/*******************************************************
*Function Name	:USART1_Sendbyte
*Description  	:串口1发送单个字节
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART1_Sendbyte(u8 snddata)
{
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
    USART_SendData(USART1, snddata);         //向串口1发送数据
}
/*******************************************************
*Function Name	:USART1_Sendbuf
*Description  	:串口1发送多个字节
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART1_Sendbuf(u8 *sndbuf,u8 buflen)
{
    while(buflen--)
    {
        USART1_Sendbyte(*sndbuf++);
    }
}

/*******************************************************
*Function Name	:uart2_init
*Description  	:初始化串口1
*Input		  	:bound:波特率
*Output		  	:
*Question	  	:1、
*******************************************************/
void uart2_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//使能USART2时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 	//GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 	//GPIOA10复用为USART1

    //USART2端口配置
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; 					//GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//推挽复用输出,RX口应该设置为输入：GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//初始化PA2，PA3
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ; 					//GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//复用功能    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	

    //USART2 初始化设置
    USART_InitStructure.USART_BaudRate = bound;						//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); 						//初始化串口2


#if EN_USART2_RX_TX
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=11;	//抢占优先级7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

    //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//开启接收中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 			//开启总线空闲中断
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);			//开启发送中断（发送寄存器空）,发送时再开启
#endif

    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  		//开启串口DMA接收
    USART_Cmd(USART2, ENABLE);  //使能串口2
}

/*******************************************************
*Function Name	:USART2_IRQHandler
*Description  	:串口1中断函数
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
    uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0;    
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//    {
//        Res =USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
//    }

    //接收总线空闲中断，即可以
    if(USART_GetITStatus(USART2,USART_IT_IDLE) == SET)
    {
        Res = USART2->SR;
        Res =USART_ReceiveData(USART2) ; 							//读数据，同时清USART_IT_IDLE标志，USART2->DR
        DMA_Cmd(UART2_RX_DMA_Stream,DISABLE);    					//关闭DMA
		Res = USART2_REC_LEN -  DMA_GetCurrDataCounter(UART2_RX_DMA_Stream);  //得到真正接收数据个数        
		USART2_RX_BUF[USART2_REC_LEN-2]=Res>>8;							//在数组的最后2个字节保存该次串口接收数据长度；
		USART2_RX_BUF[USART2_REC_LEN-1]=Res&0xFF;						//在数组的最后2个字节保存该次串口接收数据长度；
		Result=xQueueSendFromISR(Uart2Rx_MessageData_Q,USART2_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART2_RX_BUF,0,USART2_REC_LEN);
        DMA_ClearFlag(UART2_RX_DMA_Stream,UART2_RX_DMA_FLAG_TCIF);		//清除接收完成标志,必须清除，否则下次DMA不会接收
		//DMA_SetCurrDataCounter(UART2_RX_DMA_Stream,USART_REC_LEN);  	//重新设置接收数据个数 :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART2_RX_DMA_Stream,ENABLE);  							//开启DMA
        // receive_flag = 1;           								//接收数据标志位置1
		if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}
/*******************************************************
*Function Name	:USART2_Sendbyte
*Description  	:串口1发送单个字节
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART2_Sendbyte(u8 snddata)
{
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
    USART_SendData(USART2, snddata);         //向串口2发送数据
}
/*******************************************************
*Function Name	:USART2_Sendbuf
*Description  	:串口1发送多个字节
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART2_Sendbuf(u8 *sndbuf,u8 buflen)
{
    while(buflen--)
    {
        USART2_Sendbyte(*sndbuf++);
    }
}



/*******************************************************
*Function Name	:uart2_init
*Description  	:初始化串口1
*Input		  	:bound:波特率
*Output		  	:
*Question	  	:1、
*******************************************************/
void uart4_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);		//使能UART4时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); 		//GPIOA0复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); 		//GPIOA1复用为USART1

    //UART4端口配置
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 					//GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//推挽复用输出,RX口应该设置为输入：GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//初始化PA0，PA1
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 					//GPIOA1与GPIOA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	

    //UART4 初始化设置
    USART_InitStructure.USART_BaudRate = bound;						//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART4, &USART_InitStructure); 						//初始化串口4


#if EN_USART4_RX_TX
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//串口4中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=12;	//抢占优先级7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 			//开启总线空闲中断
#endif

    USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);  		//开启串口DMA接收
    USART_Cmd(UART4, ENABLE);  //使能串口2
}

/*******************************************************
*Function Name	:USART4_IRQHandler
*Description  	:串口1中断函数
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void UART4_IRQHandler(void)                	//串口4中断服务程序
{
    uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0;    

    //接收总线空闲中断，即可以
    if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET)
    {
        Res = UART4->SR;
        Res =USART_ReceiveData(UART4) ; 							//读数据，同时清USART_IT_IDLE标志，UART4->DR
        DMA_Cmd(UART4_RX_DMA_Stream,DISABLE);    					//关闭DMA
		Res = USART4_REC_LEN -  DMA_GetCurrDataCounter(UART4_RX_DMA_Stream);  //得到真正接收数据个数        
		USART4_RX_BUF[USART4_REC_LEN-2]=Res>>8;						//在数组的最后2个字节保存该次串口接收数据长度；
		USART4_RX_BUF[USART4_REC_LEN-1]=Res;							//在数组的最后2个字节保存该次串口接收数据长度；		
		Result=xQueueSendFromISR(Uart4Rx_MessageData_Q,USART4_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART4_RX_BUF,0,USART4_REC_LEN);
        DMA_ClearFlag(UART4_RX_DMA_Stream,UART4_RX_DMA_FLAG_TCIF);		//清除接收完成标志,必须清除，否则下次DMA不会接收
		//DMA_SetCurrDataCounter(UART2_RX_DMA_Stream,USART_REC_LEN);  	//重新设置接收数据个数 :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART4_RX_DMA_Stream,ENABLE);  							//开启DMA
        // receive_flag = 1;           								//接收数据标志位置1
		if(Result!=pdFAIL)//置位成功 进行任务切换
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}


