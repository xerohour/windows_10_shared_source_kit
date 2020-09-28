/*++
Copyright (c) Microsoft Corporation

Module Name:

    etw.c

Abstract:

    USB Controller Extension Driver

--*/

#include "pch.h"

#include "etw.tmh"

__drv_sameIRQL
VOID
Etw_EnableCallback(
    __in
        LPCGUID                     SourceId,
    __in
        ULONG                       IsEnabled,
    __in
        UCHAR                       Level,
    __in
        ULONGLONG                   MatchAnyKeyword,
    __in
        ULONGLONG                   MatchAllKeyword,
    __in_opt
        PEVENT_FILTER_DESCRIPTOR    FilterData,
    __inout_opt
        PVOID                       CallbackContext
    )
/*++

Routine Description:

    This EtwEnableCallback function is a driver-supplied callback
    function that is used to receive enable or disable notifications.

Arguments:

    SourceId - Supplies the GUID from the call to EnableTraceEx.

    IsEnabled - Supplies whether tracing is enabled or disabled.

    Level - Supplies the level enabled for the tracing.

    MatchAnyMask - Supplies composite data for match any mask.

    MatchAllMask - Supplies composite data for match all mask.

    FilterData - Supplies the pointer to the arbitrary data to EnableTraceEx.

    CallbackContext - Supplies the context provided to EtwRegister.

Return Value:

    None

--*/
{
    GUID                        activityId;
    PUCXCONTROLLER_PRIVCONTEXT *ucxControllerTable = NULL;
    ULONG                       ucxControllerTableCount;
    ULONG                       ucxControllerTableIndex;
    PUCXCONTROLLER_PRIVCONTEXT  currentUcxControllerContext;
    KIRQL                       irql;

    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(Level);
    UNREFERENCED_PARAMETER(MatchAnyKeyword);
    UNREFERENCED_PARAMETER(MatchAllKeyword);
    UNREFERENCED_PARAMETER(FilterData);
    UNREFERENCED_PARAMETER(CallbackContext);

    TRY {

        ucxControllerTable = NULL;

        if (IsEnabled != 0) {

            RtlZeroMemory(&activityId, sizeof(GUID));

            //
            // Allocate an Activity ID for ETW rundown.
            //
            EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID, &activityId);

            //
            // Start ETW rundown.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_START(&activityId);

            //
            // Get the current UcxController list count.
            //
            KeAcquireSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock, &irql);
            ucxControllerTableCount = g_WdfDriverUcxContext->UcxControllerListCount;
            KeReleaseSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock, irql);

            if (ucxControllerTableCount != 0) {

                WHILE (TRUE) {

                    //
                    // Allocate a table large large enough to contain a
                    // UcxController context entry for each UcxController in
                    // the current list.
                    //
                    ucxControllerTable = ExAllocatePoolWithTag(NonPagedPool,
                        ucxControllerTableCount * sizeof(*ucxControllerTable),
                        UCX_TAG);

                    if (ucxControllerTable == NULL) {
                        LEAVE;
                    }

                    RtlZeroMemory(ucxControllerTable, ucxControllerTableCount * sizeof(*ucxControllerTable));

                    KeAcquireSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock, &irql);

                    if (ucxControllerTableCount < g_WdfDriverUcxContext->UcxControllerListCount) {

                        KeReleaseSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock, irql);

                        //
                        // The current table size is too small.  Free the
                        // current table and reallocate a larger table and
                        // try again.
                        //
                        ExFreePoolWithTag(ucxControllerTable, UCX_TAG);
                        ucxControllerTable = NULL;

                        continue;
                    }

                    ucxControllerTableIndex = 0;

                    FOR_ALL_IN_LIST(UCXCONTROLLER_PRIVCONTEXT,
                                    &g_WdfDriverUcxContext->UcxControllerListHead,
                                    UcxControllerListEntry,
                                    currentUcxControllerContext) {

                        //
                        // Take a reference on the UcxController since we
                        // are going to walk this list of controllers
                        // outside the UcxControllerListLock.
                        //
                        WdfObjectReferenceWithTag(currentUcxControllerContext->UcxController,
                            TAG(Etw_EnableCallback));

                        ucxControllerTable[ucxControllerTableIndex++] = currentUcxControllerContext;
                    }

                    KeReleaseSpinLock(&g_WdfDriverUcxContext->UcxControllerListLock, irql);

                    break;
                }

                //
                // Rundown the table of UcxControllers and release the
                // references on the UcxControllers.
                //
                for (ucxControllerTableIndex = 0;
                     ucxControllerTableIndex < ucxControllerTableCount;
                     ucxControllerTableIndex++) {


                    currentUcxControllerContext = ucxControllerTable[ucxControllerTableIndex];

                    if (currentUcxControllerContext == NULL) {
                        break;
                    }

                    //
                    // Do the ETW rundown of the UcxController.
                    //
                    Etw_UcxControllerRundown(&activityId, currentUcxControllerContext);

                    //
                    // Done with the ETW rundown of the UcxController.
                    // Release the reference on it.
                    //
                    WdfObjectDereferenceWithTag(currentUcxControllerContext->UcxController,
                                                TAG(Etw_EnableCallback));
                }
            }

            //
            // ETW rundown is now complete.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_COMPLETE(&activityId);
        }

    } FINALLY {

        if (ucxControllerTable != NULL) {
            ExFreePoolWithTag(ucxControllerTable, UCX_TAG);
            ucxControllerTable = NULL;
        }
    }

    return;
}

VOID
Etw_UcxControllerRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the UcxController.

    It is assumed that a WdfObjectDereference is held on the
    UcxController by the caller while this routine executes.

Return Value:

    None

