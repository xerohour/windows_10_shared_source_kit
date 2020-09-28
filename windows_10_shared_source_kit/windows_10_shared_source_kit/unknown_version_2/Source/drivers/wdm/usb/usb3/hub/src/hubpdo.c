/*++

Copyright (c) Microsoft Corporation

Module Name:

    hubpdo.c

Abstract:

    Handles the events and operations for a child PDO.

Author:


Environment:

    Kernel mode only.

--*/
#include "pch.h"

// For DRM support - (Digital Rights Management) backwards compatibility
// NOTE: including only ksdrmhlp.h allows usbhub to build with ntddk.h instead of wdm.h
#include <ksdrmhlp.h>

#include "hubpdo.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, HUBPDO_CreatePdo)
#pragma alloc_text (PAGE, HUBPDO_CreateUnknownPdo)
#pragma alloc_text (PAGE, HUBPDO_EvtDevicePrepareHardware)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceReleaseHardware)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceSurpriseRemoval)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceQueryStop)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceQueryRemove)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceReportedMissing)
#pragma alloc_text (PAGE, HUBPDO_MakePdoName)
#pragma alloc_text (PAGE, HUBPDO_AssignPDOIds)
#pragma alloc_text (PAGE, HUBPDO_CreatePdoInternal)
#pragma alloc_text (PAGE, HUBPDO_D3ColdSupportInterfaceGetIdleWakeInfo)
#pragma alloc_text (PAGE, HUBPDO_D3ColdSupportInterfaceGetD3ColdCapability)
#pragma alloc_text (PAGE, HUBPDO_D3ColdSupportInterfaceGetD3ColdBusDriverSupport)
#pragma alloc_text (PAGE, HUBPDO_EvtDeviceResourceRequirementsQuery)
#pragma alloc_text (PAGE, HUBPDO_PublishBillboardDetails)
#pragma alloc_text (PAGE, HUBPDO_BillboardCleanup)
#pragma alloc_text (PAGE, HUBPDO_PublishDualRoleFeaturesProperty)
#endif

#define USBPORT_BAD_HANDLE ((PVOID)(-1))
// last supported function
#define URB_FUNCTION_LAST   URB_FUNCTION_RESERVE_0X0034

typedef enum _IOCTL_ACTION {
    IoctlActionUndefined,
    IoctlActionForwardToUCX,
    IoctlActionCompleteHere,
    IoctlActionForwardToQueue,
    IoctlActionForwardToParentQueue,
    IoctlActionAsyncForwardToQueue,
    IoctlActionDoNothing
} IOCTL_ACTION, *PIOCTL_ACTION;

const CHAR PDOTag[]            = "DSM PDO Tag";

extern PISM_STATE_ENTRY ISMStateTable[];


__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
HUBPDO_GetUSBDErrorFromNTStatus(
    __in
        NTSTATUS            Status
    )
/*++

Routine Description:

    Returns the USBD_STATUS corresponding to a
    NTSTATUS which could be returned by WDF


Return Value:

    USBD_STATUS

--*/
{
    USBD_STATUS usbdStatus;

    TRY {

        //
        // Usbhub3 has always made the following conversion, which is not consistent
        // with shared function NTSTATUS_To_USBD_STATUS() or with USBPORT.
        //
        if (Status == STATUS_DEVICE_NOT_CONNECTED) {

            usbdStatus = USBD_STATUS_DEVICE_GONE;
            LEAVE;
        }

        usbdStatus = NTSTATUS_To_USBD_STATUS(Status);

        //
        // USBD_STATUS_INVALID_PARAMETER was once returned
        // for all errors in XP, so we use it as a catchall.
        // However, maintain an assert here to catch new NTSTATUSes
        // that need to be changed for compat reasons.
        //
        if (usbdStatus == USBD_STATUS_INVALID_PARAMETER &&
            Status != STATUS_INVALID_PARAMETER &&
            Status != STATUS_UNSUCCESSFUL) {

            NT_ASSERTMSG("URB completed with status code that we don't have a USBD_STATUS mapping", FALSE);

            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "URB completed with status code that we don't have a USBD_STATUS mapping: %!STATUS!",
                       Status);
        }

    } FINALLY {


    }

    return usbdStatus;
}


VOID
HUBPDO_CompleteClientSerialRequestWithStatusSuccess (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
/*++

Routine Description:

    Called by DSM to complete the currently pending IOCTL with
    success

Return Value:

    VOID

--*/
{
    WDFREQUEST              currentRequest;
    WDF_REQUEST_PARAMETERS  parameters;
    PURB                    urb;
    PUSB_RESET_FLAGS        resetFlags;

    TRY {

        currentRequest = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(currentRequest,
                                &parameters);

        NT_ASSERT(DeviceContext->CurrentClientRequest != NULL);

        if (parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {

            urb = parameters.Parameters.Others.Arg1;

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_COMPLETE(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                urb->UrbHeader.Function,
                urb->UrbHeader.Status);

            if (((urb->UrbHeader.Function == URB_FUNCTION_SELECT_CONFIGURATION) &&
                 (urb->UrbSelectConfiguration.ConfigurationDescriptor != NULL)) ||
                (urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE)) {

                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);
            }
        }

        if (parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_RESET_PORT_ASYNC) {

            *(PULONG)parameters.Parameters.Others.Arg1 = 0;

            resetFlags  = parameters.Parameters.Others.Arg1;

            if (DeviceContext->PdoContext->PdoFlags.PowerLostOnLastReset) {
                resetFlags->DevicePowerCycled = 1;
                INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, PowerLostOnLastReset);
            }

            if (DeviceContext->PdoContext->PdoFlags.ProgrammingLostOnLastReset) {
                resetFlags->ControllerProgrammingLost = 1;
                INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ProgrammingLostOnLastReset);
            }
        }

        currentRequest = DeviceContext->CurrentClientRequest;
        DeviceContext->CurrentClientRequest = NULL;
        //
        // If we successfully completed any client request then we set the
        // the connection status back to connected. This is only for the purpose
        // for a user mode query
        //
        DeviceContext->PortContext->ConnectionStatus = DeviceConnected;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_COMPLETE(
            &DeviceContext->DsmContext.CurrentActivityId,
            DeviceContext->UsbDevice,
            parameters.Parameters.Others.IoControlCode,
            STATUS_SUCCESS);

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        WdfRequestComplete(currentRequest,
                           STATUS_SUCCESS);

    } FINALLY {

    }

}


VOID
HUBPDO_CompleteClientSerialRequestWithStatusFailed (
    __in
        PDEVICE_CONTEXT         DeviceContext
    )
/*++

Routine Description:

    Called by DSM to complete the currently pending IOCTL with
    STATUS_UNSUCCESSFUL

Return Value:

    VOID

--*/
{
    WDFREQUEST              currentRequest;
    WDF_REQUEST_PARAMETERS  parameters;
    PURB                    urb;


    TRY {

        NT_ASSERT(DeviceContext->CurrentClientRequest != NULL);

        currentRequest = DeviceContext->CurrentClientRequest;
        DeviceContext->CurrentClientRequest = NULL;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(currentRequest,
                                &parameters);

        if (parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {

            //
            // We need to set the URB status also
            //
            urb = parameters.Parameters.Others.Arg1;
            urb->UrbHeader.Status = HUBPDO_GetUSBDErrorFromNTStatus(STATUS_UNSUCCESSFUL);

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_COMPLETE(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                urb->UrbHeader.Function,
                urb->UrbHeader.Status);

            if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE) {
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);
            }

            if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_CONFIGURATION) {

                //
                // COMPAT: Win7 USB hub driver will set the configuration handle to
                //         NULL on any failure of select configuration URB
                //
                urb->UrbSelectConfiguration.ConfigurationHandle = NULL;

            } else if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE) {

                //
                // COMPAT: Win7 USB hub driver will set the interface handle to
                //         -1 on any failure of the select interface URB
                //
                urb->UrbSelectInterface.Interface.InterfaceHandle = USBPORT_BAD_HANDLE;
            }
        }

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_COMPLETE(
            &DeviceContext->DsmContext.CurrentActivityId,
            DeviceContext->UsbDevice,
            parameters.Parameters.Others.IoControlCode,
            (ULONG)STATUS_UNSUCCESSFUL);

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        WdfRequestComplete(currentRequest,
                           STATUS_UNSUCCESSFUL);

    } FINALLY {

    }

}


VOID
HUBPDO_CompleteClientSerialRequestWithLastStatus (
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Called by DSM to complete the currently pending IOCTL with
    the NTSTATUS and USBDSTATUS that was stored by the last UCX
    operation or transfer operation

Return Value:

    VOID

--*/
{
    WDFREQUEST              currentRequest;
    WDF_REQUEST_PARAMETERS  parameters;
    PURB                    urb;
    NTSTATUS                status;

    TRY {

        NT_ASSERTMSG("Last Operation NT Status is not set to a failure code",
                     DeviceContext->LastOperationNtStatus != 0);

        status = DeviceContext->LastOperationNtStatus;
        currentRequest = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(currentRequest,
                                &parameters);

        if (parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {

            //
            // We need to set the URB status also
            //
            urb = parameters.Parameters.Others.Arg1;

            urb->UrbHeader.Status = DeviceContext->LastOperationUsbdStatus;

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_COMPLETE(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                urb->UrbHeader.Function,
                urb->UrbHeader.Status);

            if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE) {
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigurationIsValid);
            }

            if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_CONFIGURATION) {

                //
                // COMPAT: Win7 USB hub driver will set the configuration handle to
                //         NULL on any failure of select configuration URB
                //
                urb->UrbSelectConfiguration.ConfigurationHandle = NULL;

            }  else if (urb->UrbHeader.Function == URB_FUNCTION_SELECT_INTERFACE) {

                //
                // COMPAT: Win7 USB hub driver will set the interface handle to
                //         -1 on any failure of the select interface URB
                //
                urb->UrbSelectInterface.Interface.InterfaceHandle = USBPORT_BAD_HANDLE;
            }

        }

        DeviceContext->CurrentClientRequest = NULL;
        DeviceContext->LastOperationNtStatus = 0;
        DeviceContext->LastOperationUsbdStatus = 0;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_COMPLETE(
            &DeviceContext->DsmContext.CurrentActivityId,
            DeviceContext->UsbDevice,
            parameters.Parameters.Others.IoControlCode,
            status);

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        if (NT_SUCCESS(status)) {
            NT_ASSERTMSG("Last Status not set to failure code",
                         FALSE);
            status = STATUS_UNSUCCESSFUL;
        }

        WdfRequestComplete(currentRequest,
                           status);

    } FINALLY {

    }

}


NTSTATUS
HUBPDO_EvtDeviceSelfManagedIoSuspend(
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Called when KMDF is moving us out of D0 and we need
    to handle IO differently

Returns:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;

    TRY {

        pdoContext = GetHubPdoContext(Device);
        pdoContext->PdoFlags.InD0 = 0;

    } FINALLY {

    }

    return STATUS_SUCCESS;

}


NTSTATUS
HUBPDO_EvtDeviceSelfManagedIoRestart(
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Called when KMDF is moving us back into D0 and we need
    to handle IO differently

Returns:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;

    TRY {

        pdoContext = GetHubPdoContext(Device);
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, InD0);

    } FINALLY {

    }

    return STATUS_SUCCESS;

}

NTSTATUS
HUBPDO_EvtDeviceQueryStop(
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Called when KMDF is about to stop the device
    for Pnp rebalance. The only reason we register
    for this callback is so that when the ReleaseHardware
    comes, we can tell if it is due to a Pnp rebalance

Returns:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;

    PAGED_CODE ();

    TRY {

        pdoContext = GetHubPdoContext(Device);
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, QueryStopped);

    } FINALLY {

    }

    return STATUS_SUCCESS;

}

NTSTATUS
HUBPDO_EvtDeviceQueryRemove(
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Called when the device is being gracefully removed.
    The only reason we register for this callback is so
    that when the ReleaseHardware comes, we can tell if
    it is due to device being disabled

Returns:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;

    PAGED_CODE ();

    TRY {

        pdoContext = GetHubPdoContext(Device);
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, QueryRemoved);

    } FINALLY {

    }

    return STATUS_SUCCESS;

}


NTSTATUS
HUBPDO_EvtDeviceSelfManagedIoInit(
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Called by KMDF to initialize our self managed IO

Returns:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;

    TRY {

        pdoContext = GetHubPdoContext(Device);
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, InD0);

    } FINALLY {

    }

    return STATUS_SUCCESS;

}

NTSTATUS
HUBPDO_DrmSetContentId (
    PIRP Irp,
    PKSP_DRMAUDIOSTREAM_CONTENTID KsProperty,
    PKSDRMAUDIOSTREAM_CONTENTID Data
)
/*++

Routine Description:

    Magic function to process KS data

Arguments:

Return Value:

    ntStatus

Remarks:

    In Windows 8 timeframe, it was determined that the so-called
    "TRUSTEDAUDIODRIVERS" content protection (a.k.a. "Secure Audio Path") does
    not require drivers other than the actual audio driver to handle this KS
    property (an IOCTL). However pre-existing 3rd party USB audio drivers might
    still call DrmForwardXxx causing this KS property to be sent to this
    driver. To support such drivers this driver will continue to handle this KS
    property, but will not call DrmForwardContentToDeviceObject. Therefore,
    other USB drivers lower in the device stack and higher in the PnP tree no
    longer need to support this KS property IOCTL.

--*/
{
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(KsProperty);
    UNREFERENCED_PARAMETER(Data);

    return STATUS_SUCCESS;
}

VOID
HUBPDO_EvtIoDeviceControl (
    WDFQUEUE                Queue,
    WDFREQUEST              Request,
    size_t                  OutputBufferLength,
    size_t                  InputBufferLength,
    ULONG                   IoControlCode
    )
/*++

Routine Description:

    This is the callback routine for when IOCTLs sent to a PDO which are handled by
    the hub driver get presented from the serial queue.

    In this routine, we will just fail the WDFREQUEST for now.

Return Value:

    VOID

--*/
{
    NTSTATUS    status;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(Queue);

    TRY {

        switch (IoControlCode) {
        case IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER:
            WdfRequestComplete(Request, STATUS_NOT_SUPPORTED);
            break;


#ifdef DRM_SUPPORT
        case IOCTL_KS_PROPERTY:
            status = KsPropertyHandleDrmSetContentId(WdfRequestWdmGetIrp(Request), HUBPDO_DrmSetContentId);
            WdfRequestComplete(Request, status);
            break;
#endif

        default:
            //
            // 2.0 stack completes an IOCTL with the same status. We need to do the same
            // to maintain compat.
            //
            status = WdfRequestGetStatus(Request);
            WdfRequestComplete(Request, status);
        }

    } FINALLY {
    }

    return;
}


VOID
HUBPDO_EvtIoInternalDeviceControl (
    WDFQUEUE                Queue,
    WDFREQUEST              Request,
    size_t                  OutputBufferLength,
    size_t                  InputBufferLength,
    ULONG                   IoControlCode
    )
/*++

Routine Description:

    This is the callback routine for when IOCTLs sent to a PDO which are handled by
    the hub driver get presented from the serial queue.

    In this routine, we will save the WDFREQUEST and queue the appropriate event
    to the DSM.

Return Value:

    VOID

--*/
{
    PURB                            urb;
    WDF_REQUEST_PARAMETERS          parameters;
    PHUB_PDO_CONTEXT                pdoContext;
    DSM_EVENT                       dsmEvent;
    PDEVICE_CONTEXT                 deviceContext;
    PHUB_GLOBALS                    hubGlobals;
    GUID                            activityId;
    NTSTATUS                        status;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    TRY {

        RtlZeroMemory(&activityId, sizeof(GUID));

        pdoContext = GetHubPdoContext(WdfIoQueueGetDevice(Queue));

        pdoContext->AssociatedDSM->CurrentClientRequest = Request;

        deviceContext = pdoContext->AssociatedDSM;

        hubGlobals = GetHubGlobals(WdfGetDriver());

        if (hubGlobals->HubGlobalFlags.EtwEnabled == 1) {

            //
            // See if the request already has an activity ID set for it
            //
            status = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request),
                                              &activityId);

            if (!NT_SUCCESS(status)) {

                //
                // Try to allocate our own activity ID for the client request.
                // DSM will extract this later and set it for its pre-allocated
                // requests.
                //
                status = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                              &activityId);

                if (NT_SUCCESS(status)) {
                    HUBMISC_SetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);
                }
            }
        }

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_START(
            &activityId,
            deviceContext->UsbDevice,
            IoControlCode);

        switch (IoControlCode) {

        case IOCTL_INTERNAL_USB_SUBMIT_URB:

            WDF_REQUEST_PARAMETERS_INIT(&parameters);
            WdfRequestGetParameters(Request,
                                    &parameters);
            urb = parameters.Parameters.Others.Arg1;

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_START(
                &activityId,
                deviceContext->UsbDevice,
                urb->UrbHeader.Function);

            switch (urb->UrbHeader.Function) {

            case URB_FUNCTION_SELECT_CONFIGURATION:

                //
                // COMPAT: Win7 always sets UsbdDeviceHandle
                //
                urb->UrbHeader.UsbdDeviceHandle = deviceContext->UsbDevice;

                if (urb->UrbSelectConfiguration.ConfigurationDescriptor == NULL) {
                    //
                    // This is a deconfig request
                    //
                    dsmEvent = DsmEventClientRequestSelectNullConfiguration;

                    //
                    // COMPAT: Win7 hub driver sets the config handle to NULL
                    //
                    urb->UrbSelectConfiguration.ConfigurationHandle = NULL;

                } else {

                    dsmEvent = DsmEventClientRequestSelectConfiguration;

                }
                break;

            case URB_FUNCTION_SELECT_INTERFACE:
                dsmEvent = DsmEventClientRequestSetInterface;
                break;
            case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL:
                USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierClientInitiatedResetPipe);
                dsmEvent = DsmEventClientRequestResetPipe;

                EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
                    deviceContext,
                    IOCTL_INTERNAL_USB_SUBMIT_URB,
                    URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL);
                break;
            case URB_FUNCTION_SYNC_RESET_PIPE:
                USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierClientInitiatedResetPipe);
                dsmEvent = DsmEventClientRequestSyncResetPipe;

                EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
                    deviceContext,
                    IOCTL_INTERNAL_USB_SUBMIT_URB,
                    URB_FUNCTION_SYNC_RESET_PIPE);
                break;
            case URB_FUNCTION_SYNC_CLEAR_STALL:
                USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierClientInitiatedResetPipe);
                dsmEvent = DsmEventClientRequestSyncClearStall;

                EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
                    deviceContext,
                    IOCTL_INTERNAL_USB_SUBMIT_URB,
                    URB_FUNCTION_SYNC_CLEAR_STALL);
                break;
            case URB_FUNCTION_OPEN_STATIC_STREAMS:
            case URB_FUNCTION_CLOSE_STATIC_STREAMS:
                dsmEvent = DsmEventClientRequestOpenOrCloseStreams;
                break;
            default:
                NT_ASSERTMSG("PDO got an URB that it shouldn't need to handle", FALSE);
                dsmEvent = DsmEventNull;
                break;
            }

            break;

        case IOCTL_INTERNAL_USB_RESET_PORT:
        case IOCTL_INTERNAL_USB_RESET_PORT_ASYNC:
            USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierClientInitiatedResetPort);

            pdoContext->AssociatedDSM->PortContext->ConnectionStatus = DeviceReset;
            dsmEvent = DsmEventClientRequestReset;

            EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
                deviceContext,
                IOCTL_INTERNAL_USB_RESET_PORT_ASYNC,
                0);
            break;

        default:
            NT_ASSERTMSG("PDO got an IOCTL that it shouldn't need to handle", FALSE);
            dsmEvent = DsmEventNull;
            break;
        }

    } FINALLY {

        NT_ASSERT(dsmEvent != DsmEventNull);
        //
        // Lets actually send the event
        //
        HUBSM_AddDsmEvent(pdoContext->AssociatedDSM,
                          dsmEvent);

    }

    return;

}

NTSTATUS
HUBPDO_ReturnTopologyAddress (
    __in
        PHUB_PDO_CONTEXT        PdoContext,
    __in
        PIRP                    Irp
    )
/*++

Routine Description:

    Fetches the topology address. This API is passed down the stack until it reaches
    the root hub PDO where the PCI values are set. The port fields are filled in by the
    completion.

Arguments:
    ioStackLocation->Parameters.Others.Argument1
    A pointer to a structure to receive the topology address.


Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    PIO_STACK_LOCATION      ioStack;
    PUSB_TOPOLOGY_ADDRESS   address;
    UCHAR                   hubDepth;

    TRY {

        ioStack = IoGetCurrentIrpStackLocation(Irp);
        address = (PUSB_TOPOLOGY_ADDRESS)ioStack->Parameters.Others.Argument1;

        if (address == NULL) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS since Arg1 is NULL");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        *address = PdoContext->HubFdoContext->HubParentInterface.HubTopologyAddress;

        hubDepth = PdoContext->HubFdoContext->HubParentInterface.HubDepth;

        if (hubDepth == 0) {
            address->RootHubPortNumber = (USHORT) PdoContext->PortNumber;
        } else {
            address->HubPortNumber[hubDepth-1] = (USHORT) PdoContext->PortNumber;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;

}


NTSTATUS
HUBPDO_GetHubName (
    __in
        PHUB_PDO_CONTEXT        PdoContext,
    __in
        PIRP                    Irp
    )
/*++

Routine Description:
    This API returns the symbolic link name for the PDO if the PDO is for a usb
    hub. Otherwise a null string is returned.

    Note: Devices don't get a symbolic link until they are 'started'

    This API returns the UNICODE symbolic name for the PDO if the PDO is for a
    usbhub, otherwise a NULL string is returned.  The symbolic name can be used
    to retrieve additional information about the hub through user mode ioctl apis
    and WMI calls.

    Parameters:
    ioStackLocation->Parameters.DeviceIoControl.OutputBufferLength
    Length of buffer passed bytes.

    Irp->AssociatedIrp.SystemBuffer
    A pointer to a structure (USB_HUB_NAME) to receive the symbolic name.

    ActualLength - The structure size in bytes necessary to hold the NULL
    terminated symbolic link name.  This includes the entire structure, not
    just the name.

    HubName - The UNICODE NULL terminated symbolic link name of the external
    hub attached to the port.  If there is no external hub attached to the port a
    single NULL is returned.

    typedef struct _USB_HUB_NAME {
        ULONG ActualLength;
        WCHAR HubName[1];
    } USB_HUB_NAME, *PUSB_HUB_NAME;

    The symbolic name is returned only if the PDO is for a USB hub.

    assuming the string is \n\name strip of '\n\' where
    n is zero or more chars

Return Value:

    NTSTATUS

--*/
{

    NTSTATUS            status;
    PIO_STACK_LOCATION  ioStack;
    PUSB_HUB_NAME       userBuffer;
    ULONG               userBufferLength;
    UNICODE_STRING      symbolicLinkNameUnicode;
    size_t              symbolicLinkNameLength;
    PWCHAR              currentCharacter;

    UNREFERENCED_PARAMETER(PdoContext);

    TRY {

        ioStack = IoGetCurrentIrpStackLocation(Irp);

        userBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
        userBuffer = (PUSB_HUB_NAME) Irp->AssociatedIrp.SystemBuffer;

        if (userBuffer == NULL) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_HUB_NAME since SystemBuffer is NULL");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (userBufferLength < sizeof(struct _USB_HUB_NAME)) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_HUB_NAME since OutputBufferLength (%d) is less than expected (%d)",
                       userBufferLength,
                       sizeof(struct _USB_HUB_NAME));

            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        if (PdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 0) {
            userBuffer->ActualLength = sizeof(struct _USB_HUB_NAME);
            userBuffer->HubName[0] = UNICODE_NULL;
            Irp->IoStatus.Information = sizeof(struct _USB_HUB_NAME);
            status = STATUS_SUCCESS;
            LEAVE;
        }

        RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);
        RtlZeroMemory(userBuffer, userBufferLength);

        HUBPDO_GetHubSymbolicLinkName(PdoContext,
                                      &symbolicLinkNameUnicode);

        currentCharacter = HUBMISC_StripSymbolicNamePrefix(&symbolicLinkNameUnicode,
                                                           &symbolicLinkNameLength);

        if ((currentCharacter != NULL) &&
            (userBufferLength >= (ULONG)(symbolicLinkNameLength + sizeof(struct _USB_HUB_NAME)))) {

                RtlCopyMemory(userBuffer->HubName,
                              currentCharacter,
                              symbolicLinkNameLength);
        }

        userBuffer->ActualLength = (ULONG)(symbolicLinkNameLength + sizeof(struct _USB_HUB_NAME));
        Irp->IoStatus.Information = userBuffer->ActualLength;
        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;

}

NTSTATUS
HUBPDO_RecordFailure (
    __in
        PHUB_PDO_CONTEXT        PdoContext,
    __in
        PIRP                    Irp
    )
/*++
Routine Description:

    See IOCTL_INTERNAL_USB_RECORD_FAILURE
    Records START_FAILURE info in the PDO for later reference.  This API is available to USB function drivers so they
    may record information about PNP start failures for later diagnosis.


Return Value:

    STATUS_SUCCESS if the data could be logged

    STATUS_INVALID_PARAMETER if the PDO already has data recorded.
    STATUS_INVALID_PARAMETER if NULL is passed in for source.

    STATUS_BUFFER_TOO_SMALL if fakdat length is shorter than sizeof(faildata);
--*/
{
    NTSTATUS            status;
    PIO_STACK_LOCATION  ioStack;
    PUSB_START_FAILDATA failData;
    ULONG               nBytes;

    TRY {
        ioStack = IoGetCurrentIrpStackLocation(Irp);

        failData = ioStack->Parameters.Others.Argument1;

        if (PdoContext->FailData != NULL) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_RECORD_FAILURE since failure already recorded for this PDO");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (failData == NULL) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_RECORD_FAILURE since Arg1 is NULL");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;

        }

        nBytes = failData->LengthInBytes;

        if (nBytes > 4096) {
            nBytes = 4096;
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "WARNING: CLIENT_COMPLIANCE: IOCTL_INTERNAL_USB_RECORD_FAILURE failure data length is greater than 4096.");
        }

        if (nBytes < sizeof(struct _USB_START_FAILDATA)) {

            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_RECORD_FAILURE since failure data length (%d) is less than expected (%d)",
                       nBytes,
                       sizeof(struct _USB_START_FAILDATA));

            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        PdoContext->FailData = ExAllocatePoolWithTag(NonPagedPool,
                                                     nBytes,
                                                     USBHUB3_DEVICE_TAG);

        if (PdoContext->FailData == NULL) {

            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_RECORD_FAILURE since attempt to allocate buffer failed");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlCopyMemory(PdoContext->FailData,
                      failData,
                      nBytes);

        PdoContext->AssociatedDSM->PortContext->ConnectionStatus = PdoContext->FailData->ConnectStatus;

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;

}


NTSTATUS
HUBPDO_ReturnDeviceConfigInfo(
    __in
        PHUB_PDO_CONTEXT        PdoContext,
    __in
        PIRP                    Irp
    )
/*
Routine Description:

    See IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO

Return Value:

    NTSTATUS

*/
{
    NTSTATUS                    status;
    PHUB_INTERNAL_CONFIG_INFO   internalConfigInfo;
    PIO_STACK_LOCATION          ioStackLocation;
    BOOLEAN                     freeStringsOnFailure;

    TRY {

        freeStringsOnFailure = FALSE;

        ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
        internalConfigInfo = ioStackLocation->Parameters.Others.Argument1;

        if (!internalConfigInfo){
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO since Arg1 is NULL");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (internalConfigInfo->Version != 1){
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO since Version is not 1");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (internalConfigInfo->Length != sizeof(HUB_DEVICE_CONFIG_INFO)){
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO since config info data length (%d) is different than expected (%d)",
                       internalConfigInfo->Length,
                       sizeof(HUB_DEVICE_CONFIG_INFO));

            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        RtlZeroMemory(internalConfigInfo, sizeof(HUB_DEVICE_CONFIG_INFO));

        freeStringsOnFailure = TRUE;

        internalConfigInfo->Version = 1;
        internalConfigInfo->Length = sizeof(HUB_DEVICE_CONFIG_INFO);

        internalConfigInfo->HubFlags.ul = 0;

        if (PdoContext->HubFdoContext->HubInfoFromParent.HubFlags.HubIsHighSpeedCapable == 1) {
            internalConfigInfo->HubFlags.HubIsHighSpeedCapable = 1;
        }

        if (PdoContext->HubFdoContext->HubInfoFromParent.DeviceDescriptor.bcdUSB >= 0x200) {
            internalConfigInfo->HubFlags.HubIsHighSpeedCapable = 1;
        }

        if (PdoContext->HubFdoContext->HubInformation.HubFlags.IsMultiTtHub) {
            internalConfigInfo->HubFlags.HubIsMultiTt = 1;
        }

        if (PdoContext->HubFdoContext->HubInformation.HubFlags.IsMultiTtHub == 1) {
            internalConfigInfo->HubFlags.HubIsMultiTtCapable = 1;
        }

        if (PdoContext->HubFdoContext->HubParentInterface.HubSpeed == UsbHighSpeed) {
            internalConfigInfo->HubFlags.HubIsHighSpeed = 1;
        }

        if (PdoContext->HubFdoContext->HubInformation.HubFlags.WakeOnConnect == 1) {
            internalConfigInfo->HubFlags.HubIsArmedWakeOnConnect = 1;
        }

        if (PdoContext->HubFdoContext->HubParentInterface.HubDepth == 0) {
            internalConfigInfo->HubFlags.HubIsRoot = 1;
        }

        if (PdoContext->HubFdoContext->HubInformation.MaxPortPower == MaxPortPower100Milliamps) {
            internalConfigInfo->HubFlags.HubIsBusPowered = 1;
        }

        if(PdoContext->AssociatedDSM->DeviceFlags.DeviceReservedUXD == 1) {

            //
            // Device is currently allocated for a virtual machine
            //
            internalConfigInfo->Flags.UxdEnabled = 1;

            internalConfigInfo->UxdSettings = PdoContext->AssociatedDSM->UxdSettings;
        }

        //
        // Copy over the PNP strings
        //

        status = HUBID_BuildCompatibleID(PdoContext->AssociatedDSM,
                                         NULL,
                                         &internalConfigInfo->CompatibleIds);

        if (!NT_SUCCESS(status)){
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO since HUBID_BuildCompatibleID failed %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBID_BuildHardwareID(PdoContext->AssociatedDSM,
                                       NULL,
                                       &internalConfigInfo->HardwareIds);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO since HUBID_BuildHardwareID failed %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBID_CopyIDString(&internalConfigInfo->DeviceDescription,
                                    &PdoContext->AssociatedDSM->FriendlyNameIdString);

        if(!NT_SUCCESS(status)){
            LEAVE;
        }

    } FINALLY {
        if (!NT_SUCCESS(status) &&
            (freeStringsOnFailure == TRUE)) {

            //
            // Note:  These functions check for the NULL, so
            // no need to do that here
            //
            HUBID_FreeID(&internalConfigInfo->CompatibleIds);
            HUBID_FreeID(&internalConfigInfo->HardwareIds);
            HUBID_FreeID(&internalConfigInfo->DeviceDescription);

        }

    }

    return status;

}


VOID
HUBPDO_ValidateBuffer(
    __in_bcount(BufferSize)
        PUCHAR      Buffer,
    __in
        size_t      BufferSize)
/*++

Routine Description:

    Test if a buffer is valid by touching the beginning and ending bytes

Arguments:

--*/
{
    volatile UCHAR       firstByte;
    volatile UCHAR       lastByte;

#pragma prefast(disable:28931, "Turn off Unused Variable Assignment warning")
    firstByte = *Buffer;
    lastByte = *(Buffer+BufferSize-1);
#pragma prefast(enable:28931, "Turn on Unused Variable Assignment warning again")
}


__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
HUBPDO_ValidateConfigurationDescriptor(
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __in
        PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor,
    __in
        ULONG                           LengthInBytes,
    __in
        BOOLEAN                         ExtendedValidation
    )
/*++

Routine Description:

    Validate a configuration descriptor

Arguments:

    ExtendedValidation - indicates whether this function
        should fail if the descriptor fails strict validation


Return Value:

    USBD_STATUS

--*/
{
    USBD_STATUS             usbdStatus;
    VALIDATION_DEVICE_INFO  devInfo;

    UNREFERENCED_PARAMETER(ExtendedValidation);

    TRY {

        usbdStatus = USBD_STATUS_SUCCESS;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (ConfigurationDescriptor->bDescriptorType !=
            USB_CONFIGURATION_DESCRIPTOR_TYPE) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Failing Configuration Descriptor Validation since ConfigDesc->bDescriptorType (%!USBDESCRIPTOR!) is not USB_CONFIGURATION_DESCRIPTOR_TYPE",
                       ConfigurationDescriptor->bDescriptorType);

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            LEAVE;
        }

        //
        // USB 1.1, Section 9.5 Descriptors:
        //
        // If a descriptor returns with a value in its length field that is
        // less than defined by this specification, the descriptor is invalid and
        // should be rejected by the host.  If the descriptor returns with a
        // value in its length field that is greater than defined by this
        // specification, the extra bytes are ignored by the host, but the next
        // descriptor is located using the length returned rather than the length
        // expected.
        //
        if (ConfigurationDescriptor->bLength <
            sizeof(struct _USB_CONFIGURATION_DESCRIPTOR)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Failing Configuration Descriptor Validation since ConfigDesc->bLength (%d) is different than expected (%d)",
                       ConfigurationDescriptor->bLength,
                       sizeof(struct _USB_CONFIGURATION_DESCRIPTOR));

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            LEAVE;
        }

        //
        // This code came from USBPORT
        //
        if (ConfigurationDescriptor->wTotalLength == 0) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Failing Configuration Descriptor Validation since ConfigDesc->wTotalLength is set to zero");

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            LEAVE;
        }
        //
        // Touch the first and last byte of the Configuration Descriptor
        // buffer.  This should fault if the buffer is invalid.
        //
        HUBPDO_ValidateBuffer((PUCHAR)ConfigurationDescriptor,
                              ConfigurationDescriptor->wTotalLength);

        //
        // Go ahead and perform strict validation, but we may not actually
        // fail the URB if this fails
        //

        if (FALSE == HUBDESC_ValidateConfigurationDescriptorSet(ConfigurationDescriptor,
                                                                LengthInBytes,
                                                                &devInfo,
                                                                DeviceContext->PortContext->IfrLog,
                                                                NULL)) {

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
        }


    } FINALLY {

    }

    return usbdStatus;

}


