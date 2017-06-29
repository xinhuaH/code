
//******************************************************************************
//<MStar Software>

#ifndef _HS_CEC_DATATYPE_H_
#define _HS_CEC_DATATYPE_H_
#include <hs_type.h>

#pragma pack(push)
#pragma pack(4)

#define INIT_ST(st) do { \
    memset(&st, 0, sizeof(st));\
    st.u16SizeofStructure = sizeof(st);\
}while(0)

//-------------------------------------------------------------------------------------------------
//  Defines
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HS_GPIO_LEVEL_ON = 0,  //LEVEL on is equal to level high if not invert, but equal to level low if invert
    E_HS_GPIO_LEVEL_OFF,     //LEVEL off iis equal to level low if not invert, but equal to level high if invert
} E_HS_GPIO_Level;

typedef enum
{
    E_HS_CEC_LA_TV              = 0,     //CEC device TV
    E_HS_CEC_LA_RECORDER1       = 1,     //CEC device RECORDER1
    E_HS_CEC_LA_RECORDER2       = 2,     //CEC device RECORDER2
    E_HS_CEC_LA_TUNER1          = 3,     //CEC device TUNER1
    E_HS_CEC_LA_PLAYBACK1       = 4,     //CEC device PLAYBACK1
    E_HS_CEC_LA_AUDIO_SYS       = 5,     //CEC device AUDIO SYSTEM
    E_HS_CEC_LA_TUNER2          = 6,     //CEC device TUNER2
    E_HS_CEC_LA_TUNER3          = 7,     //CEC device TUNER3
    E_HS_CEC_LA_PLAYBACK2       = 8,     //CEC device PLAYBACK2
    E_HS_CEC_LA_RECORER3        = 9,     //CEC device RECORDER3
    E_HS_CEC_LA_TUNER4          = 10,    //CEC device TUNER4
    E_HS_CEC_LA_PLAYBACK3       = 11,    //CEC device PLAYBACK3
    E_HS_CEC_LA_RESERVED1       = 12,    //Reserved use for CEC device
    E_HS_CEC_LA_RESERVED2       = 13,    //Reserved use for CEC device
    E_HS_CEC_LA_FREE_USE        = 14,    //Free use for CEC device
    E_HS_CEC_LA_UNREGISTERED    = 15,    //Unregistered
    E_HS_CEC_LA_BROADCAST       = 15,    //BROADCAST to all CEC devices
    E_HS_CEC_LA_MAX = 15,                //MAX Logical Addr num
}E_HS_CEC_DeviceLA;
typedef enum
{
    E_HS_AOUT_PATH_NONE = 0xFF,

    E_HS_AOUT_PATH_LR = 0,
    E_HS_AOUT_PATH_I2S,
    E_HS_AOUT_PATH_SPDIF,
    E_HS_AOUT_PATH_HDMI,

    E_HS_AOUT_PATH_MAIN_SPEAKER,
    E_HS_AOUT_PATH_HP,
    E_HS_AOUT_PATH_LINEOUT,
    E_HS_AOUT_PATH_SIFOUT,
    E_HS_AOUT_PATH_SCART1,
    E_HS_AOUT_PATH_SCART2,
    E_HS_AOUT_PATH_HDMI_ARC,

    E_HS_AOUT_PATH_ALL ,
    E_HS_AOUT_PATH_NUM,

}E_HsAoutPath;

