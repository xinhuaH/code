#ifndef ANDROID_INCLUDE_HARDWARE_HAL_POWER_CONTROL_H
#define ANDROID_INCLUDE_HARDWARE_HAL_POWER_CONTROL_H

#include <hardware.h>
#include <hs_type.h>

#define HAL_POWER_CONTROL_HARDWARE_MODULE_ID "hal_power_control"
#define HAL_POWER_CONTROL_HARDWARE_INTERFACE "hal_power_control_if"

#define HAL_POWER_CONTROL_MODULE_API_VERSION_1_0 HARDWARE_MODULE_API_VERSION(1, 0)
#define HAL_POWER_CONTROL_MODULE_API_VERSION_CURRENT HDMI_MODULE_API_VERSION_1_0

#define HAL_POWER_CONTROL_DEVICE_API_VERSION_1_0 HARDWARE_DEVICE_API_VERSION(1, 0)
#define HAL_POWER_CONTROL_DEVICE_API_VERSION_CURRENT HDMI_DEVICE_API_VERSION_1_0

#define PVR_RECORD_FILE "data/system/pvr_record.bin"
#define EPG_UPDATE_FILE "data/system/epg_update.bin"

typedef void (*send_keyevent_t)(int keyevent);

typedef struct pm_epg_update
{
    char   cEpg_update_before_power_on;
    char   cEpg_updating;
    char   cEpg_update_orderd;
    char   cEpg_error;
    long   iEpg_systemtime;
    long   iEpg_wakeuptime;
    long   iEpg_update_over_time;
}pm_epg_update_t;

typedef struct pm_pvr_record
{
    char   cPvr_record_before_power_on;
    char   cPvr_recording;
    char   cPvr_record_orderd;
    char   cPvr_error;
    long  iPvr_systemtime;
    long  iPvr_wakeuptime;
    long  iPvr_record_over_time;
}pm_pvr_record_t;

typedef struct hal_power_control_module {
    /**
     * Common methods of the power control device.  This *must* be the first member of
     * hal_power_control_device as users of this structure will cast a hw_device_t to hal_power_control_device
     * pointer in contexts where it's known the hw_device_t references a hal_power_control_device.
     */
    struct hw_module_t common;
} hal_power_control_module_t;


/*
 * POWER_CONTROL HAL interface definition.
 */
typedef struct hal_power_control_device {
    /**
     * Common methods of the power control device.  This *must* be the first member of
     * hal_power_control_device as users of this structure will cast a hw_device_t to hal_power_control_device
     * pointer in contexts where it's known the hw_device_t references a hal_power_control_device.
     */
    struct hw_device_t common;

    HS_S32 (*ControlPanel)(const struct hal_power_control_device* dev,bool enable);
    HS_S32 (*SetPowerenvToFlash)(const struct hal_power_control_device* dev,bool turnoff);
    HS_S32 (*SetPowerSequenceOfPanel)(const struct hal_power_control_device* dev,bool enable);
    HS_S32 (*TestPowerFunction)(const struct hal_power_control_device* dev,int step);
    HS_S32 (*GetPowerDownInformation)(const struct hal_power_control_device* dev,pm_epg_update_t *pEpgUpdate,pm_pvr_record_t *pPvrRecord);
    HS_S32 (*NormalStandby)(const struct hal_power_control_device* dev,long sleepintTime);
    HS_S32 (*GetWakeUpStatus)(const struct hal_power_control_device* dev,int* wakeUpStatus);
    HS_S32 (*StrStandby)(const struct hal_power_control_device* dev,long sleepingTime);
    HS_S32 (*CustomerSource)(const struct hal_power_control_device* dev);
    HS_S32 (*isDMPSource)(const struct hal_power_control_device* dev,int* dmpSource);
    HS_S32 (*SetRtcWakeUpEnvToFlash)(const struct hal_power_control_device* dev,bool enable);
    HS_S32 (*GetSystemStatus)(const struct hal_power_control_device* dev,int* sleeping);
    HS_S32 (*SetTvVolumeMute)(const struct hal_power_control_device* dev,bool muteEnable);
    HS_S32 (*Suspend)(const struct hal_power_control_device* dev,long sleepingTime);
    HS_S32 (*Resume)(const struct hal_power_control_device* dev,int powerType);
    HS_S32 (*Shutdown)(const struct hal_power_control_device* dev,long sleepingTime);
    /* Reserved for future use to maximum 16 functions. Must be NULL. */
    void* reserved[16 - 16];
} hal_power_control_device_t;

/** convenience API for opening and closing a device */

static inline int hal_power_open(const struct hw_module_t* module,
        struct hal_power_control_device** device) {
    return module->methods->open(module,
            HAL_POWER_CONTROL_HARDWARE_INTERFACE, (struct hw_device_t**)device);
}

static inline int hal_power_close(struct hal_power_control_device* device) {
    return device->common.close(&device->common);
}
#endif
