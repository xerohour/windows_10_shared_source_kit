* uiCbStride + 32;
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

					BYTE* const pbRefColor = pbRefBuffer + uiCbAddress * uiCbStride;
					const UINT bytesLeft = ( width - uiCbAddress ) * uiCbStride;
					memcpy( pbRefColor, _data, min( destMaskSize * uiCbStride, bytesLeft ) );
				}

				tx += fdtx;
				tz += fdtz;
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

	for ( UINT x = 0; x < width; ++x )
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
				WriteToLog( _T( "CStore_raw::AnalyzeResult() failed at pixel (%d), is different. \tExpected value is %d \tOutput value is %d" ), x, expectedData, resultData );
				numFailuresLogged++;
			}
			else if( !bLogVerbose && (numFailuresLogged == numFailuresToLog) )
			{
				// We will only hit this message once
				WriteToLog( _T( "CStore_raw::AnalyzeResult() has additional failures that are not logged to reduce log size. Please run the group again with -LogVerbose to log the additional failures." ));
				goto Cleanup;
			}
		}
	}

Cleanup:
	delete [] pbRefBuffer;
	GetImmediateContext()->Unmap( pResourceCopy, 0 );

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void
CStore_structured::InitTestParameters()
{
	// Define static parameters
	m_resFormat = DXGI_FORMAT_UNKNOWN;
	m_resourceType = RESOURCE_BUFFER;

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Define supported shader stages
	CUserValueSet<D3D_SHADER_STAGE>* const pShaderStages = new CUserValueSet<D3D_SHADER_STAGE>;
	pShaderStages->AddValue( D3D_SHADER_STAGE_VERTEX );
	pShaderStages->AddValue( D3D_SHADER_STAGE_GEOMETRY );
	pShaderStages->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_HULL );
	pShaderStages->AddValue( D3D_SHADER_STAGE_DOMAIN );
	testfactor::RFactor rfNonComputeShaders = this->AddParameterValueSet<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), pShaderStages );
	testfactor::RFactor rfPixelShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	testfactor::RFactor rfComputeShader = this->AddParameterValue<D3D_SHADER_STAGE>( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_COMPUTE );

	// Define TGSM usage
	CTestCaseParameter<bool>* const pUseTGSM = AddParameter( _T( "UseTGSM" ), &m_bUseTGSM );
	testfactor::RFactor rfUseTGSM = AddParameterValueSet( pUseTGSM, BoolValueSet() );
	SetParameterDefaultValue( pUseTGSM, false );

	// Define supported resource width values
	CUserValueSet<UINT>* const pWidthValues = new CUserValueSet<UINT>;
	pWidthValues->AddValue( MAX_WIDTH * 1 );
    pWidthValues->AddValue( MAX_WIDTH * 2 );
    pWidthValues->AddValue( MAX_WIDTH * 4 );
	testfactor::RFa