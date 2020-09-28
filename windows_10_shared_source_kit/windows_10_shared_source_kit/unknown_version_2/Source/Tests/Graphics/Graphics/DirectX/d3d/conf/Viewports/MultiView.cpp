#include "MultiView.h"
#include "D3DMSHelper.h"
#include "FormatHelper.h"

#define MS_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM

void MultiView::InitTestParameters()
{
	m_bUseIndex = true;
	SetRootTestFactor( GetOutputTypeBaseRFactor() * GetViewportsBaseRFactor() );
}


TEST_RESULT MultiView::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	GetEffectiveContext()->RSSetState( m_pRSView );

	UINT numHorizontal, numVertical, width, height, index;
	BOOL bExtra = FALSE;

	// Setup the viewports
	switch ( m_NumViewports )
	{
	case 1:
	case 3:
		numVertical = 1;
		break;

	case 5:
	case 7:
	case 11:
		bExtra = TRUE;
	case 2:
	case 4:
	case 6:
	case 14:
		numVertical = 2;
		break;

	case 13:
		bExtra = TRUE;
	case 8:
	case 12:
	case 16:
		numVertical = 4;
		break;

	case 10:
		bExtra = TRUE;
	case 9:
	case 15:
		numVertical = 3;
		break;

	default:
		numVertical = 1;
	};

	numHorizontal = m_NumViewports / numVertical;

	height = m_RTHeight / ( numVertical + bExtra ); //Leave space at the bottom for an extra
	width = m_RTWidth / numHorizontal; 

	for ( UINT x = 0; x < numHorizontal; ++x )
	{
		for ( UINT y = 0; y < numVertical; ++y )
		{
			index = (x * numVertical) + y; 
			
			if ( 14 == m_NumViewports && x > 0 )
			{
				//Special case for 14, 'cause I don't want people pissed about a black band
				// nor do I want to use a larger/divisible by 7 RT size
				m_Viewports[index].Height = (float) (m_RTHeight - height);
			}
			else
				m_Viewports[index].Height = (float) height;

			m_Viewports[index].Width = (float) width;
			m_Viewports[index].TopLeftX = (float) (x * width);
			m_Viewports[index].TopLeftY = (float) (y * height);
			m_Viewports[index].MinDepth = 0.0f;
			m_Viewports[index].MaxDepth = 1.0f;
		}
	}

	if ( bExtra )
	{
		m_Viewports[++index].Height = (float) height;
		m_Viewports[index].Width = (float) m_RTWidth;
		m_Viewports[index].TopLeftX = 0.0f;
		m_Viewports[index].TopLeftY = (float) (numVertical * height);
		m_Viewports[index].MinDepth = 0.0f;
		m_Viewports[index].MaxDepth = 1.0f;
	}

	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		for ( UINT v = 0; v < m_NumViewports; ++v )
		{
			m_Viewports[v].TopLeftX += ViewportShifts[v][0];
			m_Viewports[v].TopLeftY += ViewportShifts[v][1];
		}
	}
	
	GetEffectiveContext()->RSSetViewports( m_NumViewports, m_Viewports );

	VERTS *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if ( FAILED(GetEffectiveContext()->Map( m_pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog("Map on vertex buffer failed.");
		return RESULT_FAIL;
	}
	pDestVerts = (VERTS*)mappedRes.pData;


	UINT *pDestViewportIndices = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedResDest;
	if (m_bUseIndex) 
	{
		if ( FAILED( GetEffectiveContext()->Map( m_pVIBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResDest ) ) )
		{
			WriteToLog("Map on vertex buffer failed.");
			return RESULT_FAIL;
		}
		pDestViewportIndices = (UINT*) mappedResDest.pData;;
	}

	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		UINT offset = i*4;
		
		//left rectangle
		if (m_bUseIndex) { pDestViewportIndices[offset] = i; }
		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];

		if (m_bUseIndex) { pDestViewportIndices[1+offset] = i; }
		pDestVerts[1+offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[1+offset].Color = MultiColors[i];

		if (m_bUseIndex) { pDestViewportIndices[2+offset] = i; }
		pDestVerts[2+offset].Pos = pos(0.0f, 1.0f);
		pDestVerts[2+offset].Color = MultiColors[i];

		if (m_bUseIndex) { pDestViewportIndices[3+offset] = i; }
		pDestVerts[3+offset].Pos = pos(1.0f, 1.0f);
		pDestVerts[3+offset].Color = MultiColors[i];
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	if (m_bUseIndex) GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT MultiView::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	FLOAT RelativeEpsilon = 0.0005f;
	FLOAT RelativeDiff[3];

	D3D11_MAPPED_SUBRESOURCE texMap;
	D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

    for( UINT vp = 0; vp < m_NumViewports; vp++ )
    {
    	GetEffectiveContext()->Draw( 4, vp*4 );
    }

	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes the effective context
	{
		UINT16 *pData = NULL; //On the assumption that we're looking at 16-bit floats.
        UINT32 *pOverdrawData = NULL;
		UINT PixelCount[17]; //Last one is for unrecognized colors

		for ( UINT i = 0; i < m_NumViewports; ++i )
		{
			RECT ViewRect = ViewportToRect( m_Viewports[i], g_App.m_D3DOptions.FeatureLevel );
			bool bViewportFailed = false;
			
			for ( UINT k = 0; k < 17; ++k ) PixelCount[k] = 0;

			// Get the pointer to the top left of the overlapping viewport and render target (Top left of RT = (0,0))
			const LONG startY = max(0, ViewRect.top);	// Render target map is 0
			const LONG startX = max(0, ViewRect.left);	// Render target map is 0
			const LONG endY = min((LONG)m_RTHeight, ViewRect.bottom);
			const LONG endX = min((LONG)m_RTWidth, ViewRect.right);
			const LONG overlapWidth = endX - startX;
			const LONG overlapHeight = endY - startY;
			const LONG numBytesPerRow = texMap.RowPitch;
			pData = (UINT16*)((BYTE*)texMap.pData + (startY * numBytesPerRow));	// Get the point to the correct row
			pData += startX * 4;

            if( m_OutputType == OUTPUT_TYPE_UAV )
            {
                pOverdrawData = (UINT32*)((BYTE*)overdrawTexMap.pData + (startY * overdrawTexMap.RowPitch));
                pOverdrawData += startX;
            }

			for ( LONG y = 0; y < overlapHeight; ++y )
			{				
				for ( LONG x = 0; x < overlapWidth; ++x )
				{
                    bool CurPixelColorPassed = false;

					color Colors = color(
						CDXGIFloat16(pData[x*4]),
						CDXGIFloat16(pData[x*4+1]),
						CDXGIFloat16(pData[x*4+2])
					);

                    UINT32 OverdrawCount = 0;
                    if( m_OutputType == OUTPUT_TYPE_UAV )
                    {
                        OverdrawCount = pOverdrawData[x];
                    }
                    UINT32 ExpectedOverdrawCount = 1;

					CalcRelativeDiff( MultiColors[i], Colors, RelativeDiff );

					if ( RelativeDiff[0] > RelativeEpsilon ||
						 RelativeDiff[1] > RelativeEpsilon ||
						 RelativeDiff[2] > RelativeEpsilon )
					{
                        CurPixelColorPassed = false;
						bool bFoundMatch = false;
						bViewportFailed = true;
						tRes = RESULT_FAIL;

						for ( UINT j = 0; j < m_NumViewports; ++j )
						{
							if ( j == i ) continue;
							
							CalcRelativeDiff( MultiColors[j], Colors, RelativeDiff );

							//Check to see if it matches any of the viewport colors set
							if ( RelativeDiff[0] < RelativeEpsilon &&
								 RelativeDiff[1] < RelativeEpsilon &&
								 RelativeDiff[2] < RelativeEpsilon )
							{
								++PixelCount[j];
								
								bFoundMatch = true;
							}
						}

						if ( !bFoundMatch )
						{
							++PixelCount[16];
						}
					}
					else
					{
                        CurPixelColorPassed = true;
						++PixelCount[i];
					}

                    // If outputting to UAVs, check for proper OverdrawCount
                    if( m_OutputType == OUTPUT_TYPE_UAV && CurPixelColorPassed )
                    {
                        if( OverdrawCount != ExpectedOverdrawCount )
                        {
                            bViewportFailed = true;
                            tRes = RESULT_FAIL;

                            // We executed the color comparison logic already, so subtract 1 from passing pixel count
                            --PixelCount[i];

                            // Increment the unexpected pixel count
                            ++PixelCount[16];
                        }
                    }
				}

				pData += texMap.RowPitch / sizeof(UINT16);
                pOverdrawData += overdrawTexMap.RowPitch / sizeof(UINT32);
			}

			if ( bViewportFailed )
			{
				//Output info for log
				WriteToLog( "\n" );
				WriteToLog( "Viewport %d", i );
				WriteToLog( "%s", ToString( m_Viewports[i] ).c_str() );
				WriteToLog( "Passing pixels for viewport %d: %d", i, PixelCount[i] );
				for ( UINT j = 0; j < m_NumViewports; ++j )
				{
					if ( j != i )
					{
						if ( 0 < PixelCount[j] )
							WriteToLog( "Failing pixels from viewport %d found in viewport %d: %d", j, i, PixelCount[j] );
					}
				}

				if ( 0 < PixelCount[16] )
				{
					WriteToLog( "Unrecognized pixel colors/overdraw for viewport %d: %d", i, PixelCount[16] );
				}
			}
		}

		UnMapRT(true);
	}
	else
	{
		WriteToLog( "Failed to map render target." );
		tRes = RESULT_FAIL;
	}

	g_App.Present();

	return tRes;
}

TEST_RESULT ArbitraryView::VerifyMultisample()
{
	HRESULT hr;
	FLOAT_COLOR *pSamples = NULL;
	TEST_RESULT tr = RESULT_PASS;
	if( FAILED(hr = ExecuteEffectiveContext()) )
	{
		WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	pSamples = new FLOAT_COLOR[m_RTWidth * m_RTHeight * m_NumSamples];
	if( !MSHelper::Read(GetFramework(), m_pTextureRT_MS, (float*)pSamples, MS_FORMAT) )
	{
		WriteToLog(_T("MSHelper::Read failed when verifying multisample test case"));
		tr = RESULT_FAIL;
		goto Done;
	}

	FLOAT_COLOR clearColor;
	clearColor.r = clearColor.g = clearColor.b = clearColor.a = 0.0f;

	for( UINT s = 0; s < m_NumSamples; ++s )
	{
		FLOAT_COLOR *pData = pSamples + s * m_RTWidth * m_RTHeight;
		RECT ViewRect = ViewportToRect( m_Viewports[m_ActiveViewport], g_App.m_D3DOptions.FeatureLevel );
		UINT FailedPixIn = 0, FailedPixOut = 0, PassedPix = 0;

		for ( UINT y = 0; y < m_RTHeight; ++y )
		{				
			for ( UINT x = 0; x < m_RTWidth; ++x )
			{
				bool bInViewport;
				FLOAT_COLOR color = pData[x];

				//We're within the viewport
				if ( ViewRect.top <= (LONG)y && (LONG)y < ViewRect.bottom &&
					 ViewRect.left <= (LONG)x && (LONG)x < ViewRect.right )
				{
					bInViewport = true;
				}
				else
				{
					bInViewport = false;
				}
	
				if( color.r != clearColor.r || 
					color.g != clearColor.g || 
					color.b != clearColor.b || 
					color.a != clearColor.a )
				{
					if ( !bInViewport )
					{
						tr = RESULT_FAIL;
						++FailedPixOut;
					}
				}
				else
				{
					++PassedPix;
				}
			}

			pData += m_RTWidth;
		}

		WriteToLog( "\nViewport %d", m_ActiveViewport );
		WriteToLog( "%s", ToString( m_Viewports[m_ActiveViewport] ).c_str() );

		if ( 0 < PassedPix )
			WriteToLog( "Passing pixels: %d", PassedPix );

		if ( 0 < FailedPixOut )
			WriteToLog( "Unexpected colors found while looking outside viewport %d: %d.", m_ActiveViewport, FailedPixOut );
	}

Done:	
	delete [] pSamples;

	return tr;
}
	
void ArbitraryView::InitTestParameters()
{
	testfactor::RFactor rFact;
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports );
	CTestCaseParameter< UINT > *pActiveViewportParam = AddParameter< UINT >( _T("ActiveViewport"), &m_ActiveViewport );

	rFact = AddParameterValueSet< UINT >( pActiveViewportParam, new CRangeValueSet< UINT >( 0, 15, 1 ) );
	rFact = rFact * AddParameterValue< UINT >( pNumViewportsParam, 16 );

	for ( UINT i = 15; i > 1; --i )
	{
		rFact = rFact + AddParameterValueSet< UINT >( pActiveViewportParam, new CRangeValueSet< UINT >( 0, i-1, 1 ) ) *
			AddParameterValue< UINT >( pNumViewportsParam, i );
	}

	CTestCaseParameter<UINT> *pNumSamples = AddParameter<UINT>(_T("NumSamples"), &m_NumSamples);
	CUserValueSet<UINT> *pSampleValues = new CUserValueSet<UINT>();
	pSampleValues->AddValue(0); // MSAA off
	pSampleValues->AddValue(1);
	pSampleValues->AddValue(4);
	testfactor::RFactor rfSamples = AddParameterValueSet(pNumSamples, pSampleValues);

	if( g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		rFact = rFact * rfSamples;
	}

	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT ArbitraryView::SetupTestCase()
{
	TEST_RESULT result = MultiView::SetupTestCase();

    if( m_OutputType == OUTPUT_TYPE_UAV )
    {
        WriteToLog( "Multisampled UAVs are not supported. UAVs with ForcedSampleCount set are tested in WGF11Rasterizer. Skipping test case" );
        return RESULT_SKIP;
    }

	if (result == RESULT_PASS &&
		g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		for ( UINT v = 0; v < m_NumViewports; ++v )
		{
			// adding a -0.1 offset to the Width/Height fractional shift to avoid floating point precision problem
			// since both Width/Height and TopLeftX/TopLeftY use the same set of shift data (ViewportShifts[16][2])
			// refer to Windows 8 bug 30620
			m_Viewports[v].Width += (ViewportShifts[(v*2)%16][0] - (float)0.1);
			m_Viewports[v].Height += (ViewportShifts[(v*2)%16][1]- (float)0.1);
		}

		// Reset all state on FL11+ since MSHelper doesn't preserve state
		UINT strides[] = { sizeof( VERTS ), sizeof( UINT ) };
		UINT offsets[] = { 0, 0 };
		ID3D11Buffer *pBuffers[2] = {
			m_pVBuffer,
			m_pVIBuffer,
		};

		GetEffectiveContext()->IASetVertexBuffers( 0, m_bUseIndex?2:1, pBuffers, strides, offsets );
		GetEffectiveContext()->IASetInputLayout( m_pVLayout );
		GetEffectiveContext()->VSSetShader(m_pVS, NULL, 0 );
		GetEffectiveContext()->GSSetShader(m_pGS, NULL, 0 );
		GetEffectiveContext()->PSSetShader( m_pRTVPS, NULL, 0 );
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCBuffer );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCBuffer );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCBuffer );
		GetEffectiveContext()->OMSetBlendState( m_pBS, 0, D3D10_DEFAULT_SAMPLE_MASK );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		
		GetEffectiveContext()->RSSetViewports( m_NumViewports, m_Viewports );

		if( m_NumSamples > 0 )
		{
			D3D11_TEXTURE2D_DESC texDesc;
			texDesc.Width = m_RTWidth;
			texDesc.Height = m_RTHeight;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = MS_FORMAT;
			texDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
			texDesc.SampleDesc.Count = m_NumSamples;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			HRESULT hr;
			if( FAILED( hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pTextureRT_MS) ) )
			{
				WriteToLog(_T("Failed to create MS RT texture, hr=%s"), D3DHResultToString(hr).c_str());
				return RESULT_FAIL;
			}

			D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
			viewDesc.Format = DXGI_FORMAT_UNKNOWN;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

			if( FAILED( hr = GetDevice()->CreateRenderTargetView(m_pTextureRT_MS, &viewDesc, &m_pRTV_MS) ) )
			{
				WriteToLog(_T("Failed to create MS RT view, hr=%s"), D3DHResultToString(hr).c_str());
				return RESULT_FAIL;
			}

			D3D11_RASTERIZER_DESC rastDesc;
			rastDesc.CullMode = D3D11_CULL_NONE;
			rastDesc.FrontCounterClockwise = true;
			rastDesc.DepthBias = false;
			rastDesc.DepthBiasClamp = 0;
			rastDesc.SlopeScaledDepthBias = 0;
			rastDesc.DepthClipEnable = true;
			rastDesc.ScissorEnable = false;
			rastDesc.MultisampleEnable = true;
			rastDesc.AntialiasedLineEnable = false;
			rastDesc.FillMode = D3D11_FILL_SOLID;
		
			if( FAILED( hr = GetDevice()->CreateRasterizerState(&rastDesc, &m_pRast_MS) ) )
			{
				WriteToLog(_T("Failed to MS rasterizer state, hr=%s"), D3DHResultToString(hr).c_str());
				return RESULT_FAIL;
			}
		}
	}

	return result;
}

