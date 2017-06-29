/*********************************************************************
* ��Ȩ���� (C)2016, �ൺ���ŵ����ɷ����޹�˾
* �ļ�����: hdmi_cec.cpp
* �ļ���ʶ: ��
* ����ժҪ: ʵ����CEC�豸��ע�ᡢ��ʼ����ͬʱ����source�豸����������Ϣ,
* ����CEC��Ϣ��source�豸*
* ����˵��:
* ��ǰ�汾: V3.0
* ��    ��: chenweilan
* �������: 2016��7��12��
*
**********************************************************************/

#define LOG_TAG "HDMI_CEC"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <sys/signal.h>
#include <cutils/native_handle.h>
#include "hdmi_cec.h"
#include "cutils/properties.h"

/**********************************************************************
* ��������:   CecOptoHal
* ���������� ������ת��
* ���������op �ϲ������
* �����������
* �� �� ֵ��hal��ʹ�õĲ�����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                   chenweilan                    ����
***********************************************************************/

static E_HS_CEC_OpCode CecOptoHal(cec_message_type eOp)
{
    for (HS_U32 u32Loop = 0; u32Loop < (sizeof(atAndroid2Msopcode) / sizeof(T_Android2Msopcode)); u32Loop++)
    {
        if (atAndroid2Msopcode[u32Loop].eCecMsg == eOp)
        {
            return atAndroid2Msopcode[u32Loop].eEopcode;
        }
    }

    return E_HS_CEC_OP_ABORT_MESSAGE;
}


/**********************************************************************
* ��������:   CecOptoAn
* ����������������ת��
* ���������op �ϲ������
* �����������
* �� �� ֵ��android��ʹ�õĲ�����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static cec_message_type CecOptoAn(E_HS_CEC_OpCode eOp)
{
    for (HS_U32 u32Loop = 0; u32Loop < (sizeof(atAndroid2Msopcode) / sizeof(T_Android2Msopcode)); u32Loop++)
    {
        if (atAndroid2Msopcode[u32Loop].eEopcode == eOp)
        {
            return atAndroid2Msopcode[u32Loop].eCecMsg;
        }
    }
    return (cec_message_type) (CEC_MESSAGE_ABORT + 1);
}

/**********************************************************************
* ��������:   HsCecInstallEnv
* ���������� ��װCEC����
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ��0  �ɹ� otherʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                   chenweilan                    ����
***********************************************************************/

