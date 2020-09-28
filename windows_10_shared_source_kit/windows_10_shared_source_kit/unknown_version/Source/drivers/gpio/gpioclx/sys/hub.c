/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hub.c

Abstract:

    This file contains routines related to the GPIO hub module. The hub
    module provides dynamic management of GPIO instances, i.e. GPIO controller
    instances coming online or going away. The hub module maintains a
    database of VIRQ and VIO assignments for GPIO controllers and dispatches
    interrupt and IO requests to the correct GPIO instance.

    The GPIO hub driver is technically part of the class extension but it is
    referenced differently to distinguish between hub services and purely
    class extension/client driver-related services.


Environment:

    Kernel mode

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "pch.h"
#include "hub.h"

#ifdef EVENT_TRACING
#include "hub.tmh"
#endif

#include <bugcodes.h>
#include <ntintsafe.h>

//
// -------------------------------------------------------------------- Defines
//

//
// Macro to dereference an entry within the global hub list.
//

#define GpioHubpDereferenceGpioEntry(Entry)                             \
    GpioHubpReleaseGpioEntryShared(Entry, TRUE)

//
// Macro to acquire and release emergency queue lock. Note the global hub lock
// is used to protect it as well (the emergency queue should be rarely used).
//

#define GpioHubAcquireEmergencyQueueLock(x) GpioHubpAcquireGlobalLock(x)

#define GpioHubReleaseEmergencyQueueLock(x) GpioHubpReleaseGlobalLock(x)

//
// ---------------------------------------------------------------------- Types
//

//
// Define a list that maps a given VIRQ directly to the GPIO hub entry.
//

typedef struct _GPIO_HUB_VIRQ_LIST_ENTRY {

    //
    // The list pointers on VIRQ list.
    //

    LIST_ENTRY VirqListEntry;
    ULONG Virq;

    //
    // This field will never be NULL for as long GPIO_HUB_VIRQ_LIST_ENTRY is
    // in GpioHubVirqList.
    //

    PGPIO_HUB_LIST_ENTRY GpioEntry;

    //
    // Cached data opaque to the hub, maintained by the core GPIO logic.
    //
    // Currently, this holds the controller-relative pin number that
    // corresponds to this VIRQ.
    //
    // This field is set to INVALID_PIN_NUMBER when the containing
    // GPIO_HUB_VIRQ_LIST_ENTRY is initially inserted into GpioHubVirqList.
    //
    // Chronologically, it is updated with the real pin number after
    // GPIO_PIN_INFORMATION_ENTRY::Virq is set but before the interrupt is
    // enabled in hardware.
    //
    // This allows GpioClxQueryPinInformation() to quickly determine the
    // pin number (it does not need to iterate through all of the pins of the
    // controller).
    //

    volatile ULONG_PTR Cache;

} GPIO_HUB_VIRQ_LIST_ENTRY, *PGPIO_HUB_VIRQ_LIST_ENTRY;

//
// -------------------------------------------------------------------- Globals
//

//
// This master list holds all the GPIO instances that are registered with the
// GPIO hub. The entries get added when GPIO clients come online and register
// with GPIO hub. Deletion of a list entry blocks if
// GpioHubpAcquireGpioEntryShared() has been called. The actual deletion will
// occur under the below spinlock.
//
// This threads together GPIO_HUB_LIST_ENTRYs.
//

LIST_ENTRY GpioHubList;

//
// Each entry on this VIRQ list holds a reference to the GPIO instance (on the
// master list) that manages the given enabled VIRQ. The lifetime of each entry
// is a subset of the lifetime of its GPIO instance:
//
// * A VIRQ entry is added some time before the VIRQ is enabled in hardware
// * A VIRQ entry is removed some time after the VIRQ is disabled in hardware
//
// This threads together GPIO_HUB_VIRQ_LIST_ENTRYs.
//
// The entries are ordered by increasing VIRQ.
//

LIST_ENTRY GpioHubVirqList;

//
// The global spinlock used to protect traversal of the hub lists. This does
// not protect the contents of the entries.
//

KSPIN_LOCK GpioHubListSpinLock;

//
// Define the DPC used to signal per-entry event indicating that the entry
// is available for exclusive use. The DPC is required because an event can
// only be signalled at IRQL <= DISPATCH_LEVEL.
//

WDFDPC GpioHubDpc = NULL;

//
// Define the global signal list. This is a list of entries that require their
// notification event signalled.
//

LIST_ENTRY GpioHubSignalList;

//
// Define the global interrupt replay list. This is a list of entries that
// require their interrupt service routine to be called in order to replay
// interrupts.
//

LIST_ENTRY GpioHubInterruptReplayList;

//
// Define the lock used to protect accesses to the signal and interrupt replay
// lists.
//

KSPIN_LOCK GpioHubSignalReplayListLock;

//
// Define a flag to determine whether the signal and/or replay DPC is running or
// not.
//

BOOLEAN GpioHubDpcRunning;

//
// The WDF device object representing the GPIO hub.
//

WDFDEVICE GpioHubDevice;

//
// The IO target to the Resource Hub device.
//

WDFIOTARGET GpioHubResourceHubTarget;

//
// Define global emergency service queue and associated events.
//

KEVENT GpioHubEmergencyWorkEvent;
KEVENT GpioHubEmergencyTerminateEvent;
LIST_ENTRY GpioHubEmergencyWorkQueue;
PETHREAD GpioHubEmergencyThread;

//
// ----------------------------------------------------------------- Prototypes
//

__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioHubpAddGpioEntry (
    __inout __drv_aliasesMem PGPIO_HUB_LIST_ENTRY GpioEntry
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpCreateHubDeviceObject (
    __in WDFDRIVER Driver
    );

_Must_inspect_result_
__drv_sameIRQL
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpCreateVirqMapping (
    __in ULONG Virq,
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __out_opt PGPIO_HUB_VIRQ_LIST_ENTRY *VirqEntryOut
    );

__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpDeleteGpioEntry (
    __in __drv_freesMem(Mem) PGPIO_HUB_LIST_ENTRY GpioEntry
    );

KSTART_ROUTINE GpioHubpEmergencyWorkerThread;

_Must_inspect_result_
__drv_sameIRQL
__drv_requiresIRQL(HIGH_LEVEL)
PGPIO_HUB_VIRQ_LIST_ENTRY
GpioHubpFindVirqEntryLocked (
    __in ULONG Virq
    );

_Must_inspect_result_
__drv_sameIRQL
__drv_maxIRQL(HIGH_LEVEL)
PGPIO_HUB_LIST_ENTRY
GpioHubpFindGpioEntry (
    __in __drv_strictTypeMatch(__drv_typeConst) INPUT_VALUE_TYPE ValueType,
    __in ULONG_PTR Value,
    __out_opt PGPIO_HUB_VIRQ_LIST_ENTRY *VirqEntryOut
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitializeEmergencyServices (
    VOID
    );

VOID
GpioHubpInsertReplayList (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry
    );

__drv_sameIRQL
VOID
GpioHubpRemoveVirqListEntry (
    __in BOOLEAN ValueIsVirq,
    __in ULONG_PTR Value
    );

__drv_requiresIRQL(HIGH_LEVEL)
VOID
GpioHubpRemoveVirqListEntryListLocked (
    __in BOOLEAN ValueIsVirq,
    __in ULONG_PTR Value,
    __out PLIST_ENTRY DeletedListHead
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpMapGsivToControllerName (
    __in ULONG Gsiv,
    __out PUNICODE_STRING BiosName
    );

EVT_WDF_DPC GpioHubpProcessSignalReplayList;

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpQuerySecondaryInterruptInformation (
    VOID
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpRegisterSecondaryInterruptInterface (
    __in ULONG GsivBase,
    __in ULONG GsivSize,
    __in PDRIVER_OBJECT DriverObject
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioHubpUnregisterSecondaryInterruptInterface (
    __in ULONG GsivBase,
    __in ULONG GsivSize,
    __in PDRIVER_OBJECT DriverObject
    );

//
// Shared/Exclusive secondary IC entry locking related routines.
//

__drv_setsIRQL(HIGH_LEVEL)
VOID
GpioHubpAcquireGpioEntryExclusive (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OriginalIrql
    );

VOID
GpioHubpAcquireGpioEntryShared (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry
    );

__drv_setsIRQL(OldIrql)
VOID
GpioHubpReleaseGpioEntryExclusive (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    );

VOID
GpioHubpReleaseGpioEntryShared (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __in BOOLEAN SignalEvent
    );

//
// Utility global hub and signal list lock acquire/release routines.
//

__drv_setsIRQL(HIGH_LEVEL)
VOID
GpioHubpAcquireGlobalLock (
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OldIrql
    );

__drv_setsIRQL(HIGH_LEVEL)
VOID
GpioHubpAcquireSignalReplayListLock (
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OldIrql
    );

__drv_setsIRQL(OldIrql)
VOID
GpioHubpReleaseGlobalLock (
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    );

__drv_setsIRQL(OldIrql)
VOID
GpioHubpReleaseSignalReplayListLock (
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    );

//
// -------------------------------------------------------------------- Pragmas
//

//
// Routines exported to the HAL.
//

#pragma alloc_text(PAGE, GpioHubEnableInterrupt)
#pragma alloc_text(PAGE, GpioHubQueryPrimaryInterruptInformation)

//
// Routines exported to the class extension.
//

#pragma alloc_text(PAGE, GpioHubpInitialize)
#pragma alloc_text(PAGE, GpioHubpRegisterGpioDevice)
#pragma alloc_text(PAGE, GpioHubpUnregisterGpioDevice)

//
// Internal-only routines.
//

#pragma alloc_text(PAGE, GpioHubpInitializeEmergencyServices)
#pragma alloc_text(PAGE, GpioHubpInitializeSecondaryICInterface)
#pragma alloc_text(PAGE, GpioHubpMapGsivToControllerName)
#pragma alloc_text(PAGE, GpioHubpQuerySecondaryInterruptInformation)
#pragma alloc_text(PAGE, GpioHubpRegisterSecondaryInterruptInterface)
#pragma alloc_text(PAGE, GpioHubpUnregisterSecondaryInterruptInterface)

//
// -------------------------------------------------- Routines exported to HAL
//

_Must_inspect_result_
__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubEnableInterrupt (
    __in PVOID Context,
    __in ULONG Virq,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity,
    __in ULONG_PTR CallbackContext
    )

/*++

Routine Description:

    This routine enables the specified interrupt line behind a GPIO controller.
    This is an interface exported to HAL and is invoked by the HAL when a
    driver tries to connect its interrupt.

    N.B. It is illegal to call GpioHubDisableInterrupt() with the same Virq
         while this routine is executing, because it makes no sense to try to
         disable an interrupt that is in the middle of being enabled.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the hub device extension.

    Virq - Supplies the VIRQ for interrupt line that should be enabled.

    InterruptMode - Supplies the trigger mode (edge or level) associated with
        this interrupt.

    Polarity - Supplies the polarity (active low or active high) associated with
        this interrupt.

    CallbackContext - Supplies a context that the caller of this routine should
        be passed with the interrupt line fires.

Return Value:

    NTSTATUS code.

--*/

{

    UNICODE_STRING BiosName;
    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);

    GpioEntry = NULL;
    RtlZeroMemory(&BiosName, sizeof(BiosName));

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global VIRQ list. If the entry is not found, then find the
    // BIOS name of the GPIO controller owning that range using the services of
    // the Resource Hub.
    //

    VirqEntry = NULL;
    GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, &VirqEntry);
    if (GpioEntry == NULL) {
        Status = GpioHubpMapGsivToControllerName(Virq,
                                                 &BiosName);

        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INTERRUPT,
                        "GpioHubEnableInterrupt: Failed to query BIOS "
                        "descriptor from the Resource Hub! Gsiv = %#x, "
                        "Status = %#x\n",
                        Virq,
                        Status);

            goto EnableInterruptEnd;
        }

        //
        // Find the entry for the GPIO controller that manages the supplied
        // VIRQ in the global hub list. If the entry is not found, then it
        // implies that the controller supposed to manage that range did not
        // register (or could have already unregistered).
        //

        GpioEntry = GpioHubpFindGpioEntry(InputTypeBiosName,
                                          (ULONG_PTR)&BiosName,
                                          NULL);

        if (GpioEntry == NULL) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INTERRUPT,
                        "GpioHubEnableInterrupt: Failed to find BIOS name to "
                        "GPIO entry mapping! Gsiv = %#x, Bios Name = %S, "
                        "Status = %#x\n",
                        Virq,
                        BiosName.Buffer,
                        Status);

            Status = STATUS_GPIO_INSTANCE_NOT_REGISTERED;
            goto EnableInterruptEnd;
        }

        VirqEntry = NULL;
        Status = GpioHubpCreateVirqMapping(Virq, GpioEntry, &VirqEntry);
        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INTERRUPT,
                        "GpioHubEnableInterrupt: Failed to create Gsiv to GPIO "
                        "entry mapping! Gsiv = %#x, Bios Name = %S, "
                        "Status = %#x\n",
                        Virq,
                        BiosName.Buffer,
                        Status);

           goto EnableInterruptEnd;

        } else {
            TraceEvents(GpioLogHandle,
                        Info,
                        DBG_INTERRUPT,
                        "GpioHubEnableInterrupt: Created Gsiv: %#x <--> BIOS "
                        "name: %S <--> GPIO entry %p mapping.\n",
                        Virq,
                        BiosName.Buffer,
                        GpioEntry);
        }
    }

    GPIO_ASSERT(VirqEntry != NULL);

    //
    // Call into the GPIO class extension to connect the interrupt.
    //
    // The enable request will be synchronized with any parallel
    // IRP_MN_REMOVE_DEVICE. Futhermore, if the request is accepted onto
    // the WDF default I/O queue, IRP_MN_REMOVE_DEVICE could not have been
    // accepted before it. If the request is rejected, VirqEntry may have
    // been deleted but the code won't access it anyway since the request
    // won't be processed.
    //
    // Therefore, during the processing of the enable request, VirqEntry
    // will exist since IRP_MN_REMOVE_DEVICE / GpioHubpUnregisterGpioDevice()
    // will not have a chance to run until after the request is complete.
    //

    Status = GpioClxEnableInterrupt(GpioEntry->Context,
                                    Virq,
                                    InterruptMode,
                                    Polarity,
                                    CallbackContext,
                                    &VirqEntry->Cache);

    //
    // Drop the reference to the entry that was acquired implicitly when the
    // entry was searched.
    //

