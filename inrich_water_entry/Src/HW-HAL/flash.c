#include "main.h"
#include "stm32l4xx_hal.h"
#include "flash.h"
#include "protocol.h"
extern uint32_t Flash_Sensor_ID;
extern uint32_t DES_Offset_Delay;
flash_status_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len)
{

    uint32_t i;
    for(i = 0; i < Len; i++) {
        //    buff[i] = *(__IO uint8_t*)(dest_addr + i);
        buff[i] = *(__IO uint8_t*)(dest_addr + (Len-i-1));//ZCR
    }
    /* Return a valid address to avoid HardFault */
    return FLASH_OK;
}
flash_status_t Flash_If_DeInit(void)
{
    /* Lock the internal flash */
    HAL_FLASH_Lock();
    return FLASH_OK;
}
static uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }
    return page;
}
//static uint32_t GetBank(uint32_t Addr)
//{
//  uint32_t bank = 0;

//  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
//  {
//    /* No Bank swap */
//    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
//    {
//      bank = FLASH_BANK_1;
//    }
//    else
//    {
//   //   bank = FLASH_BANK_2;
//    }
//  }
//  else
//  {
//    /* Bank swap */
//    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
//    {
//     // bank = FLASH_BANK_2;
//    }
//    else
//    {
//      bank = FLASH_BANK_1;
//    }
//  }

//  return bank;
//}
flash_status_t Flash_If_Init(void)
{
    /* Unlock the internal flash */
    HAL_FLASH_Unlock();
    return FLASH_OK;
}
flash_status_t Flash_If_Erase(uint32_t Add)
{
    uint32_t PageError = 0;
    /* Variable contains Flash operation status */
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseinitstruct;

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    /* Get the number of sector to erase from 1st sector*/
    eraseinitstruct.Banks = 1;//GetBank(Add);
    eraseinitstruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseinitstruct.Page = GetPage(Add);
    eraseinitstruct.NbPages = 1;
    status = HAL_FLASHEx_Erase(&eraseinitstruct, &PageError);

    if (status != HAL_OK)
    {
        return FLASH_ERR;
    }
    return FLASH_OK;
}
flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len)
{
    uint32_t i = 0;
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    for(i = 0; i < Len; i += 8)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by byte */
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(dest_addr+i), *(uint64_t*)(src+i)) == HAL_OK)
        {
            /* Check the written value */
            if(*(uint64_t *)(src + i) != *(uint64_t*)(dest_addr+i))
            {
                /* Flash content doesn't match SRAM content */

                return FLASH_CHECK_ERR;
            }

        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return FLASH_ERR;
        }
    }
    return FLASH_OK;
}
/**
  * 函数功能: 不检查的写入
  * 输入参数: WriteAddr:起始地址
  *           pBuffer:数据指针
  *           NumToWrite:半字(16位)数
  * 返 回 值: 无
  * 说    明：zcr NumToWrite 1写入8个字节
  */
void flash_write_NoCheck ( uint32_t WriteAddr, uint8_t * pBuffer, uint16_t NumToWrite )
{
    uint16_t i;

    for(i=0; i<NumToWrite; i += 8)
    {
        //  HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST,WriteAddr,pBuffer[i]);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(WriteAddr+i), *(uint64_t*)(pBuffer+i));//地址增加2.
    }

}
void Read_flash_Parameter(void)
{
    uint32_t Message_cycle_buf=0 ;
    uint32_t Offset_Delay_buf= 0;
    uint16_t Ctrl_cycle_buf=  0 ;
    uint32_t Alarm_threshold_buf=0;
    uint32_t Alarm_threshold_DOWN_buf=0;
    uint8_t	Frequency_point_buf=0;
    uint8_t	Time_random_buf=0;
    uint32_t SensorID_buf=0;
    Flash_If_Read((uint8_t *)&Message_cycle_buf,Message_cycle_addr,4);
    if(Message_cycle_buf!=0xFFFFFFFF)
    {
        Message_cycle=Message_cycle_buf;
    }
    Flash_If_Read((uint8_t *)&Ctrl_cycle_buf,Ctrl_cycle_addr,2);
    if(Ctrl_cycle_buf!=0xFFFF)
    {
        Ctrl_cycle=Ctrl_cycle_buf;
    }
    Flash_If_Read((uint8_t *)&Time_random_buf,Time_random_addr,1);
    if(Time_random_buf!=0xFF)
    {
        Time_random=Time_random_buf;
    }

    Flash_If_Read((uint8_t *)&Frequency_point_buf,Frequency_point_addr,1);
    if(Frequency_point_buf!=0xFF)
    {
        Frequency_point=Frequency_point_buf;
    }

    Flash_If_Read((uint8_t *)&SensorID_buf,SensorID_Basics,4);
    if(SensorID_buf!=0xFFFFFFFF)
    {
        Flash_Sensor_ID=SensorID_buf;
    }
}

