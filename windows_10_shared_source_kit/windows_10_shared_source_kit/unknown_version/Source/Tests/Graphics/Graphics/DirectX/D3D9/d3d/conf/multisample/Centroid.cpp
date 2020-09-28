// FILE:        centroid.cpp
// DESC:        multisample testing w/centroid

#include "multisample.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "cd3dx.h"
#include "CSurface.h"
#include "CTextures.h"

const DWORD CENTFLEXFMT = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 ); // FVF format

typedef struct _CentVert
{
    float fPx;
    float fPy;
    float fPz;
	float fPRHW;
    D3DCOLOR Diffuse;
	float    tu, tv;
} CENTVERTEX, *PCENTVERTEX;

// NAME:            CCentroid()
// DESC:            contructor for Centroid class
// INPUT:           none
// OUTPUT:          none

CCentroid::CCentroid()
{
	m_szTestName = _T( "Centroid multisample verification" );
    m_szCommandKey = _T( "Centroid" );

    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_SWVP | DEVICECREATE_STANDARD;
}

// NAME:        ~CCentroid()
// DESC:        destructor for Centroid class
// INPUT:       none
// OUTPUT:      none

CCentroid::~CCentroid()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT_* code
// COMMENTS:    skip non multisample modes

UINT CCentroid::TestInitialize(void)
{
	UINT uiRetVal = CMultisample::TestInitialize();

	if ( D3DTESTINIT_SKIPALL != uiRetVal )
	{
		if ( m_pD3D->GetRuntimeVersion() >= 9.029f ) //If we're on 9.0c
		{
			if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion >= (DWORD)D3DPS_VERSION(3, 0) )
			{
				//PS 3.0 and 2.0
				SetTestRange((UINT) 1, (UINT) 6);
			}
			else if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion >= (DWORD)D3DPS_VERSION(2, 0) )
			{
				//PS 2.0 only
				SetTestRange((UINT) 5, (UINT) 7);
			}
			else
			{
				SetTestRange((UINT) 0, (UINT) 0);
				uiRetVal = D3DTESTINIT_SKIPALL;
			}
		}
		else if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion >= (DWORD)D3DPS_VERSION(3, 0) )
		{
			//PS 3.0 only
			SetTestRange((UINT) 1, (UINT) 4);
		}
		else
		{
			SetTestRange((UINT) 0, (UINT) 0);
			uiRetVal = D3DTESTINIT_SKIPALL;
		}
	}
	else
	{
		SetTestRange((UINT) 0, (UINT) 0);
		RELEASE( pShader );
		RELEASE( ppTexture );
	}

	return uiRetVal;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       nTest....test number
// OUTPUT:      true

bool CCentroid::ExecuteTest(UINT nTest)
{	
	BeginTestCase( m_szTestName, nTest );

	if ( !SetupPixelShader( nTest ) )
	{
		goto Exit;
	}

	if ( !SetupRender( nTest ) )
	{
		goto Exit;
	}

	if ( !Render( nTest ) )
	{
		goto Exit;
	}

	if ( !VerifyResults( nTest ) )
	{
		//It's a failure for PS 2.0 even, if they do centroid sampling on textures when it isn't requested
		if ( 6 == nTest )
		{
            WriteToLog( "If you did not intend on observing the _centroid hint, ignore this warning.\n" );
			if ( KeySet("WHQL") )
				Skip();
			else
                Warn();
		}
		else
		{
			Fail();
		}
	}

Exit:
	RELEASE( pShader );

	return true;
}

