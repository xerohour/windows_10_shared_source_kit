/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    root.c

Abstract:

    This module contains functions that apply to root buses.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define PCI_BUGCODE_TOO_MANY_CONFIG_GUESSES     0xdead0010

DEFINE_GUID(OSC_UUID_FOR_PCI_HOST_BRIDGE,
            0x33db4d5b,
            0x1ff7, 0x401c,
            0x96, 0x57, 0x74, 0x41, 0xc0, 0x3d, 0xd7, 0x66);

#define PCI_SET_OSC_SUPPORT_FIELD(_Root)                            \
{                                                                   \
    PPCI_ROOT Root = (_Root);                                       \
    Root->OscFeatureSupport.u.AsULONG = 0;                          \
    Root->OscFeatureSupport.u.ExtendedConfigOpRegions = 1;          \
    Root->OscFeatureSupport.u.ASPM = 1;                             \
    Root->OscFeatureSupport.u.SegmentGroups = 1;                    \
    Root->OscFeatureSupport.u.MSI = 1;                              \
    Root->OscFeatureSupport.u.OptimizedBufferFlushAndFill = 1;      \
}

#define PCI_SET_OSC_CONTROL_FIELD(_Root)                            \
{                                                                   \
    PPCI_ROOT Root = (_Root);                                       \
    Root->OscControlRequest.u.AsULONG = 0;                          \
    Root->OscControlRequest.u.ExpressNativeHotPlug = 1;             \
    Root->OscControlRequest.u.ExpressNativePME = 1;                 \
    Root->OscControlRequest.u.ExpressAdvancedErrorReporting = 1;    \
    Root->OscControlRequest.u.LatencyToleranceReporting = 1;        \
}

// ----------------------------------------------------------------------- Types

// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciQueryRootBusInterface(
    __inout PPCI_ROOT Root
    );

VOID
ExpressRootComplexPowerCallback (
    __inout PVOID CallbackContext,
    __in PVOID Argument1,
    __in PVOID Argument2
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciProcessNewRootBus)
    #pragma alloc_text(PAGE, PciGetSegmentForRootBus)
    #pragma alloc_text(PAGE, PciQueryRootBusInterface)
    #pragma alloc_text(PAGE, PciGetRootBusNumbersFromBootConfig)
    #pragma alloc_text(PAGE, PciRootFilterRequirementsList)
    #pragma alloc_text(PAGE, PciRootFilterStartResources)
    #pragma alloc_text(PAGE, ExpressProcessNewRootComplex)
    #pragma alloc_text(PAGE, ExpressDestroyRootComplex)
#endif

// --------------------------------------------------------------------- Globals

// ------------------------------------------------------------------- Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciProcessNewRootBus(
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine builds the structures associated with a root PCI bus.

Arguments:

    Bus - Pointer to a bus structure which happens to be a root bus.

Return Value:

    NT_SUCCESS if everything can proceed.

--*/

