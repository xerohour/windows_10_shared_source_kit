/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    power.c

Abstract:

    the power code



We have four possible cases for any sleep with regards to USB hardware:

           Powered?   Wake?
    U-1      Y         Y
    U-2      N         N
    U-3      Y         N
    U-4      N         Y

The behavior (with neohub) is deterministic for each of these cases, the hard
part is determining which one matches the hadrware from the device capabilities
structure.

Behavior Matrix:

        VBus        D0-Action       Dx-Action        ROOTHUB        HC
----------------------------------------------------------------------------------
U-1     On          Hw_Resume       Hw_Suspend          D2          D[SystemState]
        On-Int      Hw_Restart
U-2     Off         Hw_Reset        Hw_Off*             D3          D3
U-3     On          Hw_Reset        Hw_Off*             D3          D3
U-4     On-Int      Hw_Reset        Hw_Off*             D3          D3


Questions:
1. What effect does putting the hardware in suspend(*) before turning it off have,
    this is typical of shutdown -- we have seen issues where suspending the port before
    shutdown causes problems on reboot.

2. If EHCI and CC have different power mappings what dependencies can this create?
    What are the side effects? We have seen cases where this results in the system
    immediatly 'waking' on suspend.


-USB host controller should only be programmed to recognize wake events if the
bus is armed for wake (ww irp posted to PCI).  Currently this is done as part of
mp_suspend but it should be split out.



Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    03-17-04 : neohub fast resume

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "power.tmh"
#endif

#if defined(XP_COMPAT)

#undef PoSetSystemWake
#define PoSetSystemWake
#undef PoGetSystemWake
#define PoGetSystemWake

#endif

typedef struct _USBHC_WAKE_CONTEXT {
    ULONG Sig;
    PDEVICE_EXTENSION DevExt;
    PUSB_IOREQUEST_CONTEXT UsbIoRequest;
    PIRP Irp;
    BOOLEAN WakeFromS0;
} USBHC_WAKE_CONTEXT, *PUSBHC_WAKE_CONTEXT;

VOID
USBPORT_ComputeRootHubDeviceCaps(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    )
/*++

Routine Description:

    NEO + OLD HUB

    Attempt to create the root hub

    Power Summary:

    <Gloassary>

    Lightest - PowerDeviceD0, PowerSystemWorking
    Deepest - PowerDeviceD3, PowerSystemHibernate

    SystemWake - this is defined to be the 'deepest' System state in which
                 the hardware can wake the system.
    DeviceWake -

    DeviceState[] - map of system states and the corresponding D states
                    these are the states the HW is in for any given
                    System Sleep state.

    HostControllerPowerAttributes - we define our own structure to describe
                    the attributes of a host comtroller -- this allows us to
                    map all possible controller scenarios on to the messed
                    up WDM power rules.

Arguments:

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_CAPABILITIES hcDeviceCaps, rhDeviceCaps;
    PDEVICE_EXTENSION rhDevExt, devExt;
    SYSTEM_POWER_STATE s;
    SYSTEM_POWER_STATE oldRhSystemWake;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    hcDeviceCaps = &devExt->DeviceCapabilities;
    rhDeviceCaps = &rhDevExt->DeviceCapabilities;

    // clone capabilities from the HC
    RtlCopyMemory(rhDeviceCaps,
                  hcDeviceCaps,
                  sizeof(DEVICE_CAPABILITIES));

    // construct the root hub device capabilities

    // root hub is not removable
    rhDeviceCaps->Removable=FALSE;
    rhDeviceCaps->UniqueID=FALSE;
    rhDeviceCaps->Address = 0;
    // Lonny seems to think this should remain the default value (-1)
    // somtheing to do witht he test that shous up for the device
    //rhDeviceCaps->UINumber = 0;

    // for the root hub D2 translates to 'USB suspend'
    // so we always indicate we can wake from D2
    rhDeviceCaps->DeviceWake = PowerDeviceD2;
    rhDeviceCaps->WakeFromD0 = TRUE;
    rhDeviceCaps->WakeFromD1 = FALSE;
    rhDeviceCaps->WakeFromD2 = TRUE;
    rhDeviceCaps->WakeFromD3 = FALSE;

    rhDeviceCaps->DeviceD2 = TRUE;
    rhDeviceCaps->DeviceD1 = FALSE;

    // generate the root hub power capabilities from the
    // HC Power Attributes plus a little magic
    USBPORT_ASSERT(rhDeviceCaps->SystemWake >= PowerSystemUnspecified &&
                   rhDeviceCaps->SystemWake <= PowerSystemMaximum);

    rhDeviceCaps->SystemWake =
        (PowerSystemUnspecified == rhDeviceCaps->SystemWake) ?
        PowerSystemWorking :
        rhDeviceCaps->SystemWake;

    oldRhSystemWake = rhDeviceCaps->SystemWake;

    for (s=PowerSystemSleeping1; s<=PowerSystemHibernate; s++) {

        PHC_POWER_STATE hcPowerState;

        hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                               &devExt->Fdo.HcPowerStateTbl,
                                               s);

        if (hcPowerState != NULL) {
            switch (hcPowerState->Attributes) {
            case HcPower_Y_Wakeup_Y:
                rhDeviceCaps->DeviceState[s] = PowerDeviceD2;
                // root hub wake maps to deepest S state excluding shutdown.
                if (oldRhSystemWake == PowerSystemShutdown) {
                    rhDeviceCaps->SystemWake = s;
                }
                break;
            case HcPower_N_Wakeup_N:
            case HcPower_Y_Wakeup_N:
            case HcPower_N_Wakeup_Y:
                rhDeviceCaps->DeviceState[s] = PowerDeviceD3;
                break;
            }
        }
    }

    // recompute root hub caps if FastS4MapS3toS4 is indicated.  This will cause the root hub to report it is
    // wake capable from S4.

    if (USBPORT_MapS3toS4(devExt)  &&
        rhDeviceCaps->DeviceState[PowerSystemSleeping3] == PowerDeviceD2 &&
        rhDeviceCaps->DeviceState[PowerSystemHibernate] != PowerDeviceD2) {
        // can wake from S3
        rhDeviceCaps->DeviceState[PowerSystemHibernate] = PowerDeviceD2;
        rhDeviceCaps->SystemWake = PowerSystemHibernate;
        devExt->Fdo.PowerFlags.RhS3mappedToS4 = 1;
    } else {
        devExt->Fdo.PowerFlags.RhS3mappedToS4 = 0;
    }


}

#ifdef XP_COMPAT

BOOLEAN
USBPORT_IsHybridSleepState(
    PDEVICE_OBJECT HcFdo,
    PIRP SystemPowerIrp,
    PSYSTEM_POWER_STATE RealSystemState
    )
{

    return FALSE;
}

#else

BOOLEAN
USBPORT_IsHybridSleepState(
    PDEVICE_OBJECT HcFdo,
    PIRP SystemPowerIrp,
    PSYSTEM_POWER_STATE RealSystemState
    )
/*++

Routine Description:

    Detect if we are entering a hybrid sleep sate (FastS4).  If this is fast s4 we treat it like S3.

Return Value:

    TRUE if This is an Fast S4 transition (AKA hybrid sleep state).  FALSE otherwise.

--*/
{
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION devExt;
    SYSTEM_POWER_STATE requestedSystemState;
    SYSTEM_POWER_STATE_CONTEXT sysPowerCtx;
    BOOLEAN isHybridSleep = FALSE;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    irpStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == SystemPowerState);

    sysPowerCtx = irpStack->Parameters.Power.SystemPowerStateContext;

    requestedSystemState = irpStack->Parameters.Power.State.SystemState;

    if ((ULONG)requestedSystemState != sysPowerCtx.TargetSystemState) {

        // this is a fast s4 transition, return the true sleep state
        *RealSystemState = sysPowerCtx.TargetSystemState;

        DbgTrace((PMDebugTrace, "**USBPORT detected Fast S4 FDO(%p)\n", HcFdo));
        if (devExt->Fdo.PMDebug) {
            NT_ASSERTMSG("Mismatched State on HybridSleep",FALSE);
        }

        isHybridSleep = TRUE;

    }

    return isHybridSleep;
}

#endif

PHC_POWER_STATE
USBPORT_GetHcPowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PHC_POWER_STATE_TABLE HcPowerStateTbl,
    SYSTEM_POWER_STATE SystemState
    )
/*++

Routine Description:

    For a given system power state return a pointer to
    the hc power state stored in the device extension.

Arguments:

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION devExt;
    PHC_POWER_STATE powerState;
    ULONG i;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    powerState = NULL;

    //if (USBPORT_MapS3toS4(devExt) &&
    //    SystemState == PowerSystemHibernate) {
    //   // if fastS4 is enabled and this system can wake from S3 then report HC caps based on S3
    //    // for hibernate
    //    SystemState = PowerSystemSleeping3;
    //}

    for (i=0; i<USBPORT_MAPPED_SLEEP_STATES; i++) {

        if (HcPowerStateTbl->PowerState[i].SystemState ==
            SystemState) {

            powerState = &HcPowerStateTbl->PowerState[i];
            break;
        }
    }

    // if we cant match it then it's a bug
    USBPORT_ASSERT(powerState != NULL);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'ghcP', 0, powerState, 0);

    return powerState;
}


VOID
USBPORT_ComputeHcPowerStates(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_CAPABILITIES HcDeviceCaps,
    PHC_POWER_STATE_TABLE HcPowerStateTbl
    )
/*++

Routine Description:

    Using the HC capabilities reported by the parent bus compute
    the host controllers power attributes.

    Power attributes are defined as follows:

                                   {     attributes      }
     | SystemState |  DeviceState  |  Powered?  |  Wake? |
     +-------------+---------------+------------+--------+
           S1-S5          D0-D3          Y/N        Y/N

    The table includes entries for every possible system sleep state
    the OS may ask us to enter.

    (S1) PowerSystemSleeping1
    (S2) PowerSystemSleeping2
    (S3) PowerSystemSleeping3
    (S4) PowerSystemHibernate
    (S5) PowerSystemShutdown

    We have four possible cases for each sleep state:

             Powered?   Wake?
    case 1      Y         Y
    case 2      N         N
    case 3      Y         N
    case 4      N         Y

    The table indicates the state of the BUS (BUS underlined), not the HC
    brains which may or may not lose power.

    Case 1 and 2 are typical of most systems in S1.
    Some systems fall into case 3, for these we try to suspend the controller
    vs turning it off since resume is faster this way.

    Case 4 does not happen in reality -- the bus has to have power to support
    resume sigalling.



Arguments:

Return Value:

    none

--*/
{
    SYSTEM_POWER_STATE s;
    ULONG i;
    SYSTEM_POWER_STATE systemWake;

    ASSERT_PASSIVE();

    systemWake = HcDeviceCaps->SystemWake;

    // The HC can wake the system for any sleep state lighter (<=)
    // systemWake

    // iniialize the table
    s = PowerSystemSleeping1;

    for (i=0; i<USBPORT_MAPPED_SLEEP_STATES; i++) {

        HcPowerStateTbl->PowerState[i].SystemState = s;
        HcPowerStateTbl->PowerState[i].DeviceState =
            HcDeviceCaps->DeviceState[s];

        // it follows that if the map indicates that the DeviceState
        // is D3 but the system state is still <= SystemWake then the
        // hc is still powered

        if (s <= systemWake) {
            if (HcDeviceCaps->DeviceState[s] == PowerDeviceUnspecified) {
                // for unspecified we go with case 2, ie no power
                // case 2
                HcPowerStateTbl->PowerState[i].Attributes =
                    HcPower_N_Wakeup_N;
            } else {
                // case 1
                HcPowerStateTbl->PowerState[i].Attributes =
                    HcPower_Y_Wakeup_Y;
            }
        } else if (s == PowerSystemShutdown) {
            // do not support wake from the shutdown case
            //
            // by setting this we ensure that we don't leave the hardware enabled during the shutdown process
            // case 2
            HcPowerStateTbl->PowerState[i].Attributes =
                    HcPower_N_Wakeup_N;
        } else {
            if (HcDeviceCaps->DeviceState[s] == PowerDeviceD3 ||
                HcDeviceCaps->DeviceState[s] == PowerDeviceUnspecified) {
                // case 2
                HcPowerStateTbl->PowerState[i].Attributes =
                    HcPower_N_Wakeup_N;
            } else {
                //
                // case 3
                HcPowerStateTbl->PowerState[i].Attributes =
                    HcPower_Y_Wakeup_N;
            }
        }

        USBPORT_ASSERT(HcPowerStateTbl->PowerState[i].Attributes !=
                           HcPower_N_Wakeup_Y);

        s++;
    }

    USBPORT_ASSERT(s == PowerSystemMaximum);
}


