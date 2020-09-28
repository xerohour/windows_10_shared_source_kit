//////////////////////////////////////////////////////////////////////////
//  ConstantBuffers.cpp
//  created:	2006/06/29
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "ConstantBuffers.h"

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"
#include <cassert>

void CShader5xTest_ConstantBuffers::InitTestParameters()
{
	CTestCaseParameter<tstring> *pSlotsParam = AddParameter( _T( "SlotContents" ), &m_szSlotContents );
	CUserValueSet<tstring> *pSlotValues = new CUserValueSet<tstring>( );
	pSlotValues->AddValue( "CCCCCCCCCCCCCCCCC" );
	pSlotValues->AddValue( "CEEEEEEEEEEEEEEEE" );
	pSlotValues->AddValue( "CECECECECECECECEC" );
	pSlotValues->AddValue( "CCECECECECECECECE" );
	pSlotValues->AddValue( "CCEEEEEEEEEEEEEEE" );
	pSlotValues->AddValue( "CEEEEEEEEEEEEEEEC" );
	testfactor::RFactor rfSlots = AddParameterValueSet( pSlotsParam, pSlotValues );

	testfactor::RFactor rfDynamic = AddParameter( _T( "DynamicBuffer" ), &m_bDynamic, BoolValueSet( ) );
	testfactor::RFactor rfSpecials = AddParameter( _T( "SpecialFloats" ), &m_bSpecials, BoolValueSet( ) );

	CTestCaseParameter<bool> *pUnsizedParam = AddParameter( _T( "UnsizedCB" ), &m_bUnsized );
	testfactor::RFactor rfUnsizedF = AddParameterValue<bool>( pUnsizedParam, false );
	testfactor::RFactor rfUnsizedT = AddParameterValue<bool>( pUnsizedParam, true );

	CTestCaseParameter<bool> *pMaxElemsParam = AddParameter( _T( "MaxElements" ), &m_bMaxElems );
	testfactor::RFactor rfMaxElemsF = AddParameterValue<bool>( pMaxElemsParam, false );
	testfactor::RFactor rfMaxElemsT = AddParameterValue<bool>( pMaxElemsParam, true );

	// shader cb decl size, buffer size, offset, number of contants, all in bytes
	// first constant must be in 16 constant increments (256 bytes)
	// num constants must be in 16 constant increments (256 bytes)
	static const UINT32 pBuffersAndWindows[] = 
	{
		0,             16,         0,         0, // window is zero in size
	    1,             16,         0,         0, // window is zero in size
		48,       48 * 16,   16 * 16,         0, // window in the middle of the buffer, but size zero

	    0,             16,         0,   16 * 16, // window is past the end of the buffer
	    1,             16,         0,   16 * 16, // window is past the end of the buffer
	    2,             16,         0,   16 * 16, // window is past the end of the buffer

		0,        16 * 16,         0,   16 * 16,
	   16,        16 * 16,         0,   16 * 16,
	    2,        16 * 16,         0,   16 * 16,

		0,        16 * 16,         0,   32 * 16, // window is past the end of the buffer
	    1,        16 * 16,         0,   32 * 16, // window is past the end of the buffer
	    2,        16 * 16,         0,   32 * 16, // window is past the end of the buffer

		0,        32 * 16,         0,   16 * 16, // window smaller than buffer
	   16,        32 * 16,         0,   16 * 16, // window smaller than buffer
	    8,        32 * 16,         0,   16 * 16, // window smaller than buffer

		0,        16 * 16,   16 * 16,   16 * 16, // window starts after the end of the buffer
	   16,        16 * 16,   16 * 16,   16 * 16, // window starts after the end of the buffer
	    8,        16 * 16,   16 * 16,   16 * 16, // window starts after the end of the buffer

		0,        16 * 16,   32 * 16,   16 * 16, // window starts way after the end of the buffer
	   16,        16 * 16,   32 * 16,   16 * 16, // window starts way after the end of the buffer
	    8,        16 * 16,   32 * 16,   16 * 16, // window starts way after the end of the buffer

		0,        48 * 16,   16 * 16,   16 * 16, // window in the middle of the buffer
	   16,        48 * 16,   16 * 16,   16 * 16, // window in the middle of the buffer
	   48,        48 * 16,   16 * 16,   16 * 16, // window in the middle of the buffer
	    8,        48 * 16,   16 * 16,   16 * 16, // window in the middle of the buffer

		0,        48 * 16,   16 * 16,   48 * 16, // window extends past the end of the buffer
	   16,        48 * 16,   16 * 16,   48 * 16, // window extends past the end of the buffer
	   48,        48 * 16,   16 * 16,   48 * 16, // window extends past the end of the buffer
	    8,        48 * 16,   16 * 16,   48 * 16, // window extends past the end of the buffer

	    0,      4096 * 16,         0,         0, // window is zero in size
	 4096,      4096 * 16,         0,         0, // window is zero in size

	    0,      4096 * 16,         0, 4096 * 16,
	 4096,      4096 * 16,         0, 4096 * 16,
	 4095,      4096 * 16,         0, 4096 * 16,

	    0,      4096 * 16, 2048 * 16, 4096 * 16, // window is past the end of the buffer
	 2048,      4096 * 16, 2048 * 16, 4096 * 16, // window is past the end of the buffer
	 4096,      4096 * 16, 2048 * 16, 4096 * 16, // window is past the end of the buffer

	    0,      4096 * 16, 2048 * 16, 1024 * 16,
	  512,      4096 * 16, 2048 * 16, 1024 * 16,
	 1024,      4096 * 16, 2048 * 16, 1024 * 16,
	 2048,      4096 * 16, 2048 * 16, 1024 * 16,

	    0,  4096 * 16 * 2, 4096 * 16, 4096 * 16,
	 2048,  4096 * 16 * 2, 4096 * 16, 4096 * 16,
	 4096,  4096 * 16 * 2, 4096 * 16, 4096 * 16,

	    0,  4096 * 16 * 2, 2048 * 16, 4096 * 16,
	 2048,  4096 * 16 * 2, 2048 * 16, 4096 * 16,
	 4096,  4096 * 16 * 2, 2048 * 16, 4096 * 16,

	    0,  4096 * 16 * 2, 3072 * 16, 2048 * 16,
	 1024,  4096 * 16 * 2, 3072 * 16, 2048 * 16,
	 2048,  4096 * 16 * 2, 3072 * 16, 2048 * 16,
	 4096,  4096 * 16 * 2, 3072 * 16, 2048 * 16,

		// -1 for first const or num const means that the arrays are null
        0,             16,        -1,        -1,
        1,             16,        -1,        -1,
        2,             16,        -1,        -1,

        1,             32,        -1,        -1,
        2,             32,        -1,        -1,

        0,      4096 * 16,        -1,        -1,
     2048,      4096 * 16,        -1,        -1,
     4096,      4096 * 16,        -1,        -1,

        0,  4096 * 16 * 2,        -1,        -1,
     4096,  4096 * 16 * 2,        -1,        -1
	};
	
	CTestCaseParameter<UINT32>* pCBSizeInConstants = AddParameter( _T("ShaderCBsizeInConstants"), &m_uConstantBufferSizeInConstants );
	testfactor::RFactor rfCBSizeInConstants = AddParameterValueSet<UINT32>( pCBSizeInConstants, new CTableValueSet<UINT32>( pBuffersAndWindows, 4 * sizeof(UINT32), sizeof(pBuffersAndWindows)/(sizeof(UINT32) * 4) ) );
	CTestCaseParameter<UINT32>* pBufferWidthInBytes = AddParameter( _T("BufferWidthInBytes"), &m_uBufferWidthInBytes );
	testfactor::RFactor rfBufferWidthInBytes = AddParameterValueSet<UINT32>( pBufferWidthInBytes, new CTableValueSet<UINT32>( pBuffersAndWindows + 1, 4 * sizeof(UINT32), sizeof(pBuffersAndWindows)/(sizeof(UINT32) * 4) ) );
	CTestCaseParameter<UINT32>* pBufferOffsets = AddParameter( _T("BufferOffsetInBytes"), &m_uBufferOffsetInBytes );
	testfactor::RFactor rfBufferOffsetsInBytes = AddParameterValueSet<UINT32>( pBufferOffsets, new CTableValueSet<UINT32>( pBuffersAndWindows + 2, 4 * sizeof(UINT32), sizeof(pBuffersAndWindows)/(sizeof(UINT32) * 4) ) );
	CTestCaseParameter<UINT32>* pNumConstants = AddParameter( _T("NumConstantsInBytes"), &m_uNumConstantsInBytes );
	testfactor::RFactor rfNumConstantsInBytes = AddParameterValueSet<UINT32>( pNumConstants, new CTableValueSet<UINT32>( pBuffersAndWindows + 3, 4 * sizeof(UINT32), sizeof(pBuffersAndWindows)/(sizeof(UINT32) * 4) ) );

	testfactor::RFactor pFactors[] = { rfCBSizeInConstants, rfBufferWidthInBytes, rfBufferOffsetsInBytes, rfNumConstantsInBytes };

	testfactor::RFactor rfBufferParams = NewSetFactor( pFactors, sizeof(pFactors)/sizeof(*pFactors) );

	testfactor::RFactor rfDefaults = 
		AddParameterValue( pCBSizeInConstants, 1u ) * 
		AddParameterValue( pBufferWidthInBytes, 16u ) * 
		AddParameterValue( pBufferOffsets, 0xFFFFFFFF ) * 
		AddParameterValue( pNumConstants, 0xFFFFFFFF ) *
		AddParameterValue( _T("DynamicBuffer"), false );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases_Test1 = rfSlots * rfDynamic * rfSpecials * rfBufferParams;
	testfactor::RFactor rfTestCases_Test2 = rfMaxElemsT * rfUnsizedF * rfDefaults;
	testfactor::RFactor rfTestCasesWithShaderStages_Test1 = GetTestFactorWithDefaultStageCoverage( rfTestCases_Test1 );
	testfactor::RFactor rfTestCasesWithShaderStages_Test2 = GetTestFactorWithDefaultStageCoverage( rfTestCases_Test2 );

	SetRootTestFactor( rfTestCasesWithShaderStages_Test1 + rfTestCasesWithShaderStages_Test2 );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("DynamicBuffer"), true), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("SpecialFloats"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("MaxElements"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterGreaterEqual<UINT32>(_T("NumConstantsInBytes"), 32 * 16), WeightFactorLarge);	// 32+ constants
	AddPriorityPatternFilter(FilterGreater<UINT32>(_T("BufferOffsetInBytes"), 0), WeightFactorLarge);				// Non Zero offset

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}	

