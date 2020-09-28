Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_3 : texm3x3 dest r# is NOT allowed";
string PS_032 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_3 : texm3x3 _x2 dest r# is NOT allowed";
string PS_033 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x2 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_3 : texm3x3 _x4 dest r# is NOT allowed";
string PS_034 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x4 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_3 : texm3x3 _d2 dest r# is NOT allowed";
string PS_035 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_d2 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_3 : texm3x3 _sat dest r# is NOT allowed";
string PS_036 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_sat r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_3 : texm3x3 dest c# is NOT allowed";
string PS_037 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_3 : texm3x3 _x2 dest c# is NOT allowed";
string PS_038 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x2 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_3 : texm3x3 _x4 dest c# is NOT allowed";
string PS_039 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x4 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_3 : texm3x3 _d2 dest c# is NOT allowed";
string PS_040 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_d2 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_3 : texm3x3 _sat dest c# is NOT allowed";
string PS_041 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_sat c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_3 : texm3x3 write mask .rgba is allowed";
string PS_042 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.rgba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_3 : texm3x3 write mask .r is NOT allowed";
string PS_043 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.r, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_3 : texm3x3 write mask .g is NOT allowed";
string PS_044 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.g, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_3 : texm3x3 write mask .b is NOT allowed";
string PS_045 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.b, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_3 : texm3x3 write mask .a is NOT allowed";
string PS_046 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.a, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_3 : texm3x3 write mask .rg is NOT allowed";
string PS_047 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.rg, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_1_3 : texm3x3 write mask .gb is NOT allowed";
string PS_048 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.gb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_1_3 : texm3x3 write mask .ba is NOT allowed";
string PS_049 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.ba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_1_3 : texm3x3 write mask .rb is NOT allowed";
string PS_050 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.rb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_3 : texm3x3 write mask .ra is NOT allowed";
string PS_051 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.ra, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_3 : texm3x3 write mask .ga is NOT allowed";
string PS_052 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.ga, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_3 : texm3x3 write mask .rgb is NOT allowed";
string PS_053 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.rgb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_3 : texm3x3 write mask .gba is NOT allowed";
string PS_054 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.gba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_3 : texm3x3 write mask .rba is NOT allowed";
string PS_055 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.rba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_3 : texm3x3 write mask .yx is NOT allowed";
string PS_056 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.yx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_3 : texm3x3 write mask .zx is NOT allowed";
string PS_057 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.zx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_3 : texm3x3 write mask .zy is NOT allowed";
string PS_058 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.zy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_3 : texm3x3 write mask .wx is NOT allowed";
string PS_059 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_1_3 : texm3x3 write mask .wz is NOT allowed";
string PS_060 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wz, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_1_3 : texm3x3 write mask .wy is NOT allowed";
string PS_061 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_1_3 : texm3x3 write mask .zyx is NOT allowed";
string PS_062 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.zyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_1_3 : texm3x3 write mask .wzy is NOT allowed";
string PS_063 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wzy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_1_3 : texm3x3 write mask .wzx is NOT allowed";
string PS_064 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wzx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_1_3 : texm3x3 write mask .wyx is NOT allowed";
string PS_065 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_1_3 : texm3x3 write mask .yxw is NOT allowed";
string PS_066 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.yxw, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_1_3 : texm3x3 write mask .wzyx is NOT allowed";
string PS_067 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.wzyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_1_3 : texm3x3 write mask .zxwy is NOT allowed";
string PS_068 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3.zxwy, t0 "
	"mov r0, c0 ";


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