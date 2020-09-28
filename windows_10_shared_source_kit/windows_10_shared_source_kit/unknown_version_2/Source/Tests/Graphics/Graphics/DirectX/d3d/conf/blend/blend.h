#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include "AdapterUtil.h"

#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "MinMaxRange.h"

using MinMaxRange::TEST_COLOR;

#include <cmath>

#define LOG_HEADER ("[" __FUNCTION__ "()]")
#define ZeroArrayMem(x) ZeroMemory((x), sizeof(x))
#define ZeroStructMem(x) ZeroMemory(&(x), sizeof(x))

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

union TEST_VEC4F
{
	struct{
		FLOAT a,b,c,d;
	};
	struct{
		FLOAT x,y,z,w;
	};
	FLOAT f[4];
};
static const TEST_COLOR s_BorderColor = {1.0, 0.75, 0.5, 1.0};

class CBlendConfTest : public CD3D11Test
{
public:
	CBlendConfTest()
		: m_pDevice1( NULL )
		, m_pDeviceContext1( NULL )
		, m_pDebug( NULL )
		, m_bFL9X( false )
		, m_pAdapterDescriptor( nullptr )
		, m_wddmDriverVersion( KMT_DRIVERVERSION_WDDM_1_0 )
		, m_nPrimitiveW( 16 )
		, m_nPrimitiveH( 16 )
		, m_nIndexCount( 0 )
		, m_pVS( NULL )
		, m_pPS( NULL )
		, m_pPSVisibleCopy( NULL )
		, m_pRSEnabled( NULL )
		, m_pVertexBuffer( NULL )
		, m_pIndexBuffer( NULL )
		//, m_pConstBuffer( NULL )
		, m_pLayout( NULL )
		, m_nDimension( 2 )
		, m_NumRTs( 0 )
		, m_nPrevNumRTs ( 0 )
		, m_RTEnableCombo( 0 )
		, m_fColorShift( 0.0f )
	{
		ZeroArrayMem(m_pConstBuffers);
		ZeroArrayMem(m_pTextureRTs);
		ZeroArrayMem(m_pTextureRTsStaging);
		ZeroArrayMem(m_pRTViews);
		ZeroArrayMem(m_pRTShaderViews);
		ZeroArrayMem(m_eRTFormats);
		ZeroArrayMem(m_eRTFormatsEffectiveRTV);
		ZeroArrayMem(m_eRTFormatsEffectiveSRV);
		ZeroArrayMem(m_vBlendFactor.f);
		ZeroArrayMem(m_pTempMultisampleData);
		ZeroStructMem(m_BlendDesc_1);
	}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );
	virtual TEST_RESULT	SetupTestCase();
	virtual void CleanupTestCase();
	virtual int GetNumRenderWindows() const;

protected:
	ID3D11Device1				*m_pDevice1;
	ID3D11DeviceContext1		*m_pDeviceContext1;
	ID3D11Debug					*m_pDebug;
	bool						m_bFL9X;
	DevX::GRFX::AdapterDescriptorCore	*m_pAdapterDescriptor;
	D3DKMT_DRIVERVERSION            m_wddmDriverVersion;

	int						m_nPrimitiveW, m_nPrimitiveH;
	int						m_nIndexCount;

	ID3D11VertexShader			*m_pVS;
	ID3D11PixelShader			*m_pPS;
	ID3D11PixelShader			*m_pPSVisibleCopy;
	ID3D11Texture2D				*m_pTextureRTs[8];
	ID3D11Texture2D				*m_pTextureRTsStaging[8];
	ID3D11RenderTargetView		*m_pRTViews[8];
	ID3D11ShaderResourceView		*m_pRTShaderViews[8];
	ID3D11RasterizerState		*m_pRSEnabled;
	ID3D11Buffer					*m_pVertexBuffer;
	ID3D11Buffer					*m_pIndexBuffer;
	ID3D11Buffer					*m_pConstBuffers[2];
	ID3D11InputLayout			*m_pLayout;

	tstring				m_sPixelShaderName;
	tstring				m_sPixelShaderNameFL9X;
	tstring				m_sPSEntryPt;
	tstring				m_sVertexShaderName;
	tstring				m_sVertexShaderNameFL9X;