{
    ULONG index;
    OBJECT_ATTRIBUTES objectAttributes;
    PPCI_ROOT root;
    NTSTATUS  status;
    HANDLE threadHandle;

    PAGED_CODE();

    root = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_ROOT));
    if (!root) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    RtlZeroMemory(root, sizeof(PCI_ROOT));
    root->Bus = Bus;
    Bus->Root = root;
    InitializeListHead(&root->PmeRootPortList);
    InitializeListHead(&root->PmeWaitWakeSendList);
    InitializeListHead(&root->PmeWaitWakeCompleteList);
    InitializeListHead(&root->PmeWaitWakeCancelList);
    InitializeListHead(&root->PmeD0SendList);
    InitializeListHead(&root->PmeD0RetireList);
    InitializeListHead(&root->PmeQueueWakeEventList);

    status = PciQueryRootBusInterface(root);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Now obtain the hardware capabilities and operating mode of a PCI root
    // bus through a direct call interace.
    //

    root->PciBusInterface->RootBusCapability(root->PciBusInterface->Context,
                                             &root->HardwareCapability);


    //
    // Copy the secondary interface type in the pci bus extension.
    //

    Bus->SecondaryInterface = root->HardwareCapability.SecondaryInterface;
    switch (Bus->SecondaryInterface) {
    case PciExpress:

        //
        // The PCI Express hierarchy originates from a Root Complex.
        // So create and attach a root complex structure to the root bus.
        //

        status = ExpressProcessNewRootComplex(Bus);
        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Native Express mode telemetery 
        //

        if (PciIsExpressGranted(root) != FALSE) {
            TraceLoggingWrite(
                PciTraceLoggingProvider, 
                "PciNativeExpressMode",
                TraceLoggingString("Native", "ExpressMode"),
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
                );
        } else {
            TraceLoggingWrite(
                PciTraceLoggingProvider, 
                "PciNativeExpressMode",
                TraceLoggingString("BIOS Controlled", "ExpressMode"),
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
                );
        }

        //
        // Fall through.
        //

    case PciXMode2:
        PCI_BUS_SET_FLAG(Bus, ExtendedConfigAvailable);
        break;
    }

    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Initialize various events and a spin lock for PME processing.
    //
    
    for (index = 0; index < PciRootPmeEventMaximum; index++) {
    
        KeInitializeEvent(&root->PmeEvent[index],
                          SynchronizationEvent,
                          FALSE);
    }

    KeInitializeSpinLock(&root->PmeListLock);

    //
    // Create a system thread dedicated for PME processing.
    //

    InitializeObjectAttributes(&objectAttributes,
                               NULL,
                               OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL);

    status = PsCreateSystemThread(&threadHandle,
                                  (ACCESS_MASK)0L,
                                  &objectAttributes,
                                  NULL,
                                  NULL,
                                  (PKSTART_ROUTINE)
                                  RootPmeEventDispatcher,
                                  root);

    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Obtain and preserve a thread object for a thread handle. This will
    // also increment a pointer reference count on the object and prevents
    // the object from being deleted.
    //

    status =
        ObReferenceObjectByHandle(threadHandle,
                                  THREAD_ALL_ACCESS,
                                  NULL,
                                  KernelMode,
                                  &root->PmeDispatcherThread,
                                  NULL);

    PCI_ASSERT(NT_SUCCESS(status));

    //
    // Close the thread handle as it is no longer in use.
    //

    ZwClose(threadHandle);

    return status;

cleanup:

    if (root) {
        PciFreePool(root);
        Bus->Root = NULL;
    }
    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDestroyRootBus(
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine destroys the structures associated with a root PCI bus.

Arguments:

    Bus - pointer to the root bus context

Return Value:

    none

--*/

{
    PPCI_BUS_INTERFACE_STANDARD interface;

    //
    // Wait for the thread to terminate and release outstanding reference
    // on the thread object.
    //

    if (Bus->Root->PmeDispatcherThread != NULL) {

        //
        // Signal an event for the PME thread to terminate itself.
        //

        KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventExitThread],
                   IO_NO_INCREMENT,
                   FALSE);
        KeWaitForSingleObject(Bus->Root->PmeDispatcherThread,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        ObDereferenceObject(Bus->Root->PmeDispatcherThread);
    }

    if (Bus->Root->RootComplex != NULL) {

        PCI_ASSERT(Bus->Root->RootComplex->PciRootBus == Bus);

        ExpressDestroyRootComplex(Bus->Root->RootComplex);
    }

    if (Bus->Root->Segment != NULL) {
        if (!(PciSystemWideHackFlags &
              PCI_SYS_HACK_IGNORE_OVERLAPPING_ROOTBUS_RANGES)) {

            PciClearBitRangeInclusive(
                &Bus->Root->Segment->BusNumberMap,
                LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber),
                LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber)
                );
        }
    }

    interface = Bus->Root->PciBusInterface;
    if (interface->InterfaceDereference != NULL) {
        interface->InterfaceDereference(interface->Context);
    }
    PciFreePool(interface);

    PciFreePool(Bus->Root);
    Bus->Root = NULL;
}

NTSTATUS
PciGetSegmentForRootBus (
    __in PPCI_ROOT Root
    )
