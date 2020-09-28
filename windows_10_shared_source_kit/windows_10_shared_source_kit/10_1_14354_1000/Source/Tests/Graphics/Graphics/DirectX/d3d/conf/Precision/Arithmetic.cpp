#include "Arithmetic.h"
#include "strsafe.h"

//---------------------------------------------------------------

BEGIN_NAMED_VALUES( ARITH_OP )
	NAMED_VALUE( _T( "add" ), AO_ADD )
	NAMED_VALUE( _T( "max" ), AO_MAX )
	NAMED_VALUE( _T( "min" ), AO_MIN )
	NAMED_VALUE( _T( "exp" ), AO_EXP )
	NAMED_VALUE( _T( "rcp" ), AO_RCP )
	NAMED_VALUE( _T( "rsq" ), AO_RSQ )
	NAMED_VALUE( _T( "sqrt" ), AO_SQRT )
	NAMED_VALUE( _T( "mad" ), AO_MAD )
	NAMED_VALUE( _T( "iadd" ), AO_IADD )
	NAMED_VALUE( _T( "imax" ), AO_IMAX )
	NAMED_VALUE( _T( "imin" ), AO_IMIN )
	NAMED_VALUE( _T( "imad" ), AO_IMAD )
	NAMED_VALUE( _T( "uadd" ), AO_UADD )
	NAMED_VALUE( _T( "umax" ), AO_UMAX )
	NAMED_VALUE( _T( "umin" ), AO_UMIN )
	NAMED_VALUE( _T( "umad" ), AO_UMAD )
	NAMED_VALUE( _T( "mov" ), AO_MOV )
	NAMED_VALUE( _T( "movc" ), AO_MOVC )
	NAMED_VALUE( _T( "imov" ), AO_IMOV )
	NAMED_VALUE( _T( "imovc" ), AO_IMOVC )
	NAMED_VALUE( _T( "umov" ), AO_UMOV )
	NAMED_VALUE( _T( "umovc" ), AO_UMOVC )
	NAMED_VALUE( _T( "swapc" ), AO_SWAPC )
	NAMED_VALUE( _T( "iswapc" ), AO_ISWAPC )
	NAMED_VALUE( _T( "uswapc" ), AO_USWAPC )
END_NAMED_VALUES( ARITH_OP )

BEGIN_NAMED_VALUES( CArithmeticPrecisionTest::EValueType )
	NAMED_VALUE( _T( "float" ), CArithmeticPrecisionTest::VT_Float )
	NAMED_VALUE( _T( "uint" ), CArithmeticPrecisionTest::VT_Uint )
	NAMED_VALUE( _T( "int" ), CArithmeticPrecisionTest::VT_Int )
	NAMED_VALUE( _T( "double" ), CArithmeticPrecisionTest::VT_Double )
END_NAMED_VALUES( CArithmeticPrecisionTest::EValueType )


/*************************************************************
* CArithmeticPrecision*Test Setup and Initialization methods *
**************************************************************/

TEST_RESULT CArithmeticUintTest::Setup()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		WriteToLog( "UINT is not supported on 9.x feature levels" );
		return RESULT_SKIP;
	}

	return CPrecisionTest::Setup();
}

TEST_RESULT CArithmeticIntTest::Setup()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		WriteToLog( "INT is not supported on 9.x feature levels" );
		return RESULT_SKIP;
	}

	return CPrecisionTest::Setup();
}

void CArithmeticPrecisionTest::InitTestParameters()
{
	// RefreshGraphics() in order to ensure the framework device is available during parameter initialization
	// BUGBUG: WinBlue 551704: This is a hack to enable the scenario of accessing the device in InitTestParameters()
	// This should not be copied into other tests. It may not be supported in the future.
	GetFramework()->RefreshGraphics();

	// Initialize test factors
	testfactor::RFactor rOpAndValuesAndMinPrecisionAndSrcMod = InitializeOpAndValueAndMinPrecisionAndModFactor();
	testfactor::RFactor rShaderStage = InitializeShaderStageModifierFactor();
	testfactor::RFactor rOperandType = InitializeOperandTypeFactor();
	testfactor::RFactor rModLocation = InitializeLocationFactor();

	SetRootTestFactor( rOpAndValuesAndMinPrecisionAndSrcMod % rOperandType % rModLocation % rShaderStage );

	// Set priority values
	CTestCaseParameter<D3D_MIN_PRECISION>* pMinPrecisionParam = static_cast<CTestCaseParameter<D3D_MIN_PRECISION>*>( GetTestCaseParameter( _T( "MinPrecision" ) ) );
	CTestCaseParameter<ARITH_OP>* pInstructionParam = static_cast<CTestCaseParameter<ARITH_OP>*>( GetTestCaseParameter( _T( "Inst" ) ) );
	CTestCaseParameter<SRC_MOD_TYPE>* pSrcModParam = static_cast<CTestCaseParameter<SRC_MOD_TYPE>*>( GetTestCaseParameter( _T( "SrcMod" ) ) );
	assert( pMinPrecisionParam != NULL );
	assert( pInstructionParam != NULL );
	assert( pSrcModParam != NULL );

	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );
	AddPriorityPatternFilter( FilterNotEqual<ARITH_OP>(pInstructionParam, AO_ADD), WeightFactorLarge );
	AddPriorityPatternFilter( FilterNotEqual<ARITH_OP>(pInstructionParam, AO_RCP), WeightFactorLarge );
	AddPriorityPatternFilter( FilterNotEqual<ARITH_OP>(pInstructionParam, AO_MAD), WeightFactorLarge );
	AddPriorityPatternFilter( FilterNotEqual<ARITH_OP>(pInstructionParam, AO_IADD), WeightFactorLarge );
	AddPriorityPatternFilter( FilterNotEqual<ARITH_OP>(pInstructionParam, AO_UMAX), WeightFactorLarge );
	AddPriorityPatternFilter( FilterNotEqual<SRC_MOD_TYPE>(pSrcModParam, SMOD_NONE), WeightFactorLarge );

	DeclarePriorityLevelByPriorityValue( 1, 22.0f );
	DeclarePriorityLevelByPriorityValue( 2, 11.0f );
}

