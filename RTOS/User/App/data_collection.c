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
        /*�ȴ�֪ͨ������������������pdFALSE��ʾ�����˳�ǰ��������ֵ֪ͨ*/
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        printf("revieve notify\n");
        dataHandled();
        cmdHandle(5);
    }
}

void dataCollectionTaskNotify(void)
{
    /* tHigherPriorityTaskWoken���뱻��ʼ��ΪpdFALSE���������vTaskNotifyGiveFromISR()����tHandleDataCollection���������״̬��
     * ����tHandleDataCollection��������ȼ����ڵ�ǰ��������״̬��������tHigherPriorityTaskWoken�����Զ�������ΪpdTRUE��*/

	static portBASE_TYPE tHigherPriorityTaskWoken;

    /*��һ��������֪ͨ��tHandleDataCollection�Ǹ�����ľ����*/

    vTaskNotifyGiveFromISR(tHandleDataCollection, &tHigherPriorityTaskWoken);

    /* ���tHigherPriorityTaskWokenΪpdTRUE����ǿ���������л���������ʵ��ȡ������ֲ�㣬���ܻ����portEND_SWITCHING_ISR */

    portYIELD_FROM_ISR(tHigherPriorityTaskWoken);
}

void dataCollectionTaskCreate(void)
{
	xTaskCreate(dataCollectionTask,        /* �������� */
	            "data collection",         /* ���������ַ�����ʽ��������� */
	             256,                       /* ջ��С����256���ֽ� */
	             NULL,                     /* �����β� */
	             0,                        /* ���ȼ�����ֵԽ�����ȼ�Խ�� */
	             &tHandleDataCollection);  /* ������ */
}
