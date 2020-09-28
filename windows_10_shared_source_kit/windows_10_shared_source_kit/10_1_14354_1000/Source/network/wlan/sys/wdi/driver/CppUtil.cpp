/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    cpputil.cpp

Abstract:

    Contains code to general utilties and abstractions of kerenl services, e.g. NDIS, KMDF

Environment:

    Kernel mode

Author:


--*/
#include "precomp.hpp"
#include "CppUtil.tmh"

#if DBG
// Auto-enabled for checked build. 
#define _TRACK_LEAK_OF_NEW_CALLER 1
#endif

#ifdef _TRACK_LEAK_OF_NEW_CALLER

// Optional header we will include before the actual memory to track memory leaks
typedef struct _WFC_MEMORY_HEADER {
    PVOID pvCaller;
    PVOID pvCallersCaller;
} WFC_MEMORY_HEADER, *PWFC_MEMORY_HEADER;

#define WFC_MEMORY_HEADER_SIZE (sizeof(WFC_MEMORY_HEADER))

#else

// No tracking data
#define WFC_MEMORY_HEADER_SIZE (0)

#endif

void* __cdecl operator new(
    size_t Size )
/*++

  Override C++ allocation operator.

  --*/
{
    PVOID pData = ExAllocatePoolWithTag(
        NonPagedPoolNx,
        Size + WFC_MEMORY_HEADER_SIZE,
        WDI_POOL_TAG );

    if ( pData != NULL )
    {
        RtlZeroMemory(
            pData,
            Size + WFC_MEMORY_HEADER_SIZE);
    }

#ifdef _TRACK_LEAK_OF_NEW_CALLER
    //
    // enable this to track the leaks of the original allocaotr via new() -noisy
    //
    PVOID pvCaller, pvCallersCaller;
    PWFC_MEMORY_HEADER pMemoryHeader = (PWFC_MEMORY_HEADER)pData;

    RtlGetCallersAddress( &pvCaller, &pvCallersCaller );

    
//    DbgPrintEx( 0, 0, "New caller=%p callerCaller=%p Size=%p p=0x%p\n",
//            pvCaller, pvCallersCaller, Size, pData );

    pMemoryHeader->pvCaller = pvCaller;
    pMemoryHeader->pvCallersCaller = pvCallersCaller;

    pData = (PUCHAR)pData + WFC_MEMORY_HEADER_SIZE;
#endif // _TRACK_LEAK_OF_NEW_CALLER
    
    return pData;
} 

_Ret_writes_bytes_maybenull_( _Size )
void * PlacementNewHelper( size_t _Size, PCPLACEMENT_NEW_ALLOCATION_CONTEXT AllocationContext )
{
    if (_Size <= AllocationContext->cbMaxSize)
    {
        RtlZeroMemory( AllocationContext->pbBuffer, _Size );
        return AllocationContext->pbBuffer;
    }
    WFCError( "Placement operator new called with insufficient buffer space (desired: %Iu, availible: %Iu)", _Size, AllocationContext->cbMaxSize );
    return NULL;
}


void __cdecl operator delete(
    void* pData )
/*++

  Override C++ delete operator.

  --*/
{
    if ( pData != NULL )
    {
#ifdef _TRACK_LEAK_OF_NEW_CALLER
        PVOID pPreHeaderData = ((PUCHAR)pData - WFC_MEMORY_HEADER_SIZE);
    
        ExFreePoolWithTag(
            pPreHeaderData,
            WDI_POOL_TAG );

#else
        ExFreePoolWithTag(
            pData,
            WDI_POOL_TAG );


#endif // _TRACK_LEAK_OF_NEW_CALLER
            
    }
}

NDIS_STATUS CNdisSpinLock::CreateInstance(
    _Out_ CNdisSpinLock** ppLockContext)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CNdisSpinLock* toReturn = NULL;

    TraceEntry();

    toReturn = new CNdisSpinLock();

    if (toReturn == NULL)
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    *ppLockContext = toReturn;
    toReturn = NULL;
exit:
#pragma warning(suppress: 28922) // Redundant Pointer Test - yes it's redundant, but it's also safe if more error conditions are added above
    if (toReturn != NULL)
    {
        delete toReturn;
    }

    TraceExit(status);
    return status;
}



class CNdisEventContext :
    public IEventContext
{
public:
    CNdisEventContext()
    {
        NdisInitializeEvent(
            &m_Event );
    }

    void Set()
    {
        NdisSetEvent(
            &m_Event );
        
    }

    void Reset()
    {
        NdisResetEvent(
            &m_Event );
        
    }

    BOOLEAN Wait(UINT MstoWait)
    {
        return NdisWaitEvent(
            &m_Event,
            MstoWait);
        
    }

private:
    NDIS_EVENT m_Event;
};

