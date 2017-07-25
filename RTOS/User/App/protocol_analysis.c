#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portmacro.h"
#include "projdefs.h"
#include <stdio.h>
#include "device_control.h"
#include "device_command.h"
#include "protocol_analysis.h"
#include "public.h"
#include <string.h> 
#include "bsp_timer.h"

extern DeviceControler deviceControlData1;
extern DeviceControler deviceControlData2;
extern DeviceControler deviceControlData3;

static unsigned int uiDeviceCmdNum = 0;

ProtocolData  tProtocolData = { {0}, 0, 0, 0};
DeviceCmdData tDeviceCmdData[MAX_CMD_COUNT] = { 
    { 0, {0}, 0, 0},
};

char protocolBeginCheck(unsigned int uiBegin, unsigned int uiEnd)
{
    if((uiEnd - uiBegin) < PROTOCOL_DATA_CMD_BIT2)
    {
        return BK_FALSE;
    }

    if(tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_BIGIN_BIT) % MAX_DATA_RECIEVE_BUFFER] == PROTOCOL_DATA_BEGIN)
    {
        return BK_TRUE;
    }
    else
    {
        return BK_FALSE;
    }
}

signed char protocolBeginAdd(unsigned char *ucpBegin)
{
    if(ucpBegin == NULL)
    {
        return BK_FAILED;
    }
    else
    {
        ucpBegin[PROTOCOL_DATA_BIGIN_BIT] = PROTOCOL_DATA_BEGIN;
    }

    return BK_SUCCESS;
}

char protocolDataLengthCheck(unsigned int uiBegin, unsigned int uiEnd)
{
    unsigned int uiDataLength = 0;

    //uiDataLength = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT1) % MAX_DATA_RECIEVE_BUFFER] | (tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT2) % MAX_DATA_RECIEVE_BUFFER] << 8);

    uiDataLength = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT1) % MAX_DATA_RECIEVE_BUFFER];

    if((uiDataLength + PROTOCOL_DATA_LENGTH_BIT2 + 2 + uiBegin) % MAX_DATA_RECIEVE_BUFFER > uiEnd)
    {
        printf("data isn't end,uiDataLength:0x%x\n",uiDataLength);
        return BK_FALSE;
    }

    if(tProtocolData.ucDataReceive[((uiDataLength + PROTOCOL_DATA_LENGTH_BIT2 + 2 + uiBegin) % MAX_DATA_RECIEVE_BUFFER)] == PROTOCOL_DATA_END)
    {
        return BK_TRUE;
    }
    else
    {
        printf("uiBegin:%d,uiEnd:%d,uiDataLength:%d\n", uiBegin, uiEnd, uiDataLength);
    }

    return BK_FALSE;
}

signed char protocolDataLengthAdd(unsigned char *ucpLength, unsigned int uiLength)
{
    if(ucpLength == NULL)
    {
        return BK_FAILED;
    }
    else
    {
        ucpLength[PROTOCOL_DATA_LENGTH_BIT1] = uiLength & 0xFF;
        ucpLength[PROTOCOL_DATA_LENGTH_BIT2] = (uiLength & 0xFF00) >> 8;
    }

    return BK_SUCCESS;
}

char protocolDataCsCheck(unsigned int uiBegin, unsigned int uiEnd)
{
    unsigned int uiDataNum = 0;
    unsigned int uiDataCount = 0;
    unsigned char ucCs = 0;

    printf("uiBegin:%d,uiEnd:%d\n", uiBegin, uiEnd);

    printData(&(tProtocolData.ucDataReceive[uiBegin]), (uiEnd - uiBegin + 1) % MAX_DATA_RECIEVE_BUFFER);

    uiDataCount = (uiEnd - uiBegin + 1 + MAX_DATA_RECIEVE_BUFFER) % MAX_DATA_RECIEVE_BUFFER;

    for(uiDataNum = 0; uiDataNum < uiDataCount - 2; uiDataNum++)
    {
        ucCs = ucCs + tProtocolData.ucDataReceive[(uiBegin + uiDataNum) % MAX_DATA_RECIEVE_BUFFER];
    }

    if(ucCs == tProtocolData.ucDataReceive[(MAX_DATA_RECIEVE_BUFFER + uiEnd -1) % MAX_DATA_RECIEVE_BUFFER])
    {
        return BK_TRUE;
    }
    else
    {
        printf("CS:0x%x,ucCs:0x%x\n",tProtocolData.ucDataReceive[(MAX_DATA_RECIEVE_BUFFER + uiEnd -1) % MAX_DATA_RECIEVE_BUFFER],ucCs);
        return BK_FALSE;
    }
}

