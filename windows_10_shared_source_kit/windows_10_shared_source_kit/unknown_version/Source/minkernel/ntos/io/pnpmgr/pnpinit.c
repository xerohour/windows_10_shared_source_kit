/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pnpinit.c

Abstract:

    This module contains the plug-and-play initialization
    subroutines for the I/O system.


Author:

    Shie-Lin Tzong (shielint) 30-Jan-1995

Environment:

    Kernel mode

--*/

#include "pnpmgrp.h"
#include "ahcache.h"
#pragma hdrstop

#include <ntddstor.h>

#include <initguid.h>
#include <ntddramd.h>
#include <efi.h>
#include <esrt.h>
#include <smbboot.h>
#include <TraceLoggingProvider.h>
#include <telemetry\MicrosoftTelemetry.h>

TRACELOGGING_DECLARE_PROVIDER(BootProvider);

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'nipP')
#endif

extern UNICODE_STRING CmSymbolicLinkValueName;

#define REGSTR_PATH_SYSTEM_SETUP            TEXT("System\\Setup")
#define REGSTR_VAL_SYSTEM_SETUP_IN_PROGRESS TEXT("SystemSetupInProgress")
#define REGSTR_VAL_RESPECIALIZE             TEXT("Respecialize")
#define REGSTR_VAL_RESPECIALIZE_STARTED     TEXT("RespecializeStarted")
#define REGSTR_VAL_START_TYPE               TEXT("Start")
#define REGSTR_VAL_SETUP_PHASE              TEXT("SetupPhase")
#define REGSTR_VAL_SETUP_TYPE               TEXT("SetupType")
#define REGSTR_VAL_OOBE_IN_PROGRESS         TEXT("OOBEInProgress")
#define REGSTR_VAL_UPGRADE                  TEXT("Upgrade")
#define REGSTR_VAL_ROLLBACK_ACTIVE          TEXT("RollbackActive")


//
// Mask of LOAD_INFORMATION to BDCB_IMAGEFLAGS
//

#define BDCB_IMAGEFLAGS_MASK BDCB_IMAGEFLAGS_FAILED_CODE_INTEGRITY

PTREE_ENTRY IopGroupListHead;

//
// Timeout value for PnpFindBestConfiguration in milliseconds.
//
ULONG PnpFindBestConfigurationTimeout = 5000;

//
// Group order table
//

ULONG IopGroupIndex;
PLIST_ENTRY IopGroupTable;

//
// Group order cache list.
//

UNICODE_STRING *PiInitGroupOrderTable      = NULL;
USHORT          PiInitGroupOrderTableCount = 0;

//
// PnpBootOptions - holds various boot options read from the registry
//

ULONG PnpBootOptions = PNP_BOOT_OPTION_DEFAULT;

//
// Boot mode is active until file system and registry have been initialized.
//

BOOLEAN PnpBootMode = TRUE;

//
// System setup is in progress.
//

BOOLEAN PnpSetupInProgress = FALSE;
BOOLEAN PnpSetupPhaseInProgress = FALSE;
BOOLEAN PnpSetupTypeInProgress = FALSE;
BOOLEAN PnpSetupOOBEInProgress = FALSE;
BOOLEAN PnpSetupUpgradeInProgress = FALSE;
BOOLEAN PnpSetupRollbackActiveInProgress = FALSE;

//
// Hardware configuration guid of the system being booted on.
//

UNICODE_STRING PnpCurrentHardwareConfigurationGuidString = {0};
ULONG PnpCurrentHardwareConfigurationIndex = 0;

#ifdef ALLOC_DATA_PRAGMA
#pragma bss_seg("INITDATAZ")
#endif

//
// Current policy for loading drivers.
//

DWORD PnpDriverImageLoadPolicy;

#ifdef ALLOC_DATA_PRAGMA
#pragma bss_seg()
#endif

//
// Policy for loading drivers after they have been verified.
//

#define PNP_INITIALIZE_UNKNOWN_DRIVERS 0x1
#define PNP_INITIALIZE_BAD_CRITICAL_DRIVERS 0x2
#define PNP_INITIALIZE_BAD_DRIVERS 0x4
#define PNP_INITIALIZE_VALID_MASK \
    ((PNP_INITIALIZE_UNKNOWN_DRIVERS) | \
     (PNP_INITIALIZE_BAD_CRITICAL_DRIVERS) | \
     (PNP_INITIALIZE_BAD_DRIVERS))

#define PNP_INITIALIZE_DRIVERS_DEFAULT \
    (PNP_INITIALIZE_UNKNOWN_DRIVERS | PNP_INITIALIZE_BAD_CRITICAL_DRIVERS)

//
// Policy value used to work around GOOD == 0, but GP
// treats a value of 0 as needing to be deleted.
//

#define PNP_INITIALIZE_GOOD_GP_WORKAROUND 0x8

//
// Registry policy location path.
//

#define PNP_DRIVER_INITIALIZATION_POLICY_PATH \
        L"\\Registry\\Machine\\System\\CurrentControlSet\\Policies\\EarlyLaunch"

#define PNP_DRIVER_INITIALIZATION_POLICY_PATH_FALLBACK \
         L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\EarlyLaunch"

//
// Early Launch Driver Hive Location
//

#define PNP_EARLY_LAUNCH_HIVE_PATH L"\\Registry\\Machine\\ELAM"

//
// PnpSystemDeviceEnumerationComplete - Event which signals the end of
// system device enumeration.
//

KEVENT PnpSystemDeviceEnumerationComplete;

//
// Manufacturing information.
//

extern SYSTEM_MANUFACTURING_INFORMATION ExpManufacturingInformation;


#define SYSTEM_RESERVED_GROUP   TEXT("System Reserved")
#define BOOT_BUS_EXTENDER_GROUP TEXT("Boot Bus Extender")

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, IopInitializeBootDrivers)
#pragma alloc_text(INIT, PipInitializeCoreDriversAndElam)
#pragma alloc_text(INIT, PipInitializeCoreDriversByGroup)
#pragma alloc_text(INIT, PipInitializeDriverDependentDLLs)
#pragma alloc_text(INIT, PipInitializeEarlyLaunchDrivers)
#pragma alloc_text(INIT, IopInitializePlugPlayServices)
#pragma alloc_text(INIT, IopInitializeSystemDrivers)
#pragma alloc_text(INIT, PipUnloadEarlyLaunchDrivers)

#pragma alloc_text(INIT, PipAddDevicesToBootDriver)
#pragma alloc_text(INIT, PipAddDevicesToBootDriverWorker)
#pragma alloc_text(INIT, PipCheckDependencies)
#pragma alloc_text(INIT, PipCreateEntry)
#pragma alloc_text(INIT, PipFreeGroupTree)
#pragma alloc_text(INIT, PipGetDriverTagPriority)
#pragma alloc_text(INIT, PipInsertDriverList)
#pragma alloc_text(INIT, PnpLoadBootFilterDriver)
#pragma alloc_text(INIT, PipLookupGroupName)
#pragma alloc_text(INIT, PnpWaitForDevicesToStart)

#pragma alloc_text(INIT, PnpInitPhase0)
#pragma alloc_text(INIT, PnpInitPhase1)
#pragma alloc_text(INIT, PpInitSystem)
#pragma alloc_text(PAGE, PnpBootPhaseComplete)
#pragma alloc_text(INIT, PiInitFirmwareResources)
#pragma alloc_text(INIT, PiInitCacheGroupInformation)
#pragma alloc_text(PAGE, PiInitReleaseCachedGroupInformation)
#pragma alloc_text(INIT, PpInitGetGroupOrderIndex)
#pragma alloc_text(INIT, PnpDoPolicyCheck)
#pragma alloc_text(INIT, PnpInitializeBootStartDriver)
#pragma alloc_text(INIT, PnpMarkHalDeviceNode)
#pragma alloc_text(PAGE, PnpShutdownDevices)
#pragma alloc_text(PAGELK, PnpShutdownSystem)

#pragma alloc_text(INIT, PipInitDeviceOverrideCache)
#pragma alloc_text(PAGE, PipFindDeviceOverrideEntry)
#pragma alloc_text(PAGE, PiQueryRemovableDeviceOverride)
#pragma alloc_text(PAGE, PipCallbackHasDeviceOverrides)

#pragma alloc_text(INIT, PipHardwareConfigInit)
#pragma alloc_text(INIT, PipHardwareConfigExists)
#pragma alloc_text(PAGE, PipHardwareConfigActivateService)
#pragma alloc_text(PAGE, PipHardwareConfigClearStartOverrides)
#pragma alloc_text(PAGE, PipHardwareConfigOpenKey)
#pragma alloc_text(PAGE, PipHardwareConfigGetIndex)
#pragma alloc_text(PAGE, PipHardwareConfigGetLastUseTime)
#pragma alloc_text(PAGE, PipHardwareConfigClearStartOverrideCallback)
#pragma alloc_text(PAGE, PipHardwareConfigTriggerRespecialize)
#pragma alloc_text(PAGE, PnpHardwareConfigCreateBootDriverFlags)

#pragma alloc_text(INIT, PipCheckSystemFirmwareUpdated)
#pragma alloc_text(INIT, PipInitComputerIds)
#pragma alloc_text(INIT, PipCreateComputerId)
#pragma alloc_text(PAGE, PipCheckComputerSupported)
#pragma alloc_text(INIT, PipSmBiosFindStruct)
#pragma alloc_text(INIT, PipSmBiosGetString)

#pragma alloc_text(INIT, PipMigratePnpState)
#pragma alloc_text(INIT, PipMigrateServiceCallback)
#pragma alloc_text(INIT, PipMigrateResetDeviceCallback)
#endif

//
// This flag indicates if the device's InvalidateDeviceRelation is in progress.
// To read or write this flag, callers must get PnpSpinlock.
//

BOOLEAN PnpTearDownPnpStacksOnShutdown;

CORE_DRIVER_GROUP_TYPE PnpCoreDriverGroupLoadPhase;

NTSTATUS
IopInitializePlugPlayServices(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG Phase
    )

/*++

Routine Description:

    This routine initializes kernel mode Plug and Play services.

Arguments:

    LoaderBlock - supplies a pointer to the LoaderBlock passed in from the
        OS Loader.

Returns:

    NTSTATUS code for sucess or reason of failure.

--*/
{
    NTSTATUS status;
    HANDLE hTreeHandle, handle, hCurrentControlSet = NULL;
    UNICODE_STRING unicodeName, previousDate, currentDate;
    PDEVICE_OBJECT deviceObject;
    ULONG disposition, configFlags;
    EVENT_DATA_DESCRIPTOR LoaderData[2];
    OBJECT_ATTRIBUTES objectAttributes;
    PKEY_VALUE_FULL_INFORMATION oldInformation;
    PKEY_VALUE_FULL_INFORMATION information;
    PKEY_VALUE_FULL_INFORMATION asyncInfo;
    PKEY_VALUE_FULL_INFORMATION rebalInfo;
    PKEY_VALUE_FULL_INFORMATION bootOptions;
    PKEY_VALUE_FULL_INFORMATION bestConfigurationTimeout;
    HANDLE systemInfoKey;
    UNICODE_STRING instancePath, containerID;
    BOOLEAN systemFirmwareUpdated = FALSE;

    if (Phase == 0) {

        PnPInitialized = FALSE;

        //
        // Register with CM so we get called when the system hive becomes too
        // large.
        //
        PnpSystemHiveLimits.Low = SYSTEM_HIVE_LOW;
        PnpSystemHiveLimits.High = SYSTEM_HIVE_HIGH;
        CmRegisterSystemHiveLimitCallback(
            SYSTEM_HIVE_LOW,
            SYSTEM_HIVE_HIGH,
            (PVOID)&PnpSystemHiveLimits,
            (PCM_HYSTERESIS_CALLBACK)PpSystemHiveLimitCallback
            );
        PnpSystemHiveTooLarge = FALSE;

        InitializeObjectAttributes(
            &objectAttributes,
            &CmRegistryMachineHardwareDescriptionSystemName,
            OBJ_CASE_INSENSITIVE  | OBJ_KERNEL_HANDLE,
            NULL,
            NULL
            );

        status = ZwCreateKey(
            &systemInfoKey,
            KEY_ALL_ACCESS,
            &objectAttributes,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            &disposition
            );
        if (NT_SUCCESS(status)) {

            //
            // Log an event if system firmware has been updated.
            //
            status = IopGetRegistryValue(
                systemInfoKey,
                L"OldSystemBiosDate",
                0,
                &oldInformation
                );
            if (NT_SUCCESS(status) && oldInformation) {

                status = IopGetRegistryValue(
                    systemInfoKey,
                    L"SystemBiosDate",
                    0,
                    &information
                    );
                if (NT_SUCCESS(status) && information) {

                    RtlInitUnicodeString(&previousDate, (PWCHAR)KEY_VALUE_DATA(oldInformation));
                    RtlInitUnicodeString(&currentDate, (PWCHAR)KEY_VALUE_DATA(information));
                    PnpLogEvent(&previousDate, &currentDate, STATUS_FIRMWARE_UPDATED, NULL, 0);

                    ExFreePool(information);
                }
                ExFreePool(oldInformation);
            }

            ZwClose(systemInfoKey);
        }

        status = PnpDeviceCompletionQueueInitialize(&PnpDeviceCompletionQueue);
        if (!NT_SUCCESS(status)) {

            return status;
        }

        //
        // Initialize firmware resources.
        //
        PiInitFirmwareResources(LoaderBlock);

        //
        // Initialize the blocked driver database.
        //

#if !defined(XBOX_SYSTEMOS)

        if (!LoaderBlock->Extension->IsSmbboot) {

            PpInitializeBootDDB(LoaderBlock, Phase);
        }

#endif

        //
        // Initialize device override cache.
        //
        PipInitDeviceOverrideCache();

        //
        // Initialize the system enum complete event.  This event will be signaled
        // once enumeration is complete.
        //

        KeInitializeEvent(&PnpSystemDeviceEnumerationComplete,
                          NotificationEvent,
                          FALSE);

        //
        // Build up the group order cache list. This is the MultiSz string that
        // tells us what order to start legacy drivers in. Drivers belonging to
        // an earlier group get started first (within the group Tag ordering is
        // used)
        //
        status = PiInitCacheGroupInformation();
        if (!NT_SUCCESS(status)) {

            return status;
        }

        //
        // Initialize the registry access semaphore.
        //

        KeInitializeSemaphore( &PpRegistrySemaphore, 1, 1 );

        status = PnpInitializeLegacyBusInformationTable();
        if (!NT_SUCCESS(status)) {

            return status;
        }

        //
        // Initialize the resource map
        //

        IopInitializeResourceMap (LoaderBlock);
        IopInitReservedResourceList = NULL;
        IopAllocateBootResourcesRoutine = IopReportBootResources;

        //
        // Set the default interface to be used for root enumerated devices
        // with undefined interface type for their resource list or resource
        // requirements list.
        //

        PnpDefaultInterfaceType = Isa;

        //
        // Add the OS-inaccessible range for an Ordering in the registry.
        // The Arbiters will invoke the arbiter library service routine -
        // ArbAddInaccessibleAllocationRange to add the inaccessible list
        // to the RangeList.
        // Ignoring return value as this is not essential for booting an OS.
        //

        ArbInitializeOsInaccessibleRange(MmGetPhysicalAddressBits());

        //
        // Initialize root arbiters
        //

        status = IopPortInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = IopMemInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = IopDmaInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = IopIrqInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = IopBusNumberInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize pnprtl
        //

        status = PiPnpRtlInit(Phase);
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Migrate any pending PnP state, ignoring failures
        //

        PipMigratePnpState();

        //
        // Initialize the PnP data model
        //

        status = PiDmInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                                PNPCTX_BASE_KEY_TYPE_CONTROLSET,
                                                &hCurrentControlSet);

        if (!NT_SUCCESS(status)) {
            hCurrentControlSet = NULL;
            goto init_Exit0;
        }

        //
        // Open HKLM\System\CurrentControlSet\Control\Pnp
        //

        PiWstrToUnicodeString(&unicodeName, REGSTR_PATH_CONTROL_PNP);
        status = IopCreateRegistryKeyEx(&handle,
                                        hCurrentControlSet,
                                        &unicodeName,
                                        KEY_ALL_ACCESS,
                                        REG_OPTION_NON_VOLATILE,
                                        NULL);

        if (NT_SUCCESS(status)) {

            status = IopGetRegistryValue(handle,
                                         REGSTR_VALUE_ASYNC,
                                         0,
                                         &asyncInfo);
            if (NT_SUCCESS(status)) {

                if (asyncInfo->Type == REG_DWORD && asyncInfo->DataLength == sizeof(ULONG)) {

                    //
                    // KEY_VALUE_DATA returns a PUCHAR.
                    //
                    PnpAsyncOptions = *(PULONG)(KEY_VALUE_DATA(asyncInfo));
                }

                ExFreePool(asyncInfo);
            }

            status = IopGetRegistryValue(handle,
                                         REGSTR_VALUE_DISABLEREBALANCE,
                                         0,
                                         &rebalInfo);
            if (NT_SUCCESS(status)) {

                if (rebalInfo->Type == REG_DWORD && rebalInfo->DataLength == sizeof(ULONG)) {

                    //
                    // KEY_VALUE_DATA returns a PUCHAR.
                    //
                    PnpDisableRebalance = *(PULONG)(KEY_VALUE_DATA(rebalInfo));
                }

                ExFreePool(rebalInfo);
            }

            status = IopGetRegistryValue(handle,
                                         REGSTR_VALUE_PNP_BOOT_OPTIONS,
                                         0,
                                         &bootOptions);

            if (NT_SUCCESS(status)) {
                if (bootOptions->Type == REG_DWORD &&
                    bootOptions->DataLength == sizeof(ULONG)) {

                    PnpBootOptions = *(PULONG)(KEY_VALUE_DATA(bootOptions));
                }

                ExFreePool(bootOptions);
            }

            status = IopGetRegistryValue(
                         handle,
                         REGSTR_VALUE_FIND_BEST_CONFIGURATION_TIMEOUT,
                         0,
                         &bestConfigurationTimeout);

            if (NT_SUCCESS(status)) {
                if (bestConfigurationTimeout->Type == REG_DWORD &&
                    bestConfigurationTimeout->DataLength == sizeof(ULONG)) {

                    //
                    // KEY_VALUE_DATA returns a PUCHAR.
                    //
                    PnpFindBestConfigurationTimeout =
                        *(PULONG)(KEY_VALUE_DATA(bestConfigurationTimeout));
                }

                ExFreePool(bestConfigurationTimeout);
            }

            //
            // Query device reset retry interval and reattempt values from
            // registry if specified.
            //

            IopQueryDeviceResetRegistrySettings(handle);

            //
            // Close the handle to the PnP control key now that we are done.
            //
            ZwClose(handle);
        }

        //
        // Check for SystemSetupInProgress value under HKLM\System\Setup.
        // Check for additional setup\upgrade markers and update PnpSetupInProgressEx
        //

        PiWstrToUnicodeString(&unicodeName, CM_REGISTRY_MACHINE(REGSTR_PATH_SYSTEM_SETUP));

        status = IopOpenRegistryKeyEx(&handle,
                                      NULL,
                                      &unicodeName,
                                      KEY_READ);

        if (NT_SUCCESS(status)) {
            // SystemSetupInProgress
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_SYSTEM_SETUP_IN_PROGRESS,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupInProgress = TRUE;
                }

                ExFreePool(information);
            }
            //SetupPhase
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_SETUP_PHASE,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupPhaseInProgress = TRUE;
                }

                ExFreePool(information);
            }
            //SetupType
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_SETUP_TYPE,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupTypeInProgress = TRUE;
                }

                ExFreePool(information);
            }
            //OOBEInProgress
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_OOBE_IN_PROGRESS,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupOOBEInProgress = TRUE;
                }

                ExFreePool(information);
            }
            //Upgrade
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_UPGRADE,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupUpgradeInProgress = TRUE;
                }

                ExFreePool(information);
            }
            //RollbackActive
            status = IopGetRegistryValue(handle,
                                         REGSTR_VAL_ROLLBACK_ACTIVE,
                                         0,
                                         &information);

            if (NT_SUCCESS(status)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG)) &&
                    (*(PULONG)(KEY_VALUE_DATA(information)) != 0)) {
                    PnpSetupRollbackActiveInProgress = TRUE;
                }

                ExFreePool(information);
            }

            ZwClose(handle);
        }

        //
        // Initialize/update this hardware configuration
        //

        status = PipHardwareConfigInit(&(LoaderBlock->Extension->HardwareConfigurationId));

        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Check if the system firmware has been updated since the last boot.
        //

        PipCheckSystemFirmwareUpdated(&systemFirmwareUpdated);

        //
        // Initialize Device Container support phase 0.
        //

        status = PiDcInit(Phase);
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize the device api security descriptors
        //

        status =  PiAuCreateSecurityObjects();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize DevQuery
        //

        status = PiDqInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize Device Configuration
        //

        status = PpDevCfgInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Create the registry entry for the root of the hardware tree (HTREE\ROOT\0).
        //

        status = _CmCreateDevice(PiPnpRtlCtx,
                                 REGSTR_VAL_ROOT_DEVNODE,
                                 KEY_ALL_ACCESS,
                                 &hTreeHandle,
                                 NULL,
                                 0);

        if (NT_SUCCESS(status)) {

            //
            // Initialize base container ID, ConfigFlags.
            //

            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                REGSTR_VAL_ROOT_DEVNODE,
                                hTreeHandle,
                                CM_REG_PROP_BASE_CONTAINERID,
                                REG_SZ,
                                (PUCHAR)REGSTR_VALUE_ROOT_CONTAINERID,
                                sizeof(REGSTR_VALUE_ROOT_CONTAINERID),
                                0);

            configFlags = 0;
            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                REGSTR_VAL_ROOT_DEVNODE,
                                hTreeHandle,
                                CM_REG_PROP_CONFIGFLAGS,
                                REG_DWORD,
                                (PUCHAR)&configFlags,
                                sizeof(configFlags),
                                0);

            ZwClose(hTreeHandle);
        }

        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Before creating device node tree, we need to initialize the device
        // tree lock.
        //

        InitializeListHead(&IopPendingEjects);
        InitializeListHead(&IopPendingSurpriseRemovals);
        ExInitializeResourceLite(&IopDeviceTreeLock);
        ExInitializeResourceLite(&IopSurpriseRemoveListLock);
        ExInitializeResourceLite(&PnpDevicePropertyLock);
        PiInitializeEngineLock();
        KeInitializeSpinLock(&PnpSpinLock);
        KeInitializeGuardedMutex(&PiResourceListLock);

        status = PiDeviceDependencyInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        status = PnpInitializeDeviceActions();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize the hardware profile/docking support.
        //
        PpProfileInit();

        //
        // Initialize warm docking variables.
        //
        IopWarmEjectPdo = NULL;
        KeInitializeEvent(&IopWarmEjectLock, SynchronizationEvent, TRUE );

        //
        // Create a PnP manager's driver object to own all the detected PDOs.
        //

        PiWstrToUnicodeString(&unicodeName, PNPMGR_STR_PNP_DRIVER);
        status = IoCreateDriver (&unicodeName, PipPnPDriverEntry);
        if (NT_SUCCESS(status)) {

            //
            // Create empty device node tree, i.e., only contains only root device node
            //     (No need to initialize Parent, Child and Sibling links.)
            //

            status = IoCreateDevice(PnpDriverObject,
                                    0,
                                    NULL,
                                    FILE_DEVICE_CONTROLLER,
                                    0,
                                    FALSE,
                                    &deviceObject);

            if (NT_SUCCESS(status)) {
                deviceObject->Flags |= DO_BUS_ENUMERATED_DEVICE;
                status = PipAllocateDeviceNode(
                    deviceObject,
                    &IopRootDeviceNode);

                if (!IopRootDeviceNode) {
                    IoDeleteDevice(deviceObject);
                    IoDeleteDriver(PnpDriverObject);
                } else {
                    PipSetDevNodeFlags(IopRootDeviceNode, DNF_MADEUP | DNF_ENUMERATED | DNF_IDS_QUERIED | DNF_NO_RESOURCE_REQUIRED);
                    PipSetDevNodeUserFlags(IopRootDeviceNode, DNUF_NOT_DISABLEABLE | DNUF_DONT_SHOW_IN_UI);

                    IopRootDeviceNode->NumaNodeIndex = PNP_NUMA_NODE_NONE;
                    status = PnpAllocateDeviceInstancePath(IopRootDeviceNode,
                                                           sizeof(REGSTR_VAL_ROOT_DEVNODE));

                    if (!NT_SUCCESS(status)) {
                        PNP_ASSERT(PnpIsDeviceInstancePathEmpty(IopRootDeviceNode) != FALSE);
                        goto init_Exit0;
                    }

                    PiWstrToUnicodeString(&instancePath, REGSTR_VAL_ROOT_DEVNODE);
                    PnpCopyDeviceInstancePath(IopRootDeviceNode, &instancePath);
                    status = PnpMapDeviceObjectToDeviceInstance(
                        IopRootDeviceNode->PhysicalDeviceObject,
                        PnpGetDeviceInstancePath(IopRootDeviceNode));

                    if (!NT_SUCCESS(status)) {
                        goto init_Exit0;
                    }

                    PiWstrToUnicodeString(&containerID, REGSTR_VALUE_ROOT_CONTAINERID);
                    RtlGUIDFromString(&containerID, &IopRootDeviceNode->ContainerID);

                    PnpQueryAndSaveDeviceNodeCapabilities(IopRootDeviceNode);
                    PipSetDevNodeState(IopRootDeviceNode, DeviceNodeStarted, NULL);
                }
            }
        }

        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Add root device to computer container.
        //

        status = _CmAddDeviceToContainer(PiPnpRtlCtx,
                                         REGSTR_VALUE_ROOT_CONTAINERID,
                                         REGSTR_VALUE_ROOT_CONTAINERID,
                                         REGSTR_VAL_ROOT_DEVNODE,
                                         NULL);

        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Now that the root device has been added to the root container,
        // notify the container subsystem if system firmware has been updated.
        //

        if (systemFirmwareUpdated) {
            PiDcHandleSystemFirmwareUpdate();
        }

        //
        // Initialize the kernel mode pnp notification system
        //

        status = PnpInitializeDeviceEvents();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        PnpInitializeNotification();

        //
        // Initialize table for holding bus type guid list.
        //

        status = PnpBusTypeGuidInitialize();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        KeInitializeEvent(&PnpReplaceEvent, SynchronizationEvent, TRUE);

        //
        // Initialize Software Device
        //

        status = PiSwInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize the user-mode CM API subsystem
        //

        status = PiCMInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize the user-mode event notification subsystem
        //

        status = PiUEventInit(Phase);
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Initialize the DeviceApi device
        //

        status = PiDaInit();
        if (!NT_SUCCESS(status)) {
            goto init_Exit0;
        }

        //
        // Commit any pending drivers that have their driver dependencies
        // satisfied. Note that this needs to be done before the re-enumeration
        // of the root device is triggered. A re-enumeration of the root device
        // will expose root enumerated devices.  Some of those devices may be
        // pushed through device configuration so the pending services need to
        // be copied out before that happens or device configuration may fail.
        //

        PipProcessPendingOperations();

        //
        // Enumerate the ROOT bus synchronously.
        //

        PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                               ReenumerateRootDevices,
                               FALSE,
                               0,
                               NULL,
                               NULL);

init_Exit0:

        //
        // DO NOT close hCurrentControlSet.  It is a cached handle from pnprtl
        // and MUST NOT be closed
        //
        NOTHING;

    } else if (Phase == 1) {

        //
        // Initialize diagnostic support.
        //

        status = PnpDiagInitialize();
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Initialize tracelogging support.
        //

        PnpTraceInitialize();

        //
        // Initialize Device Container support phase 1.
        //

        status = PiDcInit(Phase);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Initialize the user-mode event notification subsystem
        //

        status = PiUEventInit(Phase);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Report OS Loader runtime.
        //

        EventDataDescCreate(&LoaderData[0],
                            &LoaderBlock->Extension->LoaderPerformanceData->StartTime,
                            sizeof(ULONGLONG));

        EventDataDescCreate(&LoaderData[1],
                            &LoaderBlock->Extension->LoaderPerformanceData->EndTime,
                            sizeof(ULONGLONG));

        PnpDiagnosticTrace(&KMPnPEvt_OsLoader_Time, 2, LoaderData);

        TraceLoggingWrite(BootProvider,
                          "OsloaderTime",
                          TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                          TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                          TraceLoggingUInt64(LoaderBlock->Extension->LoaderPerformanceData->StartTime, "StartTime"),
                          TraceLoggingUInt64(LoaderBlock->Extension->LoaderPerformanceData->EndTime, "EndTime"),
                          TraceLoggingUInt64(LoaderBlock->Extension->ProcessorCounterFrequency, "Frequency"));

        TraceLoggingUnregister(BootProvider);

        //
        // Initialize pnprtl driver database extensions.
        //

        status = PiPnpRtlInit(Phase);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Enumerate the ROOT bus synchronously.
        //

        PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                               ReenumerateRootDevices,
                               FALSE,
                               0,
                               NULL,
                               NULL);

        KeInitializeEvent(&PnpShutdownEvent, NotificationEvent, FALSE);

        //
        // Initialize driver blocking diagnostic support
        //

#if !defined(XBOX_SYSTEMOS)

        if (!LoaderBlock->Extension->IsSmbboot) {

            PpInitializeBootDDB(LoaderBlock, Phase);
        }

#endif

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_INVALID_PARAMETER_2;
    }

    return status;
}

LOGICAL
PipCheckDependencies(
    __in HANDLE KeyHandle
    )

