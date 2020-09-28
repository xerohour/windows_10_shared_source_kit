//////////////////////////////////////////////////////////////////////////
//  Move.cpp
//  created:	2009/03/02
//
//  purpose: This test is for move related instructions, including swaps etc...
//////////////////////////////////////////////////////////////////////////

// Includes
#include "Move.h"

//////////////////////////////////////////////////////////////////////////
// Move and Swap Instructions (swapc)
//////////////////////////////////////////////////////////////////////////

// This is used for the swapc instruction, 0 is false, non-zero is true. Note this is component wise
UINT MoveSwapConditionals[] = 
{
	0,
	1,
	2,
	0x08000000,
	0x80000000,
	0xFFFFFFFF,
};

UINT MoveSwapValues[] =
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

// This controls by how much each swizzle value differs from the "original" value
UINT g_moveSwapSwizzleOffsets[] =
{
	0,	// Note this is ZERO becuase we want to test the original value!
	1,
	-1,
	0x80,
};

BEGIN_NAMED_VALUES( REGS )
	NAMED_VALUE( _T( "Different" ), REGS_DIFFERENT )
	NAMED_VALUE( _T( "Same" ), REGS_SAME )
	NAMED_VALUE( _T( "Mixed" ), REGS_MIXED )
END_NAMED_VALUES( REGS )

