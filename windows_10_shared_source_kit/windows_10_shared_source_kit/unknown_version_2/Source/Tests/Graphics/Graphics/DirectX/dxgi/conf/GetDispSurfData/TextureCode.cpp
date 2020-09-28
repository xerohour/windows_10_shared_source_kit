,  fStart,  fRange, 6.f));
		pCubeVB[ 5+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fRange,  fRange, 0.f), D3DXVECTOR4(  -1.0f,  fStart,  fStart, 1.f), D3DXVECTOR4(  -1.0f,  fRange,  fRange, 6.f));
		pCubeVB[ 6+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fStart,  fStart, 0.f), D3DXVECTOR4(  -1.0f,  fRange,  fRange, 1.f), D3DXVECTOR4(  -1.0f,  fStart,  fStart, 6.f));
		pCubeVB[ 7+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  -1.0f,  fStart,  fRange, 0.f), D3DXVECTOR4(  -1.0f,  fRange,  fStart, 1.f), D3DXVECTOR4(  -1.0f,  fRange,  fStart, 6.f));
		
		//face 2
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[ 8+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fStart, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fStart, 6.f));
		pCubeVB[ 9+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fStart, 0.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 6.f));
		pCubeVB[10+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fStart, 6.f));
		pCubeVB[11+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fStart,  fRange,  fStart, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		
		//face 3
		x0 = (float)( m_pCurrentMode->nBackBufferWidth/2 - 1.5f*uWidth ); 
		x1 = x0 + uWidth;
		y0 = y1+4; y1 += uWidth+4;
		pCubeVB[12+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  -1.0f,  fRange, 0.f), D3DXVECTOR4(  fRange,  -1.0f,  fStart, 1.f), D3DXVECTOR4(  fRange,  -1.0f,  fStart, 6.f));
		pCubeVB[13+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  -1.0f,  fRange, 0.f), D3DXVECTOR4(  fStart,  -1.0f,  fStart, 1.f), D3DXVECTOR4(  fRange,  -1.0f,  fRange, 6.f));
		pCubeVB[14+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  -1.0f,  fStart, 0.f), D3DXVECTOR4(  fRange,  -1.0f,  fRange, 1.f), D3DXVECTOR4(  fStart,  -1.0f,  fStart, 6.f));
		pCubeVB[15+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  -1.0f,  fStart, 0.f), D3DXVECTOR4(  fStart,  -1.0f,  fRange, 1.f), D3DXVECTOR4(  fStart,  -1.0f,  fRange, 6.f));
		
		//face 4
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[16+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  fRange, 0.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fStart,  fRange, 6.f));
		pCubeVB[17+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  fRange, 0.f), D3DXVECTOR4(  fStart,  fStart,  fRange, 1.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 6.f));
		pCubeVB[18+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fStart,  fRange, 0.f), D3DXVECTOR4(  fRange,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fStart,  fRange, 6.f));
		pCubeVB[19+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  fRange, 0.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 1.f), D3DXVECTOR4(  fStart,  fRange,  fRange, 6.f));
		
		//face 5
		x0 = x1+4; x1 += uWidth+4;
		pCubeVB[20+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fRange,  -1.0f, 0.f), D3DXVECTOR4(  fStart,  fStart,  -1.0f, 1.f), D3DXVECTOR4(  fRange,  fRange,  -1.0f, 6.f));
		pCubeVB[21+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fRange,  -1.0f, 0.f), D3DXVECTOR4(  fRange,  fStart,  -1.0f, 1.f), D3DXVECTOR4(  fRange,  fStart,  -1.0f, 6.f));
		pCubeVB[22+24*level] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fRange,  fStart,  -1.0f, 0.f), D3DXVECTOR4(  fStart,  fRange,  -1.0f, 1.f), D3DXVECTOR4(  fStart,  fRange,  -1.0f, 6.f));
		pCubeVB[23+24*level] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4(  fStart,  fStart,  -1.0f, 0.f), D3DXVECTOR4(  fRange,  fRange,  -1.0f, 1.f), D3DXVECTOR4(  fStart,  fStart,  -1.0f, 6.f));
		uWidth >>= 1;
	}
	
	for(UINT i = 0; i < 24*uLevels; i++)
	{
		pCubeVB[i].x -= 0.5f;
		pCubeVB[i].y -= 0.5f;
	}

	//create and fill out index buffer used
	
	pCubeIB = new WORD[36*uLevels];

	if (NULL == pCubeIB)
		return false;
	
	for(UINT l = 0; l < uLevels; l++)
    for(UINT i = 0; i < 6;       i++)
	{
		pCubeIB[i*6+0+36*l]=i*4+0+24*l;
		pCubeIB[i*6+1+36*l]=i*4+1+24*l;
		pCubeIB[i*6+2+36*l]=i*4+3+24*l;
		pCubeIB[i*6+3+36*l]=i*4+0+24*l;
		pCubeIB[i*6+4+36*l]=i*4+3+24*l;
		pCubeIB[i*6+5+36*l]=i*4+2+24*l;
	}

	m_CubeTLInfo.nVertices = 24*uLevels;
    m_CubeTLInfo.nIndices = 36;
    m_CubeTLInfo.pVertices = pCubeVB;
	m_CubeTLInfo.pIndices = pCubeIB;
	
	VERTEXELEMENT declTL[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
            {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,    0},
            {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
			{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  2},
            DECL_END()
	};
	
    if(!CreateVertexDeclaration(declTL, &(m_CubeTLInfo.pDecl)))
    {
		WriteToLog("CreateCubeTextures: CreateVertexDeclaration() failed.\n");
        return false;
    }

	//create images used to fill out texture mip maps

	pppImages = new CImage**[uLevels];
	
	if (pppImages == NULL)
		return false;

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT j = 0; j < uLevels; j++)
	{
		pppImages[j] = new CImage*[6];
		
		if (pppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}

		ZeroMemory(pppImages[j], sizeof(CImage *) * 6);
		
		for (UINT jj = 0; jj < 6; jj++)
		{
			pppImages[j][jj] = new CImage();

			if ( pppImages[j][jj] == NULL )
			{
				bResult = false;
				goto Exit;
			}
		}
	}
	
	// Create the cube textures
	
	for (UINT i = 0; i < m_nMaxTexCube; i++)
	{
		uWidth =  (UINT) m_CubeSizes[2];
		
		if( !CreateCubeTexture(uWidth, uLevels, Usage, *pCubeFormat, Pool, m_pTexCube+m_nTexCube++) )
		{
			WriteToLog("CreateCubeTextures: CreateCubeTexture() failed on %dth cube texture.\n", m_nTexCube-1);
			bResult = false;
			goto Exit;
		}
		
		if(m_pTexCube[m_nTexCube-1]->IsPalettized())
			m_pTexCube[m_nTexCube-1]->SetPalette(NULL);
			
		for (UINT j = 0; j < uLevels; j++)
		{
			for (UINT jj = 0; jj < 6; jj++)
			{
				if ( pCubeFormat->d3dfFormat == FMT_R16F || pCubeFormat->d3dfFormat == FMT_R32F )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
				}
				else if ( pCubeFormat->d3dfFormat == FMT_A8 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
				}
				else if ( pCubeFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(1+i+j+jj+k)%MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
				}
				else if ( pCubeFormat->d3dfFormat == FMT_UYVY || pCubeFormat->d3dfFormat == FMT_YUY2 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
					{
						switch ((1+i+j+jj)%7)
						{
							case 0:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
							case 1:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
							case 2:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		255-20*k,	0,			255-20*k); break;
							case 3:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		0,			255-20*k,	255-20*k); break;
							case 4:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
							case 5:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
							case 6:	m_Stripes[(1+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,		255-20*k,	255-20*k,	255-20*k); break;
						}
					}
				}
				else
				{
					m_Stripes[(1+i+j+jj+0)%8] = RGBA_MAKE(255,255,255,255);  // White
					m_Stripes[(1+i+j+jj+1)%8] = RGBA_MAKE(255,  0,  0,225);  // Red
					m_Stripes[(1+i+j+jj+2)%8] = RGBA_MAKE(  0,255,  0,200);	 // Green
					m_Stripes[(1+i+j+jj+3)%8] = RGBA_MAKE(  0,  0,255,175);  // Blue
					m_Stripes[(1+i+j+jj+4)%8] = RGBA_MAKE(255,255,  0,150);	 // Yellow
					m_Stripes[(1+i+j+jj+5)%8] = RGBA_MAKE(255,  0,255,125);	 // Magenta
					m_Stripes[(1+i+j+jj+6)%8] = RGBA_MAKE(  0,255,255,100);	 // Cyan
					m_Stripes[(1+i+j+jj+7)%8] = RGBA_MAKE(128,128,128, 75);  // Grey
				}

				if (!pppImages[j][jj]->LoadStripes(uWidth, uWidth, i==0 ? 8 : 1, m_Stripes, (m_pCurrentMode->nCubeFormat+j+jj)%2?true:false))
				{
					bResult = false;
					goto Exit;
				}
			}
			uWidth  = max(1, uWidth >> 1);
		}

		if( !m_pTexCube[m_nTexCube-1]->LoadImage( pppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
	}

Exit:	
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT kk = 0; kk < 6; kk++)
			SAFEDELETE(pppImages[k][kk]);

		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA( pppImages );

	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateVolTextures()
{
    if (m_uCommonVolumeFormats == 0)
        return true;    // No textures;

	FORMAT *pVolFormat = &m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];
	DWORD Pool  = POOL_MANAGED;
	DWORD Usage = FMWK_DEFAULT;

	CImage     ***pppImages = NULL;
	TLVOLVERTEX  *pVolVB    = NULL;
	PWORD         pVolIB    = NULL;
	UINT          uWidth    = TEXSIZE;
	UINT          uHeight   = TEXSIZE;
	UINT          uSlices   = 16;
	UINT          uLevels   = 6;
	CAPS          caps;
	bool          bResult = true;

	m_pSrcDevice->GetDeviceCaps(&caps);

	if (!(caps.TextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		WriteToLog("CPShaderX::CreateVolTextures - volumes are not supported.\n");
		return true;
	}

	if (KeySet("nomipvol") || !(caps.TextureCaps  & D3DPTEXTURECAPS_MIPVOLUMEMAP))
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, *pVolFormat)))
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSW, D3DTADDRESS_CLAMP);
		}
	}

	m_VolSizes[0] = (float) uWidth;  //width  of the primitive
    m_VolSizes[1] = (float) uHeight; //height of the primitive
    m_VolSizes[2] = (float) uWidth;  //width  of the texture
    m_VolSizes[3] = (float) uHeight; //height of the texture

	//create and fill out vertex buffer used
	
	pVolVB = new TLVOLVERTEX[(uSlices+2)*4*uLevels];

	if (NULL == pVolVB)
		return false;

	for (UINT l = 0; l < uLevels; l++)
	{
		float w = 1.f/(2.f*uSlices);
		int row = (int) sqrt((float)uSlices);
		float x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]*(row>>1)/(1<<l);
		float y0 = m_pCurrentMode->nBackBufferHeight/2 - m_VolSizes[3]*((row>>1)+1)/(1<<l);
		float x1 = x0 + m_VolSizes[2]/(1<<l);
		float y1 = y0 + m_VolSizes[3]/(1<<l);

		//front view
		for(UINT i = 0; i < uSlices; i++)
		{
			if (i%row)
			{
				x0 = x1 + 3.f; 
				x1 += m_VolSizes[2]/(1<<l) + 3.f;
			}
			else
			{	
				x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]*(row>>1)/(1<<l); 
				x1 = x0 + m_VolSizes[2]/(1<<l);
				y0 = y1 + 3.f;
				y1 += m_VolSizes[3]/(1<<l) + 3.f; 
			}

			pVolVB[(uSlices+2)*4*l+i*4+0] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, w, 0.f), D3DXVECTOR4( 0.99f, 0.99f, w, 1.f), D3DXVECTOR4( 0.00f, 0.00f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+1] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, w, 0.f), D3DXVECTOR4( 0.00f, 0.99f, w, 1.f), D3DXVECTOR4( 0.00f, 0.99f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+2] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, w, 0.f), D3DXVECTOR4( 0.99f, 0.00f, w, 1.f), D3DXVECTOR4( 0.99f, 0.00f, w, 5.f));
			pVolVB[(uSlices+2)*4*l+i*4+3] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.99f, w, 0.f), D3DXVECTOR4( 0.00f, 0.00f, w, 1.f), D3DXVECTOR4( 0.99f, 0.99f, w, 5.f));
			w += 1.f/uSlices;
		}

		//side views
		x0 = m_pCurrentMode->nBackBufferWidth/2 - m_VolSizes[2]/(1<<(l+1));
		y0 = m_pCurrentMode->nBackBufferHeight/2 - m_VolSizes[3]/(1<<(l+1));
		x1 = x0 + m_VolSizes[2]/(1<<l); 
		y1 = y0 + m_VolSizes[3]/(1<<l);
		//need to get integer values to later offset it by 0.5f
		//which helps to match texturing between REF and HW
		x0 = (float)((int)x0);
		x1 = (float)((int)x1);
        y0 = (float)((int)y0);
		y1 = (float)((int)y1);

		pVolVB[(uSlices+2)*4*l+uSlices*4+0] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+1] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 1.f), D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+2] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+3] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.00f, 0.99f, 0.99f, 5.f));

		pVolVB[(uSlices+2)*4*l+uSlices*4+4] = TLVOLVERTEX(D3DXVECTOR3( x0, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+5] = TLVOLVERTEX(D3DXVECTOR3( x1, y0, 0.f), 0xffffffff, D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+6] = TLVOLVERTEX(D3DXVECTOR3( x0, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, 0.00f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 1.f), D3DXVECTOR4( 0.00f, 0.00f, 0.99f, 5.f));
		pVolVB[(uSlices+2)*4*l+uSlices*4+7] = TLVOLVERTEX(D3DXVECTOR3( x1, y1, 0.f), 0xffffffff, D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 0.f), D3DXVECTOR4( 0.00f, 0.00f, 0.00f, 1.f), D3DXVECTOR4( 0.99f, 0.00f, 0.99f, 5.f));
	}
	for(UINT i = 0; i < (uSlices+2)*4*uLevels; i++)
	{
		pVolVB[i].x -= 0.5f;
		pVolVB[i].y -= 0.5f;
	}

	//create and fill out index buffer used
	
	pVolIB = new WORD[(uSlices+2)*6*uLevels];

	if (NULL == pVolIB)
		return false;

	for (UINT l = 0; l < uLevels;   l++)
	for (UINT i = 0; i < uSlices+2; i++)
	{
		pVolIB[(uSlices+2)*6*l+i*6+0] = (uSlices+2)*4*l+i*4+0;
		pVolIB[(uSlices+2)*6*l+i*6+1] = (uSlices+2)*4*l+i*4+1;
		pVolIB[(uSlices+2)*6*l+i*6+2] = (uSlices+2)*4*l+i*4+3;
		pVolIB[(uSlices+2)*6*l+i*6+3] = (uSlices+2)*4*l+i*4+0;
		pVolIB[(uSlices+2)*6*l+i*6+4] = (uSlices+2)*4*l+i*4+3;
		pVolIB[(uSlices+2)*6*l+i*6+5] = (uSlices+2)*4*l+i*4+2;
	}

	m_VolTLInfo.nVertices = (uSlices+2)*4*uLevels;
	m_VolTLInfo.nIndices  = (uSlices+2)*6;
	m_VolTLInfo.pVertices = pVolVB;
	m_VolTLInfo.pIndices  = pVolIB;

	VERTEXELEMENT declTL[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
            {0, 16, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_COLOR,     0},
            {0, 20, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
			{0, 52, (DECLTYPE)D3DDECLTYPE_FLOAT4,   DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  2},
            DECL_END()
	};
	if(!CreateVertexDeclaration(declTL, &(m_VolTLInfo.pDecl)))
	{
		WriteToLog("CreateVolTextures: CreateVertexDeclaration() failed.\n");
		return false;
	}

	//create images used to fill out texture mip maps

	pppImages = new CImage**[uLevels];

	if (pppImages == NULL)
		return false;

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	UINT uDepth = uSlices;
	for (UINT j = 0; j < uLevels; j++)
	{
		pppImages[j] = new CImage*[uDepth];

		if (pppImages[j] == NULL)
		{
			bResult = false;
			goto Exit;
		}

		ZeroMemory(pppImages[j], sizeof(CImage **) * uDepth);

		for (UINT jj = 0; jj < uDepth; jj++)
		{
			pppImages[j][jj] = new CImage();

			if ( pppImages[j][jj] == NULL )
			{
				bResult = false;
				goto Exit;
			}
		}
		uDepth = max( 1, uDepth>>1 );
	}

	// Create the volume textures

	for (UINT i = 0; i < m_nMaxTexVol; i++)
	{
		uWidth  = TEXSIZE;
		uHeight = TEXSIZE;
		uDepth  = uSlices;
		if( !CreateVolumeTexture(uWidth, uHeight, uDepth, uLevels, Usage, *pVolFormat, Pool, m_pTexVol+m_nTexVol++) )
		{
			WriteToLog("CreateVolTextures: CreateVolumeTexture() failed on %dth texture.\n", m_nTexVol-1);
			bResult = false;
			goto Exit;
		}

		if(m_pTexVol[m_nTexVol-1]->IsPalettized())
			m_pTexVol[m_nTexVol-1]->SetPalette(NULL);

		for (UINT j = 0; j < uLevels; j++)
		{
			for (UINT jj = 0; jj < uDepth; jj++)
			{
				if ( pVolFormat->d3dfFormat == FMT_R16F || pVolFormat->d3dfFormat == FMT_R32F )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(255-25*k, 0, 0, 0xff);
				}
				else if ( pVolFormat->d3dfFormat == FMT_A8 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(0, 0, 0, 25*k);
				}
				else if ( pVolFormat->ddpfPixelFormat.dwFlags & PF_LUMINANCE )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
						m_Stripes[(2+i+j+jj/2+k)%MAX_STRIPES] = RGBA_MAKE(0, 255-20*k, 0, 0xff);
				}
				else if ( pVolFormat->d3dfFormat == FMT_UYVY || pVolFormat->d3dfFormat == FMT_YUY2 )
				{
					for(UINT k = 0; k < MAX_STRIPES; k++)
					{
						switch ((2+i+j+jj)%7)
						{
							case 0:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	255-20*k,	255-20*k); break;
							case 1:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			0,			255-20*k); break;
							case 2:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	0,			255-20*k); break;
							case 3:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			0,			255-20*k,	255-20*k); break;
							case 4:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	255-20*k,	0,			255-20*k); break;
							case 5:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(255-20*k,	0,			255-20*k,	255-20*k); break;
							case 6:	m_Stripes[(2+i+j+jj+k) % MAX_STRIPES] = RGBA_MAKE(0,			255-20*k,	255-20*k,	255-20*k); break;
						}
					}
				}
				else
				{
					m_Stripes[(2+i+j+jj/2+0)%8] = RGBA_MAKE(255,255,255,255); // White
					m_Stripes[(2+i+j+jj/2+1)%8] = RGBA_MAKE(255,  0,  0,225);	// Red
					m_Stripes[(2+i+j+jj/2+2)%8] = RGBA_MAKE(  0,255,  0,200);	// Green
					m_Stripes[(2+i+j+jj/2+3)%8] = RGBA_MAKE(  0,  0,255,175);	// Blue
					m_Stripes[(2+i+j+jj/2+4)%8] = RGBA_MAKE(255,255,  0,150);	// Yellow
					m_Stripes[(2+i+j+jj/2+5)%8] = RGBA_MAKE(255,  0,255,125);	// Magenta
					m_Stripes[(2+i+j+jj/2+6)%8] = RGBA_MAKE(  0,255,255,100);	// Cyan
					m_Stripes[(2+i+j+jj/2+7)%8] = RGBA_MAKE(128,128,128, 75); // Grey
				}
				if (!pppImages[j][jj]->LoadStripes(uWidth, uWidth, i==0 ? 8 : 1, m_Stripes, (m_pCurrentMode->nVolumeFormat+j+jj)%2?true:false))
				{
					bResult = false;
					goto Exit;
				}
			}
			uWidth  = max(1, uWidth  >> 1);
			uHeight = max(1, uHeight >> 1);
			uDepth  = max(1, uDepth  >> 1);
		}
		if( !m_pTexVol[m_nTexVol-1]->LoadImage( pppImages, TEXLOAD_DEFAULT ) )
		{
			bResult = false;
			goto Exit;
		}
	}

