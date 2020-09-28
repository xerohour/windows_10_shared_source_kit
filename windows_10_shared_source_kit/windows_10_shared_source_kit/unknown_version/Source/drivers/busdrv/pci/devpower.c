/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    devpower.c

Abstract:

    This module contains device power management code for PCI.SYS.












--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "devpower.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciDevice_QueryPower(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciDevice_SetPower(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciDevice_WaitWake(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

__drv_functionClass(DRIVER_CANCEL)
_Requires_lock_held_(_Global_cancel_spin_lock_)
_Releases_lock_(_Global_cancel_spin_lock_)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
PciDeviceWaitWakeCancel(
    _Inout_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ _IRQL_uses_cancel_ struct _IRP *Irp
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciPowerDownDevice(
    _Inout_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE PowerState,
    _In_ POWER_ACTION PowerAction,
    _In_ PIRP DIrp
    );

VOID
ExpressUpdateWakeControl (
    __in PPCI_DEVICE Device,
    __in BOOLEAN ArmedForWake
    );

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciStallForPowerChange(
    __in PPCI_DEVICE Device,
    __in DEVICE_POWER_STATE NewPowerState
    );

VOID
PciEnablePme(
    __in PPCI_DEVICE Device
    );

VOID
PciDisablePme(
    __in PPCI_DEVICE Device
    );

VOID
PciAdjustPmeEnable(
    __in PPCI_DEVICE Device,
    __in BOOLEAN Enable,
    __in BOOLEAN ClearStatusOnly
    );

VOID
PciGetPmeInformation(
    __in PPCI_DEVICE Device,
    __out_opt PBOOLEAN PmeCapable,
    __out_opt PBOOLEAN PmeStatus,
    __out_opt PBOOLEAN PmeEnable
    );

BOOLEAN
PciDeviceSupportsNativeExpressPme (
    _In_ PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciPmeInterface_Constructor;

VOID
PciPmeInterface_Reference(
    __inout PVOID Context
    );

VOID
PciPmeInterface_Dereference(
    __inout PVOID Context
    );

VOID
PciPmeInterface_UpdateEnable(
    __in PDEVICE_OBJECT Pdo,
    __in BOOLEAN PmeEnable
    );

VOID
PciPmeInterface_ClearPmeStatus(
    __in PDEVICE_OBJECT Pdo
    );

VOID
PciPmeInterface_GetInformation(
    __in PDEVICE_OBJECT Pdo,
    __out_opt PBOOLEAN PmeCapable,
    __out_opt PBOOLEAN PmeStatus,
    __out_opt PBOOLEAN PmeEnable
    );

VOID
PciPmeInterface_GetPmeControl (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PBOOLEAN PmeControl
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciD3ColdSupportInterface_Constructor;

VOID
PciD3ColdSupportInterface_Reference(
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    );

VOID
PciD3ColdSupportInterface_Dereference(
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    );

SET_D3COLD_SUPPORT PciD3ColdSupportInterface_SetD3ColdSupport;
GET_IDLE_WAKE_INFO PciD3ColdSupportInterface_GetIdleWakeInfo;
GET_D3COLD_CAPABILITY PciD3ColdSupportInterface_GetD3ColdCapability;
GET_D3COLD_CAPABILITY PciD3ColdSupportInterface_GetD3ColdBusDriverSupport;
GET_D3COLD_LAST_TRANSITION_STATUS PciD3ColdSupportInterface_GetLastTransitionStatus;

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciPmeInterface_Constructor)
    #pragma alloc_text(PAGE, PciD3ColdSupportInterface_Constructor)
    #pragma alloc_text(PAGE, PciD3ColdSupportInterface_GetIdleWakeInfo)
    #pragma alloc_text(PAGE, PciD3ColdSupportInterface_GetD3ColdCapability)
    #pragma alloc_text(PAGE, PciD3ColdSupportInterface_GetD3ColdBusDriverSupport)
    #pragma alloc_text(PAGE, PciSetHardwarePowerStatePassive)
#endif

// --------------------------------------------------------------------- Globals

//
// This table is taken from the PCI spec.  The units are microseconds.
//
const LONG PciPowerDelayTable[4][4] = {
//  D0      D1      D2      D3(Hot)
    0,      0,      200,    10000,  // D0
    0,      0,      200,    10000,  // D1
    200,    200,    0,      10000,  // D2
    10000,  10000,  10000,  0       // D3(Hot)
};

const PCI_MN_DISPATCH_ENTRY PciDevice_DispatchPowerTable[PCI_MAX_MINOR_POWER_IRP + 2] = {
    { IRP_COMPLETE, PciDevice_WaitWake,   TRUE }, // 0x00 - IRP_MN_WAIT_WAKE
    { IRP_COMPLETE, PciIrpNotSupported,   FALSE}, // 0x01 - IRP_MN_POWER_SEQUENCE
    { IRP_COMPLETE, PciDevice_SetPower,   FALSE}, // 0x02 - IRP_MN_SET_POWER
    { IRP_COMPLETE, PciDevice_QueryPower, FALSE}, // 0x03 - IRP_MN_QUERY_POWER
    { IRP_COMPLETE, PciIrpNotSupported,   FALSE}, //      - UNHANDLED Power IRP
};

//
// Define the Pci PME interface "interface" structure
//
const PCI_INTERFACE PciPmeInterface = {
    &GUID_PCI_PME_INTERFACE,        // Interface Type
    sizeof(PCI_PME_INTERFACE),      // Mininum Size
    PCI_PME_INTRF_STANDARD_VER,     // Minimum Version
    PCI_PME_INTRF_STANDARD_VER,     // Maximum Version
    PCIIF_FDO | PCIIF_ROOT,         // Flags
    PciInterface_PmeHandler,        // Signature
    PciPmeInterface_Constructor     // Constructor
};

//
// Define the Pci D3Cold Support "interface" structure
//
const PCI_INTERFACE PciD3ColdSupportInterface = {
    &GUID_D3COLD_SUPPORT_INTERFACE,        // Interface Type
    sizeof(D3COLD_SUPPORT_INTERFACE),      // Mininum Size
    D3COLD_SUPPORT_INTERFACE_VERSION,      // Minimum Version
    D3COLD_SUPPORT_INTERFACE_VERSION,      // Maximum Version
    PCIIF_PDO,                             // Flags
    PciInterface_D3ColdSupport,            // Signature
    PciD3ColdSupportInterface_Constructor  // Constructor
};

//
// These are defined to control an express wake disable bit in the PM1 enable
// register in the FADT table.
//

ULONG ExpressDevicesEnabledForWake;
KSPIN_LOCK ExpressWakeControlLock;

extern BOOLEAN PciKSRInProgress;

// --------------------------------------------------- Query/Set Power Functions

NTSTATUS
PciDevice_QueryPower(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Handles QueryPower Irps send to a PCI PDO

    N.B. Always succeed the the query, even when
    the requested D-state is not actually supported.  Our capabilities
    should have prevented bogus queries from coming in, but there's
    significant logic that goes into determining the capabilities, so
    don't duplicate it here.

Arguments:

    Irp - The request

    IrpStack - The current stack location

    DeviceExtension - The device that is getting powered down

Return Value:

    NTSTATUS

--*/
{
    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(IrpSp);
    UNREFERENCED_PARAMETER(DeviceExtension);

    return STATUS_SUCCESS;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciDevice_SetPower(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Handles SetPower Irps sent to a PCI PDO.

    If the irp is an S-Irp, then do nothing.

    If the irp is an D-Irp, then put the device in the appropriate state.
    Powering down a device may not actually involve setting the device
    into a low power state, if the device can't be put into a low
    power state (hiber path, or VGA, for instance).
 
    Putting the device in the right state may involve putting the parent bus
    in the right state, which can't be guaranteed to occur synchronously.  So
    this function may pend the IRP and complete it on another thread.

Arguments:

    Irp - The request

    IrpStack - The current stack location

    DeviceExtension - The device that is getting powered down

Return Value:

    NTSTATUS

--*/
{
    DEVICE_POWER_STATE desiredDeviceState;
    PPCI_DEVICE device;
    KIRQL oldIrql;
    POWER_ACTION powerAction;
    BOOLEAN powerAvailable;
    NTSTATUS status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Irp);

    device = (PPCI_DEVICE)DeviceExtension;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    //
    // We only care about D-IRPs.
    //
    if (IrpSp->Parameters.Power.Type == SystemPowerState) {
        return STATUS_SUCCESS;
    }

    if (IrpSp->Parameters.Power.Type != DevicePowerState) {
        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p Invalid supplied power parameter",
                    device->Parent->SecondaryBusNumber,
                    device);
        return STATUS_NOT_SUPPORTED;
    }

    desiredDeviceState = IrpSp->Parameters.Power.State.DeviceState;
    powerAction = IrpSp->Parameters.Power.ShutdownType;

    if ((desiredDeviceState < PowerDeviceD0) || (desiredDeviceState > PowerDeviceD3)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the transition is a nop, return early.
    //
    if (desiredDeviceState == device->LogicalPowerState) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p nop",
                    device->Parent->SecondaryBusNumber,
                    device);
        return STATUS_SUCCESS;
    }

    //
    // If the parent isn't started, D-state changes are out of the question.
    //

    if (device->Parent->DeviceState != PciStarted) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "device %p tried to change power state while the parent "
                    "wasn't running, Device state is: %!PNP_STATE!.",
                    device,
                    device->Parent->DeviceState);

        return STATUS_NO_SUCH_DEVICE;
    }

    status = STATUS_UNSUCCESSFUL;

    switch (desiredDeviceState) {
    case PowerDeviceD0:

        //
        // If there's already a D0 IRP pending, running the code below again
        // would be fatal.  So fail this new one.
        //

        if (device->DIrp != NULL) {
            TraceEvents(device->Parent->BlackBoxHandle,
                        TRACE_LEVEL_ERROR,
                        DBG_DEVPOWER,
                        "device %p sent more than one D0 IRP at a time",
                        device);
            status = STATUS_TOO_MANY_COMMANDS;
            break;
        }

        //
        // Mark the IRP pending regardless of what is about to happen, as we
        // don't know whether it's going to be completed, queued or just delayed
        // until there's been power on the bus for long enough.
        //
        IoMarkIrpPending(Irp);

        //
        // Tell the wrapper code that it's no longer responsible for completing
        // this IRP.
        //

        status = STATUS_PENDING;

        KeAcquireSpinLock(&device->D0IrpLock, &oldIrql);
        device->DIrp = Irp;

        //
        // Mark the parent bus as needing power.  If it is already turned on,
        // then power will be available.
        //
        powerAvailable = PciBus_PowerReference(device->Parent, 
                                               PciBusPowerReasonChildNeedsD0);
        
        //
        // If power is not available, queue the IRP until it becomes available.
        //
        if (powerAvailable == FALSE) {
            PciBus_QueueChildD0Irp(device);
        }

        KeReleaseSpinLock(&device->D0IrpLock, oldIrql);

        if (powerAvailable != FALSE) {
            PciDevice_RetireD0Irp(device);
        }

        break;

    //
    // D1 and D2 are interesting.  We need bus power for those.  So we need
    // to reference the bus.  But we're also leaving D0, so we need to drop that
    // reference.  So first reference and then fall through to the power down.
    //

    case PowerDeviceD1:
        __fallthrough;
    case PowerDeviceD2:

        PciBus_PowerReference(device->Parent, PciBusPowerReasonChildNeedsD1or2);
        __fallthrough;
    case PowerDeviceD3:

        //
        // If we took an extra power reference because the device triggered
        // wake, drop that reference now.
        //

        KeAcquireSpinLock(&device->WakeIrpLock, &oldIrql);
        if (device->ExtraBusPowerReferenceForWake == 1) {
            PCI_DEVICE_CLEAR_FLAG(device, ExtraBusPowerReferenceForWake);
            PciBus_PowerDereference(device->Parent,
                                    PciBusPowerReasonChildWaitWakeComplete);
        }
        KeReleaseSpinLock(&device->WakeIrpLock, oldIrql);

        //
        // Actually turn off the device.
        //

        status = PciPowerDownDevice(device, desiredDeviceState, powerAction, Irp);

        //
        // Drop the bus power reference that keeps the bus on while the child
        // device is on.
        //

        PciBus_PowerDereference(device->Parent, PciBusPowerReasonChildNeedsD0);
        break;
    }

    return status;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciDevice_RetireD0Irp(
    _Inout_ PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine completes the device D0 transition when the parent bus reaches
    D0.

Arguments:

    DeviceExtension - Pointer to the device extension for a PCI device.

    CompleteIrp - Determines whether this routine should complete the D0 IRP.
                  If this parameter is FALSE, then the caller must call
                  IoCompleteRequest to complete the D0 IRP for this device.

Return Value:

    none

--*/
{
    LARGE_INTEGER waitPeriod;

    NON_PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x): releasing D0 IRP for device %p",
                Device->Parent->SecondaryBusNumber,
                Device);

    //
    // Now we need to wait long enough that the bus timings are in-spec.
    //
    // Bus 0 devices are assumed to be internal and coherent by the time
    // the OS comes up.
    //

    waitPeriod = PciBus_WaitTimeForBusSettle(Device->Parent,
                                             Device->D0DelayInMilliseconds);

    //
    // If enough time has elapsed, invoke the change here.  If not, set a timer.
    //

    PciSetHardwarePowerStateReset(Device);

    if (waitPeriod.QuadPart == 0) {

        PciPowerUpDeviceTimerDpc(&Device->PowerTimerDpc, Device, NULL, NULL);

    } else {

        //
        // Make it a relative wait.
        //

        waitPeriod.QuadPart = (-1) * waitPeriod.QuadPart;

        KeSetTimer(&Device->PowerTimer,
                   waitPeriod,
                   &Device->PowerTimerDpc);
    }

    return;
}

