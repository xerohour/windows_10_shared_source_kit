/*++

Copyright (c) 2002 Microsoft Corporation

Module Name:

power.c

Abstract:

This module contains code to handle IRP_MJ_POWER dispatches for SD
controllers

Authors:

Neil Sandlin (neilsa) 1-Jan-2002
Yue Zuo (yuezuo) 1-May-2008

Environment:

Kernel mode only

Notes:

Revision History:


--*/

#include "pch.h"
#include "power.tmh"
#include "sdbuseventlist.h"

//
// Internal References
//

NTSTATUS
SdbusFdoCompletePowerIrp(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp,
    __in NTSTATUS Status
    );

NTSTATUS
SdbusFdoForwardPowerIrp(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    );

NTSTATUS
SdbusFdoDispatchSystemPowerIrp(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    );

IO_COMPLETION_ROUTINE SdbusFdoSystemPowerIrpCompletion;
/*
NTSTATUS
SdbusFdoSystemPowerIrpCompletion(
__in PDEVICE_OBJECT Fdo,
__in PIRP Irp,
__in_opt PVOID NotUsed
);
*/

IO_WORKITEM_ROUTINE SdbusCheckCacheOnHibernate;
/*
VOID
SdbusCheckCacheOnHibernate(
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PVOID Context
    );
*/

REQUEST_POWER_COMPLETE SdbusFdoSystemRequestPowerCompletion;
/*
VOID
SdbusFdoSystemRequestPowerCompletion(
__in PDEVICE_OBJECT Fdo,
__in UCHAR MinorFunction,
__in POWER_STATE PowerState,
__in_opt PVOID Context,
__in PIO_STATUS_BLOCK IoStatus
);
*/

NTSTATUS
SdbusFdoSetDevicePowerState(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    );

IO_COMPLETION_ROUTINE SdbusFdoDevicePowerUpIrpCompletion;
/*
NTSTATUS
SdbusFdoDevicePowerUpIrpCompletion(
__in PDEVICE_OBJECT Fdo,
__in PIRP Irp,
__in_opt PVOID Context
);
*/

VOID
SdbusFdoSocketPowerDownCompletion(
    __in PSD_SOCKET Socket,
    __in PVOID Context,
    __in NTSTATUS Status
    );

VOID
SdbusFdoSocketPowerUpCompletion(
    __in PSD_SOCKET Socket,
    __in PVOID Context,
    __in NTSTATUS Status
    );

NTSTATUS
SdbusPdoCompletePowerIrp(
    __inout PIRP Irp,
    __in NTSTATUS Status
    );

VOID
SdbusFxSocketPowerUpCompletion(
    __in PSD_SOCKET Socket,
    __in PVOID Context,
    __in NTSTATUS Status
    );

NTSTATUS
SdbusPdoSetDevicePowerState(
    __in PDEVICE_OBJECT Pdo,
    __inout PIRP Irp
    );

VOID
SdbusFxDevicePowerNotRequiredStep2(
    __in PVOID Context
    );

KSYNCHRONIZE_ROUTINE SdbusFxSynchronizeActiveState;
KSYNCHRONIZE_ROUTINE SdbusFxSynchronizeIdleState;
KSYNCHRONIZE_ROUTINE SdbusFxSynchronizeDPRState;

typedef struct _DPR_STATE_CONTEXT {
    PFDO_EXTENSION FdoExtension;
    ULONG State;
} DPR_STATE_CONTEXT, *PDPR_STATE_CONTEXT;

#define SET_DEVICE_POWER_STATE(ext, new_state) {                            \
    POWER_STATE powerState;                                                 \
    powerState.DeviceState = new_state;                                     \
    PoSetPowerState(ext->DeviceObject,                                      \
    DevicePowerState,                                       \
    powerState);                                            \
    ext->DevicePowerState = new_state;                                      \
}                                                                           \


#define SET_SOCKET_PDO_FLAG(socket, flag)                                   \
{                                                                           \
    PDEVICE_OBJECT pdo = NULL;                                              \
    PPDO_EXTENSION pdoExtension = NULL;                                     \
    for (pdo = socket->PdoList; pdo != NULL; pdo = pdoExtension->NextPdo) { \
    pdoExtension = pdo->DeviceExtension;                                \
    SetDeviceFlag(pdoExtension, flag);                                  \
    }                                                                       \
}

//
// Disables eMMC Power gating via sleep cmd and reverts to SD power gate
// behavior.
// 

BOOLEAN SdbusPowerGateOverride = TRUE;

// ---------------------------------------------------------------------------
//
// FDO Routines
//
// ---------------------------------------------------------------------------

NTSTATUS
SdbusFdoSetPowerState(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    )

