// FullScreenPresent.cpp: implementation of the CFullScreenPresent class.
//
//////////////////////////////////////////////////////////////////////

#include "FullScreenPresent.h"
#include <StrSafe.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFullScreenPresent::CFullScreenPresent()
{
	m_TestType		= TEST_FULLSCREENPRESENT;
	m_szCommandKey	= _T("FullScreenPresent");
	m_szTestName	= _T("FullScreenPresent");

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
}

CFullScreenPresent::~CFullScreenPresent()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CFullScreenPresent::Name( char* szName )
{
	StringCchPrintf( szName, 255, "%s (%s,%d)", 
        m_szTestName, 
	    FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ), 
	    m_nIndexPresentParameters );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CFullScreenPresent::InitPresentParameters()
{
    PRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    GetFrontBufferRect();

    m_vecPresentParameters.clear();
    m_vecPresentParametersRotation.clear();


    d3dpp.bWindowed						= false;
    d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
    d3dpp.bAutoDepthStencil				= false;

    d3dpp.bCachedBackBuffers			= false;
    d3dpp.DesktopFormat					= m_fmtFrontBuffer;

    FMT backbufferFormats[] = 
    {
        FMT_X8R8G8B8,
#ifndef FAST_TEST
        FMT_X8B8G8R8,
        FMT_R5G6B5,
        FMT_X1R5G5B5,
        FMT_A2R10G10B10,
        FMT_A2B10G10R10_XR_BIAS,
        FMT_A16B16G16R16F
#endif
    };

#ifdef FAST_TEST
	POINT bbSize[] =
	{
		{ m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top },
	};
#endif


    FORMAT lastFormat = FMT_ANY;


    for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
    {
        // cycle through all display modes
        UINT		nMode = 0;
        DISPLAYMODE displayMode;
        while( SUCCEEDED( m_pD3D->EnumAdapterModes( m_pSrcDevice->GetAdapterID(), backbufferFormats[iFormat], nMode++, &displayMode ) ) )
        {
#ifdef FAST_TEST
			bool validDisplayMode = false;
			for (UINT bbIndex = 0; bbIndex < ARRAYSIZE(bbSize); bbIndex++)
			{
				if (displayMode.dwWidth == bbSize[0].x && displayMode.dwHeight == bbSize[0].y)
				{
					validDisplayMode = true;
					break;
				}
			}
			if (!validDisplayMode) continue;
#endif
            
            // skip interlaced modes if required
            if( displayMode.dwRefreshRate < 60 && KeySet( _T( "NoInterlaced" ) ) )
                continue;

            d3dpp.hDeviceWindow	= m_hWndReset;
            d3dpp.uWidth					= displayMode.dwWidth;
            d3dpp.uHeight					= displayMode.dwHeight;
            d3dpp.uFullscreenRefresh		= displayMode.dwRefreshRate;
            d3dpp.BackBufferFormat			= displayMode.Format;

            // cycle through backbuffers count
            d3dpp.uBackBufferCount			= 1;

            // cycle through swapeffects
            DWORD swapEffects[] = 
            {
                SWAPEFFECT_DISCARD,
#ifndef FAST_TEST
                SWAPEFFECT_FLIP,
                SWAPEFFECT_COPY
#endif
            };
            for( int iSwapEffect = 0; iSwapEffect < sizeof( swapEffects ) / sizeof DWORD; iSwapEffect++ )
            {
                if( d3dpp.uBackBufferCount > 1 && SWAPEFFECT_COPY == swapEffects[iSwapEffect] )
                    continue;

                // verify command line restrictions
                if( KeySet(_T("discard")) && SWAPEFFECT_DISCARD != swapEffects[iSwapEffect] )
                    continue;
                if( KeySet(_T("flip")) && SWAPEFFECT_FLIP != swapEffects[iSwapEffect] )
                    continue;
                if( KeySet(_T("copy")) && SWAPEFFECT_COPY != swapEffects[iSwapEffect] )
                    continue;

                d3dpp.SwapEffect				= swapEffects[iSwapEffect];

                // cycle through present intervals
                DWORD presentIntervals[] = 
                {
                    PRESENT_INTERVAL_DEFAULT,
#ifndef FAST_TEST
                    PRESENT_INTERVAL_ONE,
                    PRESENT_INTERVAL_TWO,
                    PRESENT_INTERVAL_THREE,
                    PRESENT_INTERVAL_FOUR,
                    PRESENT_INTERVAL_IMMEDIATE
#endif
                };
                for( int iPresentInterval = 0; iPresentInterval < sizeof( presentIntervals ) / sizeof DWORD; iPresentInterval++ )
                {
                    if( PRESENT_INTERVAL_DEFAULT != presentIntervals[iPresentInterval] &&
                        !(m_deviceCaps.PresentationIntervals & presentIntervals[iPresentInterval])
                        )
                        continue;

                    // verify command line restrictions
                    if( KeySet(_T("interval_default")) && PRESENT_INTERVAL_DEFAULT != presentIntervals[iPresentInterval] )
                        continue;
                    if( KeySet(_T("interval_one")) && PRESENT_INTERVAL_ONE != presentIntervals[iPresentInterval] )
                        continue;
                    if( KeySet(_T("interval_two")) && PRESENT_INTERVAL_TWO != presentIntervals[iPresentInterval] )
                        continue;
                    if( KeySet(_T("interval_three")) && PRESENT_INTERVAL_THREE != presentIntervals[iPresentInterval] )
                        continue;
                    if( KeySet(_T("interval_four")) && PRESENT_INTERVAL_FOUR != presentIntervals[iPresentInterval] )
                        continue;
                    if( KeySet(_T("interval_immediate")) && PRESENT_INTERVAL_IMMEDIATE != presentIntervals[iPresentInterval] )
                        continue;

                    d3dpp.uPresentInterval = presentIntervals[iPresentInterval];

                    // cycle through multisample types
                    MULTISAMPLE_TYPE multisampleTypes[] = 
                    {
                        MULTISAMPLE_NONE,
#ifndef FAST_TEST
                        MULTISAMPLE_NONMASKABLE,
                        MULTISAMPLE_2_SAMPLES,
                        MULTISAMPLE_3_SAMPLES,
                        MULTISAMPLE_4_SAMPLES,
                        MULTISAMPLE_5_SAMPLES,
                        MULTISAMPLE_6_SAMPLES,
                        MULTISAMPLE_7_SAMPLES,
                        MULTISAMPLE_8_SAMPLES,
                        MULTISAMPLE_9_SAMPLES,
                        MULTISAMPLE_10_SAMPLES,
                        MULTISAMPLE_11_SAMPLES,
                        MULTISAMPLE_12_SAMPLES,
                        MULTISAMPLE_13_SAMPLES,
                        MULTISAMPLE_14_SAMPLES,
                        MULTISAMPLE_15_SAMPLES,
                        MULTISAMPLE_16_SAMPLES,
#endif
                    };

                    for( int iMultisampleType = 0; iMultisampleType < sizeof( multisampleTypes ) / sizeof MULTISAMPLE_TYPE; iMultisampleType++ )
                    {
                        if( KeySet( _T( "multisample_none" ) ) && MULTISAMPLE_NONE != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_nonmaskable" ) ) && MULTISAMPLE_NONMASKABLE != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_2" ) ) && MULTISAMPLE_2_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_3" ) ) && MULTISAMPLE_3_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_4" ) ) && MULTISAMPLE_4_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_5" ) ) && MULTISAMPLE_5_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_6" ) ) && MULTISAMPLE_6_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_7" ) ) && MULTISAMPLE_7_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_8" ) ) && MULTISAMPLE_8_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_9" ) ) && MULTISAMPLE_9_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_10" ) ) && MULTISAMPLE_10_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_11" ) ) && MULTISAMPLE_11_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_12" ) ) && MULTISAMPLE_12_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_13" ) ) && MULTISAMPLE_13_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_14" ) ) && MULTISAMPLE_14_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_15" ) ) && MULTISAMPLE_15_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;
                        if( KeySet( _T( "multisample_16" ) ) && MULTISAMPLE_16_SAMPLES != multisampleTypes[iMultisampleType] )
                            continue;

                        if( FAILED( m_pD3D->CheckDeviceMultiSampleType(	
                            m_pSrcDevice->GetAdapterID(),
                            m_pSrcDevice->GetDevType(),
                            d3dpp.BackBufferFormat,
                            d3dpp.bWindowed,
                            multisampleTypes[iMultisampleType]
                        )
                            )
                            )
                            continue;

                        if( d3dpp.dwFlags & PRESENTFLAG_LOCKABLE_BACKBUFFER && MULTISAMPLE_NONE != multisampleTypes[iMultisampleType] )
                            continue;
                        d3dpp.MultiSample				= multisampleTypes[iMultisampleType];

                        //
                        //  Find the intersection of modes for this BackBufferFormat across all monitors
                        //  and push matching mode per swapchain
                        //
                        
                        UINT primaryAdapterOrientation = GetGDIDisplayOrientation(m_pSrcDevice->GetAdapterID());
                        
                        BOOL isModeOnAllAdapters = TRUE;
                        
                        //
                        // Match the current mode to modes on the other monitors
                        //
                        
                        vector<PRESENT_PARAMETERS>  adapterPresentParameters;   // present parameters
                        vector<UINT>  adapterParametersRotation; //rotation info for present parameters
                        
                        adapterParametersRotation.push_back(primaryAdapterOrientation);
                        adapterPresentParameters.push_back(d3dpp);

                        for( UINT i = 0; i < m_uNumberSwapChains; i++ )
                        {
                            if (m_pSrcDevice->GetAdapterID() == i)
                                continue;

                            DISPLAYMODE localDisplayMode;
                            UINT localMode = 0;
                            BOOL foundMatch = FALSE;
                            
                            UINT adapterOrientation = GetGDIDisplayOrientation(i);

                            while( SUCCEEDED( m_pD3D->EnumAdapterModes( i, d3dpp.BackBufferFormat, localMode++, &localDisplayMode ) ) )
                            {
                                //
                                // If other adapters are different orientation than the main adapter swap height for comparison
                                // 800x600 is considered equal to 600x800
                                // 
                                UINT swapableWidth0 = d3dpp.uWidth;
                                UINT swapableHeight0 = d3dpp.uHeight;

                                 if(primaryAdapterOrientation == 1 || primaryAdapterOrientation == 3 )
                                 {
                                    std::swap( swapableWidth0, swapableHeight0 );
                                 }

                                 UINT swapableWidth1 = localDisplayMode.dwWidth;
                                 UINT swapableHeight1 = localDisplayMode.dwHeight;
                                 if(adapterOrientation == 1 || adapterOrientation == 3 )
                                 {
                                    std::swap( swapableWidth1, swapableHeight1 );                                    
                                 }

                                
                                if( swapableWidth0 == swapableWidth1 &&
                                    swapableHeight0  == swapableHeight1 &&
                                    d3dpp.uFullscreenRefresh == localDisplayMode.dwRefreshRate)
                                {
                                    foundMatch = TRUE;
                                    
                                    PRESENT_PARAMETERS localD3dpp;
                                    ZeroMemory( &localD3dpp, sizeof(localD3dpp) );
                                    localD3dpp = d3dpp;

                                    localD3dpp.hDeviceWindow = m_hWndOverride; 
                                    localD3dpp.uWidth = localDisplayMode.dwWidth;
                                    localD3dpp.uHeight = localDisplayMode.dwHeight;
                                    localD3dpp.uFullscreenRefresh = localDisplayMode.dwRefreshRate;
                                    localD3dpp.BackBufferFormat = localDisplayMode.Format;
                                    
                                    adapterParametersRotation.push_back(adapterOrientation);
                                    adapterPresentParameters.push_back(localD3dpp);

                                    break;       
                                }
                            }

                            isModeOnAllAdapters = foundMatch && isModeOnAllAdapters;
                        }

                        if(isModeOnAllAdapters)
                        {                            

                            for(int i = 0; i < adapterPresentParameters.size();i++)
                            {
                                 // final check against CheckDeviceType
                                    
                                FORMAT bbFormat = adapterPresentParameters[i].BackBufferFormat;
                                
                                if((adapterPresentParameters[i].BackBufferFormat.d3dfFormat == FMT_A2B10G10R10_XR_BIAS))
                                {
                                    bbFormat = FMT_A2B10G10R10;
                                }
                                
                                HRESULT hr =  m_pD3D->CheckDeviceType(
                                            i,
                                            m_pSrcDevice->GetDevType(),
                                            adapterPresentParameters[i].BackBufferFormat,
                                            bbFormat,
                                            adapterPresentParameters[i].bWindowed         // always FALSE it's fullscreen test
                                            );

                                
                                if( SUCCEEDED(hr))
                                {
                                    m_vecPresentParameters.push_back( adapterPresentParameters[i] );
                                    m_vecPresentParametersRotation.push_back( adapterParametersRotation[i] );
                                }
                                else
                                {
                                    if(lastFormat.d3dfFormat != adapterPresentParameters[i].BackBufferFormat.d3dfFormat)
                                    {
                                        char sBBFormat[32], sFBFormat[32];
                                        StringCchCopy( sFBFormat, 32, FORMAT2STRING( adapterPresentParameters[i].BackBufferFormat.d3dfFormat ) );
                                        StringCchCopy( sBBFormat, 32, FORMAT2STRING( bbFormat.d3dfFormat ) );
                                    
                                        WriteToLog("CheckDeviceType hr = 0x%x for format returned from EnumAdapterModes. DisplayFormat %s BackBufferFormat %s" ,
                                            hr,
                                            sFBFormat,
                                            sBBFormat);
                                    }

                                }
                                lastFormat.d3dfFormat = adapterPresentParameters[i].BackBufferFormat.d3dfFormat;
                            }
                        }
                    }
                }
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CFullScreenPresent::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CFullScreenPresent::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CFullScreenPresent::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CFullScreenPresent::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}