/*++

Routine Description:

    This routine locates or creates a new instance of PCI_SEGMENT for a given
    PCI_ROOT based on its SegmentNumber. If a new instance of PCI_SEGMENT is
    created, then it is added to the global list sorted with ascending
    SegmentNumber.

Arguments:

    Root - supplies a pointer to the PCI_ROOT whose segment is to be located.

Return Value:

    STATUS_SUCCESS.

--*/
{

    PPCI_SEGMENT nextSegment;
    PPCI_SEGMENT prevSegment;
    USHORT segmentNumber;
    NTSTATUS status;
    PPCI_SEGMENT thisSegment;

    PAGED_CODE();

    PciAcquirePassiveLock(&PciGlobalLock);
    segmentNumber = SEGMENT_NUMBER_FOR_BUS(Root->Bus->SecondaryBusNumber);
    nextSegment = PciSegmentList;
    prevSegment = NULL;
    thisSegment = NULL;
    while (nextSegment) {
        if (nextSegment->SegmentNumber == segmentNumber) {
            thisSegment = nextSegment;
            break;
        }

        if (segmentNumber < nextSegment->SegmentNumber) {
            break;
        }

        prevSegment = nextSegment;
        nextSegment = nextSegment->Next;
    }

    if (thisSegment == NULL) {
        thisSegment = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_SEGMENT));
        if (thisSegment == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        RtlZeroMemory(thisSegment, sizeof(PCI_SEGMENT));
        RtlInitializeBitMap(&thisSegment->BusNumberMap,
                            thisSegment->BusNumberMapBuffer,
                            0x100);

        RtlClearAllBits(&thisSegment->BusNumberMap);

        //
        // Attach it to the global list.
        //

        thisSegment->SegmentNumber = segmentNumber;
        thisSegment->Next = nextSegment;
        if (prevSegment == NULL) {
            PciSegmentList = thisSegment;

        } else {
            prevSegment->Next = thisSegment;
        }
    }

    Root->Segment = thisSegment;

    status = STATUS_SUCCESS;

Exit:
    PciReleasePassiveLock(&PciGlobalLock);
    return status;
}

NTSTATUS
PciQueryRootBusInterface(
    __inout PPCI_ROOT Root
    )
/*++

Routine Description:

    This routine sends an IRP to the parent PDO requesting
    handlers for PCI configuration reads and writes.

Arguments:

    FdoExtension - this PCI bus's FDO extension

Return Value:

    STATUS_SUCCESS, if the PDO provided handlers

--*/
{
    NTSTATUS status;
    PPCI_BUS_INTERFACE_STANDARD interface;
    IO_STACK_LOCATION irpStack;
    PCI_BUS_INTERFACE_STANDARD oldInterface;

    PAGED_CODE();

    interface = PciAllocatePool(NonPagedPoolNx,
                                sizeof(PCI_BUS_INTERFACE_STANDARD));

    if (!interface) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    RtlZeroMemory(interface, sizeof(PCI_BUS_INTERFACE_STANDARD));
    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;

    irpStack.Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_PCI_BUS_INTERFACE_STANDARD2;
    irpStack.Parameters.QueryInterface.Version = PCI_BUS_INTERFACE_STANDARD_VERSION;
    irpStack.Parameters.QueryInterface.Size = sizeof(PCI_BUS_INTERFACE_STANDARD);
    irpStack.Parameters.QueryInterface.Interface = (PINTERFACE) interface;
    irpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;

    status = PciSendPnpIrp(Root->Bus->PhysicalDeviceObject,
                           &irpStack,
                           NULL
                           );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Look for the older version of the interface and compare the results.  If
    // they differ, then there is a filter in the stack that doesn't understand
    // Windows 8.  If that's the case, then we'll need to disable multi-stage
    // resume from hibernate, because the filter will be invoked as part of
    // reinitializing the system before the dump stack reads in the remaining
    // part of the hibernation file.
    //


    RtlZeroMemory(&oldInterface, sizeof(PCI_BUS_INTERFACE_STANDARD));
    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;

    irpStack.Parameters.QueryInterface.InterfaceType = (LPGUID)&GUID_PCI_BUS_INTERFACE_STANDARD;
    irpStack.Parameters.QueryInterface.Version = 1;
    irpStack.Parameters.QueryInterface.Size = PCI_BUS_INTERFACE_STANDARD_VERSION_1_LENGTH;
    irpStack.Parameters.QueryInterface.Interface = (PINTERFACE) &oldInterface;
    irpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;

    status = PciSendPnpIrp(Root->Bus->PhysicalDeviceObject,
                           &irpStack,
                           NULL
                           );

    if (NT_SUCCESS(status)) {

        if ((oldInterface.ReadConfig != interface->ReadConfig) ||
            (oldInterface.WriteConfig != interface->WriteConfig)) {

            if (interface->InterfaceDereference != NULL) {
                interface->InterfaceDereference(interface->Context);
            }

            //
            // Copy the old filter functions, as we're trying to keep the filter
            // doing whatever it had been doing.
            //

            RtlCopyMemory(interface,
                          &oldInterface,
                          FIELD_OFFSET(PCI_BUS_INTERFACE_STANDARD, PrepareMultistageResume));

            //
            // Mark the version-2 function that the filter didn't know about as
            // unimplemented.
            //

            interface->PrepareMultistageResume = NULL;

        } else {
    
            if (oldInterface.InterfaceDereference != NULL) {
                oldInterface.InterfaceDereference(oldInterface.Context);
            }
        }
    }

    Root->PciBusInterface = interface;

    return status;

cleanup:

    if (interface) {
        PciFreePool(interface);
    }

    Root->PciBusInterface = NULL;

    return status;

}

