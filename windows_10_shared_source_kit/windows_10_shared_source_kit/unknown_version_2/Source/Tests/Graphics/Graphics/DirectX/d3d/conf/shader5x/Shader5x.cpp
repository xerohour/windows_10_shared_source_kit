/////////////////////////////////////////////////////////////////////////
//  Shader5xTest.cpp
//  created:	2005/03/10
//
//  purpose:
//////////////////////////////////////////////////////////////////////////

#include <WGFTestCore.h>

#include "Shader5x.h"
#include "LoadStore.h"
#include "Raster.h"
#include "ResourceAccess.h"
#include "Condition.h"
#include "ControlFlow.h"
#include "Move.h"
#include "Topology.h"
#include "FloatArith.h"
#include "DoublesArith.h"
#include "Bitwise.h"
#include "IntArith.h"
#include "TypeConvert.h"
#include "Swizzle.h"
#include "Index.h"
#include "ConstantBuffers.h"
#include "Registers.h"
#include "Multisample.h"
#include "MemoryAccessInstructions.h"
#include "SumAbsDiff.h"

#include <testasm_api.h>
#include <d3d11on12.h>

CShader5xTestApp g_TestApp;

bool g_bDumpDissasm;
bool g_bForceSM40;

//////////////////////////////////////////////////////////////////////////
// Common Shaders
//////////////////////////////////////////////////////////////////////////

//This shader is used only to get output signature
const char szNullGS[] =
	"vs_4_0"							"\n"
	"dcl_input_sv v[0].x, vertexID"		"\n"
	"dcl_output o[0].xyzw"				"\n"
	"mov o[0], v[0].x"					"\n" // Simple passthrough
	;

//This shader is used only to clear the stream output
const char szClearStreamOutputVS[] =
	"vs_4_0"						"\n"
	"dcl_input_sv v[0].x, vertexID"	"\n"
	"dcl_output o[0].xyzw"			"\n"
	"mov o[0], " str_CLEAR_VALUE		"\n" // Clear
	;

DXGI_FORMAT Formats[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_SINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT
};

//////////////////////////////////////////////////////////////////////////
// djb2 Hashing Function
//////////////////////////////////////////////////////////////////////////
unsigned long
HashString( const unsigned char *str )
{
	unsigned long hash = /*5381*/ 11519;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}


//////////////////////////////////////////////////////////////////////////
// To/FromString()
//////////////////////////////////////////////////////////////////////////
tstring __stdcall ToString( const int4& Vector )
{
	return ToString( Vector[0] ) + tstring( ", " ) + ToString( Vector[1] ) + tstring( ", " ) + ToString( Vector[2] ) + tstring( ", " ) + ToString( Vector[3] );
}

bool __stdcall FromString( int4 *pVector, const tstring &str )
{
	tstring::size_type lastPos = 0;

	for (int i=0; i<4; i++)
	{
		tstring::size_type pos = str.find_first_of(",", lastPos);
		if (pos == tstring::npos)
			return false;

		// found a token, add it to the vector.
		if (! FromString( &pVector[i], str.substr(lastPos, pos - lastPos) ) )
			return false;

		// skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(",", pos);
//		if (lastPos == string::npos)
//			return false;
	}
	return true;
}

tstring __stdcall ToString( const uint4& Vector )
{
	return ToString( Vector[0] ) + tstring( ", " ) + ToString( Vector[1] ) + tstring( ", " ) + ToString( Vector[2] ) + tstring( ", " ) + ToString( Vector[3] );
}

bool __stdcall FromString( uint4 *pVector, const tstring &str )
{
	tstring::size_type lastPos = 0;

	for (int i=0; i<4; i++)
	{
		tstring::size_type pos = str.find_first_of(",", lastPos);
		if (pos == tstring::npos)
			return false;

		// found a token, add it to the vector.
		if (! FromString( &pVector[i], str.substr(lastPos, pos - lastPos) ) )
			return false;

		// skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(",", pos);
//		if (lastPos == string::npos)
//			return false;
	}
	return true;
}
tstring __stdcall ToString( SRC_MOD_TYPE type )
{
	switch( type )
	{
		case SMOD_NONE:
			return tstring( _T( "NONE" ) );
			break;

		case SMOD_NEG:
			return tstring( _T( "NEG" ) );
			break;

		case SMOD_ABS:
			return tstring( _T( "ABS" ) );
			break;

		case SMOD_ABSNEG:
			return tstring( _T( "ABSNEG" ) );
			break;

		default:
			return tstring( _T( "" ) );
			break;
	}
};

tstring __stdcall ToString( INST_MOD_TYPE type )
{
	switch( type )
	{
		case IMOD_NONE:
			return tstring( _T( "NONE" ) );
			break;

		case IMOD_SAT:
			return tstring( _T( "SAT" ) );
			break;

		default:
			return tstring( _T( "" ) );
			break;
	}
};

tstring __stdcall ToString( SHADER_STAGE_TYPE type )
{
	switch( type )
	{
		case SHADER_STAGE_VERTEX:
			return tstring( _T( "Vertex" ) );
			break;

		case SHADER_STAGE_GEOMETRY:
			return tstring( _T( "Geometry" ) );
			break;

		case SHADER_STAGE_PIXEL:
			return tstring( _T( "Pixel" ) );
			break;

		case SHADER_STAGE_COMPUTE:
			return tstring( _T( "Compute" ) );
			break;

		default:
			return tstring( _T( "" ) );
			break;
	}
};

bool __stdcall FromString( SRC_MOD_TYPE *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "NONE" ) )
	{
		*pType = SMOD_NONE;
		return true;
	}

	if( tcistring( str ) == _T( "NEG" ) )
	{
		*pType = SMOD_NEG;
		return true;
	}

	if( tcistring( str ) == _T( "ABS" ) )
	{
		*pType = SMOD_ABS;
		return true;
	}

	if( tcistring( str ) == _T( "ABSNEG" ) )
	{
		*pType = SMOD_ABSNEG;
		return true;
	}

	return false;
}

bool __stdcall FromString( INST_MOD_TYPE *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "NONE" ) )
	{
		*pType = IMOD_NONE;
		return true;
	}

	if( tcistring( str ) == _T( "SAT" ) )
	{
		*pType = IMOD_SAT;
		return true;
	}

	return false;
}

