/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Entry point for fpo_Check test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "testrunner.h"
#include "disabledwm.h"
#include "generatepnpstop.h"
#include "listener.h"

#include "commonmacros.h"
#include "basevid.h"

#include "dx9addwindow.h" // WindowThread_CreateDeviceEx

// default is to expect WDDM1.2 or higher  
bool gbWDDM1_2=true;
bool gbWDDM1_1=false;
bool gbDriverLayering=false;
bool gbInternal=false;
// If the user wants to debug each failure, they place "debug" on the command line
bool gbDebugStacks=false;
// If the user wants to test all drivers, they place "alldrivers" on the command line.
bool gbAllDrivers=false;

// This is the amount of time to run. This can be overwriten by the comamnd option 
// time:x where x is in seconds.
#define TIME_TO_PERFORM_WORK 800 // seconds

//
// This class handles performing the work of changing the display mode while the 
// main thread listens for stackwalk events.
//
class CStackTestCase
{
public:
    CStackTestCase(CLogWriter* _pLog,UINT _uiListenTimeMS);
    ~CStackTestCase();
    
    HRESULT Enable();
    HRESULT Disable();
    void ThreadFunc();
    HANDLE GetSignalEvent();

private:
    HANDLE     m_hThread;
    bool       m_bThreadShouldQuit;
    HANDLE     m_hEvent;
    CLogWriter* m_pLog;
    UINT       m_uiListenTimeMS;
};

DWORD WINAPI WorkerThreadProc(void* pParam);



//
// Worker thread performs mode switches
//

CStackTestCase::CStackTestCase(CLogWriter* _pLog,UINT _uiListenTimeMS)
{
    m_hThread           = NULL;
    m_bThreadShouldQuit = false;
    m_pLog = _pLog;
    m_uiListenTimeMS = _uiListenTimeMS;
    Enable();
}


CStackTestCase::~CStackTestCase()
{
    Disable();
}


HRESULT 
CStackTestCase::Enable()
{
    m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
    if(NULL == m_hEvent)
    {
        throw std::exception("Failed to create an event");
    }

    //
    // Create the worker thread
    //
    m_bThreadShouldQuit = false;
    
    m_hThread = CreateThread(NULL, 0, WorkerThreadProc, this, 0, NULL);
    if(NULL == m_hThread)
    {
        Disable();
        
        throw std::exception("Failed to create a thread");
    }

    return S_OK;
}


