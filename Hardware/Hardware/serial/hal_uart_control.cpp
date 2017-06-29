/************************************************************************
* 版权所有 (C)2013,Hisense corporation copyright
*
* 文件名称：DrvUartTest.c
* 文件标识:
* 内容摘要： Uart interface
* 其它说明:
* 当前版本：v1.0
* 作           者： chudejin
* 完成日期： 2013-9-16
*
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容:
* 修改记录2：…
************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <mi_common.h>
#include <mif_internal_handle.h>
#include <mif_internal_debug.h>
#include <hs_type.h>
#include "hal_serial_control.h"
#include <pthread.h>
#include <unistd.h>
#include <log.h>
#include <properties.h>

#define LOG_TAG "HAL_SERIAL_UART"



/**************************************************************************
 *                          宏定义                                                                              *
 **************************************************************************/

/* 流控的取值 */
#define  NO_FLOWCTRL    0
#define  HARDWARE_FLOWCTRL    1
#define  SOFTWARE_FLOWCTRL     2


/* 打开模式保持和linux内核的定义一致 */
#define UART_NONBLOCK   O_NONBLOCK//0x00000800
#define UART_NOCTTY     O_NOCTTY//0x00000400
#define UART_RDONLY     O_RDONLY//0x00000000
#define UART_WRONLY     O_WRONLY//0x00000001
#define UART_RDWR       O_RDWR//0x00000002


/**************************************************************************
 *                            数据类型                                                                       *
 **************************************************************************/
/* 串口属性 */
typedef struct tagT_DrvUartAttr
{
    unsigned int       dwBaudRate;          /*波特率*/
    unsigned char      ucDataBit;           /*数据位5，6，7，8 */
    unsigned char      ucStopBit;           /*停止位1 和2  */
    unsigned char      ucParity;            /*校验位 0 =None ，1 = Even， 2 =Odd*/
    unsigned char      ucFlowControl;       /*流控制*/
}T_DrvUartAttr;

typedef enum tagT_DrvUartFlushMode
{
    UART_INFLUSH = 0,
    UART_OUTFLUSH,
    UART_IOFLUSH
} T_DrvUartFlushMode;

/*串口文件句柄*/
static int sdwFileHander = 0;

