/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   HybridPresentETWValidation.cpp
//
//	Abstract:
//	This file contains implementation of HybridPresent ETW consumers that validate
//  Hybrid functionality
//
//	History:
//	11/14/2012	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "HybridPresentETWValidation.h"
#include <tlhelp32.h>

extern HWND* g_pTestWindow;
const UINT g_ProcessArraySize = 2;
TCHAR* g_PowerComponentTypeNames[] = {_T("DXGK_POWER_COMPONENT_ENGINE"), _T("DXGK_POWER_COMPONENT_MEMORY"), _T("DXGK_POWER_COMPONENT_MEMORY_REFRESH"), _T("DXGK_POWER_COMPONENT_D3_TRANSITION"), _T("OTHER")};

CAdapter::~CAdapter()
{
    if(!m_CrossAdapterAllocationList.empty())
    {
        for(CrossAdapterAllocationList::iterator it = m_CrossAdapterAllocationList.begin();
            it != m_CrossAdapterAllocationList.end();
            it++)
        {
            CCrossAdapterAllocation* pAllocation = *it;
            delete pAllocation;
        }

        m_CrossAdapterAllocationList.clear();
    }

    if(!m_AllocationList.empty())
    {
        for(AllocationList::iterator it = m_AllocationList.begin();
            it != m_AllocationList.end();
            it++)
        {
            CAllocation* pAllocation = *it;
            delete pAllocation;
        }

        m_AllocationList.clear();
    }

    if(!m_MemorySegmentList.empty())
    {
        for(MemorySegmentList::iterator it = m_MemorySegmentList.begin();
            it != m_MemorySegmentList.end();
            it++)
        {
            CMemorySegment* pSegment = *it;
            delete pSegment;
        }

        m_MemorySegmentList.clear();
    }

    if(!m_DeviceList.empty())
    {
        for(DeviceList::iterator it = m_DeviceList.begin();
            it != m_DeviceList.end();
            it++)
        {
            CDevice* pDevice = *it;
            delete pDevice;
        }
        m_DeviceList.clear();
    }

    if(!m_NodeList.empty())
    {
        for(NodeList::iterator it = m_NodeList.begin();
            it != m_NodeList.end();
            it++)
        {
            CNode* pNode = *it;
            delete pNode;
        }
        m_NodeList.clear();
    }

    if(!m_PowerComponentList.empty())
    {
        for(PowerComponentList::iterator it = m_PowerComponentList.begin();
            it != m_PowerComponentList.end();
            it++)
        {
            CGPUPowerComponent* pPC = *it;
            delete pPC;
        }
        m_PowerComponentList.clear();
    }

    if(!m_ProcessNameList.empty())
    {
        for(StringList::iterator it = m_ProcessNameList.begin();
            it != m_ProcessNameList.end();
            it++)
        {
            WCHAR* str = *it;
            delete str;
        }
        m_ProcessNameList.clear();
    }

    if(!m_AllDevices.empty())
    {
        m_AllDevices.clear();
    }
}

bool CAdapter::IsApertureOnly(UINT SegmentSet)
{
    UINT i = 0;

    for(MemorySegmentList::iterator it = m_MemorySegmentList.begin();
        it != m_MemorySegmentList.end();
        it++, i++)
    {
        CMemorySegment* pSegment = *it;

        UINT SegmentMask = 1 << i;
        if(SegmentSet & SegmentMask)
        {
            if(!pSegment->m_Flags.Aperture) return false;
        }
    }

    return true;
}

CDevice* CAdapter::FindTrackedDevice(HANDLE hDevice)
{
    for(DeviceList::iterator it = m_DeviceList.begin();
        it != m_DeviceList.end();
        it++)
    {
        CDevice* pDevice = *it;

        if(pDevice->m_hDeviceHandle == hDevice)
            return pDevice;
    }

    return NULL;
}

HANDLE CAdapter::FindDevice(HANDLE hDevice)
{
    for(HandleList::iterator it = m_AllDevices.begin();
        it != m_AllDevices.end();
        it++)
    {
        HANDLE hd = *it;

        if(hd == hDevice)
            return hd;
    }

    return NULL;
}

CNode* CAdapter::GetNode(UINT Node)
{
    for(NodeList::iterator it = m_NodeList.begin();
        it != m_NodeList.end();
        it++)
    {
        CNode* pNode = *it;

        if(pNode->m_NodeOrdinal == Node)
            return pNode;
    }

    return NULL;
}

CMemorySegment* CAdapter::GetMemorySegment(UINT SegmentID)
{
    for(MemorySegmentList::iterator it = m_MemorySegmentList.begin();
        it != m_MemorySegmentList.end();
        it++)
    {
        CMemorySegment* pSegment = *it;

        if(pSegment->m_ID == SegmentID)
            return pSegment;
    }

    return NULL;
}

CGPUPowerComponent* CAdapter::GetPowerComponent(UINT Index)
{
    for(PowerComponentList::iterator it = m_PowerComponentList.begin();
        it != m_PowerComponentList.end();
        it++)
    {
        CGPUPowerComponent* pPC = *it;

        if(pPC->m_Index == Index)
            return pPC;
    }

    return NULL;
}


CAllocation* CAdapter::FindAllocation(HANDLE hGlobalAlloc)
{
    for(AllocationList::iterator it = m_AllocationList.begin();
        it != m_AllocationList.end();
        it++)
    {
        CAllocation* pAllocation = *it;

        if(pAllocation->m_hVidMMGlobalHandle == hGlobalAlloc)
            return pAllocation;
    }

    return NULL;
}


bool CAdapter::AddProcessName(WCHAR* strName)
{
    bool bFound = false;

    for(StringList::iterator it = m_ProcessNameList.begin();
        it != m_ProcessNameList.end();
        it++)
    {
        WCHAR* str = *it;
        if(!wcscmp(str, strName))
        {
            bFound = true;
            break;
        }
    }

    if(!bFound)
        m_ProcessNameList.push_back(strName);

    return !bFound;
}

CDevice::~CDevice()
{
    if(!m_hContextList.empty())
    {
        m_hContextList.clear();
    }
}

HANDLE CDevice::FindContext(HANDLE hContext)
{
    HandleList::iterator it = m_hContextList.begin();

    for(it; it != m_hContextList.end(); it++)
    {
        if((*it) == hContext)
            return hContext;
    }

    return NULL;
}

CNode::~CNode()
{
    if(!m_hContextList.empty())
    {
        m_hContextList.clear();
    }
}

CGPUPowerComponent::CGPUPowerComponent(CAdapter* pAdapter, UINT Type, UINT Index, UINT ComponentTypeId, UINT FStateCount)
    : m_pAdapter(pAdapter),
      m_Type(Type),
      m_Index(Index),
      m_ComponentTypeId(ComponentTypeId),
      m_FStateCount(FStateCount),
      m_pComponent(NULL)
{
    ULONGLONG InitialLatencyTolerance = 0;

    switch(m_Type)
    {
    case DXGK_POWER_COMPONENT_ENGINE: InitialLatencyTolerance = LT_ENGINE_MONITORON_NOCONTEXT; m_ComponentTypeNameIndex = 0; break;
    case DXGK_POWER_COMPONENT_MEMORY: InitialLatencyTolerance = LT_MEMORY_NOCONTEXT; m_ComponentTypeNameIndex = 1; break;
    case DXGK_POWER_COMPONENT_MEMORY_REFRESH: InitialLatencyTolerance = LT_MEMORYREFRESH_MONITORON_NOCONTEXT; m_ComponentTypeNameIndex = 2; break;
    case DXGK_POWER_COMPONENT_D3_TRANSITION: InitialLatencyTolerance = LT_D3TRANSITION_MONITORON_NOCONTEXT; m_ComponentTypeNameIndex = 3; break;
    default: m_ComponentTypeNameIndex = 4; break;
    }

    m_pLatencyToleranceState = new CLatencyToleranceState(InitialLatencyTolerance);

    if(!m_pLatencyToleranceState)
        throw std::exception("CGPUPowerComponent - failed to allocate memory");
}

CGPUPowerComponent::~CGPUPowerComponent()
{
    if(!m_FStateList.empty())
    {
        for(FStateList::iterator it = m_FStateList.begin();
            it != m_FStateList.end();
            it++)
        {
            CFState* pFState = *it;
            delete pFState;
        }
        m_FStateList.clear();
    }

    if(!m_LatencyToleranceViolationList.empty())
    {
        for(LatencyToleranceViolationList::iterator it = m_LatencyToleranceViolationList.begin();
            it != m_LatencyToleranceViolationList.end();
            it++)
        {
            CLatencyToleranceState* pLT = *it;
            delete pLT;
        }
        m_LatencyToleranceViolationList.clear();
    }

    if(m_pLatencyToleranceState)
        delete m_pLatencyToleranceState;
}

CFState* CGPUPowerComponent::GetFState(UINT FState)
{
    for(FStateList::iterator it = m_FStateList.begin();
        it != m_FStateList.end();
        it++)
    {
        CFState* pFState = *it;

        if(pFState->m_FState == FState)
            return pFState;
    }

    return NULL;
}

void CGPUPowerComponent::UpdateActiveStatus(ULONGLONG TimeStamp)
{
    bool bChanged = false;

    switch(m_Type)
    {
    case DXGK_POWER_COMPONENT_ENGINE:
        {
            // status should be up to date
            break;
        }
    case DXGK_POWER_COMPONENT_MEMORY:
        {
            // Component is Active if there is any memory in segment associated with it
            CMemorySegment* pSegment = (CMemorySegment*) m_pComponent;

            if(!pSegment)
                throw std::exception("CGPUPowerComponent::UpdateActiveStatus - no association with memory segment");

            if(pSegment->m_BytesAllocated)
            {
                m_State = Active;
                m_pLatencyToleranceState->m_ActiveTimeStamp = TimeStamp;
            }
            else
            {
                m_State = Idle;
                m_pLatencyToleranceState->m_IdleTimeStamp = TimeStamp;
            }

            break;
        }
    case DXGK_POWER_COMPONENT_MEMORY_REFRESH:
        {
            bool bActive = false;
            // If any of the engines are active, this component should be active
            NodeList::iterator it = m_pAdapter->m_NodeList.begin();

            for(it; it != m_pAdapter->m_NodeList.end(); it++)
            {
                CNode* pNode = *it;

                // TODO:remove this once SoftGPU reports PCs for all nodes
                if(pNode->m_NodeOrdinal > 2)
                    continue;

                CGPUPowerComponent* pPC = pNode->m_pPC;

                if(!pPC)
                    throw std::exception("CGPUPowerComponent::UpdateActiveStatus - node not associated with power component");

                if(pPC->m_State == Active)
                {
                    bActive = true;
                    break;
                }
            }

            if(bActive)
            {
                m_pLatencyToleranceState->m_ActiveTimeStamp = TimeStamp;
                m_State = Active;
            }
            else
            {
                m_State = Idle;
                m_pLatencyToleranceState->m_IdleTimeStamp = TimeStamp;
            }

            break;
        }
    case DXGK_POWER_COMPONENT_D3_TRANSITION:
        {
            // status should be up to date
            break;
        }
    }		
}

