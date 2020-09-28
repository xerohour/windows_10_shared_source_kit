#pragma once

#include "WGFTestCore.h"
#include "PresentHelper.h"
#include <string>

const int numPointListData = 3;
const float pointListData[numPointListData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.7f, 0.7f, 0.1f, 1.0f}
};

const int numLineStripData = 4;
const float lineStripData[numLineStripData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.7f, 0.7f, 0.1f, 1.0f},
	{0.6f, 0.3f, 0.0f, 1.0f}
};

const int numLineStripAdjData = 6;
const float lineStripAdjData[numLineStripAdjData][4] =
{
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.7f, 0.7f, 0.1f, 1.0f},
	{0.6f, 0.3f, 0.0f, 1.0f},
	{0.8f, 0.8f, 0.0f, 1.0f}
};

const int numLineListData = 6;
const float lineListData[numLineListData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.45f, 0.45f, 0.3f, 1.0f},
	{0.7f, 0.7f, 0.1f, 1.0f},
	{0.75f, 0.75f, 0.1f, 1.0f},
	{0.6f, 0.3f, 0.0f, 1.0f}
};

const int numLineListAdjData = 12;
const float lineListAdjData[numLineListAdjData][4] =
{
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.45f, 0.45f, 0.3f, 1.0f},

	{0.4f, 0.4f, 0.3f, 1.0f},
	{0.45f, 0.45f, 0.3f, 1.0f},
	{0.7f, 0.7f, 0.1f, 1.0f},
	{0.75f, 0.75f, 0.1f, 1.0f},

	{0.7f, 0.7f, 0.1f, 1.0f},
	{0.75f, 0.75f, 0.1f, 1.0f},
	{0.6f, 0.3f, 0.0f, 1.0f},
	{0.8f, 0.8f, 0.0f, 1.0f}
};

const int numTriangleStripData = 5;
const float triangleStripDataCW[numTriangleStripData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.9f, 0.1f, 0.0f, 1.0f}
};

const float triangleStripDataCCW[numTriangleStripData][4] =
{
	{0.9f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
};

const int numTriangleStripAdjData = 10;
const float triangleStripAdjDataCW[numTriangleStripAdjData][4] =
{
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.3f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.3f, 0.0f, 1.0f},
	{0.5f, 0.7f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.7f, 0.1f, 0.0f, 1.0f},
	{0.9f, 0.3f, 0.0f, 1.0f},
	{1.0f, 0.5f, 0.0f, 1.0f},
};

const float triangleStripAdjDataCCW[numTriangleStripAdjData][4] =
{
	{0.9f, 0.3f, 0.0f, 1.0f},
	{1.0f, 0.5f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.7f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.3f, 0.0f, 1.0f},
	{0.5f, 0.7f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.3f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
};

const int numTriangleListData = 9;
const float triangleListDataCW[numTriangleListData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.7f, 0.9f, 0.0f, 1.0f},
	{0.85f, 0.55f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.9f, 0.1f, 0.0f, 1.0f}
};

const float triangleListDataCCW[numTriangleListData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.85f, 0.55f, 0.0f, 1.0f},
	{0.7f, 0.9f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.9f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f}
};

const int numTriangleListAdjData = 18;
const float triangleListAdjDataCW[numTriangleListAdjData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.0f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.35f, 0.9f, 0.0f, 1.0f},
	{0.7f, 0.9f, 0.0f, 1.0f},
	{0.85f, 0.9f, 0.0f, 1.0f},
	{0.85f, 0.55f, 0.0f, 1.0f},
	{0.7f, 0.45f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.55f, 0.5f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.9f, 0.5f, 0.0f, 1.0f},
	{0.9f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.0f, 0.0f, 1.0f}
};

const float triangleListAdjDataCCW[numTriangleListAdjData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.7f, 0.45f, 0.0f, 1.0f},
	{0.85f, 0.55f, 0.0f, 1.0f},
	{0.85f, 0.9f, 0.0f, 1.0f},
	{0.7f, 0.9f, 0.0f, 1.0f},
	{0.35f, 0.9f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.0f, 0.0f, 1.0f},
	{0.9f, 0.1f, 0.0f, 1.0f},
	{0.9f, 0.5f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.55f, 0.5f, 0.0f, 1.0f}
};