EnableInterruptEnd:
    if (GpioEntry != NULL) {
        GpioHubpDereferenceGpioEntry(GpioEntry);
    }

    if (BiosName.Buffer != NULL) {
        GPIO_FREE_POOL(BiosName.Buffer);
    }

    return Status;
}

__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubDisableInterrupt (
    __in PVOID Context,
    __in ULONG Virq
    )

/*++

Routine Description:

    This routine disables the specified interrupt line behind a GPIO controller.
    This is an interface exported to HAL and is invoked by the HAL when a
    driver tries to disconnect its interrupt.

    This routine is not marked PAGED as it may be called before/after the boot
    device is in D0/D3 if boot device has GPIO dependencies.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the hub device extension.

    Virq - Supplies the VIRQ for interrupt line that should be disconnected.

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global hub list. If the entry is not found, then it implies that
    // the controller supposed to manage that range did not register (or could
    // have already unregistered).
    //

    GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, NULL);
    if (GpioEntry == NULL) {
        Status = STATUS_GPIO_INSTANCE_NOT_REGISTERED;
        goto DisableInterruptEnd;
    }

    //
    // Call into the GPIO class extension to disconnect the interrupt.
    //

    Status = GpioClxDisableInterrupt(GpioEntry->Context, Virq);

    //
    // Remove the entry from the VIRQ list so that it cannot be used for
    // future operations.
    //

    GpioHubpRemoveVirqListEntry(TRUE, Virq);

    //
    // Drop the reference to the entry that was acquired implicitly when the
    // entry was searched.
    //

DisableInterruptEnd:
    if (GpioEntry != NULL) {
        GpioHubpDereferenceGpioEntry(GpioEntry);
    }

    return Status;
}

NTSTATUS
GpioHubMaskInterrupt (
    __in PVOID Context,
    __in ULONG Flags,
    __in ULONG Virq
    )

/*++

Routine Description:

    This routine masks the specified interrupt line behind a GPIO controller.
    This is an interface exported to HAL and is invoked by the HAL when a
    driver tries to mask its interrupt.

    N.B. This routine can be entered at DIRQL level (e.g. can be called from
         GPIO's ISR when it calls the HAL to mask its interrupt on the parent
         GPIO controller when the GPIO controller is nested).

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the GPIO device extension.

    Flags - Supplies flags that control mask/unmask behavior.

    Virq - Supplies the VIRQ for interrupt line that should be masked.

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global hub list.
    //
    // If the entry is not found, then it implies that the controller supposed
    // to manage that range did not register (or could have already
    // unregistered). Treat the interrupt as masked in this case as the
    // interrupt cannot fire.
    //

    GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, NULL);
    if (GpioEntry == NULL) {
        Status = STATUS_GPIO_INSTANCE_NOT_REGISTERED;
        goto HubMaskInterruptEnd;
    }

    //
    // N.B. Mask requests are forwarded even in cases where a delete is
    //      pending as failure to mask can cause catastrophic issues (e.g. a
    //      GPIO controller becoming permanently masked).
    //

    //
    // Call into the GPIO class extension to mask the interrupt.
    //

    Status = GpioClxMaskInterrupt(GpioEntry->Context, Flags, Virq);

    //
    // Drop the reference to the entry that was acquired implicitly when the
    // entry was searched.
    //

HubMaskInterruptEnd:
    if (GpioEntry != NULL) {
        GpioHubpDereferenceGpioEntry(GpioEntry);
    }

    return Status;
}

__drv_sameIRQL
NTSTATUS
GpioHubUnmaskInterrupt (
    __in PVOID Context,
    __in ULONG Flags,
    __in ULONG Virq
    )

/*++

Routine Description:

    This routine unmasks the specified interrupt line behind a GPIO controller.
    Unmasking the interrupt line will enable the CPU to be interrupted when the
    line fires again.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the GPIO device extension.

    Flags - Supplies flags that control mask/unmask behavior.

    Virq - Supplies the VIRQ for interrupt line that should be unmasked.

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global hub list. The entry may not exist if the interrupt
    // has already been disconnected (e.g. race between passive-level ISR and
    // interrupt disconnect).
    //

    GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, NULL);
    if (GpioEntry == NULL) {
        Status = STATUS_GPIO_INSTANCE_NOT_REGISTERED;
        goto HubUnmaskInterruptEnd;
    }

    //
    // Call into the GPIO class extension to unmask the interrupt.
    //

    Status = GpioClxUnmaskInterrupt(GpioEntry->Context, Flags, Virq);

    //
    // Drop the reference to the entry that was acquired implicitly when the
    // entry was searched.
    //

HubUnmaskInterruptEnd:
    if (GpioEntry != NULL) {
        GpioHubpDereferenceGpioEntry(GpioEntry);
    }

    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubQueryPrimaryInterruptInformation (
    __in PVOID Context,
    __in ULONG Virq,
    __out PPRIMARY_INTERRUPT_INFORMATION PrimaryInformation
    )

/*++

Routine Description:

    This routine queries the interrupt information (like mode, polarity, IRQL
    etc.) for the primary GPIO controller interrupt mapping to the given
    VIRQ. This is an interface exported to HAL and is invoked by the HAL during
    interrupt arbitration (when the primary interrupt information is needed).

    N.B. In test scenarios, the GPIO controller tree may be total virtual,
         i.e., the root GPIO controller interrupt might itself be secondary.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the GPIO device extension.

    Gsiv - Supplies a pointer that receives the GSIV for the GPIO controller's
        interrupt.

    PrimaryInformation - Supplies a pointer to the buffer that receives the
        primary interrupt information.

Return Value:

    STATUS_SUCCESS if the information for the GPIO controller connected to
        primary interrupt controller could be found.

    STATUS_MORE_PROCESSING_REQUIRED if no primary parent GPIO controller
        could be found. In this case, the information returned pertains to the
        virtually-connected root GPIO controller.

    STATUS_NOT_FOUND otherwise.

--*/

