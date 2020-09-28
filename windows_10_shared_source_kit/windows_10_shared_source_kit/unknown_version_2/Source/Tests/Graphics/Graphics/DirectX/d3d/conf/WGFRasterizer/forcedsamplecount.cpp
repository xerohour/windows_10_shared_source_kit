//////////////////////////////////////////////////////////////////////////
//  forcedsamplecount.cpp
//  created:	2009/04/21
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

// Includes
#include "forcedsamplecount.h"
#include <d3dx11.h>

// Create the strings for parameter names
const tstring g_outputTypeStr = "OutputType";
const tstring g_sampleCountStr = "SampleCount";
const tstring g_forcedSampleCountStr = "ForcedSampleCount";
const tstring g_coveredSamplesStr = "CoveredSamples";
const tstring g_coveredSamplesRepresentationStr = "CoveredSamplesRepresentation";
const tstring g_sampleMaskStr = "SampleMask";
const tstring g_qualityLevelsStr = "QualityLevel";

const UINT g_noCoveredSamples = 0x00000000;
const UINT g_coverPixelCenter = 0x10000000;	// Special value

// Named value sets
BEGIN_NAMED_VALUES( COVERED_SAMPLES_REPRESENTATION )
	NAMED_VALUE( "Forced Sample Count Pattern", COVERED_SAMPLES_REPRESENTATION_FORCED_SAMPLE_COUNT_PATTERN )
	NAMED_VALUE( "Render Target Multisample Pattern", COVERED_SAMPLES_REPRESENTATION_RENDER_TARGET_SAMPLE_PATTERN )
END_NAMED_VALUES( COVERED_SAMPLES_REPRESENTATION )

BEGIN_NAMED_VALUES( OUTPUT_TYPE )
	NAMED_VALUE( "RTV", OUTPUT_TYPE_RTV )
	NAMED_VALUE( "UAV", OUTPUT_TYPE_UAV )
END_NAMED_VALUES( OUTPUT_TYPE )

// Globals
const UINT g_constBufferSize = 512;
const float g_stdMS1Offsets[1 * 2] =
{ .5f, .5f, };

const float g_stdMS4Offsets[4 * 2] =
{
	.375f, .125f,
	.875f, .375f,
	.125f, .625f,
	.625f, .875f,
};

const float g_stdMS8Offsets[8 * 2] =
{
	.5625f, .3125f,
	.4375f, .6875f,
	.8125f, .5625f,
	.3125f, .1875f,
	.1875f, .8125f,
	.0625f, .4375f,
	.6875f, .9375f,
	.9375f, .0625f,
};

const float g_stdMS16Offsets[16 * 2] =
{
	.5625f, .5625f,
	.4375f, .3125f,
	.3125f, .625f,
	.75f, .4375f,
	.1875f, .375f,
	.625f, .8125f,
	.8125f, .6875f,
	.6875f, .1875f,
	.375f, .875f,
	.5f, .0625f,
	.25f, .125f,
	.125f, .75f,
	.0f, .5f,
	.9375f, .25f,
	.875f, .9375f,
	.0625f, .0f,
};

////////////////////////
// CForcedSampleCountTest

CForcedSampleCountTest::CForcedSampleCountTest()
	: CRasterizerTest()
    , m_OutputType( OUTPUT_TYPE_RTV )
	, m_uCoveredSamples( 0 )
	, m_uSampleMask( 0x0 )
	, m_coveredSamplesRepresentation( COVERED_SAMPLES_REPRESENTATION_FORCED_SAMPLE_COUNT_PATTERN )
	, m_qualityLevel( D3D11_STANDARD_MULTISAMPLE_PATTERN )
	, m_uVertexCount( 0 )
	, m_pDepthStateDisabled( NULL )
	, m_pPipelineQuery( NULL )
	, m_pRTTexture2DStaging( NULL )
	, m_pPSMSToSS( NULL )
	, m_pRTTexture2DSS( NULL )
	, m_pRTTexture2DSSStaging( NULL )
	, m_pSRView( NULL )
	, m_pSSRTView( NULL )
    , m_pUAV( NULL )
    , m_pUAVPS( NULL )

{
	m_RTFormat = DXGI_FORMAT_R32_UINT;
	m_uRTWidth = 2;
	m_uRTHeight = 2;

	// Setup the viewport
	m_Viewport.Height = (float) m_uRTHeight;
	m_Viewport.Width = (float) m_uRTWidth;
	m_Viewport.MinDepth = 0;
	m_Viewport.MaxDepth = 1;
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
}

////

