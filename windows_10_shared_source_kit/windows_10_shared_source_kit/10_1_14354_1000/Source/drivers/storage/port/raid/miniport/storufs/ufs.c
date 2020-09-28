/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ufs.c

Abstract:

    This module contains the subroutines for the I/O system.

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#include "storufs.h"

/* SSS_WARNINGS_OFF */ 
#pragma warning(disable:4152)

ULONG
DriverEntry(
    _In_ PVOID DriverObject,
    _In_ PVOID RegistryPath
    )
/*++

Routine Description:

    This routine is the initial entry point for the UFS Storport Miniport driver.

Arguments:

    Driver Object - Address of a variable pointing to the Driver Object

    RegistryPath - Address of the unicode string pointing to the Registry Path

Return Value:

    The status of the StorPortInitialize() function call

--*/

{
    HW_INITIALIZATION_DATA hwInitData;

    //
    // Zero out the initialization structure
    //

    RtlZeroMemory(&hwInitData, sizeof(HW_INITIALIZATION_DATA));

    hwInitData.HwInitializationDataSize = sizeof(HW_INITIALIZATION_DATA);

    //
    // Set the entry points into the UFS miniport for Storport
    //

    hwInitData.HwInitialize = UfsInitialize;
    hwInitData.HwStartIo = UfsStartIo;
    hwInitData.HwBuildIo = UfsBuildIo;
    hwInitData.HwInterrupt = UfsInterrupt;
    hwInitData.HwFindAdapter = UfsFindAdapter;
    hwInitData.HwResetBus = UfsResetBus;
    hwInitData.HwAdapterControl = UfsAdapterControl;
    hwInitData.HwUnitControl = UfsUnitControl;

    //
    // The type of Bus chosen for UFS is "Internal".
    // Storport handles certain things differently for Bus type "Internal".
    //

#if defined (_ARM_) || defined (_ARM64_)
    hwInitData.AdapterInterfaceType = ACPIBus;
#else
    hwInitData.AdapterInterfaceType = PCIBus;
#endif

    //
    // Set the size of the structures that port needs to allocate.
    //

    hwInitData.DeviceExtensionSize = sizeof(UFS_ADAPTER_EXTENSION);
    hwInitData.SrbExtensionSize = sizeof(UFS_SRB_EXTENSION);
    hwInitData.SpecificLuExtensionSize = sizeof(UFS_LUN_EXTENSION);

    hwInitData.NumberOfAccessRanges = NUM_ACCESS_RANGES;
    hwInitData.NeedPhysicalAddresses = TRUE;
    hwInitData.MapBuffers = STOR_MAP_NO_BUFFERS;
    hwInitData.TaggedQueuing = TRUE;
    hwInitData.AutoRequestSense = TRUE;
    hwInitData.MultipleRequestPerLu = TRUE;
    hwInitData.FeatureSupport = STOR_FEATURE_DEVICE_NAME_NO_SUFFIX |
                                STOR_FEATURE_DUMP_POINTERS |
                                STOR_FEATURE_DUMP_RESUME_CAPABLE;
                                // STOR_FEATURE_FULL_PNP_DEVICE_CAPABILITIES
    //
    // Register these settings with StorPort
    //

    return StorPortInitialize(DriverObject, RegistryPath, &hwInitData, NULL);

}

ULONG
UfsFindAdapter(
    _In_ PVOID DeviceExtension,
    _In_ PVOID HwContext,
    _In_ PVOID BusInformation,
    _In_z_ PSTR ArgumentString,
    _Inout_ PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    _In_  PBOOLEAN Again
    )
/*+++

Routine Description:

    This routine is called to determine if a given HBA is supported or not. We
    will initialize a few fields in ConfigInfo struct, and initialize our
    device extension.

Arguments:

    DeviceExtension - UFS Adapter Extension

    HwContext - Pointer to the physical device object

    BusInformation - Point to the adapter device object

    LowerDevice - Device Object below the port FDO

    ArgumentString - Not used

    ConfigInfo - UFS configuration information

    Again - Not used

Return Value:

    If there was any error, return value is SP_RETURN_ERROR.
    Else, SP_RETURN_FOUND/ SP_RETURN_NOT_FOUND is returned.

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension;
    PUFS_DUMP_CONTEXT dumpContext = (PUFS_DUMP_CONTEXT)ConfigInfo->MiniportDumpData;
    ULONG i;
    ULONG pcicfgLen = 0;
    UCHAR pcicfgBuffer[0x30] = {0};
    PPCI_COMMON_CONFIG pciConfigData;

    UNREFERENCED_PARAMETER(HwContext);
    UNREFERENCED_PARAMETER(BusInformation);
    UNREFERENCED_PARAMETER(ArgumentString);
    UNREFERENCED_PARAMETER(Again);

    adapterExtension = (PUFS_ADAPTER_EXTENSION) DeviceExtension;
    adapterExtension->DumpMode = (ConfigInfo->DumpMode) ? 1 : 0;

    UfsInitializeDbgLog(adapterExtension);

    if (UfsQueryDumpMode(adapterExtension) != FALSE) {
        UfsDumpDebugBreakpoint(adapterExtension);

        if (dumpContext != NULL) {
            //
            // When in dump/ hibernation mode, we need to mark the Miniport
            // Dump Data region
            //

            StorPortMarkDumpMemory(adapterExtension,
                                   dumpContext,
                                   sizeof(UFS_DUMP_CONTEXT),
                                   0);
        } else {
            NT_ASSERT(dumpContext != NULL);
            return SP_RETURN_ERROR;
        }
    }

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        UfsRegistryReadControllerSettings(adapterExtension);
    } else {
        adapterExtension->FeatureFlags = dumpContext->FeatureFlags;

        RtlCopyMemory(&adapterExtension->RegValues,
                      &dumpContext->RegValues,
                      sizeof(UFS_REG_VALUES));
    }

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        if (ConfigInfo->AdapterInterfaceType == PCIBus) {
            //
            // Get information from the PCI Bus
            //

            pcicfgLen = StorPortGetBusData(adapterExtension,
                                           PCIConfiguration,
                                           ConfigInfo->SystemIoBusNumber,
                                           (ULONG)ConfigInfo->SlotNumber,
                                           (PVOID)pcicfgBuffer,
                                           (ULONG)PCI_CONFIG_LEN);

            if (pcicfgLen == PCI_CONFIG_LEN) {
                pciConfigData = (PPCI_COMMON_CONFIG)pcicfgBuffer;
                adapterExtension->PciVendorID = pciConfigData->VendorID;
                adapterExtension->PciDeviceID = pciConfigData->DeviceID;
                adapterExtension->PciRevisionID = pciConfigData->RevisionID;

                adapterExtension->UfsBar = (pciConfigData->u.type0.BaseAddresses[0] & 0xFFFFF000);
                adapterExtension->UfsBar |= ((ULONGLONG)pciConfigData->u.type0.BaseAddresses[1] << 32);
            } else {
                // LogError: "Error Retrieving PCI Bus Data for the UFS Controller"
                return SP_RETURN_ERROR;
            }
        }
    } else {
        adapterExtension->PciVendorID = dumpContext->VendorID;
        adapterExtension->PciDeviceID = dumpContext->DeviceID;
        adapterExtension->PciRevisionID = dumpContext->RevisionID;
        adapterExtension->UfsBar = dumpContext->UfsBar;
    }

    //
    // Retrieve the Base Address of the Memory Mapped Registers for the
    // Host Controller
    //

    UfsGetRegisterAddress(adapterExtension,
                          ConfigInfo);

    if (adapterExtension->UfsRegisters == NULL) {
        // LogError: "Error Retrieving the UFS Register Base Address from PCI Bus"

        return SP_RETURN_BAD_CONFIG;
    }

    //
    // Initialize the ConfigInfo Structure
    //

    ConfigInfo->NumberOfBuses = 1;
    ConfigInfo->MaximumNumberOfTargets = 1;
    ConfigInfo->MaximumNumberOfLogicalUnits = MAX_NUM_LU;
    ConfigInfo->NumberOfPhysicalBreaks = MAX_NUM_SCATTER_GATHER;
    ConfigInfo->MapBuffers = STOR_MAP_NO_BUFFERS;
    ConfigInfo->AlignmentMask = FILE_LONG_ALIGNMENT;
    ConfigInfo->ResetTargetSupported = TRUE;
    ConfigInfo->ScatterGather = TRUE;
    ConfigInfo->Master = TRUE;
    ConfigInfo->NeedPhysicalAddresses = TRUE;
    ConfigInfo->TaggedQueuing = TRUE;
    ConfigInfo->SrbType = SRB_TYPE_SCSI_REQUEST_BLOCK;
    ConfigInfo->SynchronizationModel = StorSynchronizeFullDuplex;
    ConfigInfo->VirtualDevice = FALSE;
    ConfigInfo->DeviceExtensionSize = sizeof(UFS_ADAPTER_EXTENSION);
    ConfigInfo->SpecificLuExtensionSize = sizeof(UFS_LUN_EXTENSION);
    ConfigInfo->SrbExtensionSize = sizeof(UFS_SRB_EXTENSION);

    ConfigInfo->MaximumTransferLength = 1024 * 1024;
    ConfigInfo->MaxIOsPerLun = MAX_REQUEST_LIST_SIZE;
    ConfigInfo->InitialLunQueueDepth = MAX_REQUEST_LIST_SIZE;











#if defined (_AMD64_) || (_ARM64_) 
    ConfigInfo->Dma64BitAddresses = SCSI_DMA64_MINIPORT_FULL64BIT_SUPPORTED;
#endif

    ConfigInfo->MaxNumberOfIO = ConfigInfo->MaxIOsPerLun;
    ConfigInfo->FeatureSupport |= STOR_ADAPTER_DMA_V3_PREFERRED;







    if (!NT_SUCCESS(UfsAllocateRequestLists(adapterExtension,
                                            ConfigInfo))) {

        // LogError: "Error Initializing the Request List Structures"
        return SP_RETURN_ERROR;
    }

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        UfsInitializeDpcs(adapterExtension);
    }

    //
    // It is assumed that the device/ adapter are in a state where they can
    // receive IO. While the Device isn't strictly active, we need the 
    // driver to think it is until we register for PoFx on the Device.
    //

    adapterExtension->BusReset = FALSE;
    //InterlockedExchange(&adapterExtension->PoFxActive, 1);

    //
    // Start the Adapter (configure the TRL/TMRL Base Address Registers,
    // enable interrupts, etc.). Follows a subset of steps outlined in the
    // JESD223A spec, Section 7.2.1
    //

    for (i = 0; i < MAX_NUM_STARTUP_ATTEMPTS; i++) {
        if (UfsStartAdapter(adapterExtension) == STATUS_SUCCESS) {
            break;
        }

        // LogError: "Error Starting the Adapter: Attempt #%i. Trying again..."
    }
    if (i == MAX_NUM_STARTUP_ATTEMPTS) {
        NT_ASSERT(FALSE);

        // LogError: "Cannot Start the Adapter. Unloading..."

        return FALSE;
    }

    //
    // Query the RPMB capabilities
    //

    UfsGetRPMBCapabilities(adapterExtension);

    return SP_RETURN_FOUND;
}

BOOLEAN
UfsInitialize(
    _In_ PVOID DeviceExtension
    )
/*+++

Routine Description:

    Enable passive initialization and register the callback for the passive
    initialization routine.

Arguments:

    Device Extension - Ufs adapter extension structure

Return Value:

    Returns TRUE for successful initialization

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    ULONG status = STOR_STATUS_SUCCESS;
    PERF_CONFIGURATION_DATA perfConfigData = {0};
    STOR_RPMB_CAPABILITIES_DATA rpmbCapabilities = {0};
    PSTOR_ADDR_BTL8 rpmbTarget;

#if !(_ARM64_)
    ULONG processorCount = 1;
#endif

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        //
        // Enable passive initialization to register for Runtime Power Management
        //

        StorPortEnablePassiveInitialization(DeviceExtension, UfsPassiveInitialize);
    } else {
        //
        // If we're in dump mode, we won't register for runtime power management. Let's fill that
        // in here
        //

        adapterExtension->Power.Active = TRUE;
        adapterExtension->Power.PoFxRegistered = FALSE;
        adapterExtension->Power.D3ColdEnabled = 0;
        adapterExtension->Power.FStateCount = 1;
        adapterExtension->Power.CurrentFState = 0;
        adapterExtension->Power.CurrentSystemPowerHint = RaidSystemPowerUnknown;
    }

    //
    // Query Performance Optimization Information from Storport
    //

    perfConfigData.Version = STOR_PERF_VERSION;
    perfConfigData.Size = sizeof(PERF_CONFIGURATION_DATA);

    status = StorPortInitializePerfOpts(adapterExtension, TRUE, &perfConfigData);

    //
    // Set the Desired Performance Optimization Parameters
    //

    if (status == STOR_STATUS_SUCCESS) {
        perfConfigData.Version = STOR_PERF_VERSION;
        perfConfigData.Size = sizeof(PERF_CONFIGURATION_DATA);
#if !(_ARM64_)
        processorCount = (ULONG)KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

        perfConfigData.Flags = STOR_PERF_DPC_REDIRECTION |
                               STOR_PERF_CONCURRENT_CHANNELS; // | STOR_PERF_OPTIMIZE_FOR_COMPLETION_DURING_STARTIO;
        perfConfigData.ConcurrentChannels = processorCount;

        status = StorPortInitializePerfOpts(adapterExtension, FALSE, &perfConfigData);

        NT_ASSERT(status == STOR_STATUS_SUCCESS);
#endif
        if (status != STOR_STATUS_SUCCESS) {
            return FALSE;
        }
    }

    //
    // Register the RPMB with Storport if the device supports it
    //

    if ((UfsQueryDumpMode(adapterExtension) == FALSE) && 
        (adapterExtension->RpmbSupported)) {

        rpmbCapabilities.Version = STOR_RPMB_VERSION_1;
        rpmbCapabilities.Size = sizeof(STOR_RPMB_CAPABILITIES_DATA);
        rpmbCapabilities.SecurityProtocol = UFS_RPMB_SECURITY_PROTOCOL;
        rpmbCapabilities.SecurityProtocolSpecifier = UFS_RPMB_SECURITY_PROTOCOL_SPECIFIER;
        
#pragma warning(suppress: 25024)
        rpmbTarget = ((PSTOR_ADDR_BTL8)&rpmbCapabilities.RpmbTarget);
        rpmbTarget->Type = STOR_ADDRESS_TYPE_BTL8;
        rpmbTarget->Port = 0;
        rpmbTarget->AddressLength = STOR_ADDR_BTL8_ADDRESS_LENGTH;
        rpmbTarget->Path = 0;
        rpmbTarget->Target = 0;
        rpmbTarget->Lun = UFS_RPMB_LUN_ID;

        rpmbCapabilities.RpmbSize = adapterExtension->RpmbSize;
        rpmbCapabilities.MaxReliableRpmbWriteSize = adapterExtension->MaxRpmbWriteSize;
        rpmbCapabilities.FrameFormat = StorRpmbFrameTypeStandard;

        status = StorPortInitializeRpmb(adapterExtension, NULL, &rpmbCapabilities);

        NT_ASSERT(status == STOR_STATUS_SUCCESS);
        if (status != STOR_STATUS_SUCCESS) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOLEAN
UfsPassiveInitialize(
    _In_ PVOID DeviceExtension
    )
/*+++

Routine Description:

    The Passive initialization routine. Initializes all the structures
    associated with the connected device, such as the Transfer Request List,
    Task Management Request List, and associated spinlocks.

Arguments:

    Device Extension - Ufs adapter extension structure

Return Value:

    Returns TRUE for successful initialization

--*/
{
    ULONG status = UfsInitializePower((PUFS_ADAPTER_EXTENSION)DeviceExtension);

    UNREFERENCED_PARAMETER(status);

    NT_ASSERT(status == STOR_STATUS_SUCCESS);

    return TRUE;
}