Exit:
    uDepth = uSlices;
	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{	
		if (NULL != pppImages[k])
		for (UINT kk = 0; kk < uDepth; kk++)
			SAFEDELETE(pppImages[k][kk]);

		SAFEDELETEA(pppImages[k]);
		uDepth = max( 1, uDepth>>1 );
	}

	SAFEDELETEA( pppImages );
	
	return bResult;
}

// ----------------------------------------------------------------------------

bool CPShader2_0::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 2)
    {
        WriteToLog(_T("Pixel shader version 2.0 required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                   
        return false;
    }
    
    return true;
}
// ----------------------------------------------------------------------------

bool CPShader2_x::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 2)
    {
        WriteToLog(_T("Pixel shader version 2.x required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                   
        return false;
    }
    if (m_pSrcDevice->GetCaps()->PS20Caps.Caps == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.DynamicFlowControlDepth == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.StaticFlowControlDepth == 0 &&
        m_pSrcDevice->GetCaps()->PS20Caps.NumTemps == D3DPS20_MIN_NUMTEMPS &&
        m_pSrcDevice->GetCaps()->PS20Caps.NumInstructionSlots == D3DPS20_MIN_NUMINSTRUCTIONSLOTS)
    {
        WriteToLog(_T("Pixel shader version 2.x required! Device supports 2.0 but none of the 2.x features!\n"));
        
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool CPShader3_0::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if ((D3DSHADER_VERSION_MAJOR(dwMaxPSVersion) < 3))
    {
        WriteToLog( _T("Minimum pixel shader version of 3.0 required for this test.\n") );
        
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

HRESULT CInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	FILE *pFile = NULL;
	
	if(	m_bLoadFromFile )
	{
		pFile = fopen(pFileName, "r");
		if(NULL == pFile)
			return E_FAIL;
		
		*pBytes = _filelength(_fileno(pFile));
		*ppData = new BYTE[*pBytes];
		if(NULL == *ppData)
			return E_OUTOFMEMORY;

		memset((LPVOID)*ppData, 0, sizeof(BYTE) * *pBytes);
		*pBytes = (UINT) fread(*((char**)ppData), sizeof(TCHAR), *pBytes, pFile);
		fclose(pFile);
	}
	else
	{
		LPVOID pData = NULL;

		if(!LoadFromResource(pFileName, NULL, (DWORD*)pBytes))
			return E_FAIL;

		*ppData = new BYTE[*pBytes];
		if(NULL == *ppData)
			return E_OUTOFMEMORY;

		if(!LoadFromResource(pFileName, &pData, NULL))
		   return E_FAIL;
		memcpy((LPVOID)*ppData, pData, *pBytes);
	}

    return S_OK;
}

// ----------------------------------------------------------------------------

HRESULT CInclude::Close(LPCVOID pData)
{
	SAFEDELETEA(pData);
	return S_OK;
}
/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    async.c

Abstract:

    This module contains the support for asynchronous PNP IRPs. Currently, we only
    support asynchronous handling of IRP_MN_START_DEVICE and
    IRP_MN_QUERY_DEVICE_RELATIONS-BusRelations.

Author:

    Santosh Jodh (santoshj) 19-Feb-2003

Environment:

    Kernel mode


Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Device completion queue for asynchronous PNP operations.
//
PNP_DEVICE_COMPLETION_QUEUE PnpDeviceCompletionQueue;

//
// PNP supports async start and enumeration across the entire device tree.
//

ULONG PnpAsyncOptions = PNP_ASYNC_START_ENABLED | PNP_ASYNC_ENUM_ENABLED | PNP_ASYNC_TREE_WIDE;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PnpDeviceCompletionRequestCreate)
#pragma alloc_text(PAGE, PnpDeviceCompletionProcessCompletedRequest)
#pragma alloc_text(PAGE, PnpDeviceCompletionProcessCompletedRequests)
#endif // ALLOC_PRAGMA

PPNP_DEVICE_COMPLETION_REQUEST
PnpDeviceCompletionRequestCreate (
    __in PDEVICE_NODE         DeviceNode,
    __in PNP_DEVNODE_STATE    CompletionState,
    __in_opt PVOID            Context
    )

/*++

Description

    This function creates an individual PNP_DEVICE_COMPLETION_REQUEST and initializes it.

Parameters

    DeviceNode - Pointer to the associated devnode for the entry being created.

    CompletionState - Devnode will be set to this state on completion of the operation.

    Context - Pointer to any associated context.

Return

    Pointer to a newly created and initialize PNP_DEVICE_COMPLETION_REQUEST if successful.
    NULL on failure.

--*/

{

    PPNP_DEVICE_COMPLETION_REQUEST request;

    PAGED_CODE();

    request = ExAllocatePoolWithTag(NonPagedPoolNx,
                                    sizeof(*request),
                                    PNP_POOLTAG_DEVICE_COMPLETION_REQUEST);
    if (!request) {
        goto exit;
    }

    //
    // Initialize the completion request.
    //

    InitializeListHead(&request->ListEntry);
    request->ReferenceCount = 1;
    request->IrpPended = FALSE;
    request->Context = Context;
    request->DeviceNode = DeviceNode;
    request->CompletionState = CompletionState;
    request->Status = STATUS_INTERNAL_ERROR;
    request->Information = NULL;

exit:

    return request;
}

VOID
PnpDeviceCompletionRequestDestroy (
    __inout PPNP_DEVICE_COMPLETION_REQUEST Entry
    )

/*++

Description

    This function will free storage for the device completion request.

Parameters

    Entry - Pointer to the device completion request.

Return

    None.

--*/

{

    //
    // This routine must be non-paged because it runs during the device
    // completion routine.
    //

    if (InterlockedDecrement(&Entry->ReferenceCount) == 0) {
        ExFreePoolWithTag(Entry, PNP_POOLTAG_DEVICE_COMPLETION_REQUEST);
    }

    return;
}


NTSTATUS
PnpDeviceCompletionProcessCompletedRequest (
    __inout PPNP_DEVICE_COMPLETION_REQUEST CompletedRequest
    )

/*++

Description

    This function performs the post-processing and sets the devnode state to the
    completionState once the PNP IRP completes.

Parameters

    CompletedRequest - Pointer to the complete entry to be processed.

Return

    NTSTATUS.

--*/

{
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    //
    // Extract the IRP status and information.
    //
    deviceNode = CompletedRequest->DeviceNode;
    deviceNode->OverUsed1.Information = CompletedRequest->Information;
    deviceNode->CompletionStatus = CompletedRequest->Status;

    if (CompletedRequest->IrpPended) {

        IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                     "%s IRP was pended for %wZ\n",
                     (CompletedRequest->CompletionState == DeviceNodeEnumerateCompletion)? "Enumeration" : ((CompletedRequest->CompletionState == DeviceNodeRestartCompletion)? "Restart" : "Start"),
                     PnpGetDeviceInstancePath(deviceNode)));
    }

    //
    // Update the devnode state.
    //
    PipSetDevNodeState(deviceNode, CompletedRequest->CompletionState, NULL);

    //
    // Do additional processing for IRP_MN_START completion.
    //

    if (NT_SUCCESS(CompletedRequest->Status) &&
        ((CompletedRequest->CompletionState == DeviceNodeStartCompletion) ||
        (CompletedRequest->CompletionState == DeviceNodeRestartCompletion))) {
     
       PipUpdatePostStartCharacteristics(deviceNode->PhysicalDeviceObject);

       //
       // Clear DEVPKEY_Device_DriverProblemDesc
       //

       _PnpSetObjectProperty(PiPnpRtlCtx,
                             PnpGetDeviceInstancePath(deviceNode)->Buffer,
                             PNP_OBJECT_TYPE_DEVICE,
                             NULL,
                             NULL,
                             &DEVPKEY_Device_DriverProblemDesc,
                             DEVPROP_TYPE_EMPTY,
                             NULL,
                             0,
                             0);

       //
       // Allow software device creation to enumerate
       // software devices children.
       //

        PiSwProcessParentStartIrp(deviceNode->PhysicalDeviceObject);
    }

    //
    // If this is an enumeration completion, undo the previous device
    // activation.
    // 
    
    if (CompletedRequest->CompletionState == DeviceNodeEnumerateCompletion) {

        if ((deviceNode->StateFlags & DNSF_POFX_BUS_RELATIONS) == 0) {
            KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                          PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE,
                          (ULONG_PTR) deviceNode,
                          DNSF_POFX_BUS_RELATIONS,
                          0);

        }

        PoFxIdleDevice(deviceNode->PhysicalDeviceObject);
        deviceNode->StateFlags &= ~DNSF_POFX_BUS_RELATIONS;
    }

    //
    // Cleanup the entry.
    //
    PnpDeviceCompletionRequestDestroy(CompletedRequest);

    return deviceNode->CompletionStatus;
}

NTSTATUS
PnpDeviceCompletionProcessCompletedRequests (
    __in LOGICAL                      All,
    __in LOGICAL                      Wait,
    __out LOGICAL                     *Completed
    )

/*++

Description

    This routines processes completed requests as specified.

Parameters

    All - Specifies whether all completed entries are to be removed or any.

    Wait - Specified if its OK to wait for a request to complete.

    Completed - Pointer that recieves result if any completed operation was processed.

Return

    NTSTATUS.

--*/

{
    NTSTATUS status, finalStatus;
    PLIST_ENTRY entry;
    PPNP_DEVICE_COMPLETION_REQUEST completedEntry;

    PAGED_CODE();

    PNP_ASSERT(!(All != FALSE && Wait == FALSE));
    PNP_ASSERT(Completed);

    *Completed = FALSE;
    finalStatus = STATUS_SUCCESS;

    for (;;) {

        if (PnpDeviceCompletionQueueIsEmpty(&PnpDeviceCompletionQueue)) {

            //
            // Nothing in dispatched or completed queue - we are done.
            //
            break;
        }

        if (!PnpDeviceCompletionQueueGetCompletedCount(&PnpDeviceCompletionQueue)) {

            //
            // If we are not told to wait or we are told to get any and we got at least
            // one, then bail.
            //
            if (!Wait || (!All && (*Completed) != FALSE)) {

                break;
            }
        }

        //
        // Get the completed operation (waiting if neccessary).
        //
        entry = PnpDeviceCompletionQueueGetCompletedRequest(&PnpDeviceCompletionQueue);
        completedEntry = CONTAINING_RECORD(entry,
                                           PNP_DEVICE_COMPLETION_REQUEST,
                                           ListEntry);

        //
        // Process the completed entry.
        //
        status = PnpDeviceCompletionProcessCompletedRequest(completedEntry);
        if (status == STATUS_PNP_RESTART_ENUMERATION) {

            finalStatus = status;
        }

        //
        // We found at least one completed operation.
        //
        *Completed = TRUE;
    }

    return finalStatus;
}

