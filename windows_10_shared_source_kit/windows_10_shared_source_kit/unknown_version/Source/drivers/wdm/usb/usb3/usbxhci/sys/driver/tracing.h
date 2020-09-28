/*++

Copyright (c) Microsoft Corporation

Module Name:

    tracing.hpp

Abstract:

    Debug stuff

--*/

#pragma once

//
// Defining control guids, including this is required to happen before
// including the tmh file (if the WppRecorder API is used)
//
#include <WppRecorder.h>

#define WPP_CONTROL_GUIDS \
            WPP_DEFINE_CONTROL_GUID(USBXHCI, (9F7711DD,29AD,C1EE,1B1B,B52A0118A54C), \
                WPP_DEFINE_BIT(General)      \
                WPP_DEFINE_BIT(Driver)       \
                WPP_DEFINE_BIT(Controller)   \
                WPP_DEFINE_BIT(IoControl)    \
                WPP_DEFINE_BIT(Register)     \
                WPP_DEFINE_BIT(Command)      \
                WPP_DEFINE_BIT(CommonBuffer) \
                WPP_DEFINE_BIT(Interrupter)  \
                WPP_DEFINE_BIT(DeviceSlot)   \
                WPP_DEFINE_BIT(RootHub)      \
                WPP_DEFINE_BIT(UsbDevice)    \
                WPP_DEFINE_BIT(Endpoint)     \
                WPP_DEFINE_BIT(TransferRing) \
                WPP_DEFINE_BIT(Wmi)          \
                WPP_DEFINE_BIT(CommandFilter)\
                WPP_DEFINE_BIT(StateMachine) \
            )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl,flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

#define TL_FATAL        0x01
#define TL_ERROR        0x02
#define TL_WARNING      0x03
#define TL_INFO         0x04
#define TL_VERBOSE      0x05

// begin_wpp config
// LOG_FATAL{LEVEL=TL_FATAL}(FLAGS,MSG,...);
// LOG_ERROR{LEVEL=TL_ERROR}(FLAGS,MSG,...);
// LOG_WARNING{LEVEL=TL_WARNING}(FLAGS,MSG,...);
// LOG_INFO{LEVEL=TL_INFO}(FLAGS,MSG,...);
// LOG_VERBOSE{LEVEL=TL_VERBOSE}(FLAGS,MSG,...);
// LogFatal{LEVEL=TL_FATAL}(IFRLOG,FLAGS,MSG,...);
// LogError{LEVEL=TL_ERROR}(IFRLOG,FLAGS,MSG,...);
// LogWarning{LEVEL=TL_WARNING}(IFRLOG,FLAGS,MSG,...);
// LogInfo{LEVEL=TL_INFO}(IFRLOG,FLAGS,MSG,...);
// LogVerbose{LEVEL=TL_VERBOSE}(IFRLOG,FLAGS,MSG,...);
// end_wpp

//
// Below enum types are a copy of the corresponding #defines in usbxhci.h.
// These enum types are only used to get the integer values pretty printed
// in WPP log statements.
//
// The reason usbxhci.h uses #defines instead of enum types is that if the
// enum type is assigned to a bitfield, the value gets sign-extended when
// extracted from the bitfield. This is because C enum types are signed.
//

typedef enum _TRB_COMPLETION_CODE {

    CC_INVALID                           = 0,
    CC_SUCCESS                           = 1,
    CC_DATA_BUFFER_ERROR                 = 2,
    CC_BABBLE_DETECTED_ERROR             = 3,
    CC_USB_TRANSACTION_ERROR             = 4,
    CC_TRB_ERROR                         = 5,
    CC_STALL_ERROR                       = 6,
    CC_RESOURCE_ERROR                    = 7,
    CC_BANDWIDTH_ERROR                   = 8,
    CC_NO_SLOTS_AVAILABLE_ERROR          = 9,
    CC_INVALID_STREAM_TYPE_ERROR         = 10,
    CC_SLOT_NOT_ENABLED_ERROR            = 11,
    CC_ENDPOINT_NOT_ENABLED_ERROR        = 12,
    CC_SHORT_PACKET                      = 13,
    CC_RING_UNDERRUN                     = 14,
    CC_RING_OVERRUN                      = 15,
    CC_VF_EVENT_RING_FULL_ERROR          = 16,
    CC_PARAMETER_ERROR                   = 17,
    CC_BANDWIDTH_OVERRUN_ERROR           = 18,
    CC_CONTEXT_STATE_ERROR               = 19,
    CC_NO_PING_RESPONSE_ERROR            = 20,
    CC_EVENT_RING_FULL_ERROR             = 21,
    CC_INCOMPATIBLE_DEVICE_ERROR         = 22,
    CC_MISSED_SERVICE_ERROR              = 23,
    CC_COMMAND_RING_STOPPED              = 24,
    CC_COMMAND_ABORTED                   = 25,
    CC_STOPPED                           = 26,
    CC_STOPPED_LENGTH_INVALID            = 27,
    CC_STOPPED_SHORT_PACKET              = 28,
    CC_MAX_EXIT_LATENCY_TOO_LARGE_ERROR  = 29,
    CC_ISOCH_BUFFER_OVERRUN              = 31,
    CC_EVENT_LOST_ERROR                  = 32,
    CC_UNDEFINED_ERROR                   = 33,
    CC_INVALID_STREAM_ID_ERROR           = 34,
    CC_SECONDARY_BANDWIDTH_ERROR         = 35,
    CC_SPLIT_TRANSACTION_ERROR           = 36

} TRB_COMPLETION_CODE;