/*++

Routine Description:

    This routine gets the "DependOnGroup" field for the specified key node
    and determines whether any driver in the group(s) that this entry is
    dependent on has successfully loaded.

Arguments:

    KeyHandle - Supplies a handle to the key representing the driver in
        question.

Return Value:

    The function value is TRUE if the driver should be loaded, otherwise
    FALSE

--*/

{
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    UNICODE_STRING groupName;
    LOGICAL load;
    ULONG length;
    PWSTR source;
    PTREE_ENTRY treeEntry;

    //
    // Attempt to obtain the "DependOnGroup" key for the specified driver
    // entry.  If one does not exist, then simply mark this driver as being
    // one to attempt to load.  If it does exist, then check to see whether
    // or not any driver in the groups that it is dependent on has loaded
    // and allow it to load.
    //

    if (!NT_SUCCESS( IopGetRegistryValue( KeyHandle, L"DependOnGroup", 0, &keyValueInformation ))) {
        return TRUE;
    }

    length = keyValueInformation->DataLength;

    source = (PWSTR) ((PUCHAR) keyValueInformation + keyValueInformation->DataOffset);
    load = TRUE;

    while (length) {
        RtlInitUnicodeString( &groupName, source );
        groupName.Length = groupName.MaximumLength;
        treeEntry = PipLookupGroupName( &groupName, FALSE );
        if (treeEntry) {
            if (!treeEntry->DriversLoaded) {
                load = FALSE;
                break;
            }
        }
        length -= groupName.MaximumLength;
        source = (PWSTR) ((PUCHAR) source + groupName.MaximumLength);
    }

    ExFreePool( keyValueInformation );
    return load;
}

PTREE_ENTRY
PipCreateEntry(
    __in PUNICODE_STRING GroupName
    )

/*++

Routine Description:

    This routine creates an entry for the specified group name suitable for
    being inserted into the group name tree.

Arguments:

    GroupName - Specifies the name of the group for the entry.

Return Value:

    The function value is a pointer to the created entry.


--*/

{
    PTREE_ENTRY treeEntry;

    //
    // Allocate and initialize an entry suitable for placing into the group
    // name tree.
    //

    treeEntry = ExAllocatePool( PagedPool,
                                sizeof( TREE_ENTRY ) + GroupName->Length );

    //
    // We return NULL here and what this really implies that
    // we won't be able to determine if drivers for this group
    // was loaded.
    //
    if (!treeEntry) {
        return NULL;
    }

    RtlZeroMemory( treeEntry, sizeof( TREE_ENTRY ) );
    treeEntry->GroupName.Length = GroupName->Length;
    treeEntry->GroupName.MaximumLength = GroupName->Length;
    treeEntry->GroupName.Buffer = (PWCHAR) (treeEntry + 1);
    RtlCopyMemory( treeEntry->GroupName.Buffer,
                   GroupName->Buffer,
                   GroupName->Length );

    return treeEntry;
}

VOID
PipFreeGroupTree(
    __in PTREE_ENTRY TreeEntry
    )

/*++

Routine Description:

    This routine is invoked to free a node from the group dependency tree.
    It is invoked the first time with the root of the tree, and thereafter
    recursively to walk the tree and remove the nodes.

Arguments:

    TreeEntry - Supplies a pointer to the node to be freed.

Return Value:

    None.

--*/

{
    //
    // Simply walk the tree in ascending order from the bottom up and free
    // each node along the way.
    //

    if (TreeEntry->Left) {
        PipFreeGroupTree( TreeEntry->Left );
    }

    if (TreeEntry->Sibling) {
        PipFreeGroupTree( TreeEntry->Sibling );
    }

    if (TreeEntry->Right) {
        PipFreeGroupTree( TreeEntry->Right );
    }

    //
    // All of the children and siblings for this node have been freed, so
    // now free this node as well.
    //

    ExFreePool( TreeEntry );
}

VOID
PipInitializeCoreDriversAndElam (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine is invoked to initialize the ELAM drivers and their
    dependencies, (3rd party) Core Extensions, and other core drivers
    necessary to enumerate and initialize TPM.

    The drivers are broken into the following groups:

    1. Core Drivers: This group consists of drivers that ELAM drivers and
           3rd party Core Extensions depend upon (e.g. WDF, CNG.sys). All
           drivers in this group should be MS-supplied and thus MS-signed.

    2. ELAM drivers. This group consists of 3rd party ELAM drivers. These
           drivers need to be signed with ELAM certificate.

    4. Core Extensions: This group consists of 3rd party drivers
           (viz. Platform Extensions and Tree drivers) that TPM Core drivers
           depend upon. These drivers need to be signed with Core Extension
           certificate.

    5. TPM Core: This group consists of TPM driver and bus drivers (e.g.
           ACPI, PCI) that are necessary to enumerate TPM. All drivers in this
           group should be MS-supplied and thus MS-signed.

    The initialization order including dependent DLLs is as follows:

    [Dependent DLLs for Core drivers and TPM Core drivers]
                        |
                        v
                [ELAM drivers]
                        |
                        v
        [Dependent DLLs for Core Extensions]
                        |
                        v
                [Core Extensions]
                        |
                        v
                    [TPM Core]

    Note among such early start drivers, the load of only Core Extensions and
    their dependent DLLs is notified to ELAM drivers.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block, created by
        the OS Loader.

Return Value:

    None.

--*/

{

    //
    // Call DllInitialize for Core driver dependent DLLs. Note this includes
    // dependent DLLs for Core and TpmCore.
    //

    PipInitializeDriverDependentDLLs(CoreDriverDependentDLL, LoaderBlock);

    //
    // Initialize Core drivers (before early launch drivers) and notify PS when
    // complete.
    //

    PipInitializeCoreDriversByGroup(CoreDriverGroup, LoaderBlock);
    PsNotifyCoreDriversInitialized();

    //
    // Initialize all Early Launch drivers.
    //

    PipInitializeEarlyLaunchDrivers(LoaderBlock);

    //
    // Only allow anti-malware drivers to register for boot driver callbacks.
    //

    PnpCloseBootDriverCallbackRegistration();

    //
    // Callback Early Launch drivers with initialization status update.
    //

    PnpNotifyEarlyLaunchStatusUpdate(BdCbStatusPrepareForDependencyLoad);

    //
    // Call DllInitialize for Core Extension dependent DLLs.
    //

    PipInitializeDriverDependentDLLs(CoreExtensionDependentDLL, LoaderBlock);

    //
    // Initialize all the Core Extensions (3rd party drivers).
    //

    PipInitializeCoreDriversByGroup(CoreExtensionsDriverGroup, LoaderBlock);

    //
    // Initialize TPM Core driver group.
    //

    PipInitializeCoreDriversByGroup(TpmCoreDriverGroup, LoaderBlock);

    //
    // Mark the fact that TpmCore driver group processing has finished.
    //

    PnpCoreDriverGroupLoadPhase = CoreDriverGroupTypeMaximum;
    return;
}

VOID
PipInitializeCoreDriversByGroup (
    __in CORE_DRIVER_GROUP_TYPE GroupType,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine is invoked to initialize the supplied group of core drivers.
    Such drivers are loaded by the OS loader and the list is supplied as
    part of the loader parameter block.

Arguments:

    GroupType - Supplies the group of core drivers to be initialized.

    LoaderBlock - Supplies a pointer to the loader parameter block, created by
        the OS Loader.

Return Value:

    None.

--*/

{

    PBOOT_DRIVER_LIST_ENTRY BootDriver;
    UNICODE_STRING CompleteName;
    PBLDR_DATA_TABLE_ENTRY DriverEntry;
    PLIST_ENTRY DriverList;
    PDRIVER_OBJECT DriverObject;
    LOGICAL InvokeNotifications;
    HANDLE KeyHandle;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    InvokeNotifications = FALSE;
    switch (GroupType) {

    //
    // Core Drivers are initialized before ELAMs and thus they are not notified
    // to ELAM drivers.
    //

    case CoreDriverGroup:
        DriverList = &LoaderBlock->CoreDriverListHead;
        break;

    //
    // Core Extensions need to be notified to ELAM.
    //

    case CoreExtensionsDriverGroup:
        DriverList = &LoaderBlock->CoreExtensionsDriverListHead;
        InvokeNotifications = TRUE;
        break;

    //
    // TPM Core group of drivers are not notified to ELAMs even though they
    // are initialized after ELAMs. This is to maintain consistency with
    // previous OS releases where ELAMs weren't notified for drivers loading in
    // the TPM Core group (as they were initialized before ELAMs).
    //

    case TpmCoreDriverGroup:
        DriverList = &LoaderBlock->TpmCoreDriverListHead;
        break;

    default:

        NT_ASSERT(FALSE);

        DriverList = NULL;
    }

    if (DriverList == NULL) {
        goto InitializeCoreDriversByGroupEnd;
    }

    PnpCoreDriverGroupLoadPhase = GroupType;

    //
    // Initialize the specified driver group.
    //

    NextEntry = DriverList->Flink;
    while (NextEntry != DriverList) {
        BootDriver = CONTAINING_RECORD(NextEntry, BOOT_DRIVER_LIST_ENTRY, Link);
        NextEntry = NextEntry->Flink;

        //
        // Skip past any drivers that failed load in the boot loader.
        //

        if (!NT_SUCCESS(BootDriver->LoadStatus)) {
            continue;
        }

        CompleteName.Buffer = NULL;
        KeyHandle = NULL;
        Status = IopOpenRegistryKeyEx(&KeyHandle,
                                      NULL,
                                      &BootDriver->RegistryPath,
                                      KEY_READ);

        if (!NT_SUCCESS(Status)) {
            goto InitializeNextCoreDriver;
        }

        Status = IopGetDriverNameFromKeyNode(KeyHandle, &CompleteName);
        if (!NT_SUCCESS(Status)) {
            goto InitializeNextCoreDriver;
        }

        DriverEntry = BootDriver->LdrEntry;
        Status = PnpInitializeBootStartDriver(
                     &CompleteName,
                     &BootDriver->RegistryPath,
                     (PDRIVER_INITIALIZE)(ULONG_PTR)
                     DriverEntry->KldrEntry.EntryPoint,
                     DriverEntry,
                     &LoaderBlock->LoadOrderListHead,
                     FALSE,
                     InvokeNotifications,
                     TRUE,
                     &DriverObject);

        if (!NT_SUCCESS(Status) || (DriverObject == NULL)) {
            goto InitializeNextCoreDriver;
        }

        //
        // Certain drivers require that AddDevice be called on all their
        // devices first before any device receives a start irp (i.e.
        // pci root ports and bus number assignment).  With
        // async enum this is not guaranteed.  Also, an AddDevice call
        // may cause a filter to trigger a reenumeration which needs
        // to be blocked here as well (ACPI may do this if acpisim loads
        // a table in response to the AddDevice).
        //
        // To do so, lock the action queue to drain all requests and to
        // prevent new requests from being dispatched.  Call AddDevice
        // synchronously for each device.  Finally unlock the action
        // queue and drain any delayed actions before moving on.
        //

        PnpLockDeviceActionQueue();
        PipAddDevicesToBootDriver(DriverObject);
        PnpUnlockDeviceActionQueue();
        PnpWaitForEmptyDeviceActionQueue();

        //
        // Scan the hardware tree looking for devices which need
        // resources or starting.
        //

        PnpRequestDeviceAction(NULL,
                               ReenumerateBootDevices,
                               FALSE,
                               0,
                               NULL,
                               NULL);

        //
        // Before processing next boot driver, wait for
        // IoRequestDeviceRemoval complete. The driver to be processed may
        // need the resources being released by IoRequestDeviceRemoval.
        // (For drivers report detected BOOT device if they fail to get the
        // resources in their DriverEntry.  They will fail and we will
        // bugcheck with inaccessible boot device.)
        //

        if (PnpWaitForBootDevicesDeleted() == FALSE) {
            HeadlessKernelAddLogEntry(
                HEADLESS_LOG_WAIT_BOOT_DEVICES_DELETE_FAILED,
                NULL);

            goto InitializeNextCoreDriver;
        }

InitializeNextCoreDriver:
        if (KeyHandle != NULL) {
            ZwClose(KeyHandle);
        }

        if (CompleteName.Buffer != NULL) {
            ExFreePool(CompleteName.Buffer);
        }

        if (!NT_SUCCESS(Status)) {
            BootDriver->LdrEntry->KldrEntry.Flags |= LDRP_FAILED_BUILTIN_LOAD;
        }
    }

InitializeCoreDriversByGroupEnd:
    return;
}

LOGICAL
IopInitializeBootDrivers(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PDRIVER_OBJECT *PreviousDriver
    )

/*++

Routine Description:

    This routine is invoked to initialize the boot drivers that were loaded
    by the OS Loader.  The list of drivers is provided as part of the loader
    parameter block.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block, created
        by the OS Loader.

    Previous Driver - Supplies a variable to receive the address of the
        driver object chain created by initializing the drivers.

Return Value:

    The function value is a LOGICAL indicating whether or not the boot
    drivers were successfully initialized.

--*/

{

    USHORT i, j;
    USHORT bootBusExtenderGroupIndex;
    UNICODE_STRING bootBusExtenderGroupName;
    PBOOT_DRIVER_LIST_ENTRY bootDriver;
    LOGICAL bootReinitDriversFound;
    UNICODE_STRING completeName;
    PBLDR_DATA_TABLE_ENTRY driverEntry;
    PDRIVER_INFORMATION driverInfo;
    PDRIVER_OBJECT driverObject;
    USHORT groupIndicesFound;
    UNICODE_STRING groupName;
    ULONG hardwareConfigIndex;
    WCHAR hardwareConfigIndexString[MAX_HARDWARE_CONFIG_NUMBER_LENGTH];
    HANDLE keyHandle;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    PLIST_ENTRY nextEntry;
    UNICODE_STRING PolicyEntry;
    UNICODE_STRING rawFsName;
    HANDLE startOverrideKey;
    DWORD startType;
    NTSTATUS status;
    USHORT systemReservedGroupIndex;
    UNICODE_STRING systemReservedGroupName;
    NTSTATUS tempStatus;
    PTREE_ENTRY treeEntry;

    UNREFERENCED_PARAMETER(PreviousDriver);
    driverObject = NULL;

    PnpDiagnosticTrace(&KMPnPEvt_BootStart_Start, 0, NULL);

    //
    // Retrieve policy from registry.
    //

    PnpDriverImageLoadPolicy = PNP_INITIALIZE_DRIVERS_DEFAULT;
    PiWstrToUnicodeString(&PolicyEntry, PNP_DRIVER_INITIALIZATION_POLICY_PATH);
    status = IopOpenRegistryKeyEx(&keyHandle, NULL, &PolicyEntry, KEY_READ);

    //
    // Fallback on Control policy path, if the Policies path is unavailable.
    //

    if (!NT_SUCCESS(status)) {
        PiWstrToUnicodeString(&PolicyEntry,
                              PNP_DRIVER_INITIALIZATION_POLICY_PATH_FALLBACK);

        status = IopOpenRegistryKeyEx(&keyHandle, NULL, &PolicyEntry, KEY_READ);
    }

    if (NT_SUCCESS(status)) {
        status = IopGetRegistryValue(keyHandle,
                                     L"DriverLoadPolicy",
                                     0,
                                     &keyValueInformation);

        ZwClose(keyHandle);
        if (NT_SUCCESS(status)) {
            if ((keyValueInformation->Type == REG_DWORD) &&
                (keyValueInformation->DataLength >= sizeof(DWORD))) {

                PnpDriverImageLoadPolicy =
                    *(PDWORD)KEY_VALUE_DATA(keyValueInformation);
            }

            ExFreePool(keyValueInformation);
        }
    }

    //
    // Verify value
    //

    if (PnpDriverImageLoadPolicy == PNP_INITIALIZE_GOOD_GP_WORKAROUND) {
        PnpDriverImageLoadPolicy = 0;

    } else if ((PnpDriverImageLoadPolicy & ~(PNP_INITIALIZE_VALID_MASK)) != 0) {
        PnpDriverImageLoadPolicy = PNP_INITIALIZE_DRIVERS_DEFAULT;
    }

    //
    // Initialize Core drivers, Early launch and Core Extensions.
    //

    PipInitializeCoreDriversAndElam(LoaderBlock);

    //
    // Initialize computer IDs after the CNG Core driver has been initialized
    // since RtlGenerateClass5Guid depends on BCrypt.
    //

    PipInitComputerIds(LoaderBlock);

    //
    // Initialize the built-in RAW file system driver.
    //

    PiWstrToUnicodeString( &rawFsName, L"\\FileSystem\\RAW" );
    PiWstrToUnicodeString( &completeName, L"" );
    PnpInitializeBootStartDriver(&rawFsName,
                                 &completeName,
                                 RawInitialize,
                                 NULL,
                                 &LoaderBlock->LoadOrderListHead,
                                 FALSE,
                                 FALSE,
                                 TRUE,
                                 &driverObject);
    if (!driverObject) {
#if DBG
        IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                     "IOINIT: Failed to initialize RAW filsystem \n"));

#endif

        return FALSE;
    }

    //
    // Determine number of group orders and build a list_entry array to link
    // all the drivers together based on their groups. By asking for the group
    // index with a NULL service handle, the number of group entries + 1 will
    // be returned.  This allows the table to contain an entry for every valid
    // group plus an entry for drivers who have no group or have a group that
    // is not in the cached table.
    //

    IopGroupIndex = PpInitGetGroupOrderIndex(NULL);
    if (IopGroupIndex == NO_MORE_GROUP) {
        HeadlessKernelAddLogEntry(HEADLESS_LOG_FIND_GROUPS_FAILED, NULL);
        return FALSE;
    }

    IopGroupTable = ExAllocatePool(PagedPool,
                                   IopGroupIndex * sizeof(LIST_ENTRY));

    if (IopGroupTable == NULL) {
        HeadlessKernelAddLogEntry(HEADLESS_LOG_OUT_OF_MEMORY, NULL);
        return FALSE;
    }
    for (i = 0; i < IopGroupIndex; i++) {
        InitializeListHead(&IopGroupTable[i]);
    }

    //
    // Call DllInitialize for driver dependent DLLs.
    //

    PipInitializeDriverDependentDLLs(NormalDriverDependentDLL, LoaderBlock);

    //
    // Initialize the group index variables to something invalid.
    //

    systemReservedGroupIndex = MAXUSHORT;
    bootBusExtenderGroupIndex = MAXUSHORT;
    RtlInitUnicodeString(&systemReservedGroupName, SYSTEM_RESERVED_GROUP);
    RtlInitUnicodeString(&bootBusExtenderGroupName, BOOT_BUS_EXTENDER_GROUP);
    groupIndicesFound = 0;

    //
    // Loop through the group names extracting the index for the "System
    // Reserved" group and the "Boot Bus Extender" group.  Once they have both
    // been found, the loop can be terminated.
    //

    for (i = 0; i < PiInitGroupOrderTableCount; i++) {
        if (RtlCompareUnicodeString(&PiInitGroupOrderTable[i],
                                    &systemReservedGroupName,
                                    TRUE)==0) {

            systemReservedGroupIndex = i;
            groupIndicesFound += 1;
        } else if (RtlCompareUnicodeString(&PiInitGroupOrderTable[i],
                                           &bootBusExtenderGroupName,
                                           TRUE)==0) {

            bootBusExtenderGroupIndex = i;
            groupIndicesFound += 1;
        }

        if (groupIndicesFound >= 2) {
            break;
        }
    }

    PNP_ASSERT(groupIndicesFound == 2);

    //
    // Allocate pool to store driver's start information. All the driver info
    // records with the same group value will be linked into a list.
    //

    nextEntry = LoaderBlock->BootDriverListHead.Flink;
    while (nextEntry != &LoaderBlock->BootDriverListHead) {
        bootDriver = CONTAINING_RECORD(nextEntry, BOOT_DRIVER_LIST_ENTRY, Link);
        nextEntry = nextEntry->Flink;

        //
        // Skip this entry if the load status indicates that the driver was
        // not successfully loaded into memory.
        //

        if (!NT_SUCCESS(bootDriver->LoadStatus)) {
            continue;
        }

        driverInfo = (PDRIVER_INFORMATION) ExAllocatePool(
                        PagedPool, sizeof(DRIVER_INFORMATION));
        if (driverInfo) {
            RtlZeroMemory(driverInfo, sizeof(DRIVER_INFORMATION));
            InitializeListHead(&driverInfo->Link);
            driverInfo->DataTableEntry = bootDriver;

            //
            // Open the driver's registry key to find out if this is a
            // filesystem or a driver.
            //

            status = IopOpenRegistryKeyEx( &keyHandle,
                                           (HANDLE)NULL,
                                           &bootDriver->RegistryPath,
                                           KEY_READ
                                           );
            if (!NT_SUCCESS( status )) {
                ExFreePool(driverInfo);
            } else {
                driverInfo->ServiceHandle = keyHandle;
                j = PpInitGetGroupOrderIndex(keyHandle);
                driverInfo->TagPosition = PipGetDriverTagPriority(keyHandle);
                PNP_ASSERT(j < IopGroupIndex);
                PipInsertDriverList(&IopGroupTable[j], driverInfo);
            }
        }
    }

    //
    // Callback Early Launch drivers with initialization status update.
    //

    PnpNotifyEarlyLaunchStatusUpdate(BdCbStatusPrepareForDriverLoad);

    //
    // Process each driver base on its group.  The group with lower index
    // number (higher priority) is processed first.
    //

    for (i = 0; i < IopGroupIndex; i++) {
        nextEntry = IopGroupTable[i].Flink;
        while (nextEntry != &IopGroupTable[i]) {

            driverInfo = CONTAINING_RECORD(nextEntry, DRIVER_INFORMATION, Link);
            keyHandle = driverInfo->ServiceHandle;
            bootDriver = driverInfo->DataTableEntry;
            driverEntry = bootDriver->LdrEntry;
            driverInfo->Processed = TRUE;

            //
            // call the driver's driver entry
            //
            // See if this driver has an ObjectName value.  If so, this value
            // overrides the default ("\Driver" or "\FileSystem").
            //

            status = IopGetDriverNameFromKeyNode( keyHandle,
                                                  &completeName );
            if (!NT_SUCCESS( status )) {

#if DBG
                IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                             "IOINIT: Could not get driver name for %wZ\n",
                             &bootDriver->RegistryPath));
#endif // DBG

                driverInfo->Failed = TRUE;
            } else {

                status = IopGetRegistryValue( keyHandle,
                                              REGSTR_VALUE_GROUP,
                                              0,
                                              &keyValueInformation );
                if (NT_SUCCESS( status )) {

                    if (keyValueInformation->DataLength) {
                        groupName.Length =
                            (USHORT)keyValueInformation->DataLength;

                        groupName.MaximumLength = groupName.Length;
                        groupName.Buffer =
                            (PWSTR)((PUCHAR)keyValueInformation +
                                    keyValueInformation->DataOffset);

                        treeEntry = PipLookupGroupName( &groupName, TRUE );
                    } else {
                        treeEntry = (PTREE_ENTRY) NULL;
                    }
                    ExFreePool( keyValueInformation );
                } else {
                    treeEntry = (PTREE_ENTRY) NULL;
                }

                driverObject = NULL;
                if (PipCheckDependencies(keyHandle)) {

                    //
                    // The driver may already be initialized by
                    // PnpLoadBootFilterDriver if it is boot filter driver.
                    // If not, initialize it.
                    //

                    driverObject = driverInfo->DriverObject;
                    if (driverObject == NULL && !driverInfo->Failed) {

                        driverInfo->Status =
                            PnpInitializeBootStartDriver(
                                &completeName,
                                &bootDriver->RegistryPath,
                                (PDRIVER_INITIALIZE)(ULONG_PTR)
                                driverEntry->KldrEntry.EntryPoint,
                                driverEntry,
                                &LoaderBlock->LoadOrderListHead,
                                FALSE,
                                TRUE,
                                TRUE,
                                &driverObject);

                        if (driverObject) {

                            //
                            // Pnp might unload the driver before we get a
                            // chance to look at this. So take an extra
                            // reference.
                            //

                            ObReferenceObject(driverObject);
                        }
                    }
                }
                if (driverObject) {
                    if (treeEntry) {
                        treeEntry->DriversLoaded++;
                    }
                    driverInfo->DriverObject = driverObject;

                } else {
                    driverInfo->Failed = TRUE;
                }
                ExFreePool( completeName.Buffer );
            }
            if (!driverInfo->Failed) {

                //
                // Certain drivers require that AddDevice be called on all their
                // devices first before any device receives a start irp (i.e.
                // pci root ports and bus number assignment).  With
                // async enum this is not guaranteed.  Also, an AddDevice call
                // may cause a filter to trigger a reenumeration which needs
                // to be blocked here as well (ACPI may do this if acpisim loads
                // a table in response to the AddDevice).
                //
                // To do so, lock the action queue to drain all requests and to
                // prevent new requests from being dispatched.  Call AddDevice
                // synchronously for each device.  Finally unlock the action
                // queue and drain any delayed actions before moving on.
                //

                PnpLockDeviceActionQueue();
                PipAddDevicesToBootDriver(driverObject);
                PnpUnlockDeviceActionQueue();
                PnpWaitForEmptyDeviceActionQueue();

                //
                // Scan the hardware tree looking for devices which need
                // resources or starting.
                //

                PnpRequestDeviceAction(NULL,
                                       ReenumerateBootDevices,
                                       FALSE,
                                       0,
                                       NULL,
                                       NULL);

            }

            //
            // Before processing next boot driver, wait for
            // IoRequestDeviceRemoval complete. The driver to be processed may
            // need the resources being released by IoRequestDeviceRemoval.
            // (For drivers report detected BOOT device if they fail to get the
            // resources in their DriverEntry.  They will fail and we will
            // bugcheck with inaccessible boot device.)
            //

            if (!PnpWaitForBootDevicesDeleted()) {
                HeadlessKernelAddLogEntry(
                    HEADLESS_LOG_WAIT_BOOT_DEVICES_DELETE_FAILED,
                    NULL);

                return FALSE;
            }

            nextEntry = nextEntry->Flink;
        }

        //
        // If we are done with Bus driver group, then it's time to reserved the
        // Hal resources and reserve boot resources
        //

        if (i == bootBusExtenderGroupIndex) {

            //
            // Reserve BOOT configs on Internal bus 0.
            //

            IopAllocateLegacyBootResources(Internal, 0);
            IopAllocateBootResourcesRoutine = IopAllocateBootResources;
            PNP_ASSERT(IopInitHalResources == NULL);
            PNP_ASSERT(IopInitReservedResourceList == NULL);
            IopBootConfigsReserved = TRUE;

        }
    }

    //
    // Callback Early Launch drivers with initialization status update.
    //

    PnpNotifyEarlyLaunchStatusUpdate(BdCbStatusPrepareForUnload);

    //
    // Unload EarlyLaunch drivers.
    //

    PipUnloadEarlyLaunchDrivers(LoaderBlock);

    //
    // Scan the hardware tree looking for devices which need
    // resources or starting.
    //
    PnPBootDriversLoaded = TRUE;

    PnpRequestDeviceAction(NULL, AssignResources, FALSE, 0, NULL, NULL);

    //
    // If start irps are handled asynchronously, we need to make sure all the
    // boot devices started before continue.
    //

    if (!PnpWaitForDevicesToStart()) {
        HeadlessKernelAddLogEntry(
            HEADLESS_LOG_WAIT_BOOT_DEVICES_START_FAILED,
            NULL);

        return FALSE;
    }

    bootReinitDriversFound = IopCallBootDriverReinitializationRoutines();

    //
    // If there were any drivers that registered for boot reinitialization, then
    // we need to wait one more time to make sure we catch any additional
    // devices that were created in response to the reinitialization callback.
    //

    if (bootReinitDriversFound && !PnpWaitForDevicesToStart()) {
        HeadlessKernelAddLogEntry(
            HEADLESS_LOG_WAIT_BOOT_DEVICES_REINIT_FAILED,
            NULL);

        return FALSE;
    }

    //
    // Initialize RAM disk iff neccessary. RamdiskInitialize will bugcheck on
    // any failure.
    //

    status = RamdiskInitialize(LoaderBlock);
    if (NT_SUCCESS(status)) {

        if (!PnpWaitForDevicesToStart()) {

            HeadlessKernelAddLogEntry(
                HEADLESS_LOG_WAIT_BOOT_DEVICES_START_FAILED,
                NULL);

            return FALSE;
        }
    }

    //
    // Initialize the VHD disk driver.
    //

    status = VhdInitialize(LoaderBlock);
    if (NT_SUCCESS(status)) {
        if (PnpWaitForDevicesToStart() == FALSE) {
            HeadlessKernelAddLogEntry(
                HEADLESS_LOG_WAIT_BOOT_DEVICES_START_FAILED,
                NULL);

            return FALSE;
        }
    }

    //
    // Link NT device names to ARC names now that all of the boot drivers
    // have intialized.
    //

    status = IopCreateArcNames(LoaderBlock);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    //
    // Find and mark the boot partition device object so that if a subsequent
    // access or mount of the device during initialization occurs, an more
    // bugcheck can be produced that helps the user understand why the system
    // is failing to boot and run properly.  This occurs when either one of the
    // device drivers or the file system fails to load, or when the file system
    // cannot mount the device for some other reason.
    //
    // This call should never return unsuccessful.  If a failure did occur,
    // the system should have bugchecked.
    //

    status = PnpBootDeviceWait(LoaderBlock, IopMarkBootPartition, NULL);
    PNP_ASSERT(NT_SUCCESS(status));
    PnPBootDriversInitialized = TRUE;

#if !defined(XBOX_SYSTEMOS)

    if (LoaderBlock->Extension->IsSmbboot) {

        //
        // Wait for Vmbus RDR transport arrival, start Lanman, and
        // start new RDR instance. Wcifs has already configured itself
        // in its DriverEntry.
        //

        status = SbInitialize(LoaderBlock);

        if (!NT_SUCCESS(status)) {
            return FALSE;
        }
    }

