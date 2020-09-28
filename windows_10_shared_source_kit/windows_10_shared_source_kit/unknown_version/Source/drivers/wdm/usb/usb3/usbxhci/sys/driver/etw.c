/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.c

Abstract:

    USB XHCI Driver

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
    PCONTROLLER_DATA           *controllerTable = NULL;
    ULONG                       controllerTableCount;
    ULONG                       controllerTableIndex;
    PCONTROLLER_DATA            currentControllerData;
    KIRQL                       irql;

    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(Level);
    UNREFERENCED_PARAMETER(MatchAnyKeyword);
    UNREFERENCED_PARAMETER(MatchAllKeyword);
    UNREFERENCED_PARAMETER(FilterData);
    UNREFERENCED_PARAMETER(CallbackContext);

    TRY {

        controllerTable = NULL;

        if (IsEnabled == 0) {
            LEAVE;
        }

        RtlZeroMemory(&activityId, sizeof(GUID));

        //
        // Allocate an Activity ID for ETW rundown.
        //
        EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID, &activityId);

        //
        // Start ETW rundown.
        //
        EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_START(&activityId);

        //
        // Get the current Controller list count.
        //
        KeAcquireSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, &irql);
        controllerTableCount = g_WdfDriverUsbXhciContext->ControllerListCount;
        KeReleaseSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, irql);

        if (controllerTableCount == 0) {
            EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_COMPLETE(&activityId);
            LEAVE;
        }

        WHILE (TRUE) {

            //
            // Allocate a table large large enough to contain a
            // Controller context entry for each Controller in
            // the current list.
            //
            controllerTable = ExAllocatePoolWithTag(NonPagedPool,
                                                    (controllerTableCount * sizeof(*controllerTable)),
                                                    DriverAllocationTag);

            if (controllerTable == NULL) {
                LEAVE;
            }

            RtlZeroMemory(controllerTable, controllerTableCount * sizeof(*controllerTable));

            KeAcquireSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, &irql);

            if (controllerTableCount < g_WdfDriverUsbXhciContext->ControllerListCount) {

                controllerTableCount = g_WdfDriverUsbXhciContext->ControllerListCount;
                KeReleaseSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, irql);

                //
                // The current table size is too small.  Free the
                // current table and reallocate a larger table and
                // try again.
                //
                ExFreePoolWithTag(controllerTable, DriverAllocationTag);
                controllerTable = NULL;

                continue;
            }

            controllerTableIndex = 0;

            FOR_ALL_IN_LIST(CONTROLLER_DATA,
                            &g_WdfDriverUsbXhciContext->ControllerListHead,
                            ControllerListEntry,
                            currentControllerData) {

                //
                // Take a reference on the UcxController since we
                // are going to walk this list of controllers
                // outside the ControllerListLock.
                //
                WdfObjectReferenceWithTag(currentControllerData->UcxController,
                    TAG(Etw_EnableCallback));

                __analysis_assume(controllerTableIndex < controllerTableCount);
                controllerTable[controllerTableIndex++] = currentControllerData;
            }

            NT_ASSERT(controllerTableIndex == controllerTableCount);

            KeReleaseSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, irql);

            break;
        }

        //
        // Rundown the table of Controllers and release the
        // references on the UcxControllers.
        //
        for (controllerTableIndex = 0; controllerTableIndex < controllerTableCount; controllerTableIndex++) {

            currentControllerData = controllerTable[controllerTableIndex];

            if (currentControllerData == NULL) {
                break;
            }

            //
            // Do the ETW rundown of the Controller.
            //
            Etw_ControllerRundown(&activityId, currentControllerData);

            //
            // Done with the ETW rundown of the Controller.
            // Release the reference on the UcxController.
            //
            WdfObjectDereferenceWithTag(currentControllerData->UcxController, TAG(Etw_EnableCallback));
        }

        EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_COMPLETE(&activityId);

    } FINALLY {

        if (controllerTable != NULL) {
            ExFreePoolWithTag(controllerTable, DriverAllocationTag);
            controllerTable = NULL;
        }
    }

    return;
}

