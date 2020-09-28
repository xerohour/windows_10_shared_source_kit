/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    start.h

Abstract:

    This module implements starting\restarting a device.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

PNPCTX_REG_ENUM_KEY_ACTION
PiCreateDriverSwDeviceCallback(
    __in_opt HPNPCTX hPnpCtx,
    __in HANDLE DevicesKeyHandle,
    __in PWSTR DeviceName,
    __in_opt PVOID Context
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PipProcessStartPhase1)
#pragma alloc_text(PAGE, PipProcessStartPhase2)
#pragma alloc_text(PAGE, PipProcessStartPhase3)
#pragma alloc_text(PAGE, PipProcessRestartPhase1)
#pragma alloc_text(PAGE, PipProcessRestartPhase2)
#pragma alloc_text(PAGE, PiProcessQueryDeviceState)
#pragma alloc_text(PAGE, PiCreateDriverSwDeviceCallback)
#pragma alloc_text(PAGE, PiCreateDriverSwDevices)
#pragma alloc_text(PAGE, PiDestroyDriverSwDevices)
#pragma alloc_text(PAGE, PnpStartDeviceNode)
#endif // ALLOC_PRAGMA

NTSTATUS
PipProcessStartPhase1 (
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    )
{
    PDEVICE_OBJECT  deviceObject;
    NTSTATUS        status = STATUS_SUCCESS;
    PNP_VETO_TYPE   vetoType;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode->State == DeviceNodeResourcesAssigned);

    //
    // Trace an event indicating which device is being started.  This can help
    // in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceStart_Start(NULL,
                                                1,
                                                PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    //
    // If this device does not support async starts, make sure we dont send the start IRP
    // while some other PNP IRP is pended in the system.
    //
    if (OperationsPending && (DeviceNode->Flags & DNF_ASYNC_START_NOT_SUPPORTED)) {

        status = STATUS_RETRY;
        goto Exit;
    }

    deviceObject = DeviceNode->PhysicalDeviceObject;

    IopUncacheInterfaceInformation(deviceObject, TRUE);

    if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {

        //
        // This is a dock so we a little bit of work before starting it.
        // Take the profile change semaphore. We do this whenever a dock
        // is in our list, even if no query is going to occur.
        //
        PpProfileBeginHardwareProfileTransition(FALSE);

        //
        // Tell the profile code what dock device object may be bringing the
        // new hardware profile online.
        //
        PpProfileIncludeInHardwareProfileTransition(DeviceNode, DOCK_ARRIVING);

        //
        // Ask everyone if this is really a good idea right now.
        //
        status = PpProfileQueryHardwareProfileChange(
            FALSE,
            PROFILE_PERHAPS_IN_PNPEVENT,
            &vetoType,
            NULL
            );
    }

    if (NT_SUCCESS(status)) {

        status = PnpStartDeviceNode(DeviceNode, FALSE, Synchronous);
    }

  Exit:

    //
    // Trace an event indicating which device has finished this phase of being
    // started.  This can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceStart_Stop(NULL,
                                               1,
                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    return status;
}

NTSTATUS
PipProcessStartPhase2(
    __in PDEVICE_NODE     DeviceNode
    )
{
    ULONG       problem = CM_PROB_FAILED_START;
    NTSTATUS    status;

    PAGED_CODE();

    //
    // Trace an event indicating which device is being started.  This can help
    // in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceStart_Start(NULL,
                                                2,
                                                PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    status = DeviceNode->CompletionStatus;
    if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {

        if (NT_SUCCESS(status)) {

            //
            // Commit the current Hardware Profile as necessary.
            //
            PpProfileCommitTransitioningDock(DeviceNode, DOCK_ARRIVING);

        } else {

            PpProfileCancelHardwareProfileTransition();
        }
    }

    if (!NT_SUCCESS(status)) {

        PNP_SAVE_FAILURE_INFO(DeviceNode, DeviceNode->CompletionStatus);

        //
        // Handle certain problems determined by the status code
        //
        switch (status) {

            case STATUS_PNP_REBOOT_REQUIRED:
                problem = CM_PROB_NEED_RESTART;
                break;

            default:
                problem = CM_PROB_FAILED_START;
                break;
        }

        PnpRequestDeviceRemoval(DeviceNode, FALSE, problem, status);

        if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {

            PNP_ASSERT(DeviceNode->DockInfo.DockStatus == DOCK_QUIESCENT);
            IoRequestDeviceEject(DeviceNode->PhysicalDeviceObject);
        }

    } else {

        IopDoDeferredSetInterfaceState(DeviceNode);

        //
        // Reserve legacy resources for the legacy interface and bus number.
        //
        if (!IopBootConfigsReserved && DeviceNode->InterfaceType != InterfaceTypeUndefined) {

            //
            // ISA = EISA.
            //
            if (DeviceNode->InterfaceType == Isa) {

                IopAllocateLegacyBootResources(Eisa, DeviceNode->BusNumber);

            }

            IopAllocateLegacyBootResources(DeviceNode->InterfaceType, DeviceNode->BusNumber);
        }

        //
        // This code path currently doesn't expect any of the above functions
        // to fail. If they do, a removal should be queued and failure should
        // be returned.
        //
        PNP_ASSERT(DeviceNode->State == DeviceNodeStartCompletion);

        PipSetDevNodeState(DeviceNode, DeviceNodeStartPostWork, NULL);
    }

    //
    // Trace an event indicating which device has finished this phase of being
    // started.  This can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceStart_Stop(NULL,
                                               2,
                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    return status;
}

NTSTATUS
PipProcessStartPhase3(
    __in PDEVICE_NODE DeviceNode,
    __in LOGICAL MarkForReenumeration
    )

/*++

Routine Description:

    This function completes the start process of a newly started device node.

Arguments:

    DeviceNode - Supplies a pointer to the device node being started.

    MarkForReenumeration - Specifies whether this device node should be marked
        for reenumeration by setting DNF_REENUMERATE on the devnode.

Return Value:

    NTSTATUS code.

--*/