void CForcedSampleCountTest::InitTestParameters()
{
    // Setup output type
    testfactor::RFactor rfOutputTypeRT = AddParameter<OUTPUT_TYPE>( _T(g_outputTypeStr), &m_OutputType, OUTPUT_TYPE_RTV );
	testfactor::RFactor rfOutputTypeUAV = AddParameterValue<OUTPUT_TYPE>( _T(g_outputTypeStr), OUTPUT_TYPE_UAV );

	// Setup RT ("output") sample counts
	testfactor::RFactor rfRTSampleCount1 = AddParameter<UINT>( _T(g_sampleCountStr), &m_uRTSampleCount, 1 );
	testfactor::RFactor rfRTSampleCount4 = AddParameterValue<UINT>( _T(g_sampleCountStr), 4 );

	// Setup forced sample counts (part of the rasterizer desc)
	// With a forced sample count/pattern selected at the rasterizer (ForcedSampleCount > 0), 
	// pixels are candidates for shader invocation based on the selected sample pattern, independent of the RT / UAV ("output") sample count
	testfactor::RFactor rfForcedSampleCount0 = AddParameter<UINT>( _T(g_forcedSampleCountStr), &m_uForcedSampleCount, 0u );	// For RTs, 0 means the feature is off and rendering is 'normal'. For UAVs, 0 means the same thing as 1.
	testfactor::RFactor rfForcedSampleCount1 = AddParameterValue<UINT>( _T(g_forcedSampleCountStr), 1 );
	testfactor::RFactor rfForcedSampleCount4 = AddParameterValue<UINT>( _T(g_forcedSampleCountStr), 4 );
	testfactor::RFactor rfForcedSampleCount8 = AddParameterValue<UINT>( _T(g_forcedSampleCountStr), 8 );
	testfactor::RFactor rfForcedSampleCount16 = AddParameterValue<UINT>( _T(g_forcedSampleCountStr), 16 );

	//
	// Create the bit patterns to represent what samples will be covered for a given sample count
	// Eg. 0x0 = No samples are covered. 0x1 = The first sample in the pattern is covered. 0xF = The first four samples in the pattern are covered.
	//

	CUserValueSet<UINT> *pCoveredSamples1XValues = new CUserValueSet<UINT>();
	pCoveredSamples1XValues->AddValue( g_noCoveredSamples );	// Don't cover any samples
	pCoveredSamples1XValues->AddValue( 0x00000001 );			// Cover sample 1
	testfactor::RFactor rfCoveredSamples1X = AddParameter<UINT>( _T(g_coveredSamplesStr), &m_uCoveredSamples, pCoveredSamples1XValues );
	
	CUserValueSet<UINT> *pCoveredSamples4XValues = new CUserValueSet<UINT>();
	pCoveredSamples4XValues->AddValue( g_noCoveredSamples );	// Don't cover any samples
	pCoveredSamples4XValues->AddValue( g_coverPixelCenter );	// Cover the pixel center, even though there is no sample location there
	pCoveredSamples4XValues->AddValue( 0x00000001 );			// Cover sample 1
	pCoveredSamples4XValues->AddValue( 0x00000002 );			// Cover sample 2
	pCoveredSamples4XValues->AddValue( 0x00000005 );			// Cover samples 1 and 3
	pCoveredSamples4XValues->AddValue( 0x0000000C );			// Cover samples 3 and 4
	pCoveredSamples4XValues->AddValue( 0x0000000F );			// Cover samples 1 through 4 (all samples)
	testfactor::RFactor rfCoveredSamples4X = AddParameterValueSet<UINT>( _T(g_coveredSamplesStr), pCoveredSamples4XValues );

	CUserValueSet<UINT> *pCoveredSamples8XValues = new CUserValueSet<UINT>();
	pCoveredSamples8XValues->AddValue( g_noCoveredSamples );	// Don't cover any samples
	pCoveredSamples8XValues->AddValue( g_coverPixelCenter );	// Cover the pixel center, even though there is no sample location there
	pCoveredSamples8XValues->AddValue( 0x00000001 );			// Cover sample 1
	pCoveredSamples8XValues->AddValue( 0x00000004 );			// Cover sample 3
	pCoveredSamples8XValues->AddValue( 0x00000081 );			// Cover samples 1 and 8
	pCoveredSamples8XValues->AddValue( 0x000000F0 );			// Cover samples 5 through 8
	pCoveredSamples8XValues->AddValue( 0x000000FF );			// Cover samples 1 through 8 (all samples)
	testfactor::RFactor rfCoveredSamples8X = AddParameterValueSet<UINT>( _T(g_coveredSamplesStr), pCoveredSamples8XValues );

	CUserValueSet<UINT> *pCoveredSamples16XValues = new CUserValueSet<UINT>();
	pCoveredSamples16XValues->AddValue( g_noCoveredSamples );	// Don't cover any samples
	pCoveredSamples16XValues->AddValue( g_coverPixelCenter );	// Cover the pixel center, even though there is no sample location there
	pCoveredSamples16XValues->AddValue( 0x00000002 );			// Cover sample 2
	pCoveredSamples16XValues->AddValue( 0x00008000 );			// Cover sample 16
	pCoveredSamples16XValues->AddValue( 0x00008181 );			// Cover samples 1, 8, 9, and 16
	pCoveredSamples16XValues->AddValue( 0x0000F0F0 );			// Cover samples 5 through 8, and 13 through 16
	pCoveredSamples16XValues->AddValue( 0x0000FFFF );			// Cover samples 1 through 16 (all samples)
	testfactor::RFactor rfCoveredSamples16X = AddParameterValueSet<UINT>( _T(g_coveredSamplesStr), pCoveredSamples16XValues );

	// Setup sample masks (Set as part of the blend state)
	// When rendering to a RT, a sample mask determines which samples get updated in all the active render targets.
    // When rendering to a UAV, a sample mask has no effect.
	CUserValueSet<UINT> *pSampleMaskValues = new CUserValueSet<UINT>();
	pSampleMaskValues->AddValue( 0x00000000 );	// No sample mask (Masked out writes)
	pSampleMaskValues->AddValue( 0x00000001 );	// Enable the first sample
	testfactor::RFactor rfSampleMask1X = AddParameter<UINT>( _T(g_sampleMaskStr), &m_uSampleMask, pSampleMaskValues );

	pSampleMaskValues->AddValue( 0x00000004 );	// Enable sample 3
	pSampleMaskValues->AddValue( 0x00000009 );	// Enable samples 1 and 4
	testfactor::RFactor rfSampleMask4X = AddParameterValueSet<UINT>( _T(g_sampleMaskStr), pSampleMaskValues );
	testfactor::RFactor rfDefaultSampleMask = AddParameterValue<UINT>( _T(g_sampleMaskStr), D3D11_DEFAULT_SAMPLE_MASK );

	// Setup what sample pattern "CoveredSamples" refers to (the forced sample count pattern, or the RT sample pattern)
	testfactor::RFactor rfCoveredSamplesRepresentsForcedSampleCountPattern = AddParameter<COVERED_SAMPLES_REPRESENTATION>( _T(g_coveredSamplesRepresentationStr), &m_coveredSamplesRepresentation, COVERED_SAMPLES_REPRESENTATION_FORCED_SAMPLE_COUNT_PATTERN );
	testfactor::RFactor rfCoveredSamplesRepresentsRTSampleCountPattern = AddParameterValue<COVERED_SAMPLES_REPRESENTATION>( _T(g_coveredSamplesRepresentationStr), COVERED_SAMPLES_REPRESENTATION_RENDER_TARGET_SAMPLE_PATTERN );

	// Setup quality levels
	testfactor::RFactor rfStandardMultisamplePatternQualityLevel = AddParameter<D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS>( _T(g_qualityLevelsStr), &m_qualityLevel, D3D11_STANDARD_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfCenterMultisamplePatternQualityLevel = AddParameterValue<D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS>( _T(g_qualityLevelsStr), D3D11_CENTER_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfQualityLevels = rfStandardMultisamplePatternQualityLevel + rfCenterMultisamplePatternQualityLevel;

    //
    // Create all the sampling variations apprortiate for the current driver type and output type
    //

    // First, set up the variations for RT output type

    testfactor::RFactor rfRTSamplingVariations =

        // Basic TIR on (forced sample count > 0)
        rfRTSampleCount1 * rfForcedSampleCount1 * rfCoveredSamples1X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask * rfStandardMultisamplePatternQualityLevel +
        rfRTSampleCount1 * rfForcedSampleCount4 * rfCoveredSamples4X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask * rfStandardMultisamplePatternQualityLevel + 
		rfRTSampleCount1 * rfForcedSampleCount8 * rfCoveredSamples8X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask * rfStandardMultisamplePatternQualityLevel + 
		rfRTSampleCount1 * rfForcedSampleCount16 * rfCoveredSamples16X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask * rfStandardMultisamplePatternQualityLevel +

        // Basic TIR off
		rfRTSampleCount1 * rfForcedSampleCount0 * rfCoveredSamples1X * rfCoveredSamplesRepresentsRTSampleCountPattern * rfDefaultSampleMask * rfStandardMultisamplePatternQualityLevel + 
		rfRTSampleCount4 * rfForcedSampleCount0 * rfCoveredSamples4X * rfCoveredSamplesRepresentsRTSampleCountPattern * rfDefaultSampleMask * rfQualityLevels +

		// Special case TIR on (MS RT, sample masks, etc...)
		rfRTSampleCount4 * rfForcedSampleCount1 * rfCoveredSamples1X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfSampleMask4X * rfQualityLevels +							// If ForcedSampleCount is 1, RTVs that are bound can be any sample count
		rfRTSampleCount4 * rfForcedSampleCount1 * rfCoveredSamples4X * rfCoveredSamplesRepresentsRTSampleCountPattern * rfSampleMask4X	* rfQualityLevels +								// Output sample locations have no bearing on whether a pixel is a candidate for Pixel Shader invocation
		rfRTSampleCount1 * rfForcedSampleCount1 * rfCoveredSamples4X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfSampleMask4X * rfStandardMultisamplePatternQualityLevel;  // Shader invocation happens for a candidate pixel if any of the output sample locations is in the SampleMask Blend State. One invocation occurs if any output sample is in the SampleMask

    // Now, set up the variations for UAV output type

    testfactor::RFactor rfUAVSamplingVariations =

        // Basic UAV only rasterization (forced sample count > 0)
        rfRTSampleCount1 * rfForcedSampleCount1 * rfCoveredSamples1X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask +
        rfRTSampleCount1 * rfForcedSampleCount4 * rfCoveredSamples4X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask + 
		rfRTSampleCount1 * rfForcedSampleCount8 * rfCoveredSamples8X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask + 
        rfRTSampleCount1 * rfForcedSampleCount16 * rfCoveredSamples16X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask +

        // Basic UAV only rasterization (forced sample count = 0, same as count = 1)
		rfRTSampleCount1 * rfForcedSampleCount0 * rfCoveredSamples1X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask + 
		rfRTSampleCount1 * rfForcedSampleCount0 * rfCoveredSamples4X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfDefaultSampleMask +

		// Special UAV only rasterization (sample masks, etc...)
		rfRTSampleCount1 * rfForcedSampleCount1 * rfCoveredSamples1X * rfCoveredSamplesRepresentsForcedSampleCountPattern * rfSampleMask4X;							    // The SampleMask Rasterizer State does not affect rasterization behavior at all

	// Combine factors
    testfactor::RFactor rfRoot = 

        // Output to Render Target
        (rfOutputTypeRT * rfRTSamplingVariations) +

        // Output to UAV
        (rfOutputTypeUAV * rfUAVSamplingVariations);

	SetRootTestFactor( rfRoot );
}

////

TEST_RESULT CForcedSampleCountTest::Setup()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;

    // Get the device
	m_pDevice = GetDevice1();
	m_pDeviceContext = GetEffectiveContext1();

    // Ensure the driver supports the 11.1 DDI
	if( !GetFramework()->Is11_1DDISupported( m_pDevice ) )
	{
		WriteToLog( _T("Skipping group. Target Independent Rasterization (Forced sample counts) is only supported on drivers that support the 11.1+ DDI.") );
		return RESULT_SKIP;
	}

	// LogicOps are optional for feature levels < 11.1 but this test requires them
	// TODO: Remove the test's dependance of logic ops (Win8:438895 - WGF11Rasterizer: The test requires logic ops. However logic ops are optional. The test shouldn't rely on logic ops.)
	if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 )
	{
		// Check for support
		D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11OptionsData;
		ZeroMemory( &d3d11OptionsData, sizeof(d3d11OptionsData) );
		if( FAILED( hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &d3d11OptionsData, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS) ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed.", false, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport );
			return RESULT_FAIL;
		}

		if( !d3d11OptionsData.OutputMergerLogicOp )
		{
			WriteToLog(_T("The driver doesn't support LogicOps. Skipping the group.") );
			return RESULT_SKIP;
		}
	}

	// SV_Coverage is not supported on FL10.X
	if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog( _T("Skipping group. The test uses SV_Coverage which is not supported on FL10.X. Other tests that use TIR would be WGF11Rasterizer's PullModel group and WGF11Multisample's Centroid group.") );
		return RESULT_SKIP;
	}

	// Print some helpful messages
	WriteToLog( _T("Test will create a %u x %u render target and UAV with format %s."), m_uRTWidth, m_uRTHeight, ToString(m_RTFormat).c_str() );

	// Create shaders
	if( !CreateShaders() )
	{
		WriteToLog( _T("CreateShaders() failed.") );
		goto setupDone;
	}

	//
	// Create a depth stencil state to disable depth testing
	//

	D3D11_DEPTH_STENCILOP_DESC depthOpDesc;
	depthOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthOpDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.BackFace = depthOpDesc;
	depthDesc.DepthEnable = false;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.FrontFace = depthOpDesc;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	if( FAILED( hr = m_pDevice->CreateDepthStencilState(&depthDesc, &m_pDepthStateDisabled) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateDepthStencilState() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateDepthStencilState );
		goto setupDone;
	}

	//
	// Create blend state
	//

	D3D11_BLEND_DESC1 blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].LogicOpEnable = true;
	blendDesc.RenderTarget[0].LogicOp = D3D11_LOGIC_OP_OR;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if( FAILED( hr = m_pDevice->CreateBlendState1(&blendDesc, &m_pBSLogicOR) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBlendState1() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateBlendState );
		goto setupDone;
	}

	//
	// Create the queries
	//

	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

	if( FAILED( hr = m_pDevice->CreateQuery( &queryDesc, &m_pPipelineQuery ) ) ) 
	{
		LogError( __FILEW__, __LINE__, L"CreateQuery() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateQuery );
		goto setupDone;
	}

	//
	// Create vertex buffer
	//

	// Check the resource is NULL
	assert( m_pVB == NULL );
	if( m_pVB != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pVB is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = 100 * sizeof( CForcedSampleCountTest::VERTEX );
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	if( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVB ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto setupDone;
	}

	//
	// Create and fill constant buffer
	//

	// Check the resource is NULL
	assert( m_pConstBuffer == NULL );
	if( m_pConstBuffer != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pConstBuffer is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = g_constBufferSize;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	if( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pConstBuffer ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto setupDone;
	}

	// Map and fill the constant buffer
	float *bufferData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	if( FAILED( m_pDeviceContext->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedBuffer ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
		goto setupDone;
	}
	bufferData = (float*)mappedBuffer.pData;

	// Prescaled inverse viewport translation
	bufferData[0] = -1.f;
	bufferData[1] = 1.f;

	// Inverse viewport scale
	bufferData[2] = 2.f / (float)m_uRTWidth;
	bufferData[3] = -2.f / (float)m_uRTHeight;

	// Unmap the constant buffer
	m_pDeviceContext->Unmap(m_pConstBuffer,0);

	// If we got here, we passed
	bSuccess = true;

setupDone:
	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

TEST_RESULT CForcedSampleCountTest::SetupTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;

	// Reset some flags
	m_bRenderedGeometryAtThePixelCenter = false;

	//
	// If a forced sample count is being used, make sure the driver supports it for the specified output type
	//

    // We already skip this group if the 11.1 DDI is not supported (even though forced sample count = 0 is valid)

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        // HW with FL < 11.1 requires support for forced sample counts set to 0 and 1,
        // HW with FL >= 11.1 additionally requires support for sample counts set to 4, 8 and 16

        if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 &&
            m_uForcedSampleCount > 1 )
        {
            WriteToLog( _T( "Skipping test case because forced sample counts > 1 for RTs are only supported on FL11.1+ HW." ) );
		    return RESULT_SKIP;
        }
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // HW with FL < 11.0 does not support UAV-only rasterization,
        // HW with 11.0 <= FL < 11.1 requires support for forced sample counts set to 0, 1, 4 and 8,
        // HW with FL >= 11.1 additionally requires support for sample count set to 16

        if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
        {
            WriteToLog( _T( "Skipping test case because UAV-only rasterization is only supported on FL11.0+ HW." ) );
            return RESULT_SKIP;
        }
        else if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 &&
                 m_uForcedSampleCount == 16 )
        {
            WriteToLog( _T( "Skipping test case because forced sample count of 16 for UAVs is only supported on FL11.1+ HW." ) );
            return RESULT_SKIP;
        }
    }

	//
	// Create rasterizer state
	//

	// Check the state is NULL
	assert( m_pRSEnabled == NULL );
	if( m_pRSEnabled != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pRSEnabled is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	// Win8:293911 - Sample count 1 is only valid on D3D_FEATURE_LEVEL_11_0 or lower when MultisampleEnable is false
	const bool bMultisampleEnable = ((m_uForcedSampleCount == 1) && (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1)) ? FALSE : TRUE;

	D3D11_RASTERIZER_DESC1 rastDesc;
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	rastDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.ForcedSampleCount = m_uForcedSampleCount;
	rastDesc.FrontCounterClockwise = FALSE;
	rastDesc.MultisampleEnable = bMultisampleEnable;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	if( FAILED( hr = m_pDevice->CreateRasterizerState1( &rastDesc, &m_pRSEnabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState1() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateRasterizerState );
		goto setupDone;
	}

    //
    // Create the texture that will hold the output for both RTV and UAV rendering cases
    //

    // Check the resource is NULL
	assert( m_pRTTexture2D == NULL );
	if( m_pRTTexture2D != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pRTTexture2D is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize = 1;
	tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | ((m_OutputType == OUTPUT_TYPE_RTV) ? D3D11_BIND_RENDER_TARGET : D3D11_BIND_UNORDERED_ACCESS);
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.Format = m_RTFormat;
	tex2DDesc.Height = m_uRTHeight;
	tex2DDesc.MipLevels = 1;
	tex2DDesc.MiscFlags = 0;
	tex2DDesc.SampleDesc.Count = m_uRTSampleCount;
	tex2DDesc.SampleDesc.Quality = (m_OutputType == OUTPUT_TYPE_RTV) ? m_qualityLevel : 0;
	tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2DDesc.Width = m_uRTWidth;
	if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pRTTexture2D ) ) )
	{
		WriteToLog( _T("Failed to create the render target texture.") );
		LogError( __FILEW__, __LINE__, L"CreateTexture2D() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto setupDone;
	}

	// Check the resource is NULL
	assert( m_pRTTexture2DStaging == NULL );
	if( m_pRTTexture2DStaging != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pRTTexture2DStaging is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	tex2DDesc.BindFlags = 0;
	tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_STAGING;
	if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pRTTexture2DStaging ) ) )
	{
		WriteToLog( _T("Failed to create the staging render target texture.") );
		LogError( __FILEW__, __LINE__, L"CreateTexture2D() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
		goto setupDone;
	}

	// Check the resource is NULL
	assert( m_pRTView == NULL );
	if( m_pRTView != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pRTView is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
	    //
	    // Create render target view and shader resource view
	    //	   

	    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	    rtvDesc.Format = m_RTFormat;
	    rtvDesc.Texture2D.MipSlice = 0;
	    rtvDesc.ViewDimension = (m_uRTSampleCount == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
	    if( FAILED( hr = m_pDevice->CreateRenderTargetView( m_pRTTexture2D, &rtvDesc, &m_pRTView ) ) )
	    {
		    LogError( __FILEW__, __LINE__, L"CreateRenderTargetView() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		    goto setupDone;
	    }

	    // Check the resource is NULL
	    assert( m_pSRView == NULL );
	    if( m_pSRView != NULL )
	    {
		    LogError( __FILEW__, __LINE__, L"m_pSRView is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		    goto setupDone;
	    }

	    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	    srvDesc.Format = rtvDesc.Format;
	    srvDesc.ViewDimension = (m_uRTSampleCount == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
	    srvDesc.Texture2D.MipLevels = 1;
	    srvDesc.Texture2D.MostDetailedMip = 0;
	    // Note: If the dimension is MS, then no fields are used
	    if( FAILED( hr = m_pDevice->CreateShaderResourceView( m_pRTTexture2D, &srvDesc, &m_pSRView ) ) )
	    {
		    LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		    goto setupDone;
	    }
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        //
	    // Create the UAV
	    //

        // Check the resource is NULL
	    assert( m_pUAV == NULL );
	    if( m_pUAV != NULL )
	    {
		    LogError( __FILEW__, __LINE__, L"m_pUAV is not NULL. It should be cleaned up for each test case.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		    goto setupDone;
	    }

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = m_RTFormat;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;
        if( FAILED( hr = m_pDevice->CreateUnorderedAccessView( m_pRTTexture2D, &uavDesc, &m_pUAV ) ) )
	    {
		    LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() for the output texture failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateView );
		    goto setupDone;
	    }
    }

	// Setup the vertex buffer
	// Fill it with verticies based on the current test case
	if( !SetupVB() )
	{
		WriteToLog( _T("SetupVB() failed.") );
		goto setupDone;
	}

	// Execute the effective context
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto setupDone;
	}

	// If we got here, we passed
	bSuccess = true;

setupDone:
	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

TEST_RESULT CForcedSampleCountTest::ExecuteTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;

    //
    // Set up output-type independent variables
    //

    const bool bTIRIsEnabled = m_uForcedSampleCount > 0;
	const bool bLogVerbose = GetFramework()->LogVerbose();
    const UINT elementByteWidth = max( GetBitsPerElement( m_RTFormat ) / 8, 1 );
    D3D11_MAPPED_SUBRESOURCE mappedSubres;
	UINT rtWidth = m_uRTWidth;
	UINT rtHeight = m_uRTHeight;

    //
    // Set up output-type dependent variables
    //
    
    bool bSamplePatternHasSampleLocationAtThePixelCenter = false;
    UINT effectiveSampleCount = 0;
    UINT effectiveSampleMask = 0;
    
    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        bSamplePatternHasSampleLocationAtThePixelCenter = 
		    (m_uForcedSampleCount == 1) ||																						// The 1x forced sample pattern has a sample location at the center
		    ((m_uForcedSampleCount == 0) && (m_uRTSampleCount == 1)) ||															// If TIR is off, and the output is single sample
		    ((m_uForcedSampleCount == 0) && (m_uRTSampleCount > 1) && (m_qualityLevel == D3D11_CENTER_MULTISAMPLE_PATTERN) );	// If the output is multisample, but the quality level is the CENTER pattern

        effectiveSampleCount = bTIRIsEnabled ? m_uForcedSampleCount : m_uRTSampleCount;

        effectiveSampleMask = m_uSampleMask;
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // For UAVs, forced sample count 0 is the same as 1. So both
        // cases will yield a sample location at the center, just like
        // single-sampled RTVs
        bSamplePatternHasSampleLocationAtThePixelCenter = 
            (m_uForcedSampleCount == 0) || (m_uForcedSampleCount == 1);														// The 0x and 1x forced sample pattern has a sample location at the center

        // Forced sample count of 0 is the same as 1
        effectiveSampleCount = (m_uForcedSampleCount == 0) ? 1 : m_uForcedSampleCount;

        // No matter what sample mask is specified in the OM stage, the expected sample mask is the default one
        effectiveSampleMask = D3D11_DEFAULT_SAMPLE_MASK;
    }

    //
    // Set up output-type independent pipeline state
    //

    // Rasterizer
	m_pDeviceContext->RSSetState( m_pRSEnabled );
	m_pDeviceContext->RSSetViewports( 1, &m_Viewport );

    // Output Merger
    m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDisabled, 0 ); // Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined
    WriteToLog( _T("Blend State Sample Mask set to %#X"), m_uSampleMask );
    WriteToLog( _T("Expected Effective Sample Mask is %#X"), effectiveSampleMask );

    // Shader resources
	m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

    // Shaders
	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );

    // Input assembler
	UINT strides[] = { sizeof( CForcedSampleCountTest::VERTEX ) };

	UINT offsets[] = { 0 };
	m_pDeviceContext->IASetInputLayout( m_pVertexLayout );
	m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, strides, offsets );
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //
    // Set up output-type dependent pipeline state
    //

	const FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const UINT clearColorUint[4] = {0, 0, 0, 0};

    if( m_OutputType == OUTPUT_TYPE_RTV )
    {
        // Clear the RTV
	    m_pDeviceContext->ClearRenderTargetView( m_pRTView, clearColor );

        // Output merger resources
        m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
        m_pDeviceContext->OMSetBlendState( m_pBSLogicOR, 0, m_uSampleMask );

	    // Shaders
	    m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );

        ID3D11ShaderResourceView* pNULL = NULL;
	    m_pDeviceContext->PSSetShaderResources( 0, 1, &pNULL );	// Clear the SRV used when resolving multi-sampled RTs
    }
    else // m_OutputType == OUTPUT_TYPE_UAV
    {
        // Clear the UAV
        m_pDeviceContext->ClearUnorderedAccessViewUint( m_pUAV, clearColorUint );

        // Output merger resources
        m_pDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, 0, 1, &m_pUAV, NULL );
        m_pDeviceContext->OMSetBlendState( NULL, 0, m_uSampleMask );

	    // Shaders
	    m_pDeviceContext->PSSetShader( m_pUAVPS, NULL, 0 );
    }

	//
	// Draw
	//

	m_pDeviceContext->Begin( m_pPipelineQuery );
	m_pDeviceContext->Draw( m_uVertexCount, 0 );
	m_pDeviceContext->End( m_pPipelineQuery );

	// Execute the effective context
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto executeDone;
	}

	// Check query data
	// We have to call get data on the immediate context
	// Note we don't check the HR - since it can fail
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineQuery, &pipelineStats, sizeof( pipelineStats ), 0 );

    //
    // Resolve the multi-sampled render target if needed, then map out the output texture
    //

	if( m_uRTSampleCount > 1 )
	{
        assert( m_OutputType == OUTPUT_TYPE_RTV );

		if( bLogVerbose )
		{
			WriteToLog( _T("The render target is multisampled (sample count = %u). An extra render pass will be performed to convert it to a single sample render target."), m_uRTSampleCount );
		}

		// We have a MSRT, we need to convert to a single sample RT
		ID3D10Blob *pBlobBuffer = NULL;
		ID3D10Blob *pErrorBuffer = NULL;

		// Sample Readback with Load:
		// New Render Target & Staging pair
		D3D11_TEXTURE2D_DESC rtdesc;
		m_pRTTexture2D->GetDesc( &rtdesc );

		rtdesc.Width *= rtdesc.SampleDesc.Count;
		rtdesc.SampleDesc.Count = 1;
		rtdesc.SampleDesc.Quality = 0;
		assert( !m_pRTTexture2DSS );
		if( FAILED( hr = m_pDevice->CreateTexture2D( &rtdesc, NULL, &m_pRTTexture2DSS ) ) )
		{
			WriteToLog( _T("Failed to create the single sample render target texture.") );
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			goto setupDone;
		}

		assert( !m_pSSRTView );
		if( FAILED( hr = m_pDevice->CreateRenderTargetView( m_pRTTexture2DSS, NULL, &m_pSSRTView ) ) )
		{
			WriteToLog( _T("Failed to create the single sample render target view.") );
			LogError( __FILEW__, __LINE__, L"CreateRenderTargetView() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			goto setupDone;
		}

		rtdesc.Usage = D3D11_USAGE_STAGING;
		rtdesc.BindFlags = 0;
		rtdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		assert( !m_pRTTexture2DSSStaging );
		if( FAILED( hr = m_pDevice->CreateTexture2D( &rtdesc, NULL, &m_pRTTexture2DSSStaging ) ) )
		{
			WriteToLog( _T("Failed to create the staging single sample render target texture.") );
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() for the RT failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateResource );
			goto setupDone;
		}

		// Create pixel shader to convert from a multisample render target to a single sample render target
		char szMSPS[] =
				"Texture2DMS< uint, %d > MyTex;											\n"
				"struct PSIn																\n"
				"{																			\n"
				"	float4 pos : SV_Position;												\n"
				"	float2 tex : tex;														\n"
				"};																			\n"
				"struct PSOut																\n"
				"{																			\n"
				"	uint4 Color : SV_Target;												\n"
				"};																			\n"
				"cbuffer cb0 : register(b0)													\n"
				"{																			\n"
				"	float4 inverseViewport;													\n"
				"};																			\n"
				"PSOut PSMain(PSIn input)													\n"
				"{																			\n"
				"	uint iX = input.pos.x;													\n"
				"	uint iS = %d;															\n"
				"	PSOut output;															\n"
				"	output.Color = uint4(0,0,0,1);											\n"
				"	output.Color.x = MyTex.Load( int3(iX/iS,input.pos.y,0), iX%%iS ).x;		\n"
				"	return output;															\n"
				"}";

		// Fill in the variables
		const UINT MAX_STR = 4000;
		char szShader[ MAX_STR ];
		_snprintf( szShader, MAX_STR-1, szMSPS, m_uRTSampleCount, m_uRTSampleCount );
		szShader[MAX_STR-1] = 0;

		if ( FAILED( hr = D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "PSMain",
			GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0),
			D3D10_SHADER_DEBUG, 0, NULL, &pBlobBuffer, &pErrorBuffer, NULL )) )
		{
			if( pErrorBuffer )
			{
				tstring errorStr = (char*)pErrorBuffer->GetBufferPointer();
				WriteToLog( _T("%s"), errorStr.c_str() );
			}

			LogError( __FILEW__, __LINE__, L"D3DX11CompileFromMemory() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );
			goto setupDone;
		}

		if ( FAILED( hr = m_pDevice->CreatePixelShader( pBlobBuffer->GetBufferPointer(), pBlobBuffer->GetBufferSize(), NULL, &m_pPSMSToSS )) )
		{
			LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
			goto setupDone;
		}

		// The actual render target that will be mapped will now be wider (each texel represents a sample)
		rtWidth = m_uRTWidth * m_uRTSampleCount;
		rtHeight = m_uRTHeight;

		// Create a new viewport based on the new width and height
		D3D11_VIEWPORT viewport;
		viewport.Height = (float) rtHeight;
		viewport.Width = (float) rtWidth;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		//
		// Update the vertex buffer to cover the entire render target
		//

		// Reset the vertex buffer count
		m_uVertexCount = 0;

		// Map and fill the vertex buffer
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		if( FAILED( hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedBuffer ) ) )
		{
			LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
			goto setupDone;
		}

		CForcedSampleCountTest::VERTEX* pDestVerts = (CForcedSampleCountTest::VERTEX*) mappedBuffer.pData;

		// First vertex
		assert( m_uVertexCount == 0 );
		pDestVerts[m_uVertexCount].pos[0] = 0;
		pDestVerts[m_uVertexCount].pos[1] = 0;
		pDestVerts[m_uVertexCount].tex[0] = pDestVerts[m_uVertexCount].tex[1] = 0;
		m_uVertexCount++;

		// Second vertex
		pDestVerts[m_uVertexCount].pos[0] = 0;
		pDestVerts[m_uVertexCount].pos[1] = static_cast<float>(rtHeight);
		pDestVerts[m_uVertexCount].tex[0] = 0;
		pDestVerts[m_uVertexCount].tex[1] = 1;
		m_uVertexCount++;

		// Third vertex
		pDestVerts[m_uVertexCount].pos[0] = static_cast<float>(rtWidth);
		pDestVerts[m_uVertexCount].pos[1] = 0;
		pDestVerts[m_uVertexCount].tex[0] = 1;
		pDestVerts[m_uVertexCount].tex[1] = 0;
		m_uVertexCount++;

		// Fourth vertex
		pDestVerts[m_uVertexCount].pos[0] = static_cast<float>(rtWidth);
		pDestVerts[m_uVertexCount].pos[1] = static_cast<float>(rtHeight);
		pDestVerts[m_uVertexCount].tex[0] = 1;
		pDestVerts[m_uVertexCount].tex[1] = 1;
		m_uVertexCount++;

		// Unmap the vertex buffer
		GetEffectiveContext()->Unmap( m_pVB, 0 );

		//
		// Setup pipeline
		//

		// Output merger resources
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pSSRTView, NULL );

		// Output merger
		m_pDeviceContext->OMSetBlendState( NULL, 0, D3D11_DEFAULT_SAMPLE_MASK );	// Disable blending and logicOps

		// Rasterizer
		m_pDeviceContext->RSSetViewports( 1, &viewport );

		// Shader resources
		m_pDeviceContext->PSSetShaderResources( 0, 1, &m_pSRView );

		// Shaders
		m_pDeviceContext->PSSetShader( m_pPSMSToSS, NULL, 0 );

		// Input assembler
		m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		//
		// Draw
		//

		m_pDeviceContext->Begin( m_pPipelineQuery );
		m_pDeviceContext->Draw( m_uVertexCount, 0 );
		m_pDeviceContext->End( m_pPipelineQuery );

		// Execute the effective context
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
			goto executeDone;
		}

		// Check query data
		// We have to call get data on the immediate context
		// Note we don't check the HR - since it can fail
		D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
		GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineQuery, &pipelineStats, sizeof( pipelineStats ), 0 );

		// The output must be multisample sample already
		assert( m_uRTSampleCount > 1 );

		// Copy the render target to a staging copy
		m_pDeviceContext->CopyResource( m_pRTTexture2DSSStaging, m_pRTTexture2DSS );

		// Execute the effective context
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
			goto executeDone;
		}

		// Map the staging copy
		if( FAILED( hr = GetImmediateContext1()->Map( m_pRTTexture2DSSStaging, 0, D3D11_MAP_READ, 0, &mappedSubres ) ) )
		{
			LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
			goto executeDone;
		}