__drv_functionClass(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciPowerUpDeviceTimerDpc (
    _In_ struct _KDPC *Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine completes the power down sequence after waiting a while for
    transactions to retire on the bus.  If the transactions never seem to retire
    after revisiting this a sufficient number of times, then we just move on,
    as there's little else to do.  The software contract on the IRPs that need
    to retire is that the system will hang if they don't.  Better to complete
    a system sleep state transition and hope the problem clears up than to
    just hang.
 
    Future owners might consider using Function-Level Reset (FLR) to try to
    unwedge the device.  This would semantically look like a surprise removal
    followed by a re-enumeration, which is beyond the scope of the current
    strategy.

Arguments:
 
    Dpc - unused 
 
    DeferredContext - Pointer to the device extension.
 
    SystemArgument1 - unused
 
    SystemArgument2 - unused

Return Value:

    none

--*/
{

    PPCI_DEVICE device = (PPCI_DEVICE)DeferredContext;
    PCI_DEVICE_ID_DATA idData;
    DEVICE_POWER_STATE previousDeviceState;
    LARGE_INTEGER waitPeriod;
    NTSTATUS status;
    KIRQL oldIrql;
    PIRP dIrp;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    dIrp = device->DIrp;
    previousDeviceState = device->LogicalPowerState;

  //if (device->Parent->TimeOfPowerUp.QuadPart != 0) {
  //    TraceEvents(device->Parent->BlackBoxHandle,
  //                TRACE_LEVEL_INFORMATION,
  //                DBG_DEVPOWER,
  //                "%p: power-on after %d ms",
  //                device,
  //                (ULONG)((KeQueryInterruptTime() - device->Parent->TimeOfPowerUp.QuadPart) / 10000));
  //}

    //
    // Verify the device is still the same as before (and that someone
    // hasn't removed/replaced it with something else)
    //

    status = PciReadDeviceIdData(device, &idData);
    if (!NT_SUCCESS(status) ||
        !PciIsSameDevice(device, &idData)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "%p: D0 transition failed because IDs didn't match on "
                    "power-up %04x:%04x:%02x:%04x:%04x. Status: %!STATUS!",
                    device,
                    idData.VendorID,
                    idData.DeviceID,
                    idData.RevisionID,
                    idData.SubVendorID,
                    idData.SubSystemID,
                    status);
        status = STATUS_NO_SUCH_DEVICE;
    }

    if (NT_SUCCESS(status)) {

        //
        // If this device triggered PME while it was in a low-power state,
        // dismiss that now.
        //

        if (device->TriggeredPme != 0) {
            PciDisablePme(device);
        }

        status = PciPowerUpDevice(device);

        //
        // If the stall couldn't happen just this moment, requeue this timer, so
        // that this function can be restarted after the appropriate time.
        //

        if (status == STATUS_CANT_WAIT) {

            waitPeriod.QuadPart = -10 * device->PowerStateChangeDelay;
            KeSetTimer(&device->PowerTimer,
                       waitPeriod,
                       &device->PowerTimerDpc);
            return;
        }
    }

    //
    // If the power-up operation failed, the device may have gone missing
    // or changed during the power transition.  Queue a re-enumeration to
    // check for changes.
    //

    if (!NT_SUCCESS(status)) {
        IoInvalidateDeviceRelations(device->Parent->PhysicalDeviceObject,
                                    BusRelations);

    } else {

        //
        // There are instances where the BIOS clears the PME enable bit on
        // a PCI device on resume from hibernate. Thus, if there is a Wait
        // Wake Irp pending on a device, then enable PME for it.
        //

        KeAcquireSpinLock(&device->WakeIrpLock, &oldIrql);
        if (device->WakeIrp != NULL) {
            PciEnablePme(device);
        }

        KeReleaseSpinLock(&device->WakeIrpLock, oldIrql);
    }

    device->DIrp = NULL;
    PciCompleteRequest(dIrp, status);

    // 
    // This D0 IRP might have been the result of a PME triggered by the
    // child device. Now, that we have completed the D0 IRP, we should
    // reset the flag.
    //

    PCI_DEVICE_CLEAR_FLAG(device, TriggeredPme);

    //
    // If the previous power state was D1 or D2, then we'll still have a 
    // power reference on the bus.  Remove that extra now.
    //

    if ((previousDeviceState == PowerDeviceD1) ||
        (previousDeviceState == PowerDeviceD2)) {
        PciBus_PowerDereference(device->Parent, PciBusPowerReasonChildNeedsD1or2);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciPowerDownDevice(
    _Inout_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE PowerState,
    _In_ POWER_ACTION PowerAction,
    _In_ PIRP DIrp
    )
/*++

Routine Description:

    This routine handles putting a device into a low power
    state because it has received a SET_POWER D-Irp.  The routine
    always *logically* places the device into the requested power
    state, but only physically powers down the device if it's allowed.
    Devices on the hibernate path on hibernates, or VGA devices,
    cannot be powered down.

Arguments:

    Device - The device to put into a low power state.

    PowerState - The power state to place the device into.

    PowerAction - The "action" that caused this transition (shutdown,
        hibernate, etc)
 
    DIrp - The device power IRP that triggered this power-down.

Return Value:

    NTSTATUS - STATUS_PENDING means that the IRP is now owned by the device
        and must be completed later.

--*/
{

    PCI_DISCONNECTION_POLICY policy;
    POWER_STATE state;
    LARGE_INTEGER timeout;
    PPCI_BUS bus;

    NON_PAGED_CODE();

    bus = Device->Parent;
    if (((bus->PciBridge != NULL) &&
         (bus->PciBridge->AvoidChildD1D2ForSld != 0)) ||
        ((Device->HackFlags & PCI_HACK_AVOID_D1D2_FOR_SLD) != 0)) {

        if ((PowerState == PowerDeviceD1) || (PowerState == PowerDeviceD2)) {
            return STATUS_POWER_STATE_INVALID;
        }
    }

    //
    // Notify the system the power state of the device is changing.
    //
    state.DeviceState = PowerState;
    PoSetPowerState(
        Device->DeviceObject,
        DevicePowerState,
        state
        );

    InterlockedIncrement(&Device->Parent->DevicesPoweredDown);

    //
    // If device is currently in D0 state, update the saved register
    // settings in case the FDO changed them since we looked at them.
    //
    if (Device->LogicalPowerState == PowerDeviceD0) {
        PciUpdateSaveRestoreData(Device, &Device->SavedRegisters);
        if (Device->ExpressPort != NULL) {
            ExpressSavePortRegisters(Device->ExpressPort);

        } else if (Device->ExpressCompatibilityPort != NULL) {
            ExpressCompatibilitySavePortRegisters(
                Device->ExpressCompatibilityPort);
        }
    }

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x): device %p going to %!WDM_DEVICE_POWER_STATE!",
                Device->Parent->SecondaryBusNumber,
                Device,
                PowerState);
    Device->LogicalPowerState = PowerState;

    //
    // Notify the HAL about this D-state transition if
    // the device is SVM enabled.
    //

    if ((Device->ExpressPort != NULL) &&
        (Device->ExpressPort->HalDevicePowerTransCallback != NULL)) {

            Device->ExpressPort->HalDevicePowerTransCallback(
                    Device->DeviceObject,
                    PowerState,
                    Device->ExpressPort->HalDevicePowerTransCallbackContext);
    }

    //
    // If the device can actually be put into a low power state
    // (VGA cards can't be, for instance), do so.
    //

    policy = PciCanDisableDeviceForPowerDown(Device, PowerState, PowerAction);
    if (policy != KeepDeviceActive) {

        PciDisableInterrupts(Device);
        PciDisconnectDevice(Device);

        //
        // If this is an Express port, then we need to make sure that
        // transactions are drained.  This can take some time.  At
        // PASSIVE_LEVEL, do it inline.  At DISPATCH_LEVEL, start a timer, if
        // necessary.  Both code paths do the same thing, polling and waiting.
        // But the DISPATCH_LEVEL callback does it by repeatedly setting a 
        // timer which triggers a DPC.  The DPC will eventually complete the
        // IRP, no matter whether the pending transaction bit clears or not.
        //

        if (Device->ExpressPort != NULL) {
            if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
                ExpressWaitForPendingTransactions(Device->ExpressPort);
            } else {

                if (ExpressCheckForPendingTransactions(Device->ExpressPort)) {
                    if (policy == PowerDownDevice) {
                        PCI_DEVICE_SET_FLAG(Device, TransactionTimerSetPower);
                    }
                    Device->DIrp = DIrp;
                    IoMarkIrpPending(DIrp);
                    Device->TransactionTimeoutPasses = 0;
                    timeout.QuadPart = -10 * EXPRESS_TRANSACTION_TIMEOUT;
                    KeSetTimer(&Device->TransactionTimer,
                               timeout,
                               &Device->TransactionTimerDpc);
                    return STATUS_PENDING;
                }
            }
        }

        if (policy == PowerDownDevice) {

            //
            // The return code isn't really interesting here, as the device
            // will accumulate the appropriate settle time while it's in Dx,
            // or while we're waiting for it to come back to D0.
            //

            PciSetHardwarePowerStateReset(Device);
            PciSetHardwarePowerStateDispatch(Device, PowerState);
        }
    }

    return STATUS_SUCCESS;
}

