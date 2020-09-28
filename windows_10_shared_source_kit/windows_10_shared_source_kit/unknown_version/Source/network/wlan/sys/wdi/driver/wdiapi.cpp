#include "precomp.hpp"
#include "wdiapi.tmh"

extern ULONG       g_DebugDllRefCount;

NDIS_STATUS
Wdi_NdisMRegisterWdiMiniportDriver(
	_In_     PDRIVER_OBJECT                              DriverObject,
	_In_     PUNICODE_STRING                             RegistryPath,
	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
	_In_     PNDIS_MINIPORT_DRIVER_CHARACTERISTICS       MiniportDriverCharacteristics,
	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
	)
{
    UINT ndisVersion = NdisGetVersion();

    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    *NdisMiniportDriverHandle = NULL;
    
    if (ndisVersion >= NDIS_RUNTIME_VERSION_650)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: Wdi_NdisMRegisterWdiMiniportDriver should NOT be called on this OS\n");
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    
    if (g_pWdiDriver == NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: DriverEntry NOT called with NdisMRegisterWdiMiniportDriver\n");    
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    g_DebugDllRefCount++;

    ndisStatus = g_pWdiDriver->RegisterWdiMiniportDriver(
            DriverObject,
            RegistryPath,
            MiniportDriverContext,
            MiniportDriverCharacteristics,
            MiniportWdiCharacteristics,
            NdisMiniportDriverHandle
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to Register IHV's Miniport Driver. Error = %08x", ndisStatus);
        g_DebugDllRefCount--;
        goto exit;
    }

exit:
    
    return ndisStatus;
}

VOID
Wdi_NdisMDeregisterWdiMiniportDriver(
	_In_ NDIS_HANDLE              NdisMiniportDriverHandle
	)
{
    g_pWdiDriver->DeregisterWdiMiniportDriver(NdisMiniportDriverHandle);

    g_DebugDllRefCount--;    
}


VOID
Wdi_NdisMOidRequestComplete(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             Status
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);
    
    pAdapter->OnOidRequestCompleteFromMiniport(Request, Status);
    
    TraceExit(Status);
}

EXPORT
VOID
Wdi_NdisMIndicateStatusEx(
    _In_ NDIS_HANDLE              MiniportAdapterHandle,
    _In_ PNDIS_STATUS_INDICATION  StatusIndication
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);
    
    pAdapter->OnStatusIndicationFromMiniport(StatusIndication);
    
    TraceExit(NDIS_STATUS_SUCCESS);
}
