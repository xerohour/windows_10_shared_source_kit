//////////////////////////////////////////////////////////////////////////
//  TypeConvert.cpp
//  created:	2005/03/25
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "TypeConvert.h"
#include "float.h"
#include <d3d11shader.h>

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

CShader5xTest_TypeConvert::CShader5xTest_TypeConvert() 
	: CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
	, m_pPassThroughPS(NULL)
	, m_pPassThroughHS(NULL) 
	, m_pPassThroughDS(NULL) 
{
}


void CShader5xTest_TypeConvert::CleanupTestCase()
{
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pHS );
	SAFE_RELEASE( m_pDS );
	SAFE_RELEASE( m_pCS );
	SAFE_RELEASE( m_pPassThroughPS );
	SAFE_RELEASE( m_pPassThroughHS );
	SAFE_RELEASE( m_pPassThroughDS );
}

TEST_RESULT CShader5xTest_TypeConvert::CheckPixel( BYTE Pixel[4][4] )
{
	TEST_RESULT tRes = RESULT_PASS;
	tstring szComponents[4] = 
	{
		_T("w"),
		_T("z"),
		_T("y"),
		_T("x"),
		
	};

	tstring szFailAll = _T(" shader unit failed.\n");
	tstring szFailComponent = _T(" shader failed for %s component.\n");

	//for ( UINT uiShader = 0; uiShader < 3; ++uiShader )
	UINT uiShader = 0;
	{
		if( *((UINT32*)Pixel[uiShader]) != 0xFFFFFFFF )
		{
			if ( *((UINT32*)Pixel[uiShader]) == 0 )
			{
				WriteToLog( 1, "%s", (ToString(m_ShaderStage) + szFailAll).c_str() );
				
				for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
				{
					if ( m_bResultIsFloat )
					{
						WriteToLog( 1, _T("Expected results for component %s: %f\n"), 
							szComponents[uiComponent].c_str(), *(FLOAT*)&m_uiResult[3-uiComponent] );
					}
					else
					{
						WriteToLog( 1, _T("Expected results for component %s: 0x%X\n"), 
							szComponents[uiComponent].c_str(), m_uiResult[3-uiComponent] );
					}
				}
			}
			else
			{
				for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
				{
					if ( Pixel[uiShader][uiComponent] != 0xFF )
					{
						WriteToLog( 1, (ToString(m_ShaderStage) + szFailComponent).c_str(), szComponents[uiComponent].c_str() );
						if ( m_bResultIsFloat )
						{
							WriteToLog( 1, _T("Expected result: %f\n"), *(FLOAT*)&m_uiResult[3-uiComponent] );
						}
						else
						{
							WriteToLog( 1, _T("Expected result: 0x%X\n"), m_uiResult[3-uiComponent] );
						}
					}
				}
			}

			tRes = RESULT_FAIL;
		}
	}

	return tRes;
}

