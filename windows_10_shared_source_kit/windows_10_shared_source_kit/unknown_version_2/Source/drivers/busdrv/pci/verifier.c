/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    verifier.c

Abstract:

    This module implements routines used to catch BIOS, hardware, and driver
    bugs.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include <wdmguid.h>

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

DRIVER_NOTIFICATION_CALLBACK_ROUTINE PciVerifierProfileChangeCallback;

VOID
PciVerifierEnsureTreeConsistancy(
    VOID
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT,     PciVerifierInit)
    #pragma alloc_text(PAGE,     PciVerifierUnload)
    #pragma alloc_text(PAGE,     PciVerifierProfileChangeCallback)
#endif

// --------------------------------------------------------------------- Globals

BOOLEAN PciVerifierRegistered = FALSE;

PVOID PciVerifierNotificationHandle = NULL;

//
// This is the table of PCI verifier failures
//
PCI_VERIFIER_DATA PciVerifierFailureTable[] = {

    { PCI_VERIFIER_BRIDGE_REPROGRAMMED, VFFAILURE_FAIL_LOGO,
      0,
      "The BIOS has reprogrammed the bus numbers of an active PCI device "
      "(!devstack %DevObj) during a dock or undock!" },

    { PCI_VERIFIER_PMCSR_TIMEOUT, VFFAILURE_FAIL_LOGO,
      0,
      "A device in the system did not update it's PMCSR register in the spec "
      "mandated time (!devstack %DevObj, Power state D%Ulong)" },

    { PCI_VERIFIER_PROTECTED_CONFIGSPACE_ACCESS, VFFAILURE_FAIL_LOGO,
      0,
      "A driver controlling a PCI device has tried to access OS controlled "
      "configuration space registers (!devstack %DevObj, Offset 0x%Ulong1, "
      "Length 0x%Ulong2)" },
    
    { PCI_VERIFIER_INVALID_WHICHSPACE, VFFAILURE_FAIL_UNDER_DEBUGGER,
      0,
      "A driver controlling a PCI device has tried to read or write from "
      "an invalid space using IRP_MN_READ/WRITE_CONFIG or via BUS_INTERFACE_STANDARD.  "
      "NB: These functions take WhichSpace parameters of the form PCI_WHICHSPACE_* "
      "and not a BUS_DATA_TYPE (!devstack %DevObj, WhichSpace 0x%Ulong1)" }
};

// ------------------------------------------------------------------- Functions

