
// Includes
#include "Discard.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"

#define WCHAR_STR_SIZE 1024

// Create the strings for parameter names
const tstring g_doubleDiscardStr = "DoubleDiscard";
const tstring g_resourceArraySizeStr = "ResourceArraySize";
const tstring g_resourceNumMipLevelsStr = "ResourceNumMipLevels";
const tstring g_resourceScenarioStr = "ResourceScenario";
const tstring g_resourceUsageStr = "ResourceUsage";
const tstring g_resourceBindFlagStr = "ResourceBindFlag";
const tstring g_cpuAccessStr = "ResourceCPUAccessFlags";
const tstring g_resourceWidthStr = "ResourceWidth";
const tstring g_resourceHeightStr = "ResourceHeight";
const tstring g_resourceDepthStr = "ResourceDepth";
const tstring g_resourceInitWithData = "InitResourceData";
const tstring g_resourceFormatStr = "ResourceFormat";
const tstring g_resourceSampleCountStr = "ResourceSampleCount";
const tstring g_resourceSampleQualityStr = "ResourceSampleQuality";
const tstring g_resourceMiscFlagStr = "ResourceMiscFlag";

const tstring g_viewMipSliceStr = "ViewMipSlice";
const tstring g_viewFirstArraySliceStr = "ViewFirstArraySlice";
const tstring g_viewArraySizeStr = "ViewArraySize";
const tstring g_viewMostDetailedMipStr = "ViewMostDetailedMip";
const tstring g_viewMipLevelsStr = "ViewMipLevels";
const tstring g_viewElementOffsetStr = "ViewElementOffset";
const tstring g_viewElementWidthStr = "ViewElementWidth";
const tstring g_viewFirstWSliceStr = "ViewFirstWSlice";
const tstring g_viewWSizeStr = "ViewWSize";
const tstring g_resourceViewDSVFlagStr = "ViewDSVFlag";

const tstring g_rectScenarioStr = "RectScenario";
#if USE_GUARD_RECT_SCENARIO
const tstring g_guardRectScenarioStr = "GuardRectScenario";
#endif

bool PointCompareLessThan(POINT lhs, POINT rhs)
{
	if( lhs.y < rhs.y )
	{
		return true;
	}
	else if( lhs.y == rhs.y )
	{
		if( lhs.x < rhs.x )
		{
			return true;
		}
	}

	return false;
}
bool(*g_pfnPointCompareLessThan)(POINT,POINT) = PointCompareLessThan;

////////////////////////
// CDiscardTest

TEST_RESULT CDiscardTest::Setup()
{
	// Local variables
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_FAIL;

	// IMPORTANT: The Discard Test should always be run. If the driver doesn't support it the runtime will just NOOP the Discard call.
	m_pDevice1 = GetFramework()->GetDevice1();
	m_pEffectiveContext1 = GetFramework()->GetEffectiveContext1();

#if USE_GUARD_RECT_SCENARIO
	// Get the partner device for guard rects
	hr = m_pDevice1->QueryInterface( __uuidof(m_pPartnerDevice),  (void**) &m_pPartnerDevice );
	if( FAILED(hr) )
	{
		LogError( __FILEW__, __LINE__, L"Unable to QueryInterface for a ID3D11PartnerDevice interface.", hr, ERRORCODE_TYPE_HRESULT); 
		WriteToLog("IID_ID3D11PartnerDevice is an internal D3D11 function only available in late beta builds of Windows 8.");
		// If we don't get it, we will skip guard rect test cases
	}
#endif

	// If we got here, we passed
	tr = RESULT_PASS;
	assert(m_pDevice1);
	assert(m_pEffectiveContext1);

	return tr;
}

////

TEST_RESULT CDiscardTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_FAIL;
	HRESULT hr = E_FAIL;
	std::map< D3D_RESOURCE_SCENARIO, D3D11_FORMAT_SUPPORT > resTypes;

	// Check to see if we need to skip
	// Note: These should be kept to as few as possible
#if USE_GUARD_RECT_SCENARIO
	if( m_bTestUsesGuardRect )
	{
		const bool bUseGuardRect = m_guardRectScenario != eNULLRectList;
		if( bUseGuardRect )
		{
			// Then we are testing guard rects...so we must have a partner device
			if( m_pPartnerDevice == nullptr )
			{
				WriteToLog( _T("GuardRects are being tested, but we couldn't obtain a partner device. Skipping test case.") );
				tr = RESULT_SKIP;
				goto done;
			}

			// Begin GuardRect support
			HRESULT hr = m_pPartnerDevice->BeginGuardRectangleSupport();
			if( FAILED(hr) )
			{
				LogError( __FILEW__, __LINE__, L"BeginGuardRectangleSupport() failed.", hr, ERRORCODE_TYPE_HRESULT );
				goto done;
			}
			m_bInGuardRectangleSupport = true;
		}
	}
#endif

	if( D3D11_USAGE_DYNAMIC == m_Usage )
	{
		// Dynamic resource must be SRVs. Note that since the test cycles on all bind flags it is OK to skip.
		if( !(D3D11_BIND_SHADER_RESOURCE & m_BindFlags) )
		{
			WriteToLog( _T("Dynamic resources must be SRVs.") );
			tr = RESULT_SKIP;
			goto done;
		}

		// Dynamic resources cannot be bound to certain parts of the pipeline
		if( (D3D11_BIND_STREAM_OUTPUT & m_BindFlags) ||
			(D3D11_BIND_RENDER_TARGET & m_BindFlags) ||
			(D3D11_BIND_DEPTH_STENCIL & m_BindFlags) ||
			(D3D11_BIND_UNORDERED_ACCESS & m_BindFlags) )
		{
			WriteToLog( _T("Dynamic resources must be SRVs, but cannot be Stream output, RT, DS, or UAV") );
			tr = RESULT_SKIP;
			goto done;
		}
	}

	// R32B32G32_* cannot be bound as an UnorderedAccessView. Note that since the test cycles on all bind flags it is OK to skip.
	if( DXGI_FORMAT_R32G32B32_UINT == m_SrcFormat && 
		(D3D11_BIND_UNORDERED_ACCESS & m_BindFlags) )
	{
		WriteToLog( _T("R32B32G32_* formats cannot be bound as an UnorderedAccessView.") );
		tr = RESULT_SKIP;
		goto done;
	}

	// CB's must be a multiple of 16. Note that since the test cycles on all bind flags it is OK to skip.
	if( D3D_RESOURCE_SCENARIO_BUFFER == m_ResourceScenario  &&
		(D3D11_BIND_CONSTANT_BUFFER & m_BindFlags) &&
		m_uWidth % 16 != 0 )
	{
		WriteToLog( _T("Constant buffers must have a width that is a multiple of 16 bytes.") );
		tr = RESULT_SKIP;
		goto done;
	}

	// The resource doesn't support multiple mip levels. Note that since the test cycles on all dimensions it is OK to skip.
	if( 1 == m_uWidth &&
		1 == m_uHeight &&
		1 == m_uDepth &&
		m_uMipLevels > 1 )
	{
		WriteToLog( _T("The resource doesn't support multiple mip levels.") );
		tr = RESULT_SKIP;
		goto done;
	}

	if( CD3D10FormatHelper_Test::YUV( m_SrcFormat ) )
	{
		// Video formats need to check that the actual resource is aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( m_SrcFormat );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( m_SrcFormat );

		// Check the box alignment and resource alignment
		switch( m_ResourceScenario )
		{
		default:
			WriteToLog( _T("Invalid m_ResourceScenario = %s (%u)."), ToString(m_ResourceScenario).c_str(), m_ResourceScenario );
			return RESULT_FAIL;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			// No box depth alignment checks
			// fall-through
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			if( (m_uHeight % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's height (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_uHeight, ToString(m_SrcFormat).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			// fall-through
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			if( (m_uWidth % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's width (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_uWidth, ToString(m_SrcFormat).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			break;
		}
	}

	// Check test case restrictions due to feature level
	if( m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		if( D3D11_BIND_UNORDERED_ACCESS & m_BindFlags )
		{
			if( D3D_RESOURCE_SCENARIO_BUFFER == m_ResourceScenario )
			{
				// Check that the current adapter supports CS4_X
				D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions;
				if( FAILED( hr = m_pDevice1->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions) ) ) )
				{
					LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed for D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFeatureSupport );
					goto done;
				}

				if( !hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
				{
					// The adapter doesn't support compute on 10 (CS4_X)...
					WriteToLog( _T("The installed adapter doesn't support CS4_X.") );
					tr = RESULT_SKIP;
					goto done;
				}

				// If CS4_X is supported, we may need to change the view format to raw
				m_SrcFormat = DXGI_FORMAT_R32_TYPELESS;
				WriteToLog( _T("Changing UAV format to %s because the test is FL10.X..."), ToString(m_SrcFormat).c_str() );
				
				// If < FL11.0 we need to skip if there is an offset because the buffer width isn't great enough
				if( BufferViewHasElementOffset() )
				{
					WriteToLog( _T("The current UAV buffer view indicates it has an element offset. This is skipped because the buffer size isn't bigger than D3D11_CS_4_X_RAW_UAV_BYTE_ALIGNMENT (%d byte(s))"), D3D11_CS_4_X_RAW_UAV_BYTE_ALIGNMENT );
					tr = RESULT_SKIP;
					goto done;
				}
			}
			else
			{
				WriteToLog( _T("Only buffers can be UAVs for feature levels < 11.0.") );
				tr = RESULT_SKIP;
				goto done;
			}
		}
	}

	if( m_pDevice1->GetFeatureLevel() <= D3D_FEATURE_LEVEL_10_0 )
	{
		// CopyResource doesn't work on FL10.0 for Depth-Stencil resources or multisample resources
		// The extra work required to get this extra coverage just on FL10.0 (Note FL10.1+ work fine) is not worth it because the risk of bugs here is very low.

		if( D3D11_BIND_DEPTH_STENCIL & m_BindFlags )
		{
			WriteToLog( _T("CopyResource() will fail because this is a Depth-Stencil resource on %s."), ToString(m_pDevice1->GetFeatureLevel()).c_str() );
			tr = RESULT_SKIP;
			goto done;
		}

		if( ( D3D_RESOURCE_SCENARIO_TEXTURE2D_MS == m_ResourceScenario ||
			D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY == m_ResourceScenario) &&
			m_uSampleCount > 1 )
		{
			WriteToLog( _T("CopyResource() will fail because this is a Multisample resource on %s."), ToString(m_pDevice1->GetFeatureLevel()).c_str() );
			tr = RESULT_SKIP;
			goto done;
		}
	}

	// Check 10L9 restrictions
	if( m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
	{
		// Feature levels 9_* support only BIND_VERTEX_BUFFER, BIND_INDEX_BUFFER or DDI_BIND_CONSTANT_BUFFER for buffers
		if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER &&
			(m_BindFlags != D3D11_BIND_CONSTANT_BUFFER && m_BindFlags != D3D11_BIND_VERTEX_BUFFER && m_BindFlags != D3D11_BIND_INDEX_BUFFER) )
		{
			WriteToLog( _T("Feature levels 9_* support only BIND_VERTEX_BUFFER, BIND_INDEX_BUFFER or DDI_BIND_CONSTANT_BUFFER for buffers.") );
			tr = RESULT_SKIP;
			goto done;
		}

		// 10L9 doesnt support resource arrays
		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
			{
				WriteToLog( _T("Feature levels 9_* don't support arrays.") );
				tr = RESULT_SKIP;
				goto done;
			}
			break;
		}

		// 10L9 doesnt support Texture1D
		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
			{
				WriteToLog( _T("Feature levels 9_* don't support Texture 1D's.") );
				tr = RESULT_SKIP;
				goto done;
			}
			break;
		}

		// 10L9 doesnt support Texture3D Render Target
		switch( m_ResourceScenario )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			{
				if( D3D11_BIND_RENDER_TARGET & m_BindFlags )
				{
					WriteToLog( _T("Feature levels 9_* don't support Texture3D Render Targets.") );
					tr = RESULT_SKIP;
					goto done;
				}
			}
			break;
		}

		// 10L9 has restrictions on the views allowed
		if( D3D11_BIND_SHADER_RESOURCE & m_BindFlags )
		{
			// CreateShaderResourceView: MostDetailedMip plus MipLevels must include lowest LOD (smallest subresource)
			if( !SRViewIncludesLowestLOD() )
			{
				WriteToLog( _T("Feature levels 9_* require that SRVs include the lowest LOD (smallest subresource).") );
				tr = RESULT_SKIP;
				goto done;
			}
		}

		// 10L9 has restrictions on CopyResource on resources that ONLY have the SR bind flag
		// Note: the == is intentional.
		if( D3D11_BIND_SHADER_RESOURCE == m_BindFlags )
		{
			WriteToLog( _T("CopyResource: Any resource that has only D3D10_BIND_SHADER_RESOURCE cannot be copied from GPU-accessible memory to CPU-accessible memory. \
				To enable such copies for this resource, add D3D10_BIND_RENDER_TARGET or D3D10_BIND_DEPTH_STENCIL") );
			tr = RESULT_SKIP;
			goto done;
		}
	}

	// Check for format support if we will have a view, or we are not using a buffer
	if( D3D_RESOURCE_SCENARIO_BUFFER != m_ResourceScenario ||
		m_bTestUsesView )
	{
		UINT support = 0;
		if( FAILED( hr = m_pDevice1->CheckFormatSupport( m_SrcFormat, &support ) ) )
		{
			WriteToLog( _T("Checking resource format support for %s."), ToString(m_SrcFormat).c_str() );
			LogError( __FILEW__, __LINE__, L"CheckFormatSupport() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFormatSupport );
			// Returns S_OK if successful; otherwise, 
			// returns E_INVALIDARG if the Format parameter is NULL, or returns E_FAIL if the described format does not exist.
			tr = RESULT_SKIP;
			goto done;
		}

		resTypes[ D3D_RESOURCE_SCENARIO_BUFFER ] = D3D11_FORMAT_SUPPORT_BUFFER;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_MS ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;
		resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURECUBE;
		if( (support & resTypes[ m_ResourceScenario ]) == 0 )
		{
			WriteToLog( _T("The format is not supported on the current scenario.") );
			tr = RESULT_SKIP;
			goto done;
		}

		if( D3D11_BIND_RENDER_TARGET & m_BindFlags )
		{
			if( (support & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == 0 )
			{
				WriteToLog( _T("The format does not support render target views.") );
				tr = RESULT_SKIP;
				goto done;
			}
		}

		if ( ( 0 == ( support & D3D11_FORMAT_SUPPORT_MIP ) ) && m_uMipLevels > 1 )
		{
			WriteToLog( _T("Skipping mip > 0 because format doesn't support mipping") );
			tr = RESULT_SKIP;
			goto done;
		}
	}

	// Check for Multisample support
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		{
			UINT qualityLevels = 0;
			if( FAILED( hr = m_pDevice1->CheckMultisampleQualityLevels( m_SrcFormat, m_uSampleCount, &qualityLevels ) ) )
			{
				WriteToLog( _T("Checking multisample quality levels for %s."), ToString(m_SrcFormat).c_str() );
				LogError( __FILEW__, __LINE__, L"CheckMultisampleQualityLevels() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckMultisampleQualityLevels );
				goto done;
			}

			// If this method returns 0, the format and sample count combination is not supported for the installed adapter
			if( 0 == qualityLevels )
			{
				WriteToLog( _T("The format does not support the specified sample count.") );
				tr = RESULT_SKIP;
				goto done;
			}
		}
		break;
	}

	// Create the initial data
	BeginSection( L"Create subresource data." );
	if( !CreateSubresourceData() )
	{
		WriteToLog( _T("CreateSubresourceData() failed.") );	// Note: Not LogError(), LogError should be at the point of failure.
		EndSection();
		goto done;
	}
	EndSection();

	// Create the resources used for the current testcase
	BeginSection( L"Create resources." );

	tr = CreateResources();
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T("CreateResources() failed.") );
		EndSection();
		goto done;
	}
	EndSection();

	if( m_bTestUsesView )
	{
		BeginSection( L"Create views." );
		if( !CreateViews() )
		{
			WriteToLog( _T("CreateViews() failed.") );
			EndSection();
			goto done;
		}
		EndSection();
	}

	// If we got here, we passed
	tr = RESULT_PASS;

done:
	return tr;
}

////

