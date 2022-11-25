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

/**���񴴽�����**/
#define START_TASK_PRIO			1
#define START_STK_SIZE 			256
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

/**���Ź�����**/
#define IWDG_TASK_PRIO			31
#define IWDG_TASK_SIZE 			128
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

/**���ݽ�������**/
#define SX1280RxData_TASK_PRIO	30
#define SX1280RxData_TASK_SIZE 	256
TaskHandle_t SX1280RxData_Task_Handler;
void SX1280RxData_Task(void *pvParameters);

/**���в�����ŵ�����**/
#define SenCTR_CH_TASK_PRIO		28
#define SenCTR_CH_TASK_SIZE 	256
TaskHandle_t SenCTR_CH_Task_Handler;
void SenCTR_CH_Task(void *pvParameters);

/**���в�ҵ���ŵ�����**/
#define SenBS_CH1_TASK_PRIO			29
#define SenBS_CH1_TASK_SIZE 		256
TaskHandle_t SenBS_CH1_Task_Handler;
void SenBS_CH1_Task(void *pvParameters);

/**������ע�ᷢ������**/
#define SenRegSend_TASK_PRIO		26
#define SenRegSend_TASK_SIZE 		256
TaskHandle_t SenRegSend_Task_Handler;
void SenRegSend_Task(void *pvParameters);


/**����1��������**/
#define USART1_Snd_TASK_PRIO		24
#define USART1_Snd_TASK_SIZE 		256
TaskHandle_t USART1_Snd_Task_Handler;
void USART1_Snd_Task(void *pvParameters);

/**����1��������**/
#define USART1_Rx_TASK_PRIO			25
#define USART1_RX_TASK_SIZE 		256
TaskHandle_t USART1_Rx_Task_Handler;
void USART1_Rx_Task(void *pvParameters);

/**����2��������**/
#define USART2_Snd_TASK_PRIO		22
#define USART2_Snd_TASK_SIZE 		256
TaskHandle_t USART2_Snd_Task_Handler;
void USART2_Snd_Task(void *pvParameters);


/**����2��������**/
#define USART2_Rx_TASK_PRIO			23
#define USART2_RX_TASK_SIZE 		256
TaskHandle_t USART2_Rx_Task_Handler;
void USART2_Rx_Task(void *pvParameters);

/**����4��������**/
#define USART4_Snd_TASK_PRIO		20
#define USART4_Snd_TASK_SIZE 		256
TaskHandle_t USART4_Snd_Task_Handler;
void USART4_Snd_Task(void *pvParameters);


/**����4��������**/
#define USART4_Rx_TASK_PRIO			21
#define USART4_RX_TASK_SIZE 		256
TaskHandle_t USART4_Rx_Task_Handler;
void USART4_Rx_Task(void *pvParameters);

/***�¼���־��***/
EventGroupHandle_t SX1280RX_EventGroupHandler = NULL;   //SX1280�����ж��¼���
EventGroupHandle_t IWDG_EventGroupHandler = NULL;   	//���Ź��¼���
EventGroupHandle_t Common_EventGroupHandler = NULL;   	//�����¼���

/***����***/
QueueHandle_t SX1280_0_RxData_Q;   					//΢���ʲ�����ŵ��������ݵ���Ϣ���о��
QueueHandle_t SX1280_1_RxData_Q;   					//΢���ʲ�ҵ���ŵ��������ݵ���Ϣ���о��
QueueHandle_t Uart1Tx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
QueueHandle_t Uart1Rx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
QueueHandle_t Uart2Tx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
QueueHandle_t Uart2Rx_MessageData_Q;   				//����2�������ݵ���Ϣ���о��
QueueHandle_t Uart4Tx_MessageData_Q;   				//����4�������ݵ���Ϣ���о��
QueueHandle_t Uart4Rx_MessageData_Q;   				//����4�������ݵ���Ϣ���о��
QueueHandle_t RegSensorID_MessageData_Q;   			//��ע��ID����Ϣ���о��

bool En_debugIP1 = false;  	//���ȼ����
bool En_debugIP2 = false;
bool En_debugIP3 = true;
uint8_t  UART1orUART2=1;    //ѡ�񴮿�1���Ǵ���2������ݣ�1��ѡ����ͨ��۲��ô���1��0��ѡ���м��ͻ�۲��ô���2��
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
    if(en_WATCHDOG)IWDG_Init(6,2000); //���Ƶ��Ϊ256,����ֵΪ2000,���ʱ��Ϊ(256*2000)/32=16s��
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

//    SensorNodeList_Init();		//������ʼ��
//	SensorNodeList_Init_test(); //������ʼ��,������
    STMFLASH_Read_devicelist(STM32_SAVE_BASE,(uint32_t *)&SensorNode_List,sizeof(SensorNode_List));		 //
    Config_init();
