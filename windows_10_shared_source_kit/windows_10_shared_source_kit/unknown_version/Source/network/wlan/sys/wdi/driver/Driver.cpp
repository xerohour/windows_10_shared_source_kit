#include "precomp.hpp"
#include "driver.tmh"

#if DBG // VS2012 appears to have a bug that in free build __DATE__ and __TIME__ are undefined
char* g_BuildDateTime = __DATE__ " " __TIME__;
#endif

// The global wdi miniport driver object
CWdiDriver* g_pWdiDriver = NULL;


CWdiDriver::CWdiDriver() : 
    m_DebugLastCreateAdapter(NULL)
{
    InitializeListHead(
        &m_MiniportDriverList );
    InitializeListHead(
        &m_AdapterList );

    m_MiniportContextLock = NULL;
}

CWdiDriver::~CWdiDriver()
{
    m_NdisHook.Cleanup();
    
    if (m_MiniportContextLock)
    {   
        delete m_MiniportContextLock;
        m_MiniportContextLock = NULL;
    }
}

NTSTATUS CWdiDriver::Initialize()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NTSTATUS ntStatus;
    bool ndisHookInitialized = false;

    TraceEntry();

    // Initialize the hook to NDIS
    ntStatus = m_NdisHook.Initialize(this);
    if (!NT_SUCCESS(ntStatus))
    {
        WFCError("NdisHook Initialization failed (0x%x)", ntStatus);
        goto exit;
    }    

    ntStatus = m_CtlDevice.Initialize();
    if (!NT_SUCCESS(ntStatus))
    {
        WFCError("CtlDevice Initialization failed (0x%x)", ntStatus);
        goto exit;
    }    

    ndisStatus = CNdisSpinLock::CreateInstance(&m_MiniportContextLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to allocate lock for Miniport Driver list. Error = 0x%08x", ndisStatus);
        ntStatus = NDISSTATUS2NTSTATUS(ndisStatus);        
        goto exit;    
    }    

exit:
    if (!NT_SUCCESS(ntStatus))
    {
        if (ndisHookInitialized)
        {
            m_NdisHook.Cleanup();
        }
    }
    
    TraceExit(ntStatus);
    // Nothing since we are not tracking lists of objects 
    return ntStatus;
}