VOID
Etw_ControllerRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the Controller.

    It is assumed that a WdfObjectReference is held on the
    UcxController by the caller while this routine executes.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW rundown event for this Controller.
        //
        EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_CONTROLLER_INFORMATION(
            ActivityId,
            ControllerData->UcxController,
            ControllerData->ParentBusType,
            Controller_GetPciBusNumber(ControllerData),
            Controller_GetPciDeviceNumber(ControllerData),
            Controller_GetPciFunctionNumber(ControllerData),
            Controller_GetPciVendorID(ControllerData),
            Controller_GetPciDeviceID(ControllerData),
            Controller_GetPciRevisionID(ControllerData),
            Controller_GetAcpiVendorID(ControllerData),
            Controller_GetAcpiDeviceID(ControllerData),
            Controller_GetAcpiRevisionID(ControllerData),
            ControllerData->FirmwareVersion,
            ControllerData->ControllerS0IdleStatus,
            ControllerData->CurrentWdfPowerDeviceState,
            Controller_GetInstanceNumber(ControllerData),
            ControllerData->WdfPowerReferenceForDebuggerAcquired);

        //
        // Perform the ETW event rundown on the all of the devices
        // downstream of the UcxController.
        //
        Etw_DeviceListRundown(ActivityId, ControllerData);

    } FINALLY {

    }

    return;
}

VOID
Etw_DeviceListRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the all of the child
    devices downstream of the given Controller.

Return Value:

    None

--*/
{
    PUSBDEVICE_DATA*                usbDeviceTable;
    ULONG                           usbDeviceTableCount;
    ULONG                           usbDeviceTableIndex;
    PUSBDEVICE_DATA                 usbDeviceData;

    KIRQL                           irql;

    TRY {

        usbDeviceTable = NULL;

        //
        // Get the current count of all Device children of this
        // Controller.
        //
        KeAcquireSpinLock(&ControllerData->DeviceListLock, &irql);
        usbDeviceTableCount = ControllerData->DeviceListCount;
        KeReleaseSpinLock(&ControllerData->DeviceListLock, irql);

        if (usbDeviceTableCount == 0) {
            LEAVE;
        }

        WHILE (TRUE) {

            //
            // Allocate a table large large enough to contain a
            // Device context entry for each Device in the current
            // Device list.
            //
            usbDeviceTable = ExAllocatePoolWithTag(NonPagedPool,
                                                   (usbDeviceTableCount * sizeof(*usbDeviceTable)),
                                                   DriverAllocationTag);

            if (usbDeviceTable == NULL) {
                LEAVE;
            }

            RtlZeroMemory(usbDeviceTable, usbDeviceTableCount * sizeof(*usbDeviceTable));

            KeAcquireSpinLock(&ControllerData->DeviceListLock, &irql);

            if (usbDeviceTableCount < ControllerData->DeviceListCount) {

                usbDeviceTableCount = ControllerData->DeviceListCount;
                KeReleaseSpinLock(&ControllerData->DeviceListLock, irql);

                //
                // The current table size is too small.  Free the
                // current table and reallocate a larger table and
                // try again.
                //
                ExFreePoolWithTag(usbDeviceTable, DriverAllocationTag);
                usbDeviceTable = NULL;

                continue;
            }

            usbDeviceTableIndex = 0;

            FOR_ALL_IN_LIST(USBDEVICE_DATA,
                            &ControllerData->DeviceListHead,
                            DeviceListEntry,
                            usbDeviceData) {

                //
                // Take a reference on the UcxDevice since we
                // are going to walk this list of Devices
                // outside the DeviceListLock.
                //
                WdfObjectReferenceWithTag(usbDeviceData->UcxUsbDevice, TAG(Etw_DeviceListRundown)); 

                __analysis_assume(usbDeviceTableIndex < usbDeviceTableCount);
                usbDeviceTable[usbDeviceTableIndex++] = usbDeviceData;
            }

            NT_ASSERT(usbDeviceTableIndex == usbDeviceTableCount);

            KeReleaseSpinLock(&ControllerData->DeviceListLock, irql);

            break;
        }

        //
        // Do the ETW rundown of all of the UcxDevice children.
        //
        for (usbDeviceTableIndex = 0; usbDeviceTableIndex < usbDeviceTableCount; usbDeviceTableIndex++) {

            usbDeviceData = usbDeviceTable[usbDeviceTableIndex];

            if (usbDeviceData == NULL) {
                break;
            }

            //
            // Do the ETW rundown of the UcxDevice.
            //
            Etw_DeviceRundown(ActivityId, usbDeviceData);

            //
            // Done with the ETW rundown of the UcxDevice.  Release the
            // reference on it.
            //
            WdfObjectDereferenceWithTag(usbDeviceData->UcxUsbDevice, TAG(Etw_DeviceListRundown));
        }

    } FINALLY {

        if (usbDeviceTable != NULL) {
            ExFreePoolWithTag(usbDeviceTable, DriverAllocationTag);
            usbDeviceTable = NULL;
        }
    }
}

