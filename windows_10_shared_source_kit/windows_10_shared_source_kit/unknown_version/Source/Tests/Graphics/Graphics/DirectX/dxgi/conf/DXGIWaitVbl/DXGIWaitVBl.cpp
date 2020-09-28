//
// Project headers
//

#include "DXGIWaitVbl.h"
#include "TestApp.h"


#pragma warning(disable: 4355)

const MODEPRIORITYPATTERN  CDXGIWaitVBl::m_ModePriorityPatterns[] = 
{
    {1024, 768},
    {1280, 1024}
};

CDXGIWaitVBl::CDXGIWaitVBl()
:
	PrimaryAdapterDevice((CTest *)this),
	TestedSwapChain((CTest *)this, &PrimaryAdapterDevice),
	hwndFocusWindow(NULL),
	hwndDeviceWindow(NULL),
	m_pFactory(NULL),
	m_pD3D10Device(NULL),
	m_pSwapChain(NULL),
	m_pDXGIOutput( NULL ),
	m_uNumAdapters( 0 ),
    m_FailedCases( 0 )
{
    ZeroMemory( m_vecNumOutputs, MAX_ADAPTERS * sizeof( UINT ) );
    ZeroMemory( m_fMeasuredIntervals, NO_OF_TESTED_INTERVALS * sizeof( float ) );
};


CDXGIWaitVBl::~CDXGIWaitVBl()
{
};


TEST_RESULT CDXGIWaitVBl::Setup()
{
    WriteToLog("SetUp");

    TEST_RESULT result = RESULT_PASS;
    HRESULT hresult = S_OK;
    
    hwndFocusWindow = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_APPWINDOW,
        m_szFocusWindowClassName,
        "Focus Window",
        WS_OVERLAPPED | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        m_hProgInst,
        NULL);
    
    if(hwndFocusWindow == NULL)
    {
        DWORD error = GetLastError();
        HRESULT hresult = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(error);
        WriteToLog("CreateWindowEx Failed. hresult= 0x%x", hresult);
        result = RESULT_FAIL;
        goto CleanUp;
    }
    
    hwndDeviceWindow = hwndFocusWindow;
                   
    if(!SetForegroundWindow(hwndFocusWindow))
    {
        DWORD error = GetLastError();
        HRESULT hresult = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(error);
        WriteToLog("SetForegroundWindow. hresult= 0x%x", hresult);
        result = RESULT_FAIL;
        goto CleanUp;
    }

    hresult = PrimaryAdapterDevice.InitializeDevice
    (
        NULL,
        g_Application.m_D3DOptions.DriverType,
        NULL,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        0,
	    m_TestFeatureLevel,
	    g_Application.m_pD3D10CreateDevice1
	);
    
	if(FAILED(hresult))
	{
		if
		(
			hresult == DXGI_ERROR_UNSUPPORTED ||
		    hresult == E_NOINTERFACE ||
			hresult == E_NOTIMPL
		)
		{
			WriteToLog("PrimaryAdapterDevice.InitializeDevice - the device not supported. hresult= 0x%x", hresult);
		    result = RESULT_SKIP;
			goto CleanUp;
		}
		else
		{   
			WriteToLog("PrimaryAdapterDevice.InitializeDevice failed. hresult= 0x%x", hresult);
			result = RESULT_FAIL;
			goto CleanUp;
		}
	}

    if(!PrimaryAdapterDevice.GetDevicePtr( &m_pD3D10Device ))
    {
        WriteToLog("PrimaryAdapterDevice.GetDevicePtr failed.");
        result = RESULT_FAIL;
        goto CleanUp;
    }

    if(!PrimaryAdapterDevice.GetFactory( &m_pFactory ))
    {
        WriteToLog("PrimaryAdapterDevice.GetFactory failed.");
        result = RESULT_FAIL;
        goto CleanUp;  
    }

    hresult = m_pFactory -> MakeWindowAssociation( hwndFocusWindow, 0 );
    if(FAILED(hresult))
    {
        WriteToLog("MakeWindowAssociation. hresult= 0x%x", hresult);
        result = RESULT_FAIL;
        goto CleanUp;
    
    }

CleanUp:
    return result;
}

UINT CDXGIWaitVBl::EnumerateModes()
{
    UINT numModes = 0;
    HRESULT hresult;
	DXGI_FACTORY_POINTER  FactoryPointer = {NULL};

    if( FAILED( ( hresult = g_Application.CreateDXGIFactory( &FactoryPointer ) ) ) )
        WriteToLog( "g_Application.CreateDXGIFactory failed in CPresentFS::EnumerateModes.\n" );
    else
    {
        HMODULE hmoduleRefRast = NULL;
        IDXGIAdapter *pAdapter = NULL;
        m_uNumAdapters = 0;

        if( NULL == ( hmoduleRefRast = GetRefRastModulePointer() ) )
            WriteToLog( "EnumerateModes failed.  Unable to retrieve refrast module.\n" );
        else
            while( SUCCEEDED( FactoryPointer.pFactory->EnumAdapters( m_uNumAdapters++, &pAdapter ) ) )
            {
                IDXGIOutput *pOutput;
                UINT uOutput = 0;

                while( SUCCEEDED( ( pAdapter -> EnumOutputs( uOutput++, &pOutput ) ) ) )
                {
                    m_vecTestedModes[m_uNumAdapters - 1][uOutput - 1].clear();

                    m_vecNumOutputs[m_uNumAdapters - 1]++;
                    UINT uiFormatIndex;

                    for( uiFormatIndex = 0; uiFormatIndex < clsDXGIRenderTargetFormats::uiNoOfBBFormats; ++uiFormatIndex )
                    {
                        UINT uiModeCount;
                        DXGI_FORMAT MatchFormat = clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[uiFormatIndex];

                        if( FAILED( ( hresult = pOutput -> GetDisplayModeList( MatchFormat, 0, &uiModeCount, NULL ) ) ) )
                            DebugOut.Write( "GetDisplayModeList (1st call) failed.\n" );
                        else
                            if( 0 == uiModeCount )
                                DebugOut.Write( "Found zero modes.\n" );
                            else
                            {
                                DXGI_MODE_DESC *pModeDescArray = NULL;

                                if( NULL == ( pModeDescArray = new DXGI_MODE_DESC[uiModeCount] ) )
                                    DebugOut.Write( "Out of memory.  Failed to allocate array in CPresentFS::EnumerateModes.\n" );
                                else
                                {
                                    if( FAILED ( ( hresult = pOutput -> GetDisplayModeList( MatchFormat, 0, &uiModeCount, pModeDescArray ) ) ) )
                                        DebugOut.Write( "GetDisplayModeList (2nd call) failed.\n" );
                                    else
                                    {
                                        UINT    uiPattern, uiMode;
                                        UINT    uiTest = 0;

                                        for 
                                        (
                                            uiMode = 0; 
                                            uiMode < uiModeCount; 
                                            uiMode++
                                        )
                                        {
                                            if ( g_Application.GetExecutionPriorityLevel() == 1 )
                                            {
                                                for 
                                                (
                                                    uiPattern = 0;
                                                    uiPattern < ARRAYSIZE(m_ModePriorityPatterns);
                                                    uiPattern++
                                                )
                                                {
                                                    if 
                                                    (
                                                        pModeDescArray[ uiMode ].Width == m_ModePriorityPatterns[uiPattern].uiModeWidth &&
                                                        pModeDescArray[ uiMode ].Height == m_ModePriorityPatterns[uiPattern].uiModeHeight
                                                    )
                                                    {
                                                        break;
                                                    }
                                                }

                                                if ( uiPattern == ARRAYSIZE(m_ModePriorityPatterns) )
                                                {
                                                   continue;
                                                }
                                             }

                                             m_vecTestedModes[m_uNumAdapters - 1][uOutput - 1].insert(
                                                                                    m_vecTestedModes[m_uNumAdapters - 1][uOutput - 1].begin() + uiTest,
                                                                                    pModeDescArray[uiMode]);

                                             uiTest++;
                                             numModes++;
                                        }

                                        delete [] pModeDescArray;
                                    }
                                }
                            }
                    }

                    pOutput -> Release();
                }

                pAdapter -> Release();
            }

        FactoryPointer.pFactory -> Release();
    }

    return numModes;
}