static HS_S32 HsCecInstallEnv(T_HisenseHdmiCecDevice *ptDev)
{
    HS_S32 s32Status     = -EINVAL;
    HS_U8   u8Loop       = 0;
    HS_HAL_RESULT res    = HS_HAL_ERR_FAILED;
    HS_CEC_InitParams_t stInitParams;
    T_HS_CEC_OpenParams stOpenParams;
    HS_EXTIN_InitParams_t stExtinInitParams;
    HS_AOUT_InitParams_t stAoutInitParam;
    HS_AOUT_Attr_t stAoutOpenParams;
    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }
    ptDev->cec_tid                = 0;
    ptDev->eMyDeviceType          = E_MAPI_DEVICE_TYPE_TV;
    ptDev->u32CecHandle           = HS_INVALID_HANDLE;
    ptDev->u32AoutHdmiHandle      = HS_INVALID_HANDLE;
    ptDev->eMyLogicalAddress      = E_HS_CEC_LA_TV;    //TV:0 for TV
    ptDev->u8MyPhysicalAddress[0] = 0x00;
    ptDev->u8MyPhysicalAddress[1] = 0x00;

    for (u8Loop = 0; u8Loop < CEC_PORT_NUM_MAX; u8Loop++)
    {
        ptDev->atHdmiPort[u8Loop].eSignal = E_HS_SIGNAL_STATUS_NOTSUPPORT;
        ptDev->atHdmiPort[u8Loop].eLevel  = E_HS_GPIO_LEVEL_OFF;
    }
    INIT_ST(stInitParams);
    stInitParams.u32XtalClk = CEC_M_CLOCK;//12MHz

    res = HS_HAL_CEC_Init(&stInitParams);
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("HS CEC Init Failed. res[%d] \n", res);
        return s32Status;
    }
    res = HS_HAL_GPIO_Init();
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("init GPIO  Failed. res[%d] \n", res);
        return s32Status;
    }

    INIT_ST(stOpenParams);
    res = HS_HAL_CEC_Open(&stOpenParams, &ptDev->u32CecHandle);
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("MI CEC Open Failed. res[%d] \n", res);
        return s32Status;
    }

    res = HS_HAL_CEC_Enable(ptDev->u32CecHandle);
    if (HS_HAL_ERR_OK != res)
    {
        SLOGE("HS CEC Enable Failed. res[%d] \n", res);
        return s32Status;
    }

    stAoutOpenParams.eOpenPath              = E_HS_AOUT_PATH_HDMI;
    stAoutOpenParams.eOutputSourceInfo      = E_HS_AOUT_MAIN;
    stAoutOpenParams.stVolCfg.bUseCusVolTbl = HS_FALSE;
    stAoutOpenParams.stVolCfg.pu16VolTbl    = NULL;

    res = HS_HAL_AOUT_Open(&stAoutOpenParams, &ptDev->u32AoutHdmiHandle);
    if (HS_HAL_ERR_OK != res)
    {
        HS_HANDLE AoutHdmihandle = HS_INVALID_HANDLE;
        SLOGE("HS Aout Hdmi Open Failed. res[%d] \n", res);
        res = HS_HAL_AOUT_GetHandle(&AoutHdmihandle, E_HS_AOUT_PATH_HDMI);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("HS Aout Hdmi Get Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        res = HS_HAL_AOUT_DupHandle(AoutHdmihandle, HS_TRUE, &ptDev->u32AoutHdmiHandle);
        if (HS_HAL_ERR_OK != res)
        {
            SLOGE("HS Aout HDMI Dup Handle Failed. res[%d] \n", res);
            return s32Status;
        }

        SLOGE("HS Aout HDMI Get Handle Successful. res[%d] \n", res);
    }
    return 0;
}
/**********************************************************************
* ��������:  HsCecUninstallEnv
* ���������� ж��cec����
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static HS_S32 HsCecUninstallEnv(T_HisenseHdmiCecDevice *ptDev)
{
    HS_S32 s32Status = -EINVAL;

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    if (HS_INVALID_HANDLE != ptDev->u32CecHandle)
    {
        if (HS_HAL_ERR_OK != HS_HAL_CEC_Disable(ptDev->u32CecHandle))
        {
            SLOGE("HS CEC Disable Failed \n");
        }

        if (HS_HAL_ERR_OK != HS_HAL_CEC_Close(ptDev->u32CecHandle))
        {
            SLOGE("HS CEC Close Failed \n");
        }

        ptDev->u32CecHandle = HS_INVALID_HANDLE;
    }
    if (HS_INVALID_HANDLE != ptDev->u32AoutHdmiHandle)
    {
        if (HS_HAL_ERR_OK != HS_HAL_AOUT_Close(ptDev->u32AoutHdmiHandle))
        {
            SLOGE("HS AOUT Hdmi Close Failed \n");
        }

        ptDev->u32AoutHdmiHandle = HS_INVALID_HANDLE;
    }
    if (HS_HAL_ERR_OK != HS_HAL_CEC_DeInit())
    {
        SLOGE("HS CEC DeInit Failed \n");
    }
    return 0;
}

/**********************************************************************
* ��������: SetShortAudioDescript
* ��������: ����SAD������Ϣ
* �������: ptDev Ӳ���豸 pSAD SAD��Ϣ����  u8SadLen ���ݳ���
* �������: ��
* �� �� ֵ��1 �ɹ� 0 ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static HS_BOOL SetShortAudioDescript(T_HisenseHdmiCecDevice *ptDev, HS_U8 *pu8Sad, HS_U8 u8SadLen)
{
    HS_BOOL bRet   = HS_FALSE;
    HS_S32  s32Idx = 0;
    HS_AOUT_HdmiArcCapability_t stSAD;
    E_HS_AOUT_CodecType enAudioFormatCode = E_HS_AOUT_CODEC_NONE;
    HS_AOUT_HdmiArcCapabilityParams_t stHdmiArcCapabilityParams;
    char dtsAPtest[PROPERTY_VALUE_MAX];

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return HS_FALSE;
    }

    if ((NULL == pu8Sad)||(u8SadLen > SAD_LEN_MAX))
    {
        return HS_FALSE;
    }
    memset(&stSAD, 0, sizeof(HS_AOUT_HdmiArcCapability_t));
    stHdmiArcCapabilityParams.eAoutPath = E_HS_AOUT_PATH_HDMI_ARC;
    property_get("debug.hisense.tv.dtsAPtest", dtsAPtest, "0");
    //just enable PCM before we get the actually ARC infomation
    if (E_HS_AOUT_CODEC_NONE == (E_HS_AOUT_CodecType) pu8Sad[0])
    {
        stHdmiArcCapabilityParams.eCodecType                              = E_HS_AOUT_CODEC_LPCM;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportChannel    = stSAD.u8SupportChannel;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSampleRate = stSAD.u8SupportSampleRate;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSadByte3   = stSAD.u8SupportSadByte3;
        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);
        if(strcmp(dtsAPtest, "1") == 0)
        {
            SLOGE("[CEC] set HdmiArcCapability to DD/DDP/DTS,when do DTS test use Audio Precision x500");

            stHdmiArcCapabilityParams.eCodecType                              = E_HS_AOUT_CODEC_DD;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportChannel    = stSAD.u8SupportChannel;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSampleRate = stSAD.u8SupportSampleRate;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSadByte3   = stSAD.u8SupportSadByte3;
	        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);
	        stHdmiArcCapabilityParams.eCodecType                              = E_HS_AOUT_CODEC_DDP;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportChannel    = stSAD.u8SupportChannel;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSampleRate = stSAD.u8SupportSampleRate;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSadByte3   = stSAD.u8SupportSadByte3;
	        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);
	        stHdmiArcCapabilityParams.eCodecType                              = E_HS_AOUT_CODEC_DTS;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportChannel    = stSAD.u8SupportChannel;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSampleRate = stSAD.u8SupportSampleRate;
	        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSadByte3   = stSAD.u8SupportSadByte3;
	        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);
        }
        stHdmiArcCapabilityParams.eCodecType                              = E_HS_AOUT_CODEC_MAX;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportChannel    = stSAD.u8SupportChannel;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSampleRate = stSAD.u8SupportSampleRate;
        stHdmiArcCapabilityParams.stHdmiArcCapability.u8SupportSadByte3   = stSAD.u8SupportSadByte3;
        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);

        return HS_TRUE;
    }
    for (s32Idx = 0; s32Idx < u8SadLen; s32Idx += SAD_LEN)
    {
        SLOGE("[CEC] SAD%d: %2x-%2x-%2x\n",s32Idx,pu8Sad[s32Idx],pu8Sad[s32Idx+1],pu8Sad[s32Idx+2]);
        enAudioFormatCode         =(E_HS_AOUT_CodecType)((pu8Sad[s32Idx] & AUDIO_FORMAT_CODE)>>3);   //the first byte 4~7 bit mean the audio format code.
        stSAD.bSupport            = HS_TRUE;
        stSAD.u8SupportChannel    = pu8Sad[s32Idx] & AUDIO_SUPPORT_CHANNEL;
        stSAD.u8SupportSampleRate = pu8Sad[s32Idx+1] & AUDIO_SAMPLE_RATE;
        stSAD.u8SupportSadByte3   = pu8Sad[s32Idx+2];
        stHdmiArcCapabilityParams.stHdmiArcCapability = stSAD;
        stHdmiArcCapabilityParams.eCodecType = enAudioFormatCode;

        SLOGE("[CEC] enAudioFormatCode 0x%x, E_HS_AOUT_CODEC_DDP=0x%x\n",enAudioFormatCode, E_HS_AOUT_CODEC_DDP);

        /*enable all supported Format to avoid output PCM to DD player that anounnce support DDP,but actually just support DD.*/
        HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);

        if (enAudioFormatCode == E_HS_AOUT_CODEC_DDP)
        {
            if (((stSAD.u8SupportSadByte3)&AUDIO_SUPPORT_SADBYTE) == AUDIO_SUPPORT_SADBYTE)//byte3 bit0
            {
                bRet = HS_TRUE;
            }
        }
    }
    stHdmiArcCapabilityParams.eCodecType = E_HS_AOUT_CODEC_MAX;
    HS_HAL_AOUT_SetAttr(ptDev->u32AoutHdmiHandle, E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY, (void*)&stHdmiArcCapabilityParams);
    //HS_AOUT_Close(hAoutHandle);
    return bRet;
}

