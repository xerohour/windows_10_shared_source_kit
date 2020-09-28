/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    hydramp.c

Abstract:

    USB 2.0 EHCI driver

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1999, 2000 Microsoft Corporation.  All Rights Reserved.


Revision History:

    2-19-99 : created, jdunn

--*/

#include "common.h"

#include <initguid.h>
#include "usbpriv.h"
#include "hcdiguid.h"
#include "acpiioct.h"
#include "ehcierrlog.h"

//implements the following miniport functions:
//EHCI_StartController
//EHCI_StopController
//EHCI_DisableInterrupts
//EHCI_EnableInterrupts

VOID
EHCI_AssertHwAlignment(
    __inout PDEVICE_DATA DeviceData,
    __in HW_32BIT_PHYSICAL_ADDRESS HwPhysicalAddress,
    __in ULONG Length
    )
/*++

    This routine asserts that a HW structure does not cross a 4KB
    physical page boundary, regardless of whether or not the address
    range is physically contiguous.

    Enhanced Host Controller Interface Specification
    Revision 1.0
    3. Data Structures
    "Note that software must ensure that no interface data structure
    reachable by the EHCI host controller spans a 4K page boundary."

--*/
{
    HW_32BIT_PHYSICAL_ADDRESS endp;

    endp = HwPhysicalAddress + Length-1;
    EHCI_ASSERT(DeviceData,
        (endp & ~(EHCI_PAGE_SIZE-1)) ==
        (HwPhysicalAddress & ~(EHCI_PAGE_SIZE-1)));
}


USB_MINIPORT_STATUS
EHCI_InitializeHardware(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

   Initializes the hardware registers for the host controller.

--*/
{
    PHC_OPERATIONAL_REGISTER    hcOp;
    PHC_CAPABILITIES_REGISTER   hcCap;
    PHC_DEBUGPORT_REGISTERS     hcDebug;
    BOOLEAN                     debugPortInUse;
    USBCMD cmd;
    HCSPARAMS hcSparms;
    NTSTATUS                    tmpstatus;
    ULONG64 finishTime, currentTime;
    ULONG64 qpcTimeStamp;

    hcCap   = DeviceData->CapabilitiesRegisters;
    hcOp    = DeviceData->OperationalRegisters;
    hcDebug = DeviceData->DebugPortRegisters;


    debugPortInUse = FALSE;

    hcSparms.ul =
        READ_REGISTER_ULONG(&hcCap->HcStructuralParameters.ul);

    if (hcDebug)
    {
        DEBUGPORTCONTROL dbgControl;

        dbgControl.ul = READ_REGISTER_ULONG(&hcDebug->Control.ul);

        if (dbgControl.InUse)
        {
            debugPortInUse = TRUE;
        }

        DeviceData->DebugPortNumberUI = (USHORT) hcSparms.DebugPortNumber;

    }

    DeviceData->NumberOfPorts =
        (USHORT) hcSparms.NumberOfPorts;

    if (DeviceData->NumberOfPorts == 0 ||
        DeviceData->NumberOfPorts > EHCI_MAX_PORT) {
        return USBMP_STATUS_HARDWARE_FAILURE;
    }

    DeviceData->PortPowerControl =
        (USHORT) hcSparms.PortPowerControl;

    if (!debugPortInUse)
    {
        DeviceData->DebugPortNumber = 0;

        // reset the controller
        //
        LOGENTRY(DeviceData, G, '_res', 0, 0, 0);
        cmd.ul = 0;
        cmd.HostControllerReset = 1;
        WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

        //
        // KeQueryInterruptTimePrecise is used to get the most accurate time, 
        // Previously we used non precise versions like KeQuerySystemTime can potentially return stale time
        // Please see Windows Blue Bug: 5146530 
        //
        currentTime = KeQueryInterruptTimePrecise(&qpcTimeStamp);

        // no spec'ed time -- we will graciously grant 0.1 sec.
        //
        // figure when we quit (.1 seconds later)
        finishTime = currentTime + 1000000;

        // wait for reset bit to got to zero
        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

        while (cmd.HostControllerReset) {

            if (currentTime >= finishTime) {

                return USBMP_STATUS_HARDWARE_FAILURE;
            }

            //
            // Note: Ensure that after the call to KeQueryInterruptTime, we do read the value
            // of the command register again. This is to prevent the case where if our thread is
            // pre-empted right at the time we call KeQueryInterruptTime, we may incorrectly believe
            // that too much time has passed and the controller has not finished the reset
            // operation.
            // Please see Windows 8 Bug : 425591
            //
            // Using KeQueryInterruptTime, because of tight loop to avoid any perf impact
            currentTime = KeQueryInterruptTime();

            cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        }

        tmpstatus = EHCI_EvaluateAcpiRoutineOnHcReset(DeviceData->TargetDeviceObject);
        if (NT_SUCCESS(tmpstatus)) {
            DeviceData->EvaluateAcpiMethodOnResetSucceeded = TRUE;
        } else {
            DeviceData->EvaluateAcpiMethodOnResetSucceeded = FALSE;
        }
    }
    else
    {
        DeviceData->DebugPortNumber = (USHORT) hcSparms.DebugPortNumber;
    }

    // inialize operational registers
    WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, 0);
    WRITE_REGISTER_ULONG(&hcOp->PeriodicListBase, 0);

    // set the enabled interrupts cache, we'll enable
    // these interrupts when asked
    DeviceData->EnabledInterrupts.UsbInterrupt = 1;
    DeviceData->EnabledInterrupts.UsbError = 1;
    DeviceData->EnabledInterrupts.FrameListRollover = 1;
    DeviceData->EnabledInterrupts.HostSystemError = 1;
    DeviceData->EnabledInterrupts.IntOnAsyncAdvance = 1;

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_InitializeSchedule(
     __inout PDEVICE_DATA DeviceData,
     __inout_bcount(USBEHCI_COMMON_BUFFER_BYTES) PUCHAR StaticQHs,
     HW_32BIT_PHYSICAL_ADDRESS StaticQHsPhys,
     __inout PUCHAR DummyQHs,
     HW_32BIT_PHYSICAL_ADDRESS DummyQHsPhys
    )
/*++

Routine Description:

    Build the schedule of static QHs

    The "StaticQHs" common buffer allocation is used for the following
    sub-allocations:

    (1) Frame list.
    (2) 64 static interrupt queue heads.
    (3) Asynchronous List Persistent Queue Head.
    (4) Asynchronous List Persistent Queue Head Dummy.

    The USBEHCI_COMMON_BUFFER_BYTES value needs to be sufficiently large
    to account for the above sub-allocations.

Arguments:

Return Value:

--*/
{
    ULONG i;
    PHC_OPERATIONAL_REGISTER hcOp;
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;
    PHW_32BIT_PHYSICAL_ADDRESS frameBase;
    PHCD_QUEUEHEAD_DESCRIPTOR AsyncQueueHead;
    HW_LINK_POINTER AsyncQueueHeadHwLink;
    PHCD_QUEUEHEAD_DESCRIPTOR AsyncQueueHeadDummy;
    HW_LINK_POINTER AsyncQueueHeadDummyHwLink;

    hcOp = DeviceData->OperationalRegisters;

    // Allocate the frame list.
    //
    frameBase = DeviceData->FrameListBaseAddress =
            (PHW_32BIT_PHYSICAL_ADDRESS) StaticQHs;
    DeviceData->FrameListBasePhys =
        StaticQHsPhys;
    StaticQHs += USBEHCI_MAX_FRAME*sizeof(HW_32BIT_PHYSICAL_ADDRESS);
    StaticQHsPhys += USBEHCI_MAX_FRAME*sizeof(HW_32BIT_PHYSICAL_ADDRESS);

    // Allocate 64 static interrupt queue heads.
    //
    for (i = 0; i < 64; i++) {

        qh = (PHCD_QUEUEHEAD_DESCRIPTOR) StaticQHs;
        qh->PhysicalAddress = StaticQHsPhys;

        EHCI_AssertHwAlignment(DeviceData, qh->PhysicalAddress,
            sizeof(HW_QUEUEHEAD_DESCRIPTOR));

        DeviceData->StaticInterruptQH[i] = qh;

        StaticQHs += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
        StaticQHsPhys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
    }

    // Allocate the Asynchronous List Persistent Queue Head.
    //
    AsyncQueueHead = (PHCD_QUEUEHEAD_DESCRIPTOR) StaticQHs;
    AsyncQueueHead->PhysicalAddress = StaticQHsPhys;
    EHCI_AssertHwAlignment(DeviceData, AsyncQueueHead->PhysicalAddress,
                           sizeof(HW_QUEUEHEAD_DESCRIPTOR));

    // Head of Reclamation List Flag = 1
    // CurrentTD = NULL
    // Next_qTD = EHCI_TERMINATE_BIT
    // AltNext_qTD = NULL
    // Active = 0, Halted = 1
    //
    SET_T_BIT(AsyncQueueHead->HwQH.Overlay.qTD.Next_qTD.HwAddress);
    token.ul = 0;
    token.Halted = 1;
    AsyncQueueHead->HwQH.Overlay.qTD.Token = token;
    AsyncQueueHead->HwQH.EpChars.HeadOfReclimationList = 1;
    AsyncQueueHead->HwQH.EpCaps.HighBWPipeMultiplier = 1;
    AsyncQueueHead->Sig = SIG_HCD_AQH;

    AsyncQueueHeadHwLink.HwAddress = AsyncQueueHead->PhysicalAddress;
    SET_QH(AsyncQueueHeadHwLink.HwAddress);

    DeviceData->AsyncQueueHead = AsyncQueueHead;

    StaticQHs += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
    StaticQHsPhys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);

    // Allocate the Asynchronous List Persistent Queue Head Dummy.
    // This Queue Head is inserted in the Asynchronous schedule only
    // when the Asynchronous schedule is empty except for the
    // Persistent Queue Head.  Some host controllers do not appear to
    // disable Asynchronous schedule processing when the Asynchronous
    // schedule contains only a single Queue Head.
    //
    AsyncQueueHeadDummy = (PHCD_QUEUEHEAD_DESCRIPTOR) StaticQHs;
    AsyncQueueHeadDummy->PhysicalAddress = StaticQHsPhys;
    EHCI_AssertHwAlignment(DeviceData, AsyncQueueHeadDummy->PhysicalAddress,
                           sizeof(HW_QUEUEHEAD_DESCRIPTOR));

    // Head of Reclamation List Flag = 0
    // CurrentTD = NULL
    // Next_qTD = EHCI_TERMINATE_BIT
    // AltNext_qTD = NULL
    // Active = 0, Halted = 1
    //
    SET_T_BIT(AsyncQueueHeadDummy->HwQH.Overlay.qTD.Next_qTD.HwAddress);
    token.ul = 0;
    token.Halted = 1;
    AsyncQueueHeadDummy->HwQH.Overlay.qTD.Token = token;
    AsyncQueueHeadDummy->HwQH.EpCaps.HighBWPipeMultiplier = 1;
    AsyncQueueHeadDummy->Sig = SIG_HCD_AQH_DUMMY;

    AsyncQueueHeadDummyHwLink.HwAddress = AsyncQueueHeadDummy->PhysicalAddress;
    SET_QH(AsyncQueueHeadDummyHwLink.HwAddress);

    // If the Asynchronous List Persistent Queue Head Dummy is enabled
    // then cross link the Asynchronous List Persistent Queue Heads back
    // to each other.  Otherwise just link the Asynchronous List
    // Persistent Queue Head back to itself and the Asynchronous List
    // Persistent Queue Head Dummy will never be inserted in the
    // schedule.
    //
    if (DeviceData->AsyncQueueHeadDummyEnable) {
        AsyncQueueHead->HwQH.HLink.HwAddress = AsyncQueueHeadDummyHwLink.HwAddress;
        QH_DESCRIPTOR_PTR(AsyncQueueHead->NextQh) = AsyncQueueHeadDummy;
        QH_DESCRIPTOR_PTR(AsyncQueueHead->PrevQh) = AsyncQueueHeadDummy;
    } else {
        AsyncQueueHead->HwQH.HLink.HwAddress = AsyncQueueHeadHwLink.HwAddress;
        QH_DESCRIPTOR_PTR(AsyncQueueHead->NextQh) = AsyncQueueHead;
        QH_DESCRIPTOR_PTR(AsyncQueueHead->PrevQh) = AsyncQueueHead;
    }

    AsyncQueueHeadDummy->HwQH.HLink.HwAddress = AsyncQueueHeadHwLink.HwAddress;
    QH_DESCRIPTOR_PTR(AsyncQueueHeadDummy->NextQh) = AsyncQueueHead;
    QH_DESCRIPTOR_PTR(AsyncQueueHeadDummy->PrevQh) = AsyncQueueHead;

    DeviceData->AsyncQueueHeadDummy = AsyncQueueHeadDummy;

    StaticQHs += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);
    StaticQHsPhys += sizeof(HCD_QUEUEHEAD_DESCRIPTOR);

    // Initialize the 64 static interrupt queue heads into the interrupt
    // schedule periodic interval tree.
    //
    EHCI_InitializeInterruptSchedule(DeviceData);

    for (i=0; i<USBEHCI_MAX_FRAME; i++) {

        PHCD_QUEUEHEAD_DESCRIPTOR fr_qh;
        HW_32BIT_PHYSICAL_ADDRESS qhPhys;

        fr_qh = EHCI_GetQueueHeadForFrame(DeviceData, i);
        qhPhys = fr_qh->PhysicalAddress;
        SET_QH(qhPhys);
        EHCI_AssertHwAlignment(DeviceData, fr_qh->PhysicalAddress,
                    sizeof(HW_QUEUEHEAD_DESCRIPTOR));

        *frameBase = qhPhys;
        frameBase++;
    }

    if (DeviceData->UseDummyQheadsEnabled) {

        LOGENTRY(DeviceData, G, '_dmq', DummyQHs, DummyQHsPhys, 0);
        DeviceData->DummyQueueHeads = (PHCD_QUEUEHEAD_DESCRIPTOR)DummyQHs;
        DeviceData->DummyQueueHeadsPhys = DummyQHsPhys;

       EHCI_AddDummyQueueHeads(DeviceData);

    } else {
        DeviceData->DummyQueueHeads = 0;
        DeviceData->DummyQueueHeadsPhys = 0;
    }

    // program the frame list
    WRITE_REGISTER_ULONG(&hcOp->PeriodicListBase,
        DeviceData->FrameListBasePhys);

    // write the async qh to the controller
    WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, AsyncQueueHeadHwLink.HwAddress);

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_ReadUlongRegFlag(
     __inout PDEVICE_DATA DeviceData,
     __in_bcount(FlagNameSize) PCWSTR FlagName,
     ULONG FlagNameSize,
     __in ULONG DefaultFlagValue,
     __out_opt PULONG Flag,
     __out_opt PBOOLEAN FoundInRegistry,
     BOOLEAN SoftwareBranch
    )
/*++

Routine Description:

    Reads a ULONG registry flag from the HC Pnp Keys.

    The routine reads the FlagName registry value from the registry.

    If the value is successfully read from the registry,
        the Flags output returns 0 or 1. (0 if the registry value read was 0, 1 otherwise)
    Else
        the Flags output returns the DefaultFlagValue

Arguments:

    DeviceData - device context for the host controller.

    FlagName - pointer to a buffer that contains a null terminated unicode string.
        This is the name of the registry flag to be read.

    FlagNameSize - size of the FlagName buffer (in bytes)

    DefaultValue - value to set if no reg key found.

    Flag - optional - pointer to a ULONG that would recieve the read flag value

    FoundInRegistry - optional - pointer to a BOOLEAN that is set to true only if the registry
        flag was successfully read.

    SoftwareBranch - If TRUE, the flag is queried from the device's software branch, else
        the flag is queried from the hardware branch

Return Value:

    NONE.

--*/
{
    USB_MINIPORT_STATUS mpStatus;
    ULONG tmpFlag;
    ULONG returnFlag;

    returnFlag = DefaultFlagValue;

    mpStatus =
        USBPORT_GET_REGISTRY_KEY_VALUE(DeviceData,
                                       SoftwareBranch, // TRUE==software branch
                                       FlagName,
                                       FlagNameSize,
                                       &tmpFlag,
                                       sizeof(tmpFlag));

    // if this call fails we just use the default setting

    if (mpStatus == USBMP_STATUS_SUCCESS) {

        if (tmpFlag) {
            returnFlag = 1;
        } else {
            returnFlag = 0;
        }
    }

    if (Flag != NULL) {
        *Flag = returnFlag;
    }

    if (FoundInRegistry != NULL) {
        *FoundInRegistry = (mpStatus == USBMP_STATUS_SUCCESS);
    }

    return;
}


VOID
EHCI_ReadUlongRegValue(
     __inout PDEVICE_DATA DeviceData,
     __in_bcount(ValueNameSize) PCWSTR ValueName,
     ULONG ValueNameSize,
     __in ULONG DefaultValue,
     __out_opt PULONG Value,
     __out_opt PBOOLEAN FoundInRegistry,
     BOOLEAN SoftwareBranch
    )
/*++

Routine Description:

    Reads a ULONG registry value from the HC Pnp Keys.

    The routine reads the ValueName registry value from the registry.

    If the value is successfully read from the registry,
        the 'Value' output returns the read value
    Else
        the 'Value' output returns the DefaultValue

Arguments:

    DeviceData - device context for the host controller.

    ValueName - pointer to a buffer that contains a null terminated unicode string.
        This is the name of the registry value to be read.

    ValueNameSize - size of the ValueName buffer (in bytes)

    DefaultValue - value to set if no reg key found.

    Value - optional - pointer to a ULONG that would recieve the read value

    FoundInRegistry - optional - pointer to a BOOLEAN that is set to true only if the registry
        value was successfully read.

    SoftwareBranch - If TRUE, the value is queried from the device's software branch, else
        the value is queried from the hardware branch

Return Value:

    NONE.

--*/
{
    USB_MINIPORT_STATUS mpStatus;
    ULONG tmpValue;
    ULONG returnValue;

    returnValue = DefaultValue;

    mpStatus =
        USBPORT_GET_REGISTRY_KEY_VALUE(DeviceData,
                                       SoftwareBranch, // TRUE==software branch
                                       ValueName,
                                       ValueNameSize,
                                       &tmpValue,
                                       sizeof(tmpValue));

    // if this call fails we just use the default setting

    if (mpStatus == USBMP_STATUS_SUCCESS) {

        returnValue = tmpValue;

    }

    if (Value != NULL) {
        *Value = returnValue;
    }

    if (FoundInRegistry != NULL) {
        *FoundInRegistry = (mpStatus == USBMP_STATUS_SUCCESS);
    }

    return;
}


VOID
EHCI_GetRegistryParameters(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Reads registry parameters and sets the flag bits in DeviceData->Flags appropriately.  If the registry
key is not present the flag field in DeviceData->Flags remains unchanged.

--*/
{
    static CONST WCHAR USBEHCI_SlotsPerEndpoint[] = EHCI_SLOTS_PER_ENDPOINT; 
    static CONST WCHAR USBEHCI_InternalFlagsName[] = EN_EHCI_INTERNAL_FLAGS;
    static CONST WCHAR DsLsFlagName[] = DS_LS_HANDOFF;
    static CONST WCHAR EnPeriodicIdleName[] = EN_PERIODIC_IDLE;
    static CONST WCHAR EnAsyncIdleName[] = EN_ASYNC_IDLE;
    static CONST WCHAR EnSoftRetryName[] = EN_SOFT_RETRY;
    static CONST WCHAR SoftPerSecRetryCountName[] = SOFT_PER_SEC_RETRY_COUNT;
    static CONST WCHAR MissedMicroframePerSecRetryCountName[] = MISSED_MICROFRAME_PER_SEC_RETRY_COUNT;
    static CONST WCHAR EnAsyncQhIdleRemovalName[] = EN_ASYNC_QH_IDLE_REMOVAL;
    static CONST WCHAR EnAsyncQhDummyName[] = EN_ASYNC_QH_DUMMY;
    static CONST WCHAR En64BitAddressing[] = EN_64_BIT_ADDRESSING;
    static CONST WCHAR ResetRecoveryEnableName[] = RESET_RECOVERY_ENABLE;
    static CONST WCHAR ResetRecoveryBreakName[] = RESET_RECOVERY_BREAK;
    static CONST WCHAR IntegratedTTTypeName[] = INTEGRATED_TT_SUPPORT;




    ULONG softRetryEnabled;
    ULONG ds_ls_handoff;
    ULONG UlongAsBool = 0;
    BOOLEAN FoundInRegistry;

    /*
    hardware branch keys:
    HKLM\System\CCS\Enum\ DeviceID \ InstanceID \Device Parameters
    */
    ds_ls_handoff = 0;

    EHCI_ReadUlongRegFlag(DeviceData,
            DsLsFlagName,
            sizeof(DsLsFlagName),
            ds_ls_handoff,
            &ds_ls_handoff,
            NULL,
            FALSE); // Hardware branch

    if (ds_ls_handoff) {
        // reg key indicates low speed handoff should be disbaled
        DeviceData->LowSpeedHandOffEnabled = 0;
    }

    EHCI_ReadUlongRegValue(DeviceData,
            USBEHCI_SlotsPerEndpoint,
            sizeof(USBEHCI_SlotsPerEndpoint),
            DeviceData->SlotsPerEndpoint,
            &DeviceData->SlotsPerEndpoint,
            NULL,
            FALSE);  // Hardware branch

    if ((DeviceData->SlotsPerEndpoint == 0) ||  
        (DeviceData->SlotsPerEndpoint > 2)) {
        DeviceData->SlotsPerEndpoint = 2;
    }

    EHCI_ReadUlongRegValue(DeviceData,
            USBEHCI_InternalFlagsName,
            sizeof(USBEHCI_InternalFlagsName),
            DeviceData->EhciInternalFlags,
            &DeviceData->EhciInternalFlags,
            NULL,
            FALSE);  // Hardware branch

    if ((DeviceData->EhciInternalFlags & EHCI_INTERNAL_ENABLE_RETRY_ON_START) != 0) {
        DeviceData->InitialDoorbellRetryEnableValue = TRUE;
        DeviceData->CurrentDoorbellRetryEnableValue = TRUE;
    }

    if ((DeviceData->EhciInternalFlags & EHCI_INTERNAL_CAN_DO_CMD_CHANGES_WITH_DB) != 0) {
        DeviceData->PreventCmdChangesWithDoorbellPending = FALSE;
    }

    if ((DeviceData->EhciInternalFlags & EHCI_INTERNAL_DISABLE_OPTIMAL_CACHE_FLUSH) != 0) {
        DeviceData->OptimizeInactiveQhFlush = FALSE;
    }

    if ((DeviceData->EhciInternalFlags & EHCI_INTERNAL_DISABLE_TWO_DOORBELL_WORKAROUND) != 0) {
        DeviceData->DisableTwoDoorbellWorkaround = TRUE;
    }

    if ((DeviceData->EhciInternalFlags & EHCI_INTERNAL_ENABLE_STRICT_TRANSITION_CHECKS) != 0) {
        DeviceData->StrictTransitionChecksEnabled = TRUE;
    }

    EHCI_ReadUlongRegFlag(DeviceData,
            EnAsyncIdleName,
            sizeof(EnAsyncIdleName),
            DeviceData->AsyncIdleDetectEnabled,
            &DeviceData->AsyncIdleDetectEnabled,
            NULL,
            FALSE);  // Hardware branch

    EHCI_ReadUlongRegFlag(DeviceData,
            EnPeriodicIdleName,
            sizeof(EnPeriodicIdleName),
            DeviceData->PeriodicIdleDetectEnabled,
            &DeviceData->PeriodicIdleDetectEnabled,
            &FoundInRegistry,
            FALSE);  // Hardware branch

    DeviceData->PeriodicIdleDetectEnabledOverride = FoundInRegistry;

    softRetryEnabled = 0;

    EHCI_ReadUlongRegFlag(DeviceData,
            EnSoftRetryName,
            sizeof(EnSoftRetryName),
            softRetryEnabled,
            &softRetryEnabled,
            NULL,
            FALSE);  // Hardware branch

    if (softRetryEnabled != 0) {
        DeviceData->SoftPerSecRetryCount = DEFAULT_PER_SEC_RETRY_COUNT;
    }

    EHCI_ReadUlongRegValue(DeviceData,
            SoftPerSecRetryCountName,
            sizeof(SoftPerSecRetryCountName),
            DeviceData->SoftPerSecRetryCount,
            &DeviceData->SoftPerSecRetryCount,
            NULL,
            FALSE);  // Hardware branch







    } else {
        if (DeviceData->SoftPerSecRetryCount > MAX_PER_SEC_RETRY_COUNT) {
            DeviceData->SoftPerSecRetryCount = MAX_PER_SEC_RETRY_COUNT;
        }
    }

    EHCI_ReadUlongRegValue(DeviceData,
            MissedMicroframePerSecRetryCountName,
            sizeof(MissedMicroframePerSecRetryCountName),
            DeviceData->MissedMicroframePerSecRetryCount,
            &DeviceData->MissedMicroframePerSecRetryCount,
            NULL,
            FALSE);  // Hardware branch







    } else {
        if (DeviceData->MissedMicroframePerSecRetryCount > MAX_PER_SEC_RETRY_COUNT) {
            DeviceData->MissedMicroframePerSecRetryCount = MAX_PER_SEC_RETRY_COUNT;
        }
    }

    EHCI_ReadUlongRegFlag(DeviceData,
            EnAsyncQhIdleRemovalName,
            sizeof(EnAsyncQhIdleRemovalName),
            (ULONG)DeviceData->AsyncQhIdleRemoval,
            &UlongAsBool,
            &FoundInRegistry,
            FALSE);  // Hardware branch

    DeviceData->AsyncQhIdleRemoval = UlongAsBool ? TRUE : FALSE;
    DeviceData->AsyncQhIdleRemovalOverride = FoundInRegistry;

    EHCI_ReadUlongRegFlag(DeviceData,
            EnAsyncQhDummyName,
            sizeof(EnAsyncQhDummyName),
            (ULONG)DeviceData->AsyncQueueHeadDummyEnable,
            &UlongAsBool,
            &FoundInRegistry,
            FALSE);  // Hardware branch

    DeviceData->AsyncQueueHeadDummyEnable = UlongAsBool ? TRUE : FALSE;
    DeviceData->AsyncQueueHeadDummyEnableOverride = FoundInRegistry;














    EHCI_ReadUlongRegFlag(DeviceData,
            En64BitAddressing,
            sizeof(En64BitAddressing),
            (ULONG)DeviceData->Bit64AddressingCapability,
            &UlongAsBool,
            &FoundInRegistry,
            FALSE);  // Hardware branch

    DeviceData->Bit64AddressingCapabilityOverride = FoundInRegistry;
    DeviceData->Bit64AddressingCapability = UlongAsBool ? TRUE : FALSE;

    EHCI_ReadUlongRegValue(DeviceData,
            ResetRecoveryEnableName,
            sizeof(ResetRecoveryEnableName),
            DeviceData->ResetRecoveryEnable,
            &DeviceData->ResetRecoveryEnable,
            NULL,
            FALSE);  // Hardware branch

    EHCI_ReadUlongRegValue(DeviceData,
            ResetRecoveryBreakName,
            sizeof(ResetRecoveryBreakName),
            DeviceData->ResetRecoveryBreak,
            &DeviceData->ResetRecoveryBreak,
            NULL,
            FALSE);  // Hardware branch

    EHCI_ReadUlongRegValue(DeviceData,
            IntegratedTTTypeName,
            sizeof(IntegratedTTTypeName),
            IntegratedTTNotSupported,
            (PULONG)&DeviceData->IntegratedTTSupport,
            &DeviceData->IntegratedTTSupportRegistryOverride,
            FALSE);  // Hardware branch
}


VOID
EHCI_HaltController(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This routine clears the host controller Run/Stop bit and halts the
    host controller.

    This MINIPORT_HaltController() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    USBCMD cmd;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    USBSTS status;
    ULONG i;
    BOOLEAN keepControllerOn;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // Check if we need to keep the controller powered on
    keepControllerOn =
        USBPORT_IsControllerInHibernatePath(DeviceData->HostControllerFdo);

    // clear the run bit
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    LOGENTRY(DeviceData, G, '_stp', cmd.ul, 0, 0);
    if (!keepControllerOn) {
        cmd.HostControllerRun = 0;
    }
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    if (!keepControllerOn) {
        // The EHCI specification says "The Host Controller must halt within
        // 16 micro-frames after software clears the Run bit. The HC Halted
        // bit in the status register indicates when the Host Controller has
        // finished its pending pipelined transactions and has entered the
        // stopped state."
        //
        for (i=0; i<18; i++) {
            status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
            if (status.HcHalted) {
                break;
            }
            KeStallExecutionProcessor(125);
        }

        status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

        if (status.HcHalted != 1) {
            // hardware is messed up
            TEST_TRAP(DeviceData);
        }
    }
}


VOID
USBMPFN
EHCI_StopController(
     __inout PDEVICE_DATA DeviceData,
     BOOLEAN HwPresent
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    USBCMD cmd;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    CONFIGFLAG configFlag;
    USBSTS status;
    ULONG i;
    EVENT_DATA_DESCRIPTOR   EventDataDescriptor[1];
    BOOLEAN keepControllerOn;

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_STOP_DISPATCH,
                      NULL,
                      1,
                      EventDataDescriptor);

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // Check if we need to keep the controller powered on
    keepControllerOn =
        USBPORT_IsControllerInHibernatePath(DeviceData->HostControllerFdo);

    // clear the run bit
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    LOGENTRY(DeviceData, G, '_stp', cmd.ul, 0, 0);
    if (!keepControllerOn) {
        cmd.HostControllerRun = 0;
    }
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // mask off all interrupts
    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                         0);

    if (!keepControllerOn) {
        // The EHCI specification says "The Host Controller must halt within
        // 16 micro-frames after software clears the Run bit. The HC Halted
        // bit in the status register indicates when the Host Controller has
        // finished its pending pipelined transactions and has entered the
        // stopped state."
        //
        for (i=0; i<18; i++) {
            status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
            if (status.HcHalted) {
                break;
            }
            KeStallExecutionProcessor(125);
        }

        status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

        if (status.HcHalted != 1) {
            // hardware is messed up
            TEST_TRAP(DeviceData);
        }

        // set cc control of the hc ports to the companion
        // controllers
        configFlag.ul = 0;
        configFlag.RoutePortsToEHCI = 0;
        WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);
    }

    // usbport will zap devicedata on sto so the global endpoint list must be empty

    EHCI_ASSERT(DeviceData, IsListEmpty(&DeviceData->AllEpListHead) != FALSE);

    EHCI_ASSERT(DeviceData, IsListEmpty(&DeviceData->AsyncIdleRefListHead) != FALSE);

    DeviceData->Interrupt = NULL;

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_STOP_COMPLETE,
                      NULL,
                      1,
                      EventDataDescriptor);
}


VOID
USBMPFN
EHCI_TakePortControl(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    CONFIGFLAG configFlag;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

    // set default port routing
    configFlag.ul = 0;
    configFlag.RoutePortsToEHCI = 1;
    WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);
    DeviceData->CurrentConfigFlag.ul = configFlag.ul;
}


VOID
USBMPFN
EHCI_ReleasePortControl(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    CONFIGFLAG configFlag;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

    // set for no port routing
    configFlag.ul = 0;
    configFlag.RoutePortsToEHCI = 0;
    WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);
    DeviceData->CurrentConfigFlag.ul = configFlag.ul;
}


ULONG
USBMPFN
EHCI_ReadConfigFlag(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    CONFIGFLAG configFlag;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        configFlag.ul = 0;
        return configFlag.ul;
    }

    configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

    if (configFlag.ul == 0xFFFFFFFF) {
        // hardware gone, report as not set
        configFlag.ul = 0;
    }

    return configFlag.ul;
}


VOID
USBMPFN
EHCI_CreateDeviceData(
     __inout PDEVICE_DATA DeviceData
    )
{
    DeviceData->Sig = SIG_EHCI_DD;

    InitializeListHead(&DeviceData->AcfListHead);
    KeInitializeSpinLock(&DeviceData->AcfListSpin);

    InitializeListHead(&DeviceData->PendingSmodeCTB_ListHead);
    InitializeListHead(&DeviceData->PendingSmodeINT_ListHead);
    InitializeListHead(&DeviceData->PendingIsochListHead);
    InitializeListHead(&DeviceData->AsyncIdleRefListHead);

    DeviceData->EhciInternalFlags = 0;

    // EndpointData->EndpointInScheduleCount is initialized to ZERO and
    // DeviceData->CurrentAsyncEnableCount is initialized to ONE
    // so that an unexpected match does not happen when both these counters
    // are set to ZERO without the endpoint/QH being ever added to the schedule
    //

    DeviceData->CurrentAsyncEnableCount = 1;

    DeviceData->HcSuspendedFlag = 0;

    // NOTE: Dummy QHs have always been enabled.  There has never been a
    // mechanism exposed to disable the use of Dummy QHs and no testing
    // has been done with the use of Dummy QHs disabled.  The current
    // implementation of EHCI_InterruptAtFrame() was initially added
    // with blind assumptions that Dummy QHs are always enabled.
    //
    DeviceData->UseDummyQheadsEnabled = 1;

    DeviceData->LowSpeedHandOffEnabled = 1;

    DeviceData->CompanionControllerHandoffEnabled = TRUE;

    DeviceData->AsyncIdleDetectEnabled = 1;

    DeviceData->PeriodicIdleDetectEnabled = 1;

    DeviceData->SoftPerSecRetryCount = 0;

    DeviceData->MissedMicroframePerSecRetryCount = DEFAULT_PER_SEC_RETRY_COUNT;

    DeviceData->ImmediateAsyncEnableChange = 0;     // Default: DISABLED

    DeviceData->ImmediatePeriodicEnableChange = 0;  // Default: DISABLED

    InitializeListHead(&DeviceData->AllEpListHead);
    DeviceData->DeviceStarted = FALSE;

    DeviceData->IsoEndpointListHead = NULL;

    DeviceData->Bit64AddressingCapability = FALSE;
    DeviceData->IntegratedTTSupport = IntegratedTTNotSupported;

    // Idle QH removal from async schedule
    //
    // See description in comments for EnAsyncQhIdleRemoval reg
    // setting. This default can be overridden by a reg value.
    //
    DeviceData->AsyncQhIdleRemoval = TRUE;

    // Asynchronous schedule perisistent QH control.  By default a
    // second QH is never linked into the Asynchronous schedule.
    //
    // This default can be overridden by a registry value.
    //
    DeviceData->AsyncQueueHeadDummyEnable = FALSE;

    // Enable host controller reset recovery by default.
    // Do not break into the debugger before the reset by default.
    //
    DeviceData->ResetRecoveryEnable = 1;
    DeviceData->ResetRecoveryBreak = 0;

    // Apply defaults to EHCI improvements to minimize doorbell
    // requests to hardware
    //

    // EHCI should not require a retry logic to be able to handshake
    // with the hardware through the doorbell mechanism
    //
    DeviceData->InitialDoorbellRetryEnableValue = FALSE;
    DeviceData->CurrentDoorbellRetryEnableValue = FALSE;
    DeviceData->DoorbellRetryRequired = FALSE;

    // Concurrent access to the COMMAND register bits in the async
    // and periodic schedule handling code paths without synchronization
    // can require us to enable the retry logic in doorbell handling. This
    // can lead to race conditions and need to be avoided by default.
    //

    DeviceData->PreventCmdChangesWithDoorbellPending = TRUE;

    // Enable, by default, an optimization to track active queue heads that
    // were inserted into the hardware schedule since the last enable operation
    // of the async schedule.
    //

    DeviceData->OptimizeInactiveQhFlush = TRUE;

    DeviceData->DisableTwoDoorbellWorkaround = FALSE;
    DeviceData->StrictTransitionChecksEnabled = FALSE;

    DeviceData->AsyncScheduleState = AsyncScheduleState_Disabled;
    DeviceData->AsyncDoorbellState = AsyncDoorbellState_NotWaiting;
    DeviceData->PeriodicScheduleState = PeriodicScheduleState_Disabled;

    // Read registry values which can override the defaults.
    //
    EHCI_GetRegistryParameters(DeviceData);




    DeviceData->TerminateQH = FALSE;
    
}


