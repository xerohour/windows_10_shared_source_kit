/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubmisc.c

Abstract:

    Miscellaneous helper functions.

Author:

Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "hubmisc.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, HUBMISC_InitializeHsm)
#pragma alloc_text (PAGE, HUBMISC_MarkDeviceAsNotSupportingMSOSDescriptor)
#pragma alloc_text (PAGE, HUBMISC_StoreDeviceMSOSVendorCodeInRegsitry)
#pragma alloc_text (PAGE, HUBMISC_QueryAndCacheRegistryValuesForDevice)
#pragma alloc_text (PAGE, HUBMISC_SetExtPropDescSemaphoreInRegistry)
#pragma alloc_text (PAGE, HUBMISC_ParseAndWriteCustomPropertiesInRegistry)
#pragma alloc_text (PAGE, HUBMISC_GenerateUserNotificationForPersistentOverCurrent)
#pragma alloc_text (PAGE, HUBMISC_WerReportWorkItem)
#pragma alloc_text (PAGE, HUBMISC_VerifyCallerIsAdmin)
#pragma alloc_text (PAGE, HUBMISC_ReportPnPFailureProblem)
#pragma alloc_text (PAGE, HUBMISC_SetSqmEnumerationData)
#endif

#define ENDPOINT_ADDR_WITHOUT_DIRECTION(EA)  (EA & USB_ENDPOINT_ADDRESS_MASK)
#define BIT_TEST_ENDPOINT_MAP(X, EA)         ((X >> (ENDPOINT_ADDR_WITHOUT_DIRECTION(EA))) & 1)
#define BIT_SET_ENDPOINT_MAP(X, EA)          (X | (1 << (ENDPOINT_ADDR_WITHOUT_DIRECTION(EA))))

typedef struct _MESSAGE_RESOURCE_ENTRY {
    USHORT Length;
    USHORT Flags;
    UCHAR Text[ 1 ];
} MESSAGE_RESOURCE_ENTRY, *PMESSAGE_RESOURCE_ENTRY;

NTSYSAPI
NTSTATUS
NTAPI
RtlFindMessage (
    __in PVOID DllHandle,
    __in ULONG MessageTableId,
    __in ULONG MessageLanguageId,
    __in ULONG MessageId,
    __out PMESSAGE_RESOURCE_ENTRY *MessageEntry
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlFormatMessage (
    __in PWSTR MessageFormat,
    __in ULONG MaximumWidth,
    __in BOOLEAN IgnoreInserts,
    __in BOOLEAN ArgumentsAreAnsi,
    __in BOOLEAN ArgumentsAreAnArray,
    __in va_list *Arguments,
    __out_bcount_part(Length, *ReturnLength) PWSTR Buffer,
    __in ULONG Length,
    __out_opt PULONG ReturnLength
    );


#if DBG

//
// Test flag to set in the debugger to cause a 0x9F bugcheck in USBHUB3
//
volatile BOOLEAN g_TriggerPowerStateFailureBugcheck = 0;

//
// Test flag which can be set in the debugger to cause live dump creation
// Once set, disable/enable the XHCI controller. The dumps will be created
// in \Windows\LiveKernelReports
//
volatile BOOLEAN g_TriggerLiveDump = 0;
volatile HUB_WER_REPORT_TYPE g_TriggerLiveDumpReportType = HubWerSuccessfulReset;

#endif


#define USBPORT_BAD_HANDLE ((PVOID)(-1))

const CHAR DsmTag[]        = "DSM Tag";
const CHAR DsmCreateTag[]  = "DSM Create Tag";

extern PSTATE_ENTRY HSMStateTable[];
extern PDRIVER_OBJECT g_Usbhub3DriverObject;
extern CHAR PsmTag[];


_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBMISC_InitializeHsm (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Initializes HSM components of hub FDO context

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;

    PAGED_CODE();

    TRY {

        HubFdoContext->HsmContext.ParentContext = HubFdoContext;

        //
        // Create our pre-allocated request for hub control transfers
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = (HubFdoContext->HubFdoDevice);

        status = WdfRequestCreate(&attributes,
                                  WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice),
                                  &HubFdoContext->HubControlRequest.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate for Control Transfer Failed %!STATUS!",
                       status);
            LEAVE;
        }

        HubFdoContext->HubControlRequest.Irp =
            WdfRequestWdmGetIrp(HubFdoContext->HubControlRequest.WdfRequest);

        //
        // Create our pre-allocated request for Status Change interrupt transfers
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = (HubFdoContext->HubFdoDevice);

        status = WdfRequestCreate(&attributes,
                                  WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice),
                                  &HubFdoContext->StatusChangeInterruptRequest.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate for Interrupt Transfer Failed %!STATUS!",
                       status);
            LEAVE;
        }


        KeInitializeSpinLock(&HubFdoContext->HsmContext.StateMachineLock);

        //
        // Initialize the HSM PnP/Power Event
        //
        KeInitializeEvent(&HubFdoContext->PnPPowerEvent, NotificationEvent, FALSE);

        //
        // Set initial state for HSM
        //
        HubFdoContext->HsmContext.CurrentState[0].HsmState = StartIndexHsm;
        HubFdoContext->HsmContext.CurrentStateDepth = 0;

        if (HubFdoContext->HubParentInterface.HubDepth == 0) {

            HubFdoContext->HsmContext.SubSmFlags = HsmSubSmFlagHubRoot;

        } else {

            switch(HubFdoContext->HubParentInterface.HubSpeed) {
            case UsbHighSpeed:
            case UsbFullSpeed:
                HubFdoContext->HsmContext.SubSmFlags = HsmSubSmFlagHub20;
                break;

            case UsbSuperSpeed:
                HubFdoContext->HsmContext.SubSmFlags = HsmSubSmFlagHub30;
                break;

            default:
                NT_ASSERTMSG("Invalid hub speed", FALSE);
                break;
            }
        }

        HubFdoContext->HsmContext.StateTable = HSMStateTable;
        HubFdoContext->HsmContext.StartIndex = StartIndexHsm;
        HubFdoContext->HsmContext.ParentHandle = HubFdoContext->HubParentInterface.Hub;
        HubFdoContext->HsmContext.HubFdoContext = HubFdoContext;


        //
        // Create the HSM timer used for hub reset and overcurrent timing
        //
        HubFdoContext->HsmContext.Timer =
            ExAllocateTimer(HUBMISC_HubEventTimer,
                            HubFdoContext,
                            EX_TIMER_HIGH_RESOLUTION);

        if (HubFdoContext->HsmContext.Timer == NULL) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "HUBTIMER_AllocateTimer Failed");
            LEAVE;
        }

        //
        // Create the worker item for the state machine that will be used to
        // execute state entry functions that require passive level
        //
        HubFdoContext->HsmContext.SmWorker =
            HUBMISC_AllocateWorkerItem(HubFdoContext,
                                       UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY);

        if (HubFdoContext->HsmContext.SmWorker == NULL) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "HUBMISC_AllocateWorkerItem Failed");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

VOID
HUBMISC_MarkDeviceAsNotSupportingMSOSDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sets a flag in the DSM context indicating the device does not support the
    MS OS descriptor.  Also sets the OSVC flag for the device in the UsbFlags
    registry key indicating the device does not support the MS OS descriptor.

Returns:

    VOID

--*/
{
    USHORT value;

    PAGED_CODE();

    TRY {

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, MsOsDescriptorNotSupported);
        value = 0;

        HUBREG_AssignUsbflagsValueForDevice(DeviceContext,
                                            &g_MsOsVendorCode,
                                            (PUCHAR)&value,
                                            sizeof(value));

    } FINALLY {

    }
}

