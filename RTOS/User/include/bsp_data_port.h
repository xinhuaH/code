#ifndef _BSP_DATA_PORT_H_
#define _BSP_DATA_PORT_H_

#include "stm32f4xx.h"

typedef struct dataPortStruct
{
    uint16_t     u16PinNum;
    GPIO_TypeDef *tpGpioPort;
    uint32_t     u32GpioClk;
}DataPort;

//位宽定义
#define ADDRESS_WIDTH  12
#define DATA_WIDTH     8

#define READ_DELAY     1
#define WRITE_DELAY    1

//引脚定义
/*******************************************************/
#define ADDRESS_0_PIN             GPIO_Pin_6
#define ADDRESS_0_GPIO_PORT       GPIOA
#define ADDRESS_0_GPIO_CLK        RCC_AHB1Periph_GPIOA

#define ADDRESS_1_PIN             GPIO_Pin_10
#define ADDRESS_1_GPIO_PORT       GPIOH
#define ADDRESS_1_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_2_PIN             GPIO_Pin_10
#define ADDRESS_2_GPIO_PORT       GPIOH
#define ADDRESS_2_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_3_PIN             GPIO_Pin_10
#define ADDRESS_3_GPIO_PORT       GPIOH
#define ADDRESS_3_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_4_PIN             GPIO_Pin_10
#define ADDRESS_4_GPIO_PORT       GPIOH
#define ADDRESS_4_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_5_PIN             GPIO_Pin_10
#define ADDRESS_5_GPIO_PORT       GPIOH
#define ADDRESS_5_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_6_PIN             GPIO_Pin_10
#define ADDRESS_6_GPIO_PORT       GPIOH
#define ADDRESS_6_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_7_PIN             GPIO_Pin_10
#define ADDRESS_7_GPIO_PORT       GPIOH
#define ADDRESS_7_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_8_PIN             GPIO_Pin_10
#define ADDRESS_8_GPIO_PORT       GPIOH
#define ADDRESS_8_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_9_PIN             GPIO_Pin_10
#define ADDRESS_9_GPIO_PORT       GPIOH
#define ADDRESS_9_GPIO_CLK        RCC_AHB1Periph_GPIOH

#define ADDRESS_10_PIN            GPIO_Pin_10
#define ADDRESS_10_GPIO_PORT      GPIOH
#define ADDRESS_10_GPIO_CLK       RCC_AHB1Periph_GPIOH

#define ADDRESS_11_PIN            GPIO_Pin_10
#define ADDRESS_11_GPIO_PORT      GPIOH
#define ADDRESS_11_GPIO_CLK       RCC_AHB1Periph_GPIOH

#define DATA_0_PIN                GPIO_Pin_5
#define DATA_0_GPIO_PORT          GPIOA
#define DATA_0_GPIO_CLK           RCC_AHB1Periph_GPIOA

#define DATA_1_PIN                GPIO_Pin_10
#define DATA_1_GPIO_PORT          GPIOH
#define DATA_1_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_2_PIN                GPIO_Pin_10
#define DATA_2_GPIO_PORT          GPIOH
#define DATA_2_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_3_PIN                GPIO_Pin_10
#define DATA_3_GPIO_PORT          GPIOH
#define DATA_3_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_4_PIN                GPIO_Pin_10
#define DATA_4_GPIO_PORT          GPIOH
#define DATA_4_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_5_PIN                GPIO_Pin_10
#define DATA_5_GPIO_PORT          GPIOH
#define DATA_5_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_6_PIN                GPIO_Pin_10
#define DATA_6_GPIO_PORT          GPIOH
#define DATA_6_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_7_PIN                GPIO_Pin_10
#define DATA_7_GPIO_PORT          GPIOH
#define DATA_7_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_8_PIN                GPIO_Pin_10
#define DATA_8_GPIO_PORT          GPIOH
#define DATA_8_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_9_PIN                GPIO_Pin_10
#define DATA_9_GPIO_PORT          GPIOH
#define DATA_9_GPIO_CLK           RCC_AHB1Periph_GPIOH

#define DATA_10_PIN               GPIO_Pin_10
#define DATA_10_GPIO_PORT         GPIOH
#define DATA_10_GPIO_CLK          RCC_AHB1Periph_GPIOH

#define DATA_11_PIN               GPIO_Pin_10
#define DATA_11_GPIO_PORT         GPIOH
#define DATA_11_GPIO_CLK          RCC_AHB1Periph_GPIOH

#define WRITE_ENABLE_PIN          GPIO_Pin_4
#define WRITE_ENABLE_GPIO_PORT    GPIOA
#define WRITE_ENABLE_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define READ_ENABLE_PIN           GPIO_Pin_2
#define READ_ENABLE_GPIO_PORT     GPIOA
#define READ_ENABLE_GPIO_CLK      RCC_AHB1Periph_GPIOA

/******************************************************/

//中断定义
#define DATAPORT_INT_GPIO_PORT                GPIOD
#define DATAPORT_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define DATAPORT_INT_SYSCFG_CLK               RCC_APB2Periph_SYSCFG
#define DATAPORT_INT_GPIO_PIN                 GPIO_Pin_2
#define DATAPORT_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOD
#define DATAPORT_INT_EXTI_PINSOURCE           EXTI_PinSource2
#define DATAPORT_INT_EXTI_LINE                EXTI_Line2
#define DATAPORT_INT_EXTI_IRQ                 EXTI2_IRQn

#define DATAPORT_IRQHandler                   EXTI2_IRQHandler

/****************************************************/

void BSP_DATAPORT_Init(void);
void BSP_DATAPORT_DATA_ModeSet(uint16_t u16Mode);
void BSP_DATAPORT_ADDRESS_ModeSet(uint16_t u16Mode);
void BSP_DATAPORT_Write(uint32_t u32Address, uint32_t u32Data);
void BSP_DATAPORT_Read(uint32_t u32Address, uint32_t *u32pData);
void BSP_DATAPORT_CLK_Enable(DataPort *tpDataPort, uint32_t u32Width);
void BSP_DATAPORT_ModeSet(DataPort *tpDataPort, uint16_t u16Mode, uint32_t u32Width);
void BSP_DATAPORT_Write_Read_Init(void);
void BSP_DATAPORT_Read_Enable(void);
void BSP_DATAPORT_Write_Enable(void);
void BSP_DATAPORT_Read_Disable(void);
void BSP_DATAPORT_Write_Disable(void);

void BSP_DATAPORT_INT_NVIC_Enable(void);
void BSP_DATAPORT_INT_NVIC_Disable(void);
void BSP_DATAPORT_INT_Init(void);

#endif
