#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网友guanfu_wang代码。
//ALIENTEK MiniSTM32开发板
//OV7670 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/10/31
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

#define SCCB_SDA_IN()  {GPIOA->CRL&=0XFF0FFFFF;GPIOA->CRL|=0X00800000;}
#define SCCB_SDA_OUT() {GPIOA->CRL&=0XFF0FFFFF;GPIOA->CRL|=0X00300000;}

//IO操作函数	 
#define SCCB_PORT           GPIOA
#define SCCB_SCL_PIN        GPIO_Pin_4
#define SCCB_SDA_PIN        GPIO_Pin_5
#define SCCB_SCL    		PAout(4)	 	//SCL
#define SCCB_SDA    		PAout(5) 		//SDA	 

#define SCCB_READ_SDA    	PAin(5)  		//输入SDA    
#define SCCB_ID   			0X42  			//OV7670的ID

///////////////////////////////////////////
void sccb_port_config(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif













