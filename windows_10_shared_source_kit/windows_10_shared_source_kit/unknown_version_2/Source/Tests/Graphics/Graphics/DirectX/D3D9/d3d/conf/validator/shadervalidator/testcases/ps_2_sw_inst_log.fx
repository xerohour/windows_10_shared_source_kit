rs[1] = D3DRGBA( 0, 1, 1, 1);
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




//
// vs.vsa
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

struct VSIn
{
	float3 pos : pos;
	float3 col : col;
};

struct VSOut
{
	float4 pos : SV_Position;
	float4 col : col;
};

cbuffer cb0 : register(b0)
{
	float4 inverseViewport;
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.pos = float4( 0, 0, 0, 1.0 );
	output.pos.z = input.pos.z;
	output.pos.xy = input.pos.xy * inverseViewport.zw + inverseViewport.xy;
	
	output.col = float4( 0, 0, 0, 1.0 );
	output.col.xyz = input.col;	
	
	return output;
}
// ***************************************************************
//  GSConf   version:  1.0   ·  date: 02/22/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include <D3D11Test.h>

//////////////////////////////////////////////////////////////////////////

enum GS_INPUT_PRIM_TYPE
{
	PRIM_TYPE_POINT,
	PRIM_TYPE_LINE,
	PRIM_TYPE_LINE_ADJ,
	PRIM_TYPE_TRIANGLE,
	PRIM_TYPE_TRIANGLE_ADJ
};

struct CONST_DATA
{
	float vpSize[4];
};

TEST_RESULT TRLatch(TEST_RESULT& origVal, TEST_RESULT& newVal);

#define MEMBER_OFFSET( member, instance ) \
	(int) ( ptrdiff_t( &(instance).member ) - ptrdiff_t( &(instance) ) )

//////////////////////////////////////////////////////////////////////////

class CGSConfTest : public CD3D11Test
{
protected:
	ID3D11InputLayout *m_pInputLayout;
	ID3D11VertexShader *m_pVS;
	ID3D11GeometryShader *m_pGS;
	ID3D11PixelShader *m_pPS;
	ID3D11Buffer *m_pSOTarget;
	ID3D11Buffer *m_pVB;
	ID3D11Buffer *m_pCB;
	ID3D11RasterizerState *m_pRS;
	ID3D11DepthStencilState *m_pDSS;
	ID3D11Texture2D *m_pMemTexture;
	
public:
	CGSConfTest();
	
	int GetNumRenderWindows() const;
	bool CreateRTFromSwapChainProps() const { return true; }
	TEST_RESULT CommonSetup();
	void CommonCleanup();
	
//	TEST_RESULT SetupRenderTarget( UINT w, UINT h, UINT slices );

	TEST_RESULT SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_INPUT_ELEMENT_DESC *iedesc, UINT numInputElements, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11GeometryShader **ppGS, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupGSWithSOFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY decl[], UINT numElements, UINT outputStreamStride, ID3D11GeometryShader **ppGS, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS );
};

//////////////////////////////////////////////////////////////////////////
// CGSConfTestApp
//////////////////////////////////////////////////////////////////////////

class CGSConfTestApp : public CD3D11TestFramework
{
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	void InitOptionVariables();
	bool InitTestGroups();
};

extern CGSConfTestApp g_App;
#include "Test_Include.fx"

int Test_Count = 345;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_sw_inst_log";

PS_CRITERIA PS_001_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_sw : log source reg combination v0.r is allowed";
string PS_001 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_sw : log source reg combination v0.g is allowed";
string PS_002 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_sw : log source reg combination v0.b is allowed";
string PS_003 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_sw : log source reg combination v0.a is allowed";
string PS_004 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_sw : log source reg combination b0 is NOT allowed";
string PS_005 = 
	"ps_2_sw "
	"defb b0, true "
	"log r0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_sw : log source reg combination i0 is NOT allowed";
string PS_006 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"log r0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_sw : log source reg combination p0 is NOT allowed";
string PS_007 = 
	"ps_2_sw "
	"log r0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_sw : log source reg combination s0 is NOT allowed";