#endif

    //
    // Get the number of the current hardware configuration.
    //

    hardwareConfigIndexString[0] = UNICODE_NULL;
    tempStatus = PipHardwareConfigGetIndex(NULL,
                                           &hardwareConfigIndex);

    if (NT_SUCCESS(tempStatus)) {
        tempStatus = RtlStringCchPrintfExW(hardwareConfigIndexString,
                                           MAX_HARDWARE_CONFIG_NUMBER_LENGTH,
                                           NULL,
                                           NULL,
                                           STRSAFE_NULL_ON_FAILURE,
                                           START_OVERRIDE_ENTRY_NAME_FORMAT,
                                           hardwareConfigIndex);
    }

    //
    // Go thru every driver that we initialized.  If the driver is
    // not being used by any devices add it to the service overlay
    // list for this hardware configuration.
    //
    // Also, remove any now unneeded references on objects.
    //

    for (i = 0; i < IopGroupIndex; i++) {
        while (IsListEmpty(&IopGroupTable[i]) == FALSE) {
            nextEntry = RemoveHeadList(&IopGroupTable[i]);
            driverInfo = CONTAINING_RECORD(nextEntry, DRIVER_INFORMATION, Link);
            driverObject = driverInfo->DriverObject;

            //
            // Assert that either the driver didn't initialize (in which case we might not (don't?)
            // have a driverObject, or we do have a driverObject that can be referenced below
            //

            ASSERT((driverInfo->Failed != FALSE) || (driverObject != NULL));

            //
            // If the service overlay for the current hardware configuration was successfully
            // opened or created, check to see if this driver is a PNP driver that is not in use.
            // If so, the driver can be added to the service overlay list to make it not be
            // loaded during the next boot.
            //

            if ((NT_SUCCESS(tempStatus)) &&
                (driverInfo->Failed == FALSE) &&
                (driverObject->DeviceObject == NULL) &&
                (driverObject->DriverExtension->AddDevice) &&
                (!(driverObject->Flags & DRVO_ADDDEVICE_CALLED)) &&
                (!(driverObject->Flags & DRVO_REINIT_REGISTERED))) {

                //
                // Make sure the driver actually has a start type of boot start before it is demoted.
                // Drivers that are only boot start by the boot flags mechanism shouldn't have their
                // start type changed by being added to the service overlay.
                //

                ASSERT(driverInfo->ServiceHandle);
                if (NT_SUCCESS(IopGetRegistryValue(driverInfo->ServiceHandle,
                                                   REGSTR_VAL_START_TYPE,
                                                   0,
                                                   &keyValueInformation))) {

                    if ((keyValueInformation->Type == REG_DWORD) &&
                        (keyValueInformation->DataLength >= sizeof(DWORD))) {

                        startType = *(PDWORD)KEY_VALUE_DATA(keyValueInformation);

                        if (startType == SERVICE_BOOT_START) {
                            //
                            // Add this driver to the service overlay
                            //

                            startType = SERVICE_DEMAND_START;

                            PNP_ASSERT((driverObject->DriverExtension->ServiceKeyName.Buffer != NULL) &&
                                       (driverObject->DriverExtension->ServiceKeyName.Length > sizeof(UNICODE_NULL)));

                            //
                            // Open/create the start override key for this service.
                            //

                            if (NT_SUCCESS(_PnpCtxRegCreateKey(PiPnpRtlCtx,
                                                               driverInfo->ServiceHandle,
                                                               REGSTR_KEY_START_OVERRIDE,
                                                               REG_OPTION_NON_VOLATILE,
                                                               KEY_SET_VALUE,
                                                               NULL,
                                                               &startOverrideKey,
                                                               NULL))) {

                                _PnpCtxRegSetValue(PiPnpRtlCtx,
                                                   startOverrideKey,
                                                   hardwareConfigIndexString,
                                                   REG_DWORD,
                                                   (PVOID)&startType,
                                                   sizeof(startType));

                                _PnpCtxRegCloseKey(PiPnpRtlCtx, startOverrideKey);
                                startOverrideKey = NULL;

                                IopDbgPrint((IOP_LOADUNLOAD_TRACE_LEVEL,
                                             "IOINIT: Driver %wZ has been added to demotion list\n",
                                             &(driverObject->DriverExtension->ServiceKeyName)));

                                if (EventEnabledKMPnPEvt_DriverOverride_SetOverride()) {
                                    PWSTR serviceName = NULL;

                                    if (NT_SUCCESS(PnpUnicodeStringToWstr(&serviceName,
                                                                          NULL,
                                                                          &(driverObject->DriverExtension->ServiceKeyName)))) {

                                        EventWriteKMPnPEvt_DriverOverride_SetOverride(NULL,
                                                                                      hardwareConfigIndex,
                                                                                      serviceName,
                                                                                      startType);

                                        PnpUnicodeStringToWstrFree(serviceName,
                                                                   &(driverObject->DriverExtension->ServiceKeyName));
                                    }
                                }
                            }
                        }
                    }
                    ExFreePool(keyValueInformation);
                }
            }

            if (driverObject) {

                //
                // Reference taken specifically for text mode setup.
                //

                ObDereferenceObject(driverObject);
            }

            if (driverInfo->Failed) {
                driverInfo->DataTableEntry->LdrEntry->KldrEntry.Flags |=
                    LDRP_FAILED_BUILTIN_LOAD;
            }

            ZwClose(driverInfo->ServiceHandle);
            ExFreePool(driverInfo);
        }
    }

    ExFreePool(IopGroupTable);

    //
    // Initialize the drivers necessary to dump all of physical memory to the
    // disk if the system is configured to do so.
    //

    PnpDiagnosticTrace(&KMPnPEvt_BootStart_Stop, 0, NULL);
    return TRUE;
}

VOID
PipInitializeDriverDependentDLLs (
    __in DRIVER_DEPENDENT_DLL_TYPE DependentDLLType,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine is invoked to initialize dependent DLLs for various groups of
    Core drivers and normal boot drivers.

Arguments:

    DependentDLLType - Supplies the type of dependent DLLs to be initialized.


    LoaderBlock - Supplies a pointer to the loader parameter block, created by
        the OS Loader.

Return Value:

    None.

--*/

{

    PBLDR_DATA_TABLE_ENTRY BldrEntry;
    BDCB_IMAGE_INFORMATION BootImageContext;
    BDCB_CLASSIFICATION Classification;
    ULONG Flags;
    BOOLEAN HonorEarlyLaunchPolicy;
    PKLDR_DATA_TABLE_ENTRY KldrEntry;
    BOOLEAN LoadDLL;
    PLIST_ENTRY NextEntry;
    BOOLEAN NotifyEarlyLaunch;
    BOOLEAN Result;

    //
    // ELAM drivers need to be notified of all modules except for Core driver
    // dependent DLLs (as they are intiailized before ELAMs).
    //

    if (DependentDLLType == CoreDriverDependentDLL) {
        NotifyEarlyLaunch = FALSE;

    } else {
        NotifyEarlyLaunch = TRUE;
    }

    //
    // Honor the current early launch driver load policy except when loading
    // Core Extension dependent DLLs as the currently policy applies to
    // normal drivers only.
    //
    // For "known bad" Core Extensions and their imports, the OS policy is not
    // to initialize them. This is because they are initialized before TPM is
    // available and as such could compromise system integrity. This policy
    // currently cannot be overwritten.
    //

    if (DependentDLLType == CoreExtensionDependentDLL) {
        HonorEarlyLaunchPolicy = FALSE;

    } else {
        HonorEarlyLaunchPolicy = TRUE;
    }

    //
    // Walk the list of all modules and load the DLLs that match the supplied
    // DLL category.
    //

    NextEntry = LoaderBlock->LoadOrderListHead.Flink;
    while (NextEntry != &LoaderBlock->LoadOrderListHead) {
        KldrEntry = CONTAINING_RECORD(NextEntry,
                                         KLDR_DATA_TABLE_ENTRY,
                                         InLoadOrderLinks);

        NextEntry = NextEntry->Flink;

        //
        // Skip entries that are not marked as dependent DLLs.
        //

        if ((KldrEntry->Flags & LDRP_DRIVER_DEPENDENT_DLL) == 0) {
            continue;
        }

        BldrEntry = CONTAINING_RECORD(KldrEntry,
                                      BLDR_DATA_TABLE_ENTRY,
                                      KldrEntry);

        Flags = BldrEntry->Flags;

        //
        // A module should either be marked as Core driver dependent DLL or
        // Core Extension dependent DLL but not both.
        //

        NT_ASSERT(((Flags & BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) == 0) ||
                  ((Flags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) == 0));

        LoadDLL = FALSE;
        switch (DependentDLLType) {

        //
        // In this case only load DLLs that have the core driver dependent
        // flag set.
        //

        case CoreDriverDependentDLL:
            if ((Flags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) != 0) {
                LoadDLL = TRUE;
            }

            break;

        //
        // In this case only load DLLs that have the core extension dependent
        // flag set.
        //

        case CoreExtensionDependentDLL:
            if ((Flags & BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) != 0) {
                LoadDLL = TRUE;
            }

            break;

        //
        // In this case load DLLs that are not marked as Core driver or
        // Core Extension dependent.
        //

        case NormalDriverDependentDLL:
            if (((Flags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) == 0) &&
                ((Flags & BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) == 0)) {

                LoadDLL = TRUE;
            }

            break;

        default:

            NT_ASSERT(FALSE);

            break;
        }

        //
        // Load the DLL if it matches the supplied DLL category.
        //
        // If ELAM drivers are to be notified of this module, then setup
        // context and notify every registered AV driver. Putting actual
        // pointers to UNICODE strings, is ok since, the pre processing
        // function is supposed to take care of correctly replicating the
        // data before notification.
        //

        if (LoadDLL != FALSE) {
            if (NotifyEarlyLaunch != FALSE) {
                Classification = BdCbClassificationUnknownImage;
                RtlZeroMemory(&BootImageContext,
                              sizeof(BDCB_IMAGE_INFORMATION));

                BootImageContext.Classification =
                    BdCbClassificationUnknownImage;

                BootImageContext.ImageName = KldrEntry->FullDllName;
                BootImageContext.ImageFlags =
                    BldrEntry->LoadInformation & BDCB_IMAGEFLAGS_MASK;
                BootImageContext.ImageHashAlgorithm =
                    BldrEntry->ImageHashAlgorithm;

                BootImageContext.ImageHashLength =
                    BldrEntry->ImageHashLength;

                BootImageContext.ImageHash = BldrEntry->ImageHash;
                BootImageContext.ThumbprintHashAlgorithm =
                    BldrEntry->ThumbprintHashAlgorithm;

                BootImageContext.CertificateThumbprintLength =
                    BldrEntry->CertificateThumbprintLength;

                BootImageContext.CertificateThumbprint =
                    BldrEntry->CertificateThumbprint;

                PiWstrToUnicodeString(&BootImageContext.RegistryPath, L"");
                BootImageContext.CertificateIssuer =
                    BldrEntry->CertificateIssuer;

                BootImageContext.CertificatePublisher =
                    BldrEntry->CertificatePublisher;

                PnpNotifyEarlyLaunchImageLoad(&BootImageContext,
                                              &Classification);

                Result = PnpDoPolicyCheck(Classification,
                                          HonorEarlyLaunchPolicy);

            } else {
                Result = TRUE;
            }

            if (Result != FALSE) {
                (VOID)MmCallDllInitialize(KldrEntry,
                                          &LoaderBlock->LoadOrderListHead);
            }
        }
    }

    return;
}

VOID
PipInitializeEarlyLaunchDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine is invoked to initialize the Early Launch drivers that were
    loaded by the OS Loader.  The list of drivers is provided as part of the
    loader parameter block.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block, created
        by the OS Loader.

Return Value:

    None.

--*/

{


    PBOOT_DRIVER_LIST_ENTRY BootDriver;
    UNICODE_STRING CompleteName;
    PBLDR_DATA_TABLE_ENTRY DriverEntry;
    PDRIVER_OBJECT DriverObject;
    HANDLE KeyHandle;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    //
    // Initialize anti-malware drivers registered for early launch.
    //

    NextEntry = LoaderBlock->EarlyLaunchListHead.Flink;
    while (NextEntry != &LoaderBlock->EarlyLaunchListHead) {
        BootDriver = CONTAINING_RECORD(NextEntry, BOOT_DRIVER_LIST_ENTRY, Link);
        NextEntry = NextEntry->Flink;

        //
        // Skip past any drivers that failed load in the boot loader.
        //

        if (!NT_SUCCESS(BootDriver->LoadStatus)) {
            continue;
        }

        //
        // Check for certificate resources inside the driver.
        //

        if ((BootDriver->LdrEntry) &&
            (BootDriver->LdrEntry->KldrEntry.DllBase)) {

            SeRegisterElamCertResources(
                BootDriver->LdrEntry->KldrEntry.DllBase,
                BootDriver->LdrEntry->KldrEntry.SizeOfImage,
                FALSE,
                NULL);
        }

        CompleteName.Buffer = NULL;
        KeyHandle = NULL;

        //
        // Initialize each anti-malware boot start driver.
        //

        Status = IopOpenRegistryKeyEx(&KeyHandle,
                                      NULL,
                                      &BootDriver->RegistryPath,
                                      KEY_READ);

        if (!NT_SUCCESS(Status)) {
            goto InitializeNextEarlyLaunchDriver;
        }

        Status = IopGetDriverNameFromKeyNode(KeyHandle, &CompleteName);
        if (!NT_SUCCESS(Status)) {
            goto InitializeNextEarlyLaunchDriver;
        }

        DriverEntry = BootDriver->LdrEntry;
        Status = PnpInitializeBootStartDriver(
                     &CompleteName,
                     &BootDriver->RegistryPath,
                     (PDRIVER_INITIALIZE)(ULONG_PTR)
                     DriverEntry->KldrEntry.EntryPoint,
                     DriverEntry,
                     &LoaderBlock->LoadOrderListHead,
                     FALSE,
                     FALSE,
                     TRUE,
                     &DriverObject);

InitializeNextEarlyLaunchDriver:
        if (KeyHandle != NULL) {
            ZwClose(KeyHandle);
        }

        if (CompleteName.Buffer != NULL) {
            ExFreePool(CompleteName.Buffer);
        }

        if (!NT_SUCCESS(Status)) {
            BootDriver->LdrEntry->KldrEntry.Flags |= LDRP_FAILED_BUILTIN_LOAD;
        }
    }

    return;
}

VOID
PipUnloadEarlyLaunchDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    Unloads all EarlyLaunch drivers from the system so the resources can be
    reclaimed.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block, created
        by the OS Loader.

Return Value:

    None.

--*/

{

    OBJECT_ATTRIBUTES Attributes;
    ULONG AttributeFlags;
    PBOOT_DRIVER_LIST_ENTRY BootDriver;
    UNICODE_STRING ElamHiveKey;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    if (IsListEmpty(&LoaderBlock->EarlyLaunchListHead) != FALSE) {
        return;
    }

    NextEntry = LoaderBlock->EarlyLaunchListHead.Flink;
    while (NextEntry != &LoaderBlock->EarlyLaunchListHead) {
        BootDriver = CONTAINING_RECORD(NextEntry, BOOT_DRIVER_LIST_ENTRY, Link);
        NextEntry = NextEntry->Flink;

        //
        // Skip past any drivers that failed load in the boot loader.
        //

        if (!NT_SUCCESS(BootDriver->LoadStatus)) {
            continue;
        }

        //
        // Get Early Launch driver name, and unload driver.
        //

        Status = IopUnloadDriver(&BootDriver->RegistryPath, TRUE);
    }

    //
    // Unload the Early Launch Anti-Malware registry hive.
    //

    AttributeFlags = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
    RtlInitUnicodeString(&ElamHiveKey, PNP_EARLY_LAUNCH_HIVE_PATH);
    InitializeObjectAttributes(&Attributes,
                               &ElamHiveKey,
                               AttributeFlags,
                               NULL,
                               NULL);

    ZwUnloadKey2(&Attributes, REG_FORCE_UNLOAD);
    return;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
BOOLEAN
PnpCheckTpmCoreGroupStarted (
    VOID
    )

/*++

Routine Description:

    This routine returns whether the TPM Core group (and thereby TPM driver)
    has been started or not.

Parameters:

    None.

Return Value:

    TRUE indicates TPM core group has been started; FALSE otherwise.

--*/

{

    return (PnpCoreDriverGroupLoadPhase > TpmCoreDriverGroup);
}

NTSTATUS
PipAddDevicesToBootDriver(
   __in PDRIVER_OBJECT DriverObject
   )

/*++

Routine Description:

    This functions is used by Pnp manager to inform a boot device driver of
    all the devices it can possibly control.  This routine is for boot
    drivers only.

Parameters:

    DriverObject - Supplies a driver object to receive its boot devices.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status;


    //
    // For each device instance in the driver's service/enum key, we will
    // invoke the driver's AddDevice routine and perform enumeration on
    // the device.
    // Note, we don't acquire registry lock before calling IopApplyFunction
    // routine.  We know this code is for boot driver initialization.  No
    // one else would access the registry Enum key at this time and most
    // important we need the registry lock in other down level routines.
    //

    status = PipApplyFunctionToServiceInstances(
                                NULL,
                                &DriverObject->DriverExtension->ServiceKeyName,
                                KEY_ALL_ACCESS,
                                TRUE,
                                PipAddDevicesToBootDriverWorker,
                                DriverObject,
                                NULL
                                );

    return status;
}

LOGICAL
PipAddDevicesToBootDriverWorker(
    __in HANDLE DeviceInstanceHandle,
    __in PUNICODE_STRING DeviceInstancePath,
    __inout PVOID Context
    )

/*++

Routine Description:

    This routine is a callback function for PipApplyFunctionToServiceInstances.
    It is called for each device instance key referenced by a service instance
    value under the specified service's volatile Enum subkey. The purpose of this
    routine is to invoke the AddDevice() entry of a boot driver with the device
    object.

Arguments:

    DeviceInstanceHandle - Supplies a handle to the registry path (relative to
        HKLM\CCS\System\Enum) to this device instance.

    DeviceInstancePath - Supplies the registry path (relative to HKLM\CCS\System\Enum)
        to this device instance.

    Context - Supplies a pointer to a DRIVER_OBJECT structure.

Return Value:

    TRUE to continue the enumeration.
    FALSE to abort it.

--*/

{
    PDEVICE_OBJECT physicalDevice;

    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( DeviceInstanceHandle );

    //
    // Reference the physical device object associated with the device instance.
    //

    physicalDevice = PnpDeviceObjectFromDeviceInstance(DeviceInstancePath);
    if (!physicalDevice) {
        return TRUE;
    }

    PiProcessAddBootDevices(physicalDevice);
    ObDereferenceObject(physicalDevice);
    return TRUE;
}

LOGICAL
IopInitializeSystemDrivers(
    VOID
    )

/*++

Routine Description:

    This routine is invoked to load and initialize all of the drivers that
    are supposed to be loaded during Phase 1 initialization of the I/O
    system.  This is accomplished by calling the Configuration Manager to
    get a NULL-terminated array of handles to the open keys for each driver
    that is to be loaded, and then loading and initializing the driver.

Arguments:

    None.

Return Value:

    The function value is a LOGICAL indicating whether or not the drivers
    were successfully loaded and initialized.

--*/

{
    NTSTATUS status, driverEntryStatus;
    PHANDLE driverList;
    PHANDLE savedList;
    HANDLE enumHandle;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    UNICODE_STRING groupName, enumName;
    PTREE_ENTRY treeEntry;
    UNICODE_STRING driverName;
    PDRIVER_OBJECT driverObject;

    PnpDiagnosticTrace(&KMPnPEvt_SystemStart_Start, 0, NULL);

    //
    // Get the list of drivers that are to be loaded during this phase of
    // system initialization, and invoke each driver in turn.  Ensure that
    // the list really exists, otherwise get out now.
    //

    driverList = CmGetSystemDriverList(
                     ExIsManufacturingModeEnabled() ?
                         ExpManufacturingInformation.ProfileName.Buffer :
                         NULL);

    if (driverList != NULL) {

        //
        // Walk the entire list, loading each of the drivers if not already loaded,
        // until there are no more drivers in the list.
        //

        for (savedList = driverList; *driverList; driverList++) {

            //
            // Now check if the driver has been loaded already.
            // get the name of the driver object first ...
            //

            status = IopGetDriverNameFromKeyNode( *driverList,
                                                  &driverName );
            if (NT_SUCCESS( status )) {

                driverObject = IopReferenceDriverObjectByName(&driverName);
                RtlFreeUnicodeString(&driverName);
                if (driverObject) {

                    //
                    // Driver was loaded already.  Dereference the driver object
                    // and skip it.
                    //

                    ObDereferenceObject(driverObject);
                    ZwClose(*driverList);
                    continue;
                }
            }

            //
            // Open registry ServiceKeyName\Enum branch to check if the driver was
            // loaded before but failed.
            //

            PiWstrToUnicodeString(&enumName, REGSTR_KEY_ENUM);
            status = IopOpenRegistryKeyEx( &enumHandle,
                                           *driverList,
                                           &enumName,
                                           KEY_READ
                                           );

            if (NT_SUCCESS( status )) {

                ULONG startFailed = 0;

                status = IopGetRegistryValue(enumHandle, L"INITSTARTFAILED", 0, &keyValueInformation);

                if (NT_SUCCESS( status )) {
                    if (keyValueInformation->DataLength == sizeof(ULONG)) {
                        startFailed = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
                    }
                    ExFreePool( keyValueInformation );
                }
                ZwClose(enumHandle);
                if (startFailed != 0) {
                    ZwClose(*driverList);
                    continue;
                }
            }

            //
            // The driver is not loaded yet.  Load it ...
            //

            status = IopGetRegistryValue( *driverList,
                                          REGSTR_VALUE_GROUP,
                                          0,
                                          &keyValueInformation );
            if (NT_SUCCESS( status )) {
                if (keyValueInformation->DataLength) {
                    groupName.Length = (USHORT) keyValueInformation->DataLength;
                    groupName.MaximumLength = groupName.Length;
                    groupName.Buffer = (PWSTR) ((PUCHAR) keyValueInformation + keyValueInformation->DataOffset);
                    treeEntry = PipLookupGroupName( &groupName, TRUE );
                } else {
                    treeEntry = (PTREE_ENTRY) NULL;
                }
                ExFreePool( keyValueInformation );
            } else {
                treeEntry = (PTREE_ENTRY) NULL;
            }

            if (PipCheckDependencies(*driverList)) {

                status = IopLoadDriver(*driverList, TRUE, FALSE, &driverEntryStatus);
                if (NT_SUCCESS(status)) {
                    if (treeEntry) {
                        treeEntry->DriversLoaded++;
                    }
                }

            } else {
                ZwClose(*driverList);
            }

            //
            // The boot process takes a while loading drivers.   Indicate that
            // progress is being made.
            //

            InbvIndicateProgress();

        }

        //
        // Finally, free the pool that was allocated for the list and return
        // an indicator the load operation worked.
        //

        ExFreePool( (PVOID) savedList );
    }

    status = PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                                    StartSystemDevicesPass0,
                                    FALSE,
                                    0,
                                    NULL,
                                    NULL);

    //
    // Handle asynchronous boot override.
    //

    if (PnpBootOptions == PNP_BOOT_OPTION_DISABLE_ASYNC_BOOT) {
        PnpWaitForDevicesToStart();
    }

    PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                           StartSystemDevicesPass1,
                           FALSE,
                           0,
                           NULL,
                           NULL);

    //
    // Free the memory allocated to contain the group dependency list.
    //

    if (IopGroupListHead) {
        PipFreeGroupTree( IopGroupListHead );
    }

    PnpDiagnosticTrace(&KMPnPEvt_SystemStart_Stop, 0, NULL);
    return TRUE;
}

PTREE_ENTRY
PipLookupGroupName(
    __in PUNICODE_STRING GroupName,
    __in LOGICAL Insert
    )

/*++

Routine Description:

    This routine looks up a group entry in the group load tree and either
    returns a pointer to it, or optionally creates the entry and inserts
    it into the tree.

Arguments:

    GroupName - The name of the group to look up, or insert.

    Insert - Indicates whether or not an entry is to be created and inserted
        into the tree if the name does not already exist.

Return Value:

    The function value is a pointer to the entry for the specified group
    name, or NULL.

--*/

{
    PTREE_ENTRY treeEntry;
    PTREE_ENTRY previousEntry;

    //
    // Begin by determining whether or not there are any entries in the tree
    // whatsoever.  If not, and it is OK to insert, then insert this entry
    // into the tree.
    //

    if (!IopGroupListHead) {
        if (!Insert) {
            return (PTREE_ENTRY) NULL;
        } else {
            IopGroupListHead = PipCreateEntry( GroupName );
            return IopGroupListHead;
        }
    }

    //
    // The tree is not empty, so actually attempt to do a lookup.
    //

    treeEntry = IopGroupListHead;

    for (;;) {
        if (GroupName->Length < treeEntry->GroupName.Length) {
            if (treeEntry->Left) {
                treeEntry = treeEntry->Left;
            } else {
                if (!Insert) {
                    return (PTREE_ENTRY) NULL;
                } else {
                    treeEntry->Left = PipCreateEntry( GroupName );
                    return treeEntry->Left;
                }

            }
        } else if (GroupName->Length > treeEntry->GroupName.Length) {
            if (treeEntry->Right) {
                treeEntry = treeEntry->Right;
            } else {
                if (!Insert) {
                    return (PTREE_ENTRY) NULL;
                } else {
                    treeEntry->Right = PipCreateEntry( GroupName );
                    return treeEntry->Right;
                }
            }
        } else {
            if (!RtlEqualUnicodeString( GroupName, &treeEntry->GroupName, TRUE )) {
                previousEntry = treeEntry;
                while (treeEntry->Sibling) {
                    treeEntry = treeEntry->Sibling;
                    if (RtlEqualUnicodeString( GroupName, &treeEntry->GroupName, TRUE )) {
                        return treeEntry;
                    }
                    previousEntry = previousEntry->Sibling;
                }
                if (!Insert) {
                    return (PTREE_ENTRY) NULL;
                } else {
                    previousEntry->Sibling = PipCreateEntry( GroupName );
                    return previousEntry->Sibling;
                }
            } else {
                return treeEntry;
            }
        }
    }
}

USHORT
PipGetDriverTagPriority (
    __in HANDLE ServiceHandle
    )

/*++

Routine Description:

    This routine reads the Tag value of a driver and determine the tag's priority
    among its driver group.

Arguments:

    ServiceHandle - specifies the handle of the driver's service key.

Return Value:

    USHORT for priority.

--*/