DSM_EVENT
HUBPDO_ValidateSelectConfigUrb(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine validates a SelectConfig URB to determine if we should fail
    it immediately. If it fails, then it stores the ntstatus and Usbdstatus
    that should be used by DSM to fail the client request

    The validation should mimic that done in the USB 2.0 stack's
    USBPORT_ProcessURB function

Return Value:

    DSM_EVENT - Returns DsmEventOperationSuccess or DsmEventOperationFailure

--*/
{
    NTSTATUS                        status;
    USBD_STATUS                     usbdStatus;
    PUSB_CONFIGURATION_DESCRIPTOR   configDesc;
    PUCHAR                          currentByte;
    PUCHAR                          end;
    ULONG                           numInterfaces;
    PUSBD_INTERFACE_INFORMATION     interfaceInformation;
    WDF_REQUEST_PARAMETERS          parameters;
    PURB                            urb;
    DSM_EVENT                       result;
    PHUB_PDO_CONTEXT                pdoContext;
    ULONG                           maxSsPower;

    TRY {

        pdoContext = DeviceContext->PdoContext;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);

        WdfRequestGetParameters(DeviceContext->CurrentClientRequest,
                                &parameters);

        NT_ASSERT(parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB);

        urb = parameters.Parameters.Others.Arg1;

        configDesc = urb->UrbSelectConfiguration.ConfigurationDescriptor;
        //
        // If the ConfigurationDescriptor is NULL, then this is a deconfigure
        // request
        //
        if (configDesc == NULL) {
            status = STATUS_SUCCESS;
            usbdStatus = USBD_STATUS_SUCCESS;
            pdoContext->LastPowerRequested = 0;
            LEAVE;
        }

        //
        // Lets make sure the descriptor is well formed
        //
        usbdStatus = HUBPDO_ValidateConfigurationDescriptor(DeviceContext,
                                                            configDesc,
                                                           (ULONG)configDesc->wTotalLength,
                                                           FALSE);

        if (usbdStatus != USBD_STATUS_SUCCESS) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing SelectConfig URB due to config descriptor validation failure");

            status = USBD_STATUS_To_NTSTATUS(usbdStatus);
            LEAVE;
        }

        //
        // Lets see if we have enough power available for this configuration
        //
        pdoContext->LastPowerRequested = ((LONG)configDesc->MaxPower)*2;

        if ((pdoContext->HubFdoContext->HubParentInterface.HubDepth != 0) &&
            (pdoContext->LastPowerRequested > (ULONG)pdoContext->HubFdoContext->HubInformation.MaxPortPower)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing SelectConfig URB due to insufficient power available on the port for this configuration");

            status = STATUS_INSUFFICIENT_RESOURCES;
            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;

            DeviceContext->PortContext->ConnectionStatus = DeviceNotEnoughPower;

            WMI_FireNotification(DeviceContext->HubFdoContext,
                                 pdoContext->PortNumber,
                                 InsufficentPower);
            LEAVE;
        }

        //
        // There is a bug in this path. For 3.0 devies, instead of interpreting
        // the power data based on the 3.0 spec, we are using the 2.0 calculations
        // and values. In 3.0 spec, the values in the config descriptor is in
        // units of 8 milliamps as opposed to 2 milliamps. Also, the 3.0 increases
        // the limits to 150 and 900. Fixing this bug has the potential to create a
        // compat issue for 3.0 devices. So all we can do is put a log to call out
        // if the driver is asking for too much power.
        //
        if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
            maxSsPower = (pdoContext->HubFdoContext->HubInformation.MaxPortPower == MaxPortPower100Milliamps) ?
                150:900;

            if (((ULONG)(configDesc->MaxPower)*8) > maxSsPower) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Configuration requires more power than is available on the port. Ignoring error for compat");

                USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierSetConfigTooMuchPowerRequired);


            }
        }

        //
        // Validate that the number of interfaces in the urb matches the
        // configuration descriptor provided
        // NOTE: This code came from USBPORT
        //
        currentByte = (PUCHAR) &urb->UrbSelectConfiguration.Interface;
        numInterfaces = 0;
        end = ((PUCHAR) urb) + urb->UrbSelectConfiguration.Hdr.Length;

#define MIN_INTERFACE_INFO_SIZE     \
           (sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION))

        //
        // Count the number of interfaces
        //
        while (((currentByte + sizeof(USHORT)) < end) &&
               (numInterfaces < configDesc->bNumInterfaces)) {

            interfaceInformation = (PUSBD_INTERFACE_INFORMATION) currentByte;

            if((interfaceInformation->Length >= MIN_INTERFACE_INFO_SIZE) &&
               (currentByte + interfaceInformation->Length <= end)) {

                numInterfaces++;
                currentByte += interfaceInformation->Length;

            } else {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing SelectConfig URB due to invalid interface information length (%d)",
                           interfaceInformation->Length);

                status = STATUS_UNSUCCESSFUL;
                usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
                LEAVE;

            }

        }

        //
        // Verify config descriptor has at least 1 interface
        //
        if (configDesc->bNumInterfaces == 0) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing SelectConfig URB due bNumInterfaces being set to zero");

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            status = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR);

            LEAVE;
        }


        //
        // Now see if the number of interfaces agree
        //
        if (numInterfaces != configDesc->bNumInterfaces ||
            numInterfaces == 0) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing SelectConfig URB due to inconsistency between the bNumInterfaces (%d) and the number of interface information entries (%d)",
                       configDesc->bNumInterfaces,
                       numInterfaces);

            usbdStatus = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            status = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR);

            LEAVE;

        }

        status = STATUS_SUCCESS;
        usbdStatus = USBD_STATUS_SUCCESS;

    } FINALLY {

        if (NT_SUCCESS(status)) {
            result = DsmEventOperationSuccess;
        } else {
            result = DsmEventOperationFailure;
            DeviceContext->LastOperationNtStatus = status;
            DeviceContext->LastOperationUsbdStatus = usbdStatus;
        }

    }

    return result;

}

__drv_maxIRQL(DISPATCH_LEVEL)
PUSB_INTERFACE_DESCRIPTOR
HUBPDO_InternalParseConfigurationDescriptor(
    __in
        PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor,
    __in
        UCHAR                           InterfaceNumber,
    __in
        UCHAR                           AlternateSetting,
    __out_opt
        PBOOLEAN                        HasAlternateSettings
    )
/*++

Routine Description:

    Returns the Interface Descriptor within the Configuration Descriptor
    buffer parameter which matches the InterfaceNumber and
    AlternateSetting parameters.

Arguments:

    ConfigurationDescriptor - Configuration Descriptor buffer which is
    searched for the target Interface Descriptor.  At a minimum it is
    required that this Configuration Descriptor buffer is readable for
    at least wTotalLength bytes.

    InterfaceNumber - bInterfaceNumber of target Interface Descriptor.

    AlternateSetting - bAlternateSetting of target Interface Descriptor.

    HasAlternateSettings - (optional) Set to TRUE if more than one
    Interface Descriptor is found with a bInterfaceNumber which matches
    the InterfaceNumber arg.

Return Value:

    Pointer to matching target Interface Descriptor contained within
    ConfigurationDescriptor arg if succesfully found, else NULL.

--*/
{
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptorSetting;
    PUSB_COMMON_DESCRIPTOR      commonDescriptor;
    PUCHAR                      end;
    ULONG                       altSettingCount;
    ULONG                       endpointCount;

    TRY {

        //
        // Default return value is NULL unless matching target Interface
        // Descriptor is succesfully found.
        //
        interfaceDescriptorSetting = NULL;

        //
        // Default is no alternate settings unless more than one Interface
        // Descriptor is found with a bInterfaceNumber which matches the
        // InterfaceNumber arg.
        //
        if (HasAlternateSettings) {
            *HasAlternateSettings = FALSE;
        }

        altSettingCount = 0;

        endpointCount = 0;

        //
        // Minimum validation of ConfigurationDescriptor arg
        //
        if (ConfigurationDescriptor->bLength <
            sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            //
            // Client passed in bad Configuaration Descriptor
            //
            LEAVE;
        }

        if (ConfigurationDescriptor->bDescriptorType !=
            USB_CONFIGURATION_DESCRIPTOR_TYPE) {

            //
            // Client passed in bad Configuaration Descriptor
            //
            LEAVE;
        }

        if (ConfigurationDescriptor->wTotalLength <
            sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

            //
            // Client passed in bad Configuaration Descriptor
            //
            LEAVE;
        }

        //
        // Assume that the entire Configuration Descriptor buffer is
        // readable for wTotalLength bytes.  Do not attempt to read past
        // that point.
        //
        end = (PUCHAR)ConfigurationDescriptor +
              ConfigurationDescriptor->wTotalLength;

        commonDescriptor = (PUSB_COMMON_DESCRIPTOR)ConfigurationDescriptor;

        WHILE (TRUE) {
            //
            // Advance to the next descriptor.
            //
            commonDescriptor = (PUSB_COMMON_DESCRIPTOR)(((PUCHAR) commonDescriptor) + commonDescriptor->bLength);

            //
            // Make sure at least bLength and bDescriptorType can be read.
            //
            if ((PUCHAR)commonDescriptor + sizeof(USB_COMMON_DESCRIPTOR) > end) {
                break;
            }

            //
            // Make sure all of the current descriptor can be read.
            //
            if ((PUCHAR)commonDescriptor + commonDescriptor->bLength > end) {
                break;
            }

            //
            // Make sure we don't loop forever.
            //
            if (commonDescriptor->bLength == 0) {
                //
                // Client passed in bad Configuaration Descriptor
                //
                break;
            }

            //
            // Skip to the next descriptor if this descriptor is not an
            // Interface Descriptor or an Endpoint Descriptor.
            //
            if (commonDescriptor->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

                //
                // Skip to the next descriptor if this Interface Descriptor
                // does not have a valid bLength.
                //
                if (commonDescriptor->bLength < sizeof(USB_INTERFACE_DESCRIPTOR)) {
                    //
                    // Client passed in bad Configuaration Descriptor
                    //
                    continue;
                }

                //
                // Found a valid Interface Descriptor.
                //
                interfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)commonDescriptor;

                if (interfaceDescriptor->bInterfaceNumber == InterfaceNumber) {
                    //
                    // Found an Interface Descriptor with a bInterfaceNumber
                    // which matches the InterfaceNumber arg.
                    //
                    // Have at least one setting for this bInterfaceNumber.
                    //
                    altSettingCount++;

                    if (interfaceDescriptorSetting) {
                        //
                        // Stop looking at descriptors if another Interface
                        // Descriptor with a bInterfaceNumber which matches
                        // the InterfaceNumber arg was found after the
                        // target Interface Descriptor was found.  At this
                        // point the return value for HasAlternateSettings
                        // is known and all of the Endpoint Descriptors for
                        // the target Interface Descriptor should have been
                        // found.
                        //
                        break;
                    }

                    if (interfaceDescriptor->bAlternateSetting == AlternateSetting) {
                        //
                        // Found an Interface Descriptor with a bInterfaceNumber
                        // which matches the InterfaceNumber arg plus a
                        // bAlternateSetting which matches the AlternateSetting
                        // arg.  This is Interface Descriptor which will be
                        // returned.
                        //
                        interfaceDescriptorSetting = interfaceDescriptor;
                    }

                } else if (altSettingCount) {
                    //
                    // Stop looking at descriptors if an Interface
                    // Descriptor with a bInterfaceNumber which matches the
                    // InterfaceNumber arg was found and we have now
                    // advanced to an Interface Descriptors which no longer
                    // matches the InterfaceNumber arg.
                    //
                    break;
                }

            } else if (commonDescriptor->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE) {

                if (interfaceDescriptorSetting) {
                    //
                    // Found an Endpoint Descriptor belonging to the target
                    // Interface Descriptor.  Make sure it looks valid.
                    //
                    if (commonDescriptor->bLength < sizeof(USB_ENDPOINT_DESCRIPTOR)) {
                        //
                        // Client passed in bad Configuaration Descriptor.
                        // This target Interface Descriptor cannot be used
                        // if it contains invalid Endpoint Descriptors.
                        interfaceDescriptorSetting = NULL;
                        break;
                    }

                    //
                    // Found one more valid Endpoint Descriptor belonging to
                    // the target Interface Descriptor.
                    //
                    endpointCount++;
                }
            }
        }

        if (interfaceDescriptorSetting &&
            interfaceDescriptorSetting->bNumEndpoints > endpointCount) {
            //
            // Client passed in bad Configuaration Descriptor.
            // This target Interface Descriptor cannot be used if it does
            // not contain enough Endpoint Descriptors.
            //
            interfaceDescriptorSetting = NULL;
        }

        if (altSettingCount > 1 && HasAlternateSettings) {
            *HasAlternateSettings = TRUE;
        }

    } FINALLY {

    }

    return interfaceDescriptorSetting;
}


__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
HUBPDO_InitializeInterfaceInformation(
    __in
        PHUB_PDO_CONTEXT            PdoContext,
    __in
        PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    __in
        PDEVICE_CONFIGURATION       ConfigHandle
    )
/*++

Routine Description:

    This routine validates and initializes the Interface Information
    structure passed in by the client.


Return Value:

    USBD_STATUS

--*/
{

    USBD_STATUS                 usbdStatus;
    ULONG                       need;
    ULONG                       i;
    ULONG                       numEndpoints;
    BOOLEAN                     hasAlternateSettings;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;

    UNREFERENCED_PARAMETER(PdoContext);

    TRY {

        need = 0;
        interfaceDescriptor = HUBPDO_InternalParseConfigurationDescriptor((PUSB_CONFIGURATION_DESCRIPTOR)
                                                                             ConfigHandle->ConfigurationDescriptor,
                                                                          InterfaceInformation->InterfaceNumber,
                                                                          InterfaceInformation->AlternateSetting,
                                                                          &hasAlternateSettings);

        if (interfaceDescriptor == NULL) {

            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to find requested interface");

            usbdStatus = USBD_STATUS_INTERFACE_NOT_FOUND;
            LEAVE;
        }

        //
        // Here is where we verify there is enough room in the client
        // buffer since we know how many pipes we'll need based on the
        // interface descriptor.
        //
        // we need space for pipe_info for each endpoint plus the
        // interface_info
        //


        numEndpoints = interfaceDescriptor->bNumEndpoints;
        //
        // backward compat
        //
        need = (sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION))
                    + (numEndpoints * sizeof(USBD_PIPE_INFORMATION));

        if (InterfaceInformation->Length < need) {
            //
            // the client has indicated that the buffer
            // is smaller than what we need
            //
            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing due to InterfaceInformation->Length (%d) being smaller than what is needed (%d)",
                       InterfaceInformation->Length,
                       need);

            usbdStatus = USBD_STATUS_BUFFER_TOO_SMALL;
            LEAVE;

        }

        usbdStatus = USBD_STATUS_SUCCESS;
        //
        // initialize all fields not set by caller to zero
        //
        InterfaceInformation->Class = 0;
        InterfaceInformation->SubClass = 0;
        InterfaceInformation->Protocol = 0;
        InterfaceInformation->Reserved = 0;
        InterfaceInformation->InterfaceHandle = NULL;
        InterfaceInformation->NumberOfPipes = numEndpoints;

        for (i = 0; i < numEndpoints; i++) {

            InterfaceInformation->Pipes[i].EndpointAddress = 0;
            InterfaceInformation->Pipes[i].Interval = 0;
            InterfaceInformation->Pipes[i].PipeType = 0;
            InterfaceInformation->Pipes[i].PipeHandle = NULL;

            //
            // attempt to detect bad flags
            // if any unused bits are set we assume that the pipeflags
            // field is uninitialized.
            //
            if (InterfaceInformation->Pipes[i].PipeFlags & ~USBD_PF_VALID_MASK){

                //
                // client driver is passing bad flags
                //
                DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing due to invalid pipe flags (0x%x)",
                           InterfaceInformation->Pipes[i].PipeFlags);

                usbdStatus = USBD_STATUS_INAVLID_PIPE_FLAGS;

            }

            //
            // note: if USBD_PF_CHANGE_MAX_PACKET is set then
            // maxpacket size is passed in as a parameter so
            // we don't initialize it
            //

            if (!TEST_FLAG(InterfaceInformation->Pipes[i].PipeFlags,
                           USBD_PF_CHANGE_MAX_PACKET)) {

                InterfaceInformation->Pipes[i].MaximumPacketSize = 0;

            }

        }

    } FINALLY {

        //
        // set length to the correct value regardless
        // of error
        //
        InterfaceInformation->Length = (USHORT)need;

    }

    return usbdStatus;

}


DSM_EVENT
HUBPDO_ValidateSelectInterfaceUrb(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine validates a SelectInterface URB to determine if we should fail
    it immediately. If it fails, then it stores the ntstatus and Usbdstatus
    that should be used by DSM to fail the client request

    The validation should mimic that done in the USB 2.0 stack's
    USBPORT_ProcessURB function

Return Value:

    DSM_EVENT - Returns DsmEventOperationSuccess or DsmEventOperationFailure

--*/
{
    PDEVICE_CONFIGURATION           configHandle;
    PUSBD_INTERFACE_INFORMATION     interfaceInfo;
    ULONG                           length;
    USBD_STATUS                     usbdStatus;
    WDF_REQUEST_PARAMETERS          parameters;
    PURB                            urb;
    DSM_EVENT                       result;
    PHUB_PDO_CONTEXT                pdoContext;



    TRY {

        pdoContext = DeviceContext->PdoContext;


        WDF_REQUEST_PARAMETERS_INIT(&parameters);

        WdfRequestGetParameters(DeviceContext->CurrentClientRequest,
                                &parameters);

        NT_ASSERT(parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB);

        urb = parameters.Parameters.Others.Arg1;

        //
        // We need to do this for KMDF Usbser to workaround a bug in a buggy filter driver sitting
        // below KMDF. On recieving a SelectConfig URB, the filter creates a copy of a selectconfig URB
        // and sends to USB Core. On the completion path, it doesn't copy the ConfigurationHandle from
        // the copy to the original URB. Since we have the ConfigurationHandle saved in the DeviceHandle
        // we will use it. This is perfectly safe since USBPORT_SelectConfiguration keeps this copy updated
        //

        if (urb->UrbSelectInterface.ConfigurationHandle == NULL &&
            pdoContext->USBDClientContractVersion == USBD_CLIENT_CONTRACT_VERSION_INVALID) {

            NT_ASSERTMSG("Client passed NULL urb->UrbSelectInterface.ConfigurationHandle", FALSE);

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                TL_WARNING,
                Device,
                "Client passed NULL urb->UrbSelectInterface.ConfigurationHandle"
                );

            urb->UrbSelectInterface.ConfigurationHandle = DeviceContext->CurrentDeviceConfiguration;
            configHandle = urb->UrbSelectInterface.ConfigurationHandle;
        } else {

            configHandle = urb->UrbSelectInterface.ConfigurationHandle;

            if (configHandle != DeviceContext->CurrentDeviceConfiguration) {
                usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

        interfaceInfo = &urb->UrbSelectInterface.Interface;

        //
        // Initialize the handle to something invalid
        //
        interfaceInfo->InterfaceHandle = USBPORT_BAD_HANDLE;

        //
        // Validate the Length field in the Urb header, we can
        // figure out the correct value based on the interface
        // information passed in.
        //
        length = interfaceInfo->Length + sizeof(struct _URB_HEADER)
                 + sizeof(PDEVICE_CONFIGURATION);

        if (length != urb->UrbHeader.Length) {

            //
            // Client passe in bogus total length
            //
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Client failed to set urb->UrbHeader.Length (%d) in a SelectInterface URB to required value (%d)",
                       urb->UrbHeader.Length,
                       length);

            //
            // Generally clients mess up the header length so
            // we will override with the length we calculated
            // from the interface information
            //
            // Win9Consider: Verify for new clients
            //
            urb->UrbHeader.Length = (USHORT)length;
        }

        usbdStatus = HUBPDO_InitializeInterfaceInformation(pdoContext,
                                                           interfaceInfo,
                                                           configHandle);

        if (usbdStatus != USBD_STATUS_SUCCESS) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing SelectInterface urb due to failure in HUBPDO_InitializeInterfaceInformation");
            LEAVE;
        }


    } FINALLY {

        if (usbdStatus == USBD_STATUS_SUCCESS) {
            result = DsmEventOperationSuccess;
        } else {
            result = DsmEventOperationFailure;
            DeviceContext->LastOperationUsbdStatus = usbdStatus;
            DeviceContext->LastOperationNtStatus = USBD_STATUS_To_NTSTATUS(usbdStatus);
        }

    }

    return result;

}


DSM_EVENT
HUBPDO_FindAndStoreMatchingInternalPipeHandle(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine finds a matching handle for the current URB
    that is being processed by DSM and if found, stores it
    in the device context.

Return Value:

    DSM_EVENT - Returns DsmEventOperationSuccess or DsmEventOperationFailure

--*/
{
    WDF_REQUEST_PARAMETERS          parameters;
    PURB                            urb;
    DSM_EVENT                       result;
    PPIPE_HANDLE                    pipeHandle;

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&parameters);

        WdfRequestGetParameters(DeviceContext->CurrentClientRequest,
                                &parameters);

        NT_ASSERT(parameters.Parameters.Others.IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB);

        urb = parameters.Parameters.Others.Arg1;

        //
        // Validate the pipe handle
        //
        pipeHandle = HUBDTX_GetMatchingInternalPipeHandle(DeviceContext->CurrentDeviceConfiguration,
                                                          urb->UrbPipeRequest.PipeHandle);

        if (pipeHandle == NULL) {

            result = DsmEventOperationFailure;
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
            DeviceContext->LastOperationNtStatus =  USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_PIPE_HANDLE);
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing due to invalid PipeHandle (%p) in URB (%p)",
                       (PVOID)urb->UrbPipeRequest.PipeHandle,
                       urb);

        } else {

            DeviceContext->CurrentTargetPipe = pipeHandle;
            result = DsmEventOperationSuccess;

        }


    } FINALLY {

    }

    return result;
}



