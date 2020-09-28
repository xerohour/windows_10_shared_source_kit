// ClearView.inl

#include "FormatDesc.hpp"
#include <limits>

extern const WCHAR * ClearView_Inl;

bool HasIntComponent( DXGI_FORMAT format );

//---------------------------------------------------------------------------------------------------------------------------------
template< UINT TSampleCount >
inline void CClearViewTest<TSampleCount>::InitTestParameters( )
{
	CResourceAccessTestApp * pFwk = static_cast< CResourceAccessTestApp * > ( GetFramework() );

	// RefreshGraphics() in order to ensure the framework device is available during parameter initialization
	// BUGBUG: WinBlue: 551704 - Tests should not be using d3D device capabilities to generate test factors
	pFwk->RefreshGraphics();

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam  = AddParameter( _T("Format"), &m_Format );
	CTestCaseParameter<UINT> *pMSAAQualityParam = AddParameter( _T("MSAAQuality"), &m_MultisampleQuality);
	testfactor::RFactor rFormatsAndMSAA = GetFormatsAndMSAA(MSAACheckFormats, TSampleCount, pFormatParam, pMSAAQualityParam);
	testfactor::RFactor rFormatsAndMSAA_Video = GetFormatsAndMSAA(MSAACheckFormats_Video, TSampleCount, pFormatParam, pMSAAQualityParam);
	testfactor::RFactor rFormatsAndMSAA_Video_Mips = GetFormatsAndMSAA(MSAACheckFormats_Video_Mips, TSampleCount, pFormatParam, pMSAAQualityParam);

#if 0
	//
		If this test factor is added, rfResScenario must be added to the root test factor below.
	//
	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T("ResourceScenario"), &m_ResourceScenario );
	testfactor::RFactor rfResScenario = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	rfResScenario = rfResScenario + AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	if ( pFwk->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		rfResScenario = rfResScenario + AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	}
#endif

	CTestCaseParameter<UINT> *pArraysParam = AddParameter<UINT>( _T("ArraySize"), &m_ArraySize );
	testfactor::RFactor rfArray1 = AddParameterValue<UINT>( pArraysParam, 1 );
	testfactor::RFactor rfArray3 = AddParameterValue<UINT>( pArraysParam, 3 );
	testfactor::RFactor rfArray6 = AddParameterValue<UINT>( pArraysParam, 6 );

	CTestCaseParameter<UINT> *pViewArraySize = AddParameter<UINT>( _T("ViewArraySize"), &m_ViewArraySize );
	testfactor::RFactor rfViewArray1 = AddParameterValue<UINT>( pViewArraySize, 1 );
	testfactor::RFactor rfViewArray2 = AddParameterValue<UINT>( pViewArraySize, 2 );
	testfactor::RFactor rfViewArray6 = AddParameterValue<UINT>( pViewArraySize, 6 );

	CTestCaseParameter<UINT> *pFirstArraySlice = AddParameter<UINT>( _T("FirstArraySlice"), &m_FirstArraySlice );
	testfactor::RFactor rfFirstArraySlice0 = AddParameterValue<UINT>( pFirstArraySlice, 0 );
	testfactor::RFactor rfFirstArraySlice1 = AddParameterValue<UINT>( pFirstArraySlice, 1 );


	testfactor::RFactor rfArrayAndView1 = rfArray1 * ( rfViewArray1 * rfFirstArraySlice0 );
	testfactor::RFactor rfArrayAndView2 = rfArray3 * ( rfViewArray1 * rfFirstArraySlice0 + rfViewArray2 * ( rfFirstArraySlice0 + rfFirstArraySlice1 ) );
	testfactor::RFactor rfArrayAndView6 = rfArray6 * ( rfViewArray1 * rfFirstArraySlice0 + rfViewArray2 * ( rfFirstArraySlice0 + rfFirstArraySlice1 ) + rfViewArray6 * rfFirstArraySlice0 );

	testfactor::RFactor rfArrayAndView;

	if ( pFwk->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		rfArrayAndView = rfArrayAndView1 + rfArrayAndView2;
		if( TSampleCount == 1 )
		{
			// No multisample on cube maps
			rfArrayAndView = rfArrayAndView + rfArrayAndView6;
		}
	}
	else
	{
		rfArrayAndView = rfArrayAndView1;
	}
	
	CTestCaseParameter<UINT> *pMipLevelsParam = AddParameter<UINT>( _T("MipLevels"), &m_MipLevels );
	testfactor::RFactor rfMips1 = AddParameterValue<UINT>( pMipLevelsParam, 1 );
	testfactor::RFactor rfMips2;
	testfactor::RFactor rfMips6;
	if( TSampleCount == 1 )
	{
		// Mips not supported on multisample textures
		rfMips2 = AddParameterValue<UINT>( pMipLevelsParam, 2 );
		rfMips6 = AddParameterValue<UINT>( pMipLevelsParam, 6 );
	}

	CTestCaseParameter<UINT> *pMipSlice = AddParameter<UINT>( _T("MipSlice"), &m_MipSlice );
	testfactor::RFactor rfSlice0 = AddParameterValue<UINT>( pMipSlice, 0 );
	testfactor::RFactor rfSlice1 = AddParameterValue<UINT>( pMipSlice, 1 );

	testfactor::RFactor rfMips = 
		rfMips1 * rfSlice0 +
		rfMips2 * ( rfSlice0 + rfSlice1 ) +
		rfMips6 * ( rfSlice0 + rfSlice1 );

	CTestCaseParameter<eRectScenario> *pRectScenario = AddParameter(_T("RectScenario"), &m_RectScenario );
	CNamedValuesTable<eRectScenario>* pRectValuesTable = NULL;
	GetNamedValuesTable(&pRectValuesTable);
	testfactor::RFactor rfRectScenario = AddParameterValueSet( pRectScenario, new CUserValueSet<eRectScenario>(pRectValuesTable));
	
	CTestCaseParameter<eColorScenario> *pColorScenario = AddParameter(_T("ColorScenario"), &m_ColorScenario );
	CNamedValuesTable<eColorScenario>* pColorValuesTable = NULL;
	GetNamedValuesTable(&pColorValuesTable);
	testfactor::RFactor rfColorScenario = AddParameterValueSet<eColorScenario>( pColorScenario, new CUserValueSet<eColorScenario>(pColorValuesTable));
	
	CTestCaseParameter<eViewType> *pViewType = AddParameter(_T("ViewType"), &m_ViewType );
	CUserValueSet<eViewType>* pViewValues = new CUserValueSet<eViewType>( );
	pViewValues->AddValue( eRTV );
	pViewValues->AddValue( eVPV );
	pViewValues->AddValue( eVDV );
	pViewValues->AddValue( eVIV );
	testfactor::RFactor rfViewTypeNonUAV = AddParameterValueSet<eViewType>( pViewType, pViewValues );
	testfactor::RFactor rfViewTypeUAV = AddParameterValue<eViewType>( pViewType, eUAV );

	testfactor::RFactor rfArrayAndViewAndType = rfArrayAndView * rfViewTypeNonUAV;
	if( TSampleCount == 1 )
	{
		rfArrayAndViewAndType = rfArrayAndViewAndType + rfArrayAndView1 * rfViewTypeUAV;
	}

	testfactor::XFACTOR xfPairwise[] =
	{
			{rFormatsAndMSAA, 1, NULL, 0},      
			{rfRectScenario, 1, NULL, 0},      
			{rfColorScenario, 1, NULL, 0},
			{rfMips, 1, NULL, 0},
	};

	testfactor::XFACTOR xfPairwise_Video[] =
	{ 
			{rfRectScenario, 1, NULL, 0},      
			{rfColorScenario, 1, NULL, 0},
			{rfMips1 * rfSlice0, 1, NULL, 0},
	};

	testfactor::XFACTOR xfPairwise_Video_Mips[] =
	{ 
			{rfRectScenario, 1, NULL, 0},      
			{rfColorScenario, 1, NULL, 0},
			{rfMips, 1, NULL, 0},
	};

	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfFactors_Pairwise = 
			NewCrossFactor( 
					xfPairwise,
					_countof(xfPairwise),
					_T("PairwiseFactors"),
					groupOrders,
					_countof(groupOrders)
			);

	testfactor::RFactor rfFactors_Pairwise_Video = 
			NewCrossFactor( 
					xfPairwise_Video,
					_countof(xfPairwise_Video),
					_T("PairwiseFactors_Video"),
					groupOrders,
					_countof(groupOrders)
			);

	testfactor::RFactor rfFactors_Pairwise_Video_Mips = 
			NewCrossFactor( 
					xfPairwise_Video_Mips,
					_countof(xfPairwise_Video_Mips),
					_T("PairwiseFactors_Video_Mips"),
					groupOrders,
					_countof(groupOrders)
			);
	
	SetRootTestFactor( (/*rfResScenario * */ rfFactors_Pairwise * rfArrayAndViewAndType) +
					(rFormatsAndMSAA_Video * rfFactors_Pairwise_Video * rfArrayAndViewAndType) +
					(rFormatsAndMSAA_Video_Mips * rfFactors_Pairwise_Video_Mips * rfArrayAndViewAndType));
}