--*/
{
    UNICODE_STRING     symbolicLinkNameUnicode;
    PWSTR              symbolicLinkNameString;
    ULONG              counter;
    ULONG              index;
    PSM_ENGINE_CONTEXT smEngineContext;

    TRY {

        //
        // Retrieve the Device Interface String for the host controller
        // GUID_CLASS_USB_HOST_CONTROLLER interface which was created
        // during the time of UcxControllerCreate().
        //
        RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

        WdfStringGetUnicodeString(UcxControllerContext->SymbolicNameForHostControllerInterface,
                                  &symbolicLinkNameUnicode);

        symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
            symbolicLinkNameUnicode.Length + sizeof(WCHAR),
            UCX_TAG);

        //
        // String needs to be NULL-terminated for ETW
        //
        if (symbolicLinkNameString != NULL) {
            RtlCopyMemory(symbolicLinkNameString,
                          symbolicLinkNameUnicode.Buffer,
                          symbolicLinkNameUnicode.Length);

            symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
        }

        //
        // Log the ETW rundown event for this UcxController.
        //
        EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_CONTROLLER_INFORMATION(
            ActivityId,
            UcxControllerContext->UcxController,
            UcxControllerContext->Config.ParentBusType,
            UcxControllerContext->Config.PciDeviceInfo.BusNumber,
            UcxControllerContext->Config.PciDeviceInfo.DeviceNumber,
            UcxControllerContext->Config.PciDeviceInfo.FunctionNumber,
            UcxControllerContext->Config.PciDeviceInfo.VendorId,
            UcxControllerContext->Config.PciDeviceInfo.DeviceId,
            UcxControllerContext->Config.PciDeviceInfo.RevisionId,
            UcxControllerContext->Config.AcpiDeviceInfo.VendorId,
            UcxControllerContext->Config.AcpiDeviceInfo.DeviceId,
            UcxControllerContext->Config.AcpiDeviceInfo.RevisionId,
            symbolicLinkNameString);

        if (symbolicLinkNameString != NULL) {
            ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
            symbolicLinkNameString = NULL;
        }

        smEngineContext = &UcxControllerContext->SmEngineContext;

        //
        // Perform the ETW event rundown for the Controller Reset State machine
        //
        index = smEngineContext->StateHistoryIndex;

        for ( counter = 0; counter <  STATE_HISTORY_DEPTH; counter++) {
            index = (index + STATE_HISTORY_DEPTH - 1) & STATE_HISTORY_MASK;

            //
            // NOTE: This call orders Event and State incorrectly (State comes first in the
            //       manifest). This behavior shipped in Win8 so the parsers just swap the values;
            //       Do not correct the order here or in the manifest.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_CONTROLLER_STATE_MACHINE_TRANSITION(
                ActivityId,
                UcxControllerContext->UcxController,
                smEngineContext,
                smEngineContext->StateHistory[index].Event,
                smEngineContext->StateHistory[index].State);
        }

        //
        // After a full loop index should be same as smEngineContext->StateHistoryIndex
        // If the following NT_ASSERT hits, there is some issue in the above loop
        //
        NT_ASSERT(index == smEngineContext->StateHistoryIndex);

        //
        // Perform a rundown of Event History
        //
        index = smEngineContext->EventHistoryIndex;

        for ( counter = 0; counter <  EVENT_HISTORY_DEPTH; counter++) {
            index = (index + EVENT_HISTORY_DEPTH - 1) & EVENT_HISTORY_MASK;

            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_CONTROLLER_STATE_MACHINE_EVENT(
                ActivityId,
                UcxControllerContext->UcxController,
                smEngineContext,
                smEngineContext->EventHistory[index]);
        }

        //
        // After a full loop index should be same as smEngineContext->EventHistoryIndex
        // If the following NT_ASSERT hits, there is some issue in the above loop
        //
        NT_ASSERT(index == smEngineContext->EventHistoryIndex);

        //
        // Perform the ETW event rundown on the all of the devices
        // downstream of the UcxController.
        //
        Etw_UcxDeviceTreeRundown(ActivityId, UcxControllerContext);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxDeviceTreeRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the all of the child
    devices downstream of the given UcxController.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT              *ucxDeviceTable = NULL;
    ULONG                                   ucxDeviceTableCount;
    ULONG                                   ucxDeviceTableIndex;
    PUCXUSBDEVICE_PRIVCONTEXT               ucxUsbDeviceContext;

    PETW_UCXENDPOINT_TABLE_ENTRY            ucxEndpointTable = NULL;
    ULONG                                   ucxEndpointTableCount;
    ULONG                                   ucxEndpointTableIndex;
    PETW_UCXENDPOINT_TABLE_ENTRY            ucxEndpointTableEntry;

    ETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT  ucxDeviceRundownCallbackContext;
    KIRQL                                   irql;

    TRY {

        //
        // Get the current count of all UcxDevice children of this
        // UcxController, and all of their UcxEndpoints.
        //
        KeAcquireSpinLock(&UcxControllerContext->TopologyLock, &irql);
        ucxDeviceTableCount = UcxControllerContext->ChildListCount;
        ucxEndpointTableCount = UcxControllerContext->ChildEndpointListCount;
        KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

        WHILE (TRUE) {

            if ((ucxDeviceTableCount != 0) && (ucxDeviceTable == NULL)) {

                //
                // Allocate a table large large enough to contain a
                // UcxDevice context entry for each UcxDevice in the
                // current child device tree.
                //
                ucxDeviceTable = ExAllocatePoolWithTag(NonPagedPool,
                    ucxDeviceTableCount * sizeof(*ucxDeviceTable),
                    UCX_TAG);

                if (ucxDeviceTable == NULL) {
                    LEAVE;
                }
            }

            if ((ucxEndpointTableCount != 0) && (ucxEndpointTable == NULL)) {

                //
                // Allocate a table large large enough to contain a
                // UcxEndpoint context entry for each ucxEndpoint in the
                // current child device tree.
                //
                ucxEndpointTable = ExAllocatePoolWithTag(NonPagedPool,
                    ucxEndpointTableCount * sizeof(*ucxEndpointTable),
                    UCX_TAG);

                if (ucxEndpointTable == NULL) {
                    LEAVE;
                }
            }

            KeAcquireSpinLock(&UcxControllerContext->TopologyLock, &irql);

            //
            // Only run down the device tree if UcxRootHubCreate() has
            // successfully executed to completion for this UcxController.
            // Otherwise the device tree does not yet exist.
            //
            if (UcxControllerContext->UcxRootHubContext != NULL) {

                if (ucxDeviceTableCount < UcxControllerContext->ChildListCount) {

                    //
                    // The current table size is too small.  Update the
                    // table size, free the current table, and loop again
                    // to allocate a larger table and try again.
                    //
                    ucxDeviceTableCount = UcxControllerContext->ChildListCount;

                    KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

                    if (ucxDeviceTable != NULL) {
                        ExFreePoolWithTag(ucxDeviceTable, UCX_TAG);
                        ucxDeviceTable = NULL;
                    }
                    continue;
                }

                if (ucxEndpointTableCount < UcxControllerContext->ChildEndpointListCount) {

                    //
                    // The current table size is too small.  Update the
                    // table size, free the current table, and loop again
                    // to allocate a larger table and try again.
                    //
                    ucxEndpointTableCount = UcxControllerContext->ChildEndpointListCount;

                    KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

                    if (ucxEndpointTable != NULL) {
                        ExFreePoolWithTag(ucxEndpointTable, UCX_TAG);
                        ucxEndpointTable = NULL;
                    }
                    continue;
                }

                //
                // The UcxDevice and UcxEndpoint tables have now been
                // allocated large enough to contain the current set in the
                // current child device tree.
                //
                if (ucxDeviceTable != NULL) {
                    RtlZeroMemory(ucxDeviceTable, ucxDeviceTableCount * sizeof(*ucxDeviceTable));
                }
                if (ucxEndpointTable != NULL) {
                    RtlZeroMemory(ucxEndpointTable, ucxEndpointTableCount * sizeof(*ucxEndpointTable));
                }

                //
                // Initialize the callback context.
                //
                ucxDeviceRundownCallbackContext.UcxDeviceTable = ucxDeviceTable;
                ucxDeviceRundownCallbackContext.UcxDeviceTableCount = ucxDeviceTableCount;
                ucxDeviceRundownCallbackContext.UcxDeviceTableIndex = 0;

                ucxDeviceRundownCallbackContext.UcxEndpointTable = ucxEndpointTable;
                ucxDeviceRundownCallbackContext.UcxEndpointTableCount = ucxEndpointTableCount;
                ucxDeviceRundownCallbackContext.UcxEndpointTableIndex = 0;

                //
                // Walk the tree of all devices downstream of this
                // UcxController, including the UcxRootHub device.
                //
                UsbDevice_WalkTree(UcxControllerContext->UcxRootHubContext->UcxUsbDeviceContextOfUcxRootHub,
                                   TRUE,
                                   Etw_UcxDeviceTreeRundownWalkCallback,
                                   &ucxDeviceRundownCallbackContext);

                KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

                break;

            } else {

                //
                // The device tree does not yet exist.  Nothing to run
                // down.
                //
                KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

                LEAVE;
            }
        }

        NT_ASSERT(ucxDeviceRundownCallbackContext.UcxDeviceTableIndex ==
                  ucxDeviceRundownCallbackContext.UcxDeviceTableCount);

        NT_ASSERT(ucxDeviceRundownCallbackContext.UcxEndpointTableIndex ==
                  ucxDeviceRundownCallbackContext.UcxEndpointTableCount);

        //
        // Do the ETW rundown of all of the UcxDevice children.
        //
        for (ucxDeviceTableIndex = 0; ucxDeviceTableIndex < ucxDeviceTableCount; ucxDeviceTableIndex++) {

            ucxUsbDeviceContext = ucxDeviceTable[ucxDeviceTableIndex];

            //
            // Do the ETW rundown of the UcxDevice.
            //
            Etw_UcxDeviceRundown(ActivityId, ucxUsbDeviceContext);

            //
            // Done with the ETW rundown of the UcxDevice.  Release the
            // reference on it.
            //
            WdfObjectDereferenceWithTag(ucxUsbDeviceContext->UcxUsbDevice,
                                        TAG(Etw_UcxDeviceTreeRundownWalkCallback));
        }

        //
        // Do the ETW rundown of all UcxEndpoints of the UcxDevice children.
        //
        for (ucxEndpointTableIndex = 0; ucxEndpointTableIndex < ucxEndpointTableCount; ucxEndpointTableIndex++) {

            ucxEndpointTableEntry = &ucxEndpointTable[ucxEndpointTableIndex];

            //
            // Do the ETW rundown of the UcxEndpoint.
            //
            Etw_UcxEndpointRundown(ActivityId,
                                   ucxEndpointTableEntry->UcxUsbDevice,
                                   ucxEndpointTableEntry->UcxEndpointContext,
                                   ucxEndpointTableEntry->UcxSStreamsContext);

            //
            // Done with the ETW rundown of the UcxEndpoint.  Release the
            // reference on it.
            //
            WdfObjectDereferenceWithTag(ucxEndpointTableEntry->UcxEndpointContext->UcxEndpoint,
                                        TAG(Etw_UcxDeviceTreeRundownWalkCallback));

            if (ucxEndpointTableEntry->UcxSStreamsContext != NULL) {

                WdfObjectDereferenceWithTag(ucxEndpointTableEntry->UcxSStreamsContext->UcxSStreams,
                                            TAG(Etw_UcxDeviceTreeRundownWalkCallback));
            }
        }

    } FINALLY {

        if (ucxDeviceTable != NULL) {
            ExFreePoolWithTag(ucxDeviceTable, UCX_TAG);
            ucxDeviceTable = NULL;
        }

        if (ucxEndpointTable != NULL) {
            ExFreePoolWithTag(ucxEndpointTable, UCX_TAG);
            ucxEndpointTable = NULL;
        }
    }
}

