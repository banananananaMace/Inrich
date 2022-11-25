#ifndef __SENCTR_CH_TASK_H
#define __SENCTR_CH_TASK_H

#include "sys.h"
#include "stdio.h"
#include <stdbool.h>

//变量申明

//函数申明
extern uint8_t  SenCTR_CH_DataPro(uint8_t *Redata_pt);
extern bool	SenCTR_ComPara_REQRSP(uint16_t SenID_Serial,u8 Rsp_type);
extern bool	SenCTR_CtrPara_REQRSP(uint16_t SenID_Serial,u8 Rs_type);
extern void  BURST_ACK_Send(uint8_t *SenID);
#endif
