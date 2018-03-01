#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网友guanfu_wang代码。
//ALIENTEK MiniSTM32开发板
//OV7670 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/10/31
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////
#define OV7670_VSYNC_PORT GPIOA
#define OV7670_VSYNC_PIN  GPIO_Pin_6
#define OV7670_HREF_PORT GPIOA
#define OV7670_HREF_PIN  GPIO_Pin_7
#define OV7670_DATA   GPIOB->IDR&0x00FF  					//数据输入端口

#define ROW_SIZE 44
#define COL_SIZE 36
#define XCLK_H	        GPIOA->BSRR =  GPIO_Pin_8;
#define XCLK_L		    GPIOA->BRR =   GPIO_Pin_8;
/////////////////////////////////////////									
	    				 
u8 ov7670_init(void);
void camera_refresh(void);
void ov7670_window_set(u16 sx,u16 sy,u16 width,u16 height);
#endif





