VOID
HUBMISC_StoreDeviceMSOSVendorCodeInRegsitry (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Stores MSOS vendor code fro the device in the regsitry.

Returns:

    VOID

--*/
{
    UCHAR osvcContext[2];

    PAGED_CODE();

    TRY {

        osvcContext[0] = 1;
        osvcContext[1] = DeviceContext->MsOsVendorCode;
        HUBREG_AssignUsbflagsValueForDevice(DeviceContext,
                                            &g_MsOsVendorCode,
                                            &osvcContext[0],
                                            sizeof(osvcContext));
    } FINALLY {

    }
}

VOID
HUBMISC_MarkDeviceAsNotSupportingContainerId (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sets a flag in the DSM context indicating the device does not support the
    Container Id.  Also set flag for the device in the UsbFlags
    registry key indicating the device does not support Container Id.

Returns:

    VOID

--*/
{
    USHORT   skipContainerID;
    TRY {

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_HACKS, &DeviceContext->DeviceHackFlags, SkipContainerIdQuery);
        skipContainerID = 1;

        HUBREG_AssignUsbflagsValueForDevice(DeviceContext,
                                            &g_SkipContainerIdQuery,
                                            (PUCHAR)&skipContainerID,
                                            sizeof(skipContainerID));

    } FINALLY {

    }
}


VOID
HUBMISC_StartEnumRetryTimer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Start Wdf timer after which to retry enumeration. The expiration routine
    for the timer should queue DsmEventTimerFired event in to DSM.

Returns:

    VOID

--*/
{
    TRY {
        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                   TL_WARNING,
                   Device,
                   "Starting %d ms Enumeration Retry timer for device 0x%p", 
                   DEVICE_ENUMERATION_RETRY_TIMEOUT, DeviceContext);

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_ENUMERATION_RETRY_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}


_Use_decl_annotations_
VOID
HUBMISC_PsmEventTimer (
    PEX_TIMER           Timer,
    PVOID               Context
    )

/*++

Routine Description:

    Timer callback function for port timeouts

    Sends PsmEventTimerFired to the PSM

Arguments:

    Dpc -

--*/

{
    PPORT_CONTEXT       portContext;
    
    _Analysis_assume_(Context != NULL);
    UNREFERENCED_PARAMETER(Timer);

    portContext = (PPORT_CONTEXT)Context;

    portContext->PsmContext.AddEvent(portContext, PsmEventTimerFired);

#if DBG
        if (g_TriggerLiveDump) {
            HUBMISC_InduceLiveDumpCreation(portContext->HubFdoContext, /*BugCheckSubReason*/ 0);
        }
#endif

}

VOID
HUBMISC_PsmResetCompletePollingTimer (
    __in
        WDFTIMER           Timer
    )
/*++

Routine Description:

    Timer callback function for port reset polling timeout

    Sends PsmEventPortResetPollingTimerFired to the PSM

Return Value:

    VOID

--*/
{
    WDFOBJECT               psmObject;
    PPORT_CONTEXT           portContext;

    TRY {

        psmObject = WdfTimerGetParentObject(Timer);

        portContext = GetPortContext(psmObject);

        portContext->PsmContext.AddEvent(portContext, PsmEventPortResetPollingTimerFired);

    } FINALLY {

    }
}



_Use_decl_annotations_
VOID
HUBMISC_HubEventTimer (
    PEX_TIMER           Timer,
    PVOID               Context
    )

/*++

Routine Description:

    Completion routine for high resolution timer

Arguments:

    Dpc -

--*/

{
    _Analysis_assume_(Context != NULL);
    UNREFERENCED_PARAMETER(Timer);

    HUBSM_AddHsmEvent((PHUB_FDO_CONTEXT)Context, HsmEventTimerFired);
}


_Use_decl_annotations_
VOID
HUBMISC_DsmEventTimer (
    PEX_TIMER           Timer,
    PVOID               Context
    )

/*++

Routine Description:

    Completion routine for high resolution timer

Arguments:

    Dpc -

--*/

{
    _Analysis_assume_(Context != NULL);
    UNREFERENCED_PARAMETER(Timer);

    HUBSM_AddDsmEvent((PDEVICE_CONTEXT)Context, DsmEventTimerFired);
}


DSM_EVENT
HUBMISC_StopTimer (
    __in PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Stop the current WDF timer.

    If WdfTimerStop returns true, then return DsmEventOperationSuccess,
    otherwise return DsmEventOperationFailure.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT       dsmEvent;
    BOOLEAN         wasInQueue;

    TRY {

        wasInQueue = ExCancelTimer(DeviceContext->DsmContext.Timer, NULL);

        if (wasInQueue == TRUE ) {

            dsmEvent = DsmEventOperationSuccess;
        } else {

            dsmEvent = DsmEventOperationFailure;
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfEndpointCreateListIsNonEmpty (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;
    ULONG                   i;
    DSM_EVENT               dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        //
        // Walk the list of interfaces for this configuration and see if there are any endpoints
        // that need to be created in UCX
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                PPIPE_HANDLE    pipeHandle;

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (pipeHandle->UcxEndpointState == UcxEndpointStateNotCreated) {

                    dsmEvent = DsmEventYes;
                    LEAVE;
                }
            }
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfIProductIsZero (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        if (DeviceContext->DeviceDescriptor.iProduct == 0) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfDeviceSpeedChanged (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

   Compares the current device speed stored with the device speed
   as per the latest port status bits in the port context and determine
   if the device connected after system resume is of a different speed
   than the one that was connected before system went to sleep.

Returns:

   Yes if the device speed changed, otherwise returns No.

--*/
{

    DSM_EVENT           dsmEvent;

    TRY {

        if (DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbSuperSpeed) {
            //
            // Should only get this called for 2.x ports and lower
            //
            NT_ASSERTMSG("CheckIfDeviceSpeedChanged got invoked for SuperSpeed Devices",
                         FALSE);

            //
            // We don't know how to change speeds for a super speed port,
            // so we assume it didn't change
            //
            dsmEvent = DsmEventNo;
            LEAVE;
        }

        if (DeviceContext->PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.HighSpeedDeviceAttached) {
            if (DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbHighSpeed) {
                dsmEvent = DsmEventNo;
            } else {
                dsmEvent = DsmEventYes;
            }
        } else if (DeviceContext->PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.LowSpeedDeviceAttached) {
            if (DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbLowSpeed) {
                dsmEvent = DsmEventNo;
            } else {
                dsmEvent = DsmEventYes;
            }
        } else {
            if (DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbFullSpeed) {
                dsmEvent = DsmEventNo;
            } else {
                dsmEvent = DsmEventYes;
            }
        }

    } FINALLY {

    }

    return dsmEvent;

}


DSM_EVENT
HUBMISC_CheckIfSerialNumberStringIndexIsZero (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        if (DeviceContext->DeviceDescriptor.iSerialNumber == 0) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}


VOID
HUBMISC_StartPostResetTimerFor20 (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
/*++

Routine Description:

    Starts the post reset WDF timer.

    The expiration routine for the timer should queue
    DsmEventTimerFired to the DSM

Return Value:

    VOID

--*/
{

    TRY {

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_POST_RESET_TIMEOUT_FOR_20),
                   0,
                   NULL);


    } FINALLY {

    }
}

VOID
HUBMISC_StartPostResetTimerFor30 (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
/*++

Routine Description:

    Starts the post reset WDF timer.

    The expiration routine for the timer should queue
    DsmEventTimerFired to the DSM

Return Value:

    VOID

--*/
{

    TRY {

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_POST_RESET_TIMEOUT_FOR_30),
                   0,
                   NULL);


    } FINALLY {

    }
}

VOID
HUBMISC_StartSuperSpeedDisableTimer (
    __in
        PPORT_CONTEXT               PortContext
    )
/*++

Routine Description:


Returns:


--*/
/*++

Routine Description:

    Starts the SuperSpeed disable WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PORT_SUPERSPEED_DISABLE_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_InitializeEnumRetryCount (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    TRY {
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigDescIsValid);
        DeviceContext->EnumMsgId = 0;
        DeviceContext->EnumRetryCount = 0;
        DeviceContext->U1Timeout = 0;
        DeviceContext->U2Timeout = 0;
        DeviceContext->EffectiveExitLatency = 0;
        INTERLOCKED_CLEAR_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCompleted);
        INTERLOCKED_CLEAR_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCmdSent);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1EnabledForUSPort);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U2EnabledForUSPort);
    } FINALLY {

    }
}

VOID
HUBMISC_InitializeDuplicateSerialNumberRetryCount (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    TRY {
        DeviceContext->DuplicateSerialNumberRetryCount = 0;
    } FINALLY {

    }
}

VOID
HUBMISC_LogDeviceReEnumeration (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    TRY {

        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                   TL_ERROR,
                   Device,
                   "Device is getting re-enumerated in top-level DSM state %!DSMSTATE!. This enumeration is not visible to PnP manager.",
                   DeviceContext->DsmContext.CurrentState[0].StateAsUlong);

        USB_HW_VERIFIER_DEVICE_BREAK(DeviceContext, DeviceHwVerifierBusRenumeration);

        EventWriteUSBHUB3_ETW_EVENT_BUS_RENUMERATION(
            &DeviceContext->DsmContext.CurrentActivityId,
            DeviceContext->UsbDevice);

    } FINALLY {

    }
}


VOID
HUBMISC_StartPostResetExtendedTimer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    TRY {

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_POST_RESET_EXTENDED_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}


DSM_EVENT
HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        DeviceContext->EnumRetryCount++;

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceFailedAtLeastOneEnumeration);

        if (DeviceContext->EnumRetryCount > MAXIMUM_ENUMERATION_RETRY_COUNT) {
            dsmEvent = DsmEventYes;

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                TL_ERROR,
                Device,
                "Device Context 0x%p - ERROR: All %d enumeration attempts failed...giving up",
                DeviceContext,
                DeviceContext->EnumRetryCount);

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_MAXIMUM_RETRY_COUNT_REACHED(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);

            USB_HW_VERIFIER_PORT_BREAK(DeviceContext->PortContext, HubHwVerifierDeviceFailedEnumeration);
        } else {

            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigDescIsValid);
            DeviceContext->EnumMsgId = 0;

            HUBMISC_FreeBufferIfNotNull(DeviceContext->MsOs20Info.DescriptorSet, USBHUB3_DEVICE_TAG);
            RtlZeroMemory(&DeviceContext->MsOs20Info, sizeof(MSOS20_INFO));

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                TL_ERROR,
                Device,
                "Device Context 0x%p - Attempting enumeration retry %d",
                DeviceContext,
                DeviceContext->EnumRetryCount);
            EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_RETRY(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
            USB_HW_VERIFIER_PORT_BREAK(DeviceContext->PortContext, HubHwVerifierDeviceEnumerationRetry);
        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_IncrementAndCheckIfDuplicateSerialNumberRetryCountExceededLimit (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        DeviceContext->DuplicateSerialNumberRetryCount++;

        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                   TL_WARNING,
                   Device,
                   "Incremented DuplicateSerialNumberRetryCount to %d", DeviceContext->DuplicateSerialNumberRetryCount);

        if (DeviceContext->DuplicateSerialNumberRetryCount > DEVICE_DUPLICATE_SN_MAX_RETRY_COUNT) {
            dsmEvent = DsmEventYes;

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Duplicate device detected and is not going away. Serial number will be ignored");
        }

    } FINALLY {

    }

    return dsmEvent;
}


FORCEINLINE
VOID
HUBMISC_InitializePipe (
    __in
        PDEVICE_CONTEXT             DeviceContext,
    __in
        PUSB_ENDPOINT_DESCRIPTOR    EndpointDescriptor,
    __in
        PUSBD_PIPE_INFORMATION  Pipe
    )
/*++

Routine Description:


Returns:


--*/
{
    USB_HIGH_SPEED_MAXPACKET    muxPacket;

    TRY {

        if (DeviceContext->DeviceDescriptor.bcdUSB < 0x250) {
            muxPacket.us = EndpointDescriptor->wMaxPacketSize;
            Pipe->MaximumPacketSize = muxPacket.MaxPacket * (muxPacket.HSmux + 1);
        } else {
            Pipe->MaximumPacketSize = EndpointDescriptor->wMaxPacketSize;
        }

        //
        // The highest bInterval value we allow is 6 (period=32)
        //
        Pipe->Interval = min(EndpointDescriptor->bInterval, 6);

        Pipe->EndpointAddress = EndpointDescriptor->bEndpointAddress;

        switch (EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
        case USB_ENDPOINT_TYPE_CONTROL:
            Pipe->PipeType = UsbdPipeTypeControl;
            break;

        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            Pipe->PipeType = UsbdPipeTypeIsochronous;
            break;

        case USB_ENDPOINT_TYPE_BULK:
            Pipe->PipeType = UsbdPipeTypeBulk;
            break;

        case USB_ENDPOINT_TYPE_INTERRUPT:
            Pipe->PipeType = UsbdPipeTypeInterrupt;
            break;

        default:
            NT_ASSERTMSG("Unknown endpoint type", 0);
            break;
        }

    } FINALLY {

    }
}

_Must_inspect_result_
NTSTATUS
HUBMISC_CheckForSharedEndpoints (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __in
        PINTERFACE_HANDLE               InterfaceHandle1,
    __in
        PINTERFACE_HANDLE               InterfaceHandle2
    )
/*++

Routine Description:

    Checks for shared endpoints between 2 interfaces.

Returns:

    STATUS_SUCCESS if no shared endpoints found.

    STATUS_INVALID_PARAMETER if a shared endpoint is found.

--*/
{
    ULONG           i;
    ULONG           j;
    NTSTATUS        status;
    PPIPE_HANDLE    pipeHandle1;
    PPIPE_HANDLE    pipeHandle2;

    TRY {

        status = STATUS_SUCCESS;

        for (i=0; i<InterfaceHandle1->NumberOfEndpoints; i++) {

            pipeHandle1 = &InterfaceHandle1->PipeHandle[i];

            for (j=0; j<InterfaceHandle2->NumberOfEndpoints; j++) {

                pipeHandle2 = &InterfaceHandle2->PipeHandle[j];

                //
                // Only allow interfaces to share an endpoint if one or both of them are zero bandwidth
                //
                if ((pipeHandle1->EndpointDescriptor->bEndpointAddress ==
                     pipeHandle2->EndpointDescriptor->bEndpointAddress) &&
                    (pipeHandle1->EndpointDescriptor->wMaxPacketSize != 0) &&
                    (pipeHandle2->EndpointDescriptor->wMaxPacketSize != 0)) {

                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Endpoint address 0x%x shared between interface %d alternate setting %d and interface %d alternate setting %d!!",
                               pipeHandle1->EndpointDescriptor->bEndpointAddress,
                               InterfaceHandle1->InterfaceDescriptor->bInterfaceNumber,
                               InterfaceHandle1->InterfaceDescriptor->bAlternateSetting,
                               InterfaceHandle2->InterfaceDescriptor->bInterfaceNumber,
                               InterfaceHandle2->InterfaceDescriptor->bAlternateSetting);

                    status = STATUS_INVALID_PARAMETER;
                    LEAVE;
                }
            }
        }
    } FINALLY {
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
HUBMISC_CheckForDuplicateEndpointAddresses(
    __in
        PDEVICE_CONTEXT DeviceContext
    )
/*++

Routine Description:

    Checks if that there are interfaces that share the same endpoint
    addresses in the current device configuration.

Returns:
    
    NTSTATUS

--*/
{
    NTSTATUS                status;
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;
    USHORT                  inEndpointsMap;
    USHORT                  outEndpointsMap;
    USHORT                  controlEndpointMap;

    TRY {

        status = STATUS_SUCCESS;
        configuration = DeviceContext->CurrentDeviceConfiguration;

        inEndpointsMap  = 0;
        outEndpointsMap = 0;
        controlEndpointMap = 0;

        if (configuration == NULL) {
            LEAVE;
        }

        //
        // Walk the list of interfaces for this configuration and verify 
        // that none of the interfaces share an endpoint
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {
            ULONG i;

            //
            // Walk the endpoint handles
            //
            for (i =0; i < interfaceHandle->NumberOfEndpoints; i++) {

                PPIPE_HANDLE pipeHandle;
                PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor;
                PUSHORT      bitmap;

                bitmap = NULL;
                pipeHandle = &interfaceHandle->PipeHandle[i];
                endpointDescriptor = pipeHandle->EndpointDescriptor;

                if (pipeHandle->EndpointFlags.ZeroBandwidth == 1) {
                    //
                    // Ignore zero bandwidth endpoints
                    //
                    continue;
                }

                if ((endpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) == 
                    USB_ENDPOINT_TYPE_CONTROL) {
                    bitmap = &controlEndpointMap;
                } else if (USB_ENDPOINT_DIRECTION_IN(endpointDescriptor->bEndpointAddress)) {
                    bitmap = &inEndpointsMap;
                } else if (USB_ENDPOINT_DIRECTION_OUT(endpointDescriptor->bEndpointAddress)){
                    bitmap = &outEndpointsMap;
                }
            
                //
                // Direction bit of control endpoints is ignored. Hence also check
                // all IN and OUT endpoints with control endpoint map
                //
                if ((BIT_TEST_ENDPOINT_MAP(controlEndpointMap, endpointDescriptor->bEndpointAddress))  ||
                    (BIT_TEST_ENDPOINT_MAP(*bitmap, endpointDescriptor->bEndpointAddress))) {
                    // 
                    // Endpoint was already set in bit map
                    //
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "INVALID_CONFIGURATION_DESCRIPTOR: More than one interface is attempting to use endpoint address 0x%x in configuration 0x%p.",
                               pipeHandle->EndpointDescriptor->bEndpointAddress,
                               (PVOID) configuration);

                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                } 

                //
                // Add endpoint descriptor to bitmaps
                //
                *bitmap = BIT_SET_ENDPOINT_MAP(*bitmap, endpointDescriptor->bEndpointAddress);
            }
        }

    } FINALLY {

    }

    return (status);
}

_Must_inspect_result_
BOOLEAN
HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInCurrentConfiguration(
    __in
        PDEVICE_CONTEXT DeviceContext
    )
/*++

Routine Description:

    Checks if there is an endpoint descriptor of endpoint 0 in the current 
    device configuration.

Returns:
    
    BOOLEAN

--*/
{
    BOOLEAN                 found;
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;

    TRY {

        found = FALSE;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        if (configuration == NULL) {
            LEAVE;
        }

        //
        // Walk the list of interfaces for this configuration
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {
            
            if (HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInInterface(DeviceContext, interfaceHandle)) {
                found = TRUE;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return found;
}

_Must_inspect_result_
BOOLEAN
HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInInterface(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PINTERFACE_HANDLE   InterfaceHandle
    )
/*++

Routine Description:

    Checks if there is an endpoint descriptor of endpoint 0 in the specified 
    interface.

Returns:
    
    BOOLEAN

--*/
{
    BOOLEAN         found;
    ULONG           i;
    PPIPE_HANDLE    pipeHandle;

    UNREFERENCED_PARAMETER(DeviceContext);

    TRY {

        found = FALSE;

        for (i=0; i<InterfaceHandle->NumberOfEndpoints; i++) {

            pipeHandle = &InterfaceHandle->PipeHandle[i];

            if ((pipeHandle->EndpointDescriptor->bEndpointAddress & 0xF) == 0) {
                found = TRUE;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return found;
}


FORCEINLINE
NTSTATUS
HUBMISC_InitializeInterfacePipes (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __in
        PUSBD_INTERFACE_INFORMATION     InterfaceInformation,
    __in
        PINTERFACE_HANDLE               InterfaceHandle,
    __in_bcount(DescriptorsLength)
        PUCHAR                          Descriptors,
    __in
        size_t                          DescriptorsLength
    )
/*++

Routine Description:


Returns:


--*/
{
    PUSB_COMMON_DESCRIPTOR                                  commonDescriptor;
    PUSB_ENDPOINT_DESCRIPTOR                                endpointDescriptor;
    PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR           ssEndpointCompanionDescriptor;
    PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR sspIsochEndpointCompanionDescriptor;
    PUCHAR                                                  endDescriptors;
    ULONG                                                   i;
    NTSTATUS                                                status;

    TRY {

        status = STATUS_UNSUCCESSFUL;

        endDescriptors = Descriptors + DescriptorsLength;

        //
        // Walk the endpoint descriptors for this interface
        //
        for (i = 0; i < InterfaceInformation->NumberOfPipes; i++) {

            commonDescriptor = (PUSB_COMMON_DESCRIPTOR)Descriptors;

            if (Descriptors >= endDescriptors) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Number of endpoint descriptors is less than expected");
                LEAVE;
            }

            InterfaceHandle->PipeHandle[i].UsbdPipeFlags =
                InterfaceInformation->Pipes[i].PipeFlags;

            InterfaceHandle->PipeHandle[i].UcxEndpointState = UcxEndpointStateNotCreated;

            if ((&commonDescriptor->bDescriptorType) >= endDescriptors) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Descriptor extends past configuration descriptor's wLength value");
                LEAVE;
            }

            //
            // Skip over non-endpoint descriptors
            //
            while (commonDescriptor->bDescriptorType != USB_ENDPOINT_DESCRIPTOR_TYPE) {

                if (commonDescriptor->bLength == 0) {
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Descriptor has bLength of 0");
                    LEAVE;
                }

                Descriptors += commonDescriptor->bLength;
                commonDescriptor = (PUSB_COMMON_DESCRIPTOR)Descriptors;

                if (Descriptors >= endDescriptors) {
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Number of endpoint descriptors is less than expected");
                    LEAVE;
                }

                if ((&commonDescriptor->bDescriptorType) >= endDescriptors) {
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Descriptor extends past configuration descriptor's wLength value");
                    LEAVE;
                }
            }

            if (Descriptors >= endDescriptors) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Number of endpoint descriptors is less than expected");
                LEAVE;
            }

            endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)Descriptors;

            if (((PUCHAR)endpointDescriptor + sizeof(USB_ENDPOINT_DESCRIPTOR)) > endDescriptors) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Endpoint descriptor extends past configuration descriptor's wLength value");
                LEAVE;
            }

            //
            // Override max packet size if the client requested it
            //
            if (InterfaceInformation->Pipes[i].PipeFlags & USBD_PF_CHANGE_MAX_PACKET) {

                endpointDescriptor->wMaxPacketSize =
                    InterfaceInformation->Pipes[i].MaximumPacketSize;
            }

            //
            // Store a pointer to the endpoint descriptor
            //
            InterfaceHandle->PipeHandle[i].EndpointDescriptor = endpointDescriptor;
            InterfaceHandle->PipeHandle[i].EndpointDescriptorBufferLength
                = (ULONG)(ULONG_PTR) (endDescriptors - (PUCHAR)endpointDescriptor);
            HUBMISC_InitializePipe(DeviceContext,
                                   endpointDescriptor,
                                   &InterfaceInformation->Pipes[i]);

            if (InterfaceInformation->Pipes[i].MaximumPacketSize == 0) {
                INTERLOCKED_SET_BIT_FIELD(ENDPOINT_FLAGS, &InterfaceHandle->PipeHandle[i].EndpointFlags, ZeroBandwidth);
            }

            if (endpointDescriptor->bLength == 0) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Endpoint Descriptor has bLength of 0");
                LEAVE;
            }

            Descriptors += endpointDescriptor->bLength;

            if (Descriptors >= endDescriptors) {
                continue;
            }

            commonDescriptor = (PUSB_COMMON_DESCRIPTOR)Descriptors;

            if ((&commonDescriptor->bDescriptorType) >=  endDescriptors) {
                continue;
            }

            if (commonDescriptor->bDescriptorType != USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR_TYPE) {
                continue;
            }

            if (commonDescriptor->bLength == 0) {
                continue;
            }

            ssEndpointCompanionDescriptor = (PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR)Descriptors;

            if ((((PUCHAR)ssEndpointCompanionDescriptor) + sizeof(USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR)) > endDescriptors) {
                continue;
            }

            InterfaceHandle->PipeHandle[i].SSEndpointCompanionDescriptor = ssEndpointCompanionDescriptor;

            if ((InterfaceHandle->PipeHandle[i].EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {

                if ((InterfaceHandle->PipeHandle[i].EndpointFlags.ZeroBandwidth == 0)) {
                    //
                    // For Superspeed Isochronous endpoints, the MaxPacketSize that we will return to the clients
                    // will be the total number of bytes per interval.  This is returned in the wBytesPerInterval
                    // in the endpoint companion descriptor.  This is different than the true MaxPacketSize of the
                    // endpoint.
                    //
                    InterfaceInformation->Pipes[i].MaximumPacketSize = ssEndpointCompanionDescriptor->wBytesPerInterval;
                }

                if (ssEndpointCompanionDescriptor->bmAttributes.Isochronous.SspCompanion) {
                    Descriptors += ssEndpointCompanionDescriptor->bLength;

                    if (Descriptors >= endDescriptors) {
                        continue;
                    }

                    commonDescriptor = (PUSB_COMMON_DESCRIPTOR)Descriptors;

                    if ((&commonDescriptor->bDescriptorType) >= endDescriptors) {
                        continue;
                    }

                    if (commonDescriptor->bDescriptorType != USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR_TYPE) {
                        continue;
                    }

                    if (commonDescriptor->bLength == 0) {
                        continue;
                    }

                    sspIsochEndpointCompanionDescriptor = (PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)Descriptors;

                    if ((((PUCHAR)sspIsochEndpointCompanionDescriptor) + sizeof(USB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)) > endDescriptors) {
                        continue;
                    }

                    InterfaceHandle->PipeHandle[i].SSPIsochEndpointCompanionDescriptor = sspIsochEndpointCompanionDescriptor;
                }
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}


FORCEINLINE
VOID
HUBMISC_FreeUcxEndpointArrays (
    __in
        PDEVICE_CONTEXT                 DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{

    TRY {

        DeviceContext->UcxEndpointArrayCount = 0;

        if (DeviceContext->EndpointsToBeEnabled != NULL) {
            ExFreePoolWithTag(DeviceContext->EndpointsToBeEnabled, USBHUB3_DEVICE_TAG);
            DeviceContext->EndpointsToBeEnabled = NULL;
        }

        if (DeviceContext->EndpointsToRemainUnchanged != NULL) {
            ExFreePoolWithTag(DeviceContext->EndpointsToRemainUnchanged, USBHUB3_DEVICE_TAG);
            DeviceContext->EndpointsToRemainUnchanged = NULL;
        }

        if (DeviceContext->EndpointsToBeDisabled != NULL) {
            ExFreePoolWithTag(DeviceContext->EndpointsToBeDisabled, USBHUB3_DEVICE_TAG);
            DeviceContext->EndpointsToBeDisabled = NULL;
        }

    } FINALLY {

    }
}


FORCEINLINE
_Must_inspect_result_
NTSTATUS
HUBMISC_AllocateUcxEndpointArrays (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __in
        ULONG                           NumberOfEndpoints
    )
/*++

Routine Description:


Returns:


--*/
{
    NTSTATUS    status;

    TRY {

        status = STATUS_SUCCESS;

        DeviceContext->EndpointsToBeDisabledCount = 0;
        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        DeviceContext->EndpointsToBeEnabledCount = 0;

        //
        // Free the existing arrays of UCX endpoints if they are not large enough to
        // accomodate the endpoints in the new configuration
        //
        if ((DeviceContext->UcxEndpointArrayCount != 0) &&
            (NumberOfEndpoints > DeviceContext->UcxEndpointArrayCount)) {

            HUBMISC_FreeUcxEndpointArrays(DeviceContext);

        } else if (DeviceContext->UcxEndpointArrayCount != 0) {
            //
            // Existing endpoint array is large enough to accomodate
            // the new configuration's endpoints
            //
            LEAVE;
        }

        if ((DeviceContext->UcxEndpointArrayCount == 0) &&
            (NumberOfEndpoints != 0)) {

            //
            // Allocate an array of UCX endpoint handles for endpoints that need
            // to be created/configured in UCX
            //
            DeviceContext->EndpointsToBeEnabled =
                ExAllocatePoolWithTag(NonPagedPool,
                                      NumberOfEndpoints * sizeof(UCXENDPOINT),
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->EndpointsToBeEnabled == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            //
            // Allocate an array of UCX endpoint handles for endpoints that are
            // unchanged
            //
            DeviceContext->EndpointsToRemainUnchanged =
                ExAllocatePoolWithTag(NonPagedPool,
                                      NumberOfEndpoints * sizeof(UCXENDPOINT),
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->EndpointsToRemainUnchanged == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            //
            // Allocate an array of UCX endpoint handles for endpoints that need to
            // be deleted.
            //
            DeviceContext->EndpointsToBeDisabled =
                ExAllocatePoolWithTag(NonPagedPool,
                                      NumberOfEndpoints * sizeof(UCXENDPOINT),
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->EndpointsToBeDisabled == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            DeviceContext->UcxEndpointArrayCount = NumberOfEndpoints;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            HUBMISC_FreeUcxEndpointArrays(DeviceContext);

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate memory for UCX endpoint arrays %!STATUS!",
                       status);
        }
    }

    return status;
}


DSM_EVENT
HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectConfiguration (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    This function will be called when we want to configure the device when
    client driver sends a Select configuration request. This function will
    store the list of endpoints that needs to be purged, created, configured
    or de-configured and list of interface alternate settings that need to
    be set on device, at a particular place in the device context. It might
    return failure as there would be memory allocation involved.

    Client will(should) pass in a URB buffer that looks like this:

    +------------------------------+
    |Hdr                           |
    |(_URB_HEADER)                 |
    |    - <caller inputs>         |
    |      Function                |
    |      Length                  |
    |      UsbdDeviceHandle        |
    |                              |
    |    - <port outputs>          |
    |       Status                 |
    +------------------------------+
    |    - <caller inputs>         |
    |      ConfigurationDescriptor |
    |    - <port outputs>          |
    |      ConfigurationHandle     |
    +------------------------------+
    |Interface(0)                  |
    |(USBD_INTERFACE_INFORMATION)  |
    |    - <caller inputs>         |
    |      Length                  |
    |      InterfaceNumber         |
    |      AlternateSetting        |
    |                              |
    |    - <port  outputs>         |
    |      InterfaceHandle         |
    |      NumberOfPipes           |
    |      SubClass                |
    |      Class                   |
    |      Protocol                |
    +------------------------------+
    |Pipes[0]                      | one of these for each pipe in the
    |(USBD_PIPE_INFORMATION)       | interface
    |    - <caller inputs>         |
    |                              |
    |    - <port outputs>          |
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+
    | Interface(1)                 | one of these for each interface in
    |                              | the configuration
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    ULONG                           numberOfEndpoints;
    WDFREQUEST                      request;
    PURB                            urb;
    WDF_REQUEST_PARAMETERS          parameters;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFMEMORY                       configurationMemory;
    PUCHAR                          informationPtr;
    PUCHAR                          informationEnd;
    size_t                          bufferSize;
    PDEVICE_CONFIGURATION           newDeviceConfiguration;
    PUSBD_INTERFACE_INFORMATION     interfaceInformation;
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;
    KIRQL                           irql;
    USHORT                          wTotalLengthFromUrb;

    TRY {

        request = DeviceContext->CurrentClientRequest;
        DeviceContext->EndpointsToBeEnabledCount = 0;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        urb = parameters.Parameters.Others.Arg1;

        NT_ASSERT(urb->UrbHeader.Function == URB_FUNCTION_SELECT_CONFIGURATION);

        newDeviceConfiguration = NULL;

        //
        // Access to the device configuration interface/endpoint structures is synchronized
        // via the DSM, with the exception of the IOCTL_USB_GET_NODE_CONNECTION_INFORMATION
        // handler.  A spinlock is used to insure serialization with
        // IOCTL_USB_GET_NODE_CONNECTION_INFORMATION.
        //
        KeAcquireSpinLock(&DeviceContext->ConfigurationLock, &irql);

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);
        DeviceContext->OldDeviceConfiguration = DeviceContext->CurrentDeviceConfiguration;
        DeviceContext->CurrentDeviceConfiguration = NULL;
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, FailAlternateSetting);

        KeReleaseSpinLock(&DeviceContext->ConfigurationLock, irql);

        //
        // Allocate a new configuration info structure.  It will store the configuration
        // descriptor and the list of interfaces.
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        //
        // Let the object parent default to the driver. We will delete it explicitly.
        // Making it a child of DSM or HSM does not work because we might want to delete
        // them much sooner
        //

        wTotalLengthFromUrb = urb->UrbSelectConfiguration.ConfigurationDescriptor->wTotalLength;
        bufferSize = sizeof(DEVICE_CONFIGURATION) + wTotalLengthFromUrb - 1;
                                                                                        
        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_DEVICE_TAG,
                                 bufferSize,
                                 &configurationMemory,
                                 &DeviceContext->CurrentDeviceConfiguration);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfMemoryCreate Failed for configuration handle %!STATUS!",
                       status);
            LEAVE;
        }

        newDeviceConfiguration = DeviceContext->CurrentDeviceConfiguration;
        newDeviceConfiguration->WdfMemory = configurationMemory;

        //
        // Cache the configuration descriptor
        //
        RtlCopyMemory(&DeviceContext->CurrentDeviceConfiguration->ConfigurationDescriptor,
                      urb->UrbSelectConfiguration.ConfigurationDescriptor,
                      wTotalLengthFromUrb);

        InitializeListHead(&newDeviceConfiguration->InterfaceList);

        informationPtr = (PUCHAR)&urb->UrbSelectConfiguration.Interface;
        informationEnd = ((PUCHAR)urb) + urb->UrbSelectConfiguration.Hdr.Length;

        configurationDescriptor =
            (PUSB_CONFIGURATION_DESCRIPTOR)&newDeviceConfiguration->ConfigurationDescriptor;

        numberOfEndpoints = 0;

        //
        // Walk the array of USBD_INTERFACE_INFORMATION structures
        //
        while ((informationPtr + sizeof(USHORT)) < informationEnd) {
            BOOLEAN                     hasAlternateSettings;
            PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
            WDFMEMORY                   interfaceHandleMemory;
            PINTERFACE_HANDLE           interfaceHandle;

            interfaceInformation = (PUSBD_INTERFACE_INFORMATION)informationPtr;

            hasAlternateSettings = FALSE;

            //
            // Find the matching interface descriptor
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(configurationDescriptor,
                                                     (PVOID)configurationDescriptor,
                                                     interfaceInformation->InterfaceNumber,
                                                     interfaceInformation->AlternateSetting,
                                                     -1,
                                                     -1,
                                                     -1,
                                                     &hasAlternateSettings,
                                                     DeviceContext->PortContext->IfrLog);

            if (interfaceDescriptor == NULL) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Cannot find matching interface descriptor for URB_FUNCTION_SELECT_CONFIGURAION");

                status = STATUS_UNSUCCESSFUL;
                DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
                LEAVE;
            }

            //
            // Check if this InterfaceNumber/AlternateSetting should be filtered out
            //
            if (DeviceContext->AlternateSettingFilter != NULL) {
                ULONG                       i;
                PALTERNATE_SETTING_FILTER   alternateSettingFilter;

                alternateSettingFilter = DeviceContext->AlternateSettingFilter;

                for (i = 0; i < alternateSettingFilter->Count; i++) {
                    if ((alternateSettingFilter->FilterList[i].AlternateSetting == interfaceDescriptor->bAlternateSetting) &&
                        (alternateSettingFilter->FilterList[i].InterfaceNumber == interfaceDescriptor->bInterfaceNumber)) {

                        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, FailAlternateSetting);
                        break;
                    }
                }
            }

            interfaceInformation->NumberOfPipes = interfaceDescriptor->bNumEndpoints;
            numberOfEndpoints += interfaceDescriptor->bNumEndpoints;
            informationPtr += interfaceInformation->Length;

            //
            // Allocate the interface handle for this interface
            //
            WDF_OBJECT_ATTRIBUTES_INIT( &attributes );
            //
            // Let the object parent default to the driver. We will delete it explicitly.
            // Making it a child of DSM or HSM does not work because we might want to delete
            // them much sooner
            //

            bufferSize = sizeof(INTERFACE_HANDLE) -
                         sizeof(PIPE_HANDLE) +
                         (sizeof(PIPE_HANDLE) *
                          interfaceInformation->NumberOfPipes);

            status = WdfMemoryCreate(&attributes,
                                     NonPagedPool,
                                     USBHUB3_DEVICE_TAG,
                                     bufferSize,
                                     &interfaceHandleMemory,
                                     &interfaceHandle);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Driver,
                           "WdfMemoryCreate Failed for interface handle %!STATUS!",
                           status);
                LEAVE;
            }

            interfaceInformation->InterfaceHandle = interfaceHandle;

            RtlZeroMemory(interfaceHandle, bufferSize);

            interfaceHandle->MemoryObject = interfaceHandleMemory;

            interfaceHandle->InterfaceDescriptor = interfaceDescriptor;
            interfaceHandle->NumberOfEndpoints = interfaceDescriptor->bNumEndpoints;

            if (hasAlternateSettings == TRUE) {
                INTERLOCKED_SET_BIT_FIELD(INTERFACE_FLAGS, &interfaceHandle->InterfaceFlags, HasAlternateSettings);
                INTERLOCKED_SET_BIT_FIELD(INTERFACE_FLAGS, &interfaceHandle->InterfaceFlags, NeedsSetInterface);
            }

            InsertTailList(&newDeviceConfiguration->InterfaceList,
                           &interfaceHandle->InterfaceLink);

            //
            // Populate the interface information structure
            //
            interfaceInformation->Class = interfaceDescriptor->bInterfaceClass;
            interfaceInformation->SubClass = interfaceDescriptor->bInterfaceSubClass;
            interfaceInformation->Protocol = interfaceDescriptor->bInterfaceProtocol;

            bufferSize = (size_t)((PUCHAR)interfaceDescriptor + interfaceDescriptor->bLength -
                                  (PUCHAR)configurationDescriptor);

            bufferSize = urb->UrbSelectConfiguration.ConfigurationDescriptor->wTotalLength - bufferSize;

            status = HUBMISC_InitializeInterfacePipes(DeviceContext,
                                                      interfaceInformation,
                                                      interfaceHandle,
                                                      (PUCHAR)interfaceDescriptor +
                                                         interfaceDescriptor->bLength,
                                                      bufferSize);

            if (!NT_SUCCESS(status)) {
                status = STATUS_UNSUCCESSFUL;
                DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
                LEAVE;
            }

        }

        // 
        // Bail out if an endpoint descriptor of endpoint 0 exists.
        // USB 1.0: 9.6.4 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        // USB 2.0: 9.6.6 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        // USB 3.0: 9.6.6 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        //
        if (HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInCurrentConfiguration(DeviceContext)) {
            status = STATUS_UNSUCCESSFUL;
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_BAD_ENDPOINT_ADDRESS;
            LEAVE;
        }

        // 
        // Check for duplicate endpoint addresses
        //
        status = HUBMISC_CheckForDuplicateEndpointAddresses(DeviceContext);

        if (!NT_SUCCESS(status)) {
            status = STATUS_UNSUCCESSFUL;
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            LEAVE;
        }

        newDeviceConfiguration->NumberOfEndpoints = numberOfEndpoints;

        status = HUBMISC_AllocateUcxEndpointArrays(DeviceContext, numberOfEndpoints);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


        if (DeviceContext->OldDeviceConfiguration != NULL) {
            PINTERFACE_HANDLE   interfaceHandle;

            DeviceContext->EndpointsToRemainUnchangedCount = 0;
            DeviceContext->EndpointsToBeDisabledCount = 0;

            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &DeviceContext->OldDeviceConfiguration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                HUBMISC_AddEnabledEndpointsToPendingDisableList(DeviceContext, interfaceHandle);
            }
        }

    } FINALLY {


        if (!NT_SUCCESS(status)) {

            DeviceContext->LastOperationNtStatus = status;

            if (DeviceContext->LastOperationUsbdStatus == 0) {
                DeviceContext->LastOperationUsbdStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);
            }

            //
            // Clean up our memory allocations on failure
            //
            newDeviceConfiguration = DeviceContext->CurrentDeviceConfiguration;

            DeviceContext->CurrentDeviceConfiguration = DeviceContext->OldDeviceConfiguration;
            DeviceContext->OldDeviceConfiguration = NULL;

            //
            // Current configuration hasn't been freed yet, so it is still valid
            //
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);

            if (newDeviceConfiguration != NULL) {
                PINTERFACE_HANDLE   interfaceHandle;

                    while (!IsListEmpty(&newDeviceConfiguration->InterfaceList)) {

                    interfaceHandle =
                        CONTAINING_RECORD(RemoveHeadList(&newDeviceConfiguration->InterfaceList),
                                          INTERFACE_HANDLE,
                                          InterfaceLink);

                    //
                    // Delete the interface handle
                    //
                    WdfObjectDelete(interfaceHandle->MemoryObject);

                    }

                //
                // Delete the configuration handle
                //
                WdfObjectDelete(newDeviceConfiguration->WdfMemory);
            }

            urb->UrbSelectConfiguration.ConfigurationHandle = NULL;

        } else {
            urb->UrbSelectConfiguration.ConfigurationHandle = DeviceContext->CurrentDeviceConfiguration;
        }
    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectInterface (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    This function will be called when we want to configure the device when
    client driver sends a Select Interface request. This function will
    store the list of endpoints that needs to be purged, created, configured
    or de-configured and list of interface alternate settings that need to
    be set on device, at a particular place in the device context.

    Client will (should) pass in a URB buffer that looks like this:

    +------------------------------+
    |Hdr                           |
    |(_URB_HEADER)                 |
    |    - <caller inputs>         |
    |      Function                |
    |      Length                  |
    |      UsbdDeviceHandle        |
    |                              |
    |    - <port outputs>          |
    |        Status                |
    +------------------------------+
    |    - <caller inputs>         |
    |      ConfigurationHandle     |
    +------------------------------+
    |Interface                     |
    |(USBD_INTERFACE_INFORMATION)  |
    |    - <caller inputs>         |
    |      Length                  |
    |      InterfaceNumber         |
    |      AlternateSetting        |
    |                              |
    |    - <port  outputs>         |
    |      InterfaceHandle         |
    |      NumberOfPipes           |
    |      SubClass                |
    |      Class                   |
    |      Protocol                |
    +------------------------------+
    |Pipes[0]                      | one of these for each pipe in the
    |(USBD_PIPE_INFORMATION)       | interface
    |    - <caller inputs>         |
    |      PipeFlags               |
    |      MaximumPacketSize (opt) |
    |                              |
    |    - <port outputs>          |
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    WDFREQUEST                  request;
    PURB                        urb;
    WDF_REQUEST_PARAMETERS      parameters;
    WDF_OBJECT_ATTRIBUTES       attributes;
    PDEVICE_CONFIGURATION       deviceConfiguration;
    PUSBD_INTERFACE_INFORMATION interfaceInformation;
    BOOLEAN                     hasAlternateSettings;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    size_t                      interfaceHandleSize;
    WDFMEMORY                   interfaceHandleMemory;
    PINTERFACE_HANDLE           newInterfaceHandle;
    PINTERFACE_HANDLE           oldInterfaceHandle;
    size_t                      bufferSize;
    KIRQL                       irql;
    ULONG                       numberOfEndpoints;
    BOOLEAN                     foundOldInterface;
    PINTERFACE_HANDLE           interfaceHandle;
    ULONG                       i;
    PUSB_CONFIGURATION_DESCRIPTOR
                                configurationDescriptor;


    TRY {

        status = STATUS_SUCCESS;
        hasAlternateSettings = FALSE;
        newInterfaceHandle = NULL;
        oldInterfaceHandle = NULL;

        DeviceContext->NewInterface = NULL;
        DeviceContext->EndpointsToBeEnabledCount = 0;
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, FailAlternateSetting);

        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        urb = parameters.Parameters.Others.Arg1;

        NT_ASSERT(urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE);

        NT_ASSERTMSG("URB_FUNCTION_SELECT_INTERFACE specified an invalid configuration handle",
                     urb->UrbSelectInterface.ConfigurationHandle ==
                     DeviceContext->CurrentDeviceConfiguration);

        //
        // Synchronize device configuration access with IOCTL_USB_GET_NODE_CONNECTION_INFORMATION.
        //
        KeAcquireSpinLock(&DeviceContext->ConfigurationLock, &irql);
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);
        KeReleaseSpinLock(&DeviceContext->ConfigurationLock, irql);

        deviceConfiguration = DeviceContext->CurrentDeviceConfiguration;

        interfaceInformation = &urb->UrbSelectInterface.Interface;

        //
        // Find the matching interface descriptor for the new interface
        //
        interfaceDescriptor =
            HUBDESC_ParseConfigurationDescriptor((PUSB_CONFIGURATION_DESCRIPTOR)
                                                   &deviceConfiguration->ConfigurationDescriptor,
                                                 (PVOID)&deviceConfiguration->ConfigurationDescriptor,
                                                 interfaceInformation->InterfaceNumber,
                                                 interfaceInformation->AlternateSetting,
                                                 -1,
                                                 -1,
                                                 -1,
                                                 &hasAlternateSettings,
                                                 DeviceContext->PortContext->IfrLog);

        if (interfaceDescriptor == NULL) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Cannot find matching interface descriptor for URB_FUNCTION_SELECT_INTERFACE");

            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INTERFACE_NOT_FOUND;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // Check if this InterfaceNumber/AlternateSetting should be filtered out
        //
        if (DeviceContext->AlternateSettingFilter != NULL) {
            ULONG                       j;
            PALTERNATE_SETTING_FILTER   alternateSettingFilter;

            alternateSettingFilter = DeviceContext->AlternateSettingFilter;

            for (j = 0; j < alternateSettingFilter->Count; j++) {
                if ((alternateSettingFilter->FilterList[j].AlternateSetting == interfaceDescriptor->bAlternateSetting) &&
                    (alternateSettingFilter->FilterList[j].InterfaceNumber == interfaceDescriptor->bInterfaceNumber)) {

                    INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, FailAlternateSetting);
                    break;
                }
            }
        }


        interfaceInformation->Length = (sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION)) +
                                       (sizeof(USBD_PIPE_INFORMATION) * interfaceDescriptor->bNumEndpoints);

        interfaceInformation->Class = 0;
        interfaceInformation->SubClass = 0;
        interfaceInformation->Protocol = 0;
        interfaceInformation->Reserved = 0;
        interfaceInformation->InterfaceHandle = NULL;

        interfaceInformation->NumberOfPipes = interfaceDescriptor->bNumEndpoints;

        //
        // Allocate the interface handle for this interface
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        //
        // Let the object parent default to the driver. We will delete it explicitly.
        // Making it a child of DSM or HSM does not work because we might want to delete
        // them much sooner
        //

        interfaceHandleSize = sizeof(INTERFACE_HANDLE) -
                              sizeof(PIPE_HANDLE) +
                              (sizeof(PIPE_HANDLE) *
                               interfaceInformation->NumberOfPipes);

        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_DEVICE_TAG,
                                 interfaceHandleSize,
                                 &interfaceHandleMemory,
                                 &newInterfaceHandle);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfMemoryCreate Failed for interface handle %!STATUS!",
                       status);
            LEAVE;
        }

        RtlZeroMemory (newInterfaceHandle, interfaceHandleSize);

        newInterfaceHandle->MemoryObject = interfaceHandleMemory;

        newInterfaceHandle->InterfaceDescriptor = interfaceDescriptor;
        newInterfaceHandle->NumberOfEndpoints = interfaceDescriptor->bNumEndpoints;

        DeviceContext->NextInterfaceToSet = NULL;

        if (hasAlternateSettings == TRUE) {
            INTERLOCKED_SET_BIT_FIELD(INTERFACE_FLAGS, &newInterfaceHandle->InterfaceFlags, HasAlternateSettings);
            INTERLOCKED_SET_BIT_FIELD(INTERFACE_FLAGS, &newInterfaceHandle->InterfaceFlags, NeedsSetInterface);
            DeviceContext->NextInterfaceToSet = newInterfaceHandle;
        }

        //
        // Populate the interface information structure
        //
        interfaceInformation->Class = interfaceDescriptor->bInterfaceClass;
        interfaceInformation->SubClass = interfaceDescriptor->bInterfaceSubClass;
        interfaceInformation->Protocol = interfaceDescriptor->bInterfaceProtocol;

        bufferSize = (size_t)((PUCHAR)interfaceDescriptor + interfaceDescriptor->bLength -
                              (PUCHAR)&deviceConfiguration->ConfigurationDescriptor);

        configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)&deviceConfiguration->ConfigurationDescriptor;
        
        bufferSize = configurationDescriptor->wTotalLength - bufferSize;

        status = HUBMISC_InitializeInterfacePipes(DeviceContext,
                                                  interfaceInformation,
                                                  newInterfaceHandle,
                                                  (PUCHAR)interfaceDescriptor +
                                                    interfaceDescriptor->bLength,
                                                  bufferSize);

        if (!NT_SUCCESS(status)) {
            status = STATUS_UNSUCCESSFUL;
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INTERFACE_NOT_FOUND;
            LEAVE;
        }

        //
        // Bail out if an endpoint descriptor of endpoint 0 exists.
        // USB 1.0: 9.6.4 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        // USB 2.0: 9.6.6 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        // USB 3.0: 9.6.6 Endpoint: "There is never an endpoint descriptor for endpoint zero."
        //
        if (HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInInterface(DeviceContext, newInterfaceHandle)) {
            status = STATUS_UNSUCCESSFUL;
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_BAD_ENDPOINT_ADDRESS;
            LEAVE;
        }

        DeviceContext->OldInterface = NULL;

        foundOldInterface = FALSE;
        //
        // Find and remove the old matching interface in the current configuration
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &deviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            if (interfaceHandle->InterfaceDescriptor->bInterfaceNumber ==
                interfaceInformation->InterfaceNumber) {

                oldInterfaceHandle = interfaceHandle;
                foundOldInterface = TRUE;
            } else {
                //
                // For interfaces that are not being removed, make sure that the new
                // interface doesn't share any endpoints with the existing interface
                //
                status = HUBMISC_CheckForSharedEndpoints(DeviceContext,
                                                         interfaceHandle,
                                                         newInterfaceHandle);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }

        //
        // Calculate the number of endpoints that would be present after the
        // new interface has been selected
        //
        numberOfEndpoints  =  deviceConfiguration->NumberOfEndpoints + newInterfaceHandle->NumberOfEndpoints;

        status = HUBMISC_AllocateUcxEndpointArrays(DeviceContext, numberOfEndpoints);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (foundOldInterface) {
            numberOfEndpoints -= oldInterfaceHandle->NumberOfEndpoints;
        }

        //
        // Now that we have allocated all resources, we will go ahead and change
        // the state of the current configuration
        //

        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        DeviceContext->EndpointsToBeDisabledCount = 0;

        if (foundOldInterface) {
            RemoveEntryList(&oldInterfaceHandle->InterfaceLink);
            DeviceContext->OldInterface = oldInterfaceHandle;

            HUBMISC_AddEnabledEndpointsToPendingDisableList(DeviceContext, oldInterfaceHandle);
        }

        //
        // Mark all the existing endpoints to be remain unchanged
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &deviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                PPIPE_HANDLE    pipeHandle;

                pipeHandle = &interfaceHandle->PipeHandle[i];

                DeviceContext->EndpointsToRemainUnchanged[DeviceContext->EndpointsToRemainUnchangedCount++] =
                    pipeHandle->UcxEndpoint;

            }
        }

        //
        // Insert the new interface handle into the current configuration
        //
        InsertTailList(&deviceConfiguration->InterfaceList, &newInterfaceHandle->InterfaceLink);

        DeviceContext->NewInterface = newInterfaceHandle;

        //
        // Update the total number endpoints for the current configuration
        //
        deviceConfiguration->NumberOfEndpoints = numberOfEndpoints;

    } FINALLY {

        if (NT_SUCCESS(status)) {

            interfaceInformation->InterfaceHandle = newInterfaceHandle;

        } else {

            interfaceInformation->InterfaceHandle = USBPORT_BAD_HANDLE;

            if (newInterfaceHandle != NULL) {
                WdfObjectDelete(newInterfaceHandle->MemoryObject);
            }

            DeviceContext->LastOperationNtStatus = status;

            if (DeviceContext->LastOperationUsbdStatus == 0) {
                DeviceContext->LastOperationUsbdStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);
            }
        }
    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBMISC_CheckIfDevicePDChargingPolicySupported (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device supports setting charging policy via SET_FEATURE.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        dsmEvent = (DeviceContext->DeviceFlags.ChargingPolicySupported) ? DsmEventYes : DsmEventNo;

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfFirstEnumTry (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        dsmEvent = (DeviceContext->EnumRetryCount == 0) ? DsmEventYes : DsmEventNo;

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfMSOSExtendedConfigDescriptorIsSupported (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the device may support the MS OS Extended Configuration
    Descriptor.  This is based on whether or not the device is marked as
    supporting the MS OS Descriptor, and if the device is not a composite device.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       dsmEvent;

    TRY {

        if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {

            dsmEvent = DsmEventNo;

        } else if (((DeviceContext->DeviceDescriptor.bcdUSB >= 0x200) ||
                   (DeviceContext->DeviceDescriptor.bcdUSB < 0x100)) &&
                  (DeviceContext->DeviceFlags.MsOsDescriptorNotSupported == 0) &&
                  (DeviceContext->DeviceFlags.DeviceIsComposite == 0)) {

            dsmEvent = DsmEventYes;

        } else if (DeviceContext->DeviceHackFlags.DontSkipMsOsDescriptor == 1) {

            //
            // This is set for 1.1 devices that need MS OS descriptor support
            //
            dsmEvent = DsmEventYes;

        } else {

            dsmEvent = DsmEventNo;

        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_AddDeviceInfoToTheGlobalChildList (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    This routine searches for the given child in the global list of all USB devices.
    If it finds that the child is already present in the list and the device is
    physically connected (i.e. the disconnected flag is not set) then it returns
    DsmEventOperationFailure. If it finds that the child is already present in the
    list but the device is physically dis-connected (i.e. the disconnected flag is set)
    then it returns DsmEventOperationPending.  If it does not find the child, then it
    adds the device to the global list.. It needs to do these operations atomically,
    so it would need to do it under a lock.

Returns:

    DsmEventOperationFailure
    DsmEventOperationSuccess
    DsmEventOperationPending

--*/
{
    USBD_CHILD_STATUS       childStatus;
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        //
        // No need to insert devices into the global list if they don't have a serial number
        //
        if (DeviceContext->SerialNumberId.Buffer == NULL) {
            dsmEvent = DsmEventOperationSuccess;
            LEAVE;
        }

        //
        // Try to insert the device into the global list
        //
        childStatus = USBD_AddDeviceToGlobalList(DeviceContext,
                                                 DeviceContext->HubFdoContext,
                                                 DeviceContext->PortContext->PortInformation.PortNumber,
                                                 (PVOID)HUBCONNECTOR_GetConnectorMapNodeForPort(DeviceContext->PortContext),
                                                 DeviceContext->DeviceDescriptor.idVendor,
                                                 DeviceContext->DeviceDescriptor.idProduct,
                                                 &DeviceContext->SerialNumberId);

        switch (childStatus) {
        case USBD_CHILD_STATUS_INSERTED:
            dsmEvent = DsmEventOperationSuccess;
            break;

        case USBD_CHILD_STATUS_DUPLICATE_FOUND:
            
            HUBDESC_InitializeDeviceInfo(DeviceContext,
                                         &devInfo);

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Duplicate serial number descriptor found");
            LOG_DESCRIPTOR_VALIDATION_ERROR(&devInfo, DescriptorValidationErrorDuplicateSerialNumber);

            __fallthrough;
        case USBD_CHILD_STATUS_FAILURE:
            dsmEvent = DsmEventOperationFailure;
            break;

        case USBD_CHILD_STATUS_DUPLICATE_PENDING_REMOVAL:
            dsmEvent = DsmEventOperationPending;
            break;

        default:
            NT_ASSERTMSG("Unexpected return value from USBD_AddDeviceToGlobalList", 0);
            dsmEvent = DsmEventOperationFailure;
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfDeviceIsRemovable (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device has been marked as removable.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT   result;

    TRY {

        if (DeviceContext->DeviceFlags.DeviceNotRemovable == 1) {
            result = DsmEventNo;
        } else {
            result = DsmEventYes;
        }

    } FINALLY {

    }

    return result;
}

DSM_EVENT
HUBMISC_CheckIfDeviceShouldBeDisarmedForWakeOnResume (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device should be disarmed for wake on
    resume

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT   result;

    TRY {

        if ((DeviceContext->DeviceFlags.UsbWakeupSupport == 0) ||
            (DeviceContext->PdoContext->PdoFlags.SupportsFunctionSuspend == 1)) {
            result = DsmEventNo;
            LEAVE;
        } else {
            result = DsmEventYes;
        }

    } FINALLY {

    }

    return result;
}

DSM_EVENT
HUBMISC_CheckIfDeviceShouldBeDisabledOnRemove (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device should be disabled on soft remove.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       result;
    PHUB_GLOBALS    hubGlobals;

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        if ((DeviceContext->DeviceHackFlags.DisableOnSoftRemove == 1) &&
            (DeviceContext->HubFdoContext->HubInformation.HubFlags.DisableOnSoftRemove == 1) &&
            (hubGlobals->HubGlobalFlags.DisableOnSoftRemove == 1)) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;
}

DSM_EVENT
HUBMISC_CheckIfLTMShouldBeEnabled (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the LTM should be enabled for the device

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       result;
    PHUB_GLOBALS    hubGlobals;

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        //
        // LTM is enabled only if the global policy directs it 
        // to be enabled, device supports LTM and it is an internal
        // device.
        //
        if ((DeviceContext->DeviceFlags.LtmCapable == 1) &&
            (DeviceContext->DeviceFlags.DeviceNotRemovable == 1) &&
            (hubGlobals->HubGlobalFlags.EnableUsbLtm == 1)) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;
}

VOID
HUBMISC_DiscardSerialNumber (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Discards the cached serial number for the device, and frees the buffer
    used to store it.

Returns:

    VOID

--*/
{
    HUBMISC_FreeBufferIfNotNull(DeviceContext->SerialNumberId.Buffer, USBHUB3_DEVICE_TAG);
    INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, SerialNumber);
    DeviceContext->SerialNumberId.Buffer = NULL;
}


DSM_EVENT
HUBMISC_IsResetOnResumeInS0Set (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the device has been marked as needing a "ResetOnResume"
    when resuming from selective suspend.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       result;

    TRY {

        if (DeviceContext->DeviceHackFlags.ResetOnResumeS0 == 1) {
            result = DsmEventYes;
            LEAVE;
        }

        result = DsmEventNo;

    } FINALLY {

    }

    return result;
}



DSM_EVENT
HUBMISC_IsResetOnResumeInSxSet (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the device has been marked as needing a "ResetOnResume"
    when resuming from system suspend.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       result;

    TRY {

        if (DeviceContext->DeviceHackFlags.ResetOnResumeSx == 1) {
            result = DsmEventYes;
            LEAVE;
        }

        result = DsmEventNo;


    } FINALLY {

    }

    return result;
}



VOID
HUBMISC_MarkDeviceAsDisconnectedInGlobalChildList (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sets a flag in the device's entry in the global USB device list as having
    been disconnected from the port.

Returns:

    DsmEventOperationSuccess

--*/
{
    if (DeviceContext->SerialNumberId.Buffer != NULL) {
        USBD_MarkDeviceAsDisconnected(DeviceContext);
    }
}



VOID
HUBMISC_SetPortAndSpeedFlagsFor30Device (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Sets SubSm Flags for Version And Speed for a
    30 Device working in SuperSpeed mode.

Returns:

    DsmEventOperationSuccess

--*/
{
    TRY {

        PortContext->DeviceContext->DsmContext.SubSmFlags |= (DsmSubSmFlagSpeedSuper | DsmSubSmFlagPort30);

    } FINALLY {

    }

}

VOID
HUBMISC_SetDeviceFlagToForceResetOnEnumeration (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Sets Device flag to indicate that DSM needs
    to reset the device while enumerating.

Returns:

    DsmEventOperationSuccess

--*/
{
    TRY {

        //
        // If something is worng with the port state, then
        // we want a warm enumeration
        //
        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &PortContext->DeviceContext->DeviceStateFlags, WarmResetOnEnumRequired);

    } FINALLY {

    }

}


DSM_EVENT
HUBMISC_DetermineDeviceResetTypeRequiredAndClearResetFlags (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the port needs to be reset during
    enumeration or re-enumeration of a SuperSpeed
    device and if yes, whether we want to do a warm
    reset or hot reset

Returns:

    DsmEventNoReset
    DsmEventWarmReset
    DsmEventHotReset

--*/
{
    DSM_EVENT           retEvent;
    LARGE_INTEGER       currentTime;
    LARGE_INTEGER       delta;


    TRY {
        //
        // If this is second or subsequent try for enumeration,
        // then we want to do warm reset in the hope that it
        // might bring the device back to a good state
        //
        if (DeviceContext->EnumRetryCount > 0) {

            retEvent = DsmEventDeviceWarmReset;
            LEAVE;
        }

        //
        // If everything is ok with the port but the client driver
        // asks for a reset, we will start with a hot reset. But
        // if we get another reset request quickly, then hot
        // reset might not be working and we need to fall back
        // to warm reset
        //
        KeQuerySystemTime(&currentTime);
        NT_ASSERT(DeviceContext->LastResetTimeStamp.QuadPart <= currentTime.QuadPart);
        delta.QuadPart = currentTime.QuadPart - DeviceContext->LastResetTimeStamp.QuadPart;
        DeviceContext->LastResetTimeStamp.QuadPart = currentTime.QuadPart;

        if (delta.QuadPart < WDF_ABS_TIMEOUT_IN_SEC(MINIMUM_TIME_BETWEEN_CONSECUTIVE_HOT_RESETS_IN_SEC)) {
            retEvent = DsmEventDeviceWarmReset;
            LEAVE;
        }

        //
        // Whoever asked for a reset has explcitly requested warm
        // reset, then we do want to honour that. Normally it is
        // the PSM that does that when it sees that the port is
        // in a bad state
        //
        if (DeviceContext->DeviceStateFlags.WarmResetOnEnumRequired == 1) {

            retEvent = DsmEventDeviceWarmReset;
            LEAVE;

        }

        if (DeviceContext->DeviceStateFlags.HotResetOnEnumRequired == 1) {
            //
            // Even though the current conditions don't require a warm
            // reset, we have to do warm reset because a device doesn't
            // support hot reset
            //
            if (DeviceContext->DeviceHackFlags.DisableHotReset == 1) {

                retEvent = DsmEventDeviceWarmReset;

            } else {

                retEvent = DsmEventDeviceHotReset;

            }

            LEAVE;
        }

        //
        // We don't need any resets
        //
        retEvent = DsmEventDeviceNoReset;


    } FINALLY {

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, HotResetOnEnumRequired);
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, WarmResetOnEnumRequired);


    }

    return retEvent;
}

VOID
HUBMISC_InitializePortChangeBits (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This functions clears cumulative port change bits

Return Value:

--*/
{

    RtlZeroMemory(&PortContext->CumulativePortChange,
                  sizeof(PortContext->CumulativePortChange));

}


DSM_EVENT
HUBMISC_SetSpeedFlagFor20TypeDevice (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
/*++

Routine Description:

    Sets SubSm flag for Speed for any device that
    enumerates on a 2.1/2.0/1.1/1.0 port

Returns:

    void

--*/
{
    ULONG       newSpeedFlag;
    ULONG       oldSpeedFlag;
    DSM_EVENT   retEvent;
    
    TRY {
        
        retEvent = DsmEventOperationSuccess;

        oldSpeedFlag =  DeviceContext->DsmContext.SubSmFlags & (DsmSubSmFlagSpeedAny);
        
        if (DeviceContext->PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.HighSpeedDeviceAttached == 1) {

            newSpeedFlag = DsmSubSmFlagSpeedHigh;

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsHighSpeedCapable);

        } else if (DeviceContext->PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.LowSpeedDeviceAttached == 1) {

            newSpeedFlag = DsmSubSmFlagSpeedLow; 

        } else {

            newSpeedFlag = DsmSubSmFlagSpeedFull;
        }

        
        //
        // Clear the old and set the new
        //
        DeviceContext->DsmContext.SubSmFlags &= ~DsmSubSmFlagSpeedAny;
        DeviceContext->DsmContext.SubSmFlags |=  newSpeedFlag;
        
        if ((oldSpeedFlag != 0) && (newSpeedFlag != oldSpeedFlag)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Device speed flags changed speed from 0x%x to 0x%x",
                       oldSpeedFlag, newSpeedFlag);
            
            if (oldSpeedFlag == DsmSubSmFlagSpeedHigh) {
                //
                // If the device went from High to Full/Low, we 
                // need to retry
                //
                retEvent = DsmEventOperationFailure;
                LEAVE;
            } 
        }



    } FINALLY {

    }

    return retEvent;
}

VOID
HUBMISC_SetDeviceVersionFlag (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
/*++

Routine Description:

    Set the appropriate Sub state machine flags
    for the version of the device

Returns:

    void

--*/
{
    USHORT      bcdUSB;

    TRY {
        DeviceContext->DsmContext.SubSmFlags &= ~DsmSubSmFlagVersionAny;

        bcdUSB = DeviceContext->DeviceDescriptor.bcdUSB;

        if (bcdUSB == 0x100) {

            DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion10;

        } else if ((bcdUSB > 0x100) && (bcdUSB < 0x200)) {

            DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion1X;

        } else if (bcdUSB == 0x200) {

            DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion20;

        } else if ((bcdUSB > 0x200) && (bcdUSB < 0x300)) {

            DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion2X;

        } else if (bcdUSB >= 0x300) {

            if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagPort30) {

                DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion3X;

            } else {
                //
                // Note that even 30 devices are supposed to report version
                // as 2X when connected on a 20 port. So we will just assume
                // 2X
                //
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Device connected to 2.0 port reporting bcdUSB value of 0x%x in Device Descriptor",
                           bcdUSB);

                DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion2X;
            }

        } else {

            //
            // Device Version is being reported less than 0x100. We will
            // treat them as 1X
            //
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Device reporting invalid bcdUSB value of 0x%x in Device Descriptor",
                       bcdUSB);

            DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagVersion1X;
        }

    } FINALLY {

    }

}

VOID
HUBMISC_SetPortFlagFor20TypeDevice (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Sets the SubSmFlag for version for a device that enumerates
    on a  2.1/2.0/1.1/1.0 port as 20. Exact USB version might not
    be known at this point and will be updated in a different
    function

Returns:

    void

--*/
{
    TRY {

        PortContext->DeviceContext->DsmContext.SubSmFlags |= DsmSubSmFlagPort20;


    } FINALLY {

    }

}


VOID
HUBMISC_RemoveDeviceInfoFromGlobalChildList (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Removes the device's information from the global USB device list.

Returns:

    DsmEventOperationSuccess

--*/
{
    if (DeviceContext->SerialNumberId.Buffer != NULL) {
        USBD_RemoveDeviceFromGlobalList(DeviceContext);
    }
}

VOID
HUBMISC_ShutdownSystemOnBootDeviceFailure (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Start Wdf timer for duplicate device to go away. The expiration routine
    for the timer should queue DsmEventTimerFired event in to DSM.

Returns:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(DeviceContext);
    NT_ASSERTMSG("Boot device could not be recovered. System Shutting down",
                 FALSE);

}

VOID
HUBMISC_StartDuplicateDeviceTimer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Start Wdf timer for duplicate device to go away. The expiration routine
    for the timer should queue DsmEventTimerFired event in to DSM.

Returns:

    VOID

--*/
{
    TRY {

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_DUPLICATE_SN_POLLING_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_SetAttachStatusToSuccess (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    TRY {

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceAttachSuccessful);

    } FINALLY {

    }

}

VOID
HUBMISC_UpdateDeviceU1StatusToDisabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1EnabledForUSPort);

}

VOID
HUBMISC_UpdateDeviceU1StatusToEnabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1EnabledForUSPort);

}

VOID
HUBMISC_UpdateDeviceU2StatusToDisabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U2EnabledForUSPort);

}

VOID
HUBMISC_UpdateDeviceU2StatusToEnabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U2EnabledForUSPort);

}

DSM_EVENT
HUBMISC_CheckIfU2ShouldBeSetForEnumeratedDevice (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Computes the LPM timeout for a device that is in a enumerated state
    but has not yet been configured

Returns:
    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           retEvent;
    TRY {


        if ((DeviceContext->LpmPolicy.U2TimeoutAcceptTransitions == 0) ||
            (DeviceContext->DeviceHackFlags.DisableLpm == 1) ||
            (DeviceContext->HubFdoContext->HubInformation.HubFlags.DisableLpm == 1)) {

            retEvent = DsmEventNo;
            LEAVE;
        }

        //
        // Set U2 to maximum value
        //
        if (DeviceContext->LpmPolicy.U2TimeoutInitiateTransitions == 0) {
            DeviceContext->U2Timeout = 0xFF;
        } else {
            DeviceContext->U2Timeout = 0xFE;
        }

        retEvent = DsmEventYes;


    } FINALLY {

    }

    return retEvent;

}

//
// USB_ENDPOINT_DESCRIPTOR bmAttributes bits 4-5
//
#define USB_ENDPOINT_INTERRUPT_USAGE_TYPE_MASK                      0x30
#define USB_ENDPOINT_INTERRUPT_USAGE_TYPE_PERIODIC                  0x0
#define USB_ENDPOINT_INTERRUPT_USAGE_TYPE_NOTIFICATION              0x1


VOID
HUBMISC_InitializeU1U2Flags (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Reset all the U1 U2 state flags. This is function is
    invoked before the state machine is about to re-compute
    and re-prgoram U1 U2 timeout values and the max
    exit latency

Returns:

--*/
{

    TRY {
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1DisabledDueToIntolerableLatency);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U2DisabledDueToIntolerableLatency);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1U2DisabledDueToNoPingResponse);
        DeviceContext->LatencyAdjustmentPercentForNoPingError = 0;

    } FINALLY {
    }
}

