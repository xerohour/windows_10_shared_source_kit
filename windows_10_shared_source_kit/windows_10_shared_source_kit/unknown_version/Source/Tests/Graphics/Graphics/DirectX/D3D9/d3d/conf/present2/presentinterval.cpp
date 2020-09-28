// PresentInterval.cpp: implementation of the CPresentInterval class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentInterval.h"
#include "CSwapChain.h"
#include "CSurface.h"
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPresentInterval::CPresentInterval()
{
	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    m_Options.D3DOptions.bReference         = false;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.bZBuffer			= false;
	m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;
    m_Options.D3DOptions.dwPresentInterval	= D3DPRESENT_INTERVAL_DEFAULT;
    m_Options.D3DOptions.dwPresentFlags = 0;


    m_vecPresentIntervals.clear();
    m_vecSwapEffects.clear();

    m_fRefreshInterval = 0.0f;
    
    
    m_fAllowedVariancePrecent = .1f;
	m_LastExpectedRefreshInterval = 0;

}

//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CPresentInterval::CommandLineHelp()
{
    WriteCommandLineHelp(_T("-Variance:<float> Change the allowed variance for Present intervals to Refresh Rate intervals.\n"));
    WriteCommandLineHelp(_T("-TestIntervals:<int> Change the number of Present samples\n"));
}

//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CPresentInterval::Check_HRFail( HRESULT hResult, const char *sMethod )
{
	if( FAILED(hResult) )
	{
		WriteToLog( TEXT( "%s failed, error code %s (0x%x)\n" ),
				sMethod,
				m_pD3D->HResultToString( hResult ),
				hResult );
		Fail();
		throw hResult;
	}
}

bool CPresentInterval::CapsCheck()
{
	HRESULT hr;

	// windowed tests cannot support -MultiHead
	if( KeySet( _T( "MultiHead" ) ) && !m_bFullscreen )
    {
        WriteToLog(_T("Windowed tests don't support -MultiHead"));
		return false;
    }

	// go back windowed if desired
	if( !m_bFullscreen )
	{
        DWORD behaviors = m_pSrcDevice->GetBehaviors();
        m_pSrcDevice->SetBehaviors(behaviors & ~DEVICECREATE_ADAPTERGROUP_DEVICE);
        
        PRESENT_PARAMETERS presentationParameters, *pPresentParameters = m_pSrcDevice->GetPresParams();
		memcpy( &presentationParameters, pPresentParameters, sizeof( PRESENT_PARAMETERS ) );
		presentationParameters.bWindowed = true;
        presentationParameters.uFullscreenRefresh = D3DPRESENT_RATE_DEFAULT;

        

		if( FAILED( hr = m_pSrcDevice->UpdateDevice( &presentationParameters ) ) )
		{
			WriteToLog( _T( "UpdateDevice failed, return code %s (0x%x)\n" ),
							m_pD3D->HResultToString( hr ),
							hr );
			return false;
		}
	}

	CAPS DeviceCaps;
	if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&DeviceCaps) ) )
	{
		WriteToLog(
				_T("Error: GetDeviceCaps failed: %s (0x%x). Some tests might show inaccurate results\n"),
				m_pD3D->HResultToString(hr),
				hr 
			);
		return false;
	}
    
	DWORD presentIntervals[] = 
	{
		PRESENT_INTERVAL_ONE,
		PRESENT_INTERVAL_TWO,
		PRESENT_INTERVAL_THREE,
		PRESENT_INTERVAL_FOUR,
		PRESENT_INTERVAL_IMMEDIATE
	};

    m_vecPresentIntervals.clear();
	for( int i = 0; i < sizeof( presentIntervals ) / sizeof DWORD; i++ )
	{
		if( !(DeviceCaps.PresentationIntervals & presentIntervals[i]) )
			continue;

        if( false == m_bFullscreen )
        {
            switch(presentIntervals[i]) 
            {
                case PRESENT_INTERVAL_DEFAULT:
                case PRESENT_INTERVAL_IMMEDIATE:
                case PRESENT_INTERVAL_ONE:
                    break;

                default:
                    continue;
            }
        }

        m_vecPresentIntervals.push_back(presentIntervals[i]);
    }

    if( 0 == m_vecPresentIntervals.size() )
    {
        WriteToLog(_T("No PresentationIntervals found.\n"));
        return true;
    }

	return true;
}

UINT CPresentInterval::TestInitialize()
{
    timeBeginPeriod(1);
    SetTestParameters();

	return CD3DTest::TestInitialize();  
}

