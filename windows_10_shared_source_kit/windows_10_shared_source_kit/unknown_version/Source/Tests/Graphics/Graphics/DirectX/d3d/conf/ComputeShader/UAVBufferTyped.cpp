#include "d3dx11async.h"
#include "D3DMSHelper.h"

#include "UAV.h"

//----------------------------------------------------------------------------------------
//
// UAVBufferTyped.cpp - CUAVBufferTypedTest
//
//
//
// This abstract test class is intended to exercise the following UAV buffers:
//
// - RWBuffer of type uint, float, and int with various supported DXGI format
//
// See UAVBufferTypedPS.cs and UAVBufferTypedCS.cs for concrete implementation.
//
// The shader repeatedly writes two alternative values into the UAV. The test verifies
// that the correct value is written.
//
// Note that loading from a UAV only works with R32_*, and is not checked here.  This is tested in 
// UAVBufferRWTest.
// 

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedTest::SetupTestCase()
{
	if ( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		return RESULT_SKIP;
	}
	return CUAVBufferTest::SetupTestCase();
}

//----------------------------------------------------------------------------------------
//
//	Shader code alternately writes two values into the UAV. This is the 2nd value 
//	for this DXGI format.
//

float CUAVBufferTypedTest::GetExpectedValue1( DXGI_FORMAT format )
{
	switch( format )
	{
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_UNORM;

	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R16_SNORM:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_SNORM;

	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_FLOAT;

	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R32_UINT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_UINT;

	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_R32_SINT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_1_SINT;
	}

	assert( false ); // should not be here
	return 0; 
}

//----------------------------------------------------------------------------------------
//
//	Shader code alternately writes two values into the UAV. This is the 2nd value 
//	for this DXGI format.
//

float CUAVBufferTypedTest::GetExpectedValue2( DXGI_FORMAT format )
{
	switch( format )
	{
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_UNORM;

	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R16_SNORM:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_SNORM;

	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_FLOAT;

	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R32_UINT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_UINT;

	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_R32_SINT:
		return UAV_BUFFER_TYPED_TEST_EXPECTED_RESULT_2_SINT;
	}

	assert( false ); // should not be here
	return 0; 
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedTest::CreateUAV()
{
	// Now start creating the UAV

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = m_uByteWidth;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, (ID3D11Buffer**)&m_pUAVResource ) ) )
	{
		WriteToLog( _T( "CUAVBufferTypedTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	bufDesc.ByteWidth = m_uByteWidth;
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.BindFlags = 0;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, (ID3D11Buffer**)&m_pUAVResourceStaging ) ) )
	{
		WriteToLog( _T( "CUAVBufferTypedTest::CreateUAV() failed to create buffer, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = GetUAVDimension( m_UAVResourceType );
	uavDesc.Format = m_UAVFormat; 
	uavDesc.Buffer.Flags = 0; 
	uavDesc.Buffer.FirstElement = 0; 
	uavDesc.Buffer.NumElements = bufDesc.ByteWidth / GetByteSizePerColor( m_UAVFormat );

	if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pUAVResource, &uavDesc, &m_pUAV ) ) )
	{
		WriteToLog( _T( "CUAVBufferTypedTest::CreateUAV() failed to create UAV, hr=%s" ), D3DHResultToString( hr ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------

TEST_RESULT CUAVBufferTypedTest::CompareResults( void *pResult )
{
	// Shader alternately writes two values into the UAV. Each thread writes into its 
	// own section of the UAV.  This code verifies that the expected pattern exists in the UAV.
	// Keep in mind that store_uav_typed always have a .xyzw mask, so each channel will have
	// the same value.

	TEST_RESULT testResult = RESULT_PASS;

	UINT numElements = m_uByteWidth / GetByteSizePerColor( m_UAVFormat );
	FLOAT_COLOR *floatColors = new FLOAT_COLOR[ numElements ];
	if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, m_UAVFormat, floatColors, 1, numElements, pResult ))
	{
		WriteToLog( "CUAVBufferTypedTest::CompareResults() - CFormatHelper::ConvertPixels() failed" );
		testResult = RESULT_FAIL;
		goto methodEnd;
	}

	bool hasRedChannel = HasRedChannel( m_UAVFormat );
	bool hasGreenChannel = HasGreenChannel( m_UAVFormat );
	bool hasBlueChannel = HasBlueChannel( m_UAVFormat );
	bool hasAlphaChannel = HasAlphaChannel( m_UAVFormat );
	float expectedValue1 = GetExpectedValue1( m_UAVFormat );
	float expectedValue2 = GetExpectedValue2( m_UAVFormat );

	for (UINT i = 0; i < numElements; i++)
	{
		bool isPass = true;
		float expectedValue = i % 2 == 0 ? expectedValue1 : expectedValue2;
		float expectedRed = hasRedChannel ? expectedValue : 0.0f;
		float expectedGreen = hasGreenChannel ? expectedValue : 0.0f;
		float expectedBlue = hasBlueChannel ? expectedValue : 0.0f;
		float expectedAlpha = hasAlphaChannel ? expectedValue : 1.0f;
		
		if ( !EqualWithinTolerance( expectedRed, expectedGreen, expectedBlue, expectedAlpha, floatColors[i], m_UAVFormat ) )
		{
			WriteToLog( "CUAVBufferTypedTest::CompareResults() - color#%d in buffer is incorrect.\n", i * 4 );
			WriteToLog( " Expected value in channels (if available) = %f \n", expectedValue );
			WriteToLog( " Actual RGBA value (in float) = %f,%f,%f,%f \n", floatColors[ i ].r, floatColors[ i ].g, floatColors[ i ].b, floatColors[ i ].a );
			testResult = RESULT_FAIL;
			break;
		}
	}

methodEnd:
	delete [] floatColors;

	return testResult;
}