/*****************************************************
* 函数名称： DrvUartSetAttr
* 功能描述：设置串口属性
* 输入参数： 无
* 输出参数：无
* 返 回 值：   0 or 错误码
* 其他说明：
* 修改日期   版本号    修改人  修改内容
* ----------------------------------------------------
 2013-9-16         v0.1              初德进   创建
******************************************************/
static int DrvUartSetAttr (int dwIndex, T_DrvUartAttr * ptUartAttr)
{
    int sdwRet = 0;
    struct termios tNewTio;
    //bzero(&tNewTio, sizeof(tNewTio));
    memset(&tNewTio, 0, sizeof(tNewTio));
    cfmakeraw(&tNewTio);
    /* 设置波特率 */
    switch (ptUartAttr->dwBaudRate)
    {
        case 1000000:
        {
            cfsetispeed(&tNewTio, B1000000);
            cfsetospeed(&tNewTio, B1000000);
            break;
        }
        case 921600:
        {
            cfsetispeed(&tNewTio, B921600);
            cfsetospeed(&tNewTio, B921600);
            break;
        }
        case 576000:
        {
            cfsetispeed(&tNewTio, B576000);
            cfsetospeed(&tNewTio, B576000);
            break;
        }
        case 115200:
        {
            cfsetispeed(&tNewTio, B115200);
            cfsetospeed(&tNewTio, B115200);
            break;
        }
        case 57600:
        {
            cfsetispeed(&tNewTio, B57600);
            cfsetospeed(&tNewTio, B57600);
            break;
        }
        case 38400:
        {
            cfsetispeed(&tNewTio, B38400);
            cfsetospeed(&tNewTio, B38400);
            break;
        }
        case 19200:
        {
            cfsetispeed(&tNewTio, B19200);
            cfsetospeed(&tNewTio, B19200);
            break;
        }
        case 9600:
        {
            cfsetispeed(&tNewTio, B9600);
            cfsetospeed(&tNewTio, B9600);
            break;
        }
        default:
        {
            return -1;
        }
    }
    /* 更改立即生效 */
    sdwRet = tcsetattr(dwIndex, TCSANOW, &tNewTio);
    if (sdwRet != 0)
    {
        SLOGE("set_Baut tcgetattr error!");
        return -1;
    }
    /* 刷新缓冲区 */
    tcflush(dwIndex, TCIOFLUSH);
    sdwRet = tcgetattr( dwIndex, &tNewTio);
    if ( sdwRet != 0)
    {
        SLOGE("set_parity tcgetattr error!");
        return -1;
    }
    /*  数据位设置 */
    switch (ptUartAttr->ucDataBit)
    {
        case 5:
        {
            tNewTio.c_cflag &= ~CSIZE;
            tNewTio.c_cflag |= CS5;
            break;
        }
        case 6:
        {
            tNewTio.c_cflag &= ~CSIZE;
            tNewTio.c_cflag |= CS6;
            break;
        }

        case 7:
        {
            tNewTio.c_cflag &= ~CSIZE;
            tNewTio.c_cflag |= CS7;
            break;
        }
        case 8:
        {
            tNewTio.c_cflag &= ~CSIZE;
            tNewTio.c_cflag |= CS8;
            break;
        }
        default:
        {
            return -1;
        }
    }
    /* 奇偶校验设置 */
    switch (ptUartAttr->ucParity)
    {
        case 0:
        {
            tNewTio.c_cflag &= ~PARENB;
            tNewTio.c_iflag &= ~INPCK;
            break;
        }
        case 1:
        {
            tNewTio.c_iflag |= (INPCK | ISTRIP);
            tNewTio.c_cflag |= PARENB;
            tNewTio.c_cflag &= ~PARODD;
            break;
        }
        case 2:
        {
            tNewTio.c_cflag |= PARENB;
            tNewTio.c_cflag |= PARODD;
            tNewTio.c_iflag |= (INPCK | ISTRIP);
            break;
        }
        default:
        {
            return -1;
        }
    }
    /* 停止位设置 */
    switch (ptUartAttr->ucStopBit)
    {
        case 1:
        {
            tNewTio.c_cflag &= ~CSTOPB;
            break;
        }
        case 2:
        {
            tNewTio.c_cflag |= CSTOPB;
            break;
        }
        default:
        {
            return -1;
        }
    }

    tNewTio.c_oflag &= ~OPOST;
    tNewTio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tNewTio.c_cc[VTIME] = 0;
    tNewTio.c_cc[VMIN] = 1;
    tNewTio.c_cflag |= (CLOCAL | CREAD);
    tNewTio.c_iflag &= ~(BRKINT | ICRNL | IXON);


    /* 设置串口流控 */
    switch (ptUartAttr->ucFlowControl)
    {
        case NO_FLOWCTRL:
        {
            tNewTio.c_cflag &= ~CRTSCTS;
            tNewTio.c_iflag &= ~(IXON | IXOFF | IXANY);
            break;
        }
        case HARDWARE_FLOWCTRL:
        {
            tNewTio.c_cflag |= CRTSCTS;
            break;
        }
        case SOFTWARE_FLOWCTRL:
        {
            tNewTio.c_cflag |= (IXON | IXOFF | IXANY);
            break;
        }

        default:                                                                                                                                                                           /* 缺省不设置流控 */
        {
            tNewTio.c_cflag &= ~CRTSCTS;
            tNewTio.c_iflag &= ~(IXON | IXOFF | IXANY);

            break;
        }
    }

    /* 更改立即生效 */
    sdwRet = tcsetattr(dwIndex, TCSANOW, &tNewTio);
    if (sdwRet != 0)
    {
        return -1;
    }
    /* 刷新缓冲区 */
    tcflush(dwIndex, TCIOFLUSH);
    return 0;

}
/*****************************************************
* 函数名称： DrvUartFlush
* 功能描述：刷新串口缓冲区
* 修改变量：
* 输入参数： dwIndex : 串口索引
*                              tMode : 刷新模式
* 输出参数:
* 返 回 值：    USP_SUCCESS or 错误码
* 其他说明：
* 修改日期   版本号    修改人  修改内容
* ----------------------------------------------------

******************************************************/
static int DrvUartFlush (int dwIndex, T_DrvUartFlushMode tMode)
{
    int sdwRet = 0;
    sdwRet = tcflush(dwIndex, tMode);
    if (0 != sdwRet)
    {
        return -1;
    }
    return 0;
}
/*****************************************************
* 函数名称： Cominit
* 功能描述：串口初始化
* 输入参数： 无
* 输出参数：无
* 返 回 值：   0 or 错误码
* 其他说明：
* 修改日期   版本号    修改人  修改内容
* ----------------------------------------------------
 2013-9-16         v0.1              初德进   创建
******************************************************/
int Cominit(void)
{
    int sdwRet = 0;
    T_DrvUartAttr  tUartAttr;
    sdwFileHander = open("/dev/ttyS1", UART_NOCTTY | UART_RDWR);
    if(sdwFileHander < 0)
    {
        SLOGE(" \nCominit open uart error\n ");
        return -1;
    }
    tUartAttr.dwBaudRate = 115200;
    tUartAttr.ucDataBit = 8;
    tUartAttr.ucParity = 0;
    tUartAttr.ucStopBit = 1;
    tUartAttr.ucFlowControl = 0;
    sdwRet = DrvUartSetAttr(sdwFileHander, &tUartAttr);
    if(0 != sdwRet)
    {
        SLOGE(" \nCominit open uart DrvUartSetAttr error : %d\n ", sdwRet);
        close(sdwFileHander);
        return sdwRet;
    }
    sdwRet = DrvUartFlush(sdwFileHander, UART_IOFLUSH);
    if(0 != sdwRet)
    {
        SLOGE(" \nCominit open uart DrvUartFlush error : %d\n ", sdwRet);
        close(sdwFileHander);
        return sdwRet;
    }
    return 0;
}

