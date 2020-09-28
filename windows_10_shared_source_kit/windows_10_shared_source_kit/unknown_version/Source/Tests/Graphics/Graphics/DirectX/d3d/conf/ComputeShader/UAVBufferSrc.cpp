#include "d3dx11async.h"

#include "UAV.h"


//----------------------------------------------------------------------------------------
//
// UAVBufferSrc.cpp - CUAVBufferSrcTest
//
//
//
// This abstract test class exercises the following input buffers:
//
// - ByteAddressBuffer
// - StructuredBuffer
//
// All threads/thread groups/pixel shaders read into the same region of the buffer.
//
// See UAVBufferSrcPS.cs and UAVBufferSrcCS.cs for concrete implementation.
//
// (Using RW******Buffer for input is tested in CUAVBufferRWTest.)
//
// The test initializes the input buffer with a repeated pattern, and verify this pattern 
// in the shader.
//
// Out-of-bound/non-DWORD-aligned reads are also tested.
// 


//----------------------------------------------------------------------------------------

void CUAVBufferSrcTest::CreateInitialData()
{
	// initialize test pattern into m_pInitialData

	UINT testPatternSize;
	void* pTestPattern = NULL;
	UIntGroup testDataUIntGroup( UAV_BUFFER_SRC_TEST_PATTERN_UINTGROUP );
	UINT testDataUInt[] = { UAV_BUFFER_SRC_TEST_PATTERN_UINT };
	SIntGroup testDataSIntGroup( UAV_BUFFER_SRC_TEST_PATTERN_SINTGROUP );
	FloatGroup testDataFloatGroup( UAV_BUFFER_SRC_TEST_PATTERN_FLOATGROUP );
	MixGroup testDataMixGroup( UAV_BUFFER_SRC_TEST_PATTERN_MIXGROUP );

	switch( m_SrcResourceType )
	{
	case RESOURCE_STRUCTUREDBUFFER_4UINT:
		{
			testPatternSize = sizeof( UIntGroup );
			pTestPattern = &testDataUIntGroup;
			m_uBufferElementWidth = sizeof( UIntGroup );
			m_uBufferStride = sizeof( UIntGroup );
			break;
		}
	case RESOURCE_BYTEADDRESSBUFFER:
		{
			testPatternSize = sizeof( UINT ) * 4;
			pTestPattern = testDataUInt;
			m_uBufferElementWidth = sizeof( UINT );
			break;
		}
	case RESOURCE_STRUCTUREDBUFFER_2SINT:
		{
			testPatternSize = sizeof( SIntGroup );
			pTestPattern = &testDataSIntGroup;
			m_uBufferElementWidth = sizeof( SIntGroup );
			m_uBufferStride = sizeof( SIntGroup );
			break;
		}
	case RESOURCE_STRUCTUREDBUFFER_1FLOAT:
		{
			testPatternSize = sizeof( FloatGroup );
			pTestPattern = &testDataFloatGroup;
			m_uBufferElementWidth = sizeof( FloatGroup );
			m_uBufferStride = sizeof( FloatGroup );
			break;
		}
	case RESOURCE_STRUCTUREDBUFFER_MIX:
		{
			testPatternSize = sizeof( MixGroup );
			pTestPattern = &testDataMixGroup;
			m_uBufferElementWidth = sizeof( MixGroup );
			m_uBufferStride = sizeof( MixGroup );
			break;
		}		
	}

	// copy pattern repeatedly into m_pInitialData
	char* pTargetData = (char*) m_pInitialData; // make ptr arithmetic work out
	for( UINT i = 0; i < MAX_RESULT_SIZE / testPatternSize; ++i )
	{
		memcpy( pTargetData + ( i * testPatternSize ), pTestPattern, testPatternSize );
	}
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcTest::CreateShaderResource()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	
	// Create a blank shader resource
	D3D11_BUFFER_DESC descBuffer;
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;

	descBuffer.ByteWidth = UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT;
	descBuffer.Usage = D3D11_USAGE_DYNAMIC;
	descBuffer.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	descBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	descBuffer.StructureByteStride = m_uBufferStride;

	BYTE* blankByte = new BYTE[UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT];
	memset( blankByte, 0, UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT );
	D3D11_SUBRESOURCE_DATA subresData;
	subresData.pSysMem = (void*)blankByte;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &descBuffer, &subresData, &m_pSrcBufferBlank ) ) )
	{
		WriteToLog( _T( "CUAVBufferSrcTest::CreateShaderResource() - Failed to create SR buffer, hr=%s"), D3DHResultToString( hr ).c_str() );
		delete[] blankByte;
		return RESULT_FAIL;
	}
	delete[] blankByte;

	descSRV.Format = DXGI_FORMAT_R32_TYPELESS;
	descSRV.ViewDimension = GetSRVDimension( RESOURCE_BYTEADDRESSBUFFER );	 
	descSRV.BufferEx.FirstElement = 0;	
	descSRV.BufferEx.NumElements = UAV_BUFFER_SRC_TEST_BLANK_BUFFER_BYTE_COUNT / 4;	
	descSRV.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pSrcBufferBlank, &descSRV, &m_pSRVBlank ) ) )
	{
		WriteToLog(_T("CUAVBufferSrcTest::CreateShaderResource() - Failed to create SRV, hr=%s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return CUAVTest::CreateShaderResource();
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcTest::CreateUAV()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = m_uTotalThreadCount * sizeof( UINT );
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; 
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = sizeof( UINT );

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, ( ID3D11Buffer** )&m_pUAVResource ) ) )
	{
		WriteToLog( _T( "CUAVBufferSrcTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	bufDesc.ByteWidth = m_uTotalThreadCount * sizeof( UINT );
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.BindFlags = 0;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, (ID3D11Buffer** )&m_pUAVResourceStaging ) ) )
	{
		WriteToLog( _T( "CUAVBufferSrcTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = GetUAVDimension( RESOURCE_STRUCTUREDBUFFER );
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( UINT );

	if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pUAVResource, &uavDesc, &m_pUAV ) ) )
	{
		WriteToLog( _T( "CUAVBufferSrcTest::CreateUAV() failed to create UAV, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferSrcTest::CompareResults( void *pResult )
{
	// each DWORD in the UAV represents result from a single thread.  Test writes a 1 into it
	// if test passes.

	TEST_RESULT testResult = RESULT_PASS;
	UINT* pResultUInt = (UINT*)pResult;
	for( UINT i = 0; i < m_uTotalThreadCount; ++i )
	{
		if ( pResultUInt[i] != 1 )
		{
			WriteToLog( "CUAVBufferSrcTest::CompareResults() - thread %d read incorrect data.\n", i );
			testResult = RESULT_FAIL;
			break;
		}
	}

	return testResult;
}

//----------------------------------------------------------------------------------------

void CUAVBufferSrcTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pSRVBlank);
	SAFE_RELEASE(m_pSrcBufferBlank);
	CUAVTest::CleanupTestCase();
}