VOID
USBPORT_PoRequestCompletionD0(
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:

    Called when the DEVICE_POWER_STATE Irp which was requested by
    USBPORT_SystemPowerS0IrpCompletion() completes.

    If Fast Resume is enabled then the SYSTEM_POWER_STATE Irp is
    completed by USBPORT_SystemPowerS0IrpCompletion().

    If Fast Resume is not enabled then the SYSTEM_POWER_STATE Irp is
    normally completed by the work item call to USBPORT_PowerUpPhase1()
    which is queued here, but the SYSTEM_POWER_STATE Irp will be
    completed here in error paths where the work item call to
    USBPORT_PowerUpPhase1() is not queued.

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DevicePowerState - The Dx that we are in/tagetted.

    Context - Driver defined context.

    IoStatus - The status of the IRP.

Return Value:

    None.

--*/
{
    PIRP systemPowerIrp;
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus = IoStatus->Status;
    KIRQL irql;
    HC_FDO_POWERSTATE fdoPwrState;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    // a call to this function basically tells us
    // that we are now in the requested D-state
    // we now finish the whole process by calling
    // down the original SysPower request to our
    // PDO

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    systemPowerIrp = (PIRP) Context;

    DbgTrace((UsbpDebugTrace,
            "PoRequestComplete fdo(%p) MN_SET_POWER DEV(%d)\n",
            FdoDeviceObject, PowerState.DeviceState));

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'PwC0', ntStatus,
             devExt->Dx_State, PowerState.DeviceState);

    // note that if the SetD0 has failed we do not attempt
    // to re-start the controller

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

    if (NT_SUCCESS(ntStatus)) {

        fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

        USBPORT_ASSERT(fdoPwrState == FdoPwr_WaitD0_PoComplete);

        if (fdoPwrState == FdoPwr_WaitD0_PoComplete) {

            NTSTATUS qw_nts;

            USBPORT_ASSERT(PowerState.DeviceState == PowerDeviceD0);
            USBPORT_ASSERT(devExt->Dx_State != PowerDeviceD0);

            // not in D0 yet
            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       systemPowerIrp,
                                       FdoPwr_D0_WaitWorker1,
                                       FdoPwrEv_D0_PoComplete_Success);


            // defer start to our worker thread or workitem
            USBPORT_RELEASE_POWER_LOCK(devExt, irql);

            USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
            devExt->Fdo.DmTimerFlags.DecodesOff = 0;
            USBPORT_RELEASE_DM_LOCK(devExt, irql);

            //
            // Now that the controller is powered back up, the
            // deadman timer can be restarted
            //
            USBPORT_ResumeDM_Timer(FdoDeviceObject);

            //
            // If an interrupt is connected then report it as active
            //
            if (devExt->Fdo.Flags.IrqConnected) {
                IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS	parameters;

                RtlZeroMemory(&parameters,
                              sizeof(IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS));

                parameters.Version = CONNECT_FULLY_SPECIFIED;
                parameters.ConnectionContext.InterruptObject = devExt->Fdo.InterruptObject;
                IoReportInterruptActive(&parameters);
            }

            MPf_FlushInterrupts(devExt);


            if (USBPORT_IS_USB20(devExt)) {
                MPf_TakePortControl(devExt);
            }

            // on completion of this function (and completion of S irp) the
            // controller is in D0 as far as the OS is concerned.
            // We may not have completely powered up yet though, we handle
            // this when the hub (rh PDO) tries to go to D0.

            // mark ourselves as D0 before queue or signalling the threads
            // as they may check this flag
            devExt->Dx_State = PowerDeviceD0;

            // complete wake irp if HC was source of resume
            if (devExt->Fdo.PowerFlags.HcIsWakeupSource) {
                USBPORT_CompleteHubPdoWaitWake(FdoDeviceObject, STATUS_SUCCESS);
            }

            //queue USBPORT_UsbPowerWorker()
            // queue a high priority workitem to finish the power up.
            // note we that don't sit on the power irp here -- the rest of USB
            // power up is async.


            // NOTE: we hold the S-irp for worker, this allows us to use the
            // po managers serialization to power the ports an re-init of the
            // controller.  This means the HCs will show up in bootvis but only
            // for cases where the controller must be re-initailized.
            //
            // we split init into two phases -- each has a separate workitem
            //

            qw_nts = USBPORT_QueuePowerWorker(FdoDeviceObject,
                                              systemPowerIrp,
                                              PowerWorkPhase1);

            if (NT_ERROR(qw_nts)) {

                // if we cannot queue the worker we cannot finish power up
                // we mark the controller as failed and try to recover later

                USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

                USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       systemPowerIrp,
                                       FdoPwr_HardwareFailure,
                                       FdoPwrEv_D0_HardwareFailure);
                TEST_TRAP();

                USBPORT_RELEASE_POWER_LOCK(devExt, irql);

                // Fall thru and complete S irp with success
                KeSetEvent(&devExt->Fdo.RootHubReadyEv, IO_NO_INCREMENT, FALSE);
                KeSetEvent(&devExt->Fdo.ChirpCompleteEv, IO_NO_INCREMENT, FALSE);

            } else {

                // we still hold the S irp
                return; // STATUS_MORE_PROCESSING_REQUIRED;
            }

        } else {
            // should not reach this code path
            USBPORT_RELEASE_POWER_LOCK(devExt, irql);
        }
    } else {
        // ERROR PATH

        // an error occurred
        // According to adriano 'S IRP should be immediately completed with
        // the same status as the D IRP in the failure case'
        // Since the method of handling this is not documented anywhere we will
        // go with what Adrian says.
        //
        // The fact that this request has failed will probably cause other
        // complaints.

        fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

        USBPORT_ASSERT(fdoPwrState == FdoPwr_WaitD0_PoComplete);

        if (fdoPwrState == FdoPwr_WaitD0_PoComplete) {

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       systemPowerIrp,
                                       FdoPwr_PowerError,
                                       FdoPwrEv_D0_PoComplete_Error);

            USBPORT_ASSERT(PowerState.DeviceState == PowerDeviceD0);
            USBPORT_ASSERT(devExt->Dx_State != PowerDeviceD0);

            // NOTE: this is the path hit on the subsysId change,
            // we need to propigate the failure status to the hub driver if possible when it
            // attempts to power up.

            TEST_TRAP();
            devExt->Fdo.PowerError_Nts = ntStatus;

            USBPORT_RELEASE_POWER_LOCK(devExt, irql);

            // since we won't be going to D0 we need to signal that no USB synch is necessary
            //
            // This error will result in many devices being 'removed and reattached during resume but
            // we can't do anything about that.

            KeSetEvent(&devExt->Fdo.RootHubReadyEv, IO_NO_INCREMENT, FALSE);
            KeSetEvent(&devExt->Fdo.ChirpCompleteEv, IO_NO_INCREMENT, FALSE);

            // complete any pending wake irp with the same status
            USBPORT_CompleteHubPdoWaitWake(FdoDeviceObject, ntStatus);

            // If set PowerDeviceD0 fails there is nothing that can be done
            // to recover.  Treat this as though it were a surprise removal
            // of the host controller to kick off the cleanup process.
            //
            USBPORT_InvalidateController(FdoDeviceObject,
                                         UsbMpControllerRemoved);
        } else {
            // should not reach this code path
            USBPORT_RELEASE_POWER_LOCK(devExt, irql);
        }

        if (!devExt->Fdo.FastResumeEnable) {

            // If Fast Resume is not enabled then the SYSTEM_POWER_STATE
            // Irp is normally completed by the work item call to
            // USBPORT_PowerUpPhase1(), but the SYSTEM_POWER_STATE Irp
            // will be completed here in error paths where the work item
            // call to USBPORT_PowerUpPhase1() is not queued.
            //
            systemPowerIrp->IoStatus.Status = ntStatus;
        }
    }

    // for D1,D2 or D3 and failure status we just release the power manager
    // there is.
    // for D0 this is equivalent to async resume -- we release the S0 irp here
    // and allow other things to begin power up.

    // on error the CurrentDevicePowerState will remain unchanged

    if (!devExt->Fdo.FastResumeEnable) {

        // If Fast Resume is not enabled then the SYSTEM_POWER_STATE Irp
        // is normally completed by the work item call to
        // USBPORT_PowerUpPhase1(), but the SYSTEM_POWER_STATE Irp will
        // be completed here in error paths where the work item call to
        // USBPORT_PowerUpPhase1() is not queued.
        //
        DbgTrace((UsbpDebugTrace,
                  "Complete Sys Irp(%p) %x\n", systemPowerIrp, ntStatus));

        PoStartNextPowerIrp(systemPowerIrp);

        usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, systemPowerIrp);

        IoCompleteRequest(systemPowerIrp,
                          IO_NO_INCREMENT);

        USBPORT_ASSERT(usbIoRequest);
        USBPORT_Core_FreeIoRequest(usbIoRequest);
    }
}


VOID
USBPORT_PoRequestCompletionDx(
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:

    Called when the Device Power State Irp we requested is completed.
    this is where we complete the systemPowerIrp

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DevicePowerState - The Dx that we are in/tagetted.

    Context - Driver defined context.

    IoStatus - The status of the IRP.

Return Value:

    None.

--*/
{
    PIRP systemPowerIrp;
    PDEVICE_EXTENSION devExt;
    NTSTATUS ntStatus = IoStatus->Status;
    KIRQL irql;
    HC_FDO_POWERSTATE fdoPwrState;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    // a call to this function basically tells us
    // that we are now in the requested D-state
    // we now finish the whole process by calling
    // down the original SysPower request to our
    // PDO

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    systemPowerIrp = (PIRP) Context;

    DbgTrace((UsbpDebugTrace,
            "PoRequestComplete fdo(%p) MN_SET_POWER DEV(%d)\n",
            FdoDeviceObject, PowerState.DeviceState));

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'PwCx', ntStatus,
             devExt->Dx_State, PowerState.DeviceState);

    // note that if the SetD0 has failed we do not attempt
    // to re-start the controller

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

    if (NT_SUCCESS(ntStatus)) {

        fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

        USBPORT_ASSERT(fdoPwrState == FdoPwr_WaitDx_PoComplete);

        if (fdoPwrState == FdoPwr_WaitDx_PoComplete) {

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       systemPowerIrp,
                                       FdoPwr_WaitS0,
                                       FdoPwrEv_Dx_PoComplete_Success);

            devExt->Dx_State = PowerState.DeviceState;

            USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        } else {
            // should not reach this code path
            USBPORT_RELEASE_POWER_LOCK(devExt, irql);
        }
    } else {
        // ERROR PATH

        // an error occurred
        // According to adriano 'S IRP should be immediately completed with
        // the same status as the D IRP in the failure case'
        // Since the method of handling this is not documented anywhere we will
        // go with what Adrian says.
        //
        // The fact that this request has failed will probably cause other
        // complaints.

        fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

        USBPORT_ASSERT(fdoPwrState == FdoPwr_WaitDx_PoComplete);

        if (fdoPwrState == FdoPwr_WaitDx_PoComplete) {

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       systemPowerIrp,
                                       fdoPwrState,
                                       FdoPwrEv_Dx_PoComplete_Error);

            USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        } else {
            // should not reach this code path
            USBPORT_RELEASE_POWER_LOCK(devExt, irql);
        }

        systemPowerIrp->IoStatus.Status = ntStatus;
    }

    // for D1,D2 or D3 and failure status we just release the power manager
    // there is.
    // for D0 this is equivalent to async resume -- we release the S0 irp here
    // and allow other things to begin power up.

    // on error the CurrentDevicePowerState will remain unchanged


    DbgTrace((UsbpDebugTrace,
                "Complete Sys Irp(%p) %x\n", systemPowerIrp, ntStatus));
    PoStartNextPowerIrp(systemPowerIrp);

    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, systemPowerIrp);

    IoCompleteRequest(systemPowerIrp,
                      IO_NO_INCREMENT);

    USBPORT_ASSERT(usbIoRequest);
    USBPORT_Core_FreeIoRequest(usbIoRequest);
}


NTSTATUS
USBPORT_SystemPowerSxIrpCompletion(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP SystemPowerIrp,
    PVOID Context
    )
