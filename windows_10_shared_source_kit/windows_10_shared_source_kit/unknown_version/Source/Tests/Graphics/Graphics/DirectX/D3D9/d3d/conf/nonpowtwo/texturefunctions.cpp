#include "CD3DX.h"
#include "nonpowtwotest.h"
#include "CTextures.h"

#define STRIPE_COUNT 4
													
//------------------------------------------------------------------------------

bool CNonPowTwoTest::FilterTextureInfo(const DEVICEDESC& Caps, STextureInfo& TexInfo)
{
	if(TEXTURE_3D == TexInfo.TexType)
	{
		if( KeySet(_T("NoVolume")) || !(Caps.dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) )
			return false;

		if( 0 == TexInfo.uMipCount && !(Caps.dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP) )
			TexInfo.uMipCount = 1;

		if (m_bVolumePow2 && !KeySet("pow2"))
			return false;
	}

	if(TEXTURE_CUBE == TexInfo.TexType)
	{
		if( KeySet(_T("NoCube")) || !(Caps.dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP) )
			return false;

		if( 0 == TexInfo.uMipCount && !(Caps.dwTextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP) )
			TexInfo.uMipCount = 1;

		if (m_bCubePow2 && !KeySet("pow2"))
			return false;
	}

    if(TEXTURE_2D == TexInfo.TexType)
	{
        if(KeySet(_T("No2D")))
            return false;

		if( 0 == TexInfo.uMipCount && !(Caps.dwTextureCaps & D3DPTEXTURECAPS_MIPMAP) )
			TexInfo.uMipCount = 1;

		// The A1 texture format can not have mip maps
		if (FMT_A1 == TexInfo.Format.d3dfFormat)
			TexInfo.uMipCount = 1;

		//only create texture with mip maps if the device supports nonpow2 undconditionally
		if(m_bNonPow2Cond && (TexInfo.uMipCount != 1) )
		{
			return false;
		}

		//can only use nonPow2 textures with 1 level and fmt != DXTn unless commend key -pow2 is set
		if ( m_bTexPow2 &&  m_bNonPow2Cond && !KeySet("pow2"))
		{
			TexInfo.uMipCount = 1;
			
			if ( IsDXTN(TexInfo.Format) )
				return false;
		}
		
		//this case should be aborted in Setup because it's a wrong combination of caps
		//however we'll check it just in case
		if (!m_bTexPow2 &&  m_bNonPow2Cond)
			return false;

		//can onl use nonPow2 textures if commend key -pow2 is set
		if (m_bTexPow2 && !m_bNonPow2Cond && !KeySet("pow2"))
			return false;
	}

	return true;
}

//------------------------------------------------------------------------------