TEST_RESULT CDiscardTest::ExecuteTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = RESULT_FAIL;
	std::vector<UINT> subresourcesToVerify;
	std::vector<UINT> allSubresources;

	// Update the resource
	// This is where the test should write data to each subresource
	// This is the data that will be verified after the Discard*() call
	BeginSection( L"Updating subresource data in the test's resources." );
	if( !UpdateSubresourceData() )
	{
		WriteToLog( _T("UpdateSubResourceData() failed.") );
		EndSection();
		goto done;
	}
	EndSection();

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Verify subresource data
	// At this point all subresource data should be valid for each texture
	BeginSection( L"Verify subresource data before Discard*()." );
	WriteToLog( _T("All subresources should be valid.") );
	if( !GetAllSubresources( allSubresources, m_pResourceToDiscard ) )
	{
		WriteToLog( _T("GetAllSubresources() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	WriteToLog( _T("Checking m_pResourceToDiscard...") );
	if( !VerifySubresourceData( m_pResourceToDiscard, allSubresources ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	WriteToLog( _T("Checking m_pDummyResourceNoDiscard...") );
	if( !VerifySubresourceData( m_pDummyResourceNoDiscard, allSubresources ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pDummyResourceNoDiscard failed.") );
		EndSection();
		goto done;
	}
	EndSection(false);
	WriteToLog( _T("All subresources in m_pResourceToDiscard and m_pDummyResourceNoDiscard are valid before Discard*().") );


	// Discard
	BeginSection( L"Discard resources/views." );
	if( !Discard() )
	{
		WriteToLog( _T("Discard() failed.") );
		EndSection();
		goto done;
	}
	EndSection();

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Verify subresource data
	BeginSection( L"Verify subresource data after Discard*()." );
	if( !VerifyResults() )
	{
		WriteToLog( _T("VerifyResults() failed.") );
		EndSection();
		goto done;
	}
	EndSection();

	// If we got here, we passed
	tr = RESULT_PASS;

done:
	return tr;
}

////

void CDiscardTest::CleanupTestCase()
{
#if USE_GUARD_RECT_SCENARIO
	// Stop GuardRect support
	if( m_bInGuardRectangleSupport )
	{
		m_pPartnerDevice->EndGuardRectangleSupport();
	}
	m_bInGuardRectangleSupport = false;
#endif

	// Cleanup the allocated system memory
	if( m_pSubresourceData )
	{
		delete [] m_pSubresourceData;
		m_pSubresourceData = NULL;
	}

	SAFE_RELEASE( m_pResourceToDiscard );
	SAFE_RELEASE( m_pDummyResourceNoDiscard );
	SAFE_RELEASE( m_pStagingResource );

	// Call the parent cleanup
	CResourceAccessTest::CleanupTestCase();
}

////

void CDiscardTest::Cleanup()
{
	// Call the parent cleanup
	CResourceAccessTest::Cleanup();
}

////

void CDiscardTest::GenerateCommonResourceParameterFactors()
{
	// Setup the resource dimension sets
	// Cycle over different resource dimensions
	m_rResourceScenarioBuffer = AddParameter<D3D_RESOURCE_SCENARIO>( _T( g_resourceScenarioStr ), &m_ResourceScenario, D3D_RESOURCE_SCENARIO_BUFFER );
	m_rResourceScenarioTex1D = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE1D );
	m_rResourceScenarioTex1DArray = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY );
	m_rResourceScenarioTex2D = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE2D );
	m_rResourceScenarioTex2DArray = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	m_rResourceScenarioTex2DMS = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE2D_MS );
	m_rResourceScenarioTex2DMSArray = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY );
	m_rResourceScenarioTex3D = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE3D );
	m_rResourceScenarioTexCube = AddParameterValue( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURECUBE ); 
	SetParameterDefaultValue<D3D_RESOURCE_SCENARIO>( _T( g_resourceScenarioStr ), D3D_RESOURCE_SCENARIO_TEXTURE2D );

	// Setup the format sets
	CUserValueSet<DXGI_FORMAT> *pFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		pFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );			// 32 bit
		pFormatValues->AddValue( DXGI_FORMAT_R8G8_UNORM );				// 16 bit
		pFormatValues->AddValue( DXGI_FORMAT_A8_UNORM );				// 8 bit (more important swapbuffer format)
		pFormatValues->AddValue( DXGI_FORMAT_R8_UNORM );				// 8 bit (but needed for emulating L8)
	}
	else
	{
		pFormatValues->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );		// 128 bit
		pFormatValues->AddValue( DXGI_FORMAT_R32G32B32_UINT );			// 96 bit
		pFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );		// 64 bit
		pFormatValues->AddValue( DXGI_FORMAT_R32_SINT );				// 32 bit
		pFormatValues->AddValue( DXGI_FORMAT_R8G8_SNORM );				// 16 bit
		pFormatValues->AddValue( DXGI_FORMAT_R8_UNORM );				// 8 bit
	}
	m_rResourceFormats = AddParameter<DXGI_FORMAT>( _T( g_resourceFormatStr ), &m_SrcFormat, pFormatValues );
	SetParameterDefaultValue<DXGI_FORMAT>( _T( g_resourceFormatStr ), DXGI_FORMAT_UNKNOWN );

	CUserValueSet<DXGI_FORMAT> *pDFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pDFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT );
	pDFormatValues->AddValue( DXGI_FORMAT_D16_UNORM );
	m_rResourceDepthFormats = AddParameterValueSet( _T( g_resourceFormatStr ), pDFormatValues );

	CUserValueSet<DXGI_FORMAT> *pSFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pSFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	pSFormatValues->AddValue( DXGI_FORMAT_D24_UNORM_S8_UINT );
	m_rResourceStencilFormats = AddParameterValueSet( _T( g_resourceFormatStr ), pSFormatValues );

	m_rResourceDSFormats = m_rResourceDepthFormats + m_rResourceStencilFormats;

	// BC formats are only valid for Tex2D, Tex3D, and TexCube
	CUserValueSet<DXGI_FORMAT> *pBCFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC1_UNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC2_UNORM_SRGB );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC3_UNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC4_SNORM );
	pBCFormatValues->AddValue( DXGI_FORMAT_BC5_SNORM );
	m_rResourceBCFormats = AddParameterValueSet( _T( g_resourceFormatStr ), pBCFormatValues );

	CUserValueSet<DXGI_FORMAT> *pVideoFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pVideoFormatValues->AddValue( DXGI_FORMAT_NV12 );	// Planar
	pVideoFormatValues->AddValue( DXGI_FORMAT_YUY2 );	// Subsampled
	pVideoFormatValues->AddValue( DXGI_FORMAT_AYUV );	// Regular
	m_rResourceVideoFormats = AddParameterValueSet( _T( g_resourceFormatStr ), pVideoFormatValues );

	// Setup the resource usage sets
	m_rResourceUsageDefault = AddParameter<D3D11_USAGE>( _T( g_resourceUsageStr ), &m_Usage, D3D11_USAGE_DEFAULT );
	m_rResourceUsageDynamic = AddParameterValue( _T( g_resourceUsageStr ), D3D11_USAGE_DYNAMIC );
	SetParameterDefaultValue<D3D11_USAGE>( _T( g_resourceUsageStr ), D3D11_USAGE_DEFAULT );

	// Setup the bing flags sets
	// Cycle over different bind flags
	m_rResourceBindFlagDS = AddParameter<D3D11_BIND_FLAG>( _T( g_resourceBindFlagStr ), &m_BindFlags, D3D11_BIND_DEPTH_STENCIL );
	m_rResourceBindFlagSR = AddParameterValue( _T( g_resourceBindFlagStr ), D3D11_BIND_SHADER_RESOURCE );
	m_rResourceBindFlagSRRT = AddParameterValue( _T( g_resourceBindFlagStr ), (D3D11_BIND_FLAG)(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) );
	m_rResourceBindFlagRT = AddParameterValue( _T( g_resourceBindFlagStr ), D3D11_BIND_RENDER_TARGET );
	m_rResourceBindFlagUA =	AddParameterValue( _T( g_resourceBindFlagStr ), D3D11_BIND_UNORDERED_ACCESS );
	m_rResourceBindFlagCB =	AddParameterValue( _T( g_resourceBindFlagStr ), D3D11_BIND_CONSTANT_BUFFER );
	SetParameterDefaultValue<D3D11_BIND_FLAG>( _T( g_resourceBindFlagStr ), D3D11_BIND_SHADER_RESOURCE );

	// Setup the cpu access flags
	CUserValueSet<UINT> *pCPUAccessFlags = new CUserValueSet<UINT>();
	pCPUAccessFlags->AddValue((UINT)(D3D11_CPU_ACCESS_FLAG)0);
	pCPUAccessFlags->AddValue((UINT)D3D11_CPU_ACCESS_WRITE);
	pCPUAccessFlags->AddValue((UINT)D3D11_CPU_ACCESS_READ);
	pCPUAccessFlags->AddValue((UINT)(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE));
	m_rCPUAccessFlags = AddParameter<UINT>( _T( g_cpuAccessStr ), &m_CPUAccessFlag, pCPUAccessFlags );
	m_rCPUAccessFlagNone = AddParameterValue( _T( g_cpuAccessStr ), (UINT)0 );
	m_rCPUAccessFlagWrite = AddParameterValue( _T( g_cpuAccessStr ), (UINT)D3D11_CPU_ACCESS_WRITE );
	SetParameterDefaultValue<UINT>( _T( g_cpuAccessStr ), (UINT)0 );

	// Setup the resource array size sets
	CUserValueSet<UINT>	*pResourceArraySizeValues = new CUserValueSet<UINT>();
	pResourceArraySizeValues->AddValue(1);
	pResourceArraySizeValues->AddValue(2);
	pResourceArraySizeValues->AddValue(6);
	m_rResourceArraySize = AddParameter<UINT>( _T( g_resourceArraySizeStr ), &m_uArraySize, pResourceArraySizeValues );
	m_rResourceArraySize_1 = AddParameterValue( _T( g_resourceArraySizeStr ), (UINT)1 );
	m_rResourceArraySize_2 = AddParameterValue( _T( g_resourceArraySizeStr ), (UINT)2 );
	m_rResourceArraySize_6 = AddParameterValue( _T( g_resourceArraySizeStr ), (UINT)6 );
	SetParameterDefaultValue<UINT>( _T( g_resourceArraySizeStr ), 1 );

	// Setup the mip levels sets
	CUserValueSet<UINT>	*pNumMipLevelsValues = new CUserValueSet<UINT>();
	//pNumMipLevelsValues->AddValue(0);	// Generates the max set of subresources
	pNumMipLevelsValues->AddValue(1);
	pNumMipLevelsValues->AddValue(3);
	m_rResourceNumMipLevels = AddParameter<UINT>( _T( g_resourceNumMipLevelsStr ), &m_uMipLevels, pNumMipLevelsValues );
	m_rResourceNumMipLevels_0 = AddParameterValue( _T( g_resourceNumMipLevelsStr ), (UINT)0 );
	m_rResourceNumMipLevels_1 = AddParameterValue( _T( g_resourceNumMipLevelsStr ), (UINT)1 );
	m_rResourceNumMipLevels_2 = AddParameterValue( _T( g_resourceNumMipLevelsStr ), (UINT)2 );
	m_rResourceNumMipLevels_3 = AddParameterValue( _T( g_resourceNumMipLevelsStr ), (UINT)3 );
	SetParameterDefaultValue<UINT>( _T( g_resourceNumMipLevelsStr ), 1 );

	// Setup the resource dimensions
	m_rResourceWidth_1 = AddParameter<UINT32>( _T( g_resourceWidthStr ), &m_uWidth, (UINT32)1 );
	m_rResourceWidth_16 = AddParameterValue( _T( g_resourceWidthStr ), (UINT32)16 );
	m_rResourceWidth_64 = AddParameterValue( _T( g_resourceWidthStr ), (UINT32)64 );
	m_rResourceWidth = m_rResourceWidth_1 + m_rResourceWidth_16;
	SetParameterDefaultValue<UINT>( _T( g_resourceWidthStr ), 1 );

	m_rResourceHeight_1 = AddParameter<UINT32>( _T( g_resourceHeightStr ), &m_uHeight, (UINT32)1 );
	m_rResourceHeight_16 = AddParameterValue( _T( g_resourceHeightStr ), (UINT32)16 );
	m_rResourceHeight_64 = AddParameterValue( _T( g_resourceHeightStr ), (UINT32)64 );
	m_rResourceHeight = m_rResourceHeight_1 + m_rResourceHeight_16;
	SetParameterDefaultValue<UINT>( _T( g_resourceHeightStr ), 1 );
	
	m_rResourceDepth_1 = AddParameter<UINT32>( _T( g_resourceDepthStr ), &m_uDepth, (UINT32)1 );
	m_rResourceDepth_16 = AddParameterValue( _T( g_resourceDepthStr ), (UINT32)16 );
	m_rResourceDepth_64 = AddParameterValue( _T( g_resourceDepthStr ), (UINT32)64 );
	m_rResourceDepth = m_rResourceDepth_1 + m_rResourceDepth_16;
	SetParameterDefaultValue<UINT>( _T( g_resourceDepthStr ), 1 );

	// Setup the multisample sets
	m_rResourceSampleCount_1 = AddParameter<UINT>( _T( g_resourceSampleCountStr ), &m_uSampleCount, (UINT)1 );
	m_rResourceSampleCount_2 = AddParameterValue( _T( g_resourceSampleCountStr ), (UINT)2 );
	SetParameterDefaultValue<UINT>( _T( g_resourceSampleCountStr ), (UINT)1 );

	m_rResourceSampleQuality_0 = AddParameter<UINT>( _T( g_resourceSampleQualityStr ), &m_uSampleQuality, (UINT)0 );
	SetParameterDefaultValue<UINT>( _T( g_resourceSampleQualityStr ), (UINT)0 );

	m_rResourceMiscFlag_TexCube = AddParameter<D3D11_RESOURCE_MISC_FLAG>( _T( g_resourceMiscFlagStr ), &m_ResourceMiscFlag, D3D11_RESOURCE_MISC_TEXTURECUBE );
	SetParameterDefaultValue<D3D11_RESOURCE_MISC_FLAG>( _T( g_resourceMiscFlagStr ), (D3D11_RESOURCE_MISC_FLAG)0 );

	// Setup the double discard set
	// Cycle over if the discarded resource should be discarded twice, one immediatley after the other
	CUserValueSet<bool>	*pDoubleDiscardValues = new CUserValueSet<bool>();
	pDoubleDiscardValues->AddValue(true);
	pDoubleDiscardValues->AddValue(false);
	m_rDoubleDiscard = AddParameter<bool>( _T( g_doubleDiscardStr ), &m_bDoubleDiscard, pDoubleDiscardValues );
	SetParameterDefaultValue<bool>( _T( g_doubleDiscardStr ), false );

	// Setup the data initalization
	m_rInitResourceData_True = AddParameter<bool>( _T( g_resourceInitWithData ), &m_bInitResourceData, true );
	m_rInitResourceData_False = AddParameterValue( _T( g_resourceInitWithData ), false );
	m_rInitResourceData = m_rInitResourceData_True + m_rInitResourceData_False;
	SetParameterDefaultValue<bool>( _T( g_resourceInitWithData ), false );
}

////