void CDXGIWaitVBl::ReleaseTestCaseObjects()
{
    if( m_pDXGIOutput )
    {
        m_pDXGIOutput -> Release();
        m_pDXGIOutput = NULL;
    }

    if( m_pSwapChain )
    {
        m_pSwapChain -> Release();
        m_pSwapChain = NULL;
    }

    TestedSwapChain.ReleaseAllObjects();
}

bool CDXGIWaitVBl::UpdateOutputWindowInformation()
{
    if( m_pDXGIOutput )
    {
        m_pDXGIOutput -> Release();
        m_pDXGIOutput = NULL;
    }

    if( !m_pSwapChain )
        WriteToLog( "UpdateOutputWindowInformation aborting: Swap chain not initialized.\n" );
    else
    {
        HRESULT hresult;

        if( FAILED( ( hresult = m_pSwapChain -> GetContainingOutput( &m_pDXGIOutput ) ) ) )
            WriteToLog( "UpdateOutputWindowInformation aborting: IDXGISwapChain::GetContainingOutput failed.\n" );
        else
            return true;
    }

    return false;
}

bool CDXGIWaitVBl::SetupDevSCFS( int m_uiModeIndexArg )
{
    HRESULT hresult;

    bool bMatch = false;
    UINT sumModeDescs = 0;
    for( m_uAdapter = 0; !bMatch && m_uAdapter < m_uNumAdapters; m_uAdapter++ )
        for( m_uOutput = 0; !bMatch && m_uOutput < m_vecNumOutputs[m_uAdapter]; m_uOutput++ )
            if( sumModeDescs + m_vecTestedModes[m_uAdapter][m_uOutput].size() > (UINT)m_uiModeIndexArg )
                bMatch = true;
            else
            {
                sumModeDescs += m_vecTestedModes[m_uAdapter][m_uOutput].size();
            }
            m_uOutput--; m_uAdapter--;
            m_uModeIndex = (UINT)m_uiModeIndexArg - sumModeDescs;

            DXGI_MODE_DESC dxgiModeDesc = m_vecTestedModes[m_uAdapter][m_uOutput][m_uModeIndex];

            IDXGIAdapter *pDXGIAdapter = NULL;
            IDXGIOutput *pDXGIOutput = NULL;
            DXGI_OUTPUT_DESC dxgiOutputDesc;
            memset( &dxgiOutputDesc, 0, sizeof( DXGI_OUTPUT_DESC ) );

            // Retrieve the DXGI factory interface pointer.
            if( FAILED( m_pFactory->EnumAdapters( m_uAdapter, &pDXGIAdapter ) ) ||
                FAILED( pDXGIAdapter->EnumOutputs( m_uOutput, &pDXGIOutput ) ) ||
                FAILED( pDXGIOutput->GetDesc( &dxgiOutputDesc ) ) )
                WriteToLog( "Failed to retrieve factory interface pointer in ExecuteTest.\n" );
            else
            {
                pDXGIOutput->Release();
                pDXGIAdapter->Release();

                // Specify the swap chain properties.
                DXGI_SWAP_CHAIN_DESC SCDesc;
                memset( &SCDesc, 0, sizeof( DXGI_SWAP_CHAIN_DESC ) );

                SCDesc.BufferDesc = dxgiModeDesc;

                SCDesc.BufferCount = 1;
                SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                SCDesc.OutputWindow = hwndDeviceWindow;
                SCDesc.SampleDesc.Count = 1;
                SCDesc.SampleDesc.Quality = 0;
                SCDesc.Windowed = FALSE;
                SCDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
                SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

                m_fRefreshRateInHz =
                    (float)(((SCDesc.BufferDesc).RefreshRate).Numerator) /
                    (float)(((SCDesc.BufferDesc).RefreshRate).Denominator);

                WriteToLog("some important fields from DXGI_SWAP_CHAIN_DESC:\n"
                           "    SCDesc.BufferDesc.Width = %u\n"
                           "    SCDesc.BufferDesc.Height = %u\n"
                           "    SCDesc.BufferDesc.Format = %u\n"
                           "    m_fRefreshRateInHz = %f\n"
                           "    SCDesc.BufferDesc.RefreshRate.Numerator = %u\n"
                           "    SCDesc.BufferDesc.RefreshRate.Denominator = %u\n",
                           SCDesc.BufferDesc.Width,
                           SCDesc.BufferDesc.Height,
                           SCDesc.BufferDesc.Format,
                           m_fRefreshRateInHz,
                           SCDesc.BufferDesc.RefreshRate.Numerator,
                           SCDesc.BufferDesc.RefreshRate.Denominator);

                if(!( TestedSwapChain.InitializeSwapChain(&SCDesc)))
                {
                    WriteToLog( "Swap chain initialization failed in ExecuteTestCase.\n" );
                }
                else
                {
                    WriteToLog( "Swap Chain Created. DXGI_MODE_DESC Width=%d Height=%d Refreshrate=%d/%d format=%d scanlineordering=%d scaling=%d ",(SCDesc.BufferDesc).Width, (SCDesc.BufferDesc).Height, ((SCDesc.BufferDesc).RefreshRate).Numerator,((SCDesc.BufferDesc).RefreshRate).Denominator,(SCDesc.BufferDesc).Format,(SCDesc.BufferDesc).ScanlineOrdering,(SCDesc.BufferDesc).Scaling   );
                    WaitAndPumpMessages(100);
                    
                    TestedSwapChain.GetSwapChainPtr( &m_pSwapChain );
                    if( !m_pSwapChain )
                    {
                        WriteToLog( "ExecuteTestCase obtained NULL swap chain pointer.\n" );
                    }
                    else
                    {
                        {
                            const DXGIFormatInfo *pBBFormatInfo = NULL;

                            if( pBBFormatInfo = CFormatInfoMap::Find( ( SCDesc.BufferDesc ).Format ) )
                            {
                                DebugOut.Write( pBBFormatInfo -> szDesc );
                                DebugOut.Write( "\n" );
                            }
                        }

                        // Assign the swap chain to an output.
                        if( !UpdateOutputWindowInformation() )
                            WriteToLog( "UpdateOutputWindowInformation failed.\n" );
                        else
                            if( !m_pDXGIOutput )
                                WriteToLog( "UpdateOutputWindowInformation assigned NULL output pointer.\n" );
                            else
                            {
                                // verify it's the correct output
                                DXGI_OUTPUT_DESC dxgiSwapChainOutputDesc;
                                memset( &dxgiSwapChainOutputDesc, 0, sizeof( DXGI_OUTPUT_DESC ) );
                                if( SUCCEEDED( m_pDXGIOutput->GetDesc( &dxgiSwapChainOutputDesc ) ) )
                                {
                                    if( dxgiSwapChainOutputDesc.Monitor != dxgiOutputDesc.Monitor ||
                                        0 != wcscmp( dxgiSwapChainOutputDesc.DeviceName, dxgiOutputDesc.DeviceName ) )
                                        WriteToLog( "Unexpected DXGIOutput description received from swapchain\n" );
                                    else
                                        return true;
                                }        
                            }
                    }
                }
            }

    return false;
}