string PS_008 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"log r0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_sw : log source reg combination oC0 is NOT allowed";
string PS_009 = 
	"ps_2_sw "
	"log r0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_sw : log source reg combination oDepth is NOT allowed";
string PS_010 = 
	"ps_2_sw "
	"log r0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_sw : log source reg combination v0.gbra is NOT allowed";
string PS_011 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_sw : log source reg combination v0.brga is NOT allowed";
string PS_012 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_sw : log source reg combination v0.abgr is NOT allowed";
string PS_013 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_sw : log source reg combination v0.agrb is NOT allowed";
string PS_014 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_sw : log source reg combination v0.rbga is NOT allowed";
string PS_015 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_sw : log source reg combination v0.rgab is NOT allowed";
string PS_016 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_sw : log source reg combination v0.bgr is NOT allowed";
string PS_017 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_sw : log source reg combination v0.rbg is NOT allowed";
string PS_018 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_sw : log source reg combination v0.gar is NOT allowed";
string PS_019 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_sw : log source reg combination v0.gr is NOT allowed";
string PS_020 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_sw : log source reg combination v0.ab is NOT allowed";
string PS_021 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_sw : log source reg combination v0.bg is NOT allowed";
string PS_022 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_sw : log dest r# is allowed";
string PS_023 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_sw : log _pp dest r# is allowed";
string PS_024 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_sw : log _sat dest r# is allowed";
string PS_025 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_sw : log dest v# is NOT allowed";
string PS_026 = 
	"ps_2_sw "
	"dcl v0 "
	"log v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_sw : log _pp dest v# is NOT allowed";
string PS_027 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_sw : log _sat dest v# is NOT allowed";
string PS_028 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_sw : log dest c# is NOT allowed";
string PS_029 = 
	"ps_2_sw "
	"dcl v0 "
	"log c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_sw : log _pp dest c# is NOT allowed";
string PS_030 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_sw : log _sat dest c# is NOT allowed";
string PS_031 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_sw : log dest b# is NOT allowed";
string PS_032 = 
	"ps_2_sw "
	"dcl v0 "
	"log b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_sw : log _pp dest b# is NOT allowed";
string PS_033 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_sw : log _sat dest b# is NOT allowed";
string PS_034 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_sw : log dest i# is NOT allowed";
string PS_035 = 
	"ps_2_sw "
	"dcl v0 "
	"log i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_sw : log _pp dest i# is NOT allowed";
string PS_036 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_sw : log _sat dest i# is NOT allowed";
string PS_037 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_038_Desc = "ps_2_sw : log dest p0 is NOT allowed";
string PS_038 = 
	"ps_2_sw "
	"dcl v0 "
	"log p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_039_Desc = "ps_2_sw : log _pp dest p0 is NOT allowed";
string PS_039 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_040_Desc = "ps_2_sw : log _sat dest p0 is NOT allowed";
string PS_040 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_sw : log dest s# is NOT allowed";
string PS_041 = 
	"ps_2_sw "
	"dcl v0 "
	"log s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_sw : log _pp dest s# is NOT allowed";
string PS_042 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_sw : log _sat dest s# is NOT allowed";
string PS_043 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_sw : log dest t# is NOT allowed";
string PS_044 = 
	"ps_2_sw "
	"dcl v0 "
	"log t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_sw : log _pp dest t# is NOT allowed";
string PS_045 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_sw : log _sat dest t# is NOT allowed";
string PS_046 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_sw : log dest oC# is NOT allowed";
string PS_047 = 
	"ps_2_sw "
	"dcl v0 "
	"log oC0, v0.x ";

PS_CRITERIA PS_048_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_sw : log _pp dest oC# is NOT allowed";
string PS_048 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp oC0, v0.x ";

PS_CRITERIA PS_049_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_sw : log _sat dest oC# is NOT allowed";
string PS_049 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat oC0, v0.x ";

PS_CRITERIA PS_050_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_sw : log dest oDepth is NOT allowed";
string PS_050 = 
	"ps_2_sw "
	"dcl v0 "
	"log oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_sw : log _pp dest oDepth is NOT allowed";
