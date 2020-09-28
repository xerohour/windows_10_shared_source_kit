/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPowerManagement.cpp
//
//	Abstract:
//	This file contains implementation of DXGIHybridPowerManagement tests
//
//	History:
//	03/14/2013	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "DXGIHybridPowerManagement.h"
#include <Sddl.h>
#include <SynchronizationEvent.h>
#include <SecurityDescriptors.h>
#include "PowerUtil.h"

using namespace WindowManager;
using namespace GRFX::POWER;

extern bool g_bSkipIfNotHybrid;
extern TCHAR* g_PowerComponentTypeNames[];

// log functions used by PowerUtil
CHybridPowerManagement* g_pThis = NULL;

// log functions used by PowerUtil
void LogFail(const wchar_t* szFmtString,...)  
{
    va_list args;
    va_start( args, szFmtString );
    std::wstring msg = FormatStringVATS( szFmtString, args );
    DBGPrint( (LPWSTR)msg.c_str() );
    if(g_pThis)
        g_pThis->WriteToLog((LPWSTR)msg.c_str() );
    va_end(args);
}

void LogSkip(const wchar_t* szFmtString,...)  
{
    va_list args;
    va_start( args, szFmtString );
    std::wstring msg = FormatStringVATS( szFmtString, args );
    DBGPrint( (LPWSTR)msg.c_str() );
    if(g_pThis)
        g_pThis->WriteToLog((LPWSTR)msg.c_str() );
    va_end(args);
}

void LogMessage(const wchar_t* szFmtString,...)  
{
    va_list args;
    va_start( args, szFmtString );
    std::wstring msg = FormatStringVATS( szFmtString, args );
    DBGPrint( (LPWSTR)msg.c_str() );
    if(g_pThis)
        g_pThis->WriteToLog((LPWSTR)msg.c_str() );
    va_end(args);
}


BEGIN_NAMED_VALUES(HYBRID_PM_VALIDATIONS)
    NAMED_VALUE(_T("VALIDATE_POWER_COMPONENTS"), VALIDATE_POWER_COMPONENTS)
    NAMED_VALUE(_T("VALIDATE_LATENCY_TOLERANCE"), VALIDATE_LATENCY_TOLERANCE)
    NAMED_VALUE(_T("VALIDATE_TRANSITION_LATENCY"), VALIDATE_TRANSITION_LATENCY)
    NAMED_VALUE(_T("VALIDATE_ACTIVE_IDLE_STATUS"), VALIDATE_ACTIVE_IDLE_STATUS)
    NAMED_VALUE(_T("VALIDATE_DSTATE"), VALIDATE_DSTATE)
    NAMED_VALUE(_T("VALIDATE_DGPU_PROCESSES"), VALIDATE_DGPU_PROCESSES)
END_NAMED_VALUES(HYBRID_PM_VALIDATIONS)

#define MANAGER_CHECK_CLIENT_STATUS                                                                           \
{                                                                                                             \
    DBGPrint(L"MANAGER_CHECK_CLIENT_STATUS - Checking Client's status");                                      \
    if(m_pManagerSync->AcquireLock())                                                                         \
    {                                                                                                         \
        DBGPrint(L"MANAGER_CHECK_CLIENT_STATUS - Successfully aquired lock");                                 \
        sHYBRID_PRESENT_SHARED_DATA *pData = (sHYBRID_PRESENT_SHARED_DATA*)m_pManagerSync->GetExtraDataPtr(); \
        assert(pData);                                                                                        \
        if(pData->tr != RESULT_PASS)                                                                          \
        {                                                                                                     \
            DBGPrint(L"MANAGER_CHECK_CLIENT_STATUS - Following error in client process is detected: %s", pData->strError); \
            WriteToLog(_T("Following error in client process is detected:"));                                 \
            WriteToLog(pData->strError);                                                                      \
            tr = pData->tr;                                                                                   \
            if(pData->tr != RESULT_FAIL)                                                                      \
            {                                                                                                 \
                m_pManagerSync->ReleaseLock();                                                                \
                goto MacroFailed;                                                                             \
            }                                                                                                 \
        }                                                                                                     \
        m_pManagerSync->ReleaseLock();                                                                        \
    }                                                                                                         \
    DBGPrint(L"MANAGER_CHECK_CLIENT_STATUS - Done checking Client's status");                                 \
}                                                                                                             \


#define MANAGER_SIGNAL                                                                                     \
DBGPrint(L"MANAGER_SIGNAL - Signaling client\n");                                                         \
if(!m_pManagerSync->Signal(SYNC_PAIR_INDEX, MANAGER_HANDLE))                                              \
{                                                                                                         \
    DBGPrint(L"MANAGER_SIGNAL - failed to signal client\n");                                              \
    tr = RESULT_BLOCKED;                                                                                  \
    goto MacroFailed;                                                                                     \
}                                                                                                         


#define MANAGER_SIGNAL_AND_WAIT                                                                           \
                                                                                                          \
MANAGER_SIGNAL                                                                                            \
DBGPrint(L"MANAGER_SIGNAL_AND_WAIT - Waiting for client signal\n");                                       \
if(WAIT_OBJECT_0 !=  m_pManagerSync->WaitForSignal(SYNC_PAIR_INDEX, CLIENT_HANDLE, XPROC_TIMEOUT))        \
{                                                                                                         \
    DBGPrint(L"MANAGER_SIGNAL_AND_WAIT - Manager timed out waiting for client's signal\n");               \
    tr = RESULT_BLOCKED;                                                                                  \
    goto MacroFailed;                                                                                     \
}                                                                                                         \
DBGPrint(L"MANAGER_SIGNAL_AND_WAIT - Manager got client's signal\n");                                     \
MANAGER_CHECK_CLIENT_STATUS                                                                               



#define MANAGER_SIGNAL_TO_TERMINATE                                                                       \
DBGPrint(L"MANAGER_SIGNAL_TO_TERMINATE - Signaling client\n");                                            \
if(!m_pManagerSync->Signal(SYNC_PAIR_INDEX, MANAGER_HANDLE))                                              \
{                                                                                                         \
    DBGPrint(L"MANAGER_SIGNAL_TO_TERMINATE - Failed to signal client\n");                                 \
    tr = RESULT_BLOCKED;                                                                                  \
}                                                                                                         \
DBGPrint(L"MANAGER_SIGNAL_TO_TERMINATE - Manager is asking client to terminate\n");                       \
if(!m_pManagerSync->TransitionClientsTo(eMoveToTerminate))                                                \
{                                                                                                         \
    DBGPrint(L"MANAGER_SIGNAL_TO_TERMINATE - Manager's request for client to terminate wasn't satisfied");\
    WriteToLog(_T("Manager's request for client to terminate wasn't satisfied\n"));                       \
}                                                                                                         \
DBGPrint(L"MANAGER_SIGNAL_TO_TERMINATE - Client should be terminating now\n");                                                  


TEST_RESULT CHybridPowerManagement::Setup()
{
    TEST_RESULT tr = RESULT_PASS;
    bool bHybridSystem = false;
    bool bFoundBDAOrBDDAdapter = false;
    g_pThis = this;

    // Handle high-dpi setting so we can successfully use GetFrontBuffer
    SetProcessDPIAware();

    //
    // Check if we are running on a hybrid system.
    //

    if( !CHybridPresentRedirection::CheckSystemAdapters( bHybridSystem, bFoundBDAOrBDDAdapter ) )
    {
        WriteToLog(_T( "Setup:  CheckSystemAdapters() failed." ));
        tr = RESULT_FAIL;
    }

    if( !bHybridSystem )
    {
        if(g_bSkipIfNotHybrid)
        {
            // SKIP when running with "-skipifnothybrid" parameter - we expect this test to be run on non-hybrid configurations
            WriteToLog(_T( "Setup:  CHybridPowerManagement is being run on a non-hybrid system. Skipping group." ));
            tr = RESULT_SKIP;
        }
        else
        {
            // FAIL when running without "-skipifnothybrid" parameter is not specified - we don't expect this test to be run on non-hybrid
            // configurations (i.e. in WTT environment (Hybrid support is determined by job constraint) or LOGO environment (Hybrid support is
            // determined by DisplayGatherer)
            WriteToLog(_T( "Setup:  CHybridPowerManagement is being run on a non-hybrid system. Failing group." ));
            tr = RESULT_FAIL;
        }
    }

    if( bFoundBDAOrBDDAdapter )
    {
        g_bBDDHybrid = true;	// WinBlue: 195417
    }

    if(tr == RESULT_PASS)
    {
        bool bInitializeWindowClassResult = InitializeWindowClass();

        if ( bInitializeWindowClassResult != true )
        {
            WriteToLog(_T( "CHybridPowerManagement::Setup() - InitializeWindowClass failed." ));

            tr = RESULT_ABORT;
        };
    }

    if(tr == RESULT_PASS)
    {
        // terminate dtmacmengine.exe, acmengine.exe and sysparse.exe first so that it doesn't make dGPU busy
        tr = StartChildProcess(L"taskkill.exe /IM dtmacmengine.exe /F");
        tr = StartChildProcess(L"taskkill.exe /IM acmengine.exe /F");
        tr = StartChildProcess(L"taskkill.exe /IM sysparse.exe /F");
    }

    return tr;
}


