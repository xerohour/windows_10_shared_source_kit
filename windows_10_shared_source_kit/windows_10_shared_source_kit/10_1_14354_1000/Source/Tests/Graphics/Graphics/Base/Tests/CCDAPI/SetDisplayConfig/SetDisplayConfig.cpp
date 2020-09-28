#include <Dwmapi.h>
#include "SetDisplayConfig.h"
#include "DisplayModeHelper.h"
#include "DisplayPathHelper.h"
#include "DispDiagLogWriter.h"
#include "stringutils.h"

using namespace DmmApi;
using namespace CCD;

#define NO_DISPLAYCONFIG_TOPOLOGY 0x00
//#define _DEBUG

CSetDisplayConfig::CSetDisplayConfig()
{
    AddParam<bool>(L"DmmData", L"Enumerate all paths.");
    SetParamTestDomainSingleValue<bool>(L"DmmData", false);

    AddParam<bool>(L"MultiMonExit", L"Exit if system doesn't support at least two displays.");
	SetParamTestDomainSingleValue<bool>( L"MultiMonExit", false );

    AddParam<bool>(L"FlagValidate", L"Tests all SDC flag combinatios.");
    SetParamTestDomainSingleValue<bool>(L"FlagValidate", false);

    AddParam<bool>(L"AllTopologyCombinations", L"Sets all topology combinatios.");
    SetParamTestDomainSingleValue<bool>(L"AllTopologyCombinations", false);

    AddParam<bool>(L"UseCcdLib", L"Use CCD Lib.");
    SetParamTestDomainSingleValue<bool>(L"UseCcdLib", false);    

    AddParam<bool>(L"ForceProjection", L"Set force projection.");
    SetParamTestDomainSingleValue<bool>(L"ForceProjection", false);
    AddParam<bool>(L"SetBootPersistence", L"Set boot persistence on.");
    SetParamTestDomainSingleValue<bool>(L"SetBootPersistence", false);    
    AddParam<bool>(L"RemoveBootPersistence", L"Set boot persistence off.");
    SetParamTestDomainSingleValue<bool>(L"RemoveBootPersistence", false);    
    AddParam<bool>(L"TestBootPersistenceOn", L"Check if boot persistence is on.");
    SetParamTestDomainSingleValue<bool>(L"TestBootPersistenceOn", false);    
    AddParam<bool>(L"TestBootPersistenceOff", L"Check if boot persistence is off.");
    SetParamTestDomainSingleValue<bool>(L"TestBootPersistenceOff", false);    
    AddParam<bool>(L"AdapterId", L"Specify the AdapterId.");
    SetParamTestDomainSingleValue<bool>(L"AdapterId", false);    
    AddParam<bool>(L"TargetId", L"Specify the TargetId.");
    SetParamTestDomainSingleValue<bool>(L"TargetId", false);

    AddParam<UINT32>(L"flags", L"Flags to run the test.");
    SetParamTestDomainSingleValue<UINT32>(L"flags", 0);        

    AddParam<bool>(L"PathPersistence", L"Test Path Persistence.");
    SetParamTestDomainSingleValue<bool>(L"PathPersistence", false);

    AddParam<bool>(L"bSavedispdiag", L"Log dispdiag buffer information.");
    SetParamTestDomainSingleValue<bool>(L"bSavedispdiag", false);

    AddParam<bool>(L"bSaveDump", L"Save DispDiag Dump file.");
    SetParamTestDomainSingleValue<bool>(L"bSaveDump", false);

    AddParam<bool>(L"bUseDispDiagExe", L"Use dispdiag.exe to save .dat and .dmp files");
    SetParamTestDomainSingleValue<bool>(L"bUseDispDiagExe", false);

    AddParam<bool>( L"debugbreak", L"Debug failures by breaking in." );
    SetParamTestDomainSingleValue<bool>( L"debugbreak", false );
}

