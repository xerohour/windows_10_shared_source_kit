///////////////////////////////////////////////////////////
// FILE:        TextureStage3.cpp
// DESC:        TextureStageTest 3rd generation.
// AUTHOR:      Bobby Paulose 
//				Bill Kristiansen
// Created:		June 11, 2002.
///////////////////////////////////////////////////////////


#include "TextureStage3.h"

////The test groups to run///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
CD3DWindowFramework TheApp;

CTSSTestEightStage g_TextureStageEfficientEightStage( "8Stages" );
CTSSTestSevenStage g_TextureStageEfficientSevenStage( "7Stages" );
CTSSTestSixStage g_TextureStageEfficientSixStage( "6Stages" );
CTSSTestFiveStage g_TextureStageEfficientFiveStage( "5Stages" );
CTSSTestFourStage g_TextureStageEfficientFourStage( "4Stages" );
CTSSTestThreeStage g_TextureStageEfficientThreeStage( "3Stages" );
CTSSTestTwoStage g_TextureStageEfficientTwoStage( "2Stages" );
CTSSTestOneStage g_TextureStageEfficientOneStage( "1Stages" );


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


CBaseTSSTest::CBaseTSSTest() :
	m_pVB4VrtxDST2( NULL ), 
	m_pVB4VrtxDST2_HalfDiffuse( NULL ), 
	m_pVB4VrtxDST2_ZeroDiffuse( NULL ), 
	m_pVB4VrtxDST2_DiffuseAlphaGradient( NULL ), 
	m_pVB4VrtxDST2T2( NULL ), 
	m_pVB4VrtxDST2T2half( NULL ), 
	m_pVB4VrtxDST2T3T3_forVolCube( NULL ), 
	m_pVB4VrtxDST2T3T3_forMipMap( NULL ),
	bPointFilter( false ),
	m_uTotalTestCycles( 0 ),
	m_dwPrimitiveMiscCaps( 0 ),
	m_dwRasterCaps( 0 ),
	m_dwTextureCaps( 0 ),
	m_dwTextureOpCaps( 0 ),
	m_dwSrcAlphaBlendCaps( 0 ),
	m_dwTextureStageCaps( 0 ),
	m_wMaxSimultaneousTextures( 0 ),
	m_uNumTestCases( 0 ),
	m_uIndexTestCase( 0 ),
	m_uFormatCycle( 0 )
{
	memset( m_IndxTriList, 0, sizeof( m_IndxTriList ) );
	memset( m_IndxTriStrip, 0, sizeof( m_IndxTriStrip ) );
	memset( m_IndxTriFan, 0, sizeof( m_IndxTriFan ) );
	memset( m_pStageFormat, 0, sizeof( m_pStageFormat ) );
    memset( m_pTextures, 0, sizeof( m_pTextures ) );
    
    SetTestType(TESTTYPE_CONF);
 
	// Set framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
    m_pFramework->GetImageCmp()->SetMinPixels(0);

	m_Options.D3DOptions.nBackBufferWidth = DISPLAY_WIDTH;
	m_Options.D3DOptions.nBackBufferHeight = DISPLAY_HEIGHT;

	bPointFilter = false;
    
    return;
}

CBaseTSSTest::~CBaseTSSTest()
{
}

UINT CBaseTSSTest::GetTestCaseIndexToRun(UINT uiTest, UINT *puFormatCycle)
{
	UINT uTotalTestCycles=0;

	for (UINT i=0; i<m_uNumTestCases; i++)
	{
		uTotalTestCycles = uTotalTestCycles + GetTestCaseNumCycles( i );

		if (uiTest <= uTotalTestCycles)
		{
			//we've found the test_case to run
			if (puFormatCycle) *puFormatCycle = uiTest - (uTotalTestCycles - GetTestCaseNumCycles( i ));
			return i;
		}
	}

	//control shud not reach here!
	if (puFormatCycle) *puFormatCycle = 0;
	return 0;
}

bool CBaseTSSTest::ExecuteTest( UINT uiTest )
{
	TCHAR sztBuffer[BUFFER_LENGTH];	

	//Destroy the textures created for the last test. The last test index
	// is now in m_uIndexTestCase.
	for( int i=0; i < GetNumStages(); i++ )
	{
		if( m_pTextures[i] )
		{
			//release this texture.
			RELEASE( m_pTextures[i] );
		}
	}

	m_uIndexTestCase = GetTestCaseIndexToRun( uiTest, &m_uFormatCycle );

	sprintf( sztBuffer, "<<%s>> [%dTexStages]", GetTestCaseDescription(m_uIndexTestCase), GetNumStages() );
	BeginTestCase( sztBuffer, uiTest );

	//we don't want to cull out any triangle drawn.
	SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

	if( !ExecuteTSStestCase( m_uIndexTestCase, m_uFormatCycle ) )
	{
		WriteToLog( _T( "\n ExecuteTSStestCase failed for test-case %d, format cycle %d" ), m_uIndexTestCase, m_uFormatCycle );
		return false;
	}

	return true;
}
	
void CBaseTSSTest::SceneRefresh()
{
	if( !CBaseTSSTest::DrawPrimitiveID( GetTestCasePrimID( m_uIndexTestCase ) ) )
	{
		WriteToLog( "\n(ERROR) Failed DrawPrimitiveID. (%08X)", GetLastError() );
	}
}

bool CBaseTSSTest::ExecuteTSStestCase( UINT uTestCaseIndex, UINT uFormatCycle )
{
	//Create textures required for this test case.
	if( !CreateTexturesForThisTestCase( uTestCaseIndex, uFormatCycle ) )
	{
		WriteToLog(_T("\nCreateTextures failed for test-case %d, format cycle %d"), m_uIndexTestCase,m_uFormatCycle);
		return false;
	}

	//Set RenderStates
	int i;
	for(i = 0; i < NUM_RENDER_STATES; i++ )
	{
		D3DRENDERSTATETYPE rs;
		DWORD value;
		
		GetTestCaseRenderStateValue( uTestCaseIndex, i, rs, value );
		
		if( rs != 0 )
		{
			SetRenderState( rs, value );
		}
	}
	
	//Adapt to RenderState options and caps.
	if( !( m_dwSrcAlphaBlendCaps ) )
	{
		SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );
	}


	//Set TextureStageStates and Textures.
	for( i = 0; i < GetNumStages(); i++)
	{
		//Set TextureOp and Args
		TextureStageState const &stage = GetTestCaseStage( uTestCaseIndex, i );
			
		SetTextureStageState( i, D3DTSS_COLOROP, stage.ColorOp );
		SetTextureStageState( i, D3DTSS_COLORARG1, stage.ColorArg1 );
		SetTextureStageState( i, D3DTSS_COLORARG2, stage.ColorArg2 );
		SetTextureStageState( i, D3DTSS_COLORARG0, stage.ColorArg0 );
								
		SetTextureStageState( i, D3DTSS_ALPHAOP, stage.AlphaOp );
		SetTextureStageState( i, D3DTSS_ALPHAARG1, stage.AlphaArg1 );
		SetTextureStageState( i, D3DTSS_ALPHAARG2, stage.AlphaArg2 );
		SetTextureStageState( i, D3DTSS_ALPHAARG0, stage.AlphaArg0 );

		// BUGBUG - Wrapping is enabled by default in the DirectX runtime. However,
		// with the addition of the new USAGE_QUERY_WRAPANDMIP query some (Nvidia)
		// parts are failing the bump map cases which can push texture coordinates
		// outside the range of [0,1].  I have implemented a hack to handle this special
		// case using the below variables.
		if (m_pD3D->GetRuntimeVersion() >= 9.029f)
		{
			if( i > 0 && m_pStageFormat[i] )
			{
				TextureStageState const &prevstage = GetTestCaseStage( uTestCaseIndex, i - 1 );
				
				if( prevstage.ColorOp == D3DTOP_BUMPENVMAP || prevstage.ColorOp == D3DTOP_BUMPENVMAPLUMINANCE )
				{
					HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
														USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *m_pStageFormat[i]);
					if (FAILED(Result))
					{
						SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
						SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
					}
				}
			}
		}
		else
		{
			SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
			SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		}
	
	

		//Redirect output of this stage to TEMP or CURRENT as specified.
		SetTextureStageState( i, D3DTSS_RESULTARG, stage.ResultTo );


		//Set texture coordinate index for this stage.
		SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, stage.TexCoordIndex );


		// If supported, reset MipMapLODBias back to default state
		if( m_dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS )
		{
			SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, (DWORD) 0 );
		}

		//Set other TextureStageStates
		for( int j = 0; j < NUM_TEXSTAGE_STATES; j++ )
		{
			D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
			
			if( tss != 0 )
			{
				if( ( D3DTSS_BUMPENVMAT00 == tss ) ||
					( D3DTSS_BUMPENVMAT11 == tss ) ||
					( D3DTSS_BUMPENVMAT10 == tss ) ||
					( D3DTSS_BUMPENVMAT01 == tss ) ||
					( D3DTSS_BUMPENVLSCALE == tss ) ||
					( D3DTSS_BUMPENVLOFFSET == tss ) ||
					( D3DTSS_MIPMAPLODBIAS == tss) )
				{
					//we need to send a float value disguised as a DWORD.
					float fval = (float)( GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value)/100.0f;
					
					if( (GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value) >> ( sizeof(DWORD)*8 - 1 ) )
					{
						//Negative value. Take two's complement.
						fval = -( (float)((~(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value) ) + 1) )/100.0f;
					}
					
					SetTextureStageState( i, tss, *((DWORD*)&(fval) ) );
				}
				else
				{
					// check if we need to force point sampling
					if (((tss == D3DTSS_MIPFILTER) ||
						(tss == D3DTSS_MAGFILTER) ||
						(tss == D3DTSS_MINFILTER)) &&
						(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
					{
						SetTextureStageState(i, tss, (bPointFilter) ? D3DTEXF_POINT : GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value);
					}
					else
					{
						SetTextureStageState(i, tss, GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value);
					}
				}
			}
		}


		//Set Texture for this stage
		TextureID TexID = GetTestCaseStage( uTestCaseIndex, i ).TexID;
/*		if( GetTestCaseStage( uTestCaseIndex, i ).ColorOp == D3DTOP_DISABLE )
		{
			// Leave garbage in texture stage
		}
		else
*/		if( TexID != TEX_NULL && 
			TexID != TEX_PREVIOUS_STAGE && 
			TexID != TEX_ONE_STAGE_ABOVE && 
			TexID != TEX_TWO_STAGE_ABOVE && 
			TexID != TEX_THREE_STAGE_ABOVE && 
			TexID != TEX_FOUR_STAGE_ABOVE && 
			TexID != TEX_FIVE_STAGE_ABOVE && 
			TexID != TEX_SIX_STAGE_ABOVE && 
			TexID != TEX_SEVEN_STAGE_ABOVE && 
			GetTestCaseStage( uTestCaseIndex, i ).CycID != CYC_DONT_CARE 
			)
		{
			SetTexture(i, m_pTextures[i]);
		}
		else if( TexID == TEX_PREVIOUS_STAGE || 
					TexID == TEX_ONE_STAGE_ABOVE || 
					TexID == TEX_TWO_STAGE_ABOVE || 
					TexID == TEX_THREE_STAGE_ABOVE || 
					TexID == TEX_FOUR_STAGE_ABOVE || 
					TexID == TEX_FIVE_STAGE_ABOVE || 
					TexID == TEX_SIX_STAGE_ABOVE || 
					TexID == TEX_SEVEN_STAGE_ABOVE
				)
		{
			//We must find the texture for this stage from a previous stage.
			if( TexID == TEX_PREVIOUS_STAGE )
			{
				//We must find the texture for this stage from the nearest previous stage which has a texture.
				SetTexture( i, NULL);
				for( int index = i - 1; index >= 0; index-- )
				{
					if( m_pTextures[index] != NULL )
					{
						SetTexture( i, m_pTextures[index] );
						break;
					}
				}
			}
			else
			{
				//We must find the texture for this stage from the specified previous stage.
				SetTexture(i, NULL);
				int index = i - ( TexID - TEX_ONE_STAGE_ABOVE + 1 );
				if( index >= 0 )
				{
					SetTexture( i, m_pTextures[index] );
				}
			}
		}
		else 
		{
			SetTexture( i, NULL );
		}
	}

	//Disable the rest of the TextureStages in the hardware and refrast.
	for( i = GetNumStages(); i < MAX_TEX_STAGES; i++ )
	{
		//Set TextureOp to DISABLE
		SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_DISABLE );
		SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		//Set Texture for this stage to NULL
		SetTexture( i, NULL );
	}

	// Make sure that all is fine before rendering
	DWORD dwPasses = 0;
	if( !ValidateDevice( &dwPasses ) )
	{
		// Issue a skip for this case but continue rendering (becomes a driver stability case)
		WriteToLog("ValidateDevice() failed (HResult = %s), skipping case.\n",m_pD3D->HResultToString(GetLastError()));
		Skip();
	}

	return true;
}
	