void CHybridPowerManagement::Cleanup()
{
    m_ETWMaster.StopListening();

    CleanupWindowClass();

    g_pThis = NULL;
}


void CHybridPowerManagement::InitTestParameters()
{
    CTestCaseParameter<HYBRID_PM_VALIDATIONS> *pValidationType = AddParameter<HYBRID_PM_VALIDATIONS>(_T("ValidationType"), &m_ValidationType);
    CUserValueSet<HYBRID_PM_VALIDATIONS> * pValidationTypeValues = new CUserValueSet<HYBRID_PM_VALIDATIONS>();
    pValidationTypeValues->AddValue(VALIDATE_POWER_COMPONENTS);
    pValidationTypeValues->AddValue(VALIDATE_LATENCY_TOLERANCE);
    pValidationTypeValues->AddValue(VALIDATE_TRANSITION_LATENCY);
    pValidationTypeValues->AddValue(VALIDATE_ACTIVE_IDLE_STATUS);
    pValidationTypeValues->AddValue(VALIDATE_DSTATE);
    pValidationTypeValues->AddValue(VALIDATE_DGPU_PROCESSES);

    testfactor::RFactor factor = AddParameterValueSet<HYBRID_PM_VALIDATIONS>(pValidationType, pValidationTypeValues);

    SetRootTestFactor(factor);
}


TEST_RESULT CHybridPowerManagement::FocusAway()
{
    SetCursorPos(10, 10);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 10, 10, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 10, 10, 0, 0);

    return RESULT_PASS;
}

TEST_RESULT CHybridPowerManagementNoApp::Setup()
{ 
    TEST_RESULT tr = RESULT_PASS;

    tr = CHybridPowerManagement::Setup();

    if(tr == RESULT_PASS)
    {
        m_ETWMaster.ProcessPowerEvents(true);
        m_ETWMaster.StartListening();

        if(m_bMonitorsOff)
        {
            tr = TurnMonitorsOff();
        }
        Sleep(1000);

        if(tr == RESULT_PASS)
        {
            // Drain events and update components
            if(!m_ETWMaster.PowerManagementValidate())
            {
                WriteToLog(_T("CHybridPowerManagement::ExecuteTestCase() - PowerManagementValidate() failed"));
                tr = RESULT_FAIL;
            }	
        }

        m_ETWMaster.StopListening();

        if(m_bMonitorsOff)
        {
            TurnMonitorsOn();
        }
    }

    return tr;
}

TEST_RESULT CHybridPowerManagementNoApp::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(NoApp);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(NoApp);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(NoApp);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();

    return tr;
}


TEST_RESULT CHybridPowerManagement::ValidateDGPUProcesses()
{
    // We only expect those processes that are launched as a result of this test running to run on dGPU
    TEST_RESULT tr = RESULT_PASS;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidateDState() - No dGPU Found"));
        return RESULT_FAIL;
    }

    for(DeviceList::iterator it = pAdapter->m_DeviceList.begin();
        it != pAdapter->m_DeviceList.end();
        it++)
    {
        CDevice* pDevice = *it;
        
        // if Device doesn't have any contexts, ignore it - such devices won't interfere with Power Management
        // (Devices with no contexts might exist if some object wasn't released and hold reference to the device)
        if(pDevice->m_hContextList.size() == 0)
            continue;

        WCHAR* strName = m_ETWMaster.GetProcessNameByHandle(pDevice->m_hProcess);

        if(!strName)
        {
            WriteToLog(_T("CHybridPowerManagement::ValidateDGPUProcesses() - WARNING!!! Can't find process name for hProcess %p"), pDevice->m_hProcess);
        }
        else
        {
            if(_wcsicmp(strName, L"dxgihybridpresent.exe") && _wcsicmp(strName, L"[system process]"))
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDGPUProcesses() - unexpected process is running on dGPU, results of this test are invalid, process name = %S"), strName);
                tr = RESULT_FAIL;
            }

            delete[] strName;
            strName = NULL;
        }
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::ValidateDState(eHybridPowerManagementState State)
{
    TEST_RESULT tr = RESULT_PASS;

    // Check whether this GPU can go into D3
    PowerComponentList::iterator it;
    bool bCanGoIntoD3 = false;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidateDState() - No dGPU Found"));
        return RESULT_FAIL;
    }

    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {

        CGPUPowerComponent* pPC = *it;	

        if((pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION) && (pPC->m_FStateList.size() == 2))
            bCanGoIntoD3 = true;
    }

    switch(State)
    {
    case NoApp:
    case NoAppMonitorOff:
        {
            if(bCanGoIntoD3 && pAdapter->m_bAdapterState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDState() - dGPU is expected to be in D3 state, but it is in D0 state"));
                tr = RESULT_FAIL;
            }
            else if(!bCanGoIntoD3 && !pAdapter->m_bAdapterState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDState() - dGPU is expected to be in D0 state, but it is in D3 state"));
                tr = RESULT_FAIL;
            }
            break;
        }
    case AppRunning:
    case AppRunningMonitorOff:
        {
            if(!pAdapter->m_bAdapterState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDState() - dGPU is expected to be in D0 state, but it is in D3 state"));
                tr = RESULT_FAIL;
            }
            break;
        }
    case AppIdle:
    case AppIdleMonitorOff:
        {
            bool bActive = false;

            // should be idle if all components are idle
            for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
            {
                CGPUPowerComponent* pPC = *it;	

                if(pPC->m_State == Active)
                {
                    bActive = true;
                    break;
                }
            }

            if(bActive && !pAdapter->m_bAdapterState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDState() - dGPU is expected to be in D0 state, but it is in D3 state"));
                tr = RESULT_FAIL;
            }
            else if(!bActive && pAdapter->m_bAdapterState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateDState() - dGPU is expected to be in D3 state, but it is in D0 state"));
                tr = RESULT_FAIL;
            }

            break;
        }
    }

    return tr;
}