typedef enum
{
    //----- One Touch Play ----------------------------
    E_HS_CEC_OP_ACTIVE_SOURCE                         = 0x82,
    E_HS_CEC_OP_OTP_IMAGE_VIEW_ON                     = 0x04,
    E_HS_CEC_OP_OTP_TEXT_VIEW_ON                      = 0x0D,


//----- Routing Control ---------------------------

    E_HS_CEC_OP_RC_INACTIVE_SOURCE                    = 0x9D,

    E_HS_CEC_OP_RC_REQ_ACTIVE_SOURCE                  = 0x85,


    E_HS_CEC_OP_RC_ROUTING_CHANGE                     = 0x80,

    E_HS_CEC_OP_RC_ROUTING_INFO                       = 0x81,


    E_HS_CEC_OP_RC_SET_STREM_PATH                     = 0x86,


//----- Standby Command ---------------------------
    E_HS_CEC_OP_STANDBY                               = 0x36,


//----- One Touch Record---------------------------
    E_HS_CEC_OP_OTR_RECORD_ON                         = 0x09,
    E_HS_CEC_OP_OTR_RECORD_OFF                        = 0x0B,
    E_HS_CEC_OP_OTR_RECORD_STATUS                     = 0x0A,
    E_HS_CEC_OP_OTR_RECORD_TV_SCREEN                  = 0x0F,


//----- Timer programmer -------------------------- CEC1.3a
    E_HS_CEC_OP_TP_CLEAR_ANALOG_TIMER                 = 0x33,


    E_HS_CEC_OP_TP_CLEAR_DIGITAL_TIMER                = 0x99,

    E_HS_CEC_OP_TP_CLEAR_EXT_TIMER                    = 0xA1,


    E_HS_CEC_OP_TP_SET_ANALOG_TIMER                   = 0x34,


    E_HS_CEC_OP_TP_SET_DIGITAL_TIMER                  = 0x97,

    E_HS_CEC_OP_TP_SET_EXT_TIMER                      = 0xA2,


    E_HS_CEC_OP_TP_SET_TIMER_PROGRAM_TITLE            = 0x67,
    E_HS_CEC_OP_TP_TIMER_CLEARD_STATUS                = 0x43,
    E_HS_CEC_OP_TP_TIMER_STATUS                       = 0x35,


//----- System Information ------------------------
    E_HS_CEC_OP_SI_CEC_VERSION                        = 0x9E,       //1.3a
    E_HS_CEC_OP_SI_GET_CEC_VERSION                    = 0x9F,       //1.3a

    E_HS_CEC_OP_SI_REQUEST_PHY_ADDR                   = 0x83,
    E_HS_CEC_OP_SI_REPORT_PHY_ADDR                    = 0x84,

    E_HS_CEC_OP_SI_GET_MENU_LANGUAGE                  = 0x91,




    E_HS_CEC_OP_SI_SET_MENU_LANGUAGE                  = 0x32,



//----- Deck Control Feature-----------------------
    E_HS_CEC_OP_DC_DECK_CTRL                          = 0x42,


    E_HS_CEC_OP_DC_DECK_STATUS                        = 0x1B,
    E_HS_CEC_OP_DC_GIVE_DECK_STATUS                   = 0x1A,
    E_HS_CEC_OP_DC_PLAY                               = 0x41,


//----- Tuner Control ------------------------------
    E_HS_CEC_OP_TC_GIVE_TUNER_STATUS                  = 0x08,


    E_HS_CEC_OP_TC_SEL_ANALOG_SERVICE                 = 0x92,


    E_HS_CEC_OP_TC_SEL_DIGITAL_SERVICE                = 0x93,


    E_HS_CEC_OP_TC_TUNER_DEVICE_STATUS                = 0x07,

    E_HS_CEC_OP_TC_TUNER_STEP_DEC                     = 0x06,


    E_HS_CEC_OP_TC_TUNER_STEP_INC                     = 0x05,



//---------Vendor Specific -------------------------
    //E_HS_CEC_OP_VS_CEC_VERSION                        = 0x9E,       //1.3a
    E_HS_CEC_OP_VS_DEVICE_VENDOR_ID                   = 0x87,
    //E_HS_CEC_OP_VS_GET_CEC_VERSION                    = 0x9F,       //1.3a

    E_HS_CEC_OP_VS_GIVE_VENDOR_ID                     = 0x8C,


    E_HS_CEC_OP_VS_VENDOR_COMMAND                     = 0x89,
    E_HS_CEC_OP_VS_VENDOR_COMMAND_WITH_ID             = 0xA0,      //1.3a

    E_HS_CEC_OP_VS_VENDOR_RC_BUT_DOWN                 = 0x8A,

    E_HS_CEC_OP_VS_VENDOR_RC_BUT_UP                 = 0x8B,
    //----- OSD Display --------------------------------
    E_HS_CEC_OP_SET_OSD_STRING                      = 0x64,
    //----- Device OSD Name Transfer  -------------------------
    E_HS_CEC_OP_OSDNT_GIVE_OSD_NAME                 = 0x46,
    E_HS_CEC_OP_OSDNT_SET_OSD_NAME                  = 0x47,
    //----- Device Menu Control ------------------------
    E_HS_CEC_OP_DMC_MENU_REQUEST                        = 0x8D,
    E_HS_CEC_OP_DMC_MENU_STATUS                     = 0x8E,
    //E_HS_CEC_OP_DMC_MENU_ACTIVATED                      = 0x00,   //parameter
    //E_HS_CEC_OP_DMC_MENU_DEACTIVATED                = 0x01,   //parameter
    E_HS_CEC_OP_UI_PRESS                                = 0x44,
    E_HS_CEC_OP_UI_RELEASE                          = 0x45,
    //----- Power Status  ------------------------------
    E_HS_CEC_OP_PS_GIVE_POWER_STATUS                    = 0x8F,
    E_HS_CEC_OP_PS_REPORT_POWER_STATUS              = 0x90,
    //----- General Protocal Message ------------------
    //----- Abort Message -----------------------------
    E_HS_CEC_OP_ABORT_MESSAGE                       = 0xFF,

    //----- Feature Abort -----------------------------
    E_HS_CEC_OP_FEATURE_ABORT                       = 0x00,
    //----- System Audio Control ------------------
    E_HS_CEC_OP_SAC_GIVE_AUDIO_STATUS               = 0x71,
    E_HS_CEC_OP_SAC_GIVE_SYSTEM_AUDIO_MODE_STATUS   = 0x7D,
    E_HS_CEC_OP_SAC_REPORT_AUDIO_STATUS             = 0x7A,

    E_HS_CEC_OP_SAC_SET_SYSTEM_AUDIO_MODE           = 0x72,
    E_HS_CEC_OP_SAC_SYSTEM_AUDIO_MODE_REQUEST       = 0x70,
    E_HS_CEC_OP_SAC_SYSTEM_AUDIO_MODE_STATUS        = 0x7E,
    E_HS_CEC_OP_SAC_REPORT_SHORT_AUDIO_DESCRIPTOR   = 0xA3,
    E_HS_CEC_OP_SAC_REQUEST_SHORT_AUDIO_DESCRIPTOR  = 0xA4,
    //----- System Audio Control ------------------
    E_HS_CEC_OP_SAC_SET_AUDIO_RATE                  = 0x9A,
    //----- Audio Return Channel  Control ------------------
    E_HS_CEC_OP_ARC_INITIATE_ARC                    = 0xC0,
    E_HS_CEC_OP_ARC_REPORT_ARC_INITIATED            = 0xC1,
    E_HS_CEC_OP_ARC_REPORT_ARC_TERMINATED           = 0xC2,
    E_HS_CEC_OP_ARC_REQUEST_ARC_INITIATION          = 0xC3,
    E_HS_CEC_OP_ARC_REQUEST_ARC_TERMINATION         = 0xC4,
    E_HS_CEC_OP_ARC_TERMINATED_ARC                  = 0xC5,
} E_HS_CEC_OpCode;