bool CCentroid::SetupPixelShader( UINT nTest )
{
	HRESULT			hr = S_OK; 
	LPD3DXBUFFER	pCode = NULL;
	LPD3DXBUFFER	szError = NULL;
	LPTSTR			szCurrentTestShader = NULL;
	LPTSTR			szPS3Col =
		{
			"ps_3_0 "
			"dcl_color v0 "
			"mov oC0, v0 ",
		};
	LPTSTR			szPS3Tex = 
		{
			"ps_3_0 "
			"dcl_texcoord0_centroid v0.xy "
			"dcl_2d s0 "
			"texld r0, v0, s0 "
			"mov oC0, r0 ",
		};
	LPTSTR			szPS3NoTex = 
		{
			"ps_3_0 "
			"dcl_texcoord0 v0.xy "
			"dcl_2d s0 "
			"texld r0, v0, s0 "
			"mov oC0, r0 ",
		};
	LPTSTR			szPS2Tex = 
		{
			"ps_2_0 "
			"dcl_centroid t0.xy "
			"dcl_2d s0 "
			"texld r0, t0, s0 "
			"mov oC0, r0 ",
		};
	LPTSTR			szPS2NoTex = 
		{
			"ps_2_0 "
			"dcl t0.xy "
			"dcl_2d s0 "
			"texld r0, t0, s0 "
			"mov oC0, r0 ",
		};

	
	switch ( nTest )
	{
	case 1:
	case 7:
		//No shader whatsoever, must not do any centroid sampling
		hr = m_pSrcDevice->SetPixelShader( (CPShader*)NULL );
		if ( FAILED( hr ) ) 
		{
			DPF(1, _T("CCentroid::ExecuteTest() - SetPixelShader failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr));
			Fail();
		}
		goto Exit;
	case 2:
		//ps3.0 without centroid, must not do any centroid sampling
		szCurrentTestShader = szPS3NoTex;
		break;
	case 3:
		//ps3.0 with centroid, must do centroid sampling
		szCurrentTestShader = szPS3Tex;
		break;
	case 4:
		//ps3.0 gradient, must do centroid sampling
		szCurrentTestShader = szPS3Col;
		break;
	case 5:
		//ps2.0 without centroid, must not do any centroid sampling
		szCurrentTestShader = szPS2NoTex;
		break;
	case 6:
		//ps2.0 with centroid, may do centroid sampling, not required
		szCurrentTestShader = szPS2Tex;
		break;
	}

	hr = D3DXAssembleShader9( szCurrentTestShader, ( UINT )_tcslen( szCurrentTestShader ), NULL, NULL, 0, &pCode, &szError );
	if ( FAILED( hr ) ) 
	{
		//DPF(1, (LPTSTR)szError->GetBufferPointer() );
		DPF(1, _T("CCentroid::ExecuteTest() - D3DXAssembleShader failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->CreatePixelShader( (DWORD *)pCode->GetBufferPointer(), &pShader );
	if ( FAILED( hr ) ) 
	{
		DPF(1, _T("CCentroid::ExecuteTest() - CreatePixelShader failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetPixelShader( pShader );
	if ( FAILED( hr ) ) 
	{
		DPF(1, _T("CCentroid::ExecuteTest() - SetPixelShader failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		RELEASE( pShader );
		goto Exit;
	}

Exit:
	RELEASE( pCode );
	RELEASE( szError );
	return SUCCEEDED(hr);
}

bool CCentroid::SetupRender( UINT nTest )
{
	HRESULT hr = S_OK;
	//Setting up the render states and whatnot
	hr = m_pSrcDevice->SetFVF( CENTFLEXFMT );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetFVF failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD) D3DSHADE_GOURAUD);
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState(D3DRS_CLIPPING, (DWORD) FALSE);
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD) FALSE);
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD) TRUE);
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetRenderState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_POINT );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetSamplerState failed.\n" ) );
		Fail();
		goto Exit;
	}
	hr = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_POINT );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - SetSamplerState failed.\n" ) );
		Fail();
		goto Exit;
	}
	// For the first and last 3 tests, ensure the texture is solely applied
	if ( nTest != 4 )
	{
		hr = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTextureStageState failed.\n" ) );
			Fail();
			goto Exit;
		}
		hr = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTextureStageState failed.\n" ) );
			Fail();
			goto Exit;
		}
	}
	else
	{
		hr = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTextureStageState failed.\n" ) );
			Fail();
			goto Exit;
		}
		hr = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTextureStageState failed.\n" ) );
			Fail();
			goto Exit;
		}
	}