//---------------------------------------------------------------------------------------------------------------------------------
template< UINT T >
inline TEST_RESULT CClearViewTest<T>::Setup()
{
	// Cleanup
	Cleanup();

	HRESULT hr = E_FAIL;
	D3D11_FEATURE_DATA_D3D11_OPTIONS options = {0};
	if( FAILED( hr = GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS, &options, sizeof(options) ) ) )
	{
		WriteToLog( _T("CheckFeatureSupport failed for D3D11_FEATURE_D3D11_OPTIONS" ) );
		LogError( __FILEW__, __LINE__, L"CheckFeatureSupport failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport );
		return RESULT_FAIL;
	}
	if( !options.ClearView )
	{
		WriteToLog( _T("ClearView is not supported") );
		return RESULT_SKIP;
	}

	m_bSkipVideoViewTestCases = false;
	if( FAILED( hr = GetDevice()->QueryInterface( __uuidof(m_pVideoDevice), reinterpret_cast<void**>(&m_pVideoDevice) ) ) )
	{
		WriteToLog( _T("QueryInterface failed for ID3D11VideoDevice" ) );

		// WARP and REF and 11on12 are the only exceptions. They are allowed to NOT support the ID3D11VideoDevice interface
		const bool bIsWARP = GetFramework()->IsWARPAdapter( GetFramework()->GetDXGIAdapter() );
		const bool bIsREF = GetFramework()->IsREFAdapter( GetFramework()->GetDXGIAdapter() );
		const bool bIs11On12 = GetFramework()->IsD3D11On12Device( GetDevice() );
		if( bIsREF || bIsWARP || bIs11On12 )
		{
			WriteToLog( _T("Test is running with driver type REF or WARP, or through D3D11On12, which doesn't support the ID3D11VideoDevice interface. Video view test cases will be skipped." ) );
			m_bSkipVideoViewTestCases = true;
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"QueryInterface failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCIUnknownQueryInterface );
			return RESULT_FAIL;
		}
		
	}
	if( m_pVideoDevice &&
		FAILED( hr = GetImmediateContext()->QueryInterface( __uuidof(m_pVideoContext), reinterpret_cast<void**>(&m_pVideoContext) ) ) )
	{
		WriteToLog( _T("QueryInterface failed for ID3D11VideoContext" ) );
			LogError( __FILEW__, __LINE__, L"QueryInterface failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCIUnknownQueryInterface );
			return RESULT_FAIL;
	}

	if ( m_pVideoDevice )
	{
		D3D11_VIDEO_PROCESSOR_CONTENT_DESC DescProc;
		ZeroMemory( &DescProc, sizeof( DescProc ) );
		DescProc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
		DescProc.InputFrameRate.Numerator = 30;
		DescProc.InputFrameRate.Denominator = 1;
		DescProc.InputWidth = 256;
		DescProc.InputHeight = 256;
		DescProc.OutputFrameRate.Numerator = 30;
		DescProc.OutputFrameRate.Denominator = 1;
		DescProc.OutputWidth = 256;
		DescProc.OutputHeight = 256;
		DescProc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

		if( FAILED( hr = m_pVideoDevice->CreateVideoProcessorEnumerator( & DescProc, & m_pVideoEnum ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVideoProcessorEnumerator failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCVideoDeviceCreateVideoProcessorEnumerator );
			return RESULT_FAIL;
		}
	}

	// Win8: 506049 - Runtime support for deferred context + video views has been removed
	if( GetFramework()->IsDeferredContextUsed() )
	{
		WriteToLog( _T("Test is running with a deferred context, which doesn't support Video View test cases so they will be skipped." ) );
		m_bSkipVideoViewTestCases = true;
	}

	m_Scratch.Attach( new FLOAT_COLOR[ Edge0 * Edge0 ] );
	m_TextView.Attach( new TCHAR[ Edge0 * Edge0 ] );
	return RESULT_PASS;
}

//---------------------------------------------------------------------------------------------------------------------------------
// conversion
//---------------------------------------------------------------------------------------------------------------------------------

typedef struct _AYUVsample2 {
	BYTE bCrValue;
	BYTE bCbValue;
	BYTE bY_Value;
	BYTE bSampleAlpha8;
}AYUVsample2;


__inline AYUVsample2
ConvertRGBToYCrCb(
	int r,
	int g,
	int b
	)
{
	AYUVsample2 ayuv;

	float y = 0.257F * r + 0.504F * g + 0.098F * b + 16.5F;
	float u = -.148F * r - 0.291F * g + 0.439F * b + 128.5F;
	float v = 0.439F * r - 0.368F * g - 0.071F * b + 128.5F;

	ayuv.bCrValue = (BYTE)(v);
	ayuv.bCbValue = (BYTE)(u);
	ayuv.bY_Value = (BYTE)(y);
	ayuv.bSampleAlpha8 = 0xFF;

	return ayuv;
}

//---------------------------------------------------------------------------------------------------------------------------------
template< UINT TSampleCount >
float CClearViewTest<TSampleCount>::GetExpectedComponentValue( const float ClampedClearColorData[4], const float ClampedClearColorChrominanceData[4], const DXGI_FORMAT &resourceFormat, const DXGI_FORMAT &viewFormat, const DXGI_FORMAT &viewFormatChrominanceData, UINT pixelX, UINT pixelY, UINT component )
{
	float componentValue = (float)NAN_FLOAT32;
	bool bVideoView = false;
	switch( m_ViewType )
	{
	case eVPV:
	case eVDV:
	case eVIV:
		bVideoView = true;
	}

	if( !CD3D10FormatHelper_Test::YUV( resourceFormat ) || bVideoView )
	{
		// The underlying resource format is a not video format, or the view of the video format is a video view (decode, etc...)
		// Each pixel should be the same based on the clamped value, so just return the same component back.
		componentValue = ClampedClearColorData[component];
	}
	else
	{
		// The runtime interprets the RGBA value passed into ClearView differently, depending on the underlying video format and the UAV/SRV format...
		switch( resourceFormat )
		{
		default:
			assert(!"Test must be updated for this video format");
			break;

		case DXGI_FORMAT_AYUV:
			// Straightforward mapping of the entire surface in one view
			switch( component )
			{
			default:
				assert(!"Test is passing a bad component value");
				break;
			case 0:
				componentValue = ClampedClearColorData[2];	// Y
				break;
			case 1:
				componentValue = ClampedClearColorData[1];	// U
				break;
			case 2:
				componentValue = ClampedClearColorData[0];	// V
				break;
			case 3:
				componentValue = ClampedClearColorData[3];	// A
				break;
			}
			break;

		case DXGI_FORMAT_NV12:
			// Has a luminance and chrominance data view. The RGBA value is a combination of both.
			switch( component )
			{
			default:
				assert(!"Test is passing a bad component value");
				break;
			case 0:
				componentValue = ClampedClearColorData[0];				// Y
				break;
			case 1:
				componentValue = ClampedClearColorChrominanceData[0];	// U
				break;
			case 2:
				componentValue = ClampedClearColorChrominanceData[1];	// V
				break;
			case 3:
				componentValue = ClampedClearColorData[3];				// A
				break;
			}
			break;

		case DXGI_FORMAT_YUY2:
			// More complex. D3D maps the format like this:
			//Y0->R8, 
			//U0->G8, 
			//Y1->B8, 
			//V0->A8
			// This creates vertical "stripes" in the texture, depending on if the pixel is odd or even.
			// This is due to the fact that there are two Y values.
			assert( viewFormat == DXGI_FORMAT_R8G8B8A8_UNORM ||
					viewFormat == DXGI_FORMAT_R8G8B8A8_UINT );

			switch( component )
			{
			default:
				assert(!"Test is passing a bad component value");
				break;
			case 0:
				// Odd or even column pixel?
				componentValue = ((pixelX % 2) == 0) ? ClampedClearColorData[0] : ClampedClearColorData[2]; // Y0 vs Y1
				break;
			case 1:
				componentValue = ClampedClearColorData[1];	// U0
				break;
			case 2:
				componentValue = ClampedClearColorData[3];	// V0
				break;
			case 3:
				componentValue = 1.0f;						// A
				break;
			}
			break;
		}
	}

	return componentValue;
}

//---------------------------------------------------------------------------------------------------------------------------------
template< UINT TSampleCount >
void CClearViewTest<TSampleCount>::WriteColorFailure( const TCHAR * msg, int x, int y, const float ClampedColor[4], FLOAT_COLOR & ReformattedResult )
{
	WriteToLog( _T("%s at pixel (%d,%d). Expected (%1.3f,%1.3f,%1.3f,%1.3f) != Actual (%1.3f,%1.3f,%1.3f,%1.3f)"),
		msg,
		x, y,
		ClampedColor[0], 
		ClampedColor[1], 
		ClampedColor[2], 
		ClampedColor[3],
		ReformattedResult.comp[0],
		ReformattedResult.comp[1],
		ReformattedResult.comp[2],
		ReformattedResult.comp[3] );
}

template< UINT TSampleCount >
void CClearViewTest<TSampleCount>::WriteYUVTransform( const TCHAR * msg, const float ClampedColor[4] )
{
	AYUVsample2 ayuv;
	ayuv = ConvertRGBToYCrCb( 
		(int) (ClampedColor[0] * 255.f),
		(int) (ClampedColor[1] * 255.f), 
		(int) (ClampedColor[2] * 255.f) );
	WriteToLog( _T(" - YUVA transform of %s is (%1.3f,%1.3f,%1.3f,%1.3f)"),
		msg,
		(float) ayuv.bY_Value / 255.f,
		(float) ayuv.bCbValue / 255.f,
		(float) ayuv.bCrValue / 255.f,
		(float) ayuv.bSampleAlpha8 / 255.f );   
}

template< UINT TSampleCount >
testfactor::RFactor CClearViewTest<TSampleCount>::GetFormatsAndMSAA(const DXGI_FORMAT* MSAACheckFormats, const UINT &TSampleCount, CTestCaseParameter<DXGI_FORMAT> *pFormatParam, CTestCaseParameter<UINT> *pMSAAQualityParam)
{
	testfactor::RFactor rFormatsAndMSAA;

	for (UINT uFormat = 0; MSAACheckFormats[uFormat] != DXGI_FORMAT_UNKNOWN; ++uFormat)
	{
		if( TSampleCount > 1 && HasIntComponent( MSAACheckFormats[uFormat] ) )
		{
			// Cannot resolve INT/UINT formats and can't create staging MSAA textures
			continue;
		}
		UINT uLevels = 0;
		if( FAILED( GetDevice()->CheckMultisampleQualityLevels( MSAACheckFormats[uFormat], TSampleCount, &uLevels ) ) )
		{
			// Do not add format if it doesn't support the sample count
			continue;
		}

		if( uLevels != 0 )
		{
			testfactor::RFactor rFormat = AddParameterValue<DXGI_FORMAT>( pFormatParam, MSAACheckFormats[uFormat] );
			testfactor::RFactor rMSAA = AddParameterValue<UINT>( pMSAAQualityParam, 0 );
			if( uLevels > 1 )
			{
				rMSAA = rMSAA + AddParameterValue<UINT>( pMSAAQualityParam, uLevels-1 );
			}
			rFormatsAndMSAA = rFormatsAndMSAA + rFormat * rMSAA;
		}
	}

	return rFormatsAndMSAA;
}

template< UINT TSampleCount >
bool CClearViewTest<TSampleCount>::ConvertAndClampClearColorToFormat( const float ClearColor[4], const DXGI_FORMAT &colorConvertFormat, float ConvertedAndClampedClearColor[4] )
{
	// If the format is a video format, then the spec says to treat the ClearColor as a UINT:
	// For Video Views with YUV or YCbBr formats, no color space conversion happens – 
	//		and in cases where the format name doesn’t indicate _UNORM vs. _UINT etc., _UINT is assumed (so input 235.0f maps to 235 as described above).
	bool m_bVideoView = false;
	switch( m_ViewType )
	{
	case eVDV:
	case eVPV:
	case eVIV:
		m_bVideoView = true;
		break;
	}

	if( m_bVideoView &&
		CD3D10FormatHelper_Test::YUV( colorConvertFormat ) )
	{
		//
		// Inputs are taken as integral floats, so 235.0f maps to 235 (fractions rounded to zero, out of range/INF values clamped to target range, NaN to 0).
		//

		for( UINT i = 0; i < 4; i++ )
		{
			// Get the number of bits and clamp to the range
			const UINT compNumBits = CD3D10FormatHelper_Test::GetBitsPerComponent( colorConvertFormat, i );
			if( compNumBits == 0 )
				continue;

			// NaN goes to 0, negative infinity goes to 0 because UINT is assumed
			const float pInf = std::numeric_limits<float>::infinity();
			const float nInf = -1.0f * pInf;
			if( (CFormatHelper::IsNaN( ClearColor[i] )) ||
				(ClearColor[i] == nInf) ||
				(ClearColor[i] <= 0.0f) )
			{
				ConvertedAndClampedClearColor[i] = 0.0f;
				continue;
			}

			// Convert to UINT - round to zero and clamp to range
			assert( ClearColor[i] >= 0.0f );
			const UINT compBitMask = (1 << compNumBits) - 1;
			const float maxFloatInRange = (float)compBitMask;						// Max float value possible with compNumBits bits
			float clampedFloat = max( 0u, min(ClearColor[i], maxFloatInRange) );	// Clamp float to range [0.0f, maxFloatInRange]
			ConvertedAndClampedClearColor[i] = floor(clampedFloat);					// Truncate the float
		}
	}
	else
	{
		FLOAT_COLOR ConvertedAndClampedBackGroundClearColor;
		BYTE pConvertedAndClampedTempData[8*8]; // Enough for any format, given the assert that comes next
		ZeroMemory( pConvertedAndClampedTempData, sizeof( pConvertedAndClampedTempData ) );
		assert( GetBitsPerElement( colorConvertFormat )/8 < sizeof( pConvertedAndClampedTempData ) );

		for( UINT i = 0; i < 4; i++ )
			ConvertedAndClampedBackGroundClearColor.comp[i] = ClearColor[i];

		if ( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, colorConvertFormat, &ConvertedAndClampedBackGroundClearColor, 1u, 1u, 1u, reinterpret_cast<void*>(pConvertedAndClampedTempData) ) )
		{
			WriteToLog( _T("ConvertPixels( CONV_TO_FORMAT, ... ) failed.") );
			return false;
		}
		if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, colorConvertFormat, &ConvertedAndClampedBackGroundClearColor, 1u, 1u, 1u, reinterpret_cast<void*>(pConvertedAndClampedTempData) ) )
		{
			WriteToLog( _T("ConvertPixels( CONV_FROM_FORMAT, ... ) failed.") );
			return false;
		}
		
		for( UINT i = 0; i < 4; i++ )
			ConvertedAndClampedClearColor[i] = ConvertedAndClampedBackGroundClearColor.comp[i];
	}

	return true;
}