VOID
HUBMISC_ComputeU1Timeout (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Computes the U1 timeout for a device that is in a configured state
    It takes into account the characteristics of the different endpoints
    of the device, the exit latencies and the current power settings
    to decide the new U1 timeout. Refer to the document on U1U2Timeouts
    to understand the reasoning behind the heuristics

Returns:

--*/
{
    UCHAR                   newTimeout;
    ULONG                   i;
    PINTERFACE_HANDLE       interfaceHandle;
    PPIPE_HANDLE            pipeHandle;
    USHORT                  endpointTimeout;
    USHORT                  maxEndpointTimeout;
    USHORT                  exitLatency;
    BOOLEAN                 nonControlEndpointPresent;

    TRY {

        if ((DeviceContext->DeviceHackFlags.DisableLpm == 1) ||
            (DeviceContext->HubFdoContext->HubInformation.HubFlags.DisableLpm == 1) ||
            (DeviceContext->LPMStateFlags.U1DisabledDueToIntolerableLatency == 1) ||
            (DeviceContext->LPMStateFlags.U1U2DisabledDueToNoPingResponse == 1)) {

            newTimeout = 0;
            INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, TargetU1EnabledForUSPort);
            LEAVE;
        }

        //
        // If there is no reason for disabling U1 on the US port,
        // then just follow the policy
        //
        DeviceContext->LPMStateFlags.TargetU1EnabledForUSPort = DeviceContext->LpmPolicy.U1EnabledForUSPort;

        if (DeviceContext->LpmPolicy.U1TimeoutAcceptTransitions == 0) {
            newTimeout = 0;
            LEAVE;
        }

        if (DeviceContext->LpmPolicy.U1TimeoutInitiateTransitions == 0) {
            newTimeout = 0xFF;
            LEAVE;
        }

        //
        // If the policy tells us to choose aggressive or
        // conservative value, then we can go ahead and choose
        // that now and skip the heuristics
        //
        if (DeviceContext->LpmPolicy.ForceAggressiveTimeoutValues == 1) {
            newTimeout = 1;
            LEAVE;
        }

        if (DeviceContext->LpmPolicy.ForceConservativeTimeoutValues == 1) {
            newTimeout = 0x7F;
            LEAVE;
        }


        maxEndpointTimeout = 0;

        //
        //
        // We first need to determine if there is at least one non-control
        // endpoint for the device
        //
        nonControlEndpointPresent = FALSE;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];
                if ((pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) != USB_ENDPOINT_TYPE_CONTROL) {
                    nonControlEndpointPresent = TRUE;
                    break;

                }
            }

            if (nonControlEndpointPresent) {
                break;
            }
        }


        // We go through all the endpoints of the current configuration,
        // calculate U1 timeout for each of them and the take the maximum
        // for the device. Calculation of timeout per endpoint is based
        // on a heuristics so as to optimize power with minimum effect
        // on performance
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (USB_ENDPOINT_DIRECTION_IN(pipeHandle->EndpointDescriptor->bEndpointAddress)) {
                    exitLatency = DeviceContext->UsbSelInfo.U1SEL;
                } else {
                    exitLatency = DeviceContext->HostInitiatedU1ExitLatency;
                }

                switch (pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {

                case USB_ENDPOINT_TYPE_CONTROL:
                    //
                    // If there are other endpoints on the device, then it is
                    // reasonable to conclude this endpoint is not going to
                    // be active for most of the time. So the only thing we
                    // need to prevent is the device going to U state in betweeen
                    // the packets. On the other hand, if the device only has
                    // control endpoints, then we follow our exit latency
                    // heuristics
                    //
                    if (nonControlEndpointPresent) {
                        endpointTimeout = U1_TIMEOUT_FOR_CONTROL_ENDPOINTS;
                    } else {
                        endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    }
                    break;

                case USB_ENDPOINT_TYPE_BULK:
                    //
                    // Since bulk endpoints don't have any other hints about
                    // the usage pattern, we will follow our regular exit
                    // latency heuristics
                    //
                    endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    break;

                case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                    //
                    // U1/U2 is perhaps most useful for isochronous endpoints because
                    // USB 3.0 specification provides a "Ping" mechanism to ensure that
                    // the U1/U2 transitions do not negatively impact the performance.
                    // However, the exit latency from a U state can potentially prevent
                    // this mechanism for working. Therefore we first decide if the exit
                    // latency could be tolerable.We do this at two levels. First we do it
                    // here by just comparing the exit latency to service interval. But then
                    // the hardware can also choose to reject the max exit latency in which
                    // case U1DisabledDueToIntolerableLatency flag will be set, which has
                    // alread been checked above. Assuming the latency is tolerable, then
                    // there is no impact on the performance. Therefore we would want the
                    // link to go to U state as soon as the transfer is completed within
                    // a service interval, to maximize the time the link stays in U state.
                    // So for an isochronous endpoint,we should choose a smallest time-out
                    // value.
                    //
                    if (exitLatency > (pipeHandle->EndpointDescriptor->bInterval * 125)) {
                        newTimeout = 0;
                        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, TargetU1EnabledForUSPort);
                        LEAVE;
                    }
                    endpointTimeout = 1;
                    break;

                case USB_ENDPOINT_TYPE_INTERRUPT:
                    //
                    // For interrupt with periodic usage, if we subsequently decide
                    // to set a non 0 non 0xFF value for U2 timeout, then the U1
                    // timeout will be set to 0xFF.
                    //
                    endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    break;

                default:
                    NT_ASSERTMSG("Unknown endpoint type", 0);
                    endpointTimeout = 0;
                    break;
                }

                maxEndpointTimeout = max(maxEndpointTimeout,
                                         endpointTimeout);

            }

        }

        //
        // 0x7f is the maximum value allowed
        //
        newTimeout = min((UCHAR)maxEndpointTimeout,
                         0x7F);


    } FINALLY {

        if ((DeviceContext->HubFdoContext->HubInformation.HubFlags.DisallowU2AcceptOnlyWithU1AcceptAndInitiate == 1) &&
            (DeviceContext->DeviceFlags.DeviceIsHub == 1) &&
            (newTimeout > 0)) {
            //
            // Some host controllers have this strange issue that if
            // U1 is set to accept and enable and U2 is set to accept
            // only (which we do for hubs) then they will keep rejecting
            // U2 transitions. So we will set U1 also to accept only
            //
            newTimeout = 0xFF;
        }

        DeviceContext->TargetU1Timeout = newTimeout;

    }


}

