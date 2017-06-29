#include <hs_type.h>
#include <mi_common.h>
#include <mi_pm_datatype.h>
#include <mi_flash_datatype.h>
#include <mi_dispout_datatype.h>
#include <mi_pm.h>
#include <mi_flash.h>
#include <mi_gpio.h>
#include <mi_dispout.h>
#include <mif_internal_debug.h>
#include <mif_internal_handle.h>
#include <log.h>
#include <unistd.h>
#include <pthread.h>
#include <properties.h>

#include "hal_power_control.h"

#define LOG_TAG "HAL_POWER"



extern HS_S32 ReadDataFromFile(void *pDataStruct,const char *pFliePath,MI_U32 iDataLength);
extern HS_S32 WriteDataToFile(const void *pDataStruct,const char *pFliePath,MI_U32 iDataLength);

void test_pm_write()
{
    pm_epg_update_t sEpgUpdate;
    pm_pvr_record_t sPvrRecord;
    int iRet= 0;

    memset(&sEpgUpdate,0,sizeof(pm_epg_update_t));
    memset(&sPvrRecord,0,sizeof(pm_pvr_record_t));

    sEpgUpdate.cEpg_error = 1;
    sEpgUpdate.cEpg_update_before_power_on = 2;
    sEpgUpdate.cEpg_update_orderd = 3;//预约更新
    sEpgUpdate.cEpg_updating = 4;
    sEpgUpdate.iEpg_systemtime = 1230;
    sEpgUpdate.iEpg_update_over_time = 4560;
    sEpgUpdate.iEpg_wakeuptime = 7890;

    sPvrRecord.cPvr_error = 1;
    sPvrRecord.cPvr_recording = 2;
    sPvrRecord.cPvr_record_before_power_on = 3;
    sPvrRecord.cPvr_record_orderd = 4;//预约录制
    sPvrRecord.iPvr_record_over_time = 1230;
    sPvrRecord.iPvr_systemtime = 4560;
    sPvrRecord.iPvr_wakeuptime = 7890;

    iRet = WriteDataToFile(&sEpgUpdate,EPG_UPDATE_FILE,sizeof(pm_epg_update_t));
    iRet = WriteDataToFile(&sPvrRecord,PVR_RECORD_FILE,sizeof(pm_pvr_record_t));

}

void test_pm_read()
{
    pm_epg_update_t sEpgUpdate;
    pm_pvr_record_t sPvrRecord;

    ReadDataFromFile(&sEpgUpdate,EPG_UPDATE_FILE,sizeof(pm_epg_update_t));
    ReadDataFromFile(&sPvrRecord,PVR_RECORD_FILE,sizeof(pm_pvr_record_t));

    SLOGE("sEpgUpdate.cEpg_error = %d",sEpgUpdate.cEpg_error);
    SLOGE("sEpgUpdate.cEpg_update_before_power_on = %d",sEpgUpdate.cEpg_update_before_power_on);
    SLOGE("sEpgUpdate.cEpg_update_orderd = %d",sEpgUpdate.cEpg_update_orderd);
    SLOGE("sEpgUpdate.cEpg_updating = %d",sEpgUpdate.cEpg_updating);
    SLOGE("sEpgUpdate.iEpg_systemtime = %d",sEpgUpdate.iEpg_systemtime);
    SLOGE("sEpgUpdate.iEpg_update_over_time = %d",sEpgUpdate.iEpg_update_over_time);
    SLOGE("sEpgUpdate.iEpg_wakeuptime = %d",sEpgUpdate.iEpg_wakeuptime);

    SLOGE("sEpgUpdate.sPvrRecord.cPvr_error = %d",sPvrRecord.cPvr_error);
    SLOGE("sEpgUpdate.sPvrRecord.cPvr_recording = %d",sPvrRecord.cPvr_recording);
    SLOGE("sEpgUpdate.sPvrRecord.cPvr_record_before_power_on = %d",sPvrRecord.cPvr_record_before_power_on);
    SLOGE("sEpgUpdate.sPvrRecord.cPvr_record_orderd = %d",sPvrRecord.cPvr_record_orderd);
    SLOGE("sEpgUpdate.sPvrRecord.iPvr_record_over_time = %d",sPvrRecord.iPvr_record_over_time);
    SLOGE("sEpgUpdate.sPvrRecord.iPvr_systemtime = %d",sPvrRecord.iPvr_systemtime);
    SLOGE("sEpgUpdate.sPvrRecord.iPvr_wakeuptime = %d",sPvrRecord.iPvr_wakeuptime);

}

HS_S32 TestPowerFunction(const struct hal_power_control_device* dev,int step)
{
    if(dev==NULL)
    {
        SLOGE("Dev is NULL in SetenvToFlash");
        return HS_FAILURE;
    }

    switch(step)
    {
        case 1:
        {
            test_pm_write();
            SLOGE("test_pm_write");
            break;
        }
        case 2:
        {
            test_pm_read();
            SLOGE("test_pm_read");
            break;
        }
        default:
            break;
    }
    return HS_SUCCESS;
}
