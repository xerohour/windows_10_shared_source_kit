/*
Module: DIOCTL.C


name:USB kernel mode ioctls supported by the hub driver

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    2-10-2004, jdunn

Abstract:

    This code implements the 'device'  IOCTL Apis (Internal IOCTL) available to
    function drivers. (NOTE: Internal does not mean internal to Microsoft).

    These APIs are only available to kernel mode drivers.

    External documentation precedes each function, these blocks describe how the API
    works based on my review of the Windows XP SP1 codebase and the Windows Server SP1
    DDK.



USB Internal IOCTLs:

    + IOCTL_INTERNAL_USB_GET_HUB_COUNT
    + IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO
    + IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME
    + IOCTL_INTERNAL_USB_GET_BUS_INFO
    + IOCTL_INTERNAL_USB_SUBMIT_URB
    + IOCTL_INTERNAL_USB_GET_PORT_STATUS
    + IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE
    + IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX
    + IOCTL_INTERNAL_USB_RESET_PORT
    + IOCTL_INTERNAL_USB_ENABLE_PORT
    + IOCTL_INTERNAL_USB_CYCLE_PORT
    + IOCTL_INTERNAL_USB_GET_HUB_NAME
    + IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO
    + IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION
    + IOCTL_INTERNAL_USB_RECORD_FAILURE
    + IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY

*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "dioctl.tmh"
#endif


CCHAR usbfile_dioctl_c[] = "dioctl.c";

#define USBFILE usbfile_dioctl_c

/*
Routine: IOCTL_INTERNAL_USB_GET_HUB_COUNT

Description:

This function is used to count the number of hubs in the stack.  It is used
to enforce the electrical limitation of no more that 5 hubs plus the root
being chained together. It is typically only used by the usbhub.sys driver.

Irql - Any.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

ioStackLocation->Parameters.Others.Argument1 - pointer filled in with the
count of usb hubs in the chain.

*/


