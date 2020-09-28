//////////////////////////////////////////////////////////////////////////
//  Index.cpp
//  created:	2005/05/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Index.h"

#define SZ_ARRAY_SIZE "4096"
#define EPSILON 0.0001f

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_SCENARIO )
	NAMED_VALUE( _T( "Temp" ), CShader5xTest_Index::INDEXING_SCENARIO_TEMP )
	NAMED_VALUE( _T( "TempNested" ), CShader5xTest_Index::INDEXING_SCENARIO_TEMP_NESTED )
	NAMED_VALUE( _T( "ConstBuffer" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER )
	NAMED_VALUE( _T( "ConstBufferNested" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED )
	NAMED_VALUE( _T( "InputOutput" ), CShader5xTest_Index::INDEXING_SCENARIO_INPUT_OUTPUT )
	NAMED_VALUE( _T( "InputOutputNested" ), CShader5xTest_Index::INDEXING_SCENARIO_INPUT_OUTPUT_NESTED )
	NAMED_VALUE( _T( "ConstBufferUniform" ), CShader5xTest_Index::INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM )
	NAMED_VALUE( _T( "Buffer" ), CShader5xTest_Index::INDEXING_SCENARIO_BUFFER )
	NAMED_VALUE( _T( "Texture2D" ), CShader5xTest_Index::INDEXING_SCENARIO_TEXTURE2D )
	NAMED_VALUE( _T( "Texture3D" ), CShader5xTest_Index::INDEXING_SCENARIO_TEXTURE3D )
	NAMED_VALUE( _T( "Sampler" ), CShader5xTest_Index::INDEXING_SCENARIO_SAMPLER )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_SCENARIO )

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_INSTRUCTION )
	NAMED_VALUE( _T( "mov" ), CShader5xTest_Index::INDEXING_INSTRUCTION_MOV )
	NAMED_VALUE( _T( "ld" ), CShader5xTest_Index::INDEXING_INSTRUCTION_LD )
	NAMED_VALUE( _T( "sample" ), CShader5xTest_Index::INDEXING_INSTRUCTION_SAMPLE )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_INSTRUCTION )

