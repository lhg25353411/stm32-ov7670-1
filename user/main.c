/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include "ov7670.h"
#include "fifo.h"
#include "lcd.h"
#include "usart.h"


int main(void)
{
	delay_init();
//	uart_init(9600);
//	fifo_init();
	ov7670_init();
//(1)lcd	
	LCD_Init();			   	//初始化LCD
	POINT_COLOR=RED;//设置字体为红色	 
  	LCD_ShowString(60,150,200,200,16,"OV7670 Init...");	
	while(ov7670_init()!=SUCCESS)//初始化OV7670
	{
		delay_ms(1000);
		LCD_ShowString(60,150,200,200,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,150,239,166,WHITE);
		delay_ms(200);
	}
	LCD_ShowString(60,150,200,200,16,"OV7670 Init OK");
//(1)
 	while(1)
	{
//		camera_refresh();	//更新显示
	}
}