DSM_EVENT
HUBMISC_CheckIfU1TimeoutNeedsToBeUpdated (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the target U1 timeout is different than the current

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           retEvent;

    TRY {

        if (DeviceContext->TargetU1Timeout != DeviceContext->U1Timeout) {
            DeviceContext->U1Timeout = DeviceContext->TargetU1Timeout;
            retEvent = DsmEventYes;
        } else {
            retEvent = DsmEventNo;
        }

    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBMISC_CheckIfU2TimeoutNeedsToBeUpdated (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the target U2 timeout is different than the current

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           retEvent;

    TRY {

        if (DeviceContext->TargetU2Timeout != DeviceContext->U2Timeout) {
            DeviceContext->U2Timeout = DeviceContext->TargetU2Timeout;
            retEvent = DsmEventYes;
        } else {
            retEvent = DsmEventNo;
        }

    } FINALLY {

    }

    return retEvent;
}

PSM_EVENT
HUBMISC_CheckIfDebounceTimeIsNeeded (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Checks if we need to give extra time to SuperSpeed
    devices before talking to them

Returns:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT           retEvent;
    PHUB_GLOBALS        hubGlobals;


    UNREFERENCED_PARAMETER(PortContext);

    TRY {
        hubGlobals = GetHubGlobals(WdfGetDriver());
        
        NT_ASSERT(hubGlobals->AllowDebounceTimeForSuperSpeedDevices >= 0);
        if ((hubGlobals->HubGlobalFlags.PreventDebounceTimeForSuperSpeedDevices == 0) &&
            (hubGlobals->AllowDebounceTimeForSuperSpeedDevices > 0)) {

            retEvent = PsmEventYes;

        } else {

            retEvent = PsmEventNo;
        }


    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBMISC_CheckIfEnableU1NeedsToBeUpdated (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device state with regard to U1 disable/enable state needs
    to be changed and returns DsmEventEnableUState or DsmEventDisableUState.
    If the device state does not need to be changed, then it returns
    OperationSuccess

Returns:

    DsmEventEnableUState
    DsmEventDisableUState
    DsmEventOperationSuccess

--*/
{
    DSM_EVENT           retEvent;

    TRY {

        retEvent = DsmEventOperationSuccess;

        if ((DeviceContext->LPMStateFlags.TargetU1EnabledForUSPort == 0) &&
            (DeviceContext->LPMStateFlags.U1EnabledForUSPort == 1)) {
            retEvent = DsmEventDeviceDisableUState;
        }

        if ((DeviceContext->LPMStateFlags.TargetU1EnabledForUSPort != 0) &&
            (DeviceContext->LPMStateFlags.U1EnabledForUSPort == 0)) {
            retEvent = DsmEventDeviceEnableUState;
        }


    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBMISC_CheckIfEnableU2NeedsToBeUpdated (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device state with regard to U2 disable/enable state needs
    to be changed and returns DsmEventEnableUState or DsmEventDisableUState.
    If the device state does not need to be changed, then it returns
    OperationSuccess

Returns:

    DsmEventEnableUState
    DsmEventDisableUState
    DsmEventOperationSuccess

--*/
{
    DSM_EVENT           retEvent;

    TRY {

        retEvent = DsmEventOperationSuccess;

        if ((DeviceContext->LPMStateFlags.TargetU2EnabledForUSPort == 0) &&
            (DeviceContext->LPMStateFlags.U2EnabledForUSPort == 1)) {
            retEvent = DsmEventDeviceDisableUState;
        }

        if ((DeviceContext->LPMStateFlags.TargetU2EnabledForUSPort != 0) &&
            (DeviceContext->LPMStateFlags.U2EnabledForUSPort == 0)) {
            retEvent = DsmEventDeviceEnableUState;
        }


    } FINALLY {

    }

    return retEvent;
}

VOID
HUBMISC_ComputeU2Timeout (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Computes the U2 timeout for a device that is in a configured state
    It takes into account the characteristics of the different endpoints
    of the device, the exit latencies and the current power settings
    to decide the new U1 timeout.Refer to the document on U1U2Timeouts
    to understand the reasoning behind the heuristics

Returns:

--*/
{
    UCHAR                   newTimeout;
    ULONG                   i;
    PINTERFACE_HANDLE       interfaceHandle;
    PPIPE_HANDLE            pipeHandle;
    USHORT                  endpointTimeout;
    USHORT                  maxEndpointTimeout;
    USHORT                  exitLatency;
    BOOLEAN                 nonControlEndpointPresent;
    BOOLEAN                 periodicInterruptEndpointPresent;
    BOOLEAN                 disableInitiateTransition;

    TRY {

        if ((DeviceContext->DeviceHackFlags.DisableLpm == 1) ||
            (DeviceContext->HubFdoContext->HubInformation.HubFlags.DisableLpm == 1) ||
            (DeviceContext->LPMStateFlags.U2DisabledDueToIntolerableLatency == 1) ||
            (DeviceContext->LPMStateFlags.U1U2DisabledDueToNoPingResponse == 1)) {

            newTimeout = 0;
            INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, TargetU2EnabledForUSPort);
            LEAVE;
        }

        //
        // If there is no reason for disabling U2 on the US port,
        // then just follow the policy
        //
        DeviceContext->LPMStateFlags.TargetU2EnabledForUSPort = DeviceContext->LpmPolicy.U2EnabledForUSPort;

        if (DeviceContext->LpmPolicy.U2TimeoutAcceptTransitions == 0) {
            newTimeout = 0;
            LEAVE;
        }

        //
        // Parent should not initiate U2 timeouts for hub but should
        // accept it if the hub initiates it
        //
        if ((DeviceContext->DeviceFlags.DeviceIsHub == 1) ||
            (DeviceContext->LpmPolicy.U2TimeoutInitiateTransitions == 0)) {
            newTimeout = 0xFF;
            LEAVE;
        }

        //
        // If the policy tells us to choose aggressive or
        // conservative value, then we can go ahead and choose
        // that now and skip the heuristics
        //
        if (DeviceContext->LpmPolicy.ForceAggressiveTimeoutValues == 1) {
            newTimeout = 1;
            LEAVE;
        }

        if (DeviceContext->LpmPolicy.ForceConservativeTimeoutValues == 1) {
            newTimeout = 0xFE;
            LEAVE;
        }


        maxEndpointTimeout = 0;

        //
        //
        // We first need to determine if there is at least one non-control
        // endpoint for the device
        //
        nonControlEndpointPresent = FALSE;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];
                if ((pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) != USB_ENDPOINT_TYPE_CONTROL) {
                    nonControlEndpointPresent = TRUE;
                    break;

                }
            }

            if (nonControlEndpointPresent) {
                break;
            }
        }

        disableInitiateTransition = FALSE;
        periodicInterruptEndpointPresent = FALSE;

        //
        // We go through all the endpoints of the current configuration,
        // calculate U2 timeout for each of them and the take the maximum
        // for the device. Calculation of timeout per endpoint is based
        // on a heuristics so as to optimize power with minimum effect
        // on performance
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (USB_ENDPOINT_DIRECTION_IN(pipeHandle->EndpointDescriptor->bEndpointAddress)) {
                    exitLatency = DeviceContext->UsbSelInfo.U2SEL;
                } else {
                    exitLatency = DeviceContext->HostInitiatedU2ExitLatency;
                }

                switch (pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {

                case USB_ENDPOINT_TYPE_CONTROL:
                    if (nonControlEndpointPresent) {
                        endpointTimeout = U2_TIMEOUT_FOR_CONTROL_ENDPOINTS;
                    } else {
                        endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    }
                    break;

                case USB_ENDPOINT_TYPE_BULK:
                    endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    break;

                case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                    if (exitLatency > (pipeHandle->EndpointDescriptor->bInterval * 125)) {
                        newTimeout = 0;
                        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, TargetU2EnabledForUSPort);
                        LEAVE;
                    }
                    endpointTimeout = 1;
                    break;

                case USB_ENDPOINT_TYPE_INTERRUPT:
                    if ((pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_INTERRUPT_USAGE_TYPE_MASK) ==
                        USB_ENDPOINT_INTERRUPT_USAGE_TYPE_PERIODIC){

                        periodicInterruptEndpointPresent = TRUE;

                        if (exitLatency > (pipeHandle->EndpointDescriptor->bInterval * 125)) {
                            //
                            // If the exit latency is greater than the service interval
                            // then we infer that the latency might be intolerable for the
                            // device and hence we will not enable host initiate transitions
                            // The only reason why we can't LEAVE is becasue there might
                            // another endpoint who would require us to set the value to 0
                            //
                            disableInitiateTransition = TRUE;
                            endpointTimeout = 0xFF;
                        } else {

                            endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                        }

                    } else {
                        endpointTimeout = exitLatency * EXIT_LATENCY_MULTIPLIER_FOR_U1_U2_TIMEOUT;
                    }
                    break;

                default:
                    NT_ASSERTMSG("Unknown endpoint type", 0);
                    endpointTimeout = 0;
                    break;
                }

                maxEndpointTimeout = max(maxEndpointTimeout,
                                         endpointTimeout);

            }

        }

        if (disableInitiateTransition) {
            newTimeout = 0xFF;
            LEAVE;
        }

        //
        // If we are enabling host initiated U2 transitions and there
        // is an interrupt endpoint with periodic usage then we
        // want to disable host initiated U1 transitions
        //
        if (periodicInterruptEndpointPresent &&
            (DeviceContext->TargetU1Timeout > 0)) {
            DeviceContext->TargetU1Timeout = 0xFF;
        }

        if (maxEndpointTimeout < (0xFE * 256)) {
            //
            // Add one to ensure it is not 0
            //
            newTimeout = (UCHAR) (maxEndpointTimeout/256 + 1);

        } else {
            newTimeout = 0XFE;
        }


    } FINALLY {

        if ((DeviceContext->HubFdoContext->HubInformation.HubFlags.DisallowU2AcceptOnlyWithU1AcceptAndInitiate == 1) &&
            (DeviceContext->DeviceFlags.DeviceIsHub == 0) &&
            (DeviceContext->TargetU1Timeout > 0) &&
            (DeviceContext->TargetU1Timeout < 0xFF) &&
            (newTimeout == 0xFF)) {

            //
            // Some host controllers have this strange issue that if
            // U1 is set to accept and enable and U2 is set to accept
            // only then they will keep rejecting U2 transitions. So
            // we can't have both U1 and U2 enabled. We will go with
            // keeping U1 enabled for devices in case they don't
            // tolerate U2 latency
            //
            newTimeout = 0;
            INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, TargetU2EnabledForUSPort);
        }

        DeviceContext->TargetU2Timeout = newTimeout;

    }
}

USHORT
HUBMISC_ComputeDelayInNanoSecondsFromSpeedAttr (
    __in
        PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  SpeedAttr,
    __in
        ULONG                                        LaneCount
    )
{
    ULONGLONG  speed;

    switch (SpeedAttr->LaneSpeedExponent)
    {
    case USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_KBPS:
        speed = 1000;
        break;
    case USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_MBPS:
        speed = 1000000;
        break;
    case USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_LSE_GBPS:
        speed = 1000000000;
        break;
    default:
        speed = 1;
        break;
    }

    speed *= SpeedAttr->LaneSpeedMantissa;
    speed *= (LaneCount + 1);

    //
    // Make sure that the speed is at least 50Mb/s to avoid overflows in the code.
    //
    if (speed < 50000000) {
        return USHORT_MAX;
    }

    //
    // Each TP is 20 symbols, speed is in bps, and result is in ns.
    //
    return (USHORT)((20 * 10 * 1000000000ULL) / speed);
}

DSM_EVENT
HUBMISC_SetSpeedFlagFor30TypeDevice (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
{
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  speeds;
    size_t                                       speedsCount;
    size_t                                       i;
    ULONG                                        speedID;
    USHORT                                       delay;
    ULONG                                        laneCount;
    DSM_EVENT                                    retEvent;

    TRY {

        DeviceContext->RxTpDelay = USB3_5GBPS_TP_LINK_DELAY_IN_NANO_SECONDS;
        DeviceContext->TxTpDelay = USB3_5GBPS_TP_LINK_DELAY_IN_NANO_SECONDS;

        retEvent = DsmEventOperationSuccess;

        if (DeviceContext->PortContext->PortInformation.PortFlags.PortFlagEnhancedSuperSpeed == 0) {
            LEAVE;
        }

        speeds = DeviceContext->PortContext->PortInformation.SublinkSpeedAttr;
        speedsCount = DeviceContext->PortContext->PortInformation.SublinkSpeedAttrCount;
        speedID = DeviceContext->PortContext->CurrentExtPortStatusChange.PortExtStatus.RxSublinkSpeedID;

        for (i = 0; i < speedsCount; i++) {
            if ((speeds[i].SublinkSpeedAttrID == speedID) &&
                (speeds[i].SublinkTypeDir == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX)) {

                laneCount = DeviceContext->PortContext->CurrentExtPortStatusChange.PortExtStatus.RxLaneCount;
                delay = HUBMISC_ComputeDelayInNanoSecondsFromSpeedAttr(&speeds[i], laneCount);
                if (delay == USHORT_MAX) {
                    retEvent = DsmEventOperationFailure;
                    LEAVE;
                }
                DeviceContext->RxTpDelay = delay;
                speedID = ULONG_MAX;
                break;
            }
        }
        NT_ASSERT(speedID == ULONG_MAX);

        speedID = DeviceContext->PortContext->CurrentExtPortStatusChange.PortExtStatus.TxSublinkSpeedID;

        for (i = 0; i < speedsCount; i++) {

            if ((speeds[i].SublinkSpeedAttrID == speedID) &&
                (speeds[i].SublinkTypeDir == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX)) {

                laneCount = DeviceContext->PortContext->CurrentExtPortStatusChange.PortExtStatus.TxLaneCount;
                delay = HUBMISC_ComputeDelayInNanoSecondsFromSpeedAttr(&speeds[i], laneCount);
                if (delay == USHORT_MAX) {
                    retEvent = DsmEventOperationFailure;
                    LEAVE;
                }
                DeviceContext->TxTpDelay = delay;
                speedID = ULONG_MAX;
                break;
            }
        }
        NT_ASSERT(speedID == ULONG_MAX);

    } FINALLY {

    }

    return retEvent;
}

BOOLEAN
HUBMISC_IsDeviceSuperSpeedPlusCapableOrHigher (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
{
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  speeds;
    size_t                                       speedsCount;
    size_t                                       i;
    BOOLEAN                                      retval;

    TRY {

        retval = FALSE;

        if (DeviceContext->DeviceFlags.DeviceIsEnhancedSuperSpeedCapable == 0) {
            LEAVE;
        }

        speedsCount = DeviceContext->SublinkSpeedAttrCount;
        speeds = DeviceContext->SublinkSpeedAttr;

        for (i = 0; i < speedsCount; i++) {
            if (speeds[i].LinkProtocol != USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS) {
                retval = TRUE;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return retval;
}

BOOLEAN
HUBMISC_IsDeviceOperatingAtSuperSpeedPlusOrHigher (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
{
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  speeds;
    size_t                                       speedsCount;
    size_t                                       i;
    ULONG                                        speedID;
    BOOLEAN                                      retval;

    TRY {

        retval = FALSE;

        if (DeviceContext->DeviceFlags.DeviceIsEnhancedSuperSpeedCapable == 0 ||
            DeviceContext->PortContext->PortInformation.PortFlags.PortFlagEnhancedSuperSpeed == 0) {
            LEAVE;
        }

        speeds = DeviceContext->PortContext->PortInformation.SublinkSpeedAttr;
        speedsCount = DeviceContext->PortContext->PortInformation.SublinkSpeedAttrCount;
        speedID = DeviceContext->PortContext->CurrentExtPortStatusChange.PortExtStatus.RxSublinkSpeedID;

        for (i = 0; i < speedsCount; i++) {
            if ((speeds[i].SublinkSpeedAttrID == speedID) &&
                (speeds[i].SublinkTypeDir == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_RX)) {

                if (speeds[i].LinkProtocol != USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS) {
                    retval = TRUE;
                    LEAVE;
                }
                break;
            }
        }

        for (i = 0; i < speedsCount; i++) {
            if ((speeds[i].SublinkSpeedAttrID == speedID) &&
                (speeds[i].SublinkTypeDir == USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_DIR_TX)) {

                if (speeds[i].LinkProtocol != USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED_PROTOCOL_SS) {
                    retval = TRUE;
                    LEAVE;
                }
                break;
            }
        }

    } FINALLY {

    }

    return retval;
}

VOID
HUBMISC_ComputeMaximumExitLatency (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Computes the worst case exit latency based on the current
    enabled U states and any adjustment based on No ping response
    from controller.

Returns:

--*/
{
    ULONG       exitLatency;
    ULONG       tMEL1;
    ULONG       tMEL2;
    BOOLEAN     u1Enabled;
    BOOLEAN     u2Enabled;

    TRY {

        exitLatency = 0;

        //
        // U1 is effectively disabled if either host can't accept the
        // transition or the host can only accept and the device can't
        // initiate it
        //
        if ((DeviceContext->TargetU1Timeout == 0) ||
            ((DeviceContext->LPMStateFlags.TargetU1EnabledForUSPort == 0) &&
             (DeviceContext->TargetU1Timeout == 0xFF))) {

            u1Enabled = FALSE;

        } else {

            u1Enabled = TRUE;
        }

        //
        // U2 is effectively disabled if either host can't accept the
        // transition or the host can only accept and the device can't
        // initiate it
        //
        if ((DeviceContext->TargetU2Timeout == 0) ||
            ((DeviceContext->LPMStateFlags.TargetU2EnabledForUSPort == 0) &&
             (DeviceContext->TargetU2Timeout == 0xFF))) {

            u2Enabled = FALSE;

        } else {

            u2Enabled = TRUE;
        }

        //
        // If both U1 and U2 are going to be disabled then
        // latency is 0
        //
        if ((u1Enabled == FALSE) &&
            (u2Enabled == FALSE)) {
            LEAVE;
        }

        //
        // Implemented as per section C.1.5.2 in USB 30 spec
        //

        //
        // tMEL1 is the host initiated exit latency
        //
        if (u2Enabled) {
            tMEL1 = DeviceContext->HostInitiatedU2ExitLatency * 1000;
        } else {
            tMEL1 = DeviceContext->HostInitiatedU1ExitLatency * 1000;
        }

        //
        // tMEL2 is the time for a PING TP to traverse the interconnect
        // hierarchy from the host to the device
        //
        tMEL2 = DeviceContext->HubFdoContext->HubInfoFromParent.TotalTPPropogationDelay
            + DeviceContext->HubFdoContext->HubDescriptor.Hub30Descriptor.wHubDelay
            + DeviceContext->TxTpDelay;

        //
        // tMEL3 is the time for the device to receive PING and generate
        // the PING_RESPONSE = tPingResponse
        //
        // tMEL4 is the time for PING_RESPONSE to traverse back from device
        // to host. This is same as tMEL2. But in addition, tMEL4 also has to
        // account for PING response getting stuck behind a DP.  Technically,
        // when a PING is received by the device, the device may be starting
        // to respond to a transaction on a different endpoint, so this could
        // add a delay even if there are no hubs.  However, if the links are
        // all in their lowest power state, then the device can't have a DP in
        // progress for the PING response to get stuck behind. So we will take
        // the maximum of exit latencies and this delay
        //
        exitLatency = max(tMEL1,FORWARDING_DELAY_DUE_TO_ONE_MAXIMUM_PACKET_SIZE_IN_NANOSECONDS)
                      + tMEL2
                      + T_PING_RESPONSE_IN_NANO_SECONDS //tMEL3
                      + tMEL2; //tMEL4

        //
        // Now adjust the exit latency based on any No ping response errors
        // received in the past.
        //
        exitLatency += (DeviceContext->LatencyAdjustmentPercentForNoPingError * exitLatency)/100;

        //
        // Convert from nano-seconds to micro-seconds
        //
        exitLatency = (exitLatency + 500)/1000;

    } FINALLY {

        DeviceContext->TargetEffectiveExitLatency = (USHORT)exitLatency;

    }

}

DSM_EVENT
HUBMISC_CheckIfMaxExitLatencyNeedsToBeIncreased (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if Device Exit latency needs to be increased

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    return ((DeviceContext->TargetEffectiveExitLatency > DeviceContext->EffectiveExitLatency) ? DsmEventYes:DsmEventNo);
}

DSM_EVENT
HUBMISC_CheckIfMaxExitLatencyNeedsToBeDecreased (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if Device Exit latency needs to be decreased

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    return ((DeviceContext->TargetEffectiveExitLatency < DeviceContext->EffectiveExitLatency) ? DsmEventYes:DsmEventNo);
}

DSM_EVENT
HUBMISC_DisableU1U2OnExitLatencyTooLargeError (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Disables U2 to reduce exit latency. If U2 is already
    dsiabled, then disables U1. If both are already disabled
    then return Failure

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT       dsmEvent;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        if (DeviceContext->TargetU2Timeout != 0) {
            INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U2DisabledDueToIntolerableLatency);
        } else if (DeviceContext->TargetU1Timeout != 0){
            INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1DisabledDueToIntolerableLatency);     
        } else {
            //
            // If both U1 and U2 have been disabled, then the exit latency
            // should already be 0. Controller seems to be really confused.
            //
            NT_ASSERT(DeviceContext->TargetEffectiveExitLatency == 0);
            dsmEvent = DsmEventOperationFailure;
        }

    } FINALLY {

    }

    return dsmEvent;

}

DSM_EVENT
HUBMISC_SetLatencyAdjustmentOnNoPingResponse (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Increases the adjustment factor for exit latency based on some
    heursitics. If the factor has already been increased enough,
    then return failure

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT       dsmEvent;
    USHORT          tempAdjustmentValue;

    TRY {
        dsmEvent = DsmEventOperationSuccess;

        if (DeviceContext->LatencyAdjustmentPercentForNoPingError == 0) {
            DeviceContext->LatencyAdjustmentPercentForNoPingError =
                LATENCY_ADJUSTMENT_INITIAL_VALUE;
            LEAVE;
        }

        tempAdjustmentValue = DeviceContext->LatencyAdjustmentPercentForNoPingError *
            LATENCY_ADJUSTMENT_MULTIPLIER;

        if (tempAdjustmentValue > LATENCY_ADJUSTMENT_MAXIMUM_VALUE) {

            if ((DeviceContext->U1Timeout == 0) &&
                (DeviceContext->U2Timeout == 0)) {
                //
                // We are getting a No Ping Response Error even though U1
                // U2 are disabled on this device. So the controller seems
                // to be in a bad state
                //
                dsmEvent = DsmEventOperationFailure;
                LEAVE;
            }

            //
            // We have given enough leeway to the hardware. Let us just disable
            // U1 U2 on this device
            //
            INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, U1U2DisabledDueToNoPingResponse);
            LEAVE;
        }

        DeviceContext->LatencyAdjustmentPercentForNoPingError = (UCHAR)tempAdjustmentValue;

    } FINALLY {

    }

    return dsmEvent;

}

DSM_EVENT
HUBMISC_QueryAndCacheRegistryValuesForDevice (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    NTSTATUS                status;
    CHAR                    vendorId[MAX_VENDOR_ID_STRING_LENGTH];
    CHAR                    deviceId[MAX_DEVICE_ID_STRING_LENGTH];
    CHAR                    revisionId[MAX_REVISION_ID_STRING_LENGTH];


    PAGED_CODE();

    TRY {

        HUBMISC_ConvertUsbDeviceIdsToString(&DeviceContext->DeviceDescriptor,
                                            vendorId,
                                            deviceId,
                                            revisionId);


        status = HUBREG_QueryUsbflagsValuesForDevice(DeviceContext,
                                                     vendorId,
                                                     deviceId,
                                                     revisionId);

        //
        // Get the Hw Verifier flags, Ignore the return value
        //
        HUBREG_QueryUsbHardwareVerifierValue(&DeviceContext->DeviceDescriptor,
                                             vendorId,
                                             deviceId,
                                             revisionId,
                                             &g_HwVerifierDeviceName,
                                             DeviceContext->PortContext->IfrLog,
                                             &DeviceContext->HardwareVerifierFlags);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DeviceContext->EnumMsgId = USBENUM_REGISTRY_FAILURE;

            EventWriteUSBHUB3_ETW_EVENT_REGISTRY_FAILURE(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                status);
        }
    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBMISC_CheckIfSameDeviceIsConnected (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Compares the device descriptor obtained during enumeration with the cached
    device descriptor from before and determine if the device connected after
    system resume is the same device that was connected before system went to
    sleep.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT       dsmEvent;

    TRY {

        dsmEvent = DsmEventYes;

        if (DeviceContext->MsOs20Info.Flags.AlternateEnumeration == 1) {

            if (RtlCompareMemory(DeviceContext->ScratchBuffer,
                                 &DeviceContext->MsOs20Info.OriginalDeviceDescriptor,
                                 sizeof(USB_DEVICE_DESCRIPTOR)) !=
                                   sizeof(USB_DEVICE_DESCRIPTOR)) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "New device descriptor does not match cached original device descriptor");

                dsmEvent = DsmEventNo;
            }
        } else if (RtlCompareMemory(DeviceContext->ScratchBuffer,
                                    &DeviceContext->DeviceDescriptor,
                                    sizeof(USB_DEVICE_DESCRIPTOR)) !=
                                      sizeof(USB_DEVICE_DESCRIPTOR)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "New device descriptor does not match cached device descriptor");

            dsmEvent = DsmEventNo;
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    PINTERFACE_HANDLE   interfaceHandle;
    PPIPE_HANDLE        pipeHandle;
    ULONG               i;
    UCXENDPOINT         *endpointsToEnable;
    UCXENDPOINT         *endpointsToRemainUnchanged;

    TRY {

        DeviceContext->EndpointsToBeEnabledCount = 0;
        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        DeviceContext->EndpointsToBeDisabledCount = 0;

        endpointsToEnable = DeviceContext->EndpointsToBeEnabled;
        endpointsToRemainUnchanged = DeviceContext->EndpointsToRemainUnchanged;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceContext->CurrentDeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            if (interfaceHandle->InterfaceFlags.HasAlternateSettings == 1) {

                INTERLOCKED_SET_BIT_FIELD(INTERFACE_FLAGS, &interfaceHandle->InterfaceFlags, NeedsSetInterface);
            }

            //
            // Walk the endpoint handles for this interface
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (pipeHandle->EndpointFlags.ZeroBandwidth == 1) {
                    //
                    // Don't enable zero bandwidth endpoints
                    //

                    pipeHandle->UcxEndpointState = UcxEndpointStateDisabled;

                    endpointsToRemainUnchanged[DeviceContext->EndpointsToRemainUnchangedCount++]
                        = pipeHandle->UcxEndpoint;

                } else {
                    pipeHandle->UcxEndpointState = UcxEndpointStatePendingEnable;

                    endpointsToEnable[DeviceContext->EndpointsToBeEnabledCount++] =
                        pipeHandle->UcxEndpoint;
                }
            }
        }

    } FINALLY {

    }

    return DsmEventOperationSuccess;
}


DSM_EVENT
HUBMISC_QueryCachedRegistryIfRequestConfigDescOnResetIsSet (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    return ((DeviceContext->DeviceHackFlags.RequestConfigDescOnReset == 1) ? DsmEventYes : DsmEventNo);
}

PSM_EVENT
HUBMISC_CheckIfConnectedDeviceSupportsReAttach (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:


Returns:


--*/
{
    return ((PortContext->PortFlags.ConnectedDeviceSupportsReAttach == 1) ? PsmEventYes:PsmEventNo);
}

PSM_EVENT
HUBMISC_CheckIfHubIsArmedForWake (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:


Returns:


--*/
{
    return ((PortContext->HubFdoContext->HubInformation.HubFlags.ArmedForWake == 1) ? PsmEventYes:PsmEventNo);
}

DSM_EVENT
HUBMISC_CheckIfDeviceProgrammingWasLost (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT               retEvent;
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;
    PPIPE_HANDLE            pipeHandle;
    ULONG                   i;

    TRY {
        if (DeviceContext->DeviceStateFlags.DeviceReprogrammingPending == 0) {
            retEvent = DsmEventNo;
            LEAVE;
        }

        retEvent = DsmEventYes;
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceReprogrammingPending);

        if (DeviceContext->PdoContext) {
            INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ProgrammingLostOnLastReset);
        }

        //
        // We need to mark all the endpoints as disabled because controller
        // lost all its programming
        //
        configuration = DeviceContext->CurrentDeviceConfiguration;

        if (configuration == NULL) {
            LEAVE;
        }

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (pipeHandle->UcxEndpointState == UcxEndpointStateEnabled) {

                    pipeHandle->UcxEndpointState = UcxEndpointStateDisabled;

                }
            }

        }

    } FINALLY {

    }

    return retEvent;
}

VOID
HUBMISC_ClearDeviceNeedsReprogrammingFlag (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceReprogrammingPending);
}


VOID
HUBMISC_SetAttachStatusToFailure (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{

    TRY {

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceAttachSuccessful);

    } FINALLY {

    }
}


PSM_EVENT
HUBMISC_AttachDevice (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Queues event DsmEventPortAttachDevice to DSM. As part of processing
    this event, DSM is going to synchronously call either
    HUBMISC_SetAttachStatusToSuccess or HUBMISC_SetAttachStatusToFailure.
    This function will look at the AttachStatus variable in the device context
    and return PsmEventOperationSucces or PsmEventOperationFailure.

Return Value:

    PsmEventOperationSuccess
    PsmEventOperationFailure

--*/
{
    PSM_EVENT           psmEvent;

    TRY {

        NT_ASSERT(PortContext->DeviceContext != NULL);

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &PortContext->DeviceContext->DeviceStateFlags, DeviceAttachSuccessful);
        INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, ConnectedDeviceSupportsReAttach);

        HUBSM_AddDsmEvent(PortContext->DeviceContext, DsmEventPortAttachDevice);

    } FINALLY {

        if (PortContext->DeviceContext->DeviceStateFlags.DeviceAttachSuccessful == 1) {
            INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, DeviceConnected);
            psmEvent = PsmEventOperationSuccess;
            PortContext->ConnectionStatus = DeviceEnumerating;
        } else {
            psmEvent = PsmEventOperationFailure;
            PortContext->ConnectionStatus = DeviceGeneralFailure;

        }
    }

    return psmEvent;
}

VOID
HUBMISC_DetachDevice (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Queues event DsmEventPortDetachDevice to DSM and
    calls into UCX to start purging any ongoing
    transfers to the device connected to the port

Return Value:

--*/
{

    TRY {

        NT_ASSERT(PortContext->DeviceContext != NULL);

        HUBSM_AddDsmEvent(PortContext->DeviceContext, DsmEventPortDetachDevice);
        if (PortContext->PortFlags.ConnectedDeviceSupportsReAttach == 0) {
            HUBUCX_DisconnectDeviceInUCX(PortContext);
        }

    } FINALLY {

    }

}

VOID
HUBMISC_ReAttachDevice (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Queues event DsmEventPortReAttachDevice to DSM and
    updates the internal port state. This is used only
    for devices that support special files and can not
    be reported as missing to PnP

Return Value:


--*/
{

    TRY {

        NT_ASSERT(PortContext->DeviceContext != NULL);

        HUBSM_AddDsmEvent(PortContext->DeviceContext,
                          DsmEventPortReAttachDevice);
        INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, DeviceConnected);

        PortContext->ConnectionStatus = DeviceEnumerating;


    } FINALLY {

    }

}

PSM_EVENT
HUBMISC_IsConnectBitOne (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the port status and determines if the port connect bit is set.
    This works for both 2.0 and 3.0 ports as the connect bit is at the same
    place in the status.

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;
    USB_20_PORT_STATUS      portStatus;


    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;

        portEvent = (portStatus.CurrentConnectStatus == 1) ? PsmEventYes : PsmEventNo;

    } FINALLY {

    }

    return portEvent;
}

PSM_EVENT
HUBMISC_IsOvercurrentBitOne(
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the port status and determines if the port overcurrent bit is set.
    This works for both 2.0 and 3.0 ports as the overcurrent bit is at the same
    place in the status.

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;
    USB_20_PORT_STATUS      portStatus;


    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;

        if (portStatus.OverCurrent == 1) {
            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;
            portEvent = PsmEventYes;
        } else {
            portEvent = PsmEventNo;
        }

    } FINALLY {

    }

    return portEvent;
}

PSM_EVENT
HUBMISC_Is30DevicePresent (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the 30 port status and determines if a device is connected
Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;
    USB_30_PORT_STATUS      portStatus;


    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus;
        //
        // If the connect bit is 1, then the device is connected. But it is also
        // possible that the device failed one of the early stages of SuperSpeed
        // enumeration , which is indicated by one of the following port link states
        //
        if ((portStatus.CurrentConnectStatus == 1) ||
            (portStatus.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE) ||
            (portStatus.PortLinkState == PORT_LINK_STATE_INACTIVE)) {
            portEvent = PsmEventYes;
        } else {
            portEvent = PsmEventNo;
        }

    } FINALLY {

    }

    return portEvent;
}

PSM_EVENT
HUBMISC_IsLinkStateU0 (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the 30 port status and determines if the link state is in U0, U1
    or U2

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;
    USB_30_PORT_STATUS      portStatus;


    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus;

        if ((portStatus.PortLinkState == 0) ||
            (portStatus.PortLinkState == 1) ||
            (portStatus.PortLinkState == 2)) {
            portEvent = PsmEventYes;
        } else {
            portEvent  = PsmEventNo;
        }

    } FINALLY {

    }

    return portEvent;
}

PSM_EVENT
HUBMISC_IsPortPoweredOn (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the port status and determines if a 3.0 port is
    powered on

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;
    USB_30_PORT_STATUS      portStatus;


    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus;

        portEvent = (portStatus.PortPower == 1) ? PsmEventYes : PsmEventNo;

    } FINALLY {

    }

    return portEvent;

}

PSM_EVENT
HUBMISC_CheckAndSelectIfConnectChangeSetOnResume (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Checks if the Connect Change bit is set on resume.
    If Yes, then it sets the CurrentFeatureSelector, so
    that this change will be acknowledged next. It does
    the same thing for suspend change. Note that the name
    of the function is not accurate with addition of
    suspend change. Will be fixed in Win9.


Return Value:

    VOID

--*/
{
    PSM_EVENT                       retEvent;


    TRY {

        if (PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange.ConnectStatusChange) {

            PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange.ConnectStatusChange = 0;
            PortContext->CurrentFeatureSelector =  C_PORT_CONNECTION;
            retEvent =  PsmEventYes;

        } else if (PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange.SuspendChange) {

            PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange.SuspendChange = 0;
            PortContext->CurrentFeatureSelector =  C_PORT_SUSPEND;
            retEvent =  PsmEventYes;

        } else {

            retEvent = PsmEventNo;

        }

    } FINALLY {

    }

    return retEvent;
}

PSM_EVENT
HUBMISC_CheckAndSelectIfBhResetChangeSet (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Checks if the Bh Reset Change bit is set.This wiil
    be called on reset complete to see if it was a warm
    reset.
    If Yes, then it sets the CurrentFeatureSelector, so
    that this change will be acknowledged next.

Return Value:

    VOID

--*/
{
    PSM_EVENT                       retEvent;


    TRY {

        if (PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb30PortChange.BHResetChange) {

            PortContext->CurrentFeatureSelector =  C_BH_PORT_RESET;
            retEvent =  PsmEventYes;

        } else {

            retEvent = PsmEventNo;

        }

    } FINALLY {

    }

    return retEvent;
}

VOID
HUBMISC_UpdateCurrentPortStatusAsDisabled (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    After the control transfer for disable port is
    successfully completed, update our internal current
    port status to match that.

Return Value:

    VOID

--*/
{

    TRY {

        PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.PortEnabledDisabled = 0;

    } FINALLY {

    }

}

VOID
HUBMISC_UpdateCurrentPortStatusAsSuspended (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    After the control transfer for suspend port is
    successfully completed, update our internal current
    port status to match that.

Return Value:

    VOID

--*/
{

    TRY {

        PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.Suspend = 1;

    } FINALLY {

    }

}

DSM_EVENT
HUBMISC_CheckIfMSOSDescriptorShouldBeQueried (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT               dsmEvent;
    PHUB_GLOBALS            hubGlobals;

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {
            dsmEvent = DsmEventNo;
            LEAVE;
        }

        switch (hubGlobals->MsOsMode) {
        case MsOsModeForceQuery:
            //
            // COMPAT: Registry flag MsOsDescriptorMode == 1 will force a MS OS
            //         descriptor query for all devices, regardless of USB
            //         version number.
            //
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, MsOsDescriptorNotSupported);
            dsmEvent = DsmEventYes;
            break;

        case MsOsModeDisableQuery:
            //
            // COMPAT: Registry flag MsOsDescriptorMode == 2 will disable all MS OS
            //         descriptor queries.
            //
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, MsOsDescriptorNotSupported);
            dsmEvent = DsmEventNo;
            break;

        default:
            //
            // Generally MS OS descriptor is supported only
            // for devices that are >= 0x200.
            //
            // But there are devices that report themselves as
            // 0x0110 instead of 0x0200 when operating in full-speed
            // mode but implement MsOs descriptor and require
            // the MsOs descriptor query for proper compatible id
            // enumeration. For these devices we support the
            // registry flag "DontSkipMsOsDescriptor".
            //
            // Finally there are devices that will report invalid
            // value (i.e. less than 0x100) in thier bcdUsb field.
            // These devices are treated as 1.1 for most part but
            // they happen to be queried for MS OS descriptor in the
            // EHCI stack and so we have to still query them for
            // maintaining compat.
            //
            if (((DeviceContext->DeviceDescriptor.bcdUSB >= 0x200) ||
                 (DeviceContext->DeviceDescriptor.bcdUSB < 0x100)) &&
                (DeviceContext->DeviceFlags.MsOsDescriptorNotSupported == 0) &&
                (DeviceContext->MsOsVendorCode == 0)) {

                dsmEvent = DsmEventYes;

            } else if (DeviceContext->DeviceHackFlags.DontSkipMsOsDescriptor == 1) {

                //
                // This is set for 1.1 devices that need MS OS descriptor support
                //
                dsmEvent = DsmEventYes;

            } else{

                dsmEvent = DsmEventNo;

            }

            break;
        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfContainerIdShouldBeQueried (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT       dsmEvent;

    TRY {
        if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {

            dsmEvent = DsmEventNo;

        } else if (((DeviceContext->DeviceDescriptor.bcdUSB >= 0x200) ||
                    (DeviceContext->DeviceDescriptor.bcdUSB < 0x100)) &&
                   (DeviceContext->DeviceFlags.MsOsDescriptorNotSupported == 0) &&
                   (DeviceContext->DeviceHackFlags.SkipContainerIdQuery == 0)) {

            dsmEvent = DsmEventYes;

        } else if (DeviceContext->DeviceHackFlags.DontSkipMsOsDescriptor == 1) {

            //
            // This is set for 1.1 devices that need MS OS descriptor support
            //
            dsmEvent = DsmEventYes;

        } else{

            dsmEvent = DsmEventNo;

        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckMSOSDescriptorIfContainerIdIsSupported (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT       dsmEvent;

    TRY {

        if (DeviceContext->MsOsDescriptor.bFlags & MS_OS_FLAGS_CONTAINERID){

            dsmEvent = DsmEventYes;

        } else {

            dsmEvent = DsmEventNo;

        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_QueryCachedRegistryIfIgnoreHWSerNumIsSet (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    return (DeviceContext->DeviceHackFlags.DisableSerialNumber == 1) ? DsmEventYes : DsmEventNo;
}


DSM_EVENT
HUBMISC_IsDeviceArmedForWake (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the device should be armed for wake via SET_FEATURE/REMOTE_WAKEUP.

Returns:


--*/
{
    DSM_EVENT   result;

    TRY {

        if (DeviceContext->PdoContext->PdoFlags.ArmedForWake == 1) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;

}

DSM_EVENT
HUBMISC_ShouldDeviceBeArmedForWake (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the device should be armed for wake via SET_FEATURE/REMOTE_WAKEUP.

Returns:


--*/
{
    DSM_EVENT   result;

    TRY {

        if (DeviceContext->PdoContext->PdoFlags.RequiresArmingForWake == 1) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;

}

DSM_EVENT
HUBMISC_IsDeviceSuperSpeedCapable (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines, from the BOS Descriptors, if the device is capable
    of working at Super Speed.

Returns:


--*/
{

    if (DeviceContext->DeviceFlags.DeviceIsSuperSpeedCapable == 1) {
        if ((DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagPort20) &&
            (DeviceContext->DeviceFlags.DeviceIsHub == 0) &&
            (DeviceContext->DeviceHackFlags.DisableSuperSpeed == 0)) {

            //
            // If a SuperSpeed device is working in the 2.0 mode and it is not
            // one of those devices that require SuperSpeed disable, then
            // it could indicate an error. It is not necessarily an error
            // though because a SuperSpeed device could be behind a 2.0
            // hub
            //
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "SuperSpeed device is connected on the 2.0 bus");

            USB_HW_VERIFIER_DEVICE_BREAK(DeviceContext,DeviceHwVerifierSuperSpeedDeviceWorkingAtLowerSpeed);

            EventWriteUSBHUB3_ETW_EVENT_SSDEVICE_CONNECTED_AT_LS(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);

        }

        return DsmEventYes;
    } else {
        return DsmEventNo;
    }

}


VOID
HUBMISC_SetConfigurationInformationInClientRequest(
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    PDEVICE_CONFIGURATION           configuration;
    ULONG                           i;
    WDFREQUEST                      request;
    PURB                            urb;
    WDF_REQUEST_PARAMETERS          parameters;
    PUCHAR                          informationPtr;
    PINTERFACE_HANDLE               interfaceHandle;
    PUSBD_INTERFACE_INFORMATION     interfaceInformation;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubControllerStackInterface = &DeviceContext->HubFdoContext->HubControllerStackInterface;
        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        urb = parameters.Parameters.Others.Arg1;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        informationPtr = (PUCHAR)&urb->UrbSelectConfiguration.Interface;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            interfaceInformation = (PUSBD_INTERFACE_INFORMATION) informationPtr;

            //
            // Walk the endpoint handles for this interface
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                PPIPE_HANDLE       pipeHandle;
                USBD_PIPE_HANDLE   usbdPipeHandle;
                ULONG              maxTransferSize;

                pipeHandle = &interfaceHandle->PipeHandle[i];

                usbdPipeHandle = hubControllerStackInterface->EndpointGetUsbdPipeHandle(
                    pipeHandle->UcxEndpoint);

                if (usbdPipeHandle == NULL) {
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "NULL pipe handle returned by EndpointGetUsbdPipeHandle!");

                    NT_ASSERTMSG("NULL pipe handle returned by EndpointGetUsbdPipeHandle!", 0);
                }

                maxTransferSize = hubControllerStackInterface->EndpointGetMaximumTransferSize(
                    pipeHandle->UcxEndpoint);

                pipeHandle->UsbdPipeHandle = usbdPipeHandle;
                interfaceInformation->Pipes[i].PipeHandle = usbdPipeHandle;

                hubControllerStackInterface->EndpointExposedToClient(pipeHandle->UcxEndpoint);
                interfaceInformation->Pipes[i].MaximumTransferSize = maxTransferSize;
            }

            informationPtr += interfaceInformation->Length;
        }

    } FINALLY {

    }
}


VOID
HUBMISC_SetInterfaceInformationInClientRequest(
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    PDEVICE_CONFIGURATION           configuration;
    ULONG                           i;
    WDFREQUEST                      request;
    PURB                            urb;
    WDF_REQUEST_PARAMETERS          parameters;
    PUSBD_INTERFACE_INFORMATION     interfaceInformation;
    PINTERFACE_HANDLE               interfaceHandle;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubControllerStackInterface = &DeviceContext->HubFdoContext->HubControllerStackInterface;
        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        urb = parameters.Parameters.Others.Arg1;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        interfaceInformation = &urb->UrbSelectInterface.Interface;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {
            if (interfaceHandle->InterfaceDescriptor->bInterfaceNumber ==
                interfaceInformation->InterfaceNumber) {

                //
                // Walk the endpoint handles for this interface
                //
                for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                    PPIPE_HANDLE       pipeHandle;
                    USBD_PIPE_HANDLE   usbdPipeHandle;

                    pipeHandle = &interfaceHandle->PipeHandle[i];

                    usbdPipeHandle = hubControllerStackInterface->EndpointGetUsbdPipeHandle(
                        pipeHandle->UcxEndpoint);

                    if (usbdPipeHandle == NULL) {
                        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "USBD Pipe Handle is NULL!");
                    }

                    pipeHandle->UsbdPipeHandle = usbdPipeHandle;
                    interfaceInformation->Pipes[i].PipeHandle = usbdPipeHandle;

                    hubControllerStackInterface->EndpointExposedToClient(pipeHandle->UcxEndpoint);
                }

                LEAVE;
            }
        }

        NT_ASSERTMSG("Unable to find matching interface handle for selected interface", FALSE);

    } FINALLY {

    }
}


PSM_EVENT
HUBMISC_CheckIfOldDeviceIsStillAttached (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Checks if Old DSM is still attached to the PSM.

Return Value:

    VOID

--*/
{
    PSM_EVENT               psmEvent;


    TRY {

       if (PortContext->PortFlags.DeviceConnected == 0) {
           psmEvent = PsmEventNo;
       } else{
           psmEvent = PsmEventYes;
       }

    } FINALLY {

    }

    return psmEvent;
}


extern PSTATE_ENTRY     DSMStateTable[];
EVT_WDF_TIMER           HUBPDO_EvtBootResetTimer;


PSM_EVENT
HUBMISC_CreateNewDSM (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Allocates memory and context for new DSM.
    Initializes DSM and device context, and adds it to the hub's DSM list.

Return Value:

    VOID

--*/
{
    WDFOBJECT               dsmObject;
    PDEVICE_CONTEXT         deviceContext;
    WDF_TIMER_CONFIG        timerConfig;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    NTSTATUS                status;
    NTSTATUS                activityIdStatus;
    PSM_EVENT               psmEvent;

    TRY {

        dsmObject = NULL;
        deviceContext = NULL;
        status = STATUS_SUCCESS;
        activityIdStatus = STATUS_UNSUCCESSFUL;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                DEVICE_CONTEXT);
        //
        // PSM will be responsible for explicitly
        // deleting this object

        objectAttributes.ParentObject = WdfObjectContextGetObject(PortContext);

        objectAttributes.EvtDestroyCallback = HUBMISC_EvtDsmDestroy;
        objectAttributes.ExecutionLevel = WdfExecutionLevelPassive;

        status = WdfObjectCreate(&objectAttributes, &dsmObject);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "WdfObjectCreate failed for creating DSM with status %!STATUS!",
                       status);
            dsmObject = NULL;
            LEAVE;
        }

        WdfObjectReferenceWithTag(WdfObjectContextGetObject(PortContext),
                                  (PVOID) DsmCreateTag);

        deviceContext = GetDeviceContext(dsmObject);

        RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));

        if (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.EtwEnabled == 1) {

            activityIdStatus = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                                    &deviceContext->DsmContext.CurrentActivityId);
        }

        deviceContext->PortContext = PortContext;
        deviceContext->HubFdoContext = PortContext->HubFdoContext;
        if (PortContext->PortInformation.PortFlags.PortFlagRemovable) {
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_PROPERTIES, &deviceContext->DeviceFlags, DeviceNotRemovable);
        } else {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &deviceContext->DeviceFlags, DeviceNotRemovable);
        }

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_START(
            &deviceContext->DsmContext.CurrentActivityId,
            deviceContext->HubFdoContext->HubParentInterface.Hub,
            PortContext->PortInformation.PortNumber);

        //
        // Create our pre-allocated request for DSM control transfers
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = WdfObjectContextGetObject(PortContext);

        status = WdfRequestCreate(&objectAttributes,
                                  WdfDeviceGetIoTarget(PortContext->HubFdoContext->HubFdoDevice),
                                  &deviceContext->ControlRequest.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfRequestCreate for DSM Control Transfer Failed %!STATUS!",
                       status);

            deviceContext->ControlRequest.WdfRequest = NULL;
            LEAVE;
        }

        //
        // This object is parented to the port and will be deleted
        // explicitly by the DSM when it is done. However the FDO
        // and the ports might get deleted before that. Therefore
        // DSM needs a reference on it
        //
        //
        WdfObjectReferenceWithTag(deviceContext->ControlRequest.WdfRequest,
                                  (PVOID) DsmTag);
        //
        // So that our debugger extension can get to the IRP and
        // auto diagnose stuck requests, we will store the IRP
        // in our context
        //
        deviceContext->ControlRequest.Irp = WdfRequestWdmGetIrp(deviceContext->ControlRequest.WdfRequest);

        //
        // Create our pre-allocated request for DSM UCX requests
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = WdfObjectContextGetObject(PortContext);

        status = WdfRequestCreate(&objectAttributes,
                                  WdfDeviceGetIoTarget(PortContext->HubFdoContext->HubFdoDevice),
                                  &deviceContext->UcxRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfRequestCreate for DSM UCX Request Failed %!STATUS!",
                       status);

            deviceContext->UcxRequest = NULL;
            LEAVE;
        }
        //
        // This object is parented to the port and will be deleted
        // explicitly by the DSM when it is done. However the FDO
        // and the ports might get deleted before that. Therefore
        // DSM needs a reference on it
        //
        //
        WdfObjectReferenceWithTag(deviceContext->UcxRequest,
                                  (PVOID) DsmTag);


        //
        // So that our debugger extension can get to the IRP and
        // auto diagnose stuck requests, we will store the IRP
        // in our context
        //
        deviceContext->UcxRequestIrp = WdfRequestWdmGetIrp(deviceContext->UcxRequest);
        //
        // We will query and cache the product ID in AMERICAN_ENGLISH
        // to begin with. If at a later time, we get a query
        // device text for different language, then we will
        // change this value
        //
        deviceContext->CurrentLanguageId = AMERICAN_ENGLISH;

        //
        // Initialize the DSM lock
        //
        KeInitializeSpinLock(&deviceContext->DsmContext.StateMachineLock);

        //
        // Set initial state for DSM
        //
        deviceContext->DsmContext.CurrentState[0].DsmState = StartIndexDsm;
        deviceContext->DsmContext.CurrentStateDepth = 0;
        deviceContext->DsmContext.SubSmFlags = 0;

        deviceContext->DsmContext.ParentContext = deviceContext;

        deviceContext->DsmContext.StateTable = DSMStateTable;
        deviceContext->DsmContext.StartIndex = StartIndexDsm;
        deviceContext->DsmContext.AddEvent = HUBSM_AddDsmEvent;
        //
        // This handle is used to identify the state machine in
        // the ETW events. So we will use the hub handle for now
        // and when we create a device in UCX, we will start using
        // that
        //
        deviceContext->DsmContext.ParentHandle = deviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceContext->DsmContext.HubFdoContext = deviceContext->HubFdoContext;

        HUBMISC_InitializeBitmap(&deviceContext->DescriptorValidationBitmapHeader,
                                 (PULONG)deviceContext->DescriptorValidationBitmap,
                                 sizeof(deviceContext->DescriptorValidationBitmap) * 8);
       
        //
        // Create the DSM timer
        //
        deviceContext->DsmContext.Timer =
            ExAllocateTimer(HUBMISC_DsmEventTimer,
                            deviceContext,
                            EX_TIMER_HIGH_RESOLUTION);

        if (deviceContext->DsmContext.Timer == NULL) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "HUBTIMER_AllocateTimer Failed");
            LEAVE;
        }

        //
        // Create the timer that is used for sending
        // out of bandwidth notification
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = WdfObjectContextGetObject(PortContext);
        objectAttributes.ExecutionLevel = WdfExecutionLevelPassive;

        WDF_TIMER_CONFIG_INIT( &timerConfig,
                               HUBPDO_EvtOutOfBandwidthTimer);

        status = WdfTimerCreate( &timerConfig,
                                 &objectAttributes,
                                 &deviceContext->OutOfBandwidthTimer);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfTimerCreate Failed %!STATUS!",
                       status);
            deviceContext->OutOfBandwidthTimer = NULL;
            LEAVE;
        }
        //
        // This object is parented to the port and will be deleted
        // explicitly by the DSM when it is done. However the FDO
        // and the ports might get deleted before that. Therefore
        // DSM needs a reference on it
        //
        //
        WdfObjectReferenceWithTag(deviceContext->OutOfBandwidthTimer,
                                  (PVOID) DsmTag);

        //
        // Initialize the DSM PnP/Power Event
        //
        KeInitializeEvent(&deviceContext->PnPPowerEvent, NotificationEvent, FALSE);

        KeInitializeEvent(&deviceContext->PreStartEvent, NotificationEvent, FALSE);

        KeInitializeEvent(&deviceContext->QueryDeviceTextEvent, NotificationEvent, FALSE);

        //
        // Create the worker item for the state machine that will be used to
        // execute state entry functions that require passive level
        //
        deviceContext->DsmContext.SmWorker =
            HUBMISC_AllocateWorkerItem(deviceContext->HubFdoContext,
                                       UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY);

        if (deviceContext->DsmContext.SmWorker == NULL) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HUBMISC_AllocateWorkerItem Failed");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT( &objectAttributes );
        objectAttributes.ParentObject = WdfObjectContextGetObject(PortContext);

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 USBHUB3_DEVICE_TAG,
                                 sizeof(UCX_IOCTL_REQUEST),
                                 &deviceContext->UcxIoctlRequestMemory,
                                 NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                        Device,
                        "WdfMemoryCreate Failed to create UCX_IOCTL_REQUEST %!STATUS!",
                        status);

            LEAVE;
        }

        //
        // This object is parented to the port and will be deleted
        // explicitly by the DSM when it is done. However the FDO
        // and the ports might get deleted before that. Therefore
        // DSM needs a reference on it
        //
        //
        WdfObjectReferenceWithTag(deviceContext->UcxIoctlRequestMemory,
                                  (PVOID) DsmTag);

        KeInitializeSpinLock(&deviceContext->ConfigurationLock);

        PortContext->DeviceContext = deviceContext;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "CreateNewDsm failed with status %!STATUS!",
                       status);

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_FAILED(
                deviceContext == NULL ?
                  0 :
                  &deviceContext->DsmContext.CurrentActivityId,
                PortContext->HubFdoContext->HubParentInterface.Hub,
                status);

            psmEvent = PsmEventOperationFailure;

            if (deviceContext != NULL) {
                HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->ControlRequest.WdfRequest);
                HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->UcxRequest);
                if (deviceContext->DsmContext.Timer) {
                    ExDeleteTimer(deviceContext->DsmContext.Timer,
                                  TRUE,
                                  FALSE,
                                  NULL);

                    deviceContext->DsmContext.Timer = NULL;
                }
                HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->OutOfBandwidthTimer);
                HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->UcxIoctlRequestMemory);

                if (deviceContext->DsmContext.SmWorker) {
                    HUBMISC_DeleteWorkerItem(deviceContext->HubFdoContext,
                                             deviceContext->DsmContext.SmWorker);
                    deviceContext->DsmContext.SmWorker = NULL;
                }

                HUBMISC_DeleteWdfObjectIfNotNull(&dsmObject);
            }

        } else {

            if (NT_SUCCESS(activityIdStatus)) {

                HUBMISC_SetActivityIdIrp(deviceContext->ControlRequest.Irp,
                                         &deviceContext->DsmContext.CurrentActivityId);

                HUBMISC_SetActivityIdIrp(deviceContext->UcxRequestIrp,
                                         &deviceContext->DsmContext.CurrentActivityId);

                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);

            }

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Port,
                       "Port 0x%p created child device 0x%p",
                       PortContext,
                       deviceContext);

            psmEvent = PsmEventOperationSuccess;
        }
    }

    return psmEvent;
}


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_FreeBufferIfNotNull(
    __in
        PVOID               Buffer,
    __in
        ULONG               Tag
    )
/*++

Routine Description:

    This routine is a helper routine that frees the memory
    if it is not NULL

Arguments:
    Buffer - Memory to be freed

Returns:

    VOID

--*/
{

    TRY {
        if (Buffer) {
            ExFreePoolWithTag(Buffer, Tag);
        }

    } FINALLY {

    }

    return;

} //HUBMISC_FreeBufferIfNotNull

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_DeleteWdfObjectIfNotNull(
    __in
        WDFOBJECT *             WdfObject
    )
/*++

Routine Description:

    This routine is a helper routine that frees the memory
    if it is not NULL

Arguments:
    Buffer - Memory to be freed

Returns:

    VOID

--*/
{

    TRY {
        if (*WdfObject) {
            WdfObjectDelete(*WdfObject);
            *WdfObject = NULL;
        }

    } FINALLY {

    }

    return;

} //HUBMISC_FreeBufferIfNotNull

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(
    __in
        WDFOBJECT *             WdfObject
    )
/*++

Routine Description:

    This routine is a helper routine that checks if the
    WDF object is NULL. If not, it deletes it and
    releases the reference that DSM acquired on it and
    then sets it to NULL

Arguments:
    WdfObject - Wdf object that needs to be deleted

Returns:

    VOID

--*/
{

    TRY {
        if (*WdfObject) {
            WdfObjectDelete(*WdfObject);
            WdfObjectDereferenceWithTag(*WdfObject,
                                        (PVOID) DsmTag);
            *WdfObject = NULL;
        }

    } FINALLY {

    }

    return;

} //HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull

VOID
HUBMISC_EvtDsmDestroy (
    WDFOBJECT       DsmObject
    )
/*++

Routine Description:

    Event callback to perform operations that are needed when the
    DSM is about to be destroyed. We will free memory that lives beyond the
    existence of PDO


Return Value:
    VOID

--*/
{
    PDEVICE_CONTEXT     deviceContext;

    TRY {

        deviceContext = GetDeviceContext(DsmObject);

        //
        // Ensure the DSM work item has completed before
        // the DSM goes away
        //
        if (deviceContext->DsmContext.SmWorker) {
            HUBMISC_FlushWorkerItem(deviceContext->HubFdoContext,
                                    deviceContext->DsmContext.SmWorker);
            HUBMISC_DeleteWorkerItem(deviceContext->HubFdoContext,
                                     deviceContext->DsmContext.SmWorker);
            deviceContext->DsmContext.SmWorker = NULL;
        }

        HUBMISC_FreeBufferIfNotNull(deviceContext->SerialNumberId.Buffer, USBHUB3_DEVICE_TAG);
        RtlFreeUnicodeString(&deviceContext->UxdId);

        HUBMISC_FreeBufferIfNotNull(deviceContext->AlternateSettingFilter, USBHUB3_DEVICE_TAG);

        HUBMISC_FreeBufferIfNotNull(deviceContext->FriendlyNameIdString.Buffer, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->ConfigurationDescriptor, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->MsExtConfigDesc, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->LanguageIdDescriptor, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->ProductIdStringDescriptor, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->BOSDescriptor, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->SublinkSpeedAttr, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeBufferIfNotNull(deviceContext->MsOs20Info.DescriptorSet, USBHUB3_DEVICE_TAG);
        HUBMISC_FreeUcxEndpointArrays(deviceContext);

        //
        // All the WDF objects that DSM uses are child of the port, so they need
        // to be deleted explicitly. However, there is a race. If PSM happens to
        // get deleted, then these objects might have been already deleted. DSM
        // holds a reference on them so that that they don't go away under it.
        // Therefore after deleteting the objects, it also needs to release that
        // reference.
        //
        HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->ControlRequest.WdfRequest);
        HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->UcxRequest);
        if (deviceContext->DsmContext.Timer) {
            ExDeleteTimer(deviceContext->DsmContext.Timer, TRUE, FALSE, NULL);
            deviceContext->DsmContext.Timer = NULL;
        }
        HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->OutOfBandwidthTimer);
        HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(&deviceContext->UcxIoctlRequestMemory);

        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext->PortContext),
                                    (PVOID) DsmCreateTag);

    } FINALLY {
    }

    return;

}


VOID
HUBMISC_EvtPsmDestroy (
    WDFOBJECT       PortObject
    )
/*++

Routine Description:

    Event callback to perform operations that are needed when the
    Port is being destroyed.

Return Value:
    VOID

--*/
{
    PPORT_CONTEXT       portContext;

    TRY {

        portContext = GetPortContext(PortObject);

        if (portContext->PsmContext.Timer) {
            ExDeleteTimer(portContext->PsmContext.Timer, TRUE, FALSE, NULL);
            portContext->PsmContext.Timer = NULL;
        }

        WppRecorderLogDelete(portContext->IfrLog);

        //
        // Release the reference on HSM that PSM acquired
        // when it was created
        //
        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(portContext->HubFdoContext),
                                    (PVOID) PsmTag);


    } FINALLY {
    }
}

VOID
HUBMISC_EvtPsmCleanup (
    WDFOBJECT       PortObject
    )
