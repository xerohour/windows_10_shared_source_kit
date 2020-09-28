//////////////////////////////////////////////////////////////////////////
//  Condition.cpp
//  created:	2005/03/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

// BUGBUG: still need to test 16bit floats

#include <float.h>
#include "bitwise.h"

UINT BitwiseValues[] =
{
	0x00000000,
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000080,
	0x0000FFFF,
	0x00010000,
	0x11111111,
	0x80000000,
	0xAAAAAAAA,
	0xbaadf00d,
	0xFFFFFFFF,
};

UINT ShiftValues[] =
{
	0x00000000,
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x0000000F,
	0x00000010,
	0x0000001F,
	0x00000020,
	0xFFFFFFFF,
};

UINT BitfieldWidthValues[] =
{
	0,
	1,
	2,
	10,
	20,
	30,
	31,
	32,	// Should be clamped @ 31. Valid range [0,31]
	33,
};

UINT BitfieldOffsetValues[] =
{
	0,
	1,
	8,
	9,
	16,
	17,
	30,
	31,
	32,	// Should be clamped @ 31. Valid range [0,31]
	33,
};

UINT BitfieldInsertExtractValues[] =
{
	0x00000000,
	0x00000001,
	0x80000001,
	0x00018000,
	0xAAAAAAAA,
	0xFFFFFFFF,
};

// This controls by how much each swizzle value differs from the "original" value
UINT g_bitfieldSwizzleOffsets[] =
{
	0,	// Note this is ZERO becuase we want to test the original value!
	1,
	-1,
	0x80,
};

UINT FirstBitValues[] =
{
	0x00000000,
	0x00000001, 
	0x00000002, 
	0x00000004, 
	0x00000008,
	0x80000000, 
	0x40000000, 
	0x20000000, 
	0x10000000,
	0x41000000, 
	0x01800000, 
	0x00018000, 
	0xFFFFFFFF,
	0x00000003,
	0xAAAAAAAA, 
	0x0AAAAAA0, 
	0x00AAAA00, 
	0x000AA000,
	0xBAADF00D, 
	0xDEADB175, 
	0x0FFFFFF0, 
	0x00DEAD00, 
	0x10000008, 
	0x80000001, 
	0x00018000, 
	0x00024000
};

TEST_RESULT CShader5xTest_Bitwise::SetPipeline(const char *szDest, const char *szSrc[2])
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	ID3D10Blob *pShaderBuf = NULL;
	
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	// copy inputs to r7 & r8
	asmHelper.SetupMovToTempFromCB(7, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(8, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	// output from r6 to o[0]
	asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 6, AssemblyHelper::swizXYZW);

	tstring instructionCode = FormatString("%s %s%s%s\n",
		GetOpCode(), szDest, szSrc[0], szSrc[1]);

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode, &m_pRTBufferView, &m_pRTUABufferView, 1, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

TEST_RESULT CShader5xTest_Bitwise::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;

	//
	// Set the constant buffer data
	//
	BitwiseShaderParams pData[1];

	pData[0].src0[0] = m_Src[0];
	pData[0].src0[1] = m_Src[0];
	pData[0].src0[2] = m_Src[0];
	pData[0].src0[3] = m_Src[0];

	pData[0].src1[0] = m_Src[1];
	pData[0].src1[1] = m_Src[1];
	pData[0].src1[2] = m_Src[1];
	pData[0].src1[3] = m_Src[1];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_Bitwise::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	
	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	else
		GetEffectiveContext()->Draw( 1, 0 );

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// Lock the render target and examine the results
	UINT result;
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_Bitwise::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	UINT calcResult = CalcResult( m_Src[0], m_Src[1] );
	if ( calcResult != result )
	{
		WriteToLog( 1, "Expected %x : result %x.", calcResult, result );
		tr = RESULT_FAIL;
	}

	return tr;
}

void CShader5xTest_Bitwise::CleanupTestCase()
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
}

//////////////////////////////////////////////////////////////////////////
// Shift Bitwise Instructions
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_ShiftBitwise::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter( "Src1", &m_Src[1], new CTableValueSet< UINT >( &ShiftValues[0], sizeof( ShiftValues[0] ), sizeof( ShiftValues ) / sizeof( ShiftValues[0] ) ) );
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0 * src1;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

