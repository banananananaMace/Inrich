#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stdio.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "exti.h"
#include "spi.h"
#include "DMA.h"
#include "string.h"
#include "malloc.h"
#include "sx1280_hw.h"
#include "sx1280_app.h"
#include "main.h"
#include "stmflash.h"
#include "iwdg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "limits.h"

#include "NodeListFun.h"
#include "SenCTR_CH_Task.h"
#include "SenBS_CH1_Task.h"
#include "UartCom_NetProtocol.h"
#include "UartCom_Protocol.h"

/**任务创建任务**/
#define START_TASK_PRIO			1
#define START_STK_SIZE 			256
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

/**看门狗任务**/
#define IWDG_TASK_PRIO			31
#define IWDG_TASK_SIZE 			128
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

/**数据接收任务**/
#define SX1280RxData_TASK_PRIO	30
#define SX1280RxData_TASK_SIZE 	256
TaskHandle_t SX1280RxData_Task_Handler;
void SX1280RxData_Task(void *pvParameters);

/**传感侧控制信道任务**/
#define SenCTR_CH_TASK_PRIO		28
#define SenCTR_CH_TASK_SIZE 	256
TaskHandle_t SenCTR_CH_Task_Handler;
void SenCTR_CH_Task(void *pvParameters);

/**传感侧业务信道任务**/
#define SenBS_CH1_TASK_PRIO			29
#define SenBS_CH1_TASK_SIZE 		256
TaskHandle_t SenBS_CH1_Task_Handler;
void SenBS_CH1_Task(void *pvParameters);

/**传感器注册发送任务**/
#define SenRegSend_TASK_PRIO		26
#define SenRegSend_TASK_SIZE 		256
TaskHandle_t SenRegSend_Task_Handler;
void SenRegSend_Task(void *pvParameters);


/**串口1发送任务**/
#define USART1_Snd_TASK_PRIO		24
#define USART1_Snd_TASK_SIZE 		256
TaskHandle_t USART1_Snd_Task_Handler;
void USART1_Snd_Task(void *pvParameters);

/**串口1接收任务**/
#define USART1_Rx_TASK_PRIO			25
#define USART1_RX_TASK_SIZE 		256
TaskHandle_t USART1_Rx_Task_Handler;
void USART1_Rx_Task(void *pvParameters);

/**串口2发送任务**/
#define USART2_Snd_TASK_PRIO		22
#define USART2_Snd_TASK_SIZE 		256
TaskHandle_t USART2_Snd_Task_Handler;
void USART2_Snd_Task(void *pvParameters);


/**串口2接收任务**/
#define USART2_Rx_TASK_PRIO			23
#define USART2_RX_TASK_SIZE 		256
TaskHandle_t USART2_Rx_Task_Handler;
void USART2_Rx_Task(void *pvParameters);

/**串口4发送任务**/
#define USART4_Snd_TASK_PRIO		20
#define USART4_Snd_TASK_SIZE 		256
TaskHandle_t USART4_Snd_Task_Handler;
void USART4_Snd_Task(void *pvParameters);


/**串口4接收任务**/
#define USART4_Rx_TASK_PRIO			21
#define USART4_RX_TASK_SIZE 		256
TaskHandle_t USART4_Rx_Task_Handler;
void USART4_Rx_Task(void *pvParameters);

/***事件标志组***/
EventGroupHandle_t SX1280RX_EventGroupHandler = NULL;   //SX1280接收中断事件组
EventGroupHandle_t IWDG_EventGroupHandler = NULL;   	//看门狗事件组
EventGroupHandle_t Common_EventGroupHandler = NULL;   	//常用事件组

/***队列***/
QueueHandle_t SX1280_0_RxData_Q;   					//微功率侧控制信道接收数据的消息队列句柄
QueueHandle_t SX1280_1_RxData_Q;   					//微功率侧业务信道接收数据的消息队列句柄
QueueHandle_t Uart1Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
QueueHandle_t Uart1Rx_MessageData_Q;   				//串口2接收数据的消息队列句柄
QueueHandle_t Uart2Tx_MessageData_Q;   				//串口2发送数据的消息队列句柄
QueueHandle_t Uart2Rx_MessageData_Q;   				//串口2接收数据的消息队列句柄
QueueHandle_t Uart4Tx_MessageData_Q;   				//串口4发送数据的消息队列句柄
QueueHandle_t Uart4Rx_MessageData_Q;   				//串口4接收数据的消息队列句柄
QueueHandle_t RegSensorID_MessageData_Q;   			//需注册ID的消息队列句柄

bool En_debugIP1 = false;  	//优先级最低
bool En_debugIP2 = false;
bool En_debugIP3 = true;
uint8_t  UART1orUART2=1;    //选择串口1还是串口2输出数据：1：选择普通汇聚采用串口1；0：选择中继型汇聚采用串口2；
uint8_t FWVersion[4]= {2,3,0,0};
uint8_t HWVersion[4]= {1,0,0,0};

uint8_t USART1_rx_tempbuf[USART1_REC_LEN];
uint8_t USART2_rx_tempbuf[USART2_REC_LEN];
uint8_t USART4_rx_tempbuf[USART4_REC_LEN];
uint8_t SX1280_BS1_tempbuf[SX1280_RData_Q_LENTH];
uint8_t SX1280_CTR_tempbuf[SX1280_RData_Q_LENTH];
uint8_t RegSensorID_tempbuf[USART1_REC_LEN];


void  Send_uSensorBSData(uint8_t *Redata_pt);
//void  Send_uSensorRegData(uint8_t *Redata_pt);
bool North_UL_isEnable(void);
void SoftReset(void);