TEST_RESULT CHybridPowerManagement::ValidateLatencyTolerance()
{
    TEST_RESULT tr = RESULT_PASS;

    PowerComponentList::iterator it;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidateLatencyTolerance() - No dGPU Found"));
        return RESULT_FAIL;
    }

    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {
        CGPUPowerComponent* pPC = *it;

        // if there were violations detected, print those out now (no more than 10)
        if(pPC->m_LatencyToleranceViolationList.size() != 0)
        {
            UINT ViolationsCount = 0;
            UINT MaxViolationsCount = 10;
            LatencyToleranceViolationList::iterator itViolation = pPC->m_LatencyToleranceViolationList.begin();

            WriteToLog(_T("CHybridPowerManagement::ValidateLatencyTolerance() - %d Latency Tolerance violations have been detected for component %d (Type = %s), printing out up to %d"),
                       pPC->m_LatencyToleranceViolationList.size(),
                       pPC->m_Index,
                       g_PowerComponentTypeNames[pPC->m_ComponentTypeNameIndex],
                       MaxViolationsCount);

            tr = RESULT_FAIL;

            for(itViolation; itViolation != pPC->m_LatencyToleranceViolationList.end() && ViolationsCount < MaxViolationsCount; itViolation++)
            {
                CLatencyToleranceState* pLTState = *itViolation;

                UINT IdleTime = (UINT)((pLTState->m_ActiveTimeStamp > pLTState->m_IdleTimeStamp) ? 0 : (pLTState->m_CurrentTimeStamp - pLTState->m_IdleTimeStamp));
                IdleTime = IdleTime / 10; // convert to us

                if(pLTState->m_ExpectedLatencyTolerance)
                {
                    WriteToLog(_T("LATENCY TOLERANCE VIOLATION - ActiveTS = %I64u, IdleTS = %I64u, CurrentTS = %I64u, IdleTime = %d (us), bMonitorOn = %S, bActiveContexts = %S, LatencyTolerance = %I64u, LatencyToleranceTS = %I64u, Expected Latency Tolerance = %I64u"),
                                pLTState->m_ActiveTimeStamp,
                                pLTState->m_IdleTimeStamp,
                                pLTState->m_CurrentTimeStamp,
                                IdleTime,
                                (pLTState->m_bMonitorOn)?L"TRUE":L"FALSE",
                                (pLTState->m_ActiveContextsCount)?L"TRUE":L"FALSE",
                                pLTState->m_LatencyTolerance,
                                pLTState->m_LatencyToleranceTimeStamp,
                                pLTState->m_ExpectedLatencyTolerance);
                }
                else
                {
                    WriteToLog(_T("LATENCY TOLERANCE VIOLATION - ActiveTS = %I64u, IdleTS = %I64u, CurrentTS = %I64u, IdleTime = %d (us), bMonitorOn = %S, bActiveContexts = %S, LatencyTolerance = %I64u, LatencyToleranceTS = %I64u"),
                                pLTState->m_ActiveTimeStamp,
                                pLTState->m_IdleTimeStamp,
                                pLTState->m_CurrentTimeStamp,
                                IdleTime,
                                (pLTState->m_bMonitorOn)?L"TRUE":L"FALSE",
                                (pLTState->m_ActiveContextsCount)?L"TRUE":L"FALSE",
                                pLTState->m_LatencyTolerance,
                                pLTState->m_LatencyToleranceTimeStamp);
                }

                ViolationsCount++;
            }
        }
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::ValidateActiveIdleStatus(eHybridPowerManagementState State)
{
    TEST_RESULT tr = RESULT_PASS;
    PowerComponentList::iterator it;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: No dGPU Found"));
        return RESULT_FAIL;
    }

    switch(State)
    {
    case NoApp:
    case NoAppMonitorOff:
        {			
            for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
            {
                CGPUPowerComponent* pPC = *it;
                bool bExpectIdle = true;

                // This component is not managed by DXGKernel and we don't know what criteria is used to set it active/idle. Skip it.
                if(pPC->m_Type == DXGK_POWER_COMPONENT_OTHER)
                    continue;

                if((pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION) && (pPC->m_FStateList.size() == 1))
                    bExpectIdle = false;

                if(pPC->m_State == Idle && !bExpectIdle)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: component %d is expected to be in Active state, but it is Idle"), pPC->m_Index);
                    tr = RESULT_FAIL;
                }

                if(pPC->m_State == Active && bExpectIdle)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: component %d is expected to be in Idle state, but it is Active"), pPC->m_Index);
                    tr = RESULT_FAIL;
                }
            }

            break;
        }
    case AppRunning:
    case AppIdle:
    case AppRunningMonitorOff:
    case AppIdleMonitorOff:
        {
            for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
            {
                CGPUPowerComponent* pPC = *it;
                bool bExpectIdle = true;

                // This component is not managed by DXGKernel and we don't know what criteria is used to set it active/idle. Skip it.
                if(pPC->m_Type == DXGK_POWER_COMPONENT_OTHER)
                    continue;

                if((pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION) && (pPC->m_FStateList.size() == 1))
                    bExpectIdle = false;

                if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY)
                {
                    CMemorySegment* pSegment = (CMemorySegment*)pPC->m_pComponent;

                    if(!pSegment)
                    {
                        WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: memory component %d is not associated with a memory segment"), pPC->m_Index);
                        tr = RESULT_FAIL;
                    }
                    else
                    {
                        if(pSegment->m_BytesAllocated)
                            bExpectIdle = false;
                        else
                            bExpectIdle = true;
                    }
                }

                if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY_REFRESH)
                {
                    NodeList::iterator itNode = pAdapter->m_NodeList.begin();

                    for(itNode; itNode != pAdapter->m_NodeList.end(); itNode++)
                    {
                        CNode* pNode = *itNode;

                        if(pNode && pNode->m_pPC->m_State == Active)
                        {
                            bExpectIdle = false;
                            break;
                        }				
                    }

                    if(bExpectIdle == true)
                    {
                        // Memory refresh must be Active as long as any of the engines are active
                        // However, if all engines are idle it might be still active if any of the
                        // allocations are locked in video memory. Test is not tracking this, therefore
                        // we cannot have expectations in this case;
                        continue;
                    }
                }

                if(pPC->m_Type == DXGK_POWER_COMPONENT_ENGINE)
                {
                    CNode* pNode = (CNode*) pPC->m_pComponent;

                    if(!pNode)
                    {
                        WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: engine component %d is not associated with an engine"), pPC->m_Index);
                        tr = RESULT_FAIL;
                    }
                    else
                    {
                        if(pNode->m_hContextList.empty())
                            bExpectIdle = true;

                        // For engine components we'd have to track DMAs to get their Active/Idle states. This test doesn't do this
                        // at the moment, so we will only validate that engines that don't have contexts are idle
                        else
                            continue;
                    }
                }

                if(pPC->m_State == Idle && !bExpectIdle)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: component %d is expected to be in Active state, but it is Idle"), pPC->m_Index);
                    tr = RESULT_FAIL;
                }

                if(pPC->m_State == Active && bExpectIdle)
                {
                    if((pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION) && (pPC->m_FStateList.size() == 2))
                    {
                        // For D3_Transition component with 2 FStates, DXGKernel will not change its state to Active,
                        // but an IHV might change it to Active for a short period of time. But this should only be
                        // acceptable in those scenarios when GPU is not idle.
                        if(State == AppRunning || State == AppRunningMonitorOff)
                        {
                            WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - INFO: DXGK_POWER_COMPONENT_D3_TRANSITION component with 2 FStates is Active"));
                            continue;
                        }
                    }

                    WriteToLog(_T("CHybridPowerManagement::ValidateActiveIdleStatus() - ERROR: component %d is expected to be in Idle state, but it is Active"), pPC->m_Index);
                    tr = RESULT_FAIL;
                }
            }
        }
        break;
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::ValidateTransitionLatency(eHybridPowerManagementState State)
{
    TEST_RESULT tr = RESULT_PASS;
    PowerComponentList::iterator it;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();
    bool bD3Transitions = false;
    bool bEngineTransitions = false;
    bool bMemoryTransitions = false;
    bool bMemoryRefreshTransitions = false;
    bool bExpectD3Transitions = false;
    bool bExpectMemoryRefreshTransitions = false;
    ULONGLONG ReportedD3TransitionLatency = 0;

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - No dGPU Found"));
        return RESULT_FAIL;
    }

    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {
        CGPUPowerComponent* pPC = *it;

        if(pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION &&
           pPC->m_FStateList.size() == 1)
        {
            // this component will never transition
            continue;
        }
        else
        {
            bExpectD3Transitions = true;
        }

        if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY_REFRESH && !bExpectMemoryRefreshTransitions)
        {
            bExpectMemoryRefreshTransitions = true;
        }

        FStateList::iterator itF;

        for(itF = pPC->m_FStateList.begin(); itF != pPC->m_FStateList.end(); itF++)
        {
            CFState* pFState = *itF;

            if(pFState->m_FState == 0)
                continue;

            if(!pFState->m_TransitionCountFromF0 || !pFState->m_TransitionCountToF0)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - INFO: no FState transitions detected for component %d, FState = %d"),
                            pPC->m_Index, 
                            pFState->m_FState);
                continue;
            }
            else
            {
                if(pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION)
                    bD3Transitions = true;
                else if(pPC->m_Type == DXGK_POWER_COMPONENT_ENGINE)
                    bEngineTransitions = true;
                else if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY)
                    bMemoryTransitions = true;
                else if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY_REFRESH)
                    bExpectMemoryRefreshTransitions = true;
            }

            ULONGLONG MaxExpectedLatency = (ULONGLONG)((double)pFState->m_ReportedLatency * 1.1);	// 10% margine of error
            ULONGLONG MinExpectedLatency = (ULONGLONG)((double)pFState->m_ReportedLatency * 0.9);	// 10% margine of error
            ULONGLONG ActualLatencyFromF0 = pFState->m_TotalActualLatencyFromF0 / pFState->m_TransitionCountFromF0;
            ULONGLONG ActualLatencyToF0 = pFState->m_TotalActualLatencyToF0 / pFState->m_TransitionCountToF0;			

            // Memory component with ActiveInD3 flag must report latency 0 (per DDI Spec). However, actual transition will
            // take some little time and checks below will fail. For Memory component with ActiveInD3, just check that transition
            // time is reasonable (<= 50 us)
            if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY && pPC->m_Flags.ActiveInD3)
            {
                const UINT ReasonableTransitionTime = 500; // this time is in 100s ns
                if(ActualLatencyFromF0 > ReasonableTransitionTime)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Actual transition from F0 for Memory component (%d) with ActiveInD3 is too high: ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                               pPC->m_Index,
                               ActualLatencyFromF0,
                               pFState->m_MaxActualLatencyFromF0,
                               pFState->m_MinActualLatencyFromF0,
                               pFState->m_TransitionCountFromF0);

                    tr = RESULT_FAIL;
                }

                if(ActualLatencyToF0 > ReasonableTransitionTime)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Actual transition to F0 for Memory component (%d) with ActiveInD3 is too high: ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                               pPC->m_Index,
                               ActualLatencyFromF0,
                               pFState->m_MaxActualLatencyFromF0,
                               pFState->m_MinActualLatencyFromF0,
                               pFState->m_TransitionCountFromF0);

                    tr = RESULT_FAIL;
                }

                // we are done with this component, move to the next one
                continue;
            }

            // D3_Transition component's latency covers the roundtrip D-State transition of the whole adapter. 
            // Get the Reported latency so that adapter-latency validation can be performed later
            if(pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION)
            {
                if(bExpectD3Transitions)
                    ReportedD3TransitionLatency = pFState->m_ReportedLatency;

                // we are done with this component, move to the next one
                continue;
            }

            if(ActualLatencyFromF0 > MaxExpectedLatency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Actual transition latency from F0 is too high for component %d, FState %d, ReportedLatency = 0x%I64u, ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                            pPC->m_Index,
                            pFState->m_FState,
                            pFState->m_ReportedLatency,
                            ActualLatencyFromF0,
                            pFState->m_MaxActualLatencyFromF0,
                            pFState->m_MinActualLatencyFromF0,
                            pFState->m_TransitionCountFromF0);

                tr = RESULT_FAIL;
            }

            if(ActualLatencyToF0 > MaxExpectedLatency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Actual transition latency to F0 is too high for component %d, FState %d, ReportedLatency = 0x%I64u, ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                            pPC->m_Index,
                            pFState->m_FState,
                            pFState->m_ReportedLatency,
                            ActualLatencyToF0,
                            pFState->m_MaxActualLatencyToF0,
                            pFState->m_MinActualLatencyToF0,
                            pFState->m_TransitionCountToF0);

                tr = RESULT_FAIL;
            }

            if(ActualLatencyFromF0 < MinExpectedLatency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - INFO: Reported transition latency from F0 is too high for component %d, FState %d, ReportedLatency = 0x%I64u, ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                            pPC->m_Index,
                            pFState->m_FState,
                            pFState->m_ReportedLatency,
                            ActualLatencyFromF0,
                            pFState->m_MaxActualLatencyFromF0,
                            pFState->m_MinActualLatencyFromF0,
                            pFState->m_TransitionCountFromF0);

                // Don't fail, this if FYI
            }

            if(ActualLatencyToF0 < MinExpectedLatency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - INFO: Reported transition latency to F0 is too high for component %d, FState %d, ReportedLatency = 0x%I64u, ActualLatency = 0x%I64u, MaxLatency = 0x%I64u, MinLatency = 0x%I64u, TransitionCount = %d"),
                            pPC->m_Index,
                            pFState->m_FState,
                            pFState->m_ReportedLatency,
                            ActualLatencyToF0,
                            pFState->m_MaxActualLatencyToF0,
                            pFState->m_MinActualLatencyToF0,
                            pFState->m_TransitionCountToF0);

                // Don't fail, this if FYI
            }
        }
    }

    if(State != NoApp && State != NoAppMonitorOff)
    {
        if(bExpectD3Transitions && !bD3Transitions)
        {
            WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Expected to see D3 component transitions but didn't"));
            tr = RESULT_FAIL;
        }

        if(bExpectMemoryRefreshTransitions && !bExpectMemoryRefreshTransitions)
        {
            WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Expected to see Memory Refresh component transitions but didn't"));
            tr = RESULT_FAIL;
        }

        // We don't check for Memory or Engine transitions because these components (or some of these components) might only have 1 FState in which case transitions are not going to happen
        // As long as we transitioned to D3 as expected we can be sure that all expected transitioned happened

        // Validate Adapter transition latency (this is latency reported by DXGK_POWER_COMPONENT_D3_TRANSITION and it covers time it takes to get adapter into D3 and back to D0)
        if(!pAdapter->m_DState.m_TransitionCountFromD3 && !pAdapter->m_DState.m_TransitionCountToD3)
        {
            WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Expected to see Device transitions to or from D3, but it didn't"));

            tr = RESULT_FAIL;
        }
        else if(!pAdapter->m_DState.m_TransitionCountFromD3 || !pAdapter->m_DState.m_TransitionCountToD3)
        {
            WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - INFO: Only saw one-way D3 transition: TransitionsToD3 %d, TransitionsFromD3 %d, Latency %I64u"),
                        pAdapter->m_DState.m_TransitionCountToD3,
                        pAdapter->m_DState.m_TransitionCountFromD3,
                        (pAdapter->m_DState.m_TransitionCountToD3) ? 
                        (pAdapter->m_DState.m_TotalActualLatencyToD3 / pAdapter->m_DState.m_TransitionCountToD3) : 
                        (pAdapter->m_DState.m_TotalActualLatencyFromD3 / pAdapter->m_DState.m_TransitionCountFromD3));
        }
        else
        {
            ULONGLONG MaxExpectedD3Latency = (ULONGLONG)((double)ReportedD3TransitionLatency * 1.1);	// 10% margine of error
            ULONGLONG MinExpectedD3Latency = (ULONGLONG)((double)ReportedD3TransitionLatency * 0.9);	// 10% margine of error
            ULONGLONG ActualLatencyFromD3 = pAdapter->m_DState.m_TotalActualLatencyFromD3 / pAdapter->m_DState.m_TransitionCountFromD3;
            ULONGLONG ActualLatencyToD3 = pAdapter->m_DState.m_TotalActualLatencyToD3 / pAdapter->m_DState.m_TransitionCountToD3;	

            if((ActualLatencyFromD3 + ActualLatencyToD3) > MaxExpectedD3Latency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - ERROR: Adapter D3 latency (%I64u) is greater than reported (%I64u). (Test detected %d transitions to D3 and %d transitions from D3)"),
                            (ActualLatencyFromD3 + ActualLatencyToD3),
                            ReportedD3TransitionLatency,
                            pAdapter->m_DState.m_TransitionCountToD3,
                            pAdapter->m_DState.m_TransitionCountFromD3);

                tr = RESULT_FAIL;
            }
            else if((ActualLatencyFromD3 + ActualLatencyToD3) < MinExpectedD3Latency)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - INFO: Adapter D3 latency (%I64u) is lower than reported (%I64u). (Test detected %d transitions to D3 and %d transitions from D3)"),
                            (ActualLatencyFromD3 + ActualLatencyToD3),
                            ReportedD3TransitionLatency,
                            pAdapter->m_DState.m_TransitionCountToD3,
                            pAdapter->m_DState.m_TransitionCountFromD3);
                
                // not failing here, this is FYI
            }
            else
            {
                WriteToLog(_T("CHybridPowerManagement::ValidateTransitionLatency() - FYI: Adapter D3 latency is %I64u; reported latency is %I64u. (Test detected %d transitions to D3 and %d transitions from D3)"),
                            (ActualLatencyFromD3 + ActualLatencyToD3),
                            ReportedD3TransitionLatency,
                            pAdapter->m_DState.m_TransitionCountToD3,
                            pAdapter->m_DState.m_TransitionCountFromD3);
            }
        }
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::ValidatePowerComponents()
{
    TEST_RESULT tr = RESULT_PASS;
    CAdapter* pAdapter = m_ETWMaster.GetDGpu();

    if(!pAdapter)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - No dGPU Found"));
        return RESULT_FAIL;
    }

    PowerComponentList::iterator it = pAdapter->m_PowerComponentList.begin();

    // Validate D3 transition component
    UINT Count = 0;
    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {
        CGPUPowerComponent* pPC = *it;

        if(pPC->m_Type == DXGK_POWER_COMPONENT_D3_TRANSITION)
        {
            Count++;

            if(pPC->m_FStateList.size() > 2 || pPC->m_FStateList.size() < 1)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - D3 component has incorrect number of FStates, Index = %d, FStates = %d"),
                              pPC->m_Index, 
                              pPC->m_FStateList.size());

                tr = RESULT_FAIL;
            }

            if((pPC->m_FStateList.size() == 1) && (pPC->m_State == Idle))
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - D3 component with 1 FState is Idle, Index = %d"), pPC->m_Index);

                tr = RESULT_FAIL;
            }

            if(pPC->m_FStateList.size() == 2)
            {
                if(pPC->m_State == Active)
                {
                    // DxgKrnl will always keep D3 component with 2 FStates in Idle state, but IHV might make it active
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - INFO: D3 component with 2 FState is Active, Index = %d"), pPC->m_Index);
                }

                CFState* pFState = pPC->GetFState(1);

                if(!pFState)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - FState 1 is not found, Index = %d"), 
                               pPC->m_Index);

                    tr = RESULT_FAIL;
                }
                else if(pFState->m_ReportedLatency > LT_D3TRANSITION_MONITOROFF_NOCONTEXT)
                 {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - D3 component's reported latency is too large, Index = %d, MaxLatency = 0x%d, ReportedLatency = %I64u"), 
                               pPC->m_Index,
                               LT_D3TRANSITION_MONITOROFF_NOCONTEXT,
                               pFState->m_ReportedLatency);

                    tr = RESULT_FAIL;
                }
            }

        }
    }

    if(Count != 1)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - unexpected number of D3_Transition components found, expected = 1, found = %d"), Count);
        tr = RESULT_FAIL;
    }

    // Validate Memory components
    Count = 0;
    UINT MemSegCount = 0;
    UINT PreserveDuringStandbyCount = 0;
    UINT ActiveInD3Count = 0;

    MemorySegmentList::iterator itSeg = pAdapter->m_MemorySegmentList.begin();

    for(itSeg; itSeg != pAdapter->m_MemorySegmentList.end(); itSeg++)
    {
        CMemorySegment* pSeg = *itSeg;

        if(!pSeg->m_Flags.Aperture && !pSeg->m_Flags.ReservedSysMem)
        {
            MemSegCount++;

            if(pSeg->m_Flags.PreservedDuringStandby == 1)
                PreserveDuringStandbyCount++;

            if(!pSeg->m_pPC)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - no power component is associated with segment %d"), pSeg->m_ID);
                tr = RESULT_FAIL;
            }
        }
    }

    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {
        CGPUPowerComponent* pPC = *it;

        if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY)
        {
            Count++;

            UINT FStateCount = pPC->m_FStateList.size();

            if(PreserveDuringStandbyCount || pPC->m_Flags.ActiveInD3)
            {
                if(!pPC->m_Flags.ActiveInD3)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory component for self-refresh memory segment should have ActiveInD3 flag set, component index = %d"), pPC->m_Index);
                    tr = RESULT_FAIL;
                }
                else
                {
                    // Count power components with ActiveInD3 flag set - we'll later validate that
                    // if there are any ActiveInD3 components, MEMORY_REFRESH component is also present
                    ActiveInD3Count++;
                }

                if(FStateCount != 2)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory component for self-refresh memory segment should have only 2 FStates, found %d, component index = %d"), FStateCount, pPC->m_Index);
                    tr = RESULT_FAIL;
                }

                CFState* pFState = pPC->GetFState(1);

                if(!pFState)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - FState 1 is not found, Index = %d"), 
                               pPC->m_Index);

                    tr = RESULT_FAIL;
                }

                else if(pFState->m_ReportedLatency != 0)
                {
                    WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory component's reported latency is expected to be 0, Index = %d, ReportedLatency = %I64u"), 
                               pPC->m_Index,
                               pFState->m_ReportedLatency);

                    tr = RESULT_FAIL;
                }

            }
            else
            {
                // FYI - Memory components might have only 1 FState when self-refresh is not available
                if(FStateCount >= 2)
                {
                    CFState* pFState = pPC->GetFState(FStateCount - 1);

                    if(!pFState)
                    {
                        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - FState doesn't exist, FState = %d, Index = %d"), 
                                    FStateCount - 1,
                                    pPC->m_Index);

                        tr = RESULT_FAIL;
                    }

                    else if(pFState->m_ReportedLatency > LT_MEMORY_NOCONTEXT)
                    {
                        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory component's reported latency is too high, Index = %d, MaxLatency = %d, ReportedLatency = %I64u"), 
                                    pPC->m_Index,
                                    LT_MEMORY_NOCONTEXT,
                                    pFState->m_ReportedLatency);

                        tr = RESULT_FAIL;
                    }
                }
            }
        }
    }

    if(Count != MemSegCount)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - unexpected number of Memory components, expected = %d, found = %d"), MemSegCount, Count);
        tr = RESULT_FAIL;
    }

    // Validate DXGK_POWER_COMPONENT_MEMORY_REFRESH component
    Count = 0;

    for(it = pAdapter->m_PowerComponentList.begin(); it != pAdapter->m_PowerComponentList.end(); it++)
    {
        CGPUPowerComponent* pPC = *it;

        if(pPC->m_Type == DXGK_POWER_COMPONENT_MEMORY_REFRESH)
        {
            Count++;

            UINT FStateCount = pPC->m_FStateList.size();

            if(FStateCount != 2)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory_refresh component should have only 2 FStates, found %d"), FStateCount);
                tr = RESULT_FAIL;
            }

            CFState* pFState = pPC->GetFState(1);

            if(!pFState)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - FState 1 is not found, Index = %d"), 
                            pPC->m_Index);

                tr = RESULT_FAIL;
            }
            else if(pFState->m_ReportedLatency > LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT)
            {
                WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Memory_refresh component's reported latency is too high, Index = %d, ReportedLatency = %I64u, MaxLatency = %d"), 
                            pPC->m_Index,
                            pFState->m_ReportedLatency,
                            LT_MEMORYREFRESH_MONITOROFF_NOCONTEXT);

                tr = RESULT_FAIL;
            }
        }
    }

    if((PreserveDuringStandbyCount || ActiveInD3Count) && (Count != 1))
    {
        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Expect to find 1 Memory_refresh component, found %d"), Count);
        tr = RESULT_FAIL;
    }
    
    if(!PreserveDuringStandbyCount && !ActiveInD3Count && Count)
    {
        WriteToLog(_T("CHybridPowerManagement::ValidatePowerComponents() - Don't expect to find any Memory_refresh component, found %d"), Count);
        tr = RESULT_FAIL;
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::TurnMonitorsOff()
{
    //
    // Set the monitor idle timeout to 1 sec as a workaround for the OS implementation 
    // and the test design.
    // The test set monitor idle timeout in power settings and waits until the monitor is turned off
    // by calling nt!NtPowerInformation(SystemVideoState). NtPowerInformation returns a logical monitor state,
    // not the actual power state. So a monitor could be powered off, but the logical state could be "power on".
    // When the idle timeout is greater than 5 seconds, the OS calls win32kbase!UpdateDisplayState with the reason
    // MonitorRequestReasonGracePeriod. This does not change the monitor logical state, but changes the monitor
    // actual power state. Later the win32kbase!IdleTimerProc would send a SC_MONITORPOWER message to the 
    // foreground window. If the foreground app does not filter out the message, it will be translated to another 
    // win32kbase!UpdateDisplayState with the reason MonitorRequestReasonIdleTimeout. If the thread, which waits 
    // for the monitor off, is the foreground window thread, it will not process the SC_MONITORPOWER message.
    // So the test tried to switch focus away from the app window. But this did not work because of timing issues.
    // In the future we want to detect monitor off by calling QueryDisplaySettings. But for now set the monitor idle
    // timeout to 1 sec. This will remove the OS logic, described above, and switch the monitor logical state
    // immediately.
    //
    const UINT TimeSecToOff = 1;
    bool MonitorOff = false;
    TEST_RESULT tr = RESULT_PASS;
    

    if(!m_hMonitor)
        m_hMonitor = CreateMonitorContext();
    else
    {
        WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - WARNING!!! Monitor Context already exists"));
    }
            
    // try 3 times to turn monitor off, fail if not successful after 3 tries
    for(UINT i = 0; i < 3; i++)
    {
        WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - INFO: Turning monitor Off, iteration %d"), i);
        if(!GRFX::POWER::TurnMonitorsOff(TimeSecToOff,m_hMonitor))
        {
            WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - INFO: Failed to turn monitor off, iteration %d"), i);		
                    
            char* szPowerRequest = NULL;
            if(!GRFX::POWER::GetPowerRequestsString(&szPowerRequest))
            {
                WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - INFO: Failed to get power requests string."));
            }
            else
            {
                WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - INFO: Power requests: %s\n%S"), szPowerRequest, szPowerRequest);
                delete[] szPowerRequest;
            }
        }
        else
        {
            WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOff() - INFO:Successfully turned off monitor, iteration %d"), i);
            MonitorOff = true;
            break;
        }

        // Give some time between tries
        Sleep(10000);
    }

    if(MonitorOff == false)
    {
        WriteToLog(_T("ERROR: Failed to turn monitor off"));	
        tr = RESULT_FAIL;
    }

    return tr;
}

TEST_RESULT CHybridPowerManagement::TurnMonitorsOn()
{
    FocusAway();
    bool MonitorOff = false;
    TEST_RESULT tr = RESULT_PASS;
    
    if(!m_hMonitor)
    {
        WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOn() - ERROR!!! Monitor Context doesn't exists"));
        return RESULT_FAIL;
    }

    // try 3 times to turn monitor on, fail if not successful after 3 tries
    for(UINT i = 0; i < 3; i++)
    {
        WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOn() - INFO:Turning monitor ON, iteration %d"), i);
        if(!GRFX::POWER::TurnMonitorsOn(m_hMonitor))
        {
            WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOn() - INFO: Failed to turn monitor off, iteration %d"), i);					
        }
        else
        {
            WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOn() - INFO:Successfully turned monitor ON, iteration %d"), i);
            MonitorOff = false;
            break;
        }
    }

    if(MonitorOff == true)
    {
        WriteToLog(_T("CHybridPowerManagement::TurnMonitorsOn() - ERROR: Failed to turn monitor on"));	
        tr = RESULT_FAIL;
    }
    

    DestroyMonitorContext(m_hMonitor);
    m_hMonitor = NULL;

    return tr;
}

CHybridPowerManagementAppRunning::CHybridPowerManagementAppRunning()
    : m_pManagerSync(NULL)
{
}

CHybridPowerManagementAppRunning::~CHybridPowerManagementAppRunning()
{
    CleanupTestCase();
}


TEST_RESULT CHybridPowerManagementAppRunning::Setup()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    tr = CHybridPowerManagement::Setup();

    if(tr != RESULT_PASS)
        return tr;

    assert(!m_pManagerSync);

    m_pManagerSync = new CXProcSyncManager(HYBRID_BASIC_MAPPING_NAME, HYBRID_BASIC_TOTAL_SHARERS, sizeof(sHYBRID_PRESENT_SHARED_DATA));

    if(!m_pManagerSync)
    {
        WriteToLog(_T("Out of memory"));
        tr = RESULT_BLOCKED;
    }

    tr = CHybridPowerManagement::SetupTestCase();

    if(tr == RESULT_PASS)
    {
        if(m_pManagerSync->AcquireLock())
        {
            sHYBRID_PRESENT_SHARED_DATA *pSharedData = (sHYBRID_PRESENT_SHARED_DATA*)m_pManagerSync->GetExtraDataPtr();
            assert(pSharedData);

            pSharedData->pTest = (CDXGITest*)this;
            pSharedData->tr = RESULT_PASS;
            ZeroMemory(pSharedData->strError, sizeof(TCHAR) * ERROR_SIZE);

            m_pManagerSync->ReleaseLock();
        }
    }

    if(tr == RESULT_PASS)
    {
        m_ETWMaster.ProcessPowerEvents(true);
        m_ETWMaster.StartListening();

        tr = StartChildProcess(L"DXGIHybridPresent.exe -src:HW -featurelevel:9.1 -logclean -saveBMP -hybrid -NoETW -HybridPowerManagementChild -TimeToExecute:60000");

        if(tr != RESULT_PASS)
            goto MacroFailed;
                                 
        DBGPrint(L"Master is telling client to run\n");                                                   
        if(!m_pManagerSync->TransitionClientsTo(eMoveToRunning))                                            
        {                                                                                                         
            _stprintf_s(strError, ERROR_SIZE, _T("Manager waiting for client wasn't successful\n"));               
            tr = RESULT_BLOCKED;                                                                                     
            goto MacroFailed;                                                                                     
        }                                                                                                                                                                             
        OutputDebugString("Client has been told to run\n");                                                       
        MANAGER_CHECK_CLIENT_STATUS                                                                               
        MANAGER_SIGNAL

        // The App should now be running. Wait for a bit for dGPU to warm up
        Sleep(30000);

        if(m_bMonitorsOff)
        {
            tr = TurnMonitorsOff();

            if(tr != RESULT_PASS)
                goto MacroFailed;
        }

        // Drain events and update components
        if(!m_ETWMaster.PowerManagementValidate())
        {
            WriteToLog(_T("CHybridPowerManagementAppRunning::ExecuteTestCase() - PowerManagementValidate() failed"));
            tr = RESULT_FAIL;
            goto MacroFailed;
        }

        m_ETWMaster.StopListening();

        if(m_bMonitorsOff)
        {
            TurnMonitorsOn();
        }
    }

    MANAGER_SIGNAL_TO_TERMINATE

    return tr;

MacroFailed:
    WriteToLog(strError);
    m_ETWMaster.StopListening();
    TurnMonitorsOn();

    return tr;
}



