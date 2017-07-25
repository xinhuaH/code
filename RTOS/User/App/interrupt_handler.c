#include "interrupt_handler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#include "stdio.h"
#include "string.h"

#include "device_command.h"

InterruptHandler tInterruptHandler;

void interruptHandlerSignalCreate(void)
{
    do
    {
        tInterruptHandler.tInterruptSemaphore = xSemaphoreCreateCounting(INTERRUPT_HANDLER_NUM, 0);
        printf("create interrupt handler semaphore:0x%x\n",tInterruptHandler.tInterruptSemaphore);
    }while(tInterruptHandler.tInterruptSemaphore == NULL);
}

void wakeupInterruptHandler(InterruptHandler *tpHandler)
{
    if(tpHandler == NULL)
    {
        return;
    }

    if(tpHandler->tInterruptSemaphore != NULL)
    {
        xSemaphoreGive(tpHandler->tInterruptSemaphore);
    }
    else
    {
        printf("error,wakeupInterruptHandler,tInterruptSemaphore is NULL\n");
    }
}

void wakeupInterruptHandlerFromISR(InterruptHandler *tpHandler)
{
    BaseType_t tHigherPriorityTaskWoken = pdFALSE;

    if(tpHandler == NULL)
    {
        return;
    }

    if(tpHandler->tInterruptSemaphore != NULL)
    {
        xSemaphoreGiveFromISR(tpHandler->tInterruptSemaphore, &tHigherPriorityTaskWoken);
    }
    else
    {
        printf("error,wakeupInterruptHandler,tInterruptSemaphore is NULL\n");
    }

    /* 如果 tHigherPriorityTaskWoken 表达式为真，需要执行一次上下文切换*/  
    portYIELD_FROM_ISR(tHigherPriorityTaskWoken);
}

void interruptTask(void *pvParameters)
{
    printf("interruptTask create success\n");

    while(1)
    {
        if(xSemaphoreTake(tInterruptHandler.tInterruptSemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("interruptTask wake up\n");
            runTaskOfHandler();
        }
        else
        {
            printf("acquire interruptTask semaphore timeout\n");
        }
    }
}

void pushTaskFromISR(unsigned int uiTaskNum, unsigned char *ucpBuf, unsigned int uiBufSize)
{
    InterruptHandlerFun tFun = NULL;

    findTashFun(uiTaskNum, &tFun);

    if(tFun != NULL)
    {
        pushTaskToHandler(tFun, ucpBuf, uiBufSize);
    }

    wakeupInterruptHandlerFromISR(&tInterruptHandler);
}

void findTashFun(unsigned int uiTaskNum, InterruptHandlerFun *tpFun)
{
    if(tpFun == NULL)
    {
        printf("tFun is null in findTashFun\n");
        return;
    }

    switch(uiTaskNum)
    {
        case 0:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    *tpFun = stopAllMotors;
}

void pushTaskToHandler(InterruptHandlerFun tFun, unsigned char *ucpBuf, unsigned int uiBufSize)
{
    unsigned int uiCount = 0;

    if(ucpBuf == NULL)
    {
        printf("ucpBuf is null in pushTaskToHandler\n");
        return;
    }

    for(uiCount = 0; uiCount < INTERRUPT_HANDLER_NUM; uiCount++)
    {
        if(tInterruptHandler.tFun[uiCount] == NULL)
        {
            memcpy(tInterruptHandler.tData[uiCount].uctBuf, ucpBuf, uiBufSize);
            tInterruptHandler.tData[uiCount].uiBufSize = uiBufSize;
            tInterruptHandler.tFun[uiCount] = tFun;
        }
    }
}

void runTaskOfHandler(void)
{
    unsigned int uiCount = 0;

    for(uiCount = 0; uiCount < INTERRUPT_HANDLER_NUM; uiCount++)
    {
        if(tInterruptHandler.tFun[uiCount] != NULL)
        {
            tInterruptHandler.tFun[uiCount](&(tInterruptHandler.tData[uiCount].uctBuf[0]), tInterruptHandler.tData[uiCount].uiBufSize);
            tInterruptHandler.tFun[uiCount] = NULL;
        }
    }
}

void interruptHandlerTaskCreate(void)
{
	xTaskCreate(interruptTask, "interrupt task", 256, NULL, 0,
                &(tInterruptHandler.tHandleInterruptHandler));
}

void interruptHandlerDataInit(void)
{
    unsigned int uiCount = 0;

    for(uiCount = 0; uiCount < INTERRUPT_HANDLER_NUM; uiCount++)
    {
        tInterruptHandler.tFun[uiCount] = NULL;
    }
}

void interruptHandlerInit(void)
{
    interruptHandlerDataInit();
    interruptHandlerSignalCreate();
    interruptHandlerTaskCreate();
}
