// FILE:        Mask.cpp
// DESC:        multisample masking conformance test
//				Verifies that the appropriate number of sampling locations exist per-pixel

#include "multisample.h"
#include "CSurface.h"
#include "strsafe.h"

// NAME:            CMask()
// DESC:            contructor for sample class
// INPUT:           none
// OUTPUT:          none

CMask::CMask()
{
	m_szTestName = _T( "Masking Verification" );
    m_szCommandKey = _T( "Mask" );

#ifndef FAST_TEST
	m_Options.D3DOptions.dwWinMode = WINMODE_ALL;
#endif
}

// NAME:        ~CMask()
// DESC:        destructor for multisample class
// INPUT:       none
// OUTPUT:      none

CMask::~CMask()
{
}

// NAME:        CommandLineHelp()
// DESC:        Note the command line options for the test
// INPUT:       none
// OUTPUT:      none

void CMask::CommandLineHelp()
{
	WriteCommandLineHelp("$yVerbose: $wEnable verbose logging.");
}
// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT_* code
// COMMENTS:    skip non multisample modes

UINT CMask::TestInitialize(void)
{
	UINT uiRetVal = CMultisample::TestInitialize();

	SetTestRange((UINT) 1, (UINT) 1);

	if ( m_dwVersion > 0x0800 )
    {
        if ( MULTISAMPLE_NONMASKABLE == m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] )
        {
            return D3DTESTINIT_SKIPALL; 
        }
    }
    else
    {
        if ( m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE )
        {
            return D3DTESTINIT_SKIPALL; 
        }
    }

    return uiRetVal;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       nTest....test number
// OUTPUT:      true

bool CMask::ExecuteTest(UINT nTest)
{
	bool bRetVal = true;
	float fOffset = 0.0f;
	HRESULT hr = S_OK;
	D3DCOLOR Color = BLACK;
	TCHAR *pTestName;

	size_t strSize, strTemp;
	StringCchLength(m_szTestName, STRSAFE_MAX_CCH, &strTemp);
	StringCchLength(m_pCurrentMode->bWindowed?_T(" - Windowed"):_T(" - Fullscreen"), STRSAFE_MAX_CCH, &strSize);

	strSize += ++strTemp;

	pTestName = new TCHAR[strSize];
	StringCchPrintf( pTestName, strSize, "%s%s", m_szTestName, m_pCurrentMode->bWindowed?_T(" - Windowed"):_T(" - Fullscreen"));

	BeginTestCase( pTestName, nTest );

	//Setting up the render states and whatnot
	{
		hr = m_pSrcDevice->SetFVF( FLEXFMT );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetFVF failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_FLAT);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->SetRenderState(D3DRS_ZENABLE, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}

		hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) TRUE);
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
			Fail();
		}
	}

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}

	FLEXVERTEX fvPix[] =
	{ 
		{ 0.0f,	0.0f,	1.00f, 1.00f, WHITE },
		{ 1.0f,	0.0f,	1.00f, 1.00f, WHITE },
		{ 0.0f,	1.0f,	1.00f, 1.00f, WHITE },
		{ 1.0f,	1.0f,	1.00f, 1.00f, WHITE }
	};

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < 4; i++ )
		{
			fvPix[i].fPy += fOffset;
			fvPix[i].fPx += fOffset;
		}
	}
	
	for ( UINT n = 1; n <= m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample]; n++ )
	{
		//We'll loop through all the samples, turning the bitmask up as we go
		hr = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, BLACK, 1.0f, 0 );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - Clear failed.\n" ) );
			Fail();
		}
		hr = m_pSrcDevice->BeginScene();
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - BeginScene failed.\n" ) );
			Fail();
		}

		for ( UINT k = 0; k < n; k ++ )
		{
			hr = m_pSrcDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, 1<<k );
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CMask::ExecuteTest() - SetRenderState failed.\n" ) );
				Fail();
			}

			hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, fvPix, sizeof(_FlexVert) );
			if ( FAILED( hr ) )
			{
				DPF( 1, _T( "CMask::ExecuteTest() - DrawPrimitiveUP failed.\n" ) );
				Fail();
			}
		}

		hr = m_pSrcDevice->EndScene();
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CMask::ExecuteTest() - EndScene failed.\n" ) );
			Fail();
		}
		if ( !DisplayFrame() )
		{
			Fail();
		}

		//Retrieve the new old color, and get the diff between the two
		Color = GetColor( Color );
		if ( 0 == Color )
		{
			Fail();
			WriteToLog( "Multisample masking ended at %d sample(s), instead of at %d.\n\n", n - 1, 
				m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] );
			return false;
		}
		else
		{
			if ( !VerifyDiff( &Color, n, m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample] ) )
			{
				Fail();
				bRetVal = false;
				WriteToLog( "Multisample masking implementation does not follow the factor of k\\n.\n" );
			}
		}
	}
	
	if (pTestName)
		delete[] pTestName;
	return bRetVal;
}