/**********************************************************************
* ��������: HsCec_TxMsgRequestShortAudioDescriptor
* ������������ȡ�����豸����Ƶ������Ϣ
* �������: ptDev Ӳ���豸 eDstLogAddr  �߼���ַ
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
*  2016.7.12                           chenweilan                          ����
***********************************************************************/

static void HsCecTxMsgRequestShortAudioDescriptor(T_HisenseHdmiCecDevice *ptDev, E_HS_CEC_DeviceLA eDstLogAddr)
{
    //need get audio format which TV support

    E_HS_CEC_ErrorCode eCecErrCode = E_HS_CEC_FEATURE_ABORT;
    T_HS_CEC_MsgInfo stCecMsgInfo;
    char dtsAPtest[PROPERTY_VALUE_MAX];
    HS_U8 u8DefaultValue[SAD_LEN_MAX] = {0};
	property_get("debug.hisense.tv.dtsAPtest", dtsAPtest, "0");
	if(strcmp(dtsAPtest, "1") == 0)
	{
        //call audio api to set SAD to support DTS for DTS test
        SetShortAudioDescript(ptDev, u8DefaultValue,SAD_LEN_MAX);
    }
    memset(&stCecMsgInfo, 0, sizeof(T_HS_CEC_MsgInfo));
    stCecMsgInfo.enHeader  = eDstLogAddr;
    stCecMsgInfo.enOpcode  = E_HS_CEC_OP_SAC_REQUEST_SHORT_AUDIO_DESCRIPTOR;
    stCecMsgInfo.szPara[0] = E_HS_CEC_ARC_AUDIO_FORMAT_AC3;
    stCecMsgInfo.szPara[1] = E_HS_CEC_ARC_AUDIO_FORMAT_AAC;
    stCecMsgInfo.szPara[2] = E_HS_CEC_ARC_AUDIO_FORMAT_DTS;
    stCecMsgInfo.szPara[3] = E_HS_CEC_ARC_AUDIO_FORMAT_EAC3;
    stCecMsgInfo.u8MsgLen  = SAD_MSG_LEN;
    HS_HAL_CEC_SendTxMsg(ptDev->u32CecHandle,&stCecMsgInfo,&eCecErrCode);
    if (eCecErrCode & E_HS_CEC_TX_SUCCESS)
    {
        SLOGE("send Tx Msg E_HS_CEC_OP_SAC_REQUEST_SHORT_AUDIO_DESCRIPTOR success\n");
    }

}
/**********************************************************************
* ��������: HsCecRequestShortAudioDescripe
* �������������ϲ��ȡSAD��Ϣ�Ľӿ�
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsCecRequestShortAudioDescripe(const struct hdmi_cec_device *ptDev)
{
    T_HisenseHdmiCecDevice *prev = (T_HisenseHdmiCecDevice *) ptDev;
    SLOGE("[CEC]Request Short Audio Descriptor \n");
    HsCecTxMsgRequestShortAudioDescriptor(prev, E_HS_CEC_LA_AUDIO_SYS);
}
/**********************************************************************
* ��������:  HsCecRequestShortAudioDescripe
* ��������:  ��ԭEDID
* �������:  ptDevӲ���豸
* �������:  ��
* �� �� ֵ:  ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsCecRequestResetShortAudioDescripe(const struct hdmi_cec_device *ptDev)
{
    T_HisenseHdmiCecDevice* prev = (T_HisenseHdmiCecDevice *) ptDev;
    SLOGE("[CEC]Request reset Short Audio Descriptor \n");
    if (g_bUpdateEdidFromSAD == HS_TRUE)
    {
        HS_U8 u8DefaultValue[SAD_LEN_MAX] = {0};
        //call audio api to set SAD to default
        SetShortAudioDescript(prev, u8DefaultValue,SAD_LEN_MAX);
        if (HS_HAL_ERR_OK != HS_HAL_EXTIN_SetAttr(HS_INVALID_HANDLE, HS_EXTIN_SetAttr_RestoreAudioEdid, NULL))
        {
            SLOGE("HS_EXTIN_SetAttr failed! \n");
        }
        SLOGE("[CEC]reset EDID\n");
        g_bUpdateEdidFromSAD = HS_FALSE;
    }
}
/**********************************************************************
* ��������: HsCecProcessRxcmdCore
* ���������������ֽ��յ�����Ϣ
* ���������ptDev Ӳ���豸 CecMsgInfo���յ�����Ϣ
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
*  2016.7.12                    chenweilan                  ����
***********************************************************************/