/*++

Routine Description:

    Event callback to perform operations that are needed when the
    Port is going away. We will detach the device if it is not
    already done.

Return Value:
    VOID

--*/
{
    PPORT_CONTEXT       portContext;

    TRY {

        portContext = GetPortContext(PortObject);

        //
        // If we were not able to create the worker
        // item for PSM, then we don't need to do
        // any state machine cleanup
        //
        if (portContext->PsmContext.SmWorker == NULL) {
            LEAVE;
        }
        //
        // If the device is still there, then get rid of it.
        // this can happen when our hub or upstream  hub/controller
        // gets removed directly, as opposed to the device
        //
        portContext->PsmContext.AddEvent(portContext,
                                         PsmEventObjectCleanup);

        HUBMISC_FlushWorkerItem(portContext->HubFdoContext,
                                portContext->PsmContext.SmWorker);

        HUBMISC_DeleteWorkerItem(portContext->HubFdoContext,
                                 portContext->PsmContext.SmWorker);

        portContext->PsmContext.SmWorker = NULL;


    } FINALLY {

    }

    return;

}


VOID
HUBMISC_StartOverCurrentTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port over current WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        DbgTraceTo(PortContext->IfrLog,
                   TL_ERROR,
                   Port,
                   "%p Starting OverCurrent timer with %d ms timeout",
                   PortContext, PORT_OVERCURRENT_TIMEOUT);

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PORT_OVERCURRENT_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}

VOID
HUBMISC_StartPostAddressTimer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
/*++

Routine Description:

    Starts the post address WDF timer.

    The expiration routine for the timer should queue
    DsmEventTimerFired to the DSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_POST_ADDRESS_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_StartPortPowerOnTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port power on WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        //
        // For non root hubs, we have to maintain compatability
        // with win7 stack and wait for longer period. For root
        // hubs, we can use a different shorter period
        //
        if (PortContext->HubFdoContext->HubParentInterface.HubDepth == 0) {
            ExSetTimer(
                PortContext->PsmContext.Timer,
                WDF_REL_TIMEOUT_IN_MS(PORT_POWER_ON_TIMEOUT_FOR_ROOT_PORTS),
                0,
                NULL);

        } else {
            ExSetTimer(PortContext->PsmContext.Timer,
                       WDF_REL_TIMEOUT_IN_MS(PORT_POWER_ON_TIMEOUT),
                       0,
                       NULL);
        }

    } FINALLY {

    }
}

VOID
HUBMISC_StartResumeRecoveryTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the timer for port to wait after resume before
    allowing the DSM to start interacting with the device

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    ULONGLONG   resumeTime;

    TRY {
        if ((PortContext->DeviceContext != NULL)  &&
            (PortContext->DeviceContext->MsOs20Info.Flags.MinResumeTime == 1)) {

            resumeTime =
                PortContext->DeviceContext->MsOs20Info.MinResumeTimeDescriptor->bResumeRecoveryTime;
        } else {
            resumeTime = PORT_RESUME_RECOVERY_TIMEOUT;
        }

        ExSetTimer(PortContext->PsmContext.Timer,
                          WDF_REL_TIMEOUT_IN_MS(resumeTime),
                          0,
                          NULL);

    } FINALLY {

    }
}


VOID
HUBMISC_StartReconnectOnResumeTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port re-connect WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        //
        // We give longer for boot devices as system won't make any
        // progress anyways without them.
        //
        if (PortContext->PortFlags.ConnectedDeviceSupportsReAttach) {
            ExSetTimer(
                PortContext->PsmContext.Timer,
                WDF_REL_TIMEOUT_IN_MS(PORT_RECONNECT_TIMEOUT_FOR_BOOT_DEVICE),
                0,
                NULL);

        } else {
            ExSetTimer(PortContext->PsmContext.Timer,
                       WDF_REL_TIMEOUT_IN_MS(PORT_RECONNECT_TIMEOUT),
                       0,
                       NULL);
        }


    } FINALLY {

    }
}

VOID
HUBMISC_StartD3ColdReconnectTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the timer for port re-connect resuming from D3 cold.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "%p Starting D3ColdReconnectTimer with %d ms timeout",
                   PortContext, PortContext->D3ColdReconnectTimeout);

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PortContext->D3ColdReconnectTimeout),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_StartDebounceTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port debounce WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PORT_DEBOUNCE_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_StartResetCompletionTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port reset WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PORT_RESET_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}


VOID
HUBMISC_StartResumeCompletionTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the port resume WDF timer.

    The expiration routine for the timer should queue
    PsmEventTimerFired to the PSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(PortContext->PsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(PORT_RESUME_TIMEOUT),
                   0,
                   NULL);


    } FINALLY {

    }
}

VOID
HUBMISC_StartTimerForResetCompletionPolling (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Starts the timer for polling of reset completion. This
    is only used for working around bugs in the hub
    hardware

    The expiration routine for the timer should queue
    PsmEventPortResetPollingTimerFired to the PSM

Return Value:

    VOID

--*/
{
    BOOLEAN inTimerQueue;

    TRY {

        inTimerQueue = WdfTimerStart(PortContext->ResetPollingTimer,
                                     WDF_REL_TIMEOUT_IN_MS(PORT_RESET_COMPLETE_POLLING_INTERVAL));

        if (inTimerQueue == TRUE) {
            NT_ASSERTMSG("StartTimerForResetCompletionPolling - PSM Timer Already in Queue", FALSE);
        }

    } FINALLY {

    }
}


PSM_EVENT
HUBMISC_StopPortTimer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Stops the current WDF timer.

Return Value:

    PsmEventOperationSuccess
    PsmEventOperationFailure

--*/
{
    PSM_EVENT       portEvent;
    BOOLEAN         wasInQueue;

    TRY {
        wasInQueue = ExCancelTimer(PortContext->PsmContext.Timer, NULL);

        if (wasInQueue == TRUE ) {
            portEvent = PsmEventOperationSuccess;
        } else {
            portEvent = PsmEventOperationFailure;
        }

    } FINALLY {

    }

    return portEvent;
}


