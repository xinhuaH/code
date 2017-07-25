#include "public.h"
#include "device_command.h"
#include "device_register.h"
#include "bsp_data_port.h"
#include "stdio.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "bsp_led.h"

extern DataPort Address[ADDRESS_WIDTH];
unsigned int uiReadData[500] = {0,0};
extern volatile unsigned int uiTimerCount;

xSemaphoreHandle tRWMutex;

void deviceCommandCreateMutex(void)
{
    do
    {
        tRWMutex = xSemaphoreCreateMutex();
        printf("create tRWMutex:0x%x\n",tRWMutex);
    }while(tRWMutex == NULL);
}

void deviceCommandInit(void)
{
    deviceCommandCreateMutex();
}

DeviceCommandFun commandToFunction(unsigned int uiCmd)
{
    switch(uiCmd)
    {
        case CMD_GET_SOFTWARE_VERSION:
        {
            return acquireSoftwareVersion;
        }
        case CMD_GET_FPGA_SOFTWARE_VERSION:
        {
            return acquireFPGASoftwareVersion;
        }
        case CMD_CTL_RESET_ALL_VALUES:
        {
            return resetAllValues;
        }
        case CMD_CTL_CONTROL_ONE_VALUE:
        {
            return controlOneValue;
        }
        case CMD_CTL_CONTROL_ALL_VALUES:
        {
            return controlAllValues;
        }
        case CMD_CTL_INIT_LEVEL_MOTOR:
        {
            return initLevelMotor;
        }
        case CMD_CTL_RESET_LEVEL_MOTOR:
        {
            return resetLevelMotor;
        }
        case CMD_CTL_CONTROL_LEVLE_MOTOR_MOVE_POSITION:
        {
            return controlLevelMotorMovePosition;
        }
        case CMD_CTL_CONTROL_LEVEL_MOTOR_HOLD:
        {
            return controlLevelMotorHold;
        }
        case CMD_CTL_CONTOLE_LEVE_MOTOR_MOVE_STEPS:
        {
            return controlLevelMotorMoveSteps;
        }
        case CMD_CTL_CONTROL_NEEDLE_SWAY:
        {
            return controlNeedleSway;
        }
        case CMD_CTL_INIT_VERTICAL_MOTOR:
        {
            return initVerticalMotor;
        }
        case CMD_CTL_RESET_VERTICAL_MOTOR:
        {
            return resetVerticalMotor;
        }
        case CMD_CTL_CONTROL_VERTICAL_MOTOR_MOVE_POSITION:
        {
            return controlVerticalMotorMovePosition;
        }
        case CMD_CTL_CONTROL_VERTICAL_MOTOR_HOLD:
        {
            return controlVerticalMotorHold;
        }
        case CMD_CTL_CONTOLE_VERTICAL_MOTOR_MOVE_STEPS:
        {
            return controlVerticalMotorMoveSteps;
        }
        case CMD_CTL_CONTROL_VERTICAL_PUNCTURE:
        {
            return controlVerticalPuncture;
        }
        case CMD_CTL_SAMPLING_SELF_TEST:
        {
            return controlSamplingSelfTest;
        }
        case CMD_CTL_STOP_ALL_MOTORS:
        {
            return stopAllMotors;
        }
        default:
        {
            printf("don't support this cm:0x%x\n",uiCmd);
            return NULL;
        }
    }
}

void writeRegister(unsigned int uiAddress, unsigned int uiData, unsigned char ucSize)
{
    unsigned char ucCount          = 0;
    unsigned int  uiDataToWrite    = 0;
    unsigned int  uiAddressToWrite = 0;

    BSP_DATAPORT_ADDRESS_ModeSet(BK_MODE_OUT);
    BSP_DATAPORT_DATA_ModeSet(BK_MODE_OUT);

    for(ucCount = 0; ucCount < ucSize; ucCount++)
    {
        uiDataToWrite = (uiData & (0xFF << (ucCount * 8))) >> (ucCount * 8);
        uiAddressToWrite = uiAddress + ucCount;

        BSP_DATAPORT_Write(uiAddressToWrite, uiDataToWrite);
    }

    executeDelay(5);

    BSP_DATAPORT_Write_Enable();

    executeDelay(5);

    BSP_DATAPORT_Write_Disable();
}

