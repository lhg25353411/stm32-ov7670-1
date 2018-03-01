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
u8 ov_sta;//ov7670���ݶ˿ڷ������ݵ�״ָ̬ʾ��־����ov_staΪ2��ʱ���ʾһ�����ݴ�����ɣ���������16B������
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
	EXTI_InitStructure.EXTI_LineCmd=DISABLE;//ע�⣬�˴���DISABLE
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
    RCC_MCOConfig(RCC_MCO_HSE);//hsi,PA8�����ⲿʱ�ӣ�����

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
 	if(SCCB_WR_Reg(0x12,0x80))return 1;	//��λSCCB	  
	delay_ms(50); 

	u8 ret=0;
	//��ȡ��Ʒ�ͺ�
 	ret=SCCB_RD_Reg(0x0b);   
	if(ret!=0x73)return 2;  
 	ret=SCCB_RD_Reg(0x0a);   
	if(ret!=0x76)return 2;
	//��ʼ������	  
	for(int i=0;i<sizeof(ov7670_init_reg_tbl)/sizeof(ov7670_init_reg_tbl[0]);i++)
	{
	   	SCCB_WR_Reg(ov7670_init_reg_tbl[i][0],ov7670_init_reg_tbl[i][1]);
		delay_ms(2);
 	}
   	return SUCCESS; 	//ok
}
//����ͼ���������
//��QVGA���á�
void ov7670_window_set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width*2;	//V*2
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_RD_Reg(0X03);				//��ȡVref֮ǰ��ֵ
	temp&=0XF0;
	temp|=((endx&0X03)<<2)|(sx&0X03);
	SCCB_WR_Reg(0X03,temp);				//����Vref��start��end�����2λ
	SCCB_WR_Reg(0X19,sx>>2);			//����Vref��start��8λ
	SCCB_WR_Reg(0X1A,endx>>2);			//����Vref��end�ĸ�8λ

	temp=SCCB_RD_Reg(0X32);				//��ȡHref֮ǰ��ֵ
	temp&=0XC0;
	temp|=((endy&0X07)<<3)|(sy&0X07);
	SCCB_WR_Reg(0X17,sy>>3);			//����Href��start��8λ
	SCCB_WR_Reg(0X18,endy>>3);			//����Href��end�ĸ�8λ
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
//		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������ 
//		LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
//		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	

//		for(u32 j=0;j<76800;j++)
//		{
//			/*//��RGB
//			
//			GPIOB->CRL=0X88888888;		   
//			OV7670_RCK=0; 
//			color=OV7670_DATA;		//������
//						
//			OV7670_RCK=1;
//			color<<=8;					  
// 			OV7670_RCK=0;
//			color|=OV7670_DATA;		//������	
//			
//			OV7670_RCK=1; 
//			GPIOB->CRL=0X33333333;						 	 
//			LCD_WR_DATA(color);	 
//			*/
//			//��yuv
//			GPIOB->CRL=0X88888888;		   
//			color=OV7670_DATA;		//������
//			color<<=8;					  
//			GPIOB->CRL=0X33333333;
//			color_r = color&0xf800;
//			color_b = color>>11;
//			color >>=5;
//			color_g =color&0x07e0; 
//			
//			LCD_WR_DATA(color_r+color_b+color_g);				
//		}  
//		EXTI->PR=1<<15;     		//���LINE8�ϵ��жϱ�־λ
//		ov_sta=0;					//��ʼ��һ�βɼ�
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 	  				 	 
//	} 
//}


void EXTI9_5_IRQHandler(void)
{			
	if(EXTI_GetITStatus(EXTI_Line6)==SET)//��6�ߵ��ж�
	{
		EXTI_ClearITPendingBit(EXTI_Line6);     //���LINE6�ϵ��жϱ�־λ
		EXTI->IMR |= 1<<6;
		col_index=row_index=0;
	}
	if(EXTI_GetITStatus(EXTI_Line7)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);     //���LINE7�ϵ��жϱ�־λ
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