typedef enum
{
    ///BROADCAST MESSAGE TYPE
    E_HS_CEC_TRANS_BROADCAST_MSG   = 0,
    ///DIRECT MESSAGE TYPE
    E_HS_CEC_TRANS_DIRECT_MSG      = 1,
    ///BROADCAST AND DIRECT MESSAGE TYPE
    E_HS_CEC_TRANS_BOTHTYPE_MSG    = 2,
} HS_CEC_MsgTransType_e;

typedef enum
{
    E_HS_CEC_FEATURE_ABORT = 0x00,  //CEC ERROR CODE FEATURE ABORT
    E_HS_CEC_RX_SUCCESS    = 0x01,  //CEC ERROR CODE Receive Success
    E_HS_CEC_TX_SUCCESS    = 0x02,  //CEC ERROR CODE Transfer Success
    E_HS_CEC_RF            = 0x04,  //CEC ERROR CODE RF
    E_HS_CEC_LOST_ABT      = 0x08,  //CEC ERROR CODE Lost ABT
    E_HS_CEC_BIT_SHORT     = 0x10,  //CEC ERROR CODE Bit short
    E_HS_CEC_BIT_LONG      = 0x20,  //CEC ERROR CODE Bit long
    E_HS_CEC_NACK          = 0x40,  //CEC ERROR CODE none-Ack
    E_HS_CEC_SYSTEM_BUSY   = 0x80,  //CEC ERROR CODE System busy
}E_HS_CEC_ErrorCode;
typedef enum{
    ///default
    HS_EXTIN_EDID_DeviceType_DEFAULT,
    ///HDMI
    HS_EXTIN_EDID_DeviceType_HDMI,

}HS_EXTIN_EDID_DeviceType_e;
typedef enum
{
    E_HS_AOUT_MUTE_BY_NONE = 0xFF,

    E_HS_AOUT_MUTE_BY_USER = 0x0,
    E_HS_AOUT_MUTE_BY_DECODER,
    E_HS_AOUT_MUTE_BY_INTERVAL,
    E_HS_AOUT_MUTE_BY_INIT,
    E_HS_AOUT_MUTE_BY_DEINIT,

    E_HS_AOUT_MUTE_NUM,
}MI_AOUT_MuteType_e;