void CGPUPowerComponent::ValidateLatencyTolerance(ULONGLONG Timestamp, CHybridETWMaster* pETWMaster)
{
    // Validate expected latency tolerance given the state of the power components
    bool bFail = false;
    ULONGLONG IdleTime = (m_pLatencyToleranceState->m_IdleTimeStamp>m_pLatencyToleranceState->m_ActiveTimeStamp)
                         ? (Timestamp - m_pLatencyToleranceState->m_IdleTimeStamp)
                         : 0;

    // If there were no updates to idle/active state we don't know for certain which latency tolerance to expect
    if(!m_pLatencyToleranceState->m_IdleTimeStamp && !m_pLatencyToleranceState->m_ActiveTimeStamp)
        return;

    m_pLatencyToleranceState->m_CurrentTimeStamp = Timestamp;

    switch(m_Type)
    {
    case DXGK_POWER_COMPONENT_ENGINE:
        {

            if(m_pLatencyToleranceState->m_ActiveContextsCount && (!IdleTime || IdleTime < ENGINE_IDLE_TIME))
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITORON_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITORON_INITIAL;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITOROFF_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITOROFF_INITIAL;
                    bFail = true;
                }
            }
            else if(m_pLatencyToleranceState->m_ActiveContextsCount && (IdleTime > ENGINE_IDLE_TIME))
            {
                if((m_pLatencyToleranceState->m_LatencyToleranceTimeStamp - m_pLatencyToleranceState->m_IdleTimeStamp) > ENGINE_IDLE_TIME)
                {
                    bFail = true;
                }
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITORON_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITORON_IDLE;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITOROFF_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITOROFF_IDLE;
                    bFail = true;
                }
            }
            else if(!m_pLatencyToleranceState->m_ActiveContextsCount)
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITORON_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    bFail = true;
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITORON_NOCONTEXT;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_ENGINE_MONITOROFF_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_ENGINE_MONITOROFF_NOCONTEXT;
                    bFail = true;
                }
            }
            
            break;
        }
    case DXGK_POWER_COMPONENT_MEMORY:
        {
            if(m_pLatencyToleranceState->m_ActiveContextsCount && (LT_MEMORY_CONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
            {
                m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORY_CONTEXT;
                bFail = true;
            }
            else if(!m_pLatencyToleranceState->m_ActiveContextsCount && (LT_MEMORY_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
            {
                m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORY_NOCONTEXT;
                bFail = true;
            }
            break;
        }
    case DXGK_POWER_COMPONENT_MEMORY_REFRESH:
        {
            if(m_pLatencyToleranceState->m_ActiveContextsCount && (!IdleTime || IdleTime < MEMORYREFRESH_IDLE_TIME))
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITORON_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITORON_INITIAL;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITOROFF_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITOROFF_INITIAL;
                    bFail = true;
                }
            }
            else if(m_pLatencyToleranceState->m_ActiveContextsCount && (IdleTime > MEMORYREFRESH_IDLE_TIME))
            {
                if((m_pLatencyToleranceState->m_LatencyToleranceTimeStamp - m_pLatencyToleranceState->m_IdleTimeStamp) > MEMORYREFRESH_IDLE_TIME)
                {
                    bFail = true;
                }
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITORON_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITORON_IDLE;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITOROFF_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITOROFF_IDLE;
                    bFail = true;
                }
            }
            else if(!m_pLatencyToleranceState->m_ActiveContextsCount)
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITORON_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITORON_NOCONTEXT;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT;
                    bFail = true;
                }
            }
            break;
        }
    case DXGK_POWER_COMPONENT_D3_TRANSITION:
        {
            if(m_pLatencyToleranceState->m_ActiveContextsCount && (!IdleTime || IdleTime < ALL_ENGINES_IDLE_TIME_1))
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITORON_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITORON_INITIAL;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITOROFF_INITIAL != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITOROFF_INITIAL;
                    bFail = true;
                }
            }
            else if(m_pLatencyToleranceState->m_ActiveContextsCount && (IdleTime > ALL_ENGINES_IDLE_TIME_1))
            {
                if((m_pLatencyToleranceState->m_LatencyToleranceTimeStamp - m_pLatencyToleranceState->m_IdleTimeStamp) > ALL_ENGINES_IDLE_TIME_1)
                {
                    bFail = true;
                }
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITORON_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITORON_IDLE;
                    bFail = true;
                }
            }
            else if(m_pLatencyToleranceState->m_ActiveContextsCount && (IdleTime > ALL_ENGINES_IDLE_TIME_2))
            {
                if((m_pLatencyToleranceState->m_LatencyToleranceTimeStamp - m_pLatencyToleranceState->m_IdleTimeStamp) > ALL_ENGINES_IDLE_TIME_2)
                {
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITOROFF_IDLE != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITOROFF_IDLE;
                    bFail = true;
                }
            }
            else if(!m_pLatencyToleranceState->m_ActiveContextsCount)
            {
                if(m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITORON_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITORON_NOCONTEXT;
                    bFail = true;
                }
                else if(!m_pLatencyToleranceState->m_bMonitorOn && (LT_D3TRANSITION_MONITOROFF_NOCONTEXT != m_pLatencyToleranceState->m_LatencyTolerance))
                {
                    m_pLatencyToleranceState->m_ExpectedLatencyTolerance = LT_D3TRANSITION_MONITOROFF_NOCONTEXT;
                    bFail = true;
                }
            }
            break;
        }
    }

    if(bFail)
    {
        CLatencyToleranceState* pNewLTState = new CLatencyToleranceState(0);

        if(!pNewLTState)
            throw std::exception("ValidateLatencyTolerance - failed to allocate memory");

        int i = 0;
        if(i)
        {
            pETWMaster->PrintAdapters();
        }

        pNewLTState->m_ActiveTimeStamp = m_pLatencyToleranceState->m_ActiveTimeStamp;
        pNewLTState->m_IdleTimeStamp = m_pLatencyToleranceState->m_IdleTimeStamp;
        pNewLTState->m_bMonitorOn = m_pLatencyToleranceState->m_bMonitorOn;
        pNewLTState->m_ActiveContextsCount = m_pLatencyToleranceState->m_ActiveContextsCount;
        pNewLTState->m_LatencyTolerance = m_pLatencyToleranceState->m_LatencyTolerance;

        m_LatencyToleranceViolationList.push_back(m_pLatencyToleranceState);

        m_pLatencyToleranceState = pNewLTState;
    }
}


/////////////////////////////////////////////////////////////////////////////
//	ETW Master
/////////////////////////////////////////////////////////////////////////////
CHybridETWMaster::CHybridETWMaster()
{
    m_bListening = false;
    m_hListeningThread = NULL;
    m_bConsumersAdded = false;

    m_pDxgKrnlListener = new CDxgKrnlHybridConsumer(this);
    m_pTestListener = new CTestHybridConsumer(this);
    m_pTestProvider = new CHybridETWProvider();
    m_hWaitForEvents = CreateEventW(NULL, FALSE, FALSE, HYBRID_PRESENT_WAIT_FOR_EVENTS);
    m_PresentModel = D3DKMT_PM_UNINITIALIZED;
    m_bProcessPowerEvents = false;
    m_InterruptStartTS = 0;
    m_InterruptTimeTotal = 0;
    m_DPCStartTS = 0;
    m_DPCTimeTotal = 0;

    if(!m_pDxgKrnlListener || !m_pTestListener || !m_pTestProvider || !m_hWaitForEvents)
    {
        throw std::exception("Out of Memory");
    }

    m_pTestProvider->WaitForEnable(30);
}

CHybridETWMaster::~CHybridETWMaster()
{
    Cleanup();

    if(m_pDxgKrnlListener)
    {
        delete m_pDxgKrnlListener;
        m_pDxgKrnlListener = NULL;
    }

    if(m_pTestListener)
    {
        delete m_pTestListener;
        m_pTestListener = NULL;
    }

    if(m_pTestProvider)
    {
        delete m_pTestProvider;
        m_pTestProvider = NULL;
    }

    if(m_hWaitForEvents)
    {
        CloseHandle(m_hWaitForEvents);
        m_hWaitForEvents = NULL;
    }
}


void CHybridETWMaster::Cleanup()
{
    if(!m_Adapters.empty())
    {
        for(AdapterList::iterator it = m_Adapters.begin();
            it != m_Adapters.end();
            it++)
        {
            CAdapter* pAdapter = *it;
            delete pAdapter;
            pAdapter = NULL;
        }

        m_Adapters.clear();
    }

    if(!m_TrackedProcessList.empty())
    {
        m_TrackedProcessList.clear();
    }
}

void CHybridETWMaster::CleanupAllocations()
{
    AdapterList::iterator itAdapter = m_Adapters.begin();

    for(itAdapter; itAdapter != m_Adapters.end(); itAdapter++)
    {
        CAdapter* pAdapter = *itAdapter;

        for(CrossAdapterAllocationList::iterator it = pAdapter->m_CrossAdapterAllocationList.begin();
            it != pAdapter->m_CrossAdapterAllocationList.end();
            it++)
        {
            CCrossAdapterAllocation* pAllocation = *it;

            if(pAllocation->m_bDelete)
            {
                pAdapter->m_CrossAdapterAllocationList.erase(it);
                delete pAllocation;
                break;
            }
        }
    }
}

HANDLE* CHybridETWMaster::GetProcessIDByName(CHAR* szName)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;

    HANDLE* phProcess = new HANDLE[g_ProcessArraySize]; // array of 2. We'll always have 2 csrss.exe processes running (session0, session1)

    if(!phProcess)
        throw std::exception("out of memory");
    
    ZeroMemory(phProcess, sizeof(HANDLE) * g_ProcessArraySize);

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
        throw std::exception("Failed to create a snapshot of all processes");

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );
   
    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
        throw std::exception("Failed to get first process information");

    // Now walk the snapshot of processes, looking for the HCK client process
    int index = 0;
    do
    {
        if (strcmp(pe32.szExeFile, szName) == 0)
        {   
            if(index >= g_ProcessArraySize)
            {
                throw std::exception("too many processes enumerated");
                continue;
            }

            phProcess[index] = (HANDLE)(ULONGLONG)pe32.th32ProcessID;
            index++;
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );

  return phProcess;
}