void CPresentInterval::SetTestParameters()
{

    m_vecSwapEffects.push_back(SWAPEFFECT_FLIP);
    m_vecFlagsEx.push_back(0);
    m_vecFlagInterval.clear();
    m_nTests = m_vecPresentIntervals.size() * m_vecSwapEffects.size() * m_vecFlagsEx.size();
    SetTestRange( 1, m_nTests );

}

char* CPresentInterval::INTERVAL2STRING(UINT uInterval)
{
	static char sInterval[256] = "";

	switch( uInterval )
	{
	case PRESENT_INTERVAL_DEFAULT:
		strcpy( sInterval, "PRESENT_INTERVAL_DEFAULT" );
		break;
	case PRESENT_INTERVAL_ONE:
		strcpy( sInterval, "PRESENT_INTERVAL_ONE" );
		break;
	case PRESENT_INTERVAL_TWO:
		strcpy( sInterval, "PRESENT_INTERVAL_TWO" );
		break;
	case PRESENT_INTERVAL_THREE:
		strcpy( sInterval, "PRESENT_INTERVAL_THREE" );
		break;
	case PRESENT_INTERVAL_FOUR:
		strcpy( sInterval, "PRESENT_INTERVAL_FOUR" );
		break;
	case PRESENT_INTERVAL_IMMEDIATE:
		strcpy( sInterval, "PRESENT_INTERVAL_IMMEDIATE" );
		break;
	default:
		strcpy( sInterval, "Unknown" );
	}

	return sInterval;
}

char* CPresentInterval::SWAPEFFECT2STRING(UINT uSwapEffect)
{
	static char sSwapEffect[256] = "";

	switch( uSwapEffect )
	{
	case SWAPEFFECT_FLIP:
		strcpy( sSwapEffect, "SWAPEFFECT_FLIP" );
		break;
	case SWAPEFFECT_DISCARD:
		strcpy( sSwapEffect, "SWAPEFFECT_DISCARD" );
		break;
	case SWAPEFFECT_COPY:
		strcpy( sSwapEffect, "SWAPEFFECT_COPY" );
		break;
    case SWAPEFFECT_FLIPEX:
		strcpy( sSwapEffect, "SWAPEFFECT_FLIPEX" );
		break;
	}

	return sSwapEffect;
}