void CNonPowTwoTest::CleanupTextures()
{
	for(UINT i = 0; i < TEXTURE_COUNT; i++)
	{
		RELEASE(g_pTextures[i]);
	}	

	//in case if reset is called then we need to specify 0 or 1 for number of Levels
	for (UINT i = 0; i < TEXTURE_COUNT; i++)
	{
		if ( g_TexInfo[i].uMipCount > 1)
		g_TexInfo[i].uMipCount = 0;
	}
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::InitializeTextures()
{
	CleanupTextures();

	for(UINT i = 0; i < TEXTURE_COUNT; i++)
	{
		CnBaseTexture* pTexture = NULL;
	
		const DEVICEDESC* pCaps = m_pSrcDevice->GetCaps();

		//randomly choose pool
		while (true)
		{
			DWORD pool = rand() % 3;
			if (pool == D3DPOOL_SYSTEMMEM && !( pCaps->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY ))
			{
				continue;
			} 

			g_TexInfo[i].Pool = pool;
			break;
		}

		//only attemp to create textures supported by this device
		if(FilterTextureInfo(*pCaps, g_TexInfo[i])) 
		{
			switch(g_TexInfo[i].TexType)
			{
			case TEXTURE_2D:   pTexture = Create2DTexture(i+m_uCurrentMode-1, g_TexInfo[i]); break;

			case TEXTURE_CUBE: pTexture = CreateCubeMap  (i+m_uCurrentMode-1, g_TexInfo[i]); break;

			case TEXTURE_3D:   pTexture = Create3DTexture(i+m_uCurrentMode-1, g_TexInfo[i]); break;

			default:           return false;
			}

			if (GetLastError() != S_OK)
				return false;
		}

		g_pTextures[i] = pTexture;
	}

	return true;
}

//------------------------------------------------------------------------------

CnBaseTexture* CNonPowTwoTest::Create2DTexture(const UINT index, STextureInfo& TexInfo)
{
	CnTexture*  pTexture = NULL;
	DEVICEDESC* pCaps    = NULL;
	DWORD       dwUsage  = 0;
	UINT        maxW     = 0; 
	UINT        maxH     = 0;
	UINT        minW     = 0; 
	UINT        minH     = 0;

	pCaps = m_pSrcDevice->GetCaps();

	if (TexInfo.uMipCount == 1)
	{
		maxW = min( m_uRectWidth /2, min(1024, pCaps->dwMaxTextureWidth ) );
		maxH = min( m_uRectHeight/2, min(1024, pCaps->dwMaxTextureHeight) );
		minW = pCaps->dwMinTextureWidth;
		minH = pCaps->dwMinTextureHeight;
	}
	else
	{
		maxW = min( 1024, pCaps->dwMaxTextureWidth );
		maxH = min( 1024, pCaps->dwMaxTextureHeight);
		minW = max( m_uRectWidth /2, pCaps->dwMinTextureWidth  );
		minH = max( m_uRectHeight/2, pCaps->dwMinTextureHeight );
	}

	// The A1 texture format has a height restriction of 2048
	if ( FMT_A1 == m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat )
	{
		maxH = min( 2048, maxH );
	}

	UINT uWidth  = 1 + rand() % maxW;
	UINT uHeight = 1 + rand() % maxH;
	
	uWidth  = max( uWidth,  minW );
	uHeight = max( uHeight, minH );

	if (KeySet("fixed"))
	{
		uWidth = m_uRectWidth;
		uHeight = m_uRectHeight;
	}

	if(KeySet(_T("POW2")))// || (pCaps->dwTextureCaps & D3DPTEXTURECAPS_POW2))
	{
		uWidth  = ForcePOW2(uWidth );
		uHeight = ForcePOW2(uHeight);
	}

	if (pCaps->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
		uWidth = uHeight;

	if(IsDXTN(TexInfo.Format))
    {
        uWidth  = ForceMultipleFour(uWidth );
        uHeight = ForceMultipleFour(uHeight);
    }

	if( FMT_UYVY == TexInfo.Format.d3dfFormat ||
		FMT_YUY2 == TexInfo.Format.d3dfFormat  )
	{
		uWidth  = max((uWidth >>1) << 1, 2); //max((uWidth /2)*2, 2);
		uHeight = max((uHeight>>1) << 1, 2); //max((uHeight/2)*2, 2);
	}

	// The A1 texture format must have a width in multiples of 32
	if (FMT_A1 == TexInfo.Format.d3dfFormat)
	{
        uWidth  = ForceDWORDMultiple(uWidth);
    }

	//can use demensions or pool specified in command line
	if (index - (m_uCurrentMode-1) == 0)
	{
		int t;
		if (ReadInteger("tw0", &t))
			uWidth = t;
		if (ReadInteger("th0", &t))
			uHeight = t;
		if (ReadInteger("tl0", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("tp0", &t))
			TexInfo.Pool = t;
	}
	if (index - (m_uCurrentMode-1) == 1)
	{
		int t;
		if (ReadInteger("tw1", &t))
			uWidth = t;
		if (ReadInteger("th1", &t))
			uHeight = t;
		if (ReadInteger("tl1", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("tp1", &t))
			TexInfo.Pool = t;
	}

	if (KeySet(_T("nomipmap2d")) || (m_pD3D->GetRuntimeVersion() >= 9.029f && 
		FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                         USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, TexInfo.Format)) ))
		TexInfo.uMipCount = 1;

	//creating texture we decrease the demensions if we run out of memory
	while (uWidth >= 1 && uHeight >= 1)
	{
		if(!CreateTexture(uWidth, uHeight, TexInfo.uMipCount, dwUsage, TexInfo.Format, TexInfo.Pool,  &pTexture))
		{
			HRESULT hr = GetLastError();
			if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				uWidth  = uWidth  >> 1;
				uHeight = uHeight >> 1;
				continue;
			}
			
			WriteToLog("Failed to create a 2D texture %dx%d %d with hr=%s(%x).", uWidth, uHeight, TexInfo.uMipCount, m_pD3D->HResultToString(hr), hr);
			SetLastError(hr);
			return NULL; //any other errors but out-of-memory return NULL
		}

		TexInfo.uWidth  = uWidth;
		TexInfo.uHeight = uHeight;
		break;
	}

	if ( NULL == pTexture )
		return NULL;

	if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

	TexInfo.uMipCount = pTexture->GetLevelCount();

	bool bResultVal = true;

	for(UINT uLevel = 0; uLevel < TexInfo.uMipCount; uLevel++)
	{
		if (KeySet("stripes"))
			bResultVal &= FillTextureLevelStripes(index, uLevel, pTexture);
		else if (KeySet("grad"))
			bResultVal &= FillTextureLevelGradient(index, uLevel, pTexture);
		else if (KeySet("image"))
			bResultVal &= FillTextureLevelImage(uLevel, pTexture, _T("tex0.bmp"));
		else
		switch((uLevel+index) % 3)
		{
		case 0:
			bResultVal &= FillTextureLevelImage(uLevel, pTexture, KeySet("tex1") ? _T("tex1.BMP") : _T("tex0.BMP"));
			break;
		case 1:
			bResultVal &= FillTextureLevelGradient(index, uLevel, pTexture);
			break;
		case 2:
			bResultVal &= FillTextureLevelStripes(index, uLevel, pTexture);
			break;
		}
	}

	if(bResultVal)
	{
		return pTexture;
	}
	else
	{
		pTexture->Release();
		return NULL;
	}
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillTextureLevelGradient(const UINT index, const UINT uLevel, CnTexture* pTexture)
{
	if(NULL == pTexture)
	{
		return false;
	}

	SURFACEDESC LevelDesc;

	if(!(pTexture->GetLevelDesc(uLevel, &LevelDesc)))
	{
		return false;
	}

	WORD i = index + uLevel;
	DWORD dwColors[4];

	if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
		 LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
		 LevelDesc.Format.d3dfFormat == FMT_A8 )
	{
		for(UINT k = 0; k < 4; k++)
			dwColors[(i+k)%4] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
	}
	else
	{
		dwColors[(i+0)%4] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
		dwColors[(i+1)%4] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
		dwColors[(i+2)%4] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
		dwColors[(i+3)%4] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
	}

	return pTexture->LoadGradient(uLevel+1, LevelDesc.dwWidth, LevelDesc.dwHeight, dwColors);
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillTextureLevelStripes(const UINT index, const UINT uLevel, CnTexture* pTexture)
{	
	if(NULL == pTexture)
	{
		return false;
	}

	SURFACEDESC LevelDesc;

	if(!(pTexture->GetLevelDesc(uLevel, &LevelDesc)))
	{
		return false;
	}

	WORD i = index + uLevel;
	DWORD dwStripes[STRIPE_COUNT];

	if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
		 LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
		 LevelDesc.Format.d3dfFormat == FMT_A8 )
	{
		for(UINT k = 0; k < STRIPE_COUNT; k++)
			dwStripes[(i+k)%STRIPE_COUNT] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
	}
	else
	{
		dwStripes[(i+0)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
		dwStripes[(i+1)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
		dwStripes[(i+2)%STRIPE_COUNT] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
		dwStripes[(i+3)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
	}
	
	//in the framework, level 0 means the whole chain while level 1 is the base surface level
	return pTexture->LoadStripes(uLevel+1, LevelDesc.dwWidth, LevelDesc.dwHeight,
								 STRIPE_COUNT, dwStripes, (uLevel%2 == 0 ? true : false), (uLevel%2 == 0 ? false : true));
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillTextureLevelImage(const UINT uLevel, CnTexture* pTexture, const TCHAR* szFileName)
{
	if(NULL == pTexture || NULL == szFileName)
	{
		return false;
	}

	//in the framework, level 0 means the whole chain while level 1 is the base surface level
	pTexture->LoadTexture(uLevel+1, szFileName, 0.2f);

	return true;
}

//------------------------------------------------------------------------------

CnBaseTexture* CNonPowTwoTest::Create3DTexture(const UINT index, STextureInfo& TexInfo)
{
	DEVICEDESC*   pCaps    = NULL;
	CnVolTexture* pTexture = NULL;
	DWORD         dwUsage  = 0;
	UINT          maxW     = 0; 
	UINT          maxH     = 0;
	UINT          maxD     = 0;
	UINT          minW     = 0; 
	UINT          minH     = 0;
	UINT          minD     = 0;

	if(NULL == m_pCommonVolumeFormats)
    {
        return NULL;
    }

	pCaps = m_pSrcDevice->GetCaps();

	if (TexInfo.uMipCount == 1)
	{
		maxW = min( m_uRectWidth /2, min(1024, pCaps->dwMaxVolumeExtent) );
		maxH = min( m_uRectHeight/2, min(1024, pCaps->dwMaxVolumeExtent) );
		maxD = min( 64, pCaps->dwMaxVolumeExtent );
		minW = pCaps->dwMinVolumeExtent;
		minH = pCaps->dwMinVolumeExtent;
		minD = pCaps->dwMinVolumeExtent;
	}
	else
	{
		maxW = min( 512, pCaps->dwMaxVolumeExtent );
		maxH = min( 512, pCaps->dwMaxVolumeExtent );
		maxD = min(  32, pCaps->dwMaxVolumeExtent );
		minW = max( m_uRectWidth /2, pCaps->dwMinVolumeExtent );
		minH = max( m_uRectHeight/2, pCaps->dwMinVolumeExtent );
		minH = pCaps->dwMinVolumeExtent;
	}

	UINT uWidth  = 1 + rand() % maxW;
	UINT uHeight = 1 + rand() % maxH;
	UINT uDepth  = 1 + rand() % maxD;
	
	uWidth  = max( uWidth,  minW );
	uHeight = max( uHeight, minH );
	uDepth  = max( uDepth,  minD);

	if(KeySet(_T("POW2")))//) || m_bVolumePow2)
	{
		uWidth  = ForcePOW2(uWidth );
		uHeight = ForcePOW2(uHeight);
		uDepth  = ForcePOW2(uDepth );
	}

	if (pCaps->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
		uWidth = uHeight = uDepth;

   if(IsDXTN(TexInfo.Format))
    {
        uWidth  = ForceMultipleFour(uWidth );
        uHeight = ForceMultipleFour(uHeight);
        uDepth  = ForceMultipleFour(uDepth );

		//bug 85033: Crash on releasing mipmapped DXTn volume textures with DEFAULT pool and different nonpowtwo sizes
		if ( m_pD3D->GetRuntimeVersion() < 9.109f )
			TexInfo.Pool = POOL_MANAGED;
    }

	if( FMT_UYVY == TexInfo.Format.d3dfFormat ||
		FMT_YUY2 == TexInfo.Format.d3dfFormat  )
	{
		uWidth  = max((uWidth >>1) << 1, 2); //max((uWidth /2)*2, 2);
		uHeight = max((uHeight>>1) << 1, 2); //max((uHeight/2)*2, 2);
		uDepth  = max((uDepth >>1) << 1, 2); //max((uDepth /2)*2, 2);
	}

	//can use demensions or pool specified in command line
	if (index - (m_uCurrentMode-1) == 4)
	{
		int t;
		if (ReadInteger("vw0", &t))
			uWidth = t;
		if (ReadInteger("vh0", &t))
			uHeight = t;
		if (ReadInteger("vd0", &t))
			uDepth = t;
		if (ReadInteger("vl0", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("vp0", &t))
			TexInfo.Pool = t;
	}
	if (index - (m_uCurrentMode-1) == 5)
	{
		int t;
		if (ReadInteger("vw1", &t))
			uWidth = t;
		if (ReadInteger("vh1", &t))
			uHeight = t;
		if (ReadInteger("vd1", &t))
			uDepth = t;
		if (ReadInteger("vl1", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("vp1", &t))
			TexInfo.Pool = t;
	}

	if (KeySet(_T("nomipmapvol")) || (m_pD3D->GetRuntimeVersion() >= 9.029f && 
		FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                         USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, TexInfo.Format)) ))
		TexInfo.uMipCount = 1;
	
	//creating texture we decrease the demensions if we run out of memory
	while (uWidth >= 1 && uHeight >= 1 && uDepth >=1)
	{
		if(!CreateVolumeTexture(uWidth, uHeight, uDepth, TexInfo.uMipCount, dwUsage, TexInfo.Format, TexInfo.Pool,  &pTexture))
		{
			HRESULT hr = GetLastError();
			if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				uWidth  = uWidth  >> 1;
				uHeight = uHeight >> 1;
				uDepth  = uDepth  >> 1;
				continue;
			}
			
			WriteToLog("Failed to create a 3D texture %dx%dx%d %d with hr=%s(%x).", uWidth, uHeight, uDepth, TexInfo.uMipCount, m_pD3D->HResultToString(hr), hr);
			SetLastError(hr);
			return NULL; //any other errors but out-of-memory return NULL
		}

		TexInfo.uWidth  = uWidth;
		TexInfo.uHeight = uHeight;
		TexInfo.uDepth  = uDepth;
		break;
	}

	if ( NULL == pTexture )
		return NULL;

	if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

	TexInfo.uMipCount = pTexture->GetLevelCount();
	
	bool bResultVal = true;

	for(UINT uLevel = 0; uLevel < TexInfo.uMipCount; uLevel++)
	{
		if (KeySet("stripes"))
			bResultVal &= FillVolumeWithStripes(index, uLevel, pTexture);
		else if (KeySet("grad"))
			bResultVal &= FillVolumeWithGradient(index, uLevel, pTexture);
		else if (KeySet("image"))
			bResultVal &= FillVolumeWithImage(uLevel, pTexture, _T("tex2.bmp"));
		else
		switch((uLevel+index) % 3)
		{
		case 0:
			bResultVal &= FillVolumeWithImage(uLevel, pTexture, _T("tex2.BMP"));
			break;
		case 1:
			bResultVal &= FillVolumeWithGradient(index, uLevel, pTexture);
			break;
		case 2:
			bResultVal &= FillVolumeWithStripes(index, uLevel, pTexture);
			break;
		}
	}

	if(bResultVal)
	{
		return pTexture;
	}
	else
	{
		RELEASE(pTexture);
		return NULL;
	}
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillVolumeWithGradient(const UINT index, const UINT uLevel, CnVolTexture* pTexture)
{
	if(NULL == pTexture)
	{
		return false;
	}

	VOLUME_DESC LevelDesc;

	if(!pTexture->GetLevelDesc(uLevel, &LevelDesc))
	{
		return false;
	}

	for(UINT nSlice = 0; nSlice<LevelDesc.Depth; nSlice++)
	{
		WORD i = index + uLevel + nSlice;
		DWORD dwColors[4];

		if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
			LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
			LevelDesc.Format.d3dfFormat == FMT_A8 )
		{
			for(UINT k = 0; k < 4; k++)
				dwColors[(i+k)%4] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
		}
		else
		{
			dwColors[(i+0)%4] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
			dwColors[(i+1)%4] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
			dwColors[(i+2)%4] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
			dwColors[(i+3)%4] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
		}

		if(!pTexture->LoadGradient(uLevel + 1, nSlice, LevelDesc.Width, LevelDesc.Height, dwColors))
		{
			return false;
		}
	}
	
	return true;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillVolumeWithStripes(const UINT index, const UINT uLevel, CnVolTexture* pTexture)
{
	if(NULL == pTexture)
	{
		return false;
	}

	VOLUME_DESC LevelDesc;

	if(!pTexture->GetLevelDesc(uLevel, &LevelDesc))
	{
		return false;
	}

	bool bResult = true;

	for(UINT nSlice = 0; nSlice < LevelDesc.Depth; nSlice++)
	{
		WORD i = index + uLevel + nSlice;
		DWORD dwStripes[STRIPE_COUNT];

		if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
			 LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
			 LevelDesc.Format.d3dfFormat == FMT_A8 )
		{
			for(UINT k = 0; k < STRIPE_COUNT; k++)
				dwStripes[(i+k)%STRIPE_COUNT] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
		}
		else
		{
			dwStripes[(i+0)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
			dwStripes[(i+1)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
			dwStripes[(i+2)%STRIPE_COUNT] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
			dwStripes[(i+3)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
		}
			
		bResult &= pTexture->LoadStripes(uLevel+1, nSlice, LevelDesc.Width, LevelDesc.Height, STRIPE_COUNT, dwStripes, (uLevel%2 == 0 ? true : false), (uLevel%2 == 0 ? false : true));
	}

	return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillVolumeWithImage(const UINT uLevel, CnVolTexture* pTexture, const TCHAR* szFileName)
{
	if(NULL == szFileName || NULL == pTexture)
	{
		return false;
	}
	
	bool bResult = true;

	VOLUME_DESC LevelDesc;

	if(!pTexture->GetLevelDesc(uLevel, &LevelDesc))
	{
		return false;
	}

	for(UINT nSlice = 0; nSlice < LevelDesc.Depth; nSlice++)
	{
		bResult &= pTexture->LoadTexture(uLevel+1, nSlice, szFileName, 0.2f);
	}

	return bResult;
}

//------------------------------------------------------------------------------

CnBaseTexture* CNonPowTwoTest::CreateCubeMap(const UINT index, STextureInfo& TexInfo)
{
	DEVICEDESC*    pCaps    = NULL;
	CnCubeTexture* pTexture = NULL;
	DWORD          dwUsage  = 0;
	UINT           maxW     = 0; 
	UINT           minW     = 0; 

	if(NULL == m_pCommonCubeFormats)
    {
        return NULL;
    }

	pCaps = m_pSrcDevice->GetCaps();

	if (TexInfo.uMipCount == 1)
	{
		maxW = min( m_uRectWidth/2, min(pCaps->dwMaxTextureWidth, min(512, pCaps->dwMaxTextureHeight)) );
		minW = max( pCaps->dwMinTextureWidth, pCaps->dwMinTextureHeight );
	}
	else
	{
		maxW = min( pCaps->dwMaxTextureWidth, min(512, pCaps->dwMaxTextureHeight) );
		minW = max( m_uRectWidth/2, max(pCaps->dwMinTextureWidth, pCaps->dwMinTextureHeight) );
	}
	
	UINT uWidth  = 1 + rand() % maxW;

	uWidth  = max( uWidth,  minW);

	if(KeySet(_T("POW2")))// || m_bCubePow2)
	{
		uWidth  = ForcePOW2(uWidth);
	}

    if(IsDXTN(TexInfo.Format))
    {
        uWidth  = ForceMultipleFour(uWidth);
    }

	if( FMT_UYVY == TexInfo.Format.d3dfFormat ||
		FMT_YUY2 == TexInfo.Format.d3dfFormat  )
	{
		uWidth  = max((uWidth >>1) << 1, 2); //max((uWidth /2)*2, 2);
	}

	//can use demensions specified in command line
	if (index - (m_uCurrentMode-1) == 2)
	{
		int t;
		if (ReadInteger("cw0", &t))
			uWidth = t;
		if (ReadInteger("cl0", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("cp0", &t))
			TexInfo.Pool = t;
	}
	if (index - (m_uCurrentMode-1) == 3)
	{
		int t;
		if (ReadInteger("cw1", &t))
			uWidth = t;
		if (ReadInteger("cl1", &t))
			TexInfo.uMipCount = t;
		if (ReadInteger("cp1", &t))
			TexInfo.Pool = t;
	}

	if (KeySet(_T("nomipmapcube")) || (m_pD3D->GetRuntimeVersion() >= 9.029f && 
		FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
                                         USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, TexInfo.Format)) ))
		TexInfo.uMipCount = 1;

	//creating texture we decrease the demensions if we run out of memory
	while ( uWidth >= 1 )
	{
		if(!CreateCubeTexture(uWidth, TexInfo.uMipCount, dwUsage, TexInfo.Format, TexInfo.Pool,  &pTexture))
		{
			HRESULT hr = GetLastError();
			if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
			{
				uWidth  = uWidth  >> 1;
				continue;
			}

			WriteToLog("Failed to create a cube texture %d %d with hr=%s(%x).", uWidth, TexInfo.uMipCount, m_pD3D->HResultToString(hr), hr);
			SetLastError(hr);
			return NULL; //any other errors but out-of-memory return NULL
		}

		TexInfo.uWidth  = uWidth;
		break;
	}

	if ( NULL == pTexture )
		return NULL;

	if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

	TexInfo.uMipCount = pTexture->GetLevelCount();

	bool bResultVal = true;

	for(UINT uLevel = 0; uLevel < TexInfo.uMipCount; uLevel++)
	{
		if (KeySet("stripes"))
			bResultVal &= FillCubeWithStripes(index, uLevel, pTexture);
		else if (KeySet("grad"))
			bResultVal &= FillCubeWithGradient(index, uLevel, pTexture);
		else if (KeySet("image"))
			bResultVal &= FillCubeWithImage(uLevel, pTexture, _T("tex1.bmp"));
		else	
		switch((uLevel+index) % 3)
		{
			case 0:
			bResultVal &= FillCubeWithImage(uLevel, pTexture, _T("ENV1.BMP"));
			break;
		case 1:
			bResultVal &= FillCubeWithGradient(index, uLevel, pTexture);
			break;
		case 2:
			bResultVal &= FillCubeWithStripes(index, uLevel, pTexture);
			break;
		}
	}

	if(bResultVal)
	{
		return pTexture;
	}
	else
	{
		RELEASE(pTexture);
		return NULL;
	}
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillCubeWithGradient(const UINT index, const UINT uLevel, CnCubeTexture* pTexture)
{
	if(NULL == pTexture)
	{
		return false;
	}

	bool bResult = true;

	SURFACEDESC LevelDesc;

	if(!pTexture->GetLevelDesc(uLevel, &LevelDesc))
	{
		return false;
	}

	for(UINT uFace = 0; uFace < 6; uFace++)
	{
		WORD i = index + uLevel + uFace;
		DWORD dwColors[4];

		if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
			 LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
			 LevelDesc.Format.d3dfFormat == FMT_A8 )
		{
			for(UINT k = 0; k < 4; k++)
				dwColors[(i+k)%4] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
		}
		else
		{
			dwColors[(i+0)%4] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
			dwColors[(i+1)%4] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
			dwColors[(i+2)%4] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
			dwColors[(i+3)%4] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
		}

		bResult &= pTexture->LoadGradient(uLevel + 1, (CUBEMAP_FACES)uFace, LevelDesc.Width, LevelDesc.Height, dwColors);
	}
	
	return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillCubeWithStripes(const UINT index, const UINT uLevel, CnCubeTexture* pTexture)
{
	if(NULL == pTexture)
	{
		return false;
	}

	SURFACEDESC LevelDesc;

	if(!pTexture->GetLevelDesc(uLevel, &LevelDesc))
	{
		return false;
	}

	bool bResult = true;

	for(UINT uFace = 0; uFace < 6; uFace++)
	{
		WORD i = index + uLevel + uFace;
		DWORD dwStripes[STRIPE_COUNT];

		if ((LevelDesc.Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) || 
			LevelDesc.Format.d3dfFormat == FMT_R16F || LevelDesc.Format.d3dfFormat == FMT_R32F ||
			LevelDesc.Format.d3dfFormat == FMT_A8 )
		{
			for(UINT k = 0; k < STRIPE_COUNT; k++)
				dwStripes[(i+k)%STRIPE_COUNT] = RGBA_MAKE(0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k), 0xff - 20*(i+k));
		}
		else
		{
			dwStripes[(i+0)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0), 0xff - 10*(i+0));  // grey
			dwStripes[(i+1)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+1), 0xff - 10*(i+1),              0 , 0xff - 10*(i+1));	// yellow
			dwStripes[(i+2)%STRIPE_COUNT] = RGBA_MAKE(             0 , 0xff - 10*(i+2), 0xff - 10*(i+2), 0xff - 10*(i+2));	// cyan
			dwStripes[(i+3)%STRIPE_COUNT] = RGBA_MAKE(0xff - 10*(i+3),              0 , 0xff - 10*(i+3), 0xff - 10*(i+3));	// magenta
		}
		
		bResult &= pTexture->LoadStripes(uLevel+1, (CUBEMAP_FACES)uFace, LevelDesc.Width, LevelDesc.Height, STRIPE_COUNT, dwStripes, (uLevel%2 == 0 ? true : false), (uLevel%2 == 0 ? false : true));
	}

	return bResult;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::FillCubeWithImage(const UINT uLevel, CnCubeTexture* pTexture, const TCHAR* szFileName)
{
	if(NULL == szFileName || NULL == pTexture)
	{
		return false;
	}

	bool bResult = true;

	for(UINT nFace = 0; nFace<6; nFace++)
	{
		bResult &= pTexture->LoadTexture(uLevel+1, (CUBEMAP_FACES)nFace, szFileName, 0.2f);
	}

	return bResult;
}

//------------------------------------------------------------------------------

UINT CNonPowTwoTest::ForcePOW2(UINT nDimension)
{
	//find the highest bit set in nDimension

	UINT nHighestBitSet = 0;

	for(UINT nCtr = 0; nCtr < 32; nCtr++)
	{
		if((1<<nCtr) == ((1<<nCtr) & (nDimension)))
		{
			nHighestBitSet = nCtr;
		}
	}

	return (1<<nHighestBitSet);
}

//------------------------------------------------------------------------------

UINT CNonPowTwoTest::ForceMultipleFour(UINT nDimension)
{
    if(0 != (nDimension % 4))
    {
	    nDimension += (4 - (nDimension % 4));
    }

	return nDimension;
}

//------------------------------------------------------------------------------

UINT CNonPowTwoTest::ForceDWORDMultiple(UINT nDimension)
{
    if(0 != (nDimension % 32))
    {
	    nDimension += (32 - (nDimension % 32));
    }

	return nDimension;
}

//------------------------------------------------------------------------------

bool CNonPowTwoTest::IsDXTN(const FORMAT& Format)
{
	if( FMT_DXT1 == Format.d3dfFormat ||
		FMT_DXT2 == Format.d3dfFormat ||
		FMT_DXT3 == Format.d3dfFormat ||
		FMT_DXT4 == Format.d3dfFormat ||
		FMT_DXT5 == Format.d3dfFormat  )
	{
		return true;
	}
	
	return false;
}
