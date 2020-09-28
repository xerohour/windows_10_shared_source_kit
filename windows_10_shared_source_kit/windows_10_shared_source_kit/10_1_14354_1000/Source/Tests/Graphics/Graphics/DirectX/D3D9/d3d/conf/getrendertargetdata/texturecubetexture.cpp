#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include "Utils.h"

CTextureCubeTexture::CTextureCubeTexture() :
    m_pTexture( 0 ),
    m_pVertexBuffer( 0 ),
    m_pMipTexture( 0 ),
    m_pMipSurface( 0 ),
    m_pCubeTexture( 0 ),
    m_pSrcRenderTarget( 0 )
{
    m_szTestName = _T("TEXCUBTEX");
    m_szCommandKey = _T("grtdtct");

    addParamNode( "Texture", CTextureParams() );
    addParamNode( "CubeTexture", CCubeTextureParams() );
}

bool CTextureCubeTexture::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CTextureCubeTexture::CapsCheck()
{
    CAPS caps;
    m_pSrcDevice->GetDeviceCaps( &caps );

    if ( ( caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP ) != 0 )
    {
	    static UINT levels[] =
	    {
		    0
	    };

	    unionDomain( "Texture/Levels", levels, levels + ( sizeof( levels ) / sizeof( levels[0] ) ) );
    }

    return ( caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) != 0  &&
           ( caps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES ) != 0;
}

bool CTextureCubeTexture::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CTextureCubeTexture::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CTextureCubeTexture::Setup()
{
    CHRESULT hr;

    try
    {
        struct Vertex
        {
            Vertex( float x, float y, float z, float u, float v ) : x(x), y(y), z(z), nx(0.0f), ny(0.0f), nz(-1.0f), u(u), v(v) {}
            float x;
            float y;
            float z;
            float nx;
            float ny;
            float nz;
            float u;
            float v;
        } *pVertex;

        m_nVertices = 4;

        const UINT fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

        hr = m_pSrcDevice->CreateVertexBuffer(m_nVertices * sizeof(Vertex), USAGE_WRITEONLY, fvf, POOL_DEFAULT, &m_pVertexBuffer);

        hr = m_pVertexBuffer->Lock(0, 0, (VOID**) &pVertex, 0);

        *pVertex++ = Vertex( -1.0f, 1.0f, 0.0f, 0.0f, 0.0f );
        *pVertex++ = Vertex( 1.0f, 1.0f, 0.0f, 1.0f, 0.0f );
        *pVertex++ = Vertex( 1.0f, -1.0f, 0.0f, 1.0f, 1.0f );
        *pVertex++ = Vertex( -1.0f, -1.0f, 0.0f, 0.0f, 1.0f );

        hr = m_pVertexBuffer->Unlock();

        hr = m_pSrcDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(Vertex));

        hr = m_pSrcDevice->SetFVF(fvf);

        hr = m_pSrcDevice->D3DXCreateTextureFromResourceA(NULL, MAKEINTRESOURCE(IDB_BITMAP1), &m_pTexture);

        hr = m_pSrcDevice->SetTexture(0, m_pTexture);
    }
    catch( CHRESULT hr )
    {
        return hr.setTestResult(this);
    }

    return CGetRenderTargetData::Setup();
}

