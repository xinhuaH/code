
#include <log.h>
#include <unistd.h>
#include <pthread.h>
#include <properties.h>
#include <utils/RefBase.h>

#include <hstv_source_wrapper.h>
#include <hstv_system_wrapper.h>
#include <hstv_factory_wrapper.h>
#include <hstv_picture_wrapper.h>
#include <hstv_audio_wrapper.h>
#include <til_system.h>
#include <til_source.h>

#include "hal_power_control.h"

#define LOG_TAG "HAL_POWER"

#define en_debug_level EN_INFO

#define POWERDOWN_REASON "powerdown_reason"
#define BACKLIGHT_OFF "backlight_off"
#define DC_POWEROFF "dc_poweroff"

using namespace android;


static int bSystemSleeping = 0;
static HS_S32 iSourceNeedResume = 0;

//extern void STR_allocate_all_resource();
//extern void STR_release_all_resource();

static void resetAmpGain();

static HS_S32 SetEnvToFlash(unsigned char *pEnvName,unsigned char *pEnvValue)
{

    return HS_SUCCESS;
}

static HS_S32 GetEnvFromFlash(unsigned char *pEnvName,unsigned char *pEnvValue)
{

    return HS_SUCCESS;
}

static HS_S32 ControlBacklight(bool enable)
{
    SLOGE("hsTVsetBacklightEnable: %d", enable);
    HS_S32 ret = 0;
    ret = hsTVsetBacklightEnable(enable ? HS_TRUE : HS_FALSE);
    if(!enable) {
        //Power off panel when suspend.
        //When resume happend, this will be done in MBOOT, so don't need here.
        SLOGE("hsTVsetPanelPowerOn: %d", enable);
        ret = hsTVsetPanelPowerOn(enable ? HS_TRUE : HS_FALSE);
    }
    /*
    if(!enable) {
        //STR_allocate_all_resource();
    }
    else
    {
        STR_release_all_resource();
    }*/
    if(!enable)
    {
         //STR_allocate_all_resource();
         hsTVsetBreathLEDOff(HS_FALSE);
    }
    else
    {
         hsTVsetBreathLEDOff(HS_TRUE);
         //zhangyongfang add for do in MW resume
         //resetAmpGain(); //Add by lichengcheng for zhangyongfang.
    }
    return HS_SUCCESS;
}

static HS_S32 SetRtcEnv(bool enable)
{
    HS_S32 ret = 0;

    return ret;
}

HS_S32 NormalStandby(const struct hal_power_control_device* dev,long sleepintTime)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in PM_StrStandby");
        return HS_FAILURE;
    }

    if(hsTVsetTVPower(TIL_POWER_STATUS_STANDBY))
    {
        SLOGE("enter tv power down error");
        return HS_FAILURE;
    }
    SLOGE("Normal standby sucess");
    return HS_SUCCESS;
}

HS_S32 CustomerSource(const struct hal_power_control_device* dev)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in CustomerSource");
        return HS_FAILURE;
    }

    SLOGE("CustomerSource");
    til_input_source_t inputSource = TIL_INPUT_SOURCE_TYPE_CUSTOM;
    if(hsTVsetInputSource(inputSource))
    {
        SLOGE("Set CustomerSource error");
        return HS_FAILURE;
    }
    SLOGE("Set CustomerSource sucess");
    return HS_SUCCESS;
}

HS_S32 StrStandby(const struct hal_power_control_device* dev,long sleepingTime)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in PM_StrStandby");
        return HS_FAILURE;
    }

    //CustomerSource(dev);
    // STR_allocate_all_resource();

    if(hsTVenterSTRMode())
    {
        SLOGE("enter tv power down error");
        return HS_FAILURE;
    }
   SLOGE("STR standby sucess");
   return HS_SUCCESS;
}


