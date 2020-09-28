//////////////////////////////////////////////////////////////////////////
//  ControlFlow.cpp
//  created:	2005/04/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "controlflow.h"

//TODO: Call* with more than 1 labels

UINT32 TrueValues[] =
{
	0x00000000,
	0x00000001,
	0x00000002,
	0x00000008,
	0x00080000,
	0x00080001,
	0x80000000,
	0x80000001,
	0xFFFFFFFF,
};

BEGIN_NAMED_VALUES( SwitchCaseBreak )
NAMED_VALUE( _T( "None" ), Switch_Case_None )
NAMED_VALUE( _T( "Break" ), Switch_Case_Break )
NAMED_VALUE( _T( "Ret" ), Switch_Case_Ret )
NAMED_VALUE( _T( "BreakC" ), Switch_Case_BreakC )
NAMED_VALUE( _T( "RetC" ), Switch_Case_RetC )
NAMED_VALUE( _T( "BreakC Break" ), Switch_Case_BreakC_Break )
NAMED_VALUE( _T( "BreakC BreakC" ), Switch_Case_BreakC_BreakC )
NAMED_VALUE( _T( "BreakC Ret" ), Switch_Case_BreakC_Ret )
NAMED_VALUE( _T( "BreakC RetC" ), Switch_Case_BreakC_RetC )
NAMED_VALUE( _T( "RetC Break" ), Switch_Case_RetC_Break )
NAMED_VALUE( _T( "RetC BreakC" ), Switch_Case_RetC_BreakC )
NAMED_VALUE( _T( "RetC Ret" ), Switch_Case_RetC_Ret )
NAMED_VALUE( _T( "RetC RetC" ), Switch_Case_RetC_RetC )
END_NAMED_VALUES( SwitchCaseBreak )

BEGIN_NAMED_VALUES( SwitchCaseDefault )
NAMED_VALUE( _T( "None" ), Switch_Case_No_Default )
NAMED_VALUE( _T( "Begin" ), Switch_Case_Default_Begin )
NAMED_VALUE( _T( "Middle" ), Switch_Case_Default_Middle )
NAMED_VALUE( _T( "End" ), Switch_Case_Default_End )
END_NAMED_VALUES( SwitchCaseDefault )

void WriteOut(char* szFileName, char *szSourceBuffer)
{
	return;
	FILE * outfile;
	outfile=fopen(szFileName, "a");
	if(outfile)
	{
		fprintf(outfile, "---------------------------\n%s", szSourceBuffer);
		fclose(outfile);
	}
}

tstring __stdcall ToString( ControlFlowConditionalInstruction type )
{
	switch( type )
	{
	case CFC_if_z:
		return tstring( _T( "if_z" ) );
		break;

	case CFC_if_nz:
		return tstring( _T( "if_nz" ) );
		break;

	case CFC_continuec_z:
		return tstring( _T( "continuec_z" ) );
		break;

	case CFC_continuec_nz:
		return tstring( _T( "continuec_nz" ) );
		break;

	case CFC_breakc_z:
		return tstring( _T( "breakc_z" ) );
		break;

	case CFC_breakc_nz:
		return tstring( _T( "breakc_nz" ) );
		break;

	case CFC_retc_z:
		return tstring( _T( "retc_z" ) );
		break;

	case CFC_retc_nz:
		return tstring( _T( "retc_nz" ) );
		break;

	case CFC_callc_z:
		return tstring( _T( "callc_z" ) );
		break;

	case CFC_callc_nz:
		return tstring( _T( "callc_nz" ) );
		break;

	case CFC_discard_z:
		return tstring( _T( "discard_z" ) );
		break;

	case CFC_discard_nz:
		return tstring( _T( "discard_nz" ) );
		break;


	default:
		return tstring( _T( "" ) );
		break;
	}
};

tstring __stdcall ToString( ControlFlowConditionalInstructionNC type )
{
	switch( type )
	{
	case CFNC_if_z_R:
	case CFNC_if_z_RG:
	case CFNC_if_z_RGB:
	case CFNC_if_z_RGBA:
	case CFNC_if_z_RB:
	case CFNC_if_z_GA:
	case CFNC_if_z_G:
	case CFNC_if_z_B:
	case CFNC_if_z_A:
		return tstring( _T( "if_z" ) );
		break;
	case CFNC_loop_R:
		return tstring (_T( "loop"));
		break;

	default:
		return tstring( _T( "" ) );
		break;
	}
};

bool __stdcall FromString( ControlFlowConditionalInstructionNC *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "if_z" ) )
	{
		*pType = CFNC_if_z_R;
		return true;
	}
	return false;
}



tstring __stdcall ToString( ControlFlowInstruction type )
{
	switch( type )
	{
	case CF_if_z:
		return tstring( _T( "if_z" ) );
		break;

	case CF_if_nz:
		return tstring( _T( "if_nz" ) );
		break;

	case CF_continue:
		return tstring( _T( "continue" ) );
		break;

	case CF_continuec_z:
		return tstring( _T( "continuec_z" ) );
		break;

	case CF_continuec_nz:
		return tstring( _T( "continuec_nz" ) );
		break;

	case CF_break:
		return tstring( _T( "break" ) );
		break;

	case CF_breakc_z:
		return tstring( _T( "breakc_z" ) );
		break;

	case CF_breakc_nz:
		return tstring( _T( "breakc_nz" ) );
		break;

	case CF_else:
		return tstring( _T( "else" ) );
		break;

	case CF_ret:
		return tstring( _T( "ret" ) );
		break;

	case CF_retc_z:
		return tstring( _T( "retc_z" ) );
		break;

	case CF_retc_nz:
		return tstring( _T( "retc_nz" ) );
		break;

	case CF_call:
		return tstring( _T( "call" ) );
		break;

	case CF_callc_z:
		return tstring( _T( "callc_z" ) );
		break;

	case CF_callc_nz:
		return tstring( _T( "callc_nz" ) );
		break;

	case CF_discard_z:
		return tstring( _T( "discard_z" ) );
		break;

	case CF_discard_nz:
		return tstring( _T( "discard_nz" ) );
		break;

	case CF_switch:
		return tstring( _T( "switch" ) );
		break;

	case CF_loop:
		return tstring( _T( "loop" ) );
		break;

	default:
		return tstring( _T( "" ) );
		break;
	}
};

bool __stdcall FromString( ControlFlowConditionalInstruction *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "if_z" ) )
	{
		*pType = CFC_if_z;
		return true;
	}

	if( tcistring( str ) == _T( "if_nz" ) )
	{
		*pType = CFC_if_nz;
		return true;
	}

	if( tcistring( str ) == _T( "continuec_z" ) )
	{
		*pType = CFC_continuec_z;
		return true;
	}

	if( tcistring( str ) == _T( "continuec_nz" ) )
	{
		*pType = CFC_continuec_nz;
		return true;
	}

	if( tcistring( str ) == _T( "breakc_z" ) )
	{
		*pType = CFC_breakc_z;
		return true;
	}

	if( tcistring( str ) == _T( "breakc_nz" ) )
	{
		*pType = CFC_breakc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "retc_z" ) )
	{
		*pType = CFC_retc_z;
		return true;
	}

	if( tcistring( str ) == _T( "retc_nz" ) )
	{
		*pType = CFC_retc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "callc_z" ) )
	{
		*pType = CFC_callc_z;
		return true;
	}

	if( tcistring( str ) == _T( "callc_nz" ) )
	{
		*pType = CFC_callc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "discard_z" ) )
	{
		*pType = CFC_discard_z;
		return true;
	}

	if( tcistring( str ) == _T( "discard_nz" ) )
	{
		*pType = CFC_discard_nz;
		return true;
	}


	return false;
}

bool __stdcall FromString( ControlFlowInstruction *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "if_z" ) )
	{
		*pType = CF_if_z;
		return true;
	}

	if( tcistring( str ) == _T( "if_nz" ) )
	{
		*pType = CF_if_nz;
		return true;
	}

	if( tcistring( str ) == _T( "continue" ) )
	{
		*pType = CF_continue;
		return true;
	}

	if( tcistring( str ) == _T( "continuec_z" ) )
	{
		*pType = CF_continuec_z;
		return true;
	}

	if( tcistring( str ) == _T( "continuec_nz" ) )
	{
		*pType = CF_continuec_nz;
		return true;
	}

	if( tcistring( str ) == _T( "break" ) )
	{
		*pType = CF_break;
		return true;
	}

	if( tcistring( str ) == _T( "breakc_z" ) )
	{
		*pType = CF_breakc_z;
		return true;
	}

	if( tcistring( str ) == _T( "breakc_nz" ) )
	{
		*pType = CF_breakc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "retc_z" ) )
	{
		*pType = CF_retc_z;
		return true;
	}

	if( tcistring( str ) == _T( "else" ) )
	{
		*pType = CF_else;
		return true;
	}

	if( tcistring( str ) == _T( "ret" ) )
	{
		*pType = CF_ret;
		return true;
	}

	if( tcistring( str ) == _T( "retc_nz" ) )
	{
		*pType = CF_retc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "call" ) )
	{
		*pType = CF_call;
		return true;
	}

	if( tcistring( str ) == _T( "callc_z" ) )
	{
		*pType = CF_callc_z;
		return true;
	}

	if( tcistring( str ) == _T( "callc_nz" ) )
	{
		*pType = CF_callc_nz;
		return true;
	}

	if( tcistring( str ) == _T( "discard_z" ) )
	{
		*pType = CF_discard_z;
		return true;
	}

	if( tcistring( str ) == _T( "discard_nz" ) )
	{
		*pType = CF_discard_nz;
		return true;
	}

	if( tcistring( str ) == _T( "switch" ) )
	{
		*pType = CF_switch;
		return true;
	}

	if( tcistring( str ) == _T( "loop" ) )
	{
		*pType = CF_loop;
		return true;
	}


	return false;
}

//-------------------------------------------------------------------------------
void CShader5xTest_ControlFlow::CleanupTestCase()
{
	ID3D11Buffer *pBufferNull[] = { NULL, NULL, NULL, NULL };
	UINT strides[] = { 0, 0, 0, 0 };
	UINT offsets[] = { 0, 0, 0, 0 };

	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->IASetVertexBuffers( 0, 4, pBufferNull, strides, offsets );
	GetEffectiveContext()->IASetInputLayout( NULL );

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
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pLayout);
}

//-------------------------------------------------------------------------------
void CShader5xTest_ControlFlow::Cleanup()
{
	SAFE_RELEASE(m_pRTBufferViewFloat);

	// Call the base Cleanup
	CShader5xTest::Cleanup();
}

//-------------------------------------------------------------------------------
//A triangle for vertex buffer creation
float TRIANGLE_VERTICES[] =
{
	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 1.0f,
};