/*++

Routine Description

    Dispatches the IRP based on whether a system power state or device power
    state transition is requested

Arguments

    DeviceObject - Pointer to the functional device object for the sd
    controller

    Irp - Pointer to the Irp for the power dispatch

Return value

    NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status;

    status = IoAcquireRemoveLock(&fdoExtension->RemoveLock, Irp);
    if (!NT_SUCCESS(status)) {
        return SdbusFdoCompletePowerIrp(Fdo, Irp, status);
    }

    if (irpStack->Parameters.Power.Type == DevicePowerState) {
        status = SdbusFdoSetDevicePowerState(Fdo, Irp);
    } else if (irpStack->Parameters.Power.Type == SystemPowerState) {
        status = SdbusFdoDispatchSystemPowerIrp(Fdo, Irp);
    } else {
        status = Irp->IoStatus.Status;
        SdbusFdoCompletePowerIrp(Fdo, Irp, status);
    }
    return status;
}

NTSTATUS
SdbusFdoCompletePowerIrp(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp,
    __in NTSTATUS Status
    )

/*++

Routine Description

    Completion routine for the Power Irp directed to the PDO of the SD
    function.
    
Arguments

    Irp - Irp that needs to be completed

    Status - Status to be used to complete the IRP

Return Value

    NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    Irp->IoStatus.Status = Status;
    PoStartNextPowerIrp(Irp);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
    return Status;
}

NTSTATUS
SdbusFdoForwardPowerIrp(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    )
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status;

    if ((irpStack->Parameters.Power.Type == DevicePowerState) &&
        (irpStack->Parameters.Power.State.DeviceState == PowerDeviceD0)) {

        //
        // Notify PoFx that the device completed the requested transition
        // to D0. This should be done even if the irp is not required here.
        //

        PoFxReportDevicePoweredOn(fdoExtension->FxHandle);
        SdbusAddFxDbgLog(fdoExtension,
                         'D0Cp',
                         fdoExtension->FxPState,
                         fdoExtension->FxState,
                         fdoExtension->DevicePowerState);
    }

    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);
    status = PoCallDriver(fdoExtension->LowerDevice, Irp);
    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);

    return status;
}

NTSTATUS
SdbusFdoDispatchSystemPowerIrp(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    )

/*++

Routine Description

    Handles system power state IRPs for the host controller.

Arguments

    Fdo - Pointer to the functional device object for the sd controller

    Irp - Pointer to the Irp for the power dispatch

Return value

    status

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    SYSTEM_POWER_STATE newSystemState = irpStack->Parameters.Power.State.SystemState;

    //
    // process the irp only if we are started
    //

    if (fdoExtension->PnpState == SDBUS_DEVICE_NOT_STARTED) {

        DebugPrint((SDBUS_DEBUG_WARNING,
            "fdo %p dirp %p Device not started yet",
            Fdo,
            Irp));

        return SdbusFdoCompletePowerIrp(Fdo, Irp, STATUS_SUCCESS);
    }

    if (newSystemState >= POWER_SYSTEM_MAXIMUM) {
        return SdbusFdoCompletePowerIrp(Fdo, Irp, STATUS_UNSUCCESSFUL);
    }

    //
    // This function is capable of dealing with both set and query power
    // state, but we only support 'set' at this time
    //

    NT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);

    //
    // If we're already in the requested S-state, then simply forward the
    // IRP.
    //

    if (fdoExtension->SystemPowerState == newSystemState) {
        return SdbusFdoForwardPowerIrp(Fdo, Irp);
    }

    //
    // Mark the IRP pending and we will pick it up in the completion routine
    //

    IoMarkIrpPending(Irp);

    IoCopyCurrentIrpStackLocationToNext(Irp);

    IoSetCompletionRoutine(Irp,
        SdbusFdoSystemPowerIrpCompletion,
        NULL,
        TRUE,
        TRUE,
        TRUE);

    PoCallDriver(fdoExtension->LowerDevice, Irp);

    return STATUS_PENDING;
}

NTSTATUS
SdbusFdoSystemPowerIrpCompletion(
    PDEVICE_OBJECT Fdo,
    PIRP Irp,
    PVOID NotUsed
    )
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    UCHAR index;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    POWER_STATE powerState;
    SYSTEM_POWER_STATE newSystemState = irpStack->Parameters.Power.State.SystemState;
    DEVICE_POWER_STATE devicePowerState;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(NotUsed);

    //
    // Should not fail
    //
    //if (!NT_SUCCESS(status)) {
    //    PoStartNextPowerIrp(Irp);
    //    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
    //    return STATUS_CONTINUE_COMPLETION;
    //}

    fdoExtension->SystemPowerState = newSystemState;

    //
    // Switch to the appropriate device power state
    //

    devicePowerState = fdoExtension->DeviceCapabilities.DeviceState[newSystemState];
    powerState.DeviceState = devicePowerState;

    //
    // Validate new system state
    //

    if (devicePowerState == PowerDeviceUnspecified) {

        NT_ASSERT(FALSE);

        (VOID) SdbusFdoCompletePowerIrp(Fdo, Irp, STATUS_UNSUCCESSFUL);
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    //
    // Transitioned to system state
    //

    if ((fdoExtension->PagingPathCount > 0) &&
        ((newSystemState == PowerSystemHibernate) ||
        (newSystemState == PowerSystemShutdown))) {

        //
        // May need to query eMMC cache state at this point.
        // However, the eMMC PDO might be powered down at this point.
        // If it's powered down, that means we already checked the
        // cache state of the device. Otherwise, check now.
        //

        if ((newSystemState == PowerSystemHibernate) &&
            (fdoExtension->PoweredPdoCount > 0)) {

            if (KeGetCurrentIrql() > PASSIVE_LEVEL) {

                NT_ASSERT(fdoExtension->PendingSIrp == NULL);

                fdoExtension->PendingSIrp = Irp;
                IoQueueWorkItem(fdoExtension->CacheCheckWorkItem,
                                &SdbusCheckCacheOnHibernate,
                                DelayedWorkQueue,
                                (PVOID)fdoExtension);

                return STATUS_MORE_PROCESSING_REQUIRED;
            }

            for (index = 0; index < fdoExtension->NumberOfSockets; index += 1) {
                if (fdoExtension->Socket[index]->HostAccessMode == SDBUS_FUNCTION_TYPE_MMC_MEMORY) {
                    SdbusExecuteWorkSynchronous(SDWP_MMC_CHECK_WRITE_CACHE, 
                                                fdoExtension->Socket[index], 
                                                NULL, 
                                                NULL);
                }
            }
        }

        IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
        return STATUS_CONTINUE_COMPLETION;
    }

    NT_ASSERT(fdoExtension->PendingSIrp == NULL);

    fdoExtension->PendingSIrp = Irp;

    DebugPrint((SDBUS_DEBUG_POWER,
        "fdo %p sirp %p Request d-irp(D%x)",
        Fdo,
        Irp,
        devicePowerState - 1));

    status = PoRequestPowerIrp(fdoExtension->DeviceObject,
        irpStack->MinorFunction,
        powerState,
        SdbusFdoSystemRequestPowerCompletion,
        NULL,
        NULL);

    if (!NT_SUCCESS(status)) {
        (VOID) SdbusFdoCompletePowerIrp(Fdo, Irp, status);
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}

VOID
SdbusCheckCacheOnHibernate(
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PVOID Context
    )

{

    PFDO_EXTENSION FdoExtension;
    UCHAR Index;

    FdoExtension = (PFDO_EXTENSION)Context;
    for (Index = 0; Index < FdoExtension->NumberOfSockets; Index += 1) {
        if (FdoExtension->Socket[Index]->HostAccessMode == SDBUS_FUNCTION_TYPE_MMC_MEMORY) {
            SdbusExecuteWorkSynchronous(SDWP_MMC_CHECK_WRITE_CACHE, 
                                        FdoExtension->Socket[Index], 
                                        NULL, 
                                        NULL);
        }
    }

    if (FdoExtension->PendingSIrp != NULL) {
        DebugPrint((SDBUS_DEBUG_POWER,
                    "fdo %p sirp %p Stack power transition completed. Complete the s-irp now",
                    DeviceObject,
                    FdoExtension->PendingSIrp));

        (VOID) SdbusFdoCompletePowerIrp(DeviceObject,
                                        FdoExtension->PendingSIrp,
                                        STATUS_SUCCESS);

        FdoExtension->PendingSIrp = NULL;
    }
}

VOID
SdbusFdoSystemRequestPowerCompletion(
    __in PDEVICE_OBJECT Fdo,
    __in UCHAR MinorFunction,
    __in POWER_STATE PowerState,
    __in_opt PVOID Context,
    __in PIO_STATUS_BLOCK IoStatus
    )

/*++

Routine Description

    Handles system power state IRPs for the host controller.

Arguments

    Fdo - Pointer to the functional device object for the sd controller

    MinorFunction -

    PowerState -

    Context

    Irp - Pointer to the Irp for the power dispatch

Return value

    status

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;

    UNREFERENCED_PARAMETER(MinorFunction);
    UNREFERENCED_PARAMETER(PowerState);
    UNREFERENCED_PARAMETER(Context);

    NT_ASSERT(MinorFunction == IRP_MN_SET_POWER);

    if (fdoExtension->PendingSIrp) {

        DebugPrint((SDBUS_DEBUG_POWER,
            "fdo %p sirp %p Stack power transition completed. Complete the s-irp now",
            Fdo,
            fdoExtension->PendingSIrp));

        (VOID) SdbusFdoCompletePowerIrp(Fdo,
            fdoExtension->PendingSIrp,
            IoStatus->Status);
        fdoExtension->PendingSIrp = NULL;
    }
}

NTSTATUS
SdbusFdoSetDevicePowerState(
    __in PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    )

/*++

Routine Description

    Handles device power state IRPs for the controller.

Arguments

    Fdo - Pointer to the functional device object for the sd controller

    Irp - Pointer to the Irp for the power dispatch

Return value

    NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PPDO_EXTENSION pdoExtension = NULL;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    DEVICE_POWER_STATE devicePowerState =
        irpStack->Parameters.Power.State.DeviceState;    
    ULONG socketIndex = 0;
    PDEVICE_OBJECT p = NULL;    
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;
    ULONG64 QpcTime;

    //
    // process the irp only if we are started
    //

    if (fdoExtension->PnpState == SDBUS_DEVICE_NOT_STARTED) {

        DebugPrint((SDBUS_DEBUG_WARNING,
            "fdo %p dirp %p Device not started yet",
            Fdo,
            Irp));

        return SdbusFdoCompletePowerIrp(Fdo,
            Irp,
            STATUS_SUCCESS);
    }

    if (devicePowerState == PowerDeviceD0) {        
        SdbusAddFxDbgLog(fdoExtension,
                         'D0st',
                         fdoExtension->FxPState,
                         fdoExtension->FxState,
                         (ULONG_PTR)KeQueryInterruptTimePrecise(&QpcTime));
    } else {
        SdbusAddFxDbgLog(fdoExtension,
                         'D3st',
                         fdoExtension->FxPState,
                         fdoExtension->FxState,
                         (ULONG_PTR)KeQueryInterruptTimePrecise(&QpcTime));
    }

    if (fdoExtension->NumberOfSockets == 0) {

        SET_DEVICE_POWER_STATE(fdoExtension, devicePowerState);

        DebugPrint((SDBUS_DEBUG_WARNING,
            "fdo %p dirp %p No socket to power up",
            Fdo,
            Irp));

        return SdbusFdoForwardPowerIrp(Fdo, Irp);
    }

    //
    // If we're already in the requested D-state, then simply forward the
    // IRP.
    //

    if (fdoExtension->DevicePowerState == devicePowerState) {
        return SdbusFdoForwardPowerIrp(Fdo, Irp);
    }

    if ((fdoExtension->LivePdoCount == 0) &&
        (devicePowerState == PowerDeviceD0)) {

            //
            // If the D0 was as as result of an S0, complete the S0 IRP now.
            //

            if (fdoExtension->PendingSIrp != NULL) {

                //
                // Use fast resume if is power up and there was no live PDOs before
                // going into sleep
                //

                DebugPrint((SDBUS_DEBUG_POWER,
                    "fdo %p sirp %p Fast resume: complete the s-irp",
                    Fdo,
                    fdoExtension->PendingSIrp));

                SdbusFdoCompletePowerIrp(Fdo,
                    fdoExtension->PendingSIrp,
                    STATUS_SUCCESS);

                fdoExtension->PendingSIrp = NULL;
            }
    }

    if (devicePowerState != PowerDeviceD0) {

        PSD_SET_POWER_CONTEXT powerContext = &fdoExtension->PowerContext;
        PDEVICE_OBJECT pdo;

        //
        // All of sdbus' child PDOs have already been powered down.
        //

        if (fdoExtension->PoweredPdoCount == 0) {

            SET_DEVICE_POWER_STATE(fdoExtension, devicePowerState);

            for (socketIndex = 0; 
                socketIndex < fdoExtension->NumberOfSockets; 
                socketIndex++) {

                    for (p = fdoExtension->Socket[socketIndex]->PdoList; p != NULL; p = pdoExtension->NextPdo) {
                        pdoExtension = p->DeviceExtension;
                        pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);
                    }
            }

            if (IsDeviceFlagSet(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE)) {

                //
                // Cancel the card interrupt poller.
                //

                KeCancelTimer(&fdoExtension->PollTimer);
                ResetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);
            }

            if ((fdoExtension->GpioCardDetectHandle == NULL) &&
                (fdoExtension->SdbusGpioInterruptObject != NULL)) {

                    //
                    // Reconnect GPIO interrupt, in case GPIO interrupt line is correctly hooked up
                    // and card detect pin is correctly wired to the host register, since we disconnected
                    // it when we powered on.
                    //

#if defined(CAPTURE_ACTIVE_IDLE)
                    InterlockedIncrement(&fdoExtension->Dbg.ReportGPIOInterruptActiveCount);
#endif            

                    interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
                    interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusGpioInterruptObject;
                    IoReportInterruptActive(&interruptActiveStateParameters);
            }

            //
            // Disconnect our interrupt handlers from the chain.
            //

            if (fdoExtension->SdbusInterruptObject) {

#if defined(CAPTURE_ACTIVE_IDLE)
                InterlockedIncrement(&fdoExtension->Dbg.ReportDIRQLInterruptInactiveCount);
#endif            

                interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
                interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusInterruptObject;
                IoReportInterruptInactive(&interruptActiveStateParameters);
            }

            if ((fdoExtension->SdbusPassiveInterruptObject) && 
                (fdoExtension->SdbusPassiveInterruptActive != FALSE)) {

#if defined(CAPTURE_ACTIVE_IDLE)
                InterlockedIncrement(&fdoExtension->Dbg.ReportPASSIVEInterruptInactiveCount);
#endif                            

                interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
                interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusPassiveInterruptObject;
                IoReportInterruptInactive(&interruptActiveStateParameters);
            }

            fdoExtension->Flags |= SDBUS_FDO_OFFLINE;

            DebugPrint((SDBUS_DEBUG_POWER,
                "fdo %p dirp %p Power down complete. Forwarding the d-irp",
                Fdo,
                Irp));

            SdbusAddFxDbgLog(fdoExtension,
                             'D3Cp',
                             fdoExtension->FxPState,
                             fdoExtension->FxState,
                             (ULONG_PTR)KeQueryInterruptTimePrecise(&QpcTime));

            return SdbusFdoForwardPowerIrp(fdoExtension->DeviceObject, Irp); 
        } 

        //
        // We need to wait until all of our child PDOs have been powered
        // down before completing the Dx IRP.
        //

        IoMarkIrpPending(Irp);

        NT_ASSERT(powerContext != NULL);

        powerContext->Irp = Irp;
        powerContext->SocketCount = fdoExtension->NumberOfSockets;

        //
        // We may need to power down the child PDOs individually; specifically,
        // NDIS does not send a D3 IRP to the bus driver when handling a system
        // sleep transition (S3).  To prevent interrupt storms for SDIO Wi-Fi devices,
        // sdbus has to iterate through all if its child PDOs to see if they
        // have all been powered down properly.  If they haven't, sdbus *fakes*
        // a D-state transition for the powered/offending PDOs.
        //

        for (socketIndex = 0; 
            socketIndex < fdoExtension->NumberOfSockets; 
            socketIndex++) {

                for (pdo = fdoExtension->Socket[socketIndex]->PdoList; 
                    pdo != NULL; 
                    pdo = pdoExtension->NextPdo) {

                        pdoExtension = pdo->DeviceExtension;

                        if (pdoExtension->FunctionType == SDBUS_FUNCTION_TYPE_SD_MEMORY) {
                            KeCancelTimer(&SD_DEFAULT_WORK_ENGINE(pdoExtension->Socket)->RetuningTimer);
                        }

                        if (pdoExtension->DevicePowerState == PowerDeviceD0) {

                            SET_DEVICE_POWER_STATE(pdoExtension, devicePowerState);

                            //
                            // Move the PDO to a low power state.
                            //

                            SdbusActivateSocket(pdoExtension->Socket,
                                (devicePowerState == PowerDeviceD2 ) ? SA_REQUEST_FUNCTION_D2_STOP : SA_REQUEST_FUNCTION_D3_STOP,
                                pdoExtension,
                                SdbusFdoSocketPowerDownCompletion,
                                powerContext);
                        }
                }
        }

    } else {

        SDPORT_InitializePciConfigSpace(fdoExtension);

        //
        // Powering up the device. Send the IRP to the bus driver first
        //

        IoMarkIrpPending(Irp);
        IoCopyCurrentIrpStackLocationToNext(Irp);

        //
        // Set our completion routine in the Irp..
        //

        IoSetCompletionRoutine(Irp,
            SdbusFdoDevicePowerUpIrpCompletion,
            Fdo,
            TRUE,
            TRUE,
            TRUE);

        DebugPrint((SDBUS_DEBUG_POWER,
            "fdo %p dirp %p Start to power up. Forwarding the d-irp first",
            Fdo,
            Irp));

        PoCallDriver(fdoExtension->LowerDevice, Irp);
    }

    return STATUS_PENDING;
}

VOID
SdbusFdoSocketPowerDownCompletion(
    __in PSD_SOCKET Socket,
    __in PVOID Context,
    __in NTSTATUS Status
    )

/*++

This activation completion routine is called when
the SdbusFdoSetDevicePowerState is *FAKING* the D2/D3 
transitions for all the child PDOs still in D0. This happens
on an S3 sleep sent to the FDO.

--*/