//    STMFLASH_Read_Config(STM32_SAVE_BASE,(uint32_t *)&Config,sizeof(Config));
    sx1280_HwInit();		//��ʼ��SX280����IO��SPI��
    upHeartBeat_TimeStamp=SelfNode_TimeStamp;
    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,
                (const char*    )"start_task",
                (uint16_t       )START_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )START_TASK_PRIO,
                (TaskHandle_t*  )&StartTask_Handler);
    vTaskStartScheduler();          	//�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{

    taskENTER_CRITICAL();           //�����ٽ���

    /********����SX1280ͨ�����õ���Ϣ����,***********/
    SX1280_0_RxData_Q=xQueueCreate(SX1280_RData_Q_NUM,SX1280_RData_Q_LENTH);
    SX1280_1_RxData_Q=xQueueCreate(SX1280_RData_Q_NUM,SX1280_RData_Q_LENTH);

    /********��������1�����������õ���Ϣ����,***********/
    Uart1Tx_MessageData_Q=xQueueCreate(Uart1Tx_MessageData_Q_NUM,Uart1Tx_MessageData_Q_LENTH);
    /********��������2�����������õ���Ϣ����,***********/
    Uart1Rx_MessageData_Q=xQueueCreate(Uart1Rx_MessageData_Q_NUM,Uart1Rx_MessageData_Q_LENTH); //����1�������ݵ���Ϣ���о��

    /********��������2�����������õ���Ϣ����,***********/
    Uart2Tx_MessageData_Q=xQueueCreate(Uart2Tx_MessageData_Q_NUM,Uart2Tx_MessageData_Q_LENTH);
    /********��������2�����������õ���Ϣ����,***********/
    Uart2Rx_MessageData_Q=xQueueCreate(Uart2Rx_MessageData_Q_NUM,Uart2Rx_MessageData_Q_LENTH); //����2�������ݵ���Ϣ���о��

    /********��������4�����������õ���Ϣ����,***********/
    Uart4Tx_MessageData_Q=xQueueCreate(Uart4Tx_MessageData_Q_NUM,Uart4Tx_MessageData_Q_LENTH);
    /********��������4�����������õ���Ϣ����,***********/
    Uart4Rx_MessageData_Q=xQueueCreate(Uart4Rx_MessageData_Q_NUM,Uart4Rx_MessageData_Q_LENTH); //����4�������ݵ���Ϣ���о��

    /********������ע��ID����Ϣ����,***********/
    RegSensorID_MessageData_Q=xQueueCreate(RegSensorID_MessageData_Q_NUM,RegSensorID_MessageData_Q_LENTH); //����4�������ݵ���Ϣ���о��



    if((SX1280_0_RxData_Q==0)||(SX1280_1_RxData_Q==0)||(Uart1Tx_MessageData_Q==0)||(Uart1Rx_MessageData_Q==0)||(Uart2Tx_MessageData_Q==0)||(Uart2Rx_MessageData_Q==0)||(Uart4Rx_MessageData_Q==0)||(Uart4Tx_MessageData_Q==0))
    {
        printf("SX1280_QueueCreate err!!!\r\n");
    }

    /********����SX1280�¼���***********/
    SX1280RX_EventGroupHandler = xEventGroupCreate();
    if(SX1280RX_EventGroupHandler == NULL)
    {
        printf("creak SX1280RX_EventGroupHandler failure!\r\n");
    }
    /********�������Ź��¼���***********/
    IWDG_EventGroupHandler = xEventGroupCreate();
    if(IWDG_EventGroupHandler == NULL)
    {
        printf("creak IWDG_EventGroupHandler failure!\r\n");
    }

    /********���������¼���***********/
    Common_EventGroupHandler = xEventGroupCreate();
    if(Common_EventGroupHandler == NULL)
    {
        printf("creak Common_EventGroupHandler failure!\r\n");
    }

    //�������Ź�����
    xTaskCreate((TaskFunction_t )IWDG_Task,
                (const char*    )"IWDG_Task",
                (uint16_t       )IWDG_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )IWDG_TASK_PRIO,
                (TaskHandle_t*  )&IWDG_Task_Handler);

    //����SX1280���ݴ�������
    xTaskCreate((TaskFunction_t )SX1280RxData_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SX1280RxData_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SX1280RxData_TASK_PRIO,
                (TaskHandle_t*  )&SX1280RxData_Task_Handler);

    //�������в�����ŵ�����
    xTaskCreate((TaskFunction_t )SenCTR_CH_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SenCTR_CH_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenCTR_CH_TASK_PRIO,
                (TaskHandle_t*  )&SenCTR_CH_Task_Handler);

    //�������в�ҵ���ŵ�����
    xTaskCreate((TaskFunction_t )SenBS_CH1_Task,
                (const char*    )"sx1280rx_task",
                (uint16_t       )SenBS_CH1_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenBS_CH1_TASK_PRIO,
                (TaskHandle_t*  )&SenBS_CH1_Task_Handler);

    //��������1��������
    xTaskCreate((TaskFunction_t )USART1_Snd_Task,
                (const char*    )"USART1_Snd_Task",
                (uint16_t       )USART1_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART1_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART1_Snd_Task_Handler);

    //��������1��������
    xTaskCreate((TaskFunction_t )USART1_Rx_Task,
                (const char*    )"USART1_Snd_Task",
                (uint16_t       )USART1_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART1_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART1_Rx_Task_Handler);

    //��������2��������
    xTaskCreate((TaskFunction_t )USART2_Snd_Task,
                (const char*    )"USART2_Snd_Task",
                (uint16_t       )USART2_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART2_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART2_Snd_Task_Handler);

    //��������2��������
    xTaskCreate((TaskFunction_t )USART2_Rx_Task,
                (const char*    )"USART2_Snd_Task",
                (uint16_t       )USART2_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART2_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART2_Rx_Task_Handler);

    //��������4��������
    xTaskCreate((TaskFunction_t )USART4_Snd_Task,
                (const char*    )"USART4_Snd_Task",
                (uint16_t       )USART4_Snd_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART4_Snd_TASK_PRIO,
                (TaskHandle_t*  )&USART4_Snd_Task_Handler);

    //��������4��������
    xTaskCreate((TaskFunction_t )USART4_Rx_Task,
                (const char*    )"USART4_Snd_Task",
                (uint16_t       )USART4_RX_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )USART4_Rx_TASK_PRIO,
                (TaskHandle_t*  )&USART4_Rx_Task_Handler);

    //������ע�ᷢ������
    xTaskCreate((TaskFunction_t )SenRegSend_Task,
                (const char*    )"SenRegSend_TASK",
                (uint16_t       )SenRegSend_TASK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SenRegSend_TASK_PRIO,
                (TaskHandle_t*  )&SenRegSend_Task_Handler);

    //������ڴ�λ�ã���������񴴽�֮ǰ�����ڿ�������������֮ǰ�����жϲ�������ᷢ��PORT.c 784�Ĵ���
    printf("=============www.inrich.com.cn(AccessNode)===============");
    printf( "\r\n FMVersion:V%d.%d.%d.%d   HWVersion:V%d.%d.%d.%d",FWVersion[0],FWVersion[1],FWVersion[2],FWVersion[3],HWVersion[0],HWVersion[1],HWVersion[2],HWVersion[3]);
    SX1280_Param_Init(0,SenCTR_CH_Fre); 		//��ʼ��΢���ʲ�����ŵ�SX1280оƬ    �����ⲿ�ж�����
    SX1280_Param_Init(1,SenBS_CH1_Fre);			//��ʼ����΢���ʲ�ҵ��1�ŵ�SX1280оƬ   ����FMVersion�ⲿ�ж�����
//    SX1280_Param_Init(2,Frequency_list[SenBS_CH2_Fre]);			//��ʼ����΢���ʲ�ҵ��1�ŵ�SX1280оƬ   �����ⲿ�ж�����
    printf("\r\nControl  channel's FreNum is: %d",SenCTR_CH_Fre);
    printf("\r\nBusiness channel's FreNum is: %d",SenBS_CH1_Fre);
