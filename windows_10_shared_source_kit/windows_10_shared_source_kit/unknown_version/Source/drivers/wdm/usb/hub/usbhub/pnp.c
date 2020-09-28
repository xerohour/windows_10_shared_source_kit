/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    pnp.c

     3-10-2002

Abstract:

    Provides pnp management for usbhub.sys



Module interface:

    All hub modules are independent of each other, each module specfies
    the following routines star called in the context of these PNP or POWER
    events:

    PNP event                           Root Function
    --------------------------------------------------
    Unload-->linked to DriverUnload     UsbhDriverUnload
    Entry -->linked to DriverEntry      DriverEntry
    Start -->linked to PNP START        UsbhFdoPnp_StartDevice
    Stop  -->linked to PNP STOP         UsbhFdoPnp_StopDevice
    Alloc -->linked to AddDevice        UsbhAddDevice
    Free  -->linked to PNP REMOVE       UsbhRemoveDevice

    These routines are called from the core hub code as appropriate.  Each
    module is responisble for its' own synchronization to make debugging ref
    count and leaks easier.

    This routines are serialized and are not reentrant. In most cases the OS
    handles this for us (like Entry, Unload) but in other cases we enforce this
    rule ourselves.


    The module dispatch code manages the async start of the usb 2 controller and its companions.
    The usb2 hub must complete start with success and await notification that it can safely
    access the hardware.

Author:

    jdunn
    kiranm ( Added BugCheck Call Backs functionality)

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "pnp.tmh"
#endif

CCHAR usbfile_pnp_c[] = "pnp.c";

#define USBFILE usbfile_pnp_c


PDRIVER_OBJECT UsbhDriverObject;

HUB_GLOBALS HubG;

BOOLEAN HubInit = FALSE;

PKBUGCHECK_REASON_CALLBACK_RECORD USBHUB_BugCheckPortArray = NULL;
PKBUGCHECK_REASON_CALLBACK_RECORD USBHUB_BugCheckHubExt = NULL;

// Statically initialize the USBHUB_TRIAGE_INFO structure used by
// DBGENG.DLL
//
USBHUB_TRIAGE_INFO USBHUB_Triage_Info = {
    // Version
    USBHUB_TRIAGE_INFO_VERSION_1,
    // HUB_PORT_DATA
    sizeof(HUB_PORT_DATA),
    FIELD_OFFSET(HUB_PORT_DATA, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION_HUB, PortData),
    FIELD_OFFSET(DEVICE_EXTENSION_HUB, NumberOfPorts),
    // PORT_CHANGE_CONTEXT
    sizeof(PORT_CHANGE_CONTEXT),
    FIELD_OFFSET(PORT_CHANGE_CONTEXT, Sig),
    FIELD_OFFSET(HUB_PORT_DATA, PortChangeList),
    FIELD_OFFSET(PORT_CHANGE_CONTEXT, ChangeLink),
    // HUB_WORKITEM
    sizeof(HUB_WORKITEM),
    FIELD_OFFSET(HUB_WORKITEM, WiSig),
    FIELD_OFFSET(DEVICE_EXTENSION_HUB, WorkItemListHead),
    FIELD_OFFSET(HUB_WORKITEM, WiLink),
    // HUB_EXCEPTION_RECORD
    sizeof(HUB_EXCEPTION_RECORD),
    FIELD_OFFSET(HUB_EXCEPTION_RECORD, Sig),
    FIELD_OFFSET(DEVICE_EXTENSION_HUB, ExceptionList),
    FIELD_OFFSET(HUB_EXCEPTION_RECORD, ExrLink)
};

typedef NTSTATUS HUB_PNP_HANDLER(PDEVICE_OBJECT DeviceObject, PIRP Irp);

typedef struct _HUB_PNP_DISPATCH {

    UCHAR   MinorFunction;
    HUB_PNP_HANDLER *PnPHandler;

} HUB_PNP_DISPATCH, *PHUB_PNP_DISPATCH;

#define HUB_PNP_MAX_MN      0x20

//
// FDO PnP Handlers
//

const HUB_PNP_DISPATCH HubFdoPnPDispatch[HUB_PNP_MAX_MN] = {

    IRP_MN_START_DEVICE                  ,UsbhFdoPnp_StartDevice,
    IRP_MN_QUERY_REMOVE_DEVICE           ,UsbhFdoPnp_QueryRemove,
    IRP_MN_REMOVE_DEVICE                 ,UsbhFdoPnp_RemoveDevice,
    IRP_MN_CANCEL_REMOVE_DEVICE          ,UsbhFdoPnp_PassOnSuccess,
    IRP_MN_STOP_DEVICE                   ,UsbhFdoPnp_StopDevice,
    IRP_MN_QUERY_STOP_DEVICE             ,UsbhFdoPnp_QueryStop,
    IRP_MN_CANCEL_STOP_DEVICE            ,UsbhFdoPnp_PassOnSuccess,
    IRP_MN_QUERY_DEVICE_RELATIONS        ,UsbhFdoPnp_QueryDeviceRelations,
    IRP_MN_QUERY_INTERFACE               ,UsbhFdoPnp_DoDefault,
    IRP_MN_QUERY_CAPABILITIES            ,UsbhFdoPnp_QueryCaps,
    IRP_MN_QUERY_RESOURCES               ,UsbhFdoPnp_DoDefault,
    IRP_MN_QUERY_RESOURCE_REQUIREMENTS   ,UsbhFdoPnp_DoDefault,
    IRP_MN_QUERY_DEVICE_TEXT             ,UsbhFdoPnp_DoDefault,
    IRP_MN_FILTER_RESOURCE_REQUIREMENTS  ,UsbhFdoPnp_DoDefault,
    0x0E                                 ,UsbhFdoPnp_DoDefault,
    IRP_MN_READ_CONFIG                   ,UsbhFdoPnp_DoDefault,
    IRP_MN_WRITE_CONFIG                  ,UsbhFdoPnp_DoDefault,
    IRP_MN_EJECT                         ,UsbhFdoPnp_DoDefault,
    IRP_MN_SET_LOCK                      ,UsbhFdoPnp_DoDefault,
    IRP_MN_QUERY_ID                      ,UsbhFdoPnp_DoDefault,
    IRP_MN_QUERY_PNP_DEVICE_STATE        ,UsbhFdoPnp_QueryPnpDeviceState,
    IRP_MN_QUERY_BUS_INFORMATION         ,UsbhFdoPnp_DoDefault,
    IRP_MN_DEVICE_USAGE_NOTIFICATION     ,UsbhFdoPnp_DeviceUsageNotification,
    IRP_MN_SURPRISE_REMOVAL              ,UsbhFdoPnp_SurpriseRemove,
    IRP_MN_QUERY_LEGACY_BUS_INFORMATION  ,UsbhFdoPnp_DoDefault,
    IRP_MN_DEVICE_ENUMERATED             ,UsbhFdoPnp_DoDefault
};

//
// PDO PnP Handlers
//

const HUB_PNP_DISPATCH HubPdoPnPDispatch[HUB_PNP_MAX_MN] = {

    IRP_MN_START_DEVICE                  ,UsbhPdoPnp_StartDevice,
    IRP_MN_QUERY_REMOVE_DEVICE           ,UsbhPdoPnp_Success,
    IRP_MN_REMOVE_DEVICE                 ,UsbhPdoPnp_RemoveDevice,
    IRP_MN_CANCEL_REMOVE_DEVICE          ,UsbhPdoPnp_Success,
    IRP_MN_STOP_DEVICE                   ,UsbhPdoPnp_StopDevice,
    IRP_MN_QUERY_STOP_DEVICE             ,UsbhPdoPnp_Success,
    IRP_MN_CANCEL_STOP_DEVICE            ,UsbhPdoPnp_Success,
    IRP_MN_QUERY_DEVICE_RELATIONS        ,UsbhPdoPnp_QueryDeviceRelations,
    IRP_MN_QUERY_INTERFACE               ,UsbhPdoPnp_QueryInterface,
    IRP_MN_QUERY_CAPABILITIES            ,UsbhPdoPnp_QueryCapabilities,
    IRP_MN_QUERY_RESOURCES               ,UsbhPdoPnp_DoDefault,
    IRP_MN_QUERY_RESOURCE_REQUIREMENTS   ,UsbhPdoPnp_QueryResourceRequirements,
    IRP_MN_QUERY_DEVICE_TEXT             ,UsbhPdoPnp_QueryDeviceText,
    IRP_MN_FILTER_RESOURCE_REQUIREMENTS  ,UsbhPdoPnp_DoDefault,
    0x0E                                 ,UsbhPdoPnp_DoDefault,
    IRP_MN_READ_CONFIG                   ,UsbhPdoPnp_DoDefault,
    IRP_MN_WRITE_CONFIG                  ,UsbhPdoPnp_DoDefault,
    IRP_MN_EJECT                         ,UsbhPdoPnp_DoDefault,
    IRP_MN_SET_LOCK                      ,UsbhPdoPnp_DoDefault,
    IRP_MN_QUERY_ID                      ,UsbhPdoPnp_QueryId,
    IRP_MN_QUERY_PNP_DEVICE_STATE        ,UsbhPdoPnp_QueryPnpDeviceState,
    IRP_MN_QUERY_BUS_INFORMATION         ,UsbhPdoPnp_QueryBusInfo,
    IRP_MN_DEVICE_USAGE_NOTIFICATION     ,UsbhPdoPnp_DeviceUsageNotification,
    IRP_MN_SURPRISE_REMOVAL              ,UsbhPdoPnp_SurpriseRemoveDevice,
    IRP_MN_QUERY_LEGACY_BUS_INFORMATION  ,UsbhPdoPnp_DoDefault,
    IRP_MN_DEVICE_ENUMERATED             ,UsbhPdoPnp_DeviceEnumerated
};

/* Module Dispatch Tables */
typedef VOID HUB_ADDDEVICEINIT_MOD(PDEVICE_OBJECT, PSTATE_CONTEXT);
typedef NTSTATUS HUB_ADDDEVICE_MOD(PDEVICE_OBJECT, PSTATE_CONTEXT);
typedef VOID HUB_REMOVE_MOD(PDEVICE_OBJECT, PSTATE_CONTEXT);
typedef NTSTATUS HUB_DRIVERENTRY_MOD(PDRIVER_OBJECT, PUNICODE_STRING);
typedef VOID HUB_UNLOAD_MOD(PDRIVER_OBJECT);
typedef NTSTATUS HUB_PNP_START_MOD(PDEVICE_OBJECT, PSTATE_CONTEXT);
typedef VOID HUB_PNP_STOP_MOD(PDEVICE_OBJECT, PSTATE_CONTEXT);


typedef struct _HUB_MODULE_DISPATCH {

    HUB_ADDDEVICEINIT_MOD *AddDeviceInitModule; //AddDevice - can't fail / block
    HUB_ADDDEVICE_MOD *AddDeviceModule;         //AddDevice - can fail or block
    HUB_REMOVE_MOD *RemoveModule;               //Remove, inverse of AdDevice

    HUB_DRIVERENTRY_MOD *DriverEntryModule;     //DriverEntry
    HUB_UNLOAD_MOD *UnloadModule;               //Unload, inverse of DriverEntry

    HUB_PNP_START_MOD *PnpStartModule;
    HUB_PNP_STOP_MOD  *PnpStopModule;

} HUB_MODULE_DISPATCH, *PHUB_MODULE_DISPATCH;


#define NUM_MODULES   10

HUB_MODULE_DISPATCH HubModule[NUM_MODULES] = {

// note that order of initialization is determined by this list
// for start,entry, add it goes top to bottom
//
// for stop, unload, remove it goes bottom to top

/****************************************************
    Pnp Module Dispatch Table
*****************************************************/
//timer *********************************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {NULL,                  NULL,                   NULL,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    UsbhTimerPnpStart,      UsbhTimerPnpStop},
// reg ********************************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {NULL,                  NULL,                   NULL,
    //DriverEntry           //DriverUnload
    UsbhRegDriverEntry,     NULL,
    //PnpStart              //PnpStop
    UsbhRegPnpStart,        NULL},
//etw *********************************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {NULL,                  NULL,                   NULL,
    //DriverEntry           //DriverUnload
    UsbhEtwDriverEntry,     UsbhEtwUnload,
    //PnpStart              //PnpStop
    NULL,                   NULL},
// exception log ***********************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {UsbhExAddDeviceInit,   NULL,                   UsbhExRemove,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    NULL,                   NULL},
// hub controller **********************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {NULL,                  NULL,                   NULL,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    UsbhInitialize,         UsbhUninitialize},
// bus interface ***********************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {NULL,                  UsbhBusIfAddDevice,     UsbhBusIfRemoveDevice,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    NULL,                   NULL},
//wmi  *********************************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    //{UsbhWmiAddDevice,      UsbhWmiPnpStop,
    {NULL,                  NULL,                   NULL,
    //DriverEntry           //DriverUnload
    UsbhWmiDriverEntry,     UsbhWmiUnload,
    //PnpStart              //PnpStop
    UsbhWmiPnpStart,        UsbhWmiPnpStop},
