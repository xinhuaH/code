#include "bsp_timer.h"
#include "bsp_usart.h"


volatile unsigned int uiTimerCount = 0;


 /**
  * @brief  高级控制定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void BSP_TIMER_Nvic_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQn;
    // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // 设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void BSP_TIMER_Mode_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    // 开启TIMx_CLK,x[1,8] 
    RCC_APB2PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到499，即为500次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Period = 500-1;       

    // 高级控制定时器时钟源TIMxCLK = HCLK=180MHz 
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
    TIM_TimeBaseStructure.TIM_Prescaler = 18000-1;	
    // 采样时钟分频
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // 计数方式
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // 重复计数器:重复1次，即计数两次才生成一个中断
    TIM_TimeBaseStructure.TIM_RepetitionCounter=1;

    // 初始化定时器TIMx, x[1,8]
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

    // 清除定时器更新中断标志位
    TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);

    // 开启定时器更新中断
    TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);

    // 使能定时器
    TIM_Cmd(ADVANCE_TIM, ENABLE);	
}

void BSP_TIMER_Init(void)
{
    BSP_TIMER_Nvic_Init();
    BSP_TIMER_Mode_Init();
}

unsigned int getSystemTime(void)
{
    return uiTimerCount;
}

void ADVANCE_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus( ADVANCE_TIM, TIM_IT_Update) != RESET) 
	{
		/* 发送一个字节数据到串口 */
		//USART_SendData(USART2, 0x31);

		/* 等待发送完毕 */
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        uiTimerCount++;

		TIM_ClearITPendingBit(ADVANCE_TIM , TIM_IT_Update);
	}		 	
}

/*********************************************END OF FILE**********************/
