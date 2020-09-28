#include "ScaleView.h"

void ScaleView::InitTestParameters()
{
	SetRootTestFactor( GetOutputTypeBaseRFactor() * GetViewportsBaseRFactor() );
	m_bUseIndex = true;
}


TEST_RESULT ScaleView::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
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
		pDestViewportIndices = (UINT*) mappedResDest.pData;
	}

	//create a gradient:
	//from left to right on evens
	//from top to bottom on odds
	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		UINT offset = i*4;

		//top left
		if (m_bUseIndex) { pDestViewportIndices[offset] = i; }
		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];

		//top right
		if (m_bUseIndex) { pDestViewportIndices[1+offset] = i; }
		pDestVerts[1+offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[1+offset].Color = (i % 2) ? MultiColors[15 - i] : MultiColors[i];

		//bottom left
		if (m_bUseIndex) { pDestViewportIndices[2+offset] = i; }
		pDestVerts[2+offset].Pos = pos(0.0f, 1.0f);
		pDestVerts[2+offset].Color = (i % 2) ? MultiColors[i] : MultiColors[15 - i];

		//bottom right
		if (m_bUseIndex) { pDestViewportIndices[3+offset] = i; }
		pDestVerts[3+offset].Pos = pos(1.0f, 1.0f);
		pDestVerts[3+offset].Color = (i % 2) ? MultiColors[15 - i] : MultiColors[15 - i];
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	if (m_bUseIndex) GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT ScaleView::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	FLOAT RelativeEpsilon = 0.005f;
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
		UINT PixelCount[16]; //Last one is for unrecognized colors

		for ( UINT i = 0; i < m_NumViewports; ++i )
		{
			RECT ViewRect = ViewportToRect( m_Viewports[i], g_App.m_D3DOptions.FeatureLevel );
			bool bViewportFailed = false;
			
			for ( UINT k = 0; k < 16; ++k ) PixelCount[k] = 0; //passed pixels
			UINT failedPix = 0;  //pixels the colour of the other half of the viewport

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

					color TargetColor;

                    UINT32 OverdrawCount = 0;
                    if( m_OutputType == OUTPUT_TYPE_UAV )
                    {
                        OverdrawCount = pOverdrawData[x];
                    }
                    UINT32 ExpectedOverdrawCount = 1;

					//calculate the colour that should be in this pixel
					if (i % 2) //verticle
					{
						FLOAT fraction = (FLOAT) x + 0.5f;
						if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
						{
							fraction -= ViewportShifts[i][0];
						}
						fraction /= m_Viewports[i].Width;
						TargetColor.r = (MultiColors[i].r + (MultiColors[15 - i].r - MultiColors[i].r) * fraction);
						TargetColor.g = (MultiColors[i].g + (MultiColors[15 - i].g - MultiColors[i].g) * fraction);
						TargetColor.b = (MultiColors[i].b + (MultiColors[15 - i].b - MultiColors[i].b) * fraction);
					}
					else //horizontal
					{
						FLOAT fraction = (FLOAT) y + 0.5f;
						if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
						{
							fraction -= ViewportShifts[i][1];
						}
						fraction /= m_Viewports[i].Height;
						TargetColor.r = (MultiColors[i].r + (MultiColors[15 - i].r - MultiColors[i].r) * fraction);
						TargetColor.g = (MultiColors[i].g + (MultiColors[15 - i].g - MultiColors[i].g) * fraction);
						TargetColor.b = (MultiColors[i].b + (MultiColors[15 - i].b - MultiColors[i].b) * fraction);				
					}

					if ( abs(TargetColor.r - Colors.r) > RelativeEpsilon ||
						 abs(TargetColor.g - Colors.g) > RelativeEpsilon ||
						 abs(TargetColor.b - Colors.b) > RelativeEpsilon )
					{
                        CurPixelColorPassed = false;
						bViewportFailed = true;
						tRes = RESULT_FAIL;
						++failedPix;
					}
					else //the pixel passed
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
                            ++failedPix;
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
				WriteToLog( "%d failing pixels/overdraw in viewport.", failedPix);
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
