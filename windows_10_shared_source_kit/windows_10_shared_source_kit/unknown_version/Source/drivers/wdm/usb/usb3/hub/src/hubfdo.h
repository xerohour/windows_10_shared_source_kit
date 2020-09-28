/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubfdo.h

Abstract:

    USB HUB FDO

--*/


#pragma once

#include "triage-usbhub3.h"

#define USBPORT_MAX_CONTROL_TRANSFER_SIZE       1024*64
#define USBPORT_MAX_INTERRUPT_TRANSFER_SIZE     1024*1024*4
#define USBPORT_MAX_BULK_TRANSFER_SIZE          1024*1024*4

#define MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE 255
#define MAXIMUM_USB_STRING_LENGTH 255
#define MAXIMUM_UNIQUE_ID_STRING_LENGTH 16
#define AMERICAN_ENGLISH    0x409
#define MAXIMUM_BOS_DESCRIPTOR_SIZE 255
#define MAXIMUM_EXTENDED_PROPERTY_DESCRIPTOR_SIZE 1024*4
#define MINIMUM_CUSTOM_PROPERTY_SIZE 14
#define MAXIMUM_OVERCURRENT_AUTO_RECOVER_COUNT 5
#define MINIMUM_TIME_BETWEEN_INDEPENDENT_OVERCURRENTS_IN_SEC 3
#define PORT_OVERCURRENT_TIMEOUT 500
#define PORT_RECONNECT_TIMEOUT 1000
#define PORT_RECONNECT_TIMEOUT_FOR_BOOT_DEVICE 2000
#define PORT_D3_RECONNECT_TIMEOUT 1000
#define PORT_DEBOUNCE_TIMEOUT 100
#define PORT_RESET_TIMEOUT 3000
#define PORT_RESUME_TIMEOUT 500
#define PORT_POWER_ON_TIMEOUT 300
#define PORT_POWER_ON_TIMEOUT_FOR_ROOT_PORTS 100
#define PORT_RESET_COMPLETE_POLLING_INTERVAL 500
#define PORT_RESUME_RECOVERY_TIMEOUT 10
#define PORT_SUPERSPEED_DISABLE_TIMEOUT 5000
#define PORT_RESUME_TIME 20

//
// In Windows 8 the 10ms post reset delay would typically be around 15ms due to the granularity of the system timer.
// In Windows 8.1 the USB stack moved to using the high resolution timer for these delays, so the actual delay
// was reduced to the spec required 10ms rather than 15ms.
//
// It was found that some devices occasionally fail the first request after reset if the delay is only 10ms.
// Changing the delay to 15ms allows these devices to enumerate and reset normally.
//
#define DEVICE_POST_RESET_TIMEOUT_FOR_30 15

//
// For legacy speed devices, use a 32ms timeout which is close to the delay used before the move
// to high resolution timers (USB 2.0 stack would add 16ms to all waits in order to prevent
// their early expiration).
//

#define DEVICE_POST_RESET_TIMEOUT_FOR_20 32

#define DEVICE_POST_ADDRESS_TIMEOUT 10
#define DEVICE_POST_RESET_EXTENDED_TIMEOUT 100
#define DEVICE_ENUMERATION_RETRY_TIMEOUT 500
#define DEVICE_DUPLICATE_SN_POLLING_TIMEOUT 500
#define DEVICE_DUPLICATE_SN_MAX_RETRY_COUNT 10
#define DEVICE_OUT_OF_BANDWIDTH_NOTIFICATION_TIMEOUT 1000
#define DEVICE_NO_DRIVER_FOUND_TIMEOUT 10000

#define DESCRIPTOR_VALIDATION_ARRAY_COUNT 8

#define FULL_SPEED_HUB_PROTOCOL 0
#define SINGLE_TT_HUB_PROTOCOL  1
#define MULTI_TT_HUB_PROTOCOL   2

#define LATENCY_ADJUSTMENT_INITIAL_VALUE        10
#define LATENCY_ADJUSTMENT_MULTIPLIER           2
#define LATENCY_ADJUSTMENT_MAXIMUM_VALUE        160

#define U1_TIMEOUT_FOR_CONTROL_ENDPOINTS    5
#define U2_TIMEOUT_FOR_CONTROL_ENDPOINTS    1
#define EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT   5

//
// There is no good mechanism to calculate this
// value and is theoritically unbounded. So we just
// asked the host controller vendors and took the
// worst case out of the values suggested to us
//
#define HOST_RESPONSE_TIME_IN_MICROSECONDS                  5

#define CONTROL_TRANSFER_TIMEOUT                5000

#define USB_20HUB_SERIAL_NUMBER_DECORATION      L"MSFT20"
#define USB_30HUB_SERIAL_NUMBER_DECORATION      L"MSFT30"
#define USB_SERIAL_NUMBER_DECORATION            L"MSFT30"
#define USB_SERIAL_NUMBER_DECORATION_SIZE       (sizeof(L"MSFT30") - sizeof(WCHAR))

#define DEFAULT_20_HARDWARE_LPM_BASELINE_BESL           4 //Translates to 400 us
#define DEFAULT_20_HARDWARE_LPM_TIMEOUT                 2 //Translates to 512 us

// {48E6B7A6-50F5-4782-A5D4-53BB8F07E226}
DEFINE_GUID(GUID_USB_SETTING_SELECTIVE_SUSPEND,
0x48e6b7a6, 0x50f5, 0x4782, 0xa5, 0xd4, 0x53, 0xbb, 0x8f, 0x7, 0xe2, 0x26);

// {0853A681-27C8-4100-A2FD-82013E970683}
DEFINE_GUID(GUID_POWER_HUB_SELECTIVE_SUSPEND_TIMEOUT,
0x853a681, 0x27c8, 0x4100, 0xa2, 0xfd, 0x82, 0x1, 0x3e, 0x97, 0x6, 0x83);

#define DEFAULT_HUB_IDLE_TIMEOUT   50

typedef struct _UCX_IOCTL_REQUEST {
    union {
        struct _USBDEVICE_ENABLE
            DeviceEnable;
        struct _USBDEVICE_RESET
            DeviceReset;
        struct _USBDEVICE_ADDRESS
            DeviceAddress;
        struct _ENDPOINTS_CONFIGURE
            EndpointsConfigure;
        struct _USBDEVICE_PURGEIO
            UsbDevicePurgeIo;
        struct _USBDEVICE_STARTIO
            UsbDeviceStartIo;
        struct _USBDEVICE_UPDATE
            DeviceUpdate;
        struct _DEFAULT_ENDPOINT_UPDATE
            EndpointUpdate;
        struct _USBDEVICE_DISABLE
            DeviceDisable;
        struct _ADDRESS0_OWNERSHIP_ACQUIRE
            Address0OwnershipAcquire;
        struct _USBDEVICE_HUB_INFO
            HubInfo;
    };
} UCX_IOCTL_REQUEST, *PUCX_IOCTL_REQUEST;

typedef struct _CONTROL_REQUEST {
    WDFREQUEST                       WdfRequest;
    ULONG                            NumberOfBytes;
    //
    // Irp is only used in debugger extension
    //
    PIRP                             Irp;
    union {
        struct _URB_CONTROL_TRANSFER_EX  Urb;
        struct _URB_PIPE_REQUEST         PipeUrb;
    };

    ULONG                            UsbdFlags;
} CONTROL_REQUEST, *PCONTROL_REQUEST;

