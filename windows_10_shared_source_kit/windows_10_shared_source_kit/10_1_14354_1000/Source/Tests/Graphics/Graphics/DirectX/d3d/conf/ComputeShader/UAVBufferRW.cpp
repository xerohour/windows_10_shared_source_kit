#include "d3dx11async.h"

#include "UAV.h"


//----------------------------------------------------------------------------------------
//
// UAVBufferRW.cpp - CUAVBufferRWTest
//
//
//
// This abstract test class is intended for exercising the following UAV buffers :
//
// - RWStructureBuffer - read/write at various members of struct
// - RWBuffer of type uint, float, and int with R32_* - read/write
// - RWByteAddressBuffer - load/load2/load3/load4/store/store2/store3/store4
//
// There should be no overlaps in the accessed region of the UAV 
// between threads/thread groups/pixel shader.
//
// See UAVBufferRWPS.cs andUAVBufferRWCS.cs for concrete implementation.
//
// This test exercises both using the UAV as an input resource and an output resource.
//
// The test initializes the UAV with some preset values. The shader code reads the initial value, 
// increment portion of the memory based on a pattern, and write it back into the UAV. During 
// this shader, the code does multiple consecutive reads/writes at the same memory. The test 
// then checks for the expected values in the UAV after the shader executes.
//
// Out-of-bound / non-DWORD-aligned reads/writes will also be exercised.
// 
// Note that typed UAV loads are only supported for single-component elements.
//

//----------------------------------------------------------------------------------------

