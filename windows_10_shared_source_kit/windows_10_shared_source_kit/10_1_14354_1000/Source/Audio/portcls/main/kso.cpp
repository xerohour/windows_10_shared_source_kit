/*****************************************************************************
 * kso.cpp - KS object support (IrpTargets)
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"
#include <swenum.h>

#include "pcwpptr.h"
#include "kso.tmh"

/*****************************************************************************
 * OBJECT_CONTEXT
 *****************************************************************************
 * Context structure for all file objects.
 */
typedef struct
{
    PVOID               pObjectHeader;
    PIRPTARGET          pIrpTarget;
    BOOLEAN             ReferenceParent;
}
OBJECT_CONTEXT, *POBJECT_CONTEXT;

DEFINE_KSDISPATCH_TABLE(
                       KsoDispatchTable,
                       DispatchDeviceIoControl,
                       DispatchRead,
                       DispatchWrite,
                       DispatchFlush,
                       DispatchClose,
                       DispatchQuerySecurity,
                       DispatchSetSecurity,
                       DispatchFastDeviceIoControl,
                       DispatchFastRead,
                       DispatchFastWrite );


#define CAST_LVALUE(type,lvalue) (*((type*)&(lvalue)))
#define IRPTARGET_FACTORY_IRP_STORAGE(Irp)       \
    CAST_LVALUE(PIRPTARGETFACTORY,IoGetCurrentIrpStackLocation(Irp)->    \
                Parameters.Others.Argument4)

/*  IrpSuspender mini-class implementation

  This class contains two global [device level] events.  One indicates it is
  safe for an IRP to proceed [i.e., device powered up and started].  The other
  indicates no IRP should proceed [because the device is stopped].  Its methods
  allow the event states to be changed, and one method will suspend a given IRP
  until it is either safe to go, or it must fail due to stop or dynamic 
  subdevice removal.

******************************************************************************/

IrpSuspender::IrpSuspender()
{
    KeInitializeEvent(&EnableRemoval, NotificationEvent, FALSE);
    KeInitializeEvent(&SafeToProceed, NotificationEvent, TRUE);
}

IrpSuspender::~IrpSuspender()
{
    //  Just in case
    KeSetEvent(&EnableRemoval, NotificationEvent, FALSE);
}

void    IrpSuspender::SuspendIrps()
{
    KeClearEvent(&SafeToProceed);
    KeClearEvent(&EnableRemoval);
}

//  This funtion will cause the thread dispatching this IRP to wait until it 
//  can either go or must be failed.

bool    IrpSuspender::CanIrpBeProcessed(__in PIRP Irp)
{
    //  The first handle is for "safe to go".  If this satisfies the
    //  KeWaitForMultipleObjects call, it returns STATUS_SUCCESS, hence the
    //  logic used in the return below
    PVOID       WaitHandles[3] = {&SafeToProceed, &EnableRemoval, NULL};
    unsigned    Handles = 2;

    IO_STACK_LOCATION&  Op = *IoGetCurrentIrpStackLocation(Irp);

    if  (Op.MajorFunction == IRP_MJ_CREATE)
    {
        //  This is similar to the logic KS uses in dispatching a create IRP. If
        //  there is a parent file object, then this is not a create directly
        //  against the device, and we needn't worry about subdevice removal.

        if  (Op.FileObject->RelatedFileObject == NULL)
        {
            if( (KSCREATE_ITEM_IRP_STORAGE(Irp) == NULL) || 
                (KSCREATE_ITEM_IRP_STORAGE(Irp)->Context == NULL) )
            {
                _DbgPrintF( DEBUGLVL_VERBOSE, ("failing IRP_MJ_CREATE from CanIrpBeProcessed"));
                // Just fall through to the wait since we don't have a subdevice pointer from which
                // to get the removal event.
            }
            else
            {        
                _DbgPrintF( DEBUGLVL_VERBOSE, ("Adding Removal Event in CanIrpBeProcessed"));
                //  OK, the IRP Context tells us which create item, hence which
                //  subdevice- add its handle to the list, and kick the count
                WaitHandles[Handles++] = 
                    (PSUBDEVICE(KSCREATE_ITEM_IRP_STORAGE(Irp)->Context))->
                    GetRemovalEvent();
            }
        }
    }

    return  KeWaitForMultipleObjects(Handles, WaitHandles, WaitAny, Executive,
        KernelMode, FALSE, NULL, NULL) == STATUS_WAIT_0;
}

//  This method releases all waiting threads- they will pass or fail based upon
//  which event is set.

