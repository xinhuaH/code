#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portmacro.h"
#include "projdefs.h"
#include "stdio.h"
#include "string.h"

#include "public.h"
#include "bsp_usart.h"
#include "protocol_analysis.h"
#include "message_deliver.h"

MessageControler tMsgControler;
MessageTransfer  tHostMsgTransfer   = { 0, {0, 0, {0}}};
MessageTransfer  tDeviceMsgTransfer = { 0, {0, 0, {0}}};


void dataCollectTask(void *pvParameters)
{
    printf("dataCollectTask create success\n");

    while(1)
    {
        /*�ȴ�֪ͨ������������������pdFALSE��ʾ�����˳�ǰ��������ֵ֪ͨ*/
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        printf("revieve notify\n");
        dataHandled();
        cmdHandle(5);
    }
}

void dataSendTask(void *pvParameters)
{
    printf("dataSendTask create success\n");

    unsigned char tData[6] = {0x31, 0x20, 0x32, 0x20, 0x33, 0x20};

    sendOneMessageToHost(&tData[0], 6);

    while(1)
    {
        if(xSemaphoreTake(tMsgControler.tMsgSemaphore, portMAX_DELAY) == pdTRUE)
        {
            printf("dataSendTask wake up\n");

            sendMessage(MESSAGE_TO_HOST);
            sendMessage(MESSAGE_TO_DEVICE);
        }
        else
        {
            printf("acquire tMsgSemaphore timeout\n");
        }
    }
}

void dataCollectTaskNotify(void)
{
    /* tHigherPriorityTaskWoken���뱻��ʼ��ΪpdFALSE���������vTaskNotifyGiveFromISR()����tHandleDataCollection���������״̬��
     * ����tHandleDataCollection��������ȼ����ڵ�ǰ��������״̬��������tHigherPriorityTaskWoken�����Զ�������ΪpdTRUE��*/

	static portBASE_TYPE tHigherPriorityTaskWoken;

    /*��һ��������֪ͨ��tHandleDataCollection�Ǹ�����ľ����*/

    vTaskNotifyGiveFromISR(tMsgControler.tHandleDataCollect, &tHigherPriorityTaskWoken);

    /* ���tHigherPriorityTaskWokenΪpdTRUE����ǿ���������л���������ʵ��ȡ������ֲ�㣬���ܻ����portEND_SWITCHING_ISR */

    portYIELD_FROM_ISR(tHigherPriorityTaskWoken);
}

void dataCollectTaskCreate(void)
{
	xTaskCreate(dataCollectTask,        /* �������� */
	            "data collect",         /* ���������ַ�����ʽ��������� */
	             256,                   /* ջ��С����256���ֽ� */
	             NULL,                  /* �����β� */
	             0,                     /* ���ȼ�����ֵԽ�����ȼ�Խ�� */
	             &(tMsgControler.tHandleDataCollect));  /* ������ */
}

void dataSendTaskCreate(void)
{
	xTaskCreate(dataSendTask,           /* �������� */
	            "data send",            /* ���������ַ�����ʽ��������� */
	             256,                   /* ջ��С����256���ֽ� */
	             NULL,                  /* �����β� */
	             0,                     /* ���ȼ�����ֵԽ�����ȼ�Խ�� */
	             &(tMsgControler.tHandleDataSend));    /* ������ */
}

void messageDeliverSignalCreate(void)
{
    do
    {
        tMsgControler.tMsgSemaphore = xSemaphoreCreateCounting( MAX_MESSAGE_BUFFER, 0 );
        printf("create message deliver tMsgSemaphore:0x%x\n",tMsgControler.tMsgSemaphore);
    }while(tMsgControler.tMsgSemaphore == NULL);
}

void messageDeliverMutexCreate(void)
{
    do
    {
        tMsgControler.tMsgMutex = xSemaphoreCreateMutex();
        printf("create message deliver tMsgMutex:0x%x\n",tMsgControler.tMsgMutex);
    }while(tMsgControler.tMsgMutex == NULL);
}

void wakeupMessageDeliver(MessageControler *tpControler)
{
    if(tpControler == NULL)
    {
        return;
    }

    if(tpControler->tMsgSemaphore != NULL)
    {
        xSemaphoreGive(tpControler->tMsgSemaphore);
    }
    else
    {
        printf("error,wakeupMessageDeliver,tMsgSemaphore is NULL\n");
    }
}

