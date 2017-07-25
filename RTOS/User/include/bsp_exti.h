#ifndef _BSP_EXTI_H_
#define _BSP_EXTI_H_

#include "stm32f4xx.h"

//SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��
#define INT_SYSCFG_CLK                RCC_APB2Periph_SYSCFG

//�ж϶���
#define EXTI_INT_GPIO_PORT            GPIOC
#define EXTI_INT_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define EXTI_INT_GPIO_PIN             GPIO_Pin_6
#define EXTI_INT_EXTI_PORTSOURCE      EXTI_PortSourceGPIOC
#define EXTI_INT_EXTI_PINSOURCE       EXTI_PinSource6
#define EXTI_INT_EXTI_LINE            EXTI_Line6
#define EXTI_INT_EXTI_IRQ             EXTI9_5_IRQn

#define EXTI_IRQHandler               EXTI9_5_IRQHandler

#endif