BEGIN_NAMED_VALUES( CShader5xTest_Index::INDEXING_COMPUTATION_METHOD )
	NAMED_VALUE( _T( "TempRegister" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_REG )
	NAMED_VALUE( _T( "StaticIndexableRegister" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG )
	NAMED_VALUE( _T( "ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST )

	NAMED_VALUE( _T( "TempRegister_Plus_ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE )
	NAMED_VALUE( _T( "StaticIndexableRegister_Plus_ImmediateConstant" ), CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE )
END_NAMED_VALUES( CShader5xTest_Index::INDEXING_COMPUTATION_METHOD )

typedef UINT32 VertexElement[4];
typedef FLOAT32 VertexFloatElement[4];


// temp indexing
const char szTempIndexSrc[] =
	// Write to the temp array using gray code of index
	"mov r0, vec4( 4096, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"ushr r0.y, r0.x, 1\n"
		"xor r0.y, r0.y, r0.x\n" // r0.y = gray( r0.x )
		"iadd r0.y, r0.y, -4\n" // Invent a bias so we can add an offset to the index.
		"mov x0[r0.y + 3], r0.xxxx\n"
	"endloop\n"
	// Read from the temp array and validate values
	"mov r0.x, 4096\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, x0[r0.y + 3]\n" // r0.z = revgray( r0.x )
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;
	
const char szTempIndexNestedSrc[] =
	// Write to the temp array using gray code of index
	"mov x0[0], vec4( 4096, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"ushr x0[0].y, x0[0].x, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].x\n" 
		"iadd x0[0].y, x0[0].y, -4\n" // Invent a bias so we can add an offset to the index.
		"mov x0[x0[0].y + 4], x0[0].xxxx\n"
	"endloop\n"
	// Read from the temp array and validate values
	"mov x0[0].x, 4096\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, x0[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;
	

// constant buffer indexing
const char szConstBufferIndexSrc[] =
	"mov r0, vec4( 4096, 0, 0, 0xffffffff )\n"
	// Read from the const buffer array and verify data
	// using indexed addressing.
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, c0[r0.y + 4]\n" // r0.z = revgray( r0.x )
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szConstBufferIndexNestedSrc[] =
	// Write to the temp array using gray code of index
	"mov x0[0], vec4( 4096, 0, 0, 0xffffffff )\n"
	// Read from the const buffer array and verify data
	// using indexed addressing.
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, c0[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;


// input/output indexing
const char szVSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[r0.y + 4]\n"		// r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"	// replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n" // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;
	
const char szGSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[0][r0.y + 4]\n"	// r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"	// replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n" // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szDSInputIndexSrc[] =
	"mov r0, vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, vcp[0][r0.y + 4]\n"  // r0.z = revgray( r0.x )
		"mov o[r0.y + 4], r0.zzzz\n"    // replicate r0.z to o[r0.x]
		"%s\n"							//"mov o[r0.y + 20], r0.zzzz\n"   // replicate r0.z to o[r0.x + 16]
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

const char szPSInputIndexSrc[] =
	"mov r0, vec4( 15, 0, 0, 0xffffffff )\n"
	"loop\n"
		"breakc_z r0.x\n"
		"iadd r0.y, r0.x, -4\n"
		"mov r0.z, v[r0.y + 4]\n" // r0.z = revgray( r0.x )
		"ine r0.y, r0.z, 0\n"  // Make sure the value is non-zero
		"and r0.w, r0.w, r0.y\n"
		"ushr r0.y, r0.z, 1\n"
		"xor r0.y, r0.y, r0.z\n" 
		"ieq r0.y, r0.y, r0.x\n"
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
		"iadd r0.x, r0.x, -1\n"
		"ieq r0.y, r0.z, v[r0.x + 17]\n" // check that v[r0.x + 16] == v[r0.x]
		"and r0.w, r0.w, r0.y\n" // Accumulate results in r0.w
	"endloop\n"
	"mov r0.x, r0.w\n"	// Move results from .w to .x
	;

	
const char szVSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;
	
const char szGSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[0][x0[0].y + 4]\n"  // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n"  // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char szDSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 16, 0, 0, 0xffffffff )\n"
	"loop\n"
		"iadd x0[0].x, x0[0].x, -1\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, vcp[0][x0[0].y + 4]\n"  // x0[0].z = revgray( x0[0].x )
		"mov o[x0[0].y + 4], x0[0].zzzz\n"    // replicate x0[0].z to o[x0[0].x]
		"%s\n"							//"mov o[x0[0].y + 20], x0[0].zzzz\n"   // replicate x0[0].z to o[x0[0].x + 16]
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char szPSInputIndexNestedSrc[] =
	"mov x0[0], vec4( 15, 0, 0, 0xffffffff )\n"
	"loop\n"
		"breakc_z x0[0].x\n"
		"iadd x0[0].y, x0[0].x, -4\n"
		"mov x0[0].z, v[x0[0].y + 4]\n" // x0[0].z = revgray( x0[0].x )
		"ine x0[0].y, x0[0].z, 0\n"  // Make sure the value is non-zero
		"and x0[0].w, x0[0].w, x0[0].y\n"
		"ushr x0[0].y, x0[0].z, 1\n"
		"xor x0[0].y, x0[0].y, x0[0].z\n" 
		"ieq x0[0].y, x0[0].y, x0[0].x\n"
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
		"iadd x0[0].x, x0[0].x, -1\n"
		"ieq x0[0].y, x0[0].z, v[x0[0].x + 17]\n" // check that v[r0.x + 16] == v[r0.x]
		"and x0[0].w, x0[0].w, x0[0].y\n" // Accumulate results in x0[0].w
	"endloop\n"
	"mov x0[0].x, x0[0].w\n"	// Move results from .w to .x
	;

const char g_szResourceIndexSampleSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r1, vec4(%ff,%ff,%ff,%ff)\n"
	"sample_indexable(%s)(float,float,float,float) r0, r1, t[%s], s[%s]\n"
;

const char g_szResourceIndexLoadSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r1, vec4(%d,%d,%d,%d)\n"
	"ld_indexable(%s)(float,float,float,float) r0, r1, t[%s]\n"
;

const char g_szResourceIndexMovSrc[] = 
	"mov r2.x, 0\n"
	"mov x0[0].x, 1\n"
	"mov r0, cb[%s][0]\n"
;

const CShader5xTest_Index::INDEXING_COMPUTATION_METHOD g_pResourceIndexComputations[] =
{
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_REG,				// #r
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG,		// #x
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST,		// e.g 2
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE,	// e.g #r + 2
	CShader5xTest_Index::INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE	// e.g #x + 2
};

const UINT g_pResourceIndexValues[] = 
{
	0, // == INDEXING_COMPUTATION_METHOD_TEMP_REG			
	1, // == INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG	
	2, // == INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST	
	2, // == INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE
	3  // == INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE
};

const char* g_pResourceIndexExpr[] =
{
	"r2.x",
	"x0[0].x",
	"2",
	"r2.x + 2",
	"x0[0].x + 2"
};

TEST_RESULT CShader5xTest_Index::SetupTestCase()
{
	// Local variables
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base SetupTestCase to determine if should skip based on FL and shader stage
	tRes = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	if( m_is >= INDEXING_SCENARIO_TEXTURE2D && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog(_T("Non-buffer UAVs are not supported on FL < 11"));
		return RESULT_SKIP;
	}

	{
		D3D11_BUFFER_DESC descBuffer;
		m_pRTBuffer->GetDesc( &descBuffer );
		hr = GetDevice()->CreateBuffer( &descBuffer, NULL, &m_pRTBuffer_2 );

		if( FAILED( hr ) )
		{
			WriteToLog( _T("CreateBuffer() failed (m_pRTBuffer_2). HR = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		m_pRTBufferView->GetDesc( &rtviewdesc );
		hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer_2, &rtviewdesc, &m_pRTBufferView_2 );

		if( FAILED( hr ) )
		{
			WriteToLog( _T("CreateRenderTargetView() failed (m_pRTBufferView_2). HR = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_pRTUABufferView )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc;
			m_pRTUABufferView->GetDesc( &rtuaviewdesc );
			hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer_2, &rtuaviewdesc, &m_pRTUABufferView_2 );

			if( FAILED( hr ) )
			{
				WriteToLog( _T("CreateUnorderedAccessView() failed (m_pRTUABufferView_2). HR = %s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}

	switch( m_is )
	{
		default: 
			break;
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
		{
			// Setup the constant buffer with gray code numbers.
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = 4096 * 4 * sizeof( UINT32 );
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			VertexElement *pData = new VertexElement[4096];
			for( int i = 4096; i != 0; )
			{
				--i;
				UINT32 gray = ( i >> 1 ) ^ i;
				VertexElement &e = pData[gray];
				e[0] = i;
				e[1] = i;
				e[2] = i;
				e[3] = i;
			}
			
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = pData;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, &m_pCB ) ) )
			{
				WriteToLog( "CreateBuffer() failed" );
				return RESULT_FAIL;
			}
			
			SAFE_DELETE_ARRAY( pData );
			
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pCB );
		
			break;
		}
		
		case INDEXING_SCENARIO_INPUT_OUTPUT:
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
		{
			// Setup the vertex buffer with gray code numbers.
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = 32 * 4 * sizeof( UINT32 );
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			if( FAILED( GetDevice()->CreateBuffer( &desc, NULL, &m_pVB ) ) )
			{
				WriteToLog( "CreateBuffer() failed" );
				return RESULT_FAIL;
			}
			
			VertexElement *pData;
			D3D11_MAPPED_SUBRESOURCE mappedVertexBuffer;
			HRESULT hr;

			if( FAILED( hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer ) ) )
			{
				WriteToLog( _T( "CShader5xTest_Index::SetupTestCase() - failed to map m_pVB, hr = %s" ), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}

			pData = (VertexElement *)mappedVertexBuffer.pData;

			for( int i = 16; i != 0; )
			{
				--i;
				UINT32 gray = ( i >> 1 ) ^ i;
				VertexElement &e = pData[gray];
				e[0] = i;
				e[1] = i;
				e[2] = i;
				e[3] = i;
				if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL)
				{
					// duplicate values to upper range, this is where our PS operates
					VertexElement &e2 = pData[gray + 16];
					memcpy(&e2, &e, sizeof(e));
				}
			}

			const float oneVal = 1.0f;
			VertexElement &posElem = pData[16];
			posElem[0] = 0;
			posElem[1] = 0;
			posElem[2] = 0;
			posElem[3] = *(DWORD*)(&oneVal);
			
			GetEffectiveContext()->Unmap( m_pVB, 0 );

			ID3D11Buffer *buffers[] = { m_pVB };
			UINT strides[] = { 32 * 4 * sizeof( UINT32 ) };
			UINT offsets[] = { 0 };
			
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, buffers, strides, offsets );
			
			break;
		}

		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = 4 * sizeof( UINT32 );
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				VertexElement pData;

				pData[0] = i + 1;
				pData[1] = i + 1;
				pData[2] = i + 1;
				pData[3] = i + 1;
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = 0;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, (ID3D11Buffer**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateBuffer() failed" );
					return RESULT_FAIL;
				}
			}
			
			GetEffectiveContext()->VSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->GSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->PSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->HSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->DSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
			GetEffectiveContext()->CSSetConstantBuffers( 0, NUM_INDEXABLE_RESOURCES, (ID3D11Buffer**) m_pResources );
		
			break;
		}
		
		case INDEXING_SCENARIO_SAMPLER:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				if( FAILED( GetDevice()->CreateTexture2D( &desc, NULL, (ID3D11Texture2D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture2D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
				
				float borderColor = (float) (i + 1) / (float) NUM_INDEXABLE_RESOURCES;  

				D3D11_SAMPLER_DESC sampDesc;
				sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
				sampDesc.MipLODBias = 0.0f;
				sampDesc.MaxAnisotropy = 16;
				sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDesc.BorderColor[0] = borderColor;
				sampDesc.BorderColor[1] = borderColor;
				sampDesc.BorderColor[2] = borderColor;
				sampDesc.BorderColor[3] = borderColor;
				sampDesc.MinLOD = 0.0f;
				sampDesc.MaxLOD = FLT_MAX;

				if( FAILED( GetDevice()->CreateSamplerState( &sampDesc, &m_pSamplers[i] ) ) )
				{
					WriteToLog( "CreateSamplerState() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );

			GetEffectiveContext()->VSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->GSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->PSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->HSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->DSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			GetEffectiveContext()->CSSetSamplers( 0, NUM_INDEXABLE_RESOURCES, m_pSamplers );
			
			break;
		}
		
		case INDEXING_SCENARIO_BUFFER:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = 4 * sizeof( FLOAT32 );
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				VertexFloatElement pData;

				pData[0] = FLOAT32(i + 1);
				pData[1] = FLOAT32(i + 1);
				pData[2] = FLOAT32(i + 1);
				pData[3] = FLOAT32(i + 1);
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = 0;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateBuffer( &desc, &initData, (ID3D11Buffer**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateBuffer() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.ElementOffset = 0;
				srvDesc.Buffer.ElementWidth = 4;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}

		case INDEXING_SCENARIO_TEXTURE2D:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				VertexFloatElement pData;
				pData[0] = FLOAT32(( i * 4 ) + 1);
				pData[1] = FLOAT32(( i * 4 ) + 2);
				pData[2] = FLOAT32(( i * 4 ) + 3);
				pData[3] = FLOAT32(( i * 4 ) + 4);
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = sizeof(float) * 2;
				initData.SysMemSlicePitch = 0;
				if( FAILED( GetDevice()->CreateTexture2D( &desc, &initData, (ID3D11Texture2D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture2D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}
		
		case INDEXING_SCENARIO_TEXTURE3D:
		{
			for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
			{
				// Setup the constant buffer with gray code numbers.
				D3D11_TEXTURE3D_DESC desc;
				desc.Width = 2;
				desc.Height = 2;
				desc.Depth = 2;
				desc.MipLevels = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				VertexFloatElement pData[2];
				for( UINT j = 0; j < 2; ++j )
				{
					pData[j][0] = FLOAT32(( i * 8 ) + ( j * 4 ) + 1);
					pData[j][1] = FLOAT32(( i * 8 ) + ( j * 4 ) + 2);
					pData[j][2] = FLOAT32(( i * 8 ) + ( j * 4 ) + 3);
					pData[j][3] = FLOAT32(( i * 8 ) + ( j * 4 ) + 4);
				}
				
				D3D11_SUBRESOURCE_DATA initData;
				initData.pSysMem = &pData;
				initData.SysMemPitch = sizeof(float) * 2;
				initData.SysMemSlicePitch = sizeof(float) * 4;
				if( FAILED( GetDevice()->CreateTexture3D( &desc, &initData, (ID3D11Texture3D**)&m_pResources[i] ) ) )
				{
					WriteToLog( "CreateTexture3D() failed" );
					return RESULT_FAIL;
				}
				
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MostDetailedMip = 0;
				srvDesc.Texture3D.MipLevels = 1;

				if( FAILED( GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &m_pSRVs[i] ) ) )
				{
					WriteToLog( "CreateShaderResourceView() failed" );
					return RESULT_FAIL;
				}
			}

			GetEffectiveContext()->VSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->GSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->PSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->HSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->DSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			GetEffectiveContext()->CSSetShaderResources( 0, NUM_INDEXABLE_RESOURCES, m_pSRVs );
			
			break;
		}
	}
	
	tRes = BuildShaders();
	if( tRes != RESULT_PASS )
		return tRes;	

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	return tRes;	
}


void CShader5xTest_Index::CleanupTestCase()
{
	ID3D11Buffer *pBufferNull[] = { NULL, NULL, NULL, NULL };
	UINT strides[] = { 0, 0, 0, 0 };
	UINT offsets[] = { 0, 0, 0, 0 };
	
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->HSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->DSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->CSSetConstantBuffers( 0, 1, pBufferNull );
	GetEffectiveContext()->IASetVertexBuffers( 0, 4, pBufferNull, strides, offsets );
	GetEffectiveContext()->IASetInputLayout( NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVSBlob );
	
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pRTBufferView_2 );
	SAFE_RELEASE( m_pRTUABufferView_2 );
	SAFE_RELEASE( m_pRTBuffer_2 );

	for( UINT i = 0; i < NUM_INDEXABLE_RESOURCES; ++i )
	{
		SAFE_RELEASE( m_pResources[i] );
		SAFE_RELEASE( m_pSamplers[i] );
		SAFE_RELEASE( m_pSRVs[i] );
		SAFE_RELEASE( m_pUAVs[i] );
	}
}


TEST_RESULT CShader5xTest_Index::ExecuteTestCase()
{
	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	else
		GetEffectiveContext()->Draw( 1, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// Lock the output stream and examine the results
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	bool bResult = true;

	DWORD pResult[2];
	ID3D11Buffer* pRenderTargets[2] =
	{
		m_pRTBuffer,
		m_pRTBuffer_2
	};

	// Lock the render target and examine the results
	for (int n = 0; n < ARRAYSIZE(pRenderTargets); ++n)
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, pRenderTargets[n] );
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, pResult[n]) ) )
		{
			WriteToLog( _T( "CShader5xTest_Index::ExecuteTestCase() - failed to map m_pRTBufferCopy[%d], hr = %s" ), n, D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( (tr = VerifyResult((void*)pResult)) == RESULT_FAIL )
	{
		WriteToLog( _T( "CShader5xTest_Index::ExecuteTestCase() - Test case failed." ) );
	}

	return tr;
}

TEST_RESULT CShader5xTest_Index::BuildShaders()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	const AssemblyHelper::Swizzle swizW = { AssemblyHelper::w, AssemblyHelper::noMask, AssemblyHelper::noMask, AssemblyHelper::noMask };

	bool bCustomVSPassThrough = false;
	bool bCustomPSPassThrough = false;
	bool bCustomGSPassThrough = false;
	const bool bPSPassThroughVerifies = true;

	tstring instructionCode;
	tstring pixelInstructionCode;

	const UINT STRING_SIZE = 256;
	char szResIndexSrc[STRING_SIZE];
	memset(szResIndexSrc, 0, STRING_SIZE);

	// Resource indexing test cases
	if( m_is >= INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM )
	{
		int computationIndex = -1;
		for( UINT i = 0; i < (sizeof(g_pResourceIndexComputations) / sizeof(INDEXING_COMPUTATION_METHOD)); ++i )
			if( g_pResourceIndexComputations[i] == m_icm )
				computationIndex = i;

		if( computationIndex == -1 )
		{
			WriteToLog( "Skipping unhandled indexing computation" );
			return RESULT_SKIP;
		}

		const char* pResTypeStr[] = { "buffer", "Texture2D", "Texture3D" };
		const char *szResType = NULL;
		if( m_is == INDEXING_SCENARIO_BUFFER )
			szResType = pResTypeStr[0];
		else if( m_is == INDEXING_SCENARIO_TEXTURE2D || m_is == INDEXING_SCENARIO_SAMPLER )
			szResType = pResTypeStr[1];
		else if( m_is == INDEXING_SCENARIO_TEXTURE3D )
			szResType = pResTypeStr[2];

		if( m_is == INDEXING_SCENARIO_SAMPLER )
		{
			m_fTexCoordX = 2.0f;
			m_fTexCoordY = 2.0f;
			m_fTexCoordZ = 2.0f;
		}	

		if( m_ii == INDEXING_INSTRUCTION_MOV )
		{
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexMovSrc, g_pResourceIndexExpr[computationIndex]);
		}
		else if( m_ii == INDEXING_INSTRUCTION_LD )
		{
			// ld uses unsigned ints to index into the texture
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexLoadSrc, 
				UINT32(m_fTexCoordX), UINT32(m_fTexCoordY), UINT32(m_fTexCoordZ), 0,
				szResType,
				g_pResourceIndexExpr[computationIndex]);
		}
		else if( m_ii == INDEXING_INSTRUCTION_SAMPLE )
		{
			_snprintf(szResIndexSrc, STRING_SIZE, g_szResourceIndexSampleSrc, 
				m_fTexCoordX, m_fTexCoordY, m_fTexCoordZ, 0.0f,
				szResType,
				g_pResourceIndexExpr[computationIndex],
				g_pResourceIndexExpr[computationIndex]);
		}
		else
		{
			WriteToLog( "Skipping unhandled indexing instruction" );
			return RESULT_SKIP;
		}
	}

	const bool bFL10 = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0;
	tstring extraMovStr = "";

	switch( m_is )
	{
		case INDEXING_SCENARIO_TEMP:
			instructionCode = szTempIndexSrc;
			break;
			
		case INDEXING_SCENARIO_TEMP_NESTED:
			instructionCode = szTempIndexNestedSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
			instructionCode = szConstBufferIndexSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
			instructionCode = szConstBufferIndexNestedSrc;
			break;
			
		case INDEXING_SCENARIO_INPUT_OUTPUT:
			switch (m_ShaderStage)
			{
			case D3D_SHADER_STAGE_VERTEX:
				if( !bFL10 ) // VS and GS only have 16I/O registers on FL 10.0
					extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szVSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_GEOMETRY:
			case D3D_SHADER_STAGE_HULL:
					extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szGSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_DOMAIN:
				extraMovStr = "mov o[r0.y + 20], r0.zzzz"; // replicate r0.z to o[r0.x + 16]

				instructionCode = FormatString( szDSInputIndexSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_PIXEL:
				instructionCode = szPSInputIndexSrc;
				break;
			}
			pixelInstructionCode = szPSInputIndexSrc;
			break;
			
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
			switch (m_ShaderStage)
			{
			case D3D_SHADER_STAGE_VERTEX:
				if( !bFL10 ) // VS and GS only have 16I/O registers on FL 10.0
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz"; // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szVSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_GEOMETRY:
			case D3D_SHADER_STAGE_HULL:
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz"; //"mov o[x0[0].y + 20], x0[0].zzzz\n" // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szGSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_DOMAIN:
					extraMovStr = "mov o[x0[0].y + 20], x0[0].zzzz\n";   // replicate x0[0].z to o[x0[0].x + 16]

				instructionCode = FormatString( szDSInputIndexNestedSrc, extraMovStr.c_str() );
				break;
			case D3D_SHADER_STAGE_PIXEL:
				instructionCode = szPSInputIndexNestedSrc;
				break;
			}
			pixelInstructionCode = szPSInputIndexSrc;
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
		case INDEXING_SCENARIO_SAMPLER:
		case INDEXING_SCENARIO_BUFFER:
		case INDEXING_SCENARIO_TEXTURE2D:
		case INDEXING_SCENARIO_TEXTURE3D:
			instructionCode = szResIndexSrc;
			break;

		default:
			WriteToLog( "Skipping unhandled indexing scenario" );
			return RESULT_SKIP;
	}

	AssemblyHelper asmHelper(0, 0);
	AssemblyHelper asmHelperGeometry(0, 1);
	AssemblyHelper asmHelperPixel(0, 2);
	const bool bIsFL10AndCompute = (!g_TestApp.IsFL11OrGreater() && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE);

	// The number of input/output registers for 10 was 16. D3D10.1 increased that to 32.
	const UINT numVSGSInputRegisters = bFL10 ? 16 : 32;
	const UINT numIEdescs = numVSGSInputRegisters;
	const bool bVSOrGS = m_ShaderStage == D3D_SHADER_STAGE_VERTEX || m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY;
	const UINT numInputRegisters = ( bVSOrGS ) ? numVSGSInputRegisters : 32;
	const UINT numOutputRegisters = ( m_ShaderStage == D3D_SHADER_STAGE_VERTEX && bFL10 ) ? 16 : 32;
	const UINT positionRegister = 16;

	switch( m_is )
	{
		case INDEXING_SCENARIO_TEMP:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(0, 1);
			asmHelper.SetIndexableTempSize( 0, 4095 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEMP_NESTED:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(0, 0);
			asmHelper.SetIndexableTempSize( 0, 4096 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
	
			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(4096, 1);
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;
			
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
			// use implicit position at center of RT
			asmHelper = AssemblyHelper(4096, 0);
			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );

			if( !bIsFL10AndCompute )
				asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
			asmHelper = AssemblyHelper(1, 3);
			asmHelper.SetConstantBufferSize( 1, 1 );
			asmHelper.SetConstantBufferSize( 2, 1 );
			asmHelper.SetConstantBufferSize( 3, 1 );
			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_BUFFER:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "buffer, float");
				asmHelper.SetResourceDecl(1, "buffer, float");
				asmHelper.SetResourceDecl(2, "buffer, float");
				asmHelper.SetResourceDecl(3, "buffer, float");
			}

			asmHelper.SetIndexableTempSize( 0, 1 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEXTURE2D:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "Texture2D, float");
				asmHelper.SetResourceDecl(1, "Texture2D, float");
				asmHelper.SetResourceDecl(2, "Texture2D, float");
				asmHelper.SetResourceDecl(3, "Texture2D, float");
			}

			asmHelper.SetIndexableTempSize( 0, 4 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_TEXTURE3D:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "Texture3D, float");
				asmHelper.SetResourceDecl(1, "Texture3D, float");
				asmHelper.SetResourceDecl(2, "Texture3D, float");
				asmHelper.SetResourceDecl(3, "Texture3D, float");
			}

			asmHelper.SetIndexableTempSize( 0, 4 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;

		case INDEXING_SCENARIO_SAMPLER:
			asmHelper = AssemblyHelper(0, 3);

			// Assembling the shader will fail if there is no decl and we are indexing using a immediate const
			// It’s expected because it's a case of using the indexable modifier without actually indexing anything. In this case, a decl is required.
			if( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST == m_icm )
			{
				asmHelper.SetSamplerDecl(0, "mode_default");
				asmHelper.SetSamplerDecl(1, "mode_default");
				asmHelper.SetSamplerDecl(2, "mode_default");
				asmHelper.SetSamplerDecl(3, "mode_default");
				asmHelper.SetResourceDecl(0, "Texture2D, float");
				asmHelper.SetResourceDecl(1, "Texture2D, float");
				asmHelper.SetResourceDecl(2, "Texture2D, float");
				asmHelper.SetResourceDecl(3, "Texture2D, float");
			}

			asmHelper.SetIndexableTempSize( 0, 4 );
			asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
			asmHelper.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
			break;
			
		case INDEXING_SCENARIO_INPUT_OUTPUT:
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
			{
				bool skipInputDecl = false;
				bool skipOutputDecl = false;
				if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
				{
					WriteToLog( "Skipping unsupported indexing scenario" );
					return RESULT_SKIP;
				}
				asmHelper = AssemblyHelper(0, 1);
				bCustomPSPassThrough = true;
				bCustomGSPassThrough = bFL10 ? true : false;	// Custom GS is need for FL10 to duplicate results in the upper range of registers
				asmHelper.SetIndexableTempSize( 0, 16 );
				if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL)
				{
					// We will need a custom VS if driving a pixel shader on FL10.0 because the position needs to be set in the GS, not the VS
					bCustomVSPassThrough = bFL10 ? true : false;

					if( bCustomVSPassThrough )
					{
						asmHelperGeometry.SetPositionOutputRegIdx( positionRegister );
						asmHelperGeometry.SetupMovToOutputFromLiteral( positionRegister, AssemblyHelper::xyzw, "vec4( 0, 0, 0, 1.0 )" );
					}

					// PS can only output 1 thing
					asmHelper.SetOutputRegMaskRaw( 0 , AssemblyHelper::x );
					asmHelper.SetPositionInputRegIdx( positionRegister );
					asmHelper.SetOutputRegMaskRaw( 1 , AssemblyHelper::x );
				}
				else
				{
					// drive PS with position at center of RT
					if( bFL10 && m_ShaderStage == D3D_SHADER_STAGE_VERTEX )
					{
						// VS in FL10 only has 15 input/output register. Position will be set in the GS
						asmHelperGeometry.SetPositionOutputRegIdx( positionRegister );
						asmHelperGeometry.SetupMovToOutputFromLiteral( positionRegister, AssemblyHelper::xyzw, "vec4( 0, 0, 0, 1.0 )" );
					}
					else
					{
						asmHelper.SetPositionOutputRegIdx( positionRegister );
						asmHelper.SetupMovToOutputFromLiteral( positionRegister, AssemblyHelper::xyzw, "vec4( 0, 0, 0, 1.0 )" );
					}
				}
				for (UINT n = 0; n < 32; ++n)
				{
					// reset some flags
					bool skipInputDecl = false;
					bool skipOutputDecl = false;

					// skip 16, it's for position
					if (n == positionRegister)
					{
						// If we are FL10 and the vertex shader, DONT skip because dont do position in the VS for FL10.0 - that is moved to the GS.
						if( !(bFL10 && D3D_SHADER_STAGE_VERTEX == m_ShaderStage) )
							continue;
					}

					if( bFL10 )
					{
						switch( m_ShaderStage )
						{
						case D3D_SHADER_STAGE_VERTEX:
							// Only 16 inputs allowed
							if(n >= D3D10_VS_INPUT_REGISTER_COUNT)
								skipInputDecl = true;

							// Only 16 outputs allowed
							if(n >= D3D10_VS_OUTPUT_REGISTER_COUNT)
								skipOutputDecl = true;
							break;
						case D3D_SHADER_STAGE_GEOMETRY:
							// Only 16 inputs allowed
							if(n >= D3D10_GS_INPUT_REGISTER_COUNT)
								skipInputDecl = true;

							// Only 32 outputs allowed
							if(n >= D3D10_GS_OUTPUT_REGISTER_COUNT)
								skipOutputDecl = true;
							break;
						case D3D_SHADER_STAGE_PIXEL:
							// Only 32 inputs allowed
							if(n >= D3D10_PS_INPUT_REGISTER_COUNT)
								skipInputDecl = true;

							// Only 32 outputs allowed
							if(n >= D3D10_PS_OUTPUT_REGISTER_COUNT)
								skipOutputDecl = true;
							break;
						default:
							assert(false);
							break;
						}
					}

					if( !skipInputDecl )
					{
						asmHelper.SetUserInputRegMask( n, AssemblyHelper::xyzw );
					}

					if( !skipOutputDecl )
					{
						if (m_ShaderStage != D3D_SHADER_STAGE_PIXEL)
							asmHelper.SetOutputRegMaskRaw( n, AssemblyHelper::xyzw );
					}
				}

				for (UINT n = 0; n < 32; ++n)
				{
					// skip 16, it's for position
					if (n == positionRegister)
						continue;
					if (n == 0)
						asmHelperPixel.SetUserInputRegMask( n, AssemblyHelper::x );
					else if (bPSPassThroughVerifies)
						asmHelperPixel.SetUserInputRegMask( n, AssemblyHelper::xyzw );
				}


				// input ranges
				asmHelper.SetIndexableRange( true, 1, positionRegister - 1 );
				if( numInputRegisters > 16 )	// VS and GS only have 16 inputs on FL 10.0
					asmHelper.SetIndexableRange( true, positionRegister + 1, 31 );

				if (m_ShaderStage != D3D_SHADER_STAGE_PIXEL)
				{
					// output ranges
					asmHelper.SetIndexableRange( false, 1, positionRegister - 1 );
					if( numOutputRegisters > 16 ) // VS only has 16 outputs on FL 10.0
						asmHelper.SetIndexableRange( false, positionRegister + 1, 31 );
				}

				// set up the pass-through pixel shader
				asmHelperPixel.SetPositionInputRegIdx( positionRegister );
				asmHelperPixel.SetOutputRegMaskRaw( 0, AssemblyHelper::x );
				asmHelperPixel.SetupMovToTempFromInput( 1, AssemblyHelper::x, 0, AssemblyHelper::swizX );
				asmHelperPixel.SetupMovToOutputFromTemp( 0, AssemblyHelper::x, 1, AssemblyHelper::swizX );
				if (bPSPassThroughVerifies)
				{
					asmHelperPixel.SetOutputRegMaskRaw( 1, AssemblyHelper::x );
					asmHelperPixel.SetupMovToOutputFromTemp( 1, AssemblyHelper::x, 0, swizW );
					asmHelperPixel.SetIndexableRange( true, 1, positionRegister - 1 );
					asmHelperPixel.SetIndexableRange( true, positionRegister + 1, 31 );
				}
			}
			break;
			
		default:
			WriteToLog( "Skipping unhandled indexing scenario" );
			return RESULT_SKIP;
	}

	asmHelper.ForceRawUAVOutputByteOffset(0);

	// all cases need to output to o[0]
	switch( m_is )
	{
		case INDEXING_SCENARIO_TEMP:
		case INDEXING_SCENARIO_CONSTANT_BUFFER:
		case INDEXING_SCENARIO_INPUT_OUTPUT:
		case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
		case INDEXING_SCENARIO_SAMPLER:
		case INDEXING_SCENARIO_BUFFER:
		case INDEXING_SCENARIO_TEXTURE2D:
		case INDEXING_SCENARIO_TEXTURE3D:
			instructionCode += asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x) + ", r0.x \n";
			break;

		case INDEXING_SCENARIO_TEMP_NESTED:
		case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
		case INDEXING_SCENARIO_INPUT_OUTPUT_NESTED:
			instructionCode += asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x) + ", x0[0].x \n";
			break;
	}

	// cases not doing output indexing can duplicate result to o[1]
	if( !bIsFL10AndCompute )	// FL10 compute, can only have one UAV output
	{
		switch( m_is )
		{
			case INDEXING_SCENARIO_TEMP:
			case INDEXING_SCENARIO_CONSTANT_BUFFER:
			case INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM:
			case INDEXING_SCENARIO_SAMPLER:
			case INDEXING_SCENARIO_BUFFER:
			case INDEXING_SCENARIO_TEXTURE2D:
			case INDEXING_SCENARIO_TEXTURE3D:
				instructionCode += asmHelper.GetRawOutputMovStr(m_ShaderStage, 1, AssemblyHelper::x) + ", r0.x \n";
				break;

			case INDEXING_SCENARIO_TEMP_NESTED:
			case INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED:
				instructionCode += asmHelper.GetRawOutputMovStr(m_ShaderStage, 1, AssemblyHelper::x) + ", x0[0].x \n";
				break;
		}
	}
	
	const LPCSTR semanticNames[] = 
	{
		"0_",  "1_",  "2_",  "3_",  "4_",  "5_",  "6_",  "7_",  "8_",  "9_", 
		"10_", "11_", "12_", "13_", "14_", "15_", "16_", "17_", "18_", "19_", 
		"20_", "21_", "22_", "23_", "24_", "25_", "26_", "27_", "28_", "29_", 
		"30_", "31_"
	};
	D3D11_INPUT_ELEMENT_DESC iedesc[32];
	for (UINT n = 0; n < numIEdescs; ++n)
	{
		iedesc[n].SemanticName = semanticNames[n];
		iedesc[n].SemanticIndex = 0;
		iedesc[n].Format = DXGI_FORMAT_R32G32B32A32_UINT;
		iedesc[n].InputSlot = 0;
		iedesc[n].AlignedByteOffset = n * sizeof( VertexElement );	// Should compare if VertexElement is used too? This should be ok since float32 and UINT32 are same size.
		iedesc[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		iedesc[n].InstanceDataStepRate = 0;
	}
			
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	ID3D11RenderTargetView *ppRTVs[] =
	{
		m_pRTBufferView,
		m_pRTBufferView_2
	};

	ID3D11UnorderedAccessView *ppUAVs[] =
	{
		m_pRTUABufferView,
		m_pRTUABufferView_2
	};

	const UINT numRTs = (!g_TestApp.IsFL11OrGreater() && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE) ? 1 : ARRAYSIZE(ppRTVs);	// You can only have 1 UAV for FL's < 11
	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode, ppRTVs, ppUAVs, numRTs, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS,
		iedesc, numIEdescs, &m_pLayout, 
		bCustomPSPassThrough ? false : true,	// don't create PS pass-through automatically if we need a custom one
		bCustomVSPassThrough ? false : true );	// don't create VS pass-through automatically if we need a custom one

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}

	if (bCustomVSPassThrough && m_ShaderStage != D3D_SHADER_STAGE_VERTEX)
	{
		// If FL10, a custom VS is needed to pass on values to the GS where the position will be set.
		// This is because the VS can only have 16 outputs (0-15), so the position register of 16 doesn't exist.

		const UINT numVSIORegisters = numVSGSInputRegisters;

		// The VS will drive the GS
		AssemblyHelper asmHelperVertex(0, 1);

		for( UINT i = 0; i < numVSIORegisters; ++i )
		{
			asmHelperVertex.SetUserInputRegMask( i, AssemblyHelper::xyzw );
			asmHelperVertex.SetOutputRegMaskRaw( i, AssemblyHelper::xyzw );
			asmHelperVertex.SetupMovToOutputFromInput( i, AssemblyHelper::xyzw, i, AssemblyHelper::swizXYZW );
		}
		
		tstring vsShaderCode = asmHelperVertex.MakeAssemblyShader( D3D_SHADER_STAGE_VERTEX, g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions, "" );
		
		if( !AssembleShader( vsShaderCode.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T( "AssembleShader() failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		if( FAILED( GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			WriteToLog( _T( "CDevice::CreateVertexShader() failed creating VS" ) );
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		if(	FAILED(	hr = GetDevice()->CreateInputLayout( iedesc, numIEdescs,
			pShaderBuf->GetBufferPointer(),
			pShaderBuf->GetBufferSize(),
			&m_pLayout ) ) )
		{
			tRes = RESULT_FAIL;
			WriteError( _T("Failed to create input layout, hr=%s" ), D3DHResultToString(hr).c_str() );
			goto setupDone;
		}
		else
		{
			GetEffectiveContext()->IASetInputLayout( m_pLayout );
		}

		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	}

	if (bCustomGSPassThrough && m_ShaderStage != D3D_SHADER_STAGE_GEOMETRY)
	{
		// If FL10, a custom GS is needed to duplicate the lower register values to the upper range.
		// This is because the VS can only have 16 outputs, but the pixel shader has 32 inputs.
		// 0 - 15 -> Lower range of register indices
		// 16 - 31 > Upper range of register indices

		const UINT numGSInputs = 16;
		const UINT numGSOutputs = 32;

		// Decl the inputs
		for( UINT i = 0; i < numGSInputs; ++i )
		{
			// Skip the position register, we do that below
			if( i == positionRegister )
				continue;
			if( i == 0 && m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
			{
				// These inputs carry the results from the previous stages
				asmHelperGeometry.SetUserInputRegMask( i, AssemblyHelper::x );
			}
			else if( bPSPassThroughVerifies )
			{
				// These outputs pass on the data from the previous stage
				asmHelperGeometry.SetUserInputRegMask( i, AssemblyHelper::xyzw );
			}
		}

		// Decl the outputs
		for( UINT i = 0; i < numGSOutputs; ++i )
		{
			// Skip the position register, we do that below
			if( i == positionRegister )
				continue;

			if( i == 0 && m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
			{
				// These outputs carry the results from the previous stages
				asmHelperGeometry.SetOutputRegMaskRaw( i, AssemblyHelper::x );
			}
			else if( bPSPassThroughVerifies )
			{
				// These outputs pass on the data from the previous stage
				asmHelperGeometry.SetOutputRegMaskRaw( i, AssemblyHelper::xyzw );
			}
		}

		// Go through the 16 inputs and then move to the outputs, duplicating from the lower 16 to the upper 16
		for( UINT i = 0; i < numGSOutputs; ++i )
		{
			// Calculate where to get the input from
			const UINT inputRegIdx = ( i >= numGSInputs ) ? i - 16 : i;

			// Skip 16, it is for position
			if( i == positionRegister )
				continue;

			if( inputRegIdx == 0 && m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
			{
				asmHelperGeometry.SetupMovToOutputFromInput( i, AssemblyHelper::x, inputRegIdx, AssemblyHelper::swizX );
			}
			else
			{
				asmHelperGeometry.SetupMovToOutputFromInput( i, AssemblyHelper::xyzw, inputRegIdx, AssemblyHelper::swizXYZW );
			}
		}

		tstring gsShaderCode = asmHelperGeometry.MakeAssemblyShader( D3D_SHADER_STAGE_GEOMETRY, g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions, "" );
		
		if( !AssembleShader( gsShaderCode.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T( "AssembleShader() failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		if( FAILED( GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CDevice::CreateGeometryShader() failed creating GS" ) );
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	}

	if (bCustomPSPassThrough && m_ShaderStage != D3D_SHADER_STAGE_PIXEL)
	{
		tstring psShaderCode = asmHelperPixel.MakeAssemblyShader( D3D_SHADER_STAGE_PIXEL, g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions, 
			bPSPassThroughVerifies ? pixelInstructionCode : "or r1.x, r1.x, 0x01 \n" );

		if( !AssembleShader( psShaderCode.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T( "AssembleShader() failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		
		if( FAILED( GetDevice()->CreatePixelShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			WriteToLog( _T( "CDevice::CreatePixelShader() failed creating PS" ) );
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
	}

setupDone:
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );

	return tRes;
}

/////////////////////////////////////////////////
// CShader5xTest_IndexTemps
/////////////////////////////////////////////////

void CShader5xTest_IndexTemps::InitTestParameters()
{
	CUserValueSet< INDEXING_SCENARIO > *pIndexingScenarioSet = new CUserValueSet< INDEXING_SCENARIO >();
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_TEMP );
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_TEMP_NESTED );
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_CONSTANT_BUFFER );
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_CONSTANT_BUFFER_NESTED );
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_INPUT_OUTPUT );
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_INPUT_OUTPUT_NESTED );

	CTestCaseParameter<INDEXING_SCENARIO> *pIndexScenarioParam = AddParameter( _T( "IndexingScenario" ), &m_is );
	testfactor::RFactor rfIndexingScenario = AddParameterValueSet( pIndexScenarioParam, pIndexingScenarioSet );	

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rfIndexingScenario;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}

TEST_RESULT CShader5xTest_IndexTemps::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base setup
	tRes = CShader5xTest::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

setupDone:
	return tRes;
}

TEST_RESULT CShader5xTest_IndexTemps::VerifyResult( const void *pResult )
{
	const bool bIsFL10AndCompute = (!g_TestApp.IsFL11OrGreater() && m_ShaderStage == D3D_SHADER_STAGE_COMPUTE);
	bool bResult = ((DWORD*)pResult)[0] == 0xffffffffU;
	if( !bResult )
		WriteToLog( "Shader result does not match expectation" );

	// You can only have 1 UAV for FL's < 11
	if( !bIsFL10AndCompute )
	{
		if (bResult && m_ShaderStage != D3D_SHADER_STAGE_PIXEL && m_ShaderStage != D3D_SHADER_STAGE_COMPUTE)
		{
			bResult = ((DWORD*)pResult)[1] == 0xffffffffU;
			if( !bResult )
				WriteToLog( "Shader result does not match expectation" );
		}
	}

	return (bResult) ? RESULT_PASS : RESULT_FAIL;
}

/////////////////////////////////////////////////
// CShader5xTest_IndexConstantBuffers
/////////////////////////////////////////////////

void CShader5xTest_IndexConstantBuffers::InitTestParameters()
{
	CUserValueSet< INDEXING_SCENARIO > *pIndexingScenarioSet = new CUserValueSet< INDEXING_SCENARIO >();
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_CONSTANT_BUFFER_UNIFORM );
	
	CUserValueSet< INDEXING_COMPUTATION_METHOD > *pIndexingComputationSet = new CUserValueSet< INDEXING_COMPUTATION_METHOD >();
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_REG );				
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE );	
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE );	

	CUserValueSet< INDEXING_INSTRUCTION > *pIndexingInstSet = new CUserValueSet< INDEXING_INSTRUCTION >();
	pIndexingInstSet->AddValue( INDEXING_INSTRUCTION_MOV );

	CTestCaseParameter<INDEXING_SCENARIO> *pIndexScenarioParam = AddParameter( _T( "IndexingScenario" ), &m_is );
	testfactor::RFactor rfIndexingScenario = AddParameterValueSet( pIndexScenarioParam, pIndexingScenarioSet );	

	CTestCaseParameter<INDEXING_COMPUTATION_METHOD> *pIndexComputationParam = AddParameter( _T( "IndexComputation" ), &m_icm );
	testfactor::RFactor rfIndexComputation = AddParameterValueSet( pIndexComputationParam, pIndexingComputationSet );	

	CTestCaseParameter<INDEXING_INSTRUCTION> *pIndexInstParam = AddParameter( _T( "Instruction" ), &m_ii );
	testfactor::RFactor rfIndexInst = AddParameterValueSet( pIndexInstParam, pIndexingInstSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rfIndexingScenario * rfIndexComputation * rfIndexInst;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}

TEST_RESULT CShader5xTest_IndexConstantBuffers::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base setup
	tRes = CShader5xTest::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// Uniform indexing of Constant buffers, textures, and samplers are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

	// Uniform indexing of Constant buffers, textures, and samplers are not supported on D3D12 via SM5.0
	if( g_TestApp.IsD3D11On12() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are not available through D3D11On12.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

TEST_RESULT CShader5xTest_IndexConstantBuffers::VerifyResult( const void *pResult )
{
	DWORD pRes[] = { ((DWORD*)pResult)[0], ((DWORD*)pResult)[1] };
	int computationIndex = -1;
	for( UINT i = 0; i < (sizeof(g_pResourceIndexComputations) / sizeof(INDEXING_COMPUTATION_METHOD)); ++i )
		if( g_pResourceIndexComputations[i] == m_icm )
			computationIndex = i;

	if( computationIndex == -1 )
	{
		WriteToLog( "Skipping unhandled indexing computation" );
		return RESULT_SKIP;
	}

	float expected = (float)g_pResourceIndexValues[computationIndex] + 1.0f;
	bool bResult = fabsf(pRes[0] - expected) < EPSILON;
	if( !bResult )
		WriteToLog( "Shader result at o0 does not match expectation, expected=%f, actual=%f", expected, pRes[0] );

	if (bResult && m_ShaderStage != D3D_SHADER_STAGE_PIXEL && m_ShaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		bResult = fabsf(pRes[1] - expected) < EPSILON;
		if( !bResult )
			WriteToLog( "Shader result at o1 does not match expectation, expected=%f, actual=%f", expected, pRes[1] );
	}

	return (bResult) ? RESULT_PASS : RESULT_FAIL;	
}

/////////////////////////////////////////////////
// CShader5xTest_IndexSamplers
/////////////////////////////////////////////////

void CShader5xTest_IndexSamplers::InitTestParameters()
{
	CUserValueSet< INDEXING_SCENARIO > *pIndexingScenarioSet = new CUserValueSet< INDEXING_SCENARIO >();
	pIndexingScenarioSet->AddValue( INDEXING_SCENARIO_SAMPLER );

	CUserValueSet< INDEXING_COMPUTATION_METHOD > *pIndexingComputationSet = new CUserValueSet< INDEXING_COMPUTATION_METHOD >();
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_REG );				
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE );	
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE );	

	CUserValueSet< INDEXING_INSTRUCTION > *pIndexingInstSet = new CUserValueSet< INDEXING_INSTRUCTION >();
	pIndexingInstSet->AddValue( INDEXING_INSTRUCTION_SAMPLE );

	CTestCaseParameter<INDEXING_SCENARIO> *pIndexScenarioParam = AddParameter( _T( "IndexingScenario" ), &m_is );
	testfactor::RFactor rfIndexingScenario = AddParameterValueSet( pIndexScenarioParam, pIndexingScenarioSet );	

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfPixelShader = AddParameterValue( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	CTestCaseParameter<INDEXING_COMPUTATION_METHOD> *pIndexComputationParam = AddParameter( _T( "IndexComputation" ), &m_icm );
	testfactor::RFactor rfIndexComputation = AddParameterValueSet( pIndexComputationParam, pIndexingComputationSet );	

	CTestCaseParameter<INDEXING_INSTRUCTION> *pIndexInstParam = AddParameter( _T( "Instruction" ), &m_ii );
	testfactor::RFactor rfIndexInst = AddParameterValueSet( pIndexInstParam, pIndexingInstSet );	

	SetRootTestFactor( rfPixelShader * rfIndexingScenario * rfIndexComputation * rfIndexInst );
}

TEST_RESULT CShader5xTest_IndexSamplers::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base setup
	tRes = CShader5xTest::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// Uniform indexing of Constant buffers, textures, and samplers are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

	// Uniform indexing of Constant buffers, textures, and samplers are not supported on D3D12 via SM5.0
	if( g_TestApp.IsD3D11On12() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are not available through D3D11On12.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

TEST_RESULT CShader5xTest_IndexSamplers::VerifyResult( const void *pResult )
{
	int computationIndex = -1;
	for( UINT i = 0; i < (sizeof(g_pResourceIndexComputations) / sizeof(INDEXING_COMPUTATION_METHOD)); ++i )
		if( g_pResourceIndexComputations[i] == m_icm )
			computationIndex = i;

	if( computationIndex == -1 )
	{
		WriteToLog( "Skipping unhandled indexing computation" );
		return RESULT_SKIP;
	}

	float expected = (float) (g_pResourceIndexValues[computationIndex] + 1.0f) / (float) NUM_INDEXABLE_RESOURCES;  
	bool bResult = fabsf(((float*)pResult)[0] - expected) < EPSILON;
	if( !bResult )
		WriteToLog( "Shader result at o0 does not match expectation, expected=%f, actual=%f", expected, ((float*)pResult)[0] );

	if (bResult && m_ShaderStage != D3D_SHADER_STAGE_PIXEL && m_ShaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		bResult = fabsf(((float*)pResult)[1] - expected) < EPSILON;
		if( !bResult )
			WriteToLog( "Shader result at o1 does not match expectation, expected=%f, actual=%f", expected, ((float*)pResult)[1] );
	}

	return (bResult) ? RESULT_PASS : RESULT_FAIL;	
}

/////////////////////////////////////////////////
// CShader5xTest_IndexTextures
/////////////////////////////////////////////////

void CShader5xTest_IndexTextures::InitTestParameters()
{
	CUserValueSet< INDEXING_COMPUTATION_METHOD > *pIndexingComputationSet = new CUserValueSet< INDEXING_COMPUTATION_METHOD >();
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_REG );				
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_STATIC_INDEX_REG );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_IMMEDIATE_CONST );		
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_TEMP_PLUS_IMMEDIATE );	
	pIndexingComputationSet->AddValue( INDEXING_COMPUTATION_METHOD_INDEX_PLUS_IMMEDIATE );	

	CTestCaseParameter<INDEXING_SCENARIO> *pIndexScenarioParam = AddParameter( _T( "IndexingScenario" ), &m_is );
	testfactor::RFactor rfScenarioBuffer = AddParameterValue<INDEXING_SCENARIO>(pIndexScenarioParam, INDEXING_SCENARIO_BUFFER);
	testfactor::RFactor rfScenarioTex2D = AddParameterValue<INDEXING_SCENARIO>(pIndexScenarioParam, INDEXING_SCENARIO_TEXTURE2D);
	testfactor::RFactor rfScenarioTex3D = AddParameterValue<INDEXING_SCENARIO>(pIndexScenarioParam, INDEXING_SCENARIO_TEXTURE3D);

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfPixelShader = AddParameterValue( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	CTestCaseParameter<INDEXING_COMPUTATION_METHOD> *pIndexComputationParam = AddParameter( _T( "IndexComputation" ), &m_icm );
	testfactor::RFactor rfIndexComputation = AddParameterValueSet( pIndexComputationParam, pIndexingComputationSet );	

	CTestCaseParameter<INDEXING_INSTRUCTION> *pIndexInstParam = AddParameter( _T( "Instruction" ), &m_ii );
	testfactor::RFactor rfIndexInstLoad = AddParameterValue( pIndexInstParam, INDEXING_INSTRUCTION_LD );	
	testfactor::RFactor rfIndexInstSample = AddParameterValue( pIndexInstParam, INDEXING_INSTRUCTION_SAMPLE );	

	CTestCaseParameter<float> *pTexCoordXParam = AddParameter(_T("TexCoordX"), &m_fTexCoordX);
	testfactor::RFactor rfTexCoordX_0 = AddParameterValue<float>(pTexCoordXParam, 0.0f);
	testfactor::RFactor rfTexCoordX_1 = AddParameterValue<float>(pTexCoordXParam, 1.0f);

	CTestCaseParameter<float> *pTexCoordYParam = AddParameter(_T("TexCoordY"), &m_fTexCoordY);
	testfactor::RFactor rfTexCoordY_0 = AddParameterValue<float>(pTexCoordYParam, 0.0f);
	testfactor::RFactor rfTexCoordY_1 = AddParameterValue<float>(pTexCoordYParam, 1.0f);

	CTestCaseParameter<float> *pTexCoordZParam = AddParameter(_T("TexCoordZ"), &m_fTexCoordZ);
	testfactor::RFactor rfTexCoordZ_0 = AddParameterValue<float>(pTexCoordZParam, 0.0f);
	testfactor::RFactor rfTexCoordZ_1 = AddParameterValue<float>(pTexCoordZParam, 1.0f);

	testfactor::RFactor rfTexCoordBuffer = rfTexCoordX_0 * rfTexCoordY_0 * rfTexCoordZ_0;
	testfactor::RFactor rfTexCoordTex2D = (rfTexCoordX_0 + rfTexCoordX_1) * (rfTexCoordY_0 + rfTexCoordY_1) * rfTexCoordZ_0;
	testfactor::RFactor rfTexCoordTex3D = (rfTexCoordX_0 + rfTexCoordX_1) * (rfTexCoordY_0 + rfTexCoordY_1) * (rfTexCoordZ_0 + rfTexCoordZ_1);

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfBufferLD = rfScenarioBuffer * rfIndexComputation * rfIndexInstLoad * rfTexCoordBuffer;
	testfactor::RFactor rfBufferLDWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfBufferLD );

	testfactor::RFactor rfTex2DLD = rfScenarioTex2D * rfIndexComputation * rfIndexInstLoad * rfTexCoordTex2D;
	testfactor::RFactor rfTex2DLDWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTex2DLD );

	testfactor::RFactor rfTex3DLD = rfScenarioTex3D * rfIndexComputation * rfIndexInstLoad * rfTexCoordTex3D;
	testfactor::RFactor rfTex3DLDWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTex3DLD );

	testfactor::RFactor rfTex2DSamplePSStage = rfPixelShader * rfScenarioTex2D * rfIndexComputation * rfIndexInstSample * rfTexCoordTex2D;
	testfactor::RFactor rfTex3DSamplePSStage = rfPixelShader * rfScenarioTex3D * rfIndexComputation * rfIndexInstSample * rfTexCoordTex3D;

	testfactor::RFactor rfLoadTests = rfBufferLDWithShaderStages + rfTex2DLDWithShaderStages + rfTex3DLDWithShaderStages;
	testfactor::RFactor rfSampleTests = rfTex2DSamplePSStage + rfTex3DSamplePSStage;

	SetRootTestFactor( rfLoadTests + rfSampleTests );
}