static void HsCecProcessRxcmdCore(T_HisenseHdmiCecDevice *ptDev, T_HS_CEC_MsgInfo *CecMsgInfo)
{
    HS_U8 u8Parameter[SAD_LEN_MAX]    = {0};
    HS_U8 u8DefaultValue[SAD_LEN_MAX] = {0};
    HS_U8 u8SadLen                    = 0;
    HS_BOOL bUpdateEdid               = HS_FALSE;
    HS_HAL_RESULT eRet                = HS_HAL_ERR_FAILED;
    E_HS_CEC_OpCode CecOpcode         = E_HS_CEC_OP_ACTIVE_SOURCE;
    E_HS_CEC_ErrorCode sResErrCode    = E_HS_CEC_FEATURE_ABORT;
    T_HS_CEC_MsgInfo stCecTxMsg;
    HS_EXTIN_Edid_Data_t stEdidData;
    if (NULL == ptDev || NULL == CecMsgInfo)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }

    stCecTxMsg.enOpcode = (E_HS_CEC_OpCode) CecMsgInfo->enOpcode;
    stCecTxMsg.enHeader = CecMsgInfo->enHeader;
    stCecTxMsg.u8MsgLen = CecMsgInfo->u8MsgLen;

    CecOpcode = CecOptoHal((cec_message_type) CecMsgInfo->enOpcode);
    //CEC1.3a Table 7 ~ 27
    switch (CecOpcode)
    {
        case E_HS_CEC_OP_SAC_REPORT_SHORT_AUDIO_DESCRIPTOR:
        {
            SLOGE("[CEC]E_HS_CEC_OP_SAC_REPORT_SHORT_AUDIO_DESCRIPTOR \n");
            for (u8SadLen=0; u8SadLen +2 < CecMsgInfo->u8MsgLen; u8SadLen++)
            {
                u8Parameter[u8SadLen] = CecMsgInfo->szPara[u8SadLen];
            }
            //call audio api to set SAD
            bUpdateEdid = SetShortAudioDescript(ptDev, u8Parameter,u8SadLen);
            if ((bUpdateEdid == HS_TRUE) && (g_bUpdateEdidFromSAD == HS_FALSE))
            {
                SLOGE("[CEC]update EDID\n");
            }
            if ((bUpdateEdid == HS_TRUE)&&(g_bUpdateEdidFromSAD == HS_FALSE))
            {
                //update edid
                stEdidData.pu8Info    = u8Parameter;
                stEdidData.u32InfoLen = u8SadLen;
                if (HS_HAL_ERR_OK != HS_HAL_EXTIN_SetAttr(HS_INVALID_HANDLE , HS_EXTIN_SetAttr_UpdateAudioEdid,&stEdidData))
                {
                    SLOGE("HS_EXTIN_SetAttr failed! \n");
                    return ;
                }
                g_bUpdateEdidFromSAD = HS_TRUE;
            }
            else
            {
                SLOGE("[CEC] Dont't need update EDID. \n");
            }
            g_bInitArcFinish = HS_TRUE;
            break;
        }
        case E_HS_CEC_OP_ARC_TERMINATED_ARC:
        {
            SLOGE("[CEC]E_HS_CEC_OP_ARC_TERMINATED_ARC\n");
            //short audio descriptor reset
            if (g_bUpdateEdidFromSAD == HS_TRUE)
            {
                //call audio api to set SAD to default
                SetShortAudioDescript(ptDev, u8DefaultValue,SAD_LEN_MAX);
                //reset edid
                if (HS_HAL_ERR_OK != HS_HAL_EXTIN_SetAttr(HS_INVALID_HANDLE, HS_EXTIN_SetAttr_RestoreAudioEdid, NULL))
                {
                    SLOGE("HS_EXTIN_SetAttr failed! \n");
                }
                SLOGE("[CEC]reset EDID\n");
                g_bUpdateEdidFromSAD = HS_FALSE;
            }
            break;
        }
        case E_HS_CEC_OP_PS_GIVE_POWER_STATUS:
        {
            SLOGE("[CEC]E_HS_CEC_OP_PS_GIVE_POWER_STATUS\n");
            HS_HAL_CEC_SendTxMsg(ptDev->u32CecHandle,&stCecTxMsg,&sResErrCode);
            SLOGE("ResErrCode is %d\n",(HS_S32)sResErrCode);
            break;
        }
        //if in standby, firstly power on
        case E_HS_CEC_OP_OTP_IMAGE_VIEW_ON:
        {
            SLOGE("[CEC] E_HS_CEC_OP_OTP_IMAGE_VIEW_ON\n");
            break;
        }
        case E_HS_CEC_OP_OTP_TEXT_VIEW_ON:
        {
            SLOGE("[CEC]E_HS_CEC_OP_OTP_TEXT_VIEW_ON\n");
            break;
        }
        case E_HS_CEC_OP_STANDBY:
        {
            SLOGE("[CEC]E_HS_CEC_OP_STANDBY\n");
            break;
        }
        case E_HS_CEC_OP_ARC_INITIATE_ARC:
        {
            SLOGE("[CEC]E_HS_CEC_OP_ARC_INITIATE_ARC\n");
            break;
        }
        default:
        {
            break;
        }
    }
}
/**********************************************************************
* ��������: HsCecProcessRxCmd
* ��������������cmd
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsCecProcessRxCmd(T_HisenseHdmiCecDevice *ptDev)
{
    T_HS_CEC_MsgInfo CecMsgInfo;
    HS_U8 u8Fifoidx = 0;
    HS_U8 u8FifoCnt = 0;

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }

    //HS_HAL_CEC_GetRxFifoIdx(ptDev->u32CecHandle, &u8Fifoidx);
    if (HS_TRUE != ptDev->s32HdmiOptionEnableCec)
    {
        SLOGE("Discard CEC Message due to cec disabled !!!\n");
        return;
    }
    memset(&CecMsgInfo,0,sizeof(T_HS_CEC_MsgInfo));
    HS_HAL_CEC_GetRxMsgInfo(ptDev->u32CecHandle, &CecMsgInfo);

    HsCecProcessRxcmdCore(ptDev, &CecMsgInfo);

    if (ptDev->CallBack)
    {
        hdmi_event_t event;

        SLOGE("Notify Received Message to Hdmi Control Service !!!\n");
        event.type = HDMI_EVENT_CEC_MESSAGE;
        event.dev = (struct hdmi_cec_device*) ptDev;
        event.cec.initiator = (cec_logical_address_t) ((HS_S32)CecMsgInfo.enHeader>>4);
        event.cec.destination = (cec_logical_address_t)  ((HS_S32) CecMsgInfo.enHeader & 0x0f);
        memcpy(&event.cec.body[0],&CecMsgInfo.enOpcode, 1);
        if (CecMsgInfo.u8MsgLen > 2)
        {
            memcpy(&event.cec.body[1],  &CecMsgInfo.szPara[0], (CecMsgInfo.u8MsgLen - 2));
        }
        event.cec.length = CecMsgInfo.u8MsgLen - 1;
        SLOGE("Receive msg From[%d], To[%d], OP[%#x]!!!\n", event.cec.initiator, event.cec.destination, event.cec.body[0]);
        if (event.cec.length > 0)
        {
            SLOGE("Notify Receive data: \n=======\n");
            for (HS_U32 s32loop= 0; s32loop < event.cec.length; s32loop++)
            {
                SLOGE("%#x", event.cec.body[s32loop]);
            }
            SLOGE("=======\n");
        }
        ptDev->CallBack(&event, ptDev->CallbackParam);
    }
    return;

}
/**********************************************************************
* ��������: HsCecTxmsgPing
* ����������ping�豸�ӿ�
* ���������ptDev Ӳ���豸 dest_addr Ŀ�ĵ�ַ
* �����������
* �� �� ֵ��E_HS_CEC_TX_SUCCESS �ɹ� otherʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                    ����
***********************************************************************/

static E_HS_CEC_ErrorCode HsCecTxmsgPing(T_HisenseHdmiCecDevice *ptDev, E_HS_CEC_DeviceLA eDestAddr)
{
    E_HS_CEC_ErrorCode res = E_HS_CEC_FEATURE_ABORT;

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return res;
    }
    // Logical Address 12, 13 are reserved
    if (eDestAddr > E_HS_CEC_LA_MAX)
    {
        return res;
    }
    if (HS_HAL_CEC_SendTxPolling(ptDev->u32CecHandle, eDestAddr, &res) != HS_HAL_ERR_OK)
    {
        res = E_HS_CEC_FEATURE_ABORT;
       // SLOGE("HS_CEC_SendTxPolling Failed \n");
    }
    if (eDestAddr == E_HS_CEC_LA_AUDIO_SYS)
    {
        SLOGE("HS_CEC_SendTxPolling, ErrCode[%d] \n", res);
    }
    usleep(CEC_TRANSITION_DELAY); //16.8ms
    return res;
}