__drv_functionClass(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciPowerDownDeviceTimerDpc (
    _In_ struct _KDPC *Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine completes the power down sequence after waiting a while for
    transactions to retire on the bus.  If the transactions never seem to retire
    after revisiting this a sufficient number of times, then we just move on,
    as there's little else to do.  The software contract on the IRPs that need
    to retire is that the system will hang if they don't.  Better to complete
    a system sleep state transition and hope the problem clears up than to
    just hang.
 
    Future owners might consider using Function-Level Reset (FLR) to try to
    unwedge the device.  This would semantically look like a surprise removal
    followed by a re-enumeration, which is beyond the scope of the current
    strategy.

Arguments:
 
    Dpc - unused 
 
    DeferredContext - Pointer to the device extension.
 
    SystemArgument1 - unused
 
    SystemArgument2 - unused

Return Value:

    none

--*/
{

    PPCI_DEVICE device = (PPCI_DEVICE)DeferredContext;
    PIRP dIrp;
    LARGE_INTEGER timeout;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));
    PCI_ASSERT(device->ExpressPort != NULL);

    if ((device->TransactionTimeoutPasses < 10) &&
        ExpressCheckForPendingTransactions(device->ExpressPort)) {

        timeout.QuadPart = -10 * EXPRESS_TRANSACTION_TIMEOUT;
        device->TransactionTimeoutPasses++;

        KeSetTimer(&device->TransactionTimer,
                   timeout,
                   &device->TransactionTimerDpc);
    } else {
        if (device->TransactionTimerSetPower == 1) {
            PciSetHardwarePowerStateReset(device);
            PciSetHardwarePowerStateDispatch(device, device->LogicalPowerState);
            PCI_DEVICE_CLEAR_FLAG(device, TransactionTimerSetPower);
        }

        dIrp = device->DIrp;
        device->DIrp = NULL;
        PciCompleteRequest(dIrp, STATUS_SUCCESS);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciPowerUpDevice(
    _Inout_ PPCI_DEVICE Device
    )
/*++

Routine Description:

    Powers up a device in response to receiving a SET_POWER D-Irp.
    This routine first places the device physically into D0, and then
    restores the device state.
 












Arguments:

    Device - The device to power up.

Return Value:

    NTSTATUS
    STATUS_CANT_WAIT - Indicates that this function couldn't complete at
        DISPATCH_LEVEL and should be called again later.

--*/
{
    BOOLEAN lastDeviceToPowerUp;
    BOOLEAN native;
    POWER_STATE state;
    NTSTATUS status;

    NON_PAGED_CODE();

    status = PciSetHardwarePowerStateDispatch(Device, PowerDeviceD0);

    //
    // If this returns STATUS_CANT_WAIT, the caller is responsible for calling
    // this again after the appropriate time has elapsed.
    //

    if (status == STATUS_CANT_WAIT) {
        return status;
    }

    //
    // Now restore the device, unless it's the kind of device you
    // can never restore.
    //

    if (!(Device->HackFlags & PCI_HACK_ENUM_NO_RESOURCE)) {

        //
        // If this is an IDE controller then attempt to switch it to
        // native mode
        //

        if ((Device->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR) &&
            (Device->SubClass == PCI_SUBCLASS_MSC_IDE_CTLR)) {

            //
            // It is important that once we come back from a low power state
            // and configure the IDE controller, it's in the same mode (native vs.
            // compatible) as it was before it went into the low power state.
            // Otherwise, the device state is completely different.
            //
            // The third parameter of this call is whether or not to disable the I/O
            // decodes of the IDE controller.  Only do this if the IDE driver
            // has requested control of the I/O decode.
            //

            native = PciConfigureIdeController(
                        Device,
                        (BOOLEAN)Device->IdeInterfaceActive
                        );
            PCI_ASSERT(native == (BOOLEAN)(Device->IdeInNativeMode));
            UNREFERENCED_PARAMETER(native);
        }

        //
        // Update the hardware.  Raise IRQL and rendevous the processors to do
        // this if the device is critical to system operation and might be in
        // use while this operation is occurring.  Also do this for devices
        // marked as being on the debugger path.  Disabling the debugger around
        // the reprogramming operation is not sufficient to stop the debugger
        // device from being used, since devices that are marked as being on the
        // debug path are sometimes not actually debugger devices - headless
        // console redirection devices are marked in this way as well.
        //

        status = PciDeviceCallCritical(Device, 
                                       PciUpdateHardware, 
                                       NULL, 
                                       (Device->OnDebugPath != FALSE));

        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        status = PciProgramInterruptResource(Device);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState (
        Device->DeviceObject,
        DevicePowerState,
        state
        );

    Device->LogicalPowerState = PowerDeviceD0;

    lastDeviceToPowerUp = FALSE;
    if (InterlockedDecrement(&Device->Parent->DevicesPoweredDown) == 0) {
        lastDeviceToPowerUp = TRUE;
    }

    if ((Device->ExpressPort != NULL) &&
        (Device->ExpressPort->Parent != NULL)) {

        if (lastDeviceToPowerUp != FALSE) {
            ExpressBridgeRetrainLink(Device->ExpressPort->Parent);
        }
    }

    //
    // Notify the HAL about this D-state transition if
    // the device is SVM enabled.
    //

    if ((Device->ExpressPort != NULL) &&
        (Device->ExpressPort->HalDevicePowerTransCallback != NULL)) {

            Device->ExpressPort->HalDevicePowerTransCallback(
                    Device->DeviceObject,
                    PowerDeviceD0,
                    Device->ExpressPort->HalDevicePowerTransCallbackContext);
    }

    return STATUS_SUCCESS;
}

// --------------------------------------------------------- Wait/Wake Functions

NTSTATUS
PciDevice_WaitWake(
    __inout PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Handle IRP_MN_WAIT_WAKE for PCI PDOs.

    This operation is used to wait for the device to signal a wake event.
    By waiting for a wake signal from a device, its wake event is enabled
    so long as the System Power State is above the requested SystemWake
    state.   By not waiting for a wake signal from a device, its wake
    signal is not enabled.

    Synchronization - The following synchronization primitives are used
        in the Wait/Wake code.

        WakeIrpState - This is used to synchronize between multiple callers
            of Wait/Wake on the same stack, and between the completion and
            cancellation of the same IRP.  The dispatch routine places a
            device into the PciWakeIrpProcessing state, and any caller who
            cannot do this (because it has already been done) must exit
            immediately.  When it has enabled PME and stored the IRP, it
            places the device in the PciWakeIrpQueued state.  Both the
            completion and cancellation routines attempt to change the
            device's state away from PciWakeIrpQueued, but if they detect
            that the current state is not PciWakeIrpQueued (because another
            completion/cancellation occurred already), they must not actually
            perform the completion/cancellation.

            The WakeIrpState variable in the PCI_DEVICE extension is protected
            by InterlockedXxx calls.  The WakeIrpState variable in the PCI_BUS
            extension is protected by the bus's BusPowerLock, described below.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP.

Return Value:

    NT status.

    STATUS_INVALID_DEVICE_STATE, if the device is in the PowerD0 state or
        a state below which can support waking or if the SystemWake state
        is below a state which can be supported.

        A pending IRP_MN_WAIT_WAKE will complete with this error if the
        device's state is changed to be incompatible with the wake request.

    STATUS_DEVICE_BUSY, if the device already has a WAIT_WAKE request
        outstanding.  To change the SystemWake level the outstanding
        IRP must be canceled.

    STATUS_INVALID_DEVICE_REQUEST, if the device is not capable of
        signaling a wakeup.   In theory we should have gotten out
        before getting this far because DeviceWake woud be unspecified.

    STATUS_SUCCESS.  The device has signaled a WAKE event.

    STATUS_PENDING.  This is the expected return, the IRP will not
        complete until the wait is complete or cancelled.

--*/
{
    DEVICE_POWER_STATE devicePowerState;
    BOOLEAN pmeCapable;
    PPCI_DEVICE device;
    NTSTATUS status;
    KIRQL oldIrql;
    PPCI_BUS bus;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(IrpSp);

    device = (PPCI_DEVICE)DeviceExtension;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    bus = device->Parent;

    //
    // The docs say WAIT_WAKE is allowed only from a state < D0, and
    // only if current power state supports wakeup.
    //
    devicePowerState = device->LogicalPowerState;
    PCI_ASSERT(devicePowerState < PowerDeviceMaximum);

    if ((devicePowerState > device->WakeLevel) ||
        (device->WakeLevel == PowerDeviceUnspecified)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p %!WDM_DEVICE_POWER_STATE! not valid for wake."
                    "Device wake level is %!WDM_DEVICE_POWER_STATE!.",
                    device->Parent->SecondaryBusNumber,
                    device,
                    devicePowerState,
                    device->WakeLevel);
        return STATUS_INVALID_DEVICE_STATE;
    }

    //
    // Only allow Wake if the device is capable of PME.
    // In theory we should only be here for devices that are
    // PME capable based on our capabilities
    //
    PciGetPmeInformation(device, &pmeCapable, NULL, NULL);
    if (pmeCapable == FALSE) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p does not support PM",
                    device->Parent->SecondaryBusNumber,
                    device);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Check the parent bus.  If ACPI is handling wait/wake for the parent bus,
    // and the parent bus isn't wakeable, then this is going to fail.  Better
    // to fail early than to fail later.
    //

    if (!(PCI_BUS_USES_NATIVE_PME(device->Parent)) &&
        (device->Parent->IdleDState <= DeviceWakeDepthD0) &&
        (IoGetCurrentIrpStackLocation(Irp)->Parameters.WaitWake.PowerState == PowerSystemWorking)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p is on a bus that cannot support wake",
                    device->Parent->SecondaryBusNumber,
                    device);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Only one Wait/Wake IRP is allowed per stack.
    // Only proceed if the WakeIrpState is PciWakeIrpNone.
    //
    if (PciWakeIrpNone != InterlockedCompareExchange((volatile LONG*)&device->WakeIrpState,
                                                     PciWakeIrpProcessing,
                                                     PciWakeIrpNone)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x): device %p is already waiting",
                    device->Parent->SecondaryBusNumber,
                    device);
        return STATUS_DEVICE_BUSY;
    }

    //
    // Kick wake handling up to the bus code, which will decide what's necessary
    // to do in response.  Root buses are special, though.  They don't really
    // support "wake."  ACPI has to catch the wake IRP at every function on
    // the root bus.
    //

    if (!PCI_ROOT_BUS(bus)) {
        PciBus_ArmForWake(bus);
    }

    //
    // Now that everything has succeeded, enable PME and pend the IRP.
    //

    TraceEvents(device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x): device %p setting PMEEnable",
                device->Parent->SecondaryBusNumber,
                device);

    //
    // Set a cancel routine to allow the FDO of this stack to cancel
    // the wakeup if required, and pend the IRP.  It will be completed
    // when a wakeup event actually occurs.
    //

    KeAcquireSpinLock(&device->WakeIrpLock, &oldIrql);
    IoSetCancelRoutine(Irp, PciDeviceWaitWakeCancel);

    //
    // Check if the Device Wait Wake Irp is cancelled by any chance.
    //

    if ((Irp->Cancel != FALSE) &&
        (IoSetCancelRoutine(Irp, NULL) != NULL)) {

        //
        // The Wait Wake Irp is indeed cancelled and we have successfully
        // cleared the cancellation routine. Set an appropriate error code
        // so that the Irp is immediately completed on return from this
        // routine.
        //

        InterlockedExchange((volatile LONG*)&device->WakeIrpState, PciWakeIrpNone);
        status = STATUS_CANCELLED;

    } else {
        IoMarkIrpPending(Irp);
        PciEnablePme(device);
        device->WakeIrp = Irp;
        InterlockedExchange((volatile LONG*)&device->WakeIrpState, PciWakeIrpQueued);
        status = STATUS_PENDING;

        //
        // If a wait wake Irp is successfully queued on a device then update
        // the express wake control.
        //

        if (PciDeviceSupportsNativeExpressPme(device)) {
            ExpressUpdateWakeControl(device, TRUE);
        }
    }

    KeReleaseSpinLock(&device->WakeIrpLock, oldIrql);

    if (status == STATUS_CANCELLED) {
        if (!PCI_ROOT_BUS(bus)) {
            PciBus_DisarmForWake(bus);
        }
    }

    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDevice_WakeTriggered (
    _Inout_ PPCI_DEVICE Device,
    _In_ BOOLEAN SystemWake,
    _In_ NTSTATUS CompletionStatus
    )