WCHAR* CHybridETWMaster::GetProcessNameByHandle(HANDLE hProcess)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    WCHAR* strExeName = NULL;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
        throw std::exception("Failed to create a snapshot of all processes");

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );
   
    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
        throw std::exception("Failed to get first process information");

    // Now walk the snapshot of processes, looking for the HCK client process
    int index = 0;
    do
    {
        if ((HANDLE)(ULONGLONG)pe32.th32ProcessID == hProcess)
        {   
            strExeName = new WCHAR[MAX_PATH];

            if(!strExeName)
                throw std::exception("Failed to allocate memory");

            wsprintfW(strExeName, L"%S", pe32.szExeFile);

            break;
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );

  return strExeName;
}


void CHybridETWMaster::StartListening()
{
    if(m_bListening || m_hListeningThread)
        return;

    // We want to track Test process as well as DWM.exe
    DBGPrint(L"Process ID for current process is %p", (HANDLE)(ULONGLONG)GetCurrentProcessId());
    TrackProcess((HANDLE)(ULONGLONG)GetCurrentProcessId());

    HANDLE* phProcess = GetProcessIDByName("dwm.exe");
    DBGPrint(L"Found Proccess ID for DWM.EXE; PID = %p", phProcess[0]);
    TrackProcess(phProcess[0]);
    delete[] phProcess;

    phProcess = GetProcessIDByName("csrss.exe");
    DBGPrint(L"Found Proccess ID for CSRSS.EXE; PID = %p", phProcess[0]);
    DBGPrint(L"Found Proccess ID for CSRSS.EXE; PID = %p", phProcess[1]);
    TrackProcess(phProcess[0]);
    TrackProcess(phProcess[1]);
    delete[] phProcess;


    ETWListeningThreadProcArgs Args = {0};
    Args.pMaster = this;
    Args.pDxgKrnlListener = m_pDxgKrnlListener;
    Args.pTestListener = m_pTestListener;

    m_hListeningThread = CreateThread(0, 0, &CHybridETWMaster::EtwListeningThreadProc, &Args, 0, 0);

    if(m_hListeningThread)
    {
        while(!m_bListening)
        {
            Sleep(1000);
        }
    }

    if(!m_hListeningThread || !m_bListening)
    {
        throw std::exception("Failed to start listening for ETW events");
    }

    m_bListening = true;
}

void CHybridETWMaster::StopListening()
{
    if(!m_bListening)
    {
        return;
    }

    m_bListening = false;

    WaitForSingleObject(m_hListeningThread, INFINITE);

    m_hListeningThread = NULL;
}

HRESULT CHybridETWMaster::TrackProcess(HANDLE hProcess)
{
    if(!hProcess)
        throw std::exception("Invalid parameter");

    CSLocker Lock(m_CritSection);
    m_TrackedProcessList.push_front(hProcess);

    return S_OK;
}

bool CHybridETWMaster::IsTrackedProcess(HANDLE hProcess)
{
    HandleList::iterator it = m_TrackedProcessList.begin();

    CSLocker Lock(m_CritSection);

    for(it; it != m_TrackedProcessList.end(); it++)
    {
        if((HANDLE)(*it) == hProcess)
            return true;
    }

    return false;
}

bool CHybridETWMaster::IsTrackedDevice(HANDLE hDevice)
{
    // when share'd allocation's owner goes away, these will always have NULL devices
    if(!hDevice)
        return true;

    CSLocker Lock(m_CritSection);
    AdapterList::iterator it = m_Adapters.begin();

    for(it; it != m_Adapters.end(); it++)
    {
        CAdapter* pAdapter = *it;

        CDevice* pDevice = pAdapter->FindTrackedDevice(hDevice);

        if(pDevice)
            return true;
    }

    return false;
}


bool CHybridETWMaster::IsTrackedContext(HANDLE hContext)
{
    CSLocker Lock(m_CritSection);
    AdapterList::iterator it = m_Adapters.begin();

    for(it; it != m_Adapters.end(); it++)
    {
        CAdapter* pAdapter = *it;

        for(DeviceList::iterator itDevice = pAdapter->m_DeviceList.begin();
            itDevice != pAdapter->m_DeviceList.end();
            itDevice++)
        {
            CDevice* pDevice = *itDevice;

            if(hContext == pDevice->FindContext(hContext))
                return true;
        }
    }

    return false;
}

void CHybridETWMaster::LogTestETW(WCHAR* str)
{
    m_pTestProvider->WriteToETW(str);
}

bool CHybridETWMaster::PowerManagementValidate()
{
    // Send out an ETW event. Once it is received on listener thread (after all
    // previous events have drained) will process it and do some PM validations
    m_pTestProvider->WriteToETW(HYBRID_POWER_MANAGEMENT_VALIDATE);

    return WaitForEvents(HYBRID_PRESENT_WAIT_FOR_EVENTS_TIMEOUT);
}

bool CHybridETWMaster::WaitForEvents(UINT Timeout)
{
    // Sleep for 1 second. We do this in order to avoid a situation where
    // we receive test event prior to receiving kernel events. Since kernel
    // works asynchronously we might run into situation where frame is presented
    // after test had logged an event
    Sleep(1000);

    // Send out an ETW event. Once it is received on listener thread (after all
    // previous events have drained) listener thread will signal our event
    m_pTestProvider->WriteToETW(HYBRID_PRESENT_WAIT_FOR_EVENTS);

    if(WAIT_OBJECT_0 == WaitForSingleObject(m_hWaitForEvents, Timeout))
        return true;

    return false;
}

bool CHybridETWMaster::WaitForEvents()
{
    return WaitForEvents(HYBRID_PRESENT_WAIT_FOR_EVENTS_TIMEOUT);
}

void CHybridETWMaster::ResetPresentCounters()
{

    CSLocker Lock(m_CritSection);
    m_CrossAdapterPresentsCount = 0;
    m_RegularPresentsCount = 0;
}

CAdapter* CHybridETWMaster::GetDGpu()
{
    AdapterList::iterator it = m_Adapters.begin();

    for(it; it != m_Adapters.end(); it++)
    {
        CAdapter* pAdapter = *it;

        if(pAdapter->m_bDiscrete)
            return pAdapter;
    }

    return NULL;
}

DWORD CHybridETWMaster::EtwListeningThreadProc(void* context)
{
    ETWListeningThreadProcArgs* pArgs = (ETWListeningThreadProcArgs*)context;
    CHybridETWMaster* pMaster = pArgs->pMaster;
    CDxgKrnlHybridConsumer* pDxgKrnlListener = pArgs->pDxgKrnlListener;
    CTestHybridConsumer* pTestListener = pArgs->pTestListener;

    if(!pMaster->m_bConsumersAdded)
    {
        pMaster->m_etwController.AddConsumer(pDxgKrnlListener);
        pMaster->m_etwController.AddConsumer(pTestListener);
        pMaster->m_bConsumersAdded = true;
    }

    bool bConsuming = pMaster->m_etwController.StartProcessing();

    UINT cTries = 0;

    if( bConsuming )
    {
        while(!pMaster->m_bListening)
        {
            //
            //  At the end of the Rundown we'll get ReportSegment events. See if we already
            // have this information, this will indicate that rundown is complete
            //
            pMaster->WaitForEvents();

            if(!pMaster->m_Adapters.empty())
            {
                AdapterList::iterator it = pMaster->m_Adapters.begin();

                for(it; it != pMaster->m_Adapters.end(); it++)
                {
                    CAdapter* pAdapter = *it;

                    if(!pAdapter->m_MemorySegmentList.empty())
                    {
                        pMaster->m_bListening = true;
                        break;
                    }
                }                
            }

            cTries++;

            if(cTries >= 3)
            {
                // We don't want to spin here forever
                break;
            }
        }

        if(!pMaster->m_bListening)
        {
            pMaster->m_etwController.StopProcessing();
            throw std::exception("Failed to receive expected ETW events");
        }

        while(pMaster->m_bListening)
        {
            Sleep(1000);
        }

        pMaster->m_etwController.StopProcessing();
    }
    else
    {
        DBGPrint(L"Failed to start ETW listeners. Make sure process is running with elevated privileges");
        throw std::exception("Failed to start ETW listeners. Make sure process is running with elevated privileges");
    }

    return 0;
}

CAdapter* CHybridETWMaster::GetAdapter(HANDLE hAdapter)
{
    CAdapter* pAdapter = NULL;

    for(AdapterList::iterator it = m_Adapters.begin();
        it != m_Adapters.end();
        it++)
    {
        pAdapter = *it;

        if(pAdapter->m_hAdapter == hAdapter)
        {
            return pAdapter;
        }
    }

    return NULL;
}

void CHybridETWMaster::HandleAdapter(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);
    CAdapter* pAdapter = NULL;
    bool bDiscrete = false;
    bool bBDD = false;

    CSLocker Lock(m_CritSection);
    if(EH.IsStart() || EH.IsDCStart())
    {
        pAdapter = new CAdapter(EH.GetProperty<HANDLE>(L"pDxgAdapter"), false);

        if(!pAdapter)
            throw std::exception("Out of memory");

        pAdapter->m_hMiniportContext = EH.GetProperty<HANDLE>(L"pMiniportContext");

        // Get AdapterType
        D3DKMT_ADAPTERTYPE Flags;
        Flags.Value = EH.GetProperty<UINT>(L"AdapterType");

        if(Flags.HybridDiscrete)
        {
            bDiscrete = true;
            pAdapter->m_bDiscrete = true;
        }

        if(!Flags.HybridDiscrete && !Flags.HybridIntegrated && Flags.SoftwareDevice)
        {
            bBDD = true;
            pAdapter->m_bBDD = true;
        }

        pAdapter->m_PagingNodeOrdinal = EH.GetProperty<UINT>(L"PagingNode");

        m_Adapters.push_back(pAdapter);
    }
    else if(EH.IsStop())
    {
        for(AdapterList::iterator it = m_Adapters.begin();
            it != m_Adapters.end();
            it++)
        {
            pAdapter = *it;

            if(pAdapter->m_hAdapter == EH.GetProperty<HANDLE>(L"pDxgAdapter"))
            {
                if(pAdapter->m_bDiscrete)
                    bDiscrete = true;

                if(pAdapter->m_bBDD)
                    bBDD = true;

                m_Adapters.erase(it);
                delete pAdapter;
                break;
            }
        }
    }

    DBGPrint(L"%sAdapter:hDxgAdapter = 0x%p, bDiscrete = %s, bBDD = %s", 
            (EH.IsStop()) ? L"Destroy" : L"Create", 
            EH.GetProperty<HANDLE>(L"pDxgAdapter"),
            bDiscrete ? L"TRUE" : L"FALSE",
            bBDD ? L"TRUE" : L"FALSE");
}