//-------------------------------------------------------------------------------
//Four vertices for vertex shader tests
float FOUR_VERTICES[] =
{
	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 1.0f,
};

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlow::SetPipeline(AssemblyHelper& asmHelper, 
												   const char *szInstruction, 
												   bool bAppendFinishShader)
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	tstring instructionCode = szInstruction;

	// register allocation (marked with * for ones that actual shader code should care about)
	// v[0]: position/vertexID (sometimes omitted if unnecessary)
	// r0.x: unique invocation index
	//*r6  : temporary output before writing to o[0]
	//*r7  : CB value for current invocation, taken from invocation index
	//*o[0]: result value
	// o[1]: position, to drive PS (sometimes omitted if unnecessary)

	ClearStreamOutputBuffer( 4 );

	// output result in o[0] as uint
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);

	asmHelper.SetSuppressEmit(true);

	const bool bNeedsGeometryInput = m_bDrawTriangle || m_bUseFourVertices;

	if (bNeedsGeometryInput)
	{
		tstring uniqueIndexCode;
		// input position in v[0] from input layout, output in o[1] to feed PS
		asmHelper.SetPositionInputRegIdx(0);
		asmHelper.SetPositionOutputRegIdx(1);
		// write position from v[0] to o[1]
		asmHelper.SetupMovToOutputFromInput(1, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
		// we also need a unique index for each invocation, put it in r0.x
		if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		{
			// index = position.x + 2 * position.y (in screen space [0, viewport])
			uniqueIndexCode += asmHelper.GetScreenPositionMovStr(m_ShaderStage, 0, 0);
			uniqueIndexCode += "round_ni r0.xyzw, r0.xyzw"   "\n";
			uniqueIndexCode += "mul r0.y, r0.y, l(2.000000)" "\n";
			uniqueIndexCode += "add r0.x, r0.x, r0.y"        "\n";
			uniqueIndexCode += "ftoi r0.x, r0.x"             "\n";
		}
		else
		{
			// index = vertexID
			asmHelper.SetSysvalInputRegMask(1, "vertexID", AssemblyHelper::x);
			asmHelper.SetupMovToTempFromInput(0, AssemblyHelper::x, 1, AssemblyHelper::swizX);
		}
		// prepend mov before instruction code
		uniqueIndexCode += "mov r7, c0[r0.x]\n";
		instructionCode = uniqueIndexCode + instructionCode;
	}
	else
	{
		// no position input, we will auto-generate to feed PS
		// use a constant index for only one invocation
		asmHelper.SetupMovToTempFromCB(7, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	}
	// initialize output with zeroes
	asmHelper.SetupMovToTempFromLiteral(6, AssemblyHelper::xyzw, "0x00");

	// Only initialize the output with zero if we are not in the compute shader (UAV has already been cleared)
	if( D3D_SHADER_STAGE_COMPUTE != m_ShaderStage )
	{
		instructionCode = asmHelper.GetOutputMovStr(m_ShaderStage, 0, AssemblyHelper::xyzw) + ", vec4(0,0,0,0) \n" + instructionCode;
	}

	if (bAppendFinishShader)
	{
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
			instructionCode += "emit \n" "ret \n";
		else
			instructionCode += "ret \n";
	}

	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		{"0_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position			
	};
	HRESULT hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode, &m_pRTBufferView, &m_pRTUABufferView, 1, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS,
		DeclTL, ARRAYSIZE(DeclTL), bNeedsGeometryInput ? &m_pLayout : NULL );

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		TEST_RESULT tRes = RESULT_FAIL;
		goto FAIL;
	}

	UINT offsets[] = { 0 };
	GetEffectiveContext()->SOSetTargets( 1, &m_pStreamOutputBuffer, offsets );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	if (bNeedsGeometryInput)
	{
		GetEffectiveContext()->IASetInputLayout( m_pLayout );
		//Create and set vertex buffer
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		if (m_ShaderStage == D3D_SHADER_STAGE_VERTEX && m_bUseFourVertices)
		{
			bd.ByteWidth = sizeof( float ) * 4 * 4;
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = FOUR_VERTICES;
			if( FAILED( GetDevice()->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
			{
				WriteToLog( _T( "CDevice::CreateBuffer() failed creating PS" ) );
				goto FAIL;
			}

			//Set vertex buffer
			UINT stride = sizeof( float ) * 4;
			UINT offset = 0;
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

			//// Set primitive topology
			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		}
		if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL && m_bDrawTriangle)
		{
			bd.ByteWidth = sizeof( float ) * 3 * 4;
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = TRIANGLE_VERTICES;
			if( FAILED( GetDevice()->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
			{
				WriteToLog( _T( "CDevice::CreateBuffer() failed creating PS" ) );
				goto FAIL;
			}

			//Set vertex buffer
			UINT stride = sizeof( float ) * 4;
			UINT offset = 0;
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

			//// Set primitive topology
			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		}
	}

	if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		//First Clear RT
		DWORD dwColor = CLEAR_VALUE;
		float color = *( (float*) &dwColor );
		FLOAT colors[4] = { color, color, color, color };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferViewFloat, colors );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		goto FAIL;
	}

	goto PASS;

FAIL:
	tr = RESULT_FAIL;
PASS:
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	
	return tr;
}


//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlow::SetupRenderTarget()
{
	// Local variables
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
	m_RTFormat = DXGI_FORMAT_R32G32B32A32_UINT;
	const UINT rtFormatElementSizeInBytes = GetBitsPerElement( m_RTFormat ) / 8;

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
		WriteToLog( _T("CShader5xTest_ControlFlow::SetupRenderTarget() - failed to create RT buffer (m_pRTBuffer). HR = %s"), D3DHResultToString(hr) );
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
		WriteToLog( _T("CShader5xTest_ControlFlow::SetupRenderTarget() - failed to create staging RT buffer (m_pRTBufferCopy). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Create the Render Target View (RTV)
	rtViewDesc.Format = m_RTFormat;
	rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
	rtViewDesc.Buffer.FirstElement = 0;
	rtViewDesc.Buffer.NumElements = bufferDesc.ByteWidth / rtFormatElementSizeInBytes;

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer, &rtViewDesc, &m_pRTBufferView ) ) )
	{
		WriteToLog( _T("CShader5xTest_ControlFlow::SetupRenderTarget() - failed to create Render Target View (m_pRTBufferView). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Now create the float view, used to clear the RT.
	rtViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer, &rtViewDesc, &m_pRTBufferViewFloat ) ) )
	{
		WriteToLog( _T("CShader5xTest_ControlFlow::SetupRenderTarget() - failed to create Render Target View (m_pRTBufferViewFloat). HR = %s"), D3DHResultToString(hr) );
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
			WriteToLog( _T("CShader5xTest_ControlFlow::SetupRenderTarget() - failed to create Unordered Access View (m_pRTUABufferView). HR = %s"), D3DHResultToString(hr) );
			return RESULT_FAIL;
		}
	}

	// Bind the RTV
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTBufferView, NULL );

	return RESULT_PASS;
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlow::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	if(	   (m_ShaderStage == D3D_SHADER_STAGE_VERTEX)
		|| (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		|| (m_ShaderStage == D3D_SHADER_STAGE_HULL)
		|| (m_ShaderStage == D3D_SHADER_STAGE_DOMAIN)
		)
	{
		if(m_bUseFourVertices)
		{
			GetEffectiveContext()->Draw( 4, 0 );
		}
		else
		{
			GetEffectiveContext()->Draw( 1, 0 );
		}
	}
	else if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		if (m_bUseFourVertices || m_bDrawTriangle)
		{
			GetEffectiveContext()->Dispatch(4,1,1);
		}
		else
		{
			GetEffectiveContext()->Dispatch(1,1,1);
		}
	}
	else
	{
		if(m_bDrawTriangle)
		{
			GetEffectiveContext()->Draw( 3, 0 );
		}
		else
		{
			GetEffectiveContext()->Draw( 1, 0 );
		}
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	DWORD result;
	HRESULT hr;
	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ControlFlow::ExecuteTestCase() - Failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );

		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pStreamOutputBufferCopy, result ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ControlFlow::ExecuteTestCase() - Failed to map m_pStreamOutputBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	UINT expectedResult = CalcResult();
	if ( expectedResult != result )
	{
		WriteToLog( 0, "Expected 0x%x : result 0x%x.", expectedResult, result );
		tr = RESULT_FAIL;
	}

	return tr;
}

//////////////////////////////////////////////////////////////////////////
// Control Flow Conditional Instructions : True Values
//////////////////////////////////////////////////////////////////////////