signed char protocolDataCsAdd(unsigned char *ucpData, unsigned int uiLength)
{
    unsigned char ucDataCs = 0;
    unsigned int uiCount   = 0;

    if(ucpData == NULL)
    {
        return BK_FAILED;
    }

    for(uiCount = 0; uiCount < uiLength; uiCount++)
    {
        ucDataCs = ucDataCs + ucpData[uiCount];
    }

    ucpData[uiLength] = ucDataCs;

    return BK_SUCCESS;
}

signed char protocolDataEndAdd(unsigned char *ucpData, unsigned int uiEndPoint)
{
    if(ucpData == NULL)
    {
        return BK_FAILED;
    }

    ucpData[uiEndPoint] = PROTOCOL_DATA_END;

    return BK_SUCCESS;
}

void protocolDataToCmd(unsigned int uiBegin, unsigned int uiEnd)
{
    unsigned int uiDataNum = 0;
    unsigned int uiDataCount = 0;

    uiDataCount = (uiEnd + MAX_DATA_RECIEVE_BUFFER - (uiBegin + PROTOCOL_DATA_LENGTH_BIT2) - 2/*CS,END*/ -2/*CMD*/) % MAX_DATA_RECIEVE_BUFFER;

    if(uiDataCount > MAX_CMD_DATA_BUFFER_SIZE)
    {
        printf("tDeviceCmdData overflow\n");
        return;
    }

    if(tDeviceCmdData[uiDeviceCmdNum].ucCmdNeedHandle == BK_TRUE)
    {
        printf("uiDeviceCmdNum overflow\n");
        return;
    }

    for(uiDataNum = 0; uiDataNum < uiDataCount; uiDataNum++)
    {
        tDeviceCmdData[uiDeviceCmdNum].ucCmdDataBuf[uiDataNum] = tProtocolData.ucDataReceive[(uiBegin + uiDataNum + PROTOCOL_DATA_BUFFER_BIT) % MAX_DATA_RECIEVE_BUFFER];
    }

    tDeviceCmdData[uiDeviceCmdNum].uiCmdDataLength = uiDataCount;
    tDeviceCmdData[uiDeviceCmdNum].uiCmdControl = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_CMD_BIT1) % MAX_DATA_RECIEVE_BUFFER] | (tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_CMD_BIT2) % MAX_DATA_RECIEVE_BUFFER] << 8);
    tDeviceCmdData[uiDeviceCmdNum].ucCmdNeedHandle = BK_TRUE;

    printf("CmdLength:%d,CmdControl:0x%x\n",uiDataCount,tDeviceCmdData[uiDeviceCmdNum].uiCmdControl);
}

unsigned char protocolDataForForward(unsigned int uiBegin, unsigned int uiEnd)
{
    unsigned char ucDataControl = 0;

    ucDataControl = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_CONTROL_BIT) % MAX_DATA_RECIEVE_BUFFER];

    if(ucDataControl == PROTOCLO_DATA_FORWARD)
    {
        return BK_TRUE;
    }
    else
    {
        return BK_FALSE;
    }
}

