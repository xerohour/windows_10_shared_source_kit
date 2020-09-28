#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "D3DSampleLocation.h"


const DXGI_FORMAT	FORMAT_RT = DXGI_FORMAT_R32G32B32A32_FLOAT;
const int PRIMITIVE_WIDTH = 2;
const int PRIMITIVE_HEIGHT = 2;

#define LOG_HEADER ("[" __FUNCTION__ "()]")

union clipvec4
{
	struct {
		float a,b,c,d;
	};
	float f[4];
};
struct TEST_COLOR
{
	DOUBLE r;
	DOUBLE g;
	DOUBLE b;
	DOUBLE a;
};
enum pixelLit_t
{
	pixel_edgecase,
	pixel_unlit,
	pixel_partial_low,
	pixel_partial_high,
	pixel_lit,
};

class CClippingConfTest : public CD3D11Test
{
public:
	CClippingConfTest()
		: m_pDebug(NULL)
		, m_nPrimitiveW( PRIMITIVE_WIDTH )
		, m_nPrimitiveH( PRIMITIVE_HEIGHT )
		, m_nIndexCount( 0 )
		, m_pVS( NULL )
		, m_pPS( NULL )
		, m_FormatRT( FORMAT_RT )
		, m_pTextureRT( NULL )
		, m_pTextureRTResolve( NULL )
		, m_pTextureRTCopy( NULL )
		, m_pRTView( NULL )
		, m_pRSEnabled( NULL )
		, m_pVertexBuffer( NULL )
		, m_pIndexBuffer( NULL )
		//, m_pConstBuffer( NULL )
		, m_pLayout( NULL )
		, m_nMultisamples( 1 )
		, m_nPrevSamples( 1 )
		//, m_szPixelShaderName( NULL )
		//, m_szVertexShaderName( NULL )
		//, m_szPrevPSName( NULL )
		//, m_szPrevVSName( NULL )
		, m_bEnableXYClip( false )
		, m_bEnableZClip( false )
		, m_bEnablePipelineStats( false )
	{
		m_pConstBuffers[0] = NULL;
		m_pConstBuffers[1] = NULL;
	}
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual void Cleanup( );

protected:
	ID3D11Debug			*m_pDebug;

	int							m_nPrimitiveW, m_nPrimitiveH;
	int							m_nIndexCount;
	DXGI_FORMAT					m_FormatRT;

	ID3D11VertexShader				*m_pVS;
	ID3D11PixelShader				*m_pPS;
	ID3D11Texture2D					*m_pTextureRT;
	ID3D11Texture2D					*m_pTextureRTCopy;
	ID3D11Texture2D					*m_pTextureRTResolve;
	ID3D11RenderTargetView			*m_pRTView;
	ID3D11RasterizerState			*m_pRSEnabled;
	ID3D11Buffer						*m_pVertexBuffer;
	ID3D11Buffer						*m_pIndexBuffer;
	ID3D11Buffer						*m_pConstBuffers[2];
	ID3D11InputLayout				*m_pLayout;

	tstring						m_szPixelShaderName;
	tstring						m_szVertexShaderName;

private:
	int							m_nPrevSamples;
	tstring						m_szPrevPSName;
	tstring						m_szPrevVSName;

protected:
	// test parameters to be tweaked by subclass
	bool	m_bEnableClipDist;
	bool	m_bEnableCullDist;
	bool	m_bEnableXYClip;
	bool	m_bEnableZClip;
	bool	m_bEnablePipelineStats;
	int		m_nMultisamples;

	TEST_RESULT	UpdateResources();
	HRESULT		DrawQuad(const clipvec4 clipdists[4][2]);
	HRESULT		DrawLine(const float clipdists[4]);
	HRESULT		ReadBackBufferColor(int x, int y, TEST_COLOR* pColor);
	bool		TestColorsSame(const TEST_COLOR& c1, const TEST_COLOR& c2, double delta);
	TEST_RESULT	CheckQuadPattern(const pixelLit_t* lit);


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

class CTriangleClipTest : public CClippingConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	TEST_RESULT	DrawAndCheckTris(const clipvec4 clipdists[4][2], int sampleNum, POINT sampleLoc);

protected:
	clipvec4 m_ClipDists[4][2];
};

class CLineClipTest : public CClippingConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	TEST_RESULT	DrawAndCheckLine(const float clipdists[2]);

protected:
	bool	m_bAntiAliasLine;
	int		m_nClipIdx;
	float	m_fClipDists[4];
};

class CPointClipTest : public CClippingConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
};

class CClippingConfApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	bool InitTestGroups();
	void InitOptionVariables();
};

extern CClippingConfApp  g_App;

// Utility functions
float f_lerp(float t, float t0, float t1);
void vec3_sub(float out[3], float head[3], float tail[3]);
float vec3_dot(float v1[3], float v2[3]);
void vec3_cross(float out[3], float v1[3], float v2[3]);
void vec3_normalize(float v[3]);
void make_plane(float plane[4], float v0[3], float v1[3], float v2[3]);
float plane_intersect(float plane[4], float planept[3], float p0[3], float p1[3]);
float test_point(float plane[4], float planePt[3], float x, float y);
float f_clamp(float in, float epsilon);
int f_sign(double in);
float f_getNaN();
bool isNaN(float in);
bool isINF(float in);
bool test_range(double test, double lower, double upper);
double f_quantize(double in, int bits);
double f_get_parameter(double in, double start, double stop);
double f_interpolate(double param, double start, double stop);
float maxNoNaN(float a, float b);