{
    NTSTATUS status;
    PKEY_VALUE_FULL_INFORMATION keyValueInformationGroup;
    PKEY_VALUE_FULL_INFORMATION keyValueInformationTag;
    PKEY_VALUE_FULL_INFORMATION keyValueInformationGroupOrderList;
    UNICODE_STRING groupName;
    HANDLE handle;
    USHORT index = (USHORT) -1;
    PULONG groupOrder;
    ULONG count, tag;

    //
    // Open System\CurrentControlSet\Control\GroupOrderList
    //

    PiWstrToUnicodeString(&groupName, L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\GroupOrderList");
    status = IopOpenRegistryKeyEx( &handle,
                                   NULL,
                                   &groupName,
                                   KEY_READ
                                   );

    if (!NT_SUCCESS( status )) {
        return index;
    }

    //
    // Read service key's Group value
    //

    status = IopGetRegistryValue (ServiceHandle,
                                  REGSTR_VALUE_GROUP,
                                  0,
                                  &keyValueInformationGroup);
    if (NT_SUCCESS(status)) {

        //
        // Try to read what caller wants.
        //

        if ((keyValueInformationGroup->Type == REG_SZ) &&
            (keyValueInformationGroup->DataLength != 0)) {
            IopRegistryDataToUnicodeString(&groupName,
                                           (PWSTR)KEY_VALUE_DATA(keyValueInformationGroup),
                                           keyValueInformationGroup->DataLength
                                           );
        }
    } else {

        //
        // If we failed to read the Group value, or no Group value...
        //

        ZwClose(handle);
        return index;
    }

    //
    // Read service key's Tag value
    //

    tag = 0;
    status = IopGetRegistryValue (ServiceHandle,
                                  L"Tag",
                                  DRIVER_TAG_SIZE,
                                  &keyValueInformationTag);
    if (NT_SUCCESS(status)) {

        //
        // Try to read what caller wants.
        //

        if ((keyValueInformationTag->Type == REG_DWORD) &&
            (keyValueInformationTag->DataLength == sizeof(ULONG))) {
            tag = *(PULONG)KEY_VALUE_DATA(keyValueInformationTag);
        } else {
            status = STATUS_UNSUCCESSFUL;
        }

        ExFreePool(keyValueInformationTag);
    }

    if (!NT_SUCCESS(status))  {

        //
        // If we failed to read the Group value, or no Group value...
        //

        ExFreePool(keyValueInformationGroup);
        ZwClose(handle);
        return index;
    }

    //
    // Read group order list value for the driver's Group
    //

    status = IopGetRegistryValue (handle,
                                  groupName.Buffer,
                                  DRIVER_TAG_SIZE,
                                  &keyValueInformationGroupOrderList);
    ExFreePool(keyValueInformationGroup);
    ZwClose(handle);
    if (NT_SUCCESS(status)) {

        //
        // Try to read what caller wants.
        //

        if ((keyValueInformationGroupOrderList->Type == REG_BINARY) &&
            (keyValueInformationGroupOrderList->DataLength >= sizeof(ULONG))) {
            groupOrder = (PULONG)KEY_VALUE_DATA(keyValueInformationGroupOrderList);
            count = *groupOrder;

            PNP_ASSERT((count + 1) * sizeof(ULONG) <= keyValueInformationGroupOrderList->DataLength);

            if ((count + 1) * sizeof(ULONG) <= keyValueInformationGroupOrderList->DataLength) {

                groupOrder++;
                for (index = 1; index <= count; index++) {
                    if (tag == *groupOrder) {
                        break;
                    } else {
                        groupOrder++;
                    }
                }
            }
        }
        ExFreePool(keyValueInformationGroupOrderList);
    } else {

        //
        // If we failed to read the Group value, or no Group value...
        //

        return index;
    }
    return index;
}

VOID
PipInsertDriverList (
    __in PLIST_ENTRY ListHead,
    __in PDRIVER_INFORMATION DriverInfo
    )

/*++

Routine Description:

    This routine reads the Tag value of a driver and determine the tag's priority
    among its driver group.

Arguments:

    ServiceHandle - specifies the handle of the driver's service key.

Return Value:

    USHORT for priority.

--*/

{
    PLIST_ENTRY nextEntry;
    PDRIVER_INFORMATION info;

    nextEntry = ListHead->Flink;
    while (nextEntry != ListHead) {
        info = CONTAINING_RECORD(nextEntry, DRIVER_INFORMATION, Link);

        //
        // Scan the driver info list to find the driver whose priority is
        // lower than current driver's.
        // (Lower TagPosition value means higher Priority)
        //

        if (info->TagPosition > DriverInfo->TagPosition) {
            break;
        }
        nextEntry = nextEntry->Flink;
    }

    //
    // Insert the Driver info to the front of the nextEntry
    //

    nextEntry = nextEntry->Blink;
    InsertHeadList(nextEntry, &DriverInfo->Link);
}

LOGICAL
PnpWaitForDevicesToStart (
    VOID
    )

/*++

Routine Description:

    This routine waits for all currently queue device actions to complete.

Arguments:

    None.

Return Value:

    LOGICAL.

--*/

{
    NTSTATUS status;

    //
    // Wait for the device action queue to empty.
    //

    status = PnpWaitForEmptyDeviceActionQueue();
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    return TRUE;
}

#if defined(_WIN64)

LOGICAL
PipIs32bitKey (
    IN HANDLE KeyHandle
    )

/*++

Routine Description:

    This routine checks if the specified key was created by a 32-bit application.

Arguments:

    KeyHandle - open handle to the key.

Return Value:

    LOGICAL.
--*/

{
    BYTE buffer [sizeof (KEY_VALUE_PARTIAL_INFORMATION) + sizeof (ULONG)];
    KEY_FLAGS_INFORMATION flags;
    PKEY_VALUE_PARTIAL_INFORMATION keyValuePartialInfo;
    ULONG resultLength;
    BOOLEAN retValue;
    NTSTATUS status;
    const static UNICODE_STRING wow64ValueName = RTL_CONSTANT_STRING (L"WOW64");

    retValue = FALSE;

    status = ZwQueryKey (KeyHandle,
                         KeyFlagsInformation,
                         &flags,
                         sizeof (flags),
                         &resultLength);

    if (NT_SUCCESS (status)) {
        if ((flags.Wow64Flags & 0x01) != 0) {
            retValue = TRUE;
        }
    }

    if (retValue == FALSE) {

        keyValuePartialInfo = (PKEY_VALUE_PARTIAL_INFORMATION) &buffer[0];
        status = ZwQueryValueKey (KeyHandle,
                                  (PUNICODE_STRING)&wow64ValueName,
                                  KeyValuePartialInformation,
                                  keyValuePartialInfo,
                                  sizeof (buffer),
                                  &resultLength);

        if (NT_SUCCESS (status)) {
            if ((keyValuePartialInfo->Type == REG_DWORD) &&
                (keyValuePartialInfo->DataLength == sizeof (ULONG))) {
                if ((*(PULONG)keyValuePartialInfo->Data) == 1) {
                    retValue = TRUE;
                }
            }
        }
    }

    return retValue;
}

#endif

NTSTATUS
PnpLoadBootFilterDriver (
    __in HANDLE ServiceKeyHandle,
    __in PUNICODE_STRING DriverName,
    __in ULONG GroupIndex,
    __out PDRIVER_OBJECT *LoadedFilter
    )

/*++

Routine Description:

    This initializes boot filter drivers.

Arguments:

    ServiceKeyHandle - Open key handle to the service key.

    DriverName - specifies the name of the driver to be initialized.

    GroupIndex - specifies the Driver's group index (could be anything)

    LoadedFilter - Pointer to receive a pointer to the loaded driver object.

Return Value:

    PDRIVER_OBJECT

--*/

{
    LOGICAL driverFound;
    PDRIVER_OBJECT driverObject;
    PLIST_ENTRY nextEntry;
    PDRIVER_INFORMATION driverInfo;
    UNICODE_STRING completeName;
    PBOOT_DRIVER_LIST_ENTRY bootDriver;
    PBLDR_DATA_TABLE_ENTRY driverEntry;
    HANDLE keyHandle;
    NTSTATUS status, retStatus;
    ULONG index;

    retStatus = STATUS_UNSUCCESSFUL;
    *LoadedFilter = NULL;

    //
    // If the boot driver initialization phase has not progressed enough or
    // the filter driver is not a boot driver, do nothing.
    //

    if (IopGroupTable == NULL || GroupIndex >= IopGroupIndex) {
        return retStatus;
    }

    //
    // Check to make sure that all previous groups are completely processed.
    //

    for (index = 0; index < GroupIndex; index++) {
        nextEntry = IopGroupTable[index].Flink;
        while (nextEntry != &IopGroupTable[index]) {
            driverInfo = CONTAINING_RECORD(nextEntry, DRIVER_INFORMATION, Link);
            if (driverInfo->Processed == FALSE) {
                return retStatus;
            }

            nextEntry = nextEntry->Flink;
        }
    }

    //
    // Initialize the filter by looking it up in the sorted list.
    //

    nextEntry = IopGroupTable[GroupIndex].Flink;
    driverFound = FALSE;
    while (nextEntry != &IopGroupTable[GroupIndex]) {
        driverInfo = CONTAINING_RECORD(nextEntry, DRIVER_INFORMATION, Link);
        keyHandle = driverInfo->ServiceHandle;
        status = IopGetDriverNameFromKeyNode(keyHandle,
                                             &completeName);
        if (NT_SUCCESS(status)) {
            if (RtlEqualUnicodeString(DriverName,
                                      &completeName,
                                      TRUE)) {    // case-insensitive
                driverFound = TRUE;
                if (driverInfo->Processed == FALSE) {
                    bootDriver = driverInfo->DataTableEntry;
                    driverEntry = bootDriver->LdrEntry;
                    driverInfo->Status =
                        PnpInitializeBootStartDriver(
                            &completeName,
                            &bootDriver->RegistryPath,
                            (PDRIVER_INITIALIZE)(ULONG_PTR)
                            driverEntry->KldrEntry.EntryPoint,
                            driverEntry,
                            &KeLoaderBlock->LoadOrderListHead,
                            TRUE,
                            TRUE,
                            TRUE,
                            &driverObject);

                    retStatus = driverInfo->Status;
                    driverInfo->DriverObject = driverObject;
                    driverInfo->Processed = TRUE;

                    //
                    // Pnp might unload the driver before we get a chance to
                    // look at this. So take an extra reference.
                    //

                    if (driverObject) {
                        ObReferenceObject(driverObject);
                        *LoadedFilter = driverObject;

                    } else {
                        driverInfo->Failed = TRUE;
                    }

                } else {

                    //
                    // This means that we unloaded a boot driver which is not
                    // supposed to happen until the boot process is complete.
                    //

                    if (NT_SUCCESS(driverInfo->Status)) {
                        retStatus = STATUS_UNSUCCESSFUL;

                    } else {
                        retStatus = driverInfo->Status;
                    }
                }

                ExFreePool(completeName.Buffer);
                break;
            }

            ExFreePool(completeName.Buffer);
        }

        nextEntry = nextEntry->Flink;
    }

#if defined(_WIN64)

    if ((!NT_SUCCESS (retStatus)) &&
        (driverFound == FALSE)) {

        if (PipIs32bitKey (ServiceKeyHandle) != FALSE) {
            retStatus = STATUS_DRIVER_BLOCKED;
        }
    }

#else

    UNREFERENCED_PARAMETER (ServiceKeyHandle);

#endif

    return retStatus;
}

VOID
PnpMarkHalDeviceNode(
    VOID
    )

/*++

Routine Description:

    This function will walk the device tree and mark the first
    non-legacy device that is started. This needs to be called
    immediately after HalInitPnpDriver.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if !defined(XBOX_SYSTEMOS)

    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    PNP_ASSERT(IopInitHalDeviceNode == NULL);
    PNP_ASSERT(IopRootDeviceNode != NULL);

    //
    // Walk the tree looking for a non-legacy device
    // thats already started. Only the HAL device should
    // be started at this point.
    //

    deviceNode = IopRootDeviceNode->Child;
    while (deviceNode) {
        if ((deviceNode->State == DeviceNodeStarted ||
             deviceNode->State == DeviceNodeStartPostWork ) &&
            !(deviceNode->Flags & DNF_LEGACY_DRIVER)) {

            //
            // Mark this devnode as the HAL devnode.
            //

            IopInitHalDeviceNode = deviceNode;
            PipSetDevNodeFlags(deviceNode, DNF_HAL_NODE);
            break;
        }

        deviceNode = deviceNode->Sibling;
    }

    PNP_ASSERT(IopInitHalDeviceNode != NULL);

#endif

    return;
}

NTSTATUS
PiInitCacheGroupInformation(
    VOID
    )
/*++

Routine Description:

    This routine caches the service group order list. We only need this list
    while we are processing boot start and system start legacy drivers.

Parameters:

    None.

Return Value:

    NTSTATUS.

--*/
{
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    UNICODE_STRING *groupTable, group;
    NTSTATUS status;
    HANDLE handle;
    ULONG count = 0;

    //
    // Open System\CurrentControlSet\Control\ServiceOrderList
    //
    PiWstrToUnicodeString(
        &group,
        L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\ServiceGroupOrder"
        );

    status = IopOpenRegistryKeyEx(
        &handle,
        NULL,
        &group,
        KEY_READ
        );

    if (!NT_SUCCESS(status)) {

        return status;
    }

    //
    // Read and build a unicode string array containing all the group names.
    //
    status = IopGetRegistryValue(
        handle,
        L"List",
        0,
        &keyValueInformation
        );

    ZwClose(handle);

    groupTable = NULL;
    if (NT_SUCCESS(status)) {

        if ((keyValueInformation->Type == REG_MULTI_SZ) &&
            (keyValueInformation->DataLength != 0)) {

            status = PnpRegMultiSzToUnicodeStrings(keyValueInformation,
                                                   &groupTable,
                                                   &count);
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
        ExFreePool(keyValueInformation);
    }

    if (!NT_SUCCESS(status)) {

        return status;
    }

    PiInitGroupOrderTable = groupTable;
    PiInitGroupOrderTableCount = (USHORT) count;
    return STATUS_SUCCESS;
}

VOID
PiInitReleaseCachedGroupInformation(
    VOID
    )
/*++

Routine Description:

    This routine releases the service group order list cache. It should be
    called just after the system start legacy drivers have been loaded.

Parameters:

    None.

Return Value:

    None.

--*/
{

    PAGED_CODE();

    PNP_ASSERT(PnPInitialized);

    if (PiInitGroupOrderTable) {

        PnpFreeUnicodeStringList(PiInitGroupOrderTable,
                                 PiInitGroupOrderTableCount);

        PiInitGroupOrderTable = NULL;
        PiInitGroupOrderTableCount = 0;
    }
}

USHORT
PpInitGetGroupOrderIndex(
    __in_opt HANDLE ServiceHandle
    )
/*++

Routine Description:

    This routine reads the Group value of the service key, and finds its position
    in the ServiceOrderList. If ServiceHandle is NULL, it returns a value with
    max group order + 1.  If the group value is missing or unrecognized, it
    returns a value equal to max group order.

Parameters:

    ServiceHandle - supplies a handle to the service key.

Return Value:

    group order index.

--*/
{
    UNICODE_STRING group;
    USHORT index;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(!PnPInitialized);

    if (PiInitGroupOrderTable == NULL) {

        return NO_MORE_GROUP;
    }

    if (ServiceHandle == NULL) {

        return PiInitGroupOrderTableCount + 1;
    }

    //
    // Read service key's Group value
    //

    status = IopGetRegistryValue(
        ServiceHandle,
        REGSTR_VALUE_GROUP,
        0,
        &keyValueInformation
        );

    if (!NT_SUCCESS(status)) {

        //
        // If we failed to read the Group value, or no Group value...
        //

        return PiInitGroupOrderTableCount;
    }

    //
    // Verify type information
    //

    if ((keyValueInformation->Type != REG_SZ) ||
        (keyValueInformation->DataLength == 0)) {

        PNP_ASSERT(0);
        ExFreePool(keyValueInformation);
        return PiInitGroupOrderTableCount;
    }

    IopRegistryDataToUnicodeString(
        &group,
        (PWSTR)KEY_VALUE_DATA(keyValueInformation),
        keyValueInformation->DataLength
        );

    for (index = 0; index < PiInitGroupOrderTableCount; index++) {

        if (RtlEqualUnicodeString(&group, &PiInitGroupOrderTable[index], TRUE)) {

            break;
        }
    }

    ExFreePool(keyValueInformation);

    return index;
}

_Must_inspect_result_
BOOLEAN
PpInitSystem (
    VOID
    )

/*++

Routine Description:

    This function performs initialization of the kernel-mode Plug and Play
    Manager.  It is called during phase 0 and phase 1 initialization.  Its
    function is to dispatch to the appropriate phase initialization routine.

Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{

    switch ( InitializationPhase ) {

    case 0 :
        return PnpInitPhase0();

    case 1 :
        return PnpInitPhase1();

    default:
        KeBugCheckEx(UNEXPECTED_INITIALIZATION_CALL, 2, InitializationPhase, 0, 0);
    }
}

BOOLEAN
PnpInitPhase0(
    VOID
    )

/*++

Routine Description:

    This function performs Phase 0 initializaion of the Plug and Play Manager
    component of the NT system. It initializes the PnP registry and bus list
    resources, and initializes the bus list head to empty.

Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{
    //
    // Initialize the device-specific, Plug and Play registry resource.
    //

    ExInitializeResourceLite(&PnpRegistryDeviceResource);
    PnpInitializeDeviceReferenceTable();
    return TRUE;
}

BOOLEAN
PnpInitPhase1(
    VOID
    )

/*++

Routine Description:

    This function performs Phase 1 initializaion of the Plug and Play Manager
    component of the NT system.

Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{
    return TRUE;
}

BOOLEAN
PnpDoPolicyCheck (
    __in BDCB_CLASSIFICATION Classification,
    __in BOOLEAN HonorEarlyLaunchPolicy
    )

/*++

Routine Description:

    This function decides whether to load a driver based on the current policy.

Arguments:

    Classification - Supplies the systems decision to load a driver.

    HonorEarlyLaunchPolicy - Supplies a flag indicating whether the current
        early launch policy should be honored when a bad image is detected.
        The registry policy, for instance, is not honored when Core Extensions
        are being initialized.

Return Value:

    TRUE - Policy test has passed.

    FALSE - Policy test has failed.

--*/

{

    DWORD Policy;
    BOOLEAN Result;

    Policy = PnpDriverImageLoadPolicy;
    Result = FALSE;

    //
    // Allow load when there are no AM drivers registered for callback.
    //

    if (PnpEarlyLaunchHasCallbacksRegistered() == FALSE) {
        Result = TRUE;
        goto PnpDoPolicyCheckEnd;
    }

    switch (Classification) {
    case BdCbClassificationKnownGoodImage:
        Result = TRUE;
        break;

    case BdCbClassificationUnknownImage:
        if ((Policy & PNP_INITIALIZE_UNKNOWN_DRIVERS) != 0) {
            Result = TRUE;
        }

        break;

    case BdCbClassificationKnownBadImageBootCritical:
        if (((Policy & PNP_INITIALIZE_BAD_CRITICAL_DRIVERS) != 0) &&
            (HonorEarlyLaunchPolicy != FALSE)) {

            Result = TRUE;
        }

        break;

    case  BdCbClassificationKnownBadImage:
        if (((Policy & PNP_INITIALIZE_BAD_DRIVERS) != 0) &&
            (HonorEarlyLaunchPolicy != FALSE)) {

            Result = TRUE;
        }

        break;

    default:

        NT_ASSERT(FALSE);

        break;

    }

    //
    // Log the Classification, LoadPolicy and the Decision.
    //

    PnpDiagnosticTraceElamDecision(&KMPnPEvt_EarlyLaunch_PolicyCheck,
                                   Classification,
                                   Policy,
                                   Result);

PnpDoPolicyCheckEnd:

    return Result;
}


NTSTATUS
PnpBootPhaseComplete(
    VOID
    )

/*++

Routine Description:

    This function performs Phase 2 initialization of the Plug and Play Manager
    upon the completion of the system boot phase. This involve completing the
    initialization of pnprtl to enable full access to the driver database.
    Any devices that were not previously initialized are revisited since it
    might be possible to get them configured and/or started now.

    Note that this function is called at the tail end of NtInitializeRegistry
    in order to ensure that the file system and registry are ready for use.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    //
    // Complete initialization of pnprtl now that registry is initialized.
    // This completes the initialization of the driver database so that all
    // known drivers on the system are available for device configuration.
    //
    status = PiPnpRtlInit(2);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Boot mode is now over, user-mode was successfully bootstapped.
    //
    PnpBootMode = FALSE;

    //
    // Retry all devices whose drivers could not load during boot
    // (e.g. if autochk.exe had locked the volume).
    //
    PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                           ClearProblem,
                           FALSE,
                           (ULONG_PTR)CM_PROB_DRIVER_FAILED_LOAD,
                           NULL,
                           NULL);

    //
    // Configure any devices that did not get drivers during the initial boot
    // phase, and reconfigure any devices that have better matching drivers in
    // the fully initialized driver database.
    //
    status = PpDevCfgProcessDevices();

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    return status;
}

NTSTATUS
PnpInitializeBootStartDriver (
    __in PUNICODE_STRING DriverName,
    __in PUNICODE_STRING RegistryPath,
    __in PDRIVER_INITIALIZE EntryPoint,
    __in_opt PBLDR_DATA_TABLE_ENTRY LoaderEntry,
    __in PLIST_ENTRY LoaderModuleListHead,
    __in LOGICAL IsFilter,
    __in LOGICAL InvokeNotifications,
    __in BOOLEAN HonorEarlyLaunchPolicy,
    __out PDRIVER_OBJECT *DriverObject
    )

/*++

Description

    This routine performs a diagnostic trace while initializing a boot driver
    loaded by OsLoader.

Parameters

    DriverName - Name of the boot driver.

    RegistryPath - Service key path for the boot driver.

    EntryPoint - Supplies a pointer to the DriverEntry routine.

    LoaderEntry - Supplies a pointer to the entry created for this driver by the
        OsLoader.

    LoaderModuleListHead - Supplies a pointer to the OsLoader list of loaded
        modules.

    IsFilter - If TRUE, driver being initialized is a filter driver.

    InvokeNotifications - Supplies an indication that this driver's load should
        be communicated to the early launch anti-malware driver.

    HonorEarlyLaunchPolicy - Supplies a flag indicating whether the current
        early launch policy should be honored when a bad image is detected.
        The registry policy, for instance, is not honored when Core Extensions
        are being initialized. This flag only matters if early launch
        anti-malware driver is going to be notified.

    DriverObject - Recieves pointer to the driver object.

Result

    NTSTATUS.

--*/

{

    NTSTATUS Status;
    ULONG VerifierSnapshot;
    BDCB_CLASSIFICATION Classification;
    BDCB_IMAGE_INFORMATION BootImageContext;

    Classification = BdCbClassificationUnknownImage;

    //
    // Only invoke notification if driver is not an early launch driver.
    //

    if (InvokeNotifications != FALSE) {

        //
        // Setup context and notify every registered AV driver.
        //

        RtlZeroMemory(&BootImageContext, sizeof(BDCB_IMAGE_INFORMATION));
        BootImageContext.Classification = BdCbClassificationUnknownImage;
        if (ARGUMENT_PRESENT(LoaderEntry) != FALSE) {
            BootImageContext.ImageName = LoaderEntry->KldrEntry.FullDllName;
            BootImageContext.ImageFlags =
                LoaderEntry->LoadInformation & BDCB_IMAGEFLAGS_MASK;
            BootImageContext.ImageHashAlgorithm =
                LoaderEntry->ImageHashAlgorithm;

            BootImageContext.ImageHashLength = LoaderEntry->ImageHashLength;
            BootImageContext.ImageHash = LoaderEntry->ImageHash;
            BootImageContext.ThumbprintHashAlgorithm =
                LoaderEntry->ThumbprintHashAlgorithm;

            BootImageContext.CertificateThumbprintLength =
                LoaderEntry->CertificateThumbprintLength;

            BootImageContext.CertificateThumbprint =
                LoaderEntry->CertificateThumbprint;

            BootImageContext.RegistryPath = *RegistryPath;
            BootImageContext.CertificateIssuer = LoaderEntry->CertificateIssuer;
            BootImageContext.CertificatePublisher =
                LoaderEntry->CertificatePublisher;

        } else {
            BootImageContext.ImageName = *DriverName;
        }

        //
        // Notify ever AntiMalware driver about this driver load.
        //

        PnpNotifyEarlyLaunchImageLoad(&BootImageContext, &Classification);
    }

    //
    // Always load the driver if it is an anti malware driver. For normal
    // drivers: check the AntiMalware classificaion against the system policy,
    // to decide whether to load this driver or not.
    //

    if ((PnpDoPolicyCheck(Classification, HonorEarlyLaunchPolicy) != FALSE) ||
        (InvokeNotifications == FALSE)) {

        //
        // Notify the verifier that we are about to initialize a driver.
        //

        VerifierSnapshot = VfDriverInitStarting ();

        IopDbgPrint((IOP_LOADUNLOAD_TRACE_LEVEL,
                     "PnpInitializeBootStartDriver() - "
                     "Initializing boot start driver %wZ =>\n",
                     DriverName));

        PnpDiagnosticTraceObject(&KMPnPEvt_BootInit_Start, DriverName);

        Status = IopInitializeBuiltinDriver(DriverName,
                                            RegistryPath,
                                            EntryPoint,
                                            (PKLDR_DATA_TABLE_ENTRY)LoaderEntry,
                                            (BOOLEAN)IsFilter,
                                            DriverObject);

        PnpDiagnosticTraceObjectWithStatus(&KMPnPEvt_BootInit_Stop,
                                           DriverName,
                                           Status);

        if (NT_SUCCESS(Status)) {

            IopDbgPrint((IOP_LOADUNLOAD_TRACE_LEVEL,
                         "PnpInitializeBootStartDriver() - "
                         "Successfully initialized boot start driver %wZ <=\n",
                         DriverName));

            //
            // Notify the verifier that the driver was successfuly initialized.
            //

            VfDriverInitSuccess (VerifierSnapshot, LoaderModuleListHead);

        } else {
            IopDbgPrint((((Status == STATUS_PORT_DISCONNECTED) ||
                          (Status == STATUS_PLUGPLAY_NO_DEVICE)) ?
                         IOP_LOADUNLOAD_WARNING_LEVEL :
                         IOP_LOADUNLOAD_ERROR_LEVEL,
                         "PnpInitializeBootStartDriver() - "
                         "Failed (status = 0x%08X) to initialize boot start "
                         "driver %wZ <=\n",
                         Status,
                         DriverName));
        }

    } else {
        Status = STATUS_ACCESS_DENIED;
    }

    return Status;
}

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
PnpInitializeProcessor (
    __in PKPRCB Prcb
    )

/*++

Description

    This routine will trigger a resource rebalance to allow interrupts to
    be retargetted for participating devices. This is called as part of
    processor initialization.

Parameters

    Prcb - Pointer to the processor control block.

Return

    STATUS_SUCCESS.

--*/

{
    UNREFERENCED_PARAMETER(Prcb);

    PAGED_CODE();

    //
    // Queue resource rebalance asynchronously.
    //

    PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                           Rebalance,
                           FALSE,
                           TRUE,
                           NULL,
                           NULL);

    return STATUS_SUCCESS;
}

NTSTATUS
PnpShutdownDevices (
    VOID
    )

/*++

Routine Description:

    This function is called by the IO system driver verifier during shutdown.
    It queues a work item to Query/Remove all the devices in the tree.  All
    the ones supporting removal will be removed and their drivers unloaded if
    all instances of their devices are removed.

    This API should only be called once during shutdown, it has no effect on the
    second and subsequent calls.

Parameters:

    NONE.

Return Value:

    STATUS_SUCCESS if the process was successfully completed.  Doesn't mean
    any devices were actually removed.  Otherwise an error code indicating the
    error.  There is no guarantee that no devices have been removed if an error
    occurs however in the current implementation the only time an error will
    be reported is if the operation couldn't be queued.

--*/

{
    PPNP_DEVICE_EVENT_ENTRY deviceEvent;
    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    PPNP_DEVICE_EVENT_ENTRY tempDeviceEvent;
    ULONG totalSize;
    WCHAR vetoName[80];
    UNICODE_STRING vetoNameString;
    PNP_VETO_TYPE vetoType;

    PAGED_CODE();

    //
    // Prevent any more events or action worker items from being
    // queued\processed.
    //

    KeSetEvent(&PnpShutdownEvent, 0, FALSE);

    //
    // For clean shutdown, attempt to tear down the entire device tree.
    //

    status = STATUS_SUCCESS;
    if (PnpTearDownPnpStacksOnShutdown ||
        (PoCleanShutdownEnabled() & PO_CLEAN_SHUTDOWN_PNP)) {

        totalSize = sizeof(PLUGPLAY_EVENT_BLOCK) +
                        FIELD_OFFSET(PNP_DEVICE_EVENT_ENTRY, Data) +
                        MAX_DEVICE_ID_LEN + 1;

        deviceEvent = PnpCreateDeviceEventEntry(totalSize);
        if (deviceEvent == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // Drain device event queue.
        //

        PnpWaitForEmptyDeviceEventQueue();

        //
        // Drain and device action queue.
        //

        PnpWaitForEmptyDeviceActionQueue();

        vetoNameString.Length = 0;
        vetoNameString.MaximumLength = sizeof(vetoName);
        vetoNameString.Buffer = vetoName;
        deviceNode = IopRootDeviceNode;
        PipSetDevNodeUserFlags(deviceNode, DNUF_SHUTDOWN_QUERIED);
        for (;;) {

            //
            // Acquire the registry lock to prevent in process removals causing
            // Devnodes to be unlinked from the tree.
            //

            PiLockPnpRegistry(FALSE);

            //
            // Walk the tree looking for devices to tear down.
            //

            deviceNode = deviceNode->Child;
            while (deviceNode != NULL) {
                if (deviceNode->UserFlags & DNUF_SHUTDOWN_SUBTREE_DONE) {
                    if (deviceNode == IopRootDeviceNode) {

                        //
                        // We've processed the entire devnode tree - we're done
                        //

                        deviceNode = NULL;
                        break;
                    }

                    if (deviceNode->Sibling == NULL) {
                        deviceNode = deviceNode->Parent;
                        PipSetDevNodeUserFlags(deviceNode, DNUF_SHUTDOWN_SUBTREE_DONE);

                    } else {
                        deviceNode = deviceNode->Sibling;
                    }

                    continue;
                }

                if (deviceNode->UserFlags & DNUF_SHUTDOWN_QUERIED) {
                    if (deviceNode->Child == NULL) {
                        PipSetDevNodeUserFlags(deviceNode, DNUF_SHUTDOWN_SUBTREE_DONE);
                        if (deviceNode->Sibling == NULL) {
                            deviceNode = deviceNode->Parent;
                            PipSetDevNodeUserFlags(deviceNode, DNUF_SHUTDOWN_SUBTREE_DONE);

                        } else {
                            deviceNode = deviceNode->Sibling;
                        }

                    } else {
                        deviceNode = deviceNode->Child;
                    }

                    continue;
                }

                break;
            }

            PiUnlockPnpRegistry();
            if (deviceNode != NULL) {
                PnpInitializeTargetDeviceRemoveEvent(deviceEvent,
                                                     totalSize,
                                                     deviceNode->PhysicalDeviceObject,
                                                     FALSE,
                                                     TRUE,
                                                     FALSE,
                                                     FALSE,
                                                     CM_PROB_SYSTEM_SHUTDOWN,
                                                     STATUS_SUCCESS,
                                                     NULL,
                                                     NULL,
                                                     &vetoType,
                                                     &vetoNameString);

                tempDeviceEvent = deviceEvent;
                status = PnpProcessQueryRemoveAndEject(&tempDeviceEvent);
                if (!NT_SUCCESS(status)) {
                }

                //
                // If the event entry was expanded to include related devices,
                // discard it and continue using the original which should be
                // currently set as the parent of the expanded event entry.
                //

                if (tempDeviceEvent != deviceEvent) {

                    PNP_ASSERT(tempDeviceEvent->Parent == deviceEvent);

                    PnpDeleteDeviceEventEntry(tempDeviceEvent);
                }
            }

            if (deviceNode == NULL) {

                //
                // We've processed the entire tree.
                //

                break;
            }
        }

        PnpDeleteDeviceEventEntry(deviceEvent);
    }

    return status;
}

__drv_maxIRQL(APC_LEVEL)
VOID
PnpShutdownSystem (
    __in LOGICAL Reboot,
    __in ULONG Phase,
    __inout PVOID *Context
    )

/*++

Routine Description:

    This routine performs Pnp shutdown preparation.

Arguments:

    Reboot - specifies if the system is going to reboot.

    Phase - specifies the shutdown phase.

    Context - at phase 0, it supplies a variable to receive the returned context info.
              at phase 1, it supplies a variable to specify the context info.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Reboot);
    UNREFERENCED_PARAMETER(Phase);
    UNREFERENCED_PARAMETER(Context);
}

NTSTATUS
PnpBootDeviceWait (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PBOOT_DEVICE_WAIT_FUNCTION DeviceWaitCallback,
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is essentially a wrapper for DeviceWaitCallback.  It compensates
    for boot devices that may enumerate asynchronously by polling the device
    tree over a registry defined interval.

Arguments:

    LoaderBlock - supplies a pointer to the LoaderBlock passed in from the
        OS Loader.

    DeviceWaitCallback - Supplies a function to call until successful, or
        until the maximum timeout.

    Context - Supplies an application context buffer.

Return Value:

    STATUS_SUCCESS if everything worked, otherwise the system will bugcheck.

--*/

{
    UNICODE_STRING BootDeviceNameUnicodeString;
    LARGE_INTEGER DueTime;
    UNICODE_STRING GroupName;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation;
    LONG PollBootPartitionTimeout;
    HANDLE RegPnpKey;
    ULONG RegValue;
    NTSTATUS Status;

    PAGED_CODE();

    RtlInitUnicodeString(&BootDeviceNameUnicodeString, NULL);
    PollBootPartitionTimeout = 0;

    //
    // Query the registry to get the total time to allow
    // for enumeration.
    //

    PiWstrToUnicodeString(&GroupName, L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\PnP");
    Status = IopOpenRegistryKeyEx(&RegPnpKey,
                                  NULL,
                                  &GroupName,
                                  KEY_READ
                                  );

    if (NT_SUCCESS(Status)) {

        Status = IopGetRegistryValue(
                     RegPnpKey,
                     L"PollBootPartitionTimeout",
                     0,
                     &KeyValueInformation);

        if (NT_SUCCESS(Status)) {
            if (KeyValueInformation->DataLength == sizeof(ULONG)) {

                RegValue = *(PULONG)KEY_VALUE_DATA(KeyValueInformation);
                ExFreePool(KeyValueInformation);

                //
                // If the timeout is above the limit, default to the maximum.
                // If the timeout is below the polling interval, default
                // to 0.
                //

                if (RegValue < PNP_POLL_BOOT_PARTITION_INTERVAL) {
                    PollBootPartitionTimeout = 0;

                } else if (RegValue > PNP_MAX_POLL_BOOT_PARTITION_TIMEOUT) {
                    PollBootPartitionTimeout = PNP_MAX_POLL_BOOT_PARTITION_TIMEOUT;

                } else {
                    PollBootPartitionTimeout = RegValue;
                }

            } else {
                Status = STATUS_UNSUCCESSFUL;
            }
        }

        ZwClose(RegPnpKey);
    }

    if (!NT_SUCCESS(Status)) {

        //
        // Unable to read from registry.  In this case, default to a poll timeout
        // of 0.
        //

        PollBootPartitionTimeout = 0;
    }

    DueTime.QuadPart = -1 * 10000 * PNP_POLL_BOOT_PARTITION_INTERVAL;

    IopDbgPrint((IOP_WARNING_LEVEL,
                 "PnpBootDeviceWait: Polling timeout set to %d milliseconds\n",
                 PollBootPartitionTimeout));

    for (;;) {

        RtlFreeUnicodeString(&BootDeviceNameUnicodeString);
        Status = DeviceWaitCallback(LoaderBlock,
                                    Context,
                                    &BootDeviceNameUnicodeString);

        if (NT_SUCCESS(Status) || (PollBootPartitionTimeout <= 0)) {
            break;
        }

        IopDbgPrint((IOP_WARNING_LEVEL,
                    "PnpBootDeviceWait: Remaining time to poll for boot partition: %d milliseconds\n",
                    PollBootPartitionTimeout));

        KeDelayExecutionThread(
            KernelMode,
            FALSE,
            &DueTime);

        PollBootPartitionTimeout -= PNP_POLL_BOOT_PARTITION_INTERVAL;
    }

    if (!NT_SUCCESS(Status)) {
       HeadlessKernelAddLogEntry(HEADLESS_LOG_MARK_BOOT_PARTITION_FAILED, NULL);
       KeBugCheckEx(INACCESSIBLE_BOOT_DEVICE,
           (ULONG_PTR) &BootDeviceNameUnicodeString,
           Status,
           0,
           0 );
    }

    RtlFreeUnicodeString(&BootDeviceNameUnicodeString);
    return STATUS_SUCCESS;
}

NTSTATUS
PiInitFirmwareResources(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Description:

    This routine will initialize the registry based UEFI ESRT firmware
    resource table for use by the UEFI firmware PnP device class driver.

Arguments:

    LoaderBlock - supplies a pointer to the LoaderBlock passed in from the
        OS Loader.

Return Value:

    NT Status code.

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;
    PEFI_FIRMWARE_INFORMATION EfiInformation;
    PLIST_ENTRY FirmwareResourceList;
    HANDLE UefiKeyHandle = NULL;
    HANDLE EsrtKeyHandle = NULL;
    PLIST_ENTRY ListEntry;
    PFIRMWARE_RESOURCE_LIST_ENTRY FirmwareResourceListEntry;
    PEFI_SYSTEM_RESOURCE_ENTRY FirmwareResource;
    UNICODE_STRING UnicodeString;
    UNICODE_STRING FirmwareIdString;
    HANDLE FirmwareResourceKeyHandle = NULL;

    //
    // Firmware resources are only available on EFI-based systems.
    //
    if (LoaderBlock->FirmwareInformation.FirmwareTypeEfi != 1) {
        Status = STATUS_SUCCESS;
        goto Cleanup;
    }

    //
    // Get ESRT firmware resource list from loader block EFI information.
    //
    EfiInformation = &LoaderBlock->FirmwareInformation.u.EfiInformation;
    FirmwareResourceList = &EfiInformation->FirmwareResourceList;

    if (IsListEmpty(FirmwareResourceList)) {
        Status = STATUS_SUCCESS;
        goto Cleanup;
    }

    //
    // Create volatile HKLM\HARDWARE\UEFI key when firmware resources exist.
    //
    PiWstrToUnicodeString(&UnicodeString,
                          CM_REGISTRY_MACHINE(REGSTR_PATH_UEFI));

    Status = IopCreateRegistryKeyEx(&UefiKeyHandle,
                                    NULL,
                                    &UnicodeString,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_VOLATILE,
                                    NULL);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Create UEFI ESRT firmware resources key.
    //
    PiWstrToUnicodeString(&UnicodeString, REGSTR_KEY_ESRT);

    Status = IopCreateRegistryKeyEx(&EsrtKeyHandle,
                                    UefiKeyHandle,
                                    &UnicodeString,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_VOLATILE,
                                    NULL);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    for (ListEntry = FirmwareResourceList->Flink;
         ListEntry != FirmwareResourceList;
         ListEntry = ListEntry->Flink) {
        //
        // Get next firmware resource entry.
        //
        FirmwareResourceListEntry = CONTAINING_RECORD(ListEntry,
                                                      FIRMWARE_RESOURCE_LIST_ENTRY,
                                                      Entry);

        FirmwareResource = &FirmwareResourceListEntry->FirmwareResource;

        //
        // Allocate firmware resource ID string.
        //
        Status = RtlStringFromGUID((LPGUID)&FirmwareResource->FirmwareClass,
                                   &FirmwareIdString);

        if (NT_SUCCESS(Status)) {
            //
            // Create firmware resource key.
            //
            Status = IopCreateRegistryKeyEx(&FirmwareResourceKeyHandle,
                                            EsrtKeyHandle,
                                            &FirmwareIdString,
                                            KEY_ALL_ACCESS,
                                            REG_OPTION_VOLATILE,
                                            NULL);

            //
            // Free firmware resource ID string.
            //
            RtlFreeUnicodeString(&FirmwareIdString);
        }

        if (!NT_SUCCESS(Status)) {
            break;
        }

        //
        // Set firmware resource values.
        //
        PiWstrToUnicodeString(&UnicodeString, REGSTR_VAL_ESRT_TYPE);

        ZwSetValueKey(FirmwareResourceKeyHandle,
                      &UnicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &FirmwareResource->FirmwareType,
                      sizeof(ULONG));

        PiWstrToUnicodeString(&UnicodeString, REGSTR_VAL_ESRT_VERSION);

        ZwSetValueKey(FirmwareResourceKeyHandle,
                      &UnicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &FirmwareResource->FirmwareVersion,
                      sizeof(ULONG));

        PiWstrToUnicodeString(&UnicodeString,
                              REGSTR_VAL_ESRT_LOWEST_SUPPORTED_VERSION);

        ZwSetValueKey(FirmwareResourceKeyHandle,
                      &UnicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &FirmwareResource->LowestSupportedFirmwareVersion,
                      sizeof(ULONG));

        PiWstrToUnicodeString(&UnicodeString,
                              REGSTR_VAL_ESRT_LAST_ATTEMPT_VERSION);

        ZwSetValueKey(FirmwareResourceKeyHandle,
                      &UnicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &FirmwareResource->FirmwareUpdateStatus.LastAttemptVersion,
                      sizeof(ULONG));

        PiWstrToUnicodeString(&UnicodeString,
                              REGSTR_VAL_ESRT_LAST_ATTEMPT_STATUS);

        ZwSetValueKey(FirmwareResourceKeyHandle,
                      &UnicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &FirmwareResource->FirmwareUpdateStatus.LastAttemptStatus,
                      sizeof(ULONG));

        //
        // Close firmware resource key.
        //
        ZwClose(FirmwareResourceKeyHandle);
    }

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

Cleanup:
    //
    // Close registry keys.
    //
    if (EsrtKeyHandle != NULL) {
        ZwClose(EsrtKeyHandle);
    }

    if (UefiKeyHandle != NULL) {
        ZwClose(UefiKeyHandle);
    }

    return Status;
}

//
// PnP device override cache.
//
PLIST_ENTRY PnpDeviceOverrideHashList = NULL;
ULONG       PnpDeviceOverrideHashListSize = 0;

static const ULONG PnpDeviceOverrideHashListBucketSizes[] = {
    11,
    67,
    257,
};

#define PNP_DEVICE_OVERRIDE_HASH_BUCKET_MAX_SIZE 3

//
// PnP device override cache entry.
//
typedef struct _PNP_DEVICE_OVERRIDE_ENTRY {
    LIST_ENTRY      ListEntry;
    UNICODE_STRING  HardwareId;
} PNP_DEVICE_OVERRIDE_ENTRY, *PPNP_DEVICE_OVERRIDE_ENTRY;

NTSTATUS
PipInitDeviceOverrideCache(
    VOID
    )

/*++

Description:

    This routine will initialize the device override cache by loading the
    registry based device overrides into a hash table for quick lookup.

    Only the top level hardware ID sub key names are cached, improving the
    performance of the initial phase of device override lookup.

Arguments:

    None.

Return Value:

    NT Status code.

--*/

{
    NTSTATUS       status;
    UNICODE_STRING unicodeString;
    HANDLE         deviceOverridesRootHandle = NULL;
    ULONG          keyIndex;
    PKEY_BASIC_INFORMATION keyBasicInfo;
    KEY_FULL_INFORMATION   keyFullInfo;
    UCHAR          keyBuffer[sizeof(KEY_BASIC_INFORMATION) + REG_MAX_KEY_NAME_LENGTH];
    ULONG          keyLength;
    PPNP_DEVICE_OVERRIDE_ENTRY overrideEntry = NULL;
    ULONG          hashValue;
    ULONG          i;

    //
    // Device override cache should only be initialized once.
    //
    PNP_ASSERT(PnpDeviceOverrideHashList == NULL);

    //
    // Open root registry key handle to device overrides.
    //
    PiWstrToUnicodeString(&unicodeString,
                          CM_REGISTRY_MACHINE(REGSTR_PATH_DEVICE_OVERRIDES));

    status = IopOpenRegistryKeyEx(&deviceOverridesRootHandle,
                                  NULL,
                                  &unicodeString,
                                  KEY_READ);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

        //
        // No need to cache anything if there are no device overrides
        //

        status = STATUS_SUCCESS;
        goto Cleanup;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Query device overrides key for full information.
    //
    status = ZwQueryKey(deviceOverridesRootHandle,
                        KeyFullInformation,
                        &keyFullInfo,
                        sizeof(keyFullInfo),
                        &keyLength);

    if (status == STATUS_BUFFER_OVERFLOW) {
        //
        // Buffer overflow can be safely ignored because only the fixed
        // portions of the key info structure are required.
        //
        status = STATUS_SUCCESS;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // No need to cache anything if there are no device overrides.
    //
    if (keyFullInfo.SubKeys == 0) {
        status = STATUS_SUCCESS;
        goto Cleanup;
    }

    //
    // Select number of hash buckets based on number of device overrides that
    // need to be stored.
    //
    for (i = 0; i < RTL_NUMBER_OF(PnpDeviceOverrideHashListBucketSizes); i++) {
        if (keyFullInfo.SubKeys < (PnpDeviceOverrideHashListBucketSizes[i] *
                                   PNP_DEVICE_OVERRIDE_HASH_BUCKET_MAX_SIZE)) {
            PnpDeviceOverrideHashListSize = PnpDeviceOverrideHashListBucketSizes[i];
            break;
        }
    }

    if (PnpDeviceOverrideHashListSize == 0) {
        PnpDeviceOverrideHashListSize = PnpDeviceOverrideHashListBucketSizes[RTL_NUMBER_OF(PnpDeviceOverrideHashListBucketSizes) - 1];
    }

    //
    // Allocate and initialize device override hash buckets.
    //
    PnpDeviceOverrideHashList = (PLIST_ENTRY)ExAllocatePool(PagedPool,
                                                            sizeof(LIST_ENTRY) * PnpDeviceOverrideHashListSize);

    if (PnpDeviceOverrideHashList == NULL) {
        PnpDeviceOverrideHashListSize = 0;
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    for (i = 0; i < PnpDeviceOverrideHashListSize; i++) {
        InitializeListHead(&PnpDeviceOverrideHashList[i]);
    }

    //
    // Enumerate all device override sub keys.
    //
    keyBasicInfo = (PKEY_BASIC_INFORMATION)keyBuffer;

    for (keyIndex = 0; ; keyIndex++) {
        //
        // Enumerate next sub key.
        //
        status = ZwEnumerateKey(deviceOverridesRootHandle,
                                keyIndex,
                                KeyBasicInformation,
                                keyBasicInfo,
                                sizeof(keyBuffer),
                                &keyLength);

        if (!NT_SUCCESS(status)) {
            if (status == STATUS_NO_MORE_ENTRIES) {
                //
                // No more sub keys to enumerate.
                //
                status = STATUS_SUCCESS;

            } else if (status == STATUS_BUFFER_OVERFLOW) {
                //
                // Skip any sub keys with oversized names.
                //
                continue;
            }

            break;

        } else if (keyBasicInfo->NameLength <= REG_MAX_KEY_NAME_LENGTH) {
            //
            // NULL-terminate sub key name.
            //
            keyBasicInfo->Name[keyBasicInfo->NameLength / sizeof(WCHAR)] = UNICODE_NULL;

        } else {
            //
            // Skip any sub keys with names that cannot be NULL-terminated.
            //
            continue;
        }

        //
        // Allocate a device override entry.
        //
        overrideEntry = (PPNP_DEVICE_OVERRIDE_ENTRY)ExAllocatePool(PagedPool,
                                                                   sizeof(PNP_DEVICE_OVERRIDE_ENTRY));

        if (overrideEntry == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // Create device hardware ID string from sub key name.
        //
        if (!RtlCreateUnicodeString(&(overrideEntry->HardwareId),
                                    keyBasicInfo->Name)) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // Calculate hash value for upper case hardware ID.
        //
        if (!NT_SUCCESS(RtlHashUnicodeString(&(overrideEntry->HardwareId),
                        TRUE,
                        HASH_STRING_ALGORITHM_DEFAULT,
                        &hashValue))) {
            hashValue = 0;
        }

        //
        // Insert device override entry into hash bucket list, effectively
        // hashing it by its hardware ID.
        //
        InsertTailList(&(PnpDeviceOverrideHashList[hashValue % PnpDeviceOverrideHashListSize]),
                       &(overrideEntry->ListEntry));

        //
        // Clear device override entry before cleanup.
        //
        overrideEntry = NULL;
    }

    if (!NT_SUCCESS(status)) {
        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                     "PipInitDeviceOverrideCache: "
                     "Failed to cache all device overrides (status %08lx)\n",
                     status));

        goto Cleanup;
    }

Cleanup:
    //
    // Free last device override entry on failure.
    //
    if (overrideEntry != NULL) {
        ExFreePool(overrideEntry);
    }

    //
    // Close device overrides root handle.
    //
    if (deviceOverridesRootHandle != NULL) {
        ZwClose(deviceOverridesRootHandle);
    }

    return status;
}

NTSTATUS
PipFindDeviceOverrideEntry(
    __in PCZZWSTR HardwareIds,
    __in_opt  PDEVICE_OVERRIDE_MATCH_CALLBACK DeviceOverrideMatchCallback,
    __in_opt  PVOID   DeviceOverrideMatchContext,
    __out_opt PHANDLE DeviceOverrideEntryHandle
    )

/*++

Description:

    This routine will search the device override cache for the first entry
    with a hardware ID that matches one of the provided hardware IDs.

    Uses optional callback to validate matches, skipping any entries that
    the callback invalidates.

Arguments:

    HardwareIds - Multi-sz list of hardware IDs to search.

    DeviceOverrideMatchCallback - Optional callback to validate matches.

    DeviceOverrideMatchContext - Optional callback context.

    DeviceOverrideEntryHandle - Optional pointer to return opened device
                                override entry, if a valid match was found.

Return Value:

    Returns the sucess status code if a valid device override was found.

--*/

{
    NTSTATUS        status;
    PWSTR           hardwareIds = NULL;
    ULONG           hardwareIdsLength;
    PWSTR           hardwareId;
    UNICODE_STRING  unicodeString;
    UNICODE_STRING  unicodeHardwareId;
    ULONG           hashValue;
    PLIST_ENTRY     overrideEntryList;
    PLIST_ENTRY     listEntry;
    PPNP_DEVICE_OVERRIDE_ENTRY overrideEntry;
    HANDLE          deviceOverridesRootHandle = NULL;
    HANDLE          deviceOverrideEntryHandle = NULL;

    PAGED_CODE();

    PNP_ASSERT(HardwareIds != NULL);

    //
    // No need to process empty hardware IDs.
    //
    if (*HardwareIds == UNICODE_NULL) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
        goto Cleanup;
    }

    //
    // Calculate length of hardware IDs multi-sz list.
    //
    for (hardwareId = (PWSTR)HardwareIds, hardwareIdsLength = 0;
         *hardwareId != UNICODE_NULL;
         hardwareId = (PWSTR)HardwareIds + hardwareIdsLength) {
        hardwareIdsLength += (ULONG)wcslen(hardwareId) + 1;
    }

    hardwareIdsLength += 1;

    //
    // Allocate buffer and make a copy of the hardware IDs.
    //
    hardwareIds = (PWSTR)ExAllocatePool(PagedPool,
                                        hardwareIdsLength * sizeof(WCHAR));

    if (hardwareIds == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlCopyMemory(hardwareIds,
                  HardwareIds,
                  hardwareIdsLength * sizeof(WCHAR));

    //
    // Munge hardware IDs, replacing '\' characters with '#' characters.
    //
    unicodeString.Buffer        = hardwareIds;
    unicodeString.Length        = (USHORT)hardwareIdsLength * sizeof(WCHAR);
    unicodeString.MaximumLength = unicodeString.Length;

    status = IopReplaceSeperatorWithPound(&unicodeString, &unicodeString);
    PNP_ASSERT(NT_SUCCESS(status));

    //
    // Process all hardware IDs in multi-sz list.
    //
    status = STATUS_OBJECT_NAME_NOT_FOUND;

    for (hardwareId = hardwareIds;
#pragma prefast(suppress:__WARNING_BUFFER_OVERFLOW, "This appears to be related to Esp:712 or Esp:719. From inspection, I see that this loop is following the pattern of the buffer length calculation loop earlier in this function.")
         *hardwareId != UNICODE_NULL;
         hardwareId += wcslen(hardwareId) + 1) {
        //
        // Calculate hash value for upper case hardware ID.
        //
        RtlInitUnicodeString(&unicodeHardwareId, hardwareId);

        if (!NT_SUCCESS(RtlHashUnicodeString(&unicodeHardwareId,
                        TRUE,
                        HASH_STRING_ALGORITHM_DEFAULT,
                        &hashValue))) {
            hashValue = 0;
        }

        //
        // Get hash bucket list by hash value.
        //
        overrideEntryList = &(PnpDeviceOverrideHashList[hashValue % PnpDeviceOverrideHashListSize]);

        //
        // Probe hash bucket list for a matching hardware ID.
        //
        status = STATUS_OBJECT_NAME_NOT_FOUND;

        for (listEntry = overrideEntryList->Flink;
             listEntry != overrideEntryList;
             listEntry = listEntry->Flink) {
            //
            // Check for a matching hardware ID.
            //
            overrideEntry = (PPNP_DEVICE_OVERRIDE_ENTRY)CONTAINING_RECORD(listEntry,
                                                                          PNP_DEVICE_OVERRIDE_ENTRY,
                                                                          ListEntry);

            if (RtlEqualUnicodeString(&unicodeHardwareId,
                                      &(overrideEntry->HardwareId),
                                      TRUE)) {
                //
                // Found matching device override entry.
                //
                status = STATUS_SUCCESS;
                break;
            }
        }

        //
        // Skip hardware IDs with no matching device override entries.
        //
        if (!NT_SUCCESS(status)) {
            continue;
        }

        //
        // Open root handle to device overrides on first use.
        //
        if (deviceOverridesRootHandle == NULL) {
            PiWstrToUnicodeString(&unicodeString,
                                  CM_REGISTRY_MACHINE(REGSTR_PATH_DEVICE_OVERRIDES));

            status = IopOpenRegistryKeyEx(&deviceOverridesRootHandle,
                                          NULL,
                                          &unicodeString,
                                          KEY_READ);

            if (!NT_SUCCESS(status)) {
                break;
            }
        }

        //
        // Open device override entry handle.
        //
        status = IopOpenRegistryKeyEx(&deviceOverrideEntryHandle,
                                      deviceOverridesRootHandle,
                                      &unicodeHardwareId,
                                      KEY_READ);

        if (!NT_SUCCESS(status)) {
            continue;
        }

        //
        // Validate match using callback, if provided.
        //
        if ((ARGUMENT_PRESENT(DeviceOverrideMatchCallback)) &&
            (!(DeviceOverrideMatchCallback(deviceOverrideEntryHandle,
                                           DeviceOverrideMatchContext)))) {
            //
            // Skip invalid matches.
            //
            ZwClose(deviceOverrideEntryHandle);
            status = STATUS_OBJECT_NAME_NOT_FOUND;
            continue;
        }

        //
        // Found a valid match.
        //
        break;
    }

    //
    // Return or close device override entry handle on success.
    //
    if (NT_SUCCESS(status)) {
        PNP_ASSERT(deviceOverrideEntryHandle != NULL);

        if (DeviceOverrideEntryHandle != NULL) {
            *DeviceOverrideEntryHandle = deviceOverrideEntryHandle;
        } else {
            ZwClose(deviceOverrideEntryHandle);
        }
    }

Cleanup:
    //
    // Close device overrides root handle.
    //
    if (deviceOverridesRootHandle != NULL) {
        ZwClose(deviceOverridesRootHandle);
    }

    //
    // Free hardware IDs multi-sz list.
    //
    if (hardwareIds != NULL) {
        ExFreePool(hardwareIds);
    }

    return status;
}

typedef struct _PNP_DEVICE_OVERRIDE_CONTEXT {
    UNICODE_STRING  UnicodeKeyName;
    HANDLE          LocationPathsEntryHandle;
    ULONG           LocationPathCount;
} PNP_DEVICE_OVERRIDE_CONTEXT, *PPNP_DEVICE_OVERRIDE_CONTEXT;

NTSTATUS
PiQueryRemovableDeviceOverride(
    __in      PDEVICE_NODE DeviceNode,
    __in_opt  PCWSTR       HardwareIds,
    __in_opt  PCWSTR       CompatibleIds,
    __out     PBOOLEAN     Removable
    )
/*++

Description:

    This routine will search for a removable device override for the provided
    device node.

    Device overrides can be found directly for hardware or compatible IDs of
    this device node or indirectly for hardware or compatible IDs of its
    parent device node.

Arguments:

    DeviceNode - Device node to query for a removable overrride.

    HardwareIds - Optional multi-sz list of hardware IDs for device node.

    CompatibleIds - Optional multi-sz list of compatible IDs for device node.

    Removable  - Returns removable device override value.

Return Value:

    Returns the success status code if a removable device override was found.

--*/

{
    HANDLE          deviceInstanceHandle;
    PNP_DEVICE_OVERRIDE_CONTEXT deviceOverrideContext;
    UCHAR           keyBuffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(ULONG)];
    ULONG           keyLength;
    PKEY_VALUE_PARTIAL_INFORMATION keyValueInfo;
    HANDLE          locationPathEntryHandle;
    PZZWSTR         locationPaths;
    ULONG           locationPathsLength;
    PWSTR           parentIds;
    ULONG           propertySize;
    ULONG           propertyType;
    CM_REG_PROP     registryProperties[2];
    ULONG           registryPropertiesIndex;
    ULONG           removable = 0;
    NTSTATUS        status;
    ULONG           tempSize;
    UNICODE_STRING  unicodeString;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(Removable != NULL);

    parentIds = NULL;

    //
    // Fail if the device override cache is not initialized.
    //
    if (PnpDeviceOverrideHashList == NULL) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
        goto Cleanup;
    }

    //
    // Initialize device override callback context.
    //
    RtlZeroMemory(&deviceOverrideContext,
                  sizeof(PNP_DEVICE_OVERRIDE_CONTEXT));

    //
    // Check if this device node has never been queried for overrides before.
    //
    status = STATUS_OBJECT_NAME_NOT_FOUND;

    if ((DeviceNode->OverrideFlags & DNOF_OVERRIDES_QUERIED) == 0) {
        //
        // Query device node for child location overrides while its
        // hardware and compatible IDs are available, possibly saving
        // time for children of this device node if/when they need to
        // query their parent device node for overrides later.
        //
        PiWstrToUnicodeString(&(deviceOverrideContext.UnicodeKeyName),
                              REGSTR_KEY_CHILD_LOCATION_PATHS);

        if (HardwareIds != NULL) {
            status = PipFindDeviceOverrideEntry(HardwareIds,
                                                PipCallbackHasDeviceOverrides,
                                                &deviceOverrideContext,
                                                NULL);
        }

        if ((!NT_SUCCESS(status)) &&
            (CompatibleIds != NULL)) {
            status = PipFindDeviceOverrideEntry(CompatibleIds,
                                                PipCallbackHasDeviceOverrides,
                                                &deviceOverrideContext,
                                                NULL);
        }

        if (NT_SUCCESS(status)) {
            //
            // No need to keep handle open to child location paths sub key.
            //
            PNP_ASSERT(deviceOverrideContext.LocationPathsEntryHandle != NULL);
            ZwClose(deviceOverrideContext.LocationPathsEntryHandle);

            //
            // Device node has a child location path override.
            //
            DeviceNode->OverrideFlags |= DNOF_HAS_CHILD_LOCATION_OVERRIDES;
        }

        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    //
    // Check if this device node needs to be queried for device overrides,
    // either because it was never queried before or it was queried and is
    // known to have a location path override.
    //
    if (((DeviceNode->OverrideFlags & DNOF_OVERRIDES_QUERIED) == 0) ||
        ((DeviceNode->OverrideFlags & DNOF_HAS_LOCATION_OVERRIDES) != 0)) {
        //
        // Find a direct location path device override entry for this device
        // node, searching hardware IDs followed by compatible IDs.
        //
        // Use callback to filter out device override entries without any
        // location path overrides.
        //
        PiWstrToUnicodeString(&(deviceOverrideContext.UnicodeKeyName),
                              REGSTR_KEY_LOCATION_PATHS);

        //
        // Search device node hardware IDs, if any were provided.
        //
        if (HardwareIds != NULL) {
            status = PipFindDeviceOverrideEntry(HardwareIds,
                                                PipCallbackHasDeviceOverrides,
                                                &deviceOverrideContext,
                                                NULL);
        }

        //
        // Search device node compatible IDs, if any were provided.
        //
        if ((!NT_SUCCESS(status)) &&
            (CompatibleIds != NULL)) {
            status = PipFindDeviceOverrideEntry(CompatibleIds,
                                                PipCallbackHasDeviceOverrides,
                                                &deviceOverrideContext,
                                                NULL);
        }

        if (NT_SUCCESS(status)) {
            //
            // Device node has a location path override.
            //
            DeviceNode->OverrideFlags |= DNOF_HAS_LOCATION_OVERRIDES;
        }
    }

    //
    // Device node has now been fully queried for overrides.
    //
    DeviceNode->OverrideFlags |= DNOF_OVERRIDES_QUERIED;

    //
    // When no direct override is found, try to find an indirect override
    // under the parent of this device node. Only attempt to do this when no
    // overrides have been previously queried for the parent device node or
    // they have been queried and are known to exist.
    //
    if ((!NT_SUCCESS(status)) &&
        (((DeviceNode->Parent->OverrideFlags & DNOF_OVERRIDES_QUERIED) == 0) ||
         ((DeviceNode->Parent->OverrideFlags & DNOF_HAS_CHILD_LOCATION_OVERRIDES) != 0))) {
        //
        // Find an indirect child location path device override entry for
        // this device node, searching hardware IDs followed by compatible IDs.
        //
        // Use callback to filter out device override entries without any
        // child location overrides.
        //
        PiWstrToUnicodeString(&(deviceOverrideContext.UnicodeKeyName),
                              REGSTR_KEY_CHILD_LOCATION_PATHS);

        //
        // Open parent device node instance key.
        //
        status = PnpDeviceObjectToDeviceInstance(DeviceNode->Parent->PhysicalDeviceObject,
                                                 &deviceInstanceHandle,
                                                 KEY_READ);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        registryProperties[0] = CM_REG_PROP_HARDWAREID;
        registryProperties[1] = CM_REG_PROP_COMPATIBLEIDS;

        //
        // Start with an intial guess of how large the IDs might need to be
        //
        propertySize = 256;

        for (registryPropertiesIndex = 0;
             registryPropertiesIndex < RTL_NUMBER_OF(registryProperties);
             registryPropertiesIndex++) {
            //
            // Get next hardware IDs value for parent device node.
            //

            status = STATUS_SUCCESS;

            if (parentIds == NULL) {
                parentIds = ExAllocatePool(PagedPool, propertySize);

                if (parentIds == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                }
            }

            //
            // Keep track of the current size of the buffer so it can be
            // restored later.
            //
            tempSize = propertySize;

            if (NT_SUCCESS(status)) {
                status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                             PnpGetDeviceInstancePath(DeviceNode->Parent)->Buffer,
                                             deviceInstanceHandle,
                                             registryProperties[registryPropertiesIndex],
                                             &propertyType,
                                             (PUCHAR)parentIds,
                                             &propertySize,
                                             0);

                if (status == STATUS_BUFFER_TOO_SMALL) {
                    //
                    // Allocate a buffer that is large enough to get the property.
                    //

                    PNP_ASSERT(parentIds != NULL);
                    ExFreePool(parentIds);

                    parentIds = ExAllocatePool(PagedPool, propertySize);

                    if (parentIds == NULL) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                    }

                    //
                    // Keep track of the current size of the buffer so it can be
                    // restored later.
                    //
                    tempSize = propertySize;

                    //
                    // Try to retrieve the property again.
                    //

                    if (NT_SUCCESS(status)) {
                        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode->Parent)->Buffer,
                                                     deviceInstanceHandle,
                                                     registryProperties[registryPropertiesIndex],
                                                     &propertyType,
                                                     (PUCHAR)parentIds,
                                                     &propertySize,
                                                     0);

                        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
                    }
                }

                if (NT_SUCCESS(status)) {
                    //
                    // Search parent device node hardware IDs.
                    //
                    if (propertyType == REG_MULTI_SZ) {
                        status = PipFindDeviceOverrideEntry(parentIds,
                                                            PipCallbackHasDeviceOverrides,
                                                            &deviceOverrideContext,
                                                            NULL);

                    } else {
                        status = STATUS_UNSUCCESSFUL;
                    }
                }
            }

            //
            // Reset propertySize to correctly reflect the current size of the
            // buffer.
            //
            propertySize = tempSize;

            //
            // Stop if a device override was found.
            //
            if (NT_SUCCESS(status)) {
                break;
            }
        }

        //
        // Clean up the buffer used to retrieve the IDs
        //
        if (parentIds != NULL) {
            ExFreePool(parentIds);
            parentIds = NULL;
        }

        //
        // Close parent device instance handle.
        //
        ZwClose(deviceInstanceHandle);

        //
        // Some root/legacy device nodes are not enumerated so they are never
        // directly queried for device overrides. These device nodes can be
        // safely flagged as having been queried.
        //
        if ((DeviceNode->Parent->OverrideFlags & DNOF_OVERRIDES_QUERIED) == 0) {
            if (NT_SUCCESS(status)) {
                DeviceNode->Parent->OverrideFlags |= DNOF_HAS_CHILD_LOCATION_OVERRIDES;
            }

            DeviceNode->Parent->OverrideFlags |= DNOF_OVERRIDES_QUERIED;
        }
    }

    //
    // Nothing more to do if a device override entry was not found.
    //
    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // A device override entry was found for a hardware or compatible ID of
    // this device node, now find a matching location path sub key.
    //
    PNP_ASSERT(deviceOverrideContext.LocationPathsEntryHandle != NULL);
    PNP_ASSERT(deviceOverrideContext.LocationPathCount > 0);

    locationPathEntryHandle = NULL;

    status = STATUS_OBJECT_NAME_NOT_FOUND;

    //
    // When there is only one location path sub key, check if it is for the
    // default location path in an effort to potentially avoid querying the
    // device node for its location paths.
    //
    if (deviceOverrideContext.LocationPathCount == 1) {
        //
        // Open default location path key handle.
        //
        PiWstrToUnicodeString(&unicodeString,
                              REGSTR_KEY_DEFAULT_LOCATION_PATH);

        status = IopOpenRegistryKeyEx(&locationPathEntryHandle,
                                      deviceOverrideContext.LocationPathsEntryHandle,
                                      &unicodeString,
                                      KEY_READ);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_TRACE_LEVEL,
                         "PiQueryRemovableDeviceOverride: "
                         "Found default location path override for %wZ\n",
                         PnpGetDeviceInstancePath(DeviceNode)));
        }
    }

    //
    // When there are multiple location path sub keys or the single sub key
    // is not the default location path, the device node must be queried for
    // its location paths.
    //
    if (!NT_SUCCESS(status)) {
        //
        // Query device node interface for location path strings.
        //
        status = PnpGetDeviceLocationStrings(DeviceNode,
                                             &locationPaths,
                                             &locationPathsLength);

        if (NT_SUCCESS(status)) {
            //
            // Open first matching location path sub key.
            //
            status = PnpOpenFirstMatchingSubKey(locationPaths,
                                                deviceOverrideContext.LocationPathsEntryHandle,
                                                KEY_READ,
                                                NULL,
                                                &locationPathEntryHandle,
                                                NULL);

            if (NT_SUCCESS(status)) {
                IopDbgPrint((IOP_TRACE_LEVEL,
                             "PiQueryRemovableDeviceOverride: "
                             "Found location path (%ws) override for %wZ\n",
                             locationPaths,
                             PnpGetDeviceInstancePath(DeviceNode)));
            }

            //
            // Free location paths string.
            //
            ExFreePool(locationPaths);
        }
    }

    //
    // Try to open default location path sub key if it was not already opened
    // and there were no specific location path matches.
    //
    if ((!NT_SUCCESS(status)) &&
        (deviceOverrideContext.LocationPathCount > 1)) {
        //
        // Open default location path key handle.
        //
        PiWstrToUnicodeString(&unicodeString,
                              REGSTR_KEY_DEFAULT_LOCATION_PATH);

        status = IopOpenRegistryKeyEx(&locationPathEntryHandle,
                                      deviceOverrideContext.LocationPathsEntryHandle,
                                      &unicodeString,
                                      KEY_READ);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_TRACE_LEVEL,
                         "PiQueryRemovableDeviceOverride: "
                         "Found default location path override for %wZ\n",
                         PnpGetDeviceInstancePath(DeviceNode)));
        }
    }

    //
    // Close location paths sub key now that it is not needed.
    //
    ZwClose(deviceOverrideContext.LocationPathsEntryHandle);

    //
    // Nothing more to do if a location path entry was not found.
    //
    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    PNP_ASSERT(locationPathEntryHandle != NULL);

    //
    // Query removable value from location path entry.
    //
    PiWstrToUnicodeString(&unicodeString, REGSTR_VALUE_REMOVABLE);

    status = ZwQueryValueKey(locationPathEntryHandle,
                             &unicodeString,
                             KeyValuePartialInformation,
                             keyBuffer,
                             sizeof(keyBuffer),
                             &keyLength);

    if (NT_SUCCESS(status)) {
        //
        // Validate removable value.
        //
        keyValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION)keyBuffer;

        if ((keyValueInfo->Type == REG_DWORD) &&
            (keyValueInfo->DataLength == sizeof(ULONG))) {
            removable = *((PULONG)keyValueInfo->Data);
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
    }

    //
    // Close location path entry handle.
    //
    ZwClose(locationPathEntryHandle);

    //
    // Return removable value as boolean.
    //
    *Removable = (removable != 0);