bool CBaseTSSTest::CreateTexturesForThisTestCase( UINT uTestCaseIndex, UINT uFormatCycle )
{
	m_pStageFormat[0] =
	m_pStageFormat[1] =
	m_pStageFormat[2] =
	m_pStageFormat[3] =
	m_pStageFormat[4] =
	m_pStageFormat[5] =
	m_pStageFormat[6] =
	m_pStageFormat[7] = NULL;
	
	UINT uQuotient=uFormatCycle,uFmtListIndex=0;

	int i;
	for (i=0; i<GetNumStages(); i++)
	{
		TextureID TexID = GetTestCaseStage( uTestCaseIndex, i ).TexID;
		CycleID CycID = GetTestCaseStage( uTestCaseIndex, i ).CycID;
		
		if( TexID != TEX_NULL && 
			TexID != TEX_PREVIOUS_STAGE && 
			TexID != TEX_ONE_STAGE_ABOVE && 
			TexID != TEX_TWO_STAGE_ABOVE && 
			TexID != TEX_THREE_STAGE_ABOVE && 
			TexID != TEX_FOUR_STAGE_ABOVE && 
			TexID != TEX_FIVE_STAGE_ABOVE && 
			TexID != TEX_SIX_STAGE_ABOVE && 
			TexID != TEX_SEVEN_STAGE_ABOVE && 
			CycID != CYC_DONT_CARE
			)
		{
			//We need to Create Texture for this stage.
			//Let's find out what format the texture need to be.
			if( CycID != CYC_PREVIOUS_STAGE_DEPENDENT &&
				CycID != CYC_ONE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_TWO_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_THREE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_FOUR_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_FIVE_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_SIX_STAGE_ABOVE_DEPENDENT &&
				CycID != CYC_SEVEN_STAGE_ABOVE_DEPENDENT
				)
			{
				//We need to compute and find the format to create the texture.
				uFmtListIndex = uQuotient % m_VariousTexFormatCycles[CycID].uNumFormats;
				uQuotient = uQuotient / m_VariousTexFormatCycles[CycID].uNumFormats;

				m_pStageFormat[i] = &(m_VariousTexFormatCycles[CycID].pFormats[uFmtListIndex]);
			}
			else
			{
				//We can find the format to create the texture from a previous stage.
				if (CycID == CYC_PREVIOUS_STAGE_DEPENDENT)
				{
					for (int index=i-1; index>=0; index--)
					{
						if (m_pStageFormat[index] != NULL)
						{
							m_pStageFormat[i] = m_pStageFormat[index];
							break;
						}
					}
				}
				else
				{
					int index = i - (CycID - CYC_ONE_STAGE_ABOVE_DEPENDENT + 1);
					if (index >= 0)
					{
						m_pStageFormat[i] = m_pStageFormat[index];
					}
				}
			}


			//We've found the format. Now Create the texture.
			if (NULL != m_pStageFormat[i])
			{
				//create the texture for this stage.
				if ( (TexID != TEX_MIPS) && (TexID != TEX_VOLUME) && (TexID != TEX_CUBE) )
				{
					//create the texture for this stage.
					RELEASE(m_pTextures[i]);

					if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
					{
						// Need to check the Filter Query for Dx9
						if (m_dwVersion >= 0x0900)
						{
							// Assume we can filter the format
							bPointFilter = false;

							if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																USAGE_QUERY_FILTER, RTYPE_TEXTURE, *m_pStageFormat[i])))
							{
								// Unable to filter this format, use point sampling
								for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
								{
									D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;

									if (((tss == D3DTSS_MIPFILTER) ||
										(tss == D3DTSS_MAGFILTER) ||
										(tss == D3DTSS_MINFILTER)) &&
										(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
									{
										bPointFilter = true;
									}
								}
							}
						}

						// Tell log about our texture
						TCHAR szBuffer[80], msgString[80];
						msgString[0] = '\0';

						FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
						sprintf(msgString, "TexFmt: %s",szBuffer);

						WriteToLog(_T("CreateTexture(), %s\n"),msgString);

						// Create Texture and Load Texture from CImage.
						if (!CreateTexture(
							m_TexImages[TexID].GetWidth(), 
							m_TexImages[TexID].GetHeight(), 
							1, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnTexture**)&(m_pTextures[i])
							))
						{
							WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"), TexID);
							return false;
						}

						//Load Texture from CImage.
						if (!(((CnTexture*)m_pTextures[i])->LoadImage(1, &m_TexImages[TexID])))
						{
							WriteToLog(_T("\nUnable to load texture from CImage ID:%d"),TexID);
							return false;
						}

						//Set Palette if required.
						if (((CnTexture*)m_pTextures[i])->IsPalettized())
						{
							if (!(((CnTexture*)m_pTextures[i])->SetPalette(NULL)))
							{
								WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
								Fail();
								return false;
							}
						}
					}
					else
					{
					/* This ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
					//Create Texture from file.
						if (!D3DXCreateTextureFromFileEx(
							g_TexFileNames[TexID - TEX_LAST_PLACEHOLDER_CIMAGE],
							DEFAULT_TEXWIDTH,
							DEFAULT_TEXHEIGHT,
							1, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT,
							0,0,0,NULL,NULL,
							&(m_pTextures[i])
							))

						{
							WriteToLog(_T("\nUnable to Create texture from file :%s"),g_TexFileNames[TexID - TEX_LAST_PLACEHOLDER_CIMAGE]);
							return false;
						}
					*/
					WriteToLog(_T("\nUnable to Create texture from file."));
					}



				}
				else //create mips, volume, cube texture
				{
					//create the texture for this stage.
					RELEASE(m_pTextures[i]);

					if (TexID == TEX_MIPS)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Tell log about our texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "TexFmt: %s",szBuffer);

							WriteToLog(_T("CreateTexture(), %s\n"),msgString);

							// On Dx9.L, need to check for the special A1 format
							if ((m_pD3D->GetVersion() >= 9.1f) && (m_pStageFormat[i]->d3dfFormat == FMT_A1))
							{
								WriteToLog("Unable to mipmap this format. D3DFMT_A1 does not support mipmapping.\n");
								Skip();
								return true;
							}

							// On Dx9c, need to check for UsageQueryWrapAndMip
							if (m_pD3D->GetRuntimeVersion() >= 9.029f)
							{
								HRESULT Result = m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *m_pStageFormat[i]);
								if (FAILED(Result))
								{
									WriteToLog("Unable to mipmap this format. CheckDeviceFormat(WrapAndMip) failed: %s.\n",m_pD3D->HResultToString(Result));
									Skip();
									return true;
								}
							}

							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_TEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Create Texture and Load Texture from CImage.
							if (!CreateTexture(
								0x4 << MIP_LEVELS, 
								0x4 << MIP_LEVELS, 
								MIP_LEVELS, FMWK_DEFAULT, *m_pStageFormat[i], FMWK_DEFAULT, (CnTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_MipImages[MIP_LEVELS];
							for (int miplevel=0; miplevel<MIP_LEVELS; miplevel++)
							{
								p_MipImages[miplevel] = &m_TexImages[TexID + miplevel];
							}
							if (!(((CnTexture*)m_pTextures[i])->LoadImage(0, p_MipImages)))
							{
								WriteToLog(_T("\nUnable to load Mips texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_MIPS
					else if (TexID == TEX_VOLUME)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_VOLUMETEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Tell log about our volume texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "VolFmt: %s",szBuffer);

							WriteToLog(_T("CreateVolumeTexture(), %s\n"),msgString);

							// Create Volume Texture and Load Volume Texture from CImage.
							if (!CreateVolumeTexture(
								m_TexImages[TexID].GetWidth(), 
								m_TexImages[TexID].GetHeight(), 
								VOLUME_DEPTH, VOLUME_MIP_LEVELS, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnVolTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_VolumeImages[VOLUME_MIP_LEVELS*6];
							//Note: Not sure which "for" must come first.
							//Since we won't be using volume mip map for now, its ok.
							for(int vol_level=0; vol_level< VOLUME_DEPTH; vol_level++)
							for (int miplevel=0; miplevel<VOLUME_MIP_LEVELS; miplevel++)
							{
								p_VolumeImages[vol_level*VOLUME_MIP_LEVELS + miplevel] = &m_TexImages[TexID + (vol_level*VOLUME_MIP_LEVELS) + miplevel];
							}
							if (!(((CnVolTexture*)m_pTextures[i])->LoadImage(0, p_VolumeImages)))
							{
								WriteToLog(_T("\nUnable to load Volume texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnVolTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnVolTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_VOLUME
					else if (TexID == TEX_CUBE)
					{
						if (TexID < TEX_LAST_PLACEHOLDER_CIMAGE)
						{
							// Need to check the Filter Query for Dx9
							if (m_dwVersion >= 0x0900)
							{
								// Assume we can filter the format
								bPointFilter = false;

								if (FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																	USAGE_QUERY_FILTER, RTYPE_CUBETEXTURE, *m_pStageFormat[i])))
								{
									// Unable to filter this format, use point sampling
									for (int j=0; j<NUM_TEXSTAGE_STATES; j++)
									{
										D3DTEXTURESTAGESTATETYPE tss = GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].TSS;
										
										if (((tss == D3DTSS_MIPFILTER) ||
											(tss == D3DTSS_MAGFILTER) ||
											(tss == D3DTSS_MINFILTER)) &&
											(GetTestCaseStage( uTestCaseIndex, i ).OtherTexStageStates[j].Value == D3DTEXF_LINEAR))
										{
											bPointFilter = true;
										}
									}
								}
							}

							// Tell log about our cube texture
							TCHAR szBuffer[80], msgString[80];
							msgString[0] = '\0';

							FmtToString(FORMAT_TEXTURE,m_pStageFormat[i],szBuffer);
							sprintf(msgString, "CubeFmt: %s",szBuffer);

							WriteToLog(_T("CreateCubeTexture(), %s\n"),msgString);

							// Create Cube Texture and Load Cube Texture from CImage.
							if (!CreateCubeTexture(
								m_TexImages[TexID].GetWidth(), 
								CUBE_MIP_LEVELS, (DWORD) 0, *m_pStageFormat[i], FMWK_DEFAULT, (CnCubeTexture**)&(m_pTextures[i])
								))
							{
								WriteToLog(_T("\n(ERROR) Failed to create texture.TexID:%d"),TexID);
								return false;
							}

							//Load Texture from CImage.
							CImage *p_CubeImages[CUBE_MIP_LEVELS*6];
							for(int face=0; face< 6; face++)
							for (int miplevel=0; miplevel<CUBE_MIP_LEVELS; miplevel++)
							{
								p_CubeImages[face*CUBE_MIP_LEVELS + miplevel] = &m_TexImages[TexID + (face*CUBE_MIP_LEVELS) + miplevel];
							}
							if (!(((CnCubeTexture*)m_pTextures[i])->LoadImage(0, p_CubeImages)))
							{
								WriteToLog(_T("\nUnable to load Cube texture :%d"),TexID);
								return false;
							}

							//Set Palette if required.
							if (((CnCubeTexture*)m_pTextures[i])->IsPalettized())
							{
								if (!(((CnCubeTexture*)m_pTextures[i])->SetPalette(NULL)))
								{
									WriteToLog("SetPalette() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
									Fail();
									return false;
								}
							}
						}
						else
						{
							// The ability to load texture from a file is disabled for now because it must be supported in lower DX versions too.
							WriteToLog(_T("\nUnable to Create texture from file."));
						}
					} // end TEX_CUBE

				} //end - create mips, volume, cube texture

			}
			else
			{
				//we should not reach here! TestCase Description error.
				WriteToLog(_T("\nCannot find texture creation format for TexStage %d. Possible TestCase Description error."), i);
			}
		}
		//else if (TexID != TEX_NULL)
		//{
			//We can use the same texture used in a previous stage.
			//No need to Create texture.
		//}
	} //for loop end.

	return true;
}


void CBaseTSSTest::CommandLineHelp(void)
{
    WriteCommandLineHelp("$y-#nStages\t$wRun test-cases for testing exactly #n texture-stages.");

    WriteCommandLineHelp("$y-NoAlpha\t$wNo alpha blending. Alpha computed not used");
    WriteCommandLineHelp("$y-NoCubeMap\t$wRemove all tests using CubeMap texture.");
    WriteCommandLineHelp("$y-NoVolumeMap\t$wRemove all tests using VolumeMap texture.");
    WriteCommandLineHelp("$y-NoMipMap\t$wRemove all tests using MipMap texture.");
    WriteCommandLineHelp("$y-MaxSimultTextures:#n\t$wLimit all tests to use a max of #n textures\n in different texture stages simultaneously.");

    WriteCommandLineHelp("$y-TexOpRemove:D3DTOP_X,D3DTOP_Y,...\t$wRemove all test-cases using\n texop D3DTOP_X and D3DTOP_Y and....");
    WriteCommandLineHelp("$y-TexArgRemove:D3DTA_X,D3DTA_Y,...\t$wRemove all test-cases using\n texarg D3DTA_X and D3DTA_Y and....");
}

/*
TCHAR *g_TexFileNames[TEX_LAST_PLACEHOLDER - TEX_LAST_PLACEHOLDER_CIMAGE] =
{
	"nonExistentFile.tex",
	"test.bmp",
};
*/


TCHAR *g_TexOpNames[] =
{
    // Control
	_T(""),		//This makes the names offset by their values.
    _T("D3DTOP_DISABLE"),              // = 1,      // disables stage
    _T("D3DTOP_SELECTARG1"),           // = 2,      // the default
    _T("D3DTOP_SELECTARG2"),           // = 3,

    // Modulate
    _T("D3DTOP_MODULATE"),             // = 4,      // multiply args together
    _T("D3DTOP_MODULATE2X"),           // = 5,      // multiply and  1 bit
    _T("D3DTOP_MODULATE4X"),           // = 6,      // multiply and  2 bits

    // Add
    _T("D3DTOP_ADD"),                  // =  7,   // add arguments together
    _T("D3DTOP_ADDSIGNED"),            // =  8,   // add with -0.5 bias
    _T("D3DTOP_ADDSIGNED2X"),          // =  9,   // as above but left  1 bit
    _T("D3DTOP_SUBTRACT"),             // = 10,   // Arg1 - Arg2, with no saturation
    _T("D3DTOP_ADDSMOOTH"),            // = 11,   // add 2 args, subtract product
                                        // Arg1 + Arg2 - Arg1*Arg2
                                        // = Arg1 + (1-Arg1)*Arg2

    // Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
    _T("D3DTOP_BLENDDIFFUSEALPHA"),    // = 12, // iterated alpha
    _T("D3DTOP_BLENDTEXTUREALPHA"),    // = 13, // texture alpha
    _T("D3DTOP_BLENDFACTORALPHA"),     // = 14, // alpha from D3DRS_TEXTUREFACTOR

    // Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
    _T("D3DTOP_BLENDTEXTUREALPHAPM"),  // = 15, // texture alpha
    _T("D3DTOP_BLENDCURRENTALPHA"),    // = 16, // by alpha of current color

    // Specular mapping
    _T("D3DTOP_PREMODULATE"),            // = 17,     // modulate with next texture before use
    _T("D3DTOP_MODULATEALPHA_ADDCOLOR"), // = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
                                            // COLOROP only
    _T("D3DTOP_MODULATECOLOR_ADDALPHA"), // = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
                                            // COLOROP only
    _T("D3DTOP_MODULATEINVALPHA_ADDCOLOR"), // = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
                                            // COLOROP only
    _T("D3DTOP_MODULATEINVCOLOR_ADDALPHA"), // = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
                                            // COLOROP only

    // Bump mapping
    _T("D3DTOP_BUMPENVMAP"),           // = 22, // per pixel env map perturbation
    _T("D3DTOP_BUMPENVMAPLUMINANCE"),  // = 23, // with luminance channel

    // This can do either diffuse or specular bump mapping with correct input.
    // Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
    // where each component has been scaled and offset to make it signed.
    // The result is replicated into all four (including alpha) channels.
    // This is a valid COLOROP only.
    _T("D3DTOP_DOTPRODUCT3"),          // = 24,

    // Triadic ops
    _T("D3DTOP_MULTIPLYADD"),          // = 25, // Arg0 + Arg1*Arg2
    _T("D3DTOP_LERP"),                 // = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

    _T("D3DTOP_FORCE_DWORD"), // = 0x7fffffff,
};




bool CBaseTSSTest::ReadAndFilterCapsBasedOnOptions()
{
	//read hardware caps.
	m_dwPrimitiveMiscCaps = m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps;
	m_dwRasterCaps = m_pSrcDevice->GetCaps()->dwRasterCaps;
	m_dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
	m_dwTextureOpCaps = m_pSrcDevice->GetCaps()->dwTextureOpCaps;
	m_dwSrcAlphaBlendCaps = m_pSrcDevice->GetCaps()->dwSrcBlendCaps;
	m_wMaxSimultaneousTextures = m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;

	//modify caps based on options specified for test run.

	//option NoAlpha
	if (KeySet(_T("NoAlpha")))
    {
		//clear alpha blending caps.
        m_dwSrcAlphaBlendCaps = (DWORD)0;
    }

	//option NoCubeMap
	if (KeySet(_T("NoCubeMap")))
    {
		//clear cube map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_CUBEMAP));
    }

	//option NoVolumeMap
	if (KeySet(_T("NoVolumeMap")))
    {
		//clear volume map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_VOLUMEMAP));
    }

	//option NoMipMap
	if (KeySet(_T("NoMipMap")))
    {
		//clear mip map caps.
        m_dwTextureCaps = m_dwTextureCaps & (~((DWORD)D3DPTEXTURECAPS_MIPMAP));
    }


	//option MaxSimultTextures
	int nMaxSimultTextures = MAX_TEX_STAGES;
	if (ReadInteger(_T("MaxSimultTextures"), &nMaxSimultTextures))
	{
		if (nMaxSimultTextures < 0)
		{
			nMaxSimultTextures = 0;
		}
		if (nMaxSimultTextures < m_wMaxSimultaneousTextures)
		{
			m_wMaxSimultaneousTextures = (WORD)nMaxSimultTextures;
		}
	}


	TCHAR sztOptionBuffer[OPTION_BUFFER_LENGTH];
	TCHAR *pSeparator = _T("|,;");
	//note: blank cannot be a texop separator since it is a key separator.
	TCHAR *pStart, *pEnd;
	UINT len=0,option_len=0;

	//option TexOpRemove
	if (ReadString(_T("TexOpRemove"), sztOptionBuffer, ARRAYSIZE(sztOptionBuffer)))
	{
		//force null terminate if option specified is too long.
		sztOptionBuffer[OPTION_BUFFER_LENGTH-1] = (TCHAR)0;
		pStart = pEnd = sztOptionBuffer;
		option_len = _tcslen(sztOptionBuffer);
		while (pStart < (sztOptionBuffer + option_len))
		{
			if ((len = _tcscspn(pStart, pSeparator)) > 0)
			{
				//we've found a tex_op.
				pEnd = pStart + len;
				*pEnd = (TCHAR)0;

				//search the tex_op found in the list and update caps.
				for (int i=0; i<D3DTOP_LAST_PLACEHOLDER; i++)
				{
					if (_tcscmp(g_TexOpNames[i], pStart) == 0)
					{
						//we've found the tex_op in the list.
						//Now update caps.
						m_dwTextureOpCaps = ( m_dwTextureOpCaps & (~((DWORD)(1 << (i - 1) ))) );
					}
				}


				pStart = pEnd;
			}

			//advance
			pStart++;
		}
	}




	//option TexArgRemove
	len=0;
	option_len=0;
	if (ReadString(_T("TexArgRemove"), sztOptionBuffer, ARRAYSIZE(sztOptionBuffer)))
	{
		//force null terminate if option specified is too long.
		sztOptionBuffer[OPTION_BUFFER_LENGTH-1] = (TCHAR)0;
		pStart = pEnd = sztOptionBuffer;
		option_len = _tcslen(sztOptionBuffer);
		while (pStart < (sztOptionBuffer + option_len))
		{
			if ((len = _tcscspn(pStart, pSeparator)) > 0)
			{
				//we've found a tex_arg.
				pEnd = pStart + len;
				*pEnd = (TCHAR)0;

				if (_tcscmp(_T("D3DTA_TEMP"), pStart) == 0)
				{
					//clear caps D3DPMISCCAPS_TSSARGTEMP
					m_dwPrimitiveMiscCaps = m_dwPrimitiveMiscCaps & (~((DWORD)D3DPMISCCAPS_TSSARGTEMP));
				}
				else if (_tcscmp(_T("D3DTA_CONSTANT"), pStart) == 0)
				{
					//clear caps D3DPMISCCAPS_PERSTAGECONSTANT
					m_dwPrimitiveMiscCaps = m_dwPrimitiveMiscCaps & (~((DWORD)D3DPMISCCAPS_PERSTAGECONSTANT));
				}
				else if (_tcscmp(_T("D3DTA_SPECULAR"), pStart) == 0)
				{
					//the remove D3DTA_SPECULAR is for intel i81x hardware.
					//They can't do D3DTA_SPECULAR.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_SPECULAR));
				}
				else if (_tcscmp(_T("D3DTA_COMPLEMENT"), pStart) == 0)
				{
					//most pre dx8 hardware can't do D3DTA_COMPLEMENT.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_COMPLEMENT));
				}
				else if (_tcscmp(_T("D3DTA_ALPHAREPLICATE"), pStart) == 0)
				{
					//most pre dx8 hardware can't do D3DTA_ALPHAREPLICATE.
					m_dwTextureStageCaps = m_dwTextureStageCaps & (~((DWORD)D3DTSCAPS_ALPHAREPLICATE));
				}


				pStart = pEnd;
			}

			//advance
			pStart++;
		}
	}





	return true;
}


bool CBaseTSSTest::GenerateTextureImages()
{
	//test texture.
    if ( !m_TexImages[TEX_TEST].Load("test.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load test.bmp.  Aborting.");
        return false;
    }

	//black and white checker board.
    if ( !m_TexImages[TEX_CHECKER].Load("checker.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load checker.bmp.  Aborting.");
        return false;
    }

	//high frequency black and white checker board.
    if ( !m_TexImages[TEX_CHECKER_HIGHFREQ].Load("checker_highfreq.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load checker_highfreq.bmp.  Aborting.");
        return false;
    }


	//8.1 dx logo for bump.
    if ( !m_TexImages[TEX_DX_LOGOBUMP].Load("dxlogo_bump.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo_bump.bmp.  Aborting.");
        return false;
    }

	//dx logo for bump2.
    if ( !m_TexImages[TEX_DX_LOGOBUMP2].Load("dxlogo_bump2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo_bump2.bmp.  Aborting.");
        return false;
    }

	//8.1 dx logo.
    if ( !m_TexImages[TEX_DX_LOGO].Load("dxlogo.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo.bmp.  Aborting.");
        return false;
    }

	//8.1 dx logo 2.
    if ( !m_TexImages[TEX_DX_LOGO2].Load("dxlogo2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo2.bmp.  Aborting.");
        return false;
    }

	//dx logo 3.
    if ( !m_TexImages[TEX_DX_LOGO3].Load("dxlogo3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo3.bmp.  Aborting.");
        return false;
    }

	//dx logo 4.
    if ( !m_TexImages[TEX_DX_LOGO4].Load("dxlogo4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo4.bmp.  Aborting.");
        return false;
    }

	//dx logo 5.
    if ( !m_TexImages[TEX_DX_LOGO5].Load("dxlogo5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dxlogo5.bmp.  Aborting.");
        return false;
    }

	//decoration.
    if ( !m_TexImages[TEX_DECOR].Load("decoration.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load decoration.bmp.  Aborting.");
        return false;
    }

	//decoration 2.
    if ( !m_TexImages[TEX_DECOR2].Load("decoration2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load decoration2.bmp.  Aborting.");
        return false;
    }

	//dx text1.
    if ( !m_TexImages[TEX_TEXT1].Load("text1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text1.bmp.  Aborting.");
        return false;
    }

	//dx text2.
    if ( !m_TexImages[TEX_TEXT2].Load("text2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text2.bmp.  Aborting.");
        return false;
    }

	//dx text3.
    if ( !m_TexImages[TEX_TEXT3].Load("text3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text3.bmp.  Aborting.");
        return false;
    }

	//dx text4.
    if ( !m_TexImages[TEX_TEXT4].Load("text4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text4.bmp.  Aborting.");
        return false;
    }

	//dx text5.
    if ( !m_TexImages[TEX_TEXT5].Load("text5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text5.bmp.  Aborting.");
        return false;
    }

	//dx text6.
    if ( !m_TexImages[TEX_TEXT6].Load("text6.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load text6.bmp.  Aborting.");
        return false;
    }

	//colors_blend.
    if ( !m_TexImages[TEX_COLORS_BLEND].Load("colors_blend.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load colors_blend.bmp.  Aborting.");
        return false;
    }

	//windows logo.
    if ( !m_TexImages[TEX_WIN_LOGO].Load("winlogo.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo.bmp.  Aborting.");
        return false;
    }

	//windows logo 2.
    if ( !m_TexImages[TEX_WIN_LOGO2].Load("winlogo2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo2.bmp.  Aborting.");
        return false;
    }

	//windows logo 3.
    if ( !m_TexImages[TEX_WIN_LOGO3].Load("winlogo3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load winlogo3.bmp.  Aborting.");
        return false;
    }

	//tex stage0.
    if ( !m_TexImages[TEX_STAGE0].Load("stage0.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage0.bmp.  Aborting.");
        return false;
    }

	//tex stage1.
    if ( !m_TexImages[TEX_STAGE1].Load("stage1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage1.bmp.  Aborting.");
        return false;
    }

	//tex stage2.
    if ( !m_TexImages[TEX_STAGE2].Load("stage2.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage2.bmp.  Aborting.");
        return false;
    }

	//tex stage3.
    if ( !m_TexImages[TEX_STAGE3].Load("stage3.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage3.bmp.  Aborting.");
        return false;
    }

	//tex stage4.
    if ( !m_TexImages[TEX_STAGE4].Load("stage4.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage4.bmp.  Aborting.");
        return false;
    }

	//tex stage5.
    if ( !m_TexImages[TEX_STAGE5].Load("stage5.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage5.bmp.  Aborting.");
        return false;
    }

	//tex stage6.
    if ( !m_TexImages[TEX_STAGE6].Load("stage6.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage6.bmp.  Aborting.");
        return false;
    }

	//tex stage7.
    if ( !m_TexImages[TEX_STAGE7].Load("stage7.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage7.bmp.  Aborting.");
        return false;
    }


	//tex stage_all.
    if ( !m_TexImages[TEX_STAGE_ALL].Load("stage_all.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load stage_all.bmp.  Aborting.");
        return false;
    }


	//"The New TextureStage".
    if ( !m_TexImages[TEX_NEWTS].Load("newTS.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load newTS.bmp.  Aborting.");
        return false;
    }


	//dots1.
    if ( !m_TexImages[TEX_DOTS1].Load("dots1.bmp", 1.0f) )
    {
        WriteToLog("\n(ERROR) Failed to load dots1.bmp.  Aborting.");
        return false;
    }


	
	//////////////////////////generated textures/////////////////////////

	//Stripes
	D3DCOLOR pdwColors[] = {
        D3DRGBA(1, 1, 1, 1),	//white
        D3DRGBA(1, 0, 0, 1),	//red
        D3DRGBA(0, 1, 0, 1),	//green
        D3DRGBA(0, 0, 1, 1),	//blue
		D3DRGBA(1, 1, 0, 1),	//yellow
		D3DRGBA(0, 0, 0, 1),	//black
        D3DRGBA(0, 1, 1, 1),	//Brown
	    D3DRGBA(1, 0, 1, 1)
    };
	if ( !m_TexImages[TEX_STRIPES].LoadStripes(LARGE_TEXWIDTH, LARGE_TEXHEIGHT, 4, pdwColors, true, true) )
    {
        WriteToLog("(ERROR) Failed to load TEX_STRIPES.  Aborting.\n");
        return false;
    }

/*
	//Stripes for D3DTA_CONSTANT test.
	//The idea is to use the color channels and rendering space to magnify 
	//	error as much as possible.
    pdwColors[0] = D3DRGBA(0, 0.5f, 0.5f, 0.5f);
    pdwColors[1] = D3DRGBA(1, 0.5f, 0.5f, 0.5f);
    pdwColors[2] = D3DRGBA(0.5f, 0, 0.5f, 0.5f);
    pdwColors[3] = D3DRGBA(0.5f, 1, 0.5f, 0.5f);
    pdwColors[4] = D3DRGBA(0.5f, 0.5f, 0, 0.5f);
    pdwColors[5] = D3DRGBA(0.5f, 0.5f, 1, 0.5f);
    pdwColors[6] = D3DRGBA(0.5f, 0.5f, 0.5f, 0);
    pdwColors[7] = D3DRGBA(0.5f, 0.5f, 0.5f, 1);
	if ( !m_TexImages[TEX_STRIPES_FOR_CONST].LoadStripes(LARGE_TEXWIDTH, LARGE_TEXHEIGHT, 8, pdwColors, false) )
    {
        WriteToLog("(ERROR) Failed to load TEX_STRIPES_FOR_CONST.  Aborting.\n");
        return false;
    }
*/

	// Gradient BLUE_WHITE
	DWORD dwGradColors[4] =
    {
        D3DRGBA(0, 0, 1, 1),
        D3DRGBA(0, 0, 1, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
    if ( !m_TexImages[TEX_GRADIENT_BLUE_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_BLUE_WHITE gradient data.  Aborting.\n");
        return false;
    }

	// Gradient BLACK_WHITE
    dwGradColors[0] = D3DRGBA(0, 0, 0, 0);
    dwGradColors[1] = D3DRGBA(0, 0, 0, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_BLACK_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_BLACK_WHITE gradient data.  Aborting.\n");
        return false;
    }

	// Gradient ALPHA
    dwGradColors[0] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[1] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_ALPHA].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_ALPHA gradient data.  Aborting.\n");
        return false;
    }

	// Gradient ALPHA 2
    dwGradColors[0] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[1] = D3DRGBA(1, 1, 0, 0);
    dwGradColors[2] = D3DRGBA(0, 1, 1, 1);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 0);
    if ( !m_TexImages[TEX_GRADIENT_ALPHA2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_ALPHA2 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_HALF_WHITE_HALF_ALPHA
    dwGradColors[0] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[1] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[2] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    dwGradColors[3] = D3DRGBA(0.5, 0.5, 0.5, 0.5);
    if ( !m_TexImages[TEX_HALF_WHITE_HALF_ALPHA].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_HALF_WHITE_HALF_ALPHA gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FIRE_WHITE
    dwGradColors[0] = D3DRGBA(0.0, 0.0, 0.0, 0.0);
    dwGradColors[1] = D3DRGBA(0.0, 0.0, 0.0, 0.0);
    dwGradColors[2] = D3DRGBA(0.99, 0.72, 0.15, 1.0);
    dwGradColors[3] = D3DRGBA(0.99, 0.72, 0.15, 1.0);
    if ( !m_TexImages[TEX_GRADIENT_FIRE_WHITE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FIRE_WHITE gradient data.  Aborting.\n");
        return false;
    }

	//TEX_00FFFFFF
    dwGradColors[0] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[1] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[2] = D3DRGBA(1, 1, 1, 0);
    dwGradColors[3] = D3DRGBA(1, 1, 1, 0);
    if ( !m_TexImages[TEX_00FFFFFF].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_00FFFFFF gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FOR_BUMP
    dwGradColors[0] = D3DRGBA( 0, 0, 0, 1);
    dwGradColors[1] = D3DRGBA(0.117, 1.0, 0.55, 1);
    dwGradColors[2] = D3DRGBA(0.08, 1.0, 0.38, 1);
    dwGradColors[3] = D3DRGBA(0, 1, 0, 1);
    if ( !m_TexImages[TEX_GRADIENT_FOR_BUMP].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FOR_BUMP gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FOR_BUMP2
    dwGradColors[0] = D3DRGBA( 0, 0, 0, 1);
    dwGradColors[1] = D3DRGBA(1, 0, 0, 1);
    dwGradColors[2] = D3DRGBA(0, 1, 0, 1);
    dwGradColors[3] = D3DRGBA(0, 0, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_FOR_BUMP2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FOR_BUMP2 gradient data.  Aborting.\n");
        return false;
    }

	////////////////////////CImage for Tex Mips<Cube,Vol///////////////

	//TEX_MIPS
    dwGradColors[0] = D3DRGBA( 1, 0, 0, 0.75f);
    dwGradColors[1] = D3DRGBA( 1, 0, 0, 0.75f);
    dwGradColors[2] = D3DRGBA( 1, 0, 0, 1);
    dwGradColors[3] = D3DRGBA( 1, 0, 0, 1);
    if ( !m_TexImages[TEX_MIPS].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPS gradient data.  Aborting.\n");
        return false;
    }

	//TEX_VOLUME
    dwGradColors[0] = D3DRGBA( 1, 1, 1, 1);
    dwGradColors[1] = D3DRGBA( 1, 1, 1, 1);
    dwGradColors[2] = D3DRGBA( 1, 1, 1, 0.75f);
    dwGradColors[3] = D3DRGBA( 1, 1, 1, 0.75f);
    if ( !m_TexImages[TEX_VOLUME].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_VOLUME gradient data.  Aborting.\n");
        return false;
    }

	//TEX_CUBE
    dwGradColors[0] = D3DRGBA( 1, 1, 0, 0.75f);
    dwGradColors[1] = D3DRGBA( 1, 1, 0, 0.75f);
    dwGradColors[2] = D3DRGBA( 1, 1, 0, 1);
    dwGradColors[3] = D3DRGBA( 1, 1, 0, 1);
    if ( !m_TexImages[TEX_CUBE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_CUBE gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_1
    dwGradColors[0] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[1] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[2] = D3DRGBA(1, 0, 1, 0.75f);
    dwGradColors[3] = D3DRGBA(1, 0, 1, 0.75f);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_1].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_1 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_2
    dwGradColors[0] = D3DRGBA(1, 0.1176f, 0.55f, 0.75f);
    dwGradColors[1] = D3DRGBA(1, 0.1176f, 0.55f, 0.75f);
    dwGradColors[2] = D3DRGBA(1, 0.1176f, 0.55f, 1);
    dwGradColors[3] = D3DRGBA(1, 0.1176f, 0.55f, 1);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_2 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_3
    dwGradColors[0] = D3DRGBA( 0, 1, 1, 1);
    dwGradColors[1] = D3DRGBA( 0, 1, 1, 1);
    dwGradColors[2] = D3DRGBA( 0, 1, 1, 0.75f);
    dwGradColors[3] = D3DRGBA( 0, 1, 1, 0.75f);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_3].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_3 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_4
    dwGradColors[0] = D3DRGBA( 0, 0, 1, 0.75f);
    dwGradColors[1] = D3DRGBA( 0, 0, 1, 0.75f);
    dwGradColors[2] = D3DRGBA( 0, 0, 1, 1);
    dwGradColors[3] = D3DRGBA( 0, 0, 1, 1);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_4].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_4 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_5
    dwGradColors[0] = D3DRGBA( 0, 1, 0, 1);
    dwGradColors[1] = D3DRGBA( 0, 1, 0, 1);
    dwGradColors[2] = D3DRGBA( 0, 1, 0, 0.75f);
    dwGradColors[3] = D3DRGBA( 0, 1, 0, 0.75f);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_5].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_5 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_6
    dwGradColors[0] = D3DRGBA( 0.5f, 0.5f, 0.5f, 0.75f);
    dwGradColors[1] = D3DRGBA( 0.5f, 0.5f, 0.5f, 0.75f);
    dwGradColors[2] = D3DRGBA( 0.5f, 0.5f, 0.5f, 1);
    dwGradColors[3] = D3DRGBA( 0.5f, 0.5f, 0.5f, 1);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_6].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_6 gradient data.  Aborting.\n");
        return false;
    }

	
	return true;
}




UINT CBaseTSSTest::TestInitialize()
{
	//initialize hardware caps flags.
	m_dwPrimitiveMiscCaps = ~0;
	m_dwRasterCaps = ~0;
	m_dwTextureCaps = ~0;
	m_dwTextureOpCaps = ~0;
	m_dwTextureStageCaps = ~0;
	m_wMaxSimultaneousTextures = MAX_TEX_STAGES;
	
	/////////////////////////////////////////////////////////////////////////////
	////////Create texture format cycles/////////////////////////////////////////

	m_VariousTexFormatCycles[CYC_DONT_CARE].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_DONT_CARE].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_PREVIOUS_STAGE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_PREVIOUS_STAGE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_ONE_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_ONE_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_TWO_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_TWO_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_THREE_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_THREE_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_FOUR_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_FOUR_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_FIVE_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_FIVE_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_SIX_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_SIX_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;

	m_VariousTexFormatCycles[CYC_SEVEN_STAGE_ABOVE_DEPENDENT].pFormats = NULL;
	m_VariousTexFormatCycles[CYC_SEVEN_STAGE_ABOVE_DEPENDENT].uNumFormats = 1;



	SAFEDELETEA(m_VariousTexFormatCycles[CYC_ALL].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_RGB | PF_DXTN | PF_YUV | PF_LUMINANCE | PF_ALPHA, 0, &(m_VariousTexFormatCycles[CYC_ALL].pFormats), &(m_VariousTexFormatCycles[CYC_ALL].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_ALL].uNumFormats = 0;
		WriteToLog("\nNo texture formats [CYC_ALL] found.");
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_ALL].pFormats);
		return D3DTESTINIT_ABORT;
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_RGB].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_RGB, 0, &(m_VariousTexFormatCycles[CYC_RGB].pFormats), &(m_VariousTexFormatCycles[CYC_RGB].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_RGB].uNumFormats = 0;
		WriteToLog("\nNo texture formats [CYC_RGB] found.");
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_RGB].pFormats);
		return D3DTESTINIT_ABORT;
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_ALPHA].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_ALPHA | PF_ALPHAPIXELS, 0, &(m_VariousTexFormatCycles[CYC_ALPHA].pFormats), &(m_VariousTexFormatCycles[CYC_ALPHA].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_ALPHA].uNumFormats = 0;
		WriteToLog("\nNo texture formats [CYC_ALPHA] found.");
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_ALPHA].pFormats);
		return D3DTESTINIT_ABORT;
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_BUMPDUDV].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_BUMPDUDV, 0, &(m_VariousTexFormatCycles[CYC_BUMPDUDV].pFormats), &(m_VariousTexFormatCycles[CYC_BUMPDUDV].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_BUMPDUDV].uNumFormats = 0;
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_BUMPDUDV].pFormats);
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_LUMINANCE].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_LUMINANCE, 0, &(m_VariousTexFormatCycles[CYC_LUMINANCE].pFormats), &(m_VariousTexFormatCycles[CYC_LUMINANCE].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_LUMINANCE].uNumFormats = 0;
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_LUMINANCE].pFormats);
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_BUMPLUMINANCE].pFormats);
	if(!FilterFormats(m_pCommonTextureFormats, PF_BUMPLUMINANCE, 0, &(m_VariousTexFormatCycles[CYC_BUMPLUMINANCE].pFormats), &(m_VariousTexFormatCycles[CYC_BUMPLUMINANCE].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_BUMPLUMINANCE].uNumFormats = 0;
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_BUMPLUMINANCE].pFormats);
	}


	m_VariousTexFormatCycles[CYC_VOLUME].pFormats = m_pCommonVolumeFormats;
	m_VariousTexFormatCycles[CYC_VOLUME].uNumFormats = m_uCommonVolumeFormats;


	m_VariousTexFormatCycles[CYC_CUBE].pFormats = m_pCommonCubeFormats;
	m_VariousTexFormatCycles[CYC_CUBE].uNumFormats = m_uCommonCubeFormats;


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_VOLUME_RGB].pFormats);
	if(NULL == m_pCommonVolumeFormats)
		m_VariousTexFormatCycles[CYC_VOLUME_RGB].uNumFormats = 0;
	else if(!FilterFormats(m_pCommonVolumeFormats, PF_RGB, 0, &(m_VariousTexFormatCycles[CYC_VOLUME_RGB].pFormats), &(m_VariousTexFormatCycles[CYC_VOLUME_RGB].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_VOLUME_RGB].uNumFormats = 0;
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_VOLUME_RGB].pFormats);
	}


	SAFEDELETEA(m_VariousTexFormatCycles[CYC_CUBE_RGB].pFormats);
	if(NULL == m_pCommonCubeFormats)
		m_VariousTexFormatCycles[CYC_CUBE_RGB].uNumFormats = 0;
	else if(!FilterFormats(m_pCommonCubeFormats, PF_RGB, 0, &(m_VariousTexFormatCycles[CYC_CUBE_RGB].pFormats), &(m_VariousTexFormatCycles[CYC_CUBE_RGB].uNumFormats)))
	{		            
		m_VariousTexFormatCycles[CYC_CUBE_RGB].uNumFormats = 0;
		SAFEDELETEA(m_VariousTexFormatCycles[CYC_CUBE_RGB].pFormats);
	}
    return D3DTESTINIT_RUN;
}

