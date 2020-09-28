#include "testrunner.h"
#include "basevid.h"
#include "DisableTDRDebugBreak.h"
#include "generici3dcomponent.h"
#include "CRenderThread.h"

using namespace std;



//Prototypes
void DisplayUsage();
void  Go();


//This calls Go() and catches any exception thrown by it.
void __cdecl main()
{
    try
    {
        Go();
    }catch(std::exception& e)
    {
        cout<<e.what();
    }catch(...)
    {
        cout<<"Caught exception at highest level";
    }
}//end of main


class CWait : public CTestComponent
{
public:
    CWait()
    {
        AddParam<DWORD>(L"Timeout", L"value, in ms, this test component will wait in preWalk");
        SetParamTestDomainSingleValue<DWORD>(L"Timeout", 5000);
    }
    void PreWalk(CLogWriter* pLog)
    {
        DWORD dwTimeout = GetParam<DWORD>(L"Timeout");
        Sleep(dwTimeout);
    }
    void PostWalk(CLogWriter* pLog)
    {

    }
};

const wchar_t * DESC = L"Simulates a TDR.";
//Sets up the test
void Go()
{
    //The test runner contains all the test components.
    //It parses command line options, adds components, and executes them
    CTestRunner Runner;

    //First, tell the test runner the description for the test application (This will appear in spec)
    Runner.SetRootComponentDesc(DESC);

    //Check the command line for /? -? /help -help
    if(Runner.IsCmdLineSet(L"?") || 
        Runner.IsCmdLineSet(L"/?") || 
        Runner.IsCmdLineSet(L"-?") || 
        Runner.IsCmdLineSet(L"help") ||
        Runner.IsCmdLineSet(L"/help") ||
        Runner.IsCmdLineSet(L"-help"))
        {
            DisplayUsage();
            return;
        }

    //Next setup internal flag <bool> to tell us a valid command line option was specified
    bool bIsCmdLineSet = false;

    //Setup and configure Generic device Component 
    std::wstring nodeStr = L"Device";
    Runner.AddNode<CI3DDeviceComponent>(nodeStr.c_str());
    Runner.SetParamTestDomainSingleValue<bool>(nodeStr.c_str(), L"Windowed", true);
    SetupDeviceCommandLine(Runner, nodeStr.c_str());

    //Setup render Thread Component
    nodeStr += L"/Renderthread";
    Runner.AddNode<CRenderThread>(nodeStr.c_str());

    //Setup Wait component to allow Render Thread to 'warm up'
    nodeStr += L"/Wait";
    Runner.AddNode<CWait>(nodeStr.c_str());
    Runner.SetParamTestDomainSingleValue<DWORD>(nodeStr.c_str(), L"Timeout", 3000);

    //Setup Disable Debug Break component
    nodeStr += L"/DisableDebugBreak";
    Runner.AddNode<CDisableTDRDebugBreak>(nodeStr.c_str());

    if(!Runner.IsCmdLineSet(L"NoReg"))
    {
        //Setup TDR Registry test component
        nodeStr += L"/Reg";
        Runner.AddNode<CTDRRegistry>(nodeStr.c_str());
    }
    
    if(Runner.IsCmdLineSet(L"ForceVSyncTDR") || 
        Runner.IsCmdLineSet(L"ForceVSync") || 
        Runner.IsCmdLineSet(L"VSync") || 
        Runner.IsCmdLineSet(L"VSyncTDR"))
        {
            bIsCmdLineSet = true;
            std::wcout << L"Added VSyncTDR \n";

            //Setup VSync TDR Test Component
            nodeStr += L"/ForceVSyncTdr";
            Runner.AddNode<CSimulateVSyncTDR> (nodeStr.c_str());
        }
    
    if(Runner.IsCmdLineSet(L"ForceGPUTDR") || 
        Runner.IsCmdLineSet(L"ForcePreemptTDR") || 
        Runner.IsCmdLineSet(L"ForceGPU") || 
        Runner.IsCmdLineSet(L"ForcePreempt") || 
        Runner.IsCmdLineSet(L"PreemptTDR") ||
        Runner.IsCmdLineSet(L"GPUTDR") ||
        Runner.IsCmdLineSet(L"Preempt") ||
        Runner.IsCmdLineSet(L"GPU"))
        {
            bIsCmdLineSet = true;
            std::wcout << L"Added PreemptTDR \n";
             //Setup Preempt TDR Test Component
            nodeStr += L"/ForcePreemptTdr";
            Runner.AddNode<CSimulatePreemptTDR> (nodeStr.c_str());
        }
    
    if(Runner.IsCmdLineSet(L"ForceTDR") ||
        Runner.IsCmdLineSet(L"Force") ||
        Runner.IsCmdLineSet(L"TDR"))
        {
            bIsCmdLineSet = true;
            std::wcout << L"Added ForceTDR \n";

            //Setup Force TDR test Component
            nodeStr += L"/ForceTdr";
            Runner.AddNode<CSimulateTDR>         (nodeStr.c_str());
        }

   if(!bIsCmdLineSet)
    {
        //Setup Force TDR Test Component (default)
        nodeStr += L"/ForceTdr";
        Runner.AddNode<CSimulatePreemptTDR>         (nodeStr.c_str());
    }

    //Now that we have setup our test, execute it
    Runner.Run();

}

    
void DisplayUsage()
{
std::wcout << L"===SimulateTDR Usage=== \n\n";
std::wcout << L"SIMULATE VSYNC TYPE TDR\n";
std::wcout << L"cmdline:[ForceVSyncTDR] | [ForceVSync] | [VSyncTDR] | [VSync] \n\n";
std::wcout << L"SIMULATE PREEMPTION (GPU) TYPE TDR\n";
std::wcout << L"cmdline:[ForcePreemptTDR] | [ForcePreempt] | [PreemptTDR] | [Preempt]  \n";
std::wcout << L"cmdline:[ForceGPUTDR] | [ForceGPU] | [GPUTDR] | [GPU] \n\n";
std::wcout << L"SIMULATE FORCED TYPE TDR\n";
std::wcout << L"cmdline:[no command line] | [ForceTDR] | [Force] | [TDR]\n\n";
std::wcout << L"===End SimulateTDR Usage=== \n";
}

