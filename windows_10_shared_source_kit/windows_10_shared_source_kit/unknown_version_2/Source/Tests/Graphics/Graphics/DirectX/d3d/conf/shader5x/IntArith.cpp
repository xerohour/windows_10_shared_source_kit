//////////////////////////////////////////////////////////////////////////
//  IntArith.cpp
//  created:	2005/03/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "IntArith.h"

inline void TwosComp( UINT32 uiVal[4] )
{
	for ( UINT i = 0; i < 4; ++i )
	{
		uiVal[i] ^= 0xFFFFFFFF;
	
		uiVal[i]++;
	}
}

void CShader5xTest_IArith::CleanupTestCase()
{
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	m_pCurrentUAView = NULL;

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
}

TEST_RESULT CShader5xTest_IArith::CheckPixel( BYTE Pixel[4][4] )
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

	UINT uiShader = 0;
	if( *((UINT32*)Pixel[uiShader]) != 0xFFFFFFFF )
	{
		if ( *((UINT32*)Pixel[uiShader]) == 0 )
		{
			WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailAll).c_str() );
			
			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				WriteToLog( 1, _T("Expected results for component %s: 0x%X\n"), 
					szComponents[uiComponent].c_str(), m_uiResult[3-uiComponent] );
			}
		}
		else
		{
			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				if ( Pixel[uiShader][uiComponent] != 0xFF )
				{
					WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent).c_str(), szComponents[uiComponent].c_str() );
					WriteToLog( 1, _T("Expected result: 0x%X\n"), m_uiResult[3-uiComponent] );
				}
			}
		}

		tRes = RESULT_FAIL;
	}
	

	return tRes;
}

TEST_RESULT CShader5xTest_IArith::SetupTestCase()
{
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	if( RESULT_PASS != tRes )
		return tRes;
	
	if ( !g_TestApp.IsFL11OrGreater() )
		m_pCurrentUAView = m_pRTUABufferView;
	else
		m_pCurrentUAView = m_pRTUAView;

	return RESULT_PASS;
}

TEST_RESULT CShader5xTest_IArith::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	typedef BYTE Pixel[4][4];
	Pixel pPixel[1];
	HRESULT hr;
	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT ClearColorU[4] = {0, 0, 0, 0};
	
	if( g_TestApp.UAVIsAllowed() )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pCurrentUAView, ClearColorU);
	}

	GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{	
		GetEffectiveContext()->Draw( 1, 0 );
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && m_pCurrentUAView == m_pRTUABufferView ) 
		GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, 0 );
	else
		GetEffectiveContext()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && m_pCurrentUAView == m_pRTUABufferView ) 
	{
		hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, pPixel);
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CShader5xTest_IArith::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	else
	{
		hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, pPixel);
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CShader5xTest_IArith::ExecuteTestCase() - failed to map m_pRTTexture2DCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	tRes = CheckPixel( pPixel[0] );

	return tRes;
}
	
UINT32 UINTValuesX[] =
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
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x80000000UL ), // -0
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x00000000UL ), // +0
};
UINT32 UINTValuesY[] =
{
	*(UINT32*)&(float&)FloatMaker::SNANp(),
	*(UINT32*)&(float&)FloatMaker::SNANn(),
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x80000000UL ), // -0
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x00000000UL ), // +0
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
};
UINT32 UINTValuesZ[] =
{
	*(UINT32*)&(float&)FloatMaker::QNANp(),
	*(UINT32*)&(float&)FloatMaker::QNANn(),
	*(UINT32*)&(float&)FloatMaker::SNANp(),
	*(UINT32*)&(float&)FloatMaker::SNANn(),
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x80000000UL ), // -0
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x00000000UL ), // +0
	-1,
	0,
	1,
	INT_MAX,
	INT_MIN,
	*(UINT32*)&(float&)FloatMaker::IND(),
	*(UINT32*)&(float&)FloatMaker::INFp(),
	*(UINT32*)&(float&)FloatMaker::INFn(),
};
UINT32 UINTValuesW[] =
{
	*(UINT32*)&(float&)FloatMaker::INFp(),
	*(UINT32*)&(float&)FloatMaker::INFn(),
	*(UINT32*)&(float&)FloatMaker::QNANp(),
	*(UINT32*)&(float&)FloatMaker::QNANn(),
	*(UINT32*)&(float&)FloatMaker::SNANp(),
	*(UINT32*)&(float&)FloatMaker::SNANn(),
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x80000000UL ), // -0
	*(UINT32*)&(float&)FloatMaker( (UINT32) 0x00000000UL ), // +0
	-1,
	0,
	1,
	INT_MAX,
	INT_MIN,
	*(UINT32*)&(float&)FloatMaker::IND(),
};