int main(void)
{
    //double maiii=0xff01;
    if(en_WATCHDOG)IWDG_Init(6,2000); //与分频数为256,重载值为2000,溢出时间为(256*2000)/32=16s；
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_init(168);
    DMA1_init();
    DMA2_init();
//    uart1_init(115200*4);
//    uart2_init(115200*4);
	uart1_init(115200);
    uart2_init(115200);
    uart4_init(115200);
    LED_Init();
    srand(1000);

//    SensorNodeList_Init();		//名单初始化
//	SensorNodeList_Init_test(); //名单初始化,测试用
    STMFLASH_Read_devicelist(STM32_SAVE_BASE,(uint32_t *)&SensorNode_List,sizeof(SensorNode_List));		 //
    Config_init();
//    STMFLASH_Read_Config(STM32_SAVE_BASE,(uint32_t *)&Config,sizeof(Config));
    sx1280_HwInit();		//初始化SX280所用IO和SPI等
    upHeartBeat_TimeStamp=SelfNode_TimeStamp;
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,
                (const char*    )"start_task",
                (uint16_t       )START_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )START_TASK_PRIO,
                (TaskHandle_t*  )&StartTask_Handler);
    vTaskStartScheduler();          	//开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{

    taskENTER_CRITICAL();           //进入临界区

    /********创建SX1280通道所用的消息队列,***********/
    SX1280_0_RxData_Q=xQueueCreate(SX1280_RData_Q_NUM,SX1280_RData_Q_LENTH);
    SX1280_1_RxData_Q=xQueueCreate(SX1280_RData_Q_NUM,SX1280_RData_Q_LENTH);

    /********创建串口1发送数据所用的消息队列,***********/
    Uart1Tx_MessageData_Q=xQueueCreate(Uart1Tx_MessageData_Q_NUM,Uart1Tx_MessageData_Q_LENTH);
    /********创建串口2接收数据所用的消息队列,***********/
    Uart1Rx_MessageData_Q=xQueueCreate(Uart1Rx_MessageData_Q_NUM,Uart1Rx_MessageData_Q_LENTH); //串口1接收数据的消息队列句柄

    /********创建串口2发送数据所用的消息队列,***********/
    Uart2Tx_MessageData_Q=xQueueCreate(Uart2Tx_MessageData_Q_NUM,Uart2Tx_MessageData_Q_LENTH);
    /********创建串口2接收数据所用的消息队列,***********/
    Uart2Rx_MessageData_Q=xQueueCreate(Uart2Rx_MessageData_Q_NUM,Uart2Rx_MessageData_Q_LENTH); //串口2接收数据的消息队列句柄

    /********创建串口4发送数据所用的消息队列,***********/
    Uart4Tx_MessageData_Q=xQueueCreate(Uart4Tx_MessageData_Q_NUM,Uart4Tx_MessageData_Q_LENTH);
    /********创建串口4接收数据所用的消息队列,***********/
    Uart4Rx_MessageData_Q=xQueueCreate(Uart4Rx_MessageData_Q_NUM,Uart4Rx_MessageData_Q_LENTH); //串口4接收数据的消息队列句柄

    /********创建需注册ID的消息队列,***********/
    RegSensorID_MessageData_Q=xQueueCreate(RegSensorID_MessageData_Q_NUM,RegSensorID_MessageData_Q_LENTH); //串口4接收数据的消息队列句柄



    if((SX1280_0_RxData_Q==0)||(SX1280_1_RxData_Q==0)||(Uart1Tx_MessageData_Q==0)||(Uart1Rx_MessageData_Q==0)||(Uart2Tx_MessageData_Q==0)||(Uart2Rx_MessageData_Q==0)||(Uart4Rx_MessageData_Q==0)||(Uart4Tx_MessageData_Q==0))
    {
        printf("SX1280_QueueCreate err!!!\r\n");
    }

    /********创建SX1280事件组***********/
    SX1280RX_EventGroupHandler = xEventGroupCreate();
    if(SX1280RX_EventGroupHandler == NULL)
    {
        printf("creak SX1280RX_EventGroupHandler failure!\r\n");
    }
    /********创建看门狗事件组***********/
    IWDG_EventGroupHandler = xEventGroupCreate();
    if(IWDG_EventGroupHandler == NULL)
    {
        printf("creak IWDG_EventGroupHandler failure!\r\n");
    }

    /********创建常用事件组***********/
    Common_EventGroupHandler = xEventGroupCreate();
    if(Common_EventGroupHandler == NULL)
    {
        printf("creak Common_EventGroupHandler failure!\r\n");
    }

    //创建看门狗任务
    xTaskCreate((TaskFunction_t )IWDG_Task,
                (const char*    )"IWDG_Task",
                (uint16_t       )IWDG_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )IWDG_TASK_PRIO,
                (TaskHandle_t*  )&IWDG_Task_Handler);

    //创建SX1280数据处理任务
    xTaskCreate((TaskFunction_t )SX1280RxData_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SX1280RxData_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SX1280RxData_TASK_PRIO,
                (TaskHandle_t*  )&SX1280RxData_Task_Handler);

    //创建传感侧控制信道任务
    xTaskCreate((TaskFunction_t )SenCTR_CH_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SenCTR_CH_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenCTR_CH_TASK_PRIO,
                (TaskHandle_t*  )&SenCTR_CH_Task_Handler);

    //创建传感侧业务信道任务
    xTaskCreate((TaskFunction_t )SenBS_CH1_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SenBS_CH1_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenBS_CH1_TASK_PRIO,
                (TaskHandle_t*  )&SenBS_CH1_Task_Handler);

    //创建串口1发送任务
    xTaskCreate((TaskFunction_t )USART1_Snd_Task,
                (const char*    )"USART1_Snd_Task",
                (uint16_t       )USART1_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART1_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART1_Snd_Task_Handler);

    //创建串口1接收任务
    xTaskCreate((TaskFunction_t )USART1_Rx_Task,
                (const char*    )"USART1_Snd_Task",
                (uint16_t       )USART1_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART1_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART1_Rx_Task_Handler);

    //创建串口2发送任务
    xTaskCreate((TaskFunction_t )USART2_Snd_Task,
                (const char*    )"USART2_Snd_Task",
                (uint16_t       )USART2_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART2_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART2_Snd_Task_Handler);

    //创建串口2接收任务
    xTaskCreate((TaskFunction_t )USART2_Rx_Task,
                (const char*    )"USART2_Snd_Task",
                (uint16_t       )USART2_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART2_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART2_Rx_Task_Handler);

    //创建串口4发送任务
    xTaskCreate((TaskFunction_t )USART4_Snd_Task,
                (const char*    )"USART4_Snd_Task",
                (uint16_t       )USART4_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART4_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART4_Snd_Task_Handler);

    //创建串口4接收任务
    xTaskCreate((TaskFunction_t )USART4_Rx_Task,
                (const char*    )"USART4_Snd_Task",
                (uint16_t       )USART4_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART4_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART4_Rx_Task_Handler);

    //传感器注册发送任务
    xTaskCreate((TaskFunction_t )SenRegSend_Task,
                (const char*    )"SenRegSend_TASK",
                (uint16_t       )SenRegSend_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenRegSend_TASK_PRIO,
                (TaskHandle_t*  )&SenRegSend_Task_Handler);

    //必须放在此位置，如果在任务创建之前，由于可能在任务启动之前就有中断产生，则会发生PORT.c 784的错误；
    printf("=============www.inrich.com.cn(AccessNode)===============");
    printf( "\r\n FMVersion:V%d.%d.%d.%d   HWVersion:V%d.%d.%d.%d",FWVersion[0],FWVersion[1],FWVersion[2],FWVersion[3],HWVersion[0],HWVersion[1],HWVersion[2],HWVersion[3]);
    SX1280_Param_Init(0,SenCTR_CH_Fre); 		//初始化微功率侧控制信道SX1280芯片    包括外部中断配置
    SX1280_Param_Init(1,SenBS_CH1_Fre);			//初始化接微功率侧业务1信道SX1280芯片   包括FMVersion外部中断配置