NTSTATUS
UsbhFdoReturnHubCount(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
Routine Description:
    Returns the number of hubs in the current chain.

Arguments:
ioStackLocation->Parameters.Others.Argument1 - pointer to the count.
>>Count

    This parameter contains a pointer that is filled in with the current count of
    hubs in the stack. Each instance of the hub driver that receives the IRP
    increments the counter and passes the irp on to its' PDO.  When the Irp
    reaches the hub that is the root the IRP is completed.  This will result in
    the count value being equal to the number of hubs (including the root).
    Since the number of hubs is an electrical consideration based on the max
    number of  external hubs it would be possible to increase the allowed number
    of hubs by having an embedded hub not increment the count on the way down.

IRQL: Any

Return Value:

    NT STATUS CODE


--*/
{
    PULONG count;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    count = ioStack->Parameters.Others.Argument1;

    if (!hubExt->FdoFlags.HubIsRoot) {
        // bump one for this hub and pass on
        if (count) {
            (*count)++;
        }
        nts = UsbhFdoChainIrp(HubFdo, Irp);

        UsbhDecPdoIoCount(Pdo, Irp);
        return nts;
    }

    if (count == NULL) {
        nts = STATUS_INVALID_PARAMETER;
    } else {
        (*count)++;
        nts = STATUS_SUCCESS;
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}

/*
Routine: IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO

Description:

This routine handles the GET_ROOT_HUB_PDO api if the hub is loaded for the
root otherwise it passes it on to the PDO who chains it to the next hub.

The TopOfBusStack is the top of the root hub PDO stack -- the top of the device
stack the hub driver attaches to when loaded for the root hub PDO. All USB
traffic is the forwarded to the top of this device stack.  This API is typically
only used by usbhub.sys.

Irql - Any.

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

ioStackLocation->Parameters.Others.Argument1 - pointer to RootHubPhysicalDeviceObject.
This parameter contains a pointer that is filled in with the root hub PDO.
This is the actual PDO created by the USBPORT driver for the root hub.

ioStackLocation->Parameters.Others.Argument2 - pointer to TopOfBusStack.
This parameter contains a pointer that is filled in with the top of the bus
driver stack is the device object returned when the hub driver attached to
top of the device stack associated with the root hub PDO.


<pre>
* This diagram only applies to WinNT6 (this implementation).

+------+
|DEVICE|
+------+
   ||
IoCall(Pdo)
   ||
+======+
| PDO  |-----+
+======+     |
   ||        |
   ||   +----------+
   ||   | HUB FDO  |
   ||   +----------+
   ||        ||
   ||     IoCall()
   ||        ||
   ||   +==========+     +----------+
   ||   |    PDO   |-----| HUB FDO  |
   ||   +==========+     +----------+
   ||                        ||
   ||                      IoCall()
IoCall(TopOfBusStack)        ||
   ||                    +==========+     +----------+
   ||                    |   PDO    |-----| HUB FDO  |
   ||                    +==========+     +----------+
   ||                                          ||
   ||     Urb Transfer Fast Path          +----------+
   ++-------------->>-------------------->|Bus Filter| --- TopOfBusStack (bus filter is optional)
          Q-Interface Path                +----------+
                                               ||
                                          +==========+     +---------+
                                          | Root PDO |---->| HCD FDO |
                                          +==========+     +---------+
                                                ^
                                                |
                                   (RootHubPhysicalDeviceObject)
</pre>
*/


NTSTATUS
UsbhFdoReturnRootHubPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:
    This function is used to by hubs to get the top of the physical USB stack.
    All IRPs passed to a hub PDO are either serviced by the hub or forwarded
    directly to the top of the bus driver stack i.e. the root hub PDO.  A filter
    driver interested only in bus traffic (AKA Urbs) can see such traffic by
    attaching to the top of the root hub PDO, see section 9.

    QUERY_INTERFACE calls also follow the fast path since the hub must insert
    appropriate device handle in the request.

Arguments:
ioStackLocation->Parameters.Others.Argument1 - pointer to RootHubPhysicalDeviceObject
ioStackLocation->Parameters.Others.Argument2 - pointer to TopOfBusStack

Return Value:

    NT STATUS CODE
--*/
{
    PDEVICE_OBJECT *rootHubPhysicalDeviceObject;
    PDEVICE_OBJECT *TopOfBusStack;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    if (!hubExt->FdoFlags.HubIsRoot) {
        UsbhDecPdoIoCount(Pdo, Irp);
        return UsbhFdoChainIrp(HubFdo, Irp);
    }

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    rootHubPhysicalDeviceObject =
        ioStack->Parameters.Others.Argument1;
    TopOfBusStack =
        ioStack->Parameters.Others.Argument2;

    if (TopOfBusStack == NULL ||
        rootHubPhysicalDeviceObject == NULL) {

        nts = STATUS_INVALID_PARAMETER;
    } else {
        // return with a reference to these objects, caller must release them
        // *see if we can figure out how to make the host unload before the hub for testing
        *TopOfBusStack = hubExt->TopOfBusStack;
        ObReferenceObject(hubExt->TopOfBusStack);
        *rootHubPhysicalDeviceObject = hubExt->PhysicalDeviceObject;
        ObReferenceObject(hubExt->PhysicalDeviceObject);
        nts = STATUS_SUCCESS;
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


/*
Routine: IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME

Description:

This function returns the controllers Unicode symbolic device name.

Irql - Any.

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

ioStackLocation->Parameters.Others.Argument1 - A pointer to a structure to receive the symbolic name.
ActualLength is set to the size of the controller name even if there is not enough room to return
the whole name.

ioStackLocation->Parameters.Others.Argument2 - The size of the buffer passed in Argument1 in bytes.

*/


NTSTATUS
UsbhFdoReturnControllerName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Fetches the controller symbolic name through the USBD bus interface.

Arguments:
ioStackLocation->Parameters.Others.Argument1
A pointer to a structure to receive the symbolic name.

ioStackLocation->Parameters.Others.Argument2
The size of the buffer passed in Argument1 in bytes

IRQL: Any

Return Value:

    nt status code

--*/
{
    NTSTATUS nts;
    PUSB_HUB_NAME userBuffer;
    ULONG userBufferLength;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    userBufferLength = PtrToUlong(ioStack->Parameters.Others.Argument2);
    userBuffer = ioStack->Parameters.Others.Argument1;

    nts = UsbhGetControllerName(HubFdo, userBuffer, userBufferLength);

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


/*
Routine: IOCTL_INTERNAL_USB_GET_BUS_INFO

Description:

This function returns device relative bus information to the caller.  That is
the bus information is specific the PDO and its associated device handle.
Client USB drivers should not use this API, they should use the
QueryBusInformation function in the USBD bus interface instead because it does
not require an IRP.


Irql - Any.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

ioStackLocation->Parameters.Others.Argument1 - A pointer to a structure to receive the
bus information  USB_BUS_NOTIFICATION.

<pre>

typedef struct _USB_BUS_NOTIFICATION {
    USB_NOTIFICATION_TYPE NotificationType;
    ULONG TotalBandwidth;
    ULONG ConsumedBandwidth;
    ULONG ControllerNameLength;
} USB_BUS_NOTIFICATION, *PUSB_BUS_NOTIFICATION;

</pre>
*/


NTSTATUS
UsbhFdoReturnDeviceBusInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Returns device (bus) relative bus information.  In some cases this information
    depends on the type of hub the device is attached to so we must get the
    device relative bus context first thru the hub FDO.

Parameters:
ioStackLocation->Parameters.Others.Argument1
A pointer to a structure to receive the bus information  USB_BUS_NOTIFICATION.

IRQL: Any

Return Value:

    nt status code

--*/
{
    NTSTATUS nts;
    PUSB_BUS_NOTIFICATION userBuffer;
    ULONG userBufferLength;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;

    DbgTrace((UsbhDebugTrace,">%!FUNC! %p\n", Pdo));
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);

    userBufferLength = sizeof(struct _USB_BUS_NOTIFICATION);
    userBuffer = ioStack->Parameters.Others.Argument1;

    nts = UsbhGetDeviceBusInfo(HubFdo, Pdo, userBuffer, userBufferLength);

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


/*
Routine: IOCTL_INTERNAL_USB_GET_PORT_STATUS

Description:
This function returns the current 'live' status of the port.  It can be used
by client drivers to determine the current state of their device because
certain hardware errors on the bus can result in a device port being disabled.
The hub driver must communicate with the hub to get this information, if
it cannot for some reason a failure status is returned.


Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000)?, WinNT5.1(XP), WinNT6

Parameters:

ioStackLocation->Parameters.Others.Argument1 - A pointer to a ULONG that is filled in with
the port status bits defined below:

<pre>
USBD_PORT_ENABLED      0x00000001
USBD_PORT_CONNECTED    0x00000002
</pre>
*/

NTSTATUS
UsbhFdoReturnPortStatus(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:
    Really should be called GetPdoStatus since the caller wants to know if THIS device
    is currently connected.

Parameters:
    ioStackLocation->Parameters.Others.Argument1
    A pointer to a ULONG that is filled in with the port status bits defined below:

    USBD_PORT_ENABLED      0x00000001
    USBD_PORT_CONNECTED    0x00000002

IRQL - Passive
    we query the port status registers synchronously.

Return Value:

    This routine returns success and the current status of the port as long as
    the THIS device is physically connected and a connect change is not asserted.
    If a connect change is asserted or the connect status is clear we return
    NT_DEVICE_DISCONNECTED_STATUS.

    In the case of NT_DEVICE_DISCONNECTED_STATUS Argument1 will still be set with
    the live state of the port -- this may be USBD_PORT_CONNECTED if the device
    dropped on and off the bus

* = not valid

        state                                       nt status
1       USBD_PORT_ENABLED,USBD_PORT_CONNECTED       STATUS_SUCCESS
2*      USBD_PORT_ENABLED,USBD_PORT_CONNECTED       NT_DEVICE_DISCONNECTED_STATUS
3       USBD_PORT_CONNECTED                         STATUS_SUCCESS
4*      USBD_PORT_CONNECTED                         NT_DEVICE_DISCONNECTED_STATUS
5*      0                                           STATUS_SUCCESS
6       0                                           NT_DEVICE_DISCONNECTED_STATUS
7*      USBD_PORT_ENABLED                           STATUS_SUCCESS
8*      USBD_PORT_ENABLED                           NT_DEVICE_DISCONNECTED_STATUS

1 - normal case
3 - port disabled for some reason, hardware error
6 - device removed

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION ioStack;
    PULONG ps;
    USB_HUB_PORT_STATE portState;
    USBD_STATUS usbdStatus;
    NTSTATUS nts, ps_nts;
    PSTATE_CONTEXT sc;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);

    LOG(HubFdo, LOG_HUB, 'GPS>', Irp, 0);

    // if called at raised IRQL then fail the API
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        UsbhCompleteIrp(Irp, STATUS_INVALID_PARAMETER);
        UsbhDecPdoIoCount(Pdo, Irp);
        return STATUS_INVALID_PARAMETER;
    }

    ps = ioStack->Parameters.Others.Argument1;

    if (!ps) {
        UsbhCompleteIrp(Irp, STATUS_INVALID_PARAMETER);
        UsbhDecPdoIoCount(Pdo, Irp);

        LOG(HubFdo, LOG_HUB, 'GPS!', Irp, 0);
        return STATUS_INVALID_PARAMETER;
    }
    *ps = 0;

    sc = PDO_CONTEXT(Pdo);

   
    // validate against our own power state

    UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_API, PdoCheckPortTag);

    if (GET_FDO_POWER_STATE(sc) == Fdo_PowerFailureDisconnected ||
        GET_FDO_POWER_STATE(sc) == Fdo_PowerFailure) {
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        LOG(HubFdo, LOG_HUB, 'GPdx', Irp, 0);

        UsbhCompleteIrp(Irp, NT_DEVICE_DISCONNECTED_STATUS);
        UsbhDecPdoIoCount(Pdo, Irp);
        return NT_DEVICE_DISCONNECTED_STATUS;

    } else {
        UsbhReleaseFdoPwrLock(HubFdo, sc);
    }


    // Pdo is held valid by the iolock, peek at the state of the PDO
    // this will also tell us the current 'connect' status.

    UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_PortStatus);
    if (GET_PDO_HWPNPSTATE(Pdo) != Pdo_PnpRefHwPresent) {
        UsbhReleasePdoStateLock(HubFdo, sc);

        UsbhCompleteIrp(Irp, NT_DEVICE_DISCONNECTED_STATUS);
        UsbhDecPdoIoCount(Pdo, Irp);

        LOG(HubFdo, LOG_HUB, 'GPSx', Irp, 0);
        return NT_DEVICE_DISCONNECTED_STATUS;;

    } else {
        UsbhReleasePdoStateLock(HubFdo, sc);
    }

//ssh pnp
    // check selective suspend state    
    ps_nts = Usbh_SSH_Event(sc->HubFdo, SSH_Event_GetPortStatus, sc);

    if (NT_ERROR(ps_nts)) {
        // hub is globally suspended (selective suspend)

        // if the hub is globally suspended we return status bits to indicate the device us connected unless 
        // the PDO hardware state is physically disconnected.

        nts = STATUS_SUCCESS;

        *ps |= (USBD_PORT_CONNECTED | USBD_PORT_ENABLED);
        
    } else {       
        // hub is not globally suspended
    
        // attempt to get the actual status for the callers port
        // this will return an error if we are not connected
        nts = UsbhQueryPortState(HubFdo,
                                 pdoExt->PortNumber,
                                 &portState,
                                 &usbdStatus);
    
        LOG(HubFdo, LOG_HUB, 'GPS3', portState.Status.us, nts);

        if (NT_SUCCESS(nts)) {
            if (portState.Status.Connect) {
                *ps |= USBD_PORT_CONNECTED;
            } else {
                nts = NT_DEVICE_DISCONNECTED_STATUS;
            }

            if (portState.Status.Enable) {
                *ps |= USBD_PORT_ENABLED;
            }

            // Test if we unexpectedly lost power, if we did then the first IOCTL call will indicate enable
            // clear.
            // The logic here is that the function driver may be trying to reset because of this so we
            // want to encourage a reset to occur.

            if (pdoExt->VbusState == Vbus_Interrupted) {

                *ps &= ~USBD_PORT_ENABLED;
                //pdoExt->VbusState = Vbus_On;

            }

            if (portState.Change.ConnectChange) {
                UsbhAssert(HubFdo, portState.Status.Enable == 0);
                nts = NT_DEVICE_DISCONNECTED_STATUS;
            }
        } else {
            // map other errors to device not connected
            // if the hub goes into reset it won't be connected anyway
            nts = NT_DEVICE_DISCONNECTED_STATUS;
        }
    }
        
    LOG(HubFdo, LOG_HUB, 'GPS4', *ps, nts);

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}


/*
Routine: IOCTL_INTERNAL_USB_RESET_PORT

Description:
This IOCTL is used by kernel mode drivers to reset their upstream port. This is
a synchronous that returns when the reset has completed.

After a successful reset the device is re-configured to the same configuration it
was in before the reset.  All pipe handles, configuration handles and interface
handles remain valid after the reset.  The device is effectively re-enumerated
(same process as when it is attached) but this is outside of the context of PnP so
no plug and play notifications will be associated with this event.


Irql - this API must called at passive level.

Power State - this API is only valid if the device is in D0.

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

none.


*/

NTSTATUS
UsbhFdoResetPdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Function driver reset port.

    An entire paper could be written on this one API.

    This is the API exported thru the PDO that allows the function driver to reset the USB port
    associated with its device.  We must validate the request before attempting to reset the port.

    The pdo must still be in the 'present' state and associated with the port ie not unplugged.
    This PDO is attached to the request to reset the port.  If the request reaches the dispatcher and
    the PDO is not valid it will be ignored. So we check at both the top of the request (here) and
    the bottom (when it is dequeued) for a valid PDO.  The check at the top is really an optimization
    to avoid having to queue the request just to fail it -- the PDO can go invalid while queued.
    The PDO will not be deleted because we hold a latch.

Arguments:

    HubFdo - Hub the PDO is attached to.

    Pdo - Pdo who's port needs to be reset.

    Irp - The irp associated with the request.

IRQL - Passive

Return Value:

    nt status code

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PDEVICE_OBJECT curPdo;
    PSTATE_CONTEXT sc;
    HUB_PDO_POWER_STATE pdoPowerState;
        
    LOG(HubFdo, LOG_BUS, 'res!', Irp, Pdo);
    pdoExt = PdoExt(Pdo);
    // this is done in the context of the PDO.
    sc = &pdoExt->PdoStateContext;

    UsbhKdPrint((1, "<PDO Port Reset port[%d] %p>\n", pdoExt->PortNumber, Pdo));

    UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_IOCTL_INTERNAL_USB_RESET_PORT_START, 0);    

    do {
        // validate the power state -- we only allow this in D0
        UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_DRIVER_RESET, PdoDeviceRseet_Tag);
        
        pdoPowerState = GET_PDO_POWER_STATE(Pdo);

        if (Pdo_D0 != pdoPowerState) {
                
            nts = STATUS_POWER_STATE_INVALID;
            
            // throw an exception in case someone wants to know why their reset call failed.
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, ResetPdoPort_CallerNotInD0,
                              nts, 0xFFFFFFFF, NULL, 0);
            
            UsbhReleaseFdoPwrLock(HubFdo, sc);
            break;
        }
        UsbhReleaseFdoPwrLock(HubFdo, sc);
        
    