TEST_RESULT CShader5xTest_IArith_Binary::SetupTestCase()
{
	
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( ( tRes = CShader5xTest_IArith::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);

	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 5, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::x, 5, AssemblyHelper::swizX);


	static tstring szPackRes =
		"ieq r4, r3, r2"						"\n"

		"and r5.x, 0xFF000000, r4.x"			"\n"
		"and r5.y, 0x00FF0000, r4.y"			"\n"
		"and r5.z, 0x0000FF00, r4.z"			"\n"
		"and r5.w, 0x000000FF, r4.w"			"\n"

		"or r5.x, r5.x, r5.y"					"\n"
		"or r5.y, r5.z, r5.w"					"\n"
		"or r5.x, r5.x, r5.y"					"\n";

	tstring instructionCode = GetInstruction() + szPackRes;

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);


	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}

	

	
	UINT32 pConstData[12];

	pConstData[0] = m_uiSrc0[0];
	pConstData[1] = m_uiSrc0[1];
	pConstData[2] = m_uiSrc0[2];
	pConstData[3] = m_uiSrc0[3];
	pConstData[4] = m_uiSrc1[0];
	pConstData[5] = m_uiSrc1[1];
	pConstData[6] = m_uiSrc1[2];
	pConstData[7] = m_uiSrc1[3];

	CalcResult();

	pConstData[8] = m_uiResult[0];
	pConstData[9] = m_uiResult[1];
	pConstData[10] = m_uiResult[2];
	pConstData[11] = m_uiResult[3];
	
	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_IArith_Binary::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}


