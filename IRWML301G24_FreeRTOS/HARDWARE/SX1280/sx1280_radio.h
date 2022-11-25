
#ifndef __RADIO_H__
#define __RADIO_H__
#include <stdint.h>

/*!
 * \brief Define the size of tx and rx hal buffers
 * The Tx and Rx hal buffers are used for SPI communication to
 * store data to be sent/receive to/from the chip.
 * \warning The application must ensure the maximal useful size to be much lower
 *          than the MAX_HAL_BUFFER_SIZE
 */
#define MAX_HAL_BUFFER_SIZE   0xFFF



/*!
 * \brief Class holding the basic communications with a radio
 *
 * It sets the functions to read/write registers, send commands and read/write
 * payload.
 * It also provides functions to run callback functions depending on the
 * interrupts generated from the radio.
 */
struct Radio_s
{
    /*!
     * \brief Initializes the radio
     *
     * \param [IN] callbacks Structure containing the driver callback functions
     */
    void ( *Init )( uint8_t  sxcase );

    /*!
     * \brief Resets the radio
     */
    void ( *Reset )( uint8_t sxcase );

    /*!
     * \brief Gets the current radio status
     *
     * \retval      status        Radio status
     */
    RadioStatus_t ( *GetStatus )( uint8_t  sxcase);

    /*!
     * \brief Writes the given command to the radio
     *
     * \param [in]  opcode        Command opcode
     * \param [in]  buffer        Command parameters byte array
     * \param [in]  size          Command parameters byte array size
     */
    void ( *WriteCommand )( RadioCommands_t opcode, uint8_t *buffer, uint16_t size,uint8_t  sxcase );

    /*!
     * \brief Reads the given command from the radio
     *
     * \param [in]  opcode        Command opcode
     * \param [in]  buffer        Command parameters byte array
     * \param [in]  size          Command parameters byte array size
     */
    void ( *ReadCommand )( RadioCommands_t opcode, uint8_t *buffer, uint16_t size,uint8_t  sxcase);

    /*!
     * \brief Writes multiple radio registers starting at address
     *
     * \param [in]  address       First Radio register address
     * \param [in]  buffer        Buffer containing the new register's values
     * \param [in]  size          Number of registers to be written
     */
    void ( *WriteRegisters )( uint16_t address, uint8_t *buffer, uint16_t size,uint8_t  sxcase );

    /*!
     * \brief Writes the radio register at the specified address
     *
     * \param [in]  address       Register address
     * \param [in]  value         New register value
     */
    void ( *WriteRegister )( uint16_t address, uint8_t value, uint8_t  sxcase );

    /*!
     * \brief Reads multiple radio registers starting at address
     *
     * \param [in]  address       First Radio register address
     * \param [out] buffer        Buffer where to copy the registers data
     * \param [in]  size          Number of registers to be read
     */
    void ( *ReadRegisters )( uint16_t address, uint8_t *buffer, uint16_t size, uint8_t  sxcase );

    /*!
     * \brief Reads the radio register at the specified address
     *
     * \param [in]  address       Register address
     *
     * \retval      value         The register value
     */
    uint8_t ( *ReadRegister )( uint16_t address, uint8_t  sxcase);

    /*!
     * \brief Writes Radio Data Buffer with buffer of size starting at offset.
     *
     * \param [in]  offset        Offset where to start writing
     * \param [in]  buffer        Buffer pointer
     * \param [in]  size          Buffer size
     */
    void ( *WriteBuffer )( uint8_t offset, uint8_t *buffer, uint8_t size, uint8_t  sxcase );

    /*!
     * \brief Reads Radio Data Buffer at offset to buffer of size
     *
     * \param [in]  offset        Offset where to start reading
     * \param [out] buffer        Buffer pointer
     * \param [in]  size          Buffer size
     */
    void ( *ReadBuffer )( uint8_t offset, uint8_t *buffer, uint8_t size, uint8_t  sxcase);

//    /*!
//     * \brief Gets the current status of the radio DIOs
//     *
//     * \retval      status        [Bit #3: DIO3, Bit #2: DIO2,
//     *                             Bit #1: DIO1, Bit #0: BUSY]
//     */
//    uint8_t ( *GetDioStatus )( void );