#include <d3d10_1.h> // needed for SRGB conversions
#include <d3dx10.h>  // needed for BC texture format conversion
#include "FormatDesc.hpp"
#include <d3d10wrapped.h>
#include <D3DDepthHelper.h>

#include "GetDispSurfData.hpp"


#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))

// Andy Hartman - TODO: ideally most of this functionality will be exposed through framework or d3dx

namespace Texture_ALU
{

struct Vec4
{
public:
	static const UINT R = 0;
	static const UINT G = 1;
	static const UINT B = 2;
	static const UINT A = 3;
	union
	{
		UINT u[4];
		INT32  i[4];
		float f[4];
	};
};

float
round_z( float a )
{
	modff( a, &a );
	return a;
}

double
round_z( double a )
{
	modf( a, &a );
	return a;
}

float
flush( float a )
{
	static const minNormalizedFloat = 0x00800000;
	static const signBit = 0x80000000;
	static const signBitComplement = 0x7fffffff;
	UINT b = (*(UINT*)&a) & signBitComplement; // fabs()
	if( b < minNormalizedFloat ) // UINT comparison. NaN/INF do test false here
	{
		b = signBit & (*(UINT*)&a);
		return *(float*)&b;
	}
	return a;
}

float
add( float a, float b )
{
	return flush( flush( a ) + flush( b ) );
}

float
sub( float a, float b )
{
	return flush( flush( a ) - flush( b ) );
}

float
mul( float a, float b )
{
	return flush( flush( a )* flush( b ) );
}

float
exp( float a )
{
	return flush( powf( 2, flush( a ) ) );
}

UINT 
ge( float a, float b )
{
	return (flush(a) >= flush(b)) ? 0xffffffff : 0;
}

float 
div( float a, float b )
{
	return flush( flush( a ) / flush( b ) );
}

float
linearToGamma( float a )
{
	if( ( a < D3D10_FLOAT_TO_SRGB_THRESHOLD ) )
	{
		return ( a * D3D10_FLOAT_TO_SRGB_SCALE_1 );
	}
	else
	{
		static const float scale = D3D10_FLOAT_TO_SRGB_EXPONENT_NUMERATOR/D3D10_FLOAT_TO_SRGB_EXPONENT_DENOMINATOR;
		float temp = pow( a, scale );
		temp = ( temp * D3D10_FLOAT_TO_SRGB_SCALE_2 );
		return ( temp - D3D10_FLOAT_TO_SRGB_OFFSET );
	}
}

float
gammaToLinear( float a )
{
	if( ( a < D3D10_SRGB_TO_FLOAT_THRESHOLD ) )
	{
		return ( a / D3D10_SRGB_TO_FLOAT_DENOMINATOR_1 );
	}
	else
	{
		float temp = ( a + D3D10_SRGB_TO_FLOAT_OFFSET );
		temp = ( temp / D3D10_SRGB_TO_FLOAT_DENOMINATOR_2 );
		return pow( temp, D3D10_SRGB_TO_FLOAT_EXPONENT );
	}
}

BOOL
isNaN( float a )
{
	static const exponentMask = 0x7f800000;
	static const mantissaMask = 0x007fffff;
	UINT u = *(UINT*)&a;
	return ( ( ( u & exponentMask ) == exponentMask ) && ( u & mantissaMask ) ); // NaN
}

UINT
float32_to_uint32( float a )
{
	a = round_z( a );
	if( a < 0 )
	{
		return 0;
	}
	else
	{
		return (UINT32)a;
	}
}

UINT
float64_to_uint32( double a )
{
	a = round_z( a );
	if( a < 0 )
	{
		return 0;
	}
	else
	{
		return (UINT32)a;
	}
}

UINT8
float32_to_unorm2( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 3 ); // 3 == ( 1 << 2 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm8( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 255 ); // 255 == ( 1 << 8 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT8
float32_to_unorm8_srgb( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = linearToGamma( a );

	a = mul( a, 255 ); // 255 == ( 1 << 8 ) - 1

	a = add( a, 0.5 );

	return UINT8( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm10( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 1023 ); // 1023 == ( 1 << 10 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm11( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 2047 ); // 2047 == ( 1 << 11 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT16
float32_to_unorm16( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ 0.0f, 1.0f ]
	a = max( min( a, 1.0f ), 0.0f );

	a = mul( a, 65535 ); // 65535 == ( 1 << 16 ) - 1

	a = add( a, 0.5 );

	return UINT16( float32_to_uint32( a ) ); // rounds to zero
}

UINT
float32_to_unorm24( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// need more precision since unorm24 has more precision bits than float32
	double a2 = max(min(a, 1.0f), 0.0f);
	a2 = a2 * 16777215;
	a2 = a2 + 0.5;
	return UINT( float64_to_uint32( a2 ) ); // rounds to zero
}

INT32
float32_to_sint32( float a )
{
	a = round_z( a );
	return (INT32)a;
}

INT8
float32_to_snorm8( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ -1.0f, 1.0f ]
	a = max( min( a, 1.0f ), -1.0f );

	a = mul( a, 127 ); // 127 == ( 1 << ( 8 - 1 ) ) - 1

	if( ge( a, 0 ) )
	{
		a = add( a, 0.5f );
	}
	else
	{
		a = sub( a, 0.5f );
	}

	return INT8( float32_to_sint32( a ) ); // rounds to zero
}

INT16 
float32_to_snorm16( float a )
{
	if( isNaN( a ) )
	{
		return 0;
	}

	// Clamp to [ -1.0f, 1.0f ]
	a = max( min( a, 1.0f ), -1.0f );

	a = mul( a, 32767 ); // 32767 == ( 1 << ( 16 - 1 ) ) - 1

	if( ge( a, 0 ) )
	{
		a = add( a, 0.5f );
	}
	else
	{
		a = sub( a, 0.5f );
	}

	return INT16( float32_to_sint32( a ) ); // rounds to zero
}



float
uint32_to_float32( UINT a )
{
	return (float)a;
}

float
unorm2_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 3 ); // 3 = ( 1 << 2 ) - 1
}

float 
unorm8_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 255 ); // 255 = ( 1 << 8 ) - 1
}

float
unorm8_srgb_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	b = div( b, 255); // 255 = ( 1 << 8 ) - 1
	return gammaToLinear( b );
}

float
unorm10_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 1023 ); // 1023 = ( 1 << 10 ) - 1
}

float
unorm11_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 2047 ); // 2047 = ( 1 << 11 ) - 1
}

float
unorm16_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 65535 ); // 65535 = ( 1 << 16 ) - 1
}

float
unorm24_to_float32( UINT a )
{
	float b = uint32_to_float32( a );
	return div( b, 16777215 ); // 16777215 = ( 1 << 24 ) - 1
}

float
sint32_to_float32( INT32 a )
{
	return (float)a;
}

float
snorm8_to_float32( INT3