void CHybridPowerManagementAppRunning::Cleanup()
{
    CHybridPowerManagement::Cleanup();

    if(m_pManagerSync)
    {
        delete m_pManagerSync;
        m_pManagerSync = NULL;
    }
}


TEST_RESULT CHybridPowerManagementAppRunning::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(AppRunning);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(AppRunning);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(AppRunning);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();


    return tr;
}


TEST_RESULT CHybridPowerManagement::StartChildProcess(WCHAR* strCommandLine)
{
    TEST_RESULT tr = RESULT_PASS;

    //create the child process
    STARTUPINFOW si;
    PROCESS_INFORMATION ProcInfo;

    WCHAR strExe[MAX_PATH];
    memset(strExe, 0, sizeof(WCHAR) * MAX_PATH);


    _snwprintf_s(strExe, MAX_PATH, MAX_PATH-1, L"%s", strCommandLine); 

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory(&ProcInfo, sizeof(ProcInfo));
    si.cb = sizeof(si);

    if( false == CreateProcessW(NULL, 
                            strExe, 
                            NULL, 
                            NULL, 
                            FALSE, 
                            0, 
                            NULL, 
                            NULL, 
                            &si, 
                            &ProcInfo ) ) 
    {

        WriteToLog(0, _T("CHybridPowerManagementAppRunning::StartChildProcess() - CreateProcess failed.") );
        tr = RESULT_BLOCKED;
    }
    else
    {
        CloseHandle(ProcInfo.hProcess);
        CloseHandle(ProcInfo.hThread);
    }

    return tr;
}


