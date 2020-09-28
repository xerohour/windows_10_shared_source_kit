//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// clear_test.cpp
//
// CClearTest class - Tests many possible IDirect3DDeviceX::Clear() parameter combinations
//
// History: 8/19/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////


#define STRICT
#define D3D_OVERLOADS
#include <d3d9.h>
#include "clear.h"

// Global Test and App instance
CD3DWindowFramework	App;
CTextureClearTest TestTexClr;
COffscreenClearTest TestOffscrClr;

const DWORD SCISSOR_HEIGHT = 64;
const DWORD SCISSOR_WIDTH = 48;

const DWORD CClearTest::VertexFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

const DWORD CClearTest::TexVertexFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

const DWORD CClearTest::dwVertexCount = NUM_VERTICES;

const CClearTest::Vertex CClearTest::VertexList[]=
{
	{ 0.0f,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ 0.0f,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },


	{ 0.0f,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  0.0f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
	{ (float)IMGWIDTH,  (float)IMGHEIGHT, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0x00, 0xff, 0x00 ),
	 },
};

//This is the grid on which we apply the texture.
const CClearTest::TexVertex CClearTest::Grid[]=
{
	{ -0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 1.0
	 },
	{ -0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 1.0
	 },


	{ -0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 0.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  -0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 0.0
	 },
	{ (float)IMGWIDTH - 0.5f,  (float)IMGHEIGHT - 0.5f, 0.5f, 2.0f,
	 ARGB_MAKE( 0xff, 0xff, 0xff, 0xff ),
	 1.0, 1.0
	 },
};


CClearTest::CClearTest()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 7.0;
	m_Options.ModeOptions.dwTexInclude = PF_RGB;
	m_Options.ModeOptions.dwTexExclude = PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8; 

	// Reduce default back buffer size
	m_Options.D3DOptions.nBackBufferWidth = IMGWIDTH;
	m_Options.D3DOptions.nBackBufferHeight = IMGHEIGHT;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;
	
    // Init variables
	m_bOffscreen = false; 
	m_bAlphaPresent = false;
	m_bStencilPresent = false;
	m_bScissor = false;
}

CClearTest::~CClearTest()
{
}