BOOLEAN
UfsResetBus(
    _In_ PVOID DeviceExtension,
    _In_ ULONG PathId
    )
/*+++

Routine Description:

    Reset the host controller. This is done by diabling the Host Controller
    Enable (HCE) register, then enabling it and reinitializing the Host
    Controller structures.

Arguments:

    Device Extension - StorUfs specific extension structure

    PathId - The PathId of the bus

Return Value:

    True on Success, False on Failure

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    BOOLEAN status = TRUE;

    UNREFERENCED_PARAMETER(PathId);

    //
    // We don't have to do anything here if we're in dump mode. We've already reinitialized
    // the controller
    //

    if (UfsQueryDumpMode(adapterExtension)) {
        return TRUE;
    }

    UfsAddDbgLog(adapterExtension,
                 'ResB',
                 0,
                 0,
                 0);

    adapterExtension->BusReset = TRUE;

    //
    // We can reset the device - don't know if this is necessary yet - so
    // I may remove this later...
    //

    UfsStopAdapter(adapterExtension);

    if (UfsStartAdapter(adapterExtension) == STATUS_UNSUCCESSFUL){
        // LogError: "Error Starting the Adapter"
        status = FALSE;
    }

    InterlockedCompareExchange(&adapterExtension->BusReset, 
                               UfsStandardMode, 
                               UfsBusResetMode);

    return status;
}

BOOLEAN
UfsInterrupt (
    _In_ PVOID DeviceExtension
    )
/*+++

Routine Description:

    Handle an interrupt - this either occurs when there is a completed Task
    Management or Transfer Request, or when there is an error. Identify the
    nature of the interrupt by reading the Interrupt Status (IS) register,
    handle the interrupt

Arguments:

    DeviceExtension - StorUfs specific extension structure

Return Value:

    True on Success, False on Failure

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    PUFS_HCI_REG reg = adapterExtension->UfsRegisters;
    ULONG i;
    BOOLEAN status = FALSE;    
    BOOLEAN reset = FALSE;
    UFS_HCI_IS is;
    UFS_HCI_IE ie;

    //
    // Once read, the Interrupt Status register clears, indicating to the Host
    // Controller that the interrupt is being serviced.
    //

    is.AsUlong = StorPortReadRegisterUlong(adapterExtension,
                                           &reg->Runtime.Is.AsUlong);

    if (is.AsUlong == 0) {
        return FALSE;
    }

    //
    // We want only the IS bits that aren't being handled synchronously
    //

    ie.AsUlong = StorPortReadRegisterUlong(adapterExtension,
                                           &reg->Runtime.Ie.AsUlong);

    is.AsUlong &= ie.AsUlong;

    StorPortWriteRegisterUlong(adapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               is.AsUlong);

    //
    // Check to see if a UIC Command Completed
    //

    if (is.UicCmdCompStatus) {
        StorPortIssueDpc(adapterExtension,
                         &adapterExtension->UicCompletionDpc,
                         NULL,
                         NULL);
    }

    //
    // We shouldn't really hit this, but if the ResetBus routine has been kicked
    // off already, exit the ISR.
    //

    if (InterlockedAnd(&adapterExtension->BusReset, UfsBusResetMode) != UfsStandardMode) {
        return TRUE;
    }

    //
    // If there were errors (noted by the IS register), handle the errors
    //

    reset = UfsHandleBusError(adapterExtension,
                              &is);
    
    //
    // Update the value of the BusReset status value in the AdapterExtension
    // if a Bus Reset is required.
    //

    InterlockedCompareExchange(&adapterExtension->BusReset, 
                               reset ? UfsBusResetMode : UfsStandardMode, 
                               UfsStandardMode);

    //
    // If there was a fatal error indicated by the Interrupt Status registers,
    // kick off the ResetBus routine
    //

    if (reset != FALSE) {
        if (UfsInternalResetBus(adapterExtension) == FALSE) {

            //
            // The ResetBus routine has already been kicked off. Quit out here.
            //
            
            return TRUE;
        }
    }

    //
    // Check to see if the UIC Link Lost Status bit is set. If the link is down,
    // let Storport know to reset the bus
    //

    if (is.UicLinkLostStatus == 1) {
        // Do we need to do something here?
    }

    //
    // Check to see if a Transfer Request completed, and notify Storport
    //

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        if (is.TransferReqStatus != 0) {
            for (i = 0; i < adapterExtension->Trl.ListSize; i++) {
                NT_ASSERT(i < MAX_REQUEST_LIST_SIZE);

                status = StorPortIssueDpc(adapterExtension,
                                          &adapterExtension->Trl.CompletionDpc[i],
                                          (PVOID)is.AsUlong,
                                          NULL);
                if (status != FALSE) {
                    break;
                }
            }
        }
    } else {
        UfsCompleteTransferDpc(NULL,
                               adapterExtension,
                               (PVOID)is.AsUlong,
                               NULL);
    }

    //
    // Check to see if a Task Management Request completed, and notify Storport
    //

    if (UfsQueryDumpMode(adapterExtension) == FALSE) {
        if (is.TaskMgmtReqCompStatus != 0) {
            for (i = 0; i < adapterExtension->Tmrl.ListSize; i++) {
                NT_ASSERT(i < MAX_TASK_MAN_LIST_SIZE);

                status = StorPortIssueDpc(adapterExtension,
                                          &adapterExtension->Tmrl.CompletionDpc[i],
                                          NULL,
                                          NULL);
                if (status != FALSE) {
                    break;
                }
            }
        }
    } else {
        UfsCompleteTaskManDpc(NULL,
                              adapterExtension,
                              NULL,
                              NULL);
    }

    //
    // Check to see if a UIC Command Completed
    //

    if (is.UicCmdCompStatus) {
        StorPortIssueDpc(adapterExtension,
                         &adapterExtension->UicCompletionDpc,
                         NULL,
                         NULL);
    }

    if (adapterExtension->BusReset != FALSE) {
        StorPortIssueDpc(adapterExtension,
                         &adapterExtension->ResetDpc,
                         NULL,
                         NULL);
    }

    return TRUE;
}

#pragma warning(push)
#pragma warning(disable:26010 26000)

SCSI_ADAPTER_CONTROL_STATUS
UfsAdapterControl (
    _In_ PVOID DeviceExtension,
    _In_ SCSI_ADAPTER_CONTROL_TYPE ControlType,
    _In_ PVOID Parameters
    )
/*+++

Routine Description:

    Callback function called by Storport that primarily performs power
    operations on the adapter (the Host Controller)

Arguments:

    Device Extension - StorUfs specific extension structure

    ControlType - Identifies the type of Adapter Control operation to perform

    Parameters - Parameters required for the Adapter Control operation

Return Value:

    The status of the operation, either ScsiAdapterControlSuccess or
    ScsiAdapterControlUnsuccessful

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    PSCSI_SUPPORTED_CONTROL_TYPE_LIST supportedList;
    PSTOR_ADAPTER_CONTROL_POWER adapterPower;
    SCSI_ADAPTER_CONTROL_STATUS status = ScsiAdapterControlSuccess;

    switch (ControlType) {

    case ScsiQuerySupportedControlTypes:

        supportedList = (PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters;

        supportedList->SupportedTypeList[ScsiQuerySupportedControlTypes] = TRUE;
        supportedList->SupportedTypeList[ScsiStopAdapter] = TRUE;
        supportedList->SupportedTypeList[ScsiRestartAdapter] = TRUE;
        supportedList->SupportedTypeList[ScsiAdapterPower] = TRUE;
#if POFX_ENABLED == 1
        supportedList->SupportedTypeList[ScsiAdapterPoFxPowerActive] = TRUE;
        supportedList->SupportedTypeList[ScsiAdapterPoFxPowerSetFState] = TRUE;
        supportedList->SupportedTypeList[ScsiAdapterSystemPowerHints] = TRUE;
#else
        supportedList->SupportedTypeList[ScsiAdapterPoFxPowerActive] = FALSE;
        supportedList->SupportedTypeList[ScsiAdapterPoFxPowerSetFState] = FALSE;
        supportedList->SupportedTypeList[ScsiAdapterSystemPowerHints] = FALSE;
#endif
        status = ScsiAdapterControlSuccess;
        break;

    case ScsiStopAdapter:

        //
        // We execute this if we receive a Stop IRP or during a power
        // transition if necessary
        //

        UfsStopAdapter(adapterExtension);
        status = ScsiAdapterControlSuccess;
        break;

    case ScsiRestartAdapter:

        //
        // This routine is called during the adapter power up sequence. We will
        // get a ScsiAdapterPower call during power up, so we don't have to do
        // anything here
        //

        status = ScsiAdapterControlSuccess;
        break;

    case ScsiAdapterPower:

        //
        // We've gotten a D0 or D3 power request
        //

        adapterPower = (PSTOR_ADAPTER_CONTROL_POWER)Parameters;
        status = ScsiAdapterControlSuccess;

        if (adapterPower->PowerState == StorPowerDeviceD0) {
            status = UfsAdapterPowerUp(adapterExtension);
        } else if (adapterPower->PowerState == StorPowerDeviceD3) {
            status = UfsAdapterPowerDown(adapterExtension);
        } else {
            NT_ASSERT(FALSE);
            status = ScsiAdapterControlUnsuccessful;
        }

        break;

    case ScsiAdapterPoFxPowerActive:

        //
        // This is the active/ idle notification from PoFx
        //

        status = UfsAdapterPowerActive(adapterExtension, 
                                       (PSTOR_POFX_ACTIVE_CONTEXT)Parameters);
        break;

    case ScsiAdapterPoFxPowerSetFState:

        //
        // PoFx is requesting that we transtition to a new F-State
        //

        status = UfsAdapterPowerSetFState(adapterExtension,
                                          (PSTOR_POFX_FSTATE_CONTEXT)Parameters);
        break;

    case ScsiAdapterSystemPowerHints:

        status = UfsAdapterSystemPowerHints(adapterExtension,
                                            (PSTOR_SYSTEM_POWER_HINTS)Parameters);
        break;

    default:

        // LogError: "Ufs: AdapterControl - Unsupported or unknown ControlType received (0x%08x)\n"
        NT_ASSERT(FALSE);
        status = ScsiAdapterControlUnsuccessful;
    }

    return status;
}

SCSI_UNIT_CONTROL_STATUS
UfsUnitControl (
    _In_ PVOID DeviceExtension,
    _In_ SCSI_UNIT_CONTROL_TYPE ControlType,
    _In_ PVOID Parameters
    )
/*+++

Routine Description:

    Callback function called by Storport that primarily performs power
    operations on the unit (the Memory). Right now, we only support the Unit
    Start Function, but will support the PoFxPowerInfo, PoFxPowerRequired,
    PxFxPowerActive, and PoFxPowerSetFState functions in the future.

    Note: work still needs to be done before enabling the Runtime Power
          Management

Arguments:

    Device Extension - StorUfs specific extension structure

    ControlType - Identifies the type of Unit Control operation to perform

    Parameters - Parameters required for the Unit Control operation

Return Value:

    The status of the operation, either ScsiUnitControlSuccess or
    ScsiUnitControlUnsuccessful


--*/
{
    PSCSI_SUPPORTED_CONTROL_TYPE_LIST controlTypeList;
    SCSI_UNIT_CONTROL_STATUS status = ScsiUnitControlSuccess;

    UNREFERENCED_PARAMETER(DeviceExtension);

    switch (ControlType) {

    case ScsiQuerySupportedUnitControlTypes:

        controlTypeList = (PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters;

        //
        // Report the supported Control Types
        //

        controlTypeList->SupportedTypeList[ScsiQuerySupportedUnitControlTypes] = TRUE;
        controlTypeList->SupportedTypeList[ScsiUnitStart] = TRUE;
        controlTypeList->SupportedTypeList[ScsiUnitPower] = TRUE;
        status = ScsiUnitControlSuccess;
        break;

    case ScsiUnitStart:
        
        //
        // The device is started once we initialize the adapter. So we don't
        // need to do anything here.
        //

        status = ScsiUnitControlSuccess;
        break;

    case ScsiUnitPower:

        //
        // Handle a LUN Power IRP. This is a no-op since we don't do LUN level
        // power management
        //

        status = ScsiUnitControlSuccess;
        break;

    default:
        
        // LogError: "Ufs: UnitControl - Unsupported or Unknown ControlType Received (0x%08x)\n"
        NT_ASSERT(FALSE);
        status = ScsiUnitControlUnsuccessful;
    }

    return status;
}

#pragma warning(pop)

VOID
UfsRegistryReadControllerSettings(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Read the configuration registry values. These include hack flags, as well
    as tunable values like the Interrupt Aggregation parameters.

    The interrupt aggregation parameters allow us to figure the best parameters
    to maximize performance real-time. This function reads the registry for
    those user defined values. It fills in the UTRIACR structure, but doesn't
    write to the register.

Arguments:

    AdapterExtension - StorUfs specific extension structure

Return Value:

    N/A

--*/
{
    ULONG dataLength;
    PVOID data;
    ULONG regDwordData = 0;
    ULONG status;

    data = &regDwordData;
    dataLength = sizeof(regDwordData);

    AdapterExtension->FeatureFlags = STORUFS_REG_FEATURE_FLAGS_DEFAULT;
 
    status = StorPortRegistryReadAdapterKey(AdapterExtension,
                                           NULL,
                                           (PUCHAR)STORUFS_REG_FEATURE_FLAGS,
                                           REG_DWORD,
                                           &data,
                                           &dataLength);

    if (status == STOR_STATUS_SUCCESS) {
        AdapterExtension->FeatureFlags = regDwordData;
    }

    AdapterExtension->RegValues.HighSpeedGearMode = STORUFS_REG_HIGH_SPEED_GEAR_DEFAULT;
    status = StorPortRegistryReadAdapterKey(AdapterExtension,
                                            NULL,
                                            (PUCHAR)STORUFS_REG_HIGH_SPEED_GEAR,
                                            REG_DWORD,
                                            &data,
                                            &dataLength);

    if (status == STOR_STATUS_SUCCESS) {
        AdapterExtension->RegValues.HighSpeedGearMode = regDwordData;
    }

    AdapterExtension->RegValues.HSSeries = STORUFS_REG_HSSERIES_DEFAULT;
    status = StorPortRegistryReadAdapterKey(AdapterExtension,
                                            NULL,
                                            (PUCHAR)STORUFS_REG_HSSERIES,
                                            REG_DWORD,
                                            &data,
                                            &dataLength);

    if (status == STOR_STATUS_SUCCESS) {
        AdapterExtension->RegValues.HSSeries = (UFS_HSRATE_SERIES)regDwordData;
    }

    AdapterExtension->RegValues.NumLanes = STORUFS_REG_NUM_LANES_DEFAULT;
    status = StorPortRegistryReadAdapterKey(AdapterExtension,
                                            NULL,
                                            (PUCHAR)STORUFS_REG_NUM_LANES,
                                            REG_DWORD,
                                            &data,
                                            &dataLength);

    if (status == STOR_STATUS_SUCCESS) {
        AdapterExtension->RegValues.NumLanes = regDwordData;
    }
}

VOID
UfsGetControllerCapabilities(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Read the registers corresponding to the capabilities of the controller,
    including the Capabilities, Version, Product ID, Manufacturer ID, and
    Auto Hibernate settings.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    N/A

--*/
{
    PUFS_HCI_CAPABILITIES cap = &AdapterExtension->UfsRegisters->Cap;

    AdapterExtension->Capabilities.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                                       &cap->ContCap.AsUlong);

    AdapterExtension->Version.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                                  &cap->Ver.AsUlong);

    AdapterExtension->ProductID.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                                    &cap->ProdId.AsUlong);

    AdapterExtension->ManufacturerID.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                                         &cap->ManId.AsUlong);

    AdapterExtension->AutoH8Settings.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                                         &cap->AutoH8.AsUlong);

    UfsPhyGetCapabilities(AdapterExtension);
}

VOID
UfsGetRPMBCapabilities(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Read the descriptor table entries pertaining to the RPMB

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    N/A

--*/
{
    UFS_DEVICE_DESCRIPTOR deviceDescriptor = {0};
    UFS_GEOMETRY_DESCRIPTOR geometryDescriptor = {0};
    UFS_RPMB_UNIT_DESCRIPTOR rpmbDescriptor = {0};
    UFS_QUERY_OPCODE opcode = {0};
    ULONG trlResources;
    ULONGLONG temp;
    BOOLEAN originalPowerState;

    //
    // Because we haven't initialized PoFx yet, we need to let storufs know
    // that we can still send IO
    //

    originalPowerState = AdapterExtension->Power.Active;
    AdapterExtension->Power.Active = TRUE;

    //
    // Query the Device Descriptor to determine if RPMB is supported on the
    // device
    //

    opcode.DescriptorOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_READ_DESC;
    opcode.DescriptorOpcode.DescriptorIdn = UFS_DESC_DEVICE_IDN;
    opcode.DescriptorOpcode.Index = 0;
    opcode.DescriptorOpcode.Selector = 0;
    opcode.DescriptorOpcode.Length = _byteswap_ushort(sizeof(UFS_DEVICE_DESCRIPTOR));

    trlResources = UfsSendQueryUpiu(AdapterExtension,
                                    UFS_UPIU_QUERY_FUNCTION_READ,
                                    &opcode,
                                    sizeof(UFS_DEVICE_DESCRIPTOR),
                                    (PUCHAR)&deviceDescriptor);

    AdapterExtension->RpmbSupported = deviceDescriptor.bSecurityLU;

    if (AdapterExtension->RpmbSupported != FALSE) {

        //
        // If there's an RPMB, query the Geometry Descriptor to determine
        // the Max RPMB Write Size
        //

        opcode.DescriptorOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_READ_DESC;
        opcode.DescriptorOpcode.DescriptorIdn = UFS_DESC_GEOMETRY_IDN;
        opcode.DescriptorOpcode.Index = 0;
        opcode.DescriptorOpcode.Selector = 0;
        opcode.DescriptorOpcode.Length = _byteswap_ushort(sizeof(UFS_GEOMETRY_DESCRIPTOR));

        trlResources = UfsSendQueryUpiu(AdapterExtension,
                                        UFS_UPIU_QUERY_FUNCTION_READ,
                                        &opcode,
                                        sizeof(UFS_GEOMETRY_DESCRIPTOR),
                                        (PUCHAR)&geometryDescriptor);

        AdapterExtension->MaxRpmbWriteSize = geometryDescriptor.bRPMB_ReadWriteSize * 256;

        //
        // Query the RPMB Unit Descriptor to determine the RPMB size
        //

        opcode.DescriptorOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_READ_DESC;
        opcode.DescriptorOpcode.DescriptorIdn = UFS_DESC_UNIT_IDN;
        opcode.DescriptorOpcode.Index = UFS_RPMB_LUN_ID;
        opcode.DescriptorOpcode.Selector = 0;
        opcode.DescriptorOpcode.Length = _byteswap_ushort(sizeof(UFS_RPMB_UNIT_DESCRIPTOR));

        trlResources = UfsSendQueryUpiu(AdapterExtension,
                                        UFS_UPIU_QUERY_FUNCTION_READ,
                                        &opcode,
                                        sizeof(UFS_RPMB_UNIT_DESCRIPTOR),
                                        (PUCHAR)&rpmbDescriptor);

        NT_ASSERT(_byteswap_uint64(*((PULONGLONG)&rpmbDescriptor.qLogicalBlockCount[0])) <= 0xFFFFFFFF);

        temp = *((PULONGLONG)&rpmbDescriptor.qLogicalBlockCount[0]);

        AdapterExtension->RpmbSize = (1 << rpmbDescriptor.bLogicalBlockSize) * 
                                     (ULONG)_byteswap_uint64(*((PULONGLONG)&rpmbDescriptor.qLogicalBlockCount[0]));
    }

    AdapterExtension->Power.Active = originalPowerState;
}

NTSTATUS
UfsAllocateRequestLists(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PPORT_CONFIGURATION_INFORMATION ConfigInfo
    )
/*+++

Routine Description:

    Initializes the memory for the Transfer Request List, Task Management
    Request List, as well as their assoicated lock structures

Arguments:

    AdapterExtension - Storufs specific extension structure

    ConfigInfo - Port Configuration structure passed in by Storport

Return Value:

    N/A

--*/
{
    ULONG i, len;
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    ULONG uncachedExtensionSize;
    PUCHAR uncachedExtension = NULL;
    ULONG offset = 0;
    STOR_PHYSICAL_ADDRESS addr;

    tmrl->ListSize = MAX_TASK_MAN_LIST_SIZE;
    trl->ListSize = ConfigInfo->MaxIOsPerLun;

    //
    // Allocate the Uncached Extension, which contains pre-allocated memory for
    // the request lists and UCDs (of worst case size)
    //

    uncachedExtensionSize = (BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TMRD) * tmrl->ListSize) +
                             BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TRD) * trl->ListSize) +
                             (CMD_BASE_ADDR_ALIGNED(MAX_SIZEOF_UFS_UPIU_UCD) *
                              trl->ListSize)) + BASE_ADDR_MASK;

    uncachedExtension = StorPortGetUncachedExtension(AdapterExtension,
                                                     ConfigInfo,
                                                     uncachedExtensionSize);
    addr = StorPortGetPhysicalAddress(AdapterExtension,
                                        NULL,
                                        (PVOID)uncachedExtension,
                                        &len);

    AdapterExtension->UncachedExtension = uncachedExtension;
    RtlZeroMemory(uncachedExtension, uncachedExtensionSize);

    //
    // Make sure that the physical address is correctly aligned, otherwise set
    // the offset to align it correctly
    //

    offset = addr.LowPart & BASE_ADDR_MASK;

    if (offset > 0) {
        offset = BASE_ADDR_MASK - offset + 1;
    }

    //
    // Allocate and initialize the UTP Task Management Request List (UTMRL)
    //

    NT_ASSERT((offset + BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TMRD) * tmrl->ListSize)) <= uncachedExtensionSize);
    tmrl->List = (PUFS_HCI_TMRD)(uncachedExtension + offset);
    tmrl->PhysicalAddr = StorPortGetPhysicalAddress(AdapterExtension,
                                                    NULL,
                                                    (PVOID)tmrl->List,
                                                    &len);
    NT_ASSERT((tmrl->PhysicalAddr.LowPart & BASE_ADDR_MASK) == 0);

    offset += BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TRD) * trl->ListSize);

    tmrl->BuildingRequests = 0;
    tmrl->ExecutingRequests = 0;
    tmrl->ProcessingRequests = 0;

    //
    // Allocate and initialize the UTP Transfer Request List (UTRL)
    //

    NT_ASSERT((offset + BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TRD) * trl->ListSize)) <= uncachedExtensionSize);
    trl->List = (PUFS_HCI_TRD)(uncachedExtension + offset);
    trl->PhysicalAddr = StorPortGetPhysicalAddress(AdapterExtension,
                                                   NULL,
                                                   (PVOID)trl->List,
                                                   &len);
    NT_ASSERT((trl->PhysicalAddr.LowPart & BASE_ADDR_MASK) == 0);

    offset += BASE_ADDR_ALIGNED(sizeof(UFS_HCI_TRD) * trl->ListSize);

    //
    // Allocate and initialize the UTP Command Descriptor (UCD)
    //

    for (i = 0; i < trl->ListSize; i++) {
        NT_ASSERT(i < MAX_REQUEST_LIST_SIZE);
        NT_ASSERT((offset + CMD_BASE_ADDR_ALIGNED(MAX_SIZEOF_UFS_UPIU_UCD)) <= uncachedExtensionSize);
        trl->UcdList[i].VirtualAddress = uncachedExtension + offset;
        trl->UcdList[i].PhysicalAddress = StorPortGetPhysicalAddress(AdapterExtension,
                                                                     NULL,
                                                                     trl->UcdList[i].VirtualAddress,
                                                                     &len);
        NT_ASSERT((trl->UcdList[i].PhysicalAddress.LowPart & CMD_BASE_ADDR_MASK) == 0);

        offset += CMD_BASE_ADDR_ALIGNED(MAX_SIZEOF_UFS_UPIU_UCD);
    }

    trl->BuildingRequests = 0;
    trl->ExecutingRequests = 0;
    trl->ExecutingSyncRequests = 0;
    trl->ProcessingRequests = 0;

    AdapterExtension->ActiveUicCmd = 0;

    return STATUS_SUCCESS;
}

