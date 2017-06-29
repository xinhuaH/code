/*********************************************************************
* ��Ȩ���� (C)2016, �ൺ���ŵ����ɷ����޹�˾
* �ļ�����: hdmi_mhl.cpp
* �ļ���ʶ: ��
* ����ժҪ: ʵ����MHL����RCP���ܣ��źż�⣬״̬���
* ����˵��:
* ��ǰ�汾: V3.0
* ��    ��: chenweilan
* �������: 2016��7��24��
*
**********************************************************************/

#define LOG_TAG "HDMI_MHL"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <cutils/log.h>
#include <android/keycodes.h>
#include <hardware/hdmi_mhl.h>
#include <cutils/native_handle.h>
#include "hs_hal_mhl.h"
#include "cutils/properties.h"  // for property_set




/**********************************************************************
*                             �궨��                                  *
**********************************************************************/
#define MHL_STACK_SIZE                   (4096)
#define HDMI_PORT_NUM_MAX                (3)
#define MHL_PORT_INDEX                   (1)
#define MHL_SOURCE_INDEX                 (0)
#define SEP_MHL_PORT_INDEX               (2)
#define SEP_MHL_SOURCE_INDEX             (1)

#define PTH_RET_CHK_MHL(_pf_) \
    ({ \
        HS_S32 r = _pf_; \
        if ((r != 0) && (r != ETIMEDOUT)) \
            SLOGV("[PTHREAD] %s: %d: %s\n", __FILE__, __LINE__, strerror(r)); \
         r; \
    })

/* ����MHL PORT�������Ϣ*/
typedef struct hisense_mhl_port
{
    HS_HANDLE exthandle;
    E_HS_EXTIN_SIGNAL_STATUS eSignal;
    mhl_cable_link_e eCablink;
    mhl_cbus_status_e eCbus;
}T_HisenseMhlPort;

typedef struct hisense_hdmi_mhl_device
{
    hdmi_mhl_device_t tDevice;
    HS_HANDLE mhlhandle;
    pthread_t mhl_tid;
    HS_BOOL threadexit;
    HS_S32 option_enable;
    mhl_event_cb_t pCallBack;
    void* pCallbackParam;
    T_HisenseMhlPort tMhlPortInfo;
    struct hdmi_mhl_port_info atHdmiPortInfo[HDMI_PORT_NUM_MAX];
}T_HisenseHdmiMhlDevice;


/**********************************************************************
* ��������: HsMhlValInit
* ��������: ��ʼ��MHL�ı���
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static void HsMhlValInit(T_HisenseHdmiMhlDevice *ptDev)
{
    char acTvSeperate[PROPERTY_VALUE_MAX];

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }

    ptDev->mhlhandle      = HS_INVALID_HANDLE;
    ptDev->mhl_tid        = 0;
    ptDev->pCallBack      = NULL;
    ptDev->pCallbackParam = NULL;
    /* �������ַ�����Ӻ�һ���*/
    property_get("ro.hisense.tv.seperate", acTvSeperate, "0");
    if (strcmp(acTvSeperate, "1") == 0)
    {
        ptDev->atHdmiPortInfo[0].port_id       = 1;
        ptDev->atHdmiPortInfo[0].mhl_supported = 0;

        ptDev->atHdmiPortInfo[1].port_id       = 2;
        ptDev->atHdmiPortInfo[1].mhl_supported = 1;

        ptDev->atHdmiPortInfo[2].port_id       = 3;
        ptDev->atHdmiPortInfo[2].mhl_supported = 0;
    }
    else
    {
        ptDev->atHdmiPortInfo[0].port_id       = 1;
        ptDev->atHdmiPortInfo[0].mhl_supported = 1;

        ptDev->atHdmiPortInfo[1].port_id       = 2;
        ptDev->atHdmiPortInfo[1].mhl_supported = 0;

        ptDev->atHdmiPortInfo[2].port_id       = 3;
        ptDev->atHdmiPortInfo[2].mhl_supported = 0;
    }

    ptDev->tMhlPortInfo.exthandle = HS_INVALID_HANDLE;
    ptDev->tMhlPortInfo.eCablink  = MHL_CABLE_LINK_NO;
    ptDev->tMhlPortInfo.eCbus     = MHL_CBUS_NO;
}