void    IrpSuspender::ReleaseIrps(bool FailThem)
{
    KeSetEvent(FailThem ? &EnableRemoval : &SafeToProceed, IO_NO_INCREMENT, FALSE);
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * AddIrpTargetFactoryToDevice()
 *****************************************************************************
 * Adds an IrpTargetFactory to a device's create items list.
 */
NTSTATUS
    NTAPI
    AddIrpTargetFactoryToDevice
    (
    __in        PDEVICE_OBJECT          pDeviceObject,
    __in        PIRPTARGETFACTORY       pIrpTargetFactory,
    __in        PWSTR                   pwcObjectClass,
    __in_opt    PSECURITY_DESCRIPTOR    pSecurityDescriptor
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrpTargetFactory);
    ASSERT(pwcObjectClass);

    PDEVICE_CONTEXT pDeviceContext  =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    NTSTATUS ntStatus =
        KsAddObjectCreateItemToDeviceHeader
        (
        pDeviceContext->pDeviceHeader,
        KsoDispatchCreate,
        pIrpTargetFactory,
        pwcObjectClass,
        pSecurityDescriptor
        );

    if (NT_SUCCESS(ntStatus))
    {
        pIrpTargetFactory->AddRef();
    }

    return ntStatus;
}

/*****************************************************************************
 * AddIrpTargetFactoryToObject()
 *****************************************************************************
 * Adds an IrpTargetFactory to a objects's create items list.
 */
