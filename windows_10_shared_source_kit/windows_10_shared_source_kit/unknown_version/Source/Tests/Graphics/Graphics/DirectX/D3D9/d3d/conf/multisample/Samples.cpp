// FILE:        Samples.cpp
// DESC:        multisample sample verification conformance test
//				Verifies that the appropriate number of sampling locations exist per-pixel

#include "multisample.h"
#include "CSurface.h"
#include "strsafe.h"

// NAME:            CSamples()
// DESC:            contructor for sample class
// INPUT:           none
// OUTPUT:          none

CSamples::CSamples()
{
	m_szTestName = _T( "# Sample Verification" );
    m_szCommandKey = _T( "Samples" );

#ifndef FAST_TEST
	m_Options.D3DOptions.dwWinMode = WINMODE_ALL;
#endif

	m_fSampleSize = SUBPIX_FOURBIT;
}

// NAME:        ~CSamples()
// DESC:        destructor for multisample class
// INPUT:       none
// OUTPUT:      none

CSamples::~CSamples()
{
}

// NAME:        CommandLineHelp()
// DESC:        Note the command line options for the test
// INPUT:       none
// OUTPUT:      none

void CSamples::CommandLineHelp()
{
	WriteCommandLineHelp("$yFullTest: $wForce exhaustive sample test.");
	WriteCommandLineHelp("$yVerbose: $wEnable verbose logging.");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT_* code
// COMMENTS:    skip non multisample modes

UINT CSamples::TestInitialize(void)
{
	UINT uiRetVal = D3DTESTINIT_RUN;

	SetTestRange((UINT) 1, (UINT) 1);

	if ( m_bGetSamplePoints )
	{
		UINT nSamples = m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample];

		SetWindowPos( m_pFramework->m_hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

		if ( MULTISAMPLE_NONE == nSamples )
			uiRetVal = D3DTESTINIT_SKIPALL;
	}
	else
		uiRetVal = CMultisample::TestInitialize();

	return uiRetVal;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       nTest....test number
// OUTPUT:      true

bool CSamples::ExecuteTest(UINT nTest)
{    
	HRESULT hr = S_OK;
	TCHAR *pTestName;

	size_t strSize, strTemp;
	StringCchLength(m_szTestName, STRSAFE_MAX_CCH, &strTemp);
	StringCchLength(m_pCurrentMode->bWindowed?_T(" - Windowed"):_T(" - Fullscreen"), STRSAFE_MAX_CCH, &strSize);

	strSize += ++strTemp;

	pTestName = new TCHAR[strSize + 1];
	StringCchPrintf( pTestName, strSize, "%s%s", m_szTestName, m_pCurrentMode->bWindowed?_T(" - Windowed"):_T(" - Fullscreen"));

	BeginTestCase( pTestName, nTest );

	//Setting up the render states and whatnot
	{
		hr = m_pSrcDevice->SetFVF( FLEXFMT );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetFVF failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_FLAT);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_ZENABLE, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
	}
	
	if ( m_bGetSamplePoints )
		//We're going to run the special test.
	{
		GetPointsTest();
	}
	else if ( m_bForceSlow || !QuickTest() )
	{
		//If we failed the quicktest, try the exhaustive test.
		//We may have failed because they are shifting the sampling points
		if ( !ExhaustiveTest() )
		{
			//Nevermind, they just fail...
			Fail();
			return false;
		}
	}

	if (pTestName)
		delete[] pTestName;
	return true;
}

// NAME:        Render()
// DESC:        Renders the scene
// INPUT:       pfvSubPix....Primitives to render
// OUTPUT:      none
void CSamples::Render( PFLEXVERTEX pfvSubPix )
{
	FLEXVERTEX ModVert[4];
	HRESULT hr = S_OK;

	hr = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, BLACK, 1.0f, 0 );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::Render() - Clear failed.\n" ) );
		Fail();
	}
	hr = m_pSrcDevice->BeginScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::Render() - BeginScene failed.\n" ) );
		Fail();
	}

	//Take our sub-pixel quad, and adjust the vertices to place this within several pixels in the rendertarget
	//TODO: Determine the layout and number of pixels to test
	for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
	{
		memcpy( ModVert, pfvSubPix, sizeof(_FlexVert) * 4 );

		for ( UINT j = 0; j < 4; j++ )
		{
			ModVert[j].fPx += TestPoint[i].x;
			ModVert[j].fPy += TestPoint[i].y;
		}

		hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, ModVert, sizeof(_FlexVert) );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CSamples::Render() - DrawPrimitiveUP failed.\n" ) );
			Fail();
		}
	}

	hr = m_pSrcDevice->EndScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::Render() - EndScene failed.\n" ) );
		Fail();
	}
	if ( !DisplayFrame() )
	{
		Fail();
	}
}

