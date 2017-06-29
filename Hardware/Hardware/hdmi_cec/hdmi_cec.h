
/*********************************************************************
* 版权所有 (C)2016, 青岛海信电器股份有限公司
*
* 文件名称: hdmi_cec.h
* 文件标识: 无
* 内容摘要: 给hdmi_cec.cpp提供数据
* 其它说明:
* 当前版本: V3.0
* 作    者: chenweilan
* 完成日期: 2016年7月12日
*
**********************************************************************/
#include <hardware/hdmi_cec.h>
#include "hs_hal_cec.h"
/**********************************************************************
*                             宏定义                                  *
**********************************************************************/
#define SAD_LEN                          3
#define SAD_LEN_MAX                      12
#define CEC_M_CLOCK                      12000000
#define SAD_MSG_LEN                      6
#define CEC_STACK_SIZE                   (4096)
#define CEC_PORT_NUM_MAX                 2
#define HS_GPIO_HDMI2_5V                 162   //hisense chenweilan add
#define AUDIO_SAMPLE_RATE                0x7F
#define AUDIO_FORMAT_CODE                0x78
#define AUDIO_SUPPORT_CHANNEL            0x07
#define AUDIO_SUPPORT_SADBYTE            0x01
#define SYSTEM_BUSY_RETRY_TIME           10
/// CEC BUS transition delay
#define CEC_TRANSITION_DELAY             33600



#define PTH_RET_CHK_CEC(_pf_) \
    ({ \
        int r = _pf_; \
        if ((r != 0) && (r != ETIMEDOUT)) \
            SLOGV("[PTHREAD] %s: %d: %s\n", __FILE__, __LINE__, strerror(r)); \
         r; \
    })


/**********************************************************************
*                            变量定义                                 *
**********************************************************************/
HS_BOOL g_bInitArcFinish     = HS_FALSE;
HS_BOOL g_bUpdateEdidFromSAD = HS_FALSE;

/// for <Device type>
/// reference from HDMI1.4b
typedef enum _HS_CEC_DEVICE_TYPE
{
    //DEVICE TYPE TV
    E_MAPI_DEVICE_TYPE_TV                    = 0,
    //DEVICE TYPE RECORDING_DEVICE
    E_MAPI_DEVICE_TYPE_RECORDING_DEVICE      = 1,
    //DEVICE TYPE RESERVED
    E_MAPI_DEVICE_TYPE_RESERVED              = 2,
    //DEVICE TYPE TUNER
    E_MAPI_DEVICE_TYPE_TUNER                 = 3,
    //DEVICE TYPE PLAYBACK_DEVICE
    E_MAPI_DEVICE_TYPE_PLAYBACK_DEVICE       = 4,
    //DEVICE TYPE AUDIO_SYSTEM
    E_MAPI_DEVICE_TYPE_AUDIO_SYSTEM          = 5,
} E_HS_CEC_DEVICE_TYPE;

/* for <RequestShortAudioDescriptor>, need send audio format as parameter
*reference from CEA-861-D
*/
typedef enum _HS_CEC_ARC_AUDIO_FORMAT
{
    E_HS_CEC_ARC_AUDIO_FORMAT_RESERVED  = 0x00,
    E_HS_CEC_ARC_AUDIO_FORMAT_LPCM      = 0x01,
    E_HS_CEC_ARC_AUDIO_FORMAT_AC3       = 0x02,
    E_HS_CEC_ARC_AUDIO_FORMAT_MPEG1     = 0x03,
    E_HS_CEC_ARC_AUDIO_FORMAT_MP3       = 0x04,
    E_HS_CEC_ARC_AUDIO_FORMAT_MPEG2     = 0x05,
    E_HS_CEC_ARC_AUDIO_FORMAT_AAC       = 0x06,
    E_HS_CEC_ARC_AUDIO_FORMAT_DTS       = 0x07,
    E_HS_CEC_ARC_AUDIO_FORMAT_ATRAC     = 0x08,
    E_HS_CEC_ARC_AUDIO_FORMAT_DSD       = 0x09,
    E_HS_CEC_ARC_AUDIO_FORMAT_EAC3      = 0x0A,
    E_HS_CEC_ARC_AUDIO_FORMAT_DTSHD     = 0x0B,
    E_HS_CEC_ARC_AUDIO_FORMAT_MAT       = 0x0C,
    E_HS_CEC_ARC_AUDIO_FORMAT_DST       = 0x0D,
    E_HS_CEC_ARC_AUDIO_FORMAT_WMA       = 0x0E,
    E_HS_CEC_ARC_AUDIO_FORMAT_EXTENSION = 0x0F,
}E_HS_CEC_ARC_AUDIO_FORMAT;