void CHybridETWMaster::HandleNodeMetadata(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);
    HANDLE hAdapter = EH.GetProperty<HANDLE>(L"pDxgAdapter");
    UINT NodeOrdinal = EH.GetProperty<UINT>(L"NodeOrdinal");

    CAdapter* pAdapter = GetAdapter(hAdapter);

    if(!pAdapter)
        throw std::exception("Couldn't find adapter");

    CNode* pNode = pAdapter->GetNode(NodeOrdinal);

    if(!pNode)
    {
        pNode = new CNode(NodeOrdinal);

        if(!pNode)
            throw std::exception("Out of memory");

        pAdapter->m_NodeList.push_back(pNode);
    }

    if(pAdapter->m_bDiscrete)
    {
        // Find corresponding power component
        PowerComponentList::iterator it;

        for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;	

            if((pPC->m_Type == DXGK_POWER_COMPONENT_ENGINE) && (pPC->m_ComponentTypeId == NodeOrdinal))
            {
                pPC->m_pComponent = (void*)pNode;
                pNode->m_pPC = pPC;
            }
        }
    }

    DBGPrint(L"NodeMetadata: pDxgAdapter = 0x%p, NodeOrdinal = %d", hAdapter, NodeOrdinal);
}

void CHybridETWMaster::HandleDevice(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);

    HANDLE hProcess = EH.GetProperty<HANDLE>(L"hProcessId");

        
    CSLocker Lock(m_CritSection);
    HANDLE hDevice = EH.GetProperty<HANDLE>(L"hDevice");
    CAdapter* pAdapter = GetAdapter(EH.GetProperty<HANDLE>(L"pDxgAdapter"));

    if(!pAdapter)
        throw std::exception("Couldn't find adapter");
        
    if(EH.IsStart() || EH.IsDCStart())
    {
        if(IsTrackedProcess(hProcess))
        {   
            CDevice* pDevice = new CDevice(hDevice, hProcess);
            pAdapter->m_DeviceList.push_back(pDevice);
        }

        pAdapter->m_AllDevices.push_back(hDevice);

        // Get process name and save it
        WCHAR* strName = GetProcessNameByHandle(hProcess);

        if(!strName)
            throw std::exception("Can't find process");

        if(!pAdapter->AddProcessName(strName))
            delete[] strName;
    }
    else if(EH.IsStop())
    {
        if(IsTrackedProcess(hProcess))
        {   
            for(DeviceList::iterator it = pAdapter->m_DeviceList.begin();
                it != pAdapter->m_DeviceList.end();
                it++)
            {
                CDevice* pDevice = *it;
                if(pDevice->m_hDeviceHandle == hDevice)
                {
                    pAdapter->m_DeviceList.erase(it);
                    delete pDevice;
                    break;
                }
            }
        }

        for(HandleList::iterator it = pAdapter->m_AllDevices.begin();
            it != pAdapter->m_AllDevices.end();
            it++)
        {
            if(*it == hDevice)
            {
                pAdapter->m_AllDevices.erase(it);
                break;
            }
        };
    }

    if(IsTrackedProcess(hProcess))
    {
        DBGPrint(L"%sDevice:hProcess = 0x%p, hDxgAdapter = 0x%p, hDevice = 0x%p", 
                (EH.IsStop()) ? L"Destroy" : L"Create", 
                hProcess, 
                EH.GetProperty<HANDLE>(L"pDxgAdapter"), 
                hDevice);
    }
}


void CHybridETWMaster::HandleContext(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);

    HANDLE hDevice = EH.GetProperty<HANDLE>(L"hDevice");
    UINT NodeOrdinal = EH.GetProperty<UINT>(L"NodeOrdinal");
    HANDLE hContextHandle = EH.GetProperty<HANDLE>(L"ContextHandle");
    UINT Flags = EH.GetProperty<UINT>(L"Flags");
    CAdapter* pAdapter = NULL;
    CDevice* pDevice = NULL;        

    CSLocker Lock(m_CritSection);

    if(IsTrackedDevice(hDevice))
    {
        for(AdapterList::iterator it = m_Adapters.begin();
            it != m_Adapters.end();
            it++)
        {
            pAdapter = *it;
            pDevice = pAdapter->FindTrackedDevice(hDevice);

            if(pDevice)
                break;
        }

        if(!pDevice)
            throw std::exception("Didn't find device on any adapter");
    }
    else
    {
        for(AdapterList::iterator it = m_Adapters.begin();
            it != m_Adapters.end();
            it++)
        {
            pAdapter = *it;
            if(pAdapter->FindDevice(hDevice) == hDevice)
                break;
            pAdapter = NULL;
        }

        if(!pAdapter)
            throw std::exception("Didn't find device on any adapter");
    }

    CNode* pNode = pAdapter->GetNode(NodeOrdinal);

    if(!pNode)
    {
        pNode = new CNode(NodeOrdinal);

        if(!pNode)
            throw std::exception("Out of memory");

        pAdapter->m_NodeList.push_back(pNode);
    }
 
    if(!hContextHandle /* && Flags*/)
    {
        DBGPrint(L"!!!!!!!!!!!! Skipping %sContext:hDevice = 0x%p, ContextHandle = 0x%p, dGPU = %s, NodeOrdinal = %d, PowerComponentIndex = %d, hContext = 0x%p", 
                (EH.IsStop()) ? L"Destroy" : L"Create", 
                EH.GetProperty<HANDLE>(L"hDevice"), 
                hContextHandle,
                pAdapter->IsDiscrete() ? L"TRUE" : L"FALSE",
                NodeOrdinal,
                pNode->m_pPC ? pNode->m_pPC->m_Index : 0xFFFFFFFF,
                EH.GetProperty<HANDLE>(L"hContext"));
        return;
    }

    if(EH.IsStart() || EH.IsDCStart())
    {
        if(IsTrackedDevice(hDevice))
        { 
            pDevice->m_hContextList.push_back(hContextHandle);
        }

        pNode->m_hContextList.push_back(hContextHandle);

        pAdapter->m_numActiveContexts++;

        PowerComponentList::iterator it = pAdapter->m_PowerComponentList.begin();

        for(it; it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;

            // When there is a context on any engine, kernel will treat PagingNode as having a context as well
            // even though there won't be an API context. This is done by kernel as an optimization so that
            // paging node doesn't go into an F-State when it might be needed immediately by another engine
            // Because of this, we will make sure m_ActiveCountexsCount is 1 on paging node when there is a
            // context anywhere on other engines
            if(pPC->m_Type == DXGK_POWER_COMPONENT_ENGINE)
            {
                if(pPC->m_ComponentTypeId == NodeOrdinal)
                    pPC->m_pLatencyToleranceState->m_ActiveContextsCount++;
                else if(pPC->m_ComponentTypeId == pAdapter->m_PagingNodeOrdinal)
                {
                    if(!pPC->m_pLatencyToleranceState->m_ActiveContextsCount)
                        pPC->m_pLatencyToleranceState->m_ActiveContextsCount++;
                }
            }
            else
                pPC->m_pLatencyToleranceState->m_ActiveContextsCount++;
        }
    }
    else if(EH.IsStop())
    {
        if(IsTrackedDevice(hDevice))
        {
            for(HandleList::iterator it = pDevice->m_hContextList.begin();
                it != pDevice->m_hContextList.end();
                it++)
            {
                if(*it == hContextHandle)
                {
                    pDevice->m_hContextList.erase(it);
                    break;
                }
            };
        }

        for(HandleList::iterator it = pNode->m_hContextList.begin();
            it != pNode->m_hContextList.end();
            it++)
        {
            if(*it == hContextHandle)
            {
                pNode->m_hContextList.erase(it);
                break;
            }
        };

        if(!pAdapter->m_numActiveContexts)
            throw std::exception("No active contexts tracked");

        pAdapter->m_numActiveContexts--;

        PowerComponentList::iterator it = pAdapter->m_PowerComponentList.begin();

        for(it; it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;

            // When there is a context on any engine, kernel will treat PagingNode as having a context as well
            // even though there won't be an API context. This is done by kernel as an optimization so that
            // paging node doesn't go into an F-State when it might be needed immediately by another engine
            // Because of this, we will make sure m_ActiveCountexsCount is 1 on paging node when there is a
            // context anywhere on other engines
            if(pPC->m_Type == DXGK_POWER_COMPONENT_ENGINE)
            {
                if(pPC->m_ComponentTypeId == NodeOrdinal)
                {
                    if(!pPC->m_pLatencyToleranceState->m_ActiveContextsCount)
                        throw std::exception("ActiveContxtsCount is already 0");

                    pPC->m_pLatencyToleranceState->m_ActiveContextsCount--;
                }
                else if(pPC->m_ComponentTypeId == pAdapter->m_PagingNodeOrdinal)
                {
                    if(!pAdapter->m_numActiveContexts)
                        pPC->m_pLatencyToleranceState->m_ActiveContextsCount--;
                }
            }
            else
            {
                if(!pPC->m_pLatencyToleranceState->m_ActiveContextsCount)
                    throw std::exception("ActiveContxtsCount is already 0");

                pPC->m_pLatencyToleranceState->m_ActiveContextsCount--;
            }
        }
    }

    if(IsTrackedDevice(hDevice))
    {
        DBGPrint(L"%sContext:hDevice = 0x%p, hContext = 0x%p", (EH.IsStop()) ? L"Destroy" : L"Create", EH.GetProperty<HANDLE>(L"hDevice"), hContextHandle);
    }
}


void CHybridETWMaster::HandleReportSegment(PEVENT_RECORD pEvent)
{
    CSLocker Lock(m_CritSection);
    CDxgKernelEventHelper EH(pEvent);
    CAdapter* pAdapter = GetAdapter(EH.GetProperty<HANDLE>(L"pDxgAdapter"));

    if(!pAdapter)
        throw std::exception("Couldn't find adapter");

    CMemorySegment* pSegment = new CMemorySegment(EH.GetProperty<UINT>(L"ulSegmentId"), 
                                                  EH.GetProperty<SIZE_T>(L"Size"), 
                                                  EH.GetProperty<UINT>(L"Flags"));

    if(!pSegment)
        throw std::exception("Out of memory");

    pAdapter->m_MemorySegmentList.push_back(pSegment);

    if(pAdapter->m_bDiscrete)
    {
        // Find corresponding power component
        PowerComponentList::iterator it;

        for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;	

            if((pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY) && (pPC->m_ComponentTypeId == (EH.GetProperty<UINT>(L"ulSegmentId")-1)))
            {
                pPC->m_pComponent = (void*)pSegment;
                pSegment->m_pPC = pPC;
            }
        }
    }

    DBGPrint(L"ReportSegment:hDxgAdapter = 0x%p, ID = %d, Size = 0x%p, Flags = %d", EH.GetProperty<HANDLE>(L"pDxgAdapter"), 
             EH.GetProperty<UINT>(L"ulSegmentId"),
             EH.GetProperty<SIZE_T>(L"Size"),
             EH.GetProperty<UINT>(L"Flags"));
}

