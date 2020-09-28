#include "RenderTargetTest.h"
#include "D3DX9.h"

CNonPowTwoRTTest::CNonPowTwoRTTest()
{
    SetTestType(TESTTYPE_CONF);

    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91;

    // Force test to cycle desktop resolutions 16bpp/32bpp
    m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;
	m_Options.ModeOptions.TextureFmt    = FMT_ALL;
    m_Options.ModeOptions.ZBufferFmt    = FMT_ALL;

    m_Options.D3DOptions.nBackBufferWidth  = 160;
    m_Options.D3DOptions.nBackBufferHeight = 120;

    m_Options.ModeOptions.dwTexExclude = PF_BUMPLUMINANCE | PF_BUMPDUDV | PF_METN;// | PF_DMAP;

    m_szTestName   = _T("Non-Pow2 Render Targets");
    m_szCommandKey = _T("RenderTarget");

    m_pBackBuffer = NULL;
    m_pZBuffer    = NULL;

	m_bTexPow2     = false;
	m_bNonPow2Cond = false;
}

//------------------------------------------------------------------------------

CNonPowTwoRTTest::~CNonPowTwoRTTest()
{
}

//------------------------------------------------------------------------------

void CNonPowTwoRTTest::Cleanup()
{
    RELEASE(m_pBackBuffer);
    RELEASE(m_pZBuffer);
}

//------------------------------------------------------------------------------

bool CNonPowTwoRTTest::CapsCheck()
{
    const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

	if(NULL == pCaps)
	{
		return false;

	}

	m_bNonPow2Cond = (D3DPTEXTURECAPS_NONPOW2CONDITIONAL == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL));
	
	m_bTexPow2 = (D3DPTEXTURECAPS_POW2 == (pCaps->dwTextureCaps & D3DPTEXTURECAPS_POW2));

    
	//return true for now but abort in Setup because it's a wrong combination of caps
	if (!m_bTexPow2 && m_bNonPow2Cond)
	WriteToLog("Wrong combination of Pow2 caps.\n");
        
    //if command line -pow2 is set then we support all sizes so reset all the caps
	if (KeySet("pow2"))
	{
		m_bTexPow2 = false;
		m_bNonPow2Cond = false;
	}
	
	if(m_bTexPow2 && !m_bNonPow2Cond)
    {
		WriteToLog("NonPowTwo textures ate not supported.\n");
        return false;
    }

	//see if this texture format is a supported as render target for this mode on m_pSrcDevice
    if ( FAILED(m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
										   m_pSrcDevice->GetDevType(),
										   m_pSrcDevice->GetAdapter()->DesktopMode.Format,
										   USAGE_RENDERTARGET,
										   RTYPE_TEXTURE,
                                           m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat])))
    {
		WriteToLog("This format is not supprted as a render target on m_pSrcDevice.\n");
		return false;
	}

	//see if this texture format is a supported as render target for this mode on m_pRefDevice
    if (NULL != m_pRefDevice && FAILED( m_pD3D->CheckDeviceFormat(m_pRefDevice->GetAdapterID(),
										m_pRefDevice->GetDevType(),
										m_pRefDevice->GetAdapter()->DesktopMode.Format,
										USAGE_RENDERTARGET,
										RTYPE_TEXTURE,
                                        m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat])))
	{
		WriteToLog("This format is not supprted as a render target on m_pRefDevice.\n");
		return false;
	}

	//check if this TR/Texture and ZBuffer formats match 
	if ( FAILED(m_pD3D->CheckDepthStencilMatch( m_pSrcDevice->GetAdapterID(), 
												m_pSrcDevice->GetDevType(), 
												m_pSrcDevice->GetAdapter()->DesktopMode.Format,
												m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], 
												m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat])))
	{
		WriteToLog("CheckDepthStencilMatch() failed on current ZBuffer and RT/Tex formats.\n");
		return false;
	}
    
    return true;
}

//------------------------------------------------------------------------------

