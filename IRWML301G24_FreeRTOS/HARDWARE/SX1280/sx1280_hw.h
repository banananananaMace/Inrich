#ifndef __HW_H__
#define __HW_H__

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "delay.h"
#include "spi.h"
#include "sx1280_hw-gpio.h"

#include "sx1280.h"
#include "sx1280_hal.h"

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void sx1280_HwInit( void );
void HwSetLowPower( void );
void HAL_Delay( uint32_t Delay );
void Error_Handler( void );

void SX1280_0_SET_RESET(uint8_t RstValue);
void SX1280_0_SET_NSS(uint8_t NSSValue);
uint32_t SX1280_0_ReadBusyPin( void);

void SX1280_1_SET_RESET(uint8_t RstValue);
void SX1280_1_SET_NSS(uint8_t NSSValue);
uint32_t SX1280_1_ReadBusyPin( void);

void SX1280_2_SET_RESET(uint8_t RstValue);
void SX1280_2_SET_NSS(uint8_t NSSValue);
uint32_t SX1280_2_ReadBusyPin( void);

//void SX1280_3_SET_RESET(uint8_t RstValue);
//void SX1280_3_SET_NSS(uint8_t NSSValue);
//uint32_t SX1280_3_ReadBusyPin( void);

//void SX1280_4_SET_RESET(uint8_t RstValue);
//void SX1280_4_SET_NSS(uint8_t NSSValue);
//uint32_t SX1280_4_ReadBusyPin( void);

//void SX1280_5_SET_RESET(uint8_t RstValue);
//void SX1280_5_SET_NSS(uint8_t NSSValue);
//uint32_t SX1280_5_ReadBusyPin( void);

//void SX1280_6SET_RESET(uint8_t RstValue);
//void SX1280_6_SET_NSS(uint8_t NSSValue);
//uint32_t SX1280_6_ReadBusyPin( void);

#endif // __HW_H__
