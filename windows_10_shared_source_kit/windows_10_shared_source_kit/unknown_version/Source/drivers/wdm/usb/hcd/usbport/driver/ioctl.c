/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ioctl.c

Abstract:

    Port driver for USB host controllers

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#include "usbpriv.h"

#ifdef USBPORT_WPP
#include "ioctl.tmh"
#endif

#define USBUSER_OP_CONTROLLER_NOT_INITIALIZED_OK(ur) \
    (ur == USBUSER_GET_CONTROLLER_DRIVER_KEY || \
     ur == USBUSER_GET_ROOTHUB_SYMBOLIC_NAME || \
     ur == USBUSER_GET_BANDWIDTH_INFORMATION || \
     ur == USBUSER_GET_POWER_STATE_MAP || \
     ur == USBUSER_GET_USB_DRIVER_VERSION) \


BOOLEAN
USBPORT_CheckLength(
    PUSBUSER_REQUEST_HEADER Header,
    ULONG ParameterLength
    )
/*++

Routine Description:

    Checks Length of user supplied buffer based on api

Arguments:

Return Value:

    FALSE if buffer too small

--*/
{
    ULONG length;
    BOOLEAN retCode = TRUE;

    length = sizeof(*Header) + ParameterLength;

    Header->ActualBufferLength = length;

    if (length > Header->RequestBufferLength) {
        //TEST_TRAP();
        Header->UsbUserStatusCode = UsbUserBufferTooSmall;
        retCode = FALSE;
    }

    return retCode;
}


NTSTATUS
USBPORT_FdoDeviceControlIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for DEVICE_CONTROL Irps sent to the FDO for the HC.

    NOTE: These are user mode requests

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT rhPdo;
    PDEVICE_EXTENSION devExt, rhDevExt;
    ULONG information = 0;
    BOOLEAN completeIrp = TRUE;

    DbgTrace((UsbpDebugTrace, "'IRP_MJ_DEVICE_CONTROL\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);
    USBPORT_ASSERT(UsbIoRequest != NULL);

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_USB_DIAGNOSTIC_MODE_ON:
        DbgTrace((UsbpDebugTrace, "'IOCTL_USB_DIAGNOSTIC_MODE_ON\n"));

        //
        // Note: Setting HctDiagMode will prevent the RootHub from from entering
        // a suspend state. However, there is a race condition between this
        // function and the USBPORTBUSIF_SuspendRootHub function. If the
        // roothub is in the process of going into suspended state
        // ( i.e. USBPORTBUSIF_SuspendRootHub is running ), while this Ioctl is
        // being handled, it may mean that HctDiagMode gets set to 1 and the
        // roothub gets suspended.
        //

        devExt->Fdo.Flags.HctDiagMode = 1;

        rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject,
                                      REF_RhPDO_UsbFdoIoctlDiagnosticModeOn);

        if ( NULL == rhPdo ) {
            //
            // There is no Roothub pdo, so no hub specific steps are needed.
            //
            ntStatus = STATUS_SUCCESS;
            break;
        }

        GET_DEVICE_EXT(rhDevExt, rhPdo);

        KeWaitForSingleObject(&rhDevExt->Pdo.SSLock,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        if (rhDevExt->Pdo.SelectiveSuspend) {

            KeReleaseSemaphore(&rhDevExt->Pdo.SSLock,
                               LOW_REALTIME_PRIORITY,
                               1,
                               FALSE);

            USBPORT_CompleteHubPdoWaitWake(FdoDeviceObject, STATUS_SUCCESS);
        } else {
            KeReleaseSemaphore(&rhDevExt->Pdo.SSLock,
                               LOW_REALTIME_PRIORITY,
                               1,
                               FALSE);
        }

        USBPORT_DerefRootHubPdo(FdoDeviceObject,
                                rhPdo,
                                REF_RhPDO_UsbFdoIoctlDiagnosticModeOn );

        ntStatus = STATUS_SUCCESS;
        break;

    case IOCTL_USB_DIAGNOSTIC_MODE_OFF:
        DbgTrace((UsbpDebugTrace, "'IOCTL_USB_DIAGNOSTIC_MODE_OFF\n"));
        devExt->Fdo.Flags.HctDiagMode = 0;
        // this will enable it if it is reg enabled
        ntStatus = STATUS_SUCCESS;;
        break;

    case IOCTL_GET_HCD_DRIVERKEY_NAME:

        DbgTrace((UsbpDebugTrace, "'IOCTL_GET_HCD_DRIVERKEY_NAME\n"));
        ntStatus = USBPORT_LegacyGetUnicodeName(FdoDeviceObject,
                                                Irp,
                                                &information);
        break;

    case IOCTL_USB_GET_ROOT_HUB_NAME:

        DbgTrace((UsbpDebugTrace, "'IOCTL_USB_GET_ROOT_HUB_NAME\n"));
        ntStatus = USBPORT_LegacyGetUnicodeName(FdoDeviceObject,
                                                Irp,
                                                &information);
        break;

    case IOCTL_USB_USER_REQUEST:

        DbgTrace((UsbpDebugTrace, "'IOCTL_USB_USER_REQUEST\n"));
        ntStatus = USBPORT_UsbFdoUserIoctl(FdoDeviceObject,
                                           Irp,
                                           &information);
        break;

// old IOCTLS no longer supported
//    case IOCTL_USB_HCD_GET_STATS_2:
//    case IOCTL_USB_HCD_GET_STATS_1:

    default:
        // We need to pass these on to our PDO so that filter, bus or PDO drivers
        // may respond.

        DbgTrace((UsbpDebugTrace, "DEVICE CONTROL Request not handled, passing down\n"));

        ntStatus =  USBPORT_PassIrp(
                                FdoDeviceObject,
                                NULL,  // no completion routine
                                NULL,  // no completion routine context
                                FALSE,
                                FALSE,
                                FALSE,
                                Irp,
                                UsbIoRequest);

        completeIrp = FALSE;

    } // switch (irpStack->Parameters.DeviceIoControl.IoControlCode)

    if(completeIrp){
        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, UsbIoRequest, ntStatus, information);
    }

    //
    // DO NOT TOUCH THE IRP FROM THIS POINT ON
    //

    return ntStatus;
}


NTSTATUS
USBPORT_PdoDeviceControlIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Dispatch routine for DEVICE_CONTROL Irps sent to the PDO for the Root Hub.

    NOTE: These are user mode requests

Arguments:

    DeviceObject - Pdo for USB Root Hub

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION rhDevExt;
    ULONG information = 0;

    DbgTrace((UsbpDebugTrace, "'IRP_MJ_DEVICE_CONTROL\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    default:
        DbgTrace((UsbpDebugTrace, "'INVALID DEVICE CONTROL\n"));
        TEST_TRAP();
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;

    } // switch (irpStack->Parameters.DeviceIoControl.IoControlCode)

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, UsbIoRequest, ntStatus, information);
    //
    // DO NOT TOUCH THE IRP FROM THIS POINT ON
    //

    return ntStatus;
}


NTSTATUS
USBPORT_FdoInternalDeviceControlIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for INTERNAL_DEVICE_CONTROL Irps sent to
    the FDO for the HC.

    NOTE: These are kernel mode requests

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpDebugTrace, "'IRP_MJ_DEVICE_CONTROL\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);

    // bugbug pass on to PDO or complete with error?

    DbgTrace((UsbpDebugTrace, "'INVALID INTERNAL DEVICE CONTROL\n"));
    TEST_TRAP();
    ntStatus = STATUS_INVALID_DEVICE_REQUEST;

    USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, UsbIoRequest, ntStatus, 0);

    //
    // DO NOT TOUCH THE IRP FROM THIS POINT ON
    //

    return ntStatus;
}


NTSTATUS
USBPORT_IoctlRecordRhFailure(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Record START_FAILURE info in the root PDO for later reference

Arguments:

Return Value:

    ntStatus
--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts = STATUS_NOT_SUPPORTED;
    PDEVICE_EXTENSION rhDevExt;
    PUSB_START_FAILDATA failData;
    ULONG nBytes;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_ASSERT(ioStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);

    // allocate a structure to keep it

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    failData = ioStack->Parameters.Others.Argument1;
    LOGENTRY(FdoDeviceObject, LOG_MISC, 'rhFD', failData, 0, 0);

    TEST_TRAP();

    if (rhDevExt->Pdo.FailData) {
        nts = STATUS_INVALID_PARAMETER;
    } else {
        nBytes = failData->LengthInBytes;
        if (nBytes) {
            ALLOC_POOL_Z(rhDevExt->Pdo.FailData, NonPagedPoolNx, nBytes);
            if (rhDevExt->Pdo.FailData) {
                RtlCopyMemory(rhDevExt->Pdo.FailData, failData, nBytes);
                nts = STATUS_SUCCESS;
            } else {
                nts = STATUS_INSUFFICIENT_RESOURCES;
            }
        } else {
            nts = STATUS_INVALID_PARAMETER;
        }
    }
    LOGENTRY(FdoDeviceObject, LOG_MISC, 'rhFD', failData, nts, 2);
    return nts;
}


