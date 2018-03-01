#include "ov7670.h"
#include "ov7670cfg.h"
#include "delay.h"
#include "sccb.h"
#include "lcd.h"
static void ov7670_rcc_config(void);
static void ov7670_port_config(void);
static void ov7670_vsync_config(void);
static void ov7670_xclk_config(void);
static u8 ov7670_reg_config(void);


u8 img[ROW_SIZE][COL_SIZE]={0};
u16 row_index=0,col_index=0;
u8 ov_sta;//ov7670数据端口发送数据的状态指示标志，当ov_sta为2的时候表示一次数据传输完成，即接收了16B的数据
/***********************************************************/
static void ov7670_rcc_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|
							RCC_APB2Periph_GPIOA,ENABLE);
}
static void ov7670_port_config(void)
{
	sccb_port_config();//SCL(PA4),SDA(PA5)
	ov7670_vsync_config();
	ov7670_xclk_config();
}

static void ov7670_vsync_config(void)
{
	//VSYNC(PA6)
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = OV7670_VSYNC_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
    GPIO_Init(OV7670_VSYNC_PORT, &GPIO_InitStructure);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line6;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);
}
static void ov7670_href_config(void)
{
	//href(PA7)
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = OV7670_HREF_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
    GPIO_Init(OV7670_HREF_PORT, &GPIO_InitStructure);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
	EXTI_InitStructure.EXTI_LineCmd=DISABLE;//注意，此处是DISABLE
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
}

static void ov7670_xclk_config(void)
{
	//XCLK(PA8)
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    RCC_MCOConfig(RCC_MCO_HSE);//hsi,PA8输入外部时钟，晶振

}


static void ov7670_pclk_config(void)
{
	//PCLK(PA3)
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}
static u8 ov7670_reg_config(void)
{
 	if(SCCB_WR_Reg(0x12,0x80))return 1;	//复位SCCB	  
	delay_ms(50); 

	u8 ret=0;
	//读取产品型号
 	ret=SCCB_RD_Reg(0x0b);   
	if(ret!=0x73)return 2;  
 	ret=SCCB_RD_Reg(0x0a);   
	if(ret!=0x76)return 2;
	//初始化序列	  
	for(int i=0;i<sizeof(ov7670_init_reg_tbl)/sizeof(ov7670_init_reg_tbl[0]);i++)
	{
	   	SCCB_WR_Reg(ov7670_init_reg_tbl[i][0],ov7670_init_reg_tbl[i][1]);
		delay_ms(2);
 	}
   	return SUCCESS; 	//ok
}
//设置图像输出窗口
//对QVGA设置。
void ov7670_window_set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width*2;	//V*2
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endx&0X03)<<2)|(sx&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sx>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endx>>2);			//设置Vref的end的高8位

	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endy&0X07)<<3)|(sy&0X07);
	SCCB_WR_Reg(0X17,sy>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endy>>3);			//设置Href的end的高8位
}
u8 ov7670_init(void)
{
	ov7670_rcc_config();
	ov7670_port_config();
	ov7670_window_set(10,174,240,320);
	if(ov7670_reg_config()==SUCCESS) return SUCCESS;
	else return ERROR;
}

//void camera_refresh(void)
//{
// 	u16 color;
//	u16 color_r;
//	u16 color_g;
//	u16 color_b;

//	if(ov_sta==2)
//	{
//		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右 
//		LCD_SetCursor(0x00,0x0000);	//设置光标位置 
//		LCD_WriteRAM_Prepare();     //开始写入GRAM	

//		for(u32 j=0;j<76800;j++)
//		{
//			/*//读RGB
//			
//			GPIOB->CRL=0X88888888;		   
//			OV7670_RCK=0; 
//			color=OV7670_DATA;		//读数据
//						
//			OV7670_RCK=1;
//			color<<=8;					  
// 			OV7670_RCK=0;
//			color|=OV7670_DATA;		//读数据	
//			
//			OV7670_RCK=1; 
//			GPIOB->CRL=0X33333333;						 	 
//			LCD_WR_DATA(color);	 
//			*/
//			//读yuv
//			GPIOB->CRL=0X88888888;		   
//			color=OV7670_DATA;		//读数据
//			color<<=8;					  
//			GPIOB->CRL=0X33333333;
//			color_r = color&0xf800;
//			color_b = color>>11;
//			color >>=5;
//			color_g =color&0x07e0; 
//			
//			LCD_WR_DATA(color_r+color_b+color_g);				
//		}  
//		EXTI->PR=1<<15;     		//清除LINE8上的中断标志位
//		ov_sta=0;					//开始下一次采集
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 	  				 	 
//	} 
//}


void EXTI9_5_IRQHandler(void)
{			
	if(EXTI_GetITStatus(EXTI_Line6)==SET)//是6线的中断
	{
		EXTI_ClearITPendingBit(EXTI_Line6);     //清除LINE6上的中断标志位
		EXTI->IMR |= 1<<6;
		col_index=row_index=0;
	}
	if(EXTI_GetITStatus(EXTI_Line7)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);     //清除LINE7上的中断标志位
		img[row_index][col_index++]=GPIOB->IDR&0xff;
		if(col_index==COL_SIZE)
		{
			col_index=0;
			row_index++;
			if(row_index==ROW_SIZE)
				row_index=0;
		}
	}
	
}