// begin_wpp config
// CUSTOM_TYPE(TrbCompletionCode, ItemEnum(_TRB_COMPLETION_CODE));
// end_wpp

typedef enum _TRB_TYPE {

    TT_RESERVED                             = 0,
    TT_NORMAL                               = 1,
    TT_SETUP_STAGE                          = 2,
    TT_DATA_STAGE                           = 3,
    TT_STATUS_STAGE                         = 4,
    TT_ISOCH                                = 5,
    TT_LINK                                 = 6,
    TT_EVENT_DATA                           = 7,
    TT_ENABLE_SLOT_COMMAND                  = 9,
    TT_DISABLE_SLOT_COMMAND                 = 10,
    TT_ADDRESS_DEVICE_COMMAND               = 11,
    TT_CONFIGURE_ENDPOINT_COMMAND           = 12,
    TT_EVALUATE_CONTEXT_COMMAND             = 13,
    TT_RESET_ENDPOINT_COMMAND               = 14,
    TT_STOP_ENDPOINT_COMMAND                = 15,
    TT_SET_DEQUEUE_POINTER_COMMAND          = 16,
    TT_RESET_DEVICE_COMMAND                 = 17,
    TT_FORCE_EVENT_COMMAND                  = 18,
    TT_NEGOTIATE_BANDWIDTH_COMMAND          = 19,
    TT_SET_LATENCY_TOLERANCE_VALUE_COMMAND  = 20,
    TT_GET_PORT_BANDWIDTH_COMMAND           = 21,
    TT_FORCE_HEADER_COMMAND                 = 22,
    TT_NO_OP_COMMAND                        = 23,
    TT_TRANSFER_EVENT                       = 32,
    TT_COMMAND_COMPLETION_EVENT             = 33,
    TT_PORT_STATUS_CHANGE_EVENT             = 34,
    TT_BANDWIDTH_REQUEST_EVENT              = 35,
    TT_DOORBELL_EVENT                       = 36,
    TT_HOST_CONTROLLER_EVENT                = 37,
    TT_DEVICE_NOTIFICATION_EVENT            = 38,
    TT_MFINDEX_WRAP_EVENT                   = 39,
    TT_VENDOR_CODE_199                      = 199

} TRB_TYPE;

// begin_wpp config
// CUSTOM_TYPE(TrbType, ItemEnum(_TRB_TYPE));
// end_wpp

typedef enum _SLOT_STATE {

    SS_DISABLED_OR_ENABLED  = 0,
    SS_DEFAULT              = 1,
    SS_ADDRESSED            = 2,
    SS_CONFIGURED           = 3

} SLOT_STATE;

// begin_wpp config
// CUSTOM_TYPE(SlotState, ItemEnum(_SLOT_STATE));
// end_wpp

typedef enum _ENDPOINT_STATE {

    ES_DISABLED = 0,
    ES_RUNNING  = 1,
    ES_HALTED   = 2,
    ES_STOPPED  = 3,
    ES_ERROR    = 4

} ENDPOINT_STATE;

// begin_wpp config
// CUSTOM_TYPE(EndpointState, ItemEnum(_ENDPOINT_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(CrbCompletionStatus, ItemEnum(_CRB_COMPLETION_STATUS));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(SystemPowerAction, ItemEnum(_SYSTEM_POWER_ACTION));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(DeviceEnumerator, ItemEnum(_DEVICE_ENUMERATOR));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ParentBusType, ItemEnum(_UCX_CONTROLLER_PARENT_BUS_TYPE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(Controller_FatalErrorAction, ItemEnum(_Controller_FatalErrorAction));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(StreamState, ItemEnum(_STREAM_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(TransferRingState, ItemEnum(_TRANSFERRING_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(TransferState, ItemEnum(_TRANSFER_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(WdfSpecialFileType, ItemEnum(_WDF_SPECIAL_FILE_TYPE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(WdfPowerDeviceState, ItemEnum(_WDF_POWER_DEVICE_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(BugCheckReasonCode, ItemEnum(_USB3_BUGCHECK_REASON_CODE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ControlState, ItemEnum(_CONTROL_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ControlEvent, ItemEnum(_CONTROL_EVENT));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(IsochState, ItemEnum(_ISOCH_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(IsochEvent, ItemEnum(_ISOCH_EVENT));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ESMEVENT, ItemEnum(_ESM_EVENT));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(ESMSTATE, ItemEnum(_ESM_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(MappingState, ItemEnum(_MAPPING_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(S0IdleStatus, ItemEnum(_S0IdleStatus));
// end_wpp