NTSTATUS
USBPORT_PdoInternalDeviceControlIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for INTERNAL_DEVICE_CONTROL Irps sent to
    the PDO for the Root Hub.

    NOTE: These are kernel mode requests

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION rhDevExt;

    DbgTrace((UsbpDebugTrace, "'INTERNAL_DEVICE_CONTROL\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_INTERNAL_USB_SUBMIT_URB:

        DbgTrace((UsbpDebugTrace, "'IOCTL_INTERNAL_USB_SUBMIT_URB\n"));

        {
        PURB urb;
        //
        // all URBs are eventually passed to the root hub PDO
        // this is where we service cleint requests
        //

        // extract the urb;
        urb = irpStack->Parameters.Others.Argument1;

        // call the main urb control function
        ntStatus = USBPORT_ProcessURB(PdoDeviceObject,
                                      rhDevExt->HcFdoDeviceObject,
                                      Irp,
                                      urb,
                                      UsbIoRequest);
        }
        goto USBPORT_PdoInternalDeviceControlIrp_Done;
        break;

    case IOCTL_INTERNAL_USB_GET_HUB_COUNT:

        DbgTrace((UsbpDebugTrace, "'IOCTL_INTERNAL_USB_GET_HUB_COUNT\n"));

        {
        PULONG count;

        //
        // bump the count and complete the Irp
        //
        count = irpStack->Parameters.Others.Argument1;

        USBPORT_ASSERT(count != NULL);
        (*count)++;
        ntStatus = STATUS_SUCCESS;
        }
        break;

    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX:
    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE:

        ntStatus = USBPORT_GetRootHubDeviceHandle(PdoDeviceObject,
                                                  Irp);

        break;

    case IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY:
    case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:
        // should not see this since the neohub driver will not utilize this API
        ntStatus = STATUS_NOT_SUPPORTED;
        TEST_TRAP();
        break;

    case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO:

        DbgTrace((UsbpDebugTrace, "'IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO\n"));

        // this api is called by the hub driver to get the
        // PDO for he root hub.
        // Since the hub may be loaded on a PDO enumerated by
        // another hub it uses this api to get the 'fastest path'
        // to the HCD for URB requests by client drivers.
        {
        PDEVICE_OBJECT *rootHubPdo, *hcdTopOfStackDeviceObject;

        rootHubPdo = irpStack->Parameters.Others.Argument1;
        hcdTopOfStackDeviceObject =
            irpStack->Parameters.Others.Argument2;

        USBPORT_ASSERT(hcdTopOfStackDeviceObject != NULL);
        USBPORT_ASSERT(rootHubPdo != NULL);

        *rootHubPdo = PdoDeviceObject;
        // the original USBD was somewhat screwy in the layout
        // of the HCD device objects in the case of the port
        // driver all requests should go to to the root hub PDO
        *hcdTopOfStackDeviceObject =
             PdoDeviceObject;

        ntStatus = STATUS_SUCCESS;
        }

        break;

    case IOCTL_INTERNAL_USB_GET_PORT_STATUS:

        {
        PULONG ps;
        PDEVICE_EXTENSION devExt;

        ps = irpStack->Parameters.Others.Argument1;
        GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
        ASSERT_FDOEXT(devExt);

        if (ps) {
            *ps = (USBD_PORT_CONNECTED | USBD_PORT_ENABLED);

            if (devExt->Fdo.PowerFlags.HcResumeFailure) {
                // resume failed, report root hub as 'disabled' so that the
                // hub driver will reenumerate the children.
                *ps &= ~USBD_PORT_ENABLED;
            }
            ntStatus = STATUS_SUCCESS;
        } else {
            ntStatus = STATUS_INVALID_PARAMETER;
        }
        }

        break;

    case IOCTL_INTERNAL_USB_RESET_PORT:
        {
        PDEVICE_EXTENSION devExt;

        GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
        ASSERT_FDOEXT(devExt);

        // reset resume failure state so that we report status as
        // USBD_PORT_ENABLED
        devExt->Fdo.PowerFlags.HcResumeFailure = 0;
        ntStatus = USBPORT_HardResetRootPdo(rhDevExt->HcFdoDeviceObject);
        }
        break;

    case IOCTL_INTERNAL_USB_GET_HUB_NAME:
        ntStatus = USBPORT_IoctlGetHubName(PdoDeviceObject,
                                           rhDevExt->HcFdoDeviceObject,
                                           Irp);
        goto USBPORT_PdoInternalDeviceControlIrp_Done;
        break;

    case IOCTL_INTERNAL_USB_RECORD_FAILURE:
        ntStatus = USBPORT_IoctlRecordRhFailure(PdoDeviceObject,
                                                rhDevExt->HcFdoDeviceObject,
                                                Irp);
        break;

    case IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS:
        ntStatus = USBPORT_IoctlGetTopologyAddress(PdoDeviceObject,
                                                   rhDevExt->HcFdoDeviceObject,
                                                   Irp);
        break;

    case IOCTL_INTERNAL_USB_GET_DUMP_DATA:
        ntStatus = USBPORT_IoctlGetDumpPointers(PdoDeviceObject,
                                                rhDevExt->HcFdoDeviceObject,
                                                Irp);
        break;

    case IOCTL_INTERNAL_USB_FREE_DUMP_DATA:
        ntStatus = USBPORT_IoctlFreeDumpPointers(PdoDeviceObject,
                                                rhDevExt->HcFdoDeviceObject,
                                                Irp);
        break;

    case IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS:
        ntStatus = USBPORT_IoctlNotifyForwardProgress(PdoDeviceObject,
                                                      rhDevExt->HcFdoDeviceObject,
                                                      Irp);
        break;

    case IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST:
        IoMarkIrpPending(Irp);
        return STATUS_PENDING;

    default:

        DbgTrace((UsbpDebugTrace, "'INVALID INTERNAL DEVICE CONTROL %x\n",
            irpStack->Parameters.DeviceIoControl.IoControlCode));
        //TEST_TRAP();
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // DO NOT TOUCH THE IRP FROM THIS POINT ON
    //
    USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, UsbIoRequest, ntStatus, 0);

USBPORT_PdoInternalDeviceControlIrp_Done:

    return ntStatus;
}


NTSTATUS
USBPORT_UsbFdoUserIoctl(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PULONG BytesReturned
    )