TEST_RESULT CShader5xTest_moveswap::SetPipeline(const char *szDest[], const char *szSrc[])
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	
	ID3D10Blob *pShaderBuf = NULL;
	
	AssemblyHelper asmHelper(20, 11);
	asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);
	asmHelper.SetOutputRegMask(1, AssemblyHelper::xyzw);
	
	// copy inputs to r7, r8, r9, r10... Note: This is always the same, but the outputs will change based on the register overlapps (per testcase)
	asmHelper.SetupMovToTempFromCB(7, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(8, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(9, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);

	if( REGS_SAME == m_srcRegs )
	{
		// output from r5 to o[0]
		asmHelper.SetupMovToTempFromLiteral(5, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 5, AssemblyHelper::swizXYZW);

		// output from r7 to o[1]
		asmHelper.SetupMovToTempFromLiteral(7, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(1, AssemblyHelper::xyzw, 7, AssemblyHelper::swizXYZW);
	}
	else if( REGS_MIXED == m_srcRegs )
	{
		// output from r8 to o[0]
		asmHelper.SetupMovToTempFromLiteral(8, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 8, AssemblyHelper::swizXYZW);

		// output from r9 to o[1]
		asmHelper.SetupMovToTempFromLiteral(9, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(1, AssemblyHelper::xyzw, 9, AssemblyHelper::swizXYZW);
	}
	else	// REGS_DIFFERENT
	{
		// output from r5 to o[0]
		asmHelper.SetupMovToTempFromLiteral(5, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 5, AssemblyHelper::swizXYZW);

		// output from r6 to o[1]
		asmHelper.SetupMovToTempFromLiteral(6, AssemblyHelper::xyzw, "l(0,0,0,0)");
		asmHelper.SetupMovToOutputFromTemp(1, AssemblyHelper::xyzw, 6, AssemblyHelper::swizXYZW);
	}

	tstring instructionCode;
	switch( GetNumSrcRegisters() )
	{
	case 3:	// SWAPC
		instructionCode = FormatString("%s %s.%s%s.%s%s.%s%s.%s%s.%s\n",
			GetOpCode(), 
			szDest[0], ToString(m_dest0Mask).c_str(), 
			szDest[1], ToString(m_dest1Mask).c_str(),
			szSrc[0], ToString(m_src0Swizzle).c_str(),
			szSrc[1], ToString(m_src1Swizzle).c_str(),
			szSrc[2], ToString(m_src2Swizzle).c_str());
		break;
	default:
		WriteToLog( _T("[CShader5xTest_moveswap::SetPipeline] Error, unknown number of SRC registers: %d"), GetNumSrcRegisters());
		tRes = RESULT_FAIL;
		goto setupDone;
	};

	// Because we have multiple outputs, we need multiple rendertargets
	D3D11_TEXTURE2D_DESC  tex2DDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc;

	m_pRTTexture2D->GetDesc(&tex2DDesc);
	m_pRTView->GetDesc(&rtviewdesc);
	m_pRTUAView->GetDesc(&rtuaviewdesc);

	if( FAILED( GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRTTexture2D2) ) )
	{
		WriteToLog( "Failed creating render target texture m_pRTTexture2D2" );
		return RESULT_FAIL;
	}

	if( FAILED( GetDevice()->CreateRenderTargetView( m_pRTTexture2D2, &rtviewdesc, &m_pRTView2 ) ) )
	{
		WriteToLog( "Failed creating RTV for texture m_pRTTexture2D2" );
		return RESULT_FAIL;
	}

	if( FAILED( GetDevice()->CreateUnorderedAccessView( m_pRTTexture2D2, &rtuaviewdesc, &m_pRTUAView2 ) ) )
	{
		WriteToLog( "Failed creating UAV for texture m_pRTTexture2D2" );
		return RESULT_FAIL;
	}

	ID3D11RenderTargetView *ppRTVs[] =
	{
		m_pRTView,
		m_pRTView2
	};

	ID3D11UnorderedAccessView *ppUAVs[] =
	{
		m_pRTUAView,
		m_pRTUAView2
	};

	// Set the pipeline
	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode, ppRTVs, ppUAVs, 2, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "CShader5xTest_moveswap::SetPipeline(): SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
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

void CShader5xTest_moveswap::InitTestParameters()
{
	// Local variables
	testfactor::RFactor rParameters;
	testfactor::RFactor rParameterSwizzles;
	testfactor::RFactor rParameterMaskSwizzles;
	testfactor::RFactor src0;
	testfactor::RFactor src1;
	testfactor::RFactor src2;
	testfactor::RFactor dest0Mask;
	testfactor::RFactor dest1Mask;
	testfactor::RFactor src0Swizzle;
	testfactor::RFactor src1Swizzle;
	testfactor::RFactor src2Swizzle;

	// Allocate the space needed for the source registers
	m_srcValues = new UINT[GetNumSrcRegisters()];

	CTableValueSet<AssemblyHelper::MaskFlag>	*pMaskValues = new CTableValueSet<AssemblyHelper::MaskFlag>( g_pMasks, sizeof( g_pMasks[ 0 ] ), ARRAYSIZE( g_pMasks ) );
	CTableValueSet<AssemblyHelper::Swizzle>		*pSwizzleValues = new CTableValueSet<AssemblyHelper::Swizzle>( g_pSwizzles, sizeof( g_pSwizzles[ 0 ] ), ARRAYSIZE( g_pSwizzles ) );

	// Create the swizzle and mask factors from the global sets
	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDest0MaskParam = AddParameter( _T( "Dest0Mask" ), &m_dest0Mask );
	SetParameterDefaultValue( pDest0MaskParam, AssemblyHelper::xyzw );
	dest0Mask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pDest0MaskParam, pMaskValues );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDest1MaskParam = AddParameter( _T( "Dest1Mask" ), &m_dest1Mask );
	SetParameterDefaultValue( pDest1MaskParam, AssemblyHelper::xyzw );
	dest1Mask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pDest1MaskParam, pMaskValues );

	src0 = AddParameter( "Src0", &(m_srcValues[0]), new CTableValueSet< UINT >( &MoveSwapConditionals[0], sizeof( MoveSwapConditionals[0] ), sizeof( MoveSwapConditionals ) / sizeof( MoveSwapConditionals[0] ) ) );
	src1 = AddParameter( "Src1", &(m_srcValues[1]), new CTableValueSet< UINT >( &MoveSwapValues[0], sizeof( MoveSwapValues[0] ), sizeof( MoveSwapValues ) / sizeof( MoveSwapValues[0] ) ) );
	src2 = AddParameter( "Src2", &(m_srcValues[2]), new CTableValueSet< UINT >( &MoveSwapValues[0], sizeof( MoveSwapValues[0] ), sizeof( MoveSwapValues ) / sizeof( MoveSwapValues[0] ) ) );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc0SwizzleParam = AddParameter( _T( "Src0Swizzle" ), &m_src0Swizzle );
	SetParameterDefaultValue( pSrc0SwizzleParam, AssemblyHelper::swizXYZW );
	src0Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc0SwizzleParam, pSwizzleValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc1SwizzleParam = AddParameter( _T( "Src1Swizzle" ), &m_src1Swizzle );
	SetParameterDefaultValue( pSrc1SwizzleParam, AssemblyHelper::swizXYZW );
	src1Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc1SwizzleParam, pSwizzleValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSrc2SwizzleParam = AddParameter( _T( "Src2Swizzle" ), &m_src2Swizzle );
	SetParameterDefaultValue( pSrc2SwizzleParam, AssemblyHelper::swizXYZW );
	src2Swizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSrc2SwizzleParam, pSwizzleValues );

	// Create factor to control if the src registers are the same reg, all different, or mixed.
	CTestCaseParameter<REGS> *pSrcRegParam = AddParameter( _T( "SrcRegistersAre" ), &m_srcRegs );
	SetParameterDefaultValue( pSrcRegParam, REGS_DIFFERENT );

	CUserValueSet<REGS> *pSrcRegValues = new CUserValueSet<REGS>( );
	pSrcRegValues->AddValue( REGS_DIFFERENT );
	pSrcRegValues->AddValue( REGS_SAME );
	pSrcRegValues->AddValue( REGS_MIXED );
	testfactor::RFactor srcRegs = AddParameterValueSet<REGS>( pSrcRegParam, pSrcRegValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	rParameters = (src0 * src1 * src2) % srcRegs;

	testfactor::XFACTOR maskSwizzlesPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{src0Swizzle, 1, NULL, 0},
		{src1Swizzle, 1, NULL, 0},
		{src2Swizzle, 1, NULL, 0},
		{dest0Mask, 1, NULL, 0},
		{dest1Mask, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	rParameterMaskSwizzles = NewCrossFactor(maskSwizzlesPairwiseTable, _countof(maskSwizzlesPairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rParameters % rParameterMaskSwizzles;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("Dest0Mask"), AssemblyHelper::xyzw ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("Dest1Mask"), AssemblyHelper::xyzw ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src0Swizzle"), AssemblyHelper::swizXYZW ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src1Swizzle"), AssemblyHelper::swizXYZW ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Src2Swizzle"), AssemblyHelper::swizXYZW ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<REGS>(_T("SrcRegistersAre"), REGS_DIFFERENT), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CShader5xTest_moveswap::Setup()
{
	// Call the parent Setup
	return CShader5xTest::Setup();
}

TEST_RESULT CShader5xTest_moveswap::SetupTestCase()
{
	const char *szDest[2];
	const char *szSrc[3];
	
	if( REGS_SAME == m_srcRegs )
	{
		szDest[0] = "r5";
		szDest[1] = ", r7";

		szSrc[0] = ", r7";
		szSrc[1] = ", r7";
		szSrc[2] = ", r7";
	}
	else if( REGS_MIXED == m_srcRegs )
	{
		szDest[0] = "r8";
		szDest[1] = ", r9";

		szSrc[0] = ", r7";
		szSrc[1] = ", r8";
		szSrc[2] = ", r9";
	}
	else	// REGS_DIFFERENT
	{
		szDest[0] = "r5";
		szDest[1] = ", r6";

		szSrc[0] = ", r7";
		szSrc[1] = ", r8";
		szSrc[2] = ", r9";
	}
		
	return SetPipeline(szDest, szSrc);
}

TEST_RESULT CShader5xTest_moveswap::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;

	//
	// Set the constant buffer data
	//
	MoveSwapShaderParams pData[1];

	int numSrcRegs = GetNumSrcRegisters();
	for(int i = 0; i < numSrcRegs; i++)
	{
		// Note all components do NOT share the same value, they vary from a global variable
		pData[0].srcVals[i][0] = m_srcValues[i] + g_moveSwapSwizzleOffsets[0];
		pData[0].srcVals[i][1] = m_srcValues[i] + g_moveSwapSwizzleOffsets[1];
		pData[0].srcVals[i][2] = m_srcValues[i] + g_moveSwapSwizzleOffsets[2];
		pData[0].srcVals[i][3] = m_srcValues[i] + g_moveSwapSwizzleOffsets[3];
	}

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_moveswap::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	
	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT ClearColorU[4] = {0, 0, 0, 0};

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUAView, ClearColorU);
		if ( m_pRTUAView2 )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUAView2, ClearColorU);
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		if ( m_pRTView2 )
			GetEffectiveContext()->ClearRenderTargetView( m_pRTView2, ClearColor );
		GetEffectiveContext()->Draw( 1, 0 );
	}

	// Lock the render target and examine the results
	UINT dest0result[4];
	UINT dest1result[4];

	// Always has at least one dest
	GetEffectiveContext()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, dest0result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_moveswap::ExecuteTestCase() failed to map m_pRTTexture2DCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Check to see if we have any more outputs
	if( GetNumDestRegisters() == 2 )
	{
		GetImmediateContext()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D2, 0, NULL );

		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, dest1result ) ) )
		{
			WriteToLog( _T( "CShader5xTest_moveswap::ExecuteTestCase() failed to map m_pRTTexture2DCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	// Cycle over each component
	UINT srcValues[3];
	char *pXYZW = "xyzw";
	for(UINT i = 0; i < 4; i++)
	{
		// Calculate swizzle for this component, will be eith .x, .y, .z, or .w
		UINT src0SwizzleIndex = ComputeComponentIndex( m_src0Swizzle, i );
		UINT src1SwizzleIndex = ComputeComponentIndex( m_src1Swizzle, i );
		UINT src2SwizzleIndex = ComputeComponentIndex( m_src2Swizzle, i );

		// Check what registers we used for swaps etc...
		if( REGS_SAME == m_srcRegs )
		{
			// Note:
			//szSrc[0] = ", r7";
			//szSrc[1] = ", r7";
			//szSrc[2] = ", r7";

			srcValues[0] = m_srcValues[0] + g_moveSwapSwizzleOffsets[src0SwizzleIndex];
			srcValues[1] = m_srcValues[0] + g_moveSwapSwizzleOffsets[src1SwizzleIndex];
			srcValues[2] = m_srcValues[0] + g_moveSwapSwizzleOffsets[src2SwizzleIndex];
		}
		else if( REGS_MIXED == m_srcRegs )
		{
			// Note:
			//szSrc[0] = ", r7";
			//szSrc[1] = ", r8";
			//szSrc[2] = ", r9";

			srcValues[0] = m_srcValues[0] + g_moveSwapSwizzleOffsets[src0SwizzleIndex];
			srcValues[1] = m_srcValues[1] + g_moveSwapSwizzleOffsets[src1SwizzleIndex];
			srcValues[2] = m_srcValues[2] + g_moveSwapSwizzleOffsets[src2SwizzleIndex];
		}
		else	// REGS_DIFFERENT
		{
			// Note:
			//szSrc[0] = ", r7";
			//szSrc[1] = ", r8";
			//szSrc[2] = ", r9";

			srcValues[0] = m_srcValues[0] + g_moveSwapSwizzleOffsets[src0SwizzleIndex];
			srcValues[1] = m_srcValues[1] + g_moveSwapSwizzleOffsets[src1SwizzleIndex];
			srcValues[2] = m_srcValues[2] + g_moveSwapSwizzleOffsets[src2SwizzleIndex];
		}

		UINT expectedDest0Result;
		UINT expectedDest1Result;
		CalcResult(expectedDest0Result, expectedDest1Result, srcValues[0], srcValues[1], srcValues[2]);

		// Check if we need to skip this component (because it was masked off etc...)
		if (ToString(m_dest0Mask).find(pXYZW[i]) != std::string::npos)
		{
			if ( expectedDest0Result != dest0result[i] )
			{
				WriteToLog( 1, "[Failure for dest0 %c] - Expected 0x%x : result 0x%x.", pXYZW[i], expectedDest0Result, dest0result[i] );
				WriteToLog( 1, "Note: The per component variation from the src value: [0x%x, 0x%x, 0x%x, 0x%x].", g_moveSwapSwizzleOffsets[0], g_moveSwapSwizzleOffsets[1], g_moveSwapSwizzleOffsets[2], g_moveSwapSwizzleOffsets[3]);
				tr = RESULT_FAIL;
			}
		}

		// Check if we need to skip this component (because it was masked off etc...)
		if (ToString(m_dest1Mask).find(pXYZW[i]) != std::string::npos)
		{
			if ( expectedDest1Result != dest1result[i] )
			{
				WriteToLog( 1, "[Failure for dest1 %c] - Expected 0x%x : result 0x%x.", pXYZW[i], expectedDest1Result, dest1result[i] );
				WriteToLog( 1, "Note: The per component variation from the src value: [0x%x, 0x%x, 0x%x, 0x%x].", g_moveSwapSwizzleOffsets[0], g_moveSwapSwizzleOffsets[1], g_moveSwapSwizzleOffsets[2], g_moveSwapSwizzleOffsets[3]);
				tr = RESULT_FAIL;
			}
		}
	}

	return tr;
}

UINT CShader5xTest_moveswap::ComputeComponentIndex(AssemblyHelper::Swizzle swizz, const UINT &index)
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

void CShader5xTest_moveswap::CleanupTestCase()
{
	SAFE_RELEASE( m_pRTView2 );
	SAFE_RELEASE( m_pRTTexture2D2 );
	SAFE_RELEASE( m_pRTUAView2 );

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
// SWAPC
//////////////////////////////////////////////////////////////////////////

TEST_RESULT CShader5xTest_swapc::Setup()
{
	// Swapc is FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("The swapc instruction is only for Feature Level 11 or greater.") );
		return RESULT_SKIP;
	}

	return CShader5xTest_moveswap::Setup();
}

void CShader5xTest_swapc::CalcResult(UINT &dest0, UINT &dest1, UINT src0, UINT src1, UINT src2)
{
	// Perform the swap
	if(src0 > 0)	// Non-zero is positive
	{
		dest0 = src2;
		dest1 = src1;
	}
	else
	{
		dest0 = src1;
		dest1 = src2;
	}
}

//////////////////////////////////////////////////////////////////////////
// DMOV
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 dmovInputTable[] = 
{
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }, //DC_NAN }// nan
	/*{*/ { 0x7effffffffffffffull, 0xfeffffffffffffffull }  ////max min
};

void CShader5xTest_dmov::InitTestParameters()
{
	CTestCaseParameter<CShader5xTest_DoubleArith::Binary64Vec2>* pSrc = AddParameter<CShader5xTest_DoubleArith::Binary64Vec2>( _T("Src"), &m_Src );
	
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<CShader5xTest_DoubleArith::Binary64Vec2>( dmovInputTable, sizeof(CShader5xTest_DoubleArith::Binary64Vec2), sizeof(dmovInputTable) / sizeof(CShader5xTest_DoubleArith::Binary64Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("xyxy") );
	pSwizzleValueSet->AddValue( _T("zwxy") ); 
	pSwizzleValueSet->AddValue( _T("zwzw") );  
	
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrcType = AddParameter<OPERAND_TYPE>( _T("OperandType"), &m_OperandType );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrcType, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperandTypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );

	testfactor::RFactor srcType = AddParameterValueSet( pSrcType, pOperandTypeValueSet );
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * srcType * swizzleFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"),  _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("OperandType"), OT_TEMP ), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

TEST_RESULT CShader5xTest_dmov::SetupTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;
	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	assert((OT_TEMP == m_OperandType) || (OT_CONSTANT_BUFFER == m_OperandType));

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper( 1, 2 );

	if(OT_TEMP == m_OperandType)
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 

											_T("dmov"),
											_T("r1"),
											maskString.c_str(),

											negString.c_str(),
											OT_TEMP == m_OperandType ? _T("r0") : _T("c0[0]"),
											swizString.c_str()
									);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTView,
												&m_pRTUAView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		trResult = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		trResult = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);

	return trResult;

}