__drv_mustHold(SpinLock)
VOID
Etw_UcxDeviceTreeRundownWalkCallback(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT               UcxUsbDeviceContext,
    __in
        PETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT UcxDeviceRundownCallbackContext
    )
/*++

Routine Description:

Arguments:

    UcxUsbDeviceContext - Context of the UcxUsbDevice

Return Value:

    VOID

Comments:

--*/
{
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXENDPOINT_PRIVCONTEXT    currentUcxEndpointContext;

    TRY {

        NT_ASSERT(UcxDeviceRundownCallbackContext->UcxDeviceTableIndex <
                  UcxDeviceRundownCallbackContext->UcxDeviceTableCount);

        //
        // Take a reference on the UcxUsbDevice since we are going to
        // walk this list of devices outside of the TopologyLock.
        //
        WdfObjectReferenceWithTag(UcxUsbDeviceContext->UcxUsbDevice,
                                  TAG(Etw_UcxDeviceTreeRundownWalkCallback));

        UcxDeviceRundownCallbackContext->UcxDeviceTable[UcxDeviceRundownCallbackContext->UcxDeviceTableIndex++] =
            UcxUsbDeviceContext;

        if (IsTypeRootHub(UcxUsbDeviceContext->UsbDeviceType)) {

            PUCXROOTHUB_PRIVCONTEXT ucxRootHubContext;

            ucxRootHubContext = UcxRootHubGetContext(UCXUSBDEVICE_TO_UCXROOTHUB(UcxUsbDeviceContext->UcxUsbDevice));

            ucxUsbDevice = UCXROOTHUB_TO_UCXUSBDEVICE(ucxRootHubContext->UcxRootHub);

        } else {

            ucxUsbDevice = UcxUsbDeviceContext->UcxUsbDevice;
        }

        //
        // Collect all of the UcxEndpoints for this UcxUsbDevice.
        //
        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT,
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currentUcxEndpointContext) {

            PETW_UCXENDPOINT_TABLE_ENTRY    ucxEndpointTableEntry;

            NT_ASSERT(UcxDeviceRundownCallbackContext->UcxEndpointTableIndex <
                      UcxDeviceRundownCallbackContext->UcxEndpointTableCount);

            ucxEndpointTableEntry = &UcxDeviceRundownCallbackContext->UcxEndpointTable[
                UcxDeviceRundownCallbackContext->UcxEndpointTableIndex++];

            //
            // Take a reference on the UcxEndpoint since we are going to
            // walk this list of endpoints outside of the TopologyLock.
            //
            WdfObjectReferenceWithTag(currentUcxEndpointContext->UcxEndpoint,
                                      TAG(Etw_UcxDeviceTreeRundownWalkCallback));

            ucxEndpointTableEntry->UcxUsbDevice = ucxUsbDevice;
            ucxEndpointTableEntry->UcxEndpointContext = currentUcxEndpointContext;

            if (currentUcxEndpointContext->UcxSStreamsContext != NULL) {

                //
                // Take a reference on the UcxSStreams since we are
                // going to walk this list of endpoints outside of the
                // TopologyLock.
                //
                WdfObjectReferenceWithTag(currentUcxEndpointContext->UcxSStreamsContext->UcxSStreams,
                                          TAG(Etw_UcxDeviceTreeRundownWalkCallback));

                ucxEndpointTableEntry->UcxSStreamsContext = currentUcxEndpointContext->UcxSStreamsContext;
            }
        }

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxDeviceRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the UcxUsbDevice.

    It is assumed that a WdfObjectDereference is held on the
    UcxUsbDevice by the caller while this routine executes.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO      ucxUsbDeviceInfo;

    TRY {

        if (IsTypeRootHub(UcxUsbDeviceContext->UsbDeviceType)) {

            PUCXROOTHUB_PRIVCONTEXT ucxRootHubContext;
            WDFSTRING               symbolicNameWdfString = NULL;
            UNICODE_STRING          symbolicLinkNameUnicode;
            PWSTR                   symbolicLinkNameString = NULL;
            KIRQL                   irql;

            ucxRootHubContext = UcxRootHubGetContext(UCXUSBDEVICE_TO_UCXROOTHUB(UcxUsbDeviceContext->UcxUsbDevice));

            //
            // Retrieve the Device Interface String for the root hub
            // GUID_CLASS_USBHUB interface which was created during the
            // time of RootHub_Pdo_EvtPrepareHardware().
            //
            KeAcquireSpinLock(&ucxRootHubContext->RHPdoInfoLock, &irql);

            if (ucxRootHubContext->RHPdoStarted &&
                NULL != ucxRootHubContext->RHPdoSymbolicName) {
                symbolicNameWdfString = ucxRootHubContext->RHPdoSymbolicName;
                WdfObjectReferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxDeviceRundown));
            }

            KeReleaseSpinLock(&ucxRootHubContext->RHPdoInfoLock, irql);

            if (symbolicNameWdfString != NULL) {

                RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

                WdfStringGetUnicodeString(symbolicNameWdfString,
                                          &symbolicLinkNameUnicode);

                symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                    symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                    UCX_TAG);

                //
                // String needs to be NULL-terminated for ETW
                //
                if (symbolicLinkNameString != NULL) {
                    RtlCopyMemory(symbolicLinkNameString,
                                  symbolicLinkNameUnicode.Buffer,
                                  symbolicLinkNameUnicode.Length);

                    symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
                }

                WdfObjectDereferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxDeviceRundown));
            }

            //
            // Log the ETW rundown event for this UcxRootHub.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_ROOTHUB_INFORMATION(
                ActivityId,
                ucxRootHubContext->UcxControllerContext->UcxController,
                ucxRootHubContext->UcxRootHub,
                symbolicLinkNameString);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
                symbolicLinkNameString = NULL;
            }

        } else {

            ucxUsbDeviceInfo = &UcxUsbDeviceContext->UcxUsbDeviceInfo;

            //
            // Log the ETW rundown event for this UcxUsbDevice.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_DEVICE_INFORMATION(
                ActivityId,
                UcxUsbDeviceContext->UcxControllerContext->UcxController,
                UcxUsbDeviceContext->ParentHub,
                UcxUsbDeviceContext->UcxUsbDevice,
                ucxUsbDeviceInfo->DeviceSpeed,
                ucxUsbDeviceInfo->PortPath.PortPathDepth,
                (unsigned int *)ucxUsbDeviceInfo->PortPath.PortPath);
        }

    } FINALLY {

    }
}