Cleanup:
    return status;

} // PiQueryRemovableDeviceOverride

BOOLEAN
PipCallbackHasDeviceOverrides(
    __in     HANDLE DeviceOverrideEntryHandle,
    __in_opt PVOID  Context
    )

/*++

Routine Description:

    This routine is a callback routine that checks if the specified device
    override entry has any location path sub keys containing overrides.

Arguments:

    DeviceOverrideEntryHandle - Handle to device override entry key to check.

    Context - Callback context containing location paths key name as input and
              location path key handle and sub key count as output.

Return Value:

    Returns TRUE if the device override entry key is valid, FALSE otherwise.

--*/

{
    NTSTATUS    status;
    PPNP_DEVICE_OVERRIDE_CONTEXT deviceOverrideContext;
    HANDLE      locationPathsEntryHandle;
    KEY_FULL_INFORMATION keyFullInfo;
    ULONG       keyLength;

    PAGED_CODE();

    PNP_ASSERT(DeviceOverrideEntryHandle != NULL);
    PNP_ASSERT(Context != NULL);

    deviceOverrideContext = (PPNP_DEVICE_OVERRIDE_CONTEXT)Context;

    //
    // Open location paths key handle.
    //
    status = IopOpenRegistryKeyEx(&locationPathsEntryHandle,
                                  DeviceOverrideEntryHandle,
                                  &(deviceOverrideContext->UnicodeKeyName),
                                  KEY_READ);

    if (NT_SUCCESS(status)) {
        //
        // Query location paths key for full information.
        //
        status = ZwQueryKey(locationPathsEntryHandle,
                            KeyFullInformation,
                            &keyFullInfo,
                            sizeof(keyFullInfo),
                            &keyLength);

        if (status == STATUS_BUFFER_OVERFLOW) {
            //
            // Buffer overflow can be safely ignored because only the fixed
            // portions of the key info structure are required.
            //
            status = STATUS_SUCCESS;
        }

        //
        // Check if the location paths key has any device override sub keys.
        //
        if ((NT_SUCCESS(status)) &&
            (keyFullInfo.SubKeys > 0)) {
            //
            // Leave the location paths key handle open, passing it back
            // through the caller context for further use.
            //
            deviceOverrideContext->LocationPathsEntryHandle = locationPathsEntryHandle;
            deviceOverrideContext->LocationPathCount        = keyFullInfo.SubKeys;

        } else {
            //
            // No device overrides found under location paths key.
            //
            status = STATUS_UNSUCCESSFUL;

            ZwClose(locationPathsEntryHandle);
        }
    }


    return NT_SUCCESS(status);

} // PipCallbackHasDeviceOverrides