void addMsgDataToBuffer(unsigned char ucDestination, MessageData *tpData)
{
    unsigned int uiDataCount  = 0;
    signed char  scTryCount   = 5;
    signed char  scRet = BK_SUCCESS;
    MessageTransfer *tpMsg = NULL;

    if(tpData == NULL)
    {
        printf("tpData is NULL,addMsgDataToBuffer\n");
        return;
    }

    if(tpData->uiDataLength > MAX_MESSAGE_LENGTH)
    {
        printf("warning,tpData->uiDataLength is too large:0x%x\n", tpData->uiDataLength);
        tpData->uiDataLength = MAX_MESSAGE_LENGTH;
    }

    if(ucDestination == MESSAGE_TO_HOST)
    {
        tpMsg = &tHostMsgTransfer;
    }
    else
    {
        tpMsg = &tDeviceMsgTransfer;
    }

    do
    {
        xSemaphoreTake(tMsgControler.tMsgMutex, portMAX_DELAY);

        if(tpMsg->tMsgBuf[tpMsg->usMsgNum].ucMsgHandled == MESSAGE_FREE)
        {
            for(uiDataCount = 0; uiDataCount < tpData->uiDataLength; uiDataCount++)
            {
                tpMsg->tMsgBuf[tpMsg->usMsgNum].uctMsgBuf[uiDataCount] = tpData->uctMsgBuf[uiDataCount];
            }

            tpMsg->tMsgBuf[tpMsg->usMsgNum].uiDataLength = uiDataCount;
            tpMsg->tMsgBuf[tpMsg->usMsgNum].ucMsgHandled = MESSAGE_NEED_HANDLE;
            tpMsg->usMsgNum = (tpMsg->usMsgNum + 1) % MAX_MESSAGE_BUFFER;

            scTryCount = 0;
            scRet = BK_SUCCESS;
        }
        else
        {
            printf("MsgTransfer:%d is full,try again\n",ucDestination);
            scTryCount--;
        }

        xSemaphoreGive(tMsgControler.tMsgMutex);

        if(scTryCount > 0)
        {
            scRet = BK_FAILED;
            vTaskDelay( MESSAGE_TRY_DELAY_MS );
        }
    }while(scTryCount > 0);

    if(scRet == BK_SUCCESS)
    {
        printf("addMsgDataToBuffer success\n");
        wakeupMessageDeliver(&tMsgControler);
    }
}

signed char getMsgDataFromBuffer(unsigned char ucDestination, unsigned int uiBufCount, MessageData *tpData)
{
    unsigned int uiDataCount   = 0;
    signed char  scRet = BK_FAILED;
    MessageTransfer *tpMsg = NULL;

    if(tpData == NULL)
    {
        printf("tpData is NULL,getHostMsgDataFromBuffer\n");
        return BK_FAILED;
    }
    if(uiBufCount >= MAX_MESSAGE_BUFFER)
    {
        printf("buffer count to get message is too large\n");
        return BK_FAILED;
    }

    if(ucDestination == MESSAGE_TO_HOST)
    {
        tpMsg = &tHostMsgTransfer;
    }
    else
    {
        tpMsg = &tDeviceMsgTransfer;
    }

    xSemaphoreTake(tMsgControler.tMsgMutex, portMAX_DELAY);

    if(tpMsg->tMsgBuf[uiBufCount].ucMsgHandled == MESSAGE_NEED_HANDLE)
    {
        for(uiDataCount = 0; uiDataCount < tpMsg->tMsgBuf[uiBufCount].uiDataLength; uiDataCount++)
        {
            tpData->uctMsgBuf[uiDataCount] = tpMsg->tMsgBuf[uiBufCount].uctMsgBuf[uiDataCount];
        }

        tpData->uiDataLength = tpMsg->tMsgBuf[uiBufCount].uiDataLength;

        #ifdef CONFIG_MESSAGE_RESPOND
            tpMsg->tMsgBuf[uiBufCount].ucMsgHandled = MESSAGE_NEED_RESPOND;
        #else
            tpMsg->tMsgBuf[uiBufCount].ucMsgHandled = MESSAGE_FREE;
        #endif

        scRet = BK_SUCCESS;
    }

    xSemaphoreGive(tMsgControler.tMsgMutex);

    return scRet;
}

signed char getMsgStatusFromBuffer(unsigned char ucMsgType)
{
    unsigned int uiMsgCount = 0;
    MessageTransfer *tpMsg = NULL;

    if(ucMsgType == MESSAGE_TO_HOST)
    {
        tpMsg = &tHostMsgTransfer;
    }
    else
    {
        tpMsg = &tDeviceMsgTransfer;
    }

    xSemaphoreTake(tMsgControler.tMsgMutex, portMAX_DELAY);

    for(uiMsgCount = 0; uiMsgCount < MAX_MESSAGE_BUFFER; uiMsgCount++)
    {
        if(tpMsg->tMsgBuf[uiMsgCount].ucMsgHandled == MESSAGE_NEED_RESPOND)
        {
            break;
        }
    }

    xSemaphoreGive(tMsgControler.tMsgMutex);

    if(uiMsgCount < MAX_MESSAGE_BUFFER)
    {
        printf("MESSAGE_NEED_RESPOND,NUM:0x%x\n",uiMsgCount);
        return MESSAGE_NEED_RESPOND;
    }
    else
    {
        return MESSAGE_RESPONDED;
    }
}
void sendMessage(unsigned char ucDestination)
{
    unsigned int uiMsgCount = 0;
    MessageData  tMsgData;
    UartSendFun  uartSend = NULL;
    
#ifdef CONFIG_MESSAGE_RESPOND
    if(getMsgStatusFromBuffer(ucDestination) == MESSAGE_NEED_RESPOND)
    {
        return;
    }
#endif

    if(ucDestination == MESSAGE_TO_HOST)
    {
        uartSend = BSP_USART_Send;
    }
    else
    {
        uartSend = BSP_USART_Send;
    }

    for(uiMsgCount = 0; uiMsgCount < MAX_MESSAGE_BUFFER; uiMsgCount++)
    {
        if(getMsgDataFromBuffer(ucDestination, uiMsgCount, &tMsgData) == BK_SUCCESS)
        {
            printf("get message success\n");
            uartSend(&(tMsgData.uctMsgBuf[0]), tMsgData.uiDataLength);
        }
    }
}

void messageDeliverInit(void)
{
    dataCollectTaskCreate();
    dataSendTaskCreate();
    messageDeliverSignalCreate();
    messageDeliverMutexCreate();

    memset(&tHostMsgTransfer, 0, sizeof(tHostMsgTransfer));
}
