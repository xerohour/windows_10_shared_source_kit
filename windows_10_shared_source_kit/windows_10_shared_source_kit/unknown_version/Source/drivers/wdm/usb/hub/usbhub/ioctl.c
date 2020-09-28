/*
Module: IOCTL.C


name:USB user mode ioctls supported by the hub driver

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002, jdunn

Abstract:

    This code implements the 'user mode api' for usbhub.  This is an IOCTL interface
    primarily used by the *usbui* (user interface) components in Windows to popup error
    messages and show bandwidth.  The IOCTls themselves are defined in usbioctl.h which
    ships in the DDK. This module also contains internal (kernel) ioctls supported by the 
    hub FDO.


USB IOCTLs:

    + IOCTL_USB_GET_NODE_INFORMATION
    + IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME
    + IOCTL_USB_GET_NODE_CONNECTION_INFORMATION
    + IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX
    + IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES
    + IOCTL_USB_GET_NODE_CONNECTION_NAME
    + IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION
    + IOCTL_USB_GET_HUB_CAPABILITIES
    + IOCTL_USB_HUB_CYCLE_PORT

Other IOCTLS:
    + IOCTL_KS_PROPERTY

USB Internal(HUB FDO) IOCTLs:
    + IOCTL_INTERNAL_USB_REQ_GLOBAL_SUSPEND (obsolete)
    + IOCTL_INTERNAL_USB_REQ_GLOBAL_RESUME (obsolete)
    
IRQL:
    All user mode IOCTLs must be called at IRQL passive.  If we are not at passive level
    the API is failed with STATUS_INVALID_PARAMETER.

Power State:
    All User mode IOCTLs are only supported when the hub is in D0.  (NOTE: The hub driver
    only exists D0 in response to system suspend request).

*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "ioctl.tmh"
#endif

CCHAR usbfile_ioctl_c[] = "ioctl.c";

#define USBFILE usbfile_ioctl_c


NTSTATUS
UsbhAcquireApiLock(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG Tag,
    __inout PBOOLEAN ApiLockAcquired
    )
 /*++

Routine Description:

    This function serializes and synchronizes USER mode API calls.  This
    functions takes the 'API' semaphore -- which can be held by only one thread
    at a time -- while it executes the API.
    The PnP bus engine uses the same semaphore to synchronize with this API
    code so that PDOs or hub extension data is not trashed while the API
    executes.

    This routine always acquires the API lock in the API_CONTEXT.

Arguments:

    Tag - Tag value asociated with this lock instance

Irql: PASSIVE

Return Value:

    if the hub is in a state where it can handle APIs then STATUS_SUCCESS otherwise
    NT_DEVICE_DISCONNECTED_STATUS.

--*/

{
    HUB_FDO_PNPSTATE pnpState;
    NTSTATUS nts;

    UsbhAssertPassive();
    LOG(HubFdo, LOG_IOCTL, 'AP1+', 0, Tag);

    WAIT_EVENT(&FdoExt(HubFdo)->IoctlLock);
    pnpState = UsbhAcquireFdoPnpLock(HubFdo, API_CONTEXT(HubFdo), Ev_API, Tag, 1);

    *ApiLockAcquired = TRUE;
    // are we in the normal run state?
    switch (pnpState) {
    case FDO_WaitPnpStop:
        //yes
        nts = STATUS_SUCCESS;
        break;
    case FDO_WaitPnpStop_CB:
        // waiting for CB
        LOG(HubFdo, LOG_IOCTL, 'AP1c', 0, Tag);

    default:
        nts = NT_DEVICE_DISCONNECTED_STATUS;
    }


    LOG(HubFdo, LOG_IOCTL, 'AP1!', nts, pnpState);

    return nts;
}


BOOLEAN
UsbhVerifyCallerIsAdmin(
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

    SeCaptureSubjectContext(&subjectContext);
    SeLockSubjectContext(&subjectContext);
    
    token = SeQuerySubjectContextToken(&subjectContext);

    NT_ASSERT(token != NULL);
    
    isAdmin = SeTokenIsAdmin(token);
    
    SeUnlockSubjectContext(&subjectContext);
    SeReleaseSubjectContext(&subjectContext);

    return isAdmin;
}

VOID
UsbhReleaseApiLock(
    PDEVICE_OBJECT HubFdo,
    ULONG Tag
    )
/*++

Routine Description:

    Releases the API semaphore.

Arguments:

    Tag - tag value associated with the instance we are releaseing.

Irql: PASSIVE

Return Value:

    none

--*/

{

    UsbhAssertPassive();
    LOG(HubFdo, LOG_IOCTL, 'AP1-', 0, Tag);
    UsbhReleaseFdoPnpLock(HubFdo, API_CONTEXT(HubFdo));
    SET_EVENT(&FdoExt(HubFdo)->IoctlLock);
}

VOID
UsbhIoctlTraceOutput(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    NTSTATUS NtStatus
    )
/*++

Routine Description:

    Display information about IOCTL Irps, usually called as the irp completes.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/

{
    PUCHAR buffer;
    USHORT dumpLen;
    ULONG inLen, outLen;
    PIO_STACK_LOCATION ioStack;
    ULONG ioCode, returnBytes;

    LOG(HubFdo, LOG_IOCTL, 'iocT', 0, Irp);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);

    ioCode = ioStack->Parameters.DeviceIoControl.IoControlCode;
    buffer = (PUCHAR) Irp->AssociatedIrp.SystemBuffer;
    outLen = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
    inLen = ioStack->Parameters.DeviceIoControl.InputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'iot1', outLen, inLen);
    LOG(HubFdo, LOG_IOCTL, 'iot2', buffer, ioCode);
    LOG(HubFdo, LOG_IOCTL, 'iot3', Irp->IoStatus.Status, Irp->IoStatus.Information);

    //prefast suggests some bound checking here
    returnBytes = (ULONG) Irp->IoStatus.Information;

    DbgTrace((UsbhIoctlTrace,"*Tracing USBIOCTL [0x%x]\n", ioCode));
    DbgTrace((UsbhIoctlTrace,"*InLen %d OutLen %d\n", inLen, outLen));
    DbgTrace((UsbhIoctlTrace,"*NTSTATUS = %!STATUS! ret bytes %d\n",
        Irp->IoStatus.Status, returnBytes));

    // only dump first 255 bytes
    dumpLen = (USHORT) (returnBytes & 0x000000ff);

    if (dumpLen == 0) {
        DbgTrace((UsbhIoctlTrace,"*buffer <no data returned>\n"));
    } else {
        DbgTrace((UsbhIoctlTrace,"*buffer:\n%!HEXDUMP!\n", log_xstr(dumpLen, buffer)));
    }


}

/*
Routine: IOCTL_USB_HUB_CYCLE_PORT

Description:

For security reasons this API is no longer supported from user mode.

*/

NTSTATUS
UsbhIoctlCyclePort(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

    For security reasons this API must be enabled with a registry key


Arguments:

Return Value:

    status of the operation

--*/
{
    NTSTATUS nts = STATUS_NOT_SUPPORTED;
    IOCTL_FAIL_DATA ioctlFailData;
    PUSB_CYCLE_PORT_PARAMS cyclePortParams;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    PDEVICE_OBJECT pdo = NULL;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    BOOLEAN updateRequired = FALSE;

    LOG(HubFdo, LOG_IOCTL, 'io>1', Irp, 0);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));


    ioctlFailData.UsbIoctl = USB_CYCLE_PORT;

    cyclePortParams = (PUSB_CYCLE_PORT_PARAMS) Irp->AssociatedIrp.SystemBuffer;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, cyclePortParams);

    do {

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }   

        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0002, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }
       
        // we need at least need a setup packet
        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_CYCLE_PORT,
                                          Irp,
                                          IoStack,
                                          sizeof(ULONG),
                                          sizeof(struct _USB_CYCLE_PORT_PARAMS));

        if (NT_ERROR(nts)) {
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));


        
        pdo = UsbhLatchPdo(HubFdo, (USHORT) cyclePortParams->ConnectionIndex, Irp, 'Io04');
        
        if (pdo) {
            BOOLEAN callerIsAdmin;
            
            callerIsAdmin = UsbhVerifyCallerIsAdmin();
            
            //
            // We need to refresh our UDX/Virtual-Machine settings by querying the 
            // registry again.
            //

            UsbhUpdateUxdSettings(HubFdo,
                                  pdo,
                                  TRUE,
                                  &updateRequired);

            if(HubG.GlobalEnableDiagnosticMode ||
               updateRequired ||
               callerIsAdmin) {

                nts = UsbhCycleDevicePort(HubFdo,
                                          pdo,
                                          &usbdStatus);
            }
            else{
                nts = STATUS_NOT_SUPPORTED;
            }

            UsbhUnlatchPdo(HubFdo, pdo, Irp, 'Io04');

        } else {
            LOG(HubFdo, LOG_IOCTL, 'gXcp', cyclePortParams->ConnectionIndex, cyclePortParams);
            nts = STATUS_DEVICE_NOT_CONNECTED;
            usbdStatus = USBD_STATUS_DEVICE_GONE;
        }

        Irp->IoStatus.Information = sizeof(struct _USB_CYCLE_PORT_PARAMS);
        cyclePortParams->StatusReturned = usbdStatus;

    } WHILE (0);



    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0002);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
   
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlResetStuckHub(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

    This routine will reset a hub that has stopped functioning due to hardware errors.

Arguments:

Return Value:

    status of the operation

--*/
{
    NTSTATUS nts = STATUS_NOT_SUPPORTED;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    BOOLEAN isUserAdmin = FALSE;

    LOG(HubFdo, LOG_IOCTL, 'io>1', Irp, 0);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));


    ioctlFailData.UsbIoctl = USB_RESET_HUB;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, 0);

