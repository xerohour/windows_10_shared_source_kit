// FILE:        multisample.cpp
// DESC:        multisample antialiasing/masking/toggling conformance test

#include "multisample.h"
#include "dwmapi.h"
#include "CSurface.h"
#include "CSwapChain.h"

CD3DWindowFramework App;
CSamples			Samples;
CMask				Mask;
CToggle				Toggle;
CCentroid			Centroid;

// NAME:            CMultisample()
// DESC:            contructor for multisample class
// INPUT:           none
// OUTPUT:          none

CMultisample::CMultisample()
{
	// Handle high-dpi setting so we can successfully use GetFrontBuffer
	SetProcessDPIAware();

    SetTestType(TESTTYPE_CONF);

    m_szTestName = "NA";
    m_szCommandKey = "NA";

    m_Options.ModeOptions.MultisampType = MULTISAMPLE_ALL;
    m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (enum _D3DFORMAT) FMT_ALL;

	m_Options.D3DOptions.nBackBufferWidth = (DWORD) 320;
    m_Options.D3DOptions.nBackBufferHeight = (DWORD) 240;

    m_Options.D3DOptions.bReference = false;
	m_bStretchPresent = false;

	m_Options.D3DOptions.bZBuffer = false;
	m_Options.D3DOptions.dwIncVersions = DXVERSION_91 | DXVERSION_9 | DXVERSION_8_OR_81;

	// Force test to use relevant DirectX versions for WHQL
    if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81;
}

// NAME:        ~CMultisample()
// DESC:        destructor for multisample class
// INPUT:       none
// OUTPUT:      none

CMultisample::~CMultisample()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT_* code
// COMMENTS:    skip non multisample modes

UINT CMultisample::TestInitialize(void)
{
    UINT nSamples = m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample];

	SetWindowPos( m_pFramework->m_hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	m_pFramework->SetStyleEx(m_pFramework->m_dwStyleEx | WS_EX_TOPMOST);

    if ( m_bGetSamplePoints || MULTISAMPLE_NONE == nSamples )
        return D3DTESTINIT_SKIPALL;

    return D3DTESTINIT_RUN;
}

bool CMultisample::DisplayFrame()
{
	HRESULT hr = S_OK;
	bool	bRes = true;
	int     nDevWidth = m_DeviceList[0]->GetPresParams()->uWidth;
	int     nDevHeight = m_DeviceList[0]->GetPresParams()->uHeight;
   	UINT    nWindowWidth = m_pFramework->m_pDisplayWindow->m_nWidth;
	UINT    nWindowHeight = m_pFramework->m_pDisplayWindow->m_nHeight;

	// Force the D3D pipeline to flush
	CSurface *pBackBuffer = NULL;
	if( FAILED( hr = m_pSrcDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer) ) )
	{
		DPF(2, _T("GetBackBuffer() failed. hr=%s(%x).\n"), m_pD3D->HResultToString(hr), hr );
		bRes = false;
	}
	else
	{
		LOCKED_RECT lr;
		pBackBuffer->LockRect(&lr, NULL, 0);
		pBackBuffer->UnlockRect();
		pBackBuffer->Release();
	}

	// Wait for DWM to compose the image
	DwmFlush();

	if(!m_pCurrentMode->bWindowed)
    {		
		hr = m_pSrcDevice->Present(NULL, NULL, NULL, NULL);
		
		if(FAILED(hr))
		{
			DPF(2, _T("CMultisample::DisplayFrame() - Present() failed (fullscreen). hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			bRes = false;
		}
    }
	else
	{
		RECT rSrc = {0, 0, nDevWidth, nDevHeight};
		RECT rDst = {   0,
			            0, 
				        __min( m_Options.D3DOptions.nBackBufferWidth, nWindowWidth ), 
						__min( m_Options.D3DOptions.nBackBufferHeight, nWindowHeight)};
						
		if(rDst.right - rDst.left > 0 && rDst.bottom - rDst.top > 0)
		{
			ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rDst);

			hr = m_pSrcDevice->Present(&rSrc, &rDst, NULL, NULL);
			
			if(FAILED(hr))
			{
				DPF(1, _T("CMultisample::DisplayFrame() - Present() failed (windowed). hr=%s(%x).\n"), 
					m_pD3D->HResultToString(hr), hr);
				bRes = false;
			}
		}
	}

	return bRes;
}

