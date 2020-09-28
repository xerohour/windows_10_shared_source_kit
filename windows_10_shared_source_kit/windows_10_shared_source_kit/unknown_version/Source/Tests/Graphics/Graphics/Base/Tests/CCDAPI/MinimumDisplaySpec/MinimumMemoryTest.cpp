#include "MinimumMemoryTest.h"
#include <memory>
#include <dxgi.h>
#include "CustomTestFactors.h"

struct SafeRelease
{
    void operator ()( IUnknown* p )
    {
        if( p )
        {
            p->Release();
        }
    }
};

MinimumMemoryRequired::MinimumMemoryRequired( LoggerWrapper* pLogger )
    : AbstractAdapterTest( pLogger )
{
    memset( &m_Adapter, 0, sizeof(m_Adapter) );
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

TestResult::Type MinimumMemoryRequired::SetupTest()
{
    return TestResult::Pass;
}

TestResult::Type MinimumMemoryRequired::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type MinimumMemoryRequired::ExecuteTestCase()
{
    ccd::DisplaySettings displaySettings;

    LONG ret = m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ALL_PATHS, &displaySettings );
    if( ERROR_SUCCESS != ret )
    {
        LOG_ERROR( ret, _T("Failed to initialize test parameters due to previous error.") );
        return TestResult::Fail;
    }

    UINT32 availableTargets = 0;
    UINT32 forcibleTargets = 0;
    UINT32 totalArea = 0;
    UINT32 numDVI = 0;
    UINT32 numVGA = 0;
    std::vector<UINT32> targets;
    std::vector<DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY> technology;
    for( UINT32 i = 0; i < displaySettings.m_NumPaths; ++i )
    {
        DISPLAYCONFIG_PATH_TARGET_INFO const& target = displaySettings.m_pPaths[i].targetInfo;
        if( 0 != memcmp( &target.adapterId, &m_Adapter, sizeof(LUID) ) )
        {
            continue;
        }

        if( targets.end() != std::find( targets.begin(), targets.end(), target.id ) )
        {
            continue;
        }

        targets.push_back( target.id );
        technology.push_back( target.outputTechnology );

        if( target.targetAvailable )
        {
            ++availableTargets;

            DISPLAYCONFIG_TARGET_PREFERRED_MODE trgMode;
            memset( &trgMode, 0, sizeof(trgMode) );

            trgMode.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE;
            trgMode.header.size = sizeof(trgMode);
            trgMode.header.adapterId = m_Adapter;
            trgMode.header.id = target.id;

            // probably can't get any information about a forcible target that isn't available.
            LONG ret = m_DisplayTopology.DisplayConfigGetDeviceInfo( &trgMode.header );
            if( ERROR_SUCCESS != ret )
            {
                LOG_ERROR( 
                    ret, 
                    _T("Failed to calculated memory required for adapter %s due to previous error."),
                    ToString( m_Adapter ).c_str() );
                return TestResult::Fail;
            }

            UINT32 area = trgMode.width * trgMode.height;
            totalArea += area;

            LOG_MESSAGE( _T("Target: 0x%08X preferred: % 4d x % 4d pixels type: %s"),
                    target.id,
                    trgMode.width,
                    trgMode.height,
                    ccd::str::ToString<DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY>( target.outputTechnology ).c_str() );
        }
        else
        {
            LOG_MESSAGE( _T("Target: 0x%08X type: %s"),
                    target.id,
                    ccd::str::ToString<DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY>( target.outputTechnology ).c_str() );
        }

        if( target.statusFlags & DISPLAYCONFIG_TARGET_FORCIBLE )
        {
            ++forcibleTargets;
        }

        switch( target.outputTechnology )
        {
            case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15:
                ++numVGA;
                break;
            case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI:
                ++numDVI;
                break;
        }
    }
    
    // These are actual simultaneous working targets, so 
    // we know in the case of DVI/VGA that they are independently
    // connected to the system.
    size_t requiredSizeBasedOnResolution = 0;
    switch( availableTargets )
    {
        case 1:
            if( totalArea < 1310720 )
            {
                requiredSizeBasedOnResolution = 64;
            }
            else if( totalArea < 2304000 )
            {
                requiredSizeBasedOnResolution = 128;
            }
            else
            {
                requiredSizeBasedOnResolution = 256;
            }
            break;
        case 2:
            if( totalArea < 2621440 )
            {
                requiredSizeBasedOnResolution = 128;
            }
            else
            {
                requiredSizeBasedOnResolution = 256;
            }
            break;
    }

    // figure out port structure requirement vs memory
    // we already know how many vga vs dvi so those don't need calculation
    //static const DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputtech[] = 
    //{
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE,
    //    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL
    //};

    //for( UINT32 i = 0; i < ARRAY_SIZE(outputtech); ++i )
    //{
    //    
    //}

    UINT32 portCount = targets.size();

    if( numVGA >= numDVI )
    {
        portCount -= numVGA;
        portCount += numVGA - numDVI;
    }

    // size is 128mb * portCount;
    size_t requiredSizeBasedOnPortCount = portCount * 128;

    std::unique_ptr<IDXGIFactory,SafeRelease> pFactory;
    HRESULT hr;
    {
        IDXGIFactory* pTemp = nullptr;
        if( FAILED( hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&pTemp )) )
        {
            LOG_ERROR_HR( hr, _T("Failed to create IDXGIFactory.  The factory is needed to access device memory property values.") );
            return TestResult::Fail;
        }
        
        pFactory.reset( pTemp );
    }

    DXGI_ADAPTER_DESC desc;
    bool foundAdapter = false;
    for( UINT32 i = 0; true; ++i )
    {
        std::unique_ptr<IDXGIAdapter,SafeRelease> pAdapter;
        {
            IDXGIAdapter* pTemp = nullptr;
            if( FAILED(hr = pFactory->EnumAdapters( i, &pTemp)) )
            {
                if( DXGI_ERROR_NOT_FOUND != hr )
                {
                    break;
                }

                LOG_ERROR_HR( hr, _T("Failed to enumerate adapter %d."), i );
                return TestResult::Fail;
            }
            
            pAdapter.reset( pTemp );
        }

        memset( &desc, 0, sizeof(desc) );

        if( FAILED(hr = pAdapter->GetDesc( &desc )) )
        {
            LOG_ERROR_HR( hr, _T("Failed to get adapter %d description."), i );
            return TestResult::Fail;
        }

        if( 0 == memcmp( &desc.AdapterLuid, &m_Adapter, sizeof(LUID) ) )
        {
            foundAdapter = true;

            break;
        }
    }

    if( !foundAdapter )
    {
        LOG_ERROR_HR( E_FAIL, _T("Failed to locate IDXGIAdapter for LUID: %s"), ToString( m_Adapter ).c_str() );
        return TestResult::Fail;
    }

    LOG_MESSAGE( 
            _T("Dedicated video Memory: %4d MB\n")
            _T("System video Memory:    %4d MB\n")
            _T("Shared video Memory:    %4d MB"),
            desc.DedicatedVideoMemory ? desc.DedicatedVideoMemory / (1024 * 1024) : 0,
            desc.DedicatedSystemMemory ? desc.DedicatedSystemMemory / (1024 * 1024) : 0,
            desc.SharedSystemMemory ? desc.SharedSystemMemory / (1024 * 1024) : 0 );

    // great, now all of the test info is available.
    size_t requiredMemory = 0;
    if( requiredSizeBasedOnPortCount > requiredSizeBasedOnResolution )
    {
        LOG_MESSAGE( _T("Calculating GPU memory requirement based on the presence of %d display connectors."), portCount );
        requiredMemory = requiredSizeBasedOnPortCount;
    }
    else
    {
        LOG_MESSAGE( _T("Calculating GPU memory requirement based on combined display target area %d pixels."), totalArea );
        requiredMemory = requiredSizeBasedOnResolution;
    }

    if( desc.DedicatedVideoMemory )
    {
        size_t sizeInMeg = desc.DedicatedVideoMemory / (1024 * 1024);
        if( sizeInMeg < requiredMemory )
        {
            LOG_ERROR_HR( E_FAIL, _T("Device does not meet dedicated memory requirements.  %d MB found, required minimum: %d MB"),
                    sizeInMeg,
                    requiredMemory );
            return TestResult::Fail;
        }
    }
    else if( desc.DedicatedSystemMemory )
    {
        size_t sizeInMeg = desc.DedicatedSystemMemory / (1024 * 1024);
        if( desc.DedicatedSystemMemory < requiredMemory )
        {
            LOG_ERROR_HR( E_FAIL, _T("Device does not meet system memory requirements.  %d MB found, required minimum: %d MB"),
                    sizeInMeg,
                    requiredMemory );
            return TestResult::Fail;
        }
    }
    else
    {
        LOG_ERROR_HR( 
                E_FAIL, 
                _T("Device does not meet display memory requirements. No dedicated or system graphics memory found. Required minimum: %d MB"),
                requiredMemory );
        return TestResult::Fail;
    }

    return TestResult::Pass;
}

void MinimumMemoryRequired::CleanupTestCase()
{
}

void MinimumMemoryRequired::CleanupTest()
{
}

tstring MinimumMemoryRequired::GetTestName()
{
    return _T("Minimum GPU Memory Check");       
}