TEST_RESULT CShader5xTest_ShiftBitwise::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Bitwise::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6";
	const char *szSrc[2];
	
	szSrc[0] = ", r7";
	szSrc[1] = ", r8.x";
		
	return SetPipeline(szDest, szSrc);
}



//////////////////////////////////////////////////////////////////////////
// ISHL
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_ishl::CalcResult(UINT src0, UINT src1)
{
	const UINT uLSB5bitsOfSrc = src1 & 0x1F;
	return src0 << uLSB5bitsOfSrc;
}

//////////////////////////////////////////////////////////////////////////
// ISHR
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_ishr::CalcResult(UINT src0, UINT src1)
{
	const UINT uLSB5bitsOfSrc = src1 & 0x1F;
	return (int)src0 >> (int)uLSB5bitsOfSrc;
}

//////////////////////////////////////////////////////////////////////////
// USHR
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_ushr::CalcResult(UINT src0, UINT src1)
{
	const UINT uLSB5bitsOfSrc = src1 & 0x1F;
	return src0 >> uLSB5bitsOfSrc;
}


//////////////////////////////////////////////////////////////////////////
// Non Shift Bitwise Instructions (And, Or, XOr)
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_NonShiftBitwise::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter( "Src1", &m_Src[1], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0 * src1;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

TEST_RESULT CShader5xTest_NonShiftBitwise::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Bitwise::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6";
	const char *szSrc[2];
	
	szSrc[0] = ", r7";
	szSrc[1] = ", r8";
		
	return SetPipeline(szDest, szSrc);
}



//////////////////////////////////////////////////////////////////////////
// AND
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_and::CalcResult(UINT src0, UINT src1)
{
	return src0 & src1;
}

//////////////////////////////////////////////////////////////////////////
// OR
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_or::CalcResult(UINT src0, UINT src1)
{
	return src0 | src1;
}

//////////////////////////////////////////////////////////////////////////
// XOR
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_xor::CalcResult(UINT src0, UINT src1)
{
	return src0 ^ src1;
}

//////////////////////////////////////////////////////////////////////////
// NOT
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_not::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor src0 = AddParameter( "Src", &m_Src[0], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );
}

TEST_RESULT CShader5xTest_not::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Bitwise::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6";
	const char *szSrc[2];
	
	szSrc[0] = ", r7";
	szSrc[1] = "";
		
	return SetPipeline(szDest, szSrc);
}

UINT CShader5xTest_not::CalcResult(UINT src0, UINT src1)
{
	return ~src0;
}

//////////////////////////////////////////////////////////////////////////
// Bitfield Instructions (ubfe, ibfe, bfi, bfrev)
//////////////////////////////////////////////////////////////////////////

