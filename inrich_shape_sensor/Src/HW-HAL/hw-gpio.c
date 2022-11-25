#include "hw.h"

static GpioIrqHandler *GpioIrq[16] = { NULL };

void GpioInit( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE( );
    __HAL_RCC_GPIOB_CLK_ENABLE( );
    __HAL_RCC_GPIOC_CLK_ENABLE( );
//    __HAL_RCC_GPIOH_CLK_ENABLE( );
	
	  //SX1280
			/*Configure GPIO pins : nRESET_Pin RADIO_NSS_Pin */
    GPIO_InitStruct.Pin =RADIO_nRESET_PIN | RADIO_NSS_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
	
	    /*Configure GPIO pin : BUSY_Pin */
    GPIO_InitStruct.Pin = RADIO_BUSY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;    //输入模式
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init( RADIO_BUSY_PORT, &GPIO_InitStruct );
	
	
    /*Configure GPIO pin : DIO1_Pin */
    GPIO_InitStruct.Pin = RADIO_DIO1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		//GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( RADIO_DIO_PORT, &GPIO_InitStruct);
	
	  GpioWrite( GPIOA,  RADIO_NSS_PIN | RADIO_nRESET_PIN, GPIO_PIN_SET );
	
	  //A端口外设IO
	
    GPIO_InitStruct.Pin =  IO_VBAT_PinA1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
	  GpioWrite( GPIOA, IO_VBAT_PinA1 ,GPIO_PIN_SET);//LED置低 熄灭
	
	
	 //B端口外设IO
	 
    /*Configure GPIO pins : LED_Pin */
    GPIO_InitStruct.Pin =  LED_PinB3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
	  GpioWrite( GPIOB, LED_PinB3 , GPIO_PIN_RESET );//LED置低 熄灭
	
	
	
	  GPIO_InitStruct.Pin = IO_VCC1_PinB0|IO_VCC2_PinB1|IO_VCC3_PinB5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
		GpioWrite( GPIOB, IO_VCC1_PinB0 | IO_VCC2_PinB1 | IO_VCC3_PinB5, GPIO_PIN_SET );
		
}




void GpioDeInit( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
		
		GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
		
	 
    /*Configure GPIO pins : PA1 PA4 PA9 PA10 
                           PA11 PA12 PA15 */
//    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3 
//									| GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 ;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    
       
    
      /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE( );
    __HAL_RCC_GPIOB_CLK_DISABLE( );
    __HAL_RCC_GPIOC_CLK_DISABLE( );
    __HAL_RCC_GPIOH_CLK_DISABLE( );
//    __HAL_RCC_GPIOE_CLK_DISABLE( );
//    __HAL_RCC_GPIOF_CLK_DISABLE( );
//    __HAL_RCC_GPIOG_CLK_DISABLE( );
//    __HAL_RCC_GPIOH_CLK_DISABLE( );
}



/*!
 * @brief Records the interrupt handler for the GPIO  object
 *
 * @param  GPIOx: where x can be (A..E and H) 
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] prio       NVIC priority (0 is highest)
 * @param [IN] irqHandler  points to the  function to execute
 * @retval none
 */
void GpioSetIrq( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler )
{
    IRQn_Type IRQnb;

    uint32_t BitPos = GpioGetBitPos( GPIO_Pin ) ;

    if ( irqHandler != NULL )
    {
        GpioIrq[BitPos] = irqHandler;

        IRQnb = MSP_GetIRQn( GPIO_Pin );

        HAL_NVIC_SetPriority( IRQnb , prio, 0 );

        HAL_NVIC_EnableIRQ( IRQnb );
    }
}

/*!
 * @brief Execute the interrupt from the object
 *
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval none
 */
void GpioLaunchIrqHandler( uint16_t GPIO_Pin )
{
    uint32_t BitPos = GpioGetBitPos( GPIO_Pin );

    if ( GpioIrq[BitPos]  != NULL )
    {
        GpioIrq[BitPos]( );
    }
}


/*!
 * @brief Get the position of the bit set in the GPIO_Pin
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval the position of the bit
 */
uint8_t GpioGetBitPos( uint16_t GPIO_Pin )
{
    uint8_t PinPos=0;

    if ( ( GPIO_Pin & 0xFF00 ) != 0 ) { PinPos |= 0x8; }
    if ( ( GPIO_Pin & 0xF0F0 ) != 0 ) { PinPos |= 0x4; }
    if ( ( GPIO_Pin & 0xCCCC ) != 0 ) { PinPos |= 0x2; }
    if ( ( GPIO_Pin & 0xAAAA ) != 0 ) { PinPos |= 0x1; }

    return PinPos;
}


/*!
 * @brief Writes the given value to the GPIO output
 *
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] value New GPIO output value
 * @retval none
 */
void GpioWrite( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t value )
{
    HAL_GPIO_WritePin( GPIOx, GPIO_Pin , ( GPIO_PinState ) value );
}

/*!
 * @brief Reads the current GPIO input value
 *
 * @param  GPIOx: where x can be (A..E and H) 
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval value   Current GPIO input value
 */
uint32_t GpioRead( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin )
{
    return HAL_GPIO_ReadPin( GPIOx, GPIO_Pin );
}