void CShader5xTest_IArith_Binary::InitTestParameters()
{
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;
	testfactor::RFactor src1 = src1x % src1y % src1z % src1w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{src0, 1, NULL, 0},
		{src1, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

void CShader5xTest_IArith_BinaryNoNeg::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

TEST_RESULT CShader5xTest_IArith_Unary::SetupTestCase()
{
	
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( ( tRes = CShader5xTest_IArith::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);

	
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 4, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::x, 4, AssemblyHelper::swizX);

	static tstring szPackRes =
		"ieq r3, r2, r1"						"\n"

		"and r4.x, 0xFF000000, r3.x"			"\n"
		"and r4.y, 0x00FF0000, r3.y"			"\n"
		"and r4.z, 0x0000FF00, r3.z"			"\n"
		"and r4.w, 0x000000FF, r3.w"			"\n"

		"or r4.x, r4.x, r4.y"					"\n"
		"or r4.y, r4.z, r4.w"					"\n"
		"or r4.x, r4.x, r4.y"					"\n";

	tstring instructionCode = GetInstruction() + szPackRes;

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}


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
	
	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_IArith_Unary::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}


void CShader5xTest_IArith_Unary::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0x % src0y % src0z % src0w;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

TEST_RESULT CShader5xTest_IArith_Trinary::SetupTestCase()
{
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( ( tRes = CShader5xTest_IArith::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(3, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW);

	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 6, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::x, 6, AssemblyHelper::swizX);

	static tstring szPackRes =
		"ieq r5, r4, r3"						"\n"

		"and r6.x, 0xFF000000, r5.x"			"\n"
		"and r6.y, 0x00FF0000, r5.y"			"\n"
		"and r6.z, 0x0000FF00, r5.z"			"\n"
		"and r6.w, 0x000000FF, r5.w"			"\n"

		"or r6.x, r6.x, r6.y"					"\n"
		"or r6.y, r6.z, r6.w"					"\n"
		"or r6.x, r6.x, r6.y"					"\n";
		
	tstring instructionCode = GetInstruction() + szPackRes;

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}

	UINT32 pConstData[16];

	pConstData[0] = m_uiSrc0[0];
	pConstData[1] = m_uiSrc0[1];
	pConstData[2] = m_uiSrc0[2];
	pConstData[3] = m_uiSrc0[3];
	pConstData[4] = m_uiSrc1[0];
	pConstData[5] = m_uiSrc1[1];
	pConstData[6] = m_uiSrc1[2];
	pConstData[7] = m_uiSrc1[3];
	pConstData[8] = m_uiSrc2[0];
	pConstData[9] = m_uiSrc2[1];
	pConstData[10] = m_uiSrc2[2];
	pConstData[11] = m_uiSrc2[3];

	CalcResult();

	pConstData[12] = m_uiResult[0];
	pConstData[13] = m_uiResult[1];
	pConstData[14] = m_uiResult[2];
	pConstData[15] = m_uiResult[3];
	
	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_IArith_Trinary::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}

void CShader5xTest_IArith_Trinary::InitTestParameters()
{
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src2x = AddParameter( "Src2.x", &m_uiSrc2[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src2y = AddParameter( "Src2.y", &m_uiSrc2[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src2z = AddParameter( "Src2.z", &m_uiSrc2[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src2w = AddParameter( "Src2.w", &m_uiSrc2[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );
	testfactor::RFactor src2mod = AddParameter( "Src2Mod", &m_SrcMod[2], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;
	testfactor::RFactor src1 = src1x % src1y % src1z % src1w;
	testfactor::RFactor src2 = src2x % src2y % src2z % src2w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{src0, 1, NULL, 0},
		{src1, 1, NULL, 0},
		{src2, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
		{src2mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

void CShader5xTest_IArith_TrinaryNoNeg::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src2x = AddParameter( "Src2.x", &m_uiSrc2[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src2y = AddParameter( "Src2.y", &m_uiSrc2[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src2z = AddParameter( "Src2.z", &m_uiSrc2[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src2w = AddParameter( "Src2.w", &m_uiSrc2[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * ( src2x % src2y % src2z % src2w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );


}
	
TEST_RESULT CShader5xTest_IArith_DivTest::SetupTestCase()
{
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( ( tRes = CShader5xTest_IArith::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(3, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");
	asmHelper.SetupMovToTempFromLiteral(5, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");

	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 9, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::x, 9, AssemblyHelper::swizX);


	static tstring szTest = 
		"ieq r6, r4, r2"						"\n"
		"ieq r7, r5, r3"						"\n";

	static tstring szPackRes =
		"and r8.x, 0xF0000000, r6.x"			"\n"
		"and r8.y, 0x00F00000, r6.y"			"\n"
		"and r8.z, 0x0000F000, r6.z"			"\n"
		"and r8.w, 0x000000F0, r6.w"			"\n"

		"and r9.x, 0x0F000000, r7.x"			"\n"
		"and r9.y, 0x000F0000, r7.y"			"\n"
		"and r9.z, 0x00000F00, r7.z"			"\n"
		"and r9.w, 0x0000000F, r7.w"			"\n"

		"or r8.x, r8.x, r8.y"					"\n"
		"or r8.y, r8.z, r8.w"					"\n"

		"or r9.x, r9.x, r9.y"					"\n"
		"or r9.y, r9.z, r9.w"					"\n"
		
		"or r9.x, r9.x, r8.x"					"\n"
		"or r9.y, r9.y, r8.y"					"\n"
		"or r9.x, r9.x, r9.y"					"\n";	

	tstring instructionCode = GetInstruction() + szTest + szPackRes;

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);


	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}

	

	UINT32 pConstData[16];

	pConstData[0] = m_uiSrc0[0];
	pConstData[1] = m_uiSrc0[1];
	pConstData[2] = m_uiSrc0[2];
	pConstData[3] = m_uiSrc0[3];
	pConstData[4] = m_uiSrc1[0];
	pConstData[5] = m_uiSrc1[1];
	pConstData[6] = m_uiSrc1[2];
	pConstData[7] = m_uiSrc1[3];

	CalcResults( );

	pConstData[8] = m_uiResult0[0];
	pConstData[9] = m_uiResult0[1];
	pConstData[10] = m_uiResult0[2];
	pConstData[11] = m_uiResult0[3];
	pConstData[12] = m_uiResult1[0];
	pConstData[13] = m_uiResult1[1];
	pConstData[14] = m_uiResult1[2];
	pConstData[15] = m_uiResult1[3];
	
	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_IArith_DivTest::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}


TEST_RESULT CShader5xTest_IArith_DivTest::CheckPixel( BYTE Pixel[4][4] )
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
	tstring szFailQuotient = _T("Quotient calculation failed.\n");
	tstring szFailRemainder = _T("Remainder calculation failed.\n");

	UINT uiShader = 0;
	if( *((UINT32*)Pixel[uiShader]) != 0xFFFFFFFF )
	{
		if ( *((UINT32*)Pixel[uiShader]) == 0 )
		{
			WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailAll).c_str() );

			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				WriteToLog( 1, _T("Expected results for component %s: Quotient: 0x%X, Remainder: 0x%X\n"), 
					szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent], m_uiResult1[3-uiComponent] );
			}
		}
		else
		{
			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				if ( Pixel[uiShader][uiComponent] != 0xFF )
				{
					if ( Pixel[uiShader][uiComponent] == 0 )
					{
						WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent).c_str(), szComponents[uiComponent].c_str() );
						WriteToLog( 1, _T("Expected results for component %s: destQUOT: 0x%X, destREM: 0x%X\n"), 
							szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent], m_uiResult1[3-uiComponent] );
					}
					else
					{
						if ( ( Pixel[uiShader][uiComponent] & 0xF0 ) == 0 )
						{
							WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent + szFailQuotient).c_str(), szComponents[uiComponent].c_str() );
							WriteToLog( 1, _T("Expected results for component %s: destQUOT: 0x%X\n"), 
								szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent] );
						}
						else
						{
							WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent + szFailRemainder).c_str(), szComponents[uiComponent].c_str() );
							WriteToLog( 1, _T("Expected results for component %s: destREM: 0x%X\n"), 
								szComponents[uiComponent].c_str(), m_uiResult1[3-uiComponent] );
						}
					}
				}
			}
		}

		tRes = RESULT_FAIL;
	}

	return tRes;
}


TEST_RESULT CShader5xTest_IArith_MulTest::SetupTestCase()
{
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( ( tRes = CShader5xTest_IArith::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(3, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");
	asmHelper.SetupMovToTempFromLiteral(5, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");

	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 9, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::x, 9, AssemblyHelper::swizX);


	static tstring szTest = 
		"ieq r6, r4, r2"						"\n"
		"ieq r7, r5, r3"						"\n";

	static tstring szPackRes =
		"and r8.x, 0xF0000000, r6.x"			"\n"
		"and r8.y, 0x00F00000, r6.y"			"\n"
		"and r8.z, 0x0000F000, r6.z"			"\n"
		"and r8.w, 0x000000F0, r6.w"			"\n"

		"and r9.x, 0x0F000000, r7.x"			"\n"
		"and r9.y, 0x000F0000, r7.y"			"\n"
		"and r9.z, 0x00000F00, r7.z"			"\n"
		"and r9.w, 0x0000000F, r7.w"			"\n"

		"or r8.x, r8.x, r8.y"					"\n"
		"or r8.y, r8.z, r8.w"					"\n"

		"or r9.x, r9.x, r9.y"					"\n"
		"or r9.y, r9.z, r9.w"					"\n"
		
		"or r9.x, r9.x, r8.x"					"\n"
		"or r9.y, r9.y, r8.y"					"\n"
		"or r9.x, r9.x, r9.y"					"\n";	

	tstring instructionCode = GetInstruction() + szTest + szPackRes;

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);


	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}


	UINT32 pConstData[16];

	pConstData[0] = m_uiSrc0[0];
	pConstData[1] = m_uiSrc0[1];
	pConstData[2] = m_uiSrc0[2];
	pConstData[3] = m_uiSrc0[3];
	pConstData[4] = m_uiSrc1[0];
	pConstData[5] = m_uiSrc1[1];
	pConstData[6] = m_uiSrc1[2];
	pConstData[7] = m_uiSrc1[3];

	CalcResults( );

	pConstData[8] = m_uiResult0[0];
	pConstData[9] = m_uiResult0[1];
	pConstData[10] = m_uiResult0[2];
	pConstData[11] = m_uiResult0[3];
	pConstData[12] = m_uiResult1[0];
	pConstData[13] = m_uiResult1[1];
	pConstData[14] = m_uiResult1[2];
	pConstData[15] = m_uiResult1[3];

	
	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_IArith_MulTest::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}

TEST_RESULT CShader5xTest_IArith_MulTest::CheckPixel( BYTE Pixel[4][4] )
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
	tstring szFailHigh32 = _T("High 32 bit calculation failed.\n");
	tstring szFailLow32 = _T("Low 32 bit calculation failed.\n");

	UINT uiShader = 0;
	if( *((UINT32*)Pixel[uiShader]) != 0xFFFFFFFF )
	{
		if ( *((UINT32*)Pixel[uiShader]) == 0 )
		{
			WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailAll).c_str() );

			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				WriteToLog( 1, _T("Expected results for component %s: destHI: 0x%X, destLO: 0x%X\n"), 
					szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent], m_uiResult1[3-uiComponent] );
			}
		}
		else
		{
			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				if ( Pixel[uiShader][uiComponent] != 0xFF )
				{
					if ( Pixel[uiShader][uiComponent] == 0 )
					{
						WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent).c_str(), szComponents[uiComponent].c_str() );
						WriteToLog( 1, _T("Expected results for component %s: destHI: 0x%X, destLO: 0x%X\n"), 
							szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent], m_uiResult1[3-uiComponent] );
					}
					else
					{
						if ( ( Pixel[uiShader][uiComponent] & 0xF0 ) == 0 )
						{
							WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent + szFailHigh32).c_str(), szComponents[uiComponent].c_str() );
							WriteToLog( 1, _T("Expected results for component %s: destHI: 0x%X\n"), 
								szComponents[uiComponent].c_str(), m_uiResult0[3-uiComponent] );
						}
						else
						{
							WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailComponent + szFailLow32).c_str(), szComponents[uiComponent].c_str() );
							WriteToLog( 1, _T("Expected results for component %s: destLO: 0x%X\n"), 
								szComponents[uiComponent].c_str(), m_uiResult1[3-uiComponent] );
						}
					}
				}
			}
		}

		tRes = RESULT_FAIL;
	}
	
	return tRes;
}
	