VOID
UfsInitializeRequestLists(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Routine for initialize the Request List Queues. It will write the address
    of the list to the UFS Host Controller Registers.

Arguments:

    AdapterExtension - StorUfs specific extension structure

Return Value:

    N/A

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    UFS_HCI_TRANSFER_REQUEST transfer = {0};
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    UFS_HCI_TASK_MGMT taskman = {0};

    //
    // Make sure the Run-Stop Registers are disabled
    //

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TransReq.RunStop.AsUlong,
                               0);
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TaskMgmt.RunStop.AsUlong,
                               0);

    //
    // Program the Base Addresses of the TRL and TMRL
    //

    transfer.BaseAddr.TransferReqListBaseAddress = trl->PhysicalAddr.LowPart;
    transfer.UpperBaseAddr.TransferReqListUpperBaseAddress = trl->PhysicalAddr.HighPart;

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TransReq.BaseAddr.AsUlong,
                               transfer.BaseAddr.AsUlong);

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TransReq.UpperBaseAddr.AsUlong,
                               transfer.UpperBaseAddr.AsUlong);

    taskman.BaseAddr.TaskMgmtReqListBaseAddress = tmrl->PhysicalAddr.LowPart;
    taskman.UpperBaseAddr.TaskMgmtListUpperBaseAddress = tmrl->PhysicalAddr.HighPart;

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TaskMgmt.BaseAddr.AsUlong,
                               taskman.BaseAddr.AsUlong);

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TaskMgmt.UpperBaseAddr.AsUlong,
                               taskman.UpperBaseAddr.AsUlong);
}