setupDone:
		SAFE_RELEASE( pBlobBuffer );
		SAFE_RELEASE( pErrorBuffer );
	}
	else
	{
		// The output must be single sample already
		assert( m_uRTSampleCount == 1 );

		// Copy the output texture to a staging copy
		m_pDeviceContext->CopyResource( m_pRTTexture2DStaging, m_pRTTexture2D );

		// Execute the effective context
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
			goto executeDone;
		}

		// Map the staging copy
		if( FAILED( hr = GetImmediateContext1()->Map( m_pRTTexture2DStaging, 0, D3D11_MAP_READ, 0, &mappedSubres ) ) )
		{
			LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
			goto executeDone;
		}
	}

	//
	// Verify the output texture
	// The output texture should contain the coverage information for each pixel (1x 32-bit UINT for each pixel)
	//

	if( bLogVerbose )
	{
		WriteToLog( _T("Output Texture mapped. pData = %#x. Row pitch = %u. DepthPitch = %u."), mappedSubres.pData, mappedSubres.RowPitch, mappedSubres.DepthPitch );
		WriteToLog( _T("Output Texture element byte width = %u."), elementByteWidth );
		WriteToLog( _T("Checking contents of Output Texture...") );
	}

	// Read the UINT coverage data for each pixel
	for( UINT r = 0; r < rtHeight; r++ )
	{
		const UINT *pStartOfCurrentRow = (UINT*)( (BYTE*)mappedSubres.pData + (r * mappedSubres.RowPitch) );
		if( bLogVerbose )
		{
			WriteToLog( _T("Row %u starts at %#x."), r, pStartOfCurrentRow );
		}

		for( UINT c = 0; c < rtWidth; c = c + m_uRTSampleCount )
		{
			bool bPixelFailure = false;
			for( UINT sample = 0; sample < m_uRTSampleCount; sample++ )
			{
				const UINT pixelCoverage = pStartOfCurrentRow[c + sample];
				if( bLogVerbose )
				{
					WriteToLog( _T("Coverage for pixel (%u, %u), sample index %u = %#X."), r, c, sample, pixelCoverage );
				}

				// Generate the expected coverage
				UINT expectedPixelCoverage = 0;
				if( r == 0 && c == 0 )
				{
					const UINT currentSampleIndex = 1 << sample;

					// Only Pixel (0, 0) should have something besides 0, because all geometry is rendered to the first pixel.

					// Check to see if we rendered to the center of the pixel, but the pattern doesn't have a sample location there
					if( m_bRenderedGeometryAtThePixelCenter && 
						!bSamplePatternHasSampleLocationAtThePixelCenter )
					{
						// We rendered to the center of the pixel
						if( bLogVerbose )
						{	
							WriteToLog( _T("Only the center of pixel (%u, %u) was covered with geometry. However, the sample pattern doesn't have a sample location at the pixel center. Thus, the input coverage should be %#X."), 0 );
						}
					}
					else if( !m_bRenderedGeometryAtThePixelCenter &&
						bSamplePatternHasSampleLocationAtThePixelCenter )
					{
						// We didn't render to the center of the pixel
						if( bLogVerbose )
						{	
							WriteToLog( _T("The sample pattern has a sample location at the pixel center. However, it was not covered with geometry. Thus the input coverage should be %#X."), 0 );
						}
					}
					else if( (effectiveSampleMask & currentSampleIndex) == 0 )	// Is the output sample in the sample mask?
					{
						// We masked out the write
						if( bLogVerbose )
						{	
							WriteToLog( _T("The output texture is %s and the sample mask doesn't include sample index %u. The write has been masked out, thus the output texture should contain %#X."), 
								(m_uRTSampleCount == 1) ? "single sample" : "multisample", sample, 0 );
						}
					}
					else
					{
						UINT coveredSamples = m_uCoveredSamples;
						if( m_bRenderedGeometryAtThePixelCenter &&
							bSamplePatternHasSampleLocationAtThePixelCenter )
						{
							// We need to fix up the covered sample representation because multiple samples share the same location
							coveredSamples = (1 << effectiveSampleCount) - 1;
						}

						if( bTIRIsEnabled ||
							( m_OutputType == OUTPUT_TYPE_RTV && m_qualityLevel == D3D11_CENTER_MULTISAMPLE_PATTERN ) )
						{
							// The input coverage should exactly match the covered samples.
							// Every sample in the pixel should have the same coverage.
							expectedPixelCoverage = coveredSamples;
						}
						else
						{
							// Only the covered samples will have a non-zero input coverage.
							// Each sample will have different coverage, because each sample is covered by a different triangle.
							// If the sample was covered, the input coverage should be the sample index.
							expectedPixelCoverage = coveredSamples & currentSampleIndex;

							if( expectedPixelCoverage == 0 )
							{
								if( bLogVerbose )
								{	
									WriteToLog( _T("The render target is multisample and sample index %u wasn't covered with geometry. Thus its input coverage should be %#X"), sample, 0 );
								}
							}
						}
					}
				}

				// Check the result with the expected value
				if( pixelCoverage != expectedPixelCoverage )
				{
					WriteToLog( _T("Expected coverage for pixel (%u, %u), sample index %u = %#X"), r, c, sample, expectedPixelCoverage );
					WriteToLog( _T("Actual coverage for pixel (%u, %u), sample index %u = %#X"), r, c, sample, pixelCoverage );

					if( !bPixelFailure && bLogVerbose )
					{
						WriteToLog( _T("Note: Actual coverage is logically OR'd together for each pixel shader invocation. Failures could be related to LogicOp failures.") );
					}

					LogError( __FILEW__, __LINE__, L"Actual result didn't match the expected value.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConformanceFailure );
					bPixelFailure = true;
				}
			}

			// See if there was a failure checking the pixel. If so, early exit.
			if( bPixelFailure )
			{
				if( !bLogVerbose )
				{
					WriteToLog( _T("Run the test with \"-logverbose\" on the command line to get more detailed output.") );
				}

				goto executeDone;
			}
		}
	}

	// Unmap the staging copy
	if( m_uRTSampleCount == 1 )
	{
		GetImmediateContext1()->Unmap( m_pRTTexture2DStaging, 0 );
	}
	else
	{
		GetImmediateContext1()->Unmap( m_pRTTexture2DSSStaging, 0 );
	}

	// If we got here, we passed
	bSuccess = true;

executeDone:
	return bSuccess ? RESULT_PASS : RESULT_FAIL;
}