tstring CShader5xTest_iadd::GetInstruction()
{
	static tstring szTest = "iadd r3, r0, r1"			"\n";
	static tstring szNeg1Test = "iadd r3, -r0, r1"		"\n";
	static tstring szNeg2Test = "iadd r3, r0, -r1"		"\n";
	static tstring szNeg12Test = "iadd r3, -r0, -r1"	"\n";

	if ( SMOD_NEG == m_SrcMod[0] )
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg12Test;
		}
		else
		{
			return szNeg1Test;
		}
	}
	else
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg2Test;
		}
		else
		{
			return szTest;
		}
	}
}
void CShader5xTest_iadd::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	//Take two's complement of any negated components
	if ( SMOD_NEG == m_SrcMod[0] )
		TwosComp( uiSrc0 );

	if ( SMOD_NEG == m_SrcMod[1] )
		TwosComp( uiSrc1 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = uiSrc0[i] + uiSrc1[i];
	}
}
	
tstring CShader5xTest_imad::GetInstruction()
{
	tstring szTest = "imad r4, ";
	
	if ( SMOD_NEG == m_SrcMod[0] )
	{
		szTest += (tstring)"-";
	}
	szTest += (tstring)"r0, ";

	if ( SMOD_NEG == m_SrcMod[1] )
	{
		szTest += (tstring)"-";
	}
	szTest += (tstring)"r1, ";

	if ( SMOD_NEG == m_SrcMod[2] )
	{
		szTest += (tstring)"-";
	}
	szTest += (tstring)"r2\n";
	
	return szTest;
}
void CShader5xTest_imad::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4], uiSrc2[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );
	memcpy( uiSrc2, m_uiSrc2, sizeof(UINT32) * 4 );

	//Take two's complement of any negated components
	if ( SMOD_NEG == m_SrcMod[0] )
		TwosComp( uiSrc0 );

	if ( SMOD_NEG == m_SrcMod[1] )
		TwosComp( uiSrc1 );

	if ( SMOD_NEG == m_SrcMod[2] )
		TwosComp( uiSrc2 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = (UINT32)(((INT64)(((INT64)uiSrc0[i] * (INT64)uiSrc1[i]) & 0xFFFFFFFF) + (INT64)uiSrc2[i]) & 0xFFFFFFFF);
	}
}
	
