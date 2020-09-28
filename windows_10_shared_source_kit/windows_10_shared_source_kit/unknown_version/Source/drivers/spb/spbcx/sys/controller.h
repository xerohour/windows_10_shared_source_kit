/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name: 

    controller.h

Abstract:

    This module contains the definitions for the CScxController
    class

Environment:

    kernel-mode only

Revision History:

--*/

PCScxController
FORCEINLINE
GetControllerFromFxDevice(
    __in WDFDEVICE FxDevice
    );

PCScxController
FORCEINLINE
GetControllerFromFxQueue(
    __in WDFQUEUE FxQueue
    );

//
// ScxController class
//

typedef
VOID
(CScxController::*PSCX_CONTROLLER_IO_CALLBACK)(
    __in PCScxRequest Request
    );

class CScxController : public CFxObject<WDFDEVICE,
                                        CScxController,
                                        GetControllerFromFxDevice,
                                        false>
{
private:

    //
    // The device handle to associate with
    //

    WDFDEVICE m_FxDevice;

    //
    // The remote target to the Resource Hub
    //
    WDFIOTARGET m_ResourceHubTarget;

    //
    // The controller-level I/O queue
    //

    WDFQUEUE m_ControllerQueue;

    //
    // The controller level lock queue
    //

    WDFQUEUE m_LockQueue;

    //
    // The locked IO synchronization event. It is acquired while 
    // processing locked IO or during a D0->Dx transition.
    //

    KEVENT m_LockedIoSynchronizationEvent;

    //
    // The locked IO watchdog timer. It warns when a target
    // has kept the bus locked for an excessive amount of time.
    // This is important because locked IO stalls Dx transitions,
    // which could hold up system power transitions.
    //

    WDFTIMER m_LockedIoWatchdog;
    static const ULONG s_LockedIoWatchdogTimeoutMs = 30000; // WIN8:551649 query from registry

    //
    // The target which has exclusive ownership of the controller.
    // Set when the controller queue is stopped due to a lock operation, or 
    // when a subsequent lock operation is processed.
    // Cleared when the last lock is released.
    //

    PCScxTarget m_ExclusiveOwner;

    //
    // Lock that protects the list of active targets
    //

    WDFWAITLOCK m_TargetListLock;

    //
    // List of active target objects.  Protected by the above lock.
    //

    LIST_ENTRY m_TargetListHead;

    //
    // Controller driver settings.
    //

    SPB_CONTROLLER_CONFIG m_DriverConfig;

    //
    // Set if an I/O-other callback was setup.
    //

    PFN_SPB_CONTROLLER_OTHER     m_DriverOtherIoCallback;
    PFN_WDF_IO_IN_CALLER_CONTEXT m_DriverOtherIoInCallerContextCallback;

    //
    // Driver-specified request attributes (if any).  these are used to 
    // add the driver's context into any requests which route through a 
    // class-extension queue.
    //
    // If not set then the size field is 0.
    //

    WDF_OBJECT_ATTRIBUTES m_DriverRequestAttributes;

    //
    // Same as request-attributes, but for targets (e.g. files)
    //

    WDF_OBJECT_ATTRIBUTES m_DriverTargetAttributes;

    //
    // Array of pointer-to-member-function callbacks used when dispatching 
    // steady-state I/O operations.
    //

    PSCX_CONTROLLER_IO_CALLBACK m_InvokeIoCallbacks[SpbRequestTypeMax];

    //
    // Array of connection IDs to be used when configuring function config pins.
    //

    PLARGE_INTEGER m_FunctionConfigConnectionIds;

    //
    // Array of I/O targets opened for func config.
    //

    WDFIOTARGET * m_FunctionConfigConnectionTarget;

    //
    // Number of function config connections. This should be equal to the
    // number of elements in the function config connection id/target arrays.
    //

    ULONG m_FunctionConfigConnectionCount;

    //
    // Outstanding ref count on this SPB controller resulting from file
    // create/close requests.
    //

    ULONG m_ControllerRefCount;

//
// Private methods
//

private:

    CScxController(
        __in WDFDEVICE FxDevice
        ) : CFxObject(FxDevice),
            m_FxDevice(FxDevice),
            m_ExclusiveOwner(NULL)
    {
        InitializeListHead(&m_TargetListHead);

        for(int i = 0; i < SpbRequestTypeMax; i +=1 )
        {
            m_InvokeIoCallbacks[i] = &CScxController::InvokeUnsupportedIo;
        }

        RtlZeroMemory(&m_DriverConfig, sizeof(SPB_CONTROLLER_CONFIG));
        m_DriverConfig.PowerManaged = WdfFalse;

        return;
    }

    static
    VOID
    _WriteIrpPreprocessEvent(
        __in WDFDEVICE FxDevice,
        __in PIRP Irp
        );

    NTSTATUS
    Initialize(
        __in PSPB_CONTROLLER_CONFIG Config
        );

public:
    
    static
    VOID
    _DeviceInitSetDefaultCallbacks(
        __in  PWDFCXDEVICE_INIT  CxDeviceInit
        );
    
    static
    NTSTATUS
    _DeviceInitSetDefaultDacl(
        __in  PWDFDEVICE_INIT  DeviceInit
        );

    static
    NTSTATUS
    _CreateAndInitialize(
        __in     WDFDEVICE               FxDevice,
        __in     PSPB_CONTROLLER_CONFIG  Config
        );

    static
    PCScxController
    _FromFxDevice(
        __in WDFDEVICE FxDevice
        )
    {
        return GetControllerFromFxDevice(FxDevice);
    }

    WDFDEVICE
    GetFxDevice(
        VOID
        )
    {
        return m_FxDevice;
    }

    WDFIOTARGET
    GetResourceHubTarget(
        VOID
        )
    {
        return m_ResourceHubTarget;
    }

    //
    // Methods to configure the controller.
    //

    VOID
    SetDriverOtherIoCallbacks(
        __in     PFN_SPB_CONTROLLER_OTHER     EvtSpbControllerIoOther,
        __in_opt PFN_WDF_IO_IN_CALLER_CONTEXT EvtIoInCallerContext
        )
    {
        m_DriverOtherIoCallback = EvtSpbControllerIoOther;
        m_DriverOtherIoInCallerContextCallback = EvtIoInCallerContext; 

        m_InvokeIoCallbacks[SpbRequestTypeOther] = 
            &CScxController::InvokeSpbIoOther;
    }

    VOID
    SetDriverRequestAttributes(
        __in PWDF_OBJECT_ATTRIBUTES Attributes
        )
    {
        m_DriverRequestAttributes = *Attributes;
    }

    PWDF_OBJECT_ATTRIBUTES
    GetDriverRequestAttributes(
        VOID
        )
    {
        if (m_DriverRequestAttributes.Size != 0) 
        {
            return &(m_DriverRequestAttributes);
        }
        else
        {
            return NULL;
        }
    }

    VOID
    SetDriverTargetAttributes(
        __in PWDF_OBJECT_ATTRIBUTES Attributes
        )
    {
        m_DriverTargetAttributes = *Attributes;
    }

    PWDF_OBJECT_ATTRIBUTES
    GetDriverTargetAttributes(
        VOID
        )
    {
        if (m_DriverTargetAttributes.Size != 0) 
        {
            return &(m_DriverTargetAttributes);
        }
        else
        {
            return NULL;
        }
    }

    //
    // Methods to check what the controller driver supports.
    //

    inline
    bool 
    DriverSupportsSequences(
        VOID
        )
    {
        return (m_DriverConfig.EvtSpbIoSequence != NULL);
    }

    inline
    bool 
    DriverSupportsLockUnlock(
        VOID
        )
    {
        //
        // Registration of the unlock callback is the sole indicator
        // as to whether the driver fully supports the lock and unlock 
        // IOCTLs. In all cases registering for the lock callback is
        // optional.
        //

        return (m_DriverConfig.EvtSpbControllerUnlock != NULL);
    }

    inline
    bool 
    DriverSupportsOtherIo(
        VOID
        )
    {
        return (m_DriverOtherIoCallback != NULL);
    }

    inline
    WDF_TRI_STATE 
    DriverPowerManaged(
        VOID
        )
    {
        return m_DriverConfig.PowerManaged;
    }

    inline
    VOID 
    SetExclusiveOwner(
        PCScxTarget Target
        )
    {
        m_ExclusiveOwner = Target;
    }

    VOID
    ProcessRequest(
        __in PCScxRequest Request
        );

    VOID
    ForwardRequestToControllerQueue(
        __in PCScxRequest Request
        );

    VOID
    CompleteLock(
        __in PCScxRequest LockRequest
        );

    VOID
    BeginUnlock(
        __in PCScxRequest UnlockRequest
        );

    VOID
    EndUnlock(
        VOID
        );

    //
    // Methods to invoke callbacks in the I/O controller driver.
    //

    NTSTATUS
    InvokeIoInCallerContext(
        __in PCScxRequest Request
        );

    NTSTATUS
    InvokeSpbTargetConnect(
        __in PCScxTarget Target
        );

    VOID
    InvokeSpbTargetDisconnect(
        __in PCScxTarget Target
        );

    VOID
    InvokeUnsupportedIo(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbIoRead(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbIoWrite(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbIoSequence(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbIoOther(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbControllerLock(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbControllerUnlock(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbConnectionLock(
        __in PCScxRequest Request
        );

    VOID
    InvokeSpbConnectionUnlock(
        __in PCScxRequest Request
        );

    NTSTATUS
    ParseFunctionConfigResources(
        _In_ PCM_RESOURCE_LIST ResourceList
        );

    NTSTATUS
    OpenFunctionConfigConnections(
        VOID
        );

    VOID
    CloseFunctionConfigConnections(
        VOID
        );

    //
    // Callback methods.
    //

    static EVT_WDF_IO_IN_CALLER_CONTEXT       _OnIoInCallerContext;

    static EVT_WDFDEVICE_WDM_IRP_DISPATCH     _OnIrpDispatch;

    static EVT_WDFCXDEVICE_WDM_IRP_PREPROCESS _OnIrpPreprocess;

    static IO_COMPLETION_ROUTINE              _OnDxIrpComplete;

    static EVT_WDF_TIMER                      _OnLockedIoWatchdogExpired;

    static EVT_WDF_IO_QUEUE_IO_DEFAULT        _OnControllerIoDefault;

    static EVT_WDF_IO_QUEUE_STATE             _OnLockQueueReady;

    static EVT_WDF_IO_QUEUE_STATE             _OnLockStopComplete;

    static EVT_WDFCX_DEVICE_FILE_CREATE       _OnFileCreate;
    static EVT_WDF_FILE_CLOSE                 _OnFileClose;
    static EVT_WDF_FILE_CLEANUP               _OnFileCleanup;

    static EVT_WDF_IO_TARGET_REMOVE_COMPLETE  _OnIoTargetRemoveComplete;
    
};

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CScxController , _GetControllerFromFxDevice);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PCScxController, _GetControllerFromFxQueue);

PCScxController
FORCEINLINE
GetControllerFromFxDevice(
    __in WDFDEVICE FxDevice
    )
{
    return _GetControllerFromFxDevice(FxDevice);
}

PCScxController
FORCEINLINE
GetControllerFromFxQueue(
    __in WDFQUEUE FxQueue
    )
{
    PCScxController* controller = _GetControllerFromFxQueue(FxQueue);
    NT_ASSERTMSG("_FromFxQueue invoked on non-target queue", 
                 controller != NULL);
    return *controller;
}