VOID
Etw_DeviceRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the UcxUsbDevice.

    It is assumed that a WdfObjectReference is held on the
    UcxUsbDevice by the caller while this routine executes.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO  usbDeviceInfo;

    TRY {

        usbDeviceInfo = &UsbDeviceData->UsbDeviceInfo;

        //
        // Log the ETW rundown event for this UcxUsbDevice.
        //
        EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_DEVICE_INFORMATION(
            ActivityId,
            Controller_GetUcxControllerObject(UsbDeviceData->ControllerHandle),
            usbDeviceInfo->TtHub,
            UsbDeviceData->UcxUsbDevice,
            usbDeviceInfo->DeviceSpeed,
            usbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)usbDeviceInfo->PortPath.PortPath,
            UsbDeviceData->SlotId,
            (UsbDeviceData->IsHub == FALSE) ? 0 : 1,
            UsbDeviceData->NumberOfPorts,
            UsbDeviceData->NumberOfTTs,
            sizeof(UsbDeviceData->DeviceDescriptor),
            &UsbDeviceData->DeviceDescriptor);

        //
        // Perform the ETW event rundown on the all of the Endpoints
        // that exist for the Device.
        //
        Etw_EndpointListRundown(ActivityId, UsbDeviceData);

    } FINALLY {

    }
}

