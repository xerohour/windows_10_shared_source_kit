#include "Tessellation.h"
#include <sstream>

BEGIN_NAMED_VALUES( CHullShaderPhasesTest::TESS_FACTOR_ASSIGNMENT )
	NAMED_VALUE( "OneJoin",         CHullShaderPhasesTest::TESS_FACTOR_IN_ONE_JOIN_INSTANCED )
	NAMED_VALUE( "SeparateJoin",    CHullShaderPhasesTest::TESS_FACTOR_IN_SEPARATE_JOIN )
	NAMED_VALUE( "OneFork",         CHullShaderPhasesTest::TESS_FACTOR_IN_ONE_FORK_INSTANCED )
	NAMED_VALUE( "SeparateFork",    CHullShaderPhasesTest::TESS_FACTOR_IN_SEPARATE_FORK )
	NAMED_VALUE( "EdgeFork",        CHullShaderPhasesTest::TESS_FACTOR_EDGE_IN_FORK )
	NAMED_VALUE( "EdgeJoin",        CHullShaderPhasesTest::TESS_FACTOR_EDGE_IN_JOIN )
	NAMED_VALUE( "FPSpecial",       CHullShaderPhasesTest::TESS_FACTOR_SPECIAL )
END_NAMED_VALUES( CHullShaderPhasesTest::TESS_FACTOR_ASSIGNMENT )

static const CHullShaderPhasesTest::TESS_FACTOR_ASSIGNMENT g_TessFactorAssignmentTable[] = 
{
	CHullShaderPhasesTest::TESS_FACTOR_IN_ONE_JOIN_INSTANCED,
	CHullShaderPhasesTest::TESS_FACTOR_IN_SEPARATE_JOIN,
	CHullShaderPhasesTest::TESS_FACTOR_IN_ONE_FORK_INSTANCED,
	CHullShaderPhasesTest::TESS_FACTOR_IN_SEPARATE_FORK,
	CHullShaderPhasesTest::TESS_FACTOR_EDGE_IN_FORK,
	CHullShaderPhasesTest::TESS_FACTOR_EDGE_IN_JOIN,
	CHullShaderPhasesTest::TESS_FACTOR_SPECIAL,
};

void CHullShaderPhasesTest::InitTestParameters()
{
	AddParameter<tstring>                   ( "Domain",                 &m_DomainName );
	AddParameter<tstring>                   ( "Topology",               &m_TopologyName );
	AddParameter<TESS_FACTOR_ASSIGNMENT>    ( "TessFactorPhase",        &m_TessFactorPhase );
	AddParameter<bool>                      ( "ExtraPCData",            &m_ExtraPatchConstantData );
	AddParameter<UINT>                      ( "NumInputControlPoints",  &m_NumInputControlPoints );
	AddParameter<UINT>                      ( "NumOutputControlPoints", &m_NumOutputControlPoints );

	testfactor::RFactor rDomain_Quad = AddParameterValue<tstring>( "Domain", _T("quad"));
	testfactor::RFactor rDomain_Tri = AddParameterValue<tstring>( "Domain", _T("tri"));
	testfactor::RFactor rDomain_Isoline = AddParameterValue<tstring>( "Domain", _T("isoline"));
	
	testfactor::RFactor rTopology_Point = AddParameterValue<tstring>( "Topology", _T("point"));
	testfactor::RFactor rTopology_Line = AddParameterValue<tstring>( "Topology", _T("line"));
	testfactor::RFactor rTopology_Triangle_CW = AddParameterValue<tstring>( "Topology", _T("triangle_cw"));
	testfactor::RFactor rTopology_Triangle_CCW = AddParameterValue<tstring>( "Topology", _T("triangle_ccw"));

	testfactor::RFactor rDomainAndTopology = (rDomain_Quad * (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)) +
											 (rDomain_Tri * (rTopology_Point + rTopology_Triangle_CW + rTopology_Triangle_CCW)) +
											 (rDomain_Isoline * (rTopology_Point + rTopology_Line));
											 

	testfactor::RFactor rTessFactorPhase = AddParameterValueSet<TESS_FACTOR_ASSIGNMENT>( "TessFactorPhase", 
		new CTableValueSet<TESS_FACTOR_ASSIGNMENT>( g_TessFactorAssignmentTable, sizeof(g_TessFactorAssignmentTable[0]), ARRAY_SIZE(g_TessFactorAssignmentTable) ) );

	testfactor::RFactor rExtraPCData = AddParameterValueSet<bool>( "ExtraPCData", new CBoolValueSet() );

	testfactor::RFactor rInputControlPoints = AddParameterValueSet<UINT>( "NumInputControlPoints",  new CRangeValueSet<UINT>( 1, 32, 1 ) );

	testfactor::RFactor rOutputControlPoints = AddParameterValueSet<UINT>( "NumOutputControlPoints",  new CRangeValueSet<UINT>( 1, 32, 1 ) );

	SetRootTestFactor
	( 
		(rInputControlPoints * rOutputControlPoints) % 
		(rDomainAndTopology * rTessFactorPhase * rExtraPCData)
	);
}

