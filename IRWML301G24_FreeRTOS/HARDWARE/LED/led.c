#include "led.h" 
#include "delay.h"


/*******************************************************
*Function Name:LED_Init
*Description  :LED�Ƴ�ʼ
*Input		  :
*Output		  :
*******************************************************/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��GPIOCʱ��

	//GPIOE0,E1��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//LED0��LED1��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);					//��ʼ��GPIO
//	GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);			//GPIOE0,E1���øߣ�����
	GPIO_ResetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);			//GPIOE0,E1���õͣ�����
	
	//GPIOC1  IO3��ʼ������
	GPIO_InitStructure.GPIO_Pin = IO3_Pin;					//IO3��Ӧ��IO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//����
	GPIO_Init(IO3_PORT, &GPIO_InitStructure);				//��ʼ��GPIO
	GPIO_ResetBits(IO3_PORT,IO3_Pin);						//GPIOE0,E1���õͣ�����
			
	GPIO_InitStructure.GPIO_Pin = IO2_Pin ;					//IO2_Pin��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType =GPIO_OType_OD ;			//��©���  GPIO_OType_OD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//����   GPIO_PuPd_NOPULL
	GPIO_Init(IO2_PORT, &GPIO_InitStructure);				//��ʼ��GPIO
	//Synchro_IO2_OUT();
	IO2_out(1);							//�ø�
//	delay_xms(10);
//	
//	Synchro_IO2_OUT();
//	IO2_out(1);
//	Synchro_IO2_IN();
//	delay_xms(10);
//	Synchro_IO2_OUT();
//	IO2_out(0);
//	Synchro_IO2_IN();	
}

/*******************************************************
*Function Name	:IO2_out
*Description  	:
*Input		  	:
*Output		  	:
*Q&A	  		:1��
*******************************************************/
void IO2_out(uint8_t OnOff )
{
    switch(OnOff)
    {
    case 0:
        GPIO_WriteBit(IO2_PORT,IO2_Pin,Bit_RESET);
        break;
    case 1:
        GPIO_WriteBit(IO2_PORT,IO2_Pin,Bit_SET);
        break;
    default:
        GPIO_ToggleBits(IO2_PORT,IO2_Pin);
        break;
    }
}

/*******************************************************
*Function Name	:IO2_Read
*Description  	:
*Input		  	:
*Output		  	:
*Q&A	  		:1��
*******************************************************/
bool IO2_Read(void)
{
  return (GPIO_ReadInputDataBit(IO2_PORT,IO2_Pin));
}

/*******************************************************
*Function Name	:North_UL_lock
*Description  	:���򴮿ڷ�������������ʾ���ڷ���
*Input		  	:
*Output		  	:
*Q&A	  		:1��
*******************************************************/
void North_UL_lock(void)
{
//    uart_flag_send = 0;
    Synchro_IO2_OUT();
    IO2_out(0);
}
/*******************************************************
*Function Name	:North_UL_unlock
*Description  	:���򴮿ڷ��ͽ���������ʾ�������
*Input		  	:
*Output		  	:
*Q&A	  		:1��
*******************************************************/
void North_UL_unlock(void)
{
//    uart_flag_send = 1;
//	Synchro_IO2_OUT();
    IO2_out(1);
    Synchro_IO2_IN();
}
/*******************************************************
*Function Name	:North_UL_islock
*Description  	:��ѯ���򴮿ڷ��ͽ��Ƿ�������״̬�� true-��ʾ����״̬��flase-��ʾ����״̬
*Input		  	:
*Output		  	:true-��ʾ����״̬��flase-��ʾ����״̬
*Q&A	  		:1��
*******************************************************/
bool North_UL_islock(void)
{
    Synchro_IO2_IN();
//    if ((uart_flag_send == 0) || ( IO2_Read() == 0 )) return true;
	if ( IO2_Read() == 0 ) return true;
    else return false;
}

/*******************************************************
*Function Name	:North_UL_islock
*Description  	:��ѯ���򴮿ڷ��ͽ��Ƿ��ڽ���״̬�� true-��ʾ����״̬��flase-��ʾ����״̬
*Input		  	:
*Output		  	:true-��ʾ����״̬��flase-��ʾ����״̬
*Q&A	  		:1��
*******************************************************/
bool North_UL_isunlock(void)
{
    Synchro_IO2_IN();
//    if ((uart_flag_send == 1) && ( IO2_Read() == 1 )) return true;
	if ( IO2_Read() == 1 ) return true;
    else return false;
}