//selective suspend ********************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {UsbhHubSSH_AddDeviceInit,NULL,                 NULL,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    UsbhHubSSH_PnpStart,    UsbhHubSSH_PnpStop},
//reset/esd ****************************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {UsbhHardResetAddDeviceInit,NULL,               NULL,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    NULL,                   NULL},
//bus enum engine **********************************
    //AddDeviceInit         //AddDevice             //RemoveDevice
    {UsbhBusAddInit,        UsbhBusAdd,             UsbhBusRemove,
    //DriverEntry           //DriverUnload
    NULL,                   NULL,
    //PnpStart              //PnpStop
    UsbhBusPnpStart,        UsbhBusPnpStop},



};

typedef enum {
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// UsbhModuleAddDeviceInit
// ----------------------------------------------------
//
// >UsbhExAddDeviceInit
// >UsbhBusIfAddDeviceInit
// >UsbhHardResetAddDeviceInit
// >UsbhBusAddInit
    UsbhModuleAddDeviceInit,    // allocates instance

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// UsbhModuleAddDevice
// ----------------------------------------------------
//
// >UsbhBusIfAddDevice
// >UsbhBusAdd
    UsbhModuleAddDevice,        // allocates instance

// ----------------------------------------------------
// UsbhModuleRemove
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// >UsbhBusRemove
// >UsbhBusIfRemoveDevice
// >UsbhExRemove
// >UsbhTimerRemove
    UsbhModuleRemove,           // frees instance

// -----------------------------------------------------
// UsbhModuleDriverEntry
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// >UsbhWmiDriverEntry
    UsbhModuleDriverEntry,      // allocates global

// -----------------------------------------------------
// UsbhModuleUnload
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// >UsbhWmiUnload
    UsbhModuleUnload,           // frees global

// -----------------------------------------------------
// UsbhModulePnpStart
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// >UsbhTimerPnpStart
// >UsbhRegPnpStart
// >UsbhInitialize
// >UsbhWmiPnpStart
// >UsbhHubSSH_PnpStart
// >UsbhBusPnpStart
    UsbhModulePnpStart,

// -----------------------------------------------------
// UsbhModulePnpStop
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// UsbhModulePnpStop
//
// >UsbhBusPnpStop
// >UsbhHubSSH_PnpStop
// >UsbhWmiPnpStop
// >UsbhUninitialize
// >UsbhTimerPnpStop
    UsbhModulePnpStop

} HUB_MODULE_FUNCTION;

#define START   TRUE
#define STOP    FALSE

NTSTATUS
UsbhCallModule(
    BOOLEAN Start,
    PHUB_MODULE_DISPATCH Dispatch,
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT HubFdo,
    HUB_MODULE_FUNCTION ModFunction,
    PUNICODE_STRING UniRegistryPath,
    PSTATE_CONTEXT StateContext
    )
 /*++

Routine Description:

    calls the dispatch function in the table if one exists.

Arguments:

Return Value:

    STATUS_SUCCESS if all modules succeeds or if no module present.

--*/
{
    NTSTATUS nts;

    nts = STATUS_SUCCESS;

    if (Dispatch) {
        switch(ModFunction) {
        //AddDeviceInit
        case UsbhModuleAddDeviceInit:
            UsbhAssert(HubFdo, Start);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->AddDeviceInitModule) {
                Dispatch->AddDeviceInitModule(HubFdo, StateContext);
            }
            break;
        //AddDevice
        case UsbhModuleAddDevice:
            UsbhAssert(HubFdo, Start);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->AddDeviceModule) {
                nts = Dispatch->AddDeviceModule(HubFdo, StateContext);
            }
            break;
        //RemoveDevice
        case UsbhModuleRemove:
            UsbhAssert(HubFdo, !Start);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->RemoveModule) {
                Dispatch->RemoveModule(HubFdo, StateContext);
            }
            break;
        //DriverEntry
        case UsbhModuleDriverEntry:
            UsbhAssert(HubFdo, Start);
            UsbhAssert(HubFdo, HubFdo == NULL);
            if (Dispatch->DriverEntryModule) {
                nts = Dispatch->DriverEntryModule(UsbhDriverObject, UniRegistryPath);
            }
            break;
        //Unload
        case UsbhModuleUnload:
            UsbhAssert(HubFdo, !Start);
            UsbhAssert(HubFdo, HubFdo == NULL);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->UnloadModule) {
                Dispatch->UnloadModule(UsbhDriverObject);
            }
            break;
        // PnP Start Device
        case UsbhModulePnpStart:
            UsbhAssert(HubFdo, Start);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->PnpStartModule) {
                nts = Dispatch->PnpStartModule(HubFdo, StateContext);
            }
            break;
        // PnP stop device
        case UsbhModulePnpStop:
            UsbhAssert(HubFdo, !Start);
            UsbhAssert(HubFdo, UniRegistryPath == NULL);
            if (Dispatch->PnpStopModule) {
                Dispatch->PnpStopModule(HubFdo, StateContext);
            }
            break;
        }
    }

    return nts;

}


NTSTATUS
UsbhModuleDispatch(
    BOOLEAN StartFlag,
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT HubFdo,
    HUB_MODULE_FUNCTION ModFunction,
    PUNICODE_STRING UniRegistryPath,
    PSTATE_CONTEXT StateContext
    )
 /*++

Routine Description:

    This routines executes all the dispatch functions for all modules for a
    given MODULE_FUNCTION

Arguments:

Return Value:

    STATUS_SUCCESS if all modules succeed or if modules don't return a status.
    Otherwise failure status of first module to fail.

--*/
{
    NTSTATUS nts, add_nts;
    ULONG i;
    ULONG m;
    PHUB_MODULE_DISPATCH disp;

    nts = STATUS_SUCCESS;

    // call modules in revers order on stop
    m = StartFlag ? 0 : NUM_MODULES-1;

    add_nts = STATUS_SUCCESS;

    for (i=0; i< NUM_MODULES; i++) {
        // next module
        UsbhAssert(HubFdo, m < NUM_MODULES);
        disp = &HubModule[m];

        nts = UsbhCallModule(StartFlag, disp,
                             DriverObject,
                             HubFdo,
                             ModFunction,
                             UniRegistryPath,
                             StateContext);


        if (ModFunction == UsbhModuleAddDevice) {
            // special case for add,
            // call all modules regardless of error, this  allows them to set initial state
            // so the process can be reversed by remove.
            // We do this because this is the first pnp state.
            if (!NT_SUCCESS(nts)) {
                // retain the last error
                add_nts = nts;
            }
        } else {
            // for errors on start we bail, since we set initail state with add each module
            // can tell if it was started or not
            if (!NT_SUCCESS(nts)) {
                // stop,remove,uload are not failable
                UsbhAssert(HubFdo, StartFlag);
                break;
            }
        }
        StartFlag ? m++ : m--;
    }

    if (ModFunction == UsbhModuleAddDevice) {
        nts = add_nts;
    }

    return nts;
}


VOID
UsbhSetFdoPnpState(
    PDEVICE_OBJECT HubFdo,
    HUB_FDO_PNPSTATE NewFdoPnpState,
    HUB_FDO_EVENT FdoPnpEvent
    )
/*++


Routine Description:


Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PG_STATE_LOG_ENTRY fdoLog;

    hubExt = FdoExt(HubFdo);

    hubExt->FdoLogIdx=(hubExt->FdoLogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
    fdoLog = &hubExt->FdoLog[hubExt->FdoLogIdx];

    fdoLog->Event = FdoPnpEvent;
    fdoLog->CurrentState = hubExt->CurrentFdoPnpState;
    fdoLog->NextState = NewFdoPnpState;

    // Log hub PnP state transitions to/from "started" state to ETW
    if (NewFdoPnpState != hubExt->CurrentFdoPnpState) {
        if (hubExt->CurrentFdoPnpState == FDO_WaitPnpStop) {
            // Hub is no longer operational, log to ETW
            UsbhEtwLogHubInformation(hubExt, &USBHUB_ETW_EVENT_HUB_REMOVE, FALSE);
        } else if (NewFdoPnpState == FDO_WaitPnpStop) {
            // Hub is starting, log to ETW
            UsbhEtwLogHubInformation(hubExt, &USBHUB_ETW_EVENT_HUB_START, TRUE);
        }
    }

    switch(NewFdoPnpState) {
    case FDO_WaitPnpAdd:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpAdd;
        break;

    case FDO_WaitPnpStart:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpStart;
        break;

    case FDO_WaitPnpUnload:
        hubExt->PnpStateHandler = NULL;
        break;

    case FDO_WaitPnpRemove:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpRemove;
        break;

    case FDO_WaitPnpStop:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpStop;
        break;

    case FDO_WaitPnpStop_CB:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpStop_CB;
        break;

    case FDO_WaitPnpRestart:
        hubExt->PnpStateHandler = Usbh_FDO_WaitPnpRestart;
        break;
    }

    hubExt->CurrentFdoPnpState = NewFdoPnpState;

}


BOOLEAN
UsbhInWinPE(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

Arguments:

    None

Return Value:

    TRUE if winpe (setup) is running

Notes:

--*/
{
#define MININT_KEY                    L"MiniNT"
    BOOLEAN             winPE;
    NTSTATUS            nts;

   
    
    nts = RtlCheckRegistryKey(RTL_REGISTRY_CONTROL, MININT_KEY);

    LOG(HubFdo, LOG_PNP, 'iWPE', nts, 0);
    if (NT_SUCCESS(nts)) {
        winPE = TRUE;
    } else {
        winPE = FALSE;
    }
#undef MININT_KEY   
    return winPE;
}


NTSTATUS
Usbh_PnpRemove(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Handles FDO_PnpRemove State

Arguments:


Return Value:

   nt status code

--*/
{
    HUB_FDO_PNPSTATE fdoPnpState;

    fdoPnpState = GET_FDO_PNPSTATE(Sc);

    // we never get remove if we fail add
    UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpUnload, FdoEvent);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

    // remove all child PDOs before final clean up,
    // skip this if we were not started
    UsbhDispatch_BusEvent(Sc->HubFdo, Sc, BE_BusDeviceRemove);

    // wait for any references to our FDO to complete
    UsbhReferenceListClose(Sc->HubFdo);

    UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModuleRemove, NULL, Sc);

    return STATUS_SUCCESS;
}


NTSTATUS
Usbh_FDO_WaitPnpAdd(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Handles FDO_WaitPnpAdd State

Arguments:


Return Value:

   nt status code

--*/
{
    HUB_FDO_PNPSTATE pnpState;
    NTSTATUS nts;

    nts = UsbhModuleDispatch(START, UsbhDriverObject, Sc->HubFdo, UsbhModuleAddDevice, NULL, Sc);

    pnpState = GET_FDO_PNPSTATE(Sc);
    UsbhAssert(Sc->HubFdo, pnpState == FDO_WaitPnpAdd);
    UsbhAssert(Sc->HubFdo, FdoEvent == Ev_ADD);

    if (NT_SUCCESS(nts)) {
        UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpStart, Ev_ADD);
        UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
    } else {
        Usbh_PnpRemove(Sc, Ev_ADD_ERROR);
    }

    return nts;
}


NTSTATUS
Usbh_FDO_WaitPnpRemove(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Handles FDO_WaitPnpRemove State

Arguments:


Return Value:

   nt status code

--*/
{
    HUB_FDO_PNPSTATE pnpState;

    pnpState = GET_FDO_PNPSTATE(Sc);


    LOG(Sc->HubFdo, LOG_PNP, 'wRMV', Sc, 1);
    LOG(Sc->HubFdo, LOG_PNP, 'wRMV', FdoEvent, 2);

    switch (FdoEvent) {
    case Ev_IRP_MN_REMOVE:
        Usbh_PnpRemove(Sc, FdoEvent);
        break;

    case Ev_IRP_MN_S_REMOVE:
        UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpRemove, FdoEvent);
        UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
        break;

    default:
        UsbhTrapFatal(Sc->HubFdo,Sc);
    }

    return STATUS_SUCCESS;
}