#define NUM_VALIDATION_POSITIONS 7u

UINT32 CShader5xTest_ConstantBuffers::GetTestPosition( UINT32 testPoint, UINT32 cbIndex ) const
{
	int cbPos = 0;
	// figure out which cb constant position is being tested:
	switch( testPoint )
	{
		case 0:
			cbPos = 0; // test the beginning of the constant buffer
			break;
		case 1:
			if( 14 == cbIndex )
			{
				cbPos = m_uBufferWidthInBytes;
			}
			else
			{
				cbPos = -1 == m_uNumConstantsInBytes ? m_uBufferWidthInBytes : m_uNumConstantsInBytes; // test the last valid position within the num constants range
			}
			cbPos = max( 0, cbPos / 16 - 1 ); // convert to constants and subtract 1 to be within range
			break;
		case 2:
			if( 14 == cbIndex )
			{
				cbPos = m_uBufferWidthInBytes;
			}
			else
			{
				cbPos = -1 == m_uNumConstantsInBytes ? m_uBufferWidthInBytes : m_uNumConstantsInBytes; // test the first invalid position outside of the num constants range
			}
			cbPos = cbPos / 16; // convert to constants
			break;
		case 3:
			cbPos = max( 0, (int)m_uConstantBufferSizeInConstants - 1); // test the last valid position within the shader declaired cb size
			break;
		case 4: 
			cbPos = 0 == m_uConstantBufferSizeInConstants ? 4095 : m_uConstantBufferSizeInConstants; // test the first invalid position outside of the shader declaired cb size
			break;
		case 5:
			if( 14 == cbIndex )
			{
				cbPos = m_uBufferWidthInBytes;
			}
			else
			{
				cbPos = -1 == m_uBufferOffsetInBytes ? m_uBufferWidthInBytes : 
					m_uBufferOffsetInBytes < m_uBufferWidthInBytes ? m_uBufferWidthInBytes - m_uBufferOffsetInBytes : 0; // test the last valid location in the buffer width
			}
			cbPos = cbPos / 16; // convert to constants
			cbPos = cbPos > 0 ? cbPos - 1 : 0; // don't test a negative index location
			break;
		case 6:
			if( 14 == cbIndex )
			{
				cbPos = m_uBufferWidthInBytes;
			}
			else
			{
				cbPos = -1 == m_uBufferOffsetInBytes ? m_uBufferWidthInBytes : 
					m_uBufferOffsetInBytes < m_uBufferWidthInBytes ? m_uBufferWidthInBytes - m_uBufferOffsetInBytes : 0; // test the first invalid location outside of the buffer width
			}
			cbPos = cbPos / 16; // convert to constants
			break;
		default:
			WriteToLog( _T( "Missing implementation for positional validation: %d" ), testPoint );
			return RESULT_FAIL;
	}

	// Any time that the index would be invalid (outside of 4096 constants)
	// the value at the 4095th position is taken instead.
	return min( cbPos, 4095 );
}