NTSTATUS
    NTAPI
    AddIrpTargetFactoryToObject
    (
    __in     PFILE_OBJECT            pFileObject,
    __in     PIRPTARGETFACTORY       pIrpTargetFactory,
    __in     PWSTR                   pwcObjectClass,
    __in_opt PSECURITY_DESCRIPTOR    pSecurityDescriptor
    )
{
    PAGED_CODE();

    ASSERT(pFileObject);
    ASSERT(pIrpTargetFactory);
    ASSERT(pwcObjectClass);

    POBJECT_CONTEXT pObjectContext  =
        POBJECT_CONTEXT(pFileObject->FsContext);

    NTSTATUS ntStatus =
        KsAddObjectCreateItemToObjectHeader
        (
        pObjectContext->pObjectHeader,
        KsoDispatchCreate,
        pIrpTargetFactory,
        pwcObjectClass,
        pSecurityDescriptor
        );

    if (NT_SUCCESS(ntStatus))
    {
        pIrpTargetFactory->AddRef();
    }

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * KsoGetIrpTargetFromIrp()
 *****************************************************************************
 * Extracts the IrpTarget pointer from an IRP.
 */
PIRPTARGET
    NTAPI
    KsoGetIrpTargetFromIrp
    (
    IN  PIRP    Irp
    )
{
    ASSERT(Irp);

    return
        (
        POBJECT_CONTEXT
        (
        IoGetCurrentIrpStackLocation(Irp)
        ->  FileObject
        ->  FsContext
        )
        ->  pIrpTarget
        );
}

/*****************************************************************************
 * KsoGetIrpTargetFromFileObject()
 *****************************************************************************
 * Extracts the IrpTarget pointer from a FileObject pointer.
 */
PIRPTARGET
    NTAPI
    KsoGetIrpTargetFromFileObject(
                                 IN PFILE_OBJECT FileObject
                                 )
{
    ASSERT(FileObject);

    return POBJECT_CONTEXT( FileObject->FsContext )->pIrpTarget;
}

#pragma code_seg("PAGE")
NTSTATUS
GetDispatcherLockType(
    _In_    PDEVICE_CONTEXT     pDeviceContext,
    _In_    PIRP                Irp,
    _Out_   PcDispatcherLock *  LockType
    )
{
    NTSTATUS            status   = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpSp    = IoGetCurrentIrpStackLocation(Irp);
    PcDispatcherLock    lockType = ePcDispatcherLock_Device;

    PAGED_CODE();
    
    switch (irpSp->MajorFunction)
    {
    case IRP_MJ_DEVICE_CONTROL:
        {
            BOOLEAN isPositionProperty = FALSE;
            
            switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
            {
            case IOCTL_KS_PROPERTY:
                status = CheckPositionProperty(Irp, &isPositionProperty);
                if (NT_SUCCESS(status))
                {
                    if (isPositionProperty)
                    {
                        lockType = ePcDispatcherLock_Position;
                    }
                    else
                    {
                        lockType = ePcDispatcherLock_Device;
                    }
                }
                break;
            
            default:
                lockType = ePcDispatcherLock_Device;
                break;
            }
        }
        break;
        
    case IRP_MJ_CREATE:
        lockType = ePcDispatcherLock_Device;
        break;

    default:
        lockType = ePcDispatcherLock_None;
        break;
    }

    //
    // Set output value.
    //
    *LockType = lockType;
        
    return status;
}   

#pragma code_seg("PAGE")
VOID
AcquireDispatcherLock(
    _In_    PDEVICE_CONTEXT     pDeviceContext,
    _In_    PcDispatcherLock    LockType
    )
{
    PAGED_CODE();
    
    NTSTATUS status = STATUS_SUCCESS;

    switch(LockType)
    {
    case ePcDispatcherLock_Device:
        status = AcquireDevice(pDeviceContext);
        ASSERT(NT_SUCCESS(status));
        break;
        
    case ePcDispatcherLock_Position:
        status = AcquirePositionLock(pDeviceContext);
        ASSERT(NT_SUCCESS(status));
        break;
        
    case ePcDispatcherLock_None:
    default:
        break;
    }
}

#pragma code_seg("PAGE")
VOID
ReleaseDispatcherLock(
    _In_    PDEVICE_CONTEXT     pDeviceContext,
    _In_    PcDispatcherLock    LockType
    )
{
    PAGED_CODE();
    
    switch(LockType)
    {
    case ePcDispatcherLock_Device:
        ReleaseDevice(pDeviceContext);
        break;
        
    case ePcDispatcherLock_Position:
        ReleasePositionLock(pDeviceContext);
        break;
        
    case ePcDispatcherLock_None:
    default:
        break;
    }
}

#pragma code_seg("PAGE")

IRPDISP
GetIrpDisposition(
    _In_    PDEVICE_OBJECT      DeviceObject,
    _In_    UCHAR               MajorFunction,
    _In_    PcDispatcherLock    LockType,
    _In_    PIRP                Irp
    )
{
    PAGED_CODE();

    IRPDISP         irpDisp = IRPDISP_NOTREADY;
    BOOLEAN         bActivated = FALSE;
    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    //
    //  Creates need to loop here, because all checks have to be made under
    //  mutex.  All other types will simply bail out through a return
    //
    while (true)
    {
        //
        //  If we're removed, or not accepting any calls, fail this.
        //
        if ((pDeviceContext->DeviceRemoveState == DeviceRemoved) ||
            (pDeviceContext->DeviceRemoveState == DeviceSurpriseRemoved) ||
            (pDeviceContext->DeviceStopState == DeviceStopped))
        {
            irpDisp = IRPDISP_NOTREADY;
            goto Exit;
        }

        if ((MajorFunction == IRP_MJ_CREATE) && (pDeviceContext->PendCreates))
        {
            pDeviceContext->pExtension->Suspender.SuspendIrps();
            //  We cannot wait with the device mutex held, so let go of it
            ReleaseDispatcherLock(pDeviceContext, LockType);
            if  (!pDeviceContext->pExtension->Suspender.CanIrpBeProcessed(Irp))
            {
                AcquireDispatcherLock(pDeviceContext, LockType);
                irpDisp = IRPDISP_NOTREADY;
                goto Exit;
            }
            //  We reacquire context, but must loop again in case state changed
            //  while we were acquiring it.
            AcquireDispatcherLock(pDeviceContext, LockType);
            continue;
        }

        if ((pDeviceContext->DeviceStopState == DevicePausedForRebalance) ||
            (pDeviceContext->DeviceStopState == DeviceStartPending) ||
            ((pDeviceContext->RuntimePowerInfo != NULL) && 
             (pDeviceContext->RuntimePowerInfo->RuntimeSystemWorking == FALSE) &&
             (pDeviceContext->CurrentDeviceState != PowerDeviceD0)))
        {
            pDeviceContext->pExtension->Suspender.SuspendIrps();
            if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
            {
                //  We cannot wait with the device mutex held, so let go of it
                ReleaseDispatcherLock(pDeviceContext, LockType);
            }
            if  (!pDeviceContext->pExtension->Suspender.CanIrpBeProcessed(Irp))
            {
                if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
                {
                    AcquireDispatcherLock(pDeviceContext, LockType);
                }
                irpDisp = IRPDISP_NOTREADY;
                goto Exit;
            }
            
            if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
            {
                //  We reacquire context, but must loop again in case state went bad
                //  while we were acquiring it.
                AcquireDispatcherLock(pDeviceContext, LockType);
                continue;
            }
        }

        if (!bActivated)
        {
            if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
            {
                //  We cannot wait with the device mutex held, so let go of it
                ReleaseDispatcherLock(pDeviceContext, LockType);
            }

            if (!NT_SUCCESS(ActivateRuntimePowerComponent(pDeviceContext, TRUE)))
            {
                //
                // If we can't activate our component, it means we must have unregistered
                // with the framework and we must be going down.  Can't process any IRPs.
                //
                if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
                {
                    AcquireDispatcherLock(pDeviceContext, LockType);
                }
                irpDisp = IRPDISP_NOTREADY;
                goto Exit;
            }

            bActivated = TRUE;
            if  (MajorFunction == IRP_MJ_CREATE || MajorFunction == IRP_MJ_DEVICE_CONTROL)
            {
                //  We reacquire context, but must loop again in case state went bad
                //  while we were acquiring it.
                AcquireDispatcherLock(pDeviceContext, LockType);
            }
            continue;
        }
        
        irpDisp = IRPDISP_PROCESS;
        goto Exit;
    }

Exit:
    //
    // If we previously activated the runtime component but we aren't
    // ready to process the IRP, then idle and return.  Also, only
    // do this if the device is running, not if the device has been stopped
    // or removed, because we would have already unregistered with the 
    // runtime framework in that case.
    //
    if ( bActivated &&
        (IRPDISP_PROCESS != irpDisp) &&
        (pDeviceContext->DeviceRemoveState != DeviceRemoved) &&
        (pDeviceContext->DeviceRemoveState != DeviceSurpriseRemoved) &&
        (pDeviceContext->DeviceStopState != DeviceStopped))
    {
        IdleRuntimePowerComponent(pDeviceContext, TRUE);
    }
    return irpDisp;
}

/*****************************************************************************
 * DispatchCreate()
 *****************************************************************************
 * Handles a create IRP.
 */
NTSTATUS
    DispatchCreate
    (
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PIRP                pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    NTSTATUS ntStatus = STATUS_SUCCESS;
    IRPDISP  irpDisp;

    _DbgPrintF( DEBUGLVL_VERBOSE, ("DispatchCreate"));

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    AcquireDispatcherLock(pDeviceContext, ePcDispatcherLock_Device);

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_CREATE, 
                                ePcDispatcherLock_Device, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);
            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // dispatch the irp (it will get decremented in xDispatchCreate)
            ntStatus = KsDispatchIrp(pDeviceObject,pIrp);
            ReleaseRemoveLock( pDeviceContext, pIrp );
            break;
    }

    ReleaseDispatcherLock(pDeviceContext, ePcDispatcherLock_Device);

    if (IRPDISP_PROCESS == irpDisp)
    {
        IdleRuntimePowerComponent( pDeviceContext, TRUE );
    }

    return ntStatus;
}

