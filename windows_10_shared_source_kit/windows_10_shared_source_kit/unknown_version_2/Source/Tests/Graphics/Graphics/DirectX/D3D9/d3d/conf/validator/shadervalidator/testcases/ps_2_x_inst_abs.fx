do);
    }

    KeReleaseSpinLock(&pdoExt->GlobalListStateLock, irql);

    // perform the actual delete
    IoDeleteDevice(Pdo);
}



USBD_CHILD_STATUS
UsbhInsertDeviceIntoGlobalList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Detect duplicate serial number devices and replicants.  Replicants are ghost devices that are
    created when a device is quicly moved from one port to another.

Arguments:

    Pdo - current device Pdo that  may have a duplicate serial number or replicant lurking
        in the system, Pdo must be latched.

Return Value:

    returns the type of duplicate device found or NotADuplicate.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, !IsListEmpty(&pdoExt->LatchListHead) ||
                       pdoExt->LatchCountNoRes);

    if (!pdoExt->PdoFlags.SerialNumber) {
        return USBD_CHILD_STATUS_INSERTED;
    }

    return USBD_AddDeviceToGlobalList(Pdo,
                                      HubFdo,
                                      pdoExt->PortNumber,
                                      NULL,
                                      pdoExt->DeviceDescriptor.idVendor,
                                      pdoExt->DeviceDescriptor.idProduct,
                                      &pdoExt->SerialNumberId);
}


PUSB_DEVICE_HANDLE
UsbhRefPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    Attempt to acquire the device handle for use by an API or other silliness.

    This routine will reference the 'current' handle linked to the PDO.

Arguments:


    Pdo - latched Pdo

    Tag - tagged associated with this referance.

    Object - object to associate with this ref, usually an irp

Return Value:

    NULL if the handle cannot be referenced or is invalid otherwise
    the USB device handle is returned

--*/
{
    PUSB_DEVICE_HANDLE dh;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    KIRQL irql;

    // we always perform this in the pnp context for the hub.
    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->DevHandleSpin, &irql);

    pdoExt = PdoExt(Pdo);
    LOG(HubFdo, LOG_PDO, 'DV+1', Pdo, pdoExt->DeviceHandleState);

    switch (pdoExt->DeviceHandleState) {
    case PdoDevH_Init:
    case PdoDevH_Invalid:
        LOG(HubFdo, LOG_PDO, 'DV+!', Pdo, pdoExt->DeviceHandleState);

        dh = NULL;
        break;

    case PdoDevH_Valid:

        dh = pdoExt->DeviceHandle;
        LOG(HubFdo, LOG_PDO, 'DV++', Tag, dh);

        // this would be a bug since we always initialize the field
        UsbhAssert(HubFdo, dh != NULL);
        UsbhAssert(HubFdo, dh != UsbhInvalidDeviceHandle);

        nts = Usbh_HubRefDeviceHandle(HubFdo,
                                      dh,
                                      Object,
                                      Tag);
        if (NT_SUCCESS(nts)) {
            dh = pdoExt->DeviceHandle;
        } else {
            dh = NULL;
        }
        LOG(HubFdo, LOG_PDO, 'DV+2', nts, dh);

        break;

    default:
        UsbhAssertFailure(HubFdo, Object);
        dh = NULL;
    }

    KeReleaseSpinLock(&hubExt->DevHandleSpin, irql);

    return dh;

}


VOID
UsbhDerefPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DevHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    Release a previosly acquired device handle.  It may or may not be still attached
    to the PDO.  If this is the last reference the dh will be deleted by USBPORT.

    This routine does not change the link state of the devhandle and the PDO.

Arguments:


    Tag - tag from the original ref call

    Object - object to associate with this ref

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    // we always perform this in the pnp context for the hub.
    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->DevHandleSpin, &irql);

    UsbhAssert(HubFdo, DevHandle != NULL);

    LOG(HubFdo, LOG_PDO, 'DV-1', DevHandle, Object);
    LOG(HubFdo, LOG_PDO, 'DV-2', DevHandle, Tag);
    Usbh_HubDerefDeviceHandle(HubFdo,
                              DevHandle,
                              Object,
                              Tag);

    KeReleaseSpinLock(&hubExt->DevHandleSpin, irql);


}


NTSTATUS
UsbhLinkPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEVICE_HANDLE DevHandle
    )