/*++

Routine Description:

    The goal here is to have all user mode APIS
    pass thru this routine so that the parameter
    validation is handled in one place.

    We define user APIS supported by the PORT FDO
    thru this single IOCTL.

    The USUSER APIs use the same buffer for input and output,
    hence the InputBufferLength and the OutputbufferLength must
    always be equal.


    We get here if the client sends the IOCTL_USB_USER_REQUEST.
    We only return NTSTATUS failure if the header portion of the
    buffer is invalid.



Arguments:

    DeviceObject - Fdo for USB HC

    BytesRetrned - ptr to bytes to return to caller, initially zero

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PUSBUSER_REQUEST_HEADER header = NULL;
    PDEVICE_EXTENSION devExt;
    PUCHAR ioBufferO = NULL;
    ULONG inputBufferLength, outputBufferLength, allocLength;
    ULONG ioBufferLength;
    PUCHAR myIoBuffer = NULL;
    BOOLEAN alloced;
    PDEVICE_OBJECT rhPdo = NULL;

    DbgTrace((UsbpDebugTrace, "'USBPORT_FdoUserIoctl\n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);
    USBPORT_ASSERT(irpStack->Parameters.DeviceIoControl.IoControlCode ==
        IOCTL_USB_USER_REQUEST);

    ioBufferO = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // one bug that the driver verifier does not catch is if we trash the
    // iobuffer passed to us.
    //
    // We shadow this buffer to one we allocate, this way DV should catch
    // us if we trash memory.
    // We do this in retail even though this a perf hit since the NT guys
    // tend to favor stability more than perf and this is not a hot-path.
    //
    // If DV is ever modified to do this for us we cann remove this code.
    //

    allocLength = max(inputBufferLength, outputBufferLength);

    if (allocLength) {
        ALLOC_POOL_Z(myIoBuffer, NonPagedPoolNx, allocLength);
    } else {
        myIoBuffer = NULL;
    }

    if (myIoBuffer != NULL) {
        alloced = TRUE;
        RtlCopyMemory(myIoBuffer,
                      ioBufferO,
                      inputBufferLength);
    } else {
        // if alloc fails just fall back to the original
        alloced = FALSE;
        myIoBuffer = ioBufferO;
    }

    ioBufferLength = inputBufferLength;

    DbgTrace((UsbpDebugTrace,  "'ioBuffer = %p - %p\n", ioBufferO, myIoBuffer));
    DbgTrace((UsbpDebugTrace,  "'inputBufferLength %d\n", inputBufferLength));
    DbgTrace((UsbpDebugTrace,  "'outputBufferLength %d\n", outputBufferLength));
    LOGENTRY(FdoDeviceObject, LOG_MISC, 'uIOC', ioBufferO, inputBufferLength,
            outputBufferLength);

    // some initial parameter validation

    // bogus buffer lengths
    if (inputBufferLength != outputBufferLength) {

        ntStatus = STATUS_INVALID_PARAMETER;
        goto USBPORT_UsbFdoUserIoctl_Done;
    }

    // must have at least enough for a header
    if (ioBufferLength < sizeof(USBUSER_REQUEST_HEADER)) {

        ntStatus = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_UsbFdoUserIoctl_Done;
    }

    // header buffer is valid, at this point we return
    // STATUS_SUCCESS to the caller and fill in the header
    // with the appropriate error code
    ntStatus = STATUS_SUCCESS;

    // validate the header buffer parameters

    header = (PUSBUSER_REQUEST_HEADER) myIoBuffer;

    // assume success, set return values
    header->UsbUserStatusCode = UsbUserSuccess;
    *BytesReturned =
        header->ActualBufferLength = sizeof(*header);

    // length set in header should be the same
    // as length passed in the ioctl
    if (header->RequestBufferLength != ioBufferLength) {

        header->UsbUserStatusCode =
            UsbUserInvalidHeaderParameter;
        goto USBPORT_UsbFdoUserIoctl_Done;
    }

    // we have a valid header and client buffer, attempt to execute
    // the api

    // validate rules for Api codes
    // is this an API that only works when the root hub is disabled?

    rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_UsbFdoUserIoctl);

    //
    // We allow some IOCTLs to function event though the controller is
    // suspended, basically as long as they don't call the miniport we are safe
    //

    // If this IOCTL requires the miniport to be initialized, make sure
    // the miniport is not in the process of resuming.  If it is,
    // then we will fail the ioctl.


    if( !USBUSER_OP_CONTROLLER_NOT_INITIALIZED_OK(header->UsbUserRequest) &&
        devExt->Fdo.DmTimerFlags.DecodesOff) {

        if (devExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_KdPrint((1, "USBUSER FAIL (Decodes OFF)\n"));
        } else {
            USBPORT_KdPrint((1, "USBUSER FAIL (!USBUSER_OP_CONTROLLER_NOT_INITIALIZED_OK)\n"));
        }
        header->UsbUserStatusCode = UsbUserFeatureDisabled;
        ntStatus = STATUS_UNSUCCESSFUL;
        goto USBPORT_UsbFdoUserIoctl_Done;
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'usbR', header->UsbUserRequest, header, 0);

    switch (header->UsbUserRequest) {
    case USBUSER_OP_SEND_ONE_PACKET:
        USBPORT_KdPrint((1, "USBUSER_OP_SEND_ONE_PACKET\n"));
        if (USBPORT_CheckLength(header, sizeof(PACKET_PARAMETERS))) {
            // DCA api
            USBPORT_UserSendOnePacket(FdoDeviceObject, header,
                        (PPACKET_PARAMETERS) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_OP_RAW_RESET_PORT:
        USBPORT_KdPrint((1, "USBUSER_OP_RAW_RESET_PORT\n"));
        if (USBPORT_CheckLength(header, sizeof(RAW_RESET_PORT_PARAMETERS))) {
            // DCA api
            USBPORT_UserRawResetPort(FdoDeviceObject, rhPdo, header,
                        (PRAW_RESET_PORT_PARAMETERS) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_SET_ROOTPORT_FEATURE:
        USBPORT_KdPrint((1, "USBUSER_SET_ROOTPORT_FEATURE\n"));
        if (USBPORT_CheckLength(header, sizeof(RAW_ROOTPORT_FEATURE))) {
            // DCA api
            USBPORT_UserSetRootPortFeature(FdoDeviceObject, header,
                        (PRAW_ROOTPORT_FEATURE) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_CLEAR_ROOTPORT_FEATURE:
        USBPORT_KdPrint((1, "USBUSER_CLEAR_ROOTPORT_FEATURE\n"));
        if (USBPORT_CheckLength(header, sizeof(RAW_ROOTPORT_FEATURE))) {
            // DCA api
            USBPORT_UserClearRootPortFeature(FdoDeviceObject, header,
                        (PRAW_ROOTPORT_FEATURE) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_ROOTPORT_STATUS:
        // DCA api
        USBPORT_KdPrint((1, "USBUSER_GET_ROOTPORT_STATUS\n"));
        if (USBPORT_CheckLength(header, sizeof(RAW_ROOTPORT_PARAMETERS))) {
            USBPORT_GetRootPortStatus(FdoDeviceObject, header,
                        (PRAW_ROOTPORT_PARAMETERS) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_CONTROLLER_INFO_0:
        USBPORT_KdPrint((1, "USBUSER_GET_CONTROLLER_INFO_0\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_CONTROLLER_INFO_0))) {
            USBPORT_UserGetControllerInfo_0(FdoDeviceObject, header,
                        (PUSB_CONTROLLER_INFO_0) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_CONTROLLER_DRIVER_KEY:
        USBPORT_KdPrint((1, "USBUSER_GET_CONTROLLER_DRIVER_KEY\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_UNICODE_NAME))) {
            USBPORT_UserGetControllerKey(FdoDeviceObject, header,
                        (PUSB_UNICODE_NAME) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_ROOTHUB_SYMBOLIC_NAME:
        USBPORT_KdPrint((1, "USBUSER_GET_ROOTHUB_SYMBOLIC_NAME\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_UNICODE_NAME))) {
            USBPORT_UserGetRootHubName(FdoDeviceObject, header,
                        (PUSB_UNICODE_NAME) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_PASS_THRU:
        USBPORT_KdPrint((1, "USBUSER_PASS_THRU\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_PASS_THRU_PARAMETERS))) {
            USBPORT_UserPassThru(FdoDeviceObject, header,
                        (PUSB_PASS_THRU_PARAMETERS) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_BANDWIDTH_INFORMATION:
        USBPORT_KdPrint((1, "USBUSER_GET_BANDWIDTH_INFORMATION\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_BANDWIDTH_INFO))) {
            USBPORT_UserGetBandwidthInformation(FdoDeviceObject, header,
                        (PUSB_BANDWIDTH_INFO) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_POWER_STATE_MAP:
        USBPORT_KdPrint((1, "USBUSER_GET_POWER_STATE_MAP\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_POWER_INFO))) {
            USBPORT_UserPowerInformation(FdoDeviceObject, header,
                        (PUSB_POWER_INFO) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_GET_BUS_STATISTICS_0:
        USBPORT_KdPrint((1, "USBUSER_GET_BUS_STATISTICS_0\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_BUS_STATISTICS_0))) {
            USBPORT_UserGetBusStatistics0(FdoDeviceObject, header,
                        (PUSB_BUS_STATISTICS_0) (myIoBuffer+sizeof(*header)));
        }
        break;

    case USBUSER_USB_REFRESH_HCT_REG:
        USBPORT_KdPrint((1, "USBUSER_USB_REFRESH_HCT_REG\n"));
        if (USBPORT_CheckLength(header, sizeof(ULONG))) {
            USBPORT_UserRefreshHctRegKeys(FdoDeviceObject, rhPdo, header,
                        (PULONG) (myIoBuffer+sizeof(*header)));
        }
        break;

//    case USBUSER_GET_BUS_STATISTICS_0_AND_RESET:
//        if (USBPORT_CheckLength(header, sizeof(USB_BUS_STATISTICS_0))) {
//            USBPORT_UserGetBusStatistics0(FdoDeviceObject, header,
//                        (PUSB_BUS_STATISTICS_0) (myIoBuffer+sizeof(*header)),
//                        TRUE);
//        }
//        break;

    case USBUSER_GET_USB_DRIVER_VERSION:
        USBPORT_KdPrint((1, "USBUSER_GET_USB_DRIVER_VERSION\n"));
        if (USBPORT_CheckLength(header, sizeof(USB_DRIVER_VERSION_PARAMETERS))) {
            USBPORT_UserGetDriverVersion(FdoDeviceObject, header,
                        (PUSB_DRIVER_VERSION_PARAMETERS) (myIoBuffer+sizeof(*header)));
        }
        break;

    default:
        USBPORT_KdPrint((1, "USBUSER UsbUserInvalidRequestCode\n"));
        header->UsbUserStatusCode = UsbUserInvalidRequestCode;
    }

    // this will be at least the size of the header

    if (header->RequestBufferLength > header->ActualBufferLength) {
        // if the packet buffer is larger then just return 'actual length'
        *BytesReturned =
            header->ActualBufferLength;
    } else {
        // packet buffer is smaller -- return the size of the
        // packet buffer passed in
        *BytesReturned = header->RequestBufferLength;
    }

    if (BytesReturned) {
        USBPORT_KdPrint((1, "USBUSER BytesReturned %d NTSTATUS %08.8x\n", *BytesReturned, ntStatus));
    }
USBPORT_UsbFdoUserIoctl_Done:

    if (alloced) {
        // copy the data no matter what we put in it
        // USBPORT_ASSERT(outputBufferLength == inputBufferLength);
        RtlCopyMemory(ioBufferO,
                      myIoBuffer,
                      allocLength);
        UsbFreePool(myIoBuffer);
    }

    // hold the ref until we finish
    if (rhPdo) {
        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_UsbFdoUserIoctl);
    }

    return ntStatus;
}


VOID
USBPORT_UserRefreshHctRegKeys(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT RootHubPdo,
    PUSBUSER_REQUEST_HEADER Header,
    PULONG Flags
    )
/*++

Routine Description:

    Refresh global registry keys in usbport.  This is used by the HCT after setting global registry keys that
    modify usbport behavior.

Arguments:

    DeviceObject - Fdo for USB HC

    RootHubPdo - root hub pdo (referenced).

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_USER_ERROR_CODE usbUserStatus;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort, %p\n", devExt));

    usbUserStatus = UsbUserSuccess;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rfg', 0, 0, Flags);

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserRefreshHctRegKeys\n"));
    Header->UsbUserStatusCode = UsbUserSuccess;

    // refresh are global key state

    USBPORT_Check_ForcePortsHighSpeed_Enabled(FdoDeviceObject);
    USBPORT_Check_DCA_Enabled(FdoDeviceObject);

    //IoInvalidateDeviceRelations(RootHubPdo, BusRelations);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rfg>', 0, 0, 0);

}


VOID
USBPORT_UserSendOnePacket(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PPACKET_PARAMETERS PacketParameters
    )
/*++

Routine Description:

    Execute a single step transaction

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    HW_32BIT_PHYSICAL_ADDRESS phys;
    PUCHAR va = NULL, mpData = NULL;
    ULONG length = 0, mpDataLength = 0;
    ULONG timeElapsed = 0, timeout = 0;
    MP_PACKET_PARAMETERS mpPacket = {0};
    USBD_STATUS usbdStatus;
    USB_USER_ERROR_CODE usbUserStatus;
    ETW_USBUSER_OP_SEND_ONE_PACKET_DATA EtwSendOnePacketData;
    BOOLEAN ParameterError;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserSendOnePacket\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Default return status is UsbUserSuccess unless a parameter
    // validation error occurs or MP_StartSendOnePacket() fails.
    //
    usbUserStatus = UsbUserSuccess;
    ParameterError = FALSE;

    RtlZeroMemory(&EtwSendOnePacketData, sizeof(EtwSendOnePacketData));

    do {
        // limit single packet to 64k
        EtwSendOnePacketData.DataLength = PacketParameters->DataLength;
        if (PacketParameters->DataLength > 0x10000) {
            LOGENTRY(FdoDeviceObject, LOG_MISC, 'Tbts', 0, 0,
                     PacketParameters->DataLength);
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        EtwSendOnePacketData.Timeout = PacketParameters->Timeout;
        if (PacketParameters->Timeout >= 21474) {
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        if (!USBPORT_Check_DCA_Enabled(FdoDeviceObject)) {
            usbUserStatus = UsbUserFeatureDisabled;
            ParameterError = TRUE;
            break;
        }

        if (devExt->Fdo.PowerFlags.Suspended) {
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        // extra length check is needed since we
        // have embedded data
        // if we get here we know packet parameters are valid
        length = sizeof(*Header) + sizeof(PACKET_PARAMETERS) - 4 +
                 PacketParameters->DataLength;

        if (length > Header->RequestBufferLength) {
            LOGENTRY(FdoDeviceObject, LOG_MISC, 'Tsma', length, 0,
                     Header->RequestBufferLength);
            usbUserStatus = UsbUserBufferTooSmall;
            ParameterError = TRUE;
            break;
        }

        Header->ActualBufferLength = length;

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ssPK', &mpPacket, 0,
                 PacketParameters);

        // dump the PacketParameters
        DbgTrace((UsbpDebugTrace, "'DeviceAddress %d\n", PacketParameters->DeviceAddress));
        DbgTrace((UsbpDebugTrace, "'EndpointAddress %d\n", PacketParameters->EndpointAddress));
        DbgTrace((UsbpDebugTrace, "'MaximumPacketSize %d\n", PacketParameters->MaximumPacketSize));
        DbgTrace((UsbpDebugTrace, "'Flags %08.8x\n", PacketParameters->Flags));
        DbgTrace((UsbpDebugTrace, "'ErrorCount %d\n", PacketParameters->ErrorCount));

        // build up request for miniport

        length = devExt->Fdo.ScratchCommonBuffer->TotalLength;
        va = devExt->Fdo.ScratchCommonBuffer->MiniportVa;
        phys = devExt->Fdo.ScratchCommonBuffer->MiniportPhys;

        EtwSendOnePacketData.DeviceAddress = PacketParameters->DeviceAddress;
        EtwSendOnePacketData.EndpointAddress = PacketParameters->EndpointAddress;
        EtwSendOnePacketData.MaximumPacketSize = PacketParameters->MaximumPacketSize;
        EtwSendOnePacketData.UsbPacketFlags = PacketParameters->Flags;

        mpPacket.DeviceAddress = PacketParameters->DeviceAddress;
        mpPacket.EndpointAddress = PacketParameters->EndpointAddress;
        mpPacket.MaximumPacketSize = PacketParameters->MaximumPacketSize;
        if (PacketParameters->Flags & USB_PACKETFLAG_SETUP) {
            mpPacket.Type = ss_Setup;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_ASYNC_IN) {
            DbgTrace((UsbpDebugTrace, "'Async In\n"));
            mpPacket.Type = ss_In;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_ASYNC_OUT) {
            DbgTrace((UsbpDebugTrace, "'Async Out\n"));
            mpPacket.Type = ss_Out;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_ISO_IN) {
            DbgTrace((UsbpDebugTrace, "'Iso In\n"));
            mpPacket.Type = ss_Iso_In;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_ISO_OUT) {
            DbgTrace((UsbpDebugTrace, "'Iso Out\n"));
            mpPacket.Type = ss_Iso_Out;
        } else {
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        EtwSendOnePacketData.HubDeviceAddress = PacketParameters->HubDeviceAddress;
        EtwSendOnePacketData.PortTTNumber = PacketParameters->PortTTNumber;

        if (PacketParameters->Flags & USB_PACKETFLAG_LOW_SPEED) {
            DbgTrace((UsbpDebugTrace, "'LowSpeed\n"));
            mpPacket.Speed = ss_Low;
            mpPacket.HubDeviceAddress = PacketParameters->HubDeviceAddress;
            mpPacket.PortTTNumber = PacketParameters->PortTTNumber;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_FULL_SPEED) {
            DbgTrace((UsbpDebugTrace, "'FullSpeed\n"));
            mpPacket.Speed = ss_Full;
            mpPacket.HubDeviceAddress = PacketParameters->HubDeviceAddress;
            mpPacket.PortTTNumber = PacketParameters->PortTTNumber;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_HIGH_SPEED) {
            DbgTrace((UsbpDebugTrace, "'HighSpeed\n"));
            mpPacket.Speed = ss_High;
        } else {
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        if (PacketParameters->Flags & USB_PACKETFLAG_TOGGLE0) {
            DbgTrace((UsbpDebugTrace, "'Toggle0\n"));
            mpPacket.Toggle = ss_Toggle0;
        } else if (PacketParameters->Flags & USB_PACKETFLAG_TOGGLE1) {
            DbgTrace((UsbpDebugTrace, "'Toggle1\n"));
            mpPacket.Toggle = ss_Toggle1;
        } else {
            usbUserStatus = UsbUserInvalidParameter;
            ParameterError = TRUE;
            break;
        }

        // All parameters have been validated at this point.

        mpData = &PacketParameters->Data[0];
        mpDataLength = PacketParameters->DataLength;
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ssDT', mpData, mpDataLength, 0);

        if (mpDataLength != 0 &&
            (mpPacket.Type == ss_Setup ||
             mpPacket.Type == ss_Out ||
             mpPacket.Type == ss_Iso_Out)) {

            USHORT DataLength = (USHORT)mpDataLength;

            // "USBUSER_OP_SEND_ONE_PACKET Dispatch with Data"
            // template: tid_USBPORT_Usbuser_Op_Send_One_Packet_Data
            //
            USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_DISPATCH_DATA,
                                    NULL,

                                    &EtwSendOnePacketData,
                                    sizeof(EtwSendOnePacketData),

                                    &DataLength,
                                    sizeof(DataLength),

                                    mpData,
                                    DataLength);

        } else {

            // "USBUSER_OP_SEND_ONE_PACKET Dispatch"
            // template: tid_USBPORT_Usbuser_Op_Send_One_Packet
            //
            USBPORT_EtwWriteHcData1(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_DISPATCH,
                                    NULL,

                                    &EtwSendOnePacketData,
                                    sizeof(EtwSendOnePacketData));
        }

        MP_StartSendOnePacket(devExt,
                              &mpPacket,
                              mpData,
                              &mpDataLength,
                              va,
                              phys,
                              length,
                              &usbdStatus,
                              mpStatus);

        if (USBMP_STATUS_SUCCESS != mpStatus) {
            usbUserStatus = UsbUserMiniportError;
            break;
        }

        timeElapsed = 0;
        timeout = PacketParameters->Timeout;

        do {
            // Wait 10 ms
            USBPORT_Wait(FdoDeviceObject, 10);
            timeElapsed += 10;

            // Wait a user specified time, one time only.
            if (timeout) {
                USBPORT_Wait(FdoDeviceObject, PacketParameters->Timeout);
                usbdStatus = USBD_STATUS_DEV_NOT_RESPONDING;
                timeout = 0;
            }

            if (5000 == timeElapsed) {
                // waited enough
                usbdStatus = USBD_STATUS_DEV_NOT_RESPONDING;
            }

            // If the transfer is not complete (i.e. it is still Active)
            // when MINIPORT_EndSendOnePacket() is called, AND
            // UsbdStatus is not set to USBD_STATUS_DEV_NOT_RESPONDING
            // nor USBD_STATUS_CANCELED indicating a transfer timeout,
            // AND the endpoint is the default control endpoint (i.e.
            // EndpointNumber is zero), then MINIPORT_EndSendOnePacket()
            // should:
            // (1) Return USBMP_STATUS_BUSY.
            //
            // If an active transfer timeout does occur then
            // MINIPORT_EndSendOnePacket() should:
            // (1) Set UsbdStatus to USBD_STATUS_CANCELED.
            // (2) Return USBMP_STATUS_BUSY.
            // Then MINIPORT_EndSendOnePacket() will be called again
            // after a 10 ms wait.
            //
            // If MINIPORT_EndSendOnePacket() is called when when
            // UsbdStatus is set to USBD_STATUS_CANCELED it should:
            // (1) Set UsbdStatus to the final transfer status.
            // (2) Return USBMP_STATUS_SUCCESS.
            //
            MP_EndSendOnePacket(devExt,
                                &mpPacket,
                                mpData,
                                &mpDataLength,
                                va,
                                phys,
                                length,
                                &usbdStatus,
                                mpStatus);

        } while (USBMP_STATUS_BUSY == mpStatus);

        if (USBD_STATUS_CANCELED == usbdStatus) {

            // A transfer timeout occurred.

            mpDataLength = 0;

            // "USBUSER_OP_SEND_ONE_PACKET Timeout"
            // template: tid_USBPORT_Usbuser_Op_Send_One_Packet
            //
            USBPORT_EtwWriteHcData1(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_TIMEOUT,
                                    NULL,

                                    &EtwSendOnePacketData,
                                    sizeof(EtwSendOnePacketData));
        }

        // allow one frame to pass before continuing
        USBPORT_Wait(FdoDeviceObject, 1);

        PacketParameters->DataLength = mpDataLength;
        PacketParameters->UsbdStatusCode = usbdStatus;

        EtwSendOnePacketData.UsbdStatus = usbdStatus;

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'ssDE', mpData, mpDataLength,
                 PacketParameters);

    } WHILE (0);

    if (ParameterError) {

        // A parameter validation error occurred

        // "USBUSER_OP_SEND_ONE_PACKET Dispatch"
        // template: tid_USBPORT_Usbuser_Op_Send_One_Packet
        //
        USBPORT_EtwWriteHcData1(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_DISPATCH,
                                NULL,

                                &EtwSendOnePacketData,
                                sizeof(EtwSendOnePacketData));
    }

    if (mpDataLength != 0 &&
          (mpPacket.Type == ss_In ||
           mpPacket.Type == ss_Iso_In)) {

        USHORT DataLength = (USHORT)mpDataLength;

        // "USBUSER_OP_SEND_ONE_PACKET Complete with Data"
        // template: tid_USBPORT_Usbuser_Op_Send_One_Packet_Data
        //
        USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_COMPLETE_DATA,
                                NULL,

                                &EtwSendOnePacketData,
                                sizeof(EtwSendOnePacketData),

                                &DataLength,
                                sizeof(DataLength),

                                mpData,
                                DataLength);

    } else {

        // ""USBUSER_OP_SEND_ONE_PACKET Complete"
        // template: tid_USBPORT_Usbuser_Op_Send_One_Packet_UsbStatus
        //
        USBPORT_EtwWriteHcData2(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_USBUSER_OP_SEND_ONE_PACKET_COMPLETE,
                                NULL,

                                &EtwSendOnePacketData,
                                sizeof(EtwSendOnePacketData),

                                &usbUserStatus,
                                sizeof(usbUserStatus));
    }

    Header->UsbUserStatusCode = usbUserStatus;
}


VOID
USBPORT_UserRawResetPort(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT RootHubPdo,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_RESET_PORT_PARAMETERS Parameters
    )
/*++

Routine Description:

    Cycle a specific Root Port by directly accessing the HC registers.
    Not good to do if the hub driber is loaded.  The hct tests (usbhct)
    use this API after unloading the usbhub FDO driver.

Arguments:

    DeviceObject - Fdo for USB HC

    RootHubPdo - root hub pdo (referenced).

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    USB_USER_ERROR_CODE usbUserStatus;
    RH_PORT_STATUS portStatus;
    ULONG loopCount;
    PDEVICE_EXTENSION rhDevExt;
    USBPORT_PNP_STATE rhPnpState;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort, %p\n", devExt));

    usbUserStatus = UsbUserSuccess;
    portStatus.ul = 0;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrsp', 0, 0, Parameters->PortNumber);

    // "USBUSER_OP_RAW_RESET_PORT Dispatch"
    // template: tid_USBPORT_HC_PortNumber
    //
    USBPORT_EtwWriteHcData1(FdoDeviceObject,
                            &USBPORT_ETW_EVENT_USBUSER_OP_RAW_RESET_PORT_DISPATCH,
                            NULL,

                            &Parameters->PortNumber,
                            sizeof(Parameters->PortNumber));

    do {

        if (!USBPORT_ValidateRootPortApi(FdoDeviceObject, Parameters->PortNumber)) {
            usbUserStatus = UsbUserInvalidParameter;
            break;
        }

        if (!USBPORT_Check_DCA_Enabled(FdoDeviceObject)) {
            usbUserStatus = UsbUserFeatureDisabled;
            break;
        }

        if (RootHubPdo != NULL) {
            GET_DEVICE_EXT(rhDevExt, RootHubPdo);
            ASSERT_PDOEXT(rhDevExt);

            rhPnpState = USBPORT_GetPnpState(rhDevExt);

            USBPORT_ASSERT(rhPnpState == PnpSoftRemoved);
            if (rhPnpState != PnpSoftRemoved) {
                // expect root hub to be soft removed
                usbUserStatus = UsbUserInvalidParameter;
                break;
            }
        }

        DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort: Setting port power\n"));

        // Power the port via MINIPORT_RH_SetFeaturePortPower()
        //
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_SetFeaturePortPower(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

        //
        // Wait the required time for the port power to stabilize.
        //
        //  512ms --> Max port power to power good time for root hub
        //  100ms --> Max time for device to have power stabilize
        //
        // After this time, the device must have signalled connect on the device
        //

        USBPORT_Wait(FdoDeviceObject, 612);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                           &portStatus, mpStatus);

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrs1', 0, 0, (ULONG_PTR) portStatus.ul);

        // "USBUSER_OP_RAW_RESET_PORT GetPortStatus after SetFeaturePortPower"
        // template: tid_USBPORT_HC_PortNumber_PortStatus
        //
        USBPORT_EtwWriteHcData2(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_USBUSER_OP_RAW_RESET_PORT_STATUS1,
                                NULL,

                                &Parameters->PortNumber,
                                sizeof(Parameters->PortNumber),

                                &portStatus,
                                sizeof(portStatus));

        //
        // Device should have signalled connect, if not, it's an error.
        //

        if (portStatus.Connected) {

            //
            // Provide another 100ms for debounce interval.
            //

            USBPORT_Wait(FdoDeviceObject, 100);

            //
            // Reset the device
            //

            DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort: Setting port reset\n"));

            // Reset the port via MINIPORT_RH_SetFeaturePortReset()
            //
            USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
            MPF(devExt).MINIPORT_RH_SetFeaturePortReset(
                devExt->Fdo.MiniportDeviceData,
                Parameters->PortNumber);
            USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

            //
            // wait for reset change, this process is drive by the
            // HC root hub hardware or miniport
            //

            loopCount = 0;

            USBPORT_Wait(FdoDeviceObject, 50);

            MPRH_GetPortStatus(devExt,
                               (USHORT)(Parameters->PortNumber), &portStatus, mpStatus);

            LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrs2', 0, 0,
                     (ULONG_PTR) portStatus.ul);

            //
            // Some hubs seem to be taking longer than 20 ms to signal reset change
            // This is a loop to give it up to another 20ms.
            //

            while (!portStatus.ResetChange && loopCount < 20) {

                loopCount++;

                USBPORT_Wait(FdoDeviceObject, 5);

                MPRH_GetPortStatus(devExt,
                                   (USHORT)(Parameters->PortNumber), &portStatus, mpStatus);
            }

            DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort: loopCount = %d\n",
                      loopCount));

            LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrs3', 0, loopCount,
                     (ULONG_PTR) portStatus.ul);

            // "USBUSER_OP_RAW_RESET_PORT GetPortStatus after SetFeaturePortReset"
            // template: tid_USBPORT_HC_PortNumber_PortStatus
            //
            USBPORT_EtwWriteHcData2(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_USBUSER_OP_RAW_RESET_PORT_STATUS2,
                                    NULL,

                                    &Parameters->PortNumber,
                                    sizeof(Parameters->PortNumber),

                                    &portStatus,
                                    sizeof(portStatus));

            if (portStatus.ResetChange) {

                DbgTrace((UsbpDebugTrace, "'USBPORT_UserRawResetPort: Clearing reset "
                          "change\n"));

                // Clear the ResetChange via MINIPORT_RH_ClearFeaturePortResetChange
                //
                USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
                MPF(devExt).MINIPORT_RH_ClearFeaturePortResetChange(
                    devExt->Fdo.MiniportDeviceData,
                    Parameters->PortNumber);
                USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

                MPRH_GetPortStatus(devExt,
                                   (USHORT)(Parameters->PortNumber),
                                   &portStatus,
                                   mpStatus);

                LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrs4', 0, 0,
                         (ULONG_PTR) portStatus.ul);

                // "USBUSER_OP_RAW_RESET_PORT GetPortStatus after ClearFeaturePortResetChange"
                // template: tid_USBPORT_HC_PortNumber_PortStatus
                //
                USBPORT_EtwWriteHcData2(FdoDeviceObject,
                                        &USBPORT_ETW_EVENT_USBUSER_OP_RAW_RESET_PORT_STATUS3,
                                        NULL,

                                        &Parameters->PortNumber,
                                        sizeof(Parameters->PortNumber),

                                        &portStatus,
                                        sizeof(portStatus));

                //
                // Wait an additional 10 seconds for device reset recovery
                //

                USBPORT_Wait(FdoDeviceObject, 10);

            } else {

                DbgTrace((UsbpDebugTrace,
                          "'USBPORT_UserRawResetPort: reset change not set\n"
                          "'PortStatus = 0x%x\n", portStatus.ul));

                TEST_TRAP();
            }

        } else {
            usbUserStatus = UsbUserNoDeviceConnected;
        }

    } WHILE (0);

    // status is low 16 bits
    Parameters->PortStatus = (USHORT) portStatus.ul;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'Rrs>', 0, portStatus.ul, usbUserStatus);

    Header->UsbUserStatusCode = usbUserStatus;

    // "USBUSER_OP_RAW_RESET_PORT Complete"
    // template: tid_USBPORT_HC_PortNumber_UsbStatus
    //
    USBPORT_EtwWriteHcData2(FdoDeviceObject,
                            &USBPORT_ETW_EVENT_USBUSER_OP_RAW_RESET_PORT_COMPLETE,
                            NULL,

                            &Parameters->PortNumber,
                            sizeof(Parameters->PortNumber),

                            &usbUserStatus,
                            sizeof(usbUserStatus));
}


VOID
USBPORT_GetRootPortStatus(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_PARAMETERS Parameters
    )
/*++

Routine Description:

    Cycle a specific Root Port

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    USB_USER_ERROR_CODE usbUserStatus;
    USBPRIV_ROOTPORT_STATUS portStatusInfo;

    DbgTrace((UsbpDebugTrace, "'USBPORT_GetRootPortStatus\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    usbUserStatus = UsbUserSuccess;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gRPs', 0, 0, Parameters->PortNumber);

    if (!USBPORT_ValidateRootPortApi(FdoDeviceObject, Parameters->PortNumber)) {
        Header->UsbUserStatusCode =
            usbUserStatus = UsbUserInvalidParameter;
        return;
    }

    if (!USBPORT_Check_DCA_Enabled(FdoDeviceObject)) {
        Header->UsbUserStatusCode = UsbUserFeatureDisabled;
        return;
    }

    portStatusInfo.PortNumber = (USHORT) Parameters->PortNumber;
    portStatusInfo.PortStatus.ul = 0;

    MP_PassThru(devExt,
                (LPGUID) &GUID_USBPRIV_ROOTPORT_STATUS,
                sizeof(portStatusInfo),
                &portStatusInfo, // Info,
                mpStatus);

    if (USBMP_STATUS_NOT_SUPPORTED == mpStatus) {
        MPRH_GetPortStatus(devExt, portStatusInfo.PortNumber,
                            &(portStatusInfo.PortStatus), mpStatus);
    }

    // status is low 16 bits
    Parameters->PortStatus = (USHORT) portStatusInfo.PortStatus.ul;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gRP>', 0, 0, usbUserStatus);

    Header->UsbUserStatusCode = usbUserStatus;
}

VOID
USBPORT_UserGetControllerInfo_0(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_CONTROLLER_INFO_0 ControllerInfo_0
    )
/*++

Routine Description:

    Execute a single step transaction

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.
--*/
{
    PDEVICE_EXTENSION devExt;
    ROOTHUB_DATA hubData;
    RH_HUB_CHARATERISTICS rhChars;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserGetControllerInfo_0\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ControllerInfo_0->HcFeatureFlags = 0;

    MPRH_GetRootHubData(devExt, &hubData);

    ControllerInfo_0->NumberOfRootPorts =
             hubData.NumberOfPorts;

    rhChars.us = hubData.HubCharacteristics.us;

    if (rhChars.PowerSwitchType == USBPORT_RH_POWER_SWITCH_PORT) {
            ControllerInfo_0->HcFeatureFlags |=
                USB_HC_FEATURE_FLAG_PORT_POWER_SWITCHING;
    }

    switch(devExt->Fdo.SSP_Support) {
    case SSPsupport_RegEnabled:
        ControllerInfo_0->HcFeatureFlags |= USB_HC_FEATURE_FLAG_SEL_SUSPEND;
        break;
    case SSPsupport_RegDisabled:
    case SSPsupport_GlobalDisabled:
    case SSPsupport_HcDisabled:
    case SSPsupport_ErrorDisabled:
        break;
    default:
        USBPORT_ASSERTMSG("Unexpcted devExt->Fdo.SSP_Support",FALSE);
    }

    if (devExt->Fdo.Flags.LegacyBIOS) {
        ControllerInfo_0->HcFeatureFlags |=
            USB_HC_FEATURE_LEGACY_BIOS;
    }

    ControllerInfo_0->PciVendorId =
        devExt->Fdo.PciVendorId;

    ControllerInfo_0->PciDeviceId =
        devExt->Fdo.PciDeviceId;

    ControllerInfo_0->PciRevision =
        (UCHAR) devExt->Fdo.PciRevisionId;

    ControllerInfo_0->ControllerFlavor =
        devExt->Fdo.HcFlavor;

    Header->UsbUserStatusCode =
        UsbUserSuccess;
}