NTSTATUS
PciGetRootBusNumbersFromBootConfig(
    __in PPCI_ROOT Root,
    __out PULONG SecondaryBusNumber,
    __out PULONG SubordinateBusNumber
    )
/*++

Routine Description:

    This routine retrieves the range of bus numbers passed by a root
    bus from the boot config of the root bus.  The bus numbers are
    passed in the root bus's start resources, but this routine
    allows PCI to determine the bus number range before start,
    so that it can properly respond to legacy calls that require
    the bus number before the root bus is started.

Arguments:

    Root - The root to get the bus number range of.

    SecondaryBusNumber - On successful exit, will point to the secondary
        bus number of the root bus.

    SubordinateBusNumber - Will point to the last bus number passed
        by the root bus.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
    PCM_RESOURCE_LIST resourceList;
    NTSTATUS status;
    BOOLEAN found;

    PAGED_CODE();

    found = FALSE;
    descriptor = NULL;

    //
    // Get the boot configuration (CmResourceList) for
    // this PDO.  This gives us the bus number and the
    // ranges covered by this host bridge.
    //
    status = PciGetDeviceProperty(
                Root->Bus->PhysicalDeviceObject,
                DevicePropertyBootConfigurationTranslated,
                &(PVOID)resourceList
                );
    if (NT_SUCCESS(status) && (resourceList != NULL)) {

        TRACE(RESOURCE_TRACE_LEVEL, "Boot ResourceList from Root\n");
        DebugDumpResourceList(RESOURCE_TRACE_LEVEL, resourceList);

        FOR_ALL_IN_RESOURCE_LIST(resourceList, descriptor) {

            if (descriptor->Type == CmResourceTypeBusNumber) {

                found = TRUE;
                break;
            }
        }
    }

    if (found == TRUE) {

        //
        // Sanity check, there is no way to deal with numbering bridges under a
        // bus > 256 and we don't have raw and translated bus numbers yet - it
        // had better be < 0xFF!
        //
        PCI_ASSERT(LOCAL_BUS_NUMBER(descriptor->u.BusNumber.Start) <= 0xFF);

        PCI_ASSERT((LOCAL_BUS_NUMBER(descriptor->u.BusNumber.Start) +
                    descriptor->u.BusNumber.Length - 1) <= 0xFF);

        *SecondaryBusNumber = descriptor->u.BusNumber.Start;
        *SubordinateBusNumber = (descriptor->u.BusNumber.Start +
                                 descriptor->u.BusNumber.Length - 1);

        TRACE(BUS_NUMBER_TRACE_LEVEL,
              "Root Bus # 0x%x->0x%x.\n",
              *SecondaryBusNumber,
              *SubordinateBusNumber
              );

    }

    if (resourceList != NULL) {
        PciFreePool(resourceList);
    }

    status = PciGetSegmentForRootBus(Root);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (found == FALSE) {

        //
        // There is no bus number information from the boot config.
        // For the first root that this happens, assume that the bus number
        // is 0.  If another root comes along without bus number information
        // we're hosed.
        //
        if (Root->Segment->GuessedRootBusNumber == FALSE) {

            WARN("Guessing root bus number on segment 0.\n");

            *SecondaryBusNumber = 0;
            *SubordinateBusNumber = 0xFF;
            Root->Segment->GuessedRootBusNumber = TRUE;

        } else {

#pragma prefast(suppress: __WARNING_USE_OTHER_FUNCTION, "this really is fatal")
            KeBugCheckEx(PCI_BUS_DRIVER_INTERNAL,
                         PCI_BUGCODE_TOO_MANY_CONFIG_GUESSES,
                         (ULONG_PTR)Root->Bus->PhysicalDeviceObject,
                         0,
                         0
                         );
        }
    }

    return status;
}

NTSTATUS
PciRootFilterRequirementsList (
    __in PPCI_BUS Bus,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST Requirements
    )

/*++

Routine Description:

    This routine is invoked for the PCI root bus.
    In this routine, every memory requirement is tagged with the
    CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE flag.
    This flag will enable the root memory arbiter to assign memory resources to
    the PCI root bus that fall in an inaccessible region for the OS.
    As these ranges are never assigned to any PCI bridges or PCI devices by
    the root bus, it is perfectly alright to assign them to the root bus.

Arguments:

    Bus - The PCI_BUS whose requirements are being filtered.

    Requirements - The requirements list that came in the
        IRP_MN_FILTER_RESOURCE_REQUIREMENTS IRP.

Return Value:

    STATUS_SUCCESS

--*/

