#ifndef __BOARDS_H__
#define __BOARDS_H__
  
//1280 SPI 
#define RADIO_NSS_PIN       GPIO_PIN_4
#define RADIO_NSS_PORT      GPIOA

#define RADIO_MOSI_PIN      GPIO_PIN_7
#define RADIO_MOSI_PORT     GPIOA

#define RADIO_MISO_PIN      GPIO_PIN_6
#define RADIO_MISO_PORT     GPIOA

#define RADIO_SCK_PIN       GPIO_PIN_5
#define RADIO_SCK_PORT      GPIOA

#define RADIO_nRESET_PIN    GPIO_PIN_12
#define RADIO_nRESET_PORT   GPIOA

#define RADIO_BUSY_PIN      GPIO_PIN_11
#define RADIO_BUSY_PORT     GPIOA

#define RADIO_DIO1_PIN      GPIO_PIN_10
#define RADIO_DIO_PORT     GPIOA

#define RADIO_DIO2_PIN      GPIO_PIN_9
#define RADIO_DIO2_PORT     GPIOA

#define RADIO_DIO3_PIN      GPIO_PIN_8
#define RADIO_DIO3_PORT     GPIOA



#define IO_VBAT_PinA1        GPIO_PIN_1
#define IO_VBAT_PortA       GPIOA


#define LED_PinB3          GPIO_PIN_3
#define LED_PortB         GPIOB

#define IO_VCC1_PinB0          GPIO_PIN_0
#define IO_VCC1_PortB         GPIOB

#define IO_VCC2_PinB1          GPIO_PIN_1
#define IO_VCC2_PortB         GPIOB

#define IO_VCC3_PinB5          GPIO_PIN_5
#define IO_VCC3_PortB         GPIOB

//#define POW_OE_PIN          GPIO_PIN_0
//#define POW_OE_PORT         GPIOA


//ÎÂ¶È´«¸ÐÆ÷
//#define ALERT_PIN          GPIO_PIN_4
//#define ALERT_PORT         GPIOB

//#define V_I2C_PIN        GPIO_PIN_8
//#define V_I2C_PORT       GPIOA

//#define V_SHT_PIN        GPIO_PIN_1
//#define V_SHT_PORT       GPIOB




#endif // __BOARDS_H__