UINT CTextureCubeTexture::TestInitialize()
{
#ifdef FAST_TEST
    static Dimension dims[] =
    {
        Dimension(32, 32),
    };

    FMT formats[] =
    {
        FMT_A8R8G8B8,
        FMT_DXT1,
        FMT_L8
    };

    CUBEMAP_FACES faces[] =
    {
        CUBEMAP_FACE_POSITIVE_X,
        CUBEMAP_FACE_NEGATIVE_Z
    };

    setDomain("Texture/Dimension", dims, dims + ARRAYSIZE(dims));

    setDomain("Texture/Format", formats, formats + ARRAYSIZE(formats));

    static DWORD usages[] =
    {
        0,
        USAGE_DYNAMIC,
        USAGE_RENDERTARGET,
        USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
    };

    CAPS caps;
    m_pSrcDevice->GetDeviceCaps(&caps);
    unsigned end = 0 != (caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) ? 4 : 3;

    setDomain("Texture/Usage", usages + 2, usages + end);

    setDomain("CubeTexture/Usage", usages, usages + 2);
    setDomain("CubeTexture/Pool", Eq<DWORD>(POOL_SYSTEMMEM));

    setDomain("CubeTexture/Face", faces, faces + ARRAYSIZE(faces));
#else
    static Dimension dims[] = 
    {
        Dimension( 1, 1 ),
        Dimension( 2, 2 ),
        Dimension( 4, 4 ),
        Dimension( 8, 8 ),
        Dimension( 16, 16 ),
        Dimension( 32, 32 ),
        Dimension( 64, 64 ),
        Dimension( 128, 128 ),
        Dimension( 256, 256 ),
        Dimension( 512, 512 ),
        Dimension( 1024, 1024 )
    };

	setDomain( "Texture/Dimension", dims, dims + ( sizeof( dims ) / sizeof( dims[0] ) ) );

    setDomain( "Texture/Format", BackBufferFmts() );

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("TEXCUBTEXAPI");

        setDomain( "Texture/Usage", All() );
        setDomain( "Texture/Pool", All() );

        setDomain( "CubeTexture/Format", BackBufferFmts() );
        setDomain( "CubeTexture/Usage", All() );
        setDomain( "CubeTexture/Pool", All() );
    }
    else
    {
        static DWORD usages[] =
        {
            0,
			USAGE_DYNAMIC,
            USAGE_RENDERTARGET,
            USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
        };

        CAPS caps;
        m_pSrcDevice->GetDeviceCaps( &caps );
        unsigned end = 0 != ( caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP ) ? 4 : 3;

        setDomain( "Texture/Usage", usages + 2, usages + end );

	setDomain( "CubeTexture/Usage", usages, usages + 2 );
        setDomain( "CubeTexture/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
    }

    setDomain( "CubeTexture/Face", All() );
#endif
    unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CTextureCubeTexture::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CTextureCubeTexture::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
        Dimension dim1, dim2;
        FMT format1, format2;
        UINT levels;
        DWORD usage1, usage2;
        DWORD pool1, pool2;
        UINT edgeLength;

        setState( nTest - 1 );

        getParameter( "Texture/Dimension", dim1 );
        getParameter( "Texture/Levels", levels );
        getParameter( "Texture/Usage", usage1 );
        getParameter( "Texture/Format", format1 );
        getParameter( "Texture/Pool", pool1 );

        if ( !m_bRunInvalid )
        {
            setParameter( "CubeTexture/EdgeLength", dim1.Width );
            setParameter( "CubeTexture/Format", format1 );
            setParameter( "CubeTexture/Levels", levels );
        }

        getParameter( "CubeTexture/EdgeLength", edgeLength );
        getParameter( "CubeTexture/Usage", usage2 );
        getParameter( "CubeTexture/Format", format2 );
        getParameter( "CubeTexture/Pool", pool2 );

        std::string str1, str2;
		getString( "Texture", str1 );
        getString( "CubeTexture", str2 );
        str1 += "/" + str2;
        BeginTestCase( str1.c_str(), nTest );

        hr = m_pSrcDevice->GetRenderTarget( &m_pSrcRenderTarget );

		if ( FAILED( m_pSrcDevice->CreateTexture( levels ? dim1.Width : m_Dimension, 
										          levels ? dim1.Height : m_Dimension, 
										          levels, 
										          usage1, 
										          format1, 
										          pool1, 
										          &m_pMipTexture ) ) ||
             FAILED( m_pSrcDevice->CreateCubeTexture( levels ? edgeLength : m_Dimension,  
											          levels, 
											          usage2, 
											          format2, 
											          pool2, 
											          &m_pCubeTexture ) ) )
        {
            Skip();
            GetLastError();
            return false;
        }

		DWORD nLevels = m_pMipTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) dim1.Width ) / log( 2.0 ) +.5);

        hr = m_pMipTexture->GetSurfaceLevel( level, &m_pMipSurface );

        hr = m_pSrcDevice->SetRenderTarget( m_pMipSurface );

        VIEWPORT viewport = 
        {
            0,
            0,
            dim1.Width,
            dim1.Height,
            0.0f,
            1.0f
        };

        hr = m_pSrcDevice->SetViewport( &viewport );
    }
    catch( CHRESULT hr )
    {
        return hr.setTestResult(this);
    }

    return CGetRenderTargetData::ExecuteTest(nTest);
}