VOID
Etw_EndpointListRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    )
/*++

Routine Description:

    This routine performs an ETW event rundown on the all of the
    Endpoints that exist for the given Device.

Return Value:

    None

--*/
{
    PENDPOINT_DATA*                 endpointTable;
    ULONG                           endpointTableCount;
    ULONG                           endpointTableIndex;
    PENDPOINT_DATA                  endpointData;
    KIRQL                           irql;

    TRY {

        endpointTable = NULL;

        //
        // Get the current count of all Endpoints that exist for this
        // Device.
        //
        KeAcquireSpinLock(&UsbDeviceData->EndpointListLock, &irql);
        endpointTableCount = UsbDeviceData->EndpointListCount;
        KeReleaseSpinLock(&UsbDeviceData->EndpointListLock, irql);

        if (endpointTableCount == 0) {
            LEAVE;
        }

        WHILE (TRUE) {

            //
            // Allocate a table large large enough to contain an
            // Endpoint context entry for each Endpoint in the
            // current Endpoint list.
            //
            endpointTable = ExAllocatePoolWithTag(NonPagedPool,
                                                  (endpointTableCount * sizeof(*endpointTable)),
                                                  DriverAllocationTag);

            if (endpointTable == NULL) {
                LEAVE;
            }

            RtlZeroMemory(endpointTable, endpointTableCount * sizeof(*endpointTable));

            KeAcquireSpinLock(&UsbDeviceData->EndpointListLock, &irql);

            if (endpointTableCount < UsbDeviceData->EndpointListCount) {

                endpointTableCount = UsbDeviceData->EndpointListCount;
                KeReleaseSpinLock(&UsbDeviceData->EndpointListLock, irql);

                //
                // The current table size is too small.  Free the
                // current table and reallocate a larger table and
                // try again.
                //
                ExFreePoolWithTag(endpointTable, DriverAllocationTag);
                endpointTable = NULL;

                continue;
            }

            endpointTableIndex = 0;

            FOR_ALL_IN_LIST(ENDPOINT_DATA,
                            &UsbDeviceData->EndpointListHead,
                            EndpointListEntry,
                            endpointData) {

                //
                // Take a reference on the UcxEndpoint since we are
                // going to walk this list of Endpoints outside the
                // EndpointListLock.
                //
                WdfObjectReferenceWithTag(endpointData->UcxEndpoint, TAG(Etw_EndpointListRundown));

                __analysis_assume(endpointTableIndex < endpointTableCount);
                endpointTable[endpointTableIndex++] = endpointData;
            }

            NT_ASSERT(endpointTableIndex == endpointTableCount);

            KeReleaseSpinLock(&UsbDeviceData->EndpointListLock, irql);

            break;
        }

        //
        // Do the ETW rundown of all of the Endpoints for this
        // Device.
        //
        for (endpointTableIndex = 0; endpointTableIndex < endpointTableCount; endpointTableIndex++) {

            endpointData = endpointTable[endpointTableIndex];

            if (endpointData == NULL) {
                break;
            }

            //
            // Do the ETW rundown of the UcxDevice.
            //
            Etw_EndpointRundown(ActivityId, endpointData);

            //
            // Done with the ETW rundown of the UcxDevice.  Release the
            // reference on it.
            //
            WdfObjectDereferenceWithTag(endpointData->UcxEndpoint, TAG(Etw_EndpointListRundown));
        }

    } FINALLY {

        if (endpointTable != NULL) {
            ExFreePoolWithTag(endpointTable, DriverAllocationTag);
            endpointTable = NULL;
        }
    }
}

VOID
Etw_EndpointRundown(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    )
/*++

Routine Description:

    This routine writes an ETW Rundown event for the Endpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Rundown event for this Endpoint.
        //
        EventWriteUSBXHCI_ETW_EVENT_RUNDOWN_ENDPOINT_INFORMATION(
            ActivityId,
            Controller_GetUcxControllerObject(EndpointData->ControllerHandle),
            EndpointData->UcxUsbDevice,
            EndpointData->UcxEndpoint,
            sizeof(EndpointData->UsbEndpointDescriptor),
            &EndpointData->UsbEndpointDescriptor,
            sizeof(EndpointData->UsbSuperSpeedEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedEndpointCompanionDescriptor,
            sizeof(EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor);

    } FINALLY {

    }
}

VOID
Etw_ControllerCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the Controller.

    This is done in the context of the EvtDriverDeviceAdd() callback.

Return Value:

    None

--*/
{
    TRY {













        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_CREATE(
            ActivityId,
            ControllerData->UcxController,
            ControllerData->ParentBusType,
            Controller_GetPciBusNumber(ControllerData),
            Controller_GetPciDeviceNumber(ControllerData),
            Controller_GetPciFunctionNumber(ControllerData),
            Controller_GetPciVendorID(ControllerData),
            Controller_GetPciDeviceID(ControllerData),
            Controller_GetPciRevisionID(ControllerData),
            Controller_GetAcpiVendorID(ControllerData),
            Controller_GetAcpiDeviceID(ControllerData),
            Controller_GetAcpiRevisionID(ControllerData),
            ControllerData->FirmwareVersion,
            ControllerData->ControllerS0IdleStatus,
            ControllerData->CurrentWdfPowerDeviceState,
            Controller_GetInstanceNumber(ControllerData),
            ControllerData->WdfPowerReferenceForDebuggerAcquired);

    } FINALLY {

    }

    return;
}

