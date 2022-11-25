#include "sys.h"
#include "usart.h"
#include "string.h"
//#include "absacc.h"

//////////////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��
#include "queue.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
//��������:2019/7/26
//�汾��V1.0
//Copyright(C) Inrich
//����������
////////////////////////////////////////////////////////////////////////////////// 
extern QueueHandle_t Uart1Rx_MessageData_Q;   				//����1�������ݵ���Ϣ���о��
extern QueueHandle_t Uart2Rx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
extern QueueHandle_t Uart4Rx_MessageData_Q;   				//����4�������ݵ���Ϣ���о��

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((UART4->SR&0X40)==0);//ѭ������,ֱ���������
		UART4->DR = (u8) ch;

//  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//	USART_SendData(USART1, snddata);         //�򴮿�1��������
    return ch;
}
#endif


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART1_RX_BUF[USART1_REC_LEN]  ;     //���ջ���,���USART_REC_LEN���ֽ�.  __attribute__((at(0x20010000)))  volatile
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART4_RX_BUF[USART4_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

u8 USART1_TX_BUF[USART1_TX_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_TX_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART4_TX_BUF[USART4_TX_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

u16 USART1_TX_Lenth=0;       		//ÿ����Ҫ�������ݵĳ���
u16 USART2_TX_Lenth=0;       		//ÿ����Ҫ�������ݵĳ���
u16 USART4_TX_Lenth=0;       		//ÿ����Ҫ�������ݵĳ���

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART1_RX_STA=0;       //����״̬���
//u16 USART2_RX_STA=0;       //����״̬���



/*******************************************************
*Function Name	:uart1_init
*Description  	:��ʼ������1
*Input		  	:bound:������
*Output		  	:
*Question	  	:1��
*******************************************************/
void uart1_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��

    //����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; 					//GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//���츴�����,RX��Ӧ������Ϊ���룺GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//����
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//��ʼ��PA9��PA3
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; 				//GPIOA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1

#if EN_USART1_RX
	//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;//��ռ���ȼ�7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 			//�������߿����ж�
#endif
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  		//��������DMA����	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
}

/*******************************************************
*Function Name	:USART1_IRQHandler
*Description  	:����1�жϺ���
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0; 
	
    //�������߿����жϣ�������
    if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
    {
        Res = USART1->SR;
        Res =USART_ReceiveData(USART1) ; 							//�����ݣ�ͬʱ��USART_IT_IDLE��־��USART1->DR
        DMA_Cmd(UART1_RX_DMA_Stream,DISABLE);    					//�ر�DMA
		Res = USART1_REC_LEN -  DMA_GetCurrDataCounter(UART1_RX_DMA_Stream);  //�õ������������ݸ���        
		USART1_RX_BUF[USART1_REC_LEN-2]=Res>>8;						//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�
		USART1_RX_BUF[USART1_REC_LEN-1]=Res&0xFF;						//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�
		Result=xQueueSendFromISR(Uart1Rx_MessageData_Q,USART1_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART1_RX_BUF,0,USART1_REC_LEN);
        DMA_ClearFlag(UART1_RX_DMA_Stream,UART1_RX_DMA_FLAG_TCIF);		//���������ɱ�־,��������������´�DMA�������
		//DMA_SetCurrDataCounter(UART1_RX_DMA_Stream,USART_REC_LEN);  	//�������ý������ݸ��� :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART1_RX_DMA_Stream,ENABLE);  							//����DMA
        // receive_flag = 1;           								//�������ݱ�־λ��1
		if(Result!=pdFAIL)//��λ�ɹ� ���������л�
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}
/*******************************************************
*Function Name	:USART1_Sendbyte
*Description  	:����1���͵����ֽ�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART1_Sendbyte(u8 snddata)
{
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
    USART_SendData(USART1, snddata);         //�򴮿�1��������
}
/*******************************************************
*Function Name	:USART1_Sendbuf
*Description  	:����1���Ͷ���ֽ�
*Input		  	:
*Output		  	:
*Question	  	:1��
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
*Description  	:��ʼ������1
*Input		  	:bound:������
*Output		  	:
*Question	  	:1��
*******************************************************/
void uart2_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//ʹ��USART2ʱ��

    //����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 	//GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 	//GPIOA10����ΪUSART1

    //USART2�˿�����
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; 					//GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//���츴�����,RX��Ӧ������Ϊ���룺GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//����
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//��ʼ��PA2��PA3
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ; 					//GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	

    //USART2 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;						//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;				//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); 						//��ʼ������2


#if EN_USART2_RX_TX
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=11;	//��ռ���ȼ�7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

    //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//���������ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 			//�������߿����ж�
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);			//���������жϣ����ͼĴ����գ�,����ʱ�ٿ���
#endif

    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  		//��������DMA����
    USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
}