NTSTATUS
Usbh_FDO_WaitPnpStart(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Handles FDO_WaitPnpStart State

Arguments:


Return Value:

   nt status code

--*/
{
    //HUB_FDO_PNPSTATE pnpState;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    LOG(Sc->HubFdo, LOG_PNP, 'wSRT', Sc, 1);
    LOG(Sc->HubFdo, LOG_PNP, 'wSRT', FdoEvent, 2);

    switch (FdoEvent) {

    case  Ev_IRP_MN_REMOVE:
        // this is a start error that occurs for some other reason unknown
        // to us.
        return Usbh_PnpRemove(Sc, Ev_IRP_MN_REMOVE);

    case  Ev_IRP_MN_START:
        break;

    default:
        UsbhAssertFailure(Sc->HubFdo, Sc);
    }

    do {

        BOOLEAN winpe;

        winpe = UsbhInWinPE(Sc->HubFdo);

        hubExt->FdoFlags.WinPE = winpe ? 1 : 0;
        
        INIT_EVENT_NOT_SIGNALED(&hubExt->Evnt_FdoAsyncStart);
        // this event signalled only on a successful start when pnp sends us
        // a query caps irp
        hubExt->FdoFlags.DeviceCaps = 0;
        RESET_EVENT(&hubExt->PnpQcapsComplete);

        nts = UsbhModuleDispatch(START, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStart, NULL, Sc);

        if (NT_ERROR(nts)) {

            UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);

            UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpRemove, Ev_START_ERROR);
            break;
        }

        // choose sync or async start based on type of hub

        if (hubExt->FdoFlags.HubIsRoot && !winpe) {
            UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpStop_CB, FdoEvent);
            // do the sync part of async start
            nts = UsbhHubStart(Sc, Hub_Async_Start);
        } else {
            UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpStop, FdoEvent);
            // do the sync start
            nts = UsbhHubStart(Sc, Hub_Sync_Start);
        }

        if (NT_ERROR(nts)) {

            UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);

            UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpRemove, Ev_START_ERROR);
            break;
        }

    } WHILE (0);

    LOG(Sc->HubFdo, LOG_PNP, 'wSRT', nts, 3);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
    return nts;
}


NTSTATUS
Usbh_FDO_WaitPnpRestart(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Handles FDO_WaitPnpRestart State

    We are effectively still 'started' in this case, similar to WaitPnpStop,
    the hub structures have not been freed -- only the hub hardware needs
    to re-initialize.

Arguments:


Return Value:

   nt status code

--*/
{
    //HUB_FDO_PNPSTATE pnpState;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    LOG(Sc->HubFdo, LOG_PNP, 'wRST', Sc, 1);
    LOG(Sc->HubFdo, LOG_PNP, 'wRST', FdoEvent, 2);

    switch (FdoEvent) {

    case  Ev_IRP_MN_S_REMOVE:
        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);

        UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpRemove, Ev_IRP_MN_S_REMOVE);
        UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

        return STATUS_SUCCESS;

    case  Ev_IRP_MN_REMOVE:
        // this is a start error that occurs for some other reason unknown
        // to us.
        TEST_TRAP();
        return Usbh_PnpRemove(Sc, Ev_IRP_MN_REMOVE);

    case  Ev_IRP_MN_START:
        UsbhSshEnabled(Sc->HubFdo, Sc);
        UsbhSshEnableDisable(Sc->HubFdo, hubExt->SSH_EnabledStatus);
        break;

    default:
        UsbhTrapFatal(Sc->HubFdo,Sc);
    }

    // this event signalled only on a successful start when pnp sends us
    // a query caps irp
    hubExt->FdoFlags.DeviceCaps = 0;
    RESET_EVENT(&hubExt->PnpQcapsComplete);

    UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpStop, Ev_IRP_MN_START);

    nts = UsbhHubStart(Sc, Hub_Restart);

    if (NT_ERROR(nts)) {

        // test the fail case
        TEST_TRAP();
        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);

        UsbhSetFdoPnpState(Sc->HubFdo, FDO_WaitPnpRemove, Ev_START_ERROR);
    }

    LOG(Sc->HubFdo, LOG_PNP, 'wRST', nts, 3);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
    return nts;
}


NTSTATUS
Usbh_FDO_WaitPnpStop(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Started witing for stop or remove

Arguments:

Return Value:

   nt status code

--*/
{
    HUB_FDO_PNPSTATE nextState;
    NTSTATUS c_nts;
    ULONG ps = 0;
    
    LOG(Sc->HubFdo, LOG_PNP, 'wSTP', Sc, FdoEvent);

    switch (FdoEvent) {

    case Ev_IRP_MN_REMOVE:

        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);
        return Usbh_PnpRemove(Sc, Ev_IRP_MN_REMOVE);

    case Ev_IRP_MN_S_REMOVE:

        c_nts = UsbhFdoCheckUpstreamConnectionState(Sc->HubFdo, &ps);            
        // wake up from SSH in case this is a soft-remove 
        if (NT_SUCCESS(c_nts)) {
            Usbh_SSH_Event(Sc->HubFdo, SSH_Event_ResumeHub, Sc);
        }            

        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);
        nextState = FDO_WaitPnpRemove;

        if (NT_SUCCESS(c_nts)) {
            // this detects the case where the surprise remove is not a surprise remove -- ie the 
            // hardware is still present.  This is treated the same as a stop, the device handles 
            // must be freed so that the port driver can unload. This must happen now since we cannot 
            // gurantee the order of the removes -- the hub may remove after the port.

            // This is seen in cases where we encounter 'soft' surprise remove such as pnpdtest.
           
            Usbh_BusPnpFreeResources_PdoEvent(Sc->HubFdo, Sc);
        }            
        
        break;

    case Ev_IRP_MN_STOP:

        // this will allow any reset in progress to finish.
        UsbhDisableHardReset(Sc->HubFdo, Sc);

        Usbh_SSH_Event(Sc->HubFdo, SSH_Event_ResumeHub, Sc);
        
        // disable hub selective susend until restart
        UsbhSshDisabled(Sc->HubFdo, Sc);
         
        // suspend the enum engine for all ports
        UsbhSyncBusPause(Sc->HubFdo, Sc, PauseReason_PnPStop);
        
        Usbh_BusPnpFreeResources_PdoEvent(Sc->HubFdo, Sc);
        nextState = FDO_WaitPnpRestart;
        break;

    case Ev_IRP_MN_START:        
        // double start 
        nextState = FDO_WaitPnpStop;
        break;

    default:
        UsbhTrapFatal(Sc->HubFdo,Sc);
    }

//    pnpState = UsbhAcquireFdoPnpLock(Sc->HubFdo, Sc, FdoEvent, 0);
    UsbhSetFdoPnpState(Sc->HubFdo, nextState, FdoEvent);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

    return STATUS_SUCCESS;
}


NTSTATUS
Usbh_FDO_WaitPnpStop_CB(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

Arguments:


Return Value:

   nt status code

--*/
{
    HUB_FDO_PNPSTATE nextState;
    NTSTATUS nts;

    LOG(Sc->HubFdo, LOG_PNP, 'wSCB', Sc, FdoEvent);

    switch (FdoEvent) {
    case Ev_RH_CALLBACK:
        nts = UsbhAsyncStartComplete(Sc);
        nextState = FDO_WaitPnpStop;
        if (NT_ERROR(nts)) {

            UsbhSetFdoPnpState(Sc->HubFdo, nextState, FdoEvent);
            UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

            UsbhNotifyPnpOfFailure_Action(Sc->HubFdo, nts);
            return nts;
        }
        break;

    case Ev_IRP_MN_REMOVE:
        UsbhAsyncStop(Sc, FdoEvent);
        
        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);
        return Usbh_PnpRemove(Sc, Ev_IRP_MN_REMOVE);

    case Ev_IRP_MN_S_REMOVE:
        UsbhAsyncStop(Sc, FdoEvent);
        //test #119
        UsbhModuleDispatch(STOP, UsbhDriverObject, Sc->HubFdo, UsbhModulePnpStop, NULL, Sc);
        nextState = FDO_WaitPnpRemove;
        break;

    case Ev_IRP_MN_STOP:
        UsbhAsyncStop(Sc, FdoEvent);
        TEST_TRAP();
        nextState = FDO_WaitPnpRestart;
        break;

    default:
        UsbhTrapFatal(Sc->HubFdo,Sc);
    }

    UsbhSetFdoPnpState(Sc->HubFdo, nextState, FdoEvent);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

    return STATUS_SUCCESS;
}


NTSTATUS
Usbh_FDO_Pnp_State(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Dispatches a pnp event in the pnp context.

Arguments:


Return Value:

   nt status code

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(Sc->HubFdo);

    LOG(Sc->HubFdo, LOG_PNP, 'STE>', hubExt->PnpStateHandler, FdoEvent);

    UsbhAcquireFdoPnpLock(Sc->HubFdo, Sc, FdoEvent, 0, 0);

    // track the Pnp thread calling us for triage
    hubExt->PnpThread = Sc->ScThread;

    nts = hubExt->PnpStateHandler(Sc, FdoEvent);

    hubExt->PnpThread = NULL;

    return nts;
}


ULONG
UsbhInitRetryTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    UsbhInitRetryTimer, UsbHubhInitTimeoutTimer, UsbhInitCallback

    These functions all deal with the async initialization of the
    USB 2.0 controller and its companions.

Arguments:

Irql: DISPATCH

Return Value:

    nonzero = retry

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG retry = 0;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Parameter1 == SIG_START_WORKER);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    // timeout queues the worker
    retry = UsbhQueueDelayedWorkItem(HubFdo,
                                      NULL, // auto retry,
                                      UsbhInitCallbackWorker,
                                      Context,
                                      0,
                                      SIG_START_WORKER);

    return retry;
}


ULONG
UsbHubhInitTimeoutTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:



Arguments:

Irql: DISPATCH

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Parameter1 == SIG_START_WORKER);
    sc = Context;
    ASSERT_HUB_CONTEXT(sc);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    LOG(HubFdo, LOG_PNP, 'cbTO', Context, PortNumber);
    UsbhKdPrint((1, "<Root Hub Init Timeout HUBFDO(%p)>\n", HubFdo));

    Usbh__TestPoint__Ulong(HubFdo, TstPt_AsyncStartTimeout, STATUS_SUCCESS, PortNumber);

    nts = Usbh_HubRootHubInitNotification(HubFdo, NULL, NULL);

    if (nts == STATUS_CANCELLED) {
        // callback never ran,
        // timeout queues the worker instead
        UsbhQueueDelayedWorkItem(HubFdo,
                                  hubExt->HubInitRetryTimer,
                                  UsbhInitCallbackWorker,
                                  Context,
                                  0,
                                  SIG_START_WORKER);
    }

    // good one time, no retry
    return 0;
}


VOID
UsbhInitCallback(
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Called when it is OK to enumerate devices on the root hub. More specifically it means our port
    status registers are valid and stable.

Arguments:

Irql: DISPATCH

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_OBJECT hubFdo;

    hubFdo = StateContext->HubFdo;
    hubExt = FdoExt(hubFdo);
    ASSERT_HUB_CONTEXT(StateContext);

    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));
    LOG(hubFdo, LOG_PNP, 'iCB1', hubFdo, 0);
    UsbhKdPrint((1, "<Root Hub Init Callback HUBFDO(%p)>\n", hubFdo));

    // should not be signalled
    UsbhAssert(hubFdo, !UsbhEventSignaled(&hubExt->Evnt_FdoAsyncStart));

    UsbhQueueDelayedWorkItem(hubFdo,
                              hubExt->HubInitRetryTimer,
                              UsbhInitCallbackWorker,
                              StateContext,
                              0,
                              SIG_START_WORKER);
}

ULONG long_callback = 0;

VOID
UsbhInitCallbackWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Root hub init callback worker routine.   this is the workitem callback that allows
    the hub to begin enumerating devices.

    The callback indicates that it is now valid for the hub driver to access the port
    registers.  In the case of root hubs this is delayed until all companion controllers
    have completed initialization.

    To make things simpler for the port driver the callback can be invoked at any IRQL.

    note: that this routine could be invoked in any FdoPnp_State and the hub may transition
    to any state while this routine is running.

Arguments:

    PortNumber - not used

    Context - callback context

Irql: PASSIVE

Return Value:

    none

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();
    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace,"%!FUNC! >\n"));

    UsbhAssert(HubFdo, hubExt->FdoFlags.HubIsRoot);
    // we only care if it is valid, we hold no locks yet
    ASSERT_HUB_CONTEXT(Sc);

    LOG(HubFdo, LOG_PNP, 'iCBw', HubFdo, PortNumber);

#if DBG 
    // used to test fix for 1674024
    if (long_callback) {    
        NT_ASSERTMSG("Test Trap", FALSE); 
        UsbhWait(HubFdo, long_callback);
    }
#endif 

    UsbhDisableTimerObject(HubFdo, hubExt->HubInitTimeoutTimer);

    Usbh_FDO_Pnp_State(Sc, Ev_RH_CALLBACK);
}


NTSTATUS
UsbhHubStart(
    PSTATE_CONTEXT Sc,
    HUB_START_MODE Mode
    )
