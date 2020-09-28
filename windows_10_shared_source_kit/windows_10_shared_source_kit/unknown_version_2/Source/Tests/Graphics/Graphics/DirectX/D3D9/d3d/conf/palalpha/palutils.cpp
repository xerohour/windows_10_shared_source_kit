#include <math.h>
#include <d3d.h>
#include "PalAlpha.h"

/*
CTexture* CPalTest::CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill) {
	CTexture*	pTex = new CTexture;
	HRESULT		hrLastError = 0;
	DWORD		dwFlags = 0;

	// Source texture
	pTex->m_pSrcSurface = new CDirectDrawSurface;
	if (pTex->m_pSrcSurface == NULL)
	{
		delete pTex;
		return NULL;
	}
    pTex->m_pSrcSurface->Initialize(this);
    if (!m_pSrcAdapter->Devices[m_pMode->nSrcDevice].fHardware)
		dwFlags = DDSCAPS_SYSTEMMEMORY;
	if (!CreatePalettedTexture(pTex->m_pSrcSurface, dwWidth, dwHeight, dwFlags, dwPaletteType, bFillTexture, bColorKeyFill))
	{
		delete pTex;
		return NULL;
	}

	// Reference texture
	if (m_pRefTarget != NULL)
	{
		pTex->m_pRefSurface = new CDirectDrawSurface;
		if (pTex->m_pRefSurface == NULL)
		{
			delete pTex;
			return NULL;
		}
		pTex->m_pRefSurface->Initialize(this);
	    if (!m_pRefAdapter->Devices[m_pMode->nRefDevice].fHardware)
			dwFlags = DDSCAPS_SYSTEMMEMORY;
		if (!CreatePalettedTexture(pTex->m_pRefSurface, dwWidth, dwHeight, dwFlags, dwPaletteType, bFillTexture, bColorKeyFill))
		{
			delete pTex;
			return NULL;
		}
	}
	return pTex;
}

bool CPalTest::CreatePalettedTexture(CDirectDrawSurface *pSurface, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill) {
	DDSURFACEDESC2			descPal;
	DWORD					i, j;
	BYTE					*p;

	// check for correct palette type
	if ( !(dwPaletteType == 3 || dwPaletteType == 2) )
		return false;

	// protection for lazy programming
	if (pSurface == NULL)
		return false;
	
	// create texture surface
	ZeroMemory(&descPal, sizeof(descPal));
	descPal.dwSize = sizeof(descPal);
	descPal.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_CAPS;
	descPal.dwWidth = dwWidth;
	descPal.dwHeight = dwHeight;
	descPal.ddpfPixelFormat.dwSize = sizeof(descPal.ddpfPixelFormat);
	switch(dwPaletteType) {
	case 3:
		descPal.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
		descPal.ddpfPixelFormat.dwRGBBitCount = 8;
		break;
	case 2:
		descPal.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED4;
		descPal.ddpfPixelFormat.dwRGBBitCount = 4;
		break;
	}
    descPal.ddsCaps.dwCaps = DDSCAPS_TEXTURE | dwFlags;
	if (!bFillTexture)
		descPal.ddsCaps.dwCaps |= DDSCAPS_ALLOCONLOAD;

	if (!pSurface->CreateSurface(&descPal))
		return false;

	if (bFillTexture) {
		// fill texture
		p = (unsigned char*)(pSurface->Lock());
		if (p == NULL)
			return false;

		if (dwPaletteType == 3) {
			for (i = 0; i < pSurface->m_dwHeight; i++, p += pSurface->m_lPitch) {
				BYTE	color = (BYTE)(i*255/pSurface->m_dwHeight);
                if (bColorKeyFill)
                    color = color - (color % 20);
				for (j = 0; j < pSurface->m_dwWidth; j++)
					p[j] = color;
			}
		}
		else if (dwPaletteType == 2) {
			for (i = 0; i < pSurface->m_dwHeight; i++, p += pSurface->m_lPitch) {
				BYTE	color = (BYTE)(i*15/pSurface->m_dwHeight);
				color |= (color << 4);
				for (j = 0; j < pSurface->m_dwWidth/2; j++)
					p[j] = color;
			}
		}
		
		if (!pSurface->Unlock()) {
			return false;
		}
	}
	return true;
}
*/

bool CPalTest::CreateTexturePalette(CnTexture* pTextures, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill, DWORD dwFlags) {
	DWORD				flags;
	PALETTEENTRY		pal[256];
	HRESULT				hr;
	CTexture			*pTexture;


	if (m_pD3D == NULL || pTextures == NULL)
		return FALSE;

	if (pPalette != NULL) {
		pPalette->Release();
		pPalette = NULL;
	}

	if (dwPaletteType == 3) {
		flags = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
		if (bAlphaPalette)
			flags |= DDPCAPS_ALPHA;
	}
	else {
		flags = DDPCAPS_4BIT;
		if (bAlphaPalette)
			flags |= DDPCAPS_ALPHA;
	}

	FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, bColorKeyFill);

	for(UINT i = 0; i < m_uDeviceList; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if (!pTextures->GetTexture(&pTexture, i) || pTexture == NULL)
			continue;

		hr = m_pD3D->CreateDDrawPalette(flags, pal, &pPalette);
		if (hr != DD_OK) {
			WriteToLog("PalAlpha: Can't create palette\n");
			SetLastError(hr);
			pPalette = NULL;
			return FALSE;
		}

		// attach the palette to the texture surface
		hr = pTexture->SetDDrawPalette(pPalette);
		if (hr != DD_OK) {
			WriteToLog("PalAlpha: Can't set palette\n");
			SetLastError(hr);
			pPalette->Release();
			pPalette = NULL;
			return FALSE;
		}

		RELEASE(pPalette);
		RELEASE(pTexture);
	}

	return TRUE;
}

bool CPalTest::ChangeTexturePalette(CnTexture* pTextures, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill, DWORD dwFlags) {
	HRESULT				hr;
	PALETTEENTRY		pal[256];
	LPDIRECTDRAWPALETTE	pPal;
	CTexture			*pTexture;
	DWORD				entries = 0;

	if (pTextures == NULL)
		return FALSE;

	if (dwPaletteType ==  3)
		entries = 256;
	else if (dwPaletteType == 2)
		entries = 16;

	for(UINT i = 0; i < m_uDeviceList; i++)
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		if (!pTextures->GetTexture(&pTexture, i) || pTexture == NULL)
			continue;

		hr = pTexture->GetDDrawPalette(&pPal);
		if (hr != DD_OK) {
			WriteToLog("PalAlpha: Can't get texture surface palette\n");
			SetLastError(hr);
			return FALSE;
		}

		hr = pPal->GetEntries(0, 0, entries, pal);
		if (hr != DD_OK) {
			WriteToLog("PalAlpha: Can't retrieve palette entries\n");
			SetLastError(hr);
			pPal->Release();
			return FALSE;
		}

		FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, bColorKeyFill);

		hr = pPal->SetEntries(0, 0, entries, pal);
		if (hr != DD_OK) {
			WriteToLog("PalAlpha: Can't set palette entries\n");
			SetLastError(hr);
			pPal->Release();
		}

		RELEASE(pPal);
		RELEASE(pTexture);
	}
	return TRUE;
}

void CPalTest::FillPaletteEntries(PALETTEENTRY *pal, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill) {
	DWORD			entries = 0;
	DWORD			i;
//	unsigned char	mask = (unsigned char) (cParam >> 5);
//	unsigned char	value = (unsigned char) ((cParam & 0x1f) << 3);

	if (dwPaletteType ==  3)
		entries = 256;
	else if (dwPaletteType == 2)
		entries = 16;

	for(i = 0; i < entries; i++) {
		// Color base value
		unsigned long a = i*256/entries + dwParam*256*8/dwSameFramesNumber + 128;
		unsigned char b = unsigned char(a&0x00ff);
		unsigned char mask = unsigned char( (a>>8)&0x07 );
        unsigned char base;
        
        // Base color
        if (bColorKeyFill) 
        {
            base = b;
            mask = (mask == 0) ? 0x07 : mask;
        }
        else
            base = (unsigned char) (255.* (0.5 - 0.5*cos(6.28*double(b)/256.)));

		// Mask
		if (mask == 0)
			mask = 0x07;

		// Alpha component
		if (bAlphaPalette)
			pal[i].peFlags = base;
		else
		{
			if (m_dwVersion >= 0x0800)
				pal[i].peFlags = 255;
			else
				pal[i].peFlags = 0;
		}
		// Red component
		if (mask & 0x04)
			pal[i].peRed = base;
		else
			pal[i].peRed = 0;
		// Green component
		if (mask & 0x02)
			pal[i].peGreen = base;
		else
			pal[i].peGreen = 0;
		// Blue component
		if (mask & 0x01)
			pal[i].peBlue = base;
		else
			pal[i].peBlue = 0;
	}
}


bool CPalTest::ColorSurface(CSurface* pSurface) {
	LOCKED_RECT		Rect;
	BYTE			*p;

	if (pSurface == NULL)
		return false;

	// fill background
//	p = (BYTE*)pSurf->Lock();
	if (!FAILED(pSurface->LockRect(&Rect, 0, 0)))
	{
		p = (BYTE*)Rect.pBits;
		for (DWORD i = 0; i < pSurface->m_Desc.Height; i++, p += Rect.iPitch) {
			for (DWORD j = 0; j < pSurface->m_Desc.Width; j++) {
				int r = i * 255 / pSurface->m_Desc.Height;
				int g = j * 255 / pSurface->m_Desc.Width;
				int b = 255 - abs(r - g);
				if (pSurface->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount == 16) {
					*(unsigned short*)(&p[j*2]) = 
						(unsigned short)(	((r & pSurface->m_Mask.dwRedMask)   << pSurface->m_Mask.nRedShift) | 
											((g & pSurface->m_Mask.dwGreenMask) << pSurface->m_Mask.nGreenShift) | 
											((b & pSurface->m_Mask.dwBlueMask)  << pSurface->m_Mask.nBlueShift) );
				}
				else if (pSurface->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount == 24) {
					p[j*3  ] = (BYTE)r;
					p[j*3+1] = (BYTE)g;
					p[j*3+2] = (BYTE)b;
				}
				else if (pSurface->m_Desc.Format.ddpfPixelFormat.dwRGBBitCount == 32) {
					p[j*4  ] = (BYTE)r;
					p[j*4+1] = (BYTE)g;
					p[j*4+2] = (BYTE)b;
				}
			}
		}
		pSurface->UnlockRect();
	}
	else {
		WriteToLog("PalAlpha: Can't lock back buffer\n");
		return FALSE;
	}

	return TRUE;
}

