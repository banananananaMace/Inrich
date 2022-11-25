
#include "sx1280_hw.h"
#include "sx1280_hal.h"
#include "sx1280_radio.h"
#include <string.h>


#define IRQ_HIGH_PRIORITY_0  6
#define IRQ_HIGH_PRIORITY_1  6
#define IRQ_HIGH_PRIORITY_2  6
#define IRQ_HIGH_PRIORITY_3  6
#define IRQ_HIGH_PRIORITY_4  6
#define IRQ_HIGH_PRIORITY_5  6

static uint8_t halTxBuffer[MAX_HAL_BUFFER_SIZE] = {0x00};
static uint8_t halRxBuffer[MAX_HAL_BUFFER_SIZE] = {0x00};

/// Radio driver structure initialization
const struct Radio_s Radio =
{
    SX1280Init,
    SX1280HalReset,
    SX1280GetStatus,
    SX1280HalWriteCommand,
    SX1280HalReadCommand,
    SX1280HalWriteRegisters,
    SX1280HalWriteRegister,
    SX1280HalReadRegisters,
    SX1280HalReadRegister,
    SX1280HalWriteBuffer,
    SX1280HalReadBuffer,
//    SX1280HalGetDioStatus,
    SX1280GetFirmwareVersion,
    SX1280SetRegulatorMode,       //error
    SX1280SetStandby,
    SX1280SetPacketType,
    SX1280SetModulationParams,
    SX1280SetPacketParams,
    SX1280SetRfFrequency,
    SX1280SetBufferBaseAddresses,
    SX1280SetTxParams,
    SX1280SetDioIrqParams,
    SX1280SetSyncWord,
    SX1280SetRx,
    SX1280GetPayload,
    SX1280SendPayload,
    SX1280SetRangingRole,
    //SX1280SetPollingMode,  //可以取消
    //SX1280SetInterruptMode, //可以取消
    SX1280SetRegistersDefault,
    SX1280GetOpMode,
    SX1280SetSleep,
    SX1280SetFs,
    SX1280SetTx,
    SX1280SetRxDutyCycle,
    SX1280SetCad,
    SX1280SetTxContinuousWave,
    SX1280SetTxContinuousPreamble,
    SX1280GetPacketType,
    SX1280SetCadParams,
    SX1280GetRxBufferStatus,
    SX1280GetPacketStatus,
    SX1280GetRssiInst,
    SX1280GetIrqStatus,
    SX1280ClearIrqStatus,
    SX1280Calibrate,
    SX1280SetSaveContext,
    SX1280SetAutoTx,
    SX1280SetAutoFS,
    SX1280SetLongPreamble,
    SX1280SetPayload,
    SX1280SetSyncWordErrorTolerance,
    SX1280SetCrcSeed,
    SX1280SetBleAccessAddress,
    SX1280SetBleAdvertizerAccessAddress,
    SX1280SetCrcPolynomial,
    SX1280SetWhiteningSeed,
    SX1280SetRangingIdLength,
    SX1280SetDeviceRangingAddress,
    SX1280SetRangingRequestAddress,
    SX1280GetRangingResult,
    SX1280SetRangingCalibration,
    SX1280RangingClearFilterResult,
    SX1280RangingSetFilterNumSamples,
    SX1280GetFrequencyError,
};



//\brief Used to block execution waiting for low state on radio busy pin.        Essentially used in SPI communications
#define MAXTIMEOUT_SX1280  0xFFFFF
void SX1280HalWaitOnBusy(uint8_t sxcase)
{
    uint32_t timeout_1280=0;
	switch(sxcase)
    {
    case 0:
        while( SX1280_0_ReadBusyPin() == 1 )
		{
			timeout_1280++;
			if(timeout_1280>0xFFFFF)
			{
				printf("SX1280_0_ReadBusyPin error!");
				timeout_1280=0;
			}
		}
        break;
    case 1:
        while( SX1280_1_ReadBusyPin() == 1 )
		{
			timeout_1280++;
			if(timeout_1280>0xFFFFF)
			{
				printf("SX1280_1_ReadBusyPin error!");
				timeout_1280=0;
			}
		}
        break;
    case 2:
        while( SX1280_2_ReadBusyPin() == 1 )
		{
			timeout_1280++;
			if(timeout_1280>0xFFFFF)
			{
				printf("SX1280_2_ReadBusyPin error!");
				timeout_1280=0;
			}
		}
		break;
//    case 3:
//        while( SX1280_3_ReadBusyPin() == 1 );
//        break;
//    case 4:
//        while( SX1280_4_ReadBusyPin() == 1 );
//        break;
//    case 5:
//        while( SX1280_5_ReadBusyPin() == 1 );
//        break;
    default:
        break;
    }
}