#ifdef BC_ON_USER_RESET
    UsbhTrapFatal(HubFdo,HubFdo);
#endif

    do {

        isUserAdmin = UsbhVerifyCallerIsAdmin();

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0275, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_User_Reset, nts, 0, &isUserAdmin, sizeof(BOOLEAN));

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        Irp->IoStatus.Information = 0;

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0275);
    }
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetHubCaps(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++

Routine Description:
Returns the capabilities for a particular USB hub.  The capabilities are a
bit field of flags. This information is used by USBUI.
This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Parameters:
Input:
Irp->AssociatedIrp.SystemBuffer - pointer to
Struct  _ USB_HUB_CAPABILITIES{
USB_HUB_CAP_FLAGS CapabilityFlags;
 }

CapabilityFlags.HubIsHighSpeedCapable
-Set if the hub is high speed capable, this does not necessarily mean the --
hub is operating at high speed.
-NOTE: I'm not exactly sure how USBUI uses this information.
CapabilityFlags.HubIsHighSpeedCapable
CapabilityFlags.HubIsMultiTT
-indicates the hub has multiple transaction translators.

Returns
    NT status code.

--*/
{
    NTSTATUS nts;
    PUSB_HUB_CAPABILITIES hubCaps;
    PDEVICE_EXTENSION_HUB hubExt;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
     
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC! (IOCTL_USB_GET_HUB_CAPABILITIES) \n"));

    ioctlFailData.UsbIoctl = USB_GET_HUB_CAPABILITIES;

    hubExt = FdoExt(HubFdo);

    hubCaps = (PUSB_HUB_CAPABILITIES) Irp->AssociatedIrp.SystemBuffer;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, hubCaps);

    do {

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }   
        
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0002, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        // we need at least need a setup packet
        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_HUB_CAPABILITIES,
                                      Irp,
                                      IoStack,
                                      0,
                                      sizeof(struct _USB_HUB_CAPABILITIES));

        if (NT_ERROR(nts)) {
            break;
        }

        // indicate if the hub is high speed capable
        UsbhAssert(HubFdo, NT_SUCCESS(nts));
        if (hubExt->FdoFlags.HighSpeedCapable) {
            hubCaps->HubIs2xCapable = 1;
        } else {
            hubCaps->HubIs2xCapable = 0;
        }

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0002);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
   
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetHubCapsEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++

Routine Description:
Returns the capabilities for a particular USB hub.  The capabilities are a
bit field of flags. This information is used by USBUI.
This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Parameters:
Input:
Irp->AssociatedIrp.SystemBuffer - pointer to
Struct  _ USB_HUB_CAPABILITIES{
USB_HUB_CAP_FLAGS CapabilityFlags;
 }

CapabilityFlags.HubIsHighSpeedCapable
-Set if the hub is high speed capable, this does not necessarily mean the --
hub is operating at high speed.
-NOTE: I'm not exactly sure how USBUI uses this information.
CapabilityFlags.HubIsHighSpeedCapable
CapabilityFlags.HubIsMultiTT
-indicates the hub has multiple transaction translators.

Returns
    NT status code.

--*/
{
    NTSTATUS nts;
    PUSB_HUB_CAPABILITIES_EX hubCaps;
    PDEVICE_EXTENSION_HUB hubExt;
    IOCTL_FAIL_DATA ioctlFailData;
    BOOLEAN apiLockAcquired = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_HUB_CAPABILITIES_EX;

    hubExt = FdoExt(HubFdo);

    hubCaps = (PUSB_HUB_CAPABILITIES_EX) Irp->AssociatedIrp.SystemBuffer;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, hubCaps);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    do {
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0012, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        // we need at least need a setup packet
        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_GET_HUB_CAPABILITIES,
                                          Irp,
                                          IoStack,
                                          0,
                                          sizeof(struct _USB_HUB_CAPABILITIES_EX));

        if (NT_ERROR(nts)) {
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));
        // indicate if the hub is high speed capable

        hubCaps->CapabilityFlags.ul = 0;

        if (hubExt->FdoFlags.HighSpeedCapable) {
            hubCaps->CapabilityFlags.HubIsHighSpeedCapable = 1;
        }

        if (hubExt->FdoFlags.MultiTTmode) {
            hubCaps->CapabilityFlags.HubIsMultiTt = 1;
        }

        if (hubExt->FdoFlags.MultiTTsupported) {
            hubCaps->CapabilityFlags.HubIsMultiTtCapable = 1;
        }

        if ((hubExt->FdoFlags.HighSpeedCapable &&
             hubExt->FdoFlags.HubIsRoot) ||
            hubExt->FdoFlags.HubIsTT){
            
            hubCaps->CapabilityFlags.HubIsHighSpeed = 1;
        }

        if(hubExt->FdoFlags.HWC_ARM){
            hubCaps->CapabilityFlags.HubIsArmedWakeOnConnect = 1;
        }

        if(hubExt->FdoFlags.HubIsRoot){
            hubCaps->CapabilityFlags.HubIsRoot = 1;
        }

        Irp->IoStatus.Information = sizeof(struct _USB_HUB_CAPABILITIES_EX);

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0012);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetHubInformationEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

    Returns the information about the hub.

Parameters:

    Input:
        
        Pointer to USB_HUB_INFORMATION_EX structure.

    Output: 

        Pointer to USB_HUB_INFORMATION_EX structure:

--*/
{
    NTSTATUS nts;
    PUSB_HUB_INFORMATION_EX hubInformationEx;
    ULONG outBuffLength;
    PDEVICE_EXTENSION_HUB hubExt;
    IOCTL_FAIL_DATA ioctlFailData;
    BOOLEAN apiLockAcquired = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_HUB_INFORMATION_EX;

    hubExt = FdoExt(HubFdo);

    hubInformationEx = (PUSB_HUB_INFORMATION_EX) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, hubInformationEx);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    do {
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0013, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_GET_HUB_INFORMATION_EX,
                                          Irp,
                                          IoStack,
                                          0,
                                          sizeof(struct _USB_HUB_INFORMATION_EX));

        if (NT_ERROR(nts)) {
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        RtlZeroMemory(hubInformationEx, outBuffLength);

        hubInformationEx->HighestPortNumber = N_PORTS(HubFdo); 

        if (hubExt->FdoFlags.HubIsRoot) {
            hubInformationEx->HubType = UsbRootHub;
        } else {
            hubInformationEx->HubType = Usb20Hub;

            RtlCopyMemory(&hubInformationEx->u.UsbHubDescriptor,
                          &hubExt->HubDescriptor,
                          sizeof(struct _USB_HUB_DESCRIPTOR));
        }

        Irp->IoStatus.Information = sizeof(struct _USB_HUB_INFORMATION_EX);

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0013);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetPortConnectorProperties(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

    Returns information about a port and associated connector.

Parameters:

    Input:
        
        Pointer to USB_PORT_CONNECTOR_PROPERTIES structure.

    Output: 

        Pointer to USB_PORT_CONNECTOR_PROPERTIES structure:

--*/
{
    NTSTATUS nts;
    PUSB_PORT_CONNECTOR_PROPERTIES portConnectorProperties;
    ULONG outBuffLength;
    USHORT portNumber;
    PHUB_PORT_DATA pd;
    PDEVICE_EXTENSION_HUB hubExt;
    IOCTL_FAIL_DATA ioctlFailData;
    BOOLEAN apiLockAcquired = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_PORT_CONNECTOR_PROPERTIES;

    hubExt = FdoExt(HubFdo);

    portConnectorProperties = (PUSB_PORT_CONNECTOR_PROPERTIES) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>2', Irp, portConnectorProperties);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    do {
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0014, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_GET_PORT_CONNECTOR_PROPERTIES,
                                          Irp,
                                          IoStack,
                                          sizeof(ULONG),
                                          sizeof(struct _USB_PORT_CONNECTOR_PROPERTIES));

        if (NT_ERROR(nts)) {
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        portNumber = (USHORT) portConnectorProperties->ConnectionIndex;

        RtlZeroMemory(portConnectorProperties, outBuffLength);

        //
        // restore the original connection index
        //
        portConnectorProperties->ConnectionIndex = portNumber;

        //
        // Set default for actual length
        //
        portConnectorProperties->ActualLength = sizeof(struct _USB_PORT_CONNECTOR_PROPERTIES);

        pd = UsbhGetPortData(HubFdo, portNumber);

        if (pd) {

            if (UsbhBusIf_GetDebugPortNumber(HubFdo) == portNumber) {
                portConnectorProperties->UsbPortProperties.PortIsDebugCapable = 1;
            }
        
            if (!hubExt->ExtendedPortAttributes[portNumber].DeviceNotRemovable) {
                portConnectorProperties->UsbPortProperties.PortIsUserConnectable = 1;
            }
        }
        
        Irp->IoStatus.Information = sizeof(struct _USB_PORT_CONNECTOR_PROPERTIES);

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0014);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetNodeConnectionInfoExV2(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

    Returns information about a port.

Parameters:

    Input:
        
        Pointer to USB_NODE_CONNECTION_INFORMATION_EX_V2 structure.

    Output: 

        Pointer to USB_NODE_CONNECTION_INFORMATION_EX_V2 structure:

--*/
{
    NTSTATUS nts;
    PUSB_NODE_CONNECTION_INFORMATION_EX_V2 nodeConnectionInfoExV2;
    USB_PROTOCOLS protocolsSupportedByClient;
    ULONG outBuffLength;
    USHORT portNumber;
    PHUB_PORT_DATA pd;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    IOCTL_FAIL_DATA ioctlFailData;
    BOOLEAN apiLockAcquired = FALSE;

    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_INFORMATION_EX_V2;

    hubExt = FdoExt(HubFdo);

    nodeConnectionInfoExV2 = (PUSB_NODE_CONNECTION_INFORMATION_EX_V2) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>c', Irp, nodeConnectionInfoExV2);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    do {
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0015, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_GET_NODE_CONNECTION_INFORMATION_EX_V2,
                                          Irp,
                                          IoStack,
                                          sizeof(struct _USB_NODE_CONNECTION_INFORMATION_EX_V2),
                                          sizeof(struct _USB_NODE_CONNECTION_INFORMATION_EX_V2));

        if (NT_ERROR(nts)) {
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        if (nodeConnectionInfoExV2->Length < sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2)) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        portNumber = (USHORT) nodeConnectionInfoExV2->ConnectionIndex;
        protocolsSupportedByClient = nodeConnectionInfoExV2->SupportedUsbProtocols;

        RtlZeroMemory(nodeConnectionInfoExV2, outBuffLength);

        //
        // restore the original connection index
        //
        nodeConnectionInfoExV2->ConnectionIndex = portNumber;

        //
        // For initial version of this API the client must support the USB 3.0 protocol
        //
        if (protocolsSupportedByClient.Usb300 != 1) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }
        
        //
        // restore the original connection index
        //
        nodeConnectionInfoExV2->ConnectionIndex = portNumber;
        
        pd = UsbhGetPortData(HubFdo, portNumber);

        if (!pd) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        //
        // Set the length supported by this version of the hub driver
        //
        nodeConnectionInfoExV2->Length = sizeof(struct _USB_NODE_CONNECTION_INFORMATION_EX_V2);
        Irp->IoStatus.Information = sizeof(struct _USB_NODE_CONNECTION_INFORMATION_EX_V2);

        nodeConnectionInfoExV2->SupportedUsbProtocols.Usb110 = 1;

        if (hubExt->FdoFlags.HighSpeedCapable) {
            nodeConnectionInfoExV2->SupportedUsbProtocols.Usb200 = 1;
        }

        pdo = UsbhLatchPdo(HubFdo, portNumber, Irp, 'Io0c');

        if (!pdo) {
            break;
        }

        pdoExt = PdoExt(pdo);
        
        if (pdoExt->PdoFlags.DeviceIsSuperSpeedCapable) {
            nodeConnectionInfoExV2->Flags.DeviceIsSuperSpeedCapableOrHigher = 1;
        }

        UsbhUnlatchPdo(HubFdo, pdo, Irp, 'Io0c'); 

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<c', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0015);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetDescriptorForPDO(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUSHORT DataBufferLength,
    PUCHAR DataBuffer,
    BOOLEAN ApiLockAcquired,
    BOOLEAN UseCachedDescriptor
    )