/*******************************************************
*Function Name	:USART2_IRQHandler
*Description  	:����1�жϺ���
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART2_IRQHandler(void)                	//����1�жϷ������
{
    uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0;    
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//    {
//        Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
//    }

    //�������߿����жϣ�������
    if(USART_GetITStatus(USART2,USART_IT_IDLE) == SET)
    {
        Res = USART2->SR;
        Res =USART_ReceiveData(USART2) ; 							//�����ݣ�ͬʱ��USART_IT_IDLE��־��USART2->DR
        DMA_Cmd(UART2_RX_DMA_Stream,DISABLE);    					//�ر�DMA
		Res = USART2_REC_LEN -  DMA_GetCurrDataCounter(UART2_RX_DMA_Stream);  //�õ������������ݸ���        
		USART2_RX_BUF[USART2_REC_LEN-2]=Res>>8;							//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�
		USART2_RX_BUF[USART2_REC_LEN-1]=Res&0xFF;						//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�
		Result=xQueueSendFromISR(Uart2Rx_MessageData_Q,USART2_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART2_RX_BUF,0,USART2_REC_LEN);
        DMA_ClearFlag(UART2_RX_DMA_Stream,UART2_RX_DMA_FLAG_TCIF);		//���������ɱ�־,��������������´�DMA�������
		//DMA_SetCurrDataCounter(UART2_RX_DMA_Stream,USART_REC_LEN);  	//�������ý������ݸ��� :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART2_RX_DMA_Stream,ENABLE);  							//����DMA
        // receive_flag = 1;           								//�������ݱ�־λ��1
		if(Result!=pdFAIL)//��λ�ɹ� ���������л�
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}
/*******************************************************
*Function Name	:USART2_Sendbyte
*Description  	:����1���͵����ֽ�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART2_Sendbyte(u8 snddata)
{
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
    USART_SendData(USART2, snddata);         //�򴮿�2��������
}
/*******************************************************
*Function Name	:USART2_Sendbuf
*Description  	:����1���Ͷ���ֽ�
*Input		  	:
*Output		  	:
*Question	  	:1��
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
*Description  	:��ʼ������1
*Input		  	:bound:������
*Output		  	:
*Question	  	:1��
*******************************************************/
void uart4_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);		//ʹ��UART4ʱ��

    //����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); 		//GPIOA0����ΪUSART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); 		//GPIOA1����ΪUSART1

    //UART4�˿�����
	//TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; 					//GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//���츴�����,RX��Ӧ������Ϊ���룺GPIO_Mode_IN_FLOATING
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//����
    GPIO_Init(GPIOA,&GPIO_InitStructure); 						//��ʼ��PA0��PA1
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 					//GPIOA1��GPIOA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���    ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	

    //UART4 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;						//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;				//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(UART4, &USART_InitStructure); 						//��ʼ������4


#if EN_USART4_RX_TX
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		//����4�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=12;	//��ռ���ȼ�7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 			//�������߿����ж�
#endif

    USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);  		//��������DMA����
    USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���2
}

/*******************************************************
*Function Name	:USART4_IRQHandler
*Description  	:����1�жϺ���
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void UART4_IRQHandler(void)                	//����4�жϷ������
{
    uint16_t Res;
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t Result=0;    

    //�������߿����жϣ�������
    if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET)
    {
        Res = UART4->SR;
        Res =USART_ReceiveData(UART4) ; 							//�����ݣ�ͬʱ��USART_IT_IDLE��־��UART4->DR
        DMA_Cmd(UART4_RX_DMA_Stream,DISABLE);    					//�ر�DMA
		Res = USART4_REC_LEN -  DMA_GetCurrDataCounter(UART4_RX_DMA_Stream);  //�õ������������ݸ���        
		USART4_RX_BUF[USART4_REC_LEN-2]=Res>>8;						//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�
		USART4_RX_BUF[USART4_REC_LEN-1]=Res;							//����������2���ֽڱ���ôδ��ڽ������ݳ��ȣ�		
		Result=xQueueSendFromISR(Uart4Rx_MessageData_Q,USART4_RX_BUF,&xHigherPriorityTaskWoken);			//
		memset(USART4_RX_BUF,0,USART4_REC_LEN);
        DMA_ClearFlag(UART4_RX_DMA_Stream,UART4_RX_DMA_FLAG_TCIF);		//���������ɱ�־,��������������´�DMA�������
		//DMA_SetCurrDataCounter(UART2_RX_DMA_Stream,USART_REC_LEN);  	//�������ý������ݸ��� :DMA1_Stream5->CNDTR=128
        DMA_Cmd(UART4_RX_DMA_Stream,ENABLE);  							//����DMA
        // receive_flag = 1;           								//�������ݱ�־λ��1
		if(Result!=pdFAIL)//��λ�ɹ� ���������л�
        {
            //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken );
        }
    }
}


