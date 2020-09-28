/*++
Copyright (c) Microsoft Corporation

Module Name:

    ucx.h

Abstract:

    IOCTLs and bus interface calls to UCX

--*/

#pragma once

NTSTATUS
HUBUCX_NotifyForwardProgress(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    );


NTSTATUS
HUBUCX_GetDeviceBusInfo(
    __in
        PHUB_FDO_CONTEXT        FdoContext,
    __in
        UCXUSBDEVICE            DeviceHandle,
    __in
        PUSB_BUS_NOTIFICATION   UserBuffer,
    __in
        ULONG                   UserBufferLength
    );


NTSTATUS
HUBUCX_GetControllerName (
    __in
        PHUB_FDO_CONTEXT    FdoContext,
    __in
        PUSB_HUB_NAME       HubNameBuffer,
    __in
        ULONG               HubNameBufferLength
    );


BOOLEAN
HUBUCX_CheckIfHubIsDisconnected(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    );


VOID
HUBUCX_DisconnectDeviceInUCX (
    __in
        PPORT_CONTEXT           PortContext
    );


FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
VOID       
HUBUCX_AcquirePowerReferenceOnController(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT   StopIdleContext
    )
/*++

Routine Description:
    
    Acquires a power reference on the controller to 
    ensure it remains in D0. If the controller is in 
    a failed state and the power reference can not
    be acquired, then it is upto the controller stack
    to deal with it.
    
Returns:

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;  
    
    NT_ASSERT(hubControllerStackInterface->ControllerStopIdleSynchronous != NULL);               
    
    hubControllerStackInterface->ControllerStopIdleSynchronous(HubFdoContext->HubParentInterface.Hub,
                                                               StopIdleContext);
}

FORCEINLINE
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBUCX_ReleasePowerReferenceOnController(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT   StopIdleContext
    )
/*++

Routine Description:

    Releases the power reference previously acquired on the 
    controller
    
Returns:

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;  
    
    NT_ASSERT(hubControllerStackInterface->ControllerResumeIdle != NULL);               
    
    hubControllerStackInterface->ControllerResumeIdle(HubFdoContext->HubParentInterface.Hub,
                                                      StopIdleContext);
}