typedef struct HS_CEC_MsgInfo_s
{
    /// CEC message header
    E_HS_CEC_DeviceLA enHeader;
    /// CEC message opcode
    E_HS_CEC_OpCode enOpcode;
    /// CEC message parameter
    HS_U8 szPara[16];
    /// CEC message length
    HS_U8 u8MsgLen;//len = header len + opcode len + parameter len
}T_HS_CEC_MsgInfo;

typedef enum
{
    E_HS_AOUT_PARAMTYPE_NONE = 0xFF,

    E_HS_AOUT_PARAMTYPE_SPDIF_HWCONFIG = 0,            // spdif Hw config , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_AD_VOLUME,                     // AD volume , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_AD_MUTE,                       // AD mute , parameter type is a pointer to MI_U32

    E_HS_AOUT_PARAMTYPE_AMPLIFIER1_MUTE,               // amplifier1 mute  , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_AMPLIFIER2_MUTE,               // amplifier2 mute , parameter type is a pointer to MI_U32

    E_HS_AOUT_PARAMTYPE_SET_PCM_CAPTURE1_SOURCE,       // capture1 source , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_SET_PCM_CAPTURE2_SOURCE,       // capture1 source , parameter type is a pointer to MI_U32

    E_HS_AOUT_PARAMTYPE_KTV_MICIN_MUTE,                // KTV source mic input path mute , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_MP3IN_MUTE,                // KTV source MP3 input path mute, parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_PCMIN_MUTE,                // KTV source pcm input path mute , parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_MICIN_VOLUME,              // KTV source mic input path volume, parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_MP3IN_VOLUME,              // KTV source MP3 input path volume, parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_PCMIN_VOLUME,              // KTV source pcm input path volume, parameter type is a pointer to MI_U32
    E_HS_AOUT_PARAMTYPE_KTV_MICIN_VOLUME_TBL,          // KTV source mic input path volume table , parameter type is a pointer to array of MI_16*101
    E_HS_AOUT_PARAMTYPE_KTV_MP3IN_VOLUME_TBL,          // KTV source MP3 input path volume table , parameter type is a pointer to array of MI_16*101
    E_HS_AOUT_PARAMTYPE_KTV_PCMIN_VOLUME_TBL,          // KTV source pcm input path volume table , parameter type is a pointer to array of MI_16*101

    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_PRESCALE,    //sound effect of prescale, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_EQ,          //sound effect of eq, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_PEQ,         //sound effect of peq, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_TREBLE,      //sound effect of treble, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_BASS,        //sound effect of bass parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_AVC,         //sound effect of avc, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_SURROUND,    //sound effect of surround, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_BALANCE,     //sound effect of balance, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_SOUNDMODE,   //sound effect of sound mode, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_DRC,         //sound effect of drc, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_ECHO,        //sound effect of echo, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_NR,          //sound effect of nr, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_HPF,         //sound effect of hpf, parameter type is a pointer to array of MI_AOUT_BSE_Param_t
    E_HS_AOUT_PARAMTYPE_BASIC_SOUNDEFFECT_MAX,

    E_HS_AOUT_PARAMTYPE_SPDIF_SCMS,                    //spdif scms, parameter type is a pointer to array of MI_AOUT_SPDIF_SCMS_t
    E_HS_AOUT_PARAMTYPE_CAPTURE1_VOLUME,
    E_HS_AOUT_PARAMTYPE_CAPTURE2_VOLUME,
    E_HS_AOUT_PARAMTYPE_CAPTURE1_MUTE,
    E_HS_AOUT_PARAMTYPE_CAPTURE2_MUTE,
    E_HS_AOUT_PARAMTYPE_HDMI_TX_STATUS,
    E_HS_AOUT_PARAMTYPE_BYPASS_SOUNDEFFECT,
    E_HS_AOUT_PARAMTYPE_SET_HDMI_ARC_CAPABILITY,
    E_HS_AOUT_PARAMTYPE_SET_VOLUME_FADINGTIME,
    E_HS_AOUT_PARAMTYPE_NUM,
}HS_AOUT_AttrType_e;

