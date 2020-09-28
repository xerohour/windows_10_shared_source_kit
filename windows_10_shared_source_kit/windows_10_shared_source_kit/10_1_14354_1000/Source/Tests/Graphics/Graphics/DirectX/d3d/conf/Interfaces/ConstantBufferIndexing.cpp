
// Includes
#include "ResourceIndexing.h"

// Names value sets
BEGIN_NAMED_VALUES( FAILING_INSTANCE )
	  NAMED_VALUE( _T( "DifferentSlot" ), FAILING_INSTANCE_BY_SLOT )
	  NAMED_VALUE( _T( "SameSlotDifferentOffset" ), FAILING_INSTANCE_BY_OFFSET )
END_NAMED_VALUES( FAILING_INSTANCE )

//------------------------------------------------------------------------------------------------
// CConstantBufferIndexing
//------------------------------------------------------------------------------------------------

CConstantBufferIndexing::~CConstantBufferIndexing(void)
{
}

/////

void CConstantBufferIndexing::InitTestParameters()
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
	pDimensionValues->AddValue(D3D_RESOURCE_TYPE_CONSTANT_BUFFER);

	testfactor::RFactor rfResourceDimension = AddParameterValueSet( pDimensionParam, pDimensionValues );

	//
	// Instance slot
	//

	CTestCaseParameter<UINT> *pInstanceSlotParam = AddParameter( _T("InstanceSlot"), &m_successInstanceSlot);

	CRangeValueSet<UINT> *pInstanceSlotValues = new CRangeValueSet<UINT>(0, MAX_INTERFACE_SLOTS - 1, 1);

	testfactor::RFactor rfInstanceSlot = AddParameterValueSet( pInstanceSlotParam, pInstanceSlotValues );

	//
	// "Fail by"...pick "slot" or "offset" slot
	// Thus a failing instance will differ from the success instance by either a consant buffer slot id or offset into the same slot.
	//

	CTestCaseParameter<FAILING_INSTANCE> *pFailByParam = AddParameter( _T("FailingInstance"), &m_failBy);

	CUserValueSet<FAILING_INSTANCE> *pFailByValues = new CUserValueSet<FAILING_INSTANCE>();
	pFailByValues->AddValue(FAILING_INSTANCE_BY_SLOT);
	pFailByValues->AddValue(FAILING_INSTANCE_BY_OFFSET);

	testfactor::RFactor rfFailBy = AddParameterValueSet( pFailByParam, pFailByValues );

	//
	// Constant buffer slot
	//

	CTestCaseParameter<UINT> *pConstantBufferSlotParam = AddParameter( _T("ConstantBufferSlot"), &m_successConstantBufferSlot);

	CRangeValueSet<UINT> *pConstantBufferSlotValues = new CRangeValueSet<UINT>(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1, 1);

	testfactor::RFactor rfConstantBufferSlot = AddParameterValueSet( pConstantBufferSlotParam, pConstantBufferSlotValues );

	//
	// Constant buffer size
	//

	CTestCaseParameter<UINT> *pConstantBufferNumComponentsParam = AddParameter( _T("ConstantBufferSizeInElements"), &m_numElementsInConstantBuffer);

	CUserValueSet<UINT> *pConstantBufferNumComponentsValues = new CUserValueSet<UINT>();
	//pConstantBufferNumComponentsValues->AddValue(1);
	//pConstantBufferNumComponentsValues->AddValue(2);
	pConstantBufferNumComponentsValues->AddValue(5);
	pConstantBufferNumComponentsValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);	// Max size

	testfactor::RFactor rfConstantBufferNumComponents = AddParameterValueSet( pConstantBufferNumComponentsParam, pConstantBufferNumComponentsValues );

	//
	// Constant buffer offset
	// Note: Offsets are on 16-byte boundaries. I.e Offset of 1 = the 5th component in the constant buffer
	//

	CTestCaseParameter<UINT> *pConstantBufferOffsetParam = AddParameter( _T("ConstantBufferOffsetInElements"), &m_successConstantBufferOffset);

	CUserValueSet<UINT> *pConstantBufferOffsetValues = new CUserValueSet<UINT>();
	pConstantBufferOffsetValues->AddValue(0);
	pConstantBufferOffsetValues->AddValue(1);
	pConstantBufferOffsetValues->AddValue(2);
	pConstantBufferOffsetValues->AddValue(3);
	pConstantBufferOffsetValues->AddValue(4);
	pConstantBufferOffsetValues->AddValue(5);
	pConstantBufferOffsetValues->AddValue(6);
	pConstantBufferOffsetValues->AddValue(777);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 6);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 5);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 4);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 3);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 2);
	pConstantBufferOffsetValues->AddValue(D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT - 1);

	testfactor::RFactor rfConstantBufferOffset = AddParameterValueSet( pConstantBufferOffsetParam, pConstantBufferOffsetValues );

	// Set test factor
	SetRootTestFactor( ( rfResourceDimension * ( ((rfInstanceSlot % rfConstantBufferSlot) * rfConstantBufferNumComponents * rfConstantBufferOffset) % rfFailBy ) ) % rfShaderStage );
}

