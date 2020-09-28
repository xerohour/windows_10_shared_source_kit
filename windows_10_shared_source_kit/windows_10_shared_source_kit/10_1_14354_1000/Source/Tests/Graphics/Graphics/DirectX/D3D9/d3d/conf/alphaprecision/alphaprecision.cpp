// ============================================================================
//
//  alphaprecision.cpp
//  Written by billkris.
//
// ============================================================================

#include "alphaprecision.h"
#include "alphafloatrt.h"


#include "CBuffers.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "cd3dx.h"		//for D3DXAssembleShader9
#include "assert.h"
#include "strsafe.h"

#define TEXSIZE (128)

char CAlphaPrecision::m_MsgString[] = "";


////////////////////////////////////////////////////////////
// Instantiate objects
////////////////////
CD3DWindowFramework App;
CAlphaPrecisionFF AlphaPrecisionFF;
CAlphaPrecisionPS11 AlphaPrecisionPS11;
CAlphaPrecisionPS20 AlphaPrecisionPS20;
CAlphaPrecisionPS30 AlphaPrecisionPS30;
CAlphaPrecisionFloatRTPS20 AlphaPrecisionFloatRTPS20;
CAlphaPrecisionFloatRTPS30 AlphaPrecisionFloatRTPS30;

////////////////////////////////////////////////////////////
// CAlphaPrecision
////////////////////
CAlphaPrecision::CAlphaPrecision() : 
	m_pTargetTexture( NULL ),
	m_pSysMemTexture( NULL ),
	m_pCmpTexture( NULL ),
	m_pCurrentABM( NULL ),
	m_pCurrentPSI( NULL )
{
	SetTestType( TESTTYPE_CONF );
	
	m_Options.ModeOptions.dwTexInclude = PF_RGB;
	m_Options.ModeOptions.dwTexExclude = PF_PALETTEINDEXED1 | PF_PALETTEINDEXED2 | PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8; 
	
#ifdef FAST_TEST
	m_Options.ModeOptions.TextureFmt = FMT_A8R8G8B8;
#else
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
#endif

	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ANY;
	
	// Force test to use Dx9
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	
	m_Options.D3DOptions.nBackBufferWidth = TEXSIZE;
	m_Options.D3DOptions.nBackBufferHeight = TEXSIZE;

    // Init variables
    m_bInvalid = false;
    m_bOffscreen = false;
	m_bNull = false;
    m_bValidate = true;

    if (KeySet("NoValidate"))
        m_bValidate = false;
        
	
	m_ShaderConst0[0] = 1;
	m_ShaderConst0[1] = 1;
	m_ShaderConst0[2] = 1;
	m_ShaderConst0[3] = 1;
}

CAlphaPrecision::~CAlphaPrecision()
{
}

bool CAlphaPrecision::CapsCheck()
{
	FORMAT *pFormatRT;
	
    // Tell framework what formats to use
    pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	
	if( FAILED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
											m_pSrcDevice->GetDevType(), 
											m_pCurrentMode->pDisplay->Format,
											USAGE_RENDERTARGET, 
											RTYPE_TEXTURE, 
											*pFormatRT ) ) )
	{
		return FALSE;
	}
	
	if( FAILED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
											m_pSrcDevice->GetDevType(), 
											m_pCurrentMode->pDisplay->Format,
											USAGE_QUERY_POSTPIXELSHADER_BLENDING, 
											RTYPE_TEXTURE, 
											*pFormatRT ) ) )
	{
		return FALSE;
	}

	return CD3DTest::CapsCheck();
}

