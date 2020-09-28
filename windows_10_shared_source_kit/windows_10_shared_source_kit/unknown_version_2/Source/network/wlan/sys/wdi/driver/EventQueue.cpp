#include "precomp.hpp"
#include "EventQueue.hpp"
#include "EventQueue.tmh"

EventQueue::EventQueue() :
    m_IsBeingProcessed(false),
    m_pEventQueueLock(NULL),
    m_pWorkItem(NULL),
    m_pCtlDevice(NULL),
    m_EventQueueThreadHandle(NULL)
{
    InitializeListHead(&m_EventQueue);
    InitializeListHead(&m_TimerRegistrations);
}

EventQueue::~EventQueue()
{
    Cleanup();
}

void EventQueue::Cleanup()
{
    if ( m_pWorkItem )
    {
        m_pWorkItem->WaitForCompletion();
    }

    NT_ASSERT(IsListEmpty(&m_EventQueue));

    if ( m_pEventQueueLock )
    {
        delete m_pEventQueueLock;
        m_pEventQueueLock = NULL;
    }

    if ( m_pWorkItem )
    {
        NT_ASSERT(!m_WorkItemQueued);
        delete m_pWorkItem;
        m_pWorkItem = NULL;
    }
}

VOID EventQueue::WaitForQueueToEmpty()
{
    ULONG waitCount = 0;

    // Outside lock
    
    // Wait till the event workitem queue is done
    if ( m_pWorkItem )
    {
        m_pWorkItem->WaitForCompletion();
    }

    // Wait for non-workitem processing to be done
    while (m_IsBeingProcessed && (waitCount < 20))
    {
        // Busy wait. Shouldn't happen normally
        NdisMSleep(1 * 500);
        waitCount++;
    }

    NT_ASSERT(m_IsBeingProcessed == 0);
    
}

NDIS_STATUS EventQueue::Initialize(
    _In_ CGlobalCtlDevice* pCtlDevice )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if ( pCtlDevice == NULL )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pCtlDevice = pCtlDevice;

    status = CNdisSpinLock::CreateInstance(
        &m_pEventQueueLock );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create event queue lock.");
        goto exit;
    }

    status = IWorkItemContext::CreateInstance(
        &m_pWorkItem );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create preallocated work item.");
        goto exit;
    }

    status = m_pWorkItem->Initialize(
        m_pCtlDevice,
        this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't initialize preallocated work item.");
        goto exit;
    }

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        Cleanup();
    }

    TraceExit(status);
    return status;
}


NDIS_STATUS EventQueue::QueueEvent(
    _In_ Event* pEvent,
    _In_opt_ bool AtHead)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool drainEventQueue = false;


    if ( pEvent == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    {
        CAutoLock lock(m_pEventQueueLock);
        PLIST_ENTRY pNewEntry = pEvent->get_ListEntry();

        pEvent->SetPending();

        if (AtHead)
        {
            InsertHeadList(
                &m_EventQueue,
                pNewEntry
                );
        }
        else
        {
            InsertTailList(
                &m_EventQueue,
                pNewEntry
                );
        }
        
        if ( !m_IsBeingProcessed )
        {
            m_EventQueueThreadHandle = CDebugMetadata::get_CurrentThreadHandle();
            m_IsBeingProcessed = true;
            drainEventQueue = true;
        }
    }

    if ( drainEventQueue )
    {
        ProcessEventQueueUntilEmpty();
    }

exit:       
    
    return status;
}