CShader5xTest_DoubleArith::Binary64Vec2 CShader5xTest_dmov::CalcSrcSwizzle(CShader5xTest_DoubleArith::Binary64Vec2 const& x, tstring const& szSwizzle)
{
	if(szSwizzle.size() < 4)
	{
		return x;
	}
	CShader5xTest_DoubleArith::Binary64Vec2 retValue;
	//if swizzle[0] is z (ie zw) src component [1] was used
	switch(szSwizzle[0])
	{
		case 'z'://zw
			retValue.m_X = x.m_Y;
			break;
		default://xy implied
			retValue.m_X = x.m_X;
			break;
	}
	//if swizzle[2] is x (ie xy) src component [0] was used
	switch(szSwizzle[2])
	{
		case 'x'://xy
			retValue.m_Y = x.m_X;
			break;
		default://zw implied
			retValue.m_Y = x.m_Y;
			break;
	}
	return retValue;
}

CShader5xTest_DoubleArith::Binary64Vec2 CShader5xTest_dmov::CalcResultMask(CShader5xTest_DoubleArith::Binary64Vec2 const& x, tstring const& szMask)
{
	if(szMask.size() <= 0)
	{
		return x;//xyzw is assumed
	}
	CShader5xTest_DoubleArith::Binary64Vec2 retValue;

	switch(szMask[0])
	{
		case 'z'://zw
			retValue.m_X = x.m_Y;
			retValue.m_Y = x.m_X;
			break;
		default://xy | xyzw
			retValue.m_X = x.m_X;
			retValue.m_Y = x.m_Y;
			break;
	}
	return retValue;

}