//    printf("\r\n��ǰ������ҵ��2Ƶ��=%u,Ƶ���=%d",Frequency_list[SenBS_CH2_Fre],SenBS_CH2_Fre);

    printf("\r\nEID��%02X %02X %02X %02X %02X %02X ",Config.D_DID[0],Config.D_DID[1],Config.D_DID[2],Config.D_DID[3],Config.D_DID[4],Config.D_DID[5]);
    printf("\r\nSenList length is: %d",SensorNode_List.Nodenum);
    GetNodeNum_AllType();
    printf("\r\nBLNodenum=%d, WHNodenum=%d, RegSuccessNodenum=%d, RegFailNodenum=%d. ",SensorNode_List.BLNodenum,SensorNode_List.WHNodenum,SensorNode_List.RegSuccessNodenum,SensorNode_List.RegFailNodenum);
    if(UART1orUART2==0)
        printf("\r\nNow is using Com2!");
    else
        printf("\r\nNow is using Com1!");
//    printf("\r\n��۽ڵ�ID�к����ֽڣ����ֽ���ǰ,16���ƣ���%x %x %x %x ",D_ID[2],D_ID[3],D_ID[4],D_ID[5]);
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
*Description  	:���Ź��������ڿ��Ź������ι��ʱ��Ϊ(256*2047)/32=16s����
				1�����Ź����������ǣ���1��ÿ������ص�����Ӧ���ڹ涨ʱ������������һ�β����Ͷ�Ӧ��־��
									 ��2���������ڵȴ���־��������ι����ͬʱ����ͨ����Щ��־λδ�����Σ����ж���Щ�����п��ܹҵ���
				2�����ڱ�Ӧ������������ȴ�ʱ�䶼�����ֵ������ֻ�ܼ��SX1280RxData_Task

*Input		  	:
*Output		  	:
*Question	  	:1��Ŀǰ���Ź����ԣ�
				(1)ԭ��������5s��ͬʱ��3�¼���SX1280���յ�ҵ�����ݣ����ڷ��ͣ����ڽ��գ�������ι�������������ЩС�������Ź���N(WATCHDOG_MaxTime��������)�����ڲ�ι����������
				(2)����SX1280���յ�ҵ�����ݺ���������д��ڷ��ͣ����ڽ���������1s���ڵģ���5s��ͬʱ������N(WATCHDOG_MaxTime��������)�����ڻ��ǿ��еģ���ΪN(WATCHDOG_MaxTime��������)����û���յ�SX1280ҵ������Ҳ���������ˣ�
				(3)V2.0.3.30�и�ΪN(WATCHDOG_MaxTime��������)���ӣ�3�¼�ֻҪ��������һ�μ��ɣ��ڶ��¼���־ʱ����־�����㣬ֻ����3���¶����������㣩
*******************************************************/
void IWDG_Task(void *pvParameters)
{
    EventBits_t wdguxBits;
    EventBits_t resetuxBits;
//    BaseType_t err;
    static uint16_t WDCount=0;
    while(1)
    {
        /**********�����ſ��¼�*********************/
//        wdguxBits = xEventGroupWaitBits(IWDG_EventGroupHandler, 		// �¼���־����
//                                        EVENTBIT_IWDG_ALL,     			// �ȴ�����λ
//                                        pdTRUE,             			// �ȴ���λ����󣬻ᱻ���
//                                        pdFALSE, 						// ����ΪpdTRUE��ʾ���¼���ȫ������Żᴥ����pdFALSE��һ���¼�����ͻᴥ��*/
//                                        ( TickType_t )0);				// ����  �ȴ���ʱ portMAX_DELAY(�����ʱ)  // ( TickType_t )10 (��ʱ10������)
        wdguxBits=xEventGroupGetBits(IWDG_EventGroupHandler);		// �¼���־����
		if (UART1orUART2)	//���ô���1
        {
            if((wdguxBits & EVENTBIT_IWDG_UART1)==EVENTBIT_IWDG_UART1)
            {
                xEventGroupClearBits(IWDG_EventGroupHandler,EVENTBIT_IWDG_UART1);
				IWDG_Feed();
                WDCount=0;				
            }
            else
            {
                if(WDCount<WATCHDOG_MaxTime)	 //200�ӽ�20����	//20��Լ2���ӣ�������ι��������ʱ����20*5s(����ʱ��)+16=116S;
                {
                    IWDG_Feed();
                    WDCount++;
                }
                else	printf("\r\nIWDG event,uxBits:0x%X  USART1",wdguxBits);
            }
        }
        else//���ô���2
        {
            if((wdguxBits & EVENTBIT_IWDG_UART2)==EVENTBIT_IWDG_UART2)
            {
                 xEventGroupClearBits(IWDG_EventGroupHandler,EVENTBIT_IWDG_UART2);
				IWDG_Feed();
                WDCount=0;
            }
            else
            {
                if(WDCount<WATCHDOG_MaxTime)	//200�ӽ�20����		//20��Լ2���ӣ�������ι��������ʱ����20*5s(��ʱ����ʱ��)+16=116S;
                {
                    IWDG_Feed();
                    WDCount++;
                }
                else	printf("\r\nIWDG event,uxBits:%d  USART2",wdguxBits);
            }
        }
        /**********��������*********************/
        if(SelfNode_TimeStamp>(upHeartBeat_TimeStamp)+15*1000) //����15s
        {
            upHeartBeat_TimeStamp=SelfNode_TimeStamp;
            Load_HeartBeat();
        }
//        if(SelfNode_TimeStamp>(downHeartBeat_TimeStamp)+DnBeatHeartTime_MAXNUM) //����30s
//        {
//            downHeartBeat_TimeStamp=SelfNode_TimeStamp;
//            Config.RelayClient_status = RelayClient_OFF;
//            printf("\r\nIWDG_Task��Receive heartbeat timeout.");
//        }

        /**********ͨ������*********************/
        resetuxBits = xEventGroupWaitBits(Common_EventGroupHandler, 				// �¼���־����
                                          EVENTBIT_SoftReset_Flag,     				// �ȴ�����λ
                                          pdTRUE,             					// �ȴ���λ����󣬻ᱻ���
                                          pdFALSE, 								// ����ΪpdTRUE��ʾ���¼���ȫ������Żᴥ����pdFALSE��һ���¼�����ͻᴥ��*/
                                          ( TickType_t )0);				// ����  �ȴ���ʱ portMAX_DELAY(�����ʱ)  // ( TickType_t )10 (��ʱ10������)
        if((resetuxBits & EVENTBIT_SoftReset_Flag) == EVENTBIT_SoftReset_Flag)
        {
            printf("\r\n system will reset after 3second!");
            vTaskDelay(3*1000);  //��ʱ5s��Ҳ����5000��ʱ�ӽ���
            SoftReset();	// system reset
        }
        vTaskDelay(5*1000);  //��ʱ5s����������ı䣬�漰������ι����ʱʱ�䣩��Ҳ����5000��ʱ�ӽ��� �����ܵ���16s,��Ϊ��Ҫι��
        LED0=!LED0;
    }
}
/*******************************************************
*Function Name	:SX1280RxData_Task
*Description  	:SX1280�����������񣺼��ж��¼������󣬴�SX1280�ж�ȡ���ݣ������Ӧ�����У�
*Input		  	:ע������ʱ�䲻�ܳ������Ź����ʱ�䣨�����16S��
*Output		  	:
*Question	  	:1��
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
        uxBits = xEventGroupWaitBits(SX1280RX_EventGroupHandler, 			// �¼���־����
                                     EVENTBIT_SX1280_ALL,     				// �ȴ�����λ
                                     pdTRUE,             					// �ȴ���λ����󣬻ᱻ���
                                     pdFALSE, 								// ����ΪpdTRUE��ʾ���¼���ȫ������Żᴥ����pdFALSE��һ���¼�����ͻᴥ��*/
                                     ( TickType_t )60*1000);				// ����  �ȴ���ʱ portMAX_DELAY(�����ʱ)  // ( TickType_t )10 (��ʱ10������)
