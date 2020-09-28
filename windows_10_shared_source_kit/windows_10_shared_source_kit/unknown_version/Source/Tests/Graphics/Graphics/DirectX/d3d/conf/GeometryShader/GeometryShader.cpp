// ***************************************************************
//  GSConf   version:  1.0   ·  date: 02/22/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "GeometryShader.h"
#include "GSPrimID.h"
#include "GSTopology.h"
#include "GSMultiOut.h"
#include "GSInstanceID.h"

#include "new_off.h"
#include "D3DX10.h"
#include "new_on.h"

CGSConfTestApp g_App;

static bool g_bDumpDissasm = false;

TEST_RESULT TRLatch(TEST_RESULT& origVal, TEST_RESULT& newVal)
{
	if (origVal == RESULT_FAIL || origVal == RESULT_ABORT)
		return origVal;
	return newVal;
}

CGSConfTest::CGSConfTest() :
	m_pInputLayout( NULL ),
	m_pVS( NULL ),
	m_pGS( NULL ),
	m_pPS( NULL ),
	m_pSOTarget( NULL ),
	m_pVB( NULL ),
	m_pCB( NULL ),
	m_pRS( NULL ),
	m_pDSS( NULL ),
	m_pMemTexture( NULL )
{
}

TEST_RESULT CGSConfTest::CommonSetup()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	GetDevice()->SetExceptionMode(0);
	
	//
	// Create memory buffer to copy results into
	//
	D3D11_TEXTURE2D_DESC memSurfaceDesc;
	memSurfaceDesc.BindFlags = 0;
	memSurfaceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	memSurfaceDesc.Usage = D3D11_USAGE_STAGING;
	memSurfaceDesc.Width = GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width;
	memSurfaceDesc.Height = GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height;
	memSurfaceDesc.ArraySize = 1;
	memSurfaceDesc.MipLevels = 1;
	memSurfaceDesc.Format = GetFramework()->m_RenderTargetOptions.SwapChainDesc.Format;
	memSurfaceDesc.MiscFlags = 0;
	memSurfaceDesc.SampleDesc.Count = 1;
	memSurfaceDesc.SampleDesc.Quality = 0;
	GetDevice()->CreateTexture2D( &memSurfaceDesc, NULL, &m_pMemTexture );
	
	
	//
	// Setup constant buffer
	//
	D3D11_BUFFER_DESC constBufferDesc;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.ByteWidth = sizeof( CONST_DATA );
	constBufferDesc.MiscFlags = 0;
	
	hr = GetDevice()->CreateBuffer( &constBufferDesc, NULL, &m_pCB );
	if( FAILED( hr ) )
	{
		WriteToLog( 1, _T( "CreateBuffer() unexpectedly failed creating constant buffer - result code %s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
		
	//
	// Setup rasterization state
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 1.0;
	rastDesc.DepthClipEnable = true;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.ScissorEnable = false;
	rastDesc.SlopeScaledDepthBias = 0;
	hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRS );
	if( FAILED( hr ) )
	{
		WriteToLog( 1, _T( "CreateRasterizerState() unexpectedly failed - result code %s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}
	
	//
	// Setup depth stencil state
	//
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	
	hr = GetDevice()->CreateDepthStencilState( &dsDesc, &m_pDSS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateDepthStencilState() unexpectedly failed - result code %s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return tr;
}

void CGSConfTest::CommonCleanup()
{
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pDSS );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pSOTarget );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pCB );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pDSS );
	SAFE_RELEASE( m_pMemTexture );
}


int CGSConfTest::GetNumRenderWindows() const
{
	return GetFramework()->GetShell()->IsConsole() ? 0 : 1;
}


TEST_RESULT CGSConfTest::SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_INPUT_ELEMENT_DESC *iedesc, UINT numInputElements, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout, const D3D10_SHADER_MACRO* pDefines )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	//
	// Compile shader source
	//
	const char* shaderProfile = g_App.GetShaderProfile(D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, pDefines, NULL, entryProc, shaderProfile, D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "Unexpected error/warning compiling shader '%s': %s" ), resourceID, pError->GetBufferPointer() );
	}
	
	if( pShader && g_bDumpDissasm )
	{
		ID3D10Blob *pDisasm;
		D3D10DisassembleShader( (CONST UINT *) pShader->GetBufferPointer(), pShader->GetBufferSize(), false, NULL, &pDisasm );
		LPCTSTR szDisasm = (LPCTSTR) pDisasm->GetBufferPointer();
		WriteToLog( _T("Shader disassembly:\nVS\n%s"), szDisasm );
	}
	
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );

		SAFE_RELEASE( pShader );
		SAFE_RELEASE( pError );

		tr = RESULT_FAIL;
		goto exit;
	}

	//
	// Create vertex shader object
	//
	hr = GetDevice()->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppVS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateVertexShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}
	
	if( ppInputLayout && iedesc )
	{
		//
		// Create an input layout to correspond with this shader
		//
		hr = GetDevice()->CreateInputLayout( iedesc, numInputElements, pShader->GetBufferPointer(), pShader->GetBufferSize(), ppInputLayout );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CreateInputLayout() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto exit;
		}
	}
	
