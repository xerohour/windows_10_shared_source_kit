// FILE:        Toggle.cpp
// DESC:        multisample toggling conformance test
//				Verifies that the hardware the supports multisample toggling actually does so

#include "multisample.h"
#include "CSurface.h"

// NAME:            CToggle()
// DESC:            contructor for sample class
// INPUT:           none
// OUTPUT:          none

CToggle::CToggle()
{
	m_szTestName = _T( "Toggling Verification" );
    m_szCommandKey = _T( "Toggle" );

	m_Options.D3DOptions.fMinDXVersion = 9.0f;
}

// NAME:        ~CToggle()
// DESC:        destructor for multisample class
// INPUT:       none
// OUTPUT:      none

CToggle::~CToggle()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT_* code
// COMMENTS:    skip non multisample modes

UINT CToggle::TestInitialize(void)
{
	UINT uiRetVal = CMultisample::TestInitialize();
	m_bMask = false;

	SetTestRange((UINT) 1, (UINT) 2);

	if ( !( m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_MULTISAMPLE_TOGGLE ) )
	{
		return D3DTESTINIT_SKIPALL;
	}

	if ( MULTISAMPLE_NONMASKABLE != m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] )
	{
		m_bMask = true;
	}

    return uiRetVal;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       nTest....test number
// OUTPUT:      true

bool CToggle::ExecuteTest(UINT nTest)
{	
	HRESULT hr = S_OK;

	BeginTestCase( m_szTestName, nTest );

	//Setting up the render states and whatnot
	{
		hr = m_pSrcDevice->SetFVF( FLEXFMT );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetFVF failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_FLAT);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_ZENABLE, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::Render() - SetRenderState failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, BLACK, 1.0f, 0 );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - Clear failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
	}

	switch ( nTest )
	{
	case 1:
		//First, with multisampling on; count sampling points
		//Then turn it off, and render center pixel
		//Then render a mask
		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->BeginScene();
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - BeginScene failed.\n" ) );
			Fail();
		}
		QuickSampleTest( 0 );

		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		QuickSampleTest( 1 );		

		break;
	case 2:
		//First, with multisampling off; render center pixel
		//Then turn it on, and count sampling points
		//Then render a mask
		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->BeginScene();
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - BeginScene failed.\n" ) );
			Fail();
		}
		QuickSampleTest( 1 );		

		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CToggle::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		QuickSampleTest( 0 );
		
		break;
	}

	if ( m_bMask )
	{
		MaskTest();
	}

	hr = m_pSrcDevice->EndScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CToggle::ExecuteTest() - EndScene failed.\n" ) );
		Fail();
	}
	if ( !DisplayFrame() )
	{
		Fail();
	}

	VerifyResults();

	return true;
}

void CToggle::QuickSampleTest( UINT iYOffset )
{
	HRESULT hr = S_OK;
	float fSampleSize = SUBPIX_FOURBIT;
	float fOffset = 0.0f;
	UINT Y = 0, X = 0;

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}
	
	FLEXVERTEX fvSubPix[] =
	{ 
		{ 0.00f,		0.00f,			1.00f, 1.00f, WHITE },
		{ fSampleSize,	0.00f,			1.00f, 1.00f, WHITE },
		{ 0.00f,		fSampleSize,	1.00f, 1.00f, WHITE },
		{ fSampleSize,	fSampleSize,	1.00f, 1.00f, WHITE }
	};

	if ( fOffset != 0.0f || 0 != iYOffset )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fOffset + iYOffset;
			fvSubPix[i].fPx += fOffset;
		}
	}

	//For a quick 4-bit precision test, we're going to render to the top 256 pixels.
	for ( Y = 0; Y < (1/fSampleSize); Y++ )
	{
		for ( X = 0; X < (1/fSampleSize); X++ )
		{
			hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, fvSubPix, sizeof(_FlexVert) );
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CToggle::QuickSampleTest() - DrawPrimitiveUP failed.\n" ) );
				Fail();
			}

			for ( UINT i = 0; i < 4; i++ )
			{
				//Shift to the next pixel, and adjust sub-pixel location.
				fvSubPix[i].fPx += 1.0f + fSampleSize;
			}
		}
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fSampleSize;
			fvSubPix[i].fPx -= 1.0f;
		}
	}
	
	return;
}

void CToggle::MaskTest()
{
	HRESULT hr = S_OK;
	float fOffset = 0.0f;

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}
	
	FLEXVERTEX fvSubPix[] =
	{ 
		{ 0.00f,	2.00f,		1.00f, 1.00f, WHITE },
		{ 1.00f,	2.00f,		1.00f, 1.00f, WHITE },
		{ 0.00f,	3.00f,		1.00f, 1.00f, WHITE },
		{ 1.00f,	3.00f,		1.00f, 1.00f, WHITE }
	};

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fOffset;
			fvSubPix[i].fPx += fOffset;
		}
	}

	//Set a less than full mask
	hr = m_pSrcDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] - 1 );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CToggle::MaskTest() - SetRenderState failed.\n" ) );
		Fail();
	}

	hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, fvSubPix, sizeof(_FlexVert) );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CToggle::MaskTest() - DrawPrimitiveUP failed.\n" ) );
		Fail();
	}
}

void CToggle::VerifyResults()
{
	UINT		uiSampleCount = 0;
	UINT		uiSampleNonMS = 0;
	bool		bMask = true;
	HRESULT		hr = S_OK;
    CSurface	*pFBSurf = NULL;

	BYTE		*pFB;
	LOCKED_RECT	*pFBLock;

	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CToggle::VerifyResults() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}

	//Check sample point count
	pFB = (BYTE*) pFBLock->pBits;
	for ( UINT i = 0; i < 256; i++ ) // 256 pixels
	{
		if ( BLACK != *((DWORD*)pFB) )
		{
			//We have a non-black color, increment the return value to indicate a located sampling point
			uiSampleCount++;
		}

		pFB += 4;
	}

	//Check non-multisample point
	pFB = (BYTE*) pFBLock->pBits + pFBLock->iPitch;
	for ( UINT i = 0; i < 256; i++ ) // 256 pixels
	{
		if ( BLACK != (WHITE & *((DWORD*)pFB)) )
		{
			//We have a non-black color, increment the return value to indicate a located sampling point
			uiSampleNonMS++;
		}

		pFB += 4;
	}

	//Check mask point
	pFB = (BYTE*)pFBLock->pBits + ( 2 * pFBLock->iPitch );
	if ( ( BLACK == (WHITE & *((DWORD*)pFB)) || WHITE == (WHITE & *((DWORD*)pFB)) ) )
	{
		bMask = false;
	}

	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CToggle::VerifyResults() - UnLockRect failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	//Now lets take a look at what we discovered
	if ( 2 > uiSampleCount )
	{
		WriteToLog( "Failed to enable multisampling, only one sample point detected.\n\n" );
		Fail();
	}
	if ( 1 != uiSampleNonMS )
	{
		WriteToLog( "Failed to disable multisampling, more than 1 sample point detected.\n\n" );
		Fail();
	}
	if ( m_bMask && !bMask )
	{
		WriteToLog( "Failed to render mask.\n\n" );
		Fail();
	}

	RELEASE( pFBSurf );
Exit:
    return;
}