/**********************************************************************
* ��������: HsMhlInstallEnv
* ��������: ����MHL��������ȡ������ڻ�ȡ�ź�״̬
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlInstallEnv(T_HisenseHdmiMhlDevice *ptDev)
{
    HS_S32 s32Status       = -EINVAL;
    HS_U32 u32MhlPortIndex = 1;
    HS_U32 u32SourceIndex  = 0;
    HS_HAL_RESULT res      = HS_HAL_ERR_FAILED;
    HS_HANDLE mhlhandle    = HS_INVALID_HANDLE;
    HS_HANDLE extinhandle  = HS_INVALID_HANDLE;
    HS_MHL_InitParams_t stMhlInitParams;
    HS_MHL_OpenParams_t stMhlOpenParams;
    HS_MHL_CallbackParams_t stMhlCallParams;
    char acTvSeperate[PROPERTY_VALUE_MAX];
    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    INIT_ST(stMhlInitParams);
    if (HS_HAL_ERR_FAILED == HS_HAL_MHL_Init(&stMhlInitParams))
    {
        SLOGE("MI MHL Init Failed \n");
        return s32Status;
    }
    /* ����MHL�˿�*/
    INIT_ST(stMhlOpenParams);
    property_get("ro.hisense.tv.seperate", acTvSeperate, "0");
    if (strcmp(acTvSeperate, "1") == 0)
    {
        stMhlOpenParams.u32MhlPortIndex = SEP_MHL_PORT_INDEX;
        u32MhlPortIndex                 = SEP_MHL_PORT_INDEX;
        u32SourceIndex                  = SEP_MHL_SOURCE_INDEX;
    }
    else
    {
        u32MhlPortIndex                 = MHL_PORT_INDEX;
        stMhlOpenParams.u32MhlPortIndex = MHL_PORT_INDEX;
        u32SourceIndex                  = MHL_SOURCE_INDEX;
    }
    /* ���MHL�ľ��*/
    res = HS_HAL_MHL_Open(&stMhlOpenParams, &ptDev->mhlhandle);
    if (HS_HAL_ERR_OK != res)
    {
        res = HS_HAL_MHL_GetHandle(u32MhlPortIndex, &mhlhandle);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("HS MHL Get Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        res = HS_HAL_MHL_DupHandle(mhlhandle, HS_TRUE, &ptDev->mhlhandle);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("MI MHL Dup Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        SLOGE("MI MHL Get Handle Successful. MHL handle[%#x] \n", ptDev->mhlhandle);
    }
    /* ����vbus���*/
    res = HS_HAL_MHL_SetVbusControl(ptDev->mhlhandle, E_HS_MHL_Vbus_SW_Charge_OFF);
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("HS_HAL_MHL_SetVbusControl Failed, res[%d]. SW Charge off \n", res);
        return s32Status;
    }

    res = HS_HAL_MHL_SetVbusControl(ptDev->mhlhandle, E_HS_MHL_Vbus_HW_DETECT);
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("HS_HAL_MHL_SetVbusControl Failed, res[%d]. HW detect \n", res);
        return s32Status;
    }

    ptDev->tMhlPortInfo.eSignal = E_HS_SIGNAL_STATUS_NOTSUPPORT;

    res = HS_HAL_EXTIN_Open(E_HS_EXTIN_HDMI, u32SourceIndex, &ptDev->tMhlPortInfo.exthandle);
    if (HS_HAL_ERR_OK != res)
    {

        SLOGE("MI extin Open Failed. res[%d] \n", res);
        res = HS_HAL_EXTIN_GetHandle(E_HS_EXTIN_HDMI, u32SourceIndex, &extinhandle);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("MI extin Get Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        res = HS_HAL_EXTIN_DupHandle(extinhandle,HS_TRUE,&ptDev->tMhlPortInfo.exthandle);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("HS MHL Dup Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        SLOGE("HS extin Get Handle Successful. res[%d] \n", res);
    }
    stMhlCallParams.pfEventCallback = NULL;
    stMhlCallParams.pUsrParams = NULL;
    if (HS_HAL_ERR_OK != HS_HAL_MHL_RegisterCallback(ptDev->mhlhandle, &stMhlCallParams))
    {
        SLOGE("HS MHL Register callback Failed \n");
        return s32Status;
    }

    if (HS_HAL_ERR_OK != HS_HAL_MHL_SourceChange(ptDev->mhlhandle, ptDev->tMhlPortInfo.exthandle))
    {
        SLOGE("MI_MHL_SourceChange Failed \n");
        return s32Status;
    }
    return 0;
}

