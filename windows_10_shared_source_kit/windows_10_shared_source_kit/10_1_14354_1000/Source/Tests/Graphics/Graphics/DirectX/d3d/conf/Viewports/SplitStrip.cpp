#include "SplitStrip.h"

void SplitStrip::InitTestParameters()
{
	m_bUseIndex = true;

	CRangeValueSet< UINT > *pVSNumViewports = new CRangeValueSet< UINT >( 2, 16, 2 );
	CTestCaseParameter< UINT > *pNumViewportsParam = AddParameter( _T("NumViewports"), &m_NumViewports );

	SetRootTestFactor( GetOutputTypeBaseRFactor() * AddParameterValueSet( pNumViewportsParam, pVSNumViewports ) );
}

TEST_RESULT	SplitStrip::Setup()
{
	ID3D10Blob *pGSBuf = NULL;
	TEST_RESULT tRes;

	if ( RESULT_PASS == (tRes = ViewportsTest::Setup()) )
	{
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		SAFE_RELEASE( m_pGS );
		if ( FAILED( D3DX10CompileFromResource(NULL, "gs_wIndexChange.gsh", NULL, NULL, NULL, "main", "gs_4_0", 0, 0, NULL, &pGSBuf, NULL, NULL ) ) )
		{
			WriteToLog("CompileShaderFromResource for geometry shader failed.");
			goto FAILED;
		}

		if ( FAILED( GetDevice()->CreateGeometryShader( (DWORD*) pGSBuf->GetBufferPointer(), pGSBuf->GetBufferSize(), NULL, &m_pGS ) ) )
		{
			WriteToLog("CreateGeometryShader Failed.");
			goto FAILED;
		}

		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );

		SAFE_RELEASE( pGSBuf );

		HRESULT hr;
		if( FAILED(hr = ExecuteEffectiveContext()) )
		{
			WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto FAILED;
		}
	}

	return tRes;