    /*!
     * \brief Return firmware version
     *
     * \retval      firmware      The firmware version
     */
    uint16_t ( *GetFirmwareVersion )( uint8_t sxcase );

    /*!
     * \brief Sets the power regulators operating mode
     *
     * \param [in]  mode          [0: LDO, 1:DC_DC]
     */
    void ( *SetRegulatorMode )( RadioRegulatorModes_t mode,uint8_t sxcase);

    /*!
     * \brief Sets the radio in configuration mode
     *
     * \param [in]  mode          The standby mode to put the radio into
     */
    void ( *SetStandby )( RadioStandbyModes_t mode,uint8_t  sxcase );

    /*!
     * \brief Sets the radio for the given protocol
     *
     * \param [in]  packetType    [PACKET_TYPE_GFSK, PACKET_TYPE_LORA,
     *                             PACKET_TYPE_RANGING, PACKET_TYPE_FLRC,
     *                             PACKET_TYPE_BLE]
     *
     * \remark This method has to be called before SetRfFrequency,
     *         SetModulationParams and SetPacketParams
     */
    void ( *SetPacketType )( RadioPacketTypes_t packetType,uint8_t sxcase );

    /*!
     * \brief Set the modulation parameters
     *
     * \param [in]  modParams     A structure describing the modulation parameters
     */
    void ( *SetModulationParams )( ModulationParams_t *modParams,uint8_t sxcase);

    /*!
     * \brief Sets the packet parameters
     *
     * \param [in]  packetParams  A structure describing the packet parameters
     */
    void ( *SetPacketParams )( PacketParams_t *packetParams,uint8_t sxcase);

    /*!
     * \brief Sets the RF frequency
     *
     * \param [in]  frequency     RF frequency [Hz]
     */
    void ( *SetRfFrequency )( uint32_t frequency,uint8_t sxcase );

    /*!
     * \brief Sets the data buffer base address for transmission and reception
     *
     * \param [in]  txBaseAddress Transmission base address
     * \param [in]  rxBaseAddress Reception base address
     */
    void ( *SetBufferBaseAddresses )( uint8_t txBaseAddress, uint8_t rxBaseAddress,uint8_t sxcase );

    /*!
     * \brief Sets the transmission parameters
     *
     * \param [in]  power         RF output power [-18..13] dBm
     * \param [in]  rampTime      Transmission ramp up time
     */
    void ( *SetTxParams )( int8_t power, RadioRampTimes_t rampTime,uint8_t sxcase);

    /*!
     * \brief   Sets the IRQ mask and DIO masks
     *
     * \param [in]  irqMask       General IRQ mask
     * \param [in]  dio1Mask      DIO1 mask
     * \param [in]  dio2Mask      DIO2 mask
     * \param [in]  dio3Mask      DIO3 mask
     */
    void ( *SetDioIrqParams )( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask,uint8_t  sxcase );

    /*!
     * \brief Sets the Sync Word given by index used in GFSK, FLRC and BLE protocols
     *
     * \remark 5th byte isn't used in FLRC and BLE protocols
     *
     * \param [in]  syncWordIdx   Index of SyncWord to be set [1..3]
     * \param [in]  syncWord      SyncWord bytes ( 5 bytes )
     *
     * \retval      status        [0: OK, 1: NOK]
     */
    uint8_t ( *SetSyncWord )( uint8_t syncWordIdx, uint8_t *syncWord, uint8_t  sxcase );

    /*!
     * \brief Sets the radio in reception mode
     *
     * \param [in]  timeout       Structure describing the reception timeout value
     */
    void ( *SetRx )( TickTime_t timeout, uint8_t  sxcase  );

    /*!
     * \brief Reads the payload received. If the received payload is longer
     * than maxSize, then the method returns 1 and do not set size and payload.
     *
     * \param [out] payload       A pointer to a buffer into which the payload will be copied
     * \param [out] size          A pointer to the size of the payload received
     * \param [in]  maxSize       The maximal size allowed to copy into the buffer
     */
    uint8_t ( *GetPayload )( uint8_t *payload, uint8_t *size, uint8_t maxSize, uint8_t  sxcase  );

