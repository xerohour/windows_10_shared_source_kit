// PresentRotate.cpp: implementation of the CRotatePresent class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentRotate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRotatePresent::CRotatePresent()
{
	m_TestType		= TEST_FULLSCREENPRESENT;
	m_szCommandKey	= _T("PresentRotate");
	m_szTestName	= _T("PresentRotate");
	m_Options.D3DOptions.fMinDXVersion = 9.1f;

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
}

CRotatePresent::~CRotatePresent()
{

}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CRotatePresent::Name( char* szName )
{
	sprintf( szName, "%s (%s,%d[Rotation:%d])", m_szTestName, 
									FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ), 
									m_nIndexPresentParameters,
									m_vecDevModes[m_nIndexPresentParameters].dmDisplayOrientation
			);
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	ChangeDisplaySettings()
// Desc:	change the mode if necessary
//////////////////////////////////////////////////////////////////////
void CRotatePresent::ChangeDisplaySettings()
{
	for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
	{
		LONG lResult = ::ChangeDisplaySettingsEx
						( 
								(LPCTSTR)m_vecDevModes[ m_nIndexPresentParameters + uSwapChain ].dmDeviceName,
								&( m_vecDevModes[ m_nIndexPresentParameters + uSwapChain ] ), 
								NULL,
								CDS_FULLSCREEN | CDS_UPDATEREGISTRY,
								NULL
						) ;

		switch( lResult )
		{
			case DISP_CHANGE_SUCCESSFUL:
				break;
			case DISP_CHANGE_BADDUALVIEW:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_BADDUALVIEW.\n");
				break;
			case DISP_CHANGE_BADFLAGS:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_BADFLAGS.\n");
				break;
			case DISP_CHANGE_BADMODE:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_BADMODE.\n");
				break;
			case DISP_CHANGE_BADPARAM:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_BADPARAM.\n");
				break;
			case DISP_CHANGE_FAILED:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_FAILED.\n");
				break;
			case DISP_CHANGE_NOTUPDATED:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_NOTUPDATED.\n");
				break;
			case DISP_CHANGE_RESTART:
				DebugOut.Write( "ChangeDisplaySettingsW returned DISP_CHANGE_RESTART.\n");
				break;
		}
	
		// this call is supposed to refresh DX9L's mode list
		D3DDISPLAYMODEFILTER displayModeFilter;
		ZeroMemory( &displayModeFilter, sizeof D3DDISPLAYMODEFILTER );
		displayModeFilter.Size = sizeof D3DDISPLAYMODEFILTER;
		displayModeFilter.Format = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat.d3dfFormat;
		UINT numModes = m_pD3D->GetAdapterModeCountEx( m_pSrcDevice->GetAdapterID() + uSwapChain, &displayModeFilter );
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CRotatePresent::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	GetFrontBufferRect();

	m_vecPresentParameters.clear();
	m_vecDevModes.clear();

	d3dpp.bWindowed = false;
	d3dpp.dwFlags = PRESENTFLAG_LOCKABLE_BACKBUFFER | D3DPRESENTFLAG_NOAUTOROTATE;
	d3dpp.AutoDepthStencilFormat = FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil = false;

	d3dpp.bCachedBackBuffers = false;
	d3dpp.DesktopFormat = m_fmtFrontBuffer;

	FMT backbufferFormats[] =
	{
		FMT_X8R8G8B8,
#ifndef FAST_TEST
		FMT_R5G6B5,
		FMT_X1R5G5B5,
		FMT_A2R10G10B10
#endif
	};

#ifdef FAST_TEST
	POINT bbSize[] =
	{
		{ m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top }
	};
#endif


	D3DADAPTER_IDENTIFIER9 adapterIdentifier[ MAX_SWAPCHAINS ];
	ZeroMemory( &adapterIdentifier, MAX_SWAPCHAINS * sizeof D3DADAPTER_IDENTIFIER9 );
	for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
	{
		m_pD3D->GetAdapterIdentifier(  m_pSrcDevice->GetAdapterID() + uSwapChain, 0, (ADAPTER_IDENTIFIER*)&adapterIdentifier[uSwapChain] );
	}

	DEVMODE defaultMode[ MAX_SWAPCHAINS ];
	ZeroMemory( &defaultMode, MAX_SWAPCHAINS * sizeof DEVMODE );

	for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
	{
		defaultMode[ uSwapChain ].dmSize = sizeof DEVMODE ;
		EnumDisplaySettings( adapterIdentifier[ uSwapChain ].DeviceName, ENUM_CURRENT_SETTINGS, &defaultMode[ uSwapChain ] );
		_tcscpy_s( (TCHAR*)defaultMode[ uSwapChain ].dmDeviceName, CCHDEVICENAME, adapterIdentifier[ uSwapChain ].DeviceName );
	}

	for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
    {
        for( UINT modeRotation = DMDO_DEFAULT; modeRotation <= DMDO_270; modeRotation++ )
        //for( UINT modeRotation = Mode.dmDisplayOrientation; modeRotation <= Mode.dmDisplayOrientation; modeRotation++ )
        {
            D3DDISPLAYMODEEX displayModeEx;
            D3DDISPLAYMODEFILTER displayModeFilter;
            ZeroMemory( &displayModeEx, sizeof D3DDISPLAYMODEEX );
            displayModeEx.Size = sizeof D3DDISPLAYMODEEX;
            ZeroMemory( &displayModeFilter, sizeof D3DDISPLAYMODEFILTER );
            displayModeFilter.Size = sizeof D3DDISPLAYMODEFILTER;
            displayModeFilter.Format = FORMAT( backbufferFormats[iFormat] ).d3dfFormat;

            DEVMODE rotatedMode[ MAX_SWAPCHAINS ];
            memcpy( &rotatedMode, &defaultMode, MAX_SWAPCHAINS * sizeof DEVMODE );
            
            
            BOOL addModes = TRUE;
            for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
            {
                switch( modeRotation )
                {
                case DMDO_DEFAULT:
                    break;
                case DMDO_90 :
                    std::swap( rotatedMode[ uSwapChain ].dmPelsWidth, rotatedMode[ uSwapChain ].dmPelsHeight );
                    break;
                case DMDO_180 :
                    break;
                case DMDO_270 :
                    std::swap( rotatedMode[ uSwapChain ].dmPelsWidth, rotatedMode[ uSwapChain ].dmPelsHeight );
                    break;
                }

                rotatedMode[ uSwapChain ].dmDisplayOrientation = modeRotation;
                
                //
                // Test if mode is supported
                //
                LONG lResult = ::ChangeDisplaySettingsEx
                    (
                    (LPCTSTR)rotatedMode[ uSwapChain ].dmDeviceName,
                    &rotatedMode[ uSwapChain ],
                    NULL,
                    CDS_FULLSCREEN | CDS_UPDATEREGISTRY | CDS_TEST,
                    NULL
                    );

                if(lResult == DISP_CHANGE_BADMODE)
                {
                    WriteToLog("ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE. Rotated mode %d not supported for format %s", modeRotation,  FORMAT2STRING( FORMAT(backbufferFormats[iFormat] ).d3dfFormat));
                    addModes = FALSE;
                    break;
                }
                else
                {

                    //
                    // Mode is supported change to it.
                    //
                    lResult = ::ChangeDisplaySettingsEx
                        (
                        (LPCTSTR)rotatedMode[ uSwapChain ].dmDeviceName,
                        &rotatedMode[ uSwapChain ],
                        NULL,
                        CDS_FULLSCREEN | CDS_UPDATEREGISTRY,
                        NULL
                        );
                    switch( lResult )
                    {
                    case DISP_CHANGE_SUCCESSFUL:
                        break;
                    case DISP_CHANGE_BADDUALVIEW:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW.\n");
                        break;
                    case DISP_CHANGE_BADFLAGS:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS.\n");
                        break;
                    case DISP_CHANGE_BADMODE:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE.\n");
                        break;
                    case DISP_CHANGE_BADPARAM:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM.\n");
                        break;
                    case DISP_CHANGE_FAILED:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED.\n");
                        break;
                    case DISP_CHANGE_NOTUPDATED:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED.\n");
                        break;
                    case DISP_CHANGE_RESTART:
                        DebugOut.Write( "ChangeDisplaySettingsEx returned DISP_CHANGE_RESTART.\n");
                        break;
                    }

                    // this call is supposed to refresh DX9L's mode list
                    UINT numModes = numModes = m_pD3D->GetAdapterModeCountEx( m_pSrcDevice->GetAdapterID() + uSwapChain, &displayModeFilter );
                }
            }

            // cycle through all display modes on first adpater
            UINT nMode = 0;
            while( addModes && SUCCEEDED( m_pD3D->EnumAdapterModesEx(	m_pSrcDevice->GetAdapterID(), 
                &displayModeFilter,
                nMode++,
                &displayModeEx ) ) )
            {
#ifdef FAST_TEST
				bool testDisplayMode = false;
				for (UINT uBBIndex = 0; uBBIndex < ARRAYSIZE(bbSize); uBBIndex++)
				{
					if (bbSize[uBBIndex].x == displayModeEx.Width && displayModeEx.Height == bbSize[uBBIndex].y)
					{
						testDisplayMode = true;
						break;
					}
				}

				if (!testDisplayMode) continue;
#endif

                // for 90 / 270 rotation, width and height of the backbuffer need to be swapped
                // so they're in landscape mode, always.
                // When specifying NOAUTOROTATE, BackBufferWidth and Height must be equivalent to 
                // the DisplayMode in the IDENTITY orienation.

                switch( modeRotation )
                {
                case DMDO_DEFAULT:
                    break;
                case DMDO_90 :
                    std::swap( displayModeEx.Width, displayModeEx.Height );
                    break;
                case DMDO_180 :
                    break;
                case DMDO_270 :
                    std::swap( displayModeEx.Width, displayModeEx.Height );
                    break;
                }

                // skip interlaced modes if required
                if( displayModeEx.RefreshRate < 60 && KeySet( _T( "NoInterlaced" ) ) )
                    continue;

                // only do one size per refresh rate
                bool bModeExists = false;
                for( UINT uPresentParameter = 0; uPresentParameter < m_vecPresentParameters.size(); uPresentParameter += m_uNumberSwapChains )
                    if( m_vecPresentParameters[uPresentParameter].uWidth == displayModeEx.Width &&
                        m_vecPresentParameters[uPresentParameter].uHeight == displayModeEx.Height &&
                        m_vecPresentParameters[uPresentParameter].BackBufferFormat.d3dfFormat == displayModeEx.Format &&
                        m_vecPresentParameters[uPresentParameter].uFullscreenRefresh != displayModeEx.RefreshRate )
                    {
                        bModeExists = true;
                        break;
                    }
                    if( bModeExists )
                        continue;

                    d3dpp.uWidth					= displayModeEx.Width;
                    d3dpp.uHeight					= displayModeEx.Height;
                    d3dpp.uFullscreenRefresh		= displayModeEx.RefreshRate;
                    d3dpp.BackBufferFormat			= displayModeEx.Format;

                    // cycle through backbuffers count
                    d3dpp.uBackBufferCount			= 1;

                    // only do DISCARDs
                    DWORD swapEffects[] = 
                    {
                        SWAPEFFECT_DISCARD,
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

                        // only do DEFAULT
                        DWORD presentIntervals[] = 
                        {
                            PRESENT_INTERVAL_DEFAULT,
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

                            d3dpp.uPresentInterval			= presentIntervals[iPresentInterval];

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
                                for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
                                {
                                    BOOL isModeOnAllAdapters = TRUE;

                                    //
                                    // Match the current mode to modes on the other monitors
                                    //
                                    for( UINT i = 0; i < m_uNumberSwapChains; i++ )
                                    {
                                        if (m_pSrcDevice->GetAdapterID() == i)
                                            continue;

                                        D3DDISPLAYMODEEX localDisplayMode;
                                        ZeroMemory( &localDisplayMode, sizeof D3DDISPLAYMODEEX );
                                        localDisplayMode.Size = sizeof D3DDISPLAYMODEEX;
                                        UINT localMode = 0;
                                        BOOL foundMatch = FALSE;
                                        while( SUCCEEDED( m_pD3D->EnumAdapterModesEx(i, &displayModeFilter, localMode++, &localDisplayMode ) ) )
                                        {
                                            switch( modeRotation )
                                            {
                                            case DMDO_DEFAULT:
                                                break;
                                            case DMDO_90 :
                                                std::swap( localDisplayMode.Width, localDisplayMode.Height );
                                                break;
                                            case DMDO_180 :
                                                break;
                                            case DMDO_270 :
                                                std::swap( localDisplayMode.Width, localDisplayMode.Height );
                                                break;
                                            }

                                            if( d3dpp.uWidth == localDisplayMode.Width &&
                                                d3dpp.uHeight == localDisplayMode.Height &&
                                                d3dpp.uFullscreenRefresh == localDisplayMode.RefreshRate)
                                            {
                                                foundMatch = TRUE;
                                                break;       
                                            }
                                        }

                                        isModeOnAllAdapters = foundMatch && isModeOnAllAdapters;
                                    }

                                    if(isModeOnAllAdapters)
                                    {                            
                                        // final check against CheckDeviceType
                                        if( SUCCEEDED( m_pD3D->CheckDeviceType(	m_pSrcDevice->GetAdapterID(),
                                                        m_pSrcDevice->GetDevType(),
                                                        d3dpp.BackBufferFormat,
                                                        d3dpp.BackBufferFormat,
                                                        d3dpp.bWindowed				// always FALSE it's fullscreen test
                                                    )
                                                )
                                            )
                                        {
                                            m_vecDevModes.push_back( rotatedMode[ uSwapChain ] );

                                            if(uSwapChain == m_pSrcDevice->GetAdapterID())
                                            {
                                                d3dpp.hDeviceWindow	= m_hWndReset;
                                            }
                                            else
                                            {
                                                d3dpp.hDeviceWindow = m_hWndOverride; 
                                            }
                                            m_vecPresentParameters.push_back( d3dpp );
                                        }
                                    }
                                }
                            }
                        }
                    }
            }
        }
    }

	//
    // Reset all the displays
    //
    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
	{
		::ChangeDisplaySettingsEx
		(	
				(LPCTSTR)defaultMode[ uSwapChain ].dmDeviceName,
				&( defaultMode[ uSwapChain ] ),
				NULL,
				CDS_FULLSCREEN | CDS_UPDATEREGISTRY,
				NULL
		);
	}
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CRotatePresent::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CRotatePresent::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CRotatePresent::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitTextures()
// Desc:	build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CRotatePresent::InitTextures()
{
	m_vecStringTexture.clear();
	m_vecStringTexture.push_back( "lake" );
}