/*++
Routine Description:

    Helper function,
    allows application to query a descriptor from a device.  This function
    performs validation of the setup packet passed in and sends the request out
    to the device.

    If the device handle in the PDO is invalid then the descriptors are returned
    from cached info in the extension -- this will only occur if the devices
    has been 'removed'

Arguments:

    Input:
    DataBuffer and DataBufferLength are the buffer to receive any descriptor
    data for the request.

    Output:
    DataBufferLength is set to the length of data copied by the transaction

Return Value:

    status of transaction or invalid parameter.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    USBD_STATUS usbdStatus;
    NTSTATUS nts = STATUS_BOGUS;
    PUSB_DEVICE_HANDLE dh = NULL;
    //
    // do some parameter checking
    //
    LOG(HubFdo, LOG_IOCTL, 'GDP1', Pdo, 0);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    // the wLength in the setup packet better be the size of the buffer or we
    // will trash memory
    //
    do {
        if (SetupPacket->wLength > *DataBufferLength) {
            LOG(HubFdo, LOG_IOCTL, 'GDP2', Pdo, 0);
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        // we only allow get_descriptor
        if (SetupPacket->bRequest != USB_REQUEST_GET_DESCRIPTOR) {
            LOG(HubFdo, LOG_IOCTL, 'GDP3', Pdo, SetupPacket->bRequest);
            nts =  STATUS_INVALID_PARAMETER;
            break;
        }

        // sanity check direction,
        // we let the device validate the rest
        if (SetupPacket->bmRequestType.Dir !=  BMREQUEST_DEVICE_TO_HOST) {
            LOG(HubFdo, LOG_IOCTL, 'GDP4', Pdo, 0);
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        pdoExt = PdoExt(Pdo);

        // attempt to satisfy the request with cached data if possible in the
        // event that the device handle is invalid.
        dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Irp, Devh_PdoApi_gni_Tag);
        LOG(HubFdo, LOG_IOCTL, 'rfD1', 0, dh);

        if ((dh == NULL) || UseCachedDescriptor) {
            // usbui will call the API even for PDOs that no longer have a valid
            // device handle -- devices that fail enumeration may not have a handle.

            // device handle is invalid, use cached data from the device
            // or return an error

            // *we get here when the device fails start and USBUI wants some info
            if (SetupPacket->wValue.HiByte == USB_CONFIGURATION_DESCRIPTOR_TYPE &&
                SetupPacket->wValue.LowByte == 0) {

                PUSB_CONFIGURATION_DESCRIPTOR cfgDs =
                    pdoExt->CachedConfigDescriptor;
                USHORT lengthToCopy;

                if (pdoExt->CachedConfigDescriptor) {
                    // use the smallest relevant value
                    lengthToCopy = *DataBufferLength;
                    if (lengthToCopy > cfgDs->wTotalLength) {
                        // only fill in client buffer with descriptor
                        // ie don't return extra data.
                        lengthToCopy = cfgDs->wTotalLength;
                    }

                    // setup packet is not necessarily relevant here.
                    // we will still check it.
                    if (lengthToCopy <= SetupPacket->wLength &&
                        lengthToCopy <= *DataBufferLength &&
                        lengthToCopy != 0) {

                        RtlCopyMemory(DataBuffer,
                                      cfgDs,
                                      lengthToCopy);

                        *DataBufferLength = lengthToCopy;

                        nts =  STATUS_SUCCESS;
                    } else {
                        *DataBufferLength = 0;
                        nts = STATUS_UNSUCCESSFUL;
                    }
                    break;
                } else {
                    *DataBufferLength = 0;
                    nts = STATUS_UNSUCCESSFUL;
                    break;
                }
            }

            if (SetupPacket->wValue.HiByte == USB_DEVICE_DESCRIPTOR_TYPE &&
                SetupPacket->wValue.LowByte == 0) {
                // asking for device descriptor, we have this cached
                USHORT lengthToCopy;

                // only fill in client buffer with descriptor
                // ie don't return extra data.
                lengthToCopy = *DataBufferLength;
                if (lengthToCopy > sizeof(struct _USB_DEVICE_DESCRIPTOR)) {
                    lengthToCopy = sizeof(struct _USB_DEVICE_DESCRIPTOR);
                }

                LOG(HubFdo, LOG_IOCTL, 'GDP4', SetupPacket->wLength, *DataBufferLength);

                // setup packet is not necessarily relevant here.
                // we will still check it. don't copy if we overflow either Value
                if (lengthToCopy <= SetupPacket->wLength &&
                    lengthToCopy <= *DataBufferLength) {

                    RtlCopyMemory(DataBuffer,
                          &pdoExt->DeviceDescriptor,
                          lengthToCopy);

                    *DataBufferLength = lengthToCopy;
                    LOG(HubFdo, LOG_IOCTL, 'GDP5', DataBuffer, lengthToCopy);
                    nts =  STATUS_SUCCESS;

                } else {

                    LOG(HubFdo, LOG_IOCTL, 'GDP6', DataBuffer, lengthToCopy);
                    *DataBufferLength = 0;
                    nts = STATUS_UNSUCCESSFUL;
                }
                break;
            }

            if (SetupPacket->wValue.HiByte == USB_BOS_DESCRIPTOR_TYPE &&
                SetupPacket->wValue.LowByte == 0) {

                // asking for BOS descriptor, we may have this cached
                PUSB_BOS_DESCRIPTOR bosDescriptor;
                USHORT lengthToCopy;

                bosDescriptor = pdoExt->BosDescriptor;

                if (bosDescriptor) {
                    // use the smallest relevant value
                    lengthToCopy = *DataBufferLength;
                    if (lengthToCopy > bosDescriptor->wTotalLength) {
                        // only fill in client buffer with descriptor
                        // ie don't return extra data.
                        lengthToCopy = bosDescriptor->wTotalLength;
                    }

                    // setup packet is not necessarily relevant here.
                    // we will still check it.
                    if (lengthToCopy <= SetupPacket->wLength &&
                        lengthToCopy <= *DataBufferLength &&
                        lengthToCopy != 0) {

                        RtlCopyMemory(DataBuffer,
                                      bosDescriptor,
                                      lengthToCopy);

                        *DataBufferLength = lengthToCopy;

                        nts =  STATUS_SUCCESS;
                    } else {
                        *DataBufferLength = 0;
                        nts = STATUS_UNSUCCESSFUL;
                    }
                    break;
                } else {
                    *DataBufferLength = 0;
                    nts = STATUS_UNSUCCESSFUL;
                    break;
                }
            }
    
            if ((SetupPacket->wValue.HiByte == USB_STRING_DESCRIPTOR_TYPE) &&
                (SetupPacket->wValue.LowByte != 0) && 
                (SetupPacket->wValue.LowByte == pdoExt->DeviceDescriptor.iSerialNumber) &&
                (pdoExt->SerialNumberId.Buffer != NULL) && 
                (pdoExt->SerialNumberId.LengthInBytes != 0) &&
                (SetupPacket->wIndex.W == 0x409)) {

                // asking for serial number string descriptor. We will need to re-construct it from the 
                // USB_ID_STRING

                USHORT lengthToCopy;
                USHORT stringDescriptorLength;

                stringDescriptorLength =
                    (USHORT)(FIELD_OFFSET(USB_STRING_DESCRIPTOR, bString) +  // beginning offset of the string field
                             pdoExt->SerialNumberId.LengthInBytes -          // plus length of the NULL-terminated string
                             sizeof(WCHAR));                                 // minus the terminating NULL

                // only fill in client buffer with descriptor
                // ie don't return extra data.
                lengthToCopy = (UCHAR)*DataBufferLength;
                if (lengthToCopy > stringDescriptorLength) {
                    lengthToCopy = stringDescriptorLength;
                }

                LOG(HubFdo, LOG_IOCTL, 'GDP7', SetupPacket->wLength, *DataBufferLength);

                // setup packet is not necessarily relevant here.
                // we will still check it. don't copy if we overflow either Value
                if (lengthToCopy <= SetupPacket->wLength &&
                    lengthToCopy <= *DataBufferLength) {

                    PUSB_STRING_DESCRIPTOR SerialNumberDesc;

                    SerialNumberDesc = (PUSB_STRING_DESCRIPTOR)DataBuffer;
                    SerialNumberDesc->bLength = LOBYTE(lengthToCopy);
                    SerialNumberDesc->bDescriptorType = USB_STRING_DESCRIPTOR_TYPE;
                    RtlCopyMemory(SerialNumberDesc->bString,
                                  pdoExt->SerialNumberId.Buffer,
                                  pdoExt->SerialNumberId.LengthInBytes - sizeof(WCHAR));

                    *DataBufferLength = lengthToCopy;
                    LOG(HubFdo, LOG_IOCTL, 'GDP8', DataBuffer, lengthToCopy);
                    nts = STATUS_SUCCESS;

                } else {
                    LOG(HubFdo, LOG_IOCTL, 'GDP9', DataBuffer, lengthToCopy);
                    *DataBufferLength = 0;
                    nts = STATUS_UNSUCCESSFUL;
                }
                break;
            }

            nts =  STATUS_UNSUCCESSFUL;
            break;

        } else {
            
            // Release the PnP lock for the descriptor request
            if (ApiLockAcquired) {
                UsbhReleaseFdoPnpLock(HubFdo, API_CONTEXT(HubFdo));
            }

            // send the request on to the device for processing
            nts = UsbhSyncSendCommandToDevice(HubFdo,
                                              Pdo,
                                              SetupPacket,
                                              DataBuffer,
                                              DataBufferLength,
                                              USBH_CONTROL_PIPE_TIMEOUT,
                                              &usbdStatus);

            if (dh) {
                UsbhDerefPdoDeviceHandle(HubFdo, dh, Irp, Devh_PdoApi_gni_Tag);
                dh = NULL;
            }

            // Re-acquire the PnP lock
            if (ApiLockAcquired) {
                UsbhAcquireFdoPnpLock(HubFdo, API_CONTEXT(HubFdo), Ev_API, 0xf00d000b, 0);
            }

            LOG(HubFdo, LOG_IOCTL, 'GDP4', nts, usbdStatus);
            break;
        }

    } WHILE (0);

    if (dh) {
        UsbhDerefPdoDeviceHandle(HubFdo, dh, Irp, Devh_PdoApi_gni_Tag);
    }

    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    ASSERT_NTS(HubFdo, nts);
    return nts;
}

/*
Routine: IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION

Description:

This API can be used to query for any standard USB descriptor from user mode.  This IOCTL will
build a setup request for the control pipe with a bRequest=6 (GET_DESCRIPTOR),
bmRequestType.Recipient=0(device), bmRequestType.Type=0(standard request) and
bmRequestType.Direction=1(device-to-Host). Although a complete setup packet is passed as a
parameter only the wValue, wIndex and wLength will be used.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_DESCRIPTOR_REQUEST structure

Input:
ConnectionIndex - this is the 1 based port number in the case of a hub.

Output:
Data - will be filled in with the descriptor data returned from the device.

<pre>
typedef struct _USB_DESCRIPTOR_REQUEST {
    ULONG ConnectionIndex;
    struct {
        UCHAR bmRequest;
        UCHAR bRequest;
        USHORT wValue;
        USHORT wIndex;
        USHORT wLength;
    } SetupPacket;
    UCHAR Data[0];
} USB_DESCRIPTOR_REQUEST, *PUSB_DESCRIPTOR_REQUEST;
</pre>
*/

NTSTATUS
UsbhIoctlGetDescriptorFromNodeConnection(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    NTSTATUS nts;
    PUSB_DESCRIPTOR_REQUEST dReq;
    USHORT outBuffLength;
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    PUCHAR dData;
    PDEVICE_OBJECT pdo;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    BOOLEAN useCachedDescriptor = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC! (IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION)\n"));

    ioctlFailData.UsbIoctl = USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION;

    dReq = (PUSB_DESCRIPTOR_REQUEST) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength =
        (USHORT) IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>b', Irp, dReq);

    do {
        
        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }      
        
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d000b, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        // we need at least need a setup packet
        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
                                      Irp,
                                      IoStack,
                                      sizeof(ULONG),
                                      sizeof(struct _USB_DESCRIPTOR_REQUEST)+1);

        if (NT_ERROR(nts)) {
            // No device connected, return an error
            LOG(HubFdo, LOG_IOCTL, 'i2<b', nts, 0);
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            break;
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));
        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET) &dReq->SetupPacket;
        // BUGBUG
        // the API implies a get descriptor request so we set the correct
        // value here for the caller.
        // There is a bug in USBUI -- it doesn't set the correct values here.
        // if we want to be really good about parameter validation we should
        // fix USBUI. Also we must hard code for standard requests which
        // prevents the API from ever being used for accessing vendor or class
        // requests.
        setupPacket->bRequest = USB_REQUEST_GET_DESCRIPTOR;
        setupPacket->bmRequestType.Recipient = 0;   // device
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->bmRequestType.Type = 0;        // standard
        setupPacket->bmRequestType.Dir = 1;         // device to host

        dData = &dReq->Data[0];

        pdo = UsbhLatchPdo(HubFdo, (USHORT) dReq->ConnectionIndex, Irp, 'Io0b');
        if (pdo) {
            PSTATE_CONTEXT sc;
            HUB_PDO_POWER_STATE pdoPowerState;

            sc = PNP_CONTEXT(HubFdo);

            UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_POWER, PdoUserDescReq_Tag);
            pdoPowerState = GET_PDO_POWER_STATE(pdo);  
            UsbhReleaseFdoPwrLock(HubFdo, sc);  

            if (pdoPowerState != Pdo_D0) {  
                useCachedDescriptor = TRUE;   
            }  
                        
            // attempt the request
            outBuffLength = outBuffLength - sizeof(struct _USB_DESCRIPTOR_REQUEST);

            nts = UsbhIoctlGetDescriptorForPDO(HubFdo,
                                               pdo,
                                               Irp,
                                               setupPacket,
                                               &outBuffLength,
                                               dData,
                                               apiLockAcquired,
                                               useCachedDescriptor);

            LOG(HubFdo, LOG_IOCTL, 'i1<b', nts, outBuffLength);
            if (NT_SUCCESS(nts)) {
                Irp->IoStatus.Information = outBuffLength +
                    sizeof(struct _USB_DESCRIPTOR_REQUEST);
            }
            UsbhUnlatchPdo(HubFdo, pdo, Irp, 'Io0b');
        } //else {
        // jd?
        // sometimes failing these APIs messes with the ui, check this
            //nts = NT_DEVICE_DISCONNECTED_STATUS;
        //}
    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<b', Irp, nts);

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d000b);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
    
    UsbhCompleteIrp(Irp, nts);

    return nts;
}

/*
Routine: IOCTL_USB_GET_NODE_CONNECTION_NAME

Description:

Returns the symbolic link name associated with the connection in the buffer passed in.
This API only returns a name if the connection is for another USB hub. If the
connection if for a device or the connection is empty a NULL NodeName is returned.
This API always succeeds as long as the inputbuffer is >=
sizeof(struct _USB_NODE_CONNECTION_NAME) even if the entire name will not fit.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_NODE_CONNECTION_NAME structure

Input:
ConnectionIndex - this is the 1 based port number in the case of a hub.

Output:
ActualLength - actual length of this entire structure including name.
NodeName - unicode symbolic name for this node, including NULL.

<pre>

typedef struct _USB_NODE_CONNECTION_NAME {
    ULONG ConnectionIndex;
    ULONG ActualLength;
    WCHAR NodeName[1];
} USB_NODE_CONNECTION_NAME, *PUSB_NODE_CONNECTION_NAME;

</pre>
*/

NTSTATUS
UsbhIoctlGetNodeConnectionName(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:

Arguments:

Return Value:

--*/
{
    NTSTATUS nts;
    PUSB_NODE_CONNECTION_NAME ncn;
    ULONG outBuffLength;
    PDEVICE_OBJECT pdo;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    
    DbgTrace((UsbhIoctlTrace,">%!FUNC! irp %p (IOCTL_USB_GET_NODE_CONNECTION_NAME)\n", Irp));
    UsbhAssertPassive();

    ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_NAME;

    ncn = (PUSB_NODE_CONNECTION_NAME) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>3', Irp, ncn);

    do {

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }      
        
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0003, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }

        // we need at least enough to copy a NULL string
        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_NODE_CONNECTION_NAME,
                                      Irp,
                                      IoStack,
                                      sizeof(ULONG),
                                      // min length is size of empty buffer
                                      sizeof(struct _USB_NODE_CONNECTION_NAME));

        if (NT_ERROR(nts)) {
//usbio test #1002
            break;
        }

        pdo = UsbhLatchPdo(HubFdo, (USHORT) ncn->ConnectionIndex, Irp, PdoLatchIo03_Tag);
        if (pdo) {
            ULONG offset, nBytesToReturn;

            RtlZeroMemory(ncn, outBuffLength);

            // attempt to get the name from the PDO

            // map the IOCTL parameters on to the UsbhGetHubPdoName function
            // subtract the size of the offset to the field we pass in
            offset = FIELD_OFFSET(USB_NODE_CONNECTION_NAME, ActualLength);

            // we think we already validated this
            UsbhAssert(HubFdo, outBuffLength >= offset);
            outBuffLength -= offset;

//usbio test #1003
//usbio test #1004

            //
            nts = UsbhGetHubPdoName(HubFdo,
                                    pdo,
                                    (PUSB_HUB_NAME) &ncn->ActualLength,
                                    outBuffLength,
                        // Irp Information defined as type ULONG_PTR
                        // because its only 32 bits wide and sometimes it
                        // is used as a pointer, we cast to PULONG
                        // because in this case it is not used as a pointer.
                                    &nBytesToReturn);

            if (nBytesToReturn >= sizeof(ncn->ActualLength)) {
                ncn->ActualLength += offset;
            }
            nBytesToReturn += offset;
            Irp->IoStatus.Information =  nBytesToReturn;
            UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatchIo03_Tag);

        }else {

            // No device connected, return a NULL string
            ncn->ActualLength = sizeof(struct _USB_NODE_CONNECTION_NAME);
            ncn->NodeName[0] = UNICODE_NULL;

            Irp->IoStatus.Information = sizeof(struct _USB_NODE_CONNECTION_NAME);
//usbio test #1003
            // return success so that usbui sees the empty buffer
            nts = STATUS_SUCCESS;
        }
    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<3', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0003);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);
    
    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
    
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhIoctlGetNodeConnectionAttributes(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:
Returns the Microsoft extended port attributes for a specific port. The
caller inputs the port number as the 'ConnectionIndex'.  Microsoft extended
port attributes are defined in the Extended Port Attribute specification.
This API also returns the current connection status of the port.

This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Parameters:
Input:
Irp->AssociatedIrp.SystemBuffer - pointer to
Struct  _USB_NODE_CONNECTION_INFORMATION {
    ULONG ConnectionIndex;
}
ConnectionIndex
-is the one based port number.

Output: (if a device is attached)
ConnectionStatus
-The current USB connection status. Indicates things like enumeration failure
or overcurrent.
PortAttributes
-Extended port attributes defined in usb.h.

IRQL:
    passive

Return Value:
    ntstatus

--*/
{
    NTSTATUS nts;
    PUSB_NODE_CONNECTION_ATTRIBUTES nca;
    ULONG outBuffLength;
    PHUB_PORT_DATA pd;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_ATTRIBUTES;

    nca = (PUSB_NODE_CONNECTION_ATTRIBUTES) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>a', Irp, nca);

    do {

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }   

        nts = UsbhAcquireApiLock(HubFdo, 0xf00d000a, &apiLockAcquired);
        if (NT_ERROR(nts)) {
            break;
        }
        
        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_NODE_CONNECTION_ATTRIBUTES,
                                      Irp,
                                      IoStack,
                                      sizeof(ULONG),
                                      sizeof(struct _USB_NODE_CONNECTION_ATTRIBUTES));

        if (NT_ERROR(nts)) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        pd = UsbhGetPortData(HubFdo, (USHORT) nca->ConnectionIndex);

        RtlZeroMemory(nca, outBuffLength);

        if (pd) {

            nca->ConnectionIndex = pd->PortNumber;
            nca->ConnectionStatus = pd->ConnectionStatus;
            // BUGBUG -- waiting on spec completion
            nca->PortAttributes = 0;
            Irp->IoStatus.Information = sizeof(struct _USB_NODE_CONNECTION_ATTRIBUTES);

            nts = STATUS_SUCCESS;
         }
         // jd
         // STATUS_FAILURE here -- depends on usbui.

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<a', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d000a);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
    
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


PUSB_DEVICE_INFORMATION_0
UsbhGetDeviceInformationEx(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS *NtStatus,
    PUSB_DEVICE_HANDLE DeviceHandle
    )
/*++
Routine Description:

    helper function returns a pointer to PUSB_DEVICE_INFORMATION_0
    for a given device

Arguments:

Return Value:

    NULL or pointer to PUSB_DEVICE_INFORMATION_0

--*/
{
    NTSTATUS nts;
    ULONG length, lengthCopied;
    PUSB_DEVICE_INFORMATION_0 level_0 = NULL;

    // we need the device handle to return useful information
    UsbhAssert(HubFdo, DeviceHandle != UsbhInvalidDeviceHandle);
    UsbhAssert(HubFdo, DeviceHandle != NULL);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    // start with the size of the structure with no pipes
    length = sizeof(struct _USB_DEVICE_INFORMATION_0);

    // get device info from usbport

    do {
        UsbhAssert(HubFdo, level_0 == NULL);
        UsbhAllocatePool_Z(level_0, PagedPool, length);
        if (level_0 == NULL) {
            *NtStatus = STATUS_INSUFFICIENT_RESOURCES;
            return level_0;
        }

        UsbhAssert(HubFdo, level_0 != NULL);
        level_0->InformationLevel = 0;

        nts = Usbh_HubQueryDeviceInformation(HubFdo,
                                             DeviceHandle,
                                             level_0,
                                             length,
                                             &lengthCopied);

        if (NT_ERROR(nts)) {
            if (nts == STATUS_BUFFER_TOO_SMALL) {
                length = level_0->ActualLength;
            }
            UsbhFreePool(level_0);
        }
    } while (nts == STATUS_BUFFER_TOO_SMALL);

    LOG(HubFdo, LOG_IOCTL, 'gdix', DeviceHandle, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));

    *NtStatus = nts;
    return level_0;

}


NTSTATUS
UsbhGetNodeConnectionInfoForPdo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo,
    PUSB_NODE_CONNECTION_INFORMATION NciBuffer,
    PULONG OutBufferLength,
    BOOLEAN ExApi
    )
