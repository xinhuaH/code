

#ifndef _HS_CEC_H_
#define _HS_CEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include  "hs_hal_cec_datatype.h"
#include <hs_type.h>
//#include  "hi_unf_sound.h"




//------------------------------------------------------------------------------
/// @初始化CEC模块
/// @param[in] pstInitParams: Init Param
/// @return HS_OK: Init Success
/// @return HS_ERR_FAILED: Init Fail
/// @return HS_HAS_INITED: Module Has Inited
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_Init(HS_CEC_InitParams_t *pstInitParams);

//------------------------------------------------------------------------------
/// @brief 关闭 CEC 模块.
/// @return HS_OK:  Finalize CEC module success.
/// @return HS_ERR_FAILED:  Finalize CEC module fail.
/// @return HS_ERR_NOT_INITED: CEC module not initialized.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_DeInit(void);

//------------------------------------------------------------------------------
/// @brief 打开 CEC模块
/// @param[in] pstOpenParams: Open Param
/// @param[out] phCecHandle: Get CEC handle
/// @return HS_HAL_OK: Open CEC Module success
/// @return HS_HAL_ERR_FAILED: Open CEC Module  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_Open(T_HS_CEC_OpenParams *pstOpenParams,HS_HANDLE *phCecHandle);

//------------------------------------------------------------------------------
/// @brief Close CEC module
/// @param[in] hCecHandle: CEC handle
/// @return HS_HAL_OK: Close CEC module success.
/// @return HS_HAL_ERR_FAILED: Close CEC module  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_Close(HS_HANDLE hCecHandle);

//------------------------------------------------------------------------------
/// @brief Set Device LogicalAddr
/// @param[in] hCecHandle: CEC handle
/// @param[in] enSetMyLA: Logical Addr
/// @return HS_HAL_OK: Set LogicalAddr Success
/// @return HS_HAL_ERR_FAILED: Set LogicalAddr fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SetLogicalAddr(HS_HANDLE hCecHandle,E_HS_CEC_DeviceLA enSetMyLA);

//------------------------------------------------------------------------------
/// @brief Get Rx Msg Info
/// @param[in] hCecHandle: CEC handle
/// @param[out] pstRxMsgInfo: Recive Msg Info
/// @return HS_HAL_OK: Get source Recive Msg Info success.
/// @return HS_HAL_ERR_FAILED: Recive Msg Info  fail.
//----------------------------//////////--------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetRxMsgInfo (HS_HANDLE hCecHandle,T_HS_CEC_MsgInfo* pstRxMsgInfo);

//------------------------------------------------------------------------------
/// @brief Send Msg to Device Tx
/// @param[in] hCecHandle: CEC handle
/// @param[in] pstTxMsgInfo: Msg Info to Tx
/// @param[out] psResErrCode: get process return information
/// @return HS_HAL_OK: Send Tx Msg success.
/// @return HS_HAL_ERR_FAILED:  Send Tx Msg fail.
//--------------------------------////----------------------------------------------
HS_HAL_RESULT HS_HAL_GPIO_Init(void );

HS_HAL_RESULT HS_HAL_CEC_SendTxMsg(HS_HANDLE hCecHandle,T_HS_CEC_MsgInfo *pstTxMsgInfo,E_HS_CEC_ErrorCode* psResErrCode);

//------------------------------------------------------------------------------
/// @brief Poll Device
/// @param[in] hCecHandle: CEC handle
/// @param[in] enDstAddr: Device LogicalAddr
/// @param[out] psResErrCode: process return information
/// @return HS_HAL_OK: Poll Tx Device success.
/// @return HS_HAL_ERR_FAILED: Poll Tx Device fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SendTxPolling(HS_HANDLE hCecHandle,E_HS_CEC_DeviceLA enDstAddr,E_HS_CEC_ErrorCode* psResErrCode);

//------------------------------------------------------------------------------
/// @brief Rx Check Get Msg Frame
/// @param[in] hCecHandle: CEC handle
/// @param[in] enMsgType: Msg Translate Type
/// @param[in] u8Len: Msg Len
/// @return HS_HAL_OK: Check Get Msg Frame success.
/// @return HS_HAL_ERR_FAILED: Check Get Msg Frame  fail.
//---------------------------------////---------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_CheckTxFrame(HS_HANDLE hCecHandle,HS_CEC_MsgTransType_e enMsgType,HS_U8 u8Len);