VOID
UfsInitializeDpcs(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Initializes the DPCs for the Transfer and Task Management Request Lists

Arguments:

    AdapterExtension - Storufs specific extension structure

Return Value:

    N/A

--*/
{
    ULONG i;
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;

    for (i = 0; i < AdapterExtension->Tmrl.ListSize; i++) {
        NT_ASSERT(i < MAX_TASK_MAN_LIST_SIZE);
        StorPortInitializeDpc(AdapterExtension,
                              &tmrl->CompletionDpc[i],
                              UfsCompleteTaskManDpc);
    }


    for (i = 0; i < AdapterExtension->Trl.ListSize; i++) {
        NT_ASSERT(i < MAX_REQUEST_LIST_SIZE);
        StorPortInitializeDpc(AdapterExtension,
                              &trl->CompletionDpc[i],
                              UfsCompleteTransferDpc);
    }


    StorPortInitializeDpc(AdapterExtension,
                          &AdapterExtension->UicCompletionDpc,
                          UfsCompleteUicCommandDpc);

    StorPortInitializeDpc(AdapterExtension,
                          &AdapterExtension->ResetDpc,
                          UfsResetBusDpc);

}

VOID
UfsResetBusDpc(
    _In_ PSTOR_DPC Dpc,
    _In_ PVOID DeviceExtension,
    _In_ PVOID SystemArg1,
    _In_ PVOID SystemArg2
    )
/*+++

Routine Description:

    Reset the host controller. This is done by diabling the Host Controller
    Enable (HCE) register, then enabling it and reinitializing the Host
    Controller structures.

Arguments:

    Dpc - Unused

    Device Extension - StorUfs specific extension structure

    SystemArg1 - Unused

    SystemArg2 - Unused

Return Value:

    True on Success, False on Failure

--*/
{
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArg1);
    UNREFERENCED_PARAMETER(SystemArg2);

    UfsResetBus(DeviceExtension, 0);
}

VOID
UfsCompleteTaskManDpc(
    _In_opt_ PSTOR_DPC Dpc,
    _In_ PVOID DeviceExtension,
    _In_opt_ PVOID SystemArg1,
    _In_opt_ PVOID SystemArg2
    )
/*+++

Routine Description:

    Handle a completed task management request. This list is primarily used
    for internal operations or LUN Resets

Arguments:

    Dpc - the DPC structure associated with the call. This argument is ignored.

    DeviceExtension - pointer to the StorUfs specific extension structure

    SystemArg1 - unused

    SystemArg2 - unused

Return Value:

    N/A

Required IRQL:

    Dispatch

--*/
{
    ULONG i;
    ULONG tmrlResources;
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArg1);
    UNREFERENCED_PARAMETER(SystemArg2);

    //
    // Handle the completed I/Os at dispatch level.
    //

    tmrlResources = UfsAcquireTaskManCompletionIndex(adapterExtension);

    if (tmrlResources != 0) {
        for (i = 0; i < adapterExtension->Tmrl.ListSize; i++){
            if (tmrlResources & (1 << i)){
                UfsCompleteTaskManRequest(adapterExtension,
                                          i);
            }
        }
    }

    //
    // Update the AdapterExtension variable to mark TMRL entries as available
    //

    if (tmrlResources){
        UfsReleaseTaskManCompletionIndex(adapterExtension,
                                         tmrlResources);
    }
}

VOID
UfsCompleteTransferDpc(
    _In_opt_ PSTOR_DPC Dpc,
    _In_ PVOID DeviceExtension,
    _In_opt_ PVOID SystemArg1,
    _In_opt_ PVOID SystemArg2
    )
/*+++

Routine Description:

    DPC for completing all outstanding Transfer Requests. This function will
    continue to complete requests until all outstanding requests have been
    completed (even if they were called by a different interrupt).

Arguments:

    Dpc - the DPC structure associated with the call. This argument is ignored.

    DeviceExtension - pointer to the StorUfs specific extension structure

    SystemArg1 - value of the Interrupt Status (IS) register

    SystemArg2 - unused

Return Value:

    N/A

Required IRQL:

    Dispatch

--*/
{
    ULONG i;
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    ULONG trlResources = 0;
    UFS_HCI_IS is;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArg2);

    is.AsUlong = (ULONG)SystemArg1;

    //
    // Once finished handling the current outstanding I/Os, check again to see
    // if any new I/Os completed, and repeat until there are no more
    // outstanding I/Os.
    //

    trlResources = UfsAcquireTransferCompletionIndex(adapterExtension,
                                                     FALSE);

    if (trlResources != 0) {
        for (i = 0; i < adapterExtension->Trl.ListSize; i++) {
            if (trlResources & (1 << i)) {
                UfsCompleteTransferRequest(adapterExtension,
                                           i,
                                           is);
            }
        }
    }

    //
    // Update the AdapterExtension variable to mark the TRL entries as available
    //

    UfsReleaseTransferCompletionIndex(adapterExtension,
                                      trlResources);
}

VOID
UfsCompleteUicCommandDpc(
    _In_opt_ PSTOR_DPC Dpc,
    _In_ PVOID DeviceExtension,
    _In_opt_ PVOID SystemArg1,
    _In_opt_ PVOID SystemArg2
    )
/*+++

Routine Description:

    Complete a command to the UFS Interconnect Layer (UIC). These commands are
    internal commands, mainly dealing with power management.

Arguments:

        Dpc - the DPC structure associated with the call. This is ignored.

        DeviceExtension - pointer to the StorUfs specific extension structure

        SystemArg1 (NULL) - unused

        SystemArg2 (NULL) - unused

    Return Value:

        N/A

--*/
{
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    PUFS_HCI_REG reg = adapterExtension->UfsRegisters;
    UFS_HCI_UICCMDARG arg;
    UFS_HCI_UICCMD cmd;
    ULONG errorCode = 0;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArg1);
    UNREFERENCED_PARAMETER(SystemArg2);

    //
    // If this is a synchronous UIC call, signal the event to be complete, and
    // exit the DPC
    //

    if (adapterExtension->UicSync != FALSE) {
        return;
    }

    NT_ASSERT(adapterExtension->UicSync == FALSE);

    //
    // Read the UIC ARG2 register to retrieve the ErrorCode
    //

    arg.AsUlong = StorPortReadRegisterUlong(adapterExtension,
                                            &reg->UicCmd.Args[1].AsUlong);

    cmd.AsUlong = StorPortReadRegisterUlong(adapterExtension,
                                            &reg->UicCmd.Cmd.AsUlong);

    errorCode = arg.AsUlong & 0x000000FF;

    if (errorCode != UIC_GENERIC_SUCCESS) {
        // LogError: "UIC Command:0x%08x Failed with Error Code:0x%08x\n"
        NT_ASSERT(FALSE);
    }

    //
    // Signal the release of the UIC Command resources
    //

    InterlockedCompareExchange(&adapterExtension->ActiveUicCmd, 0, 1);
    NT_ASSERT(adapterExtension->ActiveUicCmd == 0);

}

BOOLEAN
UfsHandleBusError(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_HCI_IS Is
    )
