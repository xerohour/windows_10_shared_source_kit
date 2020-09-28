/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    pnp.c

Abstract:

    Port driver for USB host controllers

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include <ntifs.h>
#include "common.h"

#ifdef USBPORT_WPP
#include "pnp.tmh"
#endif

#pragma alloc_text(PAGE, USBPORT_MakeHcdDeviceName)

#define DunTypeRecognized(t)         ((t > DeviceUsageTypeUndefined) && (t <= DeviceUsageTypeBoot))
#define DunTypeSpecialFile(t)        ((t > DeviceUsageTypeUndefined) && (t <= DeviceUsageTypeDumpFile))

LIST_ENTRY USBPORT_USB1fdoList;
LIST_ENTRY USBPORT_USB2fdoList;
LIST_ENTRY USBPORT_MiniportDriverList;
// this is used to protect global variables and FDO lists
// for all USB host controllers in the system
KSPIN_LOCK USBPORT_GlobalsSpinLock;
ULONG USBD_Version;
ULONG AcpiHcDevFunc = 0;

PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckLog = NULL;
PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckData = NULL;
PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckExtData = NULL;

// Statically initialize the USBPORT_TRIAGE_INFO structure used by
// DBGENG.DLL
//
USBPORT_TRIAGE_INFO USBPORT_Triage_Info = {
    // Version
    USBPORT_TRIAGE_INFO_VERSION_1,
    // Debug Log
    FIELD_OFFSET(DEVICE_EXTENSION, Log.LogStart),
    FIELD_OFFSET(DEVICE_EXTENSION, Log.LogSize),
    // USBD_DEVICE_HANDLE
    sizeof(USBD_DEVICE_HANDLE),
    FIELD_OFFSET(USBD_DEVICE_HANDLE, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.DeviceHandleList),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.DeviceHandleDeletedList),
    FIELD_OFFSET(USBD_DEVICE_HANDLE, DevhLink),
    // HCD_ENDPOINT
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.HcdEndpointSize),
    FIELD_OFFSET(HCD_ENDPOINT, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.GlobalEndpointList),
    FIELD_OFFSET(HCD_ENDPOINT, GlobalLink),
    // HCD_TRANSFER_CONTEXT
    sizeof(HCD_TRANSFER_CONTEXT),
    FIELD_OFFSET(HCD_TRANSFER_CONTEXT, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION, TxCsqIrpListHead),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.TxPriorityListHead),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.TxCancelIrpListHead),
    FIELD_OFFSET(HCD_TRANSFER_CONTEXT, TxCsqIrpLink),
    FIELD_OFFSET(HCD_TRANSFER_CONTEXT, TxPriorityLink),
    FIELD_OFFSET(HCD_TRANSFER_CONTEXT, TxCancelIrpLink),
    // USB_ABORT_PIPE_CONTEXT
    sizeof(USB_ABORT_PIPE_CONTEXT),
    FIELD_OFFSET(USB_ABORT_PIPE_CONTEXT, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION, AxCsqIrpListHead),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.AxPendingListHead),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.AxDoneListHead),
    FIELD_OFFSET(DEVICE_EXTENSION, Fdo.AxCanceledListHead),
    FIELD_OFFSET(USB_ABORT_PIPE_CONTEXT, AxCsqIrpLink),
    FIELD_OFFSET(USB_ABORT_PIPE_CONTEXT, AxPendingLink),
    FIELD_OFFSET(USB_ABORT_PIPE_CONTEXT, AxDoneLink),
    FIELD_OFFSET(USB_ABORT_PIPE_CONTEXT, AxCanceledLink),
    // USB_OBJ_REF
    sizeof(USBOBJ_REF),
    FIELD_OFFSET(USBOBJ_REF, Sig),
    FIELD_OFFSET(HCD_ENDPOINT, EpRefListHead),
    FIELD_OFFSET(USBOBJ_REF, UsbRefLink),
};

EXT_CALLBACK USBPORT_SimulateInterruptDpcHighRes;

USB_MINIPORT_STATUS
USBPORTSVC_GetMiniportRegistryKeyValue(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    ULONG KeyNameStringLength,
    __inout PVOID Data,
    ULONG DataLength
    )
/*++

Routine Description:

    Get a registry parameter from either the hardware
    or software branch of the registry given the PDO

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus;

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);


    ntStatus = USBPORT_GetCachedRegistryKeyValueForPdo(
                                          devExt->HcFdoDeviceObject,
                                          devExt->Fdo.PhysicalDeviceObject,
                                          SoftwareBranch,
                                          KeyNameString,
                                          KeyNameStringLength,
                                          Data,
                                          DataLength);

    return USBPORT_NtStatus_TO_MiniportStatus(ntStatus);
}


NTSTATUS
USBPORT_GetRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __inout_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    )
/*++

Routine Description:

    Get a registry parameter from either the hardware or software branch of the registry given the PDO.

    *Software Branch* (PLUGPLAY_REGKEY_DRIVER):
        key HKLM\System\CCS\Control\Class\ *Driver*

        The value of *Driver* can found in the hardware branch
        key HKLM\System\CCS\Enum\ *DeviceID* \ *InstanceID* \Driver

        example: {4D36E972-E325-11CE-BFC1-08002BE10318}\0001

    *Hardware Branch* (PLUGPLAY_REGKEY_DEVICE):

        The registry value entry would be found under the following registry
        key HKLM\System\CCS\Enum\ *DeviceID* \ *InstanceID* \Device Parameters


Arguments:

    FdoDeviceObject - controller FDO.

    PhysicalDeviceObject - Pdo associated with the key, passed to IoOpenDeviceRegistryKey

    SoftwareBranch - TRUE if key is read from software barnch of the PDO (see USBPORT_GetRegistryKeyValueForPdo)

    KeyNameString - unicode null terminated key name

    KeyNameStringLength - length of Keynamestring in bytes

    Data - pointer to a buffer that recieves the key data

    DataLength - lenth in bytes of Data buffer


Return Value:

    STATUS_SUCCESS if key can be read.

--*/
{
    ULONG length = 0;
    HANDLE handle;
    NTSTATUS ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    UNICODE_STRING keyNameUnicodeString;
    PKEY_VALUE_FULL_INFORMATION fullInfo = NULL;

    ASSERT_PASSIVE();

    ntStatus = RtlULongAdd(sizeof(KEY_VALUE_FULL_INFORMATION),
                           KeyNameStringLength,
                           &length);

    if(!NT_SUCCESS(ntStatus)){
        return ntStatus;
    }

    ntStatus = RtlULongAdd(length,
                           DataLength,
                           &length);

    if(!NT_SUCCESS(ntStatus)){
        return ntStatus;
    }

    if (SoftwareBranch) {
        ntStatus = IoOpenDeviceRegistryKey(PhysicalDeviceObject,
                                           PLUGPLAY_REGKEY_DRIVER,
                                           STANDARD_RIGHTS_ALL,
                                           &handle);
    } else {
        ntStatus = IoOpenDeviceRegistryKey(PhysicalDeviceObject,
                                           PLUGPLAY_REGKEY_DEVICE,
                                           STANDARD_RIGHTS_ALL,
                                           &handle);
    }

    if (NT_SUCCESS(ntStatus)) {

        RtlInitUnicodeString(&keyNameUnicodeString, KeyNameString);

        ALLOC_POOL_Z(fullInfo, PagedPool, length);

        DbgTrace((UsbpDebugTrace,"' GetRegistryKeyValueForPdo buffer = %p\n", fullInfo));

        if (fullInfo) {
            ntStatus = ZwQueryValueKey(handle,
                                       &keyNameUnicodeString,
                                       KeyValueFullInformation,
                                       fullInfo,
                                       length,
                                       &length);

            if (NT_SUCCESS(ntStatus)){
                USBPORT_ASSERT(DataLength == fullInfo->DataLength);
                RtlCopyMemory(Data, ((PUCHAR) fullInfo) + fullInfo->DataOffset, DataLength);
            }

            UsbFreePool(fullInfo);
        }

        ZwClose(handle);
    }

    return ntStatus;
}



NTSTATUS
USBPORT_SetRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in ULONG Type,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __inout_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    )
/*++

Routine Description:

Arguments:

    PhysicalDeviceObject - Pdo associated with the key, passed to IoOpenDeviceRegistryKey

    SoftwareBranch - TRUE if key is read from software barnch of the PDO (see USBPORT_GetRegistryKeyValueForPdo)

    Type - reg key type parameter passed to ZwSetValueKey

    KeyNameString - unicode null terminated key name

    KeyNameStringLength - length of Keynamestring in bytes

    Data - pointer to a buffer that recieves the key data

    DataLength - lenth in bytes of Data buffer


Return Value:

    STATUS_SUCCESS if key can be written.

--*/
{
    HANDLE handle;
    NTSTATUS ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    UNICODE_STRING keyNameUnicodeString;

    ASSERT_PASSIVE();

    if (SoftwareBranch) {
        ntStatus = IoOpenDeviceRegistryKey(PhysicalDeviceObject,
                                           PLUGPLAY_REGKEY_DRIVER,
                                           STANDARD_RIGHTS_ALL,
                                           &handle);
    } else {
        ntStatus = IoOpenDeviceRegistryKey(PhysicalDeviceObject,
                                           PLUGPLAY_REGKEY_DEVICE,
                                           STANDARD_RIGHTS_ALL,
                                           &handle);
    }

    if (NT_SUCCESS(ntStatus)) {

        RtlInitUnicodeString(&keyNameUnicodeString, KeyNameString);
        ntStatus = ZwSetValueKey(handle,
                                 &keyNameUnicodeString,
                                 0,
                                 Type,
                                 Data,
                                 DataLength);
        ZwClose(handle);
    }

    return ntStatus;
}