{

    GROUP_AFFINITY Affinity;
    UNICODE_STRING BiosName;
    BOOLEAN Found;
    PGPIO_HUB_LIST_ENTRY GpioEntry;
    KINTERRUPT_MODE InterruptMode;
    KIRQL Irql;
    NTSTATUS LocalStatus;
    BOOLEAN ParentFound;
    KINTERRUPT_POLARITY Polarity;
    BOOLEAN SecondaryInterrupt;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    PAGED_CODE();

    //
    // Silence compiler.
    //

    InterruptMode = 0;
    Irql = 0;
    Polarity = 0;
    Affinity.Group = 0x0;
    Affinity.Mask = 0x0;

    //
    // The input VIRQ must be a secondary interrupt.
    //

    if (GpioHubpIsSecondaryInterrupt(Virq) == FALSE) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INTERRUPT,
                    "GpioHubQueryPrimaryInterruptInformation: Supplied Gsiv is "
                    "not in the secondary interrupt range. Gsiv = %#x!\n",
                    Virq);

        Status = STATUS_INVALID_PARAMETER;
        goto QueryPrimaryInterruptInformationEnd;
    }

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global VIRQ list. If the entry is not found, then find the
    // BIOS name of the GPIO controller owning that range using the services of
    // the Resource Hub.
    //

    GpioEntry = NULL;
    Found = FALSE;
    ParentFound = FALSE;
    Status = STATUS_NOT_FOUND;
    while (Found == FALSE) {
        GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, NULL);
        if (GpioEntry == NULL) {
            Status = GpioHubpMapGsivToControllerName(Virq,
                                                     &BiosName);

            if (!NT_SUCCESS(Status)) {
                TraceEvents(GpioLogHandle,
                            Error,
                            DBG_INTERRUPT,
                            "%s: Failed to query BIOS descriptor  from the "
                            "Resource Hub! Gsiv = %#x, Status = %#x\n",
                            __FUNCTION__,
                            Virq,
                            Status);

                break;
            }

            //
            // Find the entry for the GPIO controller that manages the supplied
            // VIRQ in the global hub list. If the entry is not found, then it
            // implies that the controller supposed to manage that range did not
            // register (or could have already unregistered).
            //

            GpioEntry = GpioHubpFindGpioEntry(InputTypeBiosName,
                                              (ULONG_PTR)&BiosName,
                                              NULL);

            if (GpioEntry == NULL) {
                TraceEvents(GpioLogHandle,
                            Error,
                            DBG_INTERRUPT,
                            "%s: Failed to find BIOS name to GPIO entry mapping"
                            "! Gsiv = %#x, Bios Name = %S, Status = %#x\n",
                            __FUNCTION__,
                            Virq,
                            BiosName.Buffer,
                            Status);

            //
            // Attempt to create the VIRQ <--> GPIO entry mapping, to speed
            // up a future call to enable the interrupt.
            //
            // If this fails, the mapping will be attempted again when the
            // interrupt is enabled. So don't treat a failure as fatal as yet.
            //

            } else {
                LocalStatus = GpioHubpCreateVirqMapping(Virq, GpioEntry, NULL);
                if (!NT_SUCCESS(LocalStatus)) {
                    TraceEvents(GpioLogHandle,
                                Error,
                                DBG_INTERRUPT,
                                "%s: Failed to create Gsiv "
                                "to GPIO entry mapping! Gsiv = %#x, Bios "
                                "Name = %S, Status = %#x\n",
                                __FUNCTION__,
                                Virq,
                                BiosName.Buffer,
                                Status);

                   __fallthrough;

                } else {
                    TraceEvents(GpioLogHandle,
                                Info,
                                DBG_INTERRUPT,
                                "%s: Created Gsiv: %#x <--> "
                                "BIOS name: %S <--> GPIO entry %p mapping.\n",
                                __FUNCTION__,
                                Virq,
                                BiosName.Buffer,
                                GpioEntry);
                }
            }

            //
            // Free the BIOS name buffer as it is no longer required.
            //

            GPIO_FREE_POOL(BiosName.Buffer);
        }

        if (GpioEntry == NULL) {
            Status = STATUS_NOT_FOUND;
            break;
        }

        //
        // Call into the GPIO class extension to query the interrupt info.
        //

        Status = GpioClxQueryInterruptInformation(GpioEntry->Context,
                                                  Virq,
                                                  &Virq,
                                                  &Irql,
                                                  &Affinity,
                                                  &InterruptMode,
                                                  &Polarity);

        //
        // If the parent controller does not consume any interrupt, then treat
        // it as a failure (and discard the result).
        //

        if (NT_SUCCESS(Status) && (Virq == GPIO_UNSPECIFIED_INTERRUPT)) {
            Status = STATUS_UNSUCCESSFUL;
            __fallthrough;
        }

        //
        // If the GPIO is connected to the primary interrupt controller on the
        // platform, then return its information. Otherwise, search for the
        // parent GPIO controller that manages this GPIO's interrupt.
        //

        if (NT_SUCCESS(Status)) {
            SecondaryInterrupt = GpioHubpIsSecondaryInterrupt(Virq);
            if (SecondaryInterrupt == FALSE) {
                Found = TRUE;
            }
        }

        //
        // Drop the reference to the entry that was acquired implicitly when the
        // entry was searched.
        //

        GpioHubpDereferenceGpioEntry(GpioEntry);

        //
        // If querying for the interrupt information failed, then bail out.
        //

        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INTERRUPT,
                        "GpioHubQueryPrimaryInterruptInformation: Failed to "
                        "query interrupt information! Gsiv = %#x, Entry = %p\n",
                        Virq,
                        GpioEntry);

            break;
        }

        //
        // Register that parent GPIO controller's information was successfully
        // queried. In cases where we fail to find a parent GPIO controller
        // that is connected to the primary interrupt controller, we can
        // at least return information pertaining the root GPIO controller (also
        // virtual).
        //
        // N.B. This scenario is possible in test where the entire GPIO
        //      controller tree is virtual.
        //

        ParentFound = TRUE;
    }

    //
    // If a GPIO is connected to the primary interrupt controller could be
    // found, then return its information. Otherwise return the information
    // for the root GPIO (also virtual) if one could be found.
    //

    if ((Found != FALSE) || (ParentFound != FALSE)) {
        PrimaryInformation->Size = sizeof(PRIMARY_INTERRUPT_INFORMATION);
        PrimaryInformation->Version = SECONDARY_INTERRUPT_PRIMARY_INTERRUPT_INFORMATION_VERSION;
        PrimaryInformation->PrimaryGsiv = Virq;
        PrimaryInformation->PrimaryIrql = Irql;
        PrimaryInformation->PrimaryAffinity = Affinity;
        PrimaryInformation->InterruptMode = InterruptMode;
        PrimaryInformation->Polarity = Polarity;
    }

    if (Found != FALSE) {
        Status = STATUS_SUCCESS;
        TraceEvents(GpioLogHandle,
                    Info,
                    DBG_INTERRUPT,
                    "GpioHubQueryPrimaryInterruptInformation: Primary interrupt"
                    " mapping found! GSIV = %#x\n",
                    Virq);

    } else if (ParentFound != FALSE) {
        Status = STATUS_MORE_PROCESSING_REQUIRED;
        TraceEvents(GpioLogHandle,
                    Info,
                    DBG_INTERRUPT,
                    "GpioHubQueryPrimaryInterruptInformation: Primary interrupt"
                    " mapping not found! Returning root (virtual) GPIO "
                    "controller information. GSIV = %#x\n",
                    Virq);


    } else {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INTERRUPT,
                    "GpioHubQueryPrimaryInterruptInformation: Primary interrupt"
                    " mapping not found!\n");
    }

QueryPrimaryInterruptInformationEnd:
    return Status;
}

__drv_sameIRQL
__drv_requiresIRQL(HIGH_LEVEL)
NTSTATUS
GpioHubRequestInterrupt (
    __in PVOID Context,
    __in ULONG Virq,
    __out PULONG NextGsiv
    )

/*++

Routine Description:

    This routine requests that the specified interrupt line should be considered
    to have asserted the next time this interrupt controllers line has asserted.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the GPIO device extension.

    Virq - Supplies the VIRQ for interrupt line that should be asserted.

    NextGsiv - Supplies a pointer to a variable which receives the GSIV that
        needs to be asserted to ensure the eventual servicing the requested
        interrupt, if applicable.

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Find the entry for the GPIO controller that manages the supplied VIRQ
    // in the global hub list. If the entry is not found, then it implies that
    // the controller supposed to manage that range did not register (or could
    // have already unregistered).
    //

    GpioEntry = GpioHubpFindGpioEntry(InputTypeVirq, Virq, NULL);
    if (GpioEntry == NULL) {

        GPIO_ASSERT(FALSE);

        Status = STATUS_GPIO_INSTANCE_NOT_REGISTERED;
        goto HubRequestInterruptEnd;
    }

    //
    // Call into the GPIO class extension to request the interrupt.
    //

    Status = GpioClxRequestInterrupt(GpioEntry->Context, Virq, NextGsiv);
    if ((Status == STATUS_SUCCESS) &&
        (*NextGsiv == GPIO_UNSPECIFIED_INTERRUPT)) {

        //
        // Since this controller isn't connected to a primary interrupt queue
        // a DPC to ensure the replayed interrupt is properly serviced.
        //

        GpioHubpInsertReplayList(GpioEntry);

    } else if (Status == STATUS_SECONDARY_INTERRUPT_ALREADY_PENDING) {
        Status = STATUS_SUCCESS;
    }

    //
    // Drop the reference to the entry that was acquired implicitly when the
    // entry was searched.
    //

HubRequestInterruptEnd:
    if (GpioEntry != NULL) {
        GpioHubpDereferenceGpioEntry(GpioEntry);
    }

    return Status;
}

NTSTATUS
GpioHubQueryLineInformation (
    __in PVOID Context,
    __in ULONG Gsiv,
    __out PUSHORT PinIndex,
    __out PVOID *ControllerId
    )

/*++

Routine Description:

    This routine queries controller-specific information for the given GSIV.
    The information includes pin number and controller ID. The controller ID
    in this case maps to the power handle.

    N.B. This routine must be called at HIGH_LEVEL with all other CPUs asleep.

Arguments:

    Context - Supplies a pointer to the context supplied at the time of
        registration. The context points to the GPIO instance extension.

    Gsiv - Supplies a pointer that receives the GSIV for the GPIO controller's
        interrupt.

    PinIndex - Supplies a pointer that receives the controller-relative pin
        number.

    ControllerId - Supplies a pointer that receives the controller ID.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS Status;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    UNREFERENCED_PARAMETER(Context);

    GPIO_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);

    VirqEntry = GpioHubpFindVirqEntryLocked(Gsiv);

    //
    // If a context switch had occurred early in the interrupt enable path,
    // the VIRQ entry might not have been created yet. The interrupt would not
    // have been enabled in hardware either.
    //

    if (VirqEntry == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto QueryLineInformationEnd;
    }

    GPIO_ASSERT(VirqEntry->GpioEntry != NULL);

    //
    // Call into the GPIO class extension to query the pin number and
    // controller ID.
    //

    Status = GpioClxQueryPinInformation(VirqEntry->GpioEntry->Context,
                                        Gsiv,
                                        &VirqEntry->Cache,
                                        PinIndex,
                                        ControllerId);

QueryLineInformationEnd:
    return Status;
}

//
// --------------------------------------- Routines exported to class extension
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitialize (
    __in ULONG Phase,
    __in PDRIVER_OBJECT WdmDriverObject,
    __in WDFDRIVER WdfDriver
    )

/*++

Routine Description:

    This routine initialize the GPIO hub module. This is called when the hub
    client driver device registers with the GPIO class extension.

    Phase 0 - This is called during driver entry for GPIO class extension.
        Global lists and spinlocks are initialized during this phase.

    Phase 1 - This is called during AddDevice for GPIO hub device.
        During this phase, secondary interrupt interface is registered with the
        HAL. Also any initialization that needs to be deferred until a WDF
        device object exists is done now.

Arguments:

    Phase - Supplies the phase of the initialization.

    DriverObject - Supplies a pointer to the GPIO class extension's WDM driver
        object.

    WdfDriver - Supplies a handle to the WDF driver object.

Return Value:

    NTSTATUS code.

--*/

