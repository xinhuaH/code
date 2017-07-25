#ifndef _DEVICE_COMMAND_H_
#define _DEVICE_COMMAND_H_

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

#define CMD_CTL_SYRINGE_INIT                           0x4210
#define CMD_CTL_SYRINGE_RESET                          0x4220
#define CMD_CTL_SYRINGE_SUCK                           0x4230
#define CMD_CTL_SYRINGE_ROW                            0x4240
#define CMD_CTL_SYRINGE_SUCK_ROW                       0x4241

#define CMD_CTL_SYRINGE_SELF_TEST                      0x4410

#define CMD_GET_CHAMBER_PRESSURE                       0x6010

typedef unsigned char (*DeviceCommandFun)(unsigned char *ucpBuf, unsigned int uiBufSize);

void deviceCommandCreateMutex(void);
void deviceCommandInit(void);

DeviceCommandFun commandToFunction(unsigned int uiCmd);
void writeRegister(unsigned int uiAddress, unsigned int uiData, unsigned char ucSize);
void readRegister(unsigned int uiAddress, unsigned int *uipData, unsigned char ucSize);
unsigned char acquireSoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char acquireFPGASoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char resetAllValues(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlOneValue(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlAllValues(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char initLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char resetLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlLevelMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlLevelMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlLevelMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlNeedleSway(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char initVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char resetVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlVerticalMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlVerticalMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlVerticalMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlVerticalPuncture(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char controlSamplingSelfTest(unsigned char *ucpBuf, unsigned int uiBufSize);
unsigned char stopAllMotors(unsigned char *ucpBuf, unsigned int uiBufSize);

unsigned char acquireOptocouplerStatus(unsigned char ucNum);
void moveMotors(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces);
unsigned char isSamplingVerticalOrigin(void);
unsigned int calculateMotorsMovingTime(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces);
unsigned int acquireMotorsRemainPaces(unsigned char ucNum);
unsigned char waitingDelayMotorsComplete(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces);
unsigned char samplingMoveUpToOrigin(void);
unsigned char samplingVerticalSway(void);
unsigned char samplingVerticalToOrigin(void);
unsigned char isSamplingLevelOrigin(void);

#endif
