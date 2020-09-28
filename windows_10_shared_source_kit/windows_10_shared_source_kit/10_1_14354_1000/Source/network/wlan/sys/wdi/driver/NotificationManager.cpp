#include "precomp.hpp"
#include "NotificationManager.hpp"
#include "NotificationManager.tmh"
#include "ctlPlane.hpp"

volatile LONG s_CurrentRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;

//
// Begin NotificationRegistration Implementation
//

NotificationRegistration::NotificationRegistration(
    _In_ WDI_INDICATION_TYPE IndicationType,
    _In_ WFC_COMMAND_TOKEN   CommandToken,
    _In_ WFC_PORT_ID         PortId,
    _In_ INotifyDeviceIndicationCallback* pIndicationCallback ) :
    m_IndicationType(IndicationType),
    m_CommandToken(CommandToken),
    m_PortId(PortId),
    m_pIndicationCallback(pIndicationCallback)
{
    InitializeCppListEntry(
        this,
        &m_Link );

    m_RegistrationToken = s_get_NextRegistrationToken();
}


bool NotificationRegistration::IsMatch(
    _In_ WFC_MESSAGE_METADATA* pIndicationHeader )
{
    bool isMatch = true;

    if ( m_IndicationType != WiFiIndicationInvalid &&
         pIndicationHeader->MessageId != (UINT32)m_IndicationType )
    {
        isMatch = false;
    }

    if ( pIndicationHeader->Message.TransactionId != m_CommandToken )
    {
        isMatch = false;
    }

    if ( m_PortId != WDI_PORT_ID_ADAPTER &&
         pIndicationHeader->Message.PortId != m_PortId )
    {
        isMatch = false;
    }

    return isMatch;
}


PLIST_ENTRY NotificationRegistration::get_ListEntry()
{
    return &m_Link.ListEntry;
}


NotificationRegistration* NotificationRegistration::get_NotificationRegistrationFromListEntry(
    _In_ PLIST_ENTRY pListEntry )
{
    return (NotificationRegistration*) GetCppObjectFromListEntry(
        pListEntry );
}