//    SX1280_Param_Init(2,Frequency_list[SenBS_CH2_Fre]);			//初始化接微功率侧业务1信道SX1280芯片   包括外部中断配置
    printf("\r\nControl  channel's FreNum is: %d",SenCTR_CH_Fre);
    printf("\r\nBusiness channel's FreNum is: %d",SenBS_CH1_Fre);
//    printf("\r\n当前接入网业务2频点=%u,频点号=%d",Frequency_list[SenBS_CH2_Fre],SenBS_CH2_Fre);

    printf("\r\nEID：%02X %02X %02X %02X %02X %02X ",Config.D_DID[0],Config.D_DID[1],Config.D_DID[2],Config.D_DID[3],Config.D_DID[4],Config.D_DID[5]);
    printf("\r\nSenList length is: %d",SensorNode_List.Nodenum);
    GetNodeNum_AllType();
    printf("\r\nBLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
    if(UART1orUART2==0)
        printf("\r\nNow is using Com2!");
    else
        printf("\r\nNow is using Com1!");
//    printf("\r\n汇聚节点ID中后四字节（低字节在前,16进制）：%x %x %x %x ",D_ID[2],D_ID[3],D_ID[4],D_ID[5]);
    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();
//    printf("\r\nStart!!");
}

void my_memclear(uint8_t *ptbuf,uint8_t buflength)
{
    uint8_t bufi;
    for(bufi=0; bufi<buflength; bufi++)
    {
        *ptbuf++=0;
    }
}


