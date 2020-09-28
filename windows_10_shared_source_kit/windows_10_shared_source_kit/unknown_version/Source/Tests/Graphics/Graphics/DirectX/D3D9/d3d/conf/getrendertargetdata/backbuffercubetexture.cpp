#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include <memory>

#include "Utils.h"


CBackBufferCubeTexture::CBackBufferCubeTexture() :
    m_pVertexBuffer( 0 ),
    m_pTexture( 0 ),
    m_pTextureSurface( 0 )
{
    m_szTestName = _T("BBCUBTEX");
    m_szCommandKey = _T("grtdbbct");

    m_Options.ModeOptions.BackBufferFmt = FMT_ALL;
    m_Options.ModeOptions.MultisampType = MULTISAMPLE_ALL;

    addParamNode( "CubeTexture", CCubeTextureParams() );
}

bool CBackBufferCubeTexture::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CBackBufferCubeTexture::CapsCheck()
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

    return ( caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) != 0;
}

bool CBackBufferCubeTexture::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CBackBufferCubeTexture::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CBackBufferCubeTexture::Setup()
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

UINT CBackBufferCubeTexture::TestInitialize()
{
	// define domains for variable parameters
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
    setDomain( "CubeTexture/Face", All() );

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("BBCTEXAPI");

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
		};

		setDomain( "CubeTexture/Usage", usages, usages + 2 );
		setDomain( "CubeTexture/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
	}

	unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CBackBufferCubeTexture::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CBackBufferCubeTexture::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
		setState( nTest - 1 );

		PRESENT_PARAMETERS Params;
		memcpy( &Params, m_pSrcDevice->GetPresParams(), sizeof( PRESENT_PARAMETERS ) );

        if ( !m_bRunInvalid )
        {
            setParameter( "CubeTexture/Format", (FMT) Params.BackBufferFormat.d3dfFormat );
        }

        std::string str1;
		getString( "CubeTexture", str1 );
        BeginTestCase( str1.c_str(), nTest );

		UINT edgeLength;
		getParameter( "CubeTexture/EdgeLength", edgeLength );
		DWORD pool;
		getParameter( "CubeTexture/Pool", pool );
		UINT levels;
		getParameter( "CubeTexture/Levels", levels );
		DWORD usage;
		getParameter( "CubeTexture/Usage", usage );
        FMT format;
        getParameter( "CubeTexture/Format", format );

        // update dimension of backbuffer and reset device
		Params.uWidth = Params.uHeight = edgeLength;
        Cleanup();
		hr = m_pSrcDevice->Reset( &Params );
        Setup();

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

		hr = m_pSrcDevice->CreateCubeTexture( levels ? edgeLength : m_Dimension,  
											  levels, 
											  usage, 
											  format, 
											  pool, 
											  &m_pTextureSurface );
    }
    catch( CHRESULT hr )
    {
        GetLastError();
		Skip();
        return false;
    }

    return CGetRenderTargetData::ExecuteTest(nTest);
}

void CBackBufferCubeTexture::SceneRefresh()
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
    }
    catch( CHRESULT hr )
    {
        hr.setTestResult(this);
    }

    CGetRenderTargetData::SceneRefresh();
}

bool CBackBufferCubeTexture::ProcessFrame(void)
{
    CHRESULT hr;

    try
    {
        CAutoPtr<CSurface> pBackBuffer;
        hr = m_pSrcDevice->GetBackBuffer(0, 0, BACKBUFFER_TYPE_MONO, &pBackBuffer);

        SURFACEDESC descBB;
        hr = pBackBuffer->GetDesc(&descBB);

		DWORD nLevels = m_pTextureSurface->GetLevelCount();

		unsigned level = nLevels == 1 ? 0 : m_maxLevels - unsigned( log( (double) descBB.Width ) / log( 2.0 ) +.5);

		CUBEMAP_FACES face;
		getParameter( "CubeTexture/Face", face );

        CAutoPtr<CSurface> pSurfaceTX;
        hr = m_pTextureSurface->GetCubeMapSurface(face, level, &pSurfaceTX);

        SURFACEDESC descTX;
        hr = m_pTextureSurface->GetLevelDesc(level, &descTX);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(pBackBuffer, pSurfaceTX) ) )
        {
            hr = descTX.Pool != POOL_SYSTEMMEM || 
                 descTX.Format.d3dfFormat != descBB.Format.d3dfFormat ||
                 descTX.Width != descBB.Width ||
                 descTX.Height != descBB.Height ||
                 descBB.MultiSampleType	!= MULTISAMPLE_NONE;

            GetLastError();
        }
        else
        {
            hr = m_pFramework->GetImageCmp()->CompareImages(pBackBuffer, pSurfaceTX);

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

void CBackBufferCubeTexture::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CBackBufferCubeTexture::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CBackBufferCubeTexture::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CBackBufferCubeTexture::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pTextureSurface = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CBackBufferCubeTexture::EndTestCase(void)
{
    RELEASE( m_pTextureSurface );

    CGetRenderTargetData::EndTestCase();
}

bool CBackBufferCubeTexture::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CBackBufferCubeTexture::Cleanup()
{
	RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CBackBufferCubeTexture::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CBackBufferCubeTexture::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CBackBufferCubeTexture::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CBackBufferCubeTexture::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CBackBufferCubeTexture::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CBackBufferCubeTexture::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CBackBufferCubeTexture::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CBackBufferCubeTexture::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CBackBufferCubeTexture::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CBackBufferCubeTexture::CommandLineHelp()
{
    CGetRenderTargetData::CommandLineHelp();
}

void CBackBufferCubeTexture::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CBackBufferCubeTexture::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