VOID
Etw_UcxEndpointRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        UCXUSBDEVICE                UcxUsbDevice,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext,
    __in_opt
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the UcxEndpoint.

    It is assumed that a WdfObjectDereference is held on the
    UcxEndpoint by the caller while this routine executes.

Return Value:

    None

--*/
{
    ULONG              counter;
    ULONG              index;
    PSM_ENGINE_CONTEXT smEngineContext;

    TRY {

        //
        // Log the ETW rundown event for this UcxEndpoint.
        //
        EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_ENDPOINT_INFORMATION(
            ActivityId,
            UcxUsbDevice,
            UcxEndpointContext->UcxEndpoint,
            &UcxEndpointContext->UcxPipeInfo,
            sizeof(UcxEndpointContext->EndpointDescriptor),
            &UcxEndpointContext->EndpointDescriptor);

        smEngineContext = &UcxEndpointContext->SmEngineContext;

        //
        // Perform the ETW event rundown for the Endpoint Reset State machine
        //
        index = smEngineContext->StateHistoryIndex;

        for ( counter = 0; counter <  STATE_HISTORY_DEPTH; counter++) {
            index = (index + STATE_HISTORY_DEPTH - 1) & STATE_HISTORY_MASK;

            //
            // NOTE: This call orders Event and State incorrectly (State comes first in the
            //       manifest). This behavior shipped in Win8 so the parsers just swap the values;
            //       Do not correct the order here or in the manifest.
            //
            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_ENDPOINT_STATE_MACHINE_TRANSITION(
                ActivityId,
                UcxEndpointContext->UcxEndpoint,
                &UcxEndpointContext->UcxPipeInfo,
                smEngineContext->StateHistory[index].Event,
                smEngineContext->StateHistory[index].State);
        }

        //
        // After a full loop index should be same as smEngineContext->StateHistoryIndex
        // If the following NT_ASSERT hits, there is some issue in the above loop
        //
        NT_ASSERT(index == smEngineContext->StateHistoryIndex);

        //
        // Perform a rundown of Event History
        //
        index = smEngineContext->EventHistoryIndex;

        for ( counter = 0; counter <  EVENT_HISTORY_DEPTH; counter++) {
            index = (index + EVENT_HISTORY_DEPTH - 1) & EVENT_HISTORY_MASK;

            EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_ENDPOINT_STATE_MACHINE_EVENT(
                ActivityId,
                UcxEndpointContext->UcxEndpoint,
                &UcxEndpointContext->UcxPipeInfo,
                smEngineContext->EventHistory[index]);
        }

        //
        // After a full loop index should be same as smEngineContext->EventHistoryIndex
        // If the following NT_ASSERT hits, there is some issue in the above loop
        //
        NT_ASSERT(index == smEngineContext->EventHistoryIndex);

        if (UcxSStreamsContext != NULL) {

            //
            // Log the ETW rundown events for this UcxSStreams
            //
            for (counter = 0; counter < UcxSStreamsContext->NumberOfStreams; counter++) {

                EventWriteUCX_ETW_EVENT_RUNDOWN_UCX_STATIC_STREAM_INFORMATION(
                    ActivityId,
                    UcxUsbDevice,
                    &UcxEndpointContext->UcxPipeInfo,
                    UcxSStreamsContext->StreamContexts[counter].StreamID,
                    &UcxSStreamsContext->StreamContexts[counter].StreamPipeInfo);
            }
        }

    } FINALLY {

    }
}

VOID
Etw_UcxControllerCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the UcxController.

Return Value:

    None

--*/
{
    UNICODE_STRING  symbolicLinkNameUnicode;
    PWSTR           symbolicLinkNameString;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_UCX_CONTROLLER_CREATE()) {

            //
            // Retrieve the Device Interface String for the host controller
            // GUID_CLASS_USB_HOST_CONTROLLER interface which was created
            // during the time of UcxControllerCreate().
            //
            RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

            WdfStringGetUnicodeString(UcxControllerContext->SymbolicNameForHostControllerInterface,
                                      &symbolicLinkNameUnicode);

            symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                UCX_TAG);

            //
            // String needs to be NULL-terminated for ETW
            //
            if (symbolicLinkNameString != NULL) {
                    RtlCopyMemory(symbolicLinkNameString,
                                  symbolicLinkNameUnicode.Buffer,
                                  symbolicLinkNameUnicode.Length);

                    symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
            }

            //
            // Log the ETW Create event for this UcxController.
            //
            EventWriteUCX_ETW_EVENT_UCX_CONTROLLER_CREATE(
                ActivityId,
                UcxControllerContext->UcxController,
                UcxControllerContext->Config.ParentBusType,
                UcxControllerContext->Config.PciDeviceInfo.BusNumber,
                UcxControllerContext->Config.PciDeviceInfo.DeviceNumber,
                UcxControllerContext->Config.PciDeviceInfo.FunctionNumber,
                UcxControllerContext->Config.PciDeviceInfo.VendorId,
                UcxControllerContext->Config.PciDeviceInfo.DeviceId,
                UcxControllerContext->Config.PciDeviceInfo.RevisionId,
                UcxControllerContext->Config.AcpiDeviceInfo.VendorId,
                UcxControllerContext->Config.AcpiDeviceInfo.DeviceId,
                UcxControllerContext->Config.AcpiDeviceInfo.RevisionId,
                symbolicLinkNameString);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
                symbolicLinkNameString = NULL;
            }
        }

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxControllerDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the UcxController.

Return Value:

    None