VOID
Etw_ControllerDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the Controller.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Delete event for this Controller.
        //
        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_DELETE(
            ActivityId,
            ControllerData->UcxController,
            ControllerData->ParentBusType,
            Controller_GetPciBusNumber(ControllerData),
            Controller_GetPciDeviceNumber(ControllerData),
            Controller_GetPciFunctionNumber(ControllerData),
            Controller_GetPciVendorID(ControllerData),
            Controller_GetPciDeviceID(ControllerData),
            Controller_GetPciRevisionID(ControllerData),
            Controller_GetAcpiVendorID(ControllerData),
            Controller_GetAcpiDeviceID(ControllerData),
            Controller_GetAcpiRevisionID(ControllerData),
            ControllerData->FirmwareVersion,
            ControllerData->ControllerS0IdleStatus,
            ControllerData->CurrentWdfPowerDeviceState,
            Controller_GetInstanceNumber(ControllerData),
            ControllerData->WdfPowerReferenceForDebuggerAcquired);

    } FINALLY {

    }

    return;
}

VOID
Etw_ControllerFirmareVersionUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    )
/*++

Routine Description:

    This routine writes an ETW Firwmare Version Update event for the Controller.

    This is done only if the controller firmware version was not valid
    at the time that the ETW Create event for the Controller was written
    and the firmware version has now become valid.









Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Firmware Version Update event for this Controller.
        //
        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_FIRMWARE_VERSION_UPDATE(
            ActivityId,
            ControllerData->UcxController,
            ControllerData->ParentBusType,
            Controller_GetPciBusNumber(ControllerData),
            Controller_GetPciDeviceNumber(ControllerData),
            Controller_GetPciFunctionNumber(ControllerData),
            Controller_GetPciVendorID(ControllerData),
            Controller_GetPciDeviceID(ControllerData),
            Controller_GetPciRevisionID(ControllerData),
            Controller_GetAcpiVendorID(ControllerData),
            Controller_GetAcpiDeviceID(ControllerData),
            Controller_GetAcpiRevisionID(ControllerData),
            ControllerData->FirmwareVersion,
            ControllerData->ControllerS0IdleStatus,
            ControllerData->CurrentWdfPowerDeviceState,
            Controller_GetInstanceNumber(ControllerData),
            ControllerData->WdfPowerReferenceForDebuggerAcquired);

    } FINALLY {

    }

    return;
}

VOID
Etw_DeviceCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the Device.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO  usbDeviceInfo;

    TRY {

        usbDeviceInfo = &UsbDeviceData->UsbDeviceInfo;

        //
        // Log the ETW Create event for this Device.
        //
        EventWriteUSBXHCI_ETW_EVENT_DEVICE_CREATE(
            ActivityId,
            Controller_GetUcxControllerObject(UsbDeviceData->ControllerHandle),
            usbDeviceInfo->TtHub,
            UsbDeviceData->UcxUsbDevice,
            usbDeviceInfo->DeviceSpeed,
            usbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)usbDeviceInfo->PortPath.PortPath,
            UsbDeviceData->SlotId,
            (UsbDeviceData->IsHub == FALSE) ? 0 : 1,
            UsbDeviceData->NumberOfPorts,
            UsbDeviceData->NumberOfTTs,
            sizeof(UsbDeviceData->DeviceDescriptor),
            &UsbDeviceData->DeviceDescriptor);

    } FINALLY {

    }
}

VOID
Etw_DeviceUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    )
/*++

Routine Description:

    This routine writes an ETW Update event for the Device.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO  usbDeviceInfo;

    TRY {

        usbDeviceInfo = &UsbDeviceData->UsbDeviceInfo;

        //
        // Log the ETW Update event for this Device.
        //
        EventWriteUSBXHCI_ETW_EVENT_DEVICE_UPDATE(
            ActivityId,
            Controller_GetUcxControllerObject(UsbDeviceData->ControllerHandle),
            usbDeviceInfo->TtHub,
            UsbDeviceData->UcxUsbDevice,
            usbDeviceInfo->DeviceSpeed,
            usbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)usbDeviceInfo->PortPath.PortPath,
            UsbDeviceData->SlotId,
            (UsbDeviceData->IsHub == FALSE) ? 0 : 1,
            UsbDeviceData->NumberOfPorts,
            UsbDeviceData->NumberOfTTs,
            sizeof(UsbDeviceData->DeviceDescriptor),
            &UsbDeviceData->DeviceDescriptor);

    } FINALLY {

    }
}

VOID
Etw_DeviceDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the Device.

Return Value:

    None

--*/
{
    PUCXUSBDEVICE_INFO  usbDeviceInfo;

    TRY {

        usbDeviceInfo = &UsbDeviceData->UsbDeviceInfo;

        //
        // Log the ETW Delete event for this Device.
        //
        EventWriteUSBXHCI_ETW_EVENT_DEVICE_DELETE(
            ActivityId,
            Controller_GetUcxControllerObject(UsbDeviceData->ControllerHandle),
            usbDeviceInfo->TtHub,
            UsbDeviceData->UcxUsbDevice,
            usbDeviceInfo->DeviceSpeed,
            usbDeviceInfo->PortPath.PortPathDepth,
            (unsigned int *)usbDeviceInfo->PortPath.PortPath,
            UsbDeviceData->SlotId,
            (UsbDeviceData->IsHub == FALSE) ? 0 : 1,
            UsbDeviceData->NumberOfPorts,
            UsbDeviceData->NumberOfTTs,
            sizeof(UsbDeviceData->DeviceDescriptor),
            &UsbDeviceData->DeviceDescriptor);

    } FINALLY {

    }
}

