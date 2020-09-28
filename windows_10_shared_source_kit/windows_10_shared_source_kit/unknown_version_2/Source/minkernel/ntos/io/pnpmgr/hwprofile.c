/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    dock.c

Abstract:


Author:

    Kenneth D. Ray (kenray) Feb 1998

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#include "cm.h"
#include <string.h>
#include <profiles.h>

//
// Internal functions to dockhwp.c
//

NTSTATUS
IopExecuteHwpDefaultSelect(
    __in  PCM_HARDWARE_PROFILE_LIST ProfileList,
    __out PULONG ProfileIndexToUse,
    __in  PVOID Context
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IopExecuteHwpDefaultSelect)
#pragma alloc_text(PAGE, IopExecuteHardwareProfileChange)
#endif // ALLOC_PRAGMA


NTSTATUS
IopExecuteHwpDefaultSelect(
    __in  PCM_HARDWARE_PROFILE_LIST   ProfileList,
    __out PULONG                      ProfileIndexToUse,
    __in  PVOID                       Context
    )
{
    UNREFERENCED_PARAMETER(ProfileList);
    UNREFERENCED_PARAMETER(Context);

    * ProfileIndexToUse = 0;

    return STATUS_SUCCESS;
}


NTSTATUS
IopExecuteHardwareProfileChange(
    __in  HARDWARE_PROFILE_BUS_TYPE   Bus,
    __in_ecount(SerialNumbersCount)  PWCHAR *ProfileSerialNumbers,
    __in  ULONG                       SerialNumbersCount,
    __out HANDLE                     *NewProfile,
    __out BOOLEAN                    *ProfileChanged
    )
/*++

Routine Description:

    A docking event has occured and now, given a list of Profile Serial Numbers
    that describe the new docking state:
    Transition to the given docking state.
    Set the Current Hardware Profile to based on the new state.
    (Possibly Prompt the user if there is ambiguity)
    Send Removes to those devices that are turned off in this profile,

Arguments:
    Bus - This is the bus that is supplying the hardware profile change.
            (currently only HardwareProfileBusTypeAcpi is supported).

    ProfileSerialNumbers - A list of serial numbers (a list of null terminated
                           UCHAR lists) representing this new docking state.
                           These can be listed in any order, and form a
                           complete representation of the new docking state
    caused by a docking even on the given bus.  A Serial Number string of "\0"
    represents an "undocked state" and should not be listed with any other
    strings.  This list need not be sorted.

    SerialNumbersCount - The number of serial numbers listed.

    NewProfile - a handle to the registry key representing the new hardware
    profile (IE \CCS\HardwareProfiles\Current".)

    ProfileChanged - set to TRUE if new current profile (as a result of this
    docking event, is different that then old current profile.

--*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    ULONG           len;
    ULONG           tmplen;
    ULONG           i, j;
    PWCHAR          tmpStr;
    UNICODE_STRING  tmpUStr;
    PUNICODE_STRING sortedSerials = NULL;

    PPROFILE_ACPI_DOCKING_STATE dockState = NULL;

    IopDbgPrint((   IOP_TRACE_LEVEL,
                    "Execute Profile (BusType %x), (SerialNumCount %x)\n", Bus, SerialNumbersCount));

    //
    // Sort the list of serial numbers
    //
    len = sizeof(UNICODE_STRING) * SerialNumbersCount;
    sortedSerials = ExAllocatePoolWithTag(NonPagedPoolNx,
                                          len,
                                          PNP_POOLTAG_HWPROFILE);

    if (NULL == sortedSerials) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean;
    }

    for(i=0; i < SerialNumbersCount; i++) {

        RtlInitUnicodeString(&sortedSerials[i], ProfileSerialNumbers[i]);
    }

    //
    // I do not anticipate getting more than a few serial numbers, and I am
    // just lazy enough to write this comment and use a bubble sort.
    //
    for(i = 0; i < SerialNumbersCount; i++) {
        for(j = 0; j < SerialNumbersCount - 1; j++) {

            if (0 < RtlCompareUnicodeString(&sortedSerials[j],
                                            &sortedSerials[j+1],
                                            FALSE)) {

                tmpUStr = sortedSerials[j];
                sortedSerials[j] = sortedSerials[j+1];
                sortedSerials[j+1] = tmpUStr;
            }
        }
    }

    //
    // Construct the DockState ID
    //
    len = 0;
    for(i=0; i < SerialNumbersCount; i++) {

        len += sortedSerials[i].Length;
    }

    len += sizeof(WCHAR); // NULL termination;
    
    dockState = (PPROFILE_ACPI_DOCKING_STATE) ExAllocatePoolWithTag(NonPagedPoolNx,
                                                                    len + sizeof(PROFILE_ACPI_DOCKING_STATE),
                                                                    PNP_POOLTAG_HWPROFILE);

    if (NULL == dockState) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean;
    }

    //
    // The following expression evaluates to simply dockState->SerialNumber.
    // Its constructed as shown to make the relationship between tmpStr and
    // the buffer dockState more visible to prefast.
    // 

    tmpStr = (PWCHAR)PNP_FIELD_ADDRESS(PROFILE_ACPI_DOCKING_STATE, 
                                       dockState, 
                                       SerialNumber);
     
    PNP_ASSERT(tmpStr == dockState->SerialNumber);
    for(i = 0, tmplen = 0; 
        i < SerialNumbersCount; 
        i++) {

        tmplen = sortedSerials[i].Length;
        PNP_ASSERT(tmplen <= len - ((PCHAR)tmpStr - (PCHAR)dockState->SerialNumber));
        RtlCopyMemory(tmpStr, sortedSerials[i].Buffer, tmplen);
        tmpStr = (PWCHAR)((PCHAR)tmpStr + tmplen);
    }

    *(tmpStr++) = L'\0';

    PNP_ASSERT(len == (ULONG) ((PCHAR) tmpStr - (PCHAR) dockState->SerialNumber));
    dockState->SerialLength = (USHORT) len;

    if ((SerialNumbersCount > 1) || (L'\0' !=  dockState->SerialNumber[0])) {

        dockState->DockingState = HW_PROFILE_DOCKSTATE_DOCKED;

    } else {

        dockState->DockingState = HW_PROFILE_DOCKSTATE_UNDOCKED;
    }

    //
    // Set the new Profile
    //
    switch(Bus) {

        case HardwareProfileBusTypeACPI:

            status = CmSetAcpiHwProfile(
                dockState,
                IopExecuteHwpDefaultSelect,
                NULL,
                NewProfile,
                ProfileChanged
                );

            PNP_ASSERT(NT_SUCCESS(status) || (!(*ProfileChanged)));
            break;

        default:
            *ProfileChanged = FALSE;
            status = STATUS_NOT_SUPPORTED;
            goto Clean;
    }

Clean:

    if (NULL != sortedSerials) {

        ExFreePoolWithTag(sortedSerials, PNP_POOLTAG_HWPROFILE);
    }

    if (NULL != dockState) {

        ExFreePoolWithTag(dockState, PNP_POOLTAG_HWPROFILE);
    }

    return status;
}