HSM_EVENT
HUBMISC_IncrementAndCheckIfResetCountExceededMaximum (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Increments the number of times we have tried to reset the hub for error
    recovery. Compares the count to a maximum limit.

Return Value:

    HsmEventYes
    HsmEventNo

--*/
{
    HSM_EVENT       hsmEvent;

    TRY {

        hsmEvent = (HubFdoContext->HubInformation.HubResetCount++ > MAXIMUM_HUB_RESET_COUNT) ?
                   HsmEventYes :
                   HsmEventNo;

        if (hsmEvent == HsmEventYes) {
            HubFdoContext->HubFailureMsgId = HUB_FAILURE_HUB_RESET_FAILED;
        }

    } FINALLY {

    }

    return hsmEvent;
}

HSM_EVENT
HUBMISC_IncrementAndCheckIfInterruptPipeResetCountExceededMaximum (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Increments the number of times we have tried to reset the interrupt
    pipe since the last time we got a successful interrupt transfer

Return Value:

    HsmEventYes
    HsmEventNo

--*/
{
    HSM_EVENT       hsmEvent;

    TRY {

        hsmEvent = (++HubFdoContext->HubInformation.HubResetCount > MAXIMUM_INTERRUPT_PIPE_RESET_COUNT) ?
                   HsmEventYes :
                   HsmEventNo;

    } FINALLY {

    }

    return hsmEvent;
}

HSM_EVENT
HUBMISC_CheckIfHubDepthIsZero (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Checks is hub is the root hub

Return Value:

    HsmEventYes
    HsmEventNo

--*/
{
    HSM_EVENT       hsmEvent;

    TRY {

        hsmEvent = (HubFdoContext->HubParentInterface.HubDepth == 0) ? HsmEventYes : HsmEventNo;

    } FINALLY {

    }

    return hsmEvent;
}


HSM_EVENT
HUBMISC_IncrementAndCheckIfHubDescriptorRetryCountExceededMax (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Increments the number of times we have tried to get the hub descriptor
    Compares the count to a maximum limit.

Return Value:

    HsmEventYes
    HsmEventNo

--*/
{
    HSM_EVENT       hsmEvent;

    TRY {

        hsmEvent = (HubFdoContext->HubInformation.HubGetDescriptorCount++ >
                    MAXIMUM_HUB_RESET_COUNT) ?
                   HsmEventYes :
                   HsmEventNo;

    } FINALLY {

    }

    return hsmEvent;
}


HSM_EVENT
HUBMISC_IncrementAndCheckIfRecoveryCountExceededLimit (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    This routine determines if the hub should be allowed to do a reset
    recovery or not. This is determined by how many times the hub has
    asked for a restart within a given period.

Return Value:

    HsmEventYes
    HsmEventNo

--*/
{
    LARGE_INTEGER       currentTime;
    LARGE_INTEGER       delta;
    HSM_EVENT           hsmEvent;


    TRY {

        //
        // Start with assuming that we haven't exceeded the limit
        //
        hsmEvent = HsmEventNo;

        KeQuerySystemTime(&currentTime);

        NT_ASSERT(HubFdoContext->HubInformation.FirstHubResetRecoveryTimeStamp.QuadPart <= currentTime.QuadPart);

        delta.QuadPart = currentTime.QuadPart - HubFdoContext->HubInformation.FirstHubResetRecoveryTimeStamp.QuadPart;

        if (delta.QuadPart > WDF_ABS_TIMEOUT_IN_SEC(MAXIMUM_HUB_RECOVERIES_TIME_PERIOD_IN_SEC)) {
            //
            // Exceeded the time limit.  This is treated as a reset of
            // the time limit, so we will try to restart and reset the
            // start time and restart count.
            //
            HubFdoContext->HubInformation.HubTimedResetRecoveryCount = 1;
            HubFdoContext->HubInformation.FirstHubResetRecoveryTimeStamp.QuadPart = currentTime.QuadPart;
            LEAVE;
        }

        HubFdoContext->HubInformation.HubTimedResetRecoveryCount++;

        if (HubFdoContext->HubInformation.HubTimedResetRecoveryCount < MAXIMUM_HUB_RECOVERIES_COUNT) {
            //
            // Hub is under the allowed limit of resets
            //
            LEAVE;
        }
        //
        // Exceeded the reset limit. We are going to declare a Pnp failure in
        // the state machine.If the debug registry key is set, break in the
        // debugger to allow someone to debug before the logs are lost
        //
        hsmEvent = HsmEventYes;

        HubFdoContext->HubFailureMsgId = HUB_FAILURE_HUB_RESET_FAILED;

        USB_HW_VERIFIER_HUB_BREAK(HubFdoContext, HubHwVerifierTooManyResets);

        EventWriteUSBHUB3_ETW_EVENT_HUB_TOO_MANY_RESETS(
            &HubFdoContext->CurrentActivityId,
            HubFdoContext->HubParentInterface.Hub);

    } FINALLY {

    }

    return hsmEvent;
}


VOID
HUBMISC_InitializeHubDescriptorRetryCount (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Initializes Hub Descriptor Retry count to zero

Return Value:

    VOID

--*/
{

    TRY {

        HubFdoContext->HubInformation.HubGetDescriptorCount = 0;

    } FINALLY {

    }
}


VOID
HUBMISC_InitializeHubResetCount (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Initializes Hub Reset Retry count to zero

Return Value:

    VOID

--*/
{

    TRY {

        HubFdoContext->HubInformation.HubResetCount = 0;

    } FINALLY {

    }
}

VOID
HUBMISC_ResetCountForResetInterruptPipe (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Resets the count that keeps track of the number of
    times the interrupt pipe has been reset for recovery

Return Value:

    VOID

--*/
{

    TRY {

        HubFdoContext->HubInformation.HubResetCount = 0;

    } FINALLY {

    }
}


VOID
HUBMISC_StartHubOverCurrentTimer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Starts the OverCurrent WDF timer.

    The expiration routine for the timer should queue
    HsmEventTimerFired to the HSM

Return Value:

    VOID

--*/
{
    TRY {

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_ERROR,
                   Hub,
                   "%p HUBMISC_StartHubOverCurrentTimer calling ExSetTimer(HsmContext.Timer), timer = %d ms (HUB_OVERCURRENT_TIMEOUT)",
                   HubFdoContext, HUB_OVERCURRENT_TIMEOUT);

        ExSetTimer(HubFdoContext->HsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(HUB_OVERCURRENT_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}


VOID
HUBMISC_StartHubDescriptorRetryTimer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Starts the WDF timer for retrying fetching Hub descriptor

    The expiration routine for the timer should queue
    HsmEventTimerFired to the HSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(HubFdoContext->HsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(HUB_DESCRIPTOR_RETRY_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}


VOID
HUBMISC_StartResetRetryTimer (
    __in PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Starts the reset retry WDF timer.

    The expiration routine for the timer should queue
    HsmEventTimerFired to the HSM

Return Value:

    VOID

--*/
{
    TRY {

        ExSetTimer(HubFdoContext->HsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(HUB_RESET_RETRY_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}

HSM_EVENT
HUBMISC_StopHubTimer (
    __in PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Stops the current WDF timer.

Return Value:

    HsmEventOperationSuccess
    HsmEventOperationFailure

--*/
{
    HSM_EVENT       HsmEvent;
    BOOLEAN         wasInQueue;

    TRY {

        wasInQueue = ExCancelTimer(HubFdoContext->HsmContext.Timer, NULL);

        if (wasInQueue == TRUE) {
            HsmEvent = HsmEventOperationSuccess;
        } else {
            HsmEvent = HsmEventOperationFailure;
        }

    } FINALLY {

    }

    return HsmEvent;
}


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
HUBMISC_ControlTransfer(
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext,
    __in_opt
        UCXUSBDEVICE                        UsbDevice,
    __in
        PVOID                               Context,
    __in
        PCONTROL_REQUEST                    ControlRequest,
    __in
        PFN_WDF_REQUEST_COMPLETION_ROUTINE  CompletionRoutine,
    __in_bcount_opt(TransferBufferLength)
        PVOID                               TransferBuffer,
    __in
        size_t                              TransferBufferLength,
    __in
        BOOLEAN                             ShortXferOk,
    __in
        BOOLEAN                             EnsureForwardProgress
    )
/*++

Routine Description:

    Asynchronously sends a control transfer.

Arguments:

    HubFdoContext - If UsbDevice is NULL, control transfer target is default endpoint of this hub.

    UsbDevice - If non-NULL, control transfer target is default endpoint of this device.

    Context - Context to be passed to completion routine.

    ControlRequest - Preallocated request to use for the control transfer.

    CompletionRoutine -

    TransferBuffer -

    TransferBufferLength -

    ShortXferOk - Indicates if short transfers are legal for the control transfer.

    NOTE: The setup packet in the pre-allocated URB should already be formatted
          by the caller.

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    IO_STACK_LOCATION               ioStackLocation;
    BOOLEAN                         requestSent;
    struct _URB_CONTROL_TRANSFER_EX *urb;

    TRY {

        status = STATUS_SUCCESS;

        //
        // Extract the formatted setup packet from our pre-allocated URB
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)ControlRequest->Urb.SetupPacket;

        urb = &ControlRequest->Urb;
        urb->Hdr.UsbdDeviceHandle = UsbDevice;

        //
        // Set up the control transfer
        //
        urb->Hdr.Length = sizeof(struct _URB_CONTROL_TRANSFER_EX);
        urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

        urb->TransferFlags = USBD_DEFAULT_PIPE_TRANSFER;

        if (ShortXferOk != FALSE) {
            urb->TransferFlags |= USBD_SHORT_TRANSFER_OK;
        }

        if (setupPacket->bmRequestType.Dir ==  BMREQUEST_DEVICE_TO_HOST) {
            urb->TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
        }

        if (EnsureForwardProgress != FALSE) {
            urb->TransferFlags |= USBD_FORWARD_PROGRESS_TRANSFER;
        }

        urb->TransferBufferLength = (ULONG) TransferBufferLength;
        urb->TransferBuffer = TransferBuffer;
        urb->TransferBufferMDL = NULL;

        urb->Timeout = CONTROL_TRANSFER_TIMEOUT;

        urb->Hdr.UsbdFlags = ControlRequest->UsbdFlags;

        RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        ioStackLocation.Parameters.Others.Argument1 = urb;

        WdfRequestWdmFormatUsingStackLocation(ControlRequest->WdfRequest, &ioStackLocation);

        WdfRequestSetCompletionRoutine(ControlRequest->WdfRequest,
                                       CompletionRoutine,
                                       Context);

        requestSent = WdfRequestSend(ControlRequest->WdfRequest,
                                     HubFdoContext->RootHubIoTarget,
                                     NULL);

        if (requestSent == FALSE) {

            DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "URB_FUNCTION_CONTROL_TRANSFER_EX Request Not Sent!");

            status = WdfRequestGetStatus(ControlRequest->WdfRequest);
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            HUBMISC_RequestReuse(ControlRequest->WdfRequest);
        }
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_AcquireWdfReferenceOnPort(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls

Return Value:

    VOID

--*/
{

    TRY {

        WdfObjectReferenceWithTag(WdfObjectContextGetObject(DeviceContext->PortContext),
                                  (PVOID) DsmTag);

    } FINALLY {

    }

    return;

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_ReleaseWdfReferenceOnPort(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls

Return Value:

    VOID

--*/
{

    TRY {

        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(DeviceContext->PortContext),
                                    (PVOID) DsmTag);

    } FINALLY {

    }

    return;

}


FORCEINLINE
VOID
HUBMISC_RequestReuse(
    __in
        WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    Requests reuse of a pre-allocated WDF request

Return Value:


--*/
{
    NTSTATUS                    status;
    WDF_REQUEST_REUSE_PARAMS    params;

    TRY {

        WDF_REQUEST_REUSE_PARAMS_INIT(&params,
                                      WDF_REQUEST_REUSE_NO_FLAGS,
                                      STATUS_SUCCESS);

        status = WdfRequestReuse(WdfRequest, &params);

        NT_ASSERT(NT_SUCCESS(status));

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Error returned by WdfRequestReuse %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_InitializeBitmap (
    __out
        PRTL_BITMAP         BitMapHeader,
    __in __drv_aliasesMem
        PULONG              BitMapBuffer,
    __in
        ULONG               SizeOfBitMap
    )
/*++

Routine Description:

    Since RtlInitializeBitmap is paged code, we can't use it, so this is
    a copy of the above function.

    This procedure initializes a bit map.

Arguments:

    BitMapHeader - Supplies a pointer to the BitMap Header to initialize

    BitMapBuffer - Supplies a pointer to the buffer that is to serve as the
        BitMap.  This must be an a multiple number of longwords in size.

    SizeOfBitMap - Supplies the number of bits required in the Bit Map.

Return Value:

    None.

--*/

{
    //
    //  Initialize the BitMap header.
    //

    BitMapHeader->SizeOfBitMap = SizeOfBitMap;
    BitMapHeader->Buffer = BitMapBuffer;

    //
    //  And return to our caller
    //

    return;
}

VOID
HUBMISC_FreeBufferForMSOSExtendedPropertyDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Frees the buffer used to read in the entire MS OS Extended
    Property Descriptor.

Returns:

    VOID

--*/
{

    NT_ASSERT(DeviceContext->MsExtPropertyDesc != NULL);

    ExFreePoolWithTag(DeviceContext->MsExtPropertyDesc, USBHUB3_DEVICE_TAG);

    DeviceContext->MsExtPropertyDesc = NULL;
}

VOID
HUBMISC_MarkDeviceAsDeconfigured (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:
    Assume the device has been deconfigured without actually
    sending a control transfer to the device.

Returns:

    VOID

--*/
{

    DeviceContext->OldDeviceConfiguration = DeviceContext->CurrentDeviceConfiguration;
    DeviceContext->CurrentDeviceConfiguration = NULL;
}



DSM_EVENT
HUBMISC_CheckIfRequestedInterfaceNumberIsOpen (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the interface number being selected via
    SET_INTERFACE already has an existing interface in the
    current configuration.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    if (DeviceContext->OldInterface != NULL) {
        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_AllocateBufferForMSOSExtendedPropertyDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Allocates a buffer for the entire MS OS Extended Property
    Descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS                    status;
    PMS_EXT_PROP_DESC_HEADER    msExtPropertyDescHeader;

    TRY {

        status = STATUS_SUCCESS;

        //
        // The scratch buffer should still contain the MS OS Extended Property Descriptor Header
        //
        msExtPropertyDescHeader = (PMS_EXT_PROP_DESC_HEADER) DeviceContext->ScratchBuffer;

        //
        // Allocate the full MS OS extended property descriptor
        //
        NT_ASSERT(DeviceContext->MsExtPropertyDesc == NULL);
        DeviceContext->MsExtPropertyDesc =
            ExAllocatePoolWithTag(NonPagedPool,
                                  msExtPropertyDescHeader->dwLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->MsExtPropertyDesc == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate memory for MS OS Extended Property Descriptor");

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}

DSM_EVENT
HUBMISC_CheckIfMSOSExtendedPropertyDescriptorShouldBeQueried (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if a query should be sent to the device for MS OS Extended Property
    Descriptor.  This should only happen the first the the device is started, if it
    supports MS OS descriptors.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        HUBREG_QueryExtPropDescSemaphoreInDeviceHardwareKey(DeviceContext);

        if (DeviceContext->DeviceFlags.DeviceIsComposite == 1) {

            dsmEvent = DsmEventNo;

        } else if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {

            dsmEvent = DsmEventNo;

        } else if ((DeviceContext->DeviceHackFlags.DontSkipMsOsDescriptor == 1) &&
                   (DeviceContext->DeviceFlags.ExtPropertyInstalled == 0)) {

            dsmEvent = DsmEventYes;

        } else if (((DeviceContext->DeviceDescriptor.bcdUSB < 0x200) &&
                    (DeviceContext->DeviceDescriptor.bcdUSB >= 0x100)) ||
                   (DeviceContext->DeviceFlags.ExtPropertyInstalled == 1) ||
                   (DeviceContext->DeviceFlags.MsOsDescriptorNotSupported == 1)) {

            dsmEvent = DsmEventNo;

        } else {

            dsmEvent = DsmEventYes;

        }

    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_SetExtPropDescSemaphoreInRegistry (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Writes the "ExtPropDescSemaphore" registry flag to the device's hardware
    key to indicate that the device does not need to be queried for MS OS
    Extended Property Descriptors in the future.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS    status;
    ULONG       value;

    PAGED_CODE();

    TRY {

        value = 1;

        status = HUBREG_WriteValueToDeviceHardwareKey(DeviceContext,
                                                      &g_ExtPropDescSemaphore,
                                                      REG_DWORD,
                                                      sizeof(ULONG),
                                                      &value);
    } FINALLY {

    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


VOID
HUBMISC_SetSqmEnumerationData (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Writes various SQM data points about device enumeration to the device's hardware.


--*/
{

    PAGED_CODE();

    TRY {

        if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {
            HUBREG_WriteValueToDeviceHardwareKey(DeviceContext,
                                                  &g_MsOs20Flags,
                                                  REG_DWORD,
                                                  sizeof(ULONG),
                                                  &DeviceContext->MsOs20Info.Flags);
        }

        HUBREG_WriteValueToDeviceHardwareKey(DeviceContext,
                                             &g_EnumerationRetryCount,
                                             REG_DWORD,
                                             sizeof(ULONG),
                                             &DeviceContext->EnumRetryCount);

    } FINALLY {

    }

}


DSM_EVENT
HUBMISC_ParseAndWriteCustomPropertiesInRegistry (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Parses each custom property in the MS OS Extended Property Descriptor returned by
    the device, and writes them to the device's hardware registry key.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS            status;
    PUCHAR              offset;
    PMS_EXT_PROP_DESC   msExtPropDesc;
    ULONG               i;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        msExtPropDesc = DeviceContext->MsExtPropertyDesc;
        NT_ASSERT(msExtPropDesc != 0);

        offset = (PUCHAR)msExtPropDesc->CustomSection;

        for(i=0; i < msExtPropDesc->Header.wCount; i++){

            PMS_EXT_PROP_DESC_CUSTOM_PROP       customProp;
            PMS_EXT_PROP_DESC_CUSTOM_PROP_DATA  customData;
            UNICODE_STRING                      unicodeNameString;

            customProp = (PMS_EXT_PROP_DESC_CUSTOM_PROP)offset;

            customData = (PMS_EXT_PROP_DESC_CUSTOM_PROP_DATA)(offset + customProp->wPropertyNameLength + 10);

            //
            // Initialize the string
            //
            RtlInitUnicodeString(&unicodeNameString, customProp->PropertyName);

            status = HUBREG_WriteValueToDeviceHardwareKey(DeviceContext,
                                                          &unicodeNameString,
                                                          customProp->dwPropertyDataType,
                                                          customData->dwPropertyDataLength,
                                                          customData->PropertyData);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failed to write custom property to registry %!STATUS!",
                           status);
                LEAVE;
            }

            offset += customProp->dwSize;
        }
    } FINALLY {

    }

    return(NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}

DSM_EVENT
HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptorHeader (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Validates that the MS OS Extended Property Descriptor Header returned
    by the device is well-formed.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PMS_EXT_PROP_DESC_HEADER        msExtPropDescHeader;
    DSM_EVENT                       dsmEvent;

    TRY {

        dsmEvent = DsmEventOperationFailure;

        msExtPropDescHeader = (PMS_EXT_PROP_DESC_HEADER)DeviceContext->ScratchBuffer;

        if (DeviceContext->ControlRequest.NumberOfBytes != sizeof(MS_EXT_PROP_DESC_HEADER)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Invalid number of bytes returned for MSOS Extended Property Descriptor Header");
            LEAVE;
        }

        if (msExtPropDescHeader->dwLength < sizeof(MS_EXT_PROP_DESC_HEADER)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Invalid dwLength value for MSOS Extended Property Descriptor Header");
            LEAVE;
        }

        if (msExtPropDescHeader->bcdVersion != MS_EXT_PROP_DESC_VER) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Invalid bcdVersion value for MSOS Extended Property Descriptor Header");
            LEAVE;
        }

        if (msExtPropDescHeader->wIndex != MS_EXT_PROP_DESCRIPTOR_INDEX) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: wIndex not equal to 5 for MSOS Extended Property Descriptor Header");
            LEAVE;
        }

        if (msExtPropDescHeader->wCount == 0) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: wCount is 0 for MSOS Extended Property Descriptor Header");
            LEAVE;
        }

        dsmEvent = DsmEventOperationSuccess;

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_EXTENDED_PROPERTY_DESCRIPTOR_HEADER(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }

    }

    return dsmEvent;
}



DSM_EVENT
HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Validates that the MS OS Extended Property Descriptor returned
    by the device is well-formed.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PMS_EXT_PROP_DESC_HEADER        msExtPropDescHeader;
    PMS_EXT_PROP_DESC               msExtPropDesc;
    PUCHAR                          offset;
    PUCHAR                          end;
    ULONG                           propertyCount;
    DSM_EVENT                       dsmEvent;

    TRY {

        dsmEvent = DsmEventOperationFailure;
        propertyCount = 0;

        //
        // Scratch buffer should stilll contain the MSOS Extended Property Descriptor Header
        //

        msExtPropDescHeader = (PMS_EXT_PROP_DESC_HEADER)DeviceContext->ScratchBuffer;
        msExtPropDesc = DeviceContext->MsExtPropertyDesc;

        if (DeviceContext->ControlRequest.NumberOfBytes != msExtPropDescHeader->dwLength) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for MSOS Extended Property Descriptor not equal to dwLength field");
            LEAVE;
        }

        if (RtlCompareMemory(msExtPropDescHeader,
                             msExtPropDesc,
                             sizeof(MS_EXT_PROP_DESC_HEADER)) !=
            sizeof(MS_EXT_PROP_DESC_HEADER)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MSOS Extended Property Descriptor Header has changed!");
            LEAVE;
        }

        if (msExtPropDescHeader->dwLength > MAXIMUM_EXTENDED_PROPERTY_DESCRIPTOR_SIZE) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:MSOS Extended Property Descriptor length greater than maximum of 4K");
            LEAVE;
        }

        offset = (PUCHAR)msExtPropDesc->CustomSection;
        end = ((PUCHAR)msExtPropDesc) + msExtPropDesc->Header.dwLength;

        while(offset < end) {
            ULONG                               propertyNameLength;
            ULONG                               propertySize;
            ULONG                               propertyDataLength;
            PMS_EXT_PROP_DESC_CUSTOM_PROP       customProperty;
            PMS_EXT_PROP_DESC_CUSTOM_PROP_DATA  customPropertyData;

            //
            // Make sure the required fields are in bounds
            //
            if(offset + MINIMUM_CUSTOM_PROPERTY_SIZE > end) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property extends past end of MSOS Extended Property Descriptor");
                LEAVE;
            }

            customProperty = (PMS_EXT_PROP_DESC_CUSTOM_PROP)offset;
            propertySize = customProperty->dwSize;

            //
            // We must have at least a minimum number of bytes for the
            // fixed length fields.
            //
            if ((offset + propertySize) > end) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property extends past end of MSOS Extended Property Descriptor");
                LEAVE;
            }

            if (propertySize < 14) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property size is less than minimum");
                LEAVE;
            }

            //
            // Verify the data type is between 1-7
            //
            if((customProperty->dwPropertyDataType < REG_SZ) ||
               (customProperty->dwPropertyDataType > REG_MULTI_SZ)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property dwPropertyDataType is invalid");
                LEAVE;
            }

            //
            // Verify the property name length before we check any other fields
            //
            propertyNameLength = customProperty->wPropertyNameLength;

            if (((propertyNameLength + MINIMUM_CUSTOM_PROPERTY_SIZE) > propertySize) ||
                (propertyNameLength == 0) ||
                ((propertyNameLength % sizeof(WCHAR)) != 0)) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property wPropertyNameLength is invalid");
                LEAVE;
            }

            //
            // Check for NULL first byte of name and to see if the string is null-terminated
            //
            if ((customProperty->PropertyName[0] == 0) ||
                (customProperty->PropertyName[propertyNameLength/sizeof(WCHAR) - 1] !=
                 UNICODE_NULL)) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property PropertyName is NULL, or not NULL-terminated");
                LEAVE;
            }

            customPropertyData =
                (PMS_EXT_PROP_DESC_CUSTOM_PROP_DATA)(offset+10+propertyNameLength);

            //
            // Make sure the custom property size accounts for the data size.
            // We have already verified that data length field is in bounds
            //
            propertyDataLength = customPropertyData->dwPropertyDataLength;

            if ((propertyNameLength + propertyDataLength + MINIMUM_CUSTOM_PROPERTY_SIZE) >
                propertySize) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Custom property PropertyName is NULL, or not NULL-terminated");
                LEAVE;
            }

            propertyCount++;

            //
            // Allowed to have junk at end, just leave
            //
            if (propertyCount == msExtPropDesc->Header.wCount) {
                break;
            }

            offset += propertySize;
        }

        if (propertyCount != msExtPropDesc->Header.wCount) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of custom properties found does not match wCount");
            LEAVE;
        }

        dsmEvent = DsmEventOperationSuccess;

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_EXTENDED_PROPERTY_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}



BOOLEAN
HUBMISC_ParseAndInstallMsOs20RegistryValueDescriptors(
    __in
        PDEVICE_CONTEXT             DeviceContext,
    __in
        PMSOS20_INFO                MsOs20Info,
    __in
        PMSOS20_COMMON_DESCRIPTOR   MsOs20DescriptorSet,
    __in
        PUCHAR                      EndOfSet
    )
{
    BOOLEAN                     result;
    PMSOS20_COMMON_DESCRIPTOR   msOs20CommonDescriptor;
    NTSTATUS                    status;

    UNREFERENCED_PARAMETER(MsOs20Info);

    TRY {

        msOs20CommonDescriptor = MsOs20DescriptorSet;

        do {

            result = HUBDESC_GetNextMsOs20Descriptor(EndOfSet,
                                                     &msOs20CommonDescriptor);

            if (result == FALSE) {
                LEAVE;
            }

            if (msOs20CommonDescriptor != NULL) {

                if (msOs20CommonDescriptor->wDescriptorType == MSOS20_FEATURE_REGISTRY_PROPERTY) {

                    PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR       msOs20RegVal;
                    UNICODE_STRING                                  unicodeNameString;
                    PMSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA    propertyData;

                    msOs20RegVal = (PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR) msOs20CommonDescriptor;

                    propertyData = (PMSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA)
                                     ((PUCHAR)(msOs20RegVal->PropertyName) +
                                       msOs20RegVal->wPropertyNameLength);

                    RtlInitUnicodeString(&unicodeNameString, msOs20RegVal->PropertyName);

                    status = HUBREG_WriteValueToDeviceHardwareKey(DeviceContext,
                                                                  &unicodeNameString,
                                                                  msOs20RegVal->wPropertyDataType,
                                                                  propertyData->wPropertyDataLength,
                                                                  propertyData->PropertyData);

                    if (!NT_SUCCESS(status)) {
                        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "Write value to DeviceHardwareKey failed %!STATUS!",
                                   status);
                        LEAVE;
                    }
                }
            }
        } while (msOs20CommonDescriptor != NULL);
    } FINALLY {

    }

    return result;
}


DSM_EVENT
HUBMISC_IsMsOs20AltEnumCmdNeededInEnum(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
        
    if ((DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdCached == 1) && 
        (DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdSent == 0)) {

        INTERLOCKED_SET_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCompleted);

        RtlCopyMemory(&DeviceContext->MsOs20Info.OriginalDeviceDescriptor,
                        &DeviceContext->DeviceDescriptor,
                        sizeof(USB_DEVICE_DESCRIPTOR));

        DeviceContext->DeviceHackFlags.AsUlong32 = 0;
        DeviceContext->DeviceFlags.AsUlong32 = 0;
        DeviceContext->DeviceStateFlags.AsUlong32 = 0;

        return DsmEventYes;

    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_IsMsOs20AltEnumCmdNeededOnReenum(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    if ((DeviceContext->MsOs20Info.Flags.AlternateEnumeration == 1) &&
        (DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdSent == 0)) {

        INTERLOCKED_SET_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCompleted);

        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_IsMsOs20AltEnumCmdNeededAfterGettingBos(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    if ((DeviceContext->MsOs20Info.Flags.AlternateEnumeration == 1) &&
        (DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdSent == 0)) {

        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_IsMsOs20AltEnumNeeded(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    if ((DeviceContext->MsOs20Info.Flags.AlternateEnumeration == 1) &&
        (DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCompleted == 0)) {

        INTERLOCKED_SET_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCompleted);

        RtlCopyMemory(&DeviceContext->MsOs20Info.OriginalDeviceDescriptor,
                      &DeviceContext->DeviceDescriptor,
                      sizeof(USB_DEVICE_DESCRIPTOR));

        DeviceContext->DeviceHackFlags.AsUlong32 = 0;
        DeviceContext->DeviceFlags.AsUlong32 = 0;
        DeviceContext->DeviceStateFlags.AsUlong32 = 0;

        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}

DSM_EVENT
HUBMISC_CheckIfMsOs20RegistryValuesShouldBeInstalled(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{

    if (((DeviceContext->MsOs20Info.Flags.RegistryProperty == 1) ||
         (DeviceContext->MsOs20Info.Flags.ModelId == 1)) &&
        (DeviceContext->DeviceFlags.ExtPropertyInstalled == 0)) {
        
        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_CheckIfMsOs20DescriptorSupported(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventYes
    DsmEventNo

--*/
{

    if ((DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) && 
        (DeviceContext->MsOs20Info.DescriptorSetInfo.wLength != 0)) {
        return DsmEventYes;
    } else {
        return DsmEventNo;
    }
}


DSM_EVENT
HUBMISC_InstallMsOs20RegistryProperties(
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:


Arguments:

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS                    status;
    PMSOS20_COMMON_DESCRIPTOR   msOs20CommonDescriptor;
    PUCHAR                      endOfDescriptorSet;
    BOOLEAN                     result;
    DSM_EVENT                   dsmEvent;

    TRY {

        dsmEvent = DsmEventOperationSuccess;


        //
        // Walk the MS OS 2.0 Descriptor set and install registry properties
        //
        msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR)DeviceContext->MsOs20Info.DescriptorSet;
        endOfDescriptorSet = (PUCHAR)msOs20CommonDescriptor +
                             ((PMSOS20_DESCRIPTOR_SET_HEADER)msOs20CommonDescriptor)->wTotalLength;

        result = HUBMISC_ParseAndInstallMsOs20RegistryValueDescriptors(DeviceContext,
                                                                       &DeviceContext->MsOs20Info,
                                                                       msOs20CommonDescriptor,
                                                                       endOfDescriptorSet);

        if (result == FALSE) {
            dsmEvent = DsmEventOperationFailure;
        }

        //
        // Also set the Model ID if it exists
        //
        if (DeviceContext->MsOs20Info.Flags.ModelId == 1) {

            status = IoSetDevicePropertyData (WdfDeviceWdmGetPhysicalDevice(
                                                WdfObjectContextGetObject(DeviceContext->PdoContext)),
                                              &DEVPKEY_Device_ModelId,
                                              LOCALE_NEUTRAL,
                                              0,
                                              DEVPROP_TYPE_GUID,
                                              sizeof(GUID),
                                              &DeviceContext->MsOs20Info.ModelIdDescriptor->ModelId);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Error setting Model ID %!STATUS!",
                           status);
            }
        }
    } FINALLY {
    }

    return dsmEvent;
}



VOID
HUBMISC_WaitForSignal (
    __in
        PKEVENT Event,
    __in
        PCSTR ReasonForWaiting,
    __in
        WDFOBJECT Handle
    )
/*++

Routine Description:

    Waits for the given event indefinitely. Periodically prints
    a debugger message to indicate that the driver is stuck on
    the event. This code has been borrowed from WDF.

Returns:

--*/
{
    LARGE_INTEGER           timeOut;
    NTSTATUS                status;
    PHUB_GLOBALS            hubGlobals;
    ULONG                   waitCount;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    TRY {
        waitCount = 0;
        hubGlobals = GetHubGlobals(WdfGetDriver());
        timeOut.QuadPart = WDF_REL_TIMEOUT_IN_SEC(60);

        do {
            status = KeWaitForSingleObject(Event,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           &timeOut);

            if(status == STATUS_TIMEOUT) {
                waitCount++;

                DbgPrint("\nUSBHUB3 Watchdog: Thread 0x%p has waited %d minutes for %s to complete for WDF object 0x%p\n", 
                          PsGetCurrentThread(),
                          waitCount,
                          ReasonForWaiting, 
                          Handle);

                DbgTraceTo(hubGlobals->IfrLog,
                          TL_ERROR,
                          Driver,
                          "Watchdog: Thread 0x%p is waiting for %s to complete for WDF object 0x%p (%d)", 
                          PsGetCurrentThread(), 
                          ReasonForWaiting, 
                          Handle,
                          waitCount);
              
            } else {
                if(!(NT_SUCCESS(status))) {
                    DbgTraceTo(hubGlobals->IfrLog,
                        TL_ERROR,
                        Driver,
                        "HUBMISC_WaitForSignal exiting because KeWaitForSingleObject returned failure %!STATUS! (thread 0x%p, WDF object 0x%p, waiting for %s)", 
                        status,
                        PsGetCurrentThread(),
                        Handle,
                        ReasonForWaiting);
                    
                    NT_ASSERT(FALSE);
                }
                break;
            }

        } WHILE(TRUE);

    } FINALLY {

    }

}

VOID
HUBMISC_DbgBreak(
    __in
        PCSTR                       ReasonForBreak
    )
/*++

Routine Description:

    This function checks if there is a debugger connected
    and if yes, breaks in the debugger, even on the fre
    build

Returns:

--*/
{
   PHUB_GLOBALS         hubGlobals;

   TRY {

       hubGlobals = GetHubGlobals(WdfGetDriver());

       DbgTraceTo(hubGlobals->IfrLog,
                  TL_ERROR,
                  Driver,
                  "ERROR: HUBMISC_DbgBreak: %s. Look at the last few lines of usbhub3 log for more details",
                  ReasonForBreak);

       if (KdRefreshDebuggerNotPresent() == FALSE) {

           DbgPrint("Breaking due to %s. For more details run: !rcdrlogdump usbhub3\n", ReasonForBreak);

           DbgBreakPoint();
       }

   } FINALLY {

   }


}

VOID
HUBMISC_VerifierDbgBreak(
    __in
        PCSTR                       FlagName,
    __in
        PSM_CONTEXT                 SmContext
    )
/*++

Routine Description:

    This function checks if there is a debugger connected
    and if yes, breaks in the debugger, even on the fre
    build

Returns:

--*/
{

   TRY {

       DbgPrint("USB Hardware Verifier Break for %s\n", FlagName);

       switch (SmContext->StartIndex) {
       case StartIndexHsm:

           DbgTraceTo(((PHUB_FDO_CONTEXT)(SmContext->ParentContext))->IfrLog,
                      TL_ERROR,
                      Hub,
                      "USB Hardware Verifier Break for %s: Run !hub_info 0x%p",
                      FlagName,
                      SmContext->ParentContext);

           DbgPrint("Run !rcdrlogdump usbhub3 -a 0x%p, !hub_info 0x%p\n",
                    ((PHUB_FDO_CONTEXT)(SmContext->ParentContext))->IfrLog,
                    SmContext->ParentContext);
           break;

       case StartIndexPsm20:
       case StartIndexPsm30:

           DbgTraceTo(((PPORT_CONTEXT)(SmContext->ParentContext))->IfrLog,
                      TL_ERROR,
                      Port,
                      "USB Hardware Verifier Break for %s: Run !port_info 0x%p",
                      FlagName,
                      SmContext->ParentContext);

           DbgPrint("Run !rcdrlogdump usbhub3 -a 0x%p, !port_info 0x%p\n",
                    ((PPORT_CONTEXT)(SmContext->ParentContext))->IfrLog,
                    SmContext->ParentContext);
           break;

       case StartIndexDsm:

           DbgTraceTo(((PDEVICE_CONTEXT)(SmContext->ParentContext))->PortContext->IfrLog,
                      TL_ERROR,
                      Port,
                      "USB Hardware Verifier Break for %s: Run !device_info 0x%p",
                      FlagName,
                      SmContext->ParentContext);

           DbgPrint("Run !rcdrlogdump usbhub3 -a 0x%p, !device_info 0x%p\n",
                    ((PDEVICE_CONTEXT)(SmContext->ParentContext))->PortContext->IfrLog,
                    SmContext->ParentContext);
           break;

       default:
           NT_ASSERTMSG("Unidentified Start Index",
                        FALSE);
           break;

       }

       if (KdRefreshDebuggerNotPresent() == FALSE) {
#if DBG
           NT_ASSERTMSG("USB Hardware Verifier Break", FALSE);
#else
           DbgBreakPoint();
#endif
       }

   } FINALLY {

   }


}

VOID
HUBMISC_QueryKseDeviceFlags (
    __in
        PCHAR                   VendorId,
    __in
        PCHAR                   DeviceId,
    __in
        PCHAR                   RevisionId,
    __out
        PKSE_USB_DEVICE_FLAGS   DeviceFlagsVidPidRev,
    __out
        PKSE_USB_DEVICE_FLAGS   DeviceFlagsVidPid,
    __in
        BOOLEAN                 IsRootHub,
    __in
        RECORDER_LOG            IfrLog
    )
/*++

Routine Description:

    Queries the KSE component for errata flags to be applied to devices with matching
    VID/PID/Rev or VID/PID.

Returns:

    VOID

--*/
{
    WCHAR       usbDeviceKey[50];
    NTSTATUS    status;

    C_ASSERT((VID_PID_REV_BUFFER_SIZE +
              sizeof(L"USB:ROOT_HUB\\VID_&PID_&REV_")) <= sizeof(usbDeviceKey) );

    TRY {

        DeviceFlagsVidPidRev->AsULong64 = 0;
        DeviceFlagsVidPid->AsULong64 = 0;

        if (g_KseQueryDeviceFlags == NULL) {
            DeviceFlagsVidPidRev->AsULong64 = 0;
            DeviceFlagsVidPid->AsULong64 = 0;
            LEAVE;
        }

        //
        // See if there are any Device Flags set for this device's VID/PID/Rev
        //
        status = RtlStringCbPrintfW(usbDeviceKey,
                                    sizeof(usbDeviceKey),
                                    IsRootHub ?
                                      L"USB:ROOT_HUB\\VID_%S&PID_%S&REV_%S" :
                                      L"USB:USB\\VID_%S&PID_%S&REV_%S",
                                    VendorId,
                                    DeviceId,
                                    RevisionId);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to format device key string for KseQueryDeviceFLags %!STATUS!",
                       status);
            LEAVE;
        }

        g_KseQueryDeviceFlags(usbDeviceKey,
                              L"USB",
                              &DeviceFlagsVidPidRev->AsULong64);

        //
        // See if there are any Device Flags set for the VID/PID without Revision
        //
        status = RtlStringCbPrintfW(usbDeviceKey,
                                    sizeof(usbDeviceKey),
                                    IsRootHub ?
                                      L"USB:ROOT_HUB\\VID_%S&PID_%S" :
                                      L"USB:USB\\VID_%S&PID_%S",
                                    VendorId,
                                    DeviceId);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to format device key string for KseQueryDeviceFLags %!STATUS!",
                       status);
            LEAVE;
        }

        g_KseQueryDeviceFlags(usbDeviceKey,
                              L"USB",
                              &DeviceFlagsVidPid->AsULong64);
    } FINALLY {

    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBMISC_QueryKseGlobalFlags (
    __in
        PHUB_GLOBALS     HubGlobals
    )
/*++

Routine Description:

    Queries the KSE component for errata flags that apply to all
    hubs and devices

Returns:

    VOID

--*/
{
    KSE_GLOBAL_USB_FLAGS    globalUsbFlags;

    TRY {

        globalUsbFlags.AsULong64 = 0;

        if (g_KseQueryDeviceFlags == NULL) {
            LEAVE;
        }

        g_KseQueryDeviceFlags(L"USBHUB:GLOBAL_FLAGS",
                              L"USBHUB",
                              &globalUsbFlags.AsULong64);

        if (globalUsbFlags.DisableFastEnumeration == 1) {
            INTERLOCKED_SET_BIT_FIELD(HUB_GLOBAL_FLAGS, &HubGlobals->HubGlobalFlags, DisableFastEnumeration);
        }

    } FINALLY {

    }
}


BOOLEAN
HUBMISC_IsBitSet(
    __in
        PVOID   Bitmap,
    __in
        ULONG   BitNumber
    )
/*++
Routine Description:

    Check if a bit is set given a string of bytes.

Arguments:

Return Value:

    TRUE - if the corresponding bit is set.
    FALSE - otherwise.

-- */
{
    ULONG       dwordOffset;
    ULONG       bitOffset;
    PUCHAR      bitmap;

    bitmap = (PUCHAR) Bitmap;
    dwordOffset = BitNumber / 8;
    bitOffset = BitNumber % 8;

    return ((bitmap[dwordOffset] & (1 << bitOffset)) ? TRUE : FALSE);
}


NTSTATUS
HUBMISC_GetDeviceInterfacePath(
    __in
        LPCGUID                     InterfaceGuid,
    __in
        WDFDEVICE                   Device,
    __out
        WDFSTRING                   *DeviceInterfacePath,
    __in
        RECORDER_LOG                IfrLog
    )
/*++
Routine Description:

    Retrieves the device interface symbolic link for a USB device or hub

Arguments:

    InterfaceGuid - Device Interface GUID

    Device - WDF device to retrieve the symbolic link for

    SymbolicLinkName - WDF string handle for the returned symbolic link name

Return Value:

    NTSTATUS

-- */
{
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    NTSTATUS                status;

    TRY {

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = Device;

        *DeviceInterfacePath = NULL;

        status = WdfStringCreate(NULL, &objectAttributes, DeviceInterfacePath);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(IfrLog, TL_ERROR, Device, "Failed to create WDF string for symbolic link name %!STATUS!", status);
            LEAVE;
        }

        status = WdfDeviceRetrieveDeviceInterfaceString(Device,
                                                        InterfaceGuid,
                                                        NULL,
                                                        *DeviceInterfacePath);

    } FINALLY {
        if ((!NT_SUCCESS(status)) && (*DeviceInterfacePath != NULL)) {
            WdfObjectDelete(*DeviceInterfacePath);
            *DeviceInterfacePath = NULL;
        }
    }

    return status;
}


NTSTATUS
HUBMISC_VerifyCallerIsAdmin(
    __in
        RECORDER_LOG    IfrLog
    )
/*++
Routine Description:

    Determines if the calling process is running at admin level or higher

Arguments:

    None

Return Value:

    NTSTATUS

--*/
{
    BOOLEAN isAdmin;
    SECURITY_SUBJECT_CONTEXT subjectContext;
    PACCESS_TOKEN token;

    PAGED_CODE();

    TRY {

        SeCaptureSubjectContext(&subjectContext);
        SeLockSubjectContext(&subjectContext);

        token = SeQuerySubjectContextToken(&subjectContext);

        if (token == NULL) {
            DbgTraceTo(IfrLog, TL_ERROR, Hub, "SeQuerySubjectContextToken returned NULL token");
        }
        
        NT_ASSERT(token != NULL);
    
        isAdmin = SeTokenIsAdmin(token);

        SeUnlockSubjectContext(&subjectContext);
        SeReleaseSubjectContext(&subjectContext);

    } FINALLY {

    }

    return isAdmin==FALSE ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS;
}


DSM_EVENT
HUBMISC_CheckIfSetSelShouldBeSkipped (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        if (DeviceContext->DeviceHackFlags.SkipSetSel == 1) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckIfDeviceIsAtSuperSpeedAndNeedsToBeDisabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        //
        // If the device is at Super Speed and if it needs to be disabled
        //
        if ((DeviceContext->DeviceHackFlags.DisableSuperSpeed == 1) &&
            (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper)) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckIfSetIsochDelayShouldBeSkipped (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        if (DeviceContext->DeviceHackFlags.SkipSetIsochDelay == 1) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}


NTSTATUS
HUBMISC_GetActivityIdIrp (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    )
/*++

Routine Description:

    Wrapper for IoGetActivityIdIrp

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    TRY {

        if (g_IoGetActivityIdIrp == NULL) {
            status = STATUS_NOT_FOUND;
            LEAVE;
        }

        status = g_IoGetActivityIdIrp(Irp, Guid);

    } FINALLY {

    }

    return status;
}


NTSTATUS
HUBMISC_SetActivityIdIrp (
    __inout
        PIRP    Irp,
    __in_opt
        LPGUID  Guid
    )
/*++

Routine Description:

    Wrapper for IoSetActivityIdIrp

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    TRY {

        if (g_IoSetActivityIdIrp == NULL) {
            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        status = g_IoSetActivityIdIrp(Irp, Guid);

    } FINALLY {

    }

    return status;
}


VOID
HUBMISC_FreeActivityIdIrp (
    __inout
        PIRP    Irp
    )
/*++

Routine Description:

    Wrapper for IoFreetActivityIdIrp

Returns:

    VOID

--*/
{

    TRY {

        if (g_IoSetActivityIdIrp == NULL) {
            LEAVE;
        }

        g_IoFreeActivityIdIrp(Irp);

    } FINALLY {

    }
}


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PROOTHUB_20PORT_INFO
HUBMISC_GetRootHub20PortInfo (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        USHORT                  Index
    )
/*++

Routine Description:

    This function returns ROOTHUB_20PORT_INFO structure indexed
    into the array of ROOTHUB_20PORT_INFO structures returned by UCX

Return Value:

    Port number

--*/
{
    PROOTHUB_20PORT_INFO    *rootHub20PortInfoPointerArray;
    PROOTHUB_20PORT_INFO    rootHub20PortInfo;

    TRY {

        rootHub20PortInfoPointerArray = HubFdoContext->HubInformation.RootHub20PortInfoPointerArray;
        rootHub20PortInfoPointerArray += Index;

        rootHub20PortInfo = *rootHub20PortInfoPointerArray;

    } FINALLY {

        NT_ASSERTMSG("ROOTHUB_20PORT_INFO has invalid port number!", rootHub20PortInfo->PortNumber != 0);

    }

    return rootHub20PortInfo;

}


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PROOTHUB_30PORT_INFO_EX
HUBMISC_GetRootHub30PortInfo (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        USHORT                  Index
    )
/*++

Routine Description:

    This function returns ROOTHUB_30PORT_INFO_EX structure indexed
    into the array of ROOTHUB_30PORT_INFO_EX structures returned by UCX

Return Value:

    Port number

--*/
{
    PROOTHUB_30PORT_INFO_EX    *rootHub30PortInfoPointerArray;
    PROOTHUB_30PORT_INFO_EX    rootHub30PortInfo;

    TRY {

        rootHub30PortInfoPointerArray = HubFdoContext->HubInformation.RootHub30PortInfoPointerArray;
        rootHub30PortInfoPointerArray += Index;

        rootHub30PortInfo = *rootHub30PortInfoPointerArray;

    } FINALLY {

        NT_ASSERTMSG("ROOTHUB_30PORT_INFO has invalid port number!", rootHub30PortInfo->PortNumber != 0);

    }

    return rootHub30PortInfo;

}

PWCHAR
HUBMISC_StripSymbolicNamePrefix (
    __in
        PUNICODE_STRING     SymbolicLinkNameUnicode,
    __out
        size_t              *SymbolicLinkNameLength
    )
/*++

Routine Description:

    Strips the '\??\' prefix from a symbolic link name string

Arguments:

    SymbolicLinkNameUnicode - Unicode string containing the symbolic link name string

    SymbolicLinkNameLength - Address of a variable that will contain the stripped wide-char string's length

Return Value:

    Pointer to the stripped wide-char symbolic link name string, or NULL.

--*/
{
    PWCHAR      currentCharacter;
    size_t      length;

    TRY {

        currentCharacter = &SymbolicLinkNameUnicode->Buffer[0];
        length = (size_t)SymbolicLinkNameUnicode->Length;

        if (currentCharacter == NULL) {
            *SymbolicLinkNameLength = 0;
            LEAVE;
        }

        NT_ASSERT(*currentCharacter == L'\\');
        if (*currentCharacter == L'\\') {
            currentCharacter++;
            length -= sizeof(WCHAR);
            while (*currentCharacter && *currentCharacter != L'\\') {
                currentCharacter++;
                length -= sizeof(WCHAR);
            }
            NT_ASSERT(*currentCharacter == L'\\');
            if (*currentCharacter == L'\\') {
                currentCharacter++;
                length -= sizeof(WCHAR);
            }
        }

        *SymbolicLinkNameLength = length;

    } FINALLY {
    }

    return currentCharacter;
}

PSM_EVENT
HUBMISC_CheckIfResetCompletionRequiresPolling (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    There are certains hubs that will say the reset completed
    but the reset bit will show that the reset is still in
    progress. For such hubs, we need to do manual polling
    to check when the reset completes. But we will do it
    only if there is a hack flag for the hub

Return Value:

    VOID

--*/
{
    PSM_EVENT                       retEvent;


    TRY {

        //
        // We should only call this function is if the hub indicated that the
        // reset is still in progress
        //
        NT_ASSERT(PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus.Reset == 1);

        //
        // Port status shows reset is still in progress and we have a hack flag
        // for the hub, only then we will go into the polling mode
        //
        if (PortContext->CumulativePortChange.Usb30PortChange.ResetChange &&
            PortContext->HubFdoContext->HubInformation.HubFlags.AddDelayAfterResetComplete == 1) {

            retEvent =  PsmEventYes;

        } else {

            retEvent = PsmEventNo;

        }

    } FINALLY {

    }

    return retEvent;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_NotifyDeviceRemovalIfDeviceSupportsReattach(
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    Check if it is a boot device and if yes, report it
    missing to kernel.

Return Value:

    VOID

--*/
{

    TRY {

        if ((PortContext->PortFlags.ConnectedDeviceSupportsReAttach == 1) &&
            (PortContext->HubFdoContext->MuxContext.HubMuxFlags.HubResetInProgress == 0) &&
            (PortContext->PreviousPortStatus.Usb20PortStatus.CurrentConnectStatus == 0) &&
            (InterlockedExchange(&PortContext->DeviceContext->BootDeviceReportedMissing, 1) == 0)) {

            //
            // Make a note of the fact that the boot device went missing.
            // If and when we enumerate it again, we will update the SQM
            // registry key
            //
            if (PortContext->DeviceContext->DeviceFlags.DeviceIsHub == 0) {
                INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, PendingSQMUpdateForSurpriseRemoval);
            }

            DbgTraceTo(PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device being reported as missing on detach");
            HUBBOOT_NotifyBootDeviceRemoval(PortContext->DeviceContext->BootDeviceHandle);
        }

    } FINALLY {

    }

}

__drv_maxIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBMISC_IsBootDeviceReady(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This callback is invoked by the kernel to determine
    if the boot device is back.

Arguments:

Return Value:

    BOOLEAN

--*/
{
    NTSTATUS                    status;
    USB_PORT_STATUS_AND_CHANGE  portStatusChange;
    BOOLEAN                     retValue;


    TRY {

        //
        // If a hub reset is in progress, then we don't want to
        // block the system. We want it to complete as soon
        // as possible
        //
        if (DeviceContext->HubFdoContext->MuxContext.HubMuxFlags.HubResetInProgress == 1) {
            retValue = TRUE;
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device being reported as present because a hub reset is in progress");

            LEAVE;
        }

        DeviceContext->BootDeviceReportedMissing = 0;

        status = HUBHTX_GetPortStatusForBootDevice(DeviceContext,
                                                   &portStatusChange);
        //
        // If a hub reset is not in progress and the transfer to the hub
        // failed, then hub is in a bad state. We will assume that the
        // hub interrupt transfer is also going to fail and hub reset
        // recovery is soon going to be initiated by the state machines
        // Therefore we will not not report the device as missing so as
        // to let state machines make progress and do hub reset
        //
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device being reported as present because hub control transfer failed and we are expecting a hub reset");
            retValue = FALSE;
            LEAVE;
        }

        if ((DeviceContext->DeviceStateFlags.DifferentDeviceOnBootDevicePort == 1) &&
            (portStatusChange.PortChange.Usb20PortChange.ConnectStatusChange == 0)) {
            //
            // If we have detected a different device on the port and there
            // is no connect change, then we will keep reporting the boot
            // device missing
            //
            retValue = FALSE;
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                     TL_INFO,
                     Device,
                     "Boot Device being reported still missing because wrong device is present");
            LEAVE;
        }

        if (DeviceContext->PortContext->ConnectionStatus == DeviceFailedEnumeration) {
            retValue = FALSE;
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                     TL_INFO,
                     Device,
                     "Boot Device being reported still missing because last enumeration failed");
            LEAVE;
        }

        if (portStatusChange.PortStatus.Usb30PortStatus.CurrentConnectStatus == 1) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device being reported as present based because connect bit is set in the port status");
            retValue = TRUE;
            LEAVE;
        }

        if ((DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) &&
            (portStatusChange.PortStatus.Usb30PortStatus.PortLinkState == PORT_LINK_STATE_INACTIVE)) {
            //
            // For 3.0 ports, link being in SS.Inactive also indicates that a
            // device is present. But there is a catch. When the device disconnects,
            // the link tempeorily goes to SS.Inactive and due to an issue in
            // 3.0 spec some hub/controllers will only send port status change
            // for this intermediate state
            //
            //
            // If this is the first time we are seeing this state then
            // will assume it is the temperory state during disconnect.
            // If it is the second time, then it is safe to assume that
            // there really is a device but in error state becuase that
            // intermediate state can only live upto 20ms
            //
            if (DeviceContext->PortContext->PortFlags.SSInactiveForBootDevice == 0) {
                INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &DeviceContext->PortContext->PortFlags, SSInactiveForBootDevice);
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "Boot Device still being reported missing even though the link is SSInactive becasue it is the first time ");
                retValue = FALSE;
                LEAVE;

            }

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device being reported as present because link has stayed in SS.Inactive for long enough");
            retValue = TRUE;
            LEAVE;

        } else {
            INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &DeviceContext->PortContext->PortFlags, SSInactiveForBootDevice);

        }

        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                   TL_INFO,
                   Device,
                   "Boot Device being reported still missing based on the port status");
        retValue = FALSE;


    } FINALLY {
        if (retValue == FALSE) {
            if (InterlockedExchange(&DeviceContext->BootDeviceReportedMissing, 1) == 1) {
                //
                // Some other thread came and reported the boot device missing again,
                // so we will return TRUE from this callback.
                //
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "Boot Device being reported as present because some other thread reported it as missing");
                retValue = TRUE;
            }
        } else {

            INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &DeviceContext->PortContext->PortFlags, SSInactiveForBootDevice);   
            HUBMISC_PerformPostSurpriseRemovalRecoveryActionsIfBootDevice(DeviceContext);

        }

    }

    return retValue;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_NotifyEnumerationFailureForBootDevice(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This callback is invoked by the state machine when a
    boot device fails to re-enumerate

Arguments:

Return Value:

--*/
{
    TRY {

        DeviceContext->PortContext->ConnectionStatus = DeviceFailedEnumeration;

        //
        // We only want to notify about boot device removal once so as to
        // have faster recovery time when the boot device is connected back
        //
        if (InterlockedExchange(&DeviceContext->BootDeviceReportedMissing, 1) == 0) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device is being reported missing because it failed re-enumeration");
            HUBBOOT_NotifyBootDeviceRemoval(DeviceContext->BootDeviceHandle);

        } else {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Boot Device failed re-enumeration but has already been reported as missing");

        }

    } FINALLY {


    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_PerformPostSurpriseRemovalRecoveryActionsIfBootDevice(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This callback is invoked by the state machine when a
    device has been successfully re-enumerated. If the device
    is a boot device and has gone through suprise removal, then
    this function will perform some post-surprise-removal actions.

Arguments:

Return Value:

--*/
{
    WDF_OBJECT_ATTRIBUTES       attributes;
    WDF_WORKITEM_CONFIG         workitemConfig;
    WDFWORKITEM                 registryWorker;
    NTSTATUS                    status;

    TRY {

        if (DeviceContext->PortContext->PortFlags.PendingSQMUpdateForSurpriseRemoval == 0) {
            LEAVE;
        }


        INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &DeviceContext->PortContext->PortFlags, PendingSQMUpdateForSurpriseRemoval);

        // One of the actions that need to be performed is to update the global
        // registry key that keeps a count of times successful recovery from
        // surprise remove occured, for SQM purposes. But since registry can
        // access paging path, we can not block DSM in case the device gets
        // removed again. Therefore we will create a worker item for it and
        // proceed.

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        //
        // Device is never going to be deleted, so parent is really irrelvelant
        // but WDF requires us to give a device object as parent
        //
        attributes.ParentObject = WdfObjectContextGetObject(DeviceContext->PdoContext);

        WDF_WORKITEM_CONFIG_INIT(&workitemConfig, HUBREG_EvtWorkItemPerformPostSurpriseRemovalRecoveryActions);

        status = WdfWorkItemCreate(&workitemConfig,
                                   &attributes,
                                   &registryWorker);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfWorkItemCreate Failed for updating the global suprise removal count %!STATUS!",
                       status);
        } else {
            WdfWorkItemEnqueue(registryWorker);
        }

    } FINALLY {


    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_FlushUserInitiatedResetOnOverCurrent (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This routine stops allowing the queueing of UserInitiatedResetOnOverCurrent
    event to the port state machine. This is called by
    the state machine when it has made porgress after
    an overcurrent because either the over current got
    cleared on its own or because it recevied a cleanup
    event

Return Value:

    VOID

--*/
{

    TRY {

        INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, CanQueueOverCurrentReset);

    } FINALLY {

    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBMISC_GenerateUserNotificationForPersistentOverCurrent (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Fires the WMI notification that lets the user know about
    an overcurrent that has persisted for more than 500 ms. It
    also starts allowing the queueing of UserInitiatedResetOnOverCurrent
    event to the port state machine. This event will be queued
    when the user presses the reset button from the overcurrent UI

Return Value:

    VOID

--*/
{
    PAGED_CODE();

    TRY {

        INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, CanQueueOverCurrentReset);

        DbgTraceTo(PortContext->IfrLog,
                   TL_ERROR,
                   Port,
                   "HUBMISC_GenerateUserNotificationForPersistentOverCurrent: Firing WMI notification, HubFdoContext %p, PortContext %p, port %d",
                   PortContext->HubFdoContext,
                   PortContext,
                   PortContext->PortInformation.PortNumber
                   );

        WMI_FireNotification(PortContext->HubFdoContext,
                             PortContext->PortInformation.PortNumber,
                             OverCurrent);

    } FINALLY {

    }
}

DSM_EVENT
HUBMISC_CheckIf20TypeDeviceNeedsSecondReset (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the legacy second reset should be performed for a
    non super speed device

Returns:

    DsmYes or DsmEventNo

--*/
{
    DSM_EVENT           dsmEvent;

    TRY {
        NT_ASSERT(DeviceContext->DeviceDescriptor.bcdUSB < 0x300);
        //
        // If the version is 2X, then there is no comaptability
        // risk w.r.t. Win7 stack and therefore never do the second
        // reset.For less than equal to 2.0, minimize the compatiblity
        // risk by avoiding the second reset only for high speed devices
        // and only if the is is first enumeration try. Also for 2.0
        // devices we support a global hack flag that forces us to
        // do the second reset. 
        // In addition, we support a per device flag to do the second reset. 
        // The reason for doing that is because we have seen one superspeed 
        // device that has the following behavior: if it is working in the
        // 2.0 mode, it will not look for super speed termination on the
        // first port reset and requires a second reset to move back to
        // SuperSpeed
        //
        if (DeviceContext->DeviceHackFlags.DisableFastEnumeration == 1) {
            dsmEvent= DsmEventYes;
        } else if ((DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagVersion2X) ||
            ((DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedHigh) &&
             (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.DisableFastEnumeration == 0) &&
            (DeviceContext->EnumRetryCount == 0))) {
            dsmEvent= DsmEventNo;
        } else {
            dsmEvent= DsmEventYes;
        }



    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckIfSerialNumberIsIdentical (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Compares the serial number obtained during enumeration with the cached
    serial number from before and determine if the device connected after
    system resume or reset is the same device that was connected before

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT               dsmEvent;
    PUSB_STRING_DESCRIPTOR  stringDescriptor;
    ULONG                   stringLength;
    ULONG                   equalLength;
    PUCHAR                  serialNumberBuffer;
    ULONG                   serialNumberBufferLength;

    TRY {
        dsmEvent = DsmEventYes;
        stringDescriptor = NULL;

        if (DeviceContext->DeviceStateFlags.DecorateSerialNumber == 1) {
            //
            // If the serial number is decorated, then get rid of the decoration
            // for comparision purposes
            //
            serialNumberBufferLength = DeviceContext->SerialNumberId.LengthInBytes - USB_SERIAL_NUMBER_DECORATION_SIZE - sizeof(WCHAR);
            serialNumberBuffer = (PUCHAR)DeviceContext->SerialNumberId.Buffer + USB_SERIAL_NUMBER_DECORATION_SIZE;

        } else {
            serialNumberBufferLength = DeviceContext->SerialNumberId.LengthInBytes - sizeof(WCHAR);
            serialNumberBuffer = (PUCHAR)DeviceContext->SerialNumberId.Buffer;
        }

        stringLength =  DeviceContext->ControlRequest.NumberOfBytes - sizeof(USB_COMMON_DESCRIPTOR);

        if (serialNumberBufferLength != stringLength) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Length of the new serial number does not match that of the cached serial number");
            dsmEvent = DsmEventNo;
            LEAVE;
        }

        stringDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        equalLength = (ULONG)RtlCompareMemory(stringDescriptor->bString,
                                              serialNumberBuffer,
                                              stringLength);

        if (equalLength != stringLength) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "New serial number does not match cached serial number. Only %d bytes out of %d bytes match",
                       equalLength, stringLength);

            dsmEvent = DsmEventNo;
        }

    } FINALLY {

        if (dsmEvent == DsmEventNo) {
            USB_HW_VERIFIER_DEVICE_BREAK(DeviceContext, DeviceHwVerifierSerialNumberMismatchOnRenumeration);

            if (stringDescriptor) {
                EventWriteUSBHUB3_ETW_EVENT_USBENUM_SERIAL_MISMATCH_ON_RENUMERATION(
                    &DeviceContext->DsmContext.CurrentActivityId,
                    DeviceContext->HubFdoContext->HubParentInterface.Hub,
                    (LPCSTR)serialNumberBuffer,
                    serialNumberBufferLength,
                    stringDescriptor->bString,
                    stringLength);

            } else {
                EventWriteUSBHUB3_ETW_EVENT_USBENUM_SERIAL_MISMATCH_ON_RENUMERATION(
                    &DeviceContext->DsmContext.CurrentActivityId,
                    DeviceContext->HubFdoContext->HubParentInterface.Hub,
                    (LPCSTR)serialNumberBuffer,
                    serialNumberBufferLength,
                    0,
                    0);

            }

        }

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckIfSerialNumberShouldBeQueriedOnRenumeration (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device should be queried for serial
    number on re-enuemration

Returns:

    DsmYes or DsmEventNo

--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        //
        // Ideally we should always query for serial number, if the
        // device supports it. But for compat, we don't query <= 2.0
        // devices for it unless it is a boot device
        //
        if ((DeviceContext->DeviceFlags.SerialNumber == 1) &&
            ((DeviceContext->DeviceDescriptor.bcdUSB > 0x200) ||
            (DeviceContext->PdoContext->PdoFlags.InBootPath == 1))) {

            dsmEvent= DsmEventYes;
        } else {
            dsmEvent= DsmEventNo;
        }


    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBMISC_CheckIfProductIdDescriptorShouldBeQueried (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if the device should be queried for
    Product Id descriptor

Returns:

    DsmYes or DsmEventNo

--*/
{
    DSM_EVENT           dsmEvent;

    TRY {

        //
        // For compat with Win7 stack, we have the following checks
        //
        if ((DeviceContext->DeviceHackFlags.DisableSerialNumber == 0) &&
            HUBPDO_IsLanguageSupported(DeviceContext, AMERICAN_ENGLISH)) {

            dsmEvent= DsmEventYes;
        } else {
            dsmEvent= DsmEventNo;
        }


    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfWaitRequiredAfterSetAddress (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if we need to give the device some time
    after doing a set address before start interacting
    with the device on the new address

Returns:

    DsmYes or DsmEventNo

--*/
{
    DSM_EVENT           dsmEvent;

    TRY {
        //
        // For non super-speed devices, we need to provide the delay
        // as per the spec and compat. For super speed devices, we will
        // provide the delay if this device has exhibited any problems
        // during the enuemration process in the past
        //

        if (((DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) == 0) ||
            (DeviceContext->DeviceStateFlags.DeviceFailedAtLeastOneEnumeration == 1)) {
            dsmEvent= DsmEventYes;
        } else {
            dsmEvent= DsmEventNo;
        }


    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBMISC_CheckIfDualRoleSupported (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    DSM_EVENT   dsmEvent;

    TRY {

        dsmEvent = DsmEventNo;

        if (DeviceContext->DeviceFlags.DeviceIsDualRole == 1) {
            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}



VOID
HUBMISC_BugcheckSystemOnBootDeviceEnumerationFailure (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    During a re-enumeration, the boot device failed to enumerate.
    The system is going to bug-check soon anyways. We bug-check in
    USB for diagnosability

Returns:

    DsmYes or DsmEventNo

--*/
{

    TRY {

        Usb3_KeBugCheckEx(USB3_BUGCODE_BOOT_DEVICE_FAILED,
                          WdfDeviceWdmGetDeviceObject((WdfObjectContextGetObject(DeviceContext->PdoContext))),
                          DeviceContext->DeviceFlags.DeviceIsHub ? (ULONG_PTR) FailedBootDeviceType_UsbHub :
                                                                   (ULONG_PTR) FailedBootDeviceType_UsbStorageDevice,
                          (ULONG_PTR) NULL);
    } FINALLY {

    }

}

PSM_EVENT
HUBMISC_IsD3ColdSupported (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Checks if D3 cold is enabled for the associated device

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    PSM_EVENT               portEvent;

    TRY {

        //
        // For the device to be in D3 cold, the client driver should be
        // opted in, ACPI should support it, the device should be internal
        // and the PDO should be in D3
        //
        if ((PortContext->DeviceContext->DeviceFlags.D3ColdSupportedInAcpi == 1) &&
            (PortContext->DeviceContext->DeviceStateFlags.D3ColdEnabledByFunctionDriver == 1) &&
            (PortContext->PortInformation.PortFlags.PortFlagRemovable == 0) &&
            (PortContext->DeviceContext->DevicePowerState == PowerDeviceD3)) {
            portEvent = PsmEventYes;
        } else {
            portEvent = PsmEventNo;
        }

        if(portEvent == PsmEventYes) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Device attached to port supports D3COLD");
        }

    } FINALLY {

    }

    return portEvent;

}


VOID
HUBMISC_LogResetTimeout (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sets the problem description ID to indicate a reset timeout, and logs a WPP trace for the timeout.

Returns:

    VOID

--*/
{
    TRY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_RESET_TIMEOUT(
            &DeviceContext->DsmContext.CurrentActivityId,
            DeviceContext->UsbDevice);

        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                   TL_ERROR,
                   Device,
                   "HW_COMPLIANCE: Port Reset Timeout!!");

        DeviceContext->EnumMsgId = USBENUM_PORT_RESET_FAILURE;

        if (DeviceContext->PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.CurrentConnectStatus == 1) {
            USB_HW_VERIFIER_PORT_BREAK(DeviceContext->PortContext, HubHwVerifierPortResetTimeout);
        }

    } FINALLY {
    }
}

VOID
HUBMISC_LogPortLinkStateError (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Sets the problem description ID to indicate a reset timeout,
    and logs a WPP trace for the timeout.

Returns:

    VOID

--*/
{
    TRY {
        if (PortContext->PreviousPortStatus.Usb30PortStatus.PortLinkState == PORT_LINK_STATE_INACTIVE) {
            PortContext->DeviceContext->EnumMsgId = USBENUM_PORT_LINK_SSINACTIVE;
        } else if (PortContext->PreviousPortStatus.Usb30PortStatus.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE) {
            PortContext->DeviceContext->EnumMsgId = USBENUM_PORT_LINK_COMPLIANCE_MODE;
        }


    } FINALLY {
    }
}

NTSTATUS
HUBMISC_CreateWerReport(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        HUB_WER_REPORT_TYPE WerReportType,
    __in
        ULONG               SubReason
    )
/*++

Routine Description:

    This function queues or creates a WER report. A work item for creating the report
    is queued if the IRQL level is at DISPATCH or higher. The work item inturn will
    call this function at passive level.

Return Value:

    NTSTATUS

--*/
{
    WDF_WORKITEM_CONFIG   workItemConfig;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDFWORKITEM           werReportWorker;
    PWER_WORKITEM_CONTEXT context;
    NTSTATUS              status;
    ULONG                 bugCheckParam1;

    TRY {

        if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {

            //
            // Queue a work item to create the report
            //

            WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

            WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes,
                                                   WER_WORKITEM_CONTEXT);

            attributes.ParentObject = HubFdoContext->HubFdoDevice;

            WDF_WORKITEM_CONFIG_INIT(&workItemConfig, HUBMISC_WerReportWorkItem);

            status = WdfWorkItemCreate(&workItemConfig,
                                       &attributes,
                                       &werReportWorker);

            if (!NT_SUCCESS(status)) {

                //
                // Log failure to queue work item
                //

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "Unable to create work item live dump creation %!STATUS!",
                           status);
            } else {

                //
                // Set WER context and queue report
                //

                context = HUBMISC_GetWerWorkItemContext(werReportWorker);
                context->HubFdoContext = HubFdoContext;
                context->WerReportType = WerReportType;
                context->SubReason = SubReason;

                WdfWorkItemEnqueue(werReportWorker);
            }

        } else {

            //
            // Find the bugcheck parameter for this WER report type.
            //

            switch (WerReportType) {

                case  HubWerSuccessfulReset:
                      bugCheckParam1 = USB3_WER_BUGCODE_USBHUB3_HUB_RESET_SUCCESSFUL;
                      break;
                case HubWerFailedReset:
                      bugCheckParam1 = USB3_WER_BUGCODE_USBHUB3_HUB_RESET_UNSUCCESSFUL;
                      break;
                case HubWerFailedEnumeration:
                      bugCheckParam1 = USB3_WER_BUGCODE_USBHUB3_DEVICE_ENUMERATION_FAILURE;
                      break;
                default:
                      bugCheckParam1 = 0;
            }


            //
            // Create the WER report
            //

            status = WER_CreateReport(HubFdoContext,
                                      NULL,
                                      NULL,
                                      bugCheckParam1,
                                      SubReason);
        }
    }

    FINALLY {
    }

    return status;
}

_Use_decl_annotations_
VOID
HUBMISC_WerReportWorkItem(
        WDFWORKITEM WorkItem
    )
/*++

Routine Description:

    Work item for creating WER reports

Returns:

    VOID

--*/
{
    PWER_WORKITEM_CONTEXT workItemContext;

    PAGED_CODE();

    TRY {

        workItemContext = HUBMISC_GetWerWorkItemContext(WorkItem);

        HUBMISC_CreateWerReport(workItemContext->HubFdoContext,
                                workItemContext->WerReportType,
                                workItemContext->SubReason);

    } FINALLY {
        WdfObjectDelete(WorkItem);
    }
}

VOID
HUBMISC_ReportPnPFailureProblem(
    __in
        WDFDEVICE   Device,
    __in
        ULONG       MsgId
    )
 /*++

Routine Description:

    Set the problem description string if we are reporting the a hub or device PnP failure.

Arguments:

Return Value:

    VOID

--*/
{
    PWCHAR                  problemDescString;
    PMESSAGE_RESOURCE_ENTRY messageEntry;
    size_t                  messageStringSize;
    size_t                  problemDescStringSize;
    WCHAR                   driverPath[] = L"@System32\\drivers\\usbhub3.sys";
    NTSTATUS                status;
    BOOLEAN                 probDescSet;

    PAGED_CODE();

    TRY {

        problemDescString = NULL;
        probDescSet = FALSE;
        status = STATUS_INTERNAL_ERROR;

        if (MsgId == 0) {
            LEAVE;
        }

        //
        // Extract the problem description string from the message table.
        // This is the fallback string in case Device Manager can't extract
        // the string for the locale it is running under.
        //
#pragma prefast(disable:28175, "Disable prefast warning about accessing the driver object's DriverStart member")
        status = RtlFindMessage(g_Usbhub3DriverObject->DriverStart,
                                11, // RT_MESSAGETABLE
                                LANG_NEUTRAL,
                                MsgId,
                                &messageEntry);

        if (!NT_SUCCESS(status)) {
            NT_ASSERTMSG("Unable to extract problem description from message table", 0);
            LEAVE;
        }

        messageStringSize = messageEntry->Length - FIELD_OFFSET(MESSAGE_RESOURCE_ENTRY, Text);

        if (messageStringSize > (NTSTRSAFE_MAX_CCH * sizeof(WCHAR))) {
            LEAVE;
        }

        //
        // Get the length of the extracted message string
        //
        status = RtlUnalignedStringCbLengthW((PWSTR)messageEntry->Text,
                                             messageStringSize,
                                             &messageStringSize);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Compute the size of the indirect string we will build.  It will have the following
        // format:
        //
        //     @System32\\drivers\\usbhub3.sys,#<MsgId>;<Fallback String>
        //
        // Where <MsgId> is the ULONG message ID in decimal for the message string (max 10 characters)
        // and <Fallback String> is the string extracted from the message table.
        //
        problemDescStringSize = sizeof(driverPath) +  // "@System32\drivers\usbhub3.sys"
                                13 * sizeof(WCHAR) +  // ",<MsgId>;"
                                messageStringSize +   // <Fallback String>
                                sizeof(WCHAR);        // terminating NULL

        problemDescString = ExAllocatePoolWithTag(NonPagedPool,
                                                  problemDescStringSize,
                                                  USBHUB3_HUB_TAG);

        if (problemDescString == NULL) {
            LEAVE;
        }

        status = RtlStringCbPrintfW(problemDescString,
                                    problemDescStringSize,
                                    L"%s,#%d;%s",
                                    driverPath,
                                    MsgId,
                                    (PWCHAR)messageEntry->Text);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = RtlUnalignedStringCbLengthW(problemDescString,
                                             problemDescStringSize,
                                             &problemDescStringSize);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Set the problem description string for Device Manager
        //
        probDescSet = TRUE;
        status = IoSetDevicePropertyData(WdfDeviceWdmGetPhysicalDevice(Device),
                                         &DEVPKEY_Device_DriverProblemDesc,
                                         LOCALE_NEUTRAL,
                                         0,
                                         DEVPROP_TYPE_STRING_INDIRECT,
                                         (ULONG)(problemDescStringSize+sizeof(WCHAR)),
                                         problemDescString);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to set driver-reported problem description string: %!STATUS!",
                       status);
        }

    } FINALLY {
        if(probDescSet != TRUE) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                TL_ERROR,
                Device,
                "HUBMISC_ReportPnPFailureProblem hit a LEAVE before attempting to set problem description string. Status %!STATUS! may be relevant.",
                status);
        }
        if (problemDescString) {
            ExFreePoolWithTag(problemDescString, USBHUB3_HUB_TAG);
        }
    }
}


VOID
HUBMISC_ConvertUsbDeviceIdsToString(
    __in
        PUSB_DEVICE_DESCRIPTOR          DeviceDescriptor,
    __out_ecount(MAX_VENDOR_ID_STRING_LENGTH)
        PCHAR                           VendorId,
    __out_ecount(MAX_DEVICE_ID_STRING_LENGTH)
        PCHAR                           DeviceId,
    __out_ecount(MAX_REVISION_ID_STRING_LENGTH)
        PCHAR                           RevisionId
    )
/*++

Routine Description:

    Converts Ids in the USB device descriptor to
    strings

Returns:

    VOID

--*/
{

    TRY {

    C_ASSERT(MAX_VENDOR_ID_STRING_LENGTH >= 5);
    C_ASSERT(MAX_DEVICE_ID_STRING_LENGTH >= 5);
    C_ASSERT(MAX_REVISION_ID_STRING_LENGTH >= 5);

    RtlStringCchPrintfA(VendorId,
                        MAX_VENDOR_ID_STRING_LENGTH,
                        "%04X",
                        DeviceDescriptor->idVendor);

    RtlStringCchPrintfA(DeviceId,
                        MAX_DEVICE_ID_STRING_LENGTH,
                        "%04X",
                        DeviceDescriptor->idProduct);

    RtlStringCchPrintfA(RevisionId,
                        MAX_REVISION_ID_STRING_LENGTH,
                        "%04X",
                        DeviceDescriptor->bcdDevice);

    } FINALLY {
    }
}

VOID
HUBMISC_LogDescriptorValidationErrorForDevice (
    __in
        PVOID                           Context,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error
    )
/*++

Routine Description:

    Logs an ETW event for descriptor validation failure

Returns:

    VOID

--*/
{
    PDEVICE_CONTEXT     deviceContext;
    TRY {

        deviceContext = (PDEVICE_CONTEXT)Context;

        if (deviceContext) {
            EventWriteUSBHUB3_ETW_EVENT_DESCRIPTOR_VALIDATION_FAILURE(
                &deviceContext->DsmContext.CurrentActivityId,
                deviceContext->HubFdoContext->HubParentInterface.Hub,
                deviceContext->UsbDevice,
                deviceContext->PortContext->PortInformation.PortNumber,
                Error);
            USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierDescriptorValidationFailure);

            RtlSetBit(&deviceContext->DescriptorValidationBitmapHeader, Error); 
        }


    } FINALLY {
    }
}

VOID
HUBMISC_LogDescriptorValidationWarningForDevice (
    __in
        PVOID                           Context,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error
    )
/*++

Routine Description:

    Logs an ETW event for descriptor validation warning

Returns:

    VOID

--*/
{
    PDEVICE_CONTEXT     deviceContext;
    TRY {

        deviceContext = (PDEVICE_CONTEXT)Context;

        if (deviceContext) {
            EventWriteUSBHUB3_ETW_EVENT_DESCRIPTOR_VALIDATION_WARNING(
                &deviceContext->DsmContext.CurrentActivityId,
                deviceContext->HubFdoContext->HubParentInterface.Hub,
                deviceContext->UsbDevice,
                deviceContext->PortContext->PortInformation.PortNumber,
                Error);
            USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierDescriptorValidationFailure);

            RtlSetBit(&deviceContext->DescriptorValidationBitmapHeader, Error); 
        }


    } FINALLY {
    }
}

VOID
HUBMISC_LogDescriptorValidationErrorForHub (
    __in
        PVOID                           Context,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error
    )
/*++

Routine Description:

    Logs an ETW event for descriptor validation failure for a hub

Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT)Context;

        if (hubFdoContext) {
            EventWriteUSBHUB3_ETW_EVENT_DESCRIPTOR_VALIDATION_FAILURE(
                0,
                hubFdoContext->HubParentInterface.Hub,
                hubFdoContext->HubParentInterface.Hub,
                0,
                Error);
        }

    } FINALLY {
    }
}          


VOID
HUBMISC_LogDescriptorValidationWarningForHub (
    __in
        PVOID                           Context,
    __in
        DESCRIPTOR_VALIDATION_ERROR     Error
    )
/*++

Routine Description:

    Logs an ETW event for descriptor validation failure for a hub

Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT)Context;

        if (hubFdoContext) {
            EventWriteUSBHUB3_ETW_EVENT_DESCRIPTOR_VALIDATION_WARNING(
                0,
                hubFdoContext->HubParentInterface.Hub,
                hubFdoContext->HubParentInterface.Hub,
                0,
                Error);
        }

    } FINALLY {
    }
}          

#if DBG

VOID
HUBMISC_InducePowerStateFailureBugcheck()
/*++

Routine Description:

This routine loops untill g_HangD0Exit is reset. This is used for
testing bugcheck 0x9F dump creation. This routine should be called
from a PNP power callback routine.

Returns:

VOID

--*/
{
while (g_TriggerPowerStateFailureBugcheck) {
int i = 0;
i++;
}
}

VOID
HUBMISC_InduceLiveDumpCreation(
    __in
        PHUB_FDO_CONTEXT HubFdoContext,
        ULONG            SubReason
    )
/*++

Routine Description:

    This routine will create a live dump if the global variable g_TriggerLiveDump
    is set.

Returns:

    VOID

--*/
{
    NTSTATUS status;

    TRY {

        if (!g_TriggerLiveDump) {
            LEAVE;
        }

        //
        // g_TriggerLiveDump can be set in the debugger for testing live dump creation
        //

        status = HUBMISC_CreateWerReport(HubFdoContext,
                                         g_TriggerLiveDumpReportType,
                                         SubReason);

        if (NT_SUCCESS(status)) {

            //
            // Reset global variable if the WER report was queued successfully
            //

            g_TriggerLiveDump = FALSE;

        }
    }

    FINALLY {
    }

}
#endif

