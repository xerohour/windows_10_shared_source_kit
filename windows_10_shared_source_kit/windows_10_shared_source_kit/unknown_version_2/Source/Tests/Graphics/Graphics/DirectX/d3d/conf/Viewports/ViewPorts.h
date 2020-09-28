//////////////////////////////////////////////////////////////////////////
//  Viewports.h
//  created:	2006/04/21
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <WGFTestCore.h>
#include <D3D11Test.h>
#include <DXGIFloatTypes.h>

#include "ViewScissorDefines.h"

#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

struct pos
{
	pos(FLOAT X, FLOAT Y, FLOAT Z = 0.0f) {x=X;y=Y;z=Z;}
	FLOAT x;
	FLOAT y;
	FLOAT z;
};
struct color
{
	color(FLOAT R, FLOAT G, FLOAT B) {r=R;g=G;b=B;}
	color() {r=0.0f;g=0.0f;b=0.0f;}
	FLOAT r;
	FLOAT g;
	FLOAT b;
};
extern color MultiColors[16];
extern float ViewportShifts[16][2];

const color BLACK = color( 0.0f, 0.0f, 0.0f );
	
struct VERTS 
{
	pos Pos;
	color Color;
};

enum OUTPUT_TYPE
{
	OUTPUT_TYPE_RTV,
	OUTPUT_TYPE_UAV
};
DECLARE_NAMED_VALUES( OUTPUT_TYPE );
	
class ViewportsTestApp : public CD3D11TestFramework
{
public:
	ViewportsTestApp() : m_bPresent( false ) {}
	void Present() { if ( m_bPresent ) CD3D11TestFramework::Present( 0, 0, 0, 0, 0, 0 );	}
	bool getPresent() { return m_bPresent; }

protected:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

	void InitOptionVariables();
	bool InitTestGroups();

	bool m_bPresent;
};
extern ViewportsTestApp g_App;
	
class ViewportsTest : public CD3D11Test
{
public:
	ViewportsTest() : 
		m_OutputType(OUTPUT_TYPE_RTV),
		m_pDepthTestingDisabledState(NULL),
		m_pUAV(NULL),
		m_pOverdrawUAV(NULL),
		m_pUnorderedAccessTexture(NULL),
		m_pUnorderedAccessOverdrawTexture(NULL),
		m_NumViewports(0),
		m_NumScissors(0),
		m_numVertices(DEFAULT_NUM_VERTICES),
		m_bUseIndex(true),
		m_pTex2DCopy(NULL),
		m_pUAVOverdrawTexCopy(NULL),
		m_pCBuffer(NULL),
		m_pVBuffer(NULL),
		m_pVIBuffer(NULL),
		m_pVLayout(NULL),
		m_pRSView(NULL),
		m_pRSScissor(NULL),
		m_pBS(NULL),
		m_pVS(NULL),
		m_pGS(NULL),
		m_pRTVPS(NULL),
		m_pUAVPS(NULL),
		m_pRTView(NULL),
		m_pDSView(NULL),
		m_RTWidth(RENDER_TARGET_WIDTH),
		m_RTHeight(RENDER_TARGET_HEIGHT)
	{
		g_App.m_RenderTargetOptions.SwapChainDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		g_App.m_RenderTargetOptions.DepthTextureDesc.Height =
		g_App.m_RenderTargetOptions.SwapChainDesc.Height = m_RTHeight;
		g_App.m_RenderTargetOptions.DepthTextureDesc.Width =
		g_App.m_RenderTargetOptions.SwapChainDesc.Width = m_RTWidth;
	}

	int GetNumRenderWindows() const 
	{
		return GetShell()->IsConsole() ? 0 : 1;
	}
	bool CreateRTFromSwapChainProps() const { return true; }

protected:

	void InitRenderTargetOptions();

	virtual TEST_RESULT Setup();
	virtual void Cleanup();

	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase() = 0;
	virtual void CleanupTestCase() {}

	void ClearOutputTextures();

	testfactor::RFactor GetViewportsBaseRFactor();
	testfactor::RFactor GetOutputTypeBaseRFactor();

	bool MapRT( D3D11_MAPPED_SUBRESOURCE* TexMap, D3D11_MAPPED_SUBRESOURCE* OverdrawTexMap = NULL );
	inline void UnMapRT( bool UnMapOverdrawTex = false ) { GetImmediateContext()->Unmap(m_pTex2DCopy, 0); if(m_OutputType == OUTPUT_TYPE_UAV && UnMapOverdrawTex) GetImmediateContext()->Unmap(m_pUAVOverdrawTexCopy, 0); }

	OUTPUT_TYPE                 m_OutputType;
	ID3D11DepthStencilState*    m_pDepthTestingDisabledState;
	ID3D11UnorderedAccessView*  m_pUAV;
	ID3D11UnorderedAccessView*  m_pOverdrawUAV;
	ID3D11Texture2D	*           m_pUnorderedAccessTexture;
	ID3D11Texture2D	*           m_pUnorderedAccessOverdrawTexture;

	UINT				m_RTWidth;
	UINT				m_RTHeight;

	UINT				m_NumViewports;
	bool				m_bUseIndex;
	D3D11_VIEWPORT		m_Viewports[16];

	UINT				m_numVertices;

	UINT				m_NumScissors;
	D3D10_RECT			m_Scissors[16];

	ID3D11RenderTargetView	*m_pRTView;
	ID3D11DepthStencilView	*m_pDSView;

	ID3D11Texture2D			*m_pTex2DCopy;
	ID3D11Texture2D			*m_pUAVOverdrawTexCopy;
	ID3D11Buffer			*m_pCBuffer;
	ID3D11Buffer			*m_pVBuffer, *m_pVIBuffer;
	ID3D11InputLayout		*m_pVLayout;

	ID3D11RasterizerState	*m_pRSView;
	ID3D11RasterizerState	*m_pRSScissor;

	ID3D11BlendState			*m_pBS;

	ID3D11VertexShader		*m_pVS;
	ID3D11GeometryShader		*m_pGS;
	ID3D11PixelShader		*m_pRTVPS;
	ID3D11PixelShader		*m_pUAVPS;
};

tstring ToString( D3D11_VIEWPORT v );

RECT ViewportToRect( const D3D11_VIEWPORT& Viewport, D3D_FEATURE_LEVEL FeatureLevel );

void CalcRelativeDiff( color Expected, color Compare, FLOAT Diff[3] );
