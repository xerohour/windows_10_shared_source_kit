
// Includes
#include "ResourceIndexing.h"

//------------------------------------------------------------------------------------------------
// CSamplerIndexing
//------------------------------------------------------------------------------------------------

CSamplerIndexing::~CSamplerIndexing(void)
{
}

/////

void CSamplerIndexing::InitTestParameters()
{
	//
	// Shader stage
	//
	
	CTestCaseParameter<D3D_SHADER_STAGE> *pShaderStageParam = AddParameter( _T("ShaderStage"), &m_shaderStage);

	CUserValueSet<D3D_SHADER_STAGE> *pShaderStageValues = new CUserValueSet<D3D_SHADER_STAGE>();
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_VERTEX);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_HULL);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_DOMAIN);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_GEOMETRY);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_PIXEL);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_COMPUTE);

	testfactor::RFactor rfShaderStage = AddParameterValueSet( pShaderStageParam, pShaderStageValues );

	//
	// Resource type
	//

	CTestCaseParameter<D3D_RESOURCE_TYPE> *pDimensionParam = AddParameter( _T("ResourceDimension"), &m_resourceType);

	CUserValueSet<D3D_RESOURCE_TYPE> *pDimensionValues = new CUserValueSet<D3D_RESOURCE_TYPE>();
	pDimensionValues->AddValue(D3D_RESOURCE_TYPE_SAMPLER);

	testfactor::RFactor rfResourceDimension = AddParameterValueSet( pDimensionParam, pDimensionValues );

	//
	// Instance slot
	//

	CTestCaseParameter<UINT> *pInstanceSlotParam = AddParameter( _T("InstanceSlot"), &m_successInstanceSlot);

	CRangeValueSet<UINT> *pInstanceSlotValues = new CRangeValueSet<UINT>(0, MAX_INTERFACE_SLOTS - 1, 1);

	testfactor::RFactor rfInstanceSlot = AddParameterValueSet( pInstanceSlotParam, pInstanceSlotValues );

	//
	// Sampler slot
	//

	CTestCaseParameter<UINT> *pSamplerSlotParam = AddParameter( _T("SamplerSlot"), &m_successSamplerSlot);

	CRangeValueSet<UINT> *pSamplerSlotValues = new CRangeValueSet<UINT>(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1, 1);

	testfactor::RFactor rfSamplerSlot = AddParameterValueSet( pSamplerSlotParam, pSamplerSlotValues );

	// Set test factor
	SetRootTestFactor( rfShaderStage * rfResourceDimension * (rfInstanceSlot % rfSamplerSlot) );
}

/////

TEST_RESULT CSamplerIndexing::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Determine the texture slot that fails
	// Note: Only one out of all bound textures will be the passing slot, all others are failing
	m_failSamplerSlot = GetAFailingSamplerSlot();

	if( !IsFailingSamplerSlot( m_failSamplerSlot ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CSamplerIndexing::SetupTestCase() - Failed to generate a failing sampler slot index.") );
		WriteToLog( _T(" Generated index = %d"), m_failSamplerSlot );
		goto setupDone;
	}

	// Call the parent SetupTestCase()
	tr = CResourceIndexing::SetupTestCase();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CSamplerIndexing::SetupTestCase() - SetupTestCase() failed.") );
		goto setupDone;
	}

setupDone:
	return tr;
}

/////

void CSamplerIndexing::CleanupTestCase()
{
	// Cleanup the sampler states
	const UINT numSamplers = ARRAY_SIZE(m_pSamplerStates);
	for( UINT i = 0; i < numSamplers; ++i )
		SAFE_RELEASE( m_pSamplerStates[i] );

	SAFE_RELEASE(m_pTexture2D);
	SAFE_RELEASE(m_pShaderResourceView);

	CResourceIndexing::CleanupTestCase();
}

/////


