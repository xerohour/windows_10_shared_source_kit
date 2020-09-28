#pragma once

class Event : public IActivityId
{
public:
    Event(
        _In_ bool IsPreallocated = false,
        _In_ WFC_ACTIVITY_ID ActivityId = WFC_ACTIVITY_ID_UNKNOWN );

    NDIS_STATUS PopulateEvent(
        _In_ WFC_EVENT_TYPE EventType,
        _In_ IEventQueueCallback* pCallback,
        _In_opt_ WFC_EVENT_PAYLOAD EventPayload = NULL,
        _In_opt_ WFC_SENDER_OBJECT SenderObject = NULL,
        _In_opt_ NDIS_STATUS EventStatus = STATUS_SUCCESS );

    void set_RequiresWorkerThread(
        _In_ bool RequiresWorker );

    bool get_RequiresWorkerThread();

    void Event::set_RequiresPassiveIrql(
        _In_ bool RequiresPassiveIrql );

    bool Event::get_RequiresPassiveIrql();

    PLIST_ENTRY get_ListEntry();
    static Event* get_EventFromListEntry(
        _In_ PLIST_ENTRY pEntry );

    void IssueEventCallback();
    void SetPending();

    bool get_IsPreallocated();

protected:
    IEventQueueCallback* m_pEventQueueCallback;

    WFC_EVENT_TYPE	   m_EventType;
    WFC_SENDER_OBJECT m_SenderObject;
    WFC_EVENT_PAYLOAD m_EventPayload;
    NDIS_STATUS		  m_EventStatus;

    bool m_IsPreallocated;
    bool m_IsPending;
    bool m_RequiresWorker;
    bool m_RequiresPassiveIrql;

    CPP_LIST_ENTRY m_Link;
};

