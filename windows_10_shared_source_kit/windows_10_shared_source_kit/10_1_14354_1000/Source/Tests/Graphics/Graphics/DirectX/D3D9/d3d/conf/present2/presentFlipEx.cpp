// PresentFlipEx.cpp: implementation of the CPresentFlipEx class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentFlipEx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPresentFlipEx::CPresentFlipEx()
{
    m_TestType = TEST_PRESENT;
    m_szCommandKey = _T("PresentFlipEx");
    m_szTestName = _T("PresentFlipEx");
    
    m_SwapEffect = SWAPEFFECT_FLIPEX;
    m_Options.D3DOptions.DisplayMode.Format = FMT_X8R8G8B8; 
    
    m_Options.D3DOptions.dwIncVersions = DXVERSION_91;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
    m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIP;
    m_Options.D3DOptions.dwPresentFlags = 0;
    m_Options.TestOptions.bDisableDWM = false;

   
}

CPresentFlipEx::~CPresentFlipEx()
{
}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CPresentFlipEx::Name( char* szName )
{
    TCHAR swapEffect[MAX_PATH] = {0};
    SWAPEFFECT2STRING(m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect, swapEffect, MAX_PATH);

    TCHAR deviceReset[MAX_PATH] = {0};
    DeviceReset2String(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify], deviceReset, MAX_PATH);

    sprintf(
        szName,
        "%s (%s,%s, PresentParameters %d,Texture %d,WindowTree %d, %s, Flag=%s )", 
        m_szTestName, 
        FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ),
        swapEffect,
        m_nIndexPresentParameters, 
        m_nIndexTexture, 
        m_nIndexWindowTrees,
        deviceReset,
        (m_nIndexFlagsEx == 0)? "0" : (m_nIndexFlagsEx == 1)? "D3DPRESENT_DONOTFLIP" : "OTHER");

    return szName;
}

//////////////////////////////////////////////////////////////////////
// Name:	CapsCheck()
//////////////////////////////////////////////////////////////////////
bool CPresentFlipEx::CapsCheck()
{
    return CPresentII::CanUseFlipEx();
}

//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();
    vector<RECT> vecRectBackBufferSizes;
    InitBackBufferRects( vecRectBackBufferSizes, m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left, m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top  );

	d3dpp.bWindowed						= true;
	d3dpp.uBackBufferCount				= 1;
	d3dpp.SwapEffect					= m_SwapEffect;
	d3dpp.hDeviceWindow					= m_hWndReset;
    d3dpp.dwFlags						= 0;
	d3dpp.uPresentInterval				= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;
	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;

	FMT backbufferFormats[] = 
	{
		FMT_X8R8G8B8,
#ifndef FAST_TEST
		FMT_A8R8G8B8,
		//FMT_A2R10G10B10,
        FMT_A2B10G10R10,
		FMT_A16B16G16R16F 
#endif
	};

	for( int iFormat = 0; iFormat < sizeof( backbufferFormats ) / sizeof FMT; iFormat++ )
	{
		d3dpp.BackBufferFormat = backbufferFormats[iFormat];
		if( SuppressAlphaChannel( m_fmtFrontBuffer ).d3dfFormat == SuppressAlphaChannel( d3dpp.BackBufferFormat ).d3dfFormat && CheckDeviceFormat ( d3dpp.BackBufferFormat ) )
        {           
            for(int i = 0; i < vecRectBackBufferSizes.size(); i++)
            {
                d3dpp.uWidth = vecRectBackBufferSizes[i].right - vecRectBackBufferSizes[i].left;
	            d3dpp.uHeight = vecRectBackBufferSizes[i].bottom - vecRectBackBufferSizes[i].top;
                m_vecPresentParameters.push_back( d3dpp );
            }
        }
	}
}