void EventQueue::ProcessEventQueueUntilEmpty()
{
    Event* pNextEvent;

    do
    {
        bool isPreallocated = true;

        {
            CAutoLock lock(m_pEventQueueLock);
            pNextEvent = DequeueEvent();

            if ( pNextEvent == NULL )
            {
                WFCTrace("No more events to process.");
                m_WorkItemQueued = false;
                m_EventQueueThreadHandle = NULL;
                m_IsBeingProcessed = false;
                goto exit;
            }

            if ( !m_WorkItemQueued  && 
                (pNextEvent->get_RequiresWorkerThread() ||
                ( pNextEvent->get_RequiresPassiveIrql() && KeGetCurrentIrql()>= DISPATCH_LEVEL )))
            {
                WFCTrace("Context switch required, queueing work item.");

                InsertHeadList(
                    &m_EventQueue,
                    pNextEvent->get_ListEntry() );

                //
                // Note: m_IsBeingProcessed remains true to prevent
                // other queuers from taking over the event queue.
                //
                m_WorkItemQueued = true;
                m_EventQueueThreadHandle = NULL;

                m_pWorkItem->QueueWorkItem();

                goto exit;
            }
        }

        // For IRQL test, Raise IRQL if (!pNextEvent->get_RequiresWorkerThread() && !pNextEvent->get_RequiresPassiveIrql())
        isPreallocated = pNextEvent->get_IsPreallocated();
        
        pNextEvent->IssueEventCallback();

        if ( !isPreallocated )
        {
            delete pNextEvent;
        }
    }
    while( pNextEvent != NULL );

exit:
    return;
}


Event* EventQueue::DequeueEvent()
{
    PLIST_ENTRY pEntry = NULL;
    Event* pEvent = NULL;

    if ( IsListEmpty(&m_EventQueue) )
    {
        goto exit;
    }

    pEntry = RemoveHeadList(
        &m_EventQueue );

    // Reinitialize the list entry so it can be re-populated.
    InitializeListHead(
        pEntry );

    pEvent = Event::get_EventFromListEntry(
        pEntry );

exit:
    return pEvent;
}

NDIS_STATUS EventQueue::PopulateAndQueueDeferredCallback(
    _In_ WFC_EVENT_TYPE EventType,
    _In_ WFC_EVENT_PAYLOAD pDeferredCallback,
    _In_ WFC_SENDER_OBJECT SenderObject,
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ Event* pTargetEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool freeEvent = false;


    if ( pDeferredCallback == NULL &&
         SenderObject == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( pTargetEvent == NULL )
    {
        //
        // No preallocated event given, we need to try to create one.
        //

        pTargetEvent = new Event();

        if ( pTargetEvent == NULL )
        {
            status = NDIS_STATUS_RESOURCES;
            goto exit;
        }

        freeEvent = true;
    }

    // If this is a DeferredTimerCallback, then we must require a worker thread for this
    NT_ASSERT((EventType != WiFiEventDeferredTimerCallback) ||
              ((pTargetEvent != NULL) && (pTargetEvent->get_RequiresWorkerThread())));

    status = pTargetEvent->PopulateEvent(
        EventType,
        (IEventQueueCallback*)this,
        pDeferredCallback,
        SenderObject,
        OperationStatus );

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

    status = QueueEvent(
        pTargetEvent );

exit:

    if ( status != STATUS_SUCCESS )
    {
        if ( pTargetEvent != NULL &&
             freeEvent )
        {
            delete pTargetEvent;
        }
    }

    return status;
}

NDIS_STATUS EventQueue::DeferNotifyOperationStarted(
    _In_ INotifyOperationStarted* pOperationStarted,
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ Event* pTargetEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    status = PopulateAndQueueDeferredCallback(
        WiFiEventDeferredNotifyOperationStarted,
        (WFC_EVENT_PAYLOAD)pOperationStarted,
        SenderObject,
        OperationStatus,
        pTargetEvent );

    TraceExit(status);
    return status;
}


NDIS_STATUS EventQueue::DeferOperationCompletion(
    _In_ IOperationCompletionCallback* pCompletionCallback,
    _In_opt_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ Event* pTargetEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    status = PopulateAndQueueDeferredCallback(
        WiFiEventDeferredOperationCompletion,
        (WFC_EVENT_PAYLOAD)pCompletionCallback,
        SenderObject,
        OperationStatus,
        pTargetEvent );

    return status;
}

NDIS_STATUS EventQueue::DeferToWorkItem(
    _In_ IOperationCompletionCallback* pCompletionCallback,
    _In_ Event* pEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    pEvent->set_RequiresWorkerThread(true);

    status = PopulateAndQueueDeferredCallback(
        WiFiEventDeferredThreadpoolCallback,
        (WFC_EVENT_PAYLOAD)pCompletionCallback,
        NULL,
        NDIS_STATUS_SUCCESS,
        pEvent );

    TraceExit(status);

    return status;
}

void EventQueue::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE Type,
    _In_opt_ NDIS_STATUS Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    switch(Type)
    {
    case WiFiEventDeferredOperationCompletion:
    case WiFiEventDeferredThreadpoolCallback:
    {
        NT_ASSERT(Payload != NULL);

        IOperationCompletionCallback* pCallback = (IOperationCompletionCallback*)Payload;

        pCallback->OnOperationCompleted(
            Status,
            SenderObject );
    }
    break;

    case WiFiEventDeferredNotifyOperationStarted:
    {
        NT_ASSERT(Payload != NULL);

        INotifyOperationStarted* pCallback = (INotifyOperationStarted*)Payload;

        pCallback->OnOperationStarted(
            Status,
            SenderObject );
    }
    break;

    case WiFiEventDeferredTimerCallback:
    {
        NT_ASSERT(SenderObject != NULL);

        TimerRegistrationContext* pRegistrationContext =
            (TimerRegistrationContext*)SenderObject;

        HandleTimerCallback(
            pRegistrationContext );
    }
    break;

    default:
        NT_ASSERT(FALSE);

        break;
    };
}


