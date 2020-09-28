/*
Module: EX.C


name:USB Diagnostic Events (Hub Exceptions)

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002

Abstract:

    This code implements the hub error logging and other diagnostic support.



Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      NULL
    Unload -->linked to Unload           NULL
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        UsbhExAddDeviceInit
    Remove -->linked to PNP REMOVE       UsbhExRemove

</pre>

Hub Exception Record Format:

    This structure is a contiguous block of memory to facilitate debugging. The Data
    and SzFile pointers always point into the same allocated memory block as the
    exception record. Each exception record also conatins a snapshot of the last eight
    hub log entries.


<pre>

typedef struct _HUB_EXCEPTION_RECORD {

    ULONG Sig;
    ULONG TotalLength;
    PDEVICE_OBJECT  HubFdo;
    HUB_EXCEPTION_CLASS Class;
    USHORT PortNumber;
    USHORT Pad;
    USBD_STATUS UsbdStatus;
    NTSTATUS NtStatus;
    USHORT DataLength;
    PUCHAR Data;
    LIST_ENTRY ExrLink;
    LARGE_INTEGER EventTime;
    LOG_ENTRY DebugLog[8];
    ULONG Line;
    PUCHAR SzFile;

} HUB_EXCEPTION_RECORD, *PHUB_EXCEPTION_RECORD;

</pre>


*/

#ifndef HUB_KDEXTS

#include "hubdef.h"
#include "pch.h"

#endif

#ifdef HUB_WPP
#include "ex.tmh"
#endif



/*
name: Exception class codes

Hub Exceptions are generated for various types of hardware problems we encounter when
enumerating devices or reviving devices from power management. Hub Exceptions are
grouped into these subclasses...

sub-classes:

PnpErr_     - problems encountered during enumeration.
PowerErr_   - problems encountered during power management.
HubErr_     - hardware errors on the hub.
UsbErr_     - USB specific errors
SsErr_      - selective suspend, this troublesome feature gets its own class
GenErr_     - everything else
*/


/*
    Dispatch table of exception handler routines based on class.  Note that some
    classes use the same handler.
*/