typedef enum
{
    LOGICAL_ADDR_UNSET = 0,
    LOGICAL_ADDR_SET = 1,
}E_LOGICAL_ADDR_SET;

typedef struct hisense_hdmi_port
{
  //  HS_HANDLE exthandle;
    E_HS_EXTIN_SIGNAL_STATUS eSignal;
    E_HS_GPIO_Level eLevel;
}T_HisenseHdmiPort;

typedef struct hisense_hdmi_cec_device
{
    hdmi_cec_device_t tDevice;
    HS_HANDLE u32CecHandle;
    HS_HANDLE u32AoutHdmiHandle;
    pthread_t cec_tid;
    HS_BOOL bThreadexit;
    HS_U8 u8MyPhysicalAddress[2];
    HS_S32 s32HdmiOptionWakeup;
    HS_S32 s32HdmiOptionEnableCec;
    HS_S32 s32HdmiOptionSystemCecControl;
    event_callback_t CallBack;
    void *CallbackParam;
    E_HS_CEC_DeviceLA eMyLogicalAddress;
    E_HS_CEC_DEVICE_TYPE eMyDeviceType;
    E_LOGICAL_ADDR_SET eCecDeviceSupported[E_HS_CEC_LA_MAX];
    T_HisenseHdmiPort atHdmiPort[CEC_PORT_NUM_MAX];
    struct hdmi_port_info atPortInfo[CEC_PORT_NUM_MAX];
}T_HisenseHdmiCecDevice;

typedef struct
{
    cec_message_type eCecMsg;
    E_HS_CEC_OpCode  eEopcode;
}T_Android2Msopcode;