TEST_RESULT CShader5xTest_Bitfield::SetPipeline(const char *szDest, const char *szSrc[4])
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	ID3D10Blob *pShaderBuf = NULL;
	
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	
	// copy inputs to r7, r8, r9, r10...
	asmHelper.SetupMovToTempFromCB(7, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(8, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(9, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(10, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW);

	// output from r6 to o[0]
	asmHelper.SetupMovToTempFromLiteral(6, AssemblyHelper::xyzw, "l(0,0,0,0)");
	asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 6, AssemblyHelper::swizXYZW);

	tstring instructionCode;
	switch( GetNumSrcRegisters() )
	{
	case 1:
		instructionCode = FormatString("%s %s.%s%s.%s\n",
			GetOpCode(), 
			szDest, ToString(m_destMask).c_str(), 
			szSrc[0], ToString(m_src0Swizzle).c_str());
		break;
	case 3:
		instructionCode = FormatString("%s %s.%s%s.%s%s.%s%s.%s\n",
			GetOpCode(), 
			szDest, ToString(m_destMask).c_str(), 
			szSrc[0], ToString(m_src0Swizzle).c_str(),
			szSrc[1], ToString(m_src1Swizzle).c_str(),
			szSrc[2], ToString(m_src2Swizzle).c_str());
		break;
	case 4:
		instructionCode = FormatString("%s %s.%s%s.%s%s.%s%s.%s%s.%s\n",
			GetOpCode(), 
			szDest, ToString(m_destMask).c_str(), 
			szSrc[0], ToString(m_src0Swizzle).c_str(),
			szSrc[1], ToString(m_src1Swizzle).c_str(),
			szSrc[2], ToString(m_src2Swizzle).c_str(),
			szSrc[3], ToString(m_src3Swizzle).c_str());
		break;
	default:
		WriteToLog( _T("[CShader5xTest_Bitfield::SetPipeline] Error, unknown number of SRC registers: %d"), GetNumSrcRegisters());
		tRes = RESULT_FAIL;
		goto setupDone;
	};

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode, &m_pRTBufferView, &m_pRTUABufferView, 1, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "CShader5xTest_Bitfield::SetPipeline(): SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
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
	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

void CShader5xTest_Bitfield::InitTestParameters()
{
	// Local variables
	testfactor::RFactor rfTestCases;
	testfactor::RFactor rParameters;
	testfactor::RFactor rParameterSwizzles;
	testfactor::RFactor rParameterMaskSwizzles;
	testfactor::RFactor src0;
	testfactor::RFactor src1;
	testfactor::RFactor src2;
	testfactor::RFactor src3;
	testfactor::RFactor destMask;
	testfactor::RFactor src0Swizzle;
	testfactor::RFactor src1Swizzle;
	testfactor::RFactor src2Swizzle;
	testfactor::RFactor src3Swizzle;

	CTableValueSet<AssemblyHelper::MaskFlag>	*pMaskValues = new CTableValueSet<AssemblyHelper::MaskFlag>( g_pMasks, sizeof( g_pMasks[ 0 ] ), ARRAYSIZE( g_pMasks ) );
	CTableValueSet<AssemblyHelper::Swizzle>		*pSwizzleValues = new CTableValueSet<AssemblyHelper::Swizzle>( g_pSwizzles, sizeof( g_pSwizzles[ 0 ] ), ARRAYSIZE( g_pSwizzles ) );

	// Create the swizzle and mask factors from the global sets
	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDestMaskParam = AddParameter( _T( "DestMask" ), &m_destMask );
	SetParameterDefaultValue( pDestMaskParam, AssemblyHelper::xyzw );
	destMask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pDestMaskParam, pMaskValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc0SwizzleParam = AddParameter( _T( "Src0Swizzle" ), &m_src0Swizzle );
	SetParameterDefaultValue( pSrc0SwizzleParam, AssemblyHelper::swizXYZW );
	src0Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc0SwizzleParam, pSwizzleValues );

	rParameterMaskSwizzles = src0Swizzle % destMask;

	if( GetNumSrcRegisters() == 1 )
	{
		// FIRSTBIT's
		tstring opCode(GetOpCode());

		if(  opCode == "firstbit_lo" || opCode == "firstbit_hi" ||  opCode == "firstbit_shi")
			src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< UINT >( &FirstBitValues[0], sizeof( FirstBitValues[0] ), sizeof( FirstBitValues ) / sizeof( FirstBitValues[0] ) ) );
		else // BREV, COUNTBITS
			src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );

		rParameters = src0;
		rParameterMaskSwizzles = destMask + src0Swizzle + (src0Swizzle % destMask);
		rfTestCases = rParameters * rParameterMaskSwizzles;

		DeclarePriorityLevelByPriorityValue( 1, 2.0f );
		DeclarePriorityLevelByPriorityValue( 2, 1.0f );
	}
	else
	{
		// UBFE, IBFE
		src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< UINT >( &BitfieldWidthValues[0], sizeof( BitfieldWidthValues[0] ), sizeof( BitfieldWidthValues ) / sizeof( BitfieldWidthValues[0] ) ) );
		src1 = AddParameter( "Src1", &m_Src[1], new CTableValueSet< UINT >( &BitfieldOffsetValues[0], sizeof( BitfieldOffsetValues[0] ), sizeof( BitfieldOffsetValues ) / sizeof( BitfieldOffsetValues[0] ) ) );
		src2 = AddParameter( "Src2", &m_Src[2], new CTableValueSet< UINT >( &BitwiseValues[0], sizeof( BitwiseValues[0] ), sizeof( BitwiseValues ) / sizeof( BitwiseValues[0] ) ) );

		CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc1SwizzleParam = AddParameter( _T( "Src1Swizzle" ), &m_src1Swizzle );
		SetParameterDefaultValue( pSrc1SwizzleParam, AssemblyHelper::swizXYZW );
		src1Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc1SwizzleParam, pSwizzleValues );

		CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc2SwizzleParam = AddParameter( _T( "Src2Swizzle" ), &m_src2Swizzle );
		SetParameterDefaultValue( pSrc2SwizzleParam, AssemblyHelper::swizXYZW );
		src2Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc2SwizzleParam, pSwizzleValues );

		AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src1Swizzle"), g_pSwizzles[0]), WeightFactorVeryLarge);
		AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src2Swizzle"), g_pSwizzles[0]), WeightFactorVeryLarge);

		if( 4 == GetNumSrcRegisters())
		{
			// BFI
			src3 = AddParameter( "Src3", &m_Src[3], new CTableValueSet< UINT >( &BitfieldInsertExtractValues[0], sizeof( BitfieldInsertExtractValues[0] ), sizeof( BitfieldInsertExtractValues ) / sizeof( BitfieldInsertExtractValues[0] ) ) );
		
			CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc3SwizzleParam = AddParameter( _T( "Src3Swizzle" ), &m_src3Swizzle );
			SetParameterDefaultValue( pSrc3SwizzleParam, AssemblyHelper::swizXYZW );
			src3Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc3SwizzleParam, pSwizzleValues );

			AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src3Swizzle"), g_pSwizzles[0]), WeightFactorVeryLarge);

			// Use pairwise factors for swizzles and masks
			testfactor::XFACTOR srcPairwiseTable[] = 
			{
				// Factor, Group, pNotGrps, cNotGrps
				{src0, 1, NULL, 0},
				{src1, 1, NULL, 0},
				{src2, 1, NULL, 0},
				{src3, 1, NULL, 0},
			};

			size_t groupOrders[] = 
			{
				XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
			};

			// Use pairwise factors for swizzles and masks
			testfactor::XFACTOR swizzleMaskPairwiseTable[] = 
			{
				{destMask, 1, NULL, 0},
				{src0Swizzle, 1, NULL, 0},
				{src1Swizzle, 1, NULL, 0},
				{src2Swizzle, 1, NULL, 0},
				{src3Swizzle, 1, NULL, 0},
			};

			size_t swizzleMaskgroupOrders[] = 
			{
				XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
			};

			testfactor::RFactor rfSrcs = NewCrossFactor(srcPairwiseTable, _countof(srcPairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
			testfactor::RFactor rfSwizzleMasks = NewCrossFactor(swizzleMaskPairwiseTable, _countof(swizzleMaskPairwiseTable), "SwizzleMaskPairwiseTable", swizzleMaskgroupOrders, _countof(swizzleMaskgroupOrders));
			rfTestCases = rfSrcs % rfSwizzleMasks;

			DeclarePriorityLevelByPriorityValue( 1, 4.0f );
			DeclarePriorityLevelByPriorityValue( 2, 2.0f );
		}
		else
		{
			// Use pairwise factors for swizzles and masks
			testfactor::XFACTOR parameterPairwiseTable[] = 
			{
				// Factor, Group, pNotGrps, cNotGrps
				{src0, 0, NULL, 0},
				{src1, 0, NULL, 0},
				{src2, 0, NULL, 0},

				{destMask, 1, NULL, 0},
				{src0Swizzle, 1, NULL, 0},
				{src1Swizzle, 1, NULL, 0},
				{src2Swizzle, 1, NULL, 0},
			};

			size_t groupOrders[] = 
			{
				XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
			};

			rfTestCases = NewCrossFactor(parameterPairwiseTable, _countof(parameterPairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));

			DeclarePriorityLevelByPriorityValue( 1, 4.0f );
			DeclarePriorityLevelByPriorityValue( 2, 2.0f );
		}
	}

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	// Note: Some Priority Pattern Filters and Priority Values are added above
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), g_pMasks[0]), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src0Swizzle"), g_pSwizzles[0]), WeightFactorVeryLarge);
}