__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_CheckIfTargetPipeIsForZeroBw(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine checks if the current pipe handle stored in
    the device context is for a endpoint with zero bandwidth.
    DSM will call this function to determine how to process
    a Client request to reset a pipe. Returns Yes if it is,
    else returns No


Return Value:

    DSM_EVENT - DsmEventYes or DsmEventNo

--*/
{
    DSM_EVENT           result;

    TRY {

        NT_ASSERT(DeviceContext->CurrentTargetPipe != NULL);

        if (DeviceContext->CurrentTargetPipe->EndpointFlags.ZeroBandwidth == 1) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;

}

__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_CheckIfPdoIsStarted(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine checks if the device is in a
    started state

Return Value:

    DSM_EVENT - DsmEventYes or DsmEventNo

--*/
{
    DSM_EVENT           result;

    TRY {

        if (DeviceContext->DeviceStateFlags.DeviceStarted == 1) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;

}


__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_CheckIfTargetPipeIsIsochronous(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:

    This routine checks if the current pipe handle stored in
    the device context is for an isochronous endpoint. Returns Yes
    if it is, else returns No


Return Value:

    DSM_EVENT - DsmEventYes or DsmEventNo

--*/
{
    DSM_EVENT           result;

    TRY {

        NT_ASSERT(DeviceContext->CurrentTargetPipe != NULL);

        if ((DeviceContext->CurrentTargetPipe->EndpointDescriptor->bmAttributes
            & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
            result = DsmEventYes;
        } else {
            result = DsmEventNo;
        }

    } FINALLY {

    }

    return result;

}

__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_IsItABootDevice(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:

    DSM_EVENT - DsmEventYes or DsmEventNo

--*/
{
    return ((DeviceContext->PdoContext->PdoFlags.InBootPath) ? DsmEventYes : DsmEventNo);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_NotifyDeviceDisconnected(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{

    //
    // We also set the FailIo flag independently so that
    // we have to do only one check in IO path
    //
    INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, DeviceGone);
    DeviceContext->PdoContext->FailIo = TRUE;

    //
    // Cleanup UXD settings if the device is being removed but
    // not if it is part of cycle port because cycle port is the
    // mechanism used to enumerate the re-directed PDO
    //
    if (DeviceContext->PdoContext->PdoFlags.InBootPath == 0) {

        if (DeviceContext->PdoContext->CyclePortInProgress == 0) {
            HUBREG_DeleteUxdSettings(DeviceContext->PdoContext->AssociatedDSM,
                                     UxdEventDisconnect);
        }

    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_SetFailIo(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    DeviceContext->PdoContext->FailIo = TRUE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_ClearFailIo(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    DeviceContext->PdoContext->FailIo = FALSE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_NotifyDeviceReconnected(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    //
    // This flag should only be cleared for boot devices
    //
    DeviceContext->PdoContext->FailIo = FALSE;
    INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, DeviceGone);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_SetResetOnLastResumeFlag(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ResetOnLastResume);
    INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ReportPortAsDisabled);
}

__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_CheckIfResetOnLastResumeFlagIsSet(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    return ((DeviceContext->PdoContext->PdoFlags.ResetOnLastResume == 1) ? DsmEventYes:DsmEventNo);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_ClearResetOnLastResumeFlag(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ResetOnLastResume);
    INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ReportPortAsDisabled);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_SetDeviceLostPowerFlag(
    __in
            PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    DeviceContext->PdoContext->PdoFlags.PowerLostOnLastReset = TRUE;
}


__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
HUBPDO_ValidateURB (
    __in
        PHUB_PDO_CONTEXT    PdoContext,
    __in
        PURB                Urb
    )
/*++

Routine Description:

    This routine validates an URB to determine if we should fail it
    immediately

    The validation should mimic that done in the USB 2.0 stack's
    USBPORT_ProcessURB function

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS        status;
    USHORT          function;

    UNREFERENCED_PARAMETER(PdoContext);

    TRY {


        if (Urb->UrbHeader.Length < sizeof(struct _URB_HEADER)) {
            //
            // Unfortunately for compatibility with shipping drivers
            // variable length URBs (e.g. URB_FUNCTION_SELECT_INTERFACE)
            // cannot be blanket failed here simply because the
            // UrbHeader.Length does not appear to be valid as this has
            // never been validated in the past.
            //
            // Fixed length URBs will still be subsequently validated based
            // on the UrbDispatchTable[function].UrbRequestLength as that
            // has always been done.
            //

            EventWriteUSBHUB3_ETW_EVENT_DISPATCH_URB_INVALID_HEADER_LENGTH_WARNING(
                &PdoContext->AssociatedDSM->DsmContext.CurrentActivityId,
                &PdoContext->AssociatedDSM->UsbDevice,
                Urb,
                sizeof(struct _URB_HEADER),
                &Urb->UrbHeader);

            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Client specified a UrbHeader.Length (%d) for urb (%p) that is smaller than the minimum size (%d)",
                       Urb->UrbHeader.Length,
                       (PVOID)Urb,
                       sizeof(struct _URB_HEADER));
            //
            // Win9Consider: Fail this for new client drivers
            //
        }

        //
        // Initialize the URB status to USBD_STATUS_SUCCESS.
        // Some drivers do not initialize the URB status.
        //
        Urb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        //
        // Retrieve the URB_FUNCTION_* from the URB.
        //
        function = Urb->UrbHeader.Function;

        //
        // Initialize the UsbdFlags field of the URB.
        //
        Urb->UrbHeader.UsbdFlags = 0;

        //
        // Validate the URB Function.
        //
        if (function > URB_FUNCTION_LAST) {

            Urb->UrbHeader.Status = USBD_STATUS_INVALID_URB_FUNCTION;
            status = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_URB_FUNCTION);

            EventWriteUSBHUB3_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION(
                &PdoContext->AssociatedDSM->DsmContext.CurrentActivityId,
                &PdoContext->AssociatedDSM->UsbDevice,
                Urb,
                sizeof(struct _URB_HEADER),
                &Urb->UrbHeader);

            DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Client specified an invalid URB function (%d) in URB (%p)",
                       function,
                       Urb);

            LEAVE;
        }

        //
        // Validate the URB Length for the pipe requests
        //
        if ((function == URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL)
            || (function == URB_FUNCTION_SYNC_RESET_PIPE)
            || (function == URB_FUNCTION_SYNC_CLEAR_STALL)) {


            if (sizeof(struct _URB_PIPE_REQUEST) != Urb->UrbHeader.Length) {
                Urb->UrbHeader.Status = USBD_STATUS_INVALID_URB_FUNCTION;
                status = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_PARAMETER);
                DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Client specified an UrbHeader.Length (%d) in URB (%p) that is different than expected (%d)",
                           Urb->UrbHeader.Length,
                           (PVOID)Urb,
                           sizeof(struct _URB_PIPE_REQUEST));

                EventWriteUSBHUB3_ETW_EVENT_DISPATCH_URB_INVALID_HEADER_LENGTH_WARNING(
                    &PdoContext->AssociatedDSM->DsmContext.CurrentActivityId,
                    &PdoContext->AssociatedDSM->UsbDevice,
                    Urb,
                    sizeof(struct _URB_HEADER),
                    &Urb->UrbHeader);

                LEAVE;
            }
            status = STATUS_SUCCESS;

            LEAVE;

        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;

}


IO_COMPLETION_ROUTINE HUBPDO_SyncCompletionRoutine;

_Use_decl_annotations_
NTSTATUS
HUBPDO_SyncCompletionRoutine (
    PDEVICE_OBJECT  DeviceObject,
    PIRP            Irp,
    PVOID           Context
    )
/*++

Routine Description:

    Completion routine for IOCTLs forwarded to the WDF QUEUE
    In this routine, we will update the URB status (if necessary)
    and signal the event which the dispatch routine is waiting on


Arguments:

    Context - pointer to the KEVENT that the dispatch
        routine is waiting on.

Return Value:

    NTSTATUS

--*/
{
    PKEVENT                 event;
    PURB                    urb;
    ULONG                   ioControlCode;
    PIO_STACK_LOCATION      ioStack;
    NTSTATUS                status;

    _Analysis_assume_(Context != NULL);
    UNREFERENCED_PARAMETER(DeviceObject);

    TRY {

        NT_ASSERT(Context != NULL);

        event = (PKEVENT) Context;
        ioStack = IoGetCurrentIrpStackLocation(Irp);
        ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;
        status = Irp->IoStatus.Status;

        //
        // If the IRP completed successfully, then there is nothing we need
        // to do to "patch it up"
        //
        if (NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // If this is not an URB, then there isn't anything we need
        // to do to "patch it up
        //
        if (ioControlCode != IOCTL_INTERNAL_USB_SUBMIT_URB) {
            LEAVE;
        }

        //
        // Now, we have a failed URB.  If this was completed by KMDF
        // before it was presented, we may need to update the URB status
        // to match the IRP status
        //

        urb = ioStack->Parameters.Others.Argument1;
        //
        // When we validated the urb, we initialized the status
        // to USBD_STATUS_SUCCESS.  If the IRP failed and this is
        // still the status value, then we need to update it
        // to match the IRP failure
        //
        if (urb->UrbHeader.Status == USBD_STATUS_SUCCESS) {
            urb->UrbHeader.Status = HUBPDO_GetUSBDErrorFromNTStatus(status);
        }

    } FINALLY {

        //
        // Signal the dispatch routine that we are done
        //
        KeSetEvent(event,
                   IO_NO_INCREMENT,
                   FALSE);

    }

    return STATUS_MORE_PROCESSING_REQUIRED;

}

VOID
HUBPDO_QueryForD3ColdSupportInAcpi (
    __in
        WDFDEVICE               Device
    )
/*++

Routine Description:

    Queries ACPI if D3 cold is supported for the device

Returns:

    VOID

--*/
{
    WDF_IO_TARGET_OPEN_PARAMS   openParams;
    NTSTATUS                    status;
    WDFIOTARGET                 ioTarget;
    D3COLD_SUPPORT_INTERFACE    d3ColdInterface;
    BOOLEAN                     supported;
    PDEVICE_CONTEXT             deviceContext;

    TRY {

        deviceContext = GetHubPdoContext(Device)->AssociatedDSM;

        //
        // We are opening a remote target on our own stack. This
        // is necessary as we are sending a query to ACPI who
        // sits on top of us
        //
        status = WdfIoTargetCreate(Device,
                                   WDF_NO_OBJECT_ATTRIBUTES,
                                   &ioTarget);

        if (!NT_SUCCESS (status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Could not create the IO target for querying D3 cold support: %!STATUS!",
                       status);
            ioTarget = NULL;

            LEAVE;
        }

        WDF_IO_TARGET_OPEN_PARAMS_INIT_EXISTING_DEVICE(&openParams,
                                                       WdfDeviceWdmGetDeviceObject(Device));

        status = WdfIoTargetOpen(ioTarget,
                                 &openParams);

        if (!NT_SUCCESS (status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Could not open the IO target for querying D3 cold support: %!STATUS!",
                       status);
            LEAVE;
        }

        status = WdfIoTargetQueryForInterface(ioTarget,
                                             &GUID_D3COLD_SUPPORT_INTERFACE,
                                             (PINTERFACE)&d3ColdInterface,
                                             sizeof(d3ColdInterface),
                                             D3COLD_SUPPORT_INTERFACE_VERSION,
                                             deviceContext->PdoContext);

        if (!NT_SUCCESS (status)) {

            //
            // We expect this call to fail if ACPI is not present
            //
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "WdfIoTargetQueryForInterface failed: %!STATUS! (this is expected if ACPI is not present)",
                       status);
            LEAVE;
        }

        status =  d3ColdInterface.GetD3ColdCapability(d3ColdInterface.Context,
                                                      &supported);

        if (NT_SUCCESS (status) && supported) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &deviceContext->DeviceFlags, D3ColdSupportedInAcpi);
        }

        if (d3ColdInterface.InterfaceDereference) {
            d3ColdInterface.InterfaceDereference(d3ColdInterface.Context);
        }


    } FINALLY {
        if (ioTarget) {
            WdfObjectDelete(ioTarget);
        }
    }
}



NTSTATUS
HUBPDO_EvtDeviceWdmIrpPnPPowerPreprocess(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++
Routine Description:

    Dispatch routine that makes use of extra stack location allocated for the
    preprocess callback to set a completion-routine and catch the IRP on its way
    back to fix some of the behaviour differences caused by KMDF.
    Currently this routine fixes following IRPs to be synchronous so the client
    driver doesn't get STATUS_PENDING returned to match with EHCI.

        IRP_MN_START_DEVICE
        IRP_MN_SURPRISE_REMOVAL
        IRP_MN_SET_POWER for Device Power Down

    It fixes the return status of following IRPs to be consistent with EHCI.

        IRP_MN_QUERY_RESOURCE_REQUIREMENTS
        IRP_MN_QUERY_PNP_DEVICE_STATE
        IRP_MN_DEVICE_ENUMERATED

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    NTSTATUS                originalIrpStatus;
    PDEVICE_CONTEXT         deviceContext;
    PHUB_PDO_CONTEXT        pdoContext;
    //
    // Define the enum locally as it is very specific to this function and
    // should not be used elsewhere
    //
    enum {
        IrpActionCompleteIrp,
        IrpActionDeferCompletionToWorkerItem,
        IrpActionFireAndForget
    } irpAction;


    TRY {

        irpStack = IoGetCurrentIrpStackLocation( Irp );
        originalIrpStatus = Irp->IoStatus.Status;
        irpAction = IrpActionFireAndForget;
        //
        // Ideally, it is desirable to not initialize deviceContext to NULL as
        // that helps catch errors where the code doesn't set it to a meaniningful
        // value but then tries to use it. However, due to a compiler issue, we
        // are forced to set it. We should remove it once compliler issue is fixed
        //
        deviceContext = NULL;

        switch (irpStack->MajorFunction) {
        case IRP_MJ_PNP:
            switch (irpStack->MinorFunction) {
            case IRP_MN_QUERY_INTERFACE:
                status = HUBPDO_EvtDeviceWdmIrpQueryInterfacePreprocess(Device,
                                                                        Irp);
                LEAVE;
                break;
            case IRP_MN_QUERY_DEVICE_TEXT:
                status = HUBPDO_EvtDeviceWdmIrpQueryDeviceTextPreprocess(Device,
                                                                         Irp);
                LEAVE;
                break;

            case IRP_MN_QUERY_ID:
                status = HUBPDO_EvtDeviceWdmIrpQueryIdPreprocess(Device,
                                                                 Irp);
                break;

            case IRP_MN_REMOVE_DEVICE:
                //
                // This field is only used by debugger extension. We zero it
                // out here so that it does not try to interpret the invalid
                // value
                //
                GetHubPdoContext(Device)->ChildHubFdoContext = NULL;
                break;

            case IRP_MN_DEVICE_ENUMERATED:

                NT_ANALYSIS_ASSUME(KeGetCurrentIrql() == PASSIVE_LEVEL);

                HUBPDO_DispatchWdmPnpPowerIrpSynchronously(Device, Irp);

                //
                // Query If D3 cold is supported for this device in ACPI.
                // Now is a good time to query because the function driver
                // is not present yet, so we don't have to depend on
                // function driver sending the query down correctly
                //
                HUBPDO_QueryForD3ColdSupportInAcpi(Device);

                pdoContext = GetHubPdoContext(Device);
                if (pdoContext->AssociatedDSM->BillboardInfo != NULL) {
                    HUBPDO_PublishBillboardDetails(pdoContext);
                }

                //
                // Publish the USB Dual Role Features
                //
                if (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsDualRole) {
                    HUBPDO_PublishDualRoleFeaturesProperty(pdoContext);
                }

                //
                // Change the status from STATUS_NOT_SUPPORTED to STATUS_SUCCESS
                // to be consistent with EHCI.
                //
                NT_ASSERT(Irp->IoStatus.Status == STATUS_NOT_SUPPORTED);
                Irp->IoStatus.Status = STATUS_SUCCESS;
                irpAction = IrpActionCompleteIrp;
                break;

            case IRP_MN_START_DEVICE:
                HUBPDO_DispatchWdmPnpPowerIrpSynchronously(Device, Irp);
                irpAction = IrpActionCompleteIrp;
                break;

            case IRP_MN_SURPRISE_REMOVAL:
                HUBPDO_DispatchWdmPnpPowerIrpSynchronously(Device, Irp);
                irpAction = IrpActionCompleteIrp;
                break;

            case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
                HUBPDO_DispatchWdmPnpPowerIrpSynchronously(Device, Irp);
                //
                // Framework might complete the IRP with STATUS_SUCCESS. We will change the
                // status from STATUS_SUCCESS to whatever status that was originally in the IRP
                // to be consistent with EHCI.
                //
                Irp->IoStatus.Status = originalIrpStatus;
                irpAction = IrpActionCompleteIrp;
                break;

            case IRP_MN_QUERY_PNP_DEVICE_STATE:

                NT_ANALYSIS_ASSUME(KeGetCurrentIrql() == PASSIVE_LEVEL);

                HUBPDO_DispatchWdmPnpPowerIrpSynchronously(Device, Irp);
                if (Irp->IoStatus.Information == 0) {
                    //
                    // Framework always completes the IRP with STATUS_SUCCESS even there is no
                    // state change to report. This is inconsistenet with EHCI. We will change the status
                    // from STATUS_SUCCESS to whatever status that was originally in the IRP
                    // to be consistent with EHCI
                    //
                    NT_ASSERT(Irp->IoStatus.Status == STATUS_SUCCESS);
                    Irp->IoStatus.Status = originalIrpStatus;
                } else {
                    deviceContext = GetHubPdoContext(Device)->AssociatedDSM;

                    if (deviceContext->EnumMsgId != 0) {
                        //
                        // Log the failure message Id value to the hardware key, to be picked
                        // up by SQM.
                        //
                        HUBREG_UpdateSqmEnumerationFailureCode(deviceContext);
                        HUBMISC_ReportPnPFailureProblem(Device, deviceContext->EnumMsgId);
                    }
                }

                irpAction = IrpActionCompleteIrp;
                break;

            default:
                NT_ASSERTMSG("Received a Preprocess callback for a PnP IRP that we never registered for",
                             FALSE);
                break;

            }

            break;

        case IRP_MJ_POWER:
            switch (irpStack->MinorFunction) {
            case IRP_MN_SET_POWER:

                if (irpStack->Parameters.Power.Type == DevicePowerState) {

                    //
                    // Some client drivers errorneously go from D2 to D3 when they
                    // get a Sx IRP while in selectively suspended state. KMDF
                    // treats this Dx IRP as a no-op. In order to be compatible
                    // with Win7 stack, we need to flush any wait wake and idle
                    // Irps that might be currently pending
                    //
                    deviceContext = GetHubPdoContext(Device)->AssociatedDSM;
                    if ((irpStack->Parameters.Power.State.DeviceState == PowerDeviceD3) &&
                        (deviceContext->DevicePowerState == PowerDeviceD2)) {

                        status = WdfDeviceIndicateWakeStatus(Device,
                                                             STATUS_POWER_STATE_INVALID);

                        if (!NT_SUCCESS(status)) {
                            DbgTraceTo(deviceContext->PortContext->IfrLog,
                                       TL_WARNING,
                                       Device,
                                       "WdfDeviceIndicateWakeStatus failed %!STATUS!. Failure is expected if no wake IRP was pending.",
                                       status);
                            //
                            // There might not be a wake IRP pending, so it is ok
                            // for this call to fail
                            //
                            status = STATUS_SUCCESS;
                        }
                        HUBIDLE_AddEvent(&GetHubPdoContext(Device)->IdleIrpContext,
                                         IsmEventPDOD3,
                                         NULL);
                    }

                    deviceContext->DevicePowerState = irpStack->Parameters.Power.State.DeviceState;
                    // 
                    // If some of the PnP IRPs are completed synchronously, there 
                    // is a potential deadlock that can occur due to the way WDF
                    // is designed. WDF can complete a D IRP completion from within
                    // the state entry function of its power state machine. Now if 
                    // the completion thread goes all the way to another WDF driver
                    // on the stack, that driver can then send a different PnP or 
                    // power IRP down the stack on the same thread. When this new 
                    // PnP or Power IRP reaches the PDO level and queues and event
                    // in one of the WDF state machine but since the WDF state machine
                    // entry is in progress, WDF will not make any progress until
                    // the thread unwinds and the state machine entry gets unblocked.
                    // If the hub happens to be completing that new IRP synchronously,
                    // the thread will never unwind, leading to a deadlock. 
                    // To avoid this deadlock, we need to ensure that D IRP completion
                    // gets deferred to a different thread. Now, one way of doing that 
                    // would be to complete all D IRPs synchronously 
                    //
                    // However, there is a serious issue with blocking power threads. 
                    // In particular, blocking D0 thread can lead to a worker item 
                    // starvation and a deadlock. Consider a large and deep USB tree 
                    // in S0Idle and then a leaf device trying to wake up. In this case 
                    // the leaf device will get a D0 IRP which will in turn result in 
                    // the parent sending a D0 IRP and so on. Now, if every stack
                    // is blocking the power thread, it is possible that by the time
                    // the USB controller tries to send a D0 IRP, all power threads
                    // are used up, leading to a worker item starvation and a deadlock.
                    // 
                    // Due to the above concern, we do not want to block a power IRP but
                    // stil have the completion in a different thread. So what we 
                    // are going to do is to set a completion routine but instead of 
                    // waiting for it in the dispatch routine, we will simply queue
                    // a worker item from the completion routine to complete the IRP.
                    //
                    irpAction = IrpActionDeferCompletionToWorkerItem;
                }

                break;

            default:
                NT_ASSERTMSG("Received a Preprocess callback for a Power IRP that we never registered for",
                             FALSE);
                break;
            }
            break;

        default:
            NT_ASSERTMSG("Received a Preprocess callback for a Major code that we never registered for",
                         FALSE);
            break;

        }

        switch (irpAction) {
        
        case IrpActionCompleteIrp:
            status = Irp->IoStatus.Status;
            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);
            break;

        case IrpActionDeferCompletionToWorkerItem:
            IoCopyCurrentIrpStackLocationToNext(Irp);
            IoSetCompletionRoutine(Irp, HUBPDO_WdmPnpPowerIrpCompletionRoutineForAsynchronousCompletion, 
                                   deviceContext, TRUE, TRUE, TRUE);
            status = WdfDeviceWdmDispatchPreprocessedIrp(Device,
                                                         Irp);
            break;

        case IrpActionFireAndForget:

            IoSkipCurrentIrpStackLocation(Irp);
            status = WdfDeviceWdmDispatchPreprocessedIrp(Device,
                                                         Irp);
            break;
        default: 
            NT_ASSERTMSG("IrpAction not set",
                         FALSE);           
            status = STATUS_UNSUCCESSFUL;
            break;
        }

    } FINALLY {

    }

    return status;

}


NTSTATUS
HUBPDO_EvtDeviceWdmIrpQueryInterfacePreprocess(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++
Routine Description:



Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION          irpStack;
    NTSTATUS                    status;
    PHUB_PDO_CONTEXT            pdoContext;
    PD3COLD_SUPPORT_INTERFACE   d3ColdSupportInterface;



    TRY {

        pdoContext = GetHubPdoContext(Device);

        if (pdoContext->AssociatedDSM->DeviceStateFlags.DeviceIsKnown == 0) {
            //
            // If the PDO is not in a valid state, then complete
            // the IRP without touching it. This is what the Win7
            // stack does
            //
            status = Irp->IoStatus.Status;
            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);
            LEAVE;
        }

        irpStack = IoGetCurrentIrpStackLocation( Irp );

        if (irpStack->Parameters.QueryInterface.InterfaceSpecificData == pdoContext) {
            //
            // This is us talking to us. For example, we send D3 cold query to
            // our own stack to find out if D3 cold is supported in ACPI. Just
            // complete it
            //
            status = Irp->IoStatus.Status;
            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);
            LEAVE;
        }

        if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                             &USB_BUS_INTERFACE_USBDI_GUID,
                             sizeof(GUID)) == sizeof(GUID)) {

            irpStack->Parameters.QueryInterface.InterfaceSpecificData = pdoContext->AssociatedDSM->UsbDevice;

            IoSkipCurrentIrpStackLocation(Irp);
            status = IoCallDriver(pdoContext->TargetDeviceObject,
                                  Irp);

            LEAVE;
        }

        if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                             &GUID_USBD_INTERFACE_V1,
                             sizeof(GUID)) == sizeof(GUID)) {

            irpStack->Parameters.QueryInterface.InterfaceSpecificData = pdoContext->AssociatedDSM->UsbDevice;

            PUSBD_INTERFACE_V1  usbInterfaceV1;

            if ((irpStack->Parameters.QueryInterface.Size == sizeof(USBD_INTERFACE_V1)) ||
                (irpStack->Parameters.QueryInterface.Version == USBD_INTERFACE_VERSION_602)) {

                usbInterfaceV1 = (PUSBD_INTERFACE_V1)irpStack->Parameters.QueryInterface.Interface;

                //
                // This is only sent by USBD if the USB Client driver has called USBD_CreateHandle with 
                // USBDClientContractVersion as USBD_CLIENT_CONTRACT_VERSION_602
                //
                pdoContext->USBDClientContractVersion = usbInterfaceV1->USBDClientContractVersion;
            }

            IoSkipCurrentIrpStackLocation(Irp);
            status = IoCallDriver(pdoContext->TargetDeviceObject,
                                  Irp);

            LEAVE;
        }

        if ( RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                              &GUID_HUB_CONTROLLERSTACK_INTERFACE,
                              sizeof(GUID)) == sizeof(GUID)) {

            IoSkipCurrentIrpStackLocation(Irp);
            status = IoCallDriver(pdoContext->TargetDeviceObject, Irp);

            LEAVE;
        }

        if ( RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                              &GUID_D3COLD_SUPPORT_INTERFACE,
                              sizeof(GUID)) == sizeof(GUID)) {

            if ((irpStack->Parameters.QueryInterface.Size != sizeof(D3COLD_SUPPORT_INTERFACE)) ||
                (irpStack->Parameters.QueryInterface.Version != D3COLD_SUPPORT_INTERFACE_VERSION)) {

                //
                // We don't understand this version. Leave the status alone and
                // complete the IRP
                //
                status = Irp->IoStatus.Status;
                IoCompleteRequest(Irp,
                                  IO_NO_INCREMENT);
                LEAVE;
            }


            d3ColdSupportInterface = (PD3COLD_SUPPORT_INTERFACE) irpStack->Parameters.QueryInterface.Interface;

            if (Irp->IoStatus.Status == STATUS_SUCCESS) {
                //
                // ACPI has filtered the QI. If so, we should verify that the
                // version and size filled by ACPI is what we expect
                //
                if ((d3ColdSupportInterface->Size != sizeof(D3COLD_SUPPORT_INTERFACE)) ||
                    (d3ColdSupportInterface->Version != D3COLD_SUPPORT_INTERFACE_VERSION)) {

                    status = STATUS_NOT_SUPPORTED;
                    Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
                    IoCompleteRequest(Irp,
                                      IO_NO_INCREMENT);
                    LEAVE;
                }

                //
                // Make a copy of information filled by ACPI
                //
                RtlCopyMemory(&pdoContext->D3ColdFilter,
                              d3ColdSupportInterface,
                              sizeof(D3COLD_SUPPORT_INTERFACE));
            } else {

                d3ColdSupportInterface->Size = sizeof(D3COLD_SUPPORT_INTERFACE);
                d3ColdSupportInterface->Version = D3COLD_SUPPORT_INTERFACE_VERSION;
            }

            d3ColdSupportInterface->Context = pdoContext;
            d3ColdSupportInterface->SetD3ColdSupport = HUBPDO_D3ColdSupportInterfaceSetD3ColdSupport;
            d3ColdSupportInterface->GetIdleWakeInfo = HUBPDO_D3ColdSupportInterfaceGetIdleWakeInfo;
            d3ColdSupportInterface->InterfaceDereference = HUBPDO_D3ColdSupportInterfaceDereference;
            d3ColdSupportInterface->InterfaceReference = HUBPDO_D3ColdSupportInterfaceReference;
            d3ColdSupportInterface->GetD3ColdCapability = HUBPDO_D3ColdSupportInterfaceGetD3ColdCapability;
            d3ColdSupportInterface->GetBusDriverD3ColdSupport = HUBPDO_D3ColdSupportInterfaceGetD3ColdBusDriverSupport;
            d3ColdSupportInterface->GetLastTransitionStatus = HUBPDO_D3ColdSupportInterfaceGetLastTransitionStatus;


            Irp->IoStatus.Status = STATUS_SUCCESS;
            status = STATUS_SUCCESS;
            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);
            LEAVE;
        }

        IoSkipCurrentIrpStackLocation(Irp);
        status = WdfDeviceWdmDispatchPreprocessedIrp(Device, Irp);

    } FINALLY {

    }

    return status;

}


BOOLEAN
HUBPDO_IsLanguageSupported(
    PDEVICE_CONTEXT     DeviceContext,
    LANGID              LanguageId
    )
/*++
Routine Description:


Return Value:

    NTSTATUS

--*/
{
    BOOLEAN                 supported;
    LANGID                  *supportedLanguageIds;
    ULONG                   supportedLanguagesCount;
    ULONG                   i;

    TRY {

        supported = FALSE;

        if ((DeviceContext->LanguageIdDescriptor == NULL)||
            (DeviceContext->LanguageIdDescriptor->bLength <= sizeof(USB_COMMON_DESCRIPTOR))) {
            LEAVE;
        }
        supportedLanguageIds = (LANGID *)(DeviceContext->LanguageIdDescriptor->bString);
        supportedLanguagesCount =  (DeviceContext->LanguageIdDescriptor->bLength -
                                    sizeof(USB_COMMON_DESCRIPTOR)) / sizeof(LANGID);

        for (i = 0; i < supportedLanguagesCount; i++) {
            //
            // This check is for maintaining comapt with WIn7 stack
            //
            if (supportedLanguageIds[i] == 0) {
                break;
            }

            if (supportedLanguageIds[i] == LanguageId) {
                supported = TRUE;
                break;
            }
        }

    } FINALLY {

    }

    return supported;

}

NTSTATUS
HUBPDO_EvtDeviceWdmIrpQueryDeviceTextPreprocess(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++
Routine Description:



Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;
    PDEVICE_CONTEXT         deviceContext;
    LANGID                  languageId;
    PWCHAR                  deviceDescription;
    WCHAR                   deviceDescriptionLength;
    PWCHAR                  locationInfo;
    WCHAR                   locationInfoSize;

    TRY {

        status = STATUS_NOT_SUPPORTED;

        deviceContext = GetHubPdoContext(Device)->AssociatedDSM;

        irpStack = IoGetCurrentIrpStackLocation( Irp );

        if (irpStack->Parameters.QueryDeviceText.DeviceTextType == DeviceTextLocationInformation) {
            locationInfoSize = sizeof(L"Port_#nnnn.Hub_#nnnn");
            locationInfo = ExAllocatePoolWithTag (NonPagedPool,
                                                  locationInfoSize,
                                                  USBHUB3_DEVICE_TAG);
            if (locationInfo == NULL) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Unable to allocate space for location information");

                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }
            status = RtlStringCbPrintfW(locationInfo,
                                        locationInfoSize,
                                        L"Port_#%04d.Hub_#%04d",
                                        deviceContext->PortContext->PortInformation.PortNumber,
                                        deviceContext->HubFdoContext->HubInformation.HubNumber);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "RtlStringCbPrintfW failed for location information: %!STATUS!",
                           status); 
                ExFreePoolWithTag(locationInfo, USBHUB3_DEVICE_TAG);
                LEAVE;
            }
            Irp->IoStatus.Information = (ULONG_PTR)locationInfo;
            status = STATUS_SUCCESS;
            LEAVE;

        } else if (irpStack->Parameters.QueryDeviceText.DeviceTextType != DeviceTextDescription) {
            LEAVE;
        }

        if ((deviceContext->DeviceDescriptor.iProduct == 0) ||
            (deviceContext->DeviceHackFlags.DisableSerialNumber)) {
            //
            // In Win7 USB stack, we used to Look up in the registry
            // to see if there is a generic string to use. We are
            // removing that behavior because we don't beleive it
            // is being documented or used
            //
            LEAVE;

        }

        Irp->IoStatus.Information = 0;

        languageId = LANGIDFROMLCID(irpStack->Parameters.QueryDeviceText.LocaleId);

        //
        // We are mimicking the behavior of Win7 Stack. If the LanguageId is 0
        // or if the language is not supported, we return the English descriptor
        //
        if ((languageId == 0) ||
            (HUBPDO_IsLanguageSupported(deviceContext,languageId) == FALSE)) {
            languageId = AMERICAN_ENGLISH;
        }

        if (languageId != deviceContext->CurrentLanguageId) {

            deviceContext->CurrentLanguageId = languageId;

            HUBMISC_FreeBufferIfNotNull(deviceContext->ProductIdStringDescriptor, USBHUB3_DEVICE_TAG);
            deviceContext->ProductIdStringDescriptor = NULL;

            KeClearEvent(&deviceContext->QueryDeviceTextEvent);

            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventQueryDeviceText);
            //
            // Wait for the event to signal
            //
            HUBMISC_WaitForSignal(&deviceContext->QueryDeviceTextEvent,
                                  "Query device text",
                                  Device);

        }

        if (deviceContext->ProductIdStringDescriptor == NULL) {
            if (languageId == AMERICAN_ENGLISH) {
                LEAVE;
            }
            //
            // If the language was not english and the query
            // failed then try querying for english
            //

            deviceContext->CurrentLanguageId = AMERICAN_ENGLISH;
            KeClearEvent(&deviceContext->QueryDeviceTextEvent);

            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventQueryDeviceText);
            //
            // Wait for the event to signal
            //
            HUBMISC_WaitForSignal(&deviceContext->QueryDeviceTextEvent,
                                  "Query device text",
                                  Device);

            if (deviceContext->ProductIdStringDescriptor == NULL) {
                LEAVE;
            }
        }

        //
        // Subtract the header length. Add one WCHAR for ending NULL
        //
        deviceDescriptionLength = (deviceContext->ProductIdStringDescriptor->bLength -
                                   sizeof(USB_COMMON_DESCRIPTOR))/sizeof(WCHAR) + 1;

        deviceDescription = ExAllocatePoolWithTag (NonPagedPool,
                                                   deviceDescriptionLength * sizeof(WCHAR),
                                                   USBHUB3_DEVICE_TAG);

        if (deviceDescription == NULL) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for Device Description");

            status =  STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlCopyMemory(deviceDescription,
                      deviceContext->ProductIdStringDescriptor->bString,
                      (deviceDescriptionLength - 1) * sizeof(WCHAR));

        deviceDescription[deviceDescriptionLength - 1] = 0;

        Irp->IoStatus.Information = (ULONG_PTR)deviceDescription;

        status = STATUS_SUCCESS;

    } FINALLY {


        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp,
                          IO_NO_INCREMENT);

    }

    return status;

}

NTSTATUS
HUBPDO_EvtDeviceWdmIrpQueryIdPreprocess(
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++
Routine Description:



Return Value:

    NTSTATUS

--*/
{
    PDEVICE_CONTEXT         deviceContext;
    PIO_STACK_LOCATION      irpStack;
    NTSTATUS                status;

    TRY {

        deviceContext = GetHubPdoContext(Device)->AssociatedDSM;
        irpStack = IoGetCurrentIrpStackLocation( Irp );

        //
        // DSM suspends the device if start is not received within some time
        // after the device has been enumerated. This is done to save power.
        // However, if the client driver then subsequnetly gets installed, then
        // we want the device to be ready to handle any IO that the client driver
        // might send. Note that starting the device in the PrepareHardware/D0Entry
        // is too late because some client drivers start sending IO from their
        // AddDevice routines. Therefore we have introduced this concept of a
        // "PreStart" event. This event brings back the device in a powered up state
        // ready to accept IO.
        //
        // Now the problem is that there is no official PreStart event is defined
        // in PnP. In discussions with the PnP team, it was found that whenever PnP
        // starts installing a driver or whenever it starts a recovery of an devnode,
        // it sends the Query Ids again. Therefore we have a heuristics that we treat
        // the Query Ids that come before the device has started as a trigger for
        // Pre-Starting the deivce
        //


        //
        // We are only interested in BusQueryDeviceID and we will queue an event only
        // if we are not already started. Also,If we are running at Disptach, then the
        // query is not coming from PnP manager and hence we are not interested in it
        //
        if ((deviceContext->DeviceStateFlags.DeviceStarted == 1) ||
            (irpStack->Parameters.QueryId.IdType != BusQueryDeviceID) ||
            (KeGetCurrentIrql() != PASSIVE_LEVEL)) {
            LEAVE;
        }

        //
        // First bring the hub back
        //
        status = WdfDeviceStopIdle(WdfObjectContextGetObject(deviceContext->HubFdoContext), TRUE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfDeviceStopIdle failed : %!STATUS!",
                       status);
            LEAVE;
        }

        KeClearEvent(&deviceContext->PreStartEvent);

        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventPDOPreStart);
        //
        // Wait for the event to signal
        //
        HUBMISC_WaitForSignal(&deviceContext->PreStartEvent,
                              "Pre Start Completion",
                              Device);

        WdfDeviceResumeIdle(WdfObjectContextGetObject(deviceContext->HubFdoContext));

    } FINALLY {


    }

    return STATUS_SUCCESS;

}