//////////////////////////////////////////////////////////////////////
// Name:	InitFlagsEx()
// Desc:	LONGHORN ONLY: build the vector of Ex flags to test
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitFlagsEx()
{
	m_vecFlagsEx.clear();
	m_vecFlagsEx.push_back( 0 );
	m_vecFlagsEx.push_back( D3DPRESENT_DONOTFLIP );
}

//////////////////////////////////////////////////////////////////////
// Name:    InitPresentSourceRect()
// Desc:    build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitPresentSourceRect()
{
    m_vecRectPresentSource.clear();
    m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );    // NULL
}


//////////////////////////////////////////////////////////////////////
// Name:    InitDirtyRegions()
// Desc:    build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitPresentDestRect()
{
    m_vecRectPresentDest.clear();
    m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );        // NULL
}

//////////////////////////////////////////////////////////////////////
// Name:    InitPresentWindows()
// Desc:    build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitPresentWindows()
{
    m_vecPresentWindow.clear();
    m_vecPresentWindow.push_back( NULL );
}

//////////////////////////////////////////////////////////////////////
// Name:    InitDeviceReleaseBeforeVerify()
// Desc:    build the vector of whether or not to release the device
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitDeviceReleaseBeforeVerify()
{
    m_vecDeviceReleaseBeforeVerify.clear();
    m_vecDeviceReleaseBeforeVerify.push_back( NoRelease );
    m_vecDeviceReleaseBeforeVerify.push_back( ReleaseDevice );
    m_vecDeviceReleaseBeforeVerify.push_back( ReleaseSwapChains );
}

//////////////////////////////////////////////////////////////////////
// Name:	PrivateDataProcessing()
// Desc:	process private data case before rendering
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::PrivateDataProcessing()
{
    //
    // Move the all children out of the way for verification purposes
    //

    bool found = false;
    HRESULT hResult = S_OK;
    Node<WindowData>* pTestWindow = NULL;
    pTestWindow = FindTestWindowInWindowTree(&m_vecWindowTrees[m_nIndexWindowTrees], &found);
    if(pTestWindow)
    {
        RECT rectWnd = { 0 };    
        if(!GetWindowRect( pTestWindow->Data.Hwnd, &rectWnd ))
        {
            DWORD e = GetLastError();
            hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
            WriteToLog( _T( "GetWindowRectfail for hwnd 0x%x, hr = 0x%x \n" ),pTestWindow->Data.Hwnd, hResult);
            Abort();   
        }
        
        hResult = MoveWindowTree(pTestWindow, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top );
        if(FAILED(hResult))
        {
            WriteToLog( _T( "MoveWindowTree, return code %s (0x%x)\n" ),hResult);
            Abort();
        }
        
        hResult = PumpMessagesWindowTree(&m_vecWindowTrees[m_nIndexWindowTrees] );
        if(FAILED(hResult))
        {
            WriteToLog( _T( "PumpMessagesWindowTree, return code %s (0x%x)\n" ),hResult);
            Abort();
        }
    }

}

