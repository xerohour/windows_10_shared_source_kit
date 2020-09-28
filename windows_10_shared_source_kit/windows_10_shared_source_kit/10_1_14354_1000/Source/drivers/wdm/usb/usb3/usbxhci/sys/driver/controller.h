/*++
Copyright (c) Microsoft Corporation

Module Name:

    controller.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#include "triage-usbxhci.h"
//
// Public definitions.
//
#define DriverTag_Wdf                               'WCHX'
#define DriverAllocationTag                         'ICHX'

//
// NOTE: Since the type of field Flags can be different, thus
// not making the following macro's as inline functions
//
#define SET_FLAG(Flags, Bit)    ((Flags) |= (Bit))
#define CLEAR_FLAG(Flags, Bit)  ((Flags) &= ~(Bit))
#define TEST_FLAG(Flags, Bit)   ((Flags) & (Bit))

//
// These values are used for the controller watchdog timer.
//
#define CONTROLLER_WATCHDOG_TIMER_INTERVAL_MS           5000
#define CONTROLLER_WATCHDOG_TIMER_TOLERABLE_DELAY_MS    1000

//
// Following values are used when waiting for the HcHalted bit to clear
// after setting the RunStop bit during start controller.
//

// Primary wait = 5 ms, poll every 100 micro seconds
#define CONTROLLER_PRIMARY_POLLING_FREQUENCY_IN_uS      100
#define CONTROLLER_PRIMARY_POLLING_COUNT_LIMIT          50

// Secondary wait = 35 ms, poll every 5 milli secods
#define CONTROLLER_SECONDARY_POLLING_FREQUENCY_IN_MS    5
#define CONTROLLER_SECONDARY_POLLING_COUNT_LIMIT        7

//
// This is the interval within which only CONTROLLER_MAX_RESET_RECOVERY_PER_INTERVAL
// number of reset recoveries are allowed. It should be a multiple of the watchdog
// time interval and preferably in minutes.
//
#define CONTROLLER_RESET_RECOVERY_INTERVAL_MS           (5 * 60 * 1000)
#define CONTROLLER_MAX_RESET_RECOVERY_PER_INTERVAL      10

//
// This is the controller reset recovery interval in watchdog timer units instead
// of miliseconds.
//
#define CONTROLLER_RESET_RECOVERY_INTERVAL_TIMER    \
    (CONTROLLER_RESET_RECOVERY_INTERVAL_MS /        \
     CONTROLLER_WATCHDOG_TIMER_INTERVAL_MS)

//
// The ideal timeout is a value of 0. But Framework takes 0 to mean use whatever value
// is the default. So pick the next smallest value - 1 ms.
//
#define CONTROLLER_IDLE_TIMEOUT_SHORT_MS                1

#define CONTROLLER_IDLE_TIMEOUT_LONG_MS                 5000

#define XHCI_MAJOR_VERSION_1                            0x01
#define XHCI_MINOR_VERSION_0                            0x00
#define XHCI_MINOR_VERSION_1                            0x10

#define PCI_VENDOR_ID_ATI_AMD                           0x1002
#define PCI_VENDOR_ID_AMD                               0x1022
#define PCI_VENDOR_ID_NEC_RENESAS                       0x1033
#define PCI_VENDOR_ID_TI                                0x104C
#define PCI_VENDOR_ID_NVIDIA                            0x10DE
#define PCI_VENDOR_ID_VIA                               0x1106
#define PCI_VENDOR_ID_RENESAS                           0x1912
#define PCI_VENDOR_ID_ASMEDIA                           0x1B21
#define PCI_VENDOR_ID_ETRON                             0x1B6F
#define PCI_VENDOR_ID_FRESCO_LOGIC                      0x1B73
#define PCI_VENDOR_ID_INTEL                             0x8086















































#define HCCPARAMS2_CAPABILITY_LENGTH                    0x20

//
// Hardware Verifier Macros
// Hardware verifier is used to catch bad hardware behaviour in the debugger.
// This should be used for testing purposes
//
#define VID_PID_REV_BUFFER_SIZE                         26

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_HwVerifierKeyName,
L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\usb\\HardwareVerifier");

//
// Under the HardwareVerifier key, there can be a global key or
// a device specific key in the format VVVVDDDDRR or VVVVDDDD
//

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_HwVerifierGlobalName, L"global");

//
// Under that key, there is a key for xHCI version, one of the following
//

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_HwVerifierxHCI96,     L"xHCI96");

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_HwVerifierxHCI10,     L"xHCI10");

//
// In the version key, there can be DWORD values with the name 'controller'
// In this DWORD , each bit represents a particualr hardware behavior.
//

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_HwVerifierControllerName, L"controller");

//
// This GUID identifies the _DSM to execute on some platforms to enable RTD3.
//
DEFINE_GUID(GUID_DSM_ENABLE_RTD3,
0xAC340CB7, 0xE901, 0x45bf, 0xB7, 0xE6, 0x2B, 0x34, 0xEC, 0x93, 0x1E, 0x23);

//
// Controller hardware verifier flags
//
typedef enum _CONTROLLER_HW_VERIFIER_FLAGS {

    ControllerHwVerifierFirmwareVersionOutdated                     = 0x00000001,
    ControllerHwVerifierHostSystemError                             = 0x00000002,
    ControllerHwVerifierHostControllerError                         = 0x00000004,
    ControllerHwVerifierResetRecoveryStopTimeout                    = 0x00000008,
    ControllerHwVerifierResetRecoveryResetTimeout                   = 0x00000010,
    ControllerHwVerifierResetRecoveryStartTimeout                   = 0x00000020,
    ControllerHwVerifierRegisterReadReturnedAllFF                   = 0x00000040,
    ControllerHwVerifierSetDequeuePointerAfterEndpointResetFailed   = 0x00000080,
    ControllerHwVerifierEndpointResetFailed                         = 0x00000100,
    ControllerHwVerifierSetDequeuePointerAfterStopEndpointFailed    = 0x00000200,
    ControllerHwVerifierEndpointStopFailedContextStateError         = 0x00000400,
    ControllerHwVerifierUnrecognizedTransferEventTrbCompletionCode  = 0x00000800,
    ControllerHwVerifierCommandAbortTimeoutCommandRingNotStopped    = 0x00001000,
    ControllerHwVerifierCommandAbortTimeoutCommandRingStopped       = 0x00002000,
    ControllerHwVerifierCommandTimeout                              = 0x00004000,
    ControllerHwVerifierDequeuePointerMismatchAfterCommandAbort     = 0x00008000,
    ControllerHwVerifierOutOfOrderCommandCompletion                 = 0x00010000,
    ControllerHwVerifierEnableSlotReturnedBadSlotId                 = 0x00020000,
    ControllerHwVerifierSetAddressBSR1Failed                        = 0x00040000,
    ControllerHwVerifierEndpointsDeconfigureFailed                  = 0x00080000,
    ControllerHwVerifierDisableSlotCommandFailed                    = 0x00100000,
    ControllerHwVerifierResetDeviceCommandFailed                    = 0x00200000,
    ControllerHwVerifierControllerSaveRestoreFailed                 = 0x00400000,
    ControllerHwVerifierStopEndpointFailed                          = 0x00800000,
    ControllerHwVerifierDuplicateFSEReceived                        = 0x01000000,
    ControllerHwVerifierDuplicateHaltedCompletionReceived           = 0x02000000,
    ControllerHwVerifierMax                                         = 0x04000000

} CONTROLLER_HW_VERIFIER_FLAGS;

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_TestConfigKeyName,
                                    L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\usb\\TestConfig\\XHCI");

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_TestConfigGlobalKeyName,
                                    L"Global");

DECLARE_GLOBAL_CONST_UNICODE_STRING(G_TestConfigCommonBufferDisableCachingKeyValue,
                                    L"CommonBufferDisableCaching");

DECLARE_GLOBAL_CONST_UNICODE_STRING(G_TestConfigMaxDmaMapRegistersKeyValue,
                                    L"MaxDmaMapRegisters");

DECLARE_GLOBAL_CONST_UNICODE_STRING(G_TestConfigEventRingSegmentCount,
                                    L"EventRingSegmentCount");

DECLARE_GLOBAL_CONST_UNICODE_STRING(G_TestConfigEventRingSegmentSize,
                                    L"EventRingSegmentSize");

typedef struct _TEST_CONFIGURATION {
    ULONG CommonBufferDisableCaching;
    ULONG MaxDmaMapRegisters;
    ULONG EventRingSegmentCount;
    ULONG EventRingSegmentSize;
} TEST_CONFIGURATION, *PTEST_CONFIGURATION;

//
// These Macro's is used to add memory blocks to live memory dumps
//
#define TELEMETRY_INSERT_BLOCK(HANDLE, DATA, SIZE)                          \
    do {                                                                    \
        if (DATA != NULL) {                                                 \
            if (!NT_SUCCESS(TelemetryData_InsertTriageDataBlock(HANDLE,     \
                                                                DATA,       \
                                                                SIZE))) {   \
                LEAVE;                                                      \
            }                                                               \
        }                                                                   \
    } WHILE(0);

// {209A9224-6EA2-47F0-890D-03489208EC52}
DEFINE_GUID(USBXHCI_TELEMETRY_SECONDARY_DATA_GUID,
0x209a9224, 0x6ea2, 0x47f0, 0x89, 0xd, 0x3, 0x48, 0x92, 0x8, 0xec, 0x52);

typedef struct _PCI_VID_TABLE_ENTRY {
    USHORT  VendorId;
    PWCHAR  VendorString;
} PCI_VID_TABLE_ENTRY, *PPCI_VID_TABLE_ENTRY;

typedef struct _ACPI_VID_TABLE_ENTRY {
    PCHAR   VendorIdShort;
    PWCHAR  VendorIdLong;
} ACPI_VID_TABLE_ENTRY, *PACPI_VID_TABLE_ENTRY;

#define DEVICE_REGISTRY_DELAY_AFTER_MF_DISABLE              L"DelayAfterMFDisable"
#define DEVICE_REGISTRY_MAX_NUMBER_OF_STREAMS               L"MaxNumberOfStreams"
#define DEVICE_REGISTRY_KEY_USE_STRICT_BIOS_HANDOFF         L"UseStrictBiosHandoff"
#define DEVICE_REGISTRY_KEY_HC_RESTORE_STATE_FAILURE_COUNT  L"HCRestoreStateFailureCount"
#define DEVICE_REGISTRY_KEY_HC_RECOVERY_COUNT               L"HCRecoveryCount"


































//
// These are defined to ease debugging (get type info in the debugger).
//
typedef struct _BUFFER_DATA *PBUFFER_DATA;
typedef struct _IOCONTROL_DATA *PIOCONTROL_DATA;
typedef struct _REGISTER_DATA *PREGISTER_DATA;
typedef struct _COMMON_BUFFER_DATA *PCOMMON_BUFFER_DATA;
typedef struct _PRIMARY_INTERRUPTER_DATA *PPRIMARYINTERRUPTER_DATA;
typedef struct _DEVICESLOT_DATA *PDEVICESLOT_DATA;
typedef struct _COMMAND_DATA *PCOMMAND_DATA;
typedef struct _ROOTHUB_DATA *PROOTHUB_DATA;
typedef struct _WMI_DATA *PWMI_DATA;

typedef enum _SYSTEM_POWER_ACTION {

    //
    // Power up.
    //
    SystemPowerAction_Boot                  = 0,
    SystemPowerAction_ResumeFromSleep       = 1,
    SystemPowerAction_ResumeFromHibernate   = 2,

    //
    // Power down.
    //
    SystemPowerAction_Sleep                 = 3,
    SystemPowerAction_HybridSleep           = 4,
    SystemPowerAction_Hibernate             = 5,
    SystemPowerAction_Shutdown              = 6,

    //
    // Regular system operation.
    //
    SystemPowerAction_None                  = 7

} SYSTEM_POWER_ACTION;

#define SystemPoweringUp(s)                                 \
    (((s) >= SystemPowerAction_Boot) &&                     \
     ((s) <= SystemPowerAction_ResumeFromHibernate))        \

#define SystemPoweringDown(s)                               \
    (((s) >= SystemPowerAction_Sleep) &&                    \
     ((s) <= SystemPowerAction_Shutdown))                   \

typedef enum _DEVICE_ENUMERATOR {
    DeviceEnumeratorPci     = 0,
    DeviceEnumeratorAcpi    = 1,
    DeviceEnumeratorUrs     = 2
} DEVICE_ENUMERATOR;

#define ACPI_VENDOR_ID_UNKNOWN "UKWN"
#define ACPI_DEVICE_ID_UNKNOWN "FFFF"

typedef struct _PCI_DEVICE_INFORMATION {

    UCX_CONTROLLER_PCI_INFORMATION;

    USHORT                              SubVendorID;
    USHORT                              SubSystemID;

    //
    // This is the context for the bus interface for PCI.
    //
    PVOID                               Context;

    //
    // This is the set bus data bus interface for PCI.
    //
    PGET_SET_DEVICE_DATA                SetBusData;

} PCI_DEVICE_INFORMATION, *PPCI_DEVICE_INFORMATION;

typedef struct _ACPI_DEVICE_INFORMATION {
    UCX_CONTROLLER_ACPI_INFORMATION;
} ACPI_DEVICE_INFORMATION, *PACPI_DEVICE_INFORMATION;

#pragma warning(push)
#pragma warning( disable: 4201 ) // nonstandard extension used : nameless struct/union
#pragma warning( disable: 4214 ) // Disable warning C4214: nonstandard extension used : bit field types other than int
typedef union _FRAME_SNAPSHOT {
    struct {
        // Bits 31:11 of the 32 bit generated frame fumber
        ULONGLONG                   HighPart:21;

        // Time in miliseconds
        // when the 32 bit frame number was:
        // <FrameNumberHighPart>00000000000
        // This is a time relative to when the system booted
        // Keeping this time relative ensures that 43 bits are good enough to store
        // several hunderd years of time without having to wrap around.
        ULONGLONG                   TimeWhenLowPartWasZero_ms:43;
    };

    LONGLONG                        Asll;
} FRAME_SNAPSHOT, *PFRAME_SNAPSHOT;
#pragma warning(pop)

typedef struct _FRAME_NUMBER_DATA {
    FRAME_SNAPSHOT                      Snapshot;

    BOOLEAN                             Initialized;

    //
    // A cached value of KeQueryTimeIncrement, that returns the number of 100 nano-seconds
    // passed per tick
    //
    ULONG                               TimeIncrementPerTick;

}  FRAME_NUMBER_DATA, *PFRAME_NUMBER_DATA;

typedef struct _CONTROLLER_COUNTER {

    ULONG   ResetCount;
    ULONG   RestoreStateFailureCount;

} CONTROLLER_COUNTER, *PCONTROLLER_COUNTER;

typedef struct _CONTROLLER_DATA {

    //
    // WDF and UCX objects.
    //
    WDFDEVICE                           WdfDevice;
    UCXCONTROLLER                       UcxController;

    //
    // Links this controller into the global list of all controllers.
    //
    LIST_ENTRY                          ControllerListEntry;

    //
    // List of all devices that exist for this controller.
    //
    KSPIN_LOCK                          DeviceListLock;
    LIST_ENTRY                          DeviceListHead;
    ULONG                               DeviceListCount;

    RECORDER_LOG                        RecorderLog;

    //
    // Handle to other objects within the driver.
    //
    PIOCONTROL_DATA                     IoControlHandle;
    PREGISTER_DATA                      RegisterHandle;
    PCOMMON_BUFFER_DATA                 CommonBufferHandle;
    PPRIMARYINTERRUPTER_DATA            InterrupterHandle;
    PDEVICESLOT_DATA                    DeviceSlotHandle;
    PCOMMAND_DATA                       CommandHandle;
    PROOTHUB_DATA                       RootHubHandle;
    PWMI_DATA                           WmiHandle;

    //
    // Handle to some optional component that filters commands
    // in order to provide workarounds for hardware specific
    // issues
    //
    HANDLE                              CommandFilterHandle;

    //
    // The instance number from the device instance id.
    //
    ULONG                               InstanceNumber;

    //
    // The parent's bus type, PCI or ACPI.
    //
    UCX_CONTROLLER_PARENT_BUS_TYPE      ParentBusType;
    PCI_DEVICE_INFORMATION              PciDeviceInfo;
    ACPI_DEVICE_INFORMATION             AcpiDeviceInfo;

    //
    // True if MSE length is usable
    //
    BOOLEAN                             MSELengthUsable;

    //
    // The xHCI version of the controller.
    //
    UCHAR                               MajorVersion;
    UCHAR                               MinorVersion;

    //
    // The firmware revision of the controller, if available.
    //
    ULONGLONG                           FirmwareVersion;

    //
    // Generic flags to modify default behavior. These flags are
    // populated from the Kse database.
    //
    KSE_XHCI_CONTROLLER_FLAGS           DeviceFlags;

    //
    // Internal (non-generic) flags to modify default behavior. These
    // flags are hardcoded in the driver code for specific controllers.
    //
    ULONG64                             InternalDeviceFlags;

    //
    // Timer used peridiocally verify xHCI controller is operational.
    //
    WDFTIMER                            WdfTimer;

    //
    // Work Item used to write out live kernel dump at PASSIVE_LEVEL.
    //
    WDFWORKITEM                         TelemetryReportWorkItem;

    ULONG64                             TelemetryHistory;

    //
    // True if controller is hosting a device that has forward progress enabled.
    //
    BOOLEAN                             ForwardProgressEnabled;

    //
    // Saves information about whether special files like paging, hibernate are
    // currently enabled.
    //
    ULONG                               SpecialFileStatus;

    //
    // Set on EvtDeviceD0Entry and EvtDeviceD0Exit.
    //
    WDF_POWER_DEVICE_STATE              CurrentWdfPowerDeviceState;

    BOOLEAN                             ControllerSaveStateFailed;

    BOOLEAN                             ControllerGone;

    ULONG                               ResetCount;
    ULONG                               ResetInterval;
    LONG                                ResetLock;

    SYSTEM_POWER_ACTION                 SystemPowerAction;

    //
    // To update the Crashdump context after Crashdump initialization.
    //
    HANDLE                              HibernateContext;

    FRAME_NUMBER_DATA                   FrameNumberData;

    ULONG64                             HardwareVerifierFlags;

    //
    // Used to pad split transfer buffers if the flag
    // RoundSplitTransferSizeToPacketSize is set.
    //
    PBUFFER_DATA                        ScratchTransferBufferData;

    //
    // Saves information about whether S0Idle is confgured
    // and if not, the reason for it not being configured.
    // This is only used for debugging purpsoses and for
    // logging it in rundown events
    //
    S0IdleStatus                        ControllerS0IdleStatus;

    //
    // Saves information about whether the controller has queued
    // a wait wake
    //
    BOOLEAN                             WaitWakeQueued;

    CONTROLLER_COUNTER                  Counter;

    TEST_CONFIGURATION                  TestConfiguration;

    WDFWORKITEM                         IdleTimeoutUpdateWorkItem;

    ULONG                               LastSetIdleTimeout;

    POHANDLE                            PoHandle;

#if defined(DBG) && !defined(_WIN64)
    LONG                                EventDataCounter;
#endif

    PVOID                               LPEEnterExitCallbackHandle;

    BOOLEAN                             WdfPowerReferenceForDebuggerAcquired;

} CONTROLLER_DATA, *PCONTROLLER_DATA;

#define MAX_SUFFIX_SIZE                         4*sizeof(WCHAR)
#define BASE_DEVICE_NAME                        L"\\Device\\USBFDO-"
#define BASE_SYMBOLIC_LINK_NAME                 L"\\DosDevices\\HCD"

#define DeviceNameSize                          sizeof(BASE_DEVICE_NAME)+MAX_SUFFIX_SIZE
#define SymlinkNameSize                         sizeof(BASE_SYMBOLIC_LINK_NAME)+MAX_SUFFIX_SIZE
#define DEVICE_DESC_SIZE                        255
#define MATCHING_ID_SIZE                        255

#define ENUMERATOR_NAME_SIZE                    8
#define ENUMERATOR_NAME_PCI                     L"PCI"
#define ENUMERATOR_NAME_ACPI                    L"ACPI"

#define RegistryStoreTypeDevice                 1
#define RegistryStoreTypeDriver                 2

//
// Value used to shift the microframe count from the MFINDEX value.
//
#define MFINDEX_UFRAME_SHIFT                    3

//
// Value used to shift the hardware frame count from the 32 bit software generated Frame counter
//
#define MFINDEX_FRAME_SHIFT                     11

//
// Frame mask value for the MFINDEX value. The MFINDEX register returns a 14bit
// value, where the lower 3bits are shifted off (the microframe count), leaving
// 11bits.
//
#define FRAME_INDEX_MASK                        0x7FF

//
// The Value at which the 11 bit counter would wrap back to 0.
//
#define COUNTER_11BIT_WRAP_VALUE                0x800

typedef struct _DEVICE_DATA {

    PUSBXHCI_TRIAGE_INFO UsbXhciTriageInfo;

    PCONTROLLER_DATA     ControllerData;

    RECORDER_LOG         RecorderLog;

} DEVICE_DATA, *PDEVICE_DATA;

typedef enum _Controller_FatalErrorAction {
    Controller_FatalErrorNoAction = 0,
    Controller_FatalErrorInternalReset = 0x1,
    Controller_FatalErrorReset = 0x2,
    Controller_FatalErrorNoRestart = 0x4,
    Controller_FatalErrorControllerGoneNotifyUCX = 0x8,
    Controller_FatalErrorControllerGoneNotifyUCXAndPNP = 0x10,
    Controller_FatalErrorReportFailedNoRestart = 0x20
} Controller_FatalErrorAction;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_DATA, GetDeviceData)

typedef struct _XHCI_TELEMETRY_DATA_ENTRY {
    LIST_ENTRY                  XhciTelemetryDataListEntry;
    ULONG                       ReasonCode;
    HANDLE                      UsbDeviceHandle;
    HANDLE                      EndpointHandle;
    HANDLE                      TransferRingHandle;
} XHCI_TELEMETRY_DATA_ENTRY, *PXHCI_TELEMETRY_DATA_ENTRY;

#pragma warning(push)
#pragma warning( disable: 4201 ) // nonstandard extension used : nameless struct/union
#pragma warning( disable: 4214 ) // Disable warning C4214: nonstandard extension used : bit field types other than int
typedef struct _XHCI_TELEMETRY_DATA {

    LIST_ENTRY                  XhciTelemetryDataListHead;

    KSPIN_LOCK                  XhciTelemetryDataListLock;

    KMUTEX                      XhciTelemetryMutex;

    ULONG                       RecoveryActions;

    ULONG                       GontrollerGoneDumpGenerated:1;
    ULONG                       ControllerGoneRecoveryActionProcessed:1;
    ULONG                       ControllerGonePnpNotified:1;

} XHCI_TELEMETRY_DATA, *PXHCI_TELEMETRY_DATA;
#pragma warning(pop)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(XHCI_TELEMETRY_DATA, GetTelemetryData)

EVT_WDF_DRIVER_DEVICE_ADD Controller_WdfEvtDeviceAdd;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ConfigureS0IdleSettings(
    __in
        PCONTROLLER_DATA ControllerData
    );

EVT_WDF_WORKITEM Controller_IdleTimeoutUpdateWorker;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateIdleTimeout(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               Timeout
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateIdleTimeoutOnControllerFDOD0Entry(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_UpdateIdleTimeoutOnRootHubPDOD0Entry(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ConfigureSxWakeSettings(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
WDFDEVICE
Controller_GetWdfDeviceObject(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->WdfDevice;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PDEVICE_OBJECT
Controller_GetWdmDeviceObject(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return WdfDeviceWdmGetDeviceObject(ControllerData->WdfDevice);
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PDEVICE_OBJECT
Controller_GetWdmPhysicalDeviceObject(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return WdfDeviceWdmGetPhysicalDevice(ControllerData->WdfDevice);
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetUcxControllerObject(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->UcxController;
}

FORCEINLINE
RECORDER_LOG
Controller_GetRecorderLog(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->RecorderLog;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetIoControlHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->IoControlHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetRegisterHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->RegisterHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetCommonBufferHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->CommonBufferHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetCommandHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->CommandHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetCommandFilterHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->CommandFilterHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetRootHubHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->RootHubHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetInterrupterHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->InterrupterHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetDeviceSlotHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->DeviceSlotHandle;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
Controller_GetWmiHandle(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->WmiHandle;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciVendorID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.VendorId;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciDeviceID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.DeviceId;
}

_Must_inspect_result_
FORCEINLINE
UCHAR
Controller_GetPciRevisionID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (UCHAR)ControllerData->PciDeviceInfo.RevisionId;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciSubVendorID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.SubVendorID;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciSubSystemID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.SubSystemID;
}

_Must_inspect_result_
FORCEINLINE
PSTR
Controller_GetAcpiVendorID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->AcpiDeviceInfo.VendorId;
}

_Must_inspect_result_
FORCEINLINE
PSTR
Controller_GetAcpiDeviceID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->AcpiDeviceInfo.DeviceId;
}

_Must_inspect_result_
FORCEINLINE
PSTR
Controller_GetAcpiRevisionID(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->AcpiDeviceInfo.RevisionId;
}

FORCEINLINE
VOID
Controller_SetXhciVersion(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        UCHAR               MajorVersion,
    __in
        UCHAR               MinorVersion
    )
{
    ControllerData->MajorVersion = MajorVersion;
    ControllerData->MinorVersion = MinorVersion;
}

FORCEINLINE
UCHAR
Controller_GetXhciMajorVersion(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->MajorVersion;
}

FORCEINLINE
UCHAR
Controller_GetXhciMinorVersion(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->MinorVersion;
}

FORCEINLINE
VOID
Controller_SetFirmwareVersion(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONGLONG           FirmwareVersion
    )
{
    ControllerData->FirmwareVersion = FirmwareVersion;
}

FORCEINLINE
ULONGLONG
Controller_GetFirmwareVersion(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->FirmwareVersion;
}

_Must_inspect_result_
FORCEINLINE
UCX_CONTROLLER_PARENT_BUS_TYPE
Controller_GetParentBusType(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->ParentBusType;
}

_Must_inspect_result_
FORCEINLINE
ULONG
Controller_GetPciBusNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->PciDeviceInfo.BusNumber;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciDeviceNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.DeviceNumber;
}

_Must_inspect_result_
FORCEINLINE
USHORT
Controller_GetPciFunctionNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return (USHORT)ControllerData->PciDeviceInfo.FunctionNumber;
}

FORCEINLINE
KSE_XHCI_CONTROLLER_FLAGS
Controller_GetDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->DeviceFlags;
}

FORCEINLINE
VOID
Controller_SetDeviceFlags(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        KSE_XHCI_CONTROLLER_FLAGS   DeviceFlags
    )
{
    //
    // Only allow specific flags to be set using this function. Care should
    // be taken when adding additional flags to this function, since we want
    // to prevent widespread use of setting flags through the driver. Instead,
    // they should be set in Controller_PopulateDeviceFlags, in order to keep
    // them all located in the same function.
    //
    NT_ASSERT(DeviceFlags.Use32BitRegisterAccess || DeviceFlags.UseSingleInterrupter);

    if (DeviceFlags.Use32BitRegisterAccess || DeviceFlags.UseSingleInterrupter) {

        ControllerData->DeviceFlags.AsULong64 |= DeviceFlags.AsULong64;
    }

    return;
}

FORCEINLINE
ULONG64
Controller_GetInternalDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->InternalDeviceFlags;
}

FORCEINLINE
VOID
Controller_SetHibernateContext(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in_opt
        HANDLE                  HibernateContext
    )
{
    ControllerData->HibernateContext = HibernateContext;
}

FORCEINLINE
HANDLE
Controller_GetHibernateContext(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->HibernateContext;
}

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROLLER_DATA, GetControllerData)

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_CreateWdfDevice(
    __in
        PWDFDEVICE_INIT WdfDeviceInit,
    __in
        RECORDER_LOG    RecorderLog,
    __out
        WDFDEVICE       *WdfDevice,
    __out
        PULONG          InstanceNumber
    );

EVT_WDFDEVICE_WDM_IRP_PREPROCESS Controller_WdfEvtPreprocessSetPowerIrp;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS Controller_EvtIoInternalDeviceControl;

EVT_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS Controller_WdfEvtDeviceFilterRemoveResourceRequirements;

EVT_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS Controller_WdfEvtDeviceFilterAddResourceRequirements;

EVT_WDF_DEVICE_REMOVE_ADDED_RESOURCES Controller_WdfEvtDeviceRemoveAddedResources;

EVT_WDF_DEVICE_PREPARE_HARDWARE Controller_WdfEvtDevicePrepareHardware;

EVT_WDF_DEVICE_RELEASE_HARDWARE Controller_WdfEvtDeviceReleaseHardware;

EVT_WDF_DEVICE_USAGE_NOTIFICATION Controller_WdfEvtDeviceUsageNotification;

EVT_WDF_DEVICE_D0_ENTRY Controller_WdfEvtDeviceD0Entry;

EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED Controller_WdfEvtDeviceD0EntryPostInterruptsEnabled;

EVT_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED Controller_WdfEvtDeviceD0ExitPreInterruptsDisabled;

EVT_WDF_DEVICE_D0_EXIT Controller_WdfEvtDeviceD0Exit;

EVT_WDF_DEVICE_ARM_WAKE_FROM_S0 Controller_WdfEvtDeviceArmWakeFromS0;

EVT_WDF_DEVICE_DISARM_WAKE_FROM_S0 Controller_WdfEvtDeviceDisarmWakeFromS0;

EVT_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED Controller_WdfEvtDeviceWakeFromS0Triggered;

EVT_WDF_DEVICE_ARM_WAKE_FROM_SX Controller_WdfEvtDeviceArmWakeFromSx;

EVT_WDF_DEVICE_DISARM_WAKE_FROM_SX Controller_WdfEvtDeviceDisarmWakeFromSx;

EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT Controller_WdfEvtDeviceSelfManagedIoInit;

EVT_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP Controller_WdfEvtDeviceSelfManagedIoCleanup;

EVT_WDF_OBJECT_CONTEXT_CLEANUP Controller_WdfEvtCleanupCallback;

EVT_WDFDEVICE_WDM_POST_PO_FX_REGISTER_DEVICE Controller_EvtPostPoFxRegisterDevice;

EVT_WDFDEVICE_WDM_PRE_PO_FX_UNREGISTER_DEVICE Controller_EvtPrePoFxUnregisterDevice;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_SetLogIdentifier(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
DEVICE_ENUMERATOR
Controller_GetDeviceEnumerator(
    __in
        WDFDEVICE       WdfDevice,
    __in
        RECORDER_LOG    RecorderLog
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_Create(
    __in
        WDFDEVICE       WdfDevice,
    __in
        RECORDER_LOG    RecorderLog,
    __in
        ULONG           InstanceNumber,
    __out
        PHANDLE         ObjectHandle
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0EntryRestoreState(
    __in
        PCONTROLLER_DATA        ControllerData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0EntryPostInterruptsEnabled(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_D0Exit(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0ExitSaveState(
    __in
        PCONTROLLER_DATA        ControllerData
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrievePciData(
    __in
        WDFDEVICE               WdfDevice,
    __in
        RECORDER_LOG            RecorderLog,
    __out
        PPCI_DEVICE_INFORMATION PciDeviceInformation,
    __inout
        PULONGLONG              FirmwareVersion
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulatePciDeviceInformation(
    __in
        RECORDER_LOG            RecorderLog,
    __in
        ULONG                   PciBusNumber,
    __in
        USHORT                  PciDeviceNumber,
    __in
        USHORT                  PciFunctionNumber,
    __in
        PBUS_INTERFACE_STANDARD PciBusInterfaceStandard,
    __in
        PPCI_COMMON_CONFIG      PciCommonConfig,
    __out
        PPCI_DEVICE_INFORMATION PciDeviceInformation,
    __inout
        PULONGLONG              FirmwareVersion
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrieveAcpiData(
    __in
        WDFDEVICE                   WdfDevice,
    __in
        RECORDER_LOG                RecorderLog,
    __inout
        PACPI_DEVICE_INFORMATION    AcpiDeviceInformation
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateAcpiDeviceInformation(
    __in
        PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiInfo,
    __out
        PACPI_DEVICE_INFORMATION               AcpiDeviceInformation
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrieveUrsData(
    __in
        WDFDEVICE                       WdfDevice,
    __in
        RECORDER_LOG                    RecorderLog,
    __out
        UCX_CONTROLLER_PARENT_BUS_TYPE* ParentBusType,
    __out
        PACPI_DEVICE_INFORMATION        AcpiDeviceInformation,
    __out
        PPCI_DEVICE_INFORMATION         PciDeviceInformation,
    __inout
        PULONGLONG                      FirmwareVersion
    );

BOOLEAN
__drv_requiresIRQL(PASSIVE_LEVEL)
Controller_DriverInstalledDueToCompatibleIdMatch(
    __in
        PCONTROLLER_DATA    ControllerData
    );

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Controller_SetDeviceDescription(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlagsFromKse(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlagsFromRegistry(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               RegistryStoreType
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateInternalDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_LogDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
CRASHDUMP_PORT_TYPE
Controller_GetRootPortType(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               PortNumber
    );

EVT_WDF_TIMER Controller_WdfEvtTimerFunction;

EVT_UCX_CONTROLLER_GET_CURRENT_FRAMENUMBER Controller_UcxEvtGetCurrentFrameNumber;

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_InternalReset(
    __in
        PCONTROLLER_DATA    ControllerData
    );

EVT_WDF_WORKITEM Controller_TelemetryReportWorker;

EVT_UCX_CONTROLLER_RESET Controller_UcxEvtReset;

EVT_UCX_CONTROLLER_ENABLE_FORWARD_PROGRESS Controller_UcxEvtEnableForwardProgress;

EVT_UCX_CONTROLLER_QUERY_USB_CAPABILITY Controller_UcxEvtQueryUsbCapability;

#define USB3_WER_BUGCODE_USBXHCI_NO_LIVE_DUMP 0

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_SetControllerGone(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        BOOLEAN             NotifyPnP
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Controller_IsControllerAccessible(
    __in
        PCONTROLLER_DATA    ControllerData
    );

//
// Compat: EHCI hardware FRINDEX counter indicates 1 microframe ahead of what the current
// microframe should be. However XHCI spec does not do that. Thus while calculating the get frame
// number, we would add 1 microframe to match compat with EHCI.
//
#define MICROFRAME_INCREMENT_DEFAULT          1

//
// For certain controllers, we want to prevent programming a TD late into the hardware.
// So in addition to the 1 microframe mentioned above, we advance the microframe counter by
// and additional microframe when we are checking whether a TD is going to be late or not.
//
#define MICROFRAME_INCREMENT_PREVENT_LATE_TD  2

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
Controller_GetFrameNumber(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               MicroframeIncrement
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_AddControllerToGlobalControllerList(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_RemoveControllerFromGlobalControllerList(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_AddDeviceToControllerDeviceList(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        UCXUSBDEVICE        UcxUsbDevice,
    __in
        PLIST_ENTRY         DeviceListEntry
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_RemoveDeviceFromControllerDeviceList(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        UCXUSBDEVICE        UcxUsbDevice,
    __in
        PLIST_ENTRY         DeviceListEntry
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_InitiateRecovery(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        PXHCI_TELEMETRY_DATA        TelemetryData,
    __in
        Controller_FatalErrorAction RecoveryAction
    );

FORCEINLINE
ULONG
Controller_GetControllerDeviceListCount(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->DeviceListCount;
}

FORCEINLINE
ULONG
Controller_GetInstanceNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->InstanceNumber;
}

FORCEINLINE
ULONG64
Controller_GetHardwareVerifierFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->HardwareVerifierFlags;
}

FORCEINLINE
BOOLEAN
Controller_IsForwardProgressEnabled(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ControllerData->ForwardProgressEnabled;
}

FORCEINLINE
BOOLEAN
Controller_IsMSELengthUsable(
__in
PCONTROLLER_DATA    ControllerData
)
{
    return ControllerData->MSELengthUsable;
}

FORCEINLINE
VOID
Controller_SetMSELengthUsable(
__in
PCONTROLLER_DATA    ControllerData,
__in
BOOLEAN             MSELengthUsable
)
{
    ControllerData->MSELengthUsable = MSELengthUsable;
}

FORCEINLINE
BOOLEAN
Controller_InBootOrPagingPath(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return ((ControllerData->SpecialFileStatus & (1 << WdfSpecialFilePaging)) ||
            (ControllerData->SpecialFileStatus & (1 << WdfSpecialFileBoot)));
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_HwVerifierBreakIfEnabled(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in_opt
        UCXUSBDEVICE        Device,
    __in_opt
        UCXENDPOINT         Endpoint,
    __in
        ULONG64             Flag,
    __in
        PCSTR               MessageStr,
    __in_opt
        PTRB                CommandTRB,
    __in_opt
        PTRB                EventTRB
    );


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_ReportFatalError(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        Controller_FatalErrorAction RecoveryAction,
    __in
        ULONG                       ReasonCode,
    __in_opt
        HANDLE                      UsbDeviceHandle,
    __in_opt
        HANDLE                      EndpointHandle,
    __in_opt
        HANDLE                      TransferRingHandle
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateHardwareVerifierFlags(
    __in
        PCONTROLLER_DATA     ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateTestConfiguration(
    __in
        PCONTROLLER_DATA     ControllerData
    );

FORCEINLINE
PTEST_CONFIGURATION
Controller_GetTestConfiguration(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    return &ControllerData->TestConfiguration;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
Controller_TelemetryOkToGenerateReport(
    __in
        PCONTROLLER_DATA     ControllerData,
    __in
        PXHCI_TELEMETRY_DATA TelemetryData,
    __in
        ULONG                ReasonCode
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_TelemetryAddControllerData(
    __in
        PCONTROLLER_DATA ControllerData,
    __in
        HANDLE           TelemetryHandle
    );

VOID
Controller_InitFrameNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_TelemetryUpdateSubmitReportSuccess(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               ReasonCode
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_InitiateBootRecovery(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        ULONG                       RecoveryAction
    );

FORCEINLINE
BOOLEAN
Controller_D0ExitAttemptSaveState(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  TargetPowerDeviceState
    )
/*++

Routine Description:

    This routine determines whether a save state operation should be attempted in
    in D0Exit. It will return FALSE in the following cases:

    1. If this device has a known problem with save/restore as evident by the hack flag.
    2. If a previous save state operation failed.
    3. If target device state is D3Final, eg. shutdown, disable, resource rebalance.
    4. If the system is going to hibernate (target device state will be Dx, not D3Final).

--*/
{
    if ((Controller_GetDeviceFlags(ControllerData).NoControllerSaveRestoreState) ||
        (ControllerData->ControllerSaveStateFailed) ||
        (TargetPowerDeviceState == WdfPowerDeviceD3Final) ||
        (ControllerData->SystemPowerAction == SystemPowerAction_Hibernate)) {

        return FALSE;

    } else {

        return TRUE;
    }
}