/*******************************************************
*Function Name	:IWDG_Task
*Description  	:看门狗任务；由于看门狗的最大喂狗时间为(256*2047)/32=16s左右
				1、看门狗任务流程是：（1）每个被监控的任务应该在规定时间内运行至少一次并发送对应标志。
									 （2）本任务在等待标志到达后才能喂狗，同时可以通过哪些标志位未到达（多次）来判断哪些任务有可能挂掉；
				2、由于本应用中其他任务等待时间都是最大值，所以只能监控SX1280RxData_Task

*Input		  	:
*Output		  	:
*Question	  	:1、目前看门狗策略：
				(1)原策略是在5s内同时有3事件（SX1280接收到业务数据，串口发送，串口接收）发生才喂狗，这个概率有些小；而看门狗是N(WATCHDOG_MaxTime参数决定)分钟内不喂狗即重启；
				(2)由于SX1280接收到业务数据后会立即进行串口发送，串口接收心跳是1s周期的，故5s内同时发生在N(WATCHDOG_MaxTime参数决定)分钟内还是可行的，因为N(WATCHDOG_MaxTime参数决定)分钟没有收到SX1280业务数据也可以重启了；
				(3)V2.0.3.30中改为N(WATCHDOG_MaxTime参数决定)分钟，3事件只要都发生过一次即可（在读事件标志时，标志不清零，只有在3件事都发生后清零）
*******************************************************/
void IWDG_Task(void *pvParameters)
{
    EventBits_t wdguxBits;
    EventBits_t resetuxBits;
//    BaseType_t err;
    static uint16_t WDCount=0;
    while(1)
    {
        /**********处理看门口事件*********************/
//        wdguxBits = xEventGroupWaitBits(IWDG_EventGroupHandler, 		// 事件标志组句柄
//                                        EVENTBIT_IWDG_ALL,     			// 等待所有位
//                                        pdTRUE,             			// 等待的位到达后，会被清除
//                                        pdFALSE, 						// 设置为pdTRUE表示当事件组全部到达，才会触发。pdFALSE，一个事件到达就会触发*/
//                                        ( TickType_t )0);				// 阻塞  等待延时 portMAX_DELAY(最大延时)  // ( TickType_t )10 (延时10个节拍)
        wdguxBits=xEventGroupGetBits(IWDG_EventGroupHandler);		// 事件标志组句柄
		if (UART1orUART2)	//采用串口1
        {
            if((wdguxBits & EVENTBIT_IWDG_UART1)==EVENTBIT_IWDG_UART1)
            {
                xEventGroupClearBits(IWDG_EventGroupHandler,EVENTBIT_IWDG_UART1);
				IWDG_Feed();
                WDCount=0;				
            }
            else
            {
                if(WDCount<WATCHDOG_MaxTime)	 //200接近20分钟	//20（约2分钟）次以内喂狗，重启时间在20*5s(阻塞时间)+16=116S;
                {
                    IWDG_Feed();
                    WDCount++;
                }
                else	printf("\r\nIWDG event,uxBits:0x%X  USART1",wdguxBits);
            }
        }
        else//采用串口2
        {
            if((wdguxBits & EVENTBIT_IWDG_UART2)==EVENTBIT_IWDG_UART2)
            {
                 xEventGroupClearBits(IWDG_EventGroupHandler,EVENTBIT_IWDG_UART2);
				IWDG_Feed();
                WDCount=0;
            }
            else
            {
                if(WDCount<WATCHDOG_MaxTime)	//200接近20分钟		//20（约2分钟）次以内喂狗，重启时间在20*5s(延时阻塞时间)+16=116S;
                {
                    IWDG_Feed();
                    WDCount++;
                }
                else	printf("\r\nIWDG event,uxBits:%d  USART2",wdguxBits);
            }
        }
        /**********处理心跳*********************/
        if(SelfNode_TimeStamp>(upHeartBeat_TimeStamp)+15*1000) //超过15s
        {
            upHeartBeat_TimeStamp=SelfNode_TimeStamp;
            Load_HeartBeat();
        }
//        if(SelfNode_TimeStamp>(downHeartBeat_TimeStamp)+DnBeatHeartTime_MAXNUM) //超过30s
//        {
//            downHeartBeat_TimeStamp=SelfNode_TimeStamp;
//            Config.RelayClient_status = RelayClient_OFF;
//            printf("\r\nIWDG_Task：Receive heartbeat timeout.");
//        }

        /**********通道重启*********************/
        resetuxBits = xEventGroupWaitBits(Common_EventGroupHandler, 				// 事件标志组句柄
                                          EVENTBIT_SoftReset_Flag,     				// 等待所有位
                                          pdTRUE,             					// 等待的位到达后，会被清除
                                          pdFALSE, 								// 设置为pdTRUE表示当事件组全部到达，才会触发。pdFALSE，一个事件到达就会触发*/
                                          ( TickType_t )0);				// 阻塞  等待延时 portMAX_DELAY(最大延时)  // ( TickType_t )10 (延时10个节拍)
        if((resetuxBits & EVENTBIT_SoftReset_Flag) == EVENTBIT_SoftReset_Flag)
        {
            printf("\r\n system will reset after 3second!");
            vTaskDelay(3*1000);  //延时5s，也就是5000个时钟节拍
            SoftReset();	// system reset
        }
        vTaskDelay(5*1000);  //延时5s（不能随意改变，涉及到计算喂狗超时时间），也就是5000个时钟节拍 ，不能低于16s,因为需要喂狗
        LED0=!LED0;
    }
}
/*******************************************************
*Function Name	:SX1280RxData_Task
*Description  	:SX1280接收数据任务：即中断事件触发后，从SX1280中读取数据，送入对应队列中；
*Input		  	:注意阻塞时间不能超过看门狗监控时间（最大是16S）
*Output		  	:
*Question	  	:1、
*******************************************************/
void SX1280RxData_Task(void *pvParameters)
{
    EventBits_t uxBits;
    uint8_t sx1280_irpro_result;
    BaseType_t err;
//    uint8_t taski;
//    uint8_t tempbuf[SX_SENS_RECBUF_SIZE]= {0};
    while(1)
    {
        uxBits = xEventGroupWaitBits(SX1280RX_EventGroupHandler, 			// 事件标志组句柄
                                     EVENTBIT_SX1280_ALL,     				// 等待所有位
                                     pdTRUE,             					// 等待的位到达后，会被清除
                                     pdFALSE, 								// 设置为pdTRUE表示当事件组全部到达，才会触发。pdFALSE，一个事件到达就会触发*/
                                     ( TickType_t )60*1000);				// 阻塞  等待延时 portMAX_DELAY(最大延时)  // ( TickType_t )10 (延时10个节拍)
//                                     portMAX_DELAY);				// 阻塞  等待延时 portMAX_DELAY(最大延时)  // ( TickType_t )10 (延时10个节拍)
        //微功率侧控制信道有中断：（接收超时被取消）
        taskENTER_CRITICAL();
        if((uxBits & EVENTBIT_SX1280_SENS0_ISR) == EVENTBIT_SX1280_SENS0_ISR)
        {
            SX1280ProcessIrqs(0);						//SX280中断处理
        }
        sx1280_irpro_result=SX1280_SENS_Pro(0);		//SX1280接收到数据的中断处理
        taskEXIT_CRITICAL();
        switch(sx1280_irpro_result)
        {
        case 0: //空闲
            break;
        case 1: //正确接收到数据
            err=xQueueSend(SX1280_0_RxData_Q,SX_Sens_RecBuf[0],2);//堵塞2ms
            if(err==errQUEUE_FULL)
            {
                printf("\r\n SX1280_0_RxData_Q is full and send fail!");
            }
            //打印所有数据
//				printf("\r\n 微功率侧控制信道收到数据如下：");
//                for(taski=0; taski<SX_Sens_RecBufSize[3]; taski++)
//                    printf(" %d",tempbuf[taski]);
//            my_memclear( SX_Sens_RecBuf[0], SX_SENS_RECBUF_SIZE);  //清缓冲区
            memset(SX_Sens_RecBuf[0],0,SX_SENS_RECBUF_SIZE);  //清缓冲区
            break;
        case 2: //发送成功
            break;
        case 3: //接收超时
            break;
        case 4: //接收出错
            break;
        case 5: //发送超时
            break;
        }

        //微功率侧业务1信道有中断：（接收超时被取消）
        taskENTER_CRITICAL();
        if((uxBits & EVENTBIT_SX1280_SENS1_ISR) == EVENTBIT_SX1280_SENS1_ISR)
        {
            SX1280ProcessIrqs(1);						//SX280中断处理
            LED1=!LED1;
			IO3_LED=!IO3_LED;
        }
        sx1280_irpro_result=SX1280_SENS_Pro(1);		//SX1280接收到数据的中断处理
        taskEXIT_CRITICAL();
        switch(sx1280_irpro_result)
        {
        case 0: //空闲
            break;
        case 1: //正确接收到数据
            err=xQueueSend(SX1280_1_RxData_Q,SX_Sens_RecBuf[1],2);//堵塞2ms
            if(err==errQUEUE_FULL)
            {
                printf("\r\n SX1280_4_RxData_Q is full and send fail!");
            }
            //打印所有数据
//				printf("\r\n 微功率侧业务信道收到数据如下：");
//                for(taski=0; taski<SX_Sens_RecBufSize[4]; taski++)
//                    printf(" %d",tempbuf[taski]);
//            my_memclear( SX_Sens_RecBuf[1], SX_SENS_RECBUF_SIZE);  //清缓冲区
            memset(SX_Sens_RecBuf[1],0,SX_SENS_RECBUF_SIZE);  //清缓冲区 SX_SENS_RECBUF_SIZE
            break;
        case 2: //发送成功
            break;
        case 3: //接收超时
            break;
        case 4: //接收出错
            break;
        case 5: //发送超时
            break;
        }
        /******计算任务剩余堆栈空间********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SX1280RxData_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSX1280RxData_Task剩余堆栈% 3d",uxHighWaterMark);//
        vTaskDelay(3);  //延时3ms，也就是5个时钟节拍
    }
}

/*******************************************************
*Function Name	:SenCTR_CH_Task
*Description  	:微功率侧控制信道接收数据处理（包分类）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void SenCTR_CH_Task(void *pvParameters)
{
    BaseType_t err;
    uint8_t taski;
    uint8_t CurSenID[6];
    uint16_t SenListNum;
//    uint16_t slotorder_test=1;
//    SensorAgent_Str senpt_test ;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(SX1280_0_RxData_Q,SX1280_CTR_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            taski=SenCTR_CH_DataPro(SX1280_CTR_tempbuf);
            switch(taski)
            {
            case 0x11:	//表示收到REQ帧的通信指令参数请求
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //取得传感器ID
                SenListNum=CheckID_isWH(CurSenID); //遍历白名单
                if(SenListNum!=0xFFFF) //表示在白名单中
                {
                    SenCTR_ComPara_REQRSP(SenListNum,0);		//回复RSP_END帧
//                    printf("\r\n 设置了通信参数，其ID是：%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                break;
            case 0x12:	//表示收到REQ帧的业务控制参数请求；
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //取得传感器ID
                SenListNum=CheckID_isWH(CurSenID); //遍历白名单
                if(SenListNum!=0xFFFF) //表示在白名单中
                {
                    SenCTR_CtrPara_REQRSP(SenListNum,0);		//回复RSP_END帧,设置控制参数
//                    printf("\r\n 设置了控制参数，其ID是：%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                break;
            case 0x10:	//表示收到REQ帧的所有控制参数请求；
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //取得传感器ID
                SenListNum=CheckID_isWH(CurSenID); //遍历白名单
                if(SenListNum!=0xFFFF) //表示在白名单中
                {
                    SenCTR_ComPara_REQRSP(SenListNum,1);			//回复RSP帧,
                    delay_xms(20);	//延时5ms；
                    SenCTR_CtrPara_REQRSP(SenListNum,0);		//回复RSP_END帧,设置控制参数
//                    printf("\r\n 先设置了通信参数，后设置了控制参数，其ID是：%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                else //如果不在黑名单中,也不在白名单中，则需要上传该REQ信息
                {
//                    //测试用：如果不在白名单中，则添加到白名单中
//                    uint8_t TempID[4][6];
//                    TempID[0][0]=0xA9;
//                    TempID[0][1]=0x2E;
//                    TempID[0][2]=0x00;
//                    TempID[0][3]=0x0D;
//                    TempID[0][4]=0x01;
//                    TempID[0][5]=0xBA;
//                    TempID[1][0]=0xA9;
//                    TempID[1][1]=0x2E;
//                    TempID[1][2]=0x00;
//                    TempID[1][3]=0x0D;
//                    TempID[1][4]=0x01;
//                    TempID[1][5]=0xBB;
//                    if((memcmp(CurSenID,TempID[0],6)==0)||(memcmp(CurSenID,TempID[1],6)==0))
//                    {
//                        for(taski=0; taski<6; taski++)
//                            senpt_test.SenID[taski]=CurSenID[taski];
//                        senpt_test.SenBS_Cycle=5*60*1000;			//业务周期：5分钟周期
//                        senpt_test.SenREQ_Cycle=0;//5;					//控制周期
//                        senpt_test.SenSlotorder=0;//slotorder_test++;	//时隙
//                        senpt_test.SenShockTime=0;//;					//振荡周期
//                        senpt_test.SenLimitType[0]=0;//(0x08<<2); 		//第一参量的类型：温度物理量;
////                        WhiteList_ADDID(&senpt_test,1);
//                    }
                }
                break;
            case 0x02:	//表示收到非黑名单的告警数据，
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6);  //取得传感器ID
                ///////////////如果在白名单中，就需要发送告警响应////////////////
                SenListNum=CheckID_isWH(CurSenID); //遍历白名单
                if(SenListNum!=0xFFFF) //表示在白名单中
                {
                    BURST_ACK_Send(CurSenID);
                }
                Send_uSensorBSData(SX1280_CTR_tempbuf);	//发送告警数据
                break;
            case 0x03:	//表示收到REQ_ACK
                break;
            }
        }
        /******计算任务剩余堆栈空间********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SenCTR_CH_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSenCTR_CH_Task   剩余堆栈% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //延时5ms，也就是5个时钟节拍
    }
}

/*******************************************************
*Function Name	:SenBS_CH1_Task
*Description  	:微功率侧业务信道1接收数据处理（包分类）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void SenBS_CH1_Task(void *pvParameters)
{
    uint8_t taski;
//    uint8_t bufflength,maclength;
    uint8_t CurSenID[6];
    uint16_t SenID_SN;
    BaseType_t err;
//    uint8_t TempID[4][6];
//    SensorAgent_Str senpt_test ;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(SX1280_1_RxData_Q,SX1280_BS1_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            taski=SenBS_CH1_DataPro(SX1280_BS1_tempbuf);
            switch(taski)
            {
            case 0x00:	//数据格式有问题，不处理；
                break;
            case 0x01: 	//属于黑名单，不处理；
                break;
            case 0x05:	// //即在名单中（属性为：非黑），注册不通过，则查看是否超时，如果超时，则进行重新注册，否则不处理；
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                SenID_SN=FixPosition_InSenList(CurSenID);	//判断是否在列表中
                if(SenID_SN!=0xffff)
                {
                    if(SelfNode_TimeStamp>(SensorNode_List.Register_FailTime [SenID_SN]+Register_FAILTIME_MAX)) //如果该ID的注册失败时间超限                    
                    {
                        //Send_uSensorRegData(CurSenID); //发起传微功率传感器的注册请求帧
                        if(Config.RelayClient_status == RelayClient_ON)
                        {
                            err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//堵塞0ms，只是把ID发送到队列
                            if(err==errQUEUE_FULL)
                                printf("\r\n SenBS_CH1_Task:RegSensorID_MessageData_Q is full and send fail!");
                        }
						SensorNode_List.Register_FailTime [SenID_SN]=SelfNode_TimeStamp;
                    }
                }
                break;
            case 0x02:	//即不在名单中，进行注册
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                /***********再传微功率传感器的注册请求帧（注意不能先传注册，因为传输数据时需要用BSbuf中的原数据）**************/
                if(SensorNode_List.Nodenum<SensorLIST_MAXNUM) //名单未满才能发注册
                {
                    //Send_uSensorRegData(CurSenID); //发起传微功率传感器的注册请求帧
                    if(Config.RelayClient_status == RelayClient_ON)
                    {
                        err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//堵塞0ms，只是把ID发送到队列
                        if(err==errQUEUE_FULL)
                            printf("\r\n SenBS_CH1_Task:1RegSensorID_MessageData_Q is full and send fail!");
                    }
                }
                break;
            case 0x03:	//即在名单中（属性为：非黑），未注册，进行注册
                //取得传感器ID
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                /***********再传微功率传感器的注册请求帧（注意不能先传注册，因为传输数据时需要用BSbuf中的原数据）**************/
                //Send_uSensorRegData(CurSenID);
                if(Config.RelayClient_status == RelayClient_ON)
                {
                    err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//只是把ID发送到队列
                    if(err==errQUEUE_FULL)
                        printf("\r\n SenBS_CH1_Task:2RegSensorID_MessageData_Q is full and send fail!");
                }
                break;
            case 0x04:	// 即在名单中（属性为：非黑），且已经注册成功，则进行数据上传
                Send_uSensorBSData(SX1280_BS1_tempbuf); //发送业务数据
                //发送看门狗监控标志
                xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_SenBS_CH1_TASK);
                break;
            }
        }
        /******计算任务剩余堆栈空间********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SenBS_CH1_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSenBS_CH1_Task   剩余堆栈% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //延时5ms，也就是5个时钟节拍
    }
}


/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:串口发送任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART1_Snd_Task(void *pvParameters)
{
    BaseType_t err;
    uint16_t taski;
//    static uint16_t ParaSetNum=0;
    while(1)
    {
        if(uxQueueMessagesWaiting(Uart1Tx_MessageData_Q)!=0)	//查询队列中入队数量，即保证队列中有信息；
        {
//            if(North_UL_isEnable()==true) //判断当前串口能否使用，如能使用则立即锁定串口，并返回true
            {
                err=xQueueReceive(Uart1Tx_MessageData_Q,USART1_TX_BUF,0);  //portMAX_DELAY
                if(err==pdTRUE) //如果队列中有数据
                {
                    USART1_TX_Lenth=(USART1_TX_BUF[0]<<8)+USART1_TX_BUF[1];	//前俩字节为长度（不包括自身）
                    if(USART1_TX_Lenth>0)
                    {
                        //把要发送的数据拷贝到串口发送缓冲
                        for(taski=0; taski<USART1_TX_Lenth; taski++)
                            USART1_TX_BUF[taski]=USART1_TX_BUF[taski+2];
                        //保证该发送完成的标志位清零
                        if(DMA_GetFlagStatus(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF)!=RESET)   ////等待发送完成
                            DMA_ClearFlag(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
                        //开始发送：DMA操作:在开启前，必须吧发送完成标志TCIF清0,否则DMA不会发送。
                        USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  					//使能串口1的DMA发送
                        DMA_Cmd(UART1_TX_DMA_Stream,DISABLE);							//关闭DMA传输
                        while(DMA_GetCmdStatus(UART1_TX_DMA_Stream)!=DISABLE) {}  		//确保DMA可配置
                        DMA_SetCurrDataCounter(UART1_TX_DMA_Stream,USART1_TX_Lenth);	//设置数据传输量
                        DMA_Cmd(UART1_TX_DMA_Stream,ENABLE);							//开启DMA传输
                        while(DMA_GetFlagStatus(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF)==RESET) {}  ////等待发送完成
                        DMA_ClearFlag(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
//                        printf("\r\nCOM1数据已经发送%d:",USART1_TX_Lenth);
//                        for(taski=0; taski<(5); taski++)
//                            printf(" %02X",USART1_TX_BUF[taski]);
                        delay_xms(1);
                        //发送看门狗监控标志
                        xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART1_Tx_Task);
                    }
                }
//                North_UL_unlock(); //释放串口
            }
        }
        /******计算任务剩余堆栈空间********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( USART1_Snd_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nUSART1_Snd_Task  剩余堆栈% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //延时5ms，也就是5个时钟节拍
    }
}

/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:串口接收数据处理任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART1_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART1_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(Uart1Rx_MessageData_Q,USART1_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            if (UART1orUART2)	//采用串口1
            {
                if( NetProCom_Rx_Pro(USART1_rx_tempbuf,COM1)==true )
                {
                    //发送看门狗监控标志
                    xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART1_Rx_Task);
//                    LED0=!LED0;
                }
            }
        }
        vTaskDelay(5);  //延时5ms，也就是5个时钟节拍
    }
}

/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:串口发送任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART2_Snd_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART2_TX_LEN]= {0};
    uint16_t taski;
    while(1)
    {
        if(uxQueueMessagesWaiting(Uart2Tx_MessageData_Q)!=0)	//查询队列中入队数量，即保证队列中有信息；
        {
//            if(North_UL_isEnable()==true) //判断当前串口能否使用，如能使用则立即锁定串口，并返回true
            {
                //接收队列数据，非堵塞方式
                err=xQueueReceive(Uart2Tx_MessageData_Q,USART2_TX_BUF,0);  //portMAX_DELAY  ( TickType_t )5*1000)
                if(err==pdTRUE) //如果队列中有数据，其实这时候肯定是有信息的；
                {
                    USART2_TX_Lenth=(USART2_TX_BUF[0]<<8)+USART2_TX_BUF[1];	//前俩字节为长度（不包括自身）
                    if(USART2_TX_Lenth>0)
                    {

                        //把要发送的数据拷贝到串口发送缓冲
                        for(taski=0; taski<USART2_TX_Lenth; taski++)
                            USART2_TX_BUF[taski]=USART2_TX_BUF[taski+2];
                        //保证该发送完成的标志位清零
                        if(DMA_GetFlagStatus(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF)!=RESET)   ////等待发送完成
                            DMA_ClearFlag(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
                        //开始发送：DMA操作:在开启前，必须吧发送完成标志TCIF清0,否则DMA不会发送。
                        USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  					//使能串口2的DMA发送
                        DMA_Cmd(UART2_TX_DMA_Stream,DISABLE);							//关闭DMA传输
                        while(DMA_GetCmdStatus(UART2_TX_DMA_Stream)!=DISABLE) {}  		//确保DMA可配置
                        DMA_SetCurrDataCounter(UART2_TX_DMA_Stream,USART2_TX_Lenth);	//设置数据传输量
                        DMA_Cmd(UART2_TX_DMA_Stream,ENABLE);							//开启DMA传输
                        while(DMA_GetFlagStatus(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF)==RESET) {}  ////等待发送完成
                        DMA_ClearFlag(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
//                        printf("\r\nCOM2数据已经发送%d:",USART1_TX_Lenth); //大概花费2ms
////                    	for(taski=0; taski<(USART2_TX_Lenth); taski++)
//                        for(taski=0; taski<(5); taski++)
//                            printf(" %02X",USART2_TX_BUF[taski]);
                        delay_xms(1);
                        //发送看门狗监控标志
                        xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART2_Tx_Task);
                    }
                }
//                North_UL_unlock(); //释放串口
            }
        }
        vTaskDelay(10);  //延时5ms，也就是5个时钟节拍
    }
}


/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:串口接收数据处理任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART2_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART2_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(Uart2Rx_MessageData_Q,USART2_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            if (UART1orUART2==0)	//采用串口2
            {
                if( NetProCom_Rx_Pro(USART2_rx_tempbuf,COM2)==true )
                {
                    //发送看门狗监控标志
                    xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART2_Rx_Task);
//                    LED0=!LED0;
                }
            }
        }
        vTaskDelay(30);  //延时5ms，也就是5个时钟节拍
    }
}



/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:串口发送任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART4_Snd_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART4_TX_LEN]= {0};
    uint16_t taski;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(Uart4Tx_MessageData_Q,USART4_TX_BUF,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            USART4_TX_Lenth=(USART4_TX_BUF[0]<<8)+USART4_TX_BUF[1];	//前俩字节为长度（不包括自身）
            if(USART4_TX_Lenth>0)
            {
                //把要发送的数据拷贝到串口发送缓冲
                for(taski=0; taski<USART4_TX_Lenth; taski++)
                    USART4_TX_BUF[taski]=USART4_TX_BUF[taski+2];
                //保证该发送完成的标志位清零
                if(DMA_GetFlagStatus(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF)!=RESET)   ////等待发送完成
                    DMA_ClearFlag(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
                //开始发送：DMA操作:在开启前，必须吧发送完成标志TCIF清0,否则DMA不会发送。
                USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  					//使能串口4的DMA发送
                DMA_Cmd(UART4_TX_DMA_Stream,DISABLE);							//关闭DMA传输
                while(DMA_GetCmdStatus(UART4_TX_DMA_Stream)!=DISABLE) {}  		//确保DMA可配置
                DMA_SetCurrDataCounter(UART4_TX_DMA_Stream,USART4_TX_Lenth);	//设置数据传输量
                DMA_Cmd(UART4_TX_DMA_Stream,ENABLE);							//开启DMA传输
//            	while(DMA_GetFlagStatus(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF)==RESET) {}  ////等待发送完成
//				DMA_ClearFlag(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF);	//清楚发送完成标志,必须清楚，否则下次DMA不会发送
            }
//
        }
        vTaskDelay(10);  //延时5ms，也就是5个时钟节拍
    }
}


/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:串口接收数据处理任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void USART4_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART4_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //接收队列数据，堵塞方式
        err=xQueueReceive(Uart4Rx_MessageData_Q,USART4_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //如果队列中有数据
        {
            USART_RxData_Pro(USART4_rx_tempbuf,COM4);
        }
        vTaskDelay(50);  //延时5ms，也就是5个时钟节拍
    }
}


/*******************************************************
*Function Name	:SenRegSend_Task
*Description  	:传感器ID注册发送任务（）
*Input		  	:
*Output		  	:
*Question	  	:1、
*******************************************************/
void SenRegSend_Task(void *pvParameters)
{
    uint8_t SenRegIDbuf[6]= {0};
    uint8_t MessageNum=0;
    uint8_t SenRegIDNum=0;
    BaseType_t err;
    uint16_t buffptr=0;
    while(1)
    {
        if(Config.RelayClient_status == RelayClient_ON)
        {
            MessageNum=uxQueueMessagesWaiting(RegSensorID_MessageData_Q); //查询队列中入队数量
            if((MessageNum>0)&&(MessageNum<(RegSensorID_MessageData_Q_NUM+1)))
            {
                memcpy(&RegSensorID_tempbuf[2],CMD_Head,5);//5字节头，2字节长度（从命令开始），1字节命令
				RegSensorID_tempbuf[7]=0x00; 			//2字节长度（从命令开始）高字节：
				RegSensorID_tempbuf[8]=0x00; 			//2字节长度（从命令开始）低字节：
				RegSensorID_tempbuf[9]=0x58; 			//1字节随路命令：				
				RegSensorID_tempbuf[10]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
                memcpy(&RegSensorID_tempbuf[11],Config.D_DID,6);//拷贝末端汇聚EID
                RegSensorID_tempbuf[17]=0x02; 					//网络层指令类型：下属设备注册请求
                RegSensorID_tempbuf[18]=0x03; 				//注册请求指示：0000 0011：微功率传感器，通道编号(微功率通道)
                RegSensorID_tempbuf[19]=0; 					//从设备个数：应该后续有修改
                buffptr=20;
                SenRegIDNum=0;
                while(MessageNum>0)
                {
                    MessageNum--;
                    err=xQueueReceive(RegSensorID_MessageData_Q,SenRegIDbuf,0);  //portMAX_DELAY
                    if(err==pdTRUE) //如果队列中有数据
                    {
                        memcpy(&RegSensorID_tempbuf[buffptr],SenRegIDbuf,6);	//拷贝微功率传感器EID
                        buffptr=buffptr+6;
                        SenRegIDNum++;		//ID数量加
                    }
                    if(buffptr>(USART1_REC_LEN-15))		//防止内存溢出
                    {
                        printf("\r\n SenRegSend_Task:RegSensorID_tempbuf is full!");
                        break;
                    }
                }
                RegSensorID_tempbuf[0]=(buffptr-2+2)>>8;		//串口数据长度修改(不包括自身，故减2)(加2是还有两个CRC字节)
                RegSensorID_tempbuf[1]=buffptr-2+2;
				RegSensorID_tempbuf[7]=(buffptr-9+2)>>8;		//网络包2字节长度（从命令开始）高字节，减(2+7)：(加2是还有两个CRC字节)
				RegSensorID_tempbuf[8]=(buffptr-9+2); 			//				
                RegSensorID_tempbuf[19]=SenRegIDNum;  //从设备个数修改
				uint16_t crcdata=RTU_CRC(&(RegSensorID_tempbuf[2]),buffptr-2);		//CRC
				RegSensorID_tempbuf[buffptr++]=crcdata>>8;
				RegSensorID_tempbuf[buffptr++]=crcdata;				
                if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,RegSensorID_tempbuf,0);//堵塞0ms
                else 				err=xQueueSend(Uart2Tx_MessageData_Q,RegSensorID_tempbuf,0);//堵塞0ms
                if(err==errQUEUE_FULL)
                {
                    printf("\r\n SenRegSend_Task:UartTx_MessageData_Q is full and send fail!");
                }
            }
        }
        vTaskDelay(1*1000);  //延时1s
    }
}