TEST_RESULT CShader5xTest_dmov::ExecuteTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;

	if(FAILED(hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, m_Src)))
	{
		WriteToLog(_T("CShader5xTest_dmov::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw(1, 0);
	}

	GetEffectiveContext()->CopySubresourceRegion(m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	CShader5xTest_DoubleArith::Binary64Vec2 result;//0=x 1=y 2=z 3=w
	if(FAILED(hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, result)))
	{
		WriteToLog(_T("CShader5xTest_dmov::ExecuteTestCase() failed to map m_pRTTexture2DCopy, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	//Caculate Modified Values
	CShader5xTest_DoubleArith::Binary64Vec2 const srcValue = CalcSrcSwizzle(CShader5xTest_DoubleArith::GetModifiedValue( m_Src, m_SrcModifier ), m_Swizzle);
	CShader5xTest_DoubleArith::Binary64Vec2 const resValue = CalcResultMask(result, m_Mask);
	UINT numResults = (m_Mask.size() == 2) ? 1 : 2;
	for(UINT i = 0; i < numResults; ++i)
	{
        if ( m_SrcModifier != SMOD_NONE )
        {
            // Win7: 748246 - If we have a modifier, we may or may not have to be more flexible during verification...

			// Because we had a modifier and NaN we cannot gurantee a bit-for-bit answer.
			// ...so we just verify the result is a NaN.
			if (CShader5xTest_DoubleArith::IsNaN( srcValue.m_Vec[i] ) && CShader5xTest_DoubleArith::IsNaN( resValue.m_Vec[i] ) )
            {
                continue;
            }
				
			// If we got here, we still require bit-for-bit copies from dmov...
        }
		if( srcValue.m_Vec[i].m_AsULongLong != resValue.m_Vec[i].m_AsULongLong)
		{
			WriteToLog( 1, "Expected (for component %d) 0x%0I64X : result 0x%0I64X.", i, srcValue.m_Vec[i].m_AsULongLong, resValue.m_Vec[i].m_AsULongLong );
			trResult = RESULT_FAIL;
		}
	}
	return trResult;
}

CShader5xTest_dmov::~CShader5xTest_dmov()
{
}

//////////////////////////////////////////////////////////////////////////
// DMOVC
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 dmovcSrc0InputTable[] = 
{
	/*{*/ { 0xBFF0000000000000ull, 0x0000000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEp DC_ONEn },
};

const CShader5xTest_DoubleArith::Binary64Vec2 dmovcSrc1InputTable[] = 
{
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }, //DC_NAN }// nan
	/*{*/ { 0x7effffffffffffffull, 0xfeffffffffffffffull }  ////max min
};

const CShader5xTest_DoubleArith::Binary64Vec2 dmovcSrc2InputTable[] = 
{
	/*{*/ { 0x7effffffffffffffull, 0xfeffffffffffffffull },  ////max min
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }, //DC_NAN }// nan
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },

};

struct DoubleMoveConditionShaderParams
{
	CShader5xTest_DoubleArith::Binary64Vec2 src0;
	CShader5xTest_DoubleArith::Binary64Vec2 src1;
	CShader5xTest_DoubleArith::Binary64Vec2 src2;
};

void CShader5xTest_dmovc::InitTestParameters()
{
	CTestCaseParameter<CShader5xTest_DoubleArith::Binary64Vec2>* pSrc0 = AddParameter<CShader5xTest_DoubleArith::Binary64Vec2>( _T("Src0"), &m_Src0 );
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<CShader5xTest_DoubleArith::Binary64Vec2>( dmovcSrc0InputTable, sizeof(CShader5xTest_DoubleArith::Binary64Vec2), sizeof(dmovcSrc0InputTable) / sizeof(CShader5xTest_DoubleArith::Binary64Vec2) ) );

	CTestCaseParameter<CShader5xTest_DoubleArith::Binary64Vec2>* pSrc1 = AddParameter<CShader5xTest_DoubleArith::Binary64Vec2>( _T("Src1"), &m_Src1 );
	testfactor::RFactor src_t1 = AddParameterValueSet( pSrc1, new CTableValueSet<CShader5xTest_DoubleArith::Binary64Vec2>( dmovcSrc1InputTable, sizeof(CShader5xTest_DoubleArith::Binary64Vec2), sizeof(dmovcSrc1InputTable) / sizeof(CShader5xTest_DoubleArith::Binary64Vec2) ) );

	CTestCaseParameter<CShader5xTest_DoubleArith::Binary64Vec2>* pSrc2 = AddParameter<CShader5xTest_DoubleArith::Binary64Vec2>( _T("Src2"), &m_Src2 );
	testfactor::RFactor src_t2 = AddParameterValueSet( pSrc2, new CTableValueSet<CShader5xTest_DoubleArith::Binary64Vec2>( dmovcSrc2InputTable, sizeof(CShader5xTest_DoubleArith::Binary64Vec2), sizeof(dmovcSrc2InputTable) / sizeof(CShader5xTest_DoubleArith::Binary64Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam0 = AddParameter<tstring>( _T("Swizzle0"), &m_Swizzle0 );
	SetParameterDefaultValue<tstring>( pSwizzleParam0, _T("xyzw") );
	
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("xyxy") );
	pSwizzleValueSet->AddValue( _T("zwxy") ); 
	pSwizzleValueSet->AddValue( _T("zwzw") );  
	testfactor::RFactor swizzleFactor0 = AddParameterValueSet( pSwizzleParam0, pSwizzleValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam1 = AddParameter<tstring>( _T("Swizzle1"), &m_Swizzle1 );
	SetParameterDefaultValue<tstring>( pSwizzleParam1, _T("xyzw") );
	
	testfactor::RFactor swizzleFactor1 = AddParameterValueSet( pSwizzleParam1, pSwizzleValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam2 = AddParameter<tstring>( _T("Swizzle2"), &m_Swizzle2 );
	SetParameterDefaultValue<tstring>( pSwizzleParam2, _T("xyzw") );
	
	testfactor::RFactor swizzleFactor2 = AddParameterValueSet( pSwizzleParam2, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrc0Type = AddParameter<OPERAND_TYPE>( _T("Operand0Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc0Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand0TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src0Type = AddParameterValueSet( pSrc0Type, pOperand0TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[2] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );
	
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = ((src_t0 % (swizzleFactor0 * src0Type)) * maskFactor *((src_t1 % (swizzleFactor1 * src1Type)) * (src_t2 % (swizzleFactor2 * src2Type)) * srcMod));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"),  _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle0"), _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle1"), _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle2"), _T("xyzw") ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand0Type"), OT_TEMP ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand0Type"), OT_CONSTANT_BUFFER ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP ), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER ), WeightFactorSmall);
	
	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CShader5xTest_dmovc::SetupTestCase()
{
	HRESULT 	  hr 		  = 0;
	TEST_RESULT   trResult    = RESULT_PASS;
	const tstring negString   = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString  = m_Mask.empty() 	   ? _T("")  : _T(".") + m_Mask;
	const tstring swizString0 = m_Swizzle0.empty() ? _T(",") : _T(".") + m_Swizzle0 + ",";
	const tstring swizString1 = m_Swizzle1.empty() ? _T(",") : _T(".") + m_Swizzle1 + ",";
	const tstring swizString2 = m_Swizzle2.empty() ? _T("")  : _T(".") + m_Swizzle2;
	
	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper( 20, 11 );
	asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);
	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 7, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 8, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[2])
	{
		asmHelper.SetupMovToTempFromCB( 6, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	}
	
	asmHelper.SetupMovToOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s %s%s%s %s%s%s"), 
											_T("dmovc"),

											_T("r1"),
											maskString.c_str(),

											OT_TEMP == m_OperandType[0] ? _T("r7") : _T("c0[0]"),
											swizString0.c_str(),
											
											negString.c_str(),
											OT_TEMP == m_OperandType[1] ? _T("r8") : _T("c0[1]"),
											swizString1.c_str(),
											
											negString.c_str(),
											OT_TEMP == m_OperandType[2] ? _T("r6") : _T("c0[2]"),
											swizString2.c_str());

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTView,
												&m_pRTUAView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		trResult = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		trResult = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);

	return trResult;

}

