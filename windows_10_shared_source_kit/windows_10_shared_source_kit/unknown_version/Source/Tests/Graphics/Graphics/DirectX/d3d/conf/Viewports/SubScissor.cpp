#include "SubScissor.h"
BEGIN_NAMED_VALUES( TestColorOrOverdraw )
	NAMED_VALUE( _T("TEST_COLOR"), TEST_COLOR )
	NAMED_VALUE( _T("TEST_OVERDRAW"), TEST_OVERDRAW )
END_NAMED_VALUES( TestColorOrOverdraw )


BEGIN_NAMED_VALUES( ScissorOffset )
    NAMED_VALUE( _T( "MIDDLE" ), MIDDLE )
    NAMED_VALUE( _T( "FIVE" ), FIVE )
	NAMED_VALUE( _T( "THIRTEEN" ), THIRTEEN )
	NAMED_VALUE( _T( "SEVENTEEN" ), SEVENTEEN )
END_NAMED_VALUES( ScissorOffset )

using namespace testfactor;

void SubScissor::InitTestParameters()
{
	m_bUseIndex = true;

	RFactor rFact;

	RFactor ViewPortFactors = AddParameter< UINT >( _T("NumViewports"), &m_NumViewports, new CRangeValueSet< UINT >( 2, 16, 2 ) );

	CTestCaseParameter< ScissorOffset > *pScissorPosition = AddParameter< ScissorOffset >( _T("ScissorOffset"), &m_scissorPosition );	
	CUserValueSet<ScissorOffset> *pPositionValues = new CUserValueSet<ScissorOffset>( );
	pPositionValues->AddValue( MIDDLE );
	pPositionValues->AddValue( FIVE );
	pPositionValues->AddValue( THIRTEEN );
	pPositionValues->AddValue( SEVENTEEN );
	RFactor ScissorFactors = AddParameterValueSet< ScissorOffset >( pScissorPosition, pPositionValues );

	//
	// We need to create separate tables for both our output types and Tests.  For RTVs, we'll test pixel color only.  
	// For UAVs, we need to test both pixel color and overdraw.  But we need to test them separately.
	//
    CUserValueSet<OUTPUT_TYPE>* pOutputTypeRTV = new CUserValueSet<OUTPUT_TYPE>();
    pOutputTypeRTV->AddValue( OUTPUT_TYPE_RTV );
	RFactor OutputTypeRTVFactor = AddParameter<OUTPUT_TYPE>(_T("OutputType"), &m_OutputType , pOutputTypeRTV );

    CUserValueSet<OUTPUT_TYPE>* pOutputTypeUAV = new CUserValueSet<OUTPUT_TYPE>();
    pOutputTypeUAV->AddValue( OUTPUT_TYPE_UAV );
	RFactor OutputTypeUAVFactor = AddParameterValueSet<OUTPUT_TYPE>(_T("OutputType"), pOutputTypeUAV );

	CUserValueSet<TestColorOrOverdraw> *pTestColor = new CUserValueSet<TestColorOrOverdraw>( );
	pTestColor->AddValue( TEST_COLOR );
	RFactor ColorFactor = AddParameter< TestColorOrOverdraw >( _T("ColorOrOverdraw") , &m_ColorOrOverDraw, pTestColor);

	CUserValueSet<TestColorOrOverdraw> *pTestOverdraw = new CUserValueSet<TestColorOrOverdraw>( );
	pTestOverdraw->AddValue( TEST_OVERDRAW );
	RFactor OverdrawFactor = AddParameterValueSet< TestColorOrOverdraw >( _T("ColorOrOverdraw") , pTestOverdraw);

	SetRootTestFactor( ViewPortFactors	* ScissorFactors * (OutputTypeRTVFactor * ColorFactor + OutputTypeUAVFactor * (ColorFactor + OverdrawFactor)) );
}


