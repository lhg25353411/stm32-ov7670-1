#include "stm32f10x.h"
#include "ov7670.h"
#include "sys.h"
static void fifo_gpio_configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA15��������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
}



static void fifo_nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);
	
}

void fifo_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
							RCC_APB2Periph_AFIO,ENABLE);
	fifo_gpio_configuration();
	fifo_nvic_config();
//	JTAG_Set(SWD_ENABLE);	//�ر�JTAG  	 
}

//�жϷ�����
u8 ov_sta;
void EXTI15_10_IRQHandler(void)
{			
	if(EXTI_GetITStatus(EXTI_Line15)==SET)//��15�ߵ��ж�
	{     
		if(ov_sta<2)
		{
			if(ov_sta==0)
			{
				OV7670_WRST=0;	 	//��λдָ��		  		 
				OV7670_WRST=1;	
				OV7670_WREN=1;		//����д��FIFO
			}else OV7670_WREN=0;	//��ֹд��FIFO 	 
			ov_sta++;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line15);     //���LINE15�ϵ��жϱ�־λ	
}

