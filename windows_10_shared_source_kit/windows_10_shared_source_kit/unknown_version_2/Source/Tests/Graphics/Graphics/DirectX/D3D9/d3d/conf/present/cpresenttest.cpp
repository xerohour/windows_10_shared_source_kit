//
//  CPresentTest.cpp
//
//  Class implementation for CDevice8Test.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//

#include "CPresentTest.h"
#include "CTextures.h"
#include "CBuffers.h"


#define UNLOCKRECT(a)  {if((a) != NULL) {(a)->UnlockRect();};};



// need to access window handle
CD3DWindowFramework *g_pD3DFramework=NULL;

//
// Declare the framework
//

CD3DWindowFramework	App;

//
// Define the groups here
//

GROUP_CREATE(CPresentTest);



//
// CDevice8Test Member functions
//

CPresentTest::CPresentTest() : 
	m_fDPIFactor( 1.0f ),
	m_hWndPresent( NULL ),
	m_hWndOverride( NULL ),
	m_pTex( NULL ),
    m_pVB( NULL ),
    m_hWnd( NULL )
{
	SetTestType( TESTTYPE_CONF );

	m_Options.D3DOptions.fMinDXVersion		= 8.0f;
	m_Options.D3DOptions.fMaxDXVersion		= 8.1f;
	m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.bZBuffer = false;
	m_Options.TestOptions.bCompareSrcRef = true;
    m_Options.D3DOptions.bEnumReference = true;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	//m_Options.D3DOptions.dwDevTypes = DEVICETYPE_ALL;
    //m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;

	m_bDontHideCursor	= KeySet(_T("ShowCursor"));

    m_szTestName = TEXT("Present");
    m_szCommandKey = TEXT("Present");

	g_pD3DFramework = &App;
}