TEST_RESULT CShader5xTest_TypeConvert::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	typedef BYTE Pixel[4][4];
	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView, ClearColor );

	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch(1, 1, 1);
	else
		GetEffectiveContext()->Draw(1, 0);

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	Pixel pixel;
	if (FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, pixel) ) )
	{
		WriteToLog( _T("CShader5xTest_TypeConvert::ExecuteTestCase Failed to map RT Copy, hr=%s."), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	tRes = CheckPixel( pixel );

	return tRes;
}
TEST_RESULT CShader5xTest_TypeConvert::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base SetupTestCase to determine if should skip
	tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	ID3D10Blob *pShaderBuf = NULL;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);

	asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 4, AssemblyHelper::swizX );

	{
		static tstring szPackRes =
			"and r4.x, 0xFF000000, r3.x"			"\n"
			"and r4.y, 0x00FF0000, r3.y"			"\n"
			"and r4.z, 0x0000FF00, r3.z"			"\n"
			"and r4.w, 0x000000FF, r3.w"			"\n"

			"or r4.x, r4.x, r4.y"					"\n"
			"or r4.y, r4.z, r4.w"					"\n"
			"or r4.x, r4.x, r4.y"					"\n";

		tstring insructionCode = GetInstruction() + GetCompare() + szPackRes;

		hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			insructionCode, &m_pRTBufferView, &m_pRTUABufferView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

		if ( FAILED(hr) )
		{
			WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
	}

	{
		TCOMPtr<ID3D11ShaderReflection*> rReflector;
		if(FAILED(hr = GetFramework()->LL_D3DReflect(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)(ID3D11ShaderReflection**)rReflector)))
			goto reflectFail;

		D3D11_SHADER_DESC reflectDesc;
		if(FAILED(hr = rReflector->GetDesc(&reflectDesc)))
			goto reflectFail;

		// Reflect Constant Buffers:
		for(UINT iCBuffer=0; iCBuffer < reflectDesc.ConstantBuffers; iCBuffer++)
		{
			// Match reflected constant buffers with XML data
			ID3D11ShaderReflectionConstantBuffer* rCBuffer;
			rCBuffer = rReflector->GetConstantBufferByIndex(iCBuffer);

			D3D11_SHADER_BUFFER_DESC cbDesc;
			if(FAILED(hr = rCBuffer->GetDesc(&cbDesc)))
				goto reflectFail;

			// Reflect variables:
			for(UINT iVariable=0; iVariable < cbDesc.Variables; iVariable++)
			{
				// Match reflected constant buffers with XML data
				ID3D11ShaderReflectionVariable* rVar;
				rVar = rCBuffer->GetVariableByIndex(iVariable);

				D3D11_SHADER_VARIABLE_DESC varDesc;
				if(FAILED(hr = rVar->GetDesc(&varDesc)))
					goto reflectFail;

			}
		}

reflectFail:
		;
	}

	if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	UINT32 pConstData[8];

	pConstData[0] = m_uiSrc[0];
	pConstData[1] = m_uiSrc[1];
	pConstData[2] = m_uiSrc[2];
	pConstData[3] = m_uiSrc[3];

	CalcResult();

	pConstData[4] = m_uiResult[0];
	pConstData[5] = m_uiResult[1];
	pConstData[6] = m_uiResult[2];
	pConstData[7] = m_uiResult[3];
	
	if (FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) )
	{
		WriteToLog( _T( "Failed to map m_pConstBuffer, hr=%s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	return tRes;
}
	
UINT32 UINTValues[] =
{
	-1,
	0,
	1,
	INT_MAX,
	INT_MIN,
	*(UINT32*)&(float&)FloatMaker::IND(),
	*(UINT32*)&(float&)FloatMaker::INFp(),
	*(UINT32*)&(float&)FloatMaker::INFn(),
	*(UINT32*)&(float&)FloatMaker::QNANp(),
	*(UINT32*)&(float&)FloatMaker::QNANn(),
	*(UINT32*)&(float&)FloatMaker::SNANp(),
	*(UINT32*)&(float&)FloatMaker::SNANn(),
	-1,
	0,
	1,
	INT_MAX,
	INT_MIN,
	*(UINT32*)&(float&)FloatMaker::IND(),
	*(UINT32*)&(float&)FloatMaker::INFp(),
	*(UINT32*)&(float&)FloatMaker::INFn(),
	*(UINT32*)&(float&)FloatMaker::QNANp(),
	*(UINT32*)&(float&)FloatMaker::QNANn(),
	*(UINT32*)&(float&)FloatMaker::SNANp(),
	*(UINT32*)&(float&)FloatMaker::SNANn(),
};

FLOAT FLOATtoIValues[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x807fffffUL ), // -Denorm
	FloatMaker( (UINT32) 0x007fffffUL ), // +Denorm
	-1.f,
	1.f,
	-0.1969f,
	0.4001f,
	65535.f,
	-2147483648.999f,
	2147483647.999f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x807fffffUL ), // -Denorm
	FloatMaker( (UINT32) 0x007fffffUL ), // +Denorm
	-1.f,
	1.f,
	-0.1969f,
	0.4001f,
	65535.f,
	-2147483648.999f,
	2147483647.999f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
};
FLOAT FLOATtoUValues[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x807fffffUL ), // -Denorm
	FloatMaker( (UINT32) 0x007fffffUL ), // +Denorm
	-1.f,
	1.f,
	-0.1969f,
	0.4001f,
	65535.f,
	-2147483648.999f,
	2147483647.999f,
	-4294967295.999f,
	4294967295.999f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker::SNANn(),
	FloatMaker::QNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x807fffffUL ), // -Denorm
	FloatMaker( (UINT32) 0x007fffffUL ), // +Denorm
	-1.f,
	1.f,
	-0.1969f,
	0.4001f,
	65535.f,
	-2147483648.999f,
	2147483647.999f,
	-4294967295.999f,
	4294967295.999f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
};

