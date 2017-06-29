#include  "hs_hal_cec.h"
#include  "hs_hal_cec_datatype.h"
#include  "hi_unf_hdmirx.h"
#include  "hi_unf_sound.h"
#include  "hi_unf_gpio.h"
#include  "hi_unf_edid.h"
#include  <string.h>
#include <android/log.h>
#include <pthread.h>
#include  <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cutils/properties.h"

#define LOG  "HDMI_RX"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
typedef enum hiEDID_CEA_TAG_E
{
    EDID_CEA_TAG_Res1= 0,
    EDID_CEA_TAG_AUDIO,
    EDID_CEA_TAG_VIDEO,
    EDID_CEA_TAG_VSDB,
    EDID_CEA_TAG_SPEAK,
    EDID_CEA_TAG_VESA,
    EDID_CEA_TAG_Res2,
    EDID_CEA_TAG_EXT,
    EDID_CEA_TAG_TOTAL
}EDID_CEA_TAG_E;

typedef enum tagEDID_AUDDATA
{
    AUD_Reserved=0,
    AUD_PcmSupport,
    AUD_Ac3Support,
    AUD_Mpeg1Support,
    AUD_Mp3Support,
    AUD_Mpeg2Support,
    AUD_AacSupport,
    AUD_DtsSupport,
    AUD_AtracSupport,
    AUD_OnebitSupport,
    AUD_EAc3Support,
    AUD_DtsHdSupport,
    AUD_MatSupport,
    AUD_DstSupport,
    AUD_WmaSupport,
    AUD_BUTT
}EDID_AUDDATA;

#define GPIO_HDMI0   160
#define GPIO_HDMI1   161
#define GPIO_HDMI2   162

HS_HANDLE hCecHandle;
static HI_UNF_HDMIRX_CEC_CMD_S stCecMsgRx = {0};
/*hisilicon patch to resolve CEC TX message failure sometime*/
static pthread_mutex_t mMutex;
static HI_BOOL bMutexInit = HI_FALSE;

static HI_U32 HI_CEC_GetCeaAudioPos(HI_U32 *pstEdidData)
{
    HI_U32 *pstEdidExt;
    HI_U32 i, u32CeaDataLen;
    HI_U32 pos=0;

    pstEdidExt = pstEdidData+128+4;
    u32CeaDataLen = *(pstEdidData + 2 + 128) - 4;
    for(i=0;i<u32CeaDataLen;)
    {
        switch((pstEdidExt[i]>>5))
        {
            case EDID_CEA_TAG_AUDIO:
                pos = i+128+4;
                break;
            case EDID_CEA_TAG_VIDEO:

                break;
            case EDID_CEA_TAG_VSDB:

                break;
            case EDID_CEA_TAG_SPEAK:

                break;
            case EDID_CEA_TAG_VESA:

                break;
            case EDID_CEA_TAG_EXT:

                break;
            default:
                break;
        }
        i = i+1+ (pstEdidExt[i]& 0x1F);

    }
    return pos;
}
static HS_S32 HI_CEC_SendMSG(HI_UNF_HDMIRX_CEC_CMD_S *pstCecCmd)
{
    if(pstCecCmd == NULL)
        return HI_FAILURE;

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Count = 0;
    HI_UNF_HDMIRX_CEC_CMD_STATE_S pstTxStat;

    s32Ret = HI_UNF_HDMIRX_CEC_SetCommand(pstCecCmd);
    for(; u32Count<80; u32Count++)
    {
        s32Ret = HI_UNF_HDMIRX_CEC_GetCurCmdState(&pstTxStat);
        if(HI_FAILURE == s32Ret)
        {
            usleep(10 * 1000);
        }
        else
        {
            break;
        }
    }

    return s32Ret;
}
HI_S32 HI_CEC_SetDdpAtmosEn(HI_U32 *pstEdidData, HI_BOOL bEnable)
{
    HI_U32 u32Pos;
    HI_BOOL bEn = HI_FALSE;
    HI_U32 *pstEdidExt,*pstEdidtmp;
    HI_U32 u32Len;

    u32Pos = HI_CEC_GetCeaAudioPos(pstEdidData);
 //   LOGI("%s[%d] ,%d\n",__FUNCTION__,__LINE__,u32Pos);
    if(u32Pos <= 0)
    {
        return HI_FAILURE;
    }


    if(pstEdidData == HI_NULL)
    {
     //   LOGI("%s[%d]\n",__FUNCTION__,__LINE__);
        return HI_FAILURE;
    }

    u32Len = pstEdidData[u32Pos] & 0x1F;
    pstEdidtmp = pstEdidData + u32Pos + 1;

    if(u32Len % 3 != 0)
    {
        return HI_FALSE;
    }

    HI_U32 i;
    for(i = 0; i < u32Len; i +=3 )
    {
        if(((pstEdidtmp[i]>>3)&0xf) == AUD_EAc3Support)
        {
           // LOGI("%s[%d] ,%x,%x,%x\n",__FUNCTION__,__LINE__,pstEdidtmp[i],pstEdidtmp[i+1],pstEdidtmp[i+2]);
            if((pstEdidtmp[i+2] & 0x1)==1)
            {
                bEn = HI_TRUE;
            }
            else
            {
                bEn = HI_FALSE;
            }

            if(bEn != bEnable)
            {
               // LOGI("%s[%d] old:%d,new:%d\n",__FUNCTION__,__LINE__,bEn,bEnable);
                if(bEnable == HI_TRUE)
                {
                    pstEdidtmp[i+2] = (pstEdidtmp[i+2] +1)&0xff;
                    pstEdidData[255] = (pstEdidData[255] -1)&0xff;
                }
                else
                {
                    pstEdidtmp[i+2] = (pstEdidtmp[i+2] -1)&0xff;
                    pstEdidData[255] = (pstEdidData[255] +1)&0xff;
                }
            }
            break;
        }

    }
    return HI_SUCCESS;

}