string PS_051 = 
	"ps_2_sw "
	"dcl v0 "
	"log_pp oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_sw : log _sat dest oDepth is NOT allowed";
string PS_052 = 
	"ps_2_sw "
	"dcl v0 "
	"log_sat oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_sw : log write mask .r is allowed";
string PS_053 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.r, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_sw : log write mask .g is allowed";
string PS_054 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.g, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_sw : log write mask .b is allowed";
string PS_055 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.b, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_sw : log write mask .a is allowed";
string PS_056 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.a, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_sw : log write mask .rg is allowed";
string PS_057 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.rg, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_sw : log write mask .gb is allowed";
string PS_058 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.gb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_sw : log write mask .ba is allowed";
string PS_059 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.ba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_sw : log write mask .rb is allowed";
string PS_060 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.rb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_sw : log write mask .ra is allowed";
string PS_061 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.ra, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_sw : log write mask .ga is allowed";
string PS_062 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.ga, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_sw : log write mask .rgb is allowed";
string PS_063 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.rgb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_sw : log write mask .gba is allowed";
string PS_064 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.gba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_sw : log write mask .rba is allowed";
string PS_065 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.rba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_sw : log write mask .rgba is allowed";
string PS_066 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.rgba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_sw : log write mask .yx is NOT allowed";
string PS_067 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.yx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_sw : log write mask .zx is NOT allowed";
string PS_068 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.zx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_sw : log write mask .zy is NOT allowed";
string PS_069 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.zy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_sw : log write mask .wx is NOT allowed";
string PS_070 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_sw : log write mask .wz is NOT allowed";
string PS_071 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wz, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_sw : log write mask .wy is NOT allowed";
string PS_072 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_sw : log write mask .zyx is NOT allowed";
string PS_073 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.zyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_sw : log write mask .wzy is NOT allowed";
string PS_074 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wzy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_sw : log write mask .wzx is NOT allowed";
string PS_075 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wzx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_sw : log write mask .wyx is NOT allowed";
string PS_076 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_sw : log write mask .yxw is NOT allowed";
string PS_077 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.yxw, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_sw : log write mask .wzyx is NOT allowed";
string PS_078 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.wzyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_sw : log write mask .zxwy is NOT allowed";
string PS_079 = 
	"ps_2_sw "
	"dcl v0 "
	"log r0.zxwy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_sw : log is allowed in a 1 level if b0 block";
string PS_080 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_081_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_sw : log is allowed in a 8 level if b0 block";
string PS_081 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_082_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_sw : log is allowed in a 16 level if b0 block";
string PS_082 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_083_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_sw : log is allowed in a 24 level if b0 block";
string PS_083 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_084_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_sw : log is NOT allowed in a 25 level if b0 block";
string PS_084 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_085_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_sw : log is allowed in a 1 level if b0 nop else block";
string PS_085 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_086_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_sw : log is allowed in a 8 level if b0 nop else block";
string PS_086 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_087_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_sw : log is allowed in a 16 level if b0 nop else block";
string PS_087 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_088_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_sw : log is allowed in a 24 level if b0 nop else block";
string PS_088 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_089_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_sw : log is NOT allowed in a 25 level if b0 nop else block";
string PS_089 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_090_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_sw : log is NOT allowed in a 1 level loop aL, i0 block";
string PS_090 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	log r0, v0.x "
	"endloop ";

PS_CRITERIA PS_091_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_sw : log is NOT allowed in a 2 level loop aL, i0 block";
string PS_091 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_092_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_sw : log is NOT allowed in a 3 level loop aL, i0 block";
string PS_092 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			log r0, v0.x "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_093_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_sw : log is NOT allowed in a 4 level loop aL, i0 block";
string PS_093 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				log r0, v0.x "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_094_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_sw : log is NOT allowed in a 5 level loop aL, i0 block";
string PS_094 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					log r0, v0.x "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_095_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_sw : log is allowed in a 1 level rep i0 block";
string PS_095 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	log r0, v0.x "
	"endrep ";