void CDXGIWaitVBl::InitTestParameters()
{
	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();

    if( clsQPC::Initialize() != QPC_OK )
	{
        WriteToLog( "No suitable performance counter is available on this system." );

		// @REVIEW [SEdmison]:
		return;
	};


    m_liFrequency = clsQPC::GetFrequency();

    if( m_liFrequency.QuadPart <= 0 )
	{
        WriteToLog( "Performance counter reported an invalid frequency.\n" );

		// @REVIEW [SEdmison]:
		return;
	};

    InitializeWindowClass();

    UINT uiModeCount = EnumerateModes();

    if( uiModeCount > 0 )
        AddParameter( _T("Mode"), &m_uiModeIndex, RangeValueSet( 0, (int)(uiModeCount - 1), 1 ) );
}

TEST_RESULT CDXGIWaitVBl::ExecuteTestCase()
{
    TEST_RESULT test_result = RESULT_PASS;

    if ( SetupDevSCFS( m_uiModeIndex))
    {          
        WaitAndPumpMessages(2000);
        IDXGIOutput *pOutput = NULL;

        float fExpectedInterval, fMinInterval, fMaxInterval, fTotalIntervals = 0.f;
        float fMinMeasuredInterval= 10000.f, fMaxMeasuredInterval= 0.f;
        HRESULT hresult = S_OK;

        LARGE_INTEGER liPreviousReturnTime;
        UINT uiCount, uiFailedIntervalCount = 0;

        fExpectedInterval = 1000.0f / m_fRefreshRateInHz;
        fMinInterval = (fExpectedInterval * (1.0f - MAX_PERCENT_DIFFERENCE)) - 2.0f;
        fMaxInterval = (fExpectedInterval * (1.0f + MAX_PERCENT_DIFFERENCE)) + 2.0f;

        m_pDXGIOutput->WaitForVBlank();
        QueryPerformanceCounter( &liPreviousReturnTime);

        for ( uiCount = 0; uiCount < NO_OF_TESTED_INTERVALS; ++uiCount)
        {
            LARGE_INTEGER liCurrentReturnTime;
            float fMeasuredInterval;

            HRESULT hresult =  m_pSwapChain->Present(1, 0);
            if (FAILED(hresult))
            {
                WriteToLog("Initial Present Failed. hresult= 0x%x", hresult);
            }

            QueryPerformanceCounter( &liCurrentReturnTime);
            fMeasuredInterval =
                ((float)(liCurrentReturnTime.QuadPart - liPreviousReturnTime.QuadPart)) * 1000.0f /
                (float)(m_liFrequency.QuadPart);

            if ( uiCount )
            {
                if ( fMeasuredInterval < fMinMeasuredInterval)
                {
                    fMinMeasuredInterval = fMeasuredInterval;
                }
                if ( fMeasuredInterval > fMaxMeasuredInterval)
                {
                    fMaxMeasuredInterval = fMeasuredInterval;
                }
            }
            else
            {
                fMaxMeasuredInterval = fMinMeasuredInterval = fMeasuredInterval;
            }

            fTotalIntervals += fMeasuredInterval;
            m_fMeasuredIntervals[uiCount] = fMeasuredInterval;

            if ( fMeasuredInterval < fMinInterval || fMeasuredInterval > fMaxInterval)
            {
                ++uiFailedIntervalCount;
                WriteToLog( "Failure[%u times]: the measured interval[%f] vs expected interval[Min, Standard, Max][%f, %f, %f]\n", 
                            uiFailedIntervalCount, fMeasuredInterval, fMinInterval, fExpectedInterval, fMaxInterval);
            }
            liPreviousReturnTime = liCurrentReturnTime;
        }

        // we only test this for HW
        if ( uiFailedIntervalCount > MAX_FAILING_INTERVALS && D3D10_DRIVER_TYPE_HARDWARE == g_Application.m_D3DOptions.DriverType )
        {
            //
            // compromise to the windows 7 bug #378766 [X86/X64/WOW64]WaitForVBlank() for D3D9 - the measured intervals 
            // for VBlank sometimes deviated significantly from the expected value.
            //
            WriteToLog( "[Warning]: %d out of %d measured intervals deviated significantly from the expected value.\n" 
                        "If the test case passes, the IHV should check if a wrong refresh rate was reported.\n",
                        uiFailedIntervalCount, 
                        NO_OF_TESTED_INTERVALS);

            float fAverageInterval = fTotalIntervals / NO_OF_TESTED_INTERVALS;
            if ( fAverageInterval < fMinInterval || fAverageInterval > fMaxInterval)
            {
                //
                // Compromise to the bugs similar to windows7 bug 43807: fail all the intervals because the refresh rate
                // was changed to the wrong value by IHV driver.
                // if the measured value deviates from the average value not too large, let it pass
                //
                float fMinAverageInterval = fAverageInterval * 0.67f;
                float fMaxAverageInterval = fAverageInterval * 1.33f;

                uiFailedIntervalCount = 0;
                for ( uiCount = 0; uiCount < NO_OF_TESTED_INTERVALS; ++uiCount)
                {
                    if ( m_fMeasuredIntervals[uiCount] < fMinAverageInterval || m_fMeasuredIntervals[uiCount] > fMaxAverageInterval )
                    {
                        ++uiFailedIntervalCount;
                        WriteToLog( "Failure[%u times]: the measured interval[%f] vs expected average interval[Min, Standard, Max][%f, %f, %f]\n", 
                                    uiFailedIntervalCount, m_fMeasuredIntervals[uiCount], fMinAverageInterval, fAverageInterval, fMaxAverageInterval);
                    }
                }

                if ( uiFailedIntervalCount > MAX_FAILING_INTERVALS )
                {
                    if (++m_FailedCases > 2) 
                    {
                        WriteToLog( "The measured intervals deviated significantly from the expected value.\n");
                        WriteToLog( "Expected interval: %.4f; Average measured interval: %.4f\n", fExpectedInterval, fAverageInterval);
                        WriteToLog( "Minimum Measured Interval: %.4f; Maximum Measured Interval: %.4f\n", fMinMeasuredInterval, fMaxMeasuredInterval);

                        DXGI_SWAP_CHAIN_DESC desc;
                        m_pSwapChain->GetDesc(&desc);

                        WriteToLog( "Test Failed: Adapter = %d, Output = %d Width = %d, Height = %d, Format = %d, Refreshrate=%d/%d\n",
                            m_uAdapter, m_uOutput,
                            desc.BufferDesc.Width, desc.BufferDesc.Height, desc.BufferDesc.Format,
                        desc.BufferDesc.RefreshRate.Numerator, desc.BufferDesc.RefreshRate.Denominator);
                    
                        test_result = RESULT_FAIL;
                    }
                }
            }
        }
    }
    else
    {
        test_result = RESULT_SKIP;
        WriteToLog( "SetupDevSCFS Failed");
    }

    WaitAndPumpMessages(100);
    ReleaseTestCaseObjects();
    return test_result;
}