typedef struct _HUB_INTERRUPT_REQUEST {
    WDFREQUEST                              WdfRequest;
    struct _URB_BULK_OR_INTERRUPT_TRANSFER  Urb;
    USBD_PIPE_HANDLE                        UsbdInterruptPipeHandle;
    BOOLEAN                                 NoChangesInLastInterrupt;
    PULONG                                  StatusChangeBitMap;
    USHORT                                  MaxStatusChangeBitMapSize;
    USHORT                                  CurrentStatusChangeBitMapSize;
    RTL_BITMAP                              StatusChangeBitMapHeader;
} HUB_INTERRUPT_REQUEST, *PHUB_INTERRUPT_REQUEST;


//
// This is the maximum number of enumeration
// retry attempts
//
#define MAXIMUM_ENUMERATION_RETRY_COUNT     3
//
// This is the maximum number of times a hub
// will be reset in a given recovery instance
//
#define MAXIMUM_HUB_RESET_COUNT             3
//
// This is the maximum number of times the interrupt
// pipe of the hub will be reset on interrupt trasnfer
// failures before resorting to hub reset
//
#define MAXIMUM_INTERRUPT_PIPE_RESET_COUNT             3
//
// This is the maximum number of times a hub
// is allowed to use reset recovery mechanism
// in MAXIMUM_HUB_RECOVERIES_TIME_PERIOD seconds
//
#define MAXIMUM_HUB_RECOVERIES_COUNT                10
#define MAXIMUM_HUB_RECOVERIES_TIME_PERIOD_IN_SEC   60

#define MINIMUM_TIME_BETWEEN_CONSECUTIVE_HOT_RESETS_IN_SEC  1

#define HUB_CHANGE_BIT 0

#define HUB_OVERCURRENT_TIMEOUT 500
#define HUB_RESET_RETRY_TIMEOUT 500
#define HUB_DESCRIPTOR_RETRY_TIMEOUT 100


typedef union _PORT_MUX_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       PortChangePending:1;
        ULONG       InterruptReferenceHeld:1;
        ULONG       PnpOperationPending:1;
        ULONG       PowerReferenceHeld:1;
        ULONG       ResetPending:1;
        ULONG       WdfPowerReferenceHeld:1;
    };
} PORT_MUX_FLAGS, *PPORT_MUX_FLAGS;

C_ASSERT(sizeof(PORT_MUX_FLAGS) == sizeof(ULONG));

typedef struct _PSM_MUX_CONTEXT {
    LIST_ENTRY                      ListEntry;
    PORT_MUX_FLAGS                  PortFlags;
} PSM_MUX_CONTEXT, *PPSM_MUX_CONTEXT;


typedef union _DEVICE_MUX_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       Stopped:1;
        ULONG       PowerReferenceHeld:1;
        ULONG       PnpOperationPending:1;
    };
} DEVICE_MUX_FLAGS, *PDEVICE_MUX_FLAGS;

C_ASSERT(sizeof(DEVICE_MUX_FLAGS) == sizeof(ULONG));

typedef struct _DSM_MUX_CONTEXT {
    LIST_ENTRY                      ListEntry;
    LIST_ENTRY                      TempListEntry;
    LIST_ENTRY                      SurpriseRemoveListEntry;
    DEVICE_MUX_FLAGS                DeviceFlags;
} DSM_MUX_CONTEXT, *PDSM_MUX_CONTEXT;

typedef enum _PORT_PROTOCOL {
    PortProtocolInvalid = 0,
    PortProtocolUsb20   = 0x200,
    PortProtocolUsb30   = 0x300
} PORT_PROTOCOL, *PPORT_PROTOCOL;

typedef union _PORT_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   PortFlagRemovable:1;
        ULONG   PortFlagAcpiUpcValid:1;
        ULONG   PortFlagAcpiPldValid:1;
        ULONG   PortFlagDebugCapable:1;
        ULONG   PortFlagIntegratedHubImplemented:1;
        ULONG   PortFlagRegisteredWithConnectorMap:1;
        ULONG   PortFlagUsb20HardwareLpmSupported:1;
        ULONG   PortFlagUsb20HardwareLpmBeslCapabilitySupported:1;
        ULONG   PortFlagEnhancedSuperSpeed:1;
        ULONG   PortFlagTypeCWithoutSwitch:1;
    };
} PORT_FLAGS, *PPORT_FLAGS;

C_ASSERT(sizeof(PORT_FLAGS) == sizeof(ULONG));

typedef struct _PORT_INFORMATION {
    USHORT                                       PortNumber;
    UCHAR                                        TotalHubDepth;
    PORT_FLAGS                                   PortFlags;
    PORT_PROTOCOL                                PortProtocol;
    USHORT                                       FirstMapped30PortNumber;
    USHORT                                       LastMapped30PortNumber;
    ULONG                                        InterconnectType;
    ULONG                                        SuperSpeedPlusIsochBurstCount;
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  SublinkSpeedAttr;
    ULONG                                        SublinkSpeedAttrCount;
    USHORT                                       PortNumberForInstanceId;
} PORT_INFORMATION, *PPORT_INFORMATION;

//
// Forward Declarations
//
typedef struct _HUB_FDO_CONTEXT     HUB_FDO_CONTEXT, *PHUB_FDO_CONTEXT;
typedef struct _PORT_CONTEXT        PORT_CONTEXT, *PPORT_CONTEXT;
typedef struct _HUB_PDO_CONTEXT     HUB_PDO_CONTEXT, *PHUB_PDO_CONTEXT;


typedef union _USB_DEVICE_PROPERTIES {
    ULONG   AsUlong32;
    struct {
        ULONG       DeviceReservedUXD:1;
        ULONG       DeviceIsHub:1;
        ULONG       DeviceIsComposite:1;
        ULONG       ContainerIdValid:1;
        ULONG       BOSContainerIdValid:1;
        ULONG       DeviceNotRemovable:1;
        ULONG       SerialNumber:1;
        ULONG       MsOsDescriptorNotSupported:1;
        ULONG       UsbWakeupSupport:1;
        ULONG       ExtPropertyInstalled:1;
        ULONG       DeviceSupportsSelectiveSuspend:1;
        ULONG       DeviceIsSuperSpeedCapable:1;
        ULONG       DeviceIsEnhancedSuperSpeedCapable:1;
        ULONG       DeviceIsHighSpeedCapable:1;
        ULONG       AllowIdleIrpInD3:1;
        ULONG       D3ColdSupportedInAcpi:1;
        ULONG       ChargingPolicySupported:1;
        ULONG       DeviceIsBillboard:1;
        ULONG       DeviceIsDualRole:1;
        ULONG       FailGetStatusRequest:1;
        ULONG       LtmCapable:1;
    };
} USB_DEVICE_PROPERTIES, *PUSB_DEVICE_PROPERTIES;

C_ASSERT(sizeof(USB_DEVICE_PROPERTIES) == sizeof(ULONG));