PS_CRITERIA PS_096_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_sw : log is allowed in a 2 level rep i0 block";
string PS_096 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_097_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_sw : log is allowed in a 3 level rep i0 block";
string PS_097 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			log r0, v0.x "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_098_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_sw : log is allowed in a 4 level rep i0 block";
string PS_098 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				log r0, v0.x "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_099_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_sw : log is NOT allowed in a 5 level rep i0 block";
string PS_099 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					log r0, v0.x "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_100_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_sw : log is allowed in a 1 level if_gt c0.x, c0.y block";
string PS_100 = 
	"ps_2_sw "
	"dcl v0 "
	"if_gt c0.x, c0.y "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_101_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_sw : log is allowed in a 8 level if_gt c0.x, c0.y block";
string PS_101 = 
	"ps_2_sw "
	"dcl v0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_102_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_sw : log is allowed in a 16 level if_ge c0.x, c0.y block";
string PS_102 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_103_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_sw : log is allowed in a 24 level if_ge c0.x, c0.y block";
string PS_103 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																if_ge c0.x, c0.y "
	"																	if_ge c0.x, c0.y "
	"																		if_ge c0.x, c0.y "
	"																			if_ge c0.x, c0.y "
	"																				if_ge c0.x, c0.y "
	"																					if_ge c0.x, c0.y "
	"																						if_ge c0.x, c0.y "
	"																							if_ge c0.x, c0.y "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_104_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_sw : log is NOT allowed in a 25 level if_ge c0.x, c0.y block";
string PS_104 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																if_ge c0.x, c0.y "
	"																	if_ge c0.x, c0.y "
	"																		if_ge c0.x, c0.y "
	"																			if_ge c0.x, c0.y "
	"																				if_ge c0.x, c0.y "
	"																					if_ge c0.x, c0.y "
	"																						if_ge c0.x, c0.y "
	"																							if_ge c0.x, c0.y "
	"																								if_ge c0.x, c0.y "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_105_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_2_sw : log is allowed in a 1 level if_ge c0.x, c0.y nop else block";
string PS_105 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_106_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_2_sw : log is allowed in a 8 level if_ge c0.x, c0.y nop else block";
string PS_106 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_107_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_2_sw : log is allowed in a 16 level if_ge c0.x, c0.y nop else block";
string PS_107 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_108_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_sw : log is allowed in a 24 level if_ge c0.x, c0.y nop else block";
string PS_108 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																if_ge c0.x, c0.y nop else "
	"																	if_ge c0.x, c0.y nop else "
	"																		if_ge c0.x, c0.y nop else "
	"																			if_ge c0.x, c0.y nop else "
	"																				if_ge c0.x, c0.y nop else "
	"																					if_ge c0.x, c0.y nop else "
	"																						if_ge c0.x, c0.y nop else "
	"																							if_ge c0.x, c0.y nop else "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_109_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_sw : log is NOT allowed in a 25 level if_ge c0.x, c0.y nop else block";
string PS_109 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																if_ge c0.x, c0.y nop else "
	"																	if_ge c0.x, c0.y nop else "
	"																		if_ge c0.x, c0.y nop else "
	"																			if_ge c0.x, c0.y nop else "
	"																				if_ge c0.x, c0.y nop else "
	"																					if_ge c0.x, c0.y nop else "
	"																						if_ge c0.x, c0.y nop else "
	"																							if_ge c0.x, c0.y nop else "
	"																								if_ge c0.x, c0.y nop else "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_110_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_sw : log is allowed in a 1 level if p0.x block";
string PS_110 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_111_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_sw : log is allowed in a 8 level if p0.x block";
string PS_111 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_112_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_sw : log is allowed in a 16 level if p0.x block";
string PS_112 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_113_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_sw : log is allowed in a 24 level if p0.x block";
string PS_113 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_114_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_sw : log is NOT allowed in a 25 level if p0.x block";
string PS_114 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_115_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_sw : log is allowed in a 1 level if p0.x nop else block";
string PS_115 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	log r0, v0.x "
	"endif ";