void SX1280HalInit( DioIrqHandler **irqHandlers,uint8_t sxcase )
{
    SX1280HalReset(sxcase);
    SX1280HalIoIrqInit( irqHandlers,sxcase );
}



void SX1280HalReset( uint8_t sxcase )
{

    switch(sxcase)
    {
    case 0:
    {
        HAL_Delay( 20 );
        HAL_GPIO_WritePin( SX1280_0_nRST_PORT, SX1280_0_nRST_PIN, GPIO_PIN_RESET );
        HAL_Delay( 50 );
        HAL_GPIO_WritePin( SX1280_0_nRST_PORT, SX1280_0_nRST_PIN, GPIO_PIN_SET );
        HAL_Delay( 20 );
    }
    break;
    case 1:

        HAL_Delay( 20 );
        HAL_GPIO_WritePin( SX1280_1_nRST_PORT, SX1280_1_nRST_PIN, GPIO_PIN_RESET );
        HAL_Delay( 50 );
        HAL_GPIO_WritePin( SX1280_1_nRST_PORT, SX1280_1_nRST_PIN, GPIO_PIN_SET );
        HAL_Delay( 20 );
        break;
    case 2:
    {
        HAL_Delay( 20 );
        HAL_GPIO_WritePin( SX1280_2_nRST_PORT, SX1280_2_nRST_PIN, GPIO_PIN_RESET );
        HAL_Delay( 50 );
        HAL_GPIO_WritePin( SX1280_2_nRST_PORT, SX1280_2_nRST_PIN, GPIO_PIN_SET );
        HAL_Delay( 20 );
    }
    break;
//    case 3:
//    {
//        HAL_Delay( 20 );
//        HAL_GPIO_WritePin( SX1280_3_nRST_PORT, SX1280_3_nRST_PIN, GPIO_PIN_RESET );
//        HAL_Delay( 50 );
//        HAL_GPIO_WritePin( SX1280_3_nRST_PORT, SX1280_3_nRST_PIN, GPIO_PIN_SET );
//        HAL_Delay( 20 );
//    }
//    break;
//    case 4:
//    {
//        HAL_Delay( 20 );
//        HAL_GPIO_WritePin( SX1280_4_nRST_PORT, SX1280_4_nRST_PIN, GPIO_PIN_RESET );
//        HAL_Delay( 50 );
//        HAL_GPIO_WritePin( SX1280_4_nRST_PORT, SX1280_4_nRST_PIN, GPIO_PIN_SET );
//        HAL_Delay( 20 );
//    }
//    break;
//    case 5:
//    {
//        HAL_Delay( 20 );
//        HAL_GPIO_WritePin( SX1280_5_nRST_PORT, SX1280_5_nRST_PIN, GPIO_PIN_RESET );
//        HAL_Delay( 50 );
//        HAL_GPIO_WritePin( SX1280_5_nRST_PORT, SX1280_5_nRST_PIN, GPIO_PIN_SET );
//        HAL_Delay( 20 );
//    }
//    break;
    default:
        break;
    }
}

void SX1280HalClearInstructionRam( void )//函数没有整理
{
    // Clearing the instruction RAM is writing 0x00s on every bytes of the
    // instruction RAM
    uint16_t index;
    uint16_t halSize = 3 + IRAM_SIZE;
    halTxBuffer[0] = RADIO_WRITE_REGISTER;
    halTxBuffer[1] = ( IRAM_START_ADDRESS >> 8 ) & 0x00FF;
    halTxBuffer[2] = IRAM_START_ADDRESS & 0x00FF;
    for( index = 0; index < IRAM_SIZE; index++ )
    {
        halTxBuffer[3+index] = 0x00;
    }

    SX1280HalWaitOnBusy(0);       //函数没有整理
    SX1280_0_SET_NSS(0);//HAL_GPIO_WritePin( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
    SPI2_In( halTxBuffer, halSize );
    SX1280_0_SET_NSS(1);//HAL_GPIO_WritePin( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
    SX1280HalWaitOnBusy(0);
}

void SX1280HalWakeup( void )
{

    uint16_t halSize = 2;
    __disable_irq( );
    SX1280_0_SET_NSS(0);//GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
    halTxBuffer[0] = RADIO_GET_STATUS;
    halTxBuffer[1] = 0x00;
    SPI2_In( halTxBuffer, halSize );
    SX1280_0_SET_NSS(1);//GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
    // Wait for chip to be ready.
    SX1280HalWaitOnBusy(0);
    __enable_irq( );
}