// LiteralFloat4(1,2,3,4) prints the string "l(1.0, 2.0, 3.0, 4.0)"
struct LiteralFloat4
{
	LiteralFloat4(UINT a, UINT b, UINT c, UINT d)
	{
		_Vals[0] = (float)a;
		_Vals[1] = (float)b;
		_Vals[2] = (float)c;
		_Vals[3] = (float)d;
	}

	float _Vals[4];
};

static std::ostream& operator<<(std::ostream& stream, LiteralFloat4& rhs)
{
	stream << std::fixed << "l(" << rhs._Vals[0] << ", " << rhs._Vals[1] << ", " << rhs._Vals[2] << ", " << rhs._Vals[3] << ")";
	return stream;
}

// LiteralUINT4(1,2,3,4) prints the string "l(1, 2, 3, 4)"
struct LiteralUINT4
{
	LiteralUINT4(UINT a, UINT b, UINT c, UINT d)
	{
		_Vals[0] = a;
		_Vals[1] = b;
		_Vals[2] = c;
		_Vals[3] = d;
	}

	UINT _Vals[4];
};

static std::ostream& operator<<(std::ostream& stream, LiteralUINT4& rhs)
{
	stream << "l(" << rhs._Vals[0] << ", " << rhs._Vals[1] << ", " << rhs._Vals[2] << ", " << rhs._Vals[3] << ")";
	return stream;
}

static UINT FloatAsUINT(float f)
{
	return *((UINT*)(&f));
}

// Declares outputs o4.xyzw-o31.xyzw
static void DeclareExtraOutputsAndInputControlPoints( std::ostream& ss, UINT numInputControlPoints, UINT numOutputControlPoints )
{
	for( UINT i = 4; i < 32; i++ )
	{
		ss << "dcl_output o" << i << ".xyzw\n";
	}

	ss << "dcl_input vcp[" << numInputControlPoints << "][0].x\n";
	ss << "dcl_input vocp[" << numOutputControlPoints << "][0].x\n";
}

// Sets outputs o4.xyzw-o31.xyzw to known values
static void InitializeExtraOutputs( std::ostream& ss, UINT numInputControlPoints, UINT numOutputControlPoints, UINT seed )
{
	// Write input control points values to output registers
	UINT regIdx = 4;

	for( UINT i = 0; i < numInputControlPoints; i++ )
	{
		if( regIdx >= 32 )
			break;

		ss << "mov o" << regIdx << ".xyzw, vcp[" << i << "][0].xxxx\n";

		regIdx++;
	}

	// Write output control point values to output registers
	for( UINT i = 0; i < numOutputControlPoints; i++ )
	{
		if( regIdx >= 32 )
			break;

		ss << "mov o" << regIdx << ".xyzw, vocp[" << i << "][0].xxxx\n";

		regIdx++;
	}

	// Write literals to output registers
	for( UINT i = regIdx; i < 32; i++ )
	{
		ss << "mov o" << i << ".xyzw, " << LiteralFloat4(i+seed, (i+seed)*2, (i+seed)*3, (i+seed)*4) << "\n";
	}
}

// Declares for output, all registers components that will contain tesselation factors
// factorSIV contains a list of the tesselation factors names (like "finalQuadVeq0EdgeTessFactor")
// outputRegs contains the list of output register components that will contain the tesselation factors (like "o0.y")
static void DeclareAllTesselationFactors( std::ostream& ss, const std::vector<std::string>& factorSIV, const char** outputRegs )
{
	for( UINT i = 0; i < factorSIV.size(); i++ )
	{
		ss << "dcl_output_siv " << outputRegs[i] << ", " << factorSIV[i] << "\n";
	}
}

