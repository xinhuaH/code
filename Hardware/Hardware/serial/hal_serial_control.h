#ifndef ANDROID_INCLUDE_HARDWARE_HAL_SERIAL_CONTROL_H
#define ANDROID_INCLUDE_HARDWARE_HAL_SERIAL_CONTROL_H

#include <hardware.h>

#define SERIAL_CONTROL_HARDWARE_MODULE_ID "hal_serial_control"
#define SERIAL_CONTROL_HARDWARE_INTERFACE "hal_serial_control_if"

#define SERIAL_CONTROL_MODULE_API_VERSION_1_0 HARDWARE_MODULE_API_VERSION(1, 0)
#define SERIAL_CONTROL_MODULE_API_VERSION_CURRENT HDMI_MODULE_API_VERSION_1_0

#define SERIAL_CONTROL_DEVICE_API_VERSION_1_0 HARDWARE_DEVICE_API_VERSION(1, 0)
#define SERIAL_CONTROL_DEVICE_API_VERSION_CURRENT HDMI_DEVICE_API_VERSION_1_0

#define UART_REV_BUFFER_LEN    3          /*接收缓冲区长度*/

#define SERIAL_DATA_HEAD       0XA3       /*数据接收头*/
#define SERIAL_DATA_CHECKSUM   0X5C       /*数据校验和*/
#define SERIAL_TOTAL_KEYS      25

extern int Cominit();
extern int ComRead(unsigned char* pucReadData);
extern int ComWrite(unsigned char* pucWriteData);

typedef void (*send_keyevent_t)(int keyevent);


typedef enum
{
    SERIAL_KEY_CHANNEL_UP     = 0x02,
    SERIAL_KEY_CHANNEL_DOWN   = 0x03,
    SERIAL_KEY_VOL_UP         = 0x05,
    SERIAL_KEY_VOL_DOWN       = 0x06,
    SERIAL_KEY_UP             = 0x07,
    SERIAL_KEY_POWER          = 0x08,
    SERIAL_KEY_MENU           = 0x0A,
    SERIAL_KEY_DOWN           = 0x0B,
    SERIAL_KEY_LEFT           = 0x0C,
    SERIAL_KEY_ENTER          = 0x0D,
    SERIAL_KEY_BACK           = 0x0E,
    SERIAL_KEY_RIGHT          = 0x0F,
    SERIAL_KEY_0              = 0x10,
    SERIAL_KEY_1              = 0x11,
    SERIAL_KEY_2              = 0x12,
    SERIAL_KEY_3              = 0x13,
    SERIAL_KEY_4              = 0x14,
    SERIAL_KEY_5              = 0x15,
    SERIAL_KEY_6              = 0x16,
    SERIAL_KEY_7              = 0x17,
    SERIAL_KEY_8              = 0x18,
    SERIAL_KEY_9              = 0x19,
    SERIAL_KEY_MUTE           = 0x1A,
    SERIAL_KEY_SOURCE         = 0x20,
    SERIAL_KEY_GAME           = 0x53,
}SERIAL_DATA_FOR_KEY;

#if 0
    SERIAL_KEY_VOD = 0xFF,
    SERIAL_KEY_APP_CENTER = 0xFF,
    SERIAL_KEY_GLOABLE_PANEL = 0xFF,
    SERIAL_KEY_INFO
    SERIAL_KEY_RED
    SERIAL_KEY_GREEN
    SERIAL_KEY_YELLOW
    SERIAL_KEY_BLUE
    SERIAL_KEY_MEDIA_CENTER
    SERIAL_KEY_TV
    SERIAL_KEY_SETTINGS
    SERIAL_KEY_PRVE
    SERIAL_KEY_NEXT
#endif

typedef enum
{
    KEYCODE_BACK                = 4,
    KEYCODE_0                   = 7,
    KEYCODE_1                   = 8,
    KEYCODE_2                   = 9,
    KEYCODE_3                   = 10,
    KEYCODE_4                   = 11,
    KEYCODE_5                   = 12,
    KEYCODE_6                   = 13,
    KEYCODE_7                   = 14,
    KEYCODE_8                   = 15,
    KEYCODE_9                   = 16,
    KEYCODE_DPAD_UP             = 19,
    KEYCODE_DPAD_DOWN           = 20,
    KEYCODE_DPAD_LEFT           = 21,
    KEYCODE_DPAD_RIGHT          = 22,
    KEYCODE_DPAD_CENTER         = 23,
    KEYCODE_VOLUME_UP           = 24,
    KEYCODE_VOLUME_DOWN         = 25,
    KEYCODE_POWER               = 26,
    KEYCODE_MENU                = 82,
    KEYCODE_VOD                 = 131,
    KEYCODE_APP_CENTER          = 132,
    KEYCODE_SOURCE              = 140,
    KEYCODE_GLOABLE_PANEL       = 142,
    KEYCODE_INFO                = 165,
    KEYCODE_VOLUME_MUTE         = 164,
    KEYCODE_CHANNEL_UP          = 166,
    KEYCODE_CHANNEL_DOWN        = 167,
    KEYCODE_TV                  = 170,
    KEYCODE_SETTINGS            = 176,
    KEYCODE_RED                 = 183,
    KEYCODE_GREEN               = 184,
    KEYCODE_YELLOW              = 185,
    KEYCODE_BLUE                = 186,
    KEYCODE_MEDIA_CENTER        = 209,
    KEYCODE_PREV                = 4307,
    KEYCODE_NEXT                = 4308,
    KEYCODE_GAME                = 4309,
}REPORT_ANDROID_KEYCODE;

typedef struct _keycodeMap
{
    SERIAL_DATA_FOR_KEY SerialScancode;
    REPORT_ANDROID_KEYCODE Keycode;
} KeycodeMap;

typedef struct hal_serial_control_module
{
    struct hw_module_t common;

} hal_serial_control_module_t;

typedef struct hal_serial_control_device
{
    struct hw_device_t common;
    send_keyevent_t SendKeyeventCallback;
    int (*register_sendkeyevent_callback)(struct hal_serial_control_device* dev,send_keyevent_t SendKeyeventFunction);
    int (*send_event)(const struct hal_serial_control_device* dev,int keyevent);
    int (*read_data_from_serial)(struct hal_serial_control_device* dev,int* serial_data);

    void* reserved[16 - 4];

} hal_serial_control_device_t;

static inline int serial_control_open(const struct hw_module_t* module,struct hal_serial_control_device** device)
{
    return module->methods->open(module,SERIAL_CONTROL_HARDWARE_INTERFACE, (struct hw_device_t**)device);
}

static inline int serial_control_close(struct hal_serial_control_device* device)
{
    return device->common.close(&device->common);
}

#endif