NTSTATUS
HUBPDO_EvtDeviceWdmIrpPreprocess (
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++

Routine Description:

    This routine determines the default dispatching for all KM internal IOCTLs.

Return Value:

    NTSTATUS

--*/
{

    PHUB_PDO_CONTEXT            pdoContext;
    PIO_STACK_LOCATION          ioStack;
    NTSTATUS                    status;
    PURB                        urb;
    ULONG                       ioControlCode;
    IOCTL_ACTION                action;
    USHORT                      function;
    KEVENT                      completionEvent;
    PULONG                      portStatus;
    PUSB_IDLE_CALLBACK_INFO     idleCallbackInfo;
    PREGISTER_COMPOSITE_DEVICE  compositeDeviceRegistration;
    GUID                        capabilityType;
    PQUERY_USB_CAPABILITY       usbCapability;
    PUSB_FORWARD_PROGRESS_INFO  forwardProgressInfo;
    ULONG                       descriptorSize;
    PVOID                       sourceDescriptor;
    BOOLEAN                     ignoreFailIo;
    PVOID                       UrbTransferBuffer;



    TRY {

        status = STATUS_SUCCESS;
        urb = NULL;
        UrbTransferBuffer = NULL;

        pdoContext = GetHubPdoContext(Device);
        ioStack = IoGetCurrentIrpStackLocation(Irp);
        ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;


        //
        // See if we should even allow any IO through. There are some IOCTLS
        // that always need to be allowed. If it is a boot device, then reset
        // IOCTL needs to be allowed. Unregistration request from USBCCGP
        // needs to be allowed.
        //
        ignoreFailIo = FALSE;
        if (ioControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {
            urb = ioStack->Parameters.Others.Argument1;
            function = urb->UrbHeader.Function;
            if (function == URB_FUNCTION_OPEN_STATIC_STREAMS ||
                function == URB_FUNCTION_CLOSE_STATIC_STREAMS) {
                ignoreFailIo = TRUE;
            }
        } else if (ioControlCode == IOCTL_INTERNAL_USB_UNREGISTER_COMPOSITE_DEVICE ||
                   ioControlCode == IOCTL_INTERNAL_USB_QUERY_USB_CAPABILITY) {
            ignoreFailIo = TRUE;
        } else if (ioControlCode == IOCTL_INTERNAL_USB_RESET_PORT_ASYNC &&
                   pdoContext->PdoFlags.InBootPath == 1) {
            ignoreFailIo = TRUE;
        }

        //
        // This IOCTL is sent by client to inform us that the device doesn't support 
        // Get Status request. Complete it here.
        //

        if (ioControlCode == IOCTL_INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE) {

            DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                TL_INFO,
                Device,
                "IOCTL_INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE Irp (0x%p) sent by class driver ", Irp);

            pdoContext->AssociatedDSM->DeviceFlags.FailGetStatusRequest = 1;
            status = STATUS_SUCCESS;
            action = IoctlActionCompleteHere;
            LEAVE;
        }


        if ((pdoContext->FailIo) && (ignoreFailIo == FALSE)) {

            DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failing Irp (0x%p) due to device not present (for surprise remove this is expected)", Irp);
            status = STATUS_NO_SUCH_DEVICE;
            action = IoctlActionCompleteHere;
            //
            // If this was a submit urb, we need to set the error code
            // in the URB as well.
            //
            if (ioControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {


                urb = ioStack->Parameters.Others.Argument1;
                //
                // This is the error returned by the original XP stack
                // if any ntstatus error was detected
                //
                urb->UrbHeader.Status = USBD_STATUS_INVALID_PARAMETER;

            }

            LEAVE;

        }

        //
        // Lets check for URBs first
        //
        if (ioControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {

            urb = ioStack->Parameters.Others.Argument1;
            function = urb->UrbHeader.Function;

            //
            // Lets get the fast ones on their way first
            //
            if ((function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER)
                || (function == URB_FUNCTION_ISOCH_TRANSFER)) {

                    urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                    status = STATUS_SUCCESS;
                    action = IoctlActionForwardToUCX;
                    LEAVE;

            }

            //
            // Now lets look at the ones we care about
            //
            switch (function) {

            case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:

                if (pdoContext->AssociatedDSM->DeviceDescriptor.bcdUSB <= 0x200) {
                    urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                    status = STATUS_SUCCESS;
                    action = IoctlActionForwardToUCX;
                    LEAVE;
                }

                //
                // For devices with bcdUsb > 2, we are going to return cached descriptors
                // if we can
                //
                status = HUBPDO_ValidateURB(pdoContext,
                                            urb);

                if (!NT_SUCCESS(status)) {
                    action = IoctlActionCompleteHere;
                    LEAVE;
                }

                if (urb->UrbControlTransfer.TransferBufferMDL != NULL) {
                    UrbTransferBuffer =
                        MmGetSystemAddressForMdlSafe(urb->UrbControlTransfer.TransferBufferMDL,
                                                     NormalPagePriority);
                } else {
                    UrbTransferBuffer = urb->UrbControlTransfer.TransferBuffer;
                }

                if (UrbTransferBuffer == NULL) {
                    status = STATUS_INVALID_PARAMETER;
                    urb->UrbHeader.Status = USBD_STATUS_INVALID_PARAMETER;
                    action = IoctlActionCompleteHere;
                    LEAVE;
                }

                switch(((struct _URB_CONTROL_DESCRIPTOR_REQUEST *)urb)->DescriptorType) {

                case USB_DEVICE_DESCRIPTOR_TYPE:
                    sourceDescriptor = &pdoContext->AssociatedDSM->DeviceDescriptor;
                    descriptorSize = min(sizeof(USB_DEVICE_DESCRIPTOR),
                                         urb->UrbControlTransfer.TransferBufferLength);
                    break;

                case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                    //
                    // We only cache the default configuration. Anything other than the
                    // default, needs to go the device
                    //
                    if (urb->UrbControlDescriptorRequest.Index > 0) {
                        urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                        status = STATUS_SUCCESS;
                        action = IoctlActionForwardToUCX;
                        LEAVE;
                    }
                    sourceDescriptor = pdoContext->AssociatedDSM->ConfigurationDescriptor;
                    descriptorSize = min(pdoContext->AssociatedDSM->ConfigurationDescriptor->wTotalLength,
                                         urb->UrbControlTransfer.TransferBufferLength);
                    break;

                default:
                    urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                    status = STATUS_SUCCESS;
                    action = IoctlActionForwardToUCX;
                    LEAVE;
                    break;
                }

                //
                // Compat steps.
                //
                urb->UrbControlTransfer.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
                if (urb->UrbControlTransfer.TransferFlags & USBD_DEFAULT_PIPE_TRANSFER) {
                    urb->UrbControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
                }

                RtlCopyMemory(UrbTransferBuffer,
                              sourceDescriptor,
                              descriptorSize);

                urb->UrbControlTransfer.TransferBufferLength = descriptorSize;
                status = STATUS_SUCCESS;
                action = IoctlActionCompleteHere;
                LEAVE;
                break;

            case URB_FUNCTION_SELECT_CONFIGURATION:
            case URB_FUNCTION_SELECT_INTERFACE:
            case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL:
            case URB_FUNCTION_SYNC_RESET_PIPE:
            case URB_FUNCTION_SYNC_CLEAR_STALL:

                status = HUBPDO_ValidateURB(pdoContext,
                                            urb);

                if (!NT_SUCCESS(status)) {
                    action = IoctlActionCompleteHere;
                    LEAVE;
                }

                if (status == STATUS_ABANDONED) {
                    //
                    // Validation was successful, but we need
                    // to complete it here anyway
                    //
                    status = STATUS_SUCCESS;
                    action = IoctlActionCompleteHere;
                    LEAVE;
                }

                if (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub) {
                    action = IoctlActionAsyncForwardToQueue;
                } else {
                    action = IoctlActionForwardToQueue;
                }
                LEAVE;
                break;

            case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR:
                if (urb->UrbControlVendorClassRequest.Index == MSOS20_DESCRIPTOR_INDEX) {

                    if (pdoContext->AssociatedDSM->MsOs20Info.Flags.DescriptorSetHeader == 0) {
                        status = STATUS_NOT_SUPPORTED;
                        action = IoctlActionCompleteHere;
                        LEAVE;
                    }

                    if (urb->UrbControlVendorClassRequest.TransferBufferMDL != NULL) {
                        UrbTransferBuffer = MmGetSystemAddressForMdlSafe(
                                            urb->UrbControlVendorClassRequest.TransferBufferMDL,
                                            NormalPagePriority);
                    } else {
                        UrbTransferBuffer = urb->UrbControlVendorClassRequest.TransferBuffer;
                    }

                    if (UrbTransferBuffer == NULL) {
                        status = STATUS_NOT_SUPPORTED;
                        action = IoctlActionCompleteHere;
                        LEAVE;
                    } else {
                        ULONG bytesTransferred;

                        bytesTransferred = min(pdoContext->AssociatedDSM->MsOs20Info.DescriptorSet->wTotalLength,
                                               urb->UrbControlVendorClassRequest.TransferBufferLength);

                        RtlCopyMemory(UrbTransferBuffer,
                                      pdoContext->AssociatedDSM->MsOs20Info.DescriptorSet,
                                      bytesTransferred);

                        urb->UrbControlVendorClassRequest.TransferBufferLength = bytesTransferred;
                        status = STATUS_SUCCESS;
                        action = IoctlActionCompleteHere;
                        LEAVE;
                    }
                }

                //
                // Only allow client MS OS descriptor requests for 2.0 or later devices 
                // (or broken devices with < 1.0 version number), that have not previously failed
                // the MS OS descriptor request, or have the DontSkipMsOsDescriptor hack flag set.
                //
                if ((((pdoContext->AssociatedDSM->DeviceDescriptor.bcdUSB >= 0x200) ||
                      (pdoContext->AssociatedDSM->DeviceDescriptor.bcdUSB < 0x100)) &&
                     (pdoContext->AssociatedDSM->DeviceFlags.MsOsDescriptorNotSupported == 0)) || 
                    (pdoContext->AssociatedDSM->DeviceHackFlags.DontSkipMsOsDescriptor == 1)) {

                    //
                    // We need to fill in the vendor code before passing it
                    // the UCX. This is for maintaining compat with the WIN7
                    // stack.
                    //
                    urb->UrbControlVendorClassRequest.Request = pdoContext->AssociatedDSM->MsOsVendorCode;

                    urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                    status = STATUS_SUCCESS;
                    action = IoctlActionForwardToUCX;

                } else {
                    DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                               TL_WARNING,
                               Device,
                               "Failing the MS OS Descriptor request by client as the device does not support it");

                    status = STATUS_NOT_SUPPORTED;
                    action = IoctlActionCompleteHere;

                }
                LEAVE;
                break;

            case URB_FUNCTION_OPEN_STATIC_STREAMS:
            case URB_FUNCTION_CLOSE_STATIC_STREAMS:
                urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                action = IoctlActionForwardToQueue;
                status = STATUS_SUCCESS;
                LEAVE;
                break;
                
            case URB_FUNCTION_GET_STATUS_FROM_DEVICE:

                //
                // Check if the client driver has informed us that the device doesn't support 
                // GET STATUS request. We should fail the URB in that case.
                //

                if (pdoContext->AssociatedDSM->DeviceFlags.FailGetStatusRequest == 1) {
                    
                    DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                               TL_INFO,
                               Device,
                               "Failing the GET STATUS request as the device does not support it");

                    status = STATUS_NOT_SUPPORTED;
                    action = IoctlActionCompleteHere;
                    LEAVE;
                }

                //
                // Otherwise fall through and send it to UCX
                //

            default:
                //
                // The reset get sent down the stack
                //
                urb->UrbHeader.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
                status = STATUS_SUCCESS;
                action = IoctlActionForwardToUCX;
                LEAVE;

            }

        }

        //
        // Now lets check for the other IOCTLs
        //

        switch (ioControlCode) {
        case IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS:

            forwardProgressInfo = (PUSB_FORWARD_PROGRESS_INFO)ioStack->Parameters.Others.Argument1;
            forwardProgressInfo->DeviceHandle = pdoContext->AssociatedDSM->UsbDevice;
            status = STATUS_SUCCESS;
            action = IoctlActionForwardToUCX;
            break;

        case IOCTL_INTERNAL_USB_SET_FUNCTION_HANDLE_DATA:
        case IOCTL_INTERNAL_USB_UNREGISTER_COMPOSITE_DEVICE:
        case IOCTL_INTERNAL_USB_REQUEST_REMOTE_WAKE_NOTIFICATION:

            ioStack->Parameters.Others.Argument2 = pdoContext->AssociatedDSM->UsbDevice;
            status = STATUS_SUCCESS;
            action = IoctlActionForwardToUCX;
            break;

        case IOCTL_INTERNAL_USB_REGISTER_COMPOSITE_DEVICE:
            ioStack->Parameters.Others.Argument2 = pdoContext->AssociatedDSM->UsbDevice;
            compositeDeviceRegistration = ioStack->Parameters.Others.Argument1;
            //
            // Save off the function suspend support of the composite driver so we can
            // determine who will be arming the device for wake
            //
            if (compositeDeviceRegistration->CapabilityFlags.CapabilityFunctionSuspend == 1) {
                INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, SupportsFunctionSuspend);
            }
            status = STATUS_SUCCESS;
            action = IoctlActionForwardToUCX;
            break;

        case IOCTL_INTERNAL_USB_QUERY_USB_CAPABILITY:

            usbCapability = (PQUERY_USB_CAPABILITY)ioStack->Parameters.Others.Argument1;
            capabilityType = usbCapability->CapabilityType;
            usbCapability->ResultLength = 0;
            //
            // This is the default action for capabilities that hub does not
            // want to interfere with
            //
            status = STATUS_SUCCESS;
            action = IoctlActionForwardToUCX;

            if (RtlCompareMemory(&capabilityType,
                                 &GUID_USB_CAPABILITY_FUNCTION_SUSPEND,
                                 sizeof(GUID)) == sizeof(GUID)) {

                if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags & DsmSubSmFlagPort20) {
                    status = STATUS_NOT_SUPPORTED;
                }
                //
                // Regardless of success or failure, we don't want it to go to UCX
                //
                action = IoctlActionCompleteHere;

            } else if (RtlCompareMemory(&capabilityType,
                                        &GUID_USB_CAPABILITY_STATIC_STREAMS,
                                        sizeof(GUID)) == sizeof(GUID)) {

                if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags & DsmSubSmFlagPort20) {
                    //
                    // If we are going to fail it, only then we complete it here
                    //
                    status = STATUS_NOT_SUPPORTED;
                    action = IoctlActionCompleteHere;
                }
            } else if (RtlCompareMemory(&capabilityType,
                                        &GUID_USB_CAPABILITY_DEVICE_CONNECTION_SUPER_SPEED_COMPATIBLE,
                                        sizeof(GUID)) == sizeof(GUID)) {

                if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
                    status = STATUS_SUCCESS;
                } else {
                    status = STATUS_NOT_SUPPORTED;
                }
                action = IoctlActionCompleteHere;

            } else if (RtlCompareMemory(&capabilityType,
                                        &GUID_USB_CAPABILITY_DEVICE_CONNECTION_HIGH_SPEED_COMPATIBLE,
                                        sizeof(GUID)) == sizeof(GUID)) {

                //
                // If the device is working in SuperSpeed or HighSpeed, it is
                // considered to be High Speed compatible
                //
                if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags &
                    (DsmSubSmFlagSpeedSuper | DsmSubSmFlagSpeedHigh)) {
                    status = STATUS_SUCCESS;
                } else {
                    status = STATUS_NOT_SUPPORTED;
                }
                action = IoctlActionCompleteHere;

            } else if (RtlCompareMemory(&capabilityType,
                                        &GUID_USB_CAPABILITY_DEVICE_CONNECTION_FULL_SPEED_COMPATIBLE,
                                        sizeof(GUID)) == sizeof(GUID)) {

                //
                // If the device is working in SuperSpeed, HighSpeed or FullSpeed
                // it is considered to be Full Speed compatible
                //
                if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags &
                    (DsmSubSmFlagSpeedSuper | DsmSubSmFlagSpeedHigh | DsmSubSmFlagSpeedFull)) {
                    status = STATUS_SUCCESS;
                } else {
                    status = STATUS_NOT_SUPPORTED;
                }
                action = IoctlActionCompleteHere;

            } else if (RtlCompareMemory(&capabilityType,
                                        &GUID_USB_CAPABILITY_DEVICE_CONNECTION_LOW_SPEED_COMPATIBLE,
                                        sizeof(GUID)) == sizeof(GUID)) {

                //
                // Device is always low speed comatible
                //
                status = STATUS_SUCCESS;
                action = IoctlActionCompleteHere;
            }

            break;

        case IOCTL_INTERNAL_USB_GET_HUB_COUNT:
            //
            // Documented for use by hub driver
            //
            status = STATUS_SUCCESS;
            action = IoctlActionCompleteHere;
            break;

        case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO:
            //
            // Documented for use by hub driver
            //
            status = STATUS_NOT_SUPPORTED;
            action = IoctlActionCompleteHere;
            LEAVE;

            break;

        case IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME:

            status = HUBUCX_GetControllerName(pdoContext->HubFdoContext,
                                              ioStack->Parameters.Others.Argument1,
                                              PtrToUlong(ioStack->Parameters.Others.Argument2));
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_BUS_INFO:

            status = HUBUCX_GetDeviceBusInfo(pdoContext->HubFdoContext,
                                             pdoContext->AssociatedDSM->UsbDevice,
                                             ioStack->Parameters.Others.Argument1,
                                             sizeof(struct _USB_BUS_NOTIFICATION));
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_PORT_STATUS:


            if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
                status = STATUS_INVALID_PARAMETER;
                action = IoctlActionCompleteHere;
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_GET_PORT_STATUS with STATUS_INVALID_PARAMETER because IRQL > PASSIVE_LEVEL");
                LEAVE;
            }

            portStatus = ioStack->Parameters.Others.Argument1;

            if (!portStatus) {
                status = STATUS_INVALID_PARAMETER;
                action = IoctlActionCompleteHere;
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_GET_PORT_STATUS with STATUS_INVALID_PARAMETER because portStatus is null");
                LEAVE;
            }
            *portStatus = 0;

            //
            // Need to squirrel away the pdo info in the IRP
            //
            ioStack->Parameters.Others.Argument2 = pdoContext;

            //
            // Forward to FDO power managed IO queue
            //
            action = IoctlActionForwardToParentQueue;
            LEAVE;

            break;

        case IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE:
            //
            // Documented for use by hub driver
            //
            status = STATUS_NOT_SUPPORTED;
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE:

            //
            // This is documented as used by USBHUB only.
            // In the Windows 7 driver, this function doesn't actually
            // do anything useful (doesn't actually fill in the device handle).
            // This may mean we can get rid of this IOCTL, but for now, we
            // will mimic Windows 7 behavior.
            //
            if (ioStack->Parameters.Others.Argument1 == NULL) {
                status = STATUS_INVALID_PARAMETER;
            } else {
                status = STATUS_SUCCESS;
            }

            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX:

            //
            // This is documented as used by USBHUB only.
            // May be able to depricate this
            //
            // We shouldn't need to actually implement the reference counting
            // since we will ensure that the handle is valid as long as the PDO
            // exists
            //
            status = STATUS_NOT_SUPPORTED;
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_RESET_PORT:

            if (pdoContext->PdoFlags.InD0 == 0) {

                status = STATUS_POWER_STATE_INVALID;
                action = IoctlActionCompleteHere;
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_RESET_PORT with STATUS_POWER_STATE_INVALID because InD0 is 0");

                LEAVE;

            }

            if (KeGetCurrentIrql() > PASSIVE_LEVEL) {

                status = STATUS_INVALID_PARAMETER;
                action = IoctlActionCompleteHere;

                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_RESET_PORT with STATUS_INVALID_PARAMETER because IRQL > PASSIVE_LEVEL");
                LEAVE;

            }

            status = STATUS_SUCCESS;
            action = IoctlActionForwardToQueue;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_RESET_PORT_ASYNC:

            if (pdoContext->PdoFlags.InD0 == 0) {

                status = STATUS_POWER_STATE_INVALID;
                action = IoctlActionCompleteHere;

                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_RESET_PORT_ASYNC with STATUS_POWER_STATE_INVALID because InD0 is 0");

                LEAVE;

            }

            if (KeGetCurrentIrql() > DISPATCH_LEVEL) {

                status = STATUS_INVALID_PARAMETER;
                action = IoctlActionCompleteHere;

                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_RESET_PORT with STATUS_INVALID_PARAMETER because IRQL > DISPATCH_LEVEL");
                LEAVE;

            }

            status = STATUS_SUCCESS;
            action = IoctlActionAsyncForwardToQueue;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_ENABLE_PORT:
            //
            // This IOCTL is no longer supported.  For legacy reasons, we
            // just return success
            //
            status = STATUS_SUCCESS;
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_CYCLE_PORT:

            //
            // We need to always complete this Ioctl without any wait involved
            // for legacy reasons. Therefore we don't forward it to WDF queue,
            // and queue the event in DSM directly from here.
            //
            action = IoctlActionCompleteHere;

            if (KeGetCurrentIrql() > PASSIVE_LEVEL) {

                status = STATUS_INVALID_PARAMETER;
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_USB_CYCLE_PORT with STATUS_INVALID_PARAMETER because IRQL > PASSIVE_LEVEL");
                LEAVE;

            }

            status = STATUS_SUCCESS;

            //
            // If we are already doing cycle port, then we
            // are going to treat any other cycle ports as
            // no-op.
            //
            if (InterlockedCompareExchange(&pdoContext->CyclePortInProgress,
                                           1,
                                           0) == 0) {
                //
                // PDO has a reference on DSM, so it is safe to assume that
                // DSM is there
                //
                USB_HW_VERIFIER_DEVICE_BREAK(pdoContext->AssociatedDSM, DeviceHwVerifierClientInitiatedCyclePort);

                EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
                    pdoContext->AssociatedDSM,
                    IOCTL_INTERNAL_USB_CYCLE_PORT,
                    0);

                HUBSM_AddDsmEvent(pdoContext->AssociatedDSM,
                                  DsmEventClientRequestCycle);
            }
            else {
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Ignoring IOCTL_INTERNAL_USB_CYCLE_PORT because a cycle port is currently in progress");

            }

            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_HUB_NAME:

            status = HUBPDO_GetHubName(pdoContext,
                                       Irp);
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO:
            //
            // Documented for use by hub driver
            //
            status = STATUS_NOT_SUPPORTED;
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:

            idleCallbackInfo = (PUSB_IDLE_CALLBACK_INFO)
                   IoGetCurrentIrpStackLocation(Irp)->Parameters.DeviceIoControl.Type3InputBuffer;

            if ((idleCallbackInfo == NULL) ||
                (idleCallbackInfo->IdleCallback == NULL)) {

                status = STATUS_NO_CALLBACK_ACTIVE;
                action = IoctlActionCompleteHere;

                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failing IOCTL_INTERNAL_SUBMIT_IDLE_NOTIFICATION due to NULL idle callback info or callback function");

            } else {
                GUID    activityId;

                RtlZeroMemory(&activityId, sizeof(GUID));
                HUBMISC_GetActivityIdIrp(Irp, &activityId);

                EventWriteUSBHUB3_ETW_EVENT_DEVICE_SUBMIT_IDLE_NOTIFICATION_START(
                    &activityId,
                    pdoContext->AssociatedDSM->UsbDevice);

                status = HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                                          IsmEventNewRequest,
                                          Irp);
                action = IoctlActionDoNothing;
            }

            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_RECORD_FAILURE:
            //
            // Documented for use by hub driver, but used by others
            //

            status = HUBPDO_RecordFailure(pdoContext,
                                          Irp);

            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY:
            //
            // Documented for use by hub driver
            //
            status = STATUS_NOT_SUPPORTED;
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO:

            status = HUBPDO_ReturnDeviceConfigInfo(pdoContext,
                                                   Irp);
            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS:

            status = HUBPDO_ReturnTopologyAddress(pdoContext,
                                                  Irp);

            action = IoctlActionCompleteHere;
            LEAVE;
            break;

        case IOCTL_INTERNAL_USB_GET_HUB_INFO:
            {
                PHUB_INFO_FROM_PARENT       childHubInfoFromParent;
                PHUB_INFO_FROM_PARENT       hubInfoFromParent;

                childHubInfoFromParent = ioStack->Parameters.Others.Argument1;
                childHubInfoFromParent->IoTarget = pdoContext->HubFdoContext->HubInfoFromParent.IoTarget;
                childHubInfoFromParent->DeviceDescriptor = pdoContext->AssociatedDSM->DeviceDescriptor;
                childHubInfoFromParent->U1ExitLatency = pdoContext->AssociatedDSM->U1ExitLatency;
                childHubInfoFromParent->U2ExitLatency = pdoContext->AssociatedDSM->U2ExitLatency;
                childHubInfoFromParent->SublinkSpeedAttr = pdoContext->AssociatedDSM->SublinkSpeedAttr;
                childHubInfoFromParent->SublinkSpeedAttrCount = pdoContext->AssociatedDSM->SublinkSpeedAttrCount;
                childHubInfoFromParent->TotalHubDepth = pdoContext->AssociatedDSM->PortContext->PortInformation.TotalHubDepth;
                childHubInfoFromParent->HostInitiatedU1ExitLatency = pdoContext->AssociatedDSM->HostInitiatedU1ExitLatency;
                childHubInfoFromParent->HostInitiatedU2ExitLatency = pdoContext->AssociatedDSM->HostInitiatedU2ExitLatency;

                hubInfoFromParent = &pdoContext->HubFdoContext->HubInfoFromParent;

                childHubInfoFromParent->TotalTPPropogationDelay = hubInfoFromParent->TotalTPPropogationDelay +
                    pdoContext->HubFdoContext->HubDescriptor.Hub30Descriptor.wHubDelay + pdoContext->AssociatedDSM->TxTpDelay;

                //
                // We got information from our parent about the slowest link in the chain
                // for U1 and U2. Now we have to pass that information to the child hub
                // but it is possible that the current link (i.e. between the child and us
                // is now the slowest link. We would have done this calculation while
                // calculating SEL PEL values during device configuration.
                //
                if (pdoContext->AssociatedDSM->LPMStateFlags.SlowestLinkForU1Exit == 1) {

                    childHubInfoFromParent->ExitLatencyOfSlowestLinkForU1 = pdoContext->AssociatedDSM->UsbSelInfo.U1PEL;
                    childHubInfoFromParent->DepthOfSlowestLinkForU1 = pdoContext->HubFdoContext->HubParentInterface.HubDepth;

                } else {

                    childHubInfoFromParent->ExitLatencyOfSlowestLinkForU1 = hubInfoFromParent->ExitLatencyOfSlowestLinkForU1;
                    childHubInfoFromParent->DepthOfSlowestLinkForU1 = hubInfoFromParent->DepthOfSlowestLinkForU1;
                }

                if (pdoContext->AssociatedDSM->LPMStateFlags.SlowestLinkForU2Exit == 1) {

                    childHubInfoFromParent->ExitLatencyOfSlowestLinkForU2 = pdoContext->AssociatedDSM->UsbSelInfo.U2PEL;
                    childHubInfoFromParent->DepthOfSlowestLinkForU2 = pdoContext->HubFdoContext->HubParentInterface.HubDepth;

                } else {

                    childHubInfoFromParent->ExitLatencyOfSlowestLinkForU2 = hubInfoFromParent->ExitLatencyOfSlowestLinkForU2;
                    childHubInfoFromParent->DepthOfSlowestLinkForU2 = hubInfoFromParent->DepthOfSlowestLinkForU2;
                }

                if (hubInfoFromParent->HubFlags.DisableLpmForAllDownstreamDevices == 1) {
                    childHubInfoFromParent->HubFlags.DisableLpmForAllDownstreamDevices = 1;
                }

                if (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHighSpeedCapable == 1) {
                    childHubInfoFromParent->HubFlags.HubIsHighSpeedCapable = 1;
                }

                status = STATUS_SUCCESS;
                action = IoctlActionCompleteHere;
                LEAVE;
            }

            break;

        case IOCTL_INTERNAL_USB_GET_DUMP_DATA:
            {
                PUSB_DUMP_DEVICE_INFO deviceInfo;

                deviceInfo = Irp->AssociatedIrp.SystemBuffer;
                deviceInfo->DeviceHandle = pdoContext->AssociatedDSM->UsbDevice;

                status = STATUS_SUCCESS;
                action = IoctlActionForwardToUCX;
                LEAVE;
            }

            break;

        case IOCTL_INTERNAL_USB_FREE_DUMP_DATA:

            status = STATUS_SUCCESS;
            action = IoctlActionForwardToUCX;
            LEAVE;
            break;

        default:
            //
            // Traditionally we complete all unknown irps with the
            // status from the irp
            //
            status = Irp->IoStatus.Status;
            action = IoctlActionCompleteHere;
            break;

        }

    } FINALLY {

        switch(action) {
        case IoctlActionForwardToUCX:

            IoSkipCurrentIrpStackLocation(Irp);
            status = IoCallDriver(pdoContext->TargetDeviceObject,
                                  Irp);
            break;

        case IoctlActionCompleteHere:

            //
            // Log ETW events if enabled
            //
            if (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.EtwEnabled == 1) {
                GUID    activityId;

                RtlZeroMemory(&activityId, sizeof(GUID));
                HUBMISC_GetActivityIdIrp(Irp, &activityId);

                //
                // We log both start and complete events in the case where the
                // request is being completed immediately
                //
                EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_START(
                    &activityId,
                    pdoContext->AssociatedDSM->UsbDevice,
                    ioControlCode);

                //
                // Log URB info if this is a URB request
                //
                if (ioControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {

                    urb = ioStack->Parameters.Others.Argument1;

                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_START(
                        &activityId,
                        pdoContext->AssociatedDSM->UsbDevice,
                        urb->UrbHeader.Function);

                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_URB_COMPLETE(
                        &activityId,
                        pdoContext->AssociatedDSM->UsbDevice,
                        urb->UrbHeader.Function,
                        urb->UrbHeader.Status);
                }

                //
                // Log capability being queried if this is a query capability.
                // Note, capabilities that are passed down the stack are not logged here.
                //
                if (ioControlCode == IOCTL_INTERNAL_USB_QUERY_USB_CAPABILITY) {

                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_QUERY_USB_CAPABILITY(
                        &activityId,
                        pdoContext->AssociatedDSM->UsbDevice,
                        &capabilityType,
                        status);
                }

                EventWriteUSBHUB3_ETW_EVENT_DEVICE_INTERNAL_IOCTL_COMPLETE(
                    &activityId,
                    pdoContext->AssociatedDSM->UsbDevice,
                    ioControlCode,
                    status);
            }

            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);
            break;

        case IoctlActionAsyncForwardToQueue:
            IoCopyCurrentIrpStackLocationToNext(Irp);
            status = WdfDeviceWdmDispatchIrpToIoQueue(Device,
                                                     Irp,
                                                     WdfDeviceGetDefaultQueue(Device),
                                                     WDF_DISPATCH_IRP_TO_IO_QUEUE_PREPROCESSED_IRP);
            break;

        case IoctlActionForwardToParentQueue:

            //
            // Lets initialize the URB status
            //
            // We need to set a completion routine and wait for the event to
            // get set so we can complete this serially.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            KeInitializeEvent(&completionEvent,
                              NotificationEvent,
                              FALSE);

            IoSetCompletionRoutine(Irp,
                                   HUBPDO_SyncCompletionRoutine,
                                   &completionEvent,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            status = WdfDeviceWdmDispatchIrpToIoQueue(Device,
                                                     Irp,
                                                     pdoContext->HubFdoContext->DeviceControlQueue,
                                                     WDF_DISPATCH_IRP_TO_IO_QUEUE_PREPROCESSED_IRP);
            //
            // Log any error return status.  It doesn't look like there
            // is any status code that could indicate that our completion
            // routine won't be called.  Since we are completing this
            // IOCTL synchronously, we will override the status code.
            //
            if (!NT_SUCCESS(status) && (status != STATUS_PENDING)) {
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "WdfDeviceWdmDispatchPreprocessedIrp returned %!STATUS!",
                           status);
            }

            //
            // Now we must wait for the completion routine to get called
            // which will in turn set our event
            //
            HUBMISC_WaitForSignal(&completionEvent,
                                  "Client Driver Request",
                                  Device);

            status = Irp->IoStatus.Status;

            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);

            break;


        case IoctlActionForwardToQueue:

            //
            // Lets initialize the URB status
            //
            // We need to set a completion routine and wait for the event to
            // get set so we can complete this serially.
            //
            IoCopyCurrentIrpStackLocationToNext(Irp);
            KeInitializeEvent(&completionEvent,
                              NotificationEvent,
                              FALSE);

            IoSetCompletionRoutine(Irp,
                                   HUBPDO_SyncCompletionRoutine,
                                   &completionEvent,
                                   TRUE,
                                   TRUE,
                                   TRUE);


            status = WdfDeviceWdmDispatchIrpToIoQueue(Device,
                                                     Irp,
                                                     WdfDeviceGetDefaultQueue(Device),
                                                     WDF_DISPATCH_IRP_TO_IO_QUEUE_PREPROCESSED_IRP);
            //
            // Log the return status.  It doesn't look like there
            // is any status code that could indicate that our completion
            // routine won't be called.  Since we are completing this
            // IOCTL synchronously, we will override the status code.
            //
            if (!NT_SUCCESS(status) && (status != STATUS_PENDING)) {
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "WdfDeviceWdmDispatchPreprocessedIrp returned %!STATUS!",
                           status);
            }

            //
            // Now we must wait for the completion routine to get called
            // which will in turn set our event
            //
            HUBMISC_WaitForSignal(&completionEvent,
                                  "Client Driver Request",
                                  Device);

            status = Irp->IoStatus.Status;

            IoCompleteRequest(Irp,
                              IO_NO_INCREMENT);

            break;

        }

    }

    return status;
}


VOID
HUBPDO_SignalPnpPowerEvent(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This function sets the return status to Success and sets the event
    on which all the WDF callbacks  wait on.

Return Value:

    NTSTATUS

--*/
{

    TRY {

        DeviceContext->PnPPowerStatus = STATUS_SUCCESS;

        KeSetEvent(&DeviceContext->PnPPowerEvent,
                   IO_NO_INCREMENT,
                   FALSE);

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }



    } FINALLY {

    }

}