bool __stdcall FromString( SHADER_STAGE_TYPE *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "Vertex" ) )
	{
		*pType = SHADER_STAGE_VERTEX;
		return true;
	}

	if( tcistring( str ) == _T( "Geometry" ) )
	{
		*pType = SHADER_STAGE_GEOMETRY;
		return true;
	}

	if( tcistring( str ) == _T( "Pixel" ) )
	{
		*pType = SHADER_STAGE_PIXEL;
		return true;
	}

	if( tcistring( str ) == _T( "Compute" ) )
	{
		*pType = SHADER_STAGE_COMPUTE;
		return true;
	}

	return false;
}

BEGIN_NAMED_VALUES( OPERAND_TYPE )
	NAMED_VALUE( _T( "Temp" ), OT_TEMP )
	NAMED_VALUE( _T( "ConstantBuffer" ), OT_CONSTANT_BUFFER )
END_NAMED_VALUES( OPERAND_TYPE )

void CShader5xTestApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	int n = AddOptionCategory( _T( "Debug" ) );
	RegisterOptionVariable( _T( "DumpAsm" ), _T( "Allows shader disassembly dumps to the message window." ), &g_bDumpDissasm, false, 0, n );
	RegisterOptionVariable( _T( "ForceSM40" ), _T( "Force Shader Model 4.0 even when running on 10.1 device" ), &g_bForceSM40, false, 0, n );

	UnRegisterOptionVariable( "SrcOnly" );
	m_D3DOptions.SrcOnly = true;
}

bool CShader5xTestApp::InitTestGroups()
{
	//BUGBUG: resinfo temporarily broken due to HLSL workaround, see bug #102184

	if( !ADD_TEST( _T( "LoadStore/ld_uav_typed" ), LoadStoreTest::CLd_uav_typed ) ) return false;
	if( !ADD_TEST( _T( "LoadStore/ld_uav_typed_s" ), LoadStoreTest::CLd_uav_typed_s ) ) return false;
	if( !ADD_TEST( _T( "LoadStore/store_uav_typed" ), LoadStoreTest::CStore_uav_typed ) ) return false;
	if( !ADD_TEST( _T( "LoadStore/store_raw" ), LoadStoreTest::CStore_raw ) ) return false;
	if( !ADD_TEST( _T( "LoadStore/store_structured" ), LoadStoreTest::CStore_structured ) ) return false;

	if( !ADD_TEST( _T( "ResourceAccess/bufinfo" ), CShader5xTest_bufinfo ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/resinfo" ), CShader5xTest_resinfo ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld" ), CShader5xTest_ld ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld_s" ), CShader5xTest_ld_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld_raw" ), CShader5xTest_ld_raw ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld_raw_s" ), CShader5xTest_ld_raw_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld_structured" ), CShader5xTest_ld_structured ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/ld_structured_s" ), CShader5xTest_ld_structured_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample" ), CShader5xTest_sample ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_s" ), CShader5xTest_sample_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_cl" ), CShader5xTest_sample_cl ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_cl_s" ), CShader5xTest_sample_cl_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_min_lod" ), CShader5xTest_sample_min_lod ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_l" ), CShader5xTest_sample_l ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_l_s" ), CShader5xTest_sample_l_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c" ), CShader5xTest_sample_c ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_s" ), CShader5xTest_sample_c_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_cl" ), CShader5xTest_sample_c_cl ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_cl_s" ), CShader5xTest_sample_c_cl_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_min_lod" ), CShader5xTest_sample_c_min_lod ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_lz" ), CShader5xTest_sample_c_lz ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_lz_s" ), CShader5xTest_sample_c_lz_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_c_lz_min_lod" ), CShader5xTest_sample_c_lz_min_lod ) ) return false; // this test always calls SetResourceMinLOD

	if( !ADD_TEST( _T( "ResourceAccess/sample_b" ), CShader5xTest_sample_b ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_b_s" ), CShader5xTest_sample_b_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_b_cl" ), CShader5xTest_sample_b_cl ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_b_cl_s" ), CShader5xTest_sample_b_cl_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4" ), CShader5xTest_gather4 ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_s" ), CShader5xTest_gather4_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_c" ), CShader5xTest_gather4_c ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_c_s" ), CShader5xTest_gather4_c_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_po" ), CShader5xTest_gather4_po ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_po_s" ), CShader5xTest_gather4_po_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_po_c" ), CShader5xTest_gather4_po_c ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/gather4_po_c_s" ), CShader5xTest_gather4_po_c_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/lod" ), CShader5xTest_lod ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_d" ), CShader5xTest_sample_d ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_d_s" ), CShader5xTest_sample_d_s ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_d_cl" ), CShader5xTest_sample_d_cl ) ) return false;
	if( !ADD_TEST( _T( "ResourceAccess/sample_d_cl_s" ), CShader5xTest_sample_d_cl_s ) ) return false;

	if( !ADD_TEST( _T( "Raster/deriv/rtx_coarse" ), CShader5xTest_deriv_rtx_coarse ) ) return false;
	if( !ADD_TEST( _T( "Raster/deriv/rty_coarse" ), CShader5xTest_deriv_rty_coarse ) ) return false;
	if( !ADD_TEST( _T( "Raster/deriv/rtx_fine" ), CShader5xTest_deriv_rtx_fine ) ) return false;
	if( !ADD_TEST( _T( "Raster/deriv/rty_fine" ), CShader5xTest_deriv_rty_fine ) ) return false;

	if( !ADD_TEST( _T( "Condition/eq" ), CShader5xTest_eq ) ) return false;
	if( !ADD_TEST( _T( "Condition/ne" ), CShader5xTest_ne ) ) return false;
	if( !ADD_TEST( _T( "Condition/ge" ), CShader5xTest_ge ) ) return false;
	if( !ADD_TEST( _T( "Condition/lt" ), CShader5xTest_lt ) ) return false;
	if( !ADD_TEST( _T( "Condition/ieq" ), CShader5xTest_ieq ) ) return false;
	if( !ADD_TEST( _T( "Condition/ine" ), CShader5xTest_ine ) ) return false;
	if( !ADD_TEST( _T( "Condition/ige" ), CShader5xTest_ige ) ) return false;
	if( !ADD_TEST( _T( "Condition/ilt" ), CShader5xTest_ilt ) ) return false;
	if( !ADD_TEST( _T( "Condition/uge" ), CShader5xTest_uge ) ) return false;
	if( !ADD_TEST( _T( "Condition/ult" ), CShader5xTest_ult ) ) return false;
	if( !ADD_TEST( _T( "Condition/dmin" ), CShader5xTest_dmin ) ) return false;
	if( !ADD_TEST( _T( "Condition/dmax" ), CShader5xTest_dmax ) ) return false;
	if( !ADD_TEST( _T( "Condition/deq" ), CShader5xTest_deq ) ) return false;
	if( !ADD_TEST( _T( "Condition/dge" ), CShader5xTest_dge ) ) return false;
	if( !ADD_TEST( _T( "Condition/dlt" ), CShader5xTest_dlt ) ) return false;
	if( !ADD_TEST( _T( "Condition/dne" ), CShader5xTest_dne ) ) return false;

	if( !ADD_TEST( _T( "ControlFlow/TrueValues" ), CShader5xTest_ControlFlowTrueValues ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/TrueValuesNC" ), CShader5xTest_ControlFlowTrueValuesNC ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/Nested" ), CShader5xTest_NestedControlFlow ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/if" ), CShader5xTest_if ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/loop" ), CShader5xTest_loop ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/switch" ), CShader5xTest_switch ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/switchNC" ), CShader5xTest_switchNC ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/call" ), CShader5xTest_call ) ) return false;
	if( !ADD_TEST( _T( "ControlFlow/noncoherent" ), CShader5xTest_ControlFlowNC ) ) return false;

	if( !ADD_TEST( _T( "Topology/cut" ), CShader5xTest_cut  ) ) return false;
	if( !ADD_TEST( _T( "Topology/emit" ), CShader5xTest_emit  ) ) return false;
	if( !ADD_TEST( _T( "Topology/emitThenCut" ), CShader5xTest_emitThenCut ) ) return false;
	if( !ADD_TEST( _T( "Topology/emit_stream" ), CShader5xTest_emitStream ) ) return false;
	if( !ADD_TEST( _T( "Topology/cut_stream" ), CShader5xTest_cutStream ) ) return false;
	if( !ADD_TEST( _T( "Topology/emitThenCut_stream" ), CShader5xTest_emitThenCutStream ) ) return false;

	if( !ADD_TEST( _T( "Move/swapc" ), CShader5xTest_swapc ) ) return false;
	if( !ADD_TEST( _T( "Move/dmov" ), CShader5xTest_dmov ) ) return false;
	if( !ADD_TEST( _T( "Move/dmovc" ), CShader5xTest_dmovc ) ) return false;
	if( !ADD_TEST( _T( "Move/cycleCounter" ), CShader5xTest_cyclecounter ) ) return false;