/*+++

Routine Description:

    Identify the type of bus error (if any), and handle it accordingly.

Arguments:

    AdapterExtension - pointer to the StorUfs specific extension structure

    Is - the value of the Interrupt Status (IS) register

Return Value:

    Boolean indicating whether we need to notify Storport of the error

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UECDL dl;
    UFS_HCI_UECDME dme;
    UFS_HCI_UECN net;
    UFS_HCI_UECT trans;
    UFS_HCI_UECPA pa;
    BOOLEAN reset = FALSE;

    //
    // UIC Error: UIC Error bit in the Interrupt status
    //

    if (Is->UicError == 1) {

        //
        // PA_INIT_ERROR in the UECPA and UECDL are fatal, and we want to reset
        // the adapter (UfsResetBus). Otherwise, the controller will handle
        // Data Link and Physical Layer errors.
        //

        dl.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                               &reg->Runtime.DataErr.AsUlong);
        pa.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                               &reg->Runtime.PhysErr.AsUlong);

        if (pa.Error == 1) {
            // LogError: "Physical Layer Error - Error Code: 0x%08x\n"
            UfsAddDbgLog(AdapterExtension,
                         'ErHw',
                         Is->AsUlong,
                         UFS_PHYSICAL_ERROR,
                         pa.ErrorCode);
        }

        if (dl.Error == 1) {
            // LogError: "Data Link Layer Error - Error Code: 0x%08x\n"
            UfsAddDbgLog(AdapterExtension,
                         'ErHw',
                         Is->AsUlong,
                         UFS_DATA_LINK_ERROR,
                         dl.ErrorCode);

            if (dl.ErrorCode == DATA_ERR_PA_INIT_ERROR){
                reset = TRUE;
                NT_ASSERT(FALSE);
            }
        }

        dme.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->Runtime.DmeErr.AsUlong);
        net.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->Runtime.NetErr.AsUlong);
        trans.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                  &reg->Runtime.TransErr.AsUlong);

        //
        // For UECN and UECT errors have something to do with the Unipro layer
        // going down. While not necessarily fatal, at this point, we'll go
        // ahead and reset the bus.
        //

        if (net.Error == 1){
            // LogError: "Network Layer Error - Error Code: 0x%08x\n"
            UfsAddDbgLog(AdapterExtension,
                         'ErHw',
                         Is->AsUlong,
                         UFS_NETWORK_ERROR,
                         net.ErrorCode);
            reset = TRUE;
            NT_ASSERT(FALSE);
        }

        if (trans.Error == 1) {
            // LogError: "Transport Layer Error - Error Code: 0x%08x\n"
            UfsAddDbgLog(AdapterExtension,
                         'ErHw',
                         Is->AsUlong,
                         UFS_TRANSPORT_ERROR,
                         trans.ErrorCode);
            reset = TRUE;
            NT_ASSERT(FALSE);
        }

        //
        // A DME error will most likely indicate an error with a UIC command
        // (such as DME_RESET or DME_LINKSTARTUP). This is also indicated
        // by a failed UIC command, so we'll just make note here and handle
        // it in the UIC completion routine.
        //

        if (dme.Error == 1){
            // LogError: "Device Manager Error - Error Code: 0x%08x\n"
            UfsAddDbgLog(AdapterExtension,
                         'ErHw',
                         Is->AsUlong,
                         UFS_DME_ERROR,
                         dme.ErrorCode);
        }
    }

    //
    // Host Controller Error - the bus needs to be reset
    //

    if (Is->HostContFatalErrorStatus == 1) {
        // LogError: "Fatal Host Controller Error\n"
        UfsAddDbgLog(AdapterExtension,
                     'ErHw',
                     Is->AsUlong,
                     UFS_HCFES_ERROR,
                     0);
        reset = TRUE;
        NT_ASSERT(FALSE);
    }

    //
    // Device Error - the bus needs to be reset
    //

    if (Is->DeviceFatalErrorStatus == 1) {
        // LogError: "Fatal Device Error\n"
        UfsAddDbgLog(AdapterExtension,
                     'ErHw',
                     Is->AsUlong,
                     UFS_DFES_ERROR,
                     0);
        reset = TRUE;
        NT_ASSERT(FALSE);
    }

    return reset;
}

VOID
UfsClearOutstandingRequests(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Lun
    )
/*+++

Routine Description:

    Routine for clearing all outstanding requests due to a logical unit reset
    or because of a bus reset.

Arguments:

    AdapterExtension - StorUfs specific extension structure

    Lun - Target lun for clearing requests. If INVALID_LUN, clear all requests

Return Value:

    N/A

--*/
{
    ULONG i;
    PSCSI_REQUEST_BLOCK tSrb;
    UFS_HCI_UTRLCLR clr = {0};
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    KLOCK_QUEUE_HANDLE lock;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    PUFS_UPIU_COMMAND transCmd;
    //PUFS_UPIU_TASK_MAN taskCmd;
    //UCHAR srbStatus = SRB_STATUS_SUCCESS;

    //
    // Go through and cancel all Transfer Requests
    //

    KeAcquireInStackQueuedSpinLock(&trl->RequestsLock,
                                   &lock);
#if 0
    //
    // Send an abort UPIU to cancel all outstanding requests. Because we are in
    // this function for error recovery, we don't really care if the request 
    // succeeds. If it does, then great, otherwise we'll have to reinitialize
    // the controller anyways, and aborting the requests won't matter.
    //

    for (i = 0; i < MAX_NUM_LU; i++) {
        if ((i == Lun) || (Lun == INVALID_LUN)) {
            srbStatus = UfsSendTaskManRequest(AdapterExtension,
                                              0,
                                              i,
                                              UPIU_TM_FUNCTION_ABORT_SET,
                                              NULL);

            NT_ASSERT(srbStatus == SRB_STATUS_SUCCESS);
        }
    }
#endif
    //
    // Now that the controller knows that the requests have been cancelled,
    // we need to notify this change to storport. Go through the executing 
    // requests, and cancel either all requests (if Lun == INVALID_LUN) or all 
    // requests of the specified LUN
    //

    for (i = 0; i < AdapterExtension->Trl.ListSize; i++) {
        NT_ASSERT(i < MAX_REQUEST_LIST_SIZE);
        tSrb = AdapterExtension->Trl.ActiveSrb[i];
        transCmd = (PUFS_UPIU_COMMAND)trl->UcdList[i].VirtualAddress;

        if (((trl->ExecutingRequests & (1 << i)) != 0) ||
            ((trl->ExecutingSyncRequests & (1 << i)) != 0)) {

            if ((Lun == INVALID_LUN) || (transCmd->Header.LUN == Lun)) {
                //
                // Let the Host Controller know that we are no longer using
                // these resources
                //

                trl->ExecutingRequests &= ~(1 << i);
                trl->ExecutingSyncRequests &= ~(1 << i);

                NT_ASSERT((trl->BuildingRequests &
                           trl->ExecutingRequests &
                           trl->ExecutingSyncRequests &
                           trl->ProcessingRequests) == 0);

                clr.AsUlong = ~(1 << i);
                StorPortWriteRegisterUlong(AdapterExtension,
                                           &reg->TransReq.Clear.AsUlong,
                                           clr.AsUlong);

                if (tSrb != NULL) {
                    // LogError: "Cancelling SRB:0x%0IX at Index:0x%08x\n"

                    //
                    // We are cancelling multiple outstanding IOs. We don't know
                    // which one caused the error - set SRB_STATUS_BUS_RESET
                    //

                    tSrb->SrbStatus = SRB_STATUS_ERROR;

                    StorPortNotification(RequestComplete,
                                         AdapterExtension,
                                         tSrb);
                    AdapterExtension->Trl.ActiveSrb[i] = NULL;
                } else {
                    // LogError: "Cancelling IO at Index:0x%08x\n"
                }
            }
        }
    }

    KeReleaseInStackQueuedSpinLock(&lock);

    //
    // Now go through and cancel all outstanding Task Management Requests
    //

    KeAcquireInStackQueuedSpinLock(&tmrl->RequestsLock,
                                   &lock);
    //
    // Go through the executing requests, and cancel either all requests (if
    // Lun == INVALID_LUN) or all requests of the specified LUN
    //
#if 0
    for (i = 0; i < AdapterExtension->Tmrl.ListSize; i++) {
        tSrb = AdapterExtension->Tmrl.ActiveSrb[i];
        taskCmd = &tmrl->List[i].Request;

        if ((tmrl->ExecutingRequests & (1 << i)) != 0) {
            
            if ((Lun == INVALID_LUN) || (taskCmd->Header.LUN == Lun)) {
                //
                // Let the Host Controller know that we are no longer using
                // these resources
                //

                tmrl->ExecutingRequests &= ~(1 << i);

                NT_ASSERT((tmrl->BuildingRequests &
                           tmrl->ExecutingRequests &
                           tmrl->ProcessingRequests) == 0);

                clr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                        &reg->TaskMgmt.Clear.AsUlong);
                clr.AsUlong &= ~(1 << i);
                StorPortWriteRegisterUlong(AdapterExtension,
                                           &reg->TaskMgmt.Clear.AsUlong,
                                           clr.AsUlong);

                if (tSrb != NULL) {

                    //
                    // We are cancelling multiple outstanding IOs. We don't know
                    // which one caused the error - set SRB_STATUS_BUS_RESET
                    //

                    tSrb->SrbStatus = SRB_STATUS_ERROR;

                    StorPortNotification(RequestComplete,
                                         AdapterExtension,
                                         tSrb);
                    AdapterExtension->Tmrl.ActiveSrb[i] = NULL;
                }
            }
        }
    }
#endif
    KeReleaseInStackQueuedSpinLock(&lock);
}

BOOLEAN
UfsInternalResetBus (
    PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Internal ResetBus routine in an attempt to recover from a fatal
    link error.

Arguments:

    AdapterExtension - pointer to the StorUfs adapter extension structure

Return Value:

    True if reset is signalled, False if reset has already been triggered

--*/
{
    if (InterlockedAnd(&AdapterExtension->BusReset, UfsBusResetMode) == FALSE) {
        InterlockedCompareExchange(&AdapterExtension->BusReset, 
                                   UfsBusResetMode, 
                                   UfsStandardMode);

        StorPortNotification(LinkDown, 
                             AdapterExtension);
        
        StorPortIssueDpc(AdapterExtension,
                         &AdapterExtension->ResetDpc,
                         NULL,
                         NULL);

        return TRUE;
    }

    return FALSE;
}

VOID
UfsCompleteTaskManRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index
    )