#define MAX_PDO_RESETS      10
        // This API is spec'ed such that it must be called at PASSIVE level, if this is not
        // the case we just fail the request.
        //
        // We could modify our code to support calling this API at DPC level but I suspect
        // that will just create compatibilty issues with older versions of WinNT5.1(XP).
        if (KeGetCurrentIrql() > PASSIVE_LEVEL) {

            nts = STATUS_INVALID_PARAMETER;

            // throw an exception in case someone wants to know why their reset call failed.
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, ResetPdoPort_CallerNotPassiveLevel,
                              nts, 0xFFFFFFFF, NULL, 0);
            break;
        } 

        nts = NT_DEVICE_DISCONNECTED_STATUS;
        UsbhAssertPassive();

        // latching the PDO allows us to detect if the device is being removed.
        // The latch will fail if the hub doesn't point to
        // a PDO
        curPdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, Irp, PdoLatchResetPort_Tag);
        LOG(HubFdo, LOG_HUB, 'rstl', pdoExt->PortNumber, curPdo);

        if (curPdo) {
            // make sure this is our PDO
            if (Pdo == curPdo) {

                UsbhAcquirePdoStateLock(HubFdo, sc, PdoEv_DeviceReset);
                if (GET_PDO_HWPNPSTATE(Pdo) == Pdo_PnpRefHwPresent) {
                    UsbhReleasePdoStateLock(HubFdo, sc);

                    Usbh__TestPoint__Ulong(HubFdo, TstPt_DriverReset, nts, pdoExt->PortNumber);

                    nts = UsbhSyncResetDeviceInternal(HubFdo, sc, Pdo);

                    // track how many resets have been attempted
                    pdoExt->HardErrorCount++;
                    // first driver reset restores vbus state.
                    pdoExt->VbusState = Vbus_On;

                    if (pdoExt->HardErrorCount > MAX_PDO_RESETS) {
                        // more than X resets indicates a potential hardware issue we
                        // need to take note of, log an exception
                        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, ExcessiveDriverResetsDetected,
                          nts, 0xFFFFFFFF, NULL, 0);

                        // reset the counter so we don't log again until it is exceeded
                        pdoExt->HardErrorCount = 0;
                    }

                    if (NT_SUCCESS(nts)) {
                        // this will invoke a handle leak check in the unit test.
                        // ** only do this in the success case
                        Usbh__TestPoint__Ulong(HubFdo, TstPt_DriverResetComplete, nts,
                            pdoExt->PortNumber);
                    }                        
                } else {
                    // device is being removed, fail the request now.

                    nts = NT_DEVICE_DISCONNECTED_STATUS;
                    UsbhReleasePdoStateLock(HubFdo, sc);
                }
            }

            UsbhUnlatchPdo(HubFdo, curPdo, Irp, PdoLatchResetPort_Tag);
        }

        if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, DriverPortResetFailed,
                              nts, 0xFFFFFFFF, NULL, 0);

        }
    } WHILE (0);
    
    UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_IOCTL_INTERNAL_USB_RESET_PORT_COMPLETE, nts);    

    if (pdoExt->IsBootDevice && !NT_SUCCESS(nts)) {
        
        LOG(HubFdo, LOG_HUB, 'rstf', pdoExt, nts);
        DbgTrace((UsbhPwBootTrace,"Device Reset Failure %!FUNC!\n"));

        HUB_RESET_EXCEPTION(USBFILE, HubFdo, 0, HubErr_HardError_Reset,
                  STATUS_UNSUCCESSFUL, USBD_STATUS_HUB_INTERNAL_ERROR, NULL, 0);

        pdoExt->PendingResetPortAsyncIrp = Irp;
        IoMarkIrpPending(Irp);
        nts = STATUS_PENDING;

    } else {
        UsbhCompleteIrp(Irp, nts);
    }

    UsbhKdPrint((1, ">PDO Port Reset %p %x<\n", Pdo, nts));
    UsbhDecPdoIoCount(Pdo, Irp);