VOID
HUBPDO_CompletePdoPreStart(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This function sets the return status to Success and sets the event
    on which all the WDF callbacks  wait on.

Return Value:

    NTSTATUS

--*/
{

    TRY {

        KeSetEvent(&DeviceContext->PreStartEvent,
                   IO_NO_INCREMENT,
                   FALSE);

    } FINALLY {

    }

}

VOID
HUBPDO_SignalQueryDeviceTextEvent(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This function signals the event that PreProcess routine
    for Query Device Text IRP is waiting on.

Return Value:

    VOID

--*/
{

    TRY {

        KeSetEvent(&DeviceContext->QueryDeviceTextEvent,
                   IO_NO_INCREMENT,
                   FALSE);


    } FINALLY {

    }

}


VOID
HUBPDO_SignalPnpPowerFailureEvent(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This function sets the return status to Failure and sets the event
    on which all the WDF callbacks  wait on.

Return Value:

    VOID

--*/
{

    TRY {

        DeviceContext->PnPPowerStatus = STATUS_UNSUCCESSFUL;

        KeSetEvent(&DeviceContext->PnPPowerEvent,
                   IO_NO_INCREMENT,
                   FALSE);


        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

    } FINALLY {

    }

}


NTSTATUS
HUBPDO_EvtDeviceD0Entry (
    __in
        WDFDEVICE               Device,
    __in
        WDF_POWER_DEVICE_STATE  PreviousState
    )
/*++

Routine Description:

    Event callback when WDF decides its time to power up the device

    In this case, we will send DsmEventPDOD0Entry
    and then wait for the PnPPowerEvent to be signalled.

Return Value:

    The status saved in the device's PnPPowerStatus

--*/
{
    NTSTATUS            status;
    PDEVICE_CONTEXT     deviceContext;
    GUID                activityId;
    PHUB_PDO_CONTEXT    pdoContext;
    POWER_ACTION        powerAction;

    UNREFERENCED_PARAMETER(PreviousState);

    TRY {

        pdoContext = GetHubPdoContext(Device);
        deviceContext = pdoContext->AssociatedDSM;

        //
        // Store it for logging it in ETW rundown
        //
        pdoContext->CurrentWdfPowerDeviceState = WdfPowerDeviceD0;

        RtlZeroMemory(&activityId, sizeof(GUID));

        if (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.EtwEnabled == 1) {

            status = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                          &deviceContext->WdfActivityId);

            if (NT_SUCCESS(status)) {
                activityId = deviceContext->WdfActivityId;
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, WdfActivityIdValid);
            }
        }

        powerAction = WdfDeviceGetSystemPowerAction(Device);

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_D0_ENTRY_START(
            &activityId,
            deviceContext->UsbDevice,
            WdfPowerDeviceD0,
            powerAction,
            (USHORT)pdoContext->PdoFlags.WaitWakePending);

        HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                         IsmEventPDOD0,
                         NULL);

        KeClearEvent(&deviceContext->PnPPowerEvent);

        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventPDOD0Entry);

        HUBMISC_WaitForSignal(&deviceContext->PnPPowerEvent,
                              "Device D0Entry",
                              Device);

        status = deviceContext->PnPPowerStatus;

        if (pdoContext->SleepStudyHandle)
        {
            UsbSleepStudy_DeviceD0Entry(pdoContext->SleepStudyHandle);
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_D0_ENTRY_COMPLETE(
            &activityId,
            deviceContext->UsbDevice,
            status,
            powerAction);

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, WdfActivityIdValid);
    }

    return status;

}

NTSTATUS
HUBPDO_EvtDeviceD0Exit (
    __in
        WDFDEVICE               Device,
    __in
        WDF_POWER_DEVICE_STATE  TargetState
    )
/*++

Routine Description:

    Event callback when WDF decides its time to power down the device

    In this case, we will send DsmEventPDOD0Exit or DsmEventPDOD0ExitFinal,
    and then wait for the PnPPowerEvent to be signalled.

Return Value:

    The status saved in the device's PnPPowerStatus

--*/
{
    NTSTATUS            status;
    PHUB_PDO_CONTEXT    pdoContext;
    PDEVICE_CONTEXT     deviceContext;
    POWER_ACTION        powerAction;
    GUID                activityId;

    TRY {

        pdoContext = GetHubPdoContext(Device);

        //
        // Store it for logging it in ETW rundown
        //
        pdoContext->CurrentWdfPowerDeviceState = TargetState;

        deviceContext = pdoContext->AssociatedDSM;

        RtlZeroMemory(&activityId, sizeof(GUID));

        if (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.EtwEnabled == 1) {

            status = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                          &deviceContext->WdfActivityId);

            if (NT_SUCCESS(status)) {
                activityId = deviceContext->WdfActivityId;
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, WdfActivityIdValid);
            }
        }

        KeClearEvent(&deviceContext->PnPPowerEvent);

        powerAction = WdfDeviceGetSystemPowerAction(Device);

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_D0_EXIT_START(
            &activityId,
            deviceContext->UsbDevice,
            TargetState,
            powerAction,
            (USHORT)pdoContext->PdoFlags.WaitWakePending);

        if ((TargetState != WdfPowerDeviceD3Final) &&
            (powerAction == PowerActionNone) &&
            (deviceContext->DeviceFlags.DeviceSupportsSelectiveSuspend == 0)) {

            WDF_OBJECT_ATTRIBUTES       attributes;
            WDF_WORKITEM_CONFIG         workitemConfig;
            WDFWORKITEM                 registryWorker;

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &deviceContext->DeviceFlags, DeviceSupportsSelectiveSuspend);

            WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
            attributes.ParentObject = Device;

            WDF_WORKITEM_CONFIG_INIT(&workitemConfig, HUBREG_EvtWorkItemUpdateSqmDeviceSelectiveSuspended);

            status = WdfWorkItemCreate(&workitemConfig,
                                        &attributes,
                                        &registryWorker);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Driver,
                           "WdfWorkItemCreate Failed for updating the DeviceSelectiveSuspended registry flag %!STATUS!",
                           status);
            } else {
                WdfWorkItemEnqueue(registryWorker);
            }
        }

        if ((TargetState == WdfPowerDeviceD3Final) &&
            (powerAction == PowerActionNone)) {

            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventPDOD0ExitFinal);

        } else if (TargetState == WdfPowerDevicePrepareForHibernation) {
            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventPDOPrepareForHibernation);
        } else {
            //
            // For maintaining compat with Win7 stack, we have to complete
            // Wait wake IRP and Idle Irp(done by state machine), if the
            // device state is D3
            //
            if (TargetState == WdfPowerDeviceD3) {

                if (pdoContext->PdoFlags.RequiresArmingForWake == 1) {
                    status = WdfDeviceIndicateWakeStatus(Device,
                                                         STATUS_POWER_STATE_INVALID);

                    if (!NT_SUCCESS(status)) {
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_WARNING,
                                   Device,
                                   "WdfDeviceIndicateWakeStatus failed %!STATUS!",
                                   status);
                    }

                    INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, RequiresArmingForWake);
                }

                if (deviceContext->DeviceFlags.AllowIdleIrpInD3 == 0) {
                    HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                                     IsmEventPDOD3,
                                     NULL);
                }
            }

            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventPDOD0Exit);
        }

        HUBMISC_WaitForSignal(&deviceContext->PnPPowerEvent,
                              "Device D0Exit",
                              Device);

        status = deviceContext->PnPPowerStatus;

        if (WdfTimerStop(deviceContext->OutOfBandwidthTimer,
                         TRUE) == TRUE) {

            //
            // If we had a timer running for showing out of
            // bandwidth notification and if we successfully
            // stopped htat timer, then we need to fire the
            // WMI event here
            //
            WMI_FireNotification(deviceContext->HubFdoContext,
                                 deviceContext->PortContext->PortInformation.PortNumber,
                                 InsufficentBandwidth);

        }

        if (pdoContext->SleepStudyHandle)
        {
            UsbSleepStudy_DeviceD0Exit(pdoContext->SleepStudyHandle);
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_D0_EXIT_COMPLETE(
            &activityId,
            deviceContext->UsbDevice,
            status,
            powerAction);

        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, WdfActivityIdValid);

    }

    return status;

}


NTSTATUS
HUBPDO_EvtDevicePrepareHardware (
    __in
        WDFDEVICE       Device,
    __in
        WDFCMRESLIST    ResourcesRaw,
    __in
        WDFCMRESLIST    ResourcesTranslated
    )
/*++

Routine Description:

    Event callback when our resources are assigned.

    In this case, we will send DsmEventPrepareHardware
    and then wait for the PnPPowerEvent to be signalled.

Return Value:

    The status saved in the device's PnPPowerStatus

--*/
{
    NTSTATUS                status;
    PDEVICE_CONTEXT         deviceContext;
    PHUB_PDO_CONTEXT        pdoContext;

    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE();

    TRY {

        pdoContext = GetHubPdoContext(Device);
        deviceContext = pdoContext->AssociatedDSM;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_PREPARE_HARDWARE_START(
            0,
            deviceContext->UsbDevice);

        //
        // Disable PreStart events as we are now starting. Note that
        // state machine handles the event whenever it is not in D0
        // and state machine is guaranteed to get D0Entry after any
        // PreStart events that were already queued
        //
        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, DeviceStarted);

        HUBREG_QueryValuesInDeviceHardwareKey(deviceContext);

        if ((deviceContext->MsOs20Info.Flags.AlternateEnumeration == 1) &&
            (deviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdCached == 0)) {
            
            HUBREG_AssignUsbflagsValueForDevice(deviceContext,
                                                &g_MsOs20DescriptorSetInfo,
                                                (PUCHAR)&deviceContext->MsOs20Info.DescriptorSetInfo,
                                                sizeof(MSOS20_DESCRIPTOR_SET_INFO));

            deviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdCached = 1;
        }
        
        //
        // Flush the SQM properties into registry
        //
        HUBREG_UpdateSqmFlags(deviceContext);

        HUBMISC_SetSqmEnumerationData(deviceContext);

        if (deviceContext->DeviceFlags.DeviceIsHub) {

            if (deviceContext->HubFdoContext->HubParentInterface.HubDepth >= deviceContext->PortContext->PortInformation.TotalHubDepth) {

                deviceContext->PortContext->ConnectionStatus = DeviceHubNestedTooDeeply;

                WMI_FireNotification(deviceContext->HubFdoContext,
                                     pdoContext->PortNumber,
                                     HubNestedTooDeeply);

                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "Hub nested too deeply");

                status = STATUS_UNSUCCESSFUL;

                EventWriteUSBHUB3_ETW_EVENT_DEVICE_PREPARE_HARDWARE_NESTED_TOO_DEEP(
                    0,
                    deviceContext->UsbDevice);

                LEAVE;
            }

            //
            // Create the USB Hub Device Interface.
            //
            status = WdfDeviceCreateDeviceInterface(Device,
                                                    (LPGUID)&GUID_DEVINTERFACE_USB_HUB,
                                                    NULL);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Creation of Hub Device Interface failed %!STATUS!",
                           status);
                LEAVE;
            }

            status = HUBMISC_GetDeviceInterfacePath((LPGUID)&GUID_DEVINTERFACE_USB_HUB,
                                                    Device,
                                                    &deviceContext->SymbolicLinkName,
                                                    deviceContext->PortContext->IfrLog);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failed to retrieve symbolic link name for child hub %!STATUS!",
                           status);

                LEAVE;
            }

        } else {

            //
            // Create the USB Device Interface.
            //
            status = WdfDeviceCreateDeviceInterface(Device,
                                                    (LPGUID)&GUID_DEVINTERFACE_USB_DEVICE,
                                                    NULL);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Creation of Hub Device Interface failed %!STATUS!",
                           status);
                LEAVE;
            }

            status = HUBMISC_GetDeviceInterfacePath((LPGUID)&GUID_DEVINTERFACE_USB_DEVICE,
                                                    Device,
                                                    &deviceContext->SymbolicLinkName,
                                                    deviceContext->PortContext->IfrLog);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failed to retrieve symbolic link name for child device %!STATUS!",
                           status);
                LEAVE;
            }
        }

        if (deviceContext->SymbolicLinkName != NULL) {
            status = HUBREG_WriteStringToDeviceHardwareKey(deviceContext,
                                                           &g_SymbolicName,
                                                           deviceContext->SymbolicLinkName);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Failed to write symbolic link name to registry %!STATUS!",
                           status);
                LEAVE;
            }
        }

        //
        // We need to clear the FailIo bit as long as the device is still there
        //
        if ((pdoContext->PdoFlags.DeviceGone == 0) &&
            (deviceContext->DeviceStateFlags.DeviceIsKnown == 1)) {

            pdoContext->FailIo = FALSE;

        }

        //
        // Log Device Info to ETW if it is enabled
        //
        if (GetHubGlobals(WdfGetDriver())->HubGlobalFlags.EtwEnabled == 1) {
            WDFMEMORY               deviceDescriptionMemory;
            PWSTR                   deviceDescription;
            UNICODE_STRING          symbolicLinkNameUnicode;
            LPWSTR                  symbolicLinkNameString;
            WDFSTRING               pdoName;
            UNICODE_STRING          pdoNameUnicode;
            LPWSTR                  pdoNameString;
            WDF_OBJECT_ATTRIBUTES   attributes;
            PUSB_TOPOLOGY_ADDRESS   usbTopologyAddress;
            ULONG                   portPath[6];
            ULONG                   i;
            PUCX_CONTROLLER_INFO    controllerInfo;

            RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

            if (deviceContext->SymbolicLinkName != NULL) {
                WdfStringGetUnicodeString(deviceContext->SymbolicLinkName, &symbolicLinkNameUnicode);
            }

            symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                                                           symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                                                           USBHUB3_DEVICE_TAG);

            //
            // String needs to be NULL-terminated
            //
            if (symbolicLinkNameString != NULL) {
                RtlZeroMemory(symbolicLinkNameString,
                              symbolicLinkNameUnicode.Length + sizeof(WCHAR));

                RtlCopyMemory(symbolicLinkNameString,
                              symbolicLinkNameUnicode.Buffer,
                              symbolicLinkNameUnicode.Length);
            }

            WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
            attributes.ParentObject = Device;
            pdoName = NULL;
            pdoNameString = NULL;

            if ((NT_SUCCESS(WdfStringCreate(NULL, &attributes, &pdoName))) &&
                (NT_SUCCESS(WdfDeviceRetrieveDeviceName(Device, pdoName)))) {

                WdfStringGetUnicodeString(pdoName, &pdoNameUnicode);

                pdoNameString = ExAllocatePoolWithTag(NonPagedPool,
                                                      pdoNameUnicode.Length + sizeof(WCHAR),
                                                      USBHUB3_DEVICE_TAG);

                //
                // String needs to be NULL-terminated
                //
                if (pdoNameString != NULL) {
                    RtlZeroMemory(pdoNameString,
                                  pdoNameUnicode.Length + sizeof(WCHAR));

                    RtlCopyMemory(pdoNameString,
                                  pdoNameUnicode.Buffer,
                                  pdoNameUnicode.Length);
                }
            }

            WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
            attributes.ParentObject = Device;

            deviceDescriptionMemory = NULL;
            if (NT_SUCCESS(WdfDeviceAllocAndQueryProperty(Device,
                                                          DevicePropertyDeviceDescription,
                                                          NonPagedPool,
                                                          &attributes,
                                                          &deviceDescriptionMemory))) {

                deviceDescription = WdfMemoryGetBuffer(deviceDescriptionMemory, NULL);

            } else {
                deviceDescription = NULL;
            }

            usbTopologyAddress = &pdoContext->HubFdoContext->HubParentInterface.HubTopologyAddress;

            portPath[0] = usbTopologyAddress->RootHubPortNumber;
            for (i = 0; i < 5; i++) {
                portPath[i+1] = usbTopologyAddress->HubPortNumber[i];
            }

            if (pdoContext->HubFdoContext->HubParentInterface.HubDepth < 6) {
                portPath[pdoContext->HubFdoContext->HubParentInterface.HubDepth] = pdoContext->PortNumber;
            }

            controllerInfo = &pdoContext->HubFdoContext->ControllerInfo;

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_PREPARE_HARDWARE_DEVICE_INFORMATION_V1(
                0,
                deviceContext->HubFdoContext->HubParentInterface.Hub,
                deviceContext->UsbDevice,
                deviceContext->PortContext->PortInformation.PortNumber,
                deviceDescription,
                symbolicLinkNameString,
                sizeof(USB_DEVICE_DESCRIPTOR),
                &deviceContext->DeviceDescriptor,
                (deviceContext->ConfigurationDescriptor != NULL) ?
                  deviceContext->ConfigurationDescriptor->wTotalLength :
                  0,
                (PUCHAR)deviceContext->ConfigurationDescriptor,
                pdoNameString,
                pdoContext->PdoFlags.InD0 ? 0 : 1,
                pdoContext->HubFdoContext->HubParentInterface.HubDepth+1,
                (unsigned int *)portPath,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.BusNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.FunctionNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.VendorId : LONG_MAX,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceId : LONG_MAX,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.RevisionId : 0,
                pdoContext->CurrentWdfPowerDeviceState,
                deviceContext->Usb20LpmStatus,
                controllerInfo->Type,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.VendorId : NULL,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.DeviceId : NULL,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.RevisionId : NULL,
                deviceContext->PortContext->PortInformation.PortFlags.PortFlagAcpiUpcValid ? 1 : 0,
                (deviceContext->PortContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) ?
                    deviceContext->PortContext->AcpiUpcPackage.PortConnectorType : 
                    0,
                deviceContext->PortContext->ConnectorId.UcmConnectorId);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, USBHUB3_DEVICE_TAG);
            }

            if (pdoNameString != NULL) {
                ExFreePoolWithTag(pdoNameString, USBHUB3_DEVICE_TAG);
            }

            if (pdoName != NULL) {
                WdfObjectDelete(pdoName);
            }

            if (deviceDescriptionMemory != NULL) {
                WdfObjectDelete(deviceDescriptionMemory);
            }
        }

        //
        // Register WMI provider only if the device is not composite and not a hub
        //
        if ((deviceContext->DeviceFlags.DeviceIsHub == 0) &&
            (deviceContext->DeviceFlags.DeviceIsComposite == 0)) {
            WMI_RegisterDevice(Device);
        }

        if (GetHubGlobals(WdfGetDriver())->SleepStudyEnabled) {
            status = UsbSleepStudy_RegisterPdo(WdfDeviceWdmGetPhysicalDevice(Device),
                                               WdfDeviceWdmGetPhysicalDevice(WdfObjectContextGetObject(pdoContext->HubFdoContext)),
                                               FALSE,
                                               &pdoContext->SleepStudyHandle);
            if (!NT_SUCCESS(status)) {
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                    TL_ERROR,
                    Driver,
                    "UsbSleepStudy_RegisterPdo Failed %!STATUS!",
                    status);
                pdoContext->SleepStudyHandle = NULL;
                LEAVE;
            }
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_PREPARE_HARDWARE_COMPLETE(
            0,
            deviceContext->UsbDevice,
            status);

        DbgTraceTo(deviceContext->PortContext->IfrLog,
                   TL_ERROR,
                   Device,
                   "DeviceHackFlags:0x%X",
                   deviceContext->DeviceHackFlags.AsUlong32);

    }

    return status;

}


NTSTATUS
HUBPDO_EvtDeviceReleaseHardware (
    __in
        WDFDEVICE       Device,
    __in
        WDFCMRESLIST    ResourcesTranslated
    )
/*++

Routine Description:

    Event callback to perform operations that are needed when the
    device is no longer accessible.

    In this case, we will send DsmEventReleaseHardware
    and then wait for the PnPPowerEvent to be signalled.

Return Value:

    The status saved in the device's PnPPowerStatus

--*/
{

    PHUB_PDO_CONTEXT        pdoContext;
    PDEVICE_CONTEXT         deviceContext;

    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE();

    TRY {

        pdoContext = GetHubPdoContext(Device);
        deviceContext = pdoContext->AssociatedDSM;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_RELEASE_HARDWARE_START(
            0,
            deviceContext->UsbDevice);

        if (pdoContext->SleepStudyHandle)
        {
            UsbSleepStudy_UnregisterPdo(pdoContext->SleepStudyHandle);
            pdoContext->SleepStudyHandle = NULL;
        }

        pdoContext = GetHubPdoContext(Device);

        pdoContext->FailIo = TRUE;
        INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, DeviceStarted);

        //
        // For maintaining compat with the Win7 stack, we
        // need to know if we are being actually removed and
        // not just being stopped
        //
        if (pdoContext->PdoFlags.QueryStopped == 1) {
            INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, QueryStopped);

            LEAVE;
        }


        //
        // Cleanup UXD settings if the device was disabled.
        //
        if (pdoContext->PdoFlags.QueryRemoved == 1) {
            INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, QueryRemoved);

            HUBREG_DeleteUxdSettings(pdoContext->AssociatedDSM,
                                     UxdEventDisable);
        }

        if (deviceContext->DeviceFlags.DeviceIsDualRole == 1) {
            HUBWNF_PublishUsbPartnerDualRoleFeatures(deviceContext->PortContext->ConnectorId.UcmConnectorId,
                                                     FALSE,
                                                     0L);
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_PROPERTIES, &deviceContext->DeviceFlags, DeviceIsDualRole);
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_RELEASE_HARDWARE_COMPLETE(
            0,
            deviceContext->UsbDevice,
            STATUS_SUCCESS);
    }

    return STATUS_SUCCESS;

}

VOID
HUBPDO_UnregisterForPowerSettings (
    __in
        PHUB_PDO_CONTEXT        PdoContext
    )
/*++

Routine Description:

    Unregister all the LPM related power settings


Return Value:


--*/
{
    ULONG       i;
    NTSTATUS    status;

    TRY {

        for (i = 0 ;i < LPM_SETTINGS_COUNT ; i++) {

            if (PdoContext->LpmPowerSettingsHandles[i] == NULL) {
                continue;
            }
            status = PoUnregisterPowerSettingCallback(PdoContext->LpmPowerSettingsHandles[i]);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(PdoContext->AssociatedDSM->PortContext,
                           TL_ERROR,
                           Device,
                           "PoUnregisterPowerSettingCallback Failed %!STATUS!",
                           status);
            }

            PdoContext->LpmPowerSettingsHandles[i] = NULL;
        }


    } FINALLY {

    }

    return;

}


VOID
HUBPDO_EvtDeviceCleanup (
    WDFOBJECT       Device
    )
/*++

Routine Description:

    Event callback to perform operations that are needed when the
    device is no longer accessible.

    In this case, we will send DsmEventReleaseHardware
    and then wait for the PnPPowerEvent to be signalled.

Return Value:

    The status saved in the device's PnPPowerStatus

--*/
{
    PDEVICE_CONTEXT                 deviceContext;
    PHUB_PDO_CONTEXT                pdoContext;
    UCXUSBDEVICE                    usbDevice;
    CONTROLLER_STOP_IDLE_CONTEXT    stopIdleContext;

    NT_ANALYSIS_ASSUME(KeGetCurrentIrql() == PASSIVE_LEVEL);

    TRY {

        pdoContext = GetHubPdoContext(Device);
        deviceContext = pdoContext->AssociatedDSM;
        usbDevice = deviceContext->UsbDevice;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_CLEANUP_START(
            0,
            usbDevice);

        HUBPDO_UnregisterForPowerSettings(pdoContext);

        if (pdoContext->PdoFlags.IdleStateMachineInitialized == 1) {
            HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                             IsmEventPDOCleanup,
                             NULL);
        }

        //
        // When a USB device attached to the root hub gets
        // detached while in suspended state, it is possible
        // that the controller goes into S0Idle by the time
        // remove comes through. DSM will cleanup the endpoints
        // and device from the controller in the context of
        // the cleanup event. Therefore we need to keep the 
        // controller in D0 while the cleanup is being processed 
        // by DSM. There is also the possibility that a USB
        // tree gets detached in which we case we could end
        // up cleaning the endpoints in the context of hub
        // suprise remove. To cover that possiblity, a similar
        // logic exists in FDO ReleaseHardware
        //
        // Note that we don't want to keep the controller in D0
        // from the point when the device was reported missing
        // because it can take forever for the remove to come
        // through
        //
        CONTROLLER_STOP_IDLE_CONTEXT_INIT(&stopIdleContext);

        HUBUCX_AcquirePowerReferenceOnController(deviceContext->HubFdoContext,
                                                 &stopIdleContext);           

        KeClearEvent(&deviceContext->PnPPowerEvent);

        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventPDOCleanup);

        HUBMISC_WaitForSignal(&deviceContext->PnPPowerEvent,
                              "Device Cleanup",
                              Device);

        HUBPDO_BillboardCleanup(deviceContext);

        HUBUCX_ReleasePowerReferenceOnController(deviceContext->HubFdoContext,
                                                 &stopIdleContext);

        //
        // Now we can release the reference that we took on DSM
        // when PDO got created
        //
        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                    (PVOID) PDOTag);

        if (pdoContext->FailData != NULL) {
            ExFreePoolWithTag(pdoContext->FailData, USBHUB3_DEVICE_TAG);
            pdoContext->FailData = NULL;
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_CLEANUP_COMPLETE(
            0,
            usbDevice);
    }

    return;

}

#if 0

VOID
HUBPDO_EvtDeviceDestroy (
    WDFOBJECT       Device
    )
/*++

Routine Description:

    Event callback when PDO is being deleted. The only operation
    that needs to be performed here is a part of a workaround
    for an issue in PnpMgr bug. See the comments in
    HUBPDO_EvtDeviceReportedMissing where this dummy target
    is being created.
    
    Note: This is no longer needed hence commenting out. Refer
    comments in HUBPDO_OpenDummyHandleOnParentStack why this is not
    needed anymore.

Return Value:

--*/
{
    PHUB_PDO_CONTEXT    pdoContext;

    TRY {

        pdoContext = GetHubPdoContext(Device);

        if (pdoContext->DummyParentTarget) {
            WdfObjectDelete(pdoContext->DummyParentTarget);
            pdoContext->DummyParentTarget = NULL;
        }

    } FINALLY {

    }

    return;

}

#endif

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_BillboardCleanup(
    __in
        PDEVICE_CONTEXT DeviceContext
    )
/*++

Routine Description:

    If the device was a Billboard, cleanup its state.

--*/
{

    NTSTATUS            status;
    WDF_DEVICE_PROPERTY_DATA
                        propData;

    PAGED_CODE();

    TRY {

        if (DeviceContext->BillboardInfo == NULL) {
            LEAVE;
        }

        WDF_DEVICE_PROPERTY_DATA_INIT(&propData, &DEVPKEY_Device_UsbBillboardInfo);

        status = WdfDeviceAssignProperty(WdfObjectContextGetObject(DeviceContext->PdoContext),
                                         &propData,
                                         DEVPROP_TYPE_BINARY,
                                         0,
                                         NULL);
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Failed to delete Billboard property %!STATUS!",
                       status);
        }

        //
        // Publish the event even if we failed to delete the property. The devnode
        // is going to go away, and user-mode should be filtering out non-present
        // devnodes anyways.
        //
        status = HUBWNF_PublishBillboardChange();
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Failed to publish WNF_USB_BILLBOARD_CHANGE %!STATUS!",
                       status);
        }

        ExFreePoolWithTag(DeviceContext->BillboardInfo->BillboardDescriptor,
                          USBHUB3_HUB_TAG);

        ExFreePoolWithTag(DeviceContext->BillboardInfo,
                          USBHUB3_HUB_TAG);  

        DeviceContext->BillboardInfo = NULL;

    } FINALLY {

    }

    return;

}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_PublishBillboardDetails(
    __in
        PHUB_PDO_CONTEXT PdoContext
    )
/*++

Routine Description:

    Publish details of the Billboard BOS descriptor so that any
    interested entities may consume it.

    Failures are not fatal.

--*/
{
    NTSTATUS                                    status;
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR billboardDesc;
    WDFDEVICE                                   pdo;
    PUSB_BILLBOARD_INFORMATION                  billboardInfo;
    ULONG                                       billboardInfoSize;
    PDEVICE_CONTEXT                             deviceContext;
    ULONG                                       amIndex;
    RTL_BITMAP                                  bmConfigured;
    ULONG                                       amState;
    PUSB_BILLBOARD_ALTERNATE_MODE               altMode;
    WDF_DEVICE_PROPERTY_DATA                    propData;

    PAGED_CODE();

    TRY {

        NT_ASSERT(PdoContext->AssociatedDSM->BillboardInfo != NULL);

        pdo = WdfObjectContextGetObject(PdoContext);
        deviceContext = PdoContext->AssociatedDSM;
        billboardInfo = NULL;

        billboardDesc = deviceContext->BillboardInfo->BillboardDescriptor;

        billboardInfoSize = USB_BILLBOARD_INFORMATION_SIZE(billboardDesc->bNumberOfAlternateModes);

        billboardInfo = (PUSB_BILLBOARD_INFORMATION) ExAllocatePoolWithTag(PagedPool,
                                                                           billboardInfoSize,
                                                                           USBHUB3_DEVICE_TAG);
        if (billboardInfo == NULL) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Failed to allocate %lu bytes for Billboard info",
                       billboardInfoSize);
            LEAVE;
        }

        RtlZeroMemory(billboardInfo, billboardInfoSize);
        billboardInfo->NumberOfAlternateModes = billboardDesc->bNumberOfAlternateModes;
        billboardInfo->PreferredAlternateModeIndex = billboardDesc->bPreferredAlternateMode;

        RtlInitializeBitMap(&bmConfigured,
                            (PULONG) billboardDesc->bmConfigured,
                            sizeof(billboardDesc->bmConfigured));

        for (amIndex = 0; amIndex < billboardDesc->bNumberOfAlternateModes; ++amIndex) {

            altMode = &billboardInfo->AlternateModes[amIndex];

            altMode->SVid = billboardDesc->AlternateMode[amIndex].wSVID;
            altMode->Mode = billboardDesc->AlternateMode[amIndex].bAlternateMode;

#pragma prefast(suppress:__WARNING_MUST_USE, "It is being used")
            amState = RtlCheckBit(&bmConfigured, amIndex * 2 + 1);
            amState <<= 1;
            amState |= RtlCheckBit(&bmConfigured, amIndex * 2);

            altMode->Status = (USB_BILLBOARD_ALTERNATE_MODE_STATUS) amState;
        }

        WDF_DEVICE_PROPERTY_DATA_INIT(&propData, &DEVPKEY_Device_UsbBillboardInfo);
        status = WdfDeviceAssignProperty(pdo,
                                         &propData,
                                         DEVPROP_TYPE_BINARY,
                                         billboardInfoSize,
                                         billboardInfo);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Failed to assign Billboard information property %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBWNF_PublishBillboardChange();
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "Failed to publish WNF_USB_BILLBOARD_CHANGE %!STATUS!",
                       status);
        }

    } FINALLY {

        if (billboardInfo != NULL) {
            ExFreePoolWithTag(billboardInfo, USBHUB3_DEVICE_TAG);
        }

    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_PublishDualRoleFeaturesProperty(
    __in
        PHUB_PDO_CONTEXT PdoContext
    )
/*++

    Routine Description:

    Publish the USB Dual Features Device property.

    Failures are not fatal.

    --*/
{
    WDF_DEVICE_PROPERTY_DATA    propData;
    NTSTATUS                    status;
    WDFDEVICE                   pdo;
    PDEVICE_CONTEXT             deviceContext;
    ULONG                       usbDualRoleFeatures;

    PAGED_CODE();

    TRY {
        pdo = WdfObjectContextGetObject(PdoContext);
        deviceContext = PdoContext->AssociatedDSM;
        usbDualRoleFeatures = UsbDualRoleFeaturesGetUlong(&deviceContext->UsbDualRolePartnerFeatures);

        WDF_DEVICE_PROPERTY_DATA_INIT(&propData, &DEVPKEY_Device_UsbDualRoleFeatures);

        status = WdfDeviceAssignProperty(pdo,
                                         &propData,
                                         DEVPROP_TYPE_UINT32,
                                         sizeof(usbDualRoleFeatures),
                                         &usbDualRoleFeatures);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                TL_ERROR,
                Driver,
                "Failed to assign DEVPKEY_Device_UsbDualRoleFeatures property %!STATUS!",
                status);
            LEAVE;
        }

    } FINALLY {

    }

}


VOID
HUBPDO_EvtDeviceSurpriseRemoval (
    __in
        WDFDEVICE   Device
    )