/*++

Routine Description:

    Handle the completion of the pended SYSTEM_POWER_STATE
    SystemPowerIrp.

    This is where we request the DEVICE_POWER_STATE Irp.

Arguments:

Return Value:


--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    POWER_STATE powerState;
    SYSTEM_POWER_STATE requestedSystemState, realSystemState;
    PHC_POWER_STATE hcPowerState;
    PDEVICE_EXTENSION rhDevExt;
    HC_FDO_POWERSTATE fdoState;
    KIRQL irql;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    BOOLEAN hybridSleep = FALSE;
    BOOLEAN hcGotoD3;

    irpStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhDevExt = USBPORT_GetRootHubPdoExt(FdoDeviceObject);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == SystemPowerState);

    realSystemState =
        requestedSystemState = irpStack->Parameters.Power.State.SystemState;

    DbgTrace((UsbpDebugTrace, "RESET RootHub Ready\n"));

    KeResetEvent(&devExt->Fdo.RootHubReadyEv);
    KeResetEvent(&devExt->Fdo.ChirpCompleteEv);
    DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_SET_POWER SYS(%d)\n",
            FdoDeviceObject, requestedSystemState));

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'RspC', 0,
        FdoDeviceObject, requestedSystemState);

    //USBPORT_ASSERT(TEST_FLAG(devExt->PnpStateFlags, USBPORT_PNP_STARTED));
    USBPORT_ASSERT(USBPORT_GetPnpState(devExt) == PnpStarted);

    // compute the appropriate D-state for this S request

    // remember the last 'sleep' system state we entered for debugging
    devExt->Fdo.LastSystemSleepState = requestedSystemState;
    USBPORT_ASSERT(requestedSystemState != PowerSystemWorking);

#ifdef XP_COMPAT
    //
    // get the current power state of the root hub
    // The root hub will be in D1 or D2 if the controller supports any kind
    // of USB wake -- ie powered after suspend or off.
    //
    hcGotoD3 = TRUE;
    if (rhDevExt != NULL) {
        if (rhDevExt->Dx_State == PowerDeviceD2 ||
            rhDevExt->Dx_State == PowerDeviceD1) {

            // use the mapped suspend power state if the root hub is in D2 and this is not
            // hybrid sleep.

            hcGotoD3 = FALSE;
        }
    }

#else

    // by default go to d3
    hcGotoD3 = TRUE;
    devExt->Fdo.PowerFlags.ForceControllerOff = 0;

    if (USBPORT_IsHybridSleepState(FdoDeviceObject, SystemPowerIrp, &realSystemState)) {

        // hybrid sleep, re-map sleep state
        USBPORT_ASSERT(realSystemState != requestedSystemState);

        hybridSleep = TRUE;

        // hybrid sleep, use true settings for hub based on HC transition to S3
        if (rhDevExt != NULL) {
            if (rhDevExt->Dx_State == PowerDeviceD2 ||
                rhDevExt->Dx_State == PowerDeviceD1) {

                // use the mapped suspend power state if the root hub is in D2 and this is not
                // hybrid sleep.

                hcGotoD3 = FALSE;
            }
        }

    } else {

        hybridSleep = FALSE;

        // not hybrid sleep, if the hubs s4 state was remapped and the system state is hibernate we nust correct the
        // bus wake setting here.

        USBPORT_ASSERT(realSystemState == requestedSystemState);

        if (devExt->Fdo.PowerFlags.RhS3mappedToS4 &&
            requestedSystemState == PowerSystemHibernate) {

            // If we don't go to the OFF state then some systemes with companion controllers will wake if a
            // USB device is attached besides suspending the controller in a state that it cannot wake from
            // is probabaly a bad idea.
            devExt->Fdo.PowerFlags.ForceControllerOff = 1;

            // Fast S4 is enabled but we are going into 'real S4' and the Bus really
            // cannot support wake from S4 so we must correct the wake state of the bus.
            if (devExt->Fdo.PowerFlags.BusArmedForWake) {
                //ASSERT_PASSIVE();

                // perform the override
                USBPORT_DisarmHcForWake(FdoDeviceObject);

                DbgTrace((PMDebugTrace, "**USBPORT detected Real S4 FDO(%p) - disable wake\n", FdoDeviceObject));
                LOGENTRY(FdoDeviceObject, LOG_POWER, 'PMD1', 0, 0, 0);

                if (devExt->Fdo.PMDebug) {
                    NT_ASSERTMSG("Unexpected State on Hibernate",FALSE);
                }
            }

        } else {

            // otherwise use values based on hubs power state.
            if (rhDevExt != NULL) {
                if (rhDevExt->Dx_State == PowerDeviceD2 ||
                    rhDevExt->Dx_State == PowerDeviceD1) {

                    hcGotoD3 = FALSE;
                }
            }
        }
    }

#endif

    //
    // Check to see if the Hc should be forced to D3.
    // If enabled in global registry policy, force to D3
    // if the controller is *not* armed for wake.
    // This reduces power consumption in sleep on some systems.
    //
    if (devExt->Fdo.PowerFlags.ForceHcD3NoWakeArm &&
        !devExt->Fdo.PowerFlags.BusArmedForWake ) {

        hcGotoD3 = TRUE;
    }

    requestedSystemState =
        devExt->Fdo.LastSystemSleepEntered = realSystemState;
    devExt->Fdo.LastSystemSleepBusArmedForWake = devExt->Fdo.PowerFlags.BusArmedForWake;
    devExt->Fdo.NumSleepsEntered++;

    if (hybridSleep) {
        DbgTrace((UsbpDebugTrace, " >Hybrid sleep detected\n"));
    }

    switch (requestedSystemState) {

    case PowerSystemShutdown:

        DbgTrace((UsbpDebugTrace, " >Shutdown HC Detected\n"));
        break;

    case PowerSystemHibernate:

        DbgTrace((UsbpDebugTrace, " >Hibernate HC Detected\n"));
        break;

    case PowerSystemSleeping1:
    case PowerSystemSleeping2:
    case PowerSystemSleeping3:

        DbgTrace((UsbpDebugTrace, " >Sleeping Detected\n"));
        break;

    default:
        // This is the case where the requested system state is unkown
        // to us.
        // It not clear what to do here. Vince sez try to ignore it so we will
        // try and just remain in the current device state.
        powerState.DeviceState = devExt->Dx_State;
        TEST_TRAP();
    }


    // compute the correct D state, since we support wake from hiberanate
    // and shutdown these may map to D2 as well

    // get our power info summary
    hcPowerState = USBPORT_GetHcPowerState(FdoDeviceObject,
                                           &devExt->Fdo.HcPowerStateTbl,
                                           requestedSystemState);
    // keep lint tools happy.
    USBPORT_ASSERT(hcPowerState);
    if (hcPowerState == NULL) {
        ntStatus = SystemPowerIrp->IoStatus.Status = STATUS_UNSUCCESSFUL;
        goto SystemPowerSxIrpCompletion_end;
    }

    if (hcGotoD3 == FALSE) {

        USBPORT_ASSERT(hcPowerState->Attributes == HcPower_Y_Wakeup_Y ||
                       hcPowerState->Attributes == HcPower_Y_Wakeup_N);

        // the controller should have been suspended by the hub --
        // ie the hub PDO should have been placed in D2
        USBPORT_ASSERT(devExt->Fdo.PowerFlags.Suspended);

        // The host controller hardware is already in the 'USB suspended'
        // state which means it will wake if an interrupt occurs or is
        // resume signaling is detected.  To my knowledge there is no way
        // to unwind this process should we take an interrupt.

        // select the D state for the HC

        // The hub is responsible for arming or disarming the bus as
        // appropriate so we take no additional action here.
        powerState.DeviceState = hcPowerState->DeviceState;

    } else {

        // NOT capable of USB WAKE

        // root hub is in D3 or D0

        // if the controller remains powered then it is optimal to
        // 'suspend'. The (new) hub driver will take this fact into account
        // and keep the root hub PDO in D1/D2.
        //
        // In the D3 case we cannot do any kind of wakeup so we'll be turnig
        // the controller off later.
        //
        // The root hub will be in D0 if the hub driver failed to start or is
        // otherwise unloaded.

        // Note that this will also make vincentg happy since we won't be
        // messing with the hardware in the context of the system power
        // message.

        // request a D3
        powerState.DeviceState = PowerDeviceD3;

    }


    //
    // are we already in this state?
    //
    // Note: if we get a D3 request before we are started
    // we don't need to pass the irp down to turn us off
    // we consider the controller initially off until we
    // get start.
    //

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);
    fdoState = devExt->Fdo.CurrentFdoPowerState;

    if (fdoState == FdoPwr_WaitSx_IoComplete) {

        USBPORT_ASSERT(devExt->Dx_State != powerState.DeviceState);

        // No,
        // now allocate another irp and use PoCallDriver
        // to send it to ourselves

        DbgTrace((UsbpDebugTrace, " >Requesting HC D-State - %d\n",  powerState.DeviceState));
        USBPORT_ASSERT(powerState.DeviceState != PowerDeviceD0);

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'RqDx', FdoDeviceObject,
                 devExt->Dx_State, powerState.DeviceState);


        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitDx_PoComplete,
                                   FdoPwrEv_Sx_IoComplete);


        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        ntStatus =
            PoRequestPowerIrp(FdoDeviceObject,
                              IRP_MN_SET_POWER,
                              powerState,
                              USBPORT_PoRequestCompletionDx,
                              SystemPowerIrp,
                              NULL);

        if (ntStatus == STATUS_PENDING) {
            return STATUS_MORE_PROCESSING_REQUIRED;
        }

    }else {

        USBPORT_ASSERT(devExt->Dx_State == powerState.DeviceState);

        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   fdoState,
                                   FdoPwrEv_Sx_IoComplete);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        ntStatus = STATUS_SUCCESS;
    }

    // We are already in the requested D state or we have an error
    // just complete the system irp

SystemPowerSxIrpCompletion_end:

    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, SystemPowerIrp);
    // complete the system power irp
    PoStartNextPowerIrp(SystemPowerIrp);

    USBPORT_Core_FreeIoRequest(usbIoRequest);

    return ntStatus;
}


NTSTATUS
USBPORT_SystemPowerS0IrpCompletion(
    PDEVICE_OBJECT  FdoDeviceObject,
    PIRP            SystemPowerIrp,
    PVOID           Context
    )
/*++

Routine Description:

    Handle the completion of the pended SYSTEM_POWER_STATE
    SystemPowerIrp.

    This is where we request the DEVICE_POWER_STATE Irp.

Arguments:

Return Value:

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    POWER_STATE powerState;
    SYSTEM_POWER_STATE requestedSystemState;
    HC_FDO_POWERSTATE fdoState;
    KIRQL irql;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    irpStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == SystemPowerState);

    requestedSystemState = irpStack->Parameters.Power.State.SystemState;

    DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_SET_POWER SYS(%d)\n",
            FdoDeviceObject, requestedSystemState));

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'RspC', 0,
             FdoDeviceObject, requestedSystemState);

    USBPORT_ASSERT(USBPORT_GetPnpState(devExt) == PnpStarted);
    USBPORT_ASSERT(requestedSystemState == PowerSystemWorking);

    powerState.DeviceState = PowerDeviceD0;

    // Are we already in this DEVICE_POWER_STATE?
    //
    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);
    fdoState = devExt->Fdo.CurrentFdoPowerState;

    if (fdoState == FdoPwr_WaitS0_IoComplete &&
        devExt->Dx_State != PowerDeviceD0) {

        // No, now allocate a DEVICE_POWER_STATE Irp and use
        // PoCallDriver to send it to ourselves.

        DbgTrace((UsbpDebugTrace, " >Requesting HC D-State - %d\n", powerState.DeviceState));

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'RqD0', FdoDeviceObject,
                 devExt->Dx_State, powerState.DeviceState);


        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitD0_PoComplete,
                                   FdoPwrEv_S0_IoComplete);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        ntStatus = PoRequestPowerIrp(FdoDeviceObject,
                                     IRP_MN_SET_POWER,
                                     powerState,
                                     USBPORT_PoRequestCompletionD0,
                                     SystemPowerIrp,    // Context
                                     NULL);

        if (ntStatus == STATUS_PENDING) {

            if (devExt->Fdo.FastResumeEnable) {

                // If Fast Resume is enabled then the SYSTEM_POWER_STATE
                // Irp is completed here.
                //
                // Allow the SystemPowerIrp to complete.
                //
                ntStatus = STATUS_SUCCESS;

            } else {

                // If Fast Resume is not enabled then the
                // SYSTEM_POWER_STATE Irp is completed by the work item
                // call to USBPORT_PowerUpPhase1() which is queued
                // by the USBPORT_PoRequestCompletionD0() completion
                // routine.
                //
                // Do not allow the SystemPowerIrp to complete.
                //
                return STATUS_MORE_PROCESSING_REQUIRED;
            }
        }

    } else {

        // Already powering up, powered up, or error occurred.

        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   fdoState,
                                   FdoPwrEv_S0_IoComplete);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        ntStatus = STATUS_SUCCESS;
    }

    // We are already in the requested D state, or an error occurred.
    // Just complete the system irp.
    //
    // This is also where the SystemPowerIrp is completed in the Fast
    // Resume case.
    //
    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject,
                                                       SystemPowerIrp);

    PoStartNextPowerIrp(SystemPowerIrp);

    USBPORT_Core_FreeIoRequest(usbIoRequest);

    // This allows the SystemPowerIrp to complete as ntStatus is not
    // STATUS_MORE_PROCESSING_REQUIRED here.
    //
    return ntStatus;
}


NTSTATUS
USBPORT_FdoSystemPowerIrpCompletion(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP SystemPowerIrp,
    PVOID Context
    )
/*++

Routine Description:

    Handle the completion of the pended SYSTEM_POWER_STATE
    SystemPowerIrp.

    Either USBPORT_SystemPowerS0IrpCompletion() or
    USBPORT_SystemPowerSxIrpCompletion() will request the
    DEVICE_POWER_STATE Irp.

Arguments:

Return Value:


--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    SYSTEM_POWER_STATE requestedSystemState;

    irpStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == SystemPowerState);

    requestedSystemState = irpStack->Parameters.Power.State.SystemState;

    switch (requestedSystemState) {
    case PowerSystemWorking:
        DbgTrace((UsbpDebugTrace, " >SystemWorking\n"));
        ntStatus = USBPORT_SystemPowerS0IrpCompletion(FdoDeviceObject,
                                                      SystemPowerIrp,
                                                      Context);
        break;

    case PowerSystemShutdown:
    case PowerSystemHibernate:
    case PowerSystemSleeping1:
    case PowerSystemSleeping2:
    case PowerSystemSleeping3:
    default:

        DbgTrace((UsbpDebugTrace, " >Sleeping Detected\n"));
        ntStatus = USBPORT_SystemPowerSxIrpCompletion(FdoDeviceObject,
                                                      SystemPowerIrp,
                                                      Context);
        break;
    }

    return ntStatus;
}


NTSTATUS
USBPORT_FdoDevicePowerIrpCompletion(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP DevicePowerIrp,
    PVOID Context
    )
/*++

Routine Description:

    Handle completion of the pended device power Irp.

    we just complete the irp and release the Po lock.

Arguments:

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED

--*/
{
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, DevicePowerIrp);

    PoStartNextPowerIrp(DevicePowerIrp);

    USBPORT_Core_FreeIoRequest(usbIoRequest);

    return STATUS_SUCCESS;
}



NTSTATUS
USBPORT_FdoSystemPowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP SystemPowerIrp
    )
/*++

Routine Description:

    Handle SystemPowerState Messages for the HC FDO

    In keeping with the current power protocol we pass the system irp
    down first before requesting the appropriate D irp
    from the S irp completion routine.

    FUNCTION Map:

Power Down
    <FdoDispatch>(SystemIrp)
        USBPORT_FdoSystemPowerState(SystemIrp)
            PoCallDriver(SystemIrp)
            STATUS_PENDING
        USBPORT_FdoSystemPowerIrpCompletion(SystemIrp)
            PoRequestPowerIrp(DeviceIrp)
            STATUS_MORE_PROCESSING_REQUIRED
        USBPORT_FdoDevicePowerState(DeviceIrp)
            {turn off HW}
            PoCallDriver(DeviceIrp)
            STATUS_PENDING
        USBPORT_FdoDevicePowerIrpCompletion
            IoCompleteRequest(DeviceIrp)
        USBPORT_PoRequestCompletion(DeviceIrp)
            IoCompleteRequest(SystemIrp)


Power Up
        USBPORT_FdoSystemPowerState(SystemIrp)
            PoCallDriver(SystemIrp)
            STATUS_PENDING
        USBPORT_SystemPowerIrpCompletion(SystemIrp)
            PoRequestPowerIrp(DeviceIrp)
            STATUS_MORE_PROCESSING_REQUIRED
        USBPORT_FdoDevicePowerState(DeviceIrp)
            PoCallDriver(DeviceIrp)
        USBPORT_PoRequestCompletion
            USBPORT_QueuePowerWorker <phase1>
        USBPORT_PowerUpPhase1
            IoCompleteRequest(SystemIrp)
            USBPORT_QueuePowerWorker <phase2>
        USBPORT_PowerUpPhase2
            USBPORT_SynchronizeUsbControllersResume

Arguments:

    DeviceObject - pointer to a hcd device object (FDO)

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    SYSTEM_POWER_STATE requestedSystemState, realSystemState;
    BOOLEAN isHybridSleep;
    KIRQL irql;

    irpStack = IoGetCurrentIrpStackLocation(SystemPowerIrp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == SystemPowerState);

    requestedSystemState = irpStack->Parameters.Power.State.SystemState;

    DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_SET_POWER SYS(%d)\n",
            FdoDeviceObject, requestedSystemState));

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'RspS', 0,
        FdoDeviceObject, requestedSystemState);

    // ** begin special case
    // OS may send us a power irps even if we are not 'started'. In this
    // case we just pass them on with 'STATUS_SUCCESS' since we don't
    // really need to do anything.

    if (USBPORT_GetPnpState(devExt) != PnpStarted) {
        // we should probably be in an 'Unspecified' power state.
        // FdoPowerIrp will just call down the request
        PUSB_IOREQUEST_CONTEXT usbIoRequest;

        PoStartNextPowerIrp(SystemPowerIrp);
        IoSkipCurrentIrpStackLocation(SystemPowerIrp);

        usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, SystemPowerIrp);

        ntStatus = PoCallDriver(devExt->Fdo.TopOfStackDeviceObject, SystemPowerIrp);

        USBPORT_Core_FreeIoRequest(usbIoRequest);
        return ntStatus;
    }
    // ** end special case

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

    if (requestedSystemState > PowerSystemWorking) {
        // Root hub would be going into Sx - ignore resume signal till we complete the Sx Irp
        devExt->Fdo.Flags.IgnoreResumeSignaling = 1;
    }
    else {
        devExt->Fdo.Flags.IgnoreResumeSignaling = 0;
    }

    isHybridSleep = FALSE;
    realSystemState = requestedSystemState;
    if (USBPORT_IsHybridSleepState(FdoDeviceObject, SystemPowerIrp, &realSystemState)) {
        HC_FDO_POWERSTATE fdoState;

        isHybridSleep = TRUE;
        fdoState = devExt->Fdo.CurrentFdoPowerState;

        requestedSystemState = realSystemState;
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   fdoState,
                                   FdoPwrEv_SetSxHybridSleepDetected);
    }

    if (requestedSystemState != PowerSystemWorking) {
        devExt->Fdo.PowerFlags.HcIsWakeupSource = 0;
        devExt->Fdo.PowerFlags.HcIsSystemWakeSource = 0;
    }

    switch (requestedSystemState) {
    case PowerSystemWorking:
        // go to 'ON'
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitS0_IoComplete,
                                   FdoPwrEv_SetS0);

        USBPORT_WMI_EVENT(1,
                          USBPORT_SYSTEM_POWER_RESUME,
                          (USBPORT_EVENT_TYPE_S0_DISPATCH,
                           GUID_USB_PERFORMANCE_TRACING,
                           FdoDeviceObject,
                           (ULONG)devExt->Dx_State,
                           "S0 Power Irp dispatched by FDO %p.  Device is in power state %d",
                           FdoDeviceObject,
                           devExt->Dx_State));
        break;
    case PowerSystemShutdown:
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitSx_IoComplete,
                                   FdoPwrEv_SetS5);
        SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        break;
    case PowerSystemHibernate:
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitSx_IoComplete,
                                   FdoPwrEv_SetS4);
         if (devExt->Fdo.ForceHCResetOnResume) {
            SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        } else {
            CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        }
        break;
    case PowerSystemSleeping1:
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitSx_IoComplete,
                                   FdoPwrEv_SetS1);
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        break;
    case PowerSystemSleeping2:
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitSx_IoComplete,
                                   FdoPwrEv_SetS2);
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        break;
    case PowerSystemSleeping3:
         USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   SystemPowerIrp,
                                   FdoPwr_WaitSx_IoComplete,
                                   FdoPwrEv_SetS3);
        if (devExt->Fdo.ForceHCResetOnResume) {
            SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        } else {
            CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        }
        break;
    }


    // Force a controller reset on resume if the controller is in the
    // hibernation path and the system is transitioning into a hybrid
    // sleep state. This is required because the crash dump stack
    // modifies the controller state when it writes out the hiberfile.

    if (devExt->Fdo.HibernateCount && isHybridSleep) {
        SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
    }

    USBPORT_RELEASE_POWER_LOCK(devExt, irql);

    IoMarkIrpPending(SystemPowerIrp);
    IoCopyCurrentIrpStackLocationToNext(SystemPowerIrp);
    IoSetCompletionRoutine(SystemPowerIrp,
                           USBPORT_FdoSystemPowerIrpCompletion,
                           NULL,
                           TRUE,
                           TRUE,
                           TRUE);

    PoCallDriver(devExt->Fdo.TopOfStackDeviceObject,
                 SystemPowerIrp);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_FdoDevicePowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP DevicePowerIrp
    )
/*++

Routine Description:

    Handle DevicePowerState Messages for the HC FDO

Arguments:

    DeviceObject - pointer to a hcd device object (FDO)

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

    returning STATUS_PENDING indicates that the Irp should
    not be called down to the PDO yet.

--*/
{
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION devExt, rhDevExt;
    DEVICE_POWER_STATE requestedDeviceState;
    KIRQL irql, dmirql;
    PDEVICE_OBJECT rhPdo;
    ULONG contextIdx;

    irpStack = IoGetCurrentIrpStackLocation(DevicePowerIrp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    USBPORT_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    USBPORT_ASSERT(irpStack->Parameters.Power.Type == DevicePowerState);

    requestedDeviceState = irpStack->Parameters.Power.State.DeviceState;

    DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_SET_POWER DEV(%d)\n",
            FdoDeviceObject, requestedDeviceState));

    switch (requestedDeviceState) {
    case PowerDeviceD0:
        // we actually power up in the D0 PoCompletion when our device is in D0
        // so we just pass on here.
        break;

    case PowerDeviceD1:
    case PowerDeviceD2:
    case PowerDeviceD3:

        // turn the controller off if necessary

        rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_Power);

        if ((REGISTRATION_PACKET(devExt).HciType == USB_EHCI) &&
            (devExt->Fdo.RuntimePMContext.Handle != NULL)) {

            USB_MINIPORT_STATUS mpStatus;
            USHORT port;
            ULONG usb2Nports;
            RH_PORT_STATUS portStatus;
            SUB_DX_STATE subDxState;
            ROOTHUB_DATA hubData;
            NTSTATUS status;

            subDxState = SubDxStateDisconnected;

            MPRH_GetRootHubData(devExt, &hubData);
            usb2Nports = hubData.NumberOfPorts;

            for (port=1; port <= usb2Nports; port++) {
                MPRH_GetPortStatus(devExt,
                                   port,
                                   &portStatus,
                                   mpStatus);

                if (portStatus.Connected) {
                    subDxState = SubDxStateConnected;
                    break;
                }
            }

            //
            // This Power control code is used to inform the PEP about the state of
            // the root ports of the EHCI controller. PEP can use this information
            // to optimize power when the controller enters a Dx state
            //
            status = PoFxPowerControl(devExt->Fdo.RuntimePMContext.Handle,
                                      (LPCGUID)&GUID_HC_SUB_DX_STATE,
                                      (PVOID)&subDxState,
                                      sizeof(subDxState),
                                      NULL,
                                      0,
                                      NULL);

            if (!NT_SUCCESS(status)) {
                LOGENTRY(FdoDeviceObject, LOG_POWER, 'FxRH', 0, 0, status);
            }
        }

        if (rhPdo) {
            DEVICE_POWER_STATE targetDxState;

            // if the root hub is in D2 or D1 then
            // we leave the hardwre in its current
            // suspend state.
            // otherwise we turn the controller off.
            //
            // NOTE: the pdo may still be in D0 if the root hub is disabled.

            GET_DEVICE_EXT(rhDevExt, rhPdo);
            ASSERT_PDOEXT(rhDevExt);

            targetDxState = rhDevExt->Dx_State;

            //
            // For selective suspend, the hub PDO will remain
            // in D0 while the controller will enter Dx.
            if (rhDevExt->Pdo.SelectiveSuspend) {
                targetDxState = PowerDeviceD2;
            } else if (devExt->Fdo.PowerFlags.ForceControllerOff) {
                targetDxState = PowerDeviceD3;
            }

            switch (targetDxState) {
            case PowerDeviceD3:
            case PowerDeviceD0:
            case PowerDeviceUnspecified:

                USBPORT_TurnUsbControllerOff(FdoDeviceObject);
                break;
            }

            USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_Power);
        } else {
            // no root hub, just turn off
            USBPORT_TurnUsbControllerOff(FdoDeviceObject);
        }

        //
        // Let dump driver that a hiberfile will be written
        //
        for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
            if (devExt->DumpContext[contextIdx] != NULL) {

              devExt->DumpContext[contextIdx]->Hibernate = TRUE;
            }
        }

        // it is 'impure' for the controller to interrupt while in a
        // low power state so if we suspended it we disable interrupts now.
        // The presence of a wake IRP should enable the PME that wakes
        // the system.
        MP_DisableInterrupts(FdoDeviceObject);

        // we won't have access to the decodes regardless of what we have done
        // to the controller.

        USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ACQUIRE_DM_LOCK(devExt, dmirql);
        devExt->Fdo.DmTimerFlags.DecodesOff = 1;
        USBPORT_RELEASE_DM_LOCK(devExt, dmirql);
        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, irql);

        //
        // Since we don't have access to the hardware, we can stop the deadman timer,
        //
        USBPORT_SuspendDM_Timer(FdoDeviceObject);

        //
        // If an interrupt is connected, then report the interrupt as inactive
        // This is important for the case where this interrupt is also used
        // to signal wake, as it would cause an interrupt storm since the wake signal
        // may not be acknowledgable until after we return to D0.
        //
        if (devExt->Fdo.Flags.IrqConnected) {
            IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS	parameters;

            RtlZeroMemory(&parameters,
                          sizeof(IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS));

            parameters.Version = CONNECT_FULLY_SPECIFIED;
            parameters.ConnectionContext.InterruptObject = devExt->Fdo.InterruptObject;
            IoReportInterruptInactive(&parameters);
        }


        //
        // Now, just to be sure we are done touching hardware, lets flush the
        // DPCs
        // Under some rare circumstances, this function COULD run at dispatch.
        // In this case, we will just hope everything will be fine.
        //
        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
            KeFlushQueuedDpcs();
        }


        break;

    case PowerDeviceUnspecified:

        // for unspecified we will turn the HW off (D3) -- I'm not sure we
        // will ever see this since the D messages originate from us.
        USBPORT_TurnUsbControllerOff(FdoDeviceObject);
        break;

    default:
        break;

    };

    IoMarkIrpPending(DevicePowerIrp);
    IoCopyCurrentIrpStackLocationToNext(DevicePowerIrp);
    IoSetCompletionRoutine(DevicePowerIrp,
                               USBPORT_FdoDevicePowerIrpCompletion,
                               NULL,
                               TRUE,
                               TRUE,
                               TRUE);

    PoCallDriver(devExt->Fdo.TopOfStackDeviceObject,
                 DevicePowerIrp);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_ProcessHcWakeIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Process the HC wake irp

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    USBHC_WAKE_STATE oldWakeState;
    PUSB_IOREQUEST_CONTEXT ior;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    devExt->Fdo.HcPendingWakeIrp = Irp;
    // Advance the state if the armed if we are to proceed
    oldWakeState = InterlockedCompareExchange( (PULONG) &devExt->Fdo.HcWakeState,
                                                HCWAKESTATE_ARMED,
                                                HCWAKESTATE_WAITING );

    if (oldWakeState == HCWAKESTATE_WAITING_CANCELLED) {

        // We got disarmed, finish up and complete the IRP
        devExt->Fdo.HcWakeState = HCWAKESTATE_COMPLETING;

        ior = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, Irp);
        USBPORT_ASSERT(ior == UsbIoRequest);

        Irp->IoStatus.Status = STATUS_CANCELLED;
        USBPORT_EtwWriteHcData2(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_DISPATCH_WAIT_WAKE,
                                NULL,

                                &Irp,
                                sizeof(PIRP),

                                &Irp->IoStatus.Status,
                                sizeof(Irp->IoStatus.Status));
        IoCompleteRequest( Irp, IO_NO_INCREMENT);

        USBPORT_Core_FreeIoRequest(UsbIoRequest);

        return STATUS_CANCELLED;
    }
    // We went from WAITING to ARMED. Set a completion routine and forward
    // the IRP. Note that our completion routine might complete the IRP
    // asynchronously, so we mark the IRP pending
    IoMarkIrpPending(Irp);
    IoCopyCurrentIrpStackLocationToNext( Irp );
    IoSetCompletionRoutine( Irp,
                         USBPORT_HcWakeIrp_Io_Completion,
                         NULL,
                         TRUE,
                         TRUE,
                         TRUE);

    ior = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, Irp);
    USBPORT_ASSERT(ior == UsbIoRequest);

    USBPORT_EtwWriteHcData2(FdoDeviceObject,
                            &USBPORT_ETW_EVENT_DISPATCH_WAIT_WAKE,
                            NULL,

                            &Irp,
                            sizeof(PIRP),

                            &Irp->IoStatus.Status,
                            sizeof(Irp->IoStatus.Status));
    PoCallDriver(devExt->Fdo.TopOfStackDeviceObject, Irp);

    USBPORT_Core_FreeIoRequest(UsbIoRequest);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_FdoPowerIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Process the Power IRPs sent to the FDO for the host
    controller.

