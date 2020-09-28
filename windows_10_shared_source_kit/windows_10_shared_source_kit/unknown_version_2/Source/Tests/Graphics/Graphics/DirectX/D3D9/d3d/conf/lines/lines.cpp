= _T("1");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x =  D3D11_REQ_TEXTURE1D_U_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
					resource = _T("RWTexture1DArray<uint>");
					resource_type = _T("5");
					//dimensions = _T("2");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE1D_U_DIMENSION;
					max_array_size = D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE2D:
					resource = _T("RWTexture2D<uint>");
					resource_type = _T("6");
					//dimensions = _T("2");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
					resource = _T("RWTexture2DArray<uint>");
					resource_type = _T("7");
					//dimensions = _T("3");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_array_size = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE3D:
					resource = _T("RWTexture3D<uint>");
					resource_type = _T("8");
					//dimensions = _T("3");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					max_elements_z = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					break;
				default:
					WriteToLog(_T("Unknown D3D11_UAV_DIMENSION: %s"), ToString( m_TargetUAVDimension ) ); 
					return RESULT_FAIL;
			}

			const unsigned int max_texture_bytes = max_elements_x * max_elements_y * max_elements_z * m_TargetUAVElementByteStride;

			switch( m_TargetUAVDimension )
			{
				case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
				case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
					unsigned int needed_array_count = bytesNeeded / max_texture_bytes;
					if( bytesNeeded % max_texture_bytes )
					{
						needed_array_count += 1;
					}
					if( needed_array_count > 1 )
					{
						bytesNeeded = max_texture_bytes;
					}
					if( needed_array_count > max_array_size )
					{
						WriteToLog( _T("Texture requirements are too large: (array)") );
						return RESULT_FAIL;
					}
					m_TargetUAVArraySize = max( needed_array_count, m_TargetUAVArraySize );
					break;
			}

			// determine how big it really has to be
			if( bytesNeeded > (max_elements_x * max_elements_y * m_TargetUAVElementByteStride) )
			{
				m_TargetUAVSize[2] = bytesNeeded / ( max_elements_x * max_elements_y * m_TargetUAVElementByteStride);
				if( bytesNeeded % ( max_elements_x * max_elements_y * m_TargetUAVElementByteStride) )
				{
					m_TargetUAVSize[2] += 1;
				}
				m_TargetUAVSize[1] = max_elements_y;
				m_TargetUAVSize[0] = max_elements_x;
				if( m_TargetUAVSize[2] > max_elements_z )
				{
					WriteToLog( _T("Texture requirements are too large:(3D)") );
					return RESULT_FAIL;
				}
			}
			else if( bytesNeeded > (max_elements_x * m_TargetUAVElementByteStride))
			{
				m_TargetUAVSize[1] = bytesNeeded / (max_elements_x * m_TargetUAVElementByteStride);
				if( bytesNeeded % (max_elements_x * m_TargetUAVElementByteStride) )
				{
					m_TargetUAVSize[1] += 1;
				}
				m_TargetUAVSize[0] = max_elements_x;
				if( m_TargetUAVSize[1] > max_elements_y )
				{
					WriteToLog( _T("Texture requirements are too large:(2D)") );
					return RESULT_FAIL;
				}
			}
			else
			{
				m_TargetUAVSize[0] = bytesNeeded / m_TargetUAVElementByteStride;
			}

			// Set the macros
			const tstring group_thread_count_X( FormatString( _T("%d"), m_NumGroupThreads[0] ) );
			const tstring group_thread_count_Y( FormatString( _T("%d"), m_NumGroupThreads[1] ) );
			const tstring group_thread_count_Z( FormatString( _T("%d"), m_NumGroupThreads[2] ) );
			const tstring group_count_X( FormatString( _T("%d"), m_NumGroups[0] ) );
			const tstring group_count_Y( FormatString( _T("%d"), m_NumGroups[1] ) );
			const tstring group_count_Z( FormatString( _T("%d"), m_NumGroups[2] ) );
			//const tstring group_count( FormatString( _T("%d"), numGroups ) );
			const tstring window_size( FormatString( _T("%d"), m_CheckWindowSize ) );
			//const tstring update_count( FormatString( _T("%d"), m_NumSharedMemUpdates ) );
			//const tstring check_time( FormatString( _T("%d"), m_NumSharedMemChecks ) );
			const tstring struct_size( FormatString( _T("%d"), m_TargetUAVElementByteStride / sizeof(UINT) ) );
			const tstring memory_barrier_type( FormatString( _T("%d"), m_MemoryBarrierType ) );
			const tstring resultOffsetStr( FormatString(_T("%d"), m_ResultOffset ));

			D3D10_SHADER_MACRO const macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("GROUP_THREAD_COUNT_X"), group_thread_count_X.c_str() },
				{ _T("GROUP_THREAD_COUNT_Y"), group_thread_count_Y.c_str() },
				{ _T("GROUP_THREAD_COUNT_Z"), group_thread_count_Z.c_str() },
				{ _T("GROUP_COUNT_X"), group_count_X.c_str() },
				{ _T("GROUP_COUNT_Y"), group_count_Y.c_str() },
				{ _T("GROUP_COUNT_Z"), group_count_Z.c_str() },
				//{ _T("GROUP_COUNT"), group_count.c_str() },
				{ _T("WINDOW_SIZE"), window_size.c_str() },
				//{ _T("UPDATE_COUNT"), update_count.c_str() },
				//{ _T("CHECK_TIME"), check_time.c_str() },
				{ _T("RESOURCE"), resource.c_str() },
				{ _T("RESOURCE_TYPE"), resource_type.c_str() },
				//{ _T("DIMENSIONS"), dimensions.c_str() },
				//{ _T("MAX_THREAD_GROUP_SHARED"), max_thread_group_shared.c_str() },
				{ _T("IS_STRUCTURED"), m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ? _T("1") : _T("0") },
				{ _T("IS_BYTE_ADDRESSED"), m_TargetUAVMiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS ? _T("1") : _T("0") },
				{ _T("USE_GROUP_SHARE_FLAG"), m_UseGroupSharedMemoryFlag ? _T("1") : _T("0") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("MEMORY_BARRIER_TYPE"), memory_barrier_type.c_str() },
				{ _T("USE_CROSS_GROUP_READS"), m_UseCrossGroupReads ? _T("1") : _T("0") },
				{ _T("RESULT_OFFSET"), resultOffsetStr.c_str() },
				NULL
			};

			{
				TCOMPtr<ID3D10Blob*> pError;
				char const* fLevel = g_TestApp.GetShaderProfile( D3D_SHADER_STAGE_COMPUTE, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0 );
				
				{
					TCOMPtr<ID3D10Blob*> pText;
					hr = D3DX11PreprocessShaderFromResource(	NULL,
																_T("MemoryBarrier.fx"),
																_T("MemoryBarrier.fx"),
																macros,
																NULL,
																NULL,
																pText,
																pError,
																NULL );
					if( SUCCEEDED(hr ) )
					{
						if( GetFramework()->LogVerbose() )
							WriteMessage( _T("Shader:\n%s"), pText->GetBufferPointer() );
					}
				}

				hr = D3DX11CompileFromResource(	NULL, 
												_T("MemoryBarrier.fx"),
												_T("MemoryBarrier.fx"),
												macros,
												NULL,
												_T("main"),
												fLevel,
												D3D10_SHADER_IEEE_STRICTNESS | D3D10_SHADER_OPTIMIZATION_LEVEL0,
												0,
												NULL,
												pShader,
												pError,
												NULL );
												
				if( FAILED(hr) )
				{
					if( *pError )
					{
						WriteToLog( _T("Compile failed %s\n%s"), D3DHResultToString(hr).c_str(), pError->GetBufferPointer() );
					}
					else
					{
						WriteToLog( _T("Compile failed: %s"), D3DHResultToString( hr ).c_str() );
					}
					
					tstring tmacros;
					for( unsigned int i(0); macros[i].Name != NULL; ++i )
					{
						tmacros = tmacros + FormatString( _T(" /D%s=%s"), macros[i].Name, macros[i].Definition );
					}
					WriteToLog( _T("Macros: %s"), tmacros.c_str() );
					return RESULT_FAIL;
				}
			}

			// create shader
			SAFE_RELEASE(m_pCS);
			hr = GetDevice()->CreateComputeShader(	pShader->GetBufferPointer(),
					pShader->GetBufferSize(),
					NULL,
					&m_pCS );
			if( FAILED(hr) || m_pCS == NULL )
			{
				WriteToLog( _T("CreateComputeShader failed, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}


	// set the shader
	GetEffectiveContext()->CSSetShader( m_pCS, NULL, 0 );

	// Constant buffer setup
	{
		D3D11_BUFFER_DESC const desc = 
		{
			16,
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0,
			0
		};

		if( FAILED(hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, &m_pConstBuffer )) )
		{
			WriteToLog( _T("Failed to create constant buffer, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	// map and bind constant buffer
	{
		struct ConstantBufferMapping
		{
			unsigned int m_UpdateCount;
			unsigned int m_CheckTime;
		} globalBuffer = 
		{
			m_NumSharedMemUpdates,
			m_NumSharedMemChecks
		};

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, globalBuffer ) ) )
		{
			WriteToLog( _T("Failed to write data to constant buffer, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	}

	// must go last since the m_TargetUAVSize is set above
	return CComputeMemoryTest::SetupTestCase();
	// validate
	//
}

TEST_RESULT CMemoryBarrierTest::ExecuteTestCase()
{
	// fix the uav shader indexing to allow for multiple dimensions? -NO
	// execute
	GetEffectiveContext()->Dispatch( m_NumGroups[0], m_NumGroups[1], m_NumGroups[2] );

	ExecuteEffectiveContext();
	// copy uav
	GetImmediateContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );
	
	// verify results
	// All fault values in the uav should be zero
	{
		const unsigned int numGroups = m_NumGroups[0] * m_NumGroups[1] * m_NumGroups[2];
		const unsigned int numThreads = m_NumGroupThreads[0] * m_NumGroupThreads[1] * m_NumGroupThreads[2];
		unsigned int elementsToCheck = numGroups * numThreads;

		for( unsigned int array_index(0); array_index < m_TargetUAVArraySize; ++array_index )
		{
			D3D11_MAPPED_SUBRESOURCE mappedRes;
			HRESULT hr = GetImmediateContext()->Map( m_pTargetResourceCopy, array_index, D3D11_MAP_READ, 0, &mappedRes );
			if( FAILED(hr) )
			{
				WriteToLog( _T("Map(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		
			UINT rowPitch = mappedRes.RowPitch;
			UINT depthPitch = mappedRes.DepthPitch;
			const unsigned int totalElements = m_TargetUAVSize[0] * m_TargetUAVSize[1] * m_TargetUAVSize[2];
			for( unsigned int uIndex(0); uIndex < totalElements; ++uIndex )
			{
				UINT numFaults = 0;
				if ( m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
					// for structured buffer, the result is stored in the last UINT field of the structure
					numFaults = static_cast<UINT const*>(mappedRes.pData)[ (uIndex + 1) * m_TargetUAVElementByteStride/sizeof(UINT) - 1];
				else
				{
					UINT rowIndex = 0;
					UINT depthIndex = 0;
					UINT dataIndex = m_ResultOffset + uIndex;
					if ( m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE2D )
					{
						rowIndex = (m_ResultOffset + uIndex) / rowPitch;
						dataIndex = (m_ResultOffset + uIndex) % rowPitch;
					}
					else if ( m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE3D )
					{
						depthIndex = (m_ResultOffset + uIndex) / depthPitch;
						rowIndex = (m_ResultOffset + uIndex - depthIndex * depthPitch ) / rowPitch;
						dataIndex = (m_ResultOffset + uIndex) % rowPitch;
					}
					numFaults = static_cast<UINT const*>(mappedRes.pData)[depthIndex * depthPitch + rowIndex * rowPitch + dataIndex];
				}
				if( numFaults != 0u )
				{
					WriteToLog( _T("Error: Encountered a fault: %u for thread id: %u"), numFaults, uIndex );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, array_index );
					return RESULT_FAIL;
				}

				elementsToCheck--;
				if( elementsToCheck == 0 )
				{
					break;
				}
			}

			GetImmediateContext()->Unmap( m_pTargetResourceCopy, array_index );
			if( elementsToCheck == 0 )
			{
				break;
			}
		}
	}
	
	return RESULT_PASS;

}

void CMemoryBarrierTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pConstBuffer );
	CComputeMemoryTest::CleanupTestCase();
}




//////////////////////////////////////////////////////////////////
// class CMemoryBarrierPSTest : public CMemoryBarrierTest
//////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::InitTestParameters()
{
	// viewport width and height, working as the X and Y dimensions of a thead group in Compute
	CUserValueSet<UINT> *pWidthHeight = new CUserValueSet<UINT>;
	pWidthHeight->AddValue(16);
	pWidthHeight->AddValue(31);
	testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidthHeight);
	testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pWidthHeight);

	CUserValueSet<UINT> *pWidthHeightLarge = new CUserValueSet<UINT>;
	pWidthHeightLarge->AddValue(67);
	pWidthHeightLarge->AddValue(128);
	testfactor::RFactor rfViewWidthLarge = AddParameterValueSet<UINT>(_T("ViewWidth"), pWidthHeightLarge);
	testfactor::RFactor rfViewHeightLarge = AddParameterValueSet<UINT>(_T("ViewHeight"), pWidthHeightLarge);

	// resource types
	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeGroup1 = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeGroup1->AddValue(TEXTURE1D);
	pUAVTypeGroup1->AddValue(TEXTURE1DARRAY);
	pUAVTypeGroup1->AddValue(TEXTURE2D);
	pUAVTypeGroup1->AddValue(TEXTURE2DARRAY);
	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeGroup2 = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeGroup2->AddValue(TEXTURE3D);
	pUAVTypeGroup2->AddValue(RAW_BUFFER);
	pUAVTypeGroup2->AddValue(TYPED_BUFFER);
	pUAVTypeGroup2->AddValue(STRUCTURED_BUFFER);
	testfactor::RFactor rfTypeUAV1Group1 = AddParameter<UAV_RESOURCE_TYPE>(_T("FirstUAV"), &(m_TypeUAV[0]), pUAVTypeGroup1);
	testfactor::RFactor rfTypeUAV1Group2 = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("FirstUAV"), pUAVTypeGroup2);
	testfactor::RFactor rfTypeUAV2Group1 = AddParameter<UAV_RESOURCE_TYPE>(_T("SecondUAV"), &(m_TypeUAV[1]), pUAVTypeGroup1);
	testfactor::RFactor rfTypeUAV2Group2 = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("SecondUAV"), pUAVTypeGroup2);

	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeLarge = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeLarge->AddValue(TEXTURE1D);
	pUAVTypeLarge->AddValue(TEXTURE1DARRAY);
	pUAVTypeLarge->AddValue(TEXTURE2D);
	pUAVTypeLarge->AddValue(TEXTURE2DARRAY);
	pUAVTypeLarge->AddValue(RAW_BUFFER);
	pUAVTypeLarge->AddValue(TYPED_BUFFER);
	pUAVTypeLarge->AddValue(STRUCTURED_BUFFER);
	testfactor::RFactor rfTypeUAV1Large = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("FirstUAV"), pUAVTypeLarge);
	testfactor::RFactor rfTypeUAV2Large = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("SecondUAV"),pUAVTypeLarge);

	// data type: only need to test the difference between scalar type and vector type; 
	// there's no difference between uint and float in terms of store operation and memory flush
	// BUGBUG: Compiler error X3676: typed UAV loads are only allowed for single-component element types; disable uint2 for now
	CUserValueSet<tstring> *pDataType = new CUserValueSet<tstring>;
	pDataType->AddValue(_T("uint"));
	//pDataType->AddValue(_T("uint2"));
	testfactor::RFactor rfDataType1 = AddParameter<tstring>(_T("DataTypeFirstUAV"), &(m_DataTypeUAV[0]), pDataType);
	testfactor::RFactor rfDataType2 = AddParameter<tstring>(_T("DataTypeSecondUAV"), &(m_DataTypeUAV[1]), pDataType);


	testfactor::RFactor root = (rfViewWidth * rfViewHeight) * rfDataType1 * rfDataType2 
		* ( ( (rfTypeUAV1Group1 + rfTypeUAV1Group2) % (rfTypeUAV2Group1 + rfTypeUAV2Group2) )
		    + ( rfTypeUAV1Group1 % rfTypeUAV2Group2)
		  ) 
		  + (rfViewWidthLarge * rfViewHeightLarge) * rfDataType1 * rfDataType2 * (rfTypeUAV1Large % rfTypeUAV2Large) ;

	SetRootTestFactor( root );
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::Setup()
{
	TEST_RESULT tr = CComputeMemoryTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CComputeMemoryTest::Setup() failed"));
		return tr;
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}
	
	tr = SetupVSStage();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupVSStage() failed"));
		return tr;
	}

	return RESULT_PASS;
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::SetupTestCase()
{
	if ( SkipSlowOnRefWithMessage( m_ViewWidth * m_ViewHeight  > 64 * 64  ) )
		return RESULT_SKIP;

	if ( CreateUAVs() != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: SetupUAVs failed "));
		return RESULT_FAIL;
	}

	if ( SetupPixelShader() != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: SetupPixelShader failed "));
		return RESULT_FAIL;
	}

	// setup viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)m_ViewHeight;
	vp.Width = (float)m_ViewWidth;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );
	// create a dummy rendertarget
	if ( CComputeTest::SetupRTV(m_ViewWidth, m_ViewHeight) != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: CComputeTest::SetupRTV() failed "));
		return RESULT_FAIL;
	}


	// Set vertex buffer
	UINT stride = sizeof( float ) * 3;
	UINT offset = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	// Set shaders
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

	// Set UAVs: 3 UAVs; the first 2 are read/write UAVs; the last one is the result UAV
	GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
		1, 
		&m_pRTView, 
		NULL,
		1, 
		3,
		m_ppUAVs, 
		NULL);

	return RESULT_PASS;
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::CreateUAVs()
{
	D3D11_TEXTURE1D_DESC  tex1DDesc;
	tex1DDesc.Width = m_ViewWidth * m_ViewHeight;
    tex1DDesc.MipLevels = 1;
    tex1DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex1DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex1DDesc.CPUAccessFlags = 0;
    tex1DDesc.MiscFlags = 0;

	D3D11_TEXTURE2D_DESC  tex2DDesc;
	tex2DDesc.Width = m_ViewWidth * m_ViewHeight;
	tex2DDesc.Height = 2;
    tex2DDesc.MipLevels = 1;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

	D3D11_TEXTURE3D_DESC  tex3DDesc;
	tex3DDesc.Width = m_ViewWidth * m_ViewHeight;
	tex3DDesc.Height = 2;
	tex3DDesc.Depth = 2;
    tex3DDesc.MipLevels = 1;
    tex3DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex3DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex3DDesc.CPUAccessFlags = 0;
    tex3DDesc.MiscFlags = 0;

	HRESULT hr;
	D3D11_BUFFER_DESC buffDesc;
	// create staging output UAV resource
	buffDesc.ByteWidth = sizeof(UINT) * m_ViewWidth * m_ViewHeight; 
	buffDesc.Usage = D3D11_USAGE_STAGING;
	buffDesc.BindFlags = 0;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	buffDesc.MiscFlags = 0; 
	buffDesc.StructureByteStride = 0;
	CREATERESOURCETILEDOPT(buffDesc, m_pResultCopyBuffer, CreateBuffer);
	// create output UAV resource
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffDesc.CPUAccessFlags = 0;
	CREATERESOURCETILEDOPT(buffDesc, m_pResultUAVBuffer, CreateBuffer);

	// Create UAV resources for the two read/write UAVs
	for ( UINT i = 0; i < 2; i++ )
	{
		if ( m_TypeUAV[i] == RAW_BUFFER
			|| m_TypeUAV[i] == TYPED_BUFFER 
			|| m_TypeUAV[i] == STRUCTURED_BUFFER )
		{
			if ( m_TypeUAV[i] == RAW_BUFFER )
			{
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_TypeUAV[i] == TYPED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_TypeUAV[i] == STRUCTURED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				if ( m_DataTypeUAV[i] == _T("uint"))
					buffDesc.StructureByteStride = 4;
				else if ( m_DataTypeUAV[i] == _T("uint2"))
					buffDesc.StructureByteStride = 8;
			}
			CREATERESOURCETILEDOPT(buffDesc, m_ppUAVBuf[i], CreateBuffer);
		}
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == TEXTURE1DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE1D )
				tex1DDesc.ArraySize = 1;
			else if (m_TypeUAV[i] == TEXTURE1DARRAY)
				tex1DDesc.ArraySize = 2;
			CREATERESOURCE(tex1DDesc, m_ppUAVTex1D[i], CreateTexture1D);
		}
		else if ( m_TypeUAV[i] == TEXTURE2D || m_TypeUAV[i] == TEXTURE2DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE2D )
				tex2DDesc.ArraySize = 1;
			else if (m_TypeUAV[i] == TEXTURE2DARRAY)
				tex2DDesc.ArraySize = 2;
			CREATERESOURCETILEDOPT(tex2DDesc, m_ppUAVTex2D[i], CreateTexture2D);
		}
		else if (m_TypeUAV[i] == TEXTURE3D)
		{
			CREATERESOURCE(tex3DDesc, m_ppUAVTex3D[i], CreateTexture3D);
		}
	}

	// Create UAVs on the above created resources
	D3D11_BUFFER_UAV BufUAV = {0, m_ViewWidth * m_ViewHeight, 0};
	D3D11_TEX1D_UAV Tex1DUAV = {0};
	D3D11_TEX1D_ARRAY_UAV Tex1DArrayUAV = {0, 0, 2};
	D3D11_TEX2D_UAV Tex2DUAV = {0};
    D3D11_TEX2D_ARRAY_UAV Tex2DArrayUAV = {0, 0, 2};
    D3D11_TEX3D_UAV Tex3DUAV = {0, 0, 2};

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R32_UINT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER ;
	UAVDesc.Buffer = BufUAV;

	// Create result UAV
	CREATEUAV(m_pResultUAVBuffer, UAVDesc, m_ppUAVs[2]);

	// Create UAViews for the two read/write UAVs: m_ppUAVs[0] and m_ppUAVs[1]
	for ( UINT i = 0; i < 2; i++ )
	{
		UAVDesc.Format = DXGI_FORMAT_R32_UINT;
		if ( m_TypeUAV[i] == RAW_BUFFER
			|| m_TypeUAV[i] == TYPED_BUFFER 
			|| m_TypeUAV[i] == STRUCTURED_BUFFER )
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVDesc.Buffer = BufUAV;
			if ( m_TypeUAV[i] == RAW_BUFFER )
			{
				UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			}
			else if (m_TypeUAV[i] == STRUCTURED_BUFFER)
			{
				UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
			}
			
			CREATEUAV(m_ppUAVBuf[i], UAVDesc, m_ppUAVs[i]);
		}
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == TEXTURE1DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE1D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
				UAVDesc.Texture1D = Tex1DUAV;
			}
			else if (m_TypeUAV[i] == TEXTURE1DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
				UAVDesc.Texture1DArray = Tex1DArrayUAV;
			}
			CREATEUAV(m_ppUAVTex1D[i], UAVDesc, m_ppUAVs[i]);
		}
		else if ( m_TypeUAV[i] == TEXTURE2D || m_TypeUAV[i] == TEXTURE2DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE2D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				UAVDesc.Texture2D = Tex2DUAV;
			}
			else if (m_TypeUAV[i] == TEXTURE2DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				UAVDesc.Texture2DArray = Tex2DArrayUAV;
			}
			CREATEUAV(m_ppUAVTex2D[i], UAVDesc, m_ppUAVs[i]);
		}
		else if (m_TypeUAV[i] == TEXTURE3D)
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
			UAVDesc.Texture3D = Tex3DUAV;
			CREATEUAV(m_ppUAVTex3D[i], UAVDesc, m_ppUAVs[i]);
		}
	}

	return RESULT_PASS;
	
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::SetupPixelShader()
{
	TEST_RESULT tr= RESULT_PASS;
	HRESULT hr;
	
	tstring ViewWidthStr = FormatString( _T("%i"), m_ViewWidth );
	tstring ViewHeightStr = FormatString( _T("%i"), m_ViewHeight );

	tstring UAVDclsStr;
	tstring UAVWriteStr;
	tstring UAVReadStr;

	// Generate UAV Dcls
	// UAV must be declared as "globallycoherent" to do cross-PS-invocation communication  
	for (UINT i = 0; i < 2; i++)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWByteAddressBuffer u%d; "), i+1);
		else if ( m_TypeUAV[i] == STRUCTURED_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWStructuredBuffer<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TYPED_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWBuffer<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture1D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture1DArray<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE2D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture2D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture2DArray<%s> u%d; "),(m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE3D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTextur