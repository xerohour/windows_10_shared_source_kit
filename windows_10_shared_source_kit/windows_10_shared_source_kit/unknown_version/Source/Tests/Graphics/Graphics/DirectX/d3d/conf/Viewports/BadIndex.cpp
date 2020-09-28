#include "BadIndex.h"

void BadIndex::InitTestParameters()
{
	m_bUseIndex = true;

	testfactor::RFactor rFact;
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports );
	CTestCaseParameter< UINT > *pbadViewportParam = AddParameter< UINT >( _T("IndexedViewport"), &m_ViewportArrayIndex );
	CTestCaseParameter< UINT > *pScissorEnabledParam = AddParameter< UINT >( _T("ScissorEnabled"), &m_scissorEnabled );

	//16 is a special case- can't go over # viewports without going over max # viewports
	rFact = AddParameterValue< UINT >( pbadViewportParam, 16 )* AddParameterValue< UINT >( pNumViewportsParam, 16 ) * AddParameterValueSet< UINT >(pScissorEnabledParam, new CRangeValueSet< UINT >( 0, 1, 1));

	//for each # of total viewports, we want to try indexing (numViewports+1) and 16
	for ( UINT i = 15; i >= 1; --i )
	{
		rFact = rFact + (AddParameterValueSet< UINT >( pbadViewportParam, new CRangeValueSet< UINT >( i, 16, (16-i)) ) *
			AddParameterValueSet< UINT >( pScissorEnabledParam, new CRangeValueSet< UINT >( 0, 1, 1) ) *
			AddParameterValue< UINT >( pNumViewportsParam, i ));
	}

	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT BadIndex::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	float RelativeEpsilon = 0.0005f;
	float RelativeDiff[3];

	D3D11_MAPPED_SUBRESOURCE texMap;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

	//change the index of the first viewport to the wrong value we want to test
	UINT *pDestViewportIndices;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if ( FAILED( GetEffectiveContext()->Map( m_pVIBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog("Map on vertex buffer failed.");
		return RESULT_FAIL;
	}
	pDestViewportIndices = (UINT*)mappedRes.pData;

	//set the index to the invalid value
	pDestViewportIndices[0] = m_ViewportArrayIndex;
	pDestViewportIndices[1] = m_ViewportArrayIndex;
	pDestViewportIndices[2] = m_ViewportArrayIndex;
	pDestViewportIndices[3] = m_ViewportArrayIndex;

	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	GetEffectiveContext()->Draw( 4, 0 );  //always try to draw only the first viewport

	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes the deferred context
	{
		UINT16 *pData = (UINT16*)texMap.pData; //On the assumption that we're looking at 16-bit floats.
        UINT32 *pOverdrawData = NULL;
        
        if( m_OutputType == OUTPUT_TYPE_UAV )
        {
            pOverdrawData = (UINT32*)overdrawTexMap.pData;
        }

		RECT ViewRect = ViewportToRect( m_Viewports[0], g_App.m_D3DOptions.FeatureLevel );
		UINT FailedPixIn = 0, FailedPixOut = 0, PassedPix = 0;

		//test that the correct value of SV_ViewportArrayIndex was passed to the pixel shader
		color4 ColorsAlpha = color4(
					CDXGIFloat16(pData[4]),
					CDXGIFloat16(pData[1]),
					CDXGIFloat16(pData[2]),
					CDXGIFloat16(pData[3])
		);

		//check the value of ViewportArrayIndex passed to the pixel shader
		if (m_ViewportArrayIndex > 16 && ColorsAlpha.a != m_ViewportArrayIndex)
		{
			WriteToLog( "Value of ViewportArrayIndex not passed to pixel shader unconverted.");
			tRes = RESULT_FAIL;
		}

		//check that all pixels are the correct colour
		for ( UINT y = 0; y < m_RTHeight; ++y )
		{				
			for ( UINT x = 0; x < m_RTWidth; ++x )
			{
                bool CurPixelColorPassed = false;

				bool bInViewport;               
				
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

				//everything is black
				if (m_ViewportArrayIndex == (m_NumViewports) && m_ViewportArrayIndex != 16)
				{
					bInViewport = false;
					TargetColor = BLACK;
                    ExpectedOverdrawCount = 0;
				}
				else
				{
					//within viewport
					if (ViewRect.top <= (LONG)y && (LONG)y < ViewRect.bottom &&
					    ViewRect.left <= (LONG)x && (LONG)x < ViewRect.right)
					{
						bInViewport = true;

						if (!m_scissorEnabled //scissor disabled
							|| (x >= (ViewRect.left + m_scissorSize) //in scissor
								&& x < (ViewRect.left + 2*m_scissorSize)
								&& y >= (ViewRect.top + m_scissorSize)
								&& y < (ViewRect.top + 2*m_scissorSize)))
						{						
							TargetColor = MultiColors[0];
                            ExpectedOverdrawCount = 1;
						}
						else
                        {
							TargetColor = BLACK;
                            ExpectedOverdrawCount = 0;
                        }
					}
					else
					{
						bInViewport = false;
						TargetColor = BLACK;
                        ExpectedOverdrawCount = 0;
					}
				}
	
				CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

				if ( RelativeDiff[0] > RelativeEpsilon ||
					RelativeDiff[1] > RelativeEpsilon ||
					RelativeDiff[2] > RelativeEpsilon )
				{
                    CurPixelColorPassed = false;
					tRes = RESULT_FAIL;
					if ( bInViewport )
						++FailedPixIn;
					else
						++FailedPixOut;
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
                        if ( bInViewport )
						    ++FailedPixIn;
					    else
						    ++FailedPixOut;
                    }
                }
			}

			pData += texMap.RowPitch / sizeof(UINT16);
            pOverdrawData += overdrawTexMap.RowPitch / sizeof(UINT32);
		}

		WriteToLog( "\nViewport %d", 0 );
		WriteToLog( "%s", ToString( m_Viewports[0] ).c_str() );

		if ( 0 < PassedPix )
			WriteToLog( "Passing pixels: %d", PassedPix );

		if ( 0 < FailedPixIn )
			WriteToLog( "Unexpected colors/overdraw found while testing viewport %d: %d.", 0, FailedPixIn );

		if ( 0 < FailedPixOut )
			WriteToLog( "Unexpected colors/overdraw found while looking outside viewport %d: %d.", 0, FailedPixOut );

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
