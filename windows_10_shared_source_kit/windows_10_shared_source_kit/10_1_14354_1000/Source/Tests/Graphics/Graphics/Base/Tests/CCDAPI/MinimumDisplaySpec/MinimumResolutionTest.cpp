#include "MinimumResolutionTest.h"
#include "CustomTestFactors.h"

DisplayIdType::DisplayIdType( UINT32 v )
    : m_Value(v)
{
}

tstring ToString( DisplayIdType const& id )
{
    TCHAR buf[16];
    _stprintf_s( buf, 16, _T("0x%08X"), id.m_Value );
    return buf;
}

bool FromString( DisplayIdType* pId, tstring const& str )
{
    return 1 == _stscanf_s( str.c_str(), _T("0x%08X"), &pId->m_Value );
}

bool operator ==( DisplayIdType const& lhs, DisplayIdType const& rhs )
{
    return lhs.m_Value == rhs.m_Value;
}

MinimumResolution::MinimumResolution( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
    , m_nAdapterCount(0)
{
    memset( m_pAdapters, 0, sizeof(m_pAdapters) );
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void MinimumResolution::InitializeParameters()
{
    ccd::DisplaySettings displaySettings;

    LONG ret = m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ALL_PATHS, &displaySettings );
    if( ERROR_SUCCESS != ret )
    {
        LOG_ERROR( ret, _T("Failed to initialize test parameters due to previous error.") );
        return;
    }
    
    TCHAR buf[256];
    for( UINT32 i = 0; i < displaySettings.m_NumPaths; ++i )
    {
        DISPLAYCONFIG_PATH_SOURCE_INFO const& source = displaySettings.m_pPaths[i].sourceInfo;
        DISPLAYCONFIG_PATH_TARGET_INFO const& target = displaySettings.m_pPaths[i].targetInfo;
        // find if we've seen this adapter before
        AdapterTestSet* pAdapter = std::find_if( 
                m_pAdapters,
                m_pAdapters + m_nAdapterCount,
                [&]( AdapterTestSet const& ats )
            {
                return 0 == memcmp( &source.adapterId, &ats.m_AdapterLUID, sizeof(LUID) );
            } );

        if( pAdapter == m_pAdapters + m_nAdapterCount )
        {
            if( !(m_nAdapterCount < MAX_ADAPTERS) )
            {
                LOG_ERROR( E_FAIL, _T("System has more adapters then the supported number: %d"),
                       MAX_ADAPTERS );
                return;
            }
            
            _stprintf_s( buf, 256, _T("Adapter_%d_LUID"), m_nAdapterCount );
            pAdapter->m_pAdapterParam = AddParameter( buf, &pAdapter->m_AdapterLUID );
            pAdapter->m_AdapterLUID = source.adapterId;

            DISPLAY_DEVICE dd;
            memset( &dd, 0, sizeof(dd) );
            dd.cb = sizeof(dd);
            if( m_DisplayTopology.GetDisplayDevice( source.adapterId, source.id, &dd ) )
            {
                _stprintf_s( buf, 256, _T("Adapter_%d_String"), m_nAdapterCount );
                pAdapter->m_pAdapterStrParam = AddParameter( buf, &pAdapter->m_AdapterString );
                pAdapter->m_AdapterString = dd.DeviceString;
            }

            ++m_nAdapterCount;
        }

        ptrdiff_t adapterIndex = pAdapter - m_pAdapters;

        // find if this source exists yet?
        DisplayIdType* pSrcId = std::find_if(
                pAdapter->m_pSourceId,
                pAdapter->m_pSourceId + pAdapter->m_nSourceCount,
                [&]( DisplayIdType id )
            {
                return id == source.id;
            } );

        if( pSrcId == pAdapter->m_pSourceId + pAdapter->m_nSourceCount )
        {
            if( !(pAdapter->m_nSourceCount < MAX_SOURCES ) )
            {
                LOG_ERROR( E_FAIL, _T("Adapter 0x%08X%08X has more sources then the supported number: %d"),
                        pAdapter->m_AdapterLUID.HighPart,
                        pAdapter->m_AdapterLUID.LowPart,
                        MAX_SOURCES );
                return;
            }

            _stprintf_s( buf, 256, _T("Adapter_%d_Src_%d"), adapterIndex, pAdapter->m_nSourceCount );
            pAdapter->m_pSourceParams[pAdapter->m_nSourceCount] = AddParameter( buf, pAdapter->m_pSourceId + pAdapter->m_nSourceCount );

            ++pAdapter->m_nSourceCount;
            *pSrcId = source.id;
        }

        // Only look at available targets.  ]
        // Not going to worry about forcible targets for now.
        if( target.targetAvailable )
        {
            DisplayIdType* pTrgId = std::find_if(
                    pAdapter->m_pTargetId,
                    pAdapter->m_pTargetId + pAdapter->m_nTargetCount,
                    [&]( DisplayIdType id )
                {
                    return id == target.id;
                } );

            if( pTrgId == pAdapter->m_pTargetId + pAdapter->m_nTargetCount )
            {
                if( !(pAdapter->m_nTargetCount < MAX_TARGETS ) )
                {
                    LOG_ERROR( E_FAIL, _T("Adapter 0x%08X%08X has more targets then the supported number: %d"),
                            pAdapter->m_AdapterLUID.HighPart,
                            pAdapter->m_AdapterLUID.LowPart,
                            MAX_TARGETS );
                    return;
                }

                _stprintf_s( buf, 256, _T("Adapter_%d_Trg_%d"), adapterIndex, pAdapter->m_nTargetCount );
                pAdapter->m_pTargetParams[pAdapter->m_nTargetCount] = AddParameter( buf, pAdapter->m_pTargetId + pAdapter->m_nTargetCount );

                ++pAdapter->m_nTargetCount;
                *pTrgId = target.id;
            }
        }
    }

    std::vector<testfactor::RFactor> variationsPerAdapter;

    for( UINT32 i = 0; i < m_nAdapterCount; ++i )
    {
        if( m_pAdapters[i].m_nTargetCount < 1 )
        {
            LOG_WARN( 
                    _T("Skipping adapter that has no targets.\n%s\n%s"),
                    ToString( m_pAdapters[i].m_AdapterLUID ).c_str(),
                    m_pAdapters[i].m_AdapterString.c_str() );
            // zero the sources so that we don't test against this adapter.
            m_pAdapters[i].m_nSourceCount = 0;

            continue;
        }

        testfactor::RFactor adapter = AddParameterValue<LUID>( m_pAdapters[i].m_pAdapterParam, m_pAdapters[i].m_AdapterLUID );
        if( m_pAdapters[i].m_pAdapterStrParam )
        {
            adapter = adapter * AddParameterValue<tstring>( m_pAdapters[i].m_pAdapterStrParam, m_pAdapters[i].m_AdapterString );
        }

        testfactor::RFactor paths;
        if( m_pAdapters[i].m_nSourceCount >= m_pAdapters[i].m_nTargetCount )
        {
            // just setup a 1 to 1 mapping.  Don't need to use all sources.
            m_pAdapters[i].m_nSourceCount = m_pAdapters[i].m_nTargetCount;
            for( UINT32 j = 0; j < m_pAdapters[i].m_nSourceCount; ++j )
            {
                testfactor::RFactor path = AddParameterValue( m_pAdapters[i].m_pSourceParams[j], m_pAdapters[i].m_pSourceId[j] ) *
                        AddParameterValue( m_pAdapters[i].m_pTargetParams[j], m_pAdapters[i].m_pTargetId[j] );
                
                if( paths.Valid() )
                {
                    paths = paths * path;
                }
                else
                {
                    paths = path;
                }
            }
        }
        else
        {
            // setup a nCk mapping
            // sources have fixed mapping.
            // Targets have nCk mapping
            for( UINT32 j = 0; j < m_pAdapters[i].m_nSourceCount; ++j )
            {
                testfactor::RFactor path = AddParameterValue( m_pAdapters[i].m_pSourceParams[j], m_pAdapters[i].m_pSourceId[j] );
                
                if( paths.Valid() )
                {
                    paths = paths * path;
                }
                else
                {
                    paths = path;
                }
            }

            CUserValueSet<DisplayIdType>* pTargetValueSet = new CUserValueSet<DisplayIdType>();
            for( UINT32 j = 0; j < m_pAdapters[i].m_nTargetCount; ++j )
            {
                pTargetValueSet->AddValue( m_pAdapters[i].m_pTargetId[j] );
            }

            paths = paths * NChooseK<DisplayIdType>( this, pTargetValueSet, m_pAdapters[i].m_nSourceCount, m_pAdapters[i].m_pTargetParams );
        }

        variationsPerAdapter.push_back( adapter * paths );
    }

    SetRootTestFactor( NewSimultaneousIndependentSetFactor( variationsPerAdapter.begin(), variationsPerAdapter.end() ) );
}

TestResult::Type MinimumResolution::SetupTest()
{
    if( !m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS | QDC_DATABASE_CURRENT ) )
    {
        return TestResult::Fail;
    }
    return TestResult::Pass;
}