//------------------------------------------------------------------------------
/// @brief Get Rx Recive Msg Count
/// @param[in] hCecHandle: CEC handle
/// @param[out] psMsgCnt: Msg Count
/// @return HS_HAL_OK: Get Msg Count success.
/// @return HS_HAL_ERR_FAILED: Get Msg Count  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetRxMsgCnt(HS_HANDLE hCecHandle,HS_U8* psMsgCnt);

//------------------------------------------------------------------------------
/// @brief Set Rx Recive Msg Count
/// @param[in] hCecHandle: CEC handle
/// @param[in] u8MsgCnt: Msg Count
/// @return HS_HAL_OK: Set Msg Count success.
/// @return HS_HAL_ERR_FAILED: Set Msg Count   fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SetRxMsgCnt(HS_HANDLE hCecHandle,HS_U8 u8MsgCnt);

//------------------------------------------------------------------------------
/// @brief Get Rx Recived Data
/// @param[in] hCecHandle: CEC handle
/// @param[in] u8Fifoidx: Fifo idx
/// @param[in] u8Idx:
/// @param[out] psResDate: Get Rx Data
/// @return HS_HAL_OK: Get Rx Recived Data success.
/// @return HS_HAL_ERR_FAILED: Get Rx Recived Data  fail.
//-------------------------------///-----------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetRxData(HS_HANDLE hCecHandle,HS_U8 u8Fifoidx, HS_U8 u8Idx, HS_U8* psResDate);

//------------------------------------------------------------------------------
/// @brief Get Rx Fifo idx
/// @param[in] hCecHandle: CEC handle
/// @param[out] psFifoIdx: Get Rx Fifo idx
/// @return HS_HAL_OK: Get Rx Fifo idx success.
/// @return HS_HAL_ERR_FAILED: Get Rx Fifo idx fail.
//------------------------------///------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetRxFifoIdx(HS_HANDLE hCecHandle,HS_U8* psFifoIdx);

//------------------------------------------------------------------------------
/// @brief Set Rx Fifo idx
/// @param[in] hCecHandle: CEC handle
/// @param[in] u8FifoIdx: Fifo idx
/// @return HS_HAL_OK: Set Rx Fifo idx success.
/// @return HS_HAL_ERR_FAILED: Set Rx Fifo idx  fail.
//--------------------------------///----------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SetRxFifoIdx(HS_HANDLE hCecHandle,HS_U8 u8FifoIdx);

//------------------------------------------------------------------------------
/// @brief Get Rx Fifo Count
/// @param[in] hCecHandle: CEC handle
/// @param[out] psRxFifoCnt: Get Rx Fifo Count
/// @return HS_HAL_OK: Get Rx Fifo Count success.
/// @return HS_HAL_ERR_FAILED: Get Rx Fifo Count  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetRxFifoCnt(HS_HANDLE hCecHandle,HS_U8* psRxFifoCnt);

//------------------------------------------------------------------------------
/// @brief Set Arc Control
/// @param[in] hCecHandle: CEC handle
/// @param[in] bEnable: Arc onoff
/// @return HS_HAL_OK: Set Arc on
/// @return HS_HAL_ERR_FAILED: Set Arc off
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SetArcControl(HS_HANDLE hCecHandle,HS_BOOL bEnable);

//------------------------------------------------------------------------------
/// @brief Set TV Wakeup config
/// @param[in] hCecHandle: CEC handle
/// @return HS_HAL_OK: Set TV Wakeup config success.
/// @return HS_HAL_ERR_FAILED: Set TV Wakeup config  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_SetWakeUpConfig(HS_HANDLE hCecHandle);

//------------------------------------------------------------------------------
/// @brief CEC Enable
/// @param[in] hCecHandle: CEC handle
/// @return HS_HAL_OK: Enable CEC OK
/// @return HS_HAL_ERR_FAILED:  Enable CEC fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_Enable(HS_HANDLE hCecHandle);

//---------------///---------------------------------------------------------------
/// @brief CEC Disable
/// @param[in] hCecHandle: CEC handle
/// @return HS_HAL_OK: Disable CEC OK
/// @return HS_HAL_ERR_FAILED: Disable CEC fail.
//-------------------------///-----------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_Disable(HS_HANDLE hCecHandle);

//------------------------------------------------------------------------------
/// @brief Get CEC handle
/// @param[out] phCecHandle: get CEC handle
/// @return HS_HAL_OK: Get CEC handle success.
/// @return HS_HAL_ERR_FAILED: Get CEC handle  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_GetHandle(HS_HANDLE *phCecHandle);