NDIS_STATUS NotificationRegistration::get_RegistrationToken(
    _Out_ WFC_REGISTRATION_TOKEN* pRegistrationToken )
{
    *pRegistrationToken = m_RegistrationToken;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS NotificationRegistration::get_IndicationCallback(
    _Out_ INotifyDeviceIndicationCallback** ppIndicationCallback )
{
    *ppIndicationCallback = m_pIndicationCallback;
    return NDIS_STATUS_SUCCESS;
}

WFC_REGISTRATION_TOKEN NotificationRegistration::s_get_NextRegistrationToken()
{
    LONG nextToken;

    nextToken = InterlockedIncrement(
        &s_CurrentRegistrationToken );

    if ( nextToken == WFC_REGISTRATION_TOKEN_INVALID )
    {
        nextToken = InterlockedIncrement(
            &s_CurrentRegistrationToken );
    }

    return (WFC_REGISTRATION_TOKEN)nextToken;
}

//
// End NotificationRegistration Implementation
//

//
// Begin NotificationContext Implementation
//

NotificationContext::NotificationContext() :
    m_EventBufferLength(0),
    m_EventBuffer(NULL),
    m_bToReturnBuffer(false),
    m_EventBufferManager(NULL),
    m_bInternalEvent(false)
{
    //
    // No Extra Construction Necessary
    //
}


NotificationContext::~NotificationContext()
{
    Cleanup();
}


NDIS_STATUS NotificationContext::Initialize(
    _In_ ULONG                                          IndicationEventBufferLength,
    _In_reads_bytes_(IndicationEventBufferLength) BYTE* IndicationEventBuffer,
    _In_ IWdiMessageMemoryManager*                      IndicationBufferManager,
    _In_ BOOLEAN                                        InternalEvent)
{
    NDIS_STATUS status = STATUS_SUCCESS;
    TraceEntry();

    if ( IndicationEventBuffer == NULL ||
         IndicationEventBufferLength < sizeof(WFC_MESSAGE_METADATA) ||
         (!InternalEvent && (IndicationBufferManager == NULL)) )
    {
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // clean up in case it is being reused.
    //
    Cleanup();

    m_EventBufferLength = IndicationEventBufferLength;
    m_EventBuffer = IndicationEventBuffer;
    m_EventBufferManager = IndicationBufferManager;
    m_bToReturnBuffer = TRUE;
    m_bInternalEvent = InternalEvent;

exit:

    if ( status != STATUS_SUCCESS )
    {
        Cleanup();
    }

    TraceExit(status);
    return status;
}


NDIS_STATUS NotificationContext::get_IndicationHeader(
    _Out_ WFC_MESSAGE_METADATA** ppHeader )
{
    NDIS_STATUS status = STATUS_SUCCESS;

    TraceEntry();

    if ( ppHeader == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_EventBuffer == NULL )
    {
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    *ppHeader = (WFC_MESSAGE_METADATA*)m_EventBuffer;

exit:

    TraceExit(status);
    return status;
}

void NotificationContext::NotifyClient(
    _In_ INotifyDeviceIndicationCallback* pCallback )
{
    WFC_MESSAGE_METADATA* pIndicationHeader;

    if ( pCallback == NULL ||
         m_EventBuffer == NULL )
    {
        NT_ASSERT(FALSE);
        return;
    }

    pIndicationHeader = (WFC_MESSAGE_METADATA*)m_EventBuffer;

    pCallback->OnDeviceIndicationArrived(
        (WDI_INDICATION_TYPE)pIndicationHeader->MessageId,
        (WFC_COMMAND_TOKEN)pIndicationHeader->Message.TransactionId,
        (WFC_PORT_ID)pIndicationHeader->Message.PortId,
        m_EventBufferLength,
        m_EventBuffer );

}

void NotificationContext::Cleanup()
{
    if ( m_bToReturnBuffer )
    {
        if (m_bInternalEvent)
        {
            delete[] m_EventBuffer;
        }
        else
        {
            NT_ASSERT(m_EventBufferManager);

            m_EventBufferManager->FreeMemory(
                m_EventBuffer );
        }
        m_EventBuffer = NULL;
        m_EventBufferLength = 0;
        m_EventBufferManager = NULL;
        m_bToReturnBuffer = FALSE;
    }
}

//
// End NotificationContext Implementation
//

//
// Begin NotificationManager Implementation
//

NotificationManager::NotificationManager() :
    m_pControlPlane(NULL),
    m_pEventQueue(NULL),
    m_IsInitialized(false)
{
    InitializeListHead(
        &m_NotificationRegistrationList );
}

NotificationManager::~NotificationManager()
{
    Cleanup();
}

void NotificationManager::Cleanup()
{
    PLIST_ENTRY pCurrentEntry = m_NotificationRegistrationList.Flink;

    while( pCurrentEntry != &m_NotificationRegistrationList )
    {
        NotificationRegistration* pCurrentRegistration =
            NotificationRegistration::get_NotificationRegistrationFromListEntry(
                pCurrentEntry );

        pCurrentEntry = pCurrentEntry->Flink;

        RemoveEntryList(
            pCurrentRegistration->get_ListEntry() );
        delete pCurrentRegistration;
    }

    if ( m_pControlPlane != NULL )
    {
        (VOID)m_pControlPlane->DeregisterIndicationCallback(
            &s_OnDeviceIndicationArrived,
            this );
        m_pControlPlane = NULL;
    }
}


NDIS_STATUS NotificationManager::Initialize(
    _In_ EventQueue* pEventQueue,
    _In_ CCtlPlane*  pControlPlane )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NTSTATUS    ctlPlaneStatus;

    TraceEntry();

    if ( pEventQueue == NULL ||
         pControlPlane == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_IsInitialized )
    {
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    ctlPlaneStatus = pControlPlane->RegisterIndicationCallback(
        &s_OnDeviceIndicationArrived,
        this );

    status = NTSTATUS2NDISSTATUS(ctlPlaneStatus);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    m_pControlPlane = pControlPlane;
    m_pEventQueue = pEventQueue;

exit:

    if ( status != NDIS_STATUS_SUCCESS )
    {
        Cleanup();
    }

    TraceExit(status);
    return status;
}


NDIS_STATUS NotificationManager::RegisterForNotifications(
    _In_ WDI_INDICATION_TYPE             IndicationType,
    _In_ WFC_COMMAND_TOKEN               CommandToken,
    _In_ WFC_PORT_ID                     PortId,
    _In_ INotifyDeviceIndicationCallback* pIndicationCallback,
    _Out_ WFC_REGISTRATION_TOKEN*        pRegistrationToken )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NotificationRegistration* pNewRegistration = NULL;

    TraceEntry();

    if ( pIndicationCallback == NULL ||
         pRegistrationToken == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    pNewRegistration = new NotificationRegistration(
        IndicationType,
        CommandToken,
        PortId,
        pIndicationCallback );

    if ( pNewRegistration == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    status = pNewRegistration->get_RegistrationToken(
        pRegistrationToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    InsertTailList(
        &m_NotificationRegistrationList,
        pNewRegistration->get_ListEntry() );

    pNewRegistration = NULL;

exit:

    if ( pNewRegistration != NULL )
    {
        delete pNewRegistration;
    }

    TraceExit(status);
    return status;
}


NDIS_STATUS NotificationManager::DeregisterNotificationHandler(
    _In_ WFC_REGISTRATION_TOKEN RegistrationToken )
{
    NDIS_STATUS status = NDIS_STATUS_FILE_NOT_FOUND;
    PLIST_ENTRY pCurrentEntry = m_NotificationRegistrationList.Flink;

    TraceEntry();

    while( pCurrentEntry != &m_NotificationRegistrationList )
    {
        WFC_REGISTRATION_TOKEN currentToken;
        NotificationRegistration* pCurrentRegistration =
            NotificationRegistration::get_NotificationRegistrationFromListEntry(
                pCurrentEntry );

        pCurrentEntry = pCurrentEntry->Flink;

        if ( pCurrentRegistration->get_RegistrationToken(
                 &currentToken ) == NDIS_STATUS_SUCCESS &&
            currentToken == RegistrationToken )
        {
            RemoveEntryList(
                pCurrentRegistration->get_ListEntry() );
            delete pCurrentRegistration;

            status = NDIS_STATUS_SUCCESS;
            goto exit;
        }
    }

exit:

    TraceExit(status);
    return status;
}


//
// IEventQueueCallback Implementation
//
void NotificationManager::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE        Type,
    _In_opt_ NDIS_STATUS        Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    UNREFERENCED_PARAMETER(Status);
    UNREFERENCED_PARAMETER(SenderObject);

    switch(Type)
    {
    case WiFiEventNotificationManagerNotifyRegisteredClients:

        NT_ASSERT(Payload != NULL);
        
        OnNotifyRegisteredClientsEventHandler(
            (NotificationContext*)Payload );
    break;

    default:
        NT_ASSERT(FALSE);
        break;
    };
}

void NotificationManager::OnNotifyRegisteredClientsEventHandler(
    _In_ NotificationContext* pNotificationContext )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY pCurrentEntry = m_NotificationRegistrationList.Flink;
    WFC_MESSAGE_METADATA* pIndicationHeader;

    TraceEntry();

    if ( pNotificationContext == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = pNotificationContext->get_IndicationHeader(
        &pIndicationHeader );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    //
    // Notify all matching registrations inline
    //
    while( pCurrentEntry != &m_NotificationRegistrationList )
    {
        INotifyDeviceIndicationCallback* pIndicationCallback;
        NotificationRegistration* pCurrentRegistration =
            NotificationRegistration::get_NotificationRegistrationFromListEntry(
                pCurrentEntry );

        pCurrentEntry = pCurrentEntry->Flink;

        if ( pCurrentRegistration->IsMatch(
                 pIndicationHeader ) &&
             pCurrentRegistration->get_IndicationCallback(
                 &pIndicationCallback) == NDIS_STATUS_SUCCESS )
        {
            pNotificationContext->NotifyClient(
                pIndicationCallback );
        }
    }


exit:

    if ( pNotificationContext ) 
    {
        delete( pNotificationContext );
    }

    TraceExit(status);
}


//
// This callback may not be serialized with the rest of the upper
// edge. It will defer a callback into OnNotifyRegisteredClients
// with the context information provided.
//
void NotificationManager::OnDeviceIndicationArrived(
    _In_ ULONG  EventBufferLength,
    _In_reads_bytes_(EventBufferLength) PWFC_MESSAGE_METADATA EventBuffer,
    _In_ IWdiMessageMemoryManager *           pBufferManager,
    _In_ BOOLEAN                              InternalEvent)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NotificationContext* pNotificationContext = NULL;
    Event* pNotificationEvent = NULL;
    BOOLEAN bToReturnEventBuffer = TRUE;

    TraceEntry();

    pNotificationContext = new NotificationContext();
    pNotificationEvent = new Event();

    if ( pNotificationContext == NULL ||
         pNotificationEvent == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    status = pNotificationContext->Initialize(
        EventBufferLength,
        (BYTE *)EventBuffer,
        pBufferManager,
        InternalEvent);

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

    bToReturnEventBuffer = FALSE;
    status = pNotificationEvent->PopulateEvent(
        WiFiEventNotificationManagerNotifyRegisteredClients,
        (IEventQueueCallback*)this,
        pNotificationContext );

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

    // If this is an unsolicited event and any consumer needs this event 
    // to be processed at passive, lets do it. We dont check IRQL yet since the
    // event maybe get dequeued by a different thread
    if ((EventBuffer->Message.TransactionId == 0) && 
            (
                (EventBuffer->MessageId == WDI_INDICATION_RADIO_STATUS) ||      // Accesses registry
                (EventBuffer->MessageId == WDI_INDICATION_BSS_ENTRY_LIST)  ||   // Calls RtlUTF8ToUnicode API
                (EventBuffer->MessageId == WDI_INDICATION_FIRMWARE_STALLED)     // Accesses registry
            )
        )
    {
        // Require passive level for this event
        pNotificationEvent->set_RequiresPassiveIrql(true);
    }

    status = m_pEventQueue->QueueEvent(
        pNotificationEvent );

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

    pNotificationContext = NULL;
    pNotificationEvent = NULL;

exit:

    if ( bToReturnEventBuffer ) 
    {
        pBufferManager->FreeMemory(
            (BYTE *)EventBuffer);
    }

    if ( pNotificationContext != NULL )
    {
        delete pNotificationContext;
    }

    if ( pNotificationEvent != NULL )
    {
        delete pNotificationEvent;
    }

    TraceExit(status);
}

//
// CallContext must be NotificationManager*
//
void NotificationManager::s_OnDeviceIndicationArrived(
    _In_ PVOID Context,
    _In_ ULONG  EventBufferLength,
    _In_reads_bytes_(EventBufferLength) PWFC_MESSAGE_METADATA EventBuffer, // Caller owns memory
    _In_ IWdiMessageMemoryManager *pBufferManager )
{
    NT_ASSERT(Context != NULL);

    if ( Context != NULL )
    {
        ((NotificationManager*)Context)->OnDeviceIndicationArrived(
            EventBufferLength,
            EventBuffer,
            pBufferManager);
    }
}

//
// End NotificationManager Implementation
//