--*/
{
    UNICODE_STRING  symbolicLinkNameUnicode;
    PWSTR           symbolicLinkNameString = NULL;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_UCX_CONTROLLER_DELETE()) {

            //
            // Retrieve the Device Interface String for the host controller
            // GUID_CLASS_USB_HOST_CONTROLLER interface which was created
            // during the time of UcxControllerCreate().
            //
            if (NULL != UcxControllerContext->SymbolicNameForHostControllerInterface) {

                RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

                WdfStringGetUnicodeString(UcxControllerContext->SymbolicNameForHostControllerInterface,
                                          &symbolicLinkNameUnicode);

                symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                    symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                    UCX_TAG);

                //
                // String needs to be NULL-terminated for ETW
                //
                if (symbolicLinkNameString != NULL) {
                    RtlCopyMemory(symbolicLinkNameString,
                                  symbolicLinkNameUnicode.Buffer,
                                  symbolicLinkNameUnicode.Length);

                    symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
                }
            }

            //
            // Log the ETW Delete event for this UcxController.
            //
            EventWriteUCX_ETW_EVENT_UCX_CONTROLLER_DELETE(
                ActivityId,
                UcxControllerContext->UcxController,
                UcxControllerContext->Config.ParentBusType,
                UcxControllerContext->Config.PciDeviceInfo.BusNumber,
                UcxControllerContext->Config.PciDeviceInfo.DeviceNumber,
                UcxControllerContext->Config.PciDeviceInfo.FunctionNumber,
                UcxControllerContext->Config.PciDeviceInfo.VendorId,
                UcxControllerContext->Config.PciDeviceInfo.DeviceId,
                UcxControllerContext->Config.PciDeviceInfo.RevisionId,
                UcxControllerContext->Config.AcpiDeviceInfo.VendorId,
                UcxControllerContext->Config.AcpiDeviceInfo.DeviceId,
                UcxControllerContext->Config.AcpiDeviceInfo.RevisionId,
                symbolicLinkNameString);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
                symbolicLinkNameString = NULL;
            }
        }

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxControllerStateMachineTransition(
    __in_opt
        LPCGUID               ActivityId,
    __in
        UCXCONTROLLER         Controller,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in
        SM_ENGINE_EVENT       Event,
    __in
        SM_ENGINE_STATE       State
    )
{
    //
    // Log the ETW state transition for this UcxController.
    //
    // NOTE: This call orders Event and State incorrectly (State comes first in the
    //       manifest). This behavior shipped in Win8 so the parsers just swap the values;
    //       Do not correct the order here or in the manifest.
    //
    EventWriteUCX_ETW_EVENT_UCX_CONTROLLER_STATE_MACHINE_TRANSITION(
        ActivityId,
        Controller,
        SmEngineContext,
        Event,
        State);
}

VOID
Etw_UcxControllerStateMachineEvent(
    __in_opt
        LPCGUID               ActivityId,
    __in
        UCXCONTROLLER         Controller,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in
        SM_ENGINE_EVENT       Event
    )
{
    //
    // Log the ETW state Event for this UcxController.
    //
    EventWriteUCX_ETW_EVENT_UCX_CONTROLLER_STATE_MACHINE_EVENT(
        ActivityId,
        Controller,
        SmEngineContext,
        Event);
}

VOID
Etw_UcxRootHubCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the UcxRootHub.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Create event for this UcxRootHub.
        //
        EventWriteUCX_ETW_EVENT_UCX_ROOTHUB_CREATE(
            ActivityId,
            UcxRootHubContext->UcxControllerContext->UcxController,
            UcxRootHubContext->UcxRootHub,
            NULL);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxRootHubDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the UcxRootHub.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Delete event for this UcxRootHub.
        //
        EventWriteUCX_ETW_EVENT_UCX_ROOTHUB_DELETE(
            ActivityId,
            UcxRootHubContext->UcxControllerContext->UcxController,
            UcxRootHubContext->UcxRootHub,
            NULL);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxRootHubPrepare(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    )
/*++

Routine Description:

    This routine writes an ETW Prepare event for the UcxRootHub.

Return Value:

    None

--*/
{
    WDFSTRING               symbolicNameWdfString = NULL;
    UNICODE_STRING          symbolicLinkNameUnicode;
    PWSTR                   symbolicLinkNameString = NULL;
    KIRQL                   irql;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_UCX_ROOTHUB_PREPARE()) {

            //
            // Retrieve the Device Interface String for the root hub
            // GUID_CLASS_USBHUB interface which was created during the
            // time of RootHub_Pdo_EvtPrepareHardware().
            //
            KeAcquireSpinLock(&UcxRootHubContext->RHPdoInfoLock, &irql);

            if (UcxRootHubContext->RHPdoStarted &&
                NULL != UcxRootHubContext->RHPdoSymbolicName) {
                symbolicNameWdfString = UcxRootHubContext->RHPdoSymbolicName;
                WdfObjectReferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxRootHubPrepare));
            }

            KeReleaseSpinLock(&UcxRootHubContext->RHPdoInfoLock, irql);

            if (symbolicNameWdfString != NULL) {

                RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

                WdfStringGetUnicodeString(symbolicNameWdfString,
                                          &symbolicLinkNameUnicode);

                symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                    symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                    UCX_TAG);

                //
                // String needs to be NULL-terminated for ETW
                //
                if (symbolicLinkNameString != NULL) {
                    RtlCopyMemory(symbolicLinkNameString,
                                  symbolicLinkNameUnicode.Buffer,
                                  symbolicLinkNameUnicode.Length);

                    symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
                }

                WdfObjectDereferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxRootHubPrepare));
            }

            //
            // Log the ETW Prepare event for this UcxRootHub.
            //
            EventWriteUCX_ETW_EVENT_UCX_ROOTHUB_PREPARE(
                ActivityId,
                UcxRootHubContext->UcxControllerContext->UcxController,
                UcxRootHubContext->UcxRootHub,
                symbolicLinkNameString);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
                symbolicLinkNameString = NULL;
            }
        }

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxRootHubRelease(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    )
/*++

Routine Description:

    This routine writes an ETW Release event for the UcxRootHub.

Return Value:

    None

--*/
{
    WDFSTRING               symbolicNameWdfString = NULL;
    UNICODE_STRING          symbolicLinkNameUnicode;
    PWSTR                   symbolicLinkNameString = NULL;
    KIRQL                   irql;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_UCX_ROOTHUB_RELEASE()) {

            //
            // Retrieve the Device Interface String for the root hub
            // GUID_CLASS_USBHUB interface which was created during the
            // time of RootHub_Pdo_EvtPrepareHardware().
            //
            KeAcquireSpinLock(&UcxRootHubContext->RHPdoInfoLock, &irql);

            if (UcxRootHubContext->RHPdoStarted &&
                NULL != UcxRootHubContext->RHPdoSymbolicName) {
                symbolicNameWdfString = UcxRootHubContext->RHPdoSymbolicName;
                WdfObjectReferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxRootHubRelease));
            }

            KeReleaseSpinLock(&UcxRootHubContext->RHPdoInfoLock, irql);

            if (symbolicNameWdfString != NULL) {

                RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

                WdfStringGetUnicodeString(symbolicNameWdfString,
                                          &symbolicLinkNameUnicode);

                symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                    symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                    UCX_TAG);

                //
                // String needs to be NULL-terminated for ETW
                //
                if (symbolicLinkNameString != NULL) {
                    RtlCopyMemory(symbolicLinkNameString,
                                  symbolicLinkNameUnicode.Buffer,
                                  symbolicLinkNameUnicode.Length);

                    symbolicLinkNameString[symbolicLinkNameUnicode.Length / sizeof(WCHAR)] = L'\0';
                }

                WdfObjectDereferenceWithTag(symbolicNameWdfString, TAG(Etw_UcxRootHubRelease));
            }

            //
            // Log the ETW Release event for this UcxRootHub.
            //
            EventWriteUCX_ETW_EVENT_UCX_ROOTHUB_RELEASE(
                ActivityId,
                UcxRootHubContext->UcxControllerContext->UcxController,
                UcxRootHubContext->UcxRootHub,
                symbolicLinkNameString);

            if (symbolicLinkNameString != NULL) {
                ExFreePoolWithTag(symbolicLinkNameString, UCX_TAG);
                symbolicLinkNameString = NULL;
            }
        }

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxDeviceCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the UcxUsbDevice.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO      ucxUsbDeviceInfo;

    TRY {

        ucxUsbDeviceInfo = &UcxUsbDeviceContext->UcxUsbDeviceInfo;

        //
        // Log the ETW Create event for this UcxUsbDevice.
        //
        EventWriteUCX_ETW_EVENT_UCX_DEVICE_CREATE(
            ActivityId,
            UcxUsbDeviceContext->UcxControllerContext->UcxController,
            UcxUsbDeviceContext->ParentHub,
            UcxUsbDeviceContext->UcxUsbDevice,
            ucxUsbDeviceInfo->DeviceSpeed,
            ucxUsbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)ucxUsbDeviceInfo->PortPath.PortPath);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxDeviceDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the UcxUsbDevice.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO      ucxUsbDeviceInfo;

    TRY {

        ucxUsbDeviceInfo = &UcxUsbDeviceContext->UcxUsbDeviceInfo;

        //
        // Log the ETW Delete event for this UcxUsbDevice.
        //
        EventWriteUCX_ETW_EVENT_UCX_DEVICE_DELETE(
            ActivityId,
            UcxUsbDeviceContext->UcxControllerContext->UcxController,
            UcxUsbDeviceContext->ParentHub,
            UcxUsbDeviceContext->UcxUsbDevice,
            ucxUsbDeviceInfo->DeviceSpeed,
            ucxUsbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)ucxUsbDeviceInfo->PortPath.PortPath);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxEndpointCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the UcxEndpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Create event for this UcxEndpoint.
        //
        EventWriteUCX_ETW_EVENT_UCX_ENDPOINT_CREATE(
            ActivityId,
            UcxEndpointContext->UcxUsbDeviceContext->UcxUsbDevice,
            UcxEndpointContext->UcxEndpoint,
            &UcxEndpointContext->UcxPipeInfo,
            sizeof(UcxEndpointContext->EndpointDescriptor),
            &UcxEndpointContext->EndpointDescriptor);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxEndpointDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the UcxEndpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Delete event for this UcxEndpoint.
        //
        EventWriteUCX_ETW_EVENT_UCX_ENDPOINT_DELETE(
            ActivityId,
            UcxEndpointContext->UcxUsbDeviceContext->UcxUsbDevice,
            UcxEndpointContext->UcxEndpoint,
            &UcxEndpointContext->UcxPipeInfo,
            sizeof(UcxEndpointContext->EndpointDescriptor),
            &UcxEndpointContext->EndpointDescriptor);

    } FINALLY {

    }

    return;
}