// Outputs 1 phase for each tesselation factor.  Each phase declares 1 tesselation factor output register component and writes a value to it
static void DeclareAndSetAllTesselationFactors( std::ostream& ss, const std::vector<std::string>& factorSIV, const char** outputRegs, const char* phase, UINT seed )
{
		// 1 join for each edge tesselation factor
		for( UINT i = 0; i < factorSIV.size(); i++ )
		{
			ss << phase << "\n";
			ss << "dcl_output_siv " << outputRegs[i] << ", " << factorSIV[i] << "\n";
			ss << "mov " << outputRegs[i] << ", " << LiteralFloat4((i+seed)*1, (i+seed)*3, (i+seed)*2, (i+seed)*4) << "\n";
			ss << "ret\n";
		}
}

// Declares all of the output components of registers o0-o4 that are not tesselation factors
static void DeclareRemainingOutputs( std::ostream& ss, const char** outputRegMasks, UINT size )
{
	for( UINT i = 0; i < size; i++ )
	{
		ss << "dcl_output o" << i << outputRegMasks[i] << "\n";
	}
}

// Initialies all of the output components of registers o0-o4 that are not tesselation factors to specific values
static void SetRemainingOutputs( std::ostream& ss, const char** outputRegMasks, UINT size, UINT seed )
{
	for( UINT i = 0; i < size; i++ )
	{
		ss << "mov o" << i << outputRegMasks[i] << ", " << LiteralFloat4((i+seed)*6, (i+seed)*8, (i+seed)*10, (i+seed)*12) << "\n";
	}
}