PS_CRITERIA PS_116_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_sw : log is allowed in a 8 level if p0.x nop else block";
string PS_116 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								log r0, v0.x "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_117_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_sw : log is allowed in a 16 level if p0.x nop else block";
string PS_117 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																log r0, v0.x "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_118_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_sw : log is allowed in a 24 level if p0.x nop else block";
string PS_118 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								log r0, v0.x "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_119_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_sw : log is NOT allowed in a 25 level if p0.x nop else block";
string PS_119 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									log r0, v0.x "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_120_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_sw : log is NOT allowed in a 1 level loop aL, i0 break block";
string PS_120 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	log r0, v0.x "
	"endloop ";

PS_CRITERIA PS_121_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_sw : log is NOT allowed in a 8 level loop aL, i0 break block";
string PS_121 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								log r0, v0.x "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_122_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_sw : log is NOT allowed in a 16 level loop aL, i0 break block";
string PS_122 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																log r0, v0.x "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_123_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_sw : log is NOT allowed in a 24 level loop aL, i0 break block";
string PS_123 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								log r0, v0.x "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_124_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_sw : log is NOT allowed in a 25 level loop aL, i0 break block";
string PS_124 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									log r0, v0.x "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_125_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_2_sw : log is allowed in a 1 level rep i0 break block";
string PS_125 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	log r0, v0.x "
	"endrep ";

PS_CRITERIA PS_126_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_sw : log is NOT allowed in a 8 level rep i0 break block";
string PS_126 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								log r0, v0.x "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_127_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_sw : log is NOT allowed in a 16 level rep i0 break block";
string PS_127 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																log r0, v0.x "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_128_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_sw : log is NOT allowed in a 24 level rep i0 break block";
string PS_128 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								log r0, v0.x "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_129_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_sw : log is NOT allowed in a 25 level rep i0 break block";
string PS_129 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									log r0, v0.x "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_130_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_sw : log is NOT allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_130 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	log r0, v0.x "
	"endloop ";

PS_CRITERIA PS_131_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_sw : log is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_131 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								log r0, v0.x "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_132_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_sw : log is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_132 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																log r0, v0.x "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_133_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_sw : log is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_133 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								log r0, v0.x "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_134_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_sw : log is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_134 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									log r0, v0.x "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_135_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_sw : log is allowed in a 1 level rep i0 breakp p0.x block";
string PS_135 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	log r0, v0.x "
	"endrep ";

PS_CRITERIA PS_136_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_2_sw : log is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_136 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								log r0, v0.x "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_137_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_2_sw : log is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_137 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																log r0, v0.x "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_138_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_2_sw : log is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_138 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								log r0, v0.x "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_139_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_2_sw : log is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_139 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									log r0, v0.x "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_140_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_2_sw : log is NOT allowed in a 1 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_140 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	log r0, v0.x "
	"endloop ";

PS_CRITERIA PS_141_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_2_sw : log is NOT allowed in a 8 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_141 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								log r0, v0.x "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_142_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_2_sw : log is NOT allowed in a 16 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_142 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																log r0, v0.x "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_143_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_2_sw : log is NOT allowed in a 24 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_143 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								log r0, v0.x "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_144_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_2_sw : log is NOT allowed in a 25 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_144 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								loop aL, i0 break_ge c0.x, c0.y "
	"																									log r0, v0.x "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_145_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_2_sw : log is allowed in a 1 level rep i0 break_ge c0.x, c0.y block";
string PS_145 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	log r0, v0.x "
	"endrep ";

PS_CRITERIA PS_146_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_sw : log is NOT allowed in a 8 level rep i0 break_ge c0.x, c0.y block";
string PS_146 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								log r0, v0.x "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_147_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_2_sw : log is NOT allowed in a 16 level rep i0 break_ge c0.x, c0.y block";
string PS_147 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																log r0, v0.x "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_148_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_2_sw : log is NOT allowed in a 24 level rep i0 break_ge c0.x, c0.y block";
string PS_148 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								log r0, v0.x "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_149_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_2_sw : log is NOT allowed in a 25 level rep i0 break_ge c0.x, c0.y block";
string PS_149 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								rep i0 break_ge c0.x, c0.y "
	"																									log r0, v0.x "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_150_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_2_sw : log is allowed in a if b0 and if b0 block";
