#include "hs_hal_mhl.h"
#include "hi_common.h"
#include "hi_unf_hdmirx.h"
//#include <android/log.h>
#include <log.h>
#include <unistd.h>
#include <android/keycodes.h>

#define LOG_TAG "HAL_MHL"

//#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)

T_Android2MsKey atAndroid2MsKey[] =
{
        {AKEYCODE_DPAD_CENTER,               HS_UNF_HDMIRX_RCP_KEY_SELECT},
        {AKEYCODE_DPAD_UP,                   HS_UNF_HDMIRX_RCP_KEY_UP},
        {AKEYCODE_DPAD_DOWN,                 HS_UNF_HDMIRX_RCP_KEY_DOWN},
        {AKEYCODE_DPAD_LEFT,                 HS_UNF_HDMIRX_RCP_KEY_LEFT},
        {AKEYCODE_DPAD_RIGHT,                HS_UNF_HDMIRX_RCP_KEY_RIGHT},
        {AKEYCODE_MENU,                      HS_UNF_HDMIRX_RCP_KEY_ROOT_MENU},
        {AKEYCODE_BACK,                      HS_UNF_HDMIRX_RCP_KEY_EXIT},
        {AKEYCODE_MEDIA_PLAY,                HS_UNF_HDMIRX_RCP_KEY_PLAY},
        {AKEYCODE_MEDIA_PAUSE,               HS_UNF_HDMIRX_RCP_KEY_PAUSE},
        {AKEYCODE_MEDIA_STOP,                HS_UNF_HDMIRX_RCP_KEY_STOP},
        {AKEYCODE_MEDIA_FAST_FORWARD,        HS_UNF_HDMIRX_RCP_KEY_FAST_FWD},
        {AKEYCODE_MEDIA_REWIND,              HS_UNF_HDMIRX_RCP_KEY_REWIND},
};

HS_HANDLE hMhlHandle;
HS_U8     RcpCode;

HS_U8 HI_MHL_GetRcpCode(HS_HANDLE hMhlHandle,HS_U8 RcpCode)
{
    return 0;
}


/*初始化 mhl模块*/
HS_HAL_RESULT HS_HAL_MHL_Init(HS_MHL_InitParams_t *pstInitParams)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_UNF_HDMIRX_Init();

    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}
/*打开MHL模块 获得句柄*/
HS_HAL_RESULT HS_HAL_MHL_Open(HS_MHL_OpenParams_t *pstOpenParam ,HS_HANDLE *phMhlHandle)
{
    *phMhlHandle = hMhlHandle;

    return HS_HAL_ERR_OK;
}

//------------------------------------------------------------------------------
/// @brief Get MHL handle
/// @param[in] u32MhlPortIndex: Open Params
/// @param[out] phMhlHandle: get MHL handle
/// @return MI_OK: Get MHL handle success.
/// @return MI_ERR_FAILED: Get MHL handle fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_MHL_GetHandle(HS_U32 u32MhlPortIndex,HS_HANDLE *phMhlHandle)
{
     *phMhlHandle = hMhlHandle;

    return HS_HAL_ERR_OK;
}

//------------------------------------------------------------------------------
/// @brief Dup MHL handle
/// @param[in] hMhlHandle: MHL handle
/// @param[in] bStrongRef: MHL handle type
/// @param[out] phMhlHandle: dup MHL handle
/// @return MI_OK: dup MHL handle success.
/// @return MI_ERR_FAILED: dup MHL handle fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_MHL_DupHandle(HS_HANDLE hMhlHandle, HS_BOOL bStrongRef, HS_HANDLE *phMhlHandle)
{
    return HS_HAL_ERR_OK;
}

/*打开MHL模块 获得句柄*/
HS_HAL_RESULT HS_HAL_EXTIN_Open(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle)
{
    *phExtInHandle = hMhlHandle;

    return HS_HAL_ERR_OK;
}
/* 获得句柄*/
HS_HAL_RESULT HS_HAL_EXTIN_GetHandle(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle)
{
    *phExtInHandle = hMhlHandle;

    return HS_HAL_ERR_OK;
}
/*复制句柄*/
HS_HAL_RESULT HS_HAL_EXTIN_DupHandle(HS_HANDLE hExtInHandle, HS_BOOL bStrongRef, HS_HANDLE *phExtInHandle)
{
    return HS_HAL_ERR_OK;
}
/*MHL的回调函数*/
HS_HAL_RESULT HS_HAL_MHL_RegisterCallback(HS_HANDLE hMhlHandle, HS_MHL_CallbackParams_t *pstCallbackParams)
{
    pstCallbackParams->pfEventCallback = HI_MHL_GetRcpCode;
    pstCallbackParams->pUsrParams = NULL;

    return HS_HAL_ERR_OK;
}
/*切换source*/
HS_HAL_RESULT HS_HAL_MHL_SourceChange(HS_HANDLE hMhlHandle,HS_HANDLE hInputHandle)
{
    return HS_HAL_ERR_OK;
}
/*关闭MHL模块*/
HS_HAL_RESULT HS_HAL_MHL_Close(HS_HANDLE hMhlHandle)
{
    return HS_HAL_ERR_OK;
}
/*close extin handle*/
HS_HAL_RESULT HS_HAL_EXTIN_Close(HS_HANDLE hExtInHandle)
{
    return HS_HAL_ERR_OK;
}
/*结束MHL模块 */
HS_HAL_RESULT HS_HAL_MHL_DeInit(void)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_UNF_HDMIRX_DeInit();

    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}