/*****************************************************************************
 * xDispatchCreate()
 *****************************************************************************
 * Handles a create IRP.
 */
NTSTATUS
    xDispatchCreate
    (
    IN      PIRPTARGETFACTORY   pIrpTargetFactory,
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PIRP                pIrp
    )
{
    PAGED_CODE();

    ASSERT(pIrpTargetFactory);
    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    POBJECT_CONTEXT pObjectContext=NULL;
    BOOL bCreatedIrpTarget=FALSE;
    BOOL bReferencedBusObject=FALSE;
    KSOBJECT_CREATE ksObjectCreate = {0};

    _DbgPrintF( DEBUGLVL_VERBOSE, ("xDispatchCreate"));

    // acquire the remove lock
    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // If there no target, fail the IRP
    if (! pIrpTargetFactory )
    {
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    }

    if (NT_SUCCESS(ntStatus))
    {
        // Allocate our context structure.
        pObjectContext = POBJECT_CONTEXT(ExAllocatePoolWithTag(NonPagedPoolNx,sizeof(OBJECT_CONTEXT),'OosK'));
        if (!pObjectContext)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        ntStatus = KsReferenceSoftwareBusObject(pDeviceContext->pDeviceHeader);
        if (NT_SUCCESS(ntStatus))
        {
            bReferencedBusObject = TRUE;
        }
        else if (STATUS_NOT_IMPLEMENTED == ntStatus)
        {
            ntStatus = STATUS_SUCCESS;
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        // Tell the factory to create a new object.
        ksObjectCreate.CreateItemsCount = 0;
        ksObjectCreate.CreateItemsList = NULL;

        ntStatus = pIrpTargetFactory->NewIrpTarget(&pObjectContext->pIrpTarget,
                                                   &pObjectContext->ReferenceParent,
                                                   NULL,
                                                   NonPagedPoolNx,
                                                   pDeviceObject,
                                                   pIrp,
                                                   &ksObjectCreate);

        // NewIrpTarget should not pend
        ASSERT(ntStatus != STATUS_PENDING);
    }

    if (NT_SUCCESS(ntStatus))
    {
        bCreatedIrpTarget=TRUE;

        // Allocate KS's header for this object.
        ntStatus = KsAllocateObjectHeader(&pObjectContext->pObjectHeader,
                                          ksObjectCreate.CreateItemsCount,
                                          ksObjectCreate.CreateItemsList,
                                          pIrp,
                                          &KsoDispatchTable);
    }

    PIO_STACK_LOCATION pIrpSp = IoGetCurrentIrpStackLocation(pIrp);
    if (NT_SUCCESS(ntStatus))
    {
        // Hook up the context to the file object.
        ASSERT(pObjectContext);
        pIrpSp->FileObject->FsContext = pObjectContext;

        // AddRef the parent file object if this is a child.
        if (pObjectContext->ReferenceParent && pIrpSp->FileObject->RelatedFileObject)
        {
            ObReferenceObject(pIrpSp->FileObject->RelatedFileObject);
        }

        InterlockedIncrement(PLONG(&pDeviceContext->ExistingObjectCount));
        _DbgPrintF(DEBUGLVL_VERBOSE,("xDispatchCreate  objects: %d",pDeviceContext->ExistingObjectCount));

        ASSERT(pIrpSp->FileObject->FsContext);
    }
    else
    {
        if (bCreatedIrpTarget)
        {
            pObjectContext->pIrpTarget->Release();
        }
        if (pObjectContext)
        {
            ExFreePool(pObjectContext);
        }
        pIrpSp->FileObject->FsContext = NULL;
        if (bReferencedBusObject)
        {
            KsDereferenceSoftwareBusObject(pDeviceContext->pDeviceHeader);
        }
    }
    ASSERT(ntStatus != STATUS_PENDING);

    pIrp->IoStatus.Information = 0;
    CompleteIrp(pDeviceContext,pIrp,ntStatus);
    return ntStatus;
}

/*****************************************************************************
 * CompletePendedIrps
 *****************************************************************************
 * This pulls pended irps off the queue and either fails them or passes them
 * back to KsoDispatchIrp.
 */
void
    CompletePendedIrps
    (
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      COMPLETE_STYLE      CompleteStyle
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pDeviceContext);

    _DbgPrintF(DEBUGLVL_VERBOSE,("Completing pended create IRPs..."));

    pDeviceContext->pExtension->Suspender.ReleaseIrps(CompleteStyle == EMPTY_QUEUE_AND_FAIL);
}