string PS_150 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_151_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_2_sw : log is allowed in a if b0 and if b0 nop else block";
string PS_151 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_152_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_2_sw : log is NOT allowed in a if b0 and loop aL, i0 block";
string PS_152 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_153_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_2_sw : log is allowed in a if b0 and rep i0 block";
string PS_153 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_154_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_2_sw : log is allowed in a if b0 and if_ge c0.x, c0.y block";
string PS_154 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_155_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_2_sw : log is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string PS_155 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_156_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_2_sw : log is allowed in a if b0 and if p0.x block";
string PS_156 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_157_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_2_sw : log is allowed in a if b0 and if p0.x nop else block";
string PS_157 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_158_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_2_sw : log is NOT allowed in a if b0 and loop aL, i0 break block";
string PS_158 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_159_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_2_sw : log is allowed in a if b0 and rep i0 break block";
string PS_159 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_160_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_2_sw : log is NOT allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_160 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_161_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_2_sw : log is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_161 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_162_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_2_sw : log is NOT allowed in a if b0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_162 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_163_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_2_sw : log is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string PS_163 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_164_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if b0 block";
string PS_164 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_165_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if b0 nop else block";
string PS_165 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_166_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_2_sw : log is NOT allowed in a if b0 nop else and loop aL, i0 block";
string PS_166 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_167_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_2_sw : log is allowed in a if b0 nop else and rep i0 block";
string PS_167 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_168_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string PS_168 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_169_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string PS_169 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_170_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if p0.x block";
string PS_170 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_171_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_2_sw : log is allowed in a if b0 nop else and if p0.x nop else block";
string PS_171 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_172_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_2_sw : log is NOT allowed in a if b0 nop else and loop aL, i0 break block";
string PS_172 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_173_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_2_sw : log is allowed in a if b0 nop else and rep i0 break block";
string PS_173 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_174_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_2_sw : log is NOT allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_174 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_175_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_2_sw : log is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_175 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_176_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_2_sw : log is NOT allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_176 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_177_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_2_sw : log is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string PS_177 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_178_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if b0 block";
string PS_178 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_179_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if b0 nop else block";
string PS_179 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_180_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string PS_180 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_181_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and rep i0 block";
string PS_181 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_182_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if_ge c0.x, c0.y block";
string PS_182 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_183_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if_ge c0.x, c0.y nop else block";
string PS_183 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_184_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if p0.x block";
string PS_184 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_185_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and if p0.x nop else block";
string PS_185 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_186_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_186 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_187_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and rep i0 break block";
string PS_187 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_188_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_188 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_189_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_189 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_190_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_190 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_191_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_2_sw : log is NOT allowed in a loop aL, i0 and rep i0 break_ge c0.x, c0.y block";
string PS_191 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_192_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_2_sw : log is allowed in a rep i0 and if b0 block";
string PS_192 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_193_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_2_sw : log is allowed in a rep i0 and if b0 nop else block";
string PS_193 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_194_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_2_sw : log is NOT allowed in a rep i0 and loop aL, i0 block";
string PS_194 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_195_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_2_sw : log is allowed in a rep i0 and rep i0 block";
string PS_195 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_196_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_2_sw : log is allowed in a rep i0 and if_ge c0.x, c0.y block";
string PS_196 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_197_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_2_sw : log is allowed in a rep i0 and if_ge c0.x, c0.y nop else block";
string PS_197 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_198_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_2_sw : log is allowed in a rep i0 and if p0.x block";
string PS_198 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_199_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_2_sw : log is allowed in a rep i0 and if p0.x nop else block";
string PS_199 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_200_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_2_sw : log is NOT allowed in a rep i0 and loop aL, i0 break block";
string PS_200 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_201_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_2_sw : log is allowed in a rep i0 and rep i0 break block";
string PS_201 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_202_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_2_sw : log is NOT allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_202 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_203_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_2_sw : log is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_203 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_204_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_2_sw : log is NOT allowed in a rep i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_204 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_205_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_2_sw : log is allowed in a rep i0 and rep i0 break_ge c0.x, c0.y block";
string PS_205 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"rep i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_206_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if b0 block";
string PS_206 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_207_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if b0 nop else block";
string PS_207 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_208_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y and loop aL, i0 block";
string PS_208 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_209_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and rep i0 block";
string PS_209 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_210_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_210 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_211_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_211 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_212_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if p0.x block";
string PS_212 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_213_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and if p0.x nop else block";
string PS_213 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_214_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y and loop aL, i0 break block";
string PS_214 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_215_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and rep i0 break block";
string PS_215 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_216_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_216 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_217_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_217 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_218_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_218 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_219_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_219 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_220_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if b0 block";
string PS_220 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_221_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if b0 nop else block";
string PS_221 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_222_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 block";
string PS_222 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_223_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and rep i0 block";
string PS_223 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_224_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y block";
string PS_224 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_225_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y nop else block";
string PS_225 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_226_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if p0.x block";
string PS_226 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_227_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and if p0.x nop else block";
string PS_227 = 
	"ps_2_sw "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_228_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break block";