bool CBaseTSSTest::Setup()
{
	//the primitives//////////////////////////////////////////////

	VERTEX_DST2 local_4VrtxDST2[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
		{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
		{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 },
		{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
	};
	memcpy(m_4VrtxDST2, local_4VrtxDST2, sizeof(m_4VrtxDST2));

	VERTEX_DST2 local_4VrtxDST2_ZeroDiffuse[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(0, 0, 0, 0), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
		{ 0, 1, 0, 1, D3DRGBA(0, 0, 0, 0), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
		{ 1, 0, 0, 1, D3DRGBA(0, 0, 0, 0), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 },
		{ 1, 1, 0, 1, D3DRGBA(0, 0, 0, 0), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
	};
	memcpy(m_4VrtxDST2_ZeroDiffuse, local_4VrtxDST2_ZeroDiffuse, sizeof(m_4VrtxDST2_ZeroDiffuse));

	VERTEX_DST2 local_4VrtxDST2_HalfDiffuse[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(0.5, 0.5, 0.5, 0.5), D3DRGBA(0, 0, 0, 1), 0.0, 0.0 },
		{ 0, 1, 0, 1, D3DRGBA(0.5, 0.5, 0.5, 0.5), D3DRGBA(0, 0, 0, 1), 0.0, 1.0 },
		{ 1, 0, 0, 1, D3DRGBA(0.5, 0.5, 0.5, 0.5), D3DRGBA(1, 1, 1, 1), 1.0, 0.0 },
		{ 1, 1, 0, 1, D3DRGBA(0.5, 0.5, 0.5, 0.5), D3DRGBA(1, 1, 1, 1), 1.0, 1.0 },
	};
	memcpy(m_4VrtxDST2_HalfDiffuse, local_4VrtxDST2_HalfDiffuse, sizeof(m_4VrtxDST2_HalfDiffuse));

	VERTEX_DST2 local_4VrtxDST2_DiffuseAlphaGradient[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(1.0, 1.0, 1.0, 0.0), D3DRGBA(0.5f, 0.5f, 0.5f, 0.5f), 0.0, 0.0 },
		{ 0, 1, 0, 1, D3DRGBA(1.0, 1.0, 0.0, 0.0), D3DRGBA(0.5f, 0.5f, 0.5f, 0.5f), 0.0, 1.0 },
		{ 1, 0, 0, 1, D3DRGBA(0.0, 1.0, 1.0, 1.0), D3DRGBA(0.5f, 0.5f, 0.5f, 0.5f), 1.0, 0.0 },
		{ 1, 1, 0, 1, D3DRGBA(1.0, 0.0, 1.0, 1.0), D3DRGBA(0.5f, 0.5f, 0.5f, 0.5f), 1.0, 1.0 },
	};
	memcpy(m_4VrtxDST2_DiffuseAlphaGradient, local_4VrtxDST2_DiffuseAlphaGradient, sizeof(m_4VrtxDST2_DiffuseAlphaGradient));

	VERTEX_DST2T2 local_4VrtxDST2T2[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 0, 0, 0), 0.0, 0.0,   1.0, 1.0 },
		{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(0, 0, 0, 0), 0.0, 1.0,   1.0, 0.0 },
		{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 1, 0.8), 1.0, 0.0,   0.0, 1.0 },
		{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 0.8), 1.0, 1.0,   0.0, 0.0 },
	};
	memcpy(m_4VrtxDST2T2, local_4VrtxDST2T2, sizeof(m_4VrtxDST2T2));

	VERTEX_DST2T3T3 local_4VrtxDST2T3T3_forMipMap[4] =
	{  //0.0833 is 1/(2*MIP_LEVELS). This ensured equal area for each MIP_LEVELS textures.
		{ 0, 0, 0, 0.0833f,	D3DRGBA(1, 1, 1, 0), D3DRGBA(1, 1, 1, 0), 0.0, 0.0,   0.0, 0.0, 0.0,   0.0, 0.5, 0.5, },
		{ 0, 1, 1, 1.0f,	D3DRGBA(1, 1, 1, 0), D3DRGBA(1, 1, 0, 0), 0.0, 1.0,   0.0, 0.0, 0.0,   -0.5, -0.5, 0.0, },
		{ 1, 0, 0, 0.0833f,	D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 0, 1, 1), 1.0, 0.0,   1.0, 1.0, 1.0,   0.5, 0.0, -0.5, },
		{ 1, 1, 1, 0.0833f,	D3DRGBA(1, 1, 1, 1), D3DRGBA(0, 1, 1, 1), 1.0, 1.0,   1.0, 1.0, 1.0,   0.0, 0.5, 0.5, },
	};
	memcpy(m_4VrtxDST2T3T3_forMipMap, local_4VrtxDST2T3T3_forMipMap, sizeof(m_4VrtxDST2T3T3_forMipMap));

	VERTEX_DST2T3T3 local_4VrtxDST2T3T3_forVolCube[4] =
	{  //0.0833 is 1/(2*MIP_LEVELS). This ensured equal area for each MIP_LEVELS textures.
		{ 0, 0, 0, 1,	D3DRGBA(1, 1, 1, 0), D3DRGBA(1, 1, 1, 0), 0.0, 0.0,   0.0, 0.0, 0.0,   0.0, 0.5, 0.5, },
		{ 0, 1, 1, 1,	D3DRGBA(1, 1, 1, 0), D3DRGBA(1, 1, 0, 0), 0.0, 4.0,   0.0, 0.0, 0.0,   -0.5, -0.5, 0.0, },
		{ 1, 0, 0, 1,	D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 0, 1, 1), 4.0, 0.0,   1.0, 1.0, 1.0,   0.5, 0.0, -0.5, },
		{ 1, 1, 1, 1,	D3DRGBA(1, 1, 1, 1), D3DRGBA(0, 1, 1, 1), 4.0, 4.0,   1.0, 1.0, 1.0,   0.0, 0.5, 0.5, },
	};
	memcpy(m_4VrtxDST2T3T3_forVolCube, local_4VrtxDST2T3T3_forVolCube, sizeof(m_4VrtxDST2T3T3_forVolCube));

	VERTEX_DST2T2 local_4VrtxDST2T2half[4] =
	{
		{ 0, 0, 0, 1, D3DRGBA(0, 1, 0, 1), D3DRGBA(0, 1, 1, 1), 0.0, 0.0,   0.5f, 0.5f },
		{ 0, 1, 0, 1, D3DRGBA(1, 0, 0, 1), D3DRGBA(1, 0, 1, 1), 0.0, 1.0,   0.5f, 0.0 },
		{ 1, 0, 0, 1, D3DRGBA(0, 0, 1, 1), D3DRGBA(1, 1, 0, 0), 1.0, 0.0,   0.0, 0.5f },
		{ 1, 1, 0, 1, D3DRGBA(1, 1, 1, 1), D3DRGBA(1, 1, 1, 0), 1.0, 1.0,   0.0, 0.0 },
	};
	memcpy(m_4VrtxDST2T2half, local_4VrtxDST2T2half, sizeof(m_4VrtxDST2T2half));


	m_IndxTriList[0] = 0;
	m_IndxTriList[1] = 1;
	m_IndxTriList[2] = 2;
	m_IndxTriList[3] = 1;
	m_IndxTriList[4] = 2;
	m_IndxTriList[5] = 3;

	m_IndxTriStrip[0] = 0;
	m_IndxTriStrip[1] = 1;
	m_IndxTriStrip[2] = 2;
	m_IndxTriStrip[3] = 3;

	m_IndxTriFan[0] = 2;
	m_IndxTriFan[1] = 0;
	m_IndxTriFan[2] = 1;
	m_IndxTriFan[3] = 3;


	//end of the primitives//////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	////////CreateVertexBuffers and load them.///////////////////////////////////
   
	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2), 0, VERTEX_DST2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2."));
		return false;
	}
	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2_HalfDiffuse), 0, VERTEX_DST2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2_HalfDiffuse))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2_HalfDiffuse."));
		return false;
	}
	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2_ZeroDiffuse), 0, VERTEX_DST2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2_ZeroDiffuse))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2_ZeroDiffuse."));
		return false;
	}
	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2_DiffuseAlphaGradient), 0, VERTEX_DST2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2_DiffuseAlphaGradient))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2_DiffuseAlphaGradient."));
		return false;
	}
	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2T2), 0, VERTEX_DST2T2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2T2))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2T2."));
		return false;
	}
	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2T2half), 0, VERTEX_DST2T2_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2T2half))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2T2half."));
		return false;
	}


	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2T3T3_forVolCube), 0, VERTEX_DST2T3T3_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2T3T3_forVolCube))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2T3T3_forVolCube."));
		return false;
	}

	////////////////////////////

	if (!CreateVertexBuffer(sizeof(m_4VrtxDST2T3T3_forMipMap), 0, VERTEX_DST2T3T3_FVF, FMWK_DEFAULT, &m_pVB4VrtxDST2T3T3_forMipMap))
	{
		WriteToLog(_T("\nCreateVertexBuffer failed for m_pVB4VrtxDST2T3T3_forMipMap."));
		return false;
	}
    

   

	/////////////////////////////////////////////////////////////////////////////
	////////Dynamically resize primitives////////////////////////////////////////

	DWORD dwDisplayWidth = m_pCurrentMode->nBackBufferWidth;
	DWORD dwDisplayHeight = m_pCurrentMode->nBackBufferHeight;        

	for (int i = 0; i < 4; i++)
	{
		m_4VrtxDST2[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2_HalfDiffuse[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2_HalfDiffuse[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2_HalfDiffuse[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2_HalfDiffuse[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2_ZeroDiffuse[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2_ZeroDiffuse[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2_ZeroDiffuse[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2_ZeroDiffuse[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2_DiffuseAlphaGradient[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2_DiffuseAlphaGradient[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2_DiffuseAlphaGradient[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2_DiffuseAlphaGradient[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2T2[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2T2[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2T2[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2T2[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2T2half[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2T2half[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2T2half[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2T2half[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2T3T3_forVolCube[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2T3T3_forVolCube[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2T3T3_forVolCube[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2T3T3_forVolCube[i].y -= OFFSET_FOR_TEXTURE;

		m_4VrtxDST2T3T3_forMipMap[i].x *= (float) dwDisplayWidth;
		m_4VrtxDST2T3T3_forMipMap[i].y *= (float) dwDisplayHeight;
        m_4VrtxDST2T3T3_forMipMap[i].x -= OFFSET_FOR_TEXTURE;
        m_4VrtxDST2T3T3_forMipMap[i].y -= OFFSET_FOR_TEXTURE;

		

	}


	/////////////////////////////////////////////////////////////////////////////
	//////// Load vertex buffers. ///////////////////////////////////////////////
	BYTE *pVBLocked = NULL;

    if ( m_pVB4VrtxDST2->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2, sizeof(m_4VrtxDST2));
        m_pVB4VrtxDST2->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2.");
        RELEASE(m_pVB4VrtxDST2);
		return false;
    }
    
    if ( m_pVB4VrtxDST2_HalfDiffuse->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2_HalfDiffuse, sizeof(m_4VrtxDST2_HalfDiffuse));
        m_pVB4VrtxDST2_HalfDiffuse->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2_HalfDiffuse.");
        RELEASE(m_pVB4VrtxDST2_HalfDiffuse);
		return false;
    }
    
    if ( m_pVB4VrtxDST2_ZeroDiffuse->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2_ZeroDiffuse, sizeof(m_4VrtxDST2_ZeroDiffuse));
        m_pVB4VrtxDST2_ZeroDiffuse->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2_ZeroDiffuse.");
        RELEASE(m_pVB4VrtxDST2_ZeroDiffuse);
		return false;
    }
    
    if ( m_pVB4VrtxDST2_DiffuseAlphaGradient->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2_DiffuseAlphaGradient, sizeof(m_4VrtxDST2_DiffuseAlphaGradient));
        m_pVB4VrtxDST2_DiffuseAlphaGradient->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2_DiffuseAlphaGradient.");
        RELEASE(m_pVB4VrtxDST2_DiffuseAlphaGradient);
		return false;
    }

    if ( m_pVB4VrtxDST2T2->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2T2, sizeof(m_4VrtxDST2T2));
        m_pVB4VrtxDST2T2->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2T2.");
        RELEASE(m_pVB4VrtxDST2T2);
		return false;
    }

    if ( m_pVB4VrtxDST2T2half->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2T2half, sizeof(m_4VrtxDST2T2half));
        m_pVB4VrtxDST2T2half->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2T2half.");
        RELEASE(m_pVB4VrtxDST2T2half);
		return false;
    }

    if ( m_pVB4VrtxDST2T3T3_forVolCube->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2T3T3_forVolCube, sizeof(m_4VrtxDST2T3T3_forVolCube));
        m_pVB4VrtxDST2T3T3_forVolCube->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2T3T3_forVolCube.");
        RELEASE(m_pVB4VrtxDST2T3T3_forVolCube);
		return false;
    }

    if ( m_pVB4VrtxDST2T3T3_forMipMap->Lock(0, 0, &pVBLocked, LOCK_NOSYSLOCK) )
    {
        memcpy(pVBLocked, (BYTE*)m_4VrtxDST2T3T3_forMipMap, sizeof(m_4VrtxDST2T3T3_forMipMap));
        m_pVB4VrtxDST2T3T3_forMipMap->Unlock();
        pVBLocked = NULL;
    }
    else
    {
        WriteToLog("\n(ERROR) Failed to lock vertex buffer m_pVB4VrtxDST2T3T3_forMipMap.");
        RELEASE(m_pVB4VrtxDST2T3T3_forMipMap);
		return false;
    }


	/////////////////////////////////////////////////////////////////////////////
	////////Generate CImages for textures.///////////////////////////////////////
	if (!GenerateTextureImages())
	{
		WriteToLog(_T("\nGenerateTextureImages failed."));
		return false;
	}



    return true;
}

void CBaseTSSTest::Cleanup()
{
	//Destroy VertexBuffers created.
    RELEASE(m_pVB4VrtxDST2);
    RELEASE(m_pVB4VrtxDST2_HalfDiffuse);
    RELEASE(m_pVB4VrtxDST2_ZeroDiffuse);
    RELEASE(m_pVB4VrtxDST2_DiffuseAlphaGradient);
    RELEASE(m_pVB4VrtxDST2T2);
    RELEASE(m_pVB4VrtxDST2T2half);
    RELEASE(m_pVB4VrtxDST2T3T3_forMipMap);
    RELEASE(m_pVB4VrtxDST2T3T3_forVolCube);
}

bool CBaseTSSTest::TestTerminate()
{
	////////Destroy texture format cycles/////////////////////////////////////////
	for (int i=0; i<CYC_LAST_PLACEHOLDER_TO_FREE; i++)
	{
		SAFEDELETEA(m_VariousTexFormatCycles[i].pFormats);
	}

	SAFEDELETEA(m_VariousTexFormatCycles[CYC_VOLUME_RGB].pFormats);
	SAFEDELETEA(m_VariousTexFormatCycles[CYC_CUBE_RGB].pFormats);


	//Texture CImages are not dynamically created.
	//So we need not destroy it.

	return CD3DTest::TestTerminate();
}


void CBaseTSSTest::UpdateStatus()
{
	//TCHAR sztVersion[LINE_LENGTH];

    CD3DTest::UpdateStatus();
    
    for( int i = 0; i < MAX_TEX_STAGES; ++i )
    {
		if( m_pStageFormat[i] )
		{
			static TCHAR szBuffer[80];

			FmtToString( FORMAT_TEXTURE, m_pStageFormat[i], szBuffer );
			WriteStatus( "$wFormat", "$cStage %i = %s", i, szBuffer );
		}
    }

/*  if ( m_bReportStatus )
    {
        if (0 == m_vShaders.size())
		    return;      
        WriteStatus("$gName:\t$y", m_vShaders[m_dwCurrentShader].pName);

	    wsprintf(sztVersion, "$y%d.%d", D3DSHADER_VERSION_MAJOR(m_vShaders[m_dwCurrentShader].dwRequired), 
									    D3DSHADER_VERSION_MINOR(m_vShaders[m_dwCurrentShader].dwRequired));	
	    WriteStatus("$gPSVersion:", sztVersion);
    }
*/  return;
}



bool CBaseTSSTest::DrawPrimitiveID(PrimitiveID primID)
{

    if (BeginScene())
    {

		switch(primID)
		{
		case TRI_LIST_DST2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST,m_pVB4VrtxDST2,0,4,m_IndxTriList,TRI_LIST_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_LIST_DST2");
				return false;
			}
			break;
		case TRI_STRIP_DST2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2");
				return false;
			}
			break;
		case TRI_FAN_DST2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLEFAN,m_pVB4VrtxDST2,0,4,m_IndxTriFan,TRI_FAN_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_FAN_DST2");
				return false;
			}
			break;

	//////////////////////////////////////////////////
			
		case TRI_STRIP_DST2_HalfDiffuse:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2_HalfDiffuse,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2_HalfDiffuse");
				return false;
			}
			break;

	//////////////////////////////////////////////////
			
		case TRI_STRIP_DST2_ZeroDiffuse:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2_ZeroDiffuse,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2_ZeroDiffuse");
				return false;
			}
			break;

	//////////////////////////////////////////////////
			
		case TRI_STRIP_DST2_DiffuseAlphaGradient:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2_DiffuseAlphaGradient,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2_DiffuseAlphaGradient");
				return false;
			}
			break;

	//////////////////////////////////////////////////
		case TRI_LIST_DST2T2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST,m_pVB4VrtxDST2T2,0,4,m_IndxTriList,TRI_LIST_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_LIST_DST2T2");
				return false;
			}
			break;
		case TRI_STRIP_DST2T2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2T2,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2T2");
				return false;
			}
			break;
		case TRI_FAN_DST2T2:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLEFAN,m_pVB4VrtxDST2T2,0,4,m_IndxTriFan,TRI_FAN_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_FAN_DST2T2");
				return false;
			}
			break;
	//////////////////////////////////////////////////
		case TRI_LIST_DST2T2half:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST,m_pVB4VrtxDST2T2half,0,4,m_IndxTriList,TRI_LIST_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_LIST_DST2T2half");
				return false;
			}
			break;
		case TRI_STRIP_DST2T2half:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2T2half,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2T2half");
				return false;
			}
			break;
		case TRI_FAN_DST2T2half:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLEFAN,m_pVB4VrtxDST2T2half,0,4,m_IndxTriFan,TRI_FAN_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_FAN_DST2T2half");
				return false;
			}
			break;
	//////////////////////////////////////////////////
		case TRI_LIST_DST2T3T3_forVolCube:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST,m_pVB4VrtxDST2T3T3_forVolCube,0,4,m_IndxTriList,TRI_LIST_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_LIST_DST2T3T3_forVolCube");
				return false;
			}
			break;
		case TRI_STRIP_DST2T3T3_forVolCube:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2T3T3_forVolCube,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2T3T3_forVolCube");
				return false;
			}
			break;
		case TRI_FAN_DST2T3T3_forVolCube:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLEFAN,m_pVB4VrtxDST2T3T3_forVolCube,0,4,m_IndxTriFan,TRI_FAN_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_FAN_DST2T3T3_forVolCube");
				return false;
			}
			break;
		

	//////////////////////////////////////////////////
		case TRI_LIST_DST2T3T3_forMipMap:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLELIST,m_pVB4VrtxDST2T3T3_forMipMap,0,4,m_IndxTriList,TRI_LIST_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_LIST_DST2T3T3_forMipMap");
				return false;
			}
			break;
		case TRI_STRIP_DST2T3T3_forMipMap:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP,m_pVB4VrtxDST2T3T3_forMipMap,0,4,m_IndxTriStrip,TRI_STRIP_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_STRIP_DST2T3T3_forMipMap");
				return false;
			}
			break;
		case TRI_FAN_DST2T3T3_forMipMap:
			if (!DrawPrimitiveVertexBuffer(D3DPT_TRIANGLEFAN,m_pVB4VrtxDST2T3T3_forMipMap,0,4,m_IndxTriFan,TRI_FAN_INDX_COUNT, 0))
			{
				WriteToLog("\nDrawPrimitiveVertexBuffer() failed for TRI_FAN_DST2T3T3_forMipMap");
				return false;
			}
			break;
		



		default:
			break;
		}

        if (!EndScene())
        {
            WriteToLog("\n(ERROR) EndScene() failed: (%08X)", GetLastError());
            Fail();
			return false;
        }
    }
    else
    {
        WriteToLog("\n(ERROR) BeginScene() failed: (%08X)", GetLastError());
        Fail();
		return false;
    }

	return true;
}