/*****************************************************************************
 * FailPendedIrpsForSubdevice
 *****************************************************************************
 * This acquires pended irps from the queue, checks if they are for the
 * specified subdevice.  If they are for the subdevice, the irp is completed
 * with fail.  If the irp is not for the subdevice,
 */
void
    FailPendedIrpsForSubdevice
    (
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      PSUBDEVICE          pSubdevice
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pDeviceContext);
    ASSERT(pSubdevice);

    _DbgPrintF(DEBUGLVL_VERBOSE,("Completing pended create IRPs..."));

    if  (pSubdevice)
    {
        KeSetEvent(pSubdevice->GetRemovalEvent(), IO_NO_INCREMENT, FALSE);
    }
}

/*****************************************************************************
 * KsoDispatchCreate()
 *****************************************************************************
 * Handles object create IRPs using the IIrpTargetFactory interface pointer
 * in the Context field of the create item.
 */
NTSTATUS
    KsoDispatchCreate
    (
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
    )
{
    NTSTATUS    ntStatus;

    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    ntStatus = xDispatchCreate
                (
                PIRPTARGETFACTORY(KSCREATE_ITEM_IRP_STORAGE(pIrp)->Context),
                pDeviceObject,
                pIrp
                );

    return ntStatus;
}

/*****************************************************************************
 * KsoDispatchCreateWithGenericFactory()
 *****************************************************************************
 * Handles object create IRPs using the IIrpTarget interface pointer in the
 * device or object context.
 */
NTSTATUS
    KsoDispatchCreateWithGenericFactory
    (
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    PIRPTARGETFACTORY   pIrpTargetFactory;
    PFILE_OBJECT        pParentFileObject =
        IoGetCurrentIrpStackLocation(pIrp)->FileObject->RelatedFileObject;

    if (pParentFileObject)
    {
        // Get IrpTargetFactory from parent object context.
        pIrpTargetFactory =
            (   POBJECT_CONTEXT(pParentFileObject->FsContext)
                ->  pIrpTarget
            );
    }
    else
    {
        return  STATUS_INVALID_DEVICE_REQUEST;
    }

    return xDispatchCreate(pIrpTargetFactory,pDeviceObject,pIrp);
}

/*****************************************************************************
 * DispatchDeviceIoControl()
 *****************************************************************************
 * Dispatches device I/O control IRPs.
 */