void CShader5xTest_ControlFlowTrueValues::InitTestParameters()
{
	CUserValueSet< ControlFlowConditionalInstruction > *pInstructionValues = new CUserValueSet< ControlFlowConditionalInstruction >();
	//TODO : Add movc
	pInstructionValues->AddValue( CFC_if_z );
	pInstructionValues->AddValue( CFC_if_nz );
	pInstructionValues->AddValue( CFC_continuec_z );
	pInstructionValues->AddValue( CFC_continuec_nz );
	pInstructionValues->AddValue( CFC_breakc_z );
	pInstructionValues->AddValue( CFC_breakc_nz );
	pInstructionValues->AddValue( CFC_retc_z );
	pInstructionValues->AddValue( CFC_retc_nz );
	pInstructionValues->AddValue( CFC_callc_z );
	pInstructionValues->AddValue( CFC_callc_nz );

	CUserValueSet< ControlFlowConditionalInstruction > *pPSInstructionValues = new CUserValueSet< ControlFlowConditionalInstruction >();
	pPSInstructionValues->AddValue( CFC_discard_z );
	pPSInstructionValues->AddValue( CFC_discard_nz );

	testfactor::RFactor src = AddParameter( "Src", &m_Src, new CTableValueSet< UINT32 >( &TrueValues[0], sizeof( TrueValues[0] ), sizeof( TrueValues ) / sizeof( TrueValues[0] ) ) );

	CTestCaseParameter< ControlFlowConditionalInstruction > *pInstruction = AddParameter< ControlFlowConditionalInstruction >( _T("Instruction"), &m_Instruction );
	testfactor::RFactor instruction = AddParameterValueSet< ControlFlowConditionalInstruction >( pInstruction, pInstructionValues );
	testfactor::RFactor instructionPS = AddParameterValueSet< ControlFlowConditionalInstruction >( pInstruction, pPSInstructionValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor pixelShaderStage = AddParameterValue( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases_NonPS = src * instruction;
	testfactor::RFactor rfTestCasesWithShaderStages_NonPS = GetTestFactorWithDefaultStageCoverage( rfTestCases_NonPS  );
	testfactor::RFactor rfPSTest = src * instructionPS * pixelShaderStage;

	SetRootTestFactor( rfTestCasesWithShaderStages_NonPS + rfPSTest );
}


//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlowTrueValues::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	if (m_Instruction == CFC_discard_z || m_Instruction == CFC_discard_nz)
	{
		if (m_ShaderStage != D3D_SHADER_STAGE_PIXEL)
		{
			return RESULT_SKIP;
		}
	}

	tstring outputMovStr;
	tstring szInstruction;

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	switch (m_Instruction)
	{
	case CFC_if_z :
		szInstruction = outputMovStr + ", 0xca1cca1c		\n"
			"if_z r7.x					\n"
			"    " + outputMovStr + ", 0xFFFFFFFF	\n"
			"endif						\n";
		break;

	case CFC_if_nz :
		szInstruction = outputMovStr + ", 0xca1cca1c		\n"
			"if_nz r7.x					\n"
			"    " + outputMovStr + ", 0xFFFFFFFF	\n"
			"endif						\n";
		break;

	case CFC_continuec_z :
		szInstruction =		
			"mov r0.x, 1				\n"
			"loop						\n"
			"    if_z r0.x				\n"
			"        break				\n"
			"    endif					\n"
			"    iadd r0.x, r0.x, -1	\n"
			"    " + outputMovStr + ", 0xFFFFFFFF \n"
			"    continuec_z r7.x		\n"
			"    " + outputMovStr + ", 0xca1cca1c \n"
			"endloop					\n";
		break;

	case CFC_continuec_nz :
		szInstruction =		"mov r0.x, 1				\n"
			"loop						\n"
			"    if_z r0.x				\n"
			"        break				\n"
			"    endif					\n"
			"    iadd r0.x, r0.x, -1	\n"
			"    " + outputMovStr + ", 0xFFFFFFFF \n"
			"    continuec_nz r7.x		\n"
			"    " + outputMovStr + ", 0xca1cca1c \n"
			"endloop					\n";
		break;

	case CFC_breakc_z :
		szInstruction =		"loop						\n"
			"    " + outputMovStr + ", 0xFFFFFFFF \n"
			"    breakc_z r7.x			\n"
			"    " + outputMovStr + ", 0xca1cca1c \n"
			"    break					\n"
			"endloop					\n";
		break;


	case CFC_breakc_nz :
		szInstruction =		"loop						\n"
			"    " + outputMovStr + ", 0xFFFFFFFF \n"
			"    breakc_nz r7.x			\n"
			"    " + outputMovStr + ", 0xca1cca1c \n"
			"    break					\n"
			"endloop					\n";
		break;

	case CFC_retc_z :
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
		{
			szInstruction =		"" + outputMovStr + ", 0xFFFFFFFF		\n"
				"if_z r7.x					\n"
				"    emit					\n"
				"endif						\n"
				"retc_z r7.x				\n"
				"" + outputMovStr + ", 0xca1cca1c		\n";
		}
		else
		{
			szInstruction =		"" + outputMovStr + ", 0xFFFFFFFF		\n"
				"retc_z r7.x				\n"
				"" + outputMovStr + ", 0xca1cca1c		\n";
		}
		break;

	case CFC_retc_nz :
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
		{
			szInstruction =		"" + outputMovStr + ", 0xFFFFFFFF		\n"
				"if_nz r7.x					\n"
				"    emit					\n"
				"endif						\n"
				"retc_nz r7.x				\n"
				"" + outputMovStr + ", 0xca1cca1c		\n";
		}
		else
		{
			szInstruction =		"" + outputMovStr + ", 0xFFFFFFFF		\n"
				"retc_nz r7.x				\n"
				"" + outputMovStr + ", 0xca1cca1c		\n";
		}
		break;

	case CFC_callc_z :
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
		{
			szInstruction =		"" + outputMovStr + ", 0xca1cca1c		\n"
				"callc_z r7.x, l0			\n"
				"emit						\n"
				"ret						\n"
				"label l0					\n"
				"" + outputMovStr + ", 0xFFFFFFFF		\n";
		}
		else
		{
			szInstruction =		"" + outputMovStr + ", 0xca1cca1c		\n"
				"callc_z r7.x, l0			\n"
				"ret						\n"
				"label l0					\n"
				"" + outputMovStr + ", 0xFFFFFFFF		\n";
		}
		break;

	case CFC_callc_nz :
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY )
		{
			szInstruction =		"" + outputMovStr + ", 0xca1cca1c		\n"
				"callc_nz r7.x, l0			\n"
				"emit						\n"
				"ret						\n"
				"label l0					\n"
				"" + outputMovStr + ", 0xFFFFFFFF		\n";
		}
		else
		{
			szInstruction =		"" + outputMovStr + ", 0xca1cca1c		\n"
				"callc_nz r7.x, l0			\n"
				"ret						\n"
				"label l0					\n"
				"" + outputMovStr + ", 0xFFFFFFFF		\n";
		}
		break;

	case CFC_discard_z :
		szInstruction =		"" + outputMovStr + ", 0xFFFF0000		\n"
			"discard_z r7.x				\n"
			"" + outputMovStr + ", 0x0000FFFF		\n";
		break;

	case CFC_discard_nz :
		szInstruction =		"" + outputMovStr + ", 0xFFFF0000		\n"
			"discard_nz r7.x			\n"
			"" + outputMovStr + ", 0x0000FFFF		\n";
		break;
	}

	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0] = m_Src;
	pData[4] = !m_Src;
	pData[8] = !m_Src;
	pData[12] = m_Src;

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_ControlFlowTrueValues::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline( asmHelper, szInstruction.c_str());
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_ControlFlowTrueValues::CalcResult()
{
	UINT expectedResult = 0;

	switch (m_Instruction)
	{
	case CFC_if_z :
	case CFC_continuec_z :
	case CFC_breakc_z :
	case CFC_retc_z :
	case CFC_callc_z :
		if (m_Src == 0)
			expectedResult = 0xFFFFFFFF;
		else 
			expectedResult = 0xca1cca1c;
		break;

	case CFC_if_nz :
	case CFC_continuec_nz :
	case CFC_breakc_nz :
	case CFC_retc_nz :
	case CFC_callc_nz :
		if (m_Src != 0)
			expectedResult = 0xFFFFFFFF;
		else 
			expectedResult = 0xca1cca1c;
		break;

	case CFC_discard_z :
		if (m_Src == 0)
			expectedResult = CLEAR_VALUE;
		else 
			expectedResult = 0x0000FFFF;
		break;

	case CFC_discard_nz :
		if (m_Src != 0)
			expectedResult = CLEAR_VALUE;
		else 
			expectedResult = 0x0000FFFF;
		break;
	}
	return expectedResult;
}