TEST_RESULT CHybridPowerManagement::StartChildProcessWithRestrictedPrivileges(WCHAR* strCommandLine)
{
    HANDLE hToken = NULL;
    HANDLE hDupeToken = NULL;
    PSID IntegrityLevelSid = NULL;
    TOKEN_MANDATORY_LABEL IntegrityLevelToken = {0};
    TCHAR sidValue[32] = {0};
    STARTUPINFOW si;
    PROCESS_INFORMATION ProcInfo;
    WCHAR strExe[MAX_PATH];

    TEST_RESULT tr = RESULT_PASS;

    if(!OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &hToken))
    {
        WriteToLog(_T("CHybridPowerManagement::StartChildProcessWithRestrictedPrivileges() - BLOCKED: Failed to open current process's token, error = %d"), GetLastError());
        tr = RESULT_BLOCKED;
    }

    if(tr == RESULT_PASS)
    {
        if(!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hDupeToken))
        {
            WriteToLog(_T("CHybridPowerManagement::StartChildProcessWithRestrictedPrivileges() - BLOCKED: Failed to duplicate current process's token, error = %d"), GetLastError());
            tr = RESULT_BLOCKED;
        }
    }
    
    if(tr == RESULT_PASS)
    {
        StringCchPrintf(sidValue, sizeof(sidValue), _T("S-1-16-%d"), SECURITY_MANDATORY_MEDIUM_RID);

        if(!ConvertStringSidToSid(sidValue, &IntegrityLevelSid))
        {
            WriteToLog(_T("CHybridPowerManagement::StartChildProcessWithRestrictedPrivileges() - BLOCKED: Failed to convert SID, error = %d"), GetLastError());
            tr = RESULT_BLOCKED;
        }
    }

    if(tr == RESULT_PASS)
    {
        IntegrityLevelToken.Label.Attributes = SE_GROUP_INTEGRITY;
        IntegrityLevelToken.Label.Sid = IntegrityLevelSid;

        if(!SetTokenInformation(hDupeToken, TokenIntegrityLevel, &IntegrityLevelToken, sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(IntegrityLevelSid)))
        {
            WriteToLog(_T("CHybridPowerManagement::StartChildProcessWithRestrictedPrivileges() - BLOCKED: Failed set token information, error = %d"), GetLastError());
            tr = RESULT_BLOCKED;
        }
    }


    //create the child process
    memset(strExe, 0, sizeof(WCHAR) * MAX_PATH);
    _snwprintf_s(strExe, MAX_PATH, MAX_PATH-1, L"%s", strCommandLine); 

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory(&ProcInfo, sizeof(ProcInfo));
    si.cb = sizeof(si);

    if( false == CreateProcessAsUserW(hDupeToken, 
                            NULL,
                            strExe, 
                            NULL, 
                            NULL, 
                            FALSE, 
                            0, 
                            NULL, 
                            NULL, 
                            &si, 
                            &ProcInfo ) ) 
    {

        WriteToLog(0, _T("CHybridPowerManagementAppRunning::StartChildProcessWithRestrictedPrivileges() - CreateProcessAsUser failed.") );
        tr = RESULT_BLOCKED;
    }
    else
    {
        CloseHandle(ProcInfo.hProcess);
        CloseHandle(ProcInfo.hThread);
    }

    LocalFree(IntegrityLevelSid);
    return tr;
}