typedef struct HS_CEC_InitParams_s
{
    HS_U16 u16SizeofStructure;
    HS_U32 u32XtalClk;  //Chip Clock
}HS_CEC_InitParams_t;   //CEC module Init Param
typedef struct HS_AOUT_VolCfg_s
{
    HS_BOOL bUseCusVolTbl;
    HS_U16 *pu16VolTbl;
}HS_AOUT_VolCfg_t;
typedef enum
{
    E_HS_AOUT_MAIN,                //< output connect to main source
    E_HS_AOUT_SUB,                //< output connect to sub source
    E_HS_AOUT_NULL,                //< Disable output port
}HS_AOUT_Output_SourceType_e;
typedef enum
{
    E_HS_SIGNAL_STATUS_LOCK,
    E_HS_SIGNAL_STATUS_UNLOCK,
    E_HS_SIGNAL_STATUS_NOTSUPPORT,

} E_HS_EXTIN_SIGNAL_STATUS;
//====== HDMI Tx Output TYPE========
typedef enum
{
    E_HS_AOUT_HDMI_TX_OUT_PCM,                       ///< HDMI OUTPUT PCM
    E_HS_AOUT_HDMI_TX_OUT_NONPCM,                    ///< HDMI OUTPUT non-PCM
    E_HS_AOUT_HDMI_TX_OUT_8CH_NONPCM,                ///< HDMI OUTPUT non-PCM with 8 Channel output
} HS_AOUT_HdmiTxOutputType_e;
typedef enum
{
    HS_HAL_ERR_OK = 0x0,                   ///< succeeded
    HS_HAL_ERR_CONTINUE = 0x1,                 ///< not error but hasn' be succeeded. Flow is continue...
    HS_HAL_ERR_INITED = 0x2,               ///< not error but init has be called again

    HS_HAL_ERR_FAILED = 0x3,               ///< general failed
    HS_HAL_ERR_NOT_INITED = 0x4,           ///< moudle hasn't be inited
    HS_HAL_ERR_NOT_SUPPORT = 0x5,          ///< not supported
    HS_HAL_ERR_NOT_IMPLEMENT = 0x6,        ///< not implemented
    HS_HAL_ERR_INVALID_HANDLE = 0x7,       ///< invalid handle
    HS_HAL_ERR_INVALID_PARAMETER = 0x8,    ///< invalid parameter
    HS_HAL_ERR_RESOURCES = 0x9,            ///< system resource issue
    HS_HAL_ERR_MEMORY_ALLOCATE = 0xa,      ///< memory allocation
    HS_HAL_ERR_CHAOS = 0xb,                ///< chaos state-mechine
    HS_HAL_ERR_DATA_ERROR = 0xc,           ///< data error
    HS_HAL_ERR_TIMEOUT = 0xd,              ///< timeout
    HS_HAL_ERR_LIMITION = 0xe,             ///< limitation
    HS_HAL_ERR_BUSY = 0xf,                    ///< the required operation can not be done due to current system is busy.

    HS_HAL_ERR_MAX,                  ///< maximum value of D_RESULT
}HS_HAL_RESULT;