const HUB_EXCEPTION_DISPATCH  HubExceptionTable[ExceptionClassLast] =
{
/*
name: HubErr_TransactionFailure_Reset

The hub has failed a transaction such as get port status, this generally indicates that
the hub firmware or hardware has failed in some way.

Action: Reset the hub.
*/
    HubErr_TransactionFailure_Reset,
    "HubErr_TransactionFailure_Reset",
    UsbhQueueHardReset,

/*
name: DeviceOvercurrent_Popup

Hub detected overcurrent on one of the downstream ports.

Action: Popup the overcurrent reset bubble.
*/
    DeviceOvercurrent_Popup,
    "DeviceOvercurrent_Popup",
    UsbhDeviceOvercurrent,

/*
name:CreatePdoFailure_Popup

Occurs when we can't make a PDO for some reason.

Action: Popup the unknown device bubble.
*/
    CreatePdoFailure_Popup,
    "CreatePdoFailure_Popup",
    UsbhEnumFailurePopup,

/*
name:InterruptPipeFailure_Reset

If we see more than three transaction errors on the interrupt pipe we will reset the hub
'three strikes'.  An error is defined as an interrupt transaction failing and clear
stall does not recover.

Action: Hard reset the hub.
*/
    InterruptPipeFailure_Reset,
    "InterruptPipeFailure_Reset",
    UsbhQueueHardReset,

/*
name:BadConfigDescriptor

We generate this exception if the device returns a configuration descriptor that is not
valid.

Action: none.
*/
    BadConfigDescriptor,
    "BadConfigDescriptor",
    NULL,

/*
name:HubErr_GetPortStatusFailed

This usually indicates a hardware problem with the hub.

Action: none.
*/
    HubErr_GetPortStatusFailed,
    "HubErr_GetPortStatusFailed",
    NULL,

/*
name:HubErr_BadControlData

Invalid data detected on the hubs control pipe.

Action: none.
*/
    HubErr_BadControlData,
    "HubErr_BadControlData",
    NULL,

/*
name:HubErr_NoChangeBit

The hub indicated a change with no change bits set.

Action: none.
*/
    HubErr_NoChangeBit,
    "HubErr_NoChangeBit",
    NULL,

/*
name:CreateDeviceFailure_Popup

This exception is generated when the maximum number of tries to create (enumerate the
device) has been exceeded.

Action: Generate the 'USB Device Failed' popup.
*/
    CreateDeviceFailure_Popup,
    "CreateDeviceFailure_Popup",
    UsbhEnumFailurePopup,

/*
name:CreateDeviceFailure

This is generated on any failure to the port driver *CreateUSBDevice* function.

Data: Additional data includes a copy of the _USB_CD_ERROR_INFORMATION struct.

Action: none.
*/
    CreateDeviceFailure,
    "CreateDeviceFailure",
    NULL,

/*
name:CreateDeviceFailure_NoHubHandle

CreateDevice function failed because we didn't have a valid device handle for the hub.

Action: none.
*/
    CreateDeviceFailure_NoHubHandle,
    "CreateDeviceFailure_NoHubHandle",
    NULL,

/*
name:HubErr_ResetPortFailed

The control command used to generate a port reset failed.

Action: none.
*/
    HubErr_ResetPortFailed,
    "HubErr_ResetPortFailed",
    NULL,

/*
name:HubErr_SetPortFeatureFailed

Hub class command failed, this usually indicates hardware failure.

Action: none.
*/
    HubErr_SetPortFeatureFailed,
    "HubErr_SetPortFeatureFailed",
    NULL,

/*
name:HubErr_GetConfigDescriptorFailed

Transaction to fetch the hub configuration descriptor failed.

Action: none.
*/
    HubErr_GetConfigDescriptorFailed,
    "HubErr_GetConfigDescriptorFailed",
    NULL,

/*
name:HubErr_GetHubDescriptorFailed

Transaction to fetch the hub class descriptor failed.

Action: none.
*/
    HubErr_GetHubDescriptorFailed,
    "HubErr_GetHubDescriptorFailed",
    NULL,

/*
name:HubErr_GetHubDescriptorFailed_NoPorts

The hub reported that it has no ports -- the hub class descriptor may be invalid.

Action: none.
*/
    HubErr_GetHubDescriptorFailed_NoPorts,
    "HubErr_GetHubDescriptorFailed_NoPorts",
    NULL,

/*
name:HubErr_GetDescriptorFailed

Control transaction to fetch descriptor failed.

Action: none.
*/
    HubErr_GetDescriptorFailed,
    "HubErr_GetDescriptorFailed",
    NULL,

/*
name:HubErr_GetStatusFailed

Hub class command failed, this usually indicates hardware failure.

Action: none.
*/
    HubErr_GetStatusFailed,
     "HubErr_GetStatusFailed",
    NULL,

/*
name:HubErr_HubInterfaceNotFound

Driver could not locate the hub class interface descriptor.

Action: none.
*/
    HubErr_HubInterfaceNotFound,
    "HubErr_HubInterfaceNotFound",
    NULL,

/*
name:HubErr_PortResetTtFailed

Hub class command to reset the TT failed.

Action: none.
*/
    HubErr_PortResetTtFailed,
    "HubErr_PortResetTtFailed",
    NULL,

/*
name:HubErr_BadBusIf

Hub bus interface version returned by USBPORT is invalid or not high enough.

Action: none.
*/
    HubErr_BadBusIf,
    "HubErr_BadBusIf",
    NULL,

/*
name:HubErr_NoHubBusIf

Unable to access the required bus interface.

Action: Hub will fail start.
*/

    HubErr_NoHubBusIf,
    "HubErr_NoHubBusIf",
    NULL,

/*
name:HubErr_NoUsbdiBusIf

Unable to bind to USB bus interface.

Action: Hub will fail start.
*/
    HubErr_NoUsbdiBusIf,
    "HubErr_NoUsbdiBusIf",
    NULL,

/*
name:HubErr_SetPortPowerFailed

Unable to power the hubs ports

Action: Hub will fail start.
*/
    HubErr_SetPortPowerFailed,
    "HubErr_SetPortPowerFailed",
    NULL,

/*
name:Reset1Failure_Popup

The device has repeatedly failed the first phase USB reset required for enumeration.

Action: Enumeration failure popup is generated.
*/
    Reset1Failure_Popup,
    "Reset1Failure_Popup",
    UsbhEnumFailurePopup,

/*
name:Reset2Failure_Popup

The device has repeatedly failed the second phase USB reset required for enumeration.

Action: Enumeration failure popup is generated.
*/
    Reset2Failure_Popup,
    "Reset2Failure_Popup",
    UsbhEnumFailurePopup,

/*
name:InitializeDeviceFailure_Popup

Initialize device failed multiple times -- the device cannot be enumerated.

Action: Enumeration failure popup is generated.
*/
    InitializeDeviceFailure_Popup,
    "InitializeDeviceFailure_Popup",
    UsbhEnumFailurePopup,

/*
name: InitializeDeviceFailure

This is generated on any failure to the port driver *InitializeUSBDevice* function.

Data: Additional data includes a copy of the _USB_ID_ERROR_INFORMATION struct.

Action: none.
*/
    InitializeDeviceFailure,
    "InitializeDeviceFailure",
    NULL,

/*
name:SetupDeviceFailure_Popup

An error has occurred in the initialization of the device -- this is where we fetch device, string
and string descriptors before reporting the device to pnp.

Action: Enumeration failure popup is generated.
*/
    SetupDeviceFailure_Popup,
    "SetupDeviceFailure_Popup",
    UsbhEnumFailurePopup,

/*
name:HubErr_PortDisableFailed_Reset

Hardware error attempting to disable the port.  This is bad because it can allow more than one USB
device to be listening on address zero.

Action: Reset the hub.
*/
    HubErr_PortDisableFailed_Reset,
    "HubErr_PortDisableFailed_Reset",
    UsbhQueueHardReset,

/*
name:InvalidDeviceConfigDescriptor

The devices configuration descriptor is invalid.

Action: Hub will fail to start.
*/
    InvalidDeviceConfigDescriptor,
    "InvalidDeviceConfigDescriptor",
    NULL,

/*
name:HubErr_InvalidHubConfigDescriptor

The configuration descriptor reported by the hub is invalid.

Action: Hub will fail to start.
*/
    HubErr_InvalidHubConfigDescriptor,
    "HubErr_InvalidHubConfigDescriptor",
    NULL,

/*
name:HubErr_GetPortStatusInitFailed

the hub was unable to read the port status registers at initialization time.

Action: Hub will fail to start.
*/
    HubErr_GetPortStatusInitFailed,
    "HubErr_GetPortStatusInitFailed",
    NULL,

/*
name:HubErr_QueueChangeInitFailed

Critical routine failed during initialization.

Action: Hub will fail to start.
*/
    HubErr_QueueChangeInitFailed,
    "HubErr_QueueChangeInitFailed",
    NULL,

/*
name:NotEnoughPower_Popup

There is not enough power supplied[[lied by the port for the requested configuration.

Action: The 'Insufficient Power' popup bubble is generated.
*/
    NotEnoughPower_Popup,
    "NotEnoughPower_Popup",
    UsbhNotEnoughPowerPopup,

/*
name:BadSerialNumberString

Serial number string reported by the device contains invalid chars.

Action: The string will be ignored.
*/
    BadSerialNumberString,
    "BadSerialNumberString",
    NULL,

/*
name:BadUsbString

Invalid USB string descriptor.

Action: none.
*/
    BadUsbString,
    "BadUsbString",
    NULL,

/*
name:UsbErr_InsufficientBandwidth_Popup

Insufficient bandwidth to configure device or interface.

Action: The 'Insufficient Bandwidth' popup bubble is generated.
*/
    UsbErr_InsufficientBandwidth_Popup,
    "UsbErr_InsufficientBandwidth_Popup",
    UsbhNotEnoughBandwidth_Popup,

/*
name:DuplicateSerialNumber

Two USB devices have reported the same serial number.  Since only one may be used as a unique id by
pnp the serial number on the second incarnation is ignored.

Action: Duplicate serial number is ignored.
*/
    DuplicateSerialNumber,
    "DuplicateSerialNumber",
    NULL,

/*
name:BadSerialNumberChars

Bad characters found in serial number string.

Action: none.
*/
    BadSerialNumberChars,
    "BadSerialNumberChars",
    NULL,

/*
name:LegacyDevice_Popup

User needs a USB 2.0 controller.

Action: Popup the 'High speed device in full speed port' bubble.
*/
    LegacyDevice_Popup,
    "LegacyDevice_Popup",
    UsbhLegacyDevicePopup,

/*
name:PowerErr_WarmStartFailed_Reset

Warm start (resume) after power management has failed

Action: Reset the hub.
*/
    PowerErr_WarmStartFailed_Reset,
    "PowerErr_WarmStartFailed_Reset",
    UsbhQueueHardReset,

/*
name:PowerErr_ColdStartFailed_Reset

Cold start (resume) after power management has failed

Action: Reset the hub.
*/
    PowerErr_ColdStartFailed_Reset,
    "PowerErr_ColdStartFailed_Reset",
    UsbhQueueHardReset,

/*
name:PowerErr_SetPortResumeFailed

Resume of hub port failed after power management.

Action: none.
*/
    PowerErr_SetPortResumeFailed,
    "PowerErr_SetPortResumeFailed",
    NULL,

/*
name:PowerErr_GetPortStatusFailed

Get status of hub port failed after power management

Action: none.
*/
    PowerErr_GetPortStatusFailed,
    "PowerErr_GetPortStatusFailed",
    NULL,

/*
name:PowerErr_FailedWakeDisable

Failed to CLEAR wake-up status on device

Action: none.
*/
    PowerErr_FailedWakeDisable,
    "PowerErr_FailedWakeDisable",
    NULL,

/*
name:PowerErr_FailedWakeEnable

Failed to SET wake-up status on device.

Action: none.
*/
    PowerErr_FailedWakeEnable,
    "PowerErr_FailedWakeEnable",
    NULL,

/*
name:PowerErr_HubFailAfterSuspend

The hub failed to function after a hibernate or suspend.

Action: none.
*/
    PowerErr_HubFailAfterSuspend,
    "PowerErr_HubFailAfterSuspend",
    NULL,

/*
name:PowerErr_RootHubFailAfterSuspend

Host Controller failed to function after a hibernate or suspend.

Action: none.
*/
    PowerErr_RootHubFailAfterSuspend,
    "PowerErr_RootHubFailAfterSuspend",
    NULL,

/*
name:HubErr_SuspendPortFailed

Failed to suspend a hub port.

Action: none.
*/
    HubErr_SuspendPortFailed,
    "HubErr_SuspendPortFailed",
    NULL,

/*
name:PowerErr_FlushPortChangeFailed

Failed to clear port change on resume.

Action: none.
*/
    PowerErr_FlushPortChangeFailed,
    "PowerErr_FlushPortChangeFailed",
    NULL,

/*
name:PowerErr_PowerPortsFailed

Failed to power ports after hibernate.

Action: none.
*/
    PowerErr_PowerPortsFailed,
    "PowerErr_PowerPortsFailed",
    NULL,

/*
name:PowerErr_PostResumeResetFailed

Failed to reset the hub device after power management.

Action: none.
*/
    PowerErr_PostResumeResetFailed,
    "PowerErr_PostResumeResetFailed",
    NULL,

/*
name:DebounceFailure

Encountered problems establishing a stable connection.

Action: none.
*/
    DebounceFailure,
    "DebounceFailure",
    NULL,

/*
name:DropDevice

Problems maintaining the hub port connection to the device.

Action: none.
*/
    DropDevice,
    "DropDevice",
    NULL,

/*
name:HubErr_StartDevFailedLower

Driver below the hub FDO or bus driver failed MN_START_DEVICE

Action: none.
*/
    HubErr_StartDevFailedLower,
    "HubErr_StartDevFailedLower",
    NULL,

/*
name:FailedProductIdString

Device indicated it had a ProductId String but failed to report it.

Action: none.
*/
    FailedProductIdString,
    "FailedProductIdString",
    NULL,

/*
name:NullSerialNumberString

Device reported a NULL serial number string.

Action: none.
*/
    NullSerialNumberString,
    "NullSerialNumberString",
    NULL,

/*
name:HubErr_PowerBit_Dropped

Power bit dropped to zero -- may be overcurrent.

Action: none.
*/
    HubErr_PowerBit_Dropped,
    "HubErr_PowerBit_Dropped",
    NULL,

/*
name:HubErr_Reset1BadEnable

Reset timed out with enable bit set.

Action: none.
*/
    HubErr_Reset1BadEnable,
    "Reset(1) timed out with enable bit set",
    NULL,

/*
name:HubErr_Reset2BadEnable

Reset timed out with enable bit set.

Action: none.
*/
    HubErr_Reset2BadEnable,
    "Reset(2) timed out with enable bit set",
    NULL,

/*
name:HubErr_HardError_Reset

Completely bogus change or status bits reported by the hub, so bad that our only hope is
to reset the hub.

Action: Reset the hub.
*/
    HubErr_HardError_Reset,
    "HubErr_HardError_Reset",
    UsbhQueueHardReset,

/*
name:BadUsbString_LangId

Bad USB language id string, see USB 20 sec 9.6.7 . The device has failed to report a
language id array even though it indicates the device has string descriptors.


Action: none.
*/
    BadUsbString_LangId,
    "BadUsbString_LangId",
    NULL,

/*
name:HubErr_InvalidHubDeviceDescriptor

Bad Device Descriptor for USB hub

Action: none.
*/
    HubErr_InvalidHubDeviceDescriptor,
    "HubErr_InvalidHubDeviceDescriptor",
    NULL,

/*
name:HubErr_FinishStartFailed

Delayed start failed for root hub.

Action: none.
*/
    HubErr_FinishStartFailed,
    "HubErr_FinishStartFailed",
    NULL,

/*
name:UsbdRestoreDevice_Failed

The call to RestoreDevice was failed by the port driver.

Action: none.
*/
    UsbdRestoreDevice_Failed,
    "UsbdRestoreDevice_Failed",
    NULL,

/*
name:SyncResetDevice_Failed

Attempt to reset the device port failed (Driver Reset).

Action: none.
*/
    SyncResetDevice_Failed,
    "SyncResetDevice_Failed",
    NULL,

/*
name:ResetPdoPort_CallerNotPassiveLevel

Driver issued port reset request (Driver Reset) at DISPATCH_LEVEL or higher.

Action: none.
*/
    ResetPdoPort_CallerNotPassiveLevel,
    "ResetPdoPort_CallerNotPassiveLevel",
    NULL,

/*
name:ResetPdoPort_CallerNotInD0

Driver issued port reset request (Driver Reset) while not in D0.

Action: none.
*/
    ResetPdoPort_CallerNotInD0,
    "ResetPdoPort_CallerNotInD0",
    NULL,


/*
name:ReplicantDetected

A replicant or 'ghost' device was detected on the bus.

Action: none.
*/
    ReplicantDetected,
    "ReplicantDetected",
    NULL,

/*
name:HubErr_HardResetFailed

Hardware reset attempts for this hub failed or are disabled.

Action: none.
*/
    HubErr_HardResetFailed,
    "HubErr_HardResetFailed",
    NULL,

/*
name:HubErr_HubOverCurrentChangeDetected

Hub reported a status change to global over-current.

Action: none.
*/
    HubErr_HubOverCurrentChangeDetected,
    "HubErr_HubOverCurrentChangeDetected",
    NULL,

/*
name:HubErr_HubLocalPowerChangeDetected

Hub reported a status change to LocalPower.

Action: none.
*/
    HubErr_HubLocalPowerChangeDetected,
    "HubErr_HubOverCurrentChangeDetected",
    NULL,

/*
name:HubErr_HubChangeDetected

Hub reported a global change with no change bits set.

Action: none.
*/
    HubErr_HubChangeDetected,
    "HubErr_HubChangeDetected",
    NULL,

/*
name:HubErr_HubChangeDetected

Hub indicates a global overcurrent condition.

Action: hard reset the hub.
*/
    HubErr_GlobalOvercurrent_Reset,
    "HubErr_GlobalOvercurrent_Reset",
    UsbhQueueHardReset,

/*
name:ExcessiveDriverResetsDetected

The driver for this PDO has reset the device many times -- this could indicate a hardware problem
of some kind.

Action: none.
*/
    ExcessiveDriverResetsDetected,
    "ExcessiveDriverResetsDetected",
    NULL,

/*
name:DriverPortResetFailed



Action: none.
*/
    DriverPortResetFailed,
    "DriverPortResetFailed",
    NULL,

/*
name:PowerErr_FailedGetStatusOnSuspend

unable to access port during suspend (D2)

Action: none.
*/
    PowerErr_FailedGetStatusOnSuspend,
    "PowerErr_FailedGetStatusOnSuspend",
    NULL,

/*
name:PowerErr_SelectiveSuspendFailed

unable to suspend port

Action: none.
*/
    PowerErr_SelectiveSuspendFailed,
     "PowerErr_SelectiveSuspendFailed",
    NULL,

/*
name:PnpErr_Reset1Failure

Port reset has timed out or failed for some reason.

Action: none.
*/
    PnpErr_Reset1Failure,
     "PnpErr_Reset1Failure",
    NULL,

/*
name:PnpErr_Reset2Failure

Phase 2 port reset has timed out or failed for some reason.

Action: none.
*/
    PnpErr_Reset2Failure,
     "PnpErr_Reset2Failure",
    NULL,

/*
name:HubErr_DescriptorParseError

invalid descriptor passed to an internal paring function.

Action: none.
*/

    HubErr_DescriptorParseError,
    "HubErr_DescriptorParseError",
    NULL,

/*
name:PnpErr_InvalidMsOsDescriptor

msos descriptor is invalid or not supported.

Action: none.
*/

    PnpErr_InvalidMsOsDescriptor,
    "PnpErr_InvalidMsOsDescriptor",
    NULL,

/*
name:PowerErr_FdoPowerFailure

hub driver encountered an error during power management, these
can be cause by the device disappearing during the suspend/resume
process.

Action: none.
*/

    PowerErr_FdoPowerFailure,
    "PowerErr_FdoPowerFailure",
    NULL,

/*
name:PnpErr_FailedResetOnStart

device failed to reset on a restart -- start after soft-remove or stop.

Action: none.
*/

    PnpErr_FailedResetOnStart,
    "PnpErr_FailedResetOnStart",
    NULL,

/*
name:HubErr_PnpDeviceStateError

reporting PnpDeviceState error, FDO will be removed.

Action: none.
*/

    HubErr_PnpDeviceStateError,
    "HubErr_PnpDeviceStateError",
    NULL,

/*
name:PowerErr_PostHiberRestoreFailed

The attempt to restore the device failed this could be because the
device was switched during hibernate.

Action: none.
*/

    PowerErr_PostHiberRestoreFailed,
    "PowerErr_PostHiberRestoreFailed",
    NULL,

/*
name:UsbErr_GetSNstringDescrptorFailed

The request to query the SN string failed.

Action: none.
*/
    UsbErr_GetSNstringDescrptorFailed,
    "UsbErr_GetSNstringDescrptorFailed",
    NULL,

/*
name:GenErr_ApiValidationFailure

This is generated when an API (user mode IOCTL) is failed due to
invalid parameters.

Action: none.

*/
    GenErr_ApiValidationFailure,
    "GenErr_ApiValidationFailure",
    NULL,

/*
name:GenErr_UserIoctlFailed

This is generated when an API (user mode IOCTL) is failed due to
invalid parameters.

Action: none.

*/
    GenErr_UserIoctlFailed,
    "GenErr_UserIoctlFailed",
    NULL,

/*
name:GenErr_UserIoctlFailed_Unsupported

This is generated when an API (user mode IOCTL) is failed because it
no longer supported.

Action: none.

*/
    GenErr_UserIoctlFailed_Unsupported,
    "GenErr_UserIoctlFailed_Unsupported",
    NULL,


/*
name:GenErr_UserIoctlFailed_InvalidIrql

This is generated when an API (user mode IOCTL) is failed because it
called at high irql.

Action: none.

*/
    GenErr_UserIoctlFailed_InvalidIrql,
    "GenErr_UserIoctlFailed_InvalidIrql",
    NULL,

/*
name:GenErr_IoctlCycleFailed

The call to cycle port failed.

Action: none.

*/
    GenErr_IoctlCycleFailed,
    "GenErr_IoctlCycleFailed",
    NULL,

/*
name:GenErr_IoctlCycleFailed_InvalidIrql

A driver attempted to cycle the port at raised IRQL.

Action: none.

*/
    GenErr_IoctlCycleFailed_InvalidIrql,
    "GenErr_IoctlCycleFailed_InvalidIrql",
    NULL,

/*
name:GenErr_IoctlCycleDetected

Logged whenever a driver port cycle request is detected.

Action: none.

*/
    GenErr_IoctlCycleDetected,
    "GenErr_IoctlCycleDetected",
    NULL,

/*
name:PowerErr_MultiplePortsSignalledResume

We found more than one port on the hub indicating resume, may
indicate a problem with the hub hardware.

Action: none.

*/
    PowerErr_MultiplePortsSignalledResume,
    "PowerErr_MultiplePortsSignalledResume",
    NULL,

/*
name:HubErr_PortResetTtFailed_NoAddress

Unable to acquire the device address needed to reset the TT.

Action: none.

*/

    HubErr_PortResetTtFailed_NoAddress,
    "HubErr_PortResetTtFailed_NoAddress",
    NULL,

/*
name:GenErr_WmiApiValidationFailure

Wmi API called with invalid parameters or on an invalid PDO

Action: none.

*/

    GenErr_WmiApiValidationFailure,
    "GenErr_WmiApiValidationFailure",
    NULL,

/*
name:PnpErr_LegacyDevice

HS device on a FS or LS controller, this event is generated at enum
time.  We generate this event any time we see a HS device running at FS.

Action: none.
*/
    PnpErr_LegacyDevice,
    "PnpErr_LegacyDevice",
    NULL,

/*
name:PnpErr_PdoSymLinkFailed

Failure to create a sym link. Test tools need these links to walk the device
tree.

Action: none.
*/
    PnpErr_PdoSymLinkFailed,
    "PnpErr_PdoSymLinkFailed",
    NULL,

/*
name:PnpErr_MsOsDescriptor_NotSupported

OS descriptor not supported.

Action: none.
*/

    PnpErr_MsOsDescriptor_NotSupported,
    "PnpErr_MsOsDescriptor_NotSupported",
    NULL,

/*
name:PnpErr_PdoStartFailed

Hub failed irp_mn_start_device for this PDO.

Action: none.
*/

    PnpErr_PdoStartFailed,
    "PnpErr_PdoStartFailed",
    NULL,

/*
name:CreateDeviceFailure_GetTtFailed

Could not access the TT handle to create the device.

Action: none.
*/

    PnpErr_CreateDeviceFailure_GetTtFailed,
    "PnpErr_CreateDeviceFailure_GetTtFailed",
    NULL,

/*
name:HubErr_OvercurrentFalse

The hub indicated an overcurrent change but no overcurrent
condition is present.

Action: none.
*/

    HubErr_OvercurrentFalse,
    "HubErr_OvercurrentFalse",
    NULL,

/*
name:HubErr_OvercurrentClearFailed

Hub indicated overcurrent but attempts to clear and re-power failed.

Action: none.
*/

    HubErr_OvercurrentClearFailed,
    "HubErr_OvercurrentClearFailed",
    NULL,

/*
name:HubErr_Overcurrent_AutoClear

Hub reported auto current during device enumeration and we attempted to
'auto recover'.

Action: hub overcurrent recovery.
*/

    HubErr_Overcurrent_AutoClear,
    "HubErr_Overcurrent_AutoClear",
    UsbhQueueOvercurrentReset,


/*
name:HubErr_OvercurrentIgnored

overcurrent condition was reported but the port remained powered
this condition is ignored for compatibility with NT5.1 and earlier.

Action: none.
*/

    HubErr_OvercurrentIgnored,
    "HubErr_OvercurrentIgnored",
    NULL,

/*
name:PowerErr_ResumeFailedHubClassDs

Hub resume failure.

Action: none.
*/

    PowerErr_ResumeFailedHubClassDs,
    "PowerErr_ResumeFailedHubClassDs",
    NULL,

/*
name:PowerErr_ErrorOnUpstreamStatus

Hub resume failure.

Action: none.
*/

    PowerErr_ErrorOnUpstreamStatus,
    "PowerErr_ErrorOnUpstreamStatus",
    NULL,


/*
name:PowerErr_UpstreamPortDisabled

Hub resume failure.

Action: none.
*/

    PowerErr_UpstreamPortDisabled,
    "PowerErr_UpstreamPortDisabled",
    NULL,

/*
name:PowerErr_DeviceCycledPostResume

Hub resume failure.

Action: none.
*/

    PowerErr_DeviceCycledPostResume,
    "PowerErr_DeviceCycledPostResume",
    NULL,


/*
name:PowerErr_HardwareFailureOnWarmStart

Hub resume failure.

Action: none.
*/

    PowerErr_HardwareFailureOnWarmStart,
    "PowerErr_HardwareFailureOnWarmStart",
    NULL,


/*
name:PowerErr_DeviceUnexpectedlyLostPower

Power was interrupted to the device during S1 or S3.

Action: none.
*/

    PowerErr_DeviceUnexpectedlyLostPower,
    "PowerErr_DeviceUnexpectedlyLostPower",
    NULL,


/*
name:HubErr_DriverOvercurrent_Popup

Power was interrupted to the device during S1 or S3.

Action: popup delayed UI.
*/

    HubErr_DriverOvercurrent_Popup,
    "HubErr_DriverOvercurrent_Popup",
    UsbhQueueDriverOvercurrent,

/*
name:HubErr_InterruptZeroBytes

Hub returned zero byte buffer on the interrupt pipe.

Action: none.
*/

    HubErr_InterruptZeroBytes,
    "HubErr_InterruptZeroBytes",
    NULL,

/*
name:HubErr_InvalidChangeBitReported

Hub returned a change bit for an invalid port number.

Action: none.
*/

    HubErr_InvalidChangeBitReported,
    "HubErr_InvalidChangeBitReported",
    NULL,

/*
name:PowerErr_DeviceDisconnectOnSuspend

device was disconnected while we were suspending the port.

Action: none.
*/

    PowerErr_DeviceDisconnectOnSuspend,
    "PowerErr_DeviceDisconnectOnSuspend",
    NULL,

/*
name:HubErr_SetPortIndicatorFailed

Command to set the indicators blinking has failed.

Action: none.
*/
    HubErr_SetPortIndicatorFailed,
    "HubErr_SetPortIndicatorFailed",
    NULL,

/*
name:GenErr_AppliedDeviceFix

Set whenever we apply a hardcoded fix for a USB device

Action: none.
*/

    GenErr_AppliedDeviceFix,
    "GenErr_AppliedDeviceFix",
    NULL,

/*
name:PnpErr_FailedResetOnQI

Unable to restore the device handle on a call tp QueryInterface.

Action: none.
*/

    PnpErr_FailedResetOnQI,
    "PnpErr_FailedResetOnQI",
    NULL,

/*
name:PnpErr_BadIdleIrpParameters

Bad Idle callback struct passed in IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION

Action: none.
*/

    PnpErr_BadIdleIrpParameters,
    "PnpErr_BadIdleIrpParameters",
    NULL,

/*
name:HubErr_SetUsbWakeFailed

Transaction to set/clear USB wake for a hub failed.

Action: none.
*/

    HubErr_SetUsbWakeFailed,
    "HubErr_SetUsbWakeFailed",
    NULL,

/*
name:PowerErr_PortResumeTimeout

unable to resume a port after a global bus suspend.

Action: none.
*/

    PowerErr_PortResumeTimeout,
    "PowerErr_PortResumeTimeout",
    NULL,

/*
name:PowerErr_ResumeTimeoutRestoreFailed

unable to reset port after resume timeout.

Action: none.
*/

    PowerErr_ResumeTimeoutRestoreFailed,
    "PowerErr_ResumeTimeoutRestoreFailed",
    NULL,

/*
name:HubErr_HubParked

Failure during the reset process has avused the hub to enter error'park' mode.

Action: none.
*/
    HubErr_HubParked,
    "HubErr_HubParked",
    NULL,

/*
name:PowerErr_DeviceDisconnectOnResume

Device not detected on resume from cold start -- may take too long to signal attach.

Action: none.
*/
    PowerErr_DeviceDisconnectOnResume,
    "PowerErr_DeviceDisconnectOnResume",
    NULL,

/*
name:SsErr_FailedSshResumeOnPort

Failure to resume a speific port on selective suspend/resume.

Action: none.
*/
    SsErr_FailedSshResumeOnPort,
    "SsErr_FailedSshResumeOnPort",
    NULL,

/*
name:SsErr_FailedResumeOnPort

Failure to resume a speific port on selective suspend/resume.

Action: none.
*/
    SsErr_FailedResumeOnPort,
    "SsErr_FailedResumeOnPort",
    NULL,

    /*
name:PdoFailureData

A PDO recorded failure data via IOCTL_INTERNAL_USB_RECORD_FAILURE.

Action: none.
*/
    PdoFailureData,
    "PdoFailureData",
    NULL,

/*
name:NestedHubs_Popup

More than 5 nested hubs have been connected

Action: The 'Too Many Hubs' popup bubble is generated.
*/
    NestedHubs_Popup,
    "NestedHubs_Popup",
    UsbhNestedHubsPopup,

/*
name:HubErr_User_Reset

Action: Reset the hub.
*/
    HubErr_User_Reset,
    "HubErr_User_Reset",
    UsbhQueueHardReset,

/*
name:GenErr_AppliedHubFix

Action: applied hub hack.
*/
    GenErr_AppliedHubFix,
    "GenErr_AppliedHubFix",
    NULL,

/*
name:PowerErr_SyncResumePortFailed

Unable to resume device from suspend state

Action: none.
*/
    PowerErr_SyncResumePortFailed,
    "PowerErr_SyncResumePortFailed",
    NULL,

/*
name:PnpErr_InvalidContainerID

MS OS Extended Property Descriptor contains an invalid Container ID 

Action: none.
*/

    PnpErr_InvalidContainerID,
    "PnpErr_InvalidContainerID",
    NULL,

/*
name:SsErr_FailedSshSuspendHub

Hub failed to suspend 

Action: none.
*/

    SsErr_FailedSshSuspendHub,
    "SsErr_FailedSshSuspendHub",
    NULL,

/*
name:SsErr_FailedSshResumeHub

Hub failed to resume from selective suspend

Action: none.
*/

    SsErr_FailedSshResumeHub,
    "SsErr_FailedSshResumeHub",
    NULL,

/*
name:PowerErr_ResumeOnResetFailed

Unable to reset a device on a warm resume 

Action: none.
*/
    PowerErr_ResumeOnResetFailed,
    "PowerErr_ResumeOnResetFailed",
    NULL,

/*
name:HubErr_PortChangeStorm

Port generating spurious changes

Action: none.
*/
    HubErr_PortChangeStorm,
    "HubErr_PortChangeStorm",
    UsbhQueueHardReset,

};