CMiniportDriver* CWdiDriver::GetMiniportDriverFromDriverHandle(
    _In_ NDIS_HANDLE              NdisMiniportDriverHandle
    )
{
    CMiniportDriver *pReturnMiniportDriver = NULL;
    CMiniportDriver *pTempDriver = NULL;
    PLIST_ENTRY pCurrentEntry = NULL;

    // TODO: Optimize for single instance case
    
    m_MiniportContextLock->EnterCriticalSection();

    if ( IsListEmpty(&m_MiniportDriverList) )
    {
        goto exit;
    }

    pCurrentEntry = m_MiniportDriverList.Flink;
    while ( pCurrentEntry != &m_MiniportDriverList )
    {
        pTempDriver = (CMiniportDriver*)GetCppObjectFromListEntry(
                            pCurrentEntry );

        if (pTempDriver->GetNdisMiniportDriverHandle() == NdisMiniportDriverHandle)
        {
            pReturnMiniportDriver = pTempDriver;
            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }
    
exit:    
    m_MiniportContextLock->LeaveCriticalSection();

    // This should always be present
    NT_ASSERT(pReturnMiniportDriver != NULL);
    
    return pReturnMiniportDriver;
}


CMiniportDriver* CWdiDriver::GetMiniportDriverFromDriverContext(
    _In_ NDIS_HANDLE              MiniportDriverContext
    )
{
    CMiniportDriver *pReturnMiniportDriver = NULL;
    CMiniportDriver *pTempDriver = NULL;
    PLIST_ENTRY pCurrentEntry = NULL;

    // TODO: Optimize for single instance case
    
    m_MiniportContextLock->EnterCriticalSection();

    if ( IsListEmpty(&m_MiniportDriverList) )
    {
        goto exit;
    }

    pCurrentEntry = m_MiniportDriverList.Flink;
    while ( pCurrentEntry != &m_MiniportDriverList )
    {
        pTempDriver = (CMiniportDriver*)GetCppObjectFromListEntry(
                            pCurrentEntry );

        if (pTempDriver->GetMiniportDriverContext() == MiniportDriverContext)
        {
            pReturnMiniportDriver = pTempDriver;
            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }
    
exit:    
    m_MiniportContextLock->LeaveCriticalSection();

    // This should always be present
    NT_ASSERT(pReturnMiniportDriver != NULL);
    
    return pReturnMiniportDriver;
}



CAdapter* CWdiDriver::GetAdapterFromAdapterHandle(
    _In_ NDIS_HANDLE              NdisAdapterHandle
    )
{
    CAdapter *pReturnAdapter = NULL;
    CAdapter *pTempAdapter = NULL;
    PLIST_ENTRY pCurrentEntry = NULL;

    // TODO: Optimize for single instance case
    
    m_MiniportContextLock->EnterCriticalSection();

    if ( IsListEmpty(&m_AdapterList) )
    {
        goto exit;
    }

    pCurrentEntry = m_AdapterList.Flink;
    while ( pCurrentEntry != &m_AdapterList )
    {
        pTempAdapter = (CAdapter*)GetCppObjectFromListEntry(
                            pCurrentEntry );

        if (pTempAdapter->GetMiniportAdapterHandle() == NdisAdapterHandle)
        {
            pReturnAdapter = pTempAdapter;
            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }
    
exit:    
    m_MiniportContextLock->LeaveCriticalSection();

    // This should always be present
    NT_ASSERT(pReturnAdapter != NULL);
    
    return pReturnAdapter;
}


CAdapter* CWdiDriver::GetAdapterFromAdapterContext(
    _In_ NDIS_HANDLE              AdapterContext
    )
{
    CAdapter *pReturnAdapter = NULL;
    CAdapter *pTempAdapter = NULL;
    PLIST_ENTRY pCurrentEntry = NULL;

    // TODO: Optimize for single instance case
    
    m_MiniportContextLock->EnterCriticalSection();

    if ( IsListEmpty(&m_AdapterList) )
    {
        goto exit;
    }

    pCurrentEntry = m_AdapterList.Flink;
    while ( pCurrentEntry != &m_AdapterList )
    {
        pTempAdapter = (CAdapter*)GetCppObjectFromListEntry(
                            pCurrentEntry );

        if (pTempAdapter->GetMiniportAdapterContext() == AdapterContext)
        {
            pReturnAdapter = pTempAdapter;
            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }
    
exit:    
    m_MiniportContextLock->LeaveCriticalSection();

    // This should always be present
    NT_ASSERT(pReturnAdapter != NULL);
    
    return pReturnAdapter;
}


CMiniportDriver* CWdiDriver::GetMiniportDriverFromAdapterHandle(
    _In_ NDIS_HANDLE              NdisAdapterHandle
    )
{
    CMiniportDriver *pReturnMiniportDriver = NULL;
    
    CAdapter *pTempAdapter = this->GetAdapterFromAdapterHandle(NdisAdapterHandle);

    if (pTempAdapter)
    {
        pReturnMiniportDriver = pTempAdapter->GetMiniportDriver();
    }

    return pReturnMiniportDriver;
}

CMiniportDriver* CWdiDriver::GetMiniportDriverFromAdapterContext(
    _In_ NDIS_HANDLE              AdapterContext
    )
{
    CMiniportDriver *pReturnMiniportDriver = NULL;
    
    CAdapter *pTempAdapter = this->GetAdapterFromAdapterContext(AdapterContext);

    if (pTempAdapter)
    {
        pReturnMiniportDriver = pTempAdapter->GetMiniportDriver();
    }

    return pReturnMiniportDriver;
}


NDIS_STATUS
CWdiDriver::RegisterWdiMiniportDriver(
	_In_     PDRIVER_OBJECT                              DriverObject,
	_In_     PCUNICODE_STRING                            RegistryPath,
	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
	_In_     NDIS_MINIPORT_DRIVER_CHARACTERISTICS const *MiniportDriverCharacteristics,
	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
	)
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    CMiniportDriver *pMiniportDriver = NULL;

    TraceEntry();
    *NdisMiniportDriverHandle = NULL;
    
    pMiniportDriver = new CMiniportDriver();
    if ( pMiniportDriver == NULL )
    {
        WFCError("Failed to allocate Miniport Driver object");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    m_MiniportContextLock->EnterCriticalSection();
    
    // Put this in the list of miniport drivers we have allocated
    InsertTailList(
        &m_MiniportDriverList,
        pMiniportDriver->get_MiniportDriverListEntry() );
        
    m_MiniportContextLock->LeaveCriticalSection();

    // Let the driver object handle this
    ndisStatus = pMiniportDriver->RegisterWdiMiniportDriver(
        DriverObject,
        RegistryPath,
        MiniportDriverContext,
        MiniportDriverCharacteristics,
        MiniportWdiCharacteristics,
        NdisMiniportDriverHandle
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to register Miniport Driver. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    
exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pMiniportDriver)
        {
            m_MiniportContextLock->EnterCriticalSection();        
            RemoveEntryList(
                pMiniportDriver->get_MiniportDriverListEntry());

            m_MiniportContextLock->LeaveCriticalSection();

            delete pMiniportDriver;            
        }
    }


    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID
CWdiDriver::DeregisterWdiMiniportDriver(
    _In_ NDIS_HANDLE              NdisMiniportDriverHandle
	)
{
    // Delete cleans up the miniport driver object
    CMiniportDriver* pMiniportDriver = NULL;    

    TraceEntry();

    // Get the CMiniportDriver object corresponding to this handle
    pMiniportDriver = this->GetMiniportDriverFromDriverHandle(NdisMiniportDriverHandle);
    NT_ASSERT(pMiniportDriver != NULL);

    if (pMiniportDriver)
    {
        // Have it deregister
        pMiniportDriver->DeregisterWdiMiniportDriver();

        // Remove from our list
        m_MiniportContextLock->EnterCriticalSection();        
        RemoveEntryList(
            pMiniportDriver->get_MiniportDriverListEntry());

        m_MiniportContextLock->LeaveCriticalSection();

        delete pMiniportDriver;            
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}


NDIS_STATUS CWdiDriver::AllocateAdapter(
    _In_ CMiniportDriver* pMiniportDriver,
    _Out_ CAdapter **ppAdapter)
{
    *ppAdapter = new CAdapter(pMiniportDriver);
    if ( *ppAdapter == NULL )
    {
        WFCError("Failed to create Adapter object");
        return NDIS_STATUS_RESOURCES;
    }

    // Put it into the queue
    m_MiniportContextLock->EnterCriticalSection();
    
    // Put this in the list of adapters we have allocated
    InsertTailList(
        &m_AdapterList,
        (*ppAdapter)->get_AdapterListEntry() );
        
    m_MiniportContextLock->LeaveCriticalSection();    

    // Remember this for debugging purpose
    m_DebugLastCreateAdapter = (*ppAdapter);
    
    return NDIS_STATUS_SUCCESS;
}

VOID CWdiDriver::ReleaseAdapter(_In_ CAdapter *pAdapter)
{
    // Remove from our list
    m_MiniportContextLock->EnterCriticalSection();        
    RemoveEntryList(
        pAdapter->get_AdapterListEntry());

    m_MiniportContextLock->LeaveCriticalSection();

    // Delete cleans up the adapter    
    delete pAdapter;
}


