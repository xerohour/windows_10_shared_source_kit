/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    hub.c

     3-10-2002

Abstract:

    PnP handlers and general dispatch handlers for the PDO.



Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/



#include "hubdef.h"
#include "pch.h"

// For DRM support - (Digital Rights Management)
// NOTE: including only ksdrmhlp.h allows usbhub to build with ntddk.h instead of wdm.h

#include <ksdrmhlp.h>

//#include <windef.h>
//#include <unknown.h>
//#include <ks.h>
//#include <ksmedia.h>
//#include <drmk.h>
//#include <ksdrmhlp.h>


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


// For IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER
#include <ntddstor.h>

NTSTATUS
UsbhPdoSetContentId(
    PIRP Irp,
    PKSP_DRMAUDIOSTREAM_CONTENTID KsProperty,
    PKSDRMAUDIOSTREAM_CONTENTID Data
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, UsbhPdoSetContentId)
#pragma alloc_text(PAGE, UsbhPdo_IsDeviceExternal)
#pragma alloc_text(PAGE, UsbhD3ColdSupportInterfaceGetIdleWakeInfo)
#pragma alloc_text(PAGE, UsbhD3ColdSupportInterfaceGetD3ColdCapability)
#endif

#ifdef HUB_WPP
#include "pdo.tmh"
#endif

CCHAR usbfile_pdo_c[] = "pdo.c";

#define USBFILE usbfile_pdo_c


#define FREE_ID_STRINGS(pdoExt) \
    UsbhFreeID(&(pdoExt)->PnpHardwareId);\
    UsbhFreeID(&(pdoExt)->PnpDeviceId);\
    UsbhFreeID(&(pdoExt)->PnpCompatibleId);\
    UsbhFreeID(&(pdoExt)->UniqueId);\
    UsbhFreeID(&(pdoExt)->SerialNumberId);\
    UsbhFreeID(&(pdoExt)->ProductId);\
    UsbhFreeID(&(pdoExt)->SupportedLanguageIds);\
    UsbhFreeID(&(pdoExt)->GenericDevString);\
    UsbhFreeID(&(pdoExt)->TextLocationIdString);\
    UsbhFreeID(&(pdoExt)->FriendlyNameIdString);\



NTSTATUS
UsbhIncPdoIoCount(
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG TagSig,
    ULONG UserTag
    )
 /*++

Routine Description:

    Increment our pending io count for the pdo.

    We add the Tag element to the list of pending io for the PDO so that we
    don't delete the devobj while we are processing a request.  The purpose of
    this code is to prevent the hub from crashing if the client driver has a
    request passing through the PDO during the remove (it shouldn't unless
    it has a bug).

    In many cases  hub PDO doesn't actually service the request so it never accesses the
    irp other than to complete it or pass it on.

    If the failIo flag is set NT_DEVICE_DISCONNECTED_STATUS is returned and the
    calling function must immediately complete the request.

    The definition of 'Io' is somewhat subjective. Currently this list includes:
    IRP_MJ_CREATE
    IRP_MJ_CLOSE
    IRP_MJ_DEVICE_CONTROL (aka user mode IOCTLs)
    IRP_MJ_SYSTEM_CONTROL (aka WMI)
    IRP_MN_POWER_SEQUENCE
    IRP_MN_SET_POWER
    IRP_MN_WAIT_WAKE

Arguments:

    Object - object associated with this reference

Return Value:

    ntStatus
--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_LIST_ENTRY iol;
    KIRQL irql;

    pdoExt = PdoExt(Pdo);
    iol = NULL;

    KeAcquireSpinLock(&pdoExt->IoLock, &irql);

    UsbhAssert(NULL, pdoExt->IoState != Pdo_UndefinedIo);

    if (pdoExt->IoState == Pdo_FailIo) {
        KeReleaseSpinLock(&pdoExt->IoLock, irql);
        return NT_DEVICE_DISCONNECTED_STATUS;
    }

#ifndef USB_FORCE_MEMORY_STRESS
    UsbhAllocatePool_Z(iol, NonPagedPool, sizeof(*iol));
#endif

    if (iol) {
        iol->Sig = UsbhRevSig(TagSig);
        iol->Object = Object;
        iol->UserTag = UserTag;
        InsertTailList(&pdoExt->IoList, &iol->Link);
    } else {
        // Allocation fails use the resource-less ref count
        pdoExt->IoRefCountNoRes++;
    }

    KeReleaseSpinLock(&pdoExt->IoLock, irql);

    return STATUS_SUCCESS;
}


VOID
UsbhDecPdoIoCount(
    PDEVICE_OBJECT Pdo,
    PVOID Object
    )
 /*++

Routine Description:

    Decrement our pending io count for the pdo.

    If the list goes empty then we signal the event indicating it is 'safe' to
    delete the PDO

Arguments:

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    KIRQL irql;
    PLIST_ENTRY le;
    PIO_LIST_ENTRY iol;
    BOOLEAN found = FALSE;

    pdoExt = PdoExt(Pdo);

    KeAcquireSpinLock(&pdoExt->IoLock, &irql);

    UsbhAssert(NULL, pdoExt->IoState != Pdo_UndefinedIo);

    // find the head
    GET_HEAD_LIST(pdoExt->IoList, le);

    // check if we have an matching entry -- this one.
    if (le) {

        while (le != &pdoExt->IoList) {

            iol = (PIO_LIST_ENTRY) CONTAINING_RECORD(
                            le,
                            struct _IO_LIST_ENTRY,
                            Link);

            if (iol->Object == Object) {
                found = TRUE;
                RemoveEntryList(le);
                UsbhFreePool(iol);
                break;
            }

            le = le->Flink;
        }
    }

    if (!found) {
        // This could be a resource-less deref
        if (pdoExt->IoRefCountNoRes) {
            pdoExt->IoRefCountNoRes--;
        } else {
            UsbhPdoTrapFatal(Pdo, Object);
        }
    }

    if (IsListEmpty(&pdoExt->IoList) &&
        !pdoExt->IoRefCountNoRes) {
        // last entry removed signals the event
        KeReleaseSpinLock(&pdoExt->IoLock, irql);
        SET_EVENT(&pdoExt->IoRemoveEvent);
    } else {
        KeReleaseSpinLock(&pdoExt->IoLock, irql);
    }
}



NTSTATUS
UsbhPdoCreateSymbolicLink(
    PDEVICE_OBJECT Pdo,
    LPGUID lpGuid
    )
/*++

Routine Description:

    Creates a symbolic Link for a PDO we enumerate

Irql: PASSIVE

Arguments:

Return Value:

    PdoFlag.SymbolicLink set as appropriate

    final status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts = STATUS_SUCCESS;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(pdoExt->HubFdo, pdoExt->PdoFlags.SymbolicLink == 0);
    /*
     *  Create the symbolic link
     */
    // register is a one time operation which has no 'undue' it puts the
    // initial stuff needed for sym links in the registry.
    nts = IoRegisterDeviceInterface(
                Pdo,
                lpGuid,
                NULL,
                &pdoExt->SymbolicLinkName);

    if (NT_SUCCESS(nts)) {

        /*
         *  Now set the symbolic link for the association and store it.
         */

        // write the symbolic name to the registry for USBUI to find

        nts = IoSetDeviceInterfaceState(&pdoExt->SymbolicLinkName,
                                        TRUE);

        // don't enter the reg key unless we can create the link
        if (NT_SUCCESS(nts)) {
            pdoExt->PdoFlags.DeviceInterface = 1;
            nts = UsbhSetRegSymbolicLinkName(Pdo, &pdoExt->SymbolicLinkName);
        }

        if (NT_SUCCESS(nts)) {
            // we have a link and  a reg key
            pdoExt->PdoFlags.SymbolicLink = 1;
        } else {
            if (pdoExt->PdoFlags.DeviceInterface) {
                // unwind
                pdoExt->PdoFlags.DeviceInterface = 0;
                IoSetDeviceInterfaceState(&pdoExt->SymbolicLinkName,
                                          FALSE);
            }
            pdoExt->PdoFlags.SymbolicLink = 0;
            UsbhFreePool(pdoExt->SymbolicLinkName.Buffer);
        }
    }

    if (NT_SUCCESS(nts)) {
        UsbhAssert(pdoExt->HubFdo,  pdoExt->PdoFlags.DeviceInterface &&
                                    pdoExt->PdoFlags.SymbolicLink);
    } else {
        // if we cannot create the link certain test programs will not be able
        // to walk the tree and this will generate 'treelog' stress failures
        // so we log an error.
        HUB_EXCEPTION(USBFILE,  pdoExt->HubFdo, pdoExt->PortNumber, PnpErr_PdoSymLinkFailed,
                          nts, 0xFFFFFFFF, NULL, 0);
    }

    LOG(pdoExt->HubFdo, LOG_PDO, 'symL', nts, Pdo);

    return nts;
}


VOID
UsbhPdoDeleteSymbolicLink(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Creates a symbolic Link for a PDO we enumerate

Irql: PASSIVE

Arguments:

Return Value:

    PdoFlag.SymbolicLink set as appropriate

    final status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts = STATUS_SUCCESS;

    pdoExt = PdoExt(Pdo);

    /*
     *  Disable the symbolic link
     */
    if (pdoExt->PdoFlags.SymbolicLink) {
        nts = IoSetDeviceInterfaceState(&pdoExt->SymbolicLinkName, FALSE);
        UsbhFreePool(pdoExt->SymbolicLinkName.Buffer);

        //ignore the error, as far as we are concerned its gone
        pdoExt->PdoFlags.SymbolicLink = 0;
    }

    LOG(pdoExt->HubFdo, LOG_PDO, 'symL', nts, Pdo);

}


NTSTATUS
UsbhPdoPnp_QueryInterface(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handler for IRP_MN_QUERY_INTERFACE, we add the device handle to the request

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts, default_status;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_OBJECT latched = NULL, TopOfBusStack;
    PUSB_DEVICE_HANDLE dh;

    pdoExt = PdoExt(Pdo);


    LOG(pdoExt->HubFdo, LOG_PDO, 'qif0', Irp, Pdo);

    // we always return the default status because driver verifier will complain if we
    // don't
    default_status = Irp->IoStatus.Status;

    // we don't inc the io count because this request is not IO and it can be sent
    // 'anytime after add_device is called' so we latch it instead.
    // The latch will hold the HubFdo in place while we perform the request.
    if (pdoExt->HubFdo) {
        latched = UsbhLatchPdo(pdoExt->HubFdo, pdoExt->PortNumber, Irp, 'QUIF');
    }

    // this fails after the PDO is disconnected from the hub
    // this would be a case where the client is sending bogus requests.
    if (!latched) {
        // cannot safely log unless PDO is latched
        //LOG(pdoExt->HubFdo, LOG_PDO, 'qf1!', Irp, Pdo);

        nts = NT_DEVICE_DISCONNECTED_STATUS;
        UsbhCompleteIrp(Irp, default_status);
        return default_status;
    }

    hubExt = FdoExt(pdoExt->HubFdo);

    // Also, fail the request if it looks like we are removing
    TopOfBusStack = hubExt->TopOfBusStack;
    if (!TopOfBusStack || (GET_PDO_HWPNPSTATE(Pdo) != Pdo_PnpRefHwPresent)) {
        LOG(pdoExt->HubFdo, LOG_PDO, 'qf1!', Irp, Pdo);

        nts = NT_DEVICE_DISCONNECTED_STATUS;
        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
        UsbhCompleteIrp(Irp, default_status);
        return default_status;
    }

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_INTERFACE);

    if (RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
                         &USB_BUS_INTERFACE_HUB_SS_GUID,
                         sizeof(GUID)) == sizeof(GUID)) {

        LOG(pdoExt->HubFdo, LOG_PDO, 'qiSS', Irp, Pdo);

        nts = UsbhBusifGetSSHub(pdoExt->HubFdo,
                                Pdo,
                                Irp);

        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
        UsbhCompleteIrp(Irp, nts);
    } else if (RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
                                &GUID_PNP_LOCATION_INTERFACE,
                                sizeof(GUID)) == sizeof(GUID)) {

        nts = UsbhBusifGetLocationInterface(pdoExt->HubFdo,
                                            Pdo,
                                            Irp);

        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
        UsbhCompleteIrp(Irp, nts);

    } else if (RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_USBDI_GUID,
               sizeof(GUID)) == sizeof(GUID) ||
        RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_GUID,
               sizeof(GUID)) == sizeof(GUID) ||
        RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_MINIDUMP_GUID,
               sizeof(GUID)) == sizeof(GUID) ||
        RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID,
               sizeof(GUID)) == sizeof(GUID)) {

        LOG(pdoExt->HubFdo, LOG_PDO, 'qif2', Irp, Pdo);

        // bugbug JD
        // *root bus should release this or we release on completion
        // *we can remove this if we enable proxy_usbd INTERFACE support in the hub
        dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, Pdo, Irp, Devh_QIF_Tag);

        if (dh == NULL) {
            // this may be some type of restart so we need to restore the
            // device handle

            nts = UsbhSyncResetDeviceInternal(pdoExt->HubFdo, PDO_CONTEXT(Pdo), Pdo);

            LOG(pdoExt->HubFdo, LOG_PDO, 'qifR', Irp, nts);
            if (NT_ERROR(nts)) {

                if (!Usb_Disconnected(nts)) {
                      HUB_EXCEPTION(USBFILE, pdoExt->HubFdo, pdoExt->PortNumber,
                        PnpErr_FailedResetOnQI,
                        nts, 0xFFFFFFFF, NULL, 0);
                }

               UsbhCompleteIrp(Irp, default_status);

               UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
               return default_status;
            }

            dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, Pdo, Irp, Devh_QIF_Tag);

            LOG(pdoExt->HubFdo, LOG_PDO, 'qifH', pdoExt->DeviceHandle, Pdo);
            ioStack->Parameters.QueryInterface.InterfaceSpecificData = dh;
        }

        LOG(pdoExt->HubFdo, LOG_PDO, 'qifH', pdoExt->DeviceHandle, Pdo);
        ioStack->Parameters.QueryInterface.InterfaceSpecificData = dh;

        if (dh) {
            UsbhDerefPdoDeviceHandle(pdoExt->HubFdo, dh, Irp, Devh_QIF_Tag);
        }

        LOG(pdoExt->HubFdo, LOG_PDO, 'qif3', Irp, Pdo);
        // Pass all QUERY_INTERFACE requests by USB drivers (for USB INTERFACES) to the root bus
        // we have to do this, if we do not then another hub in the chain will trash our device
        // handle we are passing along.
        IoSkipCurrentIrpStackLocation(Irp);
        nts = IoCallDriver(TopOfBusStack, Irp);

        // PDO won't disconnect from the hub until we do this
        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');

    } else if ( RtlCompareMemory(ioStack->Parameters.QueryInterface.InterfaceType,
                                 &GUID_D3COLD_SUPPORT_INTERFACE,
                                 sizeof(GUID)) == sizeof(GUID)) {

        PD3COLD_SUPPORT_INTERFACE   d3ColdSupportInterface;

        LOG(pdoExt->HubFdo, LOG_PDO, 'qif5', Irp, Pdo);

        if (ioStack->Parameters.QueryInterface.InterfaceSpecificData == pdoExt) {
            // Query originated from the hub driver, so leave the status alone and complete the IRP
            UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
            nts = Irp->IoStatus.Status;
            UsbhCompleteIrp(Irp, nts);
            return nts;
        }

        if ((ioStack->Parameters.QueryInterface.Size != sizeof(D3COLD_SUPPORT_INTERFACE)) ||
            (ioStack->Parameters.QueryInterface.Version != D3COLD_SUPPORT_INTERFACE_VERSION)) {

            //
            // We don't understand this version. Leave the status alone and
            // complete the IRP
            //
            UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
            nts = Irp->IoStatus.Status;
            UsbhCompleteIrp(Irp, nts);
            return nts;
        }

        d3ColdSupportInterface = (PD3COLD_SUPPORT_INTERFACE) ioStack->Parameters.QueryInterface.Interface;

        if (Irp->IoStatus.Status == STATUS_SUCCESS) {
            //
            // ACPI has filtered the QI. If so, we should verify that the
            // version and size filled by ACPI is what we expect
            //
            if ((d3ColdSupportInterface->Size != sizeof(D3COLD_SUPPORT_INTERFACE)) ||
                (d3ColdSupportInterface->Version != D3COLD_SUPPORT_INTERFACE_VERSION)) {

                UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
                nts = STATUS_NOT_SUPPORTED;
                UsbhCompleteIrp(Irp, nts);
                return nts;
            }

            //
            // Make a copy of information filled by ACPI
            //
            RtlCopyMemory(&pdoExt->D3ColdFilter,
                          d3ColdSupportInterface,
                          sizeof(D3COLD_SUPPORT_INTERFACE));
        } else {

            d3ColdSupportInterface->Size = sizeof(D3COLD_SUPPORT_INTERFACE);
            d3ColdSupportInterface->Version = D3COLD_SUPPORT_INTERFACE_VERSION;
        }

        d3ColdSupportInterface->Context = pdoExt;
        d3ColdSupportInterface->SetD3ColdSupport = UsbhD3ColdSupportInterfaceSetD3ColdSupport;
        d3ColdSupportInterface->GetIdleWakeInfo = UsbhD3ColdSupportInterfaceGetIdleWakeInfo;
        d3ColdSupportInterface->InterfaceDereference = UsbhD3ColdSupportInterfaceDereference;
        d3ColdSupportInterface->InterfaceReference = UsbhD3ColdSupportInterfaceReference;
        d3ColdSupportInterface->GetD3ColdCapability = UsbhD3ColdSupportInterfaceGetD3ColdCapability;
        d3ColdSupportInterface->GetBusDriverD3ColdSupport = UsbhD3ColdSupportInterfaceGetD3ColdBusDriverSupport;
        d3ColdSupportInterface->GetLastTransitionStatus = UsbhD3ColdSupportInterfaceGetLastTransitionStatus;

        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');
        nts = STATUS_SUCCESS;
        UsbhCompleteIrp(Irp, nts);
        return nts;

    }else {

        LOG(pdoExt->HubFdo, LOG_PDO, 'qif4', Irp, Pdo);

        UsbhUnlatchPdo(pdoExt->HubFdo, latched, Irp, 'QUIF');

        // Interface not for USB, complete here
        nts = default_status;
        UsbhCompleteIrp(Irp, default_status);

    }

    return nts;
}