//////////////////////////////////////////////////////////////////////////
// Control Flow Conditional Instructions : Nested control flow instructions
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_NestedControlFlow::InitTestParameters()
{
	CTestCaseParameter< ControlFlowInstruction > *pOddInstruction = AddParameter< ControlFlowInstruction >( _T("OddInstruction"), &m_OddInstruction);
	CTestCaseParameter< ControlFlowInstruction > *pEvenInstruction = AddParameter< ControlFlowInstruction >( _T("EvenInstruction"), &m_EvenInstruction);
	CTestCaseParameter< UINT > *pNumNest = AddParameter< UINT >( _T("NumNest"), &m_NumNest);
	CTestCaseParameter< UINT > *pNumSubRoutine = AddParameter< UINT >( _T("NumSubRoutine"), &m_NumSubRoutine);

	CUserValueSet< ControlFlowInstruction > *pOddInstructionNoCallValues = new CUserValueSet< ControlFlowInstruction >();
	pOddInstructionNoCallValues->AddValue( CF_if_z );
	pOddInstructionNoCallValues->AddValue( CF_switch );
	pOddInstructionNoCallValues->AddValue( CF_loop );

	CUserValueSet< ControlFlowInstruction > *pEvenInstructionNoCallValues = new CUserValueSet< ControlFlowInstruction >();
	pEvenInstructionNoCallValues->AddValue( CF_if_nz );
	pEvenInstructionNoCallValues->AddValue( CF_switch );
	pEvenInstructionNoCallValues->AddValue( CF_loop );

	CUserValueSet< ControlFlowInstruction > *pLimitedOddInstructionNoCallValues = new CUserValueSet< ControlFlowInstruction >();
	pLimitedOddInstructionNoCallValues->AddValue( CF_if_z );
	pLimitedOddInstructionNoCallValues->AddValue( CF_loop );

	CUserValueSet< ControlFlowInstruction > *pLimitedEvenInstructionNoCallValues = new CUserValueSet< ControlFlowInstruction >();
	pLimitedEvenInstructionNoCallValues->AddValue( CF_if_nz );
	pLimitedEvenInstructionNoCallValues->AddValue( CF_switch );

	CUserValueSet< UINT > *pNumNestValues = new CUserValueSet< UINT >();
	pNumNestValues->AddValue( 32 );
	pNumNestValues->AddValue( 33 );
	pNumNestValues->AddValue( 64 );

	CUserValueSet< UINT > *pBigNumNestValues = new CUserValueSet< UINT >();
	pBigNumNestValues->AddValue( 64 );

	CUserValueSet< UINT > *pNumSubRoutineValues = new CUserValueSet< UINT >();
	pNumSubRoutineValues->AddValue( 0 );
	pNumSubRoutineValues->AddValue( 1 );
	pNumSubRoutineValues->AddValue( 32 );

	CUserValueSet< UINT > *pBigNumSubRoutineValues = new CUserValueSet< UINT >();
	pBigNumSubRoutineValues->AddValue( 33 );

	//Discard can't be nested
	//CUserValueSet< ControlFlowInstruction > *pPSInstructionValues = new CUserValueSet< ControlFlowInstruction >();
	//pInstructionValues->AddValue( CF_discard_z );
	//pInstructionValues->AddValue( CF_discard_nz );

	testfactor::RFactor rOddInstruction = AddParameterValueSet< ControlFlowInstruction >( pOddInstruction, pOddInstructionNoCallValues );
	testfactor::RFactor rEvenInstruction = AddParameterValueSet< ControlFlowInstruction >( pEvenInstruction, pEvenInstructionNoCallValues );
	testfactor::RFactor rLimitedOddInstruction = AddParameterValueSet< ControlFlowInstruction >( pOddInstruction, pLimitedOddInstructionNoCallValues );
	testfactor::RFactor rLimitedEvenInstruction = AddParameterValueSet< ControlFlowInstruction >( pEvenInstruction, pLimitedEvenInstructionNoCallValues );

	testfactor::RFactor rNumNest = AddParameterValueSet< UINT >( pNumNest, pNumNestValues );
	testfactor::RFactor rNumSubRoutine = AddParameterValueSet< UINT >( pNumSubRoutine, pNumSubRoutineValues );
	testfactor::RFactor rBigNumNest = AddParameterValueSet< UINT >( pNumNest, pBigNumNestValues );
	testfactor::RFactor rBigNumSubRoutine = AddParameterValueSet< UINT >( pNumSubRoutine, pBigNumSubRoutineValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases_Test1 = rOddInstruction * rEvenInstruction * rNumNest * rNumSubRoutine;
	testfactor::RFactor rfTestCases_Test2 = rLimitedOddInstruction * rLimitedEvenInstruction * rBigNumNest * rBigNumSubRoutine;
	testfactor::RFactor rfTestCasesWithShaderStages_Test1 = GetTestFactorWithDefaultStageCoverage( rfTestCases_Test1 );
	testfactor::RFactor rfTestCasesWithShaderStages_Test2 = GetTestFactorWithDefaultStageCoverage( rfTestCases_Test2 );

	SetRootTestFactor( rfTestCasesWithShaderStages_Test1 + rfTestCasesWithShaderStages_Test2 );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumNest"), 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumSubRoutine"), 1), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_NestedControlFlow::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	TCHAR szBuffer[1024];
	tstring szInstruction = ""; 

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStrNoMask(m_ShaderStage, 0);

	szInstruction += "mov r6.x, 0xffffffff\n";
	szInstruction += outputMovStr + ", r6	\n";

	const UINT regOffset = 10; //This is because r7 is the constant buffer.

	//Initialize temp registers
	for (UINT iSub=0; iSub<m_NumSubRoutine + 1; iSub++)
	{
		for (UINT iNest=0; iNest<=(m_NumNest-1) / 32; iNest++)
		{
			UINT reg = (iSub * 3) + iNest  + regOffset;

			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "mov r%d, 0\n", reg );
			szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
			szInstruction += tstring(szBuffer);
		}
	}

	for (UINT iSub=0; iSub<m_NumSubRoutine + 1; iSub++)
	{
		if (iSub > 0)
		{
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "label l%d\n", iSub - 1 );
			szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
			szInstruction += tstring(szBuffer);
		}
		for (UINT iNest=0; iNest<m_NumNest; iNest++)
		{
			UINT shift = 1 << (iNest % 32);
			UINT reg = (iSub * 3) + (iNest / 32) + regOffset;

			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "or r%d, r%d, 0x%x\n", reg, reg, shift );
			szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
			tstring strGood = tstring(szBuffer);
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "and r%d, r%d, 0x%x\n", reg, reg, shift );
			szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
			tstring strBad = tstring(szBuffer);

			ControlFlowInstruction instruction = iNest % 2 ? m_OddInstruction : m_EvenInstruction;

			switch (instruction)
			{
			case CF_if_z :
				szInstruction +=		"if_z r7.x					\n";
				break;

			case CF_if_nz :
				szInstruction +=		"if_nz r7.y					\n";
				break;

			case CF_switch :
				szInstruction +=		"switch r7.x				\n"
					"    case 1					\n"
					+ strBad +
					"    break					\n"
					"    case 0					\n";
				break;

			case CF_loop :
				szInstruction +=		"loop		\n";
				break;
			}
			szInstruction += strGood;
		}
		if (iSub != m_NumSubRoutine)
		{
			_snprintf( szBuffer, sizeof( szBuffer ) - 1, "call l%d\n", iSub );
			szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
			szInstruction += tstring(szBuffer);
		}
		for (int iNest=m_NumNest-1; iNest>=0; iNest--)
		{
			//Go reverse order
			ControlFlowInstruction instruction = iNest % 2 ? m_OddInstruction : m_EvenInstruction;

			switch (instruction)
			{
			case CF_if_z :
			case CF_if_nz :
				szInstruction +=	"endif		\n";
				break;

			case CF_switch :
				szInstruction +=	"break		\n"
					"endswitch	\n";
				break;

			case CF_loop :
				szInstruction +=	"break		\n"
					"endloop	\n";
				break;
			}
		}

		//Output result in the main routine
		if (iSub == 0)
		{
			for (UINT iSubForResult=0; iSubForResult<m_NumSubRoutine + 1; iSubForResult++)
			{
				for (UINT iNest=0; iNest<=(m_NumNest-1) / 32; iNest++)
				{
					UINT reg = (iSubForResult * 3) + iNest + regOffset;

					UINT shift = 0;
					UINT numShift = m_NumNest >= (iNest + 1) * 32 ? 32 : m_NumNest % 32;
					for (UINT iShift=0; iShift<numShift; iShift++)
						shift |= 1 << iShift;

					if (iSubForResult <= 32)
					{
						_snprintf( szBuffer, sizeof( szBuffer ) - 1, ("xor r0, r%d, 0x%x\n"
							"if_nz r0.x\n"
							"    mov r6.x, 0\n"
							"    mov r6.y, %d\n"
							"    mov r6.z, r%d.x\n"
							"    mov r6.w, 0x%x\n"
							"    " + outputMovStr + ", r6\n"
							"%s"
							"    ret\n"
							"endif\n").c_str()
							, reg, shift, reg, reg, shift, m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY ? "   emit\n" : "" );
						szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
					}
					else
					{
						//Calls should be skipped after 32 sub routine on the stack
						_snprintf( szBuffer, sizeof( szBuffer ) - 1, ("if_nz r%d.x\n"
							"    mov r6.x, 0\n"
							"    mov r6.y, %d\n"
							"    mov r6.z, r%d.x\n"
							"    mov r6.w, 0x0\n"
							"    " + outputMovStr + ", r6\n"
							"%s"
							"    ret\n"
							"endif\n").c_str()
							, reg, reg, reg, m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY ? "   emit\n" : "" );
						szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
					}
					// Make PreFast happy	
					szBuffer[1023] = 0;
					szInstruction += tstring(szBuffer);
				}
			}
			if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
				szInstruction += "emit\n";

			szInstruction += "ret\n";
		}
		else
		{
			szInstruction += "ret\n";
		}
	}

	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0] = 0;
	pData[1] = 0xffffffff;

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_NestedControlFlow::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline(asmHelper, szInstruction.c_str(), false);
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_NestedControlFlow::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	DWORD result[4];
	HRESULT hr;

	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch(1,1,1);
	else
		GetEffectiveContext()->Draw( 1, 0 );
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result) ) ) 
		{
			WriteToLog( _T( "CShader5xTest_NestedControlFlow::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );

		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pStreamOutputBufferCopy, result) ) ) 
		{
			WriteToLog( _T( "CShader5xTest_NestedControlFlow::ExecuteTestCase() - failed to map m_pStreamOutputBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	UINT expectedResult = CalcResult();
	if ( expectedResult != result[0] )
	{
		WriteToLog( 0, "Expected 0x%x : result 0x%x (reg %d. value is 0x%x but should be 0x%x)", expectedResult, result[0], result[1], result[2], result[3] );
		tr = RESULT_FAIL;
	}

	return tr;
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_NestedControlFlow::CalcResult()
{
	UINT expectedResult = 0xffffffff;
	return expectedResult;
}

////////////////////////////////////////////////////////////////////////////////////////
// if
////////////////////////////////////////////////////////////////////////////////////////
void CShader5xTest_if::InitTestParameters()
{
	testfactor::RFactor src = AddParameter( "Src", &m_Src, RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor zeroCondition = AddParameter( "ZeroContidional", &m_bZeroCondition, BoolValueSet() );
	testfactor::RFactor instruction = AddParameter( "Else", &m_bElse, BoolValueSet() );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src * zeroCondition * instruction;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("Else"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_if::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	tstring szInstruction;
	tstring szInitResult;
	tstring szInitInstruction;
	tstring szMiddleInstruction;

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	szInitResult = "" + outputMovStr + ", 0xca1cca1c"		"\n";

	if (m_bZeroCondition)
		szInitInstruction = "if_z r7.x"	"\n";
	else
		szInitInstruction = "if_nz r7.x"	"\n";

	if (m_bElse)
		szMiddleInstruction =	"    " + outputMovStr + ", 0x11111111"	"\n"
		"else"							"\n"
		"    " + outputMovStr + ", 0x22222222"	"\n";
	else
		szMiddleInstruction =	"    " + outputMovStr + ", 0x11111111"	"\n";

	szInstruction =	szInitResult +
		szInitInstruction +
		szMiddleInstruction +
		"endif"							"\n";

	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0] = m_Src;

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_if::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline(asmHelper, szInstruction.c_str());
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_if::CalcResult()
{
	UINT result = 0xca1cca1c;
	bool bZeroValue = m_Src == 0;
	bool bExecIf = m_bZeroCondition == bZeroValue ;
	bool bExecElse = m_bElse && m_bZeroCondition != bZeroValue;
	if (bExecIf)
		result = 0x11111111;
	if (bExecElse)
		result = 0x22222222;

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
// loop
////////////////////////////////////////////////////////////////////////////////////////
void CShader5xTest_loop::InitTestParameters()
{
	CUserValueSet< ControlFlowInstruction > *pLoopCFValues = new CUserValueSet< ControlFlowInstruction >();
	pLoopCFValues->AddValue( CF_break );
	pLoopCFValues->AddValue( CF_breakc_z );
	pLoopCFValues->AddValue( CF_breakc_nz );
	pLoopCFValues->AddValue( CF_continue );
	pLoopCFValues->AddValue( CF_continuec_z );
	pLoopCFValues->AddValue( CF_continuec_nz );
	pLoopCFValues->AddValue( CF_ret );
	pLoopCFValues->AddValue( CF_retc_z );
	pLoopCFValues->AddValue( CF_retc_nz );

	testfactor::RFactor src = AddParameter( "Src", &m_Src, RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor instruction = AddParameter( "LoopInstruction", &m_LoopInstruction, pLoopCFValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src * instruction;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_loop::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	tstring szInstruction;
	tstring szInitResult;
	tstring szInitInstruction;
	tstring szMiddleInstruction;

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	szInitResult =	"" + outputMovStr + ", 0xca1cca1c"		"\n"
		"mov r0.xy, 0"					"\n"
		"mov r0.zw, 5"					"\n";

	szInitInstruction = "loop"	"\n";

	switch (m_LoopInstruction)
	{
	case CF_break:
		m_ExpectedResult = 1;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    break"						"\n";
		break;

	case CF_breakc_z:
		m_ExpectedResult = 5;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    iadd r0.z, r0.z, -1"		"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    breakc_z r0.z"				"\n"
			"    " + outputMovStr + ", 0x22222222"	"\n";
		break;

	case CF_breakc_nz:
		m_ExpectedResult = 1;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    iadd r0.z, r0.z, -1"		"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    breakc_nz r0.x"			"\n"
			"    " + outputMovStr + ", 0x22222222"	"\n";
		break;

	case CF_continue:
		m_ExpectedResult = 5;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    iadd r0.z, r0.z, -1"		"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    if_nz r0.z"				"\n"
			"        " + outputMovStr + ", 0x22222222""\n"
			"        continue"				"\n"
			"    else"						"\n"
			"        break"					"\n"
			"    endif"						"\n";
		break;

	case CF_continuec_z:
		m_ExpectedResult = 1;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    iadd r0.z, r0.z, -1"		"\n"
			"    " + outputMovStr + ", 0x22222222"	"\n"
			"    continuec_z r0.x"			"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    break"						"\n";
		break;

	case CF_continuec_nz: 
		m_ExpectedResult = 5;
		szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
			"    iadd r0.z, r0.z, -1"		"\n"
			"    " + outputMovStr + ", 0x22222222"	"\n"
			"    continuec_nz r0.z"			"\n"
			"    " + outputMovStr + ", r0.x"			"\n"
			"    break"						"\n";
		break;

	case CF_ret:
		m_ExpectedResult = 1;
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    emit"						"\n"
				"    ret"						"\n";
		}
		else
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    ret"						"\n";
		}
		break;

	case CF_retc_z:
		m_ExpectedResult = 5;
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    iadd r0.z, r0.z, -1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    if_z r0.z"					"\n"
				"        emit"					"\n"
				"    endif"						"\n"
				"    retc_z r0.z"				"\n"
				"    " + outputMovStr + ", 0x22222222"	"\n"
				"    if_z r0.z"					"\n"
				"        emit"					"\n"
				"    endif"						"\n";
		}
		else
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    iadd r0.z, r0.z, -1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    retc_z r0.z"				"\n"
				"    " + outputMovStr + ", 0x22222222"	"\n";
		}
		break;

	case CF_retc_nz:
		m_ExpectedResult = 1;
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    iadd r0.z, r0.z, -1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    if_nz r0.x"				"\n"
				"        emit"					"\n"
				"    endif"						"\n"
				"    retc_nz r0.x"				"\n"
				"    " + outputMovStr + ", 0x22222222"	"\n"
				"    if_nz r0.x"				"\n"
				"        emit"					"\n"
				"    endif"						"\n";
		}
		else
		{
			szMiddleInstruction =	"    iadd r0.x, r0.x, 1"		"\n"
				"    iadd r0.z, r0.z, -1"		"\n"
				"    " + outputMovStr + ", r0.x"			"\n"
				"    retc_nz r0.x"				"\n"
				"    " + outputMovStr + ", 0x22222222"	"\n";
		}
		break;

	}
	szInstruction =	szInitResult +
		szInitInstruction +
		szMiddleInstruction +
		"endloop"							"\n";



	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0] = m_Src;

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_loop::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline(asmHelper, szInstruction.c_str());
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_loop::CalcResult()
{
	return m_ExpectedResult;
}

////////////////////////////////////////////////////////////////////////////////////////
// call
////////////////////////////////////////////////////////////////////////////////////////
void CShader5xTest_call::InitTestParameters()
{
	CUserValueSet< UINT > *pNumRecursionValues = new CUserValueSet< UINT >();
	pNumRecursionValues->AddValue( 1 );

	// Recursion is being removed from the testing per request from billkris and andyg.
	/*pNumRecursionValues->AddValue( 2 );
	pNumRecursionValues->AddValue( 10 );
	pNumRecursionValues->AddValue( 31 );
	pNumRecursionValues->AddValue( 32 );
	pNumRecursionValues->AddValue( 33 );*/

	CUserValueSet< UINT > *pNumLabelValues = new CUserValueSet< UINT >();
	pNumLabelValues->AddValue( 1 );
	pNumLabelValues->AddValue( 2 );
	pNumLabelValues->AddValue( 3 );
	pNumLabelValues->AddValue( 10 );

	CUserValueSet< ControlFlowInstruction > *pInstructionValues = new CUserValueSet< ControlFlowInstruction >();
	pInstructionValues->AddValue( CF_call );
	pInstructionValues->AddValue( CF_callc_z );
	pInstructionValues->AddValue( CF_callc_nz );

	testfactor::RFactor rNumRecursion = AddParameter( "NumRecursion", &m_NumRecursion, pNumRecursionValues );
	testfactor::RFactor rNumLabel = AddParameter( "NumLabel", &m_NumLabel, pNumLabelValues );

	testfactor::RFactor rInstruction = AddParameter( "Instruction", &m_Instruction, pInstructionValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rNumRecursion * rNumLabel * rInstruction;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumLabel"), 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumLabel"), 3), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumLabel"), 10), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}


//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_call::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	tstring szInstruction;
	tstring szInitResult;
	tstring szLabels = "";

	TCHAR szBuffer[1024];

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	szInitResult =	"" + outputMovStr + ", 0xca1cca1c"		"\n"
		"mov r0.xy, 0"					"\n"
		"mov r0.zw, 5"					"\n"
		"mov r2.x, 0"					"\n"
		"mov r2.y, 1"					"\n";
	_snprintf( 
		szBuffer, 
		sizeof( szBuffer ) - 1, 
		"mov r1, %d\n",
		m_NumRecursion);
	szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
	szInitResult += szBuffer;

	szInitResult += "call l0"						"\n"
		"" + outputMovStr + ", r0"					"\n";
	if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		szInitResult += "emit"						"\n";

	szInitResult += "ret"						"\n";

	//TODO: validation should be improved so that we know each label was executed correct number of times and in right order

	//UINT reg = (iSubForResult * 3) + iNest + regOffset;

	//UINT shift = 0;
	//UINT numShift = m_NumNest >= (iNest + 1) * 32 ? 32 : m_NumNest % 32;
	//for (UINT iShift=0; iShift<numShift; iShift++)
	//	shift |= 1 << iShift;
	tstring szCall = "";
	switch (m_Instruction)
	{
	case CF_call:
		szCall = "call";
		break;
	case CF_callc_z:
		szCall = "callc_z r2.x,";
		break;
	case CF_callc_nz:
		szCall = "callc_nz r2.y,";
		break;
	}

	for (UINT i=0; i<m_NumLabel; i++)
	{
		// BUG: WIN7 485391 - Remove recursion from shader5x
		// Description: At the last label we want to return ("ret") and NOT call back to the begining
		tstring callStr;
		if( i == (m_NumLabel - 1) )
			callStr = "";	// Note the blank str. We want this to be blank to avoid a double "ret" at the end of the shader
		else
		{
			callStr = szCall.c_str();
			callStr += " l";
			callStr += ToString(i + 1);
		}

		_snprintf( 
			szBuffer, 
			sizeof( szBuffer ) - 1, 
			"label l%d\n"
			"    iadd r1, r1, -1\n"
			"    iadd r0.x, r0.x, 1\n"
			"    retc_z r1.x\n"
			"    %s\n"
			"    ret\n",	// Note this forced ret. If we are at the last label num, the string above should NOT be a ret to avoid a double "ret" at the end. See above comments.
			i,
			callStr.c_str());
		szBuffer[sizeof( szBuffer ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
		szLabels += szBuffer;
	}

	szInstruction =	szInitResult + szLabels;

	return SetPipeline(asmHelper, szInstruction.c_str(), false);
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_call::CalcResult()
{

	return m_NumRecursion > 32 ? 32 : m_NumRecursion;
}

////////////////////////////////////////////////////////////////////////////////////////
// switch
////////////////////////////////////////////////////////////////////////////////////////
enum Switch_Case_Value
{
	Case_0,
	Case_1,
	Case_Default,
};

//-------------------------------------------------------------------------------
void CShader5xTest_switch::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	CUserValueSet< SwitchCaseBreak > *pSwitchCaseBreakValues = new CUserValueSet< SwitchCaseBreak >();
	pSwitchCaseBreakValues->AddValue( Switch_Case_None );
	pSwitchCaseBreakValues->AddValue( Switch_Case_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_RetC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_RetC );

	testfactor::RFactor breakCase[3];
	breakCase[0] = AddParameter( "Case_0", &m_Break[0], pSwitchCaseBreakValues );
	breakCase[1] = AddParameter( "Case_1", &m_Break[1], pSwitchCaseBreakValues );
	breakCase[2] = AddParameter( "Case_Default", &m_Break[2], pSwitchCaseBreakValues );

	CUserValueSet< SwitchCaseDefault > *pSwitchCaseDefaultValues = new CUserValueSet< SwitchCaseDefault >();
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_Begin );
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_Middle );
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_End );

	CUserValueSet< SwitchCaseDefault > *pSwitchCaseNoDefaultValues = new CUserValueSet< SwitchCaseDefault >();
	pSwitchCaseNoDefaultValues->AddValue( Switch_Case_No_Default );

	CTestCaseParameter< SwitchCaseDefault > *pSwitchCase = AddParameter< SwitchCaseDefault >( _T("Default"), &m_Default );
	testfactor::RFactor defaultCase = AddParameterValueSet< SwitchCaseDefault >( pSwitchCase, pSwitchCaseDefaultValues );
	testfactor::RFactor noDefaultCase = AddParameterValueSet< SwitchCaseDefault >( pSwitchCase, pSwitchCaseNoDefaultValues );

	CTestCaseParameter< UINT > *pSrcSwitch = AddParameter< UINT >( _T("Src_Switch"), &m_Src[0] );
	testfactor::RFactor srcSwitch3 = AddParameterValueSet< UINT >( pSrcSwitch, RangeValueSet((UINT)0, (UINT)2, 1) );
	testfactor::RFactor srcSwitch2 = AddParameterValueSet< UINT >( pSrcSwitch, RangeValueSet((UINT)0, (UINT)1, 1) );

	testfactor::RFactor srcCondition1 = AddParameter( "Src_Condition1", &m_Src[1], RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor srcCondition2 = AddParameter( "Src_Condition2", &m_Src[2], RangeValueSet((UINT)0, (UINT)1, 1) );

	/*
	SetRootTestFactor(	shaderStage * 
		( 
		( breakCase[0] * breakCase[1] * breakCase[2] * defaultCase * srcSwitch3 ) +
		( breakCase[0] * breakCase[1] * noDefaultCase * srcSwitch2 )
		) *
		srcCondition1 * 
		srcCondition2
		);
	*/

	// full permutation generates too many test cases, so let's use cross-factors to reduce the number
	testfactor::XFACTOR pairwiseTable_Switch2[] = 
	{
		// factor, group, pNotGrps, cNotGrps
		{breakCase[0], 1, NULL, 0},
		{breakCase[1], 1, NULL, 0},
		{noDefaultCase, 1, NULL, 0},
	};

	testfactor::XFACTOR pairwiseTable_Switch3[] = 
	{
		// factor, group, pNotGrps, cNotGrps
		{breakCase[0], 1, NULL, 0},
		{breakCase[1], 1, NULL, 0},
		{breakCase[2], 1, NULL, 0},
		{defaultCase, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2),
	};

	testfactor::RFactor rSwitch2 
		= NewCrossFactor(pairwiseTable_Switch2, _countof(pairwiseTable_Switch2), "Switch2", groupOrders, _countof(groupOrders))
		* srcSwitch2;
	testfactor::RFactor rSwitch3 
		= NewCrossFactor(pairwiseTable_Switch3, _countof(pairwiseTable_Switch3), "Switch3", groupOrders, _countof(groupOrders))
		* srcSwitch3;
	testfactor::RFactor rSwitch_3plus2 = rSwitch3 + rSwitch2;

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rSwitch_3plus2 * srcCondition1 * srcCondition2;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SwitchCaseDefault>(_T("Default"), Switch_Case_Default_End), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseDefault>(_T("Default"), Switch_Case_No_Default), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_RetC_Break), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_RetC_Break), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_RetC_Break), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 6.5f );
	DeclarePriorityLevelByPriorityValue( 2, 3.375f );
}