/*Finalize Extin module*/
HS_HAL_RESULT HS_HAL_EXTIN_DeInit(void)
{
    return HS_HAL_ERR_OK;
}


//------------------------------------------------------------------------------
/*获得信号状态*/
/// @brief Get MHL cable status
/// @param[in] hMhlHandle:
/// @return MI_OK: MHL cable is linked
/// @return MI_ERR_FAILED: MHL cable is not link
HS_HAL_RESULT HS_HAL_EXTIN_GetSignalStatus(HS_HANDLE hExtInHandle, E_HS_EXTIN_SIGNAL_STATUS *eSignalStatus)
{
    HI_UNF_SIG_STATUS_E enSignalStatus;
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_UNF_HDMIRX_MHL_GetSigStatus(&enSignalStatus);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }
    if(HI_UNF_SIG_SUPPORT == enSignalStatus)
    {
        *eSignalStatus = E_HS_SIGNAL_STATUS_LOCK;
    }
    else if(HI_UNF_SIG_NO_SIGNAL == enSignalStatus || HI_UNF_SIG_UNSTABLE == enSignalStatus)
    {
        *eSignalStatus = E_HS_SIGNAL_STATUS_UNLOCK;
    }
    else
    {
        *eSignalStatus = E_HS_SIGNAL_STATUS_NOTSUPPORT;
    }
    return HS_HAL_ERR_OK;
}

//------------------------------------------------------------------------------
/*获得Cbus状态 */
/// @brief Get Cbus status
/// @param[in] hMhlHandle: MHL handle
/// @return MI_OK: Cbus status is OK
/// @return MI_ERR_FAILED: Cbus status is not OK
// why need Cbus status?
HS_HAL_RESULT HS_HAL_MHL_GetCbusStatus(HS_HANDLE hMhlHandle)
{
    HI_BOOL bPresent;
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_UNF_HDMIRX_MHL_GetCbusStatus(&bPresent);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    if(HI_TRUE == bPresent)
    {
        return HS_HAL_ERR_OK;
    }
    else
    {
        return HS_HAL_ERR_FAILED;
    }
}

//------------------------------------------------------------------------------
/*发送RCP 按键*/
/// @brief TV Send IR to MHL source
/// @param[in] hMhlHandle: MHL handle
/// @param[in] u32IRKeyCode: IR Code
/// @return MI_OK: TV Send IR to MHL source success.
/// @return MI_ERR_FAILED: TV Send IR to MHL source fail.
// Send RCP Key?
HS_HAL_RESULT HS_HAL_MHL_IRKeyProcess(HS_HANDLE hMhlHandle, HS_U32 u32IRKeyCode)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_UNF_HDMIRX_SendRcpKey(u32IRKeyCode);
    SLOGD("HI_UNF_HDMIRX_SendRcpKey,%d:",s32Ret);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    s32Ret = HI_UNF_HDMIRX_ReleaseRcpKey(u32IRKeyCode);
    SLOGD("HI_UNF_HDMIRX_ReleaseRcpKey,%d:",s32Ret);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}

//------------------------------------------------------------------------------
/// @brief Set Vbus if or not to charge MHL source
/// @param[in] hMhlHandle: MHL handle
/// @param[in] eVbusCtrlType: According to eVbusCtrlType to set if or not to charge MHL source
/// @return MI_OK: Set Vbus success.
/// @return MI_ERR_FAILED: Set Vbus  fail.
//
HS_HAL_RESULT HS_HAL_MHL_SetVbusControl (HS_HANDLE hMhlHandle, HS_MHL_VbusCrtl_Type_e  eVbusCtrlType)
{
    return HS_HAL_ERR_OK;
}
/**********************************************************************
* 函数名称: HsKeyAntoMs
* 功能描述: 按键转发
* 输入参数: tKey键值
* 输出参数: 无
* 返 回 值: android的按键值
* 其它说明:
* 修改日期                      修改人                      修改内容
* 2016.7.12                     chenweilan                  创建
***********************************************************************/

HS_UNF_HDMIRX_RCP_KEY_E HS_HAL_MHL_KeyAntoMs(HS_S32 s32Key)
{
    HS_U32 u32Loop;
    for (u32Loop = 0; u32Loop < (sizeof(atAndroid2MsKey) / sizeof(T_Android2MsKey)); u32Loop++)
    {
        if (atAndroid2MsKey[u32Loop].s32AndroidKeyCode == s32Key)
        {
            return atAndroid2MsKey[u32Loop].tMsKeyCode;
        }
    }

    return (HS_UNF_HDMIRX_RCP_KEY_E) (HS_UNF_HDMIRX_RCP_KEY_BUTT + 1);
}
/**********************************************************************
* 函数名称:  HsKeyMstoAn
* 功能描述:  底层按键到android按键转化
* 输入参数:  tKey键值
* 输出参数:  无
* 返 回 值:  android的按键值
* 其它说明:
* 修改日期                      修改人                      修改内容
* 2016.7.12                     chenweilan                  创建
***********************************************************************/
HS_S32 KeyMstoAn(HS_UNF_HDMIRX_RCP_KEY_E tKey)
{
    HS_U32 u32Loop;
    for (u32Loop = 0; u32Loop < (sizeof(atAndroid2MsKey) / sizeof(T_Android2MsKey)); u32Loop++)
    {
        if (atAndroid2MsKey[u32Loop].tMsKeyCode == tKey)
        {
            return atAndroid2MsKey[u32Loop].s32AndroidKeyCode;
        }
    }

    return AKEYCODE_UNKNOWN;
}


