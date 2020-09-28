#include "CheckDualResolution.h"


CheckDualResolution::CheckDualResolution( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void CheckDualResolution::InitializeParameters()
{
    testfactor::RFactor testcase = AddParameter<UINT>( _T("ModeTest"), &m_ModeTest, 1u );
    SetRootTestFactor( testcase );
}

TestResult::Type CheckDualResolution::SetupTest()
{
    HRESULT hr = m_DisplayTopology.IsSystemMultiMonCapable();
    if( FAILED(hr) )
    {
        LOG_MESSAGE( _T("Failed test due to previous error(s).") );
        return TestResult::Fail;
    }
    
    if( S_OK != hr )
    {
        LOG_MESSAGE( _T("Skipping test since system is not multi-display capable.") );
        return TestResult::Skip;
    }

    m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
    return TestResult::Pass;
}

TestResult::Type CheckDualResolution::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type CheckDualResolution::ExecuteTestCase()
{
    UINT32 anchoredTarget = 0;
    LUID anchoredAdapter;
    memset( &anchoredAdapter, 0, sizeof(LUID) );

    LONG ret = m_DisplayTopology.GetAnchoredTarget( &anchoredTarget, &anchoredAdapter );
    if( ret != ERROR_SUCCESS )
    {
        LOG_MESSAGE( _T("Failed to test multimon resolution requirement due to preivous error(s).") );
        return TestResult::Fail;
    }

    ccd::DisplaySettings displaySettings;
    ret = m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ALL_PATHS, &displaySettings );
    if( ret != ERROR_SUCCESS )
    {
        LOG_MESSAGE( _T("Failed to get the path information necessary to construct a multimon topology due to previous error(s).") );
        return TestResult::Fail;
    }

    DISPLAYCONFIG_PATH_INFO pPaths[2];
    memset( pPaths, 0, sizeof(pPaths) );
    DISPLAYCONFIG_MODE_INFO pModes[2];
    memset( pModes, 0, sizeof(pModes) );

    pPaths[0].flags = DISPLAYCONFIG_PATH_ACTIVE;
    pPaths[1].flags = DISPLAYCONFIG_PATH_ACTIVE;

    std::set<UINT32> sources = m_DisplayTopology.GetSources( displaySettings.m_NumPaths, displaySettings.m_pPaths, &anchoredAdapter );

    // setup for the first adapter.
    auto sIt = sources.begin();
    pPaths[0].sourceInfo.adapterId = anchoredAdapter;
    pPaths[0].sourceInfo.id = *sIt;
    ++sIt;
    pPaths[0].sourceInfo.modeInfoIdx = 0;
    pPaths[0].sourceInfo.statusFlags = DISPLAYCONFIG_SOURCE_IN_USE;

    pPaths[0].targetInfo.adapterId = anchoredAdapter;
    pPaths[0].targetInfo.id = anchoredTarget;
    pPaths[0].targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
    pPaths[0].targetInfo.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
    pPaths[0].targetInfo.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
    pPaths[0].targetInfo.targetAvailable = TRUE;
    pPaths[0].targetInfo.statusFlags = DISPLAYCONFIG_TARGET_IN_USE;

    // where should the second path come from?
    // check for a second target on the anchored adapter
    // otherwise check for a second adapter
    std::set<UINT32> targets = m_DisplayTopology.GetAvailableTargets( displaySettings.m_NumPaths, displaySettings.m_pPaths, &anchoredAdapter );

    if( targets.size() > 1 )
    {
        if( sources.size() < 2 )
        {
            // system only supports clone?
            LOG_ERROR( E_FAIL, _T("The anchored adapter has multiple targets available but only a single sources.") );
            return TestResult::Fail;
        }
        // great, use this setup.
        auto tIt = targets.begin();
        if( *tIt == anchoredTarget )
        {
            ++tIt;
        }

        pPaths[1].sourceInfo.adapterId = anchoredAdapter;
        pPaths[1].sourceInfo.id = *sIt;
        ++sIt;

        pPaths[1].targetInfo.adapterId = anchoredAdapter;
        pPaths[1].targetInfo.id = *tIt;
        ++tIt;
    }
    else
    {
        LUID secondAdapter;
        memset( &secondAdapter, 0, sizeof(secondAdapter) );
        for( UINT32 i = 0; i < displaySettings.m_NumPaths; ++i )
        {
            if( 0 == memcmp( &anchoredAdapter, &displaySettings.m_pPaths[i].sourceInfo.adapterId, sizeof(LUID) ) )
            {
                continue;
            }

            targets = m_DisplayTopology.GetAvailableTargets( displaySettings.m_NumPaths, displaySettings.m_pPaths, &displaySettings.m_pPaths[i].sourceInfo.adapterId );
            if( targets.empty() )
            {
                continue;
            }

            secondAdapter = displaySettings.m_pPaths[i].sourceInfo.adapterId;
            break;
        }

        if( 0 == secondAdapter.LowPart && 0 == secondAdapter.HighPart )
        {
            LOG_ERROR( E_FAIL, _T("Failed to locate a second adapter or second display to test extended topology resolution requirements.") );
            return TestResult::Fail;
        }

        sources = m_DisplayTopology.GetSources( displaySettings.m_NumPaths, displaySettings.m_pPaths, &secondAdapter );

        pPaths[1].sourceInfo.adapterId = secondAdapter;
        pPaths[1].sourceInfo.id = *(sources.begin());

        pPaths[1].targetInfo.adapterId = secondAdapter;
        pPaths[1].targetInfo.id = *(targets.begin());
    }

    pPaths[1].sourceInfo.modeInfoIdx = 1;
    pPaths[1].sourceInfo.statusFlags = DISPLAYCONFIG_SOURCE_IN_USE;
    pPaths[1].targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
    pPaths[1].targetInfo.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
    pPaths[1].targetInfo.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
    pPaths[1].targetInfo.targetAvailable = TRUE;
    pPaths[1].targetInfo.statusFlags = DISPLAYCONFIG_TARGET_IN_USE;

    pModes[0].infoType = DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE;
    pModes[0].id = pPaths[0].sourceInfo.id;
    pModes[0].adapterId = pPaths[0].sourceInfo.adapterId;
    pModes[0].sourceMode.width = 1024;
    pModes[0].sourceMode.height = 768;
    pModes[0].sourceMode.pixelFormat = DISPLAYCONFIG_PIXELFORMAT_32BPP;

    pModes[1].infoType = DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE;
    pModes[1].id = pPaths[1].sourceInfo.id;
    pModes[1].adapterId = pPaths[1].sourceInfo.adapterId;
    pModes[1].sourceMode.width = 1920;
    pModes[1].sourceMode.height = 1080;
    pModes[1].sourceMode.pixelFormat = DISPLAYCONFIG_PIXELFORMAT_32BPP;
    pModes[1].sourceMode.position.x = 1024;

    ret = m_DisplayTopology.SetDisplayConfig(
        2,
        pPaths,
        2,
        pModes,
        SDC_ALLOW_CHANGES | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_APPLY );

    if( ret != ERROR_SUCCESS )
    {
        LOG_MESSAGE( _T("Failed to verify extended topology resolution requirements due to the previous error(s).") );
        return TestResult::Fail;
    }

    return TestResult::Pass;
}

void CheckDualResolution::CleanupTestCase()
{
}

void CheckDualResolution::CleanupTest()
{
    m_DisplayTopology.PopDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
}

tstring CheckDualResolution::GetTestName()
{
    return _T("Verify multimon resolutoin requirements.");
}
