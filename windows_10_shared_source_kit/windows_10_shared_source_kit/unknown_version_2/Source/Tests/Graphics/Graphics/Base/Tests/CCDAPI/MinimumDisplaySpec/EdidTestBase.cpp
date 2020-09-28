#include "EdidTestBase.h"
#include <algorithm>


EdidTestBase::EdidTestBase( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
    , m_TargetId( 0 )
{
    memset( &m_AdapterLuid, 0, sizeof(m_AdapterLuid) );
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void EdidTestBase::InitializeParameters()
{
    using namespace testfactor;

    CTestCaseParameter<LUID>* pAdapterLuid = AddParameter<LUID>( _T("AdapterLuid"), &m_AdapterLuid );
    CTestCaseParameter<HEX32>* pTargetIdParam = AddParameter<HEX32>( _T("TargetId"), &m_TargetId );

    if( ERROR_SUCCESS != m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ALL_PATHS, &m_AllPaths ) )
    {
        return;
    }

    std::set<LUID> adapters;
    for( UINT32 i = 0; i < m_AllPaths.m_NumPaths; ++i )
    {
        if( m_AllPaths.m_pPaths[i].targetInfo.targetAvailable )
        {
            adapters.insert( m_AllPaths.m_pPaths[i].targetInfo.adapterId );
        }
    }

    RFactor root;

    for( auto it = adapters.begin(), eit = adapters.end(); it != eit; ++it )
    {
        RFactor rfAdapter = AddParameterValue<LUID>( pAdapterLuid, *it );

        CUserValueSet<HEX32>* pTargets = new CUserValueSet<HEX32>();

        std::set<UINT32> targets;
        for( UINT32 i = 0; i < m_AllPaths.m_NumPaths; ++i )
        {
            if( !m_AllPaths.m_pPaths[i].targetInfo.targetAvailable )
            {
                continue;
            }

            if( m_AllPaths.m_pPaths[i].targetInfo.adapterId != *it )
            {
                continue;
            }

            // check the connector type.  Some analog connectors can't provide
            // an edid so the test would end up failing.
            switch( m_AllPaths.m_pPaths[i].targetInfo.outputTechnology )
            {
            case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO:
            case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO:
            case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO:
                {
                    LOG_MESSAGE( 
                        _T("Skipping target 0x%X on adapter %s because %s cannot provide an edid."), 
                        m_AllPaths.m_pPaths[i].targetInfo.id,
                        ToString( m_AllPaths.m_pPaths[i].targetInfo.adapterId ).c_str(),
                        ccd::str::ToString( m_AllPaths.m_pPaths[i].targetInfo.outputTechnology ).c_str() );
                    break;
                }

            default:
                targets.insert( m_AllPaths.m_pPaths[i].targetInfo.id );
            }
        }

        std::for_each( targets.begin(), targets.end(), [&]( UINT32 t ) -> void 
        {
            pTargets->AddValue( HEX32(t) );
        } );

        RFactor rfTargets = AddParameterValueSet<HEX32>( pTargetIdParam, pTargets );

        if( root.Valid() )
        {
            root = root * (rfAdapter * rfTargets);
        }
        else
        {
            root = rfAdapter * rfTargets;
        }
    }

    SetRootTestFactor( root );
}

TestResult::Type EdidTestBase::SetupTest()
{
    // Check if the CCD API is supported - requires WDDM
    {
        UINT32 paths = 0;
        UINT32 modes = 0;
        if( ERROR_NOT_SUPPORTED == GetDisplayConfigBufferSizes( QDC_ONLY_ACTIVE_PATHS, &paths, &modes ) )
        {
            OSVERSIONINFOEX info;
            memset( &info, 0, sizeof(info) );
            info.dwOSVersionInfoSize = sizeof(info);

            if( !GetVersionEx( (OSVERSIONINFO*)&info ) )
            {
                LOG_ERROR( HRESULT_FROM_WIN32(GetLastError()), _T("Failed to get OS version info.") );
                return TestResult::Fail;
            }
            
            // make sure that we only skip if the OS is Windows Server 2008 R2 with XDDM driver
            if( info.dwMajorVersion == 6 
                && info.dwMinorVersion == 1
                && info.wProductType != VER_NT_WORKSTATION )
            {
                LOG_MESSAGE( _T("Skipping test that requires CCD support which isn't available on the current system.") );
                return TestResult::Skip;
            }

            // Otherwise we don't know what this scenario is and so need to 'Discover' it.
            LOG_ERROR( ERROR_NOT_SUPPORTED, _T("This test requires WDDM support. Please verify the system supports WDDM and rerun the test.") );
            return TestResult::Fail;
        }
    }

    m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
    return TestResult::Pass;
}

TestResult::Type EdidTestBase::SetupTestCase()
{
    return SetTestCaseTopology();
}

void EdidTestBase::CleanupTestCase()
{
}

void EdidTestBase::CleanupTest()
{
    m_DisplayTopology.PopDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
}

void SimplifyPath( DISPLAYCONFIG_PATH_INFO* pPath )
{
    pPath->sourceInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
    pPath->targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
    pPath->flags = DISPLAYCONFIG_PATH_ACTIVE;
    pPath->targetInfo.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
    pPath->targetInfo.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
    pPath->targetInfo.refreshRate.Numerator = 0;
    pPath->targetInfo.refreshRate.Denominator = 0;
    pPath->targetInfo.scanLineOrdering = DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED;
}

TestResult::Type EdidTestBase::SetTestCaseTopology()
{
    // find the path we want to use for testing.
    UINT32 pathIdx = 0;
    for( ; pathIdx < m_AllPaths.m_NumPaths; ++pathIdx )
    {
        if( m_AllPaths.m_pPaths[pathIdx].targetInfo.adapterId != m_AdapterLuid )
        {
            continue;
        }

        if( m_AllPaths.m_pPaths[pathIdx].targetInfo.id == m_TargetId )
        {
            break;
        }
    }

    if( pathIdx >= m_AllPaths.m_NumPaths )
    {
        return TestResult::Fail;
    }
    
    // clean up the path in question.
    DISPLAYCONFIG_PATH_INFO path = m_AllPaths.m_pPaths[pathIdx];
    SimplifyPath( &path );

    LOG_MESSAGE( _T("Connector Type: %s"), ccd::str::ToString( path.targetInfo.outputTechnology ).c_str() );

    // Set this path exclusively.
    if( ERROR_SUCCESS != m_DisplayTopology.SetDisplayConfig( 1, &path, 0, nullptr, SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG ) )
    {
        return TestResult::Fail;
    }

    // get the GDI name of this path
    memset( &m_GDISource, 0, sizeof(m_GDISource) );
    m_GDISource.cb = sizeof(m_GDISource);
    
    // The OS can change our source ID for the path we set.  As a work around we need to query the current path again.
    ccd::DisplaySettings ds;

    if( ERROR_SUCCESS != m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &ds ) )
    {
        return TestResult::Fail;
    }

    m_TestCasePath = ds.m_pPaths[0];
    SimplifyPath( &m_TestCasePath );

    // the source GDI device information.
    if( !m_DisplayTopology.GetDisplayDevice( m_AdapterLuid, ds.m_pPaths[0].sourceInfo.id, &m_GDISource ) )
    {
        return TestResult::Fail;
    }

    memset( &m_GDITarget, 0, sizeof(m_GDITarget) );
    m_GDITarget.cb = sizeof(m_GDITarget);

    // this is a single path so only one monitor will be here.
    // I've found that inactive displays show up as available 
    // to all sources in GDI.  That means we need to find the active
    // monitor and ignore all others.
    {
        bool found = false;
        for( DWORD i = 0; EnumDisplayDevices( m_GDISource.DeviceName, i, &m_GDITarget, EDD_GET_DEVICE_INTERFACE_NAME ); ++i )
        {
            if( m_GDITarget.StateFlags & DISPLAY_DEVICE_ACTIVE )
            {
                found = true;
                break;
            }
        }

        if( !found )
        {
            LOG_ERROR( E_FAIL, _T("Unable to find active gdi monitor on %s"), m_GDISource.DeviceName );
            return TestResult::Fail;
        }
    }

    return TestResult::Pass;
}