void CHybridETWMaster::HandleAdapterAllocation(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);

    UINT Flags = EH.GetProperty<UINT>(L"Flags");

    CSLocker Lock(m_CritSection);

    HANDLE hAdapter = EH.GetProperty<HANDLE>(L"pDxgAdapter");
    CAdapter* pAdapter = NULL;

    if(!hAdapter && EH.IsStop())
        pAdapter = NULL;
    else
    {
        pAdapter = GetAdapter(hAdapter);

        if(!pAdapter)
            throw std::exception("Couldn't find adapter");
    }

    if(EH.IsStart() || EH.IsDCStart())
    {
        if(IsTrackedDevice(EH.GetProperty<HANDLE>(L"hDevice")) && Flags & DXGK_ALLOCATIONINFOFLAGS15)
        {
            CCrossAdapterAllocation* pAllocation = new CCrossAdapterAllocation(EH.GetProperty<HANDLE>(L"hDevice"),
                                                                                Flags,
                                                                                (ULONGLONG)EH.GetProperty<ULONGLONG>(L"allocSize"),
                                                                                EH.GetProperty<ULONG>(L"ulAlignment"),
                                                                                EH.GetProperty<UINT>(L"dwWriteSegment") | EH.GetProperty<UINT>(L"dwReadSegment"),
                                                                                EH.GetProperty<UINT>(L"PreferredSegment"),
                                                                                EH.GetProperty<UINT>(L"Priority"),
                                                                                EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"),
                                                                                EH.GetProperty<HANDLE>(L"hDxgGlobalAlloc"));

            if(!pAllocation)
                throw std::exception("Out of memory");

            pAdapter->m_CrossAdapterAllocationList.push_back(pAllocation);
        }

        CAllocation* pAllocation = new CAllocation(EH.GetProperty<HANDLE>(L"hDevice"),
                                                    Flags,
                                                    (ULONGLONG)EH.GetProperty<ULONGLONG>(L"allocSize"),
                                                    EH.GetProperty<ULONG>(L"ulAlignment"),
                                                    EH.GetProperty<UINT>(L"dwWriteSegment") | EH.GetProperty<UINT>(L"dwReadSegment"),
                                                    EH.GetProperty<UINT>(L"PreferredSegment"),
                                                    EH.GetProperty<UINT>(L"Priority"),
                                                    EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"),
                                                    EH.GetProperty<HANDLE>(L"hDxgGlobalAlloc"));

        if(!pAllocation)
            throw std::exception("Out of memory");

        pAdapter->m_AllocationList.push_back(pAllocation);
    }
    else if(EH.IsStop())
    {
        if(pAdapter)
        {
            if(IsTrackedDevice(EH.GetProperty<HANDLE>(L"hDevice")) && Flags & DXGK_ALLOCATIONINFOFLAGS15)
            {
                for(CrossAdapterAllocationList::iterator it = pAdapter->m_CrossAdapterAllocationList.begin();
                    it != pAdapter->m_CrossAdapterAllocationList.end();
                    it++)
                {
                    CCrossAdapterAllocation* pAllocation = *it;

                    if(pAllocation->m_hVidMMGlobalHandle == EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"))
                    {
                        pAllocation->m_bDelete = true;
                    }
                }
            }

            for(AllocationList::iterator it = pAdapter->m_AllocationList.begin();
                it != pAdapter->m_AllocationList.end();
                it++)
            {
                CAllocation* pAllocation = *it;

                if(pAllocation->m_hVidMMGlobalHandle == EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"))
                {
                    pAdapter->m_AllocationList.erase(it);
                    delete pAllocation;
                    break;
                }
            }
        }
        else
        {
            AdapterList::iterator itAdapter = m_Adapters.begin();

            for(itAdapter; itAdapter != m_Adapters.end(); itAdapter++)
            {
                pAdapter = *itAdapter;

                if(IsTrackedDevice(EH.GetProperty<HANDLE>(L"hDevice")) && Flags & DXGK_ALLOCATIONINFOFLAGS15)
                {
                    for(CrossAdapterAllocationList::iterator it = pAdapter->m_CrossAdapterAllocationList.begin();
                        it != pAdapter->m_CrossAdapterAllocationList.end();
                        it++)
                    {
                        CCrossAdapterAllocation* pAllocation = *it;

                        if(pAllocation->m_hVidMMGlobalHandle == EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"))
                        {
                            pAllocation->m_bDelete = true;
                        }
                    }
                }

                for(AllocationList::iterator it = pAdapter->m_AllocationList.begin();
                    it != pAdapter->m_AllocationList.end();
                    it++)
                {
                    CAllocation* pAllocation = *it;

                    if(pAllocation->m_hVidMMGlobalHandle == EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"))
                    {
                        pAdapter->m_AllocationList.erase(it);
                        delete pAllocation;
                        break;
                    }
                }
            }
        }

        if(IsTrackedDevice(EH.GetProperty<HANDLE>(L"hDevice")) && Flags & DXGK_ALLOCATIONINFOFLAGS15)
        {
            DBGPrint(L"%sAllocation:hDxgAdapter=0x%p, hDevice=0x%p, Flags=0x%X, Size=%I64u, Alignment=%d, \
                        SupportedSegmentSet=0x%X, PreferredSegments=%d, Priority=0x%X, hGlobalAlloc=0x%p, hDxgAllocation=0x%p",
                        (EH.IsStop()) ? L"Destroy" : L"Create",
                        EH.GetProperty<HANDLE>(L"pDxgAdapter"),
                        EH.GetProperty<HANDLE>(L"hDevice"),
                        EH.GetProperty<UINT>(L"Flags"),
                        (ULONGLONG)EH.GetProperty<ULONGLONG>(L"allocSize"),
                        EH.GetProperty<ULONG>(L"ulAlignment"),
                        EH.GetProperty<UINT>(L"dwWriteSegment") | EH.GetProperty<UINT>(L"dwReadSegment"),
                        EH.GetProperty<UINT>(L"PreferredSegment"),
                        EH.GetProperty<UINT>(L"Priority"),
                        EH.GetProperty<HANDLE>(L"hVidMmGlobalAlloc"),
                        EH.GetProperty<HANDLE>(L"hDxgGlobalAlloc"));
        }
    }
}


void CHybridETWMaster::HandleTotalBytesResidentInSegment(PEVENT_RECORD pEvent)
{
    if(!m_bProcessPowerEvents)
        return;

    CDxgKernelEventHelper EH(pEvent);

    CSLocker Lock(m_CritSection);

    UINT SegmentId = EH.GetProperty<UINT>(L"SegmentId");
    HANDLE hAdapter = EH.GetProperty<HANDLE>(L"DxgAdapter");
    ULONGLONG BytesAllocated = EH.GetProperty<ULONGLONG>(L"TotalBytesResident");
    CAdapter* pAdapter = GetAdapter(hAdapter);

    if(!pAdapter)
        throw std::exception("Couldn't find adapter");

    if(pAdapter)
    {
        CMemorySegment* pSegment = pAdapter->GetMemorySegment(SegmentId);

        if(!pSegment)
            throw std::exception("Coudn't find segment");
        else
        {
            ULONGLONG PreviousBytes = pSegment->m_BytesAllocated;
            pSegment->m_BytesAllocated = BytesAllocated;

            DBGPrint(L"HandleTotalBytesResidentInSegment - Adapter = 0x%p (%s), SegmentId = %d, previous bytes allocated = %I64u, new bytes allocated = %I64u",
                       pAdapter, 
                       (pAdapter->IsDiscrete()) ? L"Discrete" : L"Integrated",
                       SegmentId,
                       PreviousBytes,
                       BytesAllocated);
        }
    }
}


void CHybridETWMaster::HandlePresentHistoryDetailed(PEVENT_RECORD pEvent)
{
    // Check for VistaBLT Present model from our process.
    if(pEvent->EventHeader.ProcessId == (ULONG)(ULONGLONG)GetCurrentProcessId())
    {
        CDxgKernelEventHelper EH(pEvent);
        D3DKMT_PRESENT_MODEL PresentModel = (D3DKMT_PRESENT_MODEL)EH.GetProperty<UINT>(L"Model");

        if(PresentModel == D3DKMT_PM_REDIRECTED_VISTABLT)
        {
            m_PresentModel = D3DKMT_PM_REDIRECTED_VISTABLT;
            DBGPrint(L"PresentHistoryDetailed: Detected VistaBLT Present Model");
        }

        DBGPrint(L"PresentHistoryDetailed: Process ID = %d, event id = %d", pEvent->EventHeader.ProcessId, pEvent->EventHeader.EventDescriptor.Id);
    }
}

void CHybridETWMaster::HandlePresentHistory(PEVENT_RECORD pEvent)
{
    // Check for VistaBLT Present model from our process.
    if(pEvent->EventHeader.ProcessId == (ULONG)(ULONGLONG)GetCurrentProcessId())
    {
        CDxgKernelEventHelper EH(pEvent);
        D3DKMT_PRESENT_MODEL PresentModel = (D3DKMT_PRESENT_MODEL)EH.GetProperty<UINT>(L"Model");

        if(PresentModel == D3DKMT_PM_REDIRECTED_VISTABLT)
        {
            m_PresentModel = D3DKMT_PM_REDIRECTED_VISTABLT;
            DBGPrint(L"PresentHistory: Detected VistaBLT Present Model");
        }

        DBGPrint(L"PresentHistory: Process ID = %d, event id = %d", pEvent->EventHeader.ProcessId, pEvent->EventHeader.EventDescriptor.Id);
    }
}


void CHybridETWMaster::HandlePresent(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);
    HANDLE hContext = EH.GetProperty<HANDLE>(L"hContext");

    if(IsTrackedContext(hContext))
    {
        ULONG CurrentProcessId = (ULONG)(ULONGLONG)GetCurrentProcessId();
        D3DKMT_PRESENTFLAGS Flags;
        
        Flags.Value = EH.GetProperty<UINT>(L"Flags");

        CSLocker Lock(m_CritSection);
        // First, find device that this context belongs to
        AdapterList::iterator it = m_Adapters.begin();
        CAdapter* pAdapter = NULL;
        CDevice* pDevice = NULL;
        bool bFoundContext = false;

        for(it; it != m_Adapters.end(); it++)
        {
            pAdapter = *it;

            for(DeviceList::iterator itDevice = pAdapter->m_DeviceList.begin();
                itDevice != pAdapter->m_DeviceList.end();
                itDevice++)
            {
                pDevice = *itDevice;

                if(hContext == pDevice->FindContext(hContext))
                {
                    bFoundContext = true;
                    break;
                }
            }

            if(bFoundContext)
                break;
        }

        if(!bFoundContext)
            throw std::exception("HandlePresent - can't find handled context");

        int i = 0;
        if(i)
            PrintAdapters();

        WCHAR* pProcessName = L"";

        if(pDevice->m_hProcess == GetProcessIDByName("csrss.exe")[0])
            pProcessName = L"CSRSS";
        else if(pDevice->m_hProcess == GetProcessIDByName("csrss.exe")[1])
            pProcessName = L"CSRSS";
        else if(pDevice->m_hProcess == GetProcessIDByName("dwm.exe")[0])
            pProcessName = L"DWM";
        else if(pDevice->m_hProcess == (HANDLE)(ULONGLONG)GetCurrentProcessId())
            pProcessName = L"TEST";


        // Increment counts only for TEST process and the TEST window
        if(pDevice->m_hProcess == (HANDLE)(ULONGLONG)GetCurrentProcessId())
        {
            HWND hTestWindow = (HWND)(EH.GetProperty<UINT*>(L"hWindow"));
            //if( hTestWindow && hTestWindow == *g_pTestWindow)
            {
                CSLocker Lock(m_CritSection);

                if(Flags.CrossAdapter)
                    m_CrossAdapterPresentsCount++;
                else
                    m_RegularPresentsCount++;

                DBGPrint(L"Present: got %s Present from %s process; HWND = 0x%p, hContext = 0x%p, CrossAdapter Presents = %d, Regular Presents = %d", 
                        Flags.CrossAdapter ? L"CrossAdapter" : L"Regular",					
                        pProcessName,
                        (HWND)(EH.GetProperty<UINT*>(L"hWindow")),
                        hContext,
                        m_CrossAdapterPresentsCount,
                        m_RegularPresentsCount);
            }
        }
    }
}

void CHybridETWMaster::HandleProfiler(PEVENT_RECORD pEvent)
{
    CDxgKernelEventHelper EH(pEvent);
    UINT Function = (UINT)EH.GetProperty<UINT>(L"Function");
    const UINT DpiFdoMessageInterrupt = 4014; // as defined in LDDMCore.man
    const UINT DpiDpcForIsr = 4013; // as defined in LDDMCore.man

    if(Function == DpiFdoMessageInterrupt)
    {
        if(EH.IsStart())
            m_InterruptStartTS = EH.ullTimeStamp();
        if(EH.IsStop())
            m_InterruptTimeTotal = (m_InterruptStartTS) ? (EH.ullTimeStamp() - m_InterruptStartTS) : 0;
    }
    else if(Function == DpiDpcForIsr)
    {
        if(EH.IsStart())
            m_DPCStartTS = EH.ullTimeStamp();
        if(EH.IsStop())
            m_DPCTimeTotal = (m_DPCStartTS) ? (EH.ullTimeStamp() - m_DPCStartTS) : 0;		
    }
}


void CHybridETWMaster::HandleRuntimePowerManagement(PEVENT_RECORD pEvent)
{
    if(!m_bProcessPowerEvents)
        return;

    CDxgKernelEventHelper EH(pEvent);

    if(EH.Id() != Dxgk_PowerComponent.Id &&
       EH.Id() != Dxgk_PowerComponentIdleState.Id &&
       EH.Id() != Dxgk_SetPowerComponentFState.Id &&
       EH.Id() != Dxgk_SetPowerComponentFStateEnd.Id &&
       EH.Id() != Dxgk_PowerRuntimeComponentActiveCallback.Id &&
       EH.Id() != Dxgk_SetPowerComponentLatencyCB.Id &&
       EH.Id() != Dxgk_PowerRuntimeDevicePowerRequiredCallback.Id &&
       EH.Id() != Dxgk_LatencyToleranceTimerEnd.Id &&
       EH.Id() != Dxgk_PrimaryMonitorPowerChange.Id)
       return;

    if(Dxgk_LatencyToleranceTimerEnd.Id == EH.Id())
    {
        CAdapter* pDGPUAdapter = GetDGpu();

        if(!pDGPUAdapter)
            throw std::exception("HandleRuntimePowerManagement - Failed to find dGPU adapter");

        // go through all components and validate latency tolerances
        PowerComponentList::iterator it = pDGPUAdapter->m_PowerComponentList.begin();

        for(it;it != pDGPUAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;

            pPC->ValidateLatencyTolerance(EH.ullTimeStamp(), this);
        }

        // Commenting this out as this lauses a lot of noise
        //DBGPrint(L"HandleRuntimePowerManagement - Dxgk_LatencyToleranceTimerEnd");
        return;
    }

    HANDLE hAdapter = EH.GetProperty<HANDLE>(L"AdapterHandle");
    CAdapter* pAdapter = GetAdapter(hAdapter);

    if(!pAdapter)
        return;

    if(!pAdapter->m_bDiscrete && Dxgk_PowerComponent.Id != EH.Id())
    {
        // we don't want to track events from iGPU as there shouldn't be any of the hybrid-related ones
        return;
    }

    if(Dxgk_PowerComponent.Id == EH.Id())
    {
        // Power component description
        UINT Type = EH.GetProperty<UINT>(L"ComponentType");
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");
        UINT TypeID = EH.GetProperty<UINT>(L"ComponentTypeId");
        UINT FStateCount = EH.GetProperty<UINT>(L"NumberOfIdleStates");
        UINT CurrentFState = EH.GetProperty<UINT>(L"CurrentFState");
        ULONGLONG CurrentLatencyTolerance = EH.GetProperty<ULONGLONG>(L"CurrentLatencyTolerance");
        
        CGPUPowerComponent* pPC = new CGPUPowerComponent(pAdapter, Type, Index, TypeID, FStateCount);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_PowerComponent - Failed to allocate CGPUPowerComponent");

        pPC->m_Flags.Value = EH.GetProperty<UINT>(L"Flags");
        pPC->m_LatencyTolerance = CurrentLatencyTolerance;
        pPC->m_CurrentFState = CurrentFState;
        pPC->m_pLatencyToleranceState->m_LatencyTolerance = CurrentLatencyTolerance;

        if(CurrentFState == 0)
            pAdapter->m_bAdapterState = TRUE;

        pAdapter->m_PowerComponentList.push_back(pPC);

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_PowerComponent: Adapter = 0x%p, Type = %d, Index = %d, TypeId = %d, NumberOfStates = %d, Current Latency Tolerance = 0x%I64u, CurrentFState = %d",
                 hAdapter,
                 Type,
                 Index,
                 TypeID,
                 FStateCount,
                 CurrentLatencyTolerance,
                 CurrentFState);
    }
    else if(Dxgk_PowerComponentIdleState.Id == EH.Id())
    {
        // FState description
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");
        UINT FState = EH.GetProperty<UINT>(L"FState");
        ULONGLONG Latency = EH.GetProperty<ULONGLONG>(L"TransitionLatency");

        CFState* pFState = new CFState(FState, Index, Latency);

        if(!pFState)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_PowerComponentIdleState - Failed to allocate CFState");

        CGPUPowerComponent* pPC = pAdapter->GetPowerComponent(Index);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_PowerComponentIdleState - failed to find Power Component");

        pPC->m_FStateList.push_back(pFState);

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_PowerComponentIdleState: Adapter = 0x%p, ComponentIndex = %d, FState = %d, Latency = 0x%I64u",
                 hAdapter,
                 Index,
                 FState,
                 Latency);
    }
    else if(Dxgk_SetPowerComponentFState.Id == EH.Id())
    {
        // Start FState transition
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");
        UINT FState = EH.GetProperty<UINT>(L"State");

        CGPUPowerComponent* pPC = pAdapter->GetPowerComponent(Index);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_SetPowerComponentFState - failed to find Power Component");	

        pPC->m_RequestedFState = FState;
        pPC->m_RequestedFStateTimeStamp = EH.ullTimeStamp();

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_SetPowerComponentFState: Adapter = 0x%p, ComponentIndex = %d, RequestedFState = %d",
                 hAdapter,
                 Index,
                 FState);
    }
    else if(Dxgk_SetPowerComponentFStateEnd.Id == EH.Id() || Dxgk_CompleteFStateTransitionCB.Id == EH.Id())
    {
        // End FState transition
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");

        CGPUPowerComponent* pPC = pAdapter->GetPowerComponent(Index);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_SetPowerComponentFStateEnd - failed to find Power Component");

        CFState* pFState;
        
        if(pPC->m_RequestedFState != 0)
            pFState = pPC->GetFState(pPC->m_RequestedFState);
        else 
            pFState = pPC->GetFState(pPC->m_CurrentFState);

        if(!pFState)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_SetPowerComponentFStateEnd - failed to find FState");

        ULONGLONG ActualLatency = m_etwController.TimeStampToMicroseconds(EH.ullTimeStamp() - pPC->m_RequestedFStateTimeStamp) * 10;	// in 100s ns

        if(pPC->m_RequestedFState == 0 && pPC->m_RequestedFStateTimeStamp)
        {
            // We are transitionning to F0
            pFState->m_TransitionCountToF0++;
            pFState->m_TotalActualLatencyToF0 += ActualLatency;

            if(ActualLatency < pFState->m_MinActualLatencyToF0)
                pFState->m_MinActualLatencyToF0 = ActualLatency;

            if(ActualLatency > pFState->m_MaxActualLatencyToF0)
                pFState->m_MaxActualLatencyToF0 = ActualLatency;
        }
        else if(pPC->m_RequestedFState != 0 && pPC->m_RequestedFStateTimeStamp)
        {
            // we are transitioning to Fn
            pFState->m_TransitionCountFromF0++;
            pFState->m_TotalActualLatencyFromF0 += ActualLatency;

            if(ActualLatency < pFState->m_MinActualLatencyFromF0)
                pFState->m_MinActualLatencyFromF0 = ActualLatency;

            if(ActualLatency > pFState->m_MaxActualLatencyFromF0)
                pFState->m_MaxActualLatencyFromF0 = ActualLatency;
        }

        pPC->m_CurrentFState = pPC->m_RequestedFState;
        pPC->m_CurrentFStateTimeStamp = EH.ullTimeStamp();
        pPC->m_RequestedFState = 0;
        pPC->m_RequestedFStateTimeStamp = 0;

        DBGPrint(L"HandleRuntimePowerManagement - %s: Adapter = 0x%p, ComponentIndex = %d, NewState = %d, TransitionTime = 0x%I64u",
                 (Dxgk_SetPowerComponentFStateEnd.Id == EH.Id()) ? L"Dxgk_SetPowerComponentFStateEnd" : L"Dxgk_CompleteFStateTransitionCB",
                 hAdapter,
                 Index,
                 pPC->m_CurrentFState,
                 ActualLatency);
    }
    else if(Dxgk_PowerRuntimeComponentActiveCallback.Id == EH.Id())
    {
        // This event indicates when component is Active/Idle
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");
        UINT bActive = EH.GetProperty<BOOL>(L"State");

        CGPUPowerComponent* pPC = pAdapter->GetPowerComponent(Index);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_PowerRuntimeComponentActiveCallback - failed to find Power Component");

        if(bActive)
        {
            pPC->m_State = Active;
            pPC->m_pLatencyToleranceState->m_ActiveTimeStamp = EH.ullTimeStamp();
        }
        else 
        {
            pPC->m_State = Idle;
            pPC->m_pLatencyToleranceState->m_IdleTimeStamp = EH.ullTimeStamp();
        }

        pPC->m_StateTimeStamp = EH.ullTimeStamp();

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_PowerRuntimeComponentActiveCallback: Adapter = 0x%p, ComponentIndex = %d, State = %s",
                 hAdapter,
                 Index,
                 bActive ? L"TRUE" : L"FALSE");
    }
    else if(Dxgk_SetPowerComponentLatencyCB.Id == EH.Id())
    {
        // This event is received whenever LatencyTolerance is changed for a particular component
        UINT Index = EH.GetProperty<UINT>(L"ComponentIndex");
        ULONGLONG LatencyTolerance = EH.GetProperty<ULONGLONG>(L"Latency");

        CGPUPowerComponent* pPC = pAdapter->GetPowerComponent(Index);

        if(!pPC)
            throw std::exception("HandleRuntimePowerManagement - Dxgk_SetPowerComponentLatencyCB - failed to find Power Component");

        pPC->m_LatencyTolerance = LatencyTolerance;
        pPC->m_pLatencyToleranceState->m_LatencyTolerance = LatencyTolerance;
        pPC->m_pLatencyToleranceState->m_LatencyToleranceTimeStamp = EH.ullTimeStamp();

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_SetPowerComponentLatencyCB: Adapter = 0x%p, ComponentIndex = %d, LatencyTolerance = 0x%I64u",
                 hAdapter,
                 Index,
                 LatencyTolerance);
    }
    else if(Dxgk_PowerRuntimeDevicePowerRequiredCallback.Id == EH.Id())
    {
        // This event idicates when adapter goes in and out of D3 state
        UINT bActive = EH.GetProperty<BOOL>(L"State");

        pAdapter->m_bAdapterState = bActive;
        pAdapter->m_AdapterStateTimestamp = EH.ullTimeStamp();

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_PowerRuntimeDevicePowerRequiredCallback: Adapter = 0x%p, State = %s",
                 hAdapter,
                 bActive ? L"TRUE" : L"FALSE");
    }
    else if(Dxgk_PrimaryMonitorPowerChange.Id == EH.Id())
    {
        UINT State = EH.GetProperty<UINT>(L"State");

        PowerComponentList::iterator it = pAdapter->m_PowerComponentList.begin();

        for(it;it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;
            pPC->m_pLatencyToleranceState->m_bMonitorOn = State;
        }

        DBGPrint(L"HandleRuntimePowerManagement - Dxgk_PrimaryMonitorPowerChange: State = %s",
                 State ? L"TRUE" : L"FALSE");
    }
}

