/************************************************************************
* ��Ȩ���� (C)2013,Hisense corporation copyright
*
* �ļ����ƣ�DrvUartTest.c
* �ļ���ʶ:
* ����ժҪ�� Uart interface
* ����˵��:
* ��ǰ�汾��v1.0
* ��           �ߣ� chudejin
* ������ڣ� 2013-9-16
*
* �޸ļ�¼1��
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸�����:
* �޸ļ�¼2����
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
 *                          �궨��                                                                              *
 **************************************************************************/

/* ���ص�ȡֵ */
#define  NO_FLOWCTRL    0
#define  HARDWARE_FLOWCTRL    1
#define  SOFTWARE_FLOWCTRL     2


/* ��ģʽ���ֺ�linux�ں˵Ķ���һ�� */
#define UART_NONBLOCK   O_NONBLOCK//0x00000800
#define UART_NOCTTY     O_NOCTTY//0x00000400
#define UART_RDONLY     O_RDONLY//0x00000000
#define UART_WRONLY     O_WRONLY//0x00000001
#define UART_RDWR       O_RDWR//0x00000002


/**************************************************************************
 *                            ��������                                                                       *
 **************************************************************************/
/* �������� */
typedef struct tagT_DrvUartAttr
{
    unsigned int       dwBaudRate;          /*������*/
    unsigned char      ucDataBit;           /*����λ5��6��7��8 */
    unsigned char      ucStopBit;           /*ֹͣλ1 ��2  */
    unsigned char      ucParity;            /*У��λ 0 =None ��1 = Even�� 2 =Odd*/
    unsigned char      ucFlowControl;       /*������*/
}T_DrvUartAttr;

typedef enum tagT_DrvUartFlushMode
{
    UART_INFLUSH = 0,
    UART_OUTFLUSH,
    UART_IOFLUSH
} T_DrvUartFlushMode;

/*�����ļ����*/
static int sdwFileHander = 0;

/*****************************************************
* �������ƣ� DrvUartSetAttr
* �������������ô�������
* ��������� ��
* �����������
* �� �� ֵ��   0 or ������
* ����˵����
* �޸�����   �汾��    �޸���  �޸�����
* ----------------------------------------------------
 2013-9-16         v0.1              ���½�   ����
******************************************************/
static int DrvUartSetAttr (int dwIndex, T_DrvUartAttr * ptUartAttr)
{
    int sdwRet = 0;
    struct termios tNewTio;
    //bzero(&tNewTio, sizeof(tNewTio));
    memset(&tNewTio, 0, sizeof(tNewTio));
    cfmakeraw(&tNewTio);
    /* ���ò����� */
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
    /* ����������Ч */
    sdwRet = tcsetattr(dwIndex, TCSANOW, &tNewTio);
    if (sdwRet != 0)
    {
        SLOGE("set_Baut tcgetattr error!");
        return -1;
    }
    /* ˢ�»����� */
    tcflush(dwIndex, TCIOFLUSH);
    sdwRet = tcgetattr( dwIndex, &tNewTio);
    if ( sdwRet != 0)
    {
        SLOGE("set_parity tcgetattr error!");
        return -1;
    }
    /*  ����λ���� */
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
    /* ��żУ������ */
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
    /* ֹͣλ���� */
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


    /* ���ô������� */
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

        default:                                                                                                                                                                           /* ȱʡ���������� */
        {
            tNewTio.c_cflag &= ~CRTSCTS;
            tNewTio.c_iflag &= ~(IXON | IXOFF | IXANY);

            break;
        }
    }

    /* ����������Ч */
    sdwRet = tcsetattr(dwIndex, TCSANOW, &tNewTio);
    if (sdwRet != 0)
    {
        return -1;
    }
    /* ˢ�»����� */
    tcflush(dwIndex, TCIOFLUSH);
    return 0;

}
/*****************************************************
* �������ƣ� DrvUartFlush
* ����������ˢ�´��ڻ�����
* �޸ı�����
* ��������� dwIndex : ��������
*                              tMode : ˢ��ģʽ
* �������:
* �� �� ֵ��    USP_SUCCESS or ������
* ����˵����
* �޸�����   �汾��    �޸���  �޸�����
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
* �������ƣ� Cominit
* �������������ڳ�ʼ��
* ��������� ��
* �����������
* �� �� ֵ��   0 or ������
* ����˵����
* �޸�����   �汾��    �޸���  �޸�����
* ----------------------------------------------------
 2013-9-16         v0.1              ���½�   ����
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
* �������ƣ� ComRead
* �������������ڶ�
* �����������
* �������������������
* �� �� ֵ��   0:��ʱ-1���쳣��n >0�ɹ���ȡ���ַ�����
* ����˵����
* �޸�����   �汾��    �޸���  �޸�����
* ----------------------------------------------------
 2013-9-16         v0.1              ���½�   ����
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
* �������ƣ� ComWrite
* ��������������д
* ���������Ҫ���͵�����
* �� �� ֵ��   -1���쳣��0:�ɹ�
* ����˵����
* �޸�����   �汾��    �޸���  �޸�����
* ----------------------------------------------------
 2013-9-16         v0.1              ���½�   ����
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
