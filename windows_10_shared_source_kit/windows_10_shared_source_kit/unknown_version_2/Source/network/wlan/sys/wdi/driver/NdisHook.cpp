#include "precomp.hpp"
#include "NdisHook.tmh"

extern ULONG       g_DebugDllRefCount;

extern "C" CONST NPI_MODULEID NPI_MS_WDIWIFI_MODULEID;

// Callbacks from NDIS into our hook driver
NDIS_MINIPORT_HOOK_DRIVER_REGISTER MPHookWdiDriverRegister;
NDIS_MINIPORT_HOOK_DRIVER_DEREGISTER MPHookWdiDriverDeregister;
NDIS_MINIPORT_HOOK_OID_REQUEST_COMPLETE MPHookOnMiniportOidRequestComplete;
NDIS_MINIPORT_HOOK_DIRECT_OID_REQUEST_COMPLETE MPHookOnMiniportDirectOidRequestComplete;
NDIS_MINIPORT_HOOK_INDICATE_STATUS MPHookOnMiniportIndicateStatus;

// Callbacks from NMR into our provider driver
NPI_PROVIDER_ATTACH_CLIENT_FN NMRProviderAttachClientThunk;
NPI_PROVIDER_DETACH_CLIENT_FN NMRProviderDetachClientThunk;
NPI_PROVIDER_CLEANUP_BINDING_CONTEXT_FN NMRProviderCleanupBindingContextThunk;


CNdisHook::CNdisHook() :
    m_NmrProviderHandle(NULL),
    m_NmrBindingHandle(NULL),
    m_NmrClientContext(NULL),
    m_NdisSupportsHook(true)    
{
    RtlZeroMemory(&m_HookProviderDispatch, sizeof(NDIS_MINIPORT_HOOK_DRIVER_PROVIDER_DISPATCH));
    RtlZeroMemory(&m_NmrProviderCharacteristics, sizeof(NPI_PROVIDER_CHARACTERISTICS));
    RtlZeroMemory(&m_HookCharacteristics, sizeof(NDIS_MINIPORT_HOOK_DRIVER_PROVIDER_CHARACTERISTICS));


    m_HookProviderDispatch.DriverRegister = MPHookWdiDriverRegister;
    m_HookProviderDispatch.DriverDeregister = MPHookWdiDriverDeregister;
    m_HookProviderDispatch.OnOidRequestComplete = MPHookOnMiniportOidRequestComplete;
    m_HookProviderDispatch.OnDirectOidRequestComplete = MPHookOnMiniportDirectOidRequestComplete;
    m_HookProviderDispatch.OnIndicateStatus = MPHookOnMiniportIndicateStatus;

    // Fill Hook callbacks for NDIS
    m_HookCharacteristics.HookType = NdisMiniportHookDriverTypeWdi;

    // Fill NMR characteristics
    m_NmrProviderCharacteristics.Version = 0;
    m_NmrProviderCharacteristics.Length = sizeof(NPI_PROVIDER_CHARACTERISTICS);

    m_NmrProviderCharacteristics.ProviderAttachClient = NMRProviderAttachClientThunk;
    m_NmrProviderCharacteristics.ProviderDetachClient = NMRProviderDetachClientThunk;
    m_NmrProviderCharacteristics.ProviderCleanupBindingContext = NMRProviderCleanupBindingContextThunk;
    
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.Version = NDIS_MINIPORT_DRIVER_HOOK_NPI_VERSION_1;
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.Size = sizeof(m_NmrProviderCharacteristics.ProviderRegistrationInstance);
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.NpiId = &NPI_NDIS_MINIPORT_HOOK_ID;
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.ModuleId = &NPI_MS_WDIWIFI_MODULEID;
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.Number = 0;
    m_NmrProviderCharacteristics.ProviderRegistrationInstance.NpiSpecificCharacteristics = &m_HookCharacteristics;
    
    
}

CNdisHook::~CNdisHook()
{
    if (m_NmrProviderHandle)
    {
        this->Cleanup();
    }
}