{
    PFDO_EXTENSION fdoExtension = Socket->FdoExtension;
    PPDO_EXTENSION pdoExtension = NULL;
    PSD_SET_POWER_CONTEXT powerContext = Context;
    PIRP Irp = powerContext->Irp;        
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;
    PDEVICE_OBJECT p = NULL;
    ULONG64 QpcTime;

    UNREFERENCED_PARAMETER(Status);

    if (InterlockedDecrement(&fdoExtension->PoweredPdoCount) == 0) {

        DebugPrint((SDBUS_DEBUG_POWER,
            "fdo %p dirp %p All PDOs powered down successfully",
            fdoExtension->DeviceObject,
            Irp));

        SET_DEVICE_POWER_STATE(fdoExtension, PowerDeviceD3);

        //
        // If we're disabling F-states (because this is the 
        // crashdump device), and we're the last PDO down,
        // then disable the block on the host leaving F0.
        //

        if (SdDisableFStatesCrashdump && 
            (fdoExtension->PagingPathCount != 0) &&
            (fdoExtension->PoweredPdoCount == 0)) {

            SdbusFxSetComponentWake(fdoExtension->FxHandle, 0, FALSE);        
        }

        //
        // We're powering down the host controller, crashdump cannot be done
        // if the host controller is off. Evaluate all the child pdos and mark
        // them as such.
        //

        for (p = Socket->PdoList; p != NULL; p = pdoExtension->NextPdo) {
            pdoExtension = p->DeviceExtension;
            pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);
        }

        if (IsDeviceFlagSet(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE)) {

            //
            // Cancel the card interrupt poller
            //

            KeCancelTimer(&fdoExtension->PollTimer);
            ResetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);
        }

        if ((fdoExtension->GpioCardDetectHandle == NULL) &&
            (fdoExtension->SdbusGpioInterruptObject != NULL)) {

                //
                // Reconnect GPIO interrupt, in case GPIO interrupt line is correctly hooked up
                // and card detect pin is correctly wired to the host register, since we disconnected
                // it when we powered on.
                //

#if defined(CAPTURE_ACTIVE_IDLE)
                InterlockedIncrement(&fdoExtension->Dbg.ReportGPIOInterruptActiveCount);
#endif                            

                interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
                interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusGpioInterruptObject;
                IoReportInterruptActive(&interruptActiveStateParameters);
        }

        //
        // Disconnect our interrupt handler from the chain.
        //

#if defined(CAPTURE_ACTIVE_IDLE)
        InterlockedIncrement(&fdoExtension->Dbg.ReportDIRQLInterruptInactiveCount);