/*++

Routine Description:

    This is the routine that the bus code invokes when something has triggered
    PME.  It's job is to complete the child Wait/Wake IRP and manage any other
    wake-related state.
 
    Earlier implementations checked to see if their devices were actually
    triggering PME before completing the W/W IRP.  Now that the bus itself may
    support idling in low-power states, it's not possible to deterministically
    know that a device has or hasn't actually triggered wake.  Furthermore,
    if the bus was in a low-power state (particularly if it was in D3cold) then
    we want the devices to move through D0 after the bus comes to D0, and
    completing their W/W IRPs will cause that.  If there was nothing to do, they
    should then put themselves back in Dx with a new W/W IRP sent.

Arguments:

    Device - The device context structure for a PCI PDO.

    SystemWake - Value propagated from the parent Wait/Wake IRP indicating
        whether this was the system wake source.
 
    CompletionStatus - This is the status from the parent wake IRP (or
        equivalent means.)  If the parent wake IRP failed, other than because
        it was canceled, then this status is used for the child wake IRP, mostly
        so that drivers can choose whether or not to turn right around and send
        a new wake IRP.
 
Return Value:

    none

--*/
{
    BOOLEAN ChildWakeCancel;
    BOOLEAN PmeStatus;
    KIRQL OldIrql;
    PIRP WakeIrp;

    NON_PAGED_CODE();

    PCI_DEVICE_SET_FLAG(Device, TriggeredPme);

    //
    // This callback routine can be invoked before the device Wait
    // Wake Irp is securely pended in the extension or at the same
    // time while executing the cancellation routine for this Irp.
    // In face of this, the use of InterlockedXxx routines while
    // updating the WakeIrpState guarantees the correct behavior.
    //

    if (PciWakeIrpQueued == InterlockedCompareExchange(
                                    (volatile LONG*)&Device->WakeIrpState,
                                    PciWakeIrpQueued,
                                    PciWakeIrpQueued)) {

        PciGetPmeInformation(Device, NULL, &PmeStatus, NULL);

        ChildWakeCancel = FALSE;
        KeAcquireSpinLock(&Device->WakeIrpLock, &OldIrql);
        if ((Device->WakeIrp == NULL) ||
            (IoSetCancelRoutine(Device->WakeIrp, NULL) == NULL)) {

            ChildWakeCancel = TRUE;
        }

        KeReleaseSpinLock(&Device->WakeIrpLock, OldIrql);

        if (ChildWakeCancel == FALSE) {

            if (PciWakeIrpQueued == InterlockedCompareExchange(
                                        (volatile LONG*)&Device->WakeIrpState,
                                        PciWakeIrpCompleted,
                                        PciWakeIrpQueued)) {

                TraceEvents(Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_INFORMATION,
                            DBG_DEVPOWER,
                            "(%x): completing IRP_MN_WAIT_WAKE for device %p - PMEStatus: %x,",
                            Device->Parent->SecondaryBusNumber,
                            Device,
                            PmeStatus);

                KeAcquireSpinLock(&Device->WakeIrpLock, &OldIrql);

                PCI_ASSERT(Device->WakeIrp != NULL);

                WakeIrp = Device->WakeIrp;
                Device->WakeIrp = NULL;
                InterlockedExchange((volatile LONG*)&Device->WakeIrpState, 
                                    PciWakeIrpNone);
                KeReleaseSpinLock(&Device->WakeIrpLock, OldIrql);

                //
                // If the device is on, clear the PME now.
                //

                if (Device->LogicalPowerState <= PowerDeviceD0) {
                    PciDisablePme(Device);
                }

                //
                // If this device was actually triggering a wake signal, and if
                // the system was asleep or the parent bus was marked as
                // responsible for waking the system, then tell PO that so that
                // users have some feedback about what happened.
                //

                if ((PmeStatus != FALSE) &&
                    ((SystemWake != FALSE) ||
                     (PciSystemPowerStateTransitionActive != FALSE))) {
                    PoSetSystemWake(WakeIrp);
                }

                //
                // When completing a wait/wake IRP to a child, we want the bus
                // to remain on until the child has seen the wait/wake IRP
                // and had a chance to respond to it.  If the bus switches off
                // in between completing this IRP and when the device moves to
                // D0, all wake-related state will probably be destroyed.  So
                // take an extra reference on the bus here which will be dropped
                // the next time that the device leaves D0.  Only do this once,
                // in case the child uses wait/wake IRPs without leaving D0.
                //

                KeAcquireSpinLock(&Device->WakeIrpLock, &OldIrql);
                if (Device->ExtraBusPowerReferenceForWake == 0) {
                    PciBus_PowerReference(Device->Parent, 
                                          PciBusPowerReasonChildWaitWakeComplete);
                    PCI_DEVICE_SET_FLAG(Device, ExtraBusPowerReferenceForWake);
                }
                KeReleaseSpinLock(&Device->WakeIrpLock, OldIrql);

                //
                // If this isn't a root PCI bus, the bus itself is armed
                // for wake.  Drop the reference on that.
                //

                if (!PCI_ROOT_BUS(Device->Parent)) {
                    PciBus_DisarmForWake(Device->Parent);
                }
                
                //
                // Finally, let the FDO know that wake was triggered.
                //

                PciCompleteRequest(WakeIrp, CompletionStatus);
            }
        }
    }
}

__drv_functionClass(DRIVER_CANCEL)
_Requires_lock_held_(_Global_cancel_spin_lock_)
_Releases_lock_(_Global_cancel_spin_lock_)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
PciDeviceWaitWakeCancel(
    _Inout_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ _IRQL_uses_cancel_ struct _IRP *Irp
    )
/*++

Routine Description:

    Cancel an outstanding WAIT_WAKE IRP.

    Note: The Cancel Spin Lock is held on entry.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP.

Return Value:

    None.

--*/
{
    PPCI_DEVICE device;
    KIRQL oldIrql;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)DeviceObject->DeviceExtension;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    TraceEvents(device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x): device %p IRP: %p",
                device->Parent->SecondaryBusNumber,
                device,
                Irp);

    IoReleaseCancelSpinLock(Irp->CancelIrql);

    //
    // Acquire the WakeIrpLock. This lock ensures that the cancel routine will
    // never race with the dispatch routine or parent wake irp callback routine
    // over Irp ownership.
    //

    PCI_ASSERT(device->WakeIrp == Irp);

    KeAcquireSpinLock(&device->WakeIrpLock, &oldIrql);
    device->WakeIrp = NULL;
    KeReleaseSpinLock(&device->WakeIrpLock, oldIrql);

    //
    // If the WaitWake IRP has already been dealt with (completed
    // before we got here), just return, otherwise, clear out
    // the stored IRP and clear PME.
    //
    if (PciWakeIrpQueued != InterlockedCompareExchange((volatile LONG*)&device->WakeIrpState,
                                                       PciWakeIrpCancelled,
                                                       PciWakeIrpQueued)) {
        return;
    }

    //
    // Turn the parent bus on so we can update the PME settings
    //

    PciBus_PowerReference(device->Parent, PciBusPowerReasonChildWaitWakeComplete);

    PciDisablePme(device);
    InterlockedExchange((volatile LONG*)&device->WakeIrpState, PciWakeIrpNone);
    if (PciDeviceSupportsNativeExpressPme(device)) {
        ExpressUpdateWakeControl(device, FALSE);
    }

    if (!PCI_ROOT_BUS(device->Parent)) {
        PciBus_DisarmForWake(device->Parent);
    }

    PciBus_PowerDereference(device->Parent, PciBusPowerReasonChildWaitWakeComplete);
    PciCompleteRequest(Irp, STATUS_CANCELLED);
    return;
}