{
    PDEVICE_OBJECT deviceObject;
    HANDLE handle;
    HPNPOPCTX hPnpOpCtx = NULL;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Trace an event indicating which device is being started.  This can help
    // in diagnosing performance issues.
    //

    EventWriteKMPnPEvt_ProcessDeviceStart_Start(NULL,
                                                3,
                                                PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    status = PiPnpRtlBeginOperation(&hPnpOpCtx);

    if (!NT_SUCCESS(status)) {

        goto Exit;
    }

    deviceObject = DeviceNode->PhysicalDeviceObject;

    if (!(DeviceNode->Flags & DNF_IDS_QUERIED)) {

        PWCHAR compatibleIds;
        ULONG compatibleIdsSize;
        PWCHAR hwIds;
        ULONG hwIdsSize;
        UNICODE_STRING unicodeString;

        //
        // If the DNF_IDS_QUERIED is not set, the device is a reported device.
        // It should already be started.  We need to enumerate its children and
        // ask for the HardwareId and the Compatible ids of the detected device.
        //

        status = PnpDeviceObjectToDeviceInstance(deviceObject,
                                                 &handle,
                                                 KEY_READ);

        if (NT_SUCCESS(status)) {

            PnpQueryHardwareIDs(DeviceNode,
                                &hwIds,
                                &hwIdsSize);

            PnpQueryCompatibleIDs(DeviceNode,
                                  &compatibleIds,
                                  &compatibleIdsSize);

            PnpGenerateDeviceIdsHash(hwIds,
                                     compatibleIds,
                                     &DeviceNode->DeviceIdsHash);

            if (hwIds || compatibleIds) {

                BOOLEAN configDevice = FALSE;
                ULONG flags;
                BOOLEAN idsChanged = FALSE;
                ULONG propertySize;
                ULONG propertyType;
                BOOLEAN saveFlags = FALSE;

                PiLockPnpRegistry(FALSE);

                //
                // Read the current config flags.
                //

                propertySize = sizeof(flags);
                status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                             handle,
                                             CM_REG_PROP_CONFIGFLAGS,
                                             &propertyType,
                                             (PUCHAR)&flags,
                                             &propertySize,
                                             0);
                if ((NT_SUCCESS(status)) &&
                    (propertyType == REG_DWORD) &&
                    (propertySize == sizeof(flags))) {

                    if ((flags & CONFIGFLAG_REINSTALL) &&
                        !(flags & CONFIGFLAG_DISABLED)) {
                        configDevice = TRUE;
                    }
                } else {

                    flags = 0;
                    configDevice = TRUE;
                }

                //
                // If the device isn't marked for finish install, check if the
                // hardware or compatible ids have changed.  If they changed,
                // flag this device for finish install so it gets installed
                // again.
                //

                if (!(flags & CONFIGFLAG_FINISH_INSTALL)) {
                    status = STATUS_SUCCESS;

                    if (hwIds) {

                        //
                        // Check if the hardware ids have changed.
                        //

                        status = PnpCheckDeviceIdsChanged(DeviceNode,
                                                          handle,
                                                          hwIds,
                                                          hwIdsSize,
                                                          TRUE,
                                                          &idsChanged);
                    }

                    if (!idsChanged && compatibleIds) {

                        //
                        // Check if the compatible ids have changed.
                        //

                        status = PnpCheckDeviceIdsChanged(DeviceNode,
                                                          handle,
                                                          compatibleIds,
                                                          compatibleIdsSize,
                                                          FALSE,
                                                          &idsChanged);
                    }

                    if (NT_SUCCESS(status) && (idsChanged)) {
                        flags |= CONFIGFLAG_FINISH_INSTALL;
                        saveFlags = TRUE;
                        configDevice = TRUE;
                    }
                }

                if (hwIds) {

                    _CmSetDeviceRegProp(PiPnpRtlCtx,
                                        PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                        handle,
                                        CM_REG_PROP_HARDWAREID,
                                        REG_MULTI_SZ,
                                        (PUCHAR)hwIds,
                                        hwIdsSize,
                                        0);

                    ExFreePool(hwIds);
                }

                if (compatibleIds) {

                    _CmSetDeviceRegProp(PiPnpRtlCtx,
                                        PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                        handle,
                                        CM_REG_PROP_COMPATIBLEIDS,
                                        REG_MULTI_SZ,
                                        (PUCHAR)compatibleIds,
                                        compatibleIdsSize,
                                        0);

                    ExFreePool(compatibleIds);
                }

                //
                // If we set the finish install flag, then write out the flags.
                //

                if (saveFlags) {

                    _CmSetDeviceRegProp(PiPnpRtlCtx,
                                        PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                        handle,
                                        CM_REG_PROP_CONFIGFLAGS,
                                        REG_DWORD,
                                        (PUCHAR)&flags,
                                        sizeof(flags),
                                        0);
                }

                PiUnlockPnpRegistry();

                //
                // Configure device as required.
                //

                if (configDevice) {
                    PpDevCfgProcessDevice(DeviceNode, handle, 0);
                }
            }

            //
            // Set a property indicating what hardware configuration this
            // device was last used on.
            //

            _PnpSetObjectProperty(PiPnpRtlCtx,
                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                  PNP_OBJECT_TYPE_DEVICE,
                                  handle,
                                  NULL,
                                  &DEVPKEY_Device_HardwareConfigurationIndex,
                                  DEVPROP_TYPE_UINT32,
                                  (PUCHAR)&PnpCurrentHardwareConfigurationIndex,
                                  sizeof(PnpCurrentHardwareConfigurationIndex),
                                  0);

            //
            // Add this device to the computer container since the device is root enumerated.
            //

            RtlInitUnicodeString(&unicodeString, REGSTR_VALUE_ROOT_CONTAINERID);
            RtlGUIDFromString(&unicodeString, &DeviceNode->ContainerID);

            PipSetDevNodeFlags(DeviceNode, DNF_IDS_QUERIED);

            PiLockPnpRegistry(FALSE);

            PiDcUpdateDeviceContainerMembership(PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                handle,
                                                REGSTR_VALUE_ROOT_CONTAINERID);

            PiUnlockPnpRegistry();

            ZwClose(handle);

            //
            // Raise PnpRtl events for the updated NtPlugPlay properties.
            //

            _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                         NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_IS_PRESENT);

            //
            // Notify user mode that this device has been enumerated.  User mode
            // uses this event for device installation as well as keeping its cache
            // up to date.
            //

            PnpSetPlugPlayEvent(&GUID_DEVICE_ENUMERATED,
                                DeviceNode->PhysicalDeviceObject);
        }
    }

    if (PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA)) {

        status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    //
    // Set to FALSE only when doing a shallow enumeration.  This ensures
    // that enumeration requests are not queued for all devnodes unless
    // DNF_REENUMERATE is explicitly set by PiMarkDeviceTreeForReenumeration.
    //

    if (MarkForReenumeration) {
        PipSetDevNodeFlags(DeviceNode, DNF_REENUMERATE);
    }

    PnpQueryAndSaveDeviceNodeCapabilities(DeviceNode);
    status = PiProcessQueryDeviceState(deviceObject);

    //
    // The device has been started, attempt to enumerate the device.
    //

    PnpSetPlugPlayEvent(&GUID_DEVICE_ARRIVAL,
                        DeviceNode->PhysicalDeviceObject);

    //
    // Send a device instance started event
    //

    PnpSetDeviceInstanceStartedEvent(DeviceNode);

    if (!NT_SUCCESS(status)) {

        goto Exit;
    }

    PpvUtilTestStartedPdoStack(deviceObject);
    PipSetDevNodeState( DeviceNode, DeviceNodeStarted, NULL );

    //
    // Check if the device's dependents can be started.
    //

    PnpStartedDeviceNodeDependencyCheck(DeviceNode);

    //
    // Create any persistent child software devices configured for this device.
    //

    PiCreateDriverSwDevices(DeviceNode);

    //
    // Do any necessary auditing of started devices.
    //

    if (SeAuditingPlugAndPlayEvents() != FALSE) {
        PiAuditDeviceStart(PnpGetDeviceInstancePath(DeviceNode));
    }

    status = STATUS_SUCCESS;

Exit:

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    //
    // Trace an event indicating which device has finished this phase of being
    // started.  This can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceStart_Stop(NULL,
                                               3,
                                               PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    return status;
}

