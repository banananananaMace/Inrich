#ifndef __HW_H__
#define __HW_H__

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "hw-spi.h"
#include "hw-uart.h"
#include "hw-gpio.h"
#include "i2c.h"

#include "sx1280.h"
#include "sx1280-hal.h"


#include "boards.h"

//#define USE_DMA

void HwInit( void );

void HwSetLowPower( void );


void SystemClock_Config( void );
void SystemClock_Config_24M(void);
void SystemClock_Config_1M(void);
void HAL_Delay( uint32_t Delay );



void Error_Handler( void );

#endif // __HW_H__


#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 


/******* PC12置1  *************************/
#define	IIC_SCL_SET	 GPIOB->BSRR=1<<6
/******* PC11置1  *************************/
#define	IIC_SDA_SET	 GPIOB->BSRR=1<<7
/******* PC12置0  *************************/
#define	IIC_SCL_CLR	 GPIOB->BRR=1<<6
/******* PC11置0  *************************/
#define	IIC_SDA_CLR	 GPIOB->BRR=1<<7
/******* SDA输入  *************************/
#define READ_SDA   PBin(7)
/******* SDA输出 *************************/
#define IIC_SDA    PBout(7)

///*******  SDA输入方向 *************************/
//#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
///*******  SDA输出方向 *************************/
//#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}



/******* IIC的IO口初始化 *************************/
void IIC_Init(void);
/******* 发送IIC开始信号  *************************/
void IIC_Start(void);
/******* 发送IIC停止信号  *************************/
void IIC_Stop(void);
/******* IIC发送一个字节  *************************/
void IIC_Send_Byte(uint8_t txd);
/******* IIC读取一个字节  *************************/
uint8_t IIC_Read_Byte(void);
/******* IIC等待ACK信号  *************************/
void IIC_Wait_Ack(void);
/******* IIC发送ACK信号  *************************/
void IIC_Ack(void);
/******* IIC不发送ACK信号  *************************/
void IIC_NAck(void);


void HwDeInit( void );//zcr

