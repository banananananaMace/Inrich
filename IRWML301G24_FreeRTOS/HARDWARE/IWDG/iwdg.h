#ifndef _IWDG_H
#define _IWDG_H
#include "sys.h"
//V1.1 20140504
//�����˴��ڿ��Ź���غ�����
////////////////////////////////////////////////////////////////////////////////// 	
extern void IWDG_Init(u8 prer,u16 rlr);//IWDG��ʼ��
extern void IWDG_Feed(void);  //ι������
#endif