TEST_RESULT CHybridPowerManagementAppIdle::Setup()
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    TCHAR strError[ERROR_SIZE];
    ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);

    tr = CHybridPowerManagement::Setup();

    if(tr != RESULT_PASS)
        return tr;

    assert(!m_pManagerSync);

    m_pManagerSync = new CXProcSyncManager(HYBRID_BASIC_MAPPING_NAME, HYBRID_BASIC_TOTAL_SHARERS, sizeof(sHYBRID_PRESENT_SHARED_DATA));

    if(!m_pManagerSync)
    {
        WriteToLog(_T("Out of memory"));
        tr = RESULT_BLOCKED;
    }

    tr = CHybridPowerManagement::SetupTestCase();

    if(tr == RESULT_PASS)
    {
        if(m_pManagerSync->AcquireLock())
        {
            sHYBRID_PRESENT_SHARED_DATA *pSharedData = (sHYBRID_PRESENT_SHARED_DATA*)m_pManagerSync->GetExtraDataPtr();
            assert(pSharedData);

            pSharedData->pTest = (CDXGITest*)this;
            pSharedData->tr = RESULT_PASS;
            ZeroMemory(pSharedData->strError, sizeof(TCHAR) * ERROR_SIZE);

            m_pManagerSync->ReleaseLock();
        }
    }

    if(tr == RESULT_PASS)
    {
        m_ETWMaster.ProcessPowerEvents(true);
        m_ETWMaster.StartListening();

        tr = StartChildProcess(L"DXGIHybridPresent.exe -src:HW -featurelevel:9.1 -logclean -saveBMP -hybrid -NoETW -HybridPowerManagementChildIdle -TimeToExecute:30000");

        DBGPrint(L"Master is telling client to run\n");                                                   
        if(!m_pManagerSync->TransitionClientsTo(eMoveToRunning))                                            
        {                                                                                                         
            _stprintf_s(strError, ERROR_SIZE, _T("Manager waiting for client wasn't successful\n"));               
            tr = RESULT_BLOCKED;                                                                                     
            goto MacroFailed;                                                                                     
        }                                                                                                                                                                             
        OutputDebugString("Client has been told to run\n");                                                       
        MANAGER_CHECK_CLIENT_STATUS 

        Sleep(20000); // App should become idle in this time   

        if(m_bMonitorsOff)
        {
            tr = TurnMonitorsOff();

            if(tr != RESULT_PASS)
                goto MacroFailed;

            Sleep(70000);
        }

        if(tr == RESULT_PASS)
        {
            // Drain events and update components
            if(!m_ETWMaster.PowerManagementValidate())
            {
                WriteToLog(_T("CHybridPowerManagementAppRunning::ExecuteTestCase() - PowerManagementValidate() failed"));
                tr = RESULT_FAIL;
                goto MacroFailed;
            }
        }

        m_ETWMaster.StopListening();

        if(m_bMonitorsOff)
            TurnMonitorsOn();

        MANAGER_SIGNAL_AND_WAIT

        MANAGER_SIGNAL_TO_TERMINATE
    }

    return tr;