/*****************************************************
* 函数名称： ComRead
* 功能描述：串口读
* 输入参数：无
* 输出参数：读到的数据
* 返 回 值：   0:超时-1：异常；n >0成功读取的字符长度
* 其他说明：
* 修改日期   版本号    修改人  修改内容
* ----------------------------------------------------
 2013-9-16         v0.1              初德进   创建
******************************************************/
int ComRead(unsigned char* pucReadData)
{
    int sdwRet = 0;

    if(sdwFileHander < 0)
    {
        SLOGE("Error sdwFileHander is %d\n ", sdwFileHander);
        return -1;
    }

    sdwRet = read(sdwFileHander, pucReadData, UART_REV_BUFFER_LEN);
    if (sdwRet < 0)
    {
        SLOGE("ComRead read uart error : %d\n ", sdwRet);
        return -1;
    }

    return sdwRet;

}

/*****************************************************
* 函数名称： ComWrite
* 功能描述：串口写
* 输入参数：要发送的数据
* 返 回 值：   -1：异常；0:成功
* 其他说明：
* 修改日期   版本号    修改人  修改内容
* ----------------------------------------------------
 2013-9-16         v0.1              初德进   创建
******************************************************/
int ComWrite(unsigned char* pucWriteData)
{
    int sdwRet = 0;
    if(sdwFileHander < 0)
    {
        return -1;
    }
    //SLOGE("#cdj#wr data %x %x %x %x %x %x %x %x\n",pucWriteData[0],pucWriteData[1],pucWriteData[2],pucWriteData[3],pucWriteData[4],pucWriteData[5],pucWriteData[6],pucWriteData[7]);
    sdwRet = DrvUartFlush(sdwFileHander, UART_IOFLUSH);
    if(0 != sdwRet)
    {
        SLOGE(" \n ComWrite DrvUartFlush error : %d\n ", sdwRet);
        close(sdwFileHander);
        return sdwRet;
    }

    sdwRet = write(sdwFileHander, pucWriteData, 8);
    if (sdwRet < 0)
    {
        return sdwRet;
    }
    else
    {
        return 0;
    }
}
