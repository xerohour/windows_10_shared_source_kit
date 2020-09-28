#include <math.h>
#include <d3d8.h>
#include "PalAlpha.h"


CnTexture* CPalTest::CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill)
{
	CnTexture*	pTex = NULL;
	CnTexture*	pSysTex = NULL;
	FMT			Format = (dwPaletteType == 3) ? FMT_P8 : FMT_P4;

	if (m_dwVersion > 0x0700)
	{
		if (CD3DTest::CreateTexture(dwWidth, dwHeight, 1, 0, Format, FMWK_DEFAULT, &pTex) &&
			CD3DTest::CreateTexture(dwWidth, dwHeight, 1, 0, Format, POOL_SYSTEMMEM, &pSysTex) )
		{
			LOCKED_RECT		Rect;

			if (bFillTexture)
			{
				if (pSysTex->LockRect(0, &Rect, 0, 0))
				{
					BYTE	*p = (BYTE*)Rect.pBits;
					int		i, j;

	//				for (i = 0; i < dwHeight; i++, p += Rect.iPitch) 
	//				{
	//					BYTE	color = (BYTE)(i*255/dwHeight);
	//					for (j = 0; j < dwWidth; j++)
	//						p[j] = color;
	//				}
					if (dwPaletteType == 3) {
						for (i = 0; i < dwHeight; i++, p += Rect.iPitch) {
							BYTE	color = (BYTE)(i*255/dwHeight);
							if (bColorKeyFill)
								color = color - (color % 20);
							for (j = 0; j < dwWidth; j++)
								p[j] = color;
						}
					}
					else if (dwPaletteType == 2) {
						for (i = 0; i < dwHeight; i++, p += Rect.iPitch) {
							BYTE	color = (BYTE)(i*15/dwHeight);
							color |= (color << 4);
							for (j = 0; j < dwWidth/2; j++)
								p[j] = color;
						}
					}
					
					pSysTex->UnlockRect(0);
				}
				UpdateTexture(pSysTex, pTex);
			}
			RELEASE(pSysTex);
			return pTex; 
		}
	}
	else
	{
		if (CD3DTest::CreateTexture(dwWidth, dwHeight, 1, 0, Format, FMWK_DEFAULT, &pTex) )
		{
			LOCKED_RECT		Rect;

			if (bFillTexture)
			{
				if (pTex->LockRect(0, &Rect, 0, 0))
				{
					BYTE	*p = (BYTE*)Rect.pBits;
					int		i, j;

					if (dwPaletteType == 3) {
						for (i = 0; i < dwHeight; i++, p += Rect.iPitch) {
							BYTE	color = (BYTE)(i*255/dwHeight);
							if (bColorKeyFill)
								color = color - (color % 20);
							for (j = 0; j < dwWidth; j++)
								p[j] = color;
						}
					}
					else if (dwPaletteType == 2) {
						for (i = 0; i < dwHeight; i++, p += Rect.iPitch) {
							BYTE	color = (BYTE)(i*15/dwHeight);
							color |= (color << 4);
							for (j = 0; j < dwWidth/2; j++)
								p[j] = color;
						}
					}
					
					pTex->UnlockRect(0);
				}
			}
			return pTex; 
		}
	}

	RELEASE(pTex);
	RELEASE(pSysTex);
	return NULL;
}


bool CPalTest::SetPaletteEntries(DWORD dwPalette, bool bAlphaPalette, DWORD dwParam) {
	PALETTEENTRY		pal[256];

	FillPaletteEntries(pal, dwPaletteType, bAlphaPalette, dwParam, false);

	if (FAILED(m_pSrcDevice->SetPaletteEntries(dwPalette, pal))) {
		WriteToLog("PalAlpha: SetPaletteEntries failed\n");
		return false;
	}
	if (FAILED(m_pRefDevice->SetPaletteEntries(dwPalette, pal))) {
		WriteToLog("PalAlpha: SetPaletteEntries failed\n");
		return false;
	}

	return true;
}

bool CPalTest::SetCurrentTexturePalette(DWORD dwPalette) {
	if (FAILED(m_pSrcDevice->SetCurrentTexturePalette(dwPalette))) {
		WriteToLog("PalAlpha: SetCurrentTexturePalette failed\n");
		return false;
	}
	if (FAILED(m_pRefDevice->SetCurrentTexturePalette(dwPalette))) {
		WriteToLog("PalAlpha: SetCurrentTexturePalette failed\n");
		return false;
	}
	dwCurrentTexturePalette = dwPalette;

	return true;
}

