#ifndef _DEVICE_CONTROL_H_
#define _DEVICE_CONTROL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef int (*DeviceControlerFun)(unsigned char *ucpBuf, unsigned int uiBufSize);

typedef struct controlStruct
{
    unsigned char ucDataBuf[30];
    unsigned int  uiBufSize;
    DeviceControlerFun tFun;
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
void setControlerStatus(DeviceControler *tpControler, char cStatus, DeviceControlerFun tControlerFun);
void wakeupDeviceControler(DeviceControler tControler);
signed char taskAssignment(DeviceControler tControler);
signed char getControlerStatus(DeviceControler tControler);

int acquireSoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize);
int acquireFPGASoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize);
int resetAllValues(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlOneValue(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlAllValues(unsigned char *ucpBuf, unsigned int uiBufSize);
int initLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
int resetLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlLevelMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlLevelMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlLevelMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlNeedleSway(unsigned char *ucpBuf, unsigned int uiBufSize);
int initVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
int resetVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlVerticalMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlVerticalMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlVerticalMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlVerticalPuncture(unsigned char *ucpBuf, unsigned int uiBufSize);
int controlSamplingSelfTest(unsigned char *ucpBuf, unsigned int uiBufSize);
int stopAllMotors(unsigned char *ucpBuf, unsigned int uiBufSize);

#endif