void CHybridETWMaster::HandleDdiSetPowerState(PEVENT_RECORD pEvent)
{
    if(!m_bProcessPowerEvents)
        return;

    CDxgKernelEventHelper EH(pEvent);
    HANDLE MiniportContext = EH.GetProperty<HANDLE>(L"MiniportContext");
    DEVICE_POWER_STATE DevicePowerState = (DEVICE_POWER_STATE)EH.GetProperty<UINT>(L"DevicePowerState");
    
    // we only care about transitions of dGPU
    CAdapter* pDGPUAdapter = GetDGpu();

    if(!pDGPUAdapter)
        throw std::exception("Didn't find dGPU");
    else
    {
        if(pDGPUAdapter->m_hMiniportContext != MiniportContext)
        {
            DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - UNEXPECTED!!! MiniportContext (0x%p) doesn't match that of dGPU (0x%p)",
                     MiniportContext,
                     pDGPUAdapter->m_hMiniportContext);
            return;
        }

        if(EH.IsStart())
        {
            if(DevicePowerState != PowerDeviceD0 && DevicePowerState != PowerDeviceD3)
            {
                DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - UNEXPECTED!!! DevicePowerState == %d", (UINT)DevicePowerState);
                return;
            }

            pDGPUAdapter->m_DState.m_TransitionToPowerState = DevicePowerState;
            pDGPUAdapter->m_DState.m_TransitionStartTS = EH.ullTimeStamp();
            pDGPUAdapter->m_DState.m_TransitionStartDPCTotalTime = m_DPCTimeTotal;
            pDGPUAdapter->m_DState.m_TransitionStartInterruptTotalTime = m_InterruptTimeTotal;

            DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - Started transition to %d from %d, timestamp = %I64u",
                      (UINT) pDGPUAdapter->m_DState.m_PowerState,
                      (UINT) pDGPUAdapter->m_DState.m_TransitionToPowerState,
                      pDGPUAdapter->m_DState.m_TransitionStartTS);
        }
        else if(EH.IsStop())
        {
            if(pDGPUAdapter->m_DState.m_TransitionToPowerState == PowerDeviceUnspecified)
            {
                DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - SKIPPING!!! Got Stop event, but missed Start event");
                return;
            }

            ULONGLONG TransitionEndTS = EH.ullTimeStamp();

            if(pDGPUAdapter->m_DState.m_TransitionStartTS > TransitionEndTS)
            {
                DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - UNEXPECTED!!! TransitionStartTS (%I64u) is greater then TransitionEndTS (%I64u)",
                         pDGPUAdapter->m_DState.m_TransitionStartTS,
                         TransitionEndTS);
                return;
            }

            ULONGLONG DPCTimeTotal = m_DPCTimeTotal - pDGPUAdapter->m_DState.m_TransitionStartDPCTotalTime;
            ULONGLONG IterruptTimeTotal = m_InterruptTimeTotal - pDGPUAdapter->m_DState.m_TransitionStartInterruptTotalTime;
            ULONGLONG Latency = m_etwController.TimeStampToMicroseconds((TransitionEndTS - pDGPUAdapter->m_DState.m_TransitionStartTS - DPCTimeTotal - IterruptTimeTotal)) * 10; // 100s ns

            if((pDGPUAdapter->m_DState.m_PowerState == PowerDeviceD0 || PowerDeviceUnspecified) 
                && pDGPUAdapter->m_DState.m_TransitionToPowerState == PowerDeviceD3)
            {
                pDGPUAdapter->m_DState.m_TransitionCountToD3++;
                pDGPUAdapter->m_DState.m_TotalActualLatencyToD3 += Latency;
                pDGPUAdapter->m_DState.m_MinActualLatencyToD3 = (pDGPUAdapter->m_DState.m_MinActualLatencyToD3) ? min(pDGPUAdapter->m_DState.m_MinActualLatencyToD3,Latency) : Latency;
                pDGPUAdapter->m_DState.m_MaxActualLatencyToD3 = (pDGPUAdapter->m_DState.m_MaxActualLatencyToD3) ? max(pDGPUAdapter->m_DState.m_MaxActualLatencyToD3,Latency) : Latency;
            }

            else if((pDGPUAdapter->m_DState.m_PowerState == PowerDeviceD3 || PowerDeviceUnspecified) 
                && pDGPUAdapter->m_DState.m_TransitionToPowerState == PowerDeviceD0)
            {
                pDGPUAdapter->m_DState.m_TransitionCountFromD3++;
                pDGPUAdapter->m_DState.m_TotalActualLatencyFromD3 += Latency;
                pDGPUAdapter->m_DState.m_MinActualLatencyFromD3 = (pDGPUAdapter->m_DState.m_MinActualLatencyFromD3) ? min(pDGPUAdapter->m_DState.m_MinActualLatencyFromD3,Latency) : Latency;
                pDGPUAdapter->m_DState.m_MaxActualLatencyFromD3 = (pDGPUAdapter->m_DState.m_MaxActualLatencyFromD3) ? max(pDGPUAdapter->m_DState.m_MaxActualLatencyFromD3,Latency) : Latency;
            }

            DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - Completed transition from %d to %d; latency = %I64u, timestamp = %I64u",
                    pDGPUAdapter->m_DState.m_PowerState,
                    pDGPUAdapter->m_DState.m_TransitionToPowerState,
                    Latency,
                    TransitionEndTS);
                
            pDGPUAdapter->m_DState.m_PowerState = DevicePowerState;
            pDGPUAdapter->m_DState.m_TransitionToPowerState = PowerDeviceUnspecified;
            pDGPUAdapter->m_DState.m_TransitionStartTS = 0;
            pDGPUAdapter->m_DState.m_TransitionStartDPCTotalTime = 0;
            pDGPUAdapter->m_DState.m_TransitionStartInterruptTotalTime = 0;
        }
        else
        {
            DBGPrint(L"CHybridETWMaster::HandleDdiSetPowerState() - UNEXPECTED OpCode");
        }
    }
}

