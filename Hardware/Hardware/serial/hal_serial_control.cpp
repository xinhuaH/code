#include <mi_common.h>
#include <mif_internal_handle.h>
#include <mif_internal_debug.h>
#include <hs_type.h>
#include "hal_serial_control.h"
#include <pthread.h>
#include <unistd.h>
#include <log.h>
#include <properties.h>

#define LOG_TAG "HAL_SERIAL"

KeycodeMap SerialToKeycode[] =
{
    {SERIAL_KEY_CHANNEL_UP,      KEYCODE_CHANNEL_UP},
    {SERIAL_KEY_CHANNEL_DOWN,    KEYCODE_CHANNEL_DOWN},
    {SERIAL_KEY_VOL_UP,          KEYCODE_VOLUME_UP},
    {SERIAL_KEY_VOL_DOWN,        KEYCODE_VOLUME_DOWN},
    {SERIAL_KEY_UP,              KEYCODE_DPAD_UP},
    {SERIAL_KEY_POWER,           KEYCODE_POWER},
    {SERIAL_KEY_MENU,            KEYCODE_MENU },
    {SERIAL_KEY_DOWN,            KEYCODE_DPAD_DOWN},
    {SERIAL_KEY_LEFT,            KEYCODE_DPAD_LEFT},
    {SERIAL_KEY_ENTER,           KEYCODE_DPAD_CENTER},
    {SERIAL_KEY_BACK,            KEYCODE_BACK},
    {SERIAL_KEY_RIGHT,           KEYCODE_DPAD_RIGHT},
    {SERIAL_KEY_0,               KEYCODE_0},
    {SERIAL_KEY_1,               KEYCODE_1},
    {SERIAL_KEY_2,               KEYCODE_2},
    {SERIAL_KEY_3,               KEYCODE_3},
    {SERIAL_KEY_4,               KEYCODE_4},
    {SERIAL_KEY_5,               KEYCODE_5},
    {SERIAL_KEY_6,               KEYCODE_6},
    {SERIAL_KEY_7,               KEYCODE_7},
    {SERIAL_KEY_8,               KEYCODE_8},
    {SERIAL_KEY_9,               KEYCODE_9},
    {SERIAL_KEY_MUTE,            KEYCODE_VOLUME_MUTE},
    {SERIAL_KEY_SOURCE,          KEYCODE_SOURCE},
    {SERIAL_KEY_GAME,            KEYCODE_GAME},
};

int RegisterSendKeyeventCallback(struct hal_serial_control_device* dev,send_keyevent_t SendKeyeventFunction)
{
    if(dev == NULL || SendKeyeventFunction == NULL)
    {
        SLOGE("Register send keyevent function fail");
        return -1;
    }

    dev->SendKeyeventCallback = SendKeyeventFunction;

    SLOGE("Register send keyevent function sucess");

    return 0;
}

void* SendEventThread(void *dev)
{
    struct hal_serial_control_device* device = (struct hal_serial_control_device *) dev;

    while(1)
    {
        usleep(5000000);//5000ms
        if(device->SendKeyeventCallback != NULL)
        {
            device->SendKeyeventCallback(25);
            SLOGE("[T]");
        }
    }

     return dev;

}

