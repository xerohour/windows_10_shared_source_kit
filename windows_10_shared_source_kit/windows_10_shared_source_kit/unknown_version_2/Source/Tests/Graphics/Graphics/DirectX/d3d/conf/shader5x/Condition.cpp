//////////////////////////////////////////////////////////////////////////
//  Condition.cpp
//  created:	2005/03/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

// BUGBUG: still need to test 16bit floats

#include <float.h>
#include <d3dx10strings.h>
#include "condition.h"

//Must be multiple of 4
float FloatConditionValues[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANn(),

	FloatMaker::QNANp(),
	FloatMaker::SNANn(),
	FloatMaker::ZEROp(), // +0
	FloatMaker::ZEROn(), // -0

	FloatMaker::DENORMp(), // +Denorm
	FloatMaker::DENORMn(), // -Denorm
	1.f,
	-1.f,

	0.125f,
	-0.125f,
	65535.f,
	-65535.f,

	3.14159f,
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	FloatMaker::FLOATMAXp(), // +FLOAT_MAX
	FloatMaker::FLOATMAXn(), // -FLOAT_MAX
};

D3DXVECTOR4 VectorValues[sizeof( FloatConditionValues ) / sizeof( FloatConditionValues[0] )];


//Must be multiple of 4
D3DXVECTOR4 FloatVectorConditionValues[] =
{
	D3DXVECTOR4(
		FloatMaker::INFn(),
		FloatMaker::INFp(),
		FloatMaker::QNANp(),
		FloatMaker::SNANn()
	),
	D3DXVECTOR4(
		FloatMaker::QNANp(),
		FloatMaker::SNANn(),
		FloatMaker::ZEROp(), // +0
		FloatMaker::ZEROn() // -0
	),
	D3DXVECTOR4(
		FloatMaker::DENORMp(), // +Denorm
		FloatMaker::DENORMn(), // -Denorm
		1.f,
		-1.f
	),
	D3DXVECTOR4(
		0.125f,
		-0.125f,
		65535.f,
		-65535.f
	),
	D3DXVECTOR4(
		3.14159f,
		FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
		FloatMaker::FLOATMAXp(), // +FLOAT_MAX
		FloatMaker::FLOATMAXn() // -FLOAT_MAX
	)
};

CShader5xTest_DoubleArith::Binary64 DoubleConditionValues[] = 
{
	0xFFF0000000000000ull, // -INF
	0x7FF0000000000000ull, // +INF
	0x7FFFFFFFFFFFFFFFull, // +QNAN
	0xFFFFFFFFFFFFFFFFull, // -QNAN

	0x7FFFFFFFFFFFFFFFull, // +QNAN
	0xFFFFFFFFFFFFFFFFull, // -QNAN
	0x0000000000000000ull, // +Zero
	0x8000000000000000ull, // - Zero

	0x000F4A8B6037B1F5ull, // +DNorm
	0x800F4A8B6037B1F5ull, // -DNorm
	0x3FF0000000000000ull, // +One
	0xBFF0000000000000ull, // -One

	0x000F4A8B6037B1F5ull, // +DNorm
	0x800F4A8B6037B1F5ull, // -DNorm
	0x4070005E353F7CEEull, // +256.023
	0xC070005E353F7CEEull, // -256.023
	
	0x400921FB54442D18ull, // +real
	0x3C670EF54646D497ull, // +min
	0x7effffffffffffffull, // +max
	0xfeffffffffffffffull, // -max

};

CShader5xTest_DoubleArith::Binary64Vec2 DoubleVectorConditionValues[] =
{
	{
		0xFFF0000000000000ull, // -INF
		0x7FF0000000000000ull, // +INF
	},
	{
		0x7FFFFFFFFFFFFFFFull, // +QNAN
		0xFFFFFFFFFFFFFFFFull, // -QNAN
	},
	{
		0x7FFFFFFFFFFFFFFFull, // +QNAN
		0xFFFFFFFFFFFFFFFFull, // -QNAN
	},
	{
		0x0000000000000000ull, // +Zero
		0x8000000000000000ull, // - Zero
	},
	{
		0x000F4A8B6037B1F5ull, // +DNorm
		0x800F4A8B6037B1F5ull, // -DNorm
	},
	{
		0x3FF0000000000000ull, // +One
		0xBFF0000000000000ull, // -One
	},
	{
		0x000F4A8B6037B1F5ull, // +DNorm
		0x800F4A8B6037B1F5ull, // -DNorm
	},
	{
		0x4070005E353F7CEEull, // +256.023
		0xC070005E353F7CEEull, // -256.023
	},
	{
		0x400921FB54442D18ull, // +real
		0x3C670EF54646D497ull, // +min
	},
	{
		0x7effffffffffffffull, // +max
		0xfeffffffffffffffull, // -max
	},
};