exit:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSConfTest::SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11GeometryShader **ppGS, const D3D10_SHADER_MACRO* pDefines )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	//
	// Compile shader source
	//
	const char* shaderProfile = g_App.GetShaderProfile(D3D_SHADER_STAGE_GEOMETRY, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, pDefines, NULL, entryProc, shaderProfile, D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "Unexpected error/warning compiling shader '%s': %s" ), resourceID, pError->GetBufferPointer() );
	}

	if( pShader && g_bDumpDissasm )
	{
		ID3D10Blob *pDisasm;
		D3D10DisassembleShader( (CONST UINT *) pShader->GetBufferPointer(), pShader->GetBufferSize(), false, NULL, &pDisasm );
		LPCTSTR szDisasm = (LPCTSTR) pDisasm->GetBufferPointer();
		WriteToLog( _T("Shader disassembly:\nGS\n%s"), szDisasm );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	//
	// Create geometry shader object
	//
	hr = GetDevice()->CreateGeometryShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppGS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}
	
exit:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSConfTest::SetupGSWithSOFromResource(LPCTSTR resourceID, 
												   LPCSTR entryProc, 
												   D3D11_SO_DECLARATION_ENTRY decl[], 
												   UINT numElements, 
												   UINT outputStreamStride, 
												   ID3D11GeometryShader **ppGS,
												   const D3D10_SHADER_MACRO* pDefines )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	LPCSTR szProfile;
	
	// GS entry points all start with "GS", VS entry points start with "VS"
	const D3D_SHADER_STAGE shaderStage 
		= ( entryProc[0] == 'G' ) 
		? D3D_SHADER_STAGE_GEOMETRY
		: D3D_SHADER_STAGE_VERTEX;
	szProfile = g_App.GetShaderProfile(shaderStage, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
	
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, pDefines, NULL, entryProc, szProfile, D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );
	
	if( pError )
	{
		WriteToLog( _T( "Unexpected error/warning compiling shader '%s': %s" ), resourceID, pError->GetBufferPointer() );
	}

	if( pShader && g_bDumpDissasm )
	{
		ID3D10Blob *pDisasm;
		D3D10DisassembleShader( (CONST UINT *) pShader->GetBufferPointer(), pShader->GetBufferSize(), false, NULL, &pDisasm );
		LPCTSTR szDisasm = (LPCTSTR) pDisasm->GetBufferPointer();
		WriteToLog( _T("Shader disassembly:\nGS\n%s"), szDisasm );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	hr = GetDevice()->CreateGeometryShaderWithStreamOutput( pShader->GetBufferPointer(), pShader->GetBufferSize(), decl, numElements, &outputStreamStride, 1, 0, NULL, ppGS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShaderWithStreamOutput() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}
	
exit:
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pError );
	
	return tr;
};

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSConfTest::SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	const char* shaderProfile = g_App.GetShaderProfile(D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, NULL, NULL, entryProc, shaderProfile, D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "Unexpected error/warning compiling shader '%s': %s" ), resourceID, pError->GetBufferPointer() );
	}

	if( pShader && g_bDumpDissasm )
	{
		ID3D10Blob *pDisasm;
		D3D10DisassembleShader( (CONST UINT *) pShader->GetBufferPointer(), pShader->GetBufferSize(), false, NULL, &pDisasm );
		LPCTSTR szDisasm = (LPCTSTR) pDisasm->GetBufferPointer();
		WriteToLog( _T("Shader disassembly:\nPS\n%s"), szDisasm );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

	hr = GetDevice()->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppPS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreatePixelShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto exit;
	}

exit:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );

	return tr;
}

/*

		VSIN dmmy;
		D3D11_INPUT_ELEMENT_DESC iedesc[] =
		{
			{ "pos", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MEMBER_OFFSET( pos, dmmy ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "data", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET( data, dmmy ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		
*/





//////////////////////////////////////////////////////////////////////////
// CGSConfTestApp
//////////////////////////////////////////////////////////////////////////

void CGSConfTestApp::InitOptionVariables()
{
	// Single device conformance test.
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable( "SrcOnly" );
	m_D3DOptions.SrcOnly = true;

	int n = AddOptionCategory( _T( "Debug" ) );  
	RegisterOptionVariable( _T( "DumpAsm" ), _T( "Allows shader disassembly dumps to the log." ), &g_bDumpDissasm, false, 0, n );
}

bool CGSConfTestApp::InitTestGroups()
{
	if( !ADD_TEST( "MultiOut", CGSMultiOutTest ) ) return false;
	if( !ADD_TEST( "PrimID", CGSPrimIDTest ) ) return false;

	if( m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		if( !ADD_TEST( "InstanceID", CGSInstanceIDTest ) ) return false;
	}

	if( !ADD_TEST( "Topology", CGSTopologyTest ) ) return false;

	return true;
}