static HS_S32 GetWakeUpStatus(const struct hal_power_control_device* dev,int* wakeUpStatus)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in GetWakeUpStatus");
        return HS_FAILURE;
    }

    //hsTVgetWakeUpStatus(wakeUpStatus);

    //SLOGE("GetWakeUpStatus is %d",*wakeUpStatus);

    return HS_SUCCESS;
}

static HS_S32 GpioInit()
{
    return HS_SUCCESS;
}

HS_S32 ControlPanel(const struct hal_power_control_device* dev,bool enable)
{
    HS_S32 ret = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in ControlBacklight");
        return HS_FAILURE;
    }

    ret = ControlBacklight(enable);

    SLOGE("Set backlight %d,ret = %d",enable,ret);

    return HS_SUCCESS;
}

static HS_S32 FlashInit()
{

    return HS_SUCCESS;
}
static HS_S32 BacklightInit()
{

    return HS_SUCCESS;
}

//lichengcheng Add for zhangyongfang
//Fix bug: MSD6A828-4965
//zhangyongfang add for do in MW resume
static void resetAmpGain()
{
    //int value;
    //hsTVgetAMPGain(TIL_AUDIO_PAGAIN, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_PAGAIN, 1, &value);

    //hsTVgetAMPGain(TIL_AUDIO_OUTPORT, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_OUTPORT, 1, &value);

    //hsTVgetAMPGain(TIL_AUDIO_DRC_L, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_DRC_L, 1, &value);

    //hsTVgetAMPGain(TIL_AUDIO_DRC_H, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_DRC_H, 1, &value);

    //hsTVgetAMPGain(TIL_AUDIO_DRC_S, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_DRC_S, 1, &value);

    //hsTVgetAMPGain(TIL_AUDIO_DRC_P, 1, &value);
    //hsTVsetAMPGain(TIL_AUDIO_DRC_P, 1, &value);

    //hsTVaudioStrInit();
}

HS_S32 SetPowerenvToFlash(const struct hal_power_control_device* dev,bool turnoff)
{
    HS_S32 ret = 0;

    return ret;
}

HS_S32 SetRtcWakeUpEnvToFlash(const struct hal_power_control_device* dev,bool enable)
{
    HS_S32 ret = 0;

    return ret;
}

HS_S32 GetSystemStatus(const struct hal_power_control_device* dev,int* sleeping)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in SetRtcWakeUpEnvToFlash");
        return HS_FAILURE;
    }

    *sleeping = bSystemSleeping;

    bSystemSleeping = 0;

    return HS_SUCCESS;
}

HS_S32 SetTvVolumeMute(const struct hal_power_control_device* dev,bool muteEnable)
{
    HS_BOOL volumeEnable;
    HS_S32 ret = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in SetTvMute");
        return HS_FAILURE;
    }

    volumeEnable = muteEnable ? HS_TRUE:HS_FALSE;

    ret = hsTVsetMuteWithType(volumeEnable,11);

    SLOGE("hsTVsetMuteWithType volumeEnable is:%d,ret is:%d",volumeEnable,ret);

    ret = hsTVsetMute(volumeEnable);

    SLOGE("SetTvMute volumeEnable is:%d,ret is:%d",volumeEnable,ret);

    return ret;
}

HS_S32 WriteDataToFile(const void *pDataStruct,const char *pFliePath,unsigned int iDataLength)
{
    return HS_SUCCESS;
}

HS_S32 ReadDataFromFile(void *pDataStruct,const char *pFliePath,unsigned int iDataLength)
{
    return HS_SUCCESS;
}

static HS_S32 SetPowerSequenceOfPanel(const struct hal_power_control_device* dev,bool enable)
{
    return HS_SUCCESS;
}

static HS_S32 PanelInit(void)
{
    return HS_SUCCESS;
}

HS_S32 GetPowerDownInformation(const struct hal_power_control_device* dev,pm_epg_update_t *pEpgUpdate,pm_pvr_record_t *pPvrRecord)
{

    return HS_SUCCESS;
}

