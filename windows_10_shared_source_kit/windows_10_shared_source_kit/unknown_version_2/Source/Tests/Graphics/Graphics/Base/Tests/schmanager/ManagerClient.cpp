#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>

#include "TestRunner.h"
#include "testcomponent.h"

#include "EventWatch.h"
#include "EventListening.h"

#include "BaseVid.h"


#include <CrossProcSharing.h>

using namespace std;

#include "ManagerClient.h"

//
// Because this is cross process sharing functionality, it is expected that one
// Manager class will be used by one process and X client classes will be used
// by other processes. For instance, There my be three apps in the collection that
// want to run together. One manager and two clients. In this case, the command lines
// may look something like:
//
// SchManager.exe Manager SyncWith:123sdf432 ProcCount:3 Windowed:true 
// SchManager.exe Client SyncWith:123sdf432 ProcCount:3 Windowed:true
// SchManager.exe Client SyncWith:123sdf432 ProcCount:3 Windowed:true
//
// Note that they will all have to have the same SyncWith string and same ProcCount.
//
// Also note that this is the unit test for this functionality.


//------------------------------------------------------------------------------
//
// Command line should look like:
//
// SchManager.exe Manager SyncWith:123sdf432 ProcCount:2 Windowed:true
//
CXProcManager::CXProcManager()
{
    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);
}

// It's expected that the last character will be \n.
void CXProcManager::LogMessage(WCHAR* _pMsg)
{
    OutputDebugString(_pMsg);
    OutputDebugString(L"\r");
    m_pLog->Message(_pMsg);
}


WCHAR* CXProcManager::GetState(STATE_TYPE _stateType)
{
    switch(_stateType)
    {
        case eStateUnknown:
            return L"eStateUnknown";
            break;
        case eStateTerminate:
            return L"eStateTerminate";
            break;
        case eStateRun:
            return L"eStateRun";
            break;
        case eStateReady:
            return L"eStateReady";
            break;
        case eMoveToRunning:
            return L"eMoveToRunning";
            break;
        case eMoveToTerminate:
            return L"eMoveToTerminate";
            break;
        case eMoveToReady:
            return L"eMoveToReady";
            break;
        default:
            return L"Unknown";
            break;
    }
}

void CXProcManager::DumpClientStates(CXProcSyncManager* _pxProc)
{
    // Loop over all the clients and dump their state
    ULONG ulNumberOfClients = _pxProc->GetClientCount();

    for(UINT ui=0;ui<ulNumberOfClients;ui++)
    {
        if( ui != _pxProc->GetIndex() )
        {
            WCHAR wszBuf[MAX_PATH];
            // we have a client to dump.
            WCHAR* pName = GetState(_pxProc->GetClientState(ui));
            StringCchPrintf(wszBuf,MAX_PATH,L"Manager: Current Client(%d) state %s.\n",ui, pName);
            LogMessage(wszBuf);
        }
    }
}



void CXProcManager::RunTest()
{
    std::string SyncKey = GetParam<std::string>(L"SyncKeyString");
    ULONG ulCount = GetParam<UINT>(L"CountOfProcesses");

    WCHAR wszBuf[MAX_PATH];
    StringCchPrintf(wszBuf,MAX_PATH,L"Manager: SyncKeyString=%S, CountOfProcesses=%d\n",(char*)SyncKey.c_str(),ulCount);
    LogMessage(wszBuf);

    try
    {
        // Create our client class
        CXProcSyncManager xProcSync((char*)SyncKey.c_str(),ulCount); 

        // now wait (or timeout) for the manager app.

        LogMessage(TEXT("Manager: Waiting for clients to register.\n"));

        STATE_TYPE stRequest = eMoveToRunning;
        int Loop=5;
        while( xProcSync.TransitionClientsTo(stRequest) )
        {
            DumpClientStates(&xProcSync);

            if( stRequest == eMoveToTerminate )
            {
                LogMessage(TEXT("Manager: Clients Requested to terminate.\n"));
                DumpClientStates(&xProcSync);
                break;
            }

            // Now wait for a little while as if we're measuring something...
            Sleep(5000);


            switch(stRequest)
            {
            case eMoveToRunning:
                stRequest = eMoveToReady;
                LogMessage(TEXT("Manager: Ok, tell Clients to eMoveToReady.\n"));
                break;
            case eMoveToReady:
                stRequest = eMoveToRunning;
                LogMessage(TEXT("Manager: Ok, tell Clients to eMoveToRunning.\n"));
                break;
            case eMoveToTerminate:
                PATH_TRAP(); // should never get here.
                LogMessage(TEXT("Manager: Ok, tell Clients to eMoveToTerminate.\n"));
                break;
            default:
                PATH_TRAP();
                break;
            }

            if( Loop == 0 )
            {
                stRequest = eMoveToTerminate;
            }

            Loop--;
        }

        LogMessage(TEXT("Manager: Done!\n"));
    }
    catch(...)
    {
        PATH_TRAP();
    }
}
void CXProcManager::EndTest()
{
}