VOID
EHCI_DeleteDeviceData (
     __inout PDEVICE_DATA DeviceData
    )
{
    UNREFERENCED_PARAMETER(DeviceData);
}












































USB_MINIPORT_STATUS
USBMPFN
EHCI_PreStartController(
     __inout PDEVICE_DATA DeviceData,
     __in PHC_RESOURCES HcResources
    )
/*++

Routine Description:

    Pre start controller callback called by USBPORT in order for the
    miniport to query hardware capabilities before it is initialized.

    Currently identified capabilities are 

    1. 64-bit addressing capability of the host controller and
    2. Support for integrated TT which requires querying the VEN/DEV/REV
       of the host controller.

    No common buffers are allocated yet and only the memory mapped
    registers are accessible.

Arguments:

    DeviceData - Uninitialized device data structure. On return, few
                 of its fields that relate to hardware identification
                 and capabilities are initialized.

    HcResources - Hardware resource information that is obtained by
                  USBPORT from the system (PNP) to be used for identifying
                  hardware capabilties.

Return Value:

    USBMP_STATUS_SUCCESS on success.
    USBMP_STATUS_INIT_FAILURE otherwise (that will also result in ETW events being logged)

--*/
{
    USB_MINIPORT_STATUS mpStatus;
    PHC_CAPABILITIES_REGISTER hcCap;
    HCCPARAMS hcCparms;
    UCHAR fladj;
    PCEVENT_DESCRIPTOR EventDescriptor;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];

    hcCap = NULL;
    DeviceData->ControllerFlavor = HcResources->ControllerFlavor;
    DeviceData->PciConfigSpaceDisabled = HcResources->DisablePciConfigSpace;

    // Assume success unless determined otherwise.
    //
    mpStatus = USBMP_STATUS_SUCCESS;
    EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE;

    // Query VEN:DEV:REV if available as it will be required determining
    // the Integrated TT support
    //
    if (!DeviceData->PciConfigSpaceDisabled) {
        // note: flavors not used for EHCI
        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &DeviceData->Vid,
                                  0,
                                  sizeof(DeviceData->Vid));

        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &DeviceData->Dev,
                                  2,
                                  sizeof(DeviceData->Dev));

        // get the frame length adjustment value set by the BIOS
        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &fladj,
                                  0x61,
                                  sizeof(fladj));

        DeviceData->SavedFladj = fladj;
    } else {
        DeviceData->Vid = 0;
        DeviceData->Dev = 0;
        DeviceData->SavedFladj = 0;
    }

    DeviceData->Bit64AddressingCapability = 0;
    DeviceData->IntegratedTTSupport = IntegratedTTNotSupported;

    do {

        if (!DeviceData->Bit64AddressingCapabilityOverride) {

            // validate our resources (only the register space is needed here)
            //
            if ((HcResources->Flags & (HCR_MEM_REGS)) != HCR_MEM_REGS) {
                mpStatus = USBMP_STATUS_INIT_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_1;
                break;
            }

            EHCI_ASSERT(DeviceData, HcResources->DeviceRegisters != 0);
            hcCap = (PHC_CAPABILITIES_REGISTER) (HcResources->DeviceRegisters);

            LOGENTRY(DeviceData, G, '_Psr', hcCap, 0, 0);


            // Read the HCCPARAMS - Host Controller Capability Parameters
            //
            hcCparms.ul = READ_REGISTER_ULONG(&hcCap->HcCapabilityParameters.ul);

            // Make sure it looks like the controller is there before
            // proceeding.
            //
            if (hcCparms.ul == EHCI_HARDWARE_GONE) {
                DeviceData->OperationalRegisters = NULL;
                mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_2;
                break;
            }

            // 64-bit Addressing Capability. This field documents the
            // addressing range capability of this implementation. The value
            // of this field determines whether software should use the data
            // structures defined in Section 3 (32-bit) or those defined in
            // Appendix B (64-bit).
            //




            }
            else {
                DeviceData->Bit64AddressingCapability = hcCparms.Bit64Addressing ? 1 : 0;
            }
        }

        if (!DeviceData->IntegratedTTSupportRegistryOverride) {

            switch (DeviceData->ControllerFlavor) {





            default:
                DeviceData->IntegratedTTSupport = IntegratedTTNotSupported;

                //
                // Check the HRV value specified by ACPI to see if they indicated
                // Integrated TT support
                //
                if (HcResources->AcpiDeviceInformationSize > 0) {
                    if (HcResources->AcpiDeviceInformation->HardwareRevision == 0x1) {
                        DeviceData->IntegratedTTSupport = IntegratedTTSpeedInPortSc;
                    } else if (HcResources->AcpiDeviceInformation->HardwareRevision == 0x2) {
                        DeviceData->IntegratedTTSupport = IntegratedTTSpeedInHostPc;
                    }
                }
            }

        }

    } WHILE (0);

    if (mpStatus != USBMP_STATUS_SUCCESS) {

        // If the Pre Start Controller phase is failing, then emit a pair of
        // start and failure ETW events. Otherwise, do not record any ETW event
        // here and allow the Start Controller phase to do the same.
        //

        // (pre) Start Controller ETW event
        //

        RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
        USBPORT_ETW_WRITE(DeviceData,
                          &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_DISPATCH,
                          NULL,
                          1,
                          EventDataDescriptor);

        // (pre) Start Controller failure ETW event
        //

        // The first data item is the Host Controller data, which is filled
        // in EventDataDescriptor[] by USBPORT.
        //
        RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));

        EventDataDescCreate(&EventDataDescriptor[1],
                            &mpStatus,
                            sizeof(mpStatus));

        USBPORT_ETW_WRITE(DeviceData,
                          EventDescriptor,
                          NULL,
                          2,
                          EventDataDescriptor);
    }

    return mpStatus;
}

USB_MINIPORT_STATUS
USBMPFN
EHCI_StartController(
     __inout PDEVICE_DATA DeviceData,
     __in PHC_RESOURCES HcResources
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    USB_MINIPORT_STATUS mpStatus;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    PHC_CAPABILITIES_REGISTER hcCap = NULL;
    PUCHAR base;
    USBCMD cmd;
    HCLENGTHVERSION hcLengthVersion;
    ULONG capLength;
    CONFIGFLAG configFlag;
    UCHAR fladj; // fBIOS set frame length adjustment
    HCCPARAMS hcCparms;
    PCEVENT_DESCRIPTOR      EventDescriptor;
    EVENT_DATA_DESCRIPTOR   EventDataDescriptor[2];

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_DISPATCH,
                      NULL,
                      1,
                      EventDataDescriptor);

    if (HcResources->Restart) {

        // On restart, re-initialize our device data
        //
        EHCI_CreateDeviceData(DeviceData);
    }

    DeviceData->HostControllerFdo = HcResources->HostControllerFdo;
    DeviceData->TargetDeviceObject = HcResources->TargetDeviceObject;

    DeviceData->ControllerFlavor = HcResources->ControllerFlavor;

    DeviceData->PciConfigSpaceDisabled = HcResources->DisablePciConfigSpace;
    // Assume success unless determined otherwise.
    //
    mpStatus = USBMP_STATUS_SUCCESS;
    EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE;

    if (!DeviceData->PciConfigSpaceDisabled) {
        // note: flavors not used for EHCI
        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &DeviceData->Vid,
                                  0,
                                  sizeof(DeviceData->Vid));

        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &DeviceData->Dev,
                                  2,
                                  sizeof(DeviceData->Dev));

        // get the frame length adjustment value set by the BIOS
        USBPORT_READ_CONFIG_SPACE(DeviceData,
                                  &fladj,
                                  0x61,
                                  sizeof(fladj));

        DeviceData->SavedFladj = fladj;
    } else {
        DeviceData->Vid = 0;
        DeviceData->Dev = 0;
        DeviceData->SavedFladj = 0;
    }

    do {
        EHCI_ASSERT(DeviceData, HcResources->CommonBufferVa != NULL);
        // validate our resources
        if ((HcResources->Flags & (HCR_MEM_REGS | HCR_IRQ)) !=
              (HCR_MEM_REGS | HCR_IRQ)) {
            mpStatus = USBMP_STATUS_INIT_FAILURE;
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_1;
            break;
        }

        DeviceData->Interrupt = HcResources->InterruptObject;

        base = HcResources->DeviceRegisters;

        hcCap = DeviceData->CapabilitiesRegisters =
            (PHC_CAPABILITIES_REGISTER) base;

        hcLengthVersion.ul = READ_REGISTER_ULONG(&hcCap->HcLengthVersion.ul);

        LOGENTRY(DeviceData, G, '_Str', hcCap, hcLengthVersion.ul, 0);

        // Make sure it looks like the controller is there before
        // proceeding.
        //
        if (hcLengthVersion.ul == EHCI_HARDWARE_GONE) {
            DeviceData->OperationalRegisters = NULL;
            mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_2;
            break;
        }

        capLength = hcLengthVersion.HcCapLength;
        DeviceData->EhciVersion = hcLengthVersion.HcVersion;

        // Read the HCCPARAMS - Host Controller Capability Parameters
        //
        hcCparms.ul = READ_REGISTER_ULONG(&hcCap->HcCapabilityParameters.ul);

        // Get a pointer to the host controller Operational Register base.
        //
        hcOp = DeviceData->OperationalRegisters =
            (PHC_OPERATIONAL_REGISTER) (base + capLength);

        if (HcResources->DisablePciConfigSpace == FALSE) {
            DeviceData->DebugPortRegisters = EHCI_LocateDebugPortRegisters(DeviceData, HcResources);
        } else {

            // Currently EHCIs on platforms with Pci config space disabled do not support
            // DebugPortRegisters.
            //
            DeviceData->DebugPortRegisters = NULL;
        }

        // 64-bit Addressing Capability. This field documents the
        // addressing range capability of this implementation. The value
        // of this field determines whether software should use the data
        // structures defined in Section 3 (32-bit) or those defined in
        // Appendix B (64-bit).
        //
        if (!DeviceData->Bit64AddressingCapabilityOverride) {




            } else {
                DeviceData->Bit64AddressingCapability = hcCparms.Bit64Addressing ? 1 : 0;
            }
        }

































































        if (!DeviceData->IntegratedTTSupportRegistryOverride) {

            switch (DeviceData->ControllerFlavor) {





            default:
                DeviceData->IntegratedTTSupport = IntegratedTTNotSupported;

                //
                // Check the HRV value specified by ACPI to see if they indicated
                // Integrated TT support
                //
                if (HcResources->AcpiDeviceInformationSize > 0) {
                    if (HcResources->AcpiDeviceInformation->HardwareRevision == 0x1) {
                        DeviceData->IntegratedTTSupport = IntegratedTTSpeedInPortSc;
                    } else if (HcResources->AcpiDeviceInformation->HardwareRevision == 0x2) {
                        DeviceData->IntegratedTTSupport = IntegratedTTSpeedInHostPc;
                    }
                }
            }

        }

#ifdef _ARM_




































#endif

        if (DeviceData->IntegratedTTSupport != IntegratedTTNotSupported) {
            //
            // If TTs are supported, then we don't need to do the companion controller handoff either
            //
            DeviceData->CompanionControllerHandoffEnabled = FALSE;
        }












































        // After all the settings are applied, ensure that the inactive queue
        // head optimization is disabled when the idle queue head removal is
        // disabled.
        //
        if (DeviceData->AsyncQhIdleRemoval == FALSE) {
            DeviceData->OptimizeInactiveQhFlush = FALSE;
        }

        // Do the BIOS handoff if the BIOS currently has control of the
        // host controller.
        //
        mpStatus = EHCI_StopBIOS(DeviceData);
        if (mpStatus != USBMP_STATUS_SUCCESS) {
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_3;
            break;
        }

        // Initialize the host controller hardware.
        //
        mpStatus = EHCI_InitializeHardware(DeviceData);
        if (mpStatus != USBMP_STATUS_SUCCESS) {
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_4;
            break;
        }

        // Initialize the static Queue Heads and setup the host
        // controller schedule.
        //
        DeviceData->CommonBufferVa = HcResources->CommonBufferVa;
        DeviceData->CommonBufferPhys = HcResources->CommonBufferPhys;
        DeviceData->CommonBufferBytes = RegistrationPacket.CommonBufferBytes;

        DeviceData->CommonBufferBlocksVa = HcResources->CommonBufferBlocksVa[0];
        DeviceData->CommonBufferBlocksPhys = HcResources->CommonBufferBlocksPhys[0];
        DeviceData->CommonBufferBlockBytes = RegistrationPacket.CommonBufferBlockBytes[0];

        mpStatus = EHCI_InitializeSchedule(DeviceData,
                                           DeviceData->CommonBufferVa,
                                           DeviceData->CommonBufferPhys,
                                           DeviceData->CommonBufferBlocksVa,
                                           DeviceData->CommonBufferBlocksPhys);
        if (mpStatus != USBMP_STATUS_SUCCESS) {
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_START_COMPLETE_ERROR_5;
            break;
        }

        if (!DeviceData->PciConfigSpaceDisabled) {

            // Restore the saved Frame Length Adjustment Register value.
            //
            USBPORT_READ_CONFIG_SPACE(DeviceData,
                                      &fladj,
                                      0x61,
                                      sizeof(fladj));

            if (fladj != DeviceData->SavedFladj) {
                TEST_TRAP(DeviceData);

                fladj = DeviceData->SavedFladj;
                USBPORT_WRITE_CONFIG_SPACE(DeviceData,
                                           &fladj,
                                           0x61,
                                           sizeof(fladj));
            }
        }







        // set the interrupt threshold to maximum
        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        cmd.InterruptThreshold = 1;
        cmd.IntOnAsyncAdvanceDoorbell = 0;

        WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

        // start the controller
        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        LOGENTRY(DeviceData, G, '_run', cmd.ul, 0, 0);
        cmd.HostControllerRun = 1;
        cmd.IntOnAsyncAdvanceDoorbell = 0;
        WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

        DeviceData->DeviceStarted = TRUE;

        if (HcResources->Restart) {

            // port routing is not enabled by default with the new hub
            // this is so the case where the root hub fails or is disabled
            // works.  We need to take control on a re-start since the
            // implication is that the controller is running.

            // set default port routing
            configFlag.ul = 0;
            configFlag.RoutePortsToEHCI = 1;
            WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);

            DeviceData->CurrentConfigFlag.ul = configFlag.ul;

            // the neohub will handle error recovery re-power and debounce
            // in the event of a restart provided we report this correctly
            // thru the port driver.
        }

    } WHILE (0);

    // The first data item is the Host Controller data, which is filled
    // in EventDataDescriptor[] by USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));

    EventDataDescCreate(&EventDataDescriptor[1],
                        &mpStatus,
                        sizeof(mpStatus));

    USBPORT_ETW_WRITE(DeviceData,
                      EventDescriptor,
                      NULL,
                      2,
                      EventDataDescriptor);

    return mpStatus;
}


USB_MINIPORT_STATUS
USBMPFN
EHCI_GetHcCapabilities(
    __inout PDEVICE_DATA                DeviceData,
    __inout PMINIPORT_HC_CAPABILITIES   HcCapabilities
    )
/*++

Routine Description:

    Called after the HC has been started and the HC hardware is
    accessible to query the capabilities of the host controller which
    may vary between different instances of host controllers which share
    the same miniport.

Arguments:

Return Value:

    USBMP_STATUS_SUCCESS or USBMP_STATUS_FAILURE

--*/
{
    PMINIPORT_HC_CAPABILITIES_V1    hcCapabilitiesV1;
    PMINIPORT_HC_CAPABILITIES_V2    hcCapabilitiesV2;

    if (HcCapabilities->Size >= sizeof(MINIPORT_HC_CAPABILITIES_V2)) {

        hcCapabilitiesV2 = (PMINIPORT_HC_CAPABILITIES_V2)HcCapabilities;

        hcCapabilitiesV2->Size = sizeof(MINIPORT_HC_CAPABILITIES_V2);
        hcCapabilitiesV2->Version = MINIPORT_HC_CAPABILITIES_VERSION2;

        hcCapabilitiesV2->Bit64AddressingCapability =
            DeviceData->Bit64AddressingCapability;
        hcCapabilitiesV2->IntegratedTTSupported =
            (DeviceData->IntegratedTTSupport != IntegratedTTNotSupported);
        return USBMP_STATUS_SUCCESS;

    } else if (HcCapabilities->Size >= sizeof(MINIPORT_HC_CAPABILITIES_V1)) {

        hcCapabilitiesV1 = (PMINIPORT_HC_CAPABILITIES_V1)HcCapabilities;

        hcCapabilitiesV1->Size = sizeof(MINIPORT_HC_CAPABILITIES_V1);
        hcCapabilitiesV1->Version = MINIPORT_HC_CAPABILITIES_VERSION1;

        hcCapabilitiesV1->Bit64AddressingCapability =
            DeviceData->Bit64AddressingCapability;

        return USBMP_STATUS_SUCCESS;

    } else {

        return USBMP_STATUS_FAILURE;
    }
}