//	if( !ADD_TEST( _T( "Move/mov" ), CShader5xTest_mov ) ) return false; // Covered in flow control?
//	if( !ADD_TEST( _T( "Move/movc" ), CShader5xTest_movc ) ) return false; // Covered in flow control?

	if( !ADD_TEST( _T( "FloatArith/add" ), CShader5xTest_add ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/div" ), CShader5xTest_div ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/dp2" ), CShader5xTest_dp2 ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/dp3" ), CShader5xTest_dp3 ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/dp4" ), CShader5xTest_dp4 ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/exp" ), CShader5xTest_exp ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/frc" ), CShader5xTest_frc ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/log" ), CShader5xTest_log ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/mad" ), CShader5xTest_mad ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/max" ), CShader5xTest_max ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/min" ), CShader5xTest_min ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/mul" ), CShader5xTest_mul ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/round_ne" ), CShader5xTest_round_ne ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/round_ni" ), CShader5xTest_round_ni ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/round_pi" ), CShader5xTest_round_pi ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/round_z" ), CShader5xTest_round_z ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/rsq" ), CShader5xTest_rsq ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/rcp" ), CShader5xTest_rcp ) ) return false;

	if( !ADD_TEST( _T( "FloatArith/sincos" ), CShader5xTest_sincos ) ) return false;
	if( !ADD_TEST( _T( "FloatArith/sqrt" ), CShader5xTest_sqrt ) ) return false;


	if( !ADD_TEST( _T( "DoubleArith/dadd" ), CShader5xTest_dadd ) ) return false;
	if( !ADD_TEST( _T( "DoubleArith/dmul" ), CShader5xTest_dmul ) ) return false;
	if( !ADD_TEST( _T( "DoubleArith/ftod" ), CShader5xTest_ftod ) ) return false;
	if( !ADD_TEST( _T( "DoubleArith/dtof" ), CShader5xTest_dtof ) ) return false;

	//  4/24/11:        D3D11.1 Shader Model Improvements
	//  Calculations:   ddiv, drcp, dfma
	if( !ADD_TEST( _T( "Extensions11_1/DoubleArith/ddiv" ), CShader5xTest_ddiv ) ) return false;
	if( !ADD_TEST( _T( "Extensions11_1/DoubleArith/drcp" ), CShader5xTest_drcp ) ) return false;
	if( !ADD_TEST( _T( "Extensions11_1/DoubleArith/dfma" ), CShader5xTest_dfma ) ) return false;
	//  Conversions:    dtoi, dtou, itod, utod
	if( !ADD_TEST( _T( "Extensions11_1/TypeConvert/dtoi" ), CShader5xTest_dtoi ) ) return false;
	if( !ADD_TEST( _T( "Extensions11_1/TypeConvert/dtou" ), CShader5xTest_dtou ) ) return false;
	if( !ADD_TEST( _T( "Extensions11_1/TypeConvert/itod" ), CShader5xTest_itod ) ) return false;
	if( !ADD_TEST( _T( "Extensions11_1/TypeConvert/utod" ), CShader5xTest_utod ) ) return false;
	// Sum of Absolute Differences (SAD): msad
	if( !ADD_TEST( _T( "Extensions11_1/SumAbsDiff/msad" ), CShader5xTest_msad ) ) return false;

	if( !ADD_TEST( _T( "Bitwise/and" ), CShader5xTest_and ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/ishl" ), CShader5xTest_ishl ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/ishr" ), CShader5xTest_ishr ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/not" ), CShader5xTest_not ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/or" ), CShader5xTest_or ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/ushr" ), CShader5xTest_ushr ) ) return false;
	if( !ADD_TEST( _T( "Bitwise/xor" ), CShader5xTest_xor ) ) return false;

	if( !ADD_TEST( _T( "Bitfield/ubfe" ), CShader5xTest_ubfe ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/ibfe" ), CShader5xTest_ibfe ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/bfi" ), CShader5xTest_bfi ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/bfrev" ), CShader5xTest_bfrev ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/countbits" ), CShader5xTest_countbits ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/firstbit/lo" ), CShader5xTest_firstbit_lo ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/firstbit/hi" ), CShader5xTest_firstbit_hi ) ) return false;
	if( !ADD_TEST( _T( "Bitfield/firstbit/shi" ), CShader5xTest_firstbit_shi ) ) return false;

	if( !ADD_TEST( _T( "IntArith/iadd" ), CShader5xTest_iadd ) ) return false;
	if( !ADD_TEST( _T( "IntArith/imad" ), CShader5xTest_imad ) ) return false;
	if( !ADD_TEST( _T( "IntArith/imax" ), CShader5xTest_imax ) ) return false;
	if( !ADD_TEST( _T( "IntArith/imin" ), CShader5xTest_imin ) ) return false;
	if( !ADD_TEST( _T( "IntArith/imul" ), CShader5xTest_imul ) ) return false;
	if( !ADD_TEST( _T( "IntArith/ineg" ), CShader5xTest_ineg ) ) return false;
	if( !ADD_TEST( _T( "IntArith/udiv" ), CShader5xTest_udiv ) ) return false;
	if( !ADD_TEST( _T( "IntArith/umad" ), CShader5xTest_umad ) ) return false;
	if( !ADD_TEST( _T( "IntArith/umax" ), CShader5xTest_umax ) ) return false;
	if( !ADD_TEST( _T( "IntArith/umin" ), CShader5xTest_umin ) ) return false;
	if( !ADD_TEST( _T( "IntArith/umul" ), CShader5xTest_umul ) ) return false;
	if( !ADD_TEST( _T( "IntArith/uaddc" ), CShader5xTest_uaddc ) ) return false;
	if( !ADD_TEST( _T( "IntArith/usubb" ), CShader5xTest_usubb ) ) return false;

	if( !ADD_TEST( _T( "TypeConvert/ftoi" ), CShader5xTest_ftoi ) ) return false;
	if( !ADD_TEST( _T( "TypeConvert/ftou" ), CShader5xTest_ftou ) ) return false;
	if( !ADD_TEST( _T( "TypeConvert/itof" ), CShader5xTest_itof ) ) return false;
	if( !ADD_TEST( _T( "TypeConvert/utof" ), CShader5xTest_utof ) ) return false;
	if( !ADD_TEST( _T( "TypeConvert/f32tof16" ), CShader5xTest_f32tof16 ) ) return false;
	if( !ADD_TEST( _T( "TypeConvert/f16tof32" ), CShader5xTest_f16tof32 ) ) return false;

	if( !ADD_TEST( _T( "MaskSwizzle" ), CShader5xTest_MaskSwizzle ) ) return false;
	if( !ADD_TEST( _T( "Index/Temps" ), CShader5xTest_IndexTemps ) ) return false;
	if( !ADD_TEST( _T( "Index/ConstantBuffers" ), CShader5xTest_IndexConstantBuffers ) ) return false;
	if( !ADD_TEST( _T( "Index/Samplers" ), CShader5xTest_IndexSamplers ) ) return false;
	if( !ADD_TEST( _T( "Index/Textures" ), CShader5xTest_IndexTextures ) ) return false;

	if( !ADD_TEST( _T( "ConstantBuffers" ), CShader5xTest_ConstantBuffers ) ) return false;
	if( !ADD_TEST( _T( "Registers/Limits/Temp" ), CShader5xTest_Registers_LimitsTemp ) ) return false;
	if( !ADD_TEST( _T( "Registers/Limits/InputOutput" ), CShader5xTest_Registers_LimitsInputOutput ) ) return false;

	if( !ADD_TEST( _T( "MemoryAccess/ImmAtomicAllocConsume" ), MemoryAccessInstructions::CImmAtomicAllocConsume ) ) return false;


