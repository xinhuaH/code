#ifndef _PROTOCOL_ANALYSIS_H_
#define _PROTOCOL_ANALYSIS_H_

#include "device_control.h"

#define CMD_GET_SOFTWARE_VERSION                       0x1030//0x1000
#define CMD_GET_FPGA_SOFTWARE_VERSION                  0x1001

#define CMD_CTL_RESET_ALL_VALUES                       0x2001
#define CMD_CTL_CONTROL_ONE_VALUE                      0x2002
#define CMD_CTL_CONTROL_ALL_VALUES                     0x2003

#define CMD_CTL_INIT_LEVEL_MOTOR                       0x3010
#define CMD_CTL_RESET_LEVEL_MOTOR                      0x3020
#define CMD_CTL_CONTROL_LEVLE_MOTOR_MOVE_POSITION      0x3030
#define CMD_CTL_CONTROL_LEVEL_MOTOR_HOLD               0x3040
#define CMD_CTL_CONTOLE_LEVE_MOTOR_MOVE_STEPS          0x3050
#define CMD_CTL_CONTROL_NEEDLE_SWAY                    0x3060

#define CMD_CTL_INIT_VERTICAL_MOTOR                    0x3110
#define CMD_CTL_RESET_VERTICAL_MOTOR                   0x3120
#define CMD_CTL_CONTROL_VERTICAL_MOTOR_MOVE_POSITION   0x3130
#define CMD_CTL_CONTROL_VERTICAL_MOTOR_HOLD            0x3140
#define CMD_CTL_CONTOLE_VERTICAL_MOTOR_MOVE_STEPS      0x3150
#define CMD_CTL_CONTROL_VERTICAL_PUNCTURE              0x3160


#define CMD_CTL_SAMPLING_SELF_TEST                     0x3210
#define CMD_CTL_STOP_ALL_MOTORS                        0x3300

#define MAX_CMD_COUNT                                  16
#define SIZE_OF_DATA_HEAD                              6
#define MAX_CMD_DATA_BUFFER_SIZE                       30
#define MAX_DATA_RECIEVE_BUFFER                        160

#define PROTOCOL_DATA_END     0x16
#define PROTOCOL_DATA_BEGIN   0x68


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
DeviceControlerFun commandToFunction(unsigned int uiCmd);

#endif