PUSBPORT_MINIPORT_DRIVER
USBPORT_FindMiniport(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Find a miniport given a DriverObject

Arguments:

    DriverObject - pointer to a driver object

Return Value:

    pointer to miniport or NULL

--*/
{
    KIRQL irql;
    PUSBPORT_MINIPORT_DRIVER found = NULL;
    PUSBPORT_MINIPORT_DRIVER miniportDriver;
    PLIST_ENTRY listEntry;

    KeAcquireSpinLock(&USBPORT_GlobalsSpinLock, &irql);

    listEntry = &USBPORT_MiniportDriverList;
    if (!IsListEmpty(listEntry)) {
        listEntry = USBPORT_MiniportDriverList.Flink;
    }

    while (listEntry != &USBPORT_MiniportDriverList) {

        miniportDriver = (PUSBPORT_MINIPORT_DRIVER)
            CONTAINING_RECORD(listEntry,
                              struct _USBPORT_MINIPORT_DRIVER,
                              ListEntry);

        if (miniportDriver->DriverObject == DriverObject) {
            found = miniportDriver;
            break;
        }

        // next entry
        listEntry = miniportDriver->ListEntry.Flink;
    }

    KeReleaseSpinLock(&USBPORT_GlobalsSpinLock, irql);

    return found;
}


VOID
USBPORT_Unload(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Free globally allocated miniport structure used to
    track this particular miniport driver.

    note: OS won't unload unless this is the last instance
    of the miniport

Arguments:

    DriverObject - pointer to a driver object

Return Value:

    None

--*/
{
    PUSBPORT_MINIPORT_DRIVER miniportDriver;

    // find miniport driver data
    miniportDriver = USBPORT_FindMiniport(DriverObject);
    USBPORT_ASSERT(miniportDriver != NULL);
    if (NULL == miniportDriver) {
        BUGCHECK(USBBUGCODE_INTERNAL_ERROR, // BugCode
                 0,                         // BugCheckParameter2
                 0,                         // BugCheckParameter3
                 0);                        // BugCheckParameter4
        return;
    }

    // the miniport should not need to do anything.
    // But just in case/ we will call them if they
    // indicated an unload routine in the DriverObject.
    DbgTrace((UsbpDebugTrace, "'Unloading USB miniport\n"));
    if (NULL != miniportDriver->MiniportUnload) {
        miniportDriver->MiniportUnload(DriverObject);
    }

    //
    // Clean up WMI
    //
    USBPORT_WmiUnload(DriverObject, &miniportDriver->RegistryPath);

    //WPP_CLEANUP(DriverObject);

    USBPORT_InterlockedRemoveEntryList(&miniportDriver->ListEntry,
                                       &USBPORT_GlobalsSpinLock);
    UsbFreePool(miniportDriver);
}


NTSTATUS
USBPORT_MakeHcdDeviceName(
    PUNICODE_STRING DeviceNameUnicodeString,
    ULONG Idx
    )
/*++

Routine Description:

    This function generates the name used for the FDO.  The
    name format is USBFDO-n where nnn is 0 - 65535.


Arguments:

Return Value:

    None

--*/
{
    PWCHAR deviceNameBuffer;
    WCHAR nameBuffer[]  = L"\\Device\\USBFDO-";
    NTSTATUS ntStatus;
    UNICODE_STRING tmpUnicodeString;
    WCHAR tmpBuffer[16];

    PAGED_CODE();

    // enough for 3 digits and NULL
    tmpUnicodeString.Buffer = tmpBuffer;
    tmpUnicodeString.MaximumLength = sizeof(tmpBuffer);
    tmpUnicodeString.Length = 0;

    ntStatus = RtlIntegerToUnicodeString(Idx,
                                         10,
                                         &tmpUnicodeString);

    if (NT_SUCCESS(ntStatus)) {

        USHORT siz;

        siz = sizeof(nameBuffer)+tmpUnicodeString.Length;

        // we can't log this alloc because the device object
        // has not been created yet
        ALLOC_POOL_Z(deviceNameBuffer, PagedPool, siz);

        if (deviceNameBuffer != NULL) {

            RtlCopyMemory(deviceNameBuffer, nameBuffer, sizeof(nameBuffer));
            RtlInitUnicodeString(DeviceNameUnicodeString,
                                 deviceNameBuffer);
            DeviceNameUnicodeString->MaximumLength = siz;

            ntStatus = RtlAppendUnicodeStringToString(
                            DeviceNameUnicodeString,
                            &tmpUnicodeString);

        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    return ntStatus;
}

NTSTATUS
USBPORT_MakeRootHubPdoName(
    PDEVICE_OBJECT FdoDeviceObject,
    PUNICODE_STRING PdoNameUnicodeString,
    ULONG Index
    )
/*++

Routine Description:

    This service Creates a name for a PDO created by the HUB

Arguments:

Return Value:


--*/
{
    PWCHAR nameBuffer = NULL;
    WCHAR rootName[] = L"\\Device\\USBPDO-";
    UNICODE_STRING idUnicodeString;
    WCHAR buffer[32] = {0};
    NTSTATUS ntStatus = STATUS_SUCCESS;
    USHORT length;
    BOOLEAN haveString = FALSE;

    ASSERT_PASSIVE();

    length = sizeof(buffer)+sizeof(rootName);

    // os frees this when the unicode string is 'freed'
    ALLOC_POOL_OSOWNED(&nameBuffer, PagedPool, length);

    if (nameBuffer) {
        RtlCopyMemory(nameBuffer, rootName, sizeof(rootName));

        RtlInitUnicodeString(PdoNameUnicodeString,
                             nameBuffer);
        PdoNameUnicodeString->MaximumLength =
            length;
        haveString = TRUE; // we have a string now

        RtlInitUnicodeString(&idUnicodeString,
                             &buffer[0]);
        idUnicodeString.MaximumLength =
            sizeof(buffer);

        ntStatus = RtlIntegerToUnicodeString(
                  Index,
                  10,
                  &idUnicodeString);

        if (NT_SUCCESS(ntStatus)) {
             ntStatus = RtlAppendUnicodeStringToString(PdoNameUnicodeString,
                                                       &idUnicodeString);
        }

        DbgTrace((UsbpDebugTrace, "'USBPORT_MakeNodeName string = %p\n", PdoNameUnicodeString));

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (!NT_SUCCESS(ntStatus) && haveString) {
        RtlFreeUnicodeString(PdoNameUnicodeString);
    }

    return ntStatus;
}

NTSTATUS
USBPORT_GetPhysicalDeviceObjectName(
    PDEVICE_OBJECT  PhysicalDeviceObject,
    PUNICODE_STRING Name
    )
/*++

Routine Description:

Arguments:

    PhysicalDeviceObject - PDO for which the device object name should
    be retrieved.  This PDO must be known to PnP.

    Name - If successful the PDO name will be returned in Name in a
    buffer allocated by this routine which the caller must free.

Return Value:

    NT status code.

--*/
{
    ULONG       resultLength = 0;
    PWSTR       buffer;
    NTSTATUS    ntStatus;

    Name->Length = 0;
    Name->MaximumLength = 0;
    Name->Buffer = NULL;

    // Get the size of the PDO name.
    //
    ntStatus = IoGetDeviceProperty(PhysicalDeviceObject,
                                   DevicePropertyPhysicalDeviceObjectName,
                                   0,
                                   NULL,
                                   &resultLength);

    if (ntStatus == STATUS_BUFFER_TOO_SMALL) {

        // Allocate a buffer large enough to hold the PDO name.
        //
        ALLOC_POOL_Z(buffer, NonPagedPoolNx, resultLength);

        if (buffer != NULL) {

            // Get the name into the buffer.
            //
            ntStatus = IoGetDeviceProperty(PhysicalDeviceObject,
                                           DevicePropertyPhysicalDeviceObjectName,
                                           resultLength,
                                           buffer,
                                           &resultLength);

            if (NT_SUCCESS(ntStatus)) {

                // Success, return the name.
                //
                if (resultLength > sizeof(UNICODE_NULL)) {
                    // Length should not include the terminating
                    // UNICODE_NULL.
                    Name->Length = (USHORT)resultLength - sizeof(UNICODE_NULL);
                }
                Name->MaximumLength = (USHORT)resultLength;
                Name->Buffer = buffer;

            } else {

                // Failed getting the name.
                //
                UsbFreePool(buffer);
            }
         }
    }

    return ntStatus;
}

NTSTATUS
USBPORT_PnPAddDevice(
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    This routine is called to create a new instance of a USB host
    controller.  This is where we create our deviceObject.

Arguments:

    DriverObject - pointer to the driver object for this instance of HCD
    PhysicalDeviceObject - pointer to a device object created by the bus

Return Value:

    NT STATUS CODE

--*/
{
    ULONG extSize = 0;
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_OBJECT devObj = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    PUSBPORT_MINIPORT_DRIVER miniportDriver = NULL;
    UNICODE_STRING fdoName;
    ULONG deviceNameIdx = 0;
    ULONG contextIdx;

    // find the driver
    miniportDriver = USBPORT_FindMiniport(DriverObject);
    USBPORT_ASSERT(miniportDriver != NULL);

    extSize = sizeof(DEVICE_EXTENSION) +
              miniportDriver->RegistrationPacket.DeviceDataSize;

    do {

        ntStatus = USBPORT_MakeHcdDeviceName(&fdoName,
                                             deviceNameIdx);
        if (!NT_SUCCESS(ntStatus)) {
            //
            // Resource allocation failure, do not create a device object
            //
            goto Done;
        }

        //
        // Create a named device object for compatibility
        //

        ntStatus = IoCreateDevice(DriverObject,
                                  extSize,
                                  &fdoName,
                                  FILE_DEVICE_CONTROLLER,
                                  FILE_DEVICE_SECURE_OPEN,
                                  FALSE,
                                  &devObj);

        RtlFreeUnicodeString(&fdoName);

        if(NT_SUCCESS(ntStatus)){
            break;
        }

        deviceNameIdx++;

    } while (ntStatus == STATUS_OBJECT_NAME_COLLISION);


    if (!NT_SUCCESS(ntStatus)) {
        DbgTrace((UsbpDebugTrace, "'USBPORT_AddDevice: failed to create devobj\n"));
        goto Done;
    }

    // init HC Fdo device extension
    GET_DEVICE_EXT(devExt, devObj);
    devExt->Sig = USBPORT_DEVICE_EXT_SIG;
    devExt->HcFdoDeviceObject = devObj;
    devExt->PnpState = PnpNotStarted;
    //SET_FLAG(devExt->PnpStateFlags, USBPORT_PNP_NOT_STARTED);
    devExt->Fdo.UsbHcIntDpcGenerationCount = 0;
    devExt->Fdo.PhysicalDeviceObject = PhysicalDeviceObject;
    devExt->Fdo.TopOfStackDeviceObject = IoAttachDeviceToDeviceStack(
                                                        devObj,
                                                        PhysicalDeviceObject);
    devExt->Fdo.DeviceNameIdx = deviceNameIdx;

    if (NULL == devExt->Fdo.TopOfStackDeviceObject){
        ntStatus = STATUS_UNSUCCESSFUL;
        goto Clean;
    }
    devExt->Fdo.Flags.OnPnpThread = 1;
    devExt->Fdo.MiniportDriver = miniportDriver;
    devExt->Fdo.MiniportDeviceData = &devExt->Fdo.MiniportExtension[0];
    RtlCopyMemory(&devExt->Fdo.Name[0],
                  &devExt->Fdo.MiniportDriver->RegistrationPacket.Name[0],
                  8);
    devExt->Fdo.Name[7] = 0;

    for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
        devExt->DumpContext[contextIdx] = NULL;
    }

    devExt->Fdo.HcOptionFlags = REGISTRATION_PACKET(devExt).OptionFlags;

    // this will succeed if the controller is not high speed
    ntStatus = MPx_InitializeHsbController(devExt);
    if(!NT_SUCCESS(ntStatus)) {
        goto Clean;
    }

    // Store a pointer to the USBPORT_Triage_Info structure at a fixed
    // and known offset from the DEVICE_EXTENSION.
    //
    devExt->UsbportTriageInfo = &USBPORT_Triage_Info;

    // Calculate the total HCD_ENDPOINT size.
    //
    ntStatus = RtlULongAdd(sizeof(HCD_ENDPOINT),
                           miniportDriver->RegistrationPacket.EndpointDataSize,
                           &devExt->Fdo.HcdEndpointSize);

    if (!NT_SUCCESS(ntStatus)) {
        goto Clean;
    }

    devExt->Fdo.HcResetWI = USBPORT_AllocateWorkItem(devObj, TRUE);
    if (NULL == devExt->Fdo.HcResetWI) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean;
    }
    devExt->Fdo.HcResetWIQueued = 0;

    USBPORT_InitStateVars(devObj);

    //
    // Let the miniport know that we have created the device extension
    // Note: This was added for symmetry with MPf_DeleteDeviceData
    //
    MPf_CreateDeviceData(devExt);

    // 2048 entries
    // If the debug log buffer size is greater than
    // USBPORT_MAXIMUM_DEBUG_LOG_SIZE it will not be captured in
    // minidumps by DBGENG.DLL.
    //
    USBPORT_LogAlloc(&devExt->Log, 8);

    //
    // Initialize Power and PnP logs
    //
    RtlFillMemory(&devExt->PowerFuncHistory, USB_PP_HISTORY_LEN, 0xff);
    RtlFillMemory(&devExt->PnpFuncHistory, USB_PP_HISTORY_LEN, 0xff);

    ntStatus = USBPORT_RegisterDeviceInterface(devExt,
                                               devExt->Fdo.PhysicalDeviceObject,
                                               &GUID_CLASS_USB_HOST_CONTROLLER);

    //ntStatus = IoRegisterDeviceInterface(devExt->Fdo.PhysicalDeviceObject,
    //                                     &GUID_CLASS_USB_HOST_CONTROLLER,
    //                                     NULL,
    //                                     &devExt->SymbolicLinkName);
    if(!NT_SUCCESS(ntStatus)) {
        goto Clean;
    }

    //SET_FLAG(devExt->Flags, USBPORT_FLAG_IFACE_REGISTER);

    // set appropriate flags
    devObj->Flags |= devExt->Fdo.TopOfStackDeviceObject->Flags &
                            (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_INRUSH);
    devObj->Flags |= DO_POWER_PAGABLE;
    devObj->Flags &= ~DO_DEVICE_INITIALIZING;

    ntStatus = STATUS_SUCCESS;
    goto Done;

Clean:

    if (NULL != devObj) {
        if (devExt->Fdo.HcResetWI != NULL) {
            USBPORT_FreeWorkItem(devExt->Fdo.HcResetWI);
            devExt->Fdo.HcResetWI = NULL;
        }
        if (devExt->Fdo.PhysicalDeviceObjectName.Buffer != NULL) {
            UsbFreePool(devExt->Fdo.PhysicalDeviceObjectName.Buffer);
            devExt->Fdo.PhysicalDeviceObjectName.Buffer = NULL;
        }
        if (devExt->Fdo.HsbControllerContextPtr) {
            UsbFreePool(devExt->Fdo.HsbControllerContextPtr);
        }
        if (devExt->Fdo.SimulateInterruptHighResTimer != NULL) {
            USBPORT_FreeHighResTimer(devExt->Fdo.SimulateInterruptHighResTimer, TRUE);
        }
        USBPORT_LogFree(devObj, &devExt->Log);
        if (NULL != devExt->Fdo.TopOfStackDeviceObject) {
            IoDetachDevice(devExt->Fdo.TopOfStackDeviceObject);
        }
        IoDeleteDevice(devObj);
        devObj = NULL;
    }

Done:

    if (NULL != devObj) {
        GET_DEVICE_EXT(devExt, devObj);
        devExt->Fdo.Flags.OnPnpThread = 0;
    }
    DbgTrace((UsbpDebugTrace, "'exit USBPORT_AddDevice (%x)\n", ntStatus));
    return ntStatus;
}

#define MAX_ENUMERATOR_NAME_LEN 8

BOOLEAN
USBPORT_IsEnumeratorEqual(
    __in PDEVICE_EXTENSION FdoDevExt,
    __in LPCWSTR           ExpectedName
    )
{
    NTSTATUS        status;
    WCHAR           enumerator[MAX_ENUMERATOR_NAME_LEN];
    ULONG           propertyLen;
    UNICODE_STRING  actualEnumerator;
    UNICODE_STRING  expectedEnumerator;

    status = IoGetDeviceProperty(FdoDevExt->Fdo.PhysicalDeviceObject,
                                 DevicePropertyEnumeratorName,
                                 sizeof(enumerator),
                                 enumerator,
                                 &propertyLen);

    if (!NT_SUCCESS(status)) {

        //
        // For lack of a better idea, just assume it is not equal.
        //

        return FALSE;
    }

    RtlInitUnicodeString(&actualEnumerator, enumerator);
    RtlInitUnicodeString(&expectedEnumerator, ExpectedName);

    if (RtlCompareUnicodeString(&actualEnumerator, &expectedEnumerator, TRUE) == 0) {

        return TRUE;
    }

    return FALSE;
}

NTSTATUS
USBPORT_InitStateVars(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

Arguments:

    FdoDeviceObject - DeviceObject for this USB controller.

Return Value:

    NT status code.

--*/
{
    PDEVICE_EXTENSION   devExt = NULL;
    NTSTATUS            tmpStatus;
    ULONG               disablePciConfigSpace = 0;


    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // remove type effect
    USBPORT_Core_InitIoRequestCount(FdoDeviceObject);
    USBPORT_Core_IncIoRequestCount(FdoDeviceObject, &devExt->UsbIoPnpRemove, devExt, UsbIoTag_PnpAdd, FALSE);

    // serialize rhub ds
    KeInitializeSpinLock(&devExt->Fdo.RootHubDpcSpin);

    //Hc pointer to root hub PDO
    KeInitializeSpinLock(&devExt->Fdo.RhPdoPtrSpin);

    // bus context
    InitializeListHead(&devExt->Fdo.BusContextHead);
    KeInitializeSpinLock(&devExt->Fdo.BusContextSpin);

    KeInitializeSpinLock(&devExt->Fdo.HcSyncSpin);

    // chirp
    devExt->Fdo.InPortChirp = -1;

    // enum locks
    KeInitializeSemaphore(&devExt->Fdo.BusLock, 1, 1);
    KeInitializeSemaphore(&devExt->Fdo.DeviceCfgLock, 1, 1);
    KeInitializeSemaphore(&devExt->Fdo.Usb2bwLock, 1, 1);
    KeInitializeSemaphore(&devExt->Fdo.MpSuspendResumeLock, 1, 1);

    // power mgmt
    devExt->Fdo.CurrentFdoPowerState = FdoPwr_WaitSx;
    devExt->Fdo.LastSystemSleepState = PowerSystemUnspecified;
    devExt->Fdo.CurrentSystemPowerState = PowerSystemUnspecified;

    devExt->Dx_State = PowerDeviceD3;
    KeInitializeSpinLock(&devExt->Fdo.PowerSpin);
    KeInitializeEvent(&devExt->Fdo.ChirpCompleteEv, NotificationEvent,
        USBD_EVENT_STATE_SIGNALED);
    KeInitializeEvent(&devExt->Fdo.RootHubReadyEv, NotificationEvent,
        USBD_EVENT_STATE_SIGNALED);


    // wait wake
    KeInitializeEvent(&devExt->Fdo.HcPendingWakeIrpEvent,
                      SynchronizationEvent, USBD_EVENT_STATE_SIGNALED);
    devExt->Fdo.HcWakeState = HCWAKESTATE_DISARMED;
    KeInitializeSpinLock(&devExt->Fdo.WakeIrpSpin);

    // Common Buffer Allocation Lookaside List
    //
    // The LargeCommonBufferThreshold is based on an analysis of the
    // per endpoint type common buffer allocation requirements of the
    // current implementations of the USBEHCI, USBOHCI, and USBUHCI
    // miniports.  Large common buffer allocations from HS Isochronous
    // will be placed on a separate free list from other common buffer
    // allocations.
    //
    // USBEHCI:
    // Control:              2048  00000800H
    // Bulk:                25088  00006200H
    // Interrupt:           25088  00006200H
    // FS Isochronous:       4096  00001000H
    // HS Isochronous:     262144  00040000H
    //
    // USBOHCI:
    // Control:              4992  00001380H
    // Bulk:                 8832  00002280H
    // Interrupt:             640  00000280H
    // Isochronous:          8320  00002080H
    //
    // USBUHCI:
    // Control (MPS  8):    33152  00008180H
    // Control (MPS 64):     4480  00001180H
    // Bulk (MPS  8):       65888  00010160H    (this case should be rare)
    // Bulk (MPS 64):        8544  00002160H
    // Interrupt:            1280  00000500H
    // Isochronous:         49664  0000c200H
    //
    InitializeListHead(&devExt->Fdo.FreeSmallCommonBufferList);
    InitializeListHead(&devExt->Fdo.FreeLargeCommonBufferList);
    devExt->Fdo.FreeSmallCommonBufferCount = 0;
    devExt->Fdo.FreeLargeCommonBufferCount = 0;
    devExt->Fdo.FreeSmallCommonBufferMaxCount = 8;
    devExt->Fdo.FreeLargeCommonBufferMaxCount = 4;
    devExt->Fdo.LargeCommonBufferThreshold = 64 * 1024;
    KeInitializeSpinLock(&devExt->Fdo.CommonBufferListSpin);


    //** The EndpointListSpin spinlock is currently shared with the xdpc and transfer code
    KeInitializeSpinLock(&devExt->Fdo.EndpointListSpin);

    // DM timer
    KeInitializeDpc(&devExt->Fdo.SurpriseRemoveDpc, USBPORT_SurpriseRemoveDpc,
                    FdoDeviceObject);

    USBPORT_InitDM_Timer(FdoDeviceObject);

    // transfers

    // Map DPC tracking
    devExt->Fdo.TxNeedsMapDpcCount = 0;

    // cancel.c init
    USBPORT_TxInitialize(FdoDeviceObject);

    // xdpc.c init
    USBPORT_XdpcInitialize(FdoDeviceObject);


    InitializeListHead(&devExt->Fdo.EpNeoStateChangeList);
    InitializeListHead(&devExt->Fdo.GlobalEndpointList);

    // device handles and tts
    InitializeListHead(&devExt->Fdo.DeviceHandleList);
    InitializeListHead(&devExt->Fdo.GlobalTtListHead);
    InitializeListHead(&devExt->Fdo.DeviceHandleDeletedList);
    KeInitializeSpinLock(&devExt->Fdo.DevHandleListSpin);
    KeInitializeSpinLock(&devExt->Fdo.GlobalTtSpin);

    // core func lock
    KeInitializeSpinLock(&devExt->Fdo.CoreFunctionSpin);

    KeInitializeSpinLock(&devExt->Fdo.MpRootHubFunctionSpin);
    KeInitializeSpinLock(&devExt->Fdo.MpAsyncTimerListSpin);
    InitializeListHead(&devExt->Fdo.MpAsyncTimerListHead);

    // dpc from ISR
    KeInitializeSpinLock(&devExt->Fdo.IsrDpcSpin);
    KeInitializeDpc(&devExt->Fdo.IsrDpc, USBPORT_IsrDpc, FdoDeviceObject);

    // root hub transfer vars and service dpc
    USBPORT_Rh_Initialize(FdoDeviceObject);

    // variants of invalidate controller
    devExt->Fdo.SimulateInterruptHighResTimer =
        USBPORT_AllocateHighResTimer(USBPORT_SimulateInterruptDpcHighRes,
                                     FdoDeviceObject);

    if (devExt->Fdo.SimulateInterruptHighResTimer == NULL) {

        KeInitializeTimer(&devExt->Fdo.SimulateInterruptTimer);
        KeInitializeDpc(&devExt->Fdo.SimulateInterruptDpc,
                        USBPORT_SimulateInterruptDpc,
                        FdoDeviceObject);
    }

    devExt->Fdo.SimulateInterruptIoRequest = NULL;

    KeInitializeDpc(&devExt->Fdo.HcResetDpc, USBPORT_HcResetDpc, FdoDeviceObject);

    // cache registry entries
    InitializeListHead(&devExt->Fdo.RegistryCache);

    // stat counters
    KeInitializeSpinLock(&devExt->Fdo.StatCounterSpin);

    // Get PDO Name
    //
    USBPORT_GetPhysicalDeviceObjectName(devExt->Fdo.PhysicalDeviceObject,
                                        &devExt->Fdo.PhysicalDeviceObjectName);


    devExt->Fdo.DisablePciConfigSpace = 0;

    tmpStatus = USBPORT_GetRegistryKeyValueForPdo(devExt->HcFdoDeviceObject,
                                                  devExt->Fdo.PhysicalDeviceObject,
                                                  USBPORT_HW_BRANCH,
                                                  DS_PCI_CONFIG_SPACE,
                                                  sizeof(DS_PCI_CONFIG_SPACE),
                                                  &disablePciConfigSpace,
                                                  sizeof(disablePciConfigSpace));
    if (NT_SUCCESS(tmpStatus)) {

        DbgTrace((UsbpDebugTrace, "disablePciConfigSpace %d\n", disablePciConfigSpace));

        devExt->Fdo.DisablePciConfigSpace = disablePciConfigSpace;

    } else {

        //
        // See if the controller is enumerated by ACPI. If yes, disable the Pci Config Space
        //

        if (USBPORT_IsEnumeratorEqual(devExt, L"ACPI")) {
            DbgTrace((UsbpDebugTrace, "Enumerator is ACPI\n"));
            devExt->Fdo.DisablePciConfigSpace = 1;
            USBPORT_SetSpecialAcpiDevFunc(devExt);
        }
    }

    //
    // Setup the runtime PM context.
    //
    tmpStatus = USBPORT_RuntimePMContextInit(FdoDeviceObject);

    return tmpStatus;
}

VOID
USBPORT_SetSpecialAcpiDevFunc(
    __in PDEVICE_EXTENSION FdoDevExt
    )
/*++

Routine Description:

    Set unique PCI Device and Function numbers if the controller is actually ACPI enumerated.
    ETW parsers can look for these special numbers to identify it as such.

--*/
{
    ULONG acpiHcDevFunc;

    acpiHcDevFunc = InterlockedIncrement(&AcpiHcDevFunc);
    FdoDevExt->Fdo.BusDevice = LOWORD(acpiHcDevFunc);
    FdoDevExt->Fdo.BusFunction = HIWORD(acpiHcDevFunc);
}

NTSTATUS
USBPORT_GetResources(
    PDEVICE_OBJECT FdoDeviceObject,
    PCM_RESOURCE_LIST ResourceList,
    PHC_RESOURCES HcResources
    )
/*++

Routine Description:

Arguments:

    FdoDeviceObject - DeviceObject for this USB controller
    ResourceList - Resources for this controller
    HcResources - Resources in the FDO ext

Return Value:

    NT status code.

--*/
{
    ULONG i = 0;
    ULONG addressSpace = 0;
    ULONG mpOptionFlags = 0;
    NTSTATUS ntStatus = STATUS_BOGUS;
    PHYSICAL_ADDRESS cardAddress;
    PDEVICE_EXTENSION devExt = NULL;
    PCM_PARTIAL_RESOURCE_LIST partResourceList = NULL;
    PCM_FULL_RESOURCE_DESCRIPTOR fullResourceDesc = NULL;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR memory = NULL;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR ioport = NULL;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR interrupt = NULL;

    DbgTrace((UsbpDebugTrace, "'enter USBPORT_GetResources\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    mpOptionFlags = devExt->Fdo.HcOptionFlags;

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'GRES', 0, 0, ResourceList);

    // init the resource list
    RtlZeroMemory(HcResources, sizeof(*HcResources));

    HcResources->HostControllerFdo = FdoDeviceObject;
    HcResources->HostControllerPdo = devExt->Fdo.PhysicalDeviceObject;
    HcResources->TargetDeviceObject = devExt->Fdo.TopOfStackDeviceObject;
    HcResources->DisablePciConfigSpace = (BOOLEAN)devExt->Fdo.DisablePciConfigSpace;

    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NO_PNP_RESOURCES)) {
        // no resources, bail with success
        TEST_TRAP();
        ntStatus = STATUS_SUCCESS;
        goto Exit;
    }

    if (NULL == ResourceList) {
        DbgTrace((UsbpDebugTrace, "'no resources, failing start.\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    fullResourceDesc = &ResourceList->List[0];
    partResourceList = &fullResourceDesc->PartialResourceList;

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'gres',
        partResourceList->Count, 0, partResourceList);

    for (i = 0; i < partResourceList->Count; i++) {

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'resT', i,
                 partResourceList->PartialDescriptors[i].Type, 0);

        switch (partResourceList->PartialDescriptors[i].Type) {

        case CmResourceTypePort:
            if (NULL == ioport) {
                ioport = &partResourceList->PartialDescriptors[i];
            }
            break;

        case CmResourceTypeMemory:
            if (NULL == memory) {
                memory = &partResourceList->PartialDescriptors[i];
            }
            break;

        case CmResourceTypeInterrupt:
            if (NULL == interrupt) {
                interrupt = &partResourceList->PartialDescriptors[i];
            }
            break;
        } // switch
    } // for

    // ensure controllers need either ioports or memory region
    if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NEED_IOPORT) &&
        TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NEED_MEMORY)) {
        DbgTrace((UsbpDebugTrace, "'controller needs both ioports and memory\n"));
        ntStatus = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    // only map resources this miniport actually needs
    if ((NULL != ioport) &&
        TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NEED_IOPORT)) {

        DbgTrace((UsbpDebugTrace, "'Port Resources Found @ %x'%x, %d Port Count\n",
            ioport->u.Port.Start.HighPart, ioport->u.Port.Start.LowPart,
            ioport->u.Port.Length));

        if (ioport->Flags & CM_RESOURCE_PORT_IO) {
            addressSpace = 1;
        }

        cardAddress = ioport->u.Port.Start;

        HcResources->DeviceRegistersLength = ioport->u.Port.Length;
        if (0 == addressSpace) {
            HcResources->DeviceRegisters = MmMapIoSpaceEx(cardAddress,
                                                          ioport->u.Port.Length,
                                                          PAGE_READWRITE | PAGE_NOCACHE);
        } else {
            HcResources->DeviceRegisters = (PVOID)(ULONG_PTR)cardAddress.QuadPart;
        }

        // see if we successfully mapped the IO regs
        if (NULL == HcResources->DeviceRegisters) {
            DbgTrace((UsbpDebugTrace, "'Couldn't map the device(port) registers. \n"));
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'Fmio', 0, 0, ntStatus);
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        DbgTrace((UsbpDebugTrace, "'Mapped device(port) registers to %p.\n",
            HcResources->DeviceRegisters));

        if (0 == addressSpace) {
            HcResources->Flags |= HCR_MAP_IO_REGS;
        }
        else {
            HcResources->Flags |= HCR_UNMAP_IO_REGS;
        }
    }

    if ((NULL != memory) &&
        TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NEED_MEMORY)) {

        DbgTrace((UsbpDebugTrace, "'Memory Resources Found @ %x'%x, Length = %x\n",
                        memory->u.Memory.Start.HighPart,
                        memory->u.Memory.Start.LowPart,
                        memory->u.Memory.Length));

        addressSpace = 0;
        cardAddress = memory->u.Memory.Start;
        HcResources->DeviceRegistersLength = memory->u.Memory.Length;

        HcResources->DeviceRegisters = MmMapIoSpaceEx(cardAddress,
                                                      memory->u.Memory.Length,
                                                      PAGE_READWRITE | PAGE_NOCACHE);
        if (NULL == HcResources->DeviceRegisters) {
            DbgTrace((UsbpDebugTrace, "'Couldn't map the device(memory) registers. \n"));
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'Fmmr', 0, 0, ntStatus);
            ntStatus = STATUS_NONE_MAPPED;
            goto Exit;
        }

        DbgTrace((UsbpDebugTrace, "'Mapped device(memory) registers to %p.\n",
            HcResources->DeviceRegisters));
        HcResources->Flags |= HCR_MEM_REGS;
    }

    if ((NULL != interrupt) &&
        TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_NEED_IRQ)) {

        DbgTrace((UsbpDebugTrace, "'Interrupt Resources Found!  Level = %x Vector = %x\n",
            interrupt->u.Interrupt.Level, interrupt->u.Interrupt.Vector));
        DbgTrace((UsbpDebugTrace, "'requesting interrupt vector %x level %x\n",
            interrupt->u.Interrupt.Level, interrupt->u.Interrupt.Vector));

        HcResources->Flags |= HCR_IRQ;
        HcResources->Affinity = interrupt->u.Interrupt.Affinity;
        HcResources->InterruptLevel = (KIRQL)interrupt->u.Interrupt.Level;
        HcResources->InterruptVector = interrupt->u.Interrupt.Vector;

        // Initialize the interrupt object for the controller
        HcResources->InterruptObject = NULL;
        HcResources->ShareIRQ = FALSE;
        if (CmResourceShareShared == interrupt->ShareDisposition) {
            HcResources->ShareIRQ = TRUE;
        }
        HcResources->InterruptMode = LevelSensitive;
        if (CM_RESOURCE_INTERRUPT_LATCHED == interrupt->Flags) {
            HcResources->InterruptMode = Latched;
        }
    }

    ntStatus = STATUS_SUCCESS;