NTSTATUS
UsbhPdoPnp_QueryResourceRequirements(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    In addition to completing with success we 'mark' the PDO as PnP Valid
    here so we know we can make certain kernel function calls wich take a PDO
    as a parameter.

    This code dates to some of the earliest PnP implementations (like win9x)

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    pdoExt->PdoFlags.PnpValid = 1;

    if ((pdoExt->MsOs20Info.Flags.RegistryProperty == 1) &&
        (!pdoExt->PdoFlags.ExtPropertyInstalled)) {

        UsbhInstallMsOs20RegistryProperties(PDO_CONTEXT(Pdo), Pdo);

    } else if (pdoExt->DeviceFlags.MsOsSupported &&
               !pdoExt->PdoFlags.ExtPropertyInstalled &&
               !pdoExt->PdoFlags.DeviceIsComposite ) {

        UsbhInstallMsOsExtendedProperties(PDO_CONTEXT(Pdo), Pdo);
    }

    if ((pdoExt->MsOs20Info.Flags.ModelId == 1) &&
        (!pdoExt->PdoFlags.ExtPropertyInstalled)) {

        nts = IoSetDevicePropertyData (Pdo,
                                       &DEVPKEY_Device_ModelId,
                                       LOCALE_NEUTRAL,
                                       0,
                                       DEVPROP_TYPE_GUID,
                                       sizeof(GUID),
                                       &pdoExt->MsOs20Info.ModelIdDescriptor->ModelId);

        if (!NT_SUCCESS(nts)) {
            LOG(pdoExt->HubFdo, LOG_PDO, 'QRR1', nts, 0);
        }
    }

    nts = Irp->IoStatus.Status;
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoPnp_DoDefault(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Default handler for PDO targeted PnP Irps.

    Complete request with original status

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    NTSTATUS nts;

    nts = Irp->IoStatus.Status;
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoSuccess(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    just completes the irp with success

Arguments:

Return Value:

    usually STATUS_SUCCESS unless we are in the process of deleting the PDO

--*/

{
    NTSTATUS nts;

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'PDOS', 0);
    if (NT_ERROR(nts)) {
        UsbhCompleteIrp(Irp, nts);
        return nts;
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);
    return nts;
}

ULONG
UsbhGetInternalIoctlTag(
    ULONG IoControlCode
    )
 /*++

Routine Description:

    Coverts an internal IOCTL code to a unique tag we can recognize
    in the debugger.

Arguments:

Return Value:

    Tag value
--*/
{
    switch (IoControlCode) {

    case IOCTL_INTERNAL_USB_GET_HUB_COUNT:
        return IOCTL_INTERNAL_USB_GET_HUB_COUNT_Tag;

    case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO:
        return IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO_Tag;

    case IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME:
        return IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME_Tag;

    case IOCTL_INTERNAL_USB_GET_BUS_INFO:
        return IOCTL_INTERNAL_USB_GET_BUS_INFO_Tag;

    case IOCTL_INTERNAL_USB_SUBMIT_URB:
        return IOCTL_INTERNAL_USB_SUBMIT_URB_Tag;

    case IOCTL_INTERNAL_USB_GET_PORT_STATUS:
        return IOCTL_INTERNAL_USB_GET_PORT_STATUS_Tag;

    case IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE:
        return IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE_Tag;

    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE:
        return IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_Tag;

    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX:
        return IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX_Tag;

    case IOCTL_INTERNAL_USB_RESET_PORT_ASYNC:
    case IOCTL_INTERNAL_USB_RESET_PORT:
        return IOCTL_INTERNAL_USB_RESET_PORT_Tag;

    case IOCTL_INTERNAL_USB_ENABLE_PORT:
        return IOCTL_INTERNAL_USB_ENABLE_PORT_Tag;

    case IOCTL_INTERNAL_USB_CYCLE_PORT:
        return IOCTL_INTERNAL_USB_CYCLE_PORT_Tag;

    case IOCTL_INTERNAL_USB_GET_HUB_NAME:
        return IOCTL_INTERNAL_USB_GET_HUB_NAME_Tag;

    case IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO:
        return IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO_Tag;

    case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:
        return IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION_Tag;

    case IOCTL_INTERNAL_USB_RECORD_FAILURE:
        return IOCTL_INTERNAL_USB_RECORD_FAILURE_Tag;

    case IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO:
        return IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO_Tag;

    case IOCTL_INTERNAL_USB_GET_DUMP_DATA:
        return IOCTL_INTERNAL_USB_GET_DUMP_DATA_Tag;

    case IOCTL_INTERNAL_USB_FREE_DUMP_DATA:
        return IOCTL_INTERNAL_USB_FREE_DUMP_DATA_Tag;

    case IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS:
        return IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS_Tag;
    }

    // if we don't recognoze it just use the value as the tag
    return IoControlCode;

}


NTSTATUS
UsbhPdoInternalDeviceControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handle 'Internal' Device Control Irps sent to PDOs

    Some of these APIs need access to the parent hub Device Object. The hub
    is held in memory by a hub reference (PDO->FDO). This  reference is
    freed when the PDO is removed. We only process the API if the PDO holds
    the remove lock wich holds hub reference holding the parent hub in memory.

    In addition the PDO holds a reference as long as it is processing the irp.
    The net effect is that both the PDO and the FDO for the parent hub are
    locked in until the we are done with the irp.

Arguments:

Return Value:

    ntStatus
--*/
{
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    ULONG ioControlCode;
    ULONG tag;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    pdoExt = PdoExt(Pdo);
    UsbhAssert(NULL,
        ioStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL);


    // make sure we have not already processed the remove. If the client is
    // sending requests after it has passed the remove down then it has major
    // problems and will probably crash the system.  This check simply keeps it
    // from causing us to crash.  It also prevents us from removing the PDO in
    // the event there is an irp on its way down to the port driver.
    //
    // The hub driver never pends requests in its own queue so it has no cancel
    // logic

    ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;

    //
    // The following 2 IOCTLS are allowed to come in when other
    // I/O's are failing. The PdoIoCount is not maintained for these
    if (ioControlCode == IOCTL_INTERNAL_USB_RESET_PORT_ASYNC) {

        // Give the hub sometime to be able to propogate FailIo bit
        UsbhWait(pdoExt->HubFdo,100);
        // This means that the hub was reset while there was no I/O from the
        // storage driver
        if (pdoExt->ResetCompleteWithoutFailingIO) {

            DbgTrace((UsbhPwBootTrace,"Received request from USBStor after reset completion %!FUNC!\n"));
            LOG(pdoExt->HubFdo, LOG_PDO, 'SRRC', 0, pdoExt->HubFdo);
            UsbhSetPdo_AllowIo(pdoExt->HubFdo,Pdo);
            UsbhCompleteIrp(Irp, STATUS_SUCCESS);
            pdoExt->PendingResetPortAsyncIrp = NULL;
            pdoExt->ResetCompleteWithoutFailingIO = FALSE;
            return STATUS_SUCCESS;
        } else if (pdoExt->IoState == Pdo_FailIo) {

            DbgTrace((UsbhPwBootTrace,"Received request from USBStor for reset completion %!FUNC!\n"));
            LOG(pdoExt->HubFdo, LOG_PDO, 'SRRQ', 0, pdoExt->HubFdo);
            pdoExt->PendingResetPortAsyncIrp = Irp;
            IoMarkIrpPending(Irp);
            return STATUS_PENDING;
        }

    } else if (ioControlCode == IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST) {

        DbgTrace((UsbhPwBootTrace,"Received request from downstream USBHub in HubFDO=%p %!FUNC!\n", pdoExt->HubFdo));
        LOG(pdoExt->HubFdo, LOG_PDO, 'SRRH', 0, pdoExt->HubFdo);
        pdoExt->PendingResetNotificationRequestIrp = Irp;
        IoMarkIrpPending(Irp);
        return STATUS_PENDING;
        
    } else if (ioControlCode == IOCTL_INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE) {

        DbgTrace((UsbhDebugTrace, "Received IOCTL_INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE from class driver=%p %!FUNC!\n", pdoExt->Pdo));
        UsbhCompleteIrp(Irp, STATUS_SUCCESS);
        pdoExt->DeviceFlags.FailGetStatusRequest = 1;
        return STATUS_SUCCESS;
    }
    

    tag = UsbhGetInternalIoctlTag(ioControlCode);

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'IDIO', tag);
    if (NT_ERROR(nts)) {
        PURB urb;

        // for submit urb set the error code in the urb as well
        if (ioControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {
            urb = ioStack->Parameters.Others.Argument1;
            // this error was returned by the original XP stack if any
            // ntstatus error was detected.

            urb->UrbHeader.Status = USBD_STATUS_INVALID_PARAMETER;
        }

        UsbhCompleteIrp(Irp, nts);
        return nts;
    }

 //   if (GET_PDO_DX(Pdo) != PowerDeviceD0) {
 //       TEST_TRAP();
 //   }

// snippet from original hub driver for reference
#if 0
            if (DeviceExtensionPort->DeviceState != PowerDeviceD0) {
#if DBG
            UsbhWarning(DeviceExtensionPort,
                "Client Device Driver is sending requests to a device in a low power state.\n",
                (BOOLEAN)((USBH_Debug_Trace_Level > 0) ? TRUE : FALSE));
#endif

            // Must use an error code here that can be mapped to Win32 in
            // rtl\generr.c

            ntStatus = STATUS_DEVICE_POWERED_OFF;
            USBH_CompleteIrp(Irp, ntStatus);
            break;
        }
#endif
    // we release the hub ref when we complete the remove so the hub FDO should
    // be valid
    UsbhAssert(NULL, pdoExt->HubFdo);

    switch (ioControlCode) {

    case IOCTL_INTERNAL_USB_GET_HUB_COUNT:
        return UsbhFdoReturnHubCount(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO:
        return UsbhFdoReturnRootHubPdo(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME:
        return UsbhFdoReturnControllerName(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_BUS_INFO:
        return UsbhFdoReturnDeviceBusInfo(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_SUBMIT_URB:
        return UsbhFdoUrbPdoFilter(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_PORT_STATUS:
        return UsbhFdoReturnPortStatus(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE:
        return UsbhFdoReturnTtDeviceHandle(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE:
        return UsbhFdoReturnDeviceHandle(pdoExt->HubFdo, Pdo, Irp, FALSE);

    case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX:
        return UsbhFdoReturnDeviceHandle(pdoExt->HubFdo, Pdo, Irp, TRUE);

    case IOCTL_INTERNAL_USB_RESET_PORT_ASYNC:
    case IOCTL_INTERNAL_USB_RESET_PORT:
        return UsbhFdoResetPdoPort(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_ENABLE_PORT:
        return UsbhFdoEnablePdoPort(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_CYCLE_PORT:
        return UsbhFdoCyclePdoPort(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_HUB_NAME:
        return UsbhFdoReturnHubName(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO:
        return UsbhFdoReturnParentHubInfo(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:
        return UsbhFdoSubmitPdoIdleNotification(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_RECORD_FAILURE:
        return UsbhFdoRecordFailure(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY:
        return UsbhFdoPdoIoctlIdleReady(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO:
        return UsbhFdoReturnDeviceConfigInfo(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS:
        return UsbhFdoReturnTopologyAddress(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_GET_DUMP_DATA:
        return UsbhFdoHandleGetDumpDataIoctl(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_FREE_DUMP_DATA:
        return UsbhFdoHandleFreeDumpDataIoctl(pdoExt->HubFdo, Pdo, Irp);

    case IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS:
        return UsbhFdoHandleNotifyForwardProgress(pdoExt->HubFdo, Pdo, Irp);
    }

    // complete all unknown irps with the status from the irp
    nts = Irp->IoStatus.Status;
    UsbhCompleteIrp(Irp, nts);

    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;

}



NTSTATUS
UsbhPdoSetContentId(
    PIRP Irp,
    PKSP_DRMAUDIOSTREAM_CONTENTID KsProperty,
    PKSDRMAUDIOSTREAM_CONTENTID Data
)
/*++

Routine Description:

    Magic function to process KS data

    Note that we hold an IOref on the PDO wich locks the hub in place

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
    property, but will not call DrmForwardContentToDeviceObject as it did in
    previous versions of Windows. Therefore, other USB drivers lower in the
    device stack and higher in the PnP tree nolonger need to support this KS
    property IOCTL.

--*/
{
    PAGED_CODE();

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhPdoDeviceControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    User IOCTL APIs send to our PDO.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    ULONG ioControlCode;
    PDEVICE_EXTENSION_PDO pdoExt;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(NULL, ioStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);
    ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'DIOC', ioControlCode);
    if (NT_ERROR(nts)) {
        UsbhCompleteIrp(Irp, nts);
        return nts;
    }

    pdoExt = PdoExt(Pdo);

    switch (ioControlCode) {

#if DRM_SUPPORT
    case IOCTL_KS_PROPERTY:

        nts = KsPropertyHandleDrmSetContentId(Irp, UsbhPdoSetContentId);
        LOG(pdoExt->HubFdo, LOG_PDO, 'pio1', Irp, nts);
        break;
#endif

    case IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER:
        nts = STATUS_NOT_SUPPORTED;
        LOG(pdoExt->HubFdo, LOG_PDO, 'pio2', Irp, 0);
        break;

    default:
        //complete with status in the irp
        LOG(pdoExt->HubFdo, LOG_PDO, 'pio3', Irp, Irp->IoStatus.Status);
        nts = Irp->IoStatus.Status;
    }

    UsbhCompleteIrp(Irp, nts);
    UsbhDecPdoIoCount(Pdo, Irp);

    return nts;
}


NTSTATUS
UsbhPdoSystemControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handles the WMI related system control irp send to our PDO.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    SYSCTL_IRP_DISPOSITION irpDisposition;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS orgIrpStatus;

    orgIrpStatus = Irp->IoStatus.Status;

    nts = UsbhIncPdoIoCount(Pdo, Irp, 'SYSI', 0);
    if (NT_ERROR(nts)) {
        UsbhCompleteIrp(Irp, orgIrpStatus);
        return orgIrpStatus;
    }

    pdoExt = PdoExt(Pdo);
    LOG(pdoExt->HubFdo, LOG_PDO, 'PWMI', Irp, 0);

    // Wmi lib tells us what to do
    nts = WmiSystemControl(
                &pdoExt->WmiLibInfo,
                Pdo,
                Irp,
                &irpDisposition);

    LOG(pdoExt->HubFdo, LOG_PDO, 'pWMI', Irp, nts);

    switch (irpDisposition) {
    case IrpNotWmi:
        // don't change status of IRP we don't know about.
        nts = Irp->IoStatus.Status;
    case IrpNotCompleted:
    case IrpForward:
    default:
        UsbhCompleteIrp(Irp, nts);
        break;

    case IrpProcessed:
        // Don't complete the IRP in this case, just return status.
        break;
    }

    // we are done with it now
    UsbhDecPdoIoCount(Pdo, Irp);


    return nts;
}


VOID
UsbhPdoSetDeviceData(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object
    )
/*++

Routine Description:

    Sets device data in device handle from the PDO while we have a grip on the PDO.
    This data is used later for diagnostics.


Arguments:

    Pdo - a latched Pdo

Return Value:

    none

--*/
{

    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_HANDLE dh;

    pdoExt = PdoExt(Pdo);

    dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Object, Devh_SetDevData_Tag);
    if (dh) {

        LOG(HubFdo, LOG_IOCTL, 'setD', 0, dh);

        Usbh_HubSetDeviceHandleData(HubFdo, dh, Pdo);

        UsbhDerefPdoDeviceHandle(HubFdo, dh, Object, Devh_SetDevData_Tag);
    }

}


BOOLEAN
UsbhPdoCheckBootDeviceReady(
    __in PDEVICE_OBJECT Pdo
    )
{
    PDEVICE_OBJECT hubFdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USB_HUB_PORT_STATE bootPortState;
    EX_DATA portExData;
    USBD_STATUS usbdStatus;
    BOOLEAN bootDeviceReady = FALSE;

    pdoExt = PdoExt(Pdo);
    hubFdo = pdoExt->HubFdo;
    hubExt = FdoExt(hubFdo);

    portExData.nBytes = sizeof(struct _USB_HUB_PORT_STATE);
    portExData.PortNumber = pdoExt->PortNumber;

    // setup packet for Get Port Status
    // 11.24.2.7
    setupPacket.bmRequestType.B = 0xa3; //10100011B
    setupPacket.bRequest =
        portExData.bRequest = USB_REQUEST_GET_STATUS;
    setupPacket.wValue.W = 0;
    setupPacket.wIndex.W = pdoExt->PortNumber;
    setupPacket.wLength = portExData.nBytes;

    portExData.State.Change = 0;
    portExData.State.Status = 0;
    if (NT_SUCCESS(UsbhSyncSendCommand(hubFdo,
                                       &setupPacket,
                                       (PUCHAR)&portExData.State,
                                       &portExData.nBytes,
                                       USBH_CONTROL_PIPE_TIMEOUT,
                                       &usbdStatus))) {
        bootPortState.Change.us = portExData.State.Change;
        bootPortState.Status.us = portExData.State.Status;

        if (pdoExt->BootDeviceConnectChangeBitExpected != HUB_RESET_DO_NOT_NEED_BIT &&
            pdoExt->BootDeviceConnectBitExpected != HUB_RESET_DO_NOT_NEED_BIT) {
            // This means that the caller cares about both the connect bit
            // and the Connect Change bit
            if (bootPortState.Change.ConnectChange == pdoExt->BootDeviceConnectChangeBitExpected &&
                bootPortState.Status.Connect == pdoExt->BootDeviceConnectBitExpected) {
                bootDeviceReady = TRUE;
            }
        } else if (bootPortState.Change.ConnectChange == pdoExt->BootDeviceConnectChangeBitExpected) {
            bootDeviceReady = TRUE;
        } else if (bootPortState.Status.Connect == pdoExt->BootDeviceConnectBitExpected) {
            bootDeviceReady = TRUE;
        }
    } else {
        // Unable to communicate with the parent hub. Just report that
        // the boot device is ready.
        bootDeviceReady = TRUE;
    }

    if (bootDeviceReady) {
        KeSetEvent(&pdoExt->BootDeviceArrivalEvent, 0, FALSE);
    }

    return bootDeviceReady;
}


NTSTATUS
UsbhPdoPnp_StartDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP start for a Pdo

    IRP_MN_START_DEVICE

Arguments:


Return Value:

    nt status

--*/

{
    NTSTATUS nts = STATUS_INVALID_PARAMETER;
    PDEVICE_EXTENSION_PDO pdoExt;
    LPGUID lpGuid;
    PIO_STACK_LOCATION ioStack;
    PSSH_BUSY_HANDLE bh;
    PWCHAR deviceDescription;
    ULONG requiredLength;

    pdoExt = PdoExt(Pdo);

    UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_START_DISPATCH, 0);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    DbgTrace((UsbhDebugTrace, "Starting PDO %p\n", Pdo));
    UsbhAssert(pdoExt->HubFdo, ioStack->MinorFunction == IRP_MN_START_DEVICE);

    // start device allows io if the pnp state is still 'present'.
    // Since the PDO may actually be disconnected at this time we want don't want
    // to revert the state just set when the PDO was disconnected.

    if (GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpStop) {
        nts = STATUS_SUCCESS;
        // exit on double start
        goto double_start;
    }

    UsbhSetPdo_AllowIo(pdoExt->HubFdo, Pdo);

    UsbhDisableTimerObject(pdoExt->HubFdo, &pdoExt->DriverNotFoundTimer);

    // First make sure our parent hub is awake
    bh = UsbhIncHubBusy(pdoExt->HubFdo, PDO_CONTEXT(Pdo), Pdo, UsbhEnablePdo_Tag, RESUME_HUB);

    // Sychronize with the driver not found timer
    WAIT_EVENT(&pdoExt->DriverNotFoundLock);

    do {

        // reset the port if start is a restart, start after soft-remove,
        // or port was disabled due to the driver not found timeout
        nts = UsbhPdoPnp_EnablePdo(Pdo);

        if (GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpRestart ||
            GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpRemove) {

            // Set the driver problem description string to NULL, in case the PDO
            // failed a previous enumeration and still has the problem string assigned.
            UsbhPdo_ResetDriverProblemDesc(Pdo);

            // Device failed enumeration, so just return success.
            if (pdoExt->PdoPnpDeviceState & PNP_DEVICE_FAILED) {
                nts = STATUS_SUCCESS;
                break;
            }
        }

        if (NT_ERROR(nts)) {
            // we will be failing start regardless
            break;
        }

        if (GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpRestart) {
            UsbhAssert(pdoExt->HubFdo, NT_SUCCESS(nts));
            // exit on restart, the structs should already be allocated
            break;
        }

        UsbhFreePool(pdoExt->PnPDeviceDescription);

        // Retreive and store the PnP device description in the PDO extension
        nts = IoGetDeviceProperty(Pdo,
                                  DevicePropertyDeviceDescription,
                                  0,
                                  NULL,
                                  &requiredLength);

        if ((nts == STATUS_BUFFER_TOO_SMALL) && requiredLength) {

            UsbhAllocatePool_Z(deviceDescription, NonPagedPool, requiredLength);

            if(!deviceDescription) {
                break;
            }

            nts = IoGetDeviceProperty(Pdo,
                                      DevicePropertyDeviceDescription,
                                      requiredLength,
                                      deviceDescription,
                                      &requiredLength);

            if (!NT_SUCCESS(nts)) {
                UsbhFreePool(deviceDescription);
                break;
            }

            pdoExt->PnPDeviceDescription = deviceDescription;
            pdoExt->PnPDeviceDescriptionLength = requiredLength;

            UsbhEtwLogDeviceDescription(pdoExt, &USBHUB_ETW_EVENT_DEVICE_START_DEVICE_DESCRIPTION);
        }

        // read some registry keys -- these tell us about such things as the
        // MS os descriptor
        nts = UsbhReadPdoRegistryKeys(pdoExt->HubFdo, Pdo);
        if (NT_ERROR(nts)) {
            UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_START_REGISTRY_FAILURE, nts);
            break;
        }

        // create a symbolic link for the PDO.  USBUI uses this
        // to enumerate the bus from user mode.

        if (pdoExt->PdoFlags.DeviceIsHub) {
            lpGuid = (LPGUID)&GUID_CLASS_USBHUB;
        } else {
            lpGuid = (LPGUID)&GUID_CLASS_USB_DEVICE;
        }

        nts = UsbhPdoCreateSymbolicLink(Pdo, lpGuid);
        if (NT_ERROR(nts)) {
            UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_START_SYMBOLIC_LINK_FAILURE, nts);
            break;
        }

        // successful start, register with WMI
        // WMI requires that the PDO be one that PNP knows about
        UsbhPdoRegisterWmi(Pdo);

    } WHILE (0);

    SET_PDO_SWPNPSTATE(Pdo, PDO_WaitPnpStop, Ev_PDO_IRP_MN_START);

    SET_EVENT(&pdoExt->DriverNotFoundLock);

    UsbhDecHubBusy(pdoExt->HubFdo, PDO_CONTEXT(Pdo), bh);

    // log info about the device
    UsbhPdoSetDeviceData(pdoExt->HubFdo, Pdo, Pdo);

    nts = Usbh__TestPoint__Ulong(pdoExt->HubFdo, TstPt_DevicePdoStart,
                        nts, pdoExt->PortNumber);

    if (NT_SUCCESS(nts) && pdoExt->PdoFlags.HsDeviceRunningAtFs) {
        // generate the WMI notification

        // note that by generating the notification here we
        // won't fire the event if the device is disabled or
        // has no driver.
        HUB_EXCEPTION(USBFILE,  pdoExt->HubFdo, pdoExt->PortNumber, LegacyDevice_Popup,
                             nts, 0, NULL, 0);

    }

double_start:

    // sanity check success path, successful start should have a sym link
    // created.
    if (NT_SUCCESS(nts)) {
        if (!(pdoExt->PdoPnpDeviceState & PNP_DEVICE_FAILED)) {
            UsbhAssert(pdoExt->HubFdo,  pdoExt->PdoFlags.DeviceInterface &&
                                        pdoExt->PdoFlags.SymbolicLink );
        }
    } else {

        HUB_EXCEPTION(USBFILE,  pdoExt->HubFdo, pdoExt->PortNumber, PnpErr_PdoStartFailed,
                                    nts, 0, NULL, 0);
    }

    LOG(pdoExt->HubFdo, LOG_PDO, 'STR<', Irp, nts);
    UsbhEtwLogDeviceIrpEvent(pdoExt, Irp, &USBHUB_ETW_EVENT_DEVICE_START_COMPLETE, 0);
    UsbhCompleteIrp(Irp, nts);

    UsbhUpdateSqmFlags(Pdo);

    return nts;
}


VOID
UsbhPdoRemoveCleanup(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Performs a stop of a Pdo. Since many PnP functions such as _REMOVE and
    _SURPRISE_REMOVAL imply a stop we have a separate function to handle some
    of the common cleanup duties.

    This function can be called in any PnP state and it will do the following:

    1. Complete any outstanding WAKE requests and well as any outstanding IDLE
        requests
    2. Free resources by deleting the device handle if it is valid.
    3. Delete Synbolic link we created for the PDO.
    4. Deregister with WMI.




state                                                   action
PDO_WaitPnpStart      stopping but never started        exit
PDO_WaitPnpRemove     wait on remove after remove       exit
PDO_WaitPnpRestart    wait on start after stop          cleanup
PDO_WaitPnpStop       wait on stop after being started  cleanup


Arguments:


Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_HANDLE dh;

    UsbhAssertPassive();
    pdoExt = PdoExt(Pdo);
    if (GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpStart ||
        GET_PDO_SWPNPSTATE(Pdo) == PDO_WaitPnpRemove) {
        // already stopped
        return;
    }

    // if there is a wake irp still outstanding complete it now. Client cannot
    // re-submit since the fail io flag is set.

    UsbhCompletePdoWakeIrp(pdoExt->HubFdo, Pdo, STATUS_CANCELLED);

    UsbhCompletePdoIdleIrp(pdoExt->HubFdo, pdoExt->Pdo, STATUS_CANCELLED);

    // flush any pending transfers the client may have in the system, if this
    // is a hard-remove then the device handle will have been already removed at
    // this point and client requests will have already been flushed.

    // if this is a soft-remove or surprise remove then the device handle may
    // still valid and this call will flush any transfer on the device handle
    // the function driver may still have pending.

    // ref it before we pass it to usbport
    dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, Pdo, Pdo, Devh_Pdo_Flush_Tag);
    if (dh) {

        Usbh_HubFlushTransfers(pdoExt->HubFdo, dh);

        UsbhDerefPdoDeviceHandle(pdoExt->HubFdo, dh, Pdo, Devh_Pdo_Flush_Tag);
    }

    // undue anything we did in start
    UsbhPdoDeleteSymbolicLink(Pdo);

    if (pdoExt->PdoFlags.WmiRegistered) {
        IoWMIRegistrationControl(Pdo,
                                 WMIREG_ACTION_DEREGISTER);

        pdoExt->PdoFlags.WmiRegistered = 0;
    }

    return;

}


NTSTATUS
UsbhPdoPnp_StopDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP STOP Irp for a Pdo

    IRP_MN_STOP_DEVICE

    Documentation is pretty weak here but our duties are as follows:

    1. Ensure that the device is paused and save volitile state.
    2. Release hardware resources.  In this case this does include the device handle.
    3. In a bus driver, complete the IRP using IoCompleteRequest with IO_NO_INCREMENT .

Arguments:


Return Value:

    nt status

--*/

{
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    USBD_STATUS usbdStatus;
    NTSTATUS cfg_nts;

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhFatalAssert(pdoExt->HubFdo,pdoExt->HubFdo, Pdo);
    UsbhAssert(pdoExt->HubFdo, ioStack->MinorFunction == IRP_MN_STOP_DEVICE);

    LOG(pdoExt->HubFdo, LOG_PDO, 'Spdo', Irp, 0);

    // this path is only hit with explicit testing of rebalance

    SET_PDO_SWPNPSTATE(Pdo, PDO_WaitPnpRestart, Ev_PDO_IRP_MN_STOP);

    // attempt to close the current configuration, drivers genearlly do this.
    // The device will be reset on resetart so we need not worry about it losing
    // its brains.
    cfg_nts = UsbhCloseDeviceConfiguration(pdoExt->HubFdo, Pdo, &usbdStatus);

    LOG(pdoExt->HubFdo, LOG_PDO, 'spd2', Irp, cfg_nts);

    //
    // In all real cases today the function drivers drivers stop submitting requests during a stop
    // but is technically not illegal.
    // if we want to support this we would need to not delete the device handle and update the
    // usbport driver.
    // Currently we rely on the class driver to handle the queueing if necessary.
    //

    // remove any device handle since the controller may be removed, we restore it later on the restart.
    UsbhUnlinkPdoDeviceHandle(pdoExt->HubFdo, Pdo, 'xDH6', TRUE);

    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhPdoPnp_QueryPnpDeviceState(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    return pnp state of the Pdo.

Arguments:

Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_QUERY_PNP_DEVICE_STATE);

    if (pdoExt->PdoPnpDeviceState != 0) {
        DbgTrace((UsbhDebugTrace, "%!FUNC! - PdoPnpDeviceState = %08.8x\n",
            pdoExt->PdoPnpDeviceState));

        if (pdoExt->EnumFailureMsgId != 0) {
            UsbhPdo_ReportPnPFailureProblem(Pdo);
        }

        Irp->IoStatus.Information |= pdoExt->PdoPnpDeviceState;
        nts = Irp->IoStatus.Status = STATUS_SUCCESS;
    } else {
        nts = Irp->IoStatus.Status;
    }

    UsbhCompleteIrp(Irp, nts);

    return nts;

}


NTSTATUS
UsbhPdoPnp_QueryCapabilities(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    The Dreaded DEVICE_CAPABILITIES

    Handle IRP_MN_QUERY_CAPABILITIES on a PDO

Arguments:

    Irp contains:

    Parameters.DeviceCapabilities.Capabilities

    This structure is initialized by the caller. Since we are the bottom owner
    of the PDO we set the appropriate fields and any changes must be made by
    drivers as the irp completes.

Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_CAPABILITIES pdoDevCaps;
    USHORT size, version;

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_QUERY_CAPABILITIES);

    // Get the packet.
    //
    pdoDevCaps = ioStack->
        Parameters.DeviceCapabilities.Capabilities;

    //**
    // Generate the standard device caps for a USB Pdo
    //

    // Size and Version are passed in by the caller we should not modify these,
    // all others we set as appropriate for USB (see DDK)
    //

    size = pdoDevCaps->Size;
    version = pdoDevCaps->Version;

    // init the packet to all zeros
    UsbhAssert(pdoExt->HubFdo, size >= sizeof(struct _DEVICE_CAPABILITIES));
    RtlZeroMemory(pdoDevCaps, sizeof(struct _DEVICE_CAPABILITIES));

    pdoDevCaps->Size = size;
    pdoDevCaps->Version = version;

    // check for embedded devices
    if (pdoExt->PortAttributes.DeviceNotRemovable) {
        pdoDevCaps->Removable = 0;
    } else {
        pdoDevCaps->Removable = 1;
    }

    // unique id means we have a serial number
    pdoDevCaps->UniqueID = pdoExt->PdoFlags.SerialNumber ? 1: 0;

    // default is FALSE it is up to the FDO to change the value to TRUE
    // reporting false gets you the tray icon
    pdoDevCaps->SurpriseRemovalOK = 0;

    pdoDevCaps->Address = pdoExt->PortNumber;
    pdoDevCaps->UINumber = (ULONG) -1;

    // power related fields

    //
    // Take system wake from the host controller
    //
    // WinSE:422036-If UsbhCreatePdo() gets called before UsbhQueryCapsComplete;
    // SystemWake can be PowerSystemUnspecified. So get the FDOs SystemWake again
    //
    if (pdoExt->SystemWake == PowerSystemUnspecified &&
        pdoExt->HubFdo != NULL &&
        pdoExt->PdoFlags.HubRemoved == 0) {

        PDEVICE_EXTENSION_HUB hubFdoExt;

        hubFdoExt = FdoExt(pdoExt->HubFdo);
        pdoExt->SystemWake = hubFdoExt->DeviceCapabilities.SystemWake;
    }

    pdoDevCaps->SystemWake = pdoExt->SystemWake;

    // these are the caps for USB
    pdoDevCaps->DeviceState[PowerSystemWorking] = PowerDeviceD0;

    if (pdoExt->PdoFlags.UsbWakeupSupported) {
        SYSTEM_POWER_STATE ps;

        // caps if the USB device supports wake thru the
        // the BUS.

        pdoDevCaps->DeviceWake = PowerDeviceD2;

        pdoDevCaps->WakeFromD0 = 1;
        pdoDevCaps->WakeFromD1 = 1;
        pdoDevCaps->WakeFromD2 = 1;
        pdoDevCaps->WakeFromD3 = 0;

        pdoDevCaps->D1Latency = 0;
        pdoDevCaps->D2Latency = 0;
        pdoDevCaps->D3Latency = 0;

        pdoDevCaps->DeviceD1 = 1;
        pdoDevCaps->DeviceD2 = 1;

        for (ps=PowerSystemSleeping1; ps<=PowerSystemShutdown; ps++) {
            if (ps > pdoDevCaps->SystemWake) {
                pdoDevCaps->DeviceState[ps] = PowerDeviceD3;
            } else {
                pdoDevCaps->DeviceState[ps] = PowerDeviceD2;
            }
        }
    } else {
        SYSTEM_POWER_STATE ps;

        // device doesn't support wake

        pdoDevCaps->DeviceWake = PowerDeviceD0;

        pdoDevCaps->WakeFromD0 = 1;
        pdoDevCaps->WakeFromD1 = 0;
        pdoDevCaps->WakeFromD2 = 0;
        pdoDevCaps->WakeFromD3 = 0;

        pdoDevCaps->D1Latency = 0;
        pdoDevCaps->D2Latency = 0;
        pdoDevCaps->D3Latency = 0;

        pdoDevCaps->DeviceD1 = 0;
        pdoDevCaps->DeviceD2 = 0;

        for (ps=PowerSystemSleeping1; ps<=PowerSystemShutdown; ps++) {
            if (ps > pdoDevCaps->SystemWake) {
                pdoDevCaps->DeviceState[ps] = PowerDeviceD3;
            } else {
                pdoDevCaps->DeviceState[ps] = PowerDeviceD2;
            }
        }

    }

    if (pdoExt->PdoPnpDeviceState & PNP_DEVICE_FAILED) {
        // Setting RawDeviceOK will allow devices that failed enumeration and loaded the NULL
        // driver to still start.  This is neccessary so the device can be invalidated via
        // IoInvalidateDeviceState.
        pdoDevCaps->RawDeviceOK = TRUE;
    }

    nts = STATUS_SUCCESS;

    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoPnp_QueryDeviceRelations(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Handle IRP_MN_QUERY_DEVICE_RELATIONS on a PDO

Arguments:


Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_QUERY_DEVICE_RELATIONS);

    // return original status by default
    nts = Irp->IoStatus.Status;

    if (ioStack->Parameters.QueryDeviceRelations.Type ==
            TargetDeviceRelation) {

        PDEVICE_RELATIONS deviceRelations = NULL;

        deviceRelations = ExAllocatePoolWithTag(PagedPool,
            sizeof(*deviceRelations), USBHUB_HEAP_TAG);

        if (deviceRelations == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
        } else {
            ObReferenceObject(Pdo);
            deviceRelations->Count = 1;
            deviceRelations->Objects[0] = Pdo;
            nts = STATUS_SUCCESS;
        }

        Irp->IoStatus.Information =(ULONG_PTR) deviceRelations;
    }

    UsbhCompleteIrp(Irp, nts);

    return nts;

}


NTSTATUS
UsbhPdoPnp_EnablePdo(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Checks if the PDO is currently disabled and re-enables it if needed

    A PDO's port can be disabled if the device is disabled via PnP, is being re-balanced, or
    if a driver was not found for the device.

    NOTE: Called with the DriverNotFoundLock held.

Arguments:

--*/

{
    PDEVICE_OBJECT hubFdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    HUB_PDO_SWPNP_STATE currentPdoSwPnpState;

    pdoExt = PdoExt(Pdo);
    hubFdo = pdoExt->HubFdo;

    nts = STATUS_SUCCESS;

    currentPdoSwPnpState = GET_PDO_SWPNPSTATE(Pdo);

    // Do nothing if the port is not disabled and the PDO hasn't started yet, or it failed enumeration
    if (((pdoExt->PdoFlags.PortDisabled == 0) &&
         (currentPdoSwPnpState == PDO_WaitPnpStart)) ||
        (pdoExt->PdoPnpDeviceState & PNP_DEVICE_FAILED)) {

        return STATUS_SUCCESS;
    }

    if (currentPdoSwPnpState == PDO_WaitPnpRemove ||
        currentPdoSwPnpState == PDO_WaitPnpRestart ||
        currentPdoSwPnpState == PDO_WaitPnpStart) {

        // the port is no longer idle
        UsbhSshSetPortsBusyState(hubFdo, pdoExt->PortNumber, PORT_BUSY, NO_RESUME_HUB);

        nts = UsbhSyncResetDeviceInternal(hubFdo, PDO_CONTEXT(Pdo), Pdo);

        if (NT_ERROR(nts) && (!Usb_Disconnected(nts))) {
            HUB_EXCEPTION(USBFILE, hubFdo, pdoExt->PortNumber, PnpErr_FailedResetOnStart,
                            nts, 0xFFFFFFFF, NULL, 0);
        } else {
            pdoExt->PdoFlags.PortDisabled = 0;
            UsbhSetPdo_AllowIo(hubFdo, Pdo);
        }

    }

    return nts;
}


NTSTATUS
UsbhPdoPnp_QueryId(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Query Id for a Pdo

    id are already formed and stored in the extension, we just need to return
    them.

    IRP_MN_QUERY_ID

Arguments:


Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PUSB_ID_STRING idString = NULL;
    size_t decorateLength = 0;

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_ID);

    // return original status by default
    nts = Irp->IoStatus.Status;

    //
    // It is possible to update our IDs after the PDO is created when we switch
    // from between a VM and the host IDs.  We must wait for any pending ID updates
    // to complete.
    //

    UsbhAcquirePdoUxdLock(pdoExt->HubFdo, Pdo);

    switch (ioStack->Parameters.QueryId.IdType) {
    case BusQueryDeviceID:
        // We will disable a PDO's port if we do not recieve a start IRP for it within 10 seconds
        // of it being enumerated or PnP disabled.  We want to insure that I/O is enabled for the
        // device prior to receiving the start IRP, as some client drivers may send I/O before the
        // PDO has received the start IRP.  Based on feedback from the PnP team, we can use
        // IRP_MN_QUERY_ID as an indicator that the device is being started due to a driver being
        // found or the device being re-enabled.


        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
            PSSH_BUSY_HANDLE bh;

            // First make sure our parent hub is awake
            bh = UsbhIncHubBusy(pdoExt->HubFdo, PDO_CONTEXT(Pdo), Pdo, UsbhEnablePdo_Tag, RESUME_HUB);

            // Sychronize with the driver not found timer
            WAIT_EVENT(&pdoExt->DriverNotFoundLock);

            // enable the PDO's port
            UsbhPdoPnp_EnablePdo(Pdo);

            SET_EVENT(&pdoExt->DriverNotFoundLock);

            UsbhDecHubBusy(pdoExt->HubFdo, PDO_CONTEXT(Pdo), bh);

        }

        idString = &pdoExt->PnpDeviceId;
        break;

    case BusQueryHardwareIDs:
        idString = &pdoExt->PnpHardwareId;
        break;

    case BusQueryCompatibleIDs:
        idString = &pdoExt->PnpCompatibleId;
        break;

    case BusQueryInstanceID:
        if (pdoExt->SerialNumberId.Buffer) {
            UsbhAssert(NULL, pdoExt->PdoFlags.EnumerationFailed == 0);
            idString = &pdoExt->SerialNumberId;
            if (pdoExt->PdoFlags.DecorateSerialNumber == 1) {
                decorateLength = USB_SERIAL_NUMBER_DECORATION_SIZE;
            }
        } else {
            idString = &pdoExt->UniqueId;
        }
        break;

    case BusQueryContainerID:
        if (pdoExt->ContainerIDValid) {
            UNICODE_STRING ContainerIDString;

            nts = RtlStringFromGUID(&pdoExt->ContainerID, &ContainerIDString);
            if (NT_SUCCESS(nts)) {
                Irp->IoStatus.Information = (ULONG_PTR) ContainerIDString.Buffer;
                nts = STATUS_SUCCESS;
            }
        }
        break;
    }

    // attempt to return the id
    if (idString &&
        idString->LengthInBytes &&
        idString->Buffer) {
        PUCHAR id;

        UsbhAllocatePool_Z(id, PagedPool, idString->LengthInBytes + decorateLength);

        if (id) {
            if (decorateLength) {
                RtlCopyMemory(id,
                              USB_SERIAL_NUMBER_DECORATION,
                              USB_SERIAL_NUMBER_DECORATION_SIZE);
            }

            RtlCopyMemory(id+decorateLength,
                          idString->Buffer,
                          idString->LengthInBytes);

            Irp->IoStatus.Information = (ULONG_PTR) id;
            nts = STATUS_SUCCESS;
        } else {
            nts = STATUS_INSUFFICIENT_RESOURCES;
        }

    } else if (ioStack->Parameters.QueryId.IdType == BusQueryDeviceID) {
        // The only valid error status for BusQueryDeviceID is insufficient resources
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    UsbhReleasePdoUxdLock(pdoExt->HubFdo, Pdo);

    DbgTrace((UsbhDebugTrace, "QueryId - %!STATUS!\n", nts));
    UsbhCompleteIrp(Irp, nts);

    return nts;

}


NTSTATUS
UsbhPdoPnp_QueryDeviceText(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Returns the device text strings to PnP.  We use the string descriptors
    embedded in the device for this.

    The logic used in previous implementation was:

    If device has no product string or serial numbers disabled
        return STATUS_NOT_SUPPORTED
    else
        check list of supported languages
        If language supported
            get the string
            if failure
                set to English and Retry
            else
                return string
        else
            set language to English and Retry

Arguments:

    Irp contains:

    Parameters.QueryDeviceText.DeviceTextType
    Parameters.QueryDeviceText.LocaleId


Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    DEVICE_TEXT_TYPE textType;
    LANGID languageId;
    PUSB_ID_STRING idString = NULL;

    DbgTrace((UsbhDebugTrace, "UsbhPdoPnp_QueryDeviceText PDO %p\n", Pdo));

    pdoExt = PdoExt(Pdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_QUERY_DEVICE_TEXT);

    textType = ioStack->Parameters.QueryDeviceText.DeviceTextType;
    languageId = LANGIDFROMLCID(ioStack->Parameters.QueryDeviceText.LocaleId);

    DbgTrace((UsbhDebugTrace, "QueryDeviceText %p textType %x languageId 0x%04.4x\n", Pdo,
        textType, (USHORT)languageId));

    // old hub did this -- probably for WIn9x
    if (languageId == 0) {
        // map 0 to English
        languageId = 0x409;
    }

    // default to status passed in
    nts = Irp->IoStatus.Status;

    switch(textType) {
    case DeviceTextLocationInformation:
        DbgTrace((UsbhDebugTrace, "DeviceTextLocationInformation\n"));

        if (pdoExt->TextLocationIdString.LengthInBytes) {
            idString = &pdoExt->TextLocationIdString;
            nts = STATUS_SUCCESS;
        } else {
            nts = STATUS_NOT_SUPPORTED;
        }
        break;

    case DeviceTextDescription:
        DbgTrace((UsbhDebugTrace, "DeviceTextDescription\n"));
        if (pdoExt->DeviceDescriptor.iProduct == 0 ||
            pdoExt->DeviceFlags.DisableSerialNumber) {
            // see if a string is specified in the registry
            if (pdoExt->GenericDevString.LengthInBytes) {
                idString = &pdoExt->GenericDevString;
                nts = STATUS_SUCCESS;
            } else {
                nts = STATUS_NOT_SUPPORTED;
            }
        } else {
            nts = UsbhGetProductIdString(pdoExt->HubFdo,
                                         Pdo,
                                         languageId,
                                         &pdoExt->ProductId);

            if (nts == STATUS_NOT_SUPPORTED && languageId != 0x409) {
                // no language string, try for english
                nts = UsbhGetProductIdString(pdoExt->HubFdo,
                                             pdoExt->Pdo,
                                             0x409,
                                             &pdoExt->ProductId);
            }

            // device reported a product string but we could not fetch it
            // treat this as an exception
            // note: we will throw this exception is the device has product id
            // strings but none is in english or none are supported.
            if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {

                HUB_EXCEPTION(USBFILE,  pdoExt->HubFdo, pdoExt->PortNumber,
                            FailedProductIdString,
                            nts, 0, NULL, 0);

            }

            if (NT_SUCCESS(nts)) {
                idString = &pdoExt->ProductId;
            }

        }

    }

    if (NT_SUCCESS(nts) && idString ) {
        PWSTR id = NULL;

        UsbhAssert(pdoExt->HubFdo, idString->LengthInBytes);
        UsbhAssert(pdoExt->HubFdo, idString->Buffer);
        UsbhAllocatePool_Z(id, PagedPool, idString->LengthInBytes)
        if (id &&
            idString->LengthInBytes &&
            idString->Buffer) {
            RtlCopyMemory(id,
                          idString->Buffer,
                          idString->LengthInBytes);
            Irp->IoStatus.Information = (ULONG_PTR) id;

            DbgTrace((UsbhDebugTrace, "QueryDeviceText id = :%ws:\n", id));
        } else {
            UsbhFreePool(id);
            nts = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    DbgTrace((UsbhDebugTrace, "QueryDeviceText - %!STATUS!\n", nts));
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoPnp_DeviceUsageNotification(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    IRP_MN_DEVICE_USAGE_NOTIFICATIOM

Arguments:


Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    DEVICE_USAGE_NOTIFICATION_TYPE dunType;
    BOOLEAN inPath;
    BOOLEAN isDeviceExternal;

    pdoExt = PdoExt(Pdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_DEVICE_USAGE_NOTIFICATION);
    dunType = ioStack->Parameters.UsageNotification.Type;
    inPath = ioStack->Parameters.UsageNotification.InPath;
    nts = STATUS_SUCCESS;

    isDeviceExternal = UsbhPdo_IsDeviceExternal(Pdo);

    // We do not want to block page file creation for WinPE. Page file is created
    // by setup running in WinPE on the non-boot target disk. Failing the creation
    // of page file resulted in setup failing with E_OUTOFMEMORY error
    //
    if (!NT_SUCCESS(RtlCheckRegistryKey(RTL_REGISTRY_CONTROL, WINPE_KEY))) {

        // In Non-WinPE we will only create a pagefile if the following
        // condition is met
        // Disk is internal OR Disk is a Boot disk
        //
        if (isDeviceExternal &&
            DunTypePaging(dunType) &&
            pdoExt->IsBootDevice == FALSE) {

            nts = STATUS_NOT_SUPPORTED;
            UsbhCompleteIrp(Irp, nts);
        }
    }

    if (NT_SUCCESS(nts) && DunTypeRecognized(dunType)) {

        // Pass it directly to the parent FDO
        //
        PIRP irp2;
        PIO_STACK_LOCATION ioStack2;
        PDEVICE_OBJECT deviceObject;
        KEVENT event;

        // Get the top of the FDO stack
        deviceObject = IoGetAttachedDevice(pdoExt->HubFdo);

        // Allocate and initialize a new irp for this operation.
        //
        irp2 = IoAllocateIrp(deviceObject->StackSize, FALSE);
        if (!irp2) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(nts)) {
            // Fill in the service independent parameters in the IRP.
            irp2->IoStatus.Status = STATUS_NOT_SUPPORTED;
            KeInitializeEvent(&event, NotificationEvent, FALSE);
            IoSetCompletionRoutine(irp2,
                                   UsbhDeferIrpCompletion,
                                   &event,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            // Duplicate the original function codes and parameters.
            ioStack2 = IoGetNextIrpStackLocation(irp2);
            ioStack2->MajorFunction = ioStack->MajorFunction;
            ioStack2->MinorFunction = ioStack->MinorFunction;
            ioStack2->FileObject = ioStack->FileObject;
            ioStack2->Parameters.UsageNotification.InPath = inPath;
            ioStack2->Parameters.UsageNotification.Type = dunType;

            // Now send irp2 down the stack synchronously
            //
            if (IoCallDriver(deviceObject, irp2) == STATUS_PENDING) {

                KeWaitForSingleObject(&event,
                                      Suspended,
                                      KernelMode,
                                      FALSE,
                                      NULL);
            }

            nts = irp2->IoStatus.Status;

            // Free irp2
            //
            IoFreeIrp(irp2);
        }

        if (NT_SUCCESS(nts) && DunTypeSpecialFile(dunType)) {

            // The lower stack has processed the IRP successfully
            //

            IoAdjustPagingPathCount((PLONG)&pdoExt->InPathCount, inPath);
            if (dunType == DeviceUsageTypeHibernation) {
                IoAdjustPagingPathCount((PLONG)&pdoExt->HibernateCount, inPath);
            }

            if (pdoExt->InPathCount) {
                Pdo->Flags &= ~DO_POWER_PAGABLE;
            } else {
                Pdo->Flags |= DO_POWER_PAGABLE;
            }
        }

        if ((NT_SUCCESS(nts) && DunTypePaging(dunType) &&
             ioStack->Parameters.UsageNotification.InPath) ||
             DunTypeOnBootPath(dunType)) {

            // We do not want to do anything special for WinPE
            //
            if (!NT_SUCCESS(RtlCheckRegistryKey(RTL_REGISTRY_CONTROL, WINPE_KEY))) {

                // We register for boot device notification only if the following
                // conditions are met and we have not registered before
                // Device is External and it is a boot or paging disk
                //
                if (pdoExt->IsBootDevice == FALSE) {

                    if (isDeviceExternal == TRUE) {
                        NTSTATUS nts2;
                        nts2 = UsbhRegisterBootDeviceNotification(Pdo,
                                                                  UsbhPdoCheckBootDeviceReady,
                                                                  &pdoExt->ExBootDeviceHandle);
                        LOG(pdoExt->HubFdo, LOG_PDO, 'URBD', nts2, Pdo);
                    }
                    // We want to support hub reset mitigation even if the Pdo
                    // is internal
                    //
                    pdoExt->IsBootDevice = TRUE;

                }
            }
        }

        UsbhCompleteIrp(Irp, nts);
    } else if (!DunTypeRecognized(dunType)) {

        // Not interested, skip it
        IoSkipCurrentIrpStackLocation(Irp);
        nts = IoCallDriver(pdoExt->HubFdo, Irp);
    }

    if ((pdoExt->InPathCount != 0) && (pdoExt->SqmProperties.HostSpecialFiles == 0)) {
        pdoExt->SqmProperties.HostSpecialFiles = 1;
        UsbhUpdateSqmFlags(Pdo);
    }

    DbgTrace((UsbhDebugTrace, "DeviceUsageNotification - %!STATUS!\n", nts));

    return nts;
}

NTSTATUS
UsbhPdoPnp_QueryBusInfo(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    IRP_MN_QUERY_BUS_INFORMATION

    return the standard USB GUID

Arguments:


Return Value:

    nt status

--*/
{
    PPNP_BUS_INFORMATION busInfo;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(NULL, ioStack->MinorFunction == IRP_MN_QUERY_BUS_INFORMATION);

    UsbhAllocatePool_Z(busInfo, PagedPool, sizeof(PNP_BUS_INFORMATION));

    if (busInfo == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    } else {
        busInfo->BusTypeGuid = GUID_BUS_TYPE_USB;
        busInfo->LegacyBusType = PNPBus;
        busInfo->BusNumber = 0;
        Irp->IoStatus.Information = (ULONG_PTR) busInfo;
        nts = STATUS_SUCCESS;
    }

    DbgTrace((UsbhDebugTrace, "QueryBusInfo - %!STATUS!\n", nts));
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhPdoPnp_Success(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    Complete Irp with success

Arguments:


Return Value:

    nt status

--*/
{
     UsbhCompleteIrp(Irp, STATUS_SUCCESS);

     return STATUS_SUCCESS;
}


NTSTATUS
UsbhPdoPnp_SurpriseRemoveDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP remove for a Pdo

    IRP_MN_SURPRISE_REMOVAL

    Our official duties here are as follows:

    1. Prevent any new I/O to the device.
    2. Fail outstanding I/O requests on the device.
    3. Continue to pass down any IRPs that the driver does not handle for the device.
    4. In a parent bus driver, power down the bus slot if the driver is capable of doing so
        (not clear what this means for USB but it is along the lines of the PdoSurpriseRemoveEvent that
        stops any eneumration actibity and disables the port).
    5. Disable device interfaces with IoSetDeviceInterfaceState.
    6. Release the device's hardware resources (this means free the device handle)
    7. (optional - can wait for remove)Clean up any device-specific allocations, memory, events, etc.
    8. Leave the device object attached to the device stack.


    Set Irp->IoStatus.Status to STATUS_SUCCESS.
    Complete the IRP (IoCompleteRequest) with IO_NO_INCREMENT.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION ioStack;
    PSTATE_CONTEXT sc;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pdoExt = PdoExt(Pdo);
    sc = &pdoExt->PdoStateContext;

    LOG(pdoExt->HubFdo, LOG_PDO, 'srpd', Irp, pdoExt->DeviceHandle);
    LOG(pdoExt->HubFdo, LOG_PDO, 'srp1', Pdo, pdoExt);

    UsbhAssert(pdoExt->HubFdo,
        ioStack->MinorFunction == IRP_MN_SURPRISE_REMOVAL);

    // no more io will be processed for this Pdo,
    // start failing as soon as we possibly can
    UsbhSetPdo_FailIo(Pdo);

    // Unblock D0 for D3Cold device in case it is disconnected
    if (pdoExt->PdoFlags.D3ColdEnabled == 1) {
         SET_EVENT(&pdoExt->D3ColdReconnectEvent);
    }

    if (pdoExt->DeviceFlags.DualRoleDevice == 1) {
        WnfPublishUsbPartnerDualRoleFeatures(
            pdoExt->UcmConnectorId,
            FALSE,
            0L);
        pdoExt->DeviceFlags.DualRoleDevice = 0;
    }

    if (pdoExt->BillboardDescriptor != NULL) {
        UsbhBillboardCleanup(Pdo);
    }

    //
    // device may still be valid at this point. This happens if PNP is
    // surprise removing the device as a result of the hub being surprise
    // removed.
    // In this case PnP thinks the device is gone so it is OK to clean up our
    // own references to it as well.
    //

    // perform cleanup for remove
    UsbhPdoRemoveCleanup(Pdo);

    Usbh_PdoSurpriseRemove_PdoEvent(pdoExt->HubFdo, Pdo, sc);

    SET_PDO_SWPNPSTATE(Pdo, PDO_WaitPnpRemove, Ev_PDO_IRP_MN_S_REMOVE);

    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;

}

NTSTATUS
UsbhPdoPnp_RemoveDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP remove for a Pdo

    IRP_MN_REMOVE_DEVICE

    This function processes the soft part of the remove.  Our duties are as follows:


    1. Prevent any new I/O to the device.
    2. Fail outstanding I/O requests on the device.
    3. Continue to pass down any IRPs that the driver does not handle for the device.
    4. In a parent bus driver, power down the bus slot if the driver is capable of doing so
       (not clear what this means for USB but it is along the lines of the PdoSurpriseRemoveEvent that
       stops any eneumration actibity and disables the port).
    5. Disable device interfaces with IoSetDeviceInterfaceState.
    6. Release the device's hardware resources (this means free the device handle)
    7. (optional - can wait for remove)Clean up any device-specific allocations, memory, events, etc.

    if the device is no longer physically [present on the bus we must delete the PDO and free any memory
    structures assocaited with it that may not already be free.

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_OBJECT HubFdo;
    PSTATE_CONTEXT sc = NULL;
    HUB_PDO_HWPNP_STATE curState;
    PSSH_BUSY_HANDLE bh = NULL;

    curState = GET_PDO_HWPNPSTATE(Pdo);

    pdoExt = PdoExt(Pdo);

    if (pdoExt->PdoFlags.HubRemoved == 0) {
        HubFdo = pdoExt->HubFdo;
    } else {
        HubFdo = NULL;
    }

    // Check if PDO has already been removed
    if (curState != Pdo_Deleted) {

        sc = &pdoExt->PdoStateContext;

        if (pdoExt->DeviceFlags.DualRoleDevice == 1) {
            WnfPublishUsbPartnerDualRoleFeatures(
                pdoExt->UcmConnectorId,
                FALSE,
                0L);
            pdoExt->DeviceFlags.DualRoleDevice = 0;
        }

        // Make sure the hub is still present
        if (HubFdo) {

            // no more io will be processed for this Pdo
            UsbhSetPdo_FailIo(Pdo);

            // It's possible that the host controller was selectively suspended prior
            // to deleting the device handle.  Deleting the device handle involves
            // removing endpoints from the controller schedule, so resume the hub if we
            // have a valid device handle.  Also resume the hub if the PDO is not in D0
            // so it can be returned to D0.
            if ((pdoExt->DeviceHandleState == PdoDevH_Valid) ||
                (GET_PDO_DX(Pdo) != PowerDeviceD0)) {
                bh = UsbhIncHubBusy(HubFdo, sc, Pdo, UsbhBusySoftRemove_Tag, RESUME_HUB);
            }

            // implied transition back to D0
            if (GET_PDO_DX(Pdo) != PowerDeviceD0) {
                UsbhSyncResumeDeviceInternal(HubFdo, sc, Pdo);

                UsbhAcquireFdoPwrLock(HubFdo, sc, Ev_PDO_REMOVE_DEVICE, PdoRemoveDevice_Tag);
                UsbhSetPdoPowerState( sc,  Pdo, 0, Pdo_D0, PdoPwr_Ev_RemoveInDx);
                UsbhReleaseFdoPwrLock(HubFdo, sc);
            }

            // cleanup for remove
            UsbhPdoRemoveCleanup(Pdo);
        }

        SET_PDO_SWPNPSTATE(Pdo, PDO_WaitPnpRemove, Ev_PDO_IRP_MN_REMOVE);

        Usbh_PdoRemove_PdoEvent(HubFdo, Pdo, sc);

        if (bh) {
           UsbhDecHubBusy(HubFdo, sc, bh);
        }
    }

    // we cannot fail remove
    // irp cannot be NULL
    UsbhFatalAssert(HubFdo,Irp != NULL, Pdo);

    UsbhCompleteIrp(Irp, STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


VOID
UsbhDeletePdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_STATE PdoState,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Delete a PDO for a device that is physically gone from the bus and that PnP
    has no reference to.

    Undue anything we did in create. Put all delete handling in one place.

Arguments:

Return Value:


--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_EXTENSION_PDO  pdoExt;
    PHGSL lock = NULL;
    BOOLEAN IdlePort=FALSE;

    LOG(HubFdo, LOG_PDO, 'dPDO', Pdo, PdoState);

    DbgTrace((UsbhDebugTrace, "Deleting PDO %p\n", Pdo));

    pdoExt = PdoExt(Pdo);

    if (HubFdo) {
        hubExt = FdoExt(HubFdo);

        lock = &StateContext->gLock;
        UsbhAssert(HubFdo,lock->Locked);
        UsbhAssert(HubFdo, pdoExt->CleanupFlags.IoDeleted == 0);
    } else {
        // Hub has been removed before the PDO was removed
        UsbhDeleteOrphanPdo(Pdo);
    }

    // this routine should only be called once
    if (PdoState == Pdo_Deleted) {
        // double delete?
        UsbhAssertFailure(HubFdo, Pdo);
        UsbhReleasePdoStateLock(HubFdo, StateContext);
        return;
    }
    else if (PdoState != Pdo_Created) {
        // Log device removal to ETW if the PDO was past the initial created state
        UsbhEtwLogDeviceInformation(pdoExt, &USBHUB_ETW_EVENT_DEVICE_REMOVE, FALSE);
    }

    if (pdoExt->PdoListLink.Flink || pdoExt->PdoListLink.Blink) {

        // remove from the 'PDO present list'
        RemoveEntryList(&pdoExt->PdoListLink);
        pdoExt->PdoListLink.Flink = NULL;
        pdoExt->PdoListLink.Blink = NULL;

    }

    if (pdoExt->CleanupFlags.ResetPortData) {
        // PortPata->Pdo = NULL
        UsbhResetPortData(HubFdo, Pdo, lock);
        IdlePort = TRUE;
    }

    SET_PDO_HWPNPSTATE(Pdo, Pdo_Deleted,  PnpEvent);

    UsbhReleasePdoStateLock(HubFdo, StateContext);

    if (IdlePort) {
        UsbhSshSetPortsBusyState(HubFdo, pdoExt->PortNumber, PORT_IDLE, NO_RESUME_HUB);
    }

    //UsbhSignalDriverResetEvent(HubFdo);

    if (HubFdo) {
        UsbhDisableTimerObject(HubFdo, &pdoExt->DriverNotFoundTimer);
    }

    WAIT_EVENT(&pdoExt->LatchEvent);

    if (pdoExt->BillboardDescriptor != NULL) {
        UsbhBillboardCleanup(Pdo);
    }

    UsbhFreePool(pdoExt->DeviceUsbRegPath);
    UsbhFreePool(pdoExt->PnPDeviceDescription);
    UsbhFreePool(pdoExt->MsOs20Info.DescriptorSet);
    UsbhFreePool(pdoExt->CachedConfigDescriptor);
    UsbhFreePool(pdoExt->BosDescriptor);
    UsbhFreePool(pdoExt->MsExtConfigDesc);
    UsbhFreePool(pdoExt->FailData);


    UsbhAssertPassive();

    //
    // make sure we don't leak a handle
    //
    // if the user removes a chain of devices ie hub with attached devices then
    // we can see the remove message before any notification of a disconnect.
    // In this case we free the device handle here, when we process the remove.

    UsbhUnlinkPdoDeviceHandle(HubFdo, Pdo, 'xDH7', TRUE);

    LOG(HubFdo, LOG_PDO, 'dHD1', Pdo, 0);

    if (pdoExt->CleanupFlags.WaitPdoIo) {
        // wait for any outstanding IO on this PDO
        UsbhDecPdoIoCount(Pdo, Pdo);
        WAIT_EVENT(&pdoExt->IoRemoveEvent);
    }

    if (pdoExt->EnumFailedBh) {
        UsbhDecHubBusy(HubFdo, StateContext, pdoExt->EnumFailedBh);
        pdoExt->EnumFailedBh = NULL;
    }

    if (pdoExt->CleanupFlags.DisconnectFromHub) {
        UsbhDisconnectPdoFromHub(Pdo);
    }

    pdoExt->CleanupFlags.ul = 0;

    pdoExt->CleanupFlags.IoDeleted = 1;
    UsbhRemoveAndDeletePdo(Pdo);
}


NTSTATUS
UsbhCreatePdo(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Attempts to create a PDO for a child device and binds it to
    the hub if successful.

    The PDO is attached to the port data structure is not reported to PnP yet.

Arguments:


Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    PDEVICE_OBJECT pdo = NULL;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG nameIndex = 0;
    UNICODE_STRING pdoNameUnicodeString;
    UNICODE_STRING uniqueIdUnicodeString;
    PWCHAR uniqueIdString;
    extern PDRIVER_OBJECT UsbhDriverObject;

    LOG(HubFdo, LOG_PDO, 'cPDO', PortData, 0);
    DbgTrace((UsbhDebugTrace,"UsbhCreatePdo\n"));

    hubExt = FdoExt(HubFdo);

    // create the PDO by iterating thru the possible names until we succeed or
    // get an error other than STATUS_OBJECT_NAME_COLLISION
    do {
        nts = UsbhMakePdoName(HubFdo,
                              &pdoNameUnicodeString,
                              nameIndex);

        if (NT_SUCCESS(nts)) {
            nts = IoCreateDevice(UsbhDriverObject,
                                  sizeof(DEVICE_EXTENSION_PDO),
                                  &pdoNameUnicodeString,
                                  FILE_DEVICE_UNKNOWN,
                                  0,
                                  FALSE,
                                  &pdo);

            if (NT_SUCCESS(nts)) {
                break;
            }

            RtlFreeUnicodeString(&pdoNameUnicodeString);
        }
        nameIndex++;

    } while (nts == STATUS_OBJECT_NAME_COLLISION);

    LOG(HubFdo, LOG_PDO, 'cpd1', PortData, nts);

    // if for some reason this api fails to create a pdo but no error is returned
    if (pdo == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_ERROR(nts)) {
        return nts;
    }

    UsbhAssert(HubFdo, pdo);
    // adjust stack size to take into account passing requests thru to the
    // bus plus any filters.  HcdTopOfStack is the top of the root hub PDO
    // stack, since we will be forwarding the irp to this stack we use its
    // stacksize +1 for our own PDO.
    pdo->StackSize = hubExt->TopOfStackDeviceObject->StackSize+1;

    pdo->Flags |= DO_POWER_PAGABLE;

    LOG(HubFdo, LOG_PDO, 'cPD1', PortData, pdo);

    pdoExt = pdo->DeviceExtension;
    pdoExt->ExtensionType = EXTENSION_TYPE_PDO;
    pdoExt->PhysicalDeviceObjectName = pdoNameUnicodeString;
    pdoExt->DeviceHandle = UsbhInvalidDeviceHandle;
    pdoExt->DeviceHandleState = PdoDevH_Init;
    pdoExt->WakeIrpPending = -1;
    pdoExt->PortAttributes.ul = hubExt->ExtendedPortAttributes[PortData->PortNumber].ul;
    pdoExt->IoState = Pdo_AllowIo;
    pdoExt->VbusState = Vbus_On;
    pdoExt->D3ColdReconnectTimeout = D3COLD_RECONNECT_TIMEOUT;
    // initial state
    SET_PDO_HWPNPSTATE(pdo, Pdo_Created,  PdoEv_CreatePdo);

    UsbhInitStateCtx(HubFdo, &pdoExt->PdoStateContext, HubCtx_Pdo, pdo);
    pdoExt->PdoStateContext.Parent = pdo;

    KeQuerySystemTime(&pdoExt->Stat_PdoEnumeratedAt);

    WHILE_TRUE {

        HGSL lock;

        INIT_EVENT_SIGNALED(&pdoExt->DeviceD0Event);
        INIT_EVENT_SIGNALED(&pdoExt->D3ColdReconnectEvent);

        INIT_EVENT_NOT_SIGNALED(&pdoExt->IoRemoveEvent);
        InitializeListHead(&pdoExt->IoList);

        INIT_EVENT_SIGNALED(&pdoExt->LatchEvent);
        InitializeListHead(&pdoExt->LatchListHead);

        INIT_SYNC_EVENT_SIGNALED(&pdoExt->IdleIrpCancelLock);

        INIT_SYNC_EVENT_SIGNALED(&pdoExt->D3ColdLock);

        INIT_SYNC_EVENT_SIGNALED(&pdoExt->DriverNotFoundLock);

        KeInitializeSpinLock(&pdoExt->GlobalListStateLock);

        INIT_SYNC_EVENT_NOT_SIGNALED(&pdoExt->BootDeviceArrivalEvent);

        // DNF timer needs to be initialized prior to any memory allocations
        UsbhInitializeTimerObject(HubFdo,
                                  UsbhDriverNotFoundTimer,
                                  &pdoExt->DriverNotFoundTimer,
                                  'DNFt');

        // build a unique Id string from the device extension and port number,
        // we'll need this to report the pdo to pnp
        UsbhAllocatePool_Z(uniqueIdString, NonPagedPool, 16);

        if (uniqueIdString == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlInitUnicodeString(&uniqueIdUnicodeString,
                             uniqueIdString);
        uniqueIdUnicodeString.MaximumLength = 16;

        nts = RtlIntegerToUnicodeString((ULONG) PortData->PortNumber,
                                        10,
                                        &uniqueIdUnicodeString);

        if (NT_ERROR(nts) && uniqueIdString) {
            UsbhFreePool(uniqueIdString);
            break;
        }

        // this will now be cleaned up in deletepdo
        pdoExt->UniqueId.Buffer = uniqueIdString;
        pdoExt->UniqueId.LengthInBytes =
            uniqueIdUnicodeString.MaximumLength;

        LOG(HubFdo, LOG_PDO, 'cpd2', PortData, uniqueIdString);
        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        // generate the default Ids we used to enumerate this thing as an
        // 'unknown' device
        nts = UsbhBuildDeviceID(HubFdo, NULL, &pdoExt->PnpDeviceId);
        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_PDO, 'cpE1', pdo, nts);
            break;
        }

        nts = UsbhBuildHardwareID(HubFdo, NULL, &pdoExt->PnpHardwareId);
        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_PDO, 'cpE2', pdo, nts);
            break;
        }

        nts = UsbhBuildCompatibleID(HubFdo, NULL,  &pdoExt->PnpCompatibleId);
        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_PDO, 'cpE3', pdo, nts);
            break;
        }

        // Pdo_Init->Pdo_Created
        nts = UsbhConnectPdoToHub(pdo, HubFdo);
        if (NT_ERROR(nts)) {
            LOG(HubFdo, LOG_PDO, 'cpE4', pdo, nts);
            break;
        }


        KeInitializeSpinLock(&pdoExt->IoLock);
        KeInitializeSpinLock(&pdoExt->WakeIrpSpin);
        InitializeListHead(&pdoExt->WakeIrpList);

        IoCsqInitialize(&pdoExt->WakeIrpCsq,
                UsbhInsertPdoWakeIrp,               //CsqInsertIrp,
                UsbhRemovePdoWakeIrp,               //CsqRemoveIrp,
                UsbhPeekNextPdoWakeIrp,             //CsqPeekNextIrp,
                UsbhAcquirePdoWakeIrpLock,          //CsqAcquireLock,
                UsbhReleasePdoWakeIrpLock,          //CsqReleaseLock,
                UsbhCompleteCanceledPdoWakeIrp      //CsqCompleteCanceledIrp
                );


        //jd
        // can we combine these two?
        KeInitializeSpinLock(&pdoExt->IdleIrpSpin);
        KeInitializeSpinLock(&pdoExt->IdleIrpStateSpin);
        InitializeListHead(&pdoExt->IdleIrpList);

        IoCsqInitialize(&pdoExt->IdleIrpCsq,
                            UsbhInsertPdoIdleIrp,
                            UsbhRemovePdoIdleIrp,
                            UsbhPeekNextPdoIdleIrp,
                            UsbhAcquirePdoIdleIrpLock,
                            UsbhReleasePdoIdleIrpLock,
                            UsbhCompleteCanceledPdoIdleIrp);

        UsbhInitializeTimerObject(HubFdo,
                                  UsbhIdleIrpRetryTimer,
                                  &pdoExt->IdleRetryTimer,
                                  'Idle');

        UsbhInitializeTimerObject(HubFdo,
                                  UsbhSS_WakeIrpRetryTimer,
                                  &pdoExt->D0WakeRetryTimer,
                                  'D0Wa');

        KeInitializeSemaphore(&pdoExt->UxdLock, 1, 1);

        //
        // Initialize Power and PnP logs
        //
        RtlFillMemory(&pdoExt->PowerFuncHistory, HUB_PP_HISTORY_LEN, 0xff);
        RtlFillMemory(&pdoExt->PnpFuncHistory, HUB_PP_HISTORY_LEN, 0xff);

        nts = UsbhIncPdoIoCount(pdo, pdo, SIG_IOL, 0);
        if (NT_ERROR(nts)) {
TEST_TRAP(); //code coverage
            break;
        }
        pdoExt->CleanupFlags.WaitPdoIo = 1;

        pdoExt->Pdo = pdo;
        pdoExt->PortNumber = PortData->PortNumber;
        pdoExt->UcmConnectorId = PortData->UcmConnectorId;
        // take system wake level from the hub
        pdoExt->SystemWake = hubExt->DeviceCapabilities.SystemWake;

        // created in D0, initial state does not require us to lock
        pdoExt->CurrentPdoPowerState = Pdo_D0;
        pdoExt->Pdo_Dx = PowerDeviceD0;
        pdoExt->Pdo_Sx = PowerSystemWorking;

        // initializing the variables that handle surprise removal
        pdoExt->InPathCount = 0;
        pdoExt->IsBootDevice = FALSE;

        lock.Locked = 0;
        LockGSL(&lock);
        //->Pdo_Created
        UsbhSetPortData(HubFdo, pdo, &lock);

        // set the location information string -- this is non critical
        // this will allow us to id the location of the device when when it fails
        // to enumerate
        UsbhGetLocationIdString(HubFdo, pdo, 0x409, &pdoExt->TextLocationIdString);

        unLockGSL(&lock);

        DbgTrace((UsbhDebugTrace,"UsbhCreatePdo - newPDO = %p\n",pdo));
        LOG(HubFdo, LOG_PDO, 'PDOc', pdo, nts);

        UsbhAssert(HubFdo, NT_SUCCESS(nts));

        break;
    }

    if (NT_ERROR(nts)) {

        // we were unable to connect the PDO to the hub
        // free the pdo and return the error
        UsbhAssert(HubFdo, GET_PDO_HWPNPSTATE(pdo) == Pdo_Created);
        LOG(HubFdo, LOG_PDO, 'PDO!', pdo, nts);

        Usbh_ErrorDisconnect_PdoEvent(HubFdo, pdo, PDC(PortData));
    } else {
        pdo->Flags &= ~DO_DEVICE_INITIALIZING;
        // New PDO is in D0 by default
        UsbhSshSetPortsBusyState(HubFdo, PN(PortData), PORT_BUSY, RESUME_HUB);

        SET_PDO_SWPNPSTATE(pdo, PDO_WaitPnpStart, Ev_PDO_CREATE_PDO);
    }

    return nts;
}


NTSTATUS
UsbhSyncSendCommandToDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PUSHORT BufferLength,
    ULONG MillisecondTimeout,
    USBD_STATUS *ReturnUsbdStatus
    )
/*++

Routine Description:

    Synchronously sends a command on a devices control pipe with an
    optional timeout.  This function uses a new generic control interface
    in USBDI to send the 8 setup byte packet.

    This routine will re-enter the hub driver through the PDO

    HubFdo -

    SetupPacket - 8 byte control packet

    Buffer - buffer, may be NULL

    BufferLength - ptr to buffer length, in = size of buffer, out = bytes
            returned

    MillisecondTimeout - option timeout 0 = no timeout

    UsbdStatus - ptr to return usb status of the operation

Arguments:

Return Value:

    status

--*/
{
    NTSTATUS nts;
    PURB urb = NULL;
    KEVENT event = {0};
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    PIO_STACK_LOCATION nextStk;
    ULONG ps = 0;
    PUSB_DEVICE_HANDLE dh = NULL;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    PDEVICE_OBJECT TopOfBusStack;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    do {
        // check the status on our upstream port before issuing the request
        // if our status is disconnect then so is the child device we are talking
        // to.
        // Hubs never transmit commands to devices that are not connected to their
        // downstream ports.
        nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

        if (NT_ERROR(nts)) {
            // don't bother to issue the request if the hub is disconnected
            // or the port is disabled
            usbdStatus = USBD_STATUS_DEVICE_GONE;
            break;
        }

        UsbhAssert(HubFdo, BufferLength != NULL);
        if (BufferLength == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            usbdStatus = USBD_STATUS_DEVICE_GONE;
            break;
        }

        UsbhAllocatePool_Z(urb,
                           NonPagedPool,
                           sizeof(struct _URB_CONTROL_TRANSFER_EX));

        if (urb == NULL) {
            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // reference the device handle for this internal transaction, this will keep us from
        // passing in a bogus handle although the device may still disappear while the command
        // runs, the device handle will remaon until we release it.

        dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, urb, Devh_Pdo_SyncSendCommand);
        TopOfBusStack = hubExt->TopOfBusStack;

        UsbhAssert(HubFdo, hubExt->TopOfBusStack);

        if ( (dh == NULL) || !TopOfBusStack ) {
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            usbdStatus = USBD_STATUS_DEVICE_GONE;
            break;
        }

        INIT_EVENT_NOT_SIGNALED(&event);

        irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB,
                                            TopOfBusStack,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            TRUE,  // INTERNAL
                                            &event,
                                            &ioStatus);

        if (irp == NULL) {
            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // null out device handle in case we are the root hub
        urb->UrbHeader.UsbdDeviceHandle = pdoExt->DeviceHandle;

        // set up the control transfer
        urb->UrbControlTransferEx.Hdr.Length =
            sizeof(struct _URB_CONTROL_TRANSFER_EX);
        urb->UrbControlTransferEx.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

        urb->UrbControlTransferEx.TransferFlags = (USBD_DEFAULT_PIPE_TRANSFER |
                                                     USBD_SHORT_TRANSFER_OK);
        if (SetupPacket->bmRequestType.Dir ==  BMREQUEST_DEVICE_TO_HOST) {
            urb->UrbControlTransferEx.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
        }
        urb->UrbControlTransferEx.TransferBufferLength = *BufferLength;
        urb->UrbControlTransferEx.TransferBuffer = Buffer;
        urb->UrbControlTransferEx.TransferBufferMDL = NULL;
        urb->UrbControlTransferEx.Timeout = MillisecondTimeout;
        RtlCopyMemory(&urb->UrbControlTransferEx.SetupPacket,
                      SetupPacket,
                      sizeof(struct _USB_DEFAULT_PIPE_SETUP_PACKET));

        nextStk = IoGetNextIrpStackLocation(irp);
        nextStk->Parameters.Others.Argument1 = urb;

        // send packet directly to bus driver
        nts = IoCallDriver(TopOfBusStack, irp);

        if (nts == STATUS_PENDING) {
            WAIT_EVENT(&event);
        } else {
            ioStatus.Status = nts;
        }

        nts = ioStatus.Status;

        // return the length and URB status
        usbdStatus = urb->UrbControlTransferEx.Hdr.Status;

        *BufferLength = (USHORT)urb->UrbControlTransferEx.TransferBufferLength;

        LOG(HubFdo, LOG_PDO, 'SCdv', *BufferLength, usbdStatus);

    } WHILE (0);

    SET_OPTIONAL_STATUS(ReturnUsbdStatus, usbdStatus);
    if (dh) {
        // allow device to be removed
        UsbhDerefPdoDeviceHandle(HubFdo, dh, urb, Devh_Pdo_SyncSendCommand);
    }

    if (urb) {
        UsbhFreePool(urb);
    }

    return nts;
}


NTSTATUS
UsbhGetStringFromDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus,
    PUCHAR Buffer,
    PUSHORT BufferLengthBytes,
    USHORT LanguageId,
    UCHAR StringIndex
    )
/*++

Routine Description:

    Fetches a string descriptor from a device we have enumerated.  This routine is
    used to fetch various types of string descriptors from devices.

Arguments:

    Pdo - Pdo for the device to query

    UsbdStatus - pointer to be filled in with the returned USBD status code.

    Buffer - Buffer to receive the the string descriptor

    BufferLengthBytes - input, the length of the receive buffer.
                        output, the number of bytes returned by the device

    LanguageId - Language for string requested string.

    StringIndex - Index of requested string.

Return Value:

    status of the operation.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    // because we dereference it
    UsbhAssert(HubFdo, BufferLengthBytes);
    if (BufferLengthBytes == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    LOG(HubFdo, LOG_PDO, 'gST1', *BufferLengthBytes, StringIndex);
    LOG(HubFdo, LOG_PDO, 'gST2', Pdo, LanguageId);

    // setup packet for Get_Descriptor, string
    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_STRING_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = StringIndex;
    setupPacket.wIndex.W = LanguageId;
    setupPacket.wLength = *BufferLengthBytes;


    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      Buffer,
                                      BufferLengthBytes,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      UsbdStatus);

    LOG(HubFdo, LOG_PDO, 'gST3', *BufferLengthBytes, nts);

    return nts;

}


NTSTATUS
UsbhGetBosDescriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PULONG BufferLength
    )
/*++

Routine Description:

    Fetches the BOS descriptor from the device

Arguments:

Return Value:

    NTSTATUS

--*/
{
    USB_BOS_DESCRIPTOR  usbBosDescriptor;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    USBD_STATUS usbdStatus;

    pdoExt = PdoExt(Pdo);

    // setup packet for Get_Descriptor, BOS Descriptor Type
    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_BOS_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = sizeof(USB_BOS_DESCRIPTOR);

    *BufferLength = sizeof(USB_BOS_DESCRIPTOR);

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      (PUCHAR)&usbBosDescriptor,
                                      (PUSHORT)BufferLength,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if (NT_ERROR(nts)) {
        pdoExt->EnumFailureMsgId = USBENUM_BOS_DESCRIPTOR_FAILURE;
        return nts;
    }

    UsbhAllocatePool_Z(pdoExt->BosDescriptor,
                       NonPagedPool,
                       usbBosDescriptor.wTotalLength);

    if (pdoExt->BosDescriptor == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // setup packet for Get_Descriptor, string
    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_BOS_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = usbBosDescriptor.wTotalLength;

    *BufferLength = usbBosDescriptor.wTotalLength;

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      (PUCHAR)pdoExt->BosDescriptor,
                                      (PUSHORT)BufferLength,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if (NT_ERROR(nts)) {
        pdoExt->EnumFailureMsgId = USBENUM_BOS_DESCRIPTOR_FAILURE;
        UsbhFreePool(pdoExt->BosDescriptor);
    }

    LOG(HubFdo, LOG_PDO, 'GBOS', nts, 0);

    return nts;
}


NTSTATUS
UsbhGetDeviceDescriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Fetches and validates the device descriptor for the device

Arguments:

Return Value:

    NTSTATUS

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    USHORT bufferLength;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    // setup packet for Get_Descriptor, Device Descriptor Type
    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_DEVICE_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = sizeof(USB_DEVICE_DESCRIPTOR);

    bufferLength = sizeof(USB_DEVICE_DESCRIPTOR);

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      (PUCHAR)&pdoExt->DeviceDescriptor,
                                      &bufferLength,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if (!NT_SUCCESS(nts)) {
        return nts;
    }

    if (bufferLength != sizeof(USB_DEVICE_DESCRIPTOR)) {
        return STATUS_DEVICE_DATA_ERROR;
    }


    if (!UsbhValidateDeviceDescriptor(HubFdo,
                                      &pdoExt->DeviceDescriptor,
                                      sizeof(USB_DEVICE_DESCRIPTOR),
                                      NULL,
                                      NULL)) {

        return STATUS_DEVICE_DATA_ERROR;
    }

    LOG(HubFdo, LOG_PDO, 'GeDD', nts, 0);

    return nts;
}


NTSTATUS
UsbhGetQualifierDescriptorFromDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus,
    PUCHAR Buffer,
    PUSHORT BufferLengthBytes
    )
/*++

Routine Description:

    fetches the device qualifier from a device the hub has enumerated

Arguments:

Return Value:

    optional UsbdStatus of the operation

    status

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    // because we dereference it
    UsbhAssert(HubFdo, BufferLengthBytes);
    if (BufferLengthBytes == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    // setup packet for Get_Descriptor, string
    setupPacket.bmRequestType.B = 0x80;
    setupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    setupPacket.wValue.HiByte = USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = *BufferLengthBytes;

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      Buffer,
                                      BufferLengthBytes,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      UsbdStatus);

    return nts;

}


NTSTATUS
UsbhEnableDeviceForWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Enables the device associated with a PDO for remote wakeup ie USB resume
    signalling.

Arguments:

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;
    USHORT nBytes = 0;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, pdoExt->PdoFlags.UsbWakeupSupported);

    // setup packet for Set Feature, Device
    setupPacket.bmRequestType.B = 0x00;
    setupPacket.bRequest = USB_REQUEST_SET_FEATURE;
    setupPacket.wValue.W = USB_FEATURE_REMOTE_WAKEUP;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = nBytes;

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      NULL,
                                      &nBytes,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      UsbdStatus);

    LOG(HubFdo, LOG_PDO, 'ENwk', nts, 0);
    if (NT_SUCCESS(nts)) {
        pdoExt->PdoFlags.DeviceArmedForWake = 1;
    }
    return nts;

}


NTSTATUS
UsbhDisableDeviceForWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Enables the device associated with a PDO for remote wakeup ie USB resume
    signalling.

Arguments:

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS nts;
    USHORT nBytes = 0;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, pdoExt->PdoFlags.UsbWakeupSupported);

    // setup packet for Set Feature, Device
    setupPacket.bmRequestType.B = 0x00;
    setupPacket.bRequest = USB_REQUEST_CLEAR_FEATURE;
    setupPacket.wValue.W = USB_FEATURE_REMOTE_WAKEUP;
    setupPacket.wIndex.W = 0;
    setupPacket.wLength = nBytes;

    nts = UsbhSyncSendCommandToDevice(HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      NULL,
                                      &nBytes,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      UsbdStatus);

    LOG(HubFdo, LOG_PDO, 'DNwk', nts, 0);
    if (NT_SUCCESS(nts)) {
        pdoExt->PdoFlags.DeviceArmedForWake = 0;
    }

    return nts;

}


NTSTATUS
UsbhCloseDeviceConfiguration(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Sets a PDO to configuration zero.  This will invalidate all pipe handles
    for the device as well as the config handle.  It will also free the
    endpoint resources and complete any outstanding transfers.

    NOTE: some drivers do this as part of their remove processing -- but not
    all of them.

Arguments:

Return Value:

    status

--*/
{
    NTSTATUS nts;
    PURB urb;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt;
    KEVENT event = {0};
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStk;

    hubExt = FdoExt(HubFdo);
    pdoExt = PdoExt(Pdo);

    UsbhAllocatePool_Z(urb, NonPagedPool,
            sizeof(struct _URB_SELECT_CONFIGURATION));

    if (!urb) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    INIT_EVENT_NOT_SIGNALED(&event);

    UsbhAssert(HubFdo, hubExt->TopOfBusStack);
    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB,
                                        hubExt->TopOfBusStack,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,  // INTERNAL
                                        &event,
                                        &ioStatus);

    if (irp == NULL) {
        UsbhFreePool(urb);
        return STATUS_INSUFFICIENT_RESOURCES;
    }



    urb->UrbHeader.Length = sizeof(struct _URB_SELECT_CONFIGURATION);
    urb->UrbHeader.Function = URB_FUNCTION_SELECT_CONFIGURATION;
    urb->UrbHeader.UsbdDeviceHandle = pdoExt->DeviceHandle;

    urb->UrbSelectConfiguration.ConfigurationDescriptor = NULL;

    nextStk = IoGetNextIrpStackLocation(irp);
    nextStk->Parameters.Others.Argument1 = urb;

    // send packet directly to bus driver
    nts = IoCallDriver(hubExt->TopOfBusStack, irp);

    if (nts == STATUS_PENDING) {
        WAIT_EVENT(&event);
    } else {
        ioStatus.Status = nts;
    }

    nts = ioStatus.Status;

    // return the length and URB status
    if (UsbdStatus != NULL) {
        *UsbdStatus = urb->UrbControlTransferEx.Hdr.Status;
    }

    UsbhFreePool(urb);

    return nts;

}


VOID
UsbhDerefPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag,
    PHGSL Lock
    )
/*++

Routine Description:

    removes the tagged element on the list for the PDO and
    signals latch event if list is empty.

Arguments:

    Object - optional object that identifies a tag instance, if specified it
            must match original object passed in

Return Value:

    LatchEvent will be signaled if list goes empty

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PLIST_ENTRY le;
    PLATCH_LIST_ENTRY latch = NULL;

    UsbhAssert(HubFdo, Lock->Locked);
    // find the item and remove it
    LOG(HubFdo, LOG_PDO, 'drfP', Pdo, Tag);

    pdoExt = PdoExt(Pdo);
    ASSERT_PDOEXT(pdoExt);

    for(le = pdoExt->LatchListHead.Flink;
        le != &pdoExt->LatchListHead;
        le = le->Flink) {

        latch = (PLATCH_LIST_ENTRY) CONTAINING_RECORD(
              le,
              struct _LATCH_LIST_ENTRY,
              Link);

        LOG(HubFdo, LOG_PDO, 'lat?', pdoExt, Tag);

        if (latch->Tag == Tag &&
            latch->Object == Object) {
            break;
        }
        latch = NULL;
    }

    //
    // We might not always find the latch ref object because
    // the ref object won't be created in the low-res case.
    //
    // UsbhAssert(HubFdo, latch);

    if (latch) {
        // remove this reference
        RemoveEntryList(&latch->Link);
        UsbhFreePool(latch);
    } else {
        UsbhAssert(HubFdo, pdoExt->LatchCountNoRes);
        if (pdoExt->LatchCountNoRes) {
            pdoExt->LatchCountNoRes--;
        }
    }

    if (IsListEmpty(&pdoExt->LatchListHead) &&
        !pdoExt->LatchCountNoRes) {
        SET_EVENT(&pdoExt->LatchEvent);
    }
}


VOID
UsbhResetPortData(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    Resets the portdata struct for the PDOs port number so that the hub
    is no longer pointing at the PDO.

    When the latch list goes empty we allow delete.  Once the portdata structure is
    NULL'ed out noone else can latch the PDO.

Arguments:

Return Value:

    none

--*/
{
    PHUB_PORT_DATA pd;

    LOG(HubFdo, LOG_PDO, 'rpdt', Pdo, Lock);
    UsbhAssert(HubFdo, Lock->Locked);

    pd = UsbhGetPortData(HubFdo, PdoExt(Pdo)->PortNumber);
    UsbhAssert(HubFdo, pd);
    UsbhAssert(HubFdo, PdoExt(Pdo)->CleanupFlags.ResetPortData);

    if (pd) {
        UsbhAssert(HubFdo, pd->PnpPdo == Pdo);
        pd->PnpPdo = NULL;
        UsbhSetConnectionStatus(HubFdo, pd, NoDeviceConnected);
        PdoExt(Pdo)->CleanupFlags.ResetPortData = 0;
        // we dropped the pdo, signal any resets that may be waiting
        //UsbhSignalSyncDeviceReset(HubFdo, Pdo, pd, NT_DEVICE_DISCONNECTED_STATUS);
        // we can only signal the PDO since we hold the highest priority spinlock
        UsbhSignalSyncDeviceResetPdo(HubFdo, Pdo, NT_DEVICE_DISCONNECTED_STATUS);

        UsbhDerefPdo(HubFdo, Pdo, NULL, 'LATC', Lock);
        // delete can now continue
        //...
    }
}


PDEVICE_OBJECT
UsbhRefPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag,
    PHGSL Lock
    )
/*++

Routine Description:

    inserts the tagged element on the list

Arguments:

    Object - optional pointer to associate with this tag

Return Value:

    returns the PDO if we can ref it.

--*/
{
    PLATCH_LIST_ENTRY latch;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);
    ASSERT_PDOEXT(pdoExt);

    UsbhAssert(HubFdo, Lock->Locked);
    UsbhAllocatePool_Z(latch, NonPagedPool, sizeof(struct _LATCH_LIST_ENTRY));

    if (latch) {
        latch->Sig = SIG_LATCH;
        latch->Pdo = Pdo;
        latch->Tag = Tag;
        latch->Object = Object;
        InsertTailList(&pdoExt->LatchListHead, &latch->Link);

    } else {
        // Allocation failed use the resource-less ref count
        pdoExt->LatchCountNoRes++;
    }

    return Pdo;
}


VOID
UsbhSetPortData(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    Sets the portdata structure to point to a Pdo

Arguments:

Return Value:

    none

--*/
{
    PHUB_PORT_DATA pd;

    LOG(HubFdo, LOG_PDO, 'spdt', Pdo, Lock);
    LOG(HubFdo, LOG_PDO, 'spdP', Pdo, PdoExt(Pdo)->PortNumber);
    UsbhAssert(HubFdo, Lock->Locked);

    pd = UsbhGetPortData(HubFdo, PdoExt(Pdo)->PortNumber);
    UsbhAssert(HubFdo, pd);


    if (pd) {

        UsbhAssert(HubFdo, pd->PnpPdo == NULL);
        RESET_EVENT(&PdoExt(Pdo)->LatchEvent);

        // at this point we are the only ones that can delete it,
        // if it fails due to resources no PDO is set

        Pdo = UsbhRefPdo(HubFdo, Pdo, NULL, 'LATC', Lock);
        pd->PnpPdo = Pdo;
        if (Pdo) {
            UsbhSetConnectionStatus(HubFdo, pd, DeviceConnected);
            PdoExt(Pdo)->CleanupFlags.ResetPortData = 1;
        } else {
            UsbhSetConnectionStatus(HubFdo, pd, NoDeviceConnected);
        }
    }

}


PDEVICE_OBJECT
UsbhLatchPdo(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    returns Pdo from a PortData structure with reference.  Pdo cannot be deleted by another
    process while ref'ed.

    The latch fails if the portdata structure does not point to a PDO ie the device is removed
    or is about to be removed.

    When a PDO is latched it only prevents the devobj from being deleted -- it does not hold it
    in the portdata structure or lock the device handle.

Arguments:

    PortNumber - port number to look at for PDO.

    Object - generic pointer to object associated with this latch (optional)

    Tag - dword tag associated with this latch, useful for debugging.

Return Value:

    Returns the PDO currently associated with the port or NULL if no PDO is associated
    with this port.

--*/
{
    PHUB_PORT_DATA pd;
    PDEVICE_OBJECT pdo = NULL;
    HGSL lock;

    LOG(HubFdo, LOG_PDO, 'lPDO', PortNumber, Tag);

    lock.Locked = 0;
    LockGSL(&lock);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        pdo = pd->PnpPdo;
        if (pdo) {
            pdo = UsbhRefPdo(HubFdo, pdo, Object, Tag, &lock);
        }
    }

    unLockGSL(&lock);

    return pdo;

}


VOID
UsbhUnlatchPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

    Releases a specific latch on the Pdo so it may be deleted.

Arguments:

Return Value:

--*/
{
    HGSL lock;

    LOG(HubFdo, LOG_PDO, 'uPDO', Pdo, Tag);

    lock.Locked = 0;
    LockGSL(&lock);

    UsbhDerefPdo(HubFdo, Pdo, Object, Tag, &lock);

    unLockGSL(&lock);

    return;
}


PDEVICE_OBJECT
UsbhGetHubFdo(
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    )
/*++

Routine Description:

    given a PDO return the hub Fdo with a reference

Arguments:

Return Value:

    Hub Fdo with reference

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(pdoExt->HubFdo, Lock->Locked);

    return pdoExt->HubFdo;

}


VOID
UsbhRemoveAndDeletePdo(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Deletes a Pdo on the global child list.

    Pdos are added when they are created and removed when deleted
    with IoDeleteDevice.  The global list tracks all USB PDOs in the system.

Arguments:


Return Value:

    VOID

--*/
{

    HGSL lock;
    PDEVICE_EXTENSION_PDO pdoExt;
    KIRQL irql;

    pdoExt = PdoExt(Pdo);

    RtlFreeUnicodeString(&pdoExt->PhysicalDeviceObjectName);

    lock.Locked = 0;
    LockGSL(&lock);

    FREE_ID_STRINGS(pdoExt);

    unLockGSL(&lock);

    KeAcquireSpinLock(&pdoExt->GlobalListStateLock, &irql);

    if (pdoExt->GlobalListState == PdoInGlobalList) {
        pdoExt->GlobalListState = PdoNotInGlobalList;
        USBD_RemoveDeviceFromGlobalList(Pdo);
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
}