#endif                            

        interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
        interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusInterruptObject;
        IoReportInterruptInactive(&interruptActiveStateParameters);

        if (fdoExtension->SdbusPassiveInterruptActive != FALSE) {

#if defined(CAPTURE_ACTIVE_IDLE)
            InterlockedIncrement(&fdoExtension->Dbg.ReportPASSIVEInterruptInactiveCount);
#endif                            

            interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
            interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusPassiveInterruptObject;
            IoReportInterruptInactive(&interruptActiveStateParameters);
        }

        //
        // If any children are in-band wake armed, we need to make sure the card interrupt is enabled before the
        // host controller enters D3.
        //

        if (Socket->FdoExtension->WakeInterruptForward) {
            SdbusEnableEvents(Socket, SDBUS_EVENT_CARD_INTERRUPT);
        }

        SdbusSetCardAndWakeInterruptForward(Socket, TRUE, FALSE);

        fdoExtension->Flags |= SDBUS_FDO_OFFLINE;

        DebugPrint((SDBUS_DEBUG_POWER,
            "fdo %p dirp %p Power down complete. Forwarding the d-irp",
            fdoExtension->DeviceObject,
            Irp));

        SdbusFdoForwardPowerIrp(fdoExtension->DeviceObject, Irp);

        SdbusAddFxDbgLog(fdoExtension,
                         'D3Cp',
                         fdoExtension->FxPState,
                         fdoExtension->FxState,
                         (ULONG_PTR)KeQueryInterruptTimePrecise(&QpcTime));
    }
}

NTSTATUS
SdbusFdoDevicePowerUpIrpCompletion(
    PDEVICE_OBJECT Fdo,
    PIRP Irp,
    PVOID Context
    )
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PSD_SET_POWER_CONTEXT powerContext = &fdoExtension->PowerContext;
    ULONG socketIndex;
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;
    DPR_STATE_CONTEXT DprContext = {0};

    UNREFERENCED_PARAMETER(Context);

    NT_ASSERT(IoGetCurrentIrpStackLocation(Irp)->Parameters.Power.State.DeviceState == 
        PowerDeviceD0);

    if (fdoExtension->NumberOfSockets == 0) {

        DebugPrint((SDBUS_DEBUG_WARNING,
            "fdo %p dirp %p No sockets to power up. Completing the d-irp",
            Fdo,
            Irp));

        if (fdoExtension->FxDevicePowerRequiredInProgress != FALSE) {

#if defined(CAPTURE_ACTIVE_IDLE)
            InterlockedIncrement(&fdoExtension->Dbg.FxDebugDeviceReportDevicePoweredOnCount);
#endif

            DprContext.FdoExtension = fdoExtension;
            DprContext.State = FALSE;
            KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);
            fdoExtension->FxDevicePowerRequiredInProgress = FALSE;
        }

        PoFxReportDevicePoweredOn(fdoExtension->FxHandle);
        PoStartNextPowerIrp(Irp);
        IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
        return STATUS_CONTINUE_COMPLETION;
    }

    DebugPrint((SDBUS_DEBUG_POWER,
        "fdo %p dirp %p Powering up. Will verify all the sockets",
        Fdo,
        Irp));

    NT_ASSERT(powerContext != NULL);

    powerContext->Irp = Irp;
    powerContext->SocketCount = fdoExtension->NumberOfSockets;
    powerContext->RelationChanged = FALSE;

    //
    // reconnect our interrupt object.
    //

#if defined(CAPTURE_ACTIVE_IDLE)
    InterlockedIncrement(&fdoExtension->Dbg.ReportDIRQLInterruptActiveCount);
#endif                            

    interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
    interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusInterruptObject;
    IoReportInterruptActive(&interruptActiveStateParameters);

    if (fdoExtension->SdbusPassiveInterruptActive!= FALSE) {

#if defined(CAPTURE_ACTIVE_IDLE)
        InterlockedIncrement(&fdoExtension->Dbg.ReportPASSIVEInterruptActiveCount);
#endif                            

        interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
        interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusPassiveInterruptObject;
        IoReportInterruptActive(&interruptActiveStateParameters);
    }

    if ((fdoExtension->GpioCardDetectHandle == NULL) &&
        (fdoExtension->SdbusGpioInterruptObject != NULL)) {

            //
            // Disconnect GPIO interrupt, in case GPIO interrupt line is correctly hooked up
            // and card detect pin is correctly wired to the host register, since when we are
            // powered up, normal interrupt will fire and we can read host register for card
            // detect. If we don't disconnect, some platforms will double fire both normal and
            // GPIO interrupts.
            //

#if defined(CAPTURE_ACTIVE_IDLE)
            InterlockedIncrement(&fdoExtension->Dbg.ReportGPIOInterruptInactiveCount);    
#endif                            

            interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
            interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusGpioInterruptObject;
            IoReportInterruptInactive(&interruptActiveStateParameters);
    }

    //
    // We have powered up and connected the in-band interrupt, so disarm Wait Wake if necessary.
    //

    SdbusFdoDisarmWake(fdoExtension);

    //
    // Powering up
    //

    SDPORT_InitController(fdoExtension);

    //
    // Now the controller registers should be accessible
    //

    fdoExtension->Flags &= ~SDBUS_FDO_OFFLINE;

    //
    // Getting back to D0, set the poll timer on
    //

    if ((fdoExtension->UsePollTimer == POLL_TIMER_ENABLED) ||
        (fdoExtension->UsePollTimer == POLL_TIMER_DIAGNOSTIC)) {
            LARGE_INTEGER dueTime;

            SetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);

            //
            // Set first fire to twice the peroidic interval - just
            //

            dueTime.QuadPart = -(LONG) PollTimerInterval * 1000 * 10 * 2;

            KeSetTimerEx(&(fdoExtension->PollTimer),
                dueTime,
                PollTimerInterval,
                &fdoExtension->PollTimerDpc);
    }

    for (socketIndex = 0;
         socketIndex < fdoExtension->NumberOfSockets;
         socketIndex++) {

            PSD_SOCKET socket = fdoExtension->Socket[socketIndex];

            //
            // If we're restoring controller context, then just signal completion.
            //

            if (socket->SdhcEnableSavedContext) {
                SdbusFdoSocketPowerUpCompletion(socket, powerContext, STATUS_SUCCESS);
                continue;
            }

            //
            // ActivateSocket will check to see if the same card is in the slot
            // as the one we had before the socket was powered down
            //

            if (socket->CardInserted ||
                (fdoExtension->GpioCardDetectHandle == NULL)) {

                SdbusActivateSocket(socket,
                                    SA_REQUEST_VERIFY_CARD_ON_POWER_UP,
                                    NULL,
                                    SdbusFdoSocketPowerUpCompletion,
                                    powerContext);

            } else {
                SdbusFdoSocketPowerUpCompletion(socket, powerContext, STATUS_SUCCESS);
                continue;
            }
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}

VOID
SdbusFdoSocketPowerUpCompletion(
    __in PSD_SOCKET Socket,
    __in PVOID Context,
    __in NTSTATUS Status
    )