UINT
CPresentTest::TestInitialize()
{
    LPTSTR szInfo = TEXT(
		"!!!WARNING!!! PRESENT WILL FAIL IF OTHER WINDOWS ARE PLACED ON TOP IT THE TEST WINDOW\n"
		"If you see Present failures, please try to rerun the test first."
        "Failures are reported as follows:\n"
        "\n"
        "    dest {(DX,DY) [DR DG DB]} != src {(SX,SY)(SWxSH) [SRL-SRH SGL-SGH SBL-SBH]}    diff{[LR LG LB]}\n"
        "\n"
        "with the following symbols:\n"
        "\n"
        "    DX,DY     - the X,Y coordinates of the destination pixel in error\n"
        "    DR,DG,DB  - the red, green, and blue components of the destination pixel in error\n"
        "    SX,SY     - the X,Y coordinates of the source area contributing to this destination pixel\n"
        "    SW,SH     - the width and height of the source area contributing to this destination pixel\n"
        "    SRL,SRH   - the low and high values for the red component in this source area\n"
        "    SGL,SGH   - the low and high values for the green component in this source area\n"
        "    SBL,SBH   - the low and high values for the blue component in this source area\n"
        "    LR,LG,LB  - the magnitude of error for each color component\n"
        "\n"
        "\n"
        "\n"
    );

    // output info to log
    WriteToLog( szInfo );


    // set test range
    ExecuteTest( (UINT)(-1) );

    m_FBColor = RGB( 0x12, 0x34, 0x56 );

    UINT iRet = D3DTESTINIT_RUN;
    HRESULT hr = D3D_OK;

    do
    {
        DISPLAYMODE dm;
        hr = m_pSrcDevice->GetDisplayMode( &dm );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetDisplayMode() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        m_ScreenWidth = dm.dwWidth;
        m_ScreenHeight = dm.dwHeight;
		
        // create window for hwnd override
        WNDCLASS wc =
        {
            0,  // style
            (WNDPROC)DefWindowProc,
            0,  // cbClsExtra
            0,  // cbWndExtra
            NULL,
            NULL,
            LoadCursor( NULL, IDC_CROSS ),
            (HBRUSH)(COLOR_3DFACE+1),
            NULL,  // lpszMenuName
            TEXT("Present")
        };

        if( !RegisterClass(&wc) )
        {
            DWORD dwErr = GetLastError();

            if( (dwErr != 0) && (dwErr != ERROR_CLASS_ALREADY_EXISTS) )
            {
                WriteToLog(
                    TEXT("RegisterClass() failed, last error %d (0x%x)\n"),
                    dwErr,
                    dwErr
                );
                hr = E_FAIL;
                continue;
            }
        }

		// place the window on whatever adapter the test is running on
		HMONITOR hMon = MonitorFromWindow( m_pFramework->m_pDisplayWindow->m_hWindow, MONITOR_DEFAULTTONEAREST );
		
		MONITORINFO mi;
		ZeroMemory( &mi, sizeof( mi ) );
		mi.cbSize = sizeof( mi );
		GetMonitorInfo( hMon, &mi );

		m_fDPIFactor = dm.dwWidth / fabs( (float)( mi.rcMonitor.right - mi.rcMonitor.left ) );

		float f100DPIFactor = m_fDPIFactor * 100.0f;
		int i100DPIFactor = (int)( f100DPIFactor + 0.5f);

		m_fDPIFactor = i100DPIFactor / 100.0f;

		int xWindow = ( mi.rcMonitor.left + mi.rcMonitor.right - 256 ) / 2 - 256;
		int yWindow = ( mi.rcMonitor.top + mi.rcMonitor.bottom - 256 ) / 2;
		while( ( xWindow % 4 ) > 0 )
			xWindow++;
		while( ( yWindow % 4 ) > 0 )
			yWindow++;

		// Create the reset window
		if( NULL == ( m_hWndPresent = CreateWindowEx(
									WS_EX_TOPMOST,
									_T( "Present" ), 
									_T("Present (default)"),
									WS_POPUP|WS_VISIBLE,
									xWindow,
									yWindow,
									256,
									256,
									m_pFramework->m_hWindow,
									0L,
									0L, 
									0L ) 
					) 
			)
		{
			WriteToLog(	TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
						GetLastError(),
						GetLastError()
						);
			hr = E_FAIL;
			continue;
		}

		xWindow = ( mi.rcMonitor.left + mi.rcMonitor.right - 256 ) / 2 + 256;
		while( ( xWindow % 4 ) > 0 )
			xWindow++;

		// create the override window
		if( NULL == ( m_hWndOverride = CreateWindowEx(
									WS_EX_TOPMOST,
									_T( "Present" ), 
									_T("Present Override"),
									WS_POPUP|WS_VISIBLE,
									xWindow,
									yWindow,
									256,
									256,
									NULL,
									0L,
									0L, 
									0L ) 
					)	
			)
		{
			WriteToLog(	TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
						GetLastError(),
						GetLastError()
						);
			hr = E_FAIL;
			continue;
		}

		ShowWindow( m_hWndPresent, SW_SHOW );
		ShowWindow( m_hWndOverride, SW_SHOW );

        if( m_hWndOverride == NULL )
        {
            WriteToLog(
                TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
                GetLastError(),
                GetLastError()
            );
            hr = E_FAIL;
            continue;
	    }

		// hide the mouse cursor
		if( !m_bDontHideCursor )
			::ShowCursor( FALSE );

		PRESENT_PARAMETERS* pCreationParameters = m_pSrcDevice->GetPresParams();
		pCreationParameters->uWidth = (UINT)( m_fDPIFactor * 256 );
		pCreationParameters->uHeight = (UINT)( m_fDPIFactor * 256 );
		pCreationParameters->hDeviceWindow = m_hWndPresent;
		if( FAILED( hr = m_pSrcDevice->Reset( pCreationParameters ) ) )
		{
			WriteToLog( _T( "Reset failed, return code %s (0x%x)\n" ),
						m_pD3D->HResultToString( hr ),
						hr );
			continue;
		}
		
		m_SrcWidth	=	pCreationParameters->uWidth;
		m_SrcHeight	=	pCreationParameters->uHeight;

        D3DTLVERTEX pVertices[] =
        {
        //  { sx, sy, sz, rhw, color, specular, tu, tv },
            { 0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff, 0, 0.0f, 0.0f },
            { (D3DVALUE)m_SrcWidth, 0.0f, 0.0f, 1.0f, 0xffffffff, 0, 1.0f, 0.0f },
            { 0.0f, (D3DVALUE)m_SrcHeight, 0.0f, 1.0f, 0xffffffff, 0, 0.0f, 1.0f },
            { (D3DVALUE)m_SrcWidth, (D3DVALUE)m_SrcHeight, 0.0f, 1.0f, 0xffffffff, 0, 1.0f, 1.0f },
        };
		UINT nVertices = COUNTOF(pVertices);

		LPBYTE pData = NULL;
		hr = m_pSrcDevice->D3DXCreateTextureFromResourceEx(
            NULL,
            TEXT("TEXTURE"),
			(UINT)( m_fDPIFactor * 256 ),
			(UINT)( m_fDPIFactor * 256 ),
			1,
			0,
			pCreationParameters->BackBufferFormat.d3dfFormat,
			D3DPOOL_MANAGED,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,
			NULL,
			NULL,
            &m_pTex
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("D3DXCreateTextureFromResource() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->SetTexture( 0, (LPBASETEXTURE)m_pTex );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetTexture() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->CreateVertexBuffer(
            nVertices * sizeof(D3DTLVERTEX),
            0,
            D3DFVF_TLVERTEX,
            D3DPOOL_SYSTEMMEM,
            &m_pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,%d,0x%x,%d,) returned %s (0x%x)\n"),
                nVertices * sizeof(D3DTLVERTEX),
                0,
                D3DFVF_TLVERTEX,
                D3DPOOL_SYSTEMMEM,
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pVB->Lock( 0, 0, &pData, 0 );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Lock(0,0,,0) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        CopyMemory( pData, pVertices, nVertices * sizeof(D3DTLVERTEX) );

        m_pVB->Unlock();

        hr = m_pSrcDevice->SetStreamSource( 0, m_pVB, sizeof(D3DTLVERTEX) );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetStreamSource(0,,) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->SetVertexShader( D3DFVF_TLVERTEX );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetVertexShader(D3DFVF_TLVERTEX) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }
    }
    while(0);

    if( FAILED(hr) )
    {
        // release stuff
       if( m_hWndOverride )
		{
			DestroyWindow( m_hWndOverride );
		}
		if( m_hWndPresent )
		{
			DestroyWindow( m_hWndPresent );
		}
        RELEASE(m_pTex);
        RELEASE(m_pVB);

        iRet = D3DTESTINIT_ABORT;
    }

    return(iRet);
}


bool
CPresentTest::TestTerminate()
{
    if( m_hWndOverride )
    {
        DestroyWindow( m_hWndOverride );
    }
	if( m_hWndPresent )
	{
		DestroyWindow( m_hWndPresent );
	}

    RELEASE(m_pTex);
    RELEASE(m_pVB);

    return(true);
}