void CHybridETWMaster::HandlePagingOpFill(PEVENT_RECORD pEvent)
{
    //CDxgKernelEventHelper EH(pEvent);
    //HANDLE hAdapter = EH.GetProperty<HANDLE>(L"pDxgAdapter");
    //UINT SegmentId = EH.GetProperty<UINT>(L"SegmentId");
    //ULONGLONG Size = EH.GetProperty<ULONGLONG>(L"FillSize");
    //HANDLE hGlobalAlloc = EH.GetProperty<HANDLE>(L"hAllocationGlobalHandle");

    //// Find Adapter
 //   CSLocker Lock(m_CritSection);

    //CAdapter* pAdapter = GetAdapter(hAdapter);

    //if(!pAdapter)
    //	throw std::exception("HandlePagingOpFill - failed to find adapter");

    //CMemorySegment* pSegment = pAdapter->GetMemorySegment(SegmentId);

    //if(!pSegment)
    //	throw std::exception("HandlePagingOpFill - failed to find segment");

    //if(!pSegment->m_Flags.Aperture)
    //{
    //	CAllocation* pAllocation = pAdapter->FindAllocation(hGlobalAlloc);

    //	if(!pAllocation)
    //		throw std::exception("HandlePagingOpFill - failed to find allocation");

    //	if(!pAllocation->m_bCommitted)
    //	{
    //		if((pSegment->m_BytesAllocated + pAllocation->m_Size) > pSegment->m_Size)
    //			throw std::exception("HandlePagingOpFill - BytesAllocated calculation is incorrect");

    //		pSegment->m_BytesAllocated += Size;
    //		pAllocation->m_bCommitted = true;
    //	}
    //}
}