MacroFailed:
    MANAGER_SIGNAL_TO_TERMINATE
    WriteToLog(strError);
    m_ETWMaster.StopListening();
    TurnMonitorsOn();

    return tr;
}

void CHybridPowerManagementAppIdle::Cleanup()
{
    CHybridPowerManagement::Cleanup();

    if(m_pManagerSync)
    {
        delete m_pManagerSync;
        m_pManagerSync = NULL;
    }
}

TEST_RESULT CHybridPowerManagementAppIdle::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(AppIdle);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(AppIdle);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(AppIdle);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();


    return tr;
}

TEST_RESULT CHybridPowerManagementNoAppMonitorOff::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(NoApp);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(NoApp);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(NoApp);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();

    return tr;
}


TEST_RESULT CHybridPowerManagementAppRunningMonitorOff::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(AppRunningMonitorOff);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(AppRunningMonitorOff);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(AppRunningMonitorOff);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();


    return tr;
}


TEST_RESULT CHybridPowerManagementAppIdleMonitorOff::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    if(m_ValidationType == VALIDATE_POWER_COMPONENTS)
        tr = ValidatePowerComponents();
    else if(m_ValidationType == VALIDATE_LATENCY_TOLERANCE && g_bValidateOS)
        tr = ValidateLatencyTolerance();
    else if(m_ValidationType == VALIDATE_TRANSITION_LATENCY)
        tr = ValidateTransitionLatency(AppIdleMonitorOff);
    else if(m_ValidationType == VALIDATE_ACTIVE_IDLE_STATUS)
        tr = ValidateActiveIdleStatus(AppIdleMonitorOff);
    else if(m_ValidationType == VALIDATE_DSTATE)
        tr = ValidateDState(AppIdleMonitorOff);
    else if(m_ValidationType == VALIDATE_DGPU_PROCESSES)
        tr = ValidateDGPUProcesses();


    return tr;
}