NDIS_STATUS IEventContext::CreateInstance(
    _Out_ IEventContext** ppEventContext )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    IEventContext* toReturn = NULL;

    TraceEntry();

    toReturn = new CNdisEventContext();

    if ( toReturn == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    *ppEventContext = toReturn;
    toReturn = NULL;

exit:

#pragma warning(suppress: 28922) // Redundant Pointer Test - yes it's redundant, but it's also safe if more error conditions are added above
    if (toReturn != NULL)
    {
        delete toReturn;
    }

    TraceExit(status);
    return status;
}


HANDLE CDebugMetadata::get_CurrentThreadHandle()
{
    return KeGetCurrentThread();
}

//assume verifier not running unless init shows otherwise
bool CSystem::m_IsVerifierEnabled = false;
ULONG CSystem::m_VerifierLevel = 0;
PXDV_EXPORT_TABLE CSystem::m_pVerifierCallbackTable = NULL;

void CSystem::Init()
{
    NTSTATUS status = STATUS_SUCCESS;

    status = MmIsVerifierEnabled(&m_VerifierLevel);

    if (NT_SUCCESS(status) && (m_VerifierLevel & m_VerifierNdisWifiFlag))
    {
        status = XdvGetCallbackTable(&m_pVerifierCallbackTable);

        if (NT_SUCCESS(status) && m_pVerifierCallbackTable)
        {
            m_IsVerifierEnabled = true;
        }
    }
}

bool CSystem::get_IsRunningOnOlderOs()
{
    if (IS_FEATURE_ENABLED(TEST_FEATUREMASK_OLDER_OS))
    {
        return true;
    }
    return false;
}


ULONGLONG CSystem::get_CurrentTime()
{
    LARGE_INTEGER currentTime;

    NdisGetCurrentSystemTime(&currentTime );

    return currentTime.QuadPart;
}

void CSystem::XdvStatelessAssert(
    _In_z_ PCHAR ErrorMessage)
{
    if (m_IsVerifierEnabled)
    {
        m_pVerifierCallbackTable->BugcheckStateless(ErrorMessage, m_VerifierWifiErrorCode);
    }
}

NTSTATUS
CGlobalCtlDevice::Initialize()
/*++

    This function creates a WDF control device. 
    It is needed to use WDF services,e.g. WDFWORKITEM 

--*/
{
    NTSTATUS status;
    PWDFDEVICE_INIT pWdfDeviceInit = NULL;
    WDF_OBJECT_ATTRIBUTES  attributes;

    TraceEntry();
    
    pWdfDeviceInit = WdfControlDeviceInitAllocate( WdfGetDriver(), &SDDL_DEVOBJ_KERNEL_ONLY );
    if ( pWdfDeviceInit == NULL )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        WFCError( "Alloc DeviceInit failed" );
        goto exit;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    status = WdfDeviceCreate(&pWdfDeviceInit,
                             &attributes,
                             &m_WdfDevice );
    if (!NT_SUCCESS(status)) 
    {
        WFCError( "WdfDeviceCreate failed 0x%x", status );
        goto exit;
    }

exit:
    if ( pWdfDeviceInit )
    {
        WdfDeviceInitFree( pWdfDeviceInit );
    }
    TraceExit( status );
    return status;
}


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WFC_TIMER_CONTEXT, GetWfcTimerContext);