private:
	FLOAT*				m_pTempMultisampleData[8];
	int					m_nPrevNumRTs;
	tstring				m_sPrevPSName;
	tstring				m_sPrevVSName;
	tstring				m_sPrevPSEntryPt;

protected:
	// test parameters to be tweaked by subclass
	D3D11_BLEND_DESC1	m_BlendDesc1;
	D3D11_BLEND_DESC	m_BlendDesc_1;
	TEST_VEC4F		m_vBlendFactor;
	DXGI_FORMAT		m_eRTFormats[8];
	DXGI_FORMAT		m_eRTFormatsEffectiveRTV[8];
	DXGI_FORMAT		m_eRTFormatsEffectiveSRV[8];
	int				m_NumRTs;
	int				m_RTEnableCombo;
	float			m_fColorShift;
	int				m_nDimension;
	// insert test parameter variables here

	HRESULT		UpdateResources();
	void		DoVisibleRender();
	void        DoShowResources();
	HRESULT		ReadBackBufferColor(int x, int y, int rtNum, TEST_COLOR* pColor);
	HRESULT		DrawQuad(const TEST_VEC4F vertColors[4], bool isSrc, const TEST_VEC4F& blendFactor, const TEST_VEC4F extraColors[4] = NULL);
	virtual TEST_RESULT	TestDrawnPixels(const TEST_VEC4F dstColors[4], const TEST_VEC4F srcColors[2][4], const TEST_VEC4F& blendFactor);
	bool IsValidFL9Format( const DXGI_FORMAT &format  ) const;
	// insert utility methods here


private:
	TEST_RESULT SetupDirect3D( );
	TEST_RESULT SetupGeometry( );
	TEST_RESULT SetupRenderTarget( );
	TEST_RESULT SetupShaders( );
	void CleanupDirect3D( );

	struct TVERTEX
	{
		float vPos[2];
		int vNum;
	};
};

class CBasicBlendTest : public CBlendConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	virtual TEST_RESULT SetupTestCase();

protected:
};

class CBlendStateTest : public CBlendConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	int		m_numDevices;
};

class CLogicOpTest : public CBlendConfTest
{
public:
	CLogicOpTest();
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

protected:
	virtual bool CreateShaders();
	virtual bool CreateBuffers();
	virtual bool LogicOp( const D3D11_LOGIC_OP &logicOp, const UINT32 &srcTextureValue, const UINT32 &srcRTTextureValue, const UINT32 &uCompMask, UINT32 &expectedRTData ) const;

protected:
	// Test resources
	ID3D11Texture2D *m_pSrcTexture2D;		// The reference data to be loaded from the PS
	ID3D11ShaderResourceView *m_pSrcTexture2DSRV;
	ID3D11BlendState1 *m_pLogicORBlendState;
	ID3D11PixelShader	*m_pPSInitRT;
	ID3D11Query *m_pPipelineQuery;

	UINT				*m_pSrcTextureData;
	tstring				m_sPSInitRTEntryPt;
	
	struct LOGICOPVERTEX
	{
		float vPos[2];
		float vTexCoord[2];
	};

	// Test parameters
	D3D11_LOGIC_OP m_logicOp;
};

class CBlendConfApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	bool InitTestGroups();
	void InitOptionVariables();
	bool VisibleRender() { return m_bVisibleRender; }
	bool Stretch() { return m_bStretch; }
	int Width() { return m_Width; }
	int Height() { return m_Height; }
	bool LogAllPixelFailures() { return m_bLogAllPixelFailures; }
private:
	int		m_Width, m_Height;
	bool	m_bVisibleRender;
	bool	m_bStretch;
	bool	m_bLogAllPixelFailures;
};
extern CBlendConfApp  g_App;


void operator*=(TEST_VEC4F& lhs, float rhs);
void operator*=(TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator*(const TEST_VEC4F& lhs, float rhs);
TEST_VEC4F operator*(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator+(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator-(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);

TEST_COLOR vec2color(const TEST_VEC4F& vec);