//////////////////////////////////////////////////////////////////////
// Name:    Present()
// Desc:    implementation of Present (either using the device's shortcut or swapchain's method)
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::Present()
{
    
    CPresentII::Present();
    
    //
    // Clear and present with D3DPRESENT_DONOTFLIP should change anything
    //
    if( 0 != ( m_vecFlagsEx[m_nIndexFlagsEx] & D3DPRESENT_DONOTFLIP ) )
	{

		// just do a clear - should NOT be presented
		for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
		{
			LPSURFACE pBBSurface = NULL;

			ASSERT( uSwapChain < m_vecAdapterMonitors.size() );
			ASSERT( NULL != m_vecAdapterMonitors[uSwapChain].m_pSwapChain );

			// set the render target to the swapchain's backbuffer
			if( m_uNumberSwapChains > 1 )
			{
				Check_HRFail( m_vecAdapterMonitors[uSwapChain].m_pSwapChain->GetBackBuffer( m_nIndexBB, D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
				Check_HRFail( m_pRenderingDevice->SetRenderTarget( (DWORD)0, pBBSurface ), "SetRenderTarget" );
			}
	
			Check_HRFail( m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xFF00FF00, 1.0f, 0L ), "Clear" );
			
			// release the swapchain's backbuffer
			if( m_uNumberSwapChains > 1 )
			{
				RELEASE( pBBSurface );
			}
		}
        
        
		HRESULT hrPresent = S_OK;
		if( m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
		{        
			for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
			{
				hrPresent =  m_vecAdapterMonitors[uSwapChain].m_pSwapChain->Present(  
					m_prectPresentSource, 
					m_prectPresentDest, 
					m_vecPresentWindow[m_nIndexPresentWindow], 
					m_pDirtyRegion,
					m_vecFlagsEx[m_nIndexFlagsEx]
					);
	            
				if( FAILED(hrPresent))
				{
					break;
				}
			}
		}
		else
		{
			hrPresent = m_pRenderingDevice->PresentEx(  
				m_prectPresentSource, 
				m_prectPresentDest, 
				m_vecPresentWindow[m_nIndexPresentWindow], 
				m_pDirtyRegion,
				m_vecFlagsEx[m_nIndexFlagsEx]);
		}

		switch( HandlePresentReturnCode( hrPresent ) )
        {
        case E_ABORT:
        case E_OUTOFMEMORY:
            throw hrPresent;
        default:
            Check_HRFail(hrPresent,TEXT( "Present (CPresentFlipEx::Present) with D3DPRESENT_DONOTFLIP" ));
        };
	}
}

//////////////////////////////////////////////////////////////////////
// Name:    InitWindowTrees()
// Desc:    Used by Create Window heirarchy
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::InitWindowTrees()
{
    m_vecWindowTrees.clear();
    
    Node<WindowData> root;
    Node<WindowData> parent;
    Node<WindowData> child;
    Node<WindowData> childsChild;

    //
    // 0 Just the test window with region
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.IsTestWindow =true;
    parent.Data.IsRegionWindow =true;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);

    //
    //  Background window for validating regions
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.IsTestWindow =false;
    parent.Data.IsRegionWindow =false;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);



    m_vecWindowTrees.push_back(root);

    parent.Children.clear();  
    root.Children.clear();
    
    //
    // 1 Just the test window with different styles
    //
   
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_LAYOUTRTL;
    parent.Data.IsTestWindow =true;
    parent.Data.IsRegionWindow =false;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    parent.Children.clear();  
    root.Children.clear();

    //
    // 2
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_LAYOUTRTL | WS_EX_TOPMOST;
    parent.Data.IsTestWindow =true;
    parent.Data.IsRegionWindow =true;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    
    
    //
    //  Background window for validating regions
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.IsTestWindow =false;
    parent.Data.IsRegionWindow =false;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    
    
    
    m_vecWindowTrees.push_back(root);

    parent.Children.clear();  
    root.Children.clear();
    
    //
    // 3 WS_EX_LAYERED sytle
    //
    
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_LAYERED;
    parent.Data.IsTestWindow =true;
    parent.Data.IsRegionWindow =false;
    parent.Data.IsLayeredWindow =true;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    parent.Children.clear();  
    root.Children.clear();

    //
    //  4 Background window for validating regions
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
    parent.Data.IsTestWindow =true;
    parent.Data.IsRegionWindow =true;
    parent.Data.IsLayeredWindow =true;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    
    
    //
    //  Background window for validating regions
    //
    parent.Data.Style = WS_POPUP | WS_VISIBLE;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.IsTestWindow =false;
    parent.Data.IsRegionWindow =false;
    parent.Data.IsLayeredWindow =false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;

    root.Children.push_back(parent);
    
    
    
    m_vecWindowTrees.push_back(root);

    parent.Children.clear();  
    root.Children.clear();
    
    //
    // Tree
    //
    
    //
    // 5 Test Window parent, Child GDI
    //

    parent.Data.Style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN;
    parent.Data.ExStyle = 0;
    parent.Data.IsTestWindow =true;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;

    
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.ExStyle = 0;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = false;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
 
    
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

    //
    // 6 Test Window parent, Child DX non flip
    //

    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.IsTestWindow =true;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;

    
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.ExStyle = 0;
    child.Data.SwapEffect = SWAPEFFECT_COPY;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = true;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
   
    
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

    //
    // 7 Test Window parent, Child DX  flip
    //

    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.IsTestWindow =true;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;

    
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.ExStyle = 0;
    child.Data.SwapEffect = SWAPEFFECT_FLIPEX;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = true;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
   
    
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();


    //
    // 8 Parent GDI, Test Window child
    //

    parent.Data.ExStyle = 0;
    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;
    
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.ExStyle = 0;
    child.Data.IsTestWindow =true;
    child.Data.CreateDevice = false;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
  
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

    //
    // 9 Parent DX non flip, Test Window child
    //

    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.ExStyle = 0;
    parent.Data.SwapEffect = SWAPEFFECT_COPY;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = true;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;
    
    child.Data.ExStyle = 0;
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.IsTestWindow =true;
    child.Data.CreateDevice = false;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
   
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

    //
    // 10 Parent DX  flip, Test Window child
    //

    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.ExStyle = 0;
    parent.Data.SwapEffect = SWAPEFFECT_FLIPEX;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = true;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;
    
    child.Data.ExStyle = 0;
    child.Data.Style = WS_VISIBLE | WS_CHILD;
    child.Data.IsTestWindow =true;
    child.Data.CreateDevice = false;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
   
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();


    //
    // 11 Parent GDI, 1st child GDI, 2nd child Test Window 
    //

    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.ExStyle = 0;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = false;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;

    child.Data.Style =  WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    child.Data.ExStyle = 0;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = false;
    child.Data.pDevice = NULL;
    child.Data.IsRegionWindow =false;
      
    childsChild.Data.Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    childsChild.Data.ExStyle = WS_EX_TOPMOST;
    childsChild.Data.IsTestWindow =true;
    childsChild.Data.CreateDevice = false;
    childsChild.Data.pDevice = NULL;
    childsChild.Data.IsRegionWindow =false;
   
    child.Children.push_back(childsChild);
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    childsChild.Children.clear();
    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

    //
    // 12 Parent DX non flip, 1st child DX non flip, 2nd child Test Window 
    //

    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.SwapEffect = SWAPEFFECT_DISCARD;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = true;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;
    
    child.Data.Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS  | WS_CLIPCHILDREN;
    child.Data.ExStyle = 0;
    child.Data.SwapEffect = SWAPEFFECT_DISCARD;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = true;
    child.Data.pDevice = NULL;    
    child.Data.IsRegionWindow =false;
   
    childsChild.Data.Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    childsChild.Data.ExStyle = 0;
    childsChild.Data.IsTestWindow =true;
    childsChild.Data.CreateDevice = false;
    childsChild.Data.pDevice = NULL;
    childsChild.Data.IsRegionWindow =false;
  
    child.Children.push_back(childsChild);
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    childsChild.Children.clear();
    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

   
    //
    // 13 Parent DX flip, 1st child DX flip, 2nd child Test Window 
    //

    parent.Data.Style = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN;
    parent.Data.ExStyle = WS_EX_TOPMOST;
    parent.Data.SwapEffect = SWAPEFFECT_FLIPEX;
    parent.Data.IsTestWindow = false;
    parent.Data.CreateDevice = true;
    parent.Data.pDevice = NULL;
    parent.Data.IsRegionWindow =false;
   
    child.Data.Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    child.Data.ExStyle = 0;
    child.Data.SwapEffect = SWAPEFFECT_FLIPEX;
    child.Data.IsTestWindow = false;
    child.Data.CreateDevice = true;
    child.Data.pDevice = NULL;    
    child.Data.IsRegionWindow =false;
   
    childsChild.Data.Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
    childsChild.Data.ExStyle = 0;
    childsChild.Data.IsTestWindow =true;
    childsChild.Data.CreateDevice = false;
    childsChild.Data.pDevice = NULL;
    childsChild.Data.IsRegionWindow =false;
   
    child.Children.push_back(childsChild);
    parent.Children.push_back(child);
    root.Children.push_back(parent);
    m_vecWindowTrees.push_back(root);

    childsChild.Children.clear();
    child.Children.clear();
    parent.Children.clear();  
    root.Children.clear();

}


