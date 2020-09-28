/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    cpputil.hpp

Abstract:

    Header file for cpputil

Environment:

    Kernel mode

Author:


--*/
#pragma once


//
// This small object makes logistics of using KMDF, NDIS services simpler. 
// This is global, an adapter has a pointer to the global one of these
//
class CGlobalCtlDevice
{
public:
    CGlobalCtlDevice():
    m_WdfDevice( NULL )
    {
    };
    
    ~CGlobalCtlDevice()
    {
        if ( m_WdfDevice )
        {
            WdfObjectDelete( m_WdfDevice );
        }
    };

    NTSTATUS Initialize();

    WDFDEVICE GetDeviceHandle()
    {
        return m_WdfDevice;
    };

private:

    WDFDEVICE m_WdfDevice;
};

class CNdisSpinLock
{
public:
    CNdisSpinLock() :
        m_IsLocked(false)
    {
        NdisAllocateSpinLock(
            &m_SpinLock);
    }


    ~CNdisSpinLock()
    {
        NdisFreeSpinLock(
            &m_SpinLock);
    }

    _Requires_lock_not_held_(m_SpinLock.SpinLock)
        _Acquires_lock_(m_SpinLock.SpinLock)
        _IRQL_requires_max_(DISPATCH_LEVEL)
        _IRQL_raises_(DISPATCH_LEVEL)
        __drv_savesIRQLGlobal(NdisSpinLock, m_SpinLock.OldIrql)
        void EnterCriticalSection()
    {
        NdisAcquireSpinLock(
            &m_SpinLock);

        m_IsLocked = true;
    }

    _Requires_lock_held_(m_SpinLock.SpinLock)
        _Releases_lock_(m_SpinLock.SpinLock)
        __drv_restoresIRQLGlobal(NdisSpinLock, m_SpinLock.OldIrql)
        void LeaveCriticalSection()
    {
        m_IsLocked = false;

        NdisReleaseSpinLock(
            &m_SpinLock);
    }

    static NDIS_STATUS CreateInstance(
        _Out_ CNdisSpinLock** ppLockContext);

private:
    NDIS_SPIN_LOCK m_SpinLock;
    bool m_IsLocked;
};

class CAutoLock
{
public:
    _Acquires_lock_(m_pLockContext)
    CAutoLock(
        _In_ CNdisSpinLock* pLockContext ) :
        m_pLockContext(pLockContext)
    {
        m_pLockContext->EnterCriticalSection();
    }

    _Releases_lock_(m_pLockContext)
    ~CAutoLock()
    {
        m_pLockContext->LeaveCriticalSection();
    }

private:
    CAutoLock();
    CAutoLock(const CAutoLock&);

    CNdisSpinLock *m_pLockContext;
};


class IEventContext
{
public:
    virtual void Set() = 0;
    virtual void Reset() = 0;
    virtual BOOLEAN Wait(UINT MstoWait) = 0;

    static NDIS_STATUS CreateInstance(
        _Out_ IEventContext** ppEventContext );
};


class CDebugMetadata
{
public:
    static HANDLE get_CurrentThreadHandle();
};

class CSystem
{
private:
    static bool m_IsVerifierEnabled;
    static ULONG m_VerifierLevel;
    static PXDV_EXPORT_TABLE m_pVerifierCallbackTable;

    static const ULONG m_VerifierNdisWifiFlag = 0x00200000;
    static const ULONG m_VerifierWifiErrorCode = 0x93101;

public:
    static void Init();

    static bool get_IsRunningOnOlderOs();

    static ULONGLONG get_CurrentTime();

    static void XdvStatelessAssert(_In_z_ PCHAR ErrorMessage);
};


class ITimerCallback;
class ITimerContext
{
public:
    virtual ~ITimerContext() { }

    virtual NDIS_STATUS Initialize(
        _In_ CGlobalCtlDevice* pCtlDevice,
        _In_ ITimerCallback* pCallback,
        _In_ WDF_EXECUTION_LEVEL level) = 0;

    virtual NDIS_STATUS SetTimer(
        _In_ ULONG RelativeTimeoutMs,
        _In_opt_ WFC_TIMER_CONTEXT TimerContext ) = 0;

    virtual bool CancelTimer() = 0;

    static NDIS_STATUS CreateInstance(
        _Out_ ITimerContext** ppTimerContext );
};

class IWorkItemCallback;
class IWorkItemContext
{
public:
    virtual ~IWorkItemContext() { }

    virtual NDIS_STATUS Initialize(
        _In_ CGlobalCtlDevice* pCtlDevice,
        _In_ IWorkItemCallback *pCallback,
        _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext = NULL ) = 0;

    virtual void QueueWorkItem() = 0;
    virtual void WaitForCompletion() = 0;

    static NDIS_STATUS CreateInstance(
        _Outptr_ IWorkItemContext** ppWorkItemContext );
};


typedef struct _CPP_LIST_ENTRY
{
    LIST_ENTRY ListEntry;
    VOID* pThis;
} CPP_LIST_ENTRY;

void InitializeCppListEntry(
    _In_ VOID* pThis,
    _Out_ CPP_LIST_ENTRY* pListEntry );

VOID* GetCppObjectFromListEntry(
    _In_ LIST_ENTRY* pListEntry );

typedef struct _PLACEMENT_NEW_ALLOCATION_CONTEXT
{
                                  size_t cbMaxSize;
    _Field_size_bytes_(cbMaxSize) PVOID  pbBuffer;
} PLACEMENT_NEW_ALLOCATION_CONTEXT, *PPLACEMENT_NEW_ALLOCATION_CONTEXT;
typedef const PLACEMENT_NEW_ALLOCATION_CONTEXT * PCPLACEMENT_NEW_ALLOCATION_CONTEXT;

_Ret_writes_bytes_maybenull_( _Size )
void * PlacementNewHelper( size_t _Size, PCPLACEMENT_NEW_ALLOCATION_CONTEXT AllocationContext);

__forceinline void * __cdecl operator new(size_t _Size, ULONG_PTR AllocationContext)
{
    if (AllocationContext != NULL)
    {
        return PlacementNewHelper( _Size, (PPLACEMENT_NEW_ALLOCATION_CONTEXT)AllocationContext );
    }
    return ::operator new(_Size);
}