tstring CShader5xTest_imax::GetInstruction()
{
	static tstring szTest = "imax r3, r0, r1"			"\n";
	static tstring szNeg1Test = "imax r3, -r0, r1"		"\n";
	static tstring szNeg2Test = "imax r3, r0, -r1"		"\n";
	static tstring szNeg12Test = "imax r3, -r0, -r1"	"\n";

	if ( SMOD_NEG == m_SrcMod[0] )
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg12Test;
		}
		else
		{
			return szNeg1Test;
		}
	}
	else
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg2Test;
		}
		else
		{
			return szTest;
		}
	}
}
void CShader5xTest_imax::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );


	//Take two's complement of any negated components
	if ( SMOD_NEG == m_SrcMod[0] )
		TwosComp( uiSrc0 );

	if ( SMOD_NEG == m_SrcMod[1] )
		TwosComp( uiSrc1 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = (INT32)uiSrc0[i] > (INT32)uiSrc1[i]? uiSrc0[i]: uiSrc1[i];
	}
}

	
tstring CShader5xTest_imin::GetInstruction()
{
	static tstring szTest = "imin r3, r0, r1"			"\n";
	static tstring szNeg1Test = "imin r3, -r0, r1"		"\n";
	static tstring szNeg2Test = "imin r3, r0, -r1"		"\n";
	static tstring szNeg12Test = "imin r3, -r0, -r1"	"\n";

	if ( SMOD_NEG == m_SrcMod[0] )
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg12Test;
		}
		else
		{
			return szNeg1Test;
		}
	}
	else
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			return szNeg2Test;
		}
		else
		{
			return szTest;
		}
	}
}
void CShader5xTest_imin::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );


	//Take two's complement of any negated components
	if ( SMOD_NEG == m_SrcMod[0] )
		TwosComp( uiSrc0 );

	if ( SMOD_NEG == m_SrcMod[1] )
		TwosComp( uiSrc1 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = (INT32)uiSrc0[i] < (INT32)uiSrc1[i]? uiSrc0[i]: uiSrc1[i];
	}
}

	
void CShader5xTest_imul::InitTestParameters()
{
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );

	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	testfactor::RFactor src0nul = AddParameter( "Src0Null", &m_bNULLSrc[0], BoolValueSet() );
	testfactor::RFactor src1nul = AddParameter( "Src1Null", &m_bNULLSrc[1], BoolValueSet() );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize pairwise to reduce the number of testcases
	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{src0nul, 1, NULL, 0},
		{src1nul, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;
	testfactor::RFactor src1 = src1x % src1y % src1z % src1w;
	testfactor::RFactor rfTestCases = src0 * src1 * NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src0Null"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src1Null"), false), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

tstring CShader5xTest_imul::GetInstruction()
{
	tstring szInstruction;

	static tstring szNoNullTest = "imul r4, r5, ";
	static tstring szNull1Test = "imul NULL, r5, ";
	static tstring szNull2Test = "imul r4, NULL, ";
	static tstring szNull12Test = "imul NULL, NULL, ";

	static tstring szNoNegTest = "r0, r1"		"\n";
	static tstring szNeg1Test = "-r0, r1"		"\n";
	static tstring szNeg2Test = "r0, -r1"		"\n";
	static tstring szNeg12Test = "-r0, -r1"		"\n";

	if ( m_bNULLSrc[0] )
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull12Test;
		}
		else
		{
			szInstruction = szNull1Test;
		}
	}
	else
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull2Test;
		}
		else
		{
			szInstruction = szNoNullTest;
		}
	}

	if ( SMOD_NEG == m_SrcMod[0] )
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			szInstruction += szNeg12Test;
		}
		else
		{
			szInstruction += szNeg1Test;
		}
	}
	else
	{
		if ( SMOD_NEG == m_SrcMod[1] )
		{
			szInstruction += szNeg2Test;
		}
		else
		{
			szInstruction += szNoNegTest;
		}
	}

	return szInstruction;
}
void CShader5xTest_imul::CalcResults( )
{
	INT64 i64Res;
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	//Take two's complement of any negated components
	if ( SMOD_NEG == m_SrcMod[0] )
		TwosComp( uiSrc0 );

	if ( SMOD_NEG == m_SrcMod[1] )
		TwosComp( uiSrc1 );

	for ( UINT i = 0; i < 4; ++i )
	{
		i64Res = (INT64)(INT32)uiSrc0[i] * (INT64)(INT32)uiSrc1[i];

		if ( m_bNULLSrc[0] )
			m_uiResult0[i] = 0;
		else
			m_uiResult0[i] = (UINT32)(i64Res >> 32);

		if ( m_bNULLSrc[1] )
		{
			m_uiResult1[i] = 0;
		}
		else
		{
			m_uiResult1[i] = (UINT32)(0xFFFFFFFF & i64Res);
		}
	}
}
	
tstring CShader5xTest_ineg::GetInstruction()
{
	static tstring szTest = 
		"ineg r2, r0"			"\n";
	
	return szTest;
}
void CShader5xTest_ineg::CalcResult()
{
	UINT32 uiSrc[4];

	memcpy( uiSrc, m_uiSrc, sizeof(UINT32) * 4 );

	//Take two's complement
	TwosComp( uiSrc );

	memcpy( m_uiResult, uiSrc, sizeof(UINT32) * 4 );
}
	