void readRegister(unsigned int uiAddress, unsigned int *uipData, unsigned char ucSize)
{
    unsigned char ucCount         = 0;
    unsigned int  uiDataRead      = 0;
    unsigned int  uiAddressToRead = 0;

    *uipData = 0;

    BSP_DATAPORT_DATA_ModeSet(BK_MODE_IN);

    for(ucCount = 0; ucCount < ucSize; ucCount++)
    {
        uiDataRead = 0;
        uiAddressToRead = uiAddress + ucCount;

        BSP_DATAPORT_Read(uiAddressToRead, &uiDataRead);

        *uipData = *uipData | (uiDataRead << (ucCount * 8));
    }
}

unsigned char acquireSoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    static unsigned char a = 0;
    unsigned int uiCount = 0;

    a = (a + 1) % 2;

    //writeRegister(a, a, 1);
    //vTaskDelay( 1 );
    //readRegister(uiCount, &uiReadData[uiCount], 1);

    //printf("uiReadData:%d\n", uiReadData[uiCount]);
#if 0

    if(a)
    {
    writeRegister(a, a, 1);
    }
    else
    {
    readRegister(1, &uiReadData[uiCount], 1);
    printf("uiReadData:%d\n", uiReadData[uiCount]);
    }



    if(a)
    {
        //writeRegister(0xFFF, 0xFFFF, 1);
    }
    else
    {
        //writeRegister(0xFFF, 0x000, 1);
        printf("S[0x%x]\n",uiTimerCount);
        for(uiCount = 0; uiCount < 500; uiCount++)
        {
            writeRegister(uiCount, uiCount, 1);
            readRegister(uiCount, &uiReadData[uiCount], 1);
        }
        
        //writeRegister(0xFFF, 0xFFFF, 1);
        //readRegister(0x0, &uiReadData[0], 1);
        //printf("uiReadData:%d\n", uiReadData);
        
        //readRegister(0x1, &uiReadData[1], 1);

        printf("\n\n");

        for(uiCount = 0; uiCount < 500; uiCount++)
        {
            printf("[%d]:%d,%d", uiCount,uiReadData[uiCount],uiCount%2);
        }

        printf("\n\n");
    }
#endif

    printf("%s,IN\n",__FUNCTION__);
    //createTimerFunction(timerCallbackTest, 500);
    //vTaskDelay( 5000 );
    //printf("%s,OUT\n",__FUNCTION__);

    //print_call_stack(vTaskStackPointer());

    viewTaskStatus();

#if 0
    printf("cmb_get_sp():0x%x\n",cmb_get_sp());
    printf("vTaskStackPointer():0x%x\n",vTaskStackPointer());

    
    //print_call_stack(cmb_get_sp());

    print_call_stack(vTaskStackPointer());

    //tpDevice->tFun(&(tpDevice->ucDataBuf[0]), 1);
    viewTaskStack();
#endif

    return 0;
}

