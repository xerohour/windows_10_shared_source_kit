#pragma once

typedef UINT8 BYTE;
typedef UINT8 * PBYTE;

#define Dot11ToHostShort(_x)                        \
    (((((PBYTE) &_x)[0])      &   0xFF) +           \
     ((((PBYTE) &_x)[1] << 8) & 0xFF00))

#define Dot11ToHostLong(_x)                         \
    (((((PBYTE) &_x)[0])       &       0xFF) +      \
     ((((PBYTE) &_x)[1] <<  8) &     0xFF00) +      \
     ((((PBYTE) &_x)[2] << 16) &   0xFF0000) +      \
     ((((PBYTE) &_x)[3] << 24) & 0xFF000000))

// It's a symmetrical operation
#define HostToDot11Short(_x)    Dot11ToHostShort(_x)
#define HostToDot11Long(_x)     Dot11ToHostLong(_x)


typedef enum _WFC_EVENT_TYPE : UINT32
{
    WiFiEventInvalid = 0,

    WiFiEventDeferredOperationCompletion,
    WiFiEventDeferredNotifyOperationStarted,

    WiFiEventQueueSerializedJob,
    WiFiEventAddActiveJob,
    WiFiEventStartJob,
    WiFiEventStartTask,

    WiFiEventDeviceCommandSchedulerQueueDeviceCommand,
    WiFiEventDeviceCommandSchedulerQueueCtlPlaneSendCommandComplete,

    WiFiEventNotificationManagerNotifyRegisteredClients,

    WiFiEventDeferredTimerCallback,

    WiFiEventDeferredSafeDeinitialize,

    WiFiEventDeferredThreadpoolCallback,
    WiFiEventJobOperationCompletedCallback,
} WFC_EVENT_TYPE;

typedef ULONG WFC_COMMAND_TOKEN;
#define WFC_COMMAND_TOKEN_INVALID ((WFC_COMMAND_TOKEN) WDI_TRANSACTION_ID_UNSOLICIT)

typedef UINT16 WFC_PORT_ID;

typedef ULONG WFC_REGISTRATION_TOKEN;
#define WFC_REGISTRATION_TOKEN_INVALID ((WFC_REGISTRATION_TOKEN)0)

typedef ULONG WFC_ACTIVITY_ID;
#define WFC_ACTIVITY_ID_UNKNOWN ((WFC_ACTIVITY_ID)0)

typedef UINT32 WFC_TIMER_REGISTRATION_TOKEN;
#define WFC_TIMER_REGISTRATION_TOKEN_INVALID ((WFC_TIMER_REGISTRATION_TOKEN)0)

typedef VOID* WFC_TIMER_CONTEXT;
typedef VOID* WFC_WORK_ITEM_CONTEXT;

typedef VOID* WFC_DEVICE_COMMAND_TOKEN;
#define WFC_DEVICE_COMMAND_TOKEN_NONE ((WFC_DEVICE_COMMAND_TOKEN)NULL)

typedef enum _WFC_DEVICE_COMMAND_TYPE
{
    WfcDeviceCommandTypeInvalid = 0,
    WfcDeviceCommandTypeProperty = 1,
    WfcDeviceCommandTypeTask = 2,
} WFC_DEVICE_COMMAND_TYPE;

typedef enum _WDI_INDICATION_TYPE : UINT32
{
    WiFiIndicationInvalid = (0),
    WiFiIndicationDisassociation = WDI_INDICATION_DISASSOCIATION,
    WiFiIndicationRoamingNeeded = WDI_INDICATION_ROAMING_NEEDED,
    WiFiIndicationLinkStateChange = WDI_INDICATION_LINK_STATE_CHANGE,
    WiFiIndicationP2PActionFrameReceived = WDI_INDICATION_P2P_ACTION_FRAME_RECEIVED,
    WiFiIndicationApAssociationRequestReceived = WDI_INDICATION_AP_ASSOCIATION_REQUEST_RECEIVED,
    WiFiIndicationNloDiscovery = WDI_INDICATION_NLO_DISCOVERY,
    WiFiIndicationWakeReason = WDI_INDICATION_WAKE_REASON,
    WiFiIndicationPmkIdCandidateListUpdate = WDI_INDICATION_PMKID_CANDIDATE_LIST_UPDATE,
    WiFiIndicationTkipMicFailure = WDI_INDICATION_TKIP_MIC_FAILURE,
    WiFiIndicationBssEntryList = WDI_INDICATION_BSS_ENTRY_LIST,
    WiFiIndicationRadioStatus = WDI_INDICATION_RADIO_STATUS,
    WiFiIndicationAssociationResult = WDI_INDICATION_ASSOCIATION_RESULT,
    WiFiIndicationP2POperatingChannel = WDI_INDICATION_P2P_GROUP_OPERATING_CHANNEL,
    WiFiIndicationP2POperatingChannelAttributes = WDI_INDICATION_P2P_OPERATING_CHANNEL_ATTRIBUTES,
    WiFiIndicationIHVEvent = WDI_INDICATION_IHV_EVENT,
    WiFiIndicationActionFrameReceived = WDI_INDICATION_ACTION_FRAME_RECEIVED,
    WiFiIndicationTaskOffloadCurrentConfig = WDI_INDICATION_TASK_OFFLOAD_CURRENT_CONFIG,
    WiFiIndicationFirmwareStalled = WDI_INDICATION_FIRMWARE_STALLED,
    WiFiIndicationAssociationParametersRequest = WDI_INDICATION_ASSOCIATION_PARAMETERS_REQUEST,
    WiFiIndicationStopAP = WDI_INDICATION_STOP_AP,
    WiFiIndicationCanSustainAP = WDI_INDICATION_CAN_SUSTAIN_AP,
    WiFiIndicationIhvTaskRequest = WDI_INDICATION_IHV_TASK_REQUEST,
    WiFiIndicationIhvTaskComplete = WDI_INDICATION_IHV_TASK_COMPLETE,
    WiFiIndicationFTParametersNeeded = WDI_INDICATION_FT_ASSOC_PARAMS_NEEDED
} WDI_INDICATION_TYPE;

typedef enum _WFC_SERIALIZED_JOB_PRIORITY_SCOPE
{
    WiFiPriorityScopePort,
    WiFiPriorityScopeAdapter,
} WFC_SERIALIZED_JOB_PRIORITY_SCOPE;


typedef enum _WFC_SERIALIZED_JOB_PRIORITY : int
{
    WiFiSerializedJobPriority_0 = 0,
    WiFiSerializedJobPriority_1,    // MiniportHalt, MiniportShutdown, MiniportReset
    WiFiSerializedJobPriority_2,    // NIC Power, Dot11 Reset, Create MAC, Disconnect, IhvTaskHigh
    WiFiSerializedJobPriority_3,    // WFD Send Action
    WiFiSerializedJobPriority_4,    // Connect, IhvTaskMedium
    WiFiSerializedJobPriority_5,    // Some WFD Sets
    WiFiSerializedJobPriority_6,    // Roam
    WiFiSerializedJobPriority_7,    // Some Sets
    WiFiSerializedJobPriority_8,    // Forced WFD Discover
    WiFiSerializedJobPriority_9,    // Unused
    WiFiSerializedJobPriority_10,   // Other Sets (Media Streaming, Power Save)
    WiFiSerializedJobPriority_11,   // Sets that need to go to HW
    WiFiSerializedJobPriority_Lowest // Queries, Software only sets, Scan, IhvTaskLow
} WFC_SERIALIZED_JOB_PRIORITY;