//////////////////////////////////////////////////////////////////////
// Name:    AfterReleaseDeviceBeforeVerify()
// Desc:    called by test, so it can force a dwm repaint 
//////////////////////////////////////////////////////////////////////
void CPresentFlipEx::AfterReleaseDeviceBeforeVerify()
{

    MSG msg = { 0 };

    ShowWindow( m_clearWindow, SW_SHOW );
    SetWindowPos( m_clearWindow, HWND_TOPMOST, 0, 0, 0 , 0,  SWP_NOMOVE | SWP_NOSIZE);   
    while( PeekMessage( &msg, m_clearWindow, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }


    SetWindowPos( m_hWndPresent, HWND_NOTOPMOST, 0, 0, 0 , 0,  SWP_NOMOVE | SWP_NOSIZE);      
    while( PeekMessage( &msg, m_hWndPresent, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    Sleep(25);

    SetWindowPos( m_clearWindow, HWND_BOTTOM, 0, 0, 0 , 0,  SWP_NOMOVE | SWP_NOSIZE);      
    ShowWindow( m_clearWindow, SW_HIDE );
    while( PeekMessage( &msg, m_clearWindow, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    SetWindowPos( m_hWndPresent, HWND_TOPMOST, 0, 0, 0 , 0,  SWP_NOMOVE | SWP_NOSIZE);
    while( PeekMessage( &msg, m_hWndPresent, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    Sleep(25);
}

UINT CPresentFlipEx::TestInitialize()
{
    UINT retVal = false;
    RECT rectWnd = {0};

    retVal = CPresentII::TestInitialize();
    if(retVal != 0)
    {
        GetWindowRect( m_hWndPresent, &rectWnd );
    
        //
        // Since m_hWndPresent uses a NULL_BRUSH for its backround.  Tests needs a window to clear the area
        //
        m_clearWindow = NULL;
        if( NULL == ( m_clearWindow = CreateWindowEx(
                0,
                _T( "STATIC" ), 
                _T("Wow"),
                SS_GRAYRECT | WS_POPUP| WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
                rectWnd.left,
                rectWnd.top,
                DEFAULT_WINDOW_WIDTH,
                DEFAULT_WINDOW_HEIGHT,
                NULL,
                0L,
                0L, 
                0L ) 
            )
        )
        {
            WriteToLog(    TEXT("CPresentFlipEx::TestInitialize:: CreateWindow() failed, last error %d (0x%x)\n"),
                GetLastError(),
                GetLastError());

            retVal = false;
        }
    }

    if(retVal != 0)
    {
        MSG msg = { 0 };
        while( PeekMessage( &msg, m_clearWindow, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
     }

    return retVal;
}

bool CPresentFlipEx::TestTerminate()
{
    
    if( m_clearWindow )
    {
        DestroyWindow( m_clearWindow );
    }

    return CPresentII::TestTerminate();
}

//////////////////////////////////////////////////////////////////////
// Name:    CanUseFlipEx CapsCheck()
// Desc:    check caps for FlipEx. Adding check for DWM on.
//          This test requires DWM on.
//////////////////////////////////////////////////////////////////////
bool CPresentFlipEx::CanUseFlipEx()
{
    bool retVal = true;

    retVal = CPresentII::CanUseFlipEx();

    if( retVal && !IsDWMEnabled() )
    {
        WriteToLog("PresentFlipEx test not valid when composition is disabled");
        retVal = false;  
    }

    return retVal;
}