T_Android2Msopcode atAndroid2Msopcode[] =
{
    { CEC_MESSAGE_FEATURE_ABORT,                                     E_HS_CEC_OP_FEATURE_ABORT},
    { CEC_MESSAGE_IMAGE_VIEW_ON,                                 E_HS_CEC_OP_OTP_IMAGE_VIEW_ON},
    { CEC_MESSAGE_TUNER_STEP_INCREMENT,                          E_HS_CEC_OP_TC_TUNER_STEP_INC},
    { CEC_MESSAGE_TUNER_STEP_DECREMENT,                          E_HS_CEC_OP_TC_TUNER_STEP_DEC},
    { CEC_MESSAGE_TUNER_DEVICE_STATUS,                      E_HS_CEC_OP_TC_TUNER_DEVICE_STATUS},
    { CEC_MESSAGE_GIVE_TUNER_DEVICE_STATUS,                   E_HS_CEC_OP_TC_GIVE_TUNER_STATUS},
    { CEC_MESSAGE_RECORD_ON,                                         E_HS_CEC_OP_OTR_RECORD_ON},
    { CEC_MESSAGE_RECORD_STATUS,                                 E_HS_CEC_OP_OTR_RECORD_STATUS},
    { CEC_MESSAGE_RECORD_OFF,                                       E_HS_CEC_OP_OTR_RECORD_OFF},
    { CEC_MESSAGE_TEXT_VIEW_ON,                                   E_HS_CEC_OP_OTP_TEXT_VIEW_ON},
    { CEC_MESSAGE_RECORD_TV_SCREEN,                           E_HS_CEC_OP_OTR_RECORD_TV_SCREEN},
    { CEC_MESSAGE_GIVE_DECK_STATUS,                            E_HS_CEC_OP_DC_GIVE_DECK_STATUS},
    { CEC_MESSAGE_DECK_STATUS,                                      E_HS_CEC_OP_DC_DECK_STATUS},
    { CEC_MESSAGE_SET_MENU_LANGUAGE,                          E_HS_CEC_OP_SI_SET_MENU_LANGUAGE},
    { CEC_MESSAGE_CLEAR_ANALOG_TIMER,                        E_HS_CEC_OP_TP_CLEAR_ANALOG_TIMER},
    { CEC_MESSAGE_SET_ANALOG_TIMER,                            E_HS_CEC_OP_TP_SET_ANALOG_TIMER},
    { CEC_MESSAGE_TIMER_STATUS,                                    E_HS_CEC_OP_TP_TIMER_STATUS},
    { CEC_MESSAGE_STANDBY,                                                 E_HS_CEC_OP_STANDBY},
    { CEC_MESSAGE_PLAY,                                                    E_HS_CEC_OP_DC_PLAY},
    { CEC_MESSAGE_DECK_CONTROL,                                       E_HS_CEC_OP_DC_DECK_CTRL},
    { CEC_MESSAGE_TIMER_CLEARED_STATUS,                     E_HS_CEC_OP_TP_TIMER_CLEARD_STATUS},
    { CEC_MESSAGE_USER_CONTROL_PRESSED,                                   E_HS_CEC_OP_UI_PRESS},
    { CEC_MESSAGE_USER_CONTROL_RELEASED,                                E_HS_CEC_OP_UI_RELEASE},
    { CEC_MESSAGE_GIVE_OSD_NAME,                               E_HS_CEC_OP_OSDNT_GIVE_OSD_NAME},
    { CEC_MESSAGE_SET_OSD_NAME,                                 E_HS_CEC_OP_OSDNT_SET_OSD_NAME},
    { CEC_MESSAGE_SET_OSD_STRING,                                   E_HS_CEC_OP_SET_OSD_STRING},
    { CEC_MESSAGE_SET_TIMER_PROGRAM_TITLE,              E_HS_CEC_OP_TP_SET_TIMER_PROGRAM_TITLE},
    { CEC_MESSAGE_SYSTEM_AUDIO_MODE_REQUEST,         E_HS_CEC_OP_SAC_SYSTEM_AUDIO_MODE_REQUEST},
    { CEC_MESSAGE_GIVE_AUDIO_STATUS,                         E_HS_CEC_OP_SAC_GIVE_AUDIO_STATUS},
    { CEC_MESSAGE_SET_SYSTEM_AUDIO_MODE,                 E_HS_CEC_OP_SAC_SET_SYSTEM_AUDIO_MODE},
    { CEC_MESSAGE_REPORT_AUDIO_STATUS,                     E_HS_CEC_OP_SAC_REPORT_AUDIO_STATUS},
    { CEC_MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS, E_HS_CEC_OP_SAC_GIVE_SYSTEM_AUDIO_MODE_STATUS},
    { CEC_MESSAGE_SYSTEM_AUDIO_MODE_STATUS,           E_HS_CEC_OP_SAC_SYSTEM_AUDIO_MODE_STATUS},
    { CEC_MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR, E_HS_CEC_OP_SAC_REPORT_SHORT_AUDIO_DESCRIPTOR},
    { CEC_MESSAGE_ROUTING_CHANGE,                                E_HS_CEC_OP_RC_ROUTING_CHANGE},
    { CEC_MESSAGE_ROUTING_INFORMATION,                             E_HS_CEC_OP_RC_ROUTING_INFO},
    { CEC_MESSAGE_ACTIVE_SOURCE,                                     E_HS_CEC_OP_ACTIVE_SOURCE},
    { CEC_MESSAGE_GIVE_PHYSICAL_ADDRESS,                       E_HS_CEC_OP_SI_REQUEST_PHY_ADDR},
    { CEC_MESSAGE_REPORT_PHYSICAL_ADDRESS,                      E_HS_CEC_OP_SI_REPORT_PHY_ADDR},
    { CEC_MESSAGE_REQUEST_ACTIVE_SOURCE,                      E_HS_CEC_OP_RC_REQ_ACTIVE_SOURCE},
    { CEC_MESSAGE_SET_STREAM_PATH,                               E_HS_CEC_OP_RC_SET_STREM_PATH},
    { CEC_MESSAGE_DEVICE_VENDOR_ID,                            E_HS_CEC_OP_VS_DEVICE_VENDOR_ID},
    { CEC_MESSAGE_VENDOR_COMMAND,                                E_HS_CEC_OP_VS_VENDOR_COMMAND},
    { CEC_MESSAGE_VENDOR_REMOTE_BUTTON_DOWN,                 E_HS_CEC_OP_VS_VENDOR_RC_BUT_DOWN},
    { CEC_MESSAGE_VENDOR_REMOTE_BUTTON_UP,                     E_HS_CEC_OP_VS_VENDOR_RC_BUT_UP},
    { CEC_MESSAGE_GIVE_DEVICE_VENDOR_ID,                         E_HS_CEC_OP_VS_GIVE_VENDOR_ID},
    { CEC_MESSAGE_MENU_REQUEST,                                   E_HS_CEC_OP_DMC_MENU_REQUEST},
    { CEC_MESSAGE_MENU_STATUS,                                     E_HS_CEC_OP_DMC_MENU_STATUS},
    { CEC_MESSAGE_GIVE_DEVICE_POWER_STATUS,                   E_HS_CEC_OP_PS_GIVE_POWER_STATUS},
    { CEC_MESSAGE_REPORT_POWER_STATUS,                      E_HS_CEC_OP_PS_REPORT_POWER_STATUS},
    { CEC_MESSAGE_GET_MENU_LANGUAGE,                          E_HS_CEC_OP_SI_GET_MENU_LANGUAGE},
    { CEC_MESSAGE_SELECT_ANALOG_SERVICE,                     E_HS_CEC_OP_TC_SEL_ANALOG_SERVICE},
    { CEC_MESSAGE_SELECT_DIGITAL_SERVICE,                   E_HS_CEC_OP_TC_SEL_DIGITAL_SERVICE},
    { CEC_MESSAGE_SET_DIGITAL_TIMER,                          E_HS_CEC_OP_TP_SET_DIGITAL_TIMER},
    { CEC_MESSAGE_CLEAR_DIGITAL_TIMER,                      E_HS_CEC_OP_TP_CLEAR_DIGITAL_TIMER},
    { CEC_MESSAGE_SET_AUDIO_RATE,                               E_HS_CEC_OP_SAC_SET_AUDIO_RATE},
    { CEC_MESSAGE_INACTIVE_SOURCE,                              E_HS_CEC_OP_RC_INACTIVE_SOURCE},
    { CEC_MESSAGE_CEC_VERSION,                                      E_HS_CEC_OP_SI_CEC_VERSION},
    { CEC_MESSAGE_GET_CEC_VERSION,                              E_HS_CEC_OP_SI_GET_CEC_VERSION},
    { CEC_MESSAGE_VENDOR_COMMAND_WITH_ID,                E_HS_CEC_OP_VS_VENDOR_COMMAND_WITH_ID},
    { CEC_MESSAGE_CLEAR_EXTERNAL_TIMER,                         E_HS_CEC_OP_TP_CLEAR_EXT_TIMER},
    { CEC_MESSAGE_SET_EXTERNAL_TIMER,                             E_HS_CEC_OP_TP_SET_EXT_TIMER},
    { CEC_MESSAGE_INITIATE_ARC,                                   E_HS_CEC_OP_ARC_INITIATE_ARC},
    { CEC_MESSAGE_REPORT_ARC_INITIATED,                   E_HS_CEC_OP_ARC_REPORT_ARC_INITIATED},
    { CEC_MESSAGE_REPORT_ARC_TERMINATED,                 E_HS_CEC_OP_ARC_REPORT_ARC_TERMINATED},
    { CEC_MESSAGE_REQUEST_ARC_INITIATION,               E_HS_CEC_OP_ARC_REQUEST_ARC_INITIATION},
    { CEC_MESSAGE_REQUEST_ARC_TERMINATION,             E_HS_CEC_OP_ARC_REQUEST_ARC_TERMINATION},
    { CEC_MESSAGE_TERMINATE_ARC,                                E_HS_CEC_OP_ARC_TERMINATED_ARC},
    { CEC_MESSAGE_ABORT,                                             E_HS_CEC_OP_ABORT_MESSAGE},
};