NTSTATUS
PnpSerializeBoot (
    VOID
    )

/*++

Routine Description:

    This routine is called to serialize pnp system device enumeration with
    the rest of the system boot process.  This routine will block until
    enumeration is complete.

    N.B. If this call is made within a pnp notification callback, a deadlock
    may occur.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if the routine successfully blocked until the completion
    of system device enumeration.

--*/

{

    NTSTATUS Status;

    Status = STATUS_UNSUCCESSFUL;

    //
    // Only serialize boot when boot devices have been initialized and started.
    // Doing so earlier would be before async system enum has started and could
    // lead to a deadlock.
    //

    if (PnPBootDriversInitialized != FALSE) {
        Status = KeWaitForSingleObject(&PnpSystemDeviceEnumerationComplete,
                                       Executive,
                                       KernelMode,
                                       FALSE,
                                       NULL);

    }

    PNP_ASSERT(Status == STATUS_SUCCESS);

    return Status;
}

NTSTATUS
PipHardwareConfigInit(
    __in LPGUID HardwareConfigGuid
    )

/*++

Routine Description:

    This routine initializes the hardware configuration for the current
    boot session by opening its existing hardware configuration instance or
    creating a new one if the system has not been seen before.

Arguments:

    HardwareConfigGuid - Hardware configuration ID guid that uniquely describes
        a given system and its hardware configuration.

Return Value:

    NTSTATUS code.

--*/

{
    LARGE_INTEGER bootTime;
    ULONG64 bootTimeBias;
    ULONG disposition;
    ULONG HardwareConfigIndex = (ULONG)(-1);
    HANDLE hCurrent = NULL;
    HANDLE hHardwareConfig = NULL;
    HANDLE hHardwareConfigRoot = NULL;
    HANDLE hSysprep = NULL;
    PKEY_VALUE_FULL_INFORMATION information;
    WCHAR keyPathBuffer[MAX_PATH];
    BOOLEAN performReset = FALSE;
    ULONG resetConfig;
    NTSTATUS status;
    LARGE_INTEGER systemTime;
    ULONG TempUlong;
    NTSTATUS tempStatus;
    BOOLEAN triggerRespecialize;
    UNICODE_STRING unicodeName;
    UNICODE_STRING unicodeString;

    //
    // First try to create/open the sysprep key.  Failure to do so shouldn't cause
    // the system to not boot, so don't return a failure from this function.
    //

    PiWstrToUnicodeString(&unicodeName, CM_REGISTRY_MACHINE(REGSTR_PATH_SYSTEM_SETUP));
    status = IopCreateRegistryKeyEx(&hSysprep,
                                    NULL,
                                    &unicodeName,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_NON_VOLATILE,
                                    NULL);

    if (!NT_SUCCESS(status)) {
        status = STATUS_SUCCESS;
        hSysprep = NULL;
    }

    //
    // Delete the value that triggers sysprep to run the respecialize phase.
    // This value might not exist.  Ignore any errors.
    //

    if (hSysprep != NULL) {
        PiWstrToUnicodeString(&unicodeName, REGSTR_VAL_RESPECIALIZE);
        ZwDeleteValueKey(hSysprep, &unicodeName);
    }

    //
    // Open/create System\HardwareConfig key.
    //

    PiWstrToUnicodeString(&unicodeName, CM_REGISTRY_MACHINE(REGSTR_PATH_HARDWARECONFIG));
    status = IopCreateRegistryKeyEx(&hHardwareConfigRoot,
                                    NULL,
                                    &unicodeName,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_NON_VOLATILE,
                                    NULL);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Convert the hardware configuration id from a GUID to a string
    //

    status = RtlStringFromGUID(HardwareConfigGuid, &PnpCurrentHardwareConfigurationGuidString);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Stamp this hardware configuration guid string as the last configuration
    // this machine booted on.
    //

    PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_HARDWARECONFIG_LASTCONFIG);

    PNP_ASSERT(PnpCurrentHardwareConfigurationGuidString.MaximumLength >= PnpCurrentHardwareConfigurationGuidString.Length + sizeof(UNICODE_NULL));

    status = ZwSetValueKey(hHardwareConfigRoot,
                           &unicodeString,
                           TITLE_INDEX_VALUE,
                           REG_SZ,
                           PnpCurrentHardwareConfigurationGuidString.Buffer,
                           PnpCurrentHardwareConfigurationGuidString.Length+sizeof(UNICODE_NULL));

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Next open the current hardware configuration, creating it if it doesn't exist
    //

    status = IopCreateRegistryKeyEx(&hHardwareConfig,
                                    hHardwareConfigRoot,
                                    &PnpCurrentHardwareConfigurationGuidString,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_NON_VOLATILE,
                                    &disposition);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Make sure this configuration has an Id value.  It should already have one unless
    // this is a new configuration, but it is possible that someone deleted it and this
    // should be as resilient as possible to modifications like that, so always check
    // if an Id exists instead of just using the disposition of creating the hardware
    // config key above.
    //
    status = IopGetRegistryValue(hHardwareConfig,
                                 REGSTR_VAL_HARDWARECONFIG_ID,
                                 0,
                                 &information);

    if (NT_SUCCESS(status)) {
        if ((information->Type == REG_DWORD) &&
            (information->DataLength == sizeof(ULONG))) {
            HardwareConfigIndex = *((PULONG)KEY_VALUE_DATA(information));
        } else {
            //
            // If the value could be retrieved but isn't of the right type or size
            // then something has corrupted the value, so assert
            //
            PNP_ASSERT(0);

            status = STATUS_UNSUCCESSFUL;

            //
            // Don't go to the cleanup here.  By setting status to an error, this will
            // fall through into the code below that will give this configuration a new
            // id.
            //
        }

        ExFreePool(information);
    }

    if (!NT_SUCCESS(status)) {
        //
        // A hardware configuration id could not be retrieved from this hardware
        // config.  Give this configuration an id.
        //

        //
        // There should only not be a hardware configuration id if this is a new
        // configuration.  If that is not the case, then either something deleted
        // the hardware configuration id or something corrupted it, so assert that
        // this is a newly created configuration.
        //
        PNP_ASSERT(disposition == REG_CREATED_NEW_KEY);

        //
        // Get the last hardware configuration id from the root
        //
        HardwareConfigIndex = 0;
        status = IopGetRegistryValue(hHardwareConfigRoot,
                                     REGSTR_VAL_HARDWARECONFIG_LASTID,
                                     0,
                                     &information);

        if (NT_SUCCESS(status)) {
            if ((information->Type == REG_DWORD) &&
                (information->DataLength == sizeof(ULONG))) {
                HardwareConfigIndex = *((PULONG)KEY_VALUE_DATA(information));

                //
                // Increment the hardware configuration id to get the value to use
                // as the id for this hardware configuration
                //
                HardwareConfigIndex++;
            } else {
                //
                // If the value could be retrieved but isn't of the right type or size
                // then something has corrupted the value, so assert
                //
                PNP_ASSERT(0);

                //
                // Don't go to the cleanup here.  This will be treated the same as if
                // the last id value could not be retrieved and this configuration
                // will be given an id of 0.
                //
            }

            ExFreePool(information);
        }

        //
        // Make sure this ID isn't already in use.  If it is, move on to the next id.  Make
        // sure that the ID doesn't wrap around back past where it started so an infinite
        // loop doesn't happen.
        //
        TempUlong = HardwareConfigIndex - 1;
        while ((HardwareConfigIndex != TempUlong) &&
               (PipHardwareConfigExists(hHardwareConfigRoot, HardwareConfigIndex))) {
            HardwareConfigIndex++;
        }

        //
        // Check to see if this wrapped around back to where it started
        //
        if (HardwareConfigIndex == TempUlong) {
            status = STATUS_UNSUCCESSFUL;
            goto Cleanup;
        }

        //
        // Update the last id value for the root key of the hardware configurations
        //
        PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_HARDWARECONFIG_LASTID);

        //
        // The return value of this set is not checked since failure to set it does not
        // need to stop boot from happening.  The next time a hardware configuration is
        // created, this will be updated properly.  Since we check to make sure the new
        // id is free before assigning it to a hardware configuration, this value does
        // not need to be 100% correct
        //
        ZwSetValueKey(hHardwareConfigRoot,
                      &unicodeString,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &HardwareConfigIndex,
                      sizeof(HardwareConfigIndex));

        //
        // Store this hardware configuration id in the current hardware configuration
        //
        PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_HARDWARECONFIG_ID);

        status = ZwSetValueKey(hHardwareConfig,
                               &unicodeString,
                               TITLE_INDEX_VALUE,
                               REG_DWORD,
                               &HardwareConfigIndex,
                               sizeof(HardwareConfigIndex));

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    //
    // Save away the index of this hardware configuration for use during device
    // enumeration.
    //
    PnpCurrentHardwareConfigurationIndex = HardwareConfigIndex;

    //
    // Record the timestamp for the last time this hardware configuration was
    // used to boot.  Adjust the boot time by its bias to get the original boot
    // time.
    //
    KeQueryBootTimeValues(&systemTime, &bootTime, &bootTimeBias);
    bootTime.QuadPart -= bootTimeBias;
    PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_HARDWARECONFIG_LASTUSE);

    //
    // The return value of the set is ignored since the last used time stamp is
    // not an essential property and failure to set it should not stop the boot
    // from happening
    //
    ZwSetValueKey(hHardwareConfig,
                  &unicodeString,
                  TITLE_INDEX_VALUE,
                  REG_BINARY,
                  &bootTime,
                  sizeof(bootTime));

    //
    // Create the current symlink to this hardware configuration
    //
    PiWstrToUnicodeString(&unicodeString, REGSTR_KEY_CURRENT);
    status = IopCreateRegistryKeyEx(&hCurrent,
                                    hHardwareConfigRoot,
                                    &unicodeString,
                                    KEY_ALL_ACCESS | KEY_CREATE_LINK,
                                    REG_OPTION_VOLATILE | REG_OPTION_CREATE_LINK,
                                    NULL);

    if (!NT_SUCCESS(status)) {
        //
        // There shouldn't be a problem creating the current symlink so if it
        // cannot be created successfully, assert
        //
        PNP_ASSERT(0);

        goto Cleanup;
    }

    unicodeString.Buffer        = keyPathBuffer;
    unicodeString.MaximumLength = sizeof(keyPathBuffer);
    unicodeString.Length        = 0;

    status = RtlUnicodeStringPrintf(&unicodeString,
                                    CM_REGISTRY_MACHINE(REGSTR_PATH_HARDWARECONFIG)
                                    L"\\%wZ",
                                    &PnpCurrentHardwareConfigurationGuidString);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Set the symlink to point at the current hardware configuration
    //
    status = ZwSetValueKey(hCurrent,
                           &CmSymbolicLinkValueName,
                           0,
                           REG_LINK,
                           unicodeString.Buffer,
                           unicodeString.Length);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    if (InitSafeBootMode) {
        //
        // If booting in safe mode, reset the hardware configuration.  If the
        // system is in safe mode to try to recover from a problem, doing this
        // may help the recovery.
        //
        performReset = TRUE;
    } else {
        //
        // Check to see if something flagged this hardware configuration as
        // needing to be reset.
        //
        tempStatus = IopGetRegistryValue(hHardwareConfig,
                                         REGSTR_VAL_HARDWARECONFIG_RESET,
                                         0,
                                         &information);

        if (NT_SUCCESS(tempStatus)) {
            if ((information->Type == REG_DWORD) &&
                (information->DataLength == sizeof(ULONG))) {
                resetConfig = *((PULONG)KEY_VALUE_DATA(information));
            } else {
                resetConfig = 0;
            }

            ExFreePool(information);
        } else {
            resetConfig = 0;
        }

        if (resetConfig != 0) {
            performReset = TRUE;
        }
    }

    if (performReset) {
        //
        // Clear the start overrides for this hardware configuration from all
        // services.  For any driver that is loaded this boot that isn't
        // actually needed, its start override will be regenerated again during
        // IopInitializeBootDrivers. Failure to remove the overlay shouldn't be
        // considered a fatal error for booting.
        //
        tempStatus = PipHardwareConfigClearStartOverrides(HardwareConfigIndex);

        if (NT_SUCCESS(tempStatus)) {
            PiWstrToUnicodeString(&unicodeName, REGSTR_VAL_HARDWARECONFIG_RESET);
            ZwDeleteValueKey(hHardwareConfig, &unicodeName);
        }
    }

    //
    // Determine if the sysprep respecialize phase needs to be run
    //
    if (hSysprep != NULL) {
        //
        // If the key for the current hardware configuration was just created then
        // respecialize should be triggered.
        //
        triggerRespecialize = (disposition == REG_CREATED_NEW_KEY);

        if (!triggerRespecialize) {
            //
            // If the current hardware configuration was not just created then
            // check to see if respecialize did not complete successfully last
            // time it was run.
            //
            tempStatus = IopGetRegistryValue(hSysprep,
                                             REGSTR_VAL_RESPECIALIZE_STARTED,
                                             0,
                                             &information);

            if (NT_SUCCESS(tempStatus)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG))) {
                    triggerRespecialize = (*((PULONG)KEY_VALUE_DATA(information)) == 1);
                } else {
                    //
                    // Data was malformed.  Assert here to catch corruption of the registry,
                    // but otherwise do nothing.
                    //
                    PNP_ASSERT(0);

                    NOTHING;
                }

                ExFreePool(information);
            }
        }

        if (!triggerRespecialize) {
            //
            // Check to see if this hardware configuration is marked as needing
            // a respecialize.
            //
            tempStatus = IopGetRegistryValue(hHardwareConfig,
                                             REGSTR_VAL_HARDWARECONFIG_RESPECIALIZE,
                                             0,
                                             &information);

            if (NT_SUCCESS(tempStatus)) {
                if ((information->Type == REG_DWORD) &&
                    (information->DataLength == sizeof(ULONG))) {
                    triggerRespecialize = (*((PULONG)KEY_VALUE_DATA(information)) == 1);
                } else {
                    //
                    // Data was malformed.  Assert here to catch corruption of the registry,
                    // but otherwise do nothing.
                    //
                    PNP_ASSERT(0);

                    NOTHING;
                }

                ExFreePool(information);
            }
        }

        if (triggerRespecialize) {
            //
            // Respecialize should be triggered. Don't propagate the return code
            // since failure to trigger respecialize shouldn't stop the user
            // from booting.
            //
            tempStatus = PipHardwareConfigTriggerRespecialize(hSysprep);

            //
            // If the value telling sysprep to run Respecialize was successfully
            // set, clear the respecialize marker for the current hardware
            // configuration.
            //
            if (NT_SUCCESS(tempStatus)) {
                PiWstrToUnicodeString(&unicodeName, REGSTR_VAL_HARDWARECONFIG_RESPECIALIZE);
                ZwDeleteValueKey(hHardwareConfig, &unicodeName);
            }
        }
    }

  Cleanup:
    if (hHardwareConfigRoot) {
        ZwClose(hHardwareConfigRoot);
    }

    if (hHardwareConfig) {
        ZwClose(hHardwareConfig);
    }

    if (hCurrent) {
        ZwClose(hCurrent);
    }

    if (hSysprep) {
        ZwClose(hSysprep);
    }

    return status;
}

BOOLEAN
PipHardwareConfigExists(
    __in HANDLE hHardwareConfigRoot,
    __in ULONG  HardwareConfigIndex
    )

/*++

Routine Description:

    This routine checks if the specified hardware configuration
    ID is already in use.

Arguments:

    hHardwareConfigRoot - Supplies a handle to the root of the hardware
        configuration key

    HardwareConfigIndex - Supplies an index that is compared against the existing
        hardware configurations to see if the index is already in use.

Return Value:

    TRUE if a hardware configuration is already using this ID, FALSE otherwise

--*/

{
    BOOLEAN Exists = FALSE;
    HANDLE hHardwareConfig = NULL;
    ULONG Index = 0;
    ULONG KeyIndex = 0;
    WCHAR KeyName[MAX_PATH];
    ULONG KeyNameLength;
    ULONG RegType = 0;
    ULONG Size;
    NTSTATUS Status;

    while (!Exists) {

        PNP_ASSERT(hHardwareConfig == NULL);

        KeyNameLength = MAX_PATH;
        Status = _PnpCtxRegEnumKey(NULL,
                                   hHardwareConfigRoot,
                                   KeyIndex,
                                   KeyName,
                                   &KeyNameLength);

        KeyIndex++;

        if (!NT_SUCCESS(Status)) {
            break;
        }

        Status = _PnpCtxRegOpenKey(NULL,
                                   hHardwareConfigRoot,
                                   KeyName,
                                   0,
                                   KEY_QUERY_VALUE,
                                   &hHardwareConfig);

        if (!NT_SUCCESS(Status)) {
            //
            // If there is a problem opening this key, just move on to the next
            //
            continue;
        }

        Size = sizeof(Index);
        Status = _PnpCtxRegQueryValue(NULL,
                                      hHardwareConfig,
                                      REGSTR_VAL_HARDWARECONFIG_ID,
                                      &RegType,
                                      (PVOID)&Index,
                                      &Size);

        if ((NT_SUCCESS(Status)) &&
            (RegType == REG_DWORD) &&
            (Size == sizeof(Index)) &&
            (Index == HardwareConfigIndex)) {
            Exists = TRUE;
        }

        PNP_ASSERT(hHardwareConfig != NULL);

        ZwClose(hHardwareConfig);
        hHardwareConfig = NULL;
    }

    return Exists;
}

NTSTATUS
PipHardwareConfigActivateService(
    __in PCWSTR ServiceName
    )

/*++

Routine Description:

    This routine removes start type overrides for the specified service.

Arguments:

    ServiceName - Supplies a string representing the name of the service to
        remove start type overrides for.


Return Value:

    STATUS_SUCCESS if the service successfully had any start overrides removed.
    An NTSTATUS error code otherwise.

--*/
{
    HANDLE serviceKey;
    NTSTATUS status;
    UNICODE_STRING unicodeString;

    PAGED_CODE();

    serviceKey = NULL;

    PNP_ASSERT(PiPnpRtlCtx != NULL);

    if (ServiceName == NULL) {
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    //
    // Open service key.
    //

    RtlInitUnicodeString(&unicodeString, ServiceName);

    status = PipOpenServiceEnumKeys(&unicodeString,
                                    DELETE,
                                    &serviceKey,
                                    NULL,
                                    FALSE);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Delete all start type overrides for this service.
    //

    status = _PnpCtxRegDeleteTree(PiPnpRtlCtx,
                                  serviceKey,
                                  REGSTR_KEY_START_OVERRIDE);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

        //
        // The key not existing is not an error.  It means that there were no
        // start type overrides to delete which is fine.
        //

        status = STATUS_SUCCESS;
        goto Cleanup;
    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

  Cleanup:

    if (serviceKey != NULL) {
        ZwClose(serviceKey);
    }

    return status;
}

NTSTATUS
PipHardwareConfigClearStartOverrides(
    __in ULONG HardwareConfigIndex
    )

/*++

Routine Description:

    This routine removes all start overrides for a certain hardware configuration.

Arguments:

    HardwareConfigIndex - Supplies the number of the hardware
        configuration to remove all overrides for.


Return Value:

    STATUS_SUCCESS if all overrides were removed for this hardware configuration.
    An NTSTATUS error code otherwise.

--*/
{
    WCHAR hardwareConfigIndexString[MAX_HARDWARE_CONFIG_NUMBER_LENGTH];
    HANDLE servicesRootKey;
    NTSTATUS status;

    PAGED_CODE();

    status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                            PNPCTX_BASE_KEY_TYPE_SERVICES,
                                            &servicesRootKey);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    status = RtlStringCchPrintfExW(hardwareConfigIndexString,
                                   MAX_HARDWARE_CONFIG_NUMBER_LENGTH,
                                   NULL,
                                   NULL,
                                   STRSAFE_NULL_ON_FAILURE,
                                   START_OVERRIDE_ENTRY_NAME_FORMAT,
                                   HardwareConfigIndex);

    if (!NT_SUCCESS(status)) {
        PNP_ASSERT(status != STATUS_BUFFER_OVERFLOW);
        goto Cleanup;
    }

    status = _PnpCtxRegEnumKeyWithCallback(PiPnpRtlCtx,
                                           servicesRootKey,
                                           PipHardwareConfigClearStartOverrideCallback,
                                           (PVOID)hardwareConfigIndexString);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

  Cleanup:

    //
    // DO NOT CLOSE servicesRootKey.  It is a pnprtl cached key and MUST NOT be
    // closed.
    //

    return status;
}