/**********************************************************************
* ��������: HsMhlUninstallEnv
* ��������: ж��MHL����
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlUninstallEnv(T_HisenseHdmiMhlDevice *ptDev)
{
    HS_S32 s32Status = -EINVAL;

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    if (HS_INVALID_HANDLE != ptDev->mhlhandle)
    {
        if (HS_HAL_ERR_OK != HS_HAL_MHL_Close(ptDev->mhlhandle))
        {
            SLOGE("MI MHL Close Failed \n");
            return s32Status;
        }
        ptDev->mhlhandle = HS_INVALID_HANDLE;
    }

    if (HS_INVALID_HANDLE != ptDev->tMhlPortInfo.exthandle)
    {
        if (HS_HAL_ERR_OK != HS_HAL_EXTIN_Close(ptDev->tMhlPortInfo.exthandle))
        {
            SLOGE("MI Extin Close Failed \n");
            return s32Status;
        }
        ptDev->tMhlPortInfo.exthandle = HS_INVALID_HANDLE;
    }

    if (HS_HAL_ERR_OK != HS_HAL_MHL_DeInit())
    {
        SLOGE("HS MHL DeInit Failed \n");
        return s32Status;
    }

    if (HS_HAL_ERR_OK != HS_HAL_EXTIN_DeInit())
    {
        SLOGE("HS Extin DeInit Failed \n");
        return s32Status;
    }

    return 0;
}

/**********************************************************************
* ��������: HsMhlPortCheck
* ��������: ֹͣMHL�豸
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsMhlPortCheck(T_HisenseHdmiMhlDevice *ptDev)
{
    E_HS_EXTIN_SIGNAL_STATUS eSignalStatus = E_HS_SIGNAL_STATUS_NOTSUPPORT;
    HS_HAL_RESULT res = HS_HAL_ERR_FAILED;

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }
    /* �Ծ�������ж�*/
    if (HS_INVALID_HANDLE == ptDev->mhlhandle || HS_INVALID_HANDLE == ptDev->tMhlPortInfo.exthandle)
    {
        SLOGE("invalid handle \n");
        return;
    }

    res = HS_HAL_EXTIN_GetSignalStatus(ptDev->tMhlPortInfo.exthandle, &eSignalStatus);
    if (HS_HAL_ERR_OK !=  res)
    {
        SLOGE("HDMI port signal get Failed. res[%d] \n", res);
    }
    if ((E_HS_SIGNAL_STATUS_NOTSUPPORT == eSignalStatus || E_HS_SIGNAL_STATUS_UNLOCK == eSignalStatus) &&
        (E_HS_SIGNAL_STATUS_LOCK == ptDev->tMhlPortInfo.eSignal))
    {
        /*�����źŵ����ź�*/
        SLOGE("HDMI No stream input \n");
        ptDev->tMhlPortInfo.eSignal = eSignalStatus;
        ptDev->tMhlPortInfo.eCbus   = MHL_CBUS_NO;
        if (ptDev->pCallBack)
        {
            mhl_event_t event;

            event.type = MHL_EVENT_HDMI_STREAM_STATUS;
            event.hdmistreamstatus = MHL_HDMI_STREAM_NULL;
            ptDev->pCallBack(&event, ptDev->pCallbackParam);
        }
    }
    else if ((E_HS_SIGNAL_STATUS_LOCK == eSignalStatus) &&
        (E_HS_SIGNAL_STATUS_NOTSUPPORT == ptDev->tMhlPortInfo.eSignal || E_HS_SIGNAL_STATUS_UNLOCK == ptDev->tMhlPortInfo.eSignal))
    {
        /* �����źŵ����ź�*/
        ptDev->tMhlPortInfo.eSignal = eSignalStatus;
        SLOGE("HDMI has stream input \n");
        ptDev->tMhlPortInfo.eCbus = MHL_CBUS_OK;

        if (ptDev->pCallBack)
        {
            mhl_event_t event;

            event.type = MHL_EVENT_HDMI_STREAM_STATUS;
            event.hdmistreamstatus = MHL_HDMI_STREAM_INPUT;
            ptDev->pCallBack(&event, ptDev->pCallbackParam);
        }
    }

    /*
    res = HS_HAL_MHL_GetCbusStatus(ptDev->mhlhandle);
    if (MHL_CBUS_NO == ptDev->tMhlPortInfo.eCbus && HS_HAL_ERR_OK == res)
    {
        ptDev->tMhlPortInfo.eCbus = MHL_CBUS_OK;
        SLOGE("MHL Cbus Connect. res[%d] \n", res);

        if (ptDev->pCallBack)
        {
            mhl_event_t event;

            event.type = MHL_EVENT_CBUS_STATUS;
            event.cbus= MHL_CBUS_OK;
            ptDev->pCallBack(&event, ptDev->pCallbackParam);
        }
    }
    else if (MHL_CBUS_OK == ptDev->tMhlPortInfo.eCbus && HS_HAL_ERR_FAILED == res)
    {
        ptDev->tMhlPortInfo.eCbus = MHL_CBUS_NO;
        SLOGE("MHL Cbus Disconnect. res[%d] \n", res);

        if (ptDev->pCallBack)
        {
            mhl_event_t event;

            event.type = MHL_EVENT_CBUS_STATUS;
            event.cbus = MHL_CBUS_NO;
            ptDev->pCallBack(&event, ptDev->pCallbackParam);
        }
    }
 */
}

