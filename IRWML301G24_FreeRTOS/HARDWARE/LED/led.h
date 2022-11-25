#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include <stdbool.h>
//////////////////////////////////////////////////////////////////////////////////	
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//LED�˿ڶ���
#define LED0 PEout(0)	// DS0
#define LED1 PEout(1)	// DS1	
#define IO3_LED PCout(1)	// DS1	
void LED_Init(void);//��ʼ��	

//#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 	//PB9���ģʽ
//IO2(PB3)��������  
#define Synchro_IO2_IN()  {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=0<<2*2;}	//PC2����ģʽ
#define Synchro_IO2_OUT() {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=1<<2*2;} //PC2���ģʽ
#define	IO2_Pin			GPIO_Pin_2 
#define IO2_PORT     	GPIOC  
#define	IO3_Pin			GPIO_Pin_1 
#define IO3_PORT     	GPIOC  

extern void IO2_out(uint8_t OnOff );
extern bool IO2_Read(void);
extern void North_UL_lock(void);
extern void North_UL_unlock(void);
extern bool North_UL_islock(void);
extern bool North_UL_isunlock(void);
#endif