#undef MAX_PDO_RESETS

    return nts;
}


/*
Routine: IOCTL_INTERNAL_USB_CYCLE_PORT

Description:
This IOCTL will simulate a plug/unplug on the drivers upstream port.  The device will be
removed and re-added by PnP.  This API is typically used.

Irql - this API must called at passive level.

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

none.


*/

NTSTATUS
UsbhFdoCyclePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++
    This is an API exported to client USB drivers.  It basically allows the
    driver to request PnP to re-enumerate it.

    Typically drivers use this API after they download brains to the device.
    It is then re-enumerated as another device.

    We implement this by disabling the port and queueing an artificial connect
    change. The enumeration engine will interpret this as a disconnect connect
    and respond appropriately.

IRQL - Passive


Arguments:

Return Value:

    Status of the operation.  We return success if we are successful in
    queueing the change request.

--*/
{
    NTSTATUS nts;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;

    LOG(HubFdo, LOG_HUB, 'io>1', Irp, 0);

    // cycleport is itself interesting because it is uncommon
    // for this reason we treat it as an exception so we have
    // a record of it.
    HUB_EXCEPTION(USBFILE,  HubFdo, PdoExt(Pdo)->PortNumber,
                        GenErr_IoctlCycleDetected,
                        0, 0, NULL, 0);

    if (KeGetCurrentIrql() <= PASSIVE_LEVEL) {
        nts = UsbhCycleDevicePort(HubFdo, Pdo, &usbdStatus);

        if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, PdoExt(Pdo)->PortNumber, GenErr_IoctlCycleFailed,
                        nts, usbdStatus, NULL, 0);
        }

    } else {
        nts = STATUS_INVALID_PARAMETER;
        HUB_EXCEPTION(USBFILE,  HubFdo, PdoExt(Pdo)->PortNumber,
                        GenErr_IoctlCycleFailed_InvalidIrql,
                        nts, 0, NULL, 0);
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}

