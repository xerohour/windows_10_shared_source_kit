#include "Overlapping.h"

void OverlappingView::InitTestParameters()
{
	m_bUseIndex = true;

	testfactor::RFactor rFact;
	CUserValueSet< tstring > *pPatternsValueSet = new CUserValueSet< tstring >();
	CTestCaseParameter< tstring > *pPatternParam;
	
	pPatternsValueSet->AddValue( _T("ab") );
	pPatternsValueSet->AddValue( _T("aB") );
	pPatternsValueSet->AddValue( _T("ac") );
	pPatternsValueSet->AddValue( _T("ad") );
	pPatternsValueSet->AddValue( _T("ae") );
	//pPatternsValueSet->AddValue( _T("af") );
	//pPatternsValueSet->AddValue( _T("ag") );
	pPatternsValueSet->AddValue( _T("ah") );
	pPatternsValueSet->AddValue( _T("ai") );
	pPatternsValueSet->AddValue( _T("aj") );
	pPatternsValueSet->AddValue( _T("ak") );

	pPatternsValueSet->AddValue( _T("bc") );
	pPatternsValueSet->AddValue( _T("bd") );
	pPatternsValueSet->AddValue( _T("be") );
	pPatternsValueSet->AddValue( _T("bf") );
	//pPatternsValueSet->AddValue( _T("bg") );
	//pPatternsValueSet->AddValue( _T("bh") );
	pPatternsValueSet->AddValue( _T("bi") );
	pPatternsValueSet->AddValue( _T("bj") );
	//pPatternsValueSet->AddValue( _T("bk") );

	pPatternsValueSet->AddValue( _T("cd") );
	//pPatternsValueSet->AddValue( _T("ce") );
	pPatternsValueSet->AddValue( _T("cf") );
	pPatternsValueSet->AddValue( _T("cg") );
	//pPatternsValueSet->AddValue( _T("ch") );
	pPatternsValueSet->AddValue( _T("ci") );
	pPatternsValueSet->AddValue( _T("cj") );
	pPatternsValueSet->AddValue( _T("ck") );

	//pPatternsValueSet->AddValue( _T("de") );
	//pPatternsValueSet->AddValue( _T("df") );
	pPatternsValueSet->AddValue( _T("dg") );
	pPatternsValueSet->AddValue( _T("dh") );
	pPatternsValueSet->AddValue( _T("di") );
	pPatternsValueSet->AddValue( _T("dj") );
	pPatternsValueSet->AddValue( _T("dk") );

	pPatternsValueSet->AddValue( _T("ef") );
	//pPatternsValueSet->AddValue( _T("eg") );
	pPatternsValueSet->AddValue( _T("eh") );
	pPatternsValueSet->AddValue( _T("ei") );
	//pPatternsValueSet->AddValue( _T("ej") );
	pPatternsValueSet->AddValue( _T("ek") );

	pPatternsValueSet->AddValue( _T("fg") );
	//pPatternsValueSet->AddValue( _T("fh") );
	pPatternsValueSet->AddValue( _T("fi") );
	pPatternsValueSet->AddValue( _T("fj") );
	//pPatternsValueSet->AddValue( _T("fk") );

	pPatternsValueSet->AddValue( _T("gh") );
	pPatternsValueSet->AddValue( _T("gi") );
	//pPatternsValueSet->AddValue( _T("gj") );
	pPatternsValueSet->AddValue( _T("gk") );

	pPatternsValueSet->AddValue( _T("hi") );
	pPatternsValueSet->AddValue( _T("hj") );
	//pPatternsValueSet->AddValue( _T("hk") );

	pPatternsValueSet->AddValue( _T("ij") );
	pPatternsValueSet->AddValue( _T("ik") );

	pPatternsValueSet->AddValue( _T("jk") );
	pPatternsValueSet->AddValue( _T("jK") );


	pPatternsValueSet->AddValue( _T("Ijk") );
	pPatternsValueSet->AddValue( _T("iJk") );
	pPatternsValueSet->AddValue( _T("ijK") );

	pPatternsValueSet->AddValue( _T("abcdefghijk") );
	
	
	rFact = AddParameter< tstring >( _T("ViewportPattern"), &m_pViewportPattern, pPatternsValueSet );

	SetRootTestFactor( GetOutputTypeBaseRFactor() * rFact );
}