/*++

Routine Description:

    Links an allocated device handle to a PDO. This routine acquires
    an additional ref that is associated with linkage to the PDO.
    This in addition to the one that is implied with create.

Arguments:

     Pdo - latched Pdo

Return Value:

    ntstatus STATUS_SUCCESS if we could ref the handle.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    KeAcquireSpinLock(&hubExt->DevHandleSpin, &irql);

    UsbhAssert(HubFdo, DevHandle != UsbhInvalidDeviceHandle);
    UsbhAssert(HubFdo, DevHandle != NULL);

    LOG(HubFdo, LOG_PDO, 'lnPD', DevHandle, Pdo);
    nts = Usbh_HubRefDeviceHandle(HubFdo,
                                  DevHandle,
                                  Pdo,
                                  Devh_Pdo_Tag);

    LOG(HubFdo, LOG_PDO, 'lnP1', nts, Pdo);
    if (NT_SUCCESS(nts)) {
        pdoExt->DeviceHandleState = PdoDevH_Valid;
        pdoExt->DeviceHandle = DevHandle;
    }

    KeReleaseSpinLock(&hubExt->DevHandleSpin, irql);

    return nts;

}


VOID
UsbhUnlinkPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG LogTag,
    BOOLEAN RemoveHandle
    )
/*++

Routine Description:

    Removes the linkage of the device handle to the PDO and
    derefs the device handle.

    Also deletes the device handle thru USBPORT

Arguments:

     Pdo - latched Pdo con

     RemoveHandle - indicates we should call UsbdRemove.  The usbdinitdevice api removes
            the handle on failure so we don't call remove directly in that case.

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_DEVICE_HANDLE dh = NULL;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    KeAcquireSpinLock(&hubExt->DevHandleSpin, &irql);

    pdoExt = PdoExt(Pdo);

    LOG(HubFdo, LOG_PDO, 'ulPD', 0, 0);
    LOG(HubFdo, LOG_PDO, LogTag, Pdo, 0);

    switch (pdoExt->DeviceHandleState) {
    case PdoDevH_Init:
    case PdoDevH_Invalid:
        LOG(HubFdo, LOG_PDO, 'ulP1', dh, pdoExt->DeviceHandleState);
        break;

    case PdoDevH_Valid:

        if (pdoExt->PdoFlags.ResumeTimeSet == 1) {
            UsbhBusIf_SetClearRootPortResumeTime(HubFdo,
                                                 pdoExt->DeviceHandle,
                                                 FALSE,
                                                 0);
            pdoExt->PdoFlags.ResumeTimeSet = 0;
        }

        pdoExt->DeviceHandleState = PdoDevH_Invalid;
        dh = pdoExt->DeviceHandle;
        pdoExt->DeviceHandle = UsbhInvalidDeviceHandle;

        LOG(HubFdo, LOG_PDO, 'ulP2', 0, dh);

        Usbh_HubDerefDeviceHandle(HubFdo,
                                  dh,
                                  Pdo,
                                  Devh_Pdo_Tag);

        break;

    default:
        UsbhAssertFailure(HubFdo, Pdo);

    }


    KeReleaseSpinLock(&hubExt->DevHandleSpin, irql);

    if (dh != NULL && RemoveHandle) {
        UsbhAssert(HubFdo, dh != UsbhInvalidDeviceHandle);
        Usbh_HubRemoveUsbDevice(HubFdo, dh, 0);
    }

}


VOID
UsbhDeleteOrphanPdo(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Deletes a PDO where the parent hub has already been removed.

    Orphaned PDOs have already gone through surprise removal, so most cleanup has already occurred.

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    HUB_PDO_HWPNP_STATE curState;

    pdoExt = PdoExt(Pdo);

    curState = GET_PDO_HWPNPSTATE(Pdo);

    // this routine should only be called once
    if (curState == Pdo_Deleted) {
        // double delete?
        UsbhAssertFailure(NULL, Pdo);
        return;
    }

    SET_PDO_HWPNPSTATE(Pdo, Pdo_Deleted,  PdoEv_PdoRemove);

    WAIT_EVENT(&pdoExt->LatchEvent);

    UsbhFreePool(pdoExt->DeviceUsbRegPath);
    UsbhFreePool(pdoExt->PnPDeviceDescription);
    UsbhFreePool(pdoExt->MsOs20Info.DescriptorSet);
    UsbhFreePool(pdoExt->CachedConfigDescriptor);
    UsbhFreePool(pdoExt->BosDescriptor);
    UsbhFreePool(pdoExt->MsExtConfigDesc);
    UsbhFreePool(pdoExt->FailData);

    if (pdoExt->CleanupFlags.WaitPdoIo) {
        // wait for any outstanding IO on this PDO
        UsbhDecPdoIoCount(Pdo, Pdo);
        WAIT_EVENT(&pdoExt->IoRemoveEvent);
    }

    pdoExt->CleanupFlags.ul = 0;
    pdoExt->CleanupFlags.IoDeleted = 1;
    UsbhRemoveAndDeletePdo(Pdo);
}

BOOLEAN
UsbhPdo_IsDeviceExternal(
    PDEVICE_OBJECT Pdo
)
{
    DEVPROP_BOOLEAN isInLocalMachineContainer;
    ULONG size;
    NTSTATUS nts;
    DEVPROPTYPE type;
    BOOLEAN isDeviceExternal;

    PAGED_CODE();

    size = 0;
    type = DEVPROP_TYPE_EMPTY;
    isDeviceExternal = TRUE;

    // Retrieve the device removable property
    //
    nts = IoGetDevicePropertyData(Pdo,
                                  &DEVPKEY_Device_InLocalMachineContainer,
                                  0,
                                  0,
                                  sizeof(isInLocalMachineContainer),
                                  &isInLocalMachineContainer,
                                  &size,
                                  &type);

    if (NT_SUCCESS(nts)) {
        isDeviceExternal = (isInLocalMachineContainer == 0);
    }

    return isDeviceExternal;

}


VOID
UsbhQueryD3ColdSupport(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

Arguments:

Return Value:

    Determines if D3Cold is supported for the specified PDO by querying the D3Cold bus interface

--*/
{
    PIO_STACK_LOCATION ioStack;
    PIRP irp;
    NTSTATUS nts;
    KEVENT event;
    PDEVICE_EXTENSION_PDO pdoExt;
    D3COLD_SUPPORT_INTERFACE d3ColdInterface;
    PDEVICE_OBJECT topOfPdoDeviceStack;
    BOOLEAN d3ColdSupported;

    pdoExt = PdoExt(Pdo);
    irp = NULL;
    d3ColdSupported = FALSE;
    RtlZeroMemory(&d3ColdInterface, sizeof(d3ColdInterface));

    topOfPdoDeviceStack = IoGetAttachedDeviceReference(Pdo);

    if (topOfPdoDeviceStack == Pdo) {
        // If nothing is attached to the PDO there is no ACPI D3Cold support
        goto UsbhQueryD3ColdSupport_Exit;
    }

    irp = IoAllocateIrp(topOfPdoDeviceStack->StackSize, FALSE);

    if (!irp) {
        goto UsbhQueryD3ColdSupport_Exit;
    }

    // All PnP IRP's need the Status field initialized to STATUS_NOT_SUPPORTED.
    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    UsbhSetCompletionRoutine(HubFdo,
                             irp,
                             UsbhDeferIrpCompletion,
                             &event,
                             TRUE,
                             TRUE,
                             TRUE);

    ioStack = IoGetNextIrpStackLocation(irp);
    UsbhAssert(HubFdo, ioStack);
    ioStack->MajorFunction= IRP_MJ_PNP;
    ioStack->MinorFunction= IRP_MN_QUERY_INTERFACE;

    ioStack->Parameters.QueryInterface.Interface = (PINTERFACE)&d3ColdInterface;
    ioStack->Parameters.QueryInterface.InterfaceSpecificData = pdoExt;
    ioStack->Parameters.QueryInterface.InterfaceType = &GUID_D3COLD_SUPPORT_INTERFACE;
    ioStack->Parameters.QueryInterface.Size = sizeof(d3ColdInterface);
    ioStack->Parameters.QueryInterface.Version = D3COLD_SUPPORT_INTERFACE_VERSION;

    nts = IoCallDriver(topOfPdoDeviceStack, irp);

    if (nts == STATUS_PENDING) {

        KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);

        nts = irp->IoStatus.Status;
    }

    if (NT_SUCCESS(nts)) {
        // we have a D3Cold bus interface

        if (d3ColdInterface.GetD3ColdCapability) {
            nts = d3ColdInterface.GetD3ColdCapability(d3ColdInterface.Context,
                                                      &d3ColdSupported);

            if (NT_SUCCESS(nts) && d3ColdSupported) {
                pdoExt->PdoFlags.D3ColdSupportedInAcpi = 1;
            }
        }

        if (d3ColdInterface.InterfaceDereference) {
            d3ColdInterface.InterfaceDereference(d3ColdInterface.Context);
        }
    }

UsbhQueryD3ColdSupport_Exit:

    if (irp) {
        IoFreeIrp(irp);
    }

    ObDereferenceObject(topOfPdoDeviceStack);

    LOG(HubFdo, LOG_PDO, 'Gbsi', Pdo, d3ColdSupported);
}