/////

TEST_RESULT CConstantBufferIndexing::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	m_failConstantBufferOffset = m_successConstantBufferOffset;
	m_failConstantBufferSlot = m_successConstantBufferSlot;

	//
	// Determine the texture slot that fails
	// Note: Only one out of all bound textures will be the passing slot, all others are failing
	//

	switch( m_failBy )
	{
	default:
		tr = RESULT_FAIL;
		WriteToLog( _T("CConstantBufferIndexing::SetupTestCase() - Invalid value for m_failBy used in the testcase.") );
		goto setupDone;
		break;

	case FAILING_INSTANCE_BY_OFFSET:

		m_failConstantBufferOffset = GetAFailingConstantBufferOffset();

		if( !IsFailingConstantBufferOffset( m_failConstantBufferOffset ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CConstantBufferIndexing::SetupTestCase() - Failed to generate a failing ConstantBuffer component offset.") );
			WriteToLog( _T(" Generated component offset = %d"), m_failConstantBufferOffset );
			goto setupDone;
		}

		break;
	case FAILING_INSTANCE_BY_SLOT:

		m_failConstantBufferSlot = GetAFailingConstantBufferSlot();

		if( !IsFailingConstantBufferSlot( m_failConstantBufferSlot ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CConstantBufferIndexing::SetupTestCase() - Failed to generate a failing ConstantBuffer slot index.") );
			WriteToLog( _T(" Generated index = %d"), m_failConstantBufferSlot );
			goto setupDone;
		}

		break;
	}
	
	// Call the parent SetupTestCase()
	tr = CResourceIndexing::SetupTestCase();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CConstantBufferIndexing::SetupTestCase() - SetupTestCase() failed.") );
		goto setupDone;
	}

setupDone:
	return tr;
}

/////

void CConstantBufferIndexing::CleanupTestCase()
{
	// Cleanup the ConstantBuffer
	const UINT numConstantBuffers = ARRAY_SIZE( m_ppConstantBuffers );

	m_pConstantBufferForInstanceSlot = NULL;

	for( UINT i = 0; i < numConstantBuffers; ++i )
		SAFE_RELEASE( m_ppConstantBuffers[i] );

	CResourceIndexing::CleanupTestCase();
}

/////


TEST_RESULT CConstantBufferIndexing::SetupResources()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_BUFFER_DESC constantBufferDesc;
	UINT bufferByteWidth = 0;
	const UINT numConstantBuffers = ARRAY_SIZE(m_ppConstantBuffers);
	const UINT numComponentsInConstantBuffer = m_numElementsInConstantBuffer * 4;
	int *data = new int[numComponentsInConstantBuffer];
	int dataToWrite;
	D3D11_SUBRESOURCE_DATA initialData;

	ZeroMemory( &initialData, sizeof(initialData) );
	ZeroMemory( &constantBufferDesc, sizeof(constantBufferDesc) );

	// Unbind all existing constant buffers (especially the one for the instance slot, m_pConstantBufferForInstanceSlot)
	ID3D11Buffer *nullCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

	for( UINT i = 0; i < ARRAY_SIZE(nullCBs); ++i )
		nullCBs[i] = NULL;

	GetEffectiveContext()->VSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	GetEffectiveContext()->HSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	GetEffectiveContext()->DSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	GetEffectiveContext()->GSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	GetEffectiveContext()->PSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	GetEffectiveContext()->CSSetConstantBuffers( 0, ARRAY_SIZE(nullCBs), nullCBs );
	SAFE_RELEASE( m_pConstantBufferForInstanceSlot );
	m_pConstantBufferForInstanceSlot = NULL;

	//
	// Create the constant buffers
	//

	// Determine the byte width to use
	bufferByteWidth = m_numElementsInConstantBuffer * 4 * sizeof(int);

	if( bufferByteWidth < 16 )	// Byte width must be a multiple of 16
		bufferByteWidth = 16;

	// Create the buffer description
	constantBufferDesc.ByteWidth = bufferByteWidth;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// Create the buffers
	for( UINT i = 0; i < numConstantBuffers; ++i )
	{
		// Setup the initial data in the CB to be unique
		for( UINT dataIndex = 0; dataIndex < numComponentsInConstantBuffer; ++dataIndex )
		{
			dataToWrite = GetExpectedDataForResourceInSlot( i, dataIndex );
			data[dataIndex] = dataToWrite;
		}

		// Set the initial data
		initialData.pSysMem = data;

		// Create the constant buffer
		if( FAILED( hr = GetDevice()->CreateBuffer( &constantBufferDesc, &initialData, &(m_ppConstantBuffers[i]) ) ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CConstantBufferIndexing::SetupResources() - Failed to create the constant buffer (m_ppConstantBuffers[%d]). HR = %s"), i, D3DHResultToString(hr).c_str());
			goto setupDone;
		}
	}

	// Set the cb for which to pass instance slot information to the shader
	m_pConstantBufferForInstanceSlot = m_ppConstantBuffers[0];
	m_bConstantBufferForInstanceSlotInitialized = true;	// indicate that this buffer is already initialized so that later we don't map it with discard flag

	switch( m_shaderStage )
	{
	default:
		tr = RESULT_FAIL;
		WriteToLog( _T("CConstantBufferIndexing::SetupResources() - Invalid shader stage used in the test.") );
		goto setupDone;
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetConstantBuffers( 0, numConstantBuffers, m_ppConstantBuffers );
		break;
	}	

setupDone:
	// Cleanup
	if( data )
		delete [] data;

	return tr;
}