void dataHandledCountUpdate(unsigned int uiBegin, unsigned int uiEnd)
{
    unsigned int uiDataLength = 0;

    //uiDataLength = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT1) % MAX_DATA_RECIEVE_BUFFER] | (tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT2) % MAX_DATA_RECIEVE_BUFFER] << 8);

    uiDataLength = tProtocolData.ucDataReceive[(uiBegin + PROTOCOL_DATA_LENGTH_BIT1) % MAX_DATA_RECIEVE_BUFFER];

    tProtocolData.uiDataHandledCount = (tProtocolData.uiDataHandledCount + PROTOCOL_DATA_LENGTH_BIT2 + uiDataLength + 2 + 1 /*Begin*/) % MAX_DATA_RECIEVE_BUFFER;

    if(tProtocolData.uiDataHandledCount > uiEnd + 1)
    {
        printf("error,dataHandledCountUpdate,HandledCount:%d,uiEnd:%d\n",tProtocolData.uiDataHandledCount, uiEnd);
        tProtocolData.uiDataHandledCount = uiEnd;
    }
}

void dataRecievedRefreshed(void)
{
    static unsigned int uiLastTime = 0;
    unsigned int uiCurrentSystemTime = 0;

    uiCurrentSystemTime = getSystemTime();

    if((uiCurrentSystemTime - uiLastTime + 0xFFFFFFFF ) % 0xFFFFFFFF > PROTOCOL_DATA_REFRESH_FREQUENCY)
    {
        dataRecievedDataReset();
    }

    uiLastTime = uiCurrentSystemTime;
}


void dataRecievedCountUpdate(void)
{
    tProtocolData.uiDataLastRecievedCount = (tProtocolData.uiDataRecievedCount + MAX_DATA_RECIEVE_BUFFER - 1) % MAX_DATA_RECIEVE_BUFFER;
}

void dataRecievedDataReset(void)
{
    tProtocolData.uiDataLastRecievedCount = 0;
    tProtocolData.uiDataRecievedCount = 0;
    tProtocolData.uiDataHandledCount = 0;
}

void dataRecieved(unsigned char ucData)
{
    tProtocolData.ucDataReceive[tProtocolData.uiDataRecievedCount] = ucData;
    tProtocolData.uiDataRecievedCount = (tProtocolData.uiDataRecievedCount + 1) % MAX_DATA_RECIEVE_BUFFER;

    //printf("R:%d,H:%d,D:0x%x\n", tProtocolData.uiDataRecievedCount, tProtocolData.uiDataHandledCount, ucData);
}

unsigned char dataCalculateCS(unsigned char *ucBuf, unsigned int uiSize)
{
    unsigned char ucCS = 0;
    unsigned int uiNum = 0;

    for(uiNum = 0; uiNum < uiSize; uiNum++)
    {
        if(uiNum >= MAX_CMD_DATA_BUFFER_SIZE)
        {
            printf("data overflow in CS calculate\n");
            return 0;
        }

        ucCS += ucBuf[uiNum];
    }

    return ucCS;
}

void printData(unsigned char *ucBuf, unsigned int uiSize)
{
    unsigned int uiNum = 0;

    printf("\n");

    for(uiNum = 0; uiNum < uiSize; uiNum++)
    {
        printf("0x%x ",ucBuf[uiNum]);
    }

    printf("\n");
}

void cmdHandle(unsigned char ucTryCount)
{
    unsigned int uiCmdNum = 0;
    unsigned char ucCount = 0;
    DeviceControler tControler = {{0}, 0, NULL, NULL, NULL, NULL};

    for(uiCmdNum = 0; uiCmdNum < MAX_CMD_COUNT; uiCmdNum++)
    {
        if(tDeviceCmdData[uiCmdNum].ucCmdNeedHandle == BK_TRUE)
        {
            tControler.tFun = commandToFunction(tDeviceCmdData[uiCmdNum].uiCmdControl);

            if(tControler.tFun != NULL)
            {            
                tControler.uiBufSize = tDeviceCmdData[uiCmdNum].uiCmdDataLength;
                memcpy(tControler.ucDataBuf, tDeviceCmdData[uiCmdNum].ucCmdDataBuf, tControler.uiBufSize);

                do
                {
                    if(taskAssignment(tControler) == BK_SUCCESS)
                    {
                        printf("handle cmd:0x%x success\n",tDeviceCmdData[uiCmdNum].uiCmdControl);
                        tDeviceCmdData[uiCmdNum].ucCmdNeedHandle = BK_FALSE;
                        break;
                    }
                    else
                    {
                        ucCount++;
                        if(ucCount > ucTryCount)
                        {
                            tDeviceCmdData[uiCmdNum].ucCmdNeedHandle = BK_FALSE;
                            ucCount = 0;
                            break;
                        }

                        printf("handle cmd:0x%x failed,try again\n",tDeviceCmdData[uiCmdNum].uiCmdControl);
                        vTaskDelay( 1000 );
                    }
                }while(1);
            }
            else
            {
                  printf("handle cmd:0x%x,tFun is NULL,please check\n",tDeviceCmdData[uiCmdNum].uiCmdControl);
                  tDeviceCmdData[uiCmdNum].ucCmdNeedHandle = BK_FALSE;
            }
        }
    }
}