Exit:
	return SUCCEEDED( hr );
}
bool CCentroid::VerifyResults( UINT nTest )
{
	HRESULT		hr = S_OK;
    CSurface	*pFBSurf = NULL;
	bool		bPass = true;
	LPTSTR		sz30 = "ps_3_0: ";
	LPTSTR		sz20 = "ps_2_0: ";
	LPTSTR		szVersion;

	BYTE		*pFB;
	LOCKED_RECT	*pFBLock;

	if ( 5 > nTest )
		szVersion = sz30;
	else
		szVersion = sz20;

	//Get results
	hr = GetScene( &pFBLock, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CSamples::SampleCount() - GetScene failed. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		Fail();
		goto Exit;
	}
	
	pFB = (BYTE*)pFBLock->pBits;
	
	switch ( nTest )
	{
	case 1:
	case 2:
	case 5:
	case 7:
		//These four tests all must not do any centroid sampling, and the texture sample should fall into the white area
		if ( BLACK == (WHITE & *((DWORD*)pFB)) )
		{
			WriteToLog( _T( "%sCentroid enabled for texture sampling, when not explicitly requested.\n\n" ), szVersion );
			bPass = false;
		}
		break;
	case 3:
	case 6:
		//Texture sample with centroid requested
		if ( BLACK != (WHITE & *((DWORD*)pFB)) )
		{
			WriteToLog( _T( "%sCentroid failed for texture sampling.\n\n" ), szVersion );
			bPass = false;
		}
		break;
	case 4:
		//Centroid sampling on a ps 3.0 gradient
		D3DCOLOR dwIdeal = GRAY;
		pFB += 60 + (10 * pFBLock->iPitch);
		D3DCOLOR dwColor = *((DWORD*)pFB);
		pFB += (90 * pFBLock->iPitch) + 340;
		D3DCOLOR dwNextColor = *((DWORD*)pFB);


		float fWeight;

		if ( m_pCurrentMode->nMultisample == 1 )
		{
			if ( BLACK != dwNextColor )
			{
				fWeight = 0xFF /((float) ( (0xFF - ((byte*)&dwNextColor)[0]) + (0xFF - ((byte*)&dwNextColor)[1]) + (0xFF - ((byte*)&dwNextColor)[2]) ) / 3 );
			}
			else
			{
				fWeight = 1.0;
			}
		}
		else
		{
			fWeight = ( dwNextColor != BLACK )? (float)m_pCurrentMode->nMultisample/(m_pCurrentMode->nMultisample-1): 1.0f;
		}

		for ( UINT i = 0; i < 3; i ++ )
		{
			if ( ((BYTE*)&dwIdeal)[i] > ( ((BYTE*)&dwColor)[i] *  fWeight ) )
			{
				WriteToLog( _T( "%sCentroid failed for diffuse gradient.\n\n" ), szVersion );
				bPass = false;
				break;
			}
		}
		break;
	}

	hr = pFBSurf->UnlockRect();
	if(FAILED(hr))
	{
		DPF(1, _T("CCentroid::SampleCount() - UnLockRect failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
	}

	RELEASE( pFBSurf );

Exit:
	return bPass;
}

bool CCentroid::Setup()
{
	if ( !CreateTexture( 256, 256, 0, 0, FMT_X8R8G8B8, POOL_DEFAULT, &ppTexture, CD3D_SRC ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - CreateTexture failed.\n" ) );
        return false;
	}

	if ( !ppTexture->LoadTexture( 0, "Centroid.bmp", 0.0, TEXLOAD_DEFAULT, CD3D_SRC ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - LoadTexture failed.\n" ) );
		return false;
	}
	
	return true;
}

void CCentroid::Cleanup()
{
	RELEASE( ppTexture );
}
bool CCentroid::Render( UINT nTest )
{
	HRESULT		hr = S_OK;
	float		fOffset = 0.0f;
	UINT		uiPrims = 1;

	PCENTVERTEX pfvPix;

	if ( m_dwVersion <= 0x900 )
	{
		fOffset = -0.5f;
	}
	
	if ( 4 != nTest )
	{
		pfvPix = new CENTVERTEX[12];
		uiPrims = 4;
		
		pfvPix[0].fPRHW = 1.0;		pfvPix[0].fPz = 1.00f; 
		pfvPix[1].fPRHW = 1.0;		pfvPix[1].fPz = 1.00f;
		pfvPix[2].fPRHW = 1.0;		pfvPix[2].fPz = 1.00f;
		pfvPix[3].fPRHW = 1.0;		pfvPix[3].fPz = 1.00f; 
		pfvPix[4].fPRHW = 1.0;		pfvPix[4].fPz = 1.00f;
		pfvPix[5].fPRHW = 1.0;		pfvPix[5].fPz = 1.00f;
		pfvPix[6].fPRHW = 1.0;		pfvPix[6].fPz = 1.00f; 
		pfvPix[7].fPRHW = 1.0;		pfvPix[7].fPz = 1.00f;
		pfvPix[8].fPRHW = 1.0;		pfvPix[8].fPz = 1.00f;
		pfvPix[9].fPRHW = 1.0;		pfvPix[9].fPz = 1.00f; 
		pfvPix[10].fPRHW = 1.0;		pfvPix[10].fPz = 1.00f;
		pfvPix[11].fPRHW = 1.0;		pfvPix[11].fPz = 1.00f;

		pfvPix[0].fPx = 0.00f;		pfvPix[0].fPy = 0.125f;		pfvPix[0].tu = 0.00f;	pfvPix[0].tv = 0.00f;
		pfvPix[1].fPx = 0.875f;		pfvPix[1].fPy = 1.00f;		pfvPix[1].tu = 1.00f;	pfvPix[1].tv = 1.00f;
		pfvPix[2].fPx = 0.00f;		pfvPix[2].fPy = 1.00f;		pfvPix[2].tu = 0.00f;	pfvPix[2].tv = 1.00f;

		pfvPix[3].fPx = 0.00f;		pfvPix[3].fPy = 0.00f;		pfvPix[3].tu = 0.00f;	pfvPix[3].tv = 0.00f;
		pfvPix[4].fPx = 0.875f;		pfvPix[4].fPy = 0.00f;		pfvPix[4].tu = 1.00f;	pfvPix[4].tv = 1.00f;
		pfvPix[5].fPx = 0.00f;		pfvPix[5].fPy = 0.875f;		pfvPix[5].tu = 0.00f;	pfvPix[5].tv = 1.00f;

		pfvPix[6].fPx = 0.125f;		pfvPix[6].fPy = 0.00f;		pfvPix[6].tu = 0.00f;	pfvPix[6].tv = 0.00f;
		pfvPix[7].fPx = 1.00f;		pfvPix[7].fPy = 1.00f;		pfvPix[7].tu = 1.00f;	pfvPix[7].tv = 1.00f;
		pfvPix[8].fPx = 1.00f;		pfvPix[8].fPy = 0.875f;		pfvPix[8].tu = 0.00f;	pfvPix[8].tv = 1.00f;

		pfvPix[9].fPx = 1.00f;		pfvPix[9].fPy = 0.125f;		pfvPix[9].tu = 0.00f;	pfvPix[9].tv = 0.00f;
		pfvPix[10].fPx = 1.00f;		pfvPix[10].fPy = 1.00f;		pfvPix[10].tu = 1.00f;	pfvPix[10].tv = 1.00f;
		pfvPix[11].fPx = 0.125f;	pfvPix[11].fPy = 1.00f;		pfvPix[11].tu = 0.00f;	pfvPix[11].tv = 1.00f;
	}
	else
	{
		pfvPix = new CENTVERTEX[18];
		uiPrims = 6;
		
		pfvPix[0].Diffuse = GRAY;	pfvPix[0].fPRHW = 1.0;		pfvPix[0].fPz = 1.00f; //GRAY
		pfvPix[1].Diffuse = WHITE;	pfvPix[1].fPRHW = 1.0;		pfvPix[1].fPz = 1.00f;
		pfvPix[2].Diffuse = WHITE;	pfvPix[2].fPRHW = 1.0;		pfvPix[2].fPz = 1.00f;

		pfvPix[3].Diffuse = WHITE;	pfvPix[3].fPRHW = 1.0;		pfvPix[3].fPz = 1.00f; 
		pfvPix[4].Diffuse = GRAY;	pfvPix[4].fPRHW = 1.0;		pfvPix[4].fPz = 1.00f;//GRAY
		pfvPix[5].Diffuse = WHITE;	pfvPix[5].fPRHW = 1.0;		pfvPix[5].fPz = 1.00f;

		pfvPix[6].Diffuse = WHITE;	pfvPix[6].fPRHW = 1.0;		pfvPix[6].fPz = 1.00f; 
		pfvPix[7].Diffuse = WHITE;	pfvPix[7].fPRHW = 1.0;		pfvPix[7].fPz = 1.00f;
		pfvPix[8].Diffuse = GRAY;	pfvPix[8].fPRHW = 1.0;		pfvPix[8].fPz = 1.00f;//GRAY

		pfvPix[9].Diffuse =	 WHITE;	pfvPix[9].fPRHW = 1.0;		pfvPix[9].fPz = 1.00f; 
		pfvPix[10].Diffuse = WHITE;	pfvPix[10].fPRHW = 1.0;		pfvPix[10].fPz = 1.00f;
		pfvPix[11].Diffuse = GRAY;	pfvPix[11].fPRHW = 1.0;		pfvPix[11].fPz = 1.00f;//GRAY

		pfvPix[0].fPx = 10.50f;		pfvPix[0].fPy = 10.5625f;
		pfvPix[1].fPx = 11.5f;		pfvPix[1].fPy = 11.00f;
		pfvPix[2].fPx = 9.5f;		pfvPix[2].fPy = 11.00f;

		pfvPix[3].fPx = 10.0f;		pfvPix[3].fPy = 9.5f;
		pfvPix[4].fPx = 10.50f;		pfvPix[4].fPy = 10.50f;
		pfvPix[5].fPx = 10.0f;		pfvPix[5].fPy = 11.5f;

		pfvPix[6].fPx = 9.5f;		pfvPix[6].fPy = 10.00f;
		pfvPix[7].fPx = 11.5f;		pfvPix[7].fPy = 10.00f;
		pfvPix[8].fPx = 10.5625f;	pfvPix[8].fPy = 10.50f;

		pfvPix[9].fPx = 11.00f;		pfvPix[9].fPy = 9.5f;
		pfvPix[10].fPx = 11.00f;	pfvPix[10].fPy = 11.5f;
		pfvPix[11].fPx = 10.5625f;	pfvPix[11].fPy = 10.5625f;

		//Now geometry for the next pixel over, checking the center point for a sampling location
		pfvPix[12].Diffuse = WHITE;	pfvPix[12].fPRHW = 1.0;		pfvPix[12].fPz = 1.00f; 
		pfvPix[13].Diffuse = WHITE;	pfvPix[13].fPRHW = 1.0;		pfvPix[13].fPz = 1.00f;
		pfvPix[14].Diffuse = WHITE;	pfvPix[14].fPRHW = 1.0;		pfvPix[14].fPz = 1.00f;

		pfvPix[15].Diffuse = WHITE;	pfvPix[15].fPRHW = 1.0;		pfvPix[15].fPz = 1.00f; 
		pfvPix[16].Diffuse = WHITE;	pfvPix[16].fPRHW = 1.0;		pfvPix[16].fPz = 1.00f;
		pfvPix[17].Diffuse = WHITE;	pfvPix[17].fPRHW = 1.0;		pfvPix[17].fPz = 1.00f;

		pfvPix[12].fPx = 100.50f;		pfvPix[12].fPy = 100.50f;
		pfvPix[13].fPx = 100.5625f;	pfvPix[13].fPy = 100.50f;
		pfvPix[14].fPx = 100.50f;		pfvPix[14].fPy = 100.5625f;

		pfvPix[15].fPx = 100.5625f;	pfvPix[15].fPy = 100.50f;
		pfvPix[16].fPx = 100.5625f;	pfvPix[16].fPy = 100.5625f;
		pfvPix[17].fPx = 100.50f;		pfvPix[17].fPy = 100.5625f;
	}

	if ( fOffset != 0.0f )
	{
		for ( UINT i = 0; i < ( 3 * uiPrims ); i++ )
		{
			pfvPix[i].fPy += fOffset;
			pfvPix[i].fPx += fOffset;
		}
	}

	hr = m_pSrcDevice->Clear( 0, NULL, D3DCLEAR_TARGET, BLACK, 1.0, 0 );
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - Clear failed.\n" ) );
		Fail();
		goto Exit;
	}

	hr = m_pSrcDevice->BeginScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CCCentroid::ExecuteTest() - BeginScene failed.\n" ) );
		Fail();
		goto Exit;
	}

	//Set the texture if we aren't doing test 4
	if ( 4 != nTest )
	{
		if ( !SetTexture( 0, ppTexture, CD3D_SRC ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTexture failed.\n" ) );
			Fail();
			goto Exit;
		}
	}
	else
	{
		if ( !SetTexture( 0, NULL, CD3D_SRC ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - SetTexture failed.\n" ) );
			Fail();
			goto Exit;
		}
	}

	if ( 4 != nTest )
	{	
		hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, uiPrims, pfvPix, sizeof(_CentVert) );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - DrawPrimitiveUP failed.\n" ) );
			Fail();
			goto Exit;
		}
	}
	else
	{
		//Tile the geometry to compensate for potentially complex reconstruction filters.
		for ( UINT x = 0; x < 10; ++x )
		{
			for ( UINT y = 0; y < 10; ++y )
			{
				hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, uiPrims - 2, pfvPix, sizeof(_CentVert) );
				if ( FAILED( hr ) )
				{
					DPF( 1, _T( "CCCentroid::ExecuteTest() - DrawPrimitiveUP failed.\n" ) );
					Fail();
					goto Exit;
				}

				for ( UINT i = 0; i < 12; ++i )
				{
					pfvPix[i].fPy += 1.0f;
				}
			}
			for ( UINT i = 0; i < 12; ++i )
			{
				pfvPix[i].fPx += 1.0f;
				pfvPix[i].fPy -= 10.0f;
			}
		}

		hr = m_pSrcDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, pfvPix + 12, sizeof(_CentVert) );
		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CCCentroid::ExecuteTest() - DrawPrimitiveUP failed.\n" ) );
			Fail();
			goto Exit;
		}
	}

	hr = m_pSrcDevice->EndScene();
	if ( FAILED( hr ) )
	{
		DPF( 1, _T( "CSamples::Render() - EndScene failed.\n" ) );
		Fail();
		goto Exit;
	}

	if ( !DisplayFrame() )
	{
		Fail();
		goto Exit;
	}

Exit:
	delete[] pfvPix;

	return SUCCEEDED( hr );
}