/*
Routine: IOCTL_INTERNAL_USB_ENABLE_PORT

Description:
This API is no longer supported, for legacy reasons we just return success.

Irql - n/a

Windows Version - WinNT5.0(2000), WinNT5.1(XP), WinNT6

Parameters:

none.


*/


NTSTATUS
UsbhFdoEnablePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    This API is no longer supported, for legacy reasons we just return success.

Arguments:

Return Value:

    always success

--*/
{
    UsbhCompleteIrp(Irp, STATUS_SUCCESS);
    UsbhDecPdoIoCount(Pdo, Irp);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhFdoReturnHubName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
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

 USB_BUS_NOTIFICATION.
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

IRQL: Any

Return Value:

    nt status code

--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PUSB_HUB_NAME userBuffer;
    ULONG userBufferLength;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    LOG(HubFdo, LOG_HUB, 'rhn>', Irp, 0);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    userBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
    userBuffer = (PUSB_HUB_NAME) Irp->AssociatedIrp.SystemBuffer;

    nts = UsbhGetHubPdoName(HubFdo,
                            Pdo,
                            userBuffer,
                            userBufferLength,
                            // Irp Information defined as type ULONG_PTR
                            // because its only 32 bits wide and sometimes it
                            // is used as a pointer, we cast to PULONG
                            // because in this case it is not used as a pointer
                            (PULONG) &Irp->IoStatus.Information);

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}


NTSTATUS
UsbhFdoReturnParentHubInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

This API is obsolete and no longer supported. It was previously used for device
bay implementations.