#define DEFINE_ATOMIC_TEST( Name, NumSrcs, bSINT, bUINT, bImmediate, calc ) \
	if( !ADD_TEST( _T( "MemoryAccess/" #Name ), MemoryAccessInstructions::CShader5xTest_Atomic_##Name ) ) return false;
#include "AtomicTestCases.h"
#undef DEFINE_ATOMIC_TEST

	//
	// Multisample
	//

	// need a device... or else there will be a ton (2000lbs) of skips
	if ( RESULT_PASS != SetupDXGIAndD3D() )
	{
		return true;
	}

	for( UINT uSampleCount = 1; uSampleCount <= 32; ++uSampleCount )
	{
		UINT uMaxLevel = 0;

		// Check the test's priority level
		if( 1 == GetExecutionPriorityLevel() )
		{
			// For pri 1, only use these sample counts
			if( 4 != uSampleCount &&
				2 != uSampleCount &&
				1 != uSampleCount )
			{
				// If it's not in the list, skip the sample count
				continue;
			}
		}
		else if( 2 == GetExecutionPriorityLevel() )
		{
			// For pri 2, only use these sample counts
			if( 8 != uSampleCount &&
				4 != uSampleCount &&
				2 != uSampleCount &&
				1 != uSampleCount )
			{
				// If it's not in the list, skip the sample count
				continue;
			}
		}

		// Check Multisample Quality Levels
		for (UINT uFormat = 0; uFormat < ARRAY_SIZE( Formats ); ++uFormat)
		{
			UINT uLevels;
			if( FAILED( GetDevice()->CheckMultisampleQualityLevels( Formats[uFormat], uSampleCount, &uLevels ) ) )
			{
				WriteToLog("CheckMultisampleQualityLevels failed.");
				return false;
			}

			if ( uLevels > uMaxLevel )
				uMaxLevel = uLevels;
		}

		if (uMaxLevel)
		{
			tstring szGroup;
			szGroup = _T( "Multisample/ld2dms/" ) + ToString(uSampleCount) + _T("/") + ToString(uMaxLevel);
			if( !ADD_TEST( szGroup, CMSLoadTest ) ) return false;

			szGroup = _T( "Multisample/ldms_s/" ) + ToString(uSampleCount) + _T("/") + ToString(uMaxLevel);
			if ( !ADD_TEST( szGroup, CMSLoadTestResidencyFeedback ) ) return false;

			if ( m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1 )
			{
				szGroup = _T( "Multisample/sampleinfo/" ) + ToString(uSampleCount) + _T("/") + ToString(uMaxLevel);
				if( !ADD_TEST( szGroup, CSampleInfoTest ) ) return false;

				szGroup = _T( "Multisample/samplepos/" ) + ToString(uSampleCount) + _T("/") + ToString(uMaxLevel);
				if( !ADD_TEST( szGroup, CSamplePosTest ) ) return false;

				szGroup = _T( "Multisample/sampleindex/" ) + ToString(uSampleCount) + _T("/") + ToString(uMaxLevel);
				if( !ADD_TEST( szGroup, CSampleIndexTest ) ) return false;
			}
		}
	}

	//
	// Check some caps bits on the device (needed for init test parameters)
	//
	m_bIs11_1DDISupported = Is11_1DDISupported( GetDevice() ) ? true : false;	// Only 11.1 DDI drivers support doubles extensions and MSAD.
	const bool bFL10 = (m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 || m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1);
	m_bFL11OrGreater = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0;
	m_bFL11_1OrGreater = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_1;
	m_featureLevel = g_TestApp.m_D3DOptions.FeatureLevel;
	ZeroMemory( &m_hwOptions, sizeof(m_hwOptions) );

	//
	// Check compute on 10 support
	//

	if( bFL10 )
	{
		// Check the caps of the HW
		HRESULT hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &m_hwOptions, sizeof(m_hwOptions) );

		if( FAILED( hr ) )
		{
			WriteError( _T("CheckFeatureSupport(D3D10_X_HARDWARE_OPTIONS) failed. HR = %s"), D3DHResultToString(hr).c_str() );
			return false;
		}

		// Set some flags
		if( g_TestApp.m_hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
		{
			m_bHWSupportsComputeOn10 = true;
		}
		else
		{
			WriteToLog( _T("Skipping all compute on 10 instructions, as the device does not support SM 4.x compute features.") );
			m_bHWSupportsComputeOn10 = false;
		}
	}

	// Set the flag
	m_bFL10AndHWSupportsComputeOn10 = bFL10 && m_bHWSupportsComputeOn10;

	//
	// Check doubles support
	//

	HRESULT hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &m_hwDoubles, sizeof(m_hwDoubles) );

	if( FAILED( hr ) )
	{
		WriteError( _T("CheckFeatureSupport(D3D11_FEATURE_DOUBLES) failed. HR = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	// Set the flag
	if( g_TestApp.m_hwDoubles.DoublePrecisionFloatShaderOps )
		m_bHWSupportsDoubles = true;
	else
		m_bHWSupportsDoubles = false;

	ID3D11On12Device* pDevice11On12;
	if( SUCCEEDED( GetDevice()->QueryInterface(IID_PPV_ARGS(&pDevice11On12)) ) )
	{
		m_bIsD3D11On12 = true;
		SAFE_RELEASE(pDevice11On12);
	}
	else
		m_bIsD3D11On12 = false;

	CleanupD3D();
	CleanupDXGI();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// CShader5xTest
//////////////////////////////////////////////////////////////////////////

TEST_RESULT CShader5xTest::Setup()
{
	TEST_RESULT tr = SetupD3D();
	CheckTiledSupport();
	return tr;
}

void CShader5xTest::CheckTiledSupport()
{
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
	if (SUCCEEDED(GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData))))
	{
		TiledTier = featureData.TiledResourcesTier;
	}
}

void CShader5xTest::Cleanup()
{
	// Clear the shader blobs
	ShaderBlobMapType::iterator it;
	for( it = m_ShaderBlobMap.begin(); it != m_ShaderBlobMap.end(); ++it )
	{
		// Release the shader blob
		ID3D10Blob *pBlob = it->second;
		if( pBlob )
			pBlob->Release();
	}

	m_ShaderBlobMap.clear();

	CleanupD3D();
	CD3D11Test::Cleanup();
}

FLOAT_INPUT_CLASS CShader5xTest::GetInputClass( float f )
{
	if( _isnan( f ) )
		return FIC_NAN;

	if( f == 1 )
		return FIC_ONEp;

	if( f == -1 )
		return FIC_ONEn;

	DWORD dw = *( (DWORD *) &f );

	if( dw & 0x80000000 )
	{
		dw &= 0x7fffffff;

		if( dw == 0x7f800000 )
			return FIC_INFn;

		if( dw == 0 )
			return FIC_ZEROn;

		if( dw <= 0x007fffff )
			return FIC_DENn;

		return FIC_REALn;
	}
	else
	{
		if( dw == 0x7f800000 )
			return FIC_INFp;

		if( dw == 0 )
			return FIC_ZEROp;

		if( dw <= 0x007fffff )
			return FIC_DENp;

		return FIC_REALp;
	}
}

float CShader5xTest::ResolveSrc( float f, SRC_MOD_TYPE mod )
{
	DWORD *pdwValue = (DWORD *) &f; // Need this to test for -0.0 due to compiler issue
	float returnValue;
	DWORD *pdwReturnValue = (DWORD *) &returnValue; // Also needed for -0.0 case

	switch( mod )
	{
		default:
		case SMOD_NONE:
			returnValue = f;
			break;

		case SMOD_NEG:
			if( *pdwValue == 0x00000000 )
				*pdwReturnValue = 0x80000000;
			else
				returnValue = -f;
			break;

		case SMOD_ABS:
			if( *pdwValue == 0x80000000 )
				*pdwReturnValue = 0x00000000;
			else
				returnValue = fabsf( f );
			break;

		case SMOD_ABSNEG:
			if( *pdwValue == 0x00000000 || *pdwValue == 0x80000000 )
				*pdwReturnValue = 0x80000000;
			else
				returnValue = -fabsf( f );
			break;
	}

	return returnValue;
}

float CShader5xTest::ResolveDest( float f, INST_MOD_TYPE mod )
{
	switch( mod )
	{
		case IMOD_NONE:
			return f;
			break;

		case IMOD_SAT:
			if( _isnan( f ) )
				return 0;

			return min( 1.0f, max( 0.0f, f ) );
			break;
	}

	return 0;
}

bool CShader5xTest::SkipBasedOnCommonFactors()
{
	// Should the current testcase be skipped based on test factors common to all shader5x tests?
	// This should be called from the SetupTestCase of each test group
	// return true = yes skip the current testcase
	// return false = no don't skip the current testcase

	// Hull and Domain shaders are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() &&
		( D3D_SHADER_STAGE_HULL == m_ShaderStage ||
		  D3D_SHADER_STAGE_DOMAIN == m_ShaderStage ) )
		return true;

	// Checks to see if UAV resources are allowed (Only on FL11+ or FL10 IF the hardware says it supports it)
	if( !g_TestApp.UAVIsAllowed() &&
		D3D_SHADER_STAGE_COMPUTE == m_ShaderStage )
		return true;

	// Default is don't skip
	return false;
}

void CShader5xTest::AddCommonParameters()
{
	// Every test group needs to identify the current shader stage
	AddParameter( _T(m_shaderStageParameterStr), &m_ShaderStage );
}

testfactor::RFactor CShader5xTest::GetTestFactorWithDefaultStageCoverage( testfactor::RFactor testcases )
{
	// Most test groups in shader5x will need to cycle on a shader stage. Usually the stage doesn't matter.
	// When the stage doesn't matter, the group should use this function. Otherwise, they should implement their own factor on a per group basis.

	// This function takes the testcases defined in a factor, and adds the different shader stages to the mix in such a way that
	// testcase number x will be the same accross all feature levels.

	// This function requires a shaderstage parameter is defined
	CTestCaseParameterBase *pParamBase = GetTestCaseParameter( m_shaderStageParameterStr );
	assert( pParamBase != NULL );

	// Local variables
	CUserValueSet< D3D_SHADER_STAGE > *m_pRequiredShaderStageValues = NULL;
	CUserValueSet< D3D_SHADER_STAGE > *m_pPossiblySkippedShaderStageValues = NULL;

	m_pRequiredShaderStageValues = new CUserValueSet< D3D_SHADER_STAGE >();
	m_pRequiredShaderStageValues->AddValue( D3D_SHADER_STAGE_VERTEX );	// These stages are common to FL 10.0, 10.1, and 11.0
	m_pRequiredShaderStageValues->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	m_pRequiredShaderStageValues->AddValue( D3D_SHADER_STAGE_PIXEL );

	m_pPossiblySkippedShaderStageValues = new CUserValueSet< D3D_SHADER_STAGE >();
	m_pPossiblySkippedShaderStageValues->AddValue( D3D_SHADER_STAGE_COMPUTE );	// Compute is FL 10.x+ iff the HW and Driver support it
	m_pPossiblySkippedShaderStageValues->AddValue( D3D_SHADER_STAGE_HULL );	// Hull and Domain are FL 11.0+
	m_pPossiblySkippedShaderStageValues->AddValue( D3D_SHADER_STAGE_DOMAIN );

	testfactor::RFactor requiredShaderStages = AddParameterValueSet( _T(m_shaderStageParameterStr), m_pRequiredShaderStageValues);
	testfactor::RFactor possiblySkippedShaderStages = AddParameterValueSet( _T(m_shaderStageParameterStr), m_pPossiblySkippedShaderStageValues);

	// To gurantee we run our "important" test cases on at least one stage we need to break it up into two groups: A required group, and possible skipped group.
	// The required group ensures the testcase will run regardless of feature level. The possibly skipped group may not run any tests if the hw doest support compute and running FL 10.x.
	testfactor::RFactor testcasesWithStages = (testcases % requiredShaderStages) + (testcases % possiblySkippedShaderStages);

	return testcasesWithStages;
}

///
// NOTE:	if something needs to be changed in SetupD3D for your test, it probably means
//			that the code should be moved to each test's Initialize function
///
TEST_RESULT CShader5xTest::SetupD3D()
{

	HRESULT hr;

	//
	// Setup the default viewport
	//
	m_viewPort.MinDepth = 0;
	m_viewPort.MaxDepth = 1;
	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;

	GetEffectiveContext()->RSSetViewports( 1, &m_viewPort );

	//
	//Set Rasterizer state
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled );
	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSDisabled );

	//
	//Set DepthStencil state
	//
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	GetDevice()->CreateDepthStencilState ( &dsDesc, &m_pDSDisabled );

	GetEffectiveContext()->OMSetDepthStencilState(m_pDSDisabled, 0);

	//
	// Create the constant buffer
	//
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = CONST_BUFFER_SIZE;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer );

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	//
	// Create the stream output buffer
	//
	D3D11_BUFFER_DESC bufferDesc =
	{
		m_soBufferSize,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_STREAM_OUTPUT,
		0,
		0
	};

	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pStreamOutputBuffer ) ) )
	{
		WriteToLog( _T("CDevice::CreateBuffer() Stream Output Buffer failed. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.MiscFlags = 0;

	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pStreamOutputBufferCopy ) ) )
	{
		WriteToLog( _T("CDevice::CreateBuffer() Stream Output Buffer copy failed. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}


	//
	// Create the shaders that are used often
	//
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	//Passthrough VS
	if( !AssembleShader( m_szPassThroughVS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling PassThroughVS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		return RESULT_FAIL;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPassThroughVS ) ) )
	{
		WriteToLog( _T( "CDevice::CreateVertexShader() failed creating PassThroughVS. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//Single Vertex VS
	if( !AssembleShader( m_szSingleVertexVS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling SingleVertexVS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		return RESULT_FAIL;
	}
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pSingleVertexVS ) ) )
	{
		WriteToLog( _T( "CDevice::CreateVertexShader() failed creating SingleVertexVS. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//Shader to clear stream output
	if( !AssembleShader( szClearStreamOutputVS, &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling ClearVS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		return RESULT_FAIL;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pClearStreamOutputVS ) ) )
	{
		WriteToLog( _T( "CDevice::CreateVertexShader() failed creating m_pClearStreamOutputVS. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//Assemble shader only for the output signature
	if( !AssembleShader( szNullGS, &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling NullGS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		return RESULT_FAIL;
	}

	const D3D11_SO_DECLARATION_ENTRY pDecl[] =
	{
		{ 0, "0_", 0, 0, 4, 0 },
	};

	const UINT pBufferStrides[] =
	{
		4 * 4 * sizeof( DWORD ),
		0,
		0,
		0
	};

	if( FAILED( hr = GetDevice()->CreateGeometryShaderWithStreamOutput( (DWORD*) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), pDecl, 1, pBufferStrides, 1, 0, NULL, &m_pNullGSStreamOutput ) ) )
	{
		WriteToLog( _T( "CDevice::CreateGeometryShaderWithStreamOutput() failed creating NullGS StreamOutput. HR = %s." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//
	// Set the default topology to point
	//
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	return SetupRenderTarget();
}

TEST_RESULT CShader5xTest::SetupRenderTarget()
{
	// Local variables
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
	const UINT rtFormatElementSizeInBytes = GetBitsPerElement( m_RTFormat ) / 8;

	//
	// Setup for the Render Texture2D.  Texture2D should only be used for pixel shader only code. Everything else should use the RTBuffer.
	//

	D3D11_TEXTURE2D_DESC tex2ddesc;
	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET;

	//
	//D3D11_BIND_UNORDERED_ACCESS for tex 2d is only supported in F11 or greater
	//
	bool bCreateTexture2DUAV = g_TestApp.IsFL11OrGreater();

	tex2ddesc.BindFlags |= bCreateTexture2DUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;
	tex2ddesc.Format = m_RTFormat;
	tex2ddesc.Height = (UINT)m_viewPort.Height;
	tex2ddesc.Width = (UINT)m_viewPort.Width;
	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTexture2D)))
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed CreateTexture2D (m_pRTTexture2D). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2ddesc.MiscFlags = 0;

	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTexture2DCopy )))
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed CreateTexture2D (m_pRTTexture2DCopy). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = m_RTFormat;
	rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, &rtviewdesc, &m_pRTView )))
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed CreateRenderTargetView (m_pRTView). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	if(bCreateTexture2DUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc;
		rtuaviewdesc.Texture2D.MipSlice = 0;
		rtuaviewdesc.Format = m_RTFormat;
		rtuaviewdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTTexture2D, &rtuaviewdesc, &m_pRTUAView )))
		{
			WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed CreateUnorderedAccessView (m_pRTUAView). HR = %s"), D3DHResultToString(hr) );
			return RESULT_FAIL;
		}
	}
	/////////////////////////////////
	// Setup for the Render Target Buffer

	//
	// Setup RenderTarget
	//

	// Setup the RTBuffer description
	bufferDesc.ByteWidth			= rtFormatElementSizeInBytes * (UINT)(m_viewPort.Height) * (UINT)(m_viewPort.Width);
	bufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
	bufferDesc.BindFlags			|= (g_TestApp.UAVIsAllowed()) ? D3D11_BIND_UNORDERED_ACCESS : 0;
	bufferDesc.CPUAccessFlags		= 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.MiscFlags			|= (g_TestApp.UAVIsAllowed()) ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
	bufferDesc.StructureByteStride	= 0;

	// Create the RTBuffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBuffer) ) )
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed to create RT buffer (m_pRTBuffer). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Setup the staging RTBuffer description
	bufferDesc.BindFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = 0;

	// Create the staging RTBuffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBufferCopy ) ) )
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed to create staging RT buffer (m_pRTBufferCopy). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Create the Render Target View (RTV)
	rtViewDesc.Format = m_RTFormat;
	rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
	rtViewDesc.Buffer.FirstElement = 0;
	rtViewDesc.Buffer.NumElements = bufferDesc.ByteWidth / rtFormatElementSizeInBytes;

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer, &rtViewDesc, &m_pRTBufferView ) ) )
	{
		WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed to create Render Target View (m_pRTBufferView). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Only create a UAV if the FL allows it
	if( g_TestApp.UAVIsAllowed() )
	{
		// Since the UAV is R32 Typeless, we need to account for the different number of elements
		const DXGI_FORMAT uavFormat = DXGI_FORMAT_R32_TYPELESS;
		const UINT uavFormatElementSizeInBytes = GetBitsPerElement( uavFormat ) / 8;
		assert( rtFormatElementSizeInBytes >= uavFormatElementSizeInBytes);
		const UINT elementDifference = rtFormatElementSizeInBytes / uavFormatElementSizeInBytes; // This accounts for the fact the 4x R32_Typeless = 1 R32B32G32A32_UINT etc...

		D3D11_UNORDERED_ACCESS_VIEW_DESC rtUAVDesc;
		rtUAVDesc.Format = uavFormat;
		rtUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		rtUAVDesc.Buffer.FirstElement = 0;
		rtUAVDesc.Buffer.NumElements = rtViewDesc.Buffer.NumElements * elementDifference;
		rtUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer, &rtUAVDesc, &m_pRTUABufferView ) ) )
		{
			WriteToLog( _T("CShader5xTest::SetupRenderTarget() - failed to create Unordered Access View (m_pRTUABufferView). HR = %s"), D3DHResultToString(hr) );
			return RESULT_FAIL;
		}
	}

	// Bind the RTV (Note we are binding to the RTTexture2D view. You must bind the RT Buffer view for each test that has been converted.
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	return RESULT_PASS;
}