/*++
Routine Description:
    returns NodeConnectionIfno associated with a port data structure

Arguments:

    PortData - portdata for the port in question

    Pdo - latched pdo associated with the port

    Irp - irp associated with the original IOCTL call

--*/
{
    NTSTATUS nts = STATUS_INVALID_PARAMETER;
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_INFORMATION_0 level_0;
    PUSB_DEVICE_HANDLE dh;

    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    LOG(HubFdo, LOG_IOCTL, 'gnn1', PortData->PortNumber, NciBuffer);

    pdoExt = PdoExt(Pdo);

    dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Irp, Devh_PdoApi_gni_Tag);

    RtlZeroMemory(NciBuffer, *OutBufferLength);

    if (!dh) {
        // no device handle, just return connection status
        // with success

        LOG(HubFdo, LOG_IOCTL, 'gnnd', PortData->ConnectionStatus, NciBuffer);
        // connection status should indicate no device connected
        // or a failure status
        NciBuffer->ConnectionIndex = PortData->PortNumber;
        NciBuffer->ConnectionStatus = PortData->ConnectionStatus;

        *OutBufferLength = sizeof(struct _USB_NODE_CONNECTION_INFORMATION);

        nts = STATUS_SUCCESS;
    } else {

        level_0 = UsbhGetDeviceInformationEx(HubFdo,
                                             &nts,
                                             dh);

        LOG(HubFdo, LOG_IOCTL, 'gnL1', level_0, nts);

        // drop the ref as soon as possible
        UsbhDerefPdoDeviceHandle(HubFdo, dh, Irp, Devh_PdoApi_gni_Tag);

        if (NT_SUCCESS(nts)) {
            ULONG p, remain;

            // this is the port number
            NciBuffer->ConnectionIndex = PortData->PortNumber;

            NciBuffer->DeviceDescriptor = level_0->DeviceDescriptor;

            NciBuffer->CurrentConfigurationValue =
                level_0->CurrentConfigurationValue;

            // this api returns a boolean indicating low speed or not
            // it is not much use on USB 2.0, hence, the EX api.
            if (!ExApi) {
                NciBuffer->LowSpeed =
                    (level_0->DeviceSpeed == UsbLowSpeed) ? TRUE : FALSE;
            } else {
                PUSB_NODE_CONNECTION_INFORMATION_EX nciEx;

                UsbhAssert(HubFdo, sizeof(struct _USB_NODE_CONNECTION_INFORMATION) ==
                    sizeof(struct _USB_NODE_CONNECTION_INFORMATION_EX));
                nciEx = (PUSB_NODE_CONNECTION_INFORMATION_EX) NciBuffer;

                nciEx->Speed = level_0->DeviceSpeed;
            }

            if (GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpStop) {
                NciBuffer->DeviceIsHub = pdoExt->PdoFlags.DeviceIsHub ? TRUE : FALSE;
            }

            NciBuffer->DeviceAddress = level_0->DeviceAddress;

            NciBuffer->NumberOfOpenPipes = level_0->NumberOfOpenPipes;

            // reort certian errors that can occurr on a valid device handle
            switch (PortData->ConnectionStatus) {
            case DeviceNotEnoughPower:
            case DeviceNotEnoughBandwidth:
            case DeviceCausedOvercurrent:                    
            case DeviceHubNestedTooDeeply:
                           
                NciBuffer->ConnectionStatus = PortData->ConnectionStatus;
                break;
            default:
                // This seems to disble the popup for some reason
                //if (pdoExt->PdoFlags.HsDeviceRunningAtFs) {
                //    NciBuffer->ConnectionStatus = DeviceInLegacyHub;
                    //TEST_TRAP();
                //} else {                       
                    // always show as connected if we ref the device
                    NciBuffer->ConnectionStatus = DeviceConnected;
                //}                    
            }                

            // subtract off the portion we consumed with the fixed length data
            remain = *OutBufferLength - sizeof(struct _USB_NODE_CONNECTION_INFORMATION);

            for (p = 0;
                 p < NciBuffer->NumberOfOpenPipes &&
                    remain >= sizeof(struct _USB_PIPE_INFO);
                 p++) {
                NciBuffer->PipeList[p].EndpointDescriptor =
                    level_0->PipeList[p].EndpointDescriptor;
                NciBuffer->PipeList[p].ScheduleOffset =
                    level_0->PipeList[p].ScheduleOffset;
                remain -= sizeof(struct _USB_PIPE_INFO);
            }

            // BUGBUG
            // note: we do not return STATUS_BUFFER_TOO_SMALL HERE if there is
            // not enough space for all the pipe info.  unfortunately this is the
            // way the API is defined.  We could fix this but we must fix USBUI
            // as well
            if (p != NciBuffer->NumberOfOpenPipes) {
                LOG(HubFdo, LOG_IOCTL, 'gnnS', remain,
                    NciBuffer->NumberOfOpenPipes);
                // nts = STATUS_BUFFER_TOO_SMALL;
            }

            // return what we copied
            *OutBufferLength = *OutBufferLength - remain;

        }

        if (level_0) {
            UsbhFreePool(level_0);
        }

    }

    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    return nts;
}


NTSTATUS
UsbhIoctlGetNodeConnectionInfoExApi(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack,
    BOOLEAN ExApi
    )
/*++
Routine Description:
    Helper function, does the processing for
        UsbhIoctlGetNodeConnectionInfoEx
        UsbhIoctlGetNodeConnectionInfo

Arguments:
    see
        UsbhIoctlGetNodeConnectionInfoEx
        UsbhIoctlGetNodeConnectionInfo
--*/
{
    NTSTATUS nts;
    PUSB_NODE_CONNECTION_INFORMATION nci;
    ULONG outBuffLength;
    PDEVICE_OBJECT pdo;
    PHUB_PORT_DATA pd;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
    
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    if (ExApi) {
        ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_INFORMATION_EX;

    } else {
        ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_INFORMATION;
    }

    nci = (PUSB_NODE_CONNECTION_INFORMATION) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'gnX1', Irp, nci);

    do {
        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }    
        
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0004, &apiLockAcquired);

        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_NODE_CONNECTION_INFORMATION,
                                      Irp,
                                      IoStack,
                                      sizeof(ULONG),
                                      sizeof(struct _USB_NODE_CONNECTION_INFORMATION));

        if (NT_ERROR(nts)) {
            break;
        }

        pd =UsbhGetPortData(HubFdo, (USHORT) nci->ConnectionIndex);
        if (pd) {
            pdo = UsbhLatchPdo(HubFdo, (USHORT) nci->ConnectionIndex, Irp, 'Io04');
            if (pdo) {
                nts = UsbhGetNodeConnectionInfoForPdo(HubFdo,
                                                      Irp,
                                                      pd,
                                                      pdo,
                                                      nci,
                                                      &outBuffLength,
                                                      ExApi);

                Irp->IoStatus.Information = outBuffLength;

                UsbhUnlatchPdo(HubFdo, pdo, Irp, 'Io04');

            } else {
                LOG(HubFdo, LOG_IOCTL, 'gXnp', pd->PortNumber, nci);

                // no PDO, fill in nci structure as best we can
                // this particular PDO is gone so we will indicate no
                // device connected -- there may in fact be a device
                // but it is a different instance.

                RtlZeroMemory(nci, outBuffLength);

                // connection status should indicate no device connected
                nci->ConnectionIndex = pd->PortNumber;
                nci->ConnectionStatus = NoDeviceConnected;

                nts = STATUS_SUCCESS;
                Irp->IoStatus.Information = sizeof(struct _USB_NODE_CONNECTION_INFORMATION);
            }

        } else {
             nts = STATUS_INVALID_PARAMETER;
        }

    } WHILE (0);

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0004);
    }            

    LOG(HubFdo, LOG_IOCTL, 'gnX2', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
    
    UsbhCompleteIrp(Irp, nts);

    return nts;

}


NTSTATUS
UsbhIoctlGetNodeConnectionInfo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Exactly the same as _EX but the speed field is a boolean
LowSpeed
-TRUE if the device is low speed.
--*/
{
    LOG(HubFdo, LOG_IOCTL, 'io>7', Irp, 0);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    return UsbhIoctlGetNodeConnectionInfoExApi(HubFdo,
                                               Irp,
                                               IoStack,
                                               FALSE);

}

/*
Routine: IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX

Description:

Returns information about a specific USB hub port (AKA connection).  If there
is a device connected to the port information about the device is also
returned. The caller inputs the port number as the 'ConnectionIndex'.

This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_NODE_CONNECTION_INFORMATION_EX structure

Input:

ConnectionIndex - this is the 1 based port number in the case of a hub.

Output:

DeviceDescriptor - USB device descriptor.

CurrentConfigurationValue - Currently selected configuration value.

Speed - indicates the 'current' operating speed (UsbLowSpeed=0, UsbFullSpeed=1 and UsbHighSpeed=2).
Note that high speed devices can operate at full speed when necessary.

DeviceIsHub - TRUE if the attached device is a USB hub.  True is only returnrf if the PDO is in the
            started state, a device is a hub if is both a hub and has the hub driver loaded for it.


DeviceAddress - USB assigned device address.

NumberOfOpenPipes - Number of open USB pipes in the current configuration

ConnectionStatus - The current USB connection status (see USB_CONNECTION_STATUS).

USB_PIPE_INFO PipeList[] - list of open pipes including schedule offset and endpoint descriptor.
This information can be used to calculate bandwidth usage for the FULL  speed bus.

<pre>
typedef struct _USB_NODE_CONNECTION_INFORMATION_EX {
    ULONG ConnectionIndex;
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;
    UCHAR CurrentConfigurationValue;
    UCHAR Speed;
    BOOLEAN DeviceIsHub;
    USHORT DeviceAddress;
    ULONG NumberOfOpenPipes;
    USB_CONNECTION_STATUS ConnectionStatus;
    USB_PIPE_INFO PipeList[0];
} USB_NODE_CONNECTION_INFORMATION_EX, *PUSB_NODE_CONNECTION_INFORMATION_EX;

</pre>
*/