TEST_RESULT CSamplerIndexing::SetupResources()
{
	// Local variables
	TEST_RESULT tr			= RESULT_PASS;
	HRESULT hr				= S_OK;
	const UINT numSamplers	= ARRAY_SIZE(m_pSamplerStates);
	D3D11_TEXTURE2D_DESC			tex2DDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_SAMPLER_DESC				samplerDesc;
	UINT borderColor = 0;

	//
	// Create the sampler states
	//

	// Create the sample description
	ZeroMemory( &samplerDesc, sizeof(samplerDesc) );

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = FLT_MAX;

	// Create the sampler states
	for( UINT i = 0; i < numSamplers; ++i )
	{
		// Compute the color for the texture (each texture must be unique)
		borderColor = GetExpectedDataForResourceInSlot(i);

		// Update the sampler description
		for( UINT colorIndex = 0; colorIndex < 4; ++colorIndex )
			samplerDesc.BorderColor[colorIndex] = (float)borderColor;

		// Create the sampler
		if( FAILED( hr = GetDevice()->CreateSamplerState( &samplerDesc, &(m_pSamplerStates[i]) ) ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CSamplerIndexing::SetupResources() - Failed to Create Sample State (m_pSamplerStates[%d]). HR = %s"), i, D3DHResultToString(hr));
			goto setupDone;
		}
	}

	//
	// Create a texture resource
	//

	// Texture2D description
	tex2DDesc.Width = 1;
	tex2DDesc.Height = 1;
	tex2DDesc.MipLevels = 0;
	tex2DDesc.ArraySize = 1;
	tex2DDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.MiscFlags = 0;

	// Create a texture2d resource
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pTexture2D ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CSamplerIndexing::SetupResources() - Failed to Create Texture2D (m_pTexture2D). HR = %s"), D3DHResultToString(hr));
		goto setupDone;
	}

	//
	// Create the view
	//

	// Shader resource view
	srvDesc.Format = tex2DDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// Create the Shader resource view
	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTexture2D, &srvDesc, &m_pShaderResourceView ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CSamplerIndexing::SetupResources() - Failed to Create ShaderResourceView (m_pShaderResourceView) for m_pTexture2D. HR = %s"), D3DHResultToString(hr));
		goto setupDone;
	}

	// Bind the resources to the pipeline
	{
		ID3D11SamplerState* pNULLSampler[numSamplers];
		ID3D11ShaderResourceView* pNULLSRV = NULL;

		ZeroMemory( pNULLSampler, sizeof(pNULLSampler) );

		GetEffectiveContext()->VSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->VSSetShaderResources(0, 1, &pNULLSRV);

		GetEffectiveContext()->HSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->HSSetShaderResources(0, 1, &pNULLSRV);

		GetEffectiveContext()->DSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->DSSetShaderResources(0, 1, &pNULLSRV);

		GetEffectiveContext()->GSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->GSSetShaderResources(0, 1, &pNULLSRV);

		GetEffectiveContext()->PSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->PSSetShaderResources(0, 1, &pNULLSRV);

		GetEffectiveContext()->CSSetSamplers( 0, numSamplers, pNULLSampler );
		GetEffectiveContext()->CSSetShaderResources(0, 1, &pNULLSRV);
	}

	switch( m_shaderStage )
	{
	default:
		tr = RESULT_FAIL;
		WriteToLog( _T("CSamplerIndexing::SetupResources() - Invalid shader stage used in the test.") );
		goto setupDone;
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->VSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->HSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->DSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->GSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->PSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetSamplers( 0, numSamplers, m_pSamplerStates );
		GetEffectiveContext()->CSSetShaderResources(0, 1, &m_pShaderResourceView);
		break;
	}	

setupDone:
	return tr;
}

/////

UINT CSamplerIndexing::GetAFailingSamplerSlot() const
{ 
	// Local variables
	UINT failingSlot = 0; 
	
	failingSlot = m_successSamplerSlot + 1;

	if( failingSlot >= ARRAY_SIZE(m_pSamplerStates) )
		failingSlot = 0;

	return failingSlot;
}

/////

TEST_RESULT CSamplerIndexing::VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource )
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	UINT *pResult = (UINT*)(mappedResource.pData);

	// Get the expected result
	const UINT expectedSamplerResult = GetExpectedDataForResourceInSlot( m_successSamplerSlot );

	// Get the actual result
	const UINT actualSamplerResult = pResult[0];

	if( actualSamplerResult != expectedSamplerResult )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CSamplerIndexing::VerifyResult() - The actual result did not match the expected result."));
		WriteToLog( _T(" Actual result = %d (%#x)"), actualSamplerResult, actualSamplerResult );
		WriteToLog( _T(" Expected result = %d (%#x)"), expectedSamplerResult, expectedSamplerResult);
		goto verifyDone;
	}

verifyDone:
	return tr;
}