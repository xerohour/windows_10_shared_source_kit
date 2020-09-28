#include "Mov.h"

//---------------------------------------------------------------

testfactor::RFactor CMovUintTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	// Min Precision
	testfactor::RFactor rMinPrecisionInt;
	InitializeMinPrecisionFactor( &rMinPrecisionInt );

	// Values
	CUserValueSet<UINT>* pValues = new CUserValueSet<UINT>( );
	pValues->AddValue( 0 );
	pValues->AddValue( 1 );
	pValues->AddValue( 2 );
	pValues->AddValue( USHORT_MAX );
	pValues->AddValue( USHORT_MAX - 1 );
	pValues->AddValue( USHORT_MAX + 1 );

	// MOVC Conditional
	CUserValueSet<UINT>* pConditional = new CUserValueSet<UINT>( );
	pConditional->AddValue( 0 );
	pConditional->AddValue( 1 );
	pConditional->AddValue( 25 );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesUint1 = new CUserValueSet<ARITH_OP>( );
	pValuesUint1->AddValue( AO_UMOV );
	CUserValueSet<ARITH_OP>* pValuesUint3 = new CUserValueSet<ARITH_OP>( );
	pValuesUint3->AddValue( AO_UMOVC );
	testfactor::RFactor rUintOp1Factor = AddParameterValueSet( pInstructionParam, pValuesUint1 );
	testfactor::RFactor rUintOp3Factor = AddParameterValueSet( pInstructionParam, pValuesUint3 );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_UMOV );

	// Create value sets for the 2-operand and 3-operand operations
	testfactor::RFactor rValue1Factor = InitializeValueFactor<UINT,1>( this, pValues, m_SrcUint, NULL, 4, "" );
	testfactor::RFactor rValue3Factor = InitializeValueFactor<UINT,3>( this, pConditional, m_SrcUint, pValues, 4, "" );

	testfactor::RFactor rOpAndValue = rUintOp1Factor * rValue1Factor + rUintOp3Factor * rValue3Factor;
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();

	return rMinPrecisionInt * rOpAndValue % ( rSrcMod * rInstMod );
}

testfactor::RFactor CMovIntTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	// Min Precision
	testfactor::RFactor rMinPrecisionInt;
	InitializeMinPrecisionFactor( &rMinPrecisionInt );

	// Values
	CUserValueSet<int>* pValues = new CUserValueSet<int>( );
	pValues->AddValue( 0 );
	pValues->AddValue( 1 );
	pValues->AddValue( -1 );
	pValues->AddValue( 79 );
	pValues->AddValue( SHORT_MAX );
	pValues->AddValue( SHORT_MIN );
	pValues->AddValue( SHORT_MAX + 1 );

	// MOVC Conditional
	CUserValueSet<int>* pConditional = new CUserValueSet<int>( );
	pConditional->AddValue( 0 );
	pConditional->AddValue( 1 );
	pConditional->AddValue( -1 );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesInt1 = new CUserValueSet<ARITH_OP>( );
	pValuesInt1->AddValue( AO_IMOV );
	CUserValueSet<ARITH_OP>* pValuesInt3 = new CUserValueSet<ARITH_OP>( );
	pValuesInt3->AddValue( AO_IMOVC );
	testfactor::RFactor rIntOp1Factor = AddParameterValueSet( pInstructionParam, pValuesInt1 );
	testfactor::RFactor rIntOp3Factor = AddParameterValueSet( pInstructionParam, pValuesInt3 );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_IMOV );

	// Create value sets for the 1-operand and 3-operand operations
	testfactor::RFactor rValue1Factor = InitializeValueFactor<int,1>( this, pValues, m_SrcInt, NULL, 4, "" );
	testfactor::RFactor rValue3Factor = InitializeValueFactor<int,3>( this, pConditional, m_SrcInt, pValues, 4, "" );

	testfactor::RFactor rOpAndValue = rIntOp1Factor * rValue1Factor + rIntOp3Factor * rValue3Factor;
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();

	return rMinPrecisionInt * rOpAndValue % ( rSrcMod * rInstMod );
}