NTSTATUS
UsbhIoctlGetNodeConnectionInfoEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:
    see above

Return Value:

    ntStatus code

--*/
{
    LOG(HubFdo, LOG_IOCTL, 'io>8', Irp, 0);
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    return UsbhIoctlGetNodeConnectionInfoExApi(HubFdo,
                                               Irp,
                                               IoStack,
                                               TRUE);

}


NTSTATUS
UsbhIoctlValidateParameters(
    PDEVICE_OBJECT HubFdo,
    ULONG UsbIoCode,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack,
    ULONG MinInputSize,
    ULONG MinOutputSize
    )
/*++
Routine Description:

Helper routine for:
    UsbhIoctlGetNodeConnectionInfo
    UsbhIoctlGetNodeConnectionDriverKeyName
    UsbhIoctlCyclePort

    validates buffer length and connection index input parameter

    also verifies that the bus is in the 'started' state

    If MinInputSize > 0 then it is assumed to point to a connectionIndex value
    in the first dword

Arguments:

    UsbIoCode - iocode we are validating

    Irp - irp associated with this request

    IoStack - current stack location, has the size of in/out buffers

    MinInputSize - minimum size of input data, usually a connection index

    MinOutputSize - min we need to return useful information.

--*/
{
    PULONG connectionIdx;
    ULONG outBuffLength, inBuffLength;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    API_FAIL_DATA apiFailData;

    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    hubExt = FdoExt(HubFdo);

    connectionIdx = NULL;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;
    inBuffLength = IoStack->Parameters.DeviceIoControl.InputBufferLength;

    apiFailData.UsbIoCode = UsbIoCode;
    apiFailData.OutBuffLength = outBuffLength;
    apiFailData.InBuffLength = inBuffLength;
    apiFailData.MinInputSize = MinInputSize;
    apiFailData.MinOutputSize = MinOutputSize;
    apiFailData.CurBusState = hubExt->CurrentBusState;
    apiFailData.FdoFlags = hubExt->FdoFlags.ul;
    apiFailData.ErrorLine = 0;

    LOG(HubFdo, LOG_IOCTL, 'iov1', Irp, &apiFailData);

    nts = STATUS_SUCCESS;
    do {
        // the hub must be initialized and running to return useful information
        if (hubExt->CurrentBusState != BS_BusRun) {
            LOG(HubFdo, LOG_IOCTL, 'iov2', connectionIdx, inBuffLength);
            DbgTrace((UsbhIoctlTrace,">%!FUNC! bus not started\n"));
            nts = STATUS_UNSUCCESSFUL;
            apiFailData.ErrorLine = __LINE__;
            break;
        }

        // validate input buffer length
        if (inBuffLength < MinInputSize) {
            LOG(HubFdo, LOG_IOCTL, 'iov3', connectionIdx, inBuffLength);
            DbgTrace((UsbhIoctlTrace,">%!FUNC! inBuffLength < MinInputSize\n"));
            nts = STATUS_INVALID_PARAMETER;
            apiFailData.ErrorLine = __LINE__;
            break;
        }

        // make sure out buffer length is large enough to hold the minimum response
        if (outBuffLength < MinOutputSize) {
            LOG(HubFdo, LOG_IOCTL, 'iov4', connectionIdx, inBuffLength);
            DbgTrace((UsbhIoctlTrace,">%!FUNC! outBuffLength < MinOutputSize\n"));
            nts = STATUS_BUFFER_TOO_SMALL;
            apiFailData.ErrorLine = __LINE__;
            break;
        }

        // note the assumption here is that if there is an input buffer
        // the the first ULONG is the connection index.

        if (MinInputSize >= sizeof(*connectionIdx)) {

            connectionIdx = (PULONG) Irp->AssociatedIrp.SystemBuffer;
            apiFailData.ConnectionIdx = *connectionIdx;

            LOG(HubFdo, LOG_IOCTL, 'iov5', connectionIdx, inBuffLength);
            // make sure port number is valid
            // the test guys like to pass us garbage whenever possible
            if (*connectionIdx == 0 ||
                *connectionIdx > N_PORTS(HubFdo)) {
                LOG(HubFdo, LOG_IOCTL, 'iov5', connectionIdx, inBuffLength);
                DbgTrace((UsbhIoctlTrace,">%!FUNC! invalid port number\n"));
                nts = STATUS_INVALID_PARAMETER;
                apiFailData.ErrorLine = __LINE__;
                break;
            }
        }

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

    } WHILE (0);

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {

        // since many test programs call these APIs it is useful to log
        // failures.  Invalid API calls can generate false positives.
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_ApiValidationFailure,
                         nts, 0, &apiFailData, sizeof(struct _API_FAIL_DATA));
    }

    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    return nts;

}

/*
Routine: IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME

Description:

Returns the DriverKeyName associated with a PDO on a particular hub port. This is
the DevicePropertyDriverKeyName returned from IoGetDeviceProperty.  The caller inputs
the port number as the 'ConnectionIndex' and the UNICODE key name is returned.

This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_NODE_CONNECTION_DRIVERKEY_NAME structure

Input:
ConnectionIndex - this is the 1 based port number in the case of a hub.

Output:
ActualLength - actual length of this entire structure including name.
DriverKeyName - unicode driver key name for this node, including NULL.

<pre>

typedef struct _USB_NODE_CONNECTION_DRIVERKEY_NAME {
    ULONG ConnectionIndex;
    ULONG ActualLength;
    WCHAR DriverKeyName[1];
} USB_NODE_CONNECTION_DRIVERKEY_NAME

</pre>
*/

NTSTATUS
UsbhIoctlGetNodeConnectionDriverKeyName(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:
Returns the DriverKeyName associated with a PDO on a particular port.

This API returns FAILURE (STATUS_UNSUCCESSFUL) if the hub is not started or
otherwise not functional.

Parameters:
Irp->AssociatedIrp.SystemBuffer - pointer to
USB_NODE_CONNECTION_DRIVERKEY_NAME structure

Output:
USB_NODE_CONNECTION_DRIVERKEY_NAME filled in as appropriate

--*/
{
    NTSTATUS nts;
    PUSB_NODE_CONNECTION_DRIVERKEY_NAME dkn;
    ULONG outBuffLength;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_OBJECT pdo;
    IOCTL_FAIL_DATA ioctlFailData;
    USHORT pn = 0; // zero means no port
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
        
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    ioctlFailData.UsbIoctl = USB_GET_NODE_CONNECTION_DRIVERKEY_NAME;

    dkn = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>5', Irp, dkn);

    do {
        ULONG dknLength;

        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }      

        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0005, &apiLockAcquired);

        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                      USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
                                      Irp,
                                      IoStack,
                                      sizeof(ULONG),
                                      sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME));

        if (NT_ERROR(nts)) {
            break;
        }

        // parameters check out, get the PDO
        pn = (USHORT) dkn->ConnectionIndex;
        pdo = UsbhLatchPdo(HubFdo, (USHORT) dkn->ConnectionIndex, Irp, PdoLatchIo05_Tag);
        if (pdo == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        pdoExt = PdoExt(pdo);
        // this needs to be a PDO pnp is aware of and will not barf on
        // if we use it as a parameter
        if (!pdoExt->PdoFlags.PnpValid) {
            // PnP not ready yet
            nts = STATUS_INVALID_DEVICE_STATE;
            UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatchIo05_Tag);
            break;
        }

        // we have everything we need from the input buffer
        // now erase the entire client buffer
        RtlZeroMemory(dkn, outBuffLength);

        // restore the original connection index
        dkn->ConnectionIndex = pdoExt->PortNumber;

        // set length to space available to copy the name
        dknLength = outBuffLength -
                            sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME);

        nts = IoGetDeviceProperty(pdo,
                                  DevicePropertyDriverKeyName,
                                  dknLength,
                                  &dkn->DriverKeyName[0],
                                  &dknLength);
        LOG(HubFdo, LOG_IOCTL, 'igdp', Irp, nts);

        // eat the buffer too small error and return the actual length
        if (nts == STATUS_BUFFER_TOO_SMALL) {
            nts = STATUS_SUCCESS;
        }

        if (NT_ERROR(nts)) {
            UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatchIo05_Tag);
            break;
        }

        dkn->ActualLength = dknLength +
            sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME);


        // see how much data we actually copied
        if (outBuffLength >= dkn->ActualLength) {
            // user buffer is bigger, just indicate how much
            // we copied
            Irp->IoStatus.Information = dkn->ActualLength;

        } else {
            // it is not clear that IoGetDeviceProperty
            // returns anything in the case of BUFFER_TOO_SMALL
            // so to avoid returning initialized memory we will
            // just return the structure passed in
            dkn->DriverKeyName[0] = UNICODE_NULL;

            Irp->IoStatus.Information =
                sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME);
         }

         UsbhUnlatchPdo(HubFdo, pdo, Irp, PdoLatchIo05_Tag);

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'io<5', Irp, nts);

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, pn, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    // In case of an error set the ActualLength to zero.  Some 3rd-party drivers check this
    // length without checking the returned status for an error.
    if (NT_ERROR(nts) && (outBuffLength >= sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME))) {
        dkn->DriverKeyName[0] = UNICODE_NULL;
        dkn->ActualLength = sizeof(struct _USB_NODE_CONNECTION_DRIVERKEY_NAME);
    }
    
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0005);
    }        
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);
    
    UsbhCompleteIrp(Irp, nts);

    return nts;

}


