ctor rfResWidth = this->AddParameter( _T("ResWidth"), &m_resWidth, pWidthValues );

	// Define supported resource width values
	CTestCaseParameter<UINT> *pBufStrideParam = AddParameter( _T( "BufStride" ), &m_bufStride );
	testfactor::RFactor rfBufStride32 = AddParameterValue<UINT>( pBufStrideParam, 32 );
	testfactor::RFactor rfBufStride16 = AddParameterValue<UINT>( pBufStrideParam, 16 );
	testfactor::RFactor rfBufStride8 = AddParameterValue<UINT>( pBufStrideParam, 8 );
	testfactor::RFactor rfBufStride4 = AddParameterValue<UINT>( pBufStrideParam, 4 );

	// Define supported destination address values
	CUserValueSet<AssemblyHelper::Swizzle>* const pAddressValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	pAddressValues->AddValue( g_pSwizzles[4] );	// x
	pAddressValues->AddValue( g_pSwizzles[5] );	// y
	pAddressValues->AddValue( g_pSwizzles[6] );	// z
	pAddressValues->AddValue( g_pSwizzles[7] );	// w
	testfactor::RFactor rfSrcAddressSwizzle = this->AddParameter( _T("SrcAddressSwizzle"), &m_srcAddressSwizzle, pAddressValues );

	// Define supported destination offset values
	CTestCaseParameter<ADDRESS_OFFSET> *pAddressOffsetParam = AddParameter( _T( "SrcOffset" ), &m_srcOffset );
	testfactor::RFactor rfAddressOffsetStart			= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START );
	testfactor::RFactor rfAddressOffsetStartPlus4Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START_PLUS_4_BYTES );
	testfactor::RFactor rfAddressOffsetStartPlus20Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_START_PLUS_20_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus4Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_4_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus8Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_8_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus12Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_12_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus16Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_16_BYTES );
	testfactor::RFactor rfAddressOffsetEndMinus20Bytes	= AddParameterValue<ADDRESS_OFFSET>( pAddressOffsetParam, ADDRESS_OFFSET_END_MINUS_20_BYTES );

	CUserValueSet<AssemblyHelper::Swizzle>* const pOffsetValuesSwizzle = new CUserValueSet<AssemblyHelper::Swizzle>();
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[4] );	// x
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[5] );	// y
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[6] );	// z
	pOffsetValuesSwizzle->AddValue( g_pSwizzles[7] );	// w
	testfactor::RFactor rfSrcOffsetSwizzle = this->AddParameter( _T("SrcOffsetSwizzle"), &m_srcOffsetSwizzle, pOffsetValuesSwizzle );

	// Define supported source swizzle values
	CUserValueSet<AssemblyHelper::Swizzle>* const pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>();
	for ( UINT i = 0; i < _countof( g_pSwizzles ); ++i )
	{
		pSwizzleValues->AddValue( g_pSwizzles[i] );
	}
	testfactor::RFactor rfSrcSwizzle = this->AddParameter( _T("SrcSwizzle"), &m_srcSwizzle, pSwizzleValues );

	// Define supported destination masks
	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDestMaskParam = AddParameter( _T( "DestMask" ), &m_destMask );
	testfactor::RFactor rfDestMaskXYZW = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[0] );
	testfactor::RFactor rfDestMaskXYZ = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[1] );
	testfactor::RFactor rfDestMaskXY = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[3] );
	testfactor::RFactor rfDestMaskX = AddParameterValue<AssemblyHelper::MaskFlag>( pDestMaskParam, g_pMasks[7] );

	// Define supported UAV slots
	CUserValueSet<UINT>* const pUAVSlots = new CUserValueSet<UINT>;
	UINT uUAVStartSlot = 0;
	UINT uUAVMaxSlot = 0;
	testfactor::RFactor rfUAVSlot;
	if( g_TestApp.IsFL11_1OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 63;	// 64 - 1
	}
	else if( g_TestApp.IsFL11OrGreater() )
	{
		uUAVStartSlot = 1;	// Skip slot 0 because RTV is in slot 0
		uUAVMaxSlot = 7;	// 8 - 1
	}
	else
	{
		// Only 1 UAV slot for FL10.X, CS only
		uUAVStartSlot = 0;	// No RTV in CS, so we use slot 0
		uUAVMaxSlot = 0;	// 1 - 1
	}
	rfUAVSlot = this->AddParameter( _T("UAVSlot"), &m_UAVSlot, new CRangeValueSet<UINT>(uUAVStartSlot, uUAVMaxSlot, 1) ); // Skip 0 because the PS uses slot 0 for the RTV


	// Win7:705720
	// Determine valid testcases for strides. Note that if the offset causes a out of bounds error, the UAV is undefined. Thus we don't test it.

	testfactor::RFactor rfDestMaskWrite1Component	= rfDestMaskX;
	testfactor::RFactor rfDestMaskWrite2Components	= rfDestMaskX + rfDestMaskXY;
	testfactor::RFactor rfDestMaskWrite3Components	= rfDestMaskX + rfDestMaskXY + rfDestMaskXYZ;
	testfactor::RFactor rfDestMaskWrite4Components	= rfDestMaskX + rfDestMaskXY + rfDestMaskXYZ + rfDestMaskXYZW;

	testfactor::RFactor rfStride4Test = rfBufStride4 *					// Stride of 4
										( rfAddressOffsetStart * rfDestMaskWrite1Component );					// No offset, only 1 component

	testfactor::RFactor rfStride8Test = rfBufStride8 * (				// Stride of 8
										( rfAddressOffsetStart * rfDestMaskWrite2Components ) +					// No offset, up to 2 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite1Component ) );		// 4Byte offset, only 1 component

	testfactor::RFactor rfStride16Test = rfBufStride16 * (				// Stride of 16
										( rfAddressOffsetStart * rfDestMaskWrite4Components ) +					// No offset, up to 4 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite3Components ) +		// 4Byte offset, up to 3 components

										( rfAddressOffsetEndMinus4Bytes * rfDestMaskWrite1Component ) +			// 4Byte offset from end, only 1 component
										( rfAddressOffsetEndMinus8Bytes * rfDestMaskWrite2Components ) );		// 8Byte offset from end, up to 2 components

	testfactor::RFactor rfStride32Test = rfBufStride32 * (				// Stride of 32
										( rfAddressOffsetStart * rfDestMaskWrite4Components ) +					// No offset, up to 4 components
										( rfAddressOffsetStartPlus4Bytes * rfDestMaskWrite3Components ) +		// 4Byte offset, up to 4 components
										( rfAddressOffsetStartPlus20Bytes * rfDestMaskWrite3Components ) +		// 20Byte offset, up to 3 components

										( rfAddressOffsetEndMinus4Bytes * rfDestMaskWrite1Component ) +			// 4Byte offset from end, only 1 component
										( rfAddressOffsetEndMinus8Bytes * rfDestMaskWrite2Components ) +		// 8Byte offset from end, up to 2 components
										( rfAddressOffsetEndMinus12Bytes * rfDestMaskWrite3Components ) +		// 12Byte offset from end, up to 3 components
										( rfAddressOffsetEndMinus16Bytes * rfDestMaskWrite4Components ) +		// 16Byte offset from end, up to 4 components
										( rfAddressOffsetEndMinus20Bytes * rfDestMaskWrite4Components ) );		// 20Byte offset from end, up to 4 components

	testfactor::RFactor rfResWidthStridesTests = ( rfResWidth * ( rfStride4Test + rfStride8Test + rfStride16Test + rfStride32Test ) );
	testfactor::RFactor rfSwizzles = rfSrcSwizzle * rfSrcAddressSwizzle * rfSrcOffsetSwizzle;

	// Set the test root factor
	testfactor::RFactor rfStageFactor;
	if ( g_TestApp.IsFL11_1OrGreater() )
	{
		rfStageFactor = rfNonComputeShaders + (rfComputeShader * rfUseTGSM);
	}
	else if ( g_TestApp.IsFL11OrGreater() )
	{
		rfStageFactor = rfPixelShader + (rfComputeShader * rfUseTGSM);
	}
	else
	{
		rfStageFactor = rfComputeShader;
	}

	this->SetRootTestFactor( ( rfSwizzles * rfStageFactor ) % rfResWidthStridesTests % rfUAVSlot );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ResWidth"), MAX_WIDTH * 2), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), g_pMasks[0]), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//-----------------------------------------------------------------------------
TEST_RESULT
CStore_structured::ExecuteTestCase()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();

	const UINT uiCbStride = sizeof( VSINPUT );
	const UINT uiOffset = 0;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_PIXEL:

		if ( !this->InitPipelineShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitPipelineShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->RSSetState( m_pRSEnabled );

		switch( m_ShaderStage )
		{
		default:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			break;
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
			pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			break;
		}

		pContext->IASetVertexBuffers( 0, 1, &m_pVsInput, &uiCbStride, &uiOffset );
		pContext->IASetInputLayout( m_pInputLayout );
		pContext->Draw( NUM_VERTICES, 0 );
		break;

	case D3D_SHADER_STAGE_COMPUTE:

		if ( !this->InitCSShaders() )
		{
			WriteError( _T( "CStore_uav_typed::InitCSShaders() failed" ) );
			return RESULT_FAIL;
		}

		pContext->CSSetShaderResources( 0, 1, &m_pCsInputSRV );
		pContext->CSSetUnorderedAccessViews( m_UAVSlot, 1, &m_pUAV, NULL );
		pContext->CSSetShader( m_pCS, NULL, 0 );
		pContext->Dispatch( DISP_WIDTH, DISP_HEIGHT, 1 );
		break;
	}

	hr = this->ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog( _T("this->ExecuteEffectiveContext() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	return this->AnalyzeResult() ? RESULT_PASS : RESULT_FAIL;
}

//-----------------------------------------------------------------------------
UINT
CStore_structured::GetSrcStructureOffset()
{
	// Description: This function simply returns the number of bytes the offset should be in the struct.
	//				It asserts that the offset is within the stride to ensure the UAV doesn't become undefined, as the spec describes.

	// Local variables
	int offset = 0;

	switch( m_srcOffset )
	{
	case ADDRESS_OFFSET_START:
		offset = 0;
		break;
	case ADDRESS_OFFSET_START_PLUS_4_BYTES:
		offset = 4;
		break;
	case ADDRESS_OFFSET_START_PLUS_8_BYTES:
		offset = 8;
		break;
	case ADDRESS_OFFSET_START_PLUS_12_BYTES:
		offset = 12;
		break;
	case ADDRESS_OFFSET_START_PLUS_16_BYTES:
		offset = 16;
		break;
	case ADDRESS_OFFSET_START_PLUS_20_BYTES:
		offset = 20;
		break;

	case ADDRESS_OFFSET_END_MINUS_4_BYTES:
		offset = m_bufStride - 4;
		break;
	case ADDRESS_OFFSET_END_MINUS_8_BYTES:
		offset = m_bufStride - 8;
		break;
	case ADDRESS_OFFSET_END_MINUS_12_BYTES:
		offset = m_bufStride - 12;
		break;
	case ADDRESS_OFFSET_END_MINUS_16_BYTES:
		offset = m_bufStride - 16;
		break;
	case ADDRESS_OFFSET_END_MINUS_20_BYTES:
		offset = m_bufStride - 20;
		break;
	}

	// Validate the offset is correct. If we go outside this range the UAV becomes undefined and we shouldn't test it.
	// Note: This assert doesn't take into account the numer of components to write.
	assert( offset >= 0 || offset < (int)m_bufStride );
	
	return (UINT)offset;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::InitPipelineShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );
	const tstring strSrcOffsetSwizzle = ToString( m_srcOffsetSwizzle, 1 );

	// Build the structure offset based on the stride so we keep the UAV valid
	const UINT srcOffset = GetSrcStructureOffset();

	// Create the string to init the offset register (based on the swizzle used in the test)
	tstring strSrcOffset = "vec4(%d, %d, %d, %d)";

	if( "x" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), srcOffset, 0, 0, 0 );
	}
	else if( "y" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, srcOffset, 0, 0 );
	}
	else if( "z" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, srcOffset, 0 );
	}
	else if( "w" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, 0, srcOffset );
	}
	else
	{
		// Should never get here
		assert(false);
	}

	AssemblyHelper asmHelper( 0, 4 );

	asmHelper.SetCustomUAVDecl( m_UAVSlot, FormatString( "dcl_uav_structured u%d, %d", m_UAVSlot, m_bufStride ) );

	// Position is in V[0], Address is in V[1], Offset is in V[2].
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetUserInputRegMaskTyped( 1, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetUserInputRegMaskTyped( 2, AssemblyHelper::xyzw, "linear", "float" );
	asmHelper.SetupMovToTempFromLiteral(3, AssemblyHelper::xyzw, strSrcOffset);

	if( m_ShaderStage != D3D_SHADER_STAGE_PIXEL )
	{
		asmHelper.SetPositionOutputRegIdx(5);
		asmHelper.SetupMovToOutputFromInput( 5, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );	// Move position from 0 to 5
	}

	asmHelper.SetOutputRegMaskTyped( 0, AssemblyHelper::xyzw, "float" );

	tstring strShaderCode = "";

	switch( m_ShaderStage )
	{
	default:
		strShaderCode += "ftou r1, v[1]\n";
		strShaderCode += "ftou r2, v[2]\n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		strShaderCode += "ftou r1, v[0][1]\n";
		strShaderCode += "ftou r2, v[0][2]\n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		strShaderCode += "ftou r1, vcp[0][1]\n";
		strShaderCode += "ftou r2, vcp[0][2]\n";
		break;
	}

	strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
	strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
	strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r3.%s, r0.%s",
		m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::InitCSShaders()
{
	HRESULT hr;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	ID3D11Device* const pDevice = this->GetDevice();

	const tstring strDestMask	= ToString( m_destMask );
	const tstring strSrcSwizzle	= ToString( m_srcSwizzle );
	const tstring strSrcAddressSwizzle = ToString( m_srcAddressSwizzle, 1 );
	const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );

	// Build the structure offset based on the stride so we keep the UAV valid
	const UINT srcOffset = GetSrcStructureOffset();

	// Create the string to init the offset register (based on the swizzle used in the test)
	tstring strSrcOffset = "vec4(%d, %d, %d, %d)";

	if( "x" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), srcOffset, 0, 0, 0 );
	}
	else if( "y" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, srcOffset, 0, 0 );
	}
	else if( "z" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, srcOffset, 0 );
	}
	else if( "w" == strSrcOffsetSwizzle )
	{
		strSrcOffset = FormatString( strSrcOffset.c_str(), 0, 0, 0, srcOffset );
	}
	else
	{
		// Should never get here
		assert(false);
	}

	AssemblyHelper asmHelper( 0, 5 );
	asmHelper.SetCustomDecl( 0, FormatString( "dcl_resource_structured t%d, %d", 0, sizeof( CSINPUT ) ) );
	asmHelper.SetCustomUAVDecl( 0, FormatString( "dcl_uav_structured u%d, %d", m_UAVSlot, m_bufStride ) );

	if( m_bUseTGSM )
		asmHelper.SetCustomDecl( 1, FormatString( "dcl_tgsm_structured g0, %d, %d\n", m_bufStride, m_resWidth ) );

	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, strSrcOffset);
	tstring strShaderCode = "";

	if ( m_bUseTGSM )
	{
		//strShaderCode += FormatString( "dcl_tgsm_structured g0, %d, %d\n", m_bufStride, m_resWidth );
		strShaderCode += FormatString( "imad r0, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r1, r0.x, 0, t0\n";
		strShaderCode += "ld_structured r2, r0.x, 16, t0\n";
		strShaderCode += "ftou r1, r1\n";
		strShaderCode += "ftou r2, r2\n";
		strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
		strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured g0.%s, r1.%s, r4.%s, r0.%s\n",
			strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );
		strShaderCode += "sync_uglobal_g\n";
		strShaderCode += FormatString( "ld_structured r3, r1.%s, r4.%s, g0\n", strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r4.%s, r3",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str() );
	}
	else
	{
		strShaderCode += FormatString( "imad r0, vThreadGroupID.y, vec4(%d, 0, 0, 0), vThreadGroupID.x\n", DISP_WIDTH );
		strShaderCode += "ld_structured r1, r0.x, 0, t0\n";
		strShaderCode += "ld_structured r2, r0.x, 16, t0\n";
		strShaderCode += "ftou r1, r1\n";
		strShaderCode += "ftou r2, r2\n";
		strShaderCode += "imul r0, r2, r2, vec4(4, 4, 4, 4)\n";
		strShaderCode += FormatString( "iadd r0, r1.%s, vec4(8, 16, 24, 32)\n", strSrcAddressSwizzle.c_str() );
		strShaderCode += FormatString( "store_structured u%d.%s, r1.%s, r4.%s, r0.%s",
			m_UAVSlot, strDestMask.c_str(), strSrcAddressSwizzle.c_str(), strSrcOffsetSwizzle.c_str(), strSrcSwizzle.c_str() );
	}

	ID3D10Blob* pShaderBuf = NULL;
	hr = asmHelper.SetupPipelineWithShaderCode( pDevice,
		                                        pContext,
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												strShaderCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );
	if ( FAILED( hr ) )
	{
		WriteError( _T("AssemblyHelper::SetupPipelineWithShaderCode() failed, hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if ( g_bDumpDissasm )
	{
		this->MessageDumpShaderDisassembly( (UINT*)pShaderBuf->GetBufferPointer(),
			                                pShaderBuf->GetBufferSize() );
	}

	SAFE_RELEASE( pShaderBuf );

	return true;
}

//-----------------------------------------------------------------------------
bool
CStore_structured::AnalyzeResult()
{
	HRESULT hr;

	bool bResult = true;

	ID3D11DeviceContext* const pContext = this->GetEffectiveContext();
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const UINT numFailuresToLog = NUM_FAILURES_TO_LOG;
	UINT numFailuresLogged = 0;

	int srcSwizzleIndices[4];
	m_srcSwizzle.ToIndices( srcSwizzleIndices );

	int srcAddressIndices[4];
	m_srcAddressSwizzle.ToIndices( srcAddressIndices );

	int srcOffsetIndices[4];
	m_srcOffsetSwizzle.ToIndices( srcOffsetIndices );

	UINT width	= 1;
	UINT height	= 1;
	UINT depth	= 1;

	float tx0 = 0.0f - m_outOfBoundsOffset;
	float tx1 = 0.0f + m_outOfBoundsOffset;
	float ty0 = 0.0f - m_outOfBoundsOffset;
	float ty1 = 0.0f + m_outOfBoundsOffset;
	float tz0 = 0.0f - m_outOfBoundsOffset;
	float tz1 = 0.0f + m_outOfBoundsOffset;

	ID3D11Resource* pResource;
	ID3D11Resource* pResourceCopy;

	switch ( m_resourceType )
	{
	case RESOURCE_BUFFER:
		width = m_resWidth;
		tx1 = (float)width + m_outOfBoundsOffset;
		pResource = m_pBuffer;
		pResourceCopy = m_pBufferCopy;
		break;

	default:
		WriteToLog( _T("CStore_uav_typed::AnalyzeResult() failed, invalid resource type.") );
		return false;
	}

	const UINT uiCbRefBufSize = width * m_bufStride;

	BYTE* const pbRefBuffer = new BYTE[uiCbRefBufSize];
	memset( pbRefBuffer, 0, uiCbRefBufSize );

	const UINT uiXCount = DISP_WIDTH;
	const UINT uiYCount = DISP_HEIGHT;

	const float fdtx = ( tx1 - tx0 ) / uiXCount;
	const float fdty = ( ty1 - ty0 ) / uiYCount;
	const float fdtz = ( tz1 - tz0 ) / uiXCount;

	const float fdtx0 = fdtx / 2;
	const float fdty0 = fdty / 2;
	const float fdtz0 = fdtz / 2;

	const float fdOffsetR = ( 3.0f - 0.0f ) / uiXCount;
	const float fdOffsetG = ( 6.0f - 0.0f ) / uiXCount;
	const float fdOffsetB = ( 9.0f - 0.0f ) / uiXCount;
	const float fdOffsetA = ( 12.0f - 0.0f ) / uiXCount;

	const float fdOffsetR0 = fdOffsetR / 2;
	const float fdOffsetG0 = fdOffsetG / 2;
	const float fdOffsetB0 = fdOffsetB / 2;
	const float fdOffsetA0 = fdOffsetA / 2;

	const UINT uiCbStride = sizeof( UINT );

	UINT uiCbCopySize = uiCbStride;

	switch ( m_destMask )
	{
	case AssemblyHelper::x:
		uiCbCopySize *= 1;
		break;

	case AssemblyHelper::xy:
		uiCbCopySize *= 2;
		break;

	case AssemblyHelper::xyz:
		uiCbCopySize *= 3;
		break;

	case AssemblyHelper::xyzw:
		uiCbCopySize *= 4;
		break;
	}

	float ty = ty0;

	// The VS etc, only use four verticies and point list, so only the corners will light up
	const bool bOnlyCheckCorners = m_bRenderUsingPointlist ? true : false;
	if( m_bRenderUsingPointlist )
	{
		assert( NUM_VERTICES == 4 );
		float vertexTexCoords[NUM_VERTICES][4] = {
		// Address
		{ tx0, ty1, tz0, 0.0f },
		{ tx0, ty0, tz0, 0.0f },
		{ tx1, ty1, tz1, 0.0f },
		{ tx1, ty0, tz1, 0.0f }};

		for( UINT v = 0; v < NUM_VERTICES; v++ )
		{
			UINT data[4];

			//
			// Build the offset position
			//

			UINT offsets[4] = { 0, 0, 0, 0 };
			tstring components[] = { "x", "y", "z", "w" };
			const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );
			const UINT srcOffset = GetSrcStructureOffset();

			for( UINT i = 0; i < 4; ++i )
			{
				if( components[i] == strSrcOffsetSwizzle )
					offsets[i] = srcOffset;
			}

			const UINT offset = offsets[ srcOffsetIndices[0] ];

			//
			// Build the address location
			//

			float fU = vertexTexCoords[v][ 0 ];
			const UINT u = (fU < 0) ? 0 : (UINT)fU;	// FTOU: Inputs are clamped to the range [0.0f ... 4294967295.999f] prior to conversion

			UINT uiCbAddress = 0xffffffff;

			switch ( m_resourceType )
			{
			case RESOURCE_BUFFER:
				if ( u < width &&
						offset < m_bufStride )
				{
					uiCbAddress = u * m_bufStride + offset;

					data[0] = u + 8;
					data[1] = u + 16;
					data[2] = u + 24;
					data[3] = u + 32;
				}
				break;
			}

			if ( 0xffffffff != uiCbAddress )
			{
				UINT _data[4];

				// Apply source swizzle
				_data[0] = data[ srcSwizzleIndices[0] ];
				_data[1] = data[ srcSwizzleIndices[1] ];
				_data[2] = data[ srcSwizzleIndices[2] ];
				_data[3] = data[ srcSwizzleIndices[3] ];

				BYTE* const pbRefColor = pbRefBuffer + uiCbAddress;
				const UINT bytesLeft = m_bufStride - offset;
				memcpy( pbRefColor, _data, min( uiCbCopySize, bytesLeft ) );
			}
		}
	}
	else
	{
		for ( UINT y = 0; y < uiYCount; ++y )
		{
			float tx = tx0;
			float tz = tz0;

			float fOffsetR = 0.0f;
			float fOffsetG = 0.0f;
			float fOffsetB = 0.0f;
			float fOffsetA = 0.0f;

			for ( UINT x = 0; x < uiXCount; ++x )
			{
				UINT data[4];

				//
				// Build the offset position
				//

				UINT offsets[4] = { 0, 0, 0, 0 };
				tstring components[] = { "x", "y", "z", "w" };
				const tstring strSrcOffsetSwizzle  = ToString( m_srcOffsetSwizzle, 1 );
				const UINT srcOffset = GetSrcStructureOffset();

				for( UINT i = 0; i < 4; ++i )
				{
					if( components[i] == strSrcOffsetSwizzle )
						offsets[i] = srcOffset;
				}

				const UINT offset = offsets[ srcOffsetIndices[0] ];

				//
				// Build the address location
				//

				const float address[4] = { tx + fdtx0,
										   ty + fdty0,
										   tz + fdtz0,
										   0.0f };

				// texCords[4] contains post-swizzle addresses
				const UINT u = (UINT)address[ 0 ];


				UINT uiCbAddress = 0xffffffff;

				switch ( m_resourceType )
				{
				case RESOURCE_BUFFER:
					if ( u < width &&
						 offset < m_bufStride )
					{
						uiCbAddress = u * m_bufStride + offset;

						data[0] = u + 8;
						data[1] = u + 16;
						data[2] = u + 24;
						data[3] = u + 32;
					}
					break;
				}

				if ( 0xffffffff != uiCbAddress )
				{
					UINT _data[4];

					// Apply source swizzle
					_data[0] = data[ srcSwizzleIndices[0] ];
					_data[1] = data[ srcSwizzleIndices[1] ];
					_data[2] = data[ srcSwizzleIndices[2] ];
					_data[3] = data[ srcSwizzleIndices[3] ];

					BYTE* const pbRefColor = pbRefBuffer + uiCbAddress;
					const UINT bytesLeft = m_bufStride - offset;
					memcpy( pbRefColor, _data, min( uiCbCopySize, bytesLeft ) );
				}

				tx += fdtx;
				tz += fdtz;

				fOffsetR += fdOffsetR;
				fOffsetG += fdOffsetG;
				fOffsetB += fdOffsetB;
				fOffsetA += fdOffsetA;
			}

			ty += fdty;
		}
	}

	//
	// Validate the results
	//

	D3D11_MAPPED_SUBRESOURCE texmap;
	pContext->CopySubresourceRegion( pResourceCopy, 0, 0, 0, 0, pResource, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("ExecuteEffectiveContext() failed") );
		return false;
	}

	hr = GetImmediateContext()->Map( pResourceCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if ( FAILED( hr ) )
	{
		delete [] pbRefBuffer;
		WriteToLog( _T("ID3D11DeviceContext::Map() failed, hr = %s"), D3DHResultToString( hr ).c_str() );
		return false;
	}

	for ( UINT x = 0, n = ( width * m_bufStride ) / uiCbStride; x < n; ++x )
	{
		// Load/Convert result color
		BYTE* const pbResultColor = (BYTE*)texmap.pData + x * uiCbStride;

		// Load/Convert expected color
		BYTE* const pbRefColor = pbRefBuffer + x * uiCbStride;

		UINT resultData;
		memcpy( &resultData, pbResultColor, uiCbStride );

		UINT expectedData;
		memcpy( &expectedData, pbRefColor, uiCbStride );

		// Compare the result
		if ( expectedData != resultData )
		{
			bResult = false;

			if ( bLogVerbose || (numFailuresLogged < numFailuresToLog) )
			{
				WriteToLog( _T( "CStore_structured::AnalyzeResult() failed at pixel (%d), is different. \tExpected value is %d \tOutput value is %d" ), x, expectedData, resultData );
				numFailuresLogged++;
			}
			else if( !bLogVerbose && (numFailuresLogged == numFailuresToLog) )
			{
				// We will only hit this message once
				WriteToLog( _T( "CStore_structured::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
				goto Cleanup;
			}
		}
	}

	Cleanup:
	delete [] pbRefBuffer;
	GetImmediateContext()->Unmap( pResourceCopy, 0 );

	return bResult;
}

} // namespace LoadStoreTest
//////////////////////////////////////////////////////////////////////
// File:  CImmersiveWindowWrapper.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for an immersive window thread object.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXSceneImmersiveWindow.h.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// WinRT headers
//

#include <corewindowp.h>
#include <windows.ui.core.corewindow.h>
#include <CoreWindow.h>
#include <Propsys.h>
#include <Propkey.h>
#include <shellapi.h>
#include <wrl\event.h>
#include <windowsstringp.h>


//
// Project headers
//

#include "RenderingTypes.hpp"
#include "GraphicsTAEFTest.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Helper class interfaces
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Class:  CImmersiveWindowWrapper
//
// Purpose:
// Wrapper object for managing a core window and a thread
// to launch it and pump messages for it.
////////////////////////////////////////////////////////////
class CImmersiveWindowWrapper
{
	//
	// Construction/destruction
	//

	public:
		// Default constructor.
		CImmersiveWindowWrapper();

		// Constructor that takes window width and height.
		CImmersiveWindowWrapper
		(
			UINT WindowWidth,
			UINT WindowHeight
		);

		// Constructor that takes window rectangle origin and size.
		CImmersiveWindowWrapper
		(
			LONG WindowLeft,
			LONG WindowTop,
			UINT WindowWidth,
			UINT WindowHeight
		);


		// Destructor.
		~CImmersiveWindowWrapper();


	//
	// Public accessor methods
	//

	public:
		Windows::UI::Core::ICoreWindow * GetImmersiveWindow();


		void SetLogger
		(
			_In_ ISimpleLog * pLogger
		);


		CSceneRenderer * GetSceneRenderer();

		void SetSceneRenderer
		(
			_In_ CSceneRenderer * pSceneRenderer
		);


	//
	// Public methods
	//

	public:
		HRESULT StartImmersiveWindowThread();

		HRESULT StopImmersiveWindowThread();


	//
	// Static methods
	//

	private:
		static DWORD WINAPI ThreadProc
		(
			_In_ void * pParam
		);


	//
	// Event handlers
	//

	private:
		HRESULT OnWindowSizeChanged
		(
			_In_ Windows::UI::Core::ICoreWindow * sender,
			_In_ Windows::UI::Core::IWindowSizeChangedEventArgs * e
		);


		HRESULT OnWindowClosed
		(
			_In_ Windows::UI::Core::ICoreWindow * sender,
			_In_ Windows::UI::Core::ICoreWindowEventArgs * e
		);


	//
	// Helper methods
	//

	private:
		HRESULT CreateImmersiveWindow();

		void DestroyImmersiveWindow();


		HRESULT NotifyWindowResizeManagerOfLayoutCompletion();


		HRESULT RecreateSizeDependentResources
		(
			UINT NewWidth,
			UINT NewHeight
		);


		HRESULT RenderAndProcessWindowEvents();


		HRESULT SetAppUserModelID();


		HRESULT SetImmersiveWindowCursor();


	//
	// Member data
	//

	private:
		ISimpleLog * m_pLogger;

		LONG m_WindowLeft;
		LONG m_WindowTop;

		UINT m_WindowWidth;
		UINT m_WindowHeight;

		Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreWindow > m_CoreWindow;
		Microsoft::WRL::ComPtr< Windows::UI::Core::ICoreDispatcher > m_CoreDispatcher;

		EventRegistrationToken m_WindowSizeChangedToken;
		EventRegistrationToken m_WindowClosedToken;

		HANDLE m_hWindowThread;

		HANDLE m_hWindowLaunchedEvent;
		HANDLE m_hWindowClosedEvent;

		CSceneRenderer * m_pSceneRenderer;
};
#include "RenderTargets.h"
#include "Clear.h"
#include "Direct3DFailureCategoryStrings.h"

#include <WGFTestCore.h>

using namespace WindowsTest;
using namespace WindowsTest::Graphics;

CRenderTargetsTestApp g_TestApp;

FLOAT_COLOR CRenderTargetsTest::m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ] = 
{
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.5f, 0.f, 0.5f },
		{ -23.f, 0.f, 1.f, 0.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	}
};

bool WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

bool GetScenarioFlags( D3D_RESOURCE_SCENARIO scenario, UINT *pFlags )
{
	if( pFlags == NULL )
		return false;

	*pFlags = 0;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pFlags |= D3D_RS_FLAGS_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pFlags = (*pFlags | D3D_RS_FLAGS_MS | D3D_RS_FLAGS_TEXTURE2D);
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pFlags |= D3D_RS_FLAGS_TEXTURECUBE;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_SRV_DIMENSION *pSRVDim )
{
	if( !pSRVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pSRVDim = D3D11_SRV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURECUBE;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_RTV_DIMENSION *pRTVDim )
{
	if( !pRTVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pRTVDim = D3D11_RTV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE3D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		return false;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_DSV_DIMENSION *pDSVDim )
{
	if( !pDSVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_BUFFER:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		return false;
	}

	return true;
}

tstring __stdcall ToString( const D3D_RESOURCE_SCENARIO &scenario )
{
	tstring res;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		res = _T( "BUFFER" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		res = _T( "TEXTURE1D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		res = _T( "TEXTURE1D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		res = _T( "TEXTURE2D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		res = _T( "TEXTURE2D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		res = _T( "TEXTURE2D_MS" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		res = _T( "TEXTURE2D_MS_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		res = _T( "TEXTURECUBE" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		res = _T( "TEXTURECUBE_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		res = _T( "TEXTURE3D" );
		break;
	}

	return res;
}

bool __stdcall FromString( D3D_RESOURCE_SCENARIO *pScenario, const tstring &str )
{
	if( str == _T( "BUFFER" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_BUFFER;
	else if( str == _T( "TEXTURE1D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D;
	else if( str == _T( "TEXTURE1D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY;
	else if( str == _T( "TEXTURE2D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D;
	else if( str == _T( "TEXTURE2D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY;
	else if( str == _T( "TEXTURE2D_MS" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS;
	else if( str == _T( "TEXTURE2D_MS_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY;
	else if( str == _T( "TEXTURECUBE" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	else if( str == _T( "TEXTURECUBE_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY;
	else if( str == _T( "TEXTURE3D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE3D;
	else
		return false;

	return true;
}


tstring __stdcall ToString( const IMAGE_PATTERN &pattern )
{
	tstring res;
	switch( pattern )
	{
	case IP_STRIPES:
		res = _T( "Stripes" );
		break;
	case IP_GRADIENT:
		res = _T( "Gradient" );
		break;
	case IP_TILES:
		res = _T( "Tiles" );
		break;
	default:
		res = _T( "<unknown" );
	}

	return res;
}

//---------------------------------------------------------------

bool __stdcall FromString( IMAGE_PATTERN *pPattern, const tstring &str )
{
	if( str == _T( "Stripes" ) )
		*pPattern = IP_STRIPES;
	else if( str == _T( "Gradient" ) )
		*pPattern = IP_GRADIENT;
	else if( str == _T( "Tiles" ) )
		*pPattern = IP_TILES;
	else
		return false;

	return true;
}

void CRenderTargetsTest::InitTestParameters()
{
	CTestCaseParameter<IMAGE_PATTERN> *pPatternParam = AddParameter( _T( "ImagePattern" ), &m_ImagePattern );
	testfactor::RFactor rfStripes = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_STRIPES );
	testfactor::RFactor rfGradient = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_GRADIENT );
	testfactor::RFactor rfTiles = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_TILES );

	CTestCaseParameter<UINT32> *pNumStripesParam = AddParameter( _T( "NumStripes" ), &m_uNumStripes );
	CUserValueSet<UINT32> *pNumStripesValues = new CUserValueSet<UINT32>( );
	pNumStripesValues->AddValue( 0 ); // to use current width/height
	pNumStripesValues->AddValue( 1 );
	pNumStripesValues->AddValue( 5 );
	testfactor::RFactor rfNumStripes = AddParameterValueSet( pNumStripesParam, pNumStripesValues );

	CTestCaseParameter<bool> *pVertStripesParam = AddParameter( _T( "VerticalStripes" ), &m_bVertStripes );
	testfactor::RFactor rfVertStripes = AddParameterValueSet( pVertStripesParam, BoolValueSet( ) );

	CTestCaseParameter<UINT32> *pTileParam = AddParameter( _T( "TileSize" ), &m_uTileSize );
	CUserValueSet<UINT32> *pTileValues = new CUserValueSet<UINT32>( );
	pTileValues->AddValue( 1 );
	pTileValues->AddValue( 3 );
	pTileValues->AddValue( 4 );
	pTileValues->AddValue( 16 );
	testfactor::RFactor rfTileSize = AddParameterValueSet( pTileParam, pTileValues );
	testfactor::RFactor rfTileSizeOne = AddParameterValue( pTileParam, (UINT32) 1 );

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 64 );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( 45 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 64 );
	pHeightValues->AddValue( 128 );
	pHeightValues->AddValue( 45 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	CTestCaseParameter<UINT32> *pDepthParam = AddParameter( _T( "Depth" ), &m_uDepth );
	CUserValueSet<UINT32> *pDepthValues = new CUserValueSet<UINT32>( );
	pDepthValues->AddValue( 32 );
	pDepthValues->AddValue( 16 );
	pDepthValues->AddValue( 1 );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceType" ), &m_ResourceType );
	testfactor::RFactor rfBuffer = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_BUFFER );
	testfactor::RFactor rfTex1D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	testfactor::RFactor rfTex2D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfTex3D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE3D );
	testfactor::RFactor rfTexCube = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURECUBE );

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format" ), &m_Format );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) );
	testfactor::RFactor rfFormats = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, FormatValueSet( formatFilter ) );

	CUserValueSet<UINT32> *pSliceValues = new CUserValueSet<UINT32>( );
	pSliceValues->AddValue( 0 );
	pSliceValues->AddValue( 1 );
	pSliceValues->AddValue( 2 );
	
	CTestCaseParameter<UINT32> *pMipLevelParam = AddParameter( _T( "MipLevel" ), &m_uMipLevel );
	testfactor::RFactor rfMipLevel = AddParameterValueSet<UINT32>( pMipLevelParam, pSliceValues );
	testfactor::RFactor rfMipDef = AddParameterValue<UINT32>( pMipLevelParam, 0 );

	CTestCaseParameter<D3D11_BIND_FLAG> *pBindParam = AddParameter( _T( "AdditionalBind" ), &m_BindFlag );
	CUserValueSet<D3D11_BIND_FLAG> *pBindValues = new CUserValueSet<D3D11_BIND_FLAG>( );
	pBindValues->AddValue( D3D11_BIND_RENDER_TARGET );
	pBindValues->AddValue( D3D11_BIND_VERTEX_BUFFER );
	pBindValues->AddValue( D3D11_BIND_INDEX_BUFFER );
	pBindValues->AddValue( D3D11_BIND_SHADER_RESOURCE );
	testfactor::RFactor rfBind = AddParameterValueSet<D3D11_BIND_FLAG>( pBindParam, pBindValues );

	CUserValueSet<D3D11_BIND_FLAG> *pBindValuesTex = new CUserValueSet<D3D11_BIND_FLAG>();
	pBindValuesTex->AddValue( D3D11_BIND_RENDER_TARGET );
	pBindValuesTex->AddValue( D3D11_BIND_SHADER_RESOURCE );
	testfactor::RFactor rfBindTex = AddParameterValueSet<D3D11_BIND_FLAG>( pBindParam, pBindValuesTex );

	CTestCaseParameter<UINT32> *pRTIndexParam = AddParameter( _T( "RTIndex" ), &m_uRTIndex );
	CUserValueSet<UINT32> *pRTIndexValues = new CUserValueSet<UINT32>( );
	pRTIndexValues->AddValue( 0 );
	pRTIndexValues->AddValue( 1 );
	pRTIndexValues->AddValue( 5 );
	pRTIndexValues->AddValue( 15 );
	testfactor::RFactor rfRTIndex = AddParameterValueSet<UINT32>( pRTIndexParam, pRTIndexValues );
	testfactor::RFactor rfRTIndexDef = AddParameterValue<UINT32>( pRTIndexParam, 0 );

	CTestCaseParameter<bool> *pCubeArrayParam = AddParameter( _T( "CubeArray" ), &m_bCubeArray );
	testfactor::RFactor rfCubeArray = AddParameterValueSet<bool>( pCubeArrayParam, BoolValueSet() );
	
	SetParameterDefaultValue<D3D11_BIND_FLAG>( pBindParam, D3D11_BIND_RENDER_TARGET );
	SetParameterDefaultValue<bool>( pCubeArrayParam, false );

	SetRootTestFactor( 
		( rfFormats * rfBuffer * rfTiles * rfTileSizeOne * rfWidth * rfBind * rfRTIndexDef ) +
		( rfFormats * rfTex1D * rfMipLevel * rfTiles * rfTileSizeOne * rfWidth * rfBindTex * rfRTIndex ) +
		( rfFormats * rfTex2D * rfMipLevel * rfTiles * rfTileSize * (rfWidth % rfHeight) * rfBindTex * rfRTIndex ) +
		( rfFormats * rfTex2D * rfMipLevel * rfStripes * rfNumStripes * rfVertStripes * (rfWidth % rfHeight) * rfRTIndex) +
		( rfFormats * rfTex2D * rfMipLevel * rfGradient * (rfWidth % rfHeight) * rfRTIndex ) +
		( rfFormats * rfTex3D * rfMipLevel * rfTiles * rfTileSize * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTex3D * rfMipLevel * rfStripes * rfNumStripes * rfVertStripes * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTex3D * rfMipLevel * rfGradient * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTexCube * rfCubeArray * rfTiles * rfTileSize * rfWidth * rfMipDef * rfRTIndex ) +
		( rfFormats * rfTexCube * rfCubeArray * rfStripes * rfNumStripes * rfVertStripes * rfWidth * rfMipDef * rfRTIndex ) +
		( rfFormats * rfTexCube * rfCubeArray * rfGradient * rfWidth * rfMipDef * rfRTIndex ) 
		);	
	
	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D), WeightFactorVeryLarge);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<IMAGE_PATTERN>(pPatternParam, IP_GRADIENT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(pWidthParam, 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(pHeightParam, 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT32>(pRTIndexParam, 0), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 4.5f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );

}

//---------------------------------------------------------------

TEST_RESULT CRenderTargetsTest::Setup( )
{
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = CD3D11Test::Setup();
	if( tr != RESULT_PASS )
		return tr;

	// Check what feature level we're running at
	m_bFL9X = GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0;

	//
	// Get the adapter descriptor and WDDM version.
	//

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( GetDevice() );
	if( nullptr == m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"GetAdapterDescriptor returned null.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported );
		return RESULT_FAIL;
	}

	// Get the WDDM version
	if ( GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
	{
		// Ref doesn't support the GetDisplayDriverModel() call
		m_wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;
	}
	else
	{
		if( FAILED( hr = m_pAdapterDescriptor->GetDisplayDriverModel( m_wddmDriverVersion ) ) )
		{
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
			return RESULT_FAIL;
		}
	}

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	D3D11_BUFFER_DESC desc = { 0 };
	desc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	//desc.ByteWidth set below based on the feature level
	desc.CPUAccessFlags = 0;
	desc.MiscFlags		= 0;
	desc.Usage			= D3D11_USAGE_DEFAULT;
	if( m_bFL9X )
	{
		// feature level 9 doesn't support the GS, so we'll render a quad
		desc.ByteWidth = sizeof( TLVERTEX ) * 4;

		TLVERTEX vertices[4] = 
		{
			{ {1.f,1.f},		1.f, 0.f, 0.f, 0.f },
			{ {1.f,-1.f},		1.f, 1.f, 0.f, 0.f },
			{ {-1.f,1.f},		0.f, 0.f, 0.f, 0.f },
			{ {-1.f,-1.f},		0.f, 1.f, 0.f, 0.f },
		};

		assert( sizeof(vertices) == sizeof(m_pVertexData) );
		ZeroMemory( m_pVertexData, sizeof(m_pVertexData) );
		memcpy( m_pVertexData, vertices, sizeof( vertices ) );

		initData.SysMemPitch = sizeof( TLVERTEX ) * 4;
		initData.SysMemSlicePitch = sizeof( TLVERTEX ) * 4;
		initData.pSysMem = m_pVertexData;
	}
	else
	{
		// create one vertex, actual quad will be made in GS according to cycled dimensions
		desc.ByteWidth = sizeof( TLVERTEX );
		
		ZeroMemory( m_pVertexData, sizeof(m_pVertexData) );

		initData.SysMemPitch = sizeof( TLVERTEX );
		initData.SysMemSlicePitch = sizeof( TLVERTEX );
		initData.pSysMem = m_pVertexData;
	}

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &desc, &initData, &m_pVertexBuffer ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::Setup() - failed to create vertex buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	UINT strides = sizeof( TLVERTEX );
	UINT offsets = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &strides, &offsets );
	GetEffectiveContext()->IASetPrimitiveTopology( m_bFL9X ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP : D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = m_bFL9X ? true : false;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateRasterizerState );
		return RESULT_FAIL;
	}
	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSDisabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateRasterizerState );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

//	GetDevice()->OMSetDepthStencilState( m_pDepthStencilState, 0 );

	// create and set sampler
	// Note: FL9 cannot use the border address mode or MaxLOD < FLT_MAX
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_SamplerDesc.AddressU = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressV = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressW = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_SamplerDesc.BorderColor[0] = 0.5f;
	m_SamplerDesc.BorderColor[1] = 0.5f;
	m_SamplerDesc.BorderColor[2] = 0.5f;
	m_SamplerDesc.BorderColor[3] = 1.0f;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = m_bFL9X ? FLT_MAX : 5;

	if( FAILED( hr = GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CRenderTargetsTest::Setup() - CreateSamplerState() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	// setup constant buffer
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 512;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffer ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBufferTiledOptional() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateResource );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	if( !m_bFL9X )
	{
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	}
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::SetupShaders(bool bForceFloatInput)
{
	bool bResult = true;
	HRESULT hr;
	D3DXMATRIX MatWorld;
	tstring psTexInputType = "float";
	tstring psOutputType = "float4";
	tstring numComponents = "";

	m_pVS = NULL; 
	m_pGS = NULL;
	m_pPS = NULL;

	if( !m_bFL9X )
	{
		GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);	// Vertex shader textures aren't supported on FL9
		GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	}
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	// create shaders and layout
	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;
	
	// Geometry shaders are not supported on FL9.X
	if( !m_bFL9X )
	{
		if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
			nullptr,
			_T( "RC_RENDERTARGETS_GSH" ),
			_T( "GShaderMain" ), 
			D3D_SHADER_STAGE_GEOMETRY,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_10_0,
			D3D10_SHADER_DEBUG,
			&pShaderBlob,
			&pErrorBlob ) ) || (pShaderBlob == nullptr) )
		{
			WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with geometry shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
			goto Fail;
		}

		
		if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateGeometryShader failed, hr = 0x%x" ), hr );
			goto Fail;
		}

		SAFE_RELEASE( pShaderBlob );
		SAFE_RELEASE( pErrorBlob );
	}

	//
	// Determine how to decl the input textures in the PS
	//
	const DXGI_FORMAT rtvFormat = GetRTViewFormat( m_Format );
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(rtvFormat);
	
	// Find out how many components the format has
	if (pTexFormatInfo->dwFlags & (FF_RGBA | FF_SRGB))
		numComponents = "4";
	else if( pTexFormatInfo->dwFlags & FF_RGB )
		numComponents = "3";
	else if( pTexFormatInfo->dwFlags & FF_RG )
		numComponents = "2";
	else
		numComponents = ""; // "" == 1

	// Determine the texture format type
	if (!bForceFloatInput)
	{
		if (pTexFormatInfo->dwFlags & FF_UINT)
			psTexInputType = "uint";
		else if (pTexFormatInfo->dwFlags & FF_SINT)
			psTexInputType = "int";
		else if (pTexFormatInfo->dwFlags & FF_UNORM)
			psTexInputType = "unorm float";
		else if (pTexFormatInfo->dwFlags & FF_SNORM)
			psTexInputType = "snorm float";
		else
			psTexInputType = "float";
	}

	// Add the number of components to the decl
	psTexInputType = FormatString("%s%s", psTexInputType.c_str(), numComponents.c_str());

	//
	// Determine what the PS output type is
	//
	if( pTexFormatInfo->dwFlags & FF_UINT )
		psOutputType = "uint4";
	else if( pTexFormatInfo->dwFlags & FF_SINT )
		psOutputType = "int4";
	else
		psOutputType = "float4";

	D3D10_SHADER_MACRO macros [] =
	{
		{ "TEX_INPUT_TYPE", psTexInputType.c_str() },
		{ "PS_OUTPUT_TYPE", psOutputType.c_str() },
		NULL
	};

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_FL9X_PSH" ) : _T( "RC_RENDERTARGETS_PSH" ),
		_T( "PShaderMain" ), 
		D3D_SHADER_STAGE_PIXEL,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob,
		m_bFL9X ? nullptr : macros ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with pixel shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreatePixelShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_SR_READBACK_FL9X_PSH" ) : _T( "RC_RENDERTARGETS_SR_READBACK_PSH" ),
		_T( "PShaderMain" ), 
		D3D_SHADER_STAGE_PIXEL,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob,
		m_bFL9X ? nullptr : macros ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with pixel shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pPS_SR_Readback ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreatePixelShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_FL9X_VSH" ) : _T( "RC_RENDERTARGETS_VSH" ),
		_T( "VShaderMain" ), 
		D3D_SHADER_STAGE_VERTEX,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with vertex shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateVertexShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	// setup input layout
	D3D11_INPUT_ELEMENT_DESC inputDesc [ ] =
	{
		{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
		{"texCoord", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
	};

	if( FAILED( hr = GetDevice()->CreateInputLayout( inputDesc, sizeof( inputDesc ) / sizeof( *inputDesc ), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), &m_pLayout ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateInputLayout failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	goto Pass;
Fail:
	bResult = false;
Pass:
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob )

	return bResult;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::IsValidFL9TestCase() const
{
	// Local variables
	bool bWDDM1_3OrHigher = false;
	bool bSwapbufferFormat = false;

	//
	// Common checks.
	//

	// Check the WDDM level
    if( m_wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
    {
        bWDDM1_3OrHigher = true;
    }

	// Check resource dimensions
	// Texture2D dimensions on FL9.x must be a power of 2 if they have multiple miplevels (this test always does)
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D && (
        ((m_uWidth > 0) && ((m_uWidth & (m_uWidth - 1)) != 0)) ||		// Width is a not a power of 2
        ((m_uHeight > 0) && ((m_uHeight & (m_uHeight - 1)) != 0)) ) ) 	// Height is a not a power of 2
    {
        WriteToLog( _T("D3D9 Texture2D's don't allow non power of 2 dimensions when multiple mip levels are specified.") );
        return false;
    }

	// TextureCubes dimensions on FL9.x must be a power of 2
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE && (
        ((m_uWidth > 0) && ((m_uWidth & (m_uWidth - 1)) != 0)) ||		// Width is a not a power of 2
        ((m_uHeight > 0) && ((m_uHeight & (m_uHeight - 1)) != 0)) ) ) 	// Height is a not a power of 2
    {
        WriteToLog( _T("D3D9 TextureCube's don't allow non power of 2 dimensions.") );
        return false;
    }

	//
	// Swapbuffer test cases
	// Driver must be WDDM1.3 or higher
	// Format must be A8_UNORM, R8_UNORM, or R8G8_UNORM
	//

	// Check the current format
	switch( m_Format )
    {
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8G8_UNORM:
        bSwapbufferFormat = true;
        break;
    }

	if( bWDDM1_3OrHigher && bSwapbufferFormat )
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------

TEST_RESULT CRenderTargetsTest::SetupTestCase()
{
	HRESULT hr = S_OK;

	GetResourceViewer().HideAll();	// We only want to show windows that are valid for the current test case

	// We don't run all test cases on FL9 be default; only specific test cases for new features
	if( m_bFL9X && !IsValidFL9TestCase() )
	{
		WriteToLog( _T("The test is running at feature level 9.x. Not all test cases are necessarily supported.") );
		return RESULT_SKIP;
	}

	// BUGBUG: Add support for these formats

	if( m_Format == DXGI_FORMAT_A8_UNORM )
		return RESULT_SKIP;

	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	if( m_bFL9X )
	{
		if( m_uRTIndex >= 1 )
		{
			WriteToLog( _T("Feature level 9 doesn't support texture arrays.") );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "RTIndex" ), m_uRTIndex ) );
			return RESULT_SKIP;
		}

		if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		{
			WriteToLog( _T("Feature level 9 doesn't support Texture3D as a render target.") );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
			return RESULT_SKIP;
		}
	}

	if( m_bCubeArray && GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "CubeArray" ), m_bCubeArray ) );
		return RESULT_SKIP;
	}

	if( !m_bCubeArray && m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE && m_uRTIndex >= 6 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "RTIndex" ), m_uRTIndex ) );
		return RESULT_SKIP;
	}

	if( CD3D10FormatHelper_Test::Planar(m_Format) )
	{
		// Planar formats change the default array max size to 2...
		if( m_uRTIndex > 1 )
		{
			SkipMatching( GetSkipMatchingFilter() || (FilterEqual( _T( "Format" ), m_Format ) && FilterEqual( _T( "RTIndex" ), m_uRTIndex ) ) );
			return RESULT_SKIP;
		}

		// TODO: Currently we don't support "ReformatPixels" pixels for any planar format besides NV12
		// Other planar formats should be skipped for now.
		if( m_Format != DXGI_FORMAT_NV12 )
		{
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
			return RESULT_SKIP;
		}
	}

	if( m_BindFlag == D3D11_BIND_SHADER_RESOURCE && m_ResourceType != D3D_RESOURCE_SCENARIO_TEXTURE2D )
		return RESULT_SKIP;

	UINT supportFlags = 0;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Format, &supportFlags ) ) )
	{
		WriteToLog( _T("CheckFormatSupport() failed with %s. Skipping format."), D3DHResultToString(hr).c_str() );
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}
	if( (supportFlags & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == 0)
	{
		WriteToLog( _T("Format doesn't support being a render target. Skipping format.") );
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}

	std::map< D3D_RESOURCE_SCENARIO, D3D11_FORMAT_SUPPORT > resTypes;
	resTypes[ D3D_RESOURCE_SCENARIO_BUFFER ] = D3D11_FORMAT_SUPPORT_BUFFER;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURECUBE;

	if( (supportFlags & resTypes[ m_ResourceType ]) == 0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
		return RESULT_SKIP;
	}

	if( (supportFlags & resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ]) == 0 ||
		(supportFlags & D3D11_FORMAT_SUPPORT_SHADER_LOAD) == 0)
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}

	if( ( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER && m_BindFlag != D3D11_BIND_RENDER_TARGET ) &&
		( m_Format == DXGI_FORMAT_R32G32B32_FLOAT || m_Format == DXGI_FORMAT_R32G32B32_SINT || m_Format == DXGI_FORMAT_R32G32B32_UINT ) )
	{
		return RESULT_SKIP;
	}

	const bool bFormatSupportsMips = (supportFlags & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	if( !bFormatSupportsMips &&	// The format doesn't support multiple mip levels
		m_uMipLevel != 0 )
	{
		WriteToLog( _T("%s doesn't support multiple mip levels but m_uMipLevel = %u. Skipping test case."), ToString(m_Format).c_str(), m_uMipLevel );
		return RESULT_SKIP;
	}

	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		// Video formats need to check that the resource is aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( m_Format );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( m_Format );

		// Check the box alignment and resource alignment
		switch( m_ResourceType )
		{
		default:
			WriteToLog( _T("Invalid m_ResourceScenario = %s (%u)."), ToString(m_ResourceType).c_str(), m_ResourceType );
			return RESULT_FAIL;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			// No box depth alignment checks
			// fall-through
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			if( (m_uHeight % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's height (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_uHeight, ToString(m_Format).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			// fall-through
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			if( (m_uWidth % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's width (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_uWidth, ToString(m_Format).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			break;
		}
	}

	// Set shader resource(s)
	if( !CreateShaderResources( ) )
		return RESULT_FAIL;

	// Set render target(s)
	if( !CreateRenderTargets( ) )
		return RESULT_FAIL;

	// create and set viewport
	UINT height, width, depth;
	depth = 1;
	width = m_uWidth;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER ) 
		height = 1;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		height = m_uWidth;
	else
		height = m_uHeight;
	
	if( m_ResourceType != D3D_RESOURCE_SCENARIO_BUFFER )
		GetMipDimensions( m_uMipLevel, width, height, depth );
	
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) width;
	vp.Height   = (float) height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	if( !SetupShaders( ) )
		return RESULT_FAIL;

	// pass current width and height to shader
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetImmediateContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupTestCase() - failed to map const buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}
	
	D3DXMatrixIdentity( &m_mxTrans );
	float transx = (float)width / 2.f;
	float transy = (float)height / 2.f;
	
	// Inverse viewport scale.
	m_mxTrans._13 = 1.f / transx;
	m_mxTrans._14 = -1.f / transy;

	// Prescaled inverse viewport translation.
	m_mxTrans._11 = -transx * m_mxTrans._13;
	m_mxTrans._12 = -transy * m_mxTrans._14;

	memcpy( mappedRes.pData, &m_mxTrans, sizeof( D3DXMATRIX ) );
	// write to c4, past transformation matrix
	float dims[ 2 ] = { (float)width, (float)height };
	int iRes = 0;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER )
		iRes = 0;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 2;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		iRes = 3;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 4;

	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX), dims, sizeof(float) * 2 );
	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX) + sizeof(float) * 2, &m_uRTIndex, sizeof(UINT32) );
	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX) + sizeof(float) * 2 + sizeof(UINT32), &iRes, sizeof(int) );

	GetImmediateContext()->Unmap(m_pConstBuffer, 0);

	m_bLastFaces = !m_bLastFaces;

	return RESULT_PASS;
}

//---------------------------------------------------------------

void CRenderTargetsTest::Cleanup( )
{
	CD3D11Test::Cleanup();
	
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pRSD