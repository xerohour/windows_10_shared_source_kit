#pragma once

// Forward declarations
class CMiniportDriver;
class CAdapter;
class CWdiDriver;

/*++

 The interfaces to NDIS API

--*/
class CNdisHook
{
public:
    CNdisHook();

    ~CNdisHook();

    NTSTATUS Initialize(
        _In_ CWdiDriver* pDriver
        );
    VOID Cleanup();

    NTSTATUS
    AttachToNdis(
        _In_ HANDLE  NmrBindingHandle,
        _In_ PNPI_REGISTRATION_INSTANCE  ClientRegistrationInstance,
        _In_ PVOID  ClientBindingContext,
        _In_ CONST VOID *ClientDispatch,
        _Out_ CONST VOID*  *ProviderDispatch
        );

    NTSTATUS   
    DetachFromNdis();

    NDIS_STATUS
    InvokeMiniportOidRequest(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PNDIS_OID_REQUEST       Request
        );

    VOID
    InvokeMiniportCancelOidRequest(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PVOID                   RequestId
        );

    NDIS_STATUS
    InvokeMiniportDirectOidRequest(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PNDIS_OID_REQUEST       Request
        );
    

    VOID
    InvokeMiniportCancelDirectOidRequest(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PVOID                   RequestId
        );

    VOID
    CompleteOidRequestToNdis(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PNDIS_OID_REQUEST       Request,
        _In_  NDIS_STATUS             Status
        );

    VOID
    IndicateStatusToNdis(
        _In_  NDIS_HANDLE             MiniportAdapterHandle,
        _In_  PNDIS_STATUS_INDICATION StatusIndication
        );

private:
    CWdiDriver* m_Driver;
    HANDLE m_NmrProviderHandle;
    HANDLE m_NmrBindingHandle;

    HANDLE m_NmrClientContext;
    bool m_NdisSupportsHook;

    NPI_PROVIDER_CHARACTERISTICS m_NmrProviderCharacteristics;
    NDIS_MINIPORT_HOOK_DRIVER_PROVIDER_CHARACTERISTICS m_HookCharacteristics;
    NDIS_MINIPORT_HOOK_DRIVER_CLIENT_DISPATCH m_NdisRawDispatch;
    NDIS_MINIPORT_HOOK_DRIVER_PROVIDER_DISPATCH m_HookProviderDispatch;

public:

    
};


