/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Miniport.hpp

Abstract:

    Header file for the Wi-Fi Miniport Driver structure

Environment:

    Kernel mode

Revision History:

--*/

#pragma once

/*++

 Object for each Miniport Driver object

--*/
class CMiniportDriver
{
public:
    CMiniportDriver();

    ~CMiniportDriver();

    NDIS_STATUS Initialize();

private:
    CAdapter*   m_DebugLastCreateAdapter;

    NDIS_HANDLE m_NdisMiniportDriverHandle;
    
    CPP_LIST_ENTRY  m_Link;

    NDIS_MINIPORT_DRIVER_CHARACTERISTICS       m_MiniportDriverCharacteristics;
    NDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   m_MiniportWdiCharacteristics;
    PDRIVER_OBJECT                             m_DriverObject;
    NDIS_HANDLE                                m_MiniportDriverContext;
    bool                                       m_bFakeMiniportDriverContext;
    NDIS_WDI_INIT_PARAMETERS                   m_NdisWdiInitParameters;
    
public:
    PLIST_ENTRY get_MiniportDriverListEntry()
    {
        return &m_Link.ListEntry;
    }

    NDIS_HANDLE GetNdisMiniportDriverHandle()
    {
        return m_NdisMiniportDriverHandle;
    }

    NDIS_HANDLE GetMiniportDriverContext()
    {
        return m_MiniportDriverContext;
    }

    NDIS_HANDLE GetIHVMiniportDriverContext()
    {
        // Get the IHV;s miniport diver 
        if (!m_bFakeMiniportDriverContext)
            return m_MiniportDriverContext;
        return NULL;            
    }

    PNDIS_MINIPORT_DRIVER_CHARACTERISTICS GetNdisCharacteristics()
    {
        return &m_MiniportDriverCharacteristics;
    }

    PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS GetWdiCharacteristics()
    {
        return &m_MiniportWdiCharacteristics;
    }

    NDIS_STATUS
    RegisterWdiMiniportDriver(
    	_In_     PDRIVER_OBJECT                              DriverObject,
    	_In_     PCUNICODE_STRING                            RegistryPath,
    	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
    	_In_     NDIS_MINIPORT_DRIVER_CHARACTERISTICS const *OriginalMiniportDriverCharacteristics,
    	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
    	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
    	);

    VOID
	DeregisterWdiMiniportDriver();

    NDIS_STATUS
    MPInitializeEx(
        _In_  NDIS_HANDLE                        MiniportAdapterHandle,
        _In_  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
        );

    VOID
    MPHalt(
        _In_  CAdapter*               pAdapter,
        _In_  NDIS_HALT_ACTION        HaltAction
        );

};


typedef VOID
(MP_NDIS_REEENUMERATE_FAILED_ADAPTER)(
    _In_ NDIS_HANDLE  MiniportAdapterHandle
    );

typedef MP_NDIS_REEENUMERATE_FAILED_ADAPTER (*MP_NDIS_REEUMERATE_HANDLER);
extern MP_NDIS_REEUMERATE_HANDLER g_NdisMReenumerateFailedAdapter;