/**********************************************************************
* ��������:   HsCecPortCheck
* ���������� ���port��״̬
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static void HsCecPortCheck(T_HisenseHdmiCecDevice *ptDev)
{
    HS_U8 u8Loop    = 0;
    HS_U8 u8Count = 0;
    HS_U8 u8GpioNum    = HS_GPIO_HDMI2_5V;
    HS_BOOL  bPing  = HS_FALSE;
    HS_BOOL bInvert = HS_FALSE;
    E_HS_GPIO_Level eLevel       = E_HS_GPIO_LEVEL_ON;
    static HS_BOOL bArcConnected = HS_FALSE;
    T_HisenseHdmiCecDevice *priv = (T_HisenseHdmiCecDevice *) ptDev;
    char tvSeperate[PROPERTY_VALUE_MAX];
    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }

    property_get("ro.hisense.tv.seperate", tvSeperate, "0");
    if (strcmp(tvSeperate, "1") == 0)
    {
        u8Loop = 0;
    }
    else
    {
        u8Loop = 1;
    }

    if (HS_HAL_ERR_OK !=  HS_HAL_GPIO_GetOnOff(&eLevel, &bInvert))
    {
        SLOGE("Gpio get elevel Failed \n");
    }
    if (HS_TRUE == bInvert)
    {
        if (E_HS_GPIO_LEVEL_OFF == eLevel)
        {
            eLevel = E_HS_GPIO_LEVEL_ON;
        }
        else
        {
            eLevel = E_HS_GPIO_LEVEL_OFF;
        }
    }

    if (E_HS_GPIO_LEVEL_OFF == eLevel && E_HS_GPIO_LEVEL_ON == ptDev->atHdmiPort[u8Loop].eLevel)
    {
        ptDev->atHdmiPort[u8Loop].eLevel = E_HS_GPIO_LEVEL_OFF;
        SLOGE("HDMI port[%d] disconnect \n", (u8Loop + 1));


        if (bArcConnected == HS_TRUE)
        {
            if (ptDev->CallBack)
            {
                hdmi_event_t event;
                SLOGE("Notify Hotplug event to Hdmi Control Service !!!\n");
                event.type = HDMI_EVENT_HOT_PLUG;
                event.dev = (struct hdmi_cec_device*) ptDev;
                event.hotplug.connected = HDMI_NOT_CONNECTED;
                event.hotplug.port_id = u8Loop + 1;
                ptDev->CallBack(&event, ptDev->CallbackParam);
            }
        }
        bArcConnected = HS_FALSE;
  }
  else if (E_HS_GPIO_LEVEL_ON == eLevel && E_HS_GPIO_LEVEL_OFF == ptDev->atHdmiPort[u8Loop].eLevel)
  {
      ptDev->atHdmiPort[u8Loop].eLevel = E_HS_GPIO_LEVEL_ON;
      SLOGE(" HDMI port[%d] connect \n", (u8Loop + 1));

      for (u8Count = 0;u8Count < 5;u8Count++)
      {
          if (E_HS_CEC_TX_SUCCESS == HsCecTxmsgPing(priv, E_HS_CEC_LA_AUDIO_SYS))
          {
              bPing = HS_TRUE;
              break;
          }
          else
          {
              bPing = HS_FALSE;
          }
     }
     if ((bArcConnected ==HS_FALSE) && (bPing == HS_TRUE))
     {
         if (ptDev->CallBack)
         {
             hdmi_event_t event;

             SLOGE("Notify Hotplug event to Hdmi Control Service !!!\n");
             event.type = HDMI_EVENT_HOT_PLUG;
             event.dev = (struct hdmi_cec_device*) ptDev;
             event.hotplug.connected = HDMI_CONNECTED;
             event.hotplug.port_id = u8Loop + 1;
             ptDev->CallBack(&event, ptDev->CallbackParam);
         }
         bArcConnected = HS_TRUE;
     }
 }
}
/**********************************************************************
* ��������:   HsCecMornitor
* ���������� cec�źż��
* ��������� ptDev Ӳ���豸
* �����������
* �� �� ֵ    ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
*2016.7.12                      chenweilan                  ����
***********************************************************************/

static void* HsCecMornitor(void *pDev)
{
    HS_U8 u8MsgCnt = 0;
    T_HisenseHdmiCecDevice *ptDevice = (T_HisenseHdmiCecDevice *) pDev;
    SLOGE("HsCecMornitor enter \n");

    if (NULL == ptDevice)
    {
        SLOGE("Invalid Parameter \n");
        return NULL ;
    }

    while (!ptDevice->bThreadexit)
    {
        HsCecPortCheck(ptDevice);

        u8MsgCnt = 0;
        HS_HAL_CEC_GetRxMsgCnt(ptDevice->u32CecHandle, &u8MsgCnt);
        if (u8MsgCnt != 0)
        {
            HsCecProcessRxCmd(ptDevice);
            //HS_HAL_CEC_GetRxMsgCnt(device->u32CecHandle, &msgcnt);
            //HS_HAL_CEC_SetRxMsgCnt(device->u32CecHandle, (msgcnt - 1));
        }

        usleep(10000);
    }
    SLOGE("hs_cec_mornitor exit \n");
    return NULL ;
}
/**********************************************************************
* ��������:   HsCecStart
* ���������� ����CEC�Ľ���
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ�� 0  �ɹ� other  ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                    ����
***********************************************************************/
static HS_S32 HsCecStart(T_HisenseHdmiCecDevice *ptDev)
{
    pthread_attr_t thread_attr;
    HS_S32 s32Status = -EINVAL;

    SLOGE("hs_cec_start enter \n");
    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    if (HS_HAL_ERR_OK != HS_HAL_CEC_Enable(ptDev->u32CecHandle))
    {
        SLOGE("HS CEC Enable Failed \n");
        return s32Status;
    }

    ptDev->bThreadexit = HS_FALSE;

    if (PTH_RET_CHK_CEC(pthread_attr_init(&thread_attr)))
    {
        SLOGE("pthread_attr_init failed \n");
        return s32Status;
    }

    //thread will not exit after return until join
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    pthread_attr_setstacksize(&thread_attr,CEC_STACK_SIZE);
    if (PTH_RET_CHK_CEC(pthread_create(&ptDev->cec_tid,
                               &thread_attr,
                               HsCecMornitor,
                               ptDev)))
    {
        SLOGE("pthread_create failed \n");
        return s32Status;
    }
    pthread_attr_destroy(&thread_attr);
    SLOGE("hs_cec_start exit \n");
    return 0;
}

