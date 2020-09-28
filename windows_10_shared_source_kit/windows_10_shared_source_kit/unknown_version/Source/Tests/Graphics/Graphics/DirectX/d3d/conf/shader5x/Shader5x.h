//////////////////////////////////////////////////////////////////////////
//  Shader5xTest.h
//  created:	2005/03/10
//
//  purpose:
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "d3d11internaltest.h"
#include "AsmHelper.h"
#include "MinMaxRange.h"

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

/////////////////////////////////////////////////
// CShader5xTestApp
/////////////////////////////////////////////////

class CShader5xTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

	void InitOptionVariables();

	bool InitTestGroups();

public:
	bool Is11_1DDIOrGreater() const { return m_bIs11_1DDISupported; }
	bool IsDebugRuntime() const { return m_D3DOptions.Debug; }
	D3D_FEATURE_LEVEL GetFeatureLevel() const { return m_featureLevel; }
	D3D_DRIVER_TYPE GetSrcDriverType() const { return m_D3DOptions.DriverType; }
	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS GetD3D10XHWOptions() const { return m_hwOptions; }
	bool IsFL10AndHWSupportsComputeOn10() const { return m_bFL10AndHWSupportsComputeOn10; }
	bool IsFL11OrGreater() const { return m_bFL11OrGreater; }
	bool IsFL11_1OrGreater() const { return m_bFL11_1OrGreater; }
	bool UAVIsAllowed() const { return (IsFL10AndHWSupportsComputeOn10() || IsFL11OrGreater()); }
	bool DoublesAreSupported() const { return m_bHWSupportsDoubles; }
	bool IsD3D11On12() const { return m_bIsD3D11On12; }

private:
	bool m_bIs11_1DDISupported;
	D3D_FEATURE_LEVEL m_featureLevel;
	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS m_hwOptions;
	D3D11_FEATURE_DATA_DOUBLES m_hwDoubles;
	bool m_bHWSupportsComputeOn10;
	bool m_bFL10AndHWSupportsComputeOn10;
	bool m_bFL11OrGreater;
	bool m_bFL11_1OrGreater;
	bool m_bHWSupportsDoubles;
	bool m_bIsD3D11On12;
};

extern CShader5xTestApp g_TestApp;

/////////////////////////////////////////////////
// CShader5x
/////////////////////////////////////////////////

#define CLEAR_VALUE 0x81818181
#define str_CLEAR_VALUE "0x81818181"

#define STREAM_OUTPUT_BUFFER_SIZE 65536
#define CONST_BUFFER_SIZE 65536

struct int4
{
	int i[4];

	bool operator == ( const int4& comp) const
	{
		for (int j=0; j<4; j++)
			if (comp[j] != i[j])
				return false;
		return true;
	}

	int& operator [] (UINT index) { return i[index]; } //non-const
	const int& operator [] (UINT index) const { return i[index]; } //const
};

struct uint4
{
	UINT u[4];

	bool operator == ( const uint4& comp) const
	{
		for (int j=0; j<4; j++)
			if (comp[j] != u[j])
				return false;
		return true;
	}

	UINT& operator [] (UINT index) { return u[index]; } //non-const
	const UINT& operator [] (UINT index) const { return u[index]; } //const
};


// Note:	if values are added or removed from FLOAT_INPUT_CLASS, this will break the tables
//			of values in a number of groups
enum FLOAT_INPUT_CLASS
{
	FIC_INFn,	// -Infinity
	FIC_REALn,	// -Real
	FIC_ONEn,	// -1
	FIC_DENn,	// -Denormal
	FIC_ZEROn,	// -0
	FIC_ZEROp,	// +0
	FIC_DENp,	// +Denormal
	FIC_ONEp,	// +1
	FIC_REALp,	// +Real
	FIC_INFp,	// +Infinity
	FIC_NAN,		// Nan
	NUM_FLOAT_INPUT_CLASSES,
	FIC_NONE = -1,
};

