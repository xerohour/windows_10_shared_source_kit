#include "NoIndex.h"

void NoIndex::InitTestParameters()
{
	m_bUseIndex = false;  //set so that correct shaders are used

	testfactor::RFactor rFact;
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports );
	CTestCaseParameter< UINT > *pScissorEnabledParam = AddParameter< UINT >( _T("ScissorEnabled"), &m_scissorEnabled );

	rFact = AddParameterValue< UINT >( pNumViewportsParam, 16 ) * AddParameterValueSet< UINT >(pScissorEnabledParam, new CRangeValueSet< UINT >( 0, 1, 1));

	//for each # of total viewports, we want to try indexing (numViewports+1) and 17
	for ( UINT i = 15; i >= 1; --i )
	{
		rFact = rFact + (AddParameterValueSet< UINT >( pScissorEnabledParam, new CRangeValueSet< UINT >( 0, 1, 1) ) 
						* AddParameterValue< UINT >( pNumViewportsParam, i ));
	}

	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT NoIndex::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	float RelativeEpsilon = 0.0005f;
	float RelativeDiff[3];

	D3D11_MAPPED_SUBRESOURCE texMap;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

	GetEffectiveContext()->Draw(4, 0);  //always try to draw the first viewport

	//test the first viewport to see if it's coloured in
	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes the effective context
	{
		UINT16 *pData = (UINT16*)texMap.pData; //On the assumption that we're looking at 16-bit floats.
        UINT32 *pOverdrawData = NULL;
        
        if( m_OutputType == OUTPUT_TYPE_UAV )
        {
            pOverdrawData = (UINT32*)overdrawTexMap.pData;
        }

		RECT ViewRect = ViewportToRect( m_Viewports[0], g_App.m_D3DOptions.FeatureLevel );
		UINT FailedPixIn = 0, FailedPixOut = 0, PassedPix = 0;

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

				//in the viewport
				if ( (ViewRect.top <= (LONG)y && (LONG)y < ViewRect.bottom &&
					 ViewRect.left <= (LONG)x && (LONG)x < ViewRect.right))
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
