#include "sx1280_hw.h"

void EXTIX_Init(uint8_t GPortx, uint16_t GPIO_Pin, uint32_t prio);
//uint8_t PortGetBitPos( GPIO_TypeDef* GPIOx );
uint8_t GpioGetBitPos( uint16_t GPIO_Pin );
//static GpioIrqHandler *GpioIrq[16] = { NULL };

//����Busy,DIO1(�ⲿ�жϿ�)����λ�ڣ�SPI��Ƭѡ�ܽ�
void GpioInit( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* GPIO Ports Clock Enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOA,GPIOBʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOC,GPIODʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOC,GPIODʱ��
    /******��ʼ��SX1280_0*********************************/
    GPIO_InitStructure.GPIO_Pin = SX1280_0_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init( SX1280_0_NSS_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_0_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
    GPIO_Init( SX1280_0_BUSY_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0�ĸ�λ�ܽţ�/
    GPIO_InitStructure.GPIO_Pin  = SX1280_0_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_Init( SX1280_0_nRST_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��DIO1 :PC1�ⲿ�жϹܽ�
    GPIO_InitStructure.GPIO_Pin  = SX1280_0_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//����,//�����ش����ж�
    GPIO_Init( SX1280_0_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_0_EXTI_PORT,SX1280_0_DIO1_PIN,7);//��ʼ���ⲿ�жϣ�ͬʱ��Ҫ���Ķ�Ӧ���жϺ�����EXTI1_IRQHandler

    /******��ʼ��SX1280_1*********************************/
    //��ʼ��(��SX1280_1)Ƭѡ��SPI1_NSS__CTS1_Pin   PA0
    GPIO_InitStructure.GPIO_Pin = SX1280_1_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init( SX1280_1_NSS_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_1_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
    GPIO_Init( SX1280_1_BUSY_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0�ĸ�λ�ܽţ�/
    GPIO_InitStructure.GPIO_Pin  = SX1280_1_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_Init( SX1280_1_nRST_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��DIO1 :PC1�ⲿ�жϹܽ�
    GPIO_InitStructure.GPIO_Pin  = SX1280_1_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//����,//�����ش����ж�
    GPIO_Init( SX1280_1_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_1_EXTI_PORT,SX1280_1_DIO1_PIN,8);//��ʼ���ⲿ�жϣ�ͬʱ��Ҫ���Ķ�Ӧ���жϺ�����EXTI1_IRQHandler
    /******��ʼ��SX1280_2*********************************/
    //��ʼ��(��SX1280_2)Ƭѡ��SPI1_NSS__CTS1_Pin   PA0
    GPIO_InitStructure.GPIO_Pin = SX1280_2_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init( SX1280_2_NSS_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_2_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
    GPIO_Init( SX1280_2_BUSY_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0�ĸ�λ�ܽţ�/
    GPIO_InitStructure.GPIO_Pin  = SX1280_2_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_Init( SX1280_2_nRST_PORT, &GPIO_InitStructure );
    //��ʼ��SX1280_0��DIO1 :PC1�ⲿ�жϹܽ�
    GPIO_InitStructure.GPIO_Pin  = SX1280_2_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//����,//�����ش����ж�
    GPIO_Init( SX1280_2_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_2_EXTI_PORT,SX1280_2_DIO1_PIN,9);//��ʼ���ⲿ�жϣ�ͬʱ��Ҫ���Ķ�Ӧ���жϺ�����EXTI1_IRQHandler
}


//�ⲿ�жϳ�ʼ������
void EXTIX_Init(uint8_t GPortx, uint16_t GPIO_Pin, uint32_t prio)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

    IRQn_Type IRQnb;
    uint8_t BitPos = GpioGetBitPos( GPIO_Pin ) ;
    //uint8_t PortPos = PortGetBitPos( GPIOG ) ;

    //GpioIrq[BitPos] = irqHandler;  //
    IRQnb = MSP_GetIRQn( GPIO_Pin ); //���ݹܽźţ��ҵ���Ӧ��EXTI������_IRQn�ж���ں���������ӦΪPIN=1,������ֵIRQnb=EXTI1_IRQn

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 ���ӵ��ж���4
    SYSCFG_EXTILineConfig(GPortx, BitPos);//

    EXTI_InitStructure.EXTI_Line = GPIO_Pin;//EXTI_Line0 ��������Χ��1��2��4��8��16��32,�ʿ�����GPIO_Pin�����EXTI_Line
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
    EXTI_Init(&EXTI_InitStructure);//����

    NVIC_InitStructure.NVIC_IRQChannel =IRQnb; // EXTI0_IRQn;//����GPIO_Pinͨ��MSP_GetIRQn�������Եõ���Ӧ��EXTI��_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prio;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����
}


///*******************************************************
//*Function Name 	:GpioGetBitPos
//*Description  	: ����GPIO_Pin����ֵ��Χ��1��2��4��8��16�����ظ�PIN�Ǽ��Źܽţ�����ֵ��ΧΪ0~15��
//									��GPIO_Pin_0��ԭֵΪ0x0001����Ӧ����ֵΪ0��0�Źܽţ���GPIO_Pin_15��ԭֵΪ0x8000����Ӧ����ֵΪ15��15�Źܽţ�
//*Input					: GPIO_Pin����ֵ��Χ��1��2��4��8��16��������
//*Output					: GPIO_Pin�Ĺܽźţ�0~15����
//*******************************************************/
uint8_t GpioGetBitPos( uint16_t GPIO_Pin )
{
    uint8_t PinPos=0;
    if ( ( GPIO_Pin & 0xFF00 ) != 0 ) {
        PinPos |= 0x8;
    }
    if ( ( GPIO_Pin & 0xF0F0 ) != 0 ) {
        PinPos |= 0x4;
    }
    if ( ( GPIO_Pin & 0xCCCC ) != 0 ) {
        PinPos |= 0x2;
    }
    if ( ( GPIO_Pin & 0xAAAA ) != 0 ) {
        PinPos |= 0x1;
    }
    return PinPos;
}
//void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
//{
//  uint32_t prioritygroup = 0x00;

//  /* Check the parameters */
//  assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
//  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
//  prioritygroup = NVIC_GetPriorityGrouping();
//  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
//}
//void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
//{
//  /* Check the parameters */
//  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
//
//  /* Enable interrupt */
//  NVIC_EnableIRQ(IRQn);
//}
//void GpioSetIrq( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler )
//{
//    IRQn_Type IRQnb;
//    uint32_t BitPos = GpioGetBitPos( GPIO_Pin ) ;
//    if ( irqHandler != NULL )
//    {
//        // GpioIrq[BitPos]�������Ļص�����GpioLaunchIrqHandler���ã���irqHandler���ⲿ�жϺ��������壻
//        // GpioSetIrq��һ������SX1280HalIoIrqInit������һ����SX1280HalInit�����Ӧ������DioIrq��
//        //��DioIrq���������ⲿ�жϺ������壬��  SX1280OnDioIrq  ��
//        GpioIrq[BitPos] = irqHandler;
//        IRQnb = MSP_GetIRQn( GPIO_Pin ); //���ݹܽźţ��ҵ���Ӧ��EXTI������_IRQn�ж���ں���������ӦΪPIN=1,������ֵIRQnb=EXTI1_IRQn
//   //   HAL_NVIC_SetPriority( IRQnb, 1, prio );  //���൱�� HAL_NVIC_SetPriority(EXTI1_IRQn,0,0);
//			  HAL_NVIC_SetPriority( IRQnb, 5, prio );  //���൱�� HAL_NVIC_SetPriority(EXTI1_IRQn,0,0);
//        HAL_NVIC_EnableIRQ( IRQnb );  //ʹ���ⲿ�жϣ��������ⲿ�ж�1��
//    }
//}


//void GpioLaunchIrqHandler( uint16_t GPIO_Pin )
//{
//    uint32_t BitPos = GpioGetBitPos( GPIO_Pin );
//    if ( GpioIrq[BitPos]  != NULL )
//    {
//        GpioIrq[BitPos]( );
//    }
//}