UINT CAlphaPrecision::TestInitialize()
{
	if( !CreateAlphaBlendMethods() )
		return D3DTESTINIT_SKIPALL;
		
	UINT maxtests = (UINT) ( m_PixelShaders.size() * m_AlphaBlendMethods.size() );
	
	if( maxtests == 0 )
		return D3DTESTINIT_SKIPALL;

	SetTestRange( 1, maxtests );
	
	// Create foreground/background
	if( !CreateBackground() )
		return D3DTESTINIT_ABORT;
	
	if( !CreateForeground() )
		return D3DTESTINIT_ABORT;
	
	// Create the quad that will display the result in the visible
	// window.
	float xmax = 0.5f + m_pCurrentMode->nBackBufferWidth;
	float ymax = 0.5f + m_pCurrentMode->nBackBufferHeight;

	m_QuadVerts[0] = D3DTLVERTEX( D3DXVECTOR3( 0, ymax, .5f ), ( 1.f / .5f ), 0xffffffff, 0, 0, 1 );
	m_QuadVerts[1] = D3DTLVERTEX( D3DXVECTOR3( 0, 0, .5f ), ( 1.f / .5f ), 0xffffffff, 0, 0, 0 );
	m_QuadVerts[2] = D3DTLVERTEX( D3DXVECTOR3( xmax, ymax, .5f ), ( 1.f / .5f ), 0xffffffff, 0, 1, 1 );
	m_QuadVerts[3] = D3DTLVERTEX( D3DXVECTOR3( xmax, 0, .5f ), ( 1.f / .5f ), 0xffffffff, 0, 1, 0 );

	char formatstring[256];

	FORMAT *pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	FmtToString( FORMAT_TEXTURE, pFormatRT, formatstring );
	if( FAILED( StringCchPrintf( m_MsgString, sizeof( m_MsgString ), "RT Format: %s\n", formatstring ) ) )
		assert( !"String too long" );
	
	WriteToLog( "%s\n", m_MsgString );

	return D3DTESTINIT_RUN;
}

bool CAlphaPrecision::TestTerminate()
{
	// Clear AlphaBlendMethod objects
	m_AlphaBlendMethods.clear();
		
	return CD3DTest::TestTerminate();
}