Exit:

    //TEST_PATH(ntStatus, FAILED_GETRESOURCES);

    DbgTrace((UsbpDebugTrace, "'exit USBPORT_GetResources (%x)\n", ntStatus));
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'GRSd', 0, 0, ntStatus);

    return ntStatus;
}


NTSTATUS
USBPORT_FdoStart_Complete(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

    This routine is called when the port driver completes an IRP.

Arguments:

    DeviceObject - Pointer to the device object for the class device
    Irp - Irp completed.
    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    PKEVENT event = Context;
    PIO_STACK_LOCATION irpStack = NULL;

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_START_DEVICE);

    // signal the start device dispatch to finsh
    KeSetEvent(event, IO_NO_INCREMENT, FALSE);

    // defer completion
    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
USBPORT_FdoPnP(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Process the PNP IRPs sent to the FDO for the host
    controller.

Arguments:

    DeviceObject - pointer to a hcd device object (FDO)
    Irp - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_EXTENSION devExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    USBPORT_PNP_STATE lastHcPnpState;
    USBPORT_PNP_STATE lastRhPnpState;
    ULONG idx;
    UCHAR pnp_func;
    PDEVICE_OBJECT rhPdo;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    DbgTrace((UsbpDebugTrace, "'IRP_MJ_PNP %p\n", FdoDeviceObject));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'fPnP',
        irpStack->MinorFunction, 0, 0);

    pnp_func = irpStack->MinorFunction;
    devExt->CurrentPnpFunc = (ULONG) pnp_func;

    idx = InterlockedIncrement(&devExt->PnpFuncHistoryIdx);
    idx = idx % USB_PP_HISTORY_LEN;
    devExt->PnpFuncHistory[idx] = (UCHAR) pnp_func;

    usbIoRequest = USBPORT_Core_AllocIoRequest(FdoDeviceObject, Irp, UsbIoTag_FdoPnpIrp);
    if (usbIoRequest == NULL) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, NULL, ntStatus, 0);
        return ntStatus;
    }


    if (USBPORT_GetPnpState(devExt) == PnpRemoved) {
        ntStatus = STATUS_DELETE_PENDING;
        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, 0);
        return ntStatus;
    }

    switch (irpStack->MinorFunction) {

    case IRP_MN_START_DEVICE:
    {
        do {
            KEVENT pnpStartEvent;

            USBPORT_ASSERT(USBPORT_GetPnpState(devExt) != PnpStartFailed);

            LOGENTRY(FdoDeviceObject, LOG_PNP, 'str+', 0, devExt->PnpState, Irp);
            KeInitializeEvent(&pnpStartEvent, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);
            ntStatus = USBPORT_PassIrp(FdoDeviceObject, USBPORT_FdoStart_Complete,
                                       &pnpStartEvent, TRUE, TRUE, TRUE, Irp, usbIoRequest);
            if (STATUS_PENDING == ntStatus) {
                KeWaitForSingleObject(&pnpStartEvent, Executive, KernelMode,
                                      FALSE, NULL);
                ntStatus = Irp->IoStatus.Status;
            }
            //TEST_PATH(ntStatus, FAILED_LOWER_START);

            // already started (double start)
            if (USBPORT_GetPnpState(devExt) == PnpStarted) {
                // this sequence seems to be what verifier/pnpdtest want (returning success)

                LOGENTRY(FdoDeviceObject, LOG_PNP, 'DBLs', 0, devExt->PnpState, ntStatus);
                USBPORT_SetHcPnpState(FdoDeviceObject, PnpStarted, EvFDO_IRP_MN_START_DEVICE);
                ntStatus = STATUS_SUCCESS;
                break;
            }

            if (NT_ERROR(ntStatus)) {
                DbgTrace((UsbpDebugTrace, "'lower drivers failed start %x\n", ntStatus));
                LOGENTRY(FdoDeviceObject, LOG_PNP, 'stL!', 0, devExt->PnpState, ntStatus);
                USBPORT_SetHcPnpState(FdoDeviceObject, PnpStartFailed, EvFDO_IRP_MN_START_DEVICE);
                break;
            }

            ntStatus = USBPORT_DeferredStartDevice(FdoDeviceObject, Irp);
            if (!NT_SUCCESS(ntStatus)) {
                DbgTrace((UsbpDebugTrace, "'miniport failed start %x\n", ntStatus));
                USBPORT_SetHcPnpState(FdoDeviceObject, PnpStartFailed, EvFDO_IRP_MN_START_DEVICE);
                break;
            }

            // check for stop,start where the rhPDO exists, if so re-create the root hub device
            // handle
            if (USBPORT_GetPnpState(devExt) == PnpStopped) {

                KIRQL rh_irql;

                USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, &rh_irql);

                if (NULL != devExt->Fdo.RhPdoPtr) {

                    PDEVICE_EXTENSION   rhDevExt = NULL;

                    // use the current root hub PDO
                    USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, rh_irql);

                    GET_DEVICE_EXT(rhDevExt, devExt->Fdo.RhPdoPtr);
                    ASSERT_PDOEXT(rhDevExt);

                    if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {

                        // re-create the device handle if needed

                        ntStatus = USBPORT_CreateRootHubDeviceHandle(devExt->Fdo.RhPdoPtr);

                    }

                } else {
                    // no root PDO yet
                    USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, rh_irql);
                    ntStatus = STATUS_SUCCESS;
                    LOGENTRY(FdoDeviceObject, LOG_PNP, 'SpS1', 0, devExt->PnpState, ntStatus);
                }
                LOGENTRY(FdoDeviceObject, LOG_PNP, 'SpS2', 0, devExt->PnpState, ntStatus);
                DbgTrace((UsbpDebugTrace, "re-start failed%x\n", ntStatus));

                if (!NT_SUCCESS(ntStatus)) {
                    USBPORT_SetHcPnpState(FdoDeviceObject, PnpStartFailed, EvFDO_IRP_MN_RE_START_DEVICE);
                    break;
                }

                USBPORT_SetHcPnpState(FdoDeviceObject, PnpStarted, EvFDO_IRP_MN_RE_START_DEVICE);
            } else {

                USBPORT_SetHcPnpState(FdoDeviceObject, PnpStarted, EvFDO_IRP_MN_START_DEVICE);

            }

            USBPORT_RegisterAndEnableDeviceInterface(devExt,
                                                     devExt->Fdo.PhysicalDeviceObject,
                                                     &GUID_CLASS_USB_HOST_CONTROLLER);

            //
            // Create the legacy DOS-Devices FDO name
            //

            USBPORT_CreateLegacyFdoSymbolicLink(FdoDeviceObject);

            //
            // Register with WMI
            //

            USBPORT_WmiRegisterLog(FdoDeviceObject);

            ntStatus = STATUS_SUCCESS;
        } WHILE (0);
        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, 0);
        return ntStatus;
    }

    case IRP_MN_QUERY_STOP_DEVICE:
        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'qstp', 0, devExt->PnpState, ntStatus);
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:
        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'cstp', 0, devExt->PnpState, ntStatus);
        break;

    case IRP_MN_STOP_DEVICE:
        USBPORT_ASSERT(USBPORT_GetPnpState(devExt) == PnpStarted);

        USBPORT_SetHcPnpState(FdoDeviceObject, PnpStopped, EvFDO_IRP_MN_STOP_DEVICE);

        rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_Stop);
        if (rhPdo) {
            // remove root hub device handle
            USBPORT_RootHub_RemoveDevice(FdoDeviceObject, rhPdo);
            USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_Stop);
        }
        // drop ref for irp itself here for Stop Device
        // ref for IRP_MJ_PNP.IRP_MN_STOP_DEVICE freed here
        USBPORT_Core_DecIoRequestCount(FdoDeviceObject, usbIoRequest, TRUE);
        usbIoRequest = NULL;

        USBPORT_StopDevice(FdoDeviceObject);

        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'STOP', 0, devExt->PnpState, ntStatus);
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'qrmv', 0, devExt->PnpState, ntStatus);
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'crmv', 0, devExt->PnpState, ntStatus);
        break;

    case IRP_MN_SURPRISE_REMOVAL:

        DbgTrace((UsbpDebugTrace, " HC FDO (%p) surprise removed\n", FdoDeviceObject));

        //   PNP_NOT_STARTED or
        //   PNP_STARTED
        // ie we can be in this block after AddDevice or Stop

        lastHcPnpState = USBPORT_SetHcPnpState(FdoDeviceObject, PnpSurpriseRemoved, EvFDO_IRP_MN_SURPRISE_REMOVAL);

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'hcSR', 0, ntStatus, 0);
        // adjust the PDO for the FDO surprise remove, surprise removes always happen in order
        // child to parent

        rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_FdoSurpriseRemoval);

        if (rhPdo) {
            USBPORT_SetRhPnpState(rhPdo, PnpReportedMissing, EvFDO_IRP_MN_SURPRISE_REMOVAL);

            USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_FdoSurpriseRemoval);
        }


        if (lastHcPnpState != PnpStopped) {
            // drop ref for irp itself here for Stop Device
            // ref for IRP_MJ_PNP.IRP_MN_SURPRISE_REMOVAL freed here
            USBPORT_Core_DecIoRequestCount(FdoDeviceObject, usbIoRequest, TRUE);
            usbIoRequest = NULL;

            USBPORT_StopDevice(FdoDeviceObject);
        }

        USBPORT_DeregisterDeviceInterface(devExt);
        break;

    case IRP_MN_REMOVE_DEVICE:
        DbgTrace((UsbpDebugTrace, " HC FDO (%p) is being removed\n", FdoDeviceObject));

        lastHcPnpState = USBPORT_SetHcPnpState(FdoDeviceObject, PnpRemoved, EvFDO_IRP_MN_REMOVE_DEVICE);
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'hcRM', 0, 0, lastHcPnpState);

        // clean the pdo
        do {
            PDEVICE_EXTENSION rhDevExt = NULL;
            PDEVICE_OBJECT rhPDO;

            if (lastHcPnpState == PnpSurpriseRemoved) {
                USBPORT_SetRootHubPdo(FdoDeviceObject, NULL);
                break;
            }

            // remove the PDO currently associated with the host controller.
            rhPDO = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_Remove);
            if (NULL == rhPDO) {
                // no root hub PDO present
                break;
            }

            GET_DEVICE_EXT(rhDevExt, rhPDO);
            ASSERT_PDOEXT(rhDevExt);

            // stop root hub pdo

            lastRhPnpState = USBPORT_SetRhPnpState(rhPDO, PnpRemoved, EvFDO_IRP_MN_REMOVE_DEVICE);

            if (lastRhPnpState != PnpSurpriseRemoved) {
                if (lastHcPnpState == PnpStarted) {
                    USBPORT_StopRootHubPdo(FdoDeviceObject, rhPDO);
                }
                USBPORT_DeregisterDeviceInterface(rhDevExt);
            }

            // remove root hub pdo
            USBPORT_RootHub_RemoveDevice(FdoDeviceObject,
                                         rhPDO);
            // delete root hub pdo
            if (rhDevExt->Pdo.Descriptors) {
                UsbFreePool(rhDevExt->Pdo.Descriptors);
                rhDevExt->Pdo.Descriptors = NULL;
            }

            // ref for CreateRootHubPdo (IoCreateDeviceObject), USBPORT_CreateRootHubPdo
            USBPORT_Core_DecIoRequestCount(rhPDO, &rhDevExt->UsbIoPnpRemove, TRUE);
            KeWaitForSingleObject(&rhDevExt->IoRequestRemoveEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            USBPORT_SetRhPnpState(rhPDO, PnpDeleted, EvFDO_IRP_MN_REMOVE_DEVICE);
            USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPDO, REF_RhPDO_Remove);

            USBPORT_SetRootHubPdo(FdoDeviceObject, NULL);

            IoDeleteDevice(rhPDO);

        } WHILE (0);

        USBPORT_ClearBusContextList(FdoDeviceObject);

        // drop ref for irp itself here for Stop Device
        // ref for IRP_MJ_PNP.IRP_MN_REMOVE freed here
        USBPORT_Core_DecIoRequestCount(FdoDeviceObject, usbIoRequest, TRUE);
        usbIoRequest = NULL;

        if (lastHcPnpState != PnpSurpriseRemoved) {
            // fdo remove processing

            USBPORT_StopDevice(FdoDeviceObject);
            USBPORT_DeregisterDeviceInterface(devExt);
        }

        //
        // Let the miniport know that we are deleting the device data
        // Note: This was added for Wireless USB
        //
        MPf_DeleteDeviceData(devExt);




        Irp->IoStatus.Status = STATUS_SUCCESS;
        // pass on remove
        ntStatus = USBPORT_PassIrp(FdoDeviceObject, NULL, NULL,
                                   TRUE, TRUE, TRUE, Irp, usbIoRequest);


        // ref for ADD-Device, UsbIoTag_PnpAdd
        USBPORT_Core_DecIoRequestCount(FdoDeviceObject, &devExt->UsbIoPnpRemove, TRUE);

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'watP', 0, 0, FdoDeviceObject);
        KeWaitForSingleObject(&devExt->IoRequestRemoveEvent,
                              Suspended,
                              KernelMode,
                              FALSE,
                              NULL);
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'waPD', 0, 0, FdoDeviceObject);

        USBPORT_SetHcPnpState(FdoDeviceObject, PnpDeleted, EvFDO_IRP_MN_REMOVE_DEVICE);

        if (devExt->Fdo.HcResetWI != NULL) {
            USBPORT_FreeWorkItem(devExt->Fdo.HcResetWI);
            devExt->Fdo.HcResetWI = NULL;
        }
        if (devExt->Fdo.PhysicalDeviceObjectName.Buffer != NULL) {
            UsbFreePool(devExt->Fdo.PhysicalDeviceObjectName.Buffer);
            devExt->Fdo.PhysicalDeviceObjectName.Buffer = NULL;
        }

        USBPORT_RuntimePMContextFree(FdoDeviceObject);

        if (devExt->Fdo.SimulateInterruptHighResTimer != NULL) {
            USBPORT_FreeHighResTimer(devExt->Fdo.SimulateInterruptHighResTimer, TRUE);
        }

        // last chance to debug with the log
        TEST_TRAP();
        USBPORT_LogFree(FdoDeviceObject, &devExt->Log);

        IoDetachDevice(devExt->Fdo.TopOfStackDeviceObject);

        if (devExt->Fdo.HsbControllerContextPtr) {
            UsbFreePool(devExt->Fdo.HsbControllerContextPtr);
        }
        DbgTrace((UsbpDebugTrace, "'Deleting HC FDO (%p) now.\n", FdoDeviceObject));
        IoDeleteDevice(FdoDeviceObject);
        return ntStatus;

    case IRP_MN_QUERY_DEVICE_RELATIONS:
        DbgTrace((UsbpDebugTrace, "'IRP_MN_QUERY_DEVICE_RELATIONS %p %x\n",
                        FdoDeviceObject,
                        irpStack->Parameters.QueryDeviceRelations.Type));
        switch(irpStack->Parameters.QueryDeviceRelations.Type) {
        case BusRelations:
        {
            ULONG newSize = 0;
            PDEVICE_RELATIONS oldRel = NULL;
            PDEVICE_RELATIONS newRel = NULL;
            PDEVICE_OBJECT newRhPdo;
            KIRQL rh_irql;

            oldRel = (PDEVICE_RELATIONS)Irp->IoStatus.Information;

            do {

                USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, &rh_irql);

                if (NULL != devExt->Fdo.RhPdoPtr) {

                    PDEVICE_EXTENSION   rhDevExt = NULL;

                    // use the current root hub PDO
                    USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, rh_irql);

                    GET_DEVICE_EXT(rhDevExt, devExt->Fdo.RhPdoPtr);
                    ASSERT_PDOEXT(rhDevExt);

                    if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {

                        // re-create the device handle if needed

                        ntStatus = USBPORT_CreateRootHubDeviceHandle(devExt->Fdo.RhPdoPtr);

                        if (!NT_SUCCESS(ntStatus)) {
                            break;
                        }
                    }

                } else {

                    // create new root hub PDO
                    USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.RhPdoPtrSpin, rh_irql);

                    ntStatus = USBPORT_CreateRootHubPdo(FdoDeviceObject, &newRhPdo);

                    if (NT_ERROR(ntStatus)) {
                        break;

                    } else {
                        PDEVICE_OBJECT oldRhPdo;

                        oldRhPdo = USBPORT_SetRootHubPdo(FdoDeviceObject, newRhPdo);
                        // this would mean QBR was reentered and PnP is not supposed to do this.
                        USBPORT_ASSERT(NULL == oldRhPdo);
                    }
                }

                newSize = sizeof(DEVICE_RELATIONS);

                if (oldRel && oldRel->Count) {
                    //
                    // Add root hub PDO to pre-existing relation list
                    // passed down from driver layered above.
                    //
                    ntStatus = RtlULongAdd(newSize,
                                           sizeof(PDEVICE_OBJECT) * oldRel->Count,
                                           &newSize);

                    if (!NT_SUCCESS(ntStatus)) {
                        break;
                    }
                }

                ALLOC_POOL_OSOWNED(&newRel, PagedPool, newSize);
                if (NULL == newRel) {
                    ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                newRel->Count = 1;
                newRel->Objects[0] = devExt->Fdo.RhPdoPtr;
                ObReferenceObject(devExt->Fdo.RhPdoPtr);

                if (oldRel && oldRel->Count) {
                    //
                    // Copy pre-existing relation list passed down from
                    // driver layered above to end of new root hub PDO
                    // relation list.
                    //
                    newRel->Count = oldRel->Count + 1;
                    RtlCopyMemory((PUCHAR)newRel + sizeof(DEVICE_RELATIONS),
                                  oldRel->Objects,
                                  oldRel->Count * sizeof(PDEVICE_OBJECT));
                }
                ntStatus = STATUS_SUCCESS;
            } WHILE (0);

            if (oldRel) {
                //
                // Free pre-existing relation list passed down from
                // driver layered above.
                //
                ExFreePool(oldRel);
            }

            if (!NT_SUCCESS(ntStatus)) {
                USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, 0);

                USBPORT_SetHcPnpState(FdoDeviceObject, devExt->PnpState, EvFDO_IRP_MN_QBR_RH_ERROR);

                return ntStatus;
            }

            USBPORT_SetHcPnpState(FdoDeviceObject, devExt->PnpState, EvFDO_IRP_MN_QBR_RH);

            Irp->IoStatus.Status = ntStatus;
            Irp->IoStatus.Information = (ULONG_PTR)newRel;

            DbgTrace((UsbpDebugTrace, "'IRP_MN_QUERY_DEVICE_RELATIONS %p BusRelations\n", FdoDeviceObject));
            break;
        } // BusRelations
        case TargetDeviceRelation:
        case RemovalRelations:
        case EjectionRelations:
        default:
            break;
        } // switch(irpStack->Parameters.QueryDeviceRelations.Type)
        break;

    case IRP_MN_QUERY_CAPABILITIES:
    {
        PDEVICE_CAPABILITIES pdc = NULL;

        pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;

        if (NULL == pdc ||
            pdc->Version < 1 ||
            pdc->Size < sizeof(DEVICE_CAPABILITIES)) {

            ntStatus = STATUS_UNSUCCESSFUL;

        } else {

            KEVENT event;

            KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);
            IoCopyCurrentIrpStackLocationToNext(Irp);

            ntStatus = USBPORT_PassIrp(FdoDeviceObject, USBPORT_DeferIrpCompletion,
                                       &event, TRUE, TRUE, TRUE, Irp, usbIoRequest);

            if (ntStatus == STATUS_PENDING) {
                KeWaitForSingleObject(&event,
                                      Executive,
                                      KernelMode,
                                      FALSE,
                                      NULL);

                ntStatus = Irp->IoStatus.Status;
            }

            if (NT_SUCCESS(ntStatus)) {
                RtlCopyMemory(&devExt->DeviceCapabilities,
                              pdc,
                              sizeof(DEVICE_CAPABILITIES));
            }
        }

        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, 0);
        return ntStatus;
    }

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:
    {
        // Pass it down the stack first

        DEVICE_USAGE_NOTIFICATION_TYPE dunType;
        KEVENT event;

        dunType = irpStack->Parameters.UsageNotification.Type;

        KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);
        ntStatus = USBPORT_PassIrp(FdoDeviceObject, USBPORT_DeferIrpCompletion,
                                   &event, TRUE, TRUE, TRUE, Irp, usbIoRequest);

        if (ntStatus == STATUS_PENDING) {
            KeWaitForSingleObject(&event,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            ntStatus = Irp->IoStatus.Status;
        }

        if (NT_SUCCESS(ntStatus) && DunTypeSpecialFile(dunType)) {

            // Initialize the emergency worker thread
            if (irpStack->Parameters.UsageNotification.InPath) {
                ntStatus = USBPORT_InitializeEmergencyWorkerThread();
            }

            if (NT_SUCCESS(ntStatus)) {

                // The lower stack has processed the IRP successfully
                IoAdjustPagingPathCount(&devExt->Fdo.InPathCount,
                                        irpStack->Parameters.UsageNotification.InPath);

                if (irpStack->Parameters.UsageNotification.Type ==
                        DeviceUsageTypeHibernation)
                {
                    IoAdjustPagingPathCount(&devExt->Fdo.HibernateCount,
                                            irpStack->Parameters.UsageNotification.InPath);
                }

                if (devExt->Fdo.InPathCount) {
                    FdoDeviceObject->Flags &= ~DO_POWER_PAGABLE;
                } else {
                    FdoDeviceObject->Flags |= DO_POWER_PAGABLE;
                }
            }
        }

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'duno', 0,
            devExt->PnpState, ntStatus);

        USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, 0);
        return ntStatus;
    }

    case IRP_MN_QUERY_PNP_DEVICE_STATE:

        if (devExt->Fdo.PowerFlags.MpRestartFailed) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'PDF!', 0, 0, FdoDeviceObject);
            Irp->IoStatus.Information |= PNP_DEVICE_FAILED;
            Irp->IoStatus.Status = STATUS_SUCCESS;

        } else if (devExt->Fdo.Flags.ControllerGone) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'RMD!', 0, 0, FdoDeviceObject);
            Irp->IoStatus.Information |= PNP_DEVICE_REMOVED;
            Irp->IoStatus.Status = STATUS_SUCCESS;

        }

        if (devExt->Fdo.HcResources.DisablePciConfigSpace != 0) {
            KEVENT event;

            KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

            ntStatus = USBPORT_PassIrp(FdoDeviceObject, USBPORT_DeferIrpCompletion,
                                       &event, TRUE, TRUE, TRUE, Irp, usbIoRequest);

            if (ntStatus == STATUS_PENDING) {
                KeWaitForSingleObject(&event,
                                      Executive,
                                      KernelMode,
                                      FALSE,
                                      NULL);

                ntStatus = Irp->IoStatus.Status;
            }

            // Due to a long-standing bug in ACPI, it is possible that the PNP flags set
            // for this IRP on the way down were cleared by ACPI.  Set the flags here again
            // to insure they are propagated back to the PnP Manager.
            if (NT_SUCCESS(ntStatus)) {
                if (devExt->Fdo.PowerFlags.MpRestartFailed) {
                    Irp->IoStatus.Information |= PNP_DEVICE_FAILED;
                } else if (devExt->Fdo.Flags.ControllerGone) {
                    Irp->IoStatus.Information |= PNP_DEVICE_REMOVED;
                }
            }

            USBPORT_Core_IoCompleteIrp(FdoDeviceObject, Irp, usbIoRequest, ntStatus, Irp->IoStatus.Information);
            return ntStatus;
        }
        break;

    default:
        USBPORT_ASSERT(devExt->Fdo.TopOfStackDeviceObject != NULL);
        DbgTrace((UsbpDebugTrace, "'UNKNOWN PNP MESSAGE (%x)\n", irpStack->MinorFunction));
        break;
    } // irpStack->MinorFunction

    ntStatus = USBPORT_PassIrp(FdoDeviceObject, NULL, NULL,
                               TRUE, TRUE, TRUE, Irp, usbIoRequest);
    return ntStatus;
}