/*++

Routine Description:

    Perform the async start of the hub FDO.

   // **
   // we need to deal with the USB 2.0 bus boot problems here.
   // the hub registers a callback when started and this
   // call back is what posts the initial interrupt transfer
   // This allows the port driver to control when the hubs actually
   // begin to enumerate devices.
   // **

   // pnpQcaps->not signaled
   // we need to know when pnp has completed start, the only way to tell is to
   // look at the next irp sent which is Query Caps.

Arguments:


Return Value:

   nt status code

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;
    PSTATE_CONTEXT sc_async;

    hubExt = FdoExt(Sc->HubFdo);

    switch (Mode) {
    case Hub_Async_Start:

        // issue the async start in a different context
        sc_async = PNP_ASYNC_CONTEXT(Sc->HubFdo);

        // transition to the 'waiting for callback' state
        // this schedules the callback

        LOG(sc_async->HubFdo, LOG_PNP, 'ayST', 0, sc_async);

        do {
            // reference the hub for the callback, tag with the async context structure

            nts = IoAcquireRemoveLock(&hubExt->RemoveLock, sc_async);
            //nts = UsbhReferenceListAdd(Sc->HubFdo, Sc, hASY);
            if (NT_ERROR(nts)) {
                break;
            }

           // this only fails if we cannot queue an async request so nothing is pending
            LOG(sc_async->HubFdo, LOG_HUB, 'eTMR', hubExt->HubInitTimeoutTimer, 0);

            UsbhEnableTimerObject(sc_async->HubFdo,
                              hubExt->HubInitTimeoutTimer,
                              1000, //one sec timeout
                              0,
                              sc_async,
                              SIG_START_WORKER,
                              0);

            nts = Usbh_HubRootHubInitNotification(sc_async->HubFdo, sc_async, UsbhInitCallback);
            LOG(sc_async->HubFdo, LOG_HUB, 'rhin', nts, 0);

            if (NT_ERROR(nts) && nts != STATUS_CANCELLED) {

                // something went wrong and we were unable to register the cb
                LOG(sc_async->HubFdo, LOG_HUB, 'xTMR', hubExt->HubInitTimeoutTimer, 0);

                UsbhDisableTimerObject(sc_async->HubFdo, hubExt->HubInitTimeoutTimer);

                DbgTrace((UsbhDebugTrace,"%!FUNC! register callback failed %!STATUS!\n", nts));

                IoReleaseRemoveLock(&hubExt->RemoveLock, sc_async);
                break;
            }

        } WHILE (0);

        break;

    case Hub_Sync_Start:

        // for non root hubs perform a synchronous start

        LOG(Sc->HubFdo, LOG_PNP, 'syST', 0, 0);

        nts = UsbhFinishStart(Sc->HubFdo, Sc);
        if (NT_SUCCESS(nts)) {
            UsbhDispatch_BusEvent(Sc->HubFdo, Sc, BE_BusPnpStart);
        } else {
            UsbhDispatch_BusEvent(Sc->HubFdo, Sc, BE_BusPnpStartError);
        }

        break;

    case Hub_Restart:

        // in the restart case we want to preserve the original PDOs.
        LOG(Sc->HubFdo, LOG_PNP, 'reST', 0, 0);

        // It turns out that this is the exact same thing as a a cold start for pnp
        // so we use the same function.
        // We also use this function in the 'ESD path'

        // the key difference here is that the device is preserved in the 'unconfigured' state
        // this means that we must reconfigure the device ourselves -- the device handle is not
        // preserved.

        nts = UsbhReinitialize(Sc->HubFdo, Sc);
        if (NT_SUCCESS(nts)) {
            nts = UsbhFdoSetD0Cold(Sc->HubFdo, Sc, FALSE);
        }

        break;
        
    default:
        nts = STATUS_UNSUCCESSFUL;
        UsbhAssertFailure(Sc->HubFdo, Sc);

    }

    return nts;
}


NTSTATUS
UsbhAsyncStartComplete(
    PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Complete the async start as a 'simulated' pnp event

Arguments:


Return Value:

   nt status code

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    HUB_BUS_STATE bs;

    UsbhAssertPassive();
    hubExt = FdoExt(Sc->HubFdo);

    UsbhAssert(Sc->HubFdo, hubExt->FdoFlags.HubIsRoot);
    // should be locked
    UsbhAssert(Sc->HubFdo, hubExt->CurrentFdoPnpContext == PNP_ASYNC_CONTEXT(Sc->HubFdo));

    LOG(Sc->HubFdo, LOG_PNP, 'asyC', Sc->HubFdo, Sc);

    nts = UsbhFinishStart(Sc->HubFdo, Sc);
    if (NT_SUCCESS(nts)) {
        bs = UsbhDispatch_BusEvent(Sc->HubFdo, Sc, BE_BusPnpStart);
    } else {
        bs = UsbhDispatch_BusEvent(Sc->HubFdo, Sc, BE_BusPnpStartError);
    }

    KeQuerySystemTime(&hubExt->Stat_AsyncPnpStartAt);
    SET_EVENT(&hubExt->Evnt_FdoAsyncStart);

//    UsbhReferenceListRemove(Sc->HubFdo, Sc);
    IoReleaseRemoveLock(&hubExt->RemoveLock, Sc);

    return nts;
}


VOID
UsbhAsyncStop(
    PSTATE_CONTEXT Sc,
    HUB_FDO_EVENT FdoEvent
    )
/*++

Routine Description:

    Cancel the async callback if possible otherwise wait for the timeout

Arguments:


Return Value:

   nt status code

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(Sc->HubFdo);

    LOG(Sc->HubFdo, LOG_PNP, 'BsCB', 0, 0);

    // if we are in Paused we may have never have been transitioned to run
    // so we need to wait for any pending callback.

    // if we have an init timeout terminate it now
    UsbhDisableTimerObject(Sc->HubFdo, hubExt->HubInitTimeoutTimer);

    // deregister the callback
    nts = Usbh_HubRootHubInitNotification(Sc->HubFdo, NULL, NULL);
    if (nts == STATUS_CANCELLED) {

        PSTATE_CONTEXT sc_async;

        sc_async = PNP_ASYNC_CONTEXT(Sc->HubFdo);
        // callback never ran
        hubExt->FdoFlags.HubAsyncInitCanceled = 1;
        SET_EVENT(&hubExt->Evnt_FdoAsyncStart);

        IoReleaseRemoveLock(&hubExt->RemoveLock, sc_async);
    }

    // this is where we handle the case where the callback is called in the context
    // of a remove (i.e. during remove).
    //Test #119

#define INIT_CALLBACK_TIMEOUT     5000
    // wait for any callback notifications to complete since we killed our timeout we were depending on.
    // Note that this will only work if the callback has already been initiated.
    LOG(Sc->HubFdo, LOG_PNP, 'deCB', 0, 0);

//    WAIT_EVENT_TIMEOUT(&hubExt->Evnt_FdoAsyncStart, MILLISECONDS_TO_100_NS_UNITS(INIT_CALLBACK_TIMEOUT), nts);
    nts = UsbhWaitEventWithTimeout(Sc->HubFdo, &hubExt->Evnt_FdoAsyncStart, INIT_CALLBACK_TIMEOUT, 'cbkW');    

    LOG(Sc->HubFdo, LOG_PNP, 'cb01', nts, INIT_CALLBACK_TIMEOUT);
    if (nts == STATUS_TIMEOUT) {

        LOG(Sc->HubFdo, LOG_PNP, 'cb02', 0, INIT_CALLBACK_TIMEOUT);
        // wait timed out, see if the callback is currently queued in the hub driver
        
        if (hubExt->FdoFlags.HubAsyncInitCanceled == 0) {
            // we timed out beacuse we hold the pnp lock and the callback function is trying to acquire it.
            // we need to release both the stop and the callback
            LOG(Sc->HubFdo, LOG_PNP, 'cb03', 0, INIT_CALLBACK_TIMEOUT);
            
            UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

            // allow callback to exit
            WAIT_EVENT(&hubExt->Evnt_FdoAsyncStart);
            
            UsbhAcquireFdoPnpLock(Sc->HubFdo, Sc, Ev_CB_TIMEOUT, initcallback_Tag, 1);

            if (Ev_IRP_MN_STOP == FdoEvent) {
                // this will allow any reset in progress to finish.
                UsbhDisableHardReset(Sc->HubFdo, Sc);

                Usbh_SSH_Event(Sc->HubFdo, SSH_Event_ResumeHub, Sc);
        
                // disable hub selective susend until restart
                UsbhSshDisabled(Sc->HubFdo, Sc);
                 
                // suspend the enum engine for all ports
                UsbhSyncBusPause(Sc->HubFdo, Sc, PauseReason_AsyncStop);
        
                Usbh_BusPnpFreeResources_PdoEvent(Sc->HubFdo, Sc);
            }
        } else {           
            // must be really stuck or usbport has a problem, this is fatal

            LOG(Sc->HubFdo, LOG_PNP, 'cb04', 0, INIT_CALLBACK_TIMEOUT);
            
            UsbhKdPrint((0, "Timeout: Hub InitCallback stuck in port driver!\n"));
/*

        since this will case will bugcheck 0x7e in retail with this stack we will generate our own bugcheck 
        to make this easier to diagnose on OCA in the future.
        
                usbhub!Usbh_FDO_Pnp_State
                usbhub!UsbhInitCallbackWorker+0x9a [h:\nt6w\drivers\wdm\usb\hub\usbhub\pnp.c @ 1314]
*/
         
            UsbhTrapFatal(Sc->HubFdo,Sc->HubFdo);
        }
        
    }
#undef INIT_CALLBACK_TIMEOUT

    return;
}


VOID
Usbh_PnpUnload(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Transition associated with entry into the pnpUnload state.

Arguments:


Return Value:

    none

--*/
{
    UsbhModuleDispatch(STOP, DriverObject, NULL, UsbhModuleUnload,  NULL, NULL);

}


NTSTATUS
Usbh_PnpDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    )
/*--

Routine Description:

    Transition associated with entry into the driverEntry state.

Arguments:


Return Value:

    returns status of add

--*/
{
    NTSTATUS nts;

    nts = UsbhModuleDispatch(START, DriverObject, NULL, UsbhModuleDriverEntry,
        UniRegistryPath, NULL);

    return nts;
}


NTSTATUS
UsbhFdoPnp(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch routine for PnP Irps to Fdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG idx;
    UCHAR pnp_func;
    HGSL lock;
        
    hubExt = FdoExt(HubFdo);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pnp_func = ioStack->MinorFunction;
    hubExt->CurrentPnpFunc = (ULONG) pnp_func;

    idx = InterlockedIncrement((PLONG)&hubExt->PnpFuncHistoryIdx);
    idx = idx % HUB_PP_HISTORY_LEN;
    hubExt->PnpFuncHistory[idx] = (UCHAR) pnp_func;

    lock.Locked = 0; 
    LockGSL(&lock);
      
    if (hubExt->myDO_DEVICE_INITIALIZING && 
        ioStack->MinorFunction < HUB_PNP_MAX_MN) {
        unLockGSL(&lock);
        
        nts = HubFdoPnPDispatch[ioStack->MinorFunction].PnPHandler(
                HubFdo,
                Irp);
    } else {
        
        unLockGSL(&lock);
        // this is where we handle the famous IRP_MN_BOGUS irp
        nts = UsbhFdoPnp_DoDefault_HubNotInitialized(HubFdo, Irp);
    }

    return nts;
}


NTSTATUS
UsbhPdoPnp(
    PDEVICE_OBJECT HubPdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Dispatch routine for PnP Irps to Pdo.

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG idx;
    UCHAR pnp_func;

    pdoExt = PdoExt(HubPdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);

    pnp_func = ioStack->MinorFunction;
    pdoExt->CurrentPnpFunc = (ULONG) pnp_func;

    idx = InterlockedIncrement((PLONG)&pdoExt->PnpFuncHistoryIdx);
    idx = idx % HUB_PP_HISTORY_LEN;
    pdoExt->PnpFuncHistory[idx] = pnp_func;

    DbgTrace((UsbhDebugTrace,"UsbhPdoPnp %!pnpmn! \n",ioStack->MinorFunction));

//    LOG(pdoExt->HubFdo, LOG_PNP, 'pPNP', Irp, pdoExt->HubFdo);

    if (ioStack->MinorFunction < HUB_PNP_MAX_MN) {
        nts = HubPdoPnPDispatch[ioStack->MinorFunction].PnPHandler(
                HubPdo,
                Irp);
    } else {
        nts = UsbhPdoPnp_DoDefault(HubPdo, Irp);
    }

    return nts;
}