/**********************************************************************
* ��������: HsMhlMornitor
* ��������: ���MHL״̬���߳�
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void* HsMhlMornitor(void *ptDev)
{
    T_HisenseHdmiMhlDevice *ptDevice = (T_HisenseHdmiMhlDevice  *) ptDev;
    SLOGE("mif_mhl_mornitor enter \n");

    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return NULL;
    }

    while (!ptDevice->threadexit)
    {
        HsMhlPortCheck(ptDevice);
        usleep(400000);
    }

    SLOGE("hs_mhl_mornitor exit \n");
    return NULL;
}

/**********************************************************************
* ��������: HsMhlStart
* ��������: ����MHL�豸
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlStart(T_HisenseHdmiMhlDevice *ptDev)
{
    pthread_attr_t thread_attr;
    HS_S32 s32Status = -EINVAL;

    SLOGE("mif_mhl_start enter \n");
    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    ptDev->threadexit = HS_FALSE;

    if (PTH_RET_CHK_MHL(pthread_attr_init(&thread_attr)))
    {
        SLOGE("pthread_attr_init failed \n");
        return s32Status;
    }

    //thread will not exit after return until join
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    pthread_attr_setstacksize(&thread_attr, MHL_STACK_SIZE);
    if (PTH_RET_CHK_MHL(pthread_create(&ptDev->mhl_tid,
                               &thread_attr,
                               HsMhlMornitor,
                               ptDev)))
    {
        SLOGE("pthread_create failed \n");
        return s32Status;
    }
    pthread_attr_destroy(&thread_attr);
    SLOGE("mif_mhl_start exit \n");
    return 0;
}

/**********************************************************************
* ��������: HsMhlStop
* ��������: ֹͣMHL�豸
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlStop(T_HisenseHdmiMhlDevice *ptDev)
{
    HS_S32 s32Status = -EINVAL;
    void *pResult = NULL;
    SLOGE("mif_mhl_stop enter \n");

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }
    ptDev->threadexit = HS_TRUE;
    PTH_RET_CHK_MHL(pthread_join(ptDev->mhl_tid, &pResult));

    SLOGE("mif_mhl_stop exit\n");
    return 0;
}

/**********************************************************************
* ��������: HsMhlClose
* ��������: �ر�MHL�豸
* �������: ptDev:Ӳ�������豸
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlClose(struct hw_device_t *ptDev)
{
    T_HisenseHdmiMhlDevice *ptPriv = (T_HisenseHdmiMhlDevice *) ptDev;
    HS_S32 s32Status = -EINVAL;
    SLOGE("HsMhl_close enter \n");

    if (NULL == ptPriv)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    (void) HsMhlStop(ptPriv);
    (void) HsMhlUninstallEnv(ptPriv);

    if (ptPriv)
    {
        free(ptPriv);
    }

    SLOGE("HsMhl_close exit \n");
    return 0;
}

/**********************************************************************
* ��������: HsMhlSendIrkeyEvent
* ��������: ����ir key���ײ�
* �������: s32Key��ֵ
* �������: ��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsMhlSendIrkeyEvent(const struct hdmi_mhl_device *ptDev, HS_S32 s32Key)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *) ptDev;
    HS_U32 tIrKey = HS_UNF_HDMIRX_RCP_KEY_SELECT/*E_HS_MVK_SELECT*/;

    SLOGE("HsMhlSendIrkeyEvent. key[%d] \n", s32Key);
    if (NULL == ptDevice)
    {
        SLOGE("invalid parameter \n");
        return;
    }
    if (HS_INVALID_HANDLE == ptDevice->mhlhandle)
    {
        SLOGE("invalid mhl handle \n");
        return;
    }
    /*��android ����תΪ�ײ㰴��*/
    tIrKey =  HS_HAL_MHL_KeyAntoMs(s32Key);

    if (tIrKey > HS_UNF_HDMIRX_RCP_KEY_BUTT/*E_HS_MVK_STOP_FUNC*/)
    {
        SLOGE("Unsupported IrKey[%d] \n", s32Key);
        return;
    }

    if (HS_HAL_ERR_OK != HS_HAL_MHL_IRKeyProcess(ptDevice->mhlhandle, tIrKey))
    {
        SLOGE("MI_MHL_IRKeyProcess Failed \n");
        return;
    }

    return;
}