NTSTATUS
USBPORT_DeferredStartDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    This function is called as a result of MN_START_DEVICE,
    it is called after successful completion of the START
    irp by the lower drivers.

Arguments:

    FdoDeviceObject - DeviceObject for this USB controller.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS ntStatus = STATUS_BOGUS;
    PDEVICE_EXTENSION devExt = NULL;
    PCM_RESOURCE_LIST xlatedResources = NULL;
    PIO_STACK_LOCATION irpStack = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    xlatedResources = irpStack->Parameters.StartDevice.AllocatedResourcesTranslated;

    ntStatus = USBPORT_GetResources(FdoDeviceObject,
                                    xlatedResources,
                                    &devExt->Fdo.HcResources);
    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    // got resources, start the port driver,
    // connect the interrupt and start miniport.
    ntStatus = USBPORT_StartDevice(FdoDeviceObject,
                                   &devExt->Fdo.HcResources);
    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    if (USBPORT_IS_USB20(devExt)) {
        USBPORT_RegisterUSB2fdo(FdoDeviceObject);
    } else {
        USBPORT_RegisterUSB1fdo(FdoDeviceObject);
    }
    ntStatus = STATUS_SUCCESS;

Exit:

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'dfST', 0, 0, ntStatus);
    return ntStatus;
}