TEST_RESULT SubScissor::SetupTestCase()
{
    TEST_RESULT result = ViewportsTest::SetupTestCase();
    if( result != RESULT_PASS )
    {
        return result;
    }

	//set the rasterizer state
	GetEffectiveContext()->RSSetState( m_pRSScissor );

	//setup the scissors- every second viewport has a small wierd scissor
	UINT numHorizontal, numVertical, width, height, index;
	BOOL bExtra = FALSE;	
	UINT scissorVOffset, scissorHOffset, scissorSize;

	switch ( m_NumViewports )
	{
		case 6:
		case 2:
		numVertical = 1;
		
		break;

	case 10:
	case 14:
		bExtra = TRUE;
	case 4:
	case 8:
	case 12:
		numVertical = 2;
		break;

	case 16:
		numVertical = 4;
		break;

	default:
		numVertical = 1;
	};
	numHorizontal = ((m_NumViewports / 2) - bExtra) / numVertical;

	height = m_RTHeight / ( numVertical + bExtra ); //Leave space at the bottom for an extra
	width = m_RTWidth / numHorizontal; 

	if (m_scissorPosition == MIDDLE)
	{
		scissorHOffset = width / 3;
		scissorVOffset = height / 3;
	}
	else
		scissorVOffset = scissorHOffset = m_scissorPosition;

	scissorSize = height / 3;

	for ( UINT x = 0; x < numHorizontal; ++x )
	{
		for ( UINT y = 0; y < numVertical; ++y )
		{
			index = (2* x * numVertical) + 2*y; 

			m_Viewports[index].Height = (float) height;
			m_Viewports[index].Width = (float) width;
			m_Viewports[index].TopLeftX = (float) (x * width);
			m_Viewports[index].TopLeftY = (float) (y * height);
			m_Viewports[index].MinDepth = 0.0f;
			m_Viewports[index].MaxDepth = 1.0f;
			
			m_Viewports[index + 1].Height = (float) height;
			m_Viewports[index + 1].Width = (float) width;
			m_Viewports[index + 1].TopLeftX = (float) (x * width);
			m_Viewports[index + 1].TopLeftY = (float) (y * height);
			m_Viewports[index + 1].MinDepth = 0.0f;
			m_Viewports[index + 1].MaxDepth = 1.0f;

			//small
			m_Scissors[index].top = y * height + scissorVOffset;	
			m_Scissors[index].bottom = m_Scissors[index].top + scissorSize;
			m_Scissors[index].left = x * width + scissorHOffset;
			m_Scissors[index].right = m_Scissors[index].left + scissorSize;

			//whole viewport
			m_Scissors[index + 1].top = (LONG) (m_Viewports[index + 1].TopLeftY);
			m_Scissors[index + 1].bottom = (LONG) (m_Scissors[index + 1].top + height);
			m_Scissors[index + 1].left = (LONG) (m_Viewports[index + 1].TopLeftX);
			m_Scissors[index + 1].right = (LONG) (m_Scissors[index + 1].left + width);

		}
	}

	if ( bExtra )
	{
		index = index + 2;
		m_Viewports[index].Height = (float) height;
		m_Viewports[index].Width = (float) m_RTWidth;
		m_Viewports[index].TopLeftX = 0.0f;
		m_Viewports[index].TopLeftY = (float) (numVertical * height);
		m_Viewports[index].MinDepth = 0.0f;
		m_Viewports[index].MaxDepth = 1.0f;
		
		//small
		m_Scissors[index].left = (LONG) (m_Viewports[index].TopLeftX + scissorHOffset);
		m_Scissors[index].right = (LONG) (m_Scissors[index].left + scissorSize);
		m_Scissors[index].top = (LONG) (m_Viewports[index].TopLeftY + scissorVOffset);
		m_Scissors[index].bottom = (LONG) (m_Scissors[index].top + scissorSize);

		m_Viewports[++index].Height = (float) height;
		m_Viewports[index].Width = (float) m_RTWidth;
		m_Viewports[index].TopLeftX = 0.0f;
		m_Viewports[index].TopLeftY = (float) (numVertical * height);
		m_Viewports[index].MinDepth = 0.0f;
		m_Viewports[index].MaxDepth = 1.0f;

		m_Scissors[index].left = (LONG) (m_Viewports[index].TopLeftX);
		m_Scissors[index].right = (LONG) (m_Viewports[index].TopLeftX + m_RTWidth);
		m_Scissors[index].top = (LONG) (m_Viewports[index].TopLeftY);
		m_Scissors[index].bottom = (LONG) (m_Viewports[index].TopLeftY + height);

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

	//Create Triangle strip with two overlapping triangles
	for ( UINT i = 0; i < m_NumViewports; ++i )
	{
		UINT offset = i*4;

		pDestViewportIndices[offset] = i;
		pDestVerts[offset].Pos = pos(0.0f, 0.1f);
		pDestVerts[offset].Color = (i%2) ? MultiColors[i - 1] : MultiColors[i];
		
		pDestViewportIndices[1+offset] = i;
		pDestVerts[1+offset].Pos = pos(0.9f, 1.0f);
		pDestVerts[1+offset].Color = (i%2) ? MultiColors[(i + 4) % 15] : MultiColors[(i + 5) % 15];

		pDestViewportIndices[2+offset] = i;
		pDestVerts[2+offset].Pos = pos(1.0f, 0.9f);
		pDestVerts[2+offset].Color = (i%2) ? MultiColors[(i + 9) % 15] : MultiColors[(i + 10) % 15];

		pDestViewportIndices[3+offset] = i;
		pDestVerts[3+offset].Pos = pos(0.0f, 0.1f);
		pDestVerts[3+offset].Color = (i%2) ? MultiColors[i - 1] : MultiColors[i];
	}

	GetEffectiveContext()->Unmap(m_pVBuffer,0);
	GetEffectiveContext()->Unmap(m_pVIBuffer,0);

	return RESULT_PASS;
}

TEST_RESULT SubScissor::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	FLOAT RelativeEpsilon = 1.0f;
	unsigned int uNumberofVerticesToDraw;

	BYTE* texMapSciss = NULL;
    BYTE* overdrawTexMapSciss = NULL;
	D3D11_MAPPED_SUBRESOURCE texMapView;
	D3D11_MAPPED_SUBRESOURCE texMapTemp;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMap;
    D3D11_MAPPED_SUBRESOURCE overdrawTexMapTemp;

	// We need to change our test slightly when we are trying to validate 
	// rendered pixels for UAVs.  When testing pixel colors rendered for
	// UAVs we cannot draw overlapping triangles, since there is no gauranteed
	// order that they will be drawn.  Instead we will change the vertex count
	// to only render 1 triangle. 
	// However, for validating overdraw count, we want to render both. 
	// It is therefore necessary to validate rendered pixels and overdraw in 
	// separate test cases.
	//
	if (m_ColorOrOverDraw == TEST_COLOR && m_OutputType == OUTPUT_TYPE_UAV)
		uNumberofVerticesToDraw = 3;
	else 
		uNumberofVerticesToDraw = 4;

	ClearOutputTextures();
    for( UINT vp = 0; vp < m_NumViewports; vp++ )
    {
    	GetEffectiveContext()->Draw( uNumberofVerticesToDraw, vp*4 );
    }
	
	if( !MapRT( &texMapTemp, &overdrawTexMapTemp ) ) // Executes the effective context
	{
		WriteToLog( _T("Failed to map render target."));
		tRes = RESULT_FAIL;
	}

	UINT texSize = m_RTHeight * texMapTemp.RowPitch;
	texMapSciss = new BYTE[texSize];
	memcpy(texMapSciss, (void*)texMapTemp.pData, texSize);

    if( m_OutputType == OUTPUT_TYPE_UAV )
    {
        texSize = m_RTHeight * overdrawTexMapTemp.RowPitch;
        overdrawTexMapSciss = new BYTE[texSize];
        memcpy(overdrawTexMapSciss, (void*)overdrawTexMapTemp.pData, texSize);
    }

	g_App.Present();
	UnMapRT(true);

	//draw again without scissors and compare
	GetEffectiveContext()->RSSetState( m_pRSView );
	ClearOutputTextures();
    for( UINT vp = 0; vp < m_NumViewports; vp++ )
    {
    	GetEffectiveContext()->Draw( uNumberofVerticesToDraw, vp*4 );
    }
	
	if ( !MapRT( &texMapView, &overdrawTexMap ) ) // Executes the effective context
	{
		WriteToLog( "Failed to map render target." );
		tRes = RESULT_FAIL;
	}

	UINT16 *pDataSciss = NULL; //On the assumption that we're looking at 16-bit floats.
	UINT16 *pDataView = NULL;
    UINT32 *pOverdrawDataSciss = NULL;
    UINT32 *pOverdrawDataView = NULL;

	for ( UINT i = 0; i < m_NumViewports; i = i + 2 )
	{
		UINT FailedPix = 0, PassedPix = 0;
		UINT yOffset = (UINT) m_Viewports[i].TopLeftY;
		UINT xOffset = (UINT) m_Viewports[i].TopLeftX;
		bool bViewportFailed = false;
			
		pDataSciss = (UINT16*)(texMapSciss + yOffset *  texMapView.RowPitch);			
		pDataSciss += xOffset * 4;

		pDataView = (UINT16*)texMapView.pData + yOffset * texMapView.RowPitch / sizeof(UINT16);			
		pDataView += xOffset * 4;

        if( m_OutputType == OUTPUT_TYPE_UAV )
        {
            pOverdrawDataSciss = (UINT32*)(overdrawTexMapSciss + yOffset *  overdrawTexMap.RowPitch);			
		    pOverdrawDataSciss += xOffset;

		    pOverdrawDataView = (UINT32*)overdrawTexMap.pData + yOffset * overdrawTexMap.RowPitch / sizeof(UINT32);			
		    pOverdrawDataView += xOffset;
        }

		for ( UINT y = 0; y < m_Viewports[i].Height; ++y )
		{				
			for ( UINT x = 0; x < m_Viewports[i].Width; ++x )
			{
				bool CurPixelColorPassed = false;

				//If this is a color test, compare the two colors
				if (m_ColorOrOverDraw == TEST_COLOR)
				{
					color ColorsS = color(
						CDXGIFloat16(pDataSciss[x*4]),
						CDXGIFloat16(pDataSciss[x*4+1]),
						CDXGIFloat16(pDataSciss[x*4+2])
					);

					color ColorsV = color(
						CDXGIFloat16(pDataView[x*4]),
						CDXGIFloat16(pDataView[x*4+1]),
						CDXGIFloat16(pDataView[x*4+2])
					);

					if ( ColorsV.r != ColorsS.r ||
						 ColorsV.g != ColorsS.g ||
						 ColorsV.b != ColorsS.b )
					{
						++FailedPix;
	
						bViewportFailed = true;
						tRes = RESULT_FAIL;	
					}
					else
					{
						++PassedPix;
					}
				}
	            // If outputting to UAVs, check for proper OverdrawCount
                else if( m_OutputType == OUTPUT_TYPE_UAV && m_ColorOrOverDraw == TEST_OVERDRAW )
                {
                    UINT32 ExpectedOverdrawCountSciss = pOverdrawDataView[x];

                    // Adjust the expected overdraw if we're outside of the scissor rect
                    if( (LONG)(x + xOffset) < m_Scissors[i].left || (LONG)(x + xOffset) >= m_Scissors[i].right ||
                        (LONG)(y + yOffset) < m_Scissors[i].top || (LONG)(y + yOffset) >= m_Scissors[i].bottom ) 
					{
                        ExpectedOverdrawCountSciss = ( ExpectedOverdrawCountSciss > 1 ) ? ExpectedOverdrawCountSciss - 2 : ExpectedOverdrawCountSciss;
                    }

                    if( pOverdrawDataSciss[x] != ExpectedOverdrawCountSciss )
                    {
                        bViewportFailed = true;
                        tRes = RESULT_FAIL;

                        // Increment the unexpected pixel count
                        ++FailedPix;
                    }
					else
					{
						++PassedPix;
					}

                }
            }

			pDataView += texMapView.RowPitch / sizeof(UINT16);
			pDataSciss += texMapView.RowPitch / sizeof(UINT16);

            pOverdrawDataSciss += overdrawTexMap.RowPitch / sizeof(UINT32);
		    pOverdrawDataView += overdrawTexMap.RowPitch / sizeof(UINT32);
		}

		if ( bViewportFailed )
		{
			//Output info for log
			WriteToLog( "\n" );
			WriteToLog( "Failed pixels/overdraw in viewport %d: %d / %d", i, FailedPix, (FailedPix + PassedPix) );
		}
	}
	UnMapRT(true);

	delete[] texMapSciss;
    delete[] overdrawTexMapSciss;

	return tRes;
}