///*******************************************************
//*Function Name	:Send_uSensorRegData
//*Description  	:向串口发送微功率传感器注册请求数据
//*Input			:Redata_pt：6字节ID
//*Output			:
//*Question   	:1、
//*******************************************************/
//void  Send_uSensorRegData(uint8_t *Redata_pt)
//{
//    BaseType_t err;
//    uint8_t maclength;
//    if(Config.RelayClient_status == RelayClient_ON)
//    {
//        /***********微功率传感器的注册请求帧（如果注册与数据需要同时进行，则先传数据，因为传输数据时需要用BSbuf中的原数据）**************/
//        maclength=16;
//        SX1280_BS1_tempbuf[0]=maclength>>8;
//        SX1280_BS1_tempbuf[1]=maclength;
//        SX1280_BS1_tempbuf[2]=0x70; 			//网络帧类型0111 0000：端端；上行；网络层指令；有末端汇聚EID；无末端传感器EID；无端口号；保留为0
//        memcpy(&SX1280_BS1_tempbuf[3],Config.D_DID,6);//拷贝末端汇聚EID
//        SX1280_BS1_tempbuf[9]=0x02; 					//网络层指令类型：下属设备注册请求
//        SX1280_BS1_tempbuf[10]=0x03; 				//注册请求指示：0000 0011：微功率传感器，通道编号(微功率通道)
//        SX1280_BS1_tempbuf[11]=1; 					//从设备个数：应该是1
//        memcpy(&SX1280_BS1_tempbuf[12],Redata_pt,6);	//拷贝微功率传感器EID
//        if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,SX1280_BS1_tempbuf,0);//堵塞0ms
//        else 				err=xQueueSend(Uart2Tx_MessageData_Q,SX1280_BS1_tempbuf,0);//堵塞0ms
//        if(err==errQUEUE_FULL)
//        {
//            printf("\r\n SenBS_CH1_Task2:UartTx_MessageData_Q is full and send fail!");
//        }
//    }
//}