VOID
ExpressUpdateWakeControl (
    __in PPCI_DEVICE Device,
    __in BOOLEAN ArmedForWake
    )

/*++

Routine Description:

    This routine is invoked to update the count of number of express devices
    armed for wake. When the count goes from zero to one or one to zero then
    invoke an express wake control method exported by ACPI to toggle the wake
    disable bit in the PM1 enable register

Arguments:

    Device - Supplies a pointer to an express device.

    ArmedForWake - Supplies a boolean indicating if a device is being armed or
        disarmed for wake.

Return Value:

    None.

--*/

{

    KIRQL OldIrql;
    PPCI_BUS_INTERFACE_STANDARD RootBusInterface;

    NON_PAGED_CODE();

    RootBusInterface = Device->Root->PciBusInterface;
    KeAcquireSpinLock(&ExpressWakeControlLock, &OldIrql);
    if (ArmedForWake != FALSE) {
        ExpressDevicesEnabledForWake += 1;
        if ((ExpressDevicesEnabledForWake == 1) &&
            (RootBusInterface->ExpressWakeControl != NULL)) {

            RootBusInterface->ExpressWakeControl(RootBusInterface->Context,
                                                 TRUE);
        }

    } else {
        ExpressDevicesEnabledForWake -= 1;
        if ((ExpressDevicesEnabledForWake == 0) &&
            (RootBusInterface->ExpressWakeControl != NULL)) {

            RootBusInterface->ExpressWakeControl(RootBusInterface->Context,
                                                 FALSE);

        }
    }

    KeReleaseSpinLock(&ExpressWakeControlLock, OldIrql);
    return;
}

// ----------------------------------------------------------- Utility Functions

