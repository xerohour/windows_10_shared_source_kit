/*++
 *
Copyright (c) Microsoft Corporation

Module Name:

    counter.h

Abstract:

    Responsible for creating and closing instances of various counter sets.
    Most of the logic is about creating a proper instance name.

--*/

#include "pch.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Counter_Register)
#pragma alloc_text(PAGE, Counter_Unregister)
#pragma alloc_text(PAGE, Counter_CreateInterrupterInstance)
#pragma alloc_text(PAGE, Counter_CreateCommonBufferInstance)
#pragma alloc_text(PAGE, Counter_CreateTransferRingInstance)
#endif

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Register()
{
    PAGED_CODE();

    if (g_WdfDriverUsbXhciContext->TestModeOn) {
        (VOID)Ctr_RegisterInterrupter(NULL, NULL);
        (VOID)Ctr_RegisterCommonBuffer(NULL, NULL);
        (VOID)Ctr_RegisterTransferRing(NULL, NULL);
    }
}

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Counter_Unregister()
{
    PAGED_CODE();

    if (g_WdfDriverUsbXhciContext->TestModeOn) {
        Ctr_UnregisterInterrupter();
        Ctr_UnregisterCommonBuffer();
        Ctr_UnregisterTransferRing();
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
{
    HANDLE          controllerHandle;
    UNICODE_STRING  instanceName;
    WCHAR           instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII.65535")];

    PAGED_CODE();

    TRY {

        controllerHandle = InterrupterData->ControllerHandle;

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (InterrupterData->Counter.PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(controllerHandle) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u.%u",
                                Controller_GetPciVendorID(controllerHandle),
                                Controller_GetInstanceNumber(controllerHandle),
                                InterrupterData->MessageNumber);

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u.%u",
                                Controller_GetAcpiVendorID(controllerHandle),
                                Controller_GetInstanceNumber(controllerHandle),
                                InterrupterData->MessageNumber);
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateInterrupter(&(InterrupterData->Counter.PcwInstanceHandle),
                                    &instanceName,
                                    &(InterrupterData->Counter));

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseInterrupterInstance(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
{
    if (InterrupterData->Counter.PcwInstanceHandle != NULL) {
        Ctr_CloseInterrupter(InterrupterData->Counter.PcwInstanceHandle);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    PCONTROLLER_DATA    controllerData;
    UNICODE_STRING      instanceName;
    WCHAR               instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII")];

    PAGED_CODE();

    TRY {

        controllerData = CommonBufferData->ControllerHandle;

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (CommonBufferData->Counter.PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(controllerData) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u",
                                Controller_GetPciVendorID(controllerData),
                                Controller_GetInstanceNumber(controllerData));

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u",
                                Controller_GetAcpiVendorID(controllerData),
                                Controller_GetInstanceNumber(controllerData));
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateCommonBuffer(&(CommonBufferData->Counter.PcwInstanceHandle),
                                     &instanceName,
                                     &(CommonBufferData->Counter));

    } FINALLY {

    }

    return;

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseCommonBufferInstance(
    __in
        PCOMMON_BUFFER_DATA CommonBufferData
    )
{
    if (CommonBufferData->Counter.PcwInstanceHandle != NULL) {
        Ctr_CloseCommonBuffer(CommonBufferData->Counter.PcwInstanceHandle);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Counter_CreateTransferRingInstance(
    __in
        PENDPOINT_DATA         EndpointData,
    __in
        ULONG                  StreamId,
    __in
        PTRANSFERRING_COUNTER  TransferRingCounter
    )
{
    UNICODE_STRING  instanceName;
    WCHAR           instanceNameBuffer[RTL_NUMBER_OF("VVVV.IIII.VENN.EE.SS")];

    PAGED_CODE();

    TRY {

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        if (TransferRingCounter->PcwInstanceHandle != NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(EndpointData->ControllerHandle) ==
            UcxControllerParentBusTypePci) {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%04X.%u.%04X.%02X.%u",
                                Controller_GetPciVendorID(EndpointData->ControllerHandle),
                                Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                                UsbDevice_GetVendorId(EndpointData->UsbDeviceHandle),
                                Endpoint_GetAddress(EndpointData),
                                StreamId);

        } else {

            RtlStringCchPrintfW(instanceNameBuffer,
                                RTL_NUMBER_OF(instanceNameBuffer),
                                L"%S.%u.%04X.%02X.%u",
                                Controller_GetAcpiVendorID(EndpointData->ControllerHandle),
                                Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                                UsbDevice_GetVendorId(EndpointData->UsbDeviceHandle),
                                Endpoint_GetAddress(EndpointData),
                                StreamId);
        }

        RtlInitUnicodeString(&instanceName, instanceNameBuffer);

        (VOID)Ctr_CreateTransferRing(&TransferRingCounter->PcwInstanceHandle,
                                     &instanceName,
                                     TransferRingCounter);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Counter_CloseTransferRingInstance(
    __in
        PTRANSFERRING_COUNTER TransferRingCounter
    )
{
    if (TransferRingCounter->PcwInstanceHandle != NULL) {
        Ctr_CloseTransferRing(TransferRingCounter->PcwInstanceHandle);
    }
}