NTSTATUS
UsbhPdoPnp_DeviceEnumerated(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handler for IRP_MN_DEVICE_ENUMERATED.  Used to start the driver not found timer for the PDO.

Arguments:

Return Value:

    STATUS_SUCCESS

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    UsbhQueryD3ColdSupport(pdoExt->HubFdo, Pdo);

    UsbhSetSqmEnumerationData(Pdo);

    UsbhEnableTimerObject(pdoExt->HubFdo,
                          &pdoExt->DriverNotFoundTimer,
                          DRIVER_NOT_FOUND_TIMEOUT,
                          pdoExt->PortNumber,
                          pdoExt->Pdo,
                          SIG_DNF_WORKER,
                          0);

    if (pdoExt->BillboardDescriptor != NULL) {
        UsbhPublishBillboardDetails(Pdo);
    }

    if (pdoExt->DeviceFlags.DualRoleDevice) {
        UsbhPublishDualRoleFeaturesProperty(Pdo);
    }

    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


VOID
UsbhPdo_ReportPnPFailureProblem(
    PDEVICE_OBJECT Pdo
    )
 /*++

Routine Description:

    Set the problem description string if we had an enumeration failure.

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    WCHAR * problemDescString;
    PMESSAGE_RESOURCE_ENTRY messageEntry;
    size_t messageStringSize;
    size_t problemDescStringSize;
    WCHAR driverPath[] = L"@System32\\drivers\\usbhub.sys";

    pdoExt = PdoExt(Pdo);
    problemDescString = NULL;

    do {

        if (pdoExt->EnumFailureMsgId == 0) {
            break;
        }

        //
        // Set the enumeration failure message Id value as the enumeration
        // failure code in the device's hardware key.
        //
        UsbhSetPdoEnumerationFailureCodeKey(Pdo);

        //
        // Extract the problem description string from the message table.
        // This is the fallback string in case Device Manager can't extract
        // the string for the locale it is running under.
        //
        nts = RtlFindMessage(UsbhDriverObject->DriverStart,
                                11, // RT_MESSAGETABLE
                                LANG_NEUTRAL,
                                pdoExt->EnumFailureMsgId,
                                &messageEntry);

        if (!NT_SUCCESS(nts)) {
            NT_ASSERTMSG("Unable to extract problem description from message table", 0);
            break;
        }

        //
        // Get the length of the extracted message string
        //
        nts = RtlUnalignedStringCbLengthW((PWSTR)messageEntry->Text,
                                            messageEntry->Length -
                                            FIELD_OFFSET(MESSAGE_RESOURCE_ENTRY,
                                                            Text),
                                            &messageStringSize);

        if (!NT_SUCCESS(nts)) {
            break;
        }

        //
        // Compute the size of the indirect string we will build.  It will have the following
        // format:
        //
        //     @System32\\drivers\\usbhub.sys,#<MsgId>;<Fallback String>
        //
        // Where <MsgId> is the ULONG message ID in decimal for the message string (max 10 characters)
        // and <Fallback String> is the string extracted from the message table.
        //
        problemDescStringSize = sizeof(driverPath) +  // "@System32\drivers\usbhub.sys"
                                13 * sizeof(WCHAR) +  // ",<MsgId>;"
                                messageStringSize +   // <Fallback String>
                                sizeof(WCHAR);        // terminating NULL

        UsbhAllocatePool_Z(problemDescString, NonPagedPool, problemDescStringSize);

        if (!problemDescString) {
            break;
        }

        nts = RtlStringCbPrintfW(problemDescString,
                                 problemDescStringSize,
                                 L"%s,#%d;%hs",
                                 driverPath,
                                 pdoExt->EnumFailureMsgId,
                                 messageEntry->Text);

        if (!NT_SUCCESS(nts)) {
            break;
        }

        nts = RtlUnalignedStringCbLengthW(problemDescString,
                                            problemDescStringSize,
                                            &problemDescStringSize);

        if (!NT_SUCCESS(nts)) {
            break;
        }

        //
        // Set the problem description string for Device Manager
        //
        IoSetDevicePropertyData(Pdo,
                                &DEVPKEY_Device_DriverProblemDesc,
                                LOCALE_NEUTRAL,
                                0,
                                DEVPROP_TYPE_STRING_INDIRECT,
                                (ULONG)(problemDescStringSize+sizeof(WCHAR)),
                                problemDescString);

    } WHILE(0);

    if (problemDescString) {
        UsbhFreePool(problemDescString);
    }
}

VOID
UsbhPdo_ResetDriverProblemDesc(
    PDEVICE_OBJECT Pdo
    )
 /*++

Routine Description:

    Set the driver problem description string to NULL.

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    IoSetDevicePropertyData(Pdo,
                            &DEVPKEY_Device_DriverProblemDesc,
                            LOCALE_NEUTRAL,
                            0,
                            DEVPROP_TYPE_EMPTY,
                            0,
                            NULL);

    pdoExt->EnumFailureMsgId = 0;
}


__drv_functionClass(SET_D3COLD_SUPPORT)
_IRQL_requires_(PASSIVE_LEVEL)
VOID
UsbhD3ColdSupportInterfaceSetD3ColdSupport (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    __in BOOLEAN D3ColdSupport
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
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // Let filters know that this is changing.
    //

    if (pdoExt->D3ColdFilter.SetD3ColdSupport != NULL) {
        pdoExt->D3ColdFilter.SetD3ColdSupport(pdoExt->D3ColdFilter.Context,
                                              D3ColdSupport);
    }

    //
    // Now change the internal state
    //
    if (D3ColdSupport) {
        pdoExt->PdoFlags.D3ColdEnabled = 1;
    } else {
        pdoExt->PdoFlags.D3ColdEnabled = 0;
    }
}

VOID
UsbhD3ColdSupportInterfaceReference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // We are not interested in this function. Just call the filter
    //

    if (pdoExt->D3ColdFilter.InterfaceReference != NULL) {
        pdoExt->D3ColdFilter.InterfaceReference(pdoExt->D3ColdFilter.Context);
    }
}

VOID
UsbhD3ColdSupportInterfaceDereference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:


Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // We are not interested in this function. Just call the filter
    //

    if (pdoExt->D3ColdFilter.InterfaceDereference != NULL) {
        pdoExt->D3ColdFilter.InterfaceDereference(pdoExt->D3ColdFilter.Context);
    }
}

__drv_functionClass(GET_IDLE_WAKE_INFO)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
UsbhD3ColdSupportInterfaceGetIdleWakeInfo (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    __in SYSTEM_POWER_STATE SystemPowerState,
    __out PDEVICE_WAKE_DEPTH DeepestWakeableDstate
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
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS status;

    PAGED_CODE();

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // First check if ACPI has any information. If so, then listen to it.
    //
    if (pdoExt->D3ColdFilter.GetIdleWakeInfo != NULL) {
        status = pdoExt->D3ColdFilter.GetIdleWakeInfo(pdoExt->D3ColdFilter.Context,
                                                      SystemPowerState,
                                                      DeepestWakeableDstate);
        if (NT_SUCCESS(status)) {
            return status;
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
    if ((pdoExt->SystemWake >= SystemPowerState) &&
        ((pdoExt->PdoFlags.UsbWakeupSupported == 1) ||
         (pdoExt->PdoFlags.DeviceIsHub == 1))) {

        *DeepestWakeableDstate = DeviceWakeDepthD2;
    } else {
        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
    }

    return STATUS_SUCCESS;
}


__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
UsbhD3ColdSupportInterfaceGetD3ColdCapability (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    __out PBOOLEAN D3ColdSupported
    )
/*++

Routine Description:

    This routine allows a driver to ask if there is a possibility
    that device might enter D3Cold

Arguments:

    D3ColdSupported - Out parameter indicating whether this device might enter D3cold.

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS status;

    PAGED_CODE();

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // If ACPI is present, we will let ACPI make the call
    //
    if (pdoExt->D3ColdFilter.GetD3ColdCapability != NULL) {
        status = pdoExt->D3ColdFilter.GetD3ColdCapability(pdoExt->D3ColdFilter.Context,
                                                          D3ColdSupported);
        if (NT_SUCCESS(status) && (*D3ColdSupported)) {
            NT_ASSERTMSG("ACPI returned true for GetD3ColdCapability for a removable device",
                         pdoExt->PortAttributes.DeviceNotRemovable == 1);
        }

        return status;
    }

    //
    // If there is no ACPI, then we don't support D3cold
    //
    *D3ColdSupported = FALSE;
    return STATUS_SUCCESS;
}


__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
UsbhD3ColdSupportInterfaceGetD3ColdBusDriverSupport (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    __out PBOOLEAN D3ColdSupported
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

    D3ColdSupported - Out parameter indicating whether this device might enter D3cold.

Return Value:


--*/
{

    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    //
    // Devices that can be removed should not be sent to D3 because
    // we cannot differentiate between device going to D3Cold Vs.
    // device getting removed
    //
    if (pdoExt->PortAttributes.DeviceNotRemovable == 0) {
        *D3ColdSupported = FALSE;
    } else {
        *D3ColdSupported = TRUE;
    }

    return STATUS_SUCCESS;
}


__drv_functionClass(GET_D3COLD_LAST_TRANSITION_STATUS)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
UsbhD3ColdSupportInterfaceGetLastTransitionStatus (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    __out PD3COLD_LAST_TRANSITION_STATUS LastTransitionStatus
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
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = (PDEVICE_EXTENSION_PDO)Context;

    *LastTransitionStatus = LastDStateTransitionD3hot;

    //
    // First check if ACPI has any information
    //
    if (pdoExt->D3ColdFilter.GetLastTransitionStatus != NULL) {
        pdoExt->D3ColdFilter.GetLastTransitionStatus(pdoExt->D3ColdFilter.Context,
                                                     LastTransitionStatus);

        if (*LastTransitionStatus == LastDStateTransitionD3cold) {

            NT_ASSERTMSG("ACPI thinks that the device went to D3Cold but USB state indicates that device was not re-enumerated",
                         pdoExt->PdoFlags.ResetOnLastResume == 1);
            return;
        }
    }

    //
    // Even if the device did not really go to D3 cold but
    // got re-enumerated, then we will return D3 cold because
    // the device state was lost, which is what the client
    // driver is interested in.
    //
    if (pdoExt->PdoFlags.ResetOnLastResume == 1) {
        *LastTransitionStatus = LastDStateTransitionD3cold;
    }
}


VOID
UsbhPublishBillboardDetails (
    _In_ PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Publish details of the Billboard so that interested parties may
    consume it.

    Failures are not fatal.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR billboardDesc;
    PUSB_BILLBOARD_INFORMATION billboardInfo;
    ULONG billboardInfoSize;
    ULONG amIndex;
    RTL_BITMAP bmConfigured;
    ULONG amState;
    PUSB_BILLBOARD_ALTERNATE_MODE altMode;

    pdoExt = PdoExt(Pdo);
    billboardInfo = NULL;

    billboardDesc = pdoExt->BillboardDescriptor;
    NT_ASSERT(billboardDesc != NULL);

    billboardInfoSize = USB_BILLBOARD_INFORMATION_SIZE(billboardDesc->bNumberOfAlternateModes);

    UsbhAllocatePool_Z(billboardInfo, PagedPool, billboardInfoSize);
    if (billboardInfo == NULL) {
        LOG(pdoExt->HubFdo, LOG_PDO, 'pbd1', billboardInfoSize, Pdo);
        goto UsbhPublishBillboardDetails_Exit;
    }

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

    nts = IoSetDevicePropertyData(Pdo,
                                  &DEVPKEY_Device_UsbBillboardInfo,
                                  LOCALE_NEUTRAL,
                                  0,
                                  DEVPROP_TYPE_BINARY,
                                  billboardInfoSize,
                                  billboardInfo);

    if (!NT_SUCCESS(nts)) {
        LOG(pdoExt->HubFdo, LOG_PDO, 'pdb2', nts, Pdo);
        goto UsbhPublishBillboardDetails_Exit;
    }

    nts = WnfPublishBillboardChange();
    if (!NT_SUCCESS(nts)) {
        LOG(pdoExt->HubFdo, LOG_PDO, 'pdb3', nts, Pdo);
    }

UsbhPublishBillboardDetails_Exit:

    if (billboardInfo != NULL) {
        UsbhFreePool(billboardInfo);
    }
}


VOID
UsbhBillboardCleanup (
    _In_ PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Cleanup any Billboard device state.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    pdoExt = PdoExt(Pdo);
    NT_ASSERT(pdoExt->BillboardDescriptor != NULL);

    nts = IoSetDevicePropertyData(Pdo,
                                  &DEVPKEY_Device_UsbBillboardInfo,
                                  LOCALE_NEUTRAL,
                                  0,
                                  DEVPROP_TYPE_BINARY,
                                  0,
                                  NULL);
    NT_VERIFY(NT_SUCCESS(nts));

    //
    // Publish the event even if we failed to delete the property. The devnode
    // is going to go away, and user-mode should be filtering out non-present
    // devnodes anyways.
    //
    nts = WnfPublishBillboardChange();

    NT_VERIFY(NT_SUCCESS(nts));

    pdoExt->BillboardDescriptor = NULL;
}

VOID
UsbhPublishDualRoleFeaturesProperty(
    _In_ PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Publish the USB Dual Features Device property.

    Failures are not fatal.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG usbDualRoleFeatures;

    pdoExt = PdoExt(Pdo);

    usbDualRoleFeatures = UsbDualRoleFeaturesGetUlong(&pdoExt->UsbDualRoleFeatures);

    nts = IoSetDevicePropertyData(Pdo,
                                  &DEVPKEY_Device_UsbDualRoleFeatures,
                                  LOCALE_NEUTRAL,
                                  0,
                                  DEVPROP_TYPE_UINT32,
                                  sizeof(usbDualRoleFeatures),
                                  &usbDualRoleFeatures);

    if (!NT_SUCCESS(nts)) {
        LOG(pdoExt->HubFdo, LOG_PDO, 'drp1', nts, Pdo);
    }
}#include "Test_Include.fx"

int Test_Count = 351;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_x_inst_abs";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_x : abs source reg combination v0 is allowed";
string PS_001 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_x : abs source reg combination r0 is allowed";
string PS_002 = 
	"ps_2_x "
	"mov r0, c0 "
	"abs r0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_x : abs source reg combination c0 is allowed";
string PS_003 = 
	"ps_2_x "
	"def c0, 1, 1, 1, 1 "
	"abs r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_x : abs source reg combination t0 is allowed";
string PS_004 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_x : abs source reg combination -t0 is allowed";
string PS_005 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, -t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_x : abs source reg combination t0 is allowed";
string PS_006 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_x : abs source reg combination t0.r is allowed";
string PS_007 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_x : abs source reg combination t0.g is allowed";
string PS_008 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_x : abs source reg combination t0.b is allowed";
string PS_009 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_x : abs source reg combination t0.a is allowed";
string PS_010 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_x : abs source reg combination t0.gbra is allowed";
string PS_011 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_x : abs source reg combination t0.brga is allowed";
string PS_012 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_x : abs source reg combination t0.abgr is allowed";
string PS_013 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_014_Desc = "ps_2_x : abs source reg combination t0.agrb is allowed";
string PS_014 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_015_Desc = "ps_2_x : abs source reg combination t0.rbga is allowed";
string PS_015 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_016_Desc = "ps_2_x : abs source reg combination t0.rgab is allowed";
string PS_016 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_017_Desc = "ps_2_x : abs source reg combination t0.bgr is allowed";
string PS_017 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_018_Desc = "ps_2_x : abs source reg combination t0.rbg is allowed";
string PS_018 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_019_Desc = "ps_2_x : abs source reg combination t0.gar is allowed";
string PS_019 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_020_Desc = "ps_2_x : abs source reg combination t0.gr is allowed";
string PS_020 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_021_Desc = "ps_2_x : abs source reg combination t0.ab is allowed";
string PS_021 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_022_Desc = "ps_2_x : abs source reg combination t0.bg is allowed";
string PS_022 = 
	"ps_2_x "
	"dcl t0 "
	"abs r0, t0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_x : abs source reg combination b0 is NOT allowed";
string PS_023 = 
	"ps_2_x "
	"defb b0, true "
	"abs r0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_x : abs source reg combination i0 is NOT allowed";
string PS_024 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"abs r0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_025_Desc = "ps_2_x : abs source reg combination p0 is NOT allowed";
string PS_025 = 
	"ps_2_x "
	"abs r0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_x : abs source reg combination s0 is NOT allowed";
string PS_026 = 
	"ps_2_x "
	"dcl_volume s0 "
	"abs r0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_x : abs source reg combination oC0 is NOT allowed";
string PS_027 = 
	"ps_2_x "
	"abs r0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_x : abs source reg combination oDepth is NOT allowed";
string PS_028 = 
	"ps_2_x "
	"abs r0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_x : abs dest r# is allowed";
string PS_029 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_x : abs _pp dest r# is allowed";
string PS_030 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_x : abs _sat dest r# is allowed";
string PS_031 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_x : abs dest v# is NOT allowed";
string PS_032 = 
	"ps_2_x "
	"dcl v0 "
	"abs v0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_x : abs _pp dest v# is NOT allowed";
string PS_033 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp v0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_x : abs _sat dest v# is NOT allowed";
string PS_034 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat v0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_x : abs dest c# is NOT allowed";
string PS_035 = 
	"ps_2_x "
	"dcl v0 "
	"abs c1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_x : abs _pp dest c# is NOT allowed";
string PS_036 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp c1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_x : abs _sat dest c# is NOT allowed";
string PS_037 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat c1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_x : abs dest b# is NOT allowed";
string PS_038 = 
	"ps_2_x "
	"dcl v0 "
	"abs b0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_x : abs _pp dest b# is NOT allowed";
string PS_039 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp b0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_x : abs _sat dest b# is NOT allowed";
string PS_040 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat b0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_x : abs dest i# is NOT allowed";
string PS_041 = 
	"ps_2_x "
	"dcl v0 "
	"abs i0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_x : abs _pp dest i# is NOT allowed";
string PS_042 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp i0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_x : abs _sat dest i# is NOT allowed";
string PS_043 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat i0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 9.109f };
string PS_044_Desc = "ps_2_x : abs dest p0 is NOT allowed";
string PS_044 = 
	"ps_2_x "
	"dcl v0 "
	"abs p0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 9.109f };