void Write_flash_Parameter(void)
{
    uint8_t Flash_buf[16]= {0}; //内存对齐
//message
    Flash_buf[0]=Message_cycle>>24;
    Flash_buf[1]=Message_cycle>>16;
    Flash_buf[2]=Message_cycle>>8;
    Flash_buf[3]=Message_cycle;
//ctrl
    Flash_buf[4]=Ctrl_cycle>>8;
    Flash_buf[5]=Ctrl_cycle;

    Flash_buf[6]=Time_random;

    Flash_buf[7]=Frequency_point;


    Flash_If_Init();
    Flash_If_Erase(Parameter_Basics);
    Flash_If_Write(Flash_buf,Parameter_Basics,16);//1代表8个字节；每次写8个字节
    Flash_If_DeInit();
}

//void Read_flash_Parameter(void)
//{
//    uint32_t Message_cycle_buf=0 ;
//    uint32_t Offset_Delay_buf= 0;
//    uint16_t Ctrl_cycle_buf=  0 ;
//    uint16_t Alarm_threshold_buf=0;
//    uint8_t	Frequency_point_buf=0;
//    uint8_t Time_random_buf=0;
//    Flash_If_Read((uint8_t *)&Message_cycle_buf,Message_cycle_addr,4);
//    if(Message_cycle_buf!=0xFFFFFFFF)
//    {
//        Message_cycle=Message_cycle_buf;
//    }
//    Flash_If_Read((uint8_t *)&Ctrl_cycle_buf,Ctrl_cycle_addr,2);
//    if(Ctrl_cycle_buf!=0xFFFF)
//    {
//        Ctrl_cycle=Ctrl_cycle_buf;
//    }
////    Flash_If_Read((uint8_t *)&Offset_Delay_buf,Offset_Delay_addr,4);
////    if(Offset_Delay_buf!=0xFFFFFFFF)
////    {
////        Offset_Delay=Offset_Delay_buf;
////    }
////    Flash_If_Read((uint8_t *)&Frequency_point_buf,Frequency_point_addr,1);
////    if(Frequency_point_buf!=0xFF)
////    {
////        Frequency_point=Frequency_point_buf;
////    }
//    Flash_If_Read((uint8_t *)&Alarm_threshold_buf,Alarm_threshold_addr,2);
//    if(Alarm_threshold_buf!=0xFFFF)
//    {
//        Alarm_threshold=Alarm_threshold_buf;
//    }
//    Flash_If_Read((uint8_t *)&Time_random_buf,Time_random_addr,1);
//    if(Time_random_buf!=0xFF)
//    {
//        Time_random=Time_random_buf;
//    }
//}


//void Write_flash_Parameter(void)
//{
//    uint8_t Flash_buf[16]= {0}; //内存对齐
////message
//    Flash_buf[0]=Message_cycle>>24;
//    Flash_buf[1]=Message_cycle>>16;
//    Flash_buf[2]=Message_cycle>>8;
//    Flash_buf[3]=Message_cycle;
////ctrl
//    Flash_buf[4]=Ctrl_cycle>>8;
//    Flash_buf[5]=Ctrl_cycle;
////offset
//    Flash_buf[6]=Offset_Delay>>24;
//    Flash_buf[7]=Offset_Delay>>16;
//    Flash_buf[8]=Offset_Delay>>8;
//    Flash_buf[9]=Offset_Delay;
////frequency
//    Flash_buf[10]=Frequency_point;
////alarm
//    Flash_buf[11]=Alarm_threshold>>8;
//    Flash_buf[12]=Alarm_threshold;
////time_random
//    Flash_buf[13]=Time_random;

//    Flash_If_Init();
//    Flash_If_Erase(Parameter_Basics);
//    Flash_If_Write(Flash_buf,Parameter_Basics,16);//1代表8个字节；每次写8个字节
//    Flash_If_DeInit();
//}


