TEST_RESULT OverlappingView::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
	GetEffectiveContext()->RSSetState( m_pRSView );

	m_NumViewports = 0;
	memset( m_Viewports, 0, sizeof(m_Viewports) );

	for ( UINT i = 0; i < _tcslen( m_pViewportPattern.c_str() ); ++i )
	{
		++m_NumViewports;
		int ch = (int)m_pViewportPattern.c_str()[i];

		if ( isupper(ch) )
		{
			m_Viewports[i].MinDepth = 0.0f;
			m_Viewports[i].MaxDepth = 0.6f;
			ch = (char)tolower(ch);
		}
		else
		{
			m_Viewports[i].MinDepth = 0.4f;
			m_Viewports[i].MaxDepth = 1.0f;			
		}

		switch ((LOCATION)ch)
		{
		case TOP_LEFT_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.6f);
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.6f);
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = 0;
			break;

		case TOP_RIGHT_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.6f);
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.6f);
			m_Viewports[i].TopLeftX = m_RTWidth - m_Viewports[i].Width;
			m_Viewports[i].TopLeftY = 0;
			break;

		case BOTTOM_RIGHT_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.6f);
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.6f);
			m_Viewports[i].TopLeftX = m_RTWidth - m_Viewports[i].Width;
			m_Viewports[i].TopLeftY = m_RTHeight - m_Viewports[i].Height;
			break;

		case BOTTOM_LEFT_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.6f);
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.6f);
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = m_RTHeight - m_Viewports[i].Height;
			break;

		case TOP_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.3f);
			m_Viewports[i].Width = (float) m_RTWidth;
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = 0;
			break;

		case RIGHT_BACK:
			m_Viewports[i].Height = (float) m_RTHeight;
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.3f);
			m_Viewports[i].TopLeftX = m_RTWidth - m_Viewports[i].Width;
			m_Viewports[i].TopLeftY = 0;
			break;

		case BOTTOM_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.3f);
			m_Viewports[i].Width = (float) m_RTWidth;
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = m_RTHeight - m_Viewports[i].Height;
			break;

		case LEFT_BACK:
			m_Viewports[i].Height = (float) m_RTHeight;
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.3f);
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = 0;
			break;

		case MIDDLE_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.6f);
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.6f);
			m_Viewports[i].TopLeftX = (float) ((INT)(m_RTWidth/2) - (INT)(m_Viewports[i].Width * 0.5f));
			m_Viewports[i].TopLeftY = (float) ((INT)(m_RTHeight/2) - (INT)(m_Viewports[i].Height * 0.5f));
			break;

		case MIDDLE_WIDE_BACK:
			m_Viewports[i].Height = (float) (INT)(m_RTHeight * 0.3f);
			m_Viewports[i].Width = (float) m_RTWidth;
			m_Viewports[i].TopLeftX = 0;
			m_Viewports[i].TopLeftY = (float) ((INT)(m_RTHeight/2) - (INT)(m_Viewports[i].Height * 0.5f));
			break;

		case MIDDLE_TALL_BACK:
			m_Viewports[i].Height = (float) m_RTHeight;
			m_Viewports[i].Width = (float) (INT)(m_RTWidth * 0.3f);
			m_Viewports[i].TopLeftX = (float) ((INT)(m_RTWidth/2) - (INT)(m_Viewports[i].Width * 0.5f));
			m_Viewports[i].TopLeftY = 0;
			break;

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
		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];

		pDestViewportIndices[1+offset] = i;
		pDestVerts[1+offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[1+offset].Color = MultiColors[i];

		pDestViewportIndices[2+offset] = i;
		pDestVerts[2+offset].Pos = pos(0.0f, 1.0f);
		pDestVerts[2+offset].Color = MultiColors[i];

		pDestViewportIndices[3+offset] = i;
		pDestVerts[3+offset].Pos = pos(1.0f, 1.0f);
		pDestVerts[3+offset].Color = MultiColors[i];
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT OverlappingView::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	float RelativeEpsilon = 0.0005f;
	float RelativeDiff[3];
	UINT PixelCount[17][18]; //One extra for expected black, and one for unexpected colors
	memset( PixelCount, 0, sizeof(UINT) * 17 * 18 );

	D3D11_MAPPED_SUBRESOURCE texMap;
	D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

    for( UINT vp = 0; vp < m_NumViewports; vp++ )
    {
    	GetEffectiveContext()->Draw( 4, vp*4 );
    }

	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes the effective context.
	{
		UINT16 *pData = (UINT16*)texMap.pData; //On the assumption that we're looking at 16-bit floats.
		UINT32 *pOverdrawData = NULL;
        
        if( m_OutputType == OUTPUT_TYPE_UAV )
        {
            pOverdrawData = (UINT32*)overdrawTexMap.pData;
        }

		RECT ViewRect[16];

		for ( UINT i = 0; i < m_NumViewports; ++i )
		{
			ViewRect[i] = ViewportToRect( m_Viewports[i], g_App.m_D3DOptions.FeatureLevel );
		}

		for ( UINT y = 0; y < m_RTHeight; ++y )
		{				
			for ( UINT x = 0; x < m_RTWidth; ++x )
			{
                bool CurPixelColorPassed = false;

				bool bExpectBlack;
				UINT ExpectedVisibleViewport;
				float fDepth;
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

				//We're within the viewport
				TargetColor = color( 0.0f, 0.0f, 0.0f );
				bExpectBlack = true;
				fDepth = 1.0f;
				ExpectedVisibleViewport = 16;
				for ( UINT i = 0; i < m_NumViewports; ++i )
				{
					if ( ViewRect[i].top <= (LONG)y && (LONG)y < ViewRect[i].bottom &&
						ViewRect[i].left <= (LONG)x && (LONG)x < ViewRect[i].right )
					{
                        ExpectedOverdrawCount++;
						bExpectBlack = false;
						if ( m_Viewports[i].MinDepth < fDepth )
						{
							ExpectedVisibleViewport = i;
							TargetColor = MultiColors[i];
							fDepth = m_Viewports[i].MinDepth;
						}
					}	
				}

                // If outputting to UAVs and we are in a region where multiple viewports overlap,
                // skip color comparison (since the actual color in this region is nondeterministic)
                // and just make sure the right number of overdraws happened in that region

                if( m_OutputType == OUTPUT_TYPE_RTV || ExpectedOverdrawCount <= 1 )
                {
				    CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

				    if ( RelativeDiff[0] > RelativeEpsilon ||
					    RelativeDiff[1] > RelativeEpsilon ||
					    RelativeDiff[2] > RelativeEpsilon )
				    {
                        CurPixelColorPassed = false;
					    bool bFoundMatch = false;
					    tRes = RESULT_FAIL;

					    for ( UINT j = 0; j < m_NumViewports; ++j )
					    {
						    CalcRelativeDiff( MultiColors[j], Colors, RelativeDiff );

						    //Check to see if it matches any of the viewport colors set
						    if ( RelativeDiff[0] < RelativeEpsilon &&
							    RelativeDiff[1] < RelativeEpsilon &&
							    RelativeDiff[2] < RelativeEpsilon )
						    {
							    ++PixelCount[ExpectedVisibleViewport][j];

							    bFoundMatch = true;
							    break;
						    }
					    }

					    if ( !bFoundMatch )
					    {
						    //Check against black
						    CalcRelativeDiff( color( 0.0f, 0.0f, 0.0f ), Colors, RelativeDiff );

						    //Check to see if it matches any of the viewport colors set
						    if ( RelativeDiff[0] < RelativeEpsilon &&
							    RelativeDiff[1] < RelativeEpsilon &&
							    RelativeDiff[2] < RelativeEpsilon )
						    {
							    ++PixelCount[ExpectedVisibleViewport][16];

							    bFoundMatch = true;
						    }
					    }

					    //If still nothing, increment unexpected count
					    if ( !bFoundMatch )
					    {
						    ++PixelCount[ExpectedVisibleViewport][17];
					    }
				    }
				    else
				    {
                        CurPixelColorPassed = true;
					    ++PixelCount[ExpectedVisibleViewport][ExpectedVisibleViewport];
				    }
                }
                else
                {
                    CurPixelColorPassed = true;
                }

                // If outputting to UAVs, check for proper OverdrawCount
                if( m_OutputType == OUTPUT_TYPE_UAV && CurPixelColorPassed )
                {
                    if( OverdrawCount != ExpectedOverdrawCount )
                    {
                        tRes = RESULT_FAIL;

                        if( ExpectedOverdrawCount <= 1 )
                        {
                            // We executed the color comparison logic, so subtract 1 from passing pixel count
                            --PixelCount[ExpectedVisibleViewport][ExpectedVisibleViewport];
                        }

                        // Increment the unexpected pixel count
                        ++PixelCount[ExpectedVisibleViewport][17];
                    }
                }
			}

			pData += texMap.RowPitch / sizeof(UINT16);
            pOverdrawData += overdrawTexMap.RowPitch / sizeof(UINT32);
		}

		UnMapRT(true);
	}
	else
	{
		WriteToLog( "Failed to map render target." );
		tRes = RESULT_FAIL;
	}

	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		WriteToLog( "\n" );
		WriteToLog( "Viewport %d", i );
		WriteToLog( "%s", ToString( m_Viewports[i] ).c_str() );
		WriteToLog( "Passing pixels for viewport %d: %d", i, PixelCount[i][i] );
		for ( UINT j = 0; j < m_NumViewports; ++j )
		{
			if ( j != i )
			{
				if ( 0 < PixelCount[i][j] )
					WriteToLog( "Failing pixels from viewport %d found in viewport %d: %d", j, i, PixelCount[i][j] );
			}
		}
		if ( 0 < PixelCount[i][16] )
		{
			WriteToLog( "Pixels unexpectedly black for viewport %d: %d", i, PixelCount[i][16] );
		}
		if ( 0 < PixelCount[i][17] )
		{
			WriteToLog( "Unrecognized pixel colors/incorrect overdraw for viewport %d: %d", i, PixelCount[i][17] );
		}
	}
	WriteToLog( "\n" );
	WriteToLog( "No viewport coverage" );
	WriteToLog( "Passing pixels for clear: %d", PixelCount[16][16] );
	for ( UINT j = 0; j < m_NumViewports; ++j )
	{
		if ( 0 < PixelCount[16][j] )
			WriteToLog( "Failing pixels from viewport %d found in clear: %d", j, PixelCount[16][j] );
	}
	if ( 0 < PixelCount[16][17] )
	{
		WriteToLog( "Unrecognized pixel colors for clear: %d", PixelCount[16][17] );
	}

	g_App.Present();

	return tRes;
}