void CShader5xTest_udiv::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src0nul = AddParameter( "Src0Null", &m_bNULLSrc[0], BoolValueSet() );
	testfactor::RFactor src1nul = AddParameter( "Src1Null", &m_bNULLSrc[1], BoolValueSet() );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0nul * src1nul * ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src0Null"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src1Null"), false), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

tstring CShader5xTest_udiv::GetInstruction()
{
	tstring szInstruction;

	static tstring szNoNullTest = "udiv r4, r5, ";
	static tstring szNull1Test = "udiv NULL, r5, ";
	static tstring szNull2Test = "udiv r4, NULL, ";
	static tstring szNull12Test = "udiv NULL, NULL, ";

	static tstring szNoNegTest = "r0, r1"		"\n";

	if ( m_bNULLSrc[0] )
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull12Test;
		}
		else
		{
			szInstruction = szNull1Test;
		}
	}
	else
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull2Test;
		}
		else
		{
			szInstruction = szNoNullTest;
		}
	}
	
	szInstruction += szNoNegTest;

	return szInstruction;
}

void CShader5xTest_udiv::CalcResults( )
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	if ( m_bNULLSrc[0] )
	{
		memset( m_uiResult0, 0, sizeof(UINT32) * 4 );
	}
	else
	{
		for ( UINT i = 0; i < 4; ++i )
		{
			if ( 0 == uiSrc1[i] )
			{
				m_uiResult0[i] = 0xFFFFFFFF;
			}
			else
			{
				m_uiResult0[i] = uiSrc0[i] / uiSrc1[i];
			}
		}
	}
		
	if ( m_bNULLSrc[1] )
	{
		memset( m_uiResult1, 0, sizeof(UINT32) * 4 );
	}
	else
	{
		for ( UINT i = 0; i < 4; ++i )
		{
			if ( 0 == uiSrc1[i] )
			{
				m_uiResult1[i] = 0xFFFFFFFF;
			}
			else
			{
				m_uiResult1[i] = uiSrc0[i] % uiSrc1[i];
			}
		}
	}
}
	
tstring CShader5xTest_umad::GetInstruction()
{
	tstring szTest = "umad r4, r0, r1, r2\n";
	
	return szTest;
}
void CShader5xTest_umad::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4], uiSrc2[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );
	memcpy( uiSrc2, m_uiSrc2, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = (UINT32)( ( (UINT64)uiSrc0[i] * uiSrc1[i] ) & 0xFFFFFFFF ) + uiSrc2[i];
	}
}
	
tstring CShader5xTest_umax::GetInstruction()
{
	static tstring szTest = "umax r3, r0, r1"			"\n";

	return szTest;
}
void CShader5xTest_umax::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = uiSrc0[i] > uiSrc1[i]? uiSrc0[i]: uiSrc1[i];
	}
}
	
tstring CShader5xTest_umin::GetInstruction()
{
	static tstring szTest = "umin r3, r0, r1"			"\n";

	return szTest;
}
void CShader5xTest_umin::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		m_uiResult[i] = uiSrc0[i] < uiSrc1[i]? uiSrc0[i]: uiSrc1[i];
	}
}
	
void CShader5xTest_umul::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src0nul = AddParameter( "Src0Null", &m_bNULLSrc[0], BoolValueSet() );
	testfactor::RFactor src1nul = AddParameter( "Src1Null", &m_bNULLSrc[1], BoolValueSet() );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0nul * src1nul * ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w );
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src0Null"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("Src1Null"), false), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

tstring CShader5xTest_umul::GetInstruction()
{
	tstring szInstruction;

	static tstring szNoNullTest = "umul r4, r5, ";
	static tstring szNull1Test = "umul NULL, r5, ";
	static tstring szNull2Test = "umul r4, NULL, ";
	static tstring szNull12Test = "umul NULL, NULL, ";

	static tstring szNoNegTest = "r0, r1"		"\n";

	if ( m_bNULLSrc[0] )
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull12Test;
		}
		else
		{
			szInstruction = szNull1Test;
		}
	}
	else
	{
		if ( m_bNULLSrc[1] )
		{
			szInstruction = szNull2Test;
		}
		else
		{
			szInstruction = szNoNullTest;
		}
	}

	szInstruction += szNoNegTest;

	return szInstruction;
}
void CShader5xTest_umul::CalcResults( )
{
	UINT64 ui64Res;
	UINT32 uiSrc0[4], uiSrc1[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		ui64Res = (UINT64)uiSrc0[i] * (UINT64)uiSrc1[i];

		if ( m_bNULLSrc[0] )
			m_uiResult0[i] = 0;
		else
			m_uiResult0[i] = (UINT32)(ui64Res >> 32);

		if ( m_bNULLSrc[1] )
		{
			m_uiResult1[i] = 0;
		}
		else
		{
			m_uiResult1[i] = (UINT32)(0xFFFFFFFF & ui64Res);
		}
	}
}