bool CPresentInterval::ExecuteTest(UINT uTestNumber)
{
    float fPresentInterval = 0.0f;
    DWORD dwStartTime = 0;
    char szTestName[256];
    
    UINT uPresentIntervalIndex;
    UINT uSwapEffectIndex;
    UINT flagsExIndex;
    UINT flagsIntervalIndex;

    UINT nTempIndex = m_nTests / m_vecPresentIntervals.size();
    UINT nTempTest = uTestNumber - 1;
    uPresentIntervalIndex = 1 * ( nTempTest / nTempIndex );
    
    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecSwapEffects.size();
    uSwapEffectIndex = nTempTest / nTempIndex;
   

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecFlagsEx.size();
    flagsExIndex = nTempTest / nTempIndex;

    if(m_vecFlagInterval.size())
    {
        nTempTest = nTempTest % nTempIndex;
        nTempIndex = nTempIndex / m_vecFlagInterval.size();
        flagsIntervalIndex = nTempTest / nTempIndex;
    }

    UINT uPresentInterval = m_vecPresentIntervals[uPresentIntervalIndex];
    UINT uSwapEffect = m_vecSwapEffects[uSwapEffectIndex];
    
    if(m_vecFlagsEx[flagsExIndex] == D3DPRESENT_FORCEIMMEDIATE)
    {
        sprintf(szTestName,"%s %s with D3DPRESENT_FORCEIMMEDIATE. Present w/ flag ratio %d to Present w/o flag 1",INTERVAL2STRING(uPresentInterval),SWAPEFFECT2STRING(uSwapEffect), m_vecFlagInterval[flagsIntervalIndex] -1 );  
    }
    else if(m_vecFlagsEx[flagsExIndex] == D3DPRESENT_DONOTFLIP)
    {
        sprintf(szTestName,"%s %s with D3DPRESENT_DONOTFLIP. Present w/ flag ratio %d to Present w/o flag 1",INTERVAL2STRING(uPresentInterval),SWAPEFFECT2STRING(uSwapEffect), m_vecFlagInterval[flagsIntervalIndex] -1 );  
    }
    else
    {
        sprintf(szTestName,"%s %s.",INTERVAL2STRING(uPresentInterval),SWAPEFFECT2STRING(uSwapEffect));  
    }

    BeginTestCase( szTestName, uTestNumber );

    DISPLAYMODE d3ddisplaymodeAdapter;
    DEVICE_CREATION_PARAMETERS dcpSrcDevice;

    if( FAILED(m_pSrcDevice->GetCreationParameters(&dcpSrcDevice)) )
        return D3DTESTINIT_ABORT;

    if( FAILED(m_pD3D->GetAdapterDisplayMode(dcpSrcDevice.AdapterOrdinal, &d3ddisplaymodeAdapter)) )
        return D3DTESTINIT_ABORT;

    m_fRefreshInterval = 1000.0f / (float)(d3ddisplaymodeAdapter.dwRefreshRate);

    if( 0.0f == m_fRefreshInterval )
    {
        WriteToLog(TEXT("Unable to get refreshrate"));
        return D3DTESTINIT_ABORT;
    }

    try
    {       
        if(m_vecFlagsEx[flagsExIndex] == D3DPRESENT_FORCEIMMEDIATE)
        {
            Check_HRFail(m_pSrcDevice->SetMaximumFrameLatency(m_nMaximumFrameLatency),_T("SetMaximumFrameLatency "));
        }
        
        UINT uAdapterCount = 0;
        if ( ( m_pSrcDevice->GetBehaviors() & DEVICECREATE_ADAPTERGROUP_DEVICE ) == DEVICECREATE_ADAPTERGROUP_DEVICE )
	    {
            uAdapterCount = m_pSrcDevice->GetCaps()->NumberOfAdaptersInGroup;
        }
        else
        {
            uAdapterCount = 1;   
        }

        // Update the present interval
        PRESENT_PARAMETERS *pd3dpp;
        pd3dpp = new PRESENT_PARAMETERS[uAdapterCount];
        if( NULL == pd3dpp )
        {
            Abort();
            GetLastError();
            return false;
        }

        memset( pd3dpp, 0, uAdapterCount * sizeof( PRESENT_PARAMETERS ) );

        memcpy( pd3dpp, m_pSrcDevice->GetPresParams(), uAdapterCount * sizeof(PRESENT_PARAMETERS) );
        for( UINT uAdapter = 0; uAdapter < uAdapterCount; uAdapter++ )
        {
            pd3dpp[uAdapter].uPresentInterval = uPresentInterval;
            pd3dpp[uAdapter].SwapEffect = uSwapEffect;
            pd3dpp[uAdapter].uBackBufferCount = m_BackBufferCount;
            pd3dpp[uAdapter].BackBufferFormat = FMT_A8R8G8B8;

        }

 
		//
		// Wait for previously queued presents to clear.
		//
		if( uTestNumber != 1 && uSwapEffect == SWAPEFFECT_FLIPEX)
		{
			Sleep((UINT)((float)m_BackBufferCount * m_LastExpectedRefreshInterval));
		}

		HRESULT hResult;
        // 598319	Present2_FullScreen:  Reset fails with D3DERR_INVALIDCALL on Intel parts
        if( FAILED( hResult = m_pSrcDevice->Reset( pd3dpp ) ) )
        {
            WriteToLog( _T( "CPresentInterval::ExecuteTest: Reset failed, return code %s (0x%x)\n" ),
                m_pD3D->HResultToString( hResult ), hResult );

            Skip();
            delete[] pd3dpp;
            return false;
        }
        delete[] pd3dpp;

		
        if( uTestNumber == 1)
        {
            Sleep(5000);
        }
        
        UINT *pFrameTime = new UINT[m_nTestIntervals];
        
        //
        // Windowed flipex tests
        //
        if(!m_bFullscreen)
        {   
            //
            // Need the swapchain to test D3DPRESENT_FORCEIMMEDIATE flag on present
            //
            LPSWAPCHAIN pSwapChain = NULL;
            Check_HRFail( m_pSrcDevice->GetSwapChain( 0, &pSwapChain ), "GetSwapChain" );
			Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL ), _T("Present"));
            Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, m_vecFlagsEx[flagsExIndex] ), _T("Swapchain Present"));

            for(UINT i = 0; i < m_nTestIntervals; ++i)
            {
                
                if( i < m_nTestIntervals - m_BackBufferCount )
                {
                    Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, 0xff990000, 1.f, 0 ), _T("Clear") );
                }
                else
                {
                    Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, 0xffffffff, 1.f, 0 ), _T("Clear") );
                }

                UINT dwStartTime = timeGetTime();
                if(m_vecFlagInterval.size() && ( i % m_vecFlagInterval[flagsIntervalIndex] == 0 ))
                {
                    if( m_pSrcDevice->GetInterface() <= 9.0f )
                    {
                        m_pSrcDevice->Present( NULL, NULL, NULL, NULL);
                    }
                    else
                    {
                        m_pSrcDevice->PresentEx( NULL, NULL, NULL, NULL, 0 );
                    }
                }
                else
                {
                    if( m_pSrcDevice->GetInterface() <= 9.0f )
                    {
                        m_pSrcDevice->Present( NULL, NULL, NULL, NULL);
                    }
                    else
                    {
                        m_pSrcDevice->PresentEx( NULL, NULL, NULL, NULL, m_vecFlagsEx[flagsExIndex] );
                    }
                }

                pFrameTime[i] = timeGetTime() - dwStartTime;
            }
            RELEASE( pSwapChain );
        }
        else
        {
            Check_HRFail( m_pSrcDevice->Present( NULL, NULL, NULL, NULL ), _T("Present"));

            for(UINT i = 0; i < m_nTestIntervals; ++i)
            {
                dwStartTime = timeGetTime();
                if( m_pSrcDevice->GetInterface() <= 9.0f )
                {
                    m_pSrcDevice->Present( NULL, NULL, NULL, NULL);
                }
                else
                {
                    m_pSrcDevice->PresentEx( NULL, NULL, NULL, NULL,  m_vecFlagsEx[flagsExIndex] );
                }
                pFrameTime[i] = timeGetTime() - dwStartTime;
            }
        }

        float expectedRefreshInterval = m_fRefreshInterval; 
        UINT interval = uPresentInterval;

        if(0 != m_nTestIntervals)
        {
            float adjustedTotalTime = 0;
            for(UINT i = 0; i < m_nTestIntervals; ++i)
            {
                WriteToLog("FrameTime[%d]=%d ms",i, pFrameTime[i]);
                adjustedTotalTime += pFrameTime[i];
            }
            
            UINT adjustedTestIntervals = m_nTestIntervals; 
            
            if(m_vecFlagInterval.size() &&
                m_vecFlagInterval[flagsIntervalIndex] > 0 &&
                interval != PRESENT_INTERVAL_IMMEDIATE &&
                m_vecFlagsEx.size())
            {
                //
                // Remove the first m_BackBufferCount frames of data
				//
                adjustedTestIntervals = adjustedTestIntervals - m_BackBufferCount;         		        
                
                if(m_vecFlagsEx[flagsExIndex] == D3DPRESENT_FORCEIMMEDIATE)
                {
                   
                    //
                    // Adjust the expected interval for the number of m_vecFlagsEx[flagsExIndex].  Add in 1 for each D3DPRESENT_FORCEIMMEDIATE frame
                    //
                    expectedRefreshInterval = (expectedRefreshInterval / m_vecFlagInterval[flagsIntervalIndex]) + ( m_vecFlagInterval[flagsIntervalIndex] / adjustedTestIntervals ) ;
                    WriteToLog("%d %d %d",expectedRefreshInterval , m_vecFlagInterval[flagsIntervalIndex], expectedRefreshInterval / m_vecFlagInterval[flagsIntervalIndex]);

                    if ( m_vecFlagInterval[flagsIntervalIndex] >=  m_nTestIntervals )
                    {
                        interval = PRESENT_INTERVAL_IMMEDIATE; 
                        if(expectedRefreshInterval < m_fRefreshInterval)
                        {
                            expectedRefreshInterval = m_fRefreshInterval;  
                        }
                    }
                }
            }

            fPresentInterval = adjustedTotalTime / ((float)adjustedTestIntervals);
            WriteToLog("Measured PresentInterval=%f (%f/%f) expectedRefreshInterval %f", fPresentInterval, adjustedTotalTime, (float)adjustedTestIntervals, expectedRefreshInterval);
        }

        delete[] pFrameTime;

		m_LastExpectedRefreshInterval = expectedRefreshInterval;

        if( false == VerifyPresentInterval(
            fPresentInterval,
            expectedRefreshInterval,
            interval))
        {
           Fail();
        }
    }
    catch (...)
    {
        WriteToLog (_T("Unhandled exception or API failed\n"));
    }

    GetLastError();
    return false;
}