//-------------------------------------------------------------------------------
tstring CShader5xTest_switch::GenerateSwitchCase(AssemblyHelper& asmHelper, unsigned int caseValue)
{
	tstring outputMovStr = "";
		
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	tstring szMiddleInstruction;
	UINT mask = 0;
	switch (caseValue)
	{
	case Case_0:
		szMiddleInstruction =	"    case 0"	"\n";
		mask = 0;
		break;

	case Case_1:
		szMiddleInstruction =	"    case 1"	"\n";
		mask = 8;
		break;

	case Case_Default:
		szMiddleInstruction =	"    default"	"\n";
		mask = 16;
		break;
	}

	char szTemp[20];
	tstring szMask;
	_snprintf( szTemp, 19, "0x%x", (1 << mask) );
	szTemp[sizeof( szTemp ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1

	// Non-empty fallthrough cases are not allowed
	if ( m_Break[caseValue] != Switch_Case_None )
	{	
		szMask = szTemp;
		szMiddleInstruction +=	"        or r0.x, r0.x, " + szMask + "\n"
			"        " + outputMovStr + ", r0.x"			"\n";
	}

	switch (m_Break[caseValue])
	{
	case Switch_Case_Break:
		szMiddleInstruction +=	"        break"					"\n";
		break;

	case Switch_Case_Ret:
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction +=	"        emit"					"\n"
				"        ret"					"\n";
		}
		else
		{
			szMiddleInstruction +=	"        ret"					"\n";
		}
		break;

	case Switch_Case_BreakC:
	case Switch_Case_BreakC_Break:
	case Switch_Case_BreakC_BreakC:
	case Switch_Case_BreakC_Ret:
	case Switch_Case_BreakC_RetC:
		szMiddleInstruction +=	"        breakc_z r7.y"		"\n";
		break;

	case Switch_Case_RetC:
	case Switch_Case_RetC_Break:
	case Switch_Case_RetC_BreakC:
	case Switch_Case_RetC_Ret:
	case Switch_Case_RetC_RetC:
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction +=	"        if_z r7.y"			"\n"
				"            emit"				"\n"
				"        endif"					"\n"
				"        retc_z r7.y"		"\n";
		}
		else
		{
			szMiddleInstruction +=	"        retc_z r7.y"		"\n";
		}
		break;

	}

	switch (m_Break[caseValue])
	{
	case Switch_Case_BreakC:
	case Switch_Case_BreakC_Break:
	case Switch_Case_BreakC_BreakC:
	case Switch_Case_BreakC_Ret:
	case Switch_Case_BreakC_RetC:
	case Switch_Case_RetC:
	case Switch_Case_RetC_Break:
	case Switch_Case_RetC_BreakC:
	case Switch_Case_RetC_Ret:
	case Switch_Case_RetC_RetC:
		_snprintf( szTemp, 19, "0x%x", (2 << mask) );
		szTemp[sizeof( szTemp ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
		szMask = szTemp;
		szMiddleInstruction +=	"        or r0.x, r0.x, " + szMask + "\n"
			"        " + outputMovStr + ", r0.x"			"\n";
		break;

	}

	switch (m_Break[caseValue])
	{
	case Switch_Case_BreakC_Break:
	case Switch_Case_RetC_Break:
		szMiddleInstruction +=	"        break"					"\n";
		break;

	case Switch_Case_BreakC_Ret:
	case Switch_Case_RetC_Ret:
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction +=	"        emit"					"\n"
				"        ret"					"\n";
		}
		else
		{
			szMiddleInstruction +=	"        ret"					"\n";
		}
		break;

	case Switch_Case_BreakC_BreakC:
	case Switch_Case_RetC_BreakC:
		szMiddleInstruction +=	"        breakc_z r7.z"		"\n";
		break;

	case Switch_Case_BreakC_RetC:
	case Switch_Case_RetC_RetC:
		if (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		{
			szMiddleInstruction +=	"        if_z r7.z"			"\n"
				"            emit"				"\n"
				"        endif"					"\n"
				"        retc_z r7.z"		"\n";
		}
		else
		{
			szMiddleInstruction +=	"        retc_z r7.z"		"\n";
		}
		break;
	}

	switch (m_Break[caseValue])
	{
	case Switch_Case_BreakC_BreakC:
	case Switch_Case_BreakC_RetC:
	case Switch_Case_RetC_BreakC:
	case Switch_Case_RetC_RetC:
		_snprintf( szTemp, 19, "0x%x", (4 << mask) );
		szTemp[sizeof( szTemp ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1
		szMask = szTemp;
		szMiddleInstruction +=	"        or r0.x, r0.x, " + szMask + "\n"
			"        " + outputMovStr + ", r0.x"			"\n";		
		break;

	}

	// Non-empty fallthrough cases are not allowed
	switch ( m_Break[caseValue] )
	{
	case Switch_Case_BreakC:
	case Switch_Case_RetC:
	case Switch_Case_BreakC_BreakC:
	case Switch_Case_BreakC_RetC:
	case Switch_Case_RetC_Break:
	case Switch_Case_RetC_BreakC:
	case Switch_Case_RetC_RetC:
		szMiddleInstruction += "        break" "\n";
		break;
	}

	return szMiddleInstruction;
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_switch::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	tstring szInstruction;
	tstring szInitResult;
	tstring szInitInstruction;
	tstring szCaseInstruction[3];

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::x);

	szInitResult =	"" + outputMovStr + ", 0xca1cca1c"		"\n"
		"mov r0, 0"						"\n";

	szInitInstruction = "switch r7.x"	"\n";

	switch (m_Default) 
	{
	case Switch_Case_No_Default:
		szCaseInstruction[0] += GenerateSwitchCase(asmHelper, Case_0);
		szCaseInstruction[1] += GenerateSwitchCase(asmHelper, Case_1);
		break;

	case Switch_Case_Default_Begin:
		szCaseInstruction[0] += GenerateSwitchCase(asmHelper, Case_Default);
		szCaseInstruction[1] += GenerateSwitchCase(asmHelper, Case_0);
		szCaseInstruction[2] += GenerateSwitchCase(asmHelper, Case_1);
		break;

	case Switch_Case_Default_Middle:
		szCaseInstruction[0] += GenerateSwitchCase(asmHelper, Case_0);
		szCaseInstruction[1] += GenerateSwitchCase(asmHelper, Case_Default);
		szCaseInstruction[2] += GenerateSwitchCase(asmHelper, Case_1);
		break;

	case Switch_Case_Default_End:
		szCaseInstruction[0] += GenerateSwitchCase(asmHelper, Case_0);
		szCaseInstruction[1] += GenerateSwitchCase(asmHelper, Case_1);
		szCaseInstruction[2] += GenerateSwitchCase(asmHelper, Case_Default);
		break;
	}

	szInstruction =	szInitResult +
		szInitInstruction +
		szCaseInstruction[0] + 
		szCaseInstruction[1] + 
		szCaseInstruction[2] + 
		"endswitch"							"\n";

	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0] = m_Src[0];
	pData[1] = m_Src[1];
	pData[2] = m_Src[2];

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_switch::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline(asmHelper, szInstruction.c_str());
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_switch::CalcResult()
{
	UINT realCase[3];
	switch (m_Default)
	{
	case Switch_Case_No_Default :
		realCase[0] = 0;
		realCase[1] = 1;
		realCase[2] = 0;
		break;

	case Switch_Case_Default_Begin :
		realCase[0] = 2;
		realCase[1] = 0;
		realCase[2] = 1;
		break;

	case Switch_Case_Default_Middle :
		realCase[0] = 0;
		realCase[1] = 2;
		realCase[2] = 1;
		break;

	case Switch_Case_Default_End :
		realCase[0] = 0;
		realCase[1] = 1;
		realCase[2] = 2;
		break;
	}

	bool bFallThrough[3] = { false, true, true };
	for ( int iCase = 0; iCase < 2; ++iCase )
	{
		switch ( m_Break[ realCase[iCase] ] )
		{
		case Switch_Case_Ret:
		case Switch_Case_Break:
		case Switch_Case_BreakC_Break:
		case Switch_Case_BreakC_Ret:
		case Switch_Case_RetC_Break:
		case Switch_Case_RetC_Ret:
		case Switch_Case_BreakC:
		case Switch_Case_RetC:
		case Switch_Case_BreakC_BreakC:
		case Switch_Case_BreakC_RetC:
		case Switch_Case_RetC_BreakC:
		case Switch_Case_RetC_RetC:
			bFallThrough[iCase+1] = false;
			break;

		}
		if (iCase == 0 && realCase[iCase] == m_Src[0] && bFallThrough[1] == false)
		{
			bFallThrough[2] = false;
			break;
		}
		//If it's the last case we're going in, we shouldn't fallthrough the middle one.
		if (iCase == 1 && realCase[2] == m_Src[0] && m_Default != Switch_Case_No_Default)
		{
			bFallThrough[1] = false;
		}
	}
	bFallThrough[0] = false;

	bool bDefault[3];
	bDefault[0] = (m_Default == Switch_Case_Default_Begin) && !(m_Src[0] == 0 || m_Src[0] == 1);
	bDefault[1] = (m_Default == Switch_Case_Default_Middle) && !(m_Src[0] == 0 || m_Src[0] == 1);
	bDefault[2] = (m_Default == Switch_Case_Default_End) && !(m_Src[0] == 0 || m_Src[0] == 1);

	UINT result = 0;
	for (int iCase = 0; iCase < 3; iCase ++)
	{
		if ( iCase == 2 && m_Default == Switch_Case_No_Default )
		{
			continue;
		}

		UINT mask = 0;
		switch (realCase[iCase])
		{
		case Case_0:
			mask = 0;
			break;

		case Case_1:
			mask = 8;
			break;

		case Case_Default:
			mask = 16;
			break;
		}

		if ( bDefault[iCase] || bFallThrough[iCase] || realCase[iCase] == m_Src[0] )
		{
			switch (m_Break[realCase[iCase]])
			{
			case Switch_Case_None:
				break;

			case Switch_Case_Break:
			case Switch_Case_Ret:
				result |= 1 << mask;
				break;

			case Switch_Case_BreakC:
			case Switch_Case_RetC:
			case Switch_Case_BreakC_Break:
			case Switch_Case_BreakC_Ret:
			case Switch_Case_RetC_Break:
			case Switch_Case_RetC_Ret:
				if (m_Src[1] == 0)
					result |= 1 << mask;
				else
					result |= (1 | 2) << mask;
				break;

			case Switch_Case_BreakC_BreakC:
			case Switch_Case_BreakC_RetC:
			case Switch_Case_RetC_BreakC:
			case Switch_Case_RetC_RetC:
				if (m_Src[1] == 0)
					result |= 1 << mask;
				else if (m_Src[2] == 0)
					result |= (1 | 2) << mask;
				else
					result |= (1 | 2 | 4) << mask;
				break;
			}
		}
	}
	if (result == 0)
		result = 0xca1cca1c;
	return result;
}

//-------------------------------------------------------------------------------
UINT32 TrueValuesNC[] =
{
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};

//-------------------------------------------------------------------------------
void CShader5xTest_ControlFlowNC::InitTestParameters()
{
	//Set up test factor for src
	testfactor::RFactor src = AddParameter( "Src", &m_Src, new CTableValueSet< UINT32 >( &TrueValuesNC[0], sizeof( TrueValuesNC[0] ), sizeof( TrueValuesNC ) / sizeof( TrueValuesNC[0] ) ) );

	//Set up test factor for instructions
	CUserValueSet< ControlFlowConditionalInstructionNC > *pInstructionValues = new CUserValueSet< ControlFlowConditionalInstructionNC >();	
	pInstructionValues->AddValue( CFNC_loop_R );
	pInstructionValues->AddValue( CFNC_if_z_R );
	pInstructionValues->AddValue( CFNC_if_z_RG );
	pInstructionValues->AddValue( CFNC_if_z_RGB );
	pInstructionValues->AddValue( CFNC_if_z_RGBA );
	pInstructionValues->AddValue( CFNC_if_z_RB );
	pInstructionValues->AddValue( CFNC_if_z_GA );
	pInstructionValues->AddValue( CFNC_if_z_G );
	pInstructionValues->AddValue( CFNC_if_z_B );
	pInstructionValues->AddValue( CFNC_if_z_A );

	CTestCaseParameter< ControlFlowConditionalInstructionNC > *pInstruction = AddParameter< ControlFlowConditionalInstructionNC >( _T("Instruction"), &m_Instruction );
	testfactor::RFactor instruction = AddParameterValueSet< ControlFlowConditionalInstructionNC >( pInstruction, pInstructionValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor pixelShaderStage = AddParameterValue( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	SetRootTestFactor( pixelShaderStage * src * instruction );
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlowNC::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tr )
		return tr;

	if (m_Instruction == CFC_discard_z || m_Instruction == CFC_discard_nz)
	{
		if (m_ShaderStage != D3D_SHADER_STAGE_PIXEL)
		{
			return RESULT_SKIP;
		}
	}

	tstring outputMovStr = "";
	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort, 1);
	outputMovStr = asmHelper.GetRawOutputMovStrNoMask(m_ShaderStage, 0);

	tstring szInstruction;
	switch (m_Instruction)
	{
	case CFNC_loop_R:
		szInstruction =			
			"loop							\n"
			"	breakc_nz r7.x				\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	" + outputMovStr + ", r6	\n"
			"	breakc_z r7.y				\n"
			"endloop						\n";
		break;
	case CFNC_if_z_R :
		szInstruction =
			"if_z r7.x						\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_RG :
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	mov r6.y,		0xE0E0E0E0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_RGB:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	mov r6.y,		0xE0E0E0E0	\n"
			"	mov r6.z,		0xB0B0B0B0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_RGBA:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	mov r6.y,		0xE0E0E0E0	\n"
			"	mov r6.z,		0xB0B0B0B0	\n"
			"	mov r6.w,		0xA0A0A0A0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_RB:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.x,		0xFFFFFFFF	\n"
			"	mov r6.z,		0xB0B0B0B0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_GA:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.y,		0xE0E0E0E0	\n"
			"	mov r6.w,		0xA0A0A0A0	\n"			
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_G:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.y,		0xE0E0E0E0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_B:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.z,		0xB0B0B0B0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;
	case CFNC_if_z_A:
		szInstruction =			
			"if_z r7.x						\n"
			"	mov r6.w,		0xA0A0A0A0	\n"
			"	" + outputMovStr + ", r6	\n"
			"endif							\n";
		break;

	}

	// Set the constant buffer data
	HRESULT hr;
	UINT32 pData[CONST_BUFFER_SIZE/4];
	ZeroMemory(pData, sizeof(pData));

	pData[0]  = (m_Src >> 0) & 0x1;
	pData[4]  = (m_Src >> 1) & 0x1;
	pData[8]  = (m_Src >> 2) & 0x1;
	pData[12] = (m_Src >> 3) & 0x1;

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_ControlFlowNC::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return SetPipeline(asmHelper, szInstruction.c_str());
}

//-------------------------------------------------------------------------------
PIXEL32 CShader5xTest_ControlFlowNC::CalcResult(UINT x, UINT y)
{
	PIXEL32 p;
	p.red=0;
	p.green=0;
	p.blue=0;
	p.alpha=0;
	int offset=x+(y*2);
	switch (m_Instruction)
	{
	
	case CFNC_loop_R:
	case CFNC_if_z_R:
		if(!((m_Src>>offset) & 0x1))
		{
			p.red  =0xFFFFFFFF;
		}
		break;
	
	case CFNC_if_z_RG:
		if(!((m_Src>>offset) & 0x1))
		{
			p.red  =0xFFFFFFFF;
			p.green=0xE0E0E0E0;
		}
		break;
	case CFNC_if_z_RGB:
		if(!((m_Src>>offset) & 0x1))
		{
			p.red  =0xFFFFFFFF;
			p.green=0xE0E0E0E0;
			p.blue =0xB0B0B0B0;
		}
		break;	
	case CFNC_if_z_RGBA:
		if(!((m_Src>>offset) & 0x1))
		{
			p.red  =0xFFFFFFFF;
			p.green=0xE0E0E0E0;
			p.blue =0xB0B0B0B0;
			p.alpha=0xA0A0A0A0;
		}
		break;
	case CFNC_if_z_RB:
		if(!((m_Src>>offset) & 0x1))
		{
			p.red  =0xFFFFFFFF;
			p.blue =0xB0B0B0B0;
		}
		break;
	case CFNC_if_z_GA:
		if(!((m_Src>>offset) & 0x1))
		{			
			p.green=0xE0E0E0E0;
			p.alpha=0xA0A0A0A0;
		}
		break;
	case CFNC_if_z_G:
		if(!((m_Src>>offset) & 0x1))
		{			
			p.green=0xE0E0E0E0;
		}
		break;
	case CFNC_if_z_B:
		if(!((m_Src>>offset) & 0x1))
		{			
			p.blue=0xB0B0B0B0;
		}
		break;
	case CFNC_if_z_A:
		if(!((m_Src>>offset) & 0x1))
		{			
			p.alpha=0xA0A0A0A0;
		}
		break;
	}
	return p;
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_ControlFlowNC::CalcResult()
{
	UINT expectedResult = 0;

	switch (m_Instruction)
	{
	case CFC_if_z:
		return !m_Src;
		break;	
	}
	return expectedResult;
}


//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlowNC::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch(4,1,1);
	else
		GetEffectiveContext()->Draw( 3, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	DWORD result;
	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		D3D11_MAPPED_SUBRESOURCE mappedTexture;

		if( FAILED( hr = GetImmediateContext()->Map( m_pRTBufferCopy, 0, D3D11_MAP_READ, 0, &mappedTexture ) ) ) 
		{
			WriteToLog( _T( "CShader5xTest_ControlFlowNC::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		//The results should be a checker-board pattern in the first 2x2 set of pixels		
		char* results=(char*) mappedTexture.pData;
		const UINT numPixelsInX = 2;
		const UINT numPixelsInY = 2;
		for(int y=0; y<numPixelsInY; y++)
		{
			for(int x=0; x<numPixelsInX; x++)
			{
				PIXEL32 expectedPixel=CalcResult(x,y);
				const int offset = y * (numPixelsInX * sizeof(PIXEL32)) + (x * sizeof(PIXEL32));
				//int offset=(y * mappedTexture.RowPitch) + (x * sizeof(PIXEL32));				
				PIXEL32 resultPixel=*(PIXEL32*)(results+offset);

				if(resultPixel.red!=expectedPixel.red)
				{
					WriteToLog( 0, "Pixel = %d,%d: Expected R value = 0x%x : result 0x%x.",
						x,y,
						expectedPixel.red,
						resultPixel.red);
					tr = RESULT_FAIL;
				}
				if(resultPixel.green!=expectedPixel.green)
				{
					WriteToLog( 0, "Pixel = %d,%d: Expected G value = 0x%x : result 0x%x.",
						x,y,
						expectedPixel.green,
						resultPixel.green);
					tr = RESULT_FAIL;
				}
				if(resultPixel.blue!=expectedPixel.blue)
				{
					WriteToLog( 0, "Pixel = %d,%d: Expected B value = 0x%x : result 0x%x.",
						x,y,
						expectedPixel.blue,
						resultPixel.blue);
					tr = RESULT_FAIL;
				}
				if(resultPixel.alpha!=expectedPixel.alpha)
				{
					WriteToLog( 0, "Pixel = %d,%d: Expected A value = 0x%x : result 0x%x.",
						x,y,
						expectedPixel.alpha,
						resultPixel.alpha);
					tr = RESULT_FAIL;
				}

			}
		}

		GetImmediateContext()->Unmap( m_pRTBufferCopy, 0 );
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );

		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pStreamOutputBufferCopy, result) ) )
		{
			WriteToLog( _T( "CShader5xTest_ControlFlowNC::ExecuteTestCase() - failed to map m_pStreamOutputBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		// not actually doing anything here
		tr = RESULT_SKIP;
	}

	return tr;
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ControlFlowTrueValuesNC::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	//Send 4 vertices for vertex shader and geometry shader stages
	if(	   (m_ShaderStage == D3D_SHADER_STAGE_VERTEX)
		|| (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		|| (m_ShaderStage == D3D_SHADER_STAGE_HULL)
		|| (m_ShaderStage == D3D_SHADER_STAGE_DOMAIN)
		)
	{
		GetEffectiveContext()->Draw( 4, 0 );
	}
	else if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(4,1,1);
	}
	else
	{
		//Send a triangle for the pixel shader stage (this is to a 16x16 render target)
		GetEffectiveContext()->Draw( 3, 0 );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	DWORD result1;
	DWORD result2;
	DWORD result3;
	DWORD result4;

	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		PIXEL32 results[4];
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, results) ) )
		{
			WriteToLog( _T( "CShader5xTest_ControlFlowTrueValuesNC::ExecuteTestCase()) - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		result1 = results[0].red;
		result2 = results[1].red;
		result3 = results[2].red;
		result4 = results[3].red;
	}
	else
	{
		DWORD *pResult = NULL;
		D3D11_MAPPED_SUBRESOURCE mappedSOBuffer;
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );

		if( FAILED( hr = GetImmediateContext()->Map( m_pStreamOutputBufferCopy, 0, D3D11_MAP_READ, 0, &mappedSOBuffer ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ControlFlowTrueValuesNC::ExecuteTestCase() - failed to map m_pStreamOutputBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		//The offsets below are due to the hull & domain shaders spitting out 2 outputs per input
		const size_t stride = (m_ShaderStage == D3D_SHADER_STAGE_HULL || m_ShaderStage == D3D_SHADER_STAGE_DOMAIN) ? 2 : 1;
		pResult = (DWORD *)mappedSOBuffer.pData;
		result1 = *(pResult+0*stride);
		result2 = *(pResult+4*stride);
		result3 = *(pResult+8*stride);
		result4 = *(pResult+12*stride);

		GetImmediateContext()->Unmap( m_pStreamOutputBufferCopy, 0 );
	}

	UINT expectedResult = CalcResult();
	if ( expectedResult != result1 )
	{
		WriteToLog( 0, "Expected 0x%x : result 0x%x.", expectedResult, result1 );
		tr = RESULT_FAIL;
	}

	//Ensure branching occurred, 2nd pixel can't equal first.
	if ( result1 == result2)
	{
		WriteToLog( 0, "Expected results to be different due to branching. They are not different, both equal 0x%x.",
			result1);
		tr = RESULT_FAIL;
	}

	//Ensure results 2 and 3 are equal.
	if ( result2 != result3)
	{
		WriteToLog( 0, "Expected results 2 and 3 to be the same. They are not, result2=0x%x result3=0x%x.",
			result2,
			result3);
		tr = RESULT_FAIL;
	}

	//Ensure results 1 and 4 are equal.
	if ( result1 != result4)
	{
		WriteToLog( 0, "Expected results 1 and 4 to be the same. They are not, result1=0x%x result4=0x%x.",
			result1,
			result4);
		tr = RESULT_FAIL;
	}

	return tr;
}


//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_switchNC::SetupTestCase()
{
	TEST_RESULT tr= CShader5xTest_switch::SetupTestCase();
	HRESULT hr;

	if(tr==RESULT_PASS)
	{
		// Set the constant buffer data
		// Each of the 4 input values will be set depending on the test paramater permutation
		UINT32 pData[CONST_BUFFER_SIZE/4];
		ZeroMemory(pData, sizeof(pData));

		pData[0] = m_Src[0][0];		//c0[0].x
		pData[1] = m_Src[0][1];		//c0[0].y
		pData[2] = m_Src[0][2];		//c0[0].z
		pData[3] = 0;				//c0[0].w
		
		pData[4] = m_Src[1][0];		//c0[1].x
		pData[5] = m_Src[1][1];		//c0[1].y
		pData[6] = m_Src[1][2];		//c0[1].z
		pData[7] = 0;				//c0[1].w

		pData[8] = m_Src[2][0];		//c0[2].x
		pData[9] = m_Src[2][1];		//c0[2].y
		pData[10] = m_Src[2][2];	//c0[2].z
		pData[11] = 0;				//c0[2].w

		pData[12] = m_Src[3][0];	//c0[3].x
		pData[13] = m_Src[3][1];	//c0[3].y
		pData[14] = m_Src[3][2];	//c0[3].z
		pData[15] = 0;				//c0[3].w

		if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
		{
			WriteToLog( _T( "CShader5xTest_switchNC::SetupTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
			return RESULT_FAIL;
		}
	}
	return tr;
}

//-------------------------------------------------------------------------------
void CShader5xTest_switchNC::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	CUserValueSet< SwitchCaseBreak > *pSwitchCaseBreakValues = new CUserValueSet< SwitchCaseBreak >();
	pSwitchCaseBreakValues->AddValue( Switch_Case_None );
	pSwitchCaseBreakValues->AddValue( Switch_Case_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_BreakC_RetC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_Break );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_BreakC );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_Ret );
	pSwitchCaseBreakValues->AddValue( Switch_Case_RetC_RetC );

	testfactor::RFactor breakCase[3];
	breakCase[0] = AddParameter( "Case_0", &m_Break[0], pSwitchCaseBreakValues );
	breakCase[1] = AddParameter( "Case_1", &m_Break[1], pSwitchCaseBreakValues );
	breakCase[2] = AddParameter( "Case_Default", &m_Break[2], pSwitchCaseBreakValues );

	CUserValueSet< SwitchCaseDefault > *pSwitchCaseDefaultValues = new CUserValueSet< SwitchCaseDefault >();
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_Begin );
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_Middle );
	pSwitchCaseDefaultValues->AddValue( Switch_Case_Default_End );

	CUserValueSet< SwitchCaseDefault > *pSwitchCaseNoDefaultValues = new CUserValueSet< SwitchCaseDefault >();
	pSwitchCaseNoDefaultValues->AddValue( Switch_Case_No_Default );

	CTestCaseParameter< SwitchCaseDefault > *pSwitchCase = AddParameter< SwitchCaseDefault >( _T("Default"), &m_Default );
	testfactor::RFactor defaultCase = AddParameterValueSet< SwitchCaseDefault >( pSwitchCase, pSwitchCaseDefaultValues );
	testfactor::RFactor noDefaultCase = AddParameterValueSet< SwitchCaseDefault >( pSwitchCase, pSwitchCaseNoDefaultValues );

	CTestCaseParameter< UINT > *pSrcSwitch_0 = AddParameter< UINT >( _T("c0[0].x"), &m_Src[0][0] );
	testfactor::RFactor srcSwitch3_0 = AddParameterValueSet< UINT >( pSrcSwitch_0, RangeValueSet((UINT)0, (UINT)2, 1) );
	testfactor::RFactor srcSwitch2_0 = AddParameterValueSet< UINT >( pSrcSwitch_0, RangeValueSet((UINT)0, (UINT)1, 1) );

	CTestCaseParameter< UINT > *pSrcSwitch_1 = AddParameter< UINT >( _T("c0[1].x"), &m_Src[1][0] );
	testfactor::RFactor srcSwitch3_1 = AddParameterValueSet< UINT >( pSrcSwitch_1, RangeValueSet((UINT)0, (UINT)2, 1) );
	testfactor::RFactor srcSwitch2_1 = AddParameterValueSet< UINT >( pSrcSwitch_1, RangeValueSet((UINT)0, (UINT)1, 1) );

	CTestCaseParameter< UINT > *pSrcSwitch_2 = AddParameter< UINT >( _T("c0[2].x"), &m_Src[2][0] );
	testfactor::RFactor srcSwitch3_2 = AddParameterValueSet< UINT >( pSrcSwitch_2, RangeValueSet((UINT)0, (UINT)2, 1) );
	testfactor::RFactor srcSwitch2_2 = AddParameterValueSet< UINT >( pSrcSwitch_2, RangeValueSet((UINT)0, (UINT)1, 1) );

	CTestCaseParameter< UINT > *pSrcSwitch_3 = AddParameter< UINT >( _T("c0[3].x"), &m_Src[3][0] );
	testfactor::RFactor srcSwitch3_3 = AddParameterValueSet< UINT >( pSrcSwitch_3, RangeValueSet((UINT)0, (UINT)2, 1) );
	testfactor::RFactor srcSwitch2_3 = AddParameterValueSet< UINT >( pSrcSwitch_3, RangeValueSet((UINT)0, (UINT)1, 1) );

	testfactor::RFactor srcCondition1_0 = AddParameter( "c0[0].y", &m_Src[0][1], RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor srcCondition2_0 = AddParameter( "c0[0].z", &m_Src[0][2], RangeValueSet((UINT)0, (UINT)1, 1) );

	testfactor::RFactor srcCondition1_1 = AddParameter( "c0[1].y", &m_Src[1][1], RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor srcCondition2_1 = AddParameter( "c0[1].z", &m_Src[1][2], RangeValueSet((UINT)0, (UINT)1, 1) );

	testfactor::RFactor srcCondition1_2 = AddParameter( "c0[2].y", &m_Src[2][1], RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor srcCondition2_2 = AddParameter( "c0[2].z", &m_Src[2][2], RangeValueSet((UINT)0, (UINT)1, 1) );

	testfactor::RFactor srcCondition1_3 = AddParameter( "c0[3].y", &m_Src[3][1], RangeValueSet((UINT)0, (UINT)1, 1) );
	testfactor::RFactor srcCondition2_3 = AddParameter( "c0[3].z", &m_Src[3][2], RangeValueSet((UINT)0, (UINT)1, 1) );

	/*
	SetRootTestFactor(
		shaderStage * 
		(
		(
		(
		( breakCase[0] * breakCase[1] * breakCase[2] * defaultCase * srcSwitch3_0 ) +
		( breakCase[0] * breakCase[1] * noDefaultCase * srcSwitch2_0 )
		) *
		srcCondition1_0 * 
		srcCondition2_0
		)
		)
		%
		(
		srcSwitch3_1 * srcSwitch2_1 * srcCondition1_1 * srcCondition2_1 * 
		srcSwitch3_2 * srcSwitch2_2 * srcCondition1_2 * srcCondition2_2 * 
		srcSwitch3_3 * srcSwitch2_3 * srcCondition1_3 * srcCondition2_3 
		)
		);
	*/

	// full permutation generates too many test cases, so let's use cross-factors to reduce the number
	testfactor::XFACTOR pairwiseTable_Case2[] = 
	{
		// factor, group, pNotGrps, cNotGrps
		{breakCase[0], 1, NULL, 0},
		{breakCase[1], 1, NULL, 0},
		{noDefaultCase, 1, NULL, 0},
		//{srcSwitch2_0, 1, NULL, 0},
		{srcSwitch2_1, 1, NULL, 0},
		{srcSwitch2_2, 1, NULL, 0},
		{srcSwitch2_3, 1, NULL, 0},
	};

	testfactor::XFACTOR pairwiseTable_Case3[] = 
	{
		// factor, group, pNotGrps, cNotGrps
		{breakCase[0], 1, NULL, 0},
		{breakCase[1], 1, NULL, 0},
		{breakCase[2], 1, NULL, 0},
		{defaultCase, 1, NULL, 0},
		//{srcSwitch3_0, 1, NULL, 0},
		{srcSwitch3_1, 1, NULL, 0},
		{srcSwitch3_2, 1, NULL, 0},
		{srcSwitch3_3, 1, NULL, 0},
	};

	testfactor::XFACTOR pairwiseTable_Conditions[] = 
	{
		// factor, group, pNotGrps, cNotGrps
		{srcCondition1_0, 1, NULL, 0},
		{srcCondition1_1, 1, NULL, 0},
		{srcCondition1_2, 1, NULL, 0},
		{srcCondition1_3, 1, NULL, 0},
		{srcCondition2_0, 1, NULL, 0},
		{srcCondition2_1, 1, NULL, 0},
		{srcCondition2_2, 1, NULL, 0},
		{srcCondition2_3, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2),
	};

	testfactor::RFactor rSwitch2 
		= NewCrossFactor(pairwiseTable_Case2, _countof(pairwiseTable_Case2), "Case2", groupOrders, _countof(groupOrders))
		* srcSwitch2_0
		;
	testfactor::RFactor rSwitch3 
		= NewCrossFactor(pairwiseTable_Case3, _countof(pairwiseTable_Case3), "Case3", groupOrders, _countof(groupOrders))
		* srcSwitch3_0
		;
	testfactor::RFactor rSwitch_3plus2 = rSwitch3 + rSwitch2;

	testfactor::RFactor rConditions
		= NewCrossFactor(pairwiseTable_Conditions, _countof(pairwiseTable_Conditions), "Conditions", groupOrders, _countof(groupOrders))
		;

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = rSwitch_3plus2 * rConditions;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SwitchCaseDefault>(_T("Default"), Switch_Case_Default_End), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseDefault>(_T("Default"), Switch_Case_No_Default), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_0"), Switch_Case_RetC_Break), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_1"), Switch_Case_RetC_Break), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_Break), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_BreakC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_Ret), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_RetC), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_BreakC_Ret), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SwitchCaseBreak>(_T("Case_Default"), Switch_Case_RetC_Break), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 6.0f );
	DeclarePriorityLevelByPriorityValue( 2, 3.0f );
}