void TestAllSinglePaths( CLogWriter* pLog, CcdTopology* pTopologyMGR )
{
    UINT32 numPaths = 0;
    UINT32 numModes = 0;
    DISPLAYCONFIG_PATH_INFO* pPaths = nullptr;
    DISPLAYCONFIG_MODE_INFO* pModes = nullptr;

    if( !pTopologyMGR->QueryDisplayConfig( QDC_ALL_PATHS, &numPaths, &pPaths, &numModes, &pModes, nullptr ) )
    {
        return;
    }

    // Test each path if the target is available.
    for( UINT32 i = 0; i < numPaths; ++i )
    {
        pLog->StartSubTest();

        pLog->Message( DisplayPathHelper::ToString( pPaths[i] ).c_str() );

        if( !pPaths[i].targetInfo.targetAvailable )
        {
            pLog->Skip( L"Skipping path which is unavailable.\n" );
            continue;
        }

        // cleanup the path
        pPaths[i].sourceInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
        pPaths[i].targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
        pPaths[i].targetInfo.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
        pPaths[i].targetInfo.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
        pPaths[i].targetInfo.refreshRate.Numerator = 0;
        pPaths[i].targetInfo.refreshRate.Denominator = 0;
        pPaths[i].targetInfo.scanLineOrdering = DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED;
        pPaths[i].flags = DISPLAYCONFIG_PATH_ACTIVE;

        // set the path
        if( pTopologyMGR->SetDisplayConfig(
                    1,
                    pPaths + i,
                    0,
                    nullptr,
                    SDC_USE_SUPPLIED_DISPLAY_CONFIG ) )
        {
            // Check that DWM remains on.
            {
                BOOL bIsDWMOn = FALSE;
                HRESULT hr = S_OK;
                Sleep(1000);
                for( int i = 0; i < 10 && !bIsDWMOn; ++i )
                {
                    hr = DwmIsCompositionEnabled( &bIsDWMOn );
                    if( FAILED(hr) )
                    {
                        pLog->Fail( 
                                FormatStringTS( L"Failed dwm is enabled check 0x%08X", hr ).c_str(),
                                __WFILE__,
                                __LINE__ );
                        break;
                    }

                    if( !bIsDWMOn )
                    {
                        pLog->Message( L"DWM is no longer enabled." );
                        // delay next check for 1 second
                        Sleep( 1000 );
                    }
                }

                if( !bIsDWMOn )
                {
                    pLog->Fail( L"DWM is no longer enabled and has not recovered after 10 seconds.", __WFILE__, __LINE__ );
                }
            }

            UINT32 pathCount = 0;
            UINT32 modeCount = 0;
            DISPLAYCONFIG_PATH_INFO* pPathArray = nullptr;
            DISPLAYCONFIG_MODE_INFO* pModeArray = nullptr;

            // verify that QDC and DMM agree
            if( pTopologyMGR->QueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &pathCount, &pPathArray, &modeCount, &pModeArray, nullptr ) )
            {
                delete [] pPathArray;
                delete [] pModeArray;
            }
        }
    }

    delete [] pPaths;
    delete [] pModes;
}

