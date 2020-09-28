 = pTexVB[0].x;
	m_vPosLimits[1] = pTexVB[0].y;
	m_vPosLimits[2] = pTexVB[3].x;
	m_vPosLimits[3] = pTexVB[3].y;
    	
	//create and fill out index buffer used

	pTexIB = new WORD[6*uLevels];

    if (NULL == pTexIB)
		return false;
    	
	for (UINT level = 0; level < uLevels; level++)
	{
		pTexIB[0+6*level] = 0+4*level;
		pTexIB[1+6*level] = 1+4*level; 
		pTexIB[2+6*level] = 3+4*level;
		pTexIB[3+6*level] = 0+4*level;	
		pTexIB[4+6*level] = 3+4*level;
		pTexIB[5+6*level] = 2+4*level;
	}
    	
    m_TexTLInfo.nVertices = 4*uLevels;
    m_TexTLInfo.nIndices = 6;
    m_TexTLInfo.pVertices = pTexVB;
    m_TexTLInfo.pIndices = pTexIB;

	VERTEXELEMENT declTL[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
        {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
        {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
		{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
		{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
        DECL_END()
	};
    	
    if(!CreateVertexDeclaration(declTL, &(m_TexTLInfo.pDecl)))
    {
		WriteToLog("CreateTextures: CreateVertexDeclaration() failed.\n");
        return false;
    }

	//create images used to fill out texture mip maps

	ppImages = new CImage*[uLevels];
    	
	if (NULL == ppImages)
		return false;

	ZeroMemory(ppImages,sizeof(CImage *) * uLevels);
    		
	for (UINT j = 0; j < uLevels; j++)
	{
		ppImages[j] = new CImage();
    		
		if (ppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}
	}
    	
	// Create the textures
    	
	for (UINT i = 0; i < m_nMaxTex2D; i++)
	{
		uWidth  = TEXSIZE;
		uHeight = TEXSIZE;

		if (i == 2)
			uStripes = 1;
		else
			uStripes = 8;
    		
		if( !CreateTexture(uWidth, uHeight, uLevels, Usage, *pTexFormat, Pool, m_pTex2D+m_nTex2D++) )
		{
			WriteToLog("CreateTextures: CreateTexture() failed on %dth texture.\n", m_nTex2D-1);
			bResult = false;
			goto Exit;
		}
    		
		if(m_pTex2D[m_nTex2D-1]->IsPalettized())
			m_pTex2D[m_nTex2D-1]->SetPalette(NULL);
    			
		for (UINT j = 0; j < uLevels; j++)
		{
			if (strcmp(m_szTestName, "PShader3.0") == 0 && i == 2)
			{
				m_Stripes[0] = RGBA_MAKE(255,  0, 0, 255);  // Red
				m_Stripes[1] = RGBA_MAKE( 0, 255, 0, 255);	// Green
				m_Stripes[2] = RGBA_MAKE( 0, 255, 0, 255);	// Green
				m_Stripes[3] = RGBA_MAKE(255,  0, 0, 255);  // Red
				uStripes = 4;
			}
			else if ( pTexFormat->d3dfFormat == FMT_R16F || pTexFormat->d3dfFormat == FMT_R32F )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
			}
			else if ( pTexFormat->d3dfFormat == FMT_A8 )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
			}
			else if ( pTexFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
					m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
			}
			else if ( pTexFormat->d3dfFormat == FMT_UYVY || pTexFormat->d3dfFormat == FMT_YUY2 )
			{
				for(UINT k = 0; k < MAX_STRIPES; k++)
				{
					switch ((i+j)%7)
					{
						case 0:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
						case 1:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
						case 2:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	0,			255-20*k); break;
						case 3:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			0,			255-20*k,	255-20*k); break;
						case 4:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
						case 5:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
						case 6:	m_Stripes[(i+j+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	255-20*k,	255-20*k); break;
					}
				}
			}
			else
			{
				m_Stripes[(i+j+0)%8] = RGBA_MAKE(255,255,255,255);  // White
				m_Stripes[(i+j+1)%8] = RGBA_MAKE(255,  0,  0,219);	// Red
				m_Stripes[(i+j+2)%8] = RGBA_MAKE(  0,255,  0,183);	// Green
				m_Stripes[(i+j+3)%8] = RGBA_MAKE(  0,  0,255,147);	// Blue
				m_Stripes[(i+j+4)%8] = RGBA_MAKE(255,255,  0,111);	// Yellow
				m_Stripes[(i+j+5)%8] = RGBA_MAKE(255,  0,255, 75);	// Magenta
				m_Stripes[(i+j+6)%8] = RGBA_MAKE(  0,255,255, 39);	// Cyan
				m_Stripes[(i+j+7)%8] = RGBA_MAKE(128,128,128,  0);  // Grey
			 }
    			
			 if (!ppImages[j]->LoadStripes(uWidth, uHeight, uStripes, m_Stripes, ((m_pCurrentMode->nTextureFormat+i+j)%2 || uStripes == 4)?true:false))
			 {
				 bResult = false;
				 goto Exit;
			 }
    			
			 uWidth  = max(1, uWidth  >> 1);
			 uHeight = max(1, uHeight >> 1);
		}
    		
		if( !m_pTex2D[m_nTex2D-1]->LoadImage( 0, ppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
    }
    
Exit:
	if (NULL != ppImages)
	for (UINT k = 0; k < uLevels; k++)
		SAFEDELETE( ppImages[k] );

	SAFEDELETEA( ppImages );

	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateCubeTextures()
{
    if (m_uCommonCubeFormats == 0)
        return true;    // No textures;

	FORMAT *pCubeFormat = &m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	CImage    ***pppImages;
	TLVOLVERTEX *pCubeVB = NULL;
	PWORD        pCubeIB = NULL;
	UINT         uWidth = 64;
	UINT         uLevels = 7;
	CAPS         caps;
	bool         bResult = true;

	m_pSrcDevice->GetDeviceCaps(&caps); 

	if (!(caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		WriteToLog("CPShaderX::CreateCubeTextures - cube maps are not supported.\n");
		return true;
	}
	
	if (KeySet("nomipcube") || !(caps.TextureCaps  & D3DPTEXTURECAPS_MIPCUBEMAP))
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, *pCubeFormat))) 
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}

	m_CubeSizes[0] = (float) uWidth; //width  of the primitive
    m_CubeSizes[1] = (float) uWidth; //height of the primitive
    m_CubeSizes[2] = (float) uWidth; //width  of the texture
    m_CubeSizes[3] = (float) uWidth; //height of the texture
	
	//create and fill out vertex buffer used

	pCubeVB = new TLVOLVERTEX[24*uLevels];

	if (NULL == pCubeVB)
		return false;

	for(UINT level = 0; level < uLevels; level++)
	{
		float x0 = (float)( m_pCurrentMode->nBackBufferWidth/2 - uWidth*1.5f );
		float y0 = (float)( m_pCurrentMode->nBackBufferHeight/2 - uWidth );
		float x1 = x0 + uWidth;
		float y1 = y0 + uWidth;

        float fPrecision = (1.0f / 32.0f) / (float) uWidth;

        // Maximum range of texture coordinate
		
		float fRange = 1.0f - fPrecision;
		float fStart = -1.0f;
		
		//face 0
		pCubeVB[ 0+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fStart,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		pCubeVB[ 1+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fStart, 0.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 6.f));
		pCubeVB[ 2+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 6.f));
		pCubeVB[ 3+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fStart, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fStart, 6.f));
		
		//face 1
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[ 4+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fRange,  fStart, 0.f), D3DXVECTOR4(  -1.0f,  fStart,  fRange, 1.f), D3DXVECTOR4(  -1.0f