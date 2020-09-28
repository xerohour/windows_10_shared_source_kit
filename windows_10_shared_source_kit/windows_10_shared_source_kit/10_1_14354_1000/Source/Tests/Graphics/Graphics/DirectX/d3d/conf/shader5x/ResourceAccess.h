//////////////////////////////////////////////////////////////////////////
//  ResourceAccess.h
//  created:	2005/03/10
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

#include "ShaderInstruction.h"

// DXGI_FORMAT_R32G32B32A32_FLOAT is optional for FL10 (sample instructions). DXGI_FORMAT_R16G16B16A16_FLOAT is required.
#define	RES_ACCESS_FORMAT_FL10_1_AND_GREATER DXGI_FORMAT_R32G32B32A32_FLOAT
#define	RES_ACCESS_FORMAT_FL10 DXGI_FORMAT_R16G16B16A16_FLOAT
#define	RES_RTFORMAT DXGI_FORMAT_R32G32B32A32_FLOAT

#define RES_ACCESS_ARRAY_SIZE		3
#define RES_ACCESS_MIP_LEVELS		6
#define	COMP_RTFORMAT				DXGI_FORMAT_R32_FLOAT
#define COMP_REDSTART				0
#define COMP_REDBYTES				4
#define	COMP_REFERENCEVAL			0.0f

#define	FP_INF_POS			0x7f800000
#define FP_INF_NEG			0xff800000

#define DEFAULT_WIDTH		32
#define DEFAULT_HEIGHT		32
#define DEFAULT_DEPTH		32
#define MAX_LOD				5

#define LOD_PRECISION_EPSILON 0.05f

#define MAX_RESOURCE_SIZE	DEFAULT_WIDTH * DEFAULT_HEIGHT * 16
#define MAX_MIP_COUNT       16

enum FLOAT_RESULT
{
	FLOAT_FAIL = 0,
	FLOAT_OK,
	FLOAT_INF_POS,
	FLOAT_INF_NEG,
	FLOAT_NAN,
	FLOAT_DENORM
};

enum RESOURCE_TYPE
{
	RESOURCE_BUFFER,
	RESOURCE_TEXTURE1D,
	RESOURCE_TEXTURE1D_ARRAY,
	RESOURCE_TEXTURE2D,
	RESOURCE_TEXTURE2D_ARRAY,
	RESOURCE_TEXTURE3D,
	RESOURCE_TEXTURECUBE,
	RESOURCE_TEXTURECUBE_ARRAY
};

enum BUFFER_TYPE
{
	BUFFER_TYPED,
	BUFFER_RAW,
	BUFFER_STRUCTURED
};

union int3
{
	int i[ 3 ];
	struct
	{
		int x, y, z;
	};

	bool operator ==( const int3 rhs ) const
	{
		return
			x == rhs.x &&
			y == rhs.y &&
			z == rhs.z;
	}
};

extern tstring __stdcall ToString( RESOURCE_TYPE type );
extern bool __stdcall FromString( RESOURCE_TYPE *pType, const tstring &str );

class CShader5xTest_ResourceAccess : public CShader5xTest
{
public:
	CShader5xTest_ResourceAccess(const TCHAR*, bool bTestResourceMinLOD = false);

	virtual void			InitTestParameters();
	virtual TEST_RESULT 		Setup();
	virtual void			Cleanup();
	virtual TEST_RESULT		SetupTestCase();
	virtual void			CleanupTestCase();
	virtual TEST_RESULT 		ExecuteTestCase();


	void				GenerateParameterFactorValues();
	TEST_RESULT 			GenerateCurrentWidth();
	TEST_RESULT 			GenerateCurrentHeight();
	TEST_RESULT 			GenerateCurrentDepth();
	TEST_RESULT 			GenerateCurrentMipCount();
	TEST_RESULT 			GenerateCurrentMipLevel();
	TEST_RESULT 			GenerateArraySize();
	bool				ValidateParameters( );

