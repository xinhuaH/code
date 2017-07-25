#ifndef __USART_H
#define	__USART_H

#include "stm32f4xx.h"
#include <stdio.h>

/*******************************************************/

/* 不同的串口挂载的总线不一样，时钟使能函数也不一样，移植时要注意 
 * 串口1和6是      RCC_APB2PeriphClockCmd
 * 串口2/3/4/5/7是 RCC_APB1PeriphClockCmd
 */

/************************UART1************************/

#define USART1_CLK                         RCC_APB2Periph_USART1
#define USART1_CLOCKCMD                    RCC_APB2PeriphClockCmd
#define USART1_BAUDRATE                    115200  //串口波特率
#define USART1_RX_GPIO_PORT                GPIOA
#define USART1_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define USART1_RX_PIN                      GPIO_Pin_10
#define USART1_RX_AF                       GPIO_AF_USART1
#define USART1_RX_SOURCE                   GPIO_PinSource10

#define USART1_TX_GPIO_PORT                GPIOA
#define USART1_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define USART1_TX_PIN                      GPIO_Pin_9
#define USART1_TX_AF                       GPIO_AF_USART1
#define USART1_TX_SOURCE                   GPIO_PinSource9

/*************************UART2************************/

#define USART2_CLK                         RCC_APB1Periph_USART2
#define USART2_CLOCKCMD                    RCC_APB1PeriphClockCmd
#define USART2_BAUDRATE                    115200  //串口波特率

#define USART2_RX_GPIO_PORT                GPIOD
#define USART2_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define USART2_RX_PIN                      GPIO_Pin_6
#define USART2_RX_AF                       GPIO_AF_USART2
#define USART2_RX_SOURCE                   GPIO_PinSource6

#define USART2_TX_GPIO_PORT                GPIOD
#define USART2_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define USART2_TX_PIN                      GPIO_Pin_5
#define USART2_TX_AF                       GPIO_AF_USART2
#define USART2_TX_SOURCE                   GPIO_PinSource5

/************************************************************/

typedef void (*UartSendFun)(unsigned char *ucpBuf, unsigned int uiLength);
void BSP_USART1_Init(void);
void BSP_USART2_Init(void);
void BSP_USART_Init(void);
void BSP_USART_Send(unsigned char *ucpBuf, unsigned int uiLength);

#endif /* __USART_H */

