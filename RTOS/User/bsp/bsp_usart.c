#include "bsp_usart.h"
#include "message_deliver.h"
#include "protocol_analysis.h"

void BSP_USART_Init(void)
{
    BSP_USART1_Init();
    BSP_USART2_Init();
}

void BSP_USART1_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;//�жϽṹ�嶨��
        
  RCC_AHB1PeriphClockCmd(USART1_RX_GPIO_CLK|USART1_TX_GPIO_CLK,ENABLE);

  /* ʹ�� USART ʱ�� */
  USART1_CLOCKCMD(USART1_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART1_TX_PIN  ;  
  GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART1_RX_PIN;
  GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(USART1_RX_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(USART1_TX_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ��żУ�� */  
    USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(USART1, &USART_InitStructure); 
    
  /* ʹ�ܴ��� */
  USART_Cmd(USART1, ENABLE);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܻ���ʧ��ָ����USART�ж� �����ж�
  USART_ClearFlag(USART1,USART_FLAG_TC);//���USARTx�Ĵ������־λ

  /* ����NVIC���� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);    
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;            //��USART1��ȫ���ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    //��ռ���ȼ�Ϊ`1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //��Ӧ���ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //ʹ��
  NVIC_Init(&NVIC_InitStructure);
}

void BSP_USART2_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;//�жϽṹ�嶨��
        
  RCC_AHB1PeriphClockCmd(USART2_RX_GPIO_CLK|USART2_TX_GPIO_CLK,ENABLE);

  /* ʹ�� USART ʱ�� */
  USART2_CLOCKCMD(USART2_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART2_TX_PIN  ;  
  GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART2_RX_PIN;
  GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(USART2_RX_GPIO_PORT,USART2_RX_SOURCE,USART2_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(USART2_TX_GPIO_PORT,USART2_TX_SOURCE,USART2_TX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = USART2_BAUDRATE;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ��żУ�� */  
    USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(USART2, &USART_InitStructure); 

  /* ʹ�ܴ��� */
  USART_Cmd(USART2, ENABLE);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ʹ�ܻ���ʧ��ָ����USART�ж� �����ж�
  USART_ClearFlag(USART2,USART_FLAG_TC);//���USARTx�Ĵ������־λ

  /* ����NVIC���� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);    
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;            //��USART1��ȫ���ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    //��ռ���ȼ�Ϊ2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;           //��Ӧ���ȼ�Ϊ2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //ʹ��
  NVIC_Init(&NVIC_InitStructure);
}

void BSP_USART_Send(unsigned char *ucpBuf, unsigned int uiLength)
{
    unsigned int uiNum = 0;

    for(uiNum = 0; uiNum < uiLength; uiNum++)
    {
        USART_SendData(USART2, ucpBuf[uiNum]);

        /* �ȴ�������� */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
        /* ����һ���ֽ����ݵ����� */
        USART_SendData(USART2, (uint8_t) ch);
        
        /* �ȴ�������� */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);       
    
        return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
        /* �ȴ������������� */
        while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

        return (int)USART_ReceiveData(USART2);
}

void USART1_IRQHandler(void)
{
	u8 k;

	USART_ClearFlag(USART1,USART_FLAG_TC);
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
        dataRecievedRefreshed();

		k=USART_ReceiveData(USART1);

		//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);

        dataRecieved(k);

        if(k == PROTOCOL_DATA_END)
        {
            dataRecievedCountUpdate();
            dataCollectTaskNotify();
        }
	}
}
void USART2_IRQHandler(void)
{
	static u8 k;
	USART_ClearFlag(USART2,USART_FLAG_TC);
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=Bit_RESET)//���ָ����USART�жϷ������
	{
        dataRecievedRefreshed();

		k=USART_ReceiveData(USART2);

		//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);

        dataRecieved(k);

        if(k == PROTOCOL_DATA_END)
        {
            dataRecievedCountUpdate();
            dataCollectTaskNotify();
        }
	}
}
/*********************************************END OF FILE**********************/