TEST_RESULT CShader5xTest_IndexTextures::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base setup
	tRes = CShader5xTest::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// Uniform indexing of Constant buffers, textures, and samplers are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

	// Uniform indexing of Constant buffers, textures, and samplers are not supported on D3D12 via SM5.0
	if( g_TestApp.IsD3D11On12() )
	{
		WriteToLog( _T("Uniform indexing of constant buffers, textures, and samplers are not available through D3D11On12.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

TEST_RESULT CShader5xTest_IndexTextures::VerifyResult( const void *pResult )
{
	int computationIndex = -1;
	for( UINT i = 0; i < (sizeof(g_pResourceIndexComputations) / sizeof(INDEXING_COMPUTATION_METHOD)); ++i )
		if( g_pResourceIndexComputations[i] == m_icm )
			computationIndex = i;

	if( computationIndex == -1 )
	{
		WriteToLog( "Skipping unhandled indexing computation" );
		return RESULT_SKIP;
	}

	float resourceSize = 1.0f;
	if( m_is == INDEXING_SCENARIO_TEXTURE2D )
		resourceSize = 4.0f;
	else if( m_is == INDEXING_SCENARIO_TEXTURE3D )
		resourceSize = 8.0f;

	float texFactor = (m_fTexCoordZ * 4.0f) + (m_fTexCoordY * 2.0f) + m_fTexCoordX;
	float expected = (float)g_pResourceIndexValues[computationIndex] * resourceSize + texFactor + 1.0f;
	bool bResult = fabsf(((float*)pResult)[0] - expected) < EPSILON;
	if( !bResult )
		WriteToLog( "Shader result at o0 does not match expectation, expected=%f, actual=%f", expected, ((float*)pResult)[0] );

	if (bResult && m_ShaderStage != D3D_SHADER_STAGE_PIXEL && m_ShaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		bResult = fabsf(((float*)pResult)[1] - expected) < EPSILON;
		if( !bResult )
			WriteToLog( "Shader result at o1 does not match expectation, expected=%f, actual=%f", expected, ((float*)pResult)[1] );
	}

	return (bResult) ? RESULT_PASS : RESULT_FAIL;	
}
