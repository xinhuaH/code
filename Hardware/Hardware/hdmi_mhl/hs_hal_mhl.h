#ifdef __cplusplus
extern "C" {
#endif

#include "hs_hal_mhl_datatype.h"

/*��ʼ�� mhlģ��*/
HS_HAL_RESULT HS_HAL_MHL_Init(HS_MHL_InitParams_t *pstInitParams);
/*��MHLģ�� ��þ��*/
HS_HAL_RESULT HS_HAL_MHL_Open(HS_MHL_OpenParams_t *pstOpenParam ,HS_HANDLE *phMhlHandle);
//------------------------------------------------------------------------------
/// @brief Get MHL handle
/// @param[in] u32MhlPortIndex: Open Params
/// @param[out] phMhlHandle: get MHL handle
/// @return MI_OK: Get MHL handle success.
/// @return MI_ERR_FAILED: Get MHL handle fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_MHL_GetHandle(HS_U32 u32MhlPortIndex,HS_HANDLE *phMhlHandle);

//------------------------------------------------------------------------------
/// @brief Dup MHL handle
/// @param[in] hMhlHandle: MHL handle
/// @param[in] bStrongRef: MHL handle type
/// @param[out] phMhlHandle: dup MHL handle
/// @return MI_OK: dup MHL handle success.
/// @return MI_ERR_FAILED: dup MHL handle fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_MHL_DupHandle(HS_HANDLE hMhlHandle, HS_BOOL bStrongRef, HS_HANDLE *phMhlHandle);



/*��MHLģ�� ��þ��*/
HS_HAL_RESULT HS_HAL_EXTIN_Open(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle);
/* ��þ��*/
HS_HAL_RESULT HS_HAL_EXTIN_GetHandle(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle);
/*���ƾ��*/
HS_HAL_RESULT HS_HAL_EXTIN_DupHandle(HS_HANDLE hExtInHandle, HS_BOOL bStrongRef, HS_HANDLE *phExtInHandle);
/*MHL�Ļص�����*/
HS_HAL_RESULT HS_HAL_MHL_RegisterCallback(HS_HANDLE hMhlHandle, HS_MHL_CallbackParams_t *pstCallbackParams);
/*�л�source*/
HS_HAL_RESULT HS_HAL_MHL_SourceChange(HS_HANDLE hMhlHandle,HS_HANDLE hInputHandle);//HS_U32 u32SourcePortIndex,MHL_INPUT_SOURCE_TYPE currSource);
/*�ر�MHLģ��*/
HS_HAL_RESULT HS_HAL_MHL_Close(HS_HANDLE hMhlHandle);
/*close extin handle*/
HS_HAL_RESULT HS_HAL_EXTIN_Close(HS_HANDLE hExtInHandle);
/*����MHLģ�� */
HS_HAL_RESULT HS_HAL_MHL_DeInit(void);
/*Finalize Extin module*/
HS_HAL_RESULT HS_HAL_EXTIN_DeInit(void);
/*����ź�״̬*/
/// @brief Get MHL cable status
/// @param[in] hMhlHandle:
/// @return MI_OK: MHL cable is linked
/// @return MI_ERR_FAILED: MHL cable is not link
HS_HAL_RESULT HS_HAL_EXTIN_GetSignalStatus(HS_HANDLE hExtInHandle, E_HS_EXTIN_SIGNAL_STATUS *eSignalStatus);
/*���Cbus״̬ */
/// @brief Get Cbus status
/// @param[in] hMhlHandle: MHL handle
/// @return MI_OK: Cbus status is OK
/// @return MI_ERR_FAILED: Cbus status is not OK
HS_HAL_RESULT HS_HAL_MHL_GetCbusStatus(HS_HANDLE hMhlHandle);
/*����RCP ����*/
/// @brief TV Send IR to MHL source
/// @param[in] hMhlHandle: MHL handle
/// @param[in] u32IRKeyCode: IR Code
/// @return MI_OK: TV Send IR to MHL source success.
/// @return MI_ERR_FAILED: TV Send IR to MHL source fail.
HS_HAL_RESULT HS_HAL_MHL_IRKeyProcess(HS_HANDLE hMhlHandle,HS_U32 u32IRKeyCode);
/// @brief Set Vbus if or not to charge MHL source
/// @param[in] hMhlHandle: MHL handle
/// @param[in] eVbusCtrlType: According to eVbusCtrlType to set if or not to charge MHL source
/// @return MI_OK: Set Vbus success.
/// @return MI_ERR_FAILED: Set Vbus  fail.
HS_HAL_RESULT HS_HAL_MHL_SetVbusControl (HS_HANDLE hMhlHandle,HS_MHL_VbusCrtl_Type_e  eVbusCtrlType);
/// @����ת��
/// @param[in] s32Key: tKey��ֵ
/// @return SUCCESS: android�İ���ֵ.
/// @return FAILED:  Illeagle KeyVale.
HS_UNF_HDMIRX_RCP_KEY_E HS_HAL_MHL_KeyAntoMs(HS_S32 s32Key);
/// @����ת��
/// @param[in] s32Key: tKey��ֵ
/// @return SUCCESS: android�İ���ֵ.
/// @return FAILED:  Illeagle KeyVale.

HS_S32 KeyMstoAn(HS_UNF_HDMIRX_RCP_KEY_E tKey);

typedef struct
{
    HS_S32 s32AndroidKeyCode;
    HS_UNF_HDMIRX_RCP_KEY_E tMsKeyCode;
}T_Android2MsKey;

#ifdef __cplusplus
}
#endif