VOID
Etw_UcxEndpointStateMachineTransition(
    __in_opt
        LPCGUID               ActivityId,
    __in
        UCXENDPOINT           Endpoint,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in
        SM_ENGINE_EVENT       Event,
    __in
        SM_ENGINE_STATE       State
    )
{
    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;

    TRY {

        ucxEndpointContext = CONTAINING_RECORD(SmEngineContext,
                                               UCXENDPOINT_PRIVCONTEXT,
                                               SmEngineContext);
        //
        // Log the ETW state transition for this UcxEndpoint.
        //
        // NOTE: This call orders Event and State incorrectly (State comes first in the
        //       manifest). This behavior shipped in Win8 so the parsers just swap the values;
        //       Do not correct the order here or in the manifest.
        //
        EventWriteUCX_ETW_EVENT_UCX_ENDPOINT_STATE_MACHINE_TRANSITION(
            ActivityId,
            Endpoint,
            &ucxEndpointContext->UcxPipeInfo,
            Event,
            State);

    } FINALLY {

    }
}

VOID
Etw_UcxEndpointStateMachineEvent(
    __in_opt
        LPCGUID               ActivityId,
    __in
        UCXENDPOINT           Endpoint,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in
        SM_ENGINE_EVENT       Event
    )
{

    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;

    TRY {

        ucxEndpointContext = CONTAINING_RECORD(SmEngineContext,
                                               UCXENDPOINT_PRIVCONTEXT,
                                               SmEngineContext);

        //
        // Log the ETW state Event for this UcxEndpoint.
        //
        EventWriteUCX_ETW_EVENT_UCX_ENDPOINT_STATE_MACHINE_EVENT(
            ActivityId,
            Endpoint,
            &ucxEndpointContext->UcxPipeInfo,
            Event);

    } FINALLY {
    }
}

VOID
Etw_UcxStaticStreamsCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the UcxStaticStreams.

Return Value:

    None

--*/
{
    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;
    UCXUSBDEVICE                ucxUsbDevice;

    ULONG   i;

    TRY {

        ucxEndpointContext = UcxSStreamsContext->UcxEndpointContext;
        ucxUsbDevice = ucxEndpointContext->UcxUsbDeviceContext->UcxUsbDevice;

        //
        // Log the ETW Create event for this UcxStaticStreams.
        //
        for (i = 0; i < UcxSStreamsContext->NumberOfStreams; i++) {

            EventWriteUCX_ETW_EVENT_UCX_STATIC_STREAM_CREATE(
                ActivityId,
                ucxUsbDevice,
                &ucxEndpointContext->UcxPipeInfo,
                UcxSStreamsContext->StreamContexts[i].StreamID,
                &UcxSStreamsContext->StreamContexts[i].StreamPipeInfo);
        }

    } FINALLY {

    }
}

VOID
Etw_UcxStaticStreamsDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the UcxStaticStreams.

Return Value:

    None

--*/
{
    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;
    UCXUSBDEVICE                ucxUsbDevice;

    ULONG   i;

    TRY {

        ucxEndpointContext = UcxSStreamsContext->UcxEndpointContext;
        ucxUsbDevice = ucxEndpointContext->UcxUsbDeviceContext->UcxUsbDevice;

        //
        // Log the ETW Delete event for this UcxStaticStreams.
        //
        for (i = 0; i < UcxSStreamsContext->NumberOfStreams; i++) {

            EventWriteUCX_ETW_EVENT_UCX_STATIC_STREAM_DELETE(
                ActivityId,
                ucxUsbDevice,
                &ucxEndpointContext->UcxPipeInfo,
                UcxSStreamsContext->StreamContexts[i].StreamID,
                &UcxSStreamsContext->StreamContexts[i].StreamPipeInfo);
        }

    } FINALLY {

    }
}

VOID
Etw_UcxRootHubInitiatingWake(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    )
/*++

Routine Description:

    This routine writes an ETW Iniaiting Wake event for the UcxRootHub.

Return Value:

    None

--*/
{

    TRY {

        if (EventEnabledUCX_ETW_EVENT_UCX_ROOTHUB_INITIATING_WAKE()) {

            //
            // Log the ETW Initiating Wake event for this UcxRootHub.
            //
            EventWriteUCX_ETW_EVENT_UCX_ROOTHUB_INITIATING_WAKE(
                ActivityId,
                UcxRootHubContext->UcxControllerContext->UcxController,
                UcxRootHubContext->UcxRootHub,
                NULL);

        }

    } FINALLY {

    }

    return;
}