enum DrawMethod
{
	Draw,
	DrawInstanced,
	DrawIndexed,
	DrawIndexedInstanced,
	DrawAuto
};


DECLARE_NAMED_VALUES( DrawMethod )

enum IsFrontFaceInStruct
{
	Start,
	Middle,
	End,
	Only
};

struct VERTEXPOS
{
	float x, y, z, w;
};

class ID3D11PixelShaderApp;
extern ID3D11PixelShaderApp g_App;	

class ID3D11PixelShaderTest : public CD3D11Test
{
public:
	ID3D11PixelShaderTest();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual void InitTestParameters();
	virtual int GetNumRenderWindows() const { return GetFramework()->GetShell()->IsConsole() ? 0 : 1; }

	static std::string GetShaderTypeFromFormat(DXGI_FORMAT format);
	static UINT GetNumComponentsForFormat(DXGI_FORMAT format);
	static UINT GetFormatStrideInBytes(DXGI_FORMAT format);
	static UINT GetVerticesPerPrimitive(D3D11_PRIMITIVE_TOPOLOGY topology);
	static UINT GetPrimitiveCount(UINT numVertices, D3D11_PRIMITIVE_TOPOLOGY topology);
	void ShuffleElements(UINT numElements, UINT *pArray);
	void GenerateData(UINT numVertices, D3D11_PRIMITIVE_TOPOLOGY topology, VERTEXPOS *pVertex);
	TEST_RESULT SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_INPUT_ELEMENT_DESC *iedesc, UINT numInputElements, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout );
	TEST_RESULT SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11GeometryShader **ppGS );
	TEST_RESULT SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS );
protected:
	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pDeviceContext;
	ID3D11Debug				*m_pDebug;
	ID3D11InfoQueue			*m_pInfoQueue;

	ID3D11Texture2D			*m_pRTTexture2D;
	ID3D11Texture2D			*m_pRTTexture2DCopy;
	ID3D11RenderTargetView	*m_pRTView;

	ID3D11DepthStencilState	*m_pDSState;
	ID3D11Buffer				*m_pVertexBuffer;
	ID3D11VertexShader		*m_pPassThroughVS;
	ID3D11InputLayout		*m_pInputLayout;

	ID3D11ShaderResourceView *m_pSRV;
	CPresentHelper		m_PresentHelper;

	bool CanPresent();

	//CInfoQueueHelper	 m_InfoQueueHelper;
};

class CIsFrontFaceTest : public ID3D11PixelShaderTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	BOOL						m_FrontCounterClockwise;
	D3D11_CULL_MODE				m_CullMode;
	D3D11_FILL_MODE				m_FillMode;
	D3D11_PRIMITIVE_TOPOLOGY		m_PrimitiveTopology;
	bool						m_ClockwiseData;
	bool						m_ZeroArea;

	IsFrontFaceInStruct			m_IsFrontFaceInStruct;

	UINT GetExpectedFrontFace();
};

class CGeneratedValuesTest : public ID3D11PixelShaderTest
{
public:
	CGeneratedValuesTest();

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	D3D11_PRIMITIVE_TOPOLOGY		m_PrimitiveTopology;
	D3D11_FILL_MODE				m_FillMode;
	bool						m_bIsFrontFace;
	bool						m_bNoExpected;
	UINT						m_VerticesCount;

	ID3D11InputLayout				*m_pInputLayout;
	ID3D11VertexShader				*m_pVS;
	ID3D11VertexShader				*m_pVSNoExpected;
	ID3D11PixelShader				*m_pPS;
	ID3D11PixelShader				*m_pPSWithFrontFace;
	ID3D11PixelShader				*m_pPSNoExpected;
	ID3D11Buffer						*m_pVBPos;
	ID3D11Buffer						*m_pVBExpected;

};

class CPipelineStatsTest : public ID3D11PixelShaderTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	bool						m_NullPS;
	bool						m_NoInputPS;
	bool						m_DepthEnable;
	bool						m_StencilEnable;
};

enum UAV_TYPE
{
	UAV_RAW,
	UAV_TYPED,
	UAV_STRUCTURED,
	UAV_APPEND,
	UAV_COUNTER,
};

