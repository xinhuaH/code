#ifndef _PROTOCOL_ANALYSIS_H_
#define _PROTOCOL_ANALYSIS_H_

#include "device_control.h"
#include "message_deliver.h"

#define MAX_CMD_COUNT                                  16
#define SIZE_OF_DATA_HEAD                              6
#define MAX_CMD_DATA_BUFFER_SIZE                       30
#define MAX_DATA_RECIEVE_BUFFER                        160

#define PROTOCOL_DATA_REFRESH_FREQUENCY                2000

#define PROTOCOL_DATA_END     0x16
#define PROTOCOL_DATA_BEGIN   0x68
#define PROTOCLO_DATA_FORWARD 0x36

#define PROTOCOL_DATA_BIGIN_BIT       0

#define PROTOCOL_DATA_CONTROL_BIT  (PROTOCOL_DATA_BIGIN_BIT + 1)

#define PROTOCOL_DATA_LENGTH_BIT1  (PROTOCOL_DATA_CONTROL_BIT + 1)
#define PROTOCOL_DATA_LENGTH_BIT2  (PROTOCOL_DATA_LENGTH_BIT1 + 1)

#define PROTOCOL_DATA_CMD_BIT1     (PROTOCOL_DATA_LENGTH_BIT2 + 1)
#define PROTOCOL_DATA_CMD_BIT2     (PROTOCOL_DATA_CMD_BIT1 + 1)

#define PROTOCOL_DATA_BUFFER_BIT   (PROTOCOL_DATA_CMD_BIT2 + 1)

typedef struct cmdStruct
{
    unsigned int  uiCmdControl;
    unsigned char ucCmdDataBuf[MAX_CMD_DATA_BUFFER_SIZE];
    unsigned int  uiCmdDataLength;
    unsigned char ucCmdNeedHandle;
}DeviceCmdData;

typedef struct dataStruct
{
    unsigned char ucDataReceive[MAX_DATA_RECIEVE_BUFFER];
    unsigned int  uiDataRecievedCount;
    unsigned int  uiDataLastRecievedCount;
    unsigned int  uiDataHandledCount;
}ProtocolData;

void dataRecieved(unsigned char ucData);
void dataHandled(void);
void dataRecievedCountUpdate(void);
void dataRecievedDataReset(void);
void cmdHandle(unsigned char ucTryCount);
void dataRecievedRefreshed(void);
char protocolBeginCheck(unsigned int uiBegin, unsigned int uiEnd);
char protocolDataLengthCheck(unsigned int uiBegin, unsigned int uiEnd);
char protocolDataCsCheck(unsigned int uiBegin, unsigned int uiEnd);
void protocolDataToCmd(unsigned int uiBegin, unsigned int uiEnd);
void dataHandledCountUpdate(unsigned int uiBegin, unsigned int uiEnd);
void printData(unsigned char *ucBuf, unsigned int uiSize);
signed char analysisOneMessage(void);
signed char protocolBeginAdd(unsigned char *ucpBegin);
signed char protocolDataLengthAdd(unsigned char *ucpLength, unsigned int uiLength);
signed char protocolDataCsAdd(unsigned char *ucpData, unsigned int uiLength);
signed char protocolDataEndAdd(unsigned char *ucpData, unsigned int uiEndPoint);
void sendOneMessageToHost(unsigned char *ucpMsgBuf, unsigned int uiMsgLength);
signed char generateOneMessage(MessageData *tpMessage);
unsigned char protocolDataForForward(unsigned int uiBegin, unsigned int uiEnd);
void protocolDataToDevice(unsigned int uiBegin, unsigned int uiEnd);

#endif