typedef union _USB_DEVICE_STATE_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       DeviceAttachSuccessful:1;
        ULONG       DeviceIsKnown:1;
        ULONG       HotResetOnEnumRequired:1;
        ULONG       WarmResetOnEnumRequired:1;
        ULONG       ConfigurationIsValid:1;
        ULONG       DeviceReprogrammingPending:1;
        ULONG       ActivityIdSetForDsmRequests:1;
        ULONG       WdfActivityIdValid:1;
        ULONG       WdfPowerReferenceHeldOnFdo:1;
        ULONG       DeviceFailedAtLeastOneEnumeration:1;
        ULONG       DifferentDeviceOnBootDevicePort:1;
        ULONG       DecorateSerialNumber:1;
        ULONG       D3ColdEnabledByFunctionDriver:1;
        ULONG       ConfigDescIsValid:1;
        ULONG       DeviceStarted:1;
        ULONG       FailAlternateSetting:1;
        ULONG       LastSetAddressFailed:1;
        ULONG       InstallMSOSExtEventProcessed:1;
        ULONG       DualRoleVendorCmdSent:1;
    };
} USB_DEVICE_STATE_FLAGS, *PUSB_DEVICE_STATE_FLAGS;

C_ASSERT(sizeof(USB_DEVICE_STATE_FLAGS) == sizeof(ULONG));

typedef union _DEVICE_LPM_STATE_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       SlowestLinkForU1Exit:1;
        ULONG       SlowestLinkForU2Exit:1;
        ULONG       U1EnabledForUSPort:1;
        ULONG       U2EnabledForUSPort:1;
        ULONG       TargetU1EnabledForUSPort:1;
        ULONG       TargetU2EnabledForUSPort:1;
        ULONG       U1DisabledDueToIntolerableLatency:1;
        ULONG       U2DisabledDueToIntolerableLatency:1;
        ULONG       U1U2DisabledDueToNoPingResponse:1;
    };

} DEVICE_LPM_STATE_FLAGS, *PDEVICE_LPM_STATE_FLAGS;

C_ASSERT(sizeof(DEVICE_LPM_STATE_FLAGS) == sizeof(ULONG));

typedef union _LPM_POLICY {
    ULONG   AsUlong32;
    struct {
        UCHAR       U1EnabledForUSPort:1;
        UCHAR       U2EnabledForUSPort:1;
        UCHAR       U1TimeoutAcceptTransitions:1;
        UCHAR       U2TimeoutAcceptTransitions:1;
        UCHAR       U1TimeoutInitiateTransitions:1;
        UCHAR       U2TimeoutInitiateTransitions:1;
        UCHAR       ForceConservativeTimeoutValues:1;
        UCHAR       ForceAggressiveTimeoutValues:1;
    };
} LPM_POLICY, *PLPM_POLICY;

C_ASSERT(sizeof(LPM_POLICY) == sizeof(ULONG));


typedef struct _DEVICE_20_LPM_ATTRIBUTES {

    ULONG   LPMCapable:1;
    ULONG   BESLAndAlternateHIRDSupported:1;
    ULONG   BaselineBESLValid:1;
    ULONG   DeepBESLValid:1;
    ULONG   BaselineBESL:4;
    ULONG   DeepBESL:4;

} DEVICE_20_LPM_ATTRIBUTES, *PDEVICE_20_LPM_ATTRIBUTES;


typedef struct _ALTERNATE_MODE {
    USHORT  wSVID;
    UCHAR   bAlternateMode;
    UCHAR   iAlternateModeSetting;
} ALTERNATE_MODE, *PALTERNATE_MODE;

typedef struct _BILLBOARD_PROPERTIES {

    UCHAR                           AddtionalInfoURL;
    UCHAR                           NumberOfAlternateModes;
    ALTERNATE_MODE                  PreferredAlternateMode;
    ALTERNATE_MODE                  SuccessfulAlternateMode;
    ALTERNATE_MODE                  FailedAlternateMode;
    /*ALTERNATE_MODE_CONFIG_ERROR*/UCHAR     ConfigError;

} BILLBOARD_PROPERTIES, *PBILLBOARD_PROPERTIES;

//
// These properties are written into the registry
// to be later consumed by the SQM engine. The postion
// of each bit in this structure is defined by the
// SQM engine and therefore should not be changed
// unless directed by the SQM requirements
//
typedef union _USB_DEVICE_SQM_PROPERTIES {
    ULONG   AsUlong32;
    struct {
        ULONG       Reserved1:1;
        ULONG       Reserved2:1;
        ULONG       Virtualized:1;                      //Bit 2
        ULONG       IsOnXhci:1;                         //Bit 3
        ULONG       HostSpecialFiles:1;                 //Bit 4
        ULONG       ValidBOSDescriptor:1;               //Bit 5
        ULONG       SupportsStreams:1;                  //Bit 6
        ULONG       SupportsMultipleConfigurations:1;   //Bit 7
        ULONG       Reserved3:1;
        ULONG       IsSuperSpeedCapable:1;              //Bit 9
    };
} USB_DEVICE_SQM_PROPERTIES, *PUSB_DEVICE_SQM_PROPERTIES;

C_ASSERT(sizeof(USB_DEVICE_SQM_PROPERTIES) == sizeof(ULONG));

typedef union _USB_DEVICE_HACKS {
    ULONG   AsUlong32;
    struct {
        ULONG       DisableSerialNumber:1;
        ULONG       DontSkipMsOsDescriptor:1;
        ULONG       ResetOnResumeSx:1;
        ULONG       DisableOnSoftRemove:1;
        ULONG       RequestConfigDescOnReset:1;
        ULONG       SkipContainerIdQuery:1;
        ULONG       IgnoreBOSDescriptorValidationFailure:1;
        ULONG       DisableLpm:1;
        ULONG       SkipSetSel:1;
        ULONG       ResetOnResumeInSuperSpeed:1;
        ULONG       AllowInvalidPipeHandles:1;
        ULONG       DisableUASP:1;
        ULONG       SkipSetIsochDelay:1;
        ULONG       ResetOnResumeS0:1;
        ULONG       DisableHotReset:1;
        ULONG       SkipBOSDescriptorQuery:1;
        ULONG       NonFunctional:1;
        ULONG       DisableUsb20HardwareLpm:1;
        ULONG       DisableRemoteWakeForUsb20HardwareLpm:1;
        ULONG       DisableSuperSpeed:1;
        ULONG       IncompatibleWithWindows:1;
        ULONG       UseWin8DescriptorValidation:1;
        ULONG       DisableFastEnumeration:1;
    };
} USB_DEVICE_HACKS, *PUSB_DEVICE_HACKS;

C_ASSERT(sizeof(USB_DEVICE_HACKS) == sizeof(ULONG));


typedef enum _DEVICE_HW_VERIFIER_FLAGS {

    DeviceHwVerifierControlTransferFailure              =   0x00000001,
    DeviceHwVerifierDescriptorValidationFailure         =   0x00000002,
    DeviceHwVerifierSetSelFailure                       =   0x00000004,
    DeviceHwVerifierSetIsochDelayFailure                =   0x00000008,
    DeviceHwVerifierInterfaceWakeCapabilityMismatch     =   0x00000010,
    DeviceHwVerifierSerialNumberMismatchOnRenumeration  =   0x00000020,
    DeviceHwVerifierBusRenumeration                     =   0x00000040,
    DeviceHwVerifierClientInitiatedResetPipe            =   0x00000100,
    DeviceHwVerifierClientInitiatedResetPort            =   0x00000200,
    DeviceHwVerifierClientInitiatedCyclePort            =   0x00000400,
    DeviceHwVerifierSuperSpeedDeviceWorkingAtLowerSpeed =   0x00000800,
    DeviceHwVerifierSetConfigTooMuchPowerRequired       =   0x00001000,
    DeviceHwVerifierSetU1EnableFailure                  =   0x00002000,
    DeviceHwVerifierSetU2EnableFailure                  =   0x00004000

} DEVICE_HW_VERIFIER_FLAGS;