void SX1280HalWriteCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size,uint8_t  sxcase)
{
    uint16_t halSize  = size + 1;

    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);//GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
        halTxBuffer[0] = command;
        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
        SPI2_In( halTxBuffer, halSize );
        SX1280_0_SET_NSS(1);//GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
        if( command != RADIO_SET_SLEEP )
            SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        halTxBuffer[0] = command;
        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
        SPI2_In( halTxBuffer, halSize );
        SX1280_1_SET_NSS(1);//GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
        if( command != RADIO_SET_SLEEP )
            SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        halTxBuffer[0] = command;
        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
        SPI2_In( halTxBuffer, halSize );
        SX1280_2_SET_NSS(1);
        if( command != RADIO_SET_SLEEP )
            SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        halTxBuffer[0] = command;
//        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_3_SET_NSS(1);
//        if( command != RADIO_SET_SLEEP )
//            SX1280HalWaitOnBusy(3);
//        break;
//    case 4: //对的
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        halTxBuffer[0] = command;
//        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_4_SET_NSS(1);
//        if( command != RADIO_SET_SLEEP )
//            SX1280HalWaitOnBusy(4);
//        break;
//    case 5: //没写
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        halTxBuffer[0] = command;
//        memcpy( halTxBuffer + 1, ( uint8_t * )buffer, size * sizeof( uint8_t ) );
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_5_SET_NSS(1);
//        if( command != RADIO_SET_SLEEP )
//            SX1280HalWaitOnBusy(5);
//        break;
    default:
        break;
    }
}

void SX1280HalReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size,uint8_t  sxcase )
{
    uint16_t index;
    uint16_t halSize = 2 + size;
    halTxBuffer[0] = command;
    halTxBuffer[1] = 0x00;
    for( index = 0; index < size; index++ )
    {
        halTxBuffer[2+index] = 0x00;
    }
    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 2, size );
        SX1280_0_SET_NSS(1);
        SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 2, size );
        SX1280_1_SET_NSS(1);
        SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 2, size );
        SX1280_2_SET_NSS(1);
        SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 2, size );
//        SX1280_3_SET_NSS(1);
//        SX1280HalWaitOnBusy(3);
//        break;
//    case 4:
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 2, size );
//        SX1280_4_SET_NSS(1);
//        SX1280HalWaitOnBusy(4);
//        break;
//    case 5:
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 2, size );
//        SX1280_5_SET_NSS(1);
//        SX1280HalWaitOnBusy(5);
//        break;
    default:
        break;
    }
}

void SX1280HalWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size,uint8_t  sxcase )
{
    uint16_t halSize = size + 3;
    halTxBuffer[0] = RADIO_WRITE_REGISTER;
    halTxBuffer[1] = ( address & 0xFF00 ) >> 8;
    halTxBuffer[2] = address & 0x00FF;
    memcpy( halTxBuffer + 3, buffer, size );
    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_0_SET_NSS(1);
        SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_1_SET_NSS(1);
        SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_2_SET_NSS(1);
        SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_3_SET_NSS(1);
//        SX1280HalWaitOnBusy(3);
//        break;
//    case 4:
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_4_SET_NSS(1);
//        SX1280HalWaitOnBusy(4);
//        break;
//    case 5:
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_5_SET_NSS(1);
//        SX1280HalWaitOnBusy(5);
//        break;
    default:
        break;
    }

}

void SX1280HalWriteRegister( uint16_t address, uint8_t value, uint8_t  sxcase )
{
    SX1280HalWriteRegisters( address, &value, 1,sxcase );
}

void SX1280HalReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size, uint8_t  sxcase)
{
    uint16_t index;
    uint16_t halSize = 4 + size;
    halTxBuffer[0] = RADIO_READ_REGISTER;
    halTxBuffer[1] = ( address & 0xFF00 ) >> 8;
    halTxBuffer[2] = address & 0x00FF;
    halTxBuffer[3] = 0x00;
    for( index = 0; index < size; index++ )
    {
        halTxBuffer[4+index] = 0x00;
    }
    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);//片选验证过，正确信号
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 4, size );
        SX1280_0_SET_NSS(1);
        SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 4, size );
        SX1280_1_SET_NSS(1);
        SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 4, size );
        SX1280_2_SET_NSS(1);
        SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 4, size );
//        SX1280_3_SET_NSS(1);
//        SX1280HalWaitOnBusy(3);
//        break;
//    case 4:  //对的
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 4, size );
//        SX1280_4_SET_NSS(1);
//        SX1280HalWaitOnBusy(4);
//        break;
//    case 5:
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 4, size );
//        SX1280_5_SET_NSS(1);
//        SX1280HalWaitOnBusy(5);
//        break;
    default:
        break;
    }
}

uint8_t SX1280HalReadRegister( uint16_t address, uint8_t  sxcase)
{
    uint8_t data;
    SX1280HalReadRegisters( address, &data, 1,sxcase );
    return data;
}