/*++

Routine Description:

    Activate completion routine for a FDO power up activation.

Arguments:

    Socket - pointer to structure which represents the physical socket

    Context - PSD_SET_POWER_CONTEXT

    Status - error code of activation

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION fdoExtension = Socket->FdoExtension;
    PPDO_EXTENSION pdoExtension = NULL;
    PDEVICE_OBJECT p = NULL;
    PSD_SET_POWER_CONTEXT powerContext = Context;
    PIRP Irp = powerContext->Irp;
    DPR_STATE_CONTEXT DprContext = {0};
    ULONG64 QpcTime;

    if (!NT_SUCCESS(Status)) {

        DebugPrint((SDBUS_DEBUG_FAIL,
            "fdo %p dirp %p skt %p Failed to power up",
            fdoExtension->DeviceObject,
            Irp,
            Socket));

        SET_SOCKET_PDO_FLAG(Socket, SDBUS_PDO_PHYSICALLY_REMOVED);
        SdbusCleanIoQueue(SD_DEFAULT_WORK_ENGINE(Socket));
        powerContext->RelationChanged = TRUE;
    }

    if (InterlockedDecrement(&powerContext->SocketCount) == 0) {

        if (powerContext->RelationChanged) {

            DebugPrint((SDBUS_DEBUG_FAIL,
                "fdo %p dirp %p Some sockets failed to power up. Invalidate the device relations",
                fdoExtension->DeviceObject,
                Irp));

            //
            // If the device does not exist, then there's no reason to
            // invalidate the device relations any further, as it's already
            // been done by this point.
            //

            if (Status != STATUS_DEVICE_DOES_NOT_EXIST) {

#if defined(CAPTURE_ACTIVE_IDLE)
                InterlockedIncrement(&Socket->Dbg.ActivationInvalidateDevRelCount);
#endif

                IoInvalidateDeviceRelations(Socket->FdoExtension->Pdo,
                    BusRelations);
            }
        } else {

            DebugPrint((SDBUS_DEBUG_POWER,
                "fdo %p dirp %p All sockets powered up successfully",
                fdoExtension->DeviceObject,
                Irp));
        }

        NT_ASSERT(IoGetCurrentIrpStackLocation(Irp)->Parameters.Power.State.DeviceState == 
            PowerDeviceD0);

        SET_DEVICE_POWER_STATE(fdoExtension, PowerDeviceD0);

        //
        // We just brought the host controller into D0. Evaluate all
        // the child PDOs and check if they're in a state where
        // crashdump is possible.
        //

        for (p = Socket->PdoList; p != NULL; p = pdoExtension->NextPdo) {
            pdoExtension = p->DeviceExtension;
            pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);
        }

        (VOID) SdbusFdoCompletePowerIrp(fdoExtension->DeviceObject,
            Irp,
            Status);

        //
        // Finish DPR callback.
        //

        if (fdoExtension->FxDevicePowerRequiredInProgress != FALSE) {

#if defined(CAPTURE_ACTIVE_IDLE)
            InterlockedIncrement(&fdoExtension->Dbg.FxDebugDeviceReportDevicePoweredOnCount);
#endif

            DprContext.FdoExtension = fdoExtension;
            DprContext.State = FALSE;
            KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);
            fdoExtension->FxDevicePowerRequiredInProgress = FALSE;
        }

        PoFxReportDevicePoweredOn(fdoExtension->FxHandle);

        SdbusAddFxDbgLog(fdoExtension,
                         'D0Cp',
                         fdoExtension->FxPState,
                         fdoExtension->FxState,
                         (ULONG_PTR)KeQueryInterruptTimePrecise(&QpcTime));
    }
}

// ---------------------------------------------------------------------------
//
// PDO Routines
//
// ---------------------------------------------------------------------------


NTSTATUS
SdbusPdoCompletePowerIrp(
    __inout PIRP Irp,
    __in NTSTATUS Status
    )

/*++

Routine Description

    Completion routine for the Power Irp directed to the PDO of the SD
    function.

Arguments

    Irp - Irp that needs to be completed

    Status - Status to be used to complete the IRP

Return Value

    NTSTATUS

--*/

{
    Irp->IoStatus.Status = Status;
    PoStartNextPowerIrp(Irp);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

NTSTATUS
SdbusPdoSetPowerState(
    __inout PDEVICE_OBJECT Pdo,
    __inout PIRP Irp
    )

/*++

Routine Description

    Dispatches the IRP based on whether a system power state or device power
    state transition is requested

Arguments

    Pdo - Pointer to the physical device object for the SD function

    Irp - Pointer to the Irp for the power dispatch

Return value

    status

--*/

{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status;

    switch (irpStack->Parameters.Power.Type) {

    case DevicePowerState:
        status = SdbusPdoSetDevicePowerState(Pdo, Irp);
        break;

    case SystemPowerState: {

        PPDO_EXTENSION pdoExtension = Pdo->DeviceExtension;

        if ((irpStack->Parameters.Power.ShutdownType == PowerActionSleep) ||
            (irpStack->Parameters.Power.ShutdownType == PowerActionHibernate)) {

                //
                // System is going into low power state.  Increment Media Change Count to force
                // file system to verify the volume when we resume.
                //
                pdoExtension->MediaChangeCount++;
        }

        pdoExtension->SystemPowerState =
            irpStack->Parameters.Power.State.SystemState;
        status = SdbusPdoCompletePowerIrp(Irp, STATUS_SUCCESS);


        break;
                           }

    default:
        status = SdbusPdoCompletePowerIrp(Irp, Irp->IoStatus.Status);
    }

    return status;
}

NTSTATUS
SdbusPdoSetDevicePowerState(
    __in PDEVICE_OBJECT Pdo,
    __inout PIRP Irp
    )

/*++

Routine Description

    Handles the device power state transition for the given SD function.

Arguments

    Pdo - Pointer to the physical device object for the SD function

    Irp - Irp for the system state transition

Return value

    NTSTATUS

--*/

{
    PPDO_EXTENSION pdoExtension = Pdo->DeviceExtension;
    PFDO_EXTENSION fdoExtension = pdoExtension->Socket->FdoExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    DEVICE_POWER_STATE newDevicePowerState;
    PDEVICE_OBJECT ChildPdo = NULL;
    PPDO_EXTENSION ChildPdoExtension = NULL;
    BOOLEAN NeedsSaveRestore = FALSE;

#if DBG
    NT_ASSERT((fdoExtension->DevicePowerState == PowerDeviceD0) &&
        (fdoExtension->FxState == 0));
#endif

    newDevicePowerState = irpStack->Parameters.Power.State.DeviceState;

    if (newDevicePowerState == pdoExtension->DevicePowerState) {
        SdbusFxComponentIdle(fdoExtension, 0);
        return SdbusPdoCompletePowerIrp(Irp, STATUS_SUCCESS);
    }

    IoMarkIrpPending(Irp);

    if (newDevicePowerState == PowerDeviceD0) {

        DebugPrint((SDBUS_DEBUG_POWER,
            "pdo %p dirp %p Powre up pdo. Pend the d-irp",
            Pdo,
            Irp));

        //
        // If we are the first PDO to come up, we need to walk all the 
        // child PDOs, to check if any of them are in D2. If any child 
        // PDO is in D2, we have to save/restore the host controller 
        // context.
        //

        if ((pdoExtension->Socket->SdhcEnableSavedContext) &&
            (fdoExtension->PoweredPdoCount == 0)) {

            for (ChildPdo = pdoExtension->Socket->PdoList; 
                ChildPdo != NULL; 
                ChildPdo = ChildPdoExtension->NextPdo) {

                ChildPdoExtension = ChildPdo->DeviceExtension;
                if (ChildPdoExtension->PnpState == SDBUS_DEVICE_STARTED &&
                    (pdoExtension->DevicePowerState == PowerDeviceD2)) {
                        NeedsSaveRestore = TRUE;
                        break;
                }
            }

            //
            // Restore context if all the PDOs were powered down,
            // and any child PDO is coming from D2.
            //

            if (NeedsSaveRestore) {
                SDPORT_RestoreContext(pdoExtension->Socket, &pdoExtension->Socket->SdhcSavedContext);                
            }
        }

        //
        // Power up, initialize function
        //

        if (pdoExtension->DevicePowerState == PowerDeviceD2) {

            //
            // If we are in D2, then we need special handling to go back to D0
            //

            SdbusActivateSocket(pdoExtension->Socket,
                SA_REQUEST_FUNCTION_D2_START,
                pdoExtension,
                SdbusPdoSetDevicePowerActivateComplete,
                Irp);

        } else {

            SdbusActivateSocket(pdoExtension->Socket,
                SA_REQUEST_FUNCTION_D3_START,
                pdoExtension,
                SdbusPdoSetDevicePowerActivateComplete,
                Irp);

        }

    } else {

        DebugPrint((SDBUS_DEBUG_POWER,
            "pdo %p dirp %p Power down pdo. Pend the d-irp",
            Pdo,
            Irp));

        //
        // Only SD cards need a retuning timer potentially, and multi-function
        // cards can only have one storage function. Therefore, we know that if
        // this PDO represents an SD storage function, we can safely turn off
        // the retuning timer if it's still active.
        //

        if (pdoExtension->FunctionType == SDBUS_FUNCTION_TYPE_SD_MEMORY) {
            KeCancelTimer(&SD_DEFAULT_WORK_ENGINE(pdoExtension->Socket)->RetuningTimer);
        }

        SET_DEVICE_POWER_STATE(pdoExtension, newDevicePowerState);

        if (fdoExtension->PoweredPdoCount == 1) { // if we are the last Powered Pdo
            if (pdoExtension->Socket->SdhcEnableSavedContext) { 

                //
                // Save context if any child PDO was powered down into D2.
                //

                for (ChildPdo = pdoExtension->Socket->PdoList;
                    ChildPdo != NULL;
                    ChildPdo = ChildPdoExtension->NextPdo) {

                    ChildPdoExtension = ChildPdo->DeviceExtension;
                    if ((ChildPdoExtension->PnpState == SDBUS_DEVICE_STARTED) &&
                        (pdoExtension->DevicePowerState == PowerDeviceD2)) {

                        NeedsSaveRestore = TRUE;
                        break;
                    }
                }

                //
                // Save context if all the PDOs were powered down,
                // and any child PDO went into D2.
                //

                if (NeedsSaveRestore) {
                    SDPORT_SaveContext(pdoExtension->Socket, &pdoExtension->Socket->SdhcSavedContext);
                }
            }
        }

        //
        // moving to a low power state
        //

        if (newDevicePowerState == PowerDeviceD2) {

            //
            // If we are going to D2, then we need special handling
            //

            SdbusActivateSocket(pdoExtension->Socket,
                SA_REQUEST_FUNCTION_D2_STOP,
                pdoExtension,
                SdbusPdoSetDevicePowerActivateComplete,
                Irp);

        } else {

            SdbusActivateSocket(pdoExtension->Socket,
                SA_REQUEST_FUNCTION_D3_STOP,
                pdoExtension,
                SdbusPdoSetDevicePowerActivateComplete,
                Irp);

        }

    }

    return STATUS_PENDING;
}

VOID
SdbusPdoSetDevicePowerActivateComplete(
    __inout PSD_SOCKET Socket,
    __inout PVOID Context,
    __in NTSTATUS Status
    )

/*++

Routine Description:

    Activate completion routine for a PDO power up activation.

Arguments:

    Socket - pointer to structure which represents the physical socket

    Context - power IRP

    Status - error code of activation

Return Value:

    VOID

--*/

{
    PIRP Irp = Context;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PPDO_EXTENSION pdoExtension = irpStack->DeviceObject->DeviceExtension;
    PFDO_EXTENSION fdoExtension = pdoExtension->Socket->FdoExtension;

    if (NT_SUCCESS(Status)) {

        //
        // Set the device state for power-up. For power-down, we've done that
        // earlier.
        //

        if (irpStack->Parameters.Power.State.DeviceState == PowerDeviceD0) {

            SET_DEVICE_POWER_STATE(pdoExtension, PowerDeviceD0);
            InterlockedIncrement(&fdoExtension->PoweredPdoCount);
            NT_ASSERT((ULONG)fdoExtension->PoweredPdoCount <= fdoExtension->LivePdoCount);

            //
            // If we're disabling F-states (because this is the 
            // crashdump device), and we're the first PDO up,
            // then re-enable the block on the host leaving F0.
            //

            if (SdDisableFStatesCrashdump &&
                (fdoExtension->PagingPathCount != 0) &&
                (fdoExtension->PoweredPdoCount == 1)) {

                SdbusFxSetComponentWake(fdoExtension->FxHandle, 0, TRUE);        
            }

            //
            // we just brought the device back to D0. if the controller is also in D0,
            // crashdump is ok to go.
            //

            pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);

        } else {

            InterlockedDecrement(&fdoExtension->PoweredPdoCount);
            NT_ASSERT(fdoExtension->PoweredPdoCount >= 0);

            //
            // If we're disabling F-states (because this is the 
            // crashdump device), and we're the last powered PDO,
            // then disable the block on the host leaving F0.
            //

            if (SdDisableFStatesCrashdump && 
                (fdoExtension->PagingPathCount != 0) &&
                (fdoExtension->PoweredPdoCount == 0)) {

                SdbusFxSetComponentWake(fdoExtension->FxHandle, 0, FALSE);        
            }           

            //
            // we just took the device down to a lower power state.
            // unless it is D2, and the host controller is in D0,
            // this means crashdump cannot be done.
            //

            pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);

        }

        DebugPrint((SDBUS_DEBUG_POWER,
            "pdo %p dirp %p PDO power transition succeed. Complete d-irp",
            pdoExtension->DeviceObject,
            Irp));

    } else {

        if (irpStack->Parameters.Power.State.DeviceState == PowerDeviceD0) {

            //
            // if we failed to bring the device into D0, we can't write a crashdump
            //

            pdoExtension->CrashdumpDeviceReady = FALSE;

            SET_SOCKET_PDO_FLAG(Socket, SDBUS_PDO_FAILED);
            SdbusCleanIoQueue(SD_DEFAULT_WORK_ENGINE(Socket));

            DebugPrint((SDBUS_DEBUG_FAIL,
                "pdo %p dirp %p PDO power up failed. Invalidate device state",
                pdoExtension->DeviceObject,
                Irp));

            //
            // Invalidate the device state. Pnp driver will send a remove
            // device IRP.
            //

            IoInvalidateDeviceState(irpStack->DeviceObject);

        } else {

            //
            // we failed to power the device down. if the device is currently in D2 or D0,
            // crashdump is still possible.
            //

            pdoExtension->CrashdumpDeviceReady = SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);

            //
            // If we failed to power down, silently return for now. We will
            // drain the queue later if power-up fails.
            //

            DebugPrint((SDBUS_DEBUG_WARNING,
                "pdo %p dirp %p PDO power down failed. Ignore it for now",
                pdoExtension->DeviceObject,
                Irp));

            InterlockedDecrement(&fdoExtension->PoweredPdoCount);
            NT_ASSERT(fdoExtension->PoweredPdoCount >= 0);

            Status = STATUS_SUCCESS;
        }
    }

    SdbusFxComponentIdle(fdoExtension, 0);

    SdbusPdoCompletePowerIrp(Irp, Status);
}

