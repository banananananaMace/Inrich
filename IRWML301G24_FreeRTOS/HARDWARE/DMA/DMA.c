#include "DMA.h" 
#include "usart.h"

/*******************************************************
*Function Name	:DMA1_init
*Description  	:初始化DMA
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void DMA1_init(void)
{
    DMA_InitTypeDef    DMA_Initstructure;
//   NVIC_InitTypeDef   NVIC_Initstructure;

    /*开启DMA时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 		//使能DMA1时钟
	
	//////////////////////UART2_DMA///////////////////////////////////
    DMA_DeInit(UART2_RX_DMA_Stream);  							// Uart2_RX属于DMA1的通道4，数据流5
    while(DMA_GetCmdStatus(UART2_RX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置
		
	DMA_DeInit(UART2_TX_DMA_Stream);  							// Uart2_TX属于DMA1的通道4，数据流6
    while(DMA_GetCmdStatus(UART2_TX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置	

//   /* Enable the DMA1 Interrupt,这里不需要用DMA的中断 */
//   NVIC_Initstructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;       //通道设置为串口1中断
//   NVIC_Initstructure.NVIC_IRQChannelSubPriority = 1;     //中断响应优先级0
//   NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=1;
//   NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;        //打开中断
//   NVIC_Init(&NVIC_Initstructure);

    //DMA配置：用于Uart2_RX,即串口接收用
	//（1）接收缓冲内存：USART2_RX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，而是由串口接收总线空闲中断
    DMA_Initstructure.DMA_Channel= UART2_RX_DMA_Channel;				//通道选,Uart2_RX属于DMA1的通道4，数据流5
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART2_RX_BUF;	//DMA存储器地址，即保存接收数据地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//数据方向：外设到存储器
    DMA_Initstructure.DMA_BufferSize = USART2_REC_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART2_RX_DMA_Stream,&DMA_Initstructure); 					// Uart2_RX属于DMA1的通道4，数据流5

	//DMA配置：用于Uart2_TX,即串口发送用
	//（1）发送缓冲内存：USART2_TX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，
    DMA_Initstructure.DMA_Channel= UART2_TX_DMA_Channel;			//通道选,Uart2_TX属于DMA1的通道4，数据流6
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART2_TX_BUF;	//DMA存储器地址，即发送数据的缓冲地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//数据方向：存储器到外设
    DMA_Initstructure.DMA_BufferSize = USART2_TX_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART2_TX_DMA_Stream,&DMA_Initstructure); 				// Uart2_TX属于DMA1的通道4，数据流6


	//启动DMA
    DMA_Cmd(UART2_RX_DMA_Stream,ENABLE);
    //开启DMA发送发成中断,这里不需要，而是等待串口接收总线空闲中断即可
    //DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
	
	
	//////////////////////UART4_DMA///////////////////////////////////
	DMA_DeInit(UART4_RX_DMA_Stream);  							// Uart2_RX属于DMA1的通道4，数据流5
    while(DMA_GetCmdStatus(UART4_RX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置
		
	DMA_DeInit(UART4_TX_DMA_Stream);  							// Uart2_TX属于DMA1的通道4，数据流6
    while(DMA_GetCmdStatus(UART4_TX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置	

    //DMA配置：用于Uart4_RX,即串口接收用
	//（1）接收缓冲内存：USART2_RX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，而是由串口接收总线空闲中断
    DMA_Initstructure.DMA_Channel= UART4_RX_DMA_Channel;			//通道选,Uart4_RX属于DMA1的通道
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&UART4->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART4_RX_BUF;	//DMA存储器地址，即保存接收数据地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//数据方向：外设到存储器
    DMA_Initstructure.DMA_BufferSize = USART4_REC_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART4_RX_DMA_Stream,&DMA_Initstructure); 					// Uart2_RX属于DMA1的通道4，数据流5

	//DMA配置：用于Uart4_TX,即串口发送用
	//（1）发送缓冲内存：USART4_TX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，
    DMA_Initstructure.DMA_Channel= UART4_TX_DMA_Channel;			//通道选,Uart2_TX属于DMA1的通道4，数据流6
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&UART4->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART4_TX_BUF;	//DMA存储器地址，即发送数据的缓冲地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//数据方向：存储器到外设
    DMA_Initstructure.DMA_BufferSize = USART4_TX_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART4_TX_DMA_Stream,&DMA_Initstructure); 				// Uart2_TX属于DMA1的通道4，数据流6


	//启动DMA
    DMA_Cmd(UART4_RX_DMA_Stream,ENABLE);
}


/*******************************************************
*Function Name	:DMA2_init
*Description  	:初始化DMA
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void DMA2_init(void)
{
    DMA_InitTypeDef    DMA_Initstructure;

    /*开启DMA时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 		//使能DMA2时钟
	
	//////////////////////UART1_DMA///////////////////////////////////
    DMA_DeInit(UART1_RX_DMA_Stream);  							// Uart1_RX属于DMA2的通道4，数据流5
    while(DMA_GetCmdStatus(UART1_RX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置
		
	DMA_DeInit(UART1_TX_DMA_Stream);  							// Uart1_TX属于DMA2的通道4，数据流7
    while(DMA_GetCmdStatus(UART1_TX_DMA_Stream)!=DISABLE) {}  	//等待DMA可配置	

//   /* Enable the DMA1 Interrupt,这里不需要用DMA的中断 */
//   NVIC_Initstructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;       //通道设置为串口1中断
//   NVIC_Initstructure.NVIC_IRQChannelSubPriority = 1;     //中断响应优先级0
//   NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=1;
//   NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;        //打开中断
//   NVIC_Init(&NVIC_Initstructure);

    //DMA配置：用于Uart1_RX,即串口接收用
	//（1）接收缓冲内存：USART1_RX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，而是由串口接收总线空闲中断
    DMA_Initstructure.DMA_Channel= UART1_RX_DMA_Channel;			//通道选,Uart1_RX属于DMA2的通道4，数据流5
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART1->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART1_RX_BUF;	//DMA存储器地址，即保存接收数据地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//数据方向：外设到存储器
    DMA_Initstructure.DMA_BufferSize = USART1_REC_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART1_RX_DMA_Stream,&DMA_Initstructure); 					// Uart1_RX属于DMA2的通道4，数据流5

	//DMA配置：用于Uart1_TX,即串口发送用
	//（1）发送缓冲内存：USART1_TX_BUF，长度USART_REC_LEN（200）
	//（2）这边DMA不用中断，
    DMA_Initstructure.DMA_Channel= UART1_TX_DMA_Channel;			//通道选,Uart1_TX属于DMA2的通道4，数据流7
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART1->DR);	//外设地址
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART1_TX_BUF;	//DMA存储器地址，即发送数据的缓冲地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//数据方向：存储器到外设
    DMA_Initstructure.DMA_BufferSize = USART1_TX_LEN;				//传输数据的字节数
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度：8位
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度：8位
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//使用普通模式
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//单次传输，即再次传输需重新启动
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//单次传输，即再次传输需重新启动
    DMA_Init(UART1_TX_DMA_Stream,&DMA_Initstructure); 				// Uart1_TX属于DMA2的通道4，数据流7

	//启动DMA
    DMA_Cmd(UART1_RX_DMA_Stream,ENABLE);
    //开启DMA发送发成中断,这里不需要，而是等待串口接收总线空闲中断即可
    //DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);	
}