//Must be multiple of 4
int IntConditionValues[] =
{
	INT_MAX,
	SHRT_MAX + 1,
	SHRT_MAX,
	2,

	1,
	0,
	-1,
	-2,

	-3,
	SHRT_MIN,
	SHRT_MIN - 1,
	INT_MIN,
};

int4 IntVectorConditionValues[] =
{
	{
		INT_MAX,
		SHRT_MAX + 1,
		SHRT_MAX,
		2
	},
	{
		1,
		0,
		-1,
		-2
	},
	{
		-3,
		SHRT_MIN,
		SHRT_MIN - 1,
		INT_MIN
	}
};

//Must be multiple of 4
UINT UIntConditionValues[] =
{
	UINT_MAX,
	USHRT_MAX + 1,
	USHRT_MAX,
	128,

	3,
	2,
	1,
	0,
};

//Must be multiple of 4
uint4 UIntVectorConditionValues[] =
{
	{
		UINT_MAX,
		USHRT_MAX + 1,
		USHRT_MAX,
		128
	},
	{
		3,
		2,
		1,
		0
	}
};

TEST_RESULT CShader5xTest_Condition::SetPipeline(const char *szDest, const char *szSrc[2], bool bDoubles)
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
	asmHelper.SetupMovToTempFromLiteral(6, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )"));
	if(bDoubles == true)
	{
		asmHelper.EnableDoubleSupport();
	}
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

