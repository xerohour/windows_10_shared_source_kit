/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPowerManagement.h
//
//	Abstract:
//	This file contains declarations for HybridPowerManagement tests
//
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "HybridPresentETWValidation.h"
#include <dxgipresent.h>
#include <windows.h>
#include "dxgihybridpresent.h"
#include "TestApp.h"


extern bool g_bValidateOS;


#ifndef SAFE_CLOSE
#define SAFE_CLOSE(handle) \
    if(handle != 0) \
    { \
        CloseHandle(handle); \
        handle = 0; \
    }
#endif


/////////////////////////////////////////////////////////////////////////
//		Tests
/////////////////////////////////////////////////////////////////////////

enum eHybridPowerManagementState
{
    NoApp = 0,
    AppRunning = 1,
    AppIdle = 2,
    AppSuspended = 3,
    NoAppMonitorOff = 4,
    AppRunningMonitorOff = 5,
    AppIdleMonitorOff = 6,
    AppSuspend = 7,
    AppResume = 8
};

enum HYBRID_PM_VALIDATIONS
{
    VALIDATE_POWER_COMPONENTS = 0,
    VALIDATE_LATENCY_TOLERANCE = 1,
    VALIDATE_TRANSITION_LATENCY = 2,
    VALIDATE_ACTIVE_IDLE_STATUS = 3,
    VALIDATE_DSTATE = 4,
    VALIDATE_DGPU_PROCESSES = 5
};
DECLARE_NAMED_VALUES(HYBRID_PM_VALIDATIONS);


class CHybridPowerManagement : public CDXGITest, public clsBasicWindowClass, public CTestDXGISurfaceUtils
{
public: 
    CHybridPowerManagement()
        : m_hAppStarted(NULL),
          m_hTerminateApp(NULL),
          m_hStartIdle(NULL),
          m_hStopIdle(NULL),
          m_hAppConfirm(NULL),
          m_bMonitorsOff(false),
          m_hMonitor(NULL)
    {}
    ~CHybridPowerManagement(){};

protected:
    virtual TEST_RESULT Setup();
    virtual void Cleanup();
    virtual void InitTestParameters();
    TEST_RESULT ValidatePowerComponents();
    TEST_RESULT ValidateLatencyTolerance();
    TEST_RESULT ValidateTransitionLatency(eHybridPowerManagementState State);
    TEST_RESULT ValidateActiveIdleStatus(eHybridPowerManagementState State);
    TEST_RESULT ValidateDState(eHybridPowerManagementState State);
    TEST_RESULT ValidateDGPUProcesses();

    TEST_RESULT TurnMonitorsOn();
    TEST_RESULT TurnMonitorsOff();
    TEST_RESULT StartChildProcess(WCHAR* strCommandLine);
    TEST_RESULT StartChildProcessWithRestrictedPrivileges(WCHAR* strCommandLine);

    TEST_RESULT FocusAway();

public:
    CHybridETWMaster* GetETWMaster(){return &m_ETWMaster;}

protected:
    HANDLE m_hAppStarted;
    HANDLE m_hTerminateApp;
    HANDLE m_hStartIdle;
    HANDLE m_hStopIdle;
    HANDLE m_hAppConfirm;
    HANDLE m_hMonitor;
    CHybridETWMaster m_ETWMaster;
    HYBRID_PM_VALIDATIONS m_ValidationType;
    bool m_bMonitorsOff;
};

class CHybridPowerManagementNoApp : public CHybridPowerManagement
{
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT ExecuteTestCase();
};

class CHybridPowerManagementAppRunning : public CHybridPowerManagement
{
public:
    CHybridPowerManagementAppRunning();
    ~CHybridPowerManagementAppRunning();
    virtual TEST_RESULT Setup();
    virtual void Cleanup();
    virtual TEST_RESULT ExecuteTestCase();
    CXProcSyncManager* m_pManagerSync;
};

class CHybridPowerManagementAppIdle : public CHybridPowerManagementAppRunning
{
public:
    virtual TEST_RESULT Setup();
    virtual void Cleanup();
    virtual TEST_RESULT ExecuteTestCase();
};

class CHybridPowerManagementNoAppMonitorOff : public CHybridPowerManagementNoApp
{
public:
    CHybridPowerManagementNoAppMonitorOff()
    {
        m_bMonitorsOff = true;
    }
    virtual TEST_RESULT ExecuteTestCase();
};

class CHybridPowerManagementAppRunningMonitorOff : public CHybridPowerManagementAppRunning
{
public:
    CHybridPowerManagementAppRunningMonitorOff()
    {
        m_bMonitorsOff = true;
    }
    virtual TEST_RESULT ExecuteTestCase();
};

class CHybridPowerManagementAppIdleMonitorOff : public CHybridPowerManagementAppIdle
{
public:
    CHybridPowerManagementAppIdleMonitorOff()
    {
        m_bMonitorsOff = true;
    }
    virtual TEST_RESULT ExecuteTestCase();
};

class CHybridPowerManagementAppSuspend : public CHybridPowerManagementAppRunning
{
public:
    virtual TEST_RESULT ExecuteTestCase();
    void PressWindowsKey();
};

class CHybridPowerManagementAppResume : public CHybridPowerManagementAppSuspend
{
public:
    virtual TEST_RESULT ExecuteTestCase();
};