signed char analysisOneMessage(void)
{
    if(protocolBeginCheck(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount) == BK_TRUE)
    {
        if(protocolDataLengthCheck(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount) == BK_TRUE)
        {
            if(protocolDataCsCheck(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount) == BK_TRUE)
            {
                if(protocolDataForForward(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount) == BK_TRUE)
                {
                    protocolDataToDevice(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount);
                }
                else
                {
                    protocolDataToCmd(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount);
                }
            }

            dataHandledCountUpdate(tProtocolData.uiDataHandledCount,tProtocolData.uiDataLastRecievedCount);

            return BK_SUCCESS;
        }
    }

    return BK_FAILED;
}

void sendOneMessageToHost(unsigned char *ucpMsgBuf, unsigned int uiMsgLength)
{
    MessageData tMsg;

    if(ucpMsgBuf == NULL)
    {
        return;
    }

    memcpy(&(tMsg.uctMsgBuf[PROTOCOL_DATA_CMD_BIT1]), ucpMsgBuf, uiMsgLength);

    tMsg.uiDataLength = uiMsgLength + PROTOCOL_DATA_LENGTH_BIT2 + 1/*Begin*/ + 1/*CS*/ + 1/*End*/;

    if(generateOneMessage(&tMsg) == BK_SUCCESS)
    {
        addMsgDataToBuffer(MESSAGE_TO_HOST, &tMsg);
    }
}

void protocolDataToDevice(unsigned int uiBegin, unsigned int uiEnd)
{
    MessageData tMsg;
    unsigned int uiLength = 0;
    unsigned int uiNum = 0;

    uiLength = (uiEnd - uiBegin + 1 + MAX_DATA_RECIEVE_BUFFER) % MAX_DATA_RECIEVE_BUFFER;

    for(uiNum = 0; uiNum < uiLength; uiNum++)
    {
        tMsg.uctMsgBuf[uiNum] =  tProtocolData.ucDataReceive[(uiBegin + uiNum) % MAX_DATA_RECIEVE_BUFFER];
    }

    tMsg.uiDataLength = uiLength;

    addMsgDataToBuffer(MESSAGE_TO_DEVICE, &tMsg);
}

signed char generateOneMessage(MessageData *tpMessage)
{
    if(tpMessage == NULL)
    {
        return BK_FAILED;
    }

    if(protocolBeginAdd(&(tpMessage->uctMsgBuf[0])) == BK_SUCCESS)
    {
        if(protocolDataLengthAdd(&(tpMessage->uctMsgBuf[0]), tpMessage->uiDataLength) == BK_SUCCESS)
        {
            if(protocolDataCsAdd(&(tpMessage->uctMsgBuf[0]), tpMessage->uiDataLength - 2 /*CS,End*/) == BK_SUCCESS)
            {
                if(protocolDataEndAdd(&(tpMessage->uctMsgBuf[0]), tpMessage->uiDataLength - 1) == BK_SUCCESS)
                {
                    return BK_SUCCESS;
                }
            }
        }
    }

    return BK_FAILED;
}

void dataHandled(void)
{
    while(1)
    {
        printf("analysisOneMessage\n");

        if(analysisOneMessage() != BK_SUCCESS)
        {
            break;
        }
    }
}