/*******************************************************
*Function Name	:Send_uSensorData
*Description  	:向串口发送微功率传感器业务或告警数据
*Input			:Redata_pt：从sx1280接收到业务数据缓冲区；
*Output			:
*Question   	:1、
*******************************************************/
void  Send_uSensorBSData(uint8_t *Redata_pt)
{
    BaseType_t err;
    uint8_t bufflength,maclength;
    uint8_t CurSenID[6];
    if(Config.RelayClient_status == RelayClient_ON)
    {        
        {
            ///新架构的处理:开头俩字节为长度，后续为需要发送的数据（已经组好网络帧）
            maclength=Redata_pt[1];		//业务帧的MAC长度
            bufflength=Redata_pt[1]+9;	//取得该帧总长度:9=8(帧头)+1（校验）
            if(((bufflength+9)<SX1280_RData_Q_LENTH)&&(maclength>2)) //保证长度不超过最大值：1（网络帧类型）+6（ID）+2（长度）
            {
                memcpy(CurSenID,&Redata_pt[2],6);  //取得传感器ID 
				//新帧中前17字节:为串口数据长度(2byte)，随路帧头(5byte),随路长度(2byte),命令(1byte),网络类型(1byte)和EID(6byte)，
				//后续再加原帧中第三字节开始的数据(即前2字节不需要)，故需要把原帧数据后移动15个字节；
                for(uint16_t taski=bufflength; taski>0; taski--)
                    Redata_pt[taski+14]=Redata_pt[taski-1];	
				
                maclength=maclength+13+10;			//串口数据总长度（不包括自身俩字节）：1（网络帧类型）+6（DID）+6（ID）+随路帧头(5byte)+随路长度(2byte)+命令(1byte)+crc(2byte)
                Redata_pt[0]=maclength>>8;
                Redata_pt[1]=maclength;
				memcpy(&Redata_pt[2],CMD_Head,5); 	//5字节头，
				Redata_pt[7]=(maclength-7)>>8;				//2字节长度（从命令开始）高字节：
				Redata_pt[8]=(maclength-7);		
				Redata_pt[9]=0x58; 					//1字节随路命令：	
                Redata_pt[10]=0x44; 				//网络帧类型0100 0100：端端；上行；负载为数据；无汇聚节点；有微功率传感通信地址；无端口号；保留为0
                memcpy(&Redata_pt[11],CurSenID,6);	//拷贝ID，同时覆盖了原MESSAGE帧的前两个字节
				uint16_t crcdata=RTU_CRC(&(Redata_pt[2]),maclength-2);		//CRC：(1)Redata_pt[2+7]即从第10个字节开始；(2)maclength-7-2即长度需要减掉crc本身两个字节
				Redata_pt[maclength]=crcdata>>8;
				Redata_pt[maclength+1]=crcdata;				
                if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,Redata_pt,0);//堵塞0ms
                else 				err=xQueueSend(Uart2Tx_MessageData_Q,Redata_pt,0);//堵塞0ms
                if(err==errQUEUE_FULL)
                {
                    printf("\r\n SenBS_CH1_Task:3UartTx_MessageData_Q is full and send fail!");
                }
            }
            else
            {
                printf("\r\n the length of sensor data is too long:%d",bufflength);
            }
        }
    }
}

/*******************************************************
*Function Name	:North_UL_isEnable
*Description  	:判断当前串口2能否使用，如能使用则立即锁定串口，并返回true
*Input			:
*Output			:
*Question   	:1、
*******************************************************/
bool North_UL_isEnable(void)
{
    uint16_t randtime;
    //串口锁定 则立即返回
    if(North_UL_islock()==true)
        return false;
    North_UL_lock(); //锁定串口
    randtime =1 + (rand()%1000); //随机数
    delay_us(randtime);
    North_UL_unlock(); //释放串口
    taskENTER_CRITICAL();
    if(North_UL_islock()==true)
	{
        taskEXIT_CRITICAL();
		return false;
	}
    North_UL_lock(); //锁定串口
    taskEXIT_CRITICAL();
    return true;
}


/*******************************************************
*Function Name	:void SoftReset(void)
*Description  	:软件重启
*Input			:
*Output			:
*Question   	:1、
*******************************************************/
void SoftReset(void)
{
//    __disable_fault_irq();
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 复位
}

