/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    consolelock.c

Abstract:

    This module contains code to manage how pci.sys behaves when the system
    console is locked.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include <power.h>

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

POWER_SETTING_CALLBACK PciPowerSettingCallback;
IO_SESSION_NOTIFICATION_FUNCTION PciSessionStateCallback;

VOID
PciSetConsoleState (
    _In_ PCI_CONSOLE_STATE ConsoleState
    );

// --------------------------------------------------------------------- Pragmas

#pragma alloc_text(PAGE, PciPowerSettingCallback)
#pragma alloc_text(PAGE, PciSessionStateCallback)
#pragma alloc_text(PAGE, PciSetBusDisabledByConsoleLock)
#pragma alloc_text(PAGE, PciSetConsoleState)

// --------------------------------------------------------------------- Globals

PCI_CONSOLE_STATE PciConsoleState;
PVOID PciPowerSettingCallbackHandle;
PVOID PciSessionStateCallbackHandle;
BOOLEAN PciEnableConsoleLockHandling = FALSE;

// ------------------------------------------------------------------- Functions

NTSTATUS PciInitConsoleLock (
    )

/*++

Routine Description:

    Initializes the global data structures needed to hook into console
    lock notifications.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{
    HANDLE CLHandle;
    IO_SESSION_STATE_NOTIFICATION IoSessionStateNotificationStruct;
    BOOLEAN Result;
    NTSTATUS Status;
    PULONG ValueDword;
    ULONG ValueLength;

    CLHandle = NULL;

    //
    // Default to console locked until initial login.
    //

    PciConsoleState = PciConsoleStatePreLogon;

    //
    // Register for console lock notifications.
    //

    PoRegisterPowerSettingCallback(NULL,
                                   &GUID_CONSOLE_LOCKED,
                                   PciPowerSettingCallback,
                                   NULL,
                                   &PciPowerSettingCallbackHandle);

    //
    // Register for login notifications.
    //

    IoSessionStateNotificationStruct.Size = sizeof(IO_SESSION_STATE_NOTIFICATION);
    IoSessionStateNotificationStruct.Flags = 0;
    IoSessionStateNotificationStruct.Context = NULL;
    IoSessionStateNotificationStruct.IoObject = PciDriverObject;
    IoSessionStateNotificationStruct.EventMask =
        IO_SESSION_STATE_LOGON_EVENT |
        IO_SESSION_STATE_LOGOFF_EVENT;

    IoRegisterContainerNotification(
        IoSessionStateNotification,
        (PIO_CONTAINER_NOTIFICATION_FUNCTION)PciSessionStateCallback,
        &IoSessionStateNotificationStruct,
        sizeof(IoSessionStateNotificationStruct),
        &PciSessionStateCallbackHandle);

    //
    // Query the registry to determine if the ConsoleLock policy should be enabled.  
    //

    Result = PciOpenKey(L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Control\\pnp\\pci",
                        NULL,
                        KEY_READ,
                        &CLHandle,
                        &Status);

    if (Result == FALSE) {
        goto cleanup;
    }

    //
    // Read the DisableExternalDMAUnderLock subkey. Failure to open this 
    // key is not fatal.
    //

    Status = PciGetRegistryValue(L"DisableExternalDMAUnderLock",
                                 NULL,
                                 CLHandle,
                                 REG_DWORD,
                                 &ValueDword,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        if (ValueLength == sizeof(ULONG)) {
            PciEnableConsoleLockHandling = (*ValueDword != 0)? TRUE : FALSE;
        }

        PciFreePool(ValueDword);
    }

cleanup:

    if (CLHandle) {
        ZwClose(CLHandle);
    }

    return STATUS_SUCCESS;
}

NTSTATUS PciDeinitConsoleLock (
    )

/*++

Routine Description:

    Deinitializes the global data structures used to hook into console
    lock notifications.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{

    NTSTATUS Status; 
    
    PciConsoleState = PciConsoleStatePreLogon;
    if (PciPowerSettingCallbackHandle != NULL) {
        Status = PoUnregisterPowerSettingCallback(PciPowerSettingCallbackHandle);

        PCI_ASSERT(NT_SUCCESS(Status));

        PciPowerSettingCallbackHandle = NULL;
    }

    if (PciSessionStateCallbackHandle != NULL) {
        IoUnregisterContainerNotification(PciSessionStateCallbackHandle);
        PciSessionStateCallbackHandle = NULL;
    }

    return STATUS_SUCCESS;
}

BOOLEAN
PciBusAffectedByConsoleLock(
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    Determines determines whether the console lock state should affect the bus.

Arguments:

    Bus - Provides the bus to be examined.

Return Value:

    FALSE if the bus should not be disabled while the console is locked,
    TRUE otherwise.

--*/