NTSTATUS
PipProcessRestartPhase1(
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    )
{
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER (Synchronous);

    PNP_ASSERT(DeviceNode->State == DeviceNodeStopped);

    //
    // Trace an event indicating which device is being restarted.  This can help
    // in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceRestart_Start(NULL,
                                                  1,
                                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    //
    // If this device does not support async starts, make sure we dont send the start IRP
    // while some other PNP IRP is pended in the system.
    //
    if (OperationsPending && (DeviceNode->Flags & DNF_ASYNC_START_NOT_SUPPORTED)) {

        status = STATUS_RETRY;
        goto Exit;
    }

    status = PnpStartDeviceNode(DeviceNode, TRUE, Synchronous);
    if (DeviceNode->Flags & DNF_LOCKED_FOR_REBALANCE) {
        PnpUnlockMountableDevice(DeviceNode->PhysicalDeviceObject);
        PipClearDevNodeFlags(DeviceNode, DNF_LOCKED_FOR_REBALANCE);
    }

  Exit:

    //
    // Trace an event indicating which device has finished this phase of being
    // restarted.  This can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceRestart_Stop(NULL,
                                                 1,
                                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    return status;
}

NTSTATUS
PipProcessRestartPhase2(
    __in PDEVICE_NODE     DeviceNode
    )
{
    ULONG       problem;
    NTSTATUS    status;

    PAGED_CODE();

    //
    // Trace an event indicating which device is being restarted.  This can help
    // in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceRestart_Start(NULL,
                                                  2,
                                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    status = DeviceNode->CompletionStatus;

    if (!NT_SUCCESS(status)) {

        PNP_SAVE_FAILURE_INFO(DeviceNode, status);

        //
        // Handle certain problems determined by the status code
        //
        switch (status) {

            case STATUS_PNP_REBOOT_REQUIRED:
                problem = CM_PROB_NEED_RESTART;
                break;

            default:
                problem = CM_PROB_FAILED_START;
                break;
        }

        PnpRequestDeviceRemoval(DeviceNode, FALSE, problem, status);

        if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {

            PNP_ASSERT(DeviceNode->DockInfo.DockStatus == DOCK_QUIESCENT);
            IoRequestDeviceEject(DeviceNode->PhysicalDeviceObject);
        }

    } else {

        PipSetDevNodeState(DeviceNode, DeviceNodeStarted, NULL);

        //
        // Undo the previous activation call made to PO. A non-stopped rebalance
        // request doesn't go through a query stop, so only Idle device if this
        // is a re-start after a query stop.
        //

        if ((DeviceNode->StateFlags & DNSF_POFX_QUERY_STOP) != 0) {
            PoFxIdleDevice(DeviceNode->PhysicalDeviceObject);
            DeviceNode->StateFlags &= ~DNSF_POFX_QUERY_STOP;
        }

        //
        // Check if the device's dependents can be started.
        //

        PnpStartedDeviceNodeDependencyCheck(DeviceNode);
    }

    //
    // Trace an event indicating which device has finished this phase of being
    // restarted.  This can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessDeviceRestart_Stop(NULL,
                                                 2,
                                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    return status;
}