template< UINT TSampleCount >
std::vector<RECT> CClearViewTest<TSampleCount>::GetViewSpaceRects( const RECT* const pRects, const UINT &viewSpaceRectCount )
{
	// Local variables
	std::vector<RECT> viewSpaceRects;
	viewSpaceRects.clear();

	// Check parameters
	if( viewSpaceRectCount < 1 )
	{
		return viewSpaceRects;
	}

	// Is the underlying resource format a video format?
	if( m_bIsYUV )
	{
		// Is the view being used a video view (e.g Video input view)?
		// If using a non-video view, then the rects may need to be convered to the view format's space
		if( !m_bVideoView )
		{
			// Some formats are subsampled (like YUY2). 
			// This means a 64x64 YUY2 texture with an RTV with the format R8G8B8A8_UINT appears in shaders (and to RSSetViewports()) as having dimensions 32x64 RGBA values
			if( m_bIsYUVSubsampled )
			{
				if( GetFramework()->LogVerbose() )
				{
					WriteToLog( _T("Rects will be modified for the subsampled video format because it's being used with a D3D view.") );
				}
				switch( m_Format )
				{
				default:
					assert( !"Test needs to be updated for this format" );
					break;
				case DXGI_FORMAT_YUY2:
					return GetModifiedRects( pRects, viewSpaceRectCount, 0.5f, 1.0f );	// X is half. Y is untouched.
					break;
				}
			}
		}
	}

	// If we got here, use the same rects
	return GetModifiedRects( pRects, viewSpaceRectCount, 1.0f, 1.0f );	// X is untouched. Y is untouched.
}

template< UINT TSampleCount >
std::vector<RECT> CClearViewTest<TSampleCount>::GetModifiedRects( const RECT* const pRects, const UINT &numRects, const float &xScale, const float &yScale )
{
	// Local variables
	std::vector<RECT> modifiedRects;
	modifiedRects.clear();

	for( UINT i = 0; i < numRects; i++ )
	{
		RECT newRect;
		newRect.top = static_cast<LONG>(yScale * pRects[i].top);
		newRect.bottom = static_cast<LONG>(yScale * pRects[i].bottom);
		newRect.left = static_cast<LONG>(xScale * pRects[i].left);
		newRect.right = static_cast<LONG>(xScale * pRects[i].right);

		modifiedRects.push_back(newRect);
	}

	return modifiedRects;
}

template< UINT TSampleCount >
void CClearViewTest<TSampleCount>::WriteRects( const std::vector<RECT> &rects )
{
	for( UINT i = 0; i < rects.size(); i++ )
	{
		WriteToLog( _T("Rect #%d: (L = %ld, T = %ld, R = %ld, B = %ld)"), i,
					rects[i].left, rects[i].top, rects[i].right, rects[i].bottom );
	}
}

template< UINT TSampleCount >
void CClearViewTest<TSampleCount>::WriteRects( const RECT* pRects, const UINT &numRects )
{
	for( UINT i = 0; i < numRects; i++ )
	{
		WriteToLog( _T("Rect #%d: (L = %ld, T = %ld, R = %ld, B = %ld)"), i,
					pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom );
	}
}

//#define ClearView_Inl L##__FILE__