typedef enum _HUB_HW_VERIFIER_FLAGS {

    HubHwVerifierTooManyResets                          =   0x00000001,
    HubHwVerifierControlTransferFailure                 =   0x00000002,
    HubHwVerifierInterruptTransferFailure               =   0x00000004,
    HubHwVerifierNoSelectiveSuspendSupport              =   0x00000008,
    HubHwVerifierInvalidPortStatus                      =   0x00000010,
    HubHwVerifierPortLinkStateSSInactive                =   0x00000020,
    HubHwVerifierPortResetTimeout                       =   0x00000040,
    //
    // Ucx operation Failure really belongs to the device
    // section but since the device id and version might
    // not be known at the time of the failure, it is put
    // in the hub section
    //
    HubHwVerifierControllerOperationFailure             =   0x00000080,
    HubHwVerifierPortLinkStateCompliance                =   0x00000100,
    HubHwVerifierPortOverCurrent                        =   0x00000200,
    HubHwVerifierPortDeviceDisconnected                 =   0x00000400,
    HubHwVerifierPortLinkStateErrorResetWatchdog        =   0x00000800,
    //
    // The two Enum flags below really belong in the device
    // section but since the device id and version might
    // not be known at the time of the failure, both are put
    // in the hub section
    //
    HubHwVerifierDeviceFailedEnumeration                =   0x00001000,
    HubHwVerifierDeviceEnumerationRetry                 =   0x00002000

} HUB_HW_VERIFIER_FLAGS;

VOID
HUBMISC_VerifierDbgBreak(
    __in
        PCSTR                       FlagName,
    __in
        PSM_CONTEXT                 SmContext
    );

#define USB_HW_VERIFIER_DEVICE_BREAK(_DeviceContext, _FlagName) \
    if (_DeviceContext->HardwareVerifierFlags & _FlagName ) { \
        HUBMISC_VerifierDbgBreak(#_FlagName, &_DeviceContext->DsmContext );  \
    }

#define USB_HW_VERIFIER_PORT_BREAK(_PortContext, _FlagName) \
    if (_PortContext->HubFdoContext->HardwareVerifierFlags & _FlagName ) { \
        HUBMISC_VerifierDbgBreak(#_FlagName, &_PortContext->PsmContext );  \
    }

#define USB_HW_VERIFIER_HUB_BREAK(_HubFdoContext, _FlagName) \
    if (_HubFdoContext->HardwareVerifierFlags & _FlagName ) { \
        HUBMISC_VerifierDbgBreak(#_FlagName, &_HubFdoContext->HsmContext );  \
    }

typedef enum _UCXENDPOINT_STATE {
    UcxEndpointStateInvalid = 0,
    UcxEndpointStateNotCreated,
    UcxEndpointStatePendingCreate,
    UcxEndpointStatePendingEnable,
    UcxEndpointStateEnabled,
    UcxEndpointStatePendingDisable,
    UcxEndpointStateDisabled,
    UcxEndpointStateDeleted
} UCXENDPOINT_STATE, *PUCXENDPOINT_STATE;

typedef union _ENDPOINT_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       ZeroBandwidth:1;
    };
} ENDPOINT_FLAGS, *PENDPOINT_FLAGS;

C_ASSERT(sizeof(ENDPOINT_FLAGS) == sizeof(ULONG));


typedef struct _PIPE_HANDLE {
    UCXENDPOINT                                             UcxEndpoint;
    UCXENDPOINT_STATE                                       UcxEndpointState;
    ENDPOINT_FLAGS                                          EndpointFlags;
    USBD_PIPE_HANDLE                                        UsbdPipeHandle;
    ULONG                                                   UsbdPipeFlags;
    PUSB_ENDPOINT_DESCRIPTOR                                EndpointDescriptor;
    ULONG                                                   EndpointDescriptorBufferLength;
    PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR           SSEndpointCompanionDescriptor;
    PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR SSPIsochEndpointCompanionDescriptor;
} PIPE_HANDLE, *PPIPE_HANDLE;

typedef union _INTERFACE_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       HasAlternateSettings:1;
        ULONG       NeedsSetInterface:1;
    };
} INTERFACE_FLAGS, *PINTERFACE_FLAGS;

C_ASSERT(sizeof(INTERFACE_FLAGS) == sizeof(ULONG));

typedef struct _INTERFACE_HANDLE {
    WDFMEMORY                       MemoryObject;
    LIST_ENTRY                      InterfaceLink;
    ULONG                           NumberOfEndpoints;
    INTERFACE_FLAGS                 InterfaceFlags;
    PUSB_INTERFACE_DESCRIPTOR       InterfaceDescriptor;
    PIPE_HANDLE                     PipeHandle[1];
} INTERFACE_HANDLE, *PINTERFACE_HANDLE;


typedef struct _DEVICE_CONFIGURATION {
    WDFMEMORY                       WdfMemory;
    ULONG                           NumberOfEndpoints;
    LIST_ENTRY                      InterfaceList;
    UCHAR                           ConfigurationDescriptor[1];
} DEVICE_CONFIGURATION, *PDEVICE_CONFIGURATION;


typedef union _PDO_CONFIG_FLAGS {
    ULONG   UxdEnabled:1;
} PDO_CONFIG_FLAGS, *PPDO_CONFIG_FLAGS;


typedef struct _HUB_INTERNAL_CONFIG_INFO {
    ULONG                       Version;
    ULONG                       Length;
    USB_HUB_CAP_FLAGS           HubFlags;
    USB_ID_STRING               HardwareIds;
    USB_ID_STRING               CompatibleIds;
    USB_ID_STRING               DeviceDescription;
    ULONG                       Reserved[18];
    PDO_CONFIG_FLAGS            Flags;
    USB_HUB_DEVICE_UXD_SETTINGS UxdSettings;
} HUB_INTERNAL_CONFIG_INFO, *PHUB_INTERNAL_CONFIG_INFO;

typedef struct _USB_SEL_INFO {
    UCHAR   U1SEL;
    UCHAR   U1PEL;
    USHORT  U2SEL;
    USHORT  U2PEL;
} USB_SEL_INFO, *PUSB_SEL_INFO;

C_ASSERT(sizeof(USB_SEL_INFO) == 6);

typedef struct _INTERFACE_ALTERNATE_SETTING {
    UCHAR   InterfaceNumber;
    UCHAR   AlternateSetting;
} INTERFACE_ALTERNATE_SETTING, *PINTERFACE_ALTERNATE_SETTING;

typedef struct _ALTERNATE_SETTING_FILTER {
    ULONG                           Count;
    INTERFACE_ALTERNATE_SETTING     FilterList[1];
} ALTERNATE_SETTING_FILTER, *PALTERNATE_SETTING_FILTER;