void TestLowBppMode( CLogWriter* pLog, CcdTopology* pTopologyMGR )
{
    pLog->StartSubTest();
    pLog->Message( L"Setting up for low bpp tests." );
    // first, set the internal topology
    if( !pTopologyMGR->SetDisplayConfig( 0, nullptr, 0, nullptr, SDC_TOPOLOGY_INTERNAL | SDC_PATH_PERSIST_IF_REQUIRED ) )
    {
        return;
    }

    UINT32 numPaths = 0;
    UINT32 numModes = 0;
    DISPLAYCONFIG_PATH_INFO* pPaths = nullptr;
    DISPLAYCONFIG_MODE_INFO* pModes = nullptr;

    if( !pTopologyMGR->QueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &numPaths, &pPaths, &numModes, &pModes, nullptr ) )
    {
        return;
    }

    if( pTopologyMGR->IsBDAorBDD( pPaths[0].sourceInfo.adapterId ) )
    {
        pPaths[0].targetInfo.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
        pPaths[0].targetInfo.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
        pPaths[0].targetInfo.refreshRate.Numerator = 0;
        pPaths[0].targetInfo.refreshRate.Denominator = 0;
        pPaths[0].targetInfo.scanLineOrdering = DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED;
    }

    DISPLAYCONFIG_PIXELFORMAT formats[] = 
    {
        DISPLAYCONFIG_PIXELFORMAT_8BPP,
        DISPLAYCONFIG_PIXELFORMAT_16BPP,
        DISPLAYCONFIG_PIXELFORMAT_24BPP
    };

    for( UINT32 i = 0; i < ARRAY_SIZE(formats); ++i )
    {
        pLog->StartSubTest();
        wchar_t buf[64] = {0};
        swprintf_s( buf, L"Trying low bpp mode test: %s", DisplayModeHelper::PixelFormatToString( formats[i] ).c_str() );
        pLog->Message( buf );
        pModes[ pPaths[0].sourceInfo.modeInfoIdx ].sourceMode.pixelFormat = formats[i];

        if( ERROR_INVALID_PARAMETER != ::SetDisplayConfig( numPaths, pPaths, numModes, pModes, SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG ) )
        {
            // run again to get logging for the failure
            pTopologyMGR->SetDisplayConfig( numPaths, pPaths, numModes, pModes, SDC_USE_SUPPLIED_DISPLAY_CONFIG );
        }
    }

    delete [] pPaths;
    delete [] pModes;
}