// This function will create a test factor that fills in Src0, Src1, Src2 (if NumOperands is 3) with values of type T.
// The values in pValues will be set to Src0<pPostFix>, Src1<pPostFix>, Src2<pPostFix> using pairwise cross factors.
// If pValues2 is set, it will be used for Src1 and Src2 instead of pValues
// Every Components (4) values Src0, Src1, Src2 will be combined into one Vec4<T> value.
// Use pPostFix to change the parameter name because you don't want UINT and FLOAT values in the same parameter.
// Components is the number of components in a resulting set.  Typically UINT/float/int will use 4, and doubles use 2.
// For example, if pValues contains {1,2,3} and NumOperands == 2 and T = UINT and pPostFix = ""
//     Then the pairwise mapping would be Src0 = 1,1,1,2,2,2,3,3,3 and Src1 = 1,2,3,1,2,3,1,2,3
//     So the resulting Val4s will be Src0 = (1,1,1,2),(2,2,3,3),(3,x,x,x) and Src1 = (1,2,3,1),(2,3,1,2),(3,x,x,x), where x is anything
template< class T, UINT NumOperands >
testfactor::RFactor CArithmeticPrecisionTest::InitializeValueFactor( CArithmeticPrecisionTest* pThis, CUserValueSet<T>* pValues, Vec4<T>* BackingStore, CUserValueSet<T>* pValues2Plus = NULL, UINT Components = 4, char* pPostFix = "")
{
	if( pValues->GetSize() == 0 )
	{
		testfactor::RFactor rEmpty;
		return rEmpty;
	}

	// Make a copy because ValueGenerator will delete the value set in its destructor
	CUserValueSet<T>* pValuesCopy = new CUserValueSet<T>( *pValues );
	CUserValueSet<T>* pValues2PlusCopy = pValues2Plus ? new CUserValueSet<T>( *pValues2Plus ) : pValuesCopy;

	// The Vec4 ValueSets that will be filled
	CUserValueSet<Vec4<T>>* pVec4Src[NumOperands];
	for( UINT i=0; i < NumOperands; i++ )
	{
		pVec4Src[i] = new CUserValueSet<Vec4<T>>();
	}

	// This scope will create a temporary CVariationGenerator that will be used to do the pairwise cross factoring
	// The "columns" of the result set are operands.  The "rows" each represent a component of a register (x,y,z,w)
	// These rows will be collected in groups of four to create a single 4-component register for each operand, stored in pVec4Src.
	{
		T TempValues[NumOperands];	// stores the result of the cross factor for each operand
		Vec4<T> Src[NumOperands];	// this is filled, one component at a time until full, then the full Vec4 is added to pVec4Src
		ZeroMemory( Src, sizeof(Src) );

		// Create a temporary ValueGenerator that will perform the pairwise cross factor 
		CVariationGenerator ValueGenerator;
		testfactor::RFactor rOperands[NumOperands];
		for( UINT i=0; i < NumOperands; i++ )
		{
			char pName[4];
			sprintf( pName, "%d", i );

			// Add each operand to the value generator
			CTestCaseParameter<T> *pParam = ValueGenerator.AddParameter( pName, &TempValues[i] );
			rOperands[i] = ValueGenerator.AddParameterValueSet( pParam, i == 0 ? pValuesCopy : pValues2PlusCopy );
		}

		testfactor::XFACTOR pairwiseTable[NumOperands];
		for( UINT i=0; i < NumOperands; i++ )
		{
			pairwiseTable[i].rFactor = rOperands[i];
			pairwiseTable[i].iGroup = 1;
			pairwiseTable[i].pNotGroups = NULL;
			pairwiseTable[i].cNotGroups = 0;
		};
		size_t groupOrders[] = 
		{
			XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
		};
		testfactor::RFactor rAll = NewCrossFactor( pairwiseTable, _countof(pairwiseTable), _T("pairwiseTable"), groupOrders, _countof(groupOrders) );
		ValueGenerator.SetRootTestFactor( rAll );

		const UINT count = ValueGenerator.GetNumVariations();

		// For each result (a single value for each of the operands), add it to a new component of Src
		for( UINT i=0; i < count; i++ )
		{
			ValueGenerator.GenerateVariation( i + 1 );
			const UINT comp = i % Components;
			for( UINT j=0; j < NumOperands; j++ )
			{
				Src[j][comp] = TempValues[j];
			}

			// If we have filled in all components of Src, save the result and start filling Src again
			if( comp == Components - 1 || i == count-1 )
			{
				for( UINT j=0; j < NumOperands; j++ )
				{
					pVec4Src[j]->AddValue( Src[j] );
				}
			}
		}
	}

	// At this point, pVec4Src[i] has been filled with a set of register values (4 components) for each operand i

	testfactor::RFactor rAll;
	for( UINT i=0; i < NumOperands; i++ )
	{
		char pName[8];
		sprintf( pName, "Src%d%s", i, pPostFix );

		// This function can be called multiple times, so don't create the parameter more than once.
		CTestCaseParameterBase* pExistingParameter = pThis->GetTestCaseParameter( pName );
		CTestCaseParameter<Vec4<T>> *pParam = pExistingParameter ? static_cast<CTestCaseParameter<Vec4<T>>*>(pExistingParameter) 
			                                                     : pThis->AddParameter( pName, &BackingStore[i] );

		// Add the value set for Src0, Src1, or Src2
		testfactor::RFactor rSrcFactor = pThis->AddParameterValueSet( pParam, pVec4Src[i] );

		// Set default value
		if( !pExistingParameter )
		{
			Vec4<T> pVec4Default;
			pVec4Default[0] = pVec4Default[1] = pVec4Default[2] = pVec4Default[3] = 0;
			pThis->SetParameterDefaultValue<Vec4<T>>( pParam, pVec4Default );
		}

		if( i == 0 )
		{
			rAll = rSrcFactor;
		}
		else
		{
			// Use % to accumulate the results because each register of Src0 is matches up with the respective register of Src1
			// (because those values were pairwised together)
			rAll = rAll % rSrcFactor;
		}
	}

	return rAll;
}

template testfactor::RFactor CArithmeticPrecisionTest::InitializeValueFactor<int,1>( CArithmeticPrecisionTest* pThis, CUserValueSet<int>* pValues, Vec4<int>* BackingStore, CUserValueSet<int>* pValues2Plus, UINT Components, char* pPostFix );
template testfactor::RFactor CArithmeticPrecisionTest::InitializeValueFactor<UINT,1>( CArithmeticPrecisionTest* pThis, CUserValueSet<UINT>* pValues, Vec4<UINT>* BackingStore, CUserValueSet<UINT>* pValues2Plus, UINT Components, char* pPostFix );
template testfactor::RFactor CArithmeticPrecisionTest::InitializeValueFactor<double,1>( CArithmeticPrecisionTest* pThis, CUserValueSet<double>* pValues, Vec4<double>* BackingStore, CUserValueSet<double>* pValues2Plus, UINT Components, char* pPostFix );


testfactor::RFactor CArithmeticUintTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	// Min Precision
	testfactor::RFactor rMinPrecisionInt;
	InitializeMinPrecisionFactor( &rMinPrecisionInt );

	// Values
	CUserValueSet<UINT>* pValues = new CUserValueSet<UINT>( );
	pValues->AddValue( 0 );
	pValues->AddValue( 1 );
	pValues->AddValue( 70 );
	pValues->AddValue( USHORT_MAX );
	pValues->AddValue( USHORT_MAX - 1 );
	pValues->AddValue( USHORT_MAX + 1 );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesUint2 = new CUserValueSet<ARITH_OP>( );
	pValuesUint2->AddValue( AO_UADD );
	pValuesUint2->AddValue( AO_UMAX );
	pValuesUint2->AddValue( AO_UMIN );
	CUserValueSet<ARITH_OP>* pValuesUint3 = new CUserValueSet<ARITH_OP>( );
	pValuesUint3->AddValue( AO_UMAD );
	testfactor::RFactor rUintOp2Factor = AddParameterValueSet( pInstructionParam, pValuesUint2 );
	testfactor::RFactor rUintOp3Factor = AddParameterValueSet( pInstructionParam, pValuesUint3 );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_UADD );

	// Create value sets for the 2-operand and 3-operand operations
	testfactor::RFactor rValue2Factor = InitializeValueFactor<UINT,2>( this, pValues, m_SrcUint );
	testfactor::RFactor rValue3Factor = InitializeValueFactor<UINT,3>( this, pValues, m_SrcUint );

	testfactor::RFactor rOpAndValue = rUintOp2Factor * rValue2Factor + rUintOp3Factor * rValue3Factor;
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();

	return rMinPrecisionInt * rOpAndValue % ( rSrcMod * rInstMod );
}

testfactor::RFactor CArithmeticIntTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	// Min Precision
	testfactor::RFactor rMinPrecisionInt;
	InitializeMinPrecisionFactor( &rMinPrecisionInt );

	// Values
	CUserValueSet<int>* pValues = new CUserValueSet<int>( );
	pValues->AddValue( 0 );
	pValues->AddValue( 1 );
	pValues->AddValue( -1 );
	pValues->AddValue( 45 );
	pValues->AddValue( SHORT_MAX );
	pValues->AddValue( SHORT_MIN );
	pValues->AddValue( SHORT_MAX + 1 );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesInt2 = new CUserValueSet<ARITH_OP>( );
	pValuesInt2->AddValue( AO_IADD );
	pValuesInt2->AddValue( AO_IMAX );
	pValuesInt2->AddValue( AO_IMIN );
	CUserValueSet<ARITH_OP>* pValuesInt3 = new CUserValueSet<ARITH_OP>( );
	pValuesInt3->AddValue( AO_IMAD );
	testfactor::RFactor rIntOp2Factor = AddParameterValueSet( pInstructionParam, pValuesInt2 );
	testfactor::RFactor rIntOp3Factor = AddParameterValueSet( pInstructionParam, pValuesInt3 );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_IADD );

	// Create value sets for the 2-operand and 3-operand operations
	testfactor::RFactor rValue2Factor = InitializeValueFactor<int,2>( this, pValues, m_SrcInt );
	testfactor::RFactor rValue3Factor = InitializeValueFactor<int,3>( this, pValues, m_SrcInt );

	testfactor::RFactor rOpAndValue = rIntOp2Factor * rValue2Factor + rIntOp3Factor * rValue3Factor;
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();

	return rMinPrecisionInt * rOpAndValue % ( rSrcMod * rInstMod );
}

