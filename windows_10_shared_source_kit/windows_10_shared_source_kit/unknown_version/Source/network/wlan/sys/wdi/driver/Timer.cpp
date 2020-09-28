#include "precomp.hpp"
#include "Timer.tmh"


TimerRegistrationContext::TimerRegistrationContext(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_RelativeTimeoutMs(NULL),
    m_pTimerCallback(NULL),
    m_pTimer(NULL),
    m_TimerContext(NULL),
    m_RegistrationToken(NULL),
    m_FailsafeTimerFiredEvent(true, ActivityId),
    IActivityId(ActivityId),
    m_pCtlDevice(NULL),
    m_bReuseTimer(false)
{
    InitializeCppListEntry(
        this,
        &m_Link );
}

TimerRegistrationContext::~TimerRegistrationContext()
{
    if ( m_pTimer != NULL )
    {
        delete m_pTimer;
        m_pTimer = NULL;
    }    
}

WFC_TIMER_REGISTRATION_TOKEN TimerRegistrationContext::get_NextRegistrationToken()
{
    static LONG s_NextToken =
        WFC_TIMER_REGISTRATION_TOKEN_INVALID;

    WFC_TIMER_REGISTRATION_TOKEN nextToken =
        (WFC_TIMER_REGISTRATION_TOKEN)InterlockedIncrement(
            &s_NextToken );

    if ( nextToken == WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        nextToken = (WFC_TIMER_REGISTRATION_TOKEN)InterlockedIncrement(
            &s_NextToken );
    }

    return nextToken;
}

NDIS_STATUS TimerRegistrationContext::Initialize(
    _In_ EventQueue* pEventQueue,
    _In_ CGlobalCtlDevice* pCtlDevice,
    _In_ ITimerCallback* pTimerCallback,
    _In_ ULONG RelativeTimeoutMs,
    _In_opt_ WFC_TIMER_CONTEXT TimerContext,
    _In_ WDF_EXECUTION_LEVEL level,
    _In_ bool bReuseTimer,
    _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pEventQueue == NULL ||
         pCtlDevice == NULL ||
         pTimerCallback == NULL ||
         pRegistrationToken == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        WFCActErrorStatus("Bad parameter to %!FUNC!");
        goto exit;
    }

    if ( m_State != TimerRegistrationStateInvalid &&
         m_State != TimerRegistrationStateDone )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_pTimer != NULL )
    {
        delete m_pTimer;
        m_pTimer = NULL;
    }

    status = ITimerContext::CreateInstance(
        &m_pTimer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't create timer instance.");
        goto exit;
    }

    status = m_pTimer->Initialize(
        pCtlDevice,
        this,
        level);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialized timer.");
        goto exit;
    }

    m_pEventQueue = pEventQueue;
    m_TimerContext = TimerContext;
    m_pTimerCallback = pTimerCallback;
    m_State = TimerRegistrationStateInit;
    m_bReuseTimer = bReuseTimer;

    m_RegistrationToken = get_NextRegistrationToken();
    *pRegistrationToken = m_RegistrationToken;

    if (bReuseTimer == false)
    {
        m_State = TimerRegistrationStateTimerSet;
        status = m_pTimer->SetTimer(
            RelativeTimeoutMs,
            m_TimerContext );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't set timer.");

            m_State = TimerRegistrationStateInvalid;
            delete m_pTimer;
            m_pTimer = NULL;

            goto exit;
        }

        WFCActTrace(
            "Queued timer to fire in %u milliseconds",
            RelativeTimeoutMs );
    }
    else
    {
        m_State = TimerRegistrationStateDone;
    }

exit:

    TraceActExit(status);
    return status;
}