TEST_RESULT CShader5xTest_Bitfield::Setup()
{
	// Bitfields are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Bitfield instructions are only for Feature Level 11 or greater.") );
		return RESULT_SKIP;
	}

	return CShader5xTest::Setup();
}

TEST_RESULT CShader5xTest_Bitfield::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6";
	const char *szSrc[4];
	
	szSrc[0] = ", r7";
	szSrc[1] = ", r8";
	szSrc[2] = ", r9";
	szSrc[3] = ", r10";
		
	return SetPipeline(szDest, szSrc);
}

TEST_RESULT CShader5xTest_Bitfield::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;

	//
	// Set the constant buffer data
	//
	BitfieldShaderParams pData[1];

	int numSrcRegs = GetNumSrcRegisters();
	for(int i = 0; i < numSrcRegs; i++)
	{
		// Note all components do NOT share the same value, they vary from a global variable
		pData[0].srcVals[i][0] = m_Src[i] + g_bitfieldSwizzleOffsets[0];
		pData[0].srcVals[i][1] = m_Src[i] + g_bitfieldSwizzleOffsets[1];
		pData[0].srcVals[i][2] = m_Src[i] + g_bitfieldSwizzleOffsets[2];
		pData[0].srcVals[i][3] = m_Src[i] + g_bitfieldSwizzleOffsets[3];
	}

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_Bitfield::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	
	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	else
		GetEffectiveContext()->Draw( 1, 0 );

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// Lock the render target and examine the results
	UINT result[4];	// All 4 components
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_Bitfield::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Cycle over each component
	char *pXYZW = "xyzw";
	for(UINT i = 0; i < 4; i++)
	{
		// Check if we need to skip this component (because it was masked off etc...)
		if (ToString(m_destMask).find(pXYZW[i]) == std::string::npos)
			continue;

		// Calculate swizzle for this component, will be eith .x, .y, .z, or .w
		UINT src0SwizzleIndex = ComputeComponentIndex( m_src0Swizzle, i );
		UINT src1SwizzleIndex = ComputeComponentIndex( m_src1Swizzle, i );
		UINT src2SwizzleIndex = ComputeComponentIndex( m_src2Swizzle, i );
		UINT src3SwizzleIndex = ComputeComponentIndex( m_src3Swizzle, i );

		UINT calcResult = CalcResult(	m_Src[0] + g_bitfieldSwizzleOffsets[src0SwizzleIndex], 
										m_Src[1] + g_bitfieldSwizzleOffsets[src1SwizzleIndex], 
										m_Src[2] + g_bitfieldSwizzleOffsets[src2SwizzleIndex], 
										m_Src[3] + g_bitfieldSwizzleOffsets[src3SwizzleIndex] );
		if ( calcResult != result[i] )
		{
			WriteToLog( 1, "[Failure for %c] - Expected 0x%x : result 0x%x.", pXYZW[i], calcResult, result[i] );
			WriteToLog( 1, "Note: The per component variation from the src value: [0x%x, 0x%x, 0x%x, 0x%x].", g_bitfieldSwizzleOffsets[0], g_bitfieldSwizzleOffsets[1], g_bitfieldSwizzleOffsets[2], g_bitfieldSwizzleOffsets[3]);
			tr = RESULT_FAIL;
		}
	}

	return tr;
}