VOID
SdbusFxComponentIdleStateCallback(
    PVOID Context,
    ULONG Component,
    ULONG State
    )

/*++
Routine Description:

    Set appropriate runtime power state.

Arguments:

    Context - FDO's device extension

    Component - component index

    State - F-state to be transitioned into

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION fdoExtension = (PFDO_EXTENSION) Context;
    PPDO_EXTENSION pdoExtension = NULL;
    PDEVICE_OBJECT p = NULL;
    ULONG s = 0;
    PSD_SOCKET Socket = NULL;

#if DBG    
    DebugPrint((SDBUS_DEBUG_POWER,
        "SdbusFxComponentIdleStateCallback - "
        "FdoExtension: %p, State: %d\n", 
        fdoExtension, 
        State));

    NT_ASSERT(fdoExtension->Active == FALSE);
    NT_ASSERT(fdoExtension->DevicePowerState == PowerDeviceD0);
    NT_ASSERT(Component == 0);
#endif

    fdoExtension->FxState = State;

    SdbusAddFxDbgLog(fdoExtension,
        'IsCb',
        fdoExtension->Active,
        fdoExtension->FxState,
        fdoExtension->DevicePowerState);

    NT_ASSERT(fdoExtension->FxState == SDBUS_FX_CLOCK_GATED ||
              fdoExtension->FxState == SDBUS_FX_ON);

    //
    // We just changed the F state of the host controller. 
    // Evaluate all the child PDOs and check if they're 
    // in a state where crashdump is possible.
    //

    for (s = 0; s < fdoExtension->NumberOfSockets; s++) {
        Socket = fdoExtension->Socket[s];
        for (p = Socket->PdoList; p != NULL; p = pdoExtension->NextPdo) {
            pdoExtension = p->DeviceExtension;
            pdoExtension->CrashdumpDeviceReady = 
                SdbusCheckCrashdumpDeviceReady(fdoExtension, pdoExtension);
        }
    }


    PoFxCompleteIdleState(fdoExtension->FxHandle, Component);
}

VOID
SdbusFxComponentActiveCallback(
    PVOID Context,
    ULONG Component
    )

/*++

Routine Description:

    Set component to the active condition and process any work that had to be deferred 
    until component activation was complete.

Arguments:

    Context - FDO's device extension

    Component - component index

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION FdoExtension;
    PIRP Irp;
    PIO_STACK_LOCATION IrpSp;
    PLIST_ENTRY ListEntry;
    KLOCK_QUEUE_HANDLE LockHandle;
    PDEVICE_OBJECT Pdo;
    PPDO_EXTENSION PdoExtension;
    PSDBUS_REQUEST_PACKET Sdrp;
    NTSTATUS Status;
    ULONG mask;
    ULONG i;
    BOOLEAN WorkItemQueued = FALSE;
    ULONG ActiveSocketCount = 0;
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;

    UNREFERENCED_PARAMETER(Component);

    FdoExtension = (PFDO_EXTENSION) Context;

    DebugPrint((SDBUS_DEBUG_POWER,
        "SdbusFxComponentActiveCallback: fdo: %p",
        FdoExtension->DeviceObject));

#if defined(CAPTURE_ACTIVE_IDLE)
    InterlockedIncrement(&FdoExtension->Dbg.FxDebugActiveCallbackCount);
    FdoExtension->Dbg.FxDebugActiveState = TRUE;
#endif

    //
    // Connect the DIRQL ISR. If passive interrupt were active. While we were idle, we
    // were receiving interrupts with it, which means the DIRQL ISR was disconnected 
    // at the time.
    //
    // It should also be connected if W/W was previously armed since we disconnected it
    // at that time.
    //

    if (FdoExtension->SdbusPassiveInterruptActive != FALSE) {

#if defined(CAPTURE_ACTIVE_IDLE)
        InterlockedIncrement(&FdoExtension->Dbg.ReportDIRQLInterruptActiveCount);
#endif                            

        interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
        interruptActiveStateParameters.ConnectionContext.InterruptObject = FdoExtension->SdbusInterruptObject;
        IoReportInterruptActive(&interruptActiveStateParameters);
    }

    RtlZeroMemory(&LockHandle, sizeof(LockHandle));
    SdbusAcquireInStackQueuedSpinLock(&FdoExtension->FxIrpQueueLock,
        &LockHandle);

    NT_ASSERT(FdoExtension->Active == FALSE);
    NT_ASSERT(FdoExtension->FxState == 0);
    NT_ASSERT(FdoExtension->DevicePowerState == PowerDeviceD0);

    SdbusAddFxDbgLog(FdoExtension,
        'AcCb',
        FdoExtension->Active,
        FdoExtension->FxState,
        FdoExtension->DevicePowerState);

    //
    // Process the IRP queue, and any deferred interrupts or enable card interrupt requests.
    //

    while ( (!IsListEmpty(&FdoExtension->FxIrpQueue)) ||
        (FdoExtension->DeferredInteruptDpcPendingCount != 0) ||
        (FdoExtension->DeferredEnableCardInterrupt != 0) ) {

            //
            // Process any deferred Enable Card Interrupt.
            //

            if (FdoExtension->DeferredEnableCardInterrupt != 0) {

                mask = 1;
                for (i = 0; i < MAX_SD_SOCKETS; i++) {
                    if (mask & FdoExtension->DeferredEnableCardInterrupt) {
                        SdbusEnableEvents(FdoExtension->Socket[i], SDBUS_EVENT_CARD_INTERRUPT);
#if defined(CAPTURE_ACTIVE_IDLE)
                        InterlockedIncrement(&FdoExtension->Dbg.FxDebugDeferredEnableCardInterruptCompleteCount);
#endif                
                        ActiveSocketCount++;
                    }            
                    mask <<= 1;
                }
                FdoExtension->DeferredEnableCardInterrupt = 0;
                SdbusReleaseInStackQueuedSpinLock(&LockHandle);

                while (ActiveSocketCount) {

                    //
                    // The Active count was originally incremented in 
                    // SdbusInterfaceAcknowledgeInt.
                    //
                    // We must now idle ourselves by the same count.
                    //

                    SdbusFxComponentIdle(FdoExtension, 0);
                    ActiveSocketCount--;
                }

                SdbusAcquireInStackQueuedSpinLock(&FdoExtension->FxIrpQueueLock,
                    &LockHandle);

            }

            //
            // Process any Deferred Interrupt DPCs.
            //

            while(FdoExtension->DeferredInteruptDpcPendingCount != 0) {

                SdbusReleaseInStackQueuedSpinLock(&LockHandle);

                WorkItemQueued = SdbusProcessInterruptDpc( NULL, FdoExtension->DeviceObject, NULL, NULL, TRUE, FALSE );

                //
                // If a workitem was queued, the workitem is responsible for
                // calling FX idle. Otherwise decrement our Active count here.
                //

                if (WorkItemQueued == FALSE) {

                    // 
                    // The Active count was originally incremented 
                    // in SdbusInterruptDpc.
                    //

                    SdbusFxComponentIdle(FdoExtension, 0);
                }

                SdbusAcquireInStackQueuedSpinLock(&FdoExtension->FxIrpQueueLock,
                    &LockHandle);

                FdoExtension->DeferredInteruptDpcPendingCount--;

#if defined(CAPTURE_ACTIVE_IDLE)
                InterlockedIncrement(&FdoExtension->Dbg.FxDebugDeferredDpcFinishedCount);
#endif     
            }

            //
            // Process the queued IRPs.
            //

            while (!IsListEmpty(&FdoExtension->FxIrpQueue)) {

                ListEntry = RemoveHeadList(&FdoExtension->FxIrpQueue);
                Irp = CONTAINING_RECORD(ListEntry, IRP, Tail.Overlay.ListEntry);
                IrpSp = IoGetCurrentIrpStackLocation(Irp);
                Pdo = IrpSp->DeviceObject;
                SdbusReleaseInStackQueuedSpinLock(&LockHandle);

                switch (IrpSp->MajorFunction) {

                case IRP_MJ_POWER:

                    NT_ASSERT(IrpSp->MinorFunction == IRP_MN_SET_POWER);

                    SdbusPdoSetPowerState(Pdo, Irp);
                    break;

                case IRP_MJ_INTERNAL_DEVICE_CONTROL:

                    NT_ASSERT(IrpSp->Parameters.DeviceIoControl.IoControlCode == IOCTL_SD_SUBMIT_REQUEST);

                    Sdrp = IrpSp->Parameters.DeviceIoControl.Type3InputBuffer;
                    PdoExtension = (PPDO_EXTENSION) Pdo->DeviceExtension;
                    if (Sdrp->RequestFunction == SDRF_DEVICE_COMMAND) {

                        Status = SdbusInterfaceDeviceCommand(PdoExtension,
                            Sdrp,
                            Irp);

                    } else {

                        NT_ASSERT(Sdrp->RequestFunction == SDRF_ERASE_COMMAND);

                        Status = SdbusInterfaceEraseCommand(PdoExtension,
                            Sdrp,
                            Irp);

                    }

                    //
                    // Since we're not in the completion path anymore, we have to
                    // complete the IRP if there was an error.
                    //

                    if (Status != STATUS_PENDING) {
                        DebugPrint((SDBUS_DEBUG_POWER,
                            "irp %p failed in active callback",
                            Irp));

                        SdbusAddDbgLog(PdoExtension->Socket,
                            'IFCo',
                            (ULONG_PTR) Irp,
                            Sdrp->RequestFunction,
                            Status);

                        SdbusFxComponentIdle(FdoExtension, 0);
                        Irp->IoStatus.Status = Status;
                        IoCompleteRequest(Irp, IO_NO_INCREMENT);
                    }

                    break;

                default:

                    NT_ASSERT(FALSE);

                    DebugPrint((SDBUS_DEBUG_POWER,
                        "SdbusFxComponentActiveCallback: fdo: %p - invalid irp %p processed",
                        FdoExtension->DeviceObject,
                        Irp));

                }

                SdbusAcquireInStackQueuedSpinLock(&FdoExtension->FxIrpQueueLock,
                    &LockHandle);

            }

    }

    KeSynchronizeExecution( FdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeActiveState, FdoExtension );

    SdbusReleaseInStackQueuedSpinLock(&LockHandle);
}

VOID
SdbusFxComponentIdleCallback(
    PVOID Context,
    ULONG Component
    )

/*++

Routine Description:

    Set component to the idle condition.

Arguments:

    Context - FDO's device extension

    Component - component index

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION fdoExtension = (PFDO_EXTENSION) Context;
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;

    DebugPrint((SDBUS_DEBUG_POWER,
        "SdbusFxComponentIdleCallback: fdo: %p",
        fdoExtension->DeviceObject));

    NT_ASSERT(fdoExtension->Active == TRUE);
    NT_ASSERT(fdoExtension->FxState == 0);
    NT_ASSERT(fdoExtension->DevicePowerState == PowerDeviceD0);

    SdbusAddFxDbgLog(fdoExtension,
        'IdCb',
        fdoExtension->Active,
        fdoExtension->FxState,
        fdoExtension->DevicePowerState);

#if defined(CAPTURE_ACTIVE_IDLE)
    InterlockedIncrement(&fdoExtension->Dbg.FxDebugIdleCallbackCount);
#endif

    //
    // Disconnect the DIRQL ISR, if we have a passive interrupt object to fall back on.
    //

    if (fdoExtension->SdbusPassiveInterruptActive != FALSE) {
#if defined(CAPTURE_ACTIVE_IDLE)
        InterlockedIncrement(&fdoExtension->Dbg.ReportDIRQLInterruptInactiveCount);
#endif                            

        interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
        interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusInterruptObject;
        IoReportInterruptInactive(&interruptActiveStateParameters);
    }

    //
    // Set the idle condition in the fdoExtension.
    //

    KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeIdleState, fdoExtension );
    fdoExtension->Dbg.FxDebugActiveState = FALSE;

    PoFxCompleteIdleCondition(fdoExtension->FxHandle, Component);
}

VOID
SdbusFxDevicePowerRequiredCallback(
    PVOID Context
    )

/*++

Routine Description:

    Notifies the driver that it may return to D0 state.

Arguments:

    Context - FDO's device extension

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION fdoExtension = (PFDO_EXTENSION) Context;
    POWER_STATE powerState = {0};
    DPR_STATE_CONTEXT DprContext = {0};

    NT_ASSERT(fdoExtension->Active == FALSE);

#if defined(CAPTURE_ACTIVE_IDLE)    
    fdoExtension->Dbg.FxDebugDevicePowerRequiredState = TRUE;
    InterlockedIncrement(&fdoExtension->Dbg.FxDebugDevicePowerRequiredCount);
#endif

    DprContext.FdoExtension = fdoExtension;
    DprContext.State = TRUE;
    
    KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);

    SdbusAddFxDbgLog(fdoExtension,
                     'DPR ',
                     fdoExtension->Active,
                     fdoExtension->FxState,
                     fdoExtension->DevicePowerState);

    NT_ASSERT(fdoExtension->FxDevicePowerRequiredInProgress == FALSE);

    fdoExtension->FxDevicePowerRequiredInProgress = TRUE;

    //
    // Only request a D0 if we previously requested a D3.
    //

    if (fdoExtension->FxD3Requested) {

        //
        // Callback is completed in the D-IRP completion routine.
        //

        powerState.SystemState = fdoExtension->SystemPowerState;
        powerState.DeviceState = PowerDeviceD0;

        PoRequestPowerIrp(fdoExtension->DeviceObject,
                          IRP_MN_SET_POWER,
                          powerState,
                          NULL,
                          NULL,
                          NULL);

        fdoExtension->FxD3Requested = FALSE;
        return;
    }

    NT_ASSERT(fdoExtension->FxDevicePowerRequiredInProgress != FALSE);

#if defined(CAPTURE_ACTIVE_IDLE)
    InterlockedIncrement(&fdoExtension->Dbg.FxDebugDeviceReportDevicePoweredOnCount);
#endif

    PoFxReportDevicePoweredOn(fdoExtension->FxHandle);
    fdoExtension->FxDevicePowerRequiredInProgress = FALSE;

    DprContext.FdoExtension = fdoExtension;
    DprContext.State = FALSE;
    KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);
}

VOID
SdbusFxDevicePowerNotRequiredStep2Passive(
    __in PDEVICE_OBJECT DeviceObject,
    __inout PVOID Context
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    SdbusFxDevicePowerNotRequiredStep2(Context);
    return;
}

VOID
SdbusFxDevicePowerNotRequiredStep2(
    __in PVOID Context
)
{
    PFDO_EXTENSION fdoExtension = (PFDO_EXTENSION) Context;
    DPR_STATE_CONTEXT DprContext = {0};

    //
    // Arm for Wait Wake if necessary. This must happen before requesting a D3.
    //

    if ((fdoExtension->CardDetectMethod == SDBUS_CARD_DETECT_METHOD_WAIT_WAKE) &&
        (fdoExtension->WaitWakeIrp == NULL)) {

            //
            // If the current IRQL is greater than PASSIVE_LEVEL, we need
            // to queue a work item to call this function back at PASSIVE_LEVEL.
            //

            if (KeGetCurrentIrql() > PASSIVE_LEVEL) {

                IoQueueWorkItem(fdoExtension->WaitWakeArmWorkItem,
                                &SdbusFxDevicePowerNotRequiredStep2Passive,
                                DelayedWorkQueue,
                                (PVOID)fdoExtension);
                return;
            }

            SdbusFdoArmForWake(fdoExtension);
    }

    //
    // We only want to go into D3 if all of our children are in D3 and they're
    // started.
    //

    if (fdoExtension->PoweredPdoCount == 0) {

        POWER_STATE powerState = {0};   

        powerState.SystemState = fdoExtension->SystemPowerState;
        powerState.DeviceState = PowerDeviceD3;

        PoRequestPowerIrp(fdoExtension->DeviceObject,
            IRP_MN_SET_POWER,
            powerState,
            NULL,
            NULL,
            NULL);

        fdoExtension->FxD3Requested = TRUE;

    }

    DprContext.FdoExtension = fdoExtension;
    DprContext.State = FALSE;
    KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);

    PoFxCompleteDevicePowerNotRequired(fdoExtension->FxHandle);
    return;
}

VOID
SdbusFxDevicePowerNotRequiredCallback(
    PVOID Context
)

/*++

Routine Description:

    Notifies the driver whether it may go into a Dx state while
    still under runtime power management.

Arguments:

    Context - FDO's device extension

Return Value:

    VOID

--*/

