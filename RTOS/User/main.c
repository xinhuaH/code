#include "stm32f4xx.h"

#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_sdram.h"
#include "message_deliver.h"
#include "device_control.h"
#include "bsp_data_port.h"
#include "device_command.h"

#include "cm_backtrace.h"

/* 板级支持包头文件 */
#include "bsp_led.h"

/* FreeRTOS 头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

static void BSP_Init(void);

int main(void)
{
	/* 初始化USART 配置模式为 115200 8-N-1 */
    /* 第一步：开发板硬件初始化 */
	BSP_Init();

    cmBacktraceInit("DrvBoard", "V1.1", "V1.1");

    deviceControlerInit();
    messageDeliverInit();
    deviceCommandInit();
	
	/* 第三步：启动FreeRTOS，开始多任务调度，启动成功则不返回 */
	vTaskStartScheduler();

    printf("memory error,please increase the heap size\n");

	while(1);
}


/*=================================== 函数实现 ==================================*/
/*********************************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级初始化，所有的跟开发板硬件相关的初始化都可以放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
static void BSP_Init(void)
{
    BSP_USART_Init();
    BSP_TIMER_Init();
    BSP_DATAPORT_Init();
}


/*********************************************END OF FILE**********************/
