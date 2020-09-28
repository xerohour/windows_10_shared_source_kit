#include "precomp.hpp"
#include "Event.hpp"
#include "Event.tmh"

Event::Event(
    _In_ bool IsPreallocated,
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_IsPreallocated(IsPreallocated),
    m_pEventQueueCallback(NULL),
    m_EventType(WiFiEventInvalid),
    m_EventPayload(NULL),
    m_EventStatus(NDIS_STATUS_SUCCESS),
    m_IsPending(false),
    m_RequiresWorker(false),
    m_RequiresPassiveIrql(false),
    IActivityId(ActivityId)
{
    InitializeCppListEntry(
        this,
        &m_Link );
}

PLIST_ENTRY Event::get_ListEntry()
{
    return &m_Link.ListEntry;
}

Event* Event::get_EventFromListEntry(
    _In_ PLIST_ENTRY pEntry )
{
    return (Event*) GetCppObjectFromListEntry(
        pEntry );
}

NDIS_STATUS Event::PopulateEvent(
    _In_ WFC_EVENT_TYPE EventType,
    _In_ IEventQueueCallback* pCallback,
    _In_opt_ WFC_EVENT_PAYLOAD EventPayload,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ NDIS_STATUS EventStatus )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;


    if ( EventType == WiFiEventInvalid ||
         pCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_IsPending )
    {
        WFCActError("Event is currently pending. Can't populate it while pending.");
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    m_EventType = EventType;
    m_pEventQueueCallback = pCallback;
    m_EventPayload = EventPayload;
    m_SenderObject = SenderObject;
    m_EventStatus = EventStatus;

exit:

    return status;
}

void Event::SetPending()
{
    NT_ASSERT(!m_IsPending);

    m_IsPending = true;
}

void Event::IssueEventCallback()
{

    if ( m_pEventQueueCallback == NULL )
    {
        WFCError("Bad parameter to %!FUNC!");
        NT_ASSERT(FALSE);
        goto exit;
    }

    NT_ASSERT(m_IsPending);
    m_IsPending = false;

    WFCTrace(
        "Issuing event callback Type: 0x%x, Status = 0x%x, Sender Object = 0x%p, Event Payload = %p",
        m_EventType,
        m_EventStatus,
        m_SenderObject,
        m_EventPayload );

    m_pEventQueueCallback->OnEventQueueCallback(
        m_EventType,
        m_EventStatus,
        m_SenderObject,
        m_EventPayload );

exit:

    return;
}


bool Event::get_IsPreallocated()
{
    return m_IsPreallocated;
}

void Event::set_RequiresWorkerThread(
    _In_ bool RequiresWorker )
{
    m_RequiresWorker = RequiresWorker;
}

bool Event::get_RequiresWorkerThread()
{
    return m_RequiresWorker;
}

void Event::set_RequiresPassiveIrql(
    _In_ bool RequiresPassiveIrql )
{
    m_RequiresPassiveIrql = RequiresPassiveIrql;
}

bool Event::get_RequiresPassiveIrql()
{
    return m_RequiresPassiveIrql;
}