const HS_U8 auCecOpCodeStr[256][19]=
{
    "<Feature Abort>",         //0x00
    "",
    "",
    "",
    "<Image View On>",
    "<Tuner Step Inc>",
    "<Tuner Step Dec>",
    "<Tuner Dev Status>",

    "<Give Tuner Dev>",
    "<Record On>",
    "<Record Status>",
    "<Record Off>",
    "",
    "<Text View On>",
    "",
    "<Record TV Scr>",

    "",  //0x10
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "<Give Deck Status>",
    "<Deck Status>",
    "",
    "",
    "",
    "",

    "",  //0x20
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0x30
    "",
    "<Set Menu Lang>",
    "<ClearAnalogTimer>",
    "<Set Analog Timer>",
    "<Timer Status>",
    "<Standby>",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0x40
    "<Play>",
    "<Deck Control>",
    "<TimerClearStatus>",
    "<UserCtrlPressed>",
    "<UserCtrlReleased>",
    "<Give OSD Name>",
    "<Set OSD Name>",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0x50
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0x60
    "",
    "",
    "",
    "<SetOSDString>[][]",
    "",
    "",
    "<SetTimerProgTit>",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "<SysAudioModeReq>",  //0x70
    "<Give Audio Sts>",
    "<SetSysAudioMode>",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "<ReportAudioSts>",
    "",
    "",
    "<GiveSysAudModSts>",
    "<SystemAudModeSts>",
    "",

    "<RountingChange>",                 //0x80
    "<RountingInfo>",
    "<Active Source>",
    "<GivePhysicalAddr>",
    "<Report Phy Addr>",
    "<ReqActiveSource>",
    "<Set Stream Path>",
    "<Device Vendor ID>",

    "",
    "<AVendorCommand>[]",
    "<VendorRteBtnDown>",
    "<VendorRteBtnUp>",
    "<GiveDeviceVenID>",
    "<MenuRequest>",
    "<MenuStatus>",
    "<GiveDevicePwrSts>",

    "<Report Power Sts>",   //0x90
    "<GetMenuLanguage>",
    "<SelectAnalogSer>",
    "<SelectDigitalSer>",
    "",
    "",
    "",
    "<SetDigitalTimer>",

    "",
    "<ClearDigiTimer>",
    "<Set Audio Rate>",
    "",
    "",
    "<Inactive Source>",
    "<CEC Version>",
    "<Get CEC Version>",

    "<VendorCmdwithID>",  //0xA0
    "<Clear Ext Timer>",
    "<Set Ext Timer>",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0xB0
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "<Init ARC>",  //0xC0
    "<Report ARC Init>",
    "<Report ARC Term>",
    "<Req ARC Init>",
    "<Req ARC Term>",
    "<Tem ARC>",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0xD0
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0xE0
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",  //0xF0
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "<Abort>Message",
};