NTSTATUS CNdisHook::Initialize(
    _In_ CWdiDriver* pDriver
    )
{
    NTSTATUS registrationStatus = STATUS_SUCCESS;
    UINT ndisVersion = NdisGetVersion();

    TraceEntry();

    m_Driver = pDriver;

    if (ndisVersion >= NDIS_RUNTIME_VERSION_650)
    {
        // Running on latest OS. Use the Nmr method to hook to NDIS
        registrationStatus = NmrRegisterProvider(&m_NmrProviderCharacteristics, this, &m_NmrProviderHandle);
        if (!NT_SUCCESS(registrationStatus))
        {
            WFCError("Failed to Register with NMR as a Ndis Hook Provider Driver. Error = %08x", registrationStatus);
            goto exit;
        }

        // If we succeeded, we should have been attached to NDIS
        NT_ASSERT(m_NmrBindingHandle != NULL);
    }
    else
    {
#ifdef WDI_DISABLED    
        // TODO: Temporary workaround to allow running as a standalone service on Windows 8.1 and as a hook on latest OS
#endif // WDI_DISABLED    
        // implementation
        m_NdisSupportsHook = false;
        WFCInfo("Not running as a NDIS hook driver. Reduced functionality mode");
    }
exit:

    TraceExit(registrationStatus);

    return registrationStatus;
}

VOID CNdisHook::Cleanup()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    TraceEntry();

    if (m_NdisSupportsHook)
    {
        if (m_NmrProviderHandle)
        {
            ntStatus = NmrDeregisterProvider(m_NmrProviderHandle);
            
            NT_ASSERT(ntStatus == STATUS_SUCCESS || ntStatus == STATUS_PENDING);

            if (ntStatus == STATUS_PENDING)
            {
                ntStatus = NmrWaitForProviderDeregisterComplete(m_NmrProviderHandle);
                NT_ASSERT(ntStatus == STATUS_SUCCESS);
            }
            
            m_NmrProviderHandle = NULL;
        }
    }

    TraceExit(ntStatus);  
}

NTSTATUS
CNdisHook::AttachToNdis(
    _In_ HANDLE  NmrBindingHandle,
    _In_ PNPI_REGISTRATION_INSTANCE  ClientRegistrationInstance,
    _In_ PVOID  ClientBindingContext,
    _In_ CONST VOID *ClientDispatch,
    _Out_ CONST VOID*  *ProviderDispatch
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    TraceEntry();
    UNREFERENCED_PARAMETER(ClientRegistrationInstance);

    if (m_NmrBindingHandle != NULL)
    {
        WFCError("Already attached to NDIS. Ignoring second call");
        ntStatus = STATUS_ALREADY_REGISTERED;
        goto exit;
    }

    m_NmrBindingHandle = NmrBindingHandle;
    m_NmrClientContext = ClientBindingContext;

    RtlCopyMemory(&m_NdisRawDispatch,
        ClientDispatch,
        sizeof(NDIS_MINIPORT_HOOK_DRIVER_CLIENT_DISPATCH));

    *ProviderDispatch = &m_HookProviderDispatch;

exit:
    TraceExit(ntStatus);
    return ntStatus;
}


NTSTATUS   
CNdisHook::DetachFromNdis()
{
    // We are always called when we are stopping. We complete synchronously
    
    m_NmrBindingHandle = NULL;

    return STATUS_SUCCESS;
}