/*++

Routine Description:

    Event callback which performs any operations that are needed after a
    device has been unexpectedly removed from the system, or after a driver
    reports that the device has failed.

    In this case, we will send DsmEventPDOSurpriseRemove
    and then return immediately without waiting.

Return Value:

    VOID

--*/
{
    PDEVICE_CONTEXT     deviceContext;

    PAGED_CODE();
    NT_ANALYSIS_ASSUME(KeGetCurrentIrql() == PASSIVE_LEVEL);

    TRY {

        deviceContext = GetHubPdoContext(Device)->AssociatedDSM;

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_SURPRISE_REMOVAL_START(
            0,
            deviceContext->UsbDevice);

        //
        // If one the hubs in the chain of parents is gone, then
        // we need to remove the device from the global child list
        // because in that sceanrio, KMDF is not guaranteed to call
        // our PDO missing callback
        //
        if (HUBUCX_CheckIfHubIsDisconnected(deviceContext->HubFdoContext)) {
            HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);
        }

        deviceContext->PdoContext->FailIo = TRUE;

        HUBPDO_BillboardCleanup(deviceContext);

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_DEVICE_SURPRISE_REMOVAL_COMPLETE(
            0,
            deviceContext->UsbDevice);
    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBPDO_ParentInterfaceWasDeviceProgrammingLostOnResume (
    __in
        PHUB_PDO_CONTEXT        PdoContext
    )
{
    BOOLEAN             retValue;

    if (PdoContext->PdoFlags.ProgrammingLostOnLastReset == 1) {
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &PdoContext->PdoFlags, ProgrammingLostOnLastReset);
        retValue = TRUE;
    } else {
        retValue = FALSE;
    }

    return retValue;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_GetHubSymbolicLinkName(
    __in
        PHUB_PDO_CONTEXT PdoContext,
    __in
        PUNICODE_STRING  HubSymbolicLinkName
    )
/*++

Routine Description:

    This is the handler for GetHubSymbolicLinkName of the HUB_PARENT_INTERFACE.
    This function is called by the hub driver to retrieve the hub's symbolic link
    name.

Arguments:

    PdoContext - The PDO context that was returned to the Hub driver during an HUB_PARENT_INTERFACE
                 exchange.

    HubSymbolicLinkName - A pointer to a UNICODE_STRING to store the symbolic link name string

--*/
{
    TRY {

        RtlInitUnicodeString(HubSymbolicLinkName, NULL);

        if ((PdoContext->AssociatedDSM != NULL) &&
            (PdoContext->AssociatedDSM->SymbolicLinkName != NULL)) {

            WdfStringGetUnicodeString(PdoContext->AssociatedDSM->SymbolicLinkName,
                                      HubSymbolicLinkName);
        }
    } FINALLY {
    }
}


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBPDO_ParentInterfaceWasHubResetOnResume (
    __in
        PHUB_PDO_CONTEXT        PdoContext
    )
{
    BOOLEAN             retValue;

    if (PdoContext->PdoFlags.ResetOnLastResume == 1) {
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &PdoContext->PdoFlags, ResetOnLastResume);
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &PdoContext->PdoFlags, ReportPortAsDisabled);
        retValue = TRUE;
    } else {
        retValue = FALSE;
    }

    return retValue;
}


__drv_functionClass(EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST)
NTSTATUS
HUBPDO_EvtDeviceProcessQueryInterfaceRequest (
    __in
        WDFDEVICE   Device,
    __in
        LPGUID      InterfaceType,
  __inout
        PINTERFACE  ExposedInterface,
  __inout
        PVOID       ExposedInterfaceSpecificData
    )
/*++

Routine Description:

    Event callback when hub FDO performs its
    query. We get and store the FdoContext here,
    to help the kd extension parse the tree.

Return Value:

    VOID

--*/
{
    PDEVICE_CONTEXT         deviceContext;
    PHUB_PARENT_INTERFACE   hubParentInterface;
    PHUB_PDO_CONTEXT        hubPdoContext;
    NTSTATUS                status;

    UNREFERENCED_PARAMETER(InterfaceType);
    UNREFERENCED_PARAMETER(ExposedInterfaceSpecificData);

    TRY {
        hubParentInterface = (PHUB_PARENT_INTERFACE) ExposedInterface;

        hubPdoContext = GetHubPdoContext(Device);
        deviceContext = hubPdoContext->AssociatedDSM;

        //
        // If the child is not a hub, it should not be sending
        // us this QI.
        //
        if (deviceContext->DeviceFlags.DeviceIsHub == 0) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failing QI for the PDO 0x%p as it is not a hub",
                       hubPdoContext);

            status = STATUS_UNSUCCESSFUL;

            LEAVE;

        }


        status = STATUS_SUCCESS;

        //
        // Since,it is a two way interface, KMDF will not
        // automatically fill in this information for us.
        // We are responsible for filling it up every time
        // a query comes
        //

        hubParentInterface->Size = sizeof(HUB_PARENT_INTERFACE);
        hubParentInterface->Version = HUB_PARENT_INTERFACE_VERSION_LATEST;
        hubParentInterface->InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
        hubParentInterface->InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
        hubParentInterface->Context = hubPdoContext;
        hubParentInterface->HubDepth = deviceContext->HubFdoContext->HubParentInterface.HubDepth + 1;
        hubParentInterface->IsHubWakeCapable = (deviceContext->DeviceFlags.UsbWakeupSupport == 1) ? TRUE:FALSE;

        hubParentInterface->HubTopologyAddress = deviceContext->HubFdoContext->HubParentInterface.HubTopologyAddress;

        if (hubParentInterface->HubDepth == 1) {
            //
            // Attached to root hub
            //
            hubParentInterface->HubTopologyAddress.RootHubPortNumber =
                deviceContext->PortContext->PortInformation.PortNumber;

        }  else if (hubParentInterface->HubDepth > 1) {
            //
            // External hub
            //
            hubParentInterface->HubTopologyAddress.HubPortNumber[hubParentInterface->HubDepth-2] =
                deviceContext->PortContext->PortInformation.PortNumber;
        }

        hubParentInterface->HubSpeed = deviceContext->UsbDeviceInfo.DeviceSpeed;
        hubParentInterface->IsEnhancedSuperSpeed = (deviceContext->DeviceFlags.DeviceIsEnhancedSuperSpeedCapable == 1) ? TRUE : FALSE;
        hubParentInterface->Hub = deviceContext->UsbDevice;
        hubParentInterface->WasDeviceProgrammingLostOnResume =
            HUBPDO_ParentInterfaceWasDeviceProgrammingLostOnResume;
        hubParentInterface->WasHubResetOnResume = HUBPDO_ParentInterfaceWasHubResetOnResume;
        hubParentInterface->ConnectorId = (PVOID)&deviceContext->PortContext->ConnectorId;
        hubParentInterface->GetHubSymbolicLinkName = HUBPDO_GetHubSymbolicLinkName;

        if (deviceContext->PortContext->PortInformation.PortFlags.PortFlagIntegratedHubImplemented == 1) {

            hubParentInterface->First30PortToMap =
                deviceContext->PortContext->PortInformation.FirstMapped30PortNumber;

            hubParentInterface->Last30PortToMap =
                deviceContext->PortContext->PortInformation.LastMapped30PortNumber;

            if (hubPdoContext->HubFdoContext->HubInformation.HubFlags.DisableSelectiveSuspendForIntegratedHub == 1) {
                hubParentInterface->IsHubWakeCapable = FALSE;
            }
        }

        //
        // Store the Fdo context of the child, for debugging
        //
        hubPdoContext->ChildHubFdoContext = hubParentInterface->HubFdoContext;

        DbgTraceTo(deviceContext->PortContext->IfrLog,
                   TL_INFO,
                   Device,
                   "Device 0x%p is managing the pdo for Hub 0x%p",
                   hubPdoContext->AssociatedDSM,
                   hubPdoContext->ChildHubFdoContext);

    } FINALLY {

    }

    return status;

}

__drv_functionClass(SET_D3COLD_SUPPORT)
_IRQL_requires_(PASSIVE_LEVEL)
VOID
HUBPDO_D3ColdSupportInterfaceSetD3ColdSupport (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID           Context,
    __in
        BOOLEAN         D3ColdSupport
    )
/*++

Routine Description:

    This routine toggles D3cold support for this device.

    This routine isn't paged, even though it's marked PASSIVE_LEVEL, so that
    drivers on the paging path can call through this at any time.

Arguments:

    Context

    D3ColdSupport - Indicates whether D3cold should be supported or not.

Return Value:

    None

--*/
{
    PHUB_PDO_CONTEXT            hubPdoContext;

    _Analysis_assume_(Context != NULL);

    TRY {

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;

        //
        // Let filters know that this is changing.
        //

        if (hubPdoContext->D3ColdFilter.SetD3ColdSupport != NULL) {
            hubPdoContext->D3ColdFilter.SetD3ColdSupport(hubPdoContext->D3ColdFilter.Context,
                                                         D3ColdSupport);
        }

        //
        // Now change the internal state
        //
        if (D3ColdSupport) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &hubPdoContext->AssociatedDSM->DeviceStateFlags, D3ColdEnabledByFunctionDriver);
        } else {
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &hubPdoContext->AssociatedDSM->DeviceStateFlags, D3ColdEnabledByFunctionDriver);
        }

    } FINALLY {

    }

}

VOID
HUBPDO_D3ColdSupportInterfaceReference (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID           Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

Return Value:

    None

--*/
{
    PHUB_PDO_CONTEXT            hubPdoContext;

    TRY {

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;

        //
        // We are not interested in this function. Just call the filter
        //

        if (hubPdoContext->D3ColdFilter.InterfaceReference != NULL) {
            hubPdoContext->D3ColdFilter.InterfaceReference(hubPdoContext->D3ColdFilter.Context);
        }


    } FINALLY {

    }

}

VOID
HUBPDO_D3ColdSupportInterfaceDereference (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID           Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:


Return Value:

    None

--*/
{
    PHUB_PDO_CONTEXT            hubPdoContext;

    TRY {

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;

        //
        // We are not interested in this function. Just call the filter
        //

        if (hubPdoContext->D3ColdFilter.InterfaceDereference != NULL) {
            hubPdoContext->D3ColdFilter.InterfaceDereference(hubPdoContext->D3ColdFilter.Context);
        }


    } FINALLY {

    }

}

__drv_functionClass(GET_IDLE_WAKE_INFO)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_D3ColdSupportInterfaceGetIdleWakeInfo (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID                   Context,
    __in
        SYSTEM_POWER_STATE      SystemPowerState,
    __out
        PDEVICE_WAKE_DEPTH      DeepestWakeableDstate
    )
/*++

Routine Description:

    This routine allows a driver to ask the question "what's the deepest D-state
    from which my device can trigger its wake signal, in each S-state?"  For
    now, only S0 is interesting.

Arguments:

    SystemPowerState - The system power state about which the query applies.

    DeepestWakeableDstate - Out parameter indicating the lowest D-state which
        can deliver the wake signal.

Return Value:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT            hubPdoContext;
    NTSTATUS                    status;

    TRY {

        PAGED_CODE();

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;

        //
        // First check if ACPI has any information. If so, then listen to it.
        //
        if (hubPdoContext->D3ColdFilter.GetIdleWakeInfo != NULL) {
            status = hubPdoContext->D3ColdFilter.GetIdleWakeInfo(hubPdoContext->D3ColdFilter.Context,
                                                                 SystemPowerState,
                                                                 DeepestWakeableDstate);
            if (NT_SUCCESS(status)) {
                LEAVE;
            }
        }
        //
        // If ACPI does not provide any information, then we will provide it
        // based on the wake capaiblity reported by the device and wake property
        // inherited from the parent. If it is a hub, then we always declare
        // it as wake capable. The is because the hub should always  be able
        // to propogate wake for its downstrream devices whether it itself
        // wake capable or not.At the FDO level, we will explicitly check
        // whether the hub is really wake capable and only if it is,
        // we will setup Selective Suspend for it
        //
        if ((hubPdoContext->HubFdoContext->DeviceCapabilities.SystemWake >= SystemPowerState) &&
            ((hubPdoContext->AssociatedDSM->DeviceFlags.UsbWakeupSupport == 1) ||
             (hubPdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 1))) {

            *DeepestWakeableDstate = DeviceWakeDepthD2;
        } else {
            *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        }
        status = STATUS_SUCCESS;


    } FINALLY {

    }
    return status;
}

__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_D3ColdSupportInterfaceGetD3ColdCapability (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID               Context,
    __out
        PBOOLEAN            D3ColdSupported
    )
/*++

Routine Description:

    This routine allows a driver to ask if there is a possibility
    that device might enter D3Cold

Arguments:

    D3ColdSupported - Out parameter indicating whether this device might enter
        D3cold.

Return Value:

    NTSTATUS

--*/
{
    PHUB_PDO_CONTEXT            hubPdoContext;
    NTSTATUS                    status;

    TRY {

        PAGED_CODE();

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;

        //
        // If ACPI is present, we will let ACPI make the call
        //
        if (hubPdoContext->D3ColdFilter.GetD3ColdCapability != NULL) {
            status = hubPdoContext->D3ColdFilter.GetD3ColdCapability(hubPdoContext->D3ColdFilter.Context,
                                                                     D3ColdSupported);
            if (NT_SUCCESS(status) &&
                (*D3ColdSupported)) {

                if (hubPdoContext->AssociatedDSM->PortContext->PortInformation.PortFlags.PortFlagRemovable != 0) {
                    DbgTraceTo(hubPdoContext->HubFdoContext->IfrLog,
                               TL_ERROR,
                               HwcPlatform,
                               "HW_COMPLIANCE_PLATFORM: ACPI returned D3ColdSupported for a removable device");
                    
                    NT_ASSERTMSG("ACPI returned true for GetD3ColdCapability for a removable device", FALSE);
                }
            }

            LEAVE;
        }

        //
        // If there is no ACPI, then we don't support D3cold
        //
        *D3ColdSupported = FALSE;
        status = STATUS_SUCCESS;

    } FINALLY {

    }
    return status;
}

__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_D3ColdSupportInterfaceGetD3ColdBusDriverSupport (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID               Context,
    __out
        PBOOLEAN            D3ColdSupported
    )
/*++

Routine Description:

    This routine allows a driver to ask the question "does the underlying bus
    driver support the concept of D3cold?"  At present, ACPI uses this for
    deciding whether to expose D3cold support, as it shouldn't do so unless the
    bus driver does also.  (This decision is taken because we expect our XHCI
    USB 3.0 stack to support D3cold but we don't expect all the third party
    replacements for it to do so.)

Arguments:

    D3ColdSupported - Out parameter indicating whether this device might enter
        D3cold.

Return Value:


--*/
{

    PHUB_PDO_CONTEXT            hubPdoContext;
    NTSTATUS                    status;

    TRY {

        PAGED_CODE();

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;
        //
        // Devices that can be removed should not be sent to D3 because
        // we cannot differentiate between device going to D3Cold Vs.
        // device getting removed
        //
        if (hubPdoContext->AssociatedDSM->PortContext->PortInformation.PortFlags.PortFlagRemovable == 1) {
            *D3ColdSupported = FALSE;
        } else {
            *D3ColdSupported = TRUE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }
    return status;
}

__drv_functionClass(GET_D3COLD_LAST_TRANSITION_STATUS)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
HUBPDO_D3ColdSupportInterfaceGetLastTransitionStatus (
    _In_reads_opt_(_Inexpressible_("varies"))
        PVOID                           Context,
    __out
        PD3COLD_LAST_TRANSITION_STATUS  LastTransitionStatus
    )
/*++

Routine Description:

    This routine allows a driver to query if the last transition
    to D3 was hot or cold

Arguments:

    LastTransitionStatus - Out parameter indicating whether this device
                            entered D3cold.

Return Value:


--*/
{

    PHUB_PDO_CONTEXT                hubPdoContext;

    TRY {

        hubPdoContext = (PHUB_PDO_CONTEXT)Context;
        *LastTransitionStatus = LastDStateTransitionD3hot;

        //
        // First check if ACPI has any information
        //
        if (hubPdoContext->D3ColdFilter.GetLastTransitionStatus != NULL) {
            hubPdoContext->D3ColdFilter.GetLastTransitionStatus(hubPdoContext->D3ColdFilter.Context,
                                                                LastTransitionStatus);

            if (*LastTransitionStatus == LastDStateTransitionD3cold) {

                NT_ASSERTMSG("ACPI thinks that the device went to D3Cold but USB state indicates that device was not re-enumerated",
                             (hubPdoContext->PdoFlags.ResetOnLastResume == 1) || (hubPdoContext->FailIo == 1));
                LEAVE;
            }
        }
        //
        // Even if the device did not really go to D3 cold but
        // got re-enumerated, then we will return D3 cold because
        // the device state was lost, which is what the client
        // driver is interested in.
        //
        if (hubPdoContext->PdoFlags.ResetOnLastResume == 1) {
            *LastTransitionStatus = LastDStateTransitionD3cold;
        }

    } FINALLY {

    }
}



__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_MakePdoName(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit,
    __in
        ULONG               Index
    )
/*++

Routine Description:

    creates a unicode name string for a PDO associated with a hub. The string
    is of the format:
        USBPDO-n  where n is the Value of Index.


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS        status;

    DECLARE_UNICODE_STRING_SIZE(pdoName, sizeof(L"\\Device\\USBPDO-") + 32*sizeof(WCHAR));

    UNREFERENCED_PARAMETER(DeviceContext);

    PAGED_CODE ();

    TRY {

        status = RtlUnicodeStringPrintf(&pdoName,
                                        L"\\Device\\USBPDO-%d",
                                        Index);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Building PDO name string failed %!STATUS!",
                       status);
            LEAVE;
        }

        status = WdfDeviceInitAssignName(DeviceInit,
                                         &pdoName);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfDeviceInitAssignName Failed %!STATUS!", status );
            LEAVE;
        }


    } FINALLY {


    }

    return status;

}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_AssignPDOIds(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit
    )
/*++

Routine Description:

    Build the ContainerID, DeviceID, and InstanceID strings
    and adds them to the DeviceInit structure

Return Value:

    NTSTATUS

--*/
{

    NTSTATUS            status;

    //
    // We need to decorate hub serial numbers, so we need to add some characters for that.
    //
    DECLARE_UNICODE_STRING_SIZE(uniqueIdString, MAXIMUM_UNIQUE_ID_STRING_LENGTH + USB_SERIAL_NUMBER_DECORATION_SIZE);

    PAGED_CODE ();

    TRY {

        HUBREG_UpdateUxdSettings(DeviceContext->HubFdoContext,
                                 DeviceContext,
                                 NULL);

        if (DeviceContext->DeviceFlags.DeviceReservedUXD == 1) {

            NT_ASSERT(DeviceContext->UxdId.Buffer == NULL);

            HUBID_BuildUxdPnpId(DeviceContext, &DeviceContext->UxdId);
        }

        status = HUBID_BuildDeviceID(DeviceContext,
                                     DeviceInit,
                                     NULL);
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HUBID_BuildDeviceID Failed %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBID_BuildHardwareID(DeviceContext,
                                       DeviceInit,
                                       NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HUBID_BuildHardwareID Failed %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBID_BuildCompatibleID(DeviceContext,
                                         DeviceInit,
                                         NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HUBID_BuildCompatibleID Failed %!STATUS!",
                       status);
            LEAVE;
        }

        HUBID_BuildContainerID(DeviceContext,
                               DeviceInit);

        //
        // Now lets tell WDF about the instance ID
        //


        if (DeviceContext->DeviceFlags.SerialNumber == 1) {

            if (DeviceContext->DeviceFlags.DeviceIsHub == 0) {

                //
                // We always truncate the serial numbers to 16 characters, so we need to
                // adjust our MaximumLength manually here to remove the buffer we added to decorate
                // hub serial numbers.
                //
                uniqueIdString.MaximumLength = MAXIMUM_UNIQUE_ID_STRING_LENGTH * sizeof(WCHAR);

            }

            RtlInitUnicodeString(&uniqueIdString,
                                 DeviceContext->SerialNumberId.Buffer);

            status = WdfPdoInitAssignInstanceID(DeviceInit,
                                                &uniqueIdString);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfPdoInitAssignInstanceID Failed %!STATUS!",
                       status);
                LEAVE;
            }

        } else {
            USHORT portNumber;

            if ((DeviceContext->PortContext->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 1) &&
                (DeviceContext->PortContext->PortInformation.PortNumberForInstanceId != 0) &&
                (DeviceContext->DeviceStateFlags.DeviceIsKnown == 1)) {
                portNumber = DeviceContext->PortContext->PortInformation.PortNumberForInstanceId;
            } else {
                portNumber = DeviceContext->PortContext->PortInformation.PortNumber;
            }
            
            status = RtlIntegerToUnicodeString(portNumber,
                                               10,
                                               &uniqueIdString);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to convert port number to unicode string %!STATUS!",
                       status);
                LEAVE;
            }

            status = WdfPdoInitAssignInstanceID(DeviceInit,
                                                &uniqueIdString);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfPdoInitAssignInstanceID Failed %!STATUS!",
                       status);
                LEAVE;
            }

        }


        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;

}

VOID
HUBPDO_RegisterForPowerSettingsForDevice (
    __in
        PHUB_PDO_CONTEXT        PdoContext
    )
/*++

Routine Description:

    Register for the various LPM related power settings
    for the PDO. Failure in registration is ignored

Return Value:


--*/
{
    ULONG       i;
    NTSTATUS    status;
    LPCGUID LPMPowerSettingsGuids[LPM_SETTINGS_COUNT] = {
        &GUID_POWER_USB_U1_ENABLE_FOR_DEVICES,
        &GUID_POWER_USB_U2_ENABLE_FOR_DEVICES,
        &GUID_POWER_USB_U1_TIMEOUT_FOR_DEVICES,
        &GUID_POWER_USB_U2_TIMEOUT_FOR_DEVICES,
        &GUID_POWER_USB_3_LINK_POWER_MANAGEMENT_POLICY
    };


    TRY {

        for (i = 0 ;i < LPM_SETTINGS_COUNT ; i++) {

            status = PoRegisterPowerSettingCallback(WdfDeviceWdmGetDeviceObject(WdfObjectContextGetObject(PdoContext)),
                                                    LPMPowerSettingsGuids[i],
                                                    HUBPDO_PowerSettingCallback,
                                                    PdoContext->AssociatedDSM,
                                                    &PdoContext->LpmPowerSettingsHandles[i]);
            if (!NT_SUCCESS(status)) {

                DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "PoRegisterPowerSettingCallback (device) Failed %!STATUS!",
                       status);

                //
                // Ignore the error
                //
                status = STATUS_SUCCESS;
                PdoContext->LpmPowerSettingsHandles[i] = NULL;

            }
        }


    } FINALLY {

    }

    return;

}

VOID
HUBPDO_RegisterForPowerSettingsForHub (
    __in
        PHUB_PDO_CONTEXT        PdoContext
    )
/*++

Routine Description:

    Register for the various LPM related power settings
    for the PDO. Failure in registration is ignored

Return Value:


--*/
{
    ULONG       i;
    NTSTATUS    status;
    LPCGUID LPMPowerSettingsGuids[LPM_SETTINGS_COUNT] = {
        &GUID_POWER_USB_U1_ENABLE_FOR_HUBS,
        &GUID_POWER_USB_U2_ENABLE_FOR_HUBS,
        &GUID_POWER_USB_U1_TIMEOUT_FOR_HUBS,
        &GUID_POWER_USB_U2_TIMEOUT_FOR_HUBS,
        &GUID_POWER_USB_3_LINK_POWER_MANAGEMENT_POLICY
    };


    TRY {

        for (i = 0 ;i < LPM_SETTINGS_COUNT ; i++) {

            status = PoRegisterPowerSettingCallback(WdfDeviceWdmGetDeviceObject(WdfObjectContextGetObject(PdoContext)),
                                                    LPMPowerSettingsGuids[i],
                                                    HUBPDO_PowerSettingCallback,
                                                    PdoContext->AssociatedDSM,
                                                    &PdoContext->LpmPowerSettingsHandles[i]);
            if (!NT_SUCCESS(status)) {

                DbgTraceTo(PdoContext->AssociatedDSM->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "PoRegisterPowerSettingCallback (hub) Failed %!STATUS!",
                       status);

                //
                // Ignore the error
                //
                status = STATUS_SUCCESS;
                PdoContext->LpmPowerSettingsHandles[i] = NULL;

            }
        }


    } FINALLY {

    }

    return;

}


NTSTATUS
HUBPDO_GetLocationString(
    __in
        PVOID       Context,
    __deref_out
        PZZWSTR     *LocationStrings
    )
/*++

Routine Description:

    This routine allocates, fills in, and returns a Multi-Sz string containing the bus-relative
    location identifier string for the given device.

    The location string for a USB device is "USB(X)" where X is the USB hub port number to which
    the device is immediately connected.

    This interface is permitted to return a Multi-Sz containing multiple strings describing the
    same device, but in this implementation only the single strings listed above will be
    returned from the interface.  The string must still be in the format of a Multi-Sz however,
    meaning a double-NULL terminator is required.

Arguments:

    Context - Pointer to a DEVICE_CONTEXT structure.

    LocationsStrings - Address of a pointer to a multi-string allocated and populated by this function.

Return Value:

    NTSTATUS code.

--*/
{
    PDEVICE_CONTEXT     deviceContext;
    size_t              remainingChars;
    PWCHAR              stringBuffer;
    NTSTATUS            status;

    TRY {

        *LocationStrings = NULL;

        deviceContext = Context;

        status = STATUS_SUCCESS;

        stringBuffer = ExAllocatePoolWithTag(PagedPool,
                                             LOCATION_INTERFACE_DEVICE_STRING_COUNT*sizeof(WCHAR),
                                             USBHUB3_DEVICE_TAG);

        if (stringBuffer == NULL) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate location strings %!STATUS!",
                       status);

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // The location string for a USB device is "USB(X)" where X is the USB hub port number to
        // which the device is immediately connected.
        //
        // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused portion of the buffer is
        // filled with 0s which null terminates the mult-string
        //
        status = RtlStringCchPrintfExW(stringBuffer,
                                       LOCATION_INTERFACE_DEVICE_STRING_COUNT,
                                       NULL,
                                       &remainingChars,
                                       STRSAFE_FILL_BEHIND_NULL,
                                       L"USB(%d)",
                                       deviceContext->PortContext->PortInformation.PortNumber);

        NT_ASSERT(NT_SUCCESS(status));

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "RtlStringCchPrintfExW failed to format location string %!STATUS!",
                       status);
        }

        //
        // Insure there is room for 2 terminating NULLs
        //
        NT_ASSERT(remainingChars >= 2);

        *LocationStrings = stringBuffer;

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
DSM_EVENT
HUBPDO_CreatePdoInternal(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        BOOLEAN             KnownDevice
    )
/*++

Routine Description:

    Calls WdfDeviceCreate to create the WDF device object
    for the child device.

Arguments

    KnownDevice - indicates whether we should create the real PDO
            or the "unknown" PDO

Return Value:

    DsmEventOperationFailure, DsmEventOperationSuccess

--*/
{

    WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;
    WDF_PDO_EVENT_CALLBACKS         pdoEventCallbacks;
    WDF_OBJECT_ATTRIBUTES           attributes;
    NTSTATUS                        status;
    WDFDEVICE                       device;
    DSM_EVENT                       result;
    PHUB_PDO_CONTEXT                pdoContext;
    WDF_IO_QUEUE_CONFIG             queueConfig;
    WDFQUEUE                        queue;
    PWDFDEVICE_INIT                 deviceInit;
    BOOLEAN                         freeDeviceInit;
    BOOLEAN                         deleteDeviceObject;
    BOOLEAN                         releaseDsmReference;
    ULONG                           nameIndex = 0;
    UCHAR                           minorFunctionArray[9];
    WDF_DEVICE_PNP_CAPABILITIES     pnpCaps;
    WDF_DEVICE_POWER_CAPABILITIES   powerCaps;
    HUB_PARENT_INTERFACE            hubparentInterface;
    PNP_LOCATION_INTERFACE          pnpLocationInterface;
    WDF_QUERY_INTERFACE_CONFIG      queryInterfaceConfig;
    WDF_WORKITEM_CONFIG             workitemConfig;
    WDF_REMOVE_LOCK_OPTIONS         removelockOptions;
    PWAKE_NOTIFICATION_REQUEST_CONTEXT
                                    wakeNotificationRequestContext;

    PAGED_CODE ();

    TRY {

        deleteDeviceObject = FALSE;
        freeDeviceInit = FALSE;
        releaseDsmReference = FALSE;
        device = NULL;

        if (KnownDevice) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceIsKnown);
        } else {
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DeviceIsKnown);
        
            //
            // If a device is internal (hub is described in ACPI and port is
            // marked as non-removable), we should supress the popup UI to
            // avoid confusing the user.  
            //
            if (!((DeviceContext->HubFdoContext->HubInformation.HubFlags.InAcpiNamespace == 1) &&
                  (DeviceContext->DeviceFlags.DeviceNotRemovable == 1))) {
            
                WMI_FireNotification(DeviceContext->HubFdoContext,
                                     DeviceContext->PortContext->PortInformation.PortNumber,
                                     EnumerationFailure);
            }
        }

        deviceInit = WdfPdoInitAllocate(WdfObjectContextGetObject(DeviceContext->HubFdoContext));

        if (deviceInit == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        freeDeviceInit = TRUE;

        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
        pnpPowerCallbacks.EvtDeviceD0Entry              = HUBPDO_EvtDeviceD0Entry;
        pnpPowerCallbacks.EvtDeviceD0Exit               = HUBPDO_EvtDeviceD0Exit;
        pnpPowerCallbacks.EvtDevicePrepareHardware      = HUBPDO_EvtDevicePrepareHardware;
        pnpPowerCallbacks.EvtDeviceReleaseHardware      = HUBPDO_EvtDeviceReleaseHardware;
        pnpPowerCallbacks.EvtDeviceSurpriseRemoval      = HUBPDO_EvtDeviceSurpriseRemoval;
        pnpPowerCallbacks.EvtDeviceSelfManagedIoInit    = HUBPDO_EvtDeviceSelfManagedIoInit;
        pnpPowerCallbacks.EvtDeviceSelfManagedIoRestart = HUBPDO_EvtDeviceSelfManagedIoRestart;
        pnpPowerCallbacks.EvtDeviceSelfManagedIoSuspend = HUBPDO_EvtDeviceSelfManagedIoSuspend;
        pnpPowerCallbacks.EvtDeviceUsageNotificationEx  = HUBPDO_EvtDeviceUsageNotificationEx;
        pnpPowerCallbacks.EvtDeviceQueryStop            = HUBPDO_EvtDeviceQueryStop;
        pnpPowerCallbacks.EvtDeviceQueryRemove          = HUBPDO_EvtDeviceQueryRemove;

        WdfDeviceInitSetPnpPowerEventCallbacks(deviceInit,
                                               &pnpPowerCallbacks);

        WDF_PDO_EVENT_CALLBACKS_INIT (&pdoEventCallbacks);
        pdoEventCallbacks.EvtDeviceEnableWakeAtBus  = HUBPDO_EvtDeviceEnableWakeAtBus;
        pdoEventCallbacks.EvtDeviceDisableWakeAtBus = HUBPDO_EvtDeviceDisableWakeAtBus;
        pdoEventCallbacks.EvtDeviceReportedMissing  = HUBPDO_EvtDeviceReportedMissing;
        pdoEventCallbacks.EvtDeviceResourceRequirementsQuery   = HUBPDO_EvtDeviceResourceRequirementsQuery;

        WdfPdoInitSetEventCallbacks (deviceInit,
                                     &pdoEventCallbacks);

        WdfDeviceInitSetDeviceType(deviceInit,
                                   FILE_DEVICE_UNKNOWN);

        WdfDeviceInitSetExclusive(deviceInit,
                                  FALSE);


        status = WdfDeviceInitAssignWdmIrpPreprocessCallback(deviceInit,
                                                             HUBPDO_EvtDeviceWdmIrpPreprocess,
                                                             IRP_MJ_INTERNAL_DEVICE_CONTROL,
                                                             NULL,
                                                             0);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfDeviceInitAssignWdmIrpPreprocessCallback Failed For IO %!STATUS!",
                       status);

            LEAVE;
        }

        //
        // Due to WDF restriction of not allowing us to change interface information,
        // we need to register a pre-process callback for it.
        //
        minorFunctionArray[0] =  IRP_MN_QUERY_INTERFACE;
        //
        // Since we want to support query device text for languages, other than English,
        // but don;t want to query and cache all the product Ids in advaance, we will
        // setup a pre-process routine for query text
        //
        minorFunctionArray[1] =  IRP_MN_QUERY_DEVICE_TEXT;

        //
        // We want to NULL out the child hub fdo context on the first remove
        //
        minorFunctionArray[2] = IRP_MN_REMOVE_DEVICE;

        //
        // Any problem description string needs to be reported after the device has enumerated
        //
        minorFunctionArray[3] = IRP_MN_DEVICE_ENUMERATED;

        //
        // Following PNP IRPs have to post-processed to keep their completion results
        // consistent with EHCI stack.
        //
        minorFunctionArray[4] = IRP_MN_START_DEVICE;
        minorFunctionArray[5] = IRP_MN_SURPRISE_REMOVAL;
        minorFunctionArray[6] = IRP_MN_QUERY_RESOURCE_REQUIREMENTS;
        minorFunctionArray[7] = IRP_MN_QUERY_PNP_DEVICE_STATE;
        //
        // We need to find out when the device is about to be started
        //
        minorFunctionArray[8] = IRP_MN_QUERY_ID;


        status = WdfDeviceInitAssignWdmIrpPreprocessCallback(deviceInit,
                                                             HUBPDO_EvtDeviceWdmIrpPnPPowerPreprocess,
                                                             IRP_MJ_PNP,
                                                             minorFunctionArray,
                                                             ARRAY_SIZE(minorFunctionArray));

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "HUBPDO_EvtDeviceWdmIrpPnPPreprocess Failed For PnP %!STATUS!", status );
            LEAVE;

        }

        minorFunctionArray[0] = IRP_MN_SET_POWER;

        status = WdfDeviceInitAssignWdmIrpPreprocessCallback(deviceInit,
                                                             HUBPDO_EvtDeviceWdmIrpPnPPowerPreprocess,
                                                             IRP_MJ_POWER,
                                                             minorFunctionArray,
                                                             1);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "HUBPDO_EvtDeviceWdmIrpPnPPreprocess Failed For Power %!STATUS!", status );
            LEAVE;

        }

        //
        // Need to call this in order to increment the stack count
        // as opposed to resetting it.
        //
        WdfPdoInitAllowForwardingRequestToParent(deviceInit);

        //
        // Some client drivers send I/O to the stack after the remove device by
        // keeping a reference on the PDO they are attached to. Framework cannot
        // handle I/Os after remove gracefully. So to ensure compat, we will inform
        // framework to hold remlock in the I/O path so it can fail I/Os received
        // after remove device is processed.
        //
        WDF_REMOVE_LOCK_OPTIONS_INIT(&removelockOptions,
                                     WDF_REMOVE_LOCK_OPTION_ACQUIRE_FOR_IO);

        WdfDeviceInitSetRemoveLockOptions(deviceInit, &removelockOptions);

        status = HUBPDO_AssignPDOIds(DeviceContext,
                                     deviceInit);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "HUBPDO_AssignPDOIds Failed %!STATUS!",
                       status);
            LEAVE;

        }

        if (DeviceContext->DeviceStateFlags.DeviceIsKnown == 0) {

            //
            // Setting RawDeviceOK will allow devices that failed enumeration and loaded
            // the NULL driver to still start.  This is necessary so the device can be
            // invalidated via IoInvalidateDeviceState
            //

            status = WdfPdoInitAssignRawDevice(deviceInit,
                                               &GUID_DEVCLASS_UNKNOWN);


            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_WARNING,
                           Driver,
                           "WdfPdoInitAssignRawDevice Failed %!STATUS!",
                           status);
            }

            NT_ASSERT(NT_SUCCESS(status));

        }

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes,
                                                HUB_PDO_CONTEXT);

        attributes.EvtCleanupCallback = HUBPDO_EvtDeviceCleanup;

        //attributes.EvtDestroyCallback = HUBPDO_EvtDeviceDestroy;

        //
        // PDO will take a reference on the DSM Object, to make sure
        // that DSM is there to handle any request coming from client
        // If the device creation succeeds, this reference will be
        // released in HUBPDO_EvtDeviceCleanup. If it fails, we will
        // release the reference right here.
        //
        WdfObjectReferenceWithTag(WdfObjectContextGetObject(DeviceContext),
                                  (PVOID) PDOTag);

        releaseDsmReference = TRUE;

        do {
            //
            // Create the PDO name
            //

            status = HUBPDO_MakePdoName(DeviceContext,
                                        deviceInit,
                                        nameIndex);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Driver,
                           "HUBPDO_MakePdoName Failed %!STATUS!",
                           status);
                LEAVE;

            }

            //
            // KMDF expects SDDL to create named device objects. The SDDL provided here matches
            // with legacy stack.
            //
            status = WdfDeviceInitAssignSDDLString(deviceInit,
                                                   &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Driver,
                           "WdfDeviceInitAssignSDDLString Failed %!STATUS!",
                           status);
                LEAVE;

            }

            status = WdfDeviceCreate(&deviceInit,
                                     &attributes,
                                     &device);

            if (NT_SUCCESS(status)) {

                deleteDeviceObject = TRUE;
                //
                // Device creation succeeded. So the Cleanup routine is guaranteed to be invoked.
                // Hence we don't need to worry about releasing the DSM reference here anymore.
                //
                releaseDsmReference = FALSE;
                break;

            }

            nameIndex++;

        } while (status == STATUS_OBJECT_NAME_COLLISION);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfDeviceCreate Failed %!STATUS!",
                       status);
            LEAVE;

        }

        NT_ASSERT(deleteDeviceObject == TRUE);
        NT_ASSERT(device != NULL);

        if (DeviceContext->DeviceFlags.DeviceIsHub == 1) {
            //
            // Add the hub/parent bus interface
            //
            RtlZeroMemory(&hubparentInterface, sizeof(hubparentInterface));

            hubparentInterface.Size = sizeof(hubparentInterface);
            hubparentInterface.Version = HUB_PARENT_INTERFACE_VERSION_LATEST;
            hubparentInterface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
            hubparentInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
            hubparentInterface.HubDepth = DeviceContext->HubFdoContext->HubParentInterface.HubDepth + 1;
            hubparentInterface.HubSpeed = DeviceContext->UsbDeviceInfo.DeviceSpeed;
            hubparentInterface.IsEnhancedSuperSpeed = (DeviceContext->DeviceFlags.DeviceIsEnhancedSuperSpeedCapable == 1) ? TRUE : FALSE;
            hubparentInterface.Hub = DeviceContext->UsbDevice;
            hubparentInterface.HubFdoContext = NULL;

            WDF_QUERY_INTERFACE_CONFIG_INIT(&queryInterfaceConfig,
                                            (PINTERFACE)&hubparentInterface,
                                            &GUID_HUB_PARENT_INTERFACE,
                                            NULL);

            queryInterfaceConfig.ImportInterface = TRUE;
            queryInterfaceConfig.EvtDeviceProcessQueryInterfaceRequest = HUBPDO_EvtDeviceProcessQueryInterfaceRequest;

            status = WdfDeviceAddQueryInterface(device, &queryInterfaceConfig);

            if (!NT_SUCCESS(status)){

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Driver,
                           "WdfDeviceAddQueryInterface for GUID_HUB_PARENT_INTERFACE failed %!STATUS!",
                           status);

                WdfObjectDelete(device);
                freeDeviceInit = TRUE;

                LEAVE;
            }
        }

        //
        // Add the PnP location interface
        //
        RtlZeroMemory(&pnpLocationInterface, sizeof(pnpLocationInterface));

        pnpLocationInterface.Size = sizeof(pnpLocationInterface);
        pnpLocationInterface.Version = LOCATION_INTERFACE_VERSION;
        pnpLocationInterface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
        pnpLocationInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
        pnpLocationInterface.Context = DeviceContext;
        pnpLocationInterface.GetLocationString = HUBPDO_GetLocationString;

        WDF_QUERY_INTERFACE_CONFIG_INIT(&queryInterfaceConfig,
                                        (PINTERFACE)&pnpLocationInterface,
                                        &GUID_PNP_LOCATION_INTERFACE,
                                        NULL);

        status = WdfDeviceAddQueryInterface(device, &queryInterfaceConfig);

        if (!NT_SUCCESS(status)){

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfDeviceAddQueryInterface for GUID_PNP_LOCATION_INTERFACE failed %!STATUS!",
                       status);

            WdfObjectDelete(device);
            freeDeviceInit = TRUE;

            LEAVE;
        }

        //
        // If WdfDeviceCreate succeeds, then we shouldn't free the WDFDEVICE_INIT
        // structure
        //
        freeDeviceInit = FALSE;

        //
        // Indicate special file support
        //
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFilePaging,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileHibernation,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileDump,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileBoot,
                                       TRUE);

        pdoContext = GetHubPdoContext(device);

        pdoContext->AssociatedDSM = DeviceContext;
        pdoContext->HubFdoContext = DeviceContext->HubFdoContext;
        pdoContext->PortNumber = DeviceContext->PortContext->PortInformation.PortNumber;
        //
        // To begin with, pdo is logically in a D3Final state
        //
        pdoContext->CurrentWdfPowerDeviceState = WdfPowerDeviceD3Final;

        pdoContext->USBDClientContractVersion = USBD_CLIENT_CONTRACT_VERSION_INVALID;

        if (KnownDevice) {
            pdoContext->TargetDeviceObject = DeviceContext->HubFdoContext->HubInfoFromParent.IoTarget;
        } else {
            //
            // For unknown devices, we don't expect any IO from client drivers. But we
            // might get IO from bus filters that we need to fail at the hub layer.
            //
            pdoContext->FailIo = TRUE;
        }

        DeviceContext->PdoContext = pdoContext;

        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig,
                                               WdfIoQueueDispatchSequential);
        queueConfig.PowerManaged = FALSE;

        queueConfig.EvtIoInternalDeviceControl = HUBPDO_EvtIoInternalDeviceControl;
        queueConfig.EvtIoDeviceControl = HUBPDO_EvtIoDeviceControl;

        status = WdfIoQueueCreate(device,
                                  &queueConfig,
                                  WDF_NO_OBJECT_ATTRIBUTES,
                                  &queue);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfIoQueueCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // set the Pnp and Power properties for this device
        //
        WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);
        //
        // Check for embedded devices
        //
        pnpCaps.Removable = DeviceContext->DeviceFlags.DeviceNotRemovable ? WdfFalse : WdfTrue;

        //
        // unique id means we have a serial number
        //
        pnpCaps.UniqueID = DeviceContext->DeviceFlags.SerialNumber ? WdfTrue : WdfFalse;

        //
        // For legacy reasons, this defaults to FALSE and it
        // is up to the FDO to change it to TRUE
        //
        pnpCaps.SurpriseRemovalOK = WdfFalse;

        pnpCaps.Address = DeviceContext->PortContext->PortInformation.PortNumber;
        pnpCaps.UINumber = (ULONG) -1;

        WdfDeviceSetPnpCapabilities(device,
                                    &pnpCaps);

        WDF_DEVICE_POWER_CAPABILITIES_INIT(&powerCaps);

        powerCaps.SystemWake = DeviceContext->HubFdoContext->DeviceCapabilities.SystemWake;

        powerCaps.DeviceState[PowerSystemWorking] = PowerDeviceD0;

        //
        // If it is a hub, then we always declare it as
        // wake capable. The is because the hub should always
        // be able to propogate wake for its downstrream
        // devices whether it itself wake capable or not.
        // At the FDO level, we will explicitly check whether
        // the hub is really wake capable and only if it is,
        // we will setup Selective Suspend for it
        //
        if (DeviceContext->DeviceFlags.UsbWakeupSupport ||
            DeviceContext->DeviceFlags.DeviceIsHub) {

            SYSTEM_POWER_STATE  ps;

            //
            // Caps if the USB device supports wake through the
            // bus
            //
            powerCaps.DeviceWake = PowerDeviceD2;
            powerCaps.WakeFromD0 = WdfTrue;
            powerCaps.WakeFromD1 = WdfTrue;
            powerCaps.WakeFromD2 = WdfTrue;
            powerCaps.WakeFromD3 = WdfFalse;
            powerCaps.D1Latency = 0;
            powerCaps.D2Latency = 0;
            powerCaps.D3Latency = 0;
            powerCaps.DeviceD1 = WdfTrue;
            powerCaps.DeviceD2 = WdfTrue;

            for (ps = PowerSystemSleeping1; ps <= PowerSystemShutdown; ps++) {

                if (ps > powerCaps.SystemWake) {
                    powerCaps.DeviceState[ps] = PowerDeviceD3;
                } else {
                    powerCaps.DeviceState[ps] = PowerDeviceD2;
                }

            }

            //
            // For a SS device, pre-allocate a request for getting wake
            // notifications from the device
            //
            if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
                WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WAKE_NOTIFICATION_REQUEST_CONTEXT);
                attributes.ParentObject = device;

                status = WdfRequestCreate(&attributes,
                                          WdfDeviceGetIoTarget(DeviceContext->HubFdoContext->HubFdoDevice),
                                          &pdoContext->WakeNotification);

                if (!NT_SUCCESS(status)) {
                    DbgTraceTo(DeviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Driver,
                               "WdfRequestCreate for Wake Notification Failed %!STATUS!",
                               status);

                    pdoContext->WakeNotification = NULL;
                    LEAVE;
                }

                wakeNotificationRequestContext = GetWakeNotificationRequestContext(pdoContext->WakeNotification);

                KeInitializeEvent(&wakeNotificationRequestContext->CompletionEvent,
                                  NotificationEvent,
                                  FALSE);


            }

        } else {

            SYSTEM_POWER_STATE  ps;

            //
            // Device doesn't support wake
            //
            powerCaps.DeviceWake = PowerDeviceD0;

            powerCaps.WakeFromD0 = WdfTrue;
            powerCaps.WakeFromD1 = WdfFalse;
            powerCaps.WakeFromD2 = WdfFalse;
            powerCaps.WakeFromD3 = WdfFalse;

            powerCaps.D1Latency = 0;
            powerCaps.D2Latency = 0;
            powerCaps.D3Latency = 0;

            powerCaps.DeviceD1 = WdfFalse;
            powerCaps.DeviceD2 = WdfFalse;

            for (ps = PowerSystemSleeping1; ps <= PowerSystemShutdown; ps++) {

                if (ps > powerCaps.SystemWake) {
                    powerCaps.DeviceState[ps] = PowerDeviceD3;
                } else {
                    powerCaps.DeviceState[ps] = PowerDeviceD2;
                }

            }

        }

        WdfDeviceSetPowerCapabilities(device,
                                      &powerCaps);

        //
        // Initialize the Idle Irp Context
        //
        KeInitializeSpinLock(&pdoContext->IdleIrpContext.StateMachineLock);
        pdoContext->IdleIrpContext.CurrentState = StartIndexIsm;
        pdoContext->IdleIrpContext.StateTable = ISMStateTable;
        pdoContext->IdleIrpContext.StartIndex = StartIndexIsm;

        //
        // Create the worker item for the state machine that will be used to
        // invoke client callback
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, IDLE_WORKER_CONTEXT);

        attributes.ParentObject = device;

        WDF_WORKITEM_CONFIG_INIT(&workitemConfig, HUBIDLE_EvtIdleWorkItem);

        status = WdfWorkItemCreate(&workitemConfig,
                                   &attributes,
                                   &pdoContext->IdleIrpContext.IdleWorker);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfWorkItemCreate Failed %!STATUS!",
                       status);

            pdoContext->IdleIrpContext.IdleWorker = NULL;
            LEAVE;
        }
        //
        // Kick-off the idle state machine
        //
        HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                         IsmEventStart,
                         NULL);

        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, IdleStateMachineInitialized);

        //
        // Now that we have all the device properties set, we are ready to
        // report it to KMDF and start handling EventCallbacks for it
        //
        status = WdfFdoAddStaticChild(DeviceContext->HubFdoContext->HubFdoDevice,
                                      device);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfFdoAddStaticChild Failed %!STATUS!",
                       status);
            LEAVE;

        } else {
            ULONG                   portPath[6];
            PUSB_TOPOLOGY_ADDRESS   usbTopologyAddress;
            ULONG                   i;

            usbTopologyAddress = &pdoContext->HubFdoContext->HubParentInterface.HubTopologyAddress;

            portPath[0] = usbTopologyAddress->RootHubPortNumber;
            for (i = 0; i < 5; i++) {
                portPath[i+1] = usbTopologyAddress->HubPortNumber[i];
            }

            if (pdoContext->HubFdoContext->HubParentInterface.HubDepth < 6) {
                portPath[pdoContext->HubFdoContext->HubParentInterface.HubDepth] = pdoContext->PortNumber;
            }

            //
            // Write device info to the error partition so it is remains in the log longer.
            //
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Device Context 0x%p - USB\\VID_%04X&PID_%04X&REV_%04X - Port Path %d:%d:%d:%d:%d:%d",
                       DeviceContext,
                       DeviceContext->DeviceDescriptor.idVendor,
                       DeviceContext->DeviceDescriptor.idProduct,
                       DeviceContext->DeviceDescriptor.bcdDevice,
                       portPath[0],
                       portPath[1],
                       portPath[2],
                       portPath[3],
                       portPath[4],
                       portPath[5]);
        }

        deleteDeviceObject = FALSE;

        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U1EnabledForUSPort);
        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U2EnabledForUSPort);
        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U1TimeoutAcceptTransitions);
        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U1TimeoutInitiateTransitions);
        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U2TimeoutAcceptTransitions);
        INTERLOCKED_SET_BIT_FIELD(LPM_POLICY, &DeviceContext->LpmPolicy, U2TimeoutInitiateTransitions);

        if (DeviceContext->DeviceFlags.DeviceIsHub == 1) {

            HUBPDO_RegisterForPowerSettingsForHub(pdoContext);

        } else {

            HUBPDO_RegisterForPowerSettingsForDevice(pdoContext);
        }

    } FINALLY {

        if (KnownDevice == FALSE) {

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_FAILED(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->HubFdoContext->HubParentInterface.Hub,
                status);

        } else {

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_ENUMERATION_COMPLETE(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->HubFdoContext->HubParentInterface.Hub,
                DeviceContext->UsbDevice,
                DeviceContext->PortContext->PortInformation.PortNumber,
                status);
        }

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        if (NT_SUCCESS(status)) {
            result = DsmEventOperationSuccess;
        } else {
            result = DsmEventOperationFailure;
        }

        if (freeDeviceInit) {
            //
            // The WDFDEVICE_INIT only needs to be freed
            // if we failed before or in the call
            // to WdfDeviceCreate
            //
            WdfDeviceInitFree(deviceInit);
        }

        if (deleteDeviceObject) {
            //
            // WdfDeviceObject only needs to be deleted
            // if WdfDeviceCreate succeeded and if WdfFdoAddStaticChild
            // hasn't been called successfully after that
            //
            WdfObjectDelete(device);

        }

        if (releaseDsmReference) {
            //
            // Device creation failed, so Cleanup will not be invoked. Hence, we
            // need to release the reference on DSM right here
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(DeviceContext),
                                        (PVOID) PDOTag);
        }

    }

    return result;

}

__drv_functionClass(POWER_SETTING_CALLBACK)
__drv_sameIRQL
NTSTATUS
HUBPDO_PowerSettingCallback (  
   __in 
       LPCGUID             SettingGuid,
   __in_bcount(ValueLength) 
       PULONG              NewValue,
   __in 
       ULONG               ValueLength,
   __inout_opt 
       PDEVICE_CONTEXT     DeviceContext
   )
/*++

Routine Description:

    Called by the power manager whenever one of the
    registered power setting changes.

Return Value:

    NTSTATUS

--*/
{
    LPM_POLICY      lpmPolicy;
    NTSTATUS        status;

    UNREFERENCED_PARAMETER(ValueLength);


    TRY {

        status = STATUS_SUCCESS;

        RtlCopyMemory(&lpmPolicy,
                      &DeviceContext->LpmPolicy,
                      sizeof(LPM_POLICY));


        if (RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_3_LINK_POWER_MANAGEMENT_POLICY,
                             sizeof(GUID)) == sizeof(GUID)) {

            if (*NewValue > 3) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            if (*NewValue == 0) {

                //
                // Value of 0 means do not enable the U1 U2 states
                //
                RtlZeroMemory(&lpmPolicy,
                              sizeof(LPM_POLICY));
                LEAVE;
            }

            lpmPolicy.U1EnabledForUSPort = 1;
            lpmPolicy.U1TimeoutAcceptTransitions = 1;
            lpmPolicy.U1TimeoutInitiateTransitions = 1;
            lpmPolicy.U2EnabledForUSPort = 1;
            lpmPolicy.U2TimeoutAcceptTransitions = 1;
            lpmPolicy.U2TimeoutInitiateTransitions = 1;

            if (*NewValue == 1) {
                //
                // Value of 1 means enable the U1 U2 states but choose
                // conservative timeout values to optimize for peformance
                //
                lpmPolicy.ForceAggressiveTimeoutValues = 0;
                lpmPolicy.ForceConservativeTimeoutValues = 1;
                LEAVE;
            }

            if (*NewValue == 2) {
                //
                // Value of 2 means enable the U1 U2 states and choose
                // optimal timeout values to balance power and peformance
                //
                lpmPolicy.ForceAggressiveTimeoutValues = 0;
                lpmPolicy.ForceConservativeTimeoutValues = 0;
                LEAVE;
            }

            if (*NewValue == 3) {
                //
                // Value of 3 means enable the U1 U2 states and choose
                // aggressive timeout values to optimize for power
                //
                lpmPolicy.ForceAggressiveTimeoutValues = 1;
                lpmPolicy.ForceConservativeTimeoutValues = 0;
                LEAVE;
            }


           LEAVE;
        }

        if (((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U1_ENABLE_FOR_DEVICES,
                             sizeof(GUID)) == sizeof(GUID))) ||
            ((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U1_ENABLE_FOR_HUBS,
                             sizeof(GUID)) == sizeof(GUID)))) {

            if (ValueLength < sizeof(ULONG)) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

           lpmPolicy.U1EnabledForUSPort = (*NewValue == 0) ? 0 : 1;

           LEAVE;
        }

        if (((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U2_ENABLE_FOR_DEVICES,
                             sizeof(GUID)) == sizeof(GUID))) ||
            ((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U2_ENABLE_FOR_HUBS,
                             sizeof(GUID)) == sizeof(GUID)))) {

            if (ValueLength < sizeof(ULONG)) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

           lpmPolicy.U2EnabledForUSPort = (*NewValue == 0) ? 0 : 1;

           LEAVE;
        }

        if (((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U1_TIMEOUT_FOR_DEVICES,
                             sizeof(GUID)) == sizeof(GUID))) ||
            ((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U1_TIMEOUT_FOR_HUBS,
                             sizeof(GUID)) == sizeof(GUID)))) {

            if (ValueLength < sizeof(ULONG)) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            //
            // Value of 0 means accept and initiate
            // Value of 1 means accept but not initiate
            // Value of 2 means reject and not initiate
            //
            if (*NewValue == 0) {

                lpmPolicy.U1TimeoutAcceptTransitions = 1;
                lpmPolicy.U1TimeoutInitiateTransitions = 1;

            } else if (*NewValue == 1) {

                lpmPolicy.U1TimeoutAcceptTransitions = 1;
                lpmPolicy.U1TimeoutInitiateTransitions = 0;

            } else if (*NewValue == 2) {

                lpmPolicy.U1TimeoutAcceptTransitions = 0;
                lpmPolicy.U1TimeoutInitiateTransitions = 0;

            }

           LEAVE;
        }

        if (((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U2_TIMEOUT_FOR_DEVICES,
                             sizeof(GUID)) == sizeof(GUID))) ||
            ((RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_USB_U2_TIMEOUT_FOR_HUBS,
                             sizeof(GUID)) == sizeof(GUID)))) {

            if (ValueLength < sizeof(ULONG)) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            //
            // Value of 0 means accept and initiate
            // Value of 1 means accept but not initiate
            // Value of 2 means reject and not initiate
            //
            if (*NewValue == 0) {

                lpmPolicy.U2TimeoutAcceptTransitions = 1;
                lpmPolicy.U2TimeoutInitiateTransitions = 1;

            } else if (*NewValue == 1) {

                lpmPolicy.U2TimeoutAcceptTransitions = 1;
                lpmPolicy.U2TimeoutInitiateTransitions = 0;

            } else if (*NewValue == 2) {

                lpmPolicy.U2TimeoutAcceptTransitions = 0;
                lpmPolicy.U2TimeoutInitiateTransitions = 0;

            }

           LEAVE;
        }



    } FINALLY {

        if (NT_SUCCESS(status)) {
            if (RtlCompareMemory(&lpmPolicy,
                                 &DeviceContext->LpmPolicy,
                                 sizeof(LPM_POLICY)) != sizeof(LPM_POLICY)) {

                RtlCopyMemory(&DeviceContext->LpmPolicy,
                              &lpmPolicy,
                              sizeof(LPM_POLICY));

                HUBSM_AddDsmEvent(DeviceContext,
                                  DsmEventLPMPowerSettingChange);
            }

        }
        else {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
               TL_ERROR,
               Driver,
               "HUBPDO_PowerSettingCallback Failed %!STATUS!",
               status);
        }
    }

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
DSM_EVENT
HUBPDO_CreatePdo(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Called by DSM to create a PDO when we have enough
    information to create all of the proper PnP IDs
    (Meaning we successfully retrieved the device descriptor)

Return Value:

    DsmEventOperationFailure, DsmEventOperationSuccess

--*/
{

    PFN_USBDEVICE_SET_PDO_INFORMATION
                                    setPdoInformation;
    DSM_EVENT                       retEvent;


    PAGED_CODE();

    DeviceContext->EnumMsgId = 0;

    retEvent =  HUBPDO_CreatePdoInternal(DeviceContext,
                                         TRUE);

    if (retEvent == DsmEventOperationSuccess) {
        //
        // Let UCX know about the PDO, this is used only for the purposes
        // of diagnostics
        //
        setPdoInformation = DeviceContext->HubFdoContext->HubControllerStackInterface.UsbDeviceSetPdoInformation;

        NT_ASSERT(setPdoInformation != NULL);

        setPdoInformation(DeviceContext->HubFdoContext->HubParentInterface.Hub,
                          DeviceContext->UsbDevice,
                          WdfDeviceWdmGetDeviceObject(WdfObjectContextGetObject(DeviceContext->PdoContext)));

        DeviceContext->PortContext->ConnectionStatus = DeviceConnected;


    } else {

        DeviceContext->PortContext->ConnectionStatus = DeviceGeneralFailure;

    }

    return retEvent;

}

__drv_maxIRQL(PASSIVE_LEVEL)
DSM_EVENT
HUBPDO_CreateUnknownPdo(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls WdfDeviceCreate to create an unknown failed WDF device
    object for the child device.

Return Value:

    DsmEventOperationFailure, DsmEventOperationSuccess

--*/
{

    PAGED_CODE();

    DeviceContext->PortContext->ConnectionStatus = DeviceFailedEnumeration;

    return HUBPDO_CreatePdoInternal(DeviceContext,
                                    FALSE);

}




VOID
HUBPDO_ReportDeviceAsMissing(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls WdfChildListUpdateChildDescriptionAsMissing to return the device as
    removed. Assert that it returns STATUS_SUCCESS.

Return Value:

    VOID

--*/
{

    NTSTATUS            status;
    KIRQL               currentIrql;

    TRY {

        //
        // If we call this API at passive level, KMDF can choose to turn back
        // and call in it one of our PDO event callbacks that block. Those
        // callbacks will not unblock until the state machine runs, which will
        // not happen until we return from this call. So we could end up in
        // a deadlock. To avoid this issue, we temperorily raise the IRQL
        // to dispatch
        //
        KeRaiseIrql(DISPATCH_LEVEL,
                    &currentIrql);

        status = WdfPdoMarkMissing(WdfObjectContextGetObject(DeviceContext->PdoContext));

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfPdoMarkMissing Failed %!STATUS!",
                       status);
        }


        KeLowerIrql(currentIrql);

        NT_ASSERT(NT_SUCCESS(status));


    } FINALLY {

    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_ReleaseWdfPowerReferenceOnHub(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Call WdfDeviceResumeIdle on the hub FDO. Log it , to help debugging.

Return Value:

    VOID

--*/
{
    KIRQL               currentIrql;

    TRY {

        //DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_INFO, Hub, "DSM 0x%p releasing a Power reference on the hub 0x%p",
        //         DeviceContext, DeviceContext->HubFdoContext);

        // If we call this API at passive level, KMDF can choose to turn back
        // and call in it one of our PDO event callbacks that block. Those
        // callbacks will not unblock until the state machine runs, which will
        // not happen until we return from this call. So we could end up in
        // a deadlock. To avoid this issue, we temperorily raise the IRQL
        // to dispatch
        //

        if (DeviceContext->DeviceStateFlags.WdfPowerReferenceHeldOnFdo == 1) {
            KeRaiseIrql(DISPATCH_LEVEL,
                        &currentIrql);

            WdfDeviceResumeIdle(WdfObjectContextGetObject(DeviceContext->HubFdoContext));

            KeLowerIrql(currentIrql);
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, WdfPowerReferenceHeldOnFdo);
        }

    } FINALLY {

    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
HUBPDO_AcquireWdfPowerReferenceOnHub(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls WdfDeviceStopIdle with WaitForD0 as False on the hub FDO.
    If it returns STATUS_PENDING, then return DsmEventOperationPending.
    If it returns STATUS_SUCCESS, then return DsmEventOperationSuccess.
    Assert if it returns any other status.

Return Value:

    DsmEventOperationPending, DsmEventOperationSuccess

--*/
{
    NTSTATUS            status;
    KIRQL               currentIrql;

    TRY {

        //DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_INFO, Hub, "DSM 0x%p acquiring a Power reference on the hub 0x%p",
        //         DeviceContext, DeviceContext->HubFdoContext);

        // If we call this API at passive level, KMDF can choose to turn back
        // and call in it one of our PDO event callbacks that block. Those
        // callbacks will not unblock until the state machine runs, which will
        // not happen until we return from this call. So we could end up in
        // a deadlock. To avoid this issue, we temperorily raise the IRQL
        // to dispatch
        //
        KeRaiseIrql(DISPATCH_LEVEL,
                    &currentIrql);

        status = WdfDeviceStopIdle(WdfObjectContextGetObject(DeviceContext->HubFdoContext),
                                                             FALSE);

        KeLowerIrql(currentIrql);

        if (NT_SUCCESS(status)) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, WdfPowerReferenceHeldOnFdo);
        }

    } FINALLY {

    }
    return DsmEventOperationSuccess;

}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_ReportDeviceFailure(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Calls WdfDeviceSetFailed on the PDO with
    WdfDeviceFailedAttemptRestart for a device that failed enumeration.

Return Value:

    VOID

--*/
{

    TRY {

        HUBMISC_CreateWerReport(DeviceContext->HubFdoContext,
                                HubWerFailedEnumeration,
                                DeviceContext->EnumMsgId);

        WdfDeviceSetFailed(WdfObjectContextGetObject(DeviceContext->PdoContext),
                           WdfDeviceFailedAttemptRestart);



    } FINALLY {

    }

}

VOID
HUBPDO_SetSystemWakeSource(
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:
        PoSetSystemWake API uses the Wait Wake to identify the wake source. However,
        WDF has this behaviour where it completes the Wait Wake Irp much before the
        parent HUB gets the D0 IRP and calls WdfDeviceIndicateWakeStatus for the child.
        So its too late to call PoSetSystemWake from WdfDeviceIndicateWakeStatus.
        This routine uses the PoSetSystemWakeDevice to indicate the Pdo instead 
        of the Wait Wake IRP as a wake source to Power manager.

Returns:

--*/
{
    WDFDEVICE       pdo;

    TRY {

        pdo = WdfObjectContextGetObject(DeviceContext->PdoContext);
        PoSetSystemWakeDevice(WdfDeviceWdmGetDeviceObject(pdo));

        DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
            TL_INFO,
            Driver,
            "System Wake Source is Device (%p)",
            WdfDeviceWdmGetDeviceObject(pdo));

    } FINALLY {

    }

}

VOID
HUBPDO_CompleteWaitWake (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    NTSTATUS        status;
    KIRQL           currentIrql;

    TRY {

        // If we call this API at passive level, KMDF can choose to turn back
        // and call in it one of our PDO event callbacks that block. Those
        // callbacks will not unblock until the state machine runs, which will
        // not happen until we return from this call. So we could end up in
        // a deadlock. To avoid this issue, we temperorily raise the IRQL
        // to dispatch
        //
        KeRaiseIrql(DISPATCH_LEVEL,
                    &currentIrql);

        status = WdfDeviceIndicateWakeStatus(WdfObjectContextGetObject(DeviceContext->PdoContext),
                                             STATUS_SUCCESS);
        KeLowerIrql(currentIrql);


    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "WdfDeviceIndicateWakeStatus Failed %!STATUS!",
                       status);
        }

    }

}

EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBPDO_WakeNotificationIoctlComplete;

VOID
HUBPDO_WakeNotificationIoctlComplete(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Completion routine for the wake notification IOCTL. This
    either indicates that the device has sent a wake notification
    to the controller or the IOCTL was cancelled either
    because of software wake or cleanup

Returns:

    VOID

--*/
{
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     deviceContext;
    PWAKE_NOTIFICATION_REQUEST_CONTEXT  wakeRequestContext;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        status = CompletionParams->IoStatus.Status;
        deviceContext = (PDEVICE_CONTEXT) Context;

        HUBPDO_CompleteWaitWake(deviceContext);

        wakeRequestContext = GetWakeNotificationRequestContext(WdfRequest);
        KeSetEvent(&wakeRequestContext->CompletionEvent,
                   IO_NO_INCREMENT,
                   FALSE);

        DbgTraceTo(deviceContext->PortContext->IfrLog,
                   TL_INFO,
                   Device,
                   "Wake Notification IOCTL completed with %!STATUS!",
                   status);

    } FINALLY {

    }
}


NTSTATUS
HUBPDO_SubmitDeviceWakeNotificationIoctl (
    __in
        PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:

    Routine for submitting Device wake notification UCX IOCTLs.

Arguments:


Returns:

    VOID

--*/
{
    NTSTATUS                            status;
    BOOLEAN                             requestSent;
    WDFIOTARGET                         ioTarget;
    WDFREQUEST                          wakeRequest;
    PWAKE_NOTIFICATION_REQUEST_CONTEXT  wakeRequestContext;
    IO_STACK_LOCATION                   ioStackLocation;

    TRY {

        status = STATUS_SUCCESS;
        ioTarget = DeviceContext->HubFdoContext->RootHubIoTarget;
        wakeRequest = DeviceContext->PdoContext->WakeNotification;

        HUBMISC_RequestReuse(wakeRequest);

        wakeRequestContext = GetWakeNotificationRequestContext(wakeRequest);

        RtlZeroMemory(&wakeRequestContext->UcxRequestContext,
                      sizeof(wakeRequestContext->UcxRequestContext));
        wakeRequestContext->UcxRequestContext.Interface = 0;
        wakeRequestContext->UcxRequestContext.Size = sizeof(wakeRequestContext->UcxRequestContext);

        KeClearEvent(&wakeRequestContext->CompletionEvent);

        RtlZeroMemory(&ioStackLocation,
                      sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_REQUEST_REMOTE_WAKE_NOTIFICATION;
        ioStackLocation.Parameters.Others.Argument1 = &wakeRequestContext->UcxRequestContext;
        ioStackLocation.Parameters.Others.Argument2 = DeviceContext->UsbDevice;


        WdfRequestWdmFormatUsingStackLocation(wakeRequest,
                                              &ioStackLocation);

        WdfRequestSetCompletionRoutine(wakeRequest,
                                       HUBPDO_WakeNotificationIoctlComplete,
                                       DeviceContext);


        requestSent = WdfRequestSend(wakeRequest,
                                     ioTarget,
                                     NULL);

        if ( requestSent == FALSE ) {

            status = WdfRequestGetStatus(wakeRequest);
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Wake Notification IOCTL could not be sent with %!STATUS!",
                       status);
        }
    }

    return status;
}


NTSTATUS
HUBPDO_EvtDeviceEnableWakeAtBus (
    WDFDEVICE           Device,
    SYSTEM_POWER_STATE  PowerState
    )
/*++

Routine Description:

    Called by WDF to arm the device for wake
    In this case, we just track that this function is called.  The state machine
    will determine when to actually arm the device.

Returns:


--*/
{
    PHUB_PDO_CONTEXT    pdoContext;
    NTSTATUS            status;

    TRY {
        pdoContext = GetHubPdoContext(Device);
        status = STATUS_SUCCESS;
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, WaitWakePending);

        //
        // Hubs should not be armed for wake if the system is
        // going to sleep. Note that this callback still gets
        // called in this case as hub sends wait wake for propagating
        //
        if ((PowerState != PowerSystemWorking)&&
            (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 1)) {
            LEAVE;
        }

        if (pdoContext->AssociatedDSM->DeviceFlags.UsbWakeupSupport == 0) {
            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }
        //
        // If the client driver supports function suspend, then
        // we don't need to worry about arming the device for
        // wake
        //
        if (pdoContext->PdoFlags.SupportsFunctionSuspend == 1) {
            LEAVE;
        }
        if (pdoContext->AssociatedDSM->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {

            status = HUBPDO_SubmitDeviceWakeNotificationIoctl(pdoContext->AssociatedDSM);
            if (NT_SUCCESS(status)) {
                INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, WakeNotificationSent);
            }
        }

        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, RequiresArmingForWake);

    } FINALLY {
    }

    return status;
}


VOID
HUBPDO_EvtDeviceDisableWakeAtBus (
    WDFDEVICE       Device
    )
/*++

Routine Description:

    Called by WDF to disarm the device for wake
    In this case, we just track that this function is called.  The state machine
    will determine when to actually arm/disarm the device.

Returns:


--*/
{
    PHUB_PDO_CONTEXT                    pdoContext;
    PWAKE_NOTIFICATION_REQUEST_CONTEXT  wakeRequestContext;

    TRY {

        pdoContext = GetHubPdoContext(Device);
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, RequiresArmingForWake);
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, WaitWakePending);

        if (pdoContext->PdoFlags.SupportsFunctionSuspend == 1) {
            LEAVE;
        }

        if (pdoContext->PdoFlags.WakeNotificationSent == 1) {

            INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, WakeNotificationSent);
            wakeRequestContext = GetWakeNotificationRequestContext(pdoContext->WakeNotification);

            //
            // At this point, the request might have already completed
            // But it is ok to cancel it anyways
            //
            WdfRequestCancelSentRequest(pdoContext->WakeNotification);

            HUBMISC_WaitForSignal(&wakeRequestContext->CompletionEvent,
                                  "Waiting for Wake notification Ioctl",
                                  Device);
        }
        } FINALLY {

    }
}