TestResult::Type MinimumResolution::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type MinimumResolution::ExecuteTestCase()
{
    LOG_MESSAGE( _T("Testing display configuration with all displays running %d x %d resolution."), MIN_WIDTH, MIN_HEIGHT );

    // build the configuration.
    ccd::DisplaySettings displaySettings;

    {
        UINT32 pathCount = 0;
        for( UINT32 i = 0; i < m_nAdapterCount; ++i )
        {
            pathCount += m_pAdapters[i].m_nSourceCount;
        }

        displaySettings.m_NumPaths = pathCount;
        displaySettings.m_pPaths = new DISPLAYCONFIG_PATH_INFO[pathCount];
        memset( displaySettings.m_pPaths, 0, sizeof( DISPLAYCONFIG_PATH_INFO ) * pathCount );
        displaySettings.m_NumModes = pathCount;
        displaySettings.m_pModes = new DISPLAYCONFIG_MODE_INFO[pathCount];
        memset( displaySettings.m_pModes, 0, sizeof( DISPLAYCONFIG_MODE_INFO ) * pathCount );

        UINT32 wPosition = 0;
        UINT32 nextFreePath = 0;
        for( UINT32 i = 0; i < m_nAdapterCount; ++i )
        {
            for( UINT32 j = 0; j < m_pAdapters[i].m_nSourceCount; ++j )
            {
                assert( nextFreePath < pathCount );
                UINT32 index = nextFreePath++;

                DISPLAYCONFIG_PATH_INFO& path = displaySettings.m_pPaths[index];
                path.flags = DISPLAYCONFIG_PATH_ACTIVE;
                
                // source setup
                DISPLAYCONFIG_PATH_SOURCE_INFO& src = path.sourceInfo;
                src.adapterId = m_pAdapters[i].m_AdapterLUID;
                src.id = m_pAdapters[i].m_pSourceId[j].m_Value;
                src.modeInfoIdx = index;

                // target setup
                DISPLAYCONFIG_PATH_TARGET_INFO& trg = path.targetInfo;
                trg.adapterId = m_pAdapters[i].m_AdapterLUID;
                trg.id = m_pAdapters[i].m_pTargetId[j].m_Value;
                trg.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
                //trg.outputTechnology = ;
                trg.rotation = DISPLAYCONFIG_ROTATION_IDENTITY;
                trg.scaling = DISPLAYCONFIG_SCALING_PREFERRED;
                trg.refreshRate.Numerator = 0;
                trg.refreshRate.Denominator = 0;
                trg.scanLineOrdering = DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED; 

                // setup the mode
                DISPLAYCONFIG_MODE_INFO& mode = displaySettings.m_pModes[index];
                mode.infoType = DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE;
                mode.id = src.id;
                mode.adapterId = src.adapterId;
                mode.sourceMode.width = MIN_WIDTH;
                mode.sourceMode.height = MIN_HEIGHT;
                mode.sourceMode.pixelFormat = DISPLAYCONFIG_PIXELFORMAT_32BPP;
                mode.sourceMode.position.x = wPosition;
                mode.sourceMode.position.y = 0;
                wPosition += MIN_WIDTH;
            }
        }
    }

    LOG_MESSAGE( ccd::str::ToString( displaySettings ).c_str() );

    LONG ret = m_DisplayTopology.SetDisplayConfig(
            &displaySettings,
            SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_ALLOW_CHANGES );//| SDC_PATH_PERSIST_IF_REQUIRED );

    if( ERROR_SUCCESS != ret )
    {
        LOG_ERROR( ret, _T("The system doesn't appear to support the minimum resolution across all displays in the previous configuration.") );
        return TestResult::Fail;
    }

    return TestResult::Pass;
}

void MinimumResolution::CleanupTestCase()
{
}

void MinimumResolution::CleanupTest()
{
}

tstring MinimumResolution::GetTestName()
{
    return _T("Minimum Resolution Check");       
}