TEST_RESULT CHybridPowerManagementAppSuspend::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    TEST_RESULT trValidatePowerComponents = RESULT_PASS;
    TEST_RESULT trValidateLatencyTolerance = RESULT_PASS;
    TEST_RESULT trValidateTransitionLatency = RESULT_PASS;
    TEST_RESULT trValidateActiveIdleStatus = RESULT_PASS;
    TEST_RESULT trValidateDState = RESULT_PASS;
    TEST_RESULT trValidateDGPUProcesses = RESULT_PASS;
    HRESULT hr = S_OK;
    m_ETWMaster.ProcessPowerEvents(true);
    m_ETWMaster.StartListening();

    SyncEvent AppStarted;
    SyncEvent AppCompleted;
   
    hr = AppStarted.Create<DefaultSecurityDescriptor>(L"AppStartedEvent", false, false);

    if(FAILED(hr))
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::ExecuteTestCase() - Failed to create AppStarted event, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
        goto ExecuteTestCaseExit;
    }

    hr = AppCompleted.Create<DefaultSecurityDescriptor>(L"AppCompletedEvent", false, false);
    
    if(FAILED(hr))
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::ExecuteTestCase() - Failed to create AppCompleted event, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
        goto ExecuteTestCaseExit;
    }

    tr = StartChildProcessWithRestrictedPrivileges(L"TAEF\\TE.exe HybridPowerManagement_ScenarioApp_Modern.dll /p:hybrid");

    if(tr != RESULT_PASS || FAILED(AppStarted.WaitOne(5000)))
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::ExecuteTestCase() - Failed to start child process"));
        tr = RESULT_FAIL;
        goto ExecuteTestCaseExit;
    }

    // Suspend the application by pressing Windows Key (which will open Start screen)
    PressWindowsKey();

    // Sleep a bit for application to get suspended
    Sleep(20000);        

    // Drain events and update components
    if(!m_ETWMaster.PowerManagementValidate())
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::ExecuteTestCase() - PowerManagementValidate() failed"));
        tr = RESULT_FAIL;
        goto ExecuteTestCaseExit;
    }

    trValidatePowerComponents = ValidatePowerComponents();
    trValidateLatencyTolerance = ValidateLatencyTolerance();
    trValidateTransitionLatency = ValidateTransitionLatency(AppSuspend);
    trValidateActiveIdleStatus = ValidateActiveIdleStatus(AppSuspend);
    trValidateDState = ValidateDState(AppSuspend);
    trValidateDGPUProcesses = ValidateDGPUProcesses();

    if(trValidatePowerComponents != RESULT_PASS ||
       trValidateTransitionLatency != RESULT_PASS ||
       trValidateDState != RESULT_PASS ||
       trValidateDGPUProcesses != RESULT_PASS)
    {
        tr = RESULT_FAIL;
    }

    if(trValidateLatencyTolerance != RESULT_PASS ||
       trValidateActiveIdleStatus != RESULT_PASS)
    {
        if(g_bValidateOS)
            tr = RESULT_FAIL;

    }

ExecuteTestCaseExit:
    if(FAILED(AppCompleted.WaitOne(60000)))
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::ExecuteTestCase() - child process didn't complete"));
        
        if(tr == RESULT_PASS)
            tr = RESULT_BLOCKED;
    }
    m_ETWMaster.StopListening();
    return tr;
}


void CHybridPowerManagementAppSuspend::PressWindowsKey()
{
    WORD keyCombo[1] = {VK_LWIN};

    INPUT input[2] = {0};

    // Key down
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = keyCombo[0];
    
    // Key up
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    input[1].ki.wVk = keyCombo[0];
    
    if(2 != SendInput(2, input, sizeof(INPUT)))
    {
        WriteToLog(_T("CHybridPowerManagementAppSuspend::PressWindowsKey() - WARNING: SendInput failed to send 2 inputs"));
    }
}


TEST_RESULT CHybridPowerManagementAppResume::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    TEST_RESULT trValidatePowerComponents = RESULT_PASS;
    TEST_RESULT trValidateLatencyTolerance = RESULT_PASS;
    TEST_RESULT trValidateTransitionLatency = RESULT_PASS;
    TEST_RESULT trValidateActiveIdleStatus = RESULT_PASS;
    TEST_RESULT trValidateDState = RESULT_PASS;
    TEST_RESULT trValidateDGPUProcesses = RESULT_PASS;
    HRESULT hr = S_OK;
    m_ETWMaster.ProcessPowerEvents(true);
    m_ETWMaster.StartListening();

    SyncEvent AppStarted;
    SyncEvent AppCompleted;
      
    hr = AppStarted.Create<DefaultSecurityDescriptor>(L"AppStartedEvent", false, false);

    if(FAILED(hr))
    {
        WriteToLog(_T("CHybridPowerManagementAppResume::ExecuteTestCase() - Failed to create AppStarted event, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
        goto ExecuteTestCaseExit;
    }

    hr = AppCompleted.Create<DefaultSecurityDescriptor>(L"AppCompletedEvent", false, false);

    if(FAILED(hr))
    {
        WriteToLog(_T("CHybridPowerManagementAppResume::ExecuteTestCase() - Failed to create AppCompleted event, hr = 0x%X"), hr);
        tr = RESULT_BLOCKED;
        goto ExecuteTestCaseExit;
    }

    tr = StartChildProcessWithRestrictedPrivileges(L"TAEF\\TE.exe HybridPowerManagement_ScenarioApp_Modern.dll /p:hybrid");

    if(tr != RESULT_PASS || FAILED(AppStarted.WaitOne(5000)))
    {
        WriteToLog(_T("CHybridPowerManagementAppResume::ExecuteTestCase() - Failed to start child process"));
        tr = RESULT_FAIL;
        goto ExecuteTestCaseExit;
    }

    // Suspend the application by pressing Windows Key (which will open Start screen)
    PressWindowsKey();

    // Sleep a bit for application to get suspended
    Sleep(20000);

    // Wake up the application by starting it again
    tr = StartChildProcessWithRestrictedPrivileges(L"TAEF\\TE.exe HybridPowerManagement_ScenarioApp_Modern.dll /p:hybrid");

    if(tr != RESULT_PASS)
    {
        WriteToLog(_T("CHybridPowerManagementAppResume::ExecuteTestCase() - Failed to start child process"));
        tr = RESULT_FAIL;
        goto ExecuteTestCaseExit;
    }

    // Drain events and update components
    if(!m_ETWMaster.PowerManagementValidate())
    {
        WriteToLog(_T("CHybridPowerManagementAppRunning::ExecuteTestCase() - PowerManagementValidate() failed"));
        tr = RESULT_FAIL;
        goto ExecuteTestCaseExit;
    }
   
    trValidatePowerComponents = ValidatePowerComponents();
    trValidateLatencyTolerance = ValidateLatencyTolerance();
    trValidateTransitionLatency = ValidateTransitionLatency(AppResume);
    trValidateActiveIdleStatus = ValidateActiveIdleStatus(AppResume);
    trValidateDState = ValidateDState(AppResume);
    trValidateDGPUProcesses = ValidateDGPUProcesses();

    if(trValidatePowerComponents != RESULT_PASS ||
       trValidateTransitionLatency != RESULT_PASS ||
       trValidateDState != RESULT_PASS ||
       trValidateDGPUProcesses != RESULT_PASS)
    {
        tr = RESULT_FAIL;
    }

    if(trValidateLatencyTolerance != RESULT_PASS ||
       trValidateActiveIdleStatus != RESULT_PASS)
    {
        if(g_bValidateOS)
            tr = RESULT_FAIL;
    }

ExecuteTestCaseExit:
    if(FAILED(AppCompleted.WaitOne(60000)))
    {
        WriteToLog(_T("CHybridPowerManagementAppResume::ExecuteTestCase() - child process didn't complete"));
        
        if(tr == RESULT_PASS)
            tr = RESULT_BLOCKED;
    }
    m_ETWMaster.StopListening();
    return tr;
}