//                                     portMAX_DELAY);				// ����  �ȴ���ʱ portMAX_DELAY(�����ʱ)  // ( TickType_t )10 (��ʱ10������)
        //΢���ʲ�����ŵ����жϣ������ճ�ʱ��ȡ����
        taskENTER_CRITICAL();
        if((uxBits & EVENTBIT_SX1280_SENS0_ISR) == EVENTBIT_SX1280_SENS0_ISR)
        {
            SX1280ProcessIrqs(0);						//SX280�жϴ���
        }
        sx1280_irpro_result=SX1280_SENS_Pro(0);		//SX1280���յ����ݵ��жϴ���
        taskEXIT_CRITICAL();
        switch(sx1280_irpro_result)
        {
        case 0: //����
            break;
        case 1: //��ȷ���յ�����
            err=xQueueSend(SX1280_0_RxData_Q,SX_Sens_RecBuf[0],2);//����2ms
            if(err==errQUEUE_FULL)
            {
                printf("\r\n SX1280_0_RxData_Q is full and send fail!");
            }
            //��ӡ��������
//				printf("\r\n ΢���ʲ�����ŵ��յ��������£�");
//                for(taski=0; taski<SX_Sens_RecBufSize[3]; taski++)
//                    printf(" %d",tempbuf[taski]);
//            my_memclear( SX_Sens_RecBuf[0], SX_SENS_RECBUF_SIZE);  //�建����
            memset(SX_Sens_RecBuf[0],0,SX_SENS_RECBUF_SIZE);  //�建����
            break;
        case 2: //���ͳɹ�
            break;
        case 3: //���ճ�ʱ
            break;
        case 4: //���ճ���
            break;
        case 5: //���ͳ�ʱ
            break;
        }

        //΢���ʲ�ҵ��1�ŵ����жϣ������ճ�ʱ��ȡ����
        taskENTER_CRITICAL();
        if((uxBits & EVENTBIT_SX1280_SENS1_ISR) == EVENTBIT_SX1280_SENS1_ISR)
        {
            SX1280ProcessIrqs(1);						//SX280�жϴ���
            LED1=!LED1;
			IO3_LED=!IO3_LED;
        }
        sx1280_irpro_result=SX1280_SENS_Pro(1);		//SX1280���յ����ݵ��жϴ���
        taskEXIT_CRITICAL();
        switch(sx1280_irpro_result)
        {
        case 0: //����
            break;
        case 1: //��ȷ���յ�����
            err=xQueueSend(SX1280_1_RxData_Q,SX_Sens_RecBuf[1],2);//����2ms
            if(err==errQUEUE_FULL)
            {
                printf("\r\n SX1280_4_RxData_Q is full and send fail!");
            }
            //��ӡ��������
//				printf("\r\n ΢���ʲ�ҵ���ŵ��յ��������£�");
//                for(taski=0; taski<SX_Sens_RecBufSize[4]; taski++)
//                    printf(" %d",tempbuf[taski]);
//            my_memclear( SX_Sens_RecBuf[1], SX_SENS_RECBUF_SIZE);  //�建����
            memset(SX_Sens_RecBuf[1],0,SX_SENS_RECBUF_SIZE);  //�建���� SX_SENS_RECBUF_SIZE
            break;
        case 2: //���ͳɹ�
            break;
        case 3: //���ճ�ʱ
            break;
        case 4: //���ճ���
            break;
        case 5: //���ͳ�ʱ
            break;
        }
        /******��������ʣ���ջ�ռ�********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SX1280RxData_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSX1280RxData_Taskʣ���ջ% 3d",uxHighWaterMark);//
        vTaskDelay(3);  //��ʱ3ms��Ҳ����5��ʱ�ӽ���
    }
}

/*******************************************************
*Function Name	:SenCTR_CH_Task
*Description  	:΢���ʲ�����ŵ��������ݴ��������ࣩ
*Input		  	:
*Output		  	:
*Question	  	:1��
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
        //���ն������ݣ�������ʽ
        err=xQueueReceive(SX1280_0_RxData_Q,SX1280_CTR_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            taski=SenCTR_CH_DataPro(SX1280_CTR_tempbuf);
            switch(taski)
            {
            case 0x11:	//��ʾ�յ�REQ֡��ͨ��ָ���������
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //ȡ�ô�����ID
                SenListNum=CheckID_isWH(CurSenID); //����������
                if(SenListNum!=0xFFFF) //��ʾ�ڰ�������
                {
                    SenCTR_ComPara_REQRSP(SenListNum,0);		//�ظ�RSP_END֡
//                    printf("\r\n ������ͨ�Ų�������ID�ǣ�%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                break;
            case 0x12:	//��ʾ�յ�REQ֡��ҵ����Ʋ�������
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //ȡ�ô�����ID
                SenListNum=CheckID_isWH(CurSenID); //����������
                if(SenListNum!=0xFFFF) //��ʾ�ڰ�������
                {
                    SenCTR_CtrPara_REQRSP(SenListNum,0);		//�ظ�RSP_END֡,���ÿ��Ʋ���
//                    printf("\r\n �����˿��Ʋ�������ID�ǣ�%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                break;
            case 0x10:	//��ʾ�յ�REQ֡�����п��Ʋ�������
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6); //ȡ�ô�����ID
                SenListNum=CheckID_isWH(CurSenID); //����������
                if(SenListNum!=0xFFFF) //��ʾ�ڰ�������
                {
                    SenCTR_ComPara_REQRSP(SenListNum,1);			//�ظ�RSP֡,
                    delay_xms(20);	//��ʱ5ms��
                    SenCTR_CtrPara_REQRSP(SenListNum,0);		//�ظ�RSP_END֡,���ÿ��Ʋ���
//                    printf("\r\n ��������ͨ�Ų������������˿��Ʋ�������ID�ǣ�%02X %02X %02X %02X %02X %02X", CurSenID[0], CurSenID[1], CurSenID[2], CurSenID[3], CurSenID[4], CurSenID[5]);
                }
                else //������ں�������,Ҳ���ڰ������У�����Ҫ�ϴ���REQ��Ϣ
                {
//                    //�����ã�������ڰ������У�����ӵ���������
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
//                        senpt_test.SenBS_Cycle=5*60*1000;			//ҵ�����ڣ�5��������
//                        senpt_test.SenREQ_Cycle=0;//5;					//��������
//                        senpt_test.SenSlotorder=0;//slotorder_test++;	//ʱ϶
//                        senpt_test.SenShockTime=0;//;					//������
//                        senpt_test.SenLimitType[0]=0;//(0x08<<2); 		//��һ���������ͣ��¶�������;
////                        WhiteList_ADDID(&senpt_test,1);
//                    }
                }
                break;
            case 0x02:	//��ʾ�յ��Ǻ������ĸ澯���ݣ�
                memcpy(CurSenID,&SX1280_CTR_tempbuf[2],6);  //ȡ�ô�����ID
                ///////////////����ڰ������У�����Ҫ���͸澯��Ӧ////////////////
                SenListNum=CheckID_isWH(CurSenID); //����������
                if(SenListNum!=0xFFFF) //��ʾ�ڰ�������
                {
                    BURST_ACK_Send(CurSenID);
                }
                Send_uSensorBSData(SX1280_CTR_tempbuf);	//���͸澯����
                break;
            case 0x03:	//��ʾ�յ�REQ_ACK
                break;
            }
        }
        /******��������ʣ���ջ�ռ�********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SenCTR_CH_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSenCTR_CH_Task   ʣ���ջ% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}

/*******************************************************
*Function Name	:SenBS_CH1_Task
*Description  	:΢���ʲ�ҵ���ŵ�1�������ݴ��������ࣩ
*Input		  	:
*Output		  	:
*Question	  	:1��
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
        //���ն������ݣ�������ʽ
        err=xQueueReceive(SX1280_1_RxData_Q,SX1280_BS1_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            taski=SenBS_CH1_DataPro(SX1280_BS1_tempbuf);
            switch(taski)
            {
            case 0x00:	//���ݸ�ʽ�����⣬������
                break;
            case 0x01: 	//���ں�������������
                break;
            case 0x05:	// //���������У�����Ϊ���Ǻڣ���ע�᲻ͨ������鿴�Ƿ�ʱ�������ʱ�����������ע�ᣬ���򲻴���
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                SenID_SN=FixPosition_InSenList(CurSenID);	//�ж��Ƿ����б���
                if(SenID_SN!=0xffff)
                {
                    if(SelfNode_TimeStamp>(SensorNode_List.Register_FailTime [SenID_SN]+Register_FAILTIME_MAX)) //�����ID��ע��ʧ��ʱ�䳬��                    
                    {
                        //Send_uSensorRegData(CurSenID); //����΢���ʴ�������ע������֡
                        if(Config.RelayClient_status == RelayClient_ON)
                        {
                            err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//����0ms��ֻ�ǰ�ID���͵�����
                            if(err==errQUEUE_FULL)
                                printf("\r\n SenBS_CH1_Task:RegSensorID_MessageData_Q is full and send fail!");
                        }
						SensorNode_List.Register_FailTime [SenID_SN]=SelfNode_TimeStamp;
                    }
                }
                break;
            case 0x02:	//�����������У�����ע��
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                /***********�ٴ�΢���ʴ�������ע������֡��ע�ⲻ���ȴ�ע�ᣬ��Ϊ��������ʱ��Ҫ��BSbuf�е�ԭ���ݣ�**************/
                if(SensorNode_List.Nodenum<SensorLIST_MAXNUM) //����δ�����ܷ�ע��
                {
                    //Send_uSensorRegData(CurSenID); //����΢���ʴ�������ע������֡
                    if(Config.RelayClient_status == RelayClient_ON)
                    {
                        err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//����0ms��ֻ�ǰ�ID���͵�����
                        if(err==errQUEUE_FULL)
                            printf("\r\n SenBS_CH1_Task:1RegSensorID_MessageData_Q is full and send fail!");
                    }
                }
                break;
            case 0x03:	//���������У�����Ϊ���Ǻڣ���δע�ᣬ����ע��
                //ȡ�ô�����ID
                memcpy(CurSenID,&SX1280_BS1_tempbuf[2],6);
                /***********�ٴ�΢���ʴ�������ע������֡��ע�ⲻ���ȴ�ע�ᣬ��Ϊ��������ʱ��Ҫ��BSbuf�е�ԭ���ݣ�**************/
                //Send_uSensorRegData(CurSenID);
                if(Config.RelayClient_status == RelayClient_ON)
                {
                    err=xQueueSend(RegSensorID_MessageData_Q,CurSenID,0);//ֻ�ǰ�ID���͵�����
                    if(err==errQUEUE_FULL)
                        printf("\r\n SenBS_CH1_Task:2RegSensorID_MessageData_Q is full and send fail!");
                }
                break;
            case 0x04:	// ���������У�����Ϊ���Ǻڣ������Ѿ�ע��ɹ�������������ϴ�
                Send_uSensorBSData(SX1280_BS1_tempbuf); //����ҵ������
                //���Ϳ��Ź���ر�־
                xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_SenBS_CH1_TASK);
                break;
            }
        }
        /******��������ʣ���ջ�ռ�********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( SenBS_CH1_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nSenBS_CH1_Task   ʣ���ջ% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}


/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:���ڷ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART1_Snd_Task(void *pvParameters)
{
    BaseType_t err;
    uint16_t taski;
//    static uint16_t ParaSetNum=0;
    while(1)
    {
        if(uxQueueMessagesWaiting(Uart1Tx_MessageData_Q)!=0)	//��ѯ�������������������֤����������Ϣ��
        {
//            if(North_UL_isEnable()==true) //�жϵ�ǰ�����ܷ�ʹ�ã�����ʹ���������������ڣ�������true
            {
                err=xQueueReceive(Uart1Tx_MessageData_Q,USART1_TX_BUF,0);  //portMAX_DELAY
                if(err==pdTRUE) //���������������
                {
                    USART1_TX_Lenth=(USART1_TX_BUF[0]<<8)+USART1_TX_BUF[1];	//ǰ���ֽ�Ϊ���ȣ�����������
                    if(USART1_TX_Lenth>0)
                    {
                        //��Ҫ���͵����ݿ��������ڷ��ͻ���
                        for(taski=0; taski<USART1_TX_Lenth; taski++)
                            USART1_TX_BUF[taski]=USART1_TX_BUF[taski+2];
                        //��֤�÷�����ɵı�־λ����
                        if(DMA_GetFlagStatus(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF)!=RESET)   ////�ȴ��������
                            DMA_ClearFlag(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
                        //��ʼ���ͣ�DMA����:�ڿ���ǰ������ɷ�����ɱ�־TCIF��0,����DMA���ᷢ�͡�
                        USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  					//ʹ�ܴ���1��DMA����
                        DMA_Cmd(UART1_TX_DMA_Stream,DISABLE);							//�ر�DMA����
                        while(DMA_GetCmdStatus(UART1_TX_DMA_Stream)!=DISABLE) {}  		//ȷ��DMA������
                        DMA_SetCurrDataCounter(UART1_TX_DMA_Stream,USART1_TX_Lenth);	//�������ݴ�����
                        DMA_Cmd(UART1_TX_DMA_Stream,ENABLE);							//����DMA����
                        while(DMA_GetFlagStatus(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF)==RESET) {}  ////�ȴ��������
                        DMA_ClearFlag(UART1_TX_DMA_Stream,UART1_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
//                        printf("\r\nCOM1�����Ѿ�����%d:",USART1_TX_Lenth);
//                        for(taski=0; taski<(5); taski++)
//                            printf(" %02X",USART1_TX_BUF[taski]);
                        delay_xms(1);
                        //���Ϳ��Ź���ر�־
                        xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART1_Tx_Task);
                    }
                }
//                North_UL_unlock(); //�ͷŴ���
            }
        }
        /******��������ʣ���ջ�ռ�********/
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark=uxTaskGetStackHighWaterMark( USART1_Snd_Task_Handler );
        if(uxHighWaterMark<60)	printf("\r\nUSART1_Snd_Task  ʣ���ջ% 3d",uxHighWaterMark);//
        vTaskDelay(5);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}

