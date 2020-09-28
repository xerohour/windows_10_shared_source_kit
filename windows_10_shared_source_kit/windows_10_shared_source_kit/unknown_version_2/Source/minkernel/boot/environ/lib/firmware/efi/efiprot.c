/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiprot.c

Abstract:

    Wrappers for EFI Protocol Service APIs.

    All EFI service call must be made in physical mode.  This module helps
    deal with some of the complications encountered when a boot application
    is executing with address translation enabled.  It is expected that all
    EFI Boot Applications use these routines when attempting to utilize
    EFI Protocol Services.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

/*++

Description of Problem :

    The ability to execute in virtual mode greatly complicates EFI protocol
    interface handling.  In EFI, each device has a handle and with each
    handle there is a list of interfaces for each protocol the device
    supports.  Since EFI service calls are made in physical mode, all
    interface pointers are physical addresses.

    When executing with processor address translation enabled, it becomes
    necessary to virtually map all pointers returned from an EFI
    service call.  To be proper, it is also necessary to unmap pointers when
    done with an interface.

    It becomes incredibly difficult to maintain when it is appropriate to
    unmap a protocol interface returned from an EFI protocol service.
    It is possible to maintain a reference count on an interface and therefore
    determine when it is no longer in use by a caller.  Additionally, it is
    possible to determine if the address was mapped prior to the initial
    mapping and therefore know if the interface should ever be unmapped.
    The largest problem lies in the fact that protocol interfaces are much
    smaller than a page in size.  Multiple interfaces could reside in the
    same physical page.  One way to handle this would include a reference
    count of the physical pages mapped.  But this quickly becomes a daunting
    and unrealistic task.

Solution :

    Every platform reserves 4MB of virtual address space for the HAL.  If
    an interface was not mapped during MM initialization, this module will
    use the HALs address space to a protocol interface (one interface per
    page to avoid the requirement of a physical page reference count).
    This is wasteful, but it is expected that the majority of interfaces
    are mapped during MM initialization and the virtual address space wasted
    is address space currently not usable by a boot application.

--*/

// ------------------------------------------------- Data Structure Definitions

typedef struct  _PROTOCOL_INTERFACE_ENTRY {
    LIST_ENTRY   ListEntry;
    const EFI_GUID *Protocol;
    PVOID        Interface;
    ULONG        ReferenceCount;
    ULONG        Flags;
} PROTOCOL_INTERFACE_ENTRY, *PPROTOCOL_INTERFACE_ENTRY;

#define PI_FLAGS_NONE                  (0x00000000)
#define PI_FLAGS_UNMAP_ON_CLOSE        (0x00000001)

/*++

Protocol Interface Entry Description:

    A Protocol Interface Entry structure maintains a count of the current
    callers with a valid pointer to a protocol interface.  For each EFI handle,
    a list of protocol interface entries is maintained.  Every caller
    requesting the interface for a particular protocol on the same device
    will receive the same interface pointer.  Once every caller 'closes' the
    protocol, the interface will be unmapped (if necessary).

Fields:

    ListEntry - Flink and Blink pointers used for protocol chaining.

    Protocol - Specifies the Protocol type of the interface.

    Interface - Virtual Address pointer to the protocol interface.

    ReferenceCount - Current count of the number of caller's with a pointer
        to Interface.

    Flags - Special flags for handling Interface.  Possible values include:

        PI_FLAGS_UNMAP_ON_CLOSE : When set, Interface will be unmapped
            when the last reference to a protocol interface is release.

--*/

// -------------------------------------------------------------------- Globals

//
// A hash table is used to maintain a proper count of the number of
// of callers an interface pointer was given.  This is used to properly
// unmap an interface, when necessary.
//

HASH_TABLE_ID EfiProtHashTableId;
BOOLEAN EfiProtHashTableInitialized = FALSE;

// ----------------------------------------------------- Virtual Mode Prototyes

NTSTATUS
EfiVmOpenProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __deref_out VOID **Interface
    );

NTSTATUS
EfiVmCloseProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __in VOID *Interface
    );

NTSTATUS
EfiVmpFreeInterfaceEntry (
    __in EFI_HANDLE Handle,
    __in PPROTOCOL_INTERFACE_ENTRY InterfaceEntry
    );

NTSTATUS
EfiVmpCloseProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol
    );

// ---------------------------------------------------------- Protocol Services

NTSTATUS
EfiLocateDevicePath (
    __in const EFI_GUID *Protocol,
    __deref_inout EFI_DEVICE_PATH_PROTOCOL **DevicePath,
    __out EFI_HANDLE *Device
    )

/*++

Routine Description:

    This function locates the handle to a device on the device path that
    supports the specified protocol.

Arguments:

    Protocol - Supplies the protocol to search for.

    DevicePath - Supplies the pointer to the device path on input. On output,
        the device path pointer is modified to point to the remaining part of
        the device path.

    Device - Supplies the pointer to the returned device handle.

Return Value:

    STATUS_SUCCESS if operation completed successfully.
    STATUS_INVALID_PARAMETER if parameters are invalid.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DevicePa = {0};
    PHYSICAL_ADDRESS DevicePathPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS ProtocolPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses into their repective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Protocol, &ProtocolPa);
        Protocol = (EFI_GUID *)(ULONG_PTR)ProtocolPa.QuadPart;
        BlMmTranslateVirtualAddress(DevicePath, &DevicePathPa);
        DevicePath =
            (EFI_DEVICE_PATH_PROTOCOL **)(ULONG_PTR)DevicePathPa.QuadPart;

        BlMmTranslateVirtualAddress(Device, &DevicePa);
        Device = (EFI_HANDLE *)(ULONG_PTR)DevicePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->LocateDevicePath((EFI_GUID*)Protocol, DevicePath, Device);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiLocateHandleBuffer (
    __in EFI_LOCATE_SEARCH_TYPE SearchType,
    __in_opt const EFI_GUID *Protocol,
    __in_opt VOID *SearchKey,
    __inout UINTN *NumHandles,
    __deref_out_ecount_opt(*NumHandles) EFI_HANDLE **Buffer
    )

/*++

Routine Description:

    Queries EFI and returns an array of handles that support the requested
    protocol.  If Buffer does not point to an allocated buffer,
    EfiLocateHandleBuffer will allocate a buffer from the HeapAllocator that
    is big enough to service the request.

Arguments:

    SearchType - Specifies which handles to return (options include all
                 handles, registration key or by protocol).

    Protocol - (For use with ByProtocol SearchType) : Specifies the type of
               handles to be returned.

    SearchKey - (For use with ByRegisterNotify SearchType) : Specifies the
                registration key.

    NumHandles - Pointer to address to store the number of handles returned in
                 Buffer.  If the buffer is allocated, on input NumHandles
                 supplies the size (in Handles) of the buffer.

    Buffer - Pointer to buffer to return the requested array of handles.  If the
             buffer is null, a suitable buffer will be allocated.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Buffer is NULL or NumHandles is NULL.
    STATUS_NOT_FOUND if there are no handles that match the search criteria.
    STATUS_NO_MEMORY if there is not enough memory to allocate Buffer.

--*/

