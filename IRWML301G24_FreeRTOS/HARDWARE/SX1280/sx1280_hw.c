#include "sx1280_hw.h"


extern void Error_Handler( void );

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if(PinState == GPIO_PIN_RESET)
    {
        //GPIOx->BSRR = (uint32_t)GPIO_Pin;
        GPIO_ResetBits(GPIOx,GPIO_Pin);
    }
    else
    {
        //GPIOx->BRR = (uint32_t)GPIO_Pin;
        GPIO_SetBits(GPIOx,GPIO_Pin);
    }
}

//GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//  GPIO_PinState bitstatus;
//	bitstatus=GPIO_ReadInputDataBit(GPIOx,GPIO_Pin);
//	return(bitstatus);
//}

void sx1280_HwInit( void )
{
    GpioInit(); //包括Busy,DIO1(外部中断口)，复位口，SPI的片选管脚
//    SPI1_Init();		   			//初始化SPI
//    SPI1_SetSpeed(SPI_BaudRatePrescaler_32);		//设置为21M时钟,高速模式
    SPI2_Init();//SPI
    SPI2_SetSpeed(SPI_BaudRatePrescaler_32);		//设置为21M时钟,高速模式
}

/******SX1280_0的复位，片选，忙信号的管脚操作*********************************/
void SX1280_0_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
{
    HAL_GPIO_WritePin( SX1280_0_nRST_PORT, SX1280_0_nRST_PIN,( GPIO_PinState ) RstValue );

}
void SX1280_0_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
{
    HAL_GPIO_WritePin( SX1280_0_NSS_PORT, SX1280_0_NSS_PIN,( GPIO_PinState ) NSSValue );
}
uint32_t SX1280_0_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
{
    //return HAL_GPIO_ReadPin( SX1280_0_BUSY_PORT, SX1280_0_BUSY_PIN );
    return GPIO_ReadInputDataBit( SX1280_0_BUSY_PORT, SX1280_0_BUSY_PIN );
}

/******SX1280_1的复位，片选，忙信号的管脚操作*********************************/
void SX1280_1_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
{
    HAL_GPIO_WritePin( SX1280_1_nRST_PORT, SX1280_1_nRST_PIN,( GPIO_PinState ) RstValue );
}
void SX1280_1_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
{
    HAL_GPIO_WritePin( SX1280_1_NSS_PORT, SX1280_1_NSS_PIN,( GPIO_PinState ) NSSValue );
}
uint32_t SX1280_1_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
{
    //return HAL_GPIO_ReadPin( SX1280_1_BUSY_PORT, SX1280_1_BUSY_PIN );
    return GPIO_ReadInputDataBit( SX1280_1_BUSY_PORT, SX1280_1_BUSY_PIN );
}

/******SX1280_2的复位，片选，忙信号的管脚操作*********************************/
void SX1280_2_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
{
    HAL_GPIO_WritePin( SX1280_2_nRST_PORT, SX1280_2_nRST_PIN,( GPIO_PinState ) RstValue );
}
void SX1280_2_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
{
    HAL_GPIO_WritePin( SX1280_2_NSS_PORT, SX1280_2_NSS_PIN,( GPIO_PinState ) NSSValue );
}
uint32_t SX1280_2_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
{
    //return HAL_GPIO_ReadPin( SX1280_2_BUSY_PORT, SX1280_2_BUSY_PIN );
    return GPIO_ReadInputDataBit( SX1280_2_BUSY_PORT, SX1280_2_BUSY_PIN );
}

///******SX1280_3的复位，片选，忙信号的管脚操作*********************************/
//void SX1280_3_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
//{
//    HAL_GPIO_WritePin( SX1280_3_nRST_PORT, SX1280_3_nRST_PIN,( GPIO_PinState ) RstValue );
//}
//void SX1280_3_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
//{
//    HAL_GPIO_WritePin( SX1280_3_NSS_PORT, SX1280_3_NSS_PIN,( GPIO_PinState ) NSSValue );
//}
//uint32_t SX1280_3_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
//{
//    //return HAL_GPIO_ReadPin( SX1280_3_BUSY_PORT, SX1280_3_BUSY_PIN );
//    return GPIO_ReadInputDataBit( SX1280_3_BUSY_PORT, SX1280_3_BUSY_PIN );
//}

