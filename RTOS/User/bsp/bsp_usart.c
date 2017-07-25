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
  NVIC_InitTypeDef  NVIC_InitStructure;//中断结构体定义
        
  RCC_AHB1PeriphClockCmd(USART1_RX_GPIO_CLK|USART1_TX_GPIO_CLK,ENABLE);

  /* 使能 USART 时钟 */
  USART1_CLOCKCMD(USART1_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART1_TX_PIN  ;  
  GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART1_RX_PIN;
  GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(USART1_RX_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(USART1_TX_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：偶校验 */  
    USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(USART1, &USART_InitStructure); 
    
  /* 使能串口 */
  USART_Cmd(USART1, ENABLE);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
  USART_ClearFlag(USART1,USART_FLAG_TC);//清除USARTx的待处理标志位

  /* 设置NVIC参数 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);    
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;            //打开USART1的全局中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    //抢占优先级为`1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //响应优先级为0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能
  NVIC_Init(&NVIC_InitStructure);
}

void BSP_USART2_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;//中断结构体定义
        
  RCC_AHB1PeriphClockCmd(USART2_RX_GPIO_CLK|USART2_TX_GPIO_CLK,ENABLE);

  /* 使能 USART 时钟 */
  USART2_CLOCKCMD(USART2_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART2_TX_PIN  ;  
  GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =  USART2_RX_PIN;
  GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(USART2_RX_GPIO_PORT,USART2_RX_SOURCE,USART2_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(USART2_TX_GPIO_PORT,USART2_TX_SOURCE,USART2_TX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = USART2_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：偶校验 */  
    USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(USART2, &USART_InitStructure); 

  /* 使能串口 */
  USART_Cmd(USART2, ENABLE);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
  USART_ClearFlag(USART2,USART_FLAG_TC);//清除USARTx的待处理标志位

  /* 设置NVIC参数 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);    
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;            //打开USART1的全局中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    //抢占优先级为2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;           //响应优先级为2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能
  NVIC_Init(&NVIC_InitStructure);
}

void BSP_USART_Send(unsigned char *ucpBuf, unsigned int uiLength)
{
    unsigned int uiNum = 0;

    for(uiNum = 0; uiNum < uiLength; uiNum++)
    {
        USART_SendData(USART2, ucpBuf[uiNum]);

        /* 等待发送完毕 */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
        /* 发送一个字节数据到串口 */
        USART_SendData(USART2, (uint8_t) ch);
        
        /* 等待发送完毕 */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);       
    
        return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
        /* 等待串口输入数据 */
        while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

        return (int)USART_ReceiveData(USART2);
}

void USART1_IRQHandler(void)
{
	u8 k;

	USART_ClearFlag(USART1,USART_FLAG_TC);
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
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
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=Bit_RESET)//检查指定的USART中断发生与否
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