//---------------------------------------------------------------------------------------------------------------------------------
template< UINT TSampleCount >
inline TEST_RESULT CClearViewTest<TSampleCount>::ExecuteTestCase()
{
	try
	{
		const bool bFL9 = GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0;

		UINT Bits;
		GetDevice()->CheckFormatSupport( m_Format, & Bits );

		if ( Bits == 0 )
		{
			WriteToLog( _T("No support for this format") );
			throw CTosser( RESULT_SKIP );
		}

		if ( ( 0 == ( Bits & D3D11_FORMAT_SUPPORT_MIP ) ) && m_MipLevels > 1 )
		{
			WriteToLog( _T("Skipping mip > 0 because format doesn't support mipping") );
			throw CTosser( RESULT_SKIP );
		}

        if (eVDV == m_ViewType && m_MipLevels > 1)
        {
			WriteToLog( _T("Skipping mip > 0 because video decode does not support mipping") );
			throw CTosser( RESULT_SKIP );
        }

		UINT uLevels;
		if( FAILED( GetDevice()->CheckMultisampleQualityLevels( m_Format, TSampleCount, &uLevels ) ) 
			|| m_MultisampleQuality >= uLevels )
		{
			WriteToLog( _T("No MSAA levels for this format") );
			throw CTosser( RESULT_SKIP );
		}

		UINT VideoBits = 0;
		if ( m_pVideoEnum )
		{
			m_pVideoEnum->CheckVideoProcessorFormat( m_Format, & VideoBits );
		}

		// Check to see if arrays are supported on the format
		if ( CD3D10FormatHelper_Test::Planar( m_Format ) &&
			m_ArraySize > 2 )
		{
			WriteToLog( _T("Texture arrays larger then 2 are not allowed for planar formats.") );
			throw CTosser( RESULT_SKIP );
		}

		SRectScenarios *pRectList = (m_MipSlice == 0) ? RectScenariosMip0 : RectScenariosMip1;
		assert( pRectList[m_RectScenario].SanityCheck == m_RectScenario );

		// Check that the rects are aligned to format
		// Video formats need to check that both the box and the actual resource are aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( m_Format );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( m_Format );
		const UINT numRects = pRectList[m_RectScenario].Count;

		for( UINT i = 0; i < numRects; i++ )
		{
			// Get the edges...
			const UINT left = pRectList[m_RectScenario].pRects[i].left;
			const UINT right = pRectList[m_RectScenario].pRects[i].right;
			const UINT top = pRectList[m_RectScenario].pRects[i].top;
			const UINT bottom = pRectList[m_RectScenario].pRects[i].bottom;

			// Note: This code assumes the test is using a Texture2D
			if( (top % verticalAlignment) != 0 )
			{
				WriteToLog( _T("RECT[%u]'s top (%u) doesn't match the format's (%s) vertical alignment (%u)"), i, top, ToString(m_Format).c_str(), verticalAlignment );
				LogError( __FILEW__, __LINE__, L"Test case uses bad RECT for format.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				return RESULT_FAIL;
			}

			if( (bottom % verticalAlignment) != 0 )
			{
				WriteToLog( _T("RECT[%u]'s bottom (%u) doesn't match the format's (%s) vertical alignment (%u)"), i, bottom, ToString(m_Format).c_str(), verticalAlignment );
				LogError( __FILEW__, __LINE__, L"Test case uses bad RECT for format.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				return RESULT_FAIL;
			}

			if( (left % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("RECT[%u]'s left (%u) doesn't match the format's (%s) horizontal alignment alignment (%u)"), i, left, ToString(m_Format).c_str(), horizontalAlignment );
				LogError( __FILEW__, __LINE__, L"Test case uses bad RECT for format.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				return RESULT_FAIL;
			}

			if( (right % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("RECT[%u]'s right (%u) doesn't match the format's (%s) horizontal alignment alignment (%u)"), i, right, ToString(m_Format).c_str(), horizontalAlignment );
				LogError( __FILEW__, __LINE__, L"Test case uses bad RECT for format.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				return RESULT_FAIL;
			}
		}

		// Check to see the view has a format
		m_bViewHasFormat = false;
		switch( m_ViewType )
		{
		case eRTV:
		case eUAV:
			m_bViewHasFormat = true;
			break;
		}

		// Check to see if arrays are supported on the video device
		m_bVideoView = false;
		switch( m_ViewType )
		{
		case eVDV:
		case eVPV:
		case eVIV:
			m_bVideoView = true;
			break;
		}

		// Check to see if the format is subsampled
		m_bIsYUVSubsampled = false;
		switch( m_Format )
		{
		case DXGI_FORMAT_YUY2:
			m_bIsYUVSubsampled = true;
			break;
		}

		// Win8: 771567: MSAA resources don't support VP input/Output
		const bVPInputOrOutput = (m_ViewType == eVPV) || (m_ViewType == eVIV);
		if( bVPInputOrOutput && (TSampleCount > 1) )
		{
			WriteToLog( _T("Skipping MSAA test case with Video Processor Input View or Video Processor Output View") );
			throw CTosser( RESULT_SKIP );
		}

		// See if we should skip video views
		if( m_bVideoView && m_bSkipVideoViewTestCases )
		{
			WriteToLog( _T("Skipping video view test case.") );
			throw CTosser( RESULT_SKIP );
		}

		if( m_bVideoView )
		{
			D3D11_VIDEO_PROCESSOR_CAPS vpCaps;
			ZeroMemory( &vpCaps, sizeof(vpCaps) );
			if ( FAILED( m_pVideoEnum->GetVideoProcessorCaps( &vpCaps ) ) )
			{
				WriteToLog( _T("GetVideoProcessorCaps() failed.") );
				throw CTosser( RESULT_FAIL );
			}

			const bool bStereoSupported = (vpCaps.StereoCaps > 0);
			if( bStereoSupported )
			{
				if( m_ArraySize > 2 )
				{
					WriteToLog( _T("The device supports stereo, so the max array size is 2.") );
					throw CTosser( RESULT_SKIP );
				}
			}
			else
			{
				// Stereo is not supported
				assert( !bStereoSupported );
				if( m_ArraySize > 1 )
				{
					WriteToLog( _T("The device doesn't support stereo, so the max array size is 1.") );
					throw CTosser( RESULT_SKIP );
				}
			}
		}

		D3D11_TEXTURE2D_DESC d;
		d.ArraySize = m_ArraySize;
		d.BindFlags = 0;
		switch( m_ViewType )
		{
		case eUAV:
			d.BindFlags |= ( (Bits & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW ) ? D3D11_BIND_UNORDERED_ACCESS : 0 );
			break;
		case eRTV:
			d.BindFlags |= ( (Bits & D3D11_FORMAT_SUPPORT_RENDER_TARGET ) ? D3D11_BIND_RENDER_TARGET : 0 );
			break;
		case eVDV:
			d.BindFlags |= ( (Bits & D3D11_FORMAT_SUPPORT_DECODER_OUTPUT ) ? D3D11_BIND_DECODER : 0 );
			break;
		case eVPV:
			d.BindFlags |= ( (VideoBits & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT ) ? D3D11_BIND_RENDER_TARGET : 0 );
			break;
		case eVIV:
			//no bind bits for video input view, but we need SOMEthing
			if( !CD3D10FormatHelper_Test::YUV( m_Format ) )
			{
				d.BindFlags |= ( ((Bits & D3D11_FORMAT_SUPPORT_SHADER_LOAD) && !(bFL9 && TSampleCount > 1)) ? D3D11_BIND_SHADER_RESOURCE : 0 );	// Debug layer complains about SRV bind flag if FL 9 and sample count > 1.
				d.BindFlags |= ( (VideoBits & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT ) ? D3D11_BIND_RENDER_TARGET : 0 );
			}
			break;
		}
		if ( ! d.BindFlags )
		{
			WriteToLog( _T("This format supports no bind flags under test for this case (%s). Skipping."), ToString( m_ViewType ).c_str() );
			throw CTosser( RESULT_SKIP );
		}

		d.CPUAccessFlags = 0;
		d.Format = m_Format;
		d.Height = Edge0;
		d.Width = Edge0;
		d.MipLevels = m_MipLevels;
		d.MiscFlags = m_ArraySize == 6 ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		d.SampleDesc.Count = TSampleCount;
		d.SampleDesc.Quality = m_MultisampleQuality;
		d.Usage = D3D11_USAGE_DEFAULT;

		if ( d.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE )
		{
			// Does the format support texture cubes?
			if ( 0 == ( Bits & D3D11_FORMAT_SUPPORT_TEXTURECUBE ) )
			{
				WriteToLog( _T("Skipping because format doesn't support texture cubes.") );
				throw CTosser( RESULT_SKIP );
			}
		}

		if( GetFramework()->LogVerbose() )
		{
			WriteToLog( _T("Creating a %ux%u resource with format %s."), d.Width, d.Height, ToString(d.Format).c_str() );
		}
		ATL::CComPtr< ID3D11Texture2D > p2D;
		HRESULT hr = GetDevice1()->CreateTexture2D( &d, 0, & p2D );
		if ( FAILED(hr ) )
		{
			LogError( ClearView_Inl, __LINE__, L"Failed to create Texture2D.", hr, ERRORCODE_TYPE_HRESULT );
			throw CTosser( RESULT_FAIL );
		}

		// Some video formats will need a view for the Luminance data, and another for the Chrominance data
		const bool bNeedChrominanceDataView = m_bViewHasFormat && CD3D10FormatHelper_Test::YUV( d.Format ) && !CFormatHelper::ResourceViewContainsBothLuminanceDataAndChrominanceData( d.Format );
		ATL::CComPtr< ID3D11View > pView;
		ATL::CComPtr< ID3D11View > pViewChrominanceData;	
		DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT viewFormatChrominanceData = DXGI_FORMAT_UNKNOWN;
		switch( m_ViewType )
		{
		case eRTV:
			{
				if ( 0 == ( d.BindFlags & D3D11_BIND_RENDER_TARGET ) )
				{
					WriteToLog( _T("Skipping RTV test, since driver does not claim support for this format") );
					throw CTosser( RESULT_SKIP );
				}
				std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
				UINT Index = 0;
				D3D11_RENDER_TARGET_VIEW_DESC rtvd;
				rtvd.Format = d.Format;
				if ( CD3D10FormatHelper_Test::YUV( d.Format ) )
				{
					// The view format cannot be a video format...we need to find the correct format for the view.
					if( !CFormatHelper::GetRTViewFormatsForVideoResourceFormat( d.Format, luminanceDataFormats, chrominanceDataFormats ) )
					{
						WriteToLog( _T("CFormatHelper::GetRTViewFormatsForVideoResourceFormat() failed for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_FAIL );
					}
					if( luminanceDataFormats.empty() )
					{
						WriteToLog( _T("CFormatHelper::GetRTViewFormatsForVideoResourceFormat() passed, but the luminanceDataFormats vector was empty failed for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_FAIL );
					
					}

					hr = CFormatHelper::GetFirstFormatWithSupport(GetDevice1(), luminanceDataFormats, D3D11_FORMAT_SUPPORT_RENDER_TARGET, Index);

					if (SUCCEEDED(hr))
					{
						rtvd.Format = luminanceDataFormats[Index];
						WriteToLog( _T("The texture's format is a video format (%s). The view format will be %s."), ToString(d.Format).c_str(), ToString(rtvd.Format).c_str() );
					}
					else
					{
						WriteToLog( _T("Skipping RTV test, since there's no luminance view format available for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_SKIP );
					}
				}
				viewFormat = rtvd.Format;
				if ( m_ArraySize > 1 )
				{
					if( d.SampleDesc.Count > 1 )
					{
						rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
						rtvd.Texture2DMSArray.ArraySize = m_ViewArraySize;
						rtvd.Texture2DMSArray.FirstArraySlice = m_FirstArraySlice;
					}
					else
					{
						rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						rtvd.Texture2DArray.ArraySize = m_ViewArraySize;
						rtvd.Texture2DArray.FirstArraySlice = m_FirstArraySlice;
						rtvd.Texture2DArray.MipSlice = m_MipSlice;
					}
				}
				else
				{
					if( d.SampleDesc.Count > 1 )
					{
						rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
					}
					else
					{
						rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						rtvd.Texture2D.MipSlice = m_MipSlice;
					}
				}
				hr = GetDevice1()->CreateRenderTargetView( p2D, &rtvd, reinterpret_cast< ID3D11RenderTargetView** > ( & pView ) );
				if ( FAILED( hr ) )
				{
					LogError( ClearView_Inl, __LINE__, L"Failed to create RTView.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}
				if ( bNeedChrominanceDataView )
				{
					hr = CFormatHelper::GetFirstFormatWithSupport(GetDevice1(), chrominanceDataFormats, D3D11_FORMAT_SUPPORT_RENDER_TARGET, Index);

					if (SUCCEEDED(hr))
					{
						rtvd.Format = chrominanceDataFormats[Index];
						if( GetFramework()->LogVerbose() )
						{
							WriteToLog( _T("A second view will be created for chrominance data with format %s."), ToString(rtvd.Format).c_str() );
						}
					}
					else
					{
						WriteToLog( _T("Skipping RTV test, since there's no chrominance view format available for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_SKIP );
					}
					
					hr = GetDevice1()->CreateRenderTargetView( p2D, &rtvd, reinterpret_cast< ID3D11RenderTargetView** > ( & pViewChrominanceData ) );
					if ( FAILED( hr ) )
					{
						LogError( ClearView_Inl, __LINE__, L"Failed to create RTView2.", hr, ERRORCODE_TYPE_HRESULT );
						throw CTosser( RESULT_FAIL );
					}
					viewFormatChrominanceData = rtvd.Format;
				}

			}
			break;
		case eUAV:
			{
				D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS ho;
				GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &ho, sizeof(ho));
				if ( ! ho.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
				{
					WriteToLog( _T("Skipping UAV test, since driver does not claim CS support") );
					throw CTosser( RESULT_SKIP );
				}
				if ( 0 == ( d.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) )
				{
					WriteToLog( _T("Skipping UAV test, since driver does not claim support for this format") );
					throw CTosser( RESULT_SKIP );
				}
				std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
				D3D11_UNORDERED_ACCESS_VIEW_DESC vd;
				vd.Format = d.Format;
				if ( CD3D10FormatHelper_Test::YUV( d.Format ) )
				{
					// The view format cannot be a video format...we need to find the correct format for the view.
					if( !CFormatHelper::GetUAViewFormatsForVideoResourceFormat( d.Format, luminanceDataFormats, chrominanceDataFormats ) )
					{
						WriteToLog( _T("CFormatHelper::GetUAViewFormatsForVideoResourceFormat() failed for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_FAIL );
					}
					if( luminanceDataFormats.empty() )
					{
						WriteToLog( _T("CFormatHelper::GetUAViewFormatsForVideoResourceFormat() passed, but the luminanceDataFormats vector was empty failed for %s." ), ToString(d.Format).c_str() );
						throw CTosser( RESULT_FAIL );
					
					}
					vd.Format = luminanceDataFormats[0];
					WriteToLog( _T("The texture's format is a video format (%s). The view format will be %s."), ToString(d.Format).c_str(), ToString(vd.Format).c_str() );
				}
				viewFormat = vd.Format;
				if ( m_ArraySize > 1 )
				{
					vd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
					vd.Texture2DArray.ArraySize = m_ViewArraySize;
					vd.Texture2DArray.FirstArraySlice = m_FirstArraySlice;
					vd.Texture2DArray.MipSlice = m_MipSlice;
				}
				else
				{
					vd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
					vd.Texture2D.MipSlice = m_MipSlice;
				}
				hr = GetDevice1()->CreateUnorderedAccessView( p2D, &vd, reinterpret_cast< ID3D11UnorderedAccessView** > ( & pView ) );
				if ( FAILED(hr ) )
				{
					LogError( ClearView_Inl, __LINE__, L"Failed to create UAView.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}
				if ( bNeedChrominanceDataView )
				{
					vd.Format = chrominanceDataFormats[0];
					if( GetFramework()->LogVerbose() )
					{
						WriteToLog( _T("A second view will be created for chrominance data with format %s."), ToString(vd.Format).c_str() );
					}
					hr = GetDevice1()->CreateUnorderedAccessView( p2D, &vd, reinterpret_cast< ID3D11UnorderedAccessView** > ( & pViewChrominanceData ) );
					if ( FAILED(hr ) )
					{
						LogError( ClearView_Inl, __LINE__, L"Failed to create UAView2.", hr, ERRORCODE_TYPE_HRESULT );
						throw CTosser( RESULT_FAIL );
					}
					viewFormatChrominanceData = vd.Format;
				}

			}
			break;
		case eVPV:
			{
				if ( 0 == ( VideoBits & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT ) )
				{
					WriteToLog( _T("Skipping VideoProcessorOutputView test, since driver does not claim support for this format") );
					throw CTosser( RESULT_SKIP );
				}
				if ( GetDevice1()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0  && m_MipSlice > 0 )
				{
					WriteToLog( _T("Skipping VideoProcessorOutputView test, since feature level 9.* doesn't support anything other than mip 0") );
					throw CTosser( RESULT_SKIP );
				}
				if ( m_ArraySize > 1 )
				{
					D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC vd;
					vd.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2DARRAY;
					vd.Texture2DArray.ArraySize = m_ViewArraySize;
					vd.Texture2DArray.FirstArraySlice = m_FirstArraySlice;
					vd.Texture2DArray.MipSlice = m_MipSlice;
					hr = m_pVideoDevice->CreateVideoProcessorOutputView( 
						p2D, 
						m_pVideoEnum, 
						&vd, 
						reinterpret_cast< ID3D11VideoProcessorOutputView** > ( & pView ) );
				}
				else
				{
					D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC vd;
					vd.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
					vd.Texture2D.MipSlice = m_MipSlice;
					hr = m_pVideoDevice->CreateVideoProcessorOutputView( 
						p2D, 
						m_pVideoEnum, 
						&vd, 
						reinterpret_cast< ID3D11VideoProcessorOutputView** > ( & pView ) );
				}
				if ( FAILED(hr ) )
				{
					LogError( ClearView_Inl, __LINE__, L"Failed to create Video Processor Output View.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}
			}
			break;
		case eVDV:
			{
				if ( 0 == ( d.BindFlags & D3D11_BIND_DECODER ) )
				{
					WriteToLog( _T("Skipping VideoDecoderOutputView test, since driver does not claim support for this format") );
					throw CTosser( RESULT_SKIP );
				}
				if ( m_ArraySize != 1 )
				{
					WriteToLog( _T("Skipping CreateVideoDecoderOutputView with array != 1") );
					throw CTosser( RESULT_FAIL );
				}

				// Win8: 983678 - Test assumes that decoder profiles are supported if a video device is acquired
				// If the count is 0, and we call GetVideoDecoderProfile, the runtime will crash. So we'll fail instead.
				const UINT videoDecoderProfileCount = m_pVideoDevice->GetVideoDecoderProfileCount();
				if( videoDecoderProfileCount == 0 )
				{
					LogError( ClearView_Inl, __LINE__, L"GetVideoDecoderProfileCount returned 0.", false, ERRORCODE_TYPE_BOOL );
					throw CTosser( RESULT_FAIL );
				}

				// Win8: 803324 - While the DECODER_OUTPUT flag indicates that the format can be used with at
				// least one video decoder profile, it does not guarantee that it can be used with any profile.
				// For this reason, we expect apps to call ID3D11VideoDevice::CheckVideoDecoderFormat before 
				// assuming that a format is supported by a specific decoder profile.
				GUID DecodeProfile;
				BOOL bDecoderProfileSupported = FALSE;
				UINT decoderProfileIndex = 0;
				do
				{
					hr = m_pVideoDevice->GetVideoDecoderProfile( decoderProfileIndex, &DecodeProfile );
					if ( FAILED( hr ) )
					{
						LogError( ClearView_Inl, __LINE__, L"Failed to  enum a decoder profile.", hr, ERRORCODE_TYPE_HRESULT );
						throw CTosser( RESULT_FAIL );
					}

					hr = m_pVideoDevice->CheckVideoDecoderFormat( &DecodeProfile, d.Format, &bDecoderProfileSupported );
					if ( FAILED( hr ) )
					{
						LogError( ClearView_Inl, __LINE__, L"CheckVideoDecoderFormat failed.", hr, ERRORCODE_TYPE_HRESULT );
						throw CTosser( RESULT_FAIL );
					}

					// Try the next decoder profile (if any)
					decoderProfileIndex++;

				} while( (decoderProfileIndex < videoDecoderProfileCount) && !bDecoderProfileSupported ); // If there are more decoder profiles to try and we haven't found one with support yet...go again.

				// If we got here and we still don't have support, something is very wrong because CheckFormatSupport indicated DECODER_OUTPUT support
				if( !bDecoderProfileSupported )
				{
					WriteToLog( _T("The test tried to use all decoder profile(s) (%u reported), but none returned decoder support."), videoDecoderProfileCount );
					LogError( ClearView_Inl, __LINE__, L"CheckVideoDecoderFormat failed.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}

				D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC vd;
				vd.DecodeProfile = DecodeProfile;
				vd.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;
				vd.Texture2D.ArraySlice = m_FirstArraySlice;
				hr = m_pVideoDevice->CreateVideoDecoderOutputView( 
					p2D, 
					&vd, 
					reinterpret_cast< ID3D11VideoDecoderOutputView** > ( & pView ) );

				if ( FAILED(hr ) )
				{
					LogError( ClearView_Inl, __LINE__, L"Failed to create Video decoder Output View.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}
			}
			break;
		case eVIV:
			{
				if ( 0 == ( VideoBits & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT ) )
				{
					WriteToLog( _T("Skipping VideoProcessorInputView test, since driver does not claim support for this format") );
					throw CTosser( RESULT_SKIP );
				}
				if ( m_ArraySize != 1 )
				{
					WriteToLog( _T("Skipping CreateVideoProcessorInputView with array != 1") );
					throw CTosser( RESULT_SKIP );
				}
				D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC vd;
				vd.FourCC = 0;
				vd.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
				vd.Texture2D.MipSlice = m_MipSlice;
				vd.Texture2D.ArraySlice = m_FirstArraySlice;
				hr = m_pVideoDevice->CreateVideoProcessorInputView( 
					p2D, 
					m_pVideoEnum, 
					&vd, 
					reinterpret_cast< ID3D11VideoProcessorInputView** > ( & pView ) );
				if ( FAILED(hr ) )
				{
					LogError( ClearView_Inl, __LINE__, L"Failed to create Video Processor input View.", hr, ERRORCODE_TYPE_HRESULT );
					throw CTosser( RESULT_FAIL );
				}
			}
			break;
		}

		d.SampleDesc.Count = 1;
		d.SampleDesc.Quality = 0;
		ATL::CComPtr< ID3D11Texture2D > p2DResolved;
		if ( TSampleCount > 1 )
		{
			hr = GetDevice1()->CreateTexture2D( &d, 0, & p2DResolved );
			if ( FAILED(hr ) )
			{
				LogError( ClearView_Inl, __LINE__, L"Failed to create resolved Texture2D.", hr, ERRORCODE_TYPE_HRESULT );
				throw CTosser( RESULT_FAIL );
			}
		}

		ATL::CComPtr< ID3D11Texture2D > pStaging;
		d.BindFlags = 0;
		d.Usage = D3D11_USAGE_STAGING;
		d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		hr = GetDevice1()->CreateTexture2D( &d, 0, & pStaging );
		if ( FAILED( hr ) )
		{
			LogError( ClearView_Inl, __LINE__, L"Failed to create staging Texture2D.", hr, ERRORCODE_TYPE_HRESULT );
			throw CTosser( RESULT_FAIL );
		}

		float BackGroundClearColor[4] = {.25f, 0.f, .75f, 0.f};
		float ClampedBackGroundClearColor[4] = {0.f,0.f,0.f,0.f};
		float ClampedBackGroundClearColorChrominanceData[4] = {0.f,0.f,0.f,0.f};

		// For the cases when a Chrominance data view is used, we can combine the two structures:
		//	E.g Format = NV12:
		//	Luminance View: R8_UINT (R --> Y)
		//	Chrominance View: R8G8_UINT (R --> U, G --> V)
		//	Combined: (Y, U, V, A)
		float ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[4] = {0.f,0.f,0.f,0.f};

		AYUVsample2 ayuvBackGround;

		m_bIsYUV = false;
		switch( m_Format )
		{
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_NV12:
			BackGroundClearColor[3] = 1.f;
			//fall through
		case DXGI_FORMAT_AYUV:
			m_bIsYUV = true;
		}

		bool bUINT = false;
		switch( m_Format )
		{
		case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			BackGroundClearColor[0] *= 1020.0f;
			BackGroundClearColor[1] *= 1020.0f;
			BackGroundClearColor[2] *= 1020.0f;
			BackGroundClearColor[3] *= 1020.0f;
			bUINT = true;
			break;
		}

		WriteToLog( _T("Calling ClearView() with float BackGroundClearColor[] = { %f, %f, %f, %f }."), BackGroundClearColor[0], BackGroundClearColor[1], BackGroundClearColor[2], BackGroundClearColor[3] );
		GetEffectiveContext1()->ClearView(
			pView, 
			BackGroundClearColor, 
			NULL, 0);

		// See if need to call clear view on the second view
		if ( bNeedChrominanceDataView )
		{
			assert( pViewChrominanceData );
			if( GetFramework()->LogVerbose() )
			{
				WriteToLog( _T("Calling ClearView() with float BackGroundClearColor[] a second time with the Chrominance Data View.") );
			}

			GetEffectiveContext1()->ClearView(
				pViewChrominanceData,
				BackGroundClearColor, 
				NULL, 0);
		}

		// Clear color values are converted/clamped to the destination format as appropriate per D3D conversion rules. 
		// E.g. if the format of the view is R8G8B8A8_UNORM, inputs are clamped to 0.0f to 1.0f (-INF/NaN to 0).
		// For Video Views with YUV or YCbBr formats, no color space conversion happens – and in cases where the format name doesn’t indicate _UNORM vs. _UINT etc., _UINT is assumed
		DXGI_FORMAT colorConvertFormat = m_bViewHasFormat ? viewFormat : m_Format;

		if( m_bVideoView && m_bIsYUV )
		{
			WriteToLog( _T("Converting/Clamping float BackGroundClearColor and float ClearColor to UINTs because the Video View's of YUV resources don't have view formats."), ToString(colorConvertFormat).c_str() );
		}
		else
		{
			WriteToLog( _T("Converting/Clamping float BackGroundClearColor and float ClearColor to %s."), ToString(colorConvertFormat).c_str() );
		}

		if( !ConvertAndClampClearColorToFormat( BackGroundClearColor, colorConvertFormat, ClampedBackGroundClearColor ) )
		{
			WriteToLog( _T("ConvertAndClampClearColorToFormat() failed.") );
			throw CTosser( RESULT_FAIL );
		}

		if( GetFramework()->LogVerbose() )
		{
			WriteToLog( _T("Converted/Clamped float BackGroundClearColor[] = { %f, %f, %f, %f }."), 
				ClampedBackGroundClearColor[0], ClampedBackGroundClearColor[1], ClampedBackGroundClearColor[2], ClampedBackGroundClearColor[3] );
		}

		if ( bNeedChrominanceDataView )
		{
			// If we have two views, with different formats then the color might of had some components dropped
			assert( m_bViewHasFormat );
			colorConvertFormat = viewFormatChrominanceData;
			WriteToLog( _T("Converting/Clamping chrominance float BackGroundClearColor and chrominance float ClearColor to %s."), ToString(colorConvertFormat).c_str() );
			if( !ConvertAndClampClearColorToFormat( BackGroundClearColor, colorConvertFormat, ClampedBackGroundClearColorChrominanceData ) )
			{
				WriteToLog( _T("ConvertAndClampClearColorToFormat() failed.") );
				throw CTosser( RESULT_FAIL );
			}

			// Combine the Luminance and Chromiance into a single structure to make it easier to check against later
			ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[0] = ClampedBackGroundClearColor[0];					// Y
			ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[1] = ClampedBackGroundClearColorChrominanceData[0];	// U
			ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[2] = ClampedBackGroundClearColorChrominanceData[1];	// V
			ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[3] = ClampedBackGroundClearColor[3];

			if( GetFramework()->LogVerbose() )
			{
				WriteToLog( _T("Converted/Clamped chrominance float BackGroundClearColor[] = { %f, %f, %f, %f }."), 
					ClampedBackGroundClearColorChrominanceData[0], ClampedBackGroundClearColorChrominanceData[1], ClampedBackGroundClearColorChrominanceData[2], ClampedBackGroundClearColorChrominanceData[3] );

				WriteToLog( _T("Converted/Clamped combined luminance and chrominance float BackGroundClearColor[] = { %f, %f, %f, %f } (Y, U, V, A)."), 
					ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[0], ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[1], ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[2], ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData[3] );
			}
		}

		UINT32 INFP = INFP_FLOAT32;
		UINT32 NAN  = 0x7f800001;

		float ClearColor[4] = {0.f,0.f,0.f,0.f};
		float ClampedClearColor[4] = {0.f,0.f,0.f,0.f};
		float ClampedClearColorChrominanceData[4] = {0.f,0.f,0.f,0.f};

		// For the cases when a Chrominance data view is used, we can combine the two structures:
		//	E.g Format = NV12:
		//	Luminance View: R8_UINT (R --> Y)
		//	Chrominance View: R8G8_UINT (R --> U, G --> V)
		//	Combined: (Y, U, V, A)
		float ClampedClearColorCombinedLuminanceAndChrominanceData[4] = {0.f,0.f,0.f,0.f};

		float IsNaN[4] = {false,false,false,false};
		float IsInf[4] = {false,false,false,false};
		switch(m_ColorScenario)
		{
		case eRedFractional:
			ClampedClearColor[0] = ClearColor[0] = 128.5f / 255.f;
			break;
		case eGreenIntegral:
			ClampedClearColor[1] = ClearColor[1] = 128.f / 255.f;
			break;
		case eGreenOutofRange:
			{
				ClearColor[1] = 2.f;
				ClampedClearColor[1] = 1.f;
				const UINT NumComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( m_Format );
				for( UINT i=0; i < NumComponents; i++ )
				{
					if( CD3D10FormatHelper_Test::GetComponentName( m_Format, i ) == D3D10FCN_G )
					{
						if( CD3D10FormatHelper_Test::GetFormatComponentInterpretation( m_Format, i ) == D3D10FCI_FLOAT )
						{
							// Not checking out of range on float, even float16, which has an out-of-range
							ClampedClearColor[1] = ClearColor[1];
						}
						break;
					}
				}
			}
			break;
		case eRedInf:
			ClearColor[0] = *(float*)&(INFP);
			ClampedClearColor[0] = 1.f;
			IsInf[0] = true;
			break;
		case eGreenNaN:
			ClearColor[1] = *(float*)&(NAN);
			IsNaN[1] = true;
			break;
		default:
			assert(false);
		}

		switch( m_Format )
		{
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_NV12:
			ClearColor[3] = 1.f;
			ClampedClearColor[3] = 1.f;
			break;
		case DXGI_FORMAT_R32G32B32A32_SINT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			ClearColor[0] *= 1020.0f;
			ClearColor[1] *= 1020.0f;
			ClearColor[2] *= 1020.0f;
			ClearColor[3] *= 1020.0f;
			ClampedClearColor[0] = ClearColor[0];
			ClampedClearColor[1] = ClearColor[1];
			ClampedClearColor[2] = ClearColor[2];
			ClampedClearColor[3] = ClearColor[3];
			break;
		}

		// Y=0 means black regardless of UV. Some drivers will fill with some made-up UV if Y=0 is sent
		// The visual meaning is the same, but it's hard to generalize this test to catch it, so we avoid
		// sending Y=0.
		if ( m_bIsYUV && ClearColor[0] == 0.0f )
		{
			ClampedClearColor[0] = ClearColor[0] = .01f;
		}

		// Win8:637330 The rects passed into ClearView need to be based on view format space
		// Thus a 64x64 YUY2 resource with a R8G8B8A8_UINT RTV is actually only 32x64 because YUY2 is subsampled
		const UINT viewSpaceRectCount = pRectList[m_RectScenario].Count;
		std::vector<RECT> viewSpaceRects = GetViewSpaceRects( pRectList[m_RectScenario].pRects, viewSpaceRectCount );
		assert( viewSpaceRects.size() == viewSpaceRectCount );
		const bool bPassNullRectList = m_RectScenario == eNULLRectList;

		WriteToLog( _T("Calling ClearView() with float ClearColor[] = { %f, %f, %f, %f }."), ClearColor[0], ClearColor[1], ClearColor[2], ClearColor[3] );

		ZeroMemory( m_rects, sizeof(m_rects) );
		for( UINT i = 0; i < viewSpaceRects.size(); i++ )
		{
			m_rects[i] = viewSpaceRects[i];
		}

		if( GetFramework()->LogVerbose() )
		{
			if( bPassNullRectList )
			{
				WriteToLog( _T("NULL rect(s) passed to ClearView.") );
			}
			else
			{
				WriteToLog( _T("%u rect(s) passed to ClearView (Left, Top, Right, Bottom):"), viewSpaceRects.size() );
				WriteRects( m_rects, viewSpaceRects.size() );
			}
		}
		GetEffectiveContext1()->ClearView(
			pView, 
			ClearColor, 
			bPassNullRectList ? nullptr : m_rects, 
			viewSpaceRects.size() );

		// See if need to call clear view on the second view
		if ( bNeedChrominanceDataView )
		{
			assert( pViewChrominanceData );
			if( GetFramework()->LogVerbose() )
			{
				WriteToLog( _T("Calling ClearView() with float ClearColor[] a second time with the Chrominance Data View.") );
			}
			
			// Some formats the view for the chrominance data is not the same size as the luminance view.
			// Thus, we need to modify the rect sizes according to the format
			// Note: DXGI_FORMAT_YUY2, and DXGI_FORMAT_AYUV are not included here because they don't require a chrominance view
			const UINT numRects = viewSpaceRects.size();
			RECT *pModifiedRects = nullptr;
			float xScale = 1.0f;
			float yScale = 1.0f;
			bool bModifyRects = false;
			if( numRects > 0 )
			{
				switch( m_Format )
				{
				default:
					assert( !"Test needs to be updated to modify the rects of the chrominance view to match the intended size" );
					break;
				case DXGI_FORMAT_NV12:
					// When the view is created using DXGI_FORMAT_R8G8_{UNORM|UINT}, the driver creates a view for the chrominance data, 
					// with pixel dimensions halved in both x and y from the luminance view.  R8 contains the U value and G8 contains the V value.
					bModifyRects = true;
					xScale = 0.5f;
					yScale = 0.5f;
				}
			}

			// Modify the rects
			if( bModifyRects )
			{
				pModifiedRects = new RECT[numRects];
				if( GetFramework()->LogVerbose() )
				{
					WriteToLog( _T("Rects will be modified for the chrominance view.") );
				}
				std::vector<RECT> modifiedRects = GetModifiedRects( m_rects, numRects, xScale, yScale );
				assert( modifiedRects.size() == numRects );
				ZeroMemory( pModifiedRects, sizeof(pModifiedRects) );
				for( UINT i = 0; i < numRects; i++ )
				{
					pModifiedRects[i] = modifiedRects[i];
				}
			}
			else
			{
				// Don't modify the rects
				pModifiedRects = m_rects;
			}

			assert( pModifiedRects );
			if( GetFramework()->LogVerbose() )
			{
				if( bPassNullRectList )
				{
					WriteToLog( _T("NULL rect(s) passed to ClearView.") );
				}
				else
				{
					WriteToLog( _T("%u rect(s) passed to ClearView (Left, Top, Right, Bottom):"), numRects );
					WriteRects( pModifiedRects, numRects );
				}
			}
			GetEffectiveContext1()->ClearView(
				pViewChrominanceData, 
				ClearColor,
				pModifiedRects, 
				numRects );

			// Clean up the modified RECTs
			if( bModifyRects )
			{
				delete [] pModifiedRects;
				pModifiedRects = NULL;
			}
		}

		// Clear color values are converted/clamped to the destination format as appropriate per D3D conversion rules. 
		// E.g. if the format of the view is R8G8B8A8_UNORM, inputs are clamped to 0.0f to 1.0f (INF/NaN to 0).
		// For Video Views with YUV or YCbBr formats, no color space conversion happens – and in cases where the format name doesn’t indicate _UNORM vs. _UINT etc., _UINT is assumed
		colorConvertFormat = m_bViewHasFormat ? viewFormat : m_Format;
		if( !ConvertAndClampClearColorToFormat( ClearColor, colorConvertFormat, ClampedClearColor ) )
		{
			WriteToLog( _T("ConvertAndClampClearColorToFormat() failed.") );
			throw CTosser( RESULT_FAIL );
		}

		if( GetFramework()->LogVerbose() )
		{
			WriteToLog( _T("Converted/Clamped float ClearColor[] = { %f, %f, %f, %f }."), 
				ClampedClearColor[0], ClampedClearColor[1], ClampedClearColor[2], ClampedClearColor[3] );
		}

		if ( bNeedChrominanceDataView )
		{
			// If we have two views, with different formats then the color might of had some components dropped
			assert( m_bViewHasFormat );
			colorConvertFormat = viewFormatChrominanceData;
			if( !ConvertAndClampClearColorToFormat( ClearColor, colorConvertFormat, ClampedClearColorChrominanceData ) )
			{
				WriteToLog( _T("ConvertAndClampClearColorToFormat() failed.") );
			}

			// Combine the Luminance and Chromiance into a single structure to make it easier to check against later
			ClampedClearColorCombinedLuminanceAndChrominanceData[0] = ClampedClearColor[0];					// Y
			ClampedClearColorCombinedLuminanceAndChrominanceData[1] = ClampedClearColorChrominanceData[0];	// U
			ClampedClearColorCombinedLuminanceAndChrominanceData[2] = ClampedClearColorChrominanceData[1];	// V
			ClampedClearColorCombinedLuminanceAndChrominanceData[3] = ClampedClearColor[3];

			if( GetFramework()->LogVerbose() )
			{
				WriteToLog( _T("Converted/Clamped chrominance float ClearColor[] = { %f, %f, %f, %f }."), 
					ClampedClearColorChrominanceData[0], ClampedClearColorChrominanceData[1], ClampedClearColorChrominanceData[2], ClampedClearColorChrominanceData[3] );

				WriteToLog( _T("Converted/Clamped combined luminance and chrominance float ClearColor[] = { %f, %f, %f, %f } (Y, U, V, A)."), 
					ClampedClearColorCombinedLuminanceAndChrominanceData[0], ClampedClearColorCombinedLuminanceAndChrominanceData[1], ClampedClearColorCombinedLuminanceAndChrominanceData[2], ClampedClearColorCombinedLuminanceAndChrominanceData[3] );
			}
		}

		if ( p2DResolved )
		{
			for(UINT Array = m_FirstArraySlice; Array < m_FirstArraySlice + m_ViewArraySize; ++ Array )
			{
				GetEffectiveContext1()->ResolveSubresource( 
					p2DResolved, 
					Array * m_MipLevels + m_MipSlice, 
					p2D,
					Array * m_MipLevels + m_MipSlice, 
					m_Format );
			}
			GetEffectiveContext1()->CopyResource( pStaging, p2DResolved );
		}
		else
		{
			GetEffectiveContext1()->CopyResource( pStaging, p2D );
		}

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			LogError( ClearView_Inl, __LINE__, L"ExecuteEffectiveContext() failed", hr, ERRORCODE_TYPE_HRESULT );
			throw CTosser(RESULT_FAIL);
		}

		int MisMatchCount = 0;
		int BackGroundMisMatchCount = 0;
		const int reportLimit = 4;

		RECT * pRects = & pRectList[m_RectScenario].pRects[m_RectScenario];

		assert ( d.Height == d.Width );
		UINT MipWidth =  d.Height >> m_MipSlice;

		bool bFailed = false;

		for(UINT Array = m_FirstArraySlice; Array < m_FirstArraySlice + m_ViewArraySize; ++ Array )
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			hr = GetImmediateContext1()->Map( pStaging, Array * m_MipLevels + m_MipSlice, D3D11_MAP_READ, 0, & msr );
			if ( FAILED (hr ) )
			{
				LogError( ClearView_Inl, __LINE__, L"Failed to map staging resource", hr, ERRORCODE_TYPE_HRESULT );
				throw CTosser( RESULT_FAIL );     
			}

			if ( m_bIsYUV )
			{
				CFormatHelper::ReformatPixels( 
					CONV_FROM_FORMAT, m_Format, m_Scratch, MipWidth, MipWidth, 1, msr.pData, msr.RowPitch );

				//
				// The data in ReformatPixels is always treated as UNORM. See if we need to convert to UINT ...
				//
				bool bConvertToUINT = false;
				if( m_bVideoView && m_bIsYUV )
				{
					// YUV Video views don't have a view format. Thus they should get treated as UINT according to the spec:
					// For Video Views with YUV or YCbBr formats, no color space conversion happens – 
					//		and in cases where the format name doesn’t indicate _UNORM vs. _UINT etc., _UINT is assumed (so input 235.0f maps to 235
					bConvertToUINT = true;
				}
				else
				{
					// We are using either a video view where the format specifies UNORM vs UINT, RTV, or UAV ... so the view has a format
					const DXGI_FORMAT effectiveFormat = m_bVideoView ? m_Format : viewFormat;
					assert( effectiveFormat != DXGI_FORMAT_UNKNOWN );
					const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(effectiveFormat);

					// A limitation in the test code requires that the chrominance data view be the same type (UINT, UNORM) as the luminance view.
					// To fix this in the future, the test could need to convert the different components of pScratch based on the view.
					if( bNeedChrominanceDataView )
					{
						const DXGIFormatInfo* pTexFormatInfoChrominanceData = CFormatInfoMap::Find(viewFormatChrominanceData);
						if( (pTexFormatInfo->dwFlags & FF_UINT) &&
							(pTexFormatInfoChrominanceData->dwFlags & FF_UINT) )
						{
							// They match...
						}
						else if(	(pTexFormatInfo->dwFlags & FF_UNORM) &&
									(pTexFormatInfoChrominanceData->dwFlags & FF_UNORM) )
						{
							// They match...
						}
						else
						{
							// TODO: They don't match! This is a test issue and should be fixed in the future
							LogError( __FILEW__, __LINE__, L"Test limitation. The views need to be the same type or the conversion will fail.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
							throw CTosser( RESULT_FAIL );
						}
					}

					// We have ensured that the luminance view and chrominance data view are the same type (see above)...
					if( pTexFormatInfo->dwFlags & FF_UINT )
					{
						bConvertToUINT = true;
					}
					else if( pTexFormatInfo->dwFlags & FF_UNORM )
					{
						// Nothing to do...
						bConvertToUINT = false;
					}
					else
					{
						LogError( __FILEW__, __LINE__, L"Video format is used, but the view format is not UINT or UNORM type.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
						throw CTosser( RESULT_FAIL );
					}
				}

				if( bConvertToUINT )
				{
					// Convert from UNORM to UINT
					const UINT resNumElements = MipWidth * MipWidth;
					for( UINT i = 0; i < resNumElements; i++ )
					{
						m_Scratch[i].comp[0] *= 255.f;
						m_Scratch[i].comp[1] *= 255.f;
						m_Scratch[i].comp[2] *= 255.f;
						//m_Scratch[i].comp[3] *= 255.f;	// We don't multiple the Alpha channel because the ReformatPixels func already sets this to 1.0f;
					}
				}
			}
			else
			{
				//
				// ConvertPixels assumes a tight pitch (0); so we need to account for a hardware pitch
				// We will make a copy of the data that is tightly pitched
				//

				const UINT uNumBytesPerElement = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format ) / 8;
				const UINT uWidth = MipWidth;
				const UINT uHeight = MipWidth;
				const UINT uDepth = 1;
				const UINT uTightPackedSizeInBytes = uWidth * uHeight * uDepth* uNumBytesPerElement;
				BYTE *pTightPackedCopy = new BYTE[uTightPackedSizeInBytes];

				// Ensure the memory for the copy is empty
				ZeroMemory( pTightPackedCopy, uTightPackedSizeInBytes );

				// For each depth slice...
				for( UINT depthSlice = 0; depthSlice < uDepth; depthSlice++ )
				{
					// For each row slice...
					for( UINT row = 0; row < uHeight; row++ )
					{
						// Get the pointer to the start of the row (this is could have a hardware pitch)
						BYTE *pCurrRow = reinterpret_cast<BYTE*>(msr.pData) + (depthSlice * msr.DepthPitch) + (row * msr.RowPitch);
						
						// Get the pointer to the available slot in our tight packed copy
						BYTE *pCurrTightPackedCopyRow = pTightPackedCopy + (depthSlice * uHeight * uWidth * uNumBytesPerElement) + (row * uWidth * uNumBytesPerElement);
						assert( pCurrTightPackedCopyRow < (pTightPackedCopy + uTightPackedSizeInBytes) );

						// Copy the current row into our tightly packed copy's row
						memcpy(	reinterpret_cast<void*>(pCurrTightPackedCopyRow), reinterpret_cast<void*>(pCurrRow), uWidth * uNumBytesPerElement );
					}
				}

				// Now convert using the tightly packed copy...
				if ( !CFormatHelper::ConvertPixels( 
					CONV_FROM_FORMAT, m_Format, m_Scratch, uWidth, uHeight, uDepth, pTightPackedCopy ) )
				{
					delete[] pTightPackedCopy;
					LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
					throw CTosser(RESULT_FAIL);
				}

				// Cleanup the tight packed copy
				delete [] pTightPackedCopy;
			}
			
			const float* pEffectiveBackgroundColorClamped = bNeedChrominanceDataView ? ClampedBackGroundClearColorCombinedLuminanceAndChrominanceData : ClampedBackGroundClearColor;
			const float* pEffectiveClearColorClamped = bNeedChrominanceDataView ? ClampedClearColorCombinedLuminanceAndChrominanceData : ClampedClearColor;
			float expectedClearColor[4];
			float expectedBackgroundClearColor[4];

			for ( int y = 0; y < (int)MipWidth; ++y )
			for ( int x = 0; x < (int)MipWidth; ++x )
			{
				FLOAT_COLOR & ReformattedResult = m_Scratch[ x + y * MipWidth ];

				bool bInside = pRectList[m_RectScenario].Count == 0 ? true : false;
				for( UINT r = 0; r < pRectList[m_RectScenario].Count; ++ r )
				{
					// Note this code also check to see if the rect is empty. We are never inside an empty rect.
					if ( x >= pRectList[m_RectScenario].pRects[r].left && x < pRectList[m_RectScenario].pRects[r].right &&
						 y >= pRectList[m_RectScenario].pRects[r].top  && y < pRectList[m_RectScenario].pRects[r].bottom )
					{
						bInside = true;
					}
				}

				UINT ChannelMatch = 0;
				UINT BackgroundMatch = 0;
				for( UINT c=0; c< 4; ++c )
				{
					CFormatHelper::COMPONENT_ENCODING Encoding = CFormatHelper::ComponentEncoding( m_Format, c );
					UINT ChannelBits = CFormatHelper::ComponentBits( m_Format, c );
					float CloseToMaxInt = (float)( ( 1 << (ChannelBits-1) ) - 4 );
					float Tolerance = 2.f * CFormatHelper::MachineEpsilon( Encoding, ChannelBits );

					// Some formats will have different colors depending on the pixel due to the fact of how the ClearColor maps to the underlying YUV channels. For example,
					// A YUY2 surface with a R8G8B8A8_UNORM RTV maps like this: Y0 --> R8, U0 --> G8, Y1 --> B8, V0 --> A8. Notice the Y0 and Y1, those are adjacent pixels.
					// Thus with 1 ClearView() call, it forms a stripe pattern if used with a D3D view like R8G8B8A8_UINT. Regardless, we need to be sure we pick the correct component based on the pixel
					const float backgroundColorComponentValueClamped = GetExpectedComponentValue( ClampedBackGroundClearColor, ClampedBackGroundClearColorChrominanceData, m_Format, viewFormat, viewFormatChrominanceData, x, y, c );
					const float clearColorComponentValueClamped = GetExpectedComponentValue( ClampedClearColor, ClampedClearColorChrominanceData, m_Format, viewFormat, viewFormatChrominanceData, x, y, c );
					expectedBackgroundClearColor[c] = backgroundColorComponentValueClamped;
					expectedClearColor[c] = clearColorComponentValueClamped;

					if ( m_bIsYUV )
					{
						//YUV<->RGB round trip induces further error
						Tolerance *= 2.f;
					}
					
					if ( CFormatHelper::IsComponentPresent ( m_Format, c ) )
					{
						if ( IsNaN[c] )
						{
							if ( !( ReformattedResult.comp[c] == ReformattedResult.comp[c] ) || fabsf( 0.f - ReformattedResult.comp[c] ) < Tolerance )
							{
								ChannelMatch++;
							}
						}
						else if ( IsInf[c] &&
								ReformattedResult.comp[c] == *(float*)&(INFP) )
						{
							ChannelMatch++;
						}
						else if( IsInf[c] &&
								bUINT && ReformattedResult.comp[c] >= CloseToMaxInt )
						{
							// 32760 is just about MAX_INT16
							ChannelMatch++;
						}
						else
						{
							if ( fabsf( clearColorComponentValueClamped - ReformattedResult.comp[c] ) < Tolerance )
							{
								ChannelMatch++;
							}
						}
						if ( fabsf( backgroundColorComponentValueClamped - ReformattedResult.comp[c] ) < Tolerance    )
						{
							BackgroundMatch ++;
						}
					}
					else
					{
						ChannelMatch++;
						BackgroundMatch++;
					}
				}
				if ( bInside ) 
				{
					if ( ChannelMatch != 4 )
					{
						bFailed = true;
						m_TextView[x+y*(int)MipWidth] = BackgroundMatch == 4 ? TCHAR('B') : (TCHAR)'X';
						if ( ++MisMatchCount < reportLimit )
						{
							WriteColorFailure( _T("Clear color mismatch"), x, y, expectedClearColor, ReformattedResult );
							if ( m_bIsYUV && m_bVideoView )
							{
								WriteYUVTransform(_T("expected color"), expectedClearColor );
								WriteYUVTransform(_T("actual color"), (float*)&ReformattedResult );
							}
						}
						else if ( MisMatchCount == reportLimit )
						{
							WriteToLog( _T("   ---- Suppressing further mismatch reports for this test case ----") );
						}
					}
					else
					{
						m_TextView[x+y*(int)MipWidth] = (TCHAR)'+';
					}
				}
				else
				{
					if ( BackgroundMatch != 4 )
					{
						bFailed = true;
						m_TextView[x+y*(int)MipWidth] = ChannelMatch == 4 ? TCHAR('C') : (TCHAR)'!';
						if ( ++BackGroundMisMatchCount < reportLimit )
						{
							WriteColorFailure( _T("Background color mismatch"), x, y, expectedBackgroundClearColor, ReformattedResult );
							if ( m_bIsYUV && m_bVideoView )
							{
								WriteYUVTransform(_T("expected color"), expectedBackgroundClearColor );
								WriteYUVTransform(_T("actual color"), (float*)&ReformattedResult );
							}
						}
						else if ( BackGroundMisMatchCount == reportLimit )
						{
							WriteToLog( _T("   ---- Suppressing further background mismatch reports for this test case ----") );
						}
					}
					else
					{
						m_TextView[x+y*(int)MipWidth] = (TCHAR)'#';
					}
				}
			}
			GetImmediateContext1()->Unmap( pStaging, Array * m_MipLevels + m_MipSlice );
			if ( MisMatchCount || BackGroundMisMatchCount )
			{
				if( GetFramework()->LogVerbose() )
				{
					WriteToLog( _T("Dumping text representation of results. + = inside + correct, # = outside + correct, X = inside+incorrect, ! = outside+incorrect, B = inside but background color, C = outside but clear color") );

					TCHAR Format[100];
					assert ( ARRAYSIZE(Format) >= MipWidth + 1 );
					for( UINT y = 0; y < MipWidth; ++y )
					{
						memcpy(Format, m_TextView.m_p + y * MipWidth, sizeof( TCHAR ) * MipWidth);
						Format[MipWidth] = 0;
						WriteToLog( Format );
					}
				}
				else
				{
					WriteToLog( _T("If you want more detailed output and a text representation of results dumped to the log, please run the test with -logverbose on the command line.") );
				}
			}
			if ( bFailed )
			{
				throw CTosser( RESULT_FAIL );
			}
		}
	}
	catch ( CTosser & r )
	{
		return r.m_tr;
	}

	return RESULT_PASS;
}