//======Codec TYPE========
typedef enum
{
    E_HS_AOUT_CODEC_NONE                          = 0x0,
    E_HS_AOUT_CODEC_LPCM                          = 0x1,      ///< Support LPCM
    E_HS_AOUT_CODEC_DD                            = 0x2,      ///< Support DD
    E_HS_AOUT_CODEC_MPEG1                         = 0x3,      ///< Support MPEG1
    E_HS_AOUT_CODEC_MP3                           = 0x4,      ///< Support MP3
    E_HS_AOUT_CODEC_MPEG2                         = 0x5,      ///< Support MPEG2
    E_HS_AOUT_CODEC_AAC                           = 0x6,      ///< Support AAC
    E_HS_AOUT_CODEC_DTS                           = 0x7,      ///< Support DTS
    E_HS_AOUT_CODEC_ATRAC                         = 0x8,      ///< Support ATRAC
    E_HS_AOUT_CODEC_ONEBITAUDIO                   = 0x9,      ///< Support One-Bit Audio
    E_HS_AOUT_CODEC_DDP                           = 0xA,      ///< Support DDP
    E_HS_AOUT_CODEC_DTSHD                         = 0xB,      ///< Support DTSHD
    E_HS_AOUT_CODEC_TRUEHD                        = 0xC,      ///< Support MLP/TRUE-HD
    E_HS_AOUT_CODEC_DST                           = 0xD,      ///< Support DST
    E_HS_AOUT_CODEC_WMAPRO                        = 0xE,      ///< Support WMA-Pro
    E_HS_AOUT_CODEC_MAX                           = 0xF,
} E_HS_AOUT_CodecType;


typedef struct HS_EXTIN_EdidData_s
{
    // the pointer to the block buffer
    HS_U8 * pu8Info;
   // the length of the block buffer
    HS_U32 u32InfoLen;
} HS_EXTIN_Edid_Data_t;

typedef enum
{
    HS_EXTIN_GetAttr_PhaseRange = 0,
    HS_EXTIN_GetAttr_GainRange,
    HS_EXTIN_GetAttr_OffsetRange,
    HS_EXTIN_GetAttr_PcMinClock,
    HS_EXTIN_GetAttr_PcMaxClock,
    HS_EXTIN_GetAttr_PcModeIndex,
    HS_EXTIN_GetAttr_VD_VTOTAL,
}HS_EXTIN_GetAttr_e;
typedef enum
{
    HS_EXTIN_SetAttr_VBIInitConfig,
    //update audio block in edid, parameter type is a pointer to MI_EXTIN_Edid_Data_t.Especialy,the extin handle can be MI_HANDLE_NULL.
    HS_EXTIN_SetAttr_UpdateAudioEdid,
    //update audio block in edid, parameter type is a pointer to MI_EXTIN_Edid_Data_t.Especialy,the extin handle can be MI_HANDLE_NULL.
    HS_EXTIN_SetAttr_RestoreAudioEdid,
}HS_EXTIN_SetAttr_e;
typedef struct HS_EXTIN_GetAttr_s
{
    HS_U16 u16SizeofStructure;
    HS_U16 u16PhaseRange;
    HS_U32 u32G_Y_GainRange;
    HS_U32 u32G_Y_OffsetRange;
    HS_U16 u16PcMinClk;
    HS_U16 u16PcMaxClk;
    HS_U8 u8ModeIndex;
    HS_U16 u16G_V_Vtotal;
}HS_EXTIN_GetAttr_t;