void CSetDisplayConfig::PreWalk(CLogWriter* pLog)
{
    // Ensure that we get dispdiag dumps when failures occur.
    DispDiagLogWriter logWriter( pLog );
    pLog = &logWriter;

    bool bDmmData = GetParam<bool>(L"DmmData");
    bool bMultiMonExit = GetParam<bool>(L"MultiMonExit");
    if( bMultiMonExit )
    {
        pLog->Skip( L"System not multimon capable." );
        return;
    }

    bool bFlagValidate = GetParam<bool>(L"FlagValidate");

    bool bForceProjection = GetParam<bool>(L"ForceProjection");    
    bool bSetBootPersistence = GetParam<bool>(L"SetBootPersistence");        
    bool bRemoveBootPersistence = GetParam<bool>(L"RemoveBootPersistence");    
    bool bTestBootPersistenceOn = GetParam<bool>(L"TestBootPersistenceOn");        
    bool bTestBootPersistenceOff = GetParam<bool>(L"TestBootPersistenceOff");    
    bool bAdapterId = GetParam<bool>(L"AdapterId");        
    bool bTargetId = GetParam<bool>(L"TargetId");        
    bool bSavedispdiag = GetParam<bool>(L"bSavedispdiag");
    bool bSaveDump = GetParam<bool>(L"bSaveDump");
    bool bUseDispDiagExe = GetParam<bool>(L"bUseDispDiagExe");

    bool bAllTopologyCombinations = GetParam<bool>(L"AllTopologyCombinations");    

    bool bUseCcdLib = GetParam<bool>(L"UseCcdLib");
    bool bDebugBreak = GetParam<bool>(L"debugbreak");

    UINT32 flags = GetParam<UINT32>(L"flags");

    bool bPathPersistence = GetParam<bool>(L"PathPersistence");

    MachineInfo Info;
    GatherMachineInfo(Info);
    LogMachineInfo(Info, pLog);

    if(bDmmData)
    {
        //Display source and target sets
        std::wcout << DmmStateProvider::AllAdapterInfoToString();
    }

    CcdTopology Topology(pLog, bUseCcdLib);
    Topology.m_bDebugBreak = bDebugBreak;

    {
        std::wstring value;
        LookupScriptTestData( L"AllowedBPPFormats", value );
        if( !value.empty() )
        {
            std::vector<std::wstring> values;
            {
                std::wstring::size_type strPos = 0;
                for( std::wstring::size_type strPos = 0u, strEndPos = value.size(); strPos < strEndPos; ++strPos )
                {
                    const std::wstring::size_type strFindPos = value.find_first_of( L',', strPos );
                    if( strPos != strFindPos )
                    {
                        if( strFindPos == std::wstring::npos )
                        {
                            values.push_back( value.substr( strPos ) );
                            break;
                        }
                        values.push_back( value.substr( strPos, strFindPos - strPos ) );
                    }
                    strPos = strFindPos;
                }
            }

            for( std::vector<std::wstring>::const_iterator vecStrIterator(values.begin()), vecEndPos(values.end()); vecStrIterator != vecEndPos; ++vecStrIterator )
            {
                const size_t count( sizeof(g_DisplayConfigPixelFormatList) / sizeof(g_DisplayConfigPixelFormatList[0]) );
                std::pair<DISPLAYCONFIG_PIXELFORMAT,std::wstring> const* pDispConfPixFormatIterator( std::find( g_DisplayConfigPixelFormatList, g_DisplayConfigPixelFormatList + count, *vecStrIterator ) );
                if( pDispConfPixFormatIterator != g_DisplayConfigPixelFormatList + count )
                {
                    Topology.m_AllowedFormats.insert( pDispConfPixFormatIterator->first );
                }
            }
        }
    }

#ifdef _DEBUG
    __debugbreak();
#endif 

    CDdInfo* pDdInfo = NULL;
    if (bUseDispDiagExe) {
        pDdInfo = new CDdInfo ( pLog );
        Topology.EnableSaveDispDiag(pDdInfo, 2);
    } else if(bSavedispdiag) {
        pDdInfo = new CDdInfo ( pLog );
        if (bSaveDump) {
            Topology.EnableSaveDispDiag(pDdInfo, 1);
        } else {
            Topology.EnableSaveDispDiag(pDdInfo);
        }
    }

    // The following test cases cover the win+p behavior of the API.  The outcomes of these tests cases
    // depend on the state of the current machine as well as what saved toplogies are already in the database.
    //
    // For this reason these cases should really be run more than once, with and without a clean ccd database.
    //
    if(((flags & DISPLAYCONFIG_TOPOLOGY_INTERNAL)  != 0)
        || (bPathPersistence == true))
    {
        pLog->StartSubTest();
        if(Topology.IsWinPTopologyCapable(DISPLAYCONFIG_TOPOLOGY_INTERNAL, true))
        {
            Topology.SetDisplayConfig(0, NULL, 0, NULL, DISPLAYCONFIG_TOPOLOGY_INTERNAL | SDC_PATH_PERSIST_IF_REQUIRED, true, false);

            if(bPathPersistence == true)
            {
                Topology.SetTopologyWithModes(16);
            }
        }
        else
        {
            pLog->Skip( L"Machine is not internal capable." );
        }
        flags &= ~DISPLAYCONFIG_TOPOLOGY_INTERNAL;
    }

    if(((flags & DISPLAYCONFIG_TOPOLOGY_EXTERNAL)  != 0)
        || (bPathPersistence == true))
    {
        pLog->StartSubTest();
        if(Topology.IsWinPTopologyCapable(DISPLAYCONFIG_TOPOLOGY_EXTERNAL, true))
        {
            Topology.SetDisplayConfig(0, NULL, 0, NULL, DISPLAYCONFIG_TOPOLOGY_EXTERNAL | SDC_PATH_PERSIST_IF_REQUIRED, true, false);

            if(bPathPersistence == true)
            {
                Topology.SetTopologyWithModes(16);
            }
        }
        else
        {
            pLog->Skip( L"Machine is not external capable." );
        }
        flags &= ~DISPLAYCONFIG_TOPOLOGY_EXTERNAL;
    }

    if(((flags & DISPLAYCONFIG_TOPOLOGY_EXTEND)  != 0)
        || (bPathPersistence == true))
    {
        pLog->StartSubTest();
        if(Topology.IsWinPTopologyCapable(DISPLAYCONFIG_TOPOLOGY_EXTEND, true))
        {
            Topology.SetDisplayConfig(0, NULL, 0, NULL, DISPLAYCONFIG_TOPOLOGY_EXTEND | SDC_PATH_PERSIST_IF_REQUIRED, true, false);

            if(bPathPersistence == true)
            {
                Topology.SetTopologyWithModes(16);
            }
        }
        else
        {
            pLog->Skip( L"Machine is not extend capable." );
        }
        flags &= ~DISPLAYCONFIG_TOPOLOGY_EXTEND;
    }

    if(((flags & DISPLAYCONFIG_TOPOLOGY_CLONE)  != 0)
        || (bPathPersistence == true))
    {
        pLog->StartSubTest();
        if(Topology.IsWinPTopologyCapable(DISPLAYCONFIG_TOPOLOGY_CLONE, true))
        {
            Topology.SetDisplayConfig(0, NULL, 0, NULL, DISPLAYCONFIG_TOPOLOGY_CLONE | SDC_PATH_PERSIST_IF_REQUIRED, true, false);

            if(bPathPersistence == true)
            {
                Topology.SetTopologyWithModes(16);
            }
        }
        else
        {
            pLog->Skip( L"Machine is not clone capable." );
        }
        flags &= ~DISPLAYCONFIG_TOPOLOGY_CLONE;
    }        

    //This is the work around for bug 286479
    ChangeDisplaySettingsEx(0,0,0,0,0);

    if((flags & CCDTEST_TOPOLOGY_SINGLE_ONLY)
        || (flags & CCDTEST_TOPOLOGY_CLONE_ONLY)
        || (flags & CCDTEST_TOPOLOGY_EXTEND_ONLY)
        || (flags & CCDTEST_TOPOLOGY_ALL))
    {
        Topology.SetAllTopologyWithModes(flags);
    }

    if(bFlagValidate)
    {
        Topology.FlagValidate();                    
    }

    if(bForceProjection)
    {
        if(bSetBootPersistence)
        {
            Topology.SetBootPersistence();    //add adapterid and targetid if needed                
        }         
        if(bRemoveBootPersistence)
        {
            Topology.RemoveBootPersistence();                    
        }
        if(bTestBootPersistenceOn)
        {
            Topology.TestBootPersistenceOn();                    
        }         
        if(bTestBootPersistenceOff)
        {
            Topology.TestBootPersistenceOff();                    
        } 
    }

    if(bAllTopologyCombinations)
    {
        Topology.SetAllTopologyCombinations();                    
    }    

    {
        OSVERSIONINFOEX info;
        memset( &info, 0, sizeof(info) );
        info.dwOSVersionInfoSize = sizeof(info);

        if( !GetVersionEx( (OSVERSIONINFO*)&info ) )
        {
            pLog->Fail( FormatStringTS( L"Failed to get os version info: 0x%08X", HRESULT_FROM_WIN32(GetLastError()) ).c_str() );
        }
        else
        {
            if( info.dwMajorVersion > 6 ||
                (info.dwMajorVersion == 6 && 
                info.dwMinorVersion > 1) )
            {
                if( flags & CCDTEST_ALL_PATHS )
                {
                    TestAllSinglePaths( pLog, &Topology );
                }

                if( flags & CCDTEST_LOW_BPP_MODE )
                {
                    TestLowBppMode( pLog, &Topology );
                }
            }
        }
    }

    //Clean up local heap
    if(pDdInfo != NULL)
    {
        delete pDdInfo;
    }
}

void CSetDisplayConfig::PostWalk(CLogWriter * pLog)
{
}