typedef enum _MAX_PORT_POWER {
    MaxPortPowerInvalid = 0,
    MaxPortPower100Milliamps = 100,
    MaxPortPower500Milliamps = 500
} MAX_PORT_POWER, *PMAX_PORT_POWER;

typedef union _MSOS20_ALT_ENUM_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   AltEnumCmdSent:1;
        ULONG   AltEnumCompleted:1;
        ULONG   AltEnumCmdCached:1;
    };
} MSOS20_ALT_ENUM_FLAGS, *PMSOS20_ALT_ENUM_FLAGS;

C_ASSERT(sizeof(MSOS20_ALT_ENUM_FLAGS) == sizeof(ULONG));

typedef struct _MSOS20_INFO {
    MSOS20_FLAGS                                        Flags;
    MSOS20_ALT_ENUM_FLAGS                               AltEnumFlags;
    UCHAR                                               AltEnumCmd;
    ULONG                                               BytesReturned;
    MSOS20_DESCRIPTOR_SET_INFO                          DescriptorSetInfo;
    PMSOS20_DESCRIPTOR_SET_HEADER                       DescriptorSet;
    PMSOS20_FEATURE_COMPATIBLE_ID_DESCRIPTOR            CompatibleIdDescriptor;
    PMSOS20_FEATURE_MINIMUM_RESUME_TIME_DESCRIPTOR      MinResumeTimeDescriptor;
    PMSOS20_FEATURE_MODEL_ID_DESCRIPTOR                 ModelIdDescriptor;
    PMSOS20_FEATURE_CCGP_DEVICE_DESCRIPTOR              CcgpDeviceDescriptor;
    USB_DEVICE_DESCRIPTOR                               OriginalDeviceDescriptor;
    RECORDER_LOG                                        IfrLog;
} MSOS20_INFO, *PMSOS20_INFO;

typedef enum _USB_20_LPM_STATUS {
    Usb20LpmStatusUnknown,
    Usb20LpmEnabled,
    Usb20LpmDisabledSinceDeviceIsHub,
    Usb20LpmDisabledDueToDeviceHackFlag,
    Usb20LpmDisabledDueToHubHackFlag,
    Usb20LpmDisabledDueToHubGlobalFlag,
    Usb20LpmNotSupportedByDevice,
    Usb20LpmDisabledSinceBESLandAltHIRDNotSupported,
    Usb20LpmNotSupportedOnThisPort,
    Usb20LpmDiabledSinceDeviceOperatingInSuperspeedModeOrHigher,
} USB_20_LPM_STATUS, *PUSB_20_LPM_STATUS;

#define INVALID_ALTERNATE_MODE  0xFF

typedef struct _BILLBOARD_INFO {
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR         BillboardDescriptor;
    UCHAR                                               CurrentAlternateMode;
    //
    // This field tracks whether there is at least one alternate mode that
    // has failed
    //
    BOOLEAN                                             AlternateModeError;
    //
    // This field tracks whether there is at least one alternate mode that
    // has succeeded
    //
    BOOLEAN                                             AlternateModeSuccess;
} BILLBOARD_INFO, *PBILLBOARD_INFO;

typedef enum _CLIENT_INITIATED_RECOVERY_REASON {
    RecoveryReasonResetPipeAndClearStall = 0,
    RecoveryReasonResetPipe,
    RecoveryReasonClearStall,
    RecoveryReasonResetPort,
    RecoveryReasonCyclePort,
    RecoveryReasonMax
} CLIENT_INITIATED_RECOVERY_REASON, *PCLIENT_INITIATED_RECOVERY_REASON;

VOID
EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
    _In_ PDEVICE_CONTEXT        DeviceContext,
    _In_ const unsigned int     IoctlCode,
    _In_ const unsigned int     UrbFunction
    );

VOID
EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Cleanup(
    _In_ PDEVICE_CONTEXT        DeviceContext
    );