UINT CClearTest::TestInitialize(void)
{
    if ((m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST) &&
        (m_pRefDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SCISSORTEST))
        m_bScissor = true;
    else
        m_bScissor = false;

    m_nTestCases = (UINT) (m_bScissor) ? 2*TEST_CASES : TEST_CASES;

#ifdef FAST_TEST
	m_uCommonZBufferFormats = 1;
	m_uCommonTextureFormats = 1;
#endif
    if (m_Options.D3DOptions.bZBuffer)
        SetTestRange(1, m_uCommonTextureFormats * m_uCommonZBufferFormats * m_nTestCases);
    else
        SetTestRange(1, m_uCommonTextureFormats * m_nTestCases);


	if ( (D3D_OK != (m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &m_pOriginalSrcRT))) )
	{
		WriteToLog("(ERROR): Src GetBackBufferCached failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
	}
	if ( (D3D_OK != (m_pRefDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &m_pOriginalRefRT))) )
	{
		WriteToLog("(ERROR): Ref GetBackBufferCached failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
	}


    m_pOriginalSrcZ = NULL;
    m_pOriginalRefZ = NULL;

    if (m_Options.D3DOptions.bZBuffer)
    {
		if ( (D3D_OK != (m_pSrcDevice->GetDepthStencilSurface(&m_pOriginalSrcZ))) )
		{
			WriteToLog("(ERROR): Src GetDepthStencilSurface failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}
		if ( (D3D_OK != (m_pRefDevice->GetDepthStencilSurface(&m_pOriginalRefZ))) )
		{
			WriteToLog("(ERROR): Ref GetDepthStencilSurface failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		}
    }

    m_nOriginalZFormat = m_pCurrentMode->nZBufferFormat;
    
	m_pSrcZ = NULL;
	m_pRefZ = NULL;
	m_pTextureRT = NULL;
	m_pOffSrcSurf = NULL;
	m_pOffRefSurf = NULL;

    m_pFramework->GetImageCmp()->SetPassRequired(0.95f);
    return D3DTESTINIT_RUN;
}




bool CClearTest::ClearFrame(void)
{
    bool bResult;

    m_pCurrentMode->nZBufferFormat = m_nOriginalZFormat;

    bResult =  Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,0,0), 1.0f, 0);

    if (m_Options.D3DOptions.bZBuffer)
        m_pCurrentMode->nZBufferFormat = m_nZFormat;

    return bResult;
}




bool CClearTest::ExecuteTest(UINT uiTest)
{
	D3DRECT rectList[2] = {{IMGWIDTH/4,IMGHEIGHT/4,IMGWIDTH/2,IMGHEIGHT/2},{IMGWIDTH/2,IMGHEIGHT/2,IMGWIDTH*3/4,IMGHEIGHT*3/4}};
    CSurface *pRendTgtSrc = NULL, *pRendTgtRef = NULL;
    TCHAR szTestCase[256];
    TCHAR szTestDescription[256];
	FORMAT *pFormat;
	RPOPTIONS ROpt;

	ROpt.Pool = POOL_DEFAULT;
	ROpt.Usage = 0;
	ROpt.uIBBitCount = 16;
	ROpt.Flags = 0;

    if (m_Options.D3DOptions.bZBuffer)
    {
        m_nTexFormat = (uiTest -1) / (m_uCommonZBufferFormats * m_nTestCases);
        m_nZFormat = ( (uiTest -1) % (m_uCommonZBufferFormats * m_nTestCases) ) / m_nTestCases;
		m_nTestCase = ( (uiTest -1) % (m_uCommonZBufferFormats * m_nTestCases) ) % m_nTestCases;
    }
    else
    {
        m_nTexFormat = (uiTest-1) / m_nTestCases;
        m_nZFormat = 0;
		m_nTestCase = (uiTest-1) % m_nTestCases;
    }

	DescribeFormat(szTestDescription,  m_nTexFormat, m_nZFormat);
	BeginTestCase(szTestDescription, uiTest);

	m_pCurrentMode->nTextureFormat = m_nTexFormat;
	m_pCurrentMode->nZBufferFormat = m_nZFormat;

	// Cleanup previous ZStencil
	RELEASE(m_pSrcZ);
	RELEASE(m_pRefZ);
	m_bStencilPresent = false;

	// Cleanup previous render-target
	SetTexture(0, NULL);
	RELEASE(m_pTextureRT);
	RELEASE(m_pOffSrcSurf);
	RELEASE(m_pOffRefSurf);
	m_bAlphaPresent = false;

	//create render target with the new format.
	pFormat = &(m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat]);
	if (m_bOffscreen)
	{
		if (!CreateOffScrRendTrgts(m_nTexFormat))
		{
			WriteToLog("(INFO) CreateOffScrRendTrgts failed.\n");

			// we're skipping. So clear any previous error codes stored by framework
			SkipTests(m_Options.D3DOptions.bZBuffer ? (m_uCommonZBufferFormats * m_nTestCases) : m_nTestCases);
			GetLastError();
			return false;
		}

		CreateTexture(IMGWIDTH,IMGHEIGHT,1,FMWK_DEFAULT,*pFormat,FMWK_DEFAULT,&m_pTextureRT);
	}
	else
	{
		CreateTexture(IMGWIDTH,IMGHEIGHT,1,USAGE_RENDERTARGET ,*pFormat,FMWK_DEFAULT,&m_pTextureRT);
	}

	if (NULL == m_pTextureRT)
	{
		WriteToLog("(INFO) CreateTexture failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

		// we're skipping. So clear any previous error codes stored by framework
		SkipTests(m_Options.D3DOptions.bZBuffer ? (m_uCommonZBufferFormats * m_nTestCases) : m_nTestCases);
		GetLastError();
		return false;
	}

	// update m_bAlphaPresent
	m_bAlphaPresent = ((m_pCommonTextureFormats[m_nTexFormat].ddpfPixelFormat.dwFlags & PF_ALPHAPIXELS) != 0);

	if (m_bOffscreen)
	{
		pRendTgtSrc = m_pOffSrcSurf;
		if (pRendTgtSrc) pRendTgtSrc->AddRef();
		pRendTgtRef = m_pOffRefSurf;
		if (pRendTgtRef) pRendTgtRef->AddRef();
	}
	else
	{
		CnSurface *pCnS=NULL;
		m_pTextureRT->GetSurfaceLevel( 0, &pCnS);
		if (pCnS)
		{
			pCnS->GetSurface(&pRendTgtSrc, 0);
			pCnS->GetSurface(&pRendTgtRef, 1);
			RELEASE(pCnS);
		}
		else
		{
			WriteToLog("(FATAL ERROR): GetSurfaceLevel from CnTexture failed!\n");
		}
	}

	//create ZStencil with the new format.
	if (m_Options.D3DOptions.bZBuffer)
	{
		if (!CreateZStencil(m_nZFormat))
		{
			WriteToLog("(INFO) CreateZStencil failed.\n");

			// we're skipping. So clear any previous error codes stored by framework
    		SkipTests(m_nTestCases);
			GetLastError();

			RELEASE(pRendTgtSrc);
			RELEASE(pRendTgtRef);
			return false;
		}

		// will RT and Z/Stencil go together?
		if (m_dwVersion > 0x0700)
		{
			// Check if render target and Z-buffer would go together.
			if (FAILED(m_pD3D->SetLastError(m_pD3D->CheckDepthStencilMatch(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetAdapter()->DesktopMode.Format, m_pCommonTextureFormats[m_nTexFormat], m_pCommonZBufferFormats[m_nZFormat]))))
			{
				TCHAR tbuf[256],zbuf[256];

				FmtToString(FORMAT_TEXTURE,&m_pCommonTextureFormats[m_nTexFormat],tbuf);
			    FmtToString(FORMAT_ZBUFFER,&m_pCommonZBufferFormats[m_nZFormat],zbuf);
				WriteToLog("(Info) Z format %s is not compatible with Render-Target format %s on this card.\n", zbuf, tbuf);

				// we're skipping. So clear any previous error codes stored by framework
    			SkipTests(m_nTestCases);
				GetLastError();

				RELEASE(pRendTgtSrc);
				RELEASE(pRendTgtRef);
				return false;
			}
		}

		// update m_bStencilPresent
		m_bStencilPresent = ((m_pCommonZBufferFormats[m_nZFormat].ddpfPixelFormat.dwFlags & PF_STENCILBUFFER) != 0);
	}
	else
	{
		m_pSrcZ = NULL;
		m_pRefZ = NULL;
	}


	// Set the texture/offscreen src surface as the src render target
	if (FAILED(m_pD3D->SetLastError(m_pSrcDevice->SetRenderTarget(pRendTgtSrc, m_pSrcZ))))
	{
		WriteToLog("(Info) Failed Src SetRenderTarget(). (%s)\n", m_pD3D->HResultToString(GetLastError()));
		SkipTests(m_nTestCases);
		// we're skipping. So clear any previous error codes stored by framework
		GetLastError();
		RELEASE(pRendTgtSrc);
		RELEASE(pRendTgtRef);
		return false;
	}

	// Set the texture/offscreen ref surface as the ref render target
	if (FAILED(m_pD3D->SetLastError(m_pRefDevice->SetRenderTarget(pRendTgtRef, m_pRefZ))))
	{
		WriteToLog("(Info) Failed Ref SetRenderTarget(). (%s)\n", m_pD3D->HResultToString(GetLastError()));
		SkipTests(m_nTestCases);
		// we're skipping. So clear any previous error codes stored by framework
		GetLastError();
		RELEASE(pRendTgtSrc);
		RELEASE(pRendTgtRef);
		return false;
	}

	RELEASE(pRendTgtSrc);
	RELEASE(pRendTgtRef);

	//SetupViewport();

	SetTexture(0, NULL);

    // Disable scissor rect if supported (make sure entire render target is cleared).
    if (m_bScissor)
        if (!DisableScissorTest())
            return false;

	// Clear texture with black.
	if (BeginScene())
	{
		Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_0), 1.0f, 255);
		EndScene();
	}

    // Enable/disable scissor rect if supported (scissor rect centered mid viewport).
    if (m_bScissor)
    {
        DWORD dwCenter[2];
        RECT Rect;

        dwCenter[0] = m_pCurrentMode->nBackBufferWidth >> 1;
        dwCenter[1] = m_pCurrentMode->nBackBufferHeight >> 1;

        Rect.top = dwCenter[0] - (SCISSOR_HEIGHT >> 1);
        Rect.left = dwCenter[1] - (SCISSOR_WIDTH >> 1);
        Rect.bottom = dwCenter[0] + (SCISSOR_HEIGHT >> 1);
        Rect.right = dwCenter[1] + (SCISSOR_WIDTH >> 1);

        if (m_nTestCase > TEST_CASES)
        {
            if (!EnableScissorTest(&Rect))
            return false;
        }
        else
        {
            if (!DisableScissorTest())
            return false;
        }
    }

	//Now we run the different clear() test cases and the results
	// go into the texture.
	switch(m_nTestCase)
	{

	// Clear Full Screen tests.
	//--------------------------
	case 14:
    case 0: // clear RGB full screen test.
		//Buffer must contain CLEAR_COLOR_FULL(red) after rendering grid. 
		if (BeginScene())
		{
			Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_FULL), 1.0f, 255);
			EndScene();
		}
		break;
	case 15:
	case 1: // clear Alpha full screen test
		//Buffer must contain CLEAR_COLOR_FULL_ALPHA1(red) after rendering grid.
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_FULL_ALPHA1), 1.0f, 255);
				EndScene();
			}
		}
		break;
	case 16:
	case 2: // clear Alpha full screen test
		//Buffer must contain black(0 color) after rendering grid.
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_FULL_ALPHA0), 1.0f, 255);
				EndScene();
			}
		}
		break;
	case 17:
	case 3: // clear Z full screen test
		// Triangles must NOT be rendered. So buffer must contain CLEAR_COLOR_FULL(red) after rendering grid.
		if (m_Options.D3DOptions.bZBuffer)
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), LOWER_Z, 255);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 18:
	case 4: // clear Z full screen test
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if (m_Options.D3DOptions.bZBuffer)
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), HIGHER_Z, 255);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 19:
	case 5: // clear Stencil full screen test.
		// Triangles must NOT be rendered. So buffer must contain CLEAR_COLOR_FULL(red) after rendering grid.
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_0);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 20:
	case 6: // clear Stencil full screen test.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_1);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 21:
	case 7: // test to ensure ClearZ and ClearStencil does not affect each other.
		// Testing if clear Z affect stencil.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 0.0f, STENCIL_1);
				Clear(0, NULL, CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), 0.0f, STENCIL_0);
				Clear(0, NULL, CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), 1.0f, STENCIL_0);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 22:
	case 8: // test to ensure ClearZ and ClearStencil does not affect each other.
		// Testing if clear stencil affect Z.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_0);
				Clear(0, NULL, CLEARF_STENCIL, (CLEAR_COLOR_FULL), 0.0f, STENCIL_0);
				Clear(0, NULL, CLEARF_STENCIL, (CLEAR_COLOR_FULL), 0.0f, STENCIL_1);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 23:
	case 9: //test to ensure Clear render-target alone does not affect Z or stencil.
		// Triangles must be rendered. So buffer must contain RENDERING_COLOR(green) after rendering grid.
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_1);
				Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_0), 0.0f, STENCIL_0);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;


	// Clear List of Rects.
	//--------------------------

	case 24:
	case 10: // clear RGB - list of rects.
		//rects will be CLEAR_COLOR_FULL_2(blue), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if (BeginScene())
		{

			Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_FULL), 1.0f, 255);
			Clear(2, rectList, CLEARF_TARGET, (CLEAR_COLOR_FULL_2), 1.0f, 255);
			EndScene();
		}
		break;
	case 25:
	case 11: // clear alpha test - list of rects.
		//rects will be CLEAR_COLOR_FULL_ALPHA1(red), rest black after rendering grid. 
		if (m_bAlphaPresent)
		{
			if (BeginScene())
			{

				Clear(0, NULL, CLEARF_TARGET, (CLEAR_COLOR_FULL_ALPHA0), 1.0f, 255);
				Clear(2, rectList, CLEARF_TARGET, (CLEAR_COLOR_FULL_ALPHA1), 1.0f, 255);
				EndScene();
			}
		}
		break;
	case 26:
	case 12: // clear Z test - list of rects.
		//rects will be RENDERING_COLOR(green), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if (m_Options.D3DOptions.bZBuffer)
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), LOWER_Z, 255);
				Clear(2, rectList, CLEARF_ZBUFFER, (CLEAR_COLOR_FULL), HIGHER_Z, 255);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)FALSE );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	case 27:
	case 13: // clear Stencil test - list of rects.
		//rects will be RENDERING_COLOR(green), rest CLEAR_COLOR_FULL(red) after rendering grid. 
		if ( (m_Options.D3DOptions.bZBuffer) && (m_bStencilPresent) )
		{
			if (BeginScene())
			{
				Clear(0, NULL, CLEARF_TARGET|CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_0);
				Clear(2, rectList, CLEARF_STENCIL, (CLEAR_COLOR_FULL), 1.0f, STENCIL_1);

				SetRenderState( D3DRS_CULLMODE, (DWORD)D3DCULL_NONE );
				SetRenderState( D3DRS_CLIPPING, (DWORD)FALSE );
				SetRenderState( D3DRS_LIGHTING, (DWORD)FALSE );
				SetRenderState( D3DRS_SHADEMODE, (DWORD)D3DSHADE_FLAT );
				SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );

				SetRenderState( D3DRS_ZENABLE, (DWORD)FALSE );
				SetRenderState( D3DRS_STENCILENABLE, (DWORD)TRUE );
				SetRenderState( D3DRS_STENCILREF, (DWORD)STENCIL_1 );
				SetRenderState( D3DRS_STENCILFUNC, (DWORD)D3DCMP_EQUAL );

				// Render the triangles.
				if (!RenderPrimitive(D3DPT_TRIANGLELIST, VertexFVF, (void *)VertexList, dwVertexCount, NULL, 0, &ROpt))
					WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

				if (!EndScene())
					WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
			}
		}
		break;
	default:
		break;
	}

	//if m_bOffscreen, copy offscreen rendered image to texture.
    if (m_bOffscreen)
    {
        if (!CopyToTexture())
            WriteToLog("(ERROR): CopyToTexture failed.\n");
    }

    // Reset to the original render target
    if (FAILED(m_pD3D->SetLastError(m_pSrcDevice->SetRenderTarget(m_pOriginalSrcRT, m_pOriginalSrcZ))))
	{
        WriteToLog("(ERROR): Couldn't reset Src RenderTarget. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		SkipTests(m_uCommonTextureFormats * m_uCommonZBufferFormats * m_nTestCases);
		// we're skipping. So clear any previous error codes stored by framework
		GetLastError();
		return false;
	}

    if (FAILED(m_pD3D->SetLastError(m_pRefDevice->SetRenderTarget(m_pOriginalRefRT, m_pOriginalRefZ))))
	{
        WriteToLog("(ERROR): Couldn't reset Ref RenderTarget. (%s)\n", m_pD3D->HResultToString(GetLastError()));
		SkipTests(m_uCommonTextureFormats * m_uCommonZBufferFormats * m_nTestCases);
		// we're skipping. So clear any previous error codes stored by framework
		GetLastError();
		return false;
	}

    //SetupViewport();

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);


	switch(m_nTestCase)
	{
    case 15:
    case 16:
    case 25:
	case 1:
	case 2:
	case 11:
		// for test cases testing alpha clear.
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
		SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
		break;
	default:
		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
		break;
	}


    SetTexture(0, m_pTextureRT);


	// Dump test case information to status window and log.
	DescribeTestcase(szTestCase, m_nTestCase);
    WriteStatus("$gDescription", szTestCase);
	WriteToLog("%s\n", szTestCase);


	return true;
}