void CDiscardTest::GenerateCommonResourceFactorCombinations()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	GenerateCommonResourceParameterFactors();

	//
	// Create pairwise sets for common resource combinations
	//

	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		// For FL9.X, Any resource that has only D3D10_BIND_SHADER_RESOURCE cannot be copied from GPU-accessible memory to CPU-accessible memory. 
		// To enable such copies for this resource, add D3D10_BIND_RENDER_TARGET or D3D10_BIND_DEPTH_STENCIL
		// Dynamic resources cannot have SRV and RTV at the same time, thus we repeat SRV and then SRV + RTV.
		m_rBindFlags_SRRTUA					= m_rResourceBindFlagSR + m_rResourceBindFlagSRRT + m_rResourceBindFlagRT + m_rResourceBindFlagUA;	// A buffer cannot be used as depth stencil
		m_rBindFlags_SRRT					= m_rResourceBindFlagSR + m_rResourceBindFlagSRRT + m_rResourceBindFlagRT;	// Multisampling is not supported with the D3D11_BIND_UNORDERED_ACCESS BindFlag
	}
	else
	{
		m_rBindFlags_SRRTUA					= m_rResourceBindFlagSR + m_rResourceBindFlagRT + m_rResourceBindFlagUA;	// A buffer cannot be used as depth stencil
		m_rBindFlags_SRRT					= m_rResourceBindFlagSR + m_rResourceBindFlagRT;	// Multisampling is not supported with the D3D11_BIND_UNORDERED_ACCESS BindFlag
	}

	testfactor::XFACTOR defaultUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlags, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceArraySize_1, 1, NULL, 0},	// No array
		{m_rResourceNumMipLevels, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceArraySize, 1, NULL, 0},
		{m_rResourceNumMipLevels, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceArraySize, 1, NULL, 0},
		{m_rResourceNumMipLevels_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceArraySize_1, 1, NULL, 0},		// No array
		{m_rResourceNumMipLevels_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsCubePairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceArraySize_6, 1, NULL, 0},
		{m_rResourceNumMipLevels, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		{m_rResourceArraySize_1, 1, NULL, 0},		// Dynamic resources must have 1 array slice
		{m_rResourceNumMipLevels_1, 1, NULL, 0},	// Dynamic resources must have 1 mip
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	m_rDefaultUsageCPU_Buf			= NewCrossFactor(defaultUsageBufPairwiseTable, _countof(defaultUsageBufPairwiseTable), _T("defaultUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDefaultUsageMipCPU_Tex		= NewCrossFactor(defaultUsageMipsTexPairwiseTable, _countof(defaultUsageMipsTexPairwiseTable), _T("defaultUsageMipsTexPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayMipCPU_Tex	= NewCrossFactor(defaultUsageMipsTexArrayPairwiseTable, _countof(defaultUsageMipsTexArrayPairwiseTable), _T("defaultUsageMipsTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayCPU_Tex		= NewCrossFactor(defaultUsageTexArrayPairwiseTable, _countof(defaultUsageTexArrayPairwiseTable), _T("defaultUsageTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageCPU_Tex			= NewCrossFactor(defaultUsageTexPairwiseTable, _countof(defaultUsageTexPairwiseTable), _T("defaultUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDefaultUsageMipCPU_Cube		= NewCrossFactor(defaultUsageMipsCubePairwiseTable, _countof(defaultUsageMipsCubePairwiseTable), _T("defaultUsageMipsCubePairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDynamicUsageCPU_Buf			= NewCrossFactor(dynamicUsageBufPairwiseTable, _countof(dynamicUsageBufPairwiseTable), _T("dynamicUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDynamicUsageArrayMipCPU_Tex	= NewCrossFactor(dynamicUsageTexPairwiseTable, _countof(dynamicUsageTexPairwiseTable), _T("dynamicUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));
}

////

bool CDiscardTest::GetArraySizeAndMipLevelsFromResource( ID3D11Resource *pResource, UINT &arraySize, UINT &mipLevels ) const
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	D3D11_RESOURCE_DIMENSION resourceDimension;

	// Clear the values and performs some checks
	arraySize = 0;
	mipLevels = 0;

	if( !pResource )
	{
		LogError( __FILEW__, __LINE__, L"The resource is NULL.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	// Get the resource's dimensions
	pResource->GetType( &resourceDimension );
	switch( resourceDimension )
	{
	default:
	case D3D11_RESOURCE_DIMENSION_UNKNOWN:
		assert( !"Unknown Dimension" );
		break;

	case D3D11_RESOURCE_DIMENSION_BUFFER:
		{
			// Nothing to do here, buffer's cannot have arrays or mips
			// Buffers only have 1 subresource
			arraySize = 1;
			mipLevels = 1;
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		{
			// QI for the correct interface and get the descs
			ATL::CComPtr<ID3D11Texture1D> spTex1D;
			if( FAILED( hr = pResource->QueryInterface( &spTex1D ) ) )
			{
				LogError( __FILEW__, __LINE__, L"QueryInterface for ID3D11Texture1D failed.", hr, ERRORCODE_TYPE_HRESULT, gFCIUnknownQueryInterface );
				goto done;
			}

			D3D11_TEXTURE1D_DESC tex1DDesc;
			spTex1D->GetDesc( &tex1DDesc );

			arraySize = tex1DDesc.ArraySize;
			mipLevels = tex1DDesc.MipLevels;
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			// QI for the correct interface and get the descs
			ATL::CComPtr<ID3D11Texture2D> spTex2D;
			if( FAILED( hr = pResource->QueryInterface( &spTex2D ) ) )
			{
				LogError( __FILEW__, __LINE__, L"QueryInterface for ID3D11Texture2D failed.", hr, ERRORCODE_TYPE_HRESULT, gFCIUnknownQueryInterface );
				goto done;
			}

			D3D11_TEXTURE2D_DESC tex2DDesc;
			spTex2D->GetDesc( &tex2DDesc );

			arraySize = tex2DDesc.ArraySize;
			mipLevels = tex2DDesc.MipLevels;
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			// QI for the correct interface and get the descs
			ATL::CComPtr<ID3D11Texture3D> spTex3D;
			if( FAILED( hr = pResource->QueryInterface( &spTex3D ) ) )
			{
				LogError( __FILEW__, __LINE__, L"QueryInterface for ID3D11Texture3D failed.", hr, ERRORCODE_TYPE_HRESULT, gFCIUnknownQueryInterface );
				goto done;
			}

			D3D11_TEXTURE3D_DESC tex3DDesc;
			spTex3D->GetDesc( &tex3DDesc );

			// Texture3D's cannot be arrays
			arraySize = 1;
			mipLevels = tex3DDesc.MipLevels;
		}
		break;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardTest::GetAllSubresources( std::vector<UINT> &subresources, ID3D11Resource *pResource ) const
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT numSubresourcesInResource = 0;
	UINT numArraySlicesInResource = 0;
	UINT numMiplevelsInResource = 0;

	// Clear the contents of the vector
	subresources.clear();
	assert( subresources.empty() );

	// Get the array size and number of mip levels in the resource
	if( !GetArraySizeAndMipLevelsFromResource( pResource, numArraySlicesInResource, numMiplevelsInResource ) )
	{
		WriteToLog( _T("GetArraySizeAndMipLevelsFromResource() failed.") );
		goto done;
	}

	// Calculate the number of subresources in the resource
	numSubresourcesInResource = numArraySlicesInResource * numMiplevelsInResource;

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < numArraySlicesInResource; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
		{
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, numMiplevelsInResource );
			subresources.push_back( currSubresource );
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

void CDiscardTest::SnapTexelToMipDimensions( POINT &texel, const UINT &mipWidth, const UINT &mipHeight ) const
{
	// Valid width range is [0, mipWidth - 1]:
	texel.x = min( texel.x, (LONG)(mipWidth - 1) );
	texel.x = max( texel.x, (LONG)(0) );

	// Valid height range is [0, mipHeight - 1]:
	texel.y = min( texel.y, (LONG)(mipHeight - 1) );
	texel.y = max( texel.y, (LONG)(0) );
}

////

bool CDiscardTest::CreateSubresourceData()
{
	// Local variables
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	m_ResourceBitPattern = 0x0B;
	WriteToLog( _T("Writing %#x as the bit pattern for each subresource."), m_ResourceBitPattern );

	// Check to make sure previous subresource data was cleaned up
	if( m_pSubresourceData )
	{
		LogError( __FILEW__, __LINE__, L"Subresource data was not cleaned up. The data (m_pSubresourceData) must be NULL here.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	// Create the subresource data
	// Allocate system memory for the biggest miplevel, all subresources will share the same memory
	// This algorithm is extremely conservative to pad big enough for BC & planar cases
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	const UINT numElements = m_uWidth * m_uHeight * m_uDepth;
	const UINT numBytesPerElement = bResourceIsBuffer && !m_bTestUsesView ? 1 : CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;	// Buffers are given a byte size. If a view is used, the buffer is created with numElements * bytesPerElement
	const UINT numBytesNeeded = numElements * numBytesPerElement * 2; // YUV planars use up to double the conservative estimate 
	WriteToLog( _T("Resource has %d element(s), at %d byte(s) each. Allocating space for the biggest mip level which is %d byte(s)..."), numElements, numBytesPerElement, numBytesNeeded );
	WriteToLog( _T("All subresource data will share from the same system memory."), numElements, numBytesPerElement, numBytesNeeded );
	m_pSubresourceData = new BYTE[numBytesNeeded];
	memset( m_pSubresourceData, m_ResourceBitPattern, numBytesNeeded );

	// Clear existing data
	m_subresourceData.clear();

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				goto done;
			}

			// Create the subresource data and add it to the vector
			D3D11_SUBRESOURCE_DATA newSubresData;
			newSubresData.pSysMem = m_pSubresourceData; // The shared system memory
			newSubresData.SysMemPitch = numBytesPerElement * currMipWidth;
			newSubresData.SysMemSlicePitch = numBytesPerElement * currMipWidth * currMipHeight;
			m_subresourceData.push_back(newSubresData);

			// Double check that the subresource data was written to the correct place in the vector
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			if( (newSubresData.pSysMem != m_subresourceData[currSubresource].pSysMem) ||
				(newSubresData.SysMemPitch != m_subresourceData[currSubresource].SysMemPitch) ||
				(newSubresData.SysMemSlicePitch != m_subresourceData[currSubresource].SysMemSlicePitch) )
			{
				WriteToLog( _T("Current subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );
				WriteToLog( _T("Expected new subresource data to exist at index #%d"), currSubresource );
				LogError( __FILEW__, __LINE__, L"The new subresource data doesn't match the element at the expected index.", 0, ERRORCODE_TYPE_BOOL, gFCLogicError );
				goto done;
			}
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

TEST_RESULT CDiscardTest::CreateResources()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	const D3D11_SUBRESOURCE_DATA* pInitData = m_bInitResourceData ? &(m_subresourceData[0]) : NULL;

#if USE_GUARD_RECT_SCENARIO
	// Determine if we need to use the private GuardRect API to create the texture
	bool bUseGuardRect = false;
	if( m_bTestUsesGuardRect )
	{
		if( m_guardRectScenario != eNULLRectList )
		{
			bUseGuardRect = true;
		}
	}
#endif

	// The resources should be cleaned up
	assert( NULL == m_pResourceToDiscard );
	assert( NULL == m_pDummyResourceNoDiscard );
	if( m_pResourceToDiscard ||
		m_pDummyResourceNoDiscard )
	{
		LogError( __FILEW__, __LINE__, L"Resources were not cleaned up. Resources must be NULL here.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	// The subresource data should contain the same number of resources to be used
	const UINT numSubresourcesInResource = m_uArraySize * m_uMipLevels;
	if( numSubresourcesInResource != m_subresourceData.size() )
	{
		wchar_t errStr[WCHAR_STR_SIZE];
		swprintf_s( errStr, L"The resource contains %d subresource(s), but the subresource data vector's size is %d.", numSubresourcesInResource, m_subresourceData.size());
		LogError( __FILEW__, __LINE__, errStr, false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	/*if( (m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0) && 
		((m_BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0) )
	{
		
		D3D11_BIND_FLAG oldBindFlags = m_BindFlags;
		m_BindFlags = (D3D11_BIND_FLAG)(m_BindFlags | D3D11_BIND_RENDER_TARGET);
		WriteToLog( _T("Updating bind flags so CopyResource works. Old Bind flags = %s. New Bind flags = %s."), ToString(oldBindFlags).c_str(), ToString(m_BindFlags).c_str() );
	}*/
	
	// Create the new resources
	switch( m_ResourceScenario )
	{
	default:
		assert(!"Unknown resource scenario");
		break;

	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			// Local variables
			const bool bNeedsCS4XMiscFlag = (m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0) && (D3D11_BIND_UNORDERED_ACCESS == m_BindFlags);
			D3D11_BUFFER_DESC bufDesc;
			bufDesc.BindFlags = m_BindFlags;
			bufDesc.ByteWidth = m_bTestUsesView ? m_uWidth * CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8 : m_uWidth;
			bufDesc.CPUAccessFlags = m_CPUAccessFlag;
			bufDesc.MiscFlags = m_ResourceMiscFlag;
			bufDesc.MiscFlags |= bNeedsCS4XMiscFlag ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
			bufDesc.StructureByteStride = 0;
			bufDesc.Usage = m_Usage;

			if( !IsSupportedBufferScenario(&bufDesc) )
			{
				return RESULT_SKIP;
			}

			// Create the resource
			WriteToLog( _T("Creating buffer, m_pResourceToDiscard, with a width of %d byte(s)."), bufDesc.ByteWidth );
			if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, pInitData, (ID3D11Buffer**)(&m_pResourceToDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateBuffer m_pResourceToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the dummy resource
			WriteToLog( _T("Creating buffer, m_pDummyResourceNoDiscard, with a width of %d byte(s)."), bufDesc.ByteWidth );
			if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, pInitData, (ID3D11Buffer**)(&m_pDummyResourceNoDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateBuffer m_pDummyResourceNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the staging resource

			if (m_Usage == D3D11_USAGE_DEFAULT && m_CPUAccessFlag != 0)
			{
				// We are testing the Map Default functionality
				// Since we didn't skip above, we know this is supported on this part
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
				bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
				WriteToLog( "Using a mappable DEFAULT buffer instead of a STAGING buffer." );
			}
			else
			{
				bufDesc.Usage = D3D11_USAGE_STAGING;
				bufDesc.BindFlags = 0;
			}

			bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			bufDesc.MiscFlags = m_ResourceMiscFlag;	// Clear out any additional flags that might have been set
			if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, pInitData, (ID3D11Buffer**)(&m_pStagingResource) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateBuffer m_pStagingResource.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		{
			D3D11_TEXTURE1D_DESC texDesc;
			texDesc.ArraySize = m_uArraySize;
			texDesc.BindFlags = m_BindFlags;
			texDesc.CPUAccessFlags = m_CPUAccessFlag;
			texDesc.Format = m_SrcFormat;
			texDesc.MipLevels = m_uMipLevels;
			texDesc.MiscFlags = m_ResourceMiscFlag;
			texDesc.Usage = m_Usage;
			texDesc.Width = m_uWidth;

			// Create the resource
			if( FAILED( hr = m_pDevice1->CreateTexture1D( &texDesc, pInitData, (ID3D11Texture1D**)(&m_pResourceToDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture1D m_pResourceToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the dummy resource
			if( FAILED( hr = m_pDevice1->CreateTexture1D( &texDesc, pInitData, (ID3D11Texture1D**)(&m_pDummyResourceNoDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture1D m_pDummyResourceNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the staging resource
			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			texDesc.Usage = D3D11_USAGE_STAGING;
			if( FAILED( hr = m_pDevice1->CreateTexture1D( &texDesc, pInitData, (ID3D11Texture1D**)(&m_pStagingResource) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture1D m_pStagingResource.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			D3D11_TEXTURE2D_DESC texDesc;
			texDesc.ArraySize = m_uArraySize;
			texDesc.BindFlags = m_BindFlags;
			texDesc.CPUAccessFlags = m_CPUAccessFlag;
			texDesc.Format = m_SrcFormat;
			texDesc.Height = m_uHeight;
			texDesc.MipLevels = m_uMipLevels;
			texDesc.MiscFlags = m_ResourceMiscFlag;
			texDesc.SampleDesc.Count = m_uSampleCount;
			texDesc.SampleDesc.Quality = m_uSampleQuality;
			texDesc.Usage = m_Usage;
			texDesc.Width = m_uWidth;

#if USE_GUARD_RECT_SCENARIO			
			if( bUseGuardRect )
			{
				// Create the resource
				if( FAILED( hr = m_pPartnerDevice->CreateGuardableTexture2D( &texDesc, pInitData, (ID3D11Texture2D**)(&m_pResourceToDiscard) ) ) )
				{
					LogError( __FILEW__, __LINE__, L"Failed to CreateGuardableTexture2D m_pResourceToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
					goto done;
				}

				// Create the dummy resource
				if( FAILED( hr = m_pPartnerDevice->CreateGuardableTexture2D( &texDesc, pInitData, (ID3D11Texture2D**)(&m_pDummyResourceNoDiscard) ) ) )
				{
					LogError( __FILEW__, __LINE__, L"Failed to CreateGuardableTexture2D m_pDummyResourceNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
					goto done;
				}
			}
			else
#endif
			{
				// Create the resource
				if( FAILED( hr = m_pDevice1->CreateTexture2D( &texDesc, pInitData, (ID3D11Texture2D**)(&m_pResourceToDiscard) ) ) )
				{
					LogError( __FILEW__, __LINE__, L"Failed to CreateTexture2D m_pResourceToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
					goto done;
				}

				// Create the dummy resource
				if( FAILED( hr = m_pDevice1->CreateTexture2D( &texDesc, pInitData, (ID3D11Texture2D**)(&m_pDummyResourceNoDiscard) ) ) )
				{
					LogError( __FILEW__, __LINE__, L"Failed to CreateTexture2D m_pDummyResourceNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
					goto done;
				}
			}

			// Create the staging resource
			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			texDesc.Usage = D3D11_USAGE_STAGING;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			if( FAILED( hr = m_pDevice1->CreateTexture2D( &texDesc, pInitData, (ID3D11Texture2D**)(&m_pStagingResource) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture2D m_pStagingResource.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			D3D11_TEXTURE3D_DESC texDesc;
			texDesc.BindFlags = m_BindFlags;
			texDesc.CPUAccessFlags = m_CPUAccessFlag;
			texDesc.Depth = m_uDepth;
			texDesc.Format = m_SrcFormat;
			texDesc.Height = m_uHeight;
			texDesc.MipLevels = m_uMipLevels;
			texDesc.MiscFlags = m_ResourceMiscFlag;
			texDesc.Usage = m_Usage;
			texDesc.Width = m_uWidth;
			
			// Create the resource
			if( FAILED( hr = m_pDevice1->CreateTexture3D( &texDesc, pInitData, (ID3D11Texture3D**)(&m_pResourceToDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture3D m_pResourceToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the dummy resource
			if( FAILED( hr = m_pDevice1->CreateTexture3D( &texDesc, pInitData, (ID3D11Texture3D**)(&m_pDummyResourceNoDiscard) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture3D m_pDummyResourceNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}

			// Create the staging resource
			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			texDesc.Usage = D3D11_USAGE_STAGING;
			if( FAILED( hr = m_pDevice1->CreateTexture3D( &texDesc, pInitData, (ID3D11Texture3D**)(&m_pStagingResource) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to CreateTexture3D m_pStagingResource.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
				goto done;
			}
		}
		break;
	}

	// If we got here, we pass
	assert( m_pResourceToDiscard );
	assert( m_pDummyResourceNoDiscard );
	assert( m_pStagingResource );
	success = true;

done:
	if (success == true)
	{
		return RESULT_PASS;
	}
	else
	{
		return RESULT_FAIL;
	}
}

////

bool CDiscardTest::CreateViews()
{
	// Do nothing by default.
	// A test has to override this function if it is needed
	return true;
}

////

bool CDiscardTest::UpdateSubresourceData()
{
	// Update the subresources to contain unique data which can be verified later

	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	const UINT numSubresourcesInResource = m_uArraySize * m_uMipLevels;
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	const UINT numBytesPerElement = bResourceIsBuffer && !m_bTestUsesView ? 1 : CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;
	ID3D11DeviceContext1 *pImmediateContext1 = GetFramework()->GetImmediateContext1();

	// If the resource was initalized with data, just return
	if( m_bInitResourceData )
	{
		WriteToLog( _T("Nothing to update because the resource has already been initalized with data.") );
		success = true;
		goto done;
	}

	// Ensure our subresource data is OK
	if( m_subresourceData.size() < numSubresourcesInResource )
	{
		wchar_t errStr[WCHAR_STR_SIZE];
		swprintf_s( errStr, L"The resource contains %d subresource(s), but the subresource data vector's size is %d.", numSubresourcesInResource, m_subresourceData.size());
		LogError( __FILEW__, __LINE__, errStr, false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			WriteToLog( _T("Map and write on Subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				goto done;
			}

			WriteToLog( _T("Expected dimensions for this subresource are (%d, %d, %d)."), currMipWidth, currMipHeight, currMipDepth );

			// Map the subresource
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			ZeroMemory( &mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE) );
			if( FAILED( hr = pImmediateContext1->Map( m_pStagingResource, currSubresource, D3D11_MAP_WRITE, 0, &mappedSubresource ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to map m_pStagingResource for write discard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
				goto done;
			}

			// Update the subresource
			const size_t numBytesPerRow = currMipWidth * numBytesPerElement;
			const size_t numBytesPerDepthSlice = currMipHeight * numBytesPerRow;
			const size_t numBytesToCopy = currMipDepth * numBytesPerDepthSlice;
			WriteToLog( _T("Copying %d byte(s) to the staging subresource..."), numBytesToCopy );
			BYTE *pByteDataDst = reinterpret_cast<BYTE*>(mappedSubresource.pData);
			const BYTE *pByteDataSrc = reinterpret_cast<const BYTE*>(m_subresourceData[currSubresource].pSysMem);
			for( UINT d = 0; d < currMipDepth; d++ )
			{
				for( UINT r = 0; r < currMipHeight; r++ )
				{
					const UINT rowByteOffsetSrc = d * numBytesPerDepthSlice + r * numBytesPerRow;						// Tightly packed
					const UINT rowByteOffsetDst = d * mappedSubresource.DepthPitch + r * mappedSubresource.RowPitch;	// Packed by GPU (can be loose)
					if( GetFramework()->LogVerbose() )
					{
						WriteToLog( _T("Copying %d byte(s) to the staging subresource... Src byte offset = %d. Dest byte offset = %d."), numBytesPerRow, rowByteOffsetSrc, rowByteOffsetDst );
					}
					memcpy( &(pByteDataDst[rowByteOffsetDst]), &(pByteDataSrc[rowByteOffsetSrc]), numBytesPerRow );
				}
			}

			// Unmap the subresource
			pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
		}
	}

	// Each resource should contain the same data
	m_pEffectiveContext1->CopyResource( m_pResourceToDiscard, m_pStagingResource );
	m_pEffectiveContext1->CopyResource( m_pDummyResourceNoDiscard, m_pStagingResource );

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardTest::VerifySubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	const UINT numBytesPerElement = bResourceIsBuffer && !m_bTestUsesView ? 1 : CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;
	const bool bResourceHasDepth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D;
	const bool bResourceHasHeight = bResourceHasDepth || 
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	const bool bResourceHasWidth = bResourceHasHeight ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	ID3D11DeviceContext1 *pImmediateContext1 = GetFramework()->GetImmediateContext1();

	// Copy the resource to a staging copy
	m_pEffectiveContext1->CopyResource( m_pStagingResource, pResourceToVerify );

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			// See if we should verify the current subresource
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			bool bVerifyCurrentSubresource = false;
			for( UINT i = 0; i < subresourcesToVerify.size(); i++ )
			{
				if( subresourcesToVerify[i] == currSubresource )
				{
					bVerifyCurrentSubresource = true;
					break;
				}
			}

			if( !bVerifyCurrentSubresource )
			{
				// We should not check the current subresource, try the next one
				WriteToLog( _T("Not checking subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );
				continue;
			}

			assert( bVerifyCurrentSubresource );
			BeginSection( L"Checking a subresource..." );
			WriteToLog( _T("About to verify subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				EndSection();
				goto done;
			}

			WriteToLog( _T("Expected dimensions for this subresource are (%d, %d, %d)."), currMipWidth, currMipHeight, currMipDepth );
			WriteToLog( _T("Checking to verify the subresource contains a %#x pattern for different elements."), m_ResourceBitPattern );
			WriteToLog( _T("Each element is %d byte(s)."), numBytesPerElement );

			// Map the subresource
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			ZeroMemory( &mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE) );
			if( FAILED( hr = pImmediateContext1->Map( m_pStagingResource, currSubresource, D3D11_MAP_READ, 0, &mappedSubresource ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to map m_pStagingResource for read.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
				EndSection();
				goto done;
			}

			//
			// Verify the subresource contains the expected bit pattern
			//
			BYTE *pByteData = reinterpret_cast<BYTE*>(mappedSubresource.pData);

			// Just check the first and last depth slices...
			for( UINT d = 0; d < (UINT)(bResourceHasDepth ? 2 : 1); d++ )
			{
				// Just check the first and last rows in each depth slice...
				for( UINT r = 0; r < (UINT)(bResourceHasHeight ? 2 : 1); r++ )
				{
					// Just check the first and last elements in each row...
					for( UINT e = 0; e < (UINT)(bResourceHasWidth ? 2 : 1); e++ )
					{
						// Compute the index of the element to read
						const UINT z = d * (currMipDepth - 1);
						const UINT y = r * (currMipHeight - 1);
						const UINT x = e * (currMipWidth - 1);

						WriteToLog( _T("Reading element (%d, %d, %d) from mapped memory and checking all %d byte(s)..."), x, y, z, numBytesPerElement );
						const UINT elementByteOffset = (z * mappedSubresource.DepthPitch) + (y * mappedSubresource.RowPitch) + (x * numBytesPerElement);
						
						// Check every byte in the element...
						for( UINT byte = 0; byte < numBytesPerElement; byte++ )
						{
							// Check to see if the byte for the current element is valid
							// E.g DXGI_FORMAT_D32_FLOAT_S8X24_UINT is a 64-bit format, where the last 3 bytes are junk
							switch( m_SrcFormat )
							{
							case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
								{
									assert(8 == numBytesPerElement);
									if( byte >= 5 ) // 5, 6, and 7 are junk
									{
										continue;
									}
								}
							}

							// Compute the offset in the mapped memory for the desired element and check it against the bit pattern
							const UINT byteOffset = elementByteOffset + byte;
							const BYTE mappedByteValue = pByteData[byteOffset];
							if( mappedByteValue != m_ResourceBitPattern )
							{
								WriteToLog( _T("DepthPitch = %d. RowPitch = %d. pData = %#x"), mappedSubresource.DepthPitch, mappedSubresource.RowPitch, mappedSubresource.pData);
								WriteToLog( _T("Element (%d, %d, %d) is %d byte(s) in the mapped staging memory (pData)."), x, y, z, elementByteOffset );
								WriteToLog( _T("Byte #%d of the element was %#x. The expected bit pattern is %#x."), byte, mappedByteValue, m_ResourceBitPattern );
								LogError( __FILEW__, __LINE__, L"The mapped byte did not match the expected bit pattern.", 0, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
								pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
								EndSection();
								goto done;
							}
						}
					}
				}
			}

			// Unmap the subresource
			pImmediateContext1->Unmap( m_pStagingResource, currSubresource );

			// If we got here, the subresource passed, so discard the output
			EndSection(false);
			WriteToLog( _T("Subresource #%d, MipLevel(%d), ArraySlice(%d), passed the test."), currSubresource, mipLevel, arraySlice );
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardResourceTest

void CDiscardResourceTest::InitTestParameters()
{
	// Local variables
	const bool bIsFL9X = GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0;
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	GenerateCommonResourceFactorCombinations();

	// Setup GuardRects
	CUserValueSet<eRectScenario> *pGuardRectValues = new CUserValueSet<eRectScenario>( );
	pGuardRectValues->AddValue( eOneRectCoveringQuadrant1 );
	pGuardRectValues->AddValue( eOneRectCoveringQuadrant2 );
	pGuardRectValues->AddValue( eOneRectCoveringQuadrant3 );
	pGuardRectValues->AddValue( eOneRectCoveringQuadrant4 );
	pGuardRectValues->AddValue( eOneRectFullCoverage );
	pGuardRectValues->AddValue( eOneRectCompletelyOutside );
	pGuardRectValues->AddValue( eOneRectFourPixels ); 
	pGuardRectValues->AddValue( eOneEmptyRectTopEqualsBottom );
	pGuardRectValues->AddValue( eOneEmptyRectTopGreaterThanBottom );
#if USE_GUARD_RECT_SCENARIO
	m_rGuardRectScenario = AddParameter<eRectScenario>( _T( g_guardRectScenarioStr ), &m_guardRectScenario, pGuardRectValues );
	SetParameterDefaultValue<eRectScenario>( _T( g_guardRectScenarioStr ), eNULLRectList );
#endif

	//
	// Create pairwise sets for different resource scenarios
	//

	// Buffers
	testfactor::XFACTOR bufferPairwiseTable[] =
	{
		{m_rResourceScenarioBuffer, 1, NULL, 0},
		{m_rResourceWidth},	// Width (in bytes)
		{m_rDefaultUsageCPU_Buf + m_rDynamicUsageCPU_Buf, 1, NULL, 0},	// Usage, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},		// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageCPU_Buf = NewCrossFactor(bufferPairwiseTable, _countof(bufferPairwiseTable), _T("bufferPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rBufferTest = rScenarioWidthUsageCPU_Buf * (m_rBindFlags_SRRTUA + m_rResourceBindFlagCB);	// Cycle on all bind flags. Buffers cannot be Depth Stencils

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTable[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::XFACTOR tex1DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D = NewCrossFactor(tex1DPairwiseTable, _countof(tex1DPairwiseTable), _T("tex1DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D_DS = NewCrossFactor(tex1DPairwiseTableDS, _countof(tex1DPairwiseTableDS), _T("tex1DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1D * m_rBindFlags_SRRTUA * m_rResourceWidth) +		// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_Tex1D_DS * m_rResourceBindFlagDS * m_rResourceWidth);	// DS resources have specific formats

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::XFACTOR tex1DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray = NewCrossFactor(tex1DArrayPairwiseTable, _countof(tex1DArrayPairwiseTable), _T("tex1DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS = NewCrossFactor(tex1DArrayPairwiseTableDS, _countof(tex1DArrayPairwiseTableDS), _T("tex1DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray * m_rBindFlags_SRRTUA * m_rResourceWidth) +		// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS * m_rResourceBindFlagDS * m_rResourceWidth);	// DS resources have specific formats

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	// Texture2D with GuardRect
	testfactor::XFACTOR tex2DGuardRectPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats + m_rResourceVideoFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
#if USE_GUARD_RECT_SCENARIO
		{m_rGuardRectScenario, 1, NULL, 0},
#endif
	};

	testfactor::XFACTOR tex2DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D = NewCrossFactor(tex2DPairwiseTable, _countof(tex2DPairwiseTable), _T("tex2DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DGuardRect = NewCrossFactor(tex2DGuardRectPairwiseTable, _countof(tex2DGuardRectPairwiseTable), _T("tex2DGuardRectPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D_DS = NewCrossFactor(tex2DPairwiseTableDS, _countof(tex2DPairwiseTableDS), _T("tex2DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D * m_rBindFlags_SRRTUA * m_rResourceHeight) +				// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_Tex2DGuardRect * m_rBindFlags_SRRT * m_rResourceHeight) +		// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_Tex2D_DS * m_rResourceBindFlagDS * m_rResourceHeight);			// DS resources have specific formats

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::XFACTOR tex2DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray = NewCrossFactor(tex2DArrayPairwiseTable, _countof(tex2DArrayPairwiseTable), _T("tex2DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS = NewCrossFactor(tex2DArrayPairwiseTableDS, _countof(tex2DArrayPairwiseTableDS), _T("tex2DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray * m_rBindFlags_SRRTUA * m_rResourceHeight) +		// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS * m_rResourceBindFlagDS * m_rResourceHeight);	// DS resources have specific formats

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::XFACTOR tex2DMSPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS = NewCrossFactor(tex2DMSPairwiseTable, _countof(tex2DMSPairwiseTable), _T("tex2DMSPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS = NewCrossFactor(tex2DMSPairwiseTableDS, _countof(tex2DMSPairwiseTableDS), _T("tex2DMSPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS * m_rBindFlags_SRRT) +			// Non-DS formats + bind flags (MS is not supported on UAVs)
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::XFACTOR tex2DMSArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray = NewCrossFactor(tex2DMSArrayPairwiseTable, _countof(tex2DMSArrayPairwiseTable), _T("tex2DMSArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS = NewCrossFactor(tex2DMSArrayPairwiseTableDS, _countof(tex2DMSArrayPairwiseTableDS), _T("tex2DMSArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray * m_rBindFlags_SRRT) +			// Non-DS formats + bind flags (MS is not supported on UAVs)
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Texture Cube
	testfactor::XFACTOR texCubePairwiseTable[] =
	{
		{m_rResourceScenarioTexCube, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageMipCPU_Cube, 1, NULL, 0},	// Usage, Array size (Set to 6), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rResourceMiscFlag_TexCube, 1, NULL, 0},	// Texture cube
	};

	testfactor::XFACTOR texCubePairwiseTableDS[] =
	{
		{m_rResourceScenarioTexCube, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageMipCPU_Cube, 1, NULL, 0},	// Usage, Array size (Set to 6), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rResourceMiscFlag_TexCube, 1, NULL, 0},	// Texture cube
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_TexCube = NewCrossFactor(texCubePairwiseTable, _countof(texCubePairwiseTable), _T("texCubePairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_TexCube_DS = NewCrossFactor(texCubePairwiseTableDS, _countof(texCubePairwiseTableDS), _T("texCubePairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTextureCubeTest = 
		(rScenarioWidthUsageArrayMipCPU_TexCube * m_rBindFlags_SRRTUA) +		// Non-DS formats + bind flags
		(rScenarioWidthUsageArrayMipCPU_TexCube_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Texture3D
	testfactor::XFACTOR tex3DPairwiseTable[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::XFACTOR tex3DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceDSFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D = NewCrossFactor(tex3DPairwiseTable, _countof(tex3DPairwiseTable), _T("tex3DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D_DS = NewCrossFactor(tex3DPairwiseTableDS, _countof(tex3DPairwiseTableDS), _T("tex3DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture3DTest;
	if( !bIsFL9X )
	{
		// Only run texture 3d on FL10+
		rTexture3DTest = 
			(rScenarioWidthUsageArrayMipCPU_Tex3D * m_rBindFlags_SRRTUA * m_rResourceDepth) +		// Non-DS formats + bind flags
			(rScenarioWidthUsageArrayMipCPU_Tex3D_DS * m_rResourceBindFlagDS * m_rResourceDepth);	// DS resources have specific formats
	}

	// Set the test factor and priorities
	SetRootTestFactor(
		rBufferTest +
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest +
		rTextureCubeTest +
		rTexture3DTest );
}

////

bool CDiscardResourceTest::Discard()
{
	m_bGuardRectWasSet = false;
#if USE_GUARD_RECT_SCENARIO
	// Set the guard rect
	const bool bUseGuardRect = m_guardRectScenario != eNULLRectList;
	if( bUseGuardRect )
	{
		if( m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE2D )
		{
			LogError( __FILEW__, __LINE__, L"The resource is not a Texture2D but the test is about to set a guard rect.", 0, ERRORCODE_TYPE_BOOL, gFCLogicError );
			return false;
		}

		const UINT widthDiv2 = m_uWidth / 2;
		const UINT heightDiv2 = m_uHeight / 2;
		switch( m_guardRectScenario )
		{
		case eOneRectCoveringQuadrant1:
			m_guardRect.left = 0;			m_guardRect.top = 0;			m_guardRect.right = widthDiv2;	m_guardRect.bottom = heightDiv2;
			break;
		case eOneRectCoveringQuadrant2:
			m_guardRect.left = widthDiv2;	m_guardRect.top = 0;			m_guardRect.right = m_uWidth;	m_guardRect.bottom = heightDiv2;
			break;
		case eOneRectCoveringQuadrant3:
			m_guardRect.left = 0;			m_guardRect.top = heightDiv2;	m_guardRect.right = widthDiv2;	m_guardRect.bottom = m_uHeight;
			break;
		case eOneRectCoveringQuadrant4:
			m_guardRect.left = widthDiv2;	m_guardRect.top = heightDiv2;	m_guardRect.right = m_uWidth;		m_guardRect.bottom = m_uHeight;
			break;
		case eOneRectFullCoverage:
			m_guardRect.left = 0;			m_guardRect.top = 0;			m_guardRect.right = m_uWidth;		m_guardRect.bottom = m_uHeight;
			break;
		case eOneRectCompletelyOutside:
			m_guardRect.left = -4;			m_guardRect.top = -4;			m_guardRect.right = 0;				m_guardRect.bottom = 0;
			break;
		case eOneRectFourPixels:
			m_guardRect.left = 4;			m_guardRect.top = 4;			m_guardRect.right = 8;				m_guardRect.bottom = 8;
			break;
		case eOneEmptyRectTopEqualsBottom:
			m_guardRect.left = 4;			m_guardRect.top = 0;			m_guardRect.right = 8;				m_guardRect.bottom = 0;
			break;
		case eOneEmptyRectTopGreaterThanBottom:
			m_guardRect.left = 0;			m_guardRect.top = heightDiv2;	m_guardRect.right = widthDiv2;		m_guardRect.bottom = 0;
			break;
		default:
			LogError( __FILEW__, __LINE__, L"The test is using a guard rect scenario that it doesn't understand. The test must be updated.", 0, ERRORCODE_TYPE_BOOL, gFCLogicError );
			return false;
		}
		WriteToLog( _T("Setting GuardRect: (L = %ld, T = %ld, R = %ld, B = %ld)"),
					m_guardRect.left, m_guardRect.top, m_guardRect.right, m_guardRect.bottom );
		m_pPartnerDevice->SetGuardRect( static_cast<ID3D11Texture2D*>(m_pResourceToDiscard), m_guardRect);
		m_bGuardRectWasSet = true;
	}
#endif

	// Discard the resource
	m_pEffectiveContext1->DiscardResource( m_pResourceToDiscard );

	// Do we need to discard again?
	if( m_bDoubleDiscard )
	{
		m_pEffectiveContext1->DiscardResource( m_pResourceToDiscard );
	}

#if USE_GUARD_RECT_SCENARIO
	// Remove the guard rect
	if( bUseGuardRect )
	{
		m_pPartnerDevice->SetUnguarded( static_cast<ID3D11Texture2D*>(m_pResourceToDiscard) );
	}
#endif

	return true;
}

////

bool CDiscardResourceTest::VerifyResults() const
{
	// Local variables
	bool success = false;

	// We can only verify the resource that was not discarded
	// And we can verify all the subresources in the dummy resource
	std::vector<UINT> allSubresources;
	if( !GetAllSubresources( allSubresources, m_pDummyResourceNoDiscard ) )
	{
		WriteToLog( _T("GetAllSubresources() for m_pDummyResourceNoDiscard failed.") );
		goto done;
	}

	WriteToLog( _T("About to verify all subresources in m_pDummyResourceNoDiscard.") );
	if( !VerifySubresourceData( m_pDummyResourceNoDiscard, allSubresources ) )
	{
		WriteToLog( _T("VerifySubresourceData() failed.") );
		goto done;
	}

	if( m_bGuardRectWasSet )
	{
		WriteToLog( _T("About to verify all subresources in m_pResourceToDiscard because a GuardRect was set.") );
		if( !VerifySubresourceDataAgainstGuardRect( m_pResourceToDiscard, allSubresources ) )
		{
			WriteToLog( _T("VerifySubresourceDataAgainstGuardRect() failed.") );
			goto done;
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardResourceTest::GetTexelsToCheck( StdPointSet &texelsToCheck, const UINT &mipSlice ) const
{
	// Check the corners of the Guard RECT...
	// The o's and #'s below define a rect.
	// Check at X's (outside rect) if it's a valid rect, also check o's (inside rect) if the rect should be ignored (empty or inside-out)
	//  x     x
	// xo#####ox
	//  #######
	// xo#####ox
	//  x     x

	// Note: By convention, the right and bottom edges of the rectangle are normally considered exclusive. 
	// In other words, the pixel whose coordinates are ( right, bottom ) lies immediately outside of the rectangle.
	// For example, when RECT is passed to the FillRect function, the rectangle is filled up to, 
	// but not including, the right column and bottom row of pixels.

	// Get the dimensions for the current subresource
	// Note texels are only checked 
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	UINT currMipWidth = m_uWidth;
	UINT currMipHeight = m_uHeight;
	UINT currMipDepth = m_uDepth;
	if( !GetMipDimensions(mipSlice, currMipWidth, currMipHeight, currMipDepth) )
	{
		LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}
	WriteToLog( _T("Snapping texels to check against mipslice %u's dimensions: (%u, %u, %u). Depth is not used because RECTs don't have depth."), mipSlice, currMipWidth, currMipHeight, currMipDepth );

	// Empty the set
	texelsToCheck.clear();
	assert( texelsToCheck.size() == 0 );

	//
	// Build the master set of points to check
	//

	POINT tempPoint;
	RECT currRect = m_guardRect;

	// Add texels outside the rect...
	// Note we will have to check if these points are actually in the subresource later
	tempPoint.x = currRect.left;		tempPoint.y = currRect.top - 1;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight ); texelsToCheck.insert( tempPoint );
	tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.top - 1;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );

	tempPoint.x = currRect.left - 1;	tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
	tempPoint.x = currRect.right;		tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the right edge is exclusive (outside the rect)

	tempPoint.x = currRect.left - 1;	tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
	tempPoint.x = currRect.right;		tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the right edge is exclusive (outside the rect)

	tempPoint.x = currRect.left;		tempPoint.y = currRect.bottom;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the bottom edge is exclusive (outside the rect)
	tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.bottom;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the bottom edge is exclusive (outside the rect)

	// Add texels inside the rect if the rect is empty...
	if( IsRectEmpty( &currRect ) )
	{
		tempPoint.x = currRect.left;		tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
		tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );

		tempPoint.x = currRect.left;		tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
		tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
	}

	// Now for each point in our set, see if it is inside any of the rects...
	for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); )
	{
		const POINT currPoint = *it;
		bool bPointInsideARect = false;

		// If the GuardRECT is not empty and the point is inside the rect...
		if( !IsRectEmpty( &m_guardRect ) &&
			PtInRect( &m_guardRect, currPoint ) )
		{
			bPointInsideARect = true;
		}

		// Remove the point from the set if it is inside the guard rect
		StdPointSet::iterator currIter = it++;
		if( bPointInsideARect )
		{
			texelsToCheck.erase( currIter );
		}
	}

	if( GetFramework()->LogVerbose() )
	{
		WriteToLog( _T("GuardRect resulted in %u POINTs to check%s"), texelsToCheck.size(), texelsToCheck.size() > 0 ? _T(":") : _T(".") );
		UINT count = 0;
		for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); it++ )
		{
			WriteToLog( _T("  POINT[%u] = (%li, %li)"), count++, it->x, it->y );
		}
	}

	return true;
}

////

bool CDiscardResourceTest::VerifySubresourceDataAgainstGuardRect( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	assert( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D );
	const UINT numBytesPerElement = CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;
	const bool bResourceHasDepth = false;
	const bool bResourceHasHeight = true;
	const bool bResourceHasWidth = true;
	ID3D11DeviceContext1 *pImmediateContext1 = GetFramework()->GetImmediateContext1();
	StdPointSet texelsToCheck(g_pfnPointCompareLessThan);

	// Copy the resource to a staging copy
	m_pEffectiveContext1->CopyResource( m_pStagingResource, pResourceToVerify );

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			// See if we should verify the current subresource
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			bool bVerifyCurrentSubresource = false;
			for( UINT i = 0; i < subresourcesToVerify.size(); i++ )
			{
				if( subresourcesToVerify[i] == currSubresource )
				{
					bVerifyCurrentSubresource = true;
					break;
				}
			}

			if( !bVerifyCurrentSubresource )
			{
				// We should not check the current subresource, try the next one
				if( GetFramework()->LogVerbose() ) WriteToLog( _T("Not checking subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );
				continue;
			}

			assert( bVerifyCurrentSubresource );
			BeginSection( L"Checking a subresource..." );
			WriteToLog( _T("About to verify subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );

			// Get the texels to check based on the current mip level
			if( !GetTexelsToCheck( texelsToCheck, mipLevel ) )
			{
				WriteToLog( _T("GetTexelsToCheck() failed.") );
				return false;
			}

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				EndSection();
				goto done;
			}

			WriteToLog( _T("Expected dimensions for this subresource are (%d, %d, %d)."), currMipWidth, currMipHeight, currMipDepth );
			WriteToLog( _T("Checking to verify the subresource contains a %#x pattern for different elements."), m_ResourceBitPattern );
			WriteToLog( _T("Each element is %d byte(s)."), numBytesPerElement );

			// Map the subresource
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			ZeroMemory( &mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE) );
			if( FAILED( hr = pImmediateContext1->Map( m_pStagingResource, currSubresource, D3D11_MAP_READ, 0, &mappedSubresource ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to map m_pStagingResource for read.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
				EndSection();
				goto done;
			}

			//
			// Verify the subresource contains the expected bit pattern around the guard rect
			//
			BYTE *pByteData = reinterpret_cast<BYTE*>(mappedSubresource.pData);

			// Check the first and last depth slice...
			for( UINT d = 0; d < (UINT)(bResourceHasDepth ? 2 : 1); d++ )
			{
				const UINT currDepthSlice = d * (currMipDepth - 1);

				// Go through each texel and verify it matches the expected bit pattern
				for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); it++ )
				{
					const POINT currTexel = *it;

					// Compute the index of the element to read
					const UINT z = d * (currMipDepth - 1);
					const UINT y = currTexel.y;
					const UINT x = currTexel.x;

					WriteToLog( _T("Reading element (%d, %d, %d) from mapped memory and checking all %d byte(s)..."), x, y, z, numBytesPerElement );
					const UINT elementByteOffset = (z * mappedSubresource.DepthPitch) + (y * mappedSubresource.RowPitch) + (x * numBytesPerElement);
						
					// Check every byte in the element...
					for( UINT byte = 0; byte < numBytesPerElement; byte++ )
					{
						// Check to see if the byte for the current element is valid
						// E.g DXGI_FORMAT_D32_FLOAT_S8X24_UINT is a 64-bit format, where the last 3 bytes are junk
						switch( m_SrcFormat )
						{
						case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
							{
								assert(8 == numBytesPerElement);
								if( byte >= 5 ) // 5, 6, and 7 are junk
								{
									continue;
								}
							}
						}

						// Compute the offset in the mapped memory for the desired element and check it against the bit pattern
						const UINT byteOffset = elementByteOffset + byte;
						const BYTE mappedByteValue = pByteData[byteOffset];
						if( mappedByteValue != m_ResourceBitPattern )
						{
							WriteToLog( _T("DepthPitch = %d. RowPitch = %d. pData = %#x"), mappedSubresource.DepthPitch, mappedSubresource.RowPitch, mappedSubresource.pData);
							WriteToLog( _T("Element (%d, %d, %d) is %d byte(s) in the mapped staging memory (pData)."), x, y, z, elementByteOffset );
							WriteToLog( _T("Byte #%d of the element was %#x. The expected bit pattern is %#x."), byte, mappedByteValue, m_ResourceBitPattern );
							LogError( __FILEW__, __LINE__, L"The mapped byte did not match the expected bit pattern.", 0, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
							pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
							EndSection();
							goto done;
						}
					}
				}
			}

			// Unmap the subresource
			pImmediateContext1->Unmap( m_pStagingResource, currSubresource );

			// If we got here, the subresource passed, so discard the output
			EndSection(false);
			WriteToLog( _T("Subresource #%d, MipLevel(%d), ArraySlice(%d), passed the test."), currSubresource, mipLevel, arraySlice );
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardViewTest

bool CDiscardViewTest::Discard()
{
	// Local variables
	bool success = false;

	// The views should be created at this point...
	if( !m_pResourceViewToDiscard )
	{
		LogError( __FILEW__, __LINE__, L"The view was not created. m_pResourceViewToDiscard must not be NULL here.", 0, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	// Discard the resource
	m_pEffectiveContext1->DiscardView( m_pResourceViewToDiscard );

	// Do we need to discard again?
	if( m_bDoubleDiscard )
	{
		m_pEffectiveContext1->DiscardView( m_pResourceViewToDiscard );
	}

	// If we got here, we passed
	success = true;

done:
	return true;
}

////

bool CDiscardViewTest::VerifyResults() const
{
	// Local variables
	bool success = false;

	// We can only verify subresources that were not discarded
	// And we can verify all the subresources in the dummy resource
	std::vector<UINT> allSubresources;
	std::vector<UINT> subresourcesOutsideView;
	std::vector<UINT> subresourcesPartiallyCoveredByView;

	BeginSection( L"Verify all subresource data in m_pDummyResourceNoDiscard." );
	if( !GetAllSubresources( allSubresources, m_pDummyResourceNoDiscard ) )
	{
		WriteToLog( _T("GetAllSubresources() for m_pDummyResourceNoDiscard failed.") );
		EndSection();
		goto done;
	}

	if( !VerifySubresourceData( m_pDummyResourceNoDiscard, allSubresources ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pDummyResourceNoDiscard failed.") );
		EndSection();
		goto done;
	}
	EndSection(false);
	WriteToLog( _T("Resource m_pDummyResourceNoDiscard passed the test.") );

	// Determine the set of subresources that are completely outside the view
	BeginSection( L"Verify subresource data in m_pResourceToDiscard that is completely outside the defined view." );
	if( !GetSubresourcesOutsideView( subresourcesOutsideView, m_pResourceToDiscard ) )
	{
		WriteToLog( _T("GetSubresourceOutsideView() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	if( !VerifySubresourceData( m_pResourceToDiscard, subresourcesOutsideView ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}
	EndSection(false);
	// Note: m_pResourceToDiscard has not passed the test yet!

	// Determine if a subresource had a partial view when it was discarded
	// If so, it means parts of that subresource can be checked
	BeginSection( L"Verify subresource data in m_pResourceToDiscard that is partially outside the defined view." );
	if( !GetSubresourcesPartiallyCoveredByView( subresourcesPartiallyCoveredByView, m_pResourceToDiscard ) )
	{
		WriteToLog( _T("GetSubresourcesPartiallyCoveredByView() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	if( subresourcesPartiallyCoveredByView.size() > 0 )
	{
		WriteToLog( _T("Some subresources have data that is partially covered by the defined view." ) );
		if( !VerifyPartialSubresourceData( m_pResourceToDiscard, subresourcesPartiallyCoveredByView ) )
		{
			WriteToLog( _T("VerifyPartialSubresourceData() for m_pResourceToDiscard failed.") );
			EndSection();
			goto done;
		}
	}
	EndSection(false);
	WriteToLog( _T("Resource m_pResourceToDiscard passed the test.") );

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

void CDiscardViewTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pResourceViewToDiscard );
	SAFE_RELEASE( m_pDummyResourceViewNoDiscard );

	// Call the parent's cleanup
	CDiscardTest::CleanupTestCase();
}

////

void CDiscardViewTest::GenerateCommonViewParameterFactors()
{
	m_rMipSlice0			= AddParameter<UINT>( _T( g_viewMipSliceStr ), &m_uViewMipSlice, (UINT)0 );
	m_rMipSlice1			= AddParameterValue<UINT>( _T( g_viewMipSliceStr ), (UINT)1 );
	m_rMipSlice2			= AddParameterValue<UINT>( _T( g_viewMipSliceStr ), (UINT)2 );
	m_rMipSlice01			= m_rMipSlice0 + m_rMipSlice1;
	m_rMipSlice012			= m_rMipSlice0 + m_rMipSlice1 + m_rMipSlice2;

	m_rFirstArraySlice0		= AddParameter<UINT>( _T( g_viewFirstArraySliceStr ), &m_uViewFirstArraySlice, (UINT)0 );
	m_rFirstArraySlice1		= AddParameterValue<UINT>( _T( g_viewFirstArraySliceStr ), (UINT)1 );
	m_rFirstArraySlice01	= m_rFirstArraySlice0 + m_rFirstArraySlice1;

	m_rArraySize1			= AddParameter<UINT>( _T( g_viewArraySizeStr ), &m_uViewArraySize, (UINT)1 );
	m_rArraySize2			= AddParameterValue<UINT>( _T( g_viewArraySizeStr ), (UINT)2 );
	m_rArraySize3			= AddParameterValue<UINT>( _T( g_viewArraySizeStr ), (UINT)3 );
	m_rArraySize12			= m_rArraySize1 + m_rArraySize2;

	m_rMostDetailedMip0		= AddParameter<UINT>( _T( g_viewMostDetailedMipStr ), &m_uViewMostDetailedMip, (UINT)0 ); 
	m_rMostDetailedMip1		= AddParameterValue<UINT>( _T( g_viewMostDetailedMipStr ), (UINT)1 );
	m_rMostDetailedMip2		= AddParameterValue<UINT>( _T( g_viewMostDetailedMipStr ), (UINT)2 );
	m_rMostDetailedMip012	= m_rMostDetailedMip0 + m_rMostDetailedMip1 + m_rMostDetailedMip2;

	m_rMipLevels1			= AddParameter<UINT>( _T( g_viewMipLevelsStr ), &m_uViewMipLevels, (UINT)1 );  
	m_rMipLevels2			= AddParameterValue<UINT>( _T( g_viewMipLevelsStr ), (UINT)2 );
	m_rMipLevels3			= AddParameterValue<UINT>( _T( g_viewMipLevelsStr ), (UINT)3 ); 
	m_rMipLevels123			= m_rMipLevels1 + m_rMipLevels2 + m_rMipLevels3;

	m_rElementOffset0		= AddParameter<UINT>( _T( g_viewElementOffsetStr ), &m_uViewElementOffset, (UINT)0 );  
	m_rElementOffset1		= AddParameterValue<UINT>( _T( g_viewElementOffsetStr ), (UINT)1 );
	m_rElementOffset01		= m_rElementOffset0 + m_rElementOffset1;

	m_rElementWidth1		= AddParameter<UINT>( _T( g_viewElementWidthStr ), &m_uViewElementWidth, (UINT)1 ); 
	m_rElementWidth15		= AddParameterValue<UINT>( _T( g_viewElementWidthStr ), (UINT)15 );
	m_rElementWidth16		= AddParameterValue<UINT>( _T( g_viewElementWidthStr ), (UINT)16 );
	m_rElementWidth63		= AddParameterValue<UINT>( _T( g_viewElementWidthStr ), (UINT)63 );
	m_rElementWidth64		= AddParameterValue<UINT>( _T( g_viewElementWidthStr ), (UINT)64 );

	m_rViewFirstWSlice_0	= AddParameter<UINT>( _T( g_viewFirstWSliceStr ), &m_uViewFirstWSlice, (UINT)0 );
	m_rViewFirstWSlice_1	= AddParameterValue<UINT>( _T( g_viewFirstWSliceStr ), (UINT)1 );
	m_rViewFirstWSlice_01	= m_rViewFirstWSlice_0 + m_rViewFirstWSlice_1;

	m_rViewWSize_1				= AddParameter<UINT>( _T( g_viewWSizeStr ), &m_uViewWSize, (UINT)1 );
	m_rViewWSize_3				= AddParameterValue<UINT>( _T( g_viewWSizeStr ), (UINT)3 );
	m_rViewWSize_6				= AddParameterValue<UINT>( _T( g_viewWSizeStr ), (UINT)6 );
	m_rViewWSize_8				= AddParameterValue<UINT>( _T( g_viewWSizeStr ), (UINT)8 );
	m_rViewWSize_15				= AddParameterValue<UINT>( _T( g_viewWSizeStr ), (UINT)15 );
	m_rViewWSize_16				= AddParameterValue<UINT>( _T( g_viewWSizeStr ), (UINT)16 );

	//
	// Combinations to make sure the resource and view dimensions are valid
	// Test groups will use these combinations depending on the view type and resource dimension
	//

	// Texture1D, 1DArray, 2D, 2DArray, 2DMS, and 2DMSArray use: mipslice, first array slice, and array size
	m_rResourceViewMipSlice_1		= m_rResourceNumMipLevels_1 * m_rMipSlice0;	// Resource + View have 1 mip level
	m_rResourceViewMipSlice_3		= m_rResourceNumMipLevels_3 * m_rMipSlice012; // Resource has 3 mip levels, view has 1
	m_rResourceViewMipSlice			= m_rResourceViewMipSlice_1 + m_rResourceViewMipSlice_3;

	m_rResourceViewArraySlice_1		= m_rResourceArraySize_1 * m_rFirstArraySlice0 * m_rArraySize1;	// Resource + View have 1 array slice
	m_rResourceViewArraySlice_2		= m_rResourceArraySize_2 * m_rFirstArraySlice01 * m_rArraySize1;	// Resource has 2 array slices, view has 1 starting at beginning or end
	m_rResourceViewArraySlice_6		= m_rResourceArraySize_6 * m_rFirstArraySlice1 * m_rArraySize3;	// Resource has 6 array slices, view has 3 starting in the middle
	m_rResourceViewArraySlice		= m_rResourceViewArraySlice_1 + m_rResourceViewArraySlice_2 + m_rResourceViewArraySlice_6;

	m_ResourceViewMostDetailedMipMipLevels_1_0_1	= m_rResourceNumMipLevels_1 * m_rMostDetailedMip0 * m_rMipLevels1;		// Resource + View have 1 mip level
	m_ResourceViewMostDetailedMipMipLevels_3_1_1	= m_rResourceNumMipLevels_3 * m_rMostDetailedMip1 * m_rMipLevels1;		// Resource + View have 1 mip level starting in the middle
	m_ResourceViewMostDetailedMipMipLevels_3_2_1	= m_rResourceNumMipLevels_3 * m_rMostDetailedMip2 * m_rMipLevels1;		// Resource + View have 1 mip level starting at the end
	m_ResourceViewMostDetailedMipMipLevels_3_0_123	= m_rResourceNumMipLevels_3 * m_rMostDetailedMip0 * m_rMipLevels123;	// Resource has 3 mip levels, view goes from 1 to 3
	m_ResourceViewMostDetailedMipMipLevels = 
		m_ResourceViewMostDetailedMipMipLevels_1_0_1 +
		m_ResourceViewMostDetailedMipMipLevels_3_1_1 +
		m_ResourceViewMostDetailedMipMipLevels_3_2_1 +
		m_ResourceViewMostDetailedMipMipLevels_3_0_123;

	m_rResourceViewWidthElementOffsetElementWidth = 
		m_rResourceWidth_1 * m_rElementOffset0 * m_rElementWidth1 +
		m_rResourceWidth_16 * m_rElementOffset0 * m_rElementWidth16 +
		m_rResourceWidth_16 * m_rElementOffset01 * m_rElementWidth1 +
		m_rResourceWidth_16 * m_rElementOffset1 * m_rElementWidth15;

	m_rResourceViewWidth64ElementOffsetElementWidth = 
		m_rResourceWidth_1 * m_rElementOffset0 * m_rElementWidth1 +
		m_rResourceWidth_64 * m_rElementOffset0 * m_rElementWidth64 +
		m_rResourceWidth_64 * m_rElementOffset01 * m_rElementWidth1 +
		m_rResourceWidth_64 * m_rElementOffset1 * m_rElementWidth63;

	m_rResourceViewDepthFirstWSliceWSizeMipSlice =
		m_rResourceNumMipLevels_1 * m_rMipSlice0 * m_rResourceDepth_1 * m_rViewFirstWSlice_0 * m_rViewWSize_1 +
		m_rResourceNumMipLevels_1 * m_rMipSlice0 * m_rResourceDepth_16 * m_rViewFirstWSlice_1 * m_rViewWSize_15 +
		m_rResourceNumMipLevels_3 * m_rMipSlice1 * m_rResourceDepth_16 * m_rViewFirstWSlice_1 * m_rViewWSize_6 +
		m_rResourceNumMipLevels_3 * m_rMipSlice1 * m_rResourceDepth_16 * m_rViewFirstWSlice_0 * m_rViewWSize_8 +
		m_rResourceNumMipLevels_3 * m_rMipSlice2 * m_rResourceDepth_1 * m_rViewFirstWSlice_0 * m_rViewWSize_1 +
		m_rResourceNumMipLevels_3 * m_rMipSlice2 * m_rResourceDepth_16 * m_rViewFirstWSlice_1 * m_rViewWSize_1 +
		m_rResourceNumMipLevels_3 * m_rMipSlice2 * m_rResourceDepth_16 * m_rViewFirstWSlice_1 * m_rViewWSize_3;
}

////

void CDiscardViewTest::GenerateCommonViewFactorCombinations()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	GenerateCommonViewParameterFactors();

	//
	// Create pairwise sets for common resource combinations
	//

	testfactor::XFACTOR defaultUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageBufAllPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlags, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},	// No array
		{m_rResourceViewMipSlice, 1, NULL, 0},		// Multiple mips
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice, 1, NULL, 0},	// Multiple array slices
		{m_rResourceViewMipSlice, 1, NULL, 0},		// Multiple mips
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice, 1, NULL, 0},	// Multiple array slices
		{m_rResourceViewMipSlice_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},	// No array
		{m_rResourceViewMipSlice_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsCubePairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_6, 1, NULL, 0},
		{m_rResourceViewMipSlice, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},	// Dynamic resources must have 1 array slice
		{m_rResourceViewMipSlice_1, 1, NULL, 0},	// Dynamic resources must have 1 mip
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	m_rDefaultUsageCPU_Buf			= NewCrossFactor(defaultUsageBufPairwiseTable, _countof(defaultUsageBufPairwiseTable), _T("defaultUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageCPUAll_Buf		= NewCrossFactor(defaultUsageBufAllPairwiseTable, _countof(defaultUsageBufAllPairwiseTable), _T("defaultUsageBufAllPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDefaultUsageMipCPU_Tex		= NewCrossFactor(defaultUsageMipsTexPairwiseTable, _countof(defaultUsageMipsTexPairwiseTable), _T("defaultUsageMipsTexPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayMipCPU_Tex	= NewCrossFactor(defaultUsageMipsTexArrayPairwiseTable, _countof(defaultUsageMipsTexArrayPairwiseTable), _T("defaultUsageMipsTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayCPU_Tex		= NewCrossFactor(defaultUsageTexArrayPairwiseTable, _countof(defaultUsageTexArrayPairwiseTable), _T("defaultUsageTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageCPU_Tex			= NewCrossFactor(defaultUsageTexPairwiseTable, _countof(defaultUsageTexPairwiseTable), _T("defaultUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDefaultUsageMipCPU_Cube		= NewCrossFactor(defaultUsageMipsCubePairwiseTable, _countof(defaultUsageMipsCubePairwiseTable), _T("defaultUsageMipsCubePairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDynamicUsageCPU_Buf			= NewCrossFactor(dynamicUsageBufPairwiseTable, _countof(dynamicUsageBufPairwiseTable), _T("dynamicUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	
	m_rDynamicUsageArrayMipCPU_Tex	= NewCrossFactor(dynamicUsageTexPairwiseTable, _countof(dynamicUsageTexPairwiseTable), _T("dynamicUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));
}

////

bool CDiscardViewTest::GetSubresourcesInsideView( std::vector<UINT> &subresourcesInsideView, ID3D11Resource *pResource ) const
{
	// Local variables
	bool success = false;
	const UINT firstArraySlice = m_uViewFirstArraySlice;
	const UINT lastArraySlice = m_uViewFirstArraySlice + m_uViewArraySize;
	const UINT mipSlice = m_uViewMipSlice;
	UINT numArraySlicesInResource = 0;
	UINT numMiplevelsInResource = 0;

	subresourcesInsideView.clear();
	assert( subresourcesInsideView.empty() );

	// Get the array size and number of mip levels in the resource
	if( !GetArraySizeAndMipLevelsFromResource( pResource, numArraySlicesInResource, numMiplevelsInResource ) )
	{
		WriteToLog( _T("GetArraySizeAndMipLevelsFromResource() failed.") );
		goto done;
	}

	switch( m_ResourceScenario )
	{
	default:
		assert( !"Unknown scenario" );
		break;
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			// RTV's, SRV's, and UAV's use ElementOffset
			subresourcesInsideView.push_back(0);
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		{
			// Go through each array slice...
			for( UINT arraySlice = 0; arraySlice < numArraySlicesInResource; arraySlice++ )
			{
				// Make sure we are on a array slice included in the view
				if( arraySlice < firstArraySlice || 
					arraySlice >= lastArraySlice )
				{
					continue;	// Pick the next array slice...
				}

				// Go through each mip level...
				for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
				{
					// See if we should verify the current subresource
					const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, numMiplevelsInResource );
					
					// Make sure we are on a mip view included in the view
					if( mipLevel == mipSlice )
					{
						// Add the subresource...
						subresourcesInsideView.push_back( currSubresource );
					}
				}
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			// RTV's and UAV's use mipslice. This will not work for SRVs because it uses mipslice and num mip levels.
			// SRVs must override this function
			if( D3D11_BIND_SHADER_RESOURCE == m_BindFlags )
			{
				LogError( __FILEW__, __LINE__, L"SRVs must use a different function because they use different parameters for Texture3D.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				goto done;
			}

			// Go through each mip level...
			for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
			{
				// See if we should verify the current subresource
				const UINT currSubresource = D3D11CalcSubresource( mipLevel, 0, numMiplevelsInResource );
				
				// Make sure we are on a mip view included in the view
				if( mipLevel == mipSlice )
				{
					// Add the subresource...
					subresourcesInsideView.push_back( currSubresource );
				}
			}
		}
		break;
	};

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardViewTest::GetSubresourcesOutsideView( std::vector<UINT> &subresourcesOutsideView, ID3D11Resource *pResource ) const
{
	// Local variables
	bool success = false;
	const UINT firstArraySlice = m_uViewFirstArraySlice;
	const UINT lastArraySlice = m_uViewFirstArraySlice + m_uViewArraySize;
	const UINT mipSlice = m_uViewMipSlice;
	UINT numArraySlicesInResource = 0;
	UINT numMiplevelsInResource = 0;

	subresourcesOutsideView.clear();
	assert( subresourcesOutsideView.empty() );

	// Get the subresources inside the view
	std::vector<UINT> subresourcesInsideView;
	if( !GetSubresourcesInsideView( subresourcesInsideView, pResource ) )
	{
		WriteToLog( _T("GetSubresourcesInView() for pResource failed.") );
		goto done;
	}
	const UINT numSubresourcesInsideView = subresourcesInsideView.size();
	WriteToLog( _T("%d subresource(s) inside the current view."), numSubresourcesInsideView );

	// Get the array size and number of mip levels in the resource
	if( !GetArraySizeAndMipLevelsFromResource( pResource, numArraySlicesInResource, numMiplevelsInResource ) )
	{
		WriteToLog( _T("GetArraySizeAndMipLevelsFromResource() failed.") );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < numArraySlicesInResource; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
		{
			// See if we should verify the current subresource
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, numMiplevelsInResource );

			// Go through the subresources inside the view
			bool currSubresourceIsInsideView = false;
			for( UINT i = 0; i < numSubresourcesInsideView; i++ )
			{
				if( subresourcesInsideView[i] == currSubresource )
				{
					// It is in the view
					currSubresourceIsInsideView = true;
				}
			}

			// If it is not in the view, then it must be outside
			if( !currSubresourceIsInsideView )
			{
				subresourcesOutsideView.push_back( currSubresource );
			}
		}
	}

	// If we got here, we passed
	success = true;
	WriteToLog( _T("%d subresource(s) outside the current view."), subresourcesOutsideView.size() );

done:
	return success;
}

////

bool CDiscardViewTest::GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const
{
	// RTVs and UAV's share the same partially overlapping resource types/parameters. Thus they can share the same verification code.
	
	// Local variables
	bool success = false;
	UINT numArraySlicesInResource = 0;
	UINT numMiplevelsInResource = 0;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;

	subresourcesPartiallyCoveredByView.clear();
	assert( subresourcesPartiallyCoveredByView.empty() );

	// Only buffers and Texture3D's can partially cover a subresource for RTV's
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			// If the element offset is not 0, then we are leaving some parts of the buffer outside the view
			// We need to check the bytes from [0,m_uViewElementOffset) 
			if( m_uViewElementOffset != 0 )
			{
				WriteToLog( _T("The view partially covers subresource #0. ElementOffset = %d."), m_uViewElementOffset );
				subresourcesPartiallyCoveredByView.push_back( 0 );
			}
			else if( m_uViewElementOffset == 0 && m_uViewElementWidth < m_uWidth )
			{
				WriteToLog( _T("The view partially covers subresource #0. ElementOffset = %d and ElementWidth = %d but the width is %d"), m_uViewElementOffset, m_uViewElementWidth, m_uWidth );
				subresourcesPartiallyCoveredByView.push_back( 0 );
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			// Get the current subresource
			assert( m_uViewMipSlice < m_uMipLevels );
			const UINT currSubresource = D3D11CalcSubresource( m_uViewMipSlice, 0, m_uMipLevels );

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(m_uViewMipSlice, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				goto done;
			}

			if( m_uViewFirstWSlice != 0 )
			{
				// If the view doesn't start at first w slice = 0, then it skips some slices at the beginning
				WriteToLog( _T("The view partially covers subresource #%d. FirstWSlice = %d."), currSubresource, m_uViewFirstWSlice );
				subresourcesPartiallyCoveredByView.push_back( currSubresource );
			}
			else if( m_uViewFirstWSlice == 0 && m_uViewWSize < currMipDepth )
			{
				// If m_uViewWSize < the depth then the view doesnt contain all the depth slices
				WriteToLog( _T("The view partially covers subresource #%d. FirstWSlice = %d and WSize = %d but the resource depth for mip %d is %d."), currSubresource, m_uViewFirstWSlice, m_uViewWSize, m_uViewMipSlice, currMipDepth );
				subresourcesPartiallyCoveredByView.push_back( currSubresource );
			}
		}
		break;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardViewTest::VerifyPartialSubresourceData( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesPartiallyCoveredByView ) const
{
	// RTVs and UAV's share the same partially overlapping resource types/parameters. Thus they can share the same verification code.
	// SRV's only have partially covered buffers, which is the same types/parameters too.

	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	const UINT numBytesPerElement = CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;
	const bool bResourceHasDepth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D;
	const bool bResourceHasHeight = bResourceHasDepth || 
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	const bool bResourceHasWidth = bResourceHasHeight ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	ID3D11DeviceContext1 *pImmediateContext1 = GetFramework()->GetImmediateContext1();

	// Copy the resource to a staging copy
	m_pEffectiveContext1->CopyResource( m_pStagingResource, pResourceToVerify );
	
	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			// See if we should verify the current subresource
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			bool bVerifyCurrentSubresource = false;
			for( UINT i = 0; i < subresourcesPartiallyCoveredByView.size(); i++ )
			{
				if( subresourcesPartiallyCoveredByView[i] == currSubresource )
				{
					bVerifyCurrentSubresource = true;
					break;
				}
			}

			if( !bVerifyCurrentSubresource )
			{
				// We should not check the current subresource, try the next one
				continue;
			}

			assert( bVerifyCurrentSubresource );
			BeginSection( L"Checking a subresource..." );
			WriteToLog( _T("About to partially verify subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				EndSection();
				goto done;
			}

			WriteToLog( _T("Expected dimensions for this subresource are (%d, %d, %d)."), currMipWidth, currMipHeight, currMipDepth );
			WriteToLog( _T("Checking to verify the subresource contains a %#x pattern for different elements."), m_ResourceBitPattern );
			WriteToLog( _T("Each element is %d byte(s)."), numBytesPerElement );

			// Map the subresource
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			ZeroMemory( &mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE) );
			if( FAILED( hr = pImmediateContext1->Map( m_pStagingResource, currSubresource, D3D11_MAP_READ, 0, &mappedSubresource ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to map m_pStagingResource for read.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
				EndSection();
				goto done;
			}

			//
			// Verify the subresource contains the expected bit pattern
			//
			BYTE *pByteData = reinterpret_cast<BYTE*>(mappedSubresource.pData);

			switch( m_ResourceScenario )
			{
			default:
				assert( !"Unknown resource scenario" );
				break;
			case D3D_RESOURCE_SCENARIO_BUFFER:
				{
					// Go through each element...
					for( UINT element = 0; element < m_uWidth; element++ )
					{
						// Check to see if the element is in the view
						if( element >= m_uViewElementOffset &&
							element < (m_uViewElementOffset + m_uViewElementWidth) )
						{
							// Pick the next element
							continue;
						}

						// If we got here, the current element is valid to check
						WriteToLog( _T("About to check element %d because it's outside the view."), element );
						for( UINT byte = 0; byte < numBytesPerElement; byte++ )
						{
							// Compute the offset in the mapped memory for the desired element and check it against the bit pattern
							const UINT byteOffset = (element * numBytesPerElement) + byte;
							const BYTE mappedByteValue = pByteData[byteOffset];
							if( mappedByteValue != m_ResourceBitPattern )
							{
								WriteToLog( _T("The byte offset into the mapped staging memory was %d."), byteOffset );
								WriteToLog( _T("Byte #%d was %#x. The expected bit pattern is %#x."), byte, mappedByteValue, m_ResourceBitPattern );
								LogError( __FILEW__, __LINE__, L"The mapped byte did not match the expected bit pattern.", 0, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
								pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
								EndSection();
								goto done;
							}
						}
					}
				}
				break;

			case D3D_RESOURCE_SCENARIO_TEXTURE3D:
				{
					// Go through each W Slice...
					for( UINT depthSlice = 0; depthSlice < currMipDepth; depthSlice++ )
					{
						// Make sure the current slice is outside the view
						if( depthSlice >= m_uViewFirstWSlice &&
							depthSlice < (m_uViewFirstWSlice + m_uViewWSize) )
						{
							// The current depth slice is in the view
							continue;
						}

						// If we got here, the current depth slice is valid to check
						WriteToLog( _T("About to check depth slice (w slice) %d because it's outside the view."), depthSlice );

						// Just check the first and last rows in each depth slice...
						for( UINT r = 0; r < (UINT)(bResourceHasHeight ? 2 : 1); r++ )
						{
							// Just check the first and last elements in each row...
							for( UINT e = 0; e < (UINT)(bResourceHasWidth ? 2 : 1); e++ )
							{
								// Compute the index of the element to read
								const UINT z = depthSlice;
								const UINT y = r * (currMipHeight - 1);
								const UINT x = e * (currMipWidth - 1);

								WriteToLog( _T("Reading (%d, %d, %d) from mapped memory and checking all %d byte(s)..."), x, y, z, numBytesPerElement );
								
								// Check every byte in the element...
								for( UINT byte = 0; byte < numBytesPerElement; byte++ )
								{
									// Compute the offset in the mapped memory for the desired element and check it against the bit pattern
									const UINT byteOffset = (z * mappedSubresource.DepthPitch) + (y * mappedSubresource.RowPitch) + (x * numBytesPerElement) + byte;
									const BYTE mappedByteValue = pByteData[byteOffset];
									if( mappedByteValue != m_ResourceBitPattern )
									{
										WriteToLog( _T("The byte offset into the mapped staging memory was %d."), byteOffset );
										WriteToLog( _T("Byte #%d was %#x. The expected bit pattern is %#x."), byte, mappedByteValue, m_ResourceBitPattern );
										LogError( __FILEW__, __LINE__, L"The mapped byte did not match the expected bit pattern.", 0, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
										pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
										EndSection();
										goto done;
									}
								}
							}
						}
					}
				}
				break;
			}

			// Unmap the subresource
			pImmediateContext1->Unmap( m_pStagingResource, currSubresource );

			// If we got here, the subresource passed, so discard the output
			EndSection(false);
			WriteToLog( _T("Subresource #%d, MipLevel(%d), ArraySlice(%d), passed the test."), currSubresource, mipLevel, arraySlice );
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardView1Test

bool CDiscardView1Test::Discard()
{
	// Local variables
	bool success = false;

	// Get our rect list
	m_pRectList = RectScenariosMip0;
	if( m_uViewMipSlice == 1 || m_uViewMipSlice == 2 )
	{
		// Note we don't have rects for mip2 so we just use mip1's rects.
		m_pRectList = RectScenariosMip1;
	}

	// The views should be created at this point...
	if( !m_pResourceViewToDiscard )
	{
		LogError( __FILEW__, __LINE__, L"The view was not created. m_pResourceViewToDiscard must not be NULL here.", 0, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	// Discard the resource
	WriteToLog( _T("Calling DiscardView1() with rects:") );
	WriteRectScenarioToLog( m_pRectList[m_rectScenario] );
	m_pEffectiveContext1->DiscardView1( m_pResourceViewToDiscard, m_pRectList[m_rectScenario].pRects, m_pRectList[m_rectScenario].Count );

	// Do we need to discard again?
	if( m_bDoubleDiscard )
	{
		m_pEffectiveContext1->DiscardView1( m_pResourceViewToDiscard, m_pRectList[m_rectScenario].pRects, m_pRectList[m_rectScenario].Count );
	}

	// Call clearview to ensure the driver uses our Discard hint
	const FLOAT clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pEffectiveContext1->ClearView( m_pResourceViewToDiscard, clearColor, m_pRectList[m_rectScenario].pRects, m_pRectList[m_rectScenario].Count );
	// This code proves that if they get the rects wrong, the test will fail.
	/*RECT *pModifiedRects = new RECT[m_pRectList[m_rectScenario].Count];
	for( UINT i = 0; i < m_pRectList[m_rectScenario].Count; i++ )
	{
		pModifiedRects[i] = m_pRectList[m_rectScenario].pRects[i];
		pModifiedRects[i].right += 1;
	}
	m_pEffectiveContext1->ClearView( m_pResourceViewToDiscard, clearColor, pModifiedRects, m_pRectList[m_rectScenario].Count );*/

	// If we got here, we passed
	success = true;

done:
	return true;
}

////

bool CDiscardView1Test::VerifyResults() const
{
	// Local variables
	bool success = false;

	// We can only verify subresources that were not discarded
	// And we can verify all the subresources in the dummy resource
	std::vector<UINT> allSubresources;
	std::vector<UINT> subresourcesOutsideView;
	std::vector<UINT> subresourcesPartiallyCoveredByView;
	std::vector<UINT> subresourcesInsideView;

	BeginSection( L"Verify all subresource data in m_pDummyResourceNoDiscard." );
	if( !GetAllSubresources( allSubresources, m_pDummyResourceNoDiscard ) )
	{
		WriteToLog( _T("GetAllSubresources() for m_pDummyResourceNoDiscard failed.") );
		EndSection();
		goto done;
	}

	if( !VerifySubresourceData( m_pDummyResourceNoDiscard, allSubresources ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pDummyResourceNoDiscard failed.") );
		EndSection();
		goto done;
	}
	EndSection(false);
	WriteToLog( _T("Resource m_pDummyResourceNoDiscard passed the test.") );

	// Determine the set of subresources that are completely outside the view
	BeginSection( L"Verify subresource data in m_pResourceToDiscard that is completely outside the defined view." );
	if( !GetSubresourcesOutsideView( subresourcesOutsideView, m_pResourceToDiscard ) )
	{
		WriteToLog( _T("GetSubresourceOutsideView() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	if( !VerifySubresourceData( m_pResourceToDiscard, subresourcesOutsideView ) )
	{
		WriteToLog( _T("VerifySubresourceData() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}
	EndSection(false);
	// Note: m_pResourceToDiscard has not passed the test yet!

	// Determine if a subresource had a partial view when it was discarded
	// If so, it means parts of that subresource can be checked
	BeginSection( L"Verify subresource data in m_pResourceToDiscard that is partially outside the defined view." );
	if( !GetSubresourcesPartiallyCoveredByView( subresourcesPartiallyCoveredByView, m_pResourceToDiscard ) )
	{
		WriteToLog( _T("GetSubresourcesPartiallyCoveredByView() for m_pResourceToDiscard failed.") );
		EndSection();
		goto done;
	}

	if( subresourcesPartiallyCoveredByView.size() > 0 )
	{
		WriteToLog( _T("Some subresources have data that is partially covered by the defined view." ) );
		if( !VerifyPartialSubresourceData( m_pResourceToDiscard, subresourcesPartiallyCoveredByView ) )
		{
			WriteToLog( _T("VerifyPartialSubresourceData() for m_pResourceToDiscard failed.") );
			EndSection();
			goto done;
		}
	}
	EndSection(false);

	BeginSection( L"Verify subresource data in m_pResourceToDiscard that is inside the view but outside the rects." );
	if( m_pRectList[m_rectScenario].Count > 0 && m_pRectList[m_rectScenario].pRects != nullptr )
	{
		if( !GetSubresourcesInsideView( subresourcesInsideView, m_pResourceToDiscard ) )
		{
			WriteToLog( _T("GetSubresourcesInsideView() for m_pResourceToDiscard failed.") );
			EndSection();
			goto done;
		}

		if( !VerifySubresourceDataAgainstRects( m_pResourceToDiscard, subresourcesInsideView ) )
		{
			WriteToLog( _T("VerifySubresourceDataAgainstRects() for m_pResourceToDiscard failed.") );
			EndSection();
			goto done;
		}
	}
	else
	{
		WriteToLog( _T("The number of RECTs passed to DiscardView1 (%u) is <= 0 or pRects is NULL. Thus the entire view should be discarded and the test cannot check inside the view because it's undefined."), m_pRectList[m_rectScenario].Count );
	}
	EndSection(false);

	WriteToLog( _T("Resource m_pResourceToDiscard passed the test.") );

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

void CDiscardView1Test::GenerateCommonView1ParameterFactors()
{
	CTestCaseParameter<eRectScenario> *pRectScenario = AddParameter( g_rectScenarioStr, &m_rectScenario );
	CNamedValuesTable<eRectScenario>* pRectValuesTable = NULL;
	GetNamedValuesTable(&pRectValuesTable);
	m_rfRectScenario = AddParameterValueSet( pRectScenario, new CUserValueSet<eRectScenario>(pRectValuesTable));
}

////

bool CDiscardView1Test::GetTexelsToCheck( StdPointSet &texelsToCheck ) const
{
	// Check the corners of each RECT...
	// The o's and #'s below define a rect.
	// Check at X's (outside rect) if it's a valid rect, also check o's (inside rect) if the rect should be ignored (empty or inside-out)
	//  x     x
	// xo#####ox
	//  #######
	// xo#####ox
	//  x     x

	// Note: By convention, the right and bottom edges of the rectangle are normally considered exclusive. 
	// In other words, the pixel whose coordinates are ( right, bottom ) lies immediately outside of the rectangle.
	// For example, when RECT is passed to the FillRect function, the rectangle is filled up to, 
	// but not including, the right column and bottom row of pixels.


	// Get the dimensions for the current subresource
	// Note texels are only checked 
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	UINT currMipWidth = m_uWidth;
	UINT currMipHeight = m_uHeight;
	UINT currMipDepth = m_uDepth;
	UINT mipSlice = bResourceIsBuffer ? 0 : m_uViewMipSlice;
	UINT elementOffset = bResourceIsBuffer ? 0 : m_uViewElementOffset;
	if( !GetMipDimensions(mipSlice, currMipWidth, currMipHeight, currMipDepth) )
	{
		LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}
	WriteToLog( _T("Snapping texels to check against mipslice %u's dimensions: (%u, %u, %u). Depth is not used because RECTs don't have depth."), mipSlice, currMipWidth, currMipHeight, currMipDepth );

	// Empty the set
	texelsToCheck.clear();
	assert( texelsToCheck.size() == 0 );

	//
	// Build the master set of points to check
	//

	// For each rect...
	POINT tempPoint;
	for( UINT r = 0; r < m_pRectList[m_rectScenario].Count; r++ )
	{
		RECT currRect = m_pRectList[m_rectScenario].pRects[r];

		// If using a buffer, we need to get the rect in the space of the buffer view
		if( bResourceIsBuffer )
		{
			currRect.left += m_uViewElementOffset;
			currRect.right += m_uViewElementOffset;
		}

		// Add texels outside the rect...
		// Note we will have to check if these points are actually in the subresource later
		tempPoint.x = currRect.left;		tempPoint.y = currRect.top - 1;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight ); texelsToCheck.insert( tempPoint );
		tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.top - 1;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );

		tempPoint.x = currRect.left - 1;	tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
		tempPoint.x = currRect.right;		tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the right edge is exclusive (outside the rect)

		tempPoint.x = currRect.left - 1;	tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
		tempPoint.x = currRect.right;		tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the right edge is exclusive (outside the rect)

		tempPoint.x = currRect.left;		tempPoint.y = currRect.bottom;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the bottom edge is exclusive (outside the rect)
		tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.bottom;		SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );	// Note that the bottom edge is exclusive (outside the rect)

		// Add texels inside the rect if the rect is empty...
		if( IsRectEmpty( &currRect ) )
		{
			tempPoint.x = currRect.left;		tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
			tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.top;			SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );

			tempPoint.x = currRect.left;		tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
			tempPoint.x = currRect.right - 1;	tempPoint.y = currRect.bottom - 1;	SnapTexelToMipDimensions( tempPoint, currMipWidth, currMipHeight );	texelsToCheck.insert( tempPoint );
		}
	}

	// Now for each point in our set, see if it is inside any of the rects...
	for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); )
	{
		const POINT currPoint = *it;
		bool bPointInsideARect = false;
		for( UINT r = 0; r < m_pRectList[m_rectScenario].Count; r++ )
		{
			// If the RECT is not empty and the point is inside the rect...
			RECT currRect = m_pRectList[m_rectScenario].pRects[r];

			// If using a buffer, we need to get the rect in the space of the buffer view
			if( bResourceIsBuffer )
			{
				currRect.left += m_uViewElementOffset;
				currRect.right += m_uViewElementOffset;
			}

			if( !IsRectEmpty( &currRect ) &&
				PtInRect( &currRect, currPoint ) )
			{
				bPointInsideARect = true;
				break;
			}
		}

		// Remove the point from the set if it is inside at least one rect
		StdPointSet::iterator currIter = it++;
		if( bPointInsideARect )
		{
			texelsToCheck.erase( currIter );
		}
	}

	if( GetFramework()->LogVerbose() )
	{
		WriteToLog( _T("%u RECTs resulted in %u POINTs to check%s"), m_pRectList[m_rectScenario].Count, texelsToCheck.size(), texelsToCheck.size() > 0 ? _T(":") : _T(".") );
		UINT count = 0;
		for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); it++ )
		{
			WriteToLog( _T("  POINT[%u] = (%li, %li)"), count++, it->x, it->y );
		}
	}

	return true;
}

////

bool CDiscardView1Test::VerifySubresourceDataAgainstRects( ID3D11Resource *pResourceToVerify, const std::vector<UINT> &subresourcesToVerify ) const
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	UINT currMipWidth = 0;
	UINT currMipHeight = 0;
	UINT currMipDepth = 0;
	const bool bResourceIsBuffer = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	const UINT numBytesPerElement = bResourceIsBuffer && !m_bTestUsesView ? 1 : CD3D10FormatHelper_Test::GetBitsPerElement( m_SrcFormat ) / 8;
	const bool bResourceHasDepth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D;
	const bool bResourceHasHeight = bResourceHasDepth || 
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	const bool bResourceHasWidth = bResourceHasHeight ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D ||
		m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER;
	ID3D11DeviceContext1 *pImmediateContext1 = GetFramework()->GetImmediateContext1();
	StdPointSet texelsToCheck(g_pfnPointCompareLessThan);
	if( !GetTexelsToCheck( texelsToCheck ) )
	{
		WriteToLog( _T("GetTexelsToCheck() failed.") );
		return false;
	}

	// Copy the resource to a staging copy
	m_pEffectiveContext1->CopyResource( m_pStagingResource, pResourceToVerify );

	if( FAILED( hr = GetFramework()->ExecuteEffectiveContext() ) )
	{
		LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceContextExecute );
		goto done;
	}

	// Go through each array slice...
	for( UINT arraySlice = 0; arraySlice < m_uArraySize; arraySlice++ )
	{
		// Go through each mip level...
		for( UINT mipLevel = 0; mipLevel < m_uMipLevels; mipLevel++ )
		{
			// See if we should verify the current subresource
			const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, m_uMipLevels );
			bool bVerifyCurrentSubresource = false;
			for( UINT i = 0; i < subresourcesToVerify.size(); i++ )
			{
				if( subresourcesToVerify[i] == currSubresource )
				{
					bVerifyCurrentSubresource = true;
					break;
				}
			}

			if( !bVerifyCurrentSubresource )
			{
				// We should not check the current subresource, try the next one
				if( GetFramework()->LogVerbose() ) WriteToLog( _T("Not checking subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );
				continue;
			}

			assert( bVerifyCurrentSubresource );
			BeginSection( L"Checking a subresource..." );
			WriteToLog( _T("About to verify subresource #%d... MipLevel(%d), ArraySlice(%d)."), currSubresource, mipLevel, arraySlice );

			// Get the dimensions for the current subresource
			currMipWidth = m_uWidth;
			currMipHeight = m_uHeight;
			currMipDepth = m_uDepth;
			if( !GetMipDimensions(mipLevel, currMipWidth, currMipHeight, currMipDepth) )
			{
				LogError( __FILEW__, __LINE__, L"GetMipDimensions() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
				EndSection();
				goto done;
			}

			WriteToLog( _T("Expected dimensions for this subresource are (%d, %d, %d)."), currMipWidth, currMipHeight, currMipDepth );
			WriteToLog( _T("Checking to verify the subresource contains a %#x pattern for different elements."), m_ResourceBitPattern );
			WriteToLog( _T("Each element is %d byte(s)."), numBytesPerElement );

			// Map the subresource
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			ZeroMemory( &mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE) );
			if( FAILED( hr = pImmediateContext1->Map( m_pStagingResource, currSubresource, D3D11_MAP_READ, 0, &mappedSubresource ) ) )
			{
				LogError( __FILEW__, __LINE__, L"Failed to map m_pStagingResource for read.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
				EndSection();
				goto done;
			}

			//
			// Verify the subresource contains the expected bit pattern around the rects
			//
			BYTE *pByteData = reinterpret_cast<BYTE*>(mappedSubresource.pData);

			// Check the first and last depth slice...
			for( UINT d = 0; d < (UINT)(bResourceHasDepth ? 2 : 1); d++ )
			{
				const UINT currDepthSlice = d * (currMipDepth - 1);

				// Go through each texel and verify it matches the expected bit pattern
				for( StdPointSet::iterator it = texelsToCheck.begin(); it != texelsToCheck.end(); it++ )
				{
					const POINT currTexel = *it;

					// Compute the index of the element to read
					const UINT z = d * (currMipDepth - 1);
					const UINT y = currTexel.y;
					const UINT x = currTexel.x;

					WriteToLog( _T("Reading element (%d, %d, %d) from mapped memory and checking all %d byte(s)..."), x, y, z, numBytesPerElement );
					const UINT elementByteOffset = (z * mappedSubresource.DepthPitch) + (y * mappedSubresource.RowPitch) + (x * numBytesPerElement);
						
					// Check every byte in the element...
					for( UINT byte = 0; byte < numBytesPerElement; byte++ )
					{
						// Check to see if the byte for the current element is valid
						// E.g DXGI_FORMAT_D32_FLOAT_S8X24_UINT is a 64-bit format, where the last 3 bytes are junk
						switch( m_SrcFormat )
						{
						case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
							{
								assert(8 == numBytesPerElement);
								if( byte >= 5 ) // 5, 6, and 7 are junk
								{
									continue;
								}
							}
						}

						// Compute the offset in the mapped memory for the desired element and check it against the bit pattern
						const UINT byteOffset = elementByteOffset + byte;
						const BYTE mappedByteValue = pByteData[byteOffset];
						if( mappedByteValue != m_ResourceBitPattern )
						{
							WriteToLog( _T("DepthPitch = %d. RowPitch = %d. pData = %#x"), mappedSubresource.DepthPitch, mappedSubresource.RowPitch, mappedSubresource.pData);
							WriteToLog( _T("Element (%d, %d, %d) is %d byte(s) in the mapped staging memory (pData)."), x, y, z, elementByteOffset );
							WriteToLog( _T("Byte #%d of the element was %#x. The expected bit pattern is %#x."), byte, mappedByteValue, m_ResourceBitPattern );
							LogError( __FILEW__, __LINE__, L"The mapped byte did not match the expected bit pattern.", 0, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
							pImmediateContext1->Unmap( m_pStagingResource, currSubresource );
							EndSection();
							goto done;
						}
					}
				}
			}

			// Unmap the subresource
			pImmediateContext1->Unmap( m_pStagingResource, currSubresource );

			// If we got here, the subresource passed, so discard the output
			EndSection(false);
			WriteToLog( _T("Subresource #%d, MipLevel(%d), ArraySlice(%d), passed the test."), currSubresource, mipLevel, arraySlice );
		}
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

void CDiscardView1Test::WriteRectScenarioToLog( const SRectScenarios &rectScenario ) const
{
	for( UINT i = 0; i < rectScenario.Count; i++ )
	{
		WriteToLog( _T("Rect #%d: (L = %ld, T = %ld, R = %ld, B = %ld)"), i,
					rectScenario.pRects[i].left, rectScenario.pRects[i].top, rectScenario.pRects[i].right, rectScenario.pRects[i].bottom );
	}
}

////////////////////////
// CDiscardDSViewTest

void CDiscardDSViewTest::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewFactorCombinations();

	m_rResourceViewDSVFlags_None = AddParameter<D3D11_DSV_FLAG>( _T( g_resourceViewDSVFlagStr ), &m_DSVFlags, (D3D11_DSV_FLAG)0 );
	m_rResourceViewDSVFlags_RODepth = AddParameterValue( _T( g_resourceViewDSVFlagStr ), D3D11_DSV_READ_ONLY_DEPTH  );
	m_rResourceViewDSVFlags_ROStencil = AddParameterValue( _T( g_resourceViewDSVFlagStr ), D3D11_DSV_READ_ONLY_STENCIL  );
	m_rResourceViewDSVFlags = m_rResourceViewDSVFlags_None + m_rResourceViewDSVFlags_RODepth + m_rResourceViewDSVFlags_ROStencil;
	SetParameterDefaultValue<D3D11_DSV_FLAG>( _T( g_resourceViewDSVFlagStr ), (D3D11_DSV_FLAG)0 );

	testfactor::RFactor rResourceDepthFormatsDSVFlags = (m_rResourceDepthFormats + m_rResourceStencilFormats) * m_rResourceViewDSVFlags_RODepth;
	testfactor::RFactor rResourceStencilFormatsDSVFlags = m_rResourceStencilFormats * m_rResourceViewDSVFlags_ROStencil;
	testfactor::RFactor rResourceDSFormatDSVFlags = rResourceDepthFormatsDSVFlags + rResourceStencilFormatsDSVFlags;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D_DS = NewCrossFactor(tex1DPairwiseTableDS, _countof(tex1DPairwiseTableDS), _T("tex1DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D_DS * m_rResourceBindFlagDS * m_rResourceWidth);	// DS resources have specific formats

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS = NewCrossFactor(tex1DArrayPairwiseTableDS, _countof(tex1DArrayPairwiseTableDS), _T("tex1DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS * m_rResourceBindFlagDS * m_rResourceWidth);	// DS resources have specific formats

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D_DS = NewCrossFactor(tex2DPairwiseTableDS, _countof(tex2DPairwiseTableDS), _T("tex2DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D_DS * m_rResourceBindFlagDS * m_rResourceHeight);	// DS resources have specific formats

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS = NewCrossFactor(tex2DArrayPairwiseTableDS, _countof(tex2DArrayPairwiseTableDS), _T("tex2DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS * m_rResourceBindFlagDS * m_rResourceHeight);	// DS resources have specific formats

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS = NewCrossFactor(tex2DMSPairwiseTableDS, _countof(tex2DMSPairwiseTableDS), _T("tex2DMSPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS = NewCrossFactor(tex2DMSArrayPairwiseTableDS, _countof(tex2DMSArrayPairwiseTableDS), _T("tex2DMSArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest =
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Set the test factor and priorities
	SetRootTestFactor(
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest );
}

////

bool CDiscardDSViewTest::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	const UINT mipSlice = m_uViewMipSlice;
	const UINT firstArraySlice = m_uViewFirstArraySlice;
	const UINT arraySize = m_uViewArraySize;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	dsvDesc.Format = m_SrcFormat;
	dsvDesc.Flags = (m_pDevice1->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? m_DSVFlags : 0;
	if( !GetViewDimension( m_ResourceScenario, &dsvDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( dsvDesc.ViewDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE2DMS:
		break;	// MS have a single subresource. Nothing to define
	case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
		dsvDesc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
		dsvDesc.Texture2DMSArray.ArraySize = arraySize;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1D:
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		dsvDesc.Texture1D.MipSlice = mipSlice;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		dsvDesc.Texture1DArray.MipSlice = mipSlice;
		dsvDesc.Texture1DArray.ArraySize = arraySize;
		dsvDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
		break;
	}

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateDepthStencilView( m_pResourceToDiscard, &dsvDesc, (ID3D11DepthStencilView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateDepthStencilView( m_pDummyResourceNoDiscard, &dsvDesc, (ID3D11DepthStencilView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardRTViewTest

void CDiscardRTViewTest::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewFactorCombinations();

	testfactor::XFACTOR defaultUsageTex3DPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Texture3D's don't have arrays
		{m_rResourceViewDepthFirstWSliceWSizeMipSlice, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::RFactor rDefaultUsageCPU_Tex3D = NewCrossFactor(defaultUsageTex3DPairwiseTable, _countof(defaultUsageTex3DPairwiseTable), _T("defaultUsageTex3DPairwiseTable"), groupOrders, _countof(groupOrders));

	// Buffers
	testfactor::XFACTOR bufferPairwiseTable[] =
	{
		{m_rResourceScenarioBuffer, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceViewWidthElementOffsetElementWidth},	// Width (in bytes), element offset
		{m_rDefaultUsageCPU_Buf, 1, NULL, 0},	// Usage, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},		// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageCPU_Buf = NewCrossFactor(bufferPairwiseTable, _countof(bufferPairwiseTable), _T("bufferPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rBufferTest = rScenarioWidthUsageCPU_Buf * m_rResourceBindFlagRT;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTable[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D = NewCrossFactor(tex1DPairwiseTable, _countof(tex1DPairwiseTable), _T("tex1DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D * m_rResourceBindFlagRT * m_rResourceWidth);

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray = NewCrossFactor(tex1DArrayPairwiseTable, _countof(tex1DArrayPairwiseTable), _T("tex1DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray * m_rResourceBindFlagRT * m_rResourceWidth);

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D = NewCrossFactor(tex2DPairwiseTable, _countof(tex2DPairwiseTable), _T("tex2DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D * m_rResourceBindFlagRT * m_rResourceHeight);

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray = NewCrossFactor(tex2DArrayPairwiseTable, _countof(tex2DArrayPairwiseTable), _T("tex2DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray * m_rResourceBindFlagRT * m_rResourceHeight);

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS = NewCrossFactor(tex2DMSPairwiseTable, _countof(tex2DMSPairwiseTable), _T("tex2DMSPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS * m_rResourceBindFlagRT);

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray = NewCrossFactor(tex2DMSArrayPairwiseTable, _countof(tex2DMSArrayPairwiseTable), _T("tex2DMSArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest =
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray * m_rResourceBindFlagRT);

	// Texture3D
	testfactor::XFACTOR tex3DPairwiseTable[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		// Depth/Mip levels has already been combined in rDefaultUsageCPU_Tex3D
		{rDefaultUsageCPU_Tex3D, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D = NewCrossFactor(tex3DPairwiseTable, _countof(tex3DPairwiseTable), _T("tex3DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture3DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex3D * m_rResourceBindFlagRT);

	// Set the test factor and priorities
	SetRootTestFactor(
		rBufferTest +
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest +
		rTexture3DTest
		);
}

////

bool CDiscardRTViewTest::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	rtvDesc.Format = m_SrcFormat;
	if( !GetViewDimension( m_ResourceScenario, &rtvDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( rtvDesc.ViewDimension )
	{
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
		break;	// MS have a single subresource. Nothing to define
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
		rtvDesc.Texture2DMSArray.FirstArraySlice = m_uViewFirstArraySlice;
		rtvDesc.Texture2DMSArray.ArraySize = m_uViewArraySize;
		break;
	case D3D11_RTV_DIMENSION_BUFFER:
		rtvDesc.Buffer.ElementOffset = m_uViewElementOffset;
		rtvDesc.Buffer.ElementWidth = m_uViewElementWidth;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1D:
	case D3D11_RTV_DIMENSION_TEXTURE2D:
		rtvDesc.Texture1D.MipSlice = m_uViewMipSlice;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
		rtvDesc.Texture1DArray.MipSlice = m_uViewMipSlice;
		rtvDesc.Texture1DArray.ArraySize = m_uViewArraySize;
		rtvDesc.Texture1DArray.FirstArraySlice = m_uViewFirstArraySlice;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE3D:
		rtvDesc.Texture3D.MipSlice = m_uViewMipSlice;
		rtvDesc.Texture3D.FirstWSlice = m_uViewFirstWSlice;
		rtvDesc.Texture3D.WSize = m_uViewWSize;
	}

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateRenderTargetView( m_pResourceToDiscard, &rtvDesc, (ID3D11RenderTargetView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateRenderTargetView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateRenderTargetView( m_pDummyResourceNoDiscard, &rtvDesc, (ID3D11RenderTargetView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardSRViewTest

void CDiscardSRViewTest::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewParameterFactors();

	//
	// Create pairwise sets for common resource combinations
	//

	testfactor::XFACTOR defaultUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlags, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},					// No array
		{m_ResourceViewMostDetailedMipMipLevels, 1, NULL, 0},		// Multiple mips
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice, 1, NULL, 0},					// Multiple array slices
		{m_ResourceViewMostDetailedMipMipLevels, 1, NULL, 0},		// Multiple mips
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexArrayPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice, 1, NULL, 0},					// Multiple array slices
		{m_ResourceViewMostDetailedMipMipLevels_1_0_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},					// No array
		{m_ResourceViewMostDetailedMipMipLevels_1_0_1, 1, NULL, 0},	// Only 1 mip
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR defaultUsageMipsCubePairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		{m_rResourceViewArraySlice_6, 1, NULL, 0},
		{m_ResourceViewMostDetailedMipMipLevels, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageBufPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		// Buffer's don't have arrays
		// Buffer's don't have mips
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	testfactor::XFACTOR dynamicUsageTexPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDynamic, 1, NULL, 0},
		{m_rResourceViewArraySlice_1, 1, NULL, 0},					// Dynamic resources must have 1 array slice
		{m_ResourceViewMostDetailedMipMipLevels_1_0_1, 1, NULL, 0},	// Dynamic resources must have 1 mip
		{m_rCPUAccessFlagWrite, 1, NULL, 0},
	};

	m_rDefaultUsageCPU_Buf			= NewCrossFactor(defaultUsageBufPairwiseTable, _countof(defaultUsageBufPairwiseTable), _T("defaultUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageMipCPU_Tex		= NewCrossFactor(defaultUsageMipsTexPairwiseTable, _countof(defaultUsageMipsTexPairwiseTable), _T("defaultUsageMipsTexPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayMipCPU_Tex	= NewCrossFactor(defaultUsageMipsTexArrayPairwiseTable, _countof(defaultUsageMipsTexArrayPairwiseTable), _T("defaultUsageMipsTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageArrayCPU_Tex		= NewCrossFactor(defaultUsageTexArrayPairwiseTable, _countof(defaultUsageTexArrayPairwiseTable), _T("defaultUsageTexArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageCPU_Tex			= NewCrossFactor(defaultUsageTexPairwiseTable, _countof(defaultUsageTexPairwiseTable), _T("defaultUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDefaultUsageMipCPU_Cube		= NewCrossFactor(defaultUsageMipsCubePairwiseTable, _countof(defaultUsageMipsCubePairwiseTable), _T("defaultUsageMipsCubePairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDynamicUsageCPU_Buf			= NewCrossFactor(dynamicUsageBufPairwiseTable, _countof(dynamicUsageBufPairwiseTable), _T("dynamicUsageBufPairwiseTable"), groupOrders, _countof(groupOrders));
	m_rDynamicUsageArrayMipCPU_Tex	= NewCrossFactor(dynamicUsageTexPairwiseTable, _countof(dynamicUsageTexPairwiseTable), _T("dynamicUsageTexPairwiseTable"), groupOrders, _countof(groupOrders));

	testfactor::RFactor rResourceBindFlagSR = m_rResourceBindFlagSR;
	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		// Feature level 9 requires SR 2D textures (3D textures cannot be RT for FL9.X) to also have the RT bind flag for CopyResource to work correctly
		rResourceBindFlagSR = m_rResourceBindFlagSRRT;
	}

	// Buffers
	testfactor::XFACTOR bufferPairwiseTable[] =
	{
		{m_rResourceScenarioBuffer, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceViewWidthElementOffsetElementWidth},	// Width (in bytes), element offset
		{m_rDefaultUsageCPU_Buf + m_rDynamicUsageCPU_Buf, 1, NULL, 0},	// Usage, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},		// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageCPU_Buf = NewCrossFactor(bufferPairwiseTable, _countof(bufferPairwiseTable), _T("bufferPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rBufferTest = rScenarioWidthUsageCPU_Buf * rResourceBindFlagSR;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTable[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D = NewCrossFactor(tex1DPairwiseTable, _countof(tex1DPairwiseTable), _T("tex1DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D * rResourceBindFlagSR * m_rResourceWidth);

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray = NewCrossFactor(tex1DArrayPairwiseTable, _countof(tex1DArrayPairwiseTable), _T("tex1DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray * rResourceBindFlagSR * m_rResourceWidth);

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D = NewCrossFactor(tex2DPairwiseTable, _countof(tex2DPairwiseTable), _T("tex2DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D * rResourceBindFlagSR * m_rResourceHeight);

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray = NewCrossFactor(tex2DArrayPairwiseTable, _countof(tex2DArrayPairwiseTable), _T("tex2DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray * rResourceBindFlagSR * m_rResourceHeight);

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS = NewCrossFactor(tex2DMSPairwiseTable, _countof(tex2DMSPairwiseTable), _T("tex2DMSPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS * rResourceBindFlagSR);

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray = NewCrossFactor(tex2DMSArrayPairwiseTable, _countof(tex2DMSArrayPairwiseTable), _T("tex2DMSArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest =
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray * rResourceBindFlagSR);

	// Texture3D
	testfactor::XFACTOR tex3DPairwiseTable[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		// Depth will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex + m_rDynamicUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D = NewCrossFactor(tex3DPairwiseTable, _countof(tex3DPairwiseTable), _T("tex3DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture3DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex3D * m_rResourceBindFlagSR * m_rResourceDepth);

	// Texture Cube
	testfactor::XFACTOR texCubePairwiseTable[] =
	{
		{m_rResourceScenarioTexCube, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageMipCPU_Cube, 1, NULL, 0},	// Usage, Array size (Set to 6), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rResourceMiscFlag_TexCube, 1, NULL, 0},	// Texture cube
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_TexCube = NewCrossFactor(texCubePairwiseTable, _countof(texCubePairwiseTable), _T("texCubePairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTextureCubeTest = 
		(rScenarioWidthUsageArrayMipCPU_TexCube * rResourceBindFlagSR);

	// Set the test factor and priorities
	SetRootTestFactor(
		rBufferTest +
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest +
		rTextureCubeTest +
		rTexture3DTest
		);
}

////

bool CDiscardSRViewTest::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = m_SrcFormat;
	if( !GetViewDimension( m_ResourceScenario, &srvDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( srvDesc.ViewDimension )
	{
	default:
		assert( !"Unknown Dimension" );
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2DMS:
		break;	// MS have a single subresource. Nothing to define
	case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
		srvDesc.Texture2DMSArray.FirstArraySlice = m_uViewFirstArraySlice;
		srvDesc.Texture2DMSArray.ArraySize = m_uViewArraySize;
		break;
	case D3D11_SRV_DIMENSION_BUFFER:
		srvDesc.Buffer.ElementOffset = m_uViewElementOffset;
		srvDesc.Buffer.ElementWidth = m_uViewElementWidth;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE1D:
	case D3D11_SRV_DIMENSION_TEXTURE2D:
	case D3D11_SRV_DIMENSION_TEXTURE3D:
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		srvDesc.Texture1D.MostDetailedMip = m_uViewMostDetailedMip;
		srvDesc.Texture1D.MipLevels = m_uViewMipLevels;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
		srvDesc.Texture1DArray.MostDetailedMip = m_uViewMostDetailedMip;
		srvDesc.Texture1DArray.MipLevels = m_uViewMipLevels;
		srvDesc.Texture1DArray.ArraySize = m_uViewArraySize;
		srvDesc.Texture1DArray.FirstArraySlice = m_uViewFirstArraySlice;
		break;
	}

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateShaderResourceView( m_pResourceToDiscard, &srvDesc, (ID3D11ShaderResourceView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateShaderResourceView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateShaderResourceView( m_pDummyResourceNoDiscard, &srvDesc, (ID3D11ShaderResourceView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateShaderResourceView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardSRViewTest::GetSubresourcesInsideView( std::vector<UINT> &subresourcesInsideView, ID3D11Resource *pResource ) const
{
	// Local variables
	bool success = false;
	UINT numArraySlicesInResource = 0;
	UINT numMiplevelsInResource = 0;

	subresourcesInsideView.clear();
	assert( subresourcesInsideView.empty() );

	// Get the array size and number of mip levels in the resource
	if( !GetArraySizeAndMipLevelsFromResource( pResource, numArraySlicesInResource, numMiplevelsInResource ) )
	{
		WriteToLog( _T("GetArraySizeAndMipLevelsFromResource() failed.") );
		goto done;
	}

	switch( m_ResourceScenario )
	{
	default:
		assert( !"Unknown scenario" );
		break;
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			subresourcesInsideView.push_back(0);
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			// Go through each mip level...
			for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
			{
				// See if we should verify the current subresource
				const UINT currSubresource = D3D11CalcSubresource( mipLevel, 0, numMiplevelsInResource );
				
				// Make sure we are on a mip view included in the view
				if( mipLevel >= m_uViewMostDetailedMip &&
					mipLevel < (m_uViewMostDetailedMip + m_uViewMipLevels) )
				{
					// Add the subresource...
					subresourcesInsideView.push_back( currSubresource );
				}
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			// Go through each mip level...
			for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
			{
				// See if we should verify the current subresource
				const UINT currSubresource = D3D11CalcSubresource( mipLevel, 0, numMiplevelsInResource );
				
				// Make sure we are on a mip view included in the view
				if( mipLevel >= m_uViewMostDetailedMip &&
					mipLevel < (m_uViewMostDetailedMip + m_uViewMipLevels) )
				{
					// Add the subresource...
					// Note each set of subresource is replicated for each face of the texture cube
					subresourcesInsideView.push_back( currSubresource + 0 * numMiplevelsInResource );
					subresourcesInsideView.push_back( currSubresource + 1 * numMiplevelsInResource );
					subresourcesInsideView.push_back( currSubresource + 2 * numMiplevelsInResource );
					subresourcesInsideView.push_back( currSubresource + 3 * numMiplevelsInResource );
					subresourcesInsideView.push_back( currSubresource + 4 * numMiplevelsInResource );
					subresourcesInsideView.push_back( currSubresource + 5 * numMiplevelsInResource );
				}
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		{
			// Go through each array slice...
			for( UINT arraySlice = 0; arraySlice < numArraySlicesInResource; arraySlice++ )
			{
				// Make sure we are on a array slice included in the view
				if( arraySlice >= m_uViewFirstArraySlice &&
					arraySlice < (m_uViewFirstArraySlice + m_uViewArraySize) )
				{
					// Go through each mip level...
					for( UINT mipLevel = 0; mipLevel < numMiplevelsInResource; mipLevel++ )
					{
						// See if we should verify the current subresource
						const UINT currSubresource = D3D11CalcSubresource( mipLevel, arraySlice, numMiplevelsInResource );
						
						// Make sure we are on a mip view included in the view
						if( mipLevel >= m_uViewMostDetailedMip &&
							mipLevel < (m_uViewMostDetailedMip + m_uViewMipLevels) )
						{
							// Add the subresource...
							subresourcesInsideView.push_back( currSubresource );
						}
					}
				}
			}
		}
		break;
	};

	// If we got here, we passed
	success = true;

done:
	return success;
}

////

bool CDiscardSRViewTest::GetSubresourcesPartiallyCoveredByView( std::vector<UINT> &subresourcesPartiallyCoveredByView, ID3D11Resource *pResource ) const
{
	// Local variables
	bool success = false;

	subresourcesPartiallyCoveredByView.clear();
	assert( subresourcesPartiallyCoveredByView.empty() );

	// Only buffers can partially cover a subresource for SRV's
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			// If the element offset is not 0, then we are leaving some parts of the buffer outside the view
			// We need to check the bytes from [0,m_uViewElementOffset) 
			if( m_uViewElementOffset != 0 )
			{
				WriteToLog( _T("The view partially covers subresource #0. ElementOffset = %d."), m_uViewElementOffset );
				subresourcesPartiallyCoveredByView.push_back( 0 );
			}
			else if( m_uViewElementOffset == 0 && m_uViewElementWidth < m_uWidth )
			{
				WriteToLog( _T("The view partially covers subresource #0. ElementOffset = %d and ElementWidth = %d but the width is %d"), m_uViewElementOffset, m_uViewElementWidth, m_uWidth );
				subresourcesPartiallyCoveredByView.push_back( 0 );
			}
		}
		break;
	}

	return true;
}

////////////////////////
// CDiscardUAViewTest

void CDiscardUAViewTest::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewFactorCombinations();

	testfactor::XFACTOR defaultUsageTex3DPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Texture3D's don't have arrays
		{m_rResourceViewDepthFirstWSliceWSizeMipSlice, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::RFactor rDefaultUsageCPU_Tex3D = NewCrossFactor(defaultUsageTex3DPairwiseTable, _countof(defaultUsageTex3DPairwiseTable), _T("defaultUsageTex3DPairwiseTable"), groupOrders, _countof(groupOrders));

	// Buffers
	testfactor::XFACTOR bufferPairwiseTable[] =
	{
		{m_rResourceScenarioBuffer, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceViewWidthElementOffsetElementWidth},	// Width (in bytes), element offset
		{m_rDefaultUsageCPUAll_Buf, 1, NULL, 0},	// Usage, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},		// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageCPU_Buf = NewCrossFactor(bufferPairwiseTable, _countof(bufferPairwiseTable), _T("bufferPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rBufferTest = rScenarioWidthUsageCPU_Buf * m_rResourceBindFlagUA;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTable[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D = NewCrossFactor(tex1DPairwiseTable, _countof(tex1DPairwiseTable), _T("tex1DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D * m_rResourceBindFlagUA * m_rResourceWidth);

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray = NewCrossFactor(tex1DArrayPairwiseTable, _countof(tex1DArrayPairwiseTable), _T("tex1DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray * m_rResourceBindFlagUA * m_rResourceWidth);

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D = NewCrossFactor(tex2DPairwiseTable, _countof(tex2DPairwiseTable), _T("tex2DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D * m_rResourceBindFlagUA * m_rResourceHeight);

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray = NewCrossFactor(tex2DArrayPairwiseTable, _countof(tex2DArrayPairwiseTable), _T("tex2DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray * m_rResourceBindFlagUA * m_rResourceHeight);

	// Texture3D
	testfactor::XFACTOR tex3DPairwiseTable[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_16, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_16, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		// Depth/Mip levels has already been combined in rDefaultUsageCPU_Tex3D
		{rDefaultUsageCPU_Tex3D, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D = NewCrossFactor(tex3DPairwiseTable, _countof(tex3DPairwiseTable), _T("tex3DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture3DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex3D * m_rResourceBindFlagUA);

	// Set the test factor and priorities
	SetRootTestFactor(
		rBufferTest +
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture3DTest
		);
}

////

bool CDiscardUAViewTest::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

	uavDesc.Format = m_SrcFormat;
	if( !GetViewDimension( m_ResourceScenario, &uavDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( uavDesc.ViewDimension )
	{
	case D3D11_UAV_DIMENSION_BUFFER:
		uavDesc.Buffer.FirstElement = m_uViewElementOffset;
		uavDesc.Buffer.NumElements = m_uViewElementWidth;
		uavDesc.Buffer.Flags = (m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0) ? D3D11_BUFFER_UAV_FLAG_RAW : 0;
		break;

	case D3D11_UAV_DIMENSION_TEXTURE1D:
	case D3D11_UAV_DIMENSION_TEXTURE2D:
		uavDesc.Texture1D.MipSlice = m_uViewMipSlice;
		break;

	case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
		uavDesc.Texture1DArray.MipSlice = m_uViewMipSlice;
		uavDesc.Texture1DArray.ArraySize = m_uViewArraySize;
		uavDesc.Texture1DArray.FirstArraySlice = m_uViewFirstArraySlice;
		break;

	case D3D11_UAV_DIMENSION_TEXTURE3D:
		uavDesc.Texture3D.MipSlice = m_uViewMipSlice;
		uavDesc.Texture3D.FirstWSlice = m_uViewFirstWSlice;
		uavDesc.Texture3D.WSize = m_uViewWSize;
		break;
	}

	WriteToLog( _T("UAV was created with Format %s."), ToString(uavDesc.Format).c_str() );

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateUnorderedAccessView( m_pResourceToDiscard, &uavDesc, (ID3D11UnorderedAccessView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateUnorderedAccessView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateUnorderedAccessView( m_pDummyResourceNoDiscard, &uavDesc, (ID3D11UnorderedAccessView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateUnorderedAccessView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardDSView1Test

void CDiscardDSView1Test::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewFactorCombinations();
	GenerateCommonView1ParameterFactors();

	m_rResourceViewDSVFlags_None = AddParameter<D3D11_DSV_FLAG>( _T( g_resourceViewDSVFlagStr ), &m_DSVFlags, (D3D11_DSV_FLAG)0 );
	m_rResourceViewDSVFlags_RODepth = AddParameterValue( _T( g_resourceViewDSVFlagStr ), D3D11_DSV_READ_ONLY_DEPTH  );
	m_rResourceViewDSVFlags_ROStencil = AddParameterValue( _T( g_resourceViewDSVFlagStr ), D3D11_DSV_READ_ONLY_STENCIL  );
	m_rResourceViewDSVFlags = m_rResourceViewDSVFlags_None + m_rResourceViewDSVFlags_RODepth + m_rResourceViewDSVFlags_ROStencil;
	SetParameterDefaultValue<D3D11_DSV_FLAG>( _T( g_resourceViewDSVFlagStr ), (D3D11_DSV_FLAG)0 );

	testfactor::RFactor rResourceDepthFormatsDSVFlags = (m_rResourceDepthFormats + m_rResourceStencilFormats) * m_rResourceViewDSVFlags_RODepth;
	testfactor::RFactor rResourceStencilFormatsDSVFlags = m_rResourceStencilFormats * m_rResourceViewDSVFlags_ROStencil;
	testfactor::RFactor rResourceDSFormatDSVFlags = rResourceDepthFormatsDSVFlags + rResourceStencilFormatsDSVFlags;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D_DS = NewCrossFactor(tex1DPairwiseTableDS, _countof(tex1DPairwiseTableDS), _T("tex1DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D_DS * m_rResourceBindFlagDS * m_rResourceWidth_64);	// DS resources have specific formats

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS = NewCrossFactor(tex1DArrayPairwiseTableDS, _countof(tex1DArrayPairwiseTableDS), _T("tex1DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray_DS * m_rResourceBindFlagDS * m_rResourceWidth_64);	// DS resources have specific formats

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D_DS = NewCrossFactor(tex2DPairwiseTableDS, _countof(tex2DPairwiseTableDS), _T("tex2DPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D_DS * m_rResourceBindFlagDS * m_rResourceHeight_64);	// DS resources have specific formats

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS = NewCrossFactor(tex2DArrayPairwiseTableDS, _countof(tex2DArrayPairwiseTableDS), _T("tex2DArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray_DS * m_rResourceBindFlagDS * m_rResourceHeight_64);	// DS resources have specific formats

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_64, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS = NewCrossFactor(tex2DMSPairwiseTableDS, _countof(tex2DMSPairwiseTableDS), _T("tex2DMSPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTableDS[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{rResourceDSFormatDSVFlags, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_64, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS = NewCrossFactor(tex2DMSArrayPairwiseTableDS, _countof(tex2DMSArrayPairwiseTableDS), _T("tex2DMSArrayPairwiseTableDS"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest =
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray_DS * m_rResourceBindFlagDS);	// DS resources have specific formats

	// Set the test factor and priorities
	SetRootTestFactor(
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest );
}

////

bool CDiscardDSView1Test::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	const UINT mipSlice = m_uViewMipSlice;
	const UINT firstArraySlice = m_uViewFirstArraySlice;
	const UINT arraySize = m_uViewArraySize;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	dsvDesc.Format = m_SrcFormat;
	dsvDesc.Flags = (m_pDevice1->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? m_DSVFlags : 0;
	if( !GetViewDimension( m_ResourceScenario, &dsvDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( dsvDesc.ViewDimension )
	{
	case D3D11_DSV_DIMENSION_TEXTURE2DMS:
		break;	// MS have a single subresource. Nothing to define
	case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
		dsvDesc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
		dsvDesc.Texture2DMSArray.ArraySize = arraySize;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1D:
	case D3D11_DSV_DIMENSION_TEXTURE2D:
		dsvDesc.Texture1D.MipSlice = mipSlice;
		break;
	case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
		dsvDesc.Texture1DArray.MipSlice = mipSlice;
		dsvDesc.Texture1DArray.ArraySize = arraySize;
		dsvDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
		break;
	}

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateDepthStencilView( m_pResourceToDiscard, &dsvDesc, (ID3D11DepthStencilView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateDepthStencilView( m_pDummyResourceNoDiscard, &dsvDesc, (ID3D11DepthStencilView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}

////////////////////////
// CDiscardRTView1Test

void CDiscardRTView1Test::InitTestParameters()
{
	// Local variables
	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise, thus 2 = pairwise
	};

	//
	// Add parameters and value sets
	//

	GenerateCommonResourceParameterFactors();
	GenerateCommonViewFactorCombinations();
	GenerateCommonView1ParameterFactors();

	testfactor::XFACTOR defaultUsageTex3DPairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{m_rResourceUsageDefault, 1, NULL, 0},
		// Texture3D's don't have arrays
		{m_rResourceViewDepthFirstWSliceWSizeMipSlice, 1, NULL, 0},
		{m_rCPUAccessFlagNone, 1, NULL, 0},
	};

	testfactor::RFactor rDefaultUsageCPU_Tex3D = NewCrossFactor(defaultUsageTex3DPairwiseTable, _countof(defaultUsageTex3DPairwiseTable), _T("defaultUsageTex3DPairwiseTable"), groupOrders, _countof(groupOrders));

	// Buffers
	testfactor::XFACTOR bufferPairwiseTable[] =
	{
		{m_rResourceScenarioBuffer, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceViewWidth64ElementOffsetElementWidth},	// Width (in bytes), element offset
		{m_rDefaultUsageCPU_Buf, 1, NULL, 0},	// Usage, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},		// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageCPU_Buf = NewCrossFactor(bufferPairwiseTable, _countof(bufferPairwiseTable), _T("bufferPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rBufferTest = rScenarioWidthUsageCPU_Buf * m_rResourceBindFlagRT;

	// Texture1D
	testfactor::XFACTOR tex1DPairwiseTable[] =
	{
		{m_rResourceScenarioTex1D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1D = NewCrossFactor(tex1DPairwiseTable, _countof(tex1DPairwiseTable), _T("tex1DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DTest =
		(rScenarioWidthUsageArrayMipCPU_Tex1D * m_rResourceBindFlagRT * m_rResourceWidth_64);

	// Texture1D Array
	testfactor::XFACTOR tex1DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex1DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		// Width will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex1DArray = NewCrossFactor(tex1DArrayPairwiseTable, _countof(tex1DArrayPairwiseTable), _T("tex1DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture1DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex1DArray * m_rResourceBindFlagRT * m_rResourceWidth_64);

	// Texture2D
	testfactor::XFACTOR tex2DPairwiseTable[] =
	{
		{m_rResourceScenarioTex2D, 1, NULL, 0},
		{m_rResourceFormats + m_rResourceVideoFormats, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2D = NewCrossFactor(tex2DPairwiseTable, _countof(tex2DPairwiseTable), _T("tex2DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2D * m_rResourceBindFlagRT * m_rResourceHeight_64);

	// Texture2D Array
	testfactor::XFACTOR tex2DArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		// Height will be combined later due to mip level restrictions
		{m_rDefaultUsageArrayMipCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DArray = NewCrossFactor(tex2DArrayPairwiseTable, _countof(tex2DArrayPairwiseTable), _T("tex2DArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DArrayTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DArray * m_rResourceBindFlagRT * m_rResourceHeight_64);

	// Texture2D MS
	testfactor::XFACTOR tex2DMSPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMS, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_64, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageCPU_Tex, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},		// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},				// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMS = NewCrossFactor(tex2DMSPairwiseTable, _countof(tex2DMSPairwiseTable), _T("tex2DMSPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex2DMS * m_rResourceBindFlagRT);

	// Texture2D MS Array
	testfactor::XFACTOR tex2DMSArrayPairwiseTable[] =
	{
		{m_rResourceScenarioTex2DMSArray, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_64, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		{m_rDefaultUsageArrayCPU_Tex, 1, NULL, 0},	// Usage, Array size, Mip levels (Set to 1), CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_2, 1, NULL, 0},	// Multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData_False, 1, NULL, 0},	// A Multisampled Resource cannot initialize its contents during creation
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex2DMSArray = NewCrossFactor(tex2DMSArrayPairwiseTable, _countof(tex2DMSArrayPairwiseTable), _T("tex2DMSArrayPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture2DMSArrayTest =
		(rScenarioWidthUsageArrayMipCPU_Tex2DMSArray * m_rResourceBindFlagRT);

	// Texture3D
	testfactor::XFACTOR tex3DPairwiseTable[] =
	{
		{m_rResourceScenarioTex3D, 1, NULL, 0},
		{m_rResourceFormats, 1, NULL, 0},
		{m_rResourceWidth_64, 1, NULL, 0},	// No cycling on width, this was done for Tex1D
		{m_rResourceHeight_64, 1, NULL, 0},	// No cycling on height, this was done for Tex2D
		// Depth/Mip levels has already been combined in rDefaultUsageCPU_Tex3D
		{rDefaultUsageCPU_Tex3D, 1, NULL, 0},	// Usage, Array size (Set to 1), Mip levels, CPU access flags
		// Bind flags will be combined later
		{m_rResourceSampleCount_1, 1, NULL, 0},	// No multisampling
		{m_rResourceSampleQuality_0, 1, NULL, 0},
		{m_rDoubleDiscard, 1, NULL, 0},	// Double discard
		{m_rInitResourceData, 1, NULL, 0},	// Init data during device create or map and update later
		{m_rfRectScenario, 1, NULL, 0},
	};

	testfactor::RFactor rScenarioWidthUsageArrayMipCPU_Tex3D = NewCrossFactor(tex3DPairwiseTable, _countof(tex3DPairwiseTable), _T("tex3DPairwiseTable"), groupOrders, _countof(groupOrders));
	testfactor::RFactor rTexture3DTest = 
		(rScenarioWidthUsageArrayMipCPU_Tex3D * m_rResourceBindFlagRT);

	// Set the test factor and priorities
	SetRootTestFactor(
		rBufferTest +
		rTexture1DTest +
		rTexture1DArrayTest +
		rTexture2DTest +
		rTexture2DArrayTest +
		rTexture2DMSTest +
		rTexture2DMSArrayTest +
		rTexture3DTest
		);
}

////

bool CDiscardRTView1Test::CreateViews()
{
	// Local variables
	HRESULT hr = E_FAIL;
	bool success = false;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	rtvDesc.Format = m_SrcFormat;
	if( CD3D10FormatHelper_Test::YUV( rtvDesc.Format ) )
	{
		// The view format cannot be a video format...we need to find the correct format for the view.
		std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
		if( !CFormatHelper::GetRTViewFormatsForVideoResourceFormat( rtvDesc.Format, luminanceDataFormats, chrominanceDataFormats ) )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::GetRTViewFormatsForVideoResourceFormat() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateView );
			goto done;
		}
		if( luminanceDataFormats.empty() )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::GetRTViewFormatsForVideoResourceFormat() passed, but the luminanceDataFormats vector was empty.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateView );
			goto done;
		}
		rtvDesc.Format = luminanceDataFormats[0];
		WriteToLog( _T("The texture's format is a video format (%s). The view format will be %s."), ToString(m_SrcFormat).c_str(), ToString(rtvDesc.Format).c_str() );
		
		//
		// Need to update some test parameters based on the video format
		//

		// Check to see if the format is subsampled, if so, adjust the dimensions
		switch( m_SrcFormat )
		{
		case DXGI_FORMAT_YUY2:
			m_uWidth /= 2;	// YUY2 is subsampled
			break;
		}

		// Change the format to the view format
		m_SrcFormat = rtvDesc.Format;
	}
	
	if( !GetViewDimension( m_ResourceScenario, &rtvDesc.ViewDimension ) )
	{
		LogError( __FILEW__, __LINE__, L"GetViewDimension() failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		goto done;
	}

	switch( rtvDesc.ViewDimension )
	{
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
		break;	// MS have a single subresource. Nothing to define
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
		rtvDesc.Texture2DMSArray.FirstArraySlice = m_uViewFirstArraySlice;
		rtvDesc.Texture2DMSArray.ArraySize = m_uViewArraySize;
		break;
	case D3D11_RTV_DIMENSION_BUFFER:
		rtvDesc.Buffer.ElementOffset = m_uViewElementOffset;
		rtvDesc.Buffer.ElementWidth = m_uViewElementWidth;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1D:
	case D3D11_RTV_DIMENSION_TEXTURE2D:
		rtvDesc.Texture1D.MipSlice = m_uViewMipSlice;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
		rtvDesc.Texture1DArray.MipSlice = m_uViewMipSlice;
		rtvDesc.Texture1DArray.ArraySize = m_uViewArraySize;
		rtvDesc.Texture1DArray.FirstArraySlice = m_uViewFirstArraySlice;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE3D:
		rtvDesc.Texture3D.MipSlice = m_uViewMipSlice;
		rtvDesc.Texture3D.FirstWSlice = m_uViewFirstWSlice;
		rtvDesc.Texture3D.WSize = m_uViewWSize;
	}

	// Create the view
	if( FAILED( hr = m_pDevice1->CreateRenderTargetView( m_pResourceToDiscard, &rtvDesc, (ID3D11RenderTargetView**)(&m_pResourceViewToDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateRenderTargetView m_pResourceViewToDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// Create the dummy view
	if( FAILED( hr = m_pDevice1->CreateRenderTargetView( m_pDummyResourceNoDiscard, &rtvDesc, (ID3D11RenderTargetView**)(&m_pDummyResourceViewNoDiscard) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"Failed to CreateDepthStencilView m_pDummyResourceViewNoDiscard.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
		goto done;
	}

	// If we got here, we passed
	success = true;

done:
	return success;
}