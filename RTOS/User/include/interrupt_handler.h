#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include "public.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef unsigned char (*InterruptHandlerFun)(unsigned char *ucpBuf, unsigned int uiBufSize);

typedef struct handlerData
{
    unsigned char uctBuf[INTERRUPT_HANDLER_BUFFER_SIZE];
    unsigned int  uiBufSize;
}InterruptData;

typedef struct handlerStruct
{
    xSemaphoreHandle tInterruptSemaphore;
    InterruptHandlerFun tFun[INTERRUPT_HANDLER_NUM];
    InterruptData tData[INTERRUPT_HANDLER_NUM];
    TaskHandle_t tHandleInterruptHandler;
}InterruptHandler;

void interruptHandlerSignalCreate(void);
void wakeupInterruptHandler(InterruptHandler *tpHandler);
void wakeupInterruptHandlerFromISR(InterruptHandler *tpHandler);
void interruptTask(void *pvParameters);
void pushTaskFromISR(unsigned int uiTaskNum, unsigned char *ucpBuf, unsigned int uiBufSize);
void findTashFun(unsigned int uiTaskNum, InterruptHandlerFun *tpFun);
void pushTaskToHandler(InterruptHandlerFun tFun, unsigned char *ucpBuf, unsigned int uiBufSize);
void runTaskOfHandler(void);
void interruptHandlerTaskCreate(void);
void interruptHandlerDataInit(void);
void interruptHandlerInit(void);

#endif