void CUAVBufferRWTest::CreateInitialData()
{
	// Need an clear buffer - default size == DEFAULT_BUFFER_SIZE
	// Shader uses this to add in bogus conditionals and prevent compiler optimization from
	// killing out test cases
	m_SrcResourceType = RESOURCE_BYTEADDRESSBUFFER; 
	m_srcFormat = DXGI_FORMAT_R32_FLOAT;
	char* pTargetData = (char*) m_pInitialData; // make ptr arithmetic work out
	memset( m_pInitialData, 0, MAX_RESULT_SIZE );	

	// initialize test pattern in UAV that will be read by the shader
	if ( m_UAVResourceType == RESOURCE_RWBUFFER_FLOAT4 || m_UAVResourceType == RESOURCE_RWBUFFER_UINT2 )
	{
		// special case - init to zero because these UAVs can't be loaded in shader.  (Write-only.)
		m_uintTestPattern[0] = 0;
		m_uintTestPattern[1] = 0; 
		m_uintTestPattern[2] = 0; 
		m_uintTestPattern[3] = 0;
		m_sintTestPattern[0] = 0;
		m_sintTestPattern[1] = 0; 
		m_sintTestPattern[2] = 0;
		m_sintTestPattern[3] = 0;
		m_floatTestPattern[0] = 0;
		m_floatTestPattern[1] = 0;
		m_floatTestPattern[2] = 0;
		m_floatTestPattern[3] = 0;
	}
	else
	{
		m_uintTestPattern[0] = 17895697; // 0x01111111
		m_uintTestPattern[1] = 2147483647; // 0x7FFFFFFF
		m_uintTestPattern[2] = 4096; 
		m_uintTestPattern[3] = 0;
		m_sintTestPattern[0] = -100000;
		m_sintTestPattern[1] = 17895697; // 0x01111111
		m_sintTestPattern[2] = 4096;
		m_sintTestPattern[3] = 0;
		m_floatTestPattern[0] = 24;
		m_floatTestPattern[1] = 1520;
		m_floatTestPattern[2] = -4096;
		m_floatTestPattern[3] = 0;
	}
	m_mixTestPattern.b1 = 0;
	m_mixTestPattern.b2 = 0;
	m_mixTestPattern.b3 = 0;
	m_mixTestPattern.b4 = 0;
	m_mixTestPattern.b5 = 0;
	m_mixTestPattern.f1 = 1024;
	m_mixTestPattern.f2 = 2.5;
	m_mixTestPattern.f3 = 4;
	m_mixTestPattern.f4 = -2000;
	m_mixTestPattern.i1 = 17895697; // 0x01111111
	m_mixTestPattern.i2 = 150;
	m_mixTestPattern.i3 = -1;
	m_mixTestPattern.i4 = 0;
	m_mixTestPattern.i5 = 50;
	m_mixTestPattern.u1 = 2147483647; // 0x7FFFFFFF
	m_mixTestPattern.u2 = 17895697; // 0x01111111
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWTest::CreateUAV()
{
	// pick the right repeat pattern for UAV
	BYTE* initBufferBytes = new BYTE[ m_uByteWidth ];
	UINT testPatternSize;
	void* pTestPattern = NULL;

	switch( m_UAVResourceType )
	{
	case RESOURCE_RWBYTEADDRESSBUFFER:
	case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
	case RESOURCE_RWBUFFER_UINT:
	case RESOURCE_RWBUFFER_UINT2:
		pTestPattern = m_uintTestPattern;
		testPatternSize = sizeof( m_uintTestPattern );
		break;

	case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT:
	case RESOURCE_RWBUFFER_FLOAT:
	case RESOURCE_RWBUFFER_FLOAT4:
		pTestPattern = m_floatTestPattern;
		testPatternSize = sizeof( m_floatTestPattern );
		break;

	case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
	case RESOURCE_RWBUFFER_SINT:
		pTestPattern = m_sintTestPattern;
		testPatternSize = sizeof( m_sintTestPattern );
		break;

	case RESOURCE_RWSTRUCTUREDBUFFER_MIX:
		pTestPattern = &m_mixTestPattern;
		testPatternSize = sizeof( MixGroup );
		break;
	}

	char* pTargetData = (char*) initBufferBytes; // make ptr arithmetic easier
	for( UINT i = 0; i < m_uByteWidth / testPatternSize; ++i )
	{
		memcpy( pTargetData + ( i * testPatternSize ), pTestPattern, testPatternSize );
	}

	// Now start creating the UAV

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = m_uByteWidth;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	switch(m_UAVResourceType)
	{
	case RESOURCE_RWBYTEADDRESSBUFFER:
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT:
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = sizeof( float );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = sizeof( UINT ) * 4;
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = sizeof( int ) * 2;
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_MIX:
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = sizeof( MixGroup );
		break;
	}

	D3D11_SUBRESOURCE_DATA subresData;
	subresData.pSysMem = initBufferBytes;
	subresData.SysMemPitch = 0;
	subresData.SysMemSlicePitch = 0;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, &subresData, (ID3D11Buffer**)&m_pUAVResource ) ) )
	{
		WriteToLog( _T( "CUAVBufferRWTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	delete[] initBufferBytes;

	bufDesc.ByteWidth = m_uByteWidth;
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.BindFlags = 0;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, (ID3D11Buffer**)&m_pUAVResourceStaging ) ) )
	{
		WriteToLog( _T( "CUAVBufferRWTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = GetUAVDimension( m_UAVResourceType );
	uavDesc.Format = m_UAVFormat; 
	uavDesc.Buffer.Flags = 0; 
	uavDesc.Buffer.FirstElement = 0; 
	uavDesc.Buffer.NumElements = 0;

	switch(m_UAVResourceType)
	{
	case RESOURCE_RWBYTEADDRESSBUFFER:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( UINT );
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; 
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / ( sizeof( UINT ) * 4 );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / ( sizeof( int ) * 2 );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_MIX:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( MixGroup );
		break;
	case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT: // next few obviously the same...ah well.
	case RESOURCE_RWBUFFER_FLOAT:
	case RESOURCE_RWBUFFER_FLOAT4:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( float );
		break;
	case RESOURCE_RWBUFFER_SINT:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( int );
		break;
	case RESOURCE_RWBUFFER_UINT:
	case RESOURCE_RWBUFFER_UINT2:
		uavDesc.Buffer.NumElements = bufDesc.ByteWidth / sizeof( UINT );
		break;
	}

	if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pUAVResource, &uavDesc, &m_pUAV ) ) )
	{
		WriteToLog( _T( "CUAVTypedTest::CreateUAV() failed to create UAV, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferRWTest::CompareResults( void *pResult )
{
	// Test increments what already exists in the UAV by the following pattern per thread.
	// pattern:	1,0,0,0,    5,6,0,0,    9,10,11,0,    13,14,15,16
	//          17,0,0,0,   21,22,0,0   25,26,27,0,   29,30,31,32 ...
	//
	// e.g. if the 1st integer is originally 6, it becomes 6+1==7.  If the 17th int is 8, it becomse 8+17==25.
	//
	// Each thread writes the same thing to its own area of the UAV.

	TEST_RESULT testResult = RESULT_PASS;
	UINT* pResultUInt = (UINT*)pResult;
	int* pResultSInt = (int*)pResult;
	float* pResultFloat = (float*)pResult;

	if (m_UAVResourceType == RESOURCE_RWSTRUCTUREDBUFFER_MIX)
	{
		// unfortunately this large struct doesn't mix as nicely with the overall scheme of the test,
		// so we have to code this one separately.
		// This struct is 16-DWORD long...conveniently same length as shader's incremental pattern.

		// This code hasn't been verified yet. Waiting for compiler fix.  Currently test case is disabled.
		MixGroup* groups = (MixGroup*) pResult;
		for( UINT i = 0; i < m_uByteWidth / sizeof( MixGroup ); ++i )
		{
			bool isPass = true;

			if ( groups[i].b1 == m_mixTestPattern.b1 ) // #1 - shader flips boolean for this member
			{
				isPass = false;
			}
			if ( groups[i].f1 != m_mixTestPattern.f1 ) // #2 - not touched
			{
				isPass = false;
			}
			if ( groups[i].f2 != m_mixTestPattern.f2 ) // #3 - not touched
			{
				isPass = false;
			}
			if ( groups[i].f3 != m_mixTestPattern.f3 ) // #4 - not touched
			{
				isPass = false;
			}
			if ( groups[i].f4 != m_mixTestPattern.f4 + ( i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 5 ) // #5 - incremented by index
			{
				isPass = false;
			}
			if ( groups[i].b2 == m_mixTestPattern.b2 ) // #6 - flipped 
			{
				isPass = false;
			}
			if ( groups[i].b3 != m_mixTestPattern.b3 ) // #7 - not touched
			{
				isPass = false;
			}
			if ( groups[i].b4 != m_mixTestPattern.b4 ) // #8 - not touched
			{
				isPass = false;
			}
			if ( groups[i].b5 == m_mixTestPattern.b5 ) // #9 - flipped 
			{
				isPass = false;
			}
			if ( groups[i].i1 != m_mixTestPattern.i1 + ( i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 10 ) // #10 - incremented by index 
			{
				isPass = false;
			}
			if ( groups[i].u1 != m_mixTestPattern.u1 + ( i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 11 ) // #11 - incremented by index 
			{
				isPass = false;
			}
			if ( groups[i].u2 != m_mixTestPattern.u2 ) // #12 - not touched
			{
				isPass = false;
			}
			if ( groups[i].i2 != m_mixTestPattern.i2 + ( i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 13 ) // #13 - incremented by index 
			{
				isPass = false;
			}
			if ( groups[i].i3 != m_mixTestPattern.i3 + ( i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 14 ) // #14 - incremented by index 
			{
				isPass = false;
			}
			if ( groups[i].i4 != m_mixTestPattern.i4 + (i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 15 ) // #15 - incremented by index 
			{
				isPass = false;
			}
			if ( groups[i].i5 != m_mixTestPattern.i5 + (i % ( m_uThreadByteWidth / sizeof( MixGroup ) ) ) * 16 + 16 ) // #16 - incremented by index 
			{
				isPass = false;
			}

			if ( !isPass )
			{
			
				WriteToLog( "CUAVBufferRWTest::CompareResults() - struct #%d has incorrect result.\n", i );
				testResult = RESULT_FAIL;
				break;
			} 
		} 
	}
	else // (somewhat) general case
	{
		for( UINT i = 0; i < m_uByteWidth / 4; ++i )
		{
			UINT mod = i % 16;
			bool useOriginalValue = mod == 1 || mod == 2 || mod == 3 || mod == 6 || mod == 7 || mod == 11; // shader didn't write to these indices
			
			// need separate code for each data type
			switch( m_UAVResourceType )
			{
			case RESOURCE_RWBYTEADDRESSBUFFER:
			case RESOURCE_RWSTRUCTUREDBUFFER_4UINT:
			case RESOURCE_RWBUFFER_UINT:
			case RESOURCE_RWBUFFER_UINT2:
				{
					UINT originalValueInUAV = m_uintTestPattern[i % ( sizeof( m_uintTestPattern) / sizeof( UINT ) )];
					UINT expectedValue = useOriginalValue ? originalValueInUAV : originalValueInUAV + ( i % ( m_uThreadByteWidth / 4 ) ) + 1;
					if ( pResultUInt[i] != expectedValue )
					{
						WriteToLog( "CUAVBufferRWTest::CompareResults() - UINT at byte offset %u is incorrect.\n", i * 4 );
						WriteToLog( " Expected value = %u \n", expectedValue );
						WriteToLog( " Actual value = %u \n", pResultUInt[i] );
						testResult = RESULT_FAIL;
					}
					break;
				}
			case RESOURCE_RWSTRUCTUREDBUFFER_1FLOAT:
			case RESOURCE_RWBUFFER_FLOAT:
			case RESOURCE_RWBUFFER_FLOAT4:
				{
					float originalValueInUAV = m_floatTestPattern[i % ( sizeof( m_floatTestPattern ) / sizeof( float ) )];
					float expectedValue = useOriginalValue ? originalValueInUAV : originalValueInUAV + ( i % ( m_uThreadByteWidth / 4 ) ) + 1;
					if ( pResultFloat[i] != expectedValue )
					{
						WriteToLog( "CUAVBufferRWTest::CompareResults() - FLOAT at byte offset %u is incorrect.\n", i * 4);
						WriteToLog( " Expected value = %f \n", expectedValue );
						WriteToLog( " Actual value = %f \n", pResultFloat[i] );
						testResult = RESULT_FAIL;
					}
					break;
				}
			case RESOURCE_RWSTRUCTUREDBUFFER_2SINT:
			case RESOURCE_RWBUFFER_SINT:
				{
					UINT originalValueInUAV = m_sintTestPattern[i % ( sizeof( m_sintTestPattern ) / sizeof( int ) )];
					UINT expectedValue = useOriginalValue ? originalValueInUAV : originalValueInUAV + ( i % ( m_uThreadByteWidth / 4 ) ) + 1;
					if ( pResultSInt[i] != expectedValue )
					{
						WriteToLog( "CUAVBufferRWTest::CompareResults() - SINT at byte offset %u is incorrect.\n", i * 4 );
						WriteToLog( " Expected value = %d \n", expectedValue );
						WriteToLog( " Actual value = %d \n", pResultSInt[i] );
						testResult = RESULT_FAIL;
					}
					break;
				}
			}

			if ( testResult == RESULT_FAIL )
			{
				break;
			}
		}
	}

	return testResult;
}