Arguments:

Return Value:

    nt status code

--*/
{
    UsbhCompleteIrp(Irp, STATUS_NOT_SUPPORTED);
    UsbhDecPdoIoCount(Pdo, Irp);

    return STATUS_NOT_SUPPORTED;

}


NTSTATUS
UsbhFdoSubmitPdoIdleNotification(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handler for the client driver submission of the idle notification irp.

    The calling routine validates that the driver has not already
    processed a remove.

Arguments:

Return Value:

    nt status code

--*/
{
    NTSTATUS nts;
    PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;

    if (HubG.GlobalFailSelectiveSuspend) {
        UsbhCompleteIrp(Irp, STATUS_NOT_SUPPORTED);
        UsbhDecPdoIoCount(Pdo, Irp);
        return STATUS_NOT_SUPPORTED;
    }

    // this unusual check is taken directly from the original hub
    // if the callback info is invalid the irp is completed with
    // STATUS_NO_CALLBACK_ACTIVE
    idleCallbackInfo = (PUSB_IDLE_CALLBACK_INFO)
           IoGetCurrentIrpStackLocation(Irp)->\
               Parameters.DeviceIoControl.Type3InputBuffer;

    if (idleCallbackInfo && idleCallbackInfo->IdleCallback) {
        nts = UsbhIdleIrp_Event(HubFdo, Pdo, Irp, Ev_IdleIrp_Enqueue, 0);
    } else {
        // this was behavior taken from XP
        nts = STATUS_NO_CALLBACK_ACTIVE;
        // nts = STATUS_INVALID_PARAMETER;
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);

        HUB_EXCEPTION(USBFILE, HubFdo, 0, PnpErr_BadIdleIrpParameters,
                       nts, 0, NULL, 0);
    }

    return nts;

}

/*
Routine: IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY

Description:
This IOCTL is sent when the driver attatched to a USB PDO wishes to indicate that it the device 
is 'ready' to go idle.  This Irp is good until canceled by the caller or completed by the hub 
driver. A device is only 'idle ready' while this irp is pending.

Irql - any

Windows Version - WinNT6

Parameters:

none.


*/

NTSTATUS
UsbhFdoPdoIoctlIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handler for the client driver submission of the extended idle notification irp.

    The calling routine validates that the driver has not already
    processed a remove.

Arguments:

Return Value:

    nt status code

--*/
{    
    return UsbhIdleIrp_Event(HubFdo, Pdo, Irp, Ev_IdleExIrp_Enqueue, 0);
}


/*
Routine: IOCTL_INTERNAL_USB_RECORD_FAILURE

Description:

Records START_FAILURE info in the PDO for later reference.  This API is available to USB function drivers so they
may record information about PNP start failures for later diagnosis. Since PNP will unload a driver that fails 
start a driver cannot record this information internally. 

Irql - Any.

Windows Version -  WinNT6

Parameters:

ioStack->Parameters.Others.Argument1 points to a _USB_START_FAILDATA structure.  It should be filled in as follows:

<pre>
    LengthInBytes - length of entire structure including optional function driver data, this must be at least 
        sizeof(struct _USB_START_FAILDATA) or the data will not be recorded.
        
    NtStatus - NT status code returned when start was failed. 
    
    UsbdStatus - USBD status code that caused the failure to start, if applicable.
    
    ConnectStatus - current connection status, this value should be set to DeviceGeneralFailure for most function 
        drivers, it is used by the hub internally to report errors that need special UI handling.
        
    DriverData - Additional function driver data, this can be anything the function driver needs to diagnose the cause 
    of the start failure.  This data is limited to a maximum of 4096 bytes. 

</pre>

Return Value:

STATUS_SUCCESS if the data could be logged.

STATUS_INVALID_PARAMETER if the PDO already has data recorded.

STATUS_INVALID_PARAMETER if NULL is passed in for source.

STATUS_BUFFER_TOO_SMALL if faidata length is shorter than sizeof(faildata).

*/

NTSTATUS
UsbhFdoRecordFailure(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    See IOCTL_INTERNAL_USB_RECORD_FAILURE
    Records START_FAILURE info in the PDO for later reference.  This API is available to USB function drivers so they
    may record information about PNP start failures for later diagnosis. 
        

Arguments: 

    Pdo - Pdo for device that failed to start

    Irp - irp containing falidata structure

Return Value:

    STATUS_SUCCESS if the data could be logged

    STATUS_INVALID_PARAMETER if the PDO already has data recorded.
    STATUS_INVALID_PARAMETER if NULL is passed in for source.

    STATUS_BUFFER_TOO_SMALL if fakdat length is shorter than sizeof(faildata);
    
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_START_FAILDATA failData;
    ULONG nBytes;
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts;
    PHUB_PORT_DATA pd = NULL;
    
    // allocate a structure to keep it

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    failData = ioStack->Parameters.Others.Argument1;

    do {

        LOG(HubFdo, LOG_HUB, 'fda1', pdoExt->FailData, failData);
        if (pdoExt->FailData != NULL) {
            TEST_TRAP();
            // faildata already recorded                
            nts = STATUS_INVALID_PARAMETER;
            break;
        }    
        
        if (failData == NULL) {
            LOG(HubFdo, LOG_HUB, 'fda2', pdoExt->FailData, 0);
            // null src pointer passd in
            nts = STATUS_INVALID_PARAMETER;
            break;
        }            
        
        nBytes = failData->LengthInBytes;
        LOG(HubFdo, LOG_HUB, 'fda3', pdoExt->FailData, nBytes);
        if (nBytes > 4096) {
            // #147
            nBytes = 4096;
        }                
            
        if (nBytes < sizeof(struct _USB_START_FAILDATA)) {
           
            // #146
            // buffer too small
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        UsbhAllocatePool_Z(pdoExt->FailData, NonPagedPool, nBytes);
        if (pdoExt->FailData == NULL) {
            TEST_TRAP();        

            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        } 

        
        RtlCopyMemory(pdoExt->FailData, failData, nBytes);

        //
        // Special handling for too-many-hubs-deep UI
        //
        if(failData->ConnectStatus == DeviceHubNestedTooDeeply){
            pd = UsbhGetPortData(HubFdo, pdoExt->PortNumber);

            if(pd){

                UsbhSetConnectionStatus(HubFdo, pd, DeviceHubNestedTooDeeply);

                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, NestedHubs_Popup,
                              failData->NtStatus, failData->UsbdStatus, NULL, 0);
            }
        }

        //
        // Record an exception with some basic info
        //
        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PdoFailureData,
                              failData->NtStatus, failData->UsbdStatus, NULL, 0);

        UsbhKdPrint((0, "PDO(%x) recorded start failure\n", Pdo));
        nts = STATUS_SUCCESS;
    } WHILE (0);
    
    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