NTSTATUS
UsbhFdoPnp_PassOnSuccess(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Pass on the irp with a status code of success

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    LOG(HubFdo, LOG_PNP, 'fsuc', 0, Irp);
    hubExt = FdoExt(HubFdo);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhFdoPnp_QueryStop(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    This hooks the 'query' PNP path and gives us an opportunity to wake out of the selective suspend state.
    
    Pass on the irp with a status code of success

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts, c_nts;
    PSTATE_CONTEXT sc;
    ULONG ps = 0;
    
   
    hubExt = FdoExt(HubFdo);

    sc = PNP_CONTEXT(HubFdo);
    LOG(HubFdo, LOG_PNP, 'qyst', sc, Irp);
    // wake from SSH and disable the ESD functionality 
    
    // this will allow any reset in progress to finish.
    UsbhDisableHardReset(sc->HubFdo, sc);

    c_nts = UsbhFdoCheckUpstreamConnectionState(sc->HubFdo, &ps);  
    // wake up from SSH in case this is a soft-remove

    if (NT_SUCCESS(c_nts)) {
        Usbh_SSH_Event(sc->HubFdo, SSH_Event_ResumeHub, sc);
    }    
    
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhFdoPnp_QueryRemove(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    This hooks the 'query' PNP path and gives us an opportunity to wake out of the selective suspend state.
    
    Pass on the irp with a status code of success

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts, c_nts;
    PSTATE_CONTEXT sc;
    ULONG ps = 0;

   
    hubExt = FdoExt(HubFdo);

    sc = PNP_CONTEXT(HubFdo);
    // wake from SSH and disable the ESD functionality 
    LOG(HubFdo, LOG_PNP, 'qyst', sc, Irp);
    
    // this will allow any reset in progress to finish.
    UsbhDisableHardReset(sc->HubFdo, sc);

    c_nts = UsbhFdoCheckUpstreamConnectionState(sc->HubFdo, &ps);  
    // wake up from SSH in case this is a soft-remove

    if (NT_SUCCESS(c_nts)) {
        Usbh_SSH_Event(sc->HubFdo, SSH_Event_ResumeHub, sc);
    }    
    
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


#if 0
NTSTATUS
UsbhFdoPnp_CancelStop(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Releases the busy lock associated with a stop
    
    Pass on the irp with a status code of success

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PSTATE_CONTEXT sc;

    sc = PNP_CONTEXT(HubFdo);
    LOG(HubFdo, LOG_PNP, 'cast', sc, Irp);
    hubExt = FdoExt(HubFdo);

    // wake from SSP
    UsbhDecHubBusy(HubFdo, sc, hubExt->QueryStopSspHandle);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}
#endif


NTSTATUS
UsbhFdoPnp_DoDefault(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Default handler for FDO targeted PnP Irps.

    Default behavior for an 'unhandled' PnP irp is to chain it on to the
    next devobj in the stack.

    the status code in the IRP remians unchnaged.

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    hubExt = FdoExt(HubFdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    LOG(HubFdo, LOG_PNP, 'fDef', ioStack->MinorFunction, Irp);

    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhFdoPnp_DoDefault_HubNotInitialized(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

   Same as UsbhFdoPnp_DoDefault but logging is bypassed. This routine is used if the pnp
   irp comes in before we are completly initialized.

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    hubExt = FdoExt(HubFdo);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
  
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}

NTSTATUS
UsbhFdoPnp_DeviceUsageNotification(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Pass on the irp with a status code of success

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    DEVICE_USAGE_NOTIFICATION_TYPE dunType;

    LOG(HubFdo, LOG_PNP, 'fDUN', 0, Irp);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_DEVICE_USAGE_NOTIFICATION);
    dunType = ioStack->Parameters.UsageNotification.Type;

    hubExt = FdoExt(HubFdo);

    //
    // Pass IRP_MN_DEVICE_USAGE_NOTIFICATION Irp down the stack first
    //
    if (IoForwardIrpSynchronously(hubExt->TopOfStackDeviceObject, Irp)) {
        nts = Irp->IoStatus.Status;
    } else {
        nts = STATUS_UNSUCCESSFUL;
    }
    
    //
    // Propogate the flags from the lower device object
    //
    if (DunTypeSpecialFile(dunType)) {

        if (NT_SUCCESS(nts)) {
            if (!(hubExt->TopOfStackDeviceObject->Flags & DO_POWER_PAGABLE)) {
                HubFdo->Flags &= ~DO_POWER_PAGABLE;        
            } else {
                HubFdo->Flags |= DO_POWER_PAGABLE;
            }
        }
    }

    //
    // We don't provide forward progress and surprise removal support for WinPE
    //
    if (!NT_SUCCESS(RtlCheckRegistryKey(RTL_REGISTRY_CONTROL, WINPE_KEY)) &&
        ((NT_SUCCESS(nts) && DunTypePaging(dunType) &&
         ioStack->Parameters.UsageNotification.InPath) ||
         DunTypeOnBootPath(dunType))) {
    
        PIRP resetIrp = NULL;
        NTSTATUS nts2;
        
        // Failure in this path is not fatal but we need to log an event
        
        do {
            
            // Allocate surprise removal resources
            resetIrp = IoAllocateIrp((CCHAR)(hubExt->TopOfStackDeviceObject->StackSize),
                        FALSE);            
            if (InterlockedCompareExchangePointer(&hubExt->ResetNotificationRequestIrp,
                                                  resetIrp, 
                                                  NULL)) {                                                  
                // reset irp is already registered
                break;
            } else if (!resetIrp) {
                break;
            }
            
            hubExt->IsBootDevice = TRUE;
            HubG.SQMSurpriseRemovalUnplug = FALSE;

            ioStack = IoGetNextIrpStackLocation(resetIrp);

            ioStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

            ioStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST;

            //
            // Buffer Lengths go in the io stack parameters
            //
            ioStack->Parameters.DeviceIoControl.InputBufferLength = 0;
            ioStack->Parameters.DeviceIoControl.OutputBufferLength = 0;

            UsbhSetCompletionRoutine(HubFdo,
                                     resetIrp, 
                                     UsbhResetNotificationIrpCompletion, 
                                     HubFdo, 
                                     TRUE, 
                                     TRUE, 
                                     TRUE);

            nts2 = IoCallDriver(hubExt->TopOfStackDeviceObject, resetIrp);

            UsbhAssert(HubFdo, nts2 == STATUS_PENDING);

            resetIrp = NULL;
            
        } WHILE (0);

        if (resetIrp) {
            IoFreeIrp(resetIrp);
        }
    }
    
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhFdoPnp_QueryPnpDeviceState(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handles IRP_MN_QUERY_PNP_DEVICE_STATE

    if we have any device state bits to report then we report them, otherwise
    we do the default behavior described in the DDK.

    The docs are pretty lean on this one. We set status to success if we have bits to
    report, DDK(as of server 2003) implies this is correct behavior.

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    hubExt = FdoExt(HubFdo);
    LOG(HubFdo, LOG_PNP, 'sPnP', hubExt->PnpDeviceState, Irp);
    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));

    if (hubExt->PnpDeviceState != 0) {
        DbgTrace((UsbhDebugTrace, "%!FUNC! - PnpDeviceState = %08.8x\n",
            hubExt->PnpDeviceState));
        Irp->IoStatus.Information |= hubExt->PnpDeviceState;
        Irp->IoStatus.Status = STATUS_SUCCESS;
        HUB_EXCEPTION(USBFILE,  HubFdo, 0,
                          HubErr_PnpDeviceStateError,
                          0, 0, &Irp->IoStatus.Information,
                            sizeof(Irp->IoStatus.Information));
        //Test #13
    }

    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhFdoPnp_SurpriseRemove(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handles IRP_MN_SURPRISE_REMOVAL

    *If we get a surprise remove then PnP will know that all of our children
    are gone (same as a QBR) and remove them before removing us.
    Therefore we mark the devices as no longer present so that we process the
    remove properly when we get if for the PDO.



Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    LOG(HubFdo, LOG_PNP, 'fSR!', 0, Irp);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_SURPRISE_REMOVAL);

    hubExt = FdoExt(HubFdo);

    // two cases to deal with here,
    // case 1: SURPRISE! your removed
    // case 2: SURPRISE! you gonna be removed
    //

    // We track the 'current' FDO pnp state so that we properly fail requests that
    // attempt to access hardware that is gone.  We do this by moving to the STOP
    // state on a surprise remove and we treat the hardware as gone.
    hubExt->FdoFlags.HubHwNotPresent = 1;

    Usbh_PnpCtxFDO_Pnp_State(HubFdo, Ev_IRP_MN_S_REMOVE);

    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhQueryBusRelations(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handles IRP_MN_QUERY_DEVICE_RELATIONS - BusRelations

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_RELATIONS deviceRelations, oldRelations;
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt;
    PLIST_ENTRY le;
    HUB_FDO_PNPSTATE pnpState;
    ULONG oldRelationsCount = 0;
    GUID activityId;
    NTSTATUS idStatus;
    KIRQL irql;

    idStatus = UsbhEtwGetActivityId(Irp, &activityId);

    hubExt = FdoExt(HubFdo);

    UsbhEtwLogHubIrpEvent(HubFdo,
                          NULL,
                          NT_SUCCESS(idStatus) ? &activityId : NULL,
                          &USBHUB_ETW_EVENT_HUB_QUERY_BUSRELATIONS_DISPATCH,
                          0,
                          STATUS_SUCCESS);

    LOG(HubFdo, LOG_PNP, 'QBR1', Irp, hubExt->CurrentFdoPnpState);

    // if we have a pnp error try to tell pnp about it
    //if (hubExt->PnpDeviceState) {
         //IoInvalidateDeviceState(hubExt->PhysicalDeviceObject);
    //}

    pnpState = hubExt->CurrentFdoPnpState;

    oldRelations = (PDEVICE_RELATIONS)Irp->IoStatus.Information;
    if (oldRelations) {
        oldRelationsCount = oldRelations->Count;
    }

    if (pnpState != FDO_WaitPnpStop) {
        // "device can be in any pnp state when the OS sends this irp" jeez

        // if bus engine is not started then report success but no relations
        // this is effectively a nop
        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation(Irp);
        nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

        LOG(HubFdo, LOG_PNP, 'QBR!', 0, nts);

        return nts;
    }

    // resume the bus on any QBR
    Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, PNP_CONTEXT(HubFdo));

    // first allocate a device relations structure
    // non paged pool because we use a spinlock when filling it in
    UsbhAllocatePool_Z(deviceRelations, 
                       NonPagedPool,
                       sizeof(struct _DEVICE_RELATIONS) + 
                        ((N_PORTS(HubFdo) - 1) + oldRelationsCount) *
                        sizeof(PDEVICE_OBJECT));

    if (deviceRelations == NULL) {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    } else {
        nts = STATUS_SUCCESS;
    }


    if (NT_SUCCESS(nts)) {

        if (oldRelationsCount) {
            // Copy pre-existing relation list into the new relation list
            deviceRelations->Count = oldRelationsCount;
            RtlCopyMemory((PUCHAR)deviceRelations + sizeof(DEVICE_RELATIONS),
                           oldRelations->Objects,
                           oldRelationsCount * sizeof(PDEVICE_OBJECT));
        }

        if (oldRelations) {
            ExFreePool(oldRelations);
        }
        
        LOG(HubFdo, LOG_PNP, 'QBR2', Irp, deviceRelations);
        // build relations based on the PDOs we have in our list

        UsbhAcquirePdoStateLock(HubFdo, PNP_CONTEXT(HubFdo), PdoEv_QBR);
        hubExt->BusRelationsState = BusRelationsReported;

        for(le = hubExt->PdoPresentListHead.Flink;
            le != &hubExt->PdoPresentListHead;
            le = le->Flink) {

            pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                   le,
                   struct _DEVICE_EXTENSION_PDO,
                   PdoListLink);

            LOG(HubFdo, LOG_PNP, 'qbr1', pdoExt, 0);

            ASSERT_PDOEXT(pdoExt);

            // count should never exceed the number of physical ports
            if ((deviceRelations->Count - oldRelationsCount) >= N_PORTS(HubFdo)) {
                UsbhAssertFailure(HubFdo, Irp);
            }

            if (pdoExt->Stat_PdoEnumeratedAt.QuadPart == 0) { 
                KeQuerySystemTime(&pdoExt->Stat_PdoEnumeratedAt);
            }                

            ObReferenceObject(pdoExt->Pdo);
            deviceRelations->Objects[deviceRelations->Count] = pdoExt->Pdo;
            deviceRelations->Count++;

            SET_PDO_HWPNPSTATE(pdoExt->Pdo, Pdo_PnpRefHwPresent, PdoEv_QBR);

        }

        // for any PDOs on the deleted list PnP now knows they are gone
        while (!IsListEmpty(&hubExt->PdoRemovedListHead)) {

            le = RemoveHeadList(&hubExt->PdoRemovedListHead);

            pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                    le,
                    struct _DEVICE_EXTENSION_PDO,
                    PdoListLink);

            LOG(HubFdo, LOG_PNP, 'qbr2', pdoExt, 0);
            ASSERT_PDOEXT(pdoExt);

            UsbhAssert(HubFdo, GET_PDO_HWPNPSTATE(pdoExt->Pdo) == Pdo_PnpRefHwAbsent);

            SET_PDO_HWPNPSTATE(pdoExt->Pdo, Pdo_HwAbsent, PdoEv_QBR_Removed);
            pdoExt->PdoListLink.Flink = NULL;
            pdoExt->PdoListLink.Blink = NULL;

            KeAcquireSpinLock(&pdoExt->GlobalListStateLock, &irql);

            if (pdoExt->GlobalListState == PdoInGlobalList) {
                pdoExt->GlobalListState = PdoNotInGlobalList;
                USBD_RemoveDeviceFromGlobalList(pdoExt->Pdo);
            }

            KeReleaseSpinLock(&pdoExt->GlobalListStateLock, irql);
            
            InsertTailList(&hubExt->PdoAbsentListHead, &pdoExt->PdoListLink);
        }

        UsbhReleasePdoStateLock(HubFdo, PNP_CONTEXT(HubFdo));

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = (ULONG_PTR) deviceRelations;

        IoSkipCurrentIrpStackLocation(Irp);
        nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    } else {

         LOG(HubFdo, LOG_PNP, 'QBR3', Irp, deviceRelations);
        // returning error and no relations

        // PnP will interpret an error with no relations as a NOP
        // all devices will still be considered present. We leave
        // the PnpRef vars and device lists unchanged.

        Irp->IoStatus.Information = 0;

        if (oldRelationsCount) {
            ULONG i;

            for (i=0; i<oldRelationsCount; i++) {
                ObDereferenceObject(oldRelations->Objects[i]);
            }
        }

        if (oldRelations) {
            ExFreePool(oldRelations);
        }

        if (deviceRelations != NULL) {
            UsbhFreePool(deviceRelations);
        }

        UsbhCompleteIrp(Irp, nts);
    }

    UsbhEtwLogHubIrpEvent(HubFdo, 
                          NULL,
                          NT_SUCCESS(idStatus) ? &activityId : NULL,
                          &USBHUB_ETW_EVENT_HUB_QUERY_BUSRELATIONS_COMPLETE,
                          0,
                          STATUS_SUCCESS);


    return nts;

}


NTSTATUS
UsbhFdoPnp_QueryDeviceRelations(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:


Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_DEVICE_RELATIONS);

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_PNP, 'hQDR', Irp, 0);

    switch (ioStack->Parameters.QueryDeviceRelations.Type) {
    case BusRelations:
        nts = UsbhQueryBusRelations(HubFdo, Irp);
        break;

    case TargetDeviceRelation:
        LOG(HubFdo, LOG_PNP, 'QDRt', Irp, 0);
        nts = UsbhFdoChainIrp(HubFdo, Irp);
        break;

    default:
        LOG(HubFdo, LOG_PNP, 'QDR?', Irp, 0);
        nts = UsbhFdoChainIrp(HubFdo, Irp);
        break;
    }

    return nts;
}