typedef struct _DEVICE_CONTEXT {
    PHUB_FDO_CONTEXT                HubFdoContext;
    PPORT_CONTEXT                   PortContext;
    PHUB_PDO_CONTEXT                PdoContext;
    UCXUSBDEVICE                    UsbDevice;
    ULONG                           EnumRetryCount;
    KSPIN_LOCK                      ConfigurationLock;
    PDEVICE_CONFIGURATION           CurrentDeviceConfiguration;
    PDEVICE_CONFIGURATION           OldDeviceConfiguration;
    PINTERFACE_HANDLE               NextInterfaceToSet;
    PINTERFACE_HANDLE               OldInterface;
    PINTERFACE_HANDLE               NewInterface;
    UCXENDPOINT                     DefaultEndpoint;
    ULONG                           UcxEndpointArrayCount;
    UCXENDPOINT                     *EndpointsToBeEnabled;
    ULONG                           EndpointsToBeEnabledCount;
    UCXENDPOINT                     *EndpointsToBeDisabled;
    ULONG                           EndpointsToBeDisabledCount;
    UCXENDPOINT                     *EndpointsToRemainUnchanged;
    ULONG                           EndpointsToRemainUnchangedCount;
    ULONG                           MaxPacketSize;
    USBDEVICE_INFO                  UsbDeviceInfo;
    DSM_MUX_CONTEXT                 MuxContext;
    CONTROL_REQUEST                 ControlRequest;
    WDFREQUEST                      UcxRequest;
    //
    // Irp is only used in debugger extension
    //
    PIRP                            UcxRequestIrp;
    WDFMEMORY                       UcxIoctlRequestMemory;
    ULONG                           UcxIoctl;
    WDFTIMER                        BootResetTimer;
    WDFREQUEST                      CurrentClientRequest;
    WDFREQUEST                      CurrentFdoRequest;
    KEVENT                          QueryDeviceTextEvent;
    WDFREQUEST                      PreformattedGetDescriptorRequest;
    SM_CONTEXT                      DsmContext;
    NTSTATUS                        LastOperationNtStatus;
    USBD_STATUS                     LastOperationUsbdStatus;
    PWDFDEVICE_INIT                 PdoDeviceInit;
    NTSTATUS                        PnPPowerStatus;
    KEVENT                          PnPPowerEvent;
    KEVENT                          PreStartEvent;
    USB_DEVICE_PROPERTIES           DeviceFlags;
    USB_DEVICE_STATE_FLAGS          DeviceStateFlags;
    USB_DEVICE_SQM_PROPERTIES       DeviceSqmFlags;
    USB_DEVICE_HACKS                DeviceHackFlags;
    ULONG                           DeviceAddress;
    PPIPE_HANDLE                    CurrentTargetPipe;
    USB_HUB_DEVICE_UXD_SETTINGS     UxdSettings;
    UCHAR                           ScratchBuffer[256];
    USB_DEVICE_DESCRIPTOR           DeviceDescriptor;
    PUSB_INTERFACE_DESCRIPTOR       CompatIdInterfaceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor;
    PUSB_STRING_DESCRIPTOR          LanguageIdDescriptor;
    PUSB_STRING_DESCRIPTOR          ProductIdStringDescriptor;
    USHORT                          CurrentLanguageId;
    USB_DEVICE_QUALIFIER_DESCRIPTOR DeviceQualifierDescriptor;
    UCHAR                           MsOsVendorCode;
    PUSB_BOS_DESCRIPTOR             BOSDescriptor;
    GUID                            ContainerId;
    OS_STRING                       MsOsDescriptor;
    PMS_EXT_CONFIG_DESC             MsExtConfigDesc;
    PMS_EXT_PROP_DESC               MsExtPropertyDesc;

    UNICODE_STRING                  UxdId;

    WDFSTRING                       SymbolicLinkName;
    USB_ID_STRING                   SerialNumberId;
    USB_ID_STRING                   FriendlyNameIdString;

    GUID                            WdfActivityId;

    //
    // LPM related stuff
    //
    USHORT                          U1ExitLatency;
    USHORT                          U2ExitLatency;
    USB_SEL_INFO                    UsbSelInfo;
    USHORT                          HostInitiatedU1ExitLatency;
    USHORT                          HostInitiatedU2ExitLatency;
    UCHAR                           LatencyAdjustmentPercentForNoPingError;
    USHORT                          EffectiveExitLatency;
    USHORT                          TargetEffectiveExitLatency;
    UCHAR                           U1Timeout;
    UCHAR                           U2Timeout;
    UCHAR                           TargetU1Timeout;
    UCHAR                           TargetU2Timeout;
    DEVICE_LPM_STATE_FLAGS          LPMStateFlags;
    LPM_POLICY                      LpmPolicy;
    USB_20_LPM_STATUS               Usb20LpmStatus;

    PVOID                           BootDeviceHandle;
    LONG                            BootDeviceReportedMissing;
    CONTROL_REQUEST                 ControlRequestForBootDevice;
    WDFTIMER                        OutOfBandwidthTimer;
    LARGE_INTEGER                   LastResetTimeStamp;
    ULONG                           EnumMsgId;
    ULONG                           HardwareVerifierFlags;
    DEVICE_20_LPM_ATTRIBUTES        Lpm20Attributes;
    PALTERNATE_SETTING_FILTER       AlternateSettingFilter;
    UCHAR                           DuplicateSerialNumberRetryCount;
    //
    // The only reason we need to track the WDM power state
    // is so that we can do special handling for the the case
    // where the device goes from D2 to D3
    //
    DEVICE_POWER_STATE              DevicePowerState;

    MSOS20_INFO                     MsOs20Info;

    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  SublinkSpeedAttr;
    ULONG                           SublinkSpeedAttrCount;
    USHORT                          RxTpDelay;
    USHORT                          TxTpDelay;

    ULONG                           MaxExitLatencyBasedOnEld;

    RTL_BITMAP                      DescriptorValidationBitmapHeader;
    ULONG                           DescriptorValidationBitmap[DESCRIPTOR_VALIDATION_ARRAY_COUNT];
    PBILLBOARD_INFO                 BillboardInfo;

    USB_DUAL_ROLE_FEATURES          UsbDualRoleLocalFeatures;
    USB_DUAL_ROLE_FEATURES          UsbDualRolePartnerFeatures;
    UCHAR                           DualRoleVendorCommand;

    LONG                            ClientInitiatedRecoveryEvents[RecoveryReasonMax]; // index to CLIENT_INITIATED_RECOVERY_REASON

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

typedef enum _ROOT_CONNECTOR_TYPE {
    RootConnectorTypeInvalid = 0,
    RootConnectorTypeNonAcpi = 1,
    RootConnectorTypeAcpi    = 2
} ROOT_CONNECTOR_TYPE, *PROOT_CONNECTOR_TYPE;

typedef struct _CONNECTOR_ID {
    ROOT_CONNECTOR_TYPE     Type;
    union {
        struct _ACPI_PLD_BUFFER     AcpiPldBuffer;
        ULONG                       HubNumber;
    };
    USHORT                  Depth;
    ULONG                   ConnectorPath[6];
    ULONG64                 UcmConnectorId;
} CONNECTOR_ID, *PCONNECTOR_ID;

typedef union _USB_PORT_FLAGS {
    ULONG   AsUlong32;
    struct {

        ULONG       DeviceConnected:1;
        ULONG       ConnectedDeviceSupportsReAttach:1;
        ULONG       CanQueueOverCurrentReset:1;
        ULONG       SSInactiveForBootDevice:1;
        ULONG       PendingSQMUpdateForSurpriseRemoval:1;
        ULONG       PendingPowerReferenceOnHub:1;
    };
} USB_PORT_FLAGS, *PUSB_PORT_FLAGS;

C_ASSERT(sizeof(USB_PORT_FLAGS) == sizeof(ULONG));

typedef struct _PORT_CONTEXT {
    PHUB_FDO_CONTEXT                HubFdoContext;
    PSM_EVENT                       CurrentPortEvent;
    USB_PORT_FEATURE_SELECTOR       CurrentFeatureSelector;
    CONTROL_REQUEST                 PortControlRequest;
    USB_PORT_EXT_STATUS_AND_CHANGE  CurrentExtPortStatusChange;
    USB_PORT_STATUS                 PreviousPortStatus;
    USB_PORT_CHANGE                 CumulativePortChange;
    PORT_INFORMATION                PortInformation;
    WDFTIMER                        ResetPollingTimer;
    PSM_MUX_CONTEXT                 MuxContext;
    SM_CONTEXT                      PsmContext;
    PDEVICE_CONTEXT                 DeviceContext;
    USB_PORT_FLAGS                  PortFlags;
    ACPI_UPC_PACKAGE                AcpiUpcPackage;
    ACPI_PLD_BUFFER                 AcpiPldPackage;
    CONNECTOR_ID                    ConnectorId;
    USB_CONNECTION_STATUS           ConnectionStatus;
    RECORDER_LOG                    IfrLog;
    ULONG                           D3ColdReconnectTimeout;
    ULONG                           TotalNumResets;
    LARGE_INTEGER                   LastOvercurrentTimeStamp;
    UCHAR                           OverCurrentCount;
    BOOLEAN                         PowerRefAcquiredForInitialConnectChange;
} PORT_CONTEXT, *PPORT_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PORT_CONTEXT, GetPortContext)

typedef union _CONTROLLER_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   SuperSpeedStreamsSupported:1;
    };
} CONTROLLER_FLAGS, *PCONTROLLER_FLAGS;

C_ASSERT(sizeof(CONTROLLER_FLAGS) == sizeof(ULONG));

typedef union _HUB_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   IsTtHub:1;
        ULONG   IsMultiTtHub:1;
        ULONG   PerPortPower:1;
        ULONG   PortPortOverCurrent:1;
        ULONG   NoOverCurrentProtection:1;
        ULONG   Initialized:1;
        ULONG   Configured:1;
        ULONG   SelectiveSuspendNotSupportedByParentStack:1;
        ULONG   WakeOnConnect:1;
        ULONG   ArmedForWake:1;
        ULONG   WaitWakeQueued:1;
        ULONG   ResetTTOnCancel:1;
        ULONG   NoClearTTBufferOnCancel:1;
        ULONG   PowerOnPortsOnStart:1;
        ULONG   WdfPowerReferencePending:1;
        ULONG   DisableLpm:1;
        ULONG   DisableUsb20HardwareLpm:1;
        ULONG   CanAcquirePowerReference:1;
        ULONG   InAcpiNamespace:1;
        ULONG   AddDelayAfterResetComplete:1;
        ULONG   IgnoreU0InDisconnected:1;
        ULONG   IgnoreEnabledInSSInactive:1;
        ULONG   IsInBootOrPagingPath:1;
        ULONG   DisableSuperSpeed:1;
        ULONG   IgnoreEnabledInReset:1;
        ULONG   DisableSelectiveSuspendForIntegratedHub:1;
        ULONG   S0IdleConfigured:1;
        ULONG   PowerReferenceAcquiredDueToGlobalSetting:1;
        ULONG   DisallowU2AcceptOnlyWithU1AcceptAndInitiate:1;
        ULONG   OvercurrentDetected:1;
        ULONG   DisableOnSoftRemove:1;
        ULONG   ConvertPollingToComplianceOnStart:1;
    };
} HUB_FLAGS, *PHUB_FLAGS;