HS_S32 isDMPSource(const struct hal_power_control_device* dev,int* dmpSource)
{
    HS_S32 ret = 0;
    HS_S32 currentSource = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in GetSuspendSource");
        return HS_FAILURE;
    }

    ret = hsTVgetCurInputSource(&currentSource);

    if(ret)
    {
        SLOGE("Get current input source error");
        return HS_FAILURE;
    }

    SLOGE("Get current input source:0x%x",currentSource);

    if(currentSource != TIL_INPUT_SOURCE_TYPE_CUSTOM)
    {
        SLOGE("Source isn't DMP");
        *dmpSource = 0;
    }
    else
    {
        *dmpSource = 1;
    }

    return HS_SUCCESS;
}

HS_S32 Suspend(const struct hal_power_control_device* dev,long sleepingTime)
{
    HS_S32 ret = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in Suspend");
        return HS_FAILURE;
    }

    hsTVsetTVSuspend(sleepingTime);

    return HS_SUCCESS;
}
HS_S32 Resume(const struct hal_power_control_device* dev,int powerType)
{
    HS_S32 ret = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in Resume");
        return HS_FAILURE;
    }

    hsTVsetTVResume(powerType);

    return HS_SUCCESS;
}
HS_S32 Shutdown(const struct hal_power_control_device* dev,long sleepingTime)
{
    HS_S32 ret = 0;

    if(dev==NULL)
    {
        SLOGE("Dev is NULL in ShutDown");
        return HS_FAILURE;
    }

    hsTVsetTVShutDown(sleepingTime);

    return HS_SUCCESS;
}

static int PowerControlOpen(const struct hw_module_t* module,const char* name,
                              struct hw_device_t** device)
{
    if (!strcmp(name,HAL_POWER_CONTROL_HARDWARE_INTERFACE))
        {
            //SLOGE("power_control_open sucess\n");

            hal_power_control_device *dev = (hal_power_control_device*) malloc(sizeof(hal_power_control_device));

            /* initialize our state here */
            memset(dev, 0, sizeof(*dev));

            /* initialize the procs */
            dev->common.tag = HARDWARE_DEVICE_TAG;
            dev->common.version = HAL_POWER_CONTROL_DEVICE_API_VERSION_1_0;
            dev->common.module = const_cast<hw_module_t*>(module);
            //dev->common.close = msmhl_close;

            dev->ControlPanel = ControlPanel;
            dev->SetPowerenvToFlash = SetPowerenvToFlash;
            dev->SetPowerSequenceOfPanel = SetPowerSequenceOfPanel;
//            dev->TestPowerFunction = TestPowerFunction;
            dev->GetPowerDownInformation = GetPowerDownInformation;
            dev->NormalStandby = NormalStandby;
            dev->GetWakeUpStatus = GetWakeUpStatus;
            dev->isDMPSource = isDMPSource;
            dev->StrStandby = StrStandby;
            dev->SetRtcWakeUpEnvToFlash = SetRtcWakeUpEnvToFlash;
            dev->GetSystemStatus = GetSystemStatus;
            dev->SetTvVolumeMute = SetTvVolumeMute;
            dev->CustomerSource = CustomerSource;
            dev->Suspend = Suspend;
            dev->Resume = Resume;
            dev->Shutdown = Shutdown;
            *device = &dev->common;
        }
    else
        {
            SLOGE("power_control_open fail");
            return HS_FAILURE;
        }

      //GpioInit();
      //FlashInit();
      //PanelInit();
      //BacklightInit();


      SLOGE("PowerControlOpen");

      return HS_SUCCESS;
}

static struct hw_module_methods_t hal_power_control_module_methods = {
    .open = PowerControlOpen,
};

struct hal_power_control_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = HAL_POWER_CONTROL_HARDWARE_MODULE_ID,
        .name = "Mstar 8488 POWER CONTROL HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_power_control_module_methods,
    },
};

