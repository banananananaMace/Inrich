#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__
#include "stm32f4xx.h"
/***************SX1280_0****************/
//对应原理图中的NSS4标号
#define SX1280_0_NSS_PIN    		GPIO_Pin_11
#define SX1280_0_NSS_PORT   		GPIOE

#define SX1280_0_nRST_PIN    		GPIO_Pin_8
#define SX1280_0_nRST_PORT   		GPIOD

#define SX1280_0_BUSY_PIN     		GPIO_Pin_15
#define SX1280_0_BUSY_PORT     		GPIOE

#define SX1280_0_DIO1_PIN      		GPIO_Pin_14
#define SX1280_0_DIO1_PORT      	GPIOE
#define SX1280_0_EXTI_PORT      	EXTI_PortSourceGPIOE

/***************SX1280_1****************/
//对应原理图中的NSS5
#define SX1280_1_NSS_PIN    		GPIO_Pin_9
#define SX1280_1_NSS_PORT   		GPIOD

#define SX1280_1_nRST_PIN    		GPIO_Pin_14
#define SX1280_1_nRST_PORT   		GPIOD

#define SX1280_1_BUSY_PIN     		GPIO_Pin_13
#define SX1280_1_BUSY_PORT     		GPIOD

#define SX1280_1_DIO1_PIN      		GPIO_Pin_12
#define SX1280_1_DIO1_PORT      	GPIOD
#define SX1280_1_EXTI_PORT     		EXTI_PortSourceGPIOD

/***************SX1280_2****************/
//对应原理图中的NSS6
#define SX1280_2_NSS_PIN    		GPIO_Pin_15
#define SX1280_2_NSS_PORT   		GPIOD

#define SX1280_2_nRST_PIN    		GPIO_Pin_8
#define SX1280_2_nRST_PORT   		GPIOA

#define SX1280_2_BUSY_PIN     		GPIO_Pin_9
#define SX1280_2_BUSY_PORT     		GPIOC

#define SX1280_2_DIO1_PIN      		GPIO_Pin_8
#define SX1280_2_DIO1_PORT      	GPIOC
#define SX1280_2_EXTI_PORT      	EXTI_PortSourceGPIOC






typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;


typedef void( GpioIrqHandler )( void );
void GpioLaunchIrqHandler( uint16_t GPIO_Pin );

/*!
 * GPIO IRQ handler function prototype
 */
IRQn_Type MSP_GetIRQn( uint16_t gpioPin );

void GpioInit( void );

uint8_t GpioGetBitPos( uint16_t GPIO_Pin );
void GpioSetIrq( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler );
void GpioLaunchIrqHandler( uint16_t GPIO_Pin );

#endif // __HW_GPIO_H__