class CWdfTimerContext : public ITimerContext
{

public:
    CWdfTimerContext() :
        m_WdfTimer(NULL),
        m_pTimerCallback(NULL),
        m_TimerContext(NULL),
        m_DebugInitialized(false),
        m_DebugTimerSet(false),
        m_DebugTimerFired(false)
    {
        WDF_TIMER_CONFIG_INIT(
            &m_TimerConfig,
            s_TimerCallback );

        WDF_OBJECT_ATTRIBUTES_INIT(
            &m_ObjectAttributes );

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
            &m_ObjectAttributes,
            WFC_TIMER_CONTEXT );
    }

    _Function_class_( EVT_WDF_TIMER )
    static VOID s_TimerCallback(
        _In_ WDFTIMER Timer )
    {
        CWdfTimerContext **ppWfcTimer = (CWdfTimerContext**)GetWfcTimerContext( Timer );

        NT_ASSERT(
            ppWfcTimer != NULL &&
            *ppWfcTimer != NULL );

        (*ppWfcTimer)->OnTimerCallback();
    }

    void OnTimerCallback()
    {
        TraceEntry();

        NT_ASSERT(m_pTimerCallback != NULL);

        m_DebugTimerFired = true;
        m_DebugTimerSet = false;
        m_pTimerCallback->OnTimerCallback(
            m_TimerContext );

        TraceExit(NDIS_STATUS_SUCCESS);
    }

    ~CWdfTimerContext()
    {
        if ( m_WdfTimer != NULL )
        {
            WdfObjectDelete(m_WdfTimer);
            m_WdfTimer = NULL;
        }
    }

    NDIS_STATUS Initialize(
        _In_ CGlobalCtlDevice* pCtlDevice,
        _In_ ITimerCallback* pCallback,
        _In_ WDF_EXECUTION_LEVEL level)
    {
        NDIS_STATUS status = NDIS_STATUS_SUCCESS;
        NTSTATUS wdfStatus = STATUS_SUCCESS;

        TraceEntry();

        if ( pCallback == NULL || pCtlDevice == NULL )
        {
            status = NDIS_STATUS_INVALID_PARAMETER;
            WFCError("Bad parameter to %!FUNC!");
            goto exit;
        }

        NT_ASSERT(!m_DebugInitialized);

        m_ObjectAttributes.ExecutionLevel = level;           
        m_ObjectAttributes.ParentObject = pCtlDevice->GetDeviceHandle();

        wdfStatus = WdfTimerCreate(
            &m_TimerConfig,
            &m_ObjectAttributes,
            &m_WdfTimer );

        if ( !NT_SUCCESS( wdfStatus ) )
        {
            status = NTSTATUS2NDISSTATUS(wdfStatus);
            WFCError("Call to WdfTimerCreate failed with %u", wdfStatus);
            goto exit;
        }

        WFC_TIMER_CONTEXT* pTimerContext = GetWfcTimerContext(m_WdfTimer);
        *pTimerContext = (WFC_TIMER_CONTEXT)this;

        m_DebugTimerSet = false;
        m_DebugTimerFired = false;
        m_DebugInitialized = true;
        m_pTimerCallback = pCallback;

    exit:
        TraceExit(status);
        return status;
    }

    NDIS_STATUS SetTimer(
        _In_ ULONG RelativeTimeoutMs,
        _In_opt_ WFC_TIMER_CONTEXT TimerContext )
    {
        NDIS_STATUS status = NDIS_STATUS_SUCCESS;
        BOOLEAN inTimerQueue = FALSE;

        TraceEntry();

        NT_ASSERT(!m_DebugTimerSet);

        m_TimerContext = TimerContext;

        inTimerQueue = WdfTimerStart(
            m_WdfTimer,
            WDF_REL_TIMEOUT_IN_MS(RelativeTimeoutMs) );

        if ( inTimerQueue )
        {
            WFCError("Timer already in progress.");
            status = NDIS_STATUS_FAILURE;
            goto exit;
        }

        m_DebugTimerSet = true;

    exit:
        TraceExit(status);
        return status;
    }

    bool CancelTimer()
    {
        NT_ASSERT(m_DebugInitialized);

        m_DebugTimerSet = false;
        return WdfTimerStop(
            m_WdfTimer,
            FALSE ) ? true : false;
    }

protected:
    WDF_TIMER_CONFIG      m_TimerConfig;
    WDF_OBJECT_ATTRIBUTES m_ObjectAttributes;
    WDFTIMER              m_WdfTimer;

    ITimerCallback*       m_pTimerCallback;
    WFC_TIMER_CONTEXT     m_TimerContext;

    bool                  m_DebugInitialized;
    bool                  m_DebugTimerSet;
    bool                  m_DebugTimerFired;
};

NDIS_STATUS ITimerContext::CreateInstance(
    _Out_ ITimerContext** ppTimerContext )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ITimerContext* pTimerContext = NULL;

    TraceEntry();

    if ( ppTimerContext == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    pTimerContext = new CWdfTimerContext();
    if ( pTimerContext == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    *ppTimerContext = pTimerContext;
    pTimerContext = NULL;

exit:

#pragma warning(suppress: 28922) // Redundant Pointer Test - yes it's redundant, but it's also safe if more error conditions are added above
    if (pTimerContext != NULL)
    {
        delete pTimerContext;
    }

    TraceExit(status);
    return status;
}

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WFC_WORK_ITEM_CONTEXT, GetWfcWorkItemContext);

class CWdfWorkItemContext : public IWorkItemContext
{
public:
    CWdfWorkItemContext() :
        m_DebugInitialized(false),
        m_DebugPending(false),
        m_WorkItemContext(NULL),
        m_WorkItem(NULL)
    {
        WDF_OBJECT_ATTRIBUTES_INIT(
            &m_ObjectAttributes );

        WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(
            &m_ObjectAttributes,
            WFC_WORK_ITEM_CONTEXT );

        WDF_WORKITEM_CONFIG_INIT(
            &m_WorkItemConfig,
            s_WdfWorkItemCallback );
    }

    ~CWdfWorkItemContext()
    {
        NT_ASSERT(!m_DebugPending);
        if ( m_WorkItem != NULL )
        {
            
            WdfObjectDelete(m_WorkItem);
            m_WorkItem = NULL;
        }
    }