/**********************************************************************
* ��������: HsCecStop
* ����������ֹͣӲ���豸
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ��0  �ɹ� other  ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static HS_S32 HsCecStop(T_HisenseHdmiCecDevice *ptDev)
{
    HS_S32 s32Status = -EINVAL;
    void *result = NULL;
    SLOGE("hs_cec_stop enter \n");

    if (NULL == ptDev)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    ptDev->bThreadexit = HS_TRUE;
    PTH_RET_CHK_CEC(pthread_join(ptDev->cec_tid, &result));

    SLOGE("hs_cec_stop exit\n");
    return 0;
}

/**********************************************************************
* ��������: HsCecClose
* �����������ر�Ӳ���豸
* ���������ptDev Ӳ���豸
* �����������
* �� �� ֵ��0  �ɹ� other  ʧ��
* ����˵��:
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static HS_S32 HsCecClose(struct hw_device_t *ptDev)
{
    T_HisenseHdmiCecDevice *priv = (T_HisenseHdmiCecDevice *) ptDev;
    HS_S32 s32Status = -EINVAL;
    SLOGE("mscec_close enter \n");

    if (NULL == priv)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    (void) HsCecStop(priv);
    (void) HsCecUninstallEnv(priv);
    if (priv)
    {
        free(priv);
    }

    SLOGE("mscec_close exit \n");
    return 0;
}

/**********************************************************************
* ��������: HsCecAddLogicalAddress
* �������������豸�����߼���ַ
* ���������ptDev �����߼���ַ���豸  addr ���ӵ��߼���ַ
* �����������
* �� �� ֵ��0  �ɹ� other  ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                   chenweilan                   ����
***********************************************************************/

static HS_S32 HsCecAddLogicalAddress(const struct hdmi_cec_device *ptDev, cec_logical_address_t tAddr)
{
    T_HisenseHdmiCecDevice *priv = (T_HisenseHdmiCecDevice *) ptDev;
    HS_S32 s32Status = -EINVAL;
    SLOGV("mscec_add_logical_address [%d] \n", tAddr);

    if (NULL == priv)
    {
        SLOGE("Invalid Parameter \n");
        return s32Status;
    }

    if (tAddr > CEC_ADDR_BROADCAST)
    {
        SLOGE("Invalid addr[%d] \n", tAddr);
        return s32Status;
    }

    priv->eCecDeviceSupported[tAddr] = LOGICAL_ADDR_SET ;
    return 0;
}

/**********************************************************************
* ��������: HsCecClearLogicalAddress
* ��������������豸�߼���ַ
* ���������Ӳ�������豸
* �����������
* �� �� ֵ��0  �ɹ� other  ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/
static void HsCecClearLogicalAddress(const struct hdmi_cec_device* dev)
{
    T_HisenseHdmiCecDevice *priv = (T_HisenseHdmiCecDevice *) dev;
    HS_S32 s32Loop = 0;
    HS_S32 s32Status = -EINVAL;
    SLOGE("mscec_clear_logical_address \n");

    if (NULL == priv)
    {
        SLOGE("Invalid Parameter \n");
        return;
    }

    for (s32Loop = 0; s32Loop < E_HS_CEC_LA_MAX; s32Loop++)
    {
        priv->eCecDeviceSupported[s32Loop] = LOGICAL_ADDR_UNSET ;
    }

    return;
}

/**********************************************************************
* ��������: HsCecGetPhysicalAddress
* ������������ȡTV�������ַ
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static HS_S32 HsCecGetPhysicalAddress(const struct hdmi_cec_device *pDev, uint16_t *pDddr)
{
    HS_S32 s32Status = -EINVAL;
    T_HisenseHdmiCecDevice *priv = (T_HisenseHdmiCecDevice *) pDev;
    SLOGE("mscec_get_physical_address enter \n");

    if (NULL == priv || NULL == pDddr)
    {
        SLOGE("invalid parameter \n");
        return s32Status;
    }
    *pDddr = 0x0000;
    SLOGE("mscec_get_physical_address exit \n");
    return 0;
}

/**********************************************************************
* ��������: HsCecSendMessage
* ������������cec�豸����msg
* ���������ptDev Ӳ���豸  msg���͵���Ϣ
* �������: ��
* �� �� ֵ��0  �ɹ� other  ʧ��
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static HS_S32 HsCecSendMessage(const struct hdmi_cec_device *ptDev, const cec_message_t *ptMsg)
{
    T_HisenseHdmiCecDevice *ptPriv = (T_HisenseHdmiCecDevice *) ptDev;
    E_HS_CEC_ErrorCode eResErrCode = E_HS_CEC_FEATURE_ABORT;
    E_HS_CEC_ErrorCode eCecErr     = E_HS_CEC_FEATURE_ABORT;
    T_HS_CEC_MsgInfo tTxMsgInfo;

    if (NULL == ptPriv || NULL == ptMsg)
    {
        SLOGE("invalid parameter \n");
        return HDMI_RESULT_FAIL;
    }

    if (HS_INVALID_HANDLE == ptPriv->u32CecHandle)
    {
        SLOGE("HS cec Hanlde Invalid \n");
        return HDMI_RESULT_FAIL;
    }
    if (ptMsg->destination == 5)
    {
        SLOGE("Send Message TO[%d] \n", ptMsg->destination);
        SLOGE("OP[%#x], %s \r\n========\r\n", ptMsg->body[0], (char *) auCecOpCodeStr[ptMsg->body[0]]);
    }

    eCecErr = HsCecTxmsgPing(ptPriv, (E_HS_CEC_DeviceLA) ptMsg->destination);
    if (E_HS_CEC_TX_SUCCESS != eCecErr)
    {
        if (ptMsg->destination == 5)
        {
            SLOGE("Sending message can't be response. \n");
        }
        return HDMI_RESULT_NACK;
    }
    //����Ϣ������͸�cec�豸
    memset(&tTxMsgInfo, 0x00, sizeof(tTxMsgInfo));
    tTxMsgInfo.enHeader = (E_HS_CEC_DeviceLA) ptMsg->destination;
    tTxMsgInfo.enOpcode = (E_HS_CEC_OpCode) CecOptoHal((cec_message_type) ptMsg->body[0]);

    //framework �㳤��    len =op+data������driverʱ��Ҫ+1
    tTxMsgInfo.u8MsgLen = ptMsg->length + 1;

    if (ptMsg->length > 1)
    {
        memcpy(tTxMsgInfo.szPara, &ptMsg->body[1], (ptMsg->length - 1));
        SLOGE("Send data: \n=======\n");
        for (HS_U32 s32Loop = 0; s32Loop < (ptMsg->length - 1); s32Loop++)
        {
            SLOGE("%#x", tTxMsgInfo.szPara[s32Loop]);
        }
        SLOGE("=======\n");
    }

    if (HS_HAL_ERR_OK != HS_HAL_CEC_SendTxMsg(ptPriv->u32CecHandle, &tTxMsgInfo, &eResErrCode))
    {
        SLOGE("HS_CEC_SendTxMsg Failed \n");
        return HDMI_RESULT_FAIL;
    }

    if (E_HS_CEC_OP_STANDBY == tTxMsgInfo.enOpcode && HS_TRUE == ptPriv->s32HdmiOptionWakeup)
    {
        SLOGE("Hand control over to the microprocessor before standby. \n ");
        if (HS_HAL_ERR_OK != HS_HAL_CEC_SetWakeUpConfig(ptPriv->u32CecHandle))
        {
            SLOGE("_CEC_SetWakeUpConfig Failed \n");
            return HDMI_RESULT_FAIL;
        }
    }
    return HDMI_RESULT_SUCCESS;
}

/**********************************************************************
* ��������: HsCecRegisterEventCallback
* ����������ע��ص�����
* ���������ptDev Ӳ���豸��callback �ص����� ��arg ����
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/
static void HsCecRegisterEventCallback(const struct hdmi_cec_device *ptDev,
            event_callback_t tCallBack, void *pArg)
{
    T_HisenseHdmiCecDevice *ptPriv = (T_HisenseHdmiCecDevice *) ptDev;
    if (NULL == ptPriv)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    if (tCallBack)
    {
        ptPriv->CallBack      = tCallBack;
        ptPriv->CallbackParam = pArg;
    }
    SLOGE("mscec_register_event_callback exit \n");

    return;
}

/**********************************************************************
* ��������: HsCecGetVersion
* ������������ȡcec�汾��Ϣ
* ���������ptDev Ӳ�������豸
* ����������汾��
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsCecGetVersion(const struct hdmi_cec_device *ptDev, HS_S32 *s32Version)
{
    SLOGE("mscec_get_version \n");
    T_HisenseHdmiCecDevice *ptPriv = (T_HisenseHdmiCecDevice *) ptDev;
    if (NULL == ptPriv || NULL == s32Version)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    // CEC 1.4
    *s32Version = 5;
    return;
}

/**********************************************************************
* ��������: HsCecGetVendorId
* ������������ȡvendor id��Ϣ
* ���������ptDevӲ�������豸
* ���������vendor id
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                  ����
***********************************************************************/