////

void CForcedSampleCountTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTTexture2DStaging );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pUAV );

	SAFE_RELEASE( m_pRTTexture2DSS );
	SAFE_RELEASE( m_pRTTexture2DSSStaging );
	SAFE_RELEASE( m_pSSRTView );
	SAFE_RELEASE( m_pPSMSToSS );
}

////

void CForcedSampleCountTest::Cleanup()
{
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pUAVPS );
	
	SAFE_RELEASE( m_pDepthStateDisabled );
	SAFE_RELEASE( m_pPipelineQuery );
	
	CRasterizerTest::Cleanup();
}

////////////////////////
// Helper functions

bool CForcedSampleCountTest::CreateShaders()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	ID3D10Blob *pBlobBuffer = NULL;
	ID3D10Blob *pErrorBuffer = NULL;

    //
	// Create vertex shader
    //

	assert( m_pVS == NULL );
	if( m_pVS != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pVS is not NULL. It should be cleaned up before the test starts.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	char szVS[] = 
		"struct VSIn																\n"
		"{																			\n"
		"	float2 pos : pos;														\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"struct VSOut																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"};																			\n"
		"VSOut VSMain(VSIn input)													\n"
		"{																			\n"
		"	VSOut output;															\n"
		"	output.pos = float4( 0, 0, 0, 1.0 );									\n"
		"	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;	\n"
		"	output.tex = input.tex;													\n"
		"	return output;															\n"
		"}";

	if ( FAILED( hr = D3DX11CompileFromMemory( szVS, strlen(szVS), NULL, NULL, NULL, "VSMain",
		GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0),
		D3D10_SHADER_DEBUG, 0, NULL, &pBlobBuffer, &pErrorBuffer, NULL )) )
	{
		if( pErrorBuffer )
		{
			tstring errorStr = (char*)pErrorBuffer->GetBufferPointer();
			WriteToLog( _T("%s"), errorStr.c_str() );
		}

		LogError( __FILEW__, __LINE__, L"D3DX11CompileFromMemory() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );
		goto setupDone;
	}

	if ( FAILED( hr = m_pDevice->CreateVertexShader( pBlobBuffer->GetBufferPointer(), pBlobBuffer->GetBufferSize(), NULL, &m_pVS )) )
	{
		LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
		goto setupDone;
	}

    //
	// Create input layout
    //

	assert( m_pVertexLayout == NULL );
	if( m_pVertexLayout != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pVertexLayout is not NULL. It should be cleaned up before the test starts.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	const D3D11_INPUT_ELEMENT_DESC iaLayout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "tex", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT numElements = ARRAY_SIZE( iaLayout );
	if(	FAILED(	hr = m_pDevice->CreateInputLayout( iaLayout, numElements, pBlobBuffer->GetBufferPointer(), pBlobBuffer->GetBufferSize(), &m_pVertexLayout ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateInputLayout() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateInputLayout );
		goto setupDone;
	}

	// Cleanup VS items
	assert( m_pVS );
	SAFE_RELEASE( pBlobBuffer );
	SAFE_RELEASE( pErrorBuffer );

    //
    // Create pixel shaders
    //

	// Create pixel shader for RTV-based rendering type
	assert( m_pPS == NULL );
	if( m_pPS != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pPS is not NULL. It should be cleaned up before the test starts.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	char szPS[] = 
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"struct PSOut																\n"
		"{																			\n"
		"	uint4 Color : SV_Target;												\n"
		"};																			\n"
		"PSOut PSMain(PSIn input, uint coverage : SV_COVERAGE)						\n"
		"{																			\n"
		"	PSOut output;															\n"
		"	output.Color = uint4(0,0,0,1);											\n"
		"	output.Color.x = coverage;												\n"
		"	return output;															\n"
		"}";	

	if ( FAILED( hr = D3DX11CompileFromMemory( szPS, strlen(szPS), NULL, NULL, NULL, "PSMain",
		GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0),
		D3D10_SHADER_DEBUG, 0, NULL, &pBlobBuffer, &pErrorBuffer, NULL )) )
	{
		if( pErrorBuffer )
		{
			tstring errorStr = (char*)pErrorBuffer->GetBufferPointer();
			WriteToLog( _T("%s"), errorStr.c_str() );
		}

		LogError( __FILEW__, __LINE__, L"D3DX11CompileFromMemory() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );
		goto setupDone;
	}

	if ( FAILED( hr = m_pDevice->CreatePixelShader( pBlobBuffer->GetBufferPointer(), pBlobBuffer->GetBufferSize(), NULL, &m_pPS )) )
	{
		LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
		goto setupDone;
	}

    assert( m_pPS );
	SAFE_RELEASE( pBlobBuffer );
	SAFE_RELEASE( pErrorBuffer );

    // Create pixel shader for UAV-based rendering type
    assert( m_pUAVPS == NULL );
	if( m_pUAVPS != NULL )
	{
		LogError( __FILEW__, __LINE__, L"m_pUAVPS is not NULL. It should be cleaned up before the test starts.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
		goto setupDone;
	}

	char szUAVPS[] =
        "RWTexture2D<uint> txUAV;                                                   \n"
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"void PSMain(PSIn input, uint coverage : SV_COVERAGE)						\n"
		"{																			\n"
        "   InterlockedOr(txUAV[uint2(input.pos.xy)], coverage);                    \n"
		"}";

	if ( FAILED( hr = D3DX11CompileFromMemory( szUAVPS, strlen(szUAVPS), NULL, NULL, NULL, "PSMain",
		GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0),
		D3D10_SHADER_DEBUG, 0, NULL, &pBlobBuffer, &pErrorBuffer, NULL )) )
	{
		if( pErrorBuffer )
		{
			tstring errorStr = (char*)pErrorBuffer->GetBufferPointer();
			WriteToLog( _T("%s"), errorStr.c_str() );
		}

		LogError( __FILEW__, __LINE__, L"D3DX11CompileFromMemory() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3DX::gFCCompileShader );
		goto setupDone;
	}

	if ( FAILED( hr = m_pDevice->CreatePixelShader( pBlobBuffer->GetBufferPointer(), pBlobBuffer->GetBufferSize(), NULL, &m_pUAVPS )) )
	{
		LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCreateShader );
		goto setupDone;
	}

    assert( m_pUAVPS );

	// If we got here, we passed
	bSuccess = true;

setupDone:
	SAFE_RELEASE( pBlobBuffer );
	SAFE_RELEASE( pErrorBuffer );
	return bSuccess;
}

////

bool CForcedSampleCountTest::SetupVB()
{
	// Local variables
	bool bSuccess = false;
	UINT effectiveSampleCount = 0;
	const float *stdMSOffsets = NULL;
	const UINT pixelX = 0;	// The pixel coordinate to cover - could make this a user controlled variable
	const UINT pixelY = 0;	// The pixel coordinate to cover - could make this a user controlled variable

	// Reset the vertex count
	m_uVertexCount = 0;

	// Setup geometry based on the sample count the user requests
	switch( m_coveredSamplesRepresentation )
	{
	default:
		assert(false);
		goto setupDone;
		break;

	case COVERED_SAMPLES_REPRESENTATION_FORCED_SAMPLE_COUNT_PATTERN:
		// The user wants to have geometry based on the forced sample count pattern
		effectiveSampleCount = (m_uForcedSampleCount == 0 && m_OutputType == OUTPUT_TYPE_UAV) ? 1 : m_uForcedSampleCount;
		break;
		
	case COVERED_SAMPLES_REPRESENTATION_RENDER_TARGET_SAMPLE_PATTERN:
		// The user wants to have geometry based on the render target's sample count pattern
		effectiveSampleCount = m_uRTSampleCount;
		break;
	}

	// Get the multisample offsets based on the sample count
	switch( effectiveSampleCount )
	{
	default:
		assert(false);
		goto setupDone;
		break;

	case 1:
		stdMSOffsets = g_stdMS1Offsets;
		break;

	case 4:
		stdMSOffsets = g_stdMS4Offsets;
		break;
		
	case 8:
		stdMSOffsets = g_stdMS8Offsets;
		break;

	case 16:
		stdMSOffsets = g_stdMS16Offsets;
		break;
	}

	// Add verticies that cover sections of the pixel
	if( m_uCoveredSamples == g_coverPixelCenter )
	{
		WriteToLog( _T("\"%s\" special case. Adding vertices to cover the center of pixel (%u, %u), even if the sample pattern doesn't have a sample location there."), g_coveredSamplesStr.c_str(), pixelX, pixelY );
		if( !CoverSubpixel( pixelX, pixelY, 0.5f, 0.5f ) )
		{
			WriteToLog( _T("CoverSubpixel() failed.") );
			goto setupDone;
		}
	}
	else
	{
		// Go through each requested sample...
		WriteToLog( _T("Going through the effective sample count (%u) and adding vertices to cover samples based on it's sample pattern..."), effectiveSampleCount );
		WriteToLog( _T("The test will cover samples based on the \"%s\" parameter's bit pattern (%#X)."), g_coveredSamplesStr.c_str(), m_uCoveredSamples );
		for( UINT i = 0; i < effectiveSampleCount; i++ )
		{
			// Ensure the current sample is in the test case's mask (meaning it should be covered)
			assert( i < 32 );
			const UINT currentSample = 1 << i;
			if( (m_uCoveredSamples & currentSample) == 0 )
			{
				// The current sample shouldn't be covered
				continue;
			}

			// If we got here, the vertex buffer needs to be updated to cover this sample
			float currentSampleOffsetX = 0;
			float currentSampleOffsetY = 0;

			if( m_qualityLevel == D3D11_CENTER_MULTISAMPLE_PATTERN &&
                m_OutputType == OUTPUT_TYPE_RTV &&
				m_coveredSamplesRepresentation == COVERED_SAMPLES_REPRESENTATION_RENDER_TARGET_SAMPLE_PATTERN )
			{
				// Note: The forced sample pattern is always the standard pattern...never the center pattern...regardless of the quality level.
				currentSampleOffsetX = g_stdMS1Offsets[0];
				currentSampleOffsetY = g_stdMS1Offsets[1];
			}
			else
			{
				currentSampleOffsetX = stdMSOffsets[i * 2];
				currentSampleOffsetY = stdMSOffsets[(i * 2) + 1];
			}

			WriteToLog( _T("Adding a triangle to the vertex buffer for pixel (%u, %u), sample index %u. The sample's subpixel offset is (%f, %f)."), pixelX, pixelY, i, currentSampleOffsetX, currentSampleOffsetY );
			if( !CoverSubpixel( pixelX, pixelY, currentSampleOffsetX, currentSampleOffsetY ) )
			{
				WriteToLog( _T("CoverSubpixel() failed.") );
				goto setupDone;
			}
		}
	}

	// If we got here, we passed
	WriteToLog( _T("%u vertices have been added to the vertex buffer."), m_uVertexCount );
	bSuccess = true;

setupDone:
	return bSuccess;
}

////

bool CForcedSampleCountTest::CoverSubpixel( const UINT &pixelX, const UINT &pixelY, const float &sampleOffsetX, const float &sampleOffsetY )
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool bSuccess = false;
	const float subpixelSize = 1.0 / 256.0;	// The pixel is divided into a 256x256 subpixel grid
	const float x = pixelX + sampleOffsetX;
	const float y = pixelY + sampleOffsetY;
	CForcedSampleCountTest::VERTEX *pDestVerts = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;

	if( sampleOffsetX == 0.5f && sampleOffsetY == 0.5f )
	{
		// Set a flag to indicate geometry was rendered to the center of the pixel
		m_bRenderedGeometryAtThePixelCenter = true;
	}

	// Execute the effective context
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto setupDone;
	}

	// Map and fill the vertex buffer
	// Note: D3D11_MAP_WRITE_NO_OVERWRITE. The vertex buffer is reset for each test case before the draw call.
	if( FAILED( hr = GetImmediateContext1()->Map( m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, NULL, &mappedBuffer ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Map() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextMap );
		goto setupDone;
	}

	pDestVerts = (CForcedSampleCountTest::VERTEX*) mappedBuffer.pData;

	// First vertex
	pDestVerts[m_uVertexCount].pos[0] = x;
	pDestVerts[m_uVertexCount].pos[1] = y;
	pDestVerts[m_uVertexCount].tex[0] = pDestVerts[m_uVertexCount].tex[1] = 0;
	m_uVertexCount++;

	// Second vertex
	pDestVerts[m_uVertexCount].pos[0] = x + subpixelSize;
	pDestVerts[m_uVertexCount].pos[1] = y;
	pDestVerts[m_uVertexCount].tex[0] = 1;
	pDestVerts[m_uVertexCount].tex[1] = 0;
	m_uVertexCount++;

	// Third vertex
	pDestVerts[m_uVertexCount].pos[0] = x;
	pDestVerts[m_uVertexCount].pos[1] = y + subpixelSize;
	pDestVerts[m_uVertexCount].tex[0] = 0;
	pDestVerts[m_uVertexCount].tex[1] = 1;
	m_uVertexCount++;

	// Unmap the vertex buffer
	GetImmediateContext1()->Unmap( m_pVB, 0 );

	// If we got here, we passed
	bSuccess = true;

setupDone:
	return bSuccess;
}