VOID
USBPORT_UserGetControllerKey(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_UNICODE_NAME ControllerKey
    )
/*++

Routine Description:

    Returns the Driver key associated with this symbolic link for the
    host controller.

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.
--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus;
    ULONG userLength, actualLength = 0;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserGetControllerKey\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (!NT_SUCCESS(RtlULongSub(Header->RequestBufferLength, sizeof(*Header), &userLength)) ||
        !NT_SUCCESS(RtlULongSub(userLength, sizeof(USB_UNICODE_NAME), &userLength)))
    {
        USBPORT_ASSERTMSG("RtlULongSub failed",FALSE);
        return;
    }

    // note: this will cause us to return a NULL terminated
    // key
    RtlZeroMemory(ControllerKey, userLength);

    ntStatus = IoGetDeviceProperty(
        devExt->Fdo.PhysicalDeviceObject,
        DevicePropertyDriverKeyName,
        userLength,
        &ControllerKey->String[0],
        &actualLength);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'dKEY', &ControllerKey->String[0], userLength,
            actualLength);

    if (NT_SUCCESS(ntStatus)) {
        Header->UsbUserStatusCode = UsbUserSuccess;
        ControllerKey->Length = actualLength + sizeof(UNICODE_NULL);
    } else if (ntStatus == STATUS_BUFFER_TOO_SMALL) {
        Header->UsbUserStatusCode = UsbUserBufferTooSmall;
    } else {
        Header->UsbUserStatusCode = UsbUserInvalidParameter;
    }

    Header->ActualBufferLength =
        actualLength+sizeof(*Header)+sizeof(USB_UNICODE_NAME);

}


VOID
USBPORT_UserGetRootHubName(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_UNICODE_NAME RootHubName
    )
/*++

Routine Description:

    Returns the Driver key associated with this symbolic link for the
    host controller.

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.
--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus;
    ULONG userLength, actualLength;
    UNICODE_STRING hubNameUnicodeString;
    PDEVICE_OBJECT rhPdo;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserGetRootHubName\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // we should not get here unl;ess this hold true
    USBPORT_ASSERT(Header->RequestBufferLength >=
                   sizeof(USB_UNICODE_NAME)+sizeof(*Header));

    // userlength
    userLength = Header->RequestBufferLength - sizeof(*Header)
        - sizeof(USB_UNICODE_NAME);

    // note: this will cause us to return a NULL terminated
    // key
    RtlZeroMemory(RootHubName, userLength);
    do {

        rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_UserGetRootHubName);
        if (rhPdo == NULL) {
            break;
        }

        ntStatus = USBPORT_GetSymbolicName(FdoDeviceObject,
                                           rhPdo,
                                           &hubNameUnicodeString);
        actualLength = 0;

        if (NT_SUCCESS(ntStatus)) {
            ULONG n;

            actualLength = hubNameUnicodeString.Length;
            n = hubNameUnicodeString.Length;
            if (n > userLength) {
                n = userLength;
                ntStatus = STATUS_BUFFER_TOO_SMALL;
            }
            if (n) {
                RtlCopyMemory(&RootHubName->String[0],
                              hubNameUnicodeString.Buffer,
                              n);
            }
            LOGENTRY(FdoDeviceObject, LOG_MISC, 'Huns',
                &hubNameUnicodeString, 0, 0);

            RtlFreeUnicodeString(&hubNameUnicodeString);
        }

        LOGENTRY(FdoDeviceObject, LOG_MISC, 'gHNM', ntStatus, userLength,
                actualLength);

        if (NT_SUCCESS(ntStatus)) {
            Header->UsbUserStatusCode = UsbUserSuccess;
            RootHubName->Length = actualLength + sizeof(UNICODE_NULL);
        } else if (ntStatus == STATUS_BUFFER_TOO_SMALL) {
            Header->UsbUserStatusCode = UsbUserBufferTooSmall;
        } else {
            Header->UsbUserStatusCode = UsbUserInvalidParameter;
        }

        Header->ActualBufferLength =
            actualLength+sizeof(*Header)+sizeof(USB_UNICODE_NAME);

        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_UserGetRootHubName);
    }WHILE (0);
}


VOID
USBPORT_UserPassThru(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_PASS_THRU_PARAMETERS PassThru
    )
/*++

Routine Description:

   Handles pass-thru apis for the Miniport

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.
--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG length;
    USB_MINIPORT_STATUS mpStatus;
    ULONG parameterLength;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserPassThru\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // we should not get here unless this hold true
    USBPORT_ASSERT(Header->RequestBufferLength >=
                   sizeof(USB_PASS_THRU_PARAMETERS)+sizeof(*Header));

    // limit pass thru blocks to 64k
    if (PassThru->ParameterLength > 0x10000) {
        Header->UsbUserStatusCode = UsbUserInvalidParameter;
        return;
    }

    // extra length check is needed since we
    // have embedded data
    // if we get here we know packet parameters are valid
    length = sizeof(*Header) + sizeof(USB_PASS_THRU_PARAMETERS) - 4 +
                PassThru->ParameterLength;

    if (length > Header->RequestBufferLength) {
        LOGENTRY(FdoDeviceObject, LOG_MISC, 'Tsma', length, 0,
            Header->RequestBufferLength);
        Header->UsbUserStatusCode = UsbUserBufferTooSmall;
        return;
    }

    // userlength
    parameterLength = PassThru->ParameterLength;

    Header->ActualBufferLength =
        sizeof(*Header)+sizeof(USB_PASS_THRU_PARAMETERS) +
            parameterLength;

    // call the miniport
    MP_PassThru(devExt,
                &PassThru->FunctionGUID,
                parameterLength,
                &PassThru->Parameters,
                mpStatus);

    if (mpStatus == USBMP_STATUS_SUCCESS) {
        Header->UsbUserStatusCode = UsbUserSuccess;
        DbgTrace((UsbpDebugTrace, "'USBPORT_UserPassThru Success\n"));
    } else {
        Header->UsbUserStatusCode = UsbUserMiniportError;
        DbgTrace((UsbpDebugTrace, "'USBPORT_UserPassThru Error\n"));
    }

}


WDMUSB_POWER_STATE
WdmUsbSystemPowerState(
    SYSTEM_POWER_STATE SystemPowerState
    )
/*++

Routine Description:

Arguments:

Return Value:

    WDMUSB_POWER_STATE that matches the WDM power state passed in
--*/
{
    switch(SystemPowerState) {
    case PowerSystemWorking:
        return WdmUsbPowerSystemWorking;
    case PowerSystemSleeping1:
        return WdmUsbPowerSystemSleeping1;
    case PowerSystemSleeping2:
        return WdmUsbPowerSystemSleeping2;
    case PowerSystemSleeping3:
        return WdmUsbPowerSystemSleeping3;
    case PowerSystemHibernate:
        return WdmUsbPowerSystemHibernate;
    case PowerSystemShutdown:
        return WdmUsbPowerSystemShutdown;
    }

    return WdmUsbPowerNotMapped;
}


