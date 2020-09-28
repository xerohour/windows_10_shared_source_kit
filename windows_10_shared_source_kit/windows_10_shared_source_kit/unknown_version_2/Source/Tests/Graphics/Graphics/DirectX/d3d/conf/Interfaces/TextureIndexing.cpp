
// Includes
#include "ResourceIndexing.h"

//------------------------------------------------------------------------------------------------
// CTextureIndexing
//------------------------------------------------------------------------------------------------

CTextureIndexing::~CTextureIndexing(void)
{
}

/////

void CTextureIndexing::InitTestParameters()
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
	pDimensionValues->AddValue(D3D_RESOURCE_TYPE_BUFFER);
	pDimensionValues->AddValue(D3D_RESOURCE_TYPE_TEXTURE2D);

	testfactor::RFactor rfResourceDimension = AddParameterValueSet( pDimensionParam, pDimensionValues );

	//
	// Instance slot
	//

	CTestCaseParameter<UINT> *pInstanceSlotParam = AddParameter( _T("InstanceSlot"), &m_successInstanceSlot);

	CRangeValueSet<UINT> *pInstanceSlotValues = new CRangeValueSet<UINT>(0, MAX_INTERFACE_SLOTS - 1, 1);

	testfactor::RFactor rfInstanceSlot = AddParameterValueSet( pInstanceSlotParam, pInstanceSlotValues );

	//
	// Texture slot
	//

	CTestCaseParameter<UINT> *pTextureSlotParam = AddParameter( _T("TextureSlot"), &m_successTextureSlot);

	CRangeValueSet<UINT> *pTextureSlotValues = new CRangeValueSet<UINT>(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1, 1);

	testfactor::RFactor rfTextureSlot = AddParameterValueSet( pTextureSlotParam, pTextureSlotValues );

	// Set test factor
	SetRootTestFactor( rfShaderStage * rfResourceDimension * (rfInstanceSlot % rfTextureSlot) );
}

/////

TEST_RESULT CTextureIndexing::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Determine the texture slot that fails
	// Note: Only one out of all bound textures will be the passing slot, all others are failing
	m_failTextureSlot = GetAFailingTextureSlot();

	if( !IsFailingTextureSlot( m_failTextureSlot ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CTextureIndexing::SetupTestCase() - Failed to generate a failing texture slot index.") );
		WriteToLog( _T(" Generated index = %d"), m_failTextureSlot );
		goto setupDone;
	}

	// Call the parent SetupTestCase()
	tr = CResourceIndexing::SetupTestCase();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CTextureIndexing::SetupTestCase() - SetupTestCase() failed.") );
		goto setupDone;
	}

setupDone:
	return tr;
}

/////

void CTextureIndexing::CleanupTestCase()
{
	// Cleanup the textures
	const UINT numTextures = ARRAY_SIZE(m_pResources);
	for( UINT i = 0; i < numTextures; ++i )
	{
		SAFE_RELEASE( m_pResources[i] );
		SAFE_RELEASE( m_pShaderResourceViews[i] );
	}

	CResourceIndexing::CleanupTestCase();
}

/////