PWCHAR
USB_MakeId(
    PDEVICE_OBJECT FdoDeviceObject,
    __in PCWSTR IdString,
    __in_bcount_opt(*Length) PWCHAR Buffer,
    __inout PULONG Length,
    USHORT NullCount,
    USHORT Digits,
    USHORT HexId
    )
/*
    given a wide Id string like "FOOnnnn\0"
    add the HexId value to nnnn as hex
    this string is appended to the buffer passed in

    eg
    in  : FOOnnnn\0 , 0x123A
    out : FOO123A\0
*/
{
#define NIBBLE_TO_HEX( byte ) ((WCHAR)Nibble[byte])
    CONST UCHAR Nibble[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
        'B', 'C', 'D', 'E', 'F'};

    PWCHAR tmp, id;
    PUCHAR p;
    ULONG siz, idLen;
    NTSTATUS nts;

    idLen = (ULONG)wcslen(IdString) * sizeof(WCHAR);

    nts = RtlULongAdd(idLen, *Length, &siz);

    if (NT_SUCCESS(nts)) {
        nts = RtlULongAdd(siz, NullCount * sizeof(WCHAR), &siz);
    }

    if (NT_SUCCESS(nts)) {
        ALLOC_POOL_OSOWNED(&tmp, PagedPool, siz);
    } else {
        tmp = NULL;
    }

    if (tmp == NULL) {
        *Length = 0;
    } else {
        // this takes care of the nulls
        RtlCopyMemory(tmp, Buffer, *Length);
        p = (PUCHAR) tmp;
        p += *Length;
        RtlCopyMemory(p, IdString, idLen);
        id = (PWCHAR) p;
        *Length = siz;

        // now convert the vaules
        while (*id != (WCHAR)'n' && Digits) {
            id++;
        }

        switch(Digits) {
        case 2:
            *(id) = NIBBLE_TO_HEX((HexId >> 4) & 0x000f);
            *(id+1) =  NIBBLE_TO_HEX(HexId & 0x000f);
            break;
        case 4:
            *(id) = NIBBLE_TO_HEX(HexId >> 12);
            *(id+1) = NIBBLE_TO_HEX((HexId >> 8) & 0x000f);
            *(id+2) = NIBBLE_TO_HEX((HexId >> 4) & 0x000f);
            *(id+3) =  NIBBLE_TO_HEX(HexId & 0x000f);
            break;
        }
    }

    if (Buffer != NULL) {
        UsbFreePool(Buffer);
    }

    return tmp;
#undef NIBBLE_TO_HEX
}



PWCHAR
USBPORT_GetIdString(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT Vid,
    USHORT Pid,
    USHORT Rev
    )