enum FLOAT_RESULT_CLASS
{
	FRC_UNKNOWN = 0,
	FRC_INFn,	// -Infinity
	FRC_REALn,	// -Real
	FRC_ZEROn,	// -0
	FRC_ZEROp,	// +0
	FRC_REALp,	// +Real
	FRC_INFp,	// +Infinity
	FRC_NAN,	// Nan
	FRC_REAL,	// Any real (+ or -)
	FRC_SRC0p,	// Src0
	FRC_SRC0n,	// -Src0
	FRC_SRC1p,	// Src1
	FRC_SRC1n,	// -Src1
	FRC_SRC2p,	// Src2
	FRC_SRC2n,	// -Src2
	FRC_ONEp,	// +1.0
	FRC_ONEn,	// -1.0
	FRC_ZERO,	// +/- 0.0
	FRC_EXACT,
	FRC_DENZp,	// +0 or +denorm
	FRC_DENZn,	// -0 or -denorm
	FRC_DENZ,	// +/-0 or +/-denorm
	FRC_ANY,	// Special case auto pass
	FRC_ANY_REAL, // Special case meaning any real (non-NaN/INF) value.
};

enum DOUBLE_INPUT_CLASS
{
	DIC_INFn,
	DIC_REALn,
	DIC_ONEn,
	DIC_DENn,
	DIC_ZEROn,
	DIC_ZEROp,
	DIC_DENp,
	DIC_ONEp,
	DIC_REALp,
	DIC_INFp,
	DIC_NAN,
	NUM_DOUBLE_INPUT_CLASSES,
	DIC_NONE = -1,
};

enum DOUBLE_RESULT_CLASS
{
	DRC_UNKNOWN = 0,
	DRC_INFn,	// -Infinity
	DRC_REALn,	// -Real
	DRC_ZEROn,	// -0
	DRC_ZEROp,	// +0
	DRC_REALp,	// +Real
	DRC_INFp,	// +Infinity
	DRC_NAN,	// Nan
	DRC_REAL,	// Any real (+ or -)
	DRC_SRC0p,	// Src0
	DRC_SRC0n,	// -Src0
	DRC_SRC1p,	// Src1
	DRC_SRC1n,	// -Src1
	DRC_SRC2p,	// Src2
	DRC_SRC2n,	// -Src2
	DRC_ONEp,	// +1.0
	DRC_ONEn,	// -1.0
	DRC_ZERO,	// +/- 0.0
	DRC_EXACT,
	DRC_DENZp,	// +0 or +denorm
	DRC_DENZn,	// -0 or -denorm
	DRC_DENZ,	// +/-0 or +/-denorm
	DRC_ANY,	// Special case auto pass
	DRC_ANY_REAL, // Special case meaning any real (non-NaN/INF) value.
};

enum DOUBLE_CLASSIFICATION
{
	DC_INFn,
	DC_REALn,
	DC_ONEn,
	DC_ZEROn,
	DC_ZEROp,
	DC_ONEp,
	DC_REALp,
	DC_INFp,
	DC_NAN,
	DC_SRC0n,
	DC_SRC0,
	DC_SRC1n,
	DC_SRC1,
	DC_REAL,
	DC_NORM,
	DC_NORMn,
	DC_NORMp,
	DC_DENORM,
	DC_DENORMn,
	DC_DENORMp,
	NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES,
	DC_SATURATED_REAL,
	DC_SATURATED_SRC0,
	DC_SATURATED_SRC0n,
	DC_SATURATED_SRC1,
	DC_SATURATED_SRC1n,
	DC_ZEROpn, // either positive or negative zero
	NUM_DOUBLE_CLASSIFICATION_TYPES
};

enum SRC_MOD_TYPE
{
	SMOD_NONE,
	SMOD_NEG,
	SMOD_ABS,
	SMOD_ABSNEG,
	NUM_SRC_MOD_TYPES,
};

enum INST_MOD_TYPE
{
	IMOD_NONE,
	IMOD_SAT,
	NUM_INST_MOD_TYPES,
};

enum SHADER_STAGE_TYPE
{
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_GEOMETRY,
	SHADER_STAGE_PIXEL,
	SHADER_STAGE_COMPUTE,
};