    /*!
     * \brief Sends a payload
     *
     * \param [in]  payload       A pointer to the payload to send
     * \param [in]  size          The size of the payload to send
     * \param [in]  timeout       The timeout for Tx operation
     */
    void ( *SendPayload )( uint8_t *payload, uint8_t size, TickTime_t timeout, uint8_t  sxcase );

    /*!
     * \brief Set the role of the radio during ranging operations
     *
     * \param [in]  role          Role of the radio
     */
    void ( *SetRangingRole )( RadioRangingRoles_t role, uint8_t  sxcase);

    /*!
     * \brief Set the driver in polling mode.
     *
     * In polling mode the application is responsible to call ProcessIrqs( ) to
     * execute callbacks functions.
     * The default mode is Interrupt Mode.
     * @code
     * // Initializations and callbacks declaration/definition
     * radio = SX1280( mosi, miso, sclk, nss, busy, int1, int2, int3, rst, &callbacks );
     * radio.Init( );
     * radio.SetPollingMode( );
     *
     * while( true )
     * {
     *                            //     IRQ processing is automatically done
     *     radio.ProcessIrqs( );  // <-- here, as well as callback functions
     *                            //     calls
     *     // Do some applicative work
     * }
     * @endcode
     *
     * \see SX1280SetInterruptMode
     */
    //void ( *SetPollingMode )( void );

    /*!
     * \brief Set the driver in interrupt mode.
     *
     * In interrupt mode, the driver communicate with the radio during the
     * interruption by direct calls to ProcessIrqs( ). The main advantage is
     * the possibility to have low power application architecture.
     * This is the default mode.
     * @code
     * // Initializations and callbacks declaration/definition
     * radio = SX1280( mosi, miso, sclk, nss, busy, int1, int2, int3, rst, &callbacks );
     * radio.Init( );
     * radio.SetInterruptMode( );   // Optionnal. Driver default behavior
     *
     * while( true )
     * {
     *     // Do some applicative work
     * }
     * @endcode
     *
     * \see SX1280SetPollingMode
     */
    //void ( *SetInterruptMode )( void );

    /*!
     * \brief Initializes the radio registers to the recommended default values
     */
    void ( *SetRegistersDefault )( uint8_t sxcase );

    /*!
     * \brief Gets the current Operation Mode of the Radio
     *
     * \retval      RadioOperatingModes_t last operating mode
     */
    RadioOperatingModes_t ( *GetOpMode )( uint8_t  sxcase );

    /*!
     * \brief Sets the radio in sleep mode
     *
     * \param [in]  sleepConfig   The sleep configuration describing data
     *                            retention and RTC wake-up
     */
    void ( *SetSleep )( SleepParams_t sleepConfig, uint8_t  sxcase );

    /*!
     * \brief Sets the radio in FS mode
     */
    void ( *SetFs )( uint8_t  sxcase );

    /*!
     * \brief Sets the radio in transmission mode
     *
     * \param [in]  timeout       Structure describing the transmission timeout value
     */
    void ( *SetTx )( TickTime_t timeout,uint8_t  sxcase );

    /*!
     * \brief Sets the Rx duty cycle management parameters
     *
     * \param [in]  rxTime        Structure describing reception timeout value
     * \param [in]  sleepTime     Structure describing sleep timeout value
     */
    void ( *SetRxDutyCycle )( RadioTickSizes_t Step, uint16_t NbStepRx, uint16_t RxNbStepSleep,uint8_t  sxcase);

    /*!
     * \brief Sets the radio in CAD mode
     *
     * \see SX1280::SetCadParams
     */
    void ( *SetCad )( uint8_t  sxcase );

    /*!
     * \brief Sets the radio in continuous wave transmission mode
     */
    void ( *SetTxContinuousWave )( uint8_t  sxcase );

    /*!
     * \brief Sets the radio in continuous preamble transmission mode
     */
    void ( *SetTxContinuousPreamble )( uint8_t  sxcase );

    /*!
     * \brief Gets the current radio protocol
     *
     * \retval      packetType    [PACKET_TYPE_GFSK, PACKET_TYPE_LORA,
     *                             PACKET_TYPE_RANGING, PACKET_TYPE_FLRC,
     *                             PACKET_TYPE_BLE, PACKET_TYPE_NONE]
     */
    RadioPacketTypes_t ( *GetPacketType )( uint8_t  sxcase );