void CTextureCubeTexture::SceneRefresh()
{
    CHRESULT hr;

    try
    {
        D3DCOLOR Color = 0x00000032;

	    m_pFramework->GetImageCmp()->SetClearColor( Color );

		hr = m_pSrcDevice->Clear(0, NULL, FMWK_DEFAULT, Color, 1.0f, 0);

        hr = m_pSrcDevice->BeginScene();

        hr = m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, m_nVertices - 2);

        hr = m_pSrcDevice->EndScene();

        hr = m_pSrcDevice->StretchRect( m_pMipSurface, NULL, m_pSrcRenderTarget, NULL, D3DTEXF_NONE );
    }
    catch( CHRESULT hr )
    {
        GetLastError();
        return;
    }

    CGetRenderTargetData::SceneRefresh();
}

bool CTextureCubeTexture::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        CAutoPtr<CSurface> pBackBuffer;
        hr = m_pSrcDevice->GetRenderTarget( &pBackBuffer );

        SURFACEDESC descBB;
        hr = pBackBuffer->GetDesc(&descBB);

		DWORD nLevels = m_pMipTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) descBB.Width ) / log( 2.0 ) +.5);

		CUBEMAP_FACES face;
		getParameter( "CubeTexture/Face", face );

        CAutoPtr<CSurface> pSurfaceTX;
        hr = m_pCubeTexture->GetCubeMapSurface(face, level, &pSurfaceTX);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(pBackBuffer, pSurfaceTX) ) )
        {
            SURFACEDESC descTX;
            hr = m_pCubeTexture->GetLevelDesc(level, &descTX);

            hr = descTX.Pool != POOL_SYSTEMMEM || 
                 descTX.Format.d3dfFormat != descBB.Format.d3dfFormat ||
                 descTX.Width != descBB.Width ||
                 descTX.Height != descBB.Height ||
                 descBB.MultiSampleType	!= MULTISAMPLE_NONE;

            GetLastError();
        }
        else
        {
            CAutoPtr<CSurface> pRenderTarget;
            hr = m_pSrcDevice->CreateRenderTarget(descBB.Width,
                                                  descBB.Height,
                                                  descBB.Format,
                                                  MULTISAMPLE_NONE ,
                                                  0,
                                                  TRUE,
                                                  &pRenderTarget );

            hr = m_pSrcDevice->StretchRect( pBackBuffer, NULL, pRenderTarget, NULL, D3DTEXF_NONE );

            hr = m_pFramework->GetImageCmp()->CompareImages( pRenderTarget, pSurfaceTX );

            m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();

            if(m_pFramework->m_DXGLog.InTestCase())
            {
                if(m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired())
                {
                    Fail();
                }
                else
                {
                    Pass();
                }
            }
        }
    }
    catch( CHRESULT hr )
    {
        return hr.setTestResult(this);
    }

    return true; //CGetRenderTargetData::ProcessFrame();
}

void CTextureCubeTexture::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CTextureCubeTexture::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CTextureCubeTexture::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CTextureCubeTexture::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pMipSurface = NULL;

    m_pCubeTexture = NULL;

    m_pMipTexture = NULL;

    m_pSrcRenderTarget = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CTextureCubeTexture::EndTestCase(void)
{
    if ( m_pSrcRenderTarget )
    {
        HRESULT hr = m_pSrcDevice->SetRenderTarget( m_pSrcRenderTarget );

        RELEASE( m_pSrcRenderTarget );
    }

    RELEASE( m_pMipSurface );

    RELEASE( m_pCubeTexture );

    RELEASE( m_pMipTexture );

    CGetRenderTargetData::EndTestCase();
}

bool CTextureCubeTexture::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CTextureCubeTexture::Cleanup()
{
    RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CTextureCubeTexture::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CTextureCubeTexture::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CTextureCubeTexture::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CTextureCubeTexture::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CTextureCubeTexture::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CTextureCubeTexture::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CTextureCubeTexture::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CTextureCubeTexture::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CTextureCubeTexture::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CTextureCubeTexture::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CGetRenderTargetData::CommandLineHelp();
}

void CTextureCubeTexture::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CTextureCubeTexture::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