testfactor::RFactor CArithmeticFloatTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetFramework()->m_D3DOptions.FeatureLevel;

	// Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "MinPrecision" ), &m_MinPrecision );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesFloat2 = new CUserValueSet<ARITH_OP>( );
	pValuesFloat2->AddValue( AO_ADD ); // 2
	pValuesFloat2->AddValue( AO_MAX ); // 2
	pValuesFloat2->AddValue( AO_MIN ); // 2
	CUserValueSet<ARITH_OP>* pValuesFloat1 = new CUserValueSet<ARITH_OP>( );
	pValuesFloat1->AddValue( AO_EXP ); // 1
	CUserValueSet<ARITH_OP>* pValuesNonZr1 = new CUserValueSet<ARITH_OP>( );	// 1/x requires x != 0
	pValuesNonZr1->AddValue( AO_RCP ); // 1
	CUserValueSet<ARITH_OP>* pValuesPosit1 = new CUserValueSet<ARITH_OP>( );	// 1/sqrt(x) requires x > 0
	pValuesPosit1->AddValue( AO_RSQ ); // 1
	CUserValueSet<ARITH_OP>* pValuesNonNg1 = new CUserValueSet<ARITH_OP>( );	// sqrt(x) requires x >= 0
	pValuesNonNg1->AddValue( AO_SQRT ); // 1
	CUserValueSet<ARITH_OP>* pValuesFloat3 = new CUserValueSet<ARITH_OP>( );
	pValuesFloat3->AddValue( AO_MAD ); // 3
	testfactor::RFactor rFloatOp1Factor = AddParameterValueSet( pInstructionParam, pValuesFloat1 );
	testfactor::RFactor rNonZrOp1Factor = AddParameterValueSet( pInstructionParam, pValuesNonZr1 );
	testfactor::RFactor rPositOp1Factor = AddParameterValueSet( pInstructionParam, pValuesPosit1 );
	testfactor::RFactor rNonNgOp1Factor = AddParameterValueSet( pInstructionParam, pValuesNonNg1 );
	testfactor::RFactor rFloatOp2Factor = AddParameterValueSet( pInstructionParam, pValuesFloat2 );
	testfactor::RFactor rFloatOp3Factor = AddParameterValueSet( pInstructionParam, pValuesFloat3 );
	testfactor::RFactor rFloatAddFactor = AddParameterValue( pInstructionParam, AO_ADD );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_ADD );

	// Source Modifier
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	CUserValueSet<SRC_MOD_TYPE> *pModValues = new CUserValueSet<SRC_MOD_TYPE>( );
	// 9.x shaders do not support NAN or INF literals, so make sure we don't do anything like sqrt(-a), where a > 0
	pModValues->AddValue( SMOD_NONE );
	pModValues->AddValue( SMOD_ABS );
	testfactor::RFactor rPositiveSrcModFactor = AddParameterValueSet( _T( "SrcMod" ), pModValues );

	// Instruction Modifier
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();


	// Min Precision and Values
	testfactor::RFactor rAllFactors;
	for( EPrecisionLevel minprec = EPL_32; minprec >= EPL_10; ((UINT&)minprec) >>= 1 )
	{
		D3D_MIN_PRECISION level = CMinPrecValue<float>::ConvertToPrecisionLevel( minprec );

		testfactor::RFactor rMinPrecision = AddParameterValue( pMinPrecisionParam, level );

		// Values (use appropriate values for min precision)
		CUserValueSet<float>* pFloatsNegative = new CUserValueSet<float>( );
		pFloatsNegative->AddValue( -1.0f );
		if( minprec == EPL_10 )
		{
			pFloatsNegative->AddValue( -1.0f / 255.0f );
		}
		if( minprec >= EPL_10 )
		{
			pFloatsNegative->AddValue( -1.25f );
			pFloatsNegative->AddValue( -2.0f );
			pFloatsNegative->AddValue( -3.88f );
		}

		CUserValueSet<float>* pFloatsZero = new CUserValueSet<float>( );
		pFloatsZero->AddValue( 0.0f );

		CUserValueSet<float>* pFloatsPositive = new CUserValueSet<float>( );
		pFloatsPositive->AddValue( 1.0f / 255.0f );
		if( minprec == EPL_10 )
		{
			pFloatsPositive->AddValue( 127.0f / 255.0f );
			pFloatsPositive->AddValue( 254.0f / 255.0f );
		}
		pFloatsPositive->AddValue( 1.0f );
		pFloatsPositive->AddValue( 1.0f + 1.0f / 255.0f );
		if( minprec >= EPL_16 )
		{
			pFloatsPositive->AddValue( g_fDenormFloat16 );
			pFloatsPositive->AddValue( 12345678 );	// out of range of float16 (approx max 2 ^ 16 = 65000)
		}
		else if( minprec == EPL_10 )
		{
			pFloatsPositive->AddValue( 1.0f + 255.0f / 256.0f );
			pFloatsPositive->AddValue( 3.0f );
		}

		// INF, -INF  (only available in 10+)
		CUserValueSet<float>* pFloatsInteresting = new CUserValueSet<float>( );
		if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
		{
			pFloatsInteresting->AddValue( g_fINFn32 );
			pFloatsInteresting->AddValue( g_fINFp32 );
		}

		// Create aggregate value sets
		CUserValueSet<float> NonNegative = *pFloatsZero | *pFloatsPositive;
		CUserValueSet<float> NonZero = *pFloatsNegative | *pFloatsPositive;
		CUserValueSet<float> Floats = NonZero | *pFloatsZero;
		CUserValueSet<float>* pNonNegative = &NonNegative;
		CUserValueSet<float>* pNonZero = &NonZero;
		CUserValueSet<float>* pFloats = &Floats;

		// Create cross factor value sets for the 1-operand, 2-operand and 3-operand operations
		testfactor::RFactor rValue1Factor = InitializeValueFactor<float,1>( this, pFloats, m_SrcFloat );
		testfactor::RFactor rValue1PosFactor = InitializeValueFactor<float,1>( this, pFloatsPositive, m_SrcFloat );
		testfactor::RFactor rValue1NonNegFactor = InitializeValueFactor<float,1>( this, pNonNegative, m_SrcFloat );
		testfactor::RFactor rValue1NonZero = InitializeValueFactor<float,1>( this, pNonZero, m_SrcFloat );
		testfactor::RFactor rValue2Factor = InitializeValueFactor<float,2>( this, pFloats, m_SrcFloat );
		testfactor::RFactor rValue3Factor = InitializeValueFactor<float,3>( this, pFloats, m_SrcFloat );

		// Create cross factor value sets where INF is used in the first parameter
		// These are done separately because we don't need to test INF with as many other operand combinations
		testfactor::RFactor rValue1FactorINF = InitializeValueFactor<float,1>( this, pFloatsInteresting, m_SrcFloat );
		testfactor::RFactor rValue2FactorINF = InitializeValueFactor<float,2>( this, pFloatsInteresting, m_SrcFloat, pFloatsPositive );
		testfactor::RFactor rValue3FactorINF = InitializeValueFactor<float,3>( this, pFloatsInteresting, m_SrcFloat, pFloatsPositive );

		
		testfactor::RFactor rAllOperands;
		{
			rAllOperands =
			(
				(
					rFloatOp1Factor * ( rValue1Factor + rValue1FactorINF ) +
					rNonZrOp1Factor * ( rValue1NonZero + rValue1FactorINF ) +
					rFloatOp2Factor * ( rValue2Factor + rValue2FactorINF ) +
					rFloatOp3Factor * ( rValue3Factor + rValue3FactorINF )
				) % ( rSrcMod * rInstMod ) + 
				(
					rPositOp1Factor * ( rValue1PosFactor + rValue1FactorINF ) +
					rNonNgOp1Factor * ( rValue1NonNegFactor + rValue1FactorINF )
				) % ( rPositiveSrcModFactor * rInstMod )
			);
		}
		testfactor::RFactor rCurFactor = rMinPrecision * rAllOperands;

		rAllFactors = rAllFactors + rCurFactor;
	}

	return rAllFactors;
}



/*************************************************************
* Shader generation methods                                  *
**************************************************************/

