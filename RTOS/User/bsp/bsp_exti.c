#include "stm32f4xx.h"
#include "public.h"
#include "bsp_exti.h"
#include "stdio.h"

//外部中断，光耦触发
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
  
    //使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟
    RCC_APB2PeriphClockCmd(INT_SYSCFG_CLK, ENABLE);
  
    /* 配置 NVIC */
    BSP_EXTI_INT_NVIC_Enable();
  
	/* 选择中断引脚 */ 
    GPIO_InitStructure.GPIO_Pin = EXTI_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    /* 设置引脚上拉 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(EXTI_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* 连接 EXTI 中断源到DATAPORT INT引脚 */
    SYSCFG_EXTILineConfig(EXTI_INT_EXTI_PORTSOURCE,EXTI_INT_EXTI_PINSOURCE);

    /* 选择 EXTI 中断源 */
    EXTI_InitStructure.EXTI_Line = EXTI_INT_EXTI_LINE;
    /* 中断模式 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* 下降沿触发 */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    /* 使能中断/事件线 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}

//外部中断，光耦触发
void EXTI_IRQHandler(void)
{
    //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(EXTI_INT_EXTI_LINE) != RESET) 
	{
        //清除中断标志位
		EXTI_ClearITPendingBit(EXTI_INT_EXTI_LINE);     
	}

    BSP_EXTI_INT_NVIC_Disable();
   // printf("I[0x%x]\n",uiTimerCount);//*10us
}

