#include "DMA.h" 
#include "usart.h"

/*******************************************************
*Function Name	:DMA1_init
*Description  	:��ʼ��DMA
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void DMA1_init(void)
{
    DMA_InitTypeDef    DMA_Initstructure;
//   NVIC_InitTypeDef   NVIC_Initstructure;

    /*����DMAʱ��*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE); 		//ʹ��DMA1ʱ��
	
	//////////////////////UART2_DMA///////////////////////////////////
    DMA_DeInit(UART2_RX_DMA_Stream);  							// Uart2_RX����DMA1��ͨ��4��������5
    while(DMA_GetCmdStatus(UART2_RX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������
		
	DMA_DeInit(UART2_TX_DMA_Stream);  							// Uart2_TX����DMA1��ͨ��4��������6
    while(DMA_GetCmdStatus(UART2_TX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������	

//   /* Enable the DMA1 Interrupt,���ﲻ��Ҫ��DMA���ж� */
//   NVIC_Initstructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;       //ͨ������Ϊ����1�ж�
//   NVIC_Initstructure.NVIC_IRQChannelSubPriority = 1;     //�ж���Ӧ���ȼ�0
//   NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=1;
//   NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;        //���ж�
//   NVIC_Init(&NVIC_Initstructure);

    //DMA���ã�����Uart2_RX,�����ڽ�����
	//��1�����ջ����ڴ棺USART2_RX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ������ɴ��ڽ������߿����ж�
    DMA_Initstructure.DMA_Channel= UART2_RX_DMA_Channel;				//ͨ��ѡ,Uart2_RX����DMA1��ͨ��4��������5
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART2_RX_BUF;	//DMA�洢����ַ��������������ݵ�ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//���ݷ������赽�洢��
    DMA_Initstructure.DMA_BufferSize = USART2_REC_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART2_RX_DMA_Stream,&DMA_Initstructure); 					// Uart2_RX����DMA1��ͨ��4��������5

	//DMA���ã�����Uart2_TX,�����ڷ�����
	//��1�����ͻ����ڴ棺USART2_TX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ�
    DMA_Initstructure.DMA_Channel= UART2_TX_DMA_Channel;			//ͨ��ѡ,Uart2_TX����DMA1��ͨ��4��������6
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART2_TX_BUF;	//DMA�洢����ַ�����������ݵĻ����ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//���ݷ��򣺴洢��������
    DMA_Initstructure.DMA_BufferSize = USART2_TX_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART2_TX_DMA_Stream,&DMA_Initstructure); 				// Uart2_TX����DMA1��ͨ��4��������6


	//����DMA
    DMA_Cmd(UART2_RX_DMA_Stream,ENABLE);
    //����DMA���ͷ����ж�,���ﲻ��Ҫ�����ǵȴ����ڽ������߿����жϼ���
    //DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
	
	
	//////////////////////UART4_DMA///////////////////////////////////
	DMA_DeInit(UART4_RX_DMA_Stream);  							// Uart2_RX����DMA1��ͨ��4��������5
    while(DMA_GetCmdStatus(UART4_RX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������
		
	DMA_DeInit(UART4_TX_DMA_Stream);  							// Uart2_TX����DMA1��ͨ��4��������6
    while(DMA_GetCmdStatus(UART4_TX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������	

    //DMA���ã�����Uart4_RX,�����ڽ�����
	//��1�����ջ����ڴ棺USART2_RX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ������ɴ��ڽ������߿����ж�
    DMA_Initstructure.DMA_Channel= UART4_RX_DMA_Channel;			//ͨ��ѡ,Uart4_RX����DMA1��ͨ��
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&UART4->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART4_RX_BUF;	//DMA�洢����ַ��������������ݵ�ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//���ݷ������赽�洢��
    DMA_Initstructure.DMA_BufferSize = USART4_REC_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART4_RX_DMA_Stream,&DMA_Initstructure); 					// Uart2_RX����DMA1��ͨ��4��������5

	//DMA���ã�����Uart4_TX,�����ڷ�����
	//��1�����ͻ����ڴ棺USART4_TX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ�
    DMA_Initstructure.DMA_Channel= UART4_TX_DMA_Channel;			//ͨ��ѡ,Uart2_TX����DMA1��ͨ��4��������6
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&UART4->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART4_TX_BUF;	//DMA�洢����ַ�����������ݵĻ����ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//���ݷ��򣺴洢��������
    DMA_Initstructure.DMA_BufferSize = USART4_TX_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART4_TX_DMA_Stream,&DMA_Initstructure); 				// Uart2_TX����DMA1��ͨ��4��������6


	//����DMA
    DMA_Cmd(UART4_RX_DMA_Stream,ENABLE);
}


/*******************************************************
*Function Name	:DMA2_init
*Description  	:��ʼ��DMA
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void DMA2_init(void)
{
    DMA_InitTypeDef    DMA_Initstructure;

    /*����DMAʱ��*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 		//ʹ��DMA2ʱ��
	
	//////////////////////UART1_DMA///////////////////////////////////
    DMA_DeInit(UART1_RX_DMA_Stream);  							// Uart1_RX����DMA2��ͨ��4��������5
    while(DMA_GetCmdStatus(UART1_RX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������
		
	DMA_DeInit(UART1_TX_DMA_Stream);  							// Uart1_TX����DMA2��ͨ��4��������7
    while(DMA_GetCmdStatus(UART1_TX_DMA_Stream)!=DISABLE) {}  	//�ȴ�DMA������	

//   /* Enable the DMA1 Interrupt,���ﲻ��Ҫ��DMA���ж� */
//   NVIC_Initstructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;       //ͨ������Ϊ����1�ж�
//   NVIC_Initstructure.NVIC_IRQChannelSubPriority = 1;     //�ж���Ӧ���ȼ�0
//   NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=1;
//   NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;        //���ж�
//   NVIC_Init(&NVIC_Initstructure);

    //DMA���ã�����Uart1_RX,�����ڽ�����
	//��1�����ջ����ڴ棺USART1_RX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ������ɴ��ڽ������߿����ж�
    DMA_Initstructure.DMA_Channel= UART1_RX_DMA_Channel;			//ͨ��ѡ,Uart1_RX����DMA2��ͨ��4��������5
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART1->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART1_RX_BUF;	//DMA�洢����ַ��������������ݵ�ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	 		//���ݷ������赽�洢��
    DMA_Initstructure.DMA_BufferSize = USART1_REC_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART1_RX_DMA_Stream,&DMA_Initstructure); 					// Uart1_RX����DMA2��ͨ��4��������5

	//DMA���ã�����Uart1_TX,�����ڷ�����
	//��1�����ͻ����ڴ棺USART1_TX_BUF������USART_REC_LEN��200��
	//��2�����DMA�����жϣ�
    DMA_Initstructure.DMA_Channel= UART1_TX_DMA_Channel;			//ͨ��ѡ,Uart1_TX����DMA2��ͨ��4��������7
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART1->DR);	//�����ַ
    DMA_Initstructure.DMA_Memory0BaseAddr     = (u32)USART1_TX_BUF;	//DMA�洢����ַ�����������ݵĻ����ַ
    DMA_Initstructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	 		//���ݷ��򣺴洢��������
    DMA_Initstructure.DMA_BufferSize = USART1_TX_LEN;				//�������ݵ��ֽ���
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݳ��ȣ�8λ
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ��ȣ�8λ
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;					//ʹ����ͨģʽ
    DMA_Initstructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
    DMA_Initstructure.DMA_FIFOMode = DMA_FIFOMode_Disable;			//
    DMA_Initstructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	//
    DMA_Initstructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//���δ��䣬���ٴδ�������������
    DMA_Initstructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//���δ��䣬���ٴδ�������������
    DMA_Init(UART1_TX_DMA_Stream,&DMA_Initstructure); 				// Uart1_TX����DMA2��ͨ��4��������7

	//����DMA
    DMA_Cmd(UART1_RX_DMA_Stream,ENABLE);
    //����DMA���ͷ����ж�,���ﲻ��Ҫ�����ǵȴ����ڽ������߿����жϼ���
    //DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);	
}