#ifndef HUB_KDEXTS



__drv_aliasesMem
__drv_allocatesMem(Mem)
VOID
UsbhException(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    HUB_EXCEPTION_CLASS Class,
    PVOID ExceptionData,
    ULONG ExceptionDataLength,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus,
    PCSTR SzFile,
    ULONG Line,
    BOOLEAN HardwareReset
    )
/*++

Routine Description:

   Generates a hub exception due to hardware fault.

   If we are not at passive level this routine queues a workitem to
   handle the exception so that the handler may be called at passive level.

Arguments:

    Class - class code for the exception

    ExceptionData - data stored with the record.

    File and Line - help us know where the error occurred.

Irql: Any

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_EXCEPTION_RECORD hubExr;
    NTSTATUS nts;
    PUCHAR pch;
    ULONG szLineLength, totalLength;
    PCHUB_EXCEPTION_DISPATCH hubExDispatch;
    extern ULONG UsbhDebugLogEnable;
    PDEBUG_LOG log;
    USHORT exrDataLength;

    hubExt = FdoExt(HubFdo);

    hubExDispatch = UsbhGetExceptionDispatch(HubFdo, Class);

    UsbhEtwLogHubException(HubFdo, PortNumber, Class, NtStatus, UsbdStatus, 
                           hubExDispatch->DebugText, ((ULONG)strlen(hubExDispatch->DebugText) + 1) * sizeof(CHAR) );

    if (HardwareReset) {
        // Hub hardware reset is indicated, see if we know about it.
        if (hubExt->FdoFlags.HubHardResetPending) {
            // skip logging the error -- we know we are in trouble
            return;
        }
        hubExt->FdoFlags.HubHardResetPending = 1;
    }

    // limit exception data to 4k
    // (prefast fix)
    if (ExceptionDataLength > MAXUSHORT) {
        exrDataLength = MAXUSHORT;
    } else {
        exrDataLength = ((USHORT) ExceptionDataLength);
    }
    if (exrDataLength > 4096) {
        exrDataLength = 4096;
    }

    LOG(HubFdo, LOG_EX, 'hex1', ExceptionData,  exrDataLength);


    UsbhAssert(HubFdo, hubExt->FdoFlags.HubExAdd);
    if (!hubExt->FdoFlags.HubExAdd) {
        // we cannot track exceptions before we init
        return;
    }

    // allocate the exception record data structure, attempt to use just one
    // allocated block
    szLineLength = (ULONG)strlen(SzFile)+1;

    totalLength = sizeof(HUB_EXCEPTION_RECORD) +
                       exrDataLength +
                       szLineLength;

    UsbhAllocatePool_Z(pch, NonPagedPool, totalLength);

    // if no memory for exception then no exception
    if (pch) {

        ULONG i;

        hubExr = (PHUB_EXCEPTION_RECORD) pch;
        hubExr->TotalLength = totalLength;
        // first take a reference on the hub so it does not disappear
        // while we are handling the exception
        nts = UsbhReferenceListAdd(HubFdo, hubExr, hEXR);

        if (NT_SUCCESS(nts)) {
            hubExr->Sig = SIG_HUBEXR;
            hubExr->HubFdo = HubFdo;

            hubExr->DataLength = exrDataLength;

            if (hubExr->DataLength) {
                hubExr->Data = pch + sizeof(struct _HUB_EXCEPTION_RECORD);
                RtlCopyMemory(hubExr->Data,
                              ExceptionData,
                              hubExr->DataLength);
            }

            hubExr->SzFile = pch + sizeof(struct _HUB_EXCEPTION_RECORD) + exrDataLength;

            RtlCopyMemory(hubExr->SzFile,
                          SzFile,
                          szLineLength);
            hubExr->Line = Line;

            hubExr->UsbdStatus = UsbdStatus;
            hubExr->NtStatus = NtStatus;

            hubExr->Class = Class;
            hubExr->PortNumber = PortNumber;

            KeQuerySystemTime(&hubExr->EventTime);

            // attempt capture a piece of the current log
            log = &hubExt->Log;

            if (log->LogStart != NULL) {

                PLOG_ENTRY pLogE;
                ULONG ilog;

                ilog = log->LogIdx;

                for (i=0; i< USBHUB_LOG_CATPURE; i++) {
                    ilog &= log->LogSizeMask;
                    pLogE = log->LogStart+ilog;

                    UsbhAssert(NULL, pLogE <= log->LogEnd);

                    ilog++;
                    hubExr->DebugLog[i] = *pLogE;
                }
            }

            if (HubG.GlobalDebugLevel > 0) {
               UsbhKdPrint((0, "Hub Exception %x  DevObj(FDO) %x\n",
                   hubExr,
                   HubFdo));
               if (hubExDispatch) {
                   UsbhKdPrint((0, "(%d) %s\n", hubExDispatch->Class,
                       hubExDispatch->DebugText));
               }

               // dump some record contents to debugger
               UsbhKdPrint((0, "@file %s  line %d\n", hubExr->SzFile,
                   hubExr->Line));
               UsbhKdPrint((0, "port #%d (0=no port)\n", hubExr->PortNumber));
               UsbhKdPrint((0, "usbd status = %x nt status = %x\n",
                   hubExr->UsbdStatus, hubExr->NtStatus));
               UsbhKdPrint((0, "DataLength = %d data @ %x\n",
                          hubExr->DataLength, hubExr->Data));
            }

            UsbhExceptionTrace(HubFdo, hubExr, hubExDispatch);

            //
            // if debug breaking is enabled for hw exceptions then
            // break in the debugger on checked builds since this
            // is a hardware error and it is probably interesting.
            //


            // best if we break n at the point the error occurred.
            if (HubG.GlobalDebugBreakOnHubException) {

              UsbhKdPrint((0, "*> dt usbhub!_HUB_EXCEPTION_RECORD %p OR\n",
                      hubExr));
              UsbhKdPrint((0, "*> !_hubex %p\n",
                      hubExr));
              NT_ASSERTMSG("Hub Exception",FALSE);
            }

            if (KeGetCurrentIrql() > PASSIVE_LEVEL ||
                // if exception has a handler queue a workitem so that
                // we don't notify user mode in the context of the caller.
                (hubExDispatch && hubExDispatch->Handler)) {

                nts = UsbhQueueWorkItem(HubFdo,
                                        DelayedWorkQueue,
                                        UsbhExceptionWorker,
                                        hubExr,
                                        PortNumber,
                                        SIG_EXR_WORKER);
                if (NT_ERROR(nts)) {
                    // if we cannot queue it we just skip it
                    UsbhReferenceListRemove(HubFdo, hubExr);

                    UsbhFreePool(hubExr);
                    return;
                }

            } else {
                // we can run it now
                UsbhExceptionWorker(HubFdo, PortNumber, hubExr);
            }
        }else {
            // could not ref hub, this means we tried to throw an exception
            // after remove was processed or during a failed start.
            // We need to free memory here or we'll leak.
            UsbhFreePool(pch);
        }
    }

}


VOID
UsbhExceptionTrace(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD HubExr,
    PCHUB_EXCEPTION_DISPATCH HubExDispatch
    )
/*++

Description:

    This routine prints the exception to the trace logger

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{

    if (HubExr == NULL ||
        HubExDispatch == NULL) {
        return;
    }

    DbgTrace((UsbhDebugTrace,"<HUB_EXCEPTION_RECORD>\n"));
    DbgTrace((UsbhDebugTrace,"Hub Exception %p  DevObj(FDO) %p\n",HubExr,HubFdo));

    DbgTrace((UsbhDebugTrace,"(%d) %s\n", HubExDispatch->Class,
           HubExDispatch->DebugText));

    // dump the record contents to debugger
    DbgTrace((UsbhDebugTrace,"@file %s  line %d\n", (PCSTR)HubExr->SzFile,
       HubExr->Line));
    DbgTrace((UsbhDebugTrace,"port #%d (0=no port)\n", HubExr->PortNumber));
    DbgTrace((UsbhDebugTrace,"usbd status = %08.8x nt status = %!STATUS!\n",
       HubExr->UsbdStatus,  HubExr->NtStatus));
    DbgTrace((UsbhDebugTrace,"DataLength = %d data @ %p\n",
              HubExr->DataLength, HubExr->Data));
    DbgTrace((UsbhDebugTrace,"Data: %!HEXDUMP!\n", log_xstr(HubExr->DataLength, HubExr->Data)));

    // special handling
    switch (HubExDispatch->Class) {
    case CreateDeviceFailure:
        {
            PUSB_CD_ERROR_INFORMATION cdError;
            PUCHAR p;

            cdError = (PUSB_CD_ERROR_INFORMATION) HubExr->Data;
            if (cdError) {
                DbgTrace((UsbhDebugTrace,"<USB_CD_ERROR_INFORMATION>\n"));
                p = &cdError->XtraInfo[0];
                DbgTrace((UsbhDebugTrace,"Version %d\n", cdError->Version));
                DbgTrace((UsbhDebugTrace,"PathError %d, %!USBCDERR! \n",cdError->PathError,cdError->PathError));

                DbgTrace((UsbhDebugTrace,"Arg1=%x Arg2=%x (Transaction)nt status=%x\n",cdError->UlongArg1,cdError->UlongArg2,cdError->NtStatus));
                DbgTrace((UsbhDebugTrace,"XtraInfo: %!HEXDUMP!\n",log_xstr(64, p)));
            }
        }
        break;
    case InitializeDeviceFailure:
        break;
    }

}


PCHUB_EXCEPTION_DISPATCH
UsbhGetExceptionDispatch(
    PDEVICE_OBJECT HubFdo,
    HUB_EXCEPTION_CLASS Class
    )
/*++

Description:

    Given the class returns the appropriate hub exception dispatch entry.

Arguments:

Irql: ANY

Return:

    null if no dispatch entry is found.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PCHUB_EXCEPTION_DISPATCH hubExDispatch;
    ULONG i;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_EX, 'exD?', 0,  Class);

    //
    // find the exception entry in the table
    //
    UsbhAssert(HubFdo, Class < ExceptionClassLast);
    hubExDispatch = NULL;
    // find the class code in the table so we don't have to worry about
    // ordering
    for (i=0; i< ExceptionClassLast; i++) {
        if (Class == HubExceptionTable[i].Class) {
            hubExDispatch = &HubExceptionTable[i];
            break;
        }
    }

    return hubExDispatch;
}


VOID
UsbhExceptionWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID HubExceptionRecord
    )
/*++

Description:

    Passive level handler for a hub exception.

    This routine takes some standard actions -- common to all exceptions then
    passes it on to the class specific handler

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PHUB_EXCEPTION_RECORD hubExr = HubExceptionRecord;
    ULONG i;
    extern ULONG USBHUB_UseIoErrorLog;
    extern PDRIVER_OBJECT UsbhDriverObject;
    PCHUB_EXCEPTION_DISPATCH hubExDispatch;
    PHUB_EXCEPTION_RECORD overflow_exr;

    UsbhAssertPassive();

    UsbhAssert(HubFdo, hubExr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_EX, 'exWW', 0,  hubExr);

    //
    // find the exception entry in the table
    //
    UsbhAssert(HubFdo, hubExr->Class < ExceptionClassLast);
    hubExDispatch = UsbhGetExceptionDispatch(HubFdo, hubExr->Class);

    // find the class code in the table so we don't have to worry about
    // ordering
    for (i=0; i< ExceptionClassLast; i++) {
        if (hubExr->Class == HubExceptionTable[i].Class) {
            hubExDispatch = &HubExceptionTable[i];
            break;
        }
    }
    // spew some debug info



    if (HubG.GlobalUseIoErrorLog) {
        UsbhWriteExceptionToIoErrorLog(HubFdo, hubExr);
    }

    if (hubExDispatch) {

        // call the handler if there is one
        if (hubExDispatch->Handler) {
            hubExDispatch->Handler(HubFdo, hubExr);
        }

    } else {
        // did we miss something?
        UsbhKdPrint((0, "Missing Exception Definition %d", hubExr->Class));
        UsbhAssertMsg(HubFdo, "Missing Exception Definition", FALSE);
    }

    // keep track of these for debugging, we free them on remove
    overflow_exr = UsbhLogException(HubFdo, hubExr);

    // deref the hub, allow remove to continue. The hub driver won't initiate
    // module cleanup (where these get deleted) until all outstanding refs have
    // run

    // NOTE: individual hubExr workitems may still hold ref to FDO
    UsbhReferenceListRemove(HubFdo, hubExr);
    // if this Is the last ref then at this point UsbhExRemove can run
    // and flush any exrs we have on the list

    if (overflow_exr) {
        UsbhFreePool(overflow_exr);
    }
}


VOID
UsbhDeviceOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    Generates the WMI event that pops up the overcurrent UI on a hub
    exception.

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhDeviceOvercurrentPopup(HubFdo, Exr->PortNumber);

}


VOID
UsbhEnumFailurePopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    Standard enumeration failure routine.  This routine generates the WMI event
    that results in a WMI popup 'device failed enumeration'

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification = NULL;
    NTSTATUS nts;

    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Exr->PortNumber);

    //
    // If a device is internal (hub is described in ACPI and port is
    // marked as non-removable), we should supress the popup UI to
    // avoid confusing the user.  
    //
    if ((hubExt->ExtendedPortAttributes[Exr->PortNumber].DeviceNotRemovable) &&
        (hubExt->FdoFlags2.InAcpiNamespace)) {
        return;
    }
    
    notification = UsbhBuildWmiConnectionNotification(HubFdo,
                                                      Exr->PortNumber);

    if (notification) {
        // set information
        notification->NotificationType = EnumerationFailure;

        // return the exception class code as the 'failreason'.  Previous
        // versions of the OS did not use this field so we are free to define
        // it.
        notification->EnumerationFailReason = Exr->Class;

        // attempt to trigger the event
        nts = WmiFireEvent(HubFdo,
                           (LPGUID)&GUID_USB_WMI_STD_NOTIFICATION,
                           0,
                           sizeof(struct _USB_CONNECTION_NOTIFICATION),
                           notification);

        LOG(HubFdo, LOG_EX, 'pop1', nts, notification);
    }

}


VOID
UsbhNotEnoughPowerPopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    popup the not-enough-power message

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification;
    NTSTATUS nts;
    PHUB_PORT_DATA pd;

    LOG(HubFdo, LOG_EX, 'nPWR', Exr, 0);
    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Exr->PortNumber);

    notification = UsbhBuildWmiConnectionNotification(HubFdo,
                                                      Exr->PortNumber);

    if (notification) {
        // set information
// spelling errors in usbioctl.h
#define InsufficientPower    InsufficentPower
        notification->NotificationType = InsufficientPower;
        pd = UsbhGetPortData(HubFdo, Exr->PortNumber);
        if (pd) {
            notification->PowerRequested = pd->LastPowerRequested;
        }

        // attempt to trigger the event
        LOG(HubFdo, LOG_EX, 'wmiP', Exr, notification);
        nts = WmiFireEvent(HubFdo,
                           (LPGUID)&GUID_USB_WMI_STD_NOTIFICATION,
                           0,
                           sizeof(struct _USB_CONNECTION_NOTIFICATION),
                           notification);
    }
}


VOID
UsbhLegacyDevicePopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    Popup the high speed device in full speed hub message.

    The history of this message is somewhat convoluted.  Earlier implementations
    generated the message at enumeration time whenever a HS device was detected
    on a FS bus.  I have revised this behavior somewhat to reduce some of the
    noise associated with this event.


    This message will
    appear if a HS devices is started at non high speed.

    This event is only generated under the following conditions:

        1.  The device has received a IRP_MN_START_DEVICE
        2.  The device is NOT attached to a FS bus that is a CC for a USB 2 controller.
        3.  The device is capable of High Speed.

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification;
    NTSTATUS nts;

    LOG(HubFdo, LOG_EX, 'lUSB', Exr, 0);
    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Exr->PortNumber);

    notification = UsbhBuildWmiConnectionNotification(HubFdo,
                                                      Exr->PortNumber);

    if (notification) {
        // set information
        notification->NotificationType = ModernDeviceInLegacyHub;

        // attempt to trigger the event
        LOG(HubFdo, LOG_EX, 'wmiL', Exr, notification);
        nts = WmiFireEvent(HubFdo,
                           (LPGUID)&GUID_USB_WMI_STD_NOTIFICATION,
                           0,
                           sizeof(struct _USB_CONNECTION_NOTIFICATION),
                           notification);
    }
}


VOID
UsbhNotEnoughBandwidth_Popup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    popup the not-enough-bandwidth message

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification;
    NTSTATUS nts;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Exr->PortNumber);

    notification = UsbhBuildWmiConnectionNotification(HubFdo,
                                                      Exr->PortNumber);

    if (notification) {
        // set information
#define InsufficientBandwidth  InsufficentBandwidth

        notification->NotificationType = InsufficientBandwidth;
        pd = UsbhGetPortData(HubFdo, Exr->PortNumber);
        if (pd) {
            notification->RequestedBandwidth = pd->LastBandwidthRequested;
        }

        // attempt to trigger the event
        nts = WmiFireEvent(HubFdo,
                           (LPGUID)&GUID_USB_WMI_STD_NOTIFICATION,
                           0,
                           sizeof(struct _USB_CONNECTION_NOTIFICATION),
                           notification);
    }
}



PHUB_EXCEPTION_RECORD
UsbhLogException(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++

Description:

    Keeps track of all exception records in the debug version of the stack.

    Also prints information to the trace logger if it is running.

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG exrCount;
    KLOCK_QUEUE_HANDLE lh;
    PHUB_EXCEPTION_RECORD overflow_exr = NULL;

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    // it doesn't do much good to call this before we've initialized
    UsbhAssert(HubFdo, hubExt->FdoFlags.HubExAdd);

    KeAcquireInStackQueuedSpinLock(&hubExt->ExceptionListSpin,
                                   &lh);

    InsertTailList(&hubExt->ExceptionList,
                   &Exr->ExrLink);

    exrCount = InterlockedIncrement(&hubExt->ExceptionCounter);

    if (exrCount > HubG.GlobalExceptionLogLimit) {
        // if we are over the quota then pull an entry from the head and discard it

        // the cleanup routine could run concurrently with this so we need to check for
        // an empty list.

        // since we just put one entry on we should be able to remove it

        if (!IsListEmpty(&hubExt->ExceptionList)) {
            PLIST_ENTRY le;

            le = RemoveHeadList(&hubExt->ExceptionList);

            overflow_exr = (PHUB_EXCEPTION_RECORD) CONTAINING_RECORD(
                le,
                struct _HUB_EXCEPTION_RECORD,
                ExrLink);

            UsbhAssert(HubFdo, overflow_exr->Sig == SIG_HUBEXR);
            //UsbhFreePool(overflow_exr);

            InterlockedDecrement(&hubExt->ExceptionCounter);
        }

        KeReleaseInStackQueuedSpinLock(&lh);
    } else {
        KeReleaseInStackQueuedSpinLock(&lh);
    }

    return overflow_exr;
}


VOID
UsbhExAddDeviceInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Allocs what we need for logging exceptions

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    hubExt->FdoFlags.HubExAdd = 1;

    LOG(HubFdo, LOG_EX, 'exAL', 0, 0);

    InitializeListHead(&hubExt->ExceptionList);
    KeInitializeSpinLock(&hubExt->ExceptionListSpin);
}


VOID
UsbhExRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Description:

    Frees any exception records we are keeping track of

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PLIST_ENTRY le;
    PHUB_EXCEPTION_RECORD exr;
    PDEVICE_EXTENSION_HUB  hubExt;
    KLOCK_QUEUE_HANDLE lh;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_EX, 'exFR', 0, 0);

    if (!hubExt->FdoFlags.HubExAdd) {
        return;
    }
    hubExt->FdoFlags.HubExAdd = 0;


    KeAcquireInStackQueuedSpinLock(&hubExt->ExceptionListSpin,
                                   &lh);

    while (!IsListEmpty(&hubExt->ExceptionList)) {
        le = RemoveHeadList(&hubExt->ExceptionList);

        exr = (PHUB_EXCEPTION_RECORD) CONTAINING_RECORD(
                le,
                struct _HUB_EXCEPTION_RECORD,
                ExrLink);

        UsbhAssert(HubFdo, exr->Sig == SIG_HUBEXR);

        UsbhFreePool(exr);
    }

    KeReleaseInStackQueuedSpinLock(&lh);
}


VOID
UsbhWriteExceptionToIoErrorLog(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD HubExr
    )
/*++

Description:

    Dumps an exception to the IoError Log

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    ULONG entrySize;
    USHORT n;
    PIO_ERROR_LOG_PACKET ioError;

    LOG(HubFdo, LOG_EX, 'ioe1', HubExr,  0);

    // the error log entry is puny so we have to record it in stages

    // first record the exception

    n = (USHORT)(sizeof(struct _HUB_EXCEPTION_RECORD)/sizeof(ULONG)+1);

    entrySize = sizeof(struct _IO_ERROR_LOG_PACKET) +
        n * sizeof(ULONG);

    if (entrySize <= ERROR_LOG_MAXIMUM_SIZE) {
        ioError = IoAllocateErrorLogEntry(HubFdo,
                                          (UCHAR) entrySize);
    } else {
        ioError = NULL;
    }

    if (ioError) {
        LOG(HubFdo, LOG_EX, 'ioe2', HubExr,  ioError);

        // translate exception to an ioError
        ioError->MajorFunctionCode = IRP_MJ_PNP;
        ioError->SequenceNumber = 1;
        // use the entire exception record as 'dumpdata'
        ioError->DumpDataSize = n * sizeof(ULONG);
        ioError->NumberOfStrings = 0;
        ioError->StringOffset = 0;
        // this is the error code the event viewer uses for
        // the event ID
        ioError->ErrorCode = 0xc0000001; //HubExr->NtStatus;
        ioError->UniqueErrorValue = HubExr->Class;
        ioError->EventCategory = 0;

        ioError->FinalStatus = HubExr->NtStatus;

        RtlCopyMemory(&ioError->DumpData[0],
                      HubExr,
                      sizeof(struct _HUB_EXCEPTION_RECORD));

        IoWriteErrorLogEntry(ioError);

    }

}

VOID
UsbhNestedHubsPopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    )
/*++
Description:

    Popup the too many nested hubs message

    This message will
    appear if a hub is connected and the topology has more than 5 hubs deep

    This event is only generated under the following conditions:

        1.  The connected hub has determined that there are too many hubs and failed start
        2.  The connected hub has recorded its start failure

Arguments:

Irql: PASSIVE

Return:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification;
    NTSTATUS nts;

    LOG(HubFdo, LOG_EX, 'lUSB', Exr, 0);
    UsbhAssertPassive();

    UsbhAssert(HubFdo, Exr->Sig == SIG_HUBEXR);
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Exr->PortNumber);

    notification = UsbhBuildWmiConnectionNotification(HubFdo,
                                                      Exr->PortNumber);

    if (notification) {
        // set information
        notification->NotificationType = HubNestedTooDeeply;

        // attempt to trigger the event
        LOG(HubFdo, LOG_EX, 'wmiL', Exr, notification);
        nts = WmiFireEvent(HubFdo,
                           (LPGUID)&GUID_USB_WMI_STD_NOTIFICATION,
                           0,
                           sizeof(struct _USB_CONNECTION_NOTIFICATION),
                           notification);
    }
}

#endif //HUB_KDEXTS