HRESULT 
CStackTestCase::Disable()
{
    if(NULL != m_hThread)
    {
        m_bThreadShouldQuit = true;
        WaitForSingleObject(m_hThread, INFINITE);

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    if( NULL != m_hEvent )
    {
        CloseHandle(m_hEvent);
    }
    
    return S_OK;
}


HANDLE 
CStackTestCase::GetSignalEvent()
{
    return m_hEvent;
}


//
// Run SetDXMode code here. We're going to run for about 30 seconds.
//
void CStackTestCase::ThreadFunc()
{
    WCHAR wszDbg[MAX_PATH];
#ifdef WANT_DBG
    OutputDebugString(L"Starting to work\n\r");
#endif

    m_pLog->Message(L"ThreadFunc: Start");
    // We need to allow events to drain before starting our work.
    Sleep(5000);

    m_pLog->Message(L"ThreadFunc: Starting test loop");
    DWORD dwStartTime = GetTickCount();

    //enumerate all outputs
    I3DDisplayVector Outputs;
    I3DGetDisplayInfo(Outputs);

    UINT iGetValidDeviceCreateOptionsFailures=0;
    UINT iPresents=0;

    do
    {
        m_pLog->Message(L"ThreadFunc: Top of test loop");
        for(UINT nHead = 0; nHead < Outputs.size(); nHead++)
        {
            StringCchPrintf(wszDbg,MAX_PATH,L"ThreadFunc: Outputs.size()=%d",Outputs.size());
            m_pLog->Message(wszDbg);

            const I3DDisplayInfo& HeadInfo = Outputs[nHead];

            CI3DPtr pI3D;
            if(FAILED(CreateI3D(&pI3D.p, HeadInfo.sName.c_str())))
            {
                std::wstring sError(L"Failed to create an adapter object for: ");
                sError += HeadInfo.sName;

                throw wexception(sError.c_str());
            }

            D3DKMT_DISPLAYMODE CurrentMode;
            H_CHECK(pI3D->GetCurrentDisplayMode(&CurrentMode));

            //Get the mode list
            I3DXModeList Modes;
            H_CHECK(I3DXGetModeList(pI3D, Modes));
            assert(Modes.size() > 1);

            //for each mode
            for(UINT nMode = 0; nMode < Modes.size(); nMode++)
            {
                const D3DKMT_DISPLAYMODE& Mode = Modes[nMode];

                DeviceCreateOptions Opts;       

                HRESULT hr = pI3D->GetValidDeviceCreateOptions(false,
                                                               GetDesktopWindow(),
                                                               0,
                                                               0,
                                                               1,
                                                               false,
                                                               true,
                                                               &Mode,
                                                               &Opts);
                if( !SUCCEEDED(hr) )
                {
                    iGetValidDeviceCreateOptionsFailures++;

                    // Maybe this machine doesn't support changing modes. if that is the case
                    // we're going to just go for the default and see if that works.
                    hr = pI3D->GetValidDeviceCreateOptions(false,
                                                           GetDesktopWindow(),
                                                           0,
                                                           0,
                                                           1,
                                                           false,
                                                           false,
                                                           NULL,
                                                           &Opts);
                }

                if(SUCCEEDED(hr))
                {

                    CI3DDevicePtr pDevice;

                    hr = pI3D->CreateDevice( &Opts,
                                             &pDevice.p);

                    if( SUCCEEDED(hr) )
                    {
                        // Present 6 times on this rotation.
                        for (int y=0;y<6;y++)
                        {
                            H_CHECK(pDevice->Clear(0xFF00FF11,
                                                    I3DXGetClearFlags(pDevice) | D3DCLEAR_COMPUTERECTS,
                                                    1.0f,
                                                    0,
                                                    0,
                                                    NULL));

                            H_CHECK(pDevice->Present());    

                            iPresents++;
                        }
                    }
                    else
                    {
                        //
                        // Only fail if the mode was validated against the monitor caps
                        //
                        if(Mode.Flags.ValidatedAgainstMonitorCaps)
                        {
                            std::wstring sError(L"Failed to set a display mode that was validated against monitor capabilities.  Use the following command line to reproduce just this test case: SetDXMode.exe");

                            sError += L" device:";
                            sError += HeadInfo.sName;

                            sError += L" width:";
                            sError += ToString<UINT>(Mode.Width);

                            sError += L" height:";
                            sError += ToString<UINT>(Mode.Height);

                            sError += L" format:";
                            sError += ToString<UINT>(Mode.Format);

                            sError += L" refreshnum:";
                            sError += ToString<UINT>(Mode.RefreshRate.Numerator);

                            sError += L" refreshden:";
                            sError += ToString<UINT>(Mode.RefreshRate.Denominator);

                            sError += L" scanlineordering:";
                            sError += ToString<UINT>(Mode.ScanLineOrdering);

                            sError += L" orientation:";
                            sError += ToString<UINT>(Mode.DisplayOrientation);

                            sError += L" fixedoutput:";
                            sError += ToString<UINT>(Mode.DisplayFixedOutput);

                            m_pLog->Message(sError.c_str());
                        }
                    }
                }
                else
                {
                    m_pLog->Message(L"ThreadFunc: GetValidDeviceCreateOptions failed! Sleeping 1/10th second");
                }

                // Here is our time check for 30 seconds.
                if( (GetTickCount() - dwStartTime) > m_uiListenTimeMS )
                {
                    m_pLog->Message(L"ThreadFunc: Terminating Worker thread because of time limit.");
                    break;
                }
            }
        }

    } while(( (GetTickCount() - dwStartTime) < m_uiListenTimeMS ));


    if( (GetTickCount() - dwStartTime) < m_uiListenTimeMS )
    {
        m_pLog->Message(L"ThreadFunc: Terminating Worker thread because work is done (time remaining).");
    }

#ifdef WANT_DBG
    OutputDebugString(L"Done working, Signalling\n\r");
#endif

    StringCchPrintf(wszDbg,MAX_PATH,L"ThreadFunc: iGetValidDeviceCreateOptionsFailures=%d, iPresents=%d",
                    iGetValidDeviceCreateOptionsFailures,iPresents);
    m_pLog->Message(wszDbg);
    m_pLog->Message(L"ThreadFunc: Test Loop Done. SetEvent");

    // This should stop our listener.
    SetEvent(m_hEvent);
}

//
// This is called after the thread is created.
//
DWORD WINAPI WorkerThreadProc(void* pParam)
{
    CStackTestCase* pDevice = (CStackTestCase*)pParam;
    pDevice->ThreadFunc();
    return 0;
}



//
// Standard test class here.
//
class CStackTest : public CTestComponent
{
public:
    CStackTest();
    
protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    CStackTestCase* m_pTestCase;
};


void Go()
{
    CTestRunner Runner;

    Runner.SetRootComponentDesc(L"This test validates that stackwalk events that contain video "
                                L"driver addresses can be traversed back to the originating OS Routine.");

    Runner.AddNode< CStackTest >(L"Execute");

 
    if( Runner.IsCmdLineSet(L"1.1") )
    {
        gbWDDM1_1 = true;
        gbWDDM1_2 = false;
    }
    if( Runner.IsCmdLineSet(L"1.2") )
    {
        gbWDDM1_1 = false;
        gbWDDM1_2 = true;
    }
    if( Runner.IsCmdLineSet(L"DriverLayering"))
        gbDriverLayering = true;
    if( Runner.IsCmdLineSet(L"Internal"))
        gbInternal = true;

    // If "debug" is on the command line, we'll allow for debugging each stack
    gbDebugStacks = Runner.IsCmdLineSet(L"debug");

    // Set to test all drivers rather then just the kernel mode sys driver and the UserModeDrivers
    // listed in the registry.
    gbAllDrivers = Runner.IsCmdLineSet(L"AllDrivers");

    Runner.LinkParamToCmdLine( L"Execute",
                               L"uListenTime",  // Variable name
                               L"Time",       // CommandLine string
                               L"Number of seconds to listen for events" );

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
        std::cout << e.what() << std::endl;;
    }
}

