#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include "Utils.h"

/*
 *  Create testcase nodes
 */

CRenderTargetCubeTexture::CRenderTargetCubeTexture() :
    m_pTexture( 0 ),
    m_pVertexBuffer( 0 ),
    m_pRenderTarget( 0 ),
    m_pCubeTexture( 0 ),
    m_pSrcRenderTarget( 0 )
{
    m_szTestName = _T("RTCUBTEX");
    m_szCommandKey = _T("grtdrtct");

    addParamNode( "RenderTarget", CRenderTargetParams() );
    addParamNode( "CubeTexture", CCubeTextureParams() );
}

bool CRenderTargetCubeTexture::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CRenderTargetCubeTexture::CapsCheck()
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

    return ( caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) != 0;
}

bool CRenderTargetCubeTexture::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CRenderTargetCubeTexture::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CRenderTargetCubeTexture::Setup()
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

UINT CRenderTargetCubeTexture::TestInitialize()
{
#ifdef FAST_TEST
    m_szTestName = _T("RTCUBTEXAPI");

    static Dimension dims[] =
    {
        Dimension( 32, 32 ),
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

    setDomain("RenderTarget/Dimension", dims, dims + ARRAYSIZE(dims));
    setDomain("RenderTarget/Format", formats, formats + ARRAYSIZE(formats));
    setDomain("CubeTexture/Face", faces, faces + ARRAYSIZE(faces));

    static DWORD usages[] =
    {
        0,
        USAGE_DYNAMIC
    };

    setDomain("CubeTexture/Usage", usages, usages + ARRAYSIZE(usages));
    setDomain("CubeTexture/Pool", Eq<DWORD>(POOL_SYSTEMMEM));

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

	setDomain( "RenderTarget/Dimension", dims, dims + ( sizeof( dims ) / sizeof( dims[0] ) ) );
    setDomain( "RenderTarget/Format", BackBufferFmts() );

    setDomain( "CubeTexture/Face", All() );

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("RTCUBTEXAPI");

        setDomain( "RenderTarget/MultiSample", All() );
        setDomain( "RenderTarget/Lockable", All() );

        setDomain( "CubeTexture/Format", BackBufferFmts() );
        setDomain( "CubeTexture/Usage", All() );
        setDomain( "CubeTexture/Pool", All() );
    }
    else
    {
        static DWORD usages[] =
        {
            0,
            USAGE_DYNAMIC
        };

        setDomain( "CubeTexture/Usage", usages, usages + 2 );
        setDomain( "CubeTexture/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
    }
#endif

    unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CRenderTargetCubeTexture::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CRenderTargetCubeTexture::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
        Dimension dim1;
        FMT format1, format2;
        MULTISAMPLE_TYPE multisample;
        DWORD quality, usage;
        BOOL lockable;
        DWORD pool;
        UINT levels, edgeLength;

        setState( nTest - 1 );

        getParameter( "RenderTarget/Dimension", dim1 );
        getParameter( "RenderTarget/Format", format1 );
        getParameter( "RenderTarget/MultiSample", multisample );
        getParameter( "RenderTarget/MultisampleQuality", quality );
        getParameter( "RenderTarget/Lockable", lockable );

        if ( !m_bRunInvalid )
        {
            setParameter( "CubeTexture/EdgeLength", dim1.Width );
            setParameter( "CubeTexture/Format", format1 );
        }

        getParameter( "CubeTexture/EdgeLength", edgeLength );
        getParameter( "CubeTexture/Levels", levels );
        getParameter( "CubeTexture/Usage", usage );
        getParameter( "CubeTexture/Format", format2 );
        getParameter( "CubeTexture/Pool", pool );

        // construct testcase name
        std::string str1, str2;
        getString( "RenderTarget", str1 );
        getString( "CubeTexture", str2 );
        str1 += "/" + str2;
        BeginTestCase( str1.c_str(), nTest );

        hr = m_pSrcDevice->GetRenderTarget( &m_pSrcRenderTarget );

        if ( FAILED( m_pSrcDevice->CreateRenderTarget(dim1.Width,
                                                      dim1.Height,
                                                      format1,
                                                      multisample,
                                                      quality,
                                                      lockable,
                                                      &m_pRenderTarget ) ) ||
            FAILED( m_pSrcDevice->CreateCubeTexture(levels ? edgeLength : m_Dimension,
                                                    levels,
                                                    usage,
                                                    format2, 
                                                    pool,
                                                    &m_pCubeTexture) ) )
        {
            Skip();
            GetLastError();
            return false;
        }

        hr = m_pSrcDevice->SetRenderTarget( m_pRenderTarget );

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

void CRenderTargetCubeTexture::SceneRefresh()
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

        hr = m_pSrcDevice->StretchRect( m_pRenderTarget, NULL, m_pSrcRenderTarget, NULL, D3DTEXF_NONE );
    }
    catch( CHRESULT hr )
    {
        GetLastError();
        return;
    }

    CGetRenderTargetData::SceneRefresh();
}

bool CRenderTargetCubeTexture::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        SURFACEDESC descRT;
        hr = m_pRenderTarget->GetDesc(&descRT);

		DWORD nLevels = m_pCubeTexture->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) descRT.Width ) / log( 2.0 ) +.5);

		CUBEMAP_FACES face;
		getParameter( "CubeTexture/Face", face );

        CAutoPtr<CSurface> pSurfaceTX;
        hr = m_pCubeTexture->GetCubeMapSurface(face, level, &pSurfaceTX);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(m_pRenderTarget, pSurfaceTX) ) )
        {
            SURFACEDESC descPS;
            hr = m_pCubeTexture->GetLevelDesc(level, &descPS);

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
            hr = m_pFramework->GetImageCmp()->CompareImages( m_pRenderTarget, pSurfaceTX );

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

void CRenderTargetCubeTexture::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CRenderTargetCubeTexture::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CRenderTargetCubeTexture::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CRenderTargetCubeTexture::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pRenderTarget = NULL;

    m_pCubeTexture = NULL;

    m_pSrcRenderTarget = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CRenderTargetCubeTexture::EndTestCase(void)
{
    if ( m_pSrcRenderTarget )
    {
        HRESULT hr = m_pSrcDevice->SetRenderTarget( m_pSrcRenderTarget );

        RELEASE( m_pSrcRenderTarget );
    }

    RELEASE( m_pRenderTarget );

    RELEASE( m_pCubeTexture );

    CGetRenderTargetData::EndTestCase();
}

bool CRenderTargetCubeTexture::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CRenderTargetCubeTexture::Cleanup()
{
    RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CRenderTargetCubeTexture::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CRenderTargetCubeTexture::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CRenderTargetCubeTexture::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CRenderTargetCubeTexture::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CRenderTargetCubeTexture::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CRenderTargetCubeTexture::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CRenderTargetCubeTexture::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CRenderTargetCubeTexture::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CRenderTargetCubeTexture::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CRenderTargetCubeTexture::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CGetRenderTargetData::CommandLineHelp();
}

void CRenderTargetCubeTexture::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CRenderTargetCubeTexture::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