void CMultisample::HideMouse()
{
	RECT PrevRect;

	GetWindowRect( m_pFramework->m_pRebar->m_hWindow, &MouseTrap );

	GetClipCursor( &PrevRect );
	ClipCursor( &MouseTrap );

	memcpy( (void *)&MouseTrap, &PrevRect, sizeof( RECT ) );
}
void CMultisample::ShowMouse()
{
	ClipCursor( &MouseTrap );
}

bool CMultisample::ParseCmdLine()
{
	if ( KeySet( "Verbose" ) )
		m_bVerbose = true;
	else
		m_bVerbose = false;

	if ( KeySet( "GetPoints" ) )
	{
		m_bGetSamplePoints = true;
		if ( !KeySet( "BB" ) )
		{
			m_Options.ModeOptions.BackBufferFmt = FMT_A8R8G8B8;
		}

		if ( !KeySet( "DisplayMode" ) )
		{
			m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT) FMT_ANY;
		}
	}
	else
		m_bGetSamplePoints = false;

	return CD3DTest::ParseCmdLine();
}

HRESULT CMultisample::GetScene( LOCKED_RECT ** pRect, CSurface ** fbSurf )
{
	HRESULT		hr = S_OK;
 
	CSurface	*pFBSurf = NULL;
	LOCKED_RECT	FBLockRect;

	UINT		nWidth = m_pCurrentMode->nBackBufferWidth, nHeight = m_pCurrentMode->nBackBufferHeight;
	
	RECT		SrcRect;
	POINT		CornerPoint = {0,0};


	//Create the surface to copy the fb into
	memcpy( &SrcRect, &(m_pSrcDevice->GetAdapter()->rMonitorRect), sizeof(RECT) );	

	hr = m_pSrcDevice->CreateImageSurface( SrcRect.right - SrcRect.left, SrcRect.bottom - SrcRect.top, 
		FMT_A8R8G8B8, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CMultisample::GetScene() - CreateImageSurface failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		goto Fail;
	}

	if ( m_pCurrentMode->bWindowed )
	{
		if ( !ClientToScreen( m_pFramework->m_pDisplayWindow->m_hWindow, &CornerPoint ) )
		{
			DPF(1, _T("CMultisample::GetScene() - ClientToScreen failed." ));
			goto Fail;
		}

		SrcRect.top = CornerPoint.y - SrcRect.top;
		SrcRect.left = CornerPoint.x - SrcRect.left;
		SrcRect.bottom = SrcRect.top + nHeight;
		SrcRect.right = SrcRect.left + nWidth;
	}
	else
	{
		SrcRect.right = SrcRect.right - SrcRect.left;
		SrcRect.left = 0;
		SrcRect.bottom = SrcRect.bottom - SrcRect.top;
		SrcRect.top = 0;
	}
	
	HideMouse();
	hr = m_pSrcDevice->GetFrontBuffer( pFBSurf );
	ShowMouse();
	if(FAILED(hr))
	{
		DPF(1, _T("CMultisample::GetScene() - GetFrontBuffer failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		goto Fail;
	}

	hr = pFBSurf->LockRect( &FBLockRect, &SrcRect, D3DLOCK_READONLY );
	if(FAILED(hr))
	{
		DPF(1, _T("CMultisample::GetScene() - LockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		goto Fail;
	}

	*pRect = &FBLockRect;
	*fbSurf = pFBSurf;

	return S_OK;

Fail:
	RELEASE( pFBSurf );
	
	return hr;
}