// NAME:        WasSampled()
// DESC:        Determines if we found a sample point.
// INPUT:       none
// OUTPUT:      true or false
void CSamples::WasSampled( BOOL * rgbResults )
{
	HRESULT		hr = S_OK;
    CSurface	*pFBSurf = NULL;

	BYTE		*pFB;
	LOCKED_RECT	*pFBLock;

	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::SampleCount() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}

	//Check results for each pixel tested
	for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
	{
		*rgbResults = false;
		pFB = (BYTE*)pFBLock->pBits;
		pFB += ( TestPoint[i].y * pFBLock->iPitch ) + ( TestPoint[i].x * 4 );

		if ( BLACK != (WHITE & *((DWORD*)pFB)) )
		{
			//We have a non-black color
			*rgbResults = true;
		}
		rgbResults++;
	}

	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::WasSampled() - UnLockRect failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	RELEASE( pFBSurf );
Exit:
    return;
}

// NAME:        SampleCount()
// DESC:        Determines if we found a sample point.
// INPUT:       none
// OUTPUT:      Number of pixels with a non-black value

UINT CSamples::SampleCount()
{
	HRESULT		hr = S_OK;
	UINT		uiRetVal = 0;

    CSurface	*pFBSurf = NULL;
	BYTE		*pFB;
	LOCKED_RECT	*pFBLock;

	DWORD		dwPixels = int( 1/m_fSampleSize );

	UINT X, Y;

	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::SampleCount() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}


	//Check results

	pFB = (BYTE*) pFBLock->pBits;

	for ( Y = 0; Y < dwPixels; Y++ ) //16 pixels high
	{
		for ( X = 0; X < dwPixels; pFB += 4, X++ ) //By 16 pixels wide, for 4-bit anyway
		{
			if ( BLACK != (WHITE &*((DWORD*)pFB)) )
			{
				//We have a non-black color, increment the return value to indicate a located sampling point
				uiRetVal++;
			}
		}
		
		//Drop to the next line 
		pFB += pFBLock->iPitch - ( X * 4 );
	}
	
	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::SampleCount() - UnLockRect failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	if ( false )
	{
		SaveSurface( pFBSurf );
	}
	
Exit:
	RELEASE( pFBSurf );
	
    return uiRetVal;
}

// NAME:        ExhaustiveTest()
// DESC:        Tests 256 sub-pixel locations within several test pixels.
// INPUT:       none
// OUTPUT:      Pass or fail

bool CSamples::ExhaustiveTest()
{
	char szTempNum[4];
	char szTempLog[MAXBUFFER];
	bool bLogTemp = false;
	UINT uiSamplePointCount = 0;
	HRESULT hr = S_OK;
	bool bRetVal = true;
	float fOffset = 0.0f;
	UINT Y = 0, X = 0;
	BOOL rgbSamples[TEST_POINT_COUNT];
	UINT rguiResults[TEST_POINT_COUNT];

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}

	FLEXVERTEX fvSubPix[] =
	{ 
		{ 0.00f, 0.00f, 1.00f, 1.00f, WHITE },
		{ m_fSampleSize, 0.00f, 1.00f, 1.00f, WHITE },
		{ 0.00f, m_fSampleSize, 1.00f, 1.00f, WHITE },
		{ m_fSampleSize, m_fSampleSize, 1.00f, 1.00f, WHITE }
	};

	{
		//Test the four corners
		TestPoint[0].x = 0;											TestPoint[0].y = 0;
		TestPoint[1].x = m_pCurrentMode->nBackBufferWidth -1;		TestPoint[1].y = 0;
		TestPoint[2].x = 0;											TestPoint[2].y = m_pCurrentMode->nBackBufferHeight -1;
		TestPoint[3].x = m_pCurrentMode->nBackBufferWidth -1;		TestPoint[3].y = m_pCurrentMode->nBackBufferHeight -1;

		//Test the four centers
		TestPoint[4].x = m_pCurrentMode->nBackBufferWidth / 2;		TestPoint[4].y = 0;
		TestPoint[5].x = m_pCurrentMode->nBackBufferWidth -1;		TestPoint[5].y = m_pCurrentMode->nBackBufferHeight / 2;
		TestPoint[6].x = m_pCurrentMode->nBackBufferWidth / 2;		TestPoint[6].y = m_pCurrentMode->nBackBufferHeight -1;
		TestPoint[7].x = 0;											TestPoint[7].y = m_pCurrentMode->nBackBufferHeight / 2;

		//Test four closer center points
		TestPoint[8].x = m_pCurrentMode->nBackBufferWidth / 4;		TestPoint[8].y = m_pCurrentMode->nBackBufferHeight / 4;
		TestPoint[9].x = m_pCurrentMode->nBackBufferWidth * 3 / 4;	TestPoint[9].y = m_pCurrentMode->nBackBufferHeight / 4;
		TestPoint[10].x = m_pCurrentMode->nBackBufferWidth * 3 / 4;	TestPoint[10].y = m_pCurrentMode->nBackBufferHeight * 3 / 4;
		TestPoint[11].x = m_pCurrentMode->nBackBufferWidth / 4;		TestPoint[11].y = m_pCurrentMode->nBackBufferHeight * 3 / 4;

		//Test the center
		TestPoint[12].x = m_pCurrentMode->nBackBufferWidth / 2;		TestPoint[12].y = m_pCurrentMode->nBackBufferHeight / 2;
	}

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fOffset;
			fvSubPix[i].fPx += fOffset;
		}
	}

	for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
	{
		rguiResults[i] = 0;
	}

	//For an exhaustive test, we are going to render 256 sub-pixel points to several pixels
	for ( Y = 0; Y < (1/m_fSampleSize); Y++ )
	{
		for ( X = 0; X < (1/m_fSampleSize); X++ )
		{
			Render( fvSubPix );

			WasSampled( rgbSamples );

			if ( m_bVerbose )
			{
				bLogTemp = false;
				
				hr = StringCchPrintf( szTempLog, MAXBUFFER, _T( "Sample found at sub-pixel: ( Left:%f Top:%f Right:%f Bottom:%f )\n" ),
							fvSubPix[0].fPx, fvSubPix[0].fPy, fvSubPix[3].fPx, fvSubPix[3].fPy );
				hr = StringCchCat( szTempLog, MAXBUFFER, _T( "Found with the following pixel(s): " ) );
			}

			for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
			{
				if ( rgbSamples[i] )
				{
					rguiResults[i]++;
					
					if ( m_bVerbose )
					{
						bLogTemp = true;

						hr = StringCchCat( szTempLog, MAXBUFFER, _itoa( i, szTempNum, 10 ) );
						hr = StringCchCat( szTempLog, MAXBUFFER, " " );
					}
				}
			}

			if ( m_bVerbose && bLogTemp )
			{
				hr = StringCchCat( szTempLog, MAXBUFFER, "\n\n" );
				if ( SUCCEEDED(hr) )
				{
					WriteToLog( szTempLog );
				}
				else
				{
					DPF( 1, _T( "Unable to format results" ) );
				}
			}

			for ( UINT i = 0; i < 4; i++ )
			{
				//Adjust the sub-pixel location horizontally.
				fvSubPix[i].fPx += m_fSampleSize;
			}
		}
		for ( UINT i = 0; i < 4; i++ )
		{
			//Adjust the sub-pixel location vertically and reset it horizontally.
			fvSubPix[i].fPy += m_fSampleSize;
			fvSubPix[i].fPx -= 1.0f;
		}
	}

	
	if ( MULTISAMPLE_NONMASKABLE == m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] && m_dwVersion >= 0x900 )
	{
		for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
		{
			uiSamplePointCount = rguiResults[i];
			if ( 2 > uiSamplePointCount )
			{
				Fail();
				WriteToLog( _T("Incorrect sample count for pixel: %d\n"), i ); 
				WriteToLog( _T("Looking for: At least 2 samples\n") );
				WriteToLog( _T("Found: %d\n\n"), uiSamplePointCount );
				bRetVal = false;
			}
		}
	}
	else
	{
		for ( UINT i = 0; i < TEST_POINT_COUNT; i++ )
		{
			uiSamplePointCount = rguiResults[i];
			
			if ( uiSamplePointCount != (UINT)m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] )
			{
				Fail();
				WriteToLog( _T("Incorrect sample count for pixel: %d\n"), i );
				WriteToLog( _T("Looking for: %d\n"), m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] );
				WriteToLog( _T("Found: %d\n\n"), uiSamplePointCount );
				bRetVal = false;
			}
		}
	}

	return bRetVal;
}