VOID
EHCI_ClearSavedRegisters(
    __inout PDEVICE_DATA DeviceData
    )
{
    // set all save regs to a bogus value
    DeviceData->PeriodicListBaseSave =
    DeviceData->AsyncListAddrSave =
    DeviceData->SegmentSelectorSave =
    DeviceData->CmdSave.ul =
    DeviceData->SuspendConfigFlag.ul =
    DeviceData->IntrSave.ul = 0xffffffff;
}


VOID
EHCI_AssertSavedRegisters(
    __inout PDEVICE_DATA DeviceData
    )
{
    EHCI_ASSERT(DeviceData, DeviceData->PeriodicListBaseSave != 0xffffffff);
    EHCI_ASSERT(DeviceData, DeviceData->AsyncListAddrSave != 0xffffffff);
    EHCI_ASSERT(DeviceData, DeviceData->SegmentSelectorSave != 0xffffffff);
    EHCI_ASSERT(DeviceData, DeviceData->CmdSave.ul != 0xffffffff);
    EHCI_ASSERT(DeviceData, DeviceData->SuspendConfigFlag.ul != 0xffffffff);
    EHCI_ASSERT(DeviceData, DeviceData->IntrSave.ul != 0xffffffff);
}


VOID
EHCI_SuspendController(
     __inout PDEVICE_DATA DeviceData
    )
{
    USBCMD  cmd;
    USBSTS  status;
    USBINTR intr;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    ULONG   i, p;
    USBSTS  irqStatus;
    PORTSC  portStatus;
    EVENT_DATA_DESCRIPTOR   EventDataDescriptor[1];
    BOOLEAN keepControllerOn;

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_SUSPEND_DISPATCH,
                      NULL,
                      1,
                      EventDataDescriptor);

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // Check if we need to keep the controller powered on
    keepControllerOn =
        USBPORT_IsControllerInHibernatePath(DeviceData->HostControllerFdo);

    if (!keepControllerOn) {
        for (p = 0; p < DeviceData->NumberOfPorts; p++) {
            portStatus.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);

            if ((portStatus.PortEnable == 1) &&
                (portStatus.PortOwnedByCC == 0) &&
                (portStatus.PortSuspend == 0)) {

                // EHCI (Section 4.3)
                // When system software intends to suspend the entire
                // bus, it should selectively suspend all enabled ports,
                // then shut off the host controller by setting the
                // Run/Stop bit in the USBCMD register to a zero.
                //
                // EHCI (Section 4.3.1)
                // System software places individual ports into suspend
                // mode by writing a one into the appropriate PORTSC
                // Suspend bit. Software must only set the Suspend bit
                // when the port is in the enabled state (Port Enabled
                // bit isa one) and the EHCI is the port owner (Port
                // Owner bit is a zero).
                //
                // The host controller may evaluate the Suspend bit
                // immediately or wait until a micro-frame or frame
                // boundary occurs. If evaluated immediately, the port
                // is not suspended until the current transaction (if
                // one is executing) completes. Therefore, there may be
                // several micro-frames of activity on the port until
                // the host controller evaluates the Suspend bit. The
                // host controller must evaluate the Suspend bit at
                // least every frame boundary.
                //
                portStatus.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);
                LOGENTRY(DeviceData, G, '_xPs', portStatus.ul, 0, p);

                MASK_CHANGE_BITS(portStatus);
                portStatus.PortSuspend = 1;
                WRITE_REGISTER_ULONG(&hcOp->PortRegister[p].ul,  portStatus.ul);

                // wait 1 microframe for current transaction to finish
                KeStallExecutionProcessor(125);
            }
        }
    }

    // save all volatile regs from the core power well

    EHCI_ASSERT(DeviceData, DeviceData->HcSuspendedFlag == 0);

    // since we may loose power on the controller chip (not bus)

    // the miniport is resposnible for saving HW state
    DeviceData->PeriodicListBaseSave =
        READ_REGISTER_ULONG(&hcOp->PeriodicListBase);
    DeviceData->AsyncListAddrSave =
        READ_REGISTER_ULONG(&hcOp->AsyncListAddr);
    DeviceData->SegmentSelectorSave =
        READ_REGISTER_ULONG(&hcOp->SegmentSelector);
     // preserve the state of the list enable bits
    DeviceData->CmdSave.ul =
        READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

    DeviceData->SuspendConfigFlag.ul =
        READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);

    // Read the status of all of the root hub ports.
    //
    RtlZeroMemory(DeviceData->SuspendPortStatus,
                  sizeof(DeviceData->SuspendPortStatus));

    RtlZeroMemory(DeviceData->ResumePortStatus,
                  sizeof(DeviceData->ResumePortStatus));

    for (p = 0; p < DeviceData->NumberOfPorts; p++) {

        portStatus.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);

        DeviceData->SuspendPortStatus[p] = portStatus;
    }

    // reset the PM chirp state flags for another pass at power
    // management
    DeviceData->PortPMChirp = 0;

    // Save away the command register
    // DeviceData->SuspendCommandReg.us =
    //    command.us = READ_PORT_USHORT(&reg->UsbCommand.us);

    // clear the int on async advance doorbell
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // Stop the controller
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    LOGENTRY(DeviceData, G, '_st1', cmd.ul, 0, 0);
    if (!keepControllerOn) {
        cmd.HostControllerRun = 0;
    }
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // ack any interrupts that may be left over from the halt
    // process.  The controller should not generate any new
    // interrupts when it is stopped. For some reason the NEC
    // controller generates a doorbel interrupt on halt.

    // wait 1 microframe
    KeStallExecutionProcessor(125);
    irqStatus.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
    // just look at the IRQ status bits
    irqStatus.ul &= HcInterruptStatusMask;
    if (irqStatus.ul != 0)  {
        WRITE_REGISTER_ULONG(&hcOp->UsbStatus.ul, irqStatus.ul);
    }

    // mask off all interrupts now
    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul, 0);

    if (!keepControllerOn) {
        // The EHCI specification says "The Host Controller must halt within
        // 16 micro-frames after software clears the Run bit. The HC Halted
        // bit in the status register indicates when the Host Controller has
        // finished its pending pipelined transactions and has entered the
        // stopped state."
        //
        for (i=0; i<10; i++) {
            status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
            if (status.HcHalted) {
                break;
            }
            USBPORT_WAIT(DeviceData, 1);
        }

        if (status.HcHalted != 1) {
            // hardware is messed up
            DeviceData->SuspendHcNotHalted = 1;
            TEST_TRAP(DeviceData);
        } else {
            DeviceData->SuspendHcNotHalted = 0;
        }

        //if (!status.HCHalted) {
        //
        //    // Can't get the HCHalted bit to stick, so reset the controller.
        //    command.GlobalReset = 1;
        //    WRITE_PORT_USHORT(&reg->UsbCommand.us, command.us);
        //
        //    USBPORT_WAIT(DeviceData, 10);
        //
        //    command.GlobalReset = 0;
        //    WRITE_PORT_USHORT(&reg->UsbCommand.us, command.us);

        //    // Re-enable interrupts, since they are zero'd out on reset.
        //    WRITE_PORT_USHORT(&reg->UsbInterruptEnable.us, DeviceData->EnabledInterrupts.us);
        //
        //}

    } else {
        DeviceData->SuspendHcNotHalted = 1;
    }

    // enable the port change interrupt, this allows us to wake
    // in the selective suspend case
    DeviceData->IntrSave.ul = DeviceData->EnabledInterrupts.ul;

    intr.ul = READ_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul);
    intr.PortChangeDetect = 1;
    DeviceData->EnabledInterrupts.ul = intr.ul;

    WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul, intr.ul);
    DeviceData->HcSuspendedFlag = 1;

    if (RegistrationPacket.USBPORTSVC_IgnoreResumeSignaling(DeviceData) == FALSE) {

        for (p = 0; p < DeviceData->NumberOfPorts; p++) {

            portStatus.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);

            // If resume signaling occurred concurrent with suspend of the root hub and
            // the PortSuspendChange bit was set, we now need to clear it we have suspended
            // the port.  This will avoid falsely reporting a suspend change on the port
            // when we resume.
            if (!keepControllerOn) {
                CLEAR_BIT(DeviceData->PortSuspendChange, p);
            }

            // It is possible that resume signaling occurred while portchangedetect was disabled,
            // leaving the port with ForcePortResume==1 and no resume event being detected.
            // Set InvalidateRootHub to TRUE so we invalidate the root hub on the next call to
            // EHCI_CheckInvalidateRootHub.
            if (portStatus.ForcePortResume == 1) {
                DeviceData->InvalidateRootHub = TRUE;
            }
        }
    }

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_SUSPEND_COMPLETE,
                      NULL,
                      1,
                      EventDataDescriptor);
}


USB_MINIPORT_STATUS
EHCI_ResumeController(
      __inout PDEVICE_DATA DeviceData
    )
{
    USBCMD      cmd;
    USBSTS      status;
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    CONFIGFLAG  configFlag;
    CONFIGFLAG  lastConfigFlag;
    ULONG       p;
    PORTSC      portStatus;
    USB_MINIPORT_STATUS     mpStatus;
    USHORT                  NumberOfPorts;
    PCEVENT_DESCRIPTOR      EventDescriptor;
    EVENT_DATA_DESCRIPTOR   EventDataDescriptor[5];

    // The first (and only in this case) data item is the Host
    // Controller data, which is filled in EventDataDescriptor[] by
    // USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));
    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_DISPATCH,
                      NULL,
                      1,
                      EventDataDescriptor);

    hcOp = DeviceData->OperationalRegisters;

    // Assume success unless determined otherwise.
    //
    mpStatus = USBMP_STATUS_SUCCESS;
    EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE;

    do {
        if (hcOp == NULL) {
            mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_1;
            break;
        }

        EHCI_ASSERT(DeviceData, DeviceData->HcSuspendedFlag != 0);

        status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);

        if (status.HcHalted != 1) {
            LOGENTRY(DeviceData, G, '_RNH', status.ul, 0, 0);

            if (!DeviceData->SuspendHcNotHalted) {
                // If the HcHalted bit was successfully set during
                // Suspend Controller then consider it to be an error if
                // the HcHalted bit is not still set during Resume
                // Controller.
                mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_2;
                break;
            }
        }

        // restore volitile regs
        configFlag.ul = READ_REGISTER_ULONG(&hcOp->ConfigFlag.ul);
        lastConfigFlag.ul = DeviceData->CurrentConfigFlag.ul;
        if (configFlag.RoutePortsToEHCI == 0 &&
            lastConfigFlag.RoutePortsToEHCI == 1) {

            // we have a reset of the cfg flag

            configFlag.RoutePortsToEHCI = 1;
            DeviceData->CurrentConfigFlag.ul = configFlag.ul;
            WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);

            mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
            EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_3;
            break;
        }

        // Read the status of all of the root hub ports.
        //
        for (p = 0; p < DeviceData->NumberOfPorts; p++) {

            portStatus.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);

            DeviceData->ResumePortStatus[p] = portStatus;

            LOGENTRY(DeviceData, G, '_Prp', p, portStatus.ul, 0);
        }

        // Check power bits on root ports -- these are non-volitile so if
        // the are zero don't try to resume.
        //
        for (p = 0; p < DeviceData->NumberOfPorts; p++) {

            portStatus = DeviceData->ResumePortStatus[p];

            if (portStatus.PortPower == 0) {

                mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_4;
                break;
            }

            // If the port ownership is assigned to the CC and it was not
            // assigned to the CC during suspend then the BIOS must have
            // messed with the port during resume.  Fail and reset the
            // controller in this case.
            //
            if (portStatus.PortOwnedByCC &&
                !DeviceData->SuspendPortStatus[p].PortOwnedByCC) {

                mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_5;
                break;
            }

            // Fail the resume if any of the root ports indicated an overcurrent
            if (portStatus.OvercurrentChange) {

                mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
                EventDescriptor = &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_RESUME_COMPLETE_ERROR_6;
                break;
            }

        }

        if (mpStatus != USBMP_STATUS_SUCCESS) {
            break;
        }

        EHCI_AssertSavedRegisters(DeviceData);

        // restore volitile regs
        WRITE_REGISTER_ULONG(&hcOp->SegmentSelector, DeviceData->SegmentSelectorSave);
        WRITE_REGISTER_ULONG(&hcOp->PeriodicListBase, DeviceData->PeriodicListBaseSave);
        WRITE_REGISTER_ULONG(&hcOp->AsyncListAddr, DeviceData->AsyncListAddrSave);







        // start the controller
        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
        LOGENTRY(DeviceData, G, '_run', cmd.ul, 0, 0);
        cmd.HostControllerRun = 1;

        // restore volatile cmd bits
        cmd.AsyncScheduleEnable = DeviceData->CmdSave.AsyncScheduleEnable;
        cmd.PeriodicScheduleEnable = DeviceData->CmdSave.PeriodicScheduleEnable;
        cmd.InterruptThreshold = DeviceData->CmdSave.InterruptThreshold;
        cmd.IntOnAsyncAdvanceDoorbell = 0;

        WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

        DeviceData->EnabledInterrupts.ul = DeviceData->IntrSave.ul;
        WRITE_REGISTER_ULONG(&hcOp->UsbInterruptEnable.ul,
                             DeviceData->EnabledInterrupts.ul);

        EHCI_ClearSavedRegisters(DeviceData);
        DeviceData->HcSuspendedFlag = 0;

    } WHILE (0);

    // The first data item is the Host Controller data, which is filled
    // in EventDataDescriptor[] by USBPORT.
    //
    RtlZeroMemory(EventDataDescriptor, sizeof(EventDataDescriptor));

    EventDataDescCreate(&EventDataDescriptor[1],
                        &mpStatus,
                        sizeof(mpStatus));

    NumberOfPorts = (USHORT)DeviceData->NumberOfPorts;

    EventDataDescCreate(&EventDataDescriptor[2],
                        &NumberOfPorts,
                        sizeof(USHORT));

    EventDataDescCreate(&EventDataDescriptor[3],
                        DeviceData->SuspendPortStatus,
                        NumberOfPorts * sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[4],
                        DeviceData->ResumePortStatus,
                        NumberOfPorts * sizeof(ULONG));

    USBPORT_ETW_WRITE(DeviceData,
                      EventDescriptor,
                      NULL,
                      5,
                      EventDataDescriptor);

    return mpStatus;
}


USB_MINIPORT_STATUS
EHCI_OpenEndpoint(
      __inout PDEVICE_DATA DeviceData,
      __inout PENDPOINT_PARAMETERS EndpointParameters,
      __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_OpenEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held if the
    USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE flag is not set.

Arguments:

Return Value:

--*/
{
    USB_MINIPORT_STATUS mpStatus;
    LARGE_INTEGER       time;

    EndpointData->Sig = SIG_EP_DATA;
    // save a copy of the parameters
    EndpointData->Parameters = *EndpointParameters;
    EndpointData->Flags = 0;
    EndpointData->SmodeTransfers = 0;
    InitializeListHead(&EndpointData->SmodeTransferListHead);
    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Flink == NULL);
    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Blink == NULL);

    InsertTailList(&DeviceData->AllEpListHead, &EndpointData->AllEpLink);

    // EndpointData->EndpointInScheduleCount is initialized to ZERO and
    // DeviceData->CurrentAsyncEnableCount is initialized to ONE
    // so that an unexpected match does not happen when both these counters
    // are set to ZERO without the endpoint/QH being ever added to the schedule
    //

    EndpointData->EndpointInScheduleCount = 0;

    // initial state is not closed, we should get a close for any endpoint that
    // is opened successfully.
    //closed=FALSE
    EndpointData->EpStatus = ENDPOINT_STATUS_RUN;
    EndpointData->CurEpState = ENDPOINT_PAUSE;
    EndpointData->HwSyncState = Qh_Not_Busy;
    EndpointData->TransferTypeFlag = 0;
    EndpointData->PendingSoftRetryCount = (USHORT) DeviceData->SoftPerSecRetryCount;
    EndpointData->PendingMissedMicroframeRetryCount
        = (USHORT) DeviceData->MissedMicroframePerSecRetryCount;
    KeQuerySystemTime(&time);
    EndpointData->PendingSoftRetryCountInitTimeMs = (USHORT) (time.QuadPart/10000);
    EndpointData->PendingMissedMicroframeRetryCountInitTimeMs = (USHORT) (time.QuadPart/10000);

    switch (EndpointParameters->TransferType) {

    case Control:

        EndpointData->TransferTypeFlag = control_async;

        mpStatus = EHCI_OpenBulkOrControlEndpoint(
                DeviceData,
                TRUE,
                EndpointParameters,
                EndpointData);

        break;

    case Interrupt:

        EndpointData->TransferTypeFlag = int_periodic;

        mpStatus = EHCI_OpenInterruptEndpoint(
                    DeviceData,
                    EndpointParameters,
                    EndpointData);

        break;

    case Bulk:

        EndpointData->TransferTypeFlag = bulk_async;

        mpStatus = EHCI_OpenBulkOrControlEndpoint(
                DeviceData,
                FALSE,
                EndpointParameters,
                EndpointData);

        break;

    case Isochronous:

        EndpointData->TransferTypeFlag = isoch_periodic;

        if (EndpointParameters->DeviceSpeed == HighSpeed) {
            mpStatus = EHCI_OpenHsIsochronousEndpoint(
                        DeviceData,
                        EndpointParameters,
                        EndpointData);
        } else {
            mpStatus = EHCI_OpenIsochronousEndpoint(
                        DeviceData,
                        EndpointParameters,
                        EndpointData);
        }
        break;

    default:
        TEST_TRAP(DeviceData);
        mpStatus = USBMP_STATUS_NOT_SUPPORTED;
    }

    if (mpStatus != USBMP_STATUS_SUCCESS) {

        RemoveEntryList(&EndpointData->AllEpLink);
        EndpointData->AllEpLink.Flink = NULL;
        EndpointData->AllEpLink.Blink = NULL;
        // closed=TRUE
        EndpointData->CurEpState = ENDPOINT_CLOSED;
    }

    return mpStatus;
}