string PS_045_Desc = "ps_2_x : abs _pp dest p0 is NOT allowed";
string PS_045 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp p0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 9.109f };
string PS_046_Desc = "ps_2_x : abs _sat dest p0 is NOT allowed";
string PS_046 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat p0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_x : abs dest s# is NOT allowed";
string PS_047 = 
	"ps_2_x "
	"dcl v0 "
	"abs s0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_x : abs _pp dest s# is NOT allowed";
string PS_048 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp s0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_x : abs _sat dest s# is NOT allowed";
string PS_049 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat s0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_x : abs dest t# is NOT allowed";
string PS_050 = 
	"ps_2_x "
	"dcl v0 "
	"abs t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_x : abs _pp dest t# is NOT allowed";
string PS_051 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_x : abs _sat dest t# is NOT allowed";
string PS_052 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_x : abs dest oC# is NOT allowed";
string PS_053 = 
	"ps_2_x "
	"dcl v0 "
	"abs oC0, v0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_x : abs _pp dest oC# is NOT allowed";
string PS_054 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp oC0, v0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_x : abs _sat dest oC# is NOT allowed";
string PS_055 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat oC0, v0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_x : abs dest oDepth is NOT allowed";
string PS_056 = 
	"ps_2_x "
	"dcl v0 "
	"abs oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_x : abs _pp dest oDepth is NOT allowed";