bool
CPresentTest::ExecuteTest
(
    UINT nTest
)
{
    static RECT_SCALE RectScale[] =
    {
        { 0.0f, 0.0f, 1.0f, 1.0f },  // full area
        { 0.0f, 0.0f, 0.5f, 0.5f },  // half width, half height
        { 0.0f, 0.0f, 1.0f, 0.5f },  // full width, half height
        { 0.0f, 0.0f, 0.5f, 1.0f },  // half width, full height
        { -0.5f, -0.5f, 0.5f, 0.5f },  // full area, offset on top left

        { 0.5f, 0.5f, 1.5f, 1.5f },  // full area, offset on bottom right
        { -0.25f, -0.25f, 0.25f, 0.25f },  // half width/height, offset on top left
        { 0.33f, 0.33f, 0.66f, 0.66f },  // third width/height
        { 0.0f, 0.0f, 0.1f, 1.0f },  // vertical strip
        { 0.0f, 0.0f, 1.0f, 0.1f },  // horizontal strip

        { -0.5f, -0.5f, 1.5f, 1.5f },  // double height/width
        { -0.5f, 0.5f, 0.5f, 1.5f },  // full area, offset top right
        { 0.5f, -0.5f, 1.5f, 0.5f },  // full area, offset bottom left
        { 0.0f, -0.5f, 1.0f, 0.5f },  // full area, offset top
        { 0.0f, 0.5f, 1.0f, 1.5f },  // full area, offset bottom

        { -0.5f, 0.0f, 0.5f, 1.0f },  // full area, offset left
        { 0.5f, 0.0f, 1.5f, 1.0f },  // full area, offset right
    };

    static RECT Rects[] =
    {
        // absolute rects
        { 0, 0, 1, 1 },
        { 0, 0, 13, 31 },  // unaligned
        { 3, 7, 99, 131 },  // unaligned
        { 11, 11, 67, 67 },  // unaligned
    };

    static const HWND Hwnds[] =
    {
        NULL,
    };

    static const D3DTEST_RECT TestRects[] =
    {
    //  { szName, bScale, pRectScale, pRect },
        { TEXT("default"), FALSE, NULL, NULL },
        { TEXT("full area"), TRUE, &RectScale[0], NULL },
        { TEXT("half width/height"), TRUE, &RectScale[1], NULL },
        { TEXT("half height"), TRUE, &RectScale[2], NULL },
        { TEXT("half width"), TRUE, &RectScale[3], NULL },
        { TEXT("offset top left"), TRUE, &RectScale[4], NULL },
        { TEXT("offset bottom right"), TRUE, &RectScale[5], NULL },
        { TEXT("half width/height, offset top left"), TRUE, &RectScale[6], NULL },
        { TEXT("third width/height"), TRUE, &RectScale[7], NULL },
        { TEXT("vertical strip"), TRUE, &RectScale[8], NULL },
        { TEXT("horizontal strip"), TRUE, &RectScale[9], NULL },
        { TEXT("double width/height"), TRUE, &RectScale[10], NULL },
        { TEXT("offset top right"), TRUE, &RectScale[11], NULL },
        { TEXT("offset bottom left"), TRUE, &RectScale[12], NULL },
        { TEXT("offset top"), TRUE, &RectScale[13], NULL },
        { TEXT("offset bottom"), TRUE, &RectScale[14], NULL },
        { TEXT("offset left"), TRUE, &RectScale[15], NULL },
        { TEXT("offset right"), TRUE, &RectScale[16], NULL },
        { TEXT("one pixel"), FALSE, NULL, &Rects[0] },
        { TEXT("unaligned 13x31"), FALSE, NULL, &Rects[1] },
        { TEXT("unaligned 96x124"), FALSE, NULL, &Rects[2] },
        { TEXT("unaligned 56x56"), FALSE, NULL, &Rects[3] },
    };

    static const D3DTEST_PRESENT TestList[] =
    {
    //  { Name, bScaleSrc, pSrcRectScale, pSrcRect, bScaleDest, pDestRectScale, pDestRect, bDestWndOverride, hDestWindowOverride, pDirtyRegion },

        // boring call
        { TEXT("source = NULL, dest = NULL"), FALSE, NULL, NULL, FALSE, NULL, NULL, FALSE, NULL, NULL },

        // random test cases can go here
    };
    static const UINT nTestList = COUNTOF(TestList);


    static RECT SrcRect;
    static RECT DestRect;


    typedef union __TESTCASE
    {
        struct
        {
            UINT SrcRect : 6;
            UINT DestRect : 6;
            UINT WndOverride : 2;
            UINT ListIndex : 3;
            UINT DoList : 1;
        };
        UINT nTest;
    }
    TESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(TestRects),
        COUNTOF(TestRects),
        2,  // WndOverride
        COUNTOF(TestList),
        0,  // DoList; count not used
    };


    TESTCASE tc;
    tc.nTest = (nTest - 1);

    if( nTest == (UINT)(-1) )
    {
        ClearRangeList();

        tc.nTest = 0;
        for( tc.SrcRect = 0; tc.SrcRect < tcCount.SrcRect; tc.SrcRect++ )
        {
            for( tc.DestRect = 0; tc.DestRect < tcCount.DestRect; tc.DestRect++ )
            {
                for( tc.WndOverride = 0; tc.WndOverride < tcCount.WndOverride; tc.WndOverride++ )
                {
                    AddTestRange( (tc.nTest+1), (tc.nTest+1) );
                }
            }
        }

        tc.nTest = 0;
        tc.DoList = 1;
        for( tc.ListIndex = 0; tc.ListIndex < tcCount.ListIndex; tc.ListIndex++ )
        {
            AddTestRange( (tc.nTest+1), (tc.nTest+1) );
        }

        return( FALSE );
    }
	
    if(
        (
            (tc.DoList == 0) &&
            (
                (tc.SrcRect >= tcCount.SrcRect) ||
                (tc.DestRect >= tcCount.DestRect) ||
                (tc.WndOverride >= tcCount.WndOverride)
            )
        ) ||
        (
            (tc.DoList == 1) &&
            (tc.ListIndex >= tcCount.ListIndex)
        )
    )
    {
        return(FALSE);
    }

    TCHAR szName[MAX_PATH];

    if( tc.DoList )
    {
        const D3DTEST_PRESENT *curtest = &TestList[tc.ListIndex];

        if( curtest->bDestWndOverride )
        {
            m_hDestWnd = m_hWndOverride;
            m_hWnd = m_hWndOverride;
            m_hDestParent = NULL;
            ShowWindow( m_hWndOverride, SW_SHOWNORMAL );
        }
        else
        {
            m_hDestWnd = curtest->hDestWindowOverride;
            m_hWnd = m_hWndPresent;
            m_hDestParent = NULL;
            ShowWindow( m_hWndOverride, SW_HIDE );
        }


        RECT rc;
        GetClientRect( m_hWnd, &rc );
        m_DestWidth = rc.right;
        m_DestHeight = rc.bottom;


        if( curtest->bScaleSrc )
        {
            SrcRect.left = (LONG) (curtest->pSrcRectScale->left * m_SrcWidth);
            SrcRect.right = (LONG) (curtest->pSrcRectScale->right * m_SrcWidth);
            SrcRect.top = (LONG) (curtest->pSrcRectScale->top * m_SrcHeight);
            SrcRect.bottom = (LONG) (curtest->pSrcRectScale->bottom * m_SrcHeight);
            m_pSrcRect = &SrcRect;
        }
        else
        {
            m_pSrcRect = curtest->pSrcRect;
        }

        if( curtest->bScaleDest )
        {
            DestRect.left = (LONG) (curtest->pDestRectScale->left * m_DestWidth);
            DestRect.right = (LONG) (curtest->pDestRectScale->right * m_DestWidth);
            DestRect.top = (LONG) (curtest->pDestRectScale->top * m_DestHeight);
            DestRect.bottom = (LONG) (curtest->pDestRectScale->bottom * m_DestHeight);
            m_pDestRect = &DestRect;
        }
        else
        {
            m_pDestRect = curtest->pDestRect;
        }

        m_pDirtyRegion = curtest->pDirtyRegion;

        strcpy( szName, curtest->szName );
    }
    else
    {
        if( tc.WndOverride )
        {
            m_hDestWnd = m_hWndOverride;
            m_hWnd = m_hWndOverride;
            m_hDestParent = NULL;
            ShowWindow( m_hWndOverride, SW_SHOWNORMAL );
        }
        else
        {
            m_hDestWnd = NULL;
            m_hWnd = m_hWndPresent;
            m_hDestParent = NULL;
            ShowWindow( m_hWndOverride, SW_HIDE );
        }

        RECT rc;
        GetClientRect( m_hWnd, &rc );
        m_DestWidth = rc.right;
        m_DestHeight = rc.bottom;

        const D3DTEST_RECT *pSrc = &TestRects[tc.SrcRect];
        const D3DTEST_RECT *pDest = &TestRects[tc.DestRect];


        if( pSrc->bScale )
        {
            SrcRect.left = (LONG) (pSrc->pRectScale->left * m_SrcWidth);
            SrcRect.right = (LONG) (pSrc->pRectScale->right * m_SrcWidth);
            SrcRect.top = (LONG) (pSrc->pRectScale->top * m_SrcHeight);
            SrcRect.bottom = (LONG) (pSrc->pRectScale->bottom * m_SrcHeight);
            m_pSrcRect = &SrcRect;
        }
        else
        {
            m_pSrcRect = pSrc->pRect;
        }

        if( pDest->bScale )
        {
            DestRect.left = (LONG) (pDest->pRectScale->left * m_DestWidth);
            DestRect.right = (LONG) (pDest->pRectScale->right * m_DestWidth);
            DestRect.top = (LONG) (pDest->pRectScale->top * m_DestHeight);
            DestRect.bottom = (LONG) (pDest->pRectScale->bottom * m_DestHeight);
            m_pDestRect = &DestRect;
        }
        else
        {
            m_pDestRect = pDest->pRect;
        }

        m_pDirtyRegion = NULL;

        wsprintf( szName,
            TEXT("dest{%s} <= src{%s}, WndOverride = %s"),
            pDest->szName,
            pSrc->szName,
            tc.WndOverride ? TEXT("true") : TEXT("false")
        );


        if( !ValidateRects( m_pSrcRect, m_pDestRect ) )
        {
            // if present wouldn't do anything, then skip
            return(FALSE);
        }
    }


    BeginTestCase( szName, nTest );

	// in high DPI small rects can't be precisely computed due to stretch
	// skip those tests
	if( m_fDPIFactor > 1.0f &&
		( ( NULL != m_pSrcRect && ( ( m_pSrcRect->right - m_pSrcRect->left ) < 64 ) && ( ( m_pSrcRect->bottom - m_pSrcRect->top ) < 64 ) ) ||
		  ( NULL != m_pDestRect && ( ( m_pDestRect->right - m_pDestRect->left ) < 64 ) && ( ( m_pDestRect->bottom - m_pDestRect->top ) < 64 ) )
		) 
	  )
	{
		Skip();
		return false;
	}

    HRESULT hr;

    do
    {
        // clear front buffer some funky color
        /*
          Change made by blakep
          Removed GDI calls.  Synchronization between GDI and D3D is not supported on Longhorn (LDDM),
          which causes this test to fail.

          The purpose of the GDI calls is to clear the front buffer, which the image comparison parts of this test rely on
          So clearing the front buffer is acomplished with D3D operations
        */
        /*HDC hDC = GetDC( m_hWnd );
        HGDIOBJ hBrushPrev = SelectObject( hDC, CreateSolidBrush( m_FBColor ) );
        PatBlt( hDC, 0, 0, m_DestWidth, m_DestHeight, PATCOPY );
        DeleteObject( SelectObject( hDC, hBrushPrev ) );
        ReleaseDC( m_hWnd, hDC );*/
        const DWORD dwClearColor = D3DCOLOR_XRGB(GetRValue(m_FBColor), GetGValue(m_FBColor), GetBValue(m_FBColor));
        
        hr = m_pSrcDevice->Clear(0, NULL, D3DCLEAR_TARGET, dwClearColor, 0.0f, 0 );
        if( FAILED(hr) )
        {
            WriteToLog(
                       TEXT("FBClear Clear() return %s (0x%x)\n"),
                       m_pD3D->HResultToString(hr),
                       hr
                       );
            continue;
        }
        
        hr = m_pSrcDevice->Present( NULL, NULL, m_hDestWnd, NULL );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("FBClear Present() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }
        


        hr = m_pSrcDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Clear() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->BeginScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("BeginScene() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->DrawPrimitive(
            D3DPT_TRIANGLESTRIP,
            0,
            2
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->EndScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("EndScene() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }
    }
    while(0);

    if( FAILED(hr) )
    {
        Abort();
    }
    else
    {
        hr = m_pSrcDevice->Present(
            m_pSrcRect,
            m_pDestRect,
            m_hDestWnd,
            m_pDirtyRegion
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Present() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            Fail();
        }
    }

    return( FAILED(hr) ? false : true );
}


// override present call
bool
CPresentTest::DisplayFrame(void)
{
    return(true);
}



bool
CPresentTest::CompareRangePixel
(
    MYCOLOR SrcMin,
    MYCOLOR SrcMax,
    MYCOLOR Dest,
    FORMAT Format,
    MYCOLOR *Diff
)
{
    bool bRet = true;
    MYCOLOR Bits;
    MYCOLOR Mask;
    MYCOLOR Error;


    Mask.alpha = 0x00;

    switch(Format.d3dfFormat)
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_P8:
            Bits.red = 8;
            Bits.green = 8;
            Bits.blue = 8;
            break;

        case D3DFMT_R5G6B5:
            Bits.red = 5;
            Bits.green = 6;
            Bits.blue = 5;
            break;

        case D3DFMT_X1R5G5B5:
            Bits.red = 5;
            Bits.green = 5;
            Bits.blue = 5;
            break;

        default:
            WriteToLog(
                TEXT("Format is not supported by comparison: %d (0x%x)\n"),
                Format
            );
            bRet = false;
            break;
    }

    Error.red = ((0xff >> (8 - Bits.red / 2)) << (8 - Bits.red));
    Error.green = ((0xff >> (8 - Bits.green / 2)) << (8 - Bits.green));
    Error.blue = ((0xff >> (8 - Bits.blue / 2)) << (8 - Bits.blue));

    Mask.red = (0xff << (8 - Bits.red));
    Mask.green = (0xff << (8 - Bits.green));
    Mask.blue = (0xff << (8 - Bits.blue));

    if(bRet)
    {
        SrcMin.color &= Mask.color;
        SrcMax.color &= Mask.color;
        Dest.color &= Mask.color;

        int DiffRed;
        int DiffGreen;
        int DiffBlue;

        DiffRed =
            ( Dest.red < SrcMin.red ) ? (int)Dest.red - (int)SrcMin.red :
            ( Dest.red > SrcMax.red ) ? (int)Dest.red - (int)SrcMax.red :
            0;

        DiffGreen =
            ( Dest.green < SrcMin.green ) ? (int)Dest.green - (int)SrcMin.green :
            ( Dest.green > SrcMax.green ) ? (int)Dest.green - (int)SrcMax.green :
            0;

        DiffBlue =
            ( Dest.blue < SrcMin.blue ) ? (int)Dest.blue - (int)SrcMin.blue :
            ( Dest.blue > SrcMax.blue ) ? (int)Dest.blue - (int)SrcMax.blue :
            0;

		// this comparision is confusing and should be changed.
		// the biggest issue is that when the difference is negative, the
		// bytewise error reported is formated unsigned.
		// ex: dest [47 65 f9] != src [50-6b 86-b6 fc-ff]   diff[f7 df fd]
		// sugg: recode this section to produce the absolute value of the difference
		// as:
		/*
        DiffRed =
            ( Dest.red < SrcMin.red ) ? (int)SrcMin.red - (int)Dest.red:
            ( Dest.red > SrcMax.red ) ? (int)Dest.red - (int)SrcMax.red :
            0;

        DiffGreen =
            ( Dest.green < SrcMin.green ) ? (int)SrcMin.green - (int)Dest.green:
            ( Dest.green > SrcMax.green ) ? (int)Dest.green - (int)SrcMax.green :
            0;

        DiffBlue =
            ( Dest.blue < SrcMin.blue ) ? (int)SrcMin.blue - (int)Dest.blue:
            ( Dest.blue > SrcMax.blue ) ? (int)Dest.blue - (int)SrcMax.blue :
            0;

		bRet =
            (DiffRed <= (int)Error.red) &&
            (DiffGreen <= (int)Error.green) &&
            (DiffBlue <= (int)Error.blue);
		*/
		// this would simplify and correct the reports.
		// -ChuckN

		bRet =
            (DiffRed <= (int)Error.red) && (DiffRed >= -(int)Error.red) &&
            (DiffGreen <= (int)Error.green) && (DiffGreen >= -(int)Error.green) &&
            (DiffBlue <= (int)Error.blue) && (DiffBlue >= -(int)Error.blue);

        if( Diff )
        {
            Diff->red = (BYTE)DiffRed;
            Diff->green = (BYTE)DiffGreen;
            Diff->blue = (BYTE)DiffBlue;
        }
    }

    return(bRet);
}



bool
CPresentTest::CompareSurfaces
(
    LOCKED_RECT *pSrcLocked,
    LOCKED_RECT *pDestLocked,
    FORMAT Format
)
{
    static const UINT nMaxErrors = 15;  // number of error pixels to report in detail
    static const FLOAT fThreshold = 0.05f;  // percentage of destination pixels that are allowed to be errors


    INT nErrors = 0;

    RECT SrcRect;
    RECT DestRect;
    FLOAT ScaleX;
    FLOAT ScaleY;

    MYCOLOR FBColor;
    MYCOLOR Diff;


    if( m_pSrcRect )
    {
        SrcRect.left = m_pSrcRect->left;
        SrcRect.top = m_pSrcRect->top;
        SrcRect.right = m_pSrcRect->right;
        SrcRect.bottom = m_pSrcRect->bottom;
    }
    else
    {
        SrcRect.left = 0;
        SrcRect.top = 0;
        SrcRect.right = m_SrcWidth;
        SrcRect.bottom = m_SrcHeight;
    }

    if( m_pDestRect )
    {
        DestRect.left = (LONG)( m_fDPIFactor * m_pDestRect->left );
        DestRect.top = (LONG)( m_fDPIFactor * m_pDestRect->top );
        DestRect.right = (LONG)( m_fDPIFactor * m_pDestRect->right );
        DestRect.bottom = (LONG)( m_fDPIFactor * m_pDestRect->bottom );
    }
    else
    {
        DestRect.left = 0;
        DestRect.top = 0;
        DestRect.right = (LONG)( m_fDPIFactor * m_DestWidth );
        DestRect.bottom = (LONG)( m_fDPIFactor * m_DestHeight );
    }

    ScaleX = (FLOAT)(SrcRect.right - SrcRect.left)/(FLOAT)(DestRect.right - DestRect.left);
    ScaleY = (FLOAT)(SrcRect.bottom - SrcRect.top)/(FLOAT)(DestRect.bottom - DestRect.top);

    FBColor.red = GetRValue(m_FBColor);
    FBColor.green = GetGValue(m_FBColor);
    FBColor.blue = GetBValue(m_FBColor);
    FBColor.alpha = 0;


    for( int y = 0; y < m_rcDestVis.bottom; y++ )
    {
        for( int x = 0; x < m_rcDestVis.right; x++ )
        {
            MYCOLOR DestPix;

            DestPix.color =
                *(DWORD*)( (UINT_PTR)pDestLocked->pBits +
                y * pDestLocked->iPitch +
                x * 4);

            DestPix.alpha = 0;  // ignore alpha

			// for High DPI aware apps, some stretching may be involved
			// have to be more flexible in detecting outside pixels
			bool bPixelInside = (x >= DestRect.left) && (x < DestRect.right) &&
								(y >= DestRect.top) && (y < DestRect.bottom);
			if( !bPixelInside && m_fDPIFactor > 1 )
				for( int xOffset = -1; xOffset <= 1; xOffset++ )
					for( int yOffset = -1; yOffset <= 1; yOffset++ )
					{
						bPixelInside |=	(x + xOffset >= DestRect.left) && (x + xOffset < DestRect.right) &&
										(y + yOffset >= DestRect.top) && (y + yOffset < DestRect.bottom);
					}

            if( bPixelInside )
            {
                INT SrcX = (INT)((FLOAT)(x - DestRect.left) * ScaleX) + SrcRect.left;
                INT SrcY = (INT)((FLOAT)(y - DestRect.top) * ScaleY) + SrcRect.top;
                INT RangeX;
                INT RangeY;

                if( ScaleX <= 1.0f )
                {
                    RangeX = 5;
                    SrcX-=2;
                }
                else if( ScaleX > 1.0f )
                {
                    RangeX = (INT)ceil(ScaleX);

                    SrcX -= RangeX / 2;
                    RangeX += RangeX;
                }

                if( ScaleY <= 1.0f )
                {
                    RangeY = 5;
                    SrcY-=2;
                }
                else if( ScaleY > 1.0f )
                {
                    RangeY = (INT)ceil(ScaleY);

                    SrcY -= RangeY / 2;
                    RangeY += RangeY;
                }


                MYCOLOR SrcPix;
                MYCOLOR SrcMin;
                MYCOLOR SrcMax;

                SrcMin.color = 0xffffffff;
                SrcMax.color = 0;

                UINT nSrcPix = 0;
                UINT nSrcPixSkip = 0;

                for( int yoff = SrcY; yoff < (SrcY + RangeY); yoff++ )
                {
                    for( int xoff = SrcX; xoff < (SrcX + RangeX); xoff++ )
                    {
                        if(
                            (xoff >= 0) && (xoff < m_SrcWidth) &&
                            (yoff >= 0) && (yoff < m_SrcHeight)
                        )
                        {
                            SrcPix.color =
                                *(DWORD*)( (UINT_PTR)pSrcLocked->pBits +
                                yoff * pSrcLocked->iPitch +
                                xoff * 4 );

                            SrcMin.red = (SrcPix.red < SrcMin.red) ? SrcPix.red : SrcMin.red;
                            SrcMax.red = (SrcPix.red > SrcMax.red) ? SrcPix.red : SrcMax.red;
                            SrcMin.green = (SrcPix.green < SrcMin.green) ? SrcPix.green : SrcMin.green;
                            SrcMax.green = (SrcPix.green > SrcMax.green) ? SrcPix.green : SrcMax.green;
                            SrcMin.blue = (SrcPix.blue < SrcMin.blue) ? SrcPix.blue : SrcMin.blue;
                            SrcMax.blue = (SrcPix.blue > SrcMax.blue) ? SrcPix.blue : SrcMax.blue;

                            nSrcPix++;
                        }
                        else
                        {
                            nSrcPixSkip++;
                        }
                    }
                }

                if( nSrcPix > nSrcPixSkip )
                {
                    // valid pixel
                    if( !CompareRangePixel( SrcMin, SrcMax, DestPix, Format, &Diff ) )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x-%.2x %.2x-%.2x %.2x-%.2x]}   diff{[%.2x %.2x %.2x]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                SrcMin.red,
                                SrcMax.red,
                                SrcMin.green,
                                SrcMax.green,
                                SrcMin.blue,
                                SrcMax.blue,
                                Diff.red,
                                Diff.green,
                                Diff.blue
                            );
                        }
                    }
                }
                else if( nSrcPix == 0 )
                {
                    // skipped pixel
                    if( !CompareRangePixel( FBColor, FBColor, DestPix, Format, &Diff ) )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x %.2x %.2x] (outside source surface)}   diff{[%d %d %d]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                FBColor.red,
                                FBColor.green,
                                FBColor.blue,
                                (char)Diff.red,
                                (char)Diff.green,
                                (char)Diff.blue
                            );
                        }
                    }
                }
                else  // 0 < nSrcPix <= nSrcPixSkip
                {
                    // allow either of above
                    if(
                        !CompareRangePixel( SrcMin, SrcMax, DestPix, Format, &Diff ) &&
                        !CompareRangePixel( FBColor, FBColor, DestPix, Format, NULL )
                    )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x-%.2x %.2x-%.2x %.2x-%.2x] || [%.2x %.2x %.2x]}   diff{[%.2x %.2x %.2x]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                SrcMin.red,
                                SrcMax.red,
                                SrcMin.green,
                                SrcMax.green,
                                SrcMin.blue,
                                SrcMax.blue,
                                FBColor.red,
                                FBColor.green,
                                FBColor.blue,
                                Diff.red,
                                Diff.green,
                                Diff.blue
                            );
                        }
                    }
                }
            }
            else
            {
                // make sure points outside dest rect are funk colored
                if( !CompareRangePixel( FBColor, FBColor, DestPix, Format, &Diff ) )
                {
                    if( nErrors++ < nMaxErrors )
                    {
                        WriteToLog(
                            TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {[%.2x %.2x %.2x] (outside blit destination)}   diff{[%d %d %d]}\n"),
                            x,
                            y,
                            DestPix.red,
                            DestPix.green,
                            DestPix.blue,
                            FBColor.red,
                            FBColor.green,
                            FBColor.blue,
                            (char)Diff.red,
                            (char)Diff.green,
                            (char)Diff.blue
                        );
                    }
                }
            }
        }
    }

    if( nErrors )
    {
        WriteToLog(
            TEXT("    destination pixels (incorrect/total):  %d / %d\n"),
            nErrors,
            DestRect.right * DestRect.bottom
        );
    }

    return(nErrors <= (fThreshold * DestRect.right * DestRect.bottom));
}