{
    PFDO_EXTENSION fdoExtension = (PFDO_EXTENSION) Context;
    DPR_STATE_CONTEXT DprContext = {0};

    //ULONG i = 0;

    NT_ASSERT(fdoExtension->Active == FALSE);
    NT_ASSERT(fdoExtension->DevicePowerState == PowerDeviceD0);

#if defined(CAPTURE_ACTIVE_IDLE)
    fdoExtension->Dbg.FxDebugDevicePowerRequiredState = FALSE;
    InterlockedIncrement(&fdoExtension->Dbg.FxDebugDevicePowerNotRequiredCount);
#endif

    SdbusAddFxDbgLog(fdoExtension,
                     'DPNR',
                     fdoExtension->Active,
                     fdoExtension->FxState,
                     fdoExtension->DevicePowerState);

    DprContext.FdoExtension = fdoExtension;
    DprContext.State = TRUE;
    KeSynchronizeExecution( fdoExtension->SdbusInterruptObject, &SdbusFxSynchronizeDPRState, &DprContext);

#if 0
    //
    // Check to see if all of our pdos are started, if there are any.
    //

    for (i = 0; i < fdoExtension->NumberOfSockets; i++) {
        PSD_SOCKET socket = fdoExtension->Socket[i];
        PDEVICE_OBJECT pdo = socket->PdoList;
        PPDO_EXTENSION pdoExtension = NULL;

        if (pdo == NULL) {
            continue;
        }

        //
        // If the controller has child PDOs, then check to see if they're
        // started. If any of the PDOs aren't started, don't go to D3 and let
        // them start.
        //

        for (; pdo != NULL ; pdo = pdoExtension->NextPdo) {

            pdoExtension = pdo->DeviceExtension;

            if (pdoExtension->PnpState != SDBUS_DEVICE_STARTED) {
                requestPowerIrp = FALSE;
            }
        }
    }
#endif

    SdbusFxDevicePowerNotRequiredStep2(Context);
}

NTSTATUS
SdbusFxPowerControlCallback(
    PVOID Context,
    LPCGUID PowerControlCode,
    PVOID InputBuffer,
    SIZE_T InputBufferSize,
    PVOID OutputBuffer,
    SIZE_T OutputBufferSize,
    PSIZE_T BytesReturned
    )
{

    PFDO_EXTENSION FdoExtension = (PFDO_EXTENSION) Context;
    PSD_SOCKET Socket;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(InputBufferSize);
    UNREFERENCED_PARAMETER(OutputBufferSize);
    UNREFERENCED_PARAMETER(BytesReturned);

    SdbusAddFxDbgLog(FdoExtension,
                     'PCCb',
                     (ULONG_PTR) FdoExtension->CompatType,
                     (ULONG_PTR) InputBuffer,
                     (ULONG_PTR) OutputBuffer);

    //
    // Check if we're finishing a P-state request.
    //

    if (IsEqualGUID(PowerControlCode, &GUID_SDBUS_PSTATE_REQUEST)) {

        NT_ASSERT(InputBuffer != NULL);

        NT_ASSERT(sizeof(ULONG) == InputBufferSize);

        return SdbusCompletePStateRequest(FdoExtension, *((PULONG)InputBuffer));
    }


    return Status;
}

BOOLEAN 
SdbusFxSynchronizeActiveState(
    __in  PVOID SynchronizeContext
    )
{
    PFDO_EXTENSION FdoExtension;
    FdoExtension = (PFDO_EXTENSION) SynchronizeContext;

    FdoExtension->Active = TRUE;

    return TRUE;
}

BOOLEAN 
SdbusFxSynchronizeIdleState(
    __in  PVOID SynchronizeContext
    )
{
    PFDO_EXTENSION FdoExtension;
    FdoExtension = (PFDO_EXTENSION) SynchronizeContext;

    FdoExtension->Active = FALSE;    

    return TRUE;
}

BOOLEAN
SdbusFxSynchronizeDPRState(
    __in  PVOID SynchronizeContext
    )
{
    PFDO_EXTENSION FdoExtension;
    PDPR_STATE_CONTEXT DPRState = SynchronizeContext;

    FdoExtension = (PFDO_EXTENSION) DPRState->FdoExtension;

    FdoExtension->DPRState = DPRState->State;

    return TRUE;
}


