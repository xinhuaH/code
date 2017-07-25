#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define CONFIG_MESSAGE_RESPOND

#define DATA_COLLECTION_BEGIN    1
#define DATA_COLLECTION_CONTINUE 2
#define DATA_COLLECTION_END      3

#define MESSAGE_TO_HOST          1
#define MESSAGE_TO_DEVICE        2

#define MESSAGE_TRY_DELAY_MS     10

//待处理
#define MESSAGE_NEED_HANDLE      3
//需要应答
#define MESSAGE_NEED_RESPOND     2
//应答完毕
#define MESSAGE_RESPONDED        1
//处理完毕
#define MESSAGE_FREE             0

//MESSAGE最大长度
#define MAX_MESSAGE_LENGTH    0x20
//最大MESSAGE数
#define MAX_MESSAGE_BUFFER    0x10

#define PROTOCOL_DATA_END     0x16
#define PROTOCOL_DATA_BEGIN   0x68

typedef struct msgDataStruct
{
    unsigned char ucMsgHandled;
    unsigned int  uiDataLength;
    unsigned char uctMsgBuf[MAX_MESSAGE_LENGTH];
}MessageData;

typedef struct msgTransferStruct
{
    unsigned char usMsgNum;
    MessageData   tMsgBuf[MAX_MESSAGE_BUFFER];
}MessageTransfer;

typedef struct msgControlerStruct
{
    xSemaphoreHandle tMsgSemaphore;
    xSemaphoreHandle tMsgMutex;
    TaskHandle_t tHandleDataCollect;
    TaskHandle_t tHandleDataSend;
}MessageControler;

void messageDeliverInit(void);
void dataSendTaskCreate(void);
void dataCollectTaskNotify(void);
void dataCollectTaskCreate(void);
void messageDeliverMutexCreate(void);
void messageDeliverSignalCreate(void);
void dataSendTask(void *pvParameters);
void dataCollectTask(void *pvParameters);
void sendMessage(unsigned char ucDestination);
void wakeupMessageDeliver(MessageControler *tpControler);
void addMsgDataToBuffer(unsigned char ucDestination, MessageData *tpData);
signed char getMsgDataFromBuffer(unsigned char ucDestination, unsigned int uiBufCount, MessageData *tpData);
signed char getMsgStatusFromBuffer(unsigned char ucMsgType);

#endif