TEST_RESULT CShader5xTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Determine if we need to skip the current testcase based on the test factors
	if( SkipBasedOnCommonFactors() )
	{
		tr = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tr;
}

void CShader5xTest::CleanupD3D()
{
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11Buffer* pBufferNull[] = { NULL };
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };

	if( GetDevice() )
	{
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 4, pRTNull, NULL );
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->HSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->DSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->CSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );
		GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}
	}

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pHS );
	SAFE_RELEASE( m_pDS );
	SAFE_RELEASE( m_pCS );

	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTTexture2DCopy );
	SAFE_RELEASE( m_pRTBuffer );
	SAFE_RELEASE( m_pRTBufferCopy );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pRTBufferView );
	SAFE_RELEASE( m_pRTUAView );
	SAFE_RELEASE( m_pRTUABufferView );
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pStreamOutputBuffer );
	SAFE_RELEASE( m_pStreamOutputBufferCopy );

	SAFE_RELEASE( m_pPassThroughVS );
	SAFE_RELEASE( m_pSingleVertexVS );
	SAFE_RELEASE( m_pClearStreamOutputVS );
	SAFE_RELEASE( m_pNullGSStreamOutput );

	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pRSDisabled );
	SAFE_RELEASE( m_pDSDisabled );
}

void CShader5xTest::ClearStreamOutputBuffer( unsigned int numVertices )
{
	GetEffectiveContext()->VSSetShader( m_pClearStreamOutputVS, NULL, 0 );
	GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );

	GetEffectiveContext()->GSSetShader( m_pNullGSStreamOutput, NULL, 0 );
	UINT offsets[] = { 0 };
	GetEffectiveContext()->SOSetTargets( 1, &m_pStreamOutputBuffer, offsets );

	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->RSSetState( m_pRSDisabled );

	GetEffectiveContext()->Draw( numVertices, 0 );
}

