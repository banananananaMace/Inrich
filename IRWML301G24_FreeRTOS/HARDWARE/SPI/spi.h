#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 
//#define SX1280_SPI_IN(txBuffer,size)  				SPI2_In(txBuffer,size)
//#define SX1280_SPI_INOut(txBuffer,rxBuffer,size)  	SPI2_InOut(txBuffer,rxBuffer,size)

extern void SPI1_Init(void);			 //��ʼ��SPI1��
extern void SPI2_Init(void);			 //��ʼ��SPI2��
extern void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
extern void SPI2_SetSpeed(u8 SpeedSet); //����SPI1�ٶ� 
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

extern void SPI1_InOut( uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size );
extern void SPI1_In( uint8_t *txBuffer, uint16_t size );

extern void SPI2_InOut( uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size );
extern void SPI2_In( uint8_t *txBuffer, uint16_t size );

#endif

