#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "stdio.h"
#include "string.h"
#include "portmacro.h"
#include "cm_backtrace.h"

char ucWriteBuffer[500];
TaskStack_t tStack[12];

void viewTaskStatus(void)
{
    printf("=================================================\r\n");
    printf("任务名                               任务状态   优先级    剩余栈   任务序号\r\n");
    vTaskList(&ucWriteBuffer[0]);
    printf("%s\r\n", ucWriteBuffer);

    printf("\r\n任务名                              运行计数             使用率\r\n");
    vTaskGetRunTimeStats(&ucWriteBuffer[0]);
    printf("%s\r\n", ucWriteBuffer);
}

void viewTaskStack(void)
{
   UBaseType_t tSize  = 0;
   UBaseType_t tCount = 0;

   vTaskSuspendAll();

   vTaskStatus(&(tStack[0]), &tSize);

   for(tCount = 0; tCount < tSize; tCount++)
   {
        printf("\nprocess:%s,addr:0x%x\n",tStack[tCount].cTaskName, tStack[tCount].tStackTop);

        printCallStack((uint32_t)tStack[tCount].tStackTop);
   }

   ( void ) xTaskResumeAll();
}
