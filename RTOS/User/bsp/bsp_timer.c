#include "bsp_timer.h"
#include "bsp_usart.h"


volatile unsigned int uiTimerCount = 0;


 /**
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void BSP_TIMER_Nvic_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQn;
    // ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // ������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void BSP_TIMER_Mode_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    // ����TIMx_CLK,x[1,8] 
    RCC_APB2PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

    /* �ۼ� TIM_Period�������һ�����»����ж�*/		
    //����ʱ����0������499����Ϊ500�Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Period = 500-1;       

    // �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=180MHz 
    // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
    TIM_TimeBaseStructure.TIM_Prescaler = 18000-1;	
    // ����ʱ�ӷ�Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // ������ʽ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // �ظ�������:�ظ�1�Σ����������β�����һ���ж�
    TIM_TimeBaseStructure.TIM_RepetitionCounter=1;

    // ��ʼ����ʱ��TIMx, x[1,8]
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

    // �����ʱ�������жϱ�־λ
    TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);

    // ������ʱ�������ж�
    TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);

    // ʹ�ܶ�ʱ��
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
		/* ����һ���ֽ����ݵ����� */
		//USART_SendData(USART2, 0x31);

		/* �ȴ�������� */
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        uiTimerCount++;

		TIM_ClearITPendingBit(ADVANCE_TIM , TIM_IT_Update);
	}		 	
}

/*********************************************END OF FILE**********************/