{

    WDF_OBJECT_ATTRIBUTES Attributes;
    WDF_DPC_CONFIG DpcConfiguration;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Initialize global VIRQ, VIO and signal lists.
    //

    if (Phase == 0) {

        InitializeListHead(&GpioHubList);
        InitializeListHead(&GpioHubVirqList);

        //
        // Initialize signal list and associated globals.
        //

        InitializeListHead(&GpioHubSignalList);
        InitializeListHead(&GpioHubInterruptReplayList);
        KeInitializeSpinLock(&GpioHubListSpinLock);
        KeInitializeSpinLock(&GpioHubSignalReplayListLock);
        GpioHubDpcRunning = FALSE;
        Status = GpioHubpInitializeEmergencyServices();

    } else {

        GPIO_ASSERT(Phase == 1);

        Status = GpioHubpCreateHubDeviceObject(WdfDriver);
        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INIT,
                        "%s: GpioHubpCreateHubDeviceObject() failed! "
                        "Status = %#x\n",
                        __FUNCTION__,
                        Status);

            goto InitializeEnd;
        }

        GpioHubResourceHubTarget = NULL;

        //
        // Create the DPC to process the signal list.
        //

        WDF_DPC_CONFIG_INIT(&DpcConfiguration, GpioHubpProcessSignalReplayList);
        DpcConfiguration.AutomaticSerialization = FALSE;
        WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
        Attributes.ParentObject = GpioHubDevice;
        Status = WdfDpcCreate(&DpcConfiguration,
                              &Attributes,
                              &GpioHubDpc);

        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INIT,
                        "GpioHubpInitialize: WdfDpcCreate() failed! "
                        "Status = %#x\n ",
                        Status);

            goto InitializeEnd;
        }

        //
        // Register the secondary interrupt interface with the HAL.
        //

        Status = GpioHubpInitializeSecondaryICInterface(WdmDriverObject);
        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_PNP,
                        "GpioHubpInitialize: Failed to register secondary "
                        "interrupt interface with the HAL! Status = %#x\n",
                        Status);
        }
    }

InitializeEnd:
    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpCreateHubDeviceObject (
    __in WDFDRIVER Driver
    )

/*++

Routine Description:

    This routine will create a device object representing the GPIO hub device.
    The object is required in order to utilize WDF services (for e.g. create
    a DPC).

Arguments:

    Driver - Supplies a handle to the GPIO class extension's WDF driver object.

Return Value:

    NTSTATUS code.

--*/

{

    PWDFDEVICE_INIT DeviceInit;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Prepare the device initialization structure for the call to
    // WdfDeviceCreate
    //

    DeviceInit = WdfControlDeviceInitAllocate(Driver,
                                              &SDDL_DEVOBJ_SYS_ALL_ADM_ALL);

    if (DeviceInit == NULL) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "%s: WdfControlDeviceInitAllocate() failed!\n",
                    __FUNCTION__);

        Status = STATUS_UNSUCCESSFUL;
        goto DriverEntryEnd;
    }

    //
    // Set various attributes for this device.
    //

    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);

    //
    // Call the framework to create the device.
    //

    Status = WdfDeviceCreate(&DeviceInit,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &GpioHubDevice);

    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_PNP,
                    "%s: WdfDeviceCreate() failed! Status = %#x\n",
                    __FUNCTION__,
                    Status);

        goto DriverEntryEnd;
    }

    //
    // As the GPIO hub is a non-PNP device, WDF needs to be explicitly told to
    // complete the initialization.
    //

    WdfControlFinishInitializing(GpioHubDevice);

DriverEntryEnd:
    if (!NT_SUCCESS(Status)) {
        if (DeviceInit != NULL) {
            WdfDeviceInitFree(DeviceInit);
        }
    }

    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitializeSecondaryICInterface (
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine will register the hub module with the HAL as a secondary
    interrupt controller type device and supply the VIRQ range and its
    interface. HAL will then invoke the hub module using the supplied interface
    when a driver tries to connect/disconnect interrupt in the supplied VIRQ
    range.

Arguments:

    DriverObject - Supplies a pointer to the GPIO class extension's driver
        object.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    //
    // Query for the secondary interrupt information from the HAL. This will
    // include GSIV ranges carved out by the HAL for secondary interrupts.
    //

    Status = GpioHubpQuerySecondaryInterruptInformation();
    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "%s: Failed to query secondary interrupt information from"
                    "the HAL! Status = %#x\n",
                    __FUNCTION__,
                    Status);

        goto InitializeEnd;
    }

    //
    // Check the VIRQ range assigned to GPIO controllers on this platform. This
    // is the range that will be managed by the GPIO hub. It is possible that
    // the platform desires to only utilize the GPIO IO services and not the
    // interrupt services. If so, then skip registering the hub as a secondary
    // interrupt provider with the HAL.
    //

    if (GpioHalSecondaryInformation.SecondaryGsivSize == 0) {
        goto InitializeEnd;
    }

    Status = GpioHubpRegisterSecondaryInterruptInterface(
                GpioHalSecondaryInformation.SecondaryGsivBase,
                GpioHalSecondaryInformation.SecondaryGsivSize,
                DriverObject);

InitializeEnd:
    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpQuerySecondaryInterruptInformation (
    VOID
    )

/*++

Routine Description:

    This routine queries the secondary GSIV range information from the HAL.

Arguments:

    GsivBase - Supplies a pointer to a variable that receives the secondary
        GSIV range start value.

    GsivSize - Supplies a pointer to a variable that receives the secondary
        GSIV range size.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG Length;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Query the secondary GSIV range information from the HAL.
    //

    Length = sizeof(HAL_SECONDARY_INTERRUPT_INFORMATION);
    RtlZeroMemory(&GpioHalSecondaryInformation, Length);
    Status = HalQuerySystemInformation(HalSecondaryInterruptInformation,
                                       Length,
                                       &GpioHalSecondaryInformation,
                                       &Length);

    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpRegisterSecondaryInterruptInterface (
    __in ULONG GsivBase,
    __in ULONG GsivSize,
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine registers the GPIO hub interface with the HAL.

Arguments:

    GsivBase - Supplies the secondary GSIV range start value.

    GsivSize - Supplies the secondary GSIV range size.

    DriverObject - Supplies the driver object for GPIO class extension.

Return Value:

    NTSTATUS code.

--*/