///******SX1280_4的复位，片选，忙信号的管脚操作*********************************/
//void SX1280_4_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
//{
//    HAL_GPIO_WritePin( SX1280_4_nRST_PORT, SX1280_4_nRST_PIN,( GPIO_PinState ) RstValue );

//}
//void SX1280_4_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
//{
//    HAL_GPIO_WritePin( SX1280_4_NSS_PORT, SX1280_4_NSS_PIN,( GPIO_PinState ) NSSValue );
//}
//uint32_t SX1280_4_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
//{
//    //return HAL_GPIO_ReadPin( SX1280_0_BUSY_PORT, SX1280_0_BUSY_PIN );
//    return GPIO_ReadInputDataBit( SX1280_4_BUSY_PORT, SX1280_4_BUSY_PIN );
//}


///******SX1280_4的复位，片选，忙信号的管脚操作*********************************/
//void SX1280_5_SET_RESET(uint8_t RstValue)//设置1280的复位端口：0：RstValue低电平；1：RstValue高电平
//{
//    HAL_GPIO_WritePin( SX1280_5_nRST_PORT, SX1280_5_nRST_PIN,( GPIO_PinState ) RstValue );
//}
//void SX1280_5_SET_NSS(uint8_t NSSValue)//设置1280的SPI1接口片选：0：NSS为低电平；1：NSS为高电平
//{
//    HAL_GPIO_WritePin( SX1280_5_NSS_PORT, SX1280_5_NSS_PIN,( GPIO_PinState ) NSSValue );
//}
//uint32_t SX1280_5_ReadBusyPin( void)//读1280的Busy信号端口：返回高低电平
//{
//    //return HAL_GPIO_ReadPin( SX1280_0_BUSY_PORT, SX1280_0_BUSY_PIN );
//    return GPIO_ReadInputDataBit( SX1280_5_BUSY_PORT, SX1280_5_BUSY_PIN );
//}




//void SX1280_SET_RESET(uint8_t sxcase,uint8_t)
//{
//switch(sxcase)
//{
//	case 0:SX1280_0_SET_RESET();break;
//
//
//
//}
//}



//外部中断的回调函数,用于HAL库
//void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )  //HAL_GPIO_EXTI_Callback
//{
//    GpioLaunchIrqHandler( GPIO_Pin );
//}




/**
  * @brief  Gets IRQ number as a finction of the GPIO_Pin.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
* @retval IRQ number:即根据PIN管脚号，找到对应外部中断函数
  */
IRQn_Type MSP_GetIRQn( uint16_t GPIO_Pin )
{
    switch( GPIO_Pin )
    {
    case GPIO_Pin_0:
        return EXTI0_IRQn;
    case GPIO_Pin_1:
        return EXTI1_IRQn;
    case GPIO_Pin_2:
        return EXTI2_IRQn;
    case GPIO_Pin_3:
        return EXTI3_IRQn;
    case GPIO_Pin_4:
        return EXTI4_IRQn;
    case GPIO_Pin_5:
    case GPIO_Pin_6:
    case GPIO_Pin_7:
    case GPIO_Pin_8:
    case GPIO_Pin_9:
        return EXTI9_5_IRQn;
    case GPIO_Pin_10:
    case GPIO_Pin_11:
    case GPIO_Pin_12:
    case GPIO_Pin_13:
    case GPIO_Pin_14:
    case GPIO_Pin_15:
    default:
        return EXTI15_10_IRQn;
    }
}

/**
  * @brief Provide accurate delay (in milliseconds) based on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay( uint32_t Delay )
{
//    uint32_t tickstart = 0;
//    tickstart = HAL_GetTick( );
//    while( ( HAL_GetTick( ) - tickstart ) < Delay );
    delay_xms(Delay); //直接调用delay.c中的延时函数（不带任务调度）
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler( void )
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while( 1 )
    {
    }
    /* USER CODE END Error_Handler */
}