/*+++

Routine Description:

    Complete the Task Management Request at the given index.

Arguments:

    AdapterExtension - pointer to the StorUfs adapter extension structure

    Index - the index of the Task Management Request

Return Value:

    N/A

Required IRQL:

    Dispatch

--*/
{
    PUFS_HCI_TMRD tmrd;
    PSCSI_REQUEST_BLOCK tSrb;
    PUFS_UPIU_TASK_MAN req;
    PUFS_UPIU_TASK_MAN resp;
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
    UCHAR srbStatus;

    NT_ASSERT(Index < MAX_TASK_MAN_LIST_SIZE);
    if (Index >= MAX_TASK_MAN_LIST_SIZE) {
        return;
    }
    

    tmrd = &tmrl->List[Index];
    req = &tmrd->Request;
    resp = &tmrd->Response;
    tSrb = tmrl->ActiveSrb[Index];

    //
    // Check for an error, and set the correct error code
    //

    if (tmrd->OverallCmdStatus == TMRD_OCS_SUCCESS) {

        switch (_byteswap_ulong(resp->Parameters[0])) {

        case UPIU_TM_RESPONSE_COMPLETE:
        case UPIU_TM_RESPONSE_SUCCEEDED:
            srbStatus = SRB_STATUS_SUCCESS;
            break;

        case UPIU_TM_RESPONSE_NOT_SUPPORTED:
            srbStatus = SRB_STATUS_INVALID_REQUEST;
            break;

        case UPIU_TM_RESPONSE_INCORRECT_LUN:
            srbStatus = SRB_STATUS_INVALID_LUN;
            break;

        case UPIU_TM_RESPONSE_FAILED:
        default:
            if (tmrd->Request.Header.QueryFunction == UPIU_TM_FUNCTION_ABORT){
                srbStatus = SRB_STATUS_ABORT_FAILED;
            } else {
                srbStatus = SRB_STATUS_ERROR;
            }
            break;
    
        }
    
    } else if ((tmrd->OverallCmdStatus == TMRD_OCS_FATAL_ERROR) ||
               (tmrd->OverallCmdStatus == TMRD_OCS_INVALID_OCS_VALUE)) {
    
        //
        // Otherwise, the controller borked out. We need to initiate the 
        // reset bus routine.
        //
        
        srbStatus = SRB_STATUS_ERROR;
        UfsInternalResetBus(AdapterExtension);
    } else {
        srbStatus = SRB_STATUS_ERROR;
    }
    
    if (tSrb != NULL) {
        if (tSrb->SrbStatus != SRB_STATUS_SUCCESS){
            // LogError: "Error handling Task Management Request for SRB:0x%0IX with Status:0x%08x\n"
        }
    
        UfsPostProcessSrb(AdapterExtension, tSrb, Index);
    
        StorPortNotification(RequestComplete,
                             AdapterExtension,
                             tSrb);
    
        tmrl->ActiveSrb[Index] = NULL;
    } else {
        if (srbStatus != SRB_STATUS_SUCCESS) {
            // LogError: "Internal Task Management Request:0x%08x Failed with Error Code:0x%08x\n"

            //
            // For now, just assert if the internal Task Man request failed.
            // a better infrastructure will come soon.
            //

            NT_ASSERT(FALSE);
            // TODO: Have a better internal cmd error mgmt routine...
        }
    }

    UfsAddDbgLog(AdapterExtension,
                 'TMCo',
                 Index,
                 (ULONG_PTR)tSrb,
                 _byteswap_ulong(resp->Parameters[0]));
}

VOID
UfsCompleteTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index,
    _In_ UFS_HCI_IS Is
    )
/*+++

Routine Description:

    Complete the SRB, setting the SRB status code in the SRB structure and
    copying the sense data for StorPort to use.

Arguments:

    AdapterExension - Pointer to the StorUfs extension for the adapter

    Index - The index of the completed Transfer Request

    Is - Interrupt Status Register value - corresponds to any errors while
         servicing the Transfer Request

Return Value:

    N/A

Required IRQL:

    Dispatch

--*/
{
    PUFS_HCI_TRD trd;
    PUFS_UPIU_RESPONSE resp;
    PSCSI_REQUEST_BLOCK tSrb;
    PUFS_UPIU_COMMAND cmd;
    UFS_HCI_UTRLCLR clr;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PVOID cmdBaseAddr;
    UCHAR srbStatus = SRB_STATUS_ERROR;

    NT_ASSERT(Index < MAX_REQUEST_LIST_SIZE);

    tSrb = trl->ActiveSrb[Index];
    trd = &trl->List[Index];

    cmdBaseAddr = trl->UcdList[Index].VirtualAddress;

    cmd = (PUFS_UPIU_COMMAND)cmdBaseAddr;
    resp = (PUFS_UPIU_RESPONSE)((PUCHAR)cmdBaseAddr + (trd->RespUPIUOff * sizeof(ULONG)));

    //
    // Check for an error, and set the correct error code
    //

    if (trd->OverallCmdStatus == TRD_OCS_SUCCESS) {
        
        switch (resp->Header.Status) {

        case SCSISTAT_GOOD:
            srbStatus = SRB_STATUS_SUCCESS;
            break;

        case SCSISTAT_TASK_ABORTED:
            srbStatus = SRB_STATUS_ABORTED;
            break;

        case SCSISTAT_BUSY:
            srbStatus = SRB_STATUS_BUSY;
            break;

        case SCSISTAT_CHECK_CONDITION:
        case SCSISTAT_RESERVATION_CONFLICT:
        case SCSISTAT_QUEUE_FULL:
        default:
            srbStatus = SRB_STATUS_ERROR;
            break;
        }
    } else if ((trd->OverallCmdStatus == TRD_OCS_FATAL_ERROR) ||
               (trd->OverallCmdStatus == TRD_OCS_INVALID_OCS_VALUE)) {

        if (Is.SysBusFatalErrorStatus){
            //
            // Check to see if the I/O failed because of a bad pointer to the
            // UCD. This is purely a software issue, that isn't really
            // recoverable.
            //
        
            clr.AsUlong = (1 << Index);
            StorPortWriteRegisterUlong(AdapterExtension,
                                       &reg->TransReq.Clear.AsUlong,
                                       clr.AsUlong);

            // LogError: "Bad Pointer in the Transfer Request for SRB:0x%0IX\n"
            NT_ASSERT(FALSE);
            
        } else {
            //
            // Otherwise, the controller borked out. We need to initiate the 
            // reset bus routine.
            //
            
            srbStatus = SRB_STATUS_ERROR;
            UfsInternalResetBus(AdapterExtension);
        }
    } else {
        srbStatus = SRB_STATUS_ERROR;
    }

    if (tSrb != NULL) {

        tSrb->SrbStatus = srbStatus;
        tSrb->ScsiStatus = resp->Header.Status;
        tSrb->DataTransferLength -= _byteswap_ulong(resp->ResidualTransferCount);

        UfsPostProcessSrb(AdapterExtension, tSrb, Index);

        //
        // Copy the Sense Data to the SRB
        //

        if ((tSrb->SenseInfoBufferLength >= _byteswap_ushort(resp->SenseDataLength)) &&
            (tSrb->ScsiStatus != SCSISTAT_GOOD)) {

            RtlCopyMemory(tSrb->SenseInfoBuffer,
                          &resp->SenseData[0],
                          _byteswap_ushort(resp->SenseDataLength));
            tSrb->SrbStatus |= SRB_STATUS_AUTOSENSE_VALID;
        }

        if (tSrb->SrbStatus != SRB_STATUS_SUCCESS) {
            // LogError: "Transfer Request SRB:0x%0IX Failed with Status Code:0x%08x\n"
        }

        //
        // Send the completed SRB with associated data back to Storport
        //

        StorPortNotification(RequestComplete,
                             AdapterExtension,
                             tSrb);
        trl->ActiveSrb[Index] = NULL;
    } else {
        //
        // The only time we get here is for the NOP IN UPIU, QUERY REQUEST 
        // UPIUs, and START_STOP_UNIT COMMAND UPIUs to the Device Well-Known
        // LUN for power management.
        //
    
        NT_ASSERT(trd->OverallCmdStatus == TRD_OCS_SUCCESS);

        //
        // TODO: Investigate
        // This was removed because the SSU power request returns Check Condition
        //

        // NT_ASSERT(resp->Header.Status == SCSISTAT_GOOD);
    
        // TODO: Have a better internal cmd error mgmt routine...
    }

    UfsAddDbgLog(AdapterExtension,
                 'TrCo',
                 Index,
                 (ULONG_PTR)tSrb,
                 resp->Header.Status);
}

VOID
UfsHostControllerEnable(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Enable the host controller. This entails setting the Host Controller Enable
    bit in the HCE register, enabling UIC commands, and checking the Host
    Controller Status (HCS) register for device present.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    True on success, False on failure

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    PUFS_MPHY_CONFIG mPhyState = &AdapterExtension->MPHYState;
    UFS_HCI_HCE hce;
    UFS_HCI_IE ie;
    UFS_HCI_HCS hcs;

    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);

    //
    // Disable Host Controller
    //

    hce.AsUlong = 0;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Hce.AsUlong,
                               hce.AsUlong);

    hce.AsUlong = 1;

    while (hce.HceEnable != 0) {
        UfsDelay(10);

        //
        // Wait until HCE is read as 0
        //

        hce.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->Runtime.Hce.AsUlong);
    }











    //
    // Enable Host Controller
    //

    hce.HceEnable = 1;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Hce.AsUlong,
                               hce.AsUlong);

    hce.AsUlong = 0;

    while (hce.HceEnable != 1) {
        UfsDelay(10);

        //
        // Wait until HCE is read as 1
        //

        hce.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->Runtime.Hce.AsUlong);
    }

    NT_ASSERT(hce.HceEnable == 1);





















    //
    // Set IE.UCCE to 1 to enable the IS.UCCS interrupt
    //

    ie.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                           &reg->Runtime.Ie.AsUlong);
    ie.UicCmdCompEnable = 0;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Ie.AsUlong,
                               ie.AsUlong);

    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);
    NT_ASSERT(hcs.UicCmdReady == 1);

    //
    // When enabling the controller, the link is in PWM-G1
    //

    mPhyState->PwmGear = 1;
    mPhyState->PwmModeEnabled = TRUE;
    mPhyState->HsModeEnabled = FALSE;
}

