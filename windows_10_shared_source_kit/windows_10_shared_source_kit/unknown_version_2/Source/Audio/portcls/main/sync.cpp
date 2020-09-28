/*****************************************************************************
 * sync.cpp - synchronization
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"

static KSERVICE_ROUTINE CInterruptSyncServiceRoutine;

/*****************************************************************************
 * IInterruptSyncInit
 *****************************************************************************
 * Interface for interrupt synchronizer with Init.
 */
DECLARE_INTERFACE_(IInterruptSyncInit,IInterruptSync)
{
    DEFINE_ABSTRACT_UNKNOWN()   // For IUnknown

    // For IInterruptSync
    STDMETHOD_(NTSTATUS,CallSynchronizedRoutine)
    (   THIS_
        IN      PINTERRUPTSYNCROUTINE   Routine,
        IN      PVOID                   DynamicContext
    )   PURE;
    STDMETHOD_(PKINTERRUPT,GetKInterrupt)
    (   THIS
    )   PURE;
    STDMETHOD_(NTSTATUS,Connect)
    (   THIS
    )   PURE;
    STDMETHOD_(void,Disconnect)
    (   THIS
    )   PURE;
    STDMETHOD_(NTSTATUS,RegisterServiceRoutine)
    (   THIS_
        IN      PINTERRUPTSYNCROUTINE   Routine,
        IN      PVOID                   DynamicContext,
        IN      BOOLEAN                 First
    )   PURE;

    // For IInterruptSyncInit
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      PRESOURCELIST           ResourceList,
        IN      ULONG                   ResourceIndex,
        IN      INTERRUPTSYNCMODE       Mode
    )   PURE;
};

typedef IInterruptSyncInit *PINTERRUPTSYNCINIT;

struct CAddEntryContext
{
    PLIST_ENTRY Entry;
    PLIST_ENTRY List;
    BOOLEAN     First;
};

/*****************************************************************************
 * CInterruptSync
 *****************************************************************************
 * Interrupt synchronizer implementation.
 */
class CInterruptSync
:   public IInterruptSyncInit,
    public CUnknown
{
private:
    CM_PARTIAL_RESOURCE_DESCRIPTOR  m_descriptor;
    INTERRUPTSYNCMODE               m_mode;
    PKINTERRUPT                     m_pKInterrupt;
    LIST_ENTRY                      m_listEntry;
    KSPIN_LOCK                      m_kSpinLock;
    KIRQL                           m_kIrql;

    static
    KSYNCHRONIZE_ROUTINE AddNewEntry;

public:
    DECLARE_STD_UNKNOWN();
    CInterruptSync(PUNKNOWN pUnknownOuter);
    ~CInterruptSync();

    STDMETHODIMP_(NTSTATUS) Init
    (
        IN      PRESOURCELIST           ResourceList,
        IN      ULONG                   ResourceIndex,
        IN      INTERRUPTSYNCMODE       Mode
    );

    IMP_IInterruptSync;
    
    friend
    static
    KSERVICE_ROUTINE CInterruptSyncServiceRoutine;

    friend
    static
    KSYNCHRONIZE_ROUTINE CInterruptSyncWrapperRoutine;
};

/*****************************************************************************
 * ISRLISTENTRY
 *****************************************************************************
 * Entry in the list of ISRs.
 */
typedef struct
{
    LIST_ENTRY              ListEntry;
    PINTERRUPTSYNCROUTINE   Routine;
    PVOID                   DynamicContext;
} 
ISRLISTENTRY, *PISRLISTENTRY;

/*****************************************************************************
 * WRAPPERROUTINECONTEXT
 *****************************************************************************
 * Context for synchronized routine wrapper function.
 */
typedef struct
{
    PINTERRUPTSYNCROUTINE   Routine;
    PVOID                   DynamicContext;
    CInterruptSync *        InterruptSync;
    NTSTATUS                NtStatus;
} 
WRAPPERROUTINECONTEXT, *PWRAPPERROUTINECONTEXT;





/*****************************************************************************
 * Factory
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CreateInterruptSync()
 *****************************************************************************
 * Creates an interrupt synchronization object.
 */
NTSTATUS
CreateInterruptSync
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    _DbgPrintF(DEBUGLVL_LIFETIME,("Creating INTERRUPTSYNC"));

    STD_CREATE_BODY_
    (
        CInterruptSync,
        Unknown,
        UnknownOuter,
        PoolType,
        PINTERRUPTSYNC
    );
}