VOID
EHCI_CloseEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_CloseEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held if the
    USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE flag is not set.

Arguments:

Return Value:

--*/
{
    // sanity check that the endpoint is not in the schedule

    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    KIRQL irql;
    //!closed
    EHCI_ASSERT(DeviceData, EndpointData->CurEpState != ENDPOINT_CLOSED);
    LOGENTRY(DeviceData, G, '_clo', 0, 0, EndpointData);

#if DBG
    EHCI_DbgCheckEndpoints(DeviceData);
#endif
    qh = EndpointData->QueueHead;

    switch(EndpointData->Parameters.TransferType) {
    case Interrupt:
    case Control:
    case Bulk:


        LOGENTRY(DeviceData, G, '_CLO', qh, 0, EndpointData);

        EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);
        EHCI_ASSERT(DeviceData, EndpointData->HwSyncState == Qh_Not_Busy);
        // usbport will still assume that the ep is not on hw if sufficient time
        // has elapsed.  It should probably only do this for interrupt eps.

        if (DeviceData->EpAtTheDoor == EndpointData) {
            EHCI_ASSERTMSG(DeviceData, "EpAtTheDoor same as EndpointData", FALSE);
            //EndpointData->HwSyncState = Qh_Not_Busy;
        }

        if (EndpointData->AcfLink.Flink != NULL &&
            EndpointData->AcfLink.Blink != NULL) {

            EHCI_ASSERTMSG(DeviceData, "Invalid State on CloseEndpoint", FALSE);

            //EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Blink != NULL);
            //EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Flink != NULL);
            //RemoveEntryList(&EndpointData->AcfLink);

            //EndpointData->AcfLink.Blink = NULL;
            //EndpointData->AcfLink.Flink = NULL;
            //EndpointData->HwSyncState = Qh_Not_Busy;
        }

        EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

        EHCI_ASSERT(DeviceData, (qh->QhFlags & EHCI_QH_FLAG_QH_REMOVED));
        break;
    }

    //TEST_TRAP(DeviceData);

    LOGENTRY(DeviceData, G, '_CL1', qh, EndpointData->AllEpLink.Flink, EndpointData->AllEpLink.Blink);
    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Flink != NULL);
    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Blink != NULL);

    RemoveEntryList(&EndpointData->AllEpLink);
    EndpointData->AllEpLink.Flink = NULL;
    EndpointData->AllEpLink.Blink = NULL;

    //closed=TRUE;
    EndpointData->CurEpState = ENDPOINT_CLOSED;

#if DBG
    EHCI_DbgCheckEndpoints(DeviceData);
#endif
    LOGENTRY(DeviceData, G, '_clo', 0, 1, EndpointData);

}


VOID
EHCI_DbgFreeEndpoint(
      __inout PDEVICE_DATA DeviceData,
      __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_DbgFreeEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    PENDPOINT_DATA epd;
    PLIST_ENTRY le;
    // sanity check that the endpoint is not in the schedule

    LOGENTRY(DeviceData, G, '_fep', 0, 0, EndpointData);

    EHCI_ASSERT(DeviceData, EndpointData->Sig == SIG_EP_DATA);
    EHCI_ASSERT(DeviceData, EndpointData->CurEpState == ENDPOINT_CLOSED);

    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Flink == NULL);
    EHCI_ASSERT(DeviceData, EndpointData->AllEpLink.Blink == NULL);

    EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Flink == NULL);
    EHCI_ASSERT(DeviceData, EndpointData->AcfLink.Blink == NULL);

    le = DeviceData->AllEpListHead.Flink;
    EHCI_ASSERT(DeviceData, le != NULL);

    while (le != &DeviceData->AllEpListHead) {

        epd = (PENDPOINT_DATA) CONTAINING_RECORD(
                     le,
                     struct _ENDPOINT_DATA,
                     AllEpLink);

        LOGENTRY(DeviceData, G, '_cke', le, 0, epd);

        EHCI_ASSERT(DeviceData, epd != EndpointData);
        EHCI_ASSERT(DeviceData, epd->Sig == SIG_EP_DATA);
        EHCI_ASSERT(DeviceData, epd->CurEpState != ENDPOINT_CLOSED);


        le = le->Flink;
        EHCI_ASSERT(DeviceData, le != NULL);
    }

    LOGENTRY(DeviceData, G, '_fep', 0, 1, EndpointData);
}


VOID
EHCI_DbgCheckEndpoints(
      __inout PDEVICE_DATA DeviceData
    )
/*++



--*/
{
    PENDPOINT_DATA epd;
    PLIST_ENTRY le;
    // sanity check that the endpoint is not in the schedule

    le = DeviceData->AllEpListHead.Flink;
    LOGENTRY(DeviceData, G, '_ckk', le, 0, DeviceData);

    EHCI_ASSERT(DeviceData, le != NULL);

    while (le != &DeviceData->AllEpListHead) {


        epd = (PENDPOINT_DATA) CONTAINING_RECORD(
                     le,
                     struct _ENDPOINT_DATA,
                     AllEpLink);

        LOGENTRY(DeviceData, G, '_ckx', le, le->Flink, epd);

        EHCI_ASSERT(DeviceData, epd->Sig == SIG_EP_DATA);
        EHCI_ASSERT(DeviceData, epd->CurEpState != ENDPOINT_CLOSED);


        le = le->Flink;
        EHCI_ASSERT(DeviceData, le != NULL);
    }

    LOGENTRY(DeviceData, G, '_ckk', 0, 1, DeviceData);


}


USB_MINIPORT_STATUS
EHCI_PokeEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_PARAMETERS EndpointParameters,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_PokeEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    LOGENTRY(DeviceData, G, '_Pok', EndpointData,
             EndpointParameters, 0);

    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch(EndpointData->Parameters.TransferType) {
            case Interrupt:
            case Control:
            case Bulk:
                return EHCI_PokeAsyncEndpoint(DeviceData,
                                              EndpointParameters,
                                              EndpointData);
            case Isochronous:
                return EHCI_PokeIsoEndpoint(DeviceData,
                                            EndpointParameters,
                                            EndpointData);
        }
    }

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_RebalanceEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_RebalanceEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch (EndpointParameters->TransferType) {
            case Interrupt:
                EHCI_RebalanceInterruptEndpoint(DeviceData,
                                                EndpointParameters,
                                                EndpointData);
                break;

            case Isochronous:
                EHCI_RebalanceIsoEndpoint(DeviceData,
                                          EndpointParameters,
                                          EndpointData);
                break;
        }
    }
}


USB_MINIPORT_STATUS
EHCI_QueryEndpointRequirements(
     __inout PDEVICE_DATA DeviceData,
     __in PENDPOINT_PARAMETERS EndpointParameters,
     __out PENDPOINT_REQUIREMENTS EndpointRequirements
    )
/*++

Routine Description:

    compute how much common buffer we will need
    for this endpoint

    This MINIPORT_QueryEndpointRequirements() routine is always called
    with the USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    switch (EndpointParameters->TransferType) {

    case Control:

        EndpointRequirements->MinCommonBufferBytes =
            HCD_QUEUEHEAD_DESCRIPTOR_PAD +
                TDS_PER_CONTROL_ENDPOINT*sizeof(HCD_TRANSFER_DESCRIPTOR);

        EndpointRequirements->MaximumTransferSize =
            MAX_CONTROL_TRANSFER_SIZE;
        break;

    case Interrupt:

        // in slot mode use same values as bulk
        EndpointRequirements->MinCommonBufferBytes =
            HCD_QUEUEHEAD_DESCRIPTOR_PAD + HCD_QUEUEHEAD_DESCRIPTOR_PAD +
                TDS_PER_BULK_ENDPOINT*sizeof(HCD_TRANSFER_DESCRIPTOR);

        EndpointRequirements->MaximumTransferSize = MAX_BULK_TRANSFER_SIZE;
        break;

    case Bulk:

        //
        // TDS_PER_ENDPOINT limits the largest single transfer we can handle.
        //

        //
        // Total memory needed includes: (see EHCI_OpenBulkOrControlEndpoint)
        // (reserved) HCD_QUEUEHEAD_DESCRIPTOR_PAD  - used for DMA sanity checking
        // (qHead) HCD_QUEUEHEAD_DESCRIPTOR_PAD
        // (qTdList)[TDS_PER_BULK_ENDPOINT] sizeof(HCD_TRANSFER_DESCRIPTOR)
        //
        EndpointRequirements->MinCommonBufferBytes =
            HCD_QUEUEHEAD_DESCRIPTOR_PAD + HCD_QUEUEHEAD_DESCRIPTOR_PAD +
                TDS_PER_BULK_ENDPOINT*sizeof(HCD_TRANSFER_DESCRIPTOR);

        EndpointRequirements->MaximumTransferSize = MAX_BULK_TRANSFER_SIZE;
        break;

     case Isochronous:

        if (EndpointParameters->DeviceSpeed == HighSpeed) {
            EndpointRequirements->MinCommonBufferBytes =
                    USBEHCI_MAX_FRAME*sizeof(HCD_HSISO_TRANSFER_DESCRIPTOR);

            EndpointRequirements->MaximumTransferSize =
                MAX_HSISO_TRANSFER_SIZE;
        } else {
            // TDS_PER_ENDPOINT TDs plus an ED
            EndpointRequirements->MinCommonBufferBytes =
                    TDS_PER_ISO_ENDPOINT*sizeof(HCD_SI_TRANSFER_DESCRIPTOR);

            EndpointRequirements->MaximumTransferSize =
                MAX_ISO_TRANSFER_SIZE;
        }
        break;

    default:
        // should not reach this code path
        EndpointRequirements->MinCommonBufferBytes = 0;
        EndpointRequirements->MaximumTransferSize = 0;
        USBPORT_BUGCHECK(DeviceData);
    }

    return USBMP_STATUS_SUCCESS;
}


VOID
EHCI_PollEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_PollEndpoint() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/

{
    EHCI_ASSERT(DeviceData, EndpointData->CurEpState != ENDPOINT_CLOSED);

    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch(EndpointData->Parameters.TransferType) {

            case Control:
                EHCI_PollControlEndpoint(DeviceData, EndpointData);
                break;
            case Bulk:
                EHCI_PollPerAsyEndpoint(DeviceData, EndpointData, TRUE);
                break;
            case Interrupt:
                EHCI_PollPerAsyEndpoint(DeviceData, EndpointData, FALSE);
                break;
            case Isochronous:
                EHCI_PollIsoEndpoint(DeviceData, EndpointData);
                break;
        }

    } else {

        switch(EndpointData->Parameters.TransferType) {

            case Control:
            case Bulk:
            case Interrupt:
                break;

            case Isochronous:
                EHCI_PollResetIsoEndpoint(DeviceData, EndpointData);
                break;
        }
    }
}


PHCD_TRANSFER_DESCRIPTOR
EHCI_AllocTd(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    Allocate a TD from an endpoints pool

Arguments:

Return Value:

--*/
{
    ULONG i;
    PHCD_TRANSFER_DESCRIPTOR td;

    for (i=0; i<EndpointData->TdCount; i++) {
        td = &EndpointData->TdList->Td[i];

        if (!TEST_FLAG(td->Flags, TD_FLAG_BUSY)) {
            SET_FLAG(td->Flags, TD_FLAG_BUSY);
            LOGENTRY(DeviceData, G, '_aTD', td, 0, 0);
            EndpointData->FreeTds--;
            return td;
        }
    }

    // we should always find one
    EHCI_ASSERTMSG(DeviceData, "Td not found", FALSE);
    USBPORT_BUGCHECK(DeviceData);

    return NULL;
}


VOID
EHCI_SetEndpointStatus(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     MP_ENDPOINT_STATUS Status
    )
/*++

Routine Description:

    This MINIPORT_SetEndpointStatus() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch (EndpointData->Parameters.TransferType) {
            case Control:
            case Bulk:
            case Interrupt:
                EHCI_SetAsyncEndpointStatus(DeviceData,
                                            EndpointData,
                                            Status);
                break;
            case Isochronous:
                // nothing to do for iso
                break;

            default:
                USBPORT_BUGCHECK(DeviceData);
        }
    }
}


MP_ENDPOINT_STATUS
EHCI_GetEndpointStatus(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_GetEndpointStatus() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch (EndpointData->Parameters.TransferType) {
            case Control:
            case Bulk:
            case Interrupt:
                return EHCI_GetAsyncEndpointStatus(DeviceData,
                    EndpointData);
                break;
        }

        // return RUNNING for iso

        return ENDPOINT_STATUS_RUN;

    } else {

        return ENDPOINT_STATUS_RUN;
    }
}


VOID
EHCI_SetEndpointState(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     MP_ENDPOINT_STATE NewState
    )
/*++

Routine Description:

    This MINIPORT_SetEndpointState() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    PHCD_QUEUEHEAD_DESCRIPTOR qh = NULL;

    LOGENTRY(DeviceData, G, '_ses', EndpointData, EndpointData->CurEpState, NewState);

    if (EndpointData->CurEpState == NewState) {
        TEST_TRAP(DeviceData);
    }

    EHCI_ASSERT(DeviceData, EndpointData->CurEpState != ENDPOINT_CLOSED);

    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        switch (EndpointData->Parameters.TransferType) {
            case Control:
            case Bulk:
            case Interrupt:
#if DBG
                // This code validates the current endpoint state against the queue head link status.
                qh = EndpointData->QueueHead;
                EHCI_ASSERT(DeviceData, qh != NULL);

                switch(EndpointData->CurEpState) {
                    case ENDPOINT_REMOVE:
                        EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                        EHCI_ASSERT(DeviceData, TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_QH_REMOVED));
                        break;

                    case ENDPOINT_PAUSE:
                        EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                        break;

                    case ENDPOINT_ACTIVE:
                        EHCI_ASSERT(DeviceData, TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                        break;

                    case ENDPOINT_CLOSED:
                        break;

                    default:
                        TEST_TRAP(DeviceData);
                }

#endif
                EHCI_SetAsyncEndpointState(DeviceData,
                                           EndpointData,
                                           NewState);

                EndpointData->CurEpState = NewState;
                break;

            case Isochronous:

                EHCI_SetIsoEndpointState(DeviceData,
                                         EndpointData,
                                         NewState);

                EndpointData->CurEpState = NewState;
                EndpointData->HwSyncState = Qh_Not_Busy;
                break;

            default:
                USBPORT_BUGCHECK(DeviceData);
        }

    } else {

        switch (EndpointData->Parameters.TransferType) {
            case Control:
            case Bulk:
            case Interrupt:
                qh = EndpointData->QueueHead;
                switch (NewState) {
                    case ENDPOINT_ACTIVE:
                        break;
                    case ENDPOINT_PAUSE:
                        break;
                    case ENDPOINT_REMOVE:
                        SET_FLAG(qh->QhFlags, EHCI_QH_FLAG_QH_REMOVED);
                        break;
                    default:
                        break;
                }
                break;

            case Isochronous:
                break;

            default:
                break;
        }

        EndpointData->CurEpState = NewState;
    }
}


MP_ENDPOINT_STATE
EHCI_GetEndpointState(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This MINIPORT_GetEndpointState() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

Return Value:

--*/
{
    MP_ENDPOINT_STATE currentState;

    currentState = EndpointData->CurEpState;

#if DBG
    if (!TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {

        PHCD_QUEUEHEAD_DESCRIPTOR qh;

        qh = EndpointData->QueueHead;

        if (qh != NULL) {
            switch(currentState) {
                case ENDPOINT_REMOVE:
                    EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                    EHCI_ASSERT(DeviceData, TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_QH_REMOVED));
                    break;
                case ENDPOINT_PAUSE:
                    EHCI_ASSERT(DeviceData, !TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                    break;
                case ENDPOINT_ACTIVE:
                    EHCI_ASSERT(DeviceData, TEST_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE));
                    break;
                default:
                    TEST_TRAP(DeviceData);
            }
        }
    }
#endif

    LOGENTRY(DeviceData, G, '_ges', EndpointData, 0, currentState);

    return currentState;
}