NTSTATUS
PipHardwareConfigOpenKey(
    __in_opt LPGUID HardwareConfigGuid,
    __in ACCESS_MASK DesiredAccess,
    __out PHANDLE HardwareConfigKey
    )

/*++

Routine Description:

    This routine retrieves the registry key of the specified hardware
    configuration. If no hardware configuration is specified, it retrieves the
    registry key of the current hardware configuration.

Arguments:

    HardwareConfigGuid - Optionally supplies a pointer to a GUID that
        represents the hardware configuration to retrieve the number for.  If
        this is NULL, the current hardware configuration will be used.

    HardwareConfigKey - Returns the registry key of the specified hardware
        configuration.


Return Value:

    STATUS_SUCCESS if the registry key of the specified hardware configuration
    was successfully retrieved.  An NTSTATUS error code otherwise.

--*/
{
    WCHAR GuidString[GUID_STRING_LEN];
    WCHAR HardwareConfigPath[MAX_PATH];
    HANDLE hHardwareConfig;
    NTSTATUS Status;

    PAGED_CODE();

    hHardwareConfig = NULL;

    if (HardwareConfigKey == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    Status = RtlStringCchCopyExW(HardwareConfigPath,
                                 MAX_PATH,
                                 REGSTR_PATH_HARDWARECONFIG
                                 TEXT("\\"),
                                 NULL,
                                 NULL,
                                 STRSAFE_NULL_ON_FAILURE);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    if (HardwareConfigGuid != NULL) {
        Status = PnpStringFromGuid(HardwareConfigGuid,
                                   GuidString,
                                   RTL_NUMBER_OF(GuidString));

        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }

        Status = RtlStringCchCatExW(HardwareConfigPath,
                                    MAX_PATH,
                                    GuidString,
                                    NULL,
                                    NULL,
                                    STRSAFE_NULL_ON_FAILURE);

        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }
    } else {
        Status = RtlStringCchCatExW(HardwareConfigPath,
                                    MAX_PATH,
                                    REGSTR_KEY_CURRENT,
                                    NULL,
                                    NULL,
                                    STRSAFE_NULL_ON_FAILURE);

        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }
    }

    //
    // Try opening the registry key for the hardware configuration that was
    // specified.
    //

    Status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               PNPRTL_HKEY_LOCAL_MACHINE,
                               HardwareConfigPath,
                               0,
                               DesiredAccess,
                               &hHardwareConfig);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    *HardwareConfigKey = hHardwareConfig;
    hHardwareConfig = NULL;

  Cleanup:

    if (hHardwareConfig != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hHardwareConfig);
    }

    return Status;
}

NTSTATUS
PipHardwareConfigGetIndex(
    __in_opt LPGUID HardwareConfigGuid,
    __out PULONG HardwareConfigIndex
    )

/*++

Routine Description:

    This routine retrieves the index of the specified hardware configuration. If
    no hardware configuration is specified, it retrieves the index of the
    current hardware configuration.

Arguments:

    HardwareConfigGuid - Optionally supplies a pointer to a GUID that
        represents the hardware configuration to retrieve the number for.  If
        this is NULL, the current hardware configuration will be used.

    HardwareConfigIndex - Returns the index of the specified hardware
        configuration.


Return Value:

    STATUS_SUCCESS if the number of the specified hardware configuration was
    successfully retrieved.  An NTSTATUS error code otherwise.

--*/
{
    HANDLE hHardwareConfig;
    ULONG RegSize;
    ULONG RegType;
    NTSTATUS Status;

    PAGED_CODE();

    hHardwareConfig = NULL;

    if (HardwareConfigIndex == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    //
    // Try opening the registry key for the hardware configuration that was
    // specified.
    //

    Status = PipHardwareConfigOpenKey(HardwareConfigGuid,
                                      KEY_READ,
                                      &hHardwareConfig);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Retrieve the index for this hardware configuration.
    //

    RegSize = sizeof(ULONG);
    Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  hHardwareConfig,
                                  REGSTR_VAL_HARDWARECONFIG_ID,
                                  &RegType,
                                  (PVOID)HardwareConfigIndex,
                                  &RegSize);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    PNP_ASSERT(RegType == REG_DWORD);
    PNP_ASSERT(RegSize == sizeof(ULONG));

  Cleanup:

    if (hHardwareConfig != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hHardwareConfig);
    }

    return Status;
}

NTSTATUS
PipHardwareConfigGetLastUseTime(
    __in_opt LPGUID HardwareConfigGuid,
    __out PLARGE_INTEGER LastUseTime
    )

/*++

Routine Description:

    This routine retrieves the last use time of the specified hardware
    configuration. If no hardware configuration is specified, it retrieves the
    last use time of the current hardware configuration.

Arguments:

    HardwareConfigGuid - Optionally supplies a pointer to a GUID that represents
        the hardware configuration to retrieve the last use time for.  If this
        is NULL, the current hardware configuration will be used.

    LastUseTime - Returns the last use time of the specified hardware
        configuration.


Return Value:

    STATUS_SUCCESS if the last use time of the specified hardware configuration
    was successfully retrieved.  An NTSTATUS error code otherwise.

--*/
{
    HANDLE hHardwareConfig;
    ULONG RegSize;
    ULONG RegType;
    NTSTATUS Status;

    PAGED_CODE();

    hHardwareConfig = NULL;

    if (LastUseTime == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    //
    // Try opening the registry key for the hardware configuration that was
    // specified.
    //

    Status = PipHardwareConfigOpenKey(HardwareConfigGuid,
                                      KEY_READ,
                                      &hHardwareConfig);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Retrieve the last use time for this hardware configuration.
    //

    RegSize = sizeof(LARGE_INTEGER);
    Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  hHardwareConfig,
                                  REGSTR_VAL_HARDWARECONFIG_LASTUSE,
                                  &RegType,
                                  (PVOID)LastUseTime,
                                  &RegSize);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    PNP_ASSERT(RegType == REG_BINARY);
    PNP_ASSERT(RegSize == sizeof(LARGE_INTEGER));

  Cleanup:

    if (hHardwareConfig != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hHardwareConfig);
    }

    return Status;
}

__callback
__checkReturn
PNPCTX_REG_ENUM_KEY_ACTION
PipHardwareConfigClearStartOverrideCallback(
    __in HPNPCTX hPnpCtx,
    __in HANDLE ParentKey,
    __in PWSTR KeyName,
    __in_opt PVOID Context
    )
{
    PWSTR hardwareConfigIndexString;
    HANDLE startOverrideKey;
    WCHAR startOverrideKeyPath[MAX_PATH];
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(Context != NULL);

    hardwareConfigIndexString = (PWSTR)Context;
    startOverrideKey = NULL;

    status = RtlStringCchPrintfExW(startOverrideKeyPath,
                                   MAX_PATH,
                                   NULL,
                                   NULL,
                                   STRSAFE_NULL_ON_FAILURE,
                                   L"%s\\%s",
                                   KeyName,
                                   REGSTR_KEY_START_OVERRIDE);

    if (!NT_SUCCESS(status)) {
        PNP_ASSERT(status != STATUS_BUFFER_OVERFLOW);
        goto Cleanup;
    }

    status = _PnpCtxRegOpenKey(hPnpCtx,
                               ParentKey,
                               startOverrideKeyPath,
                               0,
                               KEY_SET_VALUE,
                               &startOverrideKey);

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = STATUS_SUCCESS;
        }
        goto Cleanup;
    }

    status = _PnpCtxRegDeleteValue(hPnpCtx,
                                   startOverrideKey,
                                   hardwareConfigIndexString);

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = STATUS_SUCCESS;
        }
        goto Cleanup;
    }


  Cleanup:

    if (startOverrideKey != NULL) {
        _PnpCtxRegCloseKey(hPnpCtx, startOverrideKey);
    }

    //
    // Even if there was an error clearing the start override for this particular
    // service, continue enumerating other services to clear their start override.
    //

    return PNPCTX_REG_ENUM_KEY_NEXT;
}

NTSTATUS
PipHardwareConfigTriggerRespecialize(
    __in_opt HANDLE SysprepHandle
    )

/*++

Routine Description:

    This routine writes the values that cause respecialize to be triggered.

Arguments:

    SysprepHandle - Optionally supplies a handle to the sysprep key.

Return Value:

    STATUS_SUCCESS or an NTSTATUS error code.

--*/
{
    ULONG respecialize;
    NTSTATUS status;
    HANDLE tempSysprepHandle;
    UNICODE_STRING unicodeName;

    tempSysprepHandle = NULL;

    if (SysprepHandle == NULL) {
        PiWstrToUnicodeString(&unicodeName, CM_REGISTRY_MACHINE(REGSTR_PATH_SYSTEM_SETUP));
        status = IopCreateRegistryKeyEx(&tempSysprepHandle,
                                        NULL,
                                        &unicodeName,
                                        KEY_ALL_ACCESS,
                                        REG_OPTION_NON_VOLATILE,
                                        NULL);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        SysprepHandle = tempSysprepHandle;
    }

    respecialize = 1;
    PiWstrToUnicodeString(&unicodeName, REGSTR_VAL_RESPECIALIZE);
    status = ZwSetValueKey(SysprepHandle,
                           &unicodeName,
                           TITLE_INDEX_VALUE,
                           REG_DWORD,
                           &respecialize,
                           sizeof(respecialize));

    PNP_ASSERT(NT_SUCCESS(status));

  Cleanup:

    if (tempSysprepHandle != NULL) {
        ZwClose(tempSysprepHandle);
    }

    return status;
}

NTSTATUS
PnpHardwareConfigCreateBootDriverFlags(
    __in_opt LPGUID HardwareConfigGuid,
    __in HANDLE BootVolumeHandle
    )

/*++

Routine Description:

    This routine creates an override boot driver flags value if one does not
    already exist.

Arguments:

    HardwareConfigGuid - Optionally supplies a pointer to a GUID that represents
        the hardware configuration to retrieve the last use time for.  If this
        is NULL, the current hardware configuration will be used.

    BootVolumeHandle - Supplies a handle to the boot volume.

Return Value:

    STATUS_SUCCESS or an NTSTATUS error code.

--*/

{
    ULONG BootDriverFlags;
    STORAGE_DESCRIPTOR_HEADER DescriptorHeader;
    PSTORAGE_DEVICE_DESCRIPTOR DeviceDescriptor;
    ULONG GlobalBootDriverFlags;
    HANDLE hControl;
    HANDLE hHardwareConfig;
    IO_STATUS_BLOCK IoStatusBlock;
    ULONG NewBootDriverFlags;
    STORAGE_PROPERTY_QUERY Query;
    ULONG RegSize;
    ULONG RegType;
    NTSTATUS Status;

    PAGED_CODE();

    BootDriverFlags = 0;
    DeviceDescriptor = NULL;
    GlobalBootDriverFlags = 0;
    hControl = NULL;
    hHardwareConfig = NULL;

    if (BootVolumeHandle == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    //
    // Try opening the registry key for the hardware configuration that was
    // specified.
    //

    Status = PipHardwareConfigOpenKey(HardwareConfigGuid,
                                      KEY_READ,
                                      &hHardwareConfig);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Check if an override already exists.  If one exists, a new one does not
    // need to be created.
    //

    RegSize = sizeof(BootDriverFlags);
    Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  hHardwareConfig,
                                  REGSTR_VAL_BOOTDRIVERFLAGS,
                                  &RegType,
                                  (PVOID)&BootDriverFlags,
                                  &RegSize);

    if (NT_SUCCESS(Status)) {
        PNP_ASSERT(RegType == REG_DWORD);
        PNP_ASSERT(RegSize == sizeof(BootDriverFlags));
        goto Cleanup;

    } else if (Status != STATUS_OBJECT_NAME_NOT_FOUND) {
        goto Cleanup;
    }

    //
    // This hardware configuration does not have an override boot driver flags
    // value.  Query the enumerator from the boot disk to use in creating an
    // override boot driver flags.
    //

    RtlZeroMemory(&IoStatusBlock, sizeof(IoStatusBlock));
    RtlZeroMemory(&Query, sizeof(Query));
    RtlZeroMemory(&DescriptorHeader, sizeof(DescriptorHeader));

    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;

    //
    // Query with a descriptor header to find out the size that needs to be
    // allocated.
    //

    Status = ZwDeviceIoControlFile(BootVolumeHandle,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &IoStatusBlock,
                                   IOCTL_STORAGE_QUERY_PROPERTY,
                                   &Query,
                                   sizeof(Query),
                                   &DescriptorHeader,
                                   sizeof(DescriptorHeader));

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Allocate the proper size for the descriptor property and query it.
    //

    DeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)ExAllocatePool(PagedPool,
                                                                  DescriptorHeader.Size);

    if (DeviceDescriptor == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    Status = ZwDeviceIoControlFile(BootVolumeHandle,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &IoStatusBlock,
                                   IOCTL_STORAGE_QUERY_PROPERTY,
                                   &Query,
                                   sizeof(Query),
                                   DeviceDescriptor,
                                   DescriptorHeader.Size);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Get the default global boot driver flags.
    //

    Status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               PNPRTL_HKEY_LOCAL_MACHINE,
                               REGSTR_PATH_CONTROL,
                               0,
                               KEY_QUERY_VALUE,
                               &hControl);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    RegSize = sizeof(GlobalBootDriverFlags);
    Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  hControl,
                                  REGSTR_VAL_BOOTDRIVERFLAGS,
                                  &RegType,
                                  (PVOID)&GlobalBootDriverFlags,
                                  &RegSize);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    PNP_ASSERT(RegType == REG_DWORD);
    PNP_ASSERT(RegSize == sizeof(GlobalBootDriverFlags));

    //
    // Clear the bits for the boot driver flags that can be manipulated
    // below. It will start off with the assumption that none of them are needed
    // and add back in any that are necessary.
    //

    NewBootDriverFlags = GlobalBootDriverFlags & ~(BOOT_DRIVER_FLAG_MASK);

    //
    // Figure out what bus type it is.  The behavior for updating the boot driver
    // flags depends on the bus type of the boot disk.
    //

    switch (DeviceDescriptor->BusType) {
    case BusTypeUsb:

        //
        // Note that this could be optimized further.  If the USB device is
        // external, then you need both USB flags since it could be plugged into
        // either a USB2 or USB3 port.  However, if the USB device is internal,
        // you only need the flag that corresponds to the controller the USB
        // device is plugged into (EHCI or XHCI).  The function
        // GetUSBBootInformation() can be examined to see how to check this
        // external vs internal and the controller type.
        //

        NewBootDriverFlags |= (CM_SERVICE_USB_DISK_BOOT_LOAD | CM_SERVICE_USB3_DISK_BOOT_LOAD);
        break;

    case BusTypeSd:
        NewBootDriverFlags |= CM_SERVICE_SD_DISK_BOOT_LOAD;
        break;

    case BusTypeScsi:
    case BusTypeAtapi:
    case BusTypeAta:
    case BusTypeSata:

        //
        // These are legacy bus types whose drivers are already boot start by
        // default and don't get promoted via BootDriverFlags.  As such,
        // BootDriverFlags isn't needed to cause any other drivers to load to
        // find the boot disk.  The override value doesn't need to be modified
        // to add in anything here.
        //

        break;

    case BusTypeFileBackedVirtual:

        //
        // This is a native boot of a vhd.  The vhd could be residing on any
        // storage bus and could potentially move between buses for different
        // boots (e.g. move from a USB2 port to a USB3 port).  The override boot
        // driver flags value should contain all the flags we can manipulate
        // here.
        //

        NewBootDriverFlags |= (BOOT_DRIVER_FLAG_MASK);

        break;

    default:

        //
        // This isn't a typical bus type to boot from and it wasn't a bus type
        // that is understood here, so no flags need to be added back in.
        //

        break;
    }

    //
    // Write out the new boot driver flags override.
    //

    Status = _PnpCtxRegSetValue(PiPnpRtlCtx,
                                hHardwareConfig,
                                REGSTR_VAL_BOOTDRIVERFLAGS,
                                REG_DWORD,
                                (PVOID)&NewBootDriverFlags,
                                sizeof(NewBootDriverFlags));

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

  Cleanup:

    if (hControl != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hControl);
    }

    if (hHardwareConfig != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hHardwareConfig);
    }

    if (DeviceDescriptor != NULL) {
        ExFreePool(DeviceDescriptor);
    }

    return Status;
}

VOID
PipCheckSystemFirmwareUpdated(
   __out PBOOLEAN SystemFirmwareUpdated
   )