static HS_VOID HI_CEC_ReqArcInit()
{
    HI_UNF_HDMIRX_CEC_CMD_S stCecMsg;

    stCecMsg.u32SrcAddr = E_HS_CEC_LA_TV;
    stCecMsg.u32DstAddr = E_HS_CEC_LA_AUDIO_SYS;
    stCecMsg.enOpcode   = HI_UNF_HDMIRX_CEC_OPCODE_REQUEST_ARC_INITIATION;
    stCecMsg.stOperand.u32ArgCount = 0;
    HI_CEC_SendMSG(&stCecMsg);
}

static HS_VOID HI_CEC_ReqArcDeInit()
{
    HI_UNF_HDMIRX_CEC_CMD_S stCecMsg;

    stCecMsg.u32SrcAddr = E_HS_CEC_LA_TV;
    stCecMsg.u32DstAddr = E_HS_CEC_LA_AUDIO_SYS;
    stCecMsg.enOpcode   = HI_UNF_HDMIRX_CEC_OPCODE_REQUEST_ARC_TERMINATION;
    stCecMsg.stOperand.u32ArgCount = 0;
    HI_CEC_SendMSG(&stCecMsg);
}


HS_HAL_RESULT HS_HAL_CEC_Init(HS_CEC_InitParams_t *pstInitParams)
{
    HS_S32 s32Ret = HI_SUCCESS;

    s32Ret |= HI_UNF_HDMIRX_Init();
    s32Ret |= HI_UNF_SND_Init();

    if(HI_FALSE == bMutexInit)
    {
        pthread_mutex_init(&mMutex, NULL);
        bMutexInit = HI_TRUE;
    }

    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_DeInit(void)
{
    HS_S32 s32Ret = HI_SUCCESS;

    s32Ret |= HI_UNF_HDMIRX_DeInit();
    s32Ret |= HI_UNF_SND_DeInit();

    if(HI_TRUE == bMutexInit)
    {
        pthread_mutex_destroy(&mMutex);
        bMutexInit = HI_FALSE;
    }

    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_Open(T_HS_CEC_OpenParams *pstOpenParams, HS_HANDLE *phCecHandle)
{
    *phCecHandle = hCecHandle;

    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_Close(HS_HANDLE hCecHandle)
{
    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_SetLogicalAddr(HS_HANDLE hCecHandle,E_HS_CEC_DeviceLA enSetMyLA)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetRxMsgInfo (HS_HANDLE hCecHandle,T_HS_CEC_MsgInfo* pstRxMsgInfo)
{
    HS_S32 s32Ret = HI_SUCCESS;
    HS_U32 i = 0;

    pstRxMsgInfo->enHeader = (stCecMsgRx.u32SrcAddr << 4) | stCecMsgRx.u32DstAddr;
    pstRxMsgInfo->enOpcode = stCecMsgRx.enOpcode;
    pstRxMsgInfo->u8MsgLen = stCecMsgRx.stOperand.u32ArgCount + 2;
    if(stCecMsgRx.stOperand.u32ArgCount != 0)
    {
        for (i =0 ; i < stCecMsgRx.stOperand.u32ArgCount; i++)
        {
            pstRxMsgInfo->szPara[i] = stCecMsgRx.stOperand.au32Args[i];
            //LOGD(" i=%d,cwl pstRxMsgInfo->szPara[i]=%x",i,pstRxMsgInfo->szPara[i]);
        }
    }

    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_SendTxMsg(HS_HANDLE hCecHandle,T_HS_CEC_MsgInfo *pstTxMsgInfo,E_HS_CEC_ErrorCode* psResErrCode)
{
    HS_S32 s32Ret = HI_SUCCESS;
    HS_U32 i;
    HI_UNF_HDMIRX_CEC_CMD_S stCecMsg;
    memset(&stCecMsg, 0x00, sizeof(stCecMsg));
    stCecMsg.u32SrcAddr = pstTxMsgInfo->enHeader & 0xf0;
    stCecMsg.u32DstAddr = pstTxMsgInfo->enHeader & 0x0f;
    if(pstTxMsgInfo->u8MsgLen ==1)
    {
      stCecMsg.enOpcode = HI_UNF_HDMIRX_CEC_OPCODE_SENDPING;
    }
    else
    {
      stCecMsg.enOpcode = pstTxMsgInfo->enOpcode;
    }
    // LOGD("hdmi hs_hal_cec_sendtxmsg CWL pstTxMsgInfo->u8MsgLen=%d\n",pstTxMsgInfo->u8MsgLen);
    if(pstTxMsgInfo->u8MsgLen > 2)
    {
        stCecMsg.stOperand.u32ArgCount = pstTxMsgInfo->u8MsgLen -2;
        for (i = 0; i < stCecMsg.stOperand.u32ArgCount; i++)
        {
            stCecMsg.stOperand.au32Args[i] = pstTxMsgInfo->szPara[i];
        }
    }
    LOGD("---cecinfo---Msg send0,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
//  LOGD("hdmi hs_hal_cec_sendtxmsg stCecMsg.u32SrcAddr=%d\n",stCecMsg.u32SrcAddr);
//  LOGD("hdmi hs_hal_cec_sendtxmsg stCecMsg.u32DstAddr=%d\n",stCecMsg.u32DstAddr);

 //    LOGD("hdmi hs_hal_cec_sendtxmsg stCecMsg.enOpcode=%d\n",stCecMsg.enOpcode);
/*
    s32Ret = HI_CEC_SendMSG(&stCecMsg);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
*/
    if(HI_TRUE == bMutexInit)
    {
        pthread_mutex_lock(&mMutex);
    }
    HI_U32 u32Count = 0;
    HI_UNF_HDMIRX_CEC_CMD_STATE_S stTxStat;
    *psResErrCode = E_HS_CEC_FEATURE_ABORT;
    s32Ret = HI_UNF_HDMIRX_CEC_SetCommand(&stCecMsg);
    for(; u32Count<80; u32Count++)
    {
        s32Ret = HI_UNF_HDMIRX_CEC_GetCurCmdState(&stTxStat);
        if(HI_SUCCESS == s32Ret)
        {
            if(stTxStat.enCurState == HI_UNF_HDMIRX_CEC_CMD_STATE_SUCCESS)
            {
                *psResErrCode = E_HS_CEC_TX_SUCCESS;
                 LOGD("---cecinfo---Msg send success,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
            }
            else
            {
                *psResErrCode = E_HS_CEC_FEATURE_ABORT;
                 LOGD("---cecinfo---Msg send abort,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
            }
            break;
        }
        else
        {
            usleep(10 * 1000);
        }
    }
    if(HI_TRUE == bMutexInit)
    {
        pthread_mutex_unlock(&mMutex);
    }
    LOGD("---cecinfo---Msg send %d,src:%x,des:%x,Opc:%x\n",*psResErrCode,stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
    LOGD("---cecinfo---Msg send Data:");
    for(i = 0; i < stCecMsg.stOperand.u32ArgCount; i++)
    {
        LOGD("---cecinfo---Msg send Data%d:0x%x",i,stCecMsg.stOperand.au32Args[i]);
    }
     if(E_HS_CEC_TX_SUCCESS == *psResErrCode)
    {
        return HS_HAL_ERR_OK;
    }
    else
    {
        return HS_HAL_ERR_FAILED;
    }


}


HS_HAL_RESULT HS_HAL_CEC_SendTxPolling(HS_HANDLE hCecHandle,E_HS_CEC_DeviceLA enDstAddr,E_HS_CEC_ErrorCode* psResErrCode)
{
    HS_S32 s32Ret = HS_SUCCESS;
    HI_UNF_HDMIRX_CEC_CMD_S stCecMsg;

    memset(&stCecMsg, 0x00, sizeof(stCecMsg));

    stCecMsg.u32SrcAddr = E_HS_CEC_LA_TV;
    stCecMsg.u32DstAddr = enDstAddr;
    stCecMsg.enOpcode = HI_UNF_HDMIRX_CEC_OPCODE_SENDPING;
    LOGD("---cecinfo---Msg send0,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
    //s32Ret = HI_CEC_SendMSG(&stCecMsg);
    if(HI_TRUE == bMutexInit)
    {
        pthread_mutex_lock(&mMutex);
    }
    HI_U32 u32Count = 0;
    HI_UNF_HDMIRX_CEC_CMD_STATE_S stTxStat;
    *psResErrCode = E_HS_CEC_FEATURE_ABORT;

    s32Ret = HI_UNF_HDMIRX_CEC_SetCommand(&stCecMsg);
    for(; u32Count<80; u32Count++)
    {
        s32Ret = HI_UNF_HDMIRX_CEC_GetCurCmdState(&stTxStat);
        if(HI_SUCCESS == s32Ret)
        {
            if(stTxStat.enCurState == HI_UNF_HDMIRX_CEC_CMD_STATE_SUCCESS)
            {
                *psResErrCode = E_HS_CEC_TX_SUCCESS;
                LOGD("---cecinfo---Msg send success,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
            }
            else
            {
                *psResErrCode = E_HS_CEC_FEATURE_ABORT;
                LOGD("---cecinfo---Msg send abort,src:%x,des:%x,Opc:%x\n",stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
            }
            break;
        }
        else
        {
            usleep(10 * 1000);
        }
    }
    if(HI_TRUE == bMutexInit)
    {
        pthread_mutex_unlock(&mMutex);
    }
    LOGD("---cecinfo---Msg send %d,src:%x,des:%x,Opc:%x\n",*psResErrCode,stCecMsg.u32SrcAddr,stCecMsg.u32DstAddr,stCecMsg.enOpcode);
 if(E_HS_CEC_TX_SUCCESS == *psResErrCode)
 {
     return HS_HAL_ERR_OK;
 }
 else
 {
     return HS_HAL_ERR_FAILED;
 }
}

HS_HAL_RESULT HS_HAL_CEC_CheckTxFrame(HS_HANDLE hCecHandle,HS_CEC_MsgTransType_e enMsgType,HS_U8 u8Len)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetRxMsgCnt(HS_HANDLE hCecHandle,HS_U8* psMsgCnt)
{
    HS_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_UNF_HDMIRX_CEC_GetCommand(&stCecMsgRx);
    if(HI_FAILURE == s32Ret)
    {
        *psMsgCnt = 0;
    }
    else
    {
        HS_U32 i;
        *psMsgCnt = 1;
        LOGD("---cecinfo---Msg receive,src:%x,des:%x,Opc:%x\n",stCecMsgRx.u32SrcAddr,stCecMsgRx.u32DstAddr,stCecMsgRx.enOpcode);
        LOGD("---cecinfo---Msg receive Data:");
        for(i = 0; i < stCecMsgRx.stOperand.u32ArgCount; i++)
        {
            LOGD("---cecinfo---Msg receive Data%d:0x%x",i,stCecMsgRx.stOperand.au32Args[i]);
        }
    }
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_GPIO_Init( )
{
    HS_S32 s32Ret = HS_SUCCESS;
    s32Ret = HI_UNF_GPIO_Init();

    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_SetRxMsgCnt(HS_HANDLE hCecHandle,HS_U8 u8MsgCnt)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetRxData(HS_HANDLE hCecHandle,HS_U8 u8Fifoidx, HS_U8 u8Idx, HS_U8* psResDate)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetRxFifoIdx(HS_HANDLE hCecHandle,HS_U8* psFifoIdx)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_SetRxFifoIdx(HS_HANDLE hCecHandle,HS_U8 u8FifoIdx)
    {
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetRxFifoCnt(HS_HANDLE hCecHandle,HS_U8* psRxFifoCnt)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_SetArcControl(HS_HANDLE hCecHandle,HS_BOOL bEnable)
{

    HI_S32 s32Ret = HI_SUCCESS;
    LOGD("HS_HAL_CEC_SetArcControl(not do here)");
    /*
    s32Ret |= HI_UNF_SND_SetMute(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0, !bEnable);
    s32Ret |= HI_UNF_SND_SetArcEnable(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_ARC0, bEnable);

    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }*/

    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_SetWakeUpConfig(HS_HANDLE hCecHandle)
{
    HS_S32 s32Ret = HI_SUCCESS;

    HI_UNF_HDMIRX_CEC_StandbyEnable(HS_TRUE);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_Enable(HS_HANDLE hCecHandle)
{
    HS_S32 s32Ret = HI_SUCCESS;

    HI_UNF_HDMIRX_CEC_Enable(HS_TRUE);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}


HS_HAL_RESULT HS_HAL_CEC_Disable(HS_HANDLE hCecHandle)
{
    HS_S32 s32Ret = HI_SUCCESS;

    HI_UNF_HDMIRX_CEC_Enable(HS_FALSE);
    if(HI_FAILURE == s32Ret)
    {
        return HS_HAL_ERR_FAILED;
    }

    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_GetHandle(HS_HANDLE *phCecHandle)
{
    *phCecHandle = hCecHandle;

    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_CEC_DupHandle(HS_HANDLE hCecHandle, HS_BOOL bStrongRef, HS_HANDLE *phCecHandle)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_EXTIN_Open(HS_EXTIN_INPUTSOURCE_TYPE_e eExtInSourceType, HS_U32 u32SourceIndex ,HS_HANDLE *phExtInHandle)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_AOUT_Open(HS_AOUT_Attr_t *pstAttr, HS_HANDLE *phAoutHandle)
{
    return HS_HAL_ERR_OK;
}
HS_HAL_RESULT HS_HAL_AOUT_Close(HS_HANDLE hAoutHandle)
{
    return HS_HAL_ERR_OK;
}

HS_HAL_RESULT HS_HAL_AOUT_GetHandle(HS_HANDLE *phAoutHandle, E_HsAoutPath ePath)
{
    return HS_HAL_ERR_OK;
}
HS_HAL_RESULT HS_HAL_AOUT_DupHandle(HS_HANDLE hAoutHandle,HS_BOOL bStrongRef, HS_HANDLE *phAoutHandle)
{
    return HS_HAL_ERR_OK;
}
HS_HAL_RESULT HS_HAL_AOUT_SetAttr(HS_HANDLE hAoutHandle, HS_AOUT_AttrType_e eAttrType, void *pUsrParam)
{
    static HI_UNF_SND_ARC_AUDIO_CAP_S stArcCap = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    HS_AOUT_HdmiArcCapabilityParams_t *pstHdmiArcCapParams;

    pstHdmiArcCapParams = (HS_AOUT_HdmiArcCapabilityParams_t *)pUsrParam;

    if(eAttrType != E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY)
    {
        return HS_HAL_ERR_FAILED;
    }
    switch(pstHdmiArcCapParams->eCodecType)
    {
    case E_HS_AOUT_CODEC_LPCM:
        stArcCap.bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM] = HI_TRUE;
        break;
    case E_HS_AOUT_CODEC_DD:
        stArcCap.bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_AC3] = HI_TRUE;
        break;
    case E_HS_AOUT_CODEC_DDP:
        stArcCap.bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_DDP] = HI_TRUE;
        break;
    case E_HS_AOUT_CODEC_DTS:
        stArcCap.bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_DTS] = HI_TRUE;
        break;
    case E_HS_AOUT_CODEC_MAX:
        s32Ret = HI_UNF_SND_SetArcCap(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_ARC0, &stArcCap);
        memset(&stArcCap, 0, sizeof(HI_UNF_SND_ARC_AUDIO_CAP_S));
        break;
    default:
        break;
    }
    return s32Ret;
}
HS_HAL_RESULT HS_HAL_EXTIN_SetAttr(HS_HANDLE hExtInHandle,HS_EXTIN_SetAttr_e eAttrType,void *pAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8  u8Port = 0;
    HI_UNF_HDMIRX_EDID_S stEdid;
    HI_U32 au32EdidData[256];
    for(u8Port = HI_UNF_HDMIRX_PORT0;u8Port<HI_UNF_HDMIRX_PORT3;u8Port++)
    {
        stEdid.pu32Edid = au32EdidData;
        stEdid.enPort   = u8Port;

    s32Ret = HI_UNF_HDMIRX_GetEdid(&stEdid);
    if(s32Ret != HI_SUCCESS)
    {
        return HS_HAL_ERR_FAILED;
    }

    if(HS_EXTIN_SetAttr_UpdateAudioEdid == eAttrType)
    {
        HI_CEC_SetDdpAtmosEn(stEdid.pu32Edid, HI_TRUE);
        HI_UNF_HDMIRX_UpdateEdid(&stEdid);
    }
    else if(HS_EXTIN_SetAttr_RestoreAudioEdid == eAttrType)
    {
        HI_CEC_SetDdpAtmosEn(stEdid.pu32Edid, HI_FALSE);
        HI_UNF_HDMIRX_UpdateEdid(&stEdid);
    }
    else
    {
        return HS_HAL_ERR_FAILED;
    }
   }
    return HS_HAL_ERR_OK;
}
HS_HAL_RESULT HS_HAL_GPIO_GetOnOff(E_HS_GPIO_Level *peLevel, HS_BOOL *pbInvert)
{
        HI_BOOL bConnected;
        HI_S32 s32Ret;
        HI_UNF_HDMIRX_PORT_E enPort;
        HS_CHAR hkProduct[PROPERTY_VALUE_MAX] = {0};

        property_get("ro.jamdeo.tv.hk_product", hkProduct, "0");
        if (strcmp(hkProduct, "1") != 0)
        {
            enPort = HI_UNF_HDMIRX_PORT2;
        }
        else
        {
            enPort = HI_UNF_HDMIRX_PORT1;
        }
        s32Ret = HI_UNF_HDMIRX_GetOffLineDetStatus(enPort, &bConnected);
        if(HI_FAILURE == s32Ret)
        {
            return HS_HAL_ERR_FAILED;
        }
        if(bConnected)
        {
            *peLevel = E_HS_GPIO_LEVEL_ON;
        }
        else
        {
            *peLevel = E_HS_GPIO_LEVEL_OFF;
        }
        *pbInvert = HS_FALSE;
        return HS_HAL_ERR_OK;
}








