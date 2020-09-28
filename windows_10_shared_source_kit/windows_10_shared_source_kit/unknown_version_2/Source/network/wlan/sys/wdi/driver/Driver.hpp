#pragma once

// Forward declarations
class CMiniportDriver;
class CAdapter;

/*++

 The global Wdi Driver object

--*/
class CWdiDriver
{
public:
    CWdiDriver();

    ~CWdiDriver();

    NTSTATUS Initialize();

private:
    LIST_ENTRY           m_MiniportDriverList;
    LIST_ENTRY           m_AdapterList;
    CNdisSpinLock*       m_MiniportContextLock;
    CGlobalCtlDevice     m_CtlDevice;
    
    CNdisHook            m_NdisHook;

    CAdapter*            m_DebugLastCreateAdapter;
public:

    CMiniportDriver* GetMiniportDriverFromDriverHandle(
        _In_ NDIS_HANDLE              NdisMiniportDriverHandle
        );

    CMiniportDriver* GetMiniportDriverFromDriverContext(
        _In_ NDIS_HANDLE              MiniportDriverContext
        );

    CAdapter* GetAdapterFromAdapterHandle(
        _In_ NDIS_HANDLE              NdisMiniportAdapterHandle
        );

    CAdapter* GetAdapterFromAdapterContext(
        _In_ NDIS_HANDLE              MiniportAdapterContext
        );

    CMiniportDriver* GetMiniportDriverFromAdapterHandle(
        _In_ NDIS_HANDLE              NdisAdapterHandle
        );

    CMiniportDriver* GetMiniportDriverFromAdapterContext(
        _In_ NDIS_HANDLE              AdapterContext
        );

    CGlobalCtlDevice* GetControlDevice()
    {
        return &m_CtlDevice;
    }

    CNdisHook* GetNdisHook()
    {
        return &m_NdisHook;
    }
        
    NDIS_STATUS
    RegisterWdiMiniportDriver(
    	_In_     PDRIVER_OBJECT                              DriverObject,
    	_In_     PCUNICODE_STRING                            RegistryPath,
    	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
    	_In_     NDIS_MINIPORT_DRIVER_CHARACTERISTICS const *MiniportDriverCharacteristics,
    	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
    	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
    	);

    VOID
	DeregisterWdiMiniportDriver(
	    _In_ NDIS_HANDLE              NdisMiniportDriverHandle
    	);

    NDIS_STATUS AllocateAdapter(
        _In_ CMiniportDriver* pMiniportDriver,    
        _Out_ CAdapter **ppAdapter
        );

    VOID ReleaseAdapter(_In_ CAdapter *pAdapter);
};

// Global WDI Driver object
extern CWdiDriver* g_pWdiDriver;

// Global TraceLogging variables
#ifndef WDI_WIN81_COMPLIANT
TRACELOGGING_DECLARE_PROVIDER(g_hProvider);
extern BOOLEAN g_RegisteredWithTraceLogging;
#endif