enum OPERAND_TYPE
{
	OT_TEMP,
	OT_CONSTANT_BUFFER,
};

DECLARE_NAMED_VALUES( OPERAND_TYPE );

extern tstring __stdcall ToString( SRC_MOD_TYPE type );
extern tstring __stdcall ToString( INST_MOD_TYPE type );
extern tstring __stdcall ToString( SHADER_STAGE_TYPE type );
extern tstring __stdcall ToString( const int4& Vector );
extern tstring __stdcall ToString( const uint4& Vector );

extern bool __stdcall FromString( SRC_MOD_TYPE *pType, const tstring &str );
extern bool __stdcall FromString( INST_MOD_TYPE *pType, const tstring &str );
extern bool __stdcall FromString( SHADER_STAGE_TYPE *pType, const tstring &str );
extern bool __stdcall FromString( int4 *pVector, const tstring &str );
extern bool __stdcall FromString( uint4 *pVector, const tstring &str );


extern FLOAT_INPUT_CLASS GetInputClass( float f );

extern bool g_bDumpDissasm;
extern bool g_bForceSM40;

class CShader5xTest : public CD3D11Test
{
protected:
	CShader5xTest( DXGI_FORMAT RTFormat = DXGI_FORMAT_R8G8B8A8_UINT ) :
		m_RTFormat( RTFormat ),
		m_pRTTexture2D( NULL ),
		m_pRTTexture2DCopy( NULL ),
		m_pRTBuffer( NULL ),
		m_pRTBufferCopy( NULL ),
		m_pRTView( NULL ),
		m_pRTBufferView( NULL ),
		m_pRTUAView( NULL ),
		m_pRTUABufferView( NULL ),
		m_pConstBuffer( NULL ),
		m_pStreamOutputBuffer( NULL ),
		m_pStreamOutputBufferCopy( NULL ),
		m_pVS( NULL ),
		m_pGS( NULL ),
		m_pPS( NULL ),
		m_pHS( NULL ),
		m_pDS( NULL ),
		m_pCS( NULL ),
		m_pRSEnabled( NULL ),
		m_pRSDisabled( NULL ),
		m_pDSDisabled( NULL ),
		m_pPassThroughVS( NULL ),
		m_pSingleVertexVS(NULL),
		m_pNullGSStreamOutput( NULL ),
		m_pClearStreamOutputVS( NULL ),
		m_soBufferSize( STREAM_OUTPUT_BUFFER_SIZE ),
		m_ShaderStage( D3D_SHADER_STAGE_VERTEX ),
		m_shaderStageParameterStr( "ShaderStage" ),
		TiledTier(D3D11_TILED_RESOURCES_NOT_SUPPORTED)
		{
			m_viewPort.Width=1;
			m_viewPort.Height=1;
			m_szPassThroughVS=
				"vs_4_0"								"\n"
				"dcl_input_sv v[0].x, vertexID"			"\n"
				"dcl_output_sv o[0].xyzw, position"		"\n" // Position in o[1]
				"mov o[0], vec4( 0.0, 0.0, 0.0, 1.0 )"	"\n" // Draw pixel to center of viewport
				;
			m_szSingleVertexVS=
				"vs_4_0"								"\n"
				"dcl_input_sv v[0].x, vertexID"			"\n"
				"dcl_output_sv o[0].xyzw, position"		"\n" // Position in o[1]
				"mov o[0], vec4( 0.0, 0.0, 0.0, 1.0 )"	"\n" // Draw pixel to center of viewport
				;
		}

	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual TEST_RESULT SetupD3D();
	virtual TEST_RESULT SetupRenderTarget();
	virtual TEST_RESULT SetupTestCase();
	void CleanupD3D();
	void ClearStreamOutputBuffer( unsigned int numVertices );
	static FLOAT_INPUT_CLASS GetInputClass( float f );
	static float ResolveSrc( float f, SRC_MOD_TYPE mod );
	static float ResolveDest( float f, INST_MOD_TYPE mod );
	bool SkipBasedOnCommonFactors();
	virtual void AddCommonParameters();
	virtual testfactor::RFactor GetTestFactorWithDefaultStageCoverage( testfactor::RFactor testcases );
	bool TiledOpsSupported() const
	{
		return !!(TiledTier & D3D11_TILED_RESOURCES_TIER_2);
	}

protected:
	void CheckTiledSupport();

protected:
	D3D_SHADER_STAGE		m_ShaderStage;
	D3D11_VIEWPORT			m_viewPort;
	DXGI_FORMAT				m_RTFormat;
	ID3D11Buffer			*m_pRTBuffer;
	ID3D11Buffer			*m_pRTBufferCopy;

