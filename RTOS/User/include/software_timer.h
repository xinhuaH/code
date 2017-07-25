#ifndef _SOFTWARE_TIMER_H_
#define _SOFTWARE_TIMER_H_

#include "FreeRTOS.h"
#include "timers.h"

#define TIMER_WAIT_TIME       2
#define START_TIMER_WAIT_TIME 2

typedef TimerHandle_t SOFTTimerHandle;

typedef void (*TimerCallbackFun)(SOFTTimerHandle tTimerHandle);

SOFTTimerHandle createTimerFunction(TimerCallbackFun tFun, unsigned int uiDelayMs);
SOFTTimerHandle addTimerFunction(TimerCallbackFun tFun, unsigned int uiDelayMs);
signed char startTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs);
signed char stopTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs);
signed char resetTimerFunction(SOFTTimerHandle tHandle, unsigned int uiWaitMs);
void delTimerFunction(SOFTTimerHandle tTimerHandle, unsigned int uiWaitMs);

#endif