/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:���ڽ������ݴ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART1_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART1_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //���ն������ݣ�������ʽ
        err=xQueueReceive(Uart1Rx_MessageData_Q,USART1_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            if (UART1orUART2)	//���ô���1
            {
                if( NetProCom_Rx_Pro(USART1_rx_tempbuf,COM1)==true )
                {
                    //���Ϳ��Ź���ر�־
                    xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART1_Rx_Task);
//                    LED0=!LED0;
                }
            }
        }
        vTaskDelay(5);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}

/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:���ڷ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART2_Snd_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART2_TX_LEN]= {0};
    uint16_t taski;
    while(1)
    {
        if(uxQueueMessagesWaiting(Uart2Tx_MessageData_Q)!=0)	//��ѯ�������������������֤����������Ϣ��
        {
//            if(North_UL_isEnable()==true) //�жϵ�ǰ�����ܷ�ʹ�ã�����ʹ���������������ڣ�������true
            {
                //���ն������ݣ��Ƕ�����ʽ
                err=xQueueReceive(Uart2Tx_MessageData_Q,USART2_TX_BUF,0);  //portMAX_DELAY  ( TickType_t )5*1000)
                if(err==pdTRUE) //��������������ݣ���ʵ��ʱ��϶�������Ϣ�ģ�
                {
                    USART2_TX_Lenth=(USART2_TX_BUF[0]<<8)+USART2_TX_BUF[1];	//ǰ���ֽ�Ϊ���ȣ�����������
                    if(USART2_TX_Lenth>0)
                    {

                        //��Ҫ���͵����ݿ��������ڷ��ͻ���
                        for(taski=0; taski<USART2_TX_Lenth; taski++)
                            USART2_TX_BUF[taski]=USART2_TX_BUF[taski+2];
                        //��֤�÷�����ɵı�־λ����
                        if(DMA_GetFlagStatus(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF)!=RESET)   ////�ȴ��������
                            DMA_ClearFlag(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
                        //��ʼ���ͣ�DMA����:�ڿ���ǰ������ɷ�����ɱ�־TCIF��0,����DMA���ᷢ�͡�
                        USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  					//ʹ�ܴ���2��DMA����
                        DMA_Cmd(UART2_TX_DMA_Stream,DISABLE);							//�ر�DMA����
                        while(DMA_GetCmdStatus(UART2_TX_DMA_Stream)!=DISABLE) {}  		//ȷ��DMA������
                        DMA_SetCurrDataCounter(UART2_TX_DMA_Stream,USART2_TX_Lenth);	//�������ݴ�����
                        DMA_Cmd(UART2_TX_DMA_Stream,ENABLE);							//����DMA����
                        while(DMA_GetFlagStatus(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF)==RESET) {}  ////�ȴ��������
                        DMA_ClearFlag(UART2_TX_DMA_Stream,UART2_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
//                        printf("\r\nCOM2�����Ѿ�����%d:",USART1_TX_Lenth); //��Ż���2ms
////                    	for(taski=0; taski<(USART2_TX_Lenth); taski++)
//                        for(taski=0; taski<(5); taski++)
//                            printf(" %02X",USART2_TX_BUF[taski]);
                        delay_xms(1);
                        //���Ϳ��Ź���ر�־
                        xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART2_Tx_Task);
                    }
                }
//                North_UL_unlock(); //�ͷŴ���
            }
        }
        vTaskDelay(10);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}


/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:���ڽ������ݴ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART2_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART2_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //���ն������ݣ�������ʽ
        err=xQueueReceive(Uart2Rx_MessageData_Q,USART2_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            if (UART1orUART2==0)	//���ô���2
            {
                if( NetProCom_Rx_Pro(USART2_rx_tempbuf,COM2)==true )
                {
                    //���Ϳ��Ź���ر�־
                    xEventGroupSetBits(IWDG_EventGroupHandler, EVENTBIT_USART2_Rx_Task);
//                    LED0=!LED0;
                }
            }
        }
        vTaskDelay(30);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}



