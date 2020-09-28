//////////////////////////////////////////////////////////////////////////
//  LoadStore.h
//  created:	2008/05/21
//
//  purpose:
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"
#include "FormatHelper.h"
#include <cmath>

namespace LoadStoreTest
{

static const FLOAT EPSILON			= 0.005f;
static const FLOAT DEFAULT_VALUE	= -1.0f;
static const UINT DISP_WIDTH		= 20;
static const UINT DISP_HEIGHT		= 20;
static const UINT MAX_WIDTH			= 32;
static const UINT MAX_HEIGHT		= 32;
static const UINT MAX_DEPTH			= 32;
static const UINT MAX_ARRAY_SIZE    = 7;
static const UINT MAX_MIP_COUNT     = 7;
static const UINT MAX_NUM_VERTICES	= MAX_WIDTH * MAX_HEIGHT;
static const UINT NUM_VERTICES		= 4;
static const UINT NUM_FAILURES_TO_LOG = 1;

//--
enum RESOURCE_TYPE
{
	RESOURCE_BUFFER,
	RESOURCE_TEXTURE1D,
	RESOURCE_TEXTURE1D_ARRAY,
	RESOURCE_TEXTURE2D,
	RESOURCE_TEXTURE2D_ARRAY,
	RESOURCE_TEXTURE3D,
};

//--
enum BUFFER_TYPE
{
	BUFFER_TYPED,
	BUFFER_RAW,
	BUFFER_STRUCTURED
};

//--
enum FORMAT_TYPE
{
	FORMAT_TYPELESS,
	FORMAT_UINT,
	FORMAT_SINT,
	FORMAT_FLOAT,
	FORMAT_UNORM,
	FORMAT_SNORM,
};

//--
enum ADDRESS_OFFSET
{
	// Offsets from the begining of the addressed space
	ADDRESS_OFFSET_START,
	ADDRESS_OFFSET_START_PLUS_4_BYTES,
	ADDRESS_OFFSET_START_PLUS_8_BYTES,
	ADDRESS_OFFSET_START_PLUS_12_BYTES,
	ADDRESS_OFFSET_START_PLUS_16_BYTES,
	ADDRESS_OFFSET_START_PLUS_20_BYTES,

	// Offsets from the end of the addressed space
	ADDRESS_OFFSET_END_MINUS_4_BYTES,
	ADDRESS_OFFSET_END_MINUS_8_BYTES,
	ADDRESS_OFFSET_END_MINUS_12_BYTES,
	ADDRESS_OFFSET_END_MINUS_16_BYTES,
	ADDRESS_OFFSET_END_MINUS_20_BYTES
};

//--
struct VSINPUT
{
	float Position[4];
	float Address[4];
	float Offset[4];
};

//--
struct CSINPUT
{
	float Address[4];
	float Offset[4];
};

//--
class CLoadStoreTest : public CShader5xTest
{
public:

	CLoadStoreTest( bool bIsLoadInstruction, BUFFER_TYPE bufType );

	TEST_RESULT Setup();
	void Cleanup();

	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

protected:

	bool InitVSInput();
	bool InitCSInput();
	bool InitGradients();
	bool InitBuffer();
	bool InitTexture1D();
	bool InitTexture2D();
	bool InitTexture3D();
	bool CompareResultWithFormatULP( const float &fExpected, const float &fResult, const DXGI_FORMAT &resFormat, const UINT &componentIndex, float &fUlpDiff );

	virtual bool ResidencyFeedback() const
	{
		return false;
	}

	bool SkipTiled() const
	{
		return ResidencyFeedback() && !TiledOpsSupported();
	}

	//--
	UINT			    m_testcaseID;
	BOOL				m_bIsLoadInstruction;
	BOOL				m_bRenderUsingPointlist;
	BUFFER_TYPE			m_bufType;