HANDLE EventQueue::get_ActiveThreadHandle()
{
    return m_EventQueueThreadHandle;
}

NDIS_STATUS EventQueue::DeferTimeoutCallback(
    _In_ TimerRegistrationContext* pRegistrationContext,
    _In_opt_ Event* pTargetEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if ( pRegistrationContext == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("Bad param to %!FUNC!");
        goto exit;
    }

    status = PopulateAndQueueDeferredCallback(
        WiFiEventDeferredTimerCallback,
        NULL,
        (WFC_SENDER_OBJECT)pRegistrationContext,
        NDIS_STATUS_SUCCESS,
        pTargetEvent );

exit:
    TraceExit(status);
    return status;
}

void EventQueue::HandleTimerCallback(
    _In_ TimerRegistrationContext* pRegistrationContext )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if ( pRegistrationContext == NULL )
    {
        WFCError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;

        NT_ASSERT(FALSE);
        goto exit;
    }

    //
    // This registration context may have been deregistered. Check
    // before attempting to issue callback.
    //
    if ( pRegistrationContext->get_RegistrationToken() !=
         WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        pRegistrationContext->IssueCallback();

        //
        // Always clean up on timer callback only if its not a reuse timer or the timer is already unregistered
        //
        if (pRegistrationContext->IsReuseTimer()) goto exit;
    }

    RemoveEntryList(pRegistrationContext->get_ListEntry());
    delete pRegistrationContext;
    
exit:
    TraceExit(status);
}

NDIS_STATUS EventQueue::RegisterTimeoutCallback(
    _In_ WFC_ACTIVITY_ID ActivityId,
    _In_ ITimerCallback* pTimerCallback,
    _In_ ULONG RelativeTimeoutMs,
    _In_opt_ WFC_TIMER_CONTEXT TimerContext,
    _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken )
{
    NDIS_STATUS status = RegisterTimeoutCallbackWithLevelAndReuse(
            ActivityId,
            pTimerCallback,
            RelativeTimeoutMs,
            TimerContext,
            WdfExecutionLevelPassive,
            false,
            nullptr,
            pRegistrationToken );


    return status;
}

