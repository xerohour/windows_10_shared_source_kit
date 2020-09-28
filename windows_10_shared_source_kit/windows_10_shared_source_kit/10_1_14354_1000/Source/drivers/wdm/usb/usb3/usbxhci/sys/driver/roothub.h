/*++
Copyright (c) Microsoft Corporation

Module Name:

    roothub.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

//
// Universal Serial Bus Specification Revision 2.0
// Table 7-13. Hub Event Timings
//
// NOTE: The specification value of TDRSMDN is a minimum of 20
// milliseconds.  The USBEHCI.SYS driver requests a delay of 50
// milliseconds for EHCI_RH_PortResumeComplete().
//
#define TDRSMDN_IN_MS       50

//
// Time to wait for a port to reach U0 before sending it to U3
// when applying XHCI controller workaround
//
#define DEFAULT_U0_WAIT_COUNT                  1200     // total of 12ms
#define DEFAULT_U0_WAIT_INTERVAL_IN_US         10       // 10 microseconds

//
// Time to wait for a port to reach U3 before powering off the controller
// 
#define DEFAULT_U3_WAIT_COUNT                  2400     // total of 24ms
#define DEFAULT_U3_WAIT_INTERVAL_IN_US         10       // 10 microseconds
#define DEFAULT_U3_RETRY_INTERVAL              3        // every 3 wait intervals (30us)

#define HW_COMPLIANCE_LINK_ERROR_COUNT_WARN_LIMIT   10

typedef struct _USB_VERSION {
    UCHAR MajorVersion;
    UCHAR MinorVersion;
} USB_VERSION, *PUSB_VERSION;

typedef struct _ROOTHUB_TIMER_DATA {
    PROOTHUB_DATA   RootHubData;
    ULONG           PortNumber;
} ROOTHUB_TIMER_DATA, *PROOTHUB_TIMER_DATA;

typedef struct _ROOTHUB_PORT_DATA {
    UCHAR                                   MinorRevision;
    UCHAR                                   MajorRevision;
    UCHAR                                   HubDepth;
    BOOLEAN                                 HighSpeedOnly;
    BOOLEAN                                 IntegratedHubImplemented;
    BOOLEAN                                 DeviceRemovable;
    BOOLEAN                                 ResumeAcknowledged;
    CONTROLLER_USB_20_HARDWARE_LPM_FLAGS    HardwareLPMFlags;
    BOOLEAN                                 HardwareLPMDirty;
    UCHAR                                   U1Timeout;
    UCHAR                                   U2Timeout;
    BOOLEAN                                 HardwareLPMEnabled;
    KSPIN_LOCK                              ReadModifyWriteLock;
    PROOTHUB_TIMER_DATA                     PortResumeTimerData;
    PEX_TIMER                               ExTimer20PortResumeComplete;
    LONG                                    ResumeTimeReferenceCount;
    USHORT                                  ResumeTime;
    USHORT                                  SpeedsCount;
    PSUPPPORTED_PROTOCOL_CAPABILITY_SPEED   Speeds;
} ROOTHUB_PORT_DATA, *PROOTHUB_PORT_DATA;

typedef struct _ROOTHUB_DATA {

    UCXROOTHUB                  UcxRootHub;

    HANDLE                      ControllerHandle;

    ULONG                       NumberOfPorts;
    USHORT                      NumberOf20Ports;
    USHORT                      NumberOf30Ports;
    USHORT                      U1DeviceExitLatency;
    USHORT                      U2DeviceExitLatency;

    POPERATIONAL_REGISTERS      OperationalRegisters;
    PPORT_REGISTER_SET          PortRegisterSet;

    PROOTHUB_PORT_DATA          PortData;

    BOOLEAN                     DebugCapable;

    BOOLEAN                     NoInterruptTransfersSinceFDOD0Entry;

} ROOTHUB_DATA, *PROOTHUB_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ROOTHUB_DATA, GetRootHubData)


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
RootHub_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
RootHub_PrepareHardware(
    __in
        PROOTHUB_DATA   RootHubData
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
RootHub_D0Entry(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
RootHub_D0EntryPostInterruptsEnabled(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
RootHub_D0Exit(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState,
    __in
        SYSTEM_POWER_ACTION     SystemPowerAction                                
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
RootHub_ReleaseHardware(
    __in
        PROOTHUB_DATA   RootHubData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
RootHub_ControllerResetPostReset(
    __in
        PROOTHUB_DATA   RootHubData
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
CRASHDUMP_PORT_TYPE
RootHub_GetPortType(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    );

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtClearHubFeature;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtClearPortFeature;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtGetHubStatus;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtGetPortStatus;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtSetHubFeature;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtSetPortFeature;

EVT_UCX_ROOTHUB_CONTROL_URB         RootHub_UcxEvtGetPortErrorCount;

EVT_UCX_ROOTHUB_INTERRUPT_TX        RootHub_UcxEvtInterruptTransfer;

EVT_UCX_ROOTHUB_GET_INFO            RootHub_UcxEvtGetInfo;

EVT_UCX_ROOTHUB_GET_20PORT_INFO     RootHub_UcxEvtGet20PortInfo;

EVT_UCX_ROOTHUB_GET_30PORT_INFO     RootHub_UcxEvtGet30PortInfo;

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
RootHub_HandlePortStatusChangeEvent(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        PTRB            EventTrb
    );

_Requires_lock_held_(RootHubData->PortData[PortNumber-1].ReadModifyWriteLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
RootHub_RestoreU1U2Timeouts(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    );

NTSTATUS
RootHub_ForceU0AndWait(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    );

VOID
RootHub_Update20HardwareLpmParameters(
    __in
        PROOTHUB_DATA                                   RootHubData,
    __in
        ULONG                                           PortNumber,
    __in
        USBDEVICE_UPDATE_20_HARDWARE_LPM_PARAMETERS     LpmParameters,
    __in
        ULONG                                           DeviceSlot
    );

VOID
RootHub_DisableLPMForSlot(
    __in
        PROOTHUB_DATA           RootHubData,
    __in
        ULONG                   PortNumber,
    __in
        ULONG                   DeviceSlot
    );

VOID
RootHub_SetPortResumeTime(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        USHORT          ResumeTime
    );

VOID
RootHub_ClearPortResumeTime(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    );

NTSTATUS
RootHub_WaitForPendingU3TransitionCompletion(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        BOOLEAN         RetryU3
    );

EXT_CALLBACK     RootHub_WdfEvtTimer20PortResumeComplete;

VOID
RootHub_HandleResumedPorts(
    __in
        PROOTHUB_DATA   RootHubData
    );

BOOLEAN
RootHub_DetectAndAcknowledgePortResume(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber,
    __in
        BOOLEAN         OnPowerUp
    );

BOOLEAN
RootHub_HideInvalidDebugPortStatusAndChange(
    __in
        PROOTHUB_DATA   RootHubData,
    __in
        ULONG           PortNumber
    );

USB_VERSION
RootHub_GetHighestUsbVersionSupported(
    __in
        PROOTHUB_DATA   RootHubData
    );

BOOLEAN
RootHub_IsPortConnected(
    __in 
        PROOTHUB_DATA       RootHubData,
    __in
        ULONG               PortNumber
    );