/*++

Routine Description:

    Make an id string for PnP

Arguments:

Return Value:

    NT Status code.

--*/
{
    PWCHAR id = NULL;
    ULONG length = 0;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // we need to generate the following series of strings

    // USB\\ROOT_HUB&VIDnnnn&PIDnnnn&REVnnnn\0
    // USB\\ROOT_HUB&VIDnnnn&PIDnnnn\0
    // USB\\ROOT_HUB\0\0

    // allocate space for the three id plus a NULL

    // Vid, Pid, and Rev are only valid if access to PCI Config Space
    // has not been disabled for this controller.
    //
    if (devExt->Fdo.HcResources.DisablePciConfigSpace == 0) {

        // USB\\ROOT_HUB&VIDnnnn&PIDnnnn&REVnnnn\0
        if (USBPORT_IS_USB20(devExt)) {
            id = USB_MakeId(FdoDeviceObject,
                            L"USB\\ROOT_HUB20&VIDnnnn\0",
                            id,
                            &length,
                            0,
                            4,  // four digits
                            Vid);
        } else {
            id = USB_MakeId(FdoDeviceObject,
                            L"USB\\ROOT_HUB&VIDnnnn\0",
                            id,
                            &length,
                            0,
                            4,  // four digits
                            Vid);
        }

        id = USB_MakeId(FdoDeviceObject,
                        L"&PIDnnnn\0",
                        id,
                        &length,
                        0,
                        4,   // four digits
                        Pid);

        id = USB_MakeId(FdoDeviceObject,
                        L"&REVnnnn\0",
                        id,
                        &length,
                        1,   // add a NULL
                        4,   // four digits
                        Rev);

        // USB\\ROOT_HUB&VIDnnnn&PIDnnnn\0
        if (USBPORT_IS_USB20(devExt)) {
            id = USB_MakeId(FdoDeviceObject,
                            L"USB\\ROOT_HUB20&VIDnnnn\0",
                            id,
                            &length,
                            0,
                            4,  // four digits
                            Vid);
        } else {
            id = USB_MakeId(FdoDeviceObject,
                            L"USB\\ROOT_HUB&VIDnnnn\0",
                            id,
                            &length,
                            0,
                            4,  // four digits
                            Vid);
        }

        id = USB_MakeId(FdoDeviceObject,
                        L"&PIDnnnn\0",
                        id,
                        &length,
                        1,
                        4,   // four digits
                        Pid);
    }

    // USB\\ROOT_HUB\0\0
    if (USBPORT_IS_USB20(devExt)) {
        id = USB_MakeId(FdoDeviceObject,
                       L"USB\\ROOT_HUB20\0",
                       id,
                       &length,
                       2,  // double null
                       0,  // no digits
                       0);
    } else {
        id = USB_MakeId(FdoDeviceObject,
                       L"USB\\ROOT_HUB\0",
                       id,
                       &length,
                       2,  // double null
                       0,  // no digits
                       0);
    }

    return(id);
}


NTSTATUS
USBPORT_PdoPnP(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Disptach routine for PnP Irps sent to the PDO for the root hub.

Arguments:

    PdoDeviceObject - Pdo for the root hub

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS ntStatus = STATUS_BOGUS;
    ULONG_PTR info = 0;
    POWER_STATE newState;
    PDEVICE_EXTENSION devExt = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    PDEVICE_CAPABILITIES devCap = NULL;
    ULONG idx;
    UCHAR pnp_func;
    USBPORT_PNP_STATE lastRhPnpState;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    info = Irp->IoStatus.Information;
    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);
    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    pnp_func = irpStack->MinorFunction;
    rhDevExt->CurrentPnpFunc = (ULONG) pnp_func;

    idx = InterlockedIncrement(&rhDevExt->PnpFuncHistoryIdx);
    idx = idx % USB_PP_HISTORY_LEN;
    rhDevExt->PnpFuncHistory[idx] = (UCHAR) pnp_func;

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);

    usbIoRequest = USBPORT_Core_AllocIoRequest(PdoDeviceObject, Irp, UsbIoTag_PdoPnpIrp);
    if (usbIoRequest == NULL) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, NULL, ntStatus, 0);
        return ntStatus;
    }

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:
        DbgTrace((UsbpDebugTrace, " Starting Root hub PDO %p\n", PdoDeviceObject));

        rhDevExt->Pdo.SelectiveSuspend = FALSE;

        if (USBPORT_GetPnpState(rhDevExt) == PnpStarted) {
            //Double Start
            TEST_TRAP();
            ntStatus = STATUS_SUCCESS;
            break;
        }

        // clear the diag mode flag on start to re-enable cc handoff
        //devExt->Fdo.Flags.HctDiagMode = 0;
        //devExt->Fdo.Flags.HctForcePortsHighSpeed = 0;
        // refresh hct keys on root hub start
        USBPORT_Check_ForcePortsHighSpeed_Enabled(rhDevExt->HcFdoDeviceObject);
        USBPORT_Check_DCA_Enabled(rhDevExt->HcFdoDeviceObject);


        if (USBPORT_IS_USB20(devExt)) {
            // take control of USB 2 shared ports, normally the ehci driver does
            // this on a start but we need to do it here if this is a restart of
            // just the root hub.
            MPf_TakePortControl(devExt);

        }
        // consider ourselves powered when started
        rhDevExt->Dx_State = newState.DeviceState = PowerDeviceD0;

        PoSetPowerState(PdoDeviceObject, DevicePowerState, newState);

        if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {

            // re-create the device handle if needed

            ntStatus = USBPORT_CreateRootHubDeviceHandle(PdoDeviceObject);

            if (!NT_SUCCESS(ntStatus)) {

                USBPORT_SetRhPnpState(PdoDeviceObject,
                                      PnpStartFailed,
                                      EvPDO_IRP_MN_START_DEVICE);
                break;
            }
        }

        if (rhDevExt->Pdo.RootHubDeviceHandle->PhysicalDeviceObjectName.Buffer == NULL) {

            USBPORT_GetPhysicalDeviceObjectName(PdoDeviceObject,
                                                &rhDevExt->Pdo.RootHubDeviceHandle->PhysicalDeviceObjectName);
        }

        // enable device interface for user mode
        USBPORT_RegisterAndEnableDeviceInterface(rhDevExt,
                                                 PdoDeviceObject,
                                                 &GUID_CLASS_USBHUB);

        //if(!TEST_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_REGISTER)) {
        //    ntStatus = IoRegisterDeviceInterface(
        //                        PdoDeviceObject,
        //                        &GUID_CLASS_USBHUB, NULL,
        //                        &rhDevExt->SymbolicLinkName);
        //    if(NT_SUCCESS(ntStatus)) {
        //        SET_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_REGISTER);
        //    }
        //}

        //if(TEST_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_REGISTER) &&
        //   !TEST_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_ENABLE)) {
        //     ntStatus = IoSetDeviceInterfaceState(&rhDevExt->SymbolicLinkName,
        //                                         TRUE);
        //    if(NT_SUCCESS(ntStatus)) {
        //        SET_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_ENABLE);
        //    }
        // }

        USBPORT_SetRhPnpState(PdoDeviceObject, PnpStarted, EvPDO_IRP_MN_START_DEVICE);
        //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STARTED);
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_STOP_DEVICE:
        DbgTrace((UsbpDebugTrace, " Root Hub PDO %p is being stopped\n", PdoDeviceObject));

        //USBPORT_ASSERT(TEST_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STARTED));
        //USBPORT_ASSERT(TEST_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_PRESENT));
        //CLEAR_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STARTED);
        //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STOPPED);

        USBPORT_SetRhPnpState(PdoDeviceObject, PnpStopped, EvPDO_IRP_MN_STOP_DEVICE);
        if (USBPORT_GetPnpState(devExt) == PnpStarted) {
            USBPORT_StopRootHubPdo(rhDevExt->HcFdoDeviceObject, PdoDeviceObject);
        }
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        ntStatus = STATUS_SUCCESS;
        break;

   case IRP_MN_SURPRISE_REMOVAL:
        DbgTrace((UsbpDebugTrace," Surprise Removal (PDO) %p\n", PdoDeviceObject));

        //CLEAR_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STARTED);
        //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_SURPRISE_REMOVED);

        lastRhPnpState = USBPORT_SetRhPnpState(PdoDeviceObject, PnpSurpriseRemoved, EvPDO_IRP_MN_SURPRISE_REMOVAL);

        if (lastRhPnpState != PnpStopped) {
            if (USBPORT_GetPnpState(devExt) == PnpStarted) {
                USBPORT_StopRootHubPdo(rhDevExt->HcFdoDeviceObject, PdoDeviceObject);
            }
            USBPORT_DeregisterDeviceInterface(rhDevExt);
        }

        //if (!TEST_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_STOPPED)) {
        //   USBPORT_StopRootHubPdo(rhDevExt->HcFdoDeviceObject, PdoDeviceObject);
        //    if (TEST_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_REGISTER)) {
        //        RtlFreeUnicodeString(&rhDevExt->SymbolicLinkName);
        //        CLEAR_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_REGISTER);
        //    }
        //}

        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_REMOVE_DEVICE:
        DbgTrace((UsbpDebugTrace, " Root Hub PDO (%p) remove\n", PdoDeviceObject));

        lastRhPnpState = USBPORT_SetRhPnpState(PdoDeviceObject, PnpRemoved, EvPDO_IRP_MN_REMOVE_DEVICE);

        if (lastRhPnpState != PnpReportedMissing) {
            USBPORT_SetRhPnpState(PdoDeviceObject, PnpSoftRemoved, EvPDO_IRP_MN_REMOVE_DEVICE);

            // after query, remove is only a fake remove
            //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_FAKE_REMOVED);
            // soft remove unloads the hub so we need to clean up this as well
            if (USBPORT_GetPnpState(devExt) == PnpStarted) {
                USBPORT_StopRootHubPdo(rhDevExt->HcFdoDeviceObject, PdoDeviceObject);
            }
            ntStatus = STATUS_SUCCESS;
            break;
        }

        // remove root hub device
        USBPORT_RootHub_RemoveDevice(rhDevExt->HcFdoDeviceObject,
                                     PdoDeviceObject);

        if (rhDevExt->Pdo.Descriptors) {
            UsbFreePool(rhDevExt->Pdo.Descriptors);
            rhDevExt->Pdo.Descriptors = NULL;
        }


        // free Faildata
        if (rhDevExt->Pdo.FailData) {
            UsbFreePool(rhDevExt->Pdo.FailData);
            rhDevExt->Pdo.FailData = NULL;
        }

        // delete the pdo

        // ref for IRP_MJ_PNP.IRP_MN_REMOVE - free now since we will be doing a delete
        USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, Irp);
        usbIoRequest = NULL;
        // ref for CreateRootHubPdo (IoCreateDeviceObject), USBPORT_CreateRootHubPdo
        USBPORT_Core_DecIoRequestCount(PdoDeviceObject, &rhDevExt->UsbIoPnpRemove, TRUE);

        KeWaitForSingleObject(&rhDevExt->IoRequestRemoveEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        USBPORT_SetRhPnpState(PdoDeviceObject, PnpDeleted, EvPDO_IRP_MN_REMOVE_DEVICE);

        IoDeleteDevice(PdoDeviceObject);
        PdoDeviceObject = NULL;
        ntStatus = STATUS_SUCCESS;

        break;

    case IRP_MN_QUERY_ID:
        DbgTrace((UsbpDebugTrace, "'IRP_MN_QUERY_ID\n"));
        switch (irpStack->Parameters.QueryId.IdType) {
        case BusQueryDeviceID:
        {
            ULONG siz = 0;
            PWCHAR id = NULL;
            PWCHAR deviceId = NULL;
            WCHAR rootHubDeviceId[] = L"USB\\ROOT_HUB\0";
            WCHAR rootHubDeviceId_20[] = L"USB\\ROOT_HUB20\0";

            GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
            ASSERT_FDOEXT(devExt);

            id = &rootHubDeviceId[0];
            siz = sizeof(rootHubDeviceId);
            if (USBPORT_IS_USB20(devExt)) {
                id = &rootHubDeviceId_20[0];
                siz = sizeof(rootHubDeviceId_20);
            }

            ALLOC_POOL_OSOWNED(&deviceId, PagedPool, siz);
            if (deviceId) {
                RtlCopyMemory(deviceId, id, siz);
            }
            // device id for root hub is USB\ROOT_HUB
            info = (ULONG_PTR) deviceId;
            LOGENTRY(rhDevExt->HcFdoDeviceObject, LOG_PNP,
                'DVid', info, 0, 0);
            ntStatus = STATUS_SUCCESS;
            break;
        }

        case BusQueryHardwareIDs:
        {
            // generate hardware id for root hub
            //
            // A host controllers root hub VID,PID,REV is derived
            // from the controllers PCI VID,DEV,REV  that is:
            // root hub VID = hc VID (vendor id)
            // root hub PID = hc DEV (device id)
            // root hub REV = hc REV (revision id)
            //
            // this allows filter drivers to be loaded on
            // specific root hub instances.

            // for HW IDs we generate:
            // USB\PORT_ROOT_HUB&VIDnnnn&PIDnnnn&REVnnnn
            // USB\PORT_ROOT_HUB&VIDnnnn&PIDnnnn
            // USB\PORT_ROOT_HUB
            //
            GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
            ASSERT_FDOEXT(devExt);

            info = (ULONG_PTR)USBPORT_GetIdString(
                        rhDevExt->HcFdoDeviceObject,
                        devExt->Fdo.PciVendorId,
                        devExt->Fdo.PciDeviceId,
                        (USHORT)devExt->Fdo.PciRevisionId);

            LOGENTRY(rhDevExt->HcFdoDeviceObject, LOG_PNP,
                'HWid', info, 0, 0);
            ntStatus = STATUS_SUCCESS;
            break;
        }

        case BusQueryCompatibleIDs:
            info = 0;
            ntStatus = STATUS_SUCCESS;
            break;

        case BusQueryInstanceID:
            info = 0;
            ntStatus = STATUS_SUCCESS;
            break;

        default:
            ntStatus = Irp->IoStatus.Status;
            break;
        }

        break;

    case IRP_MN_QUERY_DEVICE_RELATIONS:

        DbgTrace((UsbpDebugTrace," IRP_MN_QUERY_DEVICE_RELATIONS (PDO) %p %x\n",
                PdoDeviceObject,
                irpStack->Parameters.QueryDeviceRelations.Type));
        switch(irpStack->Parameters.QueryDeviceRelations.Type) {
        case TargetDeviceRelation:
        {
            PDEVICE_RELATIONS devRel = NULL;
            ALLOC_POOL_OSOWNED(&devRel, PagedPool, sizeof(DEVICE_RELATIONS));
            if (NULL == devRel) {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                info = 0;
                break;
            }
            // return a reference to ourselves
            devRel->Count = 1;
            ObReferenceObject(PdoDeviceObject);
            devRel->Objects[0] = PdoDeviceObject;
            ntStatus = STATUS_SUCCESS;
            DbgTrace((UsbpDebugTrace, " TargetDeviceRelation to Root Hub PDO - complt\n"));
            info = (ULONG_PTR)devRel;
            ntStatus = STATUS_SUCCESS;
            break;
        } // TargetDeviceRelation
        case BusRelations:
        case RemovalRelations:
        case EjectionRelations:
        default:
            ntStatus = Irp->IoStatus.Status;
            break;
        } // switch
        break;

    case IRP_MN_QUERY_CAPABILITIES:
        DbgTrace((UsbpDebugTrace, "'IRP_MN_QUERY_CAPABILITIES (rh PDO)\n"));
        devCap = irpStack->Parameters.DeviceCapabilities.Capabilities;
        RtlCopyMemory(devCap,
                      &rhDevExt->DeviceCapabilities,
                      sizeof(DEVICE_CAPABILITIES));
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_INTERFACE:
        DbgTrace((UsbpDebugTrace," IRP_MN_QUERY_INTERFACE (PDO) %p\n", PdoDeviceObject));
        ntStatus = USBPORT_GetBusInterface(rhDevExt->HcFdoDeviceObject,
                                           PdoDeviceObject,
                                           Irp);
        break;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:
    {
        DEVICE_USAGE_NOTIFICATION_TYPE dunType;
        BOOLEAN inPath;
        PIRP irp2;
        PIO_STACK_LOCATION irpStack2;
        PDEVICE_OBJECT deviceObject;
        KEVENT event;

        ntStatus = STATUS_SUCCESS;

        dunType = irpStack->Parameters.UsageNotification.Type;
        inPath = irpStack->Parameters.UsageNotification.InPath;

        // Get the top of the FDO stack
        deviceObject = IoGetAttachedDevice(rhDevExt->HcFdoDeviceObject);

        // Allocate and initialize a new irp for this operation.
        //
        irp2 = IoAllocateIrp(deviceObject->StackSize, FALSE);
        if (!irp2) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus)) {
            // Fill in the service independent parameters in the IRP.
            irp2->IoStatus.Status = STATUS_NOT_SUPPORTED;
            KeInitializeEvent(&event, NotificationEvent, FALSE);
            IoSetCompletionRoutine(irp2,
                                   USBPORT_DeferIrpCompletion,
                                   &event,
                                   TRUE,
                                   TRUE,
                                   TRUE);


            // Duplicate the original function codes and parameters.
            irpStack2 = IoGetNextIrpStackLocation(irp2);
            irpStack2->MajorFunction = irpStack->MajorFunction;
            irpStack2->MinorFunction = irpStack->MinorFunction;
            irpStack2->FileObject = irpStack->FileObject;
            irpStack2->Parameters.UsageNotification.InPath = inPath;
            irpStack2->Parameters.UsageNotification.Type = dunType;

            // Now send irp2 down the stack synchronously
            //
            if (IoCallDriver(deviceObject, irp2) == STATUS_PENDING) {

                KeWaitForSingleObject(&event,
                                      Suspended,
                                      KernelMode,
                                      FALSE,
                                      NULL);
            }

            ntStatus = irp2->IoStatus.Status;

            // Free irp2
            //
            IoFreeIrp(irp2);
        }

        if (NT_SUCCESS(ntStatus) && DunTypeSpecialFile(dunType)) {

            // The lower stack has processed the IRP successfully
            IoAdjustPagingPathCount(&rhDevExt->Pdo.InPathCount, inPath);

            if (rhDevExt->Pdo.InPathCount) {
                PdoDeviceObject->Flags &= ~DO_POWER_PAGABLE;
            } else {
                PdoDeviceObject->Flags |= DO_POWER_PAGABLE;
            }
        }

        break;

    }

    case IRP_MN_QUERY_BUS_INFORMATION:
    {
        // return the standard USB GUID
        PPNP_BUS_INFORMATION busInfo;

        ALLOC_POOL_OSOWNED(&busInfo, PagedPool,
                           sizeof(PNP_BUS_INFORMATION));

        if (busInfo == NULL) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        } else {
            busInfo->BusTypeGuid = GUID_BUS_TYPE_USB;
            busInfo->LegacyBusType = PNPBus;
            busInfo->BusNumber = 0;

            ntStatus = STATUS_SUCCESS;
            info = (ULONG_PTR) busInfo;
        }
        break;
    }

    default:
        DbgTrace((UsbpDebugTrace, " PnP IOCTL(%d) to root hub PDO not handled\n",
            irpStack->MinorFunction));
        ntStatus = Irp->IoStatus.Status;
        break;
    } // irpStack->MinorFunction

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, usbIoRequest, ntStatus, info);
    return ntStatus;
}