bool CBaseTSSTest::PreModeSetup()
{
	return true;
}



// NAME:        ProcessArgs()
// DESC:        process test arguments
// INPUT:       none
// OUTPUT:      none

void CBaseTSSTest::ProcessArgs(void)
{
/*    TCHAR sztPSVersion[BUFFER_LENGTH], *ptcTemp;
	int nMajorVersion, nMinorVersion;

	m_bSpecificVerRequest = false;
	m_dwRequestedVersion = PSVERSION(0, 0);

    if (KeySet("PREVIEW"))
    {
        m_Flags.Set(BETAPREVIEW);
    }
	if (KeySet("WHQL"))
    {		
        return;
    }
    if (KeySet("NOCHECKVER"))
    {
        m_Flags.Set(NOCHECKVER);
    }
	//Get a version if it is specified
	ReadString(_T("Ver"), _T("0.0"), sztPSVersion, ARRAYSIZE(sztPSVersion));
	nMajorVersion = atoi(sztPSVersion);
	ptcTemp = _tcsstr(sztPSVersion, _T("."));
	nMinorVersion = atoi(&ptcTemp[1]);
	m_dwRequestedVersion = PSVERSION( nMajorVersion, nMinorVersion );

	if ( PSVERSION(0, 0) == m_dwRequestedVersion )
	{
		ReadString(_T("VerSpec"), _T("0.0"), sztPSVersion, ARRAYSIZE(sztPSVersion));
		nMajorVersion = atoi(sztPSVersion);
		ptcTemp = _tcsstr(sztPSVersion, _T("."));
		nMinorVersion = atoi(&ptcTemp[1]);
		m_dwRequestedVersion = PSVERSION( nMajorVersion, nMinorVersion );
		if ( m_dwRequestedVersion != PSVERSION(0, 0) )
			m_bSpecificVerRequest = true;
	}
*/	return;
}