void ArbitraryView::CleanupTestCase()
{
	SAFE_RELEASE(m_pRTV_MS);
	SAFE_RELEASE(m_pTextureRT_MS);
	SAFE_RELEASE(m_pRast_MS);
}

TEST_RESULT ArbitraryView::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	float RelativeEpsilon = 0.0005f;
	float RelativeDiff[3];

	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	D3D11_MAPPED_SUBRESOURCE texMap;

	bool bMultisample = (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0) && (m_NumSamples > 0);

	if( bMultisample )
	{
		GetEffectiveContext()->RSSetState(m_pRast_MS);
		GetEffectiveContext()->OMSetRenderTargets(1, &m_pRTV_MS, NULL);
		GetEffectiveContext()->ClearRenderTargetView( m_pRTV_MS, clearColor );
	}
	else
	{
		GetEffectiveContext()->RSSetState( m_pRSView );
		GetEffectiveContext()->OMSetRenderTargets(1, &m_pRTView, m_pDSView);
		GetEffectiveContext()->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, clearColor );
	}

	GetEffectiveContext()->Draw( 4, m_ActiveViewport * 4 );

	if( bMultisample )
	{
		tRes = VerifyMultisample();
	}
	else if ( MapRT( &texMap ) ) // Executes the effective context.
	{
		UINT16 *pData = (UINT16*)texMap.pData; //On the assumption that we're looking at 16-bit floats.
		RECT ViewRect = ViewportToRect( m_Viewports[m_ActiveViewport], g_App.m_D3DOptions.FeatureLevel );
		UINT FailedPixIn = 0, FailedPixOut = 0, PassedPix = 0;

		for ( UINT y = 0; y < m_RTHeight; ++y )
		{				
			for ( UINT x = 0; x < m_RTWidth; ++x )
			{
				bool bInViewport;
				color Colors = color(
					CDXGIFloat16(pData[x*4]),
					CDXGIFloat16(pData[x*4+1]),
					CDXGIFloat16(pData[x*4+2])
					);
				color TargetColor;

				//We're within the viewport
				if ( ViewRect.top <= (LONG)y && (LONG)y < ViewRect.bottom &&
					 ViewRect.left <= (LONG)x && (LONG)x < ViewRect.right )
				{
					bInViewport = true;
					TargetColor = MultiColors[m_ActiveViewport];
				}
				else
				{
					bInViewport = false;
					TargetColor = BLACK;
				}
	
				CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

				if ( RelativeDiff[0] > RelativeEpsilon ||
					RelativeDiff[1] > RelativeEpsilon ||
					RelativeDiff[2] > RelativeEpsilon )
				{
					tRes = RESULT_FAIL;
					if ( bInViewport )
					{
						++FailedPixIn;
					}
					else
					{
						++FailedPixOut;
					}
				}
				else
				{
					++PassedPix;
				}
			}

			pData += texMap.RowPitch / sizeof(UINT16);
		}

		WriteToLog( "\nViewport %d", m_ActiveViewport );
		WriteToLog( "%s", ToString( m_Viewports[m_ActiveViewport] ).c_str() );

		if ( 0 < PassedPix )
			WriteToLog( "Passing pixels: %d", PassedPix );

		if ( 0 < FailedPixIn )
			WriteToLog( "Unexpected colors found while testing viewport %d: %d.", m_ActiveViewport, FailedPixIn );

		if ( 0 < FailedPixOut )
			WriteToLog( "Unexpected colors found while looking outside viewport %d: %d.", m_ActiveViewport, FailedPixOut );

		UnMapRT();
	}
	else
	{
		WriteToLog( "Failed to map render target." );
		tRes = RESULT_FAIL;
	}

	g_App.Present();

	return tRes;
}