//------------------------------------------------------------------------------
//
// Command line should look like:
//
// SchManager.exe Client SyncWith:123sdf432 ProcCount:2 Windowed:true
//
CXProcClient::CXProcClient()
{
    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);
}

// It's expected that the last character will be \n.
void CXProcClient::LogMessage(WCHAR* _pMsg)
{
    OutputDebugString(_pMsg);
    OutputDebugString(L"\r");
    m_pLog->Message(_pMsg);
}


void CXProcClient::RunTest()
{
    std::string SyncKey = GetParam<std::string>(L"SyncKeyString");
    ULONG ulCount = GetParam<UINT>(L"CountOfProcesses");

    WCHAR wszBuf[MAX_PATH];
    StringCchPrintf(wszBuf,MAX_PATH,L"Client: SyncKeyString=%S, CountOfProcesses=%d\n",(char*)SyncKey.c_str(),ulCount);
    LogMessage(wszBuf);

    // This app is expecting some number of clients to load. We will wait for them here.

    try
    {
        LogMessage(TEXT("Creating xProcSharing class!\n"));

        // Create our client class
        CXProcSyncClient xProcSync((char*)SyncKey.c_str(),ulCount); 

        // now wait (or timeout) for the manager app.
        STATE_TYPE stRequest = xProcSync.TransitionTo(eStateReady);

        //
        // The inner loop is the loop where the real 'work' occurs. Inside that loop,
        // we'll constantly check to see if the manager wants us to stop working. If not
        // we'll run for our allotted time. At that point, we're back in the outer loop.
        // That loop defines how many work loops need to be performed. Generally, if
        // the manager was unsuccesful at capturing what it needed, we need to do something
        // slightly different in order to generate what it's looking for. Thus, we perform
        // those configuration changes in the outer loop. The number of tries at configuration
        // changes is defined by the loop counter.
        //
        int iLoop=0;
        while( stRequest != eStateTerminate )
        {
            // perform our work for the manager

            LogMessage(TEXT("Client: Pausing waiting to transition to something!\n"));

            // every time through our loop we're going to see what the manager wants us to do.
            // It's ok to transition to the same state.
            stRequest = xProcSync.TransitionTo(xProcSync.CheckForRequest());

            // If we're asked to work, we work
            while( stRequest == eStateRun )
            {
                // Wait one second before checking again.
                LogMessage(TEXT("Client: Looping, but checking if loop should termiante\n"));
                Sleep(1000);

                // Always query to find out if the manager wants us to change state.
                stRequest = xProcSync.CheckForRequest();
            }

            Sleep(1000);

            iLoop++;
        }

        LogMessage(TEXT("Client: manager signaled, marking that we're done.\n"));

        // But because we want to test waitforsignal, we're going to wait here until we're signaled to
        // move on
        xProcSync.SetResult(S_OK);

        LogMessage(TEXT("Client: exiting.\n"));
    }
    catch(...)
    {
        PATH_TRAP();
    }


}
void CXProcClient::EndTest()
{
}


