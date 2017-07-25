#include "FreeRTOS.h"
#include "timers.h"
#include "projdefs.h"
#include "public.h"
#include <stdio.h>

#include "software_timer.h"

unsigned int uiTimerFunCount = 0;

SOFTTimerHandle createTimerFunction(TimerCallbackFun tFun, unsigned int uiDelayMs)
{
    SOFTTimerHandle tHandle = NULL;

    if(tFun == NULL)
    {
        printf("tFun is NULL in createTimerFunction\n");
        return NULL;
    }

    tHandle = addTimerFunction(tFun, uiDelayMs);

    if(tHandle != NULL)
    {
        if(startTimerFunction(tHandle, TIMER_WAIT_TIME) == BK_SUCCESS)
        {
            printf("createTimerFunction success,tHandle:0x%x\n",tHandle);
        }
        else
        {
            delTimerFunction(tHandle, TIMER_WAIT_TIME);
            tHandle = NULL;
        }
    }
    else
    {
        printf("addTimerFunction failed\n");
    }

    return tHandle;
}

signed char startTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs)
{
    if(tHandle == NULL)
    {
        printf("tHandle is NULL in startTimerFunction\n");
        return BK_FAILED;
    }

    if(xTimerStart(tHandle, pdMS_TO_TICKS(uiWaitMs)) == pdPASS)
    {
        return BK_SUCCESS;
    }
    else
    {
        printf("startTimerFunction failed,handle:0x%x\n",tHandle);
    }

    return BK_FAILED;
}

signed char stopTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs)
{
    if(tHandle == NULL)
    {
        printf("tHandle is NULL in stopTimerFunction\n");
        return BK_FAILED;
    }

    if(xTimerStop(tHandle, pdMS_TO_TICKS(uiWaitMs)) == pdPASS)
    {
        return BK_SUCCESS;
    }
    else
    {
        printf("stopTimerFunction failed,handle:0x%x\n",tHandle);
    }

    return BK_FAILED;
}

signed char resetTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs)
{
    if(tHandle == NULL)
    {
        printf("tHandle is NULL in resetTimerFunction\n");
        return BK_FAILED;
    }

    if(xTimerReset(tHandle, pdMS_TO_TICKS(uiWaitMs)) == pdPASS)
    {
        return BK_SUCCESS;
    }
    else
    {
        printf("resetTimerFunction failed,handle:0x%x\n",tHandle);
    }

    return BK_FAILED;
}

void delTimerFunction(SOFTTimerHandle tTimerHandle, unsigned int uiWaitMs)
{
    if(tTimerHandle == NULL)
    {
        printf("tHandle is NULL in delTimerFunction\n");
        return;
    }

    if(xTimerDelete(tTimerHandle, pdMS_TO_TICKS(uiWaitMs)) != pdPASS)
    {
        printf("delTimerFunction error,handle:0x%x\n",tTimerHandle);
    }
    else
    {
        if(uiTimerFunCount > 0)
        {
            uiTimerFunCount--;
        }
    }
}

SOFTTimerHandle addTimerFunction(TimerCallbackFun tFun, unsigned int uiDelayMs)
{
    SOFTTimerHandle tHandle = NULL;

    if(tFun == NULL)
    {
        printf("tFun is null in createTimerFunction\n");
        return NULL;
    }
    else
    {
        tHandle = xTimerCreate("Timer", pdMS_TO_TICKS(uiDelayMs), pdFALSE, (void *)uiTimerFunCount, tFun);

        if(tHandle != NULL)
        {
            uiTimerFunCount++;
        }
        else
        {
            printf("create timer fail\n");
        }
    }

    return tHandle;
}
