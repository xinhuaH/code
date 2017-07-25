#include "stm32f4xx.h"
#include "bsp_data_port.h"
#include "public.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

#include "stdio.h"

extern volatile unsigned int uiTimerCount;

DataPort WE = {WRITE_ENABLE_PIN, WRITE_ENABLE_GPIO_PORT, WRITE_ENABLE_GPIO_CLK};
DataPort RE = {READ_ENABLE_PIN, READ_ENABLE_GPIO_PORT, READ_ENABLE_GPIO_CLK};

DataPort Address[ADDRESS_WIDTH] = {
    {ADDRESS_0_PIN, ADDRESS_0_GPIO_PORT, ADDRESS_0_GPIO_CLK},
    {ADDRESS_1_PIN, ADDRESS_1_GPIO_PORT, ADDRESS_1_GPIO_CLK},
    {ADDRESS_2_PIN, ADDRESS_2_GPIO_PORT, ADDRESS_2_GPIO_CLK},
    {ADDRESS_3_PIN, ADDRESS_3_GPIO_PORT, ADDRESS_3_GPIO_CLK},
    {ADDRESS_4_PIN, ADDRESS_4_GPIO_PORT, ADDRESS_4_GPIO_CLK},
    {ADDRESS_5_PIN, ADDRESS_5_GPIO_PORT, ADDRESS_5_GPIO_CLK},
    {ADDRESS_6_PIN, ADDRESS_6_GPIO_PORT, ADDRESS_6_GPIO_CLK},
    {ADDRESS_7_PIN, ADDRESS_7_GPIO_PORT, ADDRESS_7_GPIO_CLK},
    {ADDRESS_8_PIN, ADDRESS_8_GPIO_PORT, ADDRESS_8_GPIO_CLK},
    {ADDRESS_9_PIN, ADDRESS_9_GPIO_PORT, ADDRESS_9_GPIO_CLK},
    {ADDRESS_10_PIN, ADDRESS_10_GPIO_PORT, ADDRESS_10_GPIO_CLK},
    {ADDRESS_11_PIN, ADDRESS_11_GPIO_PORT, ADDRESS_11_GPIO_CLK},
};

DataPort Data[DATA_WIDTH] = {
    {DATA_0_PIN, DATA_0_GPIO_PORT, DATA_0_GPIO_CLK},
    {DATA_1_PIN, DATA_1_GPIO_PORT, DATA_1_GPIO_CLK},
    {DATA_2_PIN, DATA_2_GPIO_PORT, DATA_2_GPIO_CLK},
    {DATA_3_PIN, DATA_3_GPIO_PORT, DATA_3_GPIO_CLK},
    {DATA_4_PIN, DATA_4_GPIO_PORT, DATA_4_GPIO_CLK},
    {DATA_5_PIN, DATA_5_GPIO_PORT, DATA_5_GPIO_CLK},
    {DATA_6_PIN, DATA_6_GPIO_PORT, DATA_6_GPIO_CLK},
    {DATA_7_PIN, DATA_7_GPIO_PORT, DATA_7_GPIO_CLK},
};

void BSP_DATAPORT_Init(void)
{
    BSP_DATAPORT_Write_Read_Init();
    BSP_DATAPORT_CLK_Enable(&Address[0], ADDRESS_WIDTH);
    BSP_DATAPORT_CLK_Enable(&Data[0], DATA_WIDTH);
    BSP_DATAPORT_INT_Init();
}

void BSP_DATAPORT_CLK_Enable(DataPort *tpDataPort, uint32_t u32Width)
{
    uint32_t u32GpioClk = 0;
    uint32_t u32GpioNum = 0;

    if(tpDataPort == NULL)
    {
        printf("tpDataPort is NULL in BSP_DATAPORT_CLK_Init\n");
        return;
    }

    for(u32GpioNum = 0; u32GpioNum < u32Width; u32GpioNum++)
    {
        u32GpioClk |= tpDataPort[u32GpioNum].u32GpioClk;
    }

	/*����GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(u32GpioClk,ENABLE);
}

void BSP_DATAPORT_ModeSet(DataPort *tpDataPort, uint16_t u16Mode, uint32_t u32Width)
{
    uint32_t u32GpioNum = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

    if(tpDataPort == NULL)
    {
        printf("tpDataPort is NULL in BSP_DATAPORT_ModeSet\n");
        return;
    }

    /*������������Ϊ100MHz*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    /*�������ŵ��������Ϊ��©���*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    /*��������Ϊ����ģʽ*//*���Զ�ȡ��©������豸*/
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    if(u16Mode == BK_MODE_OUT)
    {
        /*��������Ϊ���ģʽ*/
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    }
    else
    {
        /*��������Ϊ����ģʽ*/
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    }

    for(u32GpioNum = 0; u32GpioNum < u32Width; u32GpioNum++)
    {
        /*ѡ�����ʼ��������*/
	    GPIO_InitStructure.GPIO_Pin = tpDataPort[u32GpioNum].u16PinNum;

        /*ʹ������Ľṹ���ʼ��ADDRESS*/
	    GPIO_Init(tpDataPort[u32GpioNum].tpGpioPort, &GPIO_InitStructure);
    }
}

void BSP_DATAPORT_ADDRESS_ModeSet(uint16_t u16Mode)
{
    static uint16_t u16AddressMode = 0;

    if(u16AddressMode == u16Mode)
    {
        return;
    }

    u16AddressMode = u16Mode;

    BSP_DATAPORT_ModeSet(&Address[0], u16Mode, ADDRESS_WIDTH);
}