/**********************************************************************
* ��������: HsMhlGetVersion
* ��������: ��ȡMHL�汾��Ϣ
* �������:
* �������: s32Version�汾��
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsMhlGetVersion(const struct hdmi_mhl_device *ptDev, HS_S32 *s32Version)
{
    SLOGE("HsMhl_get_version \n");
    if (NULL == s32Version || NULL == ptDev)
    {
        SLOGE("invalid parameter \n");
        return;
    }
     /* MHL specification version 2.0 returns 0x20, 3.0 will return 0x30. */
    *s32Version = 0x30;
    return;
}

/**********************************************************************
* ��������: HsMhlGetPortInfo
* ��������: ��ȡMHL  port ��Ϣ
* �������:
* �������: port ��Ϣ
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsMhlGetPortInfo(const struct hdmi_mhl_device *ptDev,
            struct hdmi_mhl_port_info* aptList[], HS_S32 *s32Total)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *) ptDev;

    SLOGE("HsMhl_get_port_info enter \n");
    if (NULL == ptDevice)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    *aptList  = &ptDevice->atHdmiPortInfo[0];
    *s32Total = HDMI_PORT_NUM_MAX;
    SLOGE("HsMhl_get_port_info exit \n");
    return;
}

/**********************************************************************
* ��������: HsMhlSetOption
* ��������: ѡ������
* �������: s32Flag ��Ҫ���õ�ѡ� s32Value ��Ҫ���õ�ֵ
* �������:
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsMhlSetOption(const struct hdmi_mhl_device* ptDev, HS_S32 s32Flag, HS_S32 s32Value)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *) ptDev;
    if (NULL == ptDevice)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    SLOGE("HsMhlSetOption flag[%d], value[%d] \n", s32Flag, s32Value);

    switch (s32Flag)
    {
        case MHL_OPTION_INPUT_SWITCHING:
        {
            break;
        }
        /*��������Ƴ�繦��*/
        case MHL_OPTION_POWER_CHARGE:
        {
            #if 0
            MI_MHL_VbusCrtl_Type_e type = E_MI_MHL_Vbus_SW_Charge_OFF;

            if (value) {
                type = E_MI_MHL_Vbus_SW_Charge_OFF;
            }
            else {
                type = E_MI_MHL_Vbus_SW_Charge_ON;
            }
            if (MI_OK != MI_MHL_SetVbusControl(device->mhlhandle, type)) {
                SLOGE("MI_MHL_SetVbusControl Failed \n");
                return;
            }
            #endif
            break;
        }
        case MHL_OPTION_ENABLE:
        {
            ptDevice->option_enable = s32Value;
            break;
        }
        case MHL_OPTION_SERVICE_CONTROL:
        {
            break;
        }
        default:
        {
            SLOGE("HsMhl_set_option Failed. Unsupport flag[%d] \n", s32Flag);
            break;
        }
    }
    return;
}