//-------------------------------------------------------------------------------
UINT CShader5xTest_switchNC::CalcResult(UINT index)
{
	UINT realCase[3];
	switch (m_Default)
	{
	case Switch_Case_No_Default :
		realCase[0] = 0;
		realCase[1] = 1;
		realCase[2] = 0;
		break;

	case Switch_Case_Default_Begin :
		realCase[0] = 2;
		realCase[1] = 0;
		realCase[2] = 1;
		break;

	case Switch_Case_Default_Middle :
		realCase[0] = 0;
		realCase[1] = 2;
		realCase[2] = 1;
		break;

	case Switch_Case_Default_End :
		realCase[0] = 0;
		realCase[1] = 1;
		realCase[2] = 2;
		break;
	}

	bool bFallThrough[3] = { false, true, true };
	for ( int iCase = 0; iCase < 2; ++iCase )
	{
		switch ( m_Break[ realCase[iCase] ] )
		{
		case Switch_Case_Ret:
		case Switch_Case_Break:
		case Switch_Case_BreakC_Break:
		case Switch_Case_BreakC_Ret:
		case Switch_Case_RetC_Break:
		case Switch_Case_RetC_Ret:
		case Switch_Case_BreakC:
		case Switch_Case_RetC:
		case Switch_Case_BreakC_BreakC:
		case Switch_Case_BreakC_RetC:
		case Switch_Case_RetC_BreakC:
		case Switch_Case_RetC_RetC:
			bFallThrough[iCase+1] = false;
			break;

		}
		if (iCase == 0 && realCase[iCase] == m_Src[index][0] && bFallThrough[1] == false)
		{
			bFallThrough[2] = false;
			break;
		}
		//If it's the last case we're going in, we shouldn't fallthrough the middle one.
		if (iCase == 1 && realCase[2] == m_Src[index][0] && m_Default != Switch_Case_No_Default)
		{
			bFallThrough[1] = false;
		}
	}
	bFallThrough[0] = false;

	bool bDefault[3];
	bDefault[0] = (m_Default == Switch_Case_Default_Begin) && !(m_Src[index][0] == 0 || m_Src[index][0] == 1);
	bDefault[1] = (m_Default == Switch_Case_Default_Middle) && !(m_Src[index][0] == 0 || m_Src[index][0] == 1);
	bDefault[2] = (m_Default == Switch_Case_Default_End) && !(m_Src[index][0] == 0 || m_Src[index][0] == 1);

	UINT result = 0;
	for (int iCase = 0; iCase < 3; iCase ++)
	{
		if ( iCase == 2 && m_Default == Switch_Case_No_Default )
		{
			continue;
		}

		UINT mask = 0;
		switch (realCase[iCase])
		{
		case Case_0:
			mask = 0;
			break;

		case Case_1:
			mask = 8;
			break;

		case Case_Default:
			mask = 16;
			break;
		}

		if ( bDefault[iCase] || bFallThrough[iCase] || realCase[iCase] == m_Src[index][0] )
		{
			switch (m_Break[realCase[iCase]])
			{
			case Switch_Case_None:
				break;

			case Switch_Case_Break:
			case Switch_Case_Ret:
				result |= 1 << mask;
				break;

			case Switch_Case_BreakC:
			case Switch_Case_RetC:
			case Switch_Case_BreakC_Break:
			case Switch_Case_BreakC_Ret:
			case Switch_Case_RetC_Break:
			case Switch_Case_RetC_Ret:
				if (m_Src[index][1] == 0)
					result |= 1 << mask;
				else
					result |= (1 | 2) << mask;
				break;

			case Switch_Case_BreakC_BreakC:
			case Switch_Case_BreakC_RetC:
			case Switch_Case_RetC_BreakC:
			case Switch_Case_RetC_RetC:
				if (m_Src[index][1] == 0)
					result |= 1 << mask;
				else if (m_Src[index][2] == 0)
					result |= (1 | 2) << mask;
				else
					result |= (1 | 2 | 4) << mask;
				break;
			}
		}
	}
	if (result == 0)
		result = 0xca1cca1c;
	return result;
}