	virtual TEST_RESULT		CreateResource();
	virtual TEST_RESULT 		InitResource();
	virtual TEST_RESULT 		InitBufferRes( bool bSetData );
	virtual TEST_RESULT 		InitTexture1D( bool bSetData, bool bRepeatValues );
	virtual TEST_RESULT 		InitTexture2D( bool bSetData, bool bRepeatValues );
	virtual TEST_RESULT 		InitTexture3D( bool bSetData, bool bRepeatValues );
	virtual TEST_RESULT		SetupInputBuffer();
	virtual bool 			CreateShaders();
	virtual TEST_RESULT		CreateResultRTVorUAV();
	virtual tstring			GetShaderCode(AssemblyHelper* pAsmHelper);
	virtual bool 			CheckResult( void *pResult, UINT uRowPitch );
	virtual void 			GetOffsetCoords( int *pX, int *pY, int *pZ, int mipLevel, UINT numBits, int offsetSwizzleIndices[4] );

	virtual bool 			WithinOneULP( float fReference, float fOutput );
	virtual bool 			GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight );
	virtual bool			IsDenorm( float fValue );
	virtual bool			IsNaN( float fValue );

	virtual bool			CreateStripes(	DXGI_FORMAT resFormat,
											const UINT &uHeight, const UINT &uWidth,
											const UINT &uNumStripes,
											FLOAT_COLOR *pColors, const UINT &uNumColors,
											void *pStripes, bool bVertical );
	virtual bool			CreateGradient(	DXGI_FORMAT resFormat,
											const UINT &uHeight, const UINT &uWidth,
											FLOAT_COLOR pColors[ 4 ], void *pGradient );
	virtual bool			CreateTiles(	DXGI_FORMAT resFormat,
											const UINT &uHeight, const UINT &uWidth,
											const UINT &uEdgeWidth,
											FLOAT_COLOR *pColors, const UINT &uNumColors,
											void *pTiles );

	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }

protected:
	DXGI_FORMAT				m_Format;

	RESOURCE_TYPE				m_Resource;
	BUFFER_TYPE				m_bufType;
	bool					m_bIsBound;
	bool					m_bIsUAV;
	bool					m_bTestOffset;
	bool                    		m_bTestResourceMinLOD;
	bool					m_bUsingFL10Format;
	int3					m_nOffsets;
	UINT					m_Width;
	UINT					m_Height;
	UINT					m_Depth;
	UINT                    		m_BufferStructStride;
	UINT32					m_MipCount;
	UINT32					m_ArraySize;
	DXGI_SAMPLE_DESC			m_SampleDesc;
	D3D11_USAGE				m_Usage;
	D3D11_COMPARISON_FUNC			m_ComparisonFunc;
	float					m_fReferenceValue;
	D3D11_SAMPLER_DESC			m_D3DSamplerDesc;

	int					m_MipLevel;
	int					m_ArraySlice;
	float					m_fMipLODBias;
	float					m_fInstBias;
	int					m_nSamplerComponent;
	float                   		m_fResourceMinLODOffset;
	UINT					m_numComponentsRawOutput;

	AssemblyHelper::MaskFlag  m_DestMask;
	AssemblyHelper::Swizzle   m_ResourceSwizzle;
	AssemblyHelper::Swizzle   m_AddressSwizzle;
	AssemblyHelper::Swizzle   m_OffsetSwizzle;

	UINT32					m_BindFlags;
	UINT32					m_CPUAccessFlags;
	UINT32					m_MiscFlags;

	ID3D11Buffer				*m_pBufferRes;
	ID3D11Texture1D				*m_pTexture1D;
	ID3D11Texture2D				*m_pTexture2D;
	ID3D11Texture3D				*m_pTexture3D;
	ID3D11Buffer				*m_pBufferTL;
	ID3D11InputLayout			*m_pLayoutTL;
	ID3D11SamplerState			*m_pSampler;

	ID3D11ShaderResourceView 	*m_pSRV;
	ID3D11UnorderedAccessView	*m_pUAV;

	float					m_matWorld[4][4];

	void					*m_pExpectedResult;
	BYTE					m_pGradient[ MAX_RESOURCE_SIZE ][ MAX_MIP_COUNT ];
	float					m_fBorderColor[ 4 ];

	struct TLVERTEX
	{
		float  position[4];
		float  texcoord[4];
		float  positionTL[2];
	};
	void TransformVertices(TLVERTEX* vTL, UINT numVerts);
	bool CompareValues( D3D11_COMPARISON_FUNC func, float refVal, float fTexture, float fOutputResult, bool bLog );
	void SetResourceMinLOD( ID3D11Resource* pResource );
	virtual bool TestingPerResourceMinLOD();
	bool DecreasePrimitiveSize();
	const ShaderInstruction::Attributes InstAttributes;
	bool SkipTiled() const
	{
		return (InstAttributes.ModifiersMatch(Modifier::ResidencyFeedback) || InstAttributes.ModifiersMatch(Modifier::Clamp)) && !TiledOpsSupported();
	}
};