static void HsCecGetVendorId(const struct hdmi_cec_device *ptDev, HS_U32* u32VendorId)
{
    SLOGE("mscec_get_vendor_id \n");
    T_HisenseHdmiCecDevice *ptPriv = (T_HisenseHdmiCecDevice *) ptDev;
    if (NULL == ptPriv || NULL == u32VendorId)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    *u32VendorId = 0x4571;
    return;
}

/**********************************************************************
* ��������: HsCecGetPortInfo
* ������������ȡport����Ϣ
* ���������Ӳ�������豸
* ���������port  ����Ϣ
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static void HsCecGetPortInfo(const struct hdmi_cec_device *ptDev,
            struct hdmi_port_info* paList[], HS_S32 *s32total)
{
    T_HisenseHdmiCecDevice* ptDevice = (T_HisenseHdmiCecDevice *) ptDev;
    SLOGE("mscec_get_port_info enter \n");
    if (NULL == ptDevice)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    *paList = &ptDevice->atPortInfo[0];
    *s32total = CEC_PORT_NUM_MAX;
    SLOGE("mscec_get_port_info exit \n");
    return;
}

/**********************************************************************
* ��������: HsCecSetOption
* ����������ѡ������
* ���������flag ��Ҫ���� ��ѡ�� value ��Ҫ���� ��ֵ
* ���������Ӳ�������豸
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static void HsCecSetOption(const struct hdmi_cec_device *ptDev, HS_S32 s32Flag, HS_S32 s32Value)
{
    T_HisenseHdmiCecDevice *ptDevice = (T_HisenseHdmiCecDevice *) ptDev;

    if (NULL == ptDevice)
    {
        SLOGE("invalid parameter \n");
        return;
    }

    SLOGE("mscec_set_option flag[%d], value[%d] \n", s32Flag, s32Value);

    switch (s32Flag)
    {
        case HDMI_OPTION_WAKEUP:
        {
            ptDevice->s32HdmiOptionWakeup = s32Value;
            break;
        }
        case HDMI_OPTION_ENABLE_CEC:
        {
            ptDevice->s32HdmiOptionEnableCec = s32Value;
            break;
        }
        case HDMI_OPTION_SYSTEM_CEC_CONTROL:
        {
            ptDevice->s32HdmiOptionSystemCecControl = s32Value;
            break;
        }
        default:
        {
            SLOGE("mscec_set_option Failed. Unsupport flag[%d] \n", s32Flag);
            break;
        }
    }
    return;
}

/**********************************************************************
* ��������: HsCecSetAudioReturnChannel
* ����������ʹ��ARCӲ��ͨ��
* ���������Ӳ�������豸PortId port�� flag�򿪻�ر�
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                  ����
***********************************************************************/

static void HsCecSetAudioReturnChannel(const struct hdmi_cec_device* ptDev, HS_S32 s32PortId, HS_S32 s32Flag)
{
    T_HisenseHdmiCecDevice* ptDevice = (T_HisenseHdmiCecDevice *) ptDev;
    SLOGE("mscec_set_audio_return_channe. port_id[%d], flag[%d] \n", s32PortId, s32Flag);

    if (s32PortId < 1 || s32PortId > CEC_PORT_NUM_MAX)
    {
        SLOGE("port_id is invalid \n");
        return;
    }

    if (HS_HAL_ERR_OK != HS_HAL_CEC_SetArcControl(ptDevice->u32CecHandle, (s32Flag ? HS_TRUE : HS_FALSE)))
    {
        SLOGE("HS_HAL_CEC_SetArcControl Failed \n");
        return;
    }

    return;
}