Arguments:

    DeviceObject - pointer to a hcd device object (FDO)
    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION devExt;
    PUSB_IOREQUEST_CONTEXT ior;

#ifdef USB_FORCE_POWER_AT_DISPATCH
    KIRQL irql;
    BOOLEAN irqlRaised = FALSE;
    if (!(FdoDeviceObject->Flags & DO_POWER_PAGABLE)) {
        KeRaiseIrql(DISPATCH_LEVEL, &irql);
        irqlRaised = TRUE;
    }
#endif

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'fPow', irpStack->MinorFunction,
        FdoDeviceObject, devExt->Dx_State);
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'fpow',
             irpStack->Parameters.Others.Argument1,     // WAIT_WAKE: PowerState
             irpStack->Parameters.Others.Argument2,     // SET_POWER: Type
             irpStack->Parameters.Others.Argument3);    // SET_POWER: State

    //
    // All PNP_POWER POWER messages get passed to the
    // top of the PDO stack we attached to when loaded
    //
    // In some cases we finish processing in a completion
    // routine
    //

    // pass on to our PDO

    switch (irpStack->MinorFunction) {
    case IRP_MN_WAIT_WAKE:

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_WAIT_WAKE\n",
            FdoDeviceObject));
        ntStatus = USBPORT_ProcessHcWakeIrp(FdoDeviceObject, Irp, UsbIoRequest);

        // USBPORT_ProcessHcWakeIrp handles completions/calldowns
        break;

    case IRP_MN_SET_POWER:

        if (irpStack->Parameters.Power.Type == SystemPowerState) {
            ntStatus = USBPORT_FdoSystemPowerState(FdoDeviceObject, Irp);
            DbgTrace((UsbpDebugTrace,
                "SET_POWER System %p %x\n", Irp, ntStatus));
        } else {
            ntStatus = USBPORT_FdoDevicePowerState(FdoDeviceObject, Irp);
            DbgTrace((UsbpDebugTrace,
                "SET_POWER Device %p %x\n", Irp, ntStatus));
        }

        break;

    case IRP_MN_QUERY_POWER:

        // we succeed all requests to enter low power
        // states for the HC fdo
        Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
        LOGENTRY(FdoDeviceObject, LOG_POWER, 'QpFD', 0, 0, ntStatus);

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_QUERY_POWER\n",
            FdoDeviceObject));

        ior = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, Irp);
        USBPORT_ASSERT(ior == UsbIoRequest);

        // pass on to our PDO
        PoStartNextPowerIrp(Irp);
        IoSkipCurrentIrpStackLocation(Irp);
        ntStatus =
            PoCallDriver(devExt->Fdo.TopOfStackDeviceObject,
                         Irp);

        USBPORT_Core_FreeIoRequest(UsbIoRequest);

        break;

    default:

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER HC fdo(%p) MN_%d not handled\n",
            FdoDeviceObject,
            irpStack->MinorFunction));

        ior = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject, Irp);
        USBPORT_ASSERT(ior == UsbIoRequest);

        IoSkipCurrentIrpStackLocation(Irp);
        ntStatus = PoCallDriver(devExt->Fdo.TopOfStackDeviceObject, Irp);
        USBPORT_Core_FreeIoRequest(UsbIoRequest);

    } /* irpStack->MinorFunction */


#ifdef USB_FORCE_POWER_AT_DISPATCH
    if (irqlRaised) {
        KeLowerIrql(irql);
    }
#endif

    return ntStatus;
}


NTSTATUS
USBPORT_PdoSystemPowerIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for System Power Irps sent to the PDO for the root hub.

    NOTE:
        irps sent to the PDO are always completed by the bus driver

Arguments:

    DeviceObject - Pdo for the root hub

Return Value:

    NTSTATUS

--*/
{
    KIRQL powIrql;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION rhDevExt, devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    HC_FDO_POWERSTATE fdoPwrState;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    irpStack = IoGetCurrentIrpStackLocation (Irp);
    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    ASSERT_PASSIVE();

    LOGENTRY(fdoDeviceObject, LOG_POWER, 'RspS', 0, 0,
        irpStack->Parameters.Power.Type);

    // we no longer support the 'wakeoninterrupt' flag --
    // this flag was originally for selective suspend.
    //
    // This prevents us from waking up prematurely due to
    // something like 'moving the mouse'.
    //
    // since the fdo driver for the root hub pdo is our own
    // hub driver and it is well behaved, we don't expect to see
    // a system message where the power state is still undefined
    //
    // we just complete this with success
    //

    // no state change but we record the event for debugging
    USBPORT_ACQUIRE_POWER_LOCK(devExt, powIrql);
    fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

    if (fdoPwrState == FdoPwr_WaitSx) {
        // ** special case Root hub goes to SX but USB HC does not (remanis in S0, D0) (OS backed out of suspend)
        // if the root hub entered SX the controller will be suspended, in this case we perform
        // a synchronous resume of the controller in the context of the S irp.
        //TEST_TRAP();
        USBPORT_SetHcFdoPowerState(fdoDeviceObject,
                   0,
                   fdoPwrState,
                   FdoPwrEv_Rh_SetPowerSys_S0_Resume);

        DbgTrace((UsbpDebugTrace,
            "RESUME-MJ_POWER RH pdo(%p) MN_SET_POWER SYS(%d) irp %p\n",
            PdoDeviceObject,
            irpStack->Parameters.Power.State.SystemState,
            Irp));

        USBPORT_RELEASE_POWER_LOCK(devExt, powIrql);

        if (USBPORT_IsRuntimePMEnabled(fdoDeviceObject) &&
            devExt->Fdo.CurrentSystemPowerState == PowerSystemWorking &&
            irpStack->Parameters.Power.State.SystemState == PowerSystemWorking) {

            //
            // Later in the Win 8 Product Cycle, the following DbgPrint should be removed.
            //
            DbgPrint("USBPORT: Hard Hang Averted: RtpmState %d\n", devExt->Fdo.RuntimePMContext.RtpmSmContext.CurrentState[0].RtpmState);

            //
            // Skip Resume Controller since this is really case of getting a set S0, while we are
            // already in S0. At this time it is possible that the controller is in a Dx state, and
            // the phy and the controller are powered off. At that time, trying to resume the
            // controller (without bringing it back to D0) causes a hard-hang.
            // However since this is a new change post Windows 8, we are scoping it
            // down to systems that support runtime power management to reduce risk of regression.
            //
            //
        } else {
            USBPORT_ResumeUsbController(fdoDeviceObject);
        }

    } else {


        USBPORT_SetHcFdoPowerState(fdoDeviceObject,
                       0,
                       fdoPwrState,
                       FdoPwrEv_Rh_SetPowerSys);

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER RH pdo(%p) MN_SET_POWER SYS(%d) irp %p\n",
            PdoDeviceObject,
            irpStack->Parameters.Power.State.SystemState,
            Irp));

        USBPORT_RELEASE_POWER_LOCK(devExt, powIrql);
    }

    // Remember  current system power state
    devExt->Fdo.CurrentSystemPowerState =
        irpStack->Parameters.Power.State.SystemState;

    PoStartNextPowerIrp(Irp);

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject,
                        Irp,
                        UsbIoRequest,
                        STATUS_SUCCESS,
                        0);

    // not allowed to fail this
    return STATUS_SUCCESS;
}

NTSTATUS
USBPORT_PdoDevicePowerIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for Device Power Irps sent to the PDO for the root hub.

    NOTE:
        irps sent to the PDO are always completed by the bus driver

Arguments:

    DeviceObject - Pdo for the root hub

Return Value:

    NTSTATUS