string PS_228 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_229_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and rep i0 break block";
string PS_229 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_230_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_230 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_231_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_231 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_232_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_2_sw : log is NOT allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_232 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_233_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_2_sw : log is allowed in a if_ge c0.x, c0.y nop else and rep i0 break_ge c0.x, c0.y block";
string PS_233 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_234_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_2_sw : log is allowed in a if p0.x and if b0 block";
string PS_234 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if p0.x "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_235_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_2_sw : log is allowed in a if p0.x and if b0 nop else block";
string PS_235 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if p0.x "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_236_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_2_sw : log is NOT allowed in a if p0.x and loop aL, i0 block";
string PS_236 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_237_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_2_sw : log is allowed in a if p0.x and rep i0 block";
string PS_237 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_238_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_2_sw : log is allowed in a if p0.x and if_ge c0.x, c0.y block";
string PS_238 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_239_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_2_sw : log is allowed in a if p0.x and if_ge c0.x, c0.y nop else block";
string PS_239 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_240_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_2_sw : log is allowed in a if p0.x and if p0.x block";
string PS_240 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_241_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_2_sw : log is allowed in a if p0.x and if p0.x nop else block";
string PS_241 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x "
	"	if p0.x nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_242_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_2_sw : log is NOT allowed in a if p0.x and loop aL, i0 break block";
string PS_242 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_243_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_2_sw : log is allowed in a if p0.x and rep i0 break block";
string PS_243 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 break "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_244_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_2_sw : log is NOT allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_244 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_245_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_2_sw : log is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_245 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_246_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_2_sw : log is NOT allowed in a if p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_246 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_247_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_2_sw : log is allowed in a if p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_247 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_248_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and if b0 block";
string PS_248 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if p0.x nop else "
	"	if b0 "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_249_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and if b0 nop else block";
string PS_249 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_250_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_2_sw : log is NOT allowed in a if p0.x nop else and loop aL, i0 block";
string PS_250 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		log r0, v0.x "
	"	endloop "
	"endif ";

PS_CRITERIA PS_251_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and rep i0 block";
string PS_251 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"if p0.x nop else "
	"	rep i0 "
	"		log r0, v0.x "
	"	endrep "
	"endif ";

PS_CRITERIA PS_252_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and if_ge c0.x, c0.y block";
string PS_252 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_253_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and if_ge c0.x, c0.y nop else block";
string PS_253 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y nop else "
	"		log r0, v0.x "
	"	endif "
	"endif ";

PS_CRITERIA PS_254_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_2_sw : log is allowed in a if p0.x nop else and if p0.x block";
string PS_254 = 
	"ps_2_sw "
	"dcl v0 "
	"if p0.