TEST_RESULT CShader5xTest_dmovc::ExecuteTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;

	DoubleMoveConditionShaderParams pData[1];
	pData[0].src0 = m_Src0;
	pData[0].src1 = m_Src1;
	pData[0].src2 = m_Src2;
	if(FAILED(hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData)))
	{
		WriteToLog(_T("CShader5xTest_dmovc::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw(1, 0);
	}

	GetEffectiveContext()->CopySubresourceRegion(m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	CShader5xTest_DoubleArith::Binary64Vec2 result;
	if(FAILED(hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, result)))
	{
		WriteToLog(_T("CShader5xTest_dmovc::ExecuteTestCase() failed to map m_pRTTexture2DCopy, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	CShader5xTest_DoubleArith::Binary64Vec2 const src0Modified = CalcSrcSwizzle(CShader5xTest_DoubleArith::GetModifiedValue( m_Src0, SMOD_NONE ), m_Swizzle0);;
	CShader5xTest_DoubleArith::Binary64Vec2 const src1Modified = CalcSrcSwizzle(CShader5xTest_DoubleArith::GetModifiedValue( m_Src1, m_SrcModifier ), m_Swizzle1);
	CShader5xTest_DoubleArith::Binary64Vec2 const src2Modified = CalcSrcSwizzle(CShader5xTest_DoubleArith::GetModifiedValue( m_Src2, m_SrcModifier ), m_Swizzle2);
	CShader5xTest_DoubleArith::Binary64Vec2 const destModified = CalcResultMask(result, m_Mask);

	UINT conIndex[2] = {0,1};

	switch(m_Mask[0])
	{
		case 'z'://look at 2nd 32bit value of src0
			conIndex[0] = 1;
			break;
		default://look at 1st 32bit value of src0
			conIndex[0] = 0;
			break;
	}
	if(m_Mask.size() == 4)
	{
		switch(m_Mask[2])
		{
			case 'z'://look at 2nd 32bit value of src0
				conIndex[1] = 1;
				break;
			default://look at 1st 32bit value of src0
				conIndex[1] = 0;
				break;
		}
	}

	UINT numResults = (m_Mask.size() == 2) ? 1 : 2;
	for(UINT i = 0; i < numResults; ++i)
	{
		UINT srcIndex = 1;  //zw
		if(conIndex[i] == 0)//xy
		{
			srcIndex = 0;
		}
		if(src0Modified.m_Vec[0].m_AsULong[conIndex[i]] != 0)//src1[srcIndex] == result[i]
		{
			if ( m_SrcModifier != SMOD_NONE )
			{
				// Win7: 748246 - If we have a modifier, we may or may not have to be more flexible during verification...

				// Because we had a modifier and NaN we cannot gurantee a bit-for-bit answer.
				// ...so we just verify the result is a NaN.
				if (CShader5xTest_DoubleArith::IsNaN( src1Modified.m_Vec[srcIndex] ) && CShader5xTest_DoubleArith::IsNaN( destModified.m_Vec[i] ) )
				{
					// Pass, don't check for bit-for-bit copies
					continue;
				}
				
				// If we got here, we still require bit-for-bit copies from dmovc...
			}
			if( src1Modified.m_Vec[srcIndex].m_AsULongLong != destModified.m_Vec[i].m_AsULongLong)
			{
				WriteToLog( 1, "Expected (for component %d) 0x%0I64X : result 0x%0I64X.", i, src1Modified.m_Vec[srcIndex].m_AsULongLong, destModified.m_Vec[i].m_AsULongLong );
				trResult = RESULT_FAIL;
			}
		}
		else//src2[srcIndex] == result[i]
		{
			if ( m_SrcModifier != SMOD_NONE )
			{
				// Win7: 748246 - If we have a modifier, we may or may not have to be more flexible during verification...

				// Because we had a modifier and NaN we cannot gurantee a bit-for-bit answer.
				// ...so we just verify the result is a NaN.
				if (CShader5xTest_DoubleArith::IsNaN( src2Modified.m_Vec[srcIndex] ) && CShader5xTest_DoubleArith::IsNaN( destModified.m_Vec[i] ) )
				{
					// Pass, don't check for bit-for-bit copies
					continue;
				}

				// If we got here, we still require bit-for-bit copies from dmovc...
			}
			if( src2Modified.m_Vec[srcIndex].m_AsULongLong != destModified.m_Vec[i].m_AsULongLong)
			{
				WriteToLog( 1, "Expected (for component %d) 0x%0I64X : result 0x%0I64X.", i, src2Modified.m_Vec[srcIndex].m_AsULongLong, destModified.m_Vec[i].m_AsULongLong );
				trResult = RESULT_FAIL;
			}
		}
	}
	
	return trResult;
}

CShader5xTest_dmovc::~CShader5xTest_dmovc()
{
}

////

void CShader5xTest_cyclecounter::InitTestParameters()
{
	AddParameter< AssemblyHelper::Swizzle >  ( _T("SrcSwizzle"),  &m_SrcSwizzle );
	AddParameter< AssemblyHelper::MaskFlag > ( _T("DclMask"),     &m_DclMask );
	AddParameter< AssemblyHelper::MaskFlag > ( _T("DstMask"),     &m_DstMask );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	const AssemblyHelper::Swizzle swizXXXX = {AssemblyHelper::x, AssemblyHelper::x, AssemblyHelper::x, AssemblyHelper::x};
	const AssemblyHelper::Swizzle swizYYYY = {AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::y};
	const AssemblyHelper::Swizzle swizXYXY = {AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::y};

	testfactor::RFactor rSrcSwizzleXXXX = AddParameterValue< AssemblyHelper::Swizzle >( _T("SrcSwizzle"), swizXXXX );
	testfactor::RFactor rSrcSwizzleYYYY = AddParameterValue< AssemblyHelper::Swizzle >( _T("SrcSwizzle"), swizYYYY );
	testfactor::RFactor rSrcSwizzleXYXY = AddParameterValue< AssemblyHelper::Swizzle >( _T("SrcSwizzle"), swizXYXY );
	
	testfactor::RFactor rDclMaskX  = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DclMask"), AssemblyHelper::x );
	testfactor::RFactor rDclMaskXY = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DclMask"), AssemblyHelper::xy );

	testfactor::RFactor rDstMaskX  = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::x );
	testfactor::RFactor rDstMaskY  = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::y );
	testfactor::RFactor rDstMaskZ  = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::z );
	testfactor::RFactor rDstMaskW  = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::w );

	testfactor::RFactor rDstMaskXY = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::xy );
	testfactor::RFactor rDstMaskZW = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::zw );
	testfactor::RFactor rDstMaskXW = AddParameterValue< AssemblyHelper::MaskFlag >( _T("DstMask"), AssemblyHelper::MakeMask( true, false, false, true) );

	testfactor::RFactor rDstMaskOneCmp = rDstMaskX + rDstMaskY + rDstMaskZ + rDstMaskW;
	testfactor::RFactor rDstMaskTwoCmp = rDstMaskXY + rDstMaskZW + rDstMaskXW;

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = (rDclMaskX * rSrcSwizzleXXXX * rDstMaskOneCmp) + (rDclMaskXY * (rSrcSwizzleXXXX + rSrcSwizzleYYYY + rSrcSwizzleXYXY) * (rDstMaskOneCmp + rDstMaskTwoCmp));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}