VOID
PciVerifierInit(
    __in PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    This routine initializes the hardware verification support, enabling
    consistancy hooks and state checks where appropriate.

Arguments:

    DriverObject - Pointer to our driver object.

Return Value:

    None.

--*/
{
    NTSTATUS status;

    NON_PAGED_CODE();
    
    if (!VfIsVerificationEnabled(VFOBJTYPE_SYSTEM_BIOS, NULL)) {

        return;
    }

    status = IoRegisterPlugPlayNotification(
        EventCategoryHardwareProfileChange,
        0,
        NULL,
        DriverObject,
        PciVerifierProfileChangeCallback,
        (PVOID) NULL,
        &PciVerifierNotificationHandle
        );

    if (NT_SUCCESS(status)) {

        PciVerifierRegistered = TRUE;
    }
}

VOID
PciVerifierUnload(
    VOID
    )
/*++

Routine Description:

    This routine uninitializes the hardware verification support.

Arguments:

    DriverObject - Pointer to our driver object.

Return Value:

    None.

--*/
{
    NTSTATUS status;

    PAGED_CODE();

    if (!PciVerifierRegistered) {
        return;
    }

    PCI_ASSERT(PciVerifierNotificationHandle);

    status = IoUnregisterPlugPlayNotification(PciVerifierNotificationHandle);

    PCI_ASSERT(NT_SUCCESS(status));

    PciVerifierRegistered = FALSE;
}


_Function_class_(DRIVER_NOTIFICATION_CALLBACK_ROUTINE)
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
PciVerifierProfileChangeCallback(
    _In_ PVOID NotificationStructure,
    _Inout_opt_ PVOID NotUsed
    )
/*++

Routine Description:

    This routine gets called back during hardware profile change events if
    hardware verification is enabled.

Arguments:

    NotificationStructure - Describes the hardware profile event that occured.

    NotUsed - Not used

Return Value:

    NTSTATUS.

--*/
{
    PHWPROFILE_CHANGE_NOTIFICATION notification = 
        (PHWPROFILE_CHANGE_NOTIFICATION)NotificationStructure;
    
    PAGED_CODE();

    UNREFERENCED_PARAMETER(NotUsed);

    if (IsEqualGUID((LPGUID) &notification->Event,
                    (LPGUID) &GUID_HWPROFILE_CHANGE_COMPLETE)) {

        //
        // This is a HW profile change complete message. Do some tests to
        // ensure our hardware hasn't been reprogrammed behind our back.
        //
        PciVerifierEnsureTreeConsistancy();
    }

    return STATUS_SUCCESS;
}

VOID
PciVerifierEnsureTreeConsistancy(
    VOID
    )
/*++

Routine Description:

    This routine checks the device tree and ensures it's physical state matches
    the virtual state described by our structures. A deviation may mean someone
    has reprogrammed the hardware behind our back.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PCI_CONFIG_HEADER configHeader;
    PPCI_VERIFIER_DATA verifierData;
    PPCI_SEGMENT segment;
    PPCI_DEVICE device;
    PPCI_BUS bus;
    
    NON_PAGED_CODE();
    
    //
    // Walk the device tree and verify that bridges have not been 
    // reprogrammed from the settings we gave them.  This lock
    // prevents any changes to the tree while we're doing this.
    //
    PciAcquirePassiveLock(&PciGlobalLock);

    for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
        for (bus = segment->PciRootBusList; 
             bus != NULL; 
             bus = PciNextBusInSegment(bus)) {

            PciAcquirePassiveLockInCriticalSection(&bus->ChildDeviceLock);
            for (device = bus->ChildDevices; device != NULL; device = device->Sibling) {
                
                if ((device->HeaderType != PCI_BRIDGE_TYPE) &&
                    (device->HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {
        
                    //
                    // Not a bridge, nothing to verify
                    //
                    continue;
                }

                if (!device->Present ||
                    (device->LogicalPowerState == PowerDeviceD3)) {
        
                    //
                    // The device is gone.  Don't touch.
                    //
                    continue;
                }

                PciReadConfigHeader(device, &configHeader);
            
                //
                // Ensure bus numbers haven't changed. Note that P2P and Cardbus
                // bridges have their Primary, Secondary & Subordinate fields in the
                // same place.
                //
                //
                // Grab the bus renumbering lock.
                //
                PciAcquirePassiveLockInCriticalSection(&PciBusLock);
                if ((configHeader.type1.PrimaryBus != 
                    LOCAL_BUS_NUMBER(device->BusNumber)) ||
                    (configHeader.type1.SecondaryBus !=
                    device->SavedRegisters.bridge.SecondaryBusNumber) ||
                    (configHeader.type1.SubordinateBus !=
                    device->SavedRegisters.bridge.SubordinateBusNumber)) {
        
                    verifierData = PciVerifierRetrieveFailureData(
                        PCI_VERIFIER_BRIDGE_REPROGRAMMED
                        );
        
                    if (!verifierData) {
                        ERROR("PciVerifierEnsureTreeConsistancy: No verifier data.\n");
                        PciReleasePassiveLockInCriticalSection(&PciBusLock);
                        PciReleasePassiveLockInCriticalSection(&bus->ChildDeviceLock);
                        goto Exit;
                    }

                    VfFailSystemBIOS(
                        PCI_VERIFIER_DETECTED_VIOLATION,
                        PCI_VERIFIER_BRIDGE_REPROGRAMMED,
                        verifierData->FailureClass,
                        &verifierData->Flags,
                        verifierData->FailureText,
                        "%DevObj",
                        device->DeviceObject
                        );
                }

                PciReleasePassiveLockInCriticalSection(&PciBusLock);
            }

            PciReleasePassiveLockInCriticalSection(&bus->ChildDeviceLock);
        }
    }

Exit:
    PciReleasePassiveLock(&PciGlobalLock);
}

PPCI_VERIFIER_DATA
PciVerifierRetrieveFailureData(
    __in PCI_VERIFIER_FAILURE VerifierFailure
    )
/*++

Routine Description:

    This routine retrieves the failure data corresponding to a particular PCI
    verifier failure event.

Arguments:

    PCI Failure.

Return Value:

    Verifier data corresponding to the failure.

--*/
{
    PPCI_VERIFIER_DATA verifierData;
    ULONG i;

    NON_PAGED_CODE();

    for(i = 0;
        i < (sizeof(PciVerifierFailureTable)/sizeof(PciVerifierFailureTable[0]));
        i++) {

        verifierData = PciVerifierFailureTable + i;

        if (verifierData->VerifierFailure == VerifierFailure) {

            return verifierData;
        }
    }

    ERROR("PciVerifierRetrieveFailureData: No verifier data for supplied failure\n");
    return NULL;
}