//========HDMI ARC Capability==============
typedef struct HS_AOUT_HdmiArcCapability_s
{
    HS_BOOL   bSupport;                   // True is support ,False is not support
    HS_U8   u8SupportChannel;                      // Short_Audio_Descriptor Byte1 [0:2]
    HS_U8   u8SupportSampleRate;              // Short_Audio_Descriptor Byte2 [0:6]
    HS_U8   u8SupportSadByte3;               // Short_Audio_Descriptor Byte3 [0:7]
} HS_AOUT_HdmiArcCapability_t;
typedef enum{
    ///default
    HS_EXTIN_HDCP_DEFAULT,
    ///HDCP1.4
    HS_EXTIN_HDCP_V1,
    ///HDCP2.0
    HS_EXTIN_HDCP_V2
}HS_EXTIN_HDCP_Version_e;

//=======HDMI ARC Capability Params=========

typedef enum
{
    E_HS_EXTIN_COMPONENT,
    E_HS_EXTIN_VGA,
    E_HS_EXTIN_HDMI,   //HDMI source can be separate by HDMI, DVI , MHL type.
    E_HS_EXTIN_CVBS,
    E_HS_EXTIN_SVIDEO,
    E_HS_EXTIN_SCART,
    E_HS_EXTIN_NUM
} HS_EXTIN_INPUTSOURCE_TYPE_e;
typedef enum{
    ///default
    HS_EXTIN_EDID_DEFAULT,
    ///EDID1.4
    HS_EXTIN_EDID_1_4,
    ///EDID2.0
    HS_EXTIN_EDID_2_0
}HS_EXTIN_EDID_Version_e;
typedef struct HS_AOUT_HdmiArcCapabilityParams_s
{
      E_HsAoutPath eAoutPath;
      E_HS_AOUT_CodecType eCodecType;
      HS_AOUT_HdmiArcCapability_t stHdmiArcCapability;
}HS_AOUT_HdmiArcCapabilityParams_t;

typedef struct MI_EXTIN_EDID_Info_s
{
    HS_U16 u16SizeofStructure;
    HS_EXTIN_EDID_DeviceType_e eDeviceType;
    HS_EXTIN_EDID_Version_e eEDIDVersion;
    HS_U32 u32Port;
} HS_EXTIN_EDID_Info_t;

typedef struct HS_EXTIN_InitParams_s
{
    HS_U16 u16SizeofStructure;
    //load hdcp
    HS_BOOL bIsLoadHDCP;
    HS_EXTIN_HDCP_Version_e eHdcpVersion;

    //load edid
    HS_BOOL bIsLoadEDID;
    HS_EXTIN_EDID_Info_t stEDIDInfo;
 //need check

} HS_EXTIN_InitParams_t;

typedef struct HS_AOUT_Attr_s
{
    E_HsAoutPath eOpenPath;
    HS_AOUT_VolCfg_t stVolCfg;
    HS_AOUT_Output_SourceType_e eOutputSourceInfo;
}HS_AOUT_Attr_t;


typedef struct HS_GPIO_InitParam_s
{
    HS_U16 u16SizeofStructure;
    HS_U32 u32Reserved;
} HS_GPIO_InitParam_t;
typedef struct HS_AOUT_InitParams_s
{
    HS_U16 u16SizeofStructure;
    HS_U32 u32Reserve;
}HS_AOUT_InitParams_t;

typedef struct HS_CEC_OpenParams_s
{
    HS_U16 u16SizeofStructure;
    HS_U32 revert;
}T_HS_CEC_OpenParams;   //CEC module Open Param

#pragma pack(pop)
#endif //_HS_CEC_DATATYPE_H_