void CClearTest::SceneRefresh(void)
{
	RPOPTIONS	ROpt;

	ROpt.Pool = POOL_DEFAULT;
	ROpt.Usage = 0;
	ROpt.uIBBitCount = 16;
	ROpt.Flags = 0;

	if (BeginScene())
	{
		SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);
		SetRenderState(D3DRS_STENCILENABLE, (DWORD)FALSE );

        if (!RenderPrimitive(D3DPT_TRIANGLELIST, TexVertexFVF, (void *)Grid, dwVertexCount, NULL, 0, &ROpt))
            WriteToLog("(ERROR): RenderPrimitive failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));

        if (!EndScene())
            WriteToLog("(ERROR): EndScene failed. (%s)\n", m_pD3D->HResultToString(GetLastError()));
	}


	// Compare the entire image without leaving out the clear color
	m_pFramework->GetImageCmp()->SetIncClearColor(true);


	return;
}






bool CClearTest::TestTerminate(void)
{

	// Cleanup previous ZStencil
	RELEASE(m_pSrcZ);
	RELEASE(m_pRefZ);

	// Cleanup previous render-target
	RELEASE(m_pTextureRT);
	RELEASE(m_pOffSrcSurf);
	RELEASE(m_pOffRefSurf);

    RELEASE(m_pOriginalSrcRT);
    RELEASE(m_pOriginalRefRT);

    if (m_Options.D3DOptions.bZBuffer)
    {
        RELEASE(m_pOriginalSrcZ);
        RELEASE(m_pOriginalRefZ);
    }


	return true;
}