void CHybridETWMaster::HandlePagingOpDiscard(PEVENT_RECORD pEvent)
{
    //CDxgKernelEventHelper EH(pEvent);
    //HANDLE hAdapter = EH.GetProperty<HANDLE>(L"pDxgAdapter");
    //HANDLE hGlobalAlloc = EH.GetProperty<HANDLE>(L"hAllocationGlobalHandle");
    //UINT SegmentId = EH.GetProperty<UINT>(L"SegmentId");

    //// Find Adapter
 //   CSLocker Lock(m_CritSection);
    //CAdapter* pAdapter = GetAdapter(hAdapter);

    //if(!pAdapter)
    //	throw std::exception("HandlePagingOpDiscard - failed to find adapter");

    //CMemorySegment* pSegment = pAdapter->GetMemorySegment(SegmentId);

    //if(!pSegment)
    //	throw std::exception("HandlePagingOpDiscard - failed to find segment");

    //if(!pSegment->m_Flags.Aperture)
    //{
    //	CAllocation* pAllocation = pAdapter->FindAllocation(hGlobalAlloc);

    //	if(!pAllocation)
    //		throw std::exception("HandlePagingOpDiscard - failed to find allocation");

    //	if((pSegment->m_BytesAllocated - pAllocation->m_Size) > (pSegment->m_BytesAllocated))
    //	{
    //		//throw std::exception("HandlePagingOpDiscard - BytesAllocated calculation is incorrect"); //TODO: Figure this out
    //		return;
    //	}

    //	pAllocation->m_bCommitted = false;
    //	pSegment->m_BytesAllocated -= pAllocation->m_Size;
    //}
}


void CHybridETWMaster::HandlePagingOpTransfer(PEVENT_RECORD pEvent)
{
    //CDxgKernelEventHelper EH(pEvent);
    //HANDLE hAdapter = EH.GetProperty<HANDLE>(L"pDxgAdapter");
    //ULONGLONG Size = EH.GetProperty<ULONGLONG>(L"TransferSize");
    //UINT SrcSegmentId = EH.GetProperty<UINT>(L"SourceSegmentId");
    //UINT DstSegmentId = EH.GetProperty<UINT>(L"DestinationSegmentId");
    //HANDLE hGlobalAlloc = EH.GetProperty<HANDLE>(L"hAllocationGlobalHandle");

    //// Find Allocation
 //   CSLocker Lock(m_CritSection);
    //CAdapter* pAdapter = GetAdapter(hAdapter);

    //if(!pAdapter)
    //	throw std::exception("HandlePagingOpTransfer - failed to find adapter");

    //CMemorySegment* pSrcSegment = pAdapter->GetMemorySegment(SrcSegmentId);

    //if(!pSrcSegment)
    //	throw std::exception("HandlePagingOpTransfer - failed to find source segment");

    //CMemorySegment* pDstSegment = pAdapter->GetMemorySegment(DstSegmentId);

    //if(!pDstSegment)
    //	throw std::exception("HandlePagingOpTransfer - failed to find destination segment");

    //if((pSrcSegment && !pSrcSegment->m_Flags.Aperture) ||
    //	(pDstSegment && !pDstSegment->m_Flags.Aperture))
    //{

    //	CAllocation* pAllocation = pAdapter->FindAllocation(hGlobalAlloc);

    //	if(!pAllocation)
    //		throw std::exception("HandlePagingOpTransfer - failed to find allocation");

    //	if(pSrcSegment && !pSrcSegment->m_Flags.Aperture && !pAllocation->m_bCommitted)
    //	{
    //		if((pSrcSegment->m_BytesAllocated + pAllocation->m_Size) > pSrcSegment->m_Size)
    //			throw std::exception("HandlePagingOpTransfer - BytesAllocated calculation is incorrect");

    //		pSrcSegment->m_BytesAllocated += Size;
    //		pAllocation->m_bCommitted = true;
    //	}

    //	if((pDstSegment && !pDstSegment->m_Flags.Aperture) && pAllocation->m_bCommitted)
    //	{
    //		if((pDstSegment->m_BytesAllocated - pAllocation->m_Size) > (pDstSegment->m_BytesAllocated))
    //			throw std::exception("HandlePagingOpTransfer - BytesAllocated calculation is incorrect");

    //		pDstSegment->m_BytesAllocated -= Size;
    //		pAllocation->m_bCommitted = false;
    //	}		
    //}
}


void CHybridETWMaster::HandleReportCommittedGlobalAllocation(PEVENT_RECORD pEvent)
{
    //CDxgKernelEventHelper EH(pEvent);
    //HANDLE hGlobalAlloc = EH.GetProperty<HANDLE>(L"hGlobalAllocationHandle");
    //UINT SegmentId = EH.GetProperty<UINT>(L"ulSegmentId");

    //// Find Allocation
 //   CSLocker Lock(m_CritSection);
    //CAdapter* pAdapter = NULL;
    //CAllocation* pAlloc = NULL;

 //   for(AdapterList::iterator it = m_Adapters.begin();
 //       it != m_Adapters.end();
 //       it++)
 //   {
 //       pAdapter = *it;
 //       pAlloc = pAdapter->FindAllocation(hGlobalAlloc);

    //	if(pAlloc)
    //		break;

    //	pAdapter = NULL;
    //	pAlloc = NULL;
 //   }

    //if(!pAdapter)
    //	throw std::exception("HandleReportCommittedGlobalAllocation - failed to find adapter");

    //if(!pAlloc)
    //	throw std::exception("HandleReportCommittedGlobalAllocation - failed to find allocation");


    //CMemorySegment* pSegment = pAdapter->GetMemorySegment(SegmentId);

    //if(!pSegment)
    //	throw std::exception("HandleReportCommittedGlobalAllocation - failed to find segment");

    //pSegment->m_BytesAllocated += pAlloc->m_Size;
}


void CHybridETWMaster::HandleGraphicsTestMessage(PEVENT_RECORD pEvent)
{
    WCHAR* pMessage = (WCHAR*) pEvent->UserData;

    // if this is a WaitForEvents event, signal
    if(!wcsncmp(pMessage, HYBRID_PRESENT_WAIT_FOR_EVENTS, wcslen(HYBRID_PRESENT_WAIT_FOR_EVENTS)))
    {
        SetEvent(m_hWaitForEvents);
    }

    if(!wcsncmp(pMessage, HYBRID_POWER_MANAGEMENT_VALIDATE, wcslen(HYBRID_POWER_MANAGEMENT_VALIDATE)))
    {
        CAdapter* pAdapter = GetDGpu();

        if(!pAdapter)
            throw std::exception("HandleGraphicsTestMessage - HYBRID_POWER_MANAGEMENT_VALIDATE - failed to find dGPU");

        PowerComponentList::iterator it = pAdapter->m_PowerComponentList.begin();

        for(it; it != pAdapter->m_PowerComponentList.end(); it++)
        {
            CGPUPowerComponent* pPC = *it;

            pPC->UpdateActiveStatus(pEvent->EventHeader.TimeStamp.QuadPart);
            pPC->ValidateLatencyTolerance(pEvent->EventHeader.TimeStamp.QuadPart, this);
        }
    }
    
    DBGPrint(pMessage);
}

void CHybridETWMaster::PrintAdapters()
{
    CSLocker Lock(m_CritSection);
    AdapterList::iterator it = m_Adapters.begin();

    DBGPrint(L"//////////////// PRINTING ADAPTERS ///////////////////////");
    DBGPrint(L"++++++++++++++++ PRINTING TRACKED DEVICES +++++++++++++++++++++++");

    for(it; it != m_Adapters.end(); it++)
    {
        CAdapter* pAdapter = *it;

        for(DeviceList::iterator itDevice = pAdapter->m_DeviceList.begin();
            itDevice != pAdapter->m_DeviceList.end();
            itDevice++)
        {
            CDevice* pDevice = *itDevice;

            for(HandleList::iterator itContext = pDevice->m_hContextList.begin();
                itContext != pDevice->m_hContextList.end();
                itContext++)
            {
                DBGPrint(L"Adapter = 0x%p, Process = 0x%p, Device = 0x%p, Context = 0x%p", pAdapter->m_hAdapter, pDevice->m_hProcess, pDevice->m_hDeviceHandle, *itContext);
            }
        }
    }

    DBGPrint(L"++++++++++++++++ PRINTING ENGINES AND CONTEXTS +++++++++++++++++++++++");
    for(it = m_Adapters.begin(); it != m_Adapters.end(); it++)
    {
        CAdapter* pAdapter = *it;

        for(NodeList::iterator itNode = pAdapter->m_NodeList.begin();
            itNode != pAdapter->m_NodeList.end();
            itNode++)
        {
            CNode* pNode = *itNode;

            for(HandleList::iterator itContext = pNode->m_hContextList.begin();
                itContext != pNode->m_hContextList.end();
                itContext++)
            {
                DBGPrint(L"Adapter = 0x%p, Node Ordinal = %d, Power Component Index = %d, Context = 0x%p", pAdapter->m_hAdapter, pNode->m_NodeOrdinal, (pNode->m_pPC)?pNode->m_pPC->m_Index:0xffffffff, *itContext);
            }
        }
    }

    DBGPrint(L"//////////////// DONE PRINTING ADAPTERS ///////////////////////");
}

/////////////////////////////////////////////////////////////
// Provider
/////////////////////////////////////////////////////////////
ULONG CHybridETWProvider::WriteToETW(__in const LPWSTR pwszMessage, ...)
{
    ULONG uRet = 0;
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    uRet = SendMessageTrace(pwszOut, true);

    if( pwszOut )
    {
        delete[] pwszOut;
    }

    return uRet;
}

void CritSection::Lock() 
{		
    //DBGPrint(L"Entering Critical Section, RefCount=%d", m_RefCount);
    EnterCriticalSection(&m_CritSection);
    m_RefCount++;
    //DBGPrint(L"Entered Critical Section, RefCount=%d", m_RefCount);
}
void CritSection::Unlock() 
{
    //DBGPrint(L"Leaving Critical Section, RefCount=%d", m_RefCount);
    LeaveCriticalSection(&m_CritSection);
    m_RefCount--;
    //DBGPrint(L"Left Critical Section, RefCount=%d", m_RefCount);
}
CSLocker CritSection::Enter()
{
    return CSLocker(*this);
}

CSLocker::CSLocker(CritSection& Target) : m_Target(Target)
{
    m_Target.Lock();
    m_bOwns = true;
}

CSLocker::CSLocker(CSLocker& RHS) : m_Target(RHS.m_Target)
{
    m_bOwns = RHS.m_bOwns;
    RHS.m_bOwns = false;
}

CSLocker::~CSLocker()
{
    if(m_bOwns)
    {
        m_Target.Unlock();
    }   
}