VOID
Etw_XrbComplete(
    __in
        PIRP            Irp,
    __in
        PTRANSFER_URB   TransferUrb
    )
{
    GUID                        activityId;
    NTSTATUS                    activityIdStatus;
    UCXCONTROLLER               ucxController;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    PURB                        urb;
    ULONG                       transferBufferLength;
    ULONG                       partialTransferBufferLength;
    PVOID                       transferBufferSystemAddress;

    TRY {

        //
        // This conditional is essentially checking to see if the any of
        // the following keyword combinations are enabled:
        //
        //  "HeadersBusTrace"
        //  "PartialDataBusTrace"
        //  "FullDataBusTrace"
        //
        if (EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER() ||
            EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA() ||
            EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            urb = (PURB) TransferUrb;

            ucxUsbDevice = (UCXUSBDEVICE) urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            transferBufferLength = 0;
            partialTransferBufferLength = 0;
            transferBufferSystemAddress = NULL;

            //
            // This conditional is essentially checking to see if the any of
            // the following keyword combinations are enabled:
            //
            //  "PartialDataBusTrace"
            //  "FullDataBusTrace"
            //
            if (EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA() ||
                EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA()) {

                transferBufferLength = TransferUrb->TransferBufferLength;

                if (transferBufferLength != 0) {

                    if (TransferUrb->TransferBuffer != NULL) {
                        transferBufferSystemAddress = TransferUrb->TransferBuffer;
                    } else {
                        transferBufferSystemAddress = MmGetSystemAddressForMdlSafe(TransferUrb->TransferBufferMDL,
                                                                                   NormalPagePriority);
                        //
                        // In the case where the transfer buffer is
                        // specified by a chained MDL only the portion
                        // of the transfer buffer spanned by the first
                        // MDL will be logged.  Logging portions of the
                        // transfer buffer spanned by multiple MDLs
                        // would require double buffering the transfer
                        // buffer before writing the ETW event.
                        //
                        if (transferBufferLength > MmGetMdlByteCount(TransferUrb->TransferBufferMDL)) {
                            transferBufferLength = MmGetMdlByteCount(TransferUrb->TransferBufferMDL);
                        }
                    }

                    if (transferBufferSystemAddress == NULL) {
                        transferBufferLength = 0;
                    } else {
                        if (transferBufferLength > MAXIMUM_FULL_DATA_TRANSFER_LENGTH) {
                            transferBufferLength = MAXIMUM_FULL_DATA_TRANSFER_LENGTH;
                        }
                        if (transferBufferLength > MAXIMUM_PARTIAL_DATA_TRANSFER_LENGTH) {
                            partialTransferBufferLength = MAXIMUM_PARTIAL_DATA_TRANSFER_LENGTH;
                        } else {
                            partialTransferBufferLength = transferBufferLength;
                        }
                    }
                }
            }

            if (urb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER ||
                urb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL) {

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status);

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)partialTransferBufferLength,
                    transferBufferSystemAddress);

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)transferBufferLength,
                    transferBufferSystemAddress);

            } else if (TransferUrb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER ||
                       TransferUrb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL) {

                USHORT  packetCount;

                packetCount = (USHORT) urb->UrbIsochronousTransfer.NumberOfPackets;

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbIsochronousTransfer.PipeHandle,
                    Irp,
                    urb,
                    packetCount,

                    //
                    // Size of the base URB_ISOCH_TRANSFER without the
                    // IsoPacket[] array, and the pointer to the base of the
                    // URB_ISOCH_TRANSFER.
                    //
                    FIELD_OFFSET(struct _URB_ISOCH_TRANSFER, IsoPacket),
                    urb,

                    //
                    // Size of an IsoPacket[] array element, and the
                    // pointer to the base of the IsoPacket[] array.
                    //
                    sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),
                    &urb->UrbIsochronousTransfer.IsoPacket[0],

                    Irp->IoStatus.Status);

                //
                // TODO: PostWin8: USBPORT does not currently log Isoch
                // transfer data buffers either and there have been no
                // external requests for such a feature.
                //
                // EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER_PARTIAL_DATA()
                // EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER_FULL_DATA()
                //
            }
        }

    } FINALLY {

    }
}

VOID
Etw_UrbComplete(
    __in
        PIRP            Irp,
    __in
        PTRANSFER_URB   TransferUrb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    PURB                        urb;
    UCXCONTROLLER               ucxController;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    ULONG                       transferBufferLength;
    ULONG                       partialTransferBufferLength;
    PVOID                       transferBufferSystemAddress;

    TRY {

        //
        // This conditional is essentially checking to see if the any of
        // the following keyword combinations are enabled:
        //
        //  "HeadersBusTrace"
        //  "PartialDataBusTrace FullDataBusTrace"
        //  "PartialDataBusTrace"
        //  "FullDataBusTrace"
        //
        if (EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER() ||
            EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_DATA() ||
            EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA() ||
            EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            urb = (PURB) TransferUrb;

            ucxUsbDevice = (UCXUSBDEVICE) urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            transferBufferLength = 0;
            partialTransferBufferLength = 0;
            transferBufferSystemAddress = NULL;

            //
            // This conditional is essentially checking to see if the any of
            // the following keyword combinations are enabled:
            //
            //  "PartialDataBusTrace FullDataBusTrace"
            //  "PartialDataBusTrace"
            //  "FullDataBusTrace"
            //
            if (EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_DATA() ||
                EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA() ||
                EventEnabledUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA()) {

                transferBufferLength = TransferUrb->TransferBufferLength;

                if (transferBufferLength != 0) {

                    if (TransferUrb->TransferBuffer != NULL) {
                        transferBufferSystemAddress = TransferUrb->TransferBuffer;
                    } else {
                        transferBufferSystemAddress = MmGetSystemAddressForMdlSafe(TransferUrb->TransferBufferMDL,
                                                                                   NormalPagePriority);
                    }

                    if (transferBufferSystemAddress == NULL) {
                        transferBufferLength = 0;
                    } else {
                        if (transferBufferLength > MAXIMUM_FULL_DATA_TRANSFER_LENGTH) {
                            transferBufferLength = MAXIMUM_FULL_DATA_TRANSFER_LENGTH;
                        }
                        if (transferBufferLength > MAXIMUM_PARTIAL_DATA_TRANSFER_LENGTH) {
                            partialTransferBufferLength = MAXIMUM_PARTIAL_DATA_TRANSFER_LENGTH;
                        } else {
                            partialTransferBufferLength = transferBufferLength;
                        }
                    }
                }
            }

            if (TransferUrb->Hdr.Function == URB_FUNCTION_CONTROL_TRANSFER) {

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_CONTROL_TRANSFER),
                    urb,
                    Irp->IoStatus.Status);

                //
                // The full data transfer length is used for both
                // PartialDataBusTrace and FullDataBusTrace for Control
                // transfer requests.
                //
                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_CONTROL_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)transferBufferLength,
                    transferBufferSystemAddress);

            } else if (TransferUrb->Hdr.Function == URB_FUNCTION_CONTROL_TRANSFER_EX) {

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_EX(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbControlTransferEx.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_CONTROL_TRANSFER_EX),
                    urb,
                    Irp->IoStatus.Status);

                //
                // The full data transfer length is used for both
                // PartialDataBusTrace and FullDataBusTrace for Control
                // transfer requests.
                //
                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_CONTROL_TRANSFER_EX_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_CONTROL_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)transferBufferLength,
                    transferBufferSystemAddress);

            } else if (TransferUrb->Hdr.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER) {

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status);

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_PARTIAL_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)partialTransferBufferLength,
                    transferBufferSystemAddress);

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_FULL_DATA(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbBulkOrInterruptTransfer.PipeHandle,
                    Irp,
                    urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                    urb,
                    Irp->IoStatus.Status,
                    (USHORT)transferBufferLength,
                    transferBufferSystemAddress);

            } else if (TransferUrb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER) {

                USHORT  packetCount;

                packetCount = (USHORT) urb->UrbIsochronousTransfer.NumberOfPackets;

                EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    urb->UrbIsochronousTransfer.PipeHandle,
                    Irp,
                    urb,
                    packetCount,

                    //
                    // Size of the base URB_ISOCH_TRANSFER without the
                    // IsoPacket[] array, and the pointer to the base of the
                    // URB_ISOCH_TRANSFER.
                    //
                    FIELD_OFFSET(struct _URB_ISOCH_TRANSFER, IsoPacket),
                    urb,

                    //
                    // Size of an IsoPacket[] array element, and the
                    // pointer to the base of the IsoPacket[] array.
                    //
                    sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),
                    &urb->UrbIsochronousTransfer.IsoPacket[0],

                    Irp->IoStatus.Status);

                //
                // TODO: PostWin8: USBPORT does not currently log Isoch
                // transfer data buffers either and there have been no
                // external requests for such a feature.
                //
                // EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER_PARTIAL_DATA()
                // EventWriteUCX_ETW_EVENT_COMPLETE_URB_FUNCTION_ISOCH_TRANSFER_FULL_DATA()
                //
            }
        }

    } FINALLY {

    }
}