NTSTATUS
UsbhQueryCapsComplete(
    PDEVICE_OBJECT PNull,
    PIRP Irp,
    PVOID Context
    )
 /*++

Routine Description:

    Handles IRP_MN_QUERY_CAPABILITIES

Arguments:

    standard OS args for a completion routine

Return Value:

    Whats in the irp, should be success.

--*/
{
    PDEVICE_OBJECT hubFdo = Context;
    PDEVICE_EXTENSION_HUB  hubExt;
    PDEVICE_CAPABILITIES deviceCapabilities;
    PIO_STACK_LOCATION ioStack;

    hubExt = FdoExt(hubFdo);
    DbgTrace((UsbhDebugTrace, "%!FUNC! %!STATUS!\n", Irp->IoStatus.Status));
    // we invoke only on success
    UsbhAssert(NULL, NT_SUCCESS(Irp->IoStatus.Status));

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    deviceCapabilities = ioStack->Parameters.DeviceCapabilities.Capabilities;

    // modify capabilities as appropriate
    deviceCapabilities->SurpriseRemovalOK = TRUE;

    // save a copy since this is what we base wakeup support on
    RtlCopyMemory(&hubExt->DeviceCapabilities,
                  deviceCapabilities,
                  sizeof(struct _DEVICE_CAPABILITIES));

    hubExt->FdoFlags.DeviceCaps = 1;

    SET_EVENT(&hubExt->PnpQcapsComplete);

    return Irp->IoStatus.Status;

}


NTSTATUS
UsbhFdoPnp_QueryCaps(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
 /*++

Routine Description:

    Handles IRP_MN_QUERY_CAPABILITIES

Arguments:

    OS args for Dispatch routine

Return Value:

    NT STATUS code returned by IoCallDriver.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_QUERY_CAPABILITIES);

    hubExt = FdoExt(HubFdo);

    IoCopyCurrentIrpStackLocationToNext(Irp);
    UsbhSetCompletionRoutine(HubFdo,
                             Irp,    // Irp
                             UsbhQueryCapsComplete,
                             HubFdo, // context
                             TRUE,   // invoke on success
                             FALSE,  // invoke on error
                             FALSE); // invoke on cancel

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}



NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    )
/*++

Routine Description:

    WDM Driver Initialization point

Arguments:


Return Value:

    nt status
*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG result = 1;

    //
    // Initialize global to make NonPagedPool be treated as NxPool on Win8 
    // and NonPagedPool on down-level
    //

    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);
    
    WPP_INIT_TRACING(DriverObject, UniRegistryPath);

    UsbhAssert(NULL, !HubInit);
    HubInit = TRUE;

    UsbhInitGlobal(UniRegistryPath);

    UsbhDriverObject = DriverObject;  // remember driver object

    if (UsbhInitializeHighResTimer()) {
        HubG.GlobalUseEnhancedTimer = 1;
    }

    //
    // Initialize the driver object with this driver's entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] =
        DriverObject->MajorFunction[IRP_MJ_CLOSE] =
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
        DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] =
        DriverObject->MajorFunction[IRP_MJ_PNP] =
        DriverObject->MajorFunction[IRP_MJ_POWER] =
        DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
            UsbhGenDispatch;

    DriverObject->DriverUnload = UsbhDriverUnload;
    DriverObject->DriverExtension->AddDevice = UsbhAddDevice;
    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = UsbhDeviceShutdown;

    nts = Usbh_PnpDriverEntry(DriverObject, UniRegistryPath);

    // Round Logsize to nearest power of 2.

    while (result < HubG.GlobalLogNPages && result <= 16)
        result = result << 1;

    HubG.GlobalLogNPages = result;

    if (NT_ERROR(nts)) {
        Usbh_PnpUnload(DriverObject);
    }
    
    USBHUB_InitAllBugChecks();

    return nts;
}


VOID
UsbhDriverUnload(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    unload point

Arguments:

Return Value:

    none
--*/
{
    DbgTrace((UsbhDebugTrace, "Hub unload\n"));

    Usbh_PnpUnload(DriverObject);

    // We should have no pending ISR work items at this point
    UsbhAssert(NULL, IsListEmpty(&HubG.HubIsrListHead));
    
    USBHUB_FreeAllBugChecks();

    WPP_CLEANUP(DriverObject);

    return;
}

ULONG Usbh_Long_AddDevice = 0;

NTSTATUS
UsbhAddDevice(
    PDRIVER_OBJECT DriverObject,
    PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    Dreaded WDM Add Device

    *Called whenever the hub driver is loaded to control a device.

     Possible reasons:
        1. a hub was attached to the BUS
        2. we where loaded as the generic parent for a composite device

Arguments:

    PhysicalDeviceObject - Parent device object PDO created to handle us.

Return Value:

    nt status
--*/
{
    NTSTATUS nts;
    PDEVICE_OBJECT hubFdo = NULL;
    PDEVICE_OBJECT topOfStackDeviceObject = NULL;
    PDEVICE_EXTENSION_HUB hubExt = NULL;  // pointer to our device extension
   

    //
    // Create a new hub on the USB
    //
    //
    DbgTrace((UsbhDebugTrace, "Hub AddDevice\n"));
#ifdef HBB    
    UsbhKdPrint((1, "USBHBB.SYS\n"));
#endif
    
    nts = IoCreateDevice(
              DriverObject,  // our driver object
              sizeof(struct _DEVICE_EXTENSION_HUB) +  // extension size for us
              HubG.GlobalLogNPages * 4096,  // plus room for debug log
              NULL,  // name for this device
              FILE_DEVICE_USB_HUB,  // HUB type
              FILE_AUTOGENERATED_DEVICE_NAME,  // device characteristics
              FALSE,  // Not exclusive
              &hubFdo);  // Our device object

    // prefix will complain if we don't check the FDO pointer too,
    // I guess it thinks IoCreateDevice might lie
    if (NT_SUCCESS(nts) && hubFdo) {
        // Congratulations  -- it's a hub
        //DDK says this is zeroed
        //RtlZeroMemory(hubExt, sizeof(struct _DEVICE_EXTENSION_HUB));
        hubExt = (PDEVICE_EXTENSION_HUB) hubFdo->DeviceExtension;
        UsbhFatalAssert(hubFdo,hubExt != NULL, DriverObject);
        hubExt->ExtensionType = EXTENSION_TYPE_HUB;

        // Store a pointer to the USBHUB_Triage_Info structure at a
        // fixed and known offset from the DEVICE_EXTENSION.
        //
        hubExt->UsbhubTriageInfo = &USBHUB_Triage_Info;
    }

    if (!hubFdo) {
        return STATUS_UNSUCCESSFUL;
    }

    
    // attach the FDO to the top of the stack
    if (NT_SUCCESS(nts)) {
        topOfStackDeviceObject =
            IoAttachDeviceToDeviceStack(hubFdo, PhysicalDeviceObject);
        if (!topOfStackDeviceObject) {
            IoDeleteDevice(hubFdo);
            return STATUS_UNSUCCESSFUL;
        }
    }

    UsbhFatalAssert(hubFdo,hubFdo != NULL, DriverObject);
    UsbhFatalAssert(hubFdo,NT_SUCCESS(nts), DriverObject);
    
    IoInitializeRemoveLock(&hubExt->RemoveLock, USBHUB_HEAP_TAG, 0, 0);

    // init the device extension

  
    hubExt->FunctionalDeviceObject = hubFdo;
    hubExt->PhysicalDeviceObject = PhysicalDeviceObject;
    hubExt->TopOfStackDeviceObject = topOfStackDeviceObject;
    hubExt->CurrentFdoPnpState = FDO_WaitPnpAdd;
    hubExt->PnpStateHandler = Usbh_FDO_WaitPnpAdd;
    // set the cycler pointer to always point at our internal buffer
    hubExt->Compatible_HubDescriptorPointer = &hubExt->HubDescriptor;

    hubExt->IsrWorkerReference = 1;
    INIT_EVENT_NOT_SIGNALED(&hubExt->HubIsrWorkerDone);

    // initialize pnp,power logs

    KeInitializeSemaphore(&hubExt->FdoPnpStateLock, 1, 1);
    RtlFillMemory(&hubExt->PowerFuncHistory, HUB_PP_HISTORY_LEN, 0xff);
    RtlFillMemory(&hubExt->PnpFuncHistory, HUB_PP_HISTORY_LEN, 0xff);

    KeInitializeSpinLock(&hubExt->FdoPwrStateLock);

    // assign a hub number, if we are all out we use zero.
    // we use this value for debugging and diagnostics

    // wait function used for testing only
    // by default Usbh_Long_AddDevice is zero making this a nop.  This var is used to introduce an artificial delay 
    // in the add device routine to reproduce race conditions assocaited with pnp Irps and AddDevice.  The 
    // DO_DEVICE_INITIALIZING flag does not prevent pnp irps from getting to the FDO.
    UsbhRawWait(Usbh_Long_AddDevice);

    UsbhAssignHubNumber(hubFdo);

    // init the device extension
    UsbhLogAlloc(hubFdo, &hubExt->Log, HubG.GlobalLogNPages);

    UsbhInitStateCtx(hubFdo, &hubExt->SyncPnpStateCtx, HubCtx_Fdo, hubFdo);
    hubExt->SyncPnpStateCtx.HubIsrWorkerState = HubIsrWorker_NotQueued;
    
    UsbhInitStateCtx(hubFdo, &hubExt->AsyncPnpStateCtx, HubCtx_Fdo, hubFdo);
    UsbhInitStateCtx(hubFdo, &hubExt->ApiStateCtx, HubCtx_Api, hubFdo);
    // this is the context which the port driver issues the global bus suspend
    // and resume requests
    UsbhInitStateCtx(hubFdo, &hubExt->SspApiStateCtx, HubCtx_SshApi, hubFdo);
    UsbhInitStateCtx(hubFdo, &hubExt->SspTimerStateCtx, HubCtx_SshTimer, hubFdo);
    // misc worker contexts, hard reset
    UsbhInitStateCtx(hubFdo, &hubExt->HardResetCtx, HubCtx_Reset, hubFdo);

    //register for shutdown notification
    IoRegisterShutdownNotification(hubFdo);

    nts = UsbhReferenceListOpen(hubFdo);

    if (NT_SUCCESS(nts)) {

        HGSL lock;
        // Take care of the module init that should be done before
        // clearing DO_DEVICE_INITIALIZING and allowing irps to come
        // through to the FDO.
        //
        UsbhModuleDispatch(START,
                           UsbhDriverObject,
                           hubFdo,
                           UsbhModuleAddDeviceInit,
                           NULL,
                           PNP_CONTEXT(hubFdo));

        // we are now ready for requests
        // some module init routines require us to be ready for irps
        hubFdo->Flags |= DO_POWER_PAGABLE;
        hubFdo->Flags &= ~DO_DEVICE_INITIALIZING;

        // allow pnp irps if not zero
        lock.Locked = 0;
        LockGSL(&lock);
        hubExt->myDO_DEVICE_INITIALIZING = 1;
        unLockGSL(&lock);
        
        //
        // init modules for 'AddDevice'
        //
        nts = Usbh_PnpCtxFDO_Pnp_State(hubFdo, Ev_ADD);
    }

    LOG(hubFdo, LOG_PNP, 'hAdd', 0, nts);

    if (NT_ERROR(nts)) {

        LOG(hubFdo, LOG_PNP, 'hAd!', 0, nts);
#if DBG
        UsbhKdPrint((1, "HUBFDO %x failed AddDevice (load) (%x)\n",
            hubFdo, nts));
        if (HubG.GlobalDebugLevel > 1) {
            // if we have debugmode (>1)on then we are interested in certain
            // breakpoints
            NT_ASSERTMSG("UsbhAddDevice failed", FALSE);
        }
#endif
        UsbhLogFree(hubFdo, &hubExt->Log);

        UsbhReleaseHubNumber(hubFdo);

        IoDetachDevice(topOfStackDeviceObject);
        IoDeleteDevice(hubFdo);
    }

    return nts;
}


NTSTATUS
UsbhFdoPnp_StartDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP start for a hub

    IRP_MN_START_DEVICE

Arguments:


Return Value:

    nt status

--*/

{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    KEVENT pnpStartEvent;
    PIO_STACK_LOCATION ioStack;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace,"UsbhFdoPnp_StartDevice\n"));

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_START_DEVICE);

    // pass the start irp down and wait for it to
    // complete
    INIT_EVENT_NOT_SIGNALED(&pnpStartEvent);
   
    IoCopyCurrentIrpStackLocationToNext(Irp);

    UsbhSetCompletionRoutine(HubFdo,
                             Irp,
                             UsbhDeferIrpCompletion,
                             // always pass FDO to completion routine
                             &pnpStartEvent,
                             TRUE,
                             TRUE,
                             TRUE);

    LOG(HubFdo, LOG_PNP, 'strW', 0, Irp);

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    if (nts == STATUS_PENDING) {
        WAIT_EVENT(&pnpStartEvent);
    }

    // get the deferred start error code
    nts = Irp->IoStatus.Status;
    if (NT_ERROR(nts)) {
        HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_StartDevFailedLower, nts, 0, NULL, 0);
        goto UsbhFdoPnp_StartDevice_Done;
    }

    UsbhAssert(HubFdo, NT_SUCCESS(nts));

    nts = Usbh_PnpCtxFDO_Pnp_State(HubFdo, Ev_IRP_MN_START);

    // synchronous start finished, complete the irp since we pended it with our
    // completion routine