testfactor::RFactor CMovFloatTest::InitializeOpAndValueAndMinPrecisionAndModFactor()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetFramework()->m_D3DOptions.FeatureLevel;

	// Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "MinPrecision" ), &m_MinPrecision );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Instructions
	CTestCaseParameter<ARITH_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_Op );
	CUserValueSet<ARITH_OP>* pValuesFloat1 = new CUserValueSet<ARITH_OP>( );
	pValuesFloat1->AddValue( AO_MOV ); // 1
	CUserValueSet<ARITH_OP>* pValuesFloat3 = new CUserValueSet<ARITH_OP>( );
	pValuesFloat3->AddValue( AO_MOVC ); // 3
	testfactor::RFactor rFloatOp1Factor = AddParameterValueSet( pInstructionParam, pValuesFloat1 );
	testfactor::RFactor rFloatOp3Factor = AddParameterValueSet( pInstructionParam, pValuesFloat3 );
	SetParameterDefaultValue<ARITH_OP>( pInstructionParam, AO_MOV );

	// Source Modifier
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();

	// Instruction Modifier
	testfactor::RFactor rInstMod = InitializeInstructionModifierFactor();


	// Min Precision and Values
	testfactor::RFactor rAllFactors;
	for( EPrecisionLevel minprec = EPL_32; minprec >= EPL_10; ((UINT&)minprec) >>= 1 )
	{
		D3D_MIN_PRECISION level = CMinPrecValue<float>::ConvertToPrecisionLevel( minprec );

		testfactor::RFactor rMinPrecision = AddParameterValue( pMinPrecisionParam, level );

		// Values (use appropriate values for min precision)
		CUserValueSet<float>* pFloatsNormal = new CUserValueSet<float>( );
		pFloatsNormal->AddValue( -1.0f );
		pFloatsNormal->AddValue( 0.0f );
		pFloatsNormal->AddValue( 1.0f / 255.0f );
		pFloatsNormal->AddValue( 1.0f );
		if( minprec == EPL_10 )
		{
			pFloatsNormal->AddValue( 127.0f / 255.0f );
		}
		if( minprec >= EPL_10 )
		{
			pFloatsNormal->AddValue( -1.25f );
			pFloatsNormal->AddValue( -2.0f );
		}
		if( minprec >= EPL_16 )
		{
			pFloatsNormal->AddValue( g_fDenormFloat16 );
			pFloatsNormal->AddValue( 12345678 );	// out of range of float16 (approx max 2 ^ 16 = 65000)
		}
		else if( minprec == EPL_10 )
		{
			pFloatsNormal->AddValue( 1.0f + 255.0f / 256.0f );
			pFloatsNormal->AddValue( 3.0f );
		}

		// INF (only available in 10+)
		CUserValueSet<float>* pFloatsInteresting = new CUserValueSet<float>( );
		if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
		{
			pFloatsInteresting->AddValue( g_fINFn32 );
		}

		// MOVC Conditional
		CUserValueSet<float>* pConditional = new CUserValueSet<float>( );
		pConditional->AddValue( 0.0f );
		pConditional->AddValue( 1.0f );
		pConditional->AddValue( g_fDenormFloat16 );
		pConditional->AddValue( -0.5f );

		// Create aggregate value sets
		CUserValueSet<float> Floats = *pFloatsNormal | *pFloatsInteresting;
		CUserValueSet<float>* pFloats = &Floats;

		// Create cross factor value sets for the 1-operand, 2-operand and 3-operand operations
		testfactor::RFactor rValue1Factor = InitializeValueFactor<float,1>( this, pFloats, m_SrcFloat, NULL, 4, "" );
		testfactor::RFactor rValue3Factor = InitializeValueFactor<float,3>( this, pConditional, m_SrcFloat, pFloats, 4, "" );

		testfactor::RFactor rAllOperands;
		{
			rAllOperands = ( rFloatOp1Factor * rValue1Factor + rFloatOp3Factor * rValue3Factor ) % ( rSrcMod * rInstMod );
		}

		testfactor::RFactor rCurFactor = rMinPrecision * rAllOperands;

		rAllFactors = rAllFactors + rCurFactor;
	}

	return rAllFactors;
}