    /*!
     * \brief Sets the number of symbols to be used for Channel Activity
     *        Detection operation
     *
     * \param [in]  cadSymbolNum  The number of symbol to use for Channel Activity
     *                            Detection operations [LORA_CAD_01_SYMBOL, LORA_CAD_02_SYMBOL,
     *                            LORA_CAD_04_SYMBOL, LORA_CAD_08_SYMBOL, LORA_CAD_16_SYMBOL]
     */
    void ( *SetCadParams )( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t sxcase );

    /*!
     * \brief Gets the last received packet buffer status
     *
     * \param [out] payloadLength Last received packet payload length
     * \param [out] rxStartBuffer Last received packet buffer address pointer
     */
    void ( *GetRxBufferStatus )( uint8_t *payloadLength, uint8_t *rxStartBuffer,uint8_t sxcase );

    /*!
     * \brief Gets the last received packet payload length
     *
     * \param [out] pktStatus     A structure of packet status
     */
    void ( *GetPacketStatus )( PacketStatus_t *pktStatus,uint8_t  sxcase );

    /*!
     * \brief Returns the instantaneous RSSI value for the last packet received
     *
     * \retval      rssiInst      Instantaneous RSSI
     */
    int8_t ( *GetRssiInst )( uint8_t  sxcase );

    /*!
     * \brief Returns the current IRQ status
     *
     * \retval      irqStatus     IRQ status
     */
    uint16_t ( *GetIrqStatus )( uint8_t  sxcase );

    /*!
     * \brief Clears the IRQs
     *
     * \param [in]  irq           IRQ(s) to be cleared
     */
    void ( *ClearIrqStatus )( uint16_t irq,uint8_t  sxcase);

    /*!
     * \brief Calibrates the given radio block
     *
     * \param [in]  calibParam    The description of blocks to be calibrated
     */
    void ( *Calibrate )( CalibrationParams_t calibParam, uint8_t  sxcase );

    /*!
     * \brief Saves the current selected modem configuration into data RAM
     */
    void ( *SetSaveContext )( uint8_t  sxcase );

    /*!
     * \brief Sets the chip to automatically send a packet after the end of a packet reception
     *
     * \remark The offset is automatically compensated inside the function
     *
     * \param [in]  time          The delay in us after which a Tx is done
     */
    void ( *SetAutoTx )( uint16_t time, uint8_t  sxcase );

    /*!
     * \brief Sets the chip to automatically receive a packet after the end of a packet transmission
     *
     * \remark The offset is automatically compensated inside the function
     *
     * \param [in]  time          The delay in us after which a Rx is done
     */
    void ( *SetAutoFS )( uint8_t enable, uint8_t  sxcase);

    /*!
     * \brief Enables or disables long preamble detection mode
     *
     * \param [in]  enable        [0: Disable, 1: Enable]
     */
    void ( *SetLongPreamble )( uint8_t enable,uint8_t  sxcase );

    /*!
     * \brief Saves the payload to be send in the radio buffer
     *
     * \param [in]  payload       A pointer to the payload
     * \param [in]  size          The size of the payload
     */
    void ( *SetPayload )( uint8_t *payload, uint8_t size,uint8_t  sxcase );

    /*!
     * \brief Sets the Sync Word given by index used in GFSK, FLRC and BLE protocols
     *
     * \remark 5th byte isn't used in FLRC and BLE protocols
     *
     * \param [in]  syncWordIdx   Index of SyncWord to be set [1..3]
     * \param [in]  syncWord      SyncWord bytes ( 5 bytes )
     *
     * \retval      status        [0: OK, 1: NOK]
     */
    void ( *SetSyncWordErrorTolerance )( uint8_t errorBits, uint8_t  sxcase );

    /*!
     * \brief Sets the Initial value for the LFSR used for the CRC calculation
     *
     * \param [in]  seed          Initial LFSR value ( 4 bytes )
     *
     */
    void ( *SetCrcSeed )( uint16_t seed,uint8_t  sxcase );

    /*!
     * \brief Set the Access Address field of BLE packet
     *
     * \param [in]  accessAddress The access address to be used for next BLE packet sent
     *
     * \see SX1280::SetBleAdvertizerAccessAddress
     */
    void ( *SetBleAccessAddress )( uint32_t accessAddress, uint8_t  sxcase );

