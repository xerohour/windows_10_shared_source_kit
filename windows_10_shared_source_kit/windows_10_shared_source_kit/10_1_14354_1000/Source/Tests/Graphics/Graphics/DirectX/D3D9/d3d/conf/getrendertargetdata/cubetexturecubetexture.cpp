#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include "Utils.h"


CCubeTextureCubeTexture::CCubeTextureCubeTexture() :
    m_pTexture( 0 ),
    m_pVertexBuffer( 0 ),
    m_pCubeTexture( 0 ),
    m_pTextureSurface( 0 ),
    m_pMipTexture( 0 ),
    m_pSrcRenderTarget( 0 )
{
    m_szTestName = _T("CUBTEXCUBTEX");
    m_szCommandKey = _T("grtdctct");

    addParamNode( "CubeTexture", CCubeTextureParams() );
    addParamNode( "Texture", CCubeTextureParams() );
}

bool CCubeTextureCubeTexture::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CCubeTextureCubeTexture::CapsCheck()
{
    CAPS caps;
    m_pSrcDevice->GetDeviceCaps( &caps );

    if ( ( caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP ) != 0 )
    {
	    static UINT levels[] =
	    {
		    0
	    };

	    unionDomain( "CubeTexture/Levels", levels, levels + ( sizeof( levels ) / sizeof( levels[0] ) ) );
    }

    return ( caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) != 0 &&
           ( caps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES ) != 0;
}

bool CCubeTextureCubeTexture::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CCubeTextureCubeTexture::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CCubeTextureCubeTexture::Setup()
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

UINT CCubeTextureCubeTexture::TestInitialize()
{
#ifdef FAST_TEST
    static UINT edgeLengths[] =
    {
        32,

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

    setDomain("CubeTexture/EdgeLength", edgeLengths, edgeLengths + (sizeof(edgeLengths) / sizeof(edgeLengths[0])));
    setDomain("CubeTexture/Format", formats, formats + ARRAYSIZE(formats));
    setDomain("CubeTexture/Face", faces, faces + ARRAYSIZE(faces));

    static DWORD usages[] =
    {
        0,
        USAGE_DYNAMIC,
        USAGE_RENDERTARGET,
        USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
    };

    setDomain("Texture/Usage", usages, usages + 2);
    setDomain("Texture/Pool", Eq<DWORD>(POOL_SYSTEMMEM));

    CAPS caps;
    m_pSrcDevice->GetDeviceCaps(&caps);
    unsigned end = 0 != (caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) ? 4 : 3;

    setDomain("CubeTexture/Usage", usages + 2, usages + end);
#else
    static UINT edgeLengths[] =
    {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        256,
        512,
        1024
    };

	setDomain( "CubeTexture/EdgeLength", edgeLengths, edgeLengths + ( sizeof( edgeLengths ) / sizeof( edgeLengths[0] ) ) );
    setDomain( "CubeTexture/Format", BackBufferFmts() );
    setDomain( "CubeTexture/Face", All() );

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("CUBTEXCUBTEXAPI");

        setDomain( "Texture/EdgeLength", edgeLengths, edgeLengths + ( sizeof( edgeLengths ) / sizeof( edgeLengths[0] ) ) );
        setDomain( "Texture/Usage", All() );
        setDomain( "Texture/Pool", All() );
        setDomain( "Texture/Format", BackBufferFmts() );
        setDomain( "Texture/Face", All() );

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

        setDomain( "Texture/Usage", usages, usages + 2 );
        setDomain( "Texture/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );

        CAPS caps;
        m_pSrcDevice->GetDeviceCaps( &caps );
        unsigned end = 0 != ( caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP ) ? 4 : 3;

        setDomain( "CubeTexture/Usage", usages + 2, usages + end );
    }
#endif

    unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CCubeTextureCubeTexture::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CCubeTextureCubeTexture::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
        UINT edgeLength1, edgeLength2;
        FMT format1, format2;
        UINT levels;
        DWORD usage1, usage2;
        DWORD pool1, pool2;
		CUBEMAP_FACES face;

        setState( nTest - 1 );

        getParameter( "CubeTexture/EdgeLength", edgeLength1 );
        getParameter( "CubeTexture/Levels", levels );
        getParameter( "CubeTexture/Usage", usage1 );
        getParameter( "CubeTexture/Format", format1 );
        getParameter( "CubeTexture/Pool", pool1 );
		getParameter( "CubeTexture/Face", face );

        if ( !m_bRunInvalid )
        {
            setParameter( "Texture/Dimension", edgeLength1 );
            setParameter( "Texture/Format", format1 );
            setParameter( "Texture/Face", face );
        }

        getParameter( "Texture/EdgeLength", edgeLength2 );
        getParameter( "Texture/Usage", usage2 );
        getParameter( "Texture/Format", format2 );
        getParameter( "Texture/Pool", pool2 );

        std::string str1, str2;
		getString( "CubeTexture", str1 );
		getString( "Texture", str2 );
		str1 += "/" + str2;
        BeginTestCase( str1.c_str(), nTest );

        hr = m_pSrcDevice->GetRenderTarget( &m_pSrcRenderTarget );

		if ( FAILED( m_pSrcDevice->CreateCubeTexture( levels ? edgeLength1 : m_Dimension, 
										              levels, 
										              usage1, 
										              format1, 
										              pool1, 
										              &m_pCubeTexture ) ) ||
             FAILED( m_pSrcDevice->CreateCubeTexture( levels ? edgeLength1 : m_Dimension, 
										              levels, 
										              usage2, 
										              format2, 
										              pool2, 
										              &m_pMipTexture ) ) )
        {
            Skip();
            GetLastError();
            return false;
        }

		DWORD nLevels = m_pCubeTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) edgeLength1 ) / log( 2.0 ) +.5);

        hr = m_pCubeTexture->GetCubeMapSurface(face, level, &m_pTextureSurface);

        hr = m_pSrcDevice->SetRenderTarget( m_pTextureSurface );

        VIEWPORT viewport = 
        {
            0,
            0,
            edgeLength1,
            edgeLength1,
            0.0f,
            1.0f
        };

        hr = m_pSrcDevice->SetViewport( &viewport );

    }
    catch( CHRESULT hr )
    {
        return false;
    }

    return CGetRenderTargetData::ExecuteTest(nTest);
}