// NAME:        DisableScissorTest()
// DESC:        disable scissor test
// INPUT:       none
// OUTPUT:      true.....if scissor test disabled successfully
//              false....otherwise

bool CClearTest::DisableScissorTest(void)
{
    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) FALSE);
}

// NAME:        EnableScissorTest()
// DESC:        enable scissor test
// INPUT:       pRect....address of RECT scissor rect
// OUTPUT:      true.....if scissor test enabled successfully
//              false....otherwise

bool CClearTest::EnableScissorTest(RECT *pRect)
{
    RECT Src, Ref;

    if (!pRect)
    {
        WriteToLog("ERROR: EnableScissorTest(): NULL scissor rect pointer.\n");
        return false;
    }

    // can call GetScissorRect() and SetScissorRect() from CD3DTest or,
    // call them from the src and ref devices directly (as below)

    if (!((m_pSrcDevice->GetBehaviors() & DEVICECREATE_PURE) ||
          (m_pRefDevice->GetBehaviors() & DEVICECREATE_PURE)))
    {
        if (D3D_OK != m_pSrcDevice->GetScissorRect(&Src))
        {
            WriteToLog("ERROR: GetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if (D3D_OK != m_pRefDevice->GetScissorRect(&Ref))
        {
            WriteToLog("ERROR: GetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
            return false;
        }

        if ((Src.top != Ref.top) || (Src.left != Ref.left) ||
            (Src.bottom != Ref.bottom) || (Src.right != Ref.right))
        {
            WriteToLog("ERROR: Src and Ref scissor rects not identical.\n");
            WriteToLog("Src rect(%d,%d,%d,%d)\n", Src.top, Src.left, Src.bottom, Src.right);
            WriteToLog("Ref rect(%d,%d,%d,%d)\n", Ref.top, Ref.left, Ref.bottom, Ref.right);
            return false;
        }
    }

    if (D3D_OK != m_pSrcDevice->SetScissorRect(pRect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on src device: %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (D3D_OK != m_pRefDevice->SetScissorRect(pRect))
    {
        WriteToLog("ERROR: SetScissorRect() failed on ref device: %s.\n", GLE(GetLastError()));
        return false;
    }

    return SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD) TRUE);
}