CArithmeticPrecisionTest::EValueType CArithmeticPrecisionTest::GetValueType( ARITH_OP op )
{
	switch( op )
	{
	case AO_ADD:
	case AO_MAX:
	case AO_MIN:
	case AO_EXP:
	case AO_RCP:
	case AO_RSQ:
	case AO_SQRT:
	case AO_MAD:
	case AO_MOV:
	case AO_MOVC:
	case AO_SWAPC:
		return VT_Float;
	case AO_IADD:
	case AO_IMAX:
	case AO_IMIN:
	case AO_IMAD:
	case AO_IMOV:
	case AO_IMOVC:
	case AO_ISWAPC:
		return VT_Int;
	case AO_UADD:
	case AO_UMAX:
	case AO_UMIN:
	case AO_UMAD:
	case AO_UMOV:
	case AO_UMOVC:
	case AO_USWAPC:
		return VT_Uint;
	}
	assert( false );
	return VT_Float;
}

string CArithmeticPrecisionTest::OperationToString( string pSrc0, string pSrc1, string pSrc2 )
{
	// Source modifier
	switch( m_SrcModifier )
	{
	case SMOD_NONE:
		break;
	case SMOD_NEG:
		pSrc0 = "-" + pSrc0;
		pSrc1 = "-" + pSrc1;
		pSrc2 = "-" + pSrc2;
		break;
	case SMOD_ABS:
		pSrc0 = "abs(" + pSrc0 + ")";
		pSrc1 = "abs(" + pSrc1 + ")";
		pSrc2 = "abs(" + pSrc2 + ")";
		break;
	case SMOD_ABSNEG:
		pSrc0 = "-abs(" + pSrc0 + ")";
		pSrc1 = "-abs(" + pSrc1 + ")";
		pSrc2 = "-abs(" + pSrc2 + ")";
		break;
	}

	string result = "";

	switch( m_Op )
	{
	case AO_ADD:
	case AO_IADD:
	case AO_UADD: 
		result = pSrc0 + " + " + pSrc1; 
		break;
	case AO_MAX: 
	case AO_IMAX: 
	case AO_UMAX: 
		result = "max(" + pSrc0 + "," + pSrc1 + ")";
		break;
	case AO_MIN:
	case AO_IMIN: 
	case AO_UMIN: 
		result = "min(" + pSrc0 + "," + pSrc1 + ")";
		break;
	case AO_MAD: 
	case AO_IMAD: 
	case AO_UMAD: 
		result = "mad(" + pSrc0 + "," + pSrc1 + "," + pSrc2 + ")";
		break;
	case AO_EXP: 
		result = "exp(" + pSrc0 + ")";
		break;
	case AO_RCP: 
		result = "rcp(" + pSrc0 + ")";
		break;
	case AO_RSQ: 
		result = "rsqrt(" + pSrc0 + ")";
		break;
	case AO_SQRT: 
		result = "sqrt(" + pSrc0 + ")";
		break;
	case AO_MOV: 
	case AO_IMOV: 
	case AO_UMOV: 
		result = pSrc0;
		break;
	case AO_MOVC: 
	case AO_IMOVC: 
	case AO_UMOVC: 
	case AO_SWAPC: 
	case AO_ISWAPC: 
	case AO_USWAPC: 
		result = pSrc0 + " ? " + pSrc1 + " : " + pSrc2;
		break;
	}

	const EValueType ValueType = GetValueType( m_Op );
	if( ValueType == VT_Float )
	{
		switch( m_InstructionModifier )
		{
		case IMOD_NONE:
			break;
		case IMOD_SAT:
			result = "saturate(" + result + ")";
		}
	}

	return result;
}

