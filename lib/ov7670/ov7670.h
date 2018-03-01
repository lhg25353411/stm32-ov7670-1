#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ο�������guanfu_wang���롣
//ALIENTEK MiniSTM32������
//OV7670 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/10/31
//�汾��V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////
#define OV7670_VSYNC_PORT GPIOA
#define OV7670_VSYNC_PIN  GPIO_Pin_6
#define OV7670_HREF_PORT GPIOA
#define OV7670_HREF_PIN  GPIO_Pin_7
#define OV7670_DATA   GPIOB->IDR&0x00FF  					//��������˿�

#define ROW_SIZE 44
#define COL_SIZE 36
#define XCLK_H	        GPIOA->BSRR =  GPIO_Pin_8;
#define XCLK_L		    GPIOA->BRR =   GPIO_Pin_8;
/////////////////////////////////////////									
	    				 
u8 ov7670_init(void);
void camera_refresh(void);
void ov7670_window_set(u16 sx,u16 sy,u16 width,u16 height);
#endif





