{

    SECONDARY_INTERRUPT_PROVIDER_INTERFACE Interface;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Initialize the interface to be supplied to the HAL. The interface is
    // only partially initialized here. It will completely filled when the
    // framework invokes our query interface callback (when such a the request
    // arrives).
    //

    RtlZeroMemory(&Interface, sizeof(SECONDARY_INTERRUPT_PROVIDER_INTERFACE));
    Interface.Size = sizeof(Interface);
    Interface.Version = SECONDARY_INTERRUPT_PROVIDER_INTERFACE_VERSION;
    Interface.Context = NULL;
    Interface.GsivBase = GsivBase;
    Interface.GsivSize = (USHORT)GsivSize;
    Interface.DriverObject = DriverObject;
    Interface.EnableInterrupt = GpioHubEnableInterrupt;
    Interface.DisableInterrupt = GpioHubDisableInterrupt;
    Interface.MaskInterrupt = GpioHubMaskInterrupt;
    Interface.UnmaskInterrupt = GpioHubUnmaskInterrupt;
    Interface.QueryPrimaryInterrupt = GpioHubQueryPrimaryInterruptInformation;
    Interface.RequestInterrupt = GpioHubRequestInterrupt;
    Interface.QueryLineInformation = GpioHubQueryLineInformation;

    //
    // Register the interface directly with the HAL.
    //

    Status = HalSetSystemInformation(HalRegisterSecondaryInterruptInterface,
                                     sizeof(Interface),
                                     &Interface);

    return Status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioHubpUnregisterSecondaryInterruptInterface (
    __in ULONG GsivBase,
    __in ULONG GsivSize,
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine unregisters the GPIO hub interface with the HAL.

Arguments:

    GsivBase - Supplies the secondary GSIV range start value.

    GsivSize - Supplies the secondary GSIV range size.

    DriverObject - Supplies the driver object for GPIO class extension.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    if (GpioHalSecondaryInformation.UnregisterProvider != NULL) {
        GpioHalSecondaryInformation.UnregisterProvider(GsivBase,
                                                       GsivSize,
                                                       DriverObject);
    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpRegisterGpioDevice (
    __in PGPIO_HUB_REGISTRATION_DATA RegistrationData
    )

/*++

Routine Description:

    This routine will create a new GPIO controller instance object and insert
    it into the global hub list. This routine is called by the GPIO class
    extension when a client driver device starts.

Arguments:

    RegistrationData - Supplies the registration packet that contains
        information about the GPIO controller instance.

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Allocate and initialize an entry for the new GPIO instance.
    //

    GpioEntry = GPIO_ALLOCATE_POOL(NonPagedPoolNx, sizeof(GPIO_HUB_LIST_ENTRY));
    if (GpioEntry == NULL) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "GpioHubpRegisterGpioDevice: Failed to allocate memory for"
                    " new hub entry!\n");

        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto RegisterGpioDeviceEnd;
    }

    RtlZeroMemory(GpioEntry, sizeof(GPIO_HUB_LIST_ENTRY));
    GpioEntry->Context = RegistrationData->Context;
    GpioEntry->TargetName = RegistrationData->TargetName;
    GpioEntry->BiosName = RegistrationData->BiosName;

    KeInitializeEvent(&GpioEntry->Event,
                      SynchronizationEvent,
                      TRUE);

    //
    // Initialize all the list pointers.
    //

    InitializeListHead(&GpioEntry->ListEntry);
    InitializeListHead(&GpioEntry->SignalListEntry);
    InitializeListHead(&GpioEntry->ReplayListEntry);

    //
    // Add the new GPIO instance to the global hub lists.
    //

    GpioHubpAddGpioEntry(GpioEntry);
    Status = STATUS_SUCCESS;

RegisterGpioDeviceEnd:
    if (!NT_SUCCESS(Status) && (GpioEntry != NULL)) {
        GPIO_FREE_POOL(GpioEntry);
    }

    return Status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
__drv_sameIRQL
VOID
GpioHubpUninitialize (
    VOID
    )

/*++

Routine Description:

    This routine performs cleanup of the hub module.

    N.B. This routine is not implemented at present.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PDRIVER_OBJECT DriverObject;
    PGPIO_HUB_LIST_ENTRY GpioEntry;
    KIRQL Irql;
    LIST_ENTRY LocalGpioHubList;
    LIST_ENTRY LocalGpioHubVirqList;
    PLIST_ENTRY NextEntry;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    //
    // Unregister the GPIO hub interface from the HAL. This will prevent
    // any future requests from being dispatched to the GPIO hub.
    //

    DriverObject = WdfDriverWdmGetDriverObject(WdfGetDriver());
    GpioHubpUnregisterSecondaryInterruptInterface(
                GpioHalSecondaryInformation.SecondaryGsivBase,
                GpioHalSecondaryInformation.SecondaryGsivSize,
                DriverObject);

    //
    // Delete the resource hub target.
    //

    if (GpioHubResourceHubTarget != NULL) {
        WdfIoTargetClose(GpioHubResourceHubTarget);
        WdfObjectDelete(GpioHubResourceHubTarget);
        GpioHubResourceHubTarget = NULL;
    }

    //
    // N.B. 1. The signal DPC is parented to the hub device object. It will be
    //          automatically deleted by WDF when the device gets deleted.
    //
    //      2. PnP will automatically notify HAL about the hub device going
    //         away and the HAL will deregister the hub driver interface.
    //

    InitializeListHead(&LocalGpioHubList);
    InitializeListHead(&LocalGpioHubVirqList);

    //
    // Acquire the global hub lock while the hub lists are being deleted.
    //

    GpioHubpAcquireGlobalLock(&Irql);

    //
    // Move the global VIRQ and hub lists into local lists.
    //
    // N.B. AppendTailList() will insert the list head also onto the new list.
    //      Hence it must be removed prior to being reset.
    //

    if (IsListEmpty(&GpioHubList) == FALSE) {
        AppendTailList(&LocalGpioHubList, &GpioHubList);
        RemoveEntryList(&GpioHubList);
        InitializeListHead(&GpioHubList);
    }

    if (IsListEmpty(&GpioHubVirqList) == FALSE) {
        AppendTailList(&LocalGpioHubVirqList, &GpioHubVirqList);
        RemoveEntryList(&GpioHubVirqList);
        InitializeListHead(&GpioHubVirqList);
    }

    GpioHubpReleaseGlobalLock(Irql);

    //
    // Walk the local VIRQ list and free each entry on that list.
    //

    while (IsListEmpty(&LocalGpioHubVirqList) == FALSE) {
        NextEntry = RemoveHeadList(&LocalGpioHubVirqList);
        VirqEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_VIRQ_LIST_ENTRY,
                                      VirqListEntry);

        GPIO_FREE_POOL(VirqEntry);
    }

    //
    // Walk the local hub list and free each entry on that list.
    //

    while (IsListEmpty(&LocalGpioHubList) == FALSE) {
        NextEntry = RemoveHeadList(&LocalGpioHubList);
        GpioEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_LIST_ENTRY,
                                      ListEntry);

        GPIO_FREE_POOL(GpioEntry);
    }

    //
    // Delete the hub DPC object if it was created.
    //

    if (GpioHubDpc != NULL) {
        WdfObjectDelete(GpioHubDpc);
        GpioHubDpc = NULL;
    }

    GpioHubDevice = NULL;

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpUnregisterGpioDevice (
    __in PGPIO_HUB_REGISTRATION_DATA RegistrationData
    )

/*++

Routine Description:

    This routine will remove the specified GPIO instance from the global hub
    list (and other lists). Once removed, all future requests directed at the
    device will fail.

Arguments:

    RegistrationData - Supplies the registration packet that contains
        information about the GPIO controller instance.

Return Value:

    NTSTATUS code.

--*/


{

    PGPIO_HUB_LIST_ENTRY GpioEntry;

    PAGED_CODE();

    GpioEntry = GpioHubpFindGpioEntry(InputTypeBiosName,
                                      (ULONG_PTR)RegistrationData->BiosName,
                                      NULL);

    if (GpioEntry != NULL) {
        GpioHubpDeleteGpioEntry(GpioEntry);
    }

    return STATUS_SUCCESS;
}

//
// ----------------------------------------------------- Internal only routines
//

__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioHubpAddGpioEntry (
    __inout __drv_aliasesMem PGPIO_HUB_LIST_ENTRY GpioEntry
    )

/*++

Routine Description:

    This routine will insert the specified GPIO instance into the global hub
    list.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO controller entry.

Return Value:

    None.

--*/

{

    KIRQL Irql;

    //
    // Insert the entry at the right position into the global VIRQ and VIO
    // lists in sorted manner.
    //

    GpioHubpAcquireGlobalLock(&Irql);

    InsertTailList(&GpioHubList, &GpioEntry->ListEntry);

    GpioHubpReleaseGlobalLock(Irql);

    return;
}

_Must_inspect_result_
__drv_sameIRQL
__drv_requiresIRQL(HIGH_LEVEL)
PGPIO_HUB_VIRQ_LIST_ENTRY
GpioHubpFindVirqEntryLocked (
    __in ULONG Virq
    )

/*++

Routine Description:

    This routine will locate the entry for the specified VIRQ.

    N.B. This routine assumes either:

         a) The GPIO hub list lock is held

         OR

         b) The current IRQL is HIGH_LEVEL and all other CPUs are asleep.

Arguments:

    Virq - Supplies the VIRQ of the entry to be located.

Return Value:

    Pointer to the VIRQ entry, if found.
    NULL otherwise.

--*/

{
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;
    PLIST_ENTRY NextEntry;

    GPIO_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);

    NextEntry = GpioHubVirqList.Flink;
    while (NextEntry != &GpioHubVirqList) {
        VirqEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_VIRQ_LIST_ENTRY,
                                      VirqListEntry);

        if (VirqEntry->Virq == Virq) {
            return VirqEntry;
        }

        NextEntry = NextEntry->Flink;
    }

    return NULL;
}

_Must_inspect_result_
__drv_sameIRQL
__drv_maxIRQL(HIGH_LEVEL)
PGPIO_HUB_LIST_ENTRY
GpioHubpFindGpioEntry (
    __in __drv_strictTypeMatch(__drv_typeConst) INPUT_VALUE_TYPE ValueType,
    __in ULONG_PTR Value,
    __out_opt PGPIO_HUB_VIRQ_LIST_ENTRY *VirqEntryOut
    )

/*++

Routine Description:

    This routine will locate the GPIO instance managing the specified VIRQ or
    having the specified BIOS name in the global hub list.

    This routine will acquire a shared reference on the returned instance.

    N.B. This routine could be called at DIRQL (for mask/unmask requests).

Arguments:

    ValueType - Supplies the type of the range information (VIRQ or VIO).

    Value - Specifies the VIRQ or VIO value to search on.

    VirqEntryOut - Supplies a pointer that receives the VIRQ entry or NULL if
                   not found.

                   This is only set if ValueType == InputTypeVirq.
                   For InputTypeBiosName, it is set to NULL.

                   N.B. This routine makes no guarantees about the lifetime of
                        the returned VIRQ entry. It could be deleted by the
                        time the caller tries to access it!

Return Value:

    Pointer to the GPIO controller instance entry within the list, if found.
    NULL otherwise.

--*/

{

    PUNICODE_STRING BiosName;
    PGPIO_HUB_LIST_ENTRY GpioEntry;
    KIRQL Irql;
    BOOLEAN MatchFound;
    PGPIO_HUB_LIST_ENTRY MatchingEntry;
    PLIST_ENTRY NextEntry;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    MatchingEntry = NULL;

    if (ARGUMENT_PRESENT(VirqEntryOut)) {
        *VirqEntryOut = NULL;
    }

    //
    // Walk through the entries in the appropriate hub list looking for an
    // entry that encompasses the supplied VIRQ (InputTypeVirq) or matches the
    // supplied name (InputTypeBiosName).
    //

    GpioHubpAcquireGlobalLock(&Irql);

    switch (ValueType) {

    case InputTypeVirq:

        GPIO_ASSERT(Value == (ULONG)Value);

        VirqEntry = GpioHubpFindVirqEntryLocked((ULONG)Value);
        if (VirqEntry != NULL) {
            MatchingEntry = VirqEntry->GpioEntry;

            GPIO_ASSERT(MatchingEntry != NULL);

        }

        if (ARGUMENT_PRESENT(VirqEntryOut)) {
            *VirqEntryOut = VirqEntry;
        }

        break;

    case InputTypeBiosName:
        BiosName = (PUNICODE_STRING)Value;
        NextEntry = GpioHubList.Flink;
        while (NextEntry != &GpioHubList) {
            GpioEntry = CONTAINING_RECORD(NextEntry,
                                          GPIO_HUB_LIST_ENTRY,
                                          ListEntry);

            if (GpioEntry->BiosName->Length == BiosName->Length) {
                MatchFound = RtlEqualMemory(GpioEntry->BiosName->Buffer,
                                            BiosName->Buffer,
                                            BiosName->Length);

            } else {
                MatchFound = FALSE;
            }

            if (MatchFound != FALSE) {
                MatchingEntry = GpioEntry;
                break;
            }

            NextEntry = NextEntry->Flink;
        }

        break;
    }

    //
    // Acquire a reference to the entry so that the caller can safely use it.
    //

    if (MatchingEntry != NULL) {
        GpioHubpAcquireGpioEntryShared(MatchingEntry);
    }

    GpioHubpReleaseGlobalLock(Irql);

    if (MatchingEntry == NULL) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INTERRUPT,
                    "GpioHubpFindGpioEntry: Failed to find GPIO entry! Type = %d, "
                    "Value = %#I64d\n",
                    ValueType,
                    (ULONG64)Value);
    }

    return MatchingEntry;
}