TEST_RESULT CHullShaderPhasesTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	// Vertex shader (the same one is used for all test cases)
	{
		m_VertexShader.shaderCode = 
			"vs_5_0\n"
			"dcl_input_sgv v0.x, vertex_id\n"
			"dcl_output o0.x\n" // output vertex id
			"mov o0.x, v0.x\n"
			"ret\n";
		m_VertexShader.location = SHADER_SOURCE_MEMORY_ASM;
	}

	std::string domain = GetTestCaseParameter( "Domain" )->GetValueAsString();
	std::string topology = GetTestCaseParameter( "Topology" )->GetValueAsString();

	
	// Hull shader output register devoted to edge tesselation factors
	const char* edgeTessFactorOutputRegs[4] = { NULL, NULL, NULL, NULL };

	// Hull shader output registers devoted to inside tesseleation factors
	const char* insideTessFactorOutputRegs[2] = { NULL, NULL };

	// Hull shader output register in the range o0-o4 that are not used for tesselation factors
	const char* remainingOutputRegMasks[4] = { NULL, NULL, NULL, NULL };

	// signature names generated by the assembler
	const char* outputNames[] = 
	{
		"0_",  "1_",  "2_",  "3_",  "4_",  "5_",  "6_",  "7_",  "8_",  "9_", 
		"10_", "11_", "12_", "13_", "14_", "15_", "16_", "17_", "18_", "19_", 
		"20_", "21_", "22_", "23_", "24_", "25_", "26_", "27_", "28_", "29_", 
		"30_", "31_"
	};

	bool declInsideTessFactorFirst = false;

	std::vector<std::string> edgeTessFactors;
	std::vector<std::string> insideTessFactors;

	if( domain == "quad" )
	{
		edgeTessFactors.push_back( "finalQuadUeq0EdgeTessFactor" );
		edgeTessFactors.push_back( "finalQuadVeq0EdgeTessFactor" );
		edgeTessFactors.push_back( "finalQuadUeq1EdgeTessFactor" );
		edgeTessFactors.push_back( "finalQuadVeq1EdgeTessFactor" );

		insideTessFactors.push_back( "finalQuadUInsideTessFactor" );
		insideTessFactors.push_back( "finalQuadVInsideTessFactor" );

		remainingOutputRegMasks[0] = ".zw";
		remainingOutputRegMasks[1] = ".zw";
		remainingOutputRegMasks[2] = ".yzw";
		remainingOutputRegMasks[3] = ".yzw";

		edgeTessFactorOutputRegs[0] = "o0.x";
		edgeTessFactorOutputRegs[1] = "o1.x";
		edgeTessFactorOutputRegs[2] = "o2.x";
		edgeTessFactorOutputRegs[3] = "o3.x";

		insideTessFactorOutputRegs[0] = "o0.y";
		insideTessFactorOutputRegs[1] = "o1.y";
	}
	else if( domain == "tri" )
	{
		edgeTessFactors.push_back( "finalTriUeq0EdgeTessFactor" );
		edgeTessFactors.push_back( "finalTriVeq0EdgeTessFactor" );
		edgeTessFactors.push_back( "finalTriWeq0EdgeTessFactor" );

		insideTessFactors.push_back( "finalTriInsideTessFactor" );

		remainingOutputRegMasks[0] = ".zw";
		remainingOutputRegMasks[1] = ".yzw";
		remainingOutputRegMasks[2] = ".yzw";
		remainingOutputRegMasks[3] = ".xyzw";

		edgeTessFactorOutputRegs[0] = "o0.x";
		edgeTessFactorOutputRegs[1] = "o1.x";
		edgeTessFactorOutputRegs[2] = "o2.x";

		insideTessFactorOutputRegs[0] = "o0.y";
	}
	else if( domain == "isoline" )
	{
		// Neither of these is really an "edge" or "inside" tesselation factor
		// but they must be in the same component of consequetive registers (o0.x, o1.x)
		// so make them both "edge" tesselation factors
		edgeTessFactors.push_back( "finalLineDensityTessFactor" ); // must come first
		edgeTessFactors.push_back( "finalLineDetailTessFactor" );

		remainingOutputRegMasks[0] = ".yzw";
		remainingOutputRegMasks[1] = ".yzw";
		remainingOutputRegMasks[2] = ".xyzw";
		remainingOutputRegMasks[3] = ".xyzw";

		edgeTessFactorOutputRegs[0] = "o0.x";
		edgeTessFactorOutputRegs[1] = "o1.x";
	}
	else
	{
		WriteToLog( "Unknown domain name" );
		tRes = RESULT_FAIL;
		goto setupDone;
	}

	// Hull shader (built up based on test case parameters)
	{
		std::ostringstream ss;

		// Standard header
		ss <<   "hs_5_0\n"
				"hs_decls\n"
				"dcl_input_control_point_count " << m_NumInputControlPoints << "\n"
				"dcl_output_control_point_count " << m_NumOutputControlPoints << "\n"
				"dcl_tessellator_domain domain_" << domain << "\n"
				"dcl_tessellator_partitioning partitioning_fractional_odd\n"
				"dcl_tessellator_output_primitive output_" << topology << "\n";

		// Control point phase
		ss <<   "hs_control_point_phase\n"
				"dcl_input vOutputControlPointID\n"
				"dcl_input v[" << m_NumInputControlPoints << "][0].x\n"
				"dcl_output o0.x\n"
				"mov o0.x, v[0][0].x\n"
				"ret\n";

		switch( m_TessFactorPhase )
		{
		case TESS_FACTOR_IN_ONE_JOIN_INSTANCED:
			if( m_ExtraPatchConstantData )
			{
				ss <<   "hs_fork_phase\n";

				// declare outputs o4-o31
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints ); 
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 1 );

				// Set o4 and o5 to specific values.  These will be read by the join phase
				ss <<   "mov o4.xyzw, l(1.0, 1.5, 2.0, 3.5)\n"
						"mov o5.xy,   l(0.5, 0.75, 4.5, 5.5)\n" 
						"ret\n";
			}
			else
			{
				// no fork phase, join phase generates all tesselation factors
			}

			ss <<   "hs_join_phase\n"
					"dcl_temps 3\n";

			if( m_ExtraPatchConstantData )
			{
				ss <<   "dcl_input vpc[4].xyzw\n" // output of fork phase
						"dcl_input vpc[5].xy\n";  // output of fork phase
			}

			// declare edge tesselation factors
			DeclareAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs );

			// declare inside tesselation factors
			DeclareAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs );

			// Declare remaining outputs
			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			ss <<   "dcl_hs_join_phase_instance_count 6\n"
					"dcl_input vJoinInstanceID\n"
					"mov r0.x, vJoinInstanceID\n";

			// m_ExtraPatchConstantData means that the hull phase generated vpc[4] and vpc[5]
			if( m_ExtraPatchConstantData )
			{
				ss <<   "mov r1, vpc[4]\n"
						"mov r2, vpc[5].xyxy\n";
			}
			else
			{
				ss <<   "mov r1, l(32.5, 33.5, 34.5, 35.5)\n"
						"mov r2, l(42.5, 43.5, 44.5, 45.5)\n";
			}

			ss <<   "switch r0.x\n"
					"   case 0\n"
					"       mov o0.x, r1.x\n"
					"       break\n"
					"   case 1\n"
					"       mov o1.x, r1.y\n"
					"       break\n"
					"   case 2\n"
					"       mov o2.x, r1.z\n"
					"       break\n"
					"   case 3\n"
					"       mov o3.x, r1.w\n"
					"       break\n"
					"   case 4\n"
					"       mov o0.y, r2.x\n"
					"       break\n"
					"   case 5\n"
					"       mov o1.y, r2.y\n"
					"       break\n"
					"endswitch\n";

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 1 );

			ss <<  "ret\n";
			break;

		case TESS_FACTOR_IN_SEPARATE_JOIN:
			if( m_ExtraPatchConstantData )
			{
				ss << "hs_fork_phase\n";

				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints ); 
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 2 );

				// Set o4 and o5 to specific values.  These will be read by the join phase
				ss <<   "mov o4.xyzw, l(6.0, 4.5, 3.25, 1.0)\n"
						"mov o5.xy,   l(3.5, 2.75, 4.5, 5.5)\n" 
						"ret\n";
			}

			// 1 join for each edge tesselation factor
			DeclareAndSetAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs, "hs_join_phase", 1 );

			// 1 join phase for each inside tesselation factor
			DeclareAndSetAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs, "hs_join_phase", 2 );

			// 1 join phase for remaining outputs
			ss << "hs_join_phase\n";

			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 2 );

			ss << "ret\n";
			break;

		case TESS_FACTOR_IN_ONE_FORK_INSTANCED:
			ss <<   "hs_fork_phase\n"
					"dcl_temps 1\n";

			// declare edge tesselation factors
			DeclareAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs );

			// declare inside tesselation factors
			DeclareAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs );

			// Declare remaining outputs
			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			ss << "dcl_hs_fork_phase_instance_count 6\n"
					"dcl_input vForkInstanceID\n"
					"mov r0.x, vForkInstanceID\n"
					"switch r0.x\n"
					"   case 0\n"
					"       mov o0.x, l(1.5)\n"
					"       break\n"
					"   case 1\n"
					"       mov o1.x, l(8.75)\n"
					"       break\n"
					"   case 2\n"
					"       mov o2.x, l(5.25)\n"
					"       break\n"
					"   case 3\n"
					"       mov o3.x, l(10.5)\n"
					"       break\n"
					"   case 4\n"
					"       mov o0.y, l(12.5)\n"
					"       break\n"
					"   case 5\n"
					"       mov o1.y, l(15.25)\n"
					"       break\n"
					"endswitch\n";

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 3 );

			ss << "ret\n";

			if( m_ExtraPatchConstantData )
			{
				ss << "hs_join_phase\n";

				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints );  
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 3 );
			}
			break;
			
		case TESS_FACTOR_IN_SEPARATE_FORK:
			// 1 fork phase for each edge tesselation factor
			DeclareAndSetAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs, "hs_fork_phase", 3 );

			// 1 fork phase for each inside tesselation factor
			DeclareAndSetAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs, "hs_fork_phase", 4 );

			// 1 fork phase for remaining outputs in o0-o4
			ss << "hs_fork_phase\n";

			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 4 );

			if( m_ExtraPatchConstantData )
			{
				ss << "hs_join_phase\n";

				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints );  
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 4 );
			}
			break;

		case TESS_FACTOR_EDGE_IN_FORK:
			// 1 fork phase for each edge tesselation factor
			DeclareAndSetAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs, "hs_fork_phase", 5 );

			// 1 join phase for each inside tesselation factor
			DeclareAndSetAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs, "hs_join_phase", 6 );

			// 1 join phase for remaining outputs in o0-o4
			ss << "hs_join_phase\n";

			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 5 );

			// 1 join phase for the outputs in o4-o32
			if( m_ExtraPatchConstantData )
			{
				ss << "hs_join_phase\n";

				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints );  
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 5 );

				ss << "ret\n";
			}
			break;

		case TESS_FACTOR_EDGE_IN_JOIN:
			// Fork phases output inside tesselation factors
			declInsideTessFactorFirst = true;
		  
			// 1 fork phase for each inside tesselation factor
			DeclareAndSetAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs, "hs_fork_phase", 6 );

			// 1 join phase for each edge tesselation factor
			DeclareAndSetAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs, "hs_join_phase", 5 );

			// 1 join phase for remaining outputs in o0-o4
			ss << "hs_join_phase\n";

			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 6 );

			if( m_ExtraPatchConstantData )
			{
				ss << "hs_join_phase\n";

				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints );  
				
				// Initialize outputs o4-o32 to at the begining of the fork phase
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 6 );

				ss << "ret\n";
			}
			break;

		case TESS_FACTOR_SPECIAL:
			ss << "hs_join_phase\n";

			// declare edge tesselation factors
			DeclareAllTesselationFactors( ss, edgeTessFactors, edgeTessFactorOutputRegs );

			// declare inside tesselation factors
			DeclareAllTesselationFactors( ss, insideTessFactors, insideTessFactorOutputRegs );

			// Declare remaining outputs in o0-o4
			DeclareRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks) );

			if( m_ExtraPatchConstantData )
			{
				// declare outputs o4-o31 and input control point registers
				DeclareExtraOutputsAndInputControlPoints( ss, m_NumInputControlPoints, m_NumOutputControlPoints );  
				
				// Initialize outputs o4-o32
				InitializeExtraOutputs( ss, m_NumInputControlPoints, m_NumOutputControlPoints, 6 );
			}

			// Initialize remaining outputs in o0-o4
			SetRemainingOutputs( ss, remainingOutputRegMasks, ARRAY_SIZE(remainingOutputRegMasks), 2 );

			for( UINT i = 0; i < edgeTessFactors.size(); i++ )
			{
				ss << "mov " << edgeTessFactorOutputRegs[i] << ", " << LiteralUINT4(FloatAsUINT(2.0), FloatAsUINT(3.0), FloatAsUINT(4.0), FloatAsUINT(5.0)) << "\n";
			}
			
			// Initialize inside tesselation factors in o0-o4
			for( UINT i = 0; i < insideTessFactors.size(); i++ )
			{
				ss << "mov " << insideTessFactorOutputRegs[i] << ", " << LiteralUINT4(FloatAsUINT(2.0), FloatAsUINT(3.0), FloatAsUINT(4.0), FloatAsUINT(5.0)) << "\n";
			}

			// Set 1 tesselation factor to a floating-point special value
			{
				// Use m_NumInputControlPoints to determine which tesselation factor will be a special floating point number 
				const UINT whichFactor = m_NumInputControlPoints % (edgeTessFactors.size() + insideTessFactors.size());

				// Use m_NumOutputControlPoints to determine which special value to use
				const UINT specialValues[] = 
				{
					0x7F800001, // positive SNAN
					0xFF800001, // negative SNAN
					0x7FC00000, // positive QNAN
					0xFFC00001, // negative QNAN
					0x7F800000, // positive INF
					0xFF800000, // negative INF
					0x807fffff, // negative denormal
					0x007fffff, // positive denormal
				};

				const UINT val = specialValues[ m_NumOutputControlPoints %  ARRAY_SIZE(specialValues) ];

				const char* outputReg = NULL;

				if( whichFactor < edgeTessFactors.size() )
				{
					outputReg = edgeTessFactorOutputRegs[ whichFactor ];
				}
				else
				{
					assert( (whichFactor - edgeTessFactors.size() ) < insideTessFactors.size() );

					outputReg = insideTessFactorOutputRegs[ whichFactor - edgeTessFactors.size() ];
				}

				ss << "mov " << outputReg << ", " << LiteralUINT4( val, val, val, val ) << "\n";
			}
			break;

		default:
			WriteToLog( "Unknown tesselation factor assignment" );
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		m_HullShader.shaderCode = ss.str();
		m_HullShader.location = SHADER_SOURCE_MEMORY_ASM;
	}

	{
		std::ostringstream ss;

		// Domain shader outputs it's inputs
		// This test only ensures that the hull shader generates the correct output
		// This "passthrough" domain shader is used to verify this
		ss <<   "ds_5_0\n"
				"dcl_input_control_point_count " << m_NumOutputControlPoints << "\n"
				"dcl_tessellator_domain domain_" << domain << "\n"
				"dcl_input vcp[" << m_NumOutputControlPoints << "][0].x\n"; // hs output control point (just passed-through vertex ID from vs)

		// Inputs 4-32 are patch constants
		if( m_ExtraPatchConstantData )
		{
			for( UINT i = 4; i < 32; i++ )
			{
				ss << "dcl_input vpc[" << i << "].xyzw\n";
			}
		}

		// The assembler requires the order of SIV declarations to match between the HS and DS
		// so if the HS declared inside tess factors before edge test factors, then the DS must match
		std::ostringstream declEdgeTessFactor;
		std::ostringstream declInsideTessFactor;
		
		for( UINT i = 0; i < edgeTessFactors.size(); i++ )
		{
			declEdgeTessFactor << "dcl_input_sv vpc[" << i << "].x, " << edgeTessFactors[i] << "\n";
		}

		for( UINT i = 0; i < insideTessFactors.size(); i++ )
		{
			declInsideTessFactor << "dcl_input_sv vpc[" << i << "].y, " << insideTessFactors[i] << "\n";
		}

		// Declare the inside tesselation factors first to make the assembler happy
		if( declInsideTessFactorFirst )
		{
			ss << declInsideTessFactor.str() << declEdgeTessFactor.str();
		}
		else
		{
			ss << declEdgeTessFactor.str() << declInsideTessFactor.str();
		}

		// Inputs 0-4 are a mix of regular patch constants, and system interpreted values
		for( UINT i = 0; i < ARRAY_SIZE(remainingOutputRegMasks); i++ )
		{
			ss << "dcl_input vpc[" << i << "]" << remainingOutputRegMasks[i] << "\n";
		}

		for( UINT i = 0; i < 32; i++ )
		{
			ss << "dcl_output o" << i << ".xyzw\n";
		}

		// Output tesselation factors
		for( UINT i = 0; i < 4; i++ )
		{
			ss << "mov o" << i << ".xyzw, vpc[" << i << "].xyzw\n";
		}

		// Output the rest of the patch data
		for( UINT i = 4; i < 32; i++ )
		{
			ss << "mov o" << i << ".xyzw, ";
			
			if( m_ExtraPatchConstantData )
			{
				ss << "vpc[" << i << "].xyzw\n";
			}
			else
			{
				ss << "l(0.0,1.0,2.0,3.0)\n";
			}
		}

		ss << "ret\n";

		m_DomainShader.shaderCode = ss.str();
		m_DomainShader.location = SHADER_SOURCE_MEMORY_ASM;
	}

	//
	//  Debugging code to review each generated shader
	//
	/*
		OutputDebugString("\nHull Shader\n");
		OutputDebugString(m_HullShader.shaderCode.c_str());

		OutputDebugString("\nDomain Shader\n");
		OutputDebugString(m_DomainShader.shaderCode.c_str());
	*/

	D3D11_SO_DECLARATION_ENTRY soDecl[32];

	for( UINT i = 0; i < 32; i++ )
	{
		soDecl[i].Stream = 0;
		soDecl[i].SemanticName = outputNames[i];
		soDecl[i].SemanticIndex = 0;
		soDecl[i].StartComponent = 0;
		soDecl[i].ComponentCount = 4;
		soDecl[i].OutputSlot = 0;
	}

	tRes = CTessellationConfTest::SetupResources( soDecl, ARRAYSIZE(soDecl) );
	if(tRes != RESULT_PASS)
		goto setupDone;

	UINT offset = 0;
	ON_EACH_CONTEXT->SOSetTargets(1, &m_pSOBuffer[CONTEXT_ID], &offset);

	D3D11_PRIMITIVE_TOPOLOGY IAtopology = (D3D11_PRIMITIVE_TOPOLOGY)(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + m_NumInputControlPoints - 1);

	ON_EACH_CONTEXT->IASetPrimitiveTopology(IAtopology);