/*
Routine: IOCTL_INTERNAL_GET_USB_TOPOLOGY_ADDRESS

Description:

Returns the USB Topology address for a PDO.

Irql - this API may be called at any level.

Windows Version - WinNT6

Parameters:

Irp->AssociatedIrp.SystemBuffer - pointer to USB_TOPOLOGY_ADDRESS structure

Output:

Topology Address for the PDO, see format description.


<pre>
Every USB PDO has an topology address that describes the physical location of the device in
the system.   This is particularly useful when the USB device fails to enumerate.  The USB
topology address is displayed is returned as the location string for USB devices.

The topology address consists of 10 digits separated by a '.' the format is:

PciBN.PciDev.PciFunc.RHP.L1.L2.L3.L4.L5.L6

##.##.##.##.##.##.##.##.##.##

BusNumber}
Device   }---------------PCI address of host
Function }
RootHub_Port (level 0)
Port_L1 (level 1)
Port_L2
Port_L3
Port_L4
Port_L5
Port_L6

The first three digits identify the host controller by bus,func,dev id on PCI. The
following digits identify the port the device is attached to at the level of the USB
tree (The USB tree only supports 5 hubs) levels.  If the device is a leaf node or a
hub then the following digits will be zero.


Examples:
Device on root hub port 1:
0.1.7.1.0.0.0.0.0.0

If the above device where a hub then the a device attached to port 7 would have this
address.
0.1.7.1.7.0.0.0.0.0


For shorthand purposes the trailing zeroes may be omitted:
0.1.7.1
and
0.1.7.1.7


</pre>
*/

NTSTATUS
UsbhFdoReturnTopologyAddress(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Fetches the topology address. This API is passed down the stack until it reaches
    the root hub PDO where the PCI values are set. The port fields are filled in by the
    completion.

Arguments:
ioStackLocation->Parameters.Others.Argument1
A pointer to a structure to receive the topology address.

IRQL: Any

Return Value:

    nt status code


--*/
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_TOPOLOGY_ADDRESS address = NULL;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    address = (PUSB_TOPOLOGY_ADDRESS)ioStack->Parameters.Others.Argument1;

    if(!address){
        nts = STATUS_INVALID_PARAMETER;
        UsbhCompleteIrp(Irp, nts);
        UsbhDecPdoIoCount(Pdo, Irp);
    }

    nts = UsbhBuildTopologyAddress(HubFdo,
                                   address,
                                   pdoExt->PortNumber);


    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);
    return nts;
}


NTSTATUS
UsbhFdoReturnDeviceConfigInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
{
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    NTSTATUS nts = STATUS_SUCCESS;
    PHUB_INTERNAL_CONFIG_INFO internalConfigInfo = NULL;
    PIO_STACK_LOCATION ioStack = NULL;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    internalConfigInfo = ioStack->Parameters.Others.Argument1;

    if(!internalConfigInfo){
        nts = STATUS_INVALID_PARAMETER;
        goto UsbhFdoReturnDeviceConfigInfoDone;
    }

    if(internalConfigInfo->Version != 1){
        nts = STATUS_INVALID_PARAMETER;
        goto UsbhFdoReturnDeviceConfigInfoDone;
    }

    if(internalConfigInfo->Length != sizeof(HUB_INTERNAL_CONFIG_INFO)){
        nts = STATUS_BUFFER_TOO_SMALL;
        goto UsbhFdoReturnDeviceConfigInfoDone;
    }

    RtlZeroMemory(internalConfigInfo, sizeof(HUB_INTERNAL_CONFIG_INFO));
    internalConfigInfo->Version = 1;
    internalConfigInfo->Length = sizeof(HUB_INTERNAL_CONFIG_INFO);

    internalConfigInfo->HubFlags.ul = 0;

    if (hubExt->FdoFlags.HighSpeedCapable) {
        internalConfigInfo->HubFlags.HubIsHighSpeedCapable = 1;
    }

    if (hubExt->FdoFlags.MultiTTmode) {
        internalConfigInfo->HubFlags.HubIsMultiTt = 1;
    }

    if (hubExt->FdoFlags.MultiTTsupported) {
        internalConfigInfo->HubFlags.HubIsMultiTtCapable = 1;
    }

    if ((hubExt->FdoFlags.HighSpeedCapable &&
         hubExt->FdoFlags.HubIsRoot) ||
        hubExt->FdoFlags.HubIsTT){
        
        internalConfigInfo->HubFlags.HubIsHighSpeed = 1;
    }

    if(hubExt->FdoFlags.HWC_ARM){
        internalConfigInfo->HubFlags.HubIsArmedWakeOnConnect = 1;
    }

    if(hubExt->FdoFlags.HubIsRoot){
        internalConfigInfo->HubFlags.HubIsRoot = 1;
    }

    if(USB_BUS_POWERED_HUB(hubExt)){
        internalConfigInfo->HubFlags.HubIsBusPowered = 1;
    }

    if(pdoExt->PdoFlags.DeviceReservedUxd){
        //
        // Device is currently allocated for a virtual machine
        //
        internalConfigInfo->Flags.UxdEnabled = 1;
        RtlCopyMemory(&internalConfigInfo->UxdSettings,
                      &pdoExt->UxdSettings,
                      sizeof(USB_HUB_DEVICE_UXD_SETTINGS));
    }

    //
    // Copy over the PNP strings
    //

    do{

        nts = UsbhCopyPnpString(&internalConfigInfo->CompatibleIds,
                                &pdoExt->PnpCompatibleId);
        if(!NT_SUCCESS(nts)){
            break;
        }

        nts = UsbhCopyPnpString(&internalConfigInfo->HardwareIds,
                                &pdoExt->PnpHardwareId);
        if(!NT_SUCCESS(nts)){
            UsbhFreeID(&internalConfigInfo->CompatibleIds);
            break;
        }

        nts = UsbhCopyPnpString(&internalConfigInfo->DeviceDescription,
                                &pdoExt->FriendlyNameIdString);

        if(!NT_SUCCESS(nts)){
            UsbhFreeID(&internalConfigInfo->CompatibleIds);
            UsbhFreeID(&internalConfigInfo->HardwareIds);
            break;
        }

    } WHILE (0);

UsbhFdoReturnDeviceConfigInfoDone:

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts; 
}


NTSTATUS
UsbhFdoHandleGetDumpDataIoctl(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:


Arguments:

IRQL: Any

Return Value:

    nt status code


--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_DUMP_DEVICE_INFO deviceInfo;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PIO_STACK_LOCATION ioStack;
    PVOID deviceHandle[2];

    //
    // Verify the ioctl parameters from the usbstor driver.
    //
    ioStack = IoGetCurrentIrpStackLocation(Irp);

    if (ioStack->Parameters.DeviceIoControl.InputBufferLength < 
        sizeof(USB_DUMP_DEVICE_INFO))
    {
        nts = STATUS_BUFFER_TOO_SMALL;
        goto UsbhFdoHandleGetDumpDataIoctlDone;
    } 

    deviceInfo = Irp->AssociatedIrp.SystemBuffer;

    deviceHandle[0] = pdoExt->DeviceHandle;
    deviceHandle[1] = UsbhGetDeviceHandle(HubFdo, HubFdo);
    if (deviceHandle[1] == NULL) {
        nts = STATUS_UNSUCCESSFUL;
        goto UsbhFdoHandleGetDumpDataIoctlDone;
    }

    deviceInfo->DeviceHandle = deviceHandle;

    hubExt = FdoExt(HubFdo);

    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfBusStack, Irp);

    Usbh_HubDerefDeviceHandle(HubFdo, deviceHandle[1], HubFdo, Devh_ReturnDH_Tag);

    UsbhDecPdoIoCount(Pdo, Irp);
    return nts;

UsbhFdoHandleGetDumpDataIoctlDone:

    Irp->IoStatus.Status = nts;
    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);
    return nts;
}

NTSTATUS
UsbhFdoHandleFreeDumpDataIoctl(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:


Arguments:

IRQL: Any

Return Value:

    nt status code


--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}

NTSTATUS
UsbhFdoHandleNotifyForwardProgress(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:


Arguments:

IRQL: Any

Return Value:

    nt status code


--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PUSB_FORWARD_PROGRESS_INFO_V1 fpInfo;
    PUSB_DEVICE_HANDLE dh;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    fpInfo = ioStack->Parameters.Others.Argument1;


    if (fpInfo->Header.Version == USB_FORWARD_PROGRESS_INFO_VERSION_1&&
        fpInfo->Header.Size >= sizeof(USB_FORWARD_PROGRESS_INFO_V1)){
          
        // Ref the device handle to insure it is still valid and prevent it from
        // going away before the request completes
        dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Pdo, Devh_ForwardProgress_Tag);
    
        if (dh != NULL) {        
            // Valid request, fill in the device handle
            fpInfo->DeviceHandle = dh;
            IoSkipCurrentIrpStackLocation(Irp);
            nts = IoCallDriver(FdoExt(HubFdo)->TopOfBusStack, Irp);
            UsbhDerefPdoDeviceHandle(HubFdo, dh, Pdo, Devh_ForwardProgress_Tag);
        } else {
            nts = STATUS_NO_SUCH_DEVICE;
            UsbhCompleteIrp(Irp, nts);
        }
    } else {
        // Unrecognized request
        nts = STATUS_INVALID_DEVICE_REQUEST;
        UsbhCompleteIrp(Irp, nts);
    }
    
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