WDMUSB_POWER_STATE
WdmUsbDevicePowerState(
    DEVICE_POWER_STATE DevicePowerState
    )
/*++

Routine Description:

Arguments:

Return Value:

    WDMUSB_POWER_STATE that matches the WDM power state passed in
--*/
{
    switch(DevicePowerState) {
    case PowerDeviceUnspecified:
        return WdmUsbPowerDeviceUnspecified;
    case PowerDeviceD0:
        return WdmUsbPowerDeviceD0;
    case PowerDeviceD1:
        return WdmUsbPowerDeviceD1;
    case PowerDeviceD2:
        return WdmUsbPowerDeviceD2;
    case PowerDeviceD3:
        return WdmUsbPowerDeviceD3;
    }

    return WdmUsbPowerNotMapped;
}


VOID
USBPORT_MapPowerStateInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_POWER_INFO PowerInformation,
    PDEVICE_CAPABILITIES HcCaps,
    PDEVICE_CAPABILITIES RhCaps
    )
/*++

Routine Description:

Arguments:

Return Value:

    none.
--*/
{
    PHC_POWER_STATE hcPowerState = NULL;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    PowerInformation->RhDeviceWake =
        WdmUsbDevicePowerState(RhCaps->DeviceWake);
    PowerInformation->RhSystemWake =
        WdmUsbSystemPowerState(RhCaps->SystemWake);
    PowerInformation->HcDeviceWake =
        WdmUsbDevicePowerState(HcCaps->DeviceWake);
    PowerInformation->HcSystemWake =
        WdmUsbSystemPowerState(HcCaps->SystemWake);

    switch (PowerInformation->SystemState) {
    case WdmUsbPowerSystemWorking:
        PowerInformation->RhDevicePowerState =
            WdmUsbDevicePowerState(RhCaps->DeviceState[PowerSystemWorking]);
        PowerInformation->HcDevicePowerState =
            WdmUsbDevicePowerState(HcCaps->DeviceState[PowerSystemWorking]);
//        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
//            PowerSystemWorking);
        break;

    case WdmUsbPowerSystemSleeping1:
        PowerInformation->RhDevicePowerState =
            WdmUsbDevicePowerState(RhCaps->DeviceState[PowerSystemSleeping1]);
        PowerInformation->HcDevicePowerState =
            WdmUsbDevicePowerState(HcCaps->DeviceState[PowerSystemSleeping1]);
        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                               &devExt->Fdo.HcPowerStateTbl,
                                               PowerSystemSleeping1);
        break;

    case WdmUsbPowerSystemSleeping2:
        PowerInformation->RhDevicePowerState =
            WdmUsbDevicePowerState(RhCaps->DeviceState[PowerSystemSleeping2]);
        PowerInformation->HcDevicePowerState =
            WdmUsbDevicePowerState(HcCaps->DeviceState[PowerSystemSleeping2]);
        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                               &devExt->Fdo.HcPowerStateTbl,
                                               PowerSystemSleeping2);
        break;

    case WdmUsbPowerSystemSleeping3:
        PowerInformation->RhDevicePowerState =
            WdmUsbDevicePowerState(RhCaps->DeviceState[PowerSystemSleeping3]);
        PowerInformation->HcDevicePowerState =
            WdmUsbDevicePowerState(HcCaps->DeviceState[PowerSystemSleeping3]);
        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                               &devExt->Fdo.HcPowerStateTbl,
                                               PowerSystemSleeping3);
        break;

     case WdmUsbPowerSystemHibernate:
        PowerInformation->RhDevicePowerState =
            WdmUsbDevicePowerState(RhCaps->DeviceState[PowerSystemHibernate]);
        PowerInformation->HcDevicePowerState =
            WdmUsbDevicePowerState(HcCaps->DeviceState[PowerSystemHibernate]);
        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                               &devExt->Fdo.HcPowerStateTbl,
                                               PowerSystemHibernate);
        break;
    }


    if (hcPowerState != NULL) {
        switch(hcPowerState->Attributes) {
        case HcPower_Y_Wakeup_Y:
            PowerInformation->CanWakeup = 1;
            PowerInformation->IsPowered = 1;
            break;
        case HcPower_N_Wakeup_N:
            PowerInformation->CanWakeup = 0;
            PowerInformation->IsPowered = 0;
            break;
        case HcPower_Y_Wakeup_N:
            PowerInformation->CanWakeup = 0;
            PowerInformation->IsPowered = 1;
            break;
        case HcPower_N_Wakeup_Y:
            PowerInformation->CanWakeup = 1;
            PowerInformation->IsPowered = 0;
            break;
        }
    } else {
        PowerInformation->CanWakeup = 0;
        PowerInformation->IsPowered = 0;
    }

    PowerInformation->LastSystemSleepState =
        WdmUsbSystemPowerState(devExt->Fdo.LastSystemSleepState);
}