{

    if (PciEnableConsoleLockHandling == FALSE) {
        return FALSE;
    }

    //
    // If the bus wasn't originally blocked by the firmware, don't disable it.
    //

    if (Bus->BusmasterDisabledOnBoot == FALSE) {
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
PciBusShouldBeDisabledByConsoleLock(
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine determines whether a bus which is affected by the console lock
    state should currently be disabled, based on:
    - The current console lock state.
    - Whether the bus has any present devices.

    Note: The caller must have already verified that the Bus should be
    affected by PciBusAffectedByConsoleLock returning TRUE.

Arguments:

    Bus - Provides the bus to be examined.

Return Value:

   Returns a boolean specifying whether the console should be locked.

--*/

{

    switch (PciConsoleState) {
        case PciConsoleStatePreLogon:

            //
            // In the prelogon state, anything affected by console lock
            // should be left disabled.
            //

            return TRUE;

        case PciConsoleStateLocked:

            //
            // In the locked state, only busses with no children should be
            // locked.
            //

            if ((Bus->ChildDevices != NULL) &&
                (Bus->OnceContainedNoDevices == FALSE)) {

                return FALSE;

            } else {
                return TRUE;
            }


        case PciConsoleStateUnlocked:
        default:

            //
            // In the unlocked state everything should be left enabled.
            //

            return FALSE;
    }
}

VOID
PciSetBusDisabledByConsoleLock(
    _In_ PPCI_BUS Bus,
    _In_ BOOLEAN Disable
    )

/*++

Routine Description:

    This routine configures the bridge busmaster bit to enable or disable
    traffic from downstream.

Arguments:

    Bus - Provides the bus to be controlled.

    Disabled - Provides a boolean specifying whether the bus should be
    enabled or disabled.

Return Value:

    None.

--*/

{

    USHORT OldCommand;
    USHORT NewCommand;
    PPCI_BRIDGE PciBridge;

    PAGED_CODE();

    PciBridge = Bus->PciBridge;
    if (PciBridge == NULL) {
        return;
    }

    PciReadBridgeConfigRegister(PciBridge, Command, &OldCommand);

    if (Disable != FALSE) {
        NewCommand = OldCommand & ~PCI_ENABLE_BUS_MASTER;

    } else {
        NewCommand = OldCommand | PCI_ENABLE_BUS_MASTER;
    }

    if (NewCommand != OldCommand) {
        PciWriteBridgeConfigRegister(PciBridge, Command, &NewCommand);
    }

    return;
}

VOID
PciSetConsoleState (
    _In_ PCI_CONSOLE_STATE ConsoleState
    )

/*++

Routine Description:

    This routine iterates over each bus in the system, determines whether
    that bus should be disabled based on the console lock state, and
    enables/disables it as appropriate.

Arguments:

    Locked - Provides a boolean specifying whether the console is locked.

Return Value:

    None.

--*/

{
    PPCI_BUS Bus;
    BOOLEAN Disable;
    PPCI_SEGMENT Segment;

    PAGED_CODE();

    if (ConsoleState == PciConsoleState) {
        return;
    }

    PciAcquirePassiveLock(&PciGlobalLock);
    PciConsoleState = ConsoleState;
    for (Segment = PciSegmentList;
         Segment != NULL;
         Segment = Segment->Next) {

        for (Bus = Segment->PciRootBusList;
             Bus != NULL;
             Bus = PciNextBusInSegment(Bus)) {

            if (PciBusAffectedByConsoleLock(Bus) != FALSE) {

                //
                // Configure the bus based on the current lock state.
                //

                Disable = PciBusShouldBeDisabledByConsoleLock(Bus);
                PciSetBusDisabledByConsoleLock(Bus, Disable);
                if (Disable == FALSE) {

                    //
                    // The bus should be unlocked. Queue up a call to
                    // PciScanBus.
                    //

                    IoInvalidateDeviceRelations(Bus->PhysicalDeviceObject, BusRelations);
                }
            }
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);
    return;
}

NTSTATUS
PciPowerSettingCallback (
    _In_ LPCGUID SettingGuid,
    _In_ PVOID   Value,
    _In_ ULONG   ValueLength,
    _In_ PVOID   Context
    )

/*++

Routine Description:

    This function is the power setting callback from the kernel. It currently
    only registers for callbacks when the console lock state changes.

Arguments:

    SettingGuid - Provides a GUID identifying the power setting which has changed.

    Value - Provides a pointer to a GUID-specific value specifying the change to
        the power setting.

    ValueLength - Provides the length in bytes of the data structure pointed to
        by Value.

    Context - Provides a context pointer which was originally passed as the
        fourth parameter to PoRegisterPowerSettingCallback.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);

    //
    // Look for changes to the console locked state.
    //

    if (IsEqualGUID(SettingGuid, &GUID_CONSOLE_LOCKED) &&
        ValueLength == sizeof(ULONG)) {

        //
        // Configure the system based on the new lock state.
        //

        if (*((PULONG)Value) == 0) {
            PciSetConsoleState(PciConsoleStateUnlocked);
        } else {
            PciSetConsoleState(PciConsoleStateLocked);
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciSessionStateCallback (
    _In_ PVOID SessionObject,
    _In_ PVOID IoObject,
    _In_ ULONG Event,
    _In_ PVOID Context,
    _In_reads_bytes_opt_(PayloadLength) PVOID NotificationPayload,
    _In_ ULONG PayloadLength
    )
{

    PAGED_CODE();

    UNREFERENCED_PARAMETER(SessionObject);
    UNREFERENCED_PARAMETER(IoObject);
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(NotificationPayload);
    UNREFERENCED_PARAMETER(PayloadLength);

    switch(Event) {
    case IoSessionStateDisconnectedLoggedOn:
    case IoSessionStateLoggedOn:
        PciSetConsoleState(PciConsoleStateUnlocked);
        break;
    case IoSessionStateLoggedOff:
        PciSetConsoleState(PciConsoleStateLocked);
        break;
    }

    return STATUS_SUCCESS;
}