C_ASSERT(sizeof(HUB_FLAGS) == sizeof(ULONG));

typedef enum _ENUM_FAILURE_PID {
    EnumFailurePid_UnknownDevice            = 0x0000,
    EnumFailurePid_ResetFailure             = 0x0001,
    EnumFailurePid_DeviceDescriptorFailure  = 0x0002,
    EnumFailurePid_ConfigDescriptorFailure  = 0x0003,
    EnumFailurePid_SetAddressFailure        = 0x0004,
    EnumFailurePid_BadDeviceDesc            = 0x0005,
    EnumFailurePid_BadConfigDesc            = 0x0006,
    EnumFailurePid_PortInSSInactive         = 0x0007,
    EnumFailurePid_PortInComplianceMode     = 0x0008
} ENUM_FAILURE_PID;

typedef enum _UXD_EVENT {
    UxdEventInvalid = 0,
    UxdEventDisable,
    UxdEventDisconnect
} UXD_EVENT, *PUXD_EVENT;

typedef struct _HUB_INFORMATION {
    HUB_FLAGS                       HubFlags;
    USHORT                          NumberOfPorts;
    USHORT                          HubAddress;
    //
    // HubResetCount is a temperorily value used
    // by HSM to keep track of the number of times
    // hub has already been reset in a given recovery
    // instance and also to keep track of the number
    // of times hub's interrupt pipe has been reset
    // on interrupt transfer failures. Note that HSM
    // only attempts one kind of recovery at a time,
    // so there is no conflict
    //
    USHORT                          HubResetCount;
    //
    // HubTimedResetRecoveryCount is a value that keeps
    // track of how many times this hub instance
    // has invoked reset recovery during the last
    // time frame
    //
    USHORT                          HubTimedResetRecoveryCount;
    //
    // HubResetRecoveryCount is a value that keeps
    // track of how many times this hub has invoked
    // reset recovery during the life of the devnode
    //
    ULONG                           HubResetRecoveryCount;
    LARGE_INTEGER                   FirstHubResetRecoveryTimeStamp;
    USB_PORT_STATUS_AND_CHANGE      LastHubResetPortStatus;
    USHORT                          HubGetDescriptorCount;
    USBD_CONFIGURATION_HANDLE       HubConfigurationHandle;
    USHORT                          InterruptPipeMaxPacketSize;
    MAX_PORT_POWER                  MaxPortPower;
    ULONG                           HubNumber;
    HUBUCX_ROOTHUB_INFO             RootHubInfo;
    PROOTHUB_20PORT_INFO            *RootHub20PortInfoPointerArray;
    PROOTHUB_30PORT_INFO_EX         *RootHub30PortInfoPointerArray;
    USHORT                          NumberOf20Ports;
    USHORT                          First20PortNumber;
    USHORT                          Last20PortNumber;
    USHORT                          NumberOf30Ports;
    USHORT                          First30PortNumber;
    USHORT                          Last30PortNumber;
    ULONG                           SuperSpeedPlusIsochBurstCount;
} HUB_INFORMATION, *PHUB_INFORMATION;


typedef union _HSM_MUX_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       InterruptReferenceReleasePending:1;
        ULONG       FailInterruptReferenceAcquired:1;
        ULONG       HubResetEnabled:1;
        ULONG       FailDsmRegistration:1;
        ULONG       HubResetInProgress:1;
    };
} HSM_MUX_FLAGS, *PHSM_MUX_FLAGS;

C_ASSERT(sizeof(HSM_MUX_FLAGS) == sizeof(ULONG));

typedef struct _HSM_MUX_CONTEXT {
    KSPIN_LOCK                      MuxLock;
    ULONG                           OutstandingPortChanges;
    ULONG                           OutstandingPnPOperations;
    ULONG                           OutstandingResetOperations;
    ULONG                           NumberOfInterruptReferencesHeld;
    ULONG                           NumberOfPowerReferencesHeld;
    ULONG                           NumberOfDevicePowerReferencesHeld;
    HSM_MUX_FLAGS                   HubMuxFlags;
    ULONG                           NumberOfPSMs;
    LIST_ENTRY                      PSMListHead;
    ULONG                           NumberOfDSMs;
    LIST_ENTRY                      DSMListHead;

} HSM_MUX_CONTEXT, *PHSM_MUX_CONTEXT;

typedef struct _USER_MODE_IOCTL_QUEUE_CONTEXT {

    CONTROL_REQUEST                 ControlRequest;

} USER_MODE_IOCTL_QUEUE_CONTEXT, *PUSER_MODE_IOCTL_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USER_MODE_IOCTL_QUEUE_CONTEXT, GetUserModeIoctlContext)


typedef struct _HUB_FDO_CONTEXT {

    PUSBHUB3_TRIAGE_INFO            Usbhub3TriageInfo;

    ULONG                           Reserved;

    BOOLEAN                         QueryPortStatusForDebugging;

    WDFDEVICE                       HubFdoDevice;
    PDEVICE_OBJECT                  HubPdo;
    WDFIOTARGET                     RootHubIoTarget;

    HUB_INFORMATION                 HubInformation;

    UCX_CONTROLLER_INFO             ControllerInfo;

    HUB_PARENT_INTERFACE            HubParentInterface;
    HUB_CONTROLLERSTACK_INTERFACE   HubControllerStackInterface;
    USB_BUS_INTERFACE_USBDI_V3      UsbdBusInterface;

    DEVICE_CAPABILITIES             DeviceCapabilities;

    CONTROLLER_FLAGS                ControllerFlags;

    //
    // Cached Hub Status and Change bitmask as returned by GetHubStatus
    //
    USB_HUB_STATUS_AND_CHANGE       HubStatusAndChange;

    USB_HUB_STATUS_AND_CHANGE       OldHubStatusAndChange;

    USB_HUB_FEATURE_SELECTOR        CurrentFeatureSelector;

    HSM_EVENT                       CurrentHubChangeEvent;

    //
    // Cached Hub Device Status
    //
    USB_DEVICE_STATUS               HubDeviceStatus;

    //
    // Pre-allocated WDF request used for hub-wide control transfers,
    // such as GetHubDescriptor, GetHubStatus, and AckHubStatusChange
    // Also used for sending reset pipe requests for error recovery
    //
    CONTROL_REQUEST                 HubControlRequest;

    //
    // Pre-allocated WDF request used for the Status Change interrupt endpoint
    //
    HUB_INTERRUPT_REQUEST           StatusChangeInterruptRequest;

    //
    // HSM PnP/Power event & status
    //
    KEVENT                          PnPPowerEvent;
    NTSTATUS                        PnPPowerStatus;

    //
    // Union for both USB 2.0 and USB 3.0 Hub Descriptor
    //
    HUB_DESCRIPTOR                  HubDescriptor;

    //
    // Pointer to the Hub's Configuration Descriptor Set
    //
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor;

    //
    // Hub State Machine
    //
    SM_CONTEXT                      HsmContext;

    //
    // Context for the HSM mux
    //
    HSM_MUX_CONTEXT                 MuxContext;

    //
    // This is a queue to receive IOCTLs from user-mode to the FDO
    //
    WDFQUEUE                        DeviceControlQueue;

    //
    // Flags filled in by the parent when hub reset is completed
    //
    USB_RESET_FLAGS                 ResetFlags;

    GUID                            CurrentActivityId;

    LIST_ENTRY                      ListEntry;

    HUB_INFO_FROM_PARENT            HubInfoFromParent;

    RECORDER_LOG                    IfrLog;

    WDFREQUEST                      PdoGetPortStatusRequest;

    WDFWMIINSTANCE                  WmiNotificationHandle;

    UNICODE_STRING                  HubSymbolicLinkNameUnicode;

    PVOID                           SelectiveSuspendSetting;

    PVOID                           SelectiveSuspendTimeoutPowerSetting;

    ULONG                           CurrentIdleTimeout;

    PPORT_CONTEXT                   GetPortStatusTargetPortContext;

    ULONG                           HardwareVerifierFlags;

    ULONG                           HubFailureMsgId;

    WDF_POWER_DEVICE_STATE          CurrentWdfPowerDeviceState;

    USHORT                          OldwTotalLength;

} HUB_FDO_CONTEXT, *PHUB_FDO_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(HUB_FDO_CONTEXT, GetHubFdoContext)