NDIS_STATUS
CNdisHook::InvokeMiniportOidRequest(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_OID_REQUEST       Request
    )
{
    return m_NdisRawDispatch.InvokeOidRequestHandler(
        (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
        Request
        );
}

VOID
CNdisHook::InvokeMiniportCancelOidRequest(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PVOID                   RequestId
)
{
    m_NdisRawDispatch.InvokeCancelOidRequestHandler(
        (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
        RequestId
        );
}

NDIS_STATUS
CNdisHook::InvokeMiniportDirectOidRequest(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_OID_REQUEST       Request
)
{
    return m_NdisRawDispatch.InvokeDirectOidRequestHandler(
        (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
        Request
        );
}

VOID
CNdisHook::InvokeMiniportCancelDirectOidRequest(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PVOID                   RequestId
)
{
    m_NdisRawDispatch.InvokeCancelDirectOidRequestHandler(
        (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
        RequestId
        );
}

VOID
CNdisHook::CompleteOidRequestToNdis(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             Status
    )
{
    if (m_NdisSupportsHook)
    {
        m_NdisRawDispatch.RawOidRequestComplete(
            (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
            Request,
            Status
            );
    }
    else
    {
        NdisMOidRequestComplete(
            MiniportAdapterHandle,
            Request,
            Status
            );
    }  
}

VOID
CNdisHook::IndicateStatusToNdis(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_STATUS_INDICATION StatusIndication
    )
{
    if (m_NdisSupportsHook)
    {
        m_NdisRawDispatch.RawIndicateStatus(
            (NDIS_MINIPORT_ADAPTER_HANDLE)MiniportAdapterHandle,
            StatusIndication
            );
    }
    else
    {
        NdisMIndicateStatusEx(
            MiniportAdapterHandle,
            StatusIndication
            );
    }
}

// Called when the pipe between us and NDIS is setup (since NDIS is 
// always around, this is after we called NmrRegisterProvider)
NTSTATUS
NMRProviderAttachClientThunk(
    _In_ HANDLE  NmrBindingHandle,
    _In_ PVOID  ProviderContext,
    _In_ PNPI_REGISTRATION_INSTANCE  ClientRegistrationInstance,
    _In_ PVOID  ClientBindingContext,
    _In_ CONST VOID *ClientDispatch,
    _Out_ PVOID  *ProviderBindingContext,
    _Out_ CONST VOID*  *ProviderDispatch
    )
{
    CNdisHook *pNdisHook = (CNdisHook *)ProviderContext;
    NTSTATUS ntStatus = STATUS_NOT_IMPLEMENTED;
    
    TraceEntry();

    ntStatus = pNdisHook->AttachToNdis(
        NmrBindingHandle,
        ClientRegistrationInstance,
        ClientBindingContext,
        ClientDispatch, 
        ProviderDispatch
        );
    if (NT_SUCCESS(ntStatus))
    {
        // For simplicity, we will use the same context
        *ProviderBindingContext = ProviderContext;
    }
    
    TraceExit(ntStatus);    
    return ntStatus;
}

// Called when the pipe between us and NDIS is torn down (since NDIS is 
// always around, this is after we called NmrDeregisterProvider)
NTSTATUS
NMRProviderDetachClientThunk(
    _In_ PVOID  ProviderBindingContext
    )
{
    CNdisHook *pNdisHook = (CNdisHook *)ProviderBindingContext;
    NTSTATUS ntStatus = STATUS_NOT_IMPLEMENTED;
    
    TraceEntry();
    ntStatus = pNdisHook->DetachFromNdis();
    
    TraceExit(ntStatus);    
    return ntStatus;
}


VOID
NMRProviderCleanupBindingContextThunk(
    _In_ PVOID  ProviderBindingContext
    )
{
    NTSTATUS ntStatus = STATUS_NOT_IMPLEMENTED;
    
    TraceEntry();
    UNREFERENCED_PARAMETER(ProviderBindingContext);
    
    TraceExit(ntStatus);    
}

// This is called when an IHV driver wants to register as a WDI miniport
NDIS_STATUS
MPHookWdiDriverRegister(
    _In_     PVOID                                       NmrProviderBindingContext,
    _In_     DRIVER_OBJECT                              *DriverObject,
    _In_     PCUNICODE_STRING                            RegistryPath,
    _In_opt_ NDIS_MINIPORT_DRIVER_CONTEXT                MiniportDriverContext,
    _In_     NDIS_MINIPORT_DRIVER_CHARACTERISTICS const *MiniportDriverCharacteristics,
    _In_     void const *                                AdditionalCharacteristics,
    _Out_    NDIS_MINIPORT_DRIVER_HANDLE                *NdisMiniportDriverHandle
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    *NdisMiniportDriverHandle = NULL;

    TraceEntry();
    UNREFERENCED_PARAMETER(NmrProviderBindingContext);
    
    if (g_pWdiDriver == NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: DriverEntry NOT called with NdisMRegisterWdiMiniportDriver\n");    
        ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
    }
    g_DebugDllRefCount++;

    ndisStatus = g_pWdiDriver->RegisterWdiMiniportDriver(
            DriverObject,
            RegistryPath,
            MiniportDriverContext,
            MiniportDriverCharacteristics,
            (PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS)AdditionalCharacteristics,
            (PNDIS_HANDLE)NdisMiniportDriverHandle  // TODO: This would become NDIS_HANDLE eventually
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to Register IHV's Miniport Driver. Error = %08x", ndisStatus);
        // Deref
        g_DebugDllRefCount--;        
        goto exit;        
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

// This is called when an IHV driver wants to deregister as a WDI miniport
VOID
MPHookWdiDriverDeregister(
    _In_     PVOID                                       NmrProviderBindingContext,
    _In_     NDIS_MINIPORT_DRIVER_HANDLE                 NdisMiniportDriverHandle,
    _In_     NDIS_MINIPORT_HOOK_DRIVER_HANDLE            NdisMiniportHookDriverHandle
    )
{
    TraceEntry();
    UNREFERENCED_PARAMETER(NmrProviderBindingContext);
    UNREFERENCED_PARAMETER(NdisMiniportHookDriverHandle);
    
    g_pWdiDriver->DeregisterWdiMiniportDriver(NdisMiniportDriverHandle);

    g_DebugDllRefCount--;

    TraceExit(NDIS_STATUS_SUCCESS);
}


VOID
MPHookOnMiniportOidRequestComplete(
    _In_     PVOID                                       NmrProviderBindingContext,
    _In_     NDIS_MINIPORT_HOOK_ADAPTER_HANDLE           MiniportHookAdapterHandle,
    _In_     NDIS_MINIPORT_ADAPTER_HANDLE                MiniportAdapterHandle,
    _In_     NDIS_OID_REQUEST                           *OidRequest,
    _In_     NDIS_STATUS                                 CompletionStatus
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();
    UNREFERENCED_PARAMETER(NmrProviderBindingContext);
    UNREFERENCED_PARAMETER(MiniportHookAdapterHandle);

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);
    
    pAdapter->OnOidRequestCompleteFromMiniport(OidRequest, CompletionStatus);
    
    TraceExit(CompletionStatus);
}

VOID
MPHookOnMiniportDirectOidRequestComplete(
    _In_     PVOID                                       NmrProviderBindingContext,
    _In_     NDIS_MINIPORT_HOOK_ADAPTER_HANDLE           MiniportHookAdapterHandle,
    _In_     NDIS_MINIPORT_ADAPTER_HANDLE                MiniportAdapterHandle,
    _In_     NDIS_OID_REQUEST                           *OidRequest,
    _In_     NDIS_STATUS                                 CompletionStatus
    )
{
    CAdapter *pAdapter = NULL;
    
    UNREFERENCED_PARAMETER(NmrProviderBindingContext);
    UNREFERENCED_PARAMETER(MiniportHookAdapterHandle);

    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);

    pAdapter->OnOidRequestCompleteFromMiniport(OidRequest, CompletionStatus);
    
    TraceExit(CompletionStatus);
}

VOID
MPHookOnMiniportIndicateStatus(
    _In_     PVOID                                       NmrProviderBindingContext,
    _In_     NDIS_MINIPORT_HOOK_ADAPTER_HANDLE           MiniportHookAdapterHandle,
    _In_     NDIS_MINIPORT_ADAPTER_HANDLE                MiniportAdapterHandle,
    _In_     NDIS_STATUS_INDICATION                     *StatusIndication
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();
    UNREFERENCED_PARAMETER(NmrProviderBindingContext);
    UNREFERENCED_PARAMETER(MiniportHookAdapterHandle);

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);

    // NDIS is forwarding internally generated indications to us. Forward them directly
    if ((StatusIndication->StatusCode & WDI_INDICATION_PREFIX) == WDI_INDICATION_PREFIX)
    {
        pAdapter->OnStatusIndicationFromMiniport(StatusIndication);
    }
    else
    {
        CNdisHook *pNdisHook = (CNdisHook *)NmrProviderBindingContext;
    
        pNdisHook->IndicateStatusToNdis(
            MiniportAdapterHandle,
            StatusIndication
            );
    }
    
    TraceExit(NDIS_STATUS_SUCCESS);
}