/**********************************************************************
* ��������: HsCecIsConnected
* ����������CEC�豸�Ƿ�����
* ���������Ӳ�������豸PortId port��
* �����������
* �� �� ֵ��1 ���� 0 δ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                           chenweilan                          ����
***********************************************************************/

static HS_S32 HsCecIsConnected(const struct hdmi_cec_device* ptDev, HS_S32 s32PortId)
{
    T_HisenseHdmiCecDevice* ptDevice = (T_HisenseHdmiCecDevice *) ptDev;
    SLOGE("mscec_is_connected get connect status of port[%d] \n", s32PortId);

    if (s32PortId < 1 || s32PortId > CEC_PORT_NUM_MAX)
    {
        SLOGE("s32PortId is invalid \n");
        return HDMI_NOT_CONNECTED;
    }

    if (E_HS_GPIO_LEVEL_ON == ptDevice->atHdmiPort[s32PortId - 1].eLevel)
    {
        SLOGE("port is connect \n");
        return HDMI_CONNECTED;
    }

    SLOGE("port is disconnect \n");
    return HDMI_NOT_CONNECTED;
}


/**********************************************************************
* ��������: MsCecOpen
* ��������������open
* ���������pModule : Ӳ������ģ�� name:ģ������ppDevice:Ӳ�������豸
* �����������
* �� �� ֵ�����״̬0:�ɹ�   ����:ʧ��
* ����˵���������Բ�ͬ��Ӳ������������
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                   chenweilan                   ����
***********************************************************************/

static HS_S32 MsCecOpen(const struct hw_module_t *pModule,
        const char *pName, struct hw_device_t **ppDevice)
{
    HS_S32 s32Status = -EINVAL;

    char tvSeperate[PROPERTY_VALUE_MAX];
    SLOGE("mscec_open enter \n");

    if (!strcmp(pName, HDMI_CEC_HARDWARE_INTERFACE))
    {
        T_HisenseHdmiCecDevice *ptDev = (T_HisenseHdmiCecDevice*) malloc(sizeof(*ptDev));
        if (ptDev == NULL)
        {
            return s32Status ;
        }
        /* initialize our state here */
        memset(ptDev, 0, sizeof(*ptDev));

        /* initialize the procs */
        ptDev->tDevice.common.tag               = HARDWARE_DEVICE_TAG;
        ptDev->tDevice.common.version           = HDMI_CEC_DEVICE_API_VERSION_1_0;
        ptDev->tDevice.common.module            = const_cast<hw_module_t*>(pModule);
        ptDev->tDevice.common.close             = HsCecClose;
        ptDev->tDevice.add_logical_address      = HsCecAddLogicalAddress;
        ptDev->tDevice.clear_logical_address    = HsCecClearLogicalAddress;
        ptDev->tDevice.get_physical_address     = HsCecGetPhysicalAddress;
        ptDev->tDevice.send_message             = HsCecSendMessage;
        ptDev->tDevice.register_event_callback  = HsCecRegisterEventCallback;
        ptDev->tDevice.get_version              = HsCecGetVersion;
        ptDev->tDevice.get_vendor_id            = HsCecGetVendorId;
        ptDev->tDevice.get_port_info            = HsCecGetPortInfo;
        ptDev->tDevice.set_option               = HsCecSetOption;
        ptDev->tDevice.set_audio_return_channel = HsCecSetAudioReturnChannel;
        ptDev->tDevice.is_connected             = HsCecIsConnected;
        ptDev->tDevice.request_short_audio      = HsCecRequestShortAudioDescripe;
        ptDev->tDevice.request_reset_sad        = HsCecRequestResetShortAudioDescripe;
        /* Init PortInfo */
        property_get("ro.hisense.tv.seperate", tvSeperate, "0");
        if (strcmp(tvSeperate, "1") == 0)
        {
            ptDev->atPortInfo[0].port_id          = 1;
            ptDev->atPortInfo[0].type             = HDMI_INPUT;
            ptDev->atPortInfo[0].cec_supported    = 1;
            ptDev->atPortInfo[0].arc_supported    = 1;
            ptDev->atPortInfo[0].physical_address = 0x1000;

            ptDev->atPortInfo[1].port_id          = 2;
            ptDev->atPortInfo[1].type             = HDMI_INPUT;
            ptDev->atPortInfo[1].cec_supported    = 1;
            ptDev->atPortInfo[1].arc_supported    = 0;
            ptDev->atPortInfo[1].physical_address = 0x2000;
        }
        else
        {
            ptDev->atPortInfo[0].port_id          = 1;
            ptDev->atPortInfo[0].type             = HDMI_INPUT;
            ptDev->atPortInfo[0].cec_supported    = 1;
            ptDev->atPortInfo[0].arc_supported    = 0;
            ptDev->atPortInfo[0].physical_address = 0x1000;

            ptDev->atPortInfo[1].port_id          = 2;
            ptDev->atPortInfo[1].type             = HDMI_INPUT;
            ptDev->atPortInfo[1].cec_supported    = 1;
            ptDev->atPortInfo[1].arc_supported    = 1;
            ptDev->atPortInfo[1].physical_address = 0x2000;
        }
        *ppDevice  = &ptDev->tDevice.common;
        s32Status  = HsCecInstallEnv(ptDev);
        s32Status |= HsCecStart(ptDev);
    }
    SLOGE("mscec_open exit \n");
    return s32Status;
}

/**********************************************************************
* ������������ ��open ����
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                    chenweilan                   ����
***********************************************************************/

static struct hw_module_methods_t tHdmiCecModuleMethods = {
    .open = MsCecOpen,
};

/**********************************************************************
* ��������������Ӳ��ģ������
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����                      �޸���                      �޸�����
* 2016.7.12                     chenweilan                   ����
***********************************************************************/

struct hdmi_cec_module HAL_MODULE_INFO_SYM =
{
    .common =
    {
        .tag           = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id            = HDMI_CEC_HARDWARE_MODULE_ID,
        .name          = "Hisense HDMI CEC HW HAL",
        .author        = "The Android Open Source Project",
        .methods       = &tHdmiCecModuleMethods,
    },
};

