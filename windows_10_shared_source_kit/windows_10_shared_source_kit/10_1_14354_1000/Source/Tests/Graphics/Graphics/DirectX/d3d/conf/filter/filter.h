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
#include "FormatHelper.h"

using MinMaxRange::TEST_COLOR;

#include <cmath>

const DXGI_FORMAT	FORMAT_RT = DXGI_FORMAT_R32G32B32A32_FLOAT;
const DXGI_FORMAT	FORMAT_RT_L9 = DXGI_FORMAT_R8G8B8A8_UNORM;

#define LOG_HEADER ("[" __FUNCTION__ "()]")
#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))

HRESULT CompileShader(CD3D11Test* pTest, tstring shaderName, LPCSTR entryPoint, D3D_SHADER_STAGE shaderStage, 
					  ID3D11VertexShader** pVS, ID3D11GeometryShader** pGS, ID3D11PixelShader** pPS,
					  ID3D10Blob** pShaderBufOut);


typedef enum MINMAX_FILTERTYPE
{
	NOT_MINMAX_FILTER,
	MINIMUM_FILTER,
	MAXIMUM_FILTER
} MINMAX_FILTERTYPE;

union TEST_VEC4F
{
	struct{
		float a,b,c,d;
	};
	struct{
		float x,y,z,w;
	};
	float f[4];
};
static const TEST_COLOR s_BorderColor = {0.5, 0.75, 1.0, 0.5};

class CFilterConfTest : public CD3D11Test
{
public:
	CFilterConfTest()
		: m_pDevice(NULL)
		, m_pDeviceContext(NULL)
		, m_pDebug(NULL)
		, m_nPrimitiveW( 16 )
		, m_nPrimitiveH( 16 )
		, m_nIndexCount( 0 )
		, m_pVS( NULL )
		, m_pGS( NULL )
		, m_pPS( NULL )
		, m_pVSDefault( NULL )
		, m_pPSVisibleCopy( NULL )
		, m_FormatRT( FORMAT_RT )
		, m_pTextureRT( NULL )
		, m_pRTShaderView( NULL )
		, m_pRTView( NULL )
		, m_pRSEnabled( NULL )
		, m_pVertexBufferQuad( NULL )
		, m_pVertexBufferPoints( NULL )
		, m_pIndexBufferQuad( NULL )
		, m_pLayout( NULL )
		, m_pLayoutDefault( NULL )
		, m_nMultiSamples( 1 )
		, m_nPrevSamples( 1 )
		, m_pTexData( NULL )
		, m_pTempMultisampleData( NULL )
		, m_nArraySize( 0 )
		, m_bMultisampleEnable( false )
		, m_bSampleFreqShader( false )
		, m_bDefaultSampler( false )
		, m_bCustomTexCoords( false )
		, m_nCustomTexCoordNumber( 0 )
		, m_TexCoordRotation( 0 )
		, m_bSampleAOffImmI( false )
		, m_ShaderType( ShaderType_Pixel )
		, m_SampleFunction( Sample )
		, m_fResourceMinLOD( 0.0f )
	{
		m_pConstBuffers[0] = NULL;
		m_pConstBuffers[1] = NULL;
		ZeroMemory(&m_SamplerDesc, sizeof(m_SamplerDesc));
		for (int i = 0; i < 4; ++i)
			m_SamplerDesc.BorderColor[i] = (float)s_BorderColor.f[i];
		m_SamplerDesc.MinLOD = -FLT_MAX;
		m_SamplerDesc.MaxLOD = FLT_MAX;
		m_SamplerDesc.MipLODBias = 0;
	}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );
	virtual TEST_RESULT	SetupTestCase();
	virtual void CleanupTestCase();
	virtual int GetNumRenderWindows() const;

	static void CalcMipSize(int texWHD[3], int mipLevel, int mipWHD[3], int nDimension);

	enum eTexPattern
	{
		TexPattern_Solid,
		TexPattern_Checker,
		TexPattern_Gradient,
	};
	enum eShaderType
	{
		ShaderType_Vertex,
		ShaderType_Geometry,
		ShaderType_Pixel,
		ShaderType_MipGen,
	};
	enum eSampleFunction
	{
		Sample = 0,
		//SampleAOff,
		SampleBias,
		//SampleBiasAOff,
		SampleLevel,
		//SampleLevelAOff,
		SampleGrad,
		//SampleGradAOff,
		SampleCmp,
		//SampleCmpAOff,
		SampleCmpLvlZero,
		//SampleCmpLvlZeroAOff,
		SampleFuncMaxVal
	};

protected:
	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pDeviceContext;
	ID3D11Debug				*m_pDebug;

	int						m_nPrimitiveW, m_nPrimitiveH;
	int						m_nIndexCount;
	DXGI_FORMAT				m_FormatRT;

	ID3D11VertexShader			*m_pVS;
	ID3D11GeometryShader			*m_pGS;
	ID3D11PixelShader			*m_pPS;
	ID3D11VertexShader			*m_pVSDefault;
	ID3D11PixelShader			*m_pPSVisibleCopy;
	ID3D11Texture2D				*m_pTextureRT;
	ID3D11ShaderResourceView		*m_pRTShaderView;
	ID3D11RenderTargetView		*m_pRTView;
	ID3D11RasterizerState		*m_pRSEnabled;
	ID3D11Buffer					*m_pVertexBufferQuad;
	ID3D11Buffer					*m_pVertexBufferPoints;
	ID3D11Buffer					*m_pIndexBufferQuad;
	ID3D11Buffer					*m_pConstBuffers[2];
	ID3D11InputLayout			*m_pLayout;
	ID3D11InputLayout			*m_pLayoutDefault;

	tstring				m_sPixelShaderName;
	tstring				m_sPSEntryPt;
	tstring				m_sVSEntryPt;
	tstring				m_sGSEntryPt;
	tstring				m_sVertexShaderName;
	tstring				m_sGeometryShaderName;
private:
	int					m_nPrevSamples;
	DXGI_FORMAT			m_ePrevFormat;
	tstring				m_sPrevVSName;
	tstring				m_sPrevGSName;
	tstring				m_sPrevPSName;
	tstring				m_sPrevVSEntryPt;
	tstring				m_sPrevGSEntryPt;
	tstring				m_sPrevPSEntryPt;

protected:
	// test parameters to be tweaked by subclass

	TEST_COLOR**		m_pTexData; // [mip][texel]
	FLOAT*			m_pTempMultisampleData;
	int			m_nMultiSamples;
	int			m_nDimension;
	int			m_nTextureSize[3];
	int			m_nArraySize;
	DXGI_FORMAT		m_eFormat;
	double			m_fMinMagCoeff;
	int			m_nNumMips;
	int			m_nMinMip;
	D3D11_SAMPLER_DESC	m_SamplerDesc;
	eShaderType		m_ShaderType;
	eSampleFunction		m_SampleFunction;
	bool			m_bDefaultSampler;
	bool			m_bSampleAOffImmI;
	bool			m_bCustomTexCoords;
	bool			m_bMultisampleEnable;
	bool			m_bSampleFreqShader;
	int			m_nCustomTexCoordNumber;
	int			m_TexCoordRotation;
	TEST_VEC4F		m_vTexCoords[4];
	float           	m_fResourceMinLOD;
	// insert test parameter variables here

	void		PreInitTestParams();
	HRESULT		UpdateResources();
	bool		CreateTestMips(int mipLevels, int texWHD[3]);
	HRESULT		CreateTexturePattern(ID3D11Resource** ppRsc, eTexPattern texPattern, bool bHDRFormat = false, bool bMipGen = false);
	HRESULT		CreateTextureNoPattern(ID3D11Resource** ppRsc, bool bStaging = true);
	void		GenerateTextureCoords();
	HRESULT		ReadBackBufferColor(int x, int y, TEST_COLOR* pColor);
	void		DoVisibleRender();
	void		DoShowResources();
	HRESULT		DrawQuad();
	virtual TEST_RESULT	TestDrawnPixels(const TEST_VEC4F texCoords[4], float fThresholdExtra = 0); // can be overridden by specific tests
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