bool
CPresentTest::ProcessFrame(void)
{
    HRESULT hr;
    BOOL bRet;
    DISPLAYMODE dm;
    LPSURFACE pTempSurf = NULL;
    LPSURFACE pSrcSurf = NULL;
    LPSURFACE pDestSurf = NULL;
    WINDOWINFO wi = { sizeof(wi) };
    RECT DestRect;
    LOCKED_RECT SrcLocked;
    LOCKED_RECT DestLocked;
	HMONITOR hMon;
	MONITORINFO mi;


    do
    {
        hr = m_pSrcDevice->GetDisplayMode( &dm );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetDisplayMode() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        // source surface, backbuffer
        hr = m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pTempSurf );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->CreateImageSurface(
            (UINT)( m_SrcWidth ),
            (UINT)( m_SrcHeight ),
            FMT_A8R8G8B8,
            &pSrcSurf
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateImageSurface(,,FMT_A8R8G8B8,) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->D3DXLoadSurfaceFromSurface(
            pSrcSurf,
            NULL,
            NULL,
            pTempSurf,
            NULL,
            NULL,
            D3DX_DEFAULT,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("D3DXLoadSurfaceFromSurface() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }
		//m_pSrcDevice->D3DXSaveSurfaceToFile( "refSurface.bmp", D3DXIFF_BMP, pSrcSurf, NULL, NULL );


        hr = pSrcSurf->LockRect( &SrcLocked, NULL, LOCK_READONLY );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("LockRect(,NULL,LOCK_READONLY) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }


        // destination surface, front buffer
        hr = m_pSrcDevice->CreateImageSurface(
            m_ScreenWidth,
            m_ScreenHeight,
            FMT_A8R8G8B8,
            &pDestSurf
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateImageSurface(,,FMT_A8R8G8B8,) returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice->GetFrontBuffer( pDestSurf );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetFrontBuffer() returned %s (0x%x)\n"),
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }

        bRet = GetWindowInfo( m_hWnd, &wi );
        if( !bRet )
        {
            WriteToLog(
                TEXT("GetWindowInfo() failed, last error %d (0x%x)\n"),
                GetLastError(),
                GetLastError()
            );
            hr = E_FAIL;
            continue;
        }

		// adjust the returned rect struct into device coords
		hMon = MonitorFromWindow( m_hWnd, MONITOR_DEFAULTTONEAREST );
		if( !hMon )
		{
			WriteToLog( TEXT("MonitorFromWindow() failed.\n") );
			continue;
		}
		
		ZeroMemory( &mi, sizeof( mi ) );
		mi.cbSize = sizeof( mi );
		if( !GetMonitorInfo( hMon, &mi ) )
        {
            WriteToLog( TEXT("GetMonitorInfo() failed.\n") );
            continue;
        }
        wi.rcClient.left -= mi.rcMonitor.left;
        wi.rcClient.top  -= mi.rcMonitor.top;
        wi.rcClient.right -= mi.rcMonitor.left;
        wi.rcClient.bottom  -= mi.rcMonitor.top;
        
		if( m_hDestParent == NULL )
        {
            DestRect.left = wi.rcClient.left;
            DestRect.top = wi.rcClient.top;
            DestRect.right = wi.rcClient.right;
            DestRect.bottom = wi.rcClient.bottom;
        }
        else
        {
            WINDOWINFO wiParent = { sizeof(wiParent) };

            bRet = GetWindowInfo( m_hDestParent, &wiParent );
            if( !bRet )
            {
                WriteToLog(
                    TEXT("GetWindowInfo() failed, last error %d (0x%x)\n"),
                    GetLastError(),
                    GetLastError()
                );
                hr = E_FAIL;
                continue;
            }
			
			// adjust the returned rect struct into device coords
			hMon = MonitorFromWindow( m_hDestParent, MONITOR_DEFAULTTONEAREST );
			if( !hMon )
			{
				WriteToLog( TEXT("MonitorFromWindow() failed.\n") );
				continue;
			}
			ZeroMemory( &mi, sizeof( mi ) );
			mi.cbSize = sizeof( mi );
			if( !GetMonitorInfo( hMon, &mi ) )
			{
				WriteToLog( TEXT("GetMonitorInfo() failed.\n") );
				continue;
			}
			wiParent.rcClient.left -= mi.rcMonitor.left;
			wiParent.rcClient.top  -= mi.rcMonitor.top;
			wiParent.rcClient.right -= mi.rcMonitor.left;
			wiParent.rcClient.bottom  -= mi.rcMonitor.top;

            IntersectRect( &DestRect, &wi.rcClient, &wiParent.rcClient );
        }

		// adjust for DPI
		DestRect.left = (LONG)( m_fDPIFactor * DestRect.left );
		DestRect.right = (LONG)( m_fDPIFactor * DestRect.right );
		DestRect.top = (LONG)( m_fDPIFactor * DestRect.top );
		DestRect.bottom = (LONG)( m_fDPIFactor * DestRect.bottom );

        m_rcDestVis.left = 0;
        m_rcDestVis.top = 0;
        m_rcDestVis.right = DestRect.right - DestRect.left;
        m_rcDestVis.bottom = DestRect.bottom - DestRect.top;

        hr = pDestSurf->LockRect( &DestLocked, &DestRect, LOCK_READONLY );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("LockRect(,(%d,%d,%d,%d),0x%x) returned %s (0x%x)\n"),
                DestRect.left,
                DestRect.top,
                DestRect.right,
                DestRect.bottom,
                LOCK_READONLY,
                m_pD3D->HResultToString(hr),
                hr
            );
            continue;
        }


        // run comparison
        if(!CompareSurfaces( &SrcLocked, &DestLocked, dm.Format ))
        {
            Fail();
        }
    }
    while(0);

    if( FAILED(hr) )
    {
        Abort();
    }

    UNLOCKRECT(pSrcSurf);
    UNLOCKRECT(pDestSurf);
    RELEASE(pTempSurf);
    RELEASE(pSrcSurf);
    RELEASE(pDestSurf);


    return(true);
}



BOOL
CPresentTest::ValidateRects
(
    const RECT *pSrc,
    const RECT *pDest
)
{
    if( (pSrc == NULL) || (pDest == NULL) )
    {
        return(TRUE);
    }

    RECT rcSrcSurf;
    RECT rcDestSurf;
    RECT rcSrcVis;
    RECT rcDestVis;
    RECT rcTemp;
    RECT rcTemp2;

    UINT SrcWidth = pSrc->right - pSrc->left;
    UINT SrcHeight = pSrc->bottom - pSrc->top;
    UINT DestWidth = pDest->right - pDest->left;
    UINT DestHeight = pDest->bottom - pDest->top;


    rcSrcSurf.left = 0;
    rcSrcSurf.top = 0;
    rcSrcSurf.right = m_SrcWidth;
    rcSrcSurf.bottom = m_SrcHeight;

    rcDestSurf.left = 0;
    rcDestSurf.top = 0;
    rcDestSurf.right = m_DestWidth;
    rcDestSurf.bottom = m_DestHeight;

    IntersectRect( &rcSrcVis, pSrc, &rcSrcSurf );
    IntersectRect( &rcDestVis, pDest, &rcDestSurf );

    rcTemp.left = pDest->left + (((rcSrcVis.left - pSrc->left) * DestWidth) / SrcWidth);
    rcTemp.top = pDest->top + (((rcSrcVis.top - pSrc->top) * DestHeight) / SrcHeight);
    rcTemp.right = rcTemp.left + (((rcSrcVis.right - rcSrcVis.left) * DestWidth) / SrcWidth);
    rcTemp.bottom = rcTemp.top + (((rcSrcVis.bottom - rcSrcVis.top) * DestHeight) / SrcHeight);

    return( IntersectRect( &rcTemp2, &rcTemp, &rcDestVis ) );
}


