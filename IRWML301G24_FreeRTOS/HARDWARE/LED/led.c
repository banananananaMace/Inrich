#include "led.h" 
#include "delay.h"


/*******************************************************
*Function Name:LED_Init
*Description  :LED灯初始
*Input		  :
*Output		  :
*******************************************************/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能GPIOC时钟

	//GPIOE0,E1初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);					//初始化GPIO
//	GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);			//GPIOE0,E1设置高，灯灭
	GPIO_ResetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);			//GPIOE0,E1设置低，灯亮
	
	//GPIOC1  IO3初始化设置
	GPIO_InitStructure.GPIO_Pin = IO3_Pin;					//IO3对应的IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_Init(IO3_PORT, &GPIO_InitStructure);				//初始化GPIO
	GPIO_ResetBits(IO3_PORT,IO3_Pin);						//GPIOE0,E1设置低，灯亮
			
	GPIO_InitStructure.GPIO_Pin = IO2_Pin ;					//IO2_Pin对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType =GPIO_OType_OD ;			//开漏输出  GPIO_OType_OD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//无拉   GPIO_PuPd_NOPULL
	GPIO_Init(IO2_PORT, &GPIO_InitStructure);				//初始化GPIO
	//Synchro_IO2_OUT();
	IO2_out(1);							//置高
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
*Q&A	  		:1、
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
*Q&A	  		:1、
*******************************************************/
bool IO2_Read(void)
{
  return (GPIO_ReadInputDataBit(IO2_PORT,IO2_Pin));
}

/*******************************************************
*Function Name	:North_UL_lock
*Description  	:北向串口发送上锁，即表示正在发送
*Input		  	:
*Output		  	:
*Q&A	  		:1、
*******************************************************/
void North_UL_lock(void)
{
//    uart_flag_send = 0;
    Synchro_IO2_OUT();
    IO2_out(0);
}
/*******************************************************
*Function Name	:North_UL_unlock
*Description  	:北向串口发送解锁，即表示发送完毕
*Input		  	:
*Output		  	:
*Q&A	  		:1、
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
*Description  	:查询北向串口发送解是否处于锁定状态？ true-表示锁定状态，flase-表示解锁状态
*Input		  	:
*Output		  	:true-表示锁定状态，flase-表示解锁状态
*Q&A	  		:1、
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
*Description  	:查询北向串口发送解是否处于解锁状态？ true-表示解锁状态，flase-表示锁定状态
*Input		  	:
*Output		  	:true-表示锁定状态，flase-表示解锁状态
*Q&A	  		:1、
*******************************************************/
bool North_UL_isunlock(void)
{
    Synchro_IO2_IN();
//    if ((uart_flag_send == 1) && ( IO2_Read() == 1 )) return true;
	if ( IO2_Read() == 1 ) return true;
    else return false;
}



