#include "stm32f4xx.h"
#include "public.h"
#include "bsp_exti.h"
#include "stdio.h"

//�ⲿ�жϣ������
void BSP_EXTI_INT_NVIC_Enable(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI_INT_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void BSP_EXTI_INT_NVIC_Disable(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI_INT_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void BSP_EXTI_INT_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    EXTI_InitTypeDef EXTI_InitStructure;
  
	RCC_AHB1PeriphClockCmd(EXTI_INT_GPIO_CLK, ENABLE);
  
    //ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��
    RCC_APB2PeriphClockCmd(INT_SYSCFG_CLK, ENABLE);
  
    /* ���� NVIC */
    BSP_EXTI_INT_NVIC_Enable();
  
	/* ѡ���ж����� */ 
    GPIO_InitStructure.GPIO_Pin = EXTI_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    /* ������������ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(EXTI_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ��DATAPORT INT���� */
    SYSCFG_EXTILineConfig(EXTI_INT_EXTI_PORTSOURCE,EXTI_INT_EXTI_PINSOURCE);

    /* ѡ�� EXTI �ж�Դ */
    EXTI_InitStructure.EXTI_Line = EXTI_INT_EXTI_LINE;
    /* �ж�ģʽ */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* �½��ش��� */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    /* ʹ���ж�/�¼��� */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}

//�ⲿ�жϣ������
void EXTI_IRQHandler(void)
{
    //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(EXTI_INT_EXTI_LINE) != RESET) 
	{
        //����жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_INT_EXTI_LINE);     
	}

    BSP_EXTI_INT_NVIC_Disable();
   // printf("I[0x%x]\n",uiTimerCount);//*10us
}

