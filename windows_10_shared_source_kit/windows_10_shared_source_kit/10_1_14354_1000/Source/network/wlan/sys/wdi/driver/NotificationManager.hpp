#pragma once

class NotificationRegistration
{
public:
    NotificationRegistration(
        _In_ WDI_INDICATION_TYPE IndicationType,
        _In_ WFC_COMMAND_TOKEN CommandToken,
        _In_ WFC_PORT_ID PortId,
        _In_ INotifyDeviceIndicationCallback* pIndicationCallback );

    bool IsMatch(
        _In_ WFC_MESSAGE_METADATA* pIndicationHeader );

    PLIST_ENTRY get_ListEntry();
    static NotificationRegistration* get_NotificationRegistrationFromListEntry(
        _In_ PLIST_ENTRY pListEntry );

    NDIS_STATUS get_RegistrationToken(
        _Out_ WFC_REGISTRATION_TOKEN* pRegistrationToken );

    NDIS_STATUS get_IndicationCallback(
        _Out_ INotifyDeviceIndicationCallback** ppIndicationCallback );

protected:
    static WFC_REGISTRATION_TOKEN s_get_NextRegistrationToken();

    WFC_REGISTRATION_TOKEN          m_RegistrationToken;
    WDI_INDICATION_TYPE             m_IndicationType;
    WFC_COMMAND_TOKEN               m_CommandToken;
    WFC_PORT_ID                     m_PortId;
    INotifyDeviceIndicationCallback* m_pIndicationCallback;
    CPP_LIST_ENTRY                   m_Link;
};

class NotificationContext
{
public:
    NotificationContext(
        );
        
    ~NotificationContext();

    //
    // Buffer must be WFC_MESSAGE_METADATA
    //
    NDIS_STATUS Initialize(
        _In_ ULONG                                          IndicationEventBufferLength,
        _In_reads_bytes_(IndicationEventBufferLength) BYTE* IndicationEventBuffer,
        _In_ IWdiMessageMemoryManager*                      IndicationBufferManager,
        _In_ BOOLEAN                                        InternalEvent);

    NDIS_STATUS get_IndicationHeader(
        _Out_ WFC_MESSAGE_METADATA** ppHeader );

    void NotifyClient(
        _In_ INotifyDeviceIndicationCallback* pCallback );

protected:
    void Cleanup();

    IWdiMessageMemoryManager* m_EventBufferManager;
    ULONG                m_EventBufferLength;
    BYTE*                m_EventBuffer;
    BOOLEAN              m_bToReturnBuffer;
    BOOLEAN              m_bInternalEvent;
    
};

class NotificationManager : IEventQueueCallback
{
public:
    NotificationManager();
    ~NotificationManager();

    NDIS_STATUS Initialize(
        _In_ EventQueue* pEventQueue,
        _In_ CCtlPlane*  pControlPlane );

    NDIS_STATUS RegisterForNotifications(
        _In_ WDI_INDICATION_TYPE             IndicationType,
        _In_ WFC_COMMAND_TOKEN               CommandToken,
        _In_ WFC_PORT_ID                     PortId,
        _In_ INotifyDeviceIndicationCallback* pIndicationCallback,
        _Out_ WFC_REGISTRATION_TOKEN*        pRegistrationToken );

    NDIS_STATUS DeregisterNotificationHandler(
        _In_ WFC_REGISTRATION_TOKEN RegistrationToken );

public:
    //
    // IEventQueueCallback Implementation
    //
    virtual void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE        Type,
        _In_opt_ NDIS_STATUS        Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload );

protected:
    void OnNotifyRegisteredClientsEventHandler(
        _In_ NotificationContext* pNotificationContext );

public:
    //
    // This callback may not be serialized with the rest of the upper
    // edge. It will defer a callback into OnNotifyRegisteredClients
    // with the context information provided.
    //
    void OnDeviceIndicationArrived(
        _In_ ULONG                                EventBufferLength,
        _In_reads_bytes_(EventBufferLength) PWFC_MESSAGE_METADATA EventBuffer,
        _In_ IWdiMessageMemoryManager *           pBufferManager,
        _In_ BOOLEAN                              InternalEvent = FALSE);

    //
    // CallContext must be NotificationManager*
    //
    static void __stdcall s_OnDeviceIndicationArrived(
        _In_ PVOID Context,
        _In_ ULONG  EventBufferLength,
        _In_reads_bytes_(EventBufferLength) PWFC_MESSAGE_METADATA EventBuffer,
        _In_ IWdiMessageMemoryManager *pBufferManager );

protected:
    void Cleanup();

    CCtlPlane*  m_pControlPlane;
    EventQueue*	m_pEventQueue;
    LIST_ENTRY  m_NotificationRegistrationList;
    bool        m_IsInitialized;
};
