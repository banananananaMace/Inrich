#include "sx1280_hw.h"

void EXTIX_Init(uint8_t GPortx, uint16_t GPIO_Pin, uint32_t prio);
//uint8_t PortGetBitPos( GPIO_TypeDef* GPIOx );
uint8_t GpioGetBitPos( uint16_t GPIO_Pin );
//static GpioIrqHandler *GpioIrq[16] = { NULL };

//包括Busy,DIO1(外部中断口)，复位口，SPI的片选管脚
void GpioInit( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* GPIO Ports Clock Enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA,GPIOB时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOC,GPIOD时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOC,GPIOD时钟
    /******初始化SX1280_0*********************************/
    GPIO_InitStructure.GPIO_Pin = SX1280_0_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init( SX1280_0_NSS_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_0_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
    GPIO_Init( SX1280_0_BUSY_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的复位管脚：/
    GPIO_InitStructure.GPIO_Pin  = SX1280_0_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_Init( SX1280_0_nRST_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的DIO1 :PC1外部中断管脚
    GPIO_InitStructure.GPIO_Pin  = SX1280_0_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//下拉,//上升沿触发中断
    GPIO_Init( SX1280_0_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_0_EXTI_PORT,SX1280_0_DIO1_PIN,7);//初始化外部中断，同时需要更改对应的中断函数：EXTI1_IRQHandler

    /******初始化SX1280_1*********************************/
    //初始化(即SX1280_1)片选：SPI1_NSS__CTS1_Pin   PA0
    GPIO_InitStructure.GPIO_Pin = SX1280_1_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init( SX1280_1_NSS_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_1_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
    GPIO_Init( SX1280_1_BUSY_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的复位管脚：/
    GPIO_InitStructure.GPIO_Pin  = SX1280_1_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_Init( SX1280_1_nRST_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的DIO1 :PC1外部中断管脚
    GPIO_InitStructure.GPIO_Pin  = SX1280_1_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//下拉,//上升沿触发中断
    GPIO_Init( SX1280_1_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_1_EXTI_PORT,SX1280_1_DIO1_PIN,8);//初始化外部中断，同时需要更改对应的中断函数：EXTI1_IRQHandler
    /******初始化SX1280_2*********************************/
    //初始化(即SX1280_2)片选：SPI1_NSS__CTS1_Pin   PA0
    GPIO_InitStructure.GPIO_Pin = SX1280_2_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init( SX1280_2_NSS_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的BUSY_Pin: PC0
    GPIO_InitStructure.GPIO_Pin = SX1280_2_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
    GPIO_Init( SX1280_2_BUSY_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的复位管脚：/
    GPIO_InitStructure.GPIO_Pin  = SX1280_2_nRST_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz
    GPIO_Init( SX1280_2_nRST_PORT, &GPIO_InitStructure );
    //初始化SX1280_0的DIO1 :PC1外部中断管脚
    GPIO_InitStructure.GPIO_Pin  = SX1280_2_DIO1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;//下拉,//上升沿触发中断
    GPIO_Init( SX1280_2_DIO1_PORT, &GPIO_InitStructure);
    EXTIX_Init(SX1280_2_EXTI_PORT,SX1280_2_DIO1_PIN,9);//初始化外部中断，同时需要更改对应的中断函数：EXTI1_IRQHandler
}


//外部中断初始化程序
void EXTIX_Init(uint8_t GPortx, uint16_t GPIO_Pin, uint32_t prio)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

    IRQn_Type IRQnb;
    uint8_t BitPos = GpioGetBitPos( GPIO_Pin ) ;
    //uint8_t PortPos = PortGetBitPos( GPIOG ) ;

    //GpioIrq[BitPos] = irqHandler;  //
    IRQnb = MSP_GetIRQn( GPIO_Pin ); //根据管脚号，找到对应的EXTI（？）_IRQn中断入口函数；本例应为PIN=1,即返回值IRQnb=EXTI1_IRQn

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 连接到中断线4
    SYSCFG_EXTILineConfig(GPortx, BitPos);//

    EXTI_InitStructure.EXTI_Line = GPIO_Pin;//EXTI_Line0 本参数范围是1，2，4，8，16，32,故可以用GPIO_Pin来替代EXTI_Line
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
    EXTI_Init(&EXTI_InitStructure);//配置

    NVIC_InitStructure.NVIC_IRQChannel =IRQnb; // EXTI0_IRQn;//根据GPIO_Pin通过MSP_GetIRQn函数可以得到对应的EXTI？_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = prio;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}


///*******************************************************
//*Function Name 	:GpioGetBitPos
//*Description  	: 根据GPIO_Pin（其值范围是1，2，4，8，16）返回该PIN是几号管脚，返回值范围为0~15；
//									故GPIO_Pin_0（原值为0x0001）对应返回值为0（0号管脚），GPIO_Pin_15（原值为0x8000）对应返回值为15（15号管脚）
//*Input					: GPIO_Pin（其值范围是1，2，4，8，16。。。）
//*Output					: GPIO_Pin的管脚号（0~15）；
//*******************************************************/
uint8_t GpioGetBitPos( uint16_t GPIO_Pin )
{
    uint8_t PinPos=0;
    if ( ( GPIO_Pin & 0xFF00 ) != 0 ) {
        PinPos |= 0x8;
    }
    if ( ( GPIO_Pin & 0xF0F0 ) != 0 ) {
        PinPos |= 0x4;
    }
    if ( ( GPIO_Pin & 0xCCCC ) != 0 ) {
        PinPos |= 0x2;
    }
    if ( ( GPIO_Pin & 0xAAAA ) != 0 ) {
        PinPos |= 0x1;
    }
    return PinPos;
}
//void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
//{
//  uint32_t prioritygroup = 0x00;

//  /* Check the parameters */
//  assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
//  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
//  prioritygroup = NVIC_GetPriorityGrouping();
//  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
//}
//void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
//{
//  /* Check the parameters */
//  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));
//
//  /* Enable interrupt */
//  NVIC_EnableIRQ(IRQn);
//}
//void GpioSetIrq( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler )
//{
//    IRQn_Type IRQnb;
//    uint32_t BitPos = GpioGetBitPos( GPIO_Pin ) ;
//    if ( irqHandler != NULL )
//    {
//        // GpioIrq[BitPos]被真正的回调函数GpioLaunchIrqHandler调用，故irqHandler是外部中断函数的主体；
//        // GpioSetIrq上一函数是SX1280HalIoIrqInit，再上一级是SX1280HalInit，其对应参数是DioIrq；
//        //故DioIrq是真正的外部中断函数主体，即  SX1280OnDioIrq  ；
//        GpioIrq[BitPos] = irqHandler;
//        IRQnb = MSP_GetIRQn( GPIO_Pin ); //根据管脚号，找到对应的EXTI（？）_IRQn中断入口函数；本例应为PIN=1,即返回值IRQnb=EXTI1_IRQn
//   //   HAL_NVIC_SetPriority( IRQnb, 1, prio );  //即相当于 HAL_NVIC_SetPriority(EXTI1_IRQn,0,0);
//			  HAL_NVIC_SetPriority( IRQnb, 5, prio );  //即相当于 HAL_NVIC_SetPriority(EXTI1_IRQn,0,0);
//        HAL_NVIC_EnableIRQ( IRQnb );  //使能外部中断（这里是外部中断1）
//    }
//}


//void GpioLaunchIrqHandler( uint16_t GPIO_Pin )
//{
//    uint32_t BitPos = GpioGetBitPos( GPIO_Pin );
//    if ( GpioIrq[BitPos]  != NULL )
//    {
//        GpioIrq[BitPos]( );
//    }
//}


