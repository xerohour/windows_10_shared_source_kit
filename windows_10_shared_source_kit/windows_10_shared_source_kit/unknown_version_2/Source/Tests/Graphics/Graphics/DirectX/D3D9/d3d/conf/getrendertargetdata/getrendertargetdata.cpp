#include "GetRenderTargetData.h"

#include "d3d9.h"

//
// Declare the framework
//

CD3DWindowFramework App;

//
// Define the groups here
//

#ifndef FAST_TEST
CBackBuffer backBufferTst;
CBackBufferTexture backBufferTextureTst;
CBackBufferCubeTexture backBufferCubeTextureTst;
#endif
CRenderTarget renderTargetTst;
CRenderTargetTexture renderTargetTextureTst;
CRenderTargetCubeTexture renderTargetCubetextureTst;
CTextureSurface textureSurfaceTst;
CTextureTexture textureTextureTst;
CTextureCubeTexture textureCubeTextureTst;
CCubeTextureSurface cubetextureSurfaceTst;
CCubeTextureTexture cubetextureTextureTst;
CCubeTextureCubeTexture cubetextureCubetextureTst;
COddBalls oddBalls;

BOOL CGetRenderTargetData::m_firstTime = TRUE;

//
// Common Base Class Implementation
//

CGetRenderTargetData::CGetRenderTargetData( unsigned maxDimension ) :
	m_bRunInvalid( FALSE ),
	m_nVertices( 0 ),
    m_Dimension( maxDimension )
{
	SetTestType( TESTTYPE_CONF );

	m_Options.D3DOptions.fMinDXVersion = 9.f;
	m_Options.D3DOptions.bZBuffer = false;
    m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.bEnumReference = false;
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
    m_Options.D3DOptions.bCachedBackBuffer	= false;

    m_Options.ModeOptions.uMaxDisplayModes = 1;
    m_Options.ModeOptions.BackBufferFmt = FMT_ANY;

    m_Options.TestOptions.bCompareSrcRef = true;

    // maximum texture level index
    m_maxLevels = unsigned( log( (double) m_Dimension ) / log( 2.0 ) +.5);

    if ( CGetRenderTargetData::m_firstTime )
    {
        // add application specific typemaps
        D3DTypeMap.addTypeMap( new CFrameworkTypeMap );
        D3DTypeMap.addType( STREAMARG( Dimension ) );

        CGetRenderTargetData::m_firstTime = FALSE;
    }
}

bool CGetRenderTargetData::ParseCmdLine()
{
    if( KeySet( _T( "INV" ) ) )
    {
       m_bRunInvalid = TRUE;
    }

    return CD3DTest::ParseCmdLine();
}

bool CGetRenderTargetData::CapsCheck()
{
    return CD3DTest::CapsCheck();
}

bool CGetRenderTargetData::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CD3DTest::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CGetRenderTargetData::FilterResolution(PDISPLAYMODE pMode)
{
    return CD3DTest::FilterResolution(pMode);
}

bool CGetRenderTargetData::Setup()
{
    return CD3DTest::Setup();
}

UINT CGetRenderTargetData::TestInitialize()
{
    return CD3DTest::TestInitialize();
}

bool CGetRenderTargetData::ClearFrame(void)
{
    return CD3DTest::ClearFrame();
}

bool CGetRenderTargetData::ExecuteTest(UINT nTest)
{
    return CD3DTest::ExecuteTest(nTest);
}

void CGetRenderTargetData::SceneRefresh()
{
    CD3DTest::SceneRefresh();
}

bool CGetRenderTargetData::ProcessFrame(void)
{
    return CD3DTest::ProcessFrame();
}

void CGetRenderTargetData::UpdateStatus()
{
    CD3DTest::UpdateStatus();
}

void CGetRenderTargetData::UpdateDetails()
{
    CD3DTest::UpdateDetails();
}

bool CGetRenderTargetData::DisplayFrame(void)
{
    return CD3DTest::DisplayFrame();
}

void CGetRenderTargetData::EndTestCase(void)
{
    CD3DTest::EndTestCase();
}

bool CGetRenderTargetData::TestTerminate(void)
{
    return CD3DTest::TestTerminate();
}

void CGetRenderTargetData::Cleanup()
{
    CD3DTest::Cleanup();
}

void CGetRenderTargetData::OnDeviceLost()
{
    CD3DTest::OnDeviceLost();
}

void CGetRenderTargetData::OnReset()
{
    CD3DTest::OnReset();
}

bool CGetRenderTargetData::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CD3DTest::DrawStats(nHeight, nWidth, pDevice);
}

bool CGetRenderTargetData::SetDefaultMaterials()
{
    MATERIAL material;

	ZeroMemory(&material,sizeof(MATERIAL));
	material.diffuse.r  = 1.0f;
	material.diffuse.g  = 1.0f;
	material.diffuse.b  = 1.0f;
	material.diffuse.a  = 1.0f;
	
	material.ambient.r  = 1.0f;
	material.ambient.g  = 1.0f;
	material.ambient.b  = 1.0f;
	material.ambient.a  = 1.0f;
	
	material.specular.r = 1.0f;
	material.specular.g = 1.0f;
	material.specular.b = 1.0f;
	material.specular.a = 1.0f;

	material.emissive.r = 0.0f;
	material.emissive.g = 0.0f;
	material.emissive.b = 0.0f;
	material.emissive.a = 0.0f;
	
	material.power      = 20.0f;
	
    SetMaterial(&material);

    return CD3DTest::SetDefaultMaterials();
}

bool CGetRenderTargetData::SetDefaultTextures()
{
    return CD3DTest::SetDefaultTextures();
}

bool CGetRenderTargetData::SetDefaultViewport()
{
    return CD3DTest::SetDefaultViewport();
}

bool CGetRenderTargetData::SetDefaultLights()
{
    LIGHT light;

	ZeroMemory(&light,sizeof(LIGHT));
	light.dltType		= D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Diffuse.a		= 1.0f;

	light.Specular.r	= 1.0f;
	light.Specular.g	= 1.0f;
	light.Specular.b	= 1.0f;
	light.Specular.a	= 1.0f;

	light.Position.x	= 0.0f;
	light.Position.y	= 0.0f;
	light.Position.z	= 0.0f;

	light.Direction.x	= 0.0f;
	light.Direction.y	= 0.0f;
	light.Direction.z	= 1.0f;

	light.Range			= 1000.0f;

    SetLight(0,&light);

    LightEnable(0,TRUE);

    return CD3DTest::SetDefaultLights();
}

bool CGetRenderTargetData::SetDefaultRenderStates()
{
    return CD3DTest::SetDefaultRenderStates();
}

bool CGetRenderTargetData::SetDefaultMatrices()
{
    return CD3DTest::SetDefaultMatrices();
}

void CGetRenderTargetData::CommandLineHelp()
{
    CD3DTest::CommandLineHelp();
}

void CGetRenderTargetData::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CD3DTest::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