UINT32 Float16Values[] =
{
	Float16Maker::INFn(),
	Float16Maker::INFp(),
	Float16Maker::QNANn(),
	Float16Maker::SNANn(),
	Float16Maker::ZEROn(),
	Float16Maker::ZEROp(),
	Float16Maker::DENORMn(),
	Float16Maker::DENORMp(),
	Float16Maker::FLOATMAXp(),
	Float16Maker(-1.f),
	Float16Maker(1.f),
	Float16Maker(-0.1969f),
	Float16Maker(65535.f),
	Float16Maker(333.3333f),
	Float16Maker::FLOATMAXn(),
	Float16Maker::FLOATMAXp(),

	Float16Maker::INFn(),
	Float16Maker::INFp(),
	Float16Maker::QNANp(),
	Float16Maker::SNANp(),
	Float16Maker::ZEROn(),
	Float16Maker::ZEROp(),
	Float16Maker::DENORMn(),
	Float16Maker::DENORMp(),
	Float16Maker::FLOATMAXp(),
	Float16Maker(-1.f),
	Float16Maker(1.f),
	Float16Maker(-0.0969f),
	Float16Maker(3.14159f),
	Float16Maker(54321.f),
	Float16Maker::FLOATMAXn(),
	Float16Maker::FLOATMAXp()
};

float Float32Values[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker::ZEROn(),
	FloatMaker::ZEROp(),
	FloatMaker::DENORMn(),
	FloatMaker::DENORMp(),
	FloatMaker::FLOATMAXp(),
	-1.f,
	1.f,
	-0.1969f,
	65535.f,
	333.3333f,
	FloatMaker::FLOATMAXn(),
	FloatMaker::FLOATMAXp(),

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker::ZEROn(),
	FloatMaker::ZEROp(),
	FloatMaker::DENORMn(),
	FloatMaker::DENORMp(),
	FloatMaker::FLOATMAXp(),
	-1.f,
	1.f,
	-0.0969f,
	3.14159f,
	54321.f,
	FloatMaker::FLOATMAXn(),
	FloatMaker::FLOATMAXp()
};