FAILED:
	Cleanup();

	return RESULT_FAIL;
}
TEST_RESULT SplitStrip::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
	GetEffectiveContext()->RSSetState( m_pRSView );

	//
	// Setup the viewports
	//

	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		m_Viewports[i].MaxDepth = 1.0f;
		m_Viewports[i].MinDepth = 0.0f;

		m_Viewports[i].Height = 2.0f;
		m_Viewports[i].Width = 1.0f;
		
		m_Viewports[i].TopLeftX = (float) (1 + i * 2); //Put gaps around them
		m_Viewports[i].TopLeftY = 1.0f;
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

	//
	// Fill vertex buffers
	//

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

	UINT offset = 0;
	UINT tri = 0;
	for ( UINT i = 0; i < m_NumViewports; i+=2 )
	{ 
		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Pos = pos(0.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];
		++offset;

		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Pos = pos(1.0f, 0.0f);
		pDestVerts[offset].Color = MultiColors[i];
		++offset;

		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Pos = pos(0.0f, 0.5f);
		pDestVerts[offset].Color = MultiColors[i];
		++offset;

		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Pos = pos(1.0f, 0.5f);
		pDestVerts[offset].Color = MultiColors[i];
		++offset;
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT SplitStrip::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	FLOAT RelativeEpsilon = 0.0005f;
	FLOAT RelativeDiff[3];

	D3D11_MAPPED_SUBRESOURCE texMap;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMap;

    for( UINT vp = 0; vp < m_NumViewports/2; vp++ )
    {
        GetEffectiveContext()->Draw( 4, vp*4 );
    }

	if ( MapRT( &texMap, &overdrawTexMap ) ) // Executes the effective context
	{
		UINT16 *pData = NULL; //On the assumption that we're looking at 16-bit floats.
        UINT32 *pOverdrawData = NULL;
		UINT PixelCount[18]; //Last ones are for BLACK and for unrecognized colors
		color TargetColor;
	

		//
		// Dump render target to log
		//
		WriteToLog("===================================");
		WriteToLog("RT Dump: (non-zero pixels only)");
		for (UINT y = 0; y < 4; ++y)
		{
			pData = (UINT16*)((BYTE*)texMap.pData + texMap.RowPitch*y);
			for (UINT x = 0; x < 33; ++x)
			{
				color Color = color(
					CDXGIFloat16(pData[x*4]),
					CDXGIFloat16(pData[x*4+1]),
					CDXGIFloat16(pData[x*4+2])
					);

				if ((pData[x*4] != 0) || (pData[x*4+1] != 0) || (pData[x*4+2] != 0))
				{
					WriteToLog("[%d,%d] %f, %f, %f", y, x, Color.r, Color.g, Color.b);
				}
			}
		}
		WriteToLog("===================================");

		for ( UINT i = 0; i <= m_NumViewports; ++i )
		{
			bool bInViewport;
			if ( i == m_NumViewports )
				bInViewport = false;
			else
				bInViewport = true;

			RECT rect = { 0, 0, m_RTWidth, m_RTHeight };
			if (bInViewport)
			{
				rect = ViewportToRect( m_Viewports[i], g_App.m_D3DOptions.FeatureLevel );
			}

			bool bOddViewport = i%2==0? false: true;
			UINT ExpectedColorIndex = bOddViewport? i-1: i;
			
			bool bViewportFailed = false;

			for ( UINT k = 0; k < 18; ++k ) PixelCount[k] = 0;
	
			WriteToLog("texMap.RowPitch = %d", texMap.RowPitch);
			pData = (UINT16*)texMap.pData + rect.top * texMap.RowPitch / sizeof(UINT16);			
			pData += rect.left * 4;

            if( m_OutputType == OUTPUT_TYPE_UAV )
            {
                pOverdrawData = (UINT32*)overdrawTexMap.pData + rect.top * overdrawTexMap.RowPitch / sizeof(UINT32);
                pOverdrawData += rect.left;
            }

			for ( LONG y = 0; y < rect.bottom - rect.top; ++y )
			{				
				for ( LONG x = 0; x < rect.right - rect.left; ++x )
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
                    UINT32 ExpectedOverdrawCount = 0;

					if ( !bInViewport )
					{	//If we're looking at the non-viewport covered area skip areas in viewports
						bool bSkip = false;

						for ( UINT j = 0; j <= m_NumViewports; ++j )
						{
							RECT ViewRect = ViewportToRect( m_Viewports[j], g_App.m_D3DOptions.FeatureLevel );
							//We're within the viewport
							if ( ViewRect.top <= y && y < ViewRect.bottom &&
								ViewRect.left <= x && x < ViewRect.right )
							{
								bSkip = true;
				 				break;
							}
						}

						if ( bSkip ) continue; //We're supposed to be looking at the non-covered area

						TargetColor = BLACK;
                        ExpectedOverdrawCount = 0;
					}
					else
					{	//Use location to determine whether color or black is expected
						if	( (y == 0 && !bOddViewport) ||	//If we're on an even viewport, and at 0,0
							  (y == 1 && bOddViewport)		//If we're on an odd viewport, and at 0,1
							)
                        {
							TargetColor = MultiColors[ExpectedColorIndex];
                            ExpectedOverdrawCount = 1;
                        }
						else
                        {
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
						WriteToLog("Error on Viewport[%d] Pixel[x %d, y %d]", i, x, y);
						WriteToLog("Actual: %f, %f, %f", Colors.r, Colors.g, Colors.b);
						WriteToLog("Expect: %f, %f, %f", TargetColor.r, TargetColor.g, TargetColor.b);
						bool bFoundMatch = false;
						bViewportFailed = true;
						tRes = RESULT_FAIL;

						for ( UINT j = 0; j <= m_NumViewports; ++j, ++j )
						{
							if ( j == i ) continue;
							UINT ExpectedColorIndex = j%2==0? j: j-1;

							if ( j == m_NumViewports )
								TargetColor = BLACK;
							else
								TargetColor = MultiColors[ExpectedColorIndex];

							CalcRelativeDiff( TargetColor, Colors, RelativeDiff );

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
							++PixelCount[17];
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
                            ++PixelCount[17];
                        }
                    }
				}

				pData += texMap.RowPitch / sizeof(UINT16);
                pOverdrawData += overdrawTexMap.RowPitch / sizeof(UINT32);
			}

			if ( bViewportFailed )
			{
				if ( bInViewport )
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

					if ( 0 < PixelCount[m_NumViewports])
					{
						WriteToLog( "Failing pixels with clear color found in viewport %d: %d", i, PixelCount[m_NumViewports] );
					}

					if ( 0 < PixelCount[17] )
					{
						WriteToLog( "Unrecognized pixel colors/overdraw for viewport %d: %d", i, PixelCount[17] );
					}
				}
				else
				{
					WriteToLog( "\n" );
					WriteToLog( "RT area not covered by any viewport" );
					WriteToLog( "Passing pixels: %d", PixelCount[i] );
					for ( UINT j = 0; j < m_NumViewports; ++j )
					{
						if ( 0 < PixelCount[j] )
							WriteToLog( "Failing pixels from viewport %d found where clear color expected: %d", j, PixelCount[j] );
					}

					if ( 0 < PixelCount[17] )
					{
						WriteToLog( "Unrecognized pixel colors where clear color expected: %d", PixelCount[17] );
					}
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