VOID
EHCI_PollController(
     __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PHC_OPERATIONAL_REGISTER hcOp = NULL;
    PORTSC port;
    ULONG p;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    for (p=0; p<DeviceData->NumberOfPorts ; p++) {
        port.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[p].ul);
        LOGENTRY(DeviceData, G, '_Pop', p, port.ul, 0);
        if (port.PortConnectChange == 1) {
            USBPORT_INVALIDATE_ROOTHUB(DeviceData);
        }
    }
}


USB_MINIPORT_STATUS
EHCI_SubmitTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferParameters,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PTRANSFER_SG_LIST TransferSGList
    )
/*++

Routine Description:

    Entry point for all bulk, interrupt and control tarnsfers

    This MINIPORT_SubmitTransfer() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    USB_MINIPORT_STATUS mpStatus = USBMP_STATUS_NOT_SUPPORTED;

    if (TEST_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET)) {
        return USBMP_STATUS_BUSY;
    }

    EHCI_ASSERT(DeviceData, EndpointData->CurEpState != ENDPOINT_CLOSED);
    // init the context
    RtlZeroMemory(TransferContext, sizeof(*TransferContext));
    TransferContext->Sig = SIG_EHCI_TRANSFER;
    TransferContext->UsbdStatus = USBD_STATUS_SUCCESS;
    TransferContext->EndpointData = EndpointData;
    TransferContext->TransferParameters = TransferParameters;

    switch (EndpointData->Parameters.TransferType) {
        case Control:
            mpStatus = EHCI_ControlTransfer(DeviceData,
                                            EndpointData,
                                            TransferParameters,
                                            TransferContext,
                                            TransferSGList);
            break;

        case Interrupt:
            mpStatus = EHCI_InterruptTransfer(DeviceData,
                                              EndpointData,
                                              TransferParameters,
                                              TransferContext,
                                              TransferSGList);
            break;

        case Bulk:
            mpStatus = EHCI_BulkTransfer(DeviceData,
                                         EndpointData,
                                         TransferParameters,
                                         TransferContext,
                                         TransferSGList);
            break;

        default:
            TEST_TRAP(DeviceData);
    }

    return mpStatus;
}


VOID
EHCI_AbortTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PULONG BytesTransferred
    )
/*++

Routine Description:

    This MINIPORT_AbortTransfer() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.

--*/
{
    EHCI_ASSERT(DeviceData, EndpointData->CurEpState != ENDPOINT_CLOSED);

    switch (EndpointData->Parameters.TransferType) {
        case Control:
            EHCI_AbortControlTransfer(DeviceData,
                                      EndpointData,
                                      TransferContext);
            break;

        case Interrupt:
        case Bulk:
            EHCI_sMode_AbortAsyncTransfer(DeviceData,
                                          EndpointData,
                                          TransferContext);
            break;
        default:
            EHCI_AbortIsoTransfer(DeviceData,
                                  EndpointData,
                                  TransferContext);
            break;
    }

    *BytesTransferred = TransferContext->BytesTransferred;
}


USB_MINIPORT_STATUS
EHCI_GetIdleStatus(
    PDEVICE_DATA DeviceData,
    ULONG ParameterLength,
    PVOID Parameters
    )
{
    ULONG status_bits = 0;
    USB_MINIPORT_STATUS mps;

    LOGENTRY(DeviceData, G, '_is1', ParameterLength, Parameters, 0);

    do {
        PULONG p;

        if (ParameterLength < sizeof(ULONG)) {
            mps = USBMP_STATUS_FAILURE;
            break;
        }

        if (Parameters == NULL) {
            mps = USBMP_STATUS_FAILURE;
            break;
        }

        // indicate this is a high speed controller
        status_bits |= USBHC_HIGHSPEED;

        if (DeviceData->AsyncScheduleState != AsyncScheduleState_Disabled) {
            status_bits |= USBHC_ASYNC_LIST_ENABLE;
        }

        if (DeviceData->PeriodicScheduleState != PeriodicScheduleState_Disabled) {
            status_bits |= USBHC_PERIODIC_LIST_ENABLE;
        }

        if ((DeviceData->HcSuspendedFlag == 0) ||
            (DeviceData->SuspendHcNotHalted)) {
            status_bits |= USBHC_RUN;
        }

        p = Parameters;
        if (p) {
            *p = status_bits;
        }

        mps = USBMP_STATUS_SUCCESS;

    } WHILE (0);

    LOGENTRY(DeviceData, G, '_isX', status_bits, mps, 0);

    return mps;
}


USB_MINIPORT_STATUS
EHCI_EtwLogDeviceData(
      __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    Logs HC diagnostics and statistics information to ETW for use by diagostics and test tools

--*/
{
    PEVENT_DATA_DESCRIPTOR   EventDataDescriptor;

    EventDataDescriptor = EHCI_AllocNpPool(sizeof(EVENT_DATA_DESCRIPTOR) * 50);

    if (EventDataDescriptor == NULL) {
        return USBMP_STATUS_SUCCESS;
    }
    
    // The first data item is the Host Controller data, 
    // which is filled in EventDataDescriptor[] by USBPORT.

    // The order in which these events are logged must match the order of
    // the fields defined in the tid_USBPORT_HC_Miniport_Ehci_Device_Data template
    // as defined in the manifest
    EventDataDescCreate(&EventDataDescriptor[1],
                        &DeviceData->HcSuspendedFlag,
                        sizeof(DeviceData->HcSuspendedFlag));

    EventDataDescCreate(&EventDataDescriptor[2],
                        &DeviceData->SuspendHcNotHalted,
                        sizeof(DeviceData->SuspendHcNotHalted));

    EventDataDescCreate(&EventDataDescriptor[3],
                        &DeviceData->HostSystemErrorCount,
                        sizeof(DeviceData->HostSystemErrorCount));

    EventDataDescCreate(&EventDataDescriptor[4],
                        &DeviceData->HostSystemErrorResetNeeded,
                        sizeof(DeviceData->HostSystemErrorResetNeeded));

    EventDataDescCreate(&EventDataDescriptor[5],
                        &DeviceData->StatAsyncEnableCount,
                        sizeof(DeviceData->StatAsyncEnableCount));

    EventDataDescCreate(&EventDataDescriptor[6],
                        &DeviceData->StatAsyncDisableCount,
                        sizeof(DeviceData->StatAsyncDisableCount));

    EventDataDescCreate(&EventDataDescriptor[7],
                        &DeviceData->StatPeriodicEnableCount,
                        sizeof(DeviceData->StatPeriodicEnableCount));

    EventDataDescCreate(&EventDataDescriptor[8],
                        &DeviceData->StatPeriodicDisableCount,
                        sizeof(DeviceData->StatPeriodicDisableCount));

    EventDataDescCreate(&EventDataDescriptor[9],
                        &DeviceData->StatAsyncEnableTimeoutCount,
                        sizeof(DeviceData->StatAsyncEnableTimeoutCount));

    EventDataDescCreate(&EventDataDescriptor[10],
                        &DeviceData->StatAsyncDisableTimeoutCount,
                        sizeof(DeviceData->StatAsyncDisableTimeoutCount));

    EventDataDescCreate(&EventDataDescriptor[11],
                        &DeviceData->StatPeriodicEnableTimeoutCount,
                        sizeof(DeviceData->StatPeriodicEnableTimeoutCount));

    EventDataDescCreate(&EventDataDescriptor[12],
                        &DeviceData->StatPeriodicDisableTimeoutCount,
                        sizeof(DeviceData->StatPeriodicDisableTimeoutCount));

    EventDataDescCreate(&EventDataDescriptor[13],
                        &DeviceData->StatRingDoorbellDisabledCount,
                        sizeof(DeviceData->StatRingDoorbellDisabledCount));

    EventDataDescCreate(&EventDataDescriptor[14],
                        &DeviceData->StatRingDoorbellDisablingCount,
                        sizeof(DeviceData->StatRingDoorbellDisablingCount));

    EventDataDescCreate(&EventDataDescriptor[15],
                        &DeviceData->StatRingDoorbellEnablingCount,
                        sizeof(DeviceData->StatRingDoorbellEnablingCount));

    EventDataDescCreate(&EventDataDescriptor[16],
                        &DeviceData->StatRingDoorbellEnabledCount,
                        sizeof(DeviceData->StatRingDoorbellEnabledCount));

    EventDataDescCreate(&EventDataDescriptor[17],
                        &DeviceData->StatRingDoorbellHwRemovedCount,
                        sizeof(DeviceData->StatRingDoorbellHwRemovedCount));

    EventDataDescCreate(&EventDataDescriptor[18],
                        &DeviceData->StatAnswerDoorbell,
                        sizeof(DeviceData->StatAnswerDoorbell));

    EventDataDescCreate(&EventDataDescriptor[19],
                        &DeviceData->StatAsyncCacheFlushIdle,
                        sizeof(DeviceData->StatAsyncCacheFlushIdle));

    EventDataDescCreate(&EventDataDescriptor[20],
                        &DeviceData->StatAsyncCacheFlushNormal,
                        sizeof(DeviceData->StatAsyncCacheFlushNormal));

    EventDataDescCreate(&EventDataDescriptor[21],
                        &DeviceData->StatAsyncCacheFlushRetry,
                        sizeof(DeviceData->StatAsyncCacheFlushRetry));

    EventDataDescCreate(&EventDataDescriptor[22],
                        &DeviceData->StatIntOnAsyncAdvancePendingCount,
                        sizeof(DeviceData->StatIntOnAsyncAdvancePendingCount));

    EventDataDescCreate(&EventDataDescriptor[23],
                        &DeviceData->StatIntOnAsyncAdvanceSyncRoutineCount,
                        sizeof(DeviceData->StatIntOnAsyncAdvanceSyncRoutineCount));

    EventDataDescCreate(&EventDataDescriptor[24],
                        &DeviceData->StatRingDoorbellEnabledWithRetryCount,
                        sizeof(DeviceData->StatRingDoorbellEnabledWithRetryCount));

    EventDataDescCreate(&EventDataDescriptor[25],
                        &DeviceData->StatRingDoorbellEnabledWithRetryIgnoredCount,
                        sizeof(DeviceData->StatRingDoorbellEnabledWithRetryIgnoredCount));

    EventDataDescCreate(&EventDataDescriptor[26],
                        &DeviceData->StatAsyncCacheFlushPrevented,
                        sizeof(DeviceData->StatAsyncCacheFlushPrevented));

    EventDataDescCreate(&EventDataDescriptor[27],
                        &DeviceData->CurrentRingDoorbellDisablingTimerCount,
                        sizeof(DeviceData->CurrentRingDoorbellDisablingTimerCount));

    EventDataDescCreate(&EventDataDescriptor[28],
                        &DeviceData->CurrentRingDoorbellEnablingTimerCount,
                        sizeof(DeviceData->CurrentRingDoorbellEnablingTimerCount));

    EventDataDescCreate(&EventDataDescriptor[29],
                        &DeviceData->CurrentRingDoorbellEnabledTimerCount,
                        sizeof(DeviceData->CurrentRingDoorbellEnabledTimerCount));

    EventDataDescCreate(&EventDataDescriptor[30],
                        &DeviceData->CurrentIntOnAsyncAdvancePendingCount,
                        sizeof(DeviceData->CurrentIntOnAsyncAdvancePendingCount));

    EventDataDescCreate(&EventDataDescriptor[31],
                        &DeviceData->StatResetNeededCount,
                        sizeof(DeviceData->StatResetNeededCount));
     
    EventDataDescCreate(&EventDataDescriptor[32],
                        &DeviceData->ResetNeeded,
                        sizeof(DeviceData->ResetNeeded));

    EventDataDescCreate(&EventDataDescriptor[33],
                        &DeviceData->ResetRecoveryEnable,
                        sizeof(DeviceData->ResetRecoveryEnable));

    EventDataDescCreate(&EventDataDescriptor[34],
                        &DeviceData->ResetRecoveryBreak,
                        sizeof(DeviceData->ResetRecoveryBreak));

    EventDataDescCreate(&EventDataDescriptor[35],
                        &DeviceData->AsyncScheduleState,
                        sizeof(DeviceData->AsyncScheduleState));

    EventDataDescCreate(&EventDataDescriptor[36],
                        &DeviceData->AsyncDoorbellState,
                        sizeof(DeviceData->AsyncDoorbellState));

    EventDataDescCreate(&EventDataDescriptor[37],
                        &DeviceData->PeriodicScheduleState,
                        sizeof(DeviceData->PeriodicScheduleState));

    EventDataDescCreate(&EventDataDescriptor[38],
                        &DeviceData->EhciInternalFlags,
                        sizeof(DeviceData->EhciInternalFlags));

    EventDataDescCreate(&EventDataDescriptor[39],
                        &DeviceData->PreventCmdChangesWithDoorbellPending,
                        sizeof(DeviceData->PreventCmdChangesWithDoorbellPending));

    EventDataDescCreate(&EventDataDescriptor[40],
                        &DeviceData->OptimizeInactiveQhFlush,
                        sizeof(DeviceData->OptimizeInactiveQhFlush));

    EventDataDescCreate(&EventDataDescriptor[41],
                        &DeviceData->DisableTwoDoorbellWorkaround,
                        sizeof(DeviceData->DisableTwoDoorbellWorkaround));

    EventDataDescCreate(&EventDataDescriptor[42],
                        &DeviceData->StrictTransitionChecksEnabled,
                        sizeof(DeviceData->StrictTransitionChecksEnabled));

    EventDataDescCreate(&EventDataDescriptor[43],
                        &DeviceData->IntegratedTTSupportRegistryOverride,
                        sizeof(DeviceData->IntegratedTTSupportRegistryOverride));

    EventDataDescCreate(&EventDataDescriptor[44],
                        &DeviceData->CurrentAsyncEnableCount,
                        sizeof(DeviceData->CurrentAsyncEnableCount));

    EventDataDescCreate(&EventDataDescriptor[45],
                        &DeviceData->InitialDoorbellRetryEnableValue,
                        sizeof(DeviceData->InitialDoorbellRetryEnableValue));

    EventDataDescCreate(&EventDataDescriptor[46],
                        &DeviceData->CurrentDoorbellRetryEnableValue,
                        sizeof(DeviceData->CurrentDoorbellRetryEnableValue));

    EventDataDescCreate(&EventDataDescriptor[47],
                        &DeviceData->DoorbellRetryRequired,
                        sizeof(DeviceData->DoorbellRetryRequired));

    EventDataDescCreate(&EventDataDescriptor[48],
                        &DeviceData->PeriodicEnableChangeWithDoorbellPending,
                        sizeof(DeviceData->PeriodicEnableChangeWithDoorbellPending));

    EventDataDescCreate(&EventDataDescriptor[49],
                        &DeviceData->PeriodicDisableChangeWithDoorbellPending,
                        sizeof(DeviceData->PeriodicDisableChangeWithDoorbellPending));

    USBPORT_ETW_WRITE(DeviceData,
                      &USBPORT_ETW_EVENT_HC_EHCI_MINIPORT_DEVICE_DATA,
                      NULL,
                      50,
                      EventDataDescriptor);

    ExFreePool(EventDataDescriptor);

    return USBMP_STATUS_SUCCESS;
}


USB_MINIPORT_STATUS
EHCI_PassThru (
     __inout PDEVICE_DATA DeviceData,
     __in GUID *FunctionGuid,
     ULONG ParameterLength,
     __inout_bcount(ParameterLength) PVOID Parameters
    )
{

    USB_MINIPORT_STATUS mpStatus;

    mpStatus = USBMP_STATUS_NOT_SUPPORTED;

    if (RtlEqualMemory(FunctionGuid, &GUID_USBPRIV_ROOTPORT_STATUS, sizeof(GUID))) {

        mpStatus = EHCI_RH_UsbprivRootPortStatus(DeviceData,
                                                 ParameterLength,
                                                 Parameters);

    } else if (RtlEqualMemory(FunctionGuid, &GUID_IDLE_STATUS, sizeof(GUID))) {

        mpStatus = EHCI_GetIdleStatus(DeviceData,
                                      ParameterLength,
                                      Parameters);
        return mpStatus;

    } else if (RtlEqualMemory(FunctionGuid, &GUID_HC_DEVICE_DATA, sizeof(GUID))) {

        mpStatus = EHCI_EtwLogDeviceData(DeviceData);

    }

    return mpStatus;
}


VOID
EHCI_SetEndpointDataToggle(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     ULONG Toggle
    )
/*++

Routine Description:

    This MINIPORT_SetEndpointDataToggle() routine is always called with
    the USBPORT Fdo.CoreFunctionSpin spinlock held.

Arguments:

    Toggle is 0 or 1

Return Value:

--*/

{
    PHCD_QUEUEHEAD_DESCRIPTOR qh;
    HC_QTD_TOKEN token;

    if (EndpointData->Parameters.TransferType == Control ||
        EndpointData->Parameters.TransferType == Isochronous) {

        // nothing to do for control and iso
        return;
    }

    qh = EndpointData->QueueHead;
    token = qh->HwQH.Overlay.qTD.Token;
    token.DataToggle = Toggle;
    qh->HwQH.Overlay.qTD.Token = token;

    LOGENTRY(DeviceData, G, '_stg', EndpointData, 0, Toggle);
}


VOID
EHCI_AssertRunBit(
    __inout PDEVICE_DATA DeviceData
    )
/*++

--*/
{
    USBCMD cmd;
    PHC_OPERATIONAL_REGISTER hcOp;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

    // if the controller is gone the run bit will be set
    // ie 0xffffffff
    EHCI_ASSERT(DeviceData, cmd.HostControllerRun == 1);
}


UCHAR
EHCI_GetEECP(
    __inout PDEVICE_DATA DeviceData,
    UCHAR CapabilityId
    )
/*
    Returns the offset into PCI Configuration space where the first
    Extended Capability register is located for the specified
    Capability ID.

    Returns 0 if Extended Capabilities are not supported by this host
    controller or if the specified Capability ID is not supported by
    this host controller.
*/
{
    UCHAR eecpOffset;
    HCCPARAMS hcCparms;
    HC_EECP eecp;
    PHC_CAPABILITIES_REGISTER hcCap;

    //
    // No PCI Configuration space -> No EHCI Extended Capabilities.
    //
    if (DeviceData->PciConfigSpaceDisabled) {
        return 0;
    }

    //
    // 5. EHCI Extended Capabilities
    //
    // EHCI controllers export EHCI-specific extended capabilities
    // utilizing a method similar to the PCI capabilities.  If an EHCI
    // controller implements any extended capabilities, it specifies a
    // non-zero value in the EHCI Extended Capabilities Pointer (EECP)
    // field of the HCCPARAMS register.  This value is an offset into
    // PCI Configuration space where the first extended capability
    // register is located.  Each capability register has the format
    // illustrated in Table 5-1.
    //
    hcCap = DeviceData->CapabilitiesRegisters;
    hcCparms.ul = READ_REGISTER_ULONG(&hcCap->HcCapabilityParameters.ul);

    eecpOffset = (UCHAR)hcCparms.EECP;

    if (eecpOffset == 0) {
        return eecpOffset;
    }

    do {

        USBPORT_READ_CONFIG_SPACE(
            DeviceData,
            &eecp,
            eecpOffset,
            sizeof(eecp));

        if (eecp.CapId == CapabilityId) {
            return eecpOffset;
        }

        eecpOffset = (UCHAR) eecp.NextCap;

    } while (eecpOffset);

    return eecpOffset;
}


USB_MINIPORT_STATUS
EHCI_StopBIOS(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    Stop Legacy BIOS

Arguments:

Return Value:

--*/
{
    UCHAR biosOffset;
    USB_LEGACYBIOS_REGISTERS hcLegReg;

    //
    // No PCI Configuration space -> No OS Handoff Synchronization
    // support capability.
    //
    if (DeviceData->PciConfigSpaceDisabled) {
        return USBMP_STATUS_SUCCESS;
    }

    biosOffset = EHCI_GetEECP(DeviceData, EECP_PRE_OS_HANDOFF);

    // do we have BIOS
    if (biosOffset) {

        // read the legacy registers
        USBPORT_READ_CONFIG_SPACE(DeviceData, &hcLegReg, biosOffset, sizeof(hcLegReg));

        // see if BIOS is enabled
        if (hcLegReg.Caps.HcBIOSowned) {
            USBLEGSUP legSup;
            LARGE_INTEGER timeout, now;

            // take ownership,
            // set OS owned
            legSup.ul = hcLegReg.Caps.ul;
            legSup.HcOSowned = 1;

            USBPORT_WRITE_CONFIG_SPACE(DeviceData, &legSup, biosOffset, sizeof(legSup));

            // wait on Bios owned to go to zero
            // timeout is 0.1 sec
            KeQuerySystemTime(&timeout);
            timeout.QuadPart += 1000000;
            do {
                USBPORT_READ_CONFIG_SPACE(DeviceData, &legSup, biosOffset, sizeof(legSup));
                KeQuerySystemTime(&now);
                if (now.QuadPart > timeout.QuadPart) {
                    break;
                }
            } while (legSup.HcBIOSowned);
        }
    }

    return USBMP_STATUS_SUCCESS;
}


PHC_DEBUGPORT_REGISTERS
EHCI_LocateDebugPortRegisters(
    __inout PDEVICE_DATA    DeviceData,
    __in PHC_RESOURCES   HcResources
    )
{
    PCI_COMMON_CONFIG           pciConfig;
    PPCI_CAPABILITIES_HEADER    capability;
    PDEBUGPORTCAPABILITY        debugCapability;
    UCHAR                       offset;
    ULONG                       loopcount;

    debugCapability = NULL;

    //
    // No PCI Configuration space -> No Debug Port capability.
    //
    if (DeviceData->PciConfigSpaceDisabled) {
        return NULL;
    }

    USBPORT_READ_CONFIG_SPACE(DeviceData,
                              &pciConfig,
                              0,
                              sizeof(pciConfig));

    //
    // If the device has a Capabilities List then search for the Debug
    // Port Capabilities.
    //

    if (!(pciConfig.Status & PCI_STATUS_CAPABILITIES_LIST))
    {
        return NULL;
    }

    offset = pciConfig.u.type0.CapabilitiesPtr & 0xFC;

    loopcount = 0;

    while ((offset >= 0x40) && (loopcount < 48))
    {
        capability = (PPCI_CAPABILITIES_HEADER)((PUCHAR)&pciConfig + offset);

        if (capability->CapabilityID == PCI_CAPABILITY_ID_DEBUG_PORT)
        {
            debugCapability = (PDEBUGPORTCAPABILITY)capability;

            break;
        }
        else
        {
            offset = capability->Next & 0xFC;

            loopcount++;
        }
    }

    if (debugCapability == NULL)
    {
        return NULL;
    }

    if (debugCapability->BarNumber != 1)
    {
        //
        // Currently HC_RESOURCES contains only one BAR address.  If
        // the Debug Port registers are not located in the first BAR
        // they cannot be accessed without modifying the HC_RESOURCES
        // structure to contain more than one BAR address.
        //
        return NULL;
    }

    return (PHC_DEBUGPORT_REGISTERS)((PUCHAR)HcResources->DeviceRegisters +
                                     debugCapability->Offset);
}

NTSTATUS
EHCI_EvaluateAcpiDsmRoutine(
    __in PDEVICE_OBJECT TargetDeviceObject,
    __in const  GUID *  Guid,
    __in ULONG          RevisionID,
    __in ULONG          FunctionIndex
    )
/*++

Routine Description:

    This function calls off to ACPI to evaluate the _DSM method.

Arguments:

    TargetDeviceObject - Supplies a pointer the targeted Device Object

    Guid - GUID for the respective interface

    FunctionIndex - Specifies which function to execute in the _DSM method.
        See ACPI specification version 4.0, section 9.14.1

Return Value:

    STATUS_SUCCESS if successful
    STATUS_INSUFFICIENT_RESOURCES if unable to allocate memory
    STATUS_NOT_SUPPORTED if the BIOS does not implement the request

--*/

{
    PACPI_METHOD_ARGUMENT           argument;
    PIRP                            irp;
    KEVENT                          irpCompleted;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX parametersBuffer = NULL;
    ULONG                           parametersBufferSize;
    ACPI_EVAL_OUTPUT_BUFFER         acpiEvalOutputBuffer = {0};
    NTSTATUS                        status;
    IO_STATUS_BLOCK                 statusBlock = {0};

    //
    // Allocate and populate a block for the IOCTL input buffer.
    //

    parametersBufferSize = FIELD_OFFSET(ACPI_EVAL_INPUT_BUFFER_COMPLEX,
                                        Argument) +
                           ACPI_METHOD_ARGUMENT_LENGTH(sizeof(GUID)) +
                           ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) +
                           ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) +
                           ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG));

    parametersBuffer = (PACPI_EVAL_INPUT_BUFFER_COMPLEX)ExAllocatePoolWithTag(
                                                            NonPagedPool,
                                                            parametersBufferSize,
                                                            EHCI_TAG);
    if (parametersBuffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto EvaluateDsmRoutineEnd;
    }

    RtlZeroMemory(parametersBuffer, parametersBufferSize);
    parametersBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    parametersBuffer->MethodNameAsUlong = DSM_METHOD_AS_ULONG;
    parametersBuffer->Size = parametersBufferSize;
    parametersBuffer->ArgumentCount = 4;

    //
    // Argument 0 - Interface GUID
    //
    argument = &parametersBuffer->Argument[0];
    ACPI_METHOD_SET_ARGUMENT_BUFFER(argument,
                                    Guid,
                                    sizeof(GUID));


    argument = ACPI_METHOD_NEXT_ARGUMENT(argument);

    //
    // Argument 1 - Revision number
    //
    ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, RevisionID);
    argument = ACPI_METHOD_NEXT_ARGUMENT(argument);

    //
    // Argument 2 - Function Index
    //
    ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, FunctionIndex);
    argument = ACPI_METHOD_NEXT_ARGUMENT(argument);


    //
    // Argument 3 - Function-dependent package.
    // Not currently used.
    //
    argument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
    argument->DataLength = 0;

    //
    // Allocate the IRP, send the IOCTL down, then wait for it to complete.
    //

    KeInitializeEvent(&irpCompleted, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_EVAL_METHOD,
                                        TargetDeviceObject,
                                        parametersBuffer,
                                        parametersBufferSize,
                                        &acpiEvalOutputBuffer,
                                        sizeof(acpiEvalOutputBuffer),
                                        FALSE,
                                        &irpCompleted,
                                        &statusBlock);

    if (irp == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto EvaluateDsmRoutineEnd;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    status = IoCallDriver(TargetDeviceObject, irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&irpCompleted,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        status = statusBlock.Status;
    }

    //
    // For this implementation, the _DSM is not required to return any data.
    //

EvaluateDsmRoutineEnd:

    if (parametersBuffer) {
        ExFreePoolWithTag(parametersBuffer, EHCI_TAG);
    }

    return status;
}