    _Function_class_( EVT_WDF_WORKITEM )
    static VOID s_WdfWorkItemCallback(
        _In_ WDFWORKITEM WorkItem )
    {
        CWdfWorkItemContext **ppWfcWorkItem = (CWdfWorkItemContext**)GetWfcWorkItemContext( WorkItem );

        NT_ASSERT(
            ppWfcWorkItem != NULL &&
            *ppWfcWorkItem != NULL );

        (*ppWfcWorkItem)->OnWorkItemCallback();
    }

    void OnWorkItemCallback()
    {
        NT_ASSERT(
            m_DebugInitialized &&
            m_DebugPending );

        m_DebugPending = false;
        
        m_pWorkItemCallback->OnWorkItemCallback(
            m_WorkItemContext );
    }

    NDIS_STATUS Initialize(
        _In_ CGlobalCtlDevice* pCtlDevice,
        _In_ IWorkItemCallback *pCallback,
        _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext )
    {
        NDIS_STATUS status = NDIS_STATUS_SUCCESS;
        NT_ASSERT(!m_DebugInitialized);

        TraceEntry();

        if ( pCallback == NULL || pCtlDevice == NULL )
        {
            NT_ASSERT(FALSE);
            status = NDIS_STATUS_INVALID_PARAMETER;
            goto exit;
        }

        m_ObjectAttributes.ParentObject = pCtlDevice->GetDeviceHandle();

        if ( ! NT_SUCCESS(
                 WdfWorkItemCreate(
                     &m_WorkItemConfig,
                     &m_ObjectAttributes,
                     &m_WorkItem ) ) )
        {
            NT_ASSERT(FALSE);
            status = NDIS_STATUS_FAILURE;
            goto exit;
        }

        WFC_WORK_ITEM_CONTEXT* pWorkItemContext = GetWfcWorkItemContext( m_WorkItem );
        *pWorkItemContext = (WFC_WORK_ITEM_CONTEXT)this;

        m_pWorkItemCallback = pCallback;
        m_WorkItemContext = WorkItemContext;
        m_DebugInitialized = true;

    exit:
        TraceExit(status);
        return status;
    }

    void QueueWorkItem()
    {
        NT_ASSERT(
            !m_DebugPending && 
            m_DebugInitialized );

        m_DebugPending = true;
        WdfWorkItemEnqueue(
            m_WorkItem );
    }

    void WaitForCompletion()
    {
        WdfWorkItemFlush(
            m_WorkItem );
    }

    WDF_OBJECT_ATTRIBUTES m_ObjectAttributes;
    WDF_WORKITEM_CONFIG m_WorkItemConfig;
    WDFWORKITEM m_WorkItem;

    IWorkItemCallback* m_pWorkItemCallback;
    WFC_WORK_ITEM_CONTEXT m_WorkItemContext;

    bool m_DebugInitialized;
    bool m_DebugPending;
};

NDIS_STATUS IWorkItemContext::CreateInstance(
    _Outptr_ IWorkItemContext** ppWorkItemContext )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    IWorkItemContext* pToReturn = NULL;

    TraceEntry();

    if ( ppWorkItemContext == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    pToReturn = new CWdfWorkItemContext();

    if ( pToReturn == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    *ppWorkItemContext = pToReturn;
    pToReturn = NULL;

exit:
#pragma warning(suppress: 28922) // Redundant Pointer Test - yes it's redundant, but it's also safe if more error conditions are added above
    if (pToReturn != NULL)
    {
        delete pToReturn;
    }

    TraceExit(status);
    return status;
}


void InitializeCppListEntry(
    _In_ VOID* pThis,
    _Out_ CPP_LIST_ENTRY* pListEntry )
/*++

  CPP_LIST_ENTRY provides a wrapper around LIST_ENTRY which also
  contains a pThis pointer, the value of which is initialized
  appropriately by the class that contains it.

  --*/
{
    NT_ASSERT(pListEntry != NULL);

    if ( pListEntry != NULL )
    {
        InitializeListHead(
            &pListEntry->ListEntry );
        pListEntry->pThis = pThis;
    }
}

VOID* GetCppObjectFromListEntry(
    _In_ LIST_ENTRY* pListEntry )
/*++

  Wrapper to get pThis from a CPP_LIST_ENTRY, given
  CPP_LIST_ENTRY::LIST_ENTRY (which would be used by standard OS
  functions).

  --*/
{
    CPP_LIST_ENTRY* pCppEntry;

    NT_ASSERT(pListEntry != NULL);

    if ( pListEntry == NULL )
    {
        return NULL;
    }

    pCppEntry = CONTAINING_RECORD(
        pListEntry,
        CPP_LIST_ENTRY,
        ListEntry );

    return pCppEntry->pThis;
}