{

    ULONG index;
    PIO_RESOURCE_DESCRIPTOR inputDesc;
    PIO_RESOURCE_LIST inputList;
    ULONG inputOffset;

    UNREFERENCED_PARAMETER(Bus);

    PAGED_CODE();

    inputList = &Requirements->List[0];
    for (index = 0; index < Requirements->AlternativeLists; index++) {
        for(inputOffset = 0; inputOffset < inputList->Count; inputOffset++) {
            inputDesc = &inputList->Descriptors[inputOffset];
            if (inputDesc->Type == CmResourceTypeMemory) {
                inputDesc->Flags |=
                    CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE;

            }
        }

        inputList = (PIO_RESOURCE_LIST)&inputList->Descriptors[inputList->Count];
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciRootFilterStartResources (
    __in PPCI_BUS Bus,
    __inout PCM_RESOURCE_LIST RawResources,
    __inout PCM_RESOURCE_LIST TranslatedResources
    )

/*++

Routine Description:

    This routine is invoked for the PCI root bus.
    In this routine, the CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE
    is untagged from every memory resource descriptor.
    This is for both the RawResources and the TranslatedResources.

Arguments:

    Bus - The PCI_BUS whose start resources are being filtered.

    RawResources - The raw resource list in the start irp.

    TranslatedResources - The translated resource list in the start irp.

Return Value:

    STATUS_SUCCESS

--*/

{

    ULONG descCount;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR inputDesc;
    PCM_FULL_RESOURCE_DESCRIPTOR inputList;
    ULONG inputOffset;
    ULONG pass;

    UNREFERENCED_PARAMETER(Bus);

    PAGED_CODE();

    inputList = NULL;

    for (pass = 0; pass < 2; pass++) {
        switch(pass) {
        case 0:
            if (RawResources == NULL) {
                inputList = NULL;
                continue;
            }

            PCI_ASSERT(RawResources->Count == 1);

            inputList = &RawResources->List[0];
            break;

        case 1:
            if (TranslatedResources == NULL) {
                inputList = NULL;
                continue;
            }

            PCI_ASSERT(TranslatedResources->Count == 1);

            inputList = &TranslatedResources->List[0];
            break;
        }

        if (inputList == NULL) {
            break;
        }

        descCount = inputList->PartialResourceList.Count;
        for(inputOffset = 0; inputOffset < descCount; inputOffset++) {
            inputDesc =
                &inputList->PartialResourceList.PartialDescriptors[inputOffset];

            if (inputDesc->Type == CmResourceTypeMemory) {
                inputDesc->Flags &=
                    ~CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE;

            }
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressProcessNewRootComplex (
    __inout PPCI_BUS PciBus
    )

/*++

Routine Description:

    This routine allocates and attaches a root complex to the PCI root bus
    extension. It registers a callback for system power transitions and does
    a one time Native PME related preprocessing for Express Root Ports. On
    failure, this routine will deallocate the root complex.

Arguments:

    PciBus - Supplies the device extension for the root bus.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on memory allocation failure.
    An error code as returned from the ExCreateCallback routine.
    STATUS_UNSUCCESSFUL on failure to register a callback.
    STATUS_SUCCESS on success.

--*/

{

    PROOT_COMPLEX RootComplex;

    PAGED_CODE();

    RootComplex = PciAllocatePool(NonPagedPoolNx, sizeof(ROOT_COMPLEX));
    if (RootComplex == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(RootComplex, sizeof(ROOT_COMPLEX));
    RootComplex->PciRootBus = PciBus->Root->Bus;

    //
    // Initialize a lock for accessing various element lists in a root complex.
    //

    KeInitializeEvent(&RootComplex->ListLock, SynchronizationEvent, TRUE);

    //
    // Attach the newly allocated Root Complex to Pci Root.
    //

    PciAcquirePassiveLock(&PciGlobalLock);
    PciBus->Root->RootComplex = RootComplex;
    PciReleasePassiveLock(&PciGlobalLock);
    return STATUS_SUCCESS;
}

VOID
ExpressRootComplexPowerCallback (
    __inout PVOID CallbackContext,
    __in PVOID Argument1,
    __in PVOID Argument2
    )

/*++

Routine Description:

    This is a callback routine registered to be invoked on system power state
    changes. For transition out of S0, this routine runs thru the list of root
    ports and disables interrupts on them. For transitions into S0, the routine
    re-enables the interrupts if the root ports did not receive S0 Irp (which
    becomes possible in the case where the system failed to enter a sleep
    state).

    This routine must be non-pageable and will be invoked at passive level.

Arguments:

    CallbackContext - Supplies a pointer to a root complex that was passed as
        a context while registering the callback.

    Argument1 - This argument can take a number of values as defined by the
        component that supports this callback. The value that we are interested
        in is the PO_CB_SYSTEM_STATE_LOCK. This indicates that a system power
        change is imminent.

    Argument2 - The meaning of Argument2 depends on Argument1. In our case this
        value is 0 if the system is about to leave S0 and 1 if the system has
        reentered S0.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    BOOLEAN DisableInterrupts;
    BOOLEAN EnableInterrupts;
    PEXPRESS_BRIDGE ExpressBridge;
    PROOT_COMPLEX RootComplex;
    NTSTATUS Status;

    NON_PAGED_CODE();

    if (PtrToUlong(Argument1) != PO_CB_SYSTEM_STATE_LOCK) {
        return;
    }

    DisableInterrupts = FALSE;
    EnableInterrupts = FALSE;
    RootComplex = (PROOT_COMPLEX)CallbackContext;
    switch (PtrToUlong(Argument2)) {
    case 0:

        //
        // Leaving S0.
        //

        DisableInterrupts = TRUE;
        break;

    case 1:

        //
        // Entering S0.
        //

        EnableInterrupts = TRUE;
        break;

    default:
        return;
    }

    PciAcquirePassiveLock(&RootComplex->ListLock);
    for (ExpressBridge = RootComplex->RootPortList;
         ExpressBridge != NULL;
         ExpressBridge = ExpressBridge->Sibling) {

        //
        // Ensure that the bridge is started and in D0. Otherwise do not touch
        // the hardware.
        //

        Bus = ExpressBridge->PciBridge->Bus;
        if ((Bus->DeviceState != PciStarted) ||
            (Bus->PowerState != PowerDeviceD0)) {

            continue;
        }

        //
        // Acquire a remove lock for the bus. If this fails then the bus is in
        // process of being removed.
        //

        Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        if (DisableInterrupts != FALSE) {
            ExpressBridgeDisableInterrupts(ExpressBridge);
            ExpressBridge->Downstream.u.RootPort.EnableInterruptsOnS0Callback = TRUE;
        }

        if ((EnableInterrupts != FALSE) &&
            (ExpressBridge->Downstream.u.RootPort.EnableInterruptsOnS0Callback != FALSE)) {

            ExpressRootPortEnableInterrupts(ExpressBridge);
        }

        IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    }

    PciReleasePassiveLock(&RootComplex->ListLock);
    return;
}

VOID
ExpressDestroyRootComplex (
    __in __drv_in(__drv_freesMem(Pool)) PROOT_COMPLEX RootComplex
    )

/*++

Routine Description:

    This routine deallocates the RootComplex structure.

Arguments:

    RootComplex - Supplies the structure that represents a Root Complex.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PciAcquirePassiveLock(&RootComplex->ListLock);

    PCI_ASSERT((RootComplex->IntegratedEndpointList == NULL) &&
               (RootComplex->EventCollectorList == NULL) &&
               (RootComplex->RootPortList == NULL));

    PciReleasePassiveLock(&RootComplex->ListLock);

    //
    // Free up the ROOT_COMPLEX structure.
    //

    PciFreePool(RootComplex);
    return;
}