/*
CnTexture* CPalTest::CreateTexture(DWORD dwWidth, DWORD dwHeight) {
	CTexture*	pTex = new CTexture;
	CTexture*	pSysTex = new CTexture;
    D3DFORMAT   Format = D3DFMT_UNKNOWN;
    D3DSURFACE_DESC     SDesc;
    BYTE        *p;
    int         i, j;

    // Check allocation
    if (pTex == NULL || pSysTex == NULL)
    {
        WriteToLog("PalAlpha: Not enough memory to create texture object\n");
        goto error;
    }

    // Check texture format
    if (dwPaletteType != 3)
    {
        WriteToLog("PalAlpha: 4 bit palettized textures are not supported\n");
        goto error;
    }

    // Source textures
    if (m_pSrcDevice != NULL)
    {
        // Source texture
        if (!SetLastError(m_pSrcDevice->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_DEFAULT, &pTex)))
            goto error;
        pTex->m_pSrcSurface = new CDirectDrawSurface;
        if (pTex->m_pSrcSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pTex->m_pSrcSurface->Initialize(this);
        if (!SetLastError(pTex->m_pSrcTexture8->GetSurfaceLevel(0, &pTex->m_pSrcSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pTex->m_pSrcSurface->GetSurfaceDescription(&SDesc))
            pTex->m_pSrcSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "CDirect3D: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }
		
        // Source sysmem texture
        if (!SetLastError(m_pSrcDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_SYSTEMMEM, &pSysTex->m_pSrcTexture8)))
            goto error;
        pSysTex->m_pSrcSurface = new CDirectDrawSurface;
        if (pSysTex->m_pSrcSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pSysTex->m_pSrcSurface->Initialize(this);
        if (!SetLastError(pSysTex->m_pSrcTexture8->GetSurfaceLevel(0, &pSysTex->m_pSrcSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pSysTex->m_pSrcSurface->GetSurfaceDescription(&SDesc))
            pSysTex->m_pSrcSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "PalAlpha: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }
    
        // Fill source sysmem texture
        p = (BYTE*)(pSysTex->m_pSrcSurface->Lock());
		if (p == NULL)
			goto error;
		for (i = 0; i < pSysTex->m_pSrcSurface->m_dwHeight; i++, p += pSysTex->m_pSrcSurface->m_lPitch) {
			BYTE	color = (BYTE)(i*255/pSysTex->m_pSrcSurface->m_dwHeight);
			for (j = 0; j < pSysTex->m_pSrcSurface->m_dwWidth; j++)
				p[j] = color;
		}
		if (!pSysTex->m_pSrcSurface->Unlock())
			goto error;

        // Copy image to source texture
        SetLastError(m_pSrcDevice8->UpdateTexture(pSysTex->m_pSrcTexture8, pTex->m_pSrcTexture8));
    }

	// Reference texture
    if (m_pRefDevice8 != NULL)
    {
        // Reference texture
        if (!SetLastError(m_pRefDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_DEFAULT, &pTex->m_pRefTexture8)))
            goto error;
        pTex->m_pRefSurface = new CDirectDrawSurface;
        if (pTex->m_pRefSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pTex->m_pRefSurface->Initialize(this);
        if (!SetLastError(pTex->m_pRefTexture8->GetSurfaceLevel(0, &pTex->m_pRefSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pTex->m_pRefSurface->GetSurfaceDescription(&SDesc))
            pTex->m_pRefSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "CDirect3D: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }
	
        // Reference sysmem texture
        if (!SetLastError(m_pRefDevice8->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_P8, D3DPOOL_SYSTEMMEM, &pSysTex->m_pRefTexture8)))
            goto error;
        pSysTex->m_pRefSurface = new CDirectDrawSurface;
        if (pSysTex->m_pRefSurface == NULL)
        {
            WriteToLog("PalAlpha: Not enough memory to create surface object\n");
            goto error;
        }
        pSysTex->m_pRefSurface->Initialize(this);
        if (!SetLastError(pSysTex->m_pRefTexture8->GetSurfaceLevel(0, &pSysTex->m_pRefSurface->m_pSurface8)))
        {
            WriteToLog("PalAlpha: GetSurfaceLevel failed\n");
            goto error;
        }
        if (pSysTex->m_pRefSurface->GetSurfaceDescription(&SDesc))
            pSysTex->m_pRefSurface->ProcessSurfaceDescription(&SDesc);
        else
        {
            DebugOut.Write(1, "PalAlpha: Failed to get texture surface description.\n");
            delete pTex;
            return NULL;
        }

        // Fill source sysmem texture
        p = (BYTE*)(pSysTex->m_pRefSurface->Lock());
		if (p == NULL)
			goto error;
		for (i = 0; i < pSysTex->m_pRefSurface->m_dwHeight; i++, p += pSysTex->m_pRefSurface->m_lPitch) {
			BYTE	color = (BYTE)(i*255/pSysTex->m_pRefSurface->m_dwHeight);
			for (j = 0; j < pSysTex->m_pRefSurface->m_dwWidth; j++)
				p[j] = color;
		}
		if (!pSysTex->m_pRefSurface->Unlock())
			goto error;

        // Copy image to source texture
        SetLastError(m_pRefDevice8->UpdateTexture(pSysTex->m_pRefTexture8, pTex->m_pRefTexture8));
    }
	
    RELEASE(pSysTex);
    return pTex;

error:
    RELEASE(pSysTex);
    RELEASE(pTex);
    WriteToLog("PalAlpha: CreateTexture failed\n");
    return NULL;
}

*/