TEST_RESULT CShader5xTest_ConstantBuffers::BindConstantBuffers()
{
	UINT32 pOffsets[14];
	memset( pOffsets, 0, sizeof(pOffsets) );
	UINT32 pNumConstants[14];
	memset( pNumConstants, 0, sizeof(pNumConstants) );

	for( UINT32 i = 0; i < NUM_CONSTANT_BUFFER_SLOTS - 1; ++i )
	{
		if( 'C' == m_szSlotContents[i] )
		{
			if( 0 == i )
			{
				pOffsets[i] = 0;
				pNumConstants[i] = 4096;
			}
			else
			{
				pOffsets[i] = m_uBufferOffsetInBytes / 16;
				pNumConstants[i] = m_uNumConstantsInBytes / 16;
			}
		}
	}

	switch( m_ShaderStage )
	{
		case D3D_SHADER_STAGE_VERTEX:
			GetEffectiveContext1()->VSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		case D3D_SHADER_STAGE_GEOMETRY:
			GetEffectiveContext1()->GSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		case D3D_SHADER_STAGE_PIXEL:
			GetEffectiveContext1()->PSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		case D3D_SHADER_STAGE_HULL:
			GetEffectiveContext1()->HSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		case D3D_SHADER_STAGE_DOMAIN:
			GetEffectiveContext1()->DSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		case D3D_SHADER_STAGE_COMPUTE:
			GetEffectiveContext1()->CSSetConstantBuffers1( 0, NUM_CONSTANT_BUFFER_SLOTS - 1, m_pBuffers, 
				-1 == m_uBufferOffsetInBytes ? nullptr : pOffsets, 
				-1 == m_uNumConstantsInBytes ? nullptr : pNumConstants );
			break;
		default:
			WriteToLog( _T("Unexpected shader stage: %s"), ToString(m_ShaderStage).c_str() );
			return RESULT_FAIL;
	}

	return RESULT_PASS;
}