// NAME:        GetDiff()
// DESC:        Get the colorvalue and ensure that it has changed
// INPUT:       ColorOld....Old D3DCOLOR value
// OUTPUT:      D3DCOLOR containing the new color

D3DCOLOR CMask::GetColor( D3DCOLOR OldColor )
{
	D3DCOLOR	Color = BLACK;
	
	HRESULT		hr = S_OK;
    CSurface	*pFBSurf = NULL;

	LOCKED_RECT	*pFBLock;
	
	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CMask::GetColor() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}

	Color = *((DWORD*)pFBLock->pBits );

	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CMask::GetColor() - UnLockRect failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	if ( OldColor == Color )
	{
		//No change, flag failure by return black
		OldColor = Color;
		Color = BLACK;
	}
	else
	{
		OldColor = Color;
	}
	

	RELEASE( pFBSurf );

Exit:
    return Color;
}

// NAME:        VerifyDiff()
// DESC:        Check the current color against the ideal color
// INPUT:       Color....D3DCOLOR value
//              uiSamples....# of samples current masked
//              uiMaxSamples....# of samples total in current mode
// OUTPUT:      bool for pass or fail

bool CMask::VerifyDiff( D3DCOLOR * Color, UINT uiSamples, UINT uiMaxSamples )
{
	HRESULT hr = S_OK;
	bool bRetVal = true;
	UINT uiColors[3] = { 0x00, 0x00, 0x00 };
	UINT uiColorsGammaCorrected[3] = { 0x00, 0x00, 0x00 };
	UINT uiColorRound[3] = { 0x00, 0x00, 0x00 };
	UINT uiColorRoundGammaCorrected[3] = { 0x00, 0x00, 0x00 };
	DWORD dwColorIdeal;
	BYTE yIdeal = 0;
	BYTE yIdealGammaCorrected = 0;
	DWORD dwOldMax[3] = { 0x00, 0x00, 0x00 };
	DWORD dwMax[3] = { 0x00, 0x00, 0x00 };
	BYTE yBits[3] = { 0, 0, 0 };
	char szBackbuffer[MAXBUFFER];
	
	CSurface * ppBackBuffer = NULL;
	SURFACEDESC Desc;
	DISPLAYMODE pMode;
	
	hr = m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &ppBackBuffer );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CMask::VerifyDiff() - GetDisplayMode failed.\n" ) );
		Fail();
		goto Exit;
	}

	hr = ppBackBuffer->GetDesc( &Desc );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CMask::VerifyDiff() - GetDisplayMode failed.\n" ) );
		Fail();
		goto Exit;
	}

	FmtToString( FORMAT_TEXTURE, &Desc.Format, szBackbuffer );

	switch ( (FMT)Desc.Format.d3dfFormat )
	{
	case FMT_A2R10G10B10:
		yBits[0] = 10;
		yBits[1] = 10;
		yBits[2] = 10;
		break;
	case FMT_A8R8G8B8:
		yBits[0] = 8;
		yBits[1] = 8;
		yBits[2] = 8;
		break;
	case FMT_X8R8G8B8: 
		yBits[0] = 8;
		yBits[1] = 8;
		yBits[2] = 8;
		break;
	case FMT_A1R5G5B5:
		yBits[0] = 5;
		yBits[1] = 5;
		yBits[2] = 5;
		break;
	case FMT_X1R5G5B5:
		yBits[0] = 5;
		yBits[1] = 5;
		yBits[2] = 5;
		break;
	case FMT_R5G6B5:
		yBits[0] = 5;
		yBits[1] = 6;
		yBits[2] = 5;
		break;
	default:
		Fail();
		WriteToLog( _T( "Unsupported backbuffer format.") );
		goto Exit;
	}


	for ( UINT i = 0; i < 3; i++ )
	{
		uiColors[i] = ( 1 << yBits[i] ) - 1;

		//cycle through rgb
		dwMax[i] = uiColors[i] + 1;
		uiColorRound[i] = (UINT)floor( 0.5f + ( uiColors[i] * uiSamples / uiMaxSamples ) );
		uiColors[i] = uiColors[i] * uiSamples / uiMaxSamples;		
	}
	
	hr = m_pSrcDevice->GetDisplayMode( &pMode );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CMask::VerifyDiff() - GetDisplayMode failed.\n" ) );
		Fail();
		goto Exit;
	}

	if ( (FMT)Desc.Format.d3dfFormat != (FMT)pMode.Format.d3dfFormat )
	{
		switch ( (FMT)pMode.Format.d3dfFormat )
		{
		case FMT_A2R10G10B10:
			yBits[0] = 10;
			yBits[1] = 10;
			yBits[2] = 10;
		break;
		case FMT_X8R8G8B8: 
			yBits[0] = 8;
			yBits[1] = 8;
			yBits[2] = 8;
		break;
		case FMT_X1R5G5B5:
			yBits[0] = 5;
			yBits[1] = 5;
			yBits[2] = 5;
		break;
		case FMT_R5G6B5:
			yBits[0] = 5;
			yBits[1] = 6;
			yBits[2] = 5;
		break;
		default:
			Fail();
			WriteToLog( _T( "Unsupported backbuffer format.") );
			goto Exit;
		}	

		for ( UINT i = 0; i < 3; i++ )
		{
			dwOldMax[i] = dwMax[i];
			dwMax[i] = 1 << yBits[i];
			
			uiColors[i] = uiColors[i] * dwMax[i] / dwOldMax[i];
			uiColorRound[i] = (UINT)floor( 0.5f + ( uiColorRound[i] * dwMax[i] / dwOldMax[i] ) );
		}
	}

	//Since front buffer grabs always return an 888 surface.  Convert again, if necessary
	if ( FMT_X8R8G8B8 != (FMT) pMode.Format.d3dfFormat )
	{
		for ( UINT i = 0; i < 3; i++ )
		{
			uiColors[i] = uiColors[i] * 256 / dwMax[i];
			uiColorRound[i] = uiColorRound[i] * 256 / dwMax[i];
		}
	}

	yIdeal =  (BYTE)( 255 * uiSamples / uiMaxSamples );

	//WinBlue:24270 - Some IHVs will do MS resolve in SRGB space. So we should compare against gamma corrected colors too.
	yIdealGammaCorrected = (BYTE)(GammaCorrectChannel( float(yIdeal) / 255.0f ) * 255.0f);
	for ( UINT i = 0; i < 3; i++ )
	{
		uiColorsGammaCorrected[i] = (BYTE)(GammaCorrectChannel( float(uiColors[i]) / 255.0f ) * 255.0f);
		uiColorRoundGammaCorrected[i] = (UINT)floor( 0.5f + ( uiColorsGammaCorrected[i] * uiSamples / uiMaxSamples ) );
	}

	for ( UINT i = 0; i < 3; i++ )
	{
		//Use either the rounded or the truncated one, whichever gives us the greater range
		if (( max( 4, max( abs( (long)(yIdeal - uiColors[i]) ), abs( (long)(yIdeal - uiColorRound[i]) ) ) ) < abs( (long)(yIdeal - ((BYTE*)Color)[i]) ) ) &&
			( max( 4, max( abs( (long)(yIdealGammaCorrected - uiColorsGammaCorrected[i]) ), abs( (long)(yIdealGammaCorrected - uiColorRoundGammaCorrected[i]) ) ) ) < abs( (long)(yIdealGammaCorrected - ((BYTE*)Color)[i]) ) ))
		{
			bRetVal = false;
			switch ( i )
			{
				case 0:
					WriteToLog( _T("Color value outside of the acceptable range in blue channel. Actual: %i, Expected: %i or %i (Gamma corrected)"), ((BYTE*)Color)[i], yIdeal, yIdealGammaCorrected );
					break;
				case 1:
					WriteToLog( _T("Color value outside of the acceptable range in green channel. Actual: %i, Expected: %i or %i (Gamma corrected)"), ((BYTE*)Color)[i], yIdeal, yIdealGammaCorrected );
					break;
				case 2:
					WriteToLog( _T("Color value outside of the acceptable range in red channel. Actual: %i, Expected: %i or %i (Gamma corrected)"), ((BYTE*)Color)[i], yIdeal, yIdealGammaCorrected );
					break;
			}
		}
	}

	if ( !bRetVal )
	{
		WriteToLog( _T("With backbuffer format of: D3DFMT_%s\n"), szBackbuffer );
		if ( m_bVerbose )
		{
			dwColorIdeal = ( yIdeal << 16 ) + ( yIdeal << 8 ) + yIdeal;

			WriteToLog( _T("The current color value at %d sample(s) is:\t0x%08X\n"), uiSamples, *Color );
			WriteToLog( _T("The ideal color value at %d sample(s) is:\t0x%08X\n"), uiSamples, dwColorIdeal );
			WriteToLog( _T("Acceptable color diff from ideal is:\t0x00%02X%02X%02X\n"), abs( (int)( yIdeal - uiColors[0] ) ),
				abs( (int)( yIdeal - uiColors[1] ) ), abs( (int)( yIdeal - uiColors[2] ) ) );
		}
		WriteToLog( _T("\n") );
	}
	
Exit:

	RELEASE( ppBackBuffer );

	return bRetVal;
}