VOID
Etw_EndpointCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    )
/*++

Routine Description:

    This routine writes an ETW Create event for the Endpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Create event for this Endpoint.
        //
        EventWriteUSBXHCI_ETW_EVENT_ENDPOINT_CREATE(
            ActivityId,
            Controller_GetUcxControllerObject(EndpointData->ControllerHandle),
            EndpointData->UcxUsbDevice,
            EndpointData->UcxEndpoint,
            sizeof(EndpointData->UsbEndpointDescriptor),
            &EndpointData->UsbEndpointDescriptor,
            sizeof(EndpointData->UsbSuperSpeedEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedEndpointCompanionDescriptor,
            sizeof(EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor);

    } FINALLY {

    }
}

VOID
Etw_EndpointUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    )
/*++

Routine Description:

    This routine writes an ETW Update event for the Endpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Update event for this Endpoint.
        //
        EventWriteUSBXHCI_ETW_EVENT_ENDPOINT_UPDATE(
            ActivityId,
            Controller_GetUcxControllerObject(EndpointData->ControllerHandle),
            EndpointData->UcxUsbDevice,
            EndpointData->UcxEndpoint,
            sizeof(EndpointData->UsbEndpointDescriptor),
            &EndpointData->UsbEndpointDescriptor,
            sizeof(EndpointData->UsbSuperSpeedEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedEndpointCompanionDescriptor,
            sizeof(EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor);

    } FINALLY {

    }
}

VOID
Etw_EndpointDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    )
/*++

Routine Description:

    This routine writes an ETW Delete event for the Endpoint.

Return Value:

    None

--*/
{
    TRY {

        //
        // Log the ETW Delete event for this Endpoint.
        //
        EventWriteUSBXHCI_ETW_EVENT_ENDPOINT_DELETE(
            ActivityId,
            Controller_GetUcxControllerObject(EndpointData->ControllerHandle),
            EndpointData->UcxUsbDevice,
            EndpointData->UcxEndpoint,
            sizeof(EndpointData->UsbEndpointDescriptor),
            &EndpointData->UsbEndpointDescriptor,
            sizeof(EndpointData->UsbSuperSpeedEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedEndpointCompanionDescriptor,
            sizeof(EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor),
            &EndpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor);

    } FINALLY {

    }
}

VOID
Etw_StartDeviceFail(
    __in
        HANDLE                      ControllerHandle,
    __in_opt
        LPCGUID                     ActivityId,
    __in
        StartDeviceFailReason       ReasonCode
    )
/*++

Routine Description:

    This routine writes an ETW event for start device failure.

Return Value:

    None

--*/
{
    ULONG       resultLength;
    NTSTATUS    status;

    DECLARE_UNICODE_STRING_SIZE(deviceDescription, DEVICE_DESC_SIZE);

    TRY {

        status = IoGetDeviceProperty(Controller_GetWdmPhysicalDeviceObject(ControllerHandle),
                                     DevicePropertyDeviceDescription,
                                     deviceDescription.MaximumLength - sizeof(UNICODE_NULL),
                                     deviceDescription.Buffer,
                                     &resultLength);

        if (NT_SUCCESS(status)) {
            NT_ASSERT(resultLength > sizeof(UNICODE_NULL));
            deviceDescription.Length = (USHORT)(resultLength - sizeof(UNICODE_NULL));
        } else {
            RtlInitUnicodeString(&deviceDescription, L"USB eXtensible Host Controller");
        }

        EventWriteUSBXHCI_ETW_EVENT_STARTDEVICE_FAIL(
            ActivityId,
            Controller_GetUcxControllerObject(ControllerHandle),
            (deviceDescription.Length + sizeof(UNICODE_NULL)) / sizeof(WCHAR),
            deviceDescription.Buffer,
            ReasonCode);

    } FINALLY {

    }
}

VOID
Etw_CommandSend(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb
    )
/*++

Routine Description:

    This routine writes an ETW event when a command is placed on the command ring.

Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(Crb);

    TRY {

        EventWriteUSBXHCI_ETW_EVENT_COMMAND_SEND(
            ActivityId,
            Controller_GetUcxControllerObject(CommandData->ControllerHandle),
            Crb,
            (PVOID) &Crb->Trb);

    } FINALLY {

    }
}

VOID
Etw_CommandWaitlisted(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        COMMAND_WAITLIST_REASON     WaitlistReason
    )
/*++

Routine Description:

    This routine writes an ETW event when a command is placed on the waiting list.

Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(Crb);

    TRY {

        EventWriteUSBXHCI_ETW_EVENT_COMMAND_WAITLISTED(
            ActivityId,
            Controller_GetUcxControllerObject(CommandData->ControllerHandle),
            Crb,
            (PVOID) &Crb->Trb,
            WaitlistReason);

    } FINALLY {

    }
}

VOID
Etw_CommandComplete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CompletionStatus
    )
/*++

Routine Description:

    This routine writes an ETW event when a command is completed by the command object.

Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(Crb);

    TRY {

        EventWriteUSBXHCI_ETW_EVENT_COMMAND_COMPLETE(
            ActivityId,
            Controller_GetUcxControllerObject(CommandData->ControllerHandle),
            Crb,
            (PVOID) &Crb->Trb,
            CompletionStatus,
            Crb->CompletionCode);

    } FINALLY {

    }
}

VOID
Etw_CommandCompleteError(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CompletionStatus
    )
/*++

Routine Description:

    This routine writes an ETW event when a command is completed by the command object and its
    status is a new error.

Return Value:

    None

--*/
{
    UNREFERENCED_PARAMETER(Crb);

    TRY {

        EventWriteUSBXHCI_ETW_EVENT_COMMAND_COMPLETE_ERROR(
            ActivityId,
            Controller_GetUcxControllerObject(CommandData->ControllerHandle),
            Crb,
            (PVOID) &Crb->Trb,
            CompletionStatus,
            Crb->CompletionCode);

    } FINALLY {

    }
}
