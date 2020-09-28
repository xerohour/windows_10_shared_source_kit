#include "ScissorPosition.h"

BEGIN_NAMED_VALUES( ScissorPositions )
    NAMED_VALUE( _T( "LARGER_THAN_TARGET" ), LARGER_THAN_TARGET )
    NAMED_VALUE( _T( "OFF_TARGET" ), OFF_TARGET )
END_NAMED_VALUES( ScissorPositions )

void ScissorPosition::InitTestParameters()
{	
	m_bUseIndex = true;

	testfactor::RFactor rFact;
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports );
	CTestCaseParameter< ScissorPositions > *pScissorPosition = AddParameter< ScissorPositions >( _T("ScissorPosition"), &m_position );

	//larger than render target
	rFact = AddParameterValue< UINT >( pNumViewportsParam, 1 )*AddParameterValue< ScissorPositions >( pScissorPosition, LARGER_THAN_TARGET);
	
	//off target
	rFact = rFact + (AddParameterValueSet< UINT >( pNumViewportsParam, new CRangeValueSet< UINT >( 1, 16, 1 ) )*
		AddParameterValue< ScissorPositions >( pScissorPosition, OFF_TARGET));
	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT ScissorPosition::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	GetEffectiveContext()->RSSetState( m_pRSScissor );

	UINT numHorizontal, numVertical, width, height, index;
	BOOL bExtra = FALSE;

	if (m_position == LARGER_THAN_TARGET)
		m_scissorSize = m_RTWidth + 2; //one pixel bigger on each edge
	else
		m_scissorSize = m_RTWidth / 3;
	
	//set up viewports
	for (UINT index = 0; index < m_NumViewports; ++index)
	{
		m_Viewports[index].Height = (float) m_RTHeight; 
		m_Viewports[index].Width = (float) m_RTWidth;
		m_Viewports[index].TopLeftX = 0.0f;
		m_Viewports[index].TopLeftY = 0.0f;

		m_Viewports[index].MinDepth = 0.0f;
		m_Viewports[index].MaxDepth = 1.0f;
	}

	//set up scissors
	if (m_position == LARGER_THAN_TARGET)
	{
		m_Scissors[0].left = -1;
		m_Scissors[0].top = -1;	
		m_Scissors[0].right = m_Scissors[0].left + m_scissorSize;
		m_Scissors[0].bottom = m_Scissors[0].top + m_scissorSize;	
	}
	else //in a ring around the render target
	{
		for (UINT i = 0; i < m_NumViewports; ++i)
		{
			int xpos = -((int) m_RTWidth / 3) - 1;
			int ypos = -((int) m_RTWidth / 3) - 1;
			switch ( i )
			{
			case 4:
				ypos += m_scissorSize;
			case 3:
				ypos += m_scissorSize;
			case 2:
				ypos += m_scissorSize;
			case 1:
				ypos += m_scissorSize + 1;
			case 0:
				break;

			case 5:
				ypos += (3*m_scissorSize) + (m_scissorSize + 1);
			case 6:	
				xpos = 0;
				break;

			case 7:
				ypos += (3*m_scissorSize) + (m_scissorSize + 1);
			case 8:
				xpos = (m_scissorSize);
				break;

			case 9:
				ypos += (3*m_scissorSize) + (m_scissorSize + 1);
			case 10:
				xpos = 2*(m_scissorSize);
				break;

			case 11:
				ypos += m_scissorSize;
			case 12:
				ypos += m_scissorSize;
			case 13:
				ypos += m_scissorSize;
			case 14:
				ypos += (m_scissorSize + 1);	
			case 15:
				xpos = m_RTWidth;
				break;
			default:
				xpos = m_RTWidth;
				ypos = m_RTHeight;
			};

			m_Scissors[i].left = xpos;
			m_Scissors[i].top = ypos;		
			m_Scissors[i].right = m_Scissors[i].left + m_scissorSize;
			m_Scissors[i].bottom = m_Scissors[i].top + m_scissorSize;	
		}
	}

	GetEffectiveContext()->RSSetViewports( m_NumViewports, m_Viewports );
	GetEffectiveContext()->RSSetScissorRects( m_NumViewports, m_Scissors );  //same number of viewports and scissors

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

		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Color = MultiColors[i];

		pDestViewportIndices[1+offset] = i;
		pDestVerts[1+offset].Color = MultiColors[i];

		pDestViewportIndices[2+offset] = i;
		pDestVerts[2+offset].Color = MultiColors[i];

		pDestViewportIndices[3+offset] = i;
		pDestVerts[3+offset].Color = MultiColors[i];

		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[1+offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[2+offset].Pos = pos(0.0f, 1.0f);
		pDestVerts[3+offset].Pos = pos(1.0f, 1.0f); 
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT ScissorPosition::ExecuteTestCase()
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
		
		UINT FailedPix = 0, PassedPix = 0;
		bool bViewportFailed = false;
		
		pData = (UINT16*)texMap.pData;

        if( m_OutputType == OUTPUT_TYPE_UAV )
        {
            pOverdrawData = (UINT32*)overdrawTexMap.pData;
        }

		for ( UINT y = 0; y < m_RTHeight; ++y )
		{				
			for ( UINT x = 0; x < m_RTWidth; ++x )
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
                UINT32 ExpectedOverdrawCount = 0;
					
				if (m_position == LARGER_THAN_TARGET)
                {
					TargetColor = MultiColors[0];
                    ExpectedOverdrawCount = 1;
                }
				else
                {
					TargetColor = BLACK;
                    ExpectedOverdrawCount = 0;
                }
					
				CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

				//Check to see if it matches any of the viewport colors set
				if (RelativeDiff[0] > RelativeEpsilon ||
					RelativeDiff[1] > RelativeEpsilon ||
					RelativeDiff[2] > RelativeEpsilon )
				{
                    CurPixelColorPassed = false;
					++FailedPix;
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
			WriteToLog( "Passing pixels: %d ", PassedPix );

			if ( 0 < FailedPix )
			{
				WriteToLog( "Unrecognized pixel colors inside scissor for viewport: %d", FailedPix );
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
