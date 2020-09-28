#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include <memory>

#include "Utils.h"


CBackBufferTexture::CBackBufferTexture() :
    m_pVertexBuffer( 0 ),
    m_pTexture( 0 ),
    m_pTextureSurface( 0 )
{
    m_szTestName = _T("BBTEX");
    m_szCommandKey = _T("grtdbbt");

    m_Options.ModeOptions.BackBufferFmt = FMT_ALL;
    m_Options.ModeOptions.MultisampType = MULTISAMPLE_ALL;

    addParamNode( "Texture", CTextureParams() );
}

bool CBackBufferTexture::ParseCmdLine()
{
    return CGetRenderTargetData::ParseCmdLine();
}

bool CBackBufferTexture::CapsCheck()
{
    return CGetRenderTargetData::CapsCheck();
}

bool CBackBufferTexture::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CBackBufferTexture::FilterResolution(PDISPLAYMODE pMode)
{
    return CGetRenderTargetData::FilterResolution(pMode);
}

bool CBackBufferTexture::Setup()
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

UINT CBackBufferTexture::TestInitialize()
{
	// define domains for variable parameters
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

	static UINT levels[] =
	{
        0,
		1
	};

	setDomain( "Texture/Levels", levels, levels + 2 );

    if ( m_bRunInvalid )
    {
        m_szTestName = _T("BBTEXAPI");

        setDomain( "Texture/Format", BackBufferFmts() );
        setDomain( "Texture/Usage", All() );
        setDomain( "Texture/Pool", All() );
    }
    else
    {
		static DWORD usages[] = 
		{
            0,
			USAGE_DYNAMIC,
		};

		setDomain( "Texture/Usage", usages, usages + 2 );
		setDomain( "Texture/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
	}

	unsigned nCount = cycleCount();

    SetTestRange(1, nCount );

    return CGetRenderTargetData::TestInitialize();
}

bool CBackBufferTexture::ClearFrame(void)
{
    return CGetRenderTargetData::ClearFrame();
}

bool CBackBufferTexture::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    try
    {
		setState( nTest - 1 );

		PRESENT_PARAMETERS Params;
		memcpy( &Params, m_pSrcDevice->GetPresParams(), sizeof( PRESENT_PARAMETERS ) );

        if ( !m_bRunInvalid )
        {
            setParameter( "Texture/Format", (FMT) Params.BackBufferFormat.d3dfFormat );
        }

        std::string str1;
		getString( "Texture", str1 );
        BeginTestCase( str1.c_str(), nTest );

		Dimension dimension;
		getParameter( "Texture/Dimension", dimension );
		DWORD pool;
		getParameter( "Texture/Pool", pool );
		UINT levels;
		getParameter( "Texture/Levels", levels );
		DWORD usage;
		getParameter( "Texture/Usage", usage );
        FMT format;
        getParameter( "Texture/Format", format );

        // update dimension of backbuffer and reset device
		Params.uWidth = dimension.Width;
		Params.uHeight = dimension.Height;
        Cleanup();
		hr = m_pSrcDevice->Reset( &Params );
        Setup();

        VIEWPORT viewport = 
        {
            0,
            0,
            dimension.Width,
            dimension.Height,
            0.0f,
            1.0f
        };

        hr = m_pSrcDevice->SetViewport( &viewport );

		hr = m_pSrcDevice->CreateTexture( levels ? dimension.Width : m_Dimension, 
										  levels ? dimension.Height : m_Dimension, 
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

void CBackBufferTexture::SceneRefresh()
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

bool CBackBufferTexture::ProcessFrame(void)
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

        CAutoPtr<CSurface> pSurfaceTX;
        hr = m_pTextureSurface->GetSurfaceLevel(level, &pSurfaceTX);

        if ( FAILED( m_pSrcDevice->GetRenderTargetData(pBackBuffer, pSurfaceTX) ) )
        {
            SURFACEDESC descTX;
            hr = m_pTextureSurface->GetLevelDesc(level, &descTX);

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

void CBackBufferTexture::UpdateStatus()
{
    CGetRenderTargetData::UpdateStatus();
}

void CBackBufferTexture::UpdateDetails()
{
    CGetRenderTargetData::UpdateDetails();
}

bool CBackBufferTexture::DisplayFrame(void)
{
    return CGetRenderTargetData::DisplayFrame();
}

void CBackBufferTexture::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
    m_pTextureSurface = NULL;

    CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CBackBufferTexture::EndTestCase(void)
{
    RELEASE( m_pTextureSurface );

    CGetRenderTargetData::EndTestCase();
}

bool CBackBufferTexture::TestTerminate(void)
{
    return CGetRenderTargetData::TestTerminate();
}

void CBackBufferTexture::Cleanup()
{
	RELEASE( m_pTexture );

    RELEASE( m_pVertexBuffer );

    CGetRenderTargetData::Cleanup();
}

void CBackBufferTexture::OnDeviceLost()
{
    CGetRenderTargetData::OnDeviceLost();
}

void CBackBufferTexture::OnReset()
{
    CGetRenderTargetData::OnReset();
}

bool CBackBufferTexture::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CBackBufferTexture::SetDefaultMaterials()
{
    return CGetRenderTargetData::SetDefaultMaterials();
}

bool CBackBufferTexture::SetDefaultTextures()
{
    return CGetRenderTargetData::SetDefaultTextures();
}

bool CBackBufferTexture::SetDefaultViewport()
{
    return CGetRenderTargetData::SetDefaultViewport();
}

bool CBackBufferTexture::SetDefaultLights()
{
    return CGetRenderTargetData::SetDefaultLights();
}

bool CBackBufferTexture::SetDefaultRenderStates()
{
    return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CBackBufferTexture::SetDefaultMatrices()
{
    return CGetRenderTargetData::SetDefaultMatrices();
}

void CBackBufferTexture::CommandLineHelp()
{
    CGetRenderTargetData::CommandLineHelp();
}

void CBackBufferTexture::SetTestType(TESTTYPE Type)
{
    CGetRenderTargetData::SetTestType(Type);
}

void CBackBufferTexture::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