//------------------------------------------------------------------------------
/// @brief Dup CEC handle
/// @param[in] hCecHandle: CEC handle
/// @param[in] bStrongRef: CEC handle type
/// @param[out] phCecHandle: get CEC handle
/// @return HS_HAL_OK: Dup CEC handle success.
/// @return HS_HAL_ERR_FAILED: Dup CEC handle  fail.
//------------------------------------------------------------------------------
HS_HAL_RESULT HS_HAL_CEC_DupHandle(HS_HANDLE hCecHandle, HS_BOOL bStrongRef, HS_HANDLE *phCecHandle);


//------------------------------------------------------------------------------

HS_HAL_RESULT HS_HAL_EXTIN_Open(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle);


/// @brief Open a Aout handle.
/// @param[in] pstOpenParams.
/// @param[out] phAoutHandle.
/// @return MI_OK: Process success.
/// @return MI_ERR_NOT_INITED: Module not initialized.
/// @return MI_ERR_INVALID_PARAMETER: Parameters is invalid.
/// @return MI_ERR_RESOURCES: No available resource.
//--------------------///----------------///------------------------------------------
HS_HAL_RESULT HS_HAL_AOUT_Open(HS_AOUT_Attr_t *pstAttr, HS_HANDLE *phAoutHandle);
//------------------------------------------------------------------------------
/// @brief Close a Aout handle.
/// @param[in] hAoutHandle.
/// @return MI_OK: Process success.
/// @return MI_ERR_NOT_INITED: Module not initialized.
/// @return MI_ERR_INVALID_HANDLE: Handle is invalid.
//-------------------------//-----------------------------------------------------
HS_HAL_RESULT HS_HAL_AOUT_Close(HS_HANDLE hAoutHandle);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// @brief Get a Aout handle.
/// @param[out] *phAoutHandle.
/// @param[in] ePath.
/// @return MI_OK: Process success.
/// @return MI_ERR_NOT_INITED: Module not initialized.
/// @return MI_ERR_INVALID_HANDLE: Handle is invalid.
//-------------------------///-----------------------------------------------------
HS_HAL_RESULT HS_HAL_AOUT_GetHandle(HS_HANDLE *phAoutHandle, E_HsAoutPath ePath);
//------------------------------------------------------------------------------
/// @brief.Duplicate  Handle from existing handle
/// @param[in] hAoutHandle
/// @param[in] bSrongRef
/// @param[out] *phAoutHandle
/// @return MI_OK: Process success.
//------------------------////------------------------------------------------------
HS_HAL_RESULT HS_HAL_AOUT_DupHandle(HS_HANDLE hAoutHandle,HS_BOOL bStrongRef, HS_HANDLE *phAoutHandle);



//------------------------------------------------------------------------------
/// @brief Set aout attribute.
/// @param[in] hAoutHandle.
/// @param[in] eAttrType.
/// @param[in] *pUsrParam.
/// @return MI_OK: Process success.
/// @return MI_ERR_FAILED: Process fail.
//----------------------------///--------------------------------------------------
HS_HAL_RESULT HS_HAL_AOUT_SetAttr(HS_HANDLE hAoutHandle, HS_AOUT_AttrType_e eAttrType, void *pUsrParam);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// @brief  set extin attribute.
/// @param[in] eAttrType:set the type of extin attribute.
/// @param[in] pAttr:set the attibute.
/// @return MI_OK: Process success.
/// @return MI_ERR_FAILED: Process fail.
//-------------------------------////-----------------------------------------------
HS_HAL_RESULT HS_HAL_EXTIN_SetAttr(HS_HANDLE hExtInHandle,HS_EXTIN_SetAttr_e eAttrType,void *pAttr);
//------------------------------------------------------------------------------
/// @brief Get Gpio number by device function control
/// @param[in] eGpioDevFunCtl: Gpio device function control id.
/// @param[out] u8GpioNum: Get of GPIO number by device funcion control.
/// @return MI_OK: Process success.
/// @return MI_ERR_FAILED: Process fail.
//------------------------------------------------------------------------------

////获得GPIO口高低
HS_HAL_RESULT HS_HAL_GPIO_GetOnOff(E_HS_GPIO_Level *peLevel, HS_BOOL *pbInvert);
//HS_HAL_RESULT HS_HAL_GPIO_GetGpioNumByDeviceFunction(_GPIO_DevFunCtl_e eGpioDevFunCtl, HS_U8 *u8GpioNum);


#ifdef __cplusplus
}
#endif

#endif