TEST_RESULT CShader5xTest_ConstantBuffers::SetupTestCase()
{
	// Init variables needed in the test (before any skips occur)
	memset( m_pBuffers, 0, sizeof(m_pBuffers) );	// Win8: 453443: WGF11Shader5x: ConstantBuffers: Test crashes in cleanup after trying to skip a test case

	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tr = CShader5xTest::SetupTestCase();

    const UINT32 NUM_VALIDATION_POINTS = NUM_CONSTANT_BUFFER_SLOTS * NUM_VALIDATION_POSITIONS;
	
	if( RESULT_PASS != tr )
		return tr;

	HRESULT hr;
	D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11Options;
	ZeroMemory( &d3d11Options, sizeof(d3d11Options) );
	if( FAILED( hr = GetDevice1()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &d3d11Options, sizeof(d3d11Options) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport );
		return RESULT_FAIL;
	}

	const bool bTestCaseUsingCBOffsetting = (m_uBufferOffsetInBytes != -1) || (m_uNumConstantsInBytes != -1);
	if( !d3d11Options.ConstantBufferOffsetting &&	// The driver doesn't support constant buffer offsetting
		bTestCaseUsingCBOffsetting )				// The current test case uses constant buffer offsetting
	{
		WriteToLog( _T("This test case is using constant buffer offsetting; however, the driver doesn't support it. Skipping...") );
		return RESULT_SKIP;
	}

	// Win8: 452571 Test must skip test cases that use constant buffers bigger than 65536 (4096 contstants @ 16 bytes each) bytes on older drivers
	const bool bTestCaseUsingBufferLargerThan65536Bytes = m_uBufferWidthInBytes > 65536;
	const bool bDriverSupportsBuffersLargerThan65536Bytes = GetFramework()->Is11_1DDISupported( GetDevice() ) ? true : false;	// 11.1 drivers require support for large CBs
	if( bTestCaseUsingBufferLargerThan65536Bytes &&
		!bDriverSupportsBuffersLargerThan65536Bytes )
	{
		WriteToLog( _T("This test case is using constant buffer sizes larger than 65536 bytes, but the driver doesn't support the 11.1 DDI. Skipping...") );
		return RESULT_SKIP;
	}

	//setup view port
	m_viewPort = CD3D11_VIEWPORT( 0.f, 0.f, (FLOAT)NUM_VALIDATION_POINTS, 1.0f );

	D3DXMatrixIdentity( (D3DXMATRIX *) m_matWorld );
	float transx = m_viewPort.Width / 2.f;
	float transy = m_viewPort.Height / 2.f;

	// Inverse viewport scale.
	m_matWorld[0][2] = 1 / transx;
	m_matWorld[0][3] = -1 / transy;

	// Prescaled inverse viewport translation.
	m_matWorld[0][0] = -transx * m_matWorld[0][2];
	m_matWorld[0][1] = -transy * m_matWorld[0][3];

	GetEffectiveContext()->RSSetViewports(1, &m_viewPort);

	for( UINT32 i = 0; i < NUM_CONSTANT_BUFFER_SLOTS - 1; ++i )
	{
		m_pBuffers[ i ] = NULL;

		if( m_szSlotContents[ i ] == 'C' || m_bMaxElems )
		{
			// The first constant buffer is always max size, while the rest are always just a single constant.
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.ByteWidth = i == 0 ? sizeof( float ) * 4 * 4096 : m_uBufferWidthInBytes;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

			if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pBuffers[ i ] ) ) )
			{
				WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - Failed to create constant buffer #%d, hr = %s" ), i, D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}
	
	if( m_bDynamic )
	{
		tr = BindConstantBuffers();
		if( RESULT_PASS != tr )
		{
			return tr;
		}
	}

	for( UINT32 i = 0; i < NUM_CONSTANT_BUFFER_SLOTS - 1; ++i )
	{
		if( m_szSlotContents[ i ] == 'C' || m_bMaxElems )
		{
			// encode components with [slotnum] * 4 + [componentnum]
			UINT32 slotCode = i * 4;
			UINT32 bufferData [ ] = { slotCode, slotCode + 1, slotCode + 2, slotCode + 3 };
			
			D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;
			if( FAILED( hr = GetEffectiveContext()->Map( m_pBuffers[ i ], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstantBuffer ) ) )
			{
				WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - Failed to map buffer #%d, hr = %s" ), i, D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
			void *pData = (mappedConstantBuffer.pData);

			if( i == 0 )
			{
				if( m_bSpecials )
					memcpy( pData, m_pSpecialFloats, sizeof( float ) * 4 );
				else
					memcpy( pData, bufferData, sizeof( UINT32 ) * 4 );
				// Set the viewport transform 
				memcpy( ((BYTE*) pData) + sizeof( float ) * 4, m_matWorld, sizeof( m_matWorld ) );
	
				// if we're just testing element limits, fill up c0 to 4095 and break
				if( m_bMaxElems )
				{
					float pFiller[ MAX_CB_ELEMS * 4 ];
                    std::fill_n( pFiller, MAX_CB_ELEMS * 4, LIMIT_TEST_VALUE );
					memcpy( ((BYTE*) pData) + sizeof(float) * 4 + sizeof(D3DXMATRIX), pFiller, MAX_CB_ELEMS * sizeof(float) * 4 );

					GetEffectiveContext()->Unmap(m_pBuffers[ i ], 0);
 					continue;
				}
				else
				{
					// Zero out the remaining values
					memset( ((BYTE*) pData) + sizeof(float) * 4 + sizeof(D3DXMATRIX), 0, MAX_CB_ELEMS * sizeof(float) * 4 );
				}
			}
			else
			{
				UINT32* pDataStart = (UINT32*)pData;
				UINT32* pDataFence = -1 == m_uBufferOffsetInBytes ? pDataStart : (UINT32*)pData + m_uBufferOffsetInBytes/sizeof(UINT32);
				UINT32* pDataEnd = (UINT32*)pData + m_uBufferWidthInBytes/sizeof(UINT32);

				// Set the beginning region outside of the window to invalid stuff.
				std::fill( pDataStart, min( pDataFence, pDataEnd ), 0xBAAD00FF );

				if( pDataFence < pDataEnd )
				{
					pDataStart = pDataFence;
					pDataFence = -1 == m_uNumConstantsInBytes ? pDataEnd : pDataStart + m_uNumConstantsInBytes/sizeof(UINT32);
					pDataFence = pDataFence > pDataEnd ? pDataEnd : pDataFence;

					if( pDataStart != pDataFence )
					{
						assert( pDataStart < pDataFence );
						if( m_bSpecials )
							memcpy( pDataStart, m_pSpecialFloats, sizeof( float ) * 4 );
						else
							memcpy( pDataStart, bufferData, sizeof( bufferData ) );

						pDataStart += 4;
					}

					// fill the rest of the window with valid stuff
					int val = 3 * i;
					std::generate( pDataStart, pDataFence, [&val]() -> UINT32 
						{
							return val++;
						} );
				}

				if( pDataFence < pDataEnd )
				{
					// fill any remainder with more invalid stuff
					pDataStart = pDataFence;

					std::fill( pDataStart, pDataEnd, 0x00FFBAAD );
				}
			}
		
			GetEffectiveContext()->Unmap(m_pBuffers[ i ], 0);
		}
	}

	if( !m_bDynamic )
	{
		tr = BindConstantBuffers();
		if( RESULT_PASS != tr )
		{
			return tr;
		}
	}

	ID3D11DebugTest *pDebugTest = NULL;
	hr = GetEffectiveContext()->QueryInterface( __uuidof(ID3D11DebugTest), (void**) &pDebugTest );
	if( SUCCEEDED(hr) && pDebugTest && m_szSlotContents[ 14 ] == 'C' )
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = m_uBufferWidthInBytes;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.StructureByteStride = 0;

		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pBuffer14 ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - Failed to create constant buffer 14, hr = %s" ), D3DHResultToString(hr).c_str() );
			SAFE_RELEASE(pDebugTest);
			return RESULT_FAIL;
		}

		if( m_bDynamic )
		{
			pDebugTest->VSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->GSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->PSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->HSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->DSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->CSSetConstantBuffer14( m_pBuffer14 );
		}

		UINT32 slotCode = 14 * 4;
		UINT32 bufferData [ ] = { slotCode, slotCode + 1, slotCode + 2, slotCode + 3 };
		float m_pSpecialFloats [ ] = { m_fInfP, m_fInfN, m_fNaN, m_fDenorm };

		D3D11_MAPPED_SUBRESOURCE mappedConstantBuffer;
		if( FAILED( GetEffectiveContext()->Map( m_pBuffer14, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstantBuffer ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - Failed to map buffer 14, hr = %s" ), D3DHResultToString(hr).c_str() );
			SAFE_RELEASE(pDebugTest);
			return RESULT_FAIL;
		}
		void *pData = (mappedConstantBuffer.pData);

		if( m_bSpecials )
			memcpy( pData, m_pSpecialFloats, sizeof( float ) * 4 );
		else
			memcpy( pData, bufferData, sizeof( bufferData ) );

		UINT32 val = 3 * 14;
		std::generate_n( (UINT32*)pData + 4, m_uBufferWidthInBytes / 4 - 4, [&val]() -> UINT32 
		{
			return val++;
		} );

		GetEffectiveContext()->Unmap( m_pBuffer14, 0 );

		if( !m_bDynamic )
		{
			pDebugTest->VSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->GSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->PSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->HSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->DSSetConstantBuffer14( m_pBuffer14 );
			pDebugTest->CSSetConstantBuffer14( m_pBuffer14 );
		}
	}
	else
	{
		m_pBuffer14 = NULL;
	}
	SAFE_RELEASE(pDebugTest);
	
	m_RTFormat = DXGI_FORMAT_R32G32B32A32_UINT;
	const UINT bufferSize = GetBitsPerElement( m_RTFormat ) / 8 * NUM_VALIDATION_POINTS;
	CD3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC( bufferSize,
		D3D11_BIND_RENDER_TARGET | (g_TestApp.UAVIsAllowed() ? D3D11_BIND_UNORDERED_ACCESS : 0) );
	bufferDesc.MiscFlags |= (g_TestApp.UAVIsAllowed()) ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;

	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBuffer ) ) )
	{
		WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - failed to create render target resource, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	bufferDesc = CD3D11_BUFFER_DESC( bufferSize, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ );
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTCopy ) ) )
	{
		WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - failed to create render target copy, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

    CD3D11_RENDER_TARGET_VIEW_DESC rtDesc =
		CD3D11_RENDER_TARGET_VIEW_DESC( m_pRTBuffer, m_RTFormat, 0, NUM_VALIDATION_POINTS );
	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer, &rtDesc, &m_pRTV ) ) )
	{
		WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - failed to create render target view, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	if( g_TestApp.UAVIsAllowed() )
	{
		// Since the UAV is R32 Typeless, we need to account for the different number of elements
		const DXGI_FORMAT uavFormat = DXGI_FORMAT_R32_TYPELESS;
		const UINT uavFormatElementSizeInBytes = GetBitsPerElement( uavFormat ) / 8;
		const UINT rtFormatElementSizeInBytes = GetBitsPerElement( rtDesc.Format ) / 8;
		const UINT elementDifference = rtFormatElementSizeInBytes / uavFormatElementSizeInBytes; // This accounts for the fact the 4x R32_Typeless = 1 R32B32G32A32_UINT etc...	

		CD3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( m_pRTBuffer,
			uavFormat, 0, rtDesc.Buffer.NumElements * elementDifference, D3D11_BUFFER_UAV_FLAG_RAW );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer, &rtuaviewdesc, &m_pRTUAV ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ConstantBuffers::SetupTestCase() - failed to create unordered access view, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	// create geometry
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth      = NUM_VALIDATION_POINTS * sizeof( CB_VERTEX );
	buffer_desc.Usage          = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags      = 0;

	if( FAILED( hr = GetDevice()->CreateBuffer( &buffer_desc, NULL, &m_pVB) ) )
	{
		WriteToLog( _T("CShader5xTest_ConstantBuffers - CreateBuffer() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	UINT StridesTL = sizeof( CB_VERTEX );
	UINT Offset = 0;

	CB_VERTEX *vTL = NULL;

	vTL = new CB_VERTEX[ NUM_VALIDATION_POINTS ];

    // Description of a constant buffer
    //  <D..> is the data window in the constant buffer
    //  -- is other space in the buffer
    //
    // CB:  |--------------<DDDDDDDDDDDDDDDDDDDDD>------------|
    //
    // We want to check the first and last valid data cosntants
    // We want to check the constant before and after the buffer window for zero
    //
    // 4 possible poisitions to check on each buffer
    //
    // a table of veritices will be used where each one looks up a specific value in a constant buffer.
    // The exact position in the cb that is looked at will vary depending on the shape of the constant buffer.
    // There are 14 (NUM_CONSTANT_BUFFER_SLOTS) constant buffer slots and possibly 4 positions that we will
    // analyze.  Each row will pertain to a specific check position.
    //
    // To get the positions to work in all stages we have to make sure that they fit into the [-1,1] range of the
    // view frustom so that they aren't clipped on the way to the pixel shader.  Therefore each stage will
    // have to transform the positions back to the [0,NUM_CONSTANT_BUFFER_SLOTS] range in order to get the
    // correct cb slot to access.
    //
    // Same goes with the y position, which is used to determine which "validation" check is being performed.

    for( UINT inp = 0; inp < NUM_VALIDATION_POSITIONS; ++inp )
    {
		for( UINT icb = 0; icb < NUM_CONSTANT_BUFFER_SLOTS; ++icb )
        {
            const UINT i = inp * NUM_CONSTANT_BUFFER_SLOTS + icb;
            // duplicate pre-scaled position
            vTL[i].position[0] = i + 0.5f;
            vTL[i].position[1] = 0.5f;
			vTL[i].positionTL[0] = icb;

			int cbPos = GetTestPosition( inp, icb );
			vTL[i].positionTL[1] = cbPos;

            // transform position
            // mad r2.xy, v[0].xy, c0[1].zwww, c0[1].xyyy
            // mov r2.zw, vec4( 0.0, 0.0, 0.0, 1.0 )
            vTL[i].position[0] = vTL[i].position[0] * m_matWorld[0][2] + m_matWorld[0][0];
            vTL[i].position[1] = vTL[i].position[1] * m_matWorld[0][3] + m_matWorld[0][1];
            vTL[i].position[2] = 0;
            vTL[i].position[3] = 1;
        }
    }
	

	GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	D3D11_MAPPED_SUBRESOURCE  mappedVertexBuffer;

	if( FAILED(hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexBuffer)) ) 
	{
		WriteToLog( _T("CShader5xTest_ConstantBuffers::CreateGeometry - Map() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
        delete [] vTL;
		return RESULT_FAIL;
	}

	memcpy(mappedVertexBuffer.pData, vTL, NUM_VALIDATION_POINTS * sizeof(CB_VERTEX));
	GetEffectiveContext()->Unmap( m_pVB, 0 );

	delete [ ] vTL;

	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVB, &StridesTL, &Offset);

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	if( ( tr = BuildShaders() ) != RESULT_PASS )
		return tr;
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return tr;
}

void CShader5xTest_ConstantBuffers::CleanupTestCase()
{
	for( UINT i = 0; i < NUM_CONSTANT_BUFFER_SLOTS - 1; ++i )
	{
		ID3D11Buffer *pNullBuff = NULL;
		if( GetEffectiveContext() )
		{
			GetEffectiveContext()->VSSetConstantBuffers( i, 1, &pNullBuff );
			GetEffectiveContext()->GSSetConstantBuffers( i, 1, &pNullBuff );
			GetEffectiveContext()->PSSetConstantBuffers( i, 1, &pNullBuff );
			GetEffectiveContext()->HSSetConstantBuffers( i, 1, &pNullBuff );
			GetEffectiveContext()->DSSetConstantBuffers( i, 1, &pNullBuff );
			GetEffectiveContext()->CSSetConstantBuffers( i, 1, &pNullBuff );
		}
		SAFE_RELEASE( m_pBuffers[ i ] );
	}

	if( GetEffectiveContext() )
	{
		ID3D11RenderTargetView *pNullRTV = NULL;
		ID3D11VertexShader *pVSNull = NULL;
		ID3D11GeometryShader *pGSNull = NULL;
		ID3D11PixelShader *pPSNull = NULL;
		ID3D11InputLayout *pLayoutNull = NULL;
		ID3D11Buffer *pVBNull = NULL;
		UINT uStrides = 0;

		GetEffectiveContext()->OMSetRenderTargets( 1, &pNullRTV, NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &pVBNull, &uStrides, &uStrides );
		GetEffectiveContext()->VSSetShader( pVSNull, NULL, 0 );
		GetEffectiveContext()->GSSetShader( pGSNull, NULL, 0 );
		GetEffectiveContext()->PSSetShader( pPSNull, NULL, 0 );
		GetEffectiveContext()->IASetInputLayout( pLayoutNull );

		ID3D11DebugTest *pDebugTest = NULL;
		GetEffectiveContext()->QueryInterface( __uuidof(ID3D11DebugTest), (void**) &pDebugTest );
		if (pDebugTest)
		{
			ID3D11Buffer *pNullBuff = NULL;
			pDebugTest->VSSetConstantBuffer14( pNullBuff );
			pDebugTest->GSSetConstantBuffer14( pNullBuff );
			pDebugTest->PSSetConstantBuffer14( pNullBuff );
			pDebugTest->HSSetConstantBuffer14( pNullBuff );
			pDebugTest->DSSetConstantBuffer14( pNullBuff );
			pDebugTest->CSSetConstantBuffer14( pNullBuff );
			SAFE_RELEASE( pDebugTest );
		}
		SAFE_RELEASE( m_pBuffer14 );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( m_pRTUAV );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pVSPass );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSPass );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pRTBuffer );
	SAFE_RELEASE( m_pRTCopy );
}

TEST_RESULT CShader5xTest_ConstantBuffers::ExecuteTestCase()
{
	HRESULT hr;
	bool bPass = true;
	bool bLogVerbose = GetFramework()->LogVerbose();
	FLOAT colors [ ] = { 0.f, 0.f, 0.f, 0.f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTV, colors );
	if (m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
		GetEffectiveContext()->Dispatch( NUM_CONSTANT_BUFFER_SLOTS * NUM_VALIDATION_POSITIONS, 1, 1 );
	else
		GetEffectiveContext()->Draw( NUM_CONSTANT_BUFFER_SLOTS * NUM_VALIDATION_POSITIONS, 0 );

	GetEffectiveContext()->CopyResource( m_pRTCopy, m_pRTBuffer );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE mappedRT;
	if( FAILED( hr = GetImmediateContext()->Map( m_pRTCopy, 0, D3D11_MAP_READ, 0, &mappedRT) ) )
	{
		WriteToLog( _T( "CShader5xTest_ConstantBuffers::ExecuteTestCase() - failed to map render target copy, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	float const* pResultF = (float*)mappedRT.pData;
	UINT32 const* pResultU = (UINT32*)mappedRT.pData;

	TEST_RESULT tr = RESULT_PASS;
	for( UINT32 i = 0; i < NUM_VALIDATION_POSITIONS; ++i )
	{
		bool tripped = false;
		for( UINT32 j = 0; j < NUM_CONSTANT_BUFFER_SLOTS; ++j )
		{
			UINT32 pixelIndex = i * NUM_CONSTANT_BUFFER_SLOTS + j;
			UINT32 resultOffset = pixelIndex * 4; // each constant is 4 uints or 4 floats

			UINT32 const* pActual = pResultU + resultOffset;

			// cbPos is in shader constant increments. (16 bytes)
			UINT32 const cbPos = GetTestPosition(i,j);
			UINT32 pExpected[4] = { 0u, 0u, 0u, 0u };

			// determine what should be at this position;
			if( 'C' == m_szSlotContents[j] )
			{
				UINT32 const pE[4] = { j * 4, j * 4 + 1, j * 4 + 2, j * 4 + 3 };
				if( 0 == cbPos )  // zero location always has interesting data.
				{
					if( (( -1 == m_uBufferOffsetInBytes ||
							m_uBufferOffsetInBytes < m_uBufferWidthInBytes) && 
							0 != m_uNumConstantsInBytes) ||
						0 == j ||
						14 == j )
					{
						if( m_bSpecials )
						{
							memcpy( pExpected, m_pSpecialFloats, sizeof(pExpected) );
						}
						else
						{
							memcpy( pExpected, pE, sizeof(pExpected) );
						}
					}
				}
				else
				{
					if( 0 == j )
					{
						if( cbPos > 0 && cbPos < 5 )
						{
							memcpy( pExpected, &m_matWorld[cbPos - 1], sizeof(pExpected) );
						}

						else if( m_bMaxElems )
						{
							float const pE[4] = { LIMIT_TEST_VALUE, LIMIT_TEST_VALUE, LIMIT_TEST_VALUE, LIMIT_TEST_VALUE };
							memcpy( pExpected, pE, sizeof(pExpected) );
						}
						// else zero
					}
					else
					{
						UINT32 shaderViewableWidthInBytes = m_uBufferWidthInBytes;

						if( -1 != m_uBufferOffsetInBytes && 14 != j)
						{
							if( m_uBufferOffsetInBytes > shaderViewableWidthInBytes )
							{
								shaderViewableWidthInBytes = 0;
							}
							else
							{
								shaderViewableWidthInBytes -= m_uBufferOffsetInBytes;
							}
						}

						if( -1 != m_uNumConstantsInBytes && 14 != j )
						{
							shaderViewableWidthInBytes = min( shaderViewableWidthInBytes, m_uNumConstantsInBytes );
						}

						UINT32 shaderViewableConstants = shaderViewableWidthInBytes / 16;

						// currently, we expect that hardware and ref are ignoring the decl parameter
						// and so that data will be available no matter what size was specified.
						//if( 0 != m_uConstantBufferSizeInConstants && 14 != j )
						//{
						//	shaderViewableConstants = min( shaderViewableConstants, m_uConstantBufferSizeInConstants );
						//}

						if( cbPos < shaderViewableConstants )
						{
							int val = 3 * j;
							val += (cbPos - 1) * 4;
							UINT32 const pE[4] = { val, val + 1, val + 2, val + 3 };
							memcpy( pExpected, pE, sizeof(pExpected) );
						}
						// else zero
					}
				}

				// compare the results;
				bool failed = false;
				for( UINT32 k = 0; k < 4; ++k )
				{
					if( pExpected[k] != pActual[k] )
					{
						failed = true;
						break;
					}
				}
				
				if( failed )
				{
					tr = RESULT_FAIL;

					if( !tripped )
					{
						tripped = true;
						static TCHAR const* pValidationDetail[] = 
						{
							_T("The following failures cover the first constant in a buffer."),
							_T("The following cases look at the last constant in the valid range of numConstants param."),
							_T("The following cases look at the first constant outside of the valid range of numConstant param."),
							_T("The following cases look at the last valid position within the shader declaired cb size."),
							_T("The following cases look at the first constant outside of the shader declaired cb size."),
							_T("The following cases look at the last valid position within the buffer width."),
							_T("The following cases look at the first constant outside of the buffer width.")
						};			
						WriteToLog( pValidationDetail[i] );
					}

					if( 0 == cbPos && m_bSpecials )
					{
						float const* pA = (float*)pActual;
						float const* pE = (float*)pExpected;
						WriteToLog( _T("Failed data compare: pixel %d should match cb%d[%d].  Actual: %e, %e, %e, %e  Expected: %e, %e, %e, %e"),
							pixelIndex, j, cbPos,
							pA[0],  pA[1], pA[2], pA[3],
							pE[0],  pE[1], pE[2], pE[3] );
					}
					else
					{
						WriteToLog( _T("Failed data compare: pixel %d should match cb%d[%d].  Actual: %d, %d, %d, %d  Expected: %d, %d, %d, %d"),
							pixelIndex, j, cbPos,
							pActual[0],  pActual[1], pActual[2], pActual[3],
							pExpected[0],  pExpected[1], pExpected[2], pExpected[3] );
					}
				}
			}
		}
	}

	GetImmediateContext()->Unmap( m_pRTCopy, 0 );

	return tr;
}

TEST_RESULT CShader5xTest_ConstantBuffers::BuildShaders()
{
	ID3D10Blob *pShaderBuffer = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// input layout decls
	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		// it's really float data, but asm doesn't have a way to specify float inputs to VS, so we say UINT instead
		{"0_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},  //position (range [-1, 1])
		{"1_", 0, DXGI_FORMAT_R32G32_UINT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},  //screen-scaled position (ie {0.5,0.5}, {1.5,0.5}, etc)
	};

	// r2.xy = position.xy

	// set up the "loop" with invariants and the first iteration
	const char szLoopStart_NeedsPositionMovToTemp2AndOutputMovToOut0[] =
		"%s"								    ; // insert position mov string

	DXGI_FORMAT targetFormat = m_RTFormat;
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		targetFormat = DXGI_FORMAT_R32_TYPELESS;
	
	AssemblyHelper asmHelper(
		m_bUnsized ? 0 : 4096, 
		D3D_SHADER_STAGE_COMPUTE == m_ShaderStage ? 100 : 4,
		targetFormat, 
		m_viewPort);

	if (m_bUnsized)
	{
		asmHelper.SetConstantBufferSize( 0, 0 );
	}

	// position to drive PS
	asmHelper.SetPositionInputRegIdx( 0 );
	asmHelper.SetPositionOutputRegIdx( 1 );
	asmHelper.SetupMovToOutputFromInput( 1, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );

	// screen-scaled position is what we use to index into CBs
	asmHelper.SetUserInputRegMask( 1, AssemblyHelper::xy );

	// output in o[0]
	asmHelper.SetOutputRegMaskRaw( 0, AssemblyHelper::xyzw );

	for (int n = 1; n < NUM_CONSTANT_BUFFER_SLOTS; ++n)
	{
		// asmHelper deals with buffer sizes in 16 byte constant increments
		if( m_szSlotContents[ n ] == 'C' )
		{
			asmHelper.SetConstantBufferSize( n, m_uConstantBufferSizeInConstants );
		}
		else
		{
			asmHelper.SetConstantBufferSize( n, m_bUnsized ? 0 : 1 );
		}
	}

	tstringstream instructionCode;
	tstring outputMovStr;

	// We're doing things differently, so ignore the compute
	// shader stuff, which is otherwise just noise
	if( D3D_SHADER_STAGE_COMPUTE == m_ShaderStage )
	{
		asmHelper.SetIndexableTempSize( 0, NUM_VALIDATION_POSITIONS );
		instructionCode << "udiv r2.y, r2.x, vThreadID.x, l(15)"   "\n"; // get the global thread index and break it into rows/cols 15x7
		for( UINT32 i = 0; i < NUM_VALIDATION_POSITIONS; ++i )
		{
			// put the sampling positions into an array
			instructionCode << FormatString( "mov x0[%d].x, l(%d)" "\n", i, GetTestPosition( i, 0 ) );
		} 
		instructionCode << "ishl r2.z, vThreadID.x, l(4)"          "\n"; // use the global thread index for the output location into the raw buffer. (bytes)
		outputMovStr =     "store_raw u0.xyzw, r2.z";
	}
	else
	{
		instructionCode << FormatString( szLoopStart_NeedsPositionMovToTemp2AndOutputMovToOut0, 
			asmHelper.GetScreenPositionMovStr(m_ShaderStage, 2, 1).c_str() ); 
		outputMovStr = asmHelper.GetRawOutputMovStr(m_ShaderStage, 0, AssemblyHelper::xyzw);
	}

	instructionCode <<               "switch r2.x"           "\n";
	for( int n = 0; n < NUM_CONSTANT_BUFFER_SLOTS; ++n )
	{
        // I moved this code here so that it would be easier to read the algorithm
        // "loop" iterations all look pretty much the same, just reading from different constant buffers
		instructionCode << FormatString( "  case %d"             "\n", n ); // choose which constant buffer to access

		if( D3D_SHADER_STAGE_COMPUTE == m_ShaderStage )
		{
			if( 14 == n ) // can't bind this one with offsets so we have to treat it differently.
			{
				for( UINT32 i = 0; i < NUM_VALIDATION_POSITIONS; ++i )
				{
					// put the sampling positions into an array
					instructionCode << FormatString( "mov x0[%d].x, l(%d)" "\n", i, GetTestPosition( i, 14 ) );
				} 
			}

			instructionCode << "    mov r2.y, x0[r2.y + 0].x"          "\n"; // look up the validation position from the array
		}
		const char szLoopIteration_NeedsOutputMovAndCBIndex[] = 
            "    %s, c%d[r2.y + 0]" "\n" // insert output mov string and CB index here
            //"    %s, l(%d,%d,%d,%d)" "\n" // debug pixel/position pathway
			//"    %s, r2.yyyy"       "\n" // debug test position selection
			//"    %s, r2.xxxx"       "\n" // debug test position selection
			//"    %s, r2.zzzz"       "\n" // debug test uav raw output offset
            "    break"             "\n";// break from this case
		instructionCode << FormatString( szLoopIteration_NeedsOutputMovAndCBIndex, outputMovStr.c_str(), n,n,n,n );
	}
	instructionCode <<               "  default"                                               "\n";
	instructionCode << FormatString( "    %s, l(0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)"  "\n", outputMovStr.c_str() );
	instructionCode <<               "    break"                                               "\n";
	instructionCode <<               "endswitch"                                               "\n";

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions, 
		instructionCode.str(), &m_pRTV, &m_pRTUAV, 1, &pShaderBuffer,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS,
		DeclTL, ARRAYSIZE(DeclTL), &m_pLayout );

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	return tRes;
}

TEST_RESULT CShader5xTest_ConstantBuffers::Setup( )
{
	m_bWasDebug = GetFramework()->m_D3DOptions.Debug;
	GetFramework()->m_D3DOptions.Debug = true;

    g_TestApp.CleanupD3D();
    g_TestApp.SetupD3D();

    return CShader5xTest::Setup();
}

void CShader5xTest_ConstantBuffers::Cleanup()
{
	CShader5xTest::Cleanup();
	
	GetFramework()->m_D3DOptions.Debug = m_bWasDebug;
}

bool CShader5xTest_ConstantBuffers::WithinOneULP( float fReference, float fOutput )
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

bool CShader5xTest_ConstantBuffers::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CShader5xTest_ConstantBuffers::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}