//-------------------------------------------------------------------------------
TEST_RESULT CShader5xTest_switchNC::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	if(	   (m_ShaderStage == D3D_SHADER_STAGE_VERTEX)
		|| (m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY)
		|| (m_ShaderStage == D3D_SHADER_STAGE_HULL)
		|| (m_ShaderStage == D3D_SHADER_STAGE_DOMAIN)
		)
	{
		GetEffectiveContext()->Draw( 4, 0 );
	}
	else if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(4,1,1);
	}
	else
	{
		GetEffectiveContext()->Draw( 3, 0 );		
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	DWORD result[4];

	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );

		// Lock the render target and examine the results
		PIXEL32 pixels[4];

		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, pixels) ) )
		{
			WriteToLog( _T( "CShader5xTest_switchNC::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		for(int i=0; i<4; i++)
			result[i]=pixels[i].red;
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStreamOutputBufferCopy, m_pStreamOutputBuffer );

		DWORD pResult[64];
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pStreamOutputBufferCopy, pResult) ) )
		{
			WriteToLog( _T( "CShader5xTest_switchNC::ExecuteTestCase() - failed to map m_pStreamOutputBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		for(int i=0; i<4; i++)
		{
			if( m_ShaderStage == D3D_SHADER_STAGE_HULL || m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
				result[i] = *(pResult+(2 * i)*4);	// Note: This is 2x because the domain shader executes with isoline frequency (at least twice) thus extra data will be streamed out when using this stage)
			else
				result[i] = *(pResult+i*4);
		}
	}

	for(UINT i=0; i<4; i++)
	{
		UINT expectedResult=CalcResult(i);		
		if(result[i]!=expectedResult)
		{
			WriteToLog( 0, "Result %d: Expected 0x%x : result 0x%x.", i, expectedResult, result[i] );
			tr = RESULT_FAIL;
		}
	}

	return tr;
}