NTSTATUS
HUBPDO_AllocateForwardProgressResources(
    PDEVICE_CONTEXT         DeviceContext
    )
/*++
Routine Description:

    Called whenever a PDO is notified of a change in its use
    for special file support

Arguments:

    Device - the WDFDEVICE representing our FDO

    NotificationType - specifies the type of special file this call pertains to

    IsInNotificationPath - Indicates whether or not the system is starting to use the special file
            or if the system has finished using the special file

Return Value:

    VOID

--*/
{
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    NTSTATUS                status;

    TRY {

        //
        // Create our pre-allocated request for port control transfers
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);

        objectAttributes.ParentObject = WdfObjectContextGetObject(DeviceContext);

        status = WdfRequestCreate(&objectAttributes,
                                  WdfDeviceGetIoTarget(DeviceContext->HubFdoContext->HubFdoDevice),
                                  &DeviceContext->ControlRequestForBootDevice.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfRequestCreate for Device Control Transfer For Boot Device Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Store the IRP for debugger extension
        //
        DeviceContext->ControlRequestForBootDevice.Irp =
            WdfRequestWdmGetIrp(DeviceContext->ControlRequestForBootDevice.WdfRequest);

    } FINALLY {

    }

    return status;
}

NTSTATUS
HUBPDO_EvtDeviceUsageNotificationEx(
    WDFDEVICE               Device,
    WDF_SPECIAL_FILE_TYPE   NotificationType,
    BOOLEAN                 IsInNotificationPath
    )
/*++
Routine Description:

    Called whenever a PDO is notified of a change in its use
    for special file support

Arguments:

    Device - the WDFDEVICE representing our FDO

    NotificationType - specifies the type of special file this call pertains to

    IsInNotificationPath - Indicates whether or not the system is starting to use the special file
            or if the system has finished using the special file

Return Value:

    NTSTATUS

--*/
{

    PHUB_PDO_CONTEXT    pdoContext;
    NTSTATUS            status;
    NTSTATUS            status2;
    BOOLEAN             isDeviceExternal;
    BOOLEAN             isWinPE;

    UNREFERENCED_PARAMETER(IsInNotificationPath);

    TRY {

        //
        // WinPE has special needs.
        // 1. We need to allow paging on external non-boot disks for WinPE
        // 2. We need to disable SR mitigations for WinPE
        //
        isWinPE = NT_SUCCESS(RtlCheckRegistryKey(RTL_REGISTRY_CONTROL, WINPE_KEY));

        pdoContext = GetHubPdoContext(Device);
        status = STATUS_SUCCESS;
        //
        // Log for SQM purposes if we see this notification at all
        //
        if (pdoContext->AssociatedDSM->DeviceSqmFlags.HostSpecialFiles == 0) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_SQM_PROPERTIES, &pdoContext->AssociatedDSM->DeviceSqmFlags, HostSpecialFiles);
            HUBREG_UpdateSqmFlags(pdoContext->AssociatedDSM);
        }

        switch(NotificationType) {
        case WdfSpecialFilePaging:

            //
            // We do not support paging on non-boot external disks except for WinPE
            //
            isDeviceExternal = HUBBOOT_IsBootDeviceExternal(Device);
            if (!isWinPE &&
                isDeviceExternal &&
                pdoContext->PdoFlags.InBootPath == 0) {

                status = STATUS_NOT_SUPPORTED;
                LEAVE;
            }
            // Fall through for the other case

        case WdfSpecialFileBoot:
            //
            // Once special file is enabled, we never disable it
            // again because system requires a reboot to change
            // any special file location
            //
            if (pdoContext->PdoFlags.InBootPath == 1) {
                LEAVE;
            }

            NT_ASSERT(IsInNotificationPath);
            status = HUBPDO_AllocateForwardProgressResources(pdoContext->AssociatedDSM);
            if (!NT_SUCCESS(status)) {
                //
                // Specific failure already logged in the child function
                //
                LEAVE;
            }

            //
            // If the device is already disconnected by the time we
            // get this notification, then it is too late
            //
            if (pdoContext->AssociatedDSM->PortContext->PortFlags.DeviceConnected == 0) {
                status = STATUS_NO_SUCH_DEVICE;
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Device has already disconnected when the usage notification arrived %!STATUS!",
                           status);

                LEAVE;
            }

            if (!isWinPE) {
                //
                // We do not want to register for a boot device removal notification if the device is internal
                // This will reduce our memory footprint for SATA replacement scenario
                //
                isDeviceExternal = HUBBOOT_IsBootDeviceExternal(Device);
                if (isDeviceExternal) {

                    //
                    // We support an external paging device only if it is also the boot device
                    //
                    NT_ASSERT(NotificationType == WdfSpecialFileBoot);
                    status2 = HUBBOOT_RegisterBootDevice(pdoContext->AssociatedDSM,
                                                        &pdoContext->AssociatedDSM->BootDeviceHandle);

                    //
                    // If the device is not a hub, register the surprise removal WMI notification
                    // instance.
                    //
                    if (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 0) {
                        WMI_RegisterSurpriseRemovalNotificationInstance(Device);
                    }

                    //
                    //  We do not want to fail the boot notification
                    //
                    if (!NT_SUCCESS(status2)) {
                        DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                                   TL_ERROR,
                                   Driver,
                                   "ExRegisterBootDevice Failed for %!STATUS!",
                                   status2);
                        HUBMISC_DbgBreak("ExRegisterBootDevice Failed");
                    }
                } else {
                    DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                               TL_ERROR,
                               Driver,
                               "ExRegisterBootDevice not called for internal device");
                }
                INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, InBootPath);
                INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &pdoContext->AssociatedDSM->PortContext->PortFlags, ConnectedDeviceSupportsReAttach);
            }
            //
            // We need to ensure that the state machines for the DSM and PSM use the
            // IoTryWorkItem or dedicated system thread to ensure forward progress
            //
            pdoContext->AssociatedDSM->DsmContext.NeedsForwardProgress = TRUE;
            pdoContext->AssociatedDSM->PortContext->PsmContext.NeedsForwardProgress = TRUE;
            break;

        case WdfSpecialFileHibernation:
            //
            // Once forward progress has been enabled, it is not worth disabling it
            //
            if (IsInNotificationPath) {
                pdoContext->AssociatedDSM->DsmContext.NeedsForwardProgress = TRUE;
                pdoContext->AssociatedDSM->PortContext->PsmContext.NeedsForwardProgress = TRUE;
            }
            break;

        default:
            break;
        }

    } FINALLY {

    }

    return status;

}

#if 0

VOID
HUBPDO_EvtIoTargetRemoveComplete(
    WDFIOTARGET                     IoTarget
    )
/*++
Routine Description:

   This is a dummy callbac, the only purpose of this
   callback is to prevent KMDF from doing its default
   action of closing the dummy io target on the parent
   when parent is surprise removed

Arguments:


Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(IoTarget);
}

VOID
HUBPDO_OpenDummyHandleOnParentStack(
    PHUB_PDO_CONTEXT                PdoContext
    )
/*++
Routine Description:

    Pnp manager has this behavior that once a node is
    reported as missing, Pnp treats the broken part of the
    tree as an independent entity. Depending on the open
    handles, it can send a remove to parent tree before
    sending removes to nodes in this entity. This is an issue
    for KMDF as well as our design. To workaround this issue
    we create an dummy I/O target on the parent. This will
    prevent the parent from getting a remove. Once the child
    PDO has been remvoed and is about to be destroyed, we
    will delete this dummy target in the EvtDeviceDestroy
    callback

    Note: Late in Win 8 timeframe, PnP team fixed some bugs
    and implemented a contract where the entire tree structure
    is preserved and the child is always removed before the 
    parent. As a part of this, they treat the devnodes in 
    the tree as a group and initiate the removal only when
    all devices in the group have their handles closed. The
    usbhub3 workaround to have a handle open on the parent
    prevents removal of the child as well leading to the bug.

Arguments:

    pdoContext

Return Value:

    VOID

--*/
{

    WDFDEVICE                   parentHub;
    WDF_IO_TARGET_OPEN_PARAMS   openParams;
    NTSTATUS                    status;
    WDF_OBJECT_ATTRIBUTES       attributes;
    UNICODE_STRING              pdoName;
    PDEVICE_CONTEXT             deviceContext;
    PDEVICE_OBJECT              parentPdo;
    ULONG                       length;
    PWSTR                       pName;

    TRY {

        pName = NULL;

        deviceContext = PdoContext->AssociatedDSM;

        parentHub = WdfObjectContextGetObject(deviceContext->HubFdoContext);

        parentPdo = WdfDeviceWdmGetPhysicalDevice(parentHub);

        length = 0;

        status = IoGetDeviceProperty(parentPdo,
                                     DevicePropertyPhysicalDeviceObjectName,
                                     0,
                                     NULL,
                                     &length);

        if (status != STATUS_BUFFER_TOO_SMALL && !NT_SUCCESS(status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "First IoGetDeviceProperty failed: %!STATUS!",
                       status);

            LEAVE;

        }

        if (length > USHORT_MAX) {

            status = STATUS_INTEGER_OVERFLOW;

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "PDO Name too long: %!STATUS!",
                       status);

            LEAVE;

        }

        if (length == 0) {

            status = STATUS_INVALID_DEVICE_STATE;

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "PDO Name length is 0: %!STATUS!",
                       status);

            LEAVE;
        }

        pName = (PWSTR) ExAllocatePoolWithTag(PagedPool,
                                              length,
                                              USBHUB3_DEVICE_TAG);

        if (pName == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "PDO Name Buffer could not be allocated: %!STATUS!",
                       status);

            LEAVE;
        }

        status = IoGetDeviceProperty(parentPdo,
                                     DevicePropertyPhysicalDeviceObjectName,
                                     length,
                                     pName,
                                     &length);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Second IoGetDeviceProperty failed: %!STATUS!",
                       status);
            LEAVE;
        }

        pdoName.Buffer = pName;
        pdoName.Length = (USHORT) length - sizeof(UNICODE_NULL);
        pdoName.MaximumLength = (USHORT) length;


        WDF_IO_TARGET_OPEN_PARAMS_INIT_CREATE_BY_NAME(&openParams,
                                                      &pdoName,
                                                      STANDARD_RIGHTS_ALL);

        openParams.EvtIoTargetRemoveComplete =  HUBPDO_EvtIoTargetRemoveComplete;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = parentHub;

        status = WdfIoTargetCreate(parentHub,
                                   &attributes,
                                   &PdoContext->DummyParentTarget);

        if (!NT_SUCCESS (status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Could not create the dummy IO target for the parent: %!STATUS!",
                       status);

            PdoContext->DummyParentTarget = NULL;

            LEAVE;
        }

        status = WdfIoTargetOpen(PdoContext->DummyParentTarget,
                                 &openParams);

        if (!NT_SUCCESS (status)) {

            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Could not open the dummy IO target for the parent: %!STATUS!",
                       status);

            WdfObjectDelete(PdoContext->DummyParentTarget);

            PdoContext->DummyParentTarget = NULL;

            LEAVE;
        }

    } FINALLY {


        if (pName) {
            ExFreePoolWithTag(pName,
                              USBHUB3_DEVICE_TAG);
        }

    }

}

#endif

NTSTATUS
HUBPDO_EvtDeviceResourceRequirementsQuery(
    IN WDFDEVICE Device,
    IN WDFIORESREQLIST IoResourceRequirementsList
    )
/*++
Routine Description:

    Called PDO receives IRP_MN_QUERY_RESOURCE_REQUIREMENT

Arguments:

    Device - the WDFDEVICE representing our PDO

Return Value:

    NTSTATUS

--*/
{

    PDEVICE_CONTEXT             deviceContext;
    NTSTATUS                    status;

    PAGED_CODE ();

    UNREFERENCED_PARAMETER(IoResourceRequirementsList);

    TRY {
        deviceContext = GetHubPdoContext(Device)->AssociatedDSM;


        //
        // We need to write MSOS extended property registry keys after the device
        // is enumerated but before the AddDevice for the FDO is invoked. This is
        // the same point at which the Win7 stack writes them.
        // Also, we need to do this exactly once in the lifetime of the
        // device
        //
        // The above comment is actually not correct. It is possible that the device
        // was uninstalled and re-installed. Therefore the extended property keys
        // might need to be read again. TODO: Remove the commented code and also remove 
        // the InstallMSOSExtEventProcessedflag completely. That will require a state
        // machine change which is being avoided at this point because this fix
        // might need to go in Windows 8.1
        //
        // if (deviceContext->DeviceStateFlags.InstallMSOSExtEventProcessed == 1) {
        //    LEAVE;
        // }
        //

        //
        // We need to wake the parent hub if it suspended, so that we can send the
        // device transfers 
        //
        status = WdfDeviceStopIdle(WdfObjectContextGetObject(deviceContext->HubFdoContext),
                                   TRUE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Not sending the DsmEventPDOInstallMSOSExt event because could not wake up the parent hub: %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // We normally expect the device to be in an enabled state at this point. 
        // However, in extreme cases where PnP took too long to process the device,
        // our DNF heruristics might have kicked in and disabled the device. So
        // we first queue the PreStart event just in case so that the device can
        // be re-enumerated. If the device is already enabled, state machine will
        // simply ack the event
        //
        if (deviceContext->DeviceStateFlags.DeviceStarted == 0) {
            KeClearEvent(&deviceContext->PreStartEvent);

            HUBSM_AddDsmEvent(deviceContext,
                              DsmEventPDOPreStart);
            
            HUBMISC_WaitForSignal(&deviceContext->PreStartEvent,
                                  "Pre Start Completion",
                                  Device);
        }

        KeClearEvent(&deviceContext->PnPPowerEvent);

        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventPDOInstallMSOSExt);

        HUBMISC_WaitForSignal(&deviceContext->PnPPowerEvent,
                              "Device ResourcesQuery",
                              Device);

        WdfDeviceResumeIdle(WdfObjectContextGetObject(deviceContext->HubFdoContext));

    } FINALLY {

    }

    return STATUS_SUCCESS;

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_SetInstallMSOSExtEventProcessed(
    __in
        PDEVICE_CONTEXT       DeviceContext
    )
/*++

Routine Description:


Return Value:


--*/
{
    INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, InstallMSOSExtEventProcessed);
}


VOID
HUBPDO_EvtDeviceReportedMissing(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    Called whenever a PDO is being reported as missing
    to PnP

Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{

    PHUB_PDO_CONTEXT            pdoContext;

    PAGED_CODE ();

    TRY {
        pdoContext = GetHubPdoContext(Device);
        HUBSM_AddDsmEvent(pdoContext->AssociatedDSM,
                          DsmEventPDOReportedMissing);

        EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Cleanup(pdoContext->AssociatedDSM);

        //
        // We no longer need the workaround in HUBPDO_OpenDummyHandleOnParentStack.
        // Refer to comments section for the function.
        // 

        // HUBPDO_OpenDummyHandleOnParentStack(pdoContext);


    } FINALLY {

    }

}


VOID
HUBPDO_SetDeviceFlagToForceResetOnEnumeration (
    __in
        PDEVICE_CONTEXT           DeviceContext
    )
/*++

Routine Description:

    Sets Device flag to indicate that DSM needs
    to reset the device while enumerating.

Returns:


--*/
{
    TRY {

        //
        // By default DSM will always request hot reset
        //
        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, HotResetOnEnumRequired);

    } FINALLY {

    }

}

VOID
HUBPDO_EvtOutOfBandwidthTimer (
    __in
        WDFTIMER           Timer
    )
/*++

Routine Description:

    Timer callback function for firing the WMI
    event to show out of bandwidth notification
    to the user

Return Value:

    VOID

--*/
{
    PPORT_CONTEXT           portContext;

    TRY {

        portContext = GetPortContext(WdfTimerGetParentObject(Timer));

        WMI_FireNotification(portContext->HubFdoContext,
                             portContext->PortInformation.PortNumber,
                             InsufficentBandwidth);

    } FINALLY {

    }
}

VOID
HUBPDO_NotifyDifferentDeviceAttachedOnBootDevicePort (
    __in
        PDEVICE_CONTEXT           DeviceContext
    )
/*++

Routine Description:

    A different device got enumerated on the port on
    which boot device was originally connected. We
    will set notify that the boot device is missing
    and set the flag so that when kernel asks if the
    boot device is back, we can tell if it is the
    wrong device

Returns:


--*/
{
    TRY {

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DifferentDeviceOnBootDevicePort);
        HUBBOOT_NotifyBootDeviceRemoval(DeviceContext->BootDeviceHandle);

    } FINALLY {

    }

}

VOID
HUBPDO_NoPingResponse(
    __in
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext
    )
{
    UNREFERENCED_PARAMETER(HubContext);
    //
    // Since this callback can only be invoked by UCX
    // when there is a pending transfer for the PDO, we
    // can assured that the PDO and the DSM are valid
    //
    HUBSM_AddDsmEvent((PDEVICE_CONTEXT)UsbDeviceContext,
                      DsmEventNoPingResponseError);

}


VOID
HUBPDO_DispatchWdmPnpPowerIrpSynchronously(
    __in
        WDFDEVICE           Device,
    __in
        PIRP                Irp
    )
/*++
Routine Description:

    Dispatch routine that forwards the IRP to KMDF
    and waits for its completion

Return Value:

--*/
{
    KEVENT                  event;


    TRY {

        KeInitializeEvent(&event, NotificationEvent, FALSE);

        IoCopyCurrentIrpStackLocationToNext(Irp);

        IoSetCompletionRoutine(Irp, HUBPDO_WdmPnpPowerIrpCompletionRoutine, &event, TRUE, TRUE, TRUE);

        (VOID) WdfDeviceWdmDispatchPreprocessedIrp(Device, Irp);

        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);

    } FINALLY {

    }

}

NTSTATUS
HUBPDO_WdmPnpPowerIrpCompletionRoutine(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++
Routine Description:

    Completion routine to fix the descrepencies created by the framework.

Arguments:

Return Value:

    NT status code.
--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeSetEvent((PKEVENT)Context, IO_NO_INCREMENT, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

_Use_decl_annotations_
VOID 
HUBPDO_EvtCompleteIrpWorkItem(
    WDFWORKITEM WorkItem
    )
/*++

Routine Description:

    
Arguments:                                                           
    
    WorkItem - WorkItem that simply completes the IRP

Returns:                                                             

    VOID
    
--*/   
{
    IoCompleteRequest(GetCompleteIrpWorkItemContext(WorkItem)->Irp,
                      IO_NO_INCREMENT);       
    WdfObjectDelete(WorkItem);
}                     

_Use_decl_annotations_
NTSTATUS
HUBPDO_WdmPnpPowerIrpCompletionRoutineForAsynchronousCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++
Routine Description:

    Completion routine that simply defers the IRP completion
    to a worker item

Arguments:

Return Value:

    NT status code.
--*/
{
    UNREFERENCED_PARAMETER(DeviceObject);
    
    NTSTATUS                    status;
    WDF_OBJECT_ATTRIBUTES       attributes;
    WDF_WORKITEM_CONFIG         workitemConfig;
    WDFWORKITEM                 completionWorker;
    PDEVICE_CONTEXT             deviceContext;

    TRY {

        if (Irp->PendingReturned) {
            IoMarkIrpPending(Irp);
        }

        deviceContext = (PDEVICE_CONTEXT)Context;
        
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes,
                                                IRP_COMPLETE_WORKER_CONTEXT);

        attributes.ParentObject = WdfObjectContextGetObject(deviceContext->PdoContext);

        WDF_WORKITEM_CONFIG_INIT(&workitemConfig, HUBPDO_EvtCompleteIrpWorkItem);

        status = WdfWorkItemCreate(&workitemConfig,
                                   &attributes,
                                   &completionWorker);

        if (!NT_SUCCESS(status)) {
            //
            // If the worker item fails, let the IRP complete on 
            // the same thread because the only reason for the
            // worker item is to avoid a rare deadlock scenario
            // in WDF
            //
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "WdfWorkItemCreate Failed for completing the Pnp/Power IRP %!STATUS!",
                       status);
            status = STATUS_CONTINUE_COMPLETION;

            LEAVE;
        }

        GetCompleteIrpWorkItemContext(completionWorker)->Irp = Irp;
        WdfWorkItemEnqueue(completionWorker);
        status = STATUS_MORE_PROCESSING_REQUIRED;
        
    } FINALLY {
    }

    return status;
}


VOID
HUBPDO_StartNoDriverFoundTimer (
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

        ExSetTimer(DeviceContext->DsmContext.Timer,
                   WDF_REL_TIMEOUT_IN_MS(DEVICE_NO_DRIVER_FOUND_TIMEOUT),
                   0,
                   NULL);

    } FINALLY {

    }
}


#define AGGREGATE_EVERY_N_EVENTS        1001

VOID
EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Wrapper(
    _In_ PDEVICE_CONTEXT        DeviceContext,
    _In_ const unsigned int     IoctlCode,
    _In_ const unsigned int     UrbFunction
    )
/*++

Routine Description:

    Aggregate every N (actually, N-1) events before firing once.

    Currently it recognizes 5 kinds of combination of ioctl/urb. If anything
    new is added, be sure to update CLIENT_INITIATED_RECOVERY_REASON as well.

    NOTE: the big N is determined as following.

    From data below the average events per machine per week is 8,876, therefore
    the big N is set to 1,000 to get reasonable but not too much samples.

    The most noisy machine (#1 below) will still upload 100,214 events, which
    is not quite good, but Cosmos should have no problem handling the volume.

    https://cosmos11.osdinfra.net/cosmos/asimov.partner.osg/my/output/123_0.count-1week-0818.ss 

    machines:     1,174,410
    events:  10,423,820,406
    average:          8,876 events/machine/week

    the top 150 noisy machines generated 44% of all events
    top 150:  44.6%
    top 200:  52.5%
    top 500:  79.6%
    top 1000: 92.9%

    top noisy, event count, % of all events
    machine#1  100,214,997  0.9614%
    machine#2   98,514,117  0.9451%
    machine#3   85,665,708  0.8218%

Returns:

    VOID

--*/
{
    CLIENT_INITIATED_RECOVERY_REASON    reason = RecoveryReasonMax;
    LONG       *eventLocation;
    LONG        count;

    TRY {

        if (DeviceContext == NULL || DeviceContext->HubFdoContext == NULL) {
            LEAVE;
        }

        // Always fire v0 event so local tracing still shows the detailed messages.
        // As v0 event is no longer a measure, it will not show up in telemetry
        EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->HubFdoContext->HubParentInterface.Hub,
                IoctlCode,
                UrbFunction);

        switch (IoctlCode) {

            case IOCTL_INTERNAL_USB_SUBMIT_URB: {

                switch (UrbFunction) {
                    case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL:
                        reason = RecoveryReasonResetPipeAndClearStall;
                        break;
     
                    case URB_FUNCTION_SYNC_RESET_PIPE:
                        reason = RecoveryReasonResetPipe;
                        break;
     
                    case URB_FUNCTION_SYNC_CLEAR_STALL:
                        reason = RecoveryReasonClearStall;
                        break;
                }
                break;
            }

            case IOCTL_INTERNAL_USB_RESET_PORT:
            case IOCTL_INTERNAL_USB_RESET_PORT_ASYNC:
                reason = RecoveryReasonResetPort;
                break;

            case IOCTL_INTERNAL_USB_CYCLE_PORT:
                reason = RecoveryReasonCyclePort;
                break;
        }

        if (reason == RecoveryReasonMax) {
            NT_ASSERT(reason != RecoveryReasonMax);
            LEAVE;
        }

        eventLocation = &DeviceContext->ClientInitiatedRecoveryEvents[reason];

        // The logic of the following code is given below:
        //
        // count++;
        // if (count == 1) {
        //   log(count);
        // }
        // else if (count == N) {
        //   log(count-1);
        //   count = 1;
        // }

        count = InterlockedIncrement(eventLocation);

        if (count == 1 || count == AGGREGATE_EVERY_N_EVENTS) {

            EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_V1(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->HubFdoContext->HubParentInterface.Hub,
                IoctlCode,
                UrbFunction,
                DeviceContext->DeviceDescriptor.idVendor,
                DeviceContext->DeviceDescriptor.idProduct,
                DeviceContext->DeviceDescriptor.bcdDevice,
                (count == 1) ? 1 : (AGGREGATE_EVERY_N_EVENTS - 1));

            InterlockedExchange(eventLocation, 1);
        }

    } FINALLY {

    }
}

VOID
EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_Cleanup(
    _In_ PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Log any aggregated but unfired events.

Returns:

    VOID

--*/
{
    struct Codes {
        int IoctlCode;
        int UrbFunction;
    };

    // NOTE: this table must be kept in sync with CLIENT_INITIATED_RECOVERY_REASON
    static struct Codes codes[] = {
        { IOCTL_INTERNAL_USB_SUBMIT_URB,  URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL },
        { IOCTL_INTERNAL_USB_SUBMIT_URB,  URB_FUNCTION_SYNC_RESET_PIPE },
        { IOCTL_INTERNAL_USB_SUBMIT_URB,  URB_FUNCTION_SYNC_CLEAR_STALL },
        { IOCTL_INTERNAL_USB_RESET_PORT_ASYNC, 0 },
        { IOCTL_INTERNAL_USB_CYCLE_PORT,  0 },
    };
    int     reason;
    LONG    count;

    C_ASSERT(sizeof(codes)/sizeof(codes[0]) == RecoveryReasonMax);

    TRY {

        if (DeviceContext == NULL || DeviceContext->HubFdoContext == NULL) {
            LEAVE;
        }

        for (reason = 0; reason < (int)RecoveryReasonMax; reason++) {

            count = DeviceContext->ClientInitiatedRecoveryEvents[reason];

            if (count != 0 && count != 1) {
                // 0: no event at all;
                // 1: just fired;
                // any other number: aggregated but unfired events.

                EventWriteUSBHUB3_ETW_EVENT_CLIENT_INITIATED_RECOVERY_ACTION_V1(
                    &DeviceContext->DsmContext.CurrentActivityId,
                    DeviceContext->HubFdoContext->HubParentInterface.Hub,
                    codes[reason].IoctlCode,
                    codes[reason].UrbFunction,
                    DeviceContext->DeviceDescriptor.idVendor,
                    DeviceContext->DeviceDescriptor.idProduct,
                    DeviceContext->DeviceDescriptor.bcdDevice,
                    count - 1);
            }

            DeviceContext->ClientInitiatedRecoveryEvents[reason] = 0;
        }

    } FINALLY {

    }
}