NDIS_STATUS EventQueue::RegisterTimeoutCallbackWithLevelAndReuse(
    _In_ WFC_ACTIVITY_ID ActivityId,
    _In_ ITimerCallback* pTimerCallback,
    _In_ ULONG RelativeTimeoutMs,
    _In_opt_ WFC_TIMER_CONTEXT TimerContext,
    _In_ WDF_EXECUTION_LEVEL level,
    _In_ bool bReuseTimer,
    _Out_ TimerRegistrationContext** ppRegistrationContext,
    _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TimerRegistrationContext* pNewRegistration = NULL;

    TraceEntry();

    if ( pTimerCallback == NULL ||
         pRegistrationToken == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("Bad parameter to %!FUNC!");
        goto exit;
    }

    pNewRegistration = new TimerRegistrationContext(ActivityId);

    if ( pNewRegistration == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCError("Couldn't allocate timer registration context.");
        goto exit;
    }

    //
    // This also sets the timer to fire if bReuseTimer is false
    //
    status = pNewRegistration->Initialize(
        this,
        m_pCtlDevice,
        pTimerCallback,
        RelativeTimeoutMs,
        TimerContext,
        level,
        bReuseTimer,
        pRegistrationToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't initialize/start timer.");
        goto exit;
    }

    InsertTailList(
        &m_TimerRegistrations,
        pNewRegistration->get_ListEntry() );

    if (ppRegistrationContext != nullptr)
    {
        *ppRegistrationContext = pNewRegistration;
    }

    pNewRegistration = NULL;
exit:

    if ( pNewRegistration != NULL )
    {
        delete pNewRegistration;
    }

    TraceExit(status);
    return status;
}

NDIS_STATUS EventQueue::StartTimer(
    _In_ TimerRegistrationContext *pTimerContext,
    _In_ ULONG RelativeTimeoutInMs
    )
{
    TraceEntry();
    NDIS_STATUS status;

    #if 0 // control by completion count instead
    //
    // Firmware hang injection if configured. This covers task and command
    //
    if ( m_HungResetRecoveryIterations )
    {
        if ( ++m_CountCommandsToLe == m_HungAtWdiCommandPasses )
        {
            //
            // inject hang by 0 ms timeout.
            //
            RelativeTimeoutInMs = 0;
            WFCInfo( "Simulate Firmware hang by 0 ms timeout" );
        }
    }
    #endif // 0

    //DbgPrintEx( 0, 0, "EventQ start timer to=%dms\n", RelativeTimeoutInMs);
    status = pTimerContext->StartTimer(RelativeTimeoutInMs);
    TraceExit(status);
    return status;
}

void EventQueue::StopTimer(
    _In_ TimerRegistrationContext *pTimerContext
    )
{
    TraceEntry();
    pTimerContext->StopTimer( false);
    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS EventQueue::DeregisterTimeoutCallback(
    _In_ WFC_TIMER_REGISTRATION_TOKEN RegistrationToken )
{
    NDIS_STATUS status = NDIS_STATUS_FILE_NOT_FOUND;
    PLIST_ENTRY pCurrentEntry = m_TimerRegistrations.Flink;
    TraceEntry();

    if ( RegistrationToken == WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_PARAMETER;

        WFCError("Passed in invalid registration token to deregister.");

        goto exit;
    }

    while( pCurrentEntry != &m_TimerRegistrations )
    {
        TimerRegistrationContext* pCurrentRegistration =
            TimerRegistrationContext::get_FromListEntry(
                pCurrentEntry );

        if ( pCurrentRegistration->get_RegistrationToken() == RegistrationToken )
        {

            //
            // Make sure no more callbacks get issued.
            //
            pCurrentRegistration->set_RegistrationToken(
                WFC_TIMER_REGISTRATION_TOKEN_INVALID );

            //
            // Regardless of whether it's in flight or gets removed
            // from the timer queue, the registration object will
            // trigger a deferred timeout callback completion. This
            // completion will trigger cleanup paths for the timer.
            //
            pCurrentRegistration->StopTimer(true);
            status = NDIS_STATUS_SUCCESS;

            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

exit:
    TraceExit(status);
    return status;
}

void EventQueue::OnWorkItemCallback(
    _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext )
{
    UNREFERENCED_PARAMETER(WorkItemContext);

    NT_ASSERT(m_WorkItemQueued);

    TraceEntry();

    m_EventQueueThreadHandle = CDebugMetadata::get_CurrentThreadHandle();
    ProcessEventQueueUntilEmpty();

    TraceExit(NDIS_STATUS_SUCCESS);
}