UINT CShader5xTest_Bitfield::ComputeComponentIndex(AssemblyHelper::Swizzle swizz, const UINT &index)
{
	// Description: Returns a zero-based index of the component based on a swizzle.
	// E.g. .xxxx will return 1 for all index [0, 3]
	// Local variables
	tstring swizzStr = ToString(swizz);

	if(index >= swizzStr.length())
		return -1;

	char ch = swizzStr[index];

	if( ch == 'x' )
		return 0;
	else if( ch == 'y' )
		return 1;
	else if( ch == 'z' )
		return 2;
	else if( ch == 'w' )
		return 3;
	else
		return -1;
}

void CShader5xTest_Bitfield::CleanupTestCase()
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
}

//////////////////////////////////////////////////////////////////////////
// UBFE
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_ubfe::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	// Make sure src0 and src1 (width and offset are clamped at 31)
	// Calculate the width and offset. Remember, only the first 5 bits are used
	const UINT fiveLSBMask = 0x1F;
	const UINT width = (fiveLSBMask) & src0;
	const UINT offset = (fiveLSBMask) & src1;
	UINT result = 0;

	// Now the we have computed the "real" widths and offsets, lets perform the extract
	if( width == 0 )
	{
		result = 0;	// Dest should be 0
	}
	else if( width + offset < 32 )
	{
		result = src2 << (32 - (width + offset));	// SHL
		result = result >> (32 - width);			// USHR
	}
	else
	{
		result = src2 >> offset;	//USHR
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// IBFE
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_ibfe::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	// Make sure src0 and src1 (width and offset are clamped at 31)
	// Calculate the width and offset. Remember, only the first 5 bits are used
	const UINT fiveLSBMask = 0x1F;
	const UINT width = (fiveLSBMask) & src0;
	const UINT offset = (fiveLSBMask) & src1;
	UINT result = 0;

	// Now the we have computed the "real" widths and offsets, lets perform the extract
	if( width == 0 )
	{
		result = 0;	// Dest should be 0
	}
	else if( width + offset < 32 )
	{
		result = src2 << (32 - (width + offset));	// SHL
		result = (int)result >> (int)(32 - width);	// USHR
	}
	else
	{
		result = (int)src2 >> (int)offset;	//USHR
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// BFI
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_bfi::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	// Make sure src0 and src1 (width and offset are clamped at 31)
	// Calculate the width and offset. Remember, only the first 5 bits are used
	const UINT fiveLSBMask = 0x1F;
	const UINT width = (fiveLSBMask) & src0;
	const UINT offset = (fiveLSBMask) & src1;
	UINT result = 0;

	// Now the we have computed the "real" widths and offsets, lets perform the insert
	UINT bitmask = ( ( ( 1 << width ) -1 ) << offset ) & 0xFFFFFFFF;
    result = ( (src2 << offset) & bitmask ) | ( src3 & ~bitmask );

	return result;
}

//////////////////////////////////////////////////////////////////////////
// BFREV
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_bfrev::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	UINT result = 0;

	// Reverse the bitfield
	for(UINT i = 0; i < 32; i++)
	{
		UINT readMask = 1 << i;
		UINT writeMask = 0x80000000 >> i;

		if(readMask & src0)
			result = result | writeMask;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// COUNTBITS
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_countbits::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	UINT result = 0;

	// Count the bitfield
	for(UINT i = 0; i < 32; i++)
	{
		UINT readMask = 1 << i;

		if(readMask & src0)
			result++;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// FIRSTBIT
//////////////////////////////////////////////////////////////////////////

UINT CShader5xTest_firstbit_lo::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	UINT result = -1;

	// Find the firstbit in the bitfield from the LSB
	for(UINT i = 0; i < 32; i++)
	{
		UINT readMask = 1 << i;

		if(readMask & src0)
		{
			result = i;
			break;
		}
	}

	return result;
}

UINT CShader5xTest_firstbit_hi::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	UINT result = -1;

	// Find the firstbit in the bitfield from the LSB
	for(int i = 31; i >= 0; i--)
	{
		UINT readMask = 1 << i;

		if(readMask & src0)
		{
			result = (31 - i);
			break;
		}
	}

	return result;
}

UINT CShader5xTest_firstbit_shi::CalcResult(UINT src0, UINT src1, UINT src2, UINT src3)
{
	UINT result = -1;

	// Find the firstbit in the bitfield from the LSB
	for(int i = 31; i >= 0; i--)
	{
		UINT readMask = 1 << i;

		// Is it negative?
		if( 0x80000000 & src0 )
		{
			// Find first 0
			if(!(readMask & src0))
			{
				result = (31 - i);
				break;
			}
		}
		else	// It is positive
		{
			// Find first 1 (just like _hi)
			if(readMask & src0)
			{
				result = (31 - i);
				break;
			}
		}
	}

	return result;
}