NDIS_STATUS TimerRegistrationContext::StartTimer(
    _In_ ULONG RelativeTimeoutMs
    )
{

    NDIS_STATUS status;

    if (m_bReuseTimer == false) 
    {
        status = NDIS_STATUS_FAILURE;
    }
    else
    {
        status = m_pTimer->SetTimer(
            RelativeTimeoutMs,
            m_TimerContext );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't set timer.");

        } 
        else
        {
            m_State = TimerRegistrationStateTimerSet;
            WFCActTrace(
                "Queued timer to fire in %u milliseconds",
                RelativeTimeoutMs );
        }

    }
    return status;
}


void TimerRegistrationContext::StopTimer(bool fromUnregister)
{
    ASSERT((fromUnregister == true) || (m_bReuseTimer == true)); 

    // for a reuse timer during unregistering, we should just free the timer registration data.
    // if we have previously successfully stopped the timer or the timer already ran and 
    // we enter this routine from the deregister timer call.
    // We just make fake a callback to free the timer registration
    if((fromUnregister == true) && (m_bReuseTimer == true) && (m_State == TimerRegistrationStateDone))
    {
            OnTimerCallback(NULL);
    }
    else
    {
        if ( m_pTimer->CancelTimer() )
        {
            // if the stop is called from unregiser, we make a call to timercallback so that 
            // the registration gets deleted from the timer callback.
            if (fromUnregister == true)
            {
                WFCActTrace(
                    "Timer got removed from timer queue. Triggering timer firing." );
                OnTimerCallback(NULL);
            }
            else
            {
                m_State = TimerRegistrationStateDone;
            }
        }
        else
        {
            NT_ASSERT(
                m_State > TimerRegistrationStateInit );

            WFCActTrace(
                "Timer is already in flight. Waiting for callback." );
        }
    }
}

void TimerRegistrationContext::OnTimerCallback(
    _In_ WFC_TIMER_CONTEXT )
{
    Event* pEvent = NULL;

    TraceEntry();

    //
    // Reuse timers may have multiple callbacks pending in the event
    // queue at the same time. Given this possibility, in the reuse
    // timer case, this function will allocate a new event object for
    // each notification, and will only use the failsafe event if it
    // can't allocate a new one.
    //
    // One shot timers can have at most 1 callback pending at a time,
    // so use of the failsafe instead of allocating is allowed.
    //
    if ( m_bReuseTimer )
    {
        pEvent = new Event(false, get_ActivityId());

        if ( pEvent == NULL )
        {
            WFCActInfo(
                "Couldn't allocate new event to complete re-use timer callback. Using failsafe." );

            pEvent = &m_FailsafeTimerFiredEvent;
        }
    }
    else
    {
        pEvent = &m_FailsafeTimerFiredEvent;
    }

    pEvent->set_RequiresWorkerThread(true);

    NDIS_STATUS status = m_pEventQueue->DeferTimeoutCallback(
        this,
        pEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    pEvent = NULL;

exit:
    if ( pEvent != NULL &&
         !pEvent->get_IsPreallocated() )
    {
        delete pEvent;
    }

    TraceExit(status);
}

PLIST_ENTRY TimerRegistrationContext::get_ListEntry()
{
    return &m_Link.ListEntry;
}

TimerRegistrationContext* TimerRegistrationContext::get_FromListEntry(
    _In_ PLIST_ENTRY pListEntry )
{
    return (TimerRegistrationContext*) GetCppObjectFromListEntry(
        pListEntry );
}

void TimerRegistrationContext::IssueCallback()
{
    NT_ASSERT(
        m_pTimerCallback != NULL &&
        m_RegistrationToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID &&
        m_State == TimerRegistrationStateTimerSet );

    if ( m_pTimerCallback )
    {
        m_State = TimerRegistrationStateDone;
        m_pTimerCallback->OnTimerCallback(
            m_TimerContext );
    }
}

WFC_TIMER_REGISTRATION_TOKEN TimerRegistrationContext::get_RegistrationToken()
{
    return m_RegistrationToken;
}

void TimerRegistrationContext::set_RegistrationToken(
    _In_ WFC_TIMER_REGISTRATION_TOKEN RegistrationToken )
{
    m_RegistrationToken = RegistrationToken;
}