void CCubeTextureCubeTexture::SceneRefresh()
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

        hr = m_pSrcDevice->StretchRect( m_pTextureSurface, NULL, m_pSrcRenderTarget, NULL, D3DTEXF_NONE );
    }
    catch( CHRESULT hr )
    {
        GetLastError();
        return;
    }

    CGetRenderTargetData::SceneRefresh();
}

bool CCubeTextureCubeTexture::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        CAutoPtr<CSurface> pBackBuffer;
        hr = m_pSrcDevice->GetRenderTarget( &pBackBuffer );

        SURFACEDESC descRT;
        hr = pBackBuffer->GetDesc(&descRT);

		DWORD nLevels = m_pCubeTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) descRT.Width ) / log( 2.0 ) +.5);

		CUBEMAP_FACES face;
		getParameter( "Texture/Face", face );

        CAutoPtr<CSurface> pSurfaceTX;
        hr = m_pMipTexture->GetCubeMapSurface(face, level, &pSurfaceTX);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(m_pTextureSurface, pSurfaceTX) ) )
        {
            SURFACEDESC descPS;
            hr = pSurfaceTX->GetDesc(&descPS);

            hr = descPS.Pool != POOL_SYSTEMMEM ||
                 descPS.Format.d3dfFormat != descRT.Format.d3dfFormat ||
                 descPS.Width != descRT.Width ||
                 descPS.Height != descRT.Height ||
                 descRT.MultiSampleType	!= MULTISAMPLE_NONE;

            Skip();
            GetLastError();
        }
        else
        {
            CAutoPtr<CSurface> pRenderTarget;
            hr = m_pSrcDevice->CreateRenderTarget(descRT.Width,
                                                  descRT.Height,
                                                  descRT.Format,
                                                  MULTISAMPLE_NONE ,
                                                  0,
                                                  TRUE,
                                                  &pRenderTarget );

            hr = m_pSrcDevice->StretchRect( m_pTextureSurface, NULL, pRenderTarget, NULL, D3DTEXF_NONE );

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

void CCubeTextureCubeTexture::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CCubeTextureCubeTexture::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CCubeTextureCubeTexture::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CCubeTextureCubeTexture::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pTextureSurface = NULL;

    m_pMipTexture = NULL;

    m_pCubeTexture = NULL;

    m_pSrcRenderTarget = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CCubeTextureCubeTexture::EndTestCase(void)
{
    if ( m_pSrcRenderTarget )
    {
        HRESULT hr = m_pSrcDevice->SetRenderTarget( m_pSrcRenderTarget );

        RELEASE( m_pSrcRenderTarget );
    }

    RELEASE( m_pTextureSurface );

    RELEASE( m_pMipTexture );

    RELEASE( m_pCubeTexture );

    CGetRenderTargetData::EndTestCase();
}

bool CCubeTextureCubeTexture::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CCubeTextureCubeTexture::Cleanup()
{
    RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CCubeTextureCubeTexture::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CCubeTextureCubeTexture::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CCubeTextureCubeTexture::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CCubeTextureCubeTexture::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CCubeTextureCubeTexture::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CCubeTextureCubeTexture::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CCubeTextureCubeTexture::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CCubeTextureCubeTexture::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CCubeTextureCubeTexture::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CCubeTextureCubeTexture::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CGetRenderTargetData::CommandLineHelp();
}

void CCubeTextureCubeTexture::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CCubeTextureCubeTexture::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
