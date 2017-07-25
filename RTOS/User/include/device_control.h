#ifndef _DEVICE_CONTROL_H_
#define _DEVICE_CONTROL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "device_command.h"

typedef struct controlStruct
{
    unsigned char ucDataBuf[30];
    unsigned int  uiBufSize;
    DeviceCommandFun tFun;
    xSemaphoreHandle tTaskSemaphore;
    xSemaphoreHandle tTaskMutex;
    TaskHandle_t tHandleDeviceControler;
}DeviceControler;

void deviceControlerTask1(void *pvParameter);
void deviceControlerTask2(void *pvParameter);
void deviceControlerTask3(void *pvParameter);
void deviceControlerInit(void);
void deviceControlerTaskCreate(void);
void deviceControlerMutexCreate(void);
void deviceControlerSignalCreate(void);
void setControlerStatus(DeviceControler *tpControler, char cStatus, DeviceCommandFun tControlerFun);
void wakeupDeviceControler(DeviceControler tControler);
signed char taskAssignment(DeviceControler tControler);
signed char getControlerStatus(DeviceControler tControler);

#endif