VOID
Etw_ControlTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbControlTransfer.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_CONTROL_TRANSFER),
                Urb);
        }

    } FINALLY {

    }
}

VOID
Etw_ControlTransferExUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER_EX()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER_EX(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbControlTransferEx.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_CONTROL_TRANSFER_EX),
                Urb);
        }

    } FINALLY {

    }
}

VOID
Etw_BulkTransferXrbDispatch(
    __in
        PIRP            Irp,
    __in
        PURB            Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

            if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER()) {

                activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

                ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
                ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
                ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

                if (Urb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER ||
                    Urb->UrbHeader.Function == URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL) {

                    EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER(
                        NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                        ucxController,
                        ucxUsbDevice,
                        Urb->UrbBulkOrInterruptTransfer.PipeHandle,
                        Irp,
                        Urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                        Urb);

                } else {

                    USHORT  packetCount;

                    packetCount = (USHORT) Urb->UrbIsochronousTransfer.NumberOfPackets;

                    EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_ISOCH_TRANSFER(
                        NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                        ucxController,
                        ucxUsbDevice,
                        Urb->UrbIsochronousTransfer.PipeHandle,
                        Irp,
                        Urb,
                        packetCount,

                        //
                        // Size of the base URB_ISOCH_TRANSFER without the
                        // IsoPacket[] array, and the pointer to the base of the
                        // URB_ISOCH_TRANSFER.
                        //
                        FIELD_OFFSET(struct _URB_ISOCH_TRANSFER, IsoPacket),
                        Urb,

                        //
                        // Size of an IsoPacket[] array element, and the
                        // pointer to the base of the IsoPacket[] array.
                        //
                        sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),
                        &Urb->UrbIsochronousTransfer.IsoPacket[0]);
                }
            }

    } FINALLY {

    }
}

VOID
Etw_BulkTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbBulkOrInterruptTransfer.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                Urb);
        }

    } FINALLY {

    }
}

VOID
Etw_IsochTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    USHORT                      packetCount;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_ISOCH_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            packetCount = (USHORT) Urb->UrbIsochronousTransfer.NumberOfPackets;

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_ISOCH_TRANSFER(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbIsochronousTransfer.PipeHandle,
                Irp,
                Urb,
                packetCount,

                //
                // Size of the base URB_ISOCH_TRANSFER without the
                // IsoPacket[] array, and the pointer to the base of the
                // URB_ISOCH_TRANSFER.
                //
                FIELD_OFFSET(struct _URB_ISOCH_TRANSFER, IsoPacket),
                Urb,

                //
                // Size of an IsoPacket[] array element, and the
                // pointer to the base of the IsoPacket[] array.
                //
                sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),
                &Urb->UrbIsochronousTransfer.IsoPacket[0]);
        }

    } FINALLY {

    }
}

VOID
Etw_GetSetDescriptorUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            switch (Urb->UrbHeader.Function) {
            default:
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    Urb);
                break;
            }
        }

    } FINALLY {

    }
}

VOID
Etw_SetClearFeatureUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            switch (Urb->UrbHeader.Function) {
            default:
            case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_OTHER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_SET_FEATURE_TO_OTHER:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_OTHER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_FEATURE_REQUEST),
                    Urb);
                break;
            }
        }

    } FINALLY {

    }
}

VOID
Etw_GetStatusUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            switch (Urb->UrbHeader.Function) {
            default:
            case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_GET_STATUS_FROM_OTHER:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_OTHER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST),
                    Urb);
                break;
            }
        }

    } FINALLY {

    }
}

VOID
Etw_ClassVendorUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            switch (Urb->UrbHeader.Function) {
            default:
            case URB_FUNCTION_VENDOR_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_VENDOR_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_VENDOR_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLASS_DEVICE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_DEVICE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLASS_INTERFACE:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_INTERFACE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLASS_ENDPOINT:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_ENDPOINT(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_CLASS_OTHER:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_OTHER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            case URB_FUNCTION_VENDOR_OTHER:
                EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_OTHER(
                    NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                    ucxController,
                    ucxUsbDevice,
                    Urb->UrbControlTransfer.PipeHandle,
                    Irp,
                    Urb,
                    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                    Urb);
                break;
            }
        }

    } FINALLY {

    }
}

VOID
Etw_GetMsFeatureUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbControlTransfer.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_OS_FEATURE_DESCRIPTOR_REQUEST),
                Urb);
        }

    } FINALLY {

    }
}

VOID
Etw_GetInterfaceUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_INTERFACE(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbControlTransfer.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_CONTROL_GET_INTERFACE_REQUEST),
                Urb);
        }

    } FINALLY {

    }
}

VOID
Etw_GetConfigurationUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    )
{
    NTSTATUS                    activityIdStatus;
    GUID                        activityId;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    UCXCONTROLLER               ucxController;

    TRY {

        if (EventEnabledUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER()) {

            activityIdStatus = Ucx_GetActivityIdIrp(Irp, &activityId);

            ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxController = ucxUsbDeviceContext->UcxControllerContext->UcxController;

            EventWriteUCX_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_CONFIGURATION(
                NT_SUCCESS(activityIdStatus) ? &activityId : NULL,
                ucxController,
                ucxUsbDevice,
                Urb->UrbControlTransfer.PipeHandle,
                Irp,
                Urb,
                sizeof(struct _URB_CONTROL_GET_CONFIGURATION_REQUEST),
                Urb);
        }

    } FINALLY {

    }
}
