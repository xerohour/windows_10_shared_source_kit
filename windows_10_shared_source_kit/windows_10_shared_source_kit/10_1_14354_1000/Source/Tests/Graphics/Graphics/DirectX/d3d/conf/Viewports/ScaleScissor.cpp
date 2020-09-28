#include "ScaleScissor.h"

void ScaleScissor::InitTestParameters()
{
	m_bUseIndex = true;
	SetRootTestFactor( GetOutputTypeBaseRFactor() * GetViewportsBaseRFactor() );
}


TEST_RESULT ScaleScissor::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
	GetEffectiveContext()->RSSetState( m_pRSScissor );
	
	//setup the viewports
	//each viewport is the size of the render target
	for (UINT i = 0; i < m_NumViewports; ++i)
	{
		m_Viewports[i].Height = (float) m_RTHeight;
		m_Viewports[i].Width = (float) m_RTWidth;
		m_Viewports[i].TopLeftX = 0.0f;
		m_Viewports[i].TopLeftY = 0.0f;
		m_Viewports[i].MinDepth = 0.0f;
		m_Viewports[i].MaxDepth = 1.0f;
	}

	//setup the scissors- one per viewport
	UINT numHorizontal, numVertical, width, height, index;
	BOOL bExtra = FALSE;
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
				//Special case for 14
				m_Scissors[index].bottom = (y * height) + (m_RTHeight - height);
			}
			else
				m_Scissors[index].bottom = (y * height) + height;

			//setup the corresponding scissor- same size as viewport
			m_Scissors[index].left = x * width;
			m_Scissors[index].right = (x * width) + width;
			m_Scissors[index].top = y * height;	
		}
	}

	if ( bExtra )
	{
		m_Scissors[++index].left = 0;
		m_Scissors[index].right = m_RTWidth;
		m_Scissors[index].top = numVertical * height;
		m_Scissors[index].bottom = (numVertical * height) + height;
	}
	
	GetEffectiveContext()->RSSetViewports( m_NumViewports, m_Viewports );
	GetEffectiveContext()->RSSetScissorRects( m_NumViewports, m_Scissors );  //same number of viewports and scissors in this test

	VERTS *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if ( FAILED(GetEffectiveContext()->Map( m_pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog("Map on vertex buffer failed.");
		return RESULT_FAIL;
	}
	pDestVerts = (VERTS*) mappedRes.pData;

	UINT *pDestViewportIndices;
	D3D11_MAPPED_SUBRESOURCE mappedResDest;
	if ( FAILED( GetEffectiveContext()->Map( m_pVIBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResDest ) ) )
	{
		WriteToLog("Map on vertex buffer failed.");
		return RESULT_FAIL;
	}
	pDestViewportIndices = (UINT*) mappedResDest.pData;

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
	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT ScaleScissor::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
    FLOAT RelativeEpsilon = 1.0f;
	D3D11_MAPPED_SUBRESOURCE texMap;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

    for( UINT vp = 0; vp < m_NumViewports; vp++ )
    {
    	GetEffectiveContext()->Draw( 4, vp*4 );
    }

	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes effective context
	{
		UINT16 *pData = NULL; //On the assumption that we're looking at 16-bit floats.
        UINT32 *pOverdrawData = NULL;
		
		for ( UINT i = 0; i < m_NumViewports; ++i )
		{
			UINT FailedPix = 0, PassedPix = 0;
			UINT yOffset = m_Scissors[i].top;
			UINT xOffset = m_Scissors[i].left;
			bool bViewportFailed = false;
			
			pData = (UINT16*)texMap.pData + yOffset * texMap.RowPitch / sizeof(UINT16);			
			pData += xOffset * 4;

            if( m_OutputType == OUTPUT_TYPE_UAV )
            {
                pOverdrawData = (UINT32*)overdrawTexMap.pData + yOffset * overdrawTexMap.RowPitch / sizeof(UINT32);
                pOverdrawData += xOffset;
            }

			//only look within the scissor for this viewport
			for ( int y = 0; y < (m_Scissors[i].bottom - m_Scissors[i].top); ++y )
			{				
				for ( int x = 0; x < (m_Scissors[i].right - m_Scissors[i].left); ++x )
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
                    UINT32 ExpectedOverdrawCount = 1;

					//calculate the colour that should be in this pixel
					if (i % 2) //verticle
					{
						FLOAT fraction = ((FLOAT) x + 0.5f) /m_Viewports[i].Width;
						TargetColor.r = (MultiColors[i].r + (MultiColors[15 - i].r - MultiColors[i].r) * fraction);
						TargetColor.g = (MultiColors[i].g + (MultiColors[15 - i].g - MultiColors[i].g) * fraction);
						TargetColor.b = (MultiColors[i].b + (MultiColors[15 - i].b - MultiColors[i].b) * fraction);
					}
					else //horizontal
					{
						FLOAT fraction = ((FLOAT) y + 0.5f) /m_Viewports[i].Height;
						TargetColor.r = (MultiColors[i].r + (MultiColors[15 - i].r - MultiColors[i].r) * fraction);
						TargetColor.g = (MultiColors[i].g + (MultiColors[15 - i].g - MultiColors[i].g) * fraction);
						TargetColor.b = (MultiColors[i].b + (MultiColors[15 - i].b - MultiColors[i].b) * fraction);				
					}
					if ( abs(TargetColor.r - Colors.r) > RelativeEpsilon ||
						 abs(TargetColor.g - Colors.g) > RelativeEpsilon ||
						 abs(TargetColor.b - Colors.b) > RelativeEpsilon )
					{
                        CurPixelColorPassed = false;
						++FailedPix;
	
						bViewportFailed = true;
						tRes = RESULT_FAIL;	
					}
					else
					{
                        CurPixelColorPassed = false;
						++PassedPix;
					}

                    // If outputting to UAVs, check for proper OverdrawCount
                    if( m_OutputType == OUTPUT_TYPE_UAV && CurPixelColorPassed )
                    {
                        if( OverdrawCount != ExpectedOverdrawCount )
                        {
                            bViewportFailed = true;
                            tRes = RESULT_FAIL;

                            // We executed the color comparison logic already, so subtract 1 from passing pixel count
                            --PassedPix;

                            // Increment the unexpected pixel count
                            ++FailedPix;
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

				if ( 0 < FailedPix )
				{
					WriteToLog( "Unrecognized pixel colors/overdraw inside scissor for viewport %d: %d", i, FailedPix );
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
