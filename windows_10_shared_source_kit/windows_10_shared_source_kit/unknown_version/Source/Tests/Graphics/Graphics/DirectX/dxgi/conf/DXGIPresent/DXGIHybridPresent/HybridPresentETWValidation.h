/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   HybridPresentETWValidation.h
//
//	Abstract:
//	This file contains declarations for ETW consumer classes used to validate Hybrid functionality
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
// ETW-related headers
#include <etwConsumersRequire.h>
#include "DxgKernelEventHelper.h"

#include <lddmcoreeventdefs.h>
// This is a private version of the file that DxgKernel used to use when it logged Classic 
// events.

//-------------------------------------------------------------
// Definitions for D3D kernel mode DDI
//
typedef struct _MDL {
    struct _MDL *MdlNext;
    short MdlSize;
    short MdlFlags;
    struct _EPROCESS *Process;
    ULONG *lpMappedSystemVa;
    ULONG *lpStartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL;
typedef MDL *PMDL;

typedef LONG NTSTATUS;

#ifndef PHYSICAL_ADDRESS
typedef LARGE_INTEGER PHYSICAL_ADDRESS;
#endif
//This is the DxgkEscape DRT Escape type signature
const UINT TEST_DRT_ESCAPE_SIGNATURE = 'XtrD';

#include "d3dkmddi.h"
#include "d3dkmthk.h"
#include "d3dkmthk_p.h"
#include "d3dukmdt.h"
#include "dmmdiag.h"
#include "dxetwevt.hxx"

#include "DxgKernelConsumer.h"
#include <TFrameworkEventHelper.h>
#include "TFrameworkConsumer.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include "Controller.h"

#include "ETWProvider.h"

using namespace std;

#define HYBRID_POWER_MANAGEMENT_VALIDATE L"HybridPowerManagementValidate"
#define HYBRID_PRESENT_WAIT_FOR_EVENTS L"HybridPresentWaitForEvents"
#define HYBRID_PRESENT_WAIT_FOR_EVENTS_TIMEOUT 5000

#define DXGK_ALLOCATIONINFOFLAGS15 0x100000 //defined in d3dkmddi_pext.h

class CSLocker;

class CritSection
{
    friend class CSLocker;
public: 
    CritSection() :m_RefCount(0){InitializeCriticalSection(&m_CritSection);}
    ~CritSection() { DeleteCriticalSection(&m_CritSection); }
    void Lock();
    void Unlock();

private:
    CRITICAL_SECTION m_CritSection;
    UINT m_RefCount;

    CSLocker Enter();
};

class CSLocker
{
private:
    CritSection& m_Target;
    bool         m_bOwns;
        
public:
    CSLocker(CritSection& Target);
    CSLocker(CSLocker& RHS);
    ~CSLocker();
};



///////////////////////////////////////////////////////////////////////
// Tracking structures
///////////////////////////////////////////////////////////////////////

class CGPUPowerComponent;
class CAdapter;

typedef std::list<HANDLE> HandleList;

class CAllocation
{
public:
    CAllocation(HANDLE hDevice, 
                            UINT Flags, 
                            ULONGLONG Size,
                            ULONG Alignment,
                            UINT SupportedSegmentSet,
                            UINT PreferredSegments,
                            ULONG Priority,
                            HANDLE hVidMmGlobalHandle,
                            HANDLE hDxgAllocationHandle)
        :m_hDevice(hDevice),
         m_Size(Size),
         m_Alignment(Alignment),
         m_SupportedSegmentSet(SupportedSegmentSet),
         m_PreferredSegments(PreferredSegments),
         m_Priority(Priority),
         m_hVidMMGlobalHandle(hVidMmGlobalHandle),
         m_hDxgAllocationHandle(hDxgAllocationHandle),
         m_bDelete(false),
         m_bCommitted(false)
    {
        m_Flags.Value = Flags;
    }
    
    ~CAllocation(){};

    HANDLE m_hDevice;
    DXGKETW_ALLOCATIONFLAGS m_Flags;
    ULONGLONG m_Size;
    ULONG m_Alignment;
    UINT m_SupportedSegmentSet;
    UINT m_PreferredSegments;
    ULONG m_Priority;
    HANDLE m_hVidMMGlobalHandle;
    HANDLE m_hDxgAllocationHandle;
    bool m_bDelete;
    bool m_bCommitted;
};


typedef CAllocation CCrossAdapterAllocation;

class CMemorySegment
{
public:
    CMemorySegment(UINT ID, SIZE_T Size, UINT Flags)
        : m_ID(ID),
          m_Size(Size),
          m_BytesAllocated(0),
          m_pPC(NULL)
    {
        m_Flags.Value = Flags;
    }
    ~CMemorySegment(){};

    UINT m_ID;
    SIZE_T m_Size;
    DXGK_SEGMENTFLAGS m_Flags;
    ULONGLONG m_BytesAllocated;
    CGPUPowerComponent* m_pPC;
};

class CDevice
{
public:
    CDevice(HANDLE hDevice, HANDLE hProcess)
        :m_hDeviceHandle(hDevice),
         m_hProcess(hProcess)
    {}
    ~CDevice();
    HANDLE m_hDeviceHandle;
    HANDLE m_hProcess;
    HandleList m_hContextList;

    HANDLE FindContext(HANDLE hContext);
};

class CNode
{
public:
    CNode(UINT NodeOrdinal)
        : m_NodeOrdinal(NodeOrdinal),
          m_bActive(false),
          m_pPC(NULL)
    {}

    ~CNode();

    UINT m_NodeOrdinal;
    bool m_bActive;
    CGPUPowerComponent* m_pPC;
    HandleList m_hContextList;
};


class CDState
{
public:
    CDState()
        : m_PowerState(PowerDeviceUnspecified),
          m_TransitionToPowerState(PowerDeviceUnspecified),
          m_TransitionCountFromD3(0),
          m_MinActualLatencyFromD3(0),
          m_MaxActualLatencyFromD3(0),
          m_TotalActualLatencyFromD3(0),
          m_TransitionCountToD3(0),
          m_MinActualLatencyToD3(0),
          m_MaxActualLatencyToD3(0),
          m_TotalActualLatencyToD3(0),
          m_TransitionStartTS(0),
          m_TransitionStartDPCTotalTime(0),
          m_TransitionStartInterruptTotalTime(0)
    {
    }

    DEVICE_POWER_STATE m_PowerState;
    DEVICE_POWER_STATE m_TransitionToPowerState;

    UINT m_TransitionCountFromD3;
    ULONGLONG m_MinActualLatencyFromD3;
    ULONGLONG m_MaxActualLatencyFromD3;
    ULONGLONG m_TotalActualLatencyFromD3;

    UINT m_TransitionCountToD3;
    ULONGLONG m_MinActualLatencyToD3;
    ULONGLONG m_MaxActualLatencyToD3;
    ULONGLONG m_TotalActualLatencyToD3;

    ULONGLONG m_TransitionStartTS;
    ULONGLONG m_TransitionStartDPCTotalTime;
    ULONGLONG m_TransitionStartInterruptTotalTime;
};

class CFState
{
public:
    CFState(UINT FState, UINT ComponentIndex, ULONGLONG ReportedLatency)
        : m_FState(FState),
          m_ComponentIndex(ComponentIndex),
          m_ReportedLatency(ReportedLatency),
          m_TransitionCountFromF0(0),
          m_MinActualLatencyFromF0(0),
          m_MaxActualLatencyFromF0(0),
          m_TotalActualLatencyFromF0(0),
          m_TransitionCountToF0(0),
          m_MinActualLatencyToF0(0),
          m_MaxActualLatencyToF0(0),
          m_TotalActualLatencyToF0(0)
    {
    };

    UINT m_FState;
    UINT m_ComponentIndex;
    ULONGLONG m_ReportedLatency;

    ULONGLONG m_TransitionCountFromF0;
    ULONGLONG m_MinActualLatencyFromF0;
    ULONGLONG m_MaxActualLatencyFromF0;
    ULONGLONG m_TotalActualLatencyFromF0;

    ULONGLONG m_TransitionCountToF0;
    ULONGLONG m_MinActualLatencyToF0;
    ULONGLONG m_MaxActualLatencyToF0;
    ULONGLONG m_TotalActualLatencyToF0;
};

// Latency tolerances (all times in 100s ns)
// Engine
#define LT_ENGINE_MONITORON_INITIAL 80	* 10
#define LT_ENGINE_MONITORON_IDLE 15000	* 10	
#define LT_ENGINE_MONITORON_NOCONTEXT 35000	* 10
#define LT_ENGINE_MONITOROFF_INITIAL 2000 * 10
#define LT_ENGINE_MONITOROFF_IDLE 50000	* 10
#define LT_ENGINE_MONITOROFF_NOCONTEXT 100000 * 10
//Memory
#define LT_MEMORY_CONTEXT 15000	* 10
#define LT_MEMORY_NOCONTEXT 30000 * 10
//MemoryRefresh
#define LT_MEMORYREFRESH_MONITORON_INITIAL 80 * 10
#define LT_MEMORYREFRESH_MONITORON_IDLE 15000 * 10
#define LT_MEMORYREFRESH_MONITORON_NOCONTEXT 30000 * 10
#define LT_MEMORYREFRESH_MONITOROFF_INITIAL 80 * 10
#define LT_MEMORYREFRESH_MONITOROFF_IDLE 80000 * 10
#define LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT 80000 * 10
//D3 transition
#define LT_D3TRANSITION_MONITORON_INITIAL 80 * 10
#define LT_D3TRANSITION_MONITORON_IDLE 80000 * 10
#define LT_D3TRANSITION_MONITORON_NOCONTEXT 250000 * 10
#define LT_D3TRANSITION_MONITOROFF_INITIAL 80 * 10
#define LT_D3TRANSITION_MONITOROFF_IDLE 250000 * 10
#define LT_D3TRANSITION_MONITOROFF_NOCONTEXT 250000 * 10

//Idle times that trigger latency tolerance changes (all times in 100s ns) * 10% error margine
// Engine
#define ENGINE_IDLE_TIME 200000 * 10 * 0.9f
#define ALL_ENGINES_IDLE_TIME_1 10000000 * 10 * 1.1f //10s
#define ALL_ENGINES_IDLE_TIME_2 20000000 * 10 * 1.1f //20s
// MemoryRefresh
#define MEMORYREFRESH_IDLE_TIME 2000000 * 10 * 0.9f


class CLatencyToleranceState
{
public:
    CLatencyToleranceState(ULONGLONG InitialLatencyTolerance)
        : m_ActiveTimeStamp(0),
          m_IdleTimeStamp(0),
          m_bMonitorOn(TRUE),
          m_ActiveContextsCount(0),
          m_LatencyTolerance(InitialLatencyTolerance),
          m_LatencyToleranceTimeStamp(0),
          m_ExpectedLatencyTolerance(0),
          m_CurrentTimeStamp(0)
    {}

    ULONGLONG m_ActiveTimeStamp;
    ULONGLONG m_IdleTimeStamp;
    BOOL m_bMonitorOn;
    UINT m_ActiveContextsCount;
    ULONGLONG m_LatencyTolerance;
    ULONGLONG m_LatencyToleranceTimeStamp;
    ULONGLONG m_ExpectedLatencyTolerance;
    ULONGLONG m_CurrentTimeStamp;
};

enum ePowerComponentState
{
    Idle = 0,
    Active = 1
};

typedef std::list<CFState*> FStateList;
typedef std::list<CLatencyToleranceState*> LatencyToleranceViolationList;

class CHybridETWMaster;

class CGPUPowerComponent
{
public:
    CGPUPowerComponent(CAdapter* pAdapter, UINT Type, UINT Index, UINT ComponentTypeId, UINT FStateCount);
    ~CGPUPowerComponent();

    UINT m_Type;
    UINT m_Index;
    UINT m_ComponentTypeId; //mapping
    void* m_pComponent;	// component that this power component is mapped to
    UINT m_CurrentFState;
    UINT m_FStateCount;
    ULONGLONG m_CurrentFStateTimeStamp;
    UINT m_RequestedFState;
    ULONGLONG m_RequestedFStateTimeStamp;
    ePowerComponentState m_State;
    ULONGLONG m_StateTimeStamp;
    ULONGLONG m_LatencyTolerance;
    FStateList m_FStateList;
    CLatencyToleranceState* m_pLatencyToleranceState;
    LatencyToleranceViolationList m_LatencyToleranceViolationList;
    CAdapter* m_pAdapter;
    DXGK_POWER_COMPONENT_FLAGS m_Flags;
    UINT m_ComponentTypeNameIndex;

    CFState* GetFState(UINT FState);
    void ValidateLatencyTolerance(ULONGLONG Timestamp, CHybridETWMaster* pETWMaster);
    void UpdateActiveStatus(ULONGLONG Timestamp);
};


typedef std::list<CCrossAdapterAllocation*> CrossAdapterAllocationList;
typedef std::list<CAllocation*> AllocationList;
typedef std::list<CMemorySegment*> MemorySegmentList;
typedef std::list<CDevice*> DeviceList;
typedef std::list<CGPUPowerComponent*> PowerComponentList;
typedef std::list<CNode*> NodeList;
typedef std::list<WCHAR*> StringList;

class CAdapter
{
public: 
    CAdapter(HANDLE hAdapter, bool bDiscrete)
        :m_hAdapter(hAdapter),
         m_bDiscrete(bDiscrete),
         m_bBDD(false),
         m_bAdapterState(FALSE),
         m_AdapterStateTimestamp(0),
         m_PagingNodeOrdinal(0xFFFFFFFF),
         m_numActiveContexts(0),
         m_hMiniportContext(NULL)
    {}
    ~CAdapter();
    bool IsDiscrete(){return m_bDiscrete;}
    bool IsBDD(){return m_bBDD;}
    bool IsApertureOnly(UINT SegmentSet);
    CDevice* FindTrackedDevice(HANDLE hDevice);
    HANDLE FindDevice(HANDLE hDevice);
    CNode* GetNode(UINT Node);
    CMemorySegment* GetMemorySegment(UINT SegmentID);
    CAllocation* FindAllocation(HANDLE hGlobalAlloc);
    CGPUPowerComponent* GetPowerComponent(UINT Index);
    bool AddProcessName(WCHAR* strName);

    HANDLE m_hAdapter;
    HANDLE m_hMiniportContext;
    DeviceList m_DeviceList; // tracked devices
    HandleList m_AllDevices; // all devices created on this adapter
    NodeList m_NodeList;
    int m_PagingNodeOrdinal;
    UINT m_numActiveContexts;
    CDState m_DState;	// Current D-State of the GPU


    // Cross-Adapter allocations
    CrossAdapterAllocationList m_CrossAdapterAllocationList;

    // All allocations - these are used to track segment content
    AllocationList m_AllocationList;
    PowerComponentList m_PowerComponentList;
    BOOL m_bAdapterState;	// TRUE if active, FALSE if idle (D3 state)
    ULONGLONG m_AdapterStateTimestamp;	// Timestamp of the state change
    StringList m_ProcessNameList;	// names of processes that ran on this adapter during test execution

    // Segments
    MemorySegmentList m_MemorySegmentList;
    bool m_bDiscrete;
    bool m_bBDD;
};


////////////////////////////////////////////////////////////////////////////////
// ETW Master
// ETW Master manages multiple ETW consumers as well as Test Provider. 
// It processes data from the events that consumers receive.
////////////////////////////////////////////////////////////////////////////////
class CDxgKrnlHybridConsumer;
class CTestHybridConsumer;
class CHybridETWProvider;

typedef std::list<CAdapter*> AdapterList;

class CHybridETWMaster
{
public:
    CHybridETWMaster();
    ~CHybridETWMaster();
    UINT GetCrossAdapterPresentsCount() {return m_CrossAdapterPresentsCount;}
    UINT GetRegularPresentsCount() {return m_RegularPresentsCount;}
    void ResetPresentCounters();
    HRESULT TrackProcess(HANDLE hProcess);
    void StartListening();
    void StopListening();
    bool WaitForEvents();
    bool WaitForEvents(UINT Timeout);
    bool PowerManagementValidate();
    HANDLE* GetProcessIDByName(CHAR* szName);
    WCHAR* GetProcessNameByHandle(HANDLE hProcess);
    void CleanupAllocations();
    CAdapter* GetDGpu();
    void Cleanup();

    UINT m_CrossAdapterPresentsCount;
    UINT m_RegularPresentsCount;

    AdapterList m_Adapters;
    D3DKMT_PRESENT_MODEL m_PresentModel;
    bool m_bConsumersAdded;
    ULONGLONG m_InterruptStartTS;
    ULONGLONG m_InterruptTimeTotal;
    ULONGLONG m_DPCStartTS;
    ULONGLONG m_DPCTimeTotal;

    CritSection m_CritSection;

    // event handlers
    void HandleAdapterAllocation(PEVENT_RECORD pEvent);
    void HandleReportSegment(PEVENT_RECORD pEvent);
    void HandleAdapter(PEVENT_RECORD pEvent);
    void HandleDevice(PEVENT_RECORD pEvent);
    void HandleContext(PEVENT_RECORD pEvent);
    void HandlePresent(PEVENT_RECORD pEvent);
    void HandlePresentHistory(PEVENT_RECORD pEvent);
    void HandlePresentHistoryDetailed(PEVENT_RECORD pEvent);
    void HandleRuntimePowerManagement(PEVENT_RECORD pEvent);
    void HandlePagingOpFill(PEVENT_RECORD pEvent);
    void HandlePagingOpDiscard(PEVENT_RECORD pEvent);
    void HandlePagingOpTransfer(PEVENT_RECORD pEvent);
    void HandleReportCommittedGlobalAllocation(PEVENT_RECORD pEvent);
    void HandleDdiSetPowerState(PEVENT_RECORD pEvent);
    void HandleNodeMetadata(PEVENT_RECORD pEvent);
    void HandleTotalBytesResidentInSegment(PEVENT_RECORD pEvent);
    void HandleProfiler(PEVENT_RECORD pEvent);

    void HandleGraphicsTestMessage(PEVENT_RECORD pEvent);

    void PrintAdapters();
    void ProcessPowerEvents(bool bProcess) {m_bProcessPowerEvents = bProcess;}

    void LogTestETW(WCHAR* str);

private:
    CDxgKrnlHybridConsumer* m_pDxgKrnlListener;
    CTestHybridConsumer* m_pTestListener;
    CHybridETWProvider* m_pTestProvider;
    bool m_bListening;
    HANDLE m_hListeningThread;
    HANDLE m_hWaitForEvents;
    bool m_bProcessPowerEvents;
    CController m_etwController;
    static DWORD EtwListeningThreadProc(void* context);

    CAdapter* GetAdapter(HANDLE hAdapter);

    // structures to only track events that belong to our test
    HandleList           m_TrackedProcessList;

    bool IsTrackedDevice(HANDLE hDevice);
    bool IsTrackedProcess(HANDLE hProcess);
    bool IsTrackedContext(HANDLE hContext);
};

struct ETWListeningThreadProcArgs
{
    CHybridETWMaster* pMaster;
    CDxgKrnlHybridConsumer* pDxgKrnlListener;
    CTestHybridConsumer* pTestListener;
};

///////////////////////////////////////////////////////////////////////////////
//	ETW Consumers
//	Consumers receive events and re-direct them to ETW Master for processing
///////////////////////////////////////////////////////////////////////////////
class CDxgKrnlHybridConsumer : public CDxgKernelConsumer
{
public:
    CDxgKrnlHybridConsumer(CHybridETWMaster* pMaster):m_pETWMaster(pMaster){};

private:
    CHybridETWMaster* m_pETWMaster;

    void AdapterAllocation_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleAdapterAllocation(pEvent);}
    void ReportSegment_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleReportSegment(pEvent);}
    void Adapter_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleAdapter(pEvent);} 
    void Device_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleDevice(pEvent);}
    void Present_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresent(pEvent);}
    void Context_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleContext(pEvent);}
    void PresentHistory_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresentHistory(pEvent);}
    void PresentHistoryDetailed_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePresentHistoryDetailed(pEvent);}
    void RuntimePowerManagement_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleRuntimePowerManagement(pEvent);}
    void PagingOpFill_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpFill(pEvent);}
    void PagingOpDiscard_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpDiscard(pEvent);}
    void PagingOpTrgansfer_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpTransfer(pEvent);}
    void PagingOpSpecialLockTransfer_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandlePagingOpTransfer(pEvent);}
    void ReportCommittedGlobalAllocation_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleReportCommittedGlobalAllocation(pEvent);}
    void DdiSetPowerState_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleDdiSetPowerState(pEvent);}
    void NodeMetadata_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleNodeMetadata(pEvent);}
    void TotalBytesResidentInSegment_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleTotalBytesResidentInSegment(pEvent);}
    void Profiler_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleProfiler(pEvent);}
};