/*****************************************************************************
 * PcNewInterruptSync()
 *****************************************************************************
 * Creates and initializes an interrupt-level synchronization object.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewInterruptSync
(
    __out       PINTERRUPTSYNC *        OutInterruptSync,
    __in_opt    PUNKNOWN                OuterUnknown,
    __in        PRESOURCELIST           ResourceList,
    __in        ULONG                   ResourceIndex,
    __in        INTERRUPTSYNCMODE       Mode
)
{
    PAGED_CODE();

    ASSERT(OutInterruptSync);
    ASSERT(ResourceList);

    //
    // Invalidate Parameters.
    //
    if (NULL == OutInterruptSync ||
        NULL == ResourceList)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcInterruptSync : Invalid Parameter"));
        return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN    unknown;
    NTSTATUS    ntStatus = CreateInterruptSync( &unknown,
                                                GUID_NULL,
                                                OuterUnknown,
                                                NonPagedPoolNx );

    if (NT_SUCCESS(ntStatus))
    {
        PINTERRUPTSYNCINIT interruptSync;
        ntStatus = unknown->QueryInterface( IID_IInterruptSync,
                                            (PVOID *) &interruptSync );

        if (NT_SUCCESS(ntStatus))
        {
            ntStatus = interruptSync->Init( ResourceList,
                                            ResourceIndex,
                                            Mode );

            if(NT_SUCCESS(ntStatus))
            {
                *OutInterruptSync = interruptSync;
            }
            else
            {
                interruptSync->Release();
            }
        }

        unknown->Release();
    }

    return ntStatus;
}





#pragma code_seg("PAGE")

/*****************************************************************************
 * CInterruptSync member functions
 */

/*****************************************************************************
 * CInterruptSync::CInterruptSync()
 *****************************************************************************
 * Constructor.
 */
CInterruptSync::
CInterruptSync
(   IN      PUNKNOWN    pUnknownOuter
)
:   CUnknown(pUnknownOuter)
{
    PAGED_CODE();

    KeInitializeSpinLock(&m_kSpinLock);
    InitializeListHead(&m_listEntry);
}

#pragma code_seg()

/*****************************************************************************
 * CInterruptSync::~CInterruptSync()
 *****************************************************************************
 * Destructor.
 */
CInterruptSync::~CInterruptSync()
{
    _DbgPrintF(DEBUGLVL_LIFETIME,("Destroying INTERRUPTSYNC (0x%08x)",this));

    //
    // Make sure we're disconnected.
    //
    Disconnect();

    //
    // Delete the list of ISRs.
    //
    if (! IsListEmpty(&m_listEntry))
    {
        KIRQL kIrqlOld;
        KeAcquireSpinLock(&m_kSpinLock,&kIrqlOld);

        //
        // Get rid of any remaining members.
        //
        while (! IsListEmpty(&m_listEntry))
        {
            PLIST_ENTRY pListEntry = RemoveHeadList(&m_listEntry);

            delete PISRLISTENTRY(pListEntry);
        }

        KeReleaseSpinLock(&m_kSpinLock,kIrqlOld);
    }
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * CDmaChannel::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
STDMETHODIMP_(NTSTATUS)
CInterruptSync::
NonDelegatingQueryInterface
(
    __in    REFIID  Interface,
    __out   PVOID * Object
)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(this));
    }
    else if (IsEqualGUIDAligned(Interface,IID_IInterruptSync))
    {
        *Object = PVOID(PINTERRUPTSYNCINIT(this));
    }
    else
    {
        *Object = NULL;
    }

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

/*****************************************************************************
 * CInterruptSync::Init()
 *****************************************************************************
 * Initializes the synchronization object.
 */
STDMETHODIMP_(NTSTATUS)
CInterruptSync::
Init
(
    IN      PRESOURCELIST           ResourceList,
    IN      ULONG                   ResourceIndex,
    IN      INTERRUPTSYNCMODE       Mode
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME,("Initializing INTERRUPTSYNC (0x%08x)",this));

    ASSERT(ResourceList);

    m_mode = Mode;

    PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor = ResourceList->FindTranslatedInterrupt(ResourceIndex);

    NTSTATUS ntStatus = ( pDescriptor ? STATUS_SUCCESS : STATUS_INSUFFICIENT_RESOURCES );

    if (NT_SUCCESS(ntStatus)) 
    {
        m_descriptor    = *pDescriptor;
        m_pKInterrupt   = NULL;

        m_kIrql = KIRQL(m_descriptor.u.Interrupt.Level);
    } 

    return ntStatus;
}

#pragma code_seg()

/*****************************************************************************
 * CInterruptSyncServiceRoutine()
 *****************************************************************************
 * Wrapper for service routine.
 */
