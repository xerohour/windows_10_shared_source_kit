/******************************Module*Header*******************************
* Module Name: schsecurity.cpp
*
* Implementation of a test that verifies that apps must have proper
* permissions to set GPU base priority
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "schsecurity.h"
#include "i3d.h"

CSchSecurity::CSchSecurity()
{
    SetDescription(L"This component verifies that the OS properly checks permissions when apps try to set their base GPU priority");
}

struct SecTestCase
{
    D3DKMT_SCHEDULINGPRIORITYCLASS Priority;
    bool bPrivilegeRequired;
};

const SecTestCase TEST_CASES[] = 
{
    { D3DKMT_SCHEDULINGPRIORITYCLASS_IDLE, false },
    { D3DKMT_SCHEDULINGPRIORITYCLASS_BELOW_NORMAL, false },
    { D3DKMT_SCHEDULINGPRIORITYCLASS_NORMAL, false },
    { D3DKMT_SCHEDULINGPRIORITYCLASS_ABOVE_NORMAL, false },
    { D3DKMT_SCHEDULINGPRIORITYCLASS_HIGH, false },
    { D3DKMT_SCHEDULINGPRIORITYCLASS_REALTIME, true },
};
const size_t TEST_CASE_COUNT = sizeof(TEST_CASES) / sizeof(TEST_CASES[0]);


void CSchSecurity::PreWalk(CLogWriter* pLog)
{
    HANDLE hProcess = GetCurrentProcess();

    //disable SetPriority privilege
    if(!I3DSetPrivilege(I3D_REALTIME_PRIVILEGE, false))
    {
        throw std::exception("This test must be run with admin privileges");
    }

    D3DKMT_SCHEDULINGPRIORITYCLASS CurrentPriority = D3DKMT_SCHEDULINGPRIORITYCLASS_IDLE;
    if(FAILED(I3DGetProcessGPUPriority(hProcess, &CurrentPriority)))
    {
        throw std::exception("Failed to get process GPU priority");
    }
    

    for(size_t i = 0; i < TEST_CASE_COUNT; i++)
    {
        const SecTestCase& Case = TEST_CASES[i];
        
        HRESULT hr = I3DSetProcessGPUPriority(hProcess, Case.Priority);
        if(Case.bPrivilegeRequired && SUCCEEDED(hr))
        {
            throw std::exception("Process was allowed to set a high priority even though it didn't have the SE_INC_BAES_PRIORTIY privilege");
        }

        if((!Case.bPrivilegeRequired) && FAILED(hr))
        {
            throw std::exception("Process was not allowed to set a low priority even though it should be allowed to with no privleges");
        }

        D3DKMT_SCHEDULINGPRIORITYCLASS ReportedPriority = D3DKMT_SCHEDULINGPRIORITYCLASS_IDLE;
        if(FAILED(I3DGetProcessGPUPriority(hProcess, &ReportedPriority)))
        {
            throw std::exception("Failed to get process GPU priority");
        }

        D3DKMT_SCHEDULINGPRIORITYCLASS ExpectedPriority;
        if(SUCCEEDED(hr))
        {
            ExpectedPriority = Case.Priority;
        }
        else
        {
            ExpectedPriority = CurrentPriority;
        }
        
       
        if(ReportedPriority != ExpectedPriority)
        {
            throw std::exception("Returned base GPU priority != set base GPU priority");
        }

        CurrentPriority = ReportedPriority;
    }

    //enable SetPriority privilege
    if(!I3DSetPrivilege(I3D_REALTIME_PRIVILEGE, true))
    {
        throw std::exception("This test must be run with admin privileges, failed to enable SE_INC_BASE_PRIORITY privlege");
    }

    for(size_t i = 0; i < TEST_CASE_COUNT; i++)
    {
        const SecTestCase& Case = TEST_CASES[i];
        
        HRESULT hr = I3DSetProcessGPUPriority(hProcess, Case.Priority);
        if(FAILED(hr))
        {
            throw std::exception("Process was not allowed to set its GPU priority, even though it has SE_INC_BASE_PRIORITY privilege");
        }

        D3DKMT_SCHEDULINGPRIORITYCLASS ReportedPriority = D3DKMT_SCHEDULINGPRIORITYCLASS_IDLE;
        if(FAILED(I3DGetProcessGPUPriority(hProcess, &ReportedPriority)))
        {
            throw std::exception("Failed to get GPU process priority");
        }

        if(ReportedPriority != Case.Priority)
        {
            throw std::exception("Reported priority != set priority");
        }
    }
}

void CSchSecurity::PostWalk(CLogWriter* pLog)
{
    
}