--*/
{
    KIRQL powIrql;
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;
    PDEVICE_EXTENSION rhDevExt, devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    HC_FDO_POWERSTATE fdoPwrState;
    DEVICE_POWER_STATE deviceState;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    irpStack = IoGetCurrentIrpStackLocation (Irp);
    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);

    deviceState =
            irpStack->Parameters.Power.State.DeviceState;

    DbgTrace((UsbpDebugTrace,
        "MJ_POWER RH pdo(%p) MN_SET_POWER DEV(%d) irp %p\n",
        PdoDeviceObject,
        deviceState,
        Irp));

    LOGENTRY(fdoDeviceObject, LOG_POWER, 'RspD', deviceState, UsbIoRequest,
        irpStack->Parameters.Power.Type);

    // Handle D states for the ROOT HUB Pdo:
    //
    // NOTE:
    // if the root hub is placed in D3 then it is considered OFF.
    //
    // if the root hub is placed in D2 or D1 then it is 'suspended',
    // the hub driver should not do this unless all the ports have
    // been selectively suspended first
    //
    // if the root hub is placed in D0 it is on
    //

    // We are not required to take any action here, however
    // this is where 'selective suspend' of the bus is handled
    //
    // For D1 - D3 we can tweak the host controller, ie stop
    // the schedule disable ints, etc. since it won't be in use
    // while the root hub PDO is suspended.
    //
    // Whatever we do to the controller here we need to be able to
    // recognize resume signalling.

    switch (deviceState) {

    case PowerDeviceD0:
        // re-activate controller if idle
        USBPORT_ACQUIRE_POWER_LOCK(devExt, powIrql);

        if (devExt->Dx_State != PowerDeviceD0) {
            // hub going to D0 before the controller is in D0  we might
            // hit this if we couldn't get to D0 for some reason.

            TEST_TRAP();
            // trap the condition in case this is our bug

            // jd_power -- we need to evolve the power fault into something more freindly
            // and write something to the event log.
            if (devExt->Fdo.PowerError_Nts == STATUS_NO_SUCH_DEVICE &&
                devExt->Fdo.CurrentFdoPowerState == FdoPwr_PowerError) {
                USBPORT_PowerFault(fdoDeviceObject, "FdoPwr_PowerError: HC Removed");
            } else {
                USBPORT_PowerFault(fdoDeviceObject, "FdoPwr_PowerError: unkonwn error");
            }

            USBPORT_RELEASE_POWER_LOCK(devExt, powIrql);

            // fail the request with the status logged in the power error
            ntStatus = devExt->Fdo.PowerError_Nts;

        } else {


            fdoPwrState = devExt->Fdo.CurrentFdoPowerState;
            // controller hasn't quite finished powering up, wait for
            // it to finish.

            // However, we do not sit on the D irp, we release it anway
            // intialize a sideband mechnaism to tell if the controller is
            // ready.

            // This allows us to release the D and S irps and power up in
            // the background -- this is particulartly useful if the hub
            // does not have any devices.
            USBPORT_SetHcFdoPowerState(fdoDeviceObject,
                               0,
                               fdoPwrState,
                               FdoPwrEv_Rh_SetD0);

            USBPORT_RELEASE_POWER_LOCK(devExt, powIrql);

            rhDevExt->Dx_State = deviceState;

            ntStatus = STATUS_SUCCESS;
        }

        break;

    // The controller is only turned off and on by power
    // action to the FDO, suspend and resume are tied
    // to the root hub PDO.

    case PowerDeviceD1:
    case PowerDeviceD2:
        ntStatus = STATUS_SUCCESS;
        USBPORT_SuspendController(fdoDeviceObject);
        rhDevExt->Dx_State = deviceState;
        break;

    case PowerDeviceD3:
        // suspend/idle the controller
        // for compat we still suspend the HW but we don't wake on an interrupt
        // by definition hubs cannot wake from D3
        ntStatus = STATUS_SUCCESS;
        USBPORT_SuspendController(fdoDeviceObject);
        rhDevExt->Dx_State = deviceState;
        break;

    case PowerDeviceUnspecified:
        // do nothing
        ntStatus = Irp->IoStatus.Status;
        break;
    }

    devExt->Fdo.Flags.IgnoreResumeSignaling = 0;

    LOGENTRY(fdoDeviceObject, LOG_POWER, 'dpwC', Irp, UsbIoRequest, ntStatus);

    PoStartNextPowerIrp(Irp);

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject,
                        Irp,
                        UsbIoRequest,
                        ntStatus,
                        0);

    return ntStatus;
}


NTSTATUS
USBPORT_PdoWaitWakeIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Handle th edreaded WaitWake Irp

Arguments:

    DeviceObject - Pdo for the root hub

Return Value:

    NTSTATUS

--*/
{
    KIRQL wkIrql;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION rhDevExt, devExt;
    PDEVICE_OBJECT fdoDeviceObject;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace,
        "MJ_POWER RH pdo(%p) MN_WAIT_WAKE\n",
        PdoDeviceObject));

    ACQUIRE_WAKEIRP_LOCK(fdoDeviceObject, wkIrql);
    do {
        // we only support one wait_wake irp pending
        // in the root hub -- basically we have a pending
        // irp table with one entry
        if (NULL != rhDevExt->Pdo.PendingWaitWakeIrp) {
            ntStatus = STATUS_DEVICE_BUSY;
            break;
        }

        IoSetCancelRoutine(Irp, USBPORT_CancelPendingWakeIrp);

        if (Irp->Cancel) {
            if (NULL != IoSetCancelRoutine(Irp, NULL)) {
                // irp was canceled and our cancel routine did not run
                ntStatus = STATUS_CANCELLED;
                break;
            }
        }

        IoMarkIrpPending(Irp);
        rhDevExt->Pdo.PendingWaitWakeIrp = Irp;
        LOGENTRY(fdoDeviceObject, LOG_POWER, 'pWWi',
            Irp, 0, 0);
        ntStatus = STATUS_PENDING;

    } WHILE (0);
    RELEASE_WAKEIRP_LOCK(fdoDeviceObject, wkIrql);

    return ntStatus;
}


NTSTATUS
USBPORT_PdoPowerIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Disptach routine for Power Irps sent to the PDO for the root hub.

    NOTE:
        irps sent to the PDO are always completed by the bus driver

Arguments:

    DeviceObject - Pdo for the root hub

Return Value:

    NTSTATUS

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PDEVICE_EXTENSION rhDevExt, devExt;
    PDEVICE_OBJECT fdoDeviceObject;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    USBPORT_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);

    // use whatever status is in the IRP by default
    ntStatus = Irp->IoStatus.Status;

    // PNP messages for the PDO created for the root hub
    LOGENTRY(fdoDeviceObject, LOG_POWER, 'pPow',
        irpStack->MinorFunction, PdoDeviceObject, ntStatus);
    LOGENTRY(fdoDeviceObject, LOG_POWER, 'ppow',
             irpStack->Parameters.Others.Argument1,     // WAIT_WAKE: PowerState
             irpStack->Parameters.Others.Argument2,     // SET_POWER: Type
             irpStack->Parameters.Others.Argument3);    // SET_POWER: State

    switch (irpStack->MinorFunction) {
    case IRP_MN_WAIT_WAKE:
        ntStatus = USBPORT_PdoWaitWakeIrp(PdoDeviceObject, Irp);
        if (ntStatus == STATUS_PENDING) {
            return STATUS_PENDING;
        }
        break;

    case IRP_MN_QUERY_POWER:

        ntStatus = STATUS_SUCCESS;
        LOGENTRY(fdoDeviceObject, LOG_POWER, 'RqrP', 0, 0, ntStatus);

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER RH pdo(%p) MN_QUERY_POWER\n",
            PdoDeviceObject));

        PoStartNextPowerIrp(Irp);
        break;

    case IRP_MN_SET_POWER:

        LOGENTRY(fdoDeviceObject, LOG_POWER, 'RspP', 0, 0,
            irpStack->Parameters.Power.Type);

        switch (irpStack->Parameters.Power.Type) {
        case SystemPowerState:

            return USBPORT_PdoSystemPowerIrp(PdoDeviceObject, Irp, UsbIoRequest);

        case DevicePowerState:

            return USBPORT_PdoDevicePowerIrp(PdoDeviceObject, Irp, UsbIoRequest);

        }
        break;

    default:
        //
        // default behavior for an unhandled Power irp is to return the
        // status currently in the irp
        // is this true for power?

        DbgTrace((UsbpDebugTrace,
            "MJ_POWER RH pdo(%p) MN_%d not handled Irp %p\n",
            PdoDeviceObject,
            irpStack->MinorFunction,
            Irp));

    } /* switch, POWER minor function */

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject,
                        Irp,
                        UsbIoRequest,
                        ntStatus,
                        0);

    return ntStatus;
}


VOID
USBPORT_RestoreUsbController(
     PDEVICE_OBJECT FdoDeviceObject
     )
/*++

Routine Description:

    Turns the controller back on to the 'suspended' state after a
    power event.

Arguments:

    DeviceObject - DeviceObject of the controller to turn off

Return Value:

    none.

--*/

{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'REST', 0, 0, 0);

    USBPORT_ASSERT(devExt->Dx_State == PowerDeviceD0);

    // we need only to enable interrupts
    MP_EnableInterrupts(FdoDeviceObject);

}


VOID
USBPORT_TurnUsbControllerOn(
     PDEVICE_OBJECT FdoDeviceObject
     )

/*++

Routine Description:

    Similar to start -- but we already have our resources.
    NOTE that the miniport activates as if it the system
    was booted normally.

    We only get here after the Set D0 request has been passed
    to the parent bus.

Arguments:

    DeviceObject - DeviceObject of the controller to turn off

Return Value:

    NT status code.

--*/

{
    PDEVICE_EXTENSION devExt;
    PHC_RESOURCES hcResources;
    USB_MINIPORT_STATUS mpStatus;
    KIRQL irql;
    ULONG mpOptionFlags = 0;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    hcResources = &devExt->Fdo.HcResources;
    mpOptionFlags = devExt->Fdo.HcOptionFlags;

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'C_on', 0, 0, 0);

    devExt->Fdo.PowerFlags.HcResumeFailure = 0;
    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    // indicate decodes off to prevent access while we re-init
    devExt->Fdo.DmTimerFlags.DecodesOff = 1;

    if(!TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_KEEP_DEVICE_DATA)){

        // zero the controller extension
        RtlZeroMemory(devExt->Fdo.MiniportDeviceData,
                      devExt->Fdo.MiniportDriver->RegistrationPacket.DeviceDataSize);
    }

    // attempt to re-start the miniport, indicate this is a restart vs a regular start
    // the  HC will set the config flag when initialized.

    // ** note the decodeOff flag is still set, this will prevent other routines from accessing
    // ** the HC during the re-start
    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    mpStatus = MPf_StartController(devExt, hcResources, MPF_RESTART);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'mpRS', mpStatus, 0, 0);

    if (mpStatus == USBMP_STATUS_SUCCESS) {
        // controller started, set flag and begin passing
        // interrupts to the miniport
        SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED);
        LOGENTRY(FdoDeviceObject, LOG_POWER, 'rIRQ', mpStatus, 0, 0);

        USBPORT_RestoreConfigFlag(FdoDeviceObject);

        MP_EnableInterrupts(FdoDeviceObject);

        // root hub change interrupt is expected to be initially disabled
        MPRH_DisableIrq(devExt, ROOTHUB_LOCK_NOT_ACQUIRED);

        USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
        devExt->Fdo.DmTimerFlags.SkipTimerWork = 0;
        devExt->Fdo.DmTimerFlags.DecodesOff = 0;
        devExt->Fdo.DmTimerFlags.FailUrbs = 0;
        USBPORT_RELEASE_DM_LOCK(devExt, irql);

    } else {
        KIRQL cf_irql;
        // note we could queued some sort of reset of the host controller here
        // but it shouldn't be necessary
        USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, &cf_irql);

        devExt->Fdo.PowerFlags.MpRestartFailed = 1;

        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, cf_irql);

        if (devExt->Fdo.PMDebug) {
            NT_ASSERTMSG("Miniport Restart Failed",FALSE);
        }
        IoInvalidateDeviceState(devExt->Fdo.PhysicalDeviceObject);
    }

    //
    // we are now fully powered and in D0
    //
    USBPORT_ASSERT(devExt->Dx_State == PowerDeviceD0);
    devExt->Fdo.PowerFlags.Off = 0;

}


VOID
USBPORT_SuspendController(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Suspends the USB Host controller, prepare it for Sx (System) suspend. This routine is used for system state
    suspend. The controller hardware is put into a state where it can recognize resume signalling.  Eventually all
    interrupts will be disabled and the system will wake on a PME.

    ** This routine is NOT used for the global portion of selective suspend.

Arguments:

    FdoDeviceObject - DeviceObject of the controller to suspend

Return Value:

    this is NON FAILABLE.

--*/

{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // There should be no transfers on the HW at time of suspend.
    // ** FailUrbs will cause URB apis to be failed in the Dispatch routine
    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    devExt->Fdo.DmTimerFlags.FailUrbs = 1;
    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    // flushes all active transfers off the hardware.  The FailUrbs flag will block new
    // transfers from being queued
    USBPORT_Core_FlushController(FdoDeviceObject);

    // in case any are still active
    //USBPORT_Core_SuspendAllEndpoints(FdoDeviceObject);

    // force transition pending endpoints
    USBPORT_ProcessNeoStateChangeList(FdoDeviceObject);

    USBPORT_ASSERT(!devExt->Fdo.PowerFlags.Off);

    // Our job here is to 'idle' controller and twiddle the
    // appropriate bits to allow it to recognize resume signalling
    if (!devExt->Fdo.PowerFlags.Suspended) {

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'Csus', 0, 0, 0);

        DbgTrace((UsbpDebugTrace, " >SUSPEND controller\n"));


        // tell the DM timer not to poll the controller
        USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
        devExt->Fdo.DmTimerFlags.SkipTimerWork = 1;
        USBPORT_RELEASE_DM_LOCK(devExt, irql);

//        if (TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {
//
//            SET_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);
//
//            // introduce a 10ms wait here to allow any
//            // port suspends to finish -- according to spec
//            USBPORT_Wait(FdoDeviceObject, 10);
//
//            MPf_SuspendController(devExt);
//
//        }
        MPf_SuspendController(devExt, FALSE);

        USBPORT_AcquireSpinLock(FdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, &irql);

        devExt->Fdo.PowerFlags.Suspended = 1;

        USBPORT_ReleaseSpinLock(FdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, irql);


    }
}