#define Ignore(x) bool Use##x##() const{ return false; }

//-----------------------------------------------------------------------------

class CShader5xTest_resinfo : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_resinfo() : CShader5xTest_ResourceAccess(_T("resinfo")){}
	void		InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT	SetupTestCase();

	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
	bool CheckResult( void *pResult, UINT uRowPitch );
	bool CheckUintResult( void *pResult, UINT uRowPitch );
	bool CheckFloatResult( void *pResult, UINT uRowPitch );

private:
	UINT	m_uSwizzle;
	UINT	m_uSuffix;
	UINT	m_uLevelSelector;
	UINT	m_uMask;
};

//-----------------------------------------------------------------------------

class CShader5xTest_bufinfo : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_bufinfo() : CShader5xTest_ResourceAccess(_T("bufinfo")){}
	void		InitTestParameters();
	TEST_RESULT	SetupTestCase();
	TEST_RESULT Setup( );

	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
	bool CheckResult( void *pResult, UINT uRowPitch );

private:
	AssemblyHelper::MaskFlag	m_uMask;
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_ld_base(const TCHAR* Instruction) : CShader5xTest_ResourceAccess( Instruction, true ) {}
	TEST_RESULT	InitResource();
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld : public CShader5xTest_ld_base
{
public:
	CShader5xTest_ld() : CShader5xTest_ld_base( _T("ld") ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld_s : public CShader5xTest_ld_base
{
public:
	CShader5xTest_ld_s() : CShader5xTest_ld_base(_T("ld_s")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ldRawOrStruct : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_ldRawOrStruct(const TCHAR* Instruction) : CShader5xTest_ResourceAccess(Instruction){}
	TEST_RESULT	SetupTestCase();
	bool CheckResult( void * pResult, UINT uRowPitch );
	AssemblyHelper::MaskFlag	m_uOffsetSelectComp;
	AssemblyHelper::MaskFlag	m_uAddressSelectComp;

};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw_base : public CShader5xTest_ldRawOrStruct
{
public:
	CShader5xTest_ld_raw_base(const TCHAR* Instruction) : CShader5xTest_ldRawOrStruct(Instruction){}
	void InitTestParameters();
	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
};

// TODO: ld_raw and ld_structured can also be used on thread group shared memory, but this will be tested by WGF11Compute.
//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw : public CShader5xTest_ld_raw_base
{
public:
	CShader5xTest_ld_raw() : CShader5xTest_ld_raw_base(_T("ld_raw")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_raw_s : public CShader5xTest_ld_raw_base
{
public:
	CShader5xTest_ld_raw_s() : CShader5xTest_ld_raw_base(_T("ld_raw_s")){}
};

class CShader5xTest_ld_structured_base : public CShader5xTest_ldRawOrStruct
{
public:
	CShader5xTest_ld_structured_base(const TCHAR* Instruction) : CShader5xTest_ldRawOrStruct(Instruction){}
	void		InitTestParameters();
	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_structured : public CShader5xTest_ld_structured_base
{
public:
	CShader5xTest_ld_structured() : CShader5xTest_ld_structured_base(_T("ld_structured")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_ld_structured_s : public CShader5xTest_ld_structured_base
{
public:
	CShader5xTest_ld_structured_s() : CShader5xTest_ld_structured_base(_T("ld_structured_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample() : CShader5xTest_ResourceAccess(_T("sample")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_s : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_s() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_cl : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_cl() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_cl_s : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_cl_s() : CShader5xTest_ResourceAccess(_T("sample_cl_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_min_lod : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_min_lod() : CShader5xTest_ResourceAccess( _T("sample"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_d_base(const TCHAR* Instruction, bool bTestResourceMinLOD) : CShader5xTest_ResourceAccess(Instruction, bTestResourceMinLOD){}
	TEST_RESULT	InitResource();
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d() : CShader5xTest_sample_d_base( _T("sample_d"), true ) {}
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_s : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_s() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_cl : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_cl() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_d_cl_s : public CShader5xTest_sample_d_base
{
public:
	CShader5xTest_sample_d_cl_s() : CShader5xTest_sample_d_base( _T("sample_d_cl_s"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_l_base(const TCHAR* Instruction, bool bTestResourceMinLOD) : CShader5xTest_ResourceAccess( Instruction, bTestResourceMinLOD ) {}
	TEST_RESULT	InitResource();
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l : public CShader5xTest_sample_l_base
{
public:
	CShader5xTest_sample_l() : CShader5xTest_sample_l_base( _T("sample_l"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_l_s : public CShader5xTest_sample_l_base
{
public:
	CShader5xTest_sample_l_s() : CShader5xTest_sample_l_base( _T("sample_l_s"), true ) {}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_Comparison : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_Comparison( const TCHAR* Instruction, bool bTestPerResourceMinLOD = false );

	void	InitTestParameters( );
	bool	CheckResult( void *pResult, UINT uRowPitch );
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c() : CShader5xTest_Comparison(_T("sample_c")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_s() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_cl : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_cl() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
	Ignore(ResidencyFeedback);
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_cl_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_cl_s() : CShader5xTest_Comparison(_T("sample_c_cl_s")){}
};

//-----------------------------------------------------------------------------
class CShader5xTest_sample_c_min_lod : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_min_lod() : CShader5xTest_Comparison( _T("sample_c"), true ) {}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz() : CShader5xTest_Comparison(_T("sample_c_lz")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz_s : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz_s() : CShader5xTest_Comparison(_T("sample_c_lz_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_c_lz_min_lod : public CShader5xTest_Comparison
{
public:
	CShader5xTest_sample_c_lz_min_lod() : CShader5xTest_Comparison( _T("sample_c_lz"), true ) {}
	TEST_RESULT	InitResource();

protected:
	virtual bool TestingPerResourceMinLOD();
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_sample_b_base(const TCHAR* Instruction) : CShader5xTest_ResourceAccess(Instruction)
	{
		m_ShaderStage = D3D_SHADER_STAGE_PIXEL;
		m_Resource = RESOURCE_TEXTURE2D_ARRAY;
		m_bIsBound = true;
	}

	void		InitTestParameters( );
	TEST_RESULT InitResource( );
	virtual bool UseClamp() const { return true; }
	virtual bool UseResidencyFeedback() const { return true; }
	bool		CheckResult( void *pResult, UINT uRowPitch );

protected:
	float		*m_pResourceData;
	float        	m_fMinLOD;
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b() : CShader5xTest_sample_b_base(_T("sample_b")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_s : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_s() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
	Ignore(Clamp);
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_cl : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_cl() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
	Ignore(ResidencyFeedback)
};

//-----------------------------------------------------------------------------

class CShader5xTest_sample_b_cl_s : public CShader5xTest_sample_b_base
{
public:
	CShader5xTest_sample_b_cl_s() : CShader5xTest_sample_b_base(_T("sample_b_cl_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_gather4_base(const TCHAR* Instruction);

	void			InitTestParameters( );
	bool			CheckResult( void *pResult, UINT uRowPitch );
	TEST_RESULT		Setup( );
	TEST_RESULT		SetupTestCase( );

	virtual TEST_RESULT InitResource();

protected:
	void			GetQuad( UINT x, UINT y, UINT elementStride, UINT uRowPitch, float *pQuad );
	float           m_fResourceMinLOD;
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4 : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4() : CShader5xTest_gather4_base(_T("gather4")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_s() : CShader5xTest_gather4_base(_T("gather4_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po() : CShader5xTest_gather4_base(_T("gather4_po")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_s() : CShader5xTest_gather4_base(_T("gather4_po_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_c : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_c() : CShader5xTest_gather4_base(_T("gather4_c")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_c_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_c_s() : CShader5xTest_gather4_base(_T("gather4_c_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_c : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_c() : CShader5xTest_gather4_base(_T("gather4_po_c")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_gather4_po_c_s : public CShader5xTest_gather4_base
{
public:
	CShader5xTest_gather4_po_c_s() : CShader5xTest_gather4_base(_T("gather4_po_c_s")){}
};

//-----------------------------------------------------------------------------

class CShader5xTest_lod : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_lod();

	void			InitTestParameters( );
	bool			CheckResult( void *pResult, UINT uRowPitch );
	TEST_RESULT		SetupInputBuffer( );
	TEST_RESULT		Setup( );

protected:
	float			m_fScale;
};

#undef Ignore