bool CNonPowTwoRTTest::Setup(VOID)
{
    int nStage;
    
	//if illigal combination of flags is set (!pow2 && nonpowtwo) then abort
	if (!m_bTexPow2 && m_bNonPow2Cond)
	{
		WriteToLog("Illigal combination of flags is set (!D3DPTEXTURECAPS_POW2 && D3DPTEXTURECAPS_NONPOW2CONDITIONAL)\n");
		return false;
	}

    if ( m_bNonPow2Cond )
    for (nStage = 0; nStage < 8; nStage++)
    {
        SetSamplerState(nStage, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        SetSamplerState(nStage, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }

	if(!GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &m_pBackBuffer))
    {
        WriteToLog(_T("CNonPowTwoRTTest::Setup - GetBackBuffer failed.\n"));
        return false;
    }

    if(!GetDepthStencilSurface(&m_pZBuffer))
    {
        WriteToLog(_T("CNonPowTwoRTTest::Setup - GetDepthStencilSurface failed.\n"));
        return false;
    }

    SetRenderState(D3DRS_LIGHTING, FALSE);
 
	UINT max_width  = m_pSrcDevice->GetCaps()->dwMaxTextureWidth;
	UINT max_height = m_pSrcDevice->GetCaps()->dwMaxTextureHeight;

	if ( m_pRefDevice )
	{
		max_width  = min( m_pRefDevice->GetCaps()->dwMaxTextureWidth,  max_width  );
		max_height = min( m_pRefDevice->GetCaps()->dwMaxTextureHeight, max_height );
	}

	// The A1 texture format has a height restriction of 2048
	if ( FMT_A1 == m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat )
	{
		max_height = min( 2048, max_height );;
	}

	//fill out m_uSizes arrya
	//first fill out it with standard screen resolution sizes then choose random values
	m_uSizes[ 0][0] =  800; m_uSizes[ 0][1] =  600;
	m_uSizes[ 1][0] = 1024; m_uSizes[ 1][1] =  768;
	m_uSizes[ 2][0] = 1152; m_uSizes[ 2][1] =  864;
	m_uSizes[ 3][0] = 1280; m_uSizes[ 3][1] =  720;
	m_uSizes[ 4][0] = 1280; m_uSizes[ 4][1] =  768;
	m_uSizes[ 5][0] = 1280; m_uSizes[ 5][1] =  960;
	m_uSizes[ 6][0] = 1280; m_uSizes[ 6][1] = 1024;
	m_uSizes[ 7][0] = 1360; m_uSizes[ 7][1] =  768;
	m_uSizes[ 8][0] = 1600; m_uSizes[ 8][1] =  900;
	m_uSizes[ 9][0] = 1600; m_uSizes[ 9][1] = 1024;
	m_uSizes[10][0] = 1600; m_uSizes[10][1] = 1200;
	m_uSizes[11][0] = 1920; m_uSizes[11][1] = 1080;
	m_uSizes[12][0] = 1920; m_uSizes[12][1] = 1200;
	m_uSizes[13][0] = 1920; m_uSizes[13][1] = 1440;
	m_uSizes[14][0] = 2048; m_uSizes[14][1] = 1536;

	for(UINT i = 0; i < 15; i++)
	{
		m_uSizes[i][0] = min(m_uSizes[i][0], max_width );
		m_uSizes[i][1] = min(m_uSizes[i][1], max_height);
	}

	srand(m_pCurrentMode->nTextureFormat + m_pCurrentMode->nBackBufferFormat + m_pCurrentMode->nZBufferFormat);
	
	for(UINT i = 15; i < TEST_ARRAY_SIZE; i++)
	{
		m_uSizes[i][0] = (rand() % max_width ) + 1;
		m_uSizes[i][1] = (rand() % max_height) + 1;

	}

    return true;
}

//------------------------------------------------------------------------------

UINT CNonPowTwoRTTest::TestInitialize()
{
    if(!SetTestRange(1, TEST_ARRAY_SIZE))
    {
        WriteToLog(_T("SetTestRange failed\n"));
        return D3DTESTINIT_ABORT;
    }
    
	return D3DTESTINIT_RUN;
}

//------------------------------------------------------------------------------

bool CNonPowTwoRTTest::ExecuteTest(UINT nTestNum)
{
	UINT uWidth  = m_uSizes[nTestNum-1][0];
	UINT uHeight = m_uSizes[nTestNum-1][1];

	if(KeySet(_T("POW2")))
	{
		uWidth  = ForcePOW2(uWidth );
		uHeight = ForcePOW2(uHeight);
	}

	// The A1 texture format must have a width in multiples of 32
	if(FMT_A1 == m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat)
	{
		uWidth = ForceDWORDMultiple(uWidth);
	}

	int rtd;
	if (ReadInteger(_T("rtw"), &rtd))
		uWidth = rtd;
	if (ReadInteger(_T("rth"), &rtd))
		uHeight = rtd;

    sprintf(m_testDesc, "NonPowTwo Render Target: width = %d, height = %d", uWidth, uHeight);

    BeginTestCase(m_testDesc, nTestNum);

    CnTexture* pRTTexture  = NULL;
    CnSurface* pRTSurface  = NULL;
    CnSurface* pNewZBuffer = NULL;

    bool bResult = true;

    bResult &= CreateRTTexture(uWidth, uHeight, &pRTTexture, &pNewZBuffer);

    if(NULL != pRTTexture)
    {
        bResult &= pRTTexture->GetSurfaceLevel(0, &pRTSurface);
    }

    if((NULL != pRTSurface) && (NULL != pNewZBuffer))
    {
        bResult &= SetRenderTarget(pRTSurface, pNewZBuffer);

        bResult &= Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), 0x000000AF, 1.0f, 0);

        bResult &= SetTexture(0, NULL);

        bResult &= RenderTriangle();
    }

    bResult &= SetRenderTarget(m_pBackBuffer, m_pZBuffer);

    bResult &= SetTexture(0, pRTTexture);

    bResult &= RenderTriangle();

    RELEASE(pNewZBuffer);
    RELEASE(pRTSurface);
    RELEASE(pRTTexture);

    if(false == bResult)
    {
        Fail();
    }

    return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoRTTest::RenderTriangle()
{
    bool bResult = true;

    SVertex rgVertices[3];

    rgVertices[0].rgfPosition [0] = 1.0f;
    rgVertices[0].rgfPosition [1] = 1.0f;
    rgVertices[0].rgfPosition [2] = 0.5f;
    rgVertices[0].rgfTexCoords[0] = 0.0f;
    rgVertices[0].rgfTexCoords[1] = 0.0f;
    rgVertices[0].dwColor = 0xFFFFFFFF;

    rgVertices[1].rgfPosition [0] =  0.0f;
    rgVertices[1].rgfPosition [1] = -1.0f;
    rgVertices[1].rgfPosition [2] =  0.5f;
    rgVertices[1].rgfTexCoords[0] =  1.0f;
    rgVertices[1].rgfTexCoords[1] =  0.0f;
    rgVertices[1].dwColor = 0xFF00FF00;

    rgVertices[2].rgfPosition [0] = -1.0f;
    rgVertices[2].rgfPosition [1] =  1.0f;
    rgVertices[2].rgfPosition [2] =  0.5f;
    rgVertices[2].rgfTexCoords[0] =  0.0f;
    rgVertices[2].rgfTexCoords[1] =  1.0f;
    rgVertices[2].dwColor = 0xFFFF0000;

    RPOPTIONS Options;
	Options.Pool = POOL_MANAGED;
	Options.uIBBitCount = 16;
	Options.Usage = 0;
	Options.Flags = 0;

	if(BeginScene())
    {
        bResult = bResult && RenderPrimitive(D3DPT_TRIANGLELIST, VERTEX_FVF, rgVertices, 3, NULL, 0, &Options);
        bResult = bResult && EndScene();
    }
    else
    {
        bResult = false;
    }

    return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoRTTest::CreateRTTexture(UINT uWidth, UINT uHeight, CnTexture** ppTexOut, CnSurface** ppZBufferOut)
{
    if(NULL == ppTexOut || NULL == ppZBufferOut)
    {
        return false;
    }

    CnTexture* pTexture = NULL;
    CnSurface* pZBuffer = NULL;

    bool bResult = true;
	HRESULT hr;

    //loop until successfully create texture and depth stencil if run out of vide memory with original sizes
	while (uWidth >= 1 && uHeight >= 1)
	{
		if ( !CreateTexture( uWidth, uHeight, 1, USAGE_RENDERTARGET, 
							 m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat],
							 POOL_DEFAULT, &pTexture) )
		{
			hr = GetLastError();

			if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				uWidth  = uWidth  >> 1;
				uHeight = uHeight >> 1;
				WriteToLog("Out of memory on creating texture. Decreased sizes to w=%d, h=%d.\n", uWidth, uHeight);
				continue;
			}

			SetLastError(hr);
			bResult = false;
			break; //if fail for any other errors then break out of the loop
		}

		if ( !CreateDepthStencilSurface( uWidth, uHeight,
										 m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat],
										 m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample],
										 &pZBuffer) )
		{
			hr = GetLastError();
			RELEASE(pTexture);

			if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				uWidth  = uWidth  >> 1;
				uHeight = uHeight >> 1;
				WriteToLog("Out of memory on creating depth stencil. Decreased sizes to w=%d, h=%d.\n", uWidth, uHeight);
				continue;
			}

			SetLastError(hr);
			bResult = false;
			break; //if fail for any other errors then break out of the loop
		}
		else
			break; //if both texture and render target got created then break out of the loop
	}

    if(bResult)
    {
        RELEASE(*ppTexOut);
		RELEASE(*ppZBufferOut);
        
		*ppTexOut     = pTexture;
		*ppZBufferOut = pZBuffer;
    }
    else
    {
        RELEASE(pTexture);
        RELEASE(pZBuffer);
    }

    return bResult;
}

//------------------------------------------------------------------------------

UINT CNonPowTwoRTTest::ForcePOW2(UINT nDimension)
{
	//find the highest bit set in nDimension

	UINT nHighestBitSet = 0;

	for(UINT nCtr = 0; nCtr < 32; nCtr++)
	{
		if((1<<nCtr) == ((1<<nCtr) & (nDimension)))
		{
			nHighestBitSet = nCtr;
		}
	}

	return (1<<nHighestBitSet);
}

//------------------------------------------------------------------------------

UINT CNonPowTwoRTTest::ForceDWORDMultiple(UINT nDimension)
{
    if(0 != (nDimension % 32))
    {
	    nDimension += (32 - (nDimension % 32));
    }

	return nDimension;
}

//------------------------------------------------------------------------------

void CNonPowTwoRTTest::CommandLineHelp()
{
	WriteCommandLineHelp(_T("$g-POW2                  $yForce power of 2 dimensions"));

}
