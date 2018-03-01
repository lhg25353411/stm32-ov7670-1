#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "delay.h"
#include "sys.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stdio.h"


 
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);

#endif