void CShader5xTest_Condition::CleanupTestCase()
{
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

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
// Float Condition Instructions
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_FloatCondition::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );

	size_t count = sizeof( FloatConditionValues ) / sizeof( FloatConditionValues[0] );
	size_t stride = sizeof( FloatConditionValues[0] );

	for (size_t i=0; i<count; i++)
	{
		UINT index0 = i < count - 0 ? i + 0 : (i - count ) + 0;
		UINT index1 = i < count - 1 ? i + 1 : (i - count ) + 1;
		UINT index2 = i < count - 2 ? i + 2 : (i - count ) + 2;
		UINT index3 = i < count - 3 ? i + 3 : (i - count ) + 3;
		VectorValues[i][0] = FloatConditionValues[index0];
		VectorValues[i][1] = FloatConditionValues[index1];
		VectorValues[i][2] = FloatConditionValues[index2];
		VectorValues[i][3] = FloatConditionValues[index3];
	}

	testfactor::RFactor src0 = AddParameter( "Vert0_Src0", &m_Src[0], new CTableValueSet< D3DXVECTOR4 >( &VectorValues[0], sizeof( VectorValues[0] ), sizeof( VectorValues ) / sizeof( VectorValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter( "Vert0_Src1", &m_Src[1], new CTableValueSet< D3DXVECTOR4 >( &FloatVectorConditionValues[0], sizeof( FloatVectorConditionValues[0] ), sizeof( FloatVectorConditionValues ) / sizeof( FloatVectorConditionValues[0] ) ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise testing to reduce the number of tests
	testfactor::RFactor rfSrcs = src0 * src1;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfSrcs, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CShader5xTest_FloatCondition::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Condition::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xyzw";
	const char *szSrc[2];
	
	switch( m_SrcMod[0] )
	{
		case SMOD_NONE:
			szSrc[0] = ", r7";
			break;
		case SMOD_ABS:
			szSrc[0] = ", abs( r7 )";
			break;
		case SMOD_NEG:
			szSrc[0] = ", -r7";
			break;
		case SMOD_ABSNEG:
			szSrc[0] = ", -abs( r7 )";
			break;
	}
	
	switch( m_SrcMod[1] )
	{
		case SMOD_NONE:
			szSrc[1] = ", r8";
			break;
		case SMOD_ABS:
			szSrc[1] = ", abs( r8 )";
			break;
		case SMOD_NEG:
			szSrc[1] = ", -r8";
			break;
		case SMOD_ABSNEG:
			szSrc[1] = ", -abs( r8 )";
			break;
	}
		
	
	return SetPipeline(szDest, szSrc);
}

TEST_RESULT CShader5xTest_FloatCondition::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	FLOAT_INPUT_CLASS ficSrc0[4];
	FLOAT_INPUT_CLASS ficSrc1[4];
	FLOAT_CONDITION_RESULT_CLASS fcrcExpectedResult[4];

	for (int i=0; i<4; i++)
	{
		ficSrc0[i] = GetInputClass( ResolveSrc(m_Src[0][i], m_SrcMod[0]) );
		ficSrc1[i] = GetInputClass( ResolveSrc(m_Src[1][i], m_SrcMod[1]) );
		fcrcExpectedResult[i] = GetResultClass( ficSrc0[i], ficSrc1[i] );
	}

	// Set the constant buffer data
	FloatConditionShaderParams pData[1];

	pData[0].src0 = m_Src[0];
	pData[0].src1 = m_Src[1];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_FloatCondition::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
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
	DWORD result[4];
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_FloatCondition::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	for (int i=0; i<4; i++)
	{
		if (fcrcExpectedResult[i] == FCRC_CALC)
		{
			float srcModified0 = ResolveSrc(m_Src[0][i], m_SrcMod[0]);
			float srcModified1 = ResolveSrc(m_Src[1][i], m_SrcMod[1]);
			DWORD calcResult = CalcResult( srcModified0, srcModified1 ) ? 0xFFFFFFFF : 0x00000000;
			if ( calcResult != result[i] )
			{
				WriteToLog( 1, "Expected (for component %d) %x : result %x.", i, calcResult, result[i] );
				tr = RESULT_FAIL;
			}
		}
		else if ( fcrcExpectedResult[i] != result[i] )
		{
			WriteToLog( 1, "Expected (for component %d) %x : result %x.", i, fcrcExpectedResult[i], result[i] );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}



//////////////////////////////////////////////////////////////////////////
// Double Condition Instructions
//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 dVectorValues[sizeof(DoubleConditionValues) / sizeof(DoubleConditionValues[0])];

void CShader5xTest_DoubleCondition::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );

	//num elements
	size_t count = sizeof( DoubleConditionValues ) / sizeof( DoubleConditionValues[0] );//sizeof(20[double elements])
	size_t stride = sizeof( DoubleConditionValues[0] );//sizeof(double)

	for (size_t i=0; i<count; i++)
	{
		UINT index0 = i < count - 0 ? i + 0 : (i - count ) + 0;//i = 0 index = 0
		UINT index1 = i < count - 1 ? i + 1 : (i - count ) + 1;//i = 0 index = 1
		dVectorValues[i].m_X = DoubleConditionValues[index0];
		dVectorValues[i].m_Y = DoubleConditionValues[index1];
	}


	testfactor::RFactor src0 = AddParameter("Vert0_Src0", &m_Src[0], new CTableValueSet< CShader5xTest_DoubleArith::Binary64Vec2 >( &dVectorValues[0], sizeof( dVectorValues[0] ), sizeof( dVectorValues ) / sizeof( dVectorValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter("Vert0_Src1", &m_Src[1], new CTableValueSet< CShader5xTest_DoubleArith::Binary64Vec2 >( &DoubleVectorConditionValues[0], sizeof( DoubleVectorConditionValues[0] ), sizeof( DoubleVectorConditionValues ) / sizeof( DoubleVectorConditionValues[0] ) ) );

	testfactor::RFactor src0mod = AddParameter("Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter("Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

		// Use pairwise testing to reduce the number of tests
	testfactor::RFactor rfSrcs = src0 * src1;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfSrcs, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
		{src1Type, 1, NULL, 0},
		{src2Type, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 3), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

static void GetOperandString(char* Buff, UINT NumChars, UINT SrcIndex, OPERAND_TYPE Type, SRC_MOD_TYPE Modifier)
{
	const char* Negation = "";
	const char* AbsBegin = "";
	const char* AbsEnd = "";
	const char* RegTypeBegin = "";
	const char* RegTypeEnd = "";

	UINT Index = SrcIndex;
	
	switch( Modifier )
	{
	case SMOD_NONE:
	case SMOD_NEG:
		break;

	case SMOD_ABSNEG:
	case SMOD_ABS:
		AbsBegin = "abs(";
		AbsEnd = ")";
		break;

	default:
		assert(false);
	}

	switch( Modifier )
	{
	case SMOD_NONE:
	case SMOD_ABS:
		break;

	case SMOD_ABSNEG:
	case SMOD_NEG:
		Negation = "-";
		break;

	default:
		assert(false);
	}

	switch( Type )
	{
	case OT_TEMP:
		Index += 7; // input comes from r7 and r8
		RegTypeBegin = "r";
		break;

	case OT_CONSTANT_BUFFER:
		// input comes from c0[0] and c0[1]
		RegTypeBegin = "c0[";
		RegTypeEnd = "]";
		break;

	default:
		assert(false);
	}

	sprintf_s(Buff, 
			  NumChars, 
			  ", %s%s%s%i%s%s", 
			  Negation,
			  AbsBegin,
			  RegTypeBegin,
			  Index,
			  RegTypeEnd,
			  AbsEnd);
}

TEST_RESULT CShader5xTest_DoubleCondition::Setup()
{
	// Doubles are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Double instructions are only for Feature Level 11 or greater.") );
		return RESULT_SKIP;
	}

	// Doubles are optional. We should only run double tests if they are supported
	if( !g_TestApp.DoublesAreSupported() )
	{
		return RESULT_SKIP;
	}

	return CShader5xTest_Condition::Setup();
}

TEST_RESULT CShader5xTest_DoubleCondition::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Condition::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xy";

	char SrcBuff[2][256];

	GetOperandString(SrcBuff[0], _countof(SrcBuff[0]), 0, m_OperandType[0], m_SrcMod[0]);
	GetOperandString(SrcBuff[1], _countof(SrcBuff[1]), 1, m_OperandType[1], m_SrcMod[1]);		

	const char* szSrc[2] = { SrcBuff[0], SrcBuff[1] };
	
	return SetPipeline(szDest, szSrc, true);
}

TEST_RESULT CShader5xTest_DoubleCondition::ExecuteTestCase()
{
	TEST_RESULT tResult = RESULT_PASS;
	HRESULT hr;

	DOUBLE_INPUT_CLASS dicSrc0[2];
	DOUBLE_INPUT_CLASS dicSrc1[2];
	DOUBLE_CONDITION_RESULT_CLASS deExpectedResult[2];
	int i = 0;
	
	for (i = 0; i < 2; ++i)
	{
		dicSrc0[i] = GetInputClass( ResolveSrc(m_Src[0][i], m_SrcMod[0]) );
		dicSrc1[i] = GetInputClass( ResolveSrc(m_Src[1][i], m_SrcMod[1]) );
		deExpectedResult[i] = GetResultClass( dicSrc0[i], dicSrc1[i] );
	}

	// Set the constant buffer data
	DoubleConditionShaderParams pData[1];

	pData[0].src0 = m_Src[0];
	pData[0].src1 = m_Src[1];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_DoubleCondition::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
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
	UINT result[4] = {0, 0, 0, 0};
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_DoubleCondition::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	CShader5xTest_DoubleArith::Binary64 srcModified0, srcModified1;

	for (i = 0; i < 2; ++i)
	{
		if (deExpectedResult[i] == DCRC_CALC)
		{
			srcModified0 = ResolveSrc(m_Src[0][i], m_SrcMod[0]);
			srcModified1 = ResolveSrc(m_Src[1][i], m_SrcMod[1]);
			UINT calcResult = CalcResult( srcModified0, srcModified1 ) ? 0xFFFFFFFF : 0x00000000;
			if ( calcResult != result[i] )
			{
				WriteToLog( 1, "Expected (for component %d) %x : result %x.", i, calcResult, result[i] );
				tResult = RESULT_FAIL;
			}
		}
		else if(deExpectedResult[i] == DCRC_SRC0)
		{
			srcModified0 = ResolveSrc(m_Src[0][i], m_SrcMod[0]);
			srcModified1.m_AsULong[0] = result[(i == 1)? 2 : 0];
			srcModified1.m_AsULong[1] = result[(i == 1)? 3 : 1];
			if((srcModified0 == srcModified1) == false)
			{
				WriteToLog( 1, "Expected (for component %d) %x {0x%0I64X} : result %x {0x%0I64X}.", i, deExpectedResult[i], srcModified0.m_AsULongLong, result[i], srcModified1.m_AsULongLong );
				tResult = RESULT_FAIL;//{0x%16llX}
			}
		}
		else if(deExpectedResult[i] == DCRC_SRC1)
		{
			srcModified0 = ResolveSrc(m_Src[1][i], m_SrcMod[1]);
			srcModified1.m_AsULong[0] = result[(i == 1)? 2 : 0];
			srcModified1.m_AsULong[1] = result[(i == 1)? 3 : 1];
			if((srcModified0 == srcModified1) == false)
			{
				WriteToLog( 1, "Expected (for component %d) %x {0x%0I64X} : result %x {0x%0I64X}.", i, deExpectedResult[i], srcModified0.m_AsULongLong, result[i], srcModified1.m_AsULongLong );
				tResult = RESULT_FAIL;

			}
		}
		else if(deExpectedResult[i] == DCRC_SRC01)
		{
			srcModified0 = ResolveSrc(m_Src[0][i], m_SrcMod[0]);
			srcModified1 = ResolveSrc(m_Src[1][i], m_SrcMod[1]);

			CShader5xTest_DoubleArith::Binary64 actualResult;
			actualResult.m_AsULong[0] = result[(i == 1)? 2 : 0];
			actualResult.m_AsULong[1] = result[(i == 1)? 3 : 1];

			if( (actualResult != srcModified0) && (actualResult != srcModified1) )
			{
				WriteToLog( 1, "Expected (for component %d) either src0 {0x%0I64X} or src1 {0x%0I64X} : result {0x%0I64X}", i, srcModified0.m_AsULongLong, srcModified1.m_AsULongLong, actualResult.m_AsULongLong );
				tResult = RESULT_FAIL;
			}
		}
		else if(deExpectedResult[i] == DCRC_SCLC)
		{
			srcModified0 = ResolveSrc(m_Src[0][i], m_SrcMod[0]);
			srcModified1 = ResolveSrc(m_Src[1][i], m_SrcMod[1]);
			
			UINT calcResult = CalcResult( srcModified0, srcModified1 ) ? 0 : 1;
			srcModified0 = ResolveSrc(m_Src[calcResult][i], m_SrcMod[calcResult]);
			srcModified1.m_AsULong[0] = result[(i == 1)? 2 : 0];
			srcModified1.m_AsULong[1] = result[(i == 1)? 3 : 1];
			if((srcModified0 == srcModified1) == false)
			{
				WriteToLog( 1, "Expected (for component %d) %x {0x%0I64X} : result %x {0x%0I64X}.", i, deExpectedResult[i], srcModified0.m_AsULongLong, result[i], srcModified1.m_AsULongLong );
				(CShader5xTest_DoubleArith::IsZero(srcModified0) && CShader5xTest_DoubleArith::IsZero(srcModified1)) ? tResult = RESULT_SKIP : tResult = RESULT_FAIL;
				tResult = RESULT_FAIL;
			}
		}
		else if ( deExpectedResult[i] != result[i] )//(true or false {0xffffffff or 0x00000000})
		{
			WriteToLog( 1, "Expected (for component %d) %x : result %x.", i, deExpectedResult[i], result[i] );
			tResult = RESULT_FAIL;
		}
	}
	return tResult;
}

DOUBLE_INPUT_CLASS CShader5xTest_DoubleCondition::GetInputClass( 
							CShader5xTest_DoubleArith::Binary64 d )
{
	if(CShader5xTest_DoubleArith::IsNaN(d))// nan
	{
		return DIC_NAN;
	}
	if(d.m_AsULongLong == 0x3FF0000000000000)// +1
	{
		return DIC_ONEp;
	}
	if(d.m_AsULongLong == 0xBFF0000000000000)// -1
	{
		return DIC_ONEn;
	}

	if(d.m_AsULongLong & 0x8000000000000000)//sign?
	{
		if(CShader5xTest_DoubleArith::IsINF(d)) // -inf
		{
			return DIC_INFn;
		}
		if(CShader5xTest_DoubleArith::IsZero(d)) // -0
		{
			return DIC_ZEROn;
		}
		if(CShader5xTest_DoubleArith::IsDNorm(d)) //-dnorm
		{
			return DIC_DENn;
		}
		return DIC_REALn;// -real
	}
	else//not
	{
		if(CShader5xTest_DoubleArith::IsINF(d)) // +inf
		{
			return DIC_INFp;
		}
		if(CShader5xTest_DoubleArith::IsZero(d))// +0
		{
			return DIC_ZEROp;
		}
		if(CShader5xTest_DoubleArith::IsDNorm(d))// +denorm
		{
			return DIC_DENp;
		}
		return DIC_REALp;// +real
	}
}
CShader5xTest_DoubleArith::Binary64 
	CShader5xTest_DoubleCondition::ResolveSrc(CShader5xTest_DoubleArith::Binary64 d, SRC_MOD_TYPE mod )
{
	unsigned long long *pqwValue = (unsigned long long *) &d; // Need this to test for -0.0
	CShader5xTest_DoubleArith::Binary64 returnValue;
	unsigned long long *pqwReturnValue = (unsigned long long *) &returnValue; // Also needed for -0.0 case

	switch( mod )
	{
		default:
		case SMOD_NONE:
			returnValue = d;
			break;
			
		case SMOD_NEG:
			if( *pqwValue == 0x0000000000000000ull )
				*pqwReturnValue = 0x8000000000000000ull;
			else
				returnValue.m_AsDouble = -d.m_AsDouble;
			break;
			
		case SMOD_ABS:
			if( *pqwValue == 0x8000000000000000 )
				*pqwReturnValue = 0x0000000000000000;
			else
				returnValue.m_AsDouble = fabs( d.m_AsDouble);
			break;
			
		case SMOD_ABSNEG:
			if( *pqwValue == 0x0000000000000000 || *pqwValue == 0x8000000000000000 )
				*pqwReturnValue = 0x8000000000000000;
			else
				returnValue.m_AsDouble = -fabs( d.m_AsDouble);
			break;
	}
	return returnValue;
}


CShader5xTest_DoubleArith::Binary64
	CShader5xTest_DoubleCondition::ResolveDest(CShader5xTest_DoubleArith::Binary64 d, INST_MOD_TYPE mod)
{
	CShader5xTest_DoubleArith::Binary64 optRet;// may not always be returned
	optRet.m_AsULongLong = 0;
		
	switch(mod)
	{
		case IMOD_NONE:
			return d;
			break;
		
		case IMOD_SAT:
			if(CShader5xTest_DoubleArith::IsNaN(d))
				return optRet;

			min(0x3FF0000000000000, max(0x0000000000000000, d.m_AsDouble));
			return d;
			break;
	}
	return optRet;
}





//////////////////////////////////////////////////////////////////////////
// EQ
//////////////////////////////////////////////////////////////////////////
FLOAT_CONDITION_RESULT_CLASS EqualResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF			        -Real			-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -Real	*/	    {	FCRC_FALSE,		FCRC_CALC,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -1		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -denorm	*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -0		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +0		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +denorm	*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +1		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +Real	*/	    {	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_CALC,		FCRC_FALSE,		FCRC_FALSE },
/* +INF		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_FALSE },
/* NaN		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE }
};

FLOAT_CONDITION_RESULT_CLASS CShader5xTest_eq::GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 /* = FIC_NONE */ ) const
{
	return EqualResultTable[ficSrc0][ficSrc1];
}

bool CShader5xTest_eq::CalcResult(float srcModified0, float srcModified1)
{
	return srcModified0 == srcModified1;
}




//////////////////////////////////////////////////////////////////////////
// NE
//////////////////////////////////////////////////////////////////////////
FLOAT_CONDITION_RESULT_CLASS NotEqualResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF			-Real			-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* -Real	*/	{	FCRC_TRUE,		FCRC_CALC,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* -1		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* -denorm	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* -0		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* +0		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* +denorm	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* +1		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE },
/* +Real	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_CALC,		FCRC_TRUE,		FCRC_TRUE },
/* +INF		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_TRUE },
/* NaN		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE }
};

FLOAT_CONDITION_RESULT_CLASS CShader5xTest_ne::GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 /* = FIC_NONE */ ) const
{
	return NotEqualResultTable[ficSrc0][ficSrc1];
}

bool CShader5xTest_ne::CalcResult(float srcModified0, float srcModified1)
{
	return srcModified0 != srcModified1;
}




//////////////////////////////////////////////////////////////////////////
// DEQ
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblEqualResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF			-Real			-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -Real	*/	{	DCRC_FALSE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -1		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -denorm	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -0		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +0		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +denorm	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +1		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +Real	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE },
/* +INF		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_FALSE },
/* NaN		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE }
};
DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_deq::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = FIC_NONE */ ) const
{
	return dblEqualResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_deq::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if( CShader5xTest_DoubleArith::IsNaN(srcModified0) || CShader5xTest_DoubleArith::IsNaN(srcModified1) )
	{
		//no-op
	}
	else if(srcModified0 == srcModified1)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
// DGE
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblGreaterEqualResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF				-Real			-1				-denorm			-0			+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -Real	*/	{	DCRC_TRUE,		DCRC_CALC,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -1		*/	{	DCRC_TRUE,		DCRC_CALC,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -denorm	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* -0		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +0		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +denorm	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* +1		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE },
/* +Real	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_CALC,		DCRC_FALSE,		DCRC_FALSE },
/* +INF		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* NaN		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE }
};
DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_dge::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = FIC_NONE */ ) const
{
	return dblGreaterEqualResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_dge::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if( CShader5xTest_DoubleArith::IsNaN(srcModified0) || CShader5xTest_DoubleArith::IsNaN(srcModified1) )
	{
		// no-op
	}
	else if(srcModified0.m_AsDouble >= srcModified1.m_AsDouble)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
// DLT
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblLessThanResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF				-Real		-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* -Real	*/	{	DCRC_FALSE,		DCRC_CALC,		DCRC_CALC,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* -1		*/	{	DCRC_FALSE,		DCRC_CALC,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* -denorm	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* -0		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* +0		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* +denorm	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE },
/* +1		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_CALC,		DCRC_TRUE,		DCRC_FALSE },
/* +Real	*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_CALC,		DCRC_CALC,		DCRC_TRUE,		DCRC_FALSE },
/* +INF		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE },
/* NaN		*/	{	DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_FALSE }
};
DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_dlt::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = DIC_NONE */ ) const
{
	return dblLessThanResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_dlt::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if( CShader5xTest_DoubleArith::IsNaN(srcModified0) || CShader5xTest_DoubleArith::IsNaN(srcModified1) )
	{
		//no-op
	}
	else if(srcModified0.m_AsDouble < srcModified1.m_AsDouble)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
// DNE
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblNotEqualResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{	
/* SRC0 */		//	-INF				-Real		-1				-denorm			-0				+0				+denorm			+1				+Real			+INF				NaN
/* -INF		*/	{	DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* -Real	*/	{	DCRC_TRUE,		DCRC_CALC,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* -1		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* -denorm	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* -0		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* +0		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* +denorm	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* +1		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE },
/* +Real	*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_CALC,		DCRC_TRUE,		DCRC_TRUE },
/* +INF		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_FALSE,		DCRC_TRUE },
/* NaN		*/	{	DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE,		DCRC_TRUE }
};
DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_dne::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = FIC_NONE */ ) const
{
	return dblNotEqualResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_dne::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if( CShader5xTest_DoubleArith::IsNaN(srcModified0) && CShader5xTest_DoubleArith::IsNaN(srcModified1) )
	{
		// no-op
	}
	else if( (srcModified0.m_AsULongLong == srcModified1.m_AsULongLong) == true )
	{
		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// DMIN
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblMinResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{	
/* SRC0 */		//	-INF				-Real		       -1				-denorm			-0				+0				+denorm			+1				+Real			+INF				NaN
/* -INF		*/	{	DCRC_SRC1,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0},
/* -Real	*/	{	DCRC_SRC1,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* -1		*/	{	DCRC_SRC1,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* -denorm	*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* -0		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC01,		DCRC_SRC01,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* +0		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC01,		DCRC_SRC01,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* +denorm	*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* +1		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0 },
/* +Real	*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC0,		DCRC_SRC0 },
/* +INF		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* NaN		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 }
};

TEST_RESULT CShader5xTest_dmin::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	// Do not call inherited SetupTestCase
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xyzw";

	char SrcBuff[2][256];

	GetOperandString(SrcBuff[0], _countof(SrcBuff[0]), 0, m_OperandType[0], m_SrcMod[0]);
	GetOperandString(SrcBuff[1], _countof(SrcBuff[1]), 1, m_OperandType[1], m_SrcMod[1]);		

	const char* szSrc[2] = { SrcBuff[0], SrcBuff[1] };
	
	return SetPipeline(szDest, szSrc, true);
}

DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_dmin::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = FIC_NONE */ ) const
{
	return dblMinResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_dmin::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if(srcModified0.m_AsDouble < srcModified1.m_AsDouble)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
// DMAX
//////////////////////////////////////////////////////////////////////////
DOUBLE_CONDITION_RESULT_CLASS dblMaxResultTable[NUM_DOUBLE_INPUT_CLASSES][NUM_DOUBLE_INPUT_CLASSES] =
{	
/* SRC0 */		//	-INF				-Real		       -1				-denorm			-0				+0				+denorm			+1				+Real			+INF				NaN
/* -INF		*/	{	DCRC_SRC0,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0},
/* -Real	*/	{	DCRC_SRC0,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* -1		*/	{	DCRC_SRC0,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* -denorm	*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* -0		*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC01,		DCRC_SRC01,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* +0		*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC01,		DCRC_SRC01,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* +denorm	*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 },
/* +1		*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC0 },
/* +Real	*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SCLC,		DCRC_SCLC,		DCRC_SRC1,		DCRC_SRC0 },
/* +INF		*/	{	DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0,		DCRC_SRC0 },
/* NaN		*/	{	DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC1,		DCRC_SRC0 }
};

TEST_RESULT CShader5xTest_dmax::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	// Do not call inherited SetupTestCase
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xyzw";

	char SrcBuff[2][256];

	GetOperandString(SrcBuff[0], _countof(SrcBuff[0]), 0, m_OperandType[0], m_SrcMod[0]);
	GetOperandString(SrcBuff[1], _countof(SrcBuff[1]), 1, m_OperandType[1], m_SrcMod[1]);		

	const char* szSrc[2] = { SrcBuff[0], SrcBuff[1] };
	
	return SetPipeline(szDest, szSrc, true);
}

DOUBLE_CONDITION_RESULT_CLASS CShader5xTest_dmax::GetResultClass( DOUBLE_INPUT_CLASS dicSrc0, DOUBLE_INPUT_CLASS dicSrc1 /* = FIC_NONE */ ) const
{
	return dblMaxResultTable[dicSrc0][dicSrc1];
}
bool CShader5xTest_dmax::CalcResult(CShader5xTest_DoubleArith::Binary64 srcModified0, CShader5xTest_DoubleArith::Binary64 srcModified1)
{
	if(srcModified0.m_AsDouble >= srcModified1.m_AsDouble)
	{
		return true;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
// GE
//////////////////////////////////////////////////////////////////////////
FLOAT_CONDITION_RESULT_CLASS GreaterEqualResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF			-Real			-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -Real	*/	{	FCRC_TRUE,		FCRC_CALC,		FCRC_CALC,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -1		*/	{	FCRC_TRUE,		FCRC_CALC,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -denorm	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* -0		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +0		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +denorm	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* +1		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_CALC,		FCRC_FALSE,		FCRC_FALSE },
/* +Real	*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_CALC,		FCRC_CALC,		FCRC_FALSE,		FCRC_FALSE },
/* +INF		*/	{	FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* NaN		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE }
};

FLOAT_CONDITION_RESULT_CLASS CShader5xTest_ge::GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 /* = FIC_NONE */ ) const
{
	return GreaterEqualResultTable[ficSrc0][ficSrc1];
}

bool CShader5xTest_ge::CalcResult(float srcModified0, float srcModified1)
{
	return srcModified0 >= srcModified1;
}





//////////////////////////////////////////////////////////////////////////
// LT
//////////////////////////////////////////////////////////////////////////
FLOAT_CONDITION_RESULT_CLASS LessThanResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF			-Real			-1				-denorm			-0				+0				+denorm			+1				+Real			+INF			NaN
/* -INF		*/	{	FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* -Real	*/	{	FCRC_FALSE,		FCRC_CALC,		FCRC_CALC,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* -1		*/	{	FCRC_FALSE,		FCRC_CALC,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* -denorm	*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* -0		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* +0		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* +denorm	*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_TRUE,		FCRC_FALSE },
/* +1		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_CALC,		FCRC_TRUE,		FCRC_FALSE },
/* +Real	*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_CALC,		FCRC_CALC,		FCRC_TRUE,		FCRC_FALSE },
/* +INF		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE },
/* NaN		*/	{	FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE,		FCRC_FALSE }
};

FLOAT_CONDITION_RESULT_CLASS CShader5xTest_lt::GetResultClass( FLOAT_INPUT_CLASS ficSrc0, FLOAT_INPUT_CLASS ficSrc1 /* = FIC_NONE */ ) const
{
	return LessThanResultTable[ficSrc0][ficSrc1];
}

bool CShader5xTest_lt::CalcResult(float srcModified0, float srcModified1)
{
	return srcModified0 < srcModified1;
}





//////////////////////////////////////////////////////////////////////////
// Int Condition Instructions
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_IntCondition::InitTestParameters()
{
	testfactor::RFactor src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< int4 >( &IntVectorConditionValues[0], sizeof( IntVectorConditionValues[0] ), sizeof( IntVectorConditionValues ) / sizeof( IntVectorConditionValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter( "Src1", &m_Src[1], new CTableValueSet< int4 >( &IntVectorConditionValues[0], sizeof( IntVectorConditionValues[0] ), sizeof( IntVectorConditionValues ) / sizeof( IntVectorConditionValues[0] ) ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0 * src1;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}


TEST_RESULT CShader5xTest_IntCondition::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Condition::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xyzw";
	const char *szSrc[2];
	
	szSrc[0] = ", r7";
	szSrc[1] = ", r8";
		
	return SetPipeline(szDest, szSrc);
}

TEST_RESULT CShader5xTest_IntCondition::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	// Set the constant buffer data
	IntConditionShaderParams pData[1];

	pData[0].src0 = m_Src[0];
	pData[0].src1 = m_Src[1];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_IntCondition::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
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
	DWORD result[4];
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_IntCondition::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	for (int i=0; i<4; i++)
	{
		DWORD calcResult = CalcResult( m_Src[0][i], m_Src[1][i] ) ? 0xFFFFFFFF : 0x00000000;
		if ( calcResult != result[i] )
		{
			WriteToLog( 1, "Expected (for component %d) 0x%x : result 0x%x.", i, calcResult, result[i] );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}

//////////////////////////////////////////////////////////////////////////
// IEQ
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_ieq::CalcResult(int src0, int src1)
{
	return src0 == src1;
}

//////////////////////////////////////////////////////////////////////////
// INE
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_ine::CalcResult(int src0, int src1)
{
	return src0 != src1;
}

//////////////////////////////////////////////////////////////////////////
// IGE
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_ige::CalcResult(int src0, int src1)
{
	return src0 >= src1;
}

//////////////////////////////////////////////////////////////////////////
// ILT
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_ilt::CalcResult(int src0, int src1)
{
	return src0 < src1;
}

//////////////////////////////////////////////////////////////////////////
// Unsigned Int Condition Instructions
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_UIntCondition::InitTestParameters()
{
	testfactor::RFactor src0 = AddParameter( "Src0", &m_Src[0], new CTableValueSet< uint4 >( &UIntVectorConditionValues[0], sizeof( UIntVectorConditionValues[0] ), sizeof( UIntVectorConditionValues ) / sizeof( UIntVectorConditionValues[0] ) ) );
	testfactor::RFactor src1 = AddParameter( "Src1", &m_Src[1], new CTableValueSet< uint4 >( &UIntVectorConditionValues[0], sizeof( UIntVectorConditionValues[0] ), sizeof( UIntVectorConditionValues ) / sizeof( UIntVectorConditionValues[0] ) ) );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src0 * src1;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}


TEST_RESULT CShader5xTest_UIntCondition::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Condition::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	const char *szDest = "r6.xyzw";
	const char *szSrc[2];
	
	szSrc[0] = ", r7";
	szSrc[1] = ", r8";
		
	return SetPipeline(szDest, szSrc);
}

TEST_RESULT CShader5xTest_UIntCondition::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;

	//
	// Set the constant buffer data
	//
	UIntConditionShaderParams pData[1];

	pData[0].src0 = m_Src[0];
	pData[0].src1 = m_Src[1];

	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, pData ) ) )
	{
		WriteToLog( _T( "CShader5xTest_UIntCondition::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
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
	DWORD result[4];
	if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result ) ) )
	{
		WriteToLog( _T( "CShader5xTest_UIntCondition::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	for (int i=0; i<4; i++)
	{
		DWORD calcResult = CalcResult( m_Src[0][i], m_Src[1][i] ) ? 0xFFFFFFFF : 0x00000000;
		if ( calcResult != result[i] )
		{
			WriteToLog( 1, "Expected 0x%x : result 0x%x.", calcResult, result[i] );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}

//////////////////////////////////////////////////////////////////////////
// UGE
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_uge::CalcResult(UINT src0, UINT src1)
{
	return src0 >= src1;
}

//////////////////////////////////////////////////////////////////////////
// ULT
//////////////////////////////////////////////////////////////////////////

bool CShader5xTest_ult::CalcResult(UINT src0, UINT src1)
{
	return src0 < src1;
}