class CBasicFilterTest : public CFilterConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
};

class CMipFilterTest : public CFilterConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
};

class CAnisoFilterTest : public CFilterConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	float		m_fAnisoMorph;
};

class CMipGenTest : public CFilterConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	TEST_RESULT TestMipGenResult(ID3D11Resource* pTexRsc);
};

class ID3D11SamplerStateTest : public CD3D11Test
{
public:
	ID3D11SamplerStateTest()
		: m_pDevice( NULL )
		, m_numDevices( 0 )
	{}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );

	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	ID3D11Device*	m_pDevice;
	int			m_numDevices;
};

class CResourceMinLODScenario : public CD3D11Test
{
public:
	enum eUpdateMethod
	{
		// Map is not supported because dynamic textures can only
		// have 1 mip level
		UpdateMethod_UpdateSubresource,
		UpdateMethod_CopySubresource,
	};

	CResourceMinLODScenario();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	static const UINT NUM_LOD_STEPS = 64;

private:

	// Test Parameters
	bool            m_bUseInitialData;
	bool            m_bUpdateDuringRender;
	bool            m_bClampBeforeUpdate;
	eUpdateMethod   m_UpdateMethod;
	DXGI_FORMAT     m_TexFormat;
	UINT            m_TextureSize[3];
	UINT            m_TextureDimension;
	D3D11_USAGE     m_TextureUsage;

	UINT GetMipCount();
	TEST_RESULT CreateRenderTarget(UINT ArraySize, ID3D11Texture2D** ppRT, ID3D11RenderTargetView** ppRTV);
	TEST_RESULT CreateTexture(ID3D11Resource** ppResource, ID3D11Resource** ppStaging, ID3D11ShaderResourceView** ppSRV);
	void Render(UINT uArraySlice,ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV);
	TEST_RESULT CheckResults(ID3D11Texture2D* pRT);
	void GetMipDimension(UINT uMip, UINT uMipDimension[3]);
	FLOAT_COLOR GetExpectedColor(UINT uMipFloor, float fFrc);
	UINT GetMipColor(UINT uMip);
	UINT GetBPP();

	UINT                               m_uNextColorIndex;
	UINT                               m_LastGeneratedColor[D3D11_REQ_MIP_LEVELS];
	FLOAT_COLOR                        m_ExpectedColor[NUM_LOD_STEPS];
	ID3D11SamplerState*                m_pSamplerState;
	ID3D11Buffer*                      m_pConstBuff;
	ID3D11VertexShader*                m_pVS;
	ID3D11GeometryShader*              m_pGS;
	ID3D11PixelShader*                 m_pPS[3];
};

class CDummyTest : public CD3D11Test
{
private:
	void InitTestParameters() {};
};

class CFilterConfApp : public CD3D11TestFramework
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
	TEST_RESULT QueryDeviceProperties();
	bool GetIsFL9Test() { return m_bFL9Test; }
	D3DKMT_DRIVERVERSION GetDriverVersion() { return m_kmtDriverVersion; }

private:
	int		m_Width, m_Height;
	bool	m_bVisibleRender = false;
	bool	m_bStretch;
	bool	m_bLogAllPixelFailures;
	
	// device property members
	D3D_FEATURE_LEVEL m_MaxHardwareFL;
	bool	m_bSupportL9PerfFeatures;
	D3DKMT_DRIVERVERSION m_kmtDriverVersion;
	bool	m_bFL9Test;

};
extern CFilterConfApp  g_App;



float dot4f(const TEST_VEC4F& A, const TEST_VEC4F& B);
TEST_VEC4F cross3f(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
void operator*=(TEST_VEC4F& lhs, float rhs);
void operator*=(TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator*(const TEST_VEC4F& lhs, float rhs);
TEST_VEC4F operator*(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator-(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
TEST_VEC4F operator+(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs);
int pow2roundup (int x);

DECLARE_NAMED_VALUES( CFilterConfTest::eShaderType )
DECLARE_NAMED_VALUES( CFilterConfTest::eSampleFunction )