	// IMPORTANT: These two definitions should only be used for pixel shader only code. Everything else should use the RTBuffer.
	ID3D11Texture2D			*m_pRTTexture2D;
	ID3D11Texture2D			*m_pRTTexture2DCopy;
	ID3D11RenderTargetView	*m_pRTView;
	ID3D11UnorderedAccessView* m_pRTUAView;

	ID3D11RenderTargetView	*m_pRTBufferView;
	ID3D11UnorderedAccessView* m_pRTUABufferView;
	ID3D11Buffer			*m_pConstBuffer;
	ID3D11Buffer			*m_pStreamOutputBuffer;
	ID3D11Buffer			*m_pStreamOutputBufferCopy;

	ID3D11RasterizerState	*m_pRSEnabled;
	ID3D11RasterizerState	*m_pRSDisabled;
	ID3D11DepthStencilState *m_pDSDisabled;


	ID3D11VertexShader		*m_pVS;
	ID3D11GeometryShader	*m_pGS;
	ID3D11PixelShader		*m_pPS;
	ID3D11HullShader		*m_pHS;
	ID3D11DomainShader		*m_pDS;
	ID3D11ComputeShader		*m_pCS;

	ID3D11VertexShader		*m_pClearStreamOutputVS;
	ID3D11VertexShader		*m_pPassThroughVS;
	ID3D11VertexShader		*m_pSingleVertexVS;
	ID3D11GeometryShader	*m_pNullGSStreamOutput;

	UINT m_soBufferSize;
	tstring m_szPassThroughVS;
	tstring m_szSingleVertexVS;
	const tstring m_shaderStageParameterStr;

public:
	// Useful utility function that "caches" shader blobs to avoid
	// assembling the same shader source multiple times.
	bool AssembleShader( const char *szSource, ID3D10Blob **ppShaderBuf, ID3D10Blob **ppErrorBuf, UINT globalFlags = 0 );

	// Dumps shader disassembly to shell's message output.
	bool MessageDumpShaderDisassembly( const UINT *pShaderBlob, SIZE_T shaderSize );

protected:
	template<typename T>
	HRESULT MapAndRead(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, T & outVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_READ, 0, &mappedResource ) ) )
			return hr;

		memcpy(&outVal, mappedResource.pData, sizeof(outVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}

	template<typename T>
	HRESULT MapAndWrite(ID3D11DeviceContext* pContext, ID3D11Resource* pResource, const T & inVal)
	{
		HRESULT hr = E_UNEXPECTED;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if( FAILED( hr = pContext->Map( pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			return hr;

		memcpy(mappedResource.pData, &inVal, sizeof(inVal));

		pContext->Unmap( pResource, 0 );

		return hr;
	}

private:
	struct SHADER_MAP_KEY
	{
		unsigned long hash;
		tstring source;

		SHADER_MAP_KEY() {}
		SHADER_MAP_KEY( const SHADER_MAP_KEY &key ) : hash( key.hash ), source( key.source ) {}

		bool operator<( const SHADER_MAP_KEY &key ) const
		{
			return ( hash < key.hash ) || ( hash == key.hash && source < key.source );
		}
		bool operator==( const SHADER_MAP_KEY &key ) const
		{
			return hash == key.hash && source == key.source;
		}
	};
	// Useful for avoiding reassembling shader source.
	typedef std::map< SHADER_MAP_KEY, ID3D10Blob * > ShaderBlobMapType;
	ShaderBlobMapType m_ShaderBlobMap;
	D3D11_TILED_RESOURCES_TIER TiledTier;
};