VOID
USBPORT_RestoreConfigFlag(
    PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    restores the state of the config flag after a controller re-start

Arguments:

    DeviceObject -

Return Value:

    none

--*/

{
    ULONG rhCfgFlag = 1;
    PDEVICE_OBJECT rhPdo;
    PDEVICE_EXTENSION rhDevExt, hcDevExt;
    KIRQL rh_irql;

    GET_DEVICE_EXT(hcDevExt, HcFdo);
    ASSERT_FDOEXT(hcDevExt);

    if (USBPORT_IS_USB20(hcDevExt)) {

        USBPORT_AcquireSpinLock(HcFdo, &hcDevExt->Fdo.RhPdoPtrSpin, &rh_irql);

        rhPdo = hcDevExt->Fdo.RhPdoPtr;

        if (rhPdo) {
            GET_DEVICE_EXT(rhDevExt, rhPdo);
            ASSERT_PDOEXT(rhDevExt);

            rhCfgFlag = rhDevExt->Pdo.RhCfgFlag;
        } else {
            // no pdo, clear cfg flag
            rhCfgFlag = 0;
        }

        USBPORT_ReleaseSpinLock(HcFdo, &hcDevExt->Fdo.RhPdoPtrSpin, rh_irql);

        if (rhCfgFlag == 0) {
             MPf_ReleasePortControl(hcDevExt);
        }

    }
}


VOID
USBPORT_ResumeUsbController(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Suspends the USB Host controller

Arguments:

    DeviceObject - DeviceObject of the controller to turn off

Return Value:

    this is NON FAILABLE.

--*/

{
    PDEVICE_EXTENSION devExt;
    KIRQL dm_irql;
    ULONG mpOptionFlags = 0;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    mpOptionFlags = devExt->Fdo.HcOptionFlags;

    if (devExt->Fdo.PowerFlags.Suspended) {

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'Cres', 0, 0, 0);

        DbgTrace((UsbpDebugTrace, " >RESUME controller\n"));
        devExt->Fdo.PowerFlags.HcResumeFailure = 0;


        USBPORT_ASSERT(!devExt->Fdo.PowerFlags.Off);

        USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);

        if (!TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {

            // Nothing to do if the miniport is not currenlty
            // suspended.
            //
            USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

        } else {

            USB_MINIPORT_STATUS mpStatus;

            //CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);

            if (TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET)) {
                // force a hard reset on the controller, we should disable interrupts or stop the hc here since the
                // controller may still be running in this case
                LOGENTRY(FdoDeviceObject, LOG_POWER, 'Fres', 0, 0, 0);

                mpStatus = USBMP_STATUS_FORCE_RESET;
                CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);

                USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

                //** transfer may be in the queue, these will move when the HC begins generating interrupts so we must
                //** resume the endpoints first
                USBPORT_Core_ResumeAllEndpoints(FdoDeviceObject);


            } else {

                USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

                //** transfer may be in the queue, these will move when the HC begins generating interrupts so we must
                //** resume the endpoints first
                USBPORT_Core_ResumeAllEndpoints(FdoDeviceObject);

                // will clear MP_STATE_SUSPENDED
                mpStatus = MPf_ResumeController(devExt);
            }

            //mpStatus = USBMP_STATUS_HARDWARE_FAILURE;
            if (mpStatus != USBMP_STATUS_SUCCESS) {

                // we expect that the controller run bit is clear (if resume failed) so removal of the
                // endpoints can be immediate (not async).  The zap operation will attempt to stop the
                // hc from running.
                USBPORT_Core_ZapAllEndpoints(FdoDeviceObject);
                LOGENTRY(FdoDeviceObject, LOG_POWER, 'PMD3', 0, 0, devExt->Fdo.MpStateFlags);

                if (devExt->Fdo.PMDebug &&
                    TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET)) {
                   // USBPORT_LogDiagError();

                    DbgPrint("**USB Forced Reset\n");
                    NT_ASSERTMSG("USB Forced Reset",FALSE);
                }

                DbgTrace((UsbpDebugTrace, " >controller failed resume, re-start\n"));
                devExt->Fdo.PowerFlags.HcResumeFailure = 1;

                USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);
                devExt->Fdo.DmTimerFlags.SkipTimerWork = 1;
                USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

                MP_DisableInterrupts(FdoDeviceObject);

                MPRH_DisableIrq(devExt, ROOTHUB_LOCK_NOT_ACQUIRED);

                MPf_StopController(devExt, TRUE);

                USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);
                // the 'decodes off' flag tells us not to access the hardware, it is typically set when
                // in low power but is also used in the case where we need to reset the HC. It prevents
                // us from attempting to access the hc registers or the devicedata fields.
                devExt->Fdo.DmTimerFlags.DecodesOff = 1;
                if(!TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_KEEP_DEVICE_DATA)){
                    // zero the controller extension
                    RtlZeroMemory(devExt->Fdo.MiniportDeviceData,
                                  devExt->Fdo.MiniportDriver->RegistrationPacket.DeviceDataSize);
                }

                USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

                mpStatus = MPf_StartController(devExt, &devExt->Fdo.HcResources, MPF_RESTART);

                if (mpStatus == USBMP_STATUS_SUCCESS) {

                    // we need to clear the config flag if the root hub is disabled or if we get an error
                    // this allows devices to show up on the full speed bus

                    USBPORT_RestoreConfigFlag(FdoDeviceObject);

                    // don't need to enable interrupts if start failed
                    MP_EnableInterrupts(FdoDeviceObject);

                    // since we reset the controller we must re-request that
                    // any ports be chirp'ed.
                }  else {

                    KIRQL cf_irql;
                    // note we could queued some sort of reset of the host controller here
                    // but it shouldn't be necessary
                    USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, &cf_irql);

                    devExt->Fdo.PowerFlags.MpRestartFailed = 1;

                    USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.CoreFunctionSpin, cf_irql);

                    if (devExt->Fdo.PMDebug) {
                        NT_ASSERTMSG("Miniport StartController Failed",FALSE);
                    }
                    IoInvalidateDeviceState(devExt->Fdo.PhysicalDeviceObject);
                }
            }

            // neohub now dynamically handles this....
            // I left the wait commented for reference
            //
            // waitms 100 after bus resume before allowing drivers to talk
            // to the device.  Unfortuantely many USB devices are busted
            // and will not respond if accessed immediately after resume.
            // USBPORT_Wait(FdoDeviceObject, 100);

        }

        devExt->Fdo.PowerFlags.Suspended = 0;

        USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);
        devExt->Fdo.DmTimerFlags.SkipTimerWork = 0;
        devExt->Fdo.DmTimerFlags.DecodesOff = 0;
        devExt->Fdo.DmTimerFlags.FailUrbs = 0;
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

    } else {
        USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_FORCE_RESET);
        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);
    }


}


VOID
USBPORT_CompleteHubPdoWaitWake(
    PDEVICE_OBJECT FdoDeviceObject,
    LONG Wake_Status
    )
/*++

Routine Description:

    This routine is called to complete any pending wait_wake irp for the root
    hub PDO. If the host controller is flagged as the wakeup source
    (if it generated USB resume) then the irp s completed with success.

    If this is the case we complete the wait_wake irp for the USB hub (root),


Arguments:

Return Value:

    none

--*/
{
    PIRP irp;
    PDEVICE_EXTENSION rhDevExt, devExt;
    KIRQL irql;
    PDEVICE_OBJECT rhPdo;
    PUSB_IOREQUEST_CONTEXT usbIoRquest;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhPdo = USBPORT_RefRootHubPdo(FdoDeviceObject, REF_RhPDO_CompleteHubPdoWaitWake);
    if (rhPdo == NULL) {
        return;
    }

    GET_DEVICE_EXT(rhDevExt, rhPdo);
    ASSERT_PDOEXT(rhDevExt);

    ACQUIRE_WAKEIRP_LOCK(FdoDeviceObject, irql);
    irp = rhDevExt->Pdo.PendingWaitWakeIrp;

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'WAKi', FdoDeviceObject, irp, 0);

    if (irp != NULL &&
        IoSetCancelRoutine(irp, NULL)) {

        HC_FDO_POWERSTATE curPowerState;

        // we have an irp and the cancel routine has not
        // run, complete the irp.
        rhDevExt->Pdo.PendingWaitWakeIrp = NULL;

        RELEASE_WAKEIRP_LOCK(FdoDeviceObject, irql);

        USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

        curPowerState = devExt->Fdo.CurrentFdoPowerState;

        // Source of system wake is indicated only by completion of the wake irp
        // we post to PCI.

        if (NT_SUCCESS(Wake_Status)) {
            // Mark this wait/wake irp as waking the system if needed.

            if (devExt->Fdo.PowerFlags.HcIsSystemWakeSource) {
                PoSetSystemWake(irp);
            }

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   curPowerState,
                                   FdoPwrEv_Rh_Wake);
        } else {
            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   curPowerState,
                                   FdoPwrEv_Rh_NoWake);
        }

        // Clear the system wake source flag.

        devExt->Fdo.PowerFlags.HcIsSystemWakeSource = 0;

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        // since this irp was sent to the PDO we
        // complete it to the PDO
        DbgTrace((UsbpDebugTrace, " Complete PDO Wake Irp %p\n", irp));
        TEST_TRAP();

        usbIoRquest = USBPORT_Core_GetIoRequestFromObject(rhPdo, irp);
        USBPORT_Core_IoCompleteIrp(rhPdo,
                            irp,
                            NULL,
                            Wake_Status,
                            0);

        USBPORT_Core_FreeIoRequest(usbIoRquest);

        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_CompleteHubPdoWaitWake);

    } else {

        RELEASE_WAKEIRP_LOCK(FdoDeviceObject, irql);

        USBPORT_DerefRootHubPdo(FdoDeviceObject, rhPdo, REF_RhPDO_CompleteHubPdoWaitWake);
    }

    return;
}


NTSTATUS
USBPORT_HcWakeIrp_Io_Completion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

    Called when the HC wake irp completes we use this to hook completion
    so we can handle the cancel

    This routine runs before the USBPORT_USBPORT_HcWakeIrp_Po_Completion

Arguments:

Return Value:

    The function value is the final status from the operation.

--*/
{
    PDEVICE_EXTENSION devExt;
    USBHC_WAKE_STATE oldWakeState;

    GET_DEVICE_EXT(devExt, DeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_EtwWriteHcData2(DeviceObject,
                            &USBPORT_ETW_EVENT_COMPLETE_WAIT_WAKE,
                            NULL,

                            &Irp,
                            sizeof(PIRP),

                            &Irp->IoStatus.Status,
                            sizeof(Irp->IoStatus.Status));

    // Advance the state to completing
    oldWakeState = InterlockedExchange( (PULONG) &devExt->Fdo.HcWakeState,
                                        HCWAKESTATE_COMPLETING );

    if (oldWakeState == HCWAKESTATE_ARMED) {
        // Normal case, IoCancelIrp isn’t being called. Note that we already

        // marked the IRP pending in our dispatch routine
        return STATUS_SUCCESS;
    } else {
        USBPORT_ASSERT(oldWakeState == HCWAKESTATE_ARMING_CANCELLED);
        // IoCancelIrp is being called RIGHT NOW. The disarm code will try
        // to put back the WAKESTATE_ARMED state. It will then see our
        // WAKESTATE_COMPLETED value, and complete the IRP itself!

        return STATUS_MORE_PROCESSING_REQUIRED;
    }
}


VOID
USBPORT_HcWakeIrp_Po_Completion(
    PDEVICE_OBJECT DeviceObject,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:

    Called when a wake irp completes for the controller

Arguments:

    DeviceObject - Pointer to the device object for the class device.
    Irp - Irp completed.
    Context - Driver defined context.
    PowerState - Supposed to be power state passed to PoRequestPowerIrp

Return Value:

    None.

--*/
{
    PIRP irp;
    PDEVICE_EXTENSION devExt;
    PUSBHC_WAKE_CONTEXT wakeContext = Context;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    PDEVICE_OBJECT hcFdo;
    BOOLEAN     wakeFromS0;

    DbgTrace((UsbpDebugTrace,
            "HcWakeIrpCompletion (%x) ctx=%p\n", IoStatus->Status, wakeContext));

    ASSERT_HC_WAKE_CONTEXT(wakeContext);

    usbIoRequest = wakeContext->UsbIoRequest;
    ASSERT_USB_IOREQUEST(usbIoRequest);
    hcFdo = usbIoRequest->DevObj;
    devExt = GetHcFdoExt(hcFdo);
    wakeFromS0 = wakeContext->WakeFromS0;
    ASSERT_FDOEXT(devExt);
    irp = wakeContext->Irp;

    UsbFreePool(wakeContext);

    LOGENTRY(hcFdo, LOG_POWER, 'WAKc', usbIoRequest, IoStatus->Status, 0);

    //
    // Zero already freed IRP pointer (not necessary, but nice when debugging)
    //
    devExt->Fdo.HcPendingWakeIrp = NULL;
    //
    // Restore state (old state will have been completing)
    //
    devExt->Fdo.HcWakeState = HCWAKESTATE_DISARMED;

    if (IoStatus->Status == STATUS_SUCCESS) {
        LOGENTRY(devExt->HcFdoDeviceObject, LOG_POWER, 'WAK0', 0, 0, 0);

        // a successful completion of the wake Irp means something
        // generated resume signalling

        // The idea here is that we won't normally have a wake irp down
        // unless we are in some D state other than D0.  Remember
        // this is the controller FDO not the root hub, we arm the bus globally.
        // for entering a low system power state.

        // Note that resume signaling was detected by completion on the wake irp.
        // If this irp was marked as waking the system, set our internal flag
        // indicating that we woke the system.
        devExt->Fdo.PowerFlags.HcIsWakeupSource = 1;
        if (PoGetSystemWake(irp)) {
            devExt->Fdo.PowerFlags.HcIsSystemWakeSource = 1;
        }

        // we canceled the wake irp when entering D0 so we should
        // not see any completions unless we are in a low power
        // state

        DbgTrace((UsbpDebugTrace, " >Wakeup HC (new hub) detected\n"));

        // There are times when we simply CANNOT conform to the official doctrine.
        // We cannot request a D0 irp here because it will deadlock, since companion
        // controllers are tied to ehci we would need to request D0 for all controllers
        // and this creates a deadlock situation.  The current doctrine is based on the
        // assumption that all PCI devices are independent of each other.   This is
        // not true for USB EHCI controllers so we are forced to rewrite the rule book
        // to work around this.
        // If you file a bug about this I will simply reassign it to whoever owns Pnp
        // 'Pnp does not support this type of hardware -- which is the truth' this is
        // where it should be fixed anyway.


        // Since we know a SystemWorking message will follow so there is no real need
        // to request powerD0 at this time.

        // completion of wait_wake is only used to track source of resume.

    } else {
        // some other error, means we f'up probably with the
        // help of the ACPI BIOS

        devExt->Fdo.PowerFlags.HcIsWakeupSource = 0;

        if (IoStatus->Status == STATUS_CANCELLED) {
            LOGENTRY(hcFdo, LOG_POWER, 'WAK1',
                     0, 0, 0);
            DbgTrace((UsbpDebugTrace, " >Wakeup Irp Completed with cancel %x\n",
                    IoStatus->Status));


        } else {
            LOGENTRY(hcFdo, LOG_POWER, 'WAK2',
                     0, 0, 0);

            USBPORT_KdPrint((1, " >Wakeup Irp Completed with error %x\n",  IoStatus->Status));
            // if status is STATUS_INVALID_DEVICE_STATE then you need
            // to complain to the ACPI guys about your system not waking
            // from USB.  This is likely due to a bad Device Capability
            // structure.
            if (IoStatus->Status == STATUS_INVALID_DEVICE_STATE) {
                 TEST_TRAP();
            }
        }
    }

    KeSetEvent(&devExt->Fdo.HcPendingWakeIrpEvent, IO_NO_INCREMENT, FALSE);

    USBPORT_Core_DecIoRequestCount(hcFdo, usbIoRequest, TRUE);

    if (wakeFromS0) {
        //
        // Send Wake Completion event to the RTPM stuff
        //
        if (NT_SUCCESS(IoStatus->Status)) {
            USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                                 RtpmEventWaitWakeSuccess);
        } else {
            USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                                 RtpmEventWaitWakeFailure);
        }
    }
}


VOID
USBPORT_PowerUpPhase2(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Completes the power up sequence for a host controller.  This is where we sync
    USB 2 and USB 1.1 companions.

    This is done outside the context of the power engine.  The hist controller should
    be reset and the port power bits turned on.

Arguments:

Return Value:

    None.

--*/
{
    KIRQL irql;
    PDEVICE_EXTENSION devExt;
    HC_FDO_POWERSTATE fdoPwrState;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

    fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

    // see if we need to power on

    if (fdoPwrState == FdoPwr_D0_WaitWorker2) {

        HC_FDO_POWERSTATE nextFdoPwrState;


        nextFdoPwrState = FdoPwr_D0_WaitSyncUsb2;

        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   nextFdoPwrState,
                                   FdoPwrEv_D0_DoSetD0_2);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        // The goal here is to wait for the USB2 and its CCs to start
        // then make sure that the USB2 controller holds the shared port
        // semaphore

        USBPORT_SynchronizeUsbControllersResume(FdoDeviceObject);

        USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

        // hub is waiting for us
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                               &devExt->Fdo.RootHubReadyEv,
                               FdoPwr_WaitSx,
                               FdoPwrEv_SyncUsb2_Complete);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        // this allows the hub to continue to power up.
        DbgTrace((UsbpDebugTrace, "SET RootHub Ready\n"));
        KeSetEvent(&devExt->Fdo.RootHubReadyEv, IO_NO_INCREMENT, FALSE);

    } else {


        TEST_TRAP();
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   FdoPwr_WaitSx,
                                   FdoPwrEv_NoD0Worker);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);
    }

    //
    // Now that we have completed our power up sequence, and we are in a steady state, we can tell
    // PoFx that we are finished.
    //
    PoFxReportDevicePoweredOn(devExt->Fdo.RuntimePMContext.Handle);

    USBPORT_WMI_EVENT(1,
              USBPORT_SYSTEM_POWER_RESUME,
              (USBPORT_EVENT_TYPE_RESUME_COMPLETE,
               GUID_USB_PERFORMANCE_TRACING,
               FdoDeviceObject,
               (ULONG)devExt->Dx_State,
               "FDO %p powered and running.",
               FdoDeviceObject));

}


VOID
USBPORT_PowerUpPhase1(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP SystemPowerIrp
    )
