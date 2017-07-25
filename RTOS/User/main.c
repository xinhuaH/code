#include "stm32f4xx.h"

#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_sdram.h"
#include "message_deliver.h"
#include "device_control.h"
#include "bsp_data_port.h"
#include "device_command.h"

#include "cm_backtrace.h"

/* �弶֧�ְ�ͷ�ļ� */
#include "bsp_led.h"

/* FreeRTOS ͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

static void BSP_Init(void);

int main(void)
{
	/* ��ʼ��USART ����ģʽΪ 115200 8-N-1 */
    /* ��һ����������Ӳ����ʼ�� */
	BSP_Init();

    cmBacktraceInit("DrvBoard", "V1.1", "V1.1");

    deviceControlerInit();
    messageDeliverInit();
    deviceCommandInit();
	
	/* ������������FreeRTOS����ʼ��������ȣ������ɹ��򲻷��� */
	vTaskStartScheduler();

    printf("memory error,please increase the heap size\n");

	while(1);
}


/*=================================== ����ʵ�� ==================================*/
/*********************************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶��ʼ�������еĸ�������Ӳ����صĳ�ʼ�������Է��������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void BSP_Init(void)
{
    BSP_USART_Init();
    BSP_TIMER_Init();
    BSP_DATAPORT_Init();
}


/*********************************************END OF FILE**********************/