/*******************************************************
*Function Name	:USART_Snd_Task
*Description  	:���ڷ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART4_Snd_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART4_TX_LEN]= {0};
    uint16_t taski;
    while(1)
    {
        //���ն������ݣ�������ʽ
        err=xQueueReceive(Uart4Tx_MessageData_Q,USART4_TX_BUF,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            USART4_TX_Lenth=(USART4_TX_BUF[0]<<8)+USART4_TX_BUF[1];	//ǰ���ֽ�Ϊ���ȣ�����������
            if(USART4_TX_Lenth>0)
            {
                //��Ҫ���͵����ݿ��������ڷ��ͻ���
                for(taski=0; taski<USART4_TX_Lenth; taski++)
                    USART4_TX_BUF[taski]=USART4_TX_BUF[taski+2];
                //��֤�÷�����ɵı�־λ����
                if(DMA_GetFlagStatus(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF)!=RESET)   ////�ȴ��������
                    DMA_ClearFlag(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
                //��ʼ���ͣ�DMA����:�ڿ���ǰ������ɷ�����ɱ�־TCIF��0,����DMA���ᷢ�͡�
                USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  					//ʹ�ܴ���4��DMA����
                DMA_Cmd(UART4_TX_DMA_Stream,DISABLE);							//�ر�DMA����
                while(DMA_GetCmdStatus(UART4_TX_DMA_Stream)!=DISABLE) {}  		//ȷ��DMA������
                DMA_SetCurrDataCounter(UART4_TX_DMA_Stream,USART4_TX_Lenth);	//�������ݴ�����
                DMA_Cmd(UART4_TX_DMA_Stream,ENABLE);							//����DMA����
//            	while(DMA_GetFlagStatus(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF)==RESET) {}  ////�ȴ��������
//				DMA_ClearFlag(UART4_TX_DMA_Stream,UART4_TX_DMA_FLAG_TCIF);	//���������ɱ�־,��������������´�DMA���ᷢ��
            }
//
        }
        vTaskDelay(10);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}


/*******************************************************
*Function Name	:USART_Rx_Task
*Description  	:���ڽ������ݴ������񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
*******************************************************/
void USART4_Rx_Task(void *pvParameters)
{
    BaseType_t err;
//    uint8_t BSbuf[USART4_REC_LEN]= {0};
//    uint8_t taski;
    while(1)
    {
        //���ն������ݣ�������ʽ
        err=xQueueReceive(Uart4Rx_MessageData_Q,USART4_rx_tempbuf,portMAX_DELAY);  //portMAX_DELAY
        if(err==pdTRUE) //���������������
        {
            USART_RxData_Pro(USART4_rx_tempbuf,COM4);
        }
        vTaskDelay(50);  //��ʱ5ms��Ҳ����5��ʱ�ӽ���
    }
}