VOID
USBPORT_StopRootHubPdo(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    )
/*++

Routine Description:

    Attempt to STOP the root hub

    Called by:

    USBPORT_FdoPnP() for HC FDO IRP_MN_REMOVE_DEVICE
    USBPORT_PdoPnP() for RH PDO IRP_MN_STOP_DEVICE
    USBPORT_PdoPnP() for RH PDO IRP_MN_SURPRISE_REMOVAL
    USBPORT_PdoPnP() for RH PDO IRP_MN_REMOVE_DEVICE

Arguments:

    FdoDeviceObject
    PdoDeviceObject

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION devExt = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'SRHP', USBPORT_GetPnpState(devExt), 0, 0);

    // disable the root hub notification interrupt
    // we won't need it while we are stopped
    MPRH_DisableIrq(devExt, ROOTHUB_LOCK_NOT_ACQUIRED);

    // This routine is called in response to a stop or remove on the root hub PDO
    // the neohub driver cancels any outstaning requests before forwarding -- as it
    // should.

    // new hub supports callback cancel so these values should not be set
    USBPORT_ASSERT(rhDevExt->Pdo.HubInitContext == NULL);
    USBPORT_ASSERT(rhDevExt->Pdo.HubInitCallback == NULL);

    // return ports to USB 11
    if (USBPORT_IS_USB20(devExt)) {

        if (!devExt->Fdo.Flags.HctForcePortsHighSpeed) {
            // skip this for diag mode -- this returns control to the CCs and we don't want that when running the HCTs.
            // NOTE: on XP disabling the root hub left control of the ports on the EHCI controller.
            MPf_ReleasePortControl(devExt);
        }
        rhDevExt->Pdo.RhCfgFlag = 0;

        // this will release any waiting CCs if this is due to failure on start
        USBPORT_FlushRhCallbacks(FdoDeviceObject);
    }

    // disable the root hub symbolic link
    USBPORT_DisableDeviceInterface(rhDevExt);

//    if (TEST_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_ENABLE)) {
//        // return value ignored!
//        IoSetDeviceInterfaceState(&rhDevExt->SymbolicLinkName, FALSE);
//        CLEAR_FLAG(rhDevExt->Flags, USBPORT_FLAG_IFACE_ENABLE);
//    }

    // resume the controller if it is 'suspended'
    USBPORT_ResumeUsbController(FdoDeviceObject);

}

/*
    Registry Key cache for miniports. Since the miniports cannot read the registry from the another thread
    other than the PNP thread we cache the reg values read from PNP start.

    Miniports may need to read the registry on a re-start.

*/

PUSBPORT_REG_CACHE_ENTRY
USBPORT_GetCahceEntry(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength
    )
/*++

Routine Description:

    Fetches a registry key value from the cache if possible.

Arguments:

    FdoDeviceObject - controller whose cache is read.

    SoftwareBranch - where entry resides in register.

    KeyNameString - NULL terminated UNICODE name of the key to look for.

    KeyNameStringLength - length in bytes of KeyNameString

Return Value:

    returns cached entry pointer or NULL if not found

--*/
{
    PLIST_ENTRY listEntry;
    PDEVICE_EXTENSION devExt;
    PUSBPORT_REG_CACHE_ENTRY regEntry;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // walk the list
    GET_HEAD_LIST(devExt->Fdo.RegistryCache, listEntry);

    while (listEntry &&
           listEntry != &devExt->Fdo.RegistryCache) {

        regEntry = (PUSBPORT_REG_CACHE_ENTRY) CONTAINING_RECORD(
                    listEntry,
                    struct _USBPORT_REG_CACHE_ENTRY,
                    RegLink);

        ASSERT_REG_CACHE(regEntry);

        if (KeyNameStringLength == regEntry->KeyNameStringLength &&
            SoftwareBranch == regEntry->SoftwareBranch &&
            RtlCompareMemory(regEntry->KeyNameString,
                             KeyNameString,
                             KeyNameStringLength)) {
            DbgTrace((UsbpDebugTrace, " reg entry found in cache\n"));
            return  regEntry;
        }

        listEntry = regEntry->RegLink.Flink;
    }

    DbgTrace((UsbpDebugTrace, " reg entry not in cache\n"));
    return NULL;
}


NTSTATUS
USBPORT_AddCachedRegistryKey(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __in_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    )
/*++

Routine Description:

    Adds a reg key value to the cache.

Arguments:

    FdoDeviceObject - controller whose cache is read.

    SoftwareBranch - where entry resides in register.

    KeyNameString - NULL terminated UNICODE name of the key to look for.

    KeyNameStringLength - length in bytes of KeyNameString.

    Data - for key to cache.

    DataLength - length of Data in bytes.

Return Value:

    returns STATUS_SUCCESS if the value was added to our cache otherwise error.


--*/
{
    PDEVICE_EXTENSION devExt;
    PUSBPORT_REG_CACHE_ENTRY regEntry;
    ULONG keyBufferSize = 0;
    NTSTATUS nts;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    nts = RtlULongAdd(sizeof(struct _USBPORT_REG_CACHE_ENTRY), KeyNameStringLength, &keyBufferSize);

    if (NT_SUCCESS(nts)) {
        ALLOC_POOL_Z(regEntry, NonPagedPoolNx, keyBufferSize);

        if (regEntry != NULL) {
            ALLOC_POOL_Z(regEntry->Data, NonPagedPoolNx, DataLength);
            if (regEntry->Data != NULL) {
                regEntry->Sig = SIG_REG_CACHE;
                regEntry->SoftwareBranch = SoftwareBranch;
                regEntry->KeyNameStringLength = KeyNameStringLength;
                regEntry->DataLength = DataLength;
                RtlCopyMemory(regEntry->Data, (PUCHAR)Data, DataLength);
                RtlCopyMemory(&regEntry->KeyNameString[0], KeyNameString, KeyNameStringLength);
                InsertTailList(&devExt->Fdo.RegistryCache, &regEntry->RegLink);

                DbgTrace((UsbpDebugTrace, " adding cache reg entry %p\n", regEntry));

                return STATUS_SUCCESS;
            } else {
                UsbFreePool(regEntry);
            }
        }
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    return nts;

}


NTSTATUS
USBPORT_GetCachedRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __in_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    )
/*++

Routine Description:

    Fetches a registry key value from the cache since we cannot read the registry on a thread other than the
    PNP,POWER thread

    We cache entries between PNP start and STOP.  If we are currently in the context of the pnp thread then the
    registry is read directly.

Arguments:

    FdoDeviceObject - controller FDO.

    PhysicalDeviceObject - Pdo associated with the key, passed to IoOpenDeviceRegistryKey

    SoftwareBranch - TRUE if key is read from software barnch of the PDO (see USBPORT_GetRegistryKeyValueForPdo)

    KeyNameString - unicode null terminated key name

    KeyNameStringLength - length of Keynamestring in bytes

    Data - pointer to a buffer that recieves the key data

    DataLength - lenth in bytes of Data buffer


Return Value:

    returns STATUS_SUCCESS if the value is found in the cache

--*/
{
    PDEVICE_EXTENSION devExt;
    PUSBPORT_REG_CACHE_ENTRY regEntry;
    NTSTATUS ntStatus;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, " USBPORT_GetCahcedRegistryKeyValueForPDO\n"));

    // read from the registry if we can
    if (devExt->Fdo.Flags.OnPnpThread) {

        ASSERT_PASSIVE();

        ntStatus = USBPORT_GetRegistryKeyValueForPdo(FdoDeviceObject,
                                                PhysicalDeviceObject,
                                                SoftwareBranch,
                                                KeyNameString,
                                                KeyNameStringLength,
                                                Data,
                                                DataLength);

        if (NT_SUCCESS(ntStatus)) {
            // cache it, if this fails we just return the result
            // of the read
            USBPORT_AddCachedRegistryKey(
                    FdoDeviceObject,
                    SoftwareBranch,
                    KeyNameString,
                    KeyNameStringLength,
                    Data,
                    DataLength);
        }

        return ntStatus;

    }

    // just read from the cache

    regEntry = USBPORT_GetCahceEntry(FdoDeviceObject,
                                     SoftwareBranch,
                                     KeyNameString,
                                     KeyNameStringLength);
    if (regEntry != NULL) {
        if (regEntry->DataLength <= DataLength) {
            RtlCopyMemory(Data, regEntry->Data, regEntry->DataLength);
            ntStatus = STATUS_SUCCESS;
            TEST_TRAP();
        } else {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
        }
    } else {
        ntStatus = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    return ntStatus;

}



VOID
USBPORT_FlushCachedRegistryKeys(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Flushes cache. Removes all cached registry keys.

Arguments:

Return Value:

    none.

--*/
{
    PLIST_ENTRY listEntry = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    PUSBPORT_REG_CACHE_ENTRY regEntry = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    while (!IsListEmpty(&devExt->Fdo.RegistryCache)) {
        listEntry = RemoveHeadList(&devExt->Fdo.RegistryCache);

        regEntry = (PUSBPORT_REG_CACHE_ENTRY) CONTAINING_RECORD(
                    listEntry,
                    struct _USBPORT_REG_CACHE_ENTRY,
                    RegLink);

        ASSERT_REG_CACHE(regEntry);

        UsbFreePool(regEntry->Data);
        UsbFreePool(regEntry);
    }

    return;
}


PKBUGCHECK_REASON_CALLBACK_RECORD
USBPORT_InitBugCheck(
    PKBUGCHECK_REASON_CALLBACK_ROUTINE BugCheckCallback
)
/*++

Routine Description:

    Initializes structures we need to log data to a crash dump. Unforumately
    this is global.

Arguments:

Return Value:

    none.

--*/
{
    PKBUGCHECK_REASON_CALLBACK_RECORD bugCheckRecord;

    ALLOC_POOL_Z(bugCheckRecord, NonPagedPoolNx,
                 sizeof(KBUGCHECK_REASON_CALLBACK_RECORD));

    if (bugCheckRecord) {

        KeInitializeCallbackRecord(bugCheckRecord);

        if (!KeRegisterBugCheckReasonCallback(
                bugCheckRecord,
                BugCheckCallback,
                KbCallbackSecondaryDumpData,
                "USBPORT\0")) {

             ExFreePool(bugCheckRecord);
             bugCheckRecord = NULL;
        }
    }

    return bugCheckRecord;
}


VOID
USBPORT_FreeBugCheck(
    PKBUGCHECK_REASON_CALLBACK_RECORD BugCheckRecord
)
/*++

Routine Description:

    Dregisters and frees bugcheck structs

Arguments:

Return Value:

    none.

--*/
{
    BOOLEAN result;

    if (BugCheckRecord) {
        result = KeDeregisterBugCheckReasonCallback(BugCheckRecord);
        USBPORT_ASSERT(result);
        if (result) {
            ExFreePool(BugCheckRecord);
        }
    }
    return;
}

PDEVICE_OBJECT USBPORT_CurrentFdo;

/*
typedef
VOID
(*PKBUGCHECK_REASON_CALLBACK_ROUTINE) (
    IN KBUGCHECK_CALLBACK_REASON Reason,
    IN struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    IN OUT PVOID ReasonSpecificData,
    IN ULONG ReasonSpecificDataLength
    );
*/


// {d753af02-29ef-43d4-b193-815632a87c3a}
DEFINE_GUID(GuidSaveLog,
0xd753af02, 0x29ef, 0x43d4, 0xb1, 0x2f, 0x40, 0xe, 0xbe, 0xe0, 0xcf, 0x9a);

// {02b5a84c-1e3d-497a-bb56-c753e08222ca}
DEFINE_GUID(GuidSaveUsbData,
0x02b5a84c, 0x1e3d, 0x497a, 0xbb, 0x56, 0xc7, 0x53, 0xe0, 0x82, 0x22, 0xca);

// {47c2fff9-6622-4210-9201-146f4af432cb}
DEFINE_GUID(GuidSaveUsbExtData,
0x47c2fff9, 0x6622, 0x4210, 0x92, 0x01, 0x14, 0x6f, 0x4a, 0xf3, 0x32, 0xcb);



VOID
USBPORT_BugCheckSaveExtData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    )
{
    PKBUGCHECK_SECONDARY_DUMP_DATA dumpData = ReasonSpecificData;
    PDEVICE_EXTENSION devExt;

    if (ReasonSpecificDataLength < sizeof(KBUGCHECK_SECONDARY_DUMP_DATA)) {
        return;
    }

    if (USBPORT_CurrentFdo == NULL) {
        return;
    }

    GET_DEVICE_EXT(devExt, USBPORT_CurrentFdo);
    ASSERT_FDOEXT(devExt);

    //
    // Always set OutBufferLength
    //

    dumpData->OutBufferLength = sizeof(DEVICE_EXTENSION) +
                                devExt->Fdo.MiniportDriver->RegistrationPacket.DeviceDataSize;

    //
    // If InBufferLength == 0 this is only a query for OutBufferLength
    //

    if (dumpData->InBufferLength) {

        dumpData->Guid = GuidSaveUsbExtData;
        dumpData->OutBuffer = devExt;
    }
}

VOID
USBPORT_BugCheckSaveLog(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    )
{
    PKBUGCHECK_SECONDARY_DUMP_DATA dumpData = ReasonSpecificData;
    PDEVICE_EXTENSION devExt;

    if (ReasonSpecificDataLength < sizeof(KBUGCHECK_SECONDARY_DUMP_DATA)) {
        return;
    }

    if (USBPORT_CurrentFdo == NULL) {
        return;
    }

    GET_DEVICE_EXT(devExt, USBPORT_CurrentFdo);
    ASSERT_FDOEXT(devExt);

    //
    // Always set OutBufferLength
    //

    dumpData->OutBufferLength = devExt->Log.LogSize;

    //
    // If InBufferLength == 0 this is only a query for OutBufferLength
    //

    if (dumpData->InBufferLength && devExt->Log.LogSize) {

        dumpData->Guid = GuidSaveLog;
        dumpData->OutBuffer = devExt->Log.LogStart;

    }
}


VOID
USBPORT_BugCheckSaveData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    )
{
    PKBUGCHECK_SECONDARY_DUMP_DATA dumpData = ReasonSpecificData;
    PUSBPORT_MINIDUMP_DATA mdd;
    PDEVICE_EXTENSION devExt;

    if (ReasonSpecificDataLength < sizeof(KBUGCHECK_SECONDARY_DUMP_DATA)) {
        return;
    }

    if (USBPORT_CurrentFdo == NULL) {
        return;
    }

    GET_DEVICE_EXT(devExt, USBPORT_CurrentFdo);
    ASSERT_FDOEXT(devExt);

    mdd = &devExt->Fdo.MiniDumpData;

    if (mdd->DataBuffer == NULL || mdd->DataLength == 0) {
        return;
    }

    DbgTrace((UsbpDebugTrace, ">Save Usb Data %p %d inLen %d\n",
        dumpData,
        dumpData->MaximumAllowed,
        dumpData->InBufferLength));

    //
    // Always set OutBufferLength
    //

    dumpData->OutBufferLength = mdd->DataLength;

    //
    // If InBufferLength == 0 this is only a query for OutBufferLength
    //

    if (dumpData->InBufferLength) {

        dumpData->Guid = GuidSaveUsbData;

        dumpData->OutBuffer = mdd->DataBuffer;
    }
}

VOID
USBPORT_SetMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject,
    __in ULONG DataLength,
    __in_bcount_opt(DataLength) PUCHAR DataBuffer
    )
/*
    Sets the bugcheck variables that will be recorded by the next crash dump.

    This routine can be used two ways:

    1. Before we throw a bugcheck to record some relevent info.

    2. Before we attempt something that could crash for reasons beyond
       our control, like IoCompleteRequest.  If 2 is used the caller
       must release the data buffers before freeing them in the event
       the operation did not cause a crash by calling
       USBPORT_SetMiniDumpData with a NULL data buffer and zero length.
*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT oldDevObj;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->IoRequestSpin, &irql);

    // in the case of bugcheck 0xfe the ref/deref doesn't matter.  this code
    // is provided in the event we use this routine for Case 2.

    ObReferenceObject(FdoDeviceObject);

    // Set the extra data pointer as used by USBPORT_BugCheckSaveData().
    //
    devExt->Fdo.MiniDumpData.DataLength = DataLength;

    if (DataLength) {
        devExt->Fdo.MiniDumpData.DataBuffer = DataBuffer;
    } else {
        devExt->Fdo.MiniDumpData.DataBuffer = NULL;
    }

    // Set the current FDO pointer as used by USBPORT_BugCheckSaveExtData()
    // and by USBPORT_BugCheckSaveLog().
    //
    oldDevObj = InterlockedExchangePointer(&USBPORT_CurrentFdo, FdoDeviceObject);

    if (oldDevObj) {
        ObDereferenceObject(oldDevObj);
    }

    KeReleaseSpinLock(&devExt->IoRequestSpin, irql);
}


VOID
USBPORT_ClearMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject
    )
/*
    Clears the bugcheck variables that will be recorded by the next crash dump.
*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT oldDevObj;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->IoRequestSpin, &irql);

    // in the case of bugcheck 0xfe the ref/deref doesn't matter.  this code
    // is provided in the event we use this routine for Case 2.

    // Clear the extra data pointer as used by USBPORT_BugCheckSaveData().
    //
    devExt->Fdo.MiniDumpData.DataLength = 0;
    devExt->Fdo.MiniDumpData.DataBuffer = NULL;

    // Clear the current FDO pointer as used by USBPORT_BugCheckSaveExtData()
    // and by USBPORT_BugCheckSaveLog().
    //
    oldDevObj = InterlockedExchangePointer(&USBPORT_CurrentFdo, NULL);

    if (oldDevObj) {
        ObDereferenceObject(oldDevObj);
    }

    KeReleaseSpinLock(&devExt->IoRequestSpin, irql);
}


USBPORT_PNP_STATE
USBPORT_SetPnpState(
    PDEVICE_EXTENSION Ext,
    USBPORT_PNP_STATE NewPnpState,
    USBPORT_PNP_EVENT PnpEvent
    )
/*++

Routine Description:

    Sets the pnp state for root hub of Hc device extension and logs the event.

Arguments:

    Ext

    NewPnpState - new state to enter

    Event - event that caused teh transition

Return Value:

    Previous pnp state
--*/

{
    USBPORT_PNP_STATE oldState;
    PHC_STATE_LOG_ENTRY pnpLog;

    oldState = Ext->PnpState;

    Ext->PnpLogIdx=(Ext->PnpLogIdx+1) % NUM_STATE_LOG_ENTRIES;
    pnpLog = &Ext->PnpLog[Ext->PnpLogIdx];

    pnpLog->Event = PnpEvent;
    pnpLog->CurrentState = oldState;
    pnpLog->NextState = NewPnpState;

    Ext->PnpState = NewPnpState;

    return oldState;
}


USBPORT_PNP_STATE
USBPORT_SetHcPnpState(
    PDEVICE_OBJECT HcFdo,
    USBPORT_PNP_STATE NewPnpState,
    USBPORT_PNP_EVENT PnpEvent
    )
/*++

Routine Description:

    Sets the pnp state for the host controller FDO and logs the event

Arguments:

    NewPnpState - new state to enter

    Event - event that caused teh transition

Return Value:

    Previous pnp state
--*/

{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    return USBPORT_SetPnpState(devExt, NewPnpState, PnpEvent);

}


USBPORT_PNP_STATE
USBPORT_SetRhPnpState(
    PDEVICE_OBJECT RhPdo,
    USBPORT_PNP_STATE NewPnpState,
    USBPORT_PNP_EVENT PnpEvent
    )
/*++

Routine Description:

    Sets the pnp state for the host controller root hub PDO and logs the event

Arguments:

    NewPnpState - new state to enter

    Event - event that caused teh transition

Return Value:

    Previous pnp state
--*/

{
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, RhPdo);
    ASSERT_PDOEXT(rhDevExt);

    return USBPORT_SetPnpState(rhDevExt, NewPnpState, PnpEvent);

}



NTSTATUS
USBPORT_RegisterAndEnableDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo,
    CONST GUID *Guid
    )
/*++

Routine Description:

    Wrapper for IoRegisterDeviceInterface

    This routine will register and enable the device interface. If already registered it just re-enables

Arguments:

    Ext - device extension of Fdo or PDO

    Pdo - to associate with register

    Guid - guid to pass to IoRegisterDeviceInterface

Return Value:

    nt status or request
--*/

{
    NTSTATUS nts;

    nts = USBPORT_RegisterDeviceInterface(Ext, Pdo, Guid);
    if (NT_SUCCESS(nts)) {
        nts = USBPORT_EnableDeviceInterface(Ext, Pdo);
    }

    return nts;
}


NTSTATUS
USBPORT_RegisterDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo,
    CONST GUID *Guid
    )
/*++

Routine Description:

    Wrapper for IoRegisterDeviceInterface

    This routine will register the device interface.

Arguments:

    Ext - device extension of Fdo or PDO

    Pdo - to associate with register

    Guid - guid to pass to IoRegisterDeviceInterface

Return Value:

    nt status or request
--*/

{
    NTSTATUS nts = STATUS_UNSUCCESSFUL;

    switch(Ext->InterfaceState) {
    case IfEnabled:
        nts = STATUS_SUCCESS;
        break;

    case IfRegistered:
        nts = STATUS_SUCCESS;
        break;
    case IfNotRegistered:
        nts = IoRegisterDeviceInterface(Pdo,
                                        Guid,
                                        NULL,
                                        &Ext->SymbolicLinkName);
        if (NT_SUCCESS(nts)) {
            Ext->InterfaceState = IfRegistered;
        }
        break;
    }

    return nts;
}


VOID
USBPORT_DeregisterDeviceInterface(
    PDEVICE_EXTENSION Ext
    )
/*++

Routine Description:

    Deregisters a device interface

Arguments:

Return Value:

    nt status or request
--*/

{
    switch(Ext->InterfaceState) {
    case IfEnabled:
        break;

    case IfRegistered:
        RtlFreeUnicodeString(&Ext->SymbolicLinkName);
        Ext->InterfaceState = IfNotRegistered;
        break;

    case IfNotRegistered:
        break;
    }

}


NTSTATUS
USBPORT_EnableDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Enables a device interface

Arguments:

Return Value:

    nt status or request
--*/

{
    NTSTATUS nts = STATUS_UNSUCCESSFUL;

    switch(Ext->InterfaceState) {
    case IfEnabled:
        nts = STATUS_SUCCESS;
        break;

    case IfNotRegistered:
        nts = STATUS_UNSUCCESSFUL;
        break;

    case IfRegistered:

        nts = IoSetDeviceInterfaceState(&Ext->SymbolicLinkName,  TRUE);
        if (NT_SUCCESS(nts)) {

           USBPORT_SetRegistryKeyValueForPdo(
                            Pdo,
                            USBPORT_HW_BRANCH,
                            REG_SZ,
                            SYM_LINK_KEY,
                            sizeof(SYM_LINK_KEY),
                            &Ext->SymbolicLinkName.Buffer[0],
                            Ext->SymbolicLinkName.Length);

            Ext->InterfaceState = IfEnabled;
        }
        break;
    }

    return nts;
}


VOID
USBPORT_DisableDeviceInterface(
    PDEVICE_EXTENSION Ext
    )
/*++

Routine Description:

    Disables a device interface

Arguments:

Return Value:

    nt status or request
--*/

{
    switch(Ext->InterfaceState) {
    case IfRegistered:
    case IfNotRegistered:
        break;

    case IfEnabled:
        IoSetDeviceInterfaceState(&Ext->SymbolicLinkName,  FALSE);
        Ext->InterfaceState = IfRegistered;
        break;
    }

}

VOID
USBPORT_HubFailSetMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject
    )
/*
    Sets the bugcheck variables that will be recorded by the next crash dump.

    This routine can be used two ways:

    1. Before we throw a bugcheck to record some relevent info.

    2. Before we attempt something that could crash for reasons beyond
       our control, like IoCompleteRequest.  If 2 is used the caller
       must release the data buffers before freeing them in the event
       the operation did not cause a crash by calling
       USBPORT_SetMiniDumpData with a NULL data buffer and zero length.

*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT oldDevObj;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Clear the extra data pointer as used by USBPORT_BugCheckSaveData().
    //
    devExt->Fdo.MiniDumpData.DataLength = 0;
    devExt->Fdo.MiniDumpData.DataBuffer = NULL;

    // Set the current FDO pointer as used by USBPORT_BugCheckSaveExtData()
    // and by USBPORT_BugCheckSaveLog().
    //
    ObReferenceObject(FdoDeviceObject);

    oldDevObj = InterlockedExchangePointer(&USBPORT_CurrentFdo, FdoDeviceObject);

    if (oldDevObj) {
        ObDereferenceObject(oldDevObj);
    }
}

VOID
USBPORT_SimulateInterruptDpcHighRes (
    _In_ PEX_TIMER Timer,
    _In_ PVOID Context
    )

/*++

Routine Description:

    This routine is an EXT_CALLBACK wrapper for USBPORT_SimulateInterruptDpc.

Arguments:

    Timer - Supplies the timer that expired and triggered this callback.

    Context - Supplies USBPORT_SimulateInterruptDpc's context.

Return value:

    None.

*/

{

    USBPORT_SimulateInterruptDpc(NULL, Context, NULL, NULL);
    return;
}