/////

UINT CConstantBufferIndexing::GetAFailingConstantBufferSlot() const
{ 
	// Local variables
	UINT failingSlot = 0; 
	
	failingSlot = m_successConstantBufferSlot + 1;

	if( failingSlot >= ARRAY_SIZE(m_ppConstantBuffers) )
		failingSlot = 0;

	return failingSlot;
}

/////

UINT CConstantBufferIndexing::GetAFailingConstantBufferOffset() const
{ 
	// Local variables
	UINT failingOffset = 0; 
	
	failingOffset = m_successConstantBufferOffset + 1;

	if( failingOffset >= m_numElementsInConstantBuffer )
		failingOffset = 0;

	return failingOffset;
}

/////

int CConstantBufferIndexing::GetExpectedDataForResourceInSlot(const UINT &slot, const UINT &componentOffset) const 
{ 
	// Local variables
	const UINT numComponentsInConstantBuffer = m_numElementsInConstantBuffer * 4;

	// The zero'th constant buffer always constains the instance slot to use. See the shader code.
	if( 0 == slot && 0 == componentOffset )
		return m_successInstanceSlot;

	// Check to see if we are readying out of bounds
	if( componentOffset >= numComponentsInConstantBuffer )
		return 0;

	return (-1 * ((slot * numComponentsInConstantBuffer) + componentOffset)); 
};

/////

TEST_RESULT CConstantBufferIndexing::VerifyResult( const D3D11_MAPPED_SUBRESOURCE &mappedResource )
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	const UINT successComponentIndex = m_successConstantBufferOffset * 4;
	const bool expectInstanceSlot = (0 == m_successConstantBufferSlot) && (0 == successComponentIndex);
	int *pResult = (int*)(mappedResource.pData);

	// Get the expected result
	// Note we multiply by 4 because when we offset in the constant buffer it is per element (16-bytes).
	const int expectedConstantBufferResult = GetExpectedDataForResourceInSlot( m_successConstantBufferSlot, successComponentIndex );

	// Get the actual result
	const int actualConstantBufferResult = pResult[0];

	if( !expectInstanceSlot && ((actualConstantBufferResult % 4) != 0) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CConstantBufferIndexing::VerifyResult() - The actual result is not on a 16-byte boundary."));
		WriteToLog( _T(" Offsets into a constant buffer are on 16-byte (4 component) boundaries."));
		WriteToLog( _T(" Actual result = %d (%#x)"), actualConstantBufferResult, actualConstantBufferResult );
		goto verifyDone;
	}

	if( actualConstantBufferResult != expectedConstantBufferResult )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CConstantBufferIndexing::VerifyResult() - The actual result did not match the expected result."));
		WriteToLog( _T(" Actual result = %d (%#x)"), actualConstantBufferResult, actualConstantBufferResult );
		WriteToLog( _T(" Expected result = %d (%#x)"), expectedConstantBufferResult, expectedConstantBufferResult);
		goto verifyDone;
	}

verifyDone:
	return tr;
}