NTSTATUS
    DispatchDeviceIoControl
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS            ntStatus;
    IRPDISP             irpDisp;
    PcDispatcherLock    lockType = ePcDispatcherLock_Device;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    //
    // Find out which dispatcher lock to acquire.
    //
    if (pDeviceContext->UsePositionLock)
    {
        ntStatus = GetDispatcherLockType(pDeviceContext, pIrp, &lockType);
        if (!NT_SUCCESS(ntStatus))
        {
            pIrp->IoStatus.Information = 0;
            // this call releases the remove lock.
            return CompleteIrp(pDeviceContext,pIrp,ntStatus);
        }
    }

    // Acquire the dispatcher lock.
    AcquireDispatcherLock(pDeviceContext, lockType);

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_DEVICE_CONTROL, 
                                lockType, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            _DbgPrintF(DEBUGLVL_VERBOSE,("FAILING DevIoCtl due to dev state"));

            ntStatus = STATUS_DEVICE_NOT_READY;

            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->DeviceIoControl( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            break;
    }

    // Release the dispatcher
    ReleaseDispatcherLock(pDeviceContext, lockType);

    if (IRPDISP_PROCESS == irpDisp)
    {
        IdleRuntimePowerComponent(pDeviceContext, TRUE);
    }

    return ntStatus;
}

/*****************************************************************************
 * DispatchFastDeviceIoControl()
 *****************************************************************************
 * Dispatches fast device I/O control calls.
 */
BOOLEAN
    DispatchFastDeviceIoControl
    (
    IN      PFILE_OBJECT        FileObject,
    IN      BOOLEAN             Wait,
    IN      PVOID               InputBuffer     OPTIONAL,
    IN      ULONG               InputBufferLength,
    OUT     PVOID               OutputBuffer    OPTIONAL,
    IN      ULONG               OutputBufferLength,
    IN      ULONG               IoControlCode,
    OUT     PIO_STATUS_BLOCK    IoStatus,
    IN      PDEVICE_OBJECT      DeviceObject
    )
{
    PAGED_CODE();

    ASSERT(FileObject);
    ASSERT(IoStatus);
    ASSERT(DeviceObject);

    BOOLEAN         bReturn = FALSE;
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    // Tell runtime power framework we're busy, if we were powered down or
    // inactive, we'll wait until we become active and powered up again.  If
    // we fail to go active, we need to fail.
    if (NT_SUCCESS(ActivateRuntimePowerComponent( pDeviceContext, TRUE )))
    {
        bReturn = (POBJECT_CONTEXT(FileObject->FsContext)->pIrpTarget->FastDeviceIoControl(
                    FileObject,
                    Wait,
                    InputBuffer,
                    InputBufferLength,
                    OutputBuffer,
                    OutputBufferLength,
                    IoControlCode,
                    IoStatus,
                    DeviceObject));

        IdleRuntimePowerComponent( pDeviceContext, TRUE );
    }

    return bReturn;
}

/*****************************************************************************
 * DispatchRead()
 *****************************************************************************
 * Dispatches read IRPs.
 */
NTSTATUS
    DispatchRead
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;
    IRPDISP  irpDisp;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_READ, 
                                ePcDispatcherLock_None, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->Read( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            IdleRuntimePowerComponent( pDeviceContext, TRUE );
            break;
    }

    return ntStatus;
}

/*****************************************************************************
 * DispatchFastRead()
 *****************************************************************************
 * Dispatches fast read calls.
 */
BOOLEAN
    DispatchFastRead
    (
    IN      PFILE_OBJECT        FileObject,
    IN      PLARGE_INTEGER      FileOffset,
    IN      ULONG               Length,
    IN      BOOLEAN             Wait,
    IN      ULONG               LockKey,
    OUT     PVOID               Buffer,
    OUT     PIO_STATUS_BLOCK    IoStatus,
    IN      PDEVICE_OBJECT      DeviceObject
    )
{
    PAGED_CODE();

    ASSERT(FileObject);
    ASSERT(IoStatus);
    ASSERT(DeviceObject);

    BOOLEAN         bReturn = FALSE;
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    // Tell runtime power framework we're busy, if we were powered down or
    // inactive, we'll wait until we become active and powered up again.  If
    // we fail to go active, we need to fail.
    if (NT_SUCCESS(ActivateRuntimePowerComponent( pDeviceContext, TRUE )))
    {
        bReturn = 
            (   POBJECT_CONTEXT(FileObject->FsContext)
                ->  pIrpTarget
                ->  FastRead
                (
                FileObject,
                FileOffset,
                Length,
                Wait,
                LockKey,
                Buffer,
                IoStatus,
                DeviceObject
                )
            );

        IdleRuntimePowerComponent( pDeviceContext, TRUE );
    }

    return bReturn;
}

/*****************************************************************************
 * DispatchWrite()
 *****************************************************************************
 * Dispatches write IRPs.
 */
NTSTATUS
    DispatchWrite
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;
    IRPDISP  irpDisp;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_WRITE, 
                                ePcDispatcherLock_None, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->Write( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            IdleRuntimePowerComponent( pDeviceContext, TRUE );
            break;
    }

    return ntStatus;
}