NTSTATUS
UfsStartAdapter(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Turn on the adapter. This will require initializing the Interrupt Enable
    Register (IE), UTP Interrupt Aggregation Register (UTRIACR), and the
    Task Man/ Transfer Request registers.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    True on success, False on failure

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_IE ie;
    UFS_HCI_UTRIACR utriacr;
    UFS_HCI_HCS hcs = { 0 };
    ULONG status;
    ULONG i, val, trlResources, j;
    UFS_QUERY_OPCODE Opcode;
    UFS_HCI_UTRLRSR utrlrsr = {0};
    UFS_HCI_UTMRLRSR utmrlrsr = {0};
    ULONG uicArgs[3] = {0};
    ULONG errorStatus;
    UFS_MPHY_CONFIG targetState = {0};
    UCHAR fDeviceInitData = 0;
























    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);

    UfsHostControllerEnable(AdapterExtension);












    //
    // Try initializing the link up to MAX_NUM_STARTUP_ATTEMPTS times, before
    // failing. This entails sending a DME_LINKSTARTUP command, and waiting to
    // see what the response is from the Host Controller.
    //

    for (i = 0; i < MAX_NUM_STARTUP_ATTEMPTS; i++) {
        status = UfsExecuteUicCommandSynchronous(AdapterExtension,
                                                 UIC_DME_LINKSTARTUP,
                                                 uicArgs,
                                                 &val);

        if (!NT_SUCCESS(status) || (val != UIC_GENERIC_SUCCESS)) {
            // LogError: "Adapter Initialization Failure - DME_LINKSTARTUP failed with error code: 0x%08x\n"
        }

        //
        // Check value of HCS.DP to make sure there is a device attached. If
        // enabled, break out of this link startup loop
        //

        hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                &reg->Runtime.Hcs.AsUlong);

        if (hcs.DevicePresent == 1) {
            break;
        }
    }

    if (i == MAX_NUM_STARTUP_ATTEMPTS) {
        // LogError: "Adapter Initialization Failure - Too Many Startup Attempts\n"

        return STATUS_UNSUCCESSFUL;
    }











ControllerConfiguration:

    //
    // Allocate the Task Management and Transfer Request Lists, set the Base
    // Addresses for each, and initialize the resource locks in AdapterExtension
    //

    UfsInitializeRequestLists(AdapterExtension);

    //
    // Now that the Host Controller is enabled, query the controller for its
    // capabilities (mostly as a sanity check)
    //

    UfsGetControllerCapabilities(AdapterExtension);

    //
    // Enable the Run Stop Registers to indicate that the HC should be ready
    // to receive IOs
    //

    utrlrsr.TransferReqListRunStop = 1;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TransReq.RunStop.AsUlong,
                               utrlrsr.AsUlong);

    utmrlrsr.TaskMgmtReqListRunStop = 1;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->TaskMgmt.RunStop.AsUlong,
                               utmrlrsr.AsUlong);

#if DBG
    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);
    NT_ASSERT(hcs.TransferReqListReady == 1);
    NT_ASSERT(hcs.TaskMgmtReqListReady == 1);
#endif

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               0xFFFFFFFF);

    //
    // Send a NOP OUT UPIU to the Host Controller, and wait until we receive
    // a NOP IN UPIU
    //

    trlResources = 0;

    //
    // List the adapter as active for the time being. We can technically send
    // these UPIUs to the controller even though the adapter isn't active.
    //

    AdapterExtension->Power.Active = TRUE;

    //
    // TODO: Figure out a more performant method of polling...
    //

    for (i = 0; i < MAX_NUM_STARTUP_ATTEMPTS; i++) {
        UfsSendNopOutUpiu(AdapterExtension);

        for (j = 0; j < UFS_COMMAND_TIMEOUT / 10; j++) {
            UfsDelay(10);

            UfsPollForCompletion(AdapterExtension, &trlResources);
            if (trlResources != 0){
                break;
            }
        }

        if (trlResources != 0) {
            break;
        }
    }

    if (i == MAX_NUM_STARTUP_ATTEMPTS) {
        // LogError: "Adapter Initialization Failure - Nop In UPIU timed out\n"

        return STATUS_UNSUCCESSFUL;
    }

    //
    // If supported, make the switch into High Speed Mode
    //

    if ((AdapterExtension->MPHYCapabilities.HsModeEnabled != FALSE) &&
        ((AdapterExtension->FeatureFlags & UFS_FEATURE_QUALCOMM_8996) == 0)) {

        targetState.HsGear = AdapterExtension->MPHYCapabilities.HsGear;
        targetState.HsModeEnabled = AdapterExtension->MPHYCapabilities.HsModeEnabled;
        targetState.Series = AdapterExtension->MPHYCapabilities.Series;
        targetState.RxDataLanes = AdapterExtension->MPHYCapabilities.RxDataLanes;
        targetState.TxDataLanes = AdapterExtension->MPHYCapabilities.TxDataLanes;

        errorStatus = UfsPhySetSpeedMode(AdapterExtension, &targetState);
        NT_ASSERT(errorStatus == UIC_GENERIC_SUCCESS);
    }







    //
    // Send a QUERY REQUEST UPIU to set the fDeviceInit Flag to finish device
    // initialization
    //

    RtlZeroMemory(&Opcode.FlagOpcode, sizeof(UFS_QUERY_OPCODE));
    Opcode.FlagOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_SET_FLAG;
    Opcode.FlagOpcode.FlagIdn = fDeviceInit;

    trlResources = 0;

    for (i = 0; i < MAX_NUM_STARTUP_ATTEMPTS; i++) {
        trlResources = UfsSendQueryUpiu(AdapterExtension,
                                        UFS_UPIU_QUERY_FUNCTION_WRITE,
                                        &Opcode,
                                        0,
                                        NULL);

        if (trlResources != 0) {
            break;
        }
    }

    if (i == MAX_NUM_STARTUP_ATTEMPTS) {
        // LogError: "Adapter Initialization Failure - FDeviceInit Query Request timed out\n"
        return STATUS_UNSUCCESSFUL;
    }














    //
    // Poll the fDeviceInit flag using a QUERY REQUEST UPIU until the flag
    // flips back to 0. This indicates the device finished initializing
    //

    RtlZeroMemory(&Opcode.FlagOpcode, sizeof(UFS_QUERY_OPCODE));
    Opcode.FlagOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_READ_FLAG;
    Opcode.FlagOpcode.FlagIdn = fDeviceInit;

    for (i = 0; i < MAX_NUM_QUERY_ATTEMPTS; i++) {
        trlResources = UfsSendQueryUpiu(AdapterExtension,
                                        UFS_UPIU_QUERY_FUNCTION_READ,
                                        &Opcode,
                                        0,
                                        &fDeviceInitData);

        NT_ASSERT(trlResources != 0);

        if (fDeviceInitData == 0) {
            break;
        }

        UfsDelay(5);
    }

    if (i == MAX_NUM_QUERY_ATTEMPTS) {
        // LogError: "Adapter Initialization Failure - FDeviceInit Query Request timed out\n"
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Now let PoFx set the proper Active/Idle state
    //

    AdapterExtension->Power.Active = FALSE;

FinalInit:

    AdapterExtension->AutoH8Enabled = FALSE;





        //
        // Configure the Auto Hibernate Settings Register
        //

        if (AdapterExtension->Capabilities.AutoH8Supported != FALSE) {


















            //
            // Only enable AutoHibern8 if we don't override it via feature flags
            //

            if (((AdapterExtension->FeatureFlags & UFS_FEATURE_ENABLE_SW_H8) == 0) &&
                ((AdapterExtension->FeatureFlags & UFS_FEATURE_DISABLE_H8) != 0)) {

                StorPortWriteRegisterUlong(AdapterExtension,
                                           &reg->Cap.AutoH8.AsUlong,
                                           AdapterExtension->AutoH8Settings.AsUlong);
                AdapterExtension->AutoH8Enabled = TRUE;
            }
        }




    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               0xFFFFFFFF);

    //
    // Enable other interrupts by programming the IE register
    //

    ie.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                           &reg->Runtime.Ie.AsUlong);
    ie.ErrorEnable = 1;
    ie.TransferRequestCompEnable = 1;
    ie.TaskMgmtReqCompEnable = 1;
    ie.UicErrorEnable = 1;
    ie.DeviceFatalErrorEnable = 1;
    ie.HCFatalErrorEnable = 1;
    ie.SysBusFatalErrorEnable = 1;
    ie.UicLinkLostStatusEnable = 1;

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Ie.AsUlong,
                               ie.AsUlong);






        //
        // Initialize the UTRIACR with desired valued for threshold (IACTH) and
        // timeout (IATOVAL)
        //

        utriacr.AsUlong = 0;

        utriacr.Enable = 1;
        utriacr.ParamWriteEnable = 1;
        utriacr.CtrTimerReset = 1;
        utriacr.CtrThresh = AdapterExtension->CtrThresh;
        utriacr.TimeoutVal = AdapterExtension->TimeoutVal;

        StorPortWriteRegisterUlong(AdapterExtension,
                                   &reg->Runtime.IntAgg.AsUlong,
                                   utriacr.AsUlong);




    return STATUS_SUCCESS;
}

VOID
UfsStopAdapter(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Disable the Ufs Host Controller. This is done by disabling all interrupts
    and the Host Controller Enable (HCE) register.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    True on success, False on failure

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_HCE hce;
    UFS_HCI_IE ie;
    UFS_HCI_IS is;

    //
    // Disable all interrupts
    //

    ie.AsUlong = 0;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Ie.AsUlong,
                               ie.AsUlong);

    is.AsUlong = 0xFFFFFFFF;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               is.AsUlong);

    UfsClearOutstandingRequests(AdapterExtension, INVALID_LUN);

    //
    // Diable the Host Controller Enable bit
    //

    hce.AsUlong = 0;
    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Hce.AsUlong,
                               hce.AsUlong);
}

VOID
UfsFreeResources (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*+++

Routine Description:

    Free all the resources associated with the AdapterExtension.

Arguments:

    AdapterExtension - StorUfs specific extension structure

Return Value:

    N/A

--*/
{
    UNREFERENCED_PARAMETER(AdapterExtension);
}
