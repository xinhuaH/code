#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portmacro.h"
#include "projdefs.h"
#include "stdio.h"

#include "bsp_usart.h"
#include "protocol_analysis.h"

static TaskHandle_t tHandleDataCollection = NULL;

void dataCollectionTask(void *pvParameters)
{
    printf("dataCollectionTask create success\n");

    while(1)
    {
        /*等待通知，无限期阻塞。参数pdFALSE表示函数退出前不会清零通知值*/
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        printf("revieve notify\n");
        dataHandled();
        cmdHandle(5);
    }
}

void dataCollectionTaskNotify(void)
{
    /* tHigherPriorityTaskWoken必须被初始化为pdFALSE。如果调用vTaskNotifyGiveFromISR()会解除tHandleDataCollection任务的阻塞状态，
     * 并且tHandleDataCollection任务的优先级高于当前处于运行状态的任务，则tHigherPriorityTaskWoken将会自动被设置为pdTRUE。*/

	static portBASE_TYPE tHigherPriorityTaskWoken;

    /*向一个任务发送通知，tHandleDataCollection是该任务的句柄。*/

    vTaskNotifyGiveFromISR(tHandleDataCollection, &tHigherPriorityTaskWoken);

    /* 如果tHigherPriorityTaskWoken为pdTRUE，则强制上下文切换。这个宏的实现取决于移植层，可能会调用portEND_SWITCHING_ISR */

    portYIELD_FROM_ISR(tHigherPriorityTaskWoken);
}

void dataCollectionTaskCreate(void)
{
	xTaskCreate(dataCollectionTask,        /* 任务函数名 */
	            "data collection",         /* 任务名，字符串形式，方便调试 */
	             256,                       /* 栈大小，即256个字节 */
	             NULL,                     /* 任务形参 */
	             0,                        /* 优先级，数值越大，优先级越高 */
	             &tHandleDataCollection);  /* 任务句柄 */
}