UsbhFdoPnp_StartDevice_Done:

    KeQuerySystemTime(&hubExt->Stat_SyncPnpStartAt);

    LOG(HubFdo, LOG_PNP, 'STRT', 0, nts);
    UsbhCompleteIrp(Irp, nts);

    return nts;
}


NTSTATUS
UsbhFdoPnp_StopDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP stop for a hub

    Handles IRP_MN_STOP_DEVICE

Arguments:


Return Value:

   ntstatus

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    LOG(HubFdo, LOG_PNP, 'fSTP', 0, Irp);

    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_STOP_DEVICE);

    hubExt = FdoExt(HubFdo);

    Usbh_PnpCtxFDO_Pnp_State(HubFdo, Ev_IRP_MN_STOP);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    return nts;
}


NTSTATUS
UsbhFdoPnp_RemoveDevice(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp
    )
/*++

Routine Description:

    PnP stop for a hub

    Handles IRP_MN_REMOVE_DEVICE

Arguments:


Return Value:

    not failable

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;

    LOG(HubFdo, LOG_PNP, 'fREM', 0, Irp);
    ioStack = IoGetCurrentIrpStackLocation(Irp);
    UsbhAssert(HubFdo, ioStack->MinorFunction == IRP_MN_REMOVE_DEVICE);

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace, "FDO remove device\n"));

    UsbhSshDisabled(HubFdo, PNP_CONTEXT(HubFdo));

    nts = IoAcquireRemoveLock(&hubExt->RemoveLock, Irp);

    if (NT_ERROR(nts)) {
        // FDO can only handle one remove irp
        UsbhAssertFailure(HubFdo, Irp);

        Irp->IoStatus.Status = nts;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return nts;
    }

    LOG(HubFdo, LOG_PNP, 'istp', 0, 0);
    Usbh__TestPoint__NoData(HubFdo, TstPt_PnpRemoveDevice, nts);

    // sync with any outstanding power requests
    IoReleaseRemoveLockAndWait(&hubExt->RemoveLock, Irp);

    // stop the bus here , this flushes any enumeration activity that may be
    // in progress during the remove
    Usbh_PnpCtxFDO_Pnp_State(HubFdo, Ev_IRP_MN_REMOVE);

    UsbhReleaseHubNumber(HubFdo);
    UsbhLogFree(HubFdo, &hubExt->Log);

    // I cannot find anywhere in the DDK that it says you have to do this but
    // the verifier will complain if you don't
    Irp->IoStatus.Status = STATUS_SUCCESS;

    // SOP is to pass down then detach and delete
    IoSkipCurrentIrpStackLocation(Irp);
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, Irp);

    // Completion handler for WW IRP (UsbhFdoWakePoComplete_Action) runs
    // after the remove lock has been released.  Wait for it to complete
    // before we detach and delete the PDO.
    if (hubExt->HubWakeState != HUB_WAKESTATE_UNINITIALIZED) {
        WAIT_EVENT(&hubExt->PendingWakeIrpEvent);
    }

    //
    // Detach FDO from PDO
    //
    IoDetachDevice(hubExt->TopOfStackDeviceObject);

    // we should not be holding on to the stack
    UsbhAssert(HubFdo, hubExt->TopOfBusStack == NULL);
    //
    // delete FDO
    //
    IoDeleteDevice(HubFdo);

    return nts;
}



NTSTATUS
UsbhMakePdoName(
    PDEVICE_OBJECT HubFdo,
    PUNICODE_STRING PdoNameUnicodeString,
    ULONG Index
    )
/*++

Routine Description:

    creates a unicode name string for a PDO associated with a hub. The string
    is of the format:
        USBPDO-n  where n is the Value of Index.

Arguments:


Return Value:

   nt status code

--*/
{
    PWCHAR nameBuffer = NULL;
    WCHAR rootName[] = L"\\Device\\USBPDO-";
    UNICODE_STRING idUnicodeString;
    WCHAR buffer[32] = {0};
    NTSTATUS nts;
    USHORT length;

    length = sizeof(buffer)+sizeof(rootName);

    UsbhAllocatePool_Z(nameBuffer, NonPagedPool, length);

    if (nameBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(nameBuffer, rootName, sizeof(rootName));

    RtlInitUnicodeString(PdoNameUnicodeString,
                         nameBuffer);

    PdoNameUnicodeString->MaximumLength = length;

    RtlInitUnicodeString(&idUnicodeString,
                         &buffer[0]);

    idUnicodeString.MaximumLength = sizeof(buffer);

    nts = RtlIntegerToUnicodeString(
              Index,
              10,
              &idUnicodeString);

    if (NT_SUCCESS(nts)) {
        nts = RtlAppendUnicodeStringToString(PdoNameUnicodeString,
                                             &idUnicodeString);
    }

    if (NT_ERROR(nts)) {
        UsbhFreePool(nameBuffer);
    }

    return nts;
}


VOID
UsbhAssignHubNumber(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

   Assigns a 1 based hub number for the hub.  The hub numbers are stored as a bitmask,
   we use these to help identify a particular hub when doing diagnostics.

   The hub number is also used in the location information we display in devman.

Arguments:


Return Value:

   nt status code

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    hubExt->HubNumber = USBD_AllocateHubNumber();

    WAIT_EVENT(&HubG.HubFdoOmniListLock);
    InsertTailList(&HubG.HubFdoOmniListHead, &hubExt->FdoOmniLink);
    SET_EVENT(&HubG.HubFdoOmniListLock);

}


VOID
UsbhReleaseHubNumber(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

   Releases the 1 based hub number assigned to the hub.

Arguments:


Return Value:

   nt status code

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (hubExt->HubNumber != 0) {
        USBD_ReleaseHubNumber(hubExt->HubNumber);
    }

    WAIT_EVENT(&HubG.HubFdoOmniListLock);
    RemoveEntryList(&hubExt->FdoOmniLink);
    InitializeListHead(&hubExt->FdoOmniLink);
    SET_EVENT(&HubG.HubFdoOmniListLock);
}


VOID
UsbhInitGlobal(
     PUNICODE_STRING UniRegistryPath
     )
{

    RtlZeroMemory(&HubG, sizeof(struct _HUB_GLOBALS));

    KeInitializeSpinLock(&HubG.PdoStateLock);
    KeInitializeSpinLock(&HubG.HubIsrListSpin);
    INIT_SYNC_EVENT_SIGNALED(&HubG.HubFdoOmniListLock);
    INIT_SYNC_EVENT_NOT_SIGNALED(&HubG.HubIsrWorkerQueued);
    
    UsbhInitStateCtx(NULL, &HubG.GlobalHubContext, HubCtx_Global, &HubG);

    InitializeListHead(&HubG.HubPowerCtxListHead);
    InitializeListHead(&HubG.HubFdoOmniListHead);
    InitializeListHead(&HubG.HubIsrListHead);

    HubG.GlobalReplicantWaitTime = 500;
    HubG.GlobalMaxReplicantRetry = 8;
    HubG.GlobalDebugLevel = DEFAULT_DEBUG_LEVEL;
    HubG.GlobalExceptionLogLimit = 32;
    HubG.GlobalEnableTestPoints = 1;
    // better known as 'ESD'
    HubG.GlobalDisableHardReset = 0;
    HubG.GlobalMaxHardResetRetries = 10;

    HubG.GlobalDefaultHubPowerUpTime = 300;
    HubG.GlobalMsOsDescriptorMode = HUBG_MSOS_QUERY_ONE_TIME;
    HubG.DebugExtVersion = USB_KD_EXT_VER;
    // The original deadman timeout in the hub driver was 5 seconds, however, this is
    // excessive should we actually need to wait this long for a timeout.
    // The reset should complete within 10 ms, we use a value of 2500ms.

    HubG.ResetPortTimeoutInMilliseconds = 2000;

    HubG.GlobalWakeOnConnectUI = HUB_WAKE_C_Never;

    HubG.GlobalDisableOnSoftRemove = 1;

// USB per hub log
// on 32 bit x86 1 page is ~256 entries
// 2 pages = 512 entries etc...

#define USBHUB_DEAFULT_LOG_N_PAGES 2
    HubG.GlobalLogNPages = USBHUB_DEAFULT_LOG_N_PAGES;
    //
    // The following aggressive timeout will be used
    // for internal hubs when in connected standby. 
    //                                            
    HubG.IdleTimeout = DEFAULT_SS_TIMEOUT_FOR_HUBS_IN_MS;

}

NTSTATUS
UsbhDeviceShutdown(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    Dispatch routine for IRP_MJ_SHUTDOWN

Arguments:

Return Value:

    ntStatus
--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    nts = UsbhUxdShutdown(DeviceObject);
    UsbhCompleteIrp(Irp, nts);
    return nts;
}




PKBUGCHECK_REASON_CALLBACK_RECORD
USBHUB_InitBugCheck(
    PKBUGCHECK_REASON_CALLBACK_ROUTINE BugCheckCallback
)
/*++

Routine Description:

    Initializes structures we need to log data to a crash dump. Unforumately
    this is global.

Arguments:

Return Value:

    none.

--*/
{
    PKBUGCHECK_REASON_CALLBACK_RECORD bugCheckRecord;

    UsbhAllocatePool_Z(bugCheckRecord, NonPagedPool,
                 sizeof(KBUGCHECK_REASON_CALLBACK_RECORD));

    if (bugCheckRecord) {

        KeInitializeCallbackRecord(bugCheckRecord);

        if (!KeRegisterBugCheckReasonCallback(
                bugCheckRecord,
                BugCheckCallback,
                KbCallbackSecondaryDumpData,
                (PUCHAR)"USBHUB\0")) {

             ExFreePool(bugCheckRecord);
             bugCheckRecord = NULL;
        }
    }

    return bugCheckRecord;
}


VOID
USBHUB_FreeBugCheck(
    PKBUGCHECK_REASON_CALLBACK_RECORD BugCheckRecord
)
/*++

Routine Description:

    Dregisters and frees bugcheck structs

Arguments:

Return Value:

    none.

--*/
{
    if (BugCheckRecord) {
        KeDeregisterBugCheckReasonCallback(BugCheckRecord);
        ExFreePool(BugCheckRecord);
    }
    return;
}

PDEVICE_OBJECT USBHUB_CurrentFdo;

//0ffd7bf7-23c9-42aa-8396-ae2ded7374ef
DEFINE_GUID(GuidSavePortArrayData,
0x0ffd7bf7, 0x23c9, 0x42aa, 0x83, 0x96, 0xae, 0x2d, 0xed, 0x73, 0x74, 0xef);

//  585e6caa-f08e-4bc7-9ac8-28bca953a4cc
DEFINE_GUID(GuidSaveHubExtData,
0x585e6caa, 0xf08e, 0x4bc7, 0x9a, 0xc8, 0x28, 0xbc, 0xa9, 0x53, 0xa4, 0xcc);


VOID
USBHUB_BugCheckSavePortArrayData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    )
{
    PDEVICE_EXTENSION_HUB  hubExt;
    USHORT NumberOfPorts;
    PKBUGCHECK_SECONDARY_DUMP_DATA dumpData = ReasonSpecificData;
    ULONG sizeofPortData=0;

    if (ReasonSpecificDataLength < sizeof(KBUGCHECK_SECONDARY_DUMP_DATA)) {
        return;
    }

    if (USBHUB_CurrentFdo == NULL) {
        return;
    }

    hubExt = FdoExt(USBHUB_CurrentFdo);

    NumberOfPorts = hubExt->HubDescriptor.bNumberOfPorts;

    //
    // If InBufferLength == 0 this is only a query for OutBufferLength
    //

    sizeofPortData = sizeof(HUB_PORT_DATA)* NumberOfPorts;

    if (dumpData->InBufferLength)
    {
        dumpData->Guid = GuidSavePortArrayData;

        dumpData->OutBuffer = hubExt->PortData;
    }

    dumpData->OutBufferLength = sizeofPortData;
}


