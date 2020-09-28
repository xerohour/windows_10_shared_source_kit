/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Implementation of CDSReset Main test runner
*
* Author: Mai Dao [maithida]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#define __MAIN_CPP__

#include "basevid.h"
#include "SystemUtil.h"
#include "CDSReset.h"
#include "PDBCcdConfig.h"
#include "PDBSetTopology.h"
#include "PDBRestoreTopology.h"
#include "PdbMustPersistent.h"
#include "CDSReset.h"
#include "SDCReset.h"
#include "CDSDevMode.h"
#include "MonitorOnOff.h"
#include "ScreenSaverOnOff.h"
#include "SystemWakeTimer.h"


using namespace std;

const wchar_t* DESC = L"This component tests the persistent data base due to ChangeDisplaySettingEx( CDS_RESET )";


void Go()
{
    /*
      The test runner contains all the test components,
      It parses command line options, adds components, and executes them
    */
    CTestRunner Runner;

#define TEST_RESET 0x0001
#define TEST_MONITOR 0x0002
#define TEST_SLEEP_STATE 0x0004
#define TEST_SCREEN_SAVER 0x0008
#define	TEST_MONITOR_ONLY 0x0010
#define	TEST_SLEEP_ONLY 0x0010
#define TEST_ALL TEST_RESET|TEST_MONITOR|TEST_SLEEP_STATE


    UINT ResetTest = 0;
    Runner.DescribeCommandLineUsage(L"Reset",
                                L"Reset test only");
    if(Runner.IsCmdLineSet(L"Reset"))
    {
        ResetTest |= TEST_RESET;
    }
    Runner.DescribeCommandLineUsage(L"Monitor",
                                L"Monitor power test only");
    if(Runner.IsCmdLineSet(L"Monitor"))
    {
        ResetTest |= TEST_MONITOR;
    }
    Runner.DescribeCommandLineUsage(L"Sleep",
                                L"Sleep power test only");
    if(Runner.IsCmdLineSet(L"Sleep"))
    {
        ResetTest |= TEST_SLEEP_STATE;
    }
    Runner.DescribeCommandLineUsage(L"Saver",
                                L"Screen Saver test only");
    if(Runner.IsCmdLineSet(L"Saver"))
    {
        ResetTest |= TEST_SCREEN_SAVER;
    }

    Runner.DescribeCommandLineUsage(L"SleepCds",
                                L"Change DEVMODE in monitor sleep state");

    Runner.DescribeCommandLineUsage(L"Multimon",
                                L"WHCK param to run test only on multi-monitor systems");

    bool bSleepCds = false;
    if(Runner.IsCmdLineSet(L"SleepCds"))
    {
        bSleepCds = true;
    }

    if(Runner.IsCmdLineSet(L"MonitorOnly"))
    {
        ResetTest = TEST_MONITOR_ONLY;
    }

    if(Runner.IsCmdLineSet(L"SleepOnly"))
    {
        ResetTest = TEST_SLEEP_ONLY;
    }

    if(Runner.IsCmdLineSet(L"Multimon"))
    {
        if(SetDisplayConfig(0, NULL, 0, NULL, DISPLAYCONFIG_TOPOLOGY_CLONE|SDC_VALIDATE) != ERROR_SUCCESS)
        // Multimon job on single Mon system, should skip Persistent tests. Simply log number of sources and targets of the display adapters
        {
            Runner.AddNode<CAdapterInfo>(L"DisplayAdapterInfo");
            
            Runner.Run();
            
            return;
        }
    }

    if(!ResetTest)
    {
        // For now, The Monitor is not good yet
        ResetTest = TEST_RESET|TEST_SLEEP_STATE; 
    }

    /*
      First, tell the test runner the description for the test application
      (This will appear in the spec)
    */
    Runner.SetRootComponentDesc(DESC);
    
    if(ResetTest & TEST_MONITOR_ONLY)
    {
        Runner.AddNode<CMonitorOnOff>(     L"TurnMonitorOffOn");
        SetupMonitorTimeoutCommandLine(Runner, L"TurnMonitorOffOn");
    }
    else if(ResetTest & TEST_SLEEP_ONLY)
    {
        Runner.AddNode<CSystemSleepRTCWakeAlarm> (L"SystemSleepRTCWakeAlarm");
        SetSleepRTCWakeAlarmCommandLine(Runner,   L"SystemSleepRTCWakeAlarm");
    }
    else
    {
        Runner.AddNode<CSystemUtil>(L"SystemInfo");

        Runner.AddNode<CPDBRestoreConfiguration>(L"RestoreTopology");
        RestoreTopologyCommandLine(Runner, L"RestoreTopology");
    
        Runner.AddNode<CPDBSetTopology>         (L"RestoreTopology/SetTopology");
        Runner.SetCycleMethod(                   L"RestoreTopology/SetTopology", CTestRunner::ALL);
        SetTopologyCommandLine(Runner,           L"RestoreTopology/SetTopology");

        if(ResetTest & TEST_MONITOR)
        {
            if(bSleepCds)
            {
                Runner.AddNode<CCDSDevMode>       (L"RestoreTopology/SetTopology/MonitorDevModes");
                Runner.SetParamTestDomainSingleValue<bool>( 
                    L"RestoreTopology/SetTopology/MonitorDevModes", 
                    L"NoUpdate", true);
                SetupChangeDevModeCommandLine(Runner, L"RestoreTopology/SetTopology/MonitorDevModes");
            
                Runner.AddNode<CPdbMustPersistent>(L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent");
                Runner.SetParamTestDomainSingleValue<bool>( 
                    L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent", 
                    L"Persistent", false);
            
                Runner.AddNode<CMonitorOnOff>(     L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn");
                SetupMonitorTimeoutCommandLine(Runner, L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn");
            
                Runner.AddNode<CCDSDevMode>       (L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn/MonDevModes2");
                Runner.SetParamTestDomainSingleValue<bool>( 
                    L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn/MonDevModes2", 
                    L"NoUpdate", true);
                SetupChangeDevModeCommandLine(Runner, L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn/MonDevModes2");
            }
            else
            {

                Runner.AddNode<CCDSDevMode>       (L"RestoreTopology/SetTopology/MonitorDevModes");
                Runner.SetParamTestDomainSingleValue<bool>( 
                    L"RestoreTopology/SetTopology/MonitorDevModes", 
                    L"NoUpdate", false);
                SetupChangeDevModeCommandLine(Runner, L"RestoreTopology/SetTopology/MonitorDevModes");
            
                Runner.AddNode<CPdbMustPersistent>(L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent");
                Runner.SetParamTestDomainSingleValue<bool>( 
                    L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent", 
                    L"Persistent", true);

                // In this case VidSch will disable Vsync as a monitor comes back in ~ 15 sec
                Runner.AddNode<CMonitorOnOff>(     L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn");
                SetupMonitorTimeoutCommandLine(Runner, L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn");
                Runner.SetParamTestDomainSingleValue<UINT>(
                    L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOn",
                    L"TimeInOffState", 
                    15);


                if(ResetTest == TEST_MONITOR)
                {
                    //In this case VidSch won't disable Vsync as a monitor comes back in ~ 0.1 sec
                    Runner.AddNode<CMonitorOnOff>( L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOnVsyncOn");
                    Runner.SetParamTestDomainSingleValue<UINT>(
                        L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOnVsyncOn",
                        L"TimeOut", 
                        4);
                    Runner.SetParamTestDomainSingleValue<UINT>(
                        L"RestoreTopology/SetTopology/MonitorDevModes/MonOffOnPersistent/TurnMonitorOffOnVsyncOn",
                        L"TimeInOffState", 
                        0);
                }
            }
        }


        if(ResetTest & TEST_RESET)
        {
            Runner.AddNode<CCDSReset>         (L"RestoreTopology/SetTopology/SetPersistentDevModes");
            SetupDeviceModeCommandLine(Runner, L"RestoreTopology/SetTopology/SetPersistentDevModes");
        
            Runner.AddNode<CPdbMustPersistent>(L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset");
            Runner.SetParamTestDomainSingleValue<bool>( 
                L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset", 
                L"Persistent", true);
        
            Runner.AddNode<CSDCReset>         (L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode");
            SetupDeviceModeCommandLine(Runner, L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode");
            Runner.SetParamTestDomainSingleValue<UINT>( 
                L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode", 
                L"Width", 1024);
            Runner.SetParamTestDomainSingleValue<UINT>( 
                L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode", 
                L"Height", 768);
             Runner.SetParamTestDomainSingleValue<UINT>( 
                L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode", 
                L"Freq", 75);
              Runner.SetParamTestDomainSingleValue<UINT>( 
                L"RestoreTopology/SetTopology/SetPersistentDevModes/PersistentReset/SetActiveDevMode", 
                L"Rotation", 0);
      }

        if(ResetTest & TEST_SCREEN_SAVER)
        {
            Runner.AddNode<CCDSDevMode>       (L"RestoreTopology/SetTopology/SaverDevModes");
            SetupChangeDevModeCommandLine(Runner, L"RestoreTopology/SetTopology/SaverDevModes");
        
            Runner.AddNode<CPdbMustPersistent>(L"RestoreTopology/SetTopology/SaverDevModes/SaverPersistent");
            Runner.SetParamTestDomainSingleValue<bool>( 
                L"RestoreTopology/SetTopology/SaverDevModes/SaverPersistent", 
                L"Persistent", true);
        
            Runner.AddNode<CScreenSaverOnOff>(L"RestoreTopology/SetTopology/SaverDevModes/SaverPersistent/ScreenSaverOnOff");
        }

        if(ResetTest & TEST_SLEEP_STATE)
        {
            Runner.AddNode<CCDSDevMode>       (L"RestoreTopology/SetTopology/SleepDevModes");
            SetupChangeDevModeCommandLine(Runner, L"RestoreTopology/SetTopology/SleepDevModes");
            Runner.SetParamTestDomainSingleValue<bool>( 
                L"RestoreTopology/SetTopology/SleepDevModes", 
                L"NoUpdate", false);
        
            Runner.AddNode<CPdbMustPersistent>(L"RestoreTopology/SetTopology/SleepDevModes/SleepPersistent");
            Runner.SetParamTestDomainSingleValue<bool>( 
                L"RestoreTopology/SetTopology/SleepDevModes/SleepPersistent", 
                L"Persistent", true);
        
            Runner.AddNode<CSystemSleepRTCWakeAlarm> (L"RestoreTopology/SetTopology/SleepDevModes/SleepPersistent/SystemSleepRTCWakeAlarm");
            SetSleepRTCWakeAlarmCommandLine(Runner,   L"RestoreTopology/SetTopology/SleepDevModes/SleepPersistent/SystemSleepRTCWakeAlarm");
        }
    }


    Runner.Run();
}



void __cdecl main()
{    
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        cout << e.what();

        //This is the work around for bug 286479
        ChangeDisplaySettingsEx(0,0,0,0,0);
    }

    CMonitorOnOff *pMonitor = new CMonitorOnOff();
    //This is the work around for bug 286479
    ChangeDisplaySettingsEx(0,0,0,0,0);
}