DEVICE_POWER_STATE
PciGetHardwarePowerState(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine returns the current hardware power state of the given
    device.

Arguments:

    Device - The device to return the power state of.

Return Value:

    The current hardware power state.

--*/
{
    PCI_PMCSR pmcsr;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->PowerCapability);

    PciReadPowerCapabilityRegister(Device, PMCSR, &pmcsr);

    return PCI_TO_NT_POWER_STATE(pmcsr.PowerState);
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciSetHardwarePowerStateReset (
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine marks the beginning of a hardware power state change, by
    zeroing the "delay" value.  This should be called at the beginning of a
    transition, as the transition may be asynchronous, and restarted several
    times.

Arguments:

    Device - The device to set the power state of.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    Device->PowerStateChangeDelay = 0;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
PciSetHardwarePowerStateInProgress (
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine detects whether a power state transition is currently in
    progress.

Arguments:

    Device - The device to set the power state of.

Return Value:

    BOOLEAN - in progress or not

--*/
{
    NON_PAGED_CODE();

    if (Device->PowerStateChangeDelay == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciSetPowerChangeDelay (
    _In_ PPCI_DEVICE        Device,
    _In_ DEVICE_POWER_STATE OldState,
    _In_ DEVICE_POWER_STATE NewState
    )

/*++

Routine Description:

    This routine calculates the proper delay for a device changing from one
    power state to another.

Arguments:

    Device - The device to set the power state of.

    OldState - The state the device is currently in.
 
    NewState - The state to set the device into.
 
Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    Device->PowerStateChangeDelay = 
        PciPowerDelayTable[OldState-PowerDeviceD0][NewState-PowerDeviceD0];

}

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciSetHardwarePowerStatePreamble (
    _In_    PPCI_DEVICE         Device,
    _In_    DEVICE_POWER_STATE  DeviceState,
    _Out_   PPCI_PMCSR          PmCsr
    )

/*++

Routine Description:

    This routine does the work common to PciSetHardwarePowerStatePassive and
    PciSetHardwarePowerStateDispatch.

Arguments:

    Device - The device to set the power state of.

    DeviceState - The state to set the device into.
 
    PmCsr - Out parameter that allows the calling wrapper to avoid rereading
        hardware that has already been read in this helper function.

Return Value:

    NT Status code.

--*/

{
    DEVICE_POWER_STATE oldDeviceState;
    PCI_PM_CAPABILITY pmCap;

    NON_PAGED_CODE();

    PCI_ASSERT(Device->PowerCapability);
    PCI_ASSERT((DeviceState >= PowerDeviceD0) && (DeviceState <= PowerDeviceD3));

    //
    // First read in the power capability to make sure it looks valid.
    //

    PciReadDeviceCapability(Device, PowerCapability, &pmCap);
    if (pmCap.Header.CapabilityID != PCI_CAPABILITY_ID_POWER_MANAGEMENT) {

        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x:%x) Invalid power capability!"
                    "Expected: 0x%02x, Actual: 0x%02x.",
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber,
                    PCI_CAPABILITY_ID_POWER_MANAGEMENT,
                    pmCap.Header.CapabilityID);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    *PmCsr = pmCap.PMCSR.ControlStatus;

    //
    // Now set the device into its new D state, stalling after the transition.
    // Only actually perform the write if the new state is different from the
    // current state of the hardware, except if the new state is D0.  Assume
    // that a device may need to be re-placed into D0 in order to properly
    // initialize itself for proper D0 operation.  Pretending that the previous
    // state is D3 ensures the maximum stall to allow this reinitialization to
    // occur.
    //

    oldDeviceState = PCI_TO_NT_POWER_STATE(PmCsr->PowerState);
    if ((DeviceState == PowerDeviceD0) && (oldDeviceState == PowerDeviceD0)) {
        oldDeviceState = PowerDeviceD3;
    }
    Device->PreviousPowerState = oldDeviceState;

    //
    // Lookup the delay we are meant to do as in the PCI spec
    //
    if ((oldDeviceState >= PowerDeviceD0) && (oldDeviceState <= PowerDeviceD3) &&
        (DeviceState >= PowerDeviceD0) && (DeviceState <= PowerDeviceD3)) {

        PciSetPowerChangeDelay(Device, oldDeviceState, DeviceState);
    }
    return STATUS_SUCCESS;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciSetHardwarePowerStateLogError (
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE TargetState
    )

/*++

Routine Description:

    This routine logs the failure if a device cannot be moved to the target
    power state.

Arguments:

    Device - The device to set the power state of.
 
    TargetState - The state that this device was going to.

Return Value:

    none

--*/

{
    PPCI_VERIFIER_DATA verifierData;

    NON_PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_ERROR,
                DBG_DEVPOWER,
                "(%x:%x) failed to stall long enough for power state change",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);






















    verifierData = PciVerifierRetrieveFailureData(
        PCI_VERIFIER_PMCSR_TIMEOUT
        );

    if (verifierData) {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_DEVPOWER,
                    "(%x): device %p No verifier data.",
                    Device->Parent->SecondaryBusNumber,
                    Device);
        VfFailDeviceNode(
            Device->DeviceObject,
            PCI_VERIFIER_DETECTED_VIOLATION,
            PCI_VERIFIER_PMCSR_TIMEOUT,
            verifierData->FailureClass,
            &verifierData->Flags,
            verifierData->FailureText,
            "%DevObj%Ulong",
            Device->DeviceObject,
            NT_TO_PCI_POWER_STATE(TargetState)
            );
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSetHardwarePowerStatePassive (
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE DeviceState
    )

/*++

Routine Description:

    This routine sets the current hardware power state of the given
    device.  After doing so, it waits for the spec-defined delay
    before proceeding, to allow the state change to settle.

Arguments:

    Device - The device to set the power state of.

    DeviceState - The state to set the device into.

Return Value:

    NT Status code.

--*/

{
    PCI_PMCSR pmcsr;
    NTSTATUS status;

    PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x:%x) -> %!WDM_DEVICE_POWER_STATE!",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber,
                DeviceState);

    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {
        return STATUS_SUCCESS;
    }

    PciSetHardwarePowerStateReset(Device);

    status = PciSetHardwarePowerStatePreamble(Device, DeviceState, &pmcsr);

    if (!NT_SUCCESS(status)) {
        PciSetHardwarePowerStateLogError(Device, DeviceState);
        return status;
    }

    if (Device->PreviousPowerState != DeviceState) {

        Device->DstateChangeRetryCount = DEVICE_DSTATE_CHANGE_LIMIT;
        pmcsr.PowerState = NT_TO_PCI_POWER_STATE(DeviceState);
        PciWritePowerCapabilityRegister(Device, PMCSR, &pmcsr);
        status = PciStallForPowerChange(Device, DeviceState);
    } else {
        status = STATUS_SUCCESS;
    }

    return status;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciSetHardwarePowerStateDispatch (
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE DeviceState
    )

/*++

Routine Description:

    This routine sets the current hardware power state of the given
    device.  After doing so, it waits for the spec-defined delay
    before proceeding, to allow the state change to settle.

Arguments:

    Device - The device to set the power state of.

    DeviceState - The state to set the device into.

Return Value:

    NT Status code.
    STATUS_CANT_WAIT - Indicates that we're not really done here and work
        will be completed when the timer fires.

--*/

{
    BOOLEAN inProgress;
    PCI_PMCSR pmcsr;
    NTSTATUS status;

    NON_PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x:%x) -> %!WDM_DEVICE_POWER_STATE!",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber,
                DeviceState);

    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {
        return STATUS_SUCCESS;
    }

    inProgress = PciSetHardwarePowerStateInProgress(Device);

    //
    // Execute the "preamble" code only on the first pass through this function.
    //

    if (inProgress == FALSE) {
        status = PciSetHardwarePowerStatePreamble(Device, DeviceState, &pmcsr);
    
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    if (Device->PreviousPowerState == DeviceState) {

        status = STATUS_SUCCESS;

    } else if (PciKSRInProgress && DeviceState != PowerDeviceD0) {

        //
        // Skip setting PMCSR for devices in any non-D0 state.
        //

        status = STATUS_SUCCESS;

    } else {

        if (inProgress == FALSE) {
            pmcsr.PowerState = NT_TO_PCI_POWER_STATE(DeviceState);
            PciWritePowerCapabilityRegister(Device, PMCSR, &pmcsr);
            Device->DstateChangeRetryCount = DEVICE_DSTATE_CHANGE_LIMIT;
        }

        status = PciStallForPowerChange(Device, DeviceState);

        if (!NT_SUCCESS(status) && (status != STATUS_CANT_WAIT)) {
            PciSetHardwarePowerStateLogError(Device, DeviceState);
        }
    }

    return status;
}

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciStallForPowerChange(
    _In_ PPCI_DEVICE        Device,
    _In_ DEVICE_POWER_STATE NewPowerState
    )
/*++

Routine Description:

    This routine stalls to allow a power state change to settle into
    the device.  The amount of time to stall is stored in the
    PciPowerDelayTable.

Arguments:

    Device - The device to stall for.

    NewPowerState - The power state of the device *after* the transition.

Return Value:

    NT Status Code.
    STATUS_CANT_WAIT - This function should be called again after the
        appropriate delay.
    STATUS_DEVICE_PROTOCOL_ERROR - The device failed to move to the requested
        power state even after waiting for as long as we could possibly imagine
        to be reasonable.

--*/
{
    PCI_PMCSR pmcsr;
    KIRQL irql;

    NON_PAGED_CODE();

    PCI_ASSERT(!(Device->HackFlags & PCI_HACK_NO_PM_CAPS));

    //
    // Stall in a polite fashion if IRQL allows
    //
    irql = KeGetCurrentIrql();

    while (Device->DstateChangeRetryCount > 0) {

        //
        // On the second and subsequent trips through this loop, reread the
        // status and control register.  The assumption here is that some cards
        // don't get their act together fast enough and the fact that they arn't
        // ready yet is reflected by them not updating the power control
        // register with what we just wrote to it.  This is not in the PCI spec
        // but is how some of these broken cards work and it can't hurt...
        //

        if (Device->DstateChangeRetryCount != DEVICE_DSTATE_CHANGE_LIMIT) {

            PciReadPowerCapabilityRegister(Device, PMCSR, &pmcsr);

            if (pmcsr.PowerState == NT_TO_PCI_POWER_STATE(NewPowerState)) {
                return STATUS_SUCCESS;

            } else if (Device->DstateChangeRetryCount == 1) {

                //
                // On the final iteration, trace the value of the PMCSR
                // for diagnostic purposes.
                //

                TraceEvents(Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_INFORMATION,
                            DBG_DEVPOWER,
                            "(%x:%x) : PciStallForPowerChange: PMCSR.PowerState = 0x%x",
                            Device->Slot.u.bits.DeviceNumber,
                            Device->Slot.u.bits.FunctionNumber,
                            pmcsr.PowerState);
            }

            //
            // Subsequent iterations, delay 1ms.
            //

            Device->PowerStateChangeDelay = 1000;
        }

        Device->DstateChangeRetryCount--;

        if (irql < DISPATCH_LEVEL) {

            //
            // Get off the processor.
            //
            // timeoutPeriod is in units of 100ns, negative means
            // relative.  Add KeQueryTimeIncrememnt to the delay
            // to make sure the delay actually occurs.
            //

            LARGE_INTEGER timeoutPeriod;

            timeoutPeriod.QuadPart = -10 * Device->PowerStateChangeDelay;
            timeoutPeriod.QuadPart -= (KeQueryTimeIncrement() - 1);

            KeDelayExecutionThread(KernelMode,
                                   FALSE,
                                   &timeoutPeriod
                                   );

        } else if (irql == DISPATCH_LEVEL) {

            return STATUS_CANT_WAIT;

        } else {

            //
            // Spin when at high IRQL.  This should only happen when 
            // doing the "early resume" stuff, when there is not alternative
            // to spinning anyhow.
            //

            PciDevice_Stall(Device,(ULONG)Device->PowerStateChangeDelay);
        }
    }

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_DEVPOWER,
                "(%x:%x) : PciStallForPowerChange: Maximum retry reached",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    return STATUS_DEVICE_PROTOCOL_ERROR;
}

VOID
PciEnablePme(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    Enable the PME Enable bit for a device(function).

    Note: The PDO Extension lock is held on entry and is not released
    by this routine.

Arguments:

    Device - the device whose PME is going to be enabled.

Return Value:

    None.

--*/
{
    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    //
    // Is the device's PME management owned by someone else?
    //
    if (!Device->PmeOwned) {

        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_DEVPOWER,
                    "(%x): device %p but PME not owned",
                    Device->Parent->SecondaryBusNumber,
                    Device);
        return;
    }

    //
    // Really update the PME signal. Note that we always need to supply
    // the 3rd argument as FALSE --- we don't want to just clear the PME
    // Status bit
    //
    PciAdjustPmeEnable(Device, TRUE, FALSE);
}

VOID
PciDisablePme(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    Disable the PME Enable bit for a device(function).

    Note: The PDO Extension lock is held on entry and is not released
    by this routine.

Arguments:

    Device - the device whose PME Enable bit is to be disabled.

Return Value:

    None.

--*/
{
    PEXPRESS_BRIDGE RootExpressBridge;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    PCI_DEVICE_CLEAR_FLAG(Device, TriggeredPme);

    //
    // Is the device's PME management owned by someone else?
    //
    if (!Device->PmeOwned) {

        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_DEVPOWER,
                    "(%x): device %p but PME not owned",
                    Device->Parent->SecondaryBusNumber,
                    Device);
        return;
    }

    //
    // Really update the PME signal. Note that we always need to supply
    // the 3rd argument as FALSE --- we don't want to just clear the PME
    // Status bit
    //
    PciAdjustPmeEnable(Device, FALSE, FALSE);

    //
    // If this is Express, clear the event in the root port so that we might
    // take another PME some time in the future.
    //

    if (PCI_BUS_USES_NATIVE_PME(Device->Parent)) {
        switch (Device->Parent->PciBridge->ExpressBridge->BridgeType) {
        case PciExpressRootPort:
            RootExpressBridge = Device->Parent->PciBridge->ExpressBridge;
            break;
        case PciExpressUpstreamSwitchPort:
            RootExpressBridge = Device->Parent->PciBridge->ExpressBridge->UpstreamSwitchPort.RootPortBridge;
            break;
        case PciExpressDownstreamSwitchPort:
            RootExpressBridge = Device->Parent->PciBridge->ExpressBridge->Downstream.u.SwitchPort.RootPortBridge;
            break;
        default:
            PCI_ASSERTMSG("What other kind of bridge could it be?", FALSE);
            RootExpressBridge = NULL;
        }

        if (RootExpressBridge != NULL) {
            ExpressRootPortAckPmeEvent(RootExpressBridge);
        }
    }
}

VOID
PciAdjustPmeEnable(
    __in PPCI_DEVICE Device,
    __in BOOLEAN Enable,
    __in BOOLEAN ClearStatusOnly
    )
/*++

Routine Description:

    This is the only routine in the the PCI driver that is allowed to set
    the PME Enable pin for a device.

Arguments:

    Device - The device that wants to have the PME enable set

    Enable - Turn on the PME pin or not

    ClearStatusOnly - Only clear the status --- ignore the Enable bit

Return Value:

    VOID

--*/
{
    PCI_PMCSR pmcsr;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    //
    // Are there any pm capabilities?
    //
    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {

        return;
    }

    //
    // Only update the enabled state in the device extension
    // if the ClearStatusOnly parameter is FALSE
    //

    if (ClearStatusOnly == FALSE) {
        if (Enable != FALSE) {
            PCI_DEVICE_SET_FLAG(Device, PmeEnabled);
        } else {
            PCI_DEVICE_CLEAR_FLAG(Device, PmeEnabled);
        }
    }

    if (PCI_DEVICE_IN_D3COLD(Device)) {
        return;
    }

    PciReadPowerCapabilityRegister(Device, PMCSR.ControlStatus, &pmcsr);

    //
    // Set or clear the PMEEnable bit depending on the value of Enable
    //
    if (ClearStatusOnly == FALSE) {
        pmcsr.PMEEnable = (Enable != 0);
    }

    //
    // Write back, note that if PME Status is set, writing back the 1
    // will clear it.
    //
    PciWritePowerCapabilityRegister(Device, PMCSR.ControlStatus, &pmcsr);
}

VOID
PciGetPmeInformation(
    __in PPCI_DEVICE Device,
    __out_opt PBOOLEAN PmeCapable,
    __out_opt PBOOLEAN PmeStatus,
    __out_opt PBOOLEAN PmeEnable
    )
/*++

Routine Description:

    This routine gets the current state of the given device's PME support.

Arguments:

    Device - The device to get the PME information of.

    PmeCapable - If present, will point to TRUE if the device is PME capable.

    PmeStatus - If present, will point to TRUE if the device's PME status
        bit is currently set.

    PmeEnable - If present, will point to TRUE if the device's PME enable
        bit is currently set.

Return Value:

    VOID

--*/
{
    BOOLEAN pmeCapable;
    BOOLEAN pmeEnable;
    BOOLEAN pmeStatus;
    PCI_PM_CAPABILITY pmCap = {0};

    NON_PAGED_CODE();

    pmeCapable = FALSE;
    pmeEnable = FALSE;
    pmeStatus = FALSE;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {
        goto exit;
    }

    //
    // A device without a power capability is not PME
    // capable.
    //

    if (Device->PowerCapability == 0) {
        goto exit;
    } 

    pmeCapable = TRUE;

    //
    // Accessing config space while in D3Cold is illegal.
    //

    if (PCI_DEVICE_IN_D3COLD(Device)) {

        //
        // Derive the PmeEnabled status from data structures.
        // As config space cannot be read, the PME status
        // cannot be determined and is left as FALSE.
        //

        pmeEnable = Device->PmeEnabled? TRUE : FALSE;
        goto exit;
    }

    PciReadDeviceCapability(Device, PowerCapability, &pmCap);

    if (pmCap.Header.CapabilityID != PCI_CAPABILITY_ID_POWER_MANAGEMENT) {
        goto exit;
    }

    if (pmCap.PMCSR.ControlStatus.PMEEnable == 1) {

        pmeEnable = TRUE;
    }

    if (pmCap.PMCSR.ControlStatus.PMEStatus == 1) {

        pmeStatus = TRUE;
    }

exit:

    if (ARGUMENT_PRESENT(PmeCapable)) {

        *PmeCapable = pmeCapable;
    }
    if (ARGUMENT_PRESENT(PmeStatus)) {

        *PmeStatus = pmeStatus;
    }
    if (ARGUMENT_PRESENT(PmeEnable)) {

        *PmeEnable = pmeEnable;
    }
    return;
}

BOOLEAN
PciDeviceSupportsNativeExpressPme (
    _In_ PPCI_DEVICE Device
    )
/*++

Routine Description:

    Indicates whether a device uses Express "native" PME, which is to say that
    the PCI driver handles power management interrupts natively at the root
    port, rather than through ACPI and wake signaling.

Arguments:

    Device - structure tracking this PCI device.
 
Return Value:

    FALSE indicates tha the device's wake signals must be handled externally,
        probably through ACPI.

--*/
{
    PPCI_ROOT_BUS_HARDWARE_CAPABILITY hardwareCapability;
    DEVICE_POWER_STATE rootIdleDstate;
    DEVICE_POWER_STATE rootWakeDstate;

    //
    // Only Express devices can support native PME.
    //

    if (Device->ExpressPort == NULL) {
        return FALSE;
    }
    
    //
    // We can only support native PME if the BIOS handed off control of it.
    //

    hardwareCapability = &Device->Root->HardwareCapability;
    if (hardwareCapability->OscControlGranted.u.ExpressNativePME == 0) {
        return FALSE;
    }
        
    //
    // Only devices below a root port can use native PME.  (This isn't strictly
    // true, as the spec allows for "root complex event collectors" that handle
    // the other devices.  We don't support those, at least not yet.)
    //

    switch (Device->ExpressPort->DeviceType) {
    case PciExpressRootComplexIntegratedEndpoint:
        __fallthrough;
    case PciExpressRootPort:
        __fallthrough;
    case PciExpressRootComplexEventCollector:
        return FALSE;
        break;
    }

    //
    // If the root port itself doesn't support wake, then assume that the BIOS
    // doesn't really know what it means to hand off control of PME to the OS
    // and let the firmware control it.
    //
    
    rootWakeDstate = Device->ExpressPort->RootPortBridge->PciBridge->Bus->DeviceWake;
    rootIdleDstate = Device->ExpressPort->RootPortBridge->PciBridge->Bus->IdleDState;
    if (((rootWakeDstate < PowerDeviceD1) || (rootWakeDstate > PowerDeviceD3)) &&
        ((rootIdleDstate < PowerDeviceD1) || (rootIdleDstate > PowerDeviceD3))) {
        return FALSE;
    }

    return TRUE;
}

// ------------------------------------------------------------------- PME Interface Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciPmeInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the PCI_PME_INTERFACE fields.

Arguments:

    PciInterface - Pointer to the PciInterface record for this
        interface type.

    InterfaceSpecificData

    InterfaceReturn

    Filtered - Not supported for this interface.
 
Return Value:

    Status

--*/
{
    PPCI_PME_INTERFACE standard = (PPCI_PME_INTERFACE)InterfaceReturn;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PCI_PME_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    standard->Size                  = sizeof(PCI_PME_INTERFACE);
    standard->Version               = Version;
    standard->Context               = DeviceExtension;
    standard->InterfaceReference    = PciPmeInterface_Reference;
    standard->InterfaceDereference  = PciPmeInterface_Dereference;
    standard->GetPmeInformation     = PciPmeInterface_GetInformation;
    standard->ClearPmeStatus        = PciPmeInterface_ClearPmeStatus;
    standard->UpdateEnable          = PciPmeInterface_UpdateEnable;
    standard->GetPmeControl         = PciPmeInterface_GetPmeControl;

    return STATUS_SUCCESS;
}

VOID
PciPmeInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_BUS

Return Value:

    None

--*/
{
    PPCI_BUS bus = (PPCI_BUS)Context;

    InterlockedIncrement(&bus->PciPmeInterfaceCount);

    return;
}

VOID
PciPmeInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_BUS

Return Value:

    None

--*/
{
    PPCI_BUS bus = (PPCI_BUS)Context;

    InterlockedDecrement(&bus->PciPmeInterfaceCount);

    return;
}

VOID
PciPmeInterface_UpdateEnable(
    __in PDEVICE_OBJECT Pdo,
    __in BOOLEAN PmeEnable
    )
/*++

Routine Description:

    This routine sets or clears the PME Enable bit on the specified
    device object

Arguments:

    Pdo         - The device object whose PME enable we care about
    PmeEnable   - Wether or not we should enable PME on the device

Return Value:

    None

--*/
{
    PPCI_DEVICE device;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)Pdo->DeviceExtension;
    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    //
    // Mark the device as not having its PME managed by PCI any more...
    //
    PCI_DEVICE_CLEAR_FLAG(device, PmeOwned);

    //
    // Call the interface that does the real work. Note that we always need
    // to supply the 3rd argument as FALSE --- we don't to just clear the
    // PME Status bit
    //
    PciAdjustPmeEnable(device, PmeEnable, FALSE);
}

VOID
PciPmeInterface_ClearPmeStatus(
    __in PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine explicitly clears the PME status bit from a device

Arguments:

    Pdo - The device whose pin we are to clear

Return Value:

    VOID

--*/
{
    PPCI_DEVICE device;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)Pdo->DeviceExtension;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    //
    // Call the Adjust function to do the actual work. Note that passing
    // in the 3rd argument as TRUE means that the 2nd argument is ignored
    //
    PciAdjustPmeEnable(device, FALSE, TRUE);
}

VOID
PciPmeInterface_GetInformation(
    __in PDEVICE_OBJECT Pdo,
    __out_opt PBOOLEAN PmeCapable,
    __out_opt PBOOLEAN PmeStatus,
    __out_opt PBOOLEAN PmeEnable
    )
/*++

Routine Description:

    Supplies the information regarding a PDO's PME capabilities

Arguments:

    Pdo - The device object whose capabilities we care about

    PmeCapable - Can the device generate a PME?

    PmeStatus - Is the PME status for the device on?

    PmeEnable - Is the PME enable for the device on?

Return Value:

    None

--*/
{
    PPCI_DEVICE device;

    NON_PAGED_CODE();

    device = (PPCI_DEVICE)Pdo->DeviceExtension;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    PciGetPmeInformation(device, PmeCapable, PmeStatus, PmeEnable);
}

VOID
PciPmeInterface_GetPmeControl (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PBOOLEAN PmeControl
    )

/*++

Routine Description:

    This routine relinquishes control of the device PME registers to the caller
    (ACPI) for non-Express devices in the case where a _PRW namespace object
    is defined in the scope of the device.

Arguments:

    PhysicalDeviceObject - Supplies a PDO for a device whose control of PME
        registers is being requested.

    PmeControl - Supplies a placeholder for control value of TRUE vs FALSE.
        TRUE means that the ACPI driver controls PME, not the PCI driver.

Return Value:

    None.

--*/

{

    PPCI_DEVICE Device;

    NON_PAGED_CODE();

    Device = (PPCI_DEVICE)PhysicalDeviceObject->DeviceExtension;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    *PmeControl = !PciDeviceSupportsNativeExpressPme(Device);
    return;
}

// -------------------------------------- PCI D3Cold Support Interface Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciD3ColdSupportInterface_Constructor (
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    Initialize the D3COLD_SUPPORT_INTERFACE fields.

Arguments:

    PciInterface - Pointer to the PciInterface record for this
        interface type.

    InterfaceSpecificData

    InterfaceReturn

    Filtered - Indicates whether the passed in interface already has valid data
        from a filter higher in the stack.  This function then must preserve
        those functions and invoke them as part of handling calls to this
        interface.
 
Return Value:

    Status

--*/
{
    PPCI_DEVICE device;
    D3COLD_SUPPORT_INTERFACE interface =
    {
        sizeof(D3COLD_SUPPORT_INTERFACE),
        0,
        NULL,
        PciD3ColdSupportInterface_Reference,
        PciD3ColdSupportInterface_Dereference,
        PciD3ColdSupportInterface_SetD3ColdSupport,
        PciD3ColdSupportInterface_GetIdleWakeInfo,
        PciD3ColdSupportInterface_GetD3ColdCapability,
        PciD3ColdSupportInterface_GetD3ColdBusDriverSupport,
        PciD3ColdSupportInterface_GetLastTransitionStatus
    };

    PAGED_CODE();
    PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceExtension));
    
    device = (PPCI_DEVICE)DeviceExtension;

    UNREFERENCED_PARAMETER(InterfaceSpecificData);

    //
    // Respond to requests for only part of the table, as we want to support
    // drivers built against earlier Win8 builds.
    //

    if (Size < FIELD_OFFSET(D3COLD_SUPPORT_INTERFACE, GetD3ColdCapability)) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Must be a later verion of the interface.
    //

    if (Size > sizeof(D3COLD_SUPPORT_INTERFACE)) {
        Size = sizeof(D3COLD_SUPPORT_INTERFACE);
    }

    if (Filtered != FALSE) {

        //
        // If this has been invoked before, and we're "filtering again" drop
        // the reference on the interface that we're about to demolish.
        //

        if (device->D3ColdFilter.InterfaceDereference != NULL) {
            device->D3ColdFilter.InterfaceDereference(device->D3ColdFilter.Context);
        }

        //
        // Keep the interface already in this IRP for filtering.
        //
#pragma prefast(suppress: __WARNING_USING_UNINIT_VAR, "the annotation on interfaces doesn't take filtering into account")
        RtlCopyMemory(&device->D3ColdFilter, 
                      InterfaceReturn, 
                      Size);

        //
        // Add a reference to match this copy of it.
        //

        if (device->D3ColdFilter.InterfaceReference != NULL) {
            device->D3ColdFilter.InterfaceReference(device->D3ColdFilter.Context);
        }
    }

    //
    // Find out whether the firmware thinks that this device is capable of
    // D3cold.
    //

    interface.Version = Version;
    interface.Context = DeviceExtension;

    RtlCopyMemory(InterfaceReturn,
                  &interface,
                  Size);

    return STATUS_SUCCESS;
}