/*++

Routine Description:

    Check if the system firmware has been updated since the last boot
    of this hardware configuration and update the current firmware version
    for this hardware configuration.

Arguments:

    SystemFirmwareUpdated - Returns whether the system firmware has been updated.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hCurrentHardwareConfig = NULL;
    HANDLE hHardwareDescription = NULL;
    PKEY_VALUE_FULL_INFORMATION currentConfigVerInfo = NULL;
    PKEY_VALUE_FULL_INFORMATION hardwareDescVerInfo = NULL;
    UNICODE_STRING unicodeString;

    *SystemFirmwareUpdated = FALSE;

    //
    // Read the System Bios Version from the Hardware Description key.  This
    // value was set from loader block information earlier during this boot
    // by Cm code.
    //

    status = IopOpenRegistryKeyEx(&hHardwareDescription,
                                  NULL,
                                  &CmRegistryMachineHardwareDescriptionSystemName,
                                  KEY_ALL_ACCESS);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    status = IopGetRegistryValue(hHardwareDescription,
                                 REGSTR_VAL_HARDWARECONFIG_SYSTEMBIOSVERSION,
                                 0,
                                 &hardwareDescVerInfo);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Read the System Bios Version from the Current Hardware Config key.
    // This value contains the System Bios Version that this hardware config
    // was last booted with.
    //

    PiWstrToUnicodeString(&unicodeString, CM_REGISTRY_MACHINE(REGSTR_PATH_CURRENT_HARDWARECONFIG));

    status = IopOpenRegistryKeyEx(&hCurrentHardwareConfig,
                                  NULL,
                                  &unicodeString,
                                  KEY_ALL_ACCESS);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    status = IopGetRegistryValue(hCurrentHardwareConfig,
                                 REGSTR_VAL_HARDWARECONFIG_SYSTEMBIOSVERSION,
                                 0,
                                 &currentConfigVerInfo);

    //
    // If the System Bios Version value does not exist under the hardware
    // config key or it does not match the value from the hardware description
    // key, then update the value and return that the firmware has been updated
    // since last boot of this hardware config.
    //

    if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
        (NT_SUCCESS(status) &&
         ((hardwareDescVerInfo->Type != currentConfigVerInfo->Type) ||
          (hardwareDescVerInfo->DataLength != currentConfigVerInfo->DataLength) ||
          (memcmp(KEY_VALUE_DATA(hardwareDescVerInfo),
                  KEY_VALUE_DATA(currentConfigVerInfo),
                  hardwareDescVerInfo->DataLength) != 0)))) {

        PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_HARDWARECONFIG_SYSTEMBIOSVERSION);

        status = ZwSetValueKey(hCurrentHardwareConfig,
                               &unicodeString,
                               0,
                               hardwareDescVerInfo->Type,
                               KEY_VALUE_DATA(hardwareDescVerInfo),
                               hardwareDescVerInfo->DataLength);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        *SystemFirmwareUpdated = TRUE;
    }

Cleanup:

    if (hCurrentHardwareConfig) {
        ZwClose(hCurrentHardwareConfig);
    }

    if (hHardwareDescription) {
        ZwClose(hHardwareDescription);
    }

    if (hardwareDescVerInfo) {
        ExFreePool(hardwareDescVerInfo);
    }

    if (currentConfigVerInfo) {
        ExFreePool(currentConfigVerInfo);
    }
}

//
// Computer ID namespace GUID:
// {70ffd812-4c7f-4c7d-0000-000000000000}
//
GUID PnpComputerIdNamespaceGuid = {
    0x70ffd812,
    0x4c7f,
    0x4c7d,
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

#define PNP_COMPUTER_ID_COUNT           15
#define PNP_COMPUTER_HARDWARE_ID_PREFIX L"ComputerMetadata\\"

#define PNP_SMBIOS_STRING_MANUFACTURER              0
#define PNP_SMBIOS_STRING_FAMILY                    1
#define PNP_SMBIOS_STRING_PRODUCT_NAME              2
#define PNP_SMBIOS_STRING_SKU_NUMBER                3
#define PNP_SMBIOS_STRING_BIOS_VENDOR               4
#define PNP_SMBIOS_STRING_BIOS_VERSION              5
#define PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER    6
#define PNP_SMBIOS_STRING_BASEBOARD_PRODUCT         7
#define PNP_SMBIOS_STRING_COUNT                     8

NTSTATUS
PipInitComputerIds(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )
/*++

Routine Description:

    This routine initializes the computer IDs and sets them as hardware IDs
    on the root device container. The computer IDs are generated from SHA-1
    hashes of the following strings from the SMBIOS data:

    (1)  Manufacturer + Family + ProductName + SKUNumber + BiosVendor + BiosVersion + BiosMajorRelease + BiosMinorRelease
    (2)  Manufacturer + Family + ProductName + BiosVendor + BiosVersion + BiosMajorRelease + BiosMinorRelease
    (3)  Manufacturer + ProductName + BiosVendor + BiosVersion + BiosMajorRelease + BiosMinorRelease
    (4)  Manufacturer + Family + ProductName + SKUNumber + Baseboard Manufacturer + Baseboard Product
    (5)  Manufacturer + Family + ProductName + SKUNumber
    (6)  Manufacturer + Family + ProductName
    (7)  Manufacturer + SKUNumber + Baseboard Manufacturer + Baseboard Product
    (8)  Manufacturer + SKUNumber
    (9)  Manufacturer + ProductName + Baseboard Manufacturer + Baseboard Product
    (10) Manufacturer + ProductName
    (11) Manufacturer + Family + Baseboard Manufacturer + Baseboard Product
    (12) Manufacturer + Family
    (13) Manufacturer + EnclosureType
    (14) Manufacturer + Baseboard Manufacturer + Baseboard Product
    (15) Manufacturer

Arguments:

    LoaderBlock - Supplies loader block containing SMBIOS data.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE hardwareConfigKeyHandle = NULL;
    HANDLE computerIdsKeyHandle = NULL;
    PSMBIOS3_EPS_HEADER smBiosEpsHeader;
    PHYSICAL_ADDRESS physicalAddress;
    PVOID mapAddress = NULL;
    ULONG mapSize = 0;
    PSMBIOS_STRUCT_HEADER structHeader;
    PSMBIOS_BIOS_INFORMATION_STRUCT biosInfo;
    PSMBIOS_SYSTEM_INFORMATION_STRUCT systemInfo;
    PSMBIOS_BASE_BOARD_INFORMATION_STRUCT baseboardInfo;
    PSMBIOS_SYSTEM_CHASIS_INFORMATION_STRUCT chasisInfo;
    UCHAR majorVersion = 0;
    UCHAR minorVersion = 0;
    WCHAR releaseVersionBuffer[8];
    UNICODE_STRING smBiosReleaseVersionString;
    UCHAR enclosureType = 0;
    WCHAR enclosureTypeBuffer[4];
    UNICODE_STRING smBiosEnclosureTypeString;
    UNICODE_STRING smBiosStrings[PNP_SMBIOS_STRING_COUNT];
    PUNICODE_STRING stringList[7];
    GUID computerIds[PNP_COMPUTER_ID_COUNT];
    ULONG computerIdCount = 0;
    PZZWSTR hardwareIds = NULL;
    UNICODE_STRING unicodeString;
    ULONG length;
    ULONG i;

    RtlZeroMemory(smBiosStrings, sizeof(smBiosStrings));

    //
    // Open current hardware configuration key.
    //
    PiWstrToUnicodeString(&unicodeString,
                          CM_REGISTRY_MACHINE(REGSTR_PATH_CURRENT_HARDWARECONFIG));

    status = IopOpenRegistryKeyEx(&hardwareConfigKeyHandle,
                                  NULL,
                                  &unicodeString,
                                  KEY_ALL_ACCESS);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Delete existing ComputerIds subkey under hardware configuration key.
    //
    _PnpCtxRegDeleteTree(PiPnpRtlCtx,
                         hardwareConfigKeyHandle,
                         REGSTR_KEY_HARDWARECONFIG_COMPUTERIDS);

    //
    // Create empty ComputerIds subkey under hardware configuration key.
    //
    PiWstrToUnicodeString(&unicodeString,
                          REGSTR_KEY_HARDWARECONFIG_COMPUTERIDS);

    status = IopCreateRegistryKeyEx(&computerIdsKeyHandle,
                                    hardwareConfigKeyHandle,
                                    &unicodeString,
                                    KEY_ALL_ACCESS,
                                    REG_OPTION_NON_VOLATILE,
                                    NULL);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Get SMBIOS table data from loader block.
    //
    if (LoaderBlock->Extension->SMBiosEPSHeader == NULL) {
        status = STATUS_NOT_SUPPORTED;
        goto Cleanup;
    }

    smBiosEpsHeader = LoaderBlock->Extension->SMBiosEPSHeader;
    physicalAddress.QuadPart = smBiosEpsHeader->StructureTableAddress;

    if (physicalAddress.QuadPart == 0) {
        status = STATUS_NOT_SUPPORTED;
        goto Cleanup;
    }

    //
    // Map SMBIOS table into memory from physical address.
    //

    mapSize = smBiosEpsHeader->StructureTableMaximumSize;
    mapAddress = MmMapIoSpaceEx(physicalAddress, mapSize, PAGE_READONLY);

    if (mapAddress == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Get SMBIOS system information.
    //
    structHeader = PipSmBiosFindStruct(SMBIOS_SYSTEM_INFORMATION,
                                       0x00,
                                       mapAddress,
                                       mapSize);

    if (structHeader != NULL) {
        systemInfo = (PSMBIOS_SYSTEM_INFORMATION_STRUCT)structHeader;

        //
        // Get system manufacturer string.
        //
        if (RTL_CONTAINS_FIELD(systemInfo, systemInfo->Length, Manufacturer)) {
            status = PipSmBiosGetString(structHeader,
                                        systemInfo->Manufacturer,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get system family string.
        //
        if (RTL_CONTAINS_FIELD(systemInfo, systemInfo->Length, Family)) {
            status = PipSmBiosGetString(structHeader,
                                        systemInfo->Family,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_FAMILY]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get system product name string.
        //
        if (RTL_CONTAINS_FIELD(systemInfo, systemInfo->Length, ProductName)) {
            status = PipSmBiosGetString(structHeader,
                                        systemInfo->ProductName,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get system SKU number string.
        //
        if (RTL_CONTAINS_FIELD(systemInfo, systemInfo->Length, SKUNumber)) {
            status = PipSmBiosGetString(structHeader,
                                        systemInfo->SKUNumber,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }
    }

    //
    // Get SMBIOS baseboard information.
    //
    structHeader = PipSmBiosFindStruct(SMBIOS_BASE_BOARD_INFORMATION_TYPE,
                                       0x0a,
                                       mapAddress,
                                       mapSize);

    if (structHeader != NULL) {
        baseboardInfo = (PSMBIOS_BASE_BOARD_INFORMATION_STRUCT)structHeader;

        //
        // Get baseboard manufacturer string.
        //
        if (RTL_CONTAINS_FIELD(baseboardInfo, baseboardInfo->Length, Manufacturer)) {
            status = PipSmBiosGetString(structHeader,
                                        baseboardInfo->Manufacturer,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get baseboard product string.
        //
        if (RTL_CONTAINS_FIELD(baseboardInfo, baseboardInfo->Length, Product)) {
            status = PipSmBiosGetString(structHeader,
                                        baseboardInfo->Product,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }
    }

    //
    // Get SMBIOS BIOS information.
    //
    structHeader = PipSmBiosFindStruct(SMBIOS_BIOS_INFORMATION_TYPE,
                                       0x00,
                                       mapAddress,
                                       mapSize);

    if (structHeader != NULL) {
        biosInfo = (PSMBIOS_BIOS_INFORMATION_STRUCT)structHeader;

        //
        // Get BIOS vendor string.
        //
        if (RTL_CONTAINS_FIELD(biosInfo, biosInfo->Length, Vendor)) {
            status = PipSmBiosGetString(structHeader,
                                        biosInfo->Vendor,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VENDOR]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get BIOS version string.
        //
        if (RTL_CONTAINS_FIELD(biosInfo, biosInfo->Length, Version)) {
            status = PipSmBiosGetString(structHeader,
                                        biosInfo->Version,
                                        mapAddress,
                                        mapSize,
                                        &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VERSION]);

            if (status == STATUS_NOT_FOUND) {
                status = STATUS_SUCCESS;
            } else if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Get BIOS major/minor release version.
        //
        if (RTL_CONTAINS_FIELD(biosInfo, biosInfo->Length, SystemBiosMajorRelease)) {
            majorVersion = biosInfo->SystemBiosMajorRelease;
        }

        if (RTL_CONTAINS_FIELD(biosInfo, biosInfo->Length, SystemBiosMinorRelease)) {
            minorVersion = biosInfo->SystemBiosMinorRelease;
        }
    }

    //
    // Get SMBIOS chasis information.
    //
    structHeader = PipSmBiosFindStruct(SMBIOS_SYSTEM_CHASIS_INFORMATION_TYPE,
                                       0x00,
                                       mapAddress,
                                       mapSize);

    if (structHeader != NULL) {
        chasisInfo = (PSMBIOS_SYSTEM_CHASIS_INFORMATION_STRUCT)structHeader;

        //
        // Get chasis enclosure type, excluding most significant bit that
        // indicates if enclosure is locked.
        //
        if (RTL_CONTAINS_FIELD(chasisInfo, chasisInfo->Length, ChasisType)) {
            enclosureType = chasisInfo->ChasisType & 0x7F;
        }
    }

    //
    // Free any empty SMBIOS strings.
    //
    for (i = 0; i < RTL_NUMBER_OF(smBiosStrings); i++) {
        if ((smBiosStrings[i].Buffer != NULL) &&
            (smBiosStrings[i].Length == 0)) {
            RtlFreeUnicodeString(&smBiosStrings[i]);
        }
    }

    //
    // Create computer IDs.
    //
    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_BIOS_VENDOR].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_BIOS_VERSION].Buffer != NULL)) {
        //
        // Build SMBIOS version string from major/minor version.
        //
        status = RtlStringCchPrintfW(releaseVersionBuffer,
                                     RTL_NUMBER_OF(releaseVersionBuffer),
                                     L"%02x&%02x",
                                     majorVersion,
                                     minorVersion);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        RtlInitUnicodeString(&smBiosReleaseVersionString, releaseVersionBuffer);

        //
        // Create computer ID (1).
        //
        if (smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER].Buffer != NULL) {
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
            stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER];
            stringList[4] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VENDOR];
            stringList[5] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VERSION];
            stringList[6] = &smBiosReleaseVersionString;

            status = PipCreateComputerId(computerIdsKeyHandle,
                                         stringList,
                                         7,
                                         &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (2).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
        stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
        stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VENDOR];
        stringList[4] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VERSION];
        stringList[5] = &smBiosReleaseVersionString;

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     6,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        //
        // Create computer ID (3).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
        stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VENDOR];
        stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_BIOS_VERSION];
        stringList[4] = &smBiosReleaseVersionString;

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     5,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME].Buffer != NULL)) {
        if (smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER].Buffer != NULL) {
            if ((smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER].Buffer != NULL) &&
                (smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT].Buffer != NULL)) {
                //
                // Create computer ID (4).
                //
                stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
                stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
                stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
                stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER];
                stringList[4] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER];
                stringList[5] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT];


                status = PipCreateComputerId(computerIdsKeyHandle,
                                             stringList,
                                             6,
                                             &computerIds[computerIdCount++]);

                if (!NT_SUCCESS(status)) {
                    goto Cleanup;
                }
            }

            //
            // Create computer ID (5).
            //
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
            stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER];

            status = PipCreateComputerId(computerIdsKeyHandle,
                                         stringList,
                                         4,
                                         &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (6).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
        stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     3,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER].Buffer != NULL)) {

        if ((smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER].Buffer != NULL) &&
            (smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT].Buffer != NULL)) {
            //
            // Create computer ID (7).
            //
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER];
            stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT];

            status = PipCreateComputerId(computerIdsKeyHandle,
                                         stringList,
                                         4,
                                         &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (8).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_SKU_NUMBER];

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     2,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME].Buffer != NULL)) {
        if ((smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER].Buffer != NULL) &&
            (smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT].Buffer != NULL)) {
            //
            // Create computer ID (9).
            //
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER];
            stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT];

            status = PipCreateComputerId(computerIdsKeyHandle,
                                         stringList,
                                         4,
                                         &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (10).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_PRODUCT_NAME];

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     2,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (smBiosStrings[PNP_SMBIOS_STRING_FAMILY].Buffer != NULL)) {

        if ((smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT].Buffer != NULL) &&
            (smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER].Buffer != NULL)) {
            //
            // Create computer ID (11).
            //
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER];
            stringList[3] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT];

            status = PipCreateComputerId(computerIdsKeyHandle,
                                         stringList,
                                         4,
                                         &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (12).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_FAMILY];

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     2,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if ((smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) &&
        (enclosureType != 0)) {
        //
        // Build SMBIOS enclosure type string.
        //
        status = RtlStringCchPrintfW(enclosureTypeBuffer,
                                     RTL_NUMBER_OF(enclosureTypeBuffer),
                                     L"%x",
                                     enclosureType);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

        RtlInitUnicodeString(&smBiosEnclosureTypeString, enclosureTypeBuffer);
        //
        // Create computer ID (13).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
        stringList[1] = &smBiosEnclosureTypeString;

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     2,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    if (smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER].Buffer != NULL) {

        if ((smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT].Buffer != NULL) &&
            (smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER].Buffer != NULL)) {
            //
            // Create computer ID (14).
            //
            stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];
            stringList[1] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_MANUFACTURER];
            stringList[2] = &smBiosStrings[PNP_SMBIOS_STRING_BASEBOARD_PRODUCT];

            status = PipCreateComputerId(computerIdsKeyHandle,
                stringList,
                3,
                &computerIds[computerIdCount++]);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }
        }

        //
        // Create computer ID (15).
        //
        stringList[0] = &smBiosStrings[PNP_SMBIOS_STRING_MANUFACTURER];

        status = PipCreateComputerId(computerIdsKeyHandle,
                                     stringList,
                                     1,
                                     &computerIds[computerIdCount++]);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    //
    // Allocate buffer for computer hardware IDs.
    //
    length = ((sizeof(PNP_COMPUTER_HARDWARE_ID_PREFIX) - sizeof(UNICODE_NULL)) +
              (GUID_STRING_LEN * sizeof(WCHAR))) * computerIdCount
           + sizeof(UNICODE_NULL);

    hardwareIds = (PZZWSTR)ExAllocatePool(PagedPool, length);

    if (hardwareIds == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Build multi-sz list of computer hardware IDs in the following format:
    //
    // ComputerMetadata\{ComputerId 1}
    // ComputerMetadata\{ComputerId 2}
    // ...
    // ComputerMetadata\{ComputerId N}
    //
    unicodeString.Buffer = hardwareIds;
    unicodeString.MaximumLength = (USHORT)length;

    for (i = 0; i < computerIdCount; i++) {
        unicodeString.Length = 0;

        status = RtlUnicodeStringCopyStringEx(&unicodeString,
                                              PNP_COMPUTER_HARDWARE_ID_PREFIX,
                                              &unicodeString,
                                              0);

        if (!NT_SUCCESS(status)) {
            break;
        }

        status = RtlStringFromGUIDEx(&computerIds[i],
                                     &unicodeString,
                                     FALSE);

        if (!NT_SUCCESS(status)) {
            break;
        }

        status = RtlUpcaseUnicodeString(&unicodeString,
                                        &unicodeString,
                                        FALSE);

        if (!NT_SUCCESS(status)) {
            break;
        }

        unicodeString.Buffer += GUID_STRING_LEN;
        unicodeString.MaximumLength -= GUID_STRING_LEN * sizeof(WCHAR);
    }

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    *unicodeString.Buffer++ = UNICODE_NULL;

    PNP_ASSERT(unicodeString.Buffer == hardwareIds + (length / sizeof(WCHAR)));

    //
    // Set computer hardware IDs on root device container.
    //
    status = _PnpSetObjectProperty(PiPnpRtlCtx,
                                   REGSTR_VALUE_ROOT_CONTAINERID,
                                   PNP_OBJECT_TYPE_DEVICE_CONTAINER,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_Device_HardwareIds,
                                   DEVPROP_TYPE_STRING_LIST,
                                   (PUCHAR)hardwareIds,
                                   length,
                                   0);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    //
    // Free computer hardware IDs.
    //
    if (hardwareIds != NULL) {
        ExFreePool(hardwareIds);
    }

    //
    // Free SMBIOS strings.
    //
    for (i = 0; i < RTL_NUMBER_OF(smBiosStrings); i++) {
        RtlFreeUnicodeString(&smBiosStrings[i]);
    }

    //
    // Unmap SMBIOS table from memory.
    //
    if (mapAddress != NULL) {
        MmUnmapIoSpace(mapAddress, mapSize);
    }

    //
    // Close registry keys.
    //
    if (computerIdsKeyHandle != NULL) {
        ZwClose(computerIdsKeyHandle);
    }

    if (hardwareConfigKeyHandle != NULL) {
        ZwClose(hardwareConfigKeyHandle);
    }

    return status;
}

NTSTATUS
PipCreateComputerId(
    __in HANDLE ComputerIdsKeyHandle,
    __in_ecount(StringCount) PUNICODE_STRING *StringList,
    __in ULONG StringCount,
    __out LPGUID ComputerId
    )
/*++

Routine Description:

    This routine creates a computer ID from a set of SMBIOS strings and sets a
    value for it under the supplied registry key.

Arguments:

    ComputerIdsKeyHandle - Supplies a handle to a key that the computer ID
        should be set under.

    StringList - Supplies the list of strings to use.

    StringCount - Supplies the number of strings in the list.

    ComputerId - Returns the created computer ID.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    USHORT length = 0;
    PWSTR buffer = NULL;
    PWSTR string;
    WCHAR computerIdString[GUID_STRING_LEN];
    UNICODE_STRING unicodeString;
    ULONG i;

    //
    // Calculate length of buffer required to store string list as a single
    // string with each element delimited by a single character.
    //
    for (i = 0; i < StringCount; i++) {
        if (i > 0) {
            length += sizeof(WCHAR);
        }

        length += StringList[i]->Length;
    }

    if (length <= sizeof(WCHAR)) {
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    length += sizeof(UNICODE_NULL);

    //
    // Allocate buffer for string.
    //
    buffer = (PWSTR)ExAllocatePool(PagedPool, length);

    if (buffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    //
    // Copy string list elements into single string buffer.
    //
    string = buffer;

    for (i = 0; i < StringCount; i++) {
        if (i > 0) {
            *string++ = L'&';
        }

        if (StringList[i]->Length > 0) {
            RtlCopyMemory(string,
                          StringList[i]->Buffer,
                          StringList[i]->Length);

            string += StringList[i]->Length / sizeof(WCHAR);
        }
    }

    *string++ = UNICODE_NULL;

    PNP_ASSERT(string == buffer + (length / sizeof(WCHAR)));

    //
    // Generate computer ID GUID from string.
    //
    status = RtlGenerateClass5Guid(&PnpComputerIdNamespaceGuid,
                                   buffer,
                                   length - sizeof(UNICODE_NULL),
                                   ComputerId);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Convert computer ID GUID to a string.
    //
    status = PnpStringFromGuid(ComputerId,
                               computerIdString,
                               RTL_NUMBER_OF(computerIdString));

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Set computer ID value under supplied registry key.
    //
    RtlInitUnicodeString(&unicodeString, computerIdString);

    status = ZwSetValueKey(ComputerIdsKeyHandle,
                           &unicodeString,
                           TITLE_INDEX_VALUE,
                           REG_SZ,
                           buffer,
                           length);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    if (buffer != NULL) {
        ExFreePool(buffer);
    }

    return status;
}

BOOLEAN
PipCheckComputerSupported(
    __in PCWSTR ComputerId
    )
/*++

Routine Description:

    This routine checks if the specified computer ID is supported by the
    target computer by verifying that it matches one of the pre-generated
    computer IDs.

Arguments:

    ComputerId - Supplies the computer ID to check.

Return Value:

    TRUE if the computer ID is supported, FALSE otherwise.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE computerIdsKeyHandle = NULL;
    UNICODE_STRING unicodeString;
    KEY_VALUE_BASIC_INFORMATION keyValueInfo;
    ULONG keyValueLength = 0;

    PAGED_CODE();

    //
    // Open computer IDs subkey under current hardware configuration key.
    //
    PiWstrToUnicodeString(&unicodeString,
                          CM_REGISTRY_MACHINE(REGSTR_PATH_CURRENT_HARDWARECONFIG L"\\"
                                              REGSTR_KEY_HARDWARECONFIG_COMPUTERIDS));

    status = IopOpenRegistryKeyEx(&computerIdsKeyHandle,
                                  NULL,
                                  &unicodeString,
                                  KEY_READ);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Check for existence of computer ID value under subkey as an indication
    // that the supplied computer ID is supported.
    //
    RtlInitUnicodeString(&unicodeString, ComputerId);

    status = ZwQueryValueKey(computerIdsKeyHandle,
                             &unicodeString,
                             KeyValueBasicInformation,
                             &keyValueInfo,
                             sizeof(keyValueInfo),
                             &keyValueLength);

    PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);

    if (status == STATUS_BUFFER_OVERFLOW) {
        status = STATUS_SUCCESS;
    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    //
    // Close computer IDs subkey.
    //
    if (computerIdsKeyHandle != NULL) {
        ZwClose(computerIdsKeyHandle);
    }

    return NT_SUCCESS(status);
}

PSMBIOS_STRUCT_HEADER
PipSmBiosFindStruct(
    __in UCHAR Type,
    __in UCHAR SubType,
    __in_ecount(SmBiosDataLength) PUCHAR SmBiosData,
    __in ULONG SmBiosDataLength
    )
/*++

Routine Description:

    This routine finds a specific structure within the SMBIOS data.

Arguments:

    Type - Supplies the SMBIOS structure header type to find.

    SubType - Supplies a type within the information struct to find.

    SmBiosData - Supplies a pointer to the SMBIOS data.

    SmBiosDataLength - Supplies the length of the SMBIOS data.

Return Value:

    Pointer to the SMBIOS structure if found, NULL otherwise.

--*/
{
    PSMBIOS_STRUCT_HEADER structResult = NULL;
    PSMBIOS_STRUCT_HEADER structHeader;
    PUCHAR data;
    PUCHAR dataEnd;
    PSMBIOS_BASE_BOARD_INFORMATION_STRUCT baseboardInfo = NULL;

    data = SmBiosData;
    dataEnd = SmBiosData + (SmBiosDataLength - sizeof(USHORT));

    while (data < dataEnd) {
        structHeader = (PSMBIOS_STRUCT_HEADER)data;

        if (structHeader->Type == Type) {
            structResult = structHeader;

            if (structHeader->Type == SMBIOS_BASE_BOARD_INFORMATION_TYPE) {
                baseboardInfo = (PSMBIOS_BASE_BOARD_INFORMATION_STRUCT)structHeader;

                if (RTL_CONTAINS_FIELD(baseboardInfo, baseboardInfo->Length, BoardType) &&
                    baseboardInfo->BoardType == SubType) {

                    break;
                } else {
                    // keep looking for the correct subtype
                    structResult = NULL;
                }
            } else {
                break;
            }
        }

        data += structHeader->Length;

        while ((data < dataEnd) &&
               (*((USHORT UNALIGNED *)data) != 0)) {
            data++;
        }

        data += sizeof(USHORT);
    }

    return structResult;
}

NTSTATUS
PipSmBiosGetString(
    __in PSMBIOS_STRUCT_HEADER StructHeader,
    __in UCHAR StringNumber,
    __in_ecount(SmBiosDataLength) PUCHAR SmBiosData,
    __in ULONG SmBiosDataLength,
    __out PUNICODE_STRING UnicodeString
    )
/*++

Routine Description:

    This routine retrieves a specific string from a structure within the
    SMBIOS data.

    Any leading and/or trailing whitespace is trimmed from the string.

Arguments:

    StructureHeader - Supplies a pointer to the SMBIOS structure.

    StringNumber - Supplies the string number to retrieve.

    SmBiosData - Supplies a pointer to the SMBIOS data.

    SmBiosDataLength - Supplies the length of the SMBIOS data.

    UnicodeString - Returns SMBIOS string allocated as a unicode string.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status;
    PCHAR string;
    PCHAR stringEnd;
    ANSI_STRING ansiString;
    ULONG i;

    const static CHAR whitespace[] = " \t\r";

    if (StringNumber == 0) {
        status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    string = (PCHAR)StructHeader + StructHeader->Length;
    stringEnd = (PCHAR)SmBiosData + (SmBiosDataLength - 1);

    for (i = 1; i < StringNumber; i++) {
        while (*string++ != ANSI_NULL) {
            if (string >= stringEnd) {
                string = NULL;
                break;
            }
        }

        if (string == NULL) {
            break;
        } else if (*string == ANSI_NULL) {
            string = NULL;
            break;
        }
    }

    if (string == NULL) {
        status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    ansiString.Buffer = string;
    ansiString.Length = 0;

    while (*string++ != ANSI_NULL) {
        ansiString.Length++;

        if ((string >= stringEnd) ||
            (ansiString.Length > 64)) {
            string = NULL;
            break;
        }
    }

    if (string == NULL) {
        status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    while ((*ansiString.Buffer != ANSI_NULL) &&
           (strchr(whitespace, *ansiString.Buffer) != NULL)) {
        ansiString.Buffer++;
        ansiString.Length--;
    }

    while ((ansiString.Length > 0) &&
           (strchr(whitespace, ansiString.Buffer[ansiString.Length - 1]) != NULL)) {
        ansiString.Length--;
    }

    ansiString.MaximumLength = ansiString.Length + sizeof(CHAR);

    status = RtlAnsiStringToUnicodeString(UnicodeString, &ansiString, TRUE);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    return status;
}

NTSTATUS
PipMigratePnpState(
    VOID
    )
/*++

Routine Description:

    This routine migrates any pending PnP device and driver state that was
    prepared by the setup upgrade operation as part of the PnP migration phase.
    The PnP migration state is merged into the PnP system state, effectively
    seeding PnP with device configuration state from the previous OS version.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE migrationKeyHandle = NULL;
    HANDLE migrationControlSetKeyHandle = NULL;
    HANDLE currentControlSetKeyHandle = NULL;
    ULONG valueType = REG_NONE;
    ULONG valueSize = 0;
    BOOLEAN migrating = FALSE;
    ULONG migrationStatus = 0;
    ULONG targetVersion = 0;
    HANDLE deviceMigrationKeyHandle = NULL;
    LARGE_INTEGER migrationTimeLast;
    LARGE_INTEGER migrationTimeNext;
    HANDLE servicesKeyHandle = NULL;
    HPNPCTX hPnpCtx = NULL;
    ULONG bufferLength = 0;

    //
    // Open PnP upgrade migration key, if available.
    //
    status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               PNPRTL_HKEY_LOCAL_MACHINE,
                               REGSTR_PATH_PNP_MIGRATION_ROOT,
                               0,
                               KEY_READ | KEY_WRITE,
                               &migrationKeyHandle);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        status = STATUS_SUCCESS;
        goto Cleanup;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Check if PnP migration state was already migrated by checking for the
    // existence of a migration status from the first attempt.
    //
    valueSize = sizeof(migrationStatus);

    status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  migrationKeyHandle,
                                  REGSTR_VALUE_MIGRATION_STATUS,
                                  &valueType,
                                  (PVOID)&migrationStatus,
                                  &valueSize);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        status = STATUS_SUCCESS;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;

    } else if ((valueType != REG_DWORD) ||
               (valueSize != sizeof(migrationStatus))) {
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;
    }

    //
    // Open PnP control set key for migration.
    //
    status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               migrationKeyHandle,
                               REGSTR_KEY_MIGRATION_CONTROL_SET,
                               0,
                               KEY_READ | KEY_WRITE,
                               &migrationControlSetKeyHandle);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        status = STATUS_SUCCESS;
        goto Cleanup;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Open source device migration key to get timestamp.
    //
    status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               migrationControlSetKeyHandle,
                               REGSTR_PATH_DEVICE_MIGRATION_ROOT,
                               0,
                               KEY_READ,
                               &deviceMigrationKeyHandle);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Query next migration timestamp.
    //
    valueSize = sizeof(migrationTimeNext);

    status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  deviceMigrationKeyHandle,
                                  REGSTR_VALUE_MIGRATION_TIME,
                                  &valueType,
                                  (PVOID)&migrationTimeNext,
                                  &valueSize);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;

    } else if ((valueType != REG_BINARY) ||
               (valueSize != sizeof(migrationTimeNext))) {
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;
    }

    _PnpCtxRegCloseKey(PiPnpRtlCtx, deviceMigrationKeyHandle);
    deviceMigrationKeyHandle = NULL;

    //
    // Get current control set key.
    //
    status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                            PNPCTX_BASE_KEY_TYPE_CONTROLSET,
                                            &currentControlSetKeyHandle);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Open/create target device migration key to check if migration state
    // was already migrated.
    //
    status = _PnpCtxRegCreateKey(PiPnpRtlCtx,
                                 currentControlSetKeyHandle,
                                 REGSTR_PATH_DEVICE_MIGRATION_ROOT,
                                 REG_OPTION_NON_VOLATILE,
                                 KEY_WRITE,
                                 NULL,
                                 &deviceMigrationKeyHandle,
                                 NULL);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Query last migration timestamp.
    //
    valueSize = sizeof(migrationTimeLast);

    status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  deviceMigrationKeyHandle,
                                  REGSTR_VALUE_MIGRATION_TIME,
                                  &valueType,
                                  (PVOID)&migrationTimeLast,
                                  &valueSize);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        status = STATUS_SUCCESS;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;

    } else if ((valueType != REG_BINARY) ||
               (valueSize != sizeof(migrationTimeLast))) {
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;

    } else {
        //
        // Check if PnP migration state was already migrated.
        //
        if (RtlCompareMemory(&migrationTimeLast,
                             &migrationTimeNext,
                             sizeof(LARGE_INTEGER)) == sizeof(LARGE_INTEGER)) {
            status = STATUS_ALREADY_COMMITTED;
            goto Cleanup;
        }
    }

    //
    // Set an initial migration status as an indicator that the operation was
    // attempted once in order to prevent it from being repeated again during
    // a future boot. This will ensure that any failures to fully merge the
    // migration state below will not have any adverse effects on the PnP state
    // beyond the initial boot.
    //
    migrationStatus = STATUS_PENDING;

    status = _PnpCtxRegSetValue(PiPnpRtlCtx,
                                migrationKeyHandle,
                                REGSTR_VALUE_MIGRATION_STATUS,
                                REG_DWORD,
                                (PVOID)&migrationStatus,
                                sizeof(migrationStatus));

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    status = _PnpCtxRegSetValue(PiPnpRtlCtx,
                                deviceMigrationKeyHandle,
                                REGSTR_VALUE_MIGRATION_TIME,
                                REG_BINARY,
                                (PVOID)&migrationTimeNext,
                                sizeof(migrationTimeNext));

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    migrating = TRUE;

    //
    // Verify that PnP migration state is compatible with OS version.
    //
    valueSize = sizeof(targetVersion);

    status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  migrationKeyHandle,
                                  REGSTR_VALUE_MIGRATION_TARGET_VERSION,
                                  &valueType,
                                  (PVOID)&targetVersion,
                                  &valueSize);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;

    } else if ((valueType != REG_DWORD) ||
               (valueSize != sizeof(targetVersion))) {
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;

    } else if (OSVER(targetVersion) != OSVER(NTDDI_VERSION)) {
        status = STATUS_REVISION_MISMATCH;
        goto Cleanup;
    }

    //
    // Merge PnP upgrade control set key into current control set key.
    //
    status = _PnpCtxRegCopyTree(PiPnpRtlCtx,
                                migrationControlSetKeyHandle,
                                NULL,
                                currentControlSetKeyHandle,
                                NULL);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Open device migration services key to migrate any services that have
    // already been created.
    //
    status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               deviceMigrationKeyHandle,
                               REGSTR_KEY_SERVICES,
                               0,
                               KEY_READ,
                               &servicesKeyHandle);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        status = STATUS_SUCCESS;

    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;

    } else {
        //
        // Enumerate and migrate services.
        //
        status = _PnpCtxRegEnumKeyWithCallback(PiPnpRtlCtx,
                                               servicesKeyHandle,
                                               PipMigrateServiceCallback,
                                               NULL);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    //
    // Open local PnP machine context to modify device state directly.
    //
    status = _PnpCtxOpenMachine(0,
                                NULL,
                                INVALID_HANDLE_VALUE,
                                INVALID_HANDLE_VALUE,
                                NULL,
                                NTDDI_PNPRTL,
                                &hPnpCtx);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Enumerate and reset any existing devices in order to give them an
    // opportunity to be migrated.
    //
    status = _CmGetMatchingDeviceList(hPnpCtx,
                                      PipMigrateResetDeviceCallback,
                                      NULL,
                                      NULL,
                                      0,
                                      &bufferLength,
                                      0);

    if (status == STATUS_BUFFER_TOO_SMALL) {
        status = STATUS_SUCCESS;
    } else if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    //
    // Store final migration status.
    //
    if (migrating) {
        migrationStatus = (ULONG)status;

        _PnpCtxRegSetValue(PiPnpRtlCtx,
                           migrationKeyHandle,
                           REGSTR_VALUE_MIGRATION_STATUS,
                           REG_DWORD,
                           (PVOID)&migrationStatus,
                           sizeof(migrationStatus));
    }

    //
    // Close local PnP machine context.
    //
    if (hPnpCtx != NULL) {
        _PnpCtxCloseMachine(hPnpCtx);
    }

    //
    // Close registry keys.
    //
    if (servicesKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, servicesKeyHandle);
    }

    if (deviceMigrationKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, deviceMigrationKeyHandle);
    }

    if (migrationControlSetKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, migrationControlSetKeyHandle);
    }

    if (migrationKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, migrationKeyHandle);
    }

    return status;
}

__callback
_Must_inspect_result_
PNPCTX_REG_ENUM_KEY_ACTION
PipMigrateServiceCallback(
    __in HPNPCTX hPnpCtx,
    __in HANDLE ServicesKeyHandle,
    __in PWSTR ServiceName,
    __in_opt PVOID Context
    )
/*++

Routine Description:

    This routine migrates a service when it already exists.

Arguments:

    hPnpCtx - Supplies PnP machine context.

    ServicesKeyHandle - Supplies handle to migration services key.

    ServiceName - Supplies service name.

    Context - Supplies context.

Return Value:

    PNPCTX_REG_ENUM_KEY_NEXT to process next service.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE serviceKeyHandle = NULL;

    UNREFERENCED_PARAMETER(Context);

    //
    // Open source service key to migrate from.
    //
    status = _PnpCtxRegOpenKey(hPnpCtx,
                               ServicesKeyHandle,
                               ServiceName,
                               0,
                               KEY_ALL_ACCESS,
                               &serviceKeyHandle);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    //
    // Migrate service.
    //
    status = PiDevCfgMigrateService(NULL,
                                    ServiceName,
                                    serviceKeyHandle);

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    if (serviceKeyHandle != NULL) {
        _PnpCtxRegCloseKey(hPnpCtx, serviceKeyHandle);
    }

    return PNPCTX_REG_ENUM_KEY_NEXT;
}

__callback
_Must_inspect_result_
BOOLEAN
PipMigrateResetDeviceCallback(
    __in HPNPCTX hPnpCtx,
    __in PCWSTR DeviceInstanceId,
    __in CM_OBJECT_TYPE CmObjectType,
    __in_opt PVOID Context
    )
/*++

Routine Description:

    This routine resets a device by marking it for re-install so that it is
    reconfigured and potentially migrated upon its next enumeration.

Arguments:

    hPnpCtx - Supplies PnP machine context.

    DeviceInstanceId - Supplies device instance ID to reset.

    CmObjectType - Supplies device object type.

    Context - Supplies context.

Return Value:

    TRUE to process next device, FALSE to stop enumeration.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG configFlags = 0;
    ULONG propertyType = REG_NONE;
    ULONG propertySize = 0;

    UNREFERENCED_PARAMETER(CmObjectType);
    UNREFERENCED_PARAMETER(Context);

    //
    // Skip root device.
    //
    if (_wcsicmp(DeviceInstanceId, REGSTR_VAL_ROOT_DEVNODE) == 0) {
        status = STATUS_SUCCESS;
        goto Cleanup;
    }

    //
    // Mark device for re-install as needed.
    //
    propertySize = sizeof(configFlags);

    status = _CmGetDeviceRegProp(hPnpCtx,
                                 DeviceInstanceId,
                                 NULL,
                                 CM_REG_PROP_CONFIGFLAGS,
                                 &propertyType,
                                 (PUCHAR)&configFlags,
                                 &propertySize,
                                 0);

    if ((!NT_SUCCESS(status)) ||
        (propertyType != REG_DWORD) ||
        (propertySize != sizeof(configFlags))) {
        configFlags = 0;
    }

    if ((configFlags & CONFIGFLAG_REINSTALL) == 0) {
        configFlags |= CONFIGFLAG_REINSTALL;

        status = _CmSetDeviceRegProp(hPnpCtx,
                                     DeviceInstanceId,
                                     NULL,
                                     CM_REG_PROP_CONFIGFLAGS,
                                     REG_DWORD,
                                     (PUCHAR)&configFlags,
                                     sizeof(configFlags),
                                     0);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

    //
    // Clear any previous device migration rank.
    //
    _PnpSetObjectProperty(hPnpCtx,
                          DeviceInstanceId,
                          PNP_OBJECT_TYPE_DEVICE,
                          NULL,
                          NULL,
                          &DEVPKEY_Device_MigrationRank,
                          DEVPROP_TYPE_EMPTY,
                          NULL,
                          0,
                          0);

Cleanup:
    return TRUE;
}