bool CArithmeticPrecisionTest::CreateShaderAndSavePassThroughShader( char* szCurShader, UINT shaderSize, char* pEntrypoint, D3D_SHADER_STAGE Stage, D3D_FEATURE_LEVEL CompileFL, UINT Flags, SPassThroughShaders& PassThroughShaders, ID3D10Blob** ppOutputBlob )
{
	ATL::CComPtr<ID3D10Blob> TempShaderBlob;
	ATL::CComPtr<ID3D10Blob> pErrorBuf;

	if( ppOutputBlob == NULL )
	{
		ppOutputBlob = &TempShaderBlob;
	}

	HRESULT hr = g_TestApp.CompileShaderFromMemory( szCurShader, shaderSize, pEntrypoint, Stage, CompileFL, CompileFL, Flags, ppOutputBlob, &pErrorBuf );
	if( FAILED( hr ) )
	{
		if( pErrorBuf ) WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
		WriteToLog( "Entrypoint: %s    FeatureLevel: %s", pEntrypoint, ToString( CompileFL ).c_str() );
		WriteToLog( szCurShader );
		LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateShader );
		return false;
	}

	assert( *ppOutputBlob != NULL );
	ID3D10Blob*& pShaderBlob = *ppOutputBlob;

	switch( Stage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		// If we just created a pass-through shader, save it for later
		if( m_ShaderStage != D3D_SVT_VERTEXSHADER && PassThroughShaders.VS == NULL )
		{
			PassThroughShaders.VS = m_pVS;
		}
		break;
	case D3D_SHADER_STAGE_HULL:
		if( FAILED( hr = GetDevice()->CreateHullShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pHS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		// If we just created a pass-through shader, save it for later
		if( m_ShaderStage != D3D_SVT_HULLSHADER && PassThroughShaders.HS == NULL )
		{
			PassThroughShaders.HS = m_pHS;
		}
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		if( FAILED( hr = GetDevice()->CreateDomainShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		// If we just created a pass-through shader, save it for later
		if( m_ShaderStage != D3D_SVT_DOMAINSHADER && PassThroughShaders.DS == NULL )
		{
			PassThroughShaders.DS = m_pDS;
		}
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pGS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		// If we just created a pass-through shader, save it for later
		if( m_ShaderStage != D3D_SVT_GEOMETRYSHADER && PassThroughShaders.GS == NULL )
		{
			PassThroughShaders.GS = m_pGS;
		}
		break;
	case D3D_SHADER_STAGE_PIXEL:
		if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		// If we just created a pass-through shader, save it for later
		if( m_ShaderStage != D3D_SVT_PIXELSHADER && PassThroughShaders.PS == NULL )
		{
			PassThroughShaders.PS = m_pPS;
		}
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		if( FAILED( hr = GetDevice()->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}
		break;
	}
	return true;
}

bool CArithmeticPrecisionTest::CreateShadersFromString( char* szCurShader, UINT shaderSize, D3D_FEATURE_LEVEL MinFL, D3D_FEATURE_LEVEL MinFLPS, UINT Flags, SPassThroughShaders& PassThroughShaders )
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL CompileFL = MinFL;
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	// Create VS, IL
	if( m_ShaderStage == D3D_SVT_VERTEXSHADER || PassThroughShaders.VS == NULL || m_pIL == NULL )
	{
		m_pVS = NULL;

		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "VSMain", D3D_SHADER_STAGE_VERTEX, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
		{
			return false;
		}

		if( m_pIL == NULL )
		{
			// Note that we use the compiled VS, whether it's the passthrough copy or not, since the signature doesn't change.
			const UINT NumVertexElements = 1;
			D3D11_INPUT_ELEMENT_DESC IADesc[NumVertexElements];
			IADesc[0].InputSlot = 0;
			IADesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			IADesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
			IADesc[0].SemanticName = "pos";
			IADesc[0].SemanticIndex = 0;
			IADesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			IADesc[0].InstanceDataStepRate = 0;

			if( FAILED( hr = GetDevice()->CreateInputLayout( IADesc, NumVertexElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pIL ) ) )
			{
				LogError( __FILEW__, __LINE__, L"CreateInputLayout() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateInputLayout );
				return false;
			}
		}
	}
	else
	{
		m_pVS = PassThroughShaders.VS;
	}

	D3D_FEATURE_LEVEL CompilePSFL = MinFLPS;

	// Create PS
	if( m_ShaderStage == D3D_SVT_PIXELSHADER || PassThroughShaders.PS == NULL )
	{
		m_pPS = NULL;

		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "PSMain", D3D_SHADER_STAGE_PIXEL, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
		{
			return false;
		}
	}
	else
	{
		m_pPS = PassThroughShaders.PS;
	}

	CompileFL = max( CompileFL, D3D_FEATURE_LEVEL_10_0 );

	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		// Create GS
		if( m_ShaderStage == D3D_SVT_GEOMETRYSHADER || PassThroughShaders.GS == NULL )
		{
			m_pGS = NULL;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "GSMain", D3D_SHADER_STAGE_GEOMETRY, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
			{
				return false;
			}
		}
		else
		{
			m_pGS = PassThroughShaders.GS;
		}

		// Create CS
		if( m_ShaderStage == D3D_SVT_COMPUTESHADER )
		{
			m_pCS = NULL;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "CSMain", D3D_SHADER_STAGE_COMPUTE, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
			{
				return false;
			}
		}
	}

	CompileFL = max( CompileFL, D3D_FEATURE_LEVEL_11_0 );

	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		// Create HS
		if( m_ShaderStage == D3D_SVT_HULLSHADER || PassThroughShaders.HS == NULL )
		{
			m_pHS = NULL;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "HSMain", D3D_SHADER_STAGE_HULL, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
			{
				return false;
			}
		}
		else
		{
			m_pHS = PassThroughShaders.HS;
		}

		// Create DS
		if( m_ShaderStage == D3D_SVT_DOMAINSHADER || PassThroughShaders.DS == NULL )
		{
			m_pDS = NULL;

			ATL::CComPtr<ID3D10Blob> pShaderBlob;
			if( !CreateShaderAndSavePassThroughShader( szCurShader, shaderSize, "DSMain", D3D_SHADER_STAGE_DOMAIN, CompileFL, Flags, PassThroughShaders, &pShaderBlob ) )
			{
				return false;
			}
		}
		else
		{
			m_pDS = PassThroughShaders.DS;
		}
	}

	return true;
}

char* CArithmeticPrecisionTest::GetTypeName( CArithmeticPrecisionTest::EValueType type )
{
	switch( type )
	{
	case VT_Float:
		return "float4";
	case VT_Int:
		return "int4";
	case VT_Uint:
		return "uint4";
	}
	assert( false );
	return "";
}


template< class T >
tstring ToStringSafe( T x )
{
	return ToString( x );
}

// HLSL requires INF to be represented as below, not "INF" which the existing ToString method outputs
// The existing ToString(float/double) code displays far fewer than 15 digits, so when the HLSL compiler compiles the shader, 
// the number it uses is different than the one the test expects.   For example, if the test expects to be setting 
// 0x1234567890123 but ToString truncates this to 0x123456f, bad things happen.
template<>
tstring ToStringSafe<float>( float x )
{
	if( x == g_fINFn32 )
		return "-1.#INF";
	else if( x == g_fINFp32 )
		return "1.#INF";
	else
	{
		TCHAR ConversionBuffer[ 256 ] = _T( "" );
		StringCchPrintf ( ConversionBuffer, ARRAYSIZE( ConversionBuffer ), _T( "%.15g" ), x );
		return tstring( ConversionBuffer );
	}
}

// HLSL requires INF to be represented as below, not "INF" which the existing ToString method outputs
// The existing ToString(float/double) code displays far fewer than 15 digits, so when the HLSL compiler compiles the shader, 
// the number it uses is different than the one the test expects.   For example, if the test expects to be setting 
// 0x1234567890123 but ToString truncates this to 0x123456f, bad things happen.
template<>
tstring ToStringSafe<double>( double x )
{
	if( x == (double)g_fINFn32 )
		return "-1.#INF";
	else if( x == (double)g_fINFp32 )
		return "1.#INF";
	else
	{
		TCHAR ConversionBuffer[ 256 ] = _T( "" );
		StringCchPrintf ( ConversionBuffer, ARRAYSIZE( ConversionBuffer ), _T( "%.15g" ), x );
		return tstring( ConversionBuffer );
	}
}

template< class T >
tstring __stdcall ToString( const Vec4<T>& Vector )
{
	if( Vec4<T>::Size == 2 )
	{
		// This is the doubles case (only two doubles per register)
	    return ToStringSafe( Vector[0] ) + tstring( ", " ) + ToStringSafe( Vector[1] );
	}
	else
	{
	    return ToStringSafe( Vector[0] ) + tstring( ", " ) + ToStringSafe( Vector[1] ) + tstring( ", " ) + ToStringSafe( Vector[2] ) + tstring( ", " ) + ToStringSafe( Vector[3] );
	}
}

template< class T >
bool __stdcall FromString( Vec4<T> *pVector, const tstring &str )
{
    tstring::size_type lastPos = 0;

	for (int i=0; i < Vec4<T>::Size; i++)
    {
        tstring::size_type pos = str.find_first_of(",", lastPos);
        if (pos == tstring::npos)
            return false;
        
        // found a token, add it to the vector.
        if (! FromString( &pVector[i], str.substr(lastPos, pos - lastPos) ) )
            return false;

        // skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(",", pos);
    }
    return true;
}

template tstring ToString<double>( const Vec4<double>& Vector );
template bool FromString<double>( Vec4<double> *pVector, const tstring &str );


bool CArithmeticPrecisionTest::CreateShaders()
{
	HRESULT hr;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	const EValueType ValueType = GetValueType( m_Op );
	SPassThroughShaders& PassThroughShaders = m_PassThroughShaders[ static_cast<UINT>(ValueType) ];

	char* pDestDecl = "";
	char* pSrcDecl = "";
	char* pMinPrecDecl = MinPrecisionPrefix( m_MinPrecision );

	// SRC or DEST
	switch( m_ModifierLocation )
	{
	case MPL_SRC:
		pSrcDecl = pMinPrecDecl;
		break;
	case MPL_DEST:
		pDestDecl = pMinPrecDecl;
		break;
	case MPL_BOTH:
		pSrcDecl = pMinPrecDecl;
		pDestDecl = pMinPrecDecl;
		break;
	}

	// The compiler flags can conceivably be modified later if there are situations that require SKIP_OPTIMIZATIONS
	UINT Flags = 0;

	char* szArithTemplateBeforeOp = "";
	char szArithTemplateOp[200];
	char* szArithTemplateAfterOp = "";

	string sType = GetTypeName( ValueType );  // ex. "float4"
	string sSrcType = pSrcDecl + sType;       // ex. "min8float4"
	string sDestType = pDestDecl + sType;     // ex. "min8float4"

	// Deal with different data types
	string sSrc0;
	string sSrc1;
	string sSrc2;
	string sArithmetic;
	string sSwapC;
	switch( ValueType )
	{
	case VT_Float:
		sSrc0 = sType + "(" + ToStringSafe( m_SrcFloat[0] ) + ")";
		sSrc1 = sType + "(" + ToStringSafe( m_SrcFloat[1] ) + ")";
		sSrc2 = sType + "(" + ToStringSafe( m_SrcFloat[2] ) + ")";
		break;
	case VT_Uint:
		sSrc0 = sType + "(" + ToString( m_SrcUint[0] ) + ")";
		sSrc1 = sType + "(" + ToString( m_SrcUint[1] ) + ")";
		sSrc2 = sType + "(" + ToString( m_SrcUint[2] ) + ")";
		break;
	case VT_Int:
		sSrc0 = sType + "(" + ToString( m_SrcInt[0] ) + ")";
		sSrc1 = sType + "(" + ToString( m_SrcInt[1] ) + ")";
		sSrc2 = sType + "(" + ToString( m_SrcInt[2] ) + ")";
		break;
	}

	// Deal with different operations
	switch( m_OperandType )
	{
	case OT_TEMP:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sSrcType + " b = " + sSrc1 + ";\n" +
			sSrcType + " c = " + sSrc2 + ";\n" +
			sDestType + " d = " + OperationToString( "a", "b", "c" ) + ";\n" +
			"value = d;\n";
		sSwapC = 
			sDestType + " e = " + OperationToString( "a", "c", "b" ) + ";\n" +
			"Result[1] = e;\n";
		break;
	case OT_CONSTANT_BUFFER:
		sArithmetic = 
			sDestType + " c = " + OperationToString( "val0", "val1", "val2" ) + ";\n" +
			"value = c;\n";
		sSwapC = 
			sDestType + " d = " + OperationToString( "val0", "val2", "val1" ) + ";\n" +
			"Result[1] = d;\n";
		break;
	case OT_CONSTANT_BUFFER_AND_IMMEDIATE:
		sArithmetic = 
			sDestType + " c = " + OperationToString( "val0", pSrcDecl + sSrc1, "val2" ) + ";\n" +
			"value = c;\n";
		sSwapC = 
			sDestType + " d = " + OperationToString( "val0", "val2", pSrcDecl + sSrc1 ) + ";\n" +
			"Result[1] = d;\n";
		break;
	case OT_TEMP_AND_IMMEDIATE:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sDestType + " c = " + OperationToString( "a", pSrcDecl + sSrc1, pSrcDecl + sSrc2 ) + ";\n" +
			"value = c;\n";
		sSwapC = 
			sDestType + " d = " + OperationToString( "a", pSrcDecl + sSrc2, pSrcDecl + sSrc1 ) + ";\n" +
			"Result[1] = d;\n";
		break;
	case OT_TEMP_AND_CONSTANT_BUFFER:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sDestType + " c = " + OperationToString( "a", "val1", pSrcDecl + sSrc2 ) +  ";\n" +
			"value = c;\n";
		sSwapC = 
			sDestType + " d = " + OperationToString( "a", pSrcDecl + sSrc2, "val1" ) +  ";\n" +
			"Result[1] = d;\n";
		break;
	}

	if( m_Op == AO_SWAPC || m_Op == AO_ISWAPC || m_Op == AO_USWAPC )
	{
		// SwapC has two results, so let's add the second half of the calculation
		sArithmetic = sArithmetic + sSwapC;
	}

	// Add arithmetic to one shader
	const char* pVSCode = "";
	const char* pHSCode = "";
	const char* pDSCode = "";
	const char* pGSCode = "";
	const char* pPSCode = "";
	const char* pCSCode = "";
	switch( m_ShaderStage )
	{
	case D3D_SVT_VERTEXSHADER:
		pVSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_HULLSHADER:
		pHSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_DOMAINSHADER:
		pDSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_GEOMETRYSHADER:
		pGSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_PIXELSHADER:
		pPSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_COMPUTESHADER:
		pCSCode = sArithmetic.c_str();
		break;
	}

	char* pInterp = "";
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		// No interpolation in the pixel shader since we're only passing one value down
		pInterp = "nointerpolation";
	}

	static const char szShaderTemplate[] = 
		"#define TYPE %s \n"
		"#define SRCTYPE %s \n"
		"\n"
		"cbuffer cb  : register(b0)\n"
		"{\n"
		"    SRCTYPE val0;\n"
		"    SRCTYPE val1;\n"
		"    SRCTYPE val2;\n"
		"}\n"
		"struct VSIN\n"
		"{\n"
		"    float2 pos : POS;\n"
		"};\n"
		"\n"
		"struct VSOUT\n"
		"{\n"
		"    float4 pos : SV_Position;\n"
		"    TYPE data : texcoord;\n"
		"};\n"
		"\n"
		"struct DSOUT\n"
		"{\n"
		"    float4 pos : SV_Position;\n"
		"    TYPE data : texcoord;\n"
		"};\n"
		"\n"
		"VSOUT VSMain(VSIN v)\n"
		"{\n"
		"    VSOUT ret;\n"
		"\n"
		"    ret.pos = float4(v.pos,0,1);\n"
		"    TYPE value = TYPE(0,0,0,0);\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"struct PCOUT\n"
		"{\n"
		"    float InsideTF[2] : SV_TessFactor;\n"
		"};\n"
		"\n"
		"PCOUT PCMain(InputPatch<VSOUT, 1> ipatch, OutputPatch<VSOUT, 1> opatch, uint p : SV_PrimitiveID)\n"
		"{\n"
		"    PCOUT ret;\n"
		"    ret.InsideTF = (float[2])(float2(1,1));\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[patchconstantfunc(\"PCMain\")]\n"
		"[domain(\"isoline\")]\n"
		"[partitioning(\"fractional_odd\")]\n"
		"[outputtopology(\"point\")]\n"
		"[maxtessfactor(1.0)]\n"
		"[outputcontrolpoints(1)]\n"
		"VSOUT HSMain(InputPatch<VSOUT, 1> patch, uint i : SV_OutputControlPointID, uint p : SV_PrimitiveID)\n"
		"{\n"
		"    VSOUT ret;\n"
		"    ret.pos = patch[0].pos;\n"
		"    TYPE value = patch[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[domain(\"isoline\")]\n"
		"DSOUT DSMain(OutputPatch<VSOUT, 1> patch, PCOUT pconst )\n"
		"{\n"
		"    VSOUT ret;\n"
		"    ret.pos = patch[0].pos;\n"
		"    TYPE value = patch[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[maxvertexcount(1)]\n"
		"void GSMain(point DSOUT pts[1], inout PointStream<DSOUT> s)\n"
		"{\n"
		"    DSOUT ret;\n"
		"    ret.pos = pts[0].pos;\n"
		"    TYPE value = pts[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    s.Append(ret);\n"
		"}\n"
		"\n"
		"\n"
		"TYPE PSMain( %s DSOUT psin) : SV_Target\n"
		"{\n"
		"    TYPE value = psin.data; \n"
		"    %s \n"
		"    return value;\n"
		"}\n"
		"RWStructuredBuffer<TYPE> Result : register(u0); \n"
		"[numthreads(1, 1, 1)] \n"
		"void CSMain( uint3 threadID: SV_GroupThreadID )  \n"
		"{  \n"
		"    TYPE value = TYPE(0,0,0,0);\n"
		"    %s \n"
		"    Result[threadID.x] = value; \n"
		"}\n";

	// Form the actual shader code (by adding a bit of arithmetic to the current shader stage
	static const UINT ArithmeticBuffer = 500;
	static const UINT shaderBufferSize = sizeof(szShaderTemplate) + ArithmeticBuffer;
	char szCurShader[shaderBufferSize];
	int MadeShader = sprintf( szCurShader, szShaderTemplate, sType.c_str(), sSrcType.c_str(), pVSCode, pHSCode, pDSCode, pGSCode, pInterp, pPSCode, pCSCode );
	if( MadeShader < 0 )
	{
		LogError( __FILEW__, __LINE__, L"sprintf failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}

	UINT shaderSize = strlen(szCurShader);

	// INTs and UINTs and INF are not support on FL 9.x
	D3D_FEATURE_LEVEL CompileFL = FeatureLevel >= D3D_FEATURE_LEVEL_10_0 ? D3D_FEATURE_LEVEL_10_0 : D3D_FEATURE_LEVEL_9_1;
	
	return CreateShadersFromString( szCurShader, shaderSize, CompileFL, CompileFL, Flags, PassThroughShaders );
}


/*************************************************************
* Other test case execution methods                          *
**************************************************************/

TEST_RESULT CArithmeticPrecisionTest::SetupTestCase()
{
	HRESULT hr;

	if( !CreateShaders() )
	{
		// Error logged in CreateShaders
		return RESULT_FAIL;
	}

	if( !CreateResources() )
	{
		// Error logged in CreateResources
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CArithmeticPrecisionTest::CleanupTestCase()
{
	// Objects are automatically released when CComPtr is set to NULL
}

DXGI_FORMAT CArithmeticPrecisionTest::GetRenderTargetFormat()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	const EValueType ValueType = GetValueType( m_Op );

	// Determine RT format
	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		switch( ValueType )
		{
		case VT_Float:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VT_Uint:
			format = DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case VT_Int:
			format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		}
	}
	else
	{
		assert( ValueType == VT_Float );
	}

	return format;
}

bool CArithmeticPrecisionTest::CreateResources()
{
	HRESULT hr;
	DXGI_FORMAT format = GetRenderTargetFormat();

	if( m_pRTV )
	{
		assert( m_pRT2D != NULL );
		D3D11_TEXTURE2D_DESC tex2DDesc;
		m_pRT2D->GetDesc( &tex2DDesc );

		// If the current render target is not suitable, delete it so it will be recreated
		if( tex2DDesc.Format != format )
		{
			m_pRT2D = NULL;
			m_pRT2DStaging = NULL;
			m_pRTV = NULL;
		}
	}

	// Create RTV
	if( m_pRTV == NULL )
	{
		// Clear in case in case previous creation failed part way through
		m_pRT2D = NULL;
		m_pRT2DStaging = NULL;

		D3D11_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize			= 1;
		tex2DDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Format			= format;
		tex2DDesc.Width				= m_RTWidth;
		tex2DDesc.Height			= m_RTHeight;
		tex2DDesc.MipLevels			= 1;
		tex2DDesc.MiscFlags			= 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;

		assert( m_pRT2D == NULL );
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT2D ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed for render target.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		tex2DDesc.BindFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_STAGING;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		assert( m_pRT2DStaging == NULL );
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT2DStaging ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed for render target staging copy.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = tex2DDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		assert( m_pRTV == NULL );
		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRT2D, &rtvDesc, &m_pRTV ) ) )
		{
			// some feature levels or cards may not support RTVs in this format
			LogError( __FILEW__, __LINE__, L"CreateRenderTargetView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}

	}

	if( m_pBufUAV == NULL && m_ShaderStage == D3D_SVT_COMPUTESHADER )
	{
		m_pStructuredBuffer = NULL;
		m_pStructuredStaging = NULL;

		const UINT Num4ComponentOutputs = 2;  // Buffer contains two elements because SWAPC has two output values

		D3D11_BUFFER_DESC bufDesc = { sizeof(float) * 4 * Num4ComponentOutputs, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * 4 };
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pStructuredBuffer ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for UAV.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		bufDesc.BindFlags = 0;
		bufDesc.Usage = D3D11_USAGE_STAGING;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pStructuredStaging ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for structured stagin buffer.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescBufUAV;
		DescBufUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescBufUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescBufUAV.Buffer.FirstElement = 0;
		DescBufUAV.Buffer.NumElements = Num4ComponentOutputs;
		DescBufUAV.Buffer.Flags = 0;

        if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pStructuredBuffer, &DescBufUAV, &m_pBufUAV ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}

	// Create VB
	if( m_pVB == NULL )
	{
		CArithmeticPrecisionTest::Vertex9 VBData[4] = { {0,0} };
		D3D11_SUBRESOURCE_DATA bufInit = { VBData, sizeof(VBData), 0 };
		D3D11_BUFFER_DESC bufDesc = { sizeof(VBData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, &bufInit, &m_pVB ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for VB.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}
	}

	// Create RS
	if( m_pRS == NULL )
	{
		D3D11_RASTERIZER_DESC rastDesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE };
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateRasterizerState );
			return false;
		}
	}

	// Create CB
	if( m_pCB == NULL )
	{
		D3D11_BUFFER_DESC buffDesc = { m_CBSize, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
		if( FAILED( hr = GetDevice()->CreateBuffer( &buffDesc, NULL, &m_pCB ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for constant buffer.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}
	}

	return true;
}

// Returns a pointer to the operand data for the current test case
void* CArithmeticPrecisionTest::GetSrcData()
{
	switch( GetValueType( m_Op ) )
	{
	case VT_Float:
		return reinterpret_cast<void*>(&m_SrcFloat[0]);
	case VT_Int:
		return reinterpret_cast<void*>(&m_SrcInt[0]);
	case VT_Uint:
		return reinterpret_cast<void*>(&m_SrcUint[0]);
	}
	assert( false );
	return NULL;
}

TEST_RESULT CArithmeticPrecisionTest::ExecuteTestCase()
{
	HRESULT hr;
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	{
		assert( m_pCB != NULL );
		void* pSrc = GetSrcData();
		GetEffectiveContext()->UpdateSubresource( m_pCB, 0, NULL, pSrc, 0, 0 );

		if( m_ShaderStage == D3D_SVT_COMPUTESHADER )
		{
			assert( m_pCS != NULL );
			assert( m_pBufUAV != NULL );

			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pBufUAV, NULL );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->CSSetShader( m_pCS, NULL, 0 );

			GetEffectiveContext()->Dispatch( 1, 1, 1 );

			ID3D11UnorderedAccessView* ppNULLUAV[] = {NULL};
			ID3D11Buffer* ppNULLBuff[] = {NULL};

			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, ppNULLUAV, NULL );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, ppNULLBuff );
			GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );
		}
		else
		{
			UINT Stride = sizeof(Vertex9);
			UINT Offset = 0;

			D3D11_VIEWPORT vp = { 0,0, m_RTWidth,m_RTHeight, 0, 1 };

			assert( m_pIL != NULL );
			assert( m_pVB != NULL );
			assert( m_pVS != NULL );
			if( m_ShaderStage == D3D_SVT_HULLSHADER || m_ShaderStage == D3D_SVT_DOMAINSHADER )
			{
				assert( m_pHS != NULL );
				assert( m_pDS != NULL );
			}
			if( m_ShaderStage == D3D_SVT_GEOMETRYSHADER )
			{
				assert( m_pGS != NULL );
			}
			assert( m_pPS != NULL );
			assert( m_pRTV != NULL );
			assert( m_pRS != NULL );

			FLOAT clearColor[4] = {0};
			GetEffectiveContext()->ClearRenderTargetView( m_pRTV, clearColor );

			GetEffectiveContext()->IASetInputLayout( m_pIL );
			GetEffectiveContext()->IASetPrimitiveTopology( m_pHS == NULL ? D3D11_PRIMITIVE_TOPOLOGY_POINTLIST : D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &Stride, &Offset );
			GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
			GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->HSSetShader( m_pHS, NULL, 0 );
				GetEffectiveContext()->DSSetShader( m_pDS, NULL, 0 );
				GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pCB );
				GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pCB );
			}
			if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
			{
				GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
				GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
			}
			GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
			GetEffectiveContext()->RSSetState( m_pRS );
			GetEffectiveContext()->RSSetViewports( 1, &vp );

			GetEffectiveContext()->Draw( 1, 0 );

			// reset the context
			ID3D11RenderTargetView* ppNULLRTV[] = {NULL};
			ID3D11Buffer* ppNULLBuff[] = {NULL};

			GetEffectiveContext()->IASetInputLayout( NULL );
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, ppNULLBuff, &Stride, &Offset );
			GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, ppNULLBuff );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, ppNULLBuff );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->HSSetConstantBuffers( 0, 1, ppNULLBuff );
				GetEffectiveContext()->DSSetConstantBuffers( 0, 1, ppNULLBuff );
			}
			if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
			{
				GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->GSSetConstantBuffers( 0, 1, ppNULLBuff );
			}
			GetEffectiveContext()->RSSetState( NULL );
		}

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceContextExecute );
			return RESULT_FAIL;
		}
	}

	if( RESULT_PASS != ValidateResult( ) )
	{
		// Error logged in ValidateResult
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}