/*******************************************************
*Function Name	:SenRegSend_Task
*Description  	:������IDע�ᷢ�����񣨣�
*Input		  	:
*Output		  	:
*Question	  	:1��
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
            MessageNum=uxQueueMessagesWaiting(RegSensorID_MessageData_Q); //��ѯ�������������
            if((MessageNum>0)&&(MessageNum<(RegSensorID_MessageData_Q_NUM+1)))
            {
                memcpy(&RegSensorID_tempbuf[2],CMD_Head,5);//5�ֽ�ͷ��2�ֽڳ��ȣ������ʼ����1�ֽ�����
				RegSensorID_tempbuf[7]=0x00; 			//2�ֽڳ��ȣ������ʼ�����ֽڣ�
				RegSensorID_tempbuf[8]=0x00; 			//2�ֽڳ��ȣ������ʼ�����ֽڣ�
				RegSensorID_tempbuf[9]=0x58; 			//1�ֽ���·���				
				RegSensorID_tempbuf[10]=0x70; 			//����֡����0111 0000���˶ˣ����У������ָ���ĩ�˻��EID����ĩ�˴�����EID���޶˿ںţ�����Ϊ0
                memcpy(&RegSensorID_tempbuf[11],Config.D_DID,6);//����ĩ�˻��EID
                RegSensorID_tempbuf[17]=0x02; 					//�����ָ�����ͣ������豸ע������
                RegSensorID_tempbuf[18]=0x03; 				//ע������ָʾ��0000 0011��΢���ʴ�������ͨ�����(΢����ͨ��)
                RegSensorID_tempbuf[19]=0; 					//���豸������Ӧ�ú������޸�
                buffptr=20;
                SenRegIDNum=0;
                while(MessageNum>0)
                {
                    MessageNum--;
                    err=xQueueReceive(RegSensorID_MessageData_Q,SenRegIDbuf,0);  //portMAX_DELAY
                    if(err==pdTRUE) //���������������
                    {
                        memcpy(&RegSensorID_tempbuf[buffptr],SenRegIDbuf,6);	//����΢���ʴ�����EID
                        buffptr=buffptr+6;
                        SenRegIDNum++;		//ID������
                    }
                    if(buffptr>(USART1_REC_LEN-15))		//��ֹ�ڴ����
                    {
                        printf("\r\n SenRegSend_Task:RegSensorID_tempbuf is full!");
                        break;
                    }
                }
                RegSensorID_tempbuf[0]=(buffptr-2+2)>>8;		//�������ݳ����޸�(�����������ʼ�2)(��2�ǻ�������CRC�ֽ�)
                RegSensorID_tempbuf[1]=buffptr-2+2;
				RegSensorID_tempbuf[7]=(buffptr-9+2)>>8;		//�����2�ֽڳ��ȣ������ʼ�����ֽڣ���(2+7)��(��2�ǻ�������CRC�ֽ�)
				RegSensorID_tempbuf[8]=(buffptr-9+2); 			//				
                RegSensorID_tempbuf[19]=SenRegIDNum;  //���豸�����޸�
				uint16_t crcdata=RTU_CRC(&(RegSensorID_tempbuf[2]),buffptr-2);		//CRC
				RegSensorID_tempbuf[buffptr++]=crcdata>>8;
				RegSensorID_tempbuf[buffptr++]=crcdata;				
                if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,RegSensorID_tempbuf,0);//����0ms
                else 				err=xQueueSend(Uart2Tx_MessageData_Q,RegSensorID_tempbuf,0);//����0ms
                if(err==errQUEUE_FULL)
                {
                    printf("\r\n SenRegSend_Task:UartTx_MessageData_Q is full and send fail!");
                }
            }
        }
        vTaskDelay(1*1000);  //��ʱ1s
    }
}


///*******************************************************
//*Function Name	:Send_uSensorRegData
//*Description  	:�򴮿ڷ���΢���ʴ�����ע����������
//*Input			:Redata_pt��6�ֽ�ID
//*Output			:
//*Question   	:1��
//*******************************************************/
//void  Send_uSensorRegData(uint8_t *Redata_pt)
//{
//    BaseType_t err;
//    uint8_t maclength;
//    if(Config.RelayClient_status == RelayClient_ON)
//    {
//        /***********΢���ʴ�������ע������֡�����ע����������Ҫͬʱ���У����ȴ����ݣ���Ϊ��������ʱ��Ҫ��BSbuf�е�ԭ���ݣ�**************/
//        maclength=16;
//        SX1280_BS1_tempbuf[0]=maclength>>8;
//        SX1280_BS1_tempbuf[1]=maclength;
//        SX1280_BS1_tempbuf[2]=0x70; 			//����֡����0111 0000���˶ˣ����У������ָ���ĩ�˻��EID����ĩ�˴�����EID���޶˿ںţ�����Ϊ0
//        memcpy(&SX1280_BS1_tempbuf[3],Config.D_DID,6);//����ĩ�˻��EID
//        SX1280_BS1_tempbuf[9]=0x02; 					//�����ָ�����ͣ������豸ע������
//        SX1280_BS1_tempbuf[10]=0x03; 				//ע������ָʾ��0000 0011��΢���ʴ�������ͨ�����(΢����ͨ��)
//        SX1280_BS1_tempbuf[11]=1; 					//���豸������Ӧ����1
//        memcpy(&SX1280_BS1_tempbuf[12],Redata_pt,6);	//����΢���ʴ�����EID
//        if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,SX1280_BS1_tempbuf,0);//����0ms
//        else 				err=xQueueSend(Uart2Tx_MessageData_Q,SX1280_BS1_tempbuf,0);//����0ms
//        if(err==errQUEUE_FULL)
//        {
//            printf("\r\n SenBS_CH1_Task2:UartTx_MessageData_Q is full and send fail!");
//        }
//    }
//}