void BSP_DATAPORT_DATA_ModeSet(uint16_t u16Mode)
{
    static uint16_t u16DataMode = 0;

    if(u16DataMode == u16Mode)
    {
        return;
    }

    u16DataMode = u16Mode;

    BSP_DATAPORT_ModeSet(&Data[0], u16Mode, DATA_WIDTH);
}

void BSP_DATAPORT_Write_Read_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*������������Ϊ100MHz*/   
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    /*�������ŵ��������Ϊ��©���*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    /*��������Ϊ����ģʽ*//*���Զ�ȡ��©������豸*/
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    /*��������Ϊ���ģʽ*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;

    GPIO_InitStructure.GPIO_Pin = WE.u16PinNum;
    GPIO_Init(WE.tpGpioPort, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RE.u16PinNum;
    GPIO_Init(RE.tpGpioPort, &GPIO_InitStructure);

	/*����GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(WE.u32GpioClk, ENABLE);
    RCC_AHB1PeriphClockCmd(RE.u32GpioClk, ENABLE);

    BSP_DATAPORT_Read_Disable();
    BSP_DATAPORT_Write_Disable();
}

void BSP_DATAPORT_Read_Enable(void)
{
    GPIO_WriteBit(RE.tpGpioPort, RE.u16PinNum, Bit_RESET);//Enable RE
}

void BSP_DATAPORT_Read_Disable(void)
{
    GPIO_WriteBit(RE.tpGpioPort, RE.u16PinNum, Bit_SET);//Disable RE
}

void BSP_DATAPORT_Write_Enable(void)
{
    GPIO_WriteBit(WE.tpGpioPort, WE.u16PinNum, Bit_RESET);//Enable WE
}

void BSP_DATAPORT_Write_Disable(void)
{
    GPIO_WriteBit(WE.tpGpioPort, WE.u16PinNum, Bit_SET);//Disable WE
}

void BSP_DATAPORT_Read(uint32_t u32Address, uint32_t *u32pData)
{
    uint8_t   u8Value       = 0;
    uint32_t  u32GpioNum    = 0;
    uint32_t  u32ReadValue  = 0;
    BitAction tValue        = Bit_RESET;

    if(u32pData == NULL)
    {
        printf("u32pData is NULL in BSP_DATAPORT_Read\n");
        return;
    }

    for(u32GpioNum = 0; u32GpioNum < ADDRESS_WIDTH; u32GpioNum++)
    {
        tValue = ((u32Address >> u32GpioNum) & 0x01) ? Bit_SET:Bit_RESET;

        GPIO_WriteBit(Address[u32GpioNum].tpGpioPort, Address[u32GpioNum].u16PinNum, tValue);
    }

    executeDelay(5);

    BSP_DATAPORT_Read_Enable();

    executeDelay(5);

    for(u32GpioNum = 0; u32GpioNum < DATA_WIDTH; u32GpioNum++)
    {
        u8Value = GPIO_ReadInputDataBit(Data[u32GpioNum].tpGpioPort, Data[u32GpioNum].u16PinNum);
        u32ReadValue |= (u8Value ? 1:0) << u32GpioNum;
    }

    *u32pData = u32ReadValue;

    BSP_DATAPORT_Read_Disable();
}

void BSP_DATAPORT_Write(uint32_t u32Address, uint32_t u32Data)
{
    uint32_t  u32GpioNum  = 0;
    BitAction tValue      = Bit_RESET;

    for(u32GpioNum = 0; u32GpioNum < ADDRESS_WIDTH; u32GpioNum++)
    {
        tValue = ((u32Address >> u32GpioNum) & 0x01) ? Bit_SET:Bit_RESET;

        GPIO_WriteBit(Address[u32GpioNum].tpGpioPort, Address[u32GpioNum].u16PinNum, tValue);
    }

    for(u32GpioNum = 0; u32GpioNum < DATA_WIDTH; u32GpioNum++)
    {
        tValue = ((u32Data >> u32GpioNum) & 0x01) ? Bit_SET:Bit_RESET;
        GPIO_WriteBit(Data[u32GpioNum].tpGpioPort, Data[u32GpioNum].u16PinNum, tValue);
    }
}

void BSP_DATAPORT_INT_NVIC_Enable(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitStructure.NVIC_IRQChannel = DATAPORT_INT_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void BSP_DATAPORT_INT_NVIC_Disable(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DATAPORT_INT_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void BSP_DATAPORT_INT_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    EXTI_InitTypeDef EXTI_InitStructure;
  
	RCC_AHB1PeriphClockCmd(DATAPORT_INT_GPIO_CLK, ENABLE);
  
    //ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��
    RCC_APB2PeriphClockCmd(DATAPORT_INT_SYSCFG_CLK, ENABLE);
  
    /* ���� NVIC */
    BSP_DATAPORT_INT_NVIC_Enable();
  
	/* ѡ���ж����� */ 
    GPIO_InitStructure.GPIO_Pin = DATAPORT_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    /* ������������ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(DATAPORT_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ��DATAPORT INT���� */
    SYSCFG_EXTILineConfig(DATAPORT_INT_EXTI_PORTSOURCE,DATAPORT_INT_EXTI_PINSOURCE);

    /* ѡ�� EXTI �ж�Դ */
    EXTI_InitStructure.EXTI_Line = DATAPORT_INT_EXTI_LINE;
    /* �ж�ģʽ */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* �½��ش��� */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    /* ʹ���ж�/�¼��� */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}

//���������ж�
void DATAPORT_IRQHandler(void)
{
    //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(DATAPORT_INT_EXTI_LINE) != RESET) 
	{
        //����жϱ�־λ
		EXTI_ClearITPendingBit(DATAPORT_INT_EXTI_LINE);     
	}

    BSP_DATAPORT_INT_NVIC_Disable();
    printf("I[0x%x]\n",uiTimerCount);//*10us
}

