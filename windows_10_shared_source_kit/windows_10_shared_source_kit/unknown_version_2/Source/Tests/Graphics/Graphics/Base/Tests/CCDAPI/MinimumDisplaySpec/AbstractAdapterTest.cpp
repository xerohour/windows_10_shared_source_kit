#include "AbstractAdapterTest.h"
#include <vector>
#include "CustomTestFactors.h"

AbstractAdapterTest::AbstractAdapterTest( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void AbstractAdapterTest::InitializeParameters()
{
    ccd::DisplaySettings displaySettings;

    LONG ret = m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ALL_PATHS, &displaySettings );
    if( ERROR_SUCCESS != ret )
    {
        LOG_ERROR( ret, _T("Failed to initialize test parameters due to previous error.") );
        return;
    }

    std::vector<LUID> adapters;
    std::vector<tstring> adapterStr;
    for( UINT32 i = 0; i < displaySettings.m_NumPaths; ++i )
    {
        LUID const& adapterId = displaySettings.m_pPaths[i].sourceInfo.adapterId;
        auto it = std::find( adapters.begin(), adapters.end(), adapterId );
        if( it == adapters.end() )
        {
            adapters.push_back( adapterId );

            DISPLAY_DEVICE dd;
            memset( &dd, 0, sizeof(dd) );
            dd.cb = sizeof(dd);
            if( m_DisplayTopology.GetDisplayDevice( adapterId, displaySettings.m_pPaths[i].sourceInfo.id, &dd ) )
            {
                adapterStr.push_back( dd.DeviceString );
            }
            else
            {
                adapterStr.push_back( _T("none") );
            }
        }
    }

    CTestCaseParameter<LUID>* pAdapterLUIDParam = AddParameter<LUID>( _T("AdapterLUID"), &m_Adapter );
    CUserValueSet<LUID>* pValues = new CUserValueSet<LUID>( adapters );

    CTestCaseParameter<tstring>* pAdapterStrParam = AddParameter<tstring>( _T("AdapterStr"), &m_AdapterStr );
    CUserValueSet<tstring>* pStrs = new CUserValueSet<tstring>( adapterStr );

    std::vector<testfactor::RFactor> factors;
    factors.push_back( AddParameterValueSet<LUID>( pAdapterLUIDParam, pValues ) );
    factors.push_back( AddParameterValueSet<tstring>( pAdapterStrParam, pStrs ) );
    
    SetRootTestFactor( NewSimultaneousIndependentSetFactor( factors.begin(), factors.end() ) );
}