_Must_inspect_result_
__drv_sameIRQL
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpCreateVirqMapping (
    __in ULONG Virq,
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __out_opt PGPIO_HUB_VIRQ_LIST_ENTRY *VirqEntryOut
    )

/*++

Routine Description:

    This routine will create an entry, for the specified VIRQ, in the global
    VIRQ list.

    N.B. This routine assumes that it is not called concurrently with itself.

Arguments:

    Virq - Specifies the VIRQ to create the entry for.

    GpioEntry - Supplies a pointer to the parent GPIO controller entry.

    VirqEntryOut - Supplies a pointer that receives the created VIRQ entry.

                   N.B. This routine makes no guarantees about the lifetime of
                        the returned VIRQ entry. It could be deleted by the
                        time the caller tries to access it!

Return Value:

    NTSTATUS code.

--*/

{

    PGPIO_HUB_VIRQ_LIST_ENTRY HubEntry;
    BOOLEAN Inserted;
    KIRQL Irql;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    //
    // First check if an entry for the VIRQ already exists. If so, just update
    // the VIRQ mapping.
    //

    Inserted = FALSE;

    GpioHubpAcquireGlobalLock(&Irql);

    NextEntry = GpioHubVirqList.Flink;
    while (NextEntry != &GpioHubVirqList) {
        HubEntry = CONTAINING_RECORD(NextEntry,
                                     GPIO_HUB_VIRQ_LIST_ENTRY,
                                     VirqListEntry);

        if (HubEntry->Virq == Virq) {
            HubEntry->GpioEntry = GpioEntry;
            if (ARGUMENT_PRESENT(VirqEntryOut)) {
                *VirqEntryOut = HubEntry;
            }

            Inserted = TRUE;
            break;
        }

        NextEntry = NextEntry->Flink;
    }

    GpioHubpReleaseGlobalLock(Irql);

    if (Inserted != FALSE) {
        Status = STATUS_SUCCESS;
        goto CreateVirqMappingEnd;
    }

    //
    // If an entry for the VIRQ does not already exist, then create a new one
    // and add it to the list.
    //

    VirqEntry = GPIO_ALLOCATE_POOL(NonPagedPoolNx,
                                   sizeof(GPIO_HUB_VIRQ_LIST_ENTRY));

    if (VirqEntry == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto CreateVirqMappingEnd;
    }

    RtlZeroMemory(VirqEntry, sizeof(GPIO_HUB_VIRQ_LIST_ENTRY));
    VirqEntry->GpioEntry = GpioEntry;
    VirqEntry->Virq = Virq;
    GpioClxInitializeHubVirqEntryCache(&VirqEntry->Cache);

    //
    // Insert the entry into the global VIRQ list in sorted manner.
    //

    GpioHubpAcquireGlobalLock(&Irql);

    NextEntry = GpioHubVirqList.Flink;
    while (NextEntry != &GpioHubVirqList) {
        HubEntry = CONTAINING_RECORD(NextEntry,
                                     GPIO_HUB_VIRQ_LIST_ENTRY,
                                     VirqListEntry);

        //
        // If the current entry's VIRQ is greater than that of the new entry,
        // then insert the new entry before the current entry.
        //

        if (HubEntry->Virq > Virq) {
            InsertTailList(NextEntry, &VirqEntry->VirqListEntry);
            if (ARGUMENT_PRESENT(VirqEntryOut)) {
                *VirqEntryOut = VirqEntry;
            }

            Inserted = TRUE;
            break;
        }

        NextEntry = NextEntry->Flink;
    }

    //
    // The new entry's VIRQ exceeds all the existing ones. Insert the new entry
    // entry at the end of the list.
    //

    if (Inserted == FALSE) {
        InsertTailList(&GpioHubVirqList, &VirqEntry->VirqListEntry);
        if (ARGUMENT_PRESENT(VirqEntryOut)) {
            *VirqEntryOut = VirqEntry;
        }
    }

    GpioHubpReleaseGlobalLock(Irql);

    Status = STATUS_SUCCESS;

CreateVirqMappingEnd:
    return Status;
}

__drv_sameIRQL
VOID
GpioHubpRemoveVirqListEntry (
    __in BOOLEAN ValueIsVirq,
    __in ULONG_PTR Value
    )

/*++

Routine Description:

    This routine removes an entry from the VIRQ list matching the supplied
    value (either VIRQ or reference to the master list entry).

Arguments:

    ValueIsVirq - Supplies whether the given value is VIRQ (True) or reference
        to the master list entry (False).

    Value - Supplies either a VIRQ or a reference to the master list entry.

Return Value:

    None.

--*/


{

    LIST_ENTRY DeletedList;
    PLIST_ENTRY NextEntry;
    KIRQL OldIrql;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    GpioHubpAcquireGlobalLock(&OldIrql);

    //
    // Remove the entry from the VIRQ list so that it cannot be used for any
    // future operations.
    //

    GpioHubpRemoveVirqListEntryListLocked(ValueIsVirq, Value, &DeletedList);

    GpioHubpReleaseGlobalLock(OldIrql);

    //
    // Remove the entries from the deleted list. There should only be one
    // entry in the list.
    //

    if (IsListEmpty(&DeletedList) == FALSE) {
        NextEntry = RemoveHeadList(&DeletedList);
        VirqEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_VIRQ_LIST_ENTRY,
                                      VirqListEntry);

        GPIO_FREE_POOL(VirqEntry);
    }

    GPIO_ASSERT(IsListEmpty(&DeletedList) != FALSE);

    return;
}

__drv_requiresIRQL(HIGH_LEVEL)
VOID
GpioHubpRemoveVirqListEntryListLocked (
    __in BOOLEAN ValueIsVirq,
    __in ULONG_PTR Value,
    __out PLIST_ENTRY DeletedListHead
    )

/*++

Routine Description:

    This routine removes the VIRQ list entry matching the supplied VIRQ.

    N.B. This routine assumes the GPIO hub list lock is held.

Arguments:

    Virq - Supplies the VIRQ for the entry to be removed.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    PLIST_ENTRY NextEntry;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;

    GPIO_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);

    InitializeListHead(DeletedListHead);

    //
    // While the global hub lock is held exclusively, remove the references to
    // the entry from the VIRQ list.
    //

    NextEntry = GpioHubVirqList.Flink;
    while (NextEntry != &GpioHubVirqList) {
        VirqEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_VIRQ_LIST_ENTRY,
                                      VirqListEntry);

        CurrentEntry = NextEntry;
        NextEntry = NextEntry->Flink;

        //
        // If the current entry matches the supplied VIRQ, remove it from the
        // VIRQ list.
        //

        if (ValueIsVirq != FALSE) {
            if (VirqEntry->Virq == Value) {
                VirqEntry->GpioEntry = NULL;
                RemoveEntryList(CurrentEntry);
                InsertTailList(DeletedListHead, CurrentEntry);
                break;
            }

        //
        // If the current references the supplied GPIO entry, then remove it
        // from the VIRQ list and insert it on the deleted list.
        //

        } else {
            if (VirqEntry->GpioEntry == (PGPIO_HUB_LIST_ENTRY)Value) {
                RemoveEntryList(CurrentEntry);
                InsertTailList(DeletedListHead, CurrentEntry);
            }
        }
    }

    return;
}

__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpDeleteGpioEntry (
    __in __drv_freesMem(Mem) PGPIO_HUB_LIST_ENTRY GpioEntry
    )

/*++

Routine Description:

    This routine removes the supplied entry from the global hub list and the
    VIRQ list.

    N.B. This operation call will synchronously block until all active
         references to the entry are released. Blocking the caller is OK
         because the control is expected to reach here when the target driver
         is being unloaded during which time an asynchronous operation is not
         really necessary.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO hub list entry.

Return Value:

    NTSTATUS code.

--*/

{

    LIST_ENTRY DeletedListHead;
    KIRQL Irql;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;
    PGPIO_HUB_VIRQ_LIST_ENTRY VirqEntry;
    LONG WaiterCount;

    WaiterCount = InterlockedCompareExchange(
                      &GpioEntry->ExclusiveWaiterCount,
                      0,
                      0);

    //
    // If there are other delete operations pending, then fail the delete
    // request.
    //

    if (WaiterCount > 0) {
        Status = STATUS_UNSUCCESSFUL;
        goto DeleteGpioEntryEnd;
    }

    GpioHubpAcquireGpioEntryExclusive(GpioEntry, &Irql);

    //
    // Remove the entry from the global hub list.
    //

    RemoveEntryList(&GpioEntry->ListEntry);

    //
    // Remove references to this entry from the VIRQ list so that it cannot be
    // used for any future operations.
    //

    GpioHubpRemoveVirqListEntryListLocked(FALSE,
                                          (ULONG_PTR)GpioEntry,
                                          &DeletedListHead);

    GpioHubpReleaseGpioEntryExclusive(GpioEntry, Irql);

    //
    // Delete the specified entry.
    //

    GPIO_FREE_POOL(GpioEntry);

    //
    // Remove the entries from VIRQ list that have been now obsoleted.
    //

    while (IsListEmpty(&DeletedListHead) == FALSE) {
        NextEntry = RemoveHeadList(&DeletedListHead);
        VirqEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_VIRQ_LIST_ENTRY,
                                      VirqListEntry);

        GPIO_FREE_POOL(VirqEntry);
    }

    Status = STATUS_SUCCESS;

DeleteGpioEntryEnd:
    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpMapGsivToControllerName (
    __in ULONG Gsiv,
    __out PUNICODE_STRING BiosName
    )

{

    PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER RhBiosDescriptor;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Open a handle to the Resource hub (i.e., create a resource hub target).
    // if not already opened.
    //
    // N.B. The handle, if successfully opened, will be deleted when the
    //      hub device is deleted.
    //

    RhBiosDescriptor = NULL;
    Status = GpiopQueryResourceHubBiosDescriptor(GpioHubDevice,
                                                 &GpioHubResourceHubTarget,
                                                 TRUE,
                                                 Gsiv,
                                                 &RhBiosDescriptor);

    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INTERRUPT,
                    "GpioHubpMapGsivToControllerName: Failed to query BIOS "
                    "descriptor from the Resource Hub! Gsiv = %#x, "
                    "Status = %#x\n",
                    Gsiv,
                    Status);

        goto MapGsivToControllerNameEnd;
    }

    Status = GpioUtilGetResourceName(RhBiosDescriptor, BiosName);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INTERRUPT,
                    "GpioHubpMapGsivToControllerName: Failed to extract BIOS "
                    "name from descriptor! Gsiv = %#x, Status = %#x\n",
                    Gsiv,
                    Status);

        goto MapGsivToControllerNameEnd;
    }

