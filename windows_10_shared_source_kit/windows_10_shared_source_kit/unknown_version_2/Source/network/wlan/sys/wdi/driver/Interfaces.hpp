#pragma once

typedef VOID* WFC_SENDER_OBJECT;
typedef VOID* WFC_EVENT_PAYLOAD;

class INotifyOperationStarted
{
public:
    virtual void OnOperationStarted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject ) = 0;
};


class IOperationCompletionCallback
{
public:
    virtual void OnOperationCompleted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject ) = 0;
};


class ITimerCallback
{
public:
    virtual void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext ) = 0;
};

class IWorkItemCallback
{
public:
    virtual void OnWorkItemCallback(
        _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext ) = 0;
};

class IEventQueueCallback
{
public:
    virtual void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE        Type,
        _In_opt_ NDIS_STATUS        Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload ) = 0;
};

class INotifyDeviceIndicationCallback
{
public:
    virtual void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                               EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer ) = 0;
};

class INdisConversion
{
public:
    virtual NDIS_STATUS ConvertNdisPortNumberToPortId(
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _Out_ WFC_PORT_ID* pPortId ) = 0;

    virtual NDIS_STATUS ConvertPortIdToNdisPortNumber(
        _In_ WFC_PORT_ID PortId,
        _Out_ NDIS_PORT_NUMBER *pNdisPortNumber) = 0;

   virtual BOOLEAN IsOperationalPowerState()=0;
};

class CPortPropertyCache;
class CAdapterPropertyCache;

class IPropertyCacheDirectory
{
public:
    virtual CPortPropertyCache* GetPortPropertyCache(
        _In_ WFC_PORT_ID PortId
        ) = 0;

    virtual CAdapterPropertyCache* GetAdapterPropertyCache() = 0;
};


class IActivityId
{
public:
    IActivityId(
        _In_ WFC_ACTIVITY_ID ActivityId )
    {
        if ( ActivityId == WFC_ACTIVITY_ID_UNKNOWN )
        {
            ActivityId = get_NextActivityId();
        }

        m_ActivityId = ActivityId;
    }

    static WFC_ACTIVITY_ID get_NextActivityId()
    {
        static LONG s_NextActivityId = WFC_ACTIVITY_ID_UNKNOWN;

        WFC_ACTIVITY_ID ActivityId = (WFC_ACTIVITY_ID)InterlockedIncrement(
            &s_NextActivityId );

        if ( ActivityId == WFC_ACTIVITY_ID_UNKNOWN )
        { 
            ActivityId = (WFC_ACTIVITY_ID)InterlockedIncrement(
                &s_NextActivityId );
        }

        return ActivityId;
    }

    WFC_ACTIVITY_ID get_ActivityId()
    {
        return m_ActivityId;
    }

private:
    IActivityId();
    WFC_ACTIVITY_ID m_ActivityId;
};

// Forward declaration
class CBSSEntry;

class IBSSList
{
public:    
    // Returns a pointer to the BSS entry in the list
    virtual CBSSEntry* FindBSSEntry(
        _In_ const DOT11_MAC_ADDRESS& BssId) = 0;

    virtual NDIS_STATUS AddBSSEntry(
        _In_ CBSSEntry* bssEntry) = 0;

};

// Forward declaration
class CConnectedPeer;

typedef ULONG PEERLIST_ENUM_CONTEXT;

class IConnectedPeerList
{
public:
    virtual NDIS_STATUS AddPeer(
        _In_ DOT11_MAC_ADDRESS MacAddress,
        _In_ DOT11_ASSOCIATION_STATE AssociationState,
        _In_ ULONG LinkQuality
        ) = 0;

    virtual NDIS_STATUS DeletePeer(
        _In_ DOT11_MAC_ADDRESS MacAddress        
        ) = 0;

    virtual CConnectedPeer *FindPeerByAddress(
        _In_ DOT11_MAC_ADDRESS MacAddress
        ) = 0;

    virtual CConnectedPeer* FindPeerByAssociationState(
        _In_ DOT11_ASSOCIATION_STATE AssociationState
        ) = 0;

    virtual void FlushPeerList() = 0;
};

class CBSSListManager;
class INotifyBSSListChangedCallback
{
public:
    virtual void OnBSSListChanged(
        _In_ CBSSListManager* pBSSListManager ) = 0;
};

class IDisassociationCompleteCallback
{
public:
    virtual void OnDisassociationProcessed( ) = 0;
};


class IWdiMessageMemoryManager
{
public:
    virtual void FreeMemory(
        _In_ BYTE* pOutputMessageBuffer
        ) = 0;
};