FORCEINLINE
BOOLEAN
Controller_D0EntryAttemptRestoreState(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  PreviousPowerDeviceState
    )
/*++

Routine Description:

    This routine determines whether a restore state operation should be attempted
    in D0Entry. It will return FALSE in the following cases:

    1. If this device has a known problem with save/restore as evident by the hack flag.
    2. If the previous save state operation failed or was not attempted.
    3. If previous device state was D3Final, eg. cold boot, enable, resource rebalance.
    4. If the system is resuming from hibernate (previous device state will be Dx, not D3Final).

--*/
{
    if ((Controller_GetDeviceFlags(ControllerData).NoControllerSaveRestoreState) ||
        (ControllerData->ControllerSaveStateFailed) ||
        (PreviousPowerDeviceState == WdfPowerDeviceD3Final) ||
        (ControllerData->SystemPowerAction == SystemPowerAction_ResumeFromHibernate)) {

        return FALSE;

    } else {

        return TRUE;
    }
}
















__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateSqmDatapoints(
    __in
        PCONTROLLER_DATA ControllerData
    );

FORCEINLINE
PBUFFER_DATA
Controller_GetScratchTransferBufferData(
    __in
        PCONTROLLER_DATA        ControllerData
    )
{
    return ControllerData->ScratchTransferBufferData;
}

VOID
Controller_NotifyPortStateToPep(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        SUB_DX_STATE        SubDxState
    );

#if defined(DBG) && !defined(_WIN64)
FORCEINLINE
LONG
Controller_GetNextEventDataId(
    __in
        PCONTROLLER_DATA        ControllerData
    )
{
    return InterlockedIncrement(&ControllerData->EventDataCounter);
}
#endif

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSMToEnableRTD3(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSMForHSICDisconnectInU3(
    __in
        PCONTROLLER_DATA ControllerData,
    __in
        BOOLEAN          IsD0Entry
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSM(
    __in
        PCONTROLLER_DATA ControllerData,
    __in
        ULONG Argument1
    );

POWER_SETTING_CALLBACK Controller_LPEEnterExitCallback;

BOOLEAN
Controller_InUseByDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_AcquireWdfPowerReferenceForDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ReleaseWdfPowerReferenceForDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    );

EXT_CALLBACK Controller_HighResTimerCompletion;