NTSTATUS
PiProcessQueryDeviceState(
    __in PDEVICE_OBJECT DeviceObject
    )
{
    PDEVICE_NODE deviceNode;
    PNP_DEVICE_STATE deviceState;
    NTSTATUS status;
    ULONG problem;
    HPNPOPCTX hPnpOpCtx = NULL;

    PAGED_CODE();

    deviceNode = DeviceObject->DeviceObjectExtension->DeviceNode;

    //
    // Activate the device prior to sending the query device state.
    //

    PNP_ASSERT((deviceNode->StateFlags & DNSF_POFX_QUERY_DEVICE_STATE) == 0);

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    PoFxActivateDevice(deviceNode->PhysicalDeviceObject);
    deviceNode->StateFlags |= DNSF_POFX_QUERY_DEVICE_STATE;

    //
    // If the device was removed or surprised removed while the work
    // item was queued then ignore it.
    //
    status = IopQueryDeviceState(DeviceObject, &deviceState);

    //
    // Now perform the appropriate action based on the returned state
    //
    if (!NT_SUCCESS(status)) {
        status = STATUS_SUCCESS;
        goto QueryDeviceStateEnd;
    }

    if (deviceState & PNP_DEVICE_DONT_DISPLAY_IN_UI) {

        PipSetDevNodeUserFlags(deviceNode, DNUF_DONT_SHOW_IN_UI);

    } else {

        PipClearDevNodeUserFlags(deviceNode, DNUF_DONT_SHOW_IN_UI);
    }

    if (deviceState & PNP_DEVICE_DISCONNECTED) {

        PipSetDevNodeUserFlags(deviceNode, DNUF_DEVICE_DISCONNECTED);

    } else {

        PipClearDevNodeUserFlags(deviceNode, DNUF_DEVICE_DISCONNECTED);
    }

    if (deviceState & PNP_DEVICE_NOT_DISABLEABLE) {

        if ((deviceNode->UserFlags & DNUF_NOT_DISABLEABLE)==0) {

            //
            // this node itself is not disableable
            //
            PipSetDevNodeUserFlags(deviceNode, DNUF_NOT_DISABLEABLE);

            //
            // propagate up tree
            //
            IopIncDisableableDepends(deviceNode);
        }

    } else {

        if (deviceNode->UserFlags & DNUF_NOT_DISABLEABLE) {

            //
            // this node itself is now disableable
            //
            //
            // check tree
            //
            IopDecDisableableDepends(deviceNode);

            PipClearDevNodeUserFlags(deviceNode, DNUF_NOT_DISABLEABLE);
        }
    }

    //
    // everything here can only be turned on (state set)
    //
    if (((deviceState & (PNP_DEVICE_DISABLED | PNP_DEVICE_REMOVED)) ||
         ((deviceState & PNP_DEVICE_FAILED) &&
          !(deviceState & PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED))) &&
          PnpCheckForActiveDependencies(deviceNode,
                                        PNP_DEPENDENCY_TYPE_INITIALIZATION |
                                        PNP_DEPENDENCY_TYPE_START) != FALSE) {

        //
        // If the device moves to a non-functioning state and has unresolved
        // dependencies, set the problem code to waiting on dependencies and
        // attempt to restart once the dependencies have been satified.
        //

        PnpRequestDeviceRemoval(deviceNode,
                                FALSE,
                                CM_PROB_WAITING_ON_DEPENDENCY,
                                STATUS_SUCCESS);

        status = STATUS_UNSUCCESSFUL;

    } else if (deviceState & (PNP_DEVICE_DISABLED | PNP_DEVICE_REMOVED)) {

        problem = (deviceState & PNP_DEVICE_DISABLED) ?
                  CM_PROB_HARDWARE_DISABLED : CM_PROB_DEVICE_NOT_THERE;

        PnpRequestDeviceRemoval(deviceNode,
                                FALSE,
                                problem,
                                STATUS_SUCCESS);

        status = STATUS_UNSUCCESSFUL;

    } else if (deviceState & PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED) {

        if (deviceState & PNP_DEVICE_FAILED) {
            IopResourceRequirementsChanged(DeviceObject, TRUE);

        } else {

            IopResourceRequirementsChanged(DeviceObject, FALSE);
        }

    } else if (deviceState & PNP_DEVICE_FAILED) {

        PnpRequestDeviceRemoval(deviceNode,
                                FALSE,
                                CM_PROB_FAILED_POST_START,
                                STATUS_SUCCESS);

        status = STATUS_UNSUCCESSFUL;
    }

QueryDeviceStateEnd:

    PoFxIdleDevice(deviceNode->PhysicalDeviceObject);
    deviceNode->StateFlags &= ~DNSF_POFX_QUERY_DEVICE_STATE;

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

typedef struct _PI_CREATE_DRIVER_SWDEVICE_PARAMS {
    PDEVICE_NODE DeviceNode;
    NTSTATUS Status;
} PI_CREATE_DRIVER_SWDEVICE_PARAMS, *PPI_CREATE_DRIVER_SWDEVICE_PARAMS;

const PCWSTR PiDriverSwDeviceEnumeratorName = L"DRIVERENUM";

PNPCTX_REG_ENUM_KEY_ACTION
PiCreateDriverSwDeviceCallback(
    __in_opt HPNPCTX hPnpCtx,
    __in HANDLE DevicesKeyHandle,
    __in PWSTR DeviceName,
    __in_opt PVOID Context
    )

/*++

Routine Description:

    This callback function creates a single driver-supplied child software
    device using the parameters found under the enumerated registry key.

Arguments:

    hPnpCtx - Supplies PnP context handle.

    DevicesKeyHandle - Supplies the handle to the root of the software devices
        key being enumerated.

    DeviceName - Supplies the key name of the software device being enumerated.

    Context - Supplies the software device creation context.

Return Value:

    PNPCTX_REG_ENUM_KEY_NEXT to indicate that next key should be enumerated.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    PPI_CREATE_DRIVER_SWDEVICE_PARAMS deviceParams = (PPI_CREATE_DRIVER_SWDEVICE_PARAMS)Context;
    HANDLE deviceKeyHandle = NULL;
    RTL_QUERY_REGISTRY_TABLE queryParams[7];
    UNICODE_STRING hardwareIds = {0};
    UNICODE_STRING compatibleIds = {0};
    UNICODE_STRING containerIdString = {0};
    GUID containerId;
    ULONG capabilities = 0;
    UNICODE_STRING description = {0};
    UNICODE_STRING locationInfo = {0};
    PDEVICE_NODE deviceNode = NULL;
    UNICODE_STRING unicodeString;
    UNICODE_STRING prefixString;
    ULONG deviceIdsHash = 0;
    ULONG hashValue = 0;
    PWSTR deviceIds[2];
    CM_REG_PROP regProps[2];
    PWSTR parentDeviceIds = NULL;
    PWSTR compatibleId = NULL;
    DEVPROPTYPE propertyType = DEVPROP_TYPE_NULL;
    ULONG propertySize = 0;
    PKEY_VALUE_FULL_INFORMATION keyValueInfo = NULL;
    PSECURITY_DESCRIPTOR securityDescriptor = NULL;
    ULONG securityDescriptorSize = 0;
    UNICODE_STRING instanceId = {0};
    PWSTR uniqueInstanceId = NULL;
    ULONG propertyCount = 0;
    DEVPROPKEY *propertyKeys = NULL;
    DEVPROPERTY *properties = NULL;
    DEVPROPERTY *property = NULL;
    ULONG requiredSize;
    ULONG i;

    PAGED_CODE();

    //
    // Open software device subkey.
    //
    status = _PnpCtxRegOpenKey(hPnpCtx,
                               DevicesKeyHandle,
                               DeviceName,
                               REG_OPTION_NON_VOLATILE,
                               KEY_READ,
                               &deviceKeyHandle);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    //
    // Query software device parameters.
    //
    RtlZeroMemory(queryParams, sizeof(queryParams));

    queryParams[0].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_NOEXPAND | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[0].Name = REGSTR_VALUE_DEVICE_HARDWARE_IDS;
    queryParams[0].EntryContext = &hardwareIds;
    queryParams[0].DefaultType = REG_MULTI_SZ << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    queryParams[1].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_NOEXPAND | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[1].Name = REGSTR_VALUE_DEVICE_COMPATIBLE_IDS;
    queryParams[1].EntryContext = &compatibleIds;
    queryParams[1].DefaultType = REG_MULTI_SZ << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    queryParams[2].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[2].Name = REGSTR_VALUE_DEVICE_CONTAINER_ID;
    queryParams[2].EntryContext = &containerIdString;
    queryParams[2].DefaultType = REG_SZ << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    queryParams[3].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[3].Name = REGSTR_VALUE_DEVICE_CAPABILITIES;
    queryParams[3].EntryContext = &capabilities;
    queryParams[3].DefaultType = REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    queryParams[4].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[4].Name = REGSTR_VALUE_DEVICE_DESCRIPTION;
    queryParams[4].EntryContext = &description;
    queryParams[4].DefaultType = REG_SZ << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    queryParams[5].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK;
    queryParams[5].Name = REGSTR_VALUE_DEVICE_LOCATION_INFO;
    queryParams[5].EntryContext = &locationInfo;
    queryParams[5].DefaultType = REG_SZ << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT;

    status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE | RTL_REGISTRY_OPTIONAL,
                                    (PWSTR)deviceKeyHandle,
                                    queryParams,
                                    NULL,
                                    NULL);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    if ((hardwareIds.Buffer != NULL) &&
        (hardwareIds.Length <= sizeof(UNICODE_NULL))) {
        RtlFreeUnicodeString(&hardwareIds);
    }

    if ((compatibleIds.Buffer != NULL) &&
        (compatibleIds.Length <= sizeof(UNICODE_NULL))) {
        RtlFreeUnicodeString(&compatibleIds);
    }

    if ((containerIdString.Buffer != NULL) &&
        (containerIdString.Length < sizeof(UNICODE_NULL))) {
        RtlFreeUnicodeString(&containerIdString);
    }

    if ((containerIdString.Buffer == NULL) ||
        (!NT_SUCCESS(RtlGUIDFromString(&containerIdString, &containerId)))) {
        RtlZeroMemory(&containerId, sizeof(GUID));
    }

    if ((description.Buffer != NULL) &&
        (description.Length < sizeof(UNICODE_NULL))) {
        RtlFreeUnicodeString(&description);
    }

    if ((locationInfo.Buffer != NULL) &&
        (locationInfo.Length < sizeof(UNICODE_NULL))) {
        RtlFreeUnicodeString(&locationInfo);
    }

    //
    // Software device must at least have hardware and/or compatible IDs.
    //
    if ((hardwareIds.Buffer == NULL) &&
        (compatibleIds.Buffer == NULL)) {
        status = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Verify that software device is not being enumerated by an existing
    // instance of itself, either as a direct parent or as an indirect
    // ancestor, by comparing its devices IDs with those of its ancestors.
    //
    deviceIds[0] = hardwareIds.Buffer;
    deviceIds[1] = compatibleIds.Buffer;

    regProps[0] = CM_REG_PROP_HARDWAREID;
    regProps[1] = CM_REG_PROP_COMPATIBLEIDS;

    for (deviceNode = deviceParams->DeviceNode;
         deviceNode != IopRootDeviceNode;
         deviceNode = deviceNode->Parent) {
        //
        // Skip non-software devices.
        //
        if (deviceNode->PhysicalDeviceObject->DriverObject != PiSwDeviceDriverObject) {
            continue;
        }

        //
        // Skip software devices that were not enumerated by drivers.
        //
        RtlInitUnicodeString(&prefixString, PiSwBusName);

        if (!RtlPrefixUnicodeString(&prefixString,
                                    PnpGetDeviceInstancePath(deviceNode),
                                    TRUE)) {
            continue;
        }

        RtlInitUnicodeString(&unicodeString,
                             PnpGetDeviceInstancePath(deviceNode)->Buffer +
                             (prefixString.Length / sizeof(WCHAR)));

        RtlInitUnicodeString(&prefixString, PiDriverSwDeviceEnumeratorName);

        if ((!RtlPrefixUnicodeString(&prefixString, &unicodeString, TRUE)) ||
            (unicodeString.Buffer[prefixString.Length / sizeof(WCHAR)] != OBJ_NAME_PATH_SEPARATOR)) {
            continue;
        }

        //
        // Generate device IDs hash value on first use.
        //
        if (deviceIdsHash == 0) {
            status = PnpGenerateDeviceIdsHash(hardwareIds.Buffer,
                                              compatibleIds.Buffer,
                                              &deviceIdsHash);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // Include implicit software device compatible IDs in device IDs
            // hash value for comparison purposes.
            //
            if ((capabilities & SWDeviceCapabilitiesDriverRequired) == 0) {
                RtlInitUnicodeString(&unicodeString, PiSwGenericRawCompatibleId);

                status = RtlHashUnicodeString(&unicodeString,
                                              TRUE,
                                              HASH_STRING_ALGORITHM_DEFAULT,
                                              &hashValue);

                if (!NT_SUCCESS(status)) {
                    break;
                }

                deviceIdsHash += hashValue;
            }

            RtlInitUnicodeString(&unicodeString, PiSwGenericCompatibleId);

            status = RtlHashUnicodeString(&unicodeString,
                                          TRUE,
                                          HASH_STRING_ALGORITHM_DEFAULT,
                                          &hashValue);

            if (!NT_SUCCESS(status)) {
                break;
            }

            deviceIdsHash += hashValue;
        }

        //
        // Skip software devices with different device IDs hash values.
        //
        if (deviceNode->DeviceIdsHash != deviceIdsHash) {
            continue;
        }

        //
        // Allocate buffer for parent device IDs on first use.
        //
        if (parentDeviceIds == NULL) {
            propertySize = REGSTR_VAL_MAX_HCID_LEN * sizeof(WCHAR);
            parentDeviceIds = (PWSTR)ExAllocatePool(PagedPool, propertySize);

            if (parentDeviceIds == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
        }

        //
        // Confirm match by checking individual device IDs.
        //
        for (i = 0; i < RTL_NUMBER_OF(deviceIds); i++) {
            //
            // Get next set of parent device IDs.
            //
            requiredSize = propertySize;

            status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                         PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                         NULL,
                                         regProps[i],
                                         &propertyType,
                                         (PUCHAR)parentDeviceIds,
                                         &requiredSize,
                                         0);

            if ((!NT_SUCCESS(status)) ||
                (propertyType != REG_MULTI_SZ) ||
                (requiredSize < sizeof(WCHAR))) {
                *parentDeviceIds = UNICODE_NULL;
                status = STATUS_SUCCESS;
            }

            //
            // Exclude implicit software device compatible IDs. 
            //
            if (i == 1) {
                for (compatibleId = parentDeviceIds;
                     *compatibleId != UNICODE_NULL;
                     compatibleId += wcslen(compatibleId) + 1) {
                    if ((_wcsicmp(compatibleId, PiSwGenericRawCompatibleId) == 0) ||
                        (_wcsicmp(compatibleId, PiSwGenericCompatibleId) == 0)) {
                        *compatibleId = UNICODE_NULL;
                        break;
                    }
                }
            }

            //
            // Compare software device IDs against parent device IDs.
            //
            if (((deviceIds[i] != NULL) &&
                 (!PnpCompareMultiSz(deviceIds[i], parentDeviceIds, TRUE))) ||
                ((deviceIds[i] == NULL) &&
                 (*parentDeviceIds != UNICODE_NULL))) {
                break;
            }
        }

        //
        // Prevent duplicate software device from being enumerated under an
        // ancestor device instance in order to avoid recursive enumeration.
        //
        if (i >= RTL_NUMBER_OF(deviceIds)) {
            status = STATUS_RECURSIVE_DISPATCH;
            break;
        }
    }

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    //
    // Query software device security descriptor.
    //
    status = IopGetRegistryValue(deviceKeyHandle,
                                 REGSTR_VALUE_DEVICE_SECURITY,
                                 0,
                                 &keyValueInfo);

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = STATUS_SUCCESS;
        } else {
            goto Exit;
        }
    } else if ((keyValueInfo->Type != REG_BINARY) ||
               (keyValueInfo->DataLength < SECURITY_DESCRIPTOR_MIN_LENGTH)) {
        status = STATUS_UNSUCCESSFUL;
        goto Exit;

    } else {
        securityDescriptor = (PSECURITY_DESCRIPTOR)KEY_VALUE_DATA(keyValueInfo);
        securityDescriptorSize = keyValueInfo->DataLength;
    }

    //
    // Build unique instance ID.
    //
    if (deviceParams->DeviceNode != IopRootDeviceNode) {
        //
        // Make globally unique instance ID.
        //
        status = PipMakeGloballyUniqueId(deviceParams->DeviceNode->PhysicalDeviceObject,
                                         NULL,
                                         &uniqueInstanceId);

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }

        //
        // Allocate buffer for final instance ID.
        //
        instanceId.Length = 0;
        instanceId.MaximumLength = (USHORT)(wcslen(DeviceName) +
                                            wcslen(uniqueInstanceId) + 2) * sizeof(WCHAR);

        instanceId.Buffer = (RtlAllocateStringRoutine)(instanceId.MaximumLength);

        if (instanceId.Buffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        //
        // Build software device instance ID with the following format:
        //
        // <DeviceName>&<UniqueInstanceId>
        //
        status = RtlUnicodeStringPrintf(&instanceId,
                                        L"%ws&%ws",
                                        DeviceName,
                                        uniqueInstanceId);

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }
    } else {
        //
        // Device name itself is sufficient for uniquely identifying this
        // software device under the root device.
        //
        if (!RtlCreateUnicodeString(&instanceId, DeviceName)) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }
    }

    //
    // Query software device property keys.
    //
    for ( ; ; ) {
        status = _PnpGetGenericStorePropertyKeys(hPnpCtx,
                                                 deviceKeyHandle,
                                                 NULL,
                                                 FALSE,
                                                 propertyKeys,
                                                 propertyCount,
                                                 &requiredSize,
                                                 0);

        if (status != STATUS_BUFFER_TOO_SMALL) {
            propertyCount = requiredSize;
            break;

        } else if (requiredSize <= propertyCount) {
            status = STATUS_INTERNAL_ERROR;
            break;
        }

        if (propertyKeys != NULL) {
            ExFreePool(propertyKeys);
        }

        propertyCount = requiredSize;
        propertyKeys = (DEVPROPKEY *)ExAllocatePool(PagedPool,
                                                    propertyCount * sizeof(DEVPROPKEY));

        if (propertyKeys == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }
    }

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    if (propertyCount > 0) {
        //
        // Allocate buffer for all software device properties.
        //
        properties = (DEVPROPERTY *)ExAllocatePool(PagedPool,
                                                   propertyCount * sizeof(DEVPROPERTY));

        if (properties == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        RtlZeroMemory(properties, propertyCount * sizeof(DEVPROPERTY));

        //
        // Query software device properties into property list.
        //
        for (i = 0; i < propertyCount; i++) {
            //
            // Query next device property.
            //
            property = &properties[i];
            property->CompKey.Key = propertyKeys[i];
            property->CompKey.Store = DEVPROP_STORE_SYSTEM;

            for ( ; ; ) {
                status = _PnpGetGenericStoreProperty(hPnpCtx,
                                                     deviceKeyHandle,
                                                     NULL,
                                                     &propertyKeys[i],
                                                     &property->Type,
                                                     property->Buffer,
                                                     property->BufferSize,
                                                     &requiredSize,
                                                     0);

                if (status != STATUS_BUFFER_TOO_SMALL) {
                    property->BufferSize = requiredSize;
                    break;

                } else if (requiredSize <= property->BufferSize) {
                    status = STATUS_INTERNAL_ERROR;
                    break;
                }

                if (property->Buffer != NULL) {
                    ExFreePool(property->Buffer);
                }

                property->BufferSize = requiredSize;
                property->Buffer = (PUCHAR)ExAllocatePool(PagedPool, requiredSize);

                if (property->Buffer == NULL) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }
            }

            if (!NT_SUCCESS(status)) {
                break;
            }
        }

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }
    }

    //
    // Start asynchronous creation process for software device.
    //
    status = PiSwStartCreate(PiDriverSwDeviceEnumeratorName,
                             PnpGetDeviceInstancePath(deviceParams->DeviceNode)->Buffer,
                             instanceId.Buffer,
                             hardwareIds.Buffer,
                             compatibleIds.Buffer,
                             (containerIdString.Buffer != NULL) ? &containerId : NULL,
                             capabilities,
                             description.Buffer,
                             locationInfo.Buffer,
                             securityDescriptor,
                             securityDescriptorSize,
                             properties,
                             propertyCount);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

Exit:

    //
    // Free software device properties.
    //
    if (properties != NULL) {
        for (i = 0; i < propertyCount; i++) {
            if (properties[i].Buffer != NULL) {
                ExFreePool(properties[i].Buffer);
            }
        }

        ExFreePool(properties);
    }

    if (propertyKeys != NULL) {
        ExFreePool(propertyKeys);
    }

    //
    // Free software device parameters.
    //
    RtlFreeUnicodeString(&instanceId);

    if (uniqueInstanceId != NULL) {
        ExFreePool(uniqueInstanceId);
    }

    RtlFreeUnicodeString(&hardwareIds);
    RtlFreeUnicodeString(&compatibleIds);
    RtlFreeUnicodeString(&containerIdString);
    RtlFreeUnicodeString(&description);
    RtlFreeUnicodeString(&locationInfo);

    if (keyValueInfo != NULL) {
        ExFreePool(keyValueInfo);
    }

    if (parentDeviceIds != NULL) {
        ExFreePool(parentDeviceIds);
    }

    //
    // Close software device subkey.
    //
    if (deviceKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, deviceKeyHandle);
    }

    //
    // Attempt to create as many software devices as possible, while tracking
    // first encountered failure.
    //
    if (NT_SUCCESS(deviceParams->Status)) {
        deviceParams->Status = status;
    }

    return PNPCTX_REG_ENUM_KEY_NEXT;
}

NTSTATUS
PiCreateDriverSwDevices(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function creates any persistent driver-supplied child software devices
    configured for the supplied parent device.

Parameters:

    DeviceNode - Supplies a pointer to the device to create children for.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    HANDLE instanceKeyHandle = NULL;
    HANDLE devicesKeyHandle = NULL;
    PI_CREATE_DRIVER_SWDEVICE_PARAMS deviceParams;

    PAGED_CODE();

    //
    // Open device instance key.
    //
    status = PnpDeviceObjectToDeviceInstance(DeviceNode->PhysicalDeviceObject,
                                             &instanceKeyHandle,
                                             KEY_READ);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    //
    // Open software devices subkey under device instance key.
    //
    status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                               instanceKeyHandle,
                               REGSTR_KEY_DEVICES,
                               REG_OPTION_NON_VOLATILE,
                               KEY_READ,
                               &devicesKeyHandle);

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = STATUS_SUCCESS;
        }

        goto Exit;
    }

    //
    // Enumerate software device subkeys to create child software devices.
    //
    deviceParams.DeviceNode = DeviceNode;
    deviceParams.Status = STATUS_SUCCESS;

    status = _PnpCtxRegEnumKeyWithCallback(PiPnpRtlCtx,
                                           devicesKeyHandle,
                                           PiCreateDriverSwDeviceCallback,
                                           &deviceParams);

    if (!NT_SUCCESS(status)) {
        goto Exit;

    } else if (!NT_SUCCESS(deviceParams.Status)) {
        status = deviceParams.Status;
        goto Exit;
    }

Exit:

    if (devicesKeyHandle != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, devicesKeyHandle);
    }

    if (instanceKeyHandle != NULL) {
        ZwClose(instanceKeyHandle);
    }

    return status;
}

NTSTATUS
PiDestroyDriverSwDevices(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function destroys any persistent driver-supplied child software
    devices created for the supplied parent device.

Parameters:

    DeviceNode - Supplies a pointer to the device to destroy children for.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    //
    // Destroy any driver enumerated software devices.
    //
    status = PiSwStopDestroy(PiDriverSwDeviceEnumeratorName,
                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                             NULL);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

Exit:
    return status;
}

NTSTATUS
PnpStartDeviceNode (
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Restart,
    __in LOGICAL        Synchronous
    )

/*++

Routine Description:

    This function sends a start device irp to the top level device
    object which roots on DeviceObject.

Parameters:

    DeviceObject - Supplies the pointer to the device object of the device
                   being removed.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS status;
    PNP_DEVNODE_STATE completionState;
    PPNP_DEVICE_COMPLETION_REQUEST completionRequest;
    PNP_DIAGNOSTIC_COMPLETION_CONTEXT startContext;

    PAGED_CODE();

    completionState = Restart ? DeviceNodeRestartCompletion : DeviceNodeStartCompletion;

    //
    // Check the Session id property of the device - if its a Per Session device, set the DOE flag accordingly
    //
    if (Restart == FALSE) {
        ULONG DeviceSessionId = INVALID_SESSION;

        DeviceSessionId = IopGetSessionIdFromPDO(DeviceNode->PhysicalDeviceObject);

        if (DeviceSessionId != INVALID_SESSION) {
            // Mark the device tree for this Device with DOE_SESSION_DEVICE
            PpMarkDeviceStackExtensionFlag(DeviceNode->PhysicalDeviceObject, DOE_SESSION_DEVICE, TRUE);

        }
    }

    if (Synchronous == FALSE &&
        (PnpAsyncOptions & PNP_ASYNC_START_ENABLED) &&
        !(DeviceNode->Flags & DNF_ASYNC_START_NOT_SUPPORTED) &&
        !(DeviceNode->PhysicalDeviceObject->Flags & DO_POWER_INRUSH) &&
        !InitSafeBootMode) {

        //
        // Everything says we can start this device asynchronously.
        //
        completionRequest = PnpDeviceCompletionRequestCreate(DeviceNode,
                                                             completionState,
                                                             NULL);
        if (!completionRequest) {

            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "Failed to create DeviceCompletionRequest for %wZ, defaulting to synchronous %s!!!\n",
                         PnpGetDeviceInstancePath(DeviceNode),
                         (Restart)? "restart" : "start"));

            //
            // Start the device synchronously if we could not allocate the completion
            // request.
            //
            goto sync;
        }

        IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                     "%s %wZ asynchronously\n",
                     (Restart)? "Restarting" : "Starting",
                     PnpGetDeviceInstancePath(DeviceNode)));

        PnpDiagnosticTraceObject(&KMPnPEvt_DeviceStart_Start, PnpGetDeviceInstancePath(DeviceNode));

        PipSetDevNodeState(DeviceNode, DeviceNodeStartPending, NULL);

        DeviceNode->CompletionStatus = STATUS_PENDING;

        PnpDeviceCompletionQueueAddDispatchedRequest(&PnpDeviceCompletionQueue,
                                                     &completionRequest->ListEntry);

        status = PnpStartDevice(DeviceNode->PhysicalDeviceObject,
                                PnpDeviceCompletionRoutine,
                                completionRequest);

        if (status != STATUS_PENDING) {

            PnpDeviceCompletionQueueRemoveCompletedRequest(&PnpDeviceCompletionQueue,
                                                           &completionRequest->ListEntry);

            status = PnpDeviceCompletionProcessCompletedRequest(completionRequest);

            IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                         "%s %wZ without pending with status 0x%8.8X <=\n",
                         (Restart)? "Restarted" : "Started",
                         PnpGetDeviceInstancePath(DeviceNode),
                         status));
        } else {

            PnpDiagnosticTraceObject(&KMPnPEvt_DeviceStart_Pend, PnpGetDeviceInstancePath(DeviceNode));
        }

        goto exit;
    }

sync:

   //
   // This device should be started synchronously.
   //

   IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                "%s %wZ synchronously\n",
                (Restart)? "Restarting" : "Starting",
                PnpGetDeviceInstancePath(DeviceNode)));

   PnpDiagnosticTraceObject(&KMPnPEvt_DeviceStart_Start, PnpGetDeviceInstancePath(DeviceNode));

   startContext.Status = STATUS_UNSUCCESSFUL;
   startContext.DriverObject = NULL;
   KeInitializeEvent(&startContext.Event,
                     SynchronizationEvent,
                     FALSE);

   status = PnpStartDevice(DeviceNode->PhysicalDeviceObject,
                           PnpDiagnosticCompletionRoutine,
                           &startContext);
   if (status == STATUS_PENDING) {
       KeWaitForSingleObject(&startContext.Event,
                             Executive,
                             KernelMode,
                             FALSE,
                             (PLARGE_INTEGER)NULL);

       status = startContext.Status;
   }

   if (NT_SUCCESS(status)){
       
       PipUpdatePostStartCharacteristics(DeviceNode->PhysicalDeviceObject);

       //
       // Clear DEVPKEY_Device_DriverProblemDesc
       //

       _PnpSetObjectProperty(PiPnpRtlCtx,
                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                             PNP_OBJECT_TYPE_DEVICE,
                             NULL,
                             NULL,
                             &DEVPKEY_Device_DriverProblemDesc,
                             DEVPROP_TYPE_EMPTY,
                             NULL,
                             0,
                             0);

        //
        // Allow software device creation to enumerate
        // software devices children.
        //

        PiSwProcessParentStartIrp(DeviceNode->PhysicalDeviceObject);
   }

   DeviceNode->CompletionStatus = status;
   PnpTraceStartDevice(DeviceNode, status, startContext.DriverObject);
   PipSetDevNodeState(DeviceNode, completionState, NULL);
   IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                "%s %wZ with status 0x%8.8X\n",
                (Restart)? "Restarted" : "Started",
                PnpGetDeviceInstancePath(DeviceNode),
                status));
exit:

    return status;
}

NTSTATUS
PnpTraceStartDevice (
    __in PDEVICE_NODE DeviceNode,
    __in NTSTATUS Status,
    __in_opt PDRIVER_OBJECT FailingDriver
    )

/*++

Routine Description:

    This function will trace the starting of a device. In case of failure,
    it will trace the failing driver image name and version.

Arguments:

    DeviceNode - Supplies a pointer to the device that got started.

    Status - Supplies status code from the start IRP.

    FailingDriver - Supplies a pointer to the failing driver if any.

Return Value:

    NTSTATUS.

--*/

{

    PUNICODE_STRING driverName;
    ULONG imageVersion;
    PIMAGE_NT_HEADERS header;
    PIMAGE_OPTIONAL_HEADER optionalHeader;

    imageVersion = 0;
    driverName = NULL;

    if (FailingDriver != NULL) {
        driverName = &FailingDriver->DriverName;

        //
        // Get the image version for this driver.
        //

        PNP_ASSERT(FailingDriver->DriverStart != NULL);

        header = RtlImageNtHeader(FailingDriver->DriverStart);
        PNP_ASSERT(header != NULL);
        optionalHeader = &header->OptionalHeader;
        imageVersion = (optionalHeader->MajorImageVersion << 16) |
                        optionalHeader->MinorImageVersion;

    }

    PnpDiagnosticTraceDeviceOperation(&KMPnPEvt_DeviceStart_Stop,
                                      PnpGetDeviceInstancePath(DeviceNode),
                                      Status,
                                      driverName,
                                      imageVersion);

    return STATUS_SUCCESS;
}


