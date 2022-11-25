#include "spi.h"


//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��

    //GPIOFB3,4,5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//PB3~5���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PB3����Ϊ SPI1 SCK
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //PB4����Ϊ SPI1 MISO
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PB5����Ϊ SPI1  MOSI

    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
    SPI1_ReadWriteByte(0xff);//��������
}
//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
//fAPB2ʱ��һ��Ϊ84Mhz��
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
    SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
}

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    SPI2->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
    SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(SPI2,ENABLE); //ʹ��SPI1
}
//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {} //�ȴ���������
    SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {} //�ȴ�������һ��byte
    return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����
}

void SPI1_InOut( uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size )
{
    //HAL_SPIEx_FlushRxFifo( &SPI1_Handler );
    //HAL_SPI_TransmitReceive( &SPI1_Handler, txBuffer, rxBuffer, size, HAL_MAX_DELAY );
    uint16_t spi1szie;
    for(spi1szie=0; spi1szie<size; spi1szie++)
    {
        *rxBuffer++=SPI1_ReadWriteByte(*txBuffer++);
    }
}



void SPI1_In( uint8_t *txBuffer, uint16_t size )
{
    //HAL_SPI_Transmit( &SPI1_Handler, txBuffer, size, HAL_MAX_DELAY );
    uint16_t spi1szie;
    for(spi1szie=0; spi1szie<size; spi1szie++)
    {
        SPI1_ReadWriteByte(*txBuffer++);
    }
}


void SPI2_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //����ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //���ų�ʼ��
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);  //�����ŵĸ��ù���
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
}
//SPI2���ܳ�ʼ����
void SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    SPI2_GPIOConfig();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);  //ʱ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��ģʽ
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   //��Ϊ����ʹ��
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //���ݳ���8
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //�������NSS����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStructure);
    SPI_Cmd(SPI2,ENABLE);
}
void MySPI_SendData(char da)
{
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
    SPI_SendData(SPI2,da);
}

uint8_t MySPI_ReceiveData(void)
{
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);
    return SPI_ReceiveData(SPI2);
}


u8 SPI2_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {} //�ȴ���������
    SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {} //�ȴ�������һ��byte
    return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����
}


void SPI2_InOut( uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size )
{
    uint16_t spi1szie;
    for(spi1szie=0; spi1szie<size; spi1szie++)
    {
        *rxBuffer++=SPI2_ReadWriteByte(*txBuffer++);
    }
}

void SPI2_In( uint8_t *txBuffer, uint16_t size )
{
    //HAL_SPI_Transmit( &SPI1_Handler, txBuffer, size, HAL_MAX_DELAY );
    uint16_t spi1szie;
    for(spi1szie=0; spi1szie<size; spi1szie++)
    {
        SPI2_ReadWriteByte(*txBuffer++);
    }
}