/*************************************************************
* Test Evaluation methods                                    *
**************************************************************/

template<>
CMinPrecValue<float> CArithmeticPrecisionTest::PerformOperation( ARITH_OP op, CMinPrecValue<float>& a, CMinPrecValue<float>& b, CMinPrecValue<float>& c, UINT component )
{
	switch( op )
	{
	case AO_ADD: return _add(a,b);
	case AO_MAX: return _max(a,b);
	case AO_MIN: return _min(a,b);
	case AO_EXP: return _exp(a);
	case AO_RCP: return _rcp(a);
	case AO_RSQ: return _rsq(a);
	case AO_SQRT: return _sqrt(a);
	case AO_MAD: return _mad(a,b,c);
	case AO_MOV: return a;
	case AO_SWAPC: // SWAPC is simply two MOVCs
	case AO_MOVC: return _movc(a,b,c);
	}
	assert( false );
	return CMinPrecValue<float>(0);
}

template< class T >
CMinPrecValue<T> CArithmeticPrecisionTest::PerformOperation( ARITH_OP op, CMinPrecValue<T>& a, CMinPrecValue<T>& b, CMinPrecValue<T>& c, UINT component )
{
	switch( op )
	{
	case AO_UADD:
	case AO_IADD: return _add(a,b);
	case AO_UMAX:
	case AO_IMAX: return _max(a,b);
	case AO_UMIN:
	case AO_IMIN: return _min(a,b);
	case AO_UMAD:
	case AO_IMAD: return _mad(a,b,c);
	case AO_IMOV:
	case AO_UMOV: return a;
	case AO_ISWAPC: // SWAPC is simply two MOVCs
	case AO_USWAPC: // SWAPC is simply two MOVCs
	case AO_IMOVC:
	case AO_UMOVC: return _movc(a,b,c);
	}
	assert( false );
	return CMinPrecValue<T>(0);
}