TEST_RESULT CShader5xTest_IArith_StatusBit::Setup()
{
	TEST_RESULT tr = CShader5xTest_IArith_Trinary::Setup();
	if ( tr != RESULT_PASS )
		return tr;

	if ( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog(_T("Skip test group on feature levels less than 11.0."));
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}

void CShader5xTest_IArith_StatusBit::InitTestParameters()
{
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_uiSrc0[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_uiSrc0[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_uiSrc0[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_uiSrc0[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_uiSrc1[0], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_uiSrc1[1], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_uiSrc1[2], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_uiSrc1[3], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );

	testfactor::RFactor src2x = AddParameter( "Src2.x", &m_uiSrc2[0], new CTableValueSet< UINT32 >( &UINTValuesZ[0], sizeof( UINTValuesZ[0] ), sizeof( UINTValuesZ ) / sizeof( UINTValuesZ[0] ) ) );
	testfactor::RFactor src2y = AddParameter( "Src2.y", &m_uiSrc2[1], new CTableValueSet< UINT32 >( &UINTValuesW[0], sizeof( UINTValuesW[0] ), sizeof( UINTValuesW ) / sizeof( UINTValuesW[0] ) ) );
	testfactor::RFactor src2z = AddParameter( "Src2.z", &m_uiSrc2[2], new CTableValueSet< UINT32 >( &UINTValuesX[0], sizeof( UINTValuesX[0] ), sizeof( UINTValuesX ) / sizeof( UINTValuesX[0] ) ) );
	testfactor::RFactor src2w = AddParameter( "Src2.w", &m_uiSrc2[3], new CTableValueSet< UINT32 >( &UINTValuesY[0], sizeof( UINTValuesY[0] ), sizeof( UINTValuesY ) / sizeof( UINTValuesY[0] ) ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor statusBit = AddParameter( "TestStatusBit", &m_bTestStatusBit, new CBoolValueSet() );

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = ( ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * ( src2x % src2y % src2z % src2w ) ) % statusBit;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("TestStatusBit"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

tstring CShader5xTest_uaddc::GetInstruction()
{
	tstring szTest;
	if( m_bTestStatusBit )
	{
		szTest = "uaddc r0, r4, r1, r2\n";
	}
	else
	{
		szTest = "uaddc r4, r0, r1, r2\n";
	}
	
	return szTest;
}

void CShader5xTest_uaddc::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4], uiSrc2[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );
	memcpy( uiSrc2, m_uiSrc2, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		if( m_bTestStatusBit )
		{
			m_uiResult[i] = ((INT64)uiSrc1[i] + (INT64)uiSrc2[i]) > 0xFFFFFFFF;	
		}
		else
		{
			m_uiResult[i] = (UINT32)((INT64)(((INT64)uiSrc1[i] + (INT64)uiSrc2[i]) & 0xFFFFFFFF));
		}
	}
}

tstring CShader5xTest_usubb::GetInstruction()
{
	tstring szTest = "usubb r4, r0, r1, r2\n";
	if( m_bTestStatusBit )
	{
		szTest = "usubb r0, r4, r1, r2\n";
	}
	else
	{
		szTest = "usubb r4, r0, r1, r2\n";
	}
	
	return szTest;
}

void CShader5xTest_usubb::CalcResult()
{
	UINT32 uiSrc0[4], uiSrc1[4], uiSrc2[4];

	memcpy( uiSrc0, m_uiSrc0, sizeof(UINT32) * 4 );
	memcpy( uiSrc1, m_uiSrc1, sizeof(UINT32) * 4 );
	memcpy( uiSrc2, m_uiSrc2, sizeof(UINT32) * 4 );

	for ( UINT i = 0; i < 4; ++i )
	{
		if( m_bTestStatusBit )
		{
			m_uiResult[i] = uiSrc1[i] < uiSrc2[i];
		}
		else
		{
			m_uiResult[i] = (UINT32)((INT64)(((INT64)uiSrc1[i] - (INT64)uiSrc2[i]) & 0xFFFFFFFF));
		}
	}
}
