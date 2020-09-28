#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include "Utils.h"

CCubeTextureSurface::CCubeTextureSurface() :
    m_pTexture( 0 ),
    m_pVertexBuffer( 0 ),
    m_pCubeTexture( 0 ),
    m_pTextureSurface( 0 ),
    m_pPlainSurface( 0 ),
    m_pSrcRenderTarget( 0 )
{
    m_szTestName = _T("CUBTEXSURF");
    m_szCommandKey = _T("grtdcts");

    addParamNode( "CubeTexture", CCubeTextureParams() );
    addParamNode( "OffscreenPlainSurface", COffscreenPlainSurfaceParams() );
}

bool CCubeTextureSurface::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CCubeTextureSurface::CapsCheck()
{
    CAPS caps;
    m_pSrcDevice->GetDeviceCaps( &caps );

    if ( ( caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP ) != 0 )
    {
	    static UINT levels[] =
	    {
		    0
	    };

	    unionDomain( "CubeTexture/Levels", levels, levels + 1 );
    }

    return ( caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) != 0  &&
           ( caps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES ) != 0;
           
}

bool CCubeTextureSurface::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CCubeTextureSurface::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CCubeTextureSurface::Setup()
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

UINT CCubeTextureSurface::TestInitialize()
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
        USAGE_RENDERTARGET,
        USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
    };

    CAPS caps;
    m_pSrcDevice->GetDeviceCaps(&caps);
    unsigned end = 0 != (caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) ? 2 : 1;

    setDomain("CubeTexture/Usage", usages, usages + end);

    setDomain("OffscreenPlainSurface/Pool", Eq<DWORD>(POOL_SYSTEMMEM));
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
        m_szTestName = _T("CUBTEXSURFAPI");

        setDomain( "CubeTexture/Usage", All() );
        setDomain( "CubeTexture/Pool", All() );

        setDomain( "OffscreenPlainSurface/Format", BackBufferFmts() );
        setDomain( "OffscreenPlainSurface/Pool", All() );
    }
    else
    {
        static DWORD usages[] =
        {
            USAGE_RENDERTARGET,
            USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
        };

        CAPS caps;
        m_pSrcDevice->GetDeviceCaps( &caps );
        unsigned end = 0 != ( caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP ) ? 2 : 1;

        setDomain( "CubeTexture/Usage", usages, usages + end );

        setDomain( "OffscreenPlainSurface/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
    }
#endif

    unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CCubeTextureSurface::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CCubeTextureSurface::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
        UINT edgeLength;
        Dimension dim2;
        FMT format1, format2;
        UINT levels;
        DWORD usage;
        DWORD pool1, pool2;

        setState( nTest - 1 );

        getParameter( "CubeTexture/EdgeLength", edgeLength );
        getParameter( "CubeTexture/Levels", levels );
        getParameter( "CubeTexture/Usage", usage );
        getParameter( "CubeTexture/Format", format1 );
        getParameter( "CubeTexture/Pool", pool1 );

        if ( !m_bRunInvalid )
        {
            setParameter( "OffscreenPlainSurface/Dimension", Dimension( edgeLength, edgeLength ) );
            setParameter( "OffscreenPlainSurface/Format", format1 );
        }

        getParameter( "OffscreenPlainSurface/Dimension", dim2 );
        getParameter( "OffscreenPlainSurface/Format", format2 );
        getParameter( "OffscreenPlainSurface/Pool", pool2 );

        std::string str1, str2;
		getString( "CubeTexture", str1 );
		getString( "OffscreenPlainSurface", str2 );
        str1 += "/" + str2;
        BeginTestCase( str1.c_str(), nTest );

        hr = m_pSrcDevice->GetRenderTarget( &m_pSrcRenderTarget );

		if ( FAILED( m_pSrcDevice->CreateCubeTexture( levels ? edgeLength : m_Dimension, 
										              levels, 
										              usage, 
										              format1, 
										              pool1, 
										              &m_pCubeTexture ) ) ||
             FAILED( m_pSrcDevice->CreateOffscreenPlainSurface( dim2.Width,
                                                                dim2.Height,
                                                                format2, 
                                                                pool2,
                                                                &m_pPlainSurface ) ) )
        {
            Skip();
            GetLastError();
            return false;
        }

		DWORD nLevels = m_pCubeTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) edgeLength ) / log( 2.0 ) +.5);

		CUBEMAP_FACES face;
		getParameter( "CubeTexture/Face", face );

        hr = m_pCubeTexture->GetCubeMapSurface(face, level, &m_pTextureSurface);

        hr = m_pSrcDevice->SetRenderTarget( m_pTextureSurface );

        VIEWPORT viewport = 
        {
            0,
            0,
            edgeLength,
            edgeLength,
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

void CCubeTextureSurface::SceneRefresh()
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

bool CCubeTextureSurface::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        SURFACEDESC descRT;
        hr = m_pTextureSurface->GetDesc(&descRT);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(m_pTextureSurface, m_pPlainSurface) ) )
        {
            SURFACEDESC descPS;
            hr = m_pPlainSurface->GetDesc(&descPS);

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
                                                  descRT.MultiSampleType,
                                                  descRT.MultiSampleQuality,
                                                  TRUE,
                                                  &pRenderTarget );

            hr = m_pSrcDevice->StretchRect( m_pTextureSurface, NULL, pRenderTarget, NULL, D3DTEXF_NONE );

            hr = m_pFramework->GetImageCmp()->CompareImages( pRenderTarget, m_pPlainSurface );

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

void CCubeTextureSurface::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CCubeTextureSurface::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CCubeTextureSurface::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CCubeTextureSurface::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pTextureSurface = NULL;

    m_pPlainSurface = NULL;

    m_pCubeTexture = NULL;

    m_pSrcRenderTarget = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CCubeTextureSurface::EndTestCase(void)
{
    if ( m_pSrcRenderTarget )
    {
        HRESULT hr = m_pSrcDevice->SetRenderTarget( m_pSrcRenderTarget );

        RELEASE( m_pSrcRenderTarget );
    }

    RELEASE( m_pTextureSurface );

    RELEASE( m_pPlainSurface );

    RELEASE( m_pCubeTexture );

    CGetRenderTargetData::EndTestCase();
}

bool CCubeTextureSurface::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CCubeTextureSurface::Cleanup()
{
    RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CCubeTextureSurface::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CCubeTextureSurface::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CCubeTextureSurface::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CCubeTextureSurface::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CCubeTextureSurface::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CCubeTextureSurface::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CCubeTextureSurface::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CCubeTextureSurface::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CCubeTextureSurface::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CCubeTextureSurface::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CGetRenderTargetData::CommandLineHelp();
}

void CCubeTextureSurface::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CCubeTextureSurface::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
