#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "queue.h"
#include "semphr.h"
#include "projdefs.h"
#include "portmacro.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "public.h"
#include "device_control.h"
#include "device_command.h"
#include "bsp_data_port.h"
#include "software_timer.h"
#include "debug.h"

#include "cm_backtrace.h"

DeviceControler deviceControler1 = {{0}, 0, NULL, NULL, NULL, NULL};
DeviceControler deviceControler2 = {{0}, 0, NULL, NULL, NULL, NULL};
DeviceControler deviceControler3 = {{0}, 0, NULL, NULL, NULL, NULL};

void deviceControlerInit(void)
{
    deviceControlerSignalCreate();
    deviceControlerTaskCreate();
    deviceControlerMutexCreate();
}

void deviceControlerTaskCreate(void)
{
    xTaskCreate(deviceControlerTask1, "device controler1", 256, NULL, 1, &deviceControler1.tHandleDeviceControler);
    xTaskCreate(deviceControlerTask2, "device controler2", 256, NULL, 1, &deviceControler2.tHandleDeviceControler);
    xTaskCreate(deviceControlerTask3, "device controler3", 256, NULL, 1, &deviceControler3.tHandleDeviceControler);
}

void deviceControlerSignalCreate(void)
{
    do
    {
        deviceControler1.tTaskSemaphore = xSemaphoreCreateCounting( 2, 0 );
        printf("create tTaskSemaphore1:0x%x\n",deviceControler1.tTaskSemaphore);
    }while(deviceControler1.tTaskSemaphore == NULL);

    do
    {
        deviceControler2.tTaskSemaphore = xSemaphoreCreateCounting( 2, 0 );
        printf("create tTaskSemaphore2:0x%x\n",deviceControler2.tTaskSemaphore);
    }while(deviceControler2.tTaskSemaphore == NULL);

    do
    {
        deviceControler3.tTaskSemaphore = xSemaphoreCreateCounting( 2, 0 );
        printf("create tTaskSemaphore3:0x%x\n",deviceControler3.tTaskSemaphore);
    }while(deviceControler3.tTaskSemaphore == NULL);
}

void deviceControlerMutexCreate(void)
{
    do
    {
        deviceControler1.tTaskMutex = xSemaphoreCreateMutex();
        printf("create tTaskMutex1:0x%x\n",deviceControler1.tTaskMutex);
    }while(deviceControler1.tTaskMutex == NULL);

    do
    {
        deviceControler2.tTaskMutex = xSemaphoreCreateMutex();
        printf("create tTaskMutex2:0x%x\n",deviceControler2.tTaskMutex);
    }while(deviceControler2.tTaskMutex == NULL);

    do
    {
        deviceControler3.tTaskMutex = xSemaphoreCreateMutex();
        printf("create tTaskMutex3:0x%x\n",deviceControler3.tTaskMutex);
    }while(deviceControler3.tTaskMutex == NULL);
}

void deviceControlerTask1(void *pvParameters)
{
    printf("deviceControl1 create success\n");

    while(1)
    {
        if(xSemaphoreTake(deviceControler1.tTaskSemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("deviceControler1 wake up\n");

            if(deviceControler1.tFun != NULL)
            {
                deviceControler1.tFun(deviceControler1.ucDataBuf, deviceControler1.uiBufSize);
                setControlerStatus(&deviceControler1, BK_STOPPING, NULL);
            }
        }
        else
        {
            printf("acquire tTaskSemaphore3 timeout\n");
        }
    }
}

void deviceControlerTask2(void *pvParameters)
{
    printf("deviceControl2 create success\n");

    while(1)
    {
        if(xSemaphoreTake(deviceControler2.tTaskSemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("deviceControler2 wake up\n");

            if(deviceControler2.tFun != NULL)
            {
                deviceControler2.tFun(deviceControler2.ucDataBuf, deviceControler2.uiBufSize);
                setControlerStatus(&deviceControler2, BK_STOPPING, NULL);
            }
        }
        else
        {
            printf("acquire tTaskSemaphore3 timeout\n");
        }
    }
}

void deviceControlerTask3(void *pvParameters)
{
    printf("deviceControl3 create success\n");

    while(1)
    {
        if(xSemaphoreTake(deviceControler3.tTaskSemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("deviceControler3 wake up\n");

            if(deviceControler3.tFun != NULL)
            {
                deviceControler3.tFun(deviceControler3.ucDataBuf, deviceControler3.uiBufSize);
                setControlerStatus(&deviceControler3, BK_STOPPING, NULL);
            }
        }
        else
        {
            printf("acquire tTaskSemaphore3 timeout\n");
        }
    }
}

signed char taskAssignment(DeviceControler tControler)
{
    if(getControlerStatus(deviceControler1) == BK_STOPPING)
    {
        deviceControler1.uiBufSize = tControler.uiBufSize;
        memcpy(deviceControler1.ucDataBuf, tControler.ucDataBuf, tControler.uiBufSize);

        setControlerStatus(&deviceControler1, BK_RUNNING, tControler.tFun);

        wakeupDeviceControler(deviceControler1);

        return BK_SUCCESS;
    }
    else if(getControlerStatus(deviceControler2) == BK_STOPPING)
    {
        deviceControler2.uiBufSize = tControler.uiBufSize;
        memcpy(deviceControler2.ucDataBuf, tControler.ucDataBuf, tControler.uiBufSize);

        setControlerStatus(&deviceControler2, BK_RUNNING, tControler.tFun);
        wakeupDeviceControler(deviceControler2);

        return BK_SUCCESS;
    }
    else if(getControlerStatus(deviceControler3) == BK_STOPPING)
    {
        deviceControler3.uiBufSize = tControler.uiBufSize;
        memcpy(deviceControler3.ucDataBuf, tControler.ucDataBuf, tControler.uiBufSize);

        setControlerStatus(&deviceControler3, BK_RUNNING, tControler.tFun);
        wakeupDeviceControler(deviceControler3);

        return BK_SUCCESS;
    }
    else
    {
        printf("the controlers are all running\n");
    }

    return BK_FAILED;
}

signed char getControlerStatus(DeviceControler tControler)
{
    signed char scRet = BK_RUNNING;

    xSemaphoreTake(tControler.tTaskMutex,portMAX_DELAY);

    if(tControler.tFun == NULL)
    {
        scRet =  BK_STOPPING;
    }

    xSemaphoreGive(tControler.tTaskMutex);

    return scRet;
}

void setControlerStatus(DeviceControler *tpControler, char cStatus, DeviceCommandFun tControlerFun)
{
    if(tpControler == NULL)
    {
        printf("pointer is null,%s:%d\n",__FUNCTION__,__LINE__);
        return;
    }

    xSemaphoreTake(tpControler->tTaskMutex,portMAX_DELAY);

    if(cStatus == BK_STOPPING)
    {
        tpControler->tFun = NULL;
    }
    else if(cStatus == BK_RUNNING)
    {
        tpControler->tFun = tControlerFun;
    }
    else
    {
        printf("warning invalid cStatus,%s:%d\n",__FUNCTION__,__LINE__);
    }

    xSemaphoreGive(tpControler->tTaskMutex);
}

void wakeupDeviceControler(DeviceControler tControler)
{
    if(tControler.tTaskSemaphore != NULL)
    {
        xSemaphoreGive(tControler.tTaskSemaphore);
    }
    else
    {
        printf("error,wakeupDeviceControler,tTaskSemaphore is NULL\n");
    }
}

void timerCallbackTest(SOFTTimerHandle tTimerHandle)
{
    printf("timerCallbackTest,tTimerHandle:0x%x\n",tTimerHandle);
}