bool CAlphaPrecision::Setup()
{
	bool bResult;
	
	// Create pixel shaders
	if( !CreatePixelShaders() )
		return false;
		
    FORMAT * pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

    int nWidthRT = m_pCurrentMode->nBackBufferWidth;
    int nHeightRT = m_pCurrentMode->nBackBufferHeight;

	// Create texture and set as render target
	bResult = CreateTexture( nWidthRT, nHeightRT, 1, USAGE_RENDERTARGET, *pFormatRT, FMWK_DEFAULT, &m_pTargetTexture );
	if( bResult == false )
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	
	// Create offcreen buffer to copy render results to which will later be used for comparison
	bResult = CreateTexture( nWidthRT, nHeightRT, 1, 0, *pFormatRT, POOL_SYSTEMMEM, &m_pSysMemTexture );
	if( bResult == false )
	{
		WriteToLog("CreateTexture() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		return false;
	}
	
	// Create a compatible texture for displaying comparison results
	bResult = CreateTexture( nWidthRT, nHeightRT, 1, 0, *pFormatRT, POOL_SYSTEMMEM, &m_pCmpTexture );
	if( bResult == false )
	{
		WriteToLog( "CreateTexture() failed with HResult = %s.\n", m_pD3D->HResultToString( GetLastError() ) );
		return false;
	}
	
	switch( pFormatRT->d3dfFormat )
	{
		case D3DFMT_R16F:
		case D3DFMT_G16R16F:
		case D3DFMT_A16B16G16R16F:
			m_fRTMax = D3DX_16F_MAX;
			break;
			
		case D3DFMT_R32F:
		case D3DFMT_G32R32F:
		case D3DFMT_A32B32G32R32F:
			m_fRTMax = FLT_MAX;
			break;
			
		default:
			m_fRTMax = 1024;
			break;
	}

	return CD3DTest::Setup();
}

void CAlphaPrecision::Cleanup()
{
	// Release pixel shaders
	for( int i = (int) m_PixelShaders.size() - 1; i >= 0; --i )
	{
		RELEASE( m_PixelShaders[i].pPixelShader );
	}
	m_PixelShaders.clear();

	// Release render targets	
	RELEASE( m_pTargetTexture );
	RELEASE( m_pSysMemTexture );
	RELEASE( m_pCmpTexture );

	CD3DTest::Cleanup();
}

bool CAlphaPrecision::ExecuteTest( UINT uTestNumber )
{
	bool bResult = true;

	// Change the render target to the floating point texture
	CnSurface *pOldRenderTarget = NULL;
	CnSurface *pOldDepthStencil = NULL;
	CnSurface *pNewRenderTarget = NULL;
	GetRenderTarget( &pOldRenderTarget );
	GetDepthStencilSurface( &pOldDepthStencil );
	
	int nAlphaBlendMethodIndex = ( uTestNumber - 1 ) % m_AlphaBlendMethods.size();
	int nPixelShaderIndex = ( uTestNumber - 1 ) / m_AlphaBlendMethods.size();
	
	bResult = m_pTargetTexture->GetSurfaceLevel( 0, &pNewRenderTarget );
	if( bResult == false )
	{
		WriteToLog("GetSurfaceLevel() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	bResult = SetRenderTarget( pNewRenderTarget, NULL );
	if( bResult == false )
	{
		WriteToLog("SetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}

	VIEWPORT ViewportRT;
	
	// Initialize the render target viewport
	ViewportRT.X = 0;    
	ViewportRT.Y = 0;    
	ViewportRT.Width = m_pCurrentMode->nBackBufferWidth;
	ViewportRT.Height = m_pCurrentMode->nBackBufferHeight;
	ViewportRT.MinZ = 0.f; 
	ViewportRT.MaxZ = 1.f; 
	
	bResult = SetViewport( &ViewportRT );
	if( bResult == false )
	{
		WriteToLog("SetViewport() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		goto Exit;
	}


    FORMAT *pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	char formatstring[256];
	FmtToString( FORMAT_TEXTURE, pFormatRT, formatstring );
	if( FAILED( StringCchPrintf( m_MsgString, sizeof( m_MsgString ), "%s - RT: %s", GetTitle(), formatstring ) ) )
		assert( !"String too long" );
	
	
	// Tell the log that we are starting
	BeginTestCase( m_MsgString, uTestNumber );

	Clear( 0, NULL, D3DCLEAR_TARGET , 0x00000000, 1.f, 0 );

	// Render to the texture RT
	if( BeginScene() )
	{
		// Clear texture state
		SetTexture( 0, NULL );

		// Render the background image
		SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) false );
		SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE);
		SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_DIFFUSE);
		SetFVF( D3DFVF_TLVERTEX );
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, NumBackgroundPrimitives, m_bgVerts, sizeof( D3DTLVERTEX ) );
		
		// Render the foreground image
		m_pCurrentABM  = &m_AlphaBlendMethods[nAlphaBlendMethodIndex];
		SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) true );
		SetRenderState( D3DRS_BLENDOP, m_pCurrentABM->d3dBlendOp );
		SetRenderState( D3DRS_SRCBLEND, m_pCurrentABM->dwSrcBlend );
		SetRenderState( D3DRS_DESTBLEND, m_pCurrentABM->dwDestBlend );
		
		m_pCurrentPSI = &m_PixelShaders[nPixelShaderIndex];
		CnPixelShader *pPixelShader = m_pCurrentPSI->pPixelShader;
		if( pPixelShader )
		{
			SetPixelShader( pPixelShader );
			m_ShaderConst0[0] = m_ShaderConst0[1] = m_ShaderConst0[2] = m_fRTMax / 2;
#if 1
			SetPixelShaderConstantF( 0, m_ShaderConst0, 1 );
#else
			SetPixelShaderConstantF( 0, m_ShaderConst0, 1, CD3D_REF );
			
			m_ShaderConst0[0] = m_ShaderConst0[1] = m_ShaderConst0[2] = m_fRTMax * .97f / 2;
			m_ShaderConst0[3] = .997f;
			SetPixelShaderConstantF( 0, m_ShaderConst0, 1, CD3D_SRC );
#endif
		}
		
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, NumBackgroundPrimitives, m_fgVerts, sizeof( D3DTLVERTEX ) );

		if( pPixelShader )
			SetPixelShader( ( CnPixelShader *) NULL );

		EndScene();
	}
	
	// Restore the render target
	SetRenderTarget( pOldRenderTarget, pOldDepthStencil );
	
	SetDefaultViewport();
	
	// Display the contents of the render target texture
	Clear( 0, NULL, D3DCLEAR_TARGET, 0xff000000, 1.f, 0 );
	
	if( BeginScene() )
	{		
		SetTexture( 0, m_pTargetTexture );
		
		SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);
		SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

		SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) false );
		
		SetFVF( D3DFVF_TLVERTEX );
		DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_QuadVerts, sizeof( D3DTLVERTEX ) );
		
		EndScene();
	}