    /*!
     * \brief Set the Access Address for Advertizer BLE packets
     *
     * All advertizer BLE packets must use a particular value for Access
     * Address field. This method sets it.
     *
     * \see SX1280::SetBleAccessAddress
     */
    void ( *SetBleAdvertizerAccessAddress )( uint8_t  sxcase );


    /*!
     * \brief Sets the seed used for the CRC calculation
     *
     * \param [in]  seed          The seed value
     *
     */
    void ( *SetCrcPolynomial )( uint16_t seed,uint8_t  sxcase );

    /*!
     * \brief Sets the Initial value of the LFSR used for the whitening in GFSK, FLRC and BLE protocols
     *
     * \param [in]  seed          Initial LFSR value
     */
    void ( *SetWhiteningSeed )( uint8_t seed,uint8_t  sxcase );

    /*!
     * \brief Sets the number of bits used to check that ranging request match ranging ID
     *
     * \param [in]  length        [0: 8 bits, 1: 16 bits,
     *                             2: 24 bits, 3: 32 bits]
     */
    void ( *SetRangingIdLength )( RadioRangingIdCheckLengths_t length,uint8_t  sxcase );

    /*!
     * \brief Sets ranging device id
     *
     * \param [in]  address       Device address
     */
    void ( *SetDeviceRangingAddress )( uint32_t address,uint8_t  sxcase );

    /*!
     * \brief Sets the device id to ping in a ranging request
     *
     * \param [in]  address       Address of the device to ping
     */
    void ( *SetRangingRequestAddress )( uint32_t address,uint8_t  sxcase );

    /*!
     * \brief Return the ranging result value
     *
     * \param [in]  resultType    Specifies the type of result.
     *                            [0: RAW, 1: Averaged,
     *                             2: De-biased, 3:Filtered]
     *
     * \retval      ranging       The ranging measure filtered according to resultType [m]
     */
    double ( *GetRangingResult )( RadioRangingResultTypes_t resultType,uint8_t  sxcase );

    /*!
     * \brief Sets the standard processing delay between Master and Slave
     *
     * \param [in]  cal           RxTx delay offset for correcting ranging bias.
     *
     * The calibration value reflects the group delay of the radio front end and
     * must be re-performed for each new SX1280 PCB design. The value is obtained
     * empirically by either conducted measurement in a known electrical length
     * coaxial RF cable (where the design is connectorised) or by radiated
     * measurement, at a known distance, where an antenna is present.
     * The result of the calibration process is that the SX1280 ranging result
     * accurately reflects the physical range, the calibration procedure therefore
     * removes the average timing error from the time-of-flight measurement for a
     * given design.
     *
     * The values are Spreading Factor dependents, and depend also of the board
     * design. Some typical values are provided in the next table.
     *
     * Spreading Factor | Calibration Value
     * ---------------- | -----------------
     *   SF5            |  12200
     *   SF6            |  12200
     *   SF7            |  12400
     *   SF8            |  12650
     *   SF9            |  12940
     *   SF10           |  13000
     *   SF11           |  13060
     *   SF12           |  13120
     */
    void ( *SetRangingCalibration )( uint16_t cal,uint8_t  sxcase );

    /*!
     * \brief Clears the ranging filter
     */
    void ( *RangingClearFilterResult )(uint8_t  sxcase );

    /*!
     * \brief Set the number of samples considered in the built-in filter
     *
     * \param [in]  numSample     The number of samples to use built-in filter
     *                            [8..255]
     *
     * \remark Value inferior to 8 will be silently set to 8
     */
    void ( *RangingSetFilterNumSamples )( uint8_t numSample,uint8_t  sxcase );

    /*!
     * \brief Return the Estimated Frequency Error in LORA and RANGING operations
     *
     * \retval efe                The estimated frequency error [Hz]
     */
    double ( *GetFrequencyError )( uint8_t  sxcase );
};

/*!
 * \brief Radio driver
 *
 * \remark This variable is defined and initialized in the specific radio
 *         board implementation
 */
extern const struct Radio_s Radio;

#endif // __RADIO_H__