bool CShader5xTest::AssembleShader( const char *szSource, ID3D10Blob **ppShaderBuf, ID3D10Blob **ppErrorBuf,
								   UINT globalFlags )
{
	ShaderBlobMapType::iterator it;
	SHADER_MAP_KEY key;
	D3D_FEATURE_LEVEL featureLevel = GetFramework()->m_D3DOptions.FeatureLevel;     // For debugging convenience.
	bool bEnableDoubles = ( globalFlags & AssemblyHelper::GF_DOUBLE_SUPPORT ? true : false );
	bool bEnableRawAndStructured = ( globalFlags & AssemblyHelper::GF_RAW_AND_STRUCTURED_SUPPORT ? true : false );
	bool bEnableDoublePrecisionExtensions = ( globalFlags & AssemblyHelper::GF_DOUBLE_EXTENSIONS_SUPPORT ? true : false );
	bool bEnableShaderExtensions = ( globalFlags & AssemblyHelper::GF_SHADER_EXTENSIONS_SUPPORT ? true : false );
	bool bEnableUAVsAtEveryStage = ( (featureLevel >= D3D_FEATURE_LEVEL_11_1) ? true : false);

	// Replace shader version
	char *szNewShader = (char*)szSource;
	UINT uSize = strlen(szSource);
	if ( featureLevel >= D3D_FEATURE_LEVEL_10_1 && !g_bForceSM40 )
	{
		UINT size = uSize + 1;
		szNewShader = new char[size];
		strcpy_s( szNewShader, size, szSource );
		char *szPointer = NULL;
		szPointer = strstr( szNewShader, "s_4_\0" );

		if (szPointer)
		{
			if ( featureLevel >= D3D_FEATURE_LEVEL_11_1 )
			{
				szPointer[2] = '5';     // Shader Model 5.0 required
			}
			else
			{
				szPointer[4] = '1';     // >= D3D_FEATURE_LEVEL_10_1 - Shader Model 4.1 required
			}
		}
	}

	key.hash = HashString( (const unsigned char *) szNewShader );
	key.source = szNewShader;

	if( g_bDumpDissasm )
	{
		WriteToLog( szNewShader );
	}

	UINT AsmFlags = D3DX11_ASSEMBLER_ADD_FEATURE_INFO_BLOB;
	UINT64 FeatureBlobContents = D3DX11_ASSEMBLER_RETAIL_SIGNED_ASSEMBLED_SHADER;

	if ( bEnableDoubles )
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_DOUBLES;
	}

	if ( bEnableRawAndStructured )
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_COMPUTE_SHADERS_OR_RAW_AND_STRUCTURED_BUFFERS_VIA_SHADER_4_X;
	}

	if( bEnableUAVsAtEveryStage )
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_UAVS_AT_EVERY_STAGE | D3DX11_ASSEMBLER_SHADER_USES_64_UAVS;
	}

	if ( bEnableDoublePrecisionExtensions )
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_11_1_DOUBLE_EXTENSIONS;
	}

	if ( bEnableShaderExtensions )
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_11_1_SHADER_EXTENSIONS;
	}

	if (globalFlags & AssemblyHelper::GF_USES_TILED_RESOURCES)
	{
		FeatureBlobContents |= D3DX11_ASSEMBLER_SHADER_USES_TILED_RESOURCES;
	}

	it = m_ShaderBlobMap.find( key );
	if( it == m_ShaderBlobMap.end() )
	{
		HRESULT hr;

		D3DTEST_ASSEMBLER_ARGUMENTS args;

		ZeroMemory(&args, sizeof(args));

		args.pSrcString = szNewShader;
		args.uSrcStringSize = uSize;
		args.uShaderFlags = D3D10_SHADER_DEBUG;
		args.uAssemblerFlags = AsmFlags;
		args.qwFeatureInfoBlobContents = FeatureBlobContents;

		hr = D3DTestAssembleShader( &args );

		*ppShaderBuf = args.pShaderBlob;
		*ppErrorBuf = args.pErrorBlob;

		if( *ppErrorBuf )
			(*ppErrorBuf)->AddRef();

		if( FAILED( hr ) )
		{
			WriteToLog( "Shader failing to assemble - result code %s", D3DHResultToString( hr ).c_str() );
			WriteToLog( szNewShader );

			if (szNewShader != szSource)
				SAFE_DELETE_ARRAY( szNewShader );

			return false;
		}

		if( *ppShaderBuf )
		{
			m_ShaderBlobMap.insert( ShaderBlobMapType::value_type( key, *ppShaderBuf ) );
			(*ppShaderBuf)->AddRef();
		}
	}
	else
	{
		assert( key.source == tstring( szNewShader ) );
		*ppShaderBuf = it->second;
	}

	if( *ppShaderBuf )
		(*ppShaderBuf)->AddRef();

	if (szNewShader != szSource)
		SAFE_DELETE_ARRAY( szNewShader );

	return true;
}

bool CShader5xTest::MessageDumpShaderDisassembly( const UINT *pShaderBlob, SIZE_T shaderSize )
{
	LPCSTR szDisasm = NULL;

	LPD3D10BLOB pDisasm;
		
	if( FAILED( GetFramework()->LL_D3DDisassemble( pShaderBlob, shaderSize, false, NULL, &pDisasm ) ) )
	{
		WriteToLog( _T( "D3DDisassemble() failed" ) );
		GetFramework()->WriteMessage( _T( "D3DDisassemble() failed" ) );
		return false;
	}

	szDisasm = (LPCSTR) pDisasm->GetBufferPointer();

	GetFramework()->WriteMessage( _T( "" ) );
	GetFramework()->WriteMessage( szDisasm );
	GetFramework()->WriteMessage( _T( "" ) );

	return true;
}