MapGsivToControllerNameEnd:
    if (RhBiosDescriptor != NULL) {
        GPIO_FREE_POOL(RhBiosDescriptor);
    }

    return Status;
}

_Must_inspect_result_
BOOLEAN
GpioHubpIsSecondaryInterrupt (
    __in ULONG Virq
    )

/*++

Routine Description:

    This routine determines whether the given VIRQ is for a secondary interrupt
    or not.

    N.B. This routine can be called at DIRQL level.

Arguments:

    Virq - Supplies the VIRQ to be checked.

Return Value:

    TRUE if the interrupt is secondary; FALSE otherwise.

--*/

{

    if ((Virq >= GpioHalSecondaryInformation.SecondaryGsivBase) &&
        (Virq < (GpioHalSecondaryInformation.SecondaryGsivBase +
                    GpioHalSecondaryInformation.SecondaryGsivSize))) {

        return TRUE;

    } else {
        return FALSE;
    }
}

VOID
GpioHubpInsertSignalList (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry
    )

/*++

Routine Description:

    This routine inserts the specified entry into the signal list.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO entry.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    //
    // Acquire the signal/replay list lock prior to manipulating the list.
    //

    GpioHubpAcquireSignalReplayListLock(&OldIrql);

    //
    // Insert the entry to be signalled to the end of the signal list.
    //

    InsertTailList(&GpioHubSignalList, &GpioEntry->SignalListEntry);

    //
    // If the signal/replay DPC is not already running, then queue it.
    //

    if (GpioHubDpcRunning == FALSE) {
        GpioHubDpcRunning = TRUE;
        WdfDpcEnqueue(GpioHubDpc);
    }

    //
    // Drop the signal/replay list lock.
    //

    GpioHubpReleaseSignalReplayListLock(OldIrql);
    return;
}

VOID
GpioHubpInsertReplayList (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry
    )

/*++

Routine Description:

    This routine inserts the specified entry into the interrupt replay list.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO entry.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    //
    // Acquire the signal/replay list lock prior to manipulating the list.
    //

    GpioHubpAcquireSignalReplayListLock(&OldIrql);

    //
    // Insert the entry to be replayed to the end of the replay list.
    //

    if (IsListEmpty(&GpioEntry->ReplayListEntry) != FALSE) {
        InsertTailList(&GpioHubInterruptReplayList,
                       &GpioEntry->ReplayListEntry);
    }

    //
    // If the signal/replay DPC is not already running, then queue it.
    //

    if (GpioHubDpcRunning == FALSE) {
        GpioHubDpcRunning = TRUE;
        WdfDpcEnqueue(GpioHubDpc);
    }

    //
    // Drop the signal/replay list lock.
    //

    GpioHubpReleaseSignalReplayListLock(OldIrql);
    return;
}

VOID
GpioHubpProcessSignalReplayList (
    __in WDFDPC Dpc
    )

/*++

Routine Description:

    This DPC routine processes the signal and interrupt replay lists and sets
    event within each entry on that list.

Arguments:

    Dpc - Supplies a pointer to the DPC object. This parameter is not used.

Return Value:

    None.

--*/

{

    PGPIO_HUB_LIST_ENTRY GpioEntry;
    PLIST_ENTRY NextEntry;
    KIRQL OldIrql;

    UNREFERENCED_PARAMETER(Dpc);

    //
    // Acquire the signal/replay list lock prior to manipulating the list.
    //

    GpioHubpAcquireSignalReplayListLock(&OldIrql);

    //
    // Walk the signal list and signal each event.
    //

    while (IsListEmpty(&GpioHubSignalList) == FALSE) {
        NextEntry = RemoveHeadList(&GpioHubSignalList);

        //
        // Drop the signal/replay list lock prior to signalling the event.
        //

        GpioHubpReleaseSignalReplayListLock(OldIrql);

        GpioEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_LIST_ENTRY,
                                      SignalListEntry);


        InitializeListHead(&GpioEntry->SignalListEntry);

        //
        // Signal the event indicating that the GPIO IC entry is
        // *potentially* available for exclusive use.
        //

        KeSetEvent(&GpioEntry->Event, IO_NO_INCREMENT, FALSE);

        //
        // Re-acquire the signal/replay list lock prior to manipulating the
        // list.
        //

        GpioHubpAcquireSignalReplayListLock(&OldIrql);
    }

    //
    // Walk the replay list and call the interrupt service routines.
    //

    while (IsListEmpty(&GpioHubInterruptReplayList) == FALSE) {
        NextEntry = RemoveHeadList(&GpioHubInterruptReplayList);

        //
        // Drop the signal/replay list lock prior to calling the interrupt
        // service routine.
        //

        GpioHubpReleaseSignalReplayListLock(OldIrql);

        GpioEntry = CONTAINING_RECORD(NextEntry,
                                      GPIO_HUB_LIST_ENTRY,
                                      ReplayListEntry);

        InitializeListHead(&GpioEntry->ReplayListEntry);

        //
        // Call the interrupt service routine associated with this synthetic
        // test GPIO controller.
        //

        GpioClxEvtVirtualRootIsr(GpioEntry->Context);

        //
        // Re-acquire the signal/replay list lock prior to manipulating the
        // list.
        //

        GpioHubpAcquireSignalReplayListLock(&OldIrql);
    }

    GpioHubDpcRunning = FALSE;

    //
    // Drop the signal/replay list lock.
    //

    GpioHubpReleaseSignalReplayListLock(OldIrql);
    return;
}

//
// -------------------------------------------------- Emergency worker routines
//

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
GpioHubQueueEmergencyWorkItem (
    __in PWORK_QUEUE_ITEM WorkItem
    )

/*++

Routine Description:

    This routine inserts the given workitem into the emergency work queue.
    Note this routine does not take a reference on the workitem. It is the
    caller's responsibility to do, if necessary.

    N.B. This routine can be entered at DISPATCH_LEVEL.

Arguments:

    Workitem - Supplies the workitem to be inserted.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    //
    // Insert the work item into the global emergency work queue.
    //

    GpioHubAcquireEmergencyQueueLock(&OldIrql);

    InsertTailList(&GpioHubEmergencyWorkQueue, &WorkItem->List);

    GpioHubReleaseEmergencyQueueLock(OldIrql);

    //
    // Signal an event to signal pending work to the emergency worker.
    //

    KeSetEvent(&GpioHubEmergencyWorkEvent, IO_NO_INCREMENT, FALSE);
    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioHubpInitializeEmergencyServices (
    VOID
    )

/*++

Routine Description:

    This routine performs initialization of the emergency services. It basically
    creates the emergency service thread.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{

    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS Status;
    PETHREAD Thread;
    HANDLE ThreadHandle;

    PAGED_CODE();

    //
    // Initialize the emergency work queue and associated event objects.
    //

    KeInitializeEvent(&GpioHubEmergencyWorkEvent, NotificationEvent, FALSE);
    KeInitializeEvent(&GpioHubEmergencyTerminateEvent,
                      NotificationEvent,
                      FALSE);

    InitializeListHead(&GpioHubEmergencyWorkQueue);

    //
    // Create and initialize the emergency GPIO worker thread.  This is used by
    // the GPIO class extension to guarantee forward progress in scenarios
    // where all system threads are blocked (perhaps due to page-fault and
    // paging is not yet available; possible early on resume from S4).
    //

    ThreadHandle = NULL;
    InitializeObjectAttributes(&ObjectAttributes,
                               NULL,
                               OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL);

    Status = PsCreateSystemThread(&ThreadHandle,
                                  THREAD_ALL_ACCESS,
                                  &ObjectAttributes,
                                  NULL,
                                  NULL,
                                  GpioHubpEmergencyWorkerThread,
                                  NULL);

    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "%s: Failed to create emergency service thread! "
                    "Status = %#x\n",
                    __FUNCTION__,
                    Status);

        goto CreatePassiveInterruptEmergencyThreadEnd;
    }

    Status = ObReferenceObjectByHandle(ThreadHandle,
                                       THREAD_ALL_ACCESS,
                                       NULL,
                                       KernelMode,
                                       (PVOID *)&Thread,
                                       NULL);

    ZwClose(ThreadHandle);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "%s: Failed to reference emergency service thread! "
                    "Status = %#x\n",
                    __FUNCTION__,
                    Status);

        goto CreatePassiveInterruptEmergencyThreadEnd;
    }

    GpioHubEmergencyThread = Thread;

CreatePassiveInterruptEmergencyThreadEnd:
    return Status;
}

VOID
GpioHubpEmergencyWorkerThread (
    __in PVOID Context
    )

/*++

Routine Description:

    This routine continually waits for a new work request to the GPIO emergency
    worker. The emergency worker is used when system thread pool is not
    available to pick up new work request (or when forced into this mode
    via registry settings).

    N.B. This routine should not be marked as paged.

Arguments:

    Context - Supplies the pointer to the thread context.

Return Value:

    None.

--*/