/*****************************************************************************
 * DispatchFastWrite()
 *****************************************************************************
 * Dispatches fast write calls.
 */
BOOLEAN
    DispatchFastWrite
    (
    IN      PFILE_OBJECT        FileObject,
    IN      PLARGE_INTEGER      FileOffset,
    IN      ULONG               Length,
    IN      BOOLEAN             Wait,
    IN      ULONG               LockKey,
    IN      PVOID               Buffer,
    OUT     PIO_STATUS_BLOCK    IoStatus,
    IN      PDEVICE_OBJECT      DeviceObject
    )
{
    PAGED_CODE();

    ASSERT(FileObject);
    ASSERT(IoStatus);
    ASSERT(DeviceObject);

    BOOLEAN         bReturn = FALSE;
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(DeviceObject->DeviceExtension);

    ASSERT(pDeviceContext);

    // Tell runtime power framework we're busy, if we were powered down or
    // inactive, we'll wait until we become active and powered up again.  If
    // we fail to go active, we need to fail.
    if (NT_SUCCESS(ActivateRuntimePowerComponent( pDeviceContext, TRUE )))
    {
        bReturn = 
            (   POBJECT_CONTEXT(FileObject->FsContext)
                ->  pIrpTarget
                ->  FastWrite
                (
                FileObject,
                FileOffset,
                Length,
                Wait,
                LockKey,
                Buffer,
                IoStatus,
                DeviceObject
                )
            );

        IdleRuntimePowerComponent( pDeviceContext, TRUE );
    }

    return bReturn;

}

/*****************************************************************************
 * DispatchFlush()
 *****************************************************************************
 * Dispatches flush IRPs.
 */
NTSTATUS
    DispatchFlush
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;
    IRPDISP  irpDisp;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_FLUSH_BUFFERS, 
                                ePcDispatcherLock_None, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->Flush( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            IdleRuntimePowerComponent( pDeviceContext, TRUE );
            break;
    }

    return ntStatus;
}

/*****************************************************************************
 * DispatchClose()
 *****************************************************************************
 * Dispatches close IRPs.
 */
NTSTATUS
    DispatchClose
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // get the stack location
    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

    // get the object context
    POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

    // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
    if( pObjectContext->pIrpTarget )
    {
        // get the parent file object (if there is one)
        PFILE_OBJECT pFileObjectParent = pIrpStack->FileObject->RelatedFileObject;

        // dispatch the close to the IrpTarget
        ntStatus = pObjectContext->pIrpTarget->Close( pDeviceObject, pIrp );

        // release the IrpTarget
        pObjectContext->pIrpTarget->Release();

        // dereference the software bus object
        KsDereferenceSoftwareBusObject( pDeviceContext->pDeviceHeader );

        // free the object header
        KsFreeObjectHeader( pObjectContext->pObjectHeader );

        // dereference the parent file object
        if (pObjectContext->ReferenceParent && pFileObjectParent)
        {
            ObDereferenceObject(pFileObjectParent);
        }

        // free the object context
        ExFreePool(pObjectContext);

    } else
    {
        ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
    }

    // decrement object count
#if DBG
    ULONG newObjectCount = 
#endif
    InterlockedDecrement(PLONG(&pDeviceContext->ExistingObjectCount));
#if DBG
    _DbgPrintF(DEBUGLVL_VERBOSE,("DispatchClose  objects: %d",newObjectCount));
#endif

    ReleaseRemoveLock( pDeviceContext, pIrp );

    return ntStatus;
}

/*****************************************************************************
 * DispatchQuerySecurity()
 *****************************************************************************
 * Dispatches query security IRPs.
 */
NTSTATUS
    DispatchQuerySecurity
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;
    IRPDISP  irpDisp;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_QUERY_SECURITY, 
                                ePcDispatcherLock_None, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->QuerySecurity( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            IdleRuntimePowerComponent( pDeviceContext, TRUE );
    }

    return ntStatus;
}

/*****************************************************************************
 * DispatchSetSecurity()
 *****************************************************************************
 * Dispatches set security IRPs.
 */