/*
Routine: IOCTL_USB_GET_NODE_INFORMATION

Description:

Returns information about the USB device(hub or ccgp) in the user buffer passed in.
The NodeType field is set to *UsbHub* when handled by the usbhub.sys driver and
u.HubInformation is returned.  This includes the hub descriptor and a flag indicating
if the hub is bus or self powered.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_NODE_INFORMATION structure

Input:

Output:
NodeType - *UsbHub* if this sent to a hub.
HubInformation.HubDescriptor - USB hub class descriptor.
HubInformation.HubIsBusPowered - bus vs self power, this is the current operating mode of
the hub driver not necessarily the mode reported by the hub hardware.  For example, if a
registry key were used to force the hub to bus powered mode HubIsBusPowered would be
*TRUE*.


<pre>

typedef enum _USB_HUB_NODE {
    UsbHub,
    UsbMIParent
} USB_HUB_NODE;

typedef struct _USB_NODE_INFORMATION {
    USB_HUB_NODE NodeType;
    union {
        USB_HUB_INFORMATION HubInformation;
        USB_MI_PARENT_INFORMATION MiParentInformation;
    } u;
} USB_NODE_INFORMATION, *PUSB_NODE_INFORMATION;

typedef struct _USB_HUB_INFORMATION {
    USB_HUB_DESCRIPTOR HubDescriptor;
    BOOLEAN HubIsBusPowered;
} USB_HUB_INFORMATION, *PUSB_HUB_INFORMATION;

</pre>
*/

NTSTATUS
UsbhIoctlGetNodeInformation(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:
Returns information about the USB hub in the user buffer passed in.

Parameters:
Input:
Irp->AssociatedIrp.SystemBuffer - pointer to USB_NODE_INFORMATION structure

Output:
USB_NODE_INFORMATION filled in as appropriate

***
This information is accessible in any power state

IRQL: PASSIVE
--*/
{
    NTSTATUS nts;
    PUSB_NODE_INFORMATION ni;
    ULONG outBuffLength;
    PDEVICE_EXTENSION_HUB hubExt;
    IOCTL_FAIL_DATA ioctlFailData;
    PSSH_BUSY_HANDLE bh = NULL;
    BOOLEAN apiLockAcquired = FALSE;
       
    UsbhAssertPassive();
    DbgTrace((UsbhIoctlTrace,">%!FUNC!\n"));

    // first dispatch a message thru our idle state machine if we are not suspended this request 
    // function will return an error

    ioctlFailData.UsbIoctl = USB_GET_NODE_INFORMATION;

    hubExt = FdoExt(HubFdo);

    ni = (PUSB_NODE_INFORMATION) Irp->AssociatedIrp.SystemBuffer;
    outBuffLength = IoStack->Parameters.DeviceIoControl.OutputBufferLength;

    LOG(HubFdo, LOG_IOCTL, 'io>6', Irp, ni);

    // erase the entire client buffer
    RtlZeroMemory(ni, outBuffLength);

    do {
        
        bh = UsbhIncHubBusy(HubFdo, API_CONTEXT(HubFdo), Irp, UsbhIocBusyResume_Tag, RESUME_HUB);
        if (bh == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }            
        
        nts = UsbhAcquireApiLock(HubFdo, 0xf00d0006, &apiLockAcquired);

        if (NT_ERROR(nts)) {
            break;
        }

        nts = UsbhIoctlValidateParameters(HubFdo,
                                          USB_GET_NODE_INFORMATION,
                                          Irp,
                                          IoStack,
                                          0,
                                          sizeof(struct _USB_NODE_INFORMATION));

        if (NT_ERROR(nts)) {
            break;
        }

        // the hub must be initialized and running to return useful information

        UsbhAssert(HubFdo, hubExt->FdoFlags.HubDescriptorValid);

        LOG(HubFdo, LOG_IOCTL, 'io6d', &hubExt->HubDescriptor,
            hubExt->HubDescriptor.bNumberOfPorts);
        // return hub information
        ni->NodeType = UsbHub;
        RtlCopyMemory(&ni->u.HubInformation.HubDescriptor,
                      &hubExt->HubDescriptor,
                      sizeof(struct _USB_HUB_DESCRIPTOR));

        // 100 milliamps/port means bus powered
        ni->u.HubInformation.HubIsBusPowered = USB_BUS_POWERED_HUB(hubExt);

        Irp->IoStatus.Information = sizeof(struct _USB_NODE_INFORMATION);

        nts = STATUS_SUCCESS;

    } WHILE (0);

    if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, GenErr_UserIoctlFailed,
                         nts, 0, &ioctlFailData, sizeof(struct _IOCTL_FAIL_DATA));
    }

    if (apiLockAcquired) {
        UsbhReleaseApiLock(HubFdo, 0xf00d0006);
    }    

    LOG(HubFdo, LOG_IOCTL, 'io<6', Irp, nts);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", nts));
    UsbhIoctlTraceOutput(HubFdo, Irp, nts);

    UsbhDecHubBusy(HubFdo, API_CONTEXT(HubFdo), bh);

    UsbhCompleteIrp(Irp, nts);

    return nts;
}



NTSTATUS
UsbhInternalIoctlPropagateResume(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    )
/*++
Routine Description:
Indicates a global Resume. 

The hub driver must resume all of its downstream ports before completing the irp.

Parameters:
Input:

Output:


IRQL: PASSIVE
--*/
{
    Usbh_SSH_Event(HubFdo, SSH_Event_PropagateResume, SSH_API_CONTEXT(HubFdo));
    
    LOG(HubFdo, LOG_IOCTL, 'gblR', Irp, STATUS_SUCCESS);
    DbgTrace((UsbhIoctlTrace,"<%!FUNC! %!STATUS!\n", STATUS_SUCCESS));
   
    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhSendSynchronousUsbIoctlRequest(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG Ioctl
    )
/*++

Routine Description:

    Synchronously sends an IOCTL api to the the top of the device stack for a USB PDO.  
    Such APIs are typically targeted at the hub FDO but could be targeted at the PDOs.
    The important thing to keep in mind is that the function and filter drivers in will 
    see this request. 

    This routine will allocate the irp and reference the device stack across the call.

    This routine is used to send requests from the root hub down the stack to all 
    leaf nodes.

Arguments:   

    HubFdo -

    Pdo - Target PDO for the request, should be latched by the caller

Return Value:

    ntstatus code returned from the call

--*/
{
    NTSTATUS nts = STATUS_INVALID_PARAMETER;
    KEVENT event = {0};
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_OBJECT topOfDeviceStack;
    
    do {
        // check the status on our upstream port before issuing the request
        // if our status is disconnect then so is the child device we are talking
        // to.
        // Hubs never transmit commands to devices that are not connected to their
        // downstream ports.
        //nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);
        //
        // if (NT_ERROR(nts)) {
        //    break;
        // }

        nts = STATUS_SUCCESS;
            
        topOfDeviceStack = IoGetAttachedDeviceReference(Pdo); 

        if (topOfDeviceStack == NULL) {
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            break;
        }    

        INIT_EVENT_NOT_SIGNALED(&event);
        
        UsbhAssert(HubFdo,topOfDeviceStack);
        irp = IoBuildDeviceIoControlRequest(Ioctl,
                                            topOfDeviceStack,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            TRUE,  // INTERNAL
                                            &event,
                                            &ioStatus);

        if (irp == NULL) {
            ObDereferenceObject(topOfDeviceStack);
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // send packet directly to bus driver
        nts = IoCallDriver(topOfDeviceStack, irp);

        if (nts == STATUS_PENDING) {
            WAIT_EVENT(&event);
        } else {
            ioStatus.Status = nts;
        }

        nts = ioStatus.Status;

        // 
        ObDereferenceObject(topOfDeviceStack);

    } WHILE (0);

    LOG(HubFdo, LOG_IOCTL, 'SIOR', Ioctl, nts);
    
    return nts;
}