NTSTATUS
EHCI_EvaluateAcpiRoutine(
    IN  PDEVICE_OBJECT           TargetDeviceObject,
    IN  ULONG                    MethodName
    )

/*++

Routine Description:

   This routine sends an IRP to the underlying bus driver to evaluate the
   specified ACPI method. The method is expected to be defined on the device
   and not on a child.

Arguments:

    TargetDeviceObject - Supplies a pointer to the device object below us.

    MethodName - Supplies the name of the method to be invoked.

Return Value:

    NTSTATUS code.

--*/

{
    ACPI_EVAL_INPUT_BUFFER input;
    ACPI_EVAL_OUTPUT_BUFFER output;
    IO_STATUS_BLOCK ioBlock;
    KEVENT event;
    NTSTATUS status;
    PIRP irp;

    //
    // Initialize the input and output buffers.
    //

    RtlZeroMemory(&input, sizeof(ACPI_EVAL_INPUT_BUFFER));
    RtlZeroMemory(&output, sizeof(ACPI_EVAL_OUTPUT_BUFFER));
    input.Signature = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
    input.MethodNameAsUlong = MethodName;

    //
    // Build the request for ACPI to evaluate the specified method.
    //

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_EVAL_METHOD,
                                        TargetDeviceObject,
                                        &input,
                                        sizeof(ACPI_EVAL_INPUT_BUFFER),
                                        &output,
                                        sizeof(ACPI_EVAL_OUTPUT_BUFFER),
                                        FALSE,
                                        &event,
                                        &ioBlock);

    if (irp == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto EvaluateAcpiRoutineEnd;
    }

    //
    // Pass request to the underlying (ACPI) bus driver and wait for the IRP
    // to be completed.
    //

    status = IoCallDriver(TargetDeviceObject, irp);
    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = ioBlock.Status;
    }

EvaluateAcpiRoutineEnd:
    return status;
}

NTSTATUS
EHCI_EvaluateAcpiRoutineOnHcReset(
    IN  PDEVICE_OBJECT           TargetDeviceObject
    )

/*++

Routine Description:

    This routine evaluates the ACPI routines when the host controller has been reset.

    This routine first evaluates the _DSM Method ('MSD_')
    If that fails, this routine evaluates the _UBF Method ('FBU_')

    If any of these menthods evaluate successfully this routine returns success

Arguments:

    TargetDeviceObject - Supplies a pointer to the device object below us.

Comments:

    On certain SOC platforms, when the controller is reset, the controller transitions to
    a function controller mode. Thus we need to Evaluate ACPI methods to put the controller into
    host mode.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;

    status = EHCI_EvaluateAcpiDsmRoutine(TargetDeviceObject,
                                         (LPGUID)&USB_DSM_RESET_POST_PROC_GUID,
                                         USB_DSM_RESET_POST_PROC_REVISION_ID,
                                         USB_DSM_RESET_POST_PROC_FUNCTION_INDEX);

    if (!NT_SUCCESS(status)) {
        status = EHCI_EvaluateAcpiRoutine(TargetDeviceObject, ((ULONG)'FBU_'));
    }

    return status;
}


VOID
EHCI_NotifyTransferQueueState(
    __inout PDEVICE_DATA DeviceData,
    __in ULONG AsyncTransferCount,
    __in ULONG PeriodicTransferCount
    )
/*
    This miniport routine is called by USBPORT when the active Async or
    Periodic transfer count increments for 0 -> 1 or when the active
    Async or Periodic transfer count decrements from 1 -> 0.

    As a result the Async or Periodic schedule may be dynamically
    enabled or disabled, as appropriate.

    This MINIPORT_NotifyTransferQueueState() routine is always called
    with the USBPORT Fdo.CoreFunctionSpin spinlock held.
*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    KIRQL irql;
    BOOLEAN registersUnavailable = FALSE;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    if (DeviceData->HcSuspendedFlag) {
        cmd.ul = 0;
        registersUnavailable = TRUE;
    } else {

        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

        if (cmd.HostControllerRun == 0 ||
            cmd.ul == 0xFFFFFFFF) {
            registersUnavailable = TRUE;
        }

    }

    LOGENTRY(DeviceData, G, '_nT1', AsyncTransferCount, PeriodicTransferCount, cmd.ul);

    if (registersUnavailable == FALSE) {

        // Must acquire the AcfListSpin spinlock prior to calling
        // EHCI_iEnableAsyncList() or EHCI_iDisableAsyncList()
        //
        EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

        LOGENTRY(DeviceData, G, '_nT2', AsyncTransferCount, PeriodicTransferCount, cmd.ul);

        DeviceData->AsyncTransferCount = AsyncTransferCount;
        DeviceData->PeriodicTransferCount = PeriodicTransferCount;

        if (AsyncTransferCount == 0 &&
            DeviceData->AsyncIdleDetectEnabled) {

            LOGENTRY(DeviceData, G, '_nT3', AsyncTransferCount, PeriodicTransferCount, cmd.ul);
            EHCI_ASSERT(DeviceData, IsListEmpty(&DeviceData->PendingSmodeCTB_ListHead));
            EHCI_iDisableAsyncList(DeviceData);

        } else {
            LOGENTRY(DeviceData, G, '_nT4', AsyncTransferCount, PeriodicTransferCount, cmd.ul);
            EHCI_iEnableAsyncList(DeviceData);
        }

        EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

        LOGENTRY(DeviceData, G, '_nT5', AsyncTransferCount, PeriodicTransferCount, cmd.ul);

        if (PeriodicTransferCount == 0 &&
            DeviceData->PeriodicIdleDetectEnabled) {
            EHCI_DisablePeriodicList(DeviceData);
        } else {
            EHCI_EnablePeriodicList(DeviceData);
        }

    }

    LOGENTRY(DeviceData, G, '_nT6', AsyncTransferCount, PeriodicTransferCount, cmd.ul);
}


// Timeout limits are in seconds
//
#define ASYNC_DOORBELL_DISABLING_TIMEOUT    30
#define ASYNC_DOORBELL_ENABLING_TIMEOUT     30
#define ASYNC_DOORBELL_ENABLED_TIMEOUT      30
#define ASYNC_DOORBELL_NO_RETRY_PERIOD      15

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_CheckController(
    __inout PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    This MINIPORT_CheckController() routine is called by the USBPORT
    timer DPC once per second.

    If it appears that the host controller has been surprise removed
    call:

        USBPORT_INVALIDATE_CONTROLLER(DeviceData, UsbMpControllerRemoved);

    If it appears that the host controller has become unresponsive and
    a reset is necessary call:

        USBPORT_INVALIDATE_CONTROLLER(DeviceData, UsbMpControllerNeedsHwReset);

--*/