VOID
PciD3ColdSupportInterface_Reference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PPCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    if (device->D3ColdFilter.InterfaceReference != NULL) {
        device->D3ColdFilter.InterfaceReference(device->D3ColdFilter.Context);
    }

    InterlockedIncrement(&(device->PciD3ColdSupportInterfaceCount));

    return;
}

VOID
PciD3ColdSupportInterface_Dereference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PPCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    if (device->D3ColdFilter.InterfaceDereference != NULL) {
        device->D3ColdFilter.InterfaceDereference(device->D3ColdFilter.Context);
    }

    InterlockedDecrement(&(device->PciD3ColdSupportInterfaceCount));

    return;
}

__drv_functionClass(SET_D3COLD_SUPPORT)
_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciD3ColdSupportInterface_SetD3ColdSupport (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    _In_ BOOLEAN D3ColdSupport
    )
/*++

Routine Description:

    This routine toggles D3cold support for this device.

    This routine isn't paged, even though it's marked PASSIVE_LEVEL, so that
    drivers on the paging path can call through this at any time.
 
Arguments:

    PPCI_DEVICE
 
    D3ColdSupport - Indicates whether D3cold should be supported or not.

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(device));

    if (device->D3ColdSupport == D3ColdSupportDisabledInRegistry) {
        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_DEVPOWER,
                    "device %p driver wants to change d3cold state but the registry is forcing it off",
                    device);
        return;
    }
         
    //
    // Let filters know that this is changing.
    //

    if (device->D3ColdFilter.SetD3ColdSupport != NULL) {
        device->D3ColdFilter.SetD3ColdSupport(device->D3ColdFilter.Context,
                                              D3ColdSupport);
    }

    //
    // Now change the internal notion of D3.
    //

    if (D3ColdSupport != FALSE) {

        if (device->D3ColdSupport != D3ColdSupportEnabled) {
            PciBus_PowerDereference(device->Parent, PciBusPowerAllowF1DisableBridgeD3);
        }
        device->D3ColdSupport = D3ColdSupportEnabled;
    } else {

        if (device->D3ColdSupport == D3ColdSupportEnabled) {
            PciBus_PowerReference(device->Parent, PciBusPowerAllowF1DisableBridgeD3);
        }
        device->D3ColdSupport = D3ColdSupportDisabled;
    }

    return;
}

__drv_functionClass(GET_IDLE_WAKE_INFO)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciD3ColdSupportInterface_GetIdleWakeInfo (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    _In_ SYSTEM_POWER_STATE SystemPowerState,
    _Out_ PDEVICE_WAKE_DEPTH DeepestWakeableDstate
    )
/*++

Routine Description:

    This routine allows a driver to ask the question "what's the deepest D-state
    from which my device can trigger its wake signal, in each S-state?"  For
    now, only S0 is interesting.

Arguments:

    PPCI_DEVICE
 
    SystemPowerState - The system power state about which the query applies.
 
    DeepestWakeableDstate - Out parameter indicating the lowest D-state which
        can deliver the wake signal.

Return Value:

    NTSTATUS

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Let ACPI have first crack at this.
    //

    if (device->D3ColdFilter.GetIdleWakeInfo != NULL) {
        status = device->D3ColdFilter.GetIdleWakeInfo(device->D3ColdFilter.Context,
                                                      SystemPowerState,
                                                      DeepestWakeableDstate);
        if (NT_SUCCESS(status)) {
            return status;
        }
    }

    if (SystemPowerState != PowerSystemWorking) {
        return STATUS_NOT_IMPLEMENTED;
    }

    //
    // If ACPI said nothing and the BIOS left an express system in compatibility
    // mode and nobody set the global reg key, then don't fall back on the
    // PM cap struct.
    //

    if ((device->ExpressCompatibilityPort != NULL) &&
        (PciEnableLegacyPcieRuntimePme == 0)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_DEVPOWER,
                    "Reporting no runtime wake capability for device (b:%x d:%x"
                    " f:%x) because the BIOS left Express in Compatibility "
                    "mode",
                    device->BusNumber,
                    device->Slot.u.bits.DeviceNumber,
                    device->Slot.u.bits.FunctionNumber);

        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        return STATUS_SUCCESS;
    }

    // 
    // For integrated root-bus devices, wake signals go out of band, which makes
    // the BIOS the final word on the subject.
    //

    if (PCI_ROOT_BUS(device->Parent)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_DEVPOWER,
                    "Reporting no runtime wake capability for device (b:%x d:%x"
                    " f:%x) because the parent bus was a root bus and the BIOS "
                    "didn't describe the device as wakeable, as with an _S0W "
                    "object",
                    device->BusNumber,
                    device->Slot.u.bits.DeviceNumber,
                    device->Slot.u.bits.FunctionNumber);

        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        return STATUS_SUCCESS;
    }

    //
    // If ACPI didn't handle this, and the parent bus can't handle a wait/wake
    // IRP, then don't add any runtime power capabilities for this device.
    // This test will effectively mean that root ports need an explicit _S0W
    // object underneath them, or else they will never power off while the
    // system as a whole stays in S0.
    //

    if (!(PCI_BUS_USES_NATIVE_PME(device->Parent)) &&
        (device->Parent->IdleDState <= DeviceWakeDepthD0)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_DEVPOWER,
                    "Reporting no runtime wake capability for device (b:%x d:%x"
                    " f:%x) because the parent bus wasn't running in native "
                    "Express mode and the BIOS didn't describe the device as "
                    "wakeable, as with an _S0W object",
                    device->BusNumber,
                    device->Slot.u.bits.DeviceNumber,
                    device->Slot.u.bits.FunctionNumber);

        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        return STATUS_SUCCESS;
    }

    //
    // If ACPI said nothing and this is a PCI to PCI bridge, then fail, unless
    // the bridge has the D3ColdSupported value in the registry.  The idea is
    // to not enable runtime power management for bridges that wouldn't have
    // done that in Win7.  A few bridges are actually part of a "device," where
    // the plug-in board has a bridge or a switch on it.  For these, the IHV
    // may have supplied an INF which references machine.inf, and enables
    // D3Cold.
    // 

    if ((device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
        (device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI) &&
        (device->D3ColdSupport != D3ColdSupportEnabled)) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_DEVPOWER,
                    "Reporting no runtime wake capability for a PCI bridge at "
                    "(b:%x d:%x f:%x) because the BIOS didn't describe the "
                    "device as wakeable, as with an _S0W object and no INF "
                    "claimed to support this bridge specifically, setting "
                    "D3ColdSupported to 1",
                    device->BusNumber,
                    device->Slot.u.bits.DeviceNumber,
                    device->Slot.u.bits.FunctionNumber);

        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        return STATUS_SUCCESS;
    }


    //
    // If the device has a not wakeable hack flag, return success with
    // DeviceWakeDepthNotWakeable.
    //    
    if ((device->HackFlags & PCI_HACK_NOT_WAKEABLE) != 0) {

        TraceEvents(device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_DEVPOWER,
                    "Reporting no runtime wake capability for device (b:%x d:%x"
                    " f:%x) because the device has been indentified in "
                    "errata.inf as one which does not support wake signals",
                    device->BusNumber,
                    device->Slot.u.bits.DeviceNumber,
                    device->Slot.u.bits.FunctionNumber);

        *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        return STATUS_SUCCESS;
    }

    //
    // Now, if the checks above don't apply, fill in by looking at the
    // combination of the bus's info and the device config registers.
    //
    // If the device supports wake from D3cold and the bus supports idling,
    // then the answer is D3cold.  If the bus doesn't support idling, the
    // answer is D3hot.
    //

    if ((device->PowerSupport.PMED3Cold != 0) &&
        (device->Parent->IdleDState > DeviceWakeDepthD0)) {

        *DeepestWakeableDstate = DeviceWakeDepthD3cold;
        return STATUS_SUCCESS;
    }

    if ((device->PowerSupport.PMED3Hot != 0)) {
        *DeepestWakeableDstate = DeviceWakeDepthD3hot;
        return STATUS_SUCCESS;
    }

    if (device->PowerSupport.PMED2 != 0) {
        *DeepestWakeableDstate = DeviceWakeDepthD2;
        return STATUS_SUCCESS;
    }

    if (device->PowerSupport.PMED1 != 0) {
        *DeepestWakeableDstate = DeviceWakeDepthD1;
        return STATUS_SUCCESS;
    }

    if (device->PowerSupport.PMED0 != 0) {
        *DeepestWakeableDstate = DeviceWakeDepthD0;
        return STATUS_SUCCESS;
    }

    TraceEvents(device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "Reporting no runtime wake capability for device (b:%x d:%x "
                "f:%x) because the device claimed not to support it in its "
                "Power Management capability structure",
                device->BusNumber,
                device->Slot.u.bits.DeviceNumber,
                device->Slot.u.bits.FunctionNumber);

    *DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
    return STATUS_SUCCESS;
}

__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciD3ColdSupportInterface_GetD3ColdCapability (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    _Out_ PBOOLEAN D3ColdSupported
    )
/*++

Routine Description:

    This routine allows a driver to ask the question "does the firmware
    indicate that this device supports D3cold?"

Arguments:

    PPCI_DEVICE
 
    D3ColdSupported - Out parameter indicating whether this device might enter
        D3cold.

Return Value:

    NTSTATUS

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Let ACPI have first crack at this.
    //

    if (device->D3ColdFilter.GetD3ColdCapability != NULL) {
        status = device->D3ColdFilter.GetD3ColdCapability(device->D3ColdFilter.Context,
                                                          D3ColdSupported);
        if (NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // If ACPI said nothing and the bus can idle in D3cold, then assume the
    // device transition can be to D3cold.
    //

    if (device->Parent->IdleDState == DeviceWakeDepthD3cold) {
        *D3ColdSupported = TRUE;
    } else {
        *D3ColdSupported = FALSE;
    }

    return STATUS_SUCCESS;
}

__drv_functionClass(GET_D3COLD_CAPABILITY)
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciD3ColdSupportInterface_GetD3ColdBusDriverSupport (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    _Out_ PBOOLEAN D3ColdSupported
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

    PPCI_DEVICE
 
    D3ColdSupported - Out parameter indicating whether this device might enter
        D3cold.

Return Value:

    success, always

--*/
{

    PAGED_CODE();

    *D3ColdSupported = TRUE;

    return STATUS_SUCCESS;
    UNREFERENCED_PARAMETER(Context);
}

__drv_functionClass(GET_D3COLD_LAST_TRANSITION_STATUS)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciD3ColdSupportInterface_GetLastTransitionStatus (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context,
    _Out_ PD3COLD_LAST_TRANSITION_STATUS LastTransitionStatus
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

    PPCI_DEVICE
 
    D3ColdSupported - Out parameter indicating whether this device might enter
        D3cold.

Return Value:

    success, always

--*/
{

    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    //
    // Let ACPI have the last word on this.
    //

    if (device->D3ColdFilter.GetLastTransitionStatus != NULL) {
        device->D3ColdFilter.GetLastTransitionStatus(device->D3ColdFilter.Context,
                                                     LastTransitionStatus);
        return;
    }

    *LastTransitionStatus = LastDStateTransitionStatusUnknown;

    return;
    UNREFERENCED_PARAMETER(Context);
}