Exit:	

	RELEASE( pNewRenderTarget );
	RELEASE( pOldRenderTarget );
	RELEASE( pOldDepthStencil );
	
	return bResult;
}

void CAlphaPrecision::UpdateStatus()
{
	CD3DTest::UpdateStatus();
	
	WriteStatus( "$wSrcBlend", "$a%s", m_pCurrentABM->strSrcBlend.c_str() );
	WriteStatus( "$wDestBlend", "$a%s", m_pCurrentABM->strDestBlend.c_str() );
	WriteStatus( "$wBlendOp", "$a%s", m_pCurrentABM->strBlendOp.c_str() );
	WriteStatus( "$wPS Output", "$a%s", m_pCurrentPSI->strDesc.c_str() );
}


void CAlphaPrecision::UpdateDetails()
{
	// BUGBUG - TODO...
	CD3DTest::UpdateDetails();
}


////////////////////////////////////////////////////////////
// Override ProcessFrame to compare the src and ref surfaces of the render target texture
// rather than the display back buffer.
//////////////////////////////
bool CAlphaPrecision::ProcessFrame(void)
{
	bool bRes = true;
    CSurface *pSrcSurf = NULL, *pRefSurf = NULL, *pCmpSurf = NULL;
	CnSurface *pRTSurface = NULL;
	CnSurface *pSysMemSurface = NULL;
	CnSurface *pCmpSurface = NULL;

	if (m_Options.D3DOptions.bReference && IsCompareEnabled())
	{
		// Copy the results of the render into the system memory texture
		if( !m_pTargetTexture->GetSurfaceLevel( 0, &pRTSurface ) )
		{
			WriteToLog("GetSurfaceLevel() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			bRes = false;
			goto Exit;
		}
		
		if( !m_pSysMemTexture->GetSurfaceLevel( 0, &pSysMemSurface ) )
		{
			WriteToLog("GetSurfaceLevel() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			bRes = false;
			goto Exit;
		}
		
		if( !GetRenderTargetData( pRTSurface, pSysMemSurface ) )
		{
			WriteToLog("GetRenderTargetData() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			bRes = false;
			goto Exit;
		}
		
		if( !pSysMemSurface->GetSurface( &pRefSurf, 1 ) )
		{
			WriteToLog("GetSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			bRes = false;
			goto Exit;
		}
		
		if( !pSysMemSurface->GetSurface( &pSrcSurf, 0 ) )
		{
			WriteToLog("GetSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			bRes = false;
			goto Exit;
		}
		
		if(m_bShowCompareWindow)
		{
			if( !m_pCmpTexture->GetSurfaceLevel( 0, &pCmpSurface ) )
			{
				WriteToLog("GetSurfaceLevel() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				bRes = false;
				goto Exit;
			}
			
			if( !pCmpSurface->GetSurface( &pCmpSurf, 0 ) )
			{
				WriteToLog("GetSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				bRes = false;
				goto Exit;
			}
		}		
		
        // Compare the buffers
        CImageCompare *pImageCompare = m_pFramework->GetImageCmp();
        pImageCompare->SetGenerateCompare( m_bShowCompareWindow );
        pImageCompare->SetIncClearColor( true );
        if(FAILED(pImageCompare->CompareImages(pSrcSurf, pRefSurf, pCmpSurf )))
        {
            DPF(1, _T("CD3DTest::ProcessFrame() - CompareImages failed.\n"));
            Fail();
            goto Exit;
        }
        
        // Convert the compare results to the compare window format
		if(m_bShowCompareWindow && m_pCmpSwapChain)
		{
			CSurface *pCmpSurfDest;
			HRESULT hr = m_pCmpSwapChain->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pCmpSurfDest);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame() - GetBackBufferCached failed.\n"));
				bRes = false;
				goto Exit;
			}
			
			// Convert to output comparison window format
			FSurface *tempSurface = new FSurface();
			tempSurface->ConvertFrom( pCmpSurf );
			tempSurface->ConvertTo( pCmpSurfDest );
			
			RELEASE( pCmpSurfDest );
			delete( tempSurface );
		}


        // Check the results
        
        m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();
        
        if(m_pFramework->m_DXGLog.InTestCase())
        {
            if(m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired())
                Fail();
            else
                Pass();
        }
   }

Exit:
 	RELEASE( pRTSurface );
	RELEASE( pSysMemSurface );
	RELEASE( pCmpSurface );
	RELEASE( pSrcSurf );
	RELEASE( pRefSurf );
	RELEASE( pCmpSurf );

	return bRes;
}



//
// Protected methods
//
bool CAlphaPrecision::CreateBackground()
{
	// Create a triangle strip with horizontal bands of colors
	DWORD dwWidth = (DWORD) m_pCurrentMode->nBackBufferWidth;
	DWORD dwHeight = (DWORD) m_pCurrentMode->nBackBufferHeight;
	float fHeight = (float) dwHeight;
	
	int nCount = 0;
	for( int i = 0; i <= BackgroundDensity; ++i )
	{
		float y = ( fHeight * i ) / BackgroundDensity;
		
		// Calculate color value for this vertex
		float fPct = i / (float) BackgroundDensity;
		float fRed = max( 0, 1 - fPct * 2 );
		float fBlue = max( 0, fPct * 2 - 1 );
		float fGreen = fPct < .5 ? ( 1 - fRed ) : ( 1 - fBlue );
		
		// Normalize the color
		float invmag = 1.f / sqrtf( fRed * fRed + fGreen * fGreen + fBlue * fBlue );
		fRed *= invmag;
		fBlue *= invmag;
		fGreen *= invmag;
		
		assert( fRed >= 0 && fRed <= 1.f );
		assert( fBlue >= 0 && fBlue <= 1.f );
		assert( fGreen >= 0 && fGreen <= 1.f );
		
		// Convert to 8-bit fixed point
		int nRed = (int) ( fRed * 255 );
		int nGreen = (int) ( fGreen * 255 );
		int nBlue = (int) ( fBlue * 255 );
		
		D3DCOLOR color0 = RGBA_MAKE( nRed, nGreen, nBlue, 255 );
		D3DCOLOR color1 = RGBA_MAKE( nRed, nGreen, nBlue, 255 );
		
		m_bgVerts[nCount++] = D3DTLVERTEX( D3DXVECTOR3( 0.f, y, .5f ), ( 1.0f / .5f ), color0, 0, 0, 0 );
		m_bgVerts[nCount++] = D3DTLVERTEX( D3DXVECTOR3( (float) dwWidth, y, .5f ), ( 1.0f / .5f ), color1, 0, 0, 0 );
	}
	
	return true;
}



bool CAlphaPrecision::CreateForeground()
{
	// Create a triangle strip with vertical bands of colors
	DWORD dwHeight = (DWORD) m_pCurrentMode->nBackBufferHeight;
	DWORD dwWidth = (DWORD) m_pCurrentMode->nBackBufferWidth;
	float fWidth = (float) dwWidth;
	
	int nCount = 0;
	for( int i = 0; i <= ForegroundDensity; ++i )
	{
		float x = ( fWidth * i ) / ForegroundDensity;
		
		// Calculate color value for this vertex
		float fPct = i / (float) ForegroundDensity;
		float fRed = max( 0, 1 - fPct * 2 );
		float fBlue = max( 0, fPct * 2 - 1 );
		float fGreen = fPct < .5 ? ( 1 - fRed ) : ( 1 - fBlue );
		
		// Normalize the color
		float invmag = 1.f / sqrtf( fRed * fRed + fGreen * fGreen + fBlue * fBlue );
		fRed *= invmag;
		fBlue *= invmag;
		fGreen *= invmag;
		
		assert( fRed >= 0 && fRed <= 1.f );
		assert( fBlue >= 0 && fBlue <= 1.f );
		assert( fGreen >= 0 && fGreen <= 1.f );
		
		// Convert to 8-bit fixed point
		int nRed = (int) ( fRed * 255 );
		int nGreen = (int) ( fGreen * 255 );
		int nBlue = (int) ( fBlue * 255 );
		
		D3DCOLOR color0 = RGBA_MAKE( nRed, nGreen, nBlue, 0 );
		D3DCOLOR color1 = RGBA_MAKE( nRed, nGreen, nBlue, 255 );
		
		m_fgVerts[nCount++] = D3DTLVERTEX( D3DXVECTOR3( x, (float) dwHeight, .5f ), ( 1.0f / .5f ), color1, 0, 0, 0 );
		m_fgVerts[nCount++] = D3DTLVERTEX( D3DXVECTOR3( x, 0.f, .5f ), ( 1.0f / .5f ), color0, 0, 0, 0 );
	}
	
	return true;
}

// Helpful macro to help define and add a blend method
#define ADD_BLEND_METHOD( src, dest, op ) \
	if( ( dwSrcBlendCaps & D3DPBLENDCAPS_##src ) && ( dwDestBlendCaps & D3DPBLENDCAPS_##dest ) ) \
	{ \
		abm.dwSrcBlend = D3DBLEND_##src; \
		abm.dwDestBlend = D3DBLEND_##dest; \
		abm.d3dBlendOp = D3DBLENDOP_##op; \
		abm.strSrcBlend = #src; \
		abm.strDestBlend = #dest; \
		abm.strBlendOp = #op; \
		m_AlphaBlendMethods.push_back( abm ); \
	}
	
// Default implementation of CreateAlphaBlendMethods()
bool CAlphaPrecision::CreateAlphaBlendMethods()
{
	AlphaBlendMethod abm;
	DWORD dwSrcBlendCaps = m_pSrcDevice->GetCaps()->dwSrcBlendCaps;
	DWORD dwDestBlendCaps = m_pSrcDevice->GetCaps()->dwDestBlendCaps;
	
	// Default blend operations
	abm.d3dBlendOp = D3DBLENDOP_ADD;
	
	ADD_BLEND_METHOD( SRCALPHA, INVSRCALPHA, ADD );
	ADD_BLEND_METHOD( SRCALPHA, SRCALPHA, ADD );
	ADD_BLEND_METHOD( ONE, ONE, ADD );
	ADD_BLEND_METHOD( ZERO, SRCCOLOR, ADD );
	
	// See of alternative blend ops are supported
	if( m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP )
	{
		ADD_BLEND_METHOD( SRCALPHA, INVSRCALPHA, SUBTRACT );
		ADD_BLEND_METHOD( INVSRCALPHA, SRCALPHA, SUBTRACT );
		ADD_BLEND_METHOD( ONE, ONE, SUBTRACT );
		ADD_BLEND_METHOD( ONE, ONE, REVSUBTRACT );
	}
		
	return true;
}




//
// CAlphaPrecisionFF
//
CAlphaPrecisionFF::CAlphaPrecisionFF()
{
	m_szTestName = "Alpha Precision - Fixed Function Pixel Pipeline";
	m_szCommandKey = "AlphaFF";
}

bool CAlphaPrecisionFF::CreatePixelShaders()
{
	// Push back a null shader.  This will result in using the fixed function
	// pixel pipeline.
	PixelShaderInfo psi;
	psi.pPixelShader = NULL;
	psi.strDesc = "Fixed Function";
	m_PixelShaders.push_back( psi );
	
	return true;
}






//
// CAlphaPrecisionPS11
//
CAlphaPrecisionPS11::CAlphaPrecisionPS11()
{
	m_szTestName = "Alpha Precision - Pixel Shader V1.1";
	m_szCommandKey = "AlphaPS11";
}

bool CAlphaPrecisionPS11::CapsCheck()
{
 	DWORD dwVer = D3DSHADER_VERSION_MAJOR( m_pSrcDevice->GetCaps()->dwPixelShaderVersion );
 	
 	if( dwVer < 1 )
 		return false;
 	
 	if( dwVer == 1 )
 	{	
 		dwVer = D3DSHADER_VERSION_MINOR( m_pSrcDevice->GetCaps()->dwPixelShaderVersion );
	 	
 		if( dwVer < 1 )
 			return false;
	}
	 	
 	return CAlphaPrecision::CapsCheck();

}

bool CAlphaPrecisionPS11::CreatePixelShaders()
{
	HRESULT hr;
    struct ShaderDesc
    {
		const TCHAR *buffer;
		const char *desc;
    };
    
    CnPixelShader *pPixelShader = NULL;
	
	const ShaderDesc shaders[] =
	{
		{
			"ps.1.1\n"
			"mov r0, v0\n"
			,
			"r0 = v0"
		},
		{
			"ps.1.1\n"
			"def c0, 2, 2, 2, 2\n"
			"mul r0, v0, c0\n"
			,
			"r0 = 2 * v0"
		},
		{
			"ps.1.1\n"
			"def c0, .5, .5, .5, .5\n"
			"mul r0, v0, c0\n"
			,
			"r0 = .5f * v0"
		},
		{
			"ps.1.1\n"
			"def c0, -2, -2, -2, -2\n"
			"mul r0, v0, c0\n"
			,
			"r0 = -2 * v0"
		},
		{
			"ps.1.1\n"
			"mul r0, v0, c0\n"
			,
			"r0.rgb = RT_FLT_MAX * .5 * v0, r0.a = v0.a"
		},
	};
	
	int nNumShaders = sizeof( shaders ) / sizeof( ShaderDesc );
	
	for( int i = 0; i < nNumShaders; ++i )
	{
		LPD3DXBUFFER pXCode = NULL;
		LPD3DXBUFFER pXError = NULL;
	    
		hr = D3DXAssembleShader9( shaders[i].buffer, (int) strlen( shaders[i].buffer ), NULL, NULL, 0, &pXCode, &pXError );
		
		if( FAILED( hr ) )
		{
			WriteToLog(_T("Pixel Shader failed to assemble. (%08X)\n"), hr);
			RELEASE(pXCode);
			RELEASE(pXError);
			return false;
		}
		
		if( !CreatePixelShader( (DWORD*) pXCode->GetBufferPointer(), &pPixelShader ) )
		{
			WriteToLog( _T( "CreatePixelShader() failed with HResult = %s.\n" ), m_pD3D->HResultToString( GetLastError() ) );
			RELEASE( pXCode );
			RELEASE( pXError );
			return false;
		}
		
		PixelShaderInfo psi;
		psi.pPixelShader = pPixelShader;
		psi.strDesc = shaders[i].desc;
		
		m_PixelShaders.push_back( psi );
		
		RELEASE( pXCode );
		RELEASE( pXError );
	}
		
	return true;
}


//
// CAlphaPrecisionPS20
//
CAlphaPrecisionPS20::CAlphaPrecisionPS20()
{
	m_szTestName = "Alpha Precision - Pixel Shader V2.0";
	m_szCommandKey = "AlphaPS20";
}


bool CAlphaPrecisionPS20::CapsCheck()
{
 	DWORD dwVer = D3DSHADER_VERSION_MAJOR( m_pSrcDevice->GetCaps()->dwPixelShaderVersion );
 	
 	if( dwVer < 2 )
 		return false;
 	
 	return CAlphaPrecision::CapsCheck();

}

bool CAlphaPrecisionPS20::CreatePixelShaders()
{
	HRESULT hr;
    struct ShaderDesc
    {
		const TCHAR *buffer;
		const char *desc;
    };
    
    CnPixelShader *pPixelShader = NULL;
	
	const ShaderDesc shaders[] =
	{
		{
			"ps.2.0\n"
			"dcl v0\n"
			"mov oC0, v0\n"
			,
			"oC0 = v0"
		},
		{
			"ps.2.0\n"
			"dcl v0\n"
			"def c0, 2, 2, 2, 2\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = 2 * v0"
		},
		{
			"ps.2.0\n"
			"dcl v0\n"
			"def c0, .5, .5, .5, .5\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = .5f * v0"
		},
		{
			"ps.2.0\n"
			"dcl v0\n"
			"def c0, -2, -2, -2, -2\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = -2 * v0"
		},
		{
			"ps.2.0\n"
			"dcl v0\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0.rgb = RT_FLT_MAX * .5 * v0, oC0.a = v0.a"
		},
	};
	
	int nNumShaders = sizeof( shaders ) / sizeof( ShaderDesc );
	
	for( int i = 0; i < nNumShaders; ++i )
	{
		LPD3DXBUFFER pXCode = NULL;
		LPD3DXBUFFER pXError = NULL;
	    
		hr = D3DXAssembleShader9( shaders[i].buffer, (int) strlen( shaders[i].buffer ), NULL, NULL, 0, &pXCode, &pXError );
		
		if( FAILED( hr ) )
		{
			WriteToLog(_T("Pixel Shader failed to assemble. (%08X)\n"), hr);
			RELEASE(pXCode);
			RELEASE(pXError);
			return false;
		}
		
		if( !CreatePixelShader( (DWORD*) pXCode->GetBufferPointer(), &pPixelShader ) )
		{
			WriteToLog( _T( "CreatePixelShader() failed with HResult = %s.\n" ), m_pD3D->HResultToString( GetLastError() ) );
			RELEASE( pXCode );
			RELEASE( pXError );
			return false;
		}
		
		PixelShaderInfo psi;
		psi.pPixelShader = pPixelShader;
		psi.strDesc = shaders[i].desc;
		
		m_PixelShaders.push_back( psi );
		
		RELEASE( pXCode );
		RELEASE( pXError );
	}
		
	return true;
}



//
// CAlphaPrecisionPS30
//
CAlphaPrecisionPS30::CAlphaPrecisionPS30()
{
	m_szTestName = "Alpha Precision - Pixel Shader V3.0";
	m_szCommandKey = "AlphaPS30";
}


bool CAlphaPrecisionPS30::CapsCheck()
{
 	DWORD dwVer = D3DSHADER_VERSION_MAJOR( m_pSrcDevice->GetCaps()->dwPixelShaderVersion );
 	
 	if( dwVer < 3 )
 		return false;
 	
 	return CAlphaPrecision::CapsCheck();

}

bool CAlphaPrecisionPS30::CreatePixelShaders()
{
	HRESULT hr;
    struct ShaderDesc
    {
		const TCHAR *buffer;
		const char *desc;
    };
    
    CnPixelShader *pPixelShader = NULL;
	
	const ShaderDesc shaders[] =
	{
		{
			"ps.3.0\n"
			"dcl_color v0.rgba\n"
			"mov oC0, v0\n"
			,
			"oC0 = v0"
		},
		{
			"ps.3.0\n"
			"dcl_color v0.rgba\n"
			"def c0, 2, 2, 2, 2\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = 2 * v0"
		},
		{
			"ps.3.0\n"
			"dcl_color v0.rgba\n"
			"def c0, .5, .5, .5, .5\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = .5f * v0"
		},
		{
			"ps.3.0\n"
			"dcl_color v0.rgba\n"
			"def c0, -2, -2, -2, -2\n"
			"mul r0, v0, -c0\n"
			"mov oC0, r0\n"
			,
			"oC0 = -2 * v0"
		},
		{
			"ps.3.0\n"
			"dcl_color v0.rgba\n"
			"mul r0, v0, c0\n"
			"mov oC0, r0\n"
			,
			"oC0.rgb = RT_FLT_MAX * .5 * v0, oC0.a = v0.a"
		},
	};
	
	int nNumShaders = sizeof( shaders ) / sizeof( ShaderDesc );
	
	for( int i = 0; i < nNumShaders; ++i )
	{
		LPD3DXBUFFER pXCode = NULL;
		LPD3DXBUFFER pXError = NULL;
	    
		hr = D3DXAssembleShader9( shaders[i].buffer, (int) strlen( shaders[i].buffer ), NULL, NULL, 0, &pXCode, &pXError );
		
		if( FAILED( hr ) )
		{
			WriteToLog(_T("Pixel Shader failed to assemble. (%08X)\n"), hr);
			RELEASE(pXCode);
			RELEASE(pXError);
			return false;
		}
		
		if( !CreatePixelShader( (DWORD*) pXCode->GetBufferPointer(), &pPixelShader ) )
		{
			WriteToLog( _T( "CreatePixelShader() failed with HResult = %s.\n" ), m_pD3D->HResultToString( GetLastError() ) );
			RELEASE( pXCode );
			RELEASE( pXError );
			return false;
		}
		
		PixelShaderInfo psi;
		psi.pPixelShader = pPixelShader;
		psi.strDesc = shaders[i].desc;
		
		m_PixelShaders.push_back( psi );
		
		RELEASE( pXCode );
		RELEASE( pXError );
	}
		
	return true;
}