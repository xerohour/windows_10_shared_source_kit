#include "testrunner.h"
#include "GeneratePnPStop.h"
#include "PnPStop.h"

using namespace std;

const wchar_t * desc = L"Generates PnP Stop in two different ways. It stops/starts the device. It also updates the driver.";

/* Prototypes */

/*
Execute the test
*/
void Go();

void print_usage(void)
{
    printf("Usage:\n");

    printf(" PnPStop StopDevice    - Stops the display device,\n");
    printf(" PnPStop StartDevice   - Starts the display device,\n");
    printf(" PnPStop RestartDevice [Linked]- Restarts the display device. \'Linked\' stops/starts each device in sequential order,\n");
    printf(" PnPStop UpdateDriver [INFPath:filename] - Updates the driver from the specified INF file. If not specified uses the INF of the currently installed driver.\n");
    printf(" PnPStop RollbackDriver - Rollback the driver.\n");
    printf(" PnPStop VerifyDriverDescription:DriverDescription - Verifies that the driver description is \'DriverDescription\'.\n");
}

/*
Entry point
This calls Go()
*/
void __cdecl main()
{
    try
    {
        Go();
    }catch(std::exception& e)
    {
        cout << e.what();
    }
    catch(...)
    {
        cout << "Caught an exception at the highest level\n";
    }
}

enum PnPStopCommandLineParams
{
    PnPStopDevice               = 0x01,
    PnPStartDevice              = 0x02,
    PnPRestartDevice            = 0x04,
    PnPUpdateDriver             = 0x08,
    PnPRollbackDriver           = 0x10,
    PnPUpdateDriverWithINFPath  = 0x20,
    PnPVerifyDriverDescription  = 0x40,
    PnPRestartLinkedDevice      = 0x80,
    PnPUpdateFullDeviceDriverWithINFPath	= 0x100,
    PnPRollBackMSBDA            = 0x200,
};

void Go()
{

    /*
    The test runner contains all the test components,
    It parses command line options, adds components, and executes them
    */
    CTestRunner Runner;

    /*
    First, tell the test runner the description for the test application
    (This will appear in the spec)
    */
    Runner.SetRootComponentDesc(desc);

    int CommandParams = 0x00;

    bool bUpdateDriver   = Runner.IsCmdLineSet(L"UpdateDriver");
    bool bINFPath        = Runner.IsCmdLineSet(L"INFPath");
    bool bRestartDevice  = Runner.IsCmdLineSet(L"RestartDevice");
    bool bLinked         = Runner.IsCmdLineSet(L"Linked");
    bool bFullDevice     = Runner.IsCmdLineSet(L"FullDevice");
    bool bRollbackMSBDA  = Runner.IsCmdLineSet(L"RollbackMSBDA");

    CommandParams +=  (Runner.IsCmdLineSet(L"StopDevice"))             ? PnPStopDevice               : 0x00;
    CommandParams +=  (Runner.IsCmdLineSet(L"StartDevice"))            ? PnPStartDevice              : 0x00;
    CommandParams +=  (bRestartDevice && (!bLinked))                   ? PnPRestartDevice            : 0x00;
    CommandParams +=  (bRestartDevice && bLinked)                      ? PnPRestartLinkedDevice      : 0x00;
    CommandParams +=  (bUpdateDriver && (!bINFPath))                   ? PnPUpdateDriver             : 0x00;
    CommandParams +=  (bUpdateDriver && bINFPath)                      ? PnPUpdateDriverWithINFPath  : 0x00;
    CommandParams +=  (Runner.IsCmdLineSet(L"RollbackDriver"))         ? PnPRollbackDriver           : 0x00;
    CommandParams +=  (Runner.IsCmdLineSet(L"VerifyDriverDescription"))? PnPVerifyDriverDescription  : 0x00;
    if(bUpdateDriver && bINFPath && bFullDevice)
        CommandParams = PnPUpdateFullDeviceDriverWithINFPath;
    if(bRollbackMSBDA)
        CommandParams = PnPRollBackMSBDA;

    Runner.AddNode<CPnPStopLogMachineInfo>(L"PnPStopLogMachineInfo");

    switch(CommandParams)
    {
    case PnPStopDevice:
        Runner.AddNode<CPnPStopDevice>(L"StopDevice");
        Runner.SetParamTestDomainSingleValue<bool>(L"StopDevice", L"Log", true);		
        break;
    case PnPStartDevice:
        Runner.AddNode<CPnPStartDevice>(L"StartDevice");
        Runner.SetParamTestDomainSingleValue<bool>(L"StartDevice", L"Log", true);
        break;
    case PnPRestartDevice:
        Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
        Runner.AddNode<CPnPRestartDevice>(L"CheckForStartedIHVDevice/RestartDevice");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/RestartDevice", L"Log", true);
        break;
    case PnPRestartLinkedDevice:
        Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
        Runner.AddNode<CPnPRestartLinkedDevice>(L"CheckForStartedIHVDevice/PnPRestartLinkedDevice");
        break;
    case PnPUpdateDriver:
        Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
        Runner.AddNode<CPnPUpdateDriver>(L"CheckForStartedIHVDevice/UpdateDriver");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/UpdateDriver", L"Log", true);
        break;
    case PnPUpdateDriverWithINFPath:
        {
            Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
            Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice", L"AllowBDASwitch", true);
            Runner.AddNode<CPnPUpdateDriver>(L"CheckForStartedIHVDevice/UpdateDriver");
            Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/UpdateDriver", L"Log", true);
            const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"INFPath");
            std::wstring sINFPath = Values[0];
            Runner.SetParamTestDomainSingleValue<std::wstring>(L"CheckForStartedIHVDevice/UpdateDriver", L"INFPath", sINFPath);
            break;
        }
    case PnPUpdateFullDeviceDriverWithINFPath:
        {
            Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
            Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice", L"AllowBDASwitch", true);
            Runner.AddNode<CPnPUpdateFullDeviceDriver>(L"CheckForStartedIHVDevice/UpdateFullDeviceDriver");
            Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/UpdateFullDeviceDriver", L"Log", true);
            const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"INFPath");
            std::wstring sINFPath = Values[0];
            Runner.SetParamTestDomainSingleValue<std::wstring>(L"CheckForStartedIHVDevice/UpdateFullDeviceDriver", L"INFPath", sINFPath);
            break;
        }
    case PnPRollbackDriver:
        Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice", L"AllowBDASwitch", true);
        Runner.AddNode<CPnPRollbackDriver>(L"CheckForStartedIHVDevice/RollbackDriver");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/RollbackDriver", L"Log", true);
        break;
    case PnPVerifyDriverDescription:
        {
            Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
            Runner.AddNode<CPnPStopDeviceParams>(L"CheckForStartedIHVDevice/DeviceParams");
            const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"VerifyDriverDescription");
            std::wstring sDriverDesc = Values[0];
            Runner.SetParamTestDomainSingleValue<std::wstring>(L"CheckForStartedIHVDevice/DeviceParams", L"DriverDesc", sDriverDesc);
            break;
        }
    case PnPRollBackMSBDA:
        Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice", L"AllowBDASwitch", true);
        Runner.AddNode<CPnPRollbackMSBDADriver>(L"CheckForStartedIHVDevice/RollbackMSBDADriver");
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/RollbackMSBDADriver", L"Log", true);
        break;
    default:
        print_usage();
        return;
    };

    /*
    Now that we have set up the tests, Execute the test cases
    */
    Runner.Run();
}