void CShader5xTest_ftoi::InitTestParameters()
{
	m_bResultIsFloat = false;

	size_t count = sizeof( FLOATtoIValues ) / ( 2 * sizeof( FLOATtoIValues[0] ) );
	size_t stride = sizeof( FLOATtoIValues[0] );

	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );

	testfactor::RFactor src0x = AddParameter( "Src0.x", (float*)&m_uiSrc[0], new CTableValueSet< FLOAT >( &FLOATtoIValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", (float*)&m_uiSrc[1], new CTableValueSet< FLOAT >( &FLOATtoIValues[2], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", (float*)&m_uiSrc[2], new CTableValueSet< FLOAT >( &FLOATtoIValues[4], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", (float*)&m_uiSrc[3], new CTableValueSet< FLOAT >( &FLOATtoIValues[6], stride, count ) );
	
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod, pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0mod * ( src0x % src0y % src0z % src0w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_ftoi::GetCompare()
{
	float fTmp;//TODO: for this and ftou, Make sure that after negation or abs, it's still within acceptable range
	tstring szRegs[] =
	{
		"x",
		"y",
		"z",
		"w",
	};

	tstring szComp = 
		"ieq r3, r1, r2"			"\n";

	for ( UINT i = 0; i < 4; ++i )
	{
		switch ( GetInputClass( *(FLOAT*)&m_uiSrc[i] ) )
		{
		case FIC_NAN:
		case FIC_INFn:
		case FIC_INFp:
			szComp += (tstring)"mov r3." + szRegs[i] + (tstring)", 0xFFFFFFFF\n";
			break;
		default:
			if ( *(FLOAT*)&m_uiSrc[i] < -2147483647.999 || 2147483647.999 < *(FLOAT*)&m_uiSrc[i] )
			{
				szComp += (tstring)"mov r3." + szRegs[i] + (tstring)", 0xFFFFFFFF\n";
			}
			break;
		}
	}

	return szComp;
}
tstring CShader5xTest_ftoi::GetInstruction()
{
	static tstring szNeg = "ftoi r2, -r0\n";
	static tstring szAbs = "ftoi r2, abs(r0)\n";
	static tstring szAbsNeg = "ftoi r2, -abs(r0)\n";
	static tstring szNone = "ftoi r2, r0\n";

	switch ( m_SrcMod )
	{
		case SMOD_NONE:
			return szNone;
		case SMOD_NEG:
			return szNeg;
		case SMOD_ABS:
			return szAbs;
		case SMOD_ABSNEG:
			return szAbsNeg;
	}

	return szNone;
}
void CShader5xTest_ftoi::CalcResult()
{
	UINT32 uiControl = _controlfp( 0, 0 );
	FLOAT fTmp;
	INT32 iTmp;

	for ( UINT i = 0; i < 4; ++i )
	{
		fTmp = *(FLOAT*)&m_uiSrc[i];
		switch ( m_SrcMod )
		{
			case SMOD_NEG:
				fTmp = -fTmp;
				break;
			case SMOD_ABS:
				fTmp = abs( fTmp );
				break;
			case SMOD_ABSNEG:
				fTmp = abs( fTmp );
				fTmp = -fTmp;
				break;
		}

		if ( 0 > fTmp )
			_controlfp( _RC_UP, _MCW_RC );
		else
			_controlfp( _RC_DOWN, _MCW_RC );


		if( fTmp >= (float) (UINT) ( 1 << 31 ) )
			iTmp = 0x7fffffff;
		else
		if( fTmp <= (float) (int) ( 1 << 31 ) )
			iTmp = 0x80000000;
		else
			iTmp = (INT32)fTmp;
		m_uiResult[i] = *(UINT32*)&iTmp;
	}
	_controlfp( uiControl, _MCW_RC );
}
	
void CShader5xTest_ftou::InitTestParameters()
{
	m_bResultIsFloat = false;

	size_t count = sizeof( FLOATtoUValues ) / ( 2 * sizeof( FLOATtoUValues[0] ) );
	size_t stride = sizeof( FLOATtoUValues[0] );

	testfactor::RFactor src0x = AddParameter( "Src0.x", (float*)&m_uiSrc[0], new CTableValueSet< FLOAT >( &FLOATtoUValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", (float*)&m_uiSrc[1], new CTableValueSet< FLOAT >( &FLOATtoUValues[2], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", (float*)&m_uiSrc[2], new CTableValueSet< FLOAT >( &FLOATtoUValues[4], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", (float*)&m_uiSrc[3], new CTableValueSet< FLOAT >( &FLOATtoUValues[6], stride, count ) );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = (src0x % src0y % src0z % src0w);
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_ftou::GetCompare()
{
	tstring szRegs[] =
	{
		"x",
		"y",
		"z",
		"w",
	};

	tstring szComp = 
		"ieq r3, r1, r2"			"\n";

	for ( UINT i = 0; i < 4; ++i )
	{
		switch ( GetInputClass( *(FLOAT*)&m_uiSrc[i] ) )
		{
		case FIC_NAN:
		case FIC_INFn:
		case FIC_INFp:
			szComp += (tstring)"mov r3." + szRegs[i] + (tstring)", 0xFFFFFFFF\n";
			break;
		default:
			if ( *(FLOAT*)&m_uiSrc[i] < 0 || 4294967295.999f < *(FLOAT*)&m_uiSrc[i] )
			{
				szComp += (tstring)"mov r3." + szRegs[i] + (tstring)", 0xFFFFFFFF\n";
			}
		}
	}

	return szComp;
}
tstring CShader5xTest_ftou::GetInstruction()
{
	tstring szInst = "ftou r2, r0\n";

	return szInst;
}
void CShader5xTest_ftou::CalcResult()
{
	UINT32 uiControl = _controlfp( 0, 0 );

	for ( UINT i = 0; i < 4; ++i )
	{
		if ( 0 > (*(FLOAT*)&m_uiSrc[i]) )
			_controlfp( _RC_UP, _MCW_RC );
		else
			_controlfp( _RC_DOWN, _MCW_RC );

		if ( (*(FLOAT*)&m_uiSrc[i]) == 4294967295.999f )
			m_uiResult[i] = 0xFFFFFFFF;
		else
			m_uiResult[i] = (UINT32)(*(FLOAT*)&m_uiSrc[i]);
	}
	_controlfp( uiControl, _MCW_RC );
}
	
void CShader5xTest_itof::InitTestParameters()
{
	m_bResultIsFloat = true;

	size_t count = sizeof( UINTValues ) / ( 2 * sizeof( UINTValues[0] ) );
	size_t stride = sizeof( UINTValues[0] );

	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	testfactor::RFactor src0x = AddParameter( "Src0.x", (INT32*)&m_uiSrc[0], new CTableValueSet< INT32 >( (INT32*)&UINTValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", (INT32*)&m_uiSrc[1], new CTableValueSet< INT32 >( (INT32*)&UINTValues[2], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", (INT32*)&m_uiSrc[2], new CTableValueSet< INT32 >( (INT32*)&UINTValues[4], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", (INT32*)&m_uiSrc[3], new CTableValueSet< INT32 >( (INT32*)&UINTValues[6], stride, count ) );
	
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod, pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0mod * ( src0x % src0y % src0z % src0w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_itof::GetCompare()
{
	tstring szComp = 
		"iadd r2, r1, -r2"			"\n"
		"ige r3, r1, 0"				"\n"

		"if_z r3.x"					"\n"
		"ineg r2.x, r2.x"			"\n"
		"endif"						"\n"

		"if_z r3.y"					"\n"
		"ineg r2.y, r2.y"			"\n"
		"endif"						"\n"

		"if_z r3.z"					"\n"
		"ineg r2.z, r2.z"			"\n"
		"endif"						"\n"

		"if_z r3.w"					"\n"
		"ineg r2.w, r2.w"			"\n"
		"endif"						"\n"

		"uge r3, 0x00000001, r2"	"\n";

	return szComp;
}
tstring CShader5xTest_itof::GetInstruction()
{
	static tstring szNeg = "itof r2, -r0\n";
	static tstring szNoNeg = "itof r2, r0\n";

	if ( SMOD_NEG == m_SrcMod )
	{
		return szNeg;
	}
	else
	{
		return szNoNeg;
	}	
}
void CShader5xTest_itof::CalcResult()
{
	FLOAT fSrc[4];
	UINT32 uiSrc[4];
	DOUBLE dTemp;
	UINT32 uiControl = 0;

	memcpy( uiSrc, m_uiSrc, sizeof(UINT32) * 4 );

	if ( SMOD_NEG == m_SrcMod )
	{
		for ( UINT i = 0; i < 4; ++i )
		{
			uiSrc[i] ^= 0xFFFFFFFF;
			uiSrc[i]++;
		}
	}

	for ( UINT i = 0; i < 4; ++i )
	{
		fSrc[i] = (float)(INT32)uiSrc[i];
		dTemp = (double)(INT32)uiSrc[i];

		if ( dTemp > fSrc[i] )
		{
			uiControl = _controlfp( 0, 0 );

			_controlfp( _RC_UP, _MCW_RC );
			fSrc[i] = (float)dTemp;
			_controlfp( uiControl, _MCW_RC );
		}
	}
	
	memcpy( m_uiResult, &fSrc, sizeof(float) * 4 );
}
	
void CShader5xTest_utof::InitTestParameters()
{
	m_bResultIsFloat = true;

	size_t count = sizeof( UINTValues ) / ( 2 * sizeof( UINTValues[0] ) );
	size_t stride = sizeof( UINTValues[0] );

	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc[0], new CTableValueSet< UINT32 >( &UINTValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc[1], new CTableValueSet< UINT32 >( &UINTValues[2], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc[2], new CTableValueSet< UINT32 >( &UINTValues[4], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc[3], new CTableValueSet< UINT32 >( &UINTValues[6], stride, count ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = (src0x % src0y % src0z % src0w);
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_utof::GetCompare()
{
	tstring szComp = 
		"iadd r2, r1, -r2"			"\n"
		"uge r3, 0x00000001, r2"	"\n";

	return szComp;
}
tstring CShader5xTest_utof::GetInstruction()
{
	tstring szInst = "utof r2, r0\n";

	return szInst;
}
void CShader5xTest_utof::CalcResult()
{
	FLOAT fSrc[4];
	DOUBLE dTemp;
	UINT32 uiControl = 0;

	for ( UINT i = 0; i < 4; ++i )
	{
		fSrc[i] = (float)m_uiSrc[i];
		dTemp = (double)m_uiSrc[i];

		if ( dTemp > fSrc[i] )
		{
			uiControl = _controlfp( 0, 0 );

			_controlfp( _RC_UP, _MCW_RC );
			fSrc[i] = (float)dTemp;
			_controlfp( uiControl, _MCW_RC );
		}
	}
	
	memcpy( m_uiResult, &fSrc, sizeof(float) * 4 );
}

TEST_RESULT CShader5xTest_f32tof16::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the parent setup
	tRes = CShader5xTest_TypeConvert::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// This is a FL11+ only instruction
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("The instruction f32tof16, is only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

void CShader5xTest_f32tof16::InitTestParameters()
{
	m_bResultIsFloat = false;

	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	size_t count = 8;
	size_t stride = sizeof( Float32Values[0] );

	testfactor::RFactor src0x = AddParameter( "Src0.x", (float*)&m_uiSrc[0], new CTableValueSet< float >( &Float32Values[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", (float*)&m_uiSrc[1], new CTableValueSet< float >( &Float32Values[8], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", (float*)&m_uiSrc[2], new CTableValueSet< float >( &Float32Values[16], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", (float*)&m_uiSrc[3], new CTableValueSet< float >( &Float32Values[24], stride, count ) );

	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod, pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0mod * (src0x % src0y % src0z % src0w);
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_f32tof16::GetCompare()
{
	const char* szRegs[] = { "x", "y", "z", "w" };

	tstring szComp = "";		

	for ( UINT i = 0; i < 4; ++i )
	{
		const float fValue = *(const float*)&m_uiSrc[i];
		switch ( CShader5xTest::GetInputClass( fValue ) )
		{
		case FIC_NAN:
			// Verify is float16 NAN 
			szComp += FormatString("and r3.%s, r2.%s, 0x7FFF\n", szRegs[i], szRegs[i]);
			szComp += FormatString("ult r3.%s, 0x7C00, r3.%s\n", szRegs[i], szRegs[i]);
			break;
		default:
			// Verify equal
			szComp += FormatString("ieq r3.%s, r1.%s, r2.%s\n", szRegs[i], szRegs[i], szRegs[i]);
			break;
		}
	}

	return szComp;
}

tstring CShader5xTest_f32tof16::GetInstruction()
{
	tstring szInst;
	if( m_SrcMod == SMOD_NEG )
		szInst = "f32tof16 r2, -r0\n";
	else
		szInst = "f32tof16 r2, r0\n";

	return szInst;
}

void CShader5xTest_f32tof16::CalcResult()
{
	UINT32 uiResult[4];

	for ( UINT i = 0; i < 4; ++i )
	{
		UINT32 uiSrc = m_uiSrc[i];
		if( m_SrcMod == SMOD_NEG )
			uiSrc ^= 0x80000000;
		
		uiResult[i] = Float16Maker(*((float*)&uiSrc));
	}
	
	memcpy( m_uiResult, uiResult, sizeof(UINT32) * 4 );
}

TEST_RESULT CShader5xTest_f16tof32::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the parent setup
	tRes = CShader5xTest_TypeConvert::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// This is a FL11+ only instruction
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("The instruction f16tof32, is only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

void CShader5xTest_f16tof32::InitTestParameters()
{
	m_bResultIsFloat = true;

	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	size_t count = 8;
	size_t stride = sizeof( Float16Values[0] );

	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc[0], new CTableValueSet< UINT32 >( &Float16Values[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc[1], new CTableValueSet< UINT32 >( &Float16Values[8], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc[2], new CTableValueSet< UINT32 >( &Float16Values[16], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc[3], new CTableValueSet< UINT32 >( &Float16Values[24], stride, count ) );

	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod, pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0mod * (src0x % src0y % src0z % src0w);
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

tstring CShader5xTest_f16tof32::GetCompare()
{
	const char* szRegs[] = { "x", "y", "z", "w" };

	tstring szComp = "";		

	for ( UINT i = 0; i < 4; ++i )
	{
		const Float16Maker f16Value( *(const UINT16*)&m_uiSrc[i] );
		const float fValue = static_cast<float>( f16Value );
		switch ( CShader5xTest::GetInputClass( fValue ) )
		{
		case FIC_NAN:
			// Verify is float32 NAN 
			szComp += FormatString("and r3.%s, r2.%s, 0x7FFFFFFF\n", szRegs[i], szRegs[i]);
			szComp += FormatString("ult r3.%s, 0x7F800000, r3.%s\n", szRegs[i], szRegs[i]);
			break;
		default:
			// Verify equal
			szComp += FormatString("ieq r3.%s, r1.%s, r2.%s\n", szRegs[i], szRegs[i], szRegs[i]);
			break;
		}
	}

	return szComp;
}

tstring CShader5xTest_f16tof32::GetInstruction()
{
	tstring szInst;
	if( m_SrcMod == SMOD_NEG )
		szInst = "f16tof32 r2, -r0\n";
	else
		szInst = "f16tof32 r2, r0\n";

	return szInst;
}

void CShader5xTest_f16tof32::CalcResult()
{
	float fResult[4];
	
	for ( UINT i = 0; i < 4; ++i )
	{
		if( m_SrcMod == SMOD_NEG )
			fResult[i] = Float16Maker((UINT16)(m_uiSrc[i] ^ 0x8000));
		else
			fResult[i] = Float16Maker((UINT16)m_uiSrc[i]);
	}
	
	memcpy( m_uiResult, fResult, sizeof(float) * 4 );
}