HRESULT CDXGIWaitVBl::WaitAndPumpMessages(DWORD duration)
{
    DWORD startTime = timeGetTime();
    DWORD currentTime = 0;
    for
    (
        currentTime = timeGetTime();
        currentTime <= startTime + duration;
        currentTime = timeGetTime()
    )
    {
        MSG                msg;    
        while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );

            // If this is a quit message, re-post it.
            if( WM_QUIT == msg.message )
            {
                PostQuitMessage( (int)msg.wParam );
            }
        }
    }

    return S_OK;
}

void CDXGIWaitVBl::Cleanup()
{
    WriteToLog("CleanUp");
    if( hwndDeviceWindow )
    {
        if( hwndDeviceWindow != hwndFocusWindow )
            DestroyWindow( hwndDeviceWindow );
        hwndDeviceWindow = NULL;
    }

    if( hwndFocusWindow )
    {
        DestroyWindow( hwndFocusWindow );
        hwndFocusWindow = NULL;
    }

    if( m_pFactory )
    {
        HWND hwndCurrentFocus = NULL;

        m_pFactory -> GetWindowAssociation(&hwndCurrentFocus);
        if (hwndCurrentFocus)
        {
            m_pFactory -> MakeWindowAssociation(NULL, 0);
        }
        m_pFactory -> Release();
        m_pFactory = NULL;
    }

    

    if( m_pD3D10Device )
    {
        m_pD3D10Device -> Release();
        m_pD3D10Device = NULL;
    }

    PrimaryAdapterDevice.ReleaseAllObjects();
}



////////////////////////////////////////////////////////////////////////////////
// clsTestableDevice Methods

clsTestableDevice::~clsTestableDevice()
{
}

bool clsTestableDevice::GetDevicePtr( ID3D10Device **pDevicePointer )
{
    if( pD3D10Device )
        pD3D10Device -> AddRef();
    *pDevicePointer = pD3D10Device;
    return true;
}



////////////////////////////////////////////////////////////////////////////////
// clsTestableDXGISwapChain Methods

clsTestableDXGISwapChain::~clsTestableDXGISwapChain()
{
}

bool clsTestableDXGISwapChain::GetSwapChainPtr( IDXGISwapChain **m_pSwapChainPointer )
{
    if( pSwapChain )
        pSwapChain -> AddRef();
    *m_pSwapChainPointer = pSwapChain;
    return true;
}