/**********************************************************************
* ��������: HsMhlIsCbusConnected
* ��������: �ж�MHL source ���Ƿ�����
* �������: s32PortId �˿�
* �������: 0 δ���� 1������
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlIsCbusConnected(const struct hdmi_mhl_device* ptDev, HS_S32 s32PortId)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *)ptDev;
    SLOGE("MHL get cbus connect status of port[%d] \n", s32PortId);
    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return MHL_CBUS_NO;
    }
    if (1 != s32PortId)
    {
        SLOGE("MHL port_id is invalid. Only port 1 support MHL.  \n");
        return MHL_CBUS_NO;
    }
    if (MHL_CBUS_OK == ptDevice->tMhlPortInfo.eCbus)
    {
        SLOGE("MHL cbus is connect \n");
        return MHL_CBUS_OK;
    }
    SLOGE("MHL cbus is disconnect \n");
    return MHL_CBUS_NO;
}

/**********************************************************************
* ��������: HsMhlIsCableLinked
* ��������: �ж�MHL cable�Ƿ�����
* �������: s32PortId �˿�
* �������: 0 δ���� 1������
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlIsCableLinked(const struct hdmi_mhl_device *ptDev, HS_S32 s32PortId)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *) ptDev;
    SLOGE("MHL get cable linked status of port[%d] \n", s32PortId);
    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return MHL_CABLE_LINK_NO;
    }
    if (1 != s32PortId)
    {
        SLOGE("port_id is invalid. Only port 1 support MHL.  \n");
        return MHL_CABLE_LINK_NO;
    }
      if (MHL_CABLE_LINK_OK == ptDevice->tMhlPortInfo.eCablink)
    {
        SLOGE("MHL cable is linked \n");
        return MHL_CBUS_OK;
    }
    SLOGE("MHL cable is not linked \n");
    return MHL_CABLE_LINK_NO;
}

/**********************************************************************
* ��������: HsMhlIsHdmiStreamInput
* ��������: �ж�MHL �˿��Ƿ�������
* �������: s32PortId �˿�
* �������: 0 ������ 1������
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlIsHdmiStreamInput(const struct hdmi_mhl_device *ptDev, HS_S32 s32PortId)
{
    T_HisenseHdmiMhlDevice* ptDevice = (T_HisenseHdmiMhlDevice *)ptDev;
    SLOGE("MHL get hdmi stream status of port[%d] \n", s32PortId);
    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return MHL_HDMI_STREAM_NULL;
    }
    if (E_HS_SIGNAL_STATUS_LOCK == ptDevice->tMhlPortInfo.eSignal)
    {
        SLOGE("MHL hdmi has stream input \n");
        return MHL_HDMI_STREAM_INPUT;
    }
    SLOGE("MHL hdmi has no stream input \n");
    return MHL_HDMI_STREAM_NULL;
}

/**********************************************************************
* ��������: HsMhlRegisterEventCallback
* ��������: ע��ص������ӿ�
* �������: pCallback �ص�����pArg����
* �������: 0 ������ 1������
* �� �� ֵ: ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static int HsMhlRegisterEventCallback(const struct hdmi_mhl_device *ptDev, mhl_event_cb_t pCallback, void *pArg)
{
    T_HisenseHdmiMhlDevice *ptDevice = (T_HisenseHdmiMhlDevice *) ptDev;
    SLOGE("MHL register callback \n");

    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return MHL_HDMI_STREAM_NULL;
    }

    if (pCallback)
    {
        ptDevice->pCallBack      = pCallback;
        ptDevice->pCallbackParam = pArg;
    }
    return MHL_HDMI_STREAM_NULL;
}

/**********************************************************************
* ��������: HsMhlOpen
* ��������������open
* ���������ptModule : Ӳ������ģ�� pcName :ģ������ppDevice:Ӳ�������豸
* �����������
* �� �� ֵ�����״̬0:�ɹ�   ����:ʧ��
* ����˵���������Բ�ͬ��Ӳ������������
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static HS_S32 HsMhlOpen(const struct hw_module_t *ptModule,
        const char *pcName, struct hw_device_t** ppDevice)
{
    HS_S32 s32Status = -EINVAL;

    SLOGE("HsMhl_open enter \n");
    if (!strcmp(pcName, HDMI_MHL_HARDWARE_INTERFACE))
    {
        T_HisenseHdmiMhlDevice *ptDev = (T_HisenseHdmiMhlDevice*) malloc(sizeof(*ptDev));
        if (ptDev == NULL)
        {
            return s32Status ;
        }

        /* initialize our state here */
        memset(ptDev, 0, sizeof(*ptDev));

        /* initialize the procs */
        ptDev->tDevice.common.tag              = HARDWARE_DEVICE_TAG;
        ptDev->tDevice.common.version          = HDMI_MHL_DEVICE_API_VERSION_1_0;
        ptDev->tDevice.common.module           = const_cast<hw_module_t*>(ptModule);
        ptDev->tDevice.common.close            = HsMhlClose;

        ptDev->tDevice.get_version             = HsMhlGetVersion;
        ptDev->tDevice.get_port_info           = HsMhlGetPortInfo;
        ptDev->tDevice.set_option              = HsMhlSetOption;
        ptDev->tDevice.is_cbus_connected       = HsMhlIsCbusConnected;
        ptDev->tDevice.is_cable_linked         = HsMhlIsCableLinked;
        ptDev->tDevice.send_irkey_event        = HsMhlSendIrkeyEvent;
        ptDev->tDevice.is_hdmi_stream_input    = HsMhlIsHdmiStreamInput;
        ptDev->tDevice.register_event_callback = HsMhlRegisterEventCallback;

        HsMhlValInit(ptDev);
        s32Status  = HsMhlInstallEnv(ptDev);
        s32Status |= HsMhlStart(ptDev);
        *ppDevice  = &ptDev->tDevice.common;
    }

    SLOGE("HsMhl_open exit \n");
    return s32Status;
}

/**********************************************************************
* ������������ ��open ����
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static struct hw_module_methods_t tHdmiMhlModuleMethods =
{
    .open = HsMhlOpen,
};

/**********************************************************************
* ��������������Ӳ��ģ������
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

struct hdmi_mhl_module HAL_MODULE_INFO_SYM =
{
    .common =
    {
        .tag           = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id            = HDMI_MHL_HARDWARE_MODULE_ID,
        .name          = "HISENSE HDMI MHL HW HAL",
        .author        = "The Android Open Source Project",
        .methods       = &tHdmiMhlModuleMethods,
    },
};