void SX1280HalWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size, uint8_t  sxcase )
{
    uint16_t halSize = size + 2;
    halTxBuffer[0] = RADIO_WRITE_BUFFER;
    halTxBuffer[1] = ( offset ) >> 8;
    memcpy( halTxBuffer + 2, buffer, size );
    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_0_SET_NSS(1);
        SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_1_SET_NSS(1);
        SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        SPI2_In( halTxBuffer, halSize );
        SX1280_2_SET_NSS(1);
        SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_3_SET_NSS(1);
//        SX1280HalWaitOnBusy(3);
//    case 4:
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_4_SET_NSS(1);
//        SX1280HalWaitOnBusy(4);
//        break;
//    case 5:
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        SPI2_In( halTxBuffer, halSize );
//        SX1280_5_SET_NSS(1);
//        SX1280HalWaitOnBusy(5);
    default:
        break;
    }
}

void SX1280HalReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size, uint8_t  sxcase)
{
    uint16_t index;
    uint16_t halSize = size + 3;
    halTxBuffer[0] = RADIO_READ_BUFFER;
    halTxBuffer[1] = offset;
    halTxBuffer[2] = 0x00;
    for( index = 0; index < size; index++ )
    {
        halTxBuffer[3+index] = 0x00;
    }
    switch(sxcase)
    {
    case 0:
        SX1280HalWaitOnBusy(0);
        SX1280_0_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 3, size );
        SX1280_0_SET_NSS(1);
        SX1280HalWaitOnBusy(0);
        break;
    case 1:
        SX1280HalWaitOnBusy(1);
        SX1280_1_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 3, size );
        SX1280_1_SET_NSS(1);
        SX1280HalWaitOnBusy(1);
        break;
    case 2:
        SX1280HalWaitOnBusy(2);
        SX1280_2_SET_NSS(0);
        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
        memcpy( buffer, halRxBuffer + 3, size );
        SX1280_2_SET_NSS(1);
        SX1280HalWaitOnBusy(2);
        break;
//    case 3:
//        SX1280HalWaitOnBusy(3);
//        SX1280_3_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 3, size );
//        SX1280_3_SET_NSS(1);
//        SX1280HalWaitOnBusy(3);
//        break;
//    case 4:
//        SX1280HalWaitOnBusy(4);
//        SX1280_4_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 3, size );
//        SX1280_4_SET_NSS(1);
//        SX1280HalWaitOnBusy(4);
//        break;
//    case 5:
//        SX1280HalWaitOnBusy(5);
//        SX1280_5_SET_NSS(0);
//        SPI2_InOut( halTxBuffer, halRxBuffer, halSize );
//        memcpy( buffer, halRxBuffer + 3, size );
//        SX1280_5_SET_NSS(1);
//        SX1280HalWaitOnBusy(5);
//        break;
    default:
        break;
    }
}

void SX1280HalIoIrqInit( DioIrqHandler **irqHandlers,uint8_t sxcase )
{
//    switch(sxcase)
//    {
//case 0:
//        GpioSetIrq( SX1280_0_DIO1_PORT, SX1280_0_DIO1_PIN, IRQ_HIGH_PRIORITY_0, irqHandlers[0] );
//        break;
//case 1:
//        GpioSetIrq( SX1280_1_DIO1_PORT, SX1280_1_DIO1_PIN, IRQ_HIGH_PRIORITY_1, irqHandlers[1] );
//        break;
//case 2:
//        GpioSetIrq( SX1280_2_DIO1_PORT, SX1280_2_DIO1_PIN, IRQ_HIGH_PRIORITY_2, irqHandlers[2] );
//        break;
//case 3:
//        GpioSetIrq( SX1280_3_DIO1_PORT, SX1280_3_DIO1_PIN, IRQ_HIGH_PRIORITY_3, irqHandlers[3] );
//        break;
//case 4:
//        GpioSetIrq( SX1280_4_DIO1_PORT, SX1280_4_DIO1_PIN, IRQ_HIGH_PRIORITY_4, irqHandlers[4] );
//	    	break;
//case 5:
//        GpioSetIrq( SX1280_5_DIO1_PORT, SX1280_5_DIO1_PIN, IRQ_HIGH_PRIORITY_5, irqHandlers[5] );
//		   break;
//      default:
//        break;
//    }
}



//uint8_t SX1280HalGetDioStatus( void )
//{
//    //return ( GpioRead( RADIO_DIOx_PORT, RADIO_DIOx_PIN ) << 1 ) | ( GpioRead( RADIO_BUSY_PORT, RADIO_BUSY_PIN ) << 0 );
//    return ( SX1280_0_ReadDIO1() << 1 ) | ( SX1280_1_ReadBusyPin() << 0 );

//}