TEST_RESULT CTextureIndexing::SetupResources()
{
	// Local variables
	TEST_RESULT tr			= RESULT_PASS;
	HRESULT hr				= S_OK;
	const UINT numTextures	= ARRAY_SIZE(m_pResources);
	D3D11_BUFFER_DESC buffDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	UINT initialResourceData = 0;
	D3D11_SUBRESOURCE_DATA initialData;

	//
	// Setup the intial data
	//
	initialData.pSysMem = &initialResourceData;

	//
	// Create descriptions for resources and views
	// Note: This test currently only cycles on buffer and texture 2d resources
	//

	switch( m_resourceType )
	{
	default:
		tr = RESULT_FAIL;
		WriteToLog( _T("CTextureIndexing::SetupResources() - Unknown resource type being used for the test.") );
		goto setupDone;
		break;

	case D3D_RESOURCE_TYPE_BUFFER:
		// Buffer description
		buffDesc.ByteWidth = sizeof(UINT);
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;
		buffDesc.StructureByteStride = 0;

		// Initial data
		initialData.SysMemPitch = 0;		// Doesn't matter for Buffer
		initialData.SysMemSlicePitch = 0;	// Doesn't matter for Buffer

		// Shader resource view
		srvDesc.Format = DXGI_FORMAT_R32_UINT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.ElementWidth = 1;
		break;

	case D3D_RESOURCE_TYPE_TEXTURE2D:
		// Texture2D description
		tex2DDesc.Width = 1;
		tex2DDesc.Height = 1;
		tex2DDesc.MipLevels = 0;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.MiscFlags = 0;

		// Initial data
		const UINT numBytesPerElement = GetBitsPerElement( tex2DDesc.Format ) / 8;
		initialData.SysMemPitch = numBytesPerElement * tex2DDesc.Width;
		initialData.SysMemSlicePitch = 0;	// Doesn't matter for 2D

		// Shader resource view
		srvDesc.Format = DXGI_FORMAT_R32_UINT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		break;
	}
	
	//
	// Create the textures
	//

	for( UINT i = 0; i < numTextures; ++i )
	{
		// Compute the color for the texture (each texture must be unique)
		initialResourceData = GetExpectedDataForResourceInSlot(i);

		if( D3D_RESOURCE_TYPE_BUFFER == m_resourceType )
		{
			// Create the buffer resource
			if( FAILED( hr = GetDevice()->CreateBuffer( &buffDesc, &initialData, reinterpret_cast<ID3D11Buffer**>(&(m_pResources[i]))) ) )
			{
				tr = RESULT_FAIL;
				WriteToLog( _T("CTextureIndexing::SetupResources() - Failed to Create Buffer (m_pResources[%d]). HR = %s"), i, D3DHResultToString(hr));
				goto setupDone;
			}
		}
		else if( D3D_RESOURCE_TYPE_TEXTURE2D == m_resourceType )
		{
			// Create the texture2d resource
			if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, &initialData, reinterpret_cast<ID3D11Texture2D**>(&(m_pResources[i]))) ) )
			{
				tr = RESULT_FAIL;
				WriteToLog( _T("CTextureIndexing::SetupResources() - Failed to Create Texture2D (m_pResources[%d]). HR = %s"), i, D3DHResultToString(hr));
				goto setupDone;
			}
		}
	}

	//
	// Create the views
	//

	for( UINT i = 0; i < numTextures; ++i )
	{
		// Compute the color for the texture (each texture must be unique)
		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pResources[i], &srvDesc, &(m_pShaderResourceViews[i])) ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CTextureIndexing::SetupResources() - Failed to Create ShaderResourceView (m_pShaderResourceViews[%d]). HR = %s"), i, D3DHResultToString(hr));
			goto setupDone;
		}
	}

	// Bind the resource views to the pipeline
	{
		ID3D11ShaderResourceView* pNULLSRV[numTextures];
		ZeroMemory( pNULLSRV, sizeof(pNULLSRV) );

		GetEffectiveContext()->VSSetShaderResources(0, numTextures, pNULLSRV);
		GetEffectiveContext()->HSSetShaderResources(0, numTextures, pNULLSRV);
		GetEffectiveContext()->DSSetShaderResources(0, numTextures, pNULLSRV);
		GetEffectiveContext()->GSSetShaderResources(0, numTextures, pNULLSRV);
		GetEffectiveContext()->PSSetShaderResources(0, numTextures, pNULLSRV);
		GetEffectiveContext()->CSSetShaderResources(0, numTextures, pNULLSRV);
	}

	switch( m_shaderStage )
	{
	default:
		tr = RESULT_FAIL;
		WriteToLog( _T("CTextureIndexing::SetupResources() - Invalid shader stage used in the test.") );
		goto setupDone;
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetShaderResources(0, numTextures, m_pShaderResourceViews);
		break;
	}	

setupDone:
	return tr;
}

/////

UINT CTextureIndexing::GetAFailingTextureSlot() const
{ 
	// Local variables
	UINT failingSlot = 0; 
	
	failingSlot = m_successTextureSlot + 1;

	if( failingSlot >= ARRAY_SIZE(m_pResources) )
		failingSlot = 0;

	return failingSlot;
}

/////

TEST_RESULT CTextureIndexing::VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource )
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	UINT *pResult = (UINT*)(mappedResource.pData);

	// Get the expected result
	const UINT expectedTextureResult = GetExpectedDataForResourceInSlot( m_successTextureSlot );

	// Get the actual result
	const UINT actualTextureResult = pResult[0];

	if( actualTextureResult != expectedTextureResult )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CTextureIndexing::VerifyResult() - The actual result did not match the expected result."));
		WriteToLog( _T(" Actual result = %d (%#x)"), actualTextureResult, actualTextureResult );
		WriteToLog( _T(" Expected result = %d (%#x)"), expectedTextureResult, expectedTextureResult);
		goto verifyDone;
	}

verifyDone:
	return tr;
}