VOID
USBPORT_UserPowerInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_POWER_INFO PowerInformation
    )
/*++

Routine Description:

   Handles power info API

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.
--*/
{
    PDEVICE_EXTENSION devExt, rhDevExt;
    PDEVICE_CAPABILITIES hcDeviceCaps, rhDeviceCaps;
    PDEVICE_OBJECT rhPdo;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserPowerInformation\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_UserPowerInformation);
    if (rhPdo == NULL) {
        Header->ActualBufferLength =
            sizeof(*Header)+sizeof(USB_POWER_INFO);
        Header->UsbUserStatusCode = UsbUserDeviceNotStarted;
        return;
    }

    GET_DEVICE_EXT(rhDevExt, rhPdo);
    ASSERT_PDOEXT(rhDevExt);

    //if (!TEST_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STARTED)) {
    if (USBPORT_GetPnpState(rhDevExt) != PnpStarted) {

        Header->ActualBufferLength =
            sizeof(*Header)+sizeof(USB_POWER_INFO);
        Header->UsbUserStatusCode = UsbUserDeviceNotStarted;
        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_UserPowerInformation);
        return;
    }

    rhDeviceCaps = &rhDevExt->DeviceCapabilities;
    hcDeviceCaps = &devExt->DeviceCapabilities;

    // we should not get here unless this holds true
    USBPORT_ASSERT(Header->RequestBufferLength >=
                   sizeof(USB_POWER_INFO)+sizeof(*Header));

    USBPORT_MapPowerStateInformation(
        FdoDeviceObject,
        PowerInformation,
        hcDeviceCaps,
        rhDeviceCaps);

    Header->ActualBufferLength =
        sizeof(*Header)+sizeof(USB_POWER_INFO);

    USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_UserPowerInformation);

}


VOID
USBPORT_UserGetBandwidthInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_BANDWIDTH_INFO BandwidthInfo
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    ULONG asyncBW, bw = 0, period;
    ULONG highSpeedTransfersIn32ms[6] = {256, 128, 64, 32, 16, 8};
    KIRQL irql;
    PLIST_ENTRY listEntry = NULL;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserGetBandwidthInformation\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gBWi', 0, 0, 0);

    RtlZeroBytes(BandwidthInfo, sizeof(USB_BANDWIDTH_INFO));

    BandwidthInfo->DeviceCount =
        USBPORT_GetDeviceCount(FdoDeviceObject);
    BandwidthInfo->TotalBusBandwidth =
        devExt->Fdo.TotalBusBandwidth;

    //
    // return allocation based on bits/32 msec
    // segment of bus time.  Data is always stored
    // in bits/frame(ms)
    //
    BandwidthInfo->Total32secBandwidth =
        devExt->Fdo.TotalBusBandwidth * 32;

    if(USBPORT_IS_USB20(devExt)){

        //
        // High speed controller
        //
        // To keep a common interface with the full/low speed bw numbers, we will report
        // our bandwidth in bits/32ms(32 frames).
        //
        // The data stored is currently in bits/microframe.
        // An interrupt or iso transaction can occur every 1,2,4,8,16 or 32 microframes.
        // Since there are 8 microframes/frame we need to convert using the following
        // formula:
        //
        // Period   Transfers/32 frames(256 microframes)    n
        //
        // 1        256                                     0
        // 2        128                                     1
        // 4        64                                      2
        // 8        32                                      3
        // 16       16                                      4
        // 32       8                                       5
        //

        // 20% bw reserved
        asyncBW = BandwidthInfo->TotalBusBandwidth/5;

        BandwidthInfo->AllocedBulkAndControl =
            asyncBW * 32;

        //
        // Go through each endpoint and collect our current bw usage
        //
        USBPORT_AcquireEpListLock(FdoDeviceObject, &irql);

        listEntry = devExt->Fdo.GlobalEndpointList.Flink;

        while (listEntry != &(devExt->Fdo.GlobalEndpointList)) {
            PHCD_ENDPOINT endpoint;

            endpoint = (PHCD_ENDPOINT) CONTAINING_RECORD(
                    listEntry,
                    struct _HCD_ENDPOINT,
                    GlobalLink);

            ASSERT_ENDPOINT(endpoint);

#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "endpoint cannot be NULL; see ASSERT_ENDPOINT")
            if(endpoint == NULL ||
               endpoint == USB_ZERO_BW_ENDP_HANDLE ||
               endpoint->EpFlags.RootHub){
                listEntry = endpoint->GlobalLink.Flink;
                continue;
            }

            if(endpoint->Parameters.TransferType == Isochronous ||
               endpoint->Parameters.TransferType == Interrupt){

                //
                // First we will convert bytes/microframe to bits/microframe.
                //
                //bw = endpointContext->Ep.calc_bus_time * 8;
                bw = MPx_QueryEpBandwidthData(devExt, endpoint);
                bw = bw*8;

                //
                // Determine the period of the endpoint
                //

                USBPORT_GET_BIT_SET(endpoint->Parameters.Period, period);
                USBPORT_ASSERT(period<6);

                //
                // Adjust to bits/32ms
                //
                if (period < 6) {
                    bw = bw * highSpeedTransfersIn32ms[period];
                }

                if(endpoint->Parameters.TransferType == Isochronous){
                    BandwidthInfo->AllocedIso += bw;
                }
                else{
                    switch (period){
                    case 0:
                        BandwidthInfo->AllocedInterrupt_1ms += bw;
                        break;
                    case 1:
                        BandwidthInfo->AllocedInterrupt_2ms += bw;
                        break;
                    case 2:
                        BandwidthInfo->AllocedInterrupt_4ms += bw;
                        break;
                    case 3:
                        BandwidthInfo->AllocedInterrupt_8ms += bw;
                        break;
                    case 4:
                         BandwidthInfo->AllocedInterrupt_16ms += bw;
                         break;
                    case 5:
                        BandwidthInfo->AllocedInterrupt_32ms += bw;
                        break;
                    default:
                        break;
                    }
                }
            }

            listEntry = endpoint->GlobalLink.Flink;

        }

        USBPORT_ReleaseEpListLock(FdoDeviceObject, irql);
    }

    else{
        //
        // Full speed 1.1 controller
        //

        //
        // The data stored is currently in bits/frame.
        // An interrupt transaction can occur every 1,2,4,8,16 or 32 microframes.
        //
        // Period   Transfers/32 frames
        //
        // 1        32
        // 2        16
        // 4        8
        // 8        4
        // 16       2
        // 32       1
        //

        // 10% bw reserved
        asyncBW = BandwidthInfo->TotalBusBandwidth/10;

        BandwidthInfo->AllocedBulkAndControl =
            asyncBW * 32;

        BandwidthInfo->AllocedIso =
            devExt->Fdo.AllocedIsoBW*32;
        BandwidthInfo->AllocedInterrupt_1ms =
            devExt->Fdo.AllocedInterruptBW[0]*32;
        BandwidthInfo->AllocedInterrupt_2ms =
            devExt->Fdo.AllocedInterruptBW[1]*16;
        BandwidthInfo->AllocedInterrupt_4ms =
            devExt->Fdo.AllocedInterruptBW[2]*8;
        BandwidthInfo->AllocedInterrupt_8ms =
            devExt->Fdo.AllocedInterruptBW[3]*4;
        BandwidthInfo->AllocedInterrupt_16ms =
            devExt->Fdo.AllocedInterruptBW[4]*2;
        BandwidthInfo->AllocedInterrupt_32ms =
            devExt->Fdo.AllocedInterruptBW[5]*1;
    }

    Header->ActualBufferLength =
        sizeof(*Header)+sizeof(USB_BANDWIDTH_INFO);
}


VOID
USBPORT_UserGetBusStatistics0(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_BUS_STATISTICS_0 BusStatistics0
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt, rhDevExt;
    KIRQL irql;
    USBPORT_PNP_STATE pnpState;
    PDEVICE_OBJECT rhPdo;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_UserGetBusStatistics0);

    if (rhPdo != NULL) {
        GET_DEVICE_EXT(rhDevExt, rhPdo);
        ASSERT_PDOEXT(rhDevExt);
    } else {
        rhDevExt = NULL;
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gBus', rhDevExt,
        BusStatistics0, 0);

    BusStatistics0->DeviceCount =
        USBPORT_GetDeviceCount(FdoDeviceObject);

    KeQuerySystemTime(&BusStatistics0->CurrentSystemTime);

    BusStatistics0->CurrentUsbFrame = MPf_Get32BitFrameNumber(devExt);

    // lock the stat counters while we read them
    KeAcquireSpinLock(&devExt->Fdo.StatCounterSpin, &irql);

    BusStatistics0->BulkBytes =
        devExt->Fdo.PerformanceData.StatBulkDataBytes;
    BusStatistics0->IsoBytes =
        devExt->Fdo.PerformanceData.StatIsoDataBytes;
    BusStatistics0->InterruptBytes =
        devExt->Fdo.PerformanceData.StatInterruptDataBytes;
    BusStatistics0->ControlDataBytes =
        devExt->Fdo.PerformanceData.StatControlDataBytes;

    BusStatistics0->RootHubDevicePowerState = 4;
    BusStatistics0->RootHubEnabled = FALSE;

    BusStatistics0->WorkerSignalCount =
        devExt->Fdo.StatWorkSignalCount;
    BusStatistics0->HardResetCount =
        devExt->Fdo.StatHardResetCount;
    BusStatistics0->WorkerIdleTimeMs =
        devExt->Fdo.StatWorkIdleTime;

    BusStatistics0->CommonBufferBytes =
        devExt->Fdo.StatCommonBufferBytes;


    if (rhDevExt != NULL) {
        pnpState = USBPORT_GetPnpState(rhDevExt);

        if (pnpState == PnpStarted) {
            BusStatistics0->RootHubEnabled = TRUE;
        }
        if (TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {
            // report D2 if the controller is suspended
            BusStatistics0->RootHubDevicePowerState = 2;
        } else {
            // report D0
            BusStatistics0->RootHubDevicePowerState = 0;
        }
#if 0
        switch(rhDevExt->Dx_State) {
        case PowerDeviceD0:
            BusStatistics0->RootHubDevicePowerState = 0;
            break;
        case PowerDeviceD1:
            BusStatistics0->RootHubDevicePowerState = 1;
            break;
        case PowerDeviceD2:
            BusStatistics0->RootHubDevicePowerState = 2;
            break;
        case PowerDeviceD3:
            BusStatistics0->RootHubDevicePowerState = 3;
            break;
        }
#endif
    }

    BusStatistics0->PciInterruptCount =
        devExt->Fdo.StatPciInterruptCount;

//    if (ResetCounters) {
//        devExt->Fdo.StatControlDataBytes =
//        devExt->Fdo.StatInterruptBytes =
//        devExt->Fdo.StatIsoBytes =
//        devExt->Fdo.StatBulkBytes =
//        devExt->Fdo.PciInterruptCount = 0;
//    }

    KeReleaseSpinLock(&devExt->Fdo.StatCounterSpin, irql);

    Header->ActualBufferLength =
        sizeof(*Header)+sizeof(USB_BUS_STATISTICS_0);

    if (rhPdo) {
        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_UserGetBusStatistics0);
    }
}


NTSTATUS
USBPORT_LegacyGetUnicodeName(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PULONG BytesReturned
    )
/*++

Routine Description:

    Handles the old style IOCTL to fetch the USB host controllers
    driver key name.

    NOTE: This function may be removed  if we ever fix the UI to use the
    newer APIs.


Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSBUSER_CONTROLLER_UNICODE_NAME request;
    NTSTATUS ntStatus;
    ULONG need;
    PUSB_HCD_DRIVERKEY_NAME ioBufferO;
    PIO_STACK_LOCATION irpStack;
    ULONG outputBufferLength;
    ULONG ioCtl;

    ASSERT_PASSIVE();

    *BytesReturned = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    ioCtl = irpStack->Parameters.DeviceIoControl.IoControlCode;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);
    USBPORT_ASSERT(ioCtl == IOCTL_GET_HCD_DRIVERKEY_NAME ||
                   ioCtl == IOCTL_USB_GET_ROOT_HUB_NAME);

    DbgTrace((UsbpDebugTrace,"'WARNING: Caller using obsolete user mode IOCTL\n"));

    ioBufferO = Irp->AssociatedIrp.SystemBuffer;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    // Bail immediately if the output buffer is too small
    //
    if (outputBufferLength < sizeof(USB_HCD_DRIVERKEY_NAME)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    // first get the driver key name using the standard API

    need = sizeof(USBUSER_CONTROLLER_UNICODE_NAME);

retry:

    ALLOC_POOL_Z(request,
                 PagedPool, need);

    if (request != NULL) {

        request->Header.RequestBufferLength = need;
        if (ioCtl == IOCTL_GET_HCD_DRIVERKEY_NAME) {
            request->Header.UsbUserRequest =
                USBUSER_GET_CONTROLLER_DRIVER_KEY;
            USBPORT_UserGetControllerKey(
                FdoDeviceObject,
                &request->Header,
                &request->UnicodeName);
        } else {
            request->Header.UsbUserRequest =
                USBUSER_GET_ROOTHUB_SYMBOLIC_NAME;
            USBPORT_UserGetRootHubName(
                FdoDeviceObject,
                &request->Header,
                &request->UnicodeName);
        }

        if (request->Header.UsbUserStatusCode ==
            UsbUserBufferTooSmall) {

            need = request->Header.ActualBufferLength;

            UsbFreePool(request);
            goto retry;

        }

        if (request->Header.UsbUserStatusCode ==
                            UsbUserSuccess) {

            // map the result into the callers buffer

            // note: actual length is the size of the request structure
            // plus the name.

            ntStatus = RtlULongAdd(request->UnicodeName.Length,
                                   sizeof(ULONG),
                                   &ioBufferO->ActualLength);

            if (outputBufferLength >= ioBufferO->ActualLength &&
                NT_SUCCESS(ntStatus) ) {

                // we can return the name
                RtlCopyMemory(&ioBufferO->DriverKeyName[0],
                              &request->UnicodeName.String[0],
                              request->UnicodeName.Length);

                *BytesReturned = ioBufferO->ActualLength;

            } else {
                ioBufferO->DriverKeyName[0] =  L'\0';
                *BytesReturned = sizeof(USB_HCD_DRIVERKEY_NAME);
            }

            ntStatus = STATUS_SUCCESS;

        } else {
            ntStatus = STATUS_UNSUCCESSFUL;
        }

        UsbFreePool(request);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gUNN', ntStatus, ioBufferO,
        *BytesReturned);

    return ntStatus;
}


NTSTATUS
USBPORT_GetSymbolicName(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT DeviceObject,
    PUNICODE_STRING SymbolicNameUnicodeString
    )
/*++

Routine Description:

    Return the symbolic name for the device object with leading
    \xxx\ removed

Arguments:

    DeviceObject - Fdo OR Pdo for USB HC

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION fdoDevExt;
    PDEVICE_EXTENSION devExt;
    ULONG length, offset = 0;
    WCHAR *pwch = NULL, *buffer = NULL;
    NTSTATUS ntStatus = STATUS_BOGUS;
    ULONG bufferLength;
    PUNICODE_STRING tmpUnicodeString;

    ASSERT_PASSIVE();
    GET_DEVICE_EXT(fdoDevExt, FdoDeviceObject);
    ASSERT_FDOEXT(fdoDevExt);

    GET_DEVICE_EXT(devExt, DeviceObject);

    tmpUnicodeString =
        &devExt->SymbolicLinkName;

    //
    // make sure there is enough room for the length,
    // string and the NULL
    //

    // assuming the string is \xxx\name strip of '\xxx\' where
    // x is zero or more chars

    pwch = &tmpUnicodeString->Buffer[0];

    // Under NT, if the controller is banged out in DeviceManager,
    // this will be NULL.

    if (pwch == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    bufferLength = tmpUnicodeString->Length;

    ALLOC_POOL_Z(buffer,
                 PagedPool, bufferLength);

    if (buffer != NULL) {

        USBPORT_ASSERT(*pwch == '\\');
        if (*pwch == '\\') {
            pwch++;
            while (*pwch != '\\' && *pwch) {
                pwch++;
            }
            USBPORT_ASSERT(*pwch == '\\');
            if (*pwch == '\\') {
                pwch++;
            }
            offset = (ULONG)((PUCHAR)pwch -
                (PUCHAR)&tmpUnicodeString->Buffer[0]);
        }

        length = tmpUnicodeString->Length - offset;

        RtlCopyMemory(buffer,
                      &tmpUnicodeString->Buffer[offset/2],
                      length);

        RtlInitUnicodeString(SymbolicNameUnicodeString,
                             buffer);

        ntStatus = STATUS_SUCCESS;

    } else {

        // init to null so subsequent free will not crash
        RtlInitUnicodeString(SymbolicNameUnicodeString,
                             NULL);

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;

    }

    return ntStatus;
}


VOID
USBPORT_UserGetDriverVersion(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_DRIVER_VERSION_PARAMETERS Parameters
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpDebugTrace, "'USBPORT_UserGetDriverVersion\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gDrv', 0, 0, 0);

    Parameters->DriverTrackingCode = USBPORT_TRACKING_ID;
    /* USBDI Api set supported */
    Parameters->USBDI_Version = USBDI_VERSION;
    /* USB USER Api Set supported */
    Parameters->USBUSER_Version = USBUSER_VERSION;

    /* set to true if checked vesrion(s) on
       the stack are loaded
    */
#if DBG
    Parameters->CheckedPortDriver = TRUE;
    Parameters->CheckedMiniportDriver = TRUE;
#else
    Parameters->CheckedPortDriver = FALSE;
    Parameters->CheckedMiniportDriver = FALSE;
#endif

    Header->ActualBufferLength =
        sizeof(*Header)+sizeof(USB_DRIVER_VERSION_PARAMETERS);
}

BOOLEAN
USBPORT_ValidateRootPortApi(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG PortNumber
    )
/*++

Routine Description:

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    ROOTHUB_DATA hubData;

    DbgTrace((UsbpDebugTrace, "'USBPORT_ValidateRootPortApi\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    MPRH_GetRootHubData(devExt, &hubData);
    if (PortNumber > hubData.NumberOfPorts ||
        PortNumber == 0) {
        return FALSE;
    }

    if (devExt->Fdo.PowerFlags.Suspended) {
        return FALSE;
    }

    return TRUE;
}

/*
    Determine if direct controller access is enabled in registry.
*/

BOOLEAN
USBPORT_DCA_KeyEnabled(
    PDEVICE_OBJECT FdoDeviceObject
    )
{
    NTSTATUS ntStatus;
    RTL_QUERY_REGISTRY_TABLE QueryTable[2];
    PWCHAR usb = L"usb";
    ULONG k = 0;
    ULONG dca = 0;

    ASSERT_PASSIVE();

    // bios hacks
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = ENABLE_DCA;
    QueryTable[k].EntryContext = &dca;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &dca;
    QueryTable[k].DefaultLength = sizeof(dca);
    k++;

    // stop
    QueryTable[k].QueryRoutine = NULL;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = NULL;

    ntStatus = RtlQueryRegistryValues(
                RTL_REGISTRY_SERVICES,
                usb,
                QueryTable,     // QueryTable
                NULL,           // Context
                NULL);          // Environment


    return NT_SUCCESS(ntStatus) && dca == 1;
}