static
BOOLEAN
CInterruptSyncServiceRoutine
(
    IN      PKINTERRUPT Interrupt,
    IN      PVOID       PVoidContext
)
{
    CInterruptSync *pCInterruptSync = (CInterruptSync *)(PVoidContext);

    BOOLEAN bResult = FALSE;

    //
    // Call ISRs as indicated by mode.
    //
    while (1)
    {
        BOOLEAN bResultThisPass = FALSE;

        for
        (   PLIST_ENTRY pListEntry = pCInterruptSync->m_listEntry.Flink;
            pListEntry != &pCInterruptSync->m_listEntry;
            pListEntry = pListEntry->Flink
        )
        {
            PISRLISTENTRY pIsrListEntry = PISRLISTENTRY(pListEntry);

            ASSERT(pIsrListEntry->Routine);

            if( NT_SUCCESS( pIsrListEntry->Routine( PINTERRUPTSYNC(pCInterruptSync),
                                                    pIsrListEntry->DynamicContext ) ) )
            {
                bResult         = TRUE;
                bResultThisPass = TRUE;

                if (pCInterruptSync->m_mode == InterruptSyncModeNormal)
                {
                    break;
                }
            }
        }

        if( (pCInterruptSync->m_mode != InterruptSyncModeRepeat) ||
            (! bResultThisPass) )
        {
            break;
        }
    }

    return bResult;
}

/*****************************************************************************
 * CInterruptSync::Connect()
 *****************************************************************************
 * Initializes the synchronization object.
 */
STDMETHODIMP_(NTSTATUS)
CInterruptSync::
Connect
(   void
)
{
    _DbgPrintF(DEBUGLVL_BLAB,("CInterruptSync::Connect"));

    NTSTATUS ntStatus = STATUS_SUCCESS;
    KAFFINITY InterruptAffinity;

    //
    // Don't even think about connecting if we don't have any
    // ISR's in our list
    //
    KIRQL oldIrql;
    KeAcquireSpinLock( &m_kSpinLock, &oldIrql );
    if( IsListEmpty( &m_listEntry ) )
    {
        ntStatus = STATUS_UNSUCCESSFUL;
    }
    KeReleaseSpinLock( &m_kSpinLock, oldIrql );

    //
    // Connect if not already connected
    //
    if ( (NT_SUCCESS(ntStatus)) && (!m_pKInterrupt) )
    {

        InterruptAffinity = m_descriptor.u.Interrupt.Affinity;

        //
        // If an interrupt affinity override was specified in the registry, and
        // we are running on an MP machine, and at least one active processor
        // will be able to handle our devices interrupt if only processors
        // in both the device interrupt affinity specification AND the registry
        // interrupt affinity are allowed to handle the interrupts, then
        // limit the device interrupt affinity to the subset of both affinity
        // masks.
        //




        //
        if (gInterruptAffinity &&
            KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS) > 1 &&
            (InterruptAffinity&gInterruptAffinity&KeQueryGroupAffinity(0)) ) {
            InterruptAffinity &= gInterruptAffinity;
        }
        
        ntStatus = IoConnectInterrupt( &m_pKInterrupt,
                                       CInterruptSyncServiceRoutine,
                                       PVOID(this),
                                       &m_kSpinLock,      
                                       m_descriptor.u.Interrupt.Vector,
                                       m_kIrql,
                                       m_kIrql,           
                                       ((m_descriptor.Flags & CM_RESOURCE_INTERRUPT_LATCHED) ? 
                                         Latched : LevelSensitive),
                                       (m_descriptor.ShareDisposition != CmResourceShareDeviceExclusive), 
                                       InterruptAffinity,
                                       FALSE );
        if(NT_SUCCESS(ntStatus))
        {
            ASSERT(m_pKInterrupt);
        }
    } 

    return ntStatus;
}

#pragma code_seg("PAGE")

/*****************************************************************************
 * CInterruptSync::Disconnect()
 *****************************************************************************
 * Disconnect from the interrupt.
 */
STDMETHODIMP_(void)
CInterruptSync::
Disconnect
(   void
)
{
    _DbgPrintF(DEBUGLVL_BLAB,("CInterruptSync::Disconnect"));

    PAGED_CODE();

    if (m_pKInterrupt) 
    {
        IoDisconnectInterrupt(m_pKInterrupt);
        m_pKInterrupt = NULL;
    }   
}

#pragma code_seg()

/*****************************************************************************
 * CInterruptSync::AddNewEntry()
 *****************************************************************************
 * SynchCritSection routine which adds a service routine to the list at DIRQL
 */

BOOLEAN CInterruptSync::AddNewEntry(PVOID pvContext)
{
    CAddEntryContext* Context = (CAddEntryContext*)pvContext;

    if(Context)
    {
        if (Context->First)
            InsertHeadList(Context->List, Context->Entry);
        else
            InsertTailList(Context->List, Context->Entry);
    }

    return TRUE;
}

/*****************************************************************************
 * CInterruptSync::RegisterServiceRoutine()
 *****************************************************************************
 * Add a service routine.
 */