CStackTest::CStackTest()
{
    EtwProfileTrace(__FUNCTION__, true);
    SetDescription(L"This component processes stackwalk events to validate we can walk through driver code");

    AddParam<UINT>(L"uListenTime", L"Number of seconds to listen for events");
    SetParamTestDomainSingleValue<UINT>(L"uListenTime", TIME_TO_PERFORM_WORK);

    EtwProfileTrace(__FUNCTION__, false);
}


void 
CStackTest::PreWalk(CLogWriter* pLog)
{
    HRESULT hr;
    EtwProfileTrace(__FUNCTION__, true);

    MachineInfo Info;
    GatherMachineInfo(Info);
    LogMachineInfo(Info, pLog);

    // Convert seconds from the command line into milliseconds
    UINT _uiListenTimeMS = (GetParam<UINT>(L"uListenTime") * 1000);

    m_pTestCase = new CStackTestCase(pLog,_uiListenTimeMS);
    if(NULL == m_pTestCase)
    {
        throw std::bad_alloc();
    }

    hr = PerfinfoGuidEnabled(m_pTestCase->GetSignalEvent(),pLog);
    if( hr != ERROR_SUCCESS )
    {
        throw std::exception("Failed to setup for listening for events.");
    }

    EtwProfileTrace(__FUNCTION__, false);
}

void 
CStackTest::PostWalk(CLogWriter* pLog)
{    
    EtwProfileTrace(__FUNCTION__, true);
    delete m_pTestCase;
    EtwProfileTrace(__FUNCTION__, false);
}



