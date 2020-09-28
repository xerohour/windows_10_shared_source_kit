//////////////////////////////////////////////////////////////////////////
//  SumAbsDiff.cpp
//  created:	5/10/2011
//
//  purpose:    Test Masked Sum of Absolute Differences (msad) and any 
//              future similar instructions.
//              These are typically used for video compression and
//              pattern matching. 
//////////////////////////////////////////////////////////////////////////

#include "SumAbsDiff.h"

static UINT32 UINTValuesX[] =
{
	0x00000000,
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x88888888,
	0x99999999,
	0xAAAAAAAA,
	0xBBBBBBBB,
	0xCCCCCCCC,
	0xDDDDDDDD,
	0xEEEEEEEE,
	0xFFFFFFFF,
};
static UINT32 UINTValuesY[] =
{
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x88888888,
	0x99999999,
	0xAAAAAAAA,
	0xBBBBBBBB,
	0xCCCCCCCC,
	0xDDDDDDDD,
	0xEEEEEEEE,
	0xFFFFFFFF,
	0x00000000,
	0x11111111,
	0x22222222,
	0x33333333,
};
static UINT32 UINTValuesZ[] =
{
	0x88888888,
	0x99999999,
	0xAAAAAAAA,
	0xBBBBBBBB,
	0xCCCCCCCC,
	0xDDDDDDDD,
	0xEEEEEEEE,
	0xFFFFFFFF,
	0x00000000,
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
};
static UINT32 UINTValuesW[] =
{
	0xDDDDDDDD,
	0xEEEEEEEE,
	0xFFFFFFFF,
	0x00000000,
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x88888888,
	0x99999999,
	0xAAAAAAAA,
	0xBBBBBBBB,
	0xCCCCCCCC,
};

void CShader5xTest_SumAbsDiff::InitTestParameters()
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

TEST_RESULT CShader5xTest_SumAbsDiff::SetupTestCase()
{
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT		hr = S_OK;
    TEST_RESULT tRes =  RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false )
		return RESULT_SKIP;

	if ( ( tRes = CShader5xTest::SetupTestCase() ) != RESULT_PASS )
		return tRes;

	m_pCurrentUAView = m_pRTUAView;

	AssemblyHelper asmHelper(16, 200, m_RTFormat, m_viewPort);
	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	else
		asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);

	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);

	asmHelper.EnableShaderExtensionsSupport();

	if ( m_pCurrentUAView == m_pRTUABufferView )
		asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 5, AssemblyHelper::swizXYZW);
	else
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 5, AssemblyHelper::swizXYZW);

	m_InstructionCode = GetInstruction();

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			m_InstructionCode.c_str(), &m_pRTView, &m_pCurrentUAView, 1, &pShaderBuf,
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
	pConstData[8] = m_uiSrc2[0];
	pConstData[9] = m_uiSrc2[1];
	pConstData[10] = m_uiSrc2[2];
	pConstData[11] = m_uiSrc2[3];

	CalcResult();

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pConstData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_SumAbsDiff_Binary::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
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

TEST_RESULT CShader5xTest_SumAbsDiff::ExecuteTestCase()
{
	TEST_RESULT		tRes = RESULT_PASS;
	BYTE			Pixel[4][4] = {0};
	HRESULT			hr = S_OK;
	FLOAT			ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT			ClearColorU[4] = {0, 0, 0, 0};
	
	GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pCurrentUAView, ClearColorU);

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
		hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, Pixel);
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CShader5xTest_SumAbsDiff::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	else
	{
		hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, Pixel);
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CShader5xTest_SumAbsDiff::ExecuteTestCase() - failed to map m_pRTTexture2DCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	tRes = CheckPixel( Pixel );

	return tRes;
}

void CShader5xTest_SumAbsDiff::CleanupTestCase()
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

	m_pCurrentUAView = NULL;

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pHS );
	SAFE_RELEASE( m_pDS );
	SAFE_RELEASE( m_pCS );
}

//////////////////////////////////////////////////////////////////////////
// MSAD (Masked Sum of Absolute Differences) - pattern matching
//////////////////////////////////////////////////////////////////////////
tstring CShader5xTest_msad::GetInstruction()
{
	tstring szTest = "msad r5, r0, r1, r2\n";
	
	return szTest;
}

void CShader5xTest_msad::CalcResult()
{
	BYTE   uiRef[4] = {0};
	BYTE   uiSrc[4] = {0};
	UINT64 uiAccum = 0;

	for ( UINT i = 0; i < 4; ++i )
	{
		*(UINT32*)uiRef = m_uiSrc0[i];
		*(UINT32*)uiSrc = m_uiSrc1[i];
		uiAccum = m_uiSrc2[i];

		for ( UINT j = 0; j < 4; j++ )
		{
			if ( uiRef[j] > 0 )
			{
				if ( uiRef[j] >= uiSrc[j] )
				{ 
					uiAccum += (UINT64)(uiRef[j] - uiSrc[j]);
				}
				else
				{ 
					uiAccum += (UINT64)(uiSrc[j] - uiRef[j]);
				}
			}
		}

		if ( uiAccum > UINT_MAX )
		{
			m_uiResult[i] = UINT_MAX;
		}
		else
		{
			m_uiResult[i] = (UINT32)uiAccum;
		}
	}
}

TEST_RESULT CShader5xTest_msad::CheckPixel( BYTE Pixel[4][4] )
{
	TEST_RESULT tRes = RESULT_PASS;
	tstring szComponents[4] = 
	{
		_T("w"),
		_T("z"),
		_T("y"),
		_T("x"),
	};

	tstring szFailAll = _T("CShader5xTest_SumAbsDiff_msad::CheckPixel: instruction failed\n");

	for ( UINT i = 0; i < 4; i++ )
	{
		if ( *(UINT32*)Pixel[i] == m_uiResult[i] ) // If overflow and 32-bit saturation occurred, result will be UINT_MAX (0xFFFFFFFF) and will match here.
			continue;
		else
		if ( m_uiResult[i] == UINT_MAX  	// Overflow occurred and result undefined.  Can be either USHRT_MAX (0x0000FFFF) or 0x0000????.
			&& ( *(UINT32*)Pixel[i] & 0xFFFF0000 ) == 0 )
			continue;
		else
		{
			tRes = RESULT_FAIL;
			WriteToLog( 1, ( ToString( m_ShaderStage ) + szFailAll).c_str() );
			
			for ( UINT uiComponent = 0; uiComponent < 4; ++uiComponent )
			{
				WriteToLog( 1, _T("Expected results for component %s: 0x%X\n"), 
					szComponents[uiComponent].c_str(), m_uiResult[3 - uiComponent] );
				WriteToLog( 1, _T("Actual results                   : 0x%X\n"), 
					*(UINT32*)Pixel[3 - uiComponent] );
			}
		}
	}

	return tRes;
}