unsigned char acquireFPGASoftwareVersion(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char resetAllValues(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlOneValue(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlAllValues(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char initLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char resetLevelMotor(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlLevelMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlLevelMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlLevelMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlNeedleSway(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char initVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char resetVerticalMotor(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlVerticalMotorMovePosition(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlVerticalMotorHold(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlVerticalMotorMoveSteps(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlVerticalPuncture(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char controlSamplingSelfTest(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

unsigned char stopAllMotors(unsigned char *ucpBuf, unsigned int uiBufSize)
{
    printf("%s\n",__FUNCTION__);
    return 0;
}

//获取光耦状态
unsigned char acquireOptocouplerStatus(unsigned char ucNum)
{
    unsigned int uiStatus = BK_SHELTER_UNKNOW;

    switch(ucNum)
    {
        case OPTOCOUPLER_LEVEL:
        {
            readRegister(REGISTER_OPTOCOUPLER_LEVEL, &uiStatus, 1);
            break;
        }
        case OPTOCOUPLER_VERTICAL:
        {
            readRegister(REGISTER_OPTOCOUPLER_VERTICAL, &uiStatus, 1);
            break;
        }
        case OPTOCOUPLER_INJECTOR:
        {
            readRegister(REGISTER_OPTOCOUPLER_INJECTOR, &uiStatus, 1);
            break;
        }
        case OPTOCOUPLER_MOBILE:
        {
            readRegister(REGISTER_OPTOCOUPLER_MOBILE, &uiStatus, 1);
            break;
        }
        default:
        {
            printf("don't support this optocoupler:%d\n",ucNum);
            break;
        }
    }

    return (unsigned char)uiStatus;
}

//移动电机
void moveMotors(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces)
{
    switch(ucNum)
    {
        case MOTOR_LEVEL:
        {
            writeRegister(REGISTER_L_MOTOR_DIRECTION, ucDirection, 1);
            writeRegister(REGISTER_L_MOTOR_SPEED, uiSpeed, 1);
            writeRegister(REGISTER_L_MOTOR_PACES_L, uiPaces, 2);
            writeRegister(REGISTER_L_MOTOR_START, MOTOR_START, 1);
            break;
        }
        case MOTOR_VERTICAL:
        {
            writeRegister(REGISTER_V_MOTOR_DIRECTION, ucDirection, 1);
            writeRegister(REGISTER_V_MOTOR_SPEED, uiSpeed, 1);
            writeRegister(REGISTER_V_MOTOR_PACES_L, uiPaces, 2);
            writeRegister(REGISTER_V_MOTOR_START, MOTOR_START, 1);
            break;
        }
        case MOTOR_INJECTOR1:
        {
            writeRegister(REGISTER_I1_MOTOR_DIRECTION, ucDirection, 1);
            writeRegister(REGISTER_I1_MOTOR_SPEED, uiSpeed, 1);
            writeRegister(REGISTER_I1_MOTOR_PACES_L, uiPaces, 2);
            writeRegister(REGISTER_I1_MOTOR_START, MOTOR_START, 1);
            break;
        }
        case MOTOR_INJECTOR2:
        {
            writeRegister(REGISTER_I2_MOTOR_DIRECTION, ucDirection, 1);
            writeRegister(REGISTER_I2_MOTOR_SPEED, uiSpeed, 1);
            writeRegister(REGISTER_I2_MOTOR_PACES_L, uiPaces, 2);
            writeRegister(REGISTER_I2_MOTOR_START, MOTOR_START, 1);
            break;
        }
        default:
        {
            printf("don't support this motor:%d\n",ucNum);
            break;
        }
    }
}

//停止电机
void stopMotors(unsigned char ucNum)
{
    switch(ucNum)
    {
        case MOTOR_LEVEL:
        {
            writeRegister(REGISTER_L_MOTOR_STOP, MOTOR_STOP, 1);
            break;
        }
        case MOTOR_VERTICAL:
        {
            writeRegister(REGISTER_V_MOTOR_STOP, MOTOR_STOP, 1);
            break;
        }
        case MOTOR_INJECTOR1:
        {
            writeRegister(REGISTER_I1_MOTOR_STOP, MOTOR_STOP, 1);
            break;
        }
        case MOTOR_INJECTOR2:
        {
            writeRegister(REGISTER_I2_MOTOR_STOP, MOTOR_STOP, 1);
            break;
        }
        default:
        {
            printf("don't support this motor:%d\n",ucNum);
            break;
        }
    }
}

//判断采样针是否在垂直原点
unsigned char isSamplingVerticalOrigin(void)
{
    if(acquireOptocouplerStatus(OPTOCOUPLER_VERTICAL) == BK_SHELTER)
    {
        return BK_TRUE;
    }

    return BK_FALSE;
}

//判断采样针是否在水平原点
unsigned char isSamplingLevelOrigin(void)
{
    if(acquireOptocouplerStatus(OPTOCOUPLER_LEVEL) == BK_SHELTER)
    {
        return BK_TRUE;
    }

    return BK_FALSE;
}

//根据电机速度和步数计算电机移动时间
unsigned int calculateMotorsMovingTime(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces)
{
    unsigned int uiTime = 0;

    ucNum = ucNum;
    ucDirection = ucDirection;

    uiTime = uiPaces/uiSpeed;

    uiTime = uiTime / MOTOR_SPEED_FACTOR;

    uiTime = uiTime ? 1:uiTime;

    return uiTime;
}

//查询电机剩余步数
unsigned int acquireMotorsRemainPaces(unsigned char ucNum)
{
    
}

//通过延时的方式等待电机执行完毕
unsigned char waitingDelayMotorsComplete(unsigned char ucNum, unsigned char ucDirection, unsigned int uiSpeed, unsigned int uiPaces)
{
    unsigned int uiTime = 0;
    unsigned int uiAllTime = 0;
    unsigned int uiRemainPaces = 0;
    unsigned char ucRet = BK_SUCCESS;

    uiTime = calculateMotorsMovingTime(ucNum, ucDirection, uiSpeed, uiPaces);
    uiAllTime += uiTime;
    vTaskDelay(uiTime);

    do
    {
        uiRemainPaces = acquireMotorsRemainPaces(ucNum);

        if(uiRemainPaces > MOTOR_MOVE_COMPLETE)
        {
            uiTime = calculateMotorsMovingTime(ucNum, ucDirection, uiSpeed, uiRemainPaces);
            uiAllTime += uiTime;

            if(uiAllTime > MOTOR_MAX_MOVING_TIME)
            {
                ucRet = BK_FAILED;
                printf("motor:%d move timeout\n",ucNum);

                break;
            }

            vTaskDelay(uiTime);
        }
        else
        {
            break;
        }
    }while (1);

    return ucRet;
}

//向上找原点
unsigned char samplingMoveUpToOrigin(void)
{
    unsigned int uiMoveSteps = 0;
    unsigned char ucRet = BK_SUCCESS;

    do
    {
        uiMoveSteps = uiMoveSteps + SAMPLING_VERTICAL_MOTOR_ORIGIN_PER_MOVING;

        if(uiMoveSteps > (SAMPLING_VERTICAL_MOTOR_ORIGIN_MAX_PACES - SAMPLING_VERTICAL_ORIGIN_EXTRA_PACES))
        {
            ucRet = BK_FAILED;
            printf("sampling move up to origin paces overflow\n");
            break;
        }

        moveMotors(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_ORIGIN_SPEED, SAMPLING_VERTICAL_MOTOR_ORIGIN_PER_MOVING);

        if(waitingDelayMotorsComplete(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_ORIGIN_SPEED, SAMPLING_VERTICAL_MOTOR_ORIGIN_PER_MOVING) == BK_FAILED)
        {
            ucRet = BK_FAILED;
            break;
        }

    }while(isSamplingVerticalOrigin() == BK_FALSE);

    if(ucRet == BK_SUCCESS)
    {
        moveMotors(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_ORIGIN_SPEED, SAMPLING_VERTICAL_ORIGIN_EXTRA_PACES);

        if(waitingDelayMotorsComplete(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_ORIGIN_SPEED, SAMPLING_VERTICAL_MOTOR_ORIGIN_PER_MOVING) == BK_FAILED)
        {
            ucRet = BK_FAILED;
            printf("sampling move up to origin fail\n");
        }
        else
        {
            printf("sampling move up to origin success\n");
        }
    }

    return ucRet;
}

//向左找原点
unsigned char samplingMoveLeftToOrigin(void)
{
    unsigned int uiMoveSteps = 0;
    unsigned char ucRet = BK_SUCCESS;

    do
    {
        uiMoveSteps++;

        if(uiMoveSteps > SAMPLING_LEVEL_MOTOR_ORIGIN_MAX_PACES)
        {
            ucRet = BK_FAILED;
            printf("sampling move left to origin paces overflow\n");
            break;
        }

        moveMotors(MOTOR_LEVEL, MOTOR_LEFT, SAMPLING_LEVEL_MOTOR_ORIGIN_SPEED, SAMPLING_LEVEL_MOTOR_ORIGIN_PER_MOVING);

        if(waitingDelayMotorsComplete(MOTOR_LEVEL, MOTOR_LEFT, SAMPLING_LEVEL_MOTOR_ORIGIN_SPEED, SAMPLING_LEVEL_MOTOR_ORIGIN_PER_MOVING) == BK_FAILED)
        {
            ucRet = BK_FAILED;
            break;
        }

    }while(isSamplingLevelOrigin() == BK_FALSE);

    if(ucRet == BK_SUCCESS)
    {
        moveMotors(MOTOR_LEVEL, MOTOR_LEFT, SAMPLING_LEVEL_MOTOR_ORIGIN_SPEED, SAMPLING_LEVEL_ORIGIN_EXTRA_PACES);

        if(waitingDelayMotorsComplete(MOTOR_LEVEL, MOTOR_LEFT, SAMPLING_LEVEL_MOTOR_ORIGIN_SPEED, SAMPLING_LEVEL_MOTOR_ORIGIN_PER_MOVING) == BK_FAILED)
        {
            ucRet = BK_FAILED;
            printf("sampling move left to origin fail\n");
        }
        else
        {
            printf("sampling move left to origin success\n");
        }
    }

    return ucRet;
}


//水平抖动：用于在原点的场景，先出原点，再进原点，同时检测光耦对应状态
unsigned char samplingVerticalSway(void)
{
    moveMotors(MOTOR_LEVEL, MOTOR_RIGHT, SAMPLING_LEVEL_MOTOR_SWAY_SPEED, SAMPLING_LEVEL_SWAY_PACES);

    waitingDelayMotorsComplete(MOTOR_LEVEL, MOTOR_RIGHT, SAMPLING_LEVEL_MOTOR_SWAY_SPEED, SAMPLING_LEVEL_SWAY_PACES);

    if(isSamplingLevelOrigin() == BK_FALSE)
    {
        moveMotors(MOTOR_LEVEL, MOTOR_RIGHT, SAMPLING_LEVEL_MOTOR_SWAY_SPEED, SAMPLING_LEVEL_SWAY_PACES);

        waitingDelayMotorsComplete(MOTOR_LEVEL, MOTOR_RIGHT, SAMPLING_LEVEL_MOTOR_SWAY_SPEED, SAMPLING_LEVEL_SWAY_PACES);

        if(isSamplingLevelOrigin() == BK_TRUE)
        {
            printf("sampling vertical sway success\n");
            return BK_SUCCESS;
        }
    }

    printf("sampling vertical sway fail\n");
    return BK_FAILED;
}

//垂直抖动：用于在原点的场景，先出原点，再进原点，同时检测光耦对应状态
unsigned char samplingLevelSway(void)
{
    moveMotors(MOTOR_LEVEL, MOTOR_DOWN, SAMPLING_VERTICAL_MOTOR_SWAY_SPEED, SAMPLING_VERTICAL_SWAY_PACES);

    waitingDelayMotorsComplete(MOTOR_VERTICAL, MOTOR_DOWN, SAMPLING_VERTICAL_MOTOR_SWAY_SPEED, SAMPLING_VERTICAL_SWAY_PACES);

    if(isSamplingVerticalOrigin() == BK_FALSE)
    {
        moveMotors(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_SWAY_SPEED, SAMPLING_VERTICAL_SWAY_PACES);

        waitingDelayMotorsComplete(MOTOR_VERTICAL, MOTOR_UP, SAMPLING_VERTICAL_MOTOR_SWAY_SPEED, SAMPLING_VERTICAL_SWAY_PACES);

        if(isSamplingVerticalOrigin() == BK_TRUE)
        {
            printf("sampling vertical sway success\n");
            return BK_SUCCESS;
        }
    }

    printf("sampling vertical sway fail\n");
    return BK_FAILED;
}

//采样针垂直方向原点
unsigned char samplingVerticalToOrigin(void)
{
    unsigned char ucRet = BK_SUCCESS;

    if(isSamplingVerticalOrigin() == BK_TRUE)
    {
        if(samplingVerticalSway() == BK_SUCCESS)
        {
            ucRet = BK_SUCCESS;
        }
        else
        {
            ucRet = BK_FAILED;
        }
    }
    else
    {
        samplingMoveUpToOrigin();
    }
}

//采样针水平方向原点
unsigned char samplingLevelToOrigin(void)
{
    unsigned char ucRet = BK_SUCCESS;

    if(isSamplingLevelOrigin() == BK_TRUE)
    {
        
    }
}
