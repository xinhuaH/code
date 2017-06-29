#include <hs_type.h>
#define INIT_ST(st) do { \
    memset(&st, 0, sizeof(st));\
    st.u16SizeofStructure = sizeof(st);\
}while(0)

typedef enum
{
    E_HS_SIGNAL_STATUS_LOCK,
    E_HS_SIGNAL_STATUS_UNLOCK,
    E_HS_SIGNAL_STATUS_NOTSUPPORT,

} E_HS_EXTIN_SIGNAL_STATUS;

typedef enum HS_UNF_HDMIRX_RCP_KEY_E
{
    HS_UNF_HDMIRX_RCP_KEY_SELECT,
    HS_UNF_HDMIRX_RCP_KEY_UP,
    HS_UNF_HDMIRX_RCP_KEY_DOWN,
    HS_UNF_HDMIRX_RCP_KEY_LEFT,
    HS_UNF_HDMIRX_RCP_KEY_RIGHT,
    HS_UNF_HDMIRX_RCP_KEY_ROOT_MENU,
    HS_UNF_HDMIRX_RCP_KEY_EXIT,
    HS_UNF_HDMIRX_RCP_KEY_PLAY,
    HS_UNF_HDMIRX_RCP_KEY_STOP,
    HS_UNF_HDMIRX_RCP_KEY_PAUSE,
    HS_UNF_HDMIRX_RCP_KEY_REWIND,
    HS_UNF_HDMIRX_RCP_KEY_FAST_FWD,
    HS_UNF_HDMIRX_RCP_KEY_BUTT,
}HS_UNF_HDMIRX_RCP_KEY_E;

typedef enum
{
    HS_MHL_MSC_RCP_NO_ERROR = 0x00,     // RCP No Error
    HS_MHL_MSC_RCP_INEFFECTIVE_KEY_CODE = 0x01,     // The key code in the RCP sub-command is not recognized
    HS_MHL_MSC_RCP_RESPONDER_BUSY = 0x02,     // RCP Response busy
}  HS_MHL_MSC_RcpStatusCode_t;
typedef struct
{
    HS_U16 u16SizeofStructure;
    HS_U32 u32Reserved;
}HS_MHL_InitParams_t;
typedef struct HS_MHL_OpenParams_s
{
    HS_U16 u16SizeofStructure;
  HS_U32 u32MhlPortIndex;
}HS_MHL_OpenParams_t;       //MHL module Open Param

typedef HS_U8(*HS_MHL_EventCallback)(HS_HANDLE hMhlHandle,HS_U8 RcpCode);
typedef struct
{
    HS_MHL_EventCallback pfEventCallback;       //Rcp Cmd callback function point
    void *pUsrParams;
} HS_MHL_CallbackParams_t;
typedef enum
{
    E_HS_MHL_Vbus_HW_DETECT = 0,
      E_HS_MHL_Vbus_SW_Charge_ON,       // open 5V charge
    E_HS_MHL_Vbus_SW_Charge_OFF,    //close 5V charge
} HS_MHL_VbusCrtl_Type_e;


typedef enum
{
    E_HS_EXTIN_COMPONENT,
    E_HS_EXTIN_VGA,
    E_HS_EXTIN_HDMI,   //HDHS source can be separate by HDHS, DVI , MHL type.
    E_HS_EXTIN_CVBS,
    E_HS_EXTIN_SVIDEO,
    E_HS_EXTIN_SCART,
    E_HS_EXTIN_NUM
} HS_EXTIN_INPUTSOURCE_TYPE_e;


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