STDMETHODIMP_(NTSTATUS)
CInterruptSync::
RegisterServiceRoutine
(   
    __in    PINTERRUPTSYNCROUTINE   Routine,
    __in    PVOID                   DynamicContext,
    __in    BOOLEAN                 First
)
{
    _DbgPrintF(DEBUGLVL_BLAB,("CInterruptSync::RegisterServiceRoutine"));

    ASSERT(Routine);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // ensure we really have a routine
    //
    if( !Routine )
    {
        ntStatus = STATUS_INVALID_PARAMETER;
    }

    if( NT_SUCCESS(ntStatus) )
    {
        //
        // Create a new member.
        //
        PISRLISTENTRY pIsrListEntry = new(NonPagedPoolNx,'lIcP') ISRLISTENTRY;
    
        if (pIsrListEntry)
        {
            pIsrListEntry->Routine          = Routine;
            pIsrListEntry->DynamicContext   = DynamicContext;
    
            //
            // Add the member to the list.
            //

            //  Get the spin lock to synchronize with list code that runs at
            //  dispatch level.  But to actually add the entry, you must go to
            //  DIRQL via a SynchCritSection routine, or you can get into
            //  trouble vis-a-vis the ISR.

            CAddEntryContext Context;

            Context.Entry = &pIsrListEntry->ListEntry;
            Context.List = &m_listEntry;
            Context.First = First;

            if  (m_pKInterrupt)
            {
                // The list, m_listEntry, is walked in the CInterruptSyncServiceRoutine
                // when interrupts are serviced, thus, if we add another entry, we must
                // synchronize with the interruption. Fortunately, the data that we're concerned
                // about is local so we don't have to take the spinlock unless we go into
                // the else case below.

                KeSynchronizeExecution(m_pKInterrupt, 
                    (PKSYNCHRONIZE_ROUTINE) AddNewEntry, &Context);
            }
            else
            {
                KIRQL kIrqlOld;

                KeAcquireSpinLock(&m_kSpinLock,&kIrqlOld);
                AddNewEntry(&Context);
                KeReleaseSpinLock(&m_kSpinLock,kIrqlOld);
            }
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    return ntStatus;
}

/*****************************************************************************
 * CInterruptSyncWrapperRoutine()
 *****************************************************************************
 * Wrapper for synchronized routines.
 */
static
BOOLEAN
CInterruptSyncWrapperRoutine
(
    IN      PVOID   PVoidContext
)
{
    PWRAPPERROUTINECONTEXT pContext = PWRAPPERROUTINECONTEXT(PVoidContext);

    pContext->NtStatus = pContext->Routine( PINTERRUPTSYNC(pContext->InterruptSync),
                                             pContext->DynamicContext);
    return TRUE;
}

/*****************************************************************************
 * CInterruptSync::CallSynchronizedRoutine()
 *****************************************************************************
 * Call a synchronized routine.
 */
STDMETHODIMP_(NTSTATUS)
CInterruptSync::
CallSynchronizedRoutine
(
    __in    PINTERRUPTSYNCROUTINE   Routine,
    __in    PVOID                   DynamicContext
)
{
    WRAPPERROUTINECONTEXT context;

    context.Routine        = Routine;
    context.DynamicContext = DynamicContext;
    context.InterruptSync  = this;
    context.NtStatus       = STATUS_SUCCESS;

    if (m_pKInterrupt)
    {
        if (!KeSynchronizeExecution(m_pKInterrupt,CInterruptSyncWrapperRoutine,&context ) )
        {
            context.NtStatus = STATUS_UNSUCCESSFUL;
        }
    }
    else if (KeGetCurrentIrql() <= DISPATCH_LEVEL)
    {
        _DbgPrintF(DEBUGLVL_TERSE,("Interrupt not connected yet, using spinlock"));

        KIRQL kIrqlOld;
        KeAcquireSpinLock(&m_kSpinLock,&kIrqlOld);

        //  we have no interrupt yet, so synchronize the best you can
        (void)CInterruptSyncWrapperRoutine(&context);

        KeReleaseSpinLock(&m_kSpinLock,kIrqlOld);
    }
    else
    {
        context.NtStatus = STATUS_UNSUCCESSFUL;
        _DbgPrintF(DEBUGLVL_TERSE,("Interrupt not connected yet, but IRQL > DISPATCH_LEVEL"));
    }

    return context.NtStatus;
}

/*****************************************************************************
 * CInterruptSync::GetKInterrupt()
 *****************************************************************************
 * Get a WDM InterruptObject from a portcls sync object.
 */
STDMETHODIMP_(PKINTERRUPT)
CInterruptSync::
GetKInterrupt
(   void
)
{
    return m_pKInterrupt;
}