class CTestHybridConsumer : public CTFrameworkConsumer
{
public:
    CTestHybridConsumer(CHybridETWMaster* pMaster):m_pETWMaster(pMaster){};

private:
    CHybridETWMaster* m_pETWMaster;
    void GraphicsTestMessage_Handler(PEVENT_RECORD pEvent){m_pETWMaster->HandleGraphicsTestMessage(pEvent);}
};

//////////////////////////////////////////////////////////////////////////////
// Test ETW provider
//////////////////////////////////////////////////////////////////////////////
class CHybridETWProvider : public CEtwProvider
{
public:
    ULONG WriteToETW(__in const LPWSTR pwszMessage, ...);
};

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////
inline WCHAR* GetArgs(const WCHAR *pwszFmt, va_list argList)
{
    const int   nAllocSize = 128;
    UINT        nAlloc     = nAllocSize;
    bool        bSuccess   = false;
    WCHAR*      pwszOut = NULL;

    // preprocess the string with the values from the variable argument list.
    while(!bSuccess)
    {
        pwszOut = new WCHAR[nAlloc];

        if(!pwszOut)
            return L"Error formatting string. Out of memory.";

        int nChars = _vsnwprintf(pwszOut, nAlloc, pwszFmt, argList);

        if((nChars < 0) || (nChars >= (int)nAlloc-1)) // we need to alloc more memory
        {
            delete[] pwszOut;
            nAlloc += nAllocSize;
        }
        else
        {
            //add \0 to the string
            pwszOut[nChars] = '\0';
            wcscat(pwszOut, L"\n");
            bSuccess = true;
        }
    }
    
    return pwszOut;
}

inline void DBGPrint( __in const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    OutputDebugStringW(pwszOut);

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}