//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//
#define FILE_DEVICE_USB_HUB  0x00008600

EVT_WDF_DRIVER_DEVICE_ADD                   HUBFDO_EvtDeviceAdd;
EVT_WDF_DEVICE_PREPARE_HARDWARE             HUBFDO_EvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE             HUBFDO_EvtDeviceReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY                     HUBFDO_EvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT                      HUBFDO_EvtDeviceD0Exit;
EVT_WDF_OBJECT_CONTEXT_CLEANUP              HUBFDO_EvtDeviceCleanupCallback;
EVT_WDF_OBJECT_CONTEXT_DESTROY              HUBFDO_EvtDeviceDestroyCallback;
EVT_WDF_DEVICE_USAGE_NOTIFICATION           HUBFDO_EvtDeviceUsageNotification;
EVT_WDF_DEVICE_SURPRISE_REMOVAL             HUBFDO_EvtDeviceSurpriseRemoval;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS            HUBFDO_EvtDeviceShutdownPreprocess;
EVT_WDF_OBJECT_CONTEXT_CLEANUP              HUBPDO_EvtDeviceCleanup;
EVT_WDF_OBJECT_CONTEXT_DESTROY              HUBPDO_EvtDeviceDestroy;
EVT_WDF_OBJECT_CONTEXT_CLEANUP              HUBMISC_EvtPsmCleanup;
EVT_WDF_OBJECT_CONTEXT_DESTROY              HUBMISC_EvtPsmDestroy;
EVT_WDF_OBJECT_CONTEXT_DESTROY              HUBMISC_EvtDsmDestroy;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL HUBFDO_EvtIoInternalDeviceControlFromPDO;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL          HUBFDO_EvtIoDeviceControl;
EVT_WDF_DEVICE_ARM_WAKE_FROM_S0             HUBFDO_EvtDeviceArmWakeFromS0;
EVT_WDF_DEVICE_DISARM_WAKE_FROM_S0          HUBFDO_EvtDeviceDisarmWakeFromS0;
EVT_WDF_DEVICE_ARM_WAKE_FROM_SX             HUBFDO_EvtDeviceArmWakeFromSx;
EVT_WDF_DEVICE_DISARM_WAKE_FROM_SX          HUBFDO_EvtDeviceDisarmWakeFromSx;
EXT_CALLBACK                                HUBMISC_HubEventTimer;
EXT_CALLBACK                                HUBMISC_PsmEventTimer;
EXT_CALLBACK                                HUBMISC_DsmEventTimer;

#define TEST_FLAG(var, flag)    (((var) & (flag)) ? TRUE : FALSE)
#define CLEAR_FLAG(var, flag)   InterlockedAnd(&(var), ~(flag))
#define SET_FLAG(var, flag)     InterlockedOr(&(var), (flag));

#define TEST_BIT(value, bitNumber)  ((value) & (1<<(bitNumber))) ? TRUE : FALSE
#define SET_BIT(value, bitNumber)   ((value) |= (1<<(bitNumber)))
#define CLEAR_BIT(value, bitNumber) ((value) &= ~(1<<(bitNumber)))


VOID
HUBFDO_BuildUsb20HubDescriptor(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __out
        PUSB_HUB_DESCRIPTOR HubDescriptor
    );

FORCEINLINE
PPORT_CONTEXT
HUBFDO_FindPortContextFromNumber(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber
    );

FORCEINLINE
WDFDEVICE
HUBFDO_FindDeviceForPort(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber
    );

__drv_functionClass(POWER_SETTING_CALLBACK)
__drv_sameIRQL
NTSTATUS
HUBFDO_PowerSettingCallback (
    _In_
        LPCGUID             SettingGuid,
    _In_reads_bytes_(ValueLength)
        PULONG              NewValue,
    _In_
        ULONG               ValueLength,
    _Inout_opt_
        PHUB_FDO_CONTEXT    HubFdoContext
   );


NTSTATUS
HUBFDO_IoctlValidateParameters(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        size_t              MinInputLength,
    __in
        size_t              InputBufferLength,
    __in_opt
        PVOID               InputBuffer,
    __in
        size_t              MinOutputLength,
    __in
        size_t              OutputBufferLength
    );


VOID
HUBFDO_IoctlGetPortStatus(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetNodeInformation(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetNodeConnectionDriverKeyName(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetNodeConnectionInfo(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength,
    __in
        BOOLEAN         extendedApi
    );


VOID
HUBFDO_IoctlGetNodeConnectionAttributes(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetHubCaps(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetHubCapsEx(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetNodeConnectionName(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetDescriptorFromNodeConnection(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetHubInformationEx(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetPortConnectorProperties(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlGetNodeConnectionInformationExV2
(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlCyclePort(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    );


VOID
HUBFDO_IoctlResetHub(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request
    );


VOID
HUBFDO_EvtIoDeviceControl(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength,
    __in
        ULONG           IoControlCode
    );


VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
HUBFDO_SetFriendlyNameForBlockedHub (
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext
    );

VOID
HUBHTX_CompleteGetPortStatusWithFailedStatus (
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        NTSTATUS            Status
    );

VOID
HUBFDO_CompleteGetDescriptorRequest(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber,
    __in
        WDFREQUEST          Request,
    __in
        NTSTATUS            Status,
    __in
        ULONG               DescriptorSize
    );


VOID
HUBDTX_CacheBillboardInfo(
    __in
        PDEVICE_CONTEXT             DeviceContext,
    __in
        PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR
                                    BillboardDescriptor
    );