// NAME:        QuickTest()
// DESC:        Tests 256 sub-pixel locations across the first 256 pixels.
// INPUT:       none
// OUTPUT:      Pass or fail

bool CSamples::QuickTest()
{
	UINT uiSamplePointCount = 0;
	HRESULT hr = S_OK;
	float fOffset = 0.0f;
	UINT Y = 0, X = 0;

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}
	
	FLEXVERTEX fvSubPix[] =
	{ 
		{ 0.00f, 0.00f, 1.00f, 1.00f, WHITE },
		{ m_fSampleSize, 0.00f, 1.00f, 1.00f, WHITE },
		{ 0.00f, m_fSampleSize, 1.00f, 1.00f, WHITE },
		{ m_fSampleSize, m_fSampleSize, 1.00f, 1.00f, WHITE }
	};

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvSubPix[i].fPy += fOffset;
			fvSubPix[i].fPx += fOffset;
		}
	}

	hr = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, BLACK, 1.0f, 0 );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::QuickTest() - Clear failed.\n" ) );
		Fail();
	}
	hr = m_pSrcDevice->BeginScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::QuickTest() - BeginScene failed.\n" ) );
		Fail();
	}


	//For a quick 4-bit precision test, we're going to render to the top 256 pixels.
	for ( Y = 0; Y < (1/m_fSampleSize); Y++ )
	{
		for ( X = 0; X < (1/m_fSampleSize); X++ )
		{
			hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, fvSubPix, sizeof(_FlexVert) );
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CSamples::QuickTest() - DrawPrimitiveUP failed.\n" ) );
				Fail();
			}

			for ( UINT i = 0; i < 4; i++ )
			{
				//Shift to the next pixel, and adjust sub-pixel location.
				fvSubPix[i].fPx += 1.0f + m_fSampleSize;
			}
		}
		for ( UINT i = 0; i < 4; i++ )
		{
			//Shift to the next line
			fvSubPix[i].fPy += 1.0f + m_fSampleSize;
			fvSubPix[i].fPx -= ( 1/m_fSampleSize + 1 );
		}
	}
	
	hr = m_pSrcDevice->EndScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::QuickTest() - EndScene failed.\n" ) );
		Fail();
	}
	
	if ( !DisplayFrame() )
	{
		Fail();
	}

	uiSamplePointCount = SampleCount();

	if ( MULTISAMPLE_NONMASKABLE == m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] && m_dwVersion >= 0x900 )
	{
		if ( 2 > uiSamplePointCount )
		{
			WriteToLog( _T("\nLooking for: At least 2 samples\nFound: %d\n\n"), uiSamplePointCount );
			WriteToLog( _T("Running exhaustive test.\n") );
			return false;
		}
	}
	else if ( uiSamplePointCount != (UINT)m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] )
	{
		WriteToLog( _T("\nLooking for: %d\nFound: %d\n\n"), m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample], uiSamplePointCount );
		WriteToLog( _T("Running exhaustive test.\n") );
		return false;
	}

	return true;
}

bool CSamples::ParseCmdLine()
{
	if ( KeySet( "FullTest" ) )
		m_bForceSlow = true;
	else
		m_bForceSlow = false;

	return CMultisample::ParseCmdLine();
}