string PS_057 = 
	"ps_2_x "
	"dcl v0 "
	"abs_pp oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_x : abs _sat dest oDepth is NOT allowed";
string PS_058 = 
	"ps_2_x "
	"dcl v0 "
	"abs_sat oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_x : abs write mask .r is allowed";
string PS_059 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.r, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_x : abs write mask .g is allowed";
string PS_060 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.g, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_x : abs write mask .b is allowed";
string PS_061 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.b, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_x : abs write mask .a is allowed";
string PS_062 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.a, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_x : abs write mask .rg is allowed";
string PS_063 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.rg, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_x : abs write mask .gb is allowed";
string PS_064 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.gb, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_x : abs write mask .ba is allowed";
string PS_065 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.ba, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_x : abs write mask .rb is allowed";
string PS_066 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.rb, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_x : abs write mask .ra is allowed";
string PS_067 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.ra, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_x : abs write mask .ga is allowed";
string PS_068 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.ga, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_x : abs write mask .rgb is allowed";
string PS_069 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.rgb, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_x : abs write mask .gba is allowed";
string PS_070 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.gba, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_x : abs write mask .rba is allowed";
string PS_071 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.rba, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_x : abs write mask .rgba is allowed";
string PS_072 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.rgba, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_x : abs write mask .yx is NOT allowed";
string PS_073 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.yx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_x : abs write mask .zx is NOT allowed";
string PS_074 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.zx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_x : abs write mask .zy is NOT allowed";
string PS_075 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.zy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_x : abs write mask .wx is NOT allowed";
string PS_076 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_x : abs write mask .wz is NOT allowed";
string PS_077 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wz, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_x : abs write mask .wy is NOT allowed";
string PS_078 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_x : abs write mask .zyx is NOT allowed";
string PS_079 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.zyx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_x : abs write mask .wzy is NOT allowed";
string PS_080 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wzy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_x : abs write mask .wzx is NOT allowed";
string PS_081 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wzx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_x : abs write mask .wyx is NOT allowed";
string PS_082 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wyx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_x : abs write mask .yxw is NOT allowed";
string PS_083 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.yxw, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_x : abs write mask .wzyx is NOT allowed";
string PS_084 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.wzyx, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_x : abs write mask .zxwy is NOT allowed";
string PS_085 = 
	"ps_2_x "
	"dcl v0 "
	"abs r0.zxwy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_x : abs is allowed in a 1 level if b0 block";
string PS_086 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_087_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_x : abs is allowed in a 8 level if b0 block";
string PS_087 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_088_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_x : abs is allowed in a 16 level if b0 block";
string PS_088 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_089_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_x : abs is allowed in a 24 level if b0 block";
string PS_089 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_090_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_x : abs is NOT allowed in a 25 level if b0 block";
string PS_090 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_x : abs is allowed in a 1 level if b0 nop else block";
string PS_091 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_x : abs is allowed in a 8 level if b0 nop else block";
string PS_092 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_093_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_x : abs is allowed in a 16 level if b0 nop else block";
string PS_093 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_x : abs is allowed in a 24 level if b0 nop else block";
string PS_094 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_x : abs is NOT allowed in a 25 level if b0 nop else block";
string PS_095 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_x : abs is NOT allowed in a 1 level loop aL, i0 block";
string PS_096 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	abs r0, v0 "
	"endloop ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_x : abs is NOT allowed in a 2 level loop aL, i0 block";
string PS_097 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_x : abs is NOT allowed in a 3 level loop aL, i0 block";
string PS_098 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			abs r0, v0 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_x : abs is NOT allowed in a 4 level loop aL, i0 block";
string PS_099 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				abs r0, v0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_x : abs is NOT allowed in a 5 level loop aL, i0 block";
string PS_100 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					abs r0, v0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_101_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_x : abs is allowed in a 1 level rep i0 block";
string PS_101 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	abs r0, v0 "
	"endrep ";