/*******************************************************
*Function Name	:Send_uSensorData
*Description  	:�򴮿ڷ���΢���ʴ�����ҵ���澯����
*Input			:Redata_pt����sx1280���յ�ҵ�����ݻ�������
*Output			:
*Question   	:1��
*******************************************************/
void  Send_uSensorBSData(uint8_t *Redata_pt)
{
    BaseType_t err;
    uint8_t bufflength,maclength;
    uint8_t CurSenID[6];
    if(Config.RelayClient_status == RelayClient_ON)
    {        
        {
            ///�¼ܹ��Ĵ���:��ͷ���ֽ�Ϊ���ȣ�����Ϊ��Ҫ���͵����ݣ��Ѿ��������֡��
            maclength=Redata_pt[1];		//ҵ��֡��MAC����
            bufflength=Redata_pt[1]+9;	//ȡ�ø�֡�ܳ���:9=8(֡ͷ)+1��У�飩
            if(((bufflength+9)<SX1280_RData_Q_LENTH)&&(maclength>2)) //��֤���Ȳ��������ֵ��1������֡���ͣ�+6��ID��+2�����ȣ�
            {
                memcpy(CurSenID,&Redata_pt[2],6);  //ȡ�ô�����ID 
				//��֡��ǰ17�ֽ�:Ϊ�������ݳ���(2byte)����·֡ͷ(5byte),��·����(2byte),����(1byte),��������(1byte)��EID(6byte)��
				//�����ټ�ԭ֡�е����ֽڿ�ʼ������(��ǰ2�ֽڲ���Ҫ)������Ҫ��ԭ֡���ݺ��ƶ�15���ֽڣ�
                for(uint16_t taski=bufflength; taski>0; taski--)
                    Redata_pt[taski+14]=Redata_pt[taski-1];	
				
                maclength=maclength+13+10;			//���������ܳ��ȣ��������������ֽڣ���1������֡���ͣ�+6��DID��+6��ID��+��·֡ͷ(5byte)+��·����(2byte)+����(1byte)+crc(2byte)
                Redata_pt[0]=maclength>>8;
                Redata_pt[1]=maclength;
				memcpy(&Redata_pt[2],CMD_Head,5); 	//5�ֽ�ͷ��
				Redata_pt[7]=(maclength-7)>>8;				//2�ֽڳ��ȣ������ʼ�����ֽڣ�
				Redata_pt[8]=(maclength-7);		
				Redata_pt[9]=0x58; 					//1�ֽ���·���	
                Redata_pt[10]=0x44; 				//����֡����0100 0100���˶ˣ����У�����Ϊ���ݣ��޻�۽ڵ㣻��΢���ʴ���ͨ�ŵ�ַ���޶˿ںţ�����Ϊ0
                memcpy(&Redata_pt[11],CurSenID,6);	//����ID��ͬʱ������ԭMESSAGE֡��ǰ�����ֽ�
				uint16_t crcdata=RTU_CRC(&(Redata_pt[2]),maclength-2);		//CRC��(1)Redata_pt[2+7]���ӵ�10���ֽڿ�ʼ��(2)maclength-7-2��������Ҫ����crc���������ֽ�
				Redata_pt[maclength]=crcdata>>8;
				Redata_pt[maclength+1]=crcdata;				
                if (UART1orUART2)	err=xQueueSend(Uart1Tx_MessageData_Q,Redata_pt,0);//����0ms
                else 				err=xQueueSend(Uart2Tx_MessageData_Q,Redata_pt,0);//����0ms
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
*Description  	:�жϵ�ǰ����2�ܷ�ʹ�ã�����ʹ���������������ڣ�������true
*Input			:
*Output			:
*Question   	:1��
*******************************************************/
bool North_UL_isEnable(void)
{
    uint16_t randtime;
    //�������� ����������
    if(North_UL_islock()==true)
        return false;
    North_UL_lock(); //��������
    randtime =1 + (rand()%1000); //�����
    delay_us(randtime);
    North_UL_unlock(); //�ͷŴ���
    taskENTER_CRITICAL();
    if(North_UL_islock()==true)
	{
        taskEXIT_CRITICAL();
		return false;
	}
    North_UL_lock(); //��������
    taskEXIT_CRITICAL();
    return true;
}


/*******************************************************
*Function Name	:void SoftReset(void)
*Description  	:�������
*Input			:
*Output			:
*Question   	:1��
*******************************************************/
void SoftReset(void)
{
//    __disable_fault_irq();
    __set_FAULTMASK(1); // �ر������ж�
    NVIC_SystemReset(); // ��λ
}