{
    NTSTATUS    errorCode = 0;
    KIRQL       irql;

    if (DeviceData->DeviceStarted == FALSE) {
        return;
    }

    //
    // Check to see if the host controller appears to have been
    // surprise removed.
    //
    if (EHCI_HardwarePresent(DeviceData, TRUE) == FALSE) {
        return;
    }

    //
    // Don't trigger again if the controller wasn't reset yet from the
    // last time around.
    //
    if (DeviceData->ResetNeeded != 0) {
        return;
    }

    //
    // Check to see if the host controller reported an error which
    // requires reset recovery.
    //
    if (DeviceData->HostSystemErrorResetNeeded) {
        DeviceData->HostSystemErrorResetNeeded = 0;
        DeviceData->ResetNeeded = 1;
        errorCode = USBEHCI_HOST_SYSTEM_ERROR_REPORTED;
    }

    //
    // Check for async doorbell timeouts which require reset recovery.
    //
    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    if (DeviceData->CurrentRingDoorbellDisablingTimerCount > 0) {
        if (DeviceData->CurrentRingDoorbellDisablingTimerCount++ >
            ASYNC_DOORBELL_DISABLING_TIMEOUT) {

            //
            // Asynchronous Schedule transitioning to disabled timeout.
            //
            DeviceData->CurrentRingDoorbellDisablingTimerCount = 0;
            DeviceData->ResetNeeded = 1;
            errorCode = USBEHCI_ASYNC_DOORBELL_DISABLING_TIMEOUT;
        }
    }
    if (DeviceData->CurrentRingDoorbellEnablingTimerCount > 0) {
        if (DeviceData->CurrentRingDoorbellEnablingTimerCount++ >
            ASYNC_DOORBELL_ENABLING_TIMEOUT) {

            //
            // Asynchronous Schedule transitioning to enabled timeout.
            //
            DeviceData->CurrentRingDoorbellEnablingTimerCount = 0;
            DeviceData->ResetNeeded = 1;
            errorCode = USBEHCI_ASYNC_DOORBELL_ENABLING_TIMEOUT;
        }
    }
    if (DeviceData->CurrentRingDoorbellEnabledTimerCount > 0) {

        // If doorbell retry is disabled and if we have expired the
        // "No Retry Period", enable retry here. Another required action here
        // is to complete any request to enable or disable the periodic schedule
        //

        if (DeviceData->CurrentDoorbellRetryEnableValue == FALSE &&
            DeviceData->CurrentRingDoorbellEnabledTimerCount > ASYNC_DOORBELL_NO_RETRY_PERIOD &&
            DeviceData->CurrentIntOnAsyncAdvancePendingCount == 0) {

            if ((DeviceData->ResetRecoveryBreak != 0) &&
                (KdRefreshDebuggerNotPresent() == FALSE)) {

                // Break into the debugger, if one is present. This should not happen
                // under normal circumstances and a debugger break can help its
                // investigation.
                //
                DbgBreakPoint();

            }

            DeviceData->CurrentDoorbellRetryEnableValue = TRUE;

            if (DeviceData->AsyncScheduleState == PeriodicScheduleState_EnablePendingDoorbell) {
                EHCI_iEnablePeriodicList(DeviceData);
            } else if (DeviceData->AsyncScheduleState == PeriodicScheduleState_DisablePendingDoorbell) {
                EHCI_iDisablePeriodicList(DeviceData);
            }
        }

        if (DeviceData->CurrentRingDoorbellEnabledTimerCount++ >
            ASYNC_DOORBELL_ENABLED_TIMEOUT) {

            //
            // Asynchronous Schedule enabled and waiting for doorbell
            // response timeout.
            //
            DeviceData->CurrentRingDoorbellEnabledTimerCount = 0;
            DeviceData->ResetNeeded = 1;
            errorCode = USBEHCI_ASYNC_DOORBELL_ENABLED_TIMEOUT;
        }
    }

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    if (DeviceData->ResetNeeded) {

        DeviceData->StatResetNeededCount++;

        //
        // Write an error log entry noting which reset trigger was
        // tripped, along with a set of statistics which might be
        // interesting for analysis purposes.
        //
        EHCI_WriteErrorLogEntry(DeviceData, errorCode, FALSE);

        DbgPrint("Detected Async Advance Hang - Reset Controller?\n");

        if ((DeviceData->ResetRecoveryBreak != 0) &&
            (KdRefreshDebuggerNotPresent() == FALSE)) {

            //
            //  A kernel debugger is active.  Break and give the
            //  debugger operator the opportunity investigate the
            //  cause of the reset recovery trigger.
            //
            DbgBreakPoint();
        }

        //
        // Don't reset the controller if the BIOS fixup is necessary as
        // EHCI_ResetController() currently will not execute the BIOS
        // fixup method.
        //
        if (DeviceData->ResetRecoveryEnable != 0) {

            if (DeviceData->EvaluateAcpiMethodOnResetSucceeded) {

                // Tell USBPORT to please reset the controller.
                //
                USBPORT_INVALIDATE_CONTROLLER(DeviceData,
                                              UsbMpControllerNeedsHwResetOnPassive);
            } else  {

                USBPORT_INVALIDATE_CONTROLLER(DeviceData,
                                              UsbMpControllerNeedsHwReset);

            }

        }
    }
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EHCI_WriteErrorLogEntry(
    __in PDEVICE_DATA   DeviceData,
    __in NTSTATUS       ErrorCode,
    __in BOOLEAN        IsLockHeld
    )
/*++

Routine Description:

    Write an error log entry noting which reset trigger was tripped,
    along with a set of statistics which might be interesting for
    analysis purposes.

Requirements:
    IRQL: <= DISPATCH_LEVEL

--*/
{
    PIO_ERROR_LOG_PACKET    errorLogPacket;
    PEHCI_ERROR_LOG_STATS   errorLogStats;
    KIRQL                   irql = PASSIVE_LEVEL;

    errorLogPacket = IoAllocateErrorLogEntry(DeviceData->HostControllerFdo,
                                             sizeof(IO_ERROR_LOG_PACKET) - sizeof(ULONG) +
                                             sizeof(*errorLogStats));

    if (errorLogPacket != NULL) {

        errorLogPacket->MajorFunctionCode = 0;
        errorLogPacket->RetryCount = 0;
        errorLogPacket->DumpDataSize = sizeof(*errorLogStats);
        errorLogPacket->NumberOfStrings = 0;
        errorLogPacket->StringOffset = 0;
        errorLogPacket->EventCategory = 0;
        errorLogPacket->ErrorCode = ErrorCode;
        errorLogPacket->UniqueErrorValue = 0;
        errorLogPacket->FinalStatus = 0;
        errorLogPacket->SequenceNumber = 0;
        errorLogPacket->IoControlCode = 0;
        errorLogPacket->DeviceOffset.QuadPart = 0;

        errorLogStats = (PEHCI_ERROR_LOG_STATS)(&errorLogPacket->DumpData[0]);

        errorLogStats->VendorId = DeviceData->Vid;
        errorLogStats->DeviceId = DeviceData->Dev;

        if (IsLockHeld == FALSE) {
            EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);
        }

        errorLogStats->HostSystemErrorCount                     = DeviceData->HostSystemErrorCount;
        errorLogStats->StatAsyncEnableCount                     = DeviceData->StatAsyncEnableCount;
        errorLogStats->StatAsyncDisableCount                    = DeviceData->StatAsyncDisableCount;
        errorLogStats->StatPeriodicEnableCount                  = DeviceData->StatPeriodicEnableCount;
        errorLogStats->StatPeriodicDisableCount                 = DeviceData->StatPeriodicDisableCount;
        errorLogStats->StatAsyncEnableTimeoutCount              = DeviceData->StatAsyncEnableTimeoutCount;
        errorLogStats->StatAsyncDisableTimeoutCount             = DeviceData->StatAsyncDisableTimeoutCount;
        errorLogStats->StatPeriodicEnableTimeoutCount           = DeviceData->StatPeriodicEnableTimeoutCount;
        errorLogStats->StatPeriodicDisableTimeoutCount          = DeviceData->StatPeriodicDisableTimeoutCount;
        errorLogStats->StatRingDoorbellDisabledCount            = DeviceData->StatRingDoorbellDisabledCount;
        errorLogStats->StatRingDoorbellDisablingCount           = DeviceData->StatRingDoorbellDisablingCount;
        errorLogStats->StatRingDoorbellEnablingCount            = DeviceData->StatRingDoorbellEnablingCount;
        errorLogStats->StatRingDoorbellEnabledCount             = DeviceData->StatRingDoorbellEnabledCount;
        errorLogStats->StatAnswerDoorbell                       = DeviceData->StatAnswerDoorbell;
        errorLogStats->StatAsyncCacheFlushIdle                  = DeviceData->StatAsyncCacheFlushIdle;
        errorLogStats->StatAsyncCacheFlushNormal                = DeviceData->StatAsyncCacheFlushNormal;
        errorLogStats->StatAsyncCacheFlushRetry                 = DeviceData->StatAsyncCacheFlushRetry;
        errorLogStats->StatIntOnAsyncAdvancePendingCount        = DeviceData->StatIntOnAsyncAdvancePendingCount;
        errorLogStats->StatIntOnAsyncAdvanceSyncRoutineCount    = DeviceData->StatIntOnAsyncAdvanceSyncRoutineCount;
        errorLogStats->StatResetNeededCount                     = DeviceData->StatResetNeededCount;

        errorLogStats->InitialDoorbellRetryEnableValue          = DeviceData->InitialDoorbellRetryEnableValue;
        errorLogStats->CurrentDoorbellRetryEnableValue          = DeviceData->CurrentDoorbellRetryEnableValue;
        errorLogStats->DoorbellRetryRequired                    = DeviceData->DoorbellRetryRequired;
        errorLogStats->PeriodicEnableChangeWithDoorbellPending  = DeviceData->PeriodicEnableChangeWithDoorbellPending;
        errorLogStats->PeriodicDisableChangeWithDoorbellPending = DeviceData->PeriodicDisableChangeWithDoorbellPending;

        if (IsLockHeld == FALSE) {
            EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);
        }

        IoWriteErrorLogEntry(errorLogPacket);
    }
}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ResetControllerDontReinit(
    __inout PDEVICE_DATA DeviceData
    )
/*
Routine Description:

    Attempt to resurrect the HC after we have determined that it is dead.

    (1)  Clear the Run/Stop bit to halt the controller.
    (1a) Wait for the HcHalted bit to become set.
    (2)  Set the Reset bit to reset the controller.
    (2a) Wait for the Reset bit to become clear.
    (3)  Empty the async advance doorbell endpoint waiting list.
    (4)  Mark all endpoints as being reset and off the hardware.

    This MINIPORT_ResetControllerDontReinit() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.
*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    USBSTS status;
    ULONG64 finishTime, currentTime;
    ULONG64 qpcTimeStamp;
    PLIST_ENTRY ListEntry;
    ULONG i;
    KIRQL irql;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    // The EHCI specification says "Software should not set this bit
    // [HCReset] to a one when the HCHalted bit in the USBSTS register
    // is a zero.  Attempting to reset an actively running host
    // controller will result in undefined behavior."
    //
    // So clear the Run/Stop bit to cause the HCHalted bit to become
    // set before setting the HCReset bit.
    //
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    cmd.HostControllerRun = 0;
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // Wait for the HCHalted bit to become set as a result of clearing
    // the Run/Stop bit.
    //
    // The EHCI specification says "The Host Controller must halt within
    // 16 micro-frames after software clears the Run bit. The HC Halted
    // bit in the status register indicates when the Host Controller has
    // finished its pending pipelined transactions and has entered the
    // stopped state."
    //
    for (i = 0; i < 18; i++) {
        status.ul = READ_REGISTER_ULONG(&hcOp->UsbStatus.ul);
        if (status.HcHalted) {
            break;
        }
        KeStallExecutionProcessor(125);
    }

    // Reset the host controller by setting the HCReset bit.
    //
    // The EHCI specification says "This bit is set to zero by the Host
    // Controller when the reset process is complete.  Software cannot
    // terminate the reset process early by writing a zero to this
    // register."
    //
    cmd.ul = 0;
    cmd.HostControllerReset = 1;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    //
    // KeQueryInterruptTimePrecise is used to get the most accurate time, 
    // Previously we used non precise versions like KeQuerySystemTime can potentially return stale time
    // Please see Windows Blue Bug: 5146530 
    //
    finishTime = KeQueryInterruptTimePrecise(&qpcTimeStamp);
    // Wait 0.1 seconds for the reset to complete
    //
    finishTime += 1000000;

    // Wait for the HCReset bit to become zero.
    //
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);

    while (cmd.HostControllerReset) {

        // Using KeQueryInterruptTime, because of tight loop to avoid any perf impact
        currentTime = KeQueryInterruptTime();

        if (currentTime >= finishTime) {
            break;
        }

        cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    }

    EHCI_AcquireAsyncDoorbellLock(DeviceData, &irql);

    // Clear states directly without having to go through sanity checks
    // If we are here, the sanity of the host controller and its state is
    // is already compromised anyway.
    //
    // N.B: This step is required before calling EHCI_ClearAsyncDoorbellList()
    // as EHCI_ClearAsyncDoorbellList() can attempt to enable the schedule
    // when there is a need to replay a previous enable/disable schedule request
    // that was prevented when the doorbell was pending.
    //

    DeviceData->AsyncDoorbellState = AsyncDoorbellState_NotWaiting;
    DeviceData->AsyncScheduleState = AsyncScheduleState_Disabled;
    DeviceData->PeriodicScheduleState = PeriodicScheduleState_Disabled;

    // Release all endpoints which are currently waiting for an
    // Interrupt on Async Advance interrupt notification.
    //
    // This set of endpoints includes the current EpAtTheDoor endpoint
    // and all endpoints currently on the AcfListHead list.
    //
    // EpAtTheDoor should be NULL and the AcfListHead list should be
    // empty after calling EHCI_ClearAsyncDoorbellList().
    //
    // The HwSyncState of all Control and Bulk endpoints should be set
    // to Qh_Not_Busy after calling EHCI_ClearAsyncDoorbellList().
    //
    EHCI_ClearAsyncDoorbellList(DeviceData);

    EHCI_ReleaseAsyncDoorbellLock(DeviceData, irql);

    // Mark all endpoints as being reset.
    //
    ListEntry = DeviceData->AllEpListHead.Flink;

    while (ListEntry != &DeviceData->AllEpListHead) {

        PENDPOINT_DATA EndpointData;
        PHCD_QUEUEHEAD_DESCRIPTOR qh;

        EndpointData = (PENDPOINT_DATA) CONTAINING_RECORD(
            ListEntry,
            struct _ENDPOINT_DATA,
            AllEpLink);

        SET_FLAG(EndpointData->Flags, EHCI_EDFLAG_HC_RESET);

        switch (EndpointData->Parameters.TransferType) {
            case Control:
            case Bulk:
                qh = EndpointData->QueueHead;
                CLEAR_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE);
                CLEAR_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE);
                break;

            case Interrupt:
                qh = EndpointData->QueueHead;
                CLEAR_FLAG(qh->QhFlags, EHCI_QH_FLAG_IN_SCHEDULE);
                EndpointData->StateChangeTc.QuadPart = 0;
                EndpointData->HwSyncState = Qh_Not_Busy;
                break;

            case Isochronous:
                EndpointData->NextEndpoint = NULL;
                EndpointData->PrevEndpoint = NULL;
                break;

            default:
                break;
        }

        ListEntry = ListEntry->Flink;
    }

}


__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ReinitController(
    __inout PDEVICE_DATA DeviceData
    )
/*
Routine Description:

    Attempt to resurrect the HC after we have determined that it is dead.

    (1-4)  See EHCI_ResetControllerDontReinit
    (5)  Reinitialize the host controller schedule from scratch.
    (5a) Reprogram the Periodic Frame List Base Register.
    (5b) Reprogram the Asynchronous List Address Register.
    (6)  Set the Run/Stop bit to start the controller.
    (7)  Power on the root hub ports, if necessary.
    (8)  Reprogram the Configure Flag Register to route root hub ports
         to this EHCI controller.

    This MINIPORT_ReinitController() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.
*/
{
    PHC_OPERATIONAL_REGISTER hcOp;
    USBCMD cmd;
    CONFIGFLAG configFlag;
    ULONG i;

    hcOp = DeviceData->OperationalRegisters;

    if (hcOp == NULL) {
        return;
    }

    RtlSecureZeroMemory(DeviceData->CommonBufferVa, DeviceData->CommonBufferBytes);
    RtlSecureZeroMemory(DeviceData->CommonBufferBlocksVa, DeviceData->CommonBufferBlockBytes);

    EHCI_InitializeSchedule(DeviceData,
                            DeviceData->CommonBufferVa,
                            DeviceData->CommonBufferPhys,
                            DeviceData->CommonBufferBlocksVa,
                            DeviceData->CommonBufferBlocksPhys);

    // Empty the active isoch endpoint list.
    //
    DeviceData->IsoEndpointListHead = NULL;

    // Re-program the USBCMD - USB Command Register to set the Maximum
    // Interrupt Interval to 1 micro-frame.
    //
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    cmd.InterruptThreshold = 1;
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);







    // Re-program the USBCMD - USB Command Register to set Run/Stop bit
    // to start the host controller.
    //
    cmd.ul = READ_REGISTER_ULONG(&hcOp->UsbCommand.ul);
    cmd.HostControllerRun = 1;
    cmd.IntOnAsyncAdvanceDoorbell = 0;
    WRITE_REGISTER_ULONG(&hcOp->UsbCommand.ul, cmd.ul);

    // Power on the root hub ports, if necessary.
    //
    if (DeviceData->PortPowerControl == 1) {

        for (i = 0; i < DeviceData->NumberOfPorts; i++) {

            PORTSC portStatusControl;

            portStatusControl.ul = READ_REGISTER_ULONG(&hcOp->PortRegister[i].ul);
            MASK_CHANGE_BITS(portStatusControl);
            portStatusControl.PortPower = 1;
            WRITE_REGISTER_ULONG(&hcOp->PortRegister[i].ul,
                                 portStatusControl.ul);
        }
    }

    // Re-program the CONFIGFLAG - Configure Flag Register to set the
    // root hub port routing to this EHCI controller.
    //
    configFlag.ul = 0;
    configFlag.RoutePortsToEHCI = 1;
    WRITE_REGISTER_ULONG(&hcOp->ConfigFlag.ul, configFlag.ul);

    DeviceData->ResetNeeded = 0;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ResetController(
    __inout PDEVICE_DATA DeviceData
    )
/*
Routine Description:

    Attempt to resurrect the HC after we have determined that it is dead.

    This MINIPORT_ResetController() routine is always called with the
    USBPORT Fdo.CoreFunctionSpin spinlock held.
*/
{
    EHCI_ResetControllerDontReinit(DeviceData);
    EHCI_ReinitController(DeviceData);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
EHCI_PostResetController(
    __inout PDEVICE_DATA DeviceData
    )
/*
Routine Description:

*/
{
    (VOID)EHCI_EvaluateAcpiRoutineOnHcReset(DeviceData->TargetDeviceObject);
}
