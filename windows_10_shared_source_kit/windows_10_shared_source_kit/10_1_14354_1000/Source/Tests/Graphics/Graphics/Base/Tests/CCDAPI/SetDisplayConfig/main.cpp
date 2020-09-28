#include "testrunner.h"
#include "SetDisplayConfig.h"

using namespace std;

const wchar_t * desc = L"This tests the behaviour of SetDisplayConfig.";

/* Prototypes */

/*
Execute the test
*/
void Go();

/*
Entry point
This calls Go()
*/
void __cdecl main()
{
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        cout << e.what();
    }
    catch(...)
    {
        cout << "Caught an exception at the highest level\n";
    }
    //This is the work around for bug win7 286479
    ChangeDisplaySettingsEx(0,0,0,0,0);
}


void PrintUsage()
{
    wcout << L"Includes test to validate the user32 SetDisplayConfig API call.\n\n"
        << L"SetDisplayConfig \n\n"
        << L"  DmmData                     Prints the source and target sets, so you can see what ids to pass into the \"settopology\" option\n"
        << L"  single                      Runs through single view\n"
        << L"  clone                       Runs through clone view\n"
        << L"  extend                      Runs through extended view\n"
        << L"  InternalTopology            Sets internal topology\n"
        << L"  ExternalTopology            Sets external topology\n"
        << L"  ExtendedTopology            Sets extended topology\n"
        << L"  CloneTopology               Sets clone topology\n"
        << L"  allTopology                 Runs through all topologies\n"
        << L"  SetModes                    Sets modes while setting topologies.\n"       
        << L"  rotation                    Runs through all rotate for all source modes\n"
        << L"  Scaling                     Runs through all scaling for all source modes\n"
        << L"  FlagValidate                Validates the various flag combinations\n"
        << L"  bvt                         Runs a bvt test\n"
        << L"  ForceProjection [UnitTest] [SetBootPersistence] [RemoveBootPersistence] [TestBootPersistenceOn] [TestBootPersistenceOff] [AdapterId:xxx] [TargetId:yyy] Boot persistence force projection\n"
        << L"  AllTopologyCombinations     Runs through all topology combinations\n"
        << L"  UseCcdLib                   Use the ccdlib for calling apis\n"
        << L"  pathpersistence             Tests the behaviour of Path Persistence.\n"
        << L"  savedispdiag                Logs dispdiag buffer information.\n"
        << L"  saveDump                    Use with the savedispdiag option to save dispdiag Dump Files.\n"
        << L"  UseDispDiagExe              Use dispdiag.exe to save .dat and .dmp files.\n"
        << L"  MultiMon                    Early outs the test if the system isn't multimon.\n";


}

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

    Runner.AddNode<CSetDisplayConfig>(L"SetDisplayConfig");

    UINT32 flags = 0;

    if( Runner.IsCmdLineSet(L"MultiMon") )
    {
        DISPLAY_DEVICE dd;
        memset( &dd, 0, sizeof(dd) );
        dd.cb = sizeof(dd);
        int count = 0;
        for( DWORD i = 0; EnumDisplayDevices( nullptr, i, &dd, 0 ); ++i )
        {
            if( dd.StateFlags & DISPLAY_DEVICE_ACTIVE )
            {
                ++count;
            }
        }

        if( count < 2 )
        {
            Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"MultiMonExit", true);
            // This config shouldn't be targeted.
            // This is a work around for the lack of constraints
            // in the Win8 WHCK kit.  The job has to run on all 
            // machines in hopes of locating a true multimon 
            // system.  This case earlies out all those machines
            // that are not multimon
        }
    }

    bool bDmmData = Runner.IsCmdLineSet(L"DmmData");

    bool bSingleView = Runner.IsCmdLineSet(L"Single");
    bool bCloneView = Runner.IsCmdLineSet(L"Clone");
    bool bExtendedView = Runner.IsCmdLineSet(L"extend");
    bool bInternalTopology = Runner.IsCmdLineSet(L"InternalTopology");
    bool bExternalTopology = Runner.IsCmdLineSet(L"ExternalTopology");
    bool bExtendedTopology = Runner.IsCmdLineSet(L"ExtendedTopology");
    bool bCloneTopology = Runner.IsCmdLineSet(L"CloneTopology");
    bool bAllTopology = Runner.IsCmdLineSet(L"AllTopology");

    bool bSetModes = Runner.IsCmdLineSet(L"SetModes");
    bool bRotation = Runner.IsCmdLineSet(L"Rotation");
    bool bScaling = Runner.IsCmdLineSet(L"Scaling");

    bool bFlagValidate = Runner.IsCmdLineSet(L"FlagValidate");
    bool bBVT = Runner.IsCmdLineSet(L"bvt");

    bool bForceProjection = Runner.IsCmdLineSet(L"ForceProjection");
    bool bUnitTest = Runner.IsCmdLineSet(L"UnitTest");
    bool bSetBootPersistence = Runner.IsCmdLineSet(L"SetBootPersistence");
    bool bRemoveBootPersistence = Runner.IsCmdLineSet(L"RemoveBootPersistence");
    bool bTestBootPersistenceOn = Runner.IsCmdLineSet(L"TestBootPersistenceOn");
    bool bTestBootPersistenceOff = Runner.IsCmdLineSet(L"TestBootPersistenceOff");
    bool bAdapterId = Runner.IsCmdLineSet(L"AdapterId");
    bool bTargetId = Runner.IsCmdLineSet(L"TargetId");

    bool bAllTopologyCombinations = Runner.IsCmdLineSet(L"AllTopologyCombinations");

    bool bUseCcdLib = Runner.IsCmdLineSet(L"UseCcdLib");

    bool bPathPersistence = Runner.IsCmdLineSet(L"pathpersistence");
    bool bSavedispdiag = Runner.IsCmdLineSet(L"savedispdiag");
    bool bSaveDump = Runner.IsCmdLineSet(L"saveDump");
    bool bUseDispDiagExe = Runner.IsCmdLineSet(L"UseDispDiagExe");

    Runner.DescribeCommandLineUsage( L"AllowedBPPFormats", L"Limit the modes tested to just the provided comma seprated list of DISPLAYCONFIG_PIXELFORMATs" );
    if( Runner.IsCmdLineSet(L"AllowedBPPFormats") )
    {
        std::vector<std::wstring> const& values( Runner.GetCmdLineValues( L"AllowedBPPFormats" ) );
        std::wstringstream wss;
        std::copy( values.begin(), values.end(), std::ostream_iterator<std::wstring, wchar_t>( wss, L"," ) );
        Runner.SetTestData( L"AllowedBPPFormats", wss.str().c_str() );
    }

    Runner.DescribeCommandLineUsage( L"-debugbreak", L"Indicate that the test should break into the debugger when a failure occurs." );
    if( Runner.IsCmdLineSet(L"-debugbreak") )
    {
        Runner.SetParamTestDomainSingleValue<bool>( L"SetDisplayConfig", L"debugbreak", true );
    }

    if(!bDmmData && !bSingleView && !bCloneView && !bExtendedView &&
        !bInternalTopology && !bExternalTopology && !bExtendedTopology && !bCloneTopology &&  !bAllTopology  &&
        !bSetModes && !bRotation && !bScaling && 
        !bFlagValidate && !bBVT && 
        !bForceProjection &&
        !bAllTopologyCombinations && 
        !bUseCcdLib &&
        !bPathPersistence)
    {
        PrintUsage();
        return;
    }		

    if(bUseCcdLib)
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"UseCcdLib", true);
    }

    if(bBVT)
    {
        flags = flags | CCDTEST_TOPOLOGY_ALL | CCDTEST_MODES_BVT | CCDTEST_LIMIT_PATHS; //change it to CCDTEST_MODES_FEW, when it starts working.

        //InternalTopology
        flags = flags | DISPLAYCONFIG_TOPOLOGY_INTERNAL;

        //ExternalTopology
        flags = flags | DISPLAYCONFIG_TOPOLOGY_EXTERNAL;

        //ExtendedTopology
        flags = flags | DISPLAYCONFIG_TOPOLOGY_EXTEND;

        //CloneTopology
        flags = flags | DISPLAYCONFIG_TOPOLOGY_CLONE;

        // test each path provided by QDC which have an available target
        flags |= CCDTEST_ALL_PATHS;

        // test low bpp mode on win8
        flags |= CCDTEST_LOW_BPP_MODE;

        //FlagValidate
        Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"FlagValidate", true);

    }
    else
    {
        if(bDmmData)
        {
            Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"DmmData", bDmmData);
        }
        if(bSingleView)
        {
            flags = flags | CCDTEST_TOPOLOGY_SINGLE_ONLY;
        }
        if(bCloneView)
        {
            flags = flags | CCDTEST_TOPOLOGY_CLONE_ONLY;
        }
        if(bExtendedView)
        {
            flags = flags | CCDTEST_TOPOLOGY_EXTEND_ONLY;
        }
        if(bAllTopology) //covers all the above views.
        {
            flags = flags | CCDTEST_TOPOLOGY_ALL;
        } 	

        if(bSetModes)
        {
            flags = flags | CCDTEST_MODES_ALL;
        }
        if(bRotation)
        {
            flags = flags | CCDTEST_MODES_ROTATED;
        }
        if(bScaling)
        {
            flags = flags | CCDTEST_MODES_SCALED;
        }        

        if(!bSetModes && !bRotation && !bScaling)
        {
            flags = flags | CCDTEST_MODES_NONE;
        }

        if(bInternalTopology)
        {
            flags = flags | DISPLAYCONFIG_TOPOLOGY_INTERNAL;
        }	
        if(bExternalTopology)
        {
            flags = flags | DISPLAYCONFIG_TOPOLOGY_EXTERNAL;
        }
        if(bExtendedTopology)
        {
            flags = flags | DISPLAYCONFIG_TOPOLOGY_EXTEND;
        }
        if(bCloneTopology)
        {
            flags = flags | DISPLAYCONFIG_TOPOLOGY_CLONE;
        }

        if(bFlagValidate)
        {
            Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"FlagValidate", true);
        }

        if(bForceProjection)
        {
            Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"ForceProjection", true);

            if(bUnitTest)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"UnitTest", true);
            }
            if(bSetBootPersistence)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"SetBootPersistence", true);
            }
            if(bRemoveBootPersistence)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"RemoveBootPersistence", true);
            }
            if(bTestBootPersistenceOn)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"TestBootPersistenceOn", true);
            }
            if(bTestBootPersistenceOff)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"TestBootPersistenceOff", true);
            }
            if(bAdapterId)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"AdapterId", true);
                //Also get value of adapter id here.
            }
            if(bTargetId)
            {
                Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"TargetId", true);
                //Also get value of target id here.
            }
        }

        if(bPathPersistence)
        {
            Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"PathPersistence", true);
        }		

    }

    if(bSavedispdiag)
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"bSavedispdiag", true);
    }

    if(bSaveDump)
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"bSaveDump", true);
    }


    if(bAllTopologyCombinations)
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"SetDisplayConfig", L"AllTopologyCombinations", true);
    }

    Runner.SetParamTestDomainSingleValue<UINT32>(L"SetDisplayConfig", L"flags", flags);

    /*
    Now that we have set up the tests, Execute the test cases
    */
    Runner.Run();
}