BOOLEAN
USBPORT_ForcePortsHighSpeed_KeyEnabled(
    PDEVICE_OBJECT FdoDeviceObject
    )
{
    NTSTATUS ntStatus;
    RTL_QUERY_REGISTRY_TABLE QueryTable[2];
    PWCHAR usb = L"usb";
    ULONG k = 0;
    ULONG diag = 0;

    ASSERT_PASSIVE();

    // bios hacks
    QueryTable[k].QueryRoutine = USBPORT_GetConfigValue;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = FORCE_PORTS_HS;
    QueryTable[k].EntryContext = &diag;
    QueryTable[k].DefaultType = REG_DWORD;
    QueryTable[k].DefaultData = &diag;
    QueryTable[k].DefaultLength = sizeof(diag);
    k++;

    // stop
    QueryTable[k].QueryRoutine = NULL;
    QueryTable[k].Flags = 0;
    QueryTable[k].Name = NULL;

    ntStatus = RtlQueryRegistryValues(
                RTL_REGISTRY_SERVICES,
                usb,
                QueryTable,     // QueryTable
                NULL,           // Context
                NULL);          // Environment


    return NT_SUCCESS(ntStatus) && diag == 1;
}

/*
    Determine if direct controller access is enabled in registry.
    Also validate that the caller has load driver privilige.
*/

BOOLEAN
USBPORT_Check_DCA_Enabled(
    PDEVICE_OBJECT FdoDeviceObject
    )
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // verify privilage
    // enable this code if the security APIs ever get
    // added to WDM

    if (USBPORT_DCA_KeyEnabled(FdoDeviceObject)) {

#if 0
        LUID securityLuid;

        // first check for correct permissions
        securityLuid = RtlConvertLongToLuid(SE_LOAD_DRIVER_PRIVILEGE);

        return SeSinglePrivilegeCheck(securityLuid, UserMode);
 #endif
        devExt->Fdo.Flags.HctDCAEnabled = 1;

        return TRUE;
    }

    devExt->Fdo.Flags.HctDCAEnabled = 0;
    return FALSE;
}


BOOLEAN
USBPORT_Check_ForcePortsHighSpeed_Enabled(
    PDEVICE_OBJECT FdoDeviceObject
    )
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // verify privilage
    // enable this code if the security APIs ever get
    // added to WDM

    if (USBPORT_ForcePortsHighSpeed_KeyEnabled(FdoDeviceObject)) {


#if 0
        LUID securityLuid;

        // first check for correct permissions
        securityLuid = RtlConvertLongToLuid(SE_LOAD_DRIVER_PRIVILEGE);

        return SeSinglePrivilegeCheck(securityLuid, UserMode);
 #endif
        devExt->Fdo.Flags.HctForcePortsHighSpeed = 1;

        return TRUE;
    }

     devExt->Fdo.Flags.HctForcePortsHighSpeed = 0;

    return FALSE;
}


NTSTATUS
USBPORT_IoctlGetTopologyAddress(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Return basic controller information

Arguments:

Return Value:

    ntStatus
--*/
{
    PUSB_TOPOLOGY_ADDRESS address = NULL;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION fdoDevExt;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(fdoDevExt, FdoDeviceObject);
    ASSERT_FDOEXT(fdoDevExt);

    address = ioStack->Parameters.Others.Argument1;

    if(!address){
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(address, sizeof(USB_TOPOLOGY_ADDRESS));

    address->PciBusNumber = fdoDevExt->Fdo.BusNumber;
    address->PciDeviceNumber = fdoDevExt->Fdo.BusDevice;
    address->PciFunctionNumber = fdoDevExt->Fdo.BusFunction;

    return nts;
}


NTSTATUS
USBPORT_IoctlGetDumpPointers(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Return basic controller information

Arguments:

Return Value:

    ntStatus
--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION fdoDevExt;
    PUSB_DUMP_CONTROLLER_INFO controllerBuffer;
    PUSB_DUMP_DEVICE_INFO deviceInfo;
    PEHCI_CRASHDUMP_CONTEXT dumpContext;
    PDMA_OPERATIONS dmaOp = NULL;
    PUSBD_DEVICE_HANDLE *devHandle;
    ULONG contextIdx;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(fdoDevExt, FdoDeviceObject);
    ASSERT_FDOEXT(fdoDevExt);

    //
    // Verify the ioctl parameters from the usbstor driver.
    //
    if (Irp->RequestorMode != KernelMode) {
        nts = STATUS_INVALID_DEVICE_REQUEST;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(USB_DUMP_CONTROLLER_INFO)) {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(USB_DUMP_DEVICE_INFO)) {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT;contextIdx++) {
        if (fdoDevExt->DumpContext[contextIdx] == NULL) {
            break;
        }
    }

    if (contextIdx == DUMP_MAX_CONTEXT_COUNT) {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    controllerBuffer = Irp->UserBuffer;
    deviceInfo = Irp->AssociatedIrp.SystemBuffer;

    if (deviceInfo->EndpointCount > DUMP_MAX_ENDPOINT_COUNT) {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    controllerBuffer->ControllerContext =
        ExAllocatePoolWithTag(NonPagedPoolNx,
                            sizeof(EHCI_CRASHDUMP_CONTEXT),
                            DUMP_POOL_TAG);
    if (controllerBuffer->ControllerContext == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    dumpContext = controllerBuffer->ControllerContext;
    if (fdoDevExt->Fdo.UseSingleDmaAdapter != FALSE) {
        dumpContext->DmaAdapter =
            fdoDevExt->Fdo.AdapterObject;
    } else {
        dumpContext->DmaAdapter =
            fdoDevExt->Fdo.CommonBufferAdapterObject;
    }

    dmaOp = dumpContext->DmaAdapter->DmaOperations;

    //
    // If this controller only supports 32 bit addresses
    // we will need to allocate a buffer in the lower
    // 32 bit physical address rhange to double buffer
    // any pages that are > 4GB physical address
    //
    USBPORT_ASSERT(controllerBuffer->ControllerCommonBufferSize != 0);
    USBPORT_ASSERT(controllerBuffer->ControllerCommonBufferSize <= DUMP_BUFFER_SCRATCH_SIZE);

    if (fdoDevExt->Fdo.Bit64AddressingCapability == 1) {
        dumpContext->CommonBufferSize =
            DUMP_BUFFER_HEADER_SIZE + DUMP_BUFFER_SCRATCH_SIZE;
        dumpContext->Bit64AddressingCapability = TRUE;
    } else {
        dumpContext->CommonBufferSize =
            DUMP_BUFFER_HEADER_SIZE + DUMP_BUFFER_SCRATCH_SIZE + DUMP_DOUBLE_BUFFER_SIZE;
        dumpContext->Bit64AddressingCapability = FALSE;
    }

    RtlZeroMemory(dumpContext->EndpointInfo,
                  DUMP_MAX_ENDPOINT_COUNT * sizeof(USB_DUMP_DEVICE_ENDPOINT_INFO));
    RtlCopyMemory(dumpContext->EndpointInfo,
                  deviceInfo->EndpointInfo,
                  deviceInfo->EndpointCount*sizeof(USB_DUMP_DEVICE_ENDPOINT_INFO));

    dumpContext->CommonBufferPool =
        dmaOp->AllocateCommonBuffer(
                 dumpContext->DmaAdapter,
                 dumpContext->CommonBufferSize,
                 &dumpContext->CommonBufferPA,
#if defined(_ARM_) || (_ARM64_)
                 FALSE);
#else
                 TRUE);
#endif

    if (dumpContext->CommonBufferPool == NULL) {
        ExFreePool(controllerBuffer->ControllerContext);
        nts = STATUS_INSUFFICIENT_RESOURCES;
        goto USBPORT_IoctlGetDumpPointers_Exit;
    }

    controllerBuffer->MajorVersion = 1;
    controllerBuffer->MinorVersion = 0;
    controllerBuffer->ControllerCommonBufferVA = (PUCHAR)dumpContext->CommonBufferPool+DUMP_BUFFER_HEADER_SIZE;
    controllerBuffer->ControllerCommonBufferPA.QuadPart = dumpContext->CommonBufferPA.QuadPart+DUMP_BUFFER_HEADER_SIZE;

    devHandle = (PUSBD_DEVICE_HANDLE *)deviceInfo->DeviceHandle;
    dumpContext->DeviceAddress =
                devHandle[0]->DeviceAddress;
    dumpContext->ParentHubDeviceAddress =
                devHandle[1]->DeviceAddress;

    RtlCopyMemory(&dumpContext->PortPath,
                &devHandle[0]->EtwUsbDevice.PortPath,
                sizeof(USB_DUMP_PORT_PATH));

    dumpContext->USBPortFDO = fdoDevExt->HcFdoDeviceObject;

    dumpContext->BaseRegisterPointer =
                fdoDevExt->Fdo.HcResources.DeviceRegisters;

    dumpContext->Hibernate = FALSE;
    dumpContext->ControllerTransition = FALSE;

    dumpContext->AlternateSettingId = deviceInfo->AlternateSettingId;
    dumpContext->ConfigurationId = deviceInfo->ConfigurationId;
    dumpContext->InterfaceId = deviceInfo->InterfaceId;

    //
    // Save the dump context in FDO DevExt so that it can be updated
    //
    fdoDevExt->DumpContext[contextIdx] = dumpContext;

    fdoDevExt->Fdo.MiniportDriver->RegistrationPacket.MINIPORT_InitializeDumpInterface(
        controllerBuffer);

USBPORT_IoctlGetDumpPointers_Exit:

    return nts;
}

NTSTATUS
USBPORT_IoctlFreeDumpPointers(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Return basic controller information

Arguments:

Return Value:

    ntStatus
--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION fdoDevExt;
    PUSB_DUMP_CONTROLLER_INFO controllerBuffer;
    PEHCI_CRASHDUMP_CONTEXT dumpContext;
    PDMA_OPERATIONS dmaOp = NULL;
    ULONG contextIdx;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(fdoDevExt, FdoDeviceObject);
    ASSERT_FDOEXT(fdoDevExt);

    if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(USB_DUMP_CONTROLLER_INFO)) {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto USBPORT_IoctlFreeDumpPointers_Exit;
    }

    controllerBuffer = Irp->AssociatedIrp.SystemBuffer;

    fdoDevExt->Fdo.MiniportDriver->RegistrationPacket.MINIPORT_CleanupDumpInterface(
        NULL);

    dumpContext = controllerBuffer->ControllerContext;

    for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
        if (fdoDevExt->DumpContext[contextIdx] == dumpContext) {
            break;
        }
    }

    if (contextIdx == DUMP_MAX_CONTEXT_COUNT) {
        goto USBPORT_IoctlFreeDumpPointers_Exit;
    }

    dmaOp = dumpContext->DmaAdapter->DmaOperations;
    dmaOp->FreeCommonBuffer(
             dumpContext->DmaAdapter,
             dumpContext->CommonBufferSize,
             dumpContext->CommonBufferPA,
             dumpContext->CommonBufferPool,
             TRUE);

    if (controllerBuffer->ControllerContext != NULL) {
        ExFreePool(controllerBuffer->ControllerContext);
        controllerBuffer->ControllerContext = NULL;
        fdoDevExt->DumpContext[contextIdx] = NULL;
    }

USBPORT_IoctlFreeDumpPointers_Exit:

    return nts;
}


NTSTATUS
USBPORT_IoctlNotifyForwardProgress(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Return basic controller information

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION rhDevExt;
    PIO_STACK_LOCATION irpStack;
    PUSB_FORWARD_PROGRESS_INFO fpInfo;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fpInfo = irpStack->Parameters.Others.Argument1;

    if (fpInfo->Header.Version != USB_FORWARD_PROGRESS_INFO_VERSION_1 ||
        fpInfo->Header.Size < sizeof(*fpInfo) ||
        fpInfo->Header.Size != SIZE_OF_USB_FORWARD_PROGRESS_INFO(fpInfo->NumberOfPipes)) {
        // Invalid request
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        goto USBPORT_IoctlNotifyForwardProgress_Exit;
    }

    // Fix up the device handle
    if (!fpInfo->DeviceHandle) {
        GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
        ASSERT_PDOEXT(rhDevExt);
        fpInfo->DeviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
    }

    // Allocate emergency resources
    ntStatus = USBPORT_AllocateForwardProgressContext(FdoDeviceObject, fpInfo);

USBPORT_IoctlNotifyForwardProgress_Exit:

    return ntStatus;
}