bool CPresentInterval::VerifyPresentInterval(
    float measuredPresentInterval,
    float expectedPresentInterval,
    UINT interval
)
{
    float fMinInterval = 0.0f;
    float fMaxInterval = 0.0f;
    float adjustedRefreshInterval = expectedPresentInterval;

    //
    // Assumption is immediate should return alot faster than the refresh rate.
    // This really should be tighter around .1 or .2. 
    //
    float adjustForImmediate = .5f;
    
    if( interval == PRESENT_INTERVAL_IMMEDIATE)
    {
        adjustedRefreshInterval = (expectedPresentInterval * adjustForImmediate);
        fMinInterval = 0;
        fMaxInterval =  adjustedRefreshInterval + (m_fAllowedVariancePrecent * adjustedRefreshInterval);
    }
    else if(interval == PRESENT_INTERVAL_DEFAULT || 
        interval == PRESENT_INTERVAL_ONE)
    {
        
        fMinInterval = adjustedRefreshInterval -  (m_fAllowedVariancePrecent * adjustedRefreshInterval);
        fMaxInterval = adjustedRefreshInterval +  (m_fAllowedVariancePrecent * adjustedRefreshInterval);
    }
    else if(interval == PRESENT_INTERVAL_TWO)
    {
        adjustedRefreshInterval = (adjustedRefreshInterval * 2.0f);
        fMinInterval = adjustedRefreshInterval - (m_fAllowedVariancePrecent * adjustedRefreshInterval);
        fMaxInterval = adjustedRefreshInterval + (m_fAllowedVariancePrecent * adjustedRefreshInterval);
    }
    else if(interval == PRESENT_INTERVAL_THREE)
    {
        adjustedRefreshInterval = (adjustedRefreshInterval * 3.0f);
        fMinInterval = adjustedRefreshInterval - (m_fAllowedVariancePrecent * adjustedRefreshInterval);
        fMaxInterval = adjustedRefreshInterval + (m_fAllowedVariancePrecent * adjustedRefreshInterval);
    }
    else if(interval == PRESENT_INTERVAL_FOUR)
    {
        adjustedRefreshInterval = (adjustedRefreshInterval * 4.0f);
        fMinInterval = adjustedRefreshInterval - (m_fAllowedVariancePrecent * adjustedRefreshInterval);
        fMaxInterval = adjustedRefreshInterval + (m_fAllowedVariancePrecent * adjustedRefreshInterval);
    }


    WriteToLog(_T("Measured PresentInterval = %.4f. MinInterval= %.4f. MaxInterval= %.4f. Expected Refresh rate interval = %.4f \n"),
        measuredPresentInterval, fMinInterval, fMaxInterval, adjustedRefreshInterval);
         
    if( measuredPresentInterval < fMinInterval )
    {
        WriteToLog(_T("Present was faster then the allowed refresh rate interval.\n"));
        return false;
    }

    if( measuredPresentInterval > fMaxInterval )
    {
        WriteToLog(_T("Present was slower then the allowed refresh rate interval.\n"));
        return false;
    }
    
    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:	CanUseFlipEx CapsCheck()
// Desc:	check caps for FlipEx
//////////////////////////////////////////////////////////////////////
bool CPresentInterval::CanUseFlipEx()
{
    bool retVal = false;

   // this test cannot be run on REF;
    retVal = ( D3DDEVTYPE_HAL == m_pSrcDevice->GetDevType() );
    if(retVal == false)
    {
        WriteToLog("FlipEx not supported on Ref");
    }

    OSVERSIONINFO osVer;
	ZeroMemory( &osVer, sizeof OSVERSIONINFO );
	osVer.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
	GetVersionEx( &osVer );
	
    if( ((osVer.dwMajorVersion == 6) && (osVer.dwMinorVersion < 1)) 
            || (osVer.dwMajorVersion != 6 && osVer.dwMajorVersion < 7))
    {
        WriteToLog("FlipEx not supported for Os versions < 6.1  Os Version=%d.%d", osVer.dwMajorVersion, osVer.dwMinorVersion);
        retVal = false;   
    }
    
    if( m_pSrcDevice->GetInterface() < 9.1f )
	{
        WriteToLog("FlipEx not supported for Dx versions < 9.1.  Dx=%d", m_pSrcDevice->GetInterface());
        retVal = false;  
    }

    return retVal;
}

bool CPresentInterval::TestTerminate(void)
{
    timeEndPeriod(1);
	return CD3DTest::TestTerminate();  
}