	// Testcase parameters
	RESOURCE_TYPE				m_resourceType;
	UINT						m_bufStride;
	DXGI_FORMAT					m_resFormat;
	UINT						m_resWidth;
	UINT						m_resHeight;
	UINT						m_resDepth;
	UINT						m_resMipLevels;
	UINT						m_resMipSlice;
	UINT						m_resArraySize;
	UINT						m_resArraySlice;
	AssemblyHelper::MaskFlag	m_destMask;
	AssemblyHelper::Swizzle		m_srcAddressSwizzle;
	AssemblyHelper::Swizzle		m_srcSwizzle;
	AssemblyHelper::Swizzle		m_srcOffsetSwizzle;
	ADDRESS_OFFSET				m_srcOffset;
	UINT						m_UAVSlot;

	// Used in structured and raw buffer cases. We need to know about this during setup because it 
	// impacts how we iterate through dimensions. Out of bounds addressing on TGSM (the bounds of that 
	// particular TGSM, as opposed to all shared memory) for any given 32 - bit component 
	// causes the entire contents of all shared memory to become undefined.
	bool						m_bUseTGSM;	
	float						m_outOfBoundsOffset;

	// Resources
	ID3D11Buffer*		m_pBuffer;
	ID3D11Buffer*		m_pBufferCopy;

	ID3D11Texture1D*	m_pTexture1D;
	ID3D11Texture1D*	m_pTexture1DCopy;

	ID3D11Texture2D*	m_pTexture2D;
	ID3D11Texture2D*	m_pTexture2DCopy;

	ID3D11Texture3D*	m_pTexture3D;
	ID3D11Texture3D*	m_pTexture3DCopy;

	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pVsInput;
	ID3D11Buffer*		m_pCsInput;
	ID3D11Buffer*		m_pCsInputCopy;
	ID3D11ShaderResourceView* m_pCsInputSRV;
	ID3D11SamplerState*	m_pSampler;
	BYTE*				m_pbGradients;
	ID3D11UnorderedAccessView* m_pUAV;
};

//--
class CLd_uav_typed : public CLoadStoreTest
{
public:

	CLd_uav_typed() : CLoadStoreTest( true, BUFFER_TYPED ) {}

	void InitTestParameters();

	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();

	TEST_RESULT ExecuteTestCase();
private:

	bool InitPipelineShaders();
	bool InitCSShaders();
	bool AnalyzeResult();
};

//-----------------------------------------------------------------------------
class CLd_uav_typed_s : public CLd_uav_typed
{
public:
	bool ResidencyFeedback() const
	{
		return true;
	}
};

//-----------------------------------------------------------------------------
class CStore_uav_typed : public CLoadStoreTest
{
public:

	CStore_uav_typed() : CLoadStoreTest( false, BUFFER_TYPED ), m_pCB(NULL)  {}

	void InitTestParameters();

	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();

	TEST_RESULT ExecuteTestCase();

	void CleanupTestCase();

private:

	bool InitCB();
	bool InitPipelineShaders();
	bool InitCSShaders();
	bool AnalyzeResult();

private:
	ID3D11Buffer *m_pCB;
};

//-----------------------------------------------------------------------------
class CStore_raw : public CLoadStoreTest
{
public:

	CStore_raw() : CLoadStoreTest( false, BUFFER_RAW ){}

	void InitTestParameters();

	TEST_RESULT ExecuteTestCase();

private:

	bool InitPipelineShaders();
	bool InitCSShaders();
	bool AnalyzeResult();
};

//-----------------------------------------------------------------------------
class CStore_structured : public CLoadStoreTest
{
public:

	CStore_structured() : CLoadStoreTest( false, BUFFER_STRUCTURED ){}

	void InitTestParameters();

	TEST_RESULT ExecuteTestCase();

private:

	UINT GetSrcStructureOffset();
	bool InitPipelineShaders();
	bool InitCSShaders();
	bool AnalyzeResult();
};

} // namespace LoadStoreTest