enum UAV_ACCESS_METHOD
{
	UAV_ACCESS_STORE,
	UAV_ACCESS_ATOMIC_AND,
	UAV_ACCESS_ATOMIC_XOR,
	UAV_ACCESS_ATOMIC_OR,
	UAV_ACCESS_ATOMIC_CMP_STORE,
	UAV_ACCESS_ATOMIC_EXCHANGE,
	UAV_ACCESS_ATOMIC_CMP_EXCHANGE,
	UAV_ACCESS_ATOMIC_ADD,
	UAV_ACCESS_ATOMIC_IMAX,
	UAV_ACCESS_ATOMIC_IMIN,
	UAV_ACCESS_ATOMIC_UMAX,
	UAV_ACCESS_ATOMIC_UMIN,
	UAV_ACCESS_IMM_ATOMIC_AND,
	UAV_ACCESS_IMM_ATOMIC_XOR,
	UAV_ACCESS_IMM_ATOMIC_OR,
	UAV_ACCESS_IMM_ATOMIC_ADD,
	UAV_ACCESS_IMM_ATOMIC_IMAX,
	UAV_ACCESS_IMM_ATOMIC_IMIN,
	UAV_ACCESS_IMM_ATOMIC_UMAX,
	UAV_ACCESS_IMM_ATOMIC_UMIN,
};

class CPixelShaderUAVWrite : public CD3D11Test
{
public:
	CPixelShaderUAVWrite();

	virtual void InitTestParameters();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();
	virtual TEST_RESULT ExecuteTestCase();

private:
	TEST_RESULT CheckResults();

	TEST_RESULT CheckResults_RenderTarget();
	TEST_RESULT CheckResults_ZBuffer();
	TEST_RESULT CheckResults_UAV();
	TEST_RESULT CheckResults_Query();
	float GetDepthBufferValue(UINT Sample);

	// Test Parameters
	bool                  m_ShaderEarlyDepth;   // true if the ForceEarlyDepthStencil flag is set
	bool                  m_WriteODepth;        // true if the shader writes to ODepth
	D3D11_COMPARISON_FUNC m_DepthFunc; 
	UINT                  m_DepthTestResult;    // Bit 'i' is set if sample 'i' should pass the depth test
	UINT                  m_SampleCount;
	bool                  m_DiscardBeforeWrite; // true if the pixel shader should discard results before the UAV write
	bool                  m_DiscardAfterWrite;  // true if the pixel shader should discard results after the UAV write
	UAV_TYPE              m_UAVType;
	UAV_ACCESS_METHOD     m_UAVAccessMethod;
	D3D11_UAV_DIMENSION   m_UAVDimension;

	// Other members

	// Created once
	typedef std::map< D3D11_COMPARISON_FUNC, ID3D11DepthStencilState* >   DEPTH_STENCIL_MAP;
	typedef std::map< UINT, ID3D11Texture2D* >                            TEXTURE2D_MAP;
	typedef std::map< UINT, ID3D11RenderTargetView* >                     RENDER_TARGET_VIEW_MAP;
	typedef std::map< UINT, ID3D11DepthStencilView* >                     DEPTH_STENCIL_VIEW_MAP;

	DEPTH_STENCIL_MAP           m_DepthStencilStateMap;
	TEXTURE2D_MAP               m_RenderTargetMap;
	RENDER_TARGET_VIEW_MAP      m_RenderTargetViewMap;
	TEXTURE2D_MAP               m_DepthStencilMap;
	DEPTH_STENCIL_VIEW_MAP      m_DepthStencilViewMap;

	ID3D11VertexShader*         m_pVertexShader;
	ID3D11InputLayout*          m_pInputLayout;
	ID3D11Buffer*               m_pVB;
	ID3D11Query*                m_pQuery;

	// Created per test case
	ID3D11PixelShader*          m_pPixelShader;
	ID3D11Buffer*               m_pUAVBuffer;
	ID3D11Texture1D*            m_pUAVTexture1D;
	ID3D11Texture2D*            m_pUAVTexture2D;
	ID3D11Texture3D*            m_pUAVTexture3D;
	ID3D11UnorderedAccessView*  m_pUAV;
	
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class ID3D11PixelShaderApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	bool InitTestGroups();
	void InitOptionVariables();

	bool	m_bPresent = false;
};