{

#define MAXIMUM_EMERGENCY_WAIT_OBJECTS (STATUS_WAIT_2) // 0x2

    PLIST_ENTRY Entry;
    KIRQL OldIrql;
    LONG Signalled;
    NTSTATUS Status;
    static KWAIT_BLOCK WaitBlockArray[MAXIMUM_EMERGENCY_WAIT_OBJECTS];
    PVOID WaitObjects[MAXIMUM_EMERGENCY_WAIT_OBJECTS];
    PWORKER_THREAD_ROUTINE WorkerRoutine;
    PVOID WorkerParameter;
    PWORK_QUEUE_ITEM WorkItem;

    UNREFERENCED_PARAMETER(Context);

    //
    // Loop forever waiting for a work queue item, calling the processing
    // routine, and then waiting for another work queue item.
    //

    WaitObjects[STATUS_WAIT_0] = (PVOID)&GpioHubEmergencyWorkEvent;
    WaitObjects[STATUS_WAIT_1] = (PVOID)&GpioHubEmergencyTerminateEvent;

    do {

        //
        // By specifying a wait mode of KernelMode, the thread's kernel stack
        // is not swappable.
        //

        Status = KeWaitForMultipleObjects(MAXIMUM_EMERGENCY_WAIT_OBJECTS,
                                          &WaitObjects[0],
                                          WaitAny,
                                          Executive,
                                          KernelMode,
                                          FALSE,
                                          NULL,
                                          &WaitBlockArray[0]);

        //
        // Switch on the wait status.
        //

        switch (Status) {

        case STATUS_WAIT_1:

            //
            // Terminate the thread.
            //

            PsTerminateSystemThread (STATUS_SUCCESS);

            //
            // Free any allocated pool.
            //

            break;

        case STATUS_WAIT_0:

        default:
                break;
        }

        //
        // There is work to be done. Pull items out of the work queue until
        // the queue becomes empty again.
        //

        WHILE (TRUE) {

            GpioHubAcquireEmergencyQueueLock(&OldIrql);

            if (IsListEmpty(&GpioHubEmergencyWorkQueue)) {
                Entry = NULL;
            } else {
                Entry = RemoveHeadList(&GpioHubEmergencyWorkQueue);
            }

            GpioHubReleaseEmergencyQueueLock(OldIrql);

            //
            // If the queue is empty again, quit servicing work items and
            // go back to waiting for the worker queue event.
            //
            // Reset the event the signals work. If the event was signalled
            // at reset time, then new work *may* have arrived. Go and recheck
            // the list.
            //
            // N.B. Since the event can only be cleared or reset at DPC_LEVEL
            //      or below, it can't be cleared under the emergency queue
            //      lock (HIGH_LEVEL). The previous state will always be
            //      signalled when the list becomes empty, the list will be
            //      checked twice for emptiness. The second time around the
            //      previous state should be non-signalled if it is truly
            //      empty.
            //

            if (Entry == NULL) {
                Signalled = KeResetEvent(&GpioHubEmergencyWorkEvent);
                if (Signalled == 0) {
                    break;

                } else {
                    continue;
                }
            }

            //
            // Execute the work item.
            //

            WorkItem = CONTAINING_RECORD(Entry, WORK_QUEUE_ITEM, List);

            //
            // Now that the workitem is off the list, set the Flink to NULL.
            // This is required because if the workitem is later inserted into
            // system worker thread queue (say resources become free), it will
            // assume an already-inserted workitem is being reinserted and flag
            // corruption.
            //
            // N.B. The Blink is left as is for debuggability.
            //

            WorkItem->List.Flink = NULL;
            WorkerRoutine = WorkItem->WorkerRoutine;
            WorkerParameter = WorkItem->Parameter;
            (WorkItem->WorkerRoutine)(WorkItem->Parameter);

            //
            // Ensure the IRQL is passive-level on return from workitem.
            //

            GPIO_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        }

    } WHILE (TRUE);
}

//
// ------------------------------------------ Shared/Exclusive locking routines
//

__drv_setsIRQL(HIGH_LEVEL)
VOID
GpioHubpAcquireGpioEntryExclusive (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OriginalIrql
    )

/*++

Routine Description:

    This routine acquires ownership of the specified GPIO entry in the
    "exclusive" mode.

    N.B. 1. This routine assumes the caller has already acquired the entry
            in shared mode (otherwise the entry cannot be safely referenced).

         2. Only one exclusive caller at a time per-entry is supported.

         3. The call must have originated at PASSIVE IRQL.

         4. The global hub spin lock remains acquired on exit from this routine.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO hub list entry.

    OriginalIrql - Supplies a pointer to a variable that receives the IRQL
        on entry.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;
    NTSTATUS Status;
    ULONG Value;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    InterlockedIncrement(&GpioEntry->ExclusiveWaiterCount);

    //
    // Only one exclusive caller per-entry at a time is supported.
    //

    GPIO_ASSERT(GpioEntry->ExclusiveWaiterCount == 1);

    do {

        //
        // Wait while the GPIO entry is in use (i.e. the entry is
        // acquired either in shared or exclusive mode).
        //

        Status = KeWaitForSingleObject(&GpioEntry->Event,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL);

        NT_ASSERT(NT_SUCCESS(Status) != FALSE);

        //
        // The entry is now *potentially* free now. Acquire the global hub lock
        // before marking the entry as in use exclusively.
        //

        GpioHubpAcquireGlobalLock(&OldIrql);

        //
        // Check if the entry is still not in use. If not in use, then mark it
        // as in use. Hold onto the global hub spin lock to make the ownership
        // exclusive.
        //
        // N.B. The busy count is checked for 1 (instead of 0) because the
        //      caller is assumed to have acquired the entry in shared mode
        //      prior to invocation.
        //

        Value = InterlockedCompareExchange(&GpioEntry->BusyCount, 2, 1);
        if (Value == 1) {
            goto AcquireGpioEntryExclusiveEnd;
        }

        //
        // If the entry became in use while the global lock was being
        // acquired, then drop the lock and retry.
        //

        GpioHubpReleaseGlobalLock(OldIrql);

    } WHILE(TRUE);

AcquireGpioEntryExclusiveEnd:
    InterlockedDecrement(&GpioEntry->ExclusiveWaiterCount);
    *OriginalIrql = OldIrql;
    return;
}

FORCEINLINE
VOID
GpioHubpAcquireGpioEntryShared (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry
    )

/*++

Routine Description:

    This routine acquires ownership of the specified GPIO entry in the
    "shared" mode.

    N.B. This routine assumes the caller has held the global hub lock.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO hub list entry.

Return Value:

    None.

--*/

{

    //
    // Increment the busy count to indicate that this entry is now in use.
    //

    InterlockedIncrement(&GpioEntry->BusyCount);
    return;
}

__drv_setsIRQL(OldIrql)
VOID
GpioHubpReleaseGpioEntryExclusive (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    )

/*++

Routine Description:

    This routine releases the ownership of the specified GPIO entry in
    the "exclusive" mode.

    N.B. This routines assumes the original IRQL was DISPATCH_LEVEL or below.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO hub list entry.

    OldIrql - Supplies the original IRQL value.

Return Value:

    None.

--*/

{

    NT_ASSERT((KeGetCurrentIrql() >= OldIrql) && (OldIrql < DISPATCH_LEVEL));

    //
    // Drop the reference that was taken when entry was exclusively acquired.
    //

    GpioHubpReleaseGpioEntryShared(GpioEntry, FALSE);

    //
    // Drop the global hub lock that was taken when the entry was exclusively
    // acquired.
    //

    GpioHubpReleaseGlobalLock(OldIrql);

    //
    // Signal the event (for the next waiter) as it was not done when the
    // reference was dropped above.
    //
    // N.B. This is not strictly necessary at present as exclusive waiter is
    //      only done when a delete request arrives. This is done anyway
    //      to support some other request that may also require to acquire
    //      an entry exclusively.
    //

    KeSetEvent(&GpioEntry->Event, IO_NO_INCREMENT, FALSE);
    return;
}

VOID
GpioHubpReleaseGpioEntryShared (
    __in PGPIO_HUB_LIST_ENTRY GpioEntry,
    __in BOOLEAN SignalEvent
    )

/*++

Routine Description:

    This routine releases the "shared" ownership of the specified GPIO
    entry.

Arguments:

    GpioEntry - Supplies a pointer to the GPIO hub list entry.

    SignalEvent - Supplies a flag that indicates whether the notification
        event should be signalled (TRUE) or not (FALSE) in case there are
        exclusive waiters.

Return Value:

    None.

--*/

{

    KIRQL Irql;
    ULONG Value;
    ULONG WaiterCount;

    //
    // Decrement the busy count to indicate that this entry is not in use.
    //

    NT_ASSERT(GpioEntry->BusyCount >= 1);

    Value = InterlockedDecrement(&GpioEntry->BusyCount);

    //
    // If the entry has now become available and if there are exclusive
    // waiters, then set the notification event.
    //
    // N.B. The value is checked against 1 since the exclusive waiter, if any,
    //      needs to have acquired the entry in shared mode first.
    //

    if ((SignalEvent != FALSE) && (Value == 1)) {
        WaiterCount = InterlockedCompareExchange(
                          &GpioEntry->ExclusiveWaiterCount,
                          0,
                          0);

        //
        // Since an event can only be set at IRQL <= DISPATCH_LEVEL, queue a
        // DPC to set the event if IRQL is greater than DISPATCH_LEVEL.
        // Otherwise signal the event indicating that the GPIO IC entry is
        // *potentially* available for exclusive use.
        //

        if (WaiterCount > 0) {
            Irql = KeGetCurrentIrql();
            if (Irql > DISPATCH_LEVEL) {
                GpioHubpInsertSignalList(GpioEntry);

            } else {
                KeSetEvent(&GpioEntry->Event,
                           IO_NO_INCREMENT,
                           FALSE);
            }
        }
    }

    return;
}

//
// --------------------------------------------------- Utility locking routines
//

__drv_setsIRQL(HIGH_LEVEL)
FORCEINLINE
VOID
GpioHubpAcquireGlobalLock (
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OldIrql
    )

/*++

Routine Description:

    This routine acquires the global hub spin lock. This lock must be
    acquired before attempting to acquire the entry in either shared or
    exclusive mode.

Arguments:

    OldIrql - Supplies a pointer to a variable that receives the IRQL on entry.

Return Value:

    None.

--*/

{

    KeRaiseIrql(HIGH_LEVEL, OldIrql);
    KeAcquireSpinLockAtDpcLevel(&GpioHubListSpinLock);
    return;
}

__drv_setsIRQL(HIGH_LEVEL)
FORCEINLINE
VOID
GpioHubpAcquireSignalReplayListLock (
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OldIrql
    )

/*++

Routine Description:

    This routine acquires the global signal and interrupt replay list lock.

Arguments:

     OldIrql - Supplies a pointer to a variable that receives the IRQL on entry.

Return Value:

    None.

--*/

{

    KeRaiseIrql(HIGH_LEVEL, OldIrql);
    KeAcquireSpinLockAtDpcLevel(&GpioHubSignalReplayListLock);
    return;
}

__drv_setsIRQL(OldIrql)
FORCEINLINE
VOID
GpioHubpReleaseGlobalLock (
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    )

/*++

Routine Description:

    This routine releases the global hub spin lock.

Arguments:

    OldIrql - Supplies the original IRQL value.

Return Value:

    None.

--*/

{

    KeReleaseSpinLockFromDpcLevel(&GpioHubListSpinLock);
    KeLowerIrql(OldIrql);
    return;
}

__drv_setsIRQL(OldIrql)
FORCEINLINE
VOID
GpioHubpReleaseSignalReplayListLock (
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    )

/*++

Routine Description:

    This routine releases the global signal and interrupt replay list lock.

Arguments:

    OldIrql - Supplies the original IRQL value.

Return Value:

    None.

--*/

{

    KeReleaseSpinLockFromDpcLevel(&GpioHubSignalReplayListLock);
    KeLowerIrql(OldIrql);
    return;
}