PS_CRITERIA PS_102_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_x : abs is allowed in a 2 level rep i0 block";
string PS_102 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_103_Criteria = { true, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_x : abs is allowed in a 3 level rep i0 block";
string PS_103 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			abs r0, v0 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_104_Criteria = { true, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_x : abs is allowed in a 4 level rep i0 block";
string PS_104 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				abs r0, v0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_2_x : abs is NOT allowed in a 5 level rep i0 block";
string PS_105 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					abs r0, v0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_106_Criteria = { true, false, 0x0200, 1, -1, 0, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_2_x : abs is allowed in a 1 level if_eq c0.x, c0.y block";
string PS_106 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_107_Criteria = { true, false, 0x0200, 8, -1, 0, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_2_x : abs is allowed in a 8 level if_eq c0.x, c0.y block";
string PS_107 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		if_eq c0.x, c0.y "
	"			if_eq c0.x, c0.y "
	"				if_eq c0.x, c0.y "
	"					if_eq c0.x, c0.y "
	"						if_eq c0.x, c0.y "
	"							if_eq c0.x, c0.y "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_108_Criteria = { true, false, 0x0200, 16, -1, 0, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_x : abs is allowed in a 16 level if_eq c0.x, c0.y block";
string PS_108 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		if_eq c0.x, c0.y "
	"			if_eq c0.x, c0.y "
	"				if_eq c0.x, c0.y "
	"					if_eq c0.x, c0.y "
	"						if_eq c0.x, c0.y "
	"							if_eq c0.x, c0.y "
	"								if_eq c0.x, c0.y "
	"									if_eq c0.x, c0.y "
	"										if_eq c0.x, c0.y "
	"											if_eq c0.x, c0.y "
	"												if_eq c0.x, c0.y "
	"													if_eq c0.x, c0.y "
	"														if_eq c0.x, c0.y "
	"															if_eq c0.x, c0.y "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_109_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_x : abs is allowed in a 24 level if_eq c0.x, c0.y block";
string PS_109 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		if_eq c0.x, c0.y "
	"			if_eq c0.x, c0.y "
	"				if_eq c0.x, c0.y "
	"					if_eq c0.x, c0.y "
	"						if_eq c0.x, c0.y "
	"							if_eq c0.x, c0.y "
	"								if_eq c0.x, c0.y "
	"									if_eq c0.x, c0.y "
	"										if_eq c0.x, c0.y "
	"											if_eq c0.x, c0.y "
	"												if_eq c0.x, c0.y "
	"													if_eq c0.x, c0.y "
	"														if_eq c0.x, c0.y "
	"															if_eq c0.x, c0.y "
	"																if_eq c0.x, c0.y "
	"																	if_eq c0.x, c0.y "
	"																		if_eq c0.x, c0.y "
	"																			if_eq c0.x, c0.y "
	"																				if_eq c0.x, c0.y "
	"																					if_eq c0.x, c0.y "
	"																						if_eq c0.x, c0.y "
	"																							if_eq c0.x, c0.y "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0200, 25, -1, 0, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_x : abs is NOT allowed in a 25 level if_eq c0.x, c0.y block";
string PS_110 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		if_eq c0.x, c0.y "
	"			if_eq c0.x, c0.y "
	"				if_eq c0.x, c0.y "
	"					if_eq c0.x, c0.y "
	"						if_eq c0.x, c0.y "
	"							if_eq c0.x, c0.y "
	"								if_eq c0.x, c0.y "
	"									if_eq c0.x, c0.y "
	"										if_eq c0.x, c0.y "
	"											if_eq c0.x, c0.y "
	"												if_eq c0.x, c0.y "
	"													if_eq c0.x, c0.y "
	"														if_eq c0.x, c0.y "
	"															if_eq c0.x, c0.y "
	"																if_eq c0.x, c0.y "
	"																	if_eq c0.x, c0.y "
	"																		if_eq c0.x, c0.y "
	"																			if_eq c0.x, c0.y "
	"																				if_eq c0.x, c0.y "
	"																					if_eq c0.x, c0.y "
	"																						if_eq c0.x, c0.y "
	"																							if_eq c0.x, c0.y "
	"																								if_eq c0.x, c0.y "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_111_Criteria = { true, false, 0x0200, 1, -1, 0, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_x : abs is allowed in a 1 level if_eq c0.x, c0.y nop else block";
string PS_111 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_112_Criteria = { true, false, 0x0200, 8, -1, 0, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_x : abs is allowed in a 8 level if_eq c0.x, c0.y nop else block";
string PS_112 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y nop else "
	"		if_eq c0.x, c0.y nop else "
	"			if_eq c0.x, c0.y nop else "
	"				if_eq c0.x, c0.y nop else "
	"					if_eq c0.x, c0.y nop else "
	"						if_eq c0.x, c0.y nop else "
	"							if_eq c0.x, c0.y nop else "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_113_Criteria = { true, false, 0x0200, 16, -1, 0, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_x : abs is allowed in a 16 level if_eq c0.x, c0.y nop else block";
string PS_113 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y nop else "
	"		if_eq c0.x, c0.y nop else "
	"			if_eq c0.x, c0.y nop else "
	"				if_eq c0.x, c0.y nop else "
	"					if_eq c0.x, c0.y nop else "
	"						if_eq c0.x, c0.y nop else "
	"							if_eq c0.x, c0.y nop else "
	"								if_eq c0.x, c0.y nop else "
	"									if_eq c0.x, c0.y nop else "
	"										if_eq c0.x, c0.y nop else "
	"											if_eq c0.x, c0.y nop else "
	"												if_eq c0.x, c0.y nop else "
	"													if_eq c0.x, c0.y nop else "
	"														if_eq c0.x, c0.y nop else "
	"															if_eq c0.x, c0.y nop else "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_114_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_x : abs is allowed in a 24 level if_eq c0.x, c0.y nop else block";
string PS_114 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y nop else "
	"		if_eq c0.x, c0.y nop else "
	"			if_eq c0.x, c0.y nop else "
	"				if_eq c0.x, c0.y nop else "
	"					if_eq c0.x, c0.y nop else "
	"						if_eq c0.x, c0.y nop else "
	"							if_eq c0.x, c0.y nop else "
	"								if_eq c0.x, c0.y nop else "
	"									if_eq c0.x, c0.y nop else "
	"										if_eq c0.x, c0.y nop else "
	"											if_eq c0.x, c0.y nop else "
	"												if_eq c0.x, c0.y nop else "
	"													if_eq c0.x, c0.y nop else "
	"														if_eq c0.x, c0.y nop else "
	"															if_eq c0.x, c0.y nop else "
	"																if_eq c0.x, c0.y nop else "
	"																	if_eq c0.x, c0.y nop else "
	"																		if_eq c0.x, c0.y nop else "
	"																			if_eq c0.x, c0.y nop else "
	"																				if_eq c0.x, c0.y nop else "
	"																					if_eq c0.x, c0.y nop else "
	"																						if_eq c0.x, c0.y nop else "
	"																							if_eq c0.x, c0.y nop else "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0200, 25, -1, 0, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_x : abs is NOT allowed in a 25 level if_eq c0.x, c0.y nop else block";
string PS_115 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y nop else "
	"		if_eq c0.x, c0.y nop else "
	"			if_eq c0.x, c0.y nop else "
	"				if_eq c0.x, c0.y nop else "
	"					if_eq c0.x, c0.y nop else "
	"						if_eq c0.x, c0.y nop else "
	"							if_eq c0.x, c0.y nop else "
	"								if_eq c0.x, c0.y nop else "
	"									if_eq c0.x, c0.y nop else "
	"										if_eq c0.x, c0.y nop else "
	"											if_eq c0.x, c0.y nop else "
	"												if_eq c0.x, c0.y nop else "
	"													if_eq c0.x, c0.y nop else "
	"														if_eq c0.x, c0.y nop else "
	"															if_eq c0.x, c0.y nop else "
	"																if_eq c0.x, c0.y nop else "
	"																	if_eq c0.x, c0.y nop else "
	"																		if_eq c0.x, c0.y nop else "
	"																			if_eq c0.x, c0.y nop else "
	"																				if_eq c0.x, c0.y nop else "
	"																					if_eq c0.x, c0.y nop else "
	"																						if_eq c0.x, c0.y nop else "
	"																							if_eq c0.x, c0.y nop else "
	"																								if_eq c0.x, c0.y nop else "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_116_Criteria = { true, false, 0x0200, 1, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_116_Desc = "ps_2_x : abs is allowed in a 1 level if p0.x block";
string PS_116 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_117_Criteria = { true, false, 0x0200, 8, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_117_Desc = "ps_2_x : abs is allowed in a 8 level if p0.x block";
string PS_117 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_118_Criteria = { true, false, 0x0200, 16, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_118_Desc = "ps_2_x : abs is allowed in a 16 level if p0.x block";
string PS_118 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_119_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_119_Desc = "ps_2_x : abs is allowed in a 24 level if p0.x block";
string PS_119 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_120_Criteria = { false, false, 0x0200, 25, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_120_Desc = "ps_2_x : abs is NOT allowed in a 25 level if p0.x block";
string PS_120 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_121_Criteria = { true, false, 0x0200, 1, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_121_Desc = "ps_2_x : abs is allowed in a 1 level if p0.x nop else block";
string PS_121 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	abs r0, v0 "
	"endif ";

PS_CRITERIA PS_122_Criteria = { true, false, 0x0200, 8, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_122_Desc = "ps_2_x : abs is allowed in a 8 level if p0.x nop else block";
string PS_122 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								abs r0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_123_Criteria = { true, false, 0x0200, 16, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_123_Desc = "ps_2_x : abs is allowed in a 16 level if p0.x nop else block";
string PS_123 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																abs r0, v0 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_124_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_124_Desc = "ps_2_x : abs is allowed in a 24 level if p0.x nop else block";
string PS_124 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								abs r0, v0 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_125_Criteria = { false, false, 0x0200, 25, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_125_Desc = "ps_2_x : abs is NOT allowed in a 25 level if p0.x nop else block";
string PS_125 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									abs r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_126_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_x : abs is NOT allowed in a 1 level loop aL, i0 break block";
string PS_126 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	abs r0, v0 "
	"endloop ";

PS_CRITERIA PS_127_Criteria = { false, false, 0x0200, 8, -1, 8, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_x : abs is NOT allowed in a 8 level loop aL, i0 break block";
string PS_127 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								abs r0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_128_Criteria = { false, false, 0x0200, 16, -1, 16, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_x : abs is NOT allowed in a 16 level loop aL, i0 break block";
string PS_128 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																abs r0, v0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_129_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_x : abs is NOT allowed in a 24 level loop aL, i0 break block";
string PS_129 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								abs r0, v0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_130_Criteria = { false, false, 0x0200, 25, -1, 25, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_x : abs is NOT allowed in a 25 level loop aL, i0 break block";
string PS_130 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									abs r0, v0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_131_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_x : abs is allowed in a 1 level rep i0 break block";
string PS_131 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	abs r0, v0 "
	"endrep ";

PS_CRITERIA PS_132_Criteria = { false, false, 0x0200, 8, -1, 8, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_x : abs is NOT allowed in a 8 level rep i0 break block";
string PS_132 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								abs r0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_133_Criteria = { false, false, 0x0200, 16, -1, 16, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_x : abs is NOT allowed in a 16 level rep i0 break block";
string PS_133 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																abs r0, v0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_134_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_x : abs is NOT allowed in a 24 level rep i0 break block";
string PS_134 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								abs r0, v0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_135_Criteria = { false, false, 0x0200, 25, -1, 25, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_x : abs is NOT allowed in a 25 level rep i0 break block";
string PS_135 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									abs r0, v0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_136_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_136_Desc = "ps_2_x : abs is NOT allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_136 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	abs r0, v0 "
	"endloop ";

PS_CRITERIA PS_137_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string PS_137_Desc = "ps_2_x : abs is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_137 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								abs r0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_138_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string PS_138_Desc = "ps_2_x : abs is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_138 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																abs r0, v0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_139_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string PS_139_Desc = "ps_2_x : abs is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_139 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								abs r0, v0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_140_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string PS_140_Desc = "ps_2_x : abs is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_140 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									abs r0, v0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_141_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_141_Desc = "ps_2_x : abs is allowed in a 1 level rep i0 breakp p0.x block";
string PS_141 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	abs r0, v0 "
	"endrep ";

PS_CRITERIA PS_142_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string PS_142_Desc = "ps_2_x : abs is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_142 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								abs r0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_143_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string PS_143_Desc = "ps_2_x : abs is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_143 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																abs r0, v0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_144_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string PS_144_Desc = "ps_2_x : abs is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_144 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								abs r0, v0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_145_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string PS_145_Desc = "ps_2_x : abs is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_145 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									abs r0, v0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_146_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_x : abs is NOT allowed in a 1 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_146 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	abs r0, v0 "
	"endloop ";

PS_CRITERIA PS_147_Criteria = { false, false, 0x0200, 8, -1, 8, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_2_x : abs is NOT allowed in a 8 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_147 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								abs r0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_148_Criteria = { false, false, 0x0200, 16, -1, 16, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_2_x : abs is NOT allowed in a 16 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_148 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																abs r0, v0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_149_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_2_x : abs is NOT allowed in a 24 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_149 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								abs r0, v0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_150_Criteria = { false, false, 0x0200, 25, -1, 25, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_2_x : abs is NOT allowed in a 25 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_150 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								loop aL, i0 break_eq c0.x, c0.y "
	"																									abs r0, v0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_151_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_2_x : abs is allowed in a 1 level rep i0 break_lt c0.x, c0.y block";
string PS_151 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	abs r0, v0 "
	"endrep ";

PS_CRITERIA PS_152_Criteria = { false, false, 0x0200, 8, -1, 8, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_2_x : abs is NOT allowed in a 8 level rep i0 break_lt c0.x, c0.y block";
string PS_152 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		rep i0 break_lt c0.x, c0.y "
	"			rep i0 break_lt c0.x, c0.y "
	"				rep i0 break_lt c0.x, c0.y "
	"					rep i0 break_lt c0.x, c0.y "
	"						rep i0 break_lt c0.x, c0.y "
	"							rep i0 break_lt c0.x, c0.y "
	"								abs r0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_153_Criteria = { false, false, 0x0200, 16, -1, 16, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_2_x : abs is NOT allowed in a 16 level rep i0 break_lt c0.x, c0.y block";
string PS_153 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		rep i0 break_lt c0.x, c0.y "
	"			rep i0 break_lt c0.x, c0.y "
	"				rep i0 break_lt c0.x, c0.y "
	"					rep i0 break_lt c0.x, c0.y "
	"						rep i0 break_lt c0.x, c0.y "
	"							rep i0 break_lt c0.x, c0.y "
	"								rep i0 break_lt c0.x, c0.y "
	"									rep i0 break_lt c0.x, c0.y "
	"										rep i0 break_lt c0.x, c0.y "
	"											rep i0 break_lt c0.x, c0.y "
	"												rep i0 break_lt c0.x, c0.y "
	"													rep i0 break_lt c0.x, c0.y "
	"														rep i0 break_lt c0.x, c0.y "
	"															rep i0 break_lt c0.x, c0.y "
	"																abs r0, v0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_154_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_2_x : abs is NOT allowed in a 24 level rep i0 break_lt c0.x, c0.y block";
string PS_154 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		rep i0 break_lt c0.x, c0.y "
	"			rep i0 break_lt c0.x, c0.y "
	"				rep i0 break_lt c0.x, c0.y "
	"					rep i0 break_lt c0.x, c0.y "
	"						rep i0 break_lt c0.x, c0.y "
	"							rep i0 break_lt c0.x, c0.y "
	"								rep i0 break_lt c0.x, c0.y "
	"									rep i0 break_lt c0.x, c0.y "
	"										rep i0 break_lt c0.x, c0.y "
	"											rep i0 break_lt c0.x, c0.y "
	"												rep i0 break_lt c0.x, c0.y "
	"													rep i0 break_lt c0.x, c0.y "
	"														rep i0 break_lt c0.x, c0.y "
	"															rep i0 break_lt c0.x, c0.y "
	"																rep i0 break_lt c0.x, c0.y "
	"																	rep i0 break_lt c0.x, c0.y "
	"																		rep i0 break_lt c0.x, c0.y "
	"																			rep i0 break_lt c0.x, c0.y "
	"																				rep i0 break_lt c0.x, c0.y "
	"																					rep i0 break_lt c0.x, c0.y "
	"																						rep i0 break_lt c0.x, c0.y "
	"																							rep i0 break_lt c0.x, c0.y "
	"																								abs r0, v0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_155_Criteria = { false, false, 0x0200, 25, -1, 25, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_2_x : abs is NOT allowed in a 25 level rep i0 break_lt c0.x, c0.y block";
string PS_155 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		rep i0 break_lt c0.x, c0.y "
	"			rep i0 break_lt c0.x, c0.y "
	"				rep i0 break_lt c0.x, c0.y "
	"					rep i0 break_lt c0.x, c0.y "
	"						rep i0 break_lt c0.x, c0.y "
	"							rep i0 break_lt c0.x, c0.y "
	"								rep i0 break_lt c0.x, c0.y "
	"									rep i0 break_lt c0.x, c0.y "
	"										rep i0 break_lt c0.x, c0.y "
	"											rep i0 break_lt c0.x, c0.y "
	"												rep i0 break_lt c0.x, c0.y "
	"													rep i0 break_lt c0.x, c0.y "
	"														rep i0 break_lt c0.x, c0.y "
	"															rep i0 break_lt c0.x, c0.y "
	"																rep i0 break_lt c0.x, c0.y "
	"																	rep i0 break_lt c0.x, c0.y "
	"																		rep i0 break_lt c0.x, c0.y "
	"																			rep i0 break_lt c0.x, c0.y "
	"																				rep i0 break_lt c0.x, c0.y "
	"																					rep i0 break_lt c0.x, c0.y "
	"																						rep i0 break_lt c0.x, c0.y "
	"																							rep i0 break_lt c0.x, c0.y "
	"																								rep i0 break_lt c0.x, c0.y "
	"																									abs r0, v0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_156_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_2_x : abs is allowed in a if b0 and if b0 block";
string PS_156 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_157_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_2_x : abs is allowed in a if b0 and if b0 nop else block";
string PS_157 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_158_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_2_x : abs is NOT allowed in a if b0 and loop aL, i0 block";
string PS_158 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_159_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_2_x : abs is allowed in a if b0 and rep i0 block";
string PS_159 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_160_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_2_x : abs is allowed in a if b0 and if_eq c0.x, c0.y block";
string PS_160 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_161_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_2_x : abs is allowed in a if b0 and if_eq c0.x, c0.y nop else block";
string PS_161 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_162_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_162_Desc = "ps_2_x : abs is allowed in a if b0 and if p0.x block";
string PS_162 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_163_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_163_Desc = "ps_2_x : abs is allowed in a if b0 and if p0.x nop else block";
string PS_163 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_164_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_2_x : abs is NOT allowed in a if b0 and loop aL, i0 break block";
string PS_164 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_165_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_2_x : abs is allowed in a if b0 and rep i0 break block";
string PS_165 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_166_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_166_Desc = "ps_2_x : abs is NOT allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_166 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_167_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_167_Desc = "ps_2_x : abs is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_167 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_168_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_2_x : abs is NOT allowed in a if b0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_168 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_169_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_2_x : abs is allowed in a if b0 and rep i0 break_lt c0.x, c0.y block";
string PS_169 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_170_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if b0 block";
string PS_170 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_171_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if b0 nop else block";
string PS_171 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_172_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_2_x : abs is NOT allowed in a if b0 nop else and loop aL, i0 block";
string PS_172 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_173_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_2_x : abs is allowed in a if b0 nop else and rep i0 block";
string PS_173 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_174_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if_eq c0.x, c0.y block";
string PS_174 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_175_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if_eq c0.x, c0.y nop else block";
string PS_175 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_176_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_176_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if p0.x block";
string PS_176 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_177_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_177_Desc = "ps_2_x : abs is allowed in a if b0 nop else and if p0.x nop else block";
string PS_177 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_178_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_2_x : abs is NOT allowed in a if b0 nop else and loop aL, i0 break block";
string PS_178 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_179_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_2_x : abs is allowed in a if b0 nop else and rep i0 break block";
string PS_179 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_180_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_180_Desc = "ps_2_x : abs is NOT allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_180 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_181_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_181_Desc = "ps_2_x : abs is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_181 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_182_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_2_x : abs is NOT allowed in a if b0 nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_182 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_183_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_2_x : abs is allowed in a if b0 nop else and rep i0 break_lt c0.x, c0.y block";
string PS_183 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_184_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if b0 block";
string PS_184 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_185_Criteria = { false, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if b0 nop else block";
string PS_185 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_186_Criteria = { false, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string PS_186 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_187_Criteria = { false, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and rep i0 block";
string PS_187 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_188_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if_eq c0.x, c0.y block";
string PS_188 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_189_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if_eq c0.x, c0.y nop else block";
string PS_189 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_190_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_190_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if p0.x block";
string PS_190 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_191_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_191_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and if p0.x nop else block";
string PS_191 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_192_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_192 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_193_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and rep i0 break block";
string PS_193 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_194_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_194_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_194 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_195_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_195_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_195 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_196_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_196 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_197_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 and rep i0 break_lt c0.x, c0.y block";
string PS_197 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_198_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_2_x : abs is allowed in a rep i0 and if b0 block";
string PS_198 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_199_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_2_x : abs is allowed in a rep i0 and if b0 nop else block";
string PS_199 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_200_Criteria = { false, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_2_x : abs is NOT allowed in a rep i0 and loop aL, i0 block";
string PS_200 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_201_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_2_x : abs is allowed in a rep i0 and rep i0 block";
string PS_201 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_202_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_2_x : abs is allowed in a rep i0 and if_eq c0.x, c0.y block";
string PS_202 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_203_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_2_x : abs is allowed in a rep i0 and if_eq c0.x, c0.y nop else block";
string PS_203 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_204_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_204_Desc = "ps_2_x : abs is allowed in a rep i0 and if p0.x block";
string PS_204 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_205_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_205_Desc = "ps_2_x : abs is allowed in a rep i0 and if p0.x nop else block";
string PS_205 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_206_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_2_x : abs is NOT allowed in a rep i0 and loop aL, i0 break block";
string PS_206 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_207_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_2_x : abs is allowed in a rep i0 and rep i0 break block";
string PS_207 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_208_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_208_Desc = "ps_2_x : abs is NOT allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_208 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_209_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_209_Desc = "ps_2_x : abs is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_209 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_210_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_2_x : abs is NOT allowed in a rep i0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_210 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_211_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_2_x : abs is allowed in a rep i0 and rep i0 break_lt c0.x, c0.y block";
string PS_211 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_212_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if b0 block";
string PS_212 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_213_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if b0 nop else block";
string PS_213 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_214_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y and loop aL, i0 block";
string PS_214 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_215_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and rep i0 block";
string PS_215 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_216_Criteria = { true, false, 0x0200, 2, -1, 0, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_216 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_217_Criteria = { true, false, 0x0200, 2, -1, 0, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_217 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_218_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_218_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if p0.x block";
string PS_218 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_219_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_219_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and if p0.x nop else block";
string PS_219 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_220_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y and loop aL, i0 break block";
string PS_220 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_221_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and rep i0 break block";
string PS_221 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_222_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_222_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_222 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_223_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_223_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_223 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_224_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_224 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_225_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_225 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_226_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if b0 block";
string PS_226 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_227_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if b0 nop else block";
string PS_227 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_228_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y nop else and loop aL, i0 block";
string PS_228 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_229_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and rep i0 block";
string PS_229 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_230_Criteria = { true, false, 0x0200, 2, -1, 0, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if_eq c0.x, c0.y block";
string PS_230 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_231_Criteria = { true, false, 0x0200, 2, -1, 0, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if_eq c0.x, c0.y nop else block";
string PS_231 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_232_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_232_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if p0.x block";
string PS_232 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_233_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_233_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and if p0.x nop else block";
string PS_233 = 
	"ps_2_x "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_234_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y nop else and loop aL, i0 break block";
string PS_234 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_235_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and rep i0 break block";
string PS_235 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_236_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_236_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_236 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_237_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_237_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_237 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_238_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_2_x : abs is NOT allowed in a if_eq c0.x, c0.y nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_238 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_239_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_2_x : abs is allowed in a if_eq c0.x, c0.y nop else and rep i0 break_lt c0.x, c0.y block";
string PS_239 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_eq c0.x, c0.y nop else "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_240_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_240_Desc = "ps_2_x : abs is allowed in a if p0.x and if b0 block";
string PS_240 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if p0.x "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_241_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_241_Desc = "ps_2_x : abs is allowed in a if p0.x and if b0 nop else block";
string PS_241 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if p0.x "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_242_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_242_Desc = "ps_2_x : abs is NOT allowed in a if p0.x and loop aL, i0 block";
string PS_242 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_243_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_243_Desc = "ps_2_x : abs is allowed in a if p0.x and rep i0 block";
string PS_243 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_244_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_244_Desc = "ps_2_x : abs is allowed in a if p0.x and if_eq c0.x, c0.y block";
string PS_244 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_245_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_245_Desc = "ps_2_x : abs is allowed in a if p0.x and if_eq c0.x, c0.y nop else block";
string PS_245 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_246_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_246_Desc = "ps_2_x : abs is allowed in a if p0.x and if p0.x block";
string PS_246 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_247_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_247_Desc = "ps_2_x : abs is allowed in a if p0.x and if p0.x nop else block";
string PS_247 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_248_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_248_Desc = "ps_2_x : abs is NOT allowed in a if p0.x and loop aL, i0 break block";
string PS_248 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_249_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_249_Desc = "ps_2_x : abs is allowed in a if p0.x and rep i0 break block";
string PS_249 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_250_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_250_Desc = "ps_2_x : abs is NOT allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_250 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_251_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_251_Desc = "ps_2_x : abs is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_251 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_252_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_252_Desc = "ps_2_x : abs is NOT allowed in a if p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_252 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_253_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_253_Desc = "ps_2_x : abs is allowed in a if p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_253 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_254_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_254_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if b0 block";
string PS_254 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if p0.x nop else "
	"	if b0 "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_255_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_255_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if b0 nop else block";
string PS_255 = 
	"ps_2_x "
	"defb b0, true "
	"dcl v0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_256_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_256_Desc = "ps_2_x : abs is NOT allowed in a if p0.x nop else and loop aL, i0 block";
string PS_256 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		abs r0, v0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_257_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_257_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and rep i0 block";
string PS_257 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	rep i0 "
	"		abs r0, v0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_258_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_258_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if_eq c0.x, c0.y block";
string PS_258 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_259_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_259_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if_eq c0.x, c0.y nop else block";
string PS_259 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_260_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_260_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if p0.x block";
string PS_260 = 
	"ps_2_x "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_261_Criteria = { true, false, 0x0200, 2, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string PS_261_Desc = "ps_2_x : abs is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_261 = 
	"ps_2_x "
	"dcl v0 "