NTSTATUS
    DispatchSetSecurity
    (
    IN      PDEVICE_OBJECT   pDeviceObject,
    IN      PIRP             pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;
    IRPDISP  irpDisp;

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    // check the device state
    irpDisp = GetIrpDisposition(pDeviceObject, 
                                IRP_MJ_SET_SECURITY, 
                                ePcDispatcherLock_None, 
                                pIrp);
    switch(irpDisp) {

        default:
            ASSERT(0);

            //
            // Fall through
            //

        case IRPDISP_NOTREADY:

            ntStatus = STATUS_DEVICE_NOT_READY;
            pIrp->IoStatus.Information = 0;
            CompleteIrp(pDeviceContext,pIrp,ntStatus);
            break;

        case IRPDISP_PROCESS:

            // get the stack location
            PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

            // get the object context
            POBJECT_CONTEXT pObjectContext = POBJECT_CONTEXT(pIrpStack->FileObject->FsContext);

            // if we have an IrpTarget, go ahead and dispatch.  Otherwise, pass off to KS.
            if( pObjectContext->pIrpTarget )
            {
                ntStatus = pObjectContext->pIrpTarget->SetSecurity( pDeviceObject, pIrp );
            } else
            {
                ntStatus = KsDispatchIrp( pDeviceObject, pIrp );
            }

            ReleaseRemoveLock( pDeviceContext, pIrp );

            IdleRuntimePowerComponent( pDeviceContext, TRUE );
            break;
    }

    return ntStatus;
}

/*****************************************************************************
 * KsoSetMajorFunctionHandler()
 *****************************************************************************
 * Sets up the handler for a major function.
 */
NTSTATUS
    KsoSetMajorFunctionHandler
    (
    IN      PDRIVER_OBJECT  pDriverObject,
    IN      ULONG           ulMajorFunction
    )
{
    PAGED_CODE();

    ASSERT(pDriverObject);

    NTSTATUS            ntStatus        = STATUS_SUCCESS;

    switch (ulMajorFunction)
    {
    case IRP_MJ_CREATE:
        pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
        break;

    case IRP_MJ_CLOSE:
        pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
        break;

    case IRP_MJ_FLUSH_BUFFERS:
        pDriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = DispatchFlush;
        break;

    case IRP_MJ_DEVICE_CONTROL:
        pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceIoControl;
        break;

    case IRP_MJ_READ:
        pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
        break;

    case IRP_MJ_WRITE:
        pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
        break;

    case IRP_MJ_QUERY_SECURITY:
        pDriverObject->MajorFunction[IRP_MJ_QUERY_SECURITY] = DispatchQuerySecurity;
        break;

    case IRP_MJ_SET_SECURITY:
        pDriverObject->MajorFunction[IRP_MJ_SET_SECURITY] = DispatchSetSecurity;
        break;

    case IRP_MJ_DEVICE_CONTROL | KSDISPATCH_FASTIO:
        pDriverObject->FastIoDispatch->FastIoDeviceControl = DispatchFastDeviceIoControl;
        break;

    case IRP_MJ_READ | KSDISPATCH_FASTIO:
        pDriverObject->FastIoDispatch->FastIoRead = DispatchFastRead;
        break;

    case IRP_MJ_WRITE | KSDISPATCH_FASTIO:
        pDriverObject->FastIoDispatch->FastIoWrite = DispatchFastWrite;
        break;

    default:
        ntStatus = STATUS_INVALID_PARAMETER;
        break;
    }

    return ntStatus;
}

/*****************************************************************************
 * KsoDispatchIrp()
 *****************************************************************************
 * Dispatch an IRP.
 */
NTSTATUS
    KsoDispatchIrp
    (
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
    )
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS ntStatus;

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ntStatus = PcValidateDeviceContext(pDeviceContext, pIrp);
    if (!NT_SUCCESS(ntStatus))
    {
        // Don't know what to do, but this is probably a PDO.
        // We'll try to make this right by completing the IRP
        // untouched (per PnP, WMI, and Power rules). Note
        // that if this isn't a PDO, and isn't a portcls FDO, then
        // the driver messed up by using Portcls as a filter (huh?)
        // In this case the verifier will fail us, WHQL will catch
        // them, and the driver will be fixed. We'd be very surprised
        // to see such a case.

        // Assume FDO, no PoStartNextPowerIrp as this isn't IRP_MJ_POWER
        ntStatus = pIrp->IoStatus.Status;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    if (IoGetCurrentIrpStackLocation(pIrp)->MajorFunction == IRP_MJ_CREATE) {
        //
        // Creates must be handled differently because portcls does not do
        // a KsSetMajorFunctionHandler on IRP_MJ_CREATE.
        //
        ntStatus = DispatchCreate(pDeviceObject,pIrp);
    } else {
        //
        // At this point, the object in question may or may not be a portcls
        // object (it may be a Ks allocator, for instance).  Calling
        // KsDispatchIrp() will dispatch the Irp as it normally would for a
        // driver which does KsSetMajorFunctionHandler().  This will call
        // through the object header to the appropriate dispatch function.
        // For portcls objects, this is KsoDispatchTable above.  For Ks
        // allocators, this will route the call to the correct function
        // instead of going to the wrong dispatch routine here.
        //
        ntStatus = KsDispatchIrp(pDeviceObject,pIrp);
    }

    return ntStatus;

}

#pragma code_seg()