int SendEvent(const struct hal_serial_control_device* dev,int keyevent)
{
    if(dev == NULL)
    {
        SLOGE("Send event fail");
        return -1;
    }

    if(dev->SendKeyeventCallback != NULL)
    {
       dev->SendKeyeventCallback(keyevent);
       SLOGE("Send keyevent:%d",keyevent);
    }

    return 0;

}
int CreateThreadForEvent(struct hal_serial_control_device* dev)
{
    pthread_attr_t thread_attr;
    pthread_t thread_id;
    int ret = 0;

    pthread_attr_init(&thread_attr);

    //thread will release all resources after return
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    //thread will be scheduled in default way
    pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);

    pthread_attr_setstacksize(&thread_attr, 4096);

    ret = pthread_create(&thread_id,&thread_attr,SendEventThread,dev);

    pthread_attr_destroy(&thread_attr);

    if(ret != 0)
    {
        SLOGE("Create thread for serial event error");
    }
    else
    {
        SLOGE("Create thread for serial sucess");
    }
    return 0;
}
static int AnalysisSerialData(unsigned char* SerialBuffer,unsigned char BufferLength,int *SerialData)
{
    if(BufferLength == UART_REV_BUFFER_LEN)
    {
        if(*SerialBuffer == SERIAL_DATA_HEAD)
        {
            if((*(SerialBuffer+1) + *(SerialBuffer+2)) == SERIAL_DATA_CHECKSUM)
            {
                *SerialData = *(SerialBuffer+1);
                return 0;
            }
        }
    }

    return -1;
}

static int SerialDataToKeycode(int* SerialData,int* Keycode)
{
    unsigned char i = 0;
    unsigned char ArraySize = sizeof(SerialToKeycode)/sizeof(KeycodeMap);

    for(i = 0; i < ArraySize; i++)
    {
        if(SerialToKeycode[i].SerialScancode == *SerialData)
        {
            *Keycode = (int)SerialToKeycode[i].Keycode;
            break;
        }
    }

    if(i < ArraySize)
    {
        return 0;
    }
    else
    {
        SLOGE("Not support this key:0x%x",*SerialData);
        return -1;
    }
}

static int ReadDataFromSerial(struct hal_serial_control_device* dev,int* serial_data)
{
    int ret = 0;
    int Keycode = 0;
    unsigned char i = 0;
    unsigned char SerialDataBuffer[UART_REV_BUFFER_LEN];

    if(dev == NULL)
    {
        SLOGE("Read data from serial fail");
        return -1;
    }

    ret = ComRead(SerialDataBuffer);

    if(ret < 0)
    {
         SLOGE("ComRead return error");

         return ret;
    }

    ret = AnalysisSerialData(SerialDataBuffer,ret,serial_data);

    if(ret)
    {
        SLOGE("AnalysisSerialData error");

        for(i = 0;i < UART_REV_BUFFER_LEN;i++)
        {
             SLOGE("Serial[%d] = 0x%x ",i,SerialDataBuffer[i]);
        }

        *serial_data = 0;

        return ret;
    }

    ret = SerialDataToKeycode(serial_data,&Keycode);

    if(ret)
    {
        *serial_data = 0;

        return ret;
    }

    *serial_data = Keycode;

    SLOGE("Recieve keycode 0x%x from serial",*serial_data);

    return ret;
}


static int serial_control_open(const struct hw_module_t* module,const char* name,
                               struct hw_device_t** device)
{
    int ret = 0;
    if (!strcmp(name,SERIAL_CONTROL_HARDWARE_INTERFACE))
    {

        hal_serial_control_device_t *dev = (hal_serial_control_device_t*) malloc(sizeof(hal_serial_control_device_t));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->common.tag = HARDWARE_DEVICE_TAG;
        dev->common.version = SERIAL_CONTROL_MODULE_API_VERSION_1_0;
        dev->common.module = const_cast<hw_module_t*>(module);

        dev->register_sendkeyevent_callback = RegisterSendKeyeventCallback;
        dev->send_event = SendEvent;
        dev->read_data_from_serial = ReadDataFromSerial;

        *device = &dev->common;

    }
    else
    {
        SLOGE("serial_control_open fail");
        return -1;
    }

    ret = Cominit();

    if(ret)
    {
        SLOGE("serial init fail");
    }

    return ret;
}

static struct hw_module_methods_t hal_serial_control_module_methods = {
    .open = serial_control_open,
};

struct hal_serial_control_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = SERIAL_CONTROL_HARDWARE_MODULE_ID,
        .name = "SERIAL CONTROL HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_serial_control_module_methods,
    },
};