VOID
USBHUB_BugCheckSaveHubExtData(
    KBUGCHECK_CALLBACK_REASON Reason,
    struct _KBUGCHECK_REASON_CALLBACK_RECORD* Record,
    OUT PVOID ReasonSpecificData,
    ULONG ReasonSpecificDataLength
    )
{
    PKBUGCHECK_SECONDARY_DUMP_DATA dumpData = ReasonSpecificData;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG TotalLenToCopy =0;

    if (ReasonSpecificDataLength < sizeof(KBUGCHECK_SECONDARY_DUMP_DATA)) {
        return;
    }

    if (USBHUB_CurrentFdo == NULL) {
        return;
    }

    hubExt = FdoExt(USBHUB_CurrentFdo);

    TotalLenToCopy = sizeof(struct _DEVICE_EXTENSION_HUB) +
                     (HubG.GlobalLogNPages * 4096);

    //
    // If InBufferLength == 0 this is only a query for OutBufferLength
    //

    if (dumpData->InBufferLength) {

        dumpData->Guid = GuidSaveHubExtData;
        dumpData->OutBuffer  = hubExt;
    }

    dumpData->OutBufferLength = TotalLenToCopy ;

}

VOID
USBHUB_TriggerCallBacks (
    __inout PDEVICE_OBJECT DeviceObject
    )
/*
    Records the USBHUB Specific Data into the CallBack Functions.

*/
{
    PDEVICE_OBJECT oldDevObj;
    PDEVICE_EXTENSION_HUB  hubExt = NULL;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HEADER devExt;
    PDEVICE_OBJECT HubFdo = NULL;

    PUSB_HUB_BUS_MINIDUMP_INTERFACE bus;

    devExt = (PDEVICE_EXTENSION_HEADER) DeviceObject->DeviceExtension;

    // Make sure we always point to the PDEVICE_OBJECT of the HUB FDO and DEVICE_EXTENSION_HUB.

    switch(devExt->ExtensionType) {
    case EXTENSION_TYPE_HUB:
            HubFdo = DeviceObject;
            hubExt = FdoExt(DeviceObject);
         break;

    case EXTENSION_TYPE_PDO:
            pdoExt = PdoExt(DeviceObject);
            HubFdo = pdoExt->HubFdo;
            if(HubFdo != NULL)
            {
                hubExt = FdoExt(HubFdo);
            }

        break;
    }

    ObReferenceObject(HubFdo);

    oldDevObj = InterlockedExchangePointer(&USBHUB_CurrentFdo, HubFdo);


    bus = &hubExt->HubMiniDumpInterface;

    // At this point we also go and tell the USBPort to save its
    // Mini-Dump Data.  so we can have more info on the reason for the
    // season..

    if (bus->SetUsbPortMiniDumpFlags) {

        bus->SetUsbPortMiniDumpFlags(bus->BusContext);

    }


    if (oldDevObj) {
        ObDereferenceObject(oldDevObj);
    }
}

NTSTATUS
UsbhResetNotificationIrpCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    )
/*++

Routine Description:

    This routine is used in the boot path to detect when 
    an upstream hub's reset completed.
    A reset of the current hub will be triggered from here

Arguments:

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED, so the OS does not complete the irp

--*/
{
    PDEVICE_OBJECT deviceObject;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    PIO_STACK_LOCATION ioStack;
    ULONG HubResetFlags;

    deviceObject = (PDEVICE_OBJECT)Context;
    hubExt = FdoExt(deviceObject);
    ioStack = IoGetNextIrpStackLocation(hubExt->ResetNotificationRequestIrp);

    HubResetFlags = (ULONG) Irp->IoStatus.Information;
    if (HubResetFlags == HUB_RESET_STARTING) {
        UsbhResetNotifyDownstreamHub(deviceObject);
    } else {
        DbgTrace((UsbhPwBootTrace,"Received request to reset hub HubFdo=%p %!FUNC!\n", deviceObject));
        if (hubExt->HardResetState == HReset_WaitReady) {
            // If we are at Passive level we can call the hub reset function directly
            if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
                UsbhDispatch_HardResetEvent(deviceObject, RESET_CONTEXT(deviceObject), HRE_RequestReset);
            } else {
                // If we are not at passive level we need to throw an exception to
                // reset the hub
                HUB_RESET_EXCEPTION(USBFILE, deviceObject, 0, HubErr_HardError_Reset,
                          STATUS_UNSUCCESSFUL, USBD_STATUS_HUB_INTERNAL_ERROR, NULL, 0);
            }
        }
    }
    IoReuseIrp(hubExt->ResetNotificationRequestIrp,STATUS_SUCCESS);

    ioStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    
    ioStack->Parameters.DeviceIoControl.IoControlCode =
        IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST;
    
    // Buffer Lengths go in the io stack parameters
    ioStack->Parameters.DeviceIoControl.InputBufferLength = 0;
    ioStack->Parameters.DeviceIoControl.OutputBufferLength = 0;
    hubExt->ResetNotificationRequestIrp->IoStatus.Information = 0;

    UsbhSetCompletionRoutine(deviceObject,
                           hubExt->ResetNotificationRequestIrp, 
                           UsbhResetNotificationIrpCompletion, 
                           Context, 
                           TRUE, 
                           TRUE, 
                           TRUE);
    
    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, hubExt->ResetNotificationRequestIrp);

    NT_ASSERT(nts == STATUS_PENDING);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

VOID
UsbhResetNotifyDownstreamHub(
    __in PDEVICE_OBJECT HubFdo
)
/*++

Routine Description:

    This routine is used in the boot path to inform downstream devices
    that they need to start failing I/O because a reset is coming their way

    The assumption here is that a downstream hub will not start a reset
    while an upstream hub is reseting

Arguments:
    HubFdo - Fdo for the hub whose downstream devices are to be informed

Return Value:

--*/
{
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    ULONG pn;
    PHUB_PORT_DATA pd;


    // We need to go through every Pdo attached to this hub
    // And signal the child hub that a reset has begun
    for (pn = 1; pn <= N_PORTS(HubFdo); pn++) {
    
        // BUGBUG: This should ideally be converted into UsbhLatchPdo
        // but this function is sometimes called with the GSL
        // taken. That woould cause a hang.
        pd = UsbhGetPortData(HubFdo, (USHORT)pn);
        pdo = pd->PnpPdo;
        if (pdo == NULL) {
            continue;
        }
    
        // We have a valid pdo here. Is this a boot device pdo?
        pdoExt = PdoExt(pdo);
        if (pdoExt->IsBootDevice) {
            DbgTrace((UsbhPwBootTrace,"Received request to notify reset hub HubFdo=%p %!FUNC!\n", pdoExt->HubFdo));
            if (pdoExt->PendingResetNotificationRequestIrp != NULL) {
                pdoExt->PendingResetNotificationRequestIrp->IoStatus.Information
                    = HUB_RESET_STARTING;
                UsbhCompleteIrp(pdoExt->PendingResetNotificationRequestIrp,STATUS_SUCCESS);
            }
            // Once we are back from Complete Irp, it means all the hubs and devices
            // in the downstream boot path have been notified
            // If we do not have a pending Irp, it means that the current pdo
            // belongs to a storage device
            UsbhSetPdo_FailIo(pdo);

            LOG(HubFdo, LOG_PNP, 'RNDH', pdoExt->ResetCompleteWithoutFailingIO, pdo);
            pdoExt->ResetCompleteWithoutFailingIO = FALSE;
        }
    }
}


NTSTATUS
UsbhDriverNotFoundWorker(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    )
/*++

Routine Description:

    Worker routine for the "Driver Not Found" timer.  If a PDO has not received
    a start IRP by the time the timer expires, the downstream port will be disabled
    to allow the parent hub to enter selective suspend.  

Arguments:

    HubFdo - Parent hub.

    PortNumber - Port number for the PDO.

    Context - Context value that contains the PDO address

Return Value:

    nt status

--*/
{
    PDEVICE_OBJECT pdo;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdo = (PDEVICE_OBJECT)Context;

    pdoExt = PdoExt(pdo);
    
    WAIT_EVENT(&pdoExt->DriverNotFoundLock);

    // If the PDO hasn't received a start IRP, disable the port
    if (GET_PDO_SWPNPSTATE(pdo) == PDO_WaitPnpStart) {
 

        if ((pdoExt->PdoFlags.PortPaused == 0) && (pdoExt->PdoFlags.PortDisabled == 0)) {
            pdoExt->PdoFlags.PortDisabled = 1;
            UsbhDisablePort(HubFdo, UsbhGetPortData(HubFdo, LOWORD(PortNumber)));
       
            // Mark the port as idle so the parent hub can be selectively suspended
            UsbhSshSetPortsBusyState(HubFdo, LOWORD(PortNumber), PORT_IDLE, NO_RESUME_HUB);
        } else if (pdoExt->PdoFlags.PortPaused == 1) {
            // port is not running.  Likely due to system suspend, or something like a 
            // hub reset.  Requeue the timer so we can check again later.
            UsbhEnableTimerObject(HubFdo,
                                  &pdoExt->DriverNotFoundTimer,
                                  DRIVER_NOT_FOUND_TIMEOUT,   
                                  pdoExt->PortNumber,
                                  pdoExt->Pdo,
                                  SIG_DNF_WORKER,
                                  0);
        }
    }
    SET_EVENT(&pdoExt->DriverNotFoundLock);

    UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_DNF_Timer_Tag);

    return STATUS_SUCCESS;
}    


ULONG
UsbhDriverNotFoundTimer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    Timer callback for the "Driver Not Found" timer.

Arguments:
    
Irql: DISPATCH

Return Value:

--*/
{
    PDEVICE_OBJECT pdo;
    PDEVICE_OBJECT latchedPdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;

    pdo = (PDEVICE_OBJECT)Context;

    latchedPdo = UsbhLatchPdo(HubFdo, LOWORD(PortNumber), NULL, PdoLatch_DNF_Timer_Tag);

    // Make sure the original PDO is still present
    if (latchedPdo == NULL) 
        return FALSE;
    else if (latchedPdo != pdo) {
        UsbhUnlatchPdo(HubFdo, latchedPdo, NULL, PdoLatch_DNF_Timer_Tag);
        return FALSE;
    }
    
    pdoExt = PdoExt(pdo);

    nts = UsbhQueueWorkItem(pdoExt->HubFdo,
                            DelayedWorkQueue,
                            UsbhDriverNotFoundWorker,
                            Context,
                            pdoExt->PortNumber,
                            SIG_DNF_WORKER);

    if (!NT_SUCCESS(nts)) {
        UsbhUnlatchPdo(HubFdo, latchedPdo, NULL, PdoLatch_DNF_Timer_Tag);
    }

    return FALSE;
}