TEST_RESULT CShader5xTest_cyclecounter::Setup()
{
	// This debug counter is FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("The debug cycle counter is only for Feature Level 11 or greater.") );
		return RESULT_SKIP;
	}

	// Call the parent Setup
	return CShader5xTest::Setup();
}

TEST_RESULT CShader5xTest_cyclecounter::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D10Blob* pShaderBuf = NULL;

	AssemblyHelper asmHelper(0, 1);
	asmHelper.SetOutputRegMask(0, AssemblyHelper::xyzw);
	asmHelper.SetCycleCounterDecl(m_DclMask);
	asmHelper.SetupMovToTempFromLiteral(0, AssemblyHelper::xyzw, tstring(_T("l( 0, 0, 0, 0 )"))); // Initial r0 to 0

	tstring shaderCode = "mov r0." + ToString(m_DstMask) + ", vCycleCounter." + ToString(m_SrcSwizzle) + "\n";
	
	asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												shaderCode,
												&m_pRTView, &m_pRTUAView, 1, &pShaderBuf,
												&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pShaderBuf );
	return tRes;                                                
}

TEST_RESULT CShader5xTest_cyclecounter::ExecuteTestCase()
{
	HRESULT hr = S_OK;
	UINT Pixel[4] = {0, 0, 0, 0};
	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT ClearColorU[4] = {0, 0, 0, 0};

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUAView, ClearColorU);
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTTexture2DCopy, Pixel ) ) )
	{
		WriteToLog( _T( "Failed to map m_pRTTexture2DCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Nothing to check, just verifying that the driver does not crash

	return RESULT_PASS;
}

void CShader5xTest_cyclecounter::CleanupTestCase()
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