setupDone:
	return tRes;
}

TEST_RESULT CHullShaderPhasesTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStats[2] = {0};
	D3D11_QUERY_DATA_SO_STATISTICS soStats[2];

	std::string topologyName = GetTestCaseParameter( "Topology" )->GetValueAsString();

	std::auto_ptr< TestBuffer<DWORD> > buffer0;
	std::auto_ptr< TestBuffer<DWORD> > buffer1;

	// issue queries & draw
	ON_EACH_CONTEXT->Begin(m_pSOQuery[CONTEXT_ID]);
	ON_EACH_CONTEXT->Begin(m_pPipeStatsQuery[CONTEXT_ID]);

	ON_EACH_CONTEXT->Draw(m_NumInputControlPoints, 0);

	ON_EACH_CONTEXT->End(m_pPipeStatsQuery[CONTEXT_ID]);
	ON_EACH_CONTEXT->End(m_pSOQuery[CONTEXT_ID]);

	// retrieve queries
	tRes = CheckPipeStats( pipeStats );
	if(tRes != RESULT_PASS)
		goto testDone;

	if( (0 != pipeStats[0].HSInvocations) && (0 == pipeStats[1].HSInvocations) )
	{
		WriteToLog( "HSInvocations was > 0 when it was expected to be 0" );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	if( (0 == pipeStats[0].HSInvocations) && (0 != pipeStats[1].HSInvocations) )
	{
		WriteToLog( "HSInvocations was 0 when it was expected to be > 0" );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	if( (0 != pipeStats[0].DSInvocations) && (0 == pipeStats[1].DSInvocations) )
	{
		WriteToLog( "DSInvocations was > 0 when it was expected to be 0" );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	if( (0 == pipeStats[0].DSInvocations) && (0 != pipeStats[1].DSInvocations) )
	{
		WriteToLog( "DSInvocations was 0 when it was expected to be > 0" );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	// The TESS_FACTOR_SPECIAL scenario will cause 0 outputs to be written
	// because NAN tesselation factors cause patches to be culled, this is expected
	// but will cause CheckSOQueries to fail
	tRes = CheckSOQueries(soStats, 2, m_TessFactorPhase != TESS_FACTOR_SPECIAL);

	if(tRes != RESULT_PASS)
		goto testDone;
	

	UINT64 bufferSizes[2] = {soStats[0].NumPrimitivesWritten, soStats[1].NumPrimitivesWritten};

	if( soStats[0].NumPrimitivesWritten != soStats[1].NumPrimitivesWritten )
	{
		WriteToLog( "Stream-output statistics were wrong.  Expected = %u, found = %u", (UINT)soStats[1].NumPrimitivesWritten, (UINT)soStats[0].NumPrimitivesWritten );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	const UINT numDWORDSPerPrimitive = 32;
	const UINT numDWORDSPerBuffer = (UINT)(soStats[0].NumPrimitivesWritten * numDWORDSPerPrimitive);
	
	buffer0 = std::auto_ptr< TestBuffer<DWORD> >( new TestBuffer<DWORD>(m_pDeviceArray[0], m_pContextArray[0], m_pSOBuffer[0], numDWORDSPerBuffer, topologyName) );
	buffer1 = std::auto_ptr< TestBuffer<DWORD> >( new TestBuffer<DWORD>(m_pDeviceArray[1], m_pContextArray[1], m_pSOBuffer[1], numDWORDSPerBuffer, topologyName) );

	if( buffer0->HasError() )
	{
		WriteToLog( buffer0->ErrorString().c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	if( buffer1->HasError() )
	{
		WriteToLog( buffer1->ErrorString().c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	const DWORD* pData0 = buffer0->Contents();
	const DWORD* pData1 = buffer1->Contents();

	for( UINT i = 0; i < numDWORDSPerBuffer; i++ )
	{
		if( pData0[i] != pData1[i] )
		{
			WriteToLog( "Buffer contents did not match ref output.  Byte offset: %u, expected: 0x%x , found: 0x%x", (UINT)(i * sizeof(DWORD)), pData1[i], pData0[i] );
			tRes = RESULT_FAIL;
			goto testDone;
		}
	}

testDone:
	return tRes;
}

void CHullShaderPhasesTest::CleanupTestCase()
{
	CTessellationConfTest::CleanupResources();
}