/*++

Routine Description:

    Starts the power up sequence for a host controller.  This is where re-init
    The hardware and if necessaru schedule synchronization.


    This is done outside the context of the power engine.  The hist controller should
    be reset and the port power bits turned on.

Arguments:

Return Value:

    None.

--*/
{
    KIRQL irql;
    PDEVICE_EXTENSION devExt;
    HC_FDO_POWERSTATE fdoPwrState;
    NTSTATUS qw_nts;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    ULONG contextIdx;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

    fdoPwrState = devExt->Fdo.CurrentFdoPowerState;

    //
    // Reset the dump hiber flag
    //
    for (contextIdx = 0; contextIdx < DUMP_MAX_CONTEXT_COUNT; contextIdx++) {
        if (devExt->DumpContext[contextIdx] != NULL) {

          devExt->DumpContext[contextIdx]->Hibernate = FALSE;
        }
    }

    // see if we need to power on
    if (fdoPwrState == FdoPwr_D0_WaitWorker1) {

        HC_FDO_POWERSTATE nextFdoPwrState;

        nextFdoPwrState = FdoPwr_D0_WaitWorker2;

        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   nextFdoPwrState,
                                   FdoPwrEv_D0_DoSetD0_1);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);

        // synchronously cancel the wake irp we have
        // in PCI so we don't get a completeion while
        // we power up.

        USBPORT_DisarmHcForWake(FdoDeviceObject);

        // Start power up for the HC

        if (devExt->Fdo.PowerFlags.Off) {
            // the 'OFF/ON' case requires hardware reset
            // chirping occurs in phase 2

            USBPORT_TurnUsbControllerOn(FdoDeviceObject);

        } else {

            // this just enables the hardware
            USBPORT_RestoreUsbController(FdoDeviceObject);

            USBPORT_ResumeUsbController(FdoDeviceObject);

        }

        // queue the phase 2 worker

        qw_nts = USBPORT_QueuePowerWorker(FdoDeviceObject,
                                          NULL,
                                          PowerWorkPhase2);

        if (NT_ERROR(qw_nts)) {

           // if we cannot queue the worker we cannot finish power up
           // we mark the controller as failed and try to recover later

           USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

           USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                      SystemPowerIrp,
                                      FdoPwr_HardwareFailure,
                                      FdoPwrEv_D0_HardwareFailure);
           TEST_TRAP();

           USBPORT_RELEASE_POWER_LOCK(devExt, irql);

           KeSetEvent(&devExt->Fdo.RootHubReadyEv, IO_NO_INCREMENT, FALSE);
           KeSetEvent(&devExt->Fdo.ChirpCompleteEv, IO_NO_INCREMENT, FALSE);
        }


    } else {

        // some type of error, take no action on the hardware
        TEST_TRAP();
        USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                   0,
                                   FdoPwr_WaitSx,
                                   FdoPwrEv_NoD0Worker);

        USBPORT_RELEASE_POWER_LOCK(devExt, irql);
    }


    if (!devExt->Fdo.FastResumeEnable) {

        // If Fast Resume is enabled then the SYSTEM_POWER_STATE Irp is
        // completed by USBPORT_SystemPowerS0IrpCompletion().
        //
        // If Fast Resume is not enabled then the SYSTEM_POWER_STATE Irp is
        // completed here by this work item which was queued by
        // USBPORT_PoRequestCompletionD0().

        // Release the system power irp
        //
        usbIoRequest = USBPORT_Core_GetIoRequestFromObject(FdoDeviceObject,
                                                           SystemPowerIrp);

        PoStartNextPowerIrp(SystemPowerIrp);
        IoCompleteRequest(SystemPowerIrp, IO_NO_INCREMENT);

        USBPORT_Core_FreeIoRequest(usbIoRequest);
    }
}

VOID
USBPORT_SynchronizeUsbControllersResume(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Synchronize the USB ehci controller with its companions.

    This routines blocks all dependent controllers until their
    hardware is restored.

    This function protects the shared physical ports from simultaneous
    access by different instances of the hub driver.  This routine effectively
    serializes the resume or re-init of the EHCI controller with its companions.

    Here are the cases we must deal with:

        These cases require the devices to be chirped to the 1.1 controller:

        1. CC and USB2 are off
        2. CC is off USB2 is suspended
        3. USB2 or a CC fails resume
        4. CC is suspended USB2 is off (CC will appear to fail resume)
        5. error (like overcurrent) is encountered accessing the ports.


        These do NOT require devices to be chirped over to th CCs
        1. CC and USB2 are suspended and successfully resumed


    When a controller returns from this function all devices that should be
    connected to the root ports should show as attached.

Arguments:

    FdoDeviceObject - devobj for a usb controller ehci or companion

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    HC_FDO_POWERSTATE curPwrState;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);
    curPwrState = devExt->Fdo.CurrentFdoPowerState;

    do {
        if (!USBPORT_IS_USB20(devExt) &&
            !devExt->Fdo.Flags.IsCompanionController) {
            // not a CC or a USB 2 controller bail now.
            // we don't need to synchronize

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       NULL,
                                       curPwrState,
                                       FdoPwrEv_SyncUsb2_NotUsb2);

            USBPORT_RELEASE_POWER_LOCK(devExt, irql);
            DbgTrace((UsbpDebugTrace, " USB2 Sync Resume FDO(%p) - not USB2\n", FdoDeviceObject));

            break;
        }

        if (USBPORT_IS_USB20(devExt)) {

            //
            // USB2 Path
            //

            // This is the actual USB 2.0 controller -- we chirp the ports here
            // to route the USB 1.1 devices to the CCs.

            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       NULL,
                                       curPwrState,
                                       FdoPwrEv_SyncUsb2_DoChirp);


            USBPORT_RELEASE_POWER_LOCK(devExt, irql);

            DbgTrace((UsbpDebugTrace, " USB2 Sync Resume FDO(%p) - USB2 do chirp\n", FdoDeviceObject));

            USBPORT_SyncPowerAndChirpUsb2Ports(FdoDeviceObject);

            KeSetEvent(&devExt->Fdo.ChirpCompleteEv, IO_NO_INCREMENT, FALSE);

        } else {

            //
            // CC Path
            //

            PDEVICE_OBJECT usb2Fdo;
            PDEVICE_EXTENSION usb2DevExt;
            NTSTATUS wait_nts;

            // note that the Fdo is returned with a reference we must release
            usb2Fdo =  USBPORT_FindUSB2Controller(FdoDeviceObject);
            if (!usb2Fdo) {
                // USB 2 controller is disabled

                USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       NULL,
                                       curPwrState,
                                       FdoPwrEv_SyncUsb2_Usb2Disabled);

                USBPORT_RELEASE_POWER_LOCK(devExt, irql);

                DbgTrace((UsbpDebugTrace, " USB2 Sync Resume FDO(%p) - USB2 disabled\n", FdoDeviceObject));
                break;
            }


            GET_DEVICE_EXT(usb2DevExt, usb2Fdo);
            ASSERT_FDOEXT(usb2DevExt);
            // This is a companion -- we must wait on the ports to be chirped before
            // we proceed with resume.
            USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       usb2Fdo,
                                       curPwrState,
                                       FdoPwrEv_SyncUsb2_WaitChirp);

            USBPORT_RELEASE_POWER_LOCK(devExt, irql);


            DbgTrace((UsbpDebugTrace, " USB2 Sync Resume FDO(%p) - USB2 wait chirp\n", FdoDeviceObject));

            // The ChirpCompleteEv is part of the USBPORT ext for the CC which is why we hold a ref
            // on the devobj.
            // Ideally we should hold an IoCount ref so the the 2.0 controller doesn't uninitialize.

            // wait a reasonable amount of time for the USB 2.0 controller to signal us.
            // we timeout in case some horrible fate befell the 2.0 controller.  The hub
            // driver will handle recovery accordingly.

            wait_nts = KeWaitForSingleObject(&usb2DevExt->Fdo.ChirpCompleteEv,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            if (wait_nts == STATUS_TIMEOUT) {
                DbgTrace((UsbpDebugTrace, " USB2 Sync Resume FDO(%p) - Wait Timed Out\n", FdoDeviceObject));
                TEST_TRAP();

                USBPORT_ACQUIRE_POWER_LOCK(devExt, irql);

                curPwrState = devExt->Fdo.CurrentFdoPowerState;
                USBPORT_SetHcFdoPowerState(FdoDeviceObject,
                                       usb2Fdo,
                                       curPwrState,
                                       FdoPwrEv_SyncUsb2_WaitTimeout);

                USBPORT_RELEASE_POWER_LOCK(devExt, irql);
            }

            ObDereferenceObject(usb2Fdo);

        }


    } WHILE (0);
}


VOID
USBPORT_UsbPowerWorker(
    PDEVICE_OBJECT DeviceObject,
    PVOID Context
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PUSB_POWER_WORK powerWork = Context;

    if (powerWork->PowerWorkPhase == PowerWorkPhase1) {
        // bring controllers on line and release the S irp
        USBPORT_PowerUpPhase1(powerWork->FdoDeviceObject,
                              powerWork->SystemPowerIrp);
    } else {
        USBPORT_PowerUpPhase2(powerWork->FdoDeviceObject);
    }

    IoFreeWorkItem(powerWork->QueueItem);

    USBPORT_ASSERT(!powerWork->UsbIoRequest.FreeContext);
    powerWork->UsbIoRequest.FreeContext = FALSE; // prefix
    USBPORT_Core_DecIoRequestCount(powerWork->FdoDeviceObject, &powerWork->UsbIoRequest, TRUE);

    UsbFreePool(powerWork);
    return;
}

NTSTATUS
USBPORT_QueuePowerWorker(
    PDEVICE_OBJECT          FdoDeviceObject,
    PIRP                    SystemPowerIrp,
    USB_POWER_WORK_PHASE    PowerWorkPhase
    )
/*++

Routine Description:

    queue a high priority workitem to finish hardware power up.

Arguments:

Return Value:

    Error Status, caller must deal with it.

--*/
{
    NTSTATUS ntStatus;
    PUSB_POWER_WORK powerWork;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    // bring controllers on line and release the S irp

    if (PowerWorkPhase == PowerWorkPhase1 &&
        devExt->Fdo.InPathCount) {

        // Use the low-resources proof queuing function for
        // paging device
        USBPORT_DeferIrpProcessing(FdoDeviceObject,
                                   USBPORT_PowerUpPhase1,
                                   SystemPowerIrp);
        ntStatus = STATUS_SUCCESS;

    } else {
        ALLOC_POOL_Z(powerWork, NonPagedPoolNx, sizeof(*powerWork));

        if (powerWork != NULL) {
            powerWork->QueueItem = IoAllocateWorkItem(FdoDeviceObject);
            if (powerWork->QueueItem != NULL) {
                powerWork->FdoDeviceObject = FdoDeviceObject;
                powerWork->SystemPowerIrp = SystemPowerIrp;
                powerWork->PowerWorkPhase = PowerWorkPhase;

                USBPORT_Core_IncIoRequestCount(FdoDeviceObject, &powerWork->UsbIoRequest, powerWork, UsbIoTag_PowerWorker, FALSE);

                IoQueueWorkItem(powerWork->QueueItem,
                                USBPORT_UsbPowerWorker,
                                DelayedWorkQueue,
                                powerWork);
                ntStatus = STATUS_SUCCESS;
            } else {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                UsbFreePool(powerWork);
            }
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    return ntStatus;
}


VOID
USBPORT_SetHcFdoPowerState(
    PDEVICE_OBJECT HcFdo,
    PVOID ContextData,
    HC_FDO_POWERSTATE NewFdoPowerState,
    HC_FDO_EVENT FdoPowerEvent
    )
/*++

Description:

    Tracks power state transitions on the host controller (FDO).


Arguments:

    ContextData - optional argument used for debugging

    NewFdoPowerState - power state we transitioning to.

    FdoPowerEvent - event causing transition

--*/
{
    PHC_STATE_LOG_ENTRY fdoLog;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    devExt->Fdo.PwrLogIdx=(devExt->Fdo.PwrLogIdx+1) % NUM_STATE_LOG_ENTRIES;
    fdoLog = &devExt->Fdo.PwrLog[devExt->Fdo.PwrLogIdx];

    fdoLog->Event = FdoPowerEvent;
    fdoLog->CurrentState = devExt->Fdo.CurrentFdoPowerState;
    fdoLog->NextState = NewFdoPowerState;
    fdoLog->ContextData = ContextData;
    KeQuerySystemTime(&fdoLog->SysTime);

    devExt->Fdo.CurrentFdoPowerState = NewFdoPowerState;

}

VOID
USBPORT_CancelPendingWakeIrp(
    __inout PDEVICE_OBJECT PdoDeviceObject,
    __inout __drv_useCancelIRQL PIRP CancelIrp
    )
/*++

Routine Description:

    Handle Cancel for the root hub wake irp

Arguments:

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION rhDevExt, devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    KIRQL irql;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    // release cancel spinlock immediatly,
    // we are protected by the WAKEIRP_LOCK
    IoReleaseCancelSpinLock(CancelIrp->CancelIrql);

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);
    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(fdoDeviceObject, LOG_POWER, 'cnWW', fdoDeviceObject, CancelIrp, 0);

    ACQUIRE_WAKEIRP_LOCK(fdoDeviceObject, irql);

    USBPORT_ASSERT(rhDevExt->Pdo.PendingWaitWakeIrp == CancelIrp);
    rhDevExt->Pdo.PendingWaitWakeIrp = NULL;

    RELEASE_WAKEIRP_LOCK(fdoDeviceObject, irql);

    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(PdoDeviceObject, CancelIrp);

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject,
                        CancelIrp,
                        NULL,
                        STATUS_CANCELLED,
                        0);

    USBPORT_Core_FreeIoRequest(usbIoRequest);
}



VOID
USBPORT_TurnUsbControllerOff(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    When we say OFF we mean OFF.

    This is similar to a stop -- the mniport does not
    know the difference.  The port however does and
    does not free the miniports resources

    This function may be called multiple times ie even
    if controller is already off with no ill effects.

Arguments:

    DeviceObject - DeviceObject of the controller to turn off

Return Value:

    this is NON FAILABLE.

--*/

{
    PDEVICE_EXTENSION devExt;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (!devExt->Fdo.PowerFlags.Off) {
        LOGENTRY(FdoDeviceObject, LOG_POWER, 'Coff', 0, 0, 0);

        DbgTrace((UsbpDebugTrace, " >Turning Controller OFF\n"));

        //
        // On power down, the miniport may need to perform some operations
        // while interrupts are still enabled
        //
        // Note: This was added for Wireless USB
        //
        if (TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {

            MPf_StopControllerWithInterrupts(devExt);

        }

        // tell the DM tiner not to poll the controller
        USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
        devExt->Fdo.DmTimerFlags.SkipTimerWork = 1;
        USBPORT_RELEASE_DM_LOCK(devExt, irql);

        if (TEST_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {

            MP_DisableInterrupts(FdoDeviceObject);
            CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_STARTED);

            USBPORT_Core_ZapAllEndpoints(FdoDeviceObject);
            MPf_StopController(devExt, TRUE);
        }



        // Off overrides suspended
        devExt->Fdo.PowerFlags.Suspended = 0;
        CLEAR_FLAG(devExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);

        USBPORT_AcquireSpinLock(FdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, &irql);
        devExt->Fdo.PowerFlags.Off = 1;
        USBPORT_ReleaseSpinLock(FdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, irql);
    }
}


NTSTATUS
USBPORT_ArmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN        WakeFromS0
    )

/*++

Routine Description:

    ArmHcforWake

    Allocate and submit a 'WaitWake' Irp to the host controllers PDO
    (usually owned by PCI).  This will enable the PME event needed to
    wake the system.

    Note: We only post the wake irp if the root hub PDO is 'enabled'
    for wakeup AND the host controller supports it.

Arguments:


Return Value:

    NTSTATUS.

--*/

{
    PDEVICE_EXTENSION devExt;
    POWER_STATE powerState;
    NTSTATUS ntStatus;
    USBHC_WAKE_STATE oldWakeState;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;
    PUSBHC_WAKE_CONTEXT wakeContext;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'hWW>', 0, 0, 0);

    ntStatus = UsbAllocPool_Z(&usbIoRequest, NonPagedPoolNx, sizeof(struct _USB_IOREQUEST_CONTEXT));
    if (NT_ERROR(ntStatus) || usbIoRequest == NULL) {
        if (usbIoRequest != NULL) {
            UsbFreePool(usbIoRequest);
            usbIoRequest = NULL;
        }
        return ntStatus;
    }

    USBPORT_Core_IncIoRequestCount(FdoDeviceObject, usbIoRequest, devExt, UsbIoTag_HcWakeIrp, TRUE);

    // this check just prevents us from posting a wake irp when we
    // already have one pending, although I'm not sure how we might
    // get into this situation.
    WHILE_TRUE {
        oldWakeState = InterlockedCompareExchange((PULONG)&devExt->Fdo.HcWakeState,
                                                  HCWAKESTATE_WAITING,
                                                  HCWAKESTATE_DISARMED);

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'hWWx', oldWakeState, 0, 0);

        if (oldWakeState == HCWAKESTATE_DISARMED) {
            break;
        }

        if ((oldWakeState == HCWAKESTATE_ARMED) ||
            (oldWakeState == HCWAKESTATE_WAITING)) {
            // The device is already arming
            USBPORT_Core_DecIoRequestCount(FdoDeviceObject, usbIoRequest, TRUE);
            return STATUS_SUCCESS;
        }

        // wait for previous wake irp to finish
        USBPORT_DisarmHcForWake(FdoDeviceObject);
    }

    // current state is HCWAKESTATE_WAITING

    // wait for wake irp to finish
    KeWaitForSingleObject(&devExt->Fdo.HcPendingWakeIrpEvent,
                          Suspended, KernelMode, FALSE, NULL);


    LOGENTRY(FdoDeviceObject, LOG_POWER, 'hWWp',
            0, 0, 0);

    ntStatus = UsbAllocPool_Z(&wakeContext, NonPagedPoolNx, sizeof(struct _USBHC_WAKE_CONTEXT));

    if (wakeContext != NULL) {

        // According to the NTDDK this should be systemwake
        powerState.DeviceState = devExt->DeviceCapabilities.SystemWake;

		wakeContext->DevExt = devExt;
        wakeContext->Sig = SIG_HC_WAKE_CONTEXT;
        wakeContext->UsbIoRequest = usbIoRequest;
        wakeContext->WakeFromS0 = WakeFromS0;

        // send the wake irp to our PDO, since it is not our
        // responsibility to free the irp we don't keep track
        // of it

        ntStatus = PoRequestPowerIrp(devExt->Fdo.PhysicalDeviceObject,
                                     IRP_MN_WAIT_WAKE,
                                     powerState,
                                     USBPORT_HcWakeIrp_Po_Completion,
                                     wakeContext,
                                     &wakeContext->Irp);

        if (!NT_SUCCESS(ntStatus)) {
            ExFreePoolWithTag(wakeContext, USBPORT_TAG);
        }

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'PMD5', 0, 0, 0);
    }

    if (ntStatus != STATUS_PENDING) {

        LOGENTRY(FdoDeviceObject, LOG_POWER, 'WAKp',
            FdoDeviceObject, 0, ntStatus);

        devExt->Fdo.HcWakeState = HCWAKESTATE_DISARMED;
        KeSetEvent(&devExt->Fdo.HcPendingWakeIrpEvent, IO_NO_INCREMENT, FALSE);

        USBPORT_Core_DecIoRequestCount(FdoDeviceObject, usbIoRequest, TRUE);

    } else {

        DbgTrace((UsbpDebugTrace, ">HC enabled for wakeup (%x) \n",  ntStatus));

    }

    return ntStatus;
}


VOID
USBPORT_CheckAndArmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject
    )

/*++

Routine Description:

    Scans the bus context list and determines if any objects have
    armed the bus.

    Note that the hub driver arms the bus on behalf of the function
    drivers when appropriate.

Arguments:


Return Value:

    none.

--*/

{
    KIRQL oldIrql;
    PLIST_ENTRY busEntry = NULL;
    PBUS_CONTEXT busCon = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    ULONG arm_bus = 0;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &oldIrql);
    for(busEntry = devExt->Fdo.BusContextHead.Flink;
        busEntry != &devExt->Fdo.BusContextHead;
        busEntry = busEntry->Flink) {
        busCon = CONTAINING_RECORD(busEntry,
                                   BUS_CONTEXT,
                                   NextEntry);
        ASSERT_BUS_CONTEXT(busCon);
        if(busCon->WakeState == BusCtx_WakeUsb &&
           //
           // Shouldn't really need to check for Bc_Valid here if
           // USBPORT_BusContextEvent() were to properly take care of
           // deleting entries from the BusContextHead list when the
           // ref count goes to zero.  Fix that another day...
           //
           busCon->BcState == Bc_Valid) {
            arm_bus = 1;
            break;
        }
    }
    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, oldIrql);

    if (arm_bus) {
        // this doesn't do anything if we are already armed
        devExt->Fdo.PowerFlags.BusArmedForWake = 1;
        USBPORT_ArmHcForWake(FdoDeviceObject, FALSE);
    } else {
        devExt->Fdo.PowerFlags.BusArmedForWake = 0;
        USBPORT_DisarmHcForWake(FdoDeviceObject);
    }

}


/*
#if defined(_M_IA64) && (_MSC_FULL_VER < 140041204)
__forceinline
LONG
InterlockedOr (
    IN OUT LONG volatile *Target,
    IN LONG Set
    )
{
    LONG i;
    LONG j;

    j = *Target;
    do {
        i = j;
        j = InterlockedCompareExchange(Target,
                                       i | Set,
                                       i);

    } while (i != j);

    return j;
    }
}
#else
#define InterlockedOr _InterlockedOr
#endif
*/

VOID
USBPORT_DisarmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject
    )

/*++

Routine Description:

    DisarmForWake

    cancels and frees the Pending wake irp for  the host controller

Arguments:


Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION devExt;
    USBHC_WAKE_STATE oldWakeState;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // no longer enabled for wake

    // Go from HCWAKESTATE_WAITING to HCWAKESTATE_WAITING_CANCELLED, or
    //         HCWAKESTATE_ARMED to HCWAKESTATE_ARMING_CANCELLED, or
    // stay in HCWAKESTATE_DISARMED or HCWAKESTATE_COMPLETING
    oldWakeState = InterlockedOr( (PULONG)&devExt->Fdo.HcWakeState, 1 );
    //oldWakeState = RtlInterlockedSetBits((PULONG)&devExt->Fdo.HcWakeState, 1 );

    if (oldWakeState == HCWAKESTATE_ARMED) {

        IoCancelIrp(devExt->Fdo.HcPendingWakeIrp);

        //
        // Now that we’ve cancelled the IRP, try to give back ownership
        // to the completion routine by restoring the HCWAKESTATE_ARMED state
        //
        oldWakeState = InterlockedCompareExchange( (PULONG) &devExt->Fdo.HcWakeState,
                                                   HCWAKESTATE_ARMED,
                                                   HCWAKESTATE_ARMING_CANCELLED );

        if (oldWakeState == HCWAKESTATE_COMPLETING) {
            //
            // We didn’t give back control of IRP in time, so we own it now.
            //
            // this will cause tp PoCompletion routine to run
            IoCompleteRequest( devExt->Fdo.HcPendingWakeIrp, IO_NO_INCREMENT);
        }
    }
}

USB_MINIPORT_STATUS
USBPORT_SyncPowerAndChirpUsb2Ports(
    PDEVICE_OBJECT Usb2Fdo
    )
/*++

Routine Description:

    This routine will:

    Power all the ports on a USB2 controller.
    Power all the ports on any associated CCs.
    Chirp all the ports so that the devices are on the correct controllers.

    Note that this function does not access the devExt for the root hub PDO on
    the USB 2 controller.  This function performs a hardware chirp on the
    ehci controller port on return from Sx.

    NOTE:
    Chirp is skipped for ports in the suspend state, if an error is encountered
    on the 20 controller all CC controllers are marked 'resume failure' since
    side effects propigate to these controllers.

    This routine accesses ehci and the companions.  The EHCI controller must be in D0,
    but the CC can still be suspended.

Arguments:

    DeviceObject - Fdo for USB 2.0 HC

Return Value:

    miniport status

--*/

{
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    USHORT p;
    ULONG i;
    PDEVICE_RELATIONS devR;
    ULONG usb2Nports, power_cnt;
    ROOTHUB_DATA hubData;
    RH_PORT_STATUS portStatus;
    ULONG chirpError = 0;
    KIRQL irql;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, Usb2Fdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(USBPORT_IS_USB20(devExt));

    USBPORT_AcquireSpinLock(Usb2Fdo, &devExt->Fdo.CoreFunctionSpin, &irql);

    if (devExt->Fdo.PowerFlags.MpRestartFailed) {

        LOGENTRY(Usb2Fdo, LOG_RH, 'CC!!', 0, 0, 0);

        // don't access usb 2 regs if miniport restart fails

        USBPORT_ReleaseSpinLock(Usb2Fdo, &devExt->Fdo.CoreFunctionSpin, irql);

        return USBMP_STATUS_FAILURE;
    }

    USBPORT_ReleaseSpinLock(Usb2Fdo, &devExt->Fdo.CoreFunctionSpin, irql);

    MPRH_GetRootHubData(devExt, &hubData);
    usb2Nports = hubData.NumberOfPorts;

    LOGENTRY(Usb2Fdo, LOG_RH, 'CCpw', 0, 0, usb2Nports);

    // Power USB 2.0 ports if necessary.  We need to ensure sufficient time for ports the
    // stabilize -- but only if we have to power them.

    DbgTrace((UsbpDebugTrace, "'** CHIRP>\n"));
    DbgTrace((UsbpDebugTrace, "'** powering USB2 ports\n"));

    power_cnt = 0;
    for (p=0;
         p< usb2Nports;
         p++) {

        MPRH_GetPortStatus(devExt,
                       p+1,
                       &portStatus,
                       mpStatus);

        LOGENTRY(Usb2Fdo, LOG_PNP, 'chrC', 0,
                    p, portStatus.ul);


        if (mpStatus == USBMP_STATUS_SUCCESS) {
            KIRQL rh_irql;

            if (portStatus.PowerOn == 0) {
                USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &rh_irql);

                MPF(devExt).MINIPORT_RH_SetFeaturePortPower(
                    devExt->Fdo.MiniportDeviceData,
                    p+1);

                USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, rh_irql);

                power_cnt++;
            }

            // check for overcurrent -- some HCs(SIS) will throw an overcurrent on resume
            // its usually not peresistent. We must clear it to attempt to chirp the ports.
            //
            // The hub will generate a log entry for the resume failure. so we will know
            // something went wrong.
            if (portStatus.OverCurrentChange) {
                chirpError = 1;

                USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &rh_irql);

                MPF(devExt).MINIPORT_RH_ClearFeaturePortOvercurrentChange(
                    devExt->Fdo.MiniportDeviceData,
                    p+1);

                USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, rh_irql);
            }

        }
    }

    if (power_cnt) {
        // allow power to stabilize. the 100 ms, this is totally arbitrary.
        // there is no power-on to power-good for EHCI. We are not in the context of the power engine so
        // the time is not as significant.

        USBPORT_Wait(Usb2Fdo, 100);
    }


    if (chirpError) {
         // if an error occurs chirping we continue but mark the controller
         // and CCs as resume failed -- this will allow the hub to revert to
         // cold_start mode and recover the devices.
         devExt->Fdo.PowerFlags.HcResumeFailure = 1;
    }

    // Power all the CC ports
    power_cnt = 0;
    // get the list of CCs and reference the device objects so the don't get deleted.
    devR = USBPORT_FindCompanionControllers(Usb2Fdo,
                                            TRUE,
                                            TRUE);
    if (devR) {

        for (i=0; i<devR->Count; i++) {

            PDEVICE_OBJECT ccFdo;
            PDEVICE_EXTENSION ccDevExt;
            ULONG nPorts;
            KIRQL ccIrql;

            ccFdo = devR->Objects[i];

            GET_DEVICE_EXT(ccDevExt, ccFdo);
            ASSERT_FDOEXT(ccDevExt);

            // verify the state of the cc. if it's not powered up skip this step.
            USBPORT_AcquireSpinLock(ccFdo,
                                    &ccDevExt->Fdo.CoreFunctionSpin, &ccIrql);
            if (ccDevExt->Fdo.PowerFlags.Off ||
                ccDevExt->Fdo.PowerFlags.Suspended) {
                USBPORT_ReleaseSpinLock(ccFdo,
                                        &ccDevExt->Fdo.CoreFunctionSpin, ccIrql);
                ObDereferenceObject(ccFdo);
                continue;
            } else {
                USBPORT_ReleaseSpinLock(ccFdo,
                                        &ccDevExt->Fdo.CoreFunctionSpin, ccIrql);
            }

            MPRH_GetRootHubData(ccDevExt, &hubData);
            nPorts = hubData.NumberOfPorts;

            if (chirpError) {
                 ccDevExt->Fdo.PowerFlags.HcResumeFailure = 1;
            }

            for (p=0;
                 p<nPorts;
                 p++) {

                KIRQL rh_irql;

                MPRH_GetPortStatus(ccDevExt,
                       p+1,
                       &portStatus,
                       mpStatus);

                LOGENTRY(Usb2Fdo, LOG_PNP, 'chrB', ccFdo,
                    p, portStatus.ul);

                if (mpStatus == USBMP_STATUS_SUCCESS &&
                    portStatus.PowerOn == 0) {

                    // Power the Companion Controller port.
                    //
                    USBPORT_AcquireSpinLock(ccDevExt->HcFdoDeviceObject, &ccDevExt->Fdo.MpRootHubFunctionSpin, &rh_irql);

                    MPF(ccDevExt).MINIPORT_RH_SetFeaturePortPower(
                        ccDevExt->Fdo.MiniportDeviceData,
                        p+1);

                    USBPORT_ReleaseSpinLock(ccDevExt->HcFdoDeviceObject, &ccDevExt->Fdo.MpRootHubFunctionSpin, rh_irql);

                    power_cnt++;
                }

            }

            ObDereferenceObject(ccFdo);
        }


        UsbFreePool(devR);
    }

    if (power_cnt) {
        // CCs are now powered

        // wait power-on to power good here for the CC.  Note  we wait once for all the CCs
        // this assumes all CCs have the same power on to power good setting for the
        // root ports.
        // we use an arbitrary value here of 100ms -- this is the max value allowed by
        // the USB specification.

        // Also:

        LOGENTRY(Usb2Fdo, LOG_PNP, 'chrA', 0, 0, power_cnt);
        USBPORT_Wait(Usb2Fdo, 100);

    } else {

        // some ammount of time must elapse between powering the ports and
        // attempting to chirp.
        //
        // according to the USB 2.0 specification this would be the minimum
        // debounce interval allowed for connections to stabilize. If CC the ports
        // were already powered we allow 50ms additional time for this here.
        USBPORT_Wait(Usb2Fdo, 50);

        LOGENTRY(Usb2Fdo, LOG_PNP, 'chrp', 0, 0, power_cnt);
    }

    // chirp all the ports on the USB2 controller

    for (p=0;
         p< usb2Nports;
         p++) {

        LONG chirp;

        if (devExt->Fdo.MiniportDriver->HciVersion <
                USB_MINIPORT_HCI_VERSION_2) {
            break;
        }

        // this counter is just a debug check to make sure we don't chirp
        // from multiple locations
        chirp = InterlockedIncrement(&devExt->Fdo.InPortChirp);
        LOGENTRY(Usb2Fdo, LOG_PNP, 'chr1', 0,
                    chirp, p+1);

        USBPORT_ASSERT(chirp == 0);

        MPF(devExt).MINIPORT_Chirp_RH_Port(devExt->Fdo.MiniportDeviceData,
                                           p+1);

        InterlockedDecrement(&devExt->Fdo.InPortChirp);
    }

    DbgTrace((UsbpDebugTrace, "'** CHIRP<\n"));

    return USBMP_STATUS_SUCCESS;
}

//
// Support for Fast S4
//

#ifdef XP_COMPAT

ULONG
USBPORT_MapS3toS4(
    PDEVICE_EXTENSION FdoDevExt
    )
{
    return 0;
}

#else

/*

Non-zero indicates S3 should be treated as S4 for all devices downstream of the host controller.

*/

ULONG
USBPORT_MapS3toS4(
    PDEVICE_EXTENSION FdoDevExt
    )
{
    return 1;
}

#endif // XP_COMPAT