template< class T >
void CArithmeticPrecisionTest::ApplySourceModifier( SRC_MOD_TYPE ModType, CMinPrecValue<T>& a, CMinPrecValue<T>& b, CMinPrecValue<T>& c )
{
	switch( ModType )
	{
	case SMOD_NONE:
		break;
	case SMOD_NEG:
		a._neg();
		b._neg();
		c._neg();
		break;
	case SMOD_ABS:
		a._abs();
		c._abs();
		b._abs();
		break;
	case SMOD_ABSNEG:
		a._absneg();
		b._absneg();
		c._absneg();
		break;
	}
}

TEST_RESULT CArithmeticPrecisionTest::ValidateResult()
{
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	HRESULT hr = S_OK;
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( m_ShaderStage != D3D_SVT_COMPUTESHADER )
	{
		GetImmediateContext()->CopyResource( m_pRT2DStaging, m_pRT2D );
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStructuredStaging, m_pStructuredBuffer );
	}

	// Map the output resource
	CMapHelper mapHelper( GetImmediateContext() );
	if( m_ShaderStage == D3D_SVT_COMPUTESHADER )
	{
		hr = mapHelper.Map( m_pStructuredStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	}
	else
	{
		hr = mapHelper.Map( m_pRT2DStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	}
	if( FAILED( hr ) )
	{
		LogError( __FILEW__, __LINE__, L"m_pRT2DStaging Map() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
		return RESULT_FAIL;
	}

	D3D11_TEXTURE2D_DESC tex2DDesc;
	m_pRT2DStaging->GetDesc( &tex2DDesc );

	const EValueType ValueType = GetValueType( m_Op );

	EPrecisionLevel MinPrecisionCheck = CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision );
	// In 9.x, only 16 bit floats are outputted, so make sure we don't expect more precision than 16 bits
	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 && ValueType == VT_Float && MinPrecisionCheck == EPL_32 )
	{
		assert( tex2DDesc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT );
		MinPrecisionCheck = EPL_16;
	}

	bool bModSrc = false;
	bool bModDest = false;

	switch( m_ModifierLocation )
	{
	case MPL_SRC:
		bModSrc = true;
		break;
	case MPL_DEST:
		bModDest = true;
		break;
	case MPL_BOTH:
		bModSrc = true;
		bModDest = true;
		break;
	}

	// We do not loop through data below.
	assert( m_RTWidth == 1 && m_RTHeight == 1 );
	const UINT NumComponents = 4;

	const UINT MaxNumResults = 2;
	UINT NumResults = ( m_Op == AO_SWAPC || m_Op == AO_ISWAPC || m_Op == AO_USWAPC ) ? 2 : 1;
	assert( NumResults <= MaxNumResults );

	if( ValueType == VT_Float )
	{
		FLOAT_COLOR fPixel[MaxNumResults];
		bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
						   tex2DDesc.Format, 
						   fPixel, 
						   NumResults, 
						   1, 
						   1, 
						   mapTex2D.pData );

		if ( !bConverted )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
			return RESULT_FAIL;
		}

		for( UINT comp = 0; comp < NumComponents; comp++ )
		{
			// Generate the expected value
			CMinPrecValue<float> a( m_SrcFloat[0][comp] );
			CMinPrecValue<float> b( m_SrcFloat[1][comp] );
			CMinPrecValue<float> c( m_SrcFloat[2][comp] );
			if( bModSrc )
			{
				a.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				b.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				c.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
			}
			ApplySourceModifier( m_SrcModifier, a, b, c);
			CMinPrecValue<float> expected[MaxNumResults];

			expected[0] = PerformOperation<float>( m_Op, a, b, c, comp );
			if( NumResults == 2 )
			{
				// Notice that "c" and "b" are swapped here compared to the previous PerformOperation.  SWAPC is implemented here as two MOVCs
				assert( m_Op == AO_SWAPC );
				expected[1] = PerformOperation<float>( m_Op, a, c, b, comp );
			}

			for( UINT i = 0; i < NumResults; i++ )
			{
				// Select the component to read
				float pixel = fPixel[i].comp[comp];

				// In HLSL, the operation precision is controlled by the inputs.  If they are min-precision, the whole
				// operation can be, regardless of the type of the result variable.  
				if( bModDest || bModSrc )
				{
					expected[i].EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				}

				if( m_InstructionModifier == IMOD_SAT )
				{
					expected[i].Saturate();
				}

				float ULPMultiplier = 1.0f;
				switch( m_Op )
				{
				case AO_MAD:   // Add a little more wiggle room for MAD, since it has two operations
				case AO_EXP:
					ULPMultiplier = 2.0f;
					break;
				}

				if( !expected[i].Matches( pixel, MinPrecisionCheck, FeatureLevel, false, ULPMultiplier ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<float>( i, comp, pixel, expected[i] );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
#if 0
				// Check for exact precision when running REF to ensure the calculations were correct
				if( g_TestApp.m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE && !expected[i].Matches( pixel, MinPrecisionCheck, FeatureLevel, false, 1.0f ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<float>( i, comp, pixel, expected[i] );
					LogError( __FILEW__, __LINE__, L"REF failed exact value match.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
#endif
			}
		}
	}
	else if( ValueType == VT_Int )
	{
		int* pPixel = reinterpret_cast<int*>( mapTex2D.pData );
		for( UINT comp = 0; comp < NumComponents; comp++ )
		{
			// Generate the expected value
			CMinPrecValue<int> a( m_SrcInt[0][comp] );
			CMinPrecValue<int> b( m_SrcInt[1][comp] );
			CMinPrecValue<int> c( m_SrcInt[2][comp] );
			if( bModSrc )
			{
				a.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				b.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				c.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
			}
			ApplySourceModifier( m_SrcModifier, a, b, c);
			CMinPrecValue<int> expected[MaxNumResults];

			expected[0] = PerformOperation<int>( m_Op, a, b, c, comp );
			if( NumResults == 2 )
			{
				// Notice that "c" and "b" are swapped here compared to the previous PerformOperation.  SWAPC is implemented here as two MOVCs
				assert( m_Op == AO_ISWAPC );
				expected[1] = PerformOperation<int>( m_Op, a, c, b, comp );
			}

			for( UINT i = 0; i < NumResults; i++ )
			{
				// Select the component to read
				int pixel = pPixel[ 4 * i + comp ];

				// In HLSL, the operation precision is controlled by the inputs.  If they are min-precision, the whole
				// operation can be, regardless of the type of the result variable.  
				if( bModDest || bModSrc )
				{
					expected[i].EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				}

				// Check for exact precision
				if( !expected[i].Matches( pixel, MinPrecisionCheck, FeatureLevel, true ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<INT>( i, comp, pixel, expected[i] );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
	}
	else
	{
		UINT* pPixel = reinterpret_cast<UINT*>( mapTex2D.pData );
		for( UINT comp = 0; comp < NumComponents; comp++ )
		{
			// Generate the expected value
			CMinPrecValue<UINT> a( m_SrcUint[0][comp] );
			CMinPrecValue<UINT> b( m_SrcUint[1][comp] );
			CMinPrecValue<UINT> c( m_SrcUint[2][comp] );
			if( bModSrc )
			{
				a.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				b.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				c.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
			}
			ApplySourceModifier( m_SrcModifier, a, b, c);
			CMinPrecValue<UINT> expected[MaxNumResults];
			
			expected[0] = PerformOperation<UINT>( m_Op, a, b, c, comp );
			if( NumResults == 2 )
			{
				// Notice that "c" and "b" are swapped here compared to the previous PerformOperation.  SWAPC is implemented here as two MOVCs
				assert( m_Op == AO_USWAPC );
				expected[1] = PerformOperation<UINT>( m_Op, a, c, b, comp );
			}

			for( UINT i = 0; i < NumResults; i++ )
			{
				// Select the component to read
				UINT pixel = pPixel[ 4 * i + comp ];

				// In HLSL, the operation precision is controlled by the inputs.  If they are min-precision, the whole
				// operation can be, regardless of the type of the result variable.  
				if( bModDest || bModSrc )
				{
					expected[i].EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );
				}

				// Check for exact precision
				if( !expected[i].Matches( pixel, MinPrecisionCheck, FeatureLevel, true ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<UINT>( i, comp, pixel, expected[i] );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
	}

	return RESULT_PASS;
}
