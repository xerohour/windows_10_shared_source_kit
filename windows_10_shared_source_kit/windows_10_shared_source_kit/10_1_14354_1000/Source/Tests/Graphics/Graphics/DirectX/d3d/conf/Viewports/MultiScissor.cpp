#include "MultiScissor.h"

void MultiScissor::InitTestParameters()
{
	m_bUseIndex = true;

	testfactor::RFactor rFact;
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports );
	CTestCaseParameter< UINT > *pScissorEnabledParam = AddParameter< UINT >( _T("ScissorEnabled"), &m_scissorEnabled );

	rFact = AddParameterValueSet< UINT >( pNumViewportsParam, new CRangeValueSet< UINT >( 1, 16, 1 ) );

	UINT uScissorSetSize = 1;
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		++uScissorSetSize;
	}
	rFact = rFact * AddParameterValueSet< UINT >( pScissorEnabledParam, new CRangeValueSet< UINT >( 0, uScissorSetSize, 1 ) );
	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT MultiScissor::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
	if (m_scissorEnabled != 0)
		GetEffectiveContext()->RSSetState( m_pRSScissor );
	else
		GetEffectiveContext()->RSSetState( m_pRSView );

	UINT numHorizontal, numVertical, width, height, index;
	BOOL bExtra = FALSE;

	m_scissorSize = 15;

	// setup the viewport orientation
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

			//setup the corresponding scissor- make each one a little smaller than the viewport
			if (m_scissorEnabled == 1)
			{
				m_Scissors[index].left = (UINT) m_Viewports[index].TopLeftX + (m_scissorSize);
				m_Scissors[index].right = (UINT) m_Viewports[index].TopLeftX + (2*m_scissorSize);
				m_Scissors[index].top = (UINT) m_Viewports[index].TopLeftY + m_scissorSize;
				m_Scissors[index].bottom = (UINT) m_Viewports[index].TopLeftY + (2*m_scissorSize);
			}
			else if (m_scissorEnabled == 2)
			{
				m_Scissors[index] = ViewportToRect( m_Viewports[index], D3D_FEATURE_LEVEL_11_0 );
			}
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

		if (m_scissorEnabled == 1)
		{
			m_Scissors[index].left = (UINT) m_Viewports[index].TopLeftX + (m_scissorSize);
			m_Scissors[index].right = (UINT) m_Viewports[index].TopLeftX + (2*m_scissorSize);
			m_Scissors[index].top = (UINT) m_Viewports[index].TopLeftY + m_scissorSize;
			m_Scissors[index].bottom = (UINT) m_Viewports[index].TopLeftY + (2*m_scissorSize);
		}
		else if (m_scissorEnabled == 2)
		{
			m_Scissors[index] = ViewportToRect( m_Viewports[index], D3D_FEATURE_LEVEL_11_0 );
		}
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

	if (m_scissorEnabled != 0)
		GetEffectiveContext()->RSSetScissorRects( m_NumViewports, m_Scissors );  //same number of viewports and scissors in this test

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
		pDestViewportIndices = (UINT*) mappedResDest.pData;
	}

	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		UINT offset = i*4;

		if (m_bUseIndex)
		{
			pDestViewportIndices[offset] = i;
			pDestViewportIndices[1+offset] = i;
			pDestViewportIndices[2+offset] = i;
			pDestViewportIndices[3+offset] = i;
		}

		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];

		pDestVerts[1+offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[1+offset].Color = MultiColors[i];
	
		pDestVerts[2+offset].Pos = pos(0.0f, 1.0f);
		pDestVerts[2+offset].Color = MultiColors[i];

		pDestVerts[3+offset].Pos = pos(1.0f, 1.0f);
		pDestVerts[3+offset].Color = MultiColors[i];
	}

    if (m_bUseIndex) GetEffectiveContext()->Unmap(m_pVIBuffer,0);
	GetEffectiveContext()->Unmap(m_pVBuffer,0);


	return RESULT_PASS;
}

TEST_RESULT MultiScissor::ExecuteTestCase()
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
		
		for ( UINT i = 0; i < m_NumViewports; ++i )
		{
		
			UINT FailedPixIn = 0, FailedPixOut = 0, PassedPix = 0;
			RECT ViewRect = ViewportToRect( m_Viewports[i], g_App.m_D3DOptions.FeatureLevel );
			bool bViewportFailed = false;
			
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

			for ( UINT y = 0; y < UINT( overlapHeight ); ++y )
			{				
				for ( UINT x = 0; x < UINT( overlapWidth ); ++x )
				{
                    bool CurPixelColorPassed = false;

					bool bInScissor;
					color Colors = color(
						CDXGIFloat16(pData[x*4]),
						CDXGIFloat16(pData[x*4+1]),
						CDXGIFloat16(pData[x*4+2])
					);

					color TargetColor;

                    UINT32 OverdrawCount = 0;
                    if( m_OutputType == OUTPUT_TYPE_UAV )
                    {
                        OverdrawCount = pOverdrawData[x];
                    }
                    UINT32 ExpectedOverdrawCount = 0;

					//We're within the scissor rectangle
					if (m_scissorEnabled != 1 || 
						(x >= m_scissorSize && x < (2 * m_scissorSize)
					 && y >= m_scissorSize && y < (2 * m_scissorSize)))
					{
						bInScissor = true;
						TargetColor = MultiColors[i];	
                        ExpectedOverdrawCount = 1;
					}
					else //outside it should be black
					{
						TargetColor = BLACK;
						bInScissor = false;
                        ExpectedOverdrawCount = 0;
					}

					CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

					if ( RelativeDiff[0] > RelativeEpsilon ||
						 RelativeDiff[1] > RelativeEpsilon ||
						 RelativeDiff[2] > RelativeEpsilon )
					{
                        CurPixelColorPassed = false;

						if (bInScissor)
							++FailedPixIn;
						else
							++FailedPixOut;

						bViewportFailed = true;
						tRes = RESULT_FAIL;	
					}
					else
					{
                        CurPixelColorPassed = true;
						++PassedPix;
					}

                    // If outputting to UAVs, check for proper OverdrawCount
                    if( m_OutputType == OUTPUT_TYPE_UAV && CurPixelColorPassed )
                    {
                        if( OverdrawCount != ExpectedOverdrawCount )
                        {
                            tRes = RESULT_FAIL;

                            // We executed the color comparison logic already, so subtract 1 from passing pixel count
                            --PassedPix;

                            // Increment the unexpected pixel count
                            if ( bInScissor )
						        ++FailedPixIn;
					        else
						        ++FailedPixOut;
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
				WriteToLog( "Passing pixels for viewport %d: %d", i, PassedPix );

				if ( 0 < FailedPixIn )
				{
					WriteToLog( "Unrecognized pixel colors/overdraw inside scissor for viewport %d: %d", i, FailedPixIn );
				}

				if ( 0 < FailedPixOut )
				{
					WriteToLog( "Unrecognized pixel colors/overdraw outside scissor for viewport %d: %d", i, FailedPixOut );
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