{

    BOOLEAN BoolRet;
    UINTN BufferSize;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    EFI_HANDLE *HandleArray;
    PHYSICAL_ADDRESS HandleArrayPa;

    //
    // Parameter checking.
    //

    if (Buffer == NULL || NumHandles == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    HandleArray = *Buffer;
    BufferSize = (*Buffer == NULL) ? 0 : *NumHandles * sizeof(EFI_HANDLE);

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BoolRet = BlMmTranslateVirtualAddress(HandleArray, &HandleArrayPa);
        if (BoolRet != FALSE) {
            HandleArray = (PVOID)(UINTN)HandleArrayPa.QuadPart;

        } else {
            HandleArray = NULL;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Attempt to get the array of handles with the buffer the user passed.
    //

    EfiStatus = EfiBS->LocateHandle(SearchType,
                                    (EFI_GUID*)Protocol, // const_cast
                                    SearchKey,
                                    &BufferSize,
                                    HandleArray);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // If the buffer is too small, allocate one from the heap allocator.
    // Note: We don't have to change the address when going from virtual
    // to physical mode and vice versa since all virtual addresses allocated
    // from either the page allocator or heap allocator are mapped using
    // translation registers.
    //

    if (EfiStatus == EFI_BUFFER_TOO_SMALL) {

        //
        // Free the memory used by the existing buffer.
        //

        if (*Buffer != NULL) {
            BlMmFreeHeap(*Buffer);
        }

        //
        // Allocate a new buffer for the handle array.
        //

        *Buffer = HandleArray = BlMmAllocateHeap(BufferSize);
        if (*Buffer == NULL) {
            return STATUS_NO_MEMORY;
        }

        //
        // Call LocateHandle again with the new buffer.
        //

        //
        // If the application is running within the application context,
        // switch it to firmware execution context for making firmware calls.
        // Translate all virtual addresses to their respective physical
        // addresses.
        //

        if (ContextType != ExecutionContextFirmware) {
            BoolRet = BlMmTranslateVirtualAddress(HandleArray, &HandleArrayPa);
            if (BoolRet != FALSE) {
                HandleArray = (PVOID)(UINTN)HandleArrayPa.QuadPart;

            } else {
                HandleArray = NULL;
            }

            BlpArchSwitchContext(ExecutionContextFirmware);
        }


        EfiStatus = EfiBS->LocateHandle(SearchType,
                                        (EFI_GUID*)Protocol, // const_cast
                                        SearchKey,
                                        &BufferSize,
                                        HandleArray);

        //
        // Restore original execution context.
        //

        if (ContextType != ExecutionContextFirmware) {
            BlpArchSwitchContext(ContextType);
        }
    }

    //
    // Return the results.
    //

    *NumHandles = BufferSize / sizeof(EFI_HANDLE);
    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiOpenProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __deref_out VOID **Interface
    )

/*++

Routine Description:

    Wrapper for either the EFI service HandleProtocol or OpenProtocol.  Both
    services will query a handle to determine if the requested protocol is
    supported.  If it is, the protocol interface is returned to the caller.

    If the boot application supports virtual address translation, the
    interface needs to be mapped before returning to the caller.  The
    handle will be hashed and a pointer to the virtually mapped interface will
    be cached.

    The difference between HandleProtocol and OpenProtocol is that OpenProtocol
    (new with EFI revisision 1.10) will maintain a handle database to
    keep track of opened protocols.  This should prevent a protocol from being
    uninstalled from underneath the boot application's execution and prevent
    an interface from being invalidated.

Arguments:

    Handle - The handle for the protocol interface that is being opened.

    Protocol - The unique identifier for the protocol.

    Interface - Address to store a pointer to the requested protocol
        interface.

Return Value:

    STATUS_SUCCESS: when successful.
    STATUS_INVALID_PARAMETER if Protocol or Handle are invalid or if Interface
        is NULL.
    STATUS_NOT_SUPPORTED if Handle does not support Protocol.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    NTSTATUS Status;

    //
    // If the boot application is not executing with address translation,
    // this routine becomes a direct wrapper for either the OpenProtocol
    // or HandleProtocol service.
    //
    // When address translation is enabled, forward the request to
    // EfiVmOpenProtocol which will return a mapped pointer of the requested
    // interface.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {

        //
        // If the application is running within the application execution
        // context, switch it to firmware execution context for making the
        // firmware call.
        //

        ContextType = BlpArchQueryCurrentContextType();
        if (ContextType != ExecutionContextFirmware) {
            BlpArchSwitchContext(ExecutionContextFirmware);
        }

        //
        // Fork code depending on the version of EFI.  The preferred
        // interface, OpenProtocol, was specified as part of EFI 1.10.
        //

        if (EfiST->Hdr.Revision == EFI_1_02_SYSTEM_TABLE_REVISION) {
            EfiStatus = EfiBS->HandleProtocol(Handle, (EFI_GUID*)Protocol, Interface);

        } else {
            EfiStatus = EfiBS->OpenProtocol(Handle,
                                            (EFI_GUID*)Protocol, // const_cast
                                            Interface,
                                            EfiImageHandle,
                                            NULL,
                                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        }

        //
        // Restore the original execution context.
        //

        if (ContextType != ExecutionContextFirmware) {
            BlpArchSwitchContext(ContextType);
        }

        Status = EfiGetNtStatusCode(EfiStatus);

    } else {
        Status = EfiVmOpenProtocol(Handle, Protocol, Interface);
    }

    //
    // Ensure similar output for all error cases across all firmware
    // implementations by manually setting Interface to NULL for any error.
    //

    if (!NT_SUCCESS(Status)) {
        *Interface = NULL;
    }

    return Status;
}

NTSTATUS
EfiCloseProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __in VOID *Interface
    )

/*++

Routine Description:

    Wrapper for the EFI service CloseProtocol.  This service is used to
    indicate to the firmware that a protocol interface is no longer in
    use by an application.

    If the boot application supports virtual address translation, the
    interface was virtually mapped during the Open operation.  This interface
    needs to unmap the interface when appropriate.

Arguments:

    Handle - The handle for the protocol interface that is being close.

    Protocol - The handle protocol that is being closed.

    Interface - Pointer that was returned from EfiOpenProtocol.

Return Value:

    STATUS_SUCCESS: when successful.
    STATUS_INVALID_PARAMETER when Handle or Protocol are invalid.
    STATUS_NOT_FOUND if Handle does not support the protocol specified by
        Protocol.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    NTSTATUS Status;

    //
    // If the boot application is not executing with address translation,
    // this routine becomes a direct wrapper for the CloseProtocol service.
    //
    // When address translation is enabled, forward the request to
    // EfiVmCloseProtocol which will unmap the interface pointer when
    // appropriate.
    //

    if (BlMmQueryTranslationType() == VA_TRANSLATION_TYPE_NONE) {

        //
        // CloseProtocol was not specified until EFI 1.10.
        //

        if (EfiST->Hdr.Revision == EFI_1_02_SYSTEM_TABLE_REVISION) {
            EfiStatus = EFI_SUCCESS;

        } else {

            //
            // If the application is running within the application execution
            // context, switch it to firmware execution context for making the
            // firmware call.
            //

            ContextType = BlpArchQueryCurrentContextType();
            if (ContextType != ExecutionContextFirmware) {
                BlpArchSwitchContext(ExecutionContextFirmware);
            }

            //
            // Unfortunetly, the bulk of current EFI implementations do
            // not reference count protocol interfaces returned for an
            // individual handle.  As a result, only the first CloseProtocol
            // call will ever return success, with subsequent calls failing
            // EFI_NOT_FOUND.
            //
            // N.B. The current assumption is that other outstanding protocol
            //      interface pointers should remain valid since they are all
            //      pointers to the same internal interface structure.
            //      Resultantly, an EFI_NOT_FOUND error is ignored.  If it is
            //      ever determined that outstanding protocol interface
            //      pointers are invalidated by the first CloseProtocol call,
            //      this routine needs to be modified to never actually call
            //      CloseProtocol.
            //

            EfiStatus = EfiBS->CloseProtocol(Handle,
                                             (EFI_GUID*)Protocol, // const_cast
                                             EfiImageHandle,
                                             NULL);

            //
            // Restore the original execution context.
            //

            if (ContextType != ExecutionContextFirmware) {
                BlpArchSwitchContext(ContextType);
            }

            if (EfiStatus == EFI_NOT_FOUND) {
                EfiStatus = EFI_SUCCESS;
            }
        }

        Status = EfiGetNtStatusCode(EfiStatus);

    } else {
        Status = EfiVmCloseProtocol(Handle, Protocol, Interface);
    }

    return Status;
}

// ----------------------------------------------------- Virtual Mode Functions

NTSTATUS
EfiVmOpenProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __deref_out VOID **Interface
    )

/*++

Routine Description:

    This routine implementes EfiOpenProtocol for a boot application executing
    with address translation enabled.  In this case, the interface returned
    to the caller must be a virtual address.

    Please see the description of this problem at the top of this file.

Arguments:

    Handle - The handle for the protocol interface that is being opened.

    Protocol - The unique identifier for the protocol.

    Interface - Address to store a pointer to the requested protocol
        interface.

Return Value:

    STATUS_SUCCESS: when successful.
    STATUS_INVALID_PARAMETER if Protocol or Handle are invalid or if Interface
        is NULL.
    STATUS_NOT_SUPPORTED if Handle does not support Protocol.

--*/

{

    PPROTOCOL_INTERFACE_ENTRY AllocatedInterfaceEntry;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHASH_DATA HashData;
    HASH_KEY HashKey;
    PPROTOCOL_INTERFACE_ENTRY InterfaceEntry;
    PLIST_ENTRY InterfaceEntryList;
    PHYSICAL_ADDRESS InterfacePa;
    BOOLEAN IsMapped;
    BOOLEAN MappedInterface;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    VirtualAddress = NULL;
    MappedInterface = FALSE;
    InterfaceEntryList = NULL;
    InterfacePa.QuadPart = 0;   // Ensure high bytes are zero when returning
                                // from EFI call on 32 bit applications.

    if (Handle == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (EfiProtHashTableInitialized == FALSE) {

        //
        // Initialize the hash table for protocol interface maintaince.
        //

        Status = BlHtCreate(0, NULL, NULL, &EfiProtHashTableId);
        if (!NT_SUCCESS(Status)) {
            goto EfiVmOpenProtocolEnd;
        }

        EfiProtHashTableInitialized = TRUE;
    }

    //
    // See if there is currently a virtual pointer to the requested
    // protocol interface.
    //

    HashKey.Value = Handle;
    HashKey.Size = sizeof(EFI_HANDLE);
    HashKey.Flags = HASH_KEY_FLAGS_VALUE;
    Status = BlHtLookup(EfiProtHashTableId, &HashKey, &HashData);
    if (NT_SUCCESS(Status)) {

        //
        // Walk the list of protocols currently opened for this handle
        // to see if the requested protocol interface is already mapped.
        //

        ASSERT(HashData->Size == sizeof(LIST_ENTRY));

        InterfaceEntryList = (PLIST_ENTRY)HashData->Value;
        NextEntry = InterfaceEntryList->Flink;
        while (NextEntry != InterfaceEntryList) {
            InterfaceEntry = CONTAINING_RECORD(NextEntry,
                                               PROTOCOL_INTERFACE_ENTRY,
                                               ListEntry);

            if (InterfaceEntry->Protocol == Protocol) {

                //
                // The requested protocol has already been opened for this
                // handle.  Return the same virtual pointer to the caller.
                //

                ASSERT(InterfaceEntry->ReferenceCount != 0);

                InterfaceEntry->ReferenceCount += 1;
                *Interface = InterfaceEntry->Interface;
                Status = STATUS_SUCCESS;
                goto EfiVmOpenProtocolEnd;
            }

            NextEntry = NextEntry->Flink;
        }
    }

    //
    // The requested protocol is not currently opened for this handle.
    // Perform the necessary EFI Service call to obtain the interface
    // for this protocol.
    //

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Fork code depending on the version of EFI.  The preferred
    // interface, OpenProtocol, was specified as part of EFI 1.10.
    //

    if (EfiST->Hdr.Revision == EFI_1_02_SYSTEM_TABLE_REVISION) {
        EfiStatus = EfiBS->HandleProtocol(Handle,
                                          (EFI_GUID*)Protocol, // const_cast
                                          (PVOID *)&InterfacePa);

    } else {
        EfiStatus = EfiBS->OpenProtocol(Handle,
                                        (EFI_GUID*)Protocol,
                                        (PVOID *)&InterfacePa,
                                        EfiImageHandle,
                                        NULL,
                                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    }

    //
    // Restore the original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    Status = EfiGetNtStatusCode(EfiStatus);
    if (!NT_SUCCESS(Status)) {
        goto EfiVmOpenProtocolEnd;
    }

    //
    // Check to see if the firmware pointer was mapped one to one during
    // MM initialization.
    //

    VirtualAddress = (PVOID)(UINTN)InterfacePa.QuadPart;
    IsMapped = BlMmTranslateVirtualAddress(VirtualAddress, &PhysicalAddress);

    //
    // The case in which the mapping exists but is not 1-to-1 is not
    // supported at this time.
    //

    if ((IsMapped != FALSE) &&
        (InterfacePa.QuadPart != PhysicalAddress.QuadPart)) {

        Status = STATUS_NOT_SUPPORTED;
        goto EfiVmOpenProtocolEnd;
    }

    //
    // When unmapped, this routine must map the protocol interface before
    // returning.
    //

    if (IsMapped == FALSE) {

        //
        // In the case where the boot service interface is not mapped, simply
        // establish a mapping using the boot library's memory management
        // interface.  This happens when a device gets connected in the boot
        // manager (causing the firmware to allocate additional boot services)
        // due to a bug wherein the boot library's view of memory is not kept
        // in sync with the firmware's, and which results in the new boot
        // service allocation not being mapped in the loader context.  The
        // problem of keeping the boot library and firmware memory maps in
        // sync will be addressed by UEFI v2.
        //

        Status = BlMmMapPhysicalAddress(
                     &VirtualAddress,
                     InterfacePa,
                     PAGE_SIZE,
                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            goto EfiVmOpenProtocolEnd;
        }

        MappedInterface = TRUE;
    }

    //
    // VirtualAddress is currently a virtual pointer to the requested
    // EFI interface.
    //

    *Interface = VirtualAddress;

    //
    // If this is the first protocol opened on the handle, insert an empty
    // linked list structure into the hash table.
    //

    if (InterfaceEntryList == NULL) {
        InterfaceEntryList = BlMmAllocateHeap(sizeof(LIST_ENTRY));
        if (InterfaceEntryList == NULL) {
            Status = STATUS_NO_MEMORY;
            goto EfiVmOpenProtocolEnd;
        }

        InitializeListHead(InterfaceEntryList);
        Status = BlHtStore(EfiProtHashTableId,
                           &HashKey,
                           InterfaceEntryList,
                           sizeof(LIST_ENTRY));

        if (!NT_SUCCESS(Status)) {
            BlMmFreeHeap(InterfaceEntryList);
            goto EfiVmOpenProtocolEnd;
        }
    }

    //
    // Maintain the list of opened protocols, inserting an entry for the
    // current protocol interface.
    //

    AllocatedInterfaceEntry =
        BlMmAllocateHeap(sizeof(PROTOCOL_INTERFACE_ENTRY));

    if (AllocatedInterfaceEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto EfiVmOpenProtocolEnd;
    }

    AllocatedInterfaceEntry->Protocol = Protocol;
    AllocatedInterfaceEntry->Interface = *Interface;
    AllocatedInterfaceEntry->ReferenceCount = 1;
    if (MappedInterface != FALSE) {
        AllocatedInterfaceEntry->Flags = PI_FLAGS_UNMAP_ON_CLOSE;

    } else {
        AllocatedInterfaceEntry->Flags = PI_FLAGS_NONE;
    }

    InsertHeadList(InterfaceEntryList, &AllocatedInterfaceEntry->ListEntry);

EfiVmOpenProtocolEnd:
    if (!NT_SUCCESS(Status) && (MappedInterface != FALSE)) {
        BlMmUnmapVirtualAddress(VirtualAddress, PAGE_SIZE);
        *Interface = NULL;
    }

    return Status;
}

NTSTATUS
EfiVmCloseProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol,
    __in VOID *Interface
    )

/*++

Routine Description:

    This routine implementes EfiCloseProtocol for a boot application executing
    with address translation enabled.  In this case, the interface returned
    from EfiOpenProtocol was virtually mapped.  This routine will unmap the
    interface when appropriate.

    Please see the description of this problem at the top of this file.

Arguments:

    Handle - The handle for the protocol interface that is being close.

    Protocol - The handle protocol that is being closed.

    Interface - Pointer that was returned from EfiOpenProtocol.  This routine
        will unmap the interface.

Return Value:

    STATUS_SUCCESS: when successful.
    STATUS_INVALID_PARAMETER when Handle or Protocol are invalid.
    STATUS_NOT_FOUND if Handle does not support the protocol specified by
        Protocol.

--*/

{

    PHASH_DATA HashData;
    HASH_KEY HashKey;
    PPROTOCOL_INTERFACE_ENTRY InterfaceEntry;
    PLIST_ENTRY InterfaceEntryList;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Interface);

    //
    // All interfaces returned from EfiVmOpenProtocol are stored in
    // a global hash table, sorted by EFI handles.  Each handle contains a
    // linked list of currently opened protocols with their interface
    // pointers.  Attempt to find the entry containing the interface for the
    // requested protocol on the specified device.
    //

    HashKey.Value = Handle;
    HashKey.Size  = sizeof(EFI_HANDLE);
    HashKey.Flags = HASH_KEY_FLAGS_VALUE;
    Status = BlHtLookup(EfiProtHashTableId, &HashKey, &HashData);
    if (!NT_SUCCESS(Status)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Walk the list of protocols currently opened for this handle, searching
    // for the requested protocol interface.
    //

    ASSERT(HashData->Size == sizeof(LIST_ENTRY));

    InterfaceEntryList = (PLIST_ENTRY)HashData->Value;
    NextEntry = InterfaceEntryList->Flink;
    while (NextEntry != InterfaceEntryList) {
        InterfaceEntry = CONTAINING_RECORD(NextEntry,
                                           PROTOCOL_INTERFACE_ENTRY,
                                           ListEntry);

        if (InterfaceEntry->Protocol == Protocol) {

            ASSERT(Interface == InterfaceEntry->Interface);

            InterfaceEntry->ReferenceCount -= 1;

            //
            // If there are no other valid pointers to this interface,
            // unmap it (if necessary), remove the entry from the hash table
            // and close the reference using the firmware.
            //

            if (InterfaceEntry->ReferenceCount == 0) {
                ReturnStatus = EfiVmpFreeInterfaceEntry(Handle, InterfaceEntry);
                Status = EfiVmpCloseProtocol(Handle, Protocol);
                if (!NT_SUCCESS(Status)) {
                    ReturnStatus = Status;
                }

            } else {
                ReturnStatus = STATUS_SUCCESS;
            }

            return ReturnStatus;
        }

        NextEntry = NextEntry->Flink;
    }

    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
EfiVmpFreeInterfaceEntry (
    __in EFI_HANDLE Handle,
    __in PPROTOCOL_INTERFACE_ENTRY InterfaceEntry
    )

/*++

Routine Description:

    This routine will remove an entry from a handle's protocol interface
    list.  When there are no entries left, the handle will be removed from
    the hash table.

    If the interface was mapped during the open operation, this routine will
    additionally unmap the interface.

Arguments:

    Handle - The handle for the protocol interface that is being close.

    InterfaceEntry - The protocol interface entry for the entry being closed.

Return Value:

    STATUS_SUCCESS: when successful.

--*/

{

    HASH_KEY HashKey;
    NTSTATUS Status;

    ASSERT(InterfaceEntry->ReferenceCount == 0);

    Status = STATUS_SUCCESS;

    //
    // Remove the entry from the list.  If this is the last entry in the
    // list, remove the list from the hash table.
    //

    if (InterfaceEntry->ListEntry.Flink == InterfaceEntry->ListEntry.Blink) {
        HashKey.Value = Handle;
        HashKey.Size = sizeof(EFI_HANDLE);
        HashKey.Flags = HASH_KEY_FLAGS_VALUE;
        Status = BlHtDelete(EfiProtHashTableId, &HashKey);

        //
        // Free the memory allocated for the root of the linked list.
        //

        BlMmFreeHeap(InterfaceEntry->ListEntry.Flink);

    } else {
        RemoveEntryList(&InterfaceEntry->ListEntry);
    }

    if (CHECK_FLAG(InterfaceEntry->Flags, PI_FLAGS_UNMAP_ON_CLOSE) != FALSE) {
        BlMmUnmapVirtualAddress(InterfaceEntry->Interface, PAGE_SIZE);
    }

    BlMmFreeHeap(InterfaceEntry);
    return Status;
}

NTSTATUS
EfiVmpCloseProtocol (
    __in EFI_HANDLE Handle,
    __in const EFI_GUID *Protocol
    )

/*++

Routine Description:

    This routine will call the EFI service CloseProtocol, indicating that
    the application is done with the specified protocol interface.

Arguments:

    Handle - The handle for the protocol interface that is being close.

    Protocol - The unique identifier for the protocol.

Return Value:

    STATUS_SUCCESS: when successful.
    STATUS_INVALID_PARAMETER when Handle or Protocol are invalid.
    STATUS_NOT_FOUND if Handle does not support the protocol specified by
        Protocol.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware calls.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // CloseProtocol was not specified until EFI 1.10.
    //

    if (EfiST->Hdr.Revision == EFI_1_02_SYSTEM_TABLE_REVISION) {
        EfiStatus = EFI_SUCCESS;

    } else {

        //
        // EFI Revision 1.10+.  Use CloseProtocol.
        //
        // Unfortunetly, the bulk of current EFI implementations do
        // not reference count protocol interfaces returned for an
        // individual handle.  As a result, only the first CloseProtocol
        // call will ever return success, with subsequent calls failing
        // EFI_NOT_FOUND.
        //
        // N.B. The current assumption is that other outstanding protocol
        //      interface pointers should remain valid since they are all
        //      pointers to the same internal interface structure.
        //      Resultantly, an EFI_NOT_FOUND error is ignored.  If it is
        //      ever determined that outstanding protocol interface
        //      pointers are invalidated by the first CloseProtocol call,
        //      this routine needs to be modified to never actually call
        //      CloseProtocol.
        //

        EfiStatus = EfiBS->CloseProtocol(Handle,
                                         (EFI_GUID*)Protocol, // const_cast
                                         EfiImageHandle,
                                         NULL);

        if (EfiStatus == EFI_NOT_FOUND) {
            EfiStatus = EFI_SUCCESS;
        }
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

