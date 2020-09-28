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

CRenderTarget::CRenderTarget() :
    m_pTexture( 0 ),
    m_pVertexBuffer( 0 ),
    m_pRenderTarget( 0 ),
    m_pPlainSurface( 0 ),
    m_pSrcRenderTarget( 0 )
{
    m_szTestName = _T("RTSUR");
    m_szCommandKey = _T("grtdrt");

    addParamNode( "RenderTarget", CRenderTargetParams() );
    addParamNode( "OffscreenPlainSurface", COffscreenPlainSurfaceParams() );
}

bool CRenderTarget::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CRenderTarget::CapsCheck()
{
    return CGetRenderTargetData::CapsCheck();
}

bool CRenderTarget::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CRenderTarget::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CRenderTarget::Setup()
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

UINT CRenderTarget::TestInitialize()
{
#ifdef FAST_TEST
    static Dimension dims[] = 
    {
        Dimension(640, 480),
    };

    m_szTestName = _T("RTSURAPI");

    setDomain("RenderTarget/Dimension", dims, dims + ARRAYSIZE(dims));
    setDomain("OffscreenPlainSurface/Pool", Eq<DWORD>(POOL_SYSTEMMEM));
#else
    static Dimension dims[] = 
    {
        Dimension( 640, 480 ),
        Dimension( 800, 600 ),
        Dimension( 1024, 768 ),
        Dimension( 0, 0 ),
        Dimension( -1, -1 )
    };

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("RTSURAPI");

        setDomain( "RenderTarget/Dimension", dims, dims+5 );
        setDomain( "RenderTarget/MultiSample", All() );
        setDomain( "RenderTarget/Lockable", All() );

        setDomain("OffscreenPlainSurface/Dimension", dims, dims + 5);
        setDomain( "OffscreenPlainSurface/Format", BackBufferFmts() );
        setDomain( "OffscreenPlainSurface/Pool", All() );
    }
    else
    {
        setDomain( "RenderTarget/Dimension", dims, dims+3 );
        setDomain( "OffscreenPlainSurface/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
    }
#endif

    setDomain( "RenderTarget/Format", BackBufferFmts() );

    unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CRenderTarget::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CRenderTarget::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
        Dimension dim1, dim2;
        FMT format1, format2;
        MULTISAMPLE_TYPE multisample;
        DWORD quality;
        BOOL lockable;
        DWORD pool;

        setState( nTest - 1 );

        getParameter( "RenderTarget/Dimension", dim1 );
        getParameter( "RenderTarget/Format", format1 );
        getParameter( "RenderTarget/MultiSample", multisample );
        getParameter( "RenderTarget/MultisampleQuality", quality );
        getParameter( "RenderTarget/Lockable", lockable );

        if ( !m_bRunInvalid )
        {
            setParameter( "OffscreenPlainSurface/Dimension", dim1 );
            setParameter( "OffscreenPlainSurface/Format", format1 );
        }

        getParameter( "OffscreenPlainSurface/Dimension", dim2 );
        getParameter( "OffscreenPlainSurface/Format", format2 );
        getParameter( "OffscreenPlainSurface/Pool", pool );

        // construct testcase name
        std::string str1, str2;
        getString( "RenderTarget", str1 );
        getString( "OffscreenPlainSurface", str2 );
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
             FAILED( m_pSrcDevice->CreateOffscreenPlainSurface(dim2.Width,
                                                               dim2.Height,
                                                               format2, pool,
                                                               &m_pPlainSurface) ) )
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

void CRenderTarget::SceneRefresh()
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

bool CRenderTarget::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        if ( FAILED( m_pSrcDevice->GetRenderTargetData(m_pRenderTarget, m_pPlainSurface) ) )
        {
            SURFACEDESC descRT;
            hr = m_pRenderTarget->GetDesc(&descRT);

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
            hr = m_pFramework->GetImageCmp()->CompareImages( m_pRenderTarget, m_pPlainSurface );

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

void CRenderTarget::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CRenderTarget::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CRenderTarget::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CRenderTarget::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pRenderTarget = NULL;

    m_pPlainSurface = NULL;

    m_pSrcRenderTarget = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CRenderTarget::EndTestCase(void)
{
    if ( m_pSrcRenderTarget )
    {
        HRESULT hr = m_pSrcDevice->SetRenderTarget( m_pSrcRenderTarget );

        RELEASE( m_pSrcRenderTarget );
    }

    RELEASE( m_pRenderTarget );

    RELEASE( m_pPlainSurface );

    CGetRenderTargetData::EndTestCase();
}

bool CRenderTarget::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CRenderTarget::Cleanup()
{
    RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CRenderTarget::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CRenderTarget::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CRenderTarget::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CRenderTarget::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CRenderTarget::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CRenderTarget::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CRenderTarget::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CRenderTarget::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CRenderTarget::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CRenderTarget::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CGetRenderTargetData::CommandLineHelp();
}

void CRenderTarget::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CRenderTarget::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
