INT16)(32767.f * (Gg*2 - 1.f) + fRoundOffset));
        }
        break;

        case FMT_L6V5U5:
        {
            INT8 iR8 = (INT8)((Rg*2 - 1.f) * 15.f + fRoundOffset);
            INT8 iG8 = (INT8)((Gg*2 - 1.f) * 15.f + fRoundOffset);
            UINT8 uA8 = (UINT8)(A * 63.f + fRoundOffset);
            *((UINT16*)pSurfaceBits) = ((uA8 & 63) << 10) | ((iG8 & 31) << 5) | (iR8 & 31);
        }
        break;


        case FMT_X8L8V8U8:
        {
            *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
            *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
            *((UINT8*)pSurfaceBits+2) = ((UINT8)(255.f * A + fRoundOffset));
        }
        break;
    case FMT_Q8W8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+2) = ((INT8)(127.f * (Bg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+3) = ((INT8)(127.f * (A*2 - 1.f) + fRoundOffset));                        
        }
        break;
        case FMT_Q16W16V16U16:
        {
            *((INT16*)pSurfaceBits+0) = ((INT16)(32767.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+1) = ((INT16)(32767.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+2) = ((INT16)(32767.f * (Bg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+3) = ((INT16)(32767.f * (A*2 - 1.f) + fRoundOffset));
        }
        break;
    case FMT_W11V11U10:
        {
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 511.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = ((iB16<<21) & 0xFFE00000) | ((iG16<<10) & 0x1FFC00) | (iR16 & 0x3FF);
        }
        break;
        case FMT_W10V11U11:
        {
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 511.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 1023.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = ((iB16<<21) & 0xFFC00000) | ((iG16<<10) & 0x3FF800) | (iR16 & 0x7FF);
        }
        break;
    case FMT_A2W10V10U10:
        {
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 510.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 510.f + fRoundOffset);
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 510.f + fRoundOffset);
                        iA = (UINT32)(A * 3.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = (iA << 30) | ((iB16<<20) & 0x3FF00000) | ((iG16<<10) & 0xFFC00) | (iR16 & 0x3FF);
        }
        break;
	case FMT_A1:
        iA = (int)((FLOAT)A + fRoundOffset);
        *((unsigned char *)pSurfaceBits) = (char)iA;
		break;

#if 0 //Removed from DX9
    case FMT_A8X8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((UINT8*)pSurfaceBits+3) = ((UINT8)(255.f * A + fRoundOffset));
        }
                break;
    case FMT_L8X8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((UINT8*)pSurfaceBits+3) = ((UINT8)(255.f * A + fRoundOffset));
        }
        break;
#endif
	default:
        {
            DPF(2, _T("FColor::ConvertTo() - Unsupported format type. FMT=%d.\n"), (UINT)Type);
        }
        break;
    }
}

// --------------------------------------------------------------------------------------

FSurface::FSurface(CSurface *pSurface)
{
        m_pData = NULL;
        m_uWidth = 0;
        m_uHeight = 0;
        m_fromFmt = FMT_A8R8G8B8;
        ConvertFrom(pSurface);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::SetSize(UINT Width, UINT Height)
{
        HRESULT hr = S_OK;

        if(m_uWidth == Width && m_uHeight == Height)
                goto Exit;

        SAFEDELETEA(m_pData);

        m_pData = new FColor[Width * Height];
        if(NULL == m_pData)
        {
                DPF(1, _T("FSurface::SetSize() - Out of memory.\n"));
                hr = E_OUTOFMEMORY;
                goto Exit;
        }

        memset(m_pData, 0, sizeof(FColor) * Width * Height);

        m_uWidth = Width;
        m_uHeight = Height;

Exit:

        return hr;
}

// --------------------------------------------------------------------------------------

FSurface::~FSurface()
{
        m_uWidth = 0;
        m_uHeight = 0;
        m_fromFmt = FMT_A8R8G8B8;
        SAFEDELETEA(m_pData);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertFrom(CSurface *pSurf)
{
    return ConvertFrom(pSurf, FILTER_NONE);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertFrom(CSurface *pSurf, DWORD filter)
{
    UINT i, j;
    HRESULT hr = S_OK;
    SURFACEDESC  Desc;
    PBYTE pScanline = NULL, pPixel = NULL;
    LOCKED_RECT r;
    FColor *pFloatSurf = NULL;
    float fY0, fY1, fU, fV;
    PALETTEENTRY Palette[256], *pEntry = NULL;
    DWORD dwNumEntries;
    FColor *pColor1 = NULL, *pColor2 = NULL, colorTmp;

    if(NULL == pSurf) {
        DPF(1, _T("FSurface::ConvertFrom() - Invalid params.\n"));
        hr = E_INVALIDARG; goto Exit;
    }

    if(FAILED(hr = pSurf->GetDesc(&Desc))) {
        DPF(1, _T("FSurface::ConvertFrom() - GetDesc failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }
    m_fromFmt = (FMT)Desc.Format.d3dfFormat;

    if((FMT)Desc.Format.d3dfFormat == FMT_P8 || (FMT)Desc.Format.d3dfFormat == FMT_A8P8) {
        if (FAILED(hr = pSurf->GetPalette((LPPALETTEENTRY)Palette, dwNumEntries))) {
            DPF(1, _T("FSurface::ConvertFrom() - GetPalette failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
            goto Exit;
        }
    }

    pFloatSurf = new FColor[Desc.Width * Desc.Height];
    if(NULL == pFloatSurf) {
        DPF(1, _T("FSurface::ConvertFrom() - Out of memory.\n"));
        hr = E_OUTOFMEMORY; goto Exit;
    }
    memset(pFloatSurf, 0, sizeof(FColor) * Desc.Width * Desc.Height);

    if (FAILED(hr = pSurf->LockRect(&r, NULL, LOCK_READONLY | LOCK_NOSYSLOCK))) {
        DPF(1, _T("FSurface::ConvertFrom() - LockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

    pScanline = (PBYTE)r.pBits;
    for(i=0; i<Desc.Height; i++) {
        for(j=0; j<Desc.Width; j++) {
            switch((FMT)Desc.Format.d3dfFormat) {
                case FMT_P8:
                case FMT_A8P8:
                {
                    pPixel = pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8) * j);
                    pEntry = Palette + (*((WORD*)pPixel) & 0xFF);
                    pColor1 = pFloatSurf + (i * Desc.Width + j);
                    pColor1->R = pEntry->peRed / 255.f; pColor1->G = pEntry->peGreen / 255.f; pColor1->B = pEntry->peBlue / 255.f;
                    if((FMT)Desc.Format.d3dfFormat == FMT_P8)
                        pColor1->A = pEntry->peFlags / 255.f;
                    else
                        pColor1->A = ((*((WORD*)pPixel) >> 8) & 0xFF) / 255.f;
                }
                break;
                case FMT_YUY2:
                case FMT_UYVY:
                {
                    pPixel = pScanline + (2 * j);
                    if ((FMT)Desc.Format.d3dfFormat == FMT_YUY2) {
                        UNPACKYUY2(*((DWORD*)pPixel), fY0, fY1, fU, fV);
                    }
                    else {
                        UNPACKUYVY(*((DWORD*)pPixel), fY0, fY1, fU, fV);
                    }

                    pColor1 = pFloatSurf + (i * Desc.dwWidth) + j;
                    if (j+1 == Desc.Width) {
                        pColor2 = &colorTmp;
                    }
                    else {
                        pColor2 = pFloatSurf + (i * Desc.dwWidth) + j + 1;
                    }
                    YUVTORGB(fY0, fY1, fU, fV, pColor1, pColor2);
                    j++;
                }
                break;
                case FMT_R8G8_B8G8:
                case FMT_G8R8_G8B8:
                {
                    pPixel = pScanline + (2 * j);
                    DWORD dwPixel = *((DWORD*)pPixel);
                    
                    pColor1 = pFloatSurf + (i * Desc.dwWidth) + j;
                    if (j+1 == Desc.Width) {
                        pColor2 = &colorTmp;
                    }
                    else {
                        pColor2 = pFloatSurf + (i * Desc.dwWidth) + j + 1;
                    }
                    pColor1->A = 1.0f; pColor2->A = 1.0f;
                    if((FMT)Desc.Format.d3dfFormat == FMT_R8G8_B8G8) {

                        pColor1->B = pColor2->B = ((dwPixel >> 24)&0xff)/255.f;
                        pColor2->G = ((dwPixel >> 16)&0xff)/255.f;
						pColor1->R = pColor2->R = ((dwPixel >> 8)&0xff)/255.f;
						pColor1->G = (dwPixel&0xff)/255.f;
                    }
                    else {
						
                        pColor2->G = ((dwPixel >> 24)&0xff)/255.f;
						pColor1->B = pColor2->B = ((dwPixel >> 16)&0xff)/255.f;
						pColor1->G = ((dwPixel >> 8)&0xff)/255.f;
						pColor1->R = pColor2->R = (dwPixel&0xff)/255.f;
                    }
                    j++;
                }
                break;
                default:
                {
                    pPixel = pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8) * j);
                    pFloatSurf[i * Desc.Width + j].ConvertFrom((FMT)Desc.Format.d3dfFormat, (const char*)pPixel, FALSE, ELEMENTINDEX_0);
                }
                break;
            }
        }
        pScanline += r.iPitch;
    }

    if (FAILED(hr = pSurf->UnlockRect())) {
        DPF(1, _T("FSurface::ConvertFrom() - UnlockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

  Exit:
    if(FAILED(hr)) {
        SAFEDELETEA(pFloatSurf);
    }
    else {
        // Release any existing data
        SAFEDELETEA(m_pData);

        // Update the surface with the new data
        m_uWidth        = Desc.Width;
        m_uHeight = Desc.Height;
        m_pData = pFloatSurf;
    }
    return hr;
}

// --------------------------------------------------------------------------------------

static const DWORD COLORRANGE_UUUU = 0x01;
static const DWORD COLORRANGE_UNNN = 0x02;
static const DWORD COLORRANGE_NNNN = 0x03;
static const DWORD COLORRANGE_FFFF = 0x04;

static const char *COLORRANGE_STRINGS[] = {
    "Invalid Color Range", "UUUU", "UNNN", "NNNN", "FFFF"
};

DWORD GetColorRangeType(FMT fmt) {
    DWORD dwColorRangeType = 0;
    
    switch (fmt) {
        case FMT_R8G8B8 :
        case FMT_A8R8G8B8 :
        case FMT_X8R8G8B8 :
        case FMT_R5G6B5 :
        case FMT_X1R5G5B5 :
        case FMT_A1R5G5B5 :
        case FMT_A4R4G4B4 :
        case FMT_R3G3B2 :
        case FMT_A8 :
		case FMT_A1 :
        case FMT_A8R3G3B2 :
        case FMT_X4R4G4B4 :
        case FMT_A2B10G10R10 :
        case FMT_A8B8G8R8 :
        case FMT_X8B8G8R8 :
        case FMT_G16R16 :
        case FMT_A2R10G10B10 :
		case FMT_A2B10G10R10_XR_BIAS :
        case FMT_A16B16G16R16 :
        case FMT_A8P8 :
        case FMT_P8 :
        case FMT_L8 :
        case FMT_A8L8 :
        case FMT_A4L4 :
        case FMT_L16 :
        case FMT_UYVY :
        case FMT_R8G8_B8G8 :
        case FMT_YUY2 :
        case FMT_G8R8_G8B8 :
        case FMT_DXT1 :
        case FMT_DXT2 :
        case FMT_DXT3 :
        case FMT_DXT4 :
        case FMT_DXT5 :
        case FMT_D16_LOCKABLE :
            dwColorRangeType = COLORRANGE_UUUU;
            break;
        case FMT_V8U8 :
        case FMT_L6V5U5 :
        case FMT_X8L8V8U8 :
        case FMT_V16U16 :
        case FMT_W11V11U10 :
        case FMT_W10V11U11 :
        case FMT_A2W10V10U10 :
#if 0
        case FMT_A8X8V8U8 :
        case FMT_L8X8V8U8 :
#endif
        case FMT_CxV8U8 :
            dwColorRangeType = COLORRANGE_UNNN;
            break;

        case FMT_Q8W8V8U8 :
        case FMT_Q16W16V16U16 :
            dwColorRangeType = COLORRANGE_NNNN;
            break;
        case FMT_R16F :
        case FMT_G16R16F :
        case FMT_A16B16G16R16F :
        case FMT_R32F :
        case FMT_G32R32F :
        case FMT_A32B32G32R32F :
        case FMT_D32F_LOCKABLE :
            dwColorRangeType = COLORRANGE_FFFF;
            break;
        default :
            char szFormat[32];
            FmtToString(FORMAT_TEXTURE, &FORMAT(fmt), szFormat);
            DPF(0, _T("FSurface::ConvertFrom() - No ColorRange known for format: %s.\n"), szFormat);
            dwColorRangeType = 0;
    }
    return dwColorRangeType;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustFFFFToUNNN(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*0.5f + 0.5f;
    pColorOut->G = colorIn.G*0.5f + 0.5f;
    pColorOut->B = colorIn.B*0.5f + 0.5f;
    pColorOut->A = colorIn.A;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustFFFFToNNNN(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*0.5f + 0.5f;
    pColorOut->G = colorIn.G*0.5f + 0.5f;
    pColorOut->B = colorIn.B*0.5f + 0.5f;
    pColorOut->A = colorIn.A*0.5f + 0.5f;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustUNNNToFFFF(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*2 - 1.f;
    pColorOut->G = colorIn.G*2 - 1.f;
    pColorOut->B = colorIn.B*2 - 1.f;
    pColorOut->A = colorIn.A;
}

// --------------------------------------------------------------------------------------

static void  ColorRangesAdjustNNNNToFFFF(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*2 - 1.f;
    pColorOut->G = colorIn.G*2 - 1.f;
    pColorOut->B = colorIn.B*2 - 1.f;
    pColorOut->A = colorIn.A*2 - 1.f;
}

// --------------------------------------------------------------------------------------

static BOOL CheckColorRanges(FMT to, FMT from, UINT_PTR **ppfn) {
    DWORD crtTo = GetColorRangeType(to);
    DWORD crtFrom = GetColorRangeType(from);
    BOOL bConversionNeeded = FALSE;
    
    switch(crtTo) {
        case COLORRANGE_FFFF :
            switch(crtFrom) {
                case COLORRANGE_UNNN:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustUNNNToFFFF;
                    bConversionNeeded = TRUE;
                    break;
                    
                case COLORRANGE_NNNN:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustNNNNToFFFF;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
        case COLORRANGE_UNNN :
            switch(crtFrom) {
                case COLORRANGE_FFFF:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustFFFFToUNNN;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
        case COLORRANGE_NNNN :
            switch(crtFrom) {
                case COLORRANGE_FFFF:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustFFFFToNNNN;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
    }
    if (bConversionNeeded) {
        DPF(1, "Conversion needed: %s -> %s\n", COLORRANGE_STRINGS[crtFrom], COLORRANGE_STRINGS[crtTo]);
    }
    else {
        DPF(2, "No conversion needed: %s -> %s\n", COLORRANGE_STRINGS[crtFrom], COLORRANGE_STRINGS[crtTo]);
    }
    return bConversionNeeded;
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertTo(CSurface *pSurf)
{
    return ConvertTo(pSurf, FILTER_NONE);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertTo(CSurface *pSurf, DWORD filter)
{
    UINT i, j;
    HRESULT hr = S_OK;
    SURFACEDESC  Desc;
    PBYTE pScanline = NULL;
    LOCKED_RECT r;
    FColor Pal[256];
    DWORD dwNumEntries;
    BOOL bGammaCorrect = FALSE; // !!! need to set this appropriately
    BOOL bAdjustColorRange = FALSE;
    void (*pfnColorRangeAdjust)(FColor colorIn, FColor *pColorOut) = 0L;

    if(NULL == pSurf || NULL == m_pData) {
        DPF(1, _T("FSurface::ConvertTo() - Invalid params.\n"));
        hr = E_INVALIDARG; goto Exit;
    }

    if(FAILED(hr = pSurf->GetDesc(&Desc))) {
        DPF(1, _T("FSurface::ConvertTo() - GetDesc failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

        // Sizes must be identical
    if(Desc.Width != m_uWidth || Desc.Height != m_uHeight) {
        DPF(1, _T("FSurface::ConvertTo() - Output surface dimensions need to be identical to FSurface dimensions.\n"));
        goto Exit;
    }

    if ((FMT)Desc.Format.d3dfFormat == FMT_P8 || (FMT)Desc.Format.d3dfFormat == FMT_A8P8) {
        PALETTEENTRY Palette[256];
        if (FAILED(hr = pSurf->GetPalette((LPPALETTEENTRY)Palette, dwNumEntries))) {
            DPF(1, _T("FSurface::ConvertTo() - GetPalette failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
            goto Exit;
        }
        for (UINT u = 0; u < dwNumEntries; u++) {
            Pal[u] = FColor(Palette[u].peRed, Palette[u].peGreen, Palette[u].peBlue, Palette[u].peFlags);
        }
    }

    if (FAILED(hr = pSurf->LockRect(&r, NULL, LOCK_READONLY | LOCK_NOSYSLOCK))) {
        DPF(1, _T("FSurface::ConvertTo() - LockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

    bAdjustColorRange = CheckColorRanges((FMT)Desc.Format.d3dfFormat, m_fromFmt, (UINT_PTR **)&pfnColorRangeAdjust);
    pScanline = (PBYTE)r.pBits;
    for (i=0; i<Desc.Height; i++) {
        for (j=0; j<Desc.Width; j++){
            FColor color2;
            FColor color = m_pData[i * Desc.Width + j];
            BYTE *pPixel = (BYTE *)(pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount/8) * j));

            switch((FMT)Desc.Format.d3dfFormat) {
                case FMT_P8:
                case FMT_A8P8:
                {
                    // !!! gamma support needed in all of these FSurface::Convert*() methods that don't call FColor::Convert*().
                    if(bGammaCorrect )
                        color = FColor(GammaCorrectChannel(color.R),GammaCorrectChannel(color.G),GammaCorrectChannel(color.B),color.A);

                    UINT uMin = 0;
                    FLOAT fDistMin = FLT_MAX;
                    for (UINT u = 0; u < dwNumEntries; u++){
                        FColor cDiff = color - Pal[u];
                        FLOAT fDist = cDiff.A * cDiff.A + cDiff.R * cDiff.R + cDiff.G * cDiff.G + cDiff.B * cDiff.B;

                        if (fDist < fDistMin) {
                            uMin = u;
                            fDistMin = fDist;
                            if (fDistMin == 0.0f)
                                u = dwNumEntries;
                        }
                    }

                    if((FMT)Desc.Format.d3dfFormat == FMT_P8) {
                        *((UINT8 *)pPixel) = (UINT8) uMin;
                    }
                    else {
                        *((UINT16 *)pPixel) = (UINT16) (uMin | (INT(color.A*255.0f) << 8));
                    }
                    break;
                }
                case FMT_R8G8_B8G8:
                case FMT_G8R8_G8B8:
                {
                    DWORD *pdwPixel = (DWORD *)(pScanline + (2 * j));
                    color = m_pData[i * Desc.Width + j];
                    color2 = m_pData[i * Desc.Width + j + 1];

                    // alternative methods can look like something below.  Current method used
                    // since that's what gives results closest to what d3dx does.
                    // R = (color.R + color2.R)/2 or R = color2.R

                    if ((FMT)Desc.Format.d3dfFormat == FMT_R8G8_B8G8) {
                        PACKYUY2(*pdwPixel, 255.f*color.G+.5f, 255.f*color2.G+.5f, 255.f*color.R+.5f, 255.f*color.B+.5f);
                    }
                    else {
                        PACKUYVY(*pdwPixel, 255.f*color.G+.5f, 255.f*color2.G+.5f, 255.f*color.R+.5f, 255.f*color.B+.5f);
                    }
                    j++;
                    break;
                }
                case FMT_YUY2:
                case FMT_UYVY:
                {
                    float y1, y2, u, v, u1, v1, u2, v2;
                    DWORD *pdwPixel = (DWORD *)(pScanline + (2 * j));

                    color = m_pData[i * Desc.Width + j];
                    RGBTOYUV((UINT32)(255.f*color.R+.5f),(UINT32)(255.f*color.G+.5f),(UINT32)(255.f*color.B+.5f),y1,u1,v1);

                    color2 = m_pData[i * Desc.Width + j + 1];
                    RGBTOYUV((UINT32)(255.f*color2.R+.5f),(UINT32)(255.f*color2.G+.5f),(UINT32)(255.f*color2.B+.5f),y2,u2,v2);
                    u = u1; v = v1;
                    // alternative methods.  current method used since
                    // that's what gives results closest to what d3dx does.
                    //u = (u1 + u2)/2.f; v = (v1 + v2)/2.f;
                    //u = u1; v = v2;
                    //u = u2; v = v2;
                    if ((FMT)Desc.Format.d3dfFormat == FMT_YUY2) {
                        PACKYUY2(*pdwPixel, y1, y2, u, v);
                    }
                    else {
                        PACKUYVY(*pdwPixel, y1, y2, u, v);
                    }
                    j++;
                    break;
                }
                default:
                    if (bAdjustColorRange) {
                        (*pfnColorRangeAdjust)(color, &color2);
                        color2.ConvertTo((FMT)Desc.Format.d3dfFormat,.5f,(char *)pPixel,FALSE,ELEMENTINDEX_0);
                    }
                    else {
                        color.ConvertTo((FMT)Desc.Format.d3dfFormat,.5f,(char *)pPixel,FALSE,ELEMENTINDEX_0);
                    }
                    
                    break;
            }
        }
        pScanline += r.iPitch;
    }

    if (FAILED(hr = pSurf->UnlockRect())) {
        DPF(1, _T("FSurface::ConvertTo() - UnlockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

 Exit:
    return hr;
}
!include ..\sources.inc

TARGETNAME=Present2_LH

SOURCES=$(SOURCES)\
	..\Present2_LH_EXEEntryPoint.cpp \
	..\PresentEx.cpp \
	..\PresentRotate.cpp \

SOURCES=$(SOURCES) kitContent.kml

#include <initguid.h>
#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include <stdio.h>

using namespace testfactor;

//////////////////////////////////////////////////////////////////////////
// CMyTest - Base class for test used in this sample.
//////////////////////////////////////////////////////////////////////////
class CMyTest : public CTest
{
public:
	CMyTest() {}
	TEST_RESULT ExecuteTestCase()
		{
			// Pretend to skip every fifth test case
			if( ( GetFramework()->GetCurrentTestCase() % 5 ) == 0 )
				return RESULT_SKIP;
			
			// Pretend to fail every third test case
			if( ( GetFramework()->GetCurrentTestCase() % 3 ) == 0 )
				return RESULT_FAIL;
				
			return RESULT_PASS;
		}
};


//////////////////////////////////////////////////////////////////////////
// CFooTest
//////////////////////////////////////////////////////////////////////////
class CFooTest : public CMyTest
{
public:

	CFooTest() :
		m_fSlider( 0 ),
		//m_TexFormat( DXGI_FORMAT_UNKNOWN ),
		m_nCounter( 0 ) {}

	void InitTestParameters()
		{
			// Simple full-permutation test
			//AddParameter( _T( "TexFormat" ), &m_TexFormat, FormatValueSet( _T( "!( {DEPTH} || {TYPELESS} )" ) ) );
			AddParameter( _T( "Slider" ), &m_fSlider, GradientValueSet( -1.3f, 5.7f, 64  ) );
			AddParameter( _T( "Counter" ), &m_nCounter, RangeValueSet( 0, 12, 1 ) );
		}
		
	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}
		
protected:
	//DXGI_FORMAT m_TexFormat;
	float m_fSlider;
	int m_nCounter;
};

struct CarInfo
{
	tstring strMake;
	tstring strModel;
	tstring strColor;
};

CarInfo CarData[] =
{
	{ _T( "Ford" ), _T( "Explorer" ), _T( "White" ) },
	{ _T( "Honda" ), _T( "Accord" ), _T( "Black" ) },
	{ _T( "Toyota" ), _T( "Corolla" ), _T( "Green" ) },
	{ _T( "Mazda" ), _T( "Miata" ), _T( "Orange" ) },
	{ _T( "Chevrolet" ), _T( "Corvette" ), _T( "Red" ) },
	{ _T( "Volkswagen" ), _T( "Beetle" ), _T( "Pink (with purple flowers)" ) },
	{ _T( "Dodge" ), _T( "Viper" ), _T( "Yellow" ) },
	{ _T( "Hyundai" ), _T( "Accent" ), _T( "Brown" ) },
	{ _T( "Chrysler" ), _T( "Sebring" ), _T( "Maroon" ) },
	{ _T( "Ford" ), _T( "Mustang" ), _T( "Blue" ) },
	{ _T( "Pontiac" ), _T( "Trans Am" ), _T( "Black" ) },
};

//////////////////////////////////////////////////////////////////////////
// CBarTest
//////////////////////////////////////////////////////////////////////////
class CBarTest : public CMyTest
{
public:
	
	CBarTest() {}
		
	void InitTestParameters()
		{
			// Simple full-permutation test
			RFactor make = AddParameter( 
				_T( "Make" ), 
				&m_CarInfo.strMake, 
				new CTableValueSet< tstring >( 
					&CarData[0].strMake,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
			
			RFactor model = AddParameter( 
				_T( "Model" ), 
				&m_CarInfo.strModel, 
				new CTableValueSet< tstring >( 
					&CarData[0].strModel,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
			
			RFactor color = AddParameter( 
				_T( "Color" ), 
				&m_CarInfo.strColor, 
				new CTableValueSet< tstring >( 
					&CarData[0].strColor,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
					
			SetRootTestFactor( make % model % color );
		}
		
	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}
	
protected:
	CarInfo m_CarInfo;
};


struct MealInfo
{
	tstring strMeal;
	tstring strFood;
	tstring strDrink;
	tstring strAppetizer;
};


const tstring BreakfastFoodNames[] =
{
	tstring( _T( "Eggs" ) ),
	tstring( _T( "Cereal" ) ),
	tstring( _T( "Pancakes" ) ),
};

const tstring LunchFoodNames[] =
{
	tstring( _T( "Soup" ) ),
	tstring( _T( "Sandwich" ) ),
};

const tstring DinnerFoodNames[] =
{
	tstring( _T( "Steak" ) ),
	tstring( _T( "Lasagna" ) ),
};

const tstring BreakfastDrinkNames[] =
{
	tstring( _T( "Orange Juice" ) ),
	tstring( _T( "Water" ) ),
	tstring( _T( "Milk" ) ),
};

const tstring LunchDrinkNames[] =
{
	tstring( _T( "Milk" ) ),
	tstring( _T( "Water" ) ),
	tstring( _T( "Mountain Dew" ) ),
};

const tstring DinnerDrinkNames[] =
{
	tstring( _T( "Water" ) ),
	tstring( _T( "Wine" ) ),
	tstring( _T( "Beer" ) ),
};

const tstring AppetizerNames[] =
{
	tstring( _T( "Salad" ) ),
	tstring( _T( "Soup" ) ),
	tstring( _T( "Chips 'n' Salsa" ) ),
};

//////////////////////////////////////////////////////////////////////////
// CBazTest
//////////////////////////////////////////////////////////////////////////
class CBazTest : public CMyTest
{
public:
	
	CBazTest() {};
	void InitTestParameters()
		{
			static bool bHappy;
			
			// Add array of structures as a parameter set for this test.
			AddParameter( _T( "Meal" ), &m_MealInfo.strMeal );
			AddParameter( _T( "Food" ), &m_MealInfo.strFood );
			AddParameter( _T( "Drink" ), &m_MealInfo.strDrink );
			AddParameter( _T( "Appetizer" ), &m_MealInfo.strAppetizer );
			CTestCaseParameter< bool > *pHappy = AddParameter( _T( "Happy" ), &bHappy );
			
			RFactor happy = AddParameterValueSet( pHappy, BoolValueSet() ); 
			
			// The following usage of AddParameterValueSet require RTTI enabled.
			RFactor meal_breakfast = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Breakfast" ) );
			RFactor meal_lunch = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Lunch" ) );
			RFactor meal_dinner = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Dinner" ) );
			
			RFactor breakfast_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( BreakfastFoodNames, sizeof( tstring ), ARRAY_SIZE( BreakfastFoodNames ) ) );
			RFactor lunch_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( LunchFoodNames, sizeof( tstring ), ARRAY_SIZE( LunchFoodNames ) ) );
			RFactor dinner_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( DinnerFoodNames, sizeof( tstring ), ARRAY_SIZE( DinnerFoodNames ) ) );
			
			RFactor breakfast_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					BreakfastDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( BreakfastDrinkNames ) ) );
					
			RFactor lunch_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					LunchDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( LunchDrinkNames ) ) );
					
			RFactor dinner_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					DinnerDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( DinnerDrinkNames ) ) );
			
			RFactor dinner_appetizer = AddParameterValueSet<tstring>( 
				_T( "Appetizer" ), 
				new CTableValueSet<tstring>( 
					AppetizerNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( AppetizerNames ) ) );

			SetRootTestFactor( 
				happy * meal_breakfast * breakfast_food * breakfast_drink + 
				meal_lunch * lunch_food * lunch_drink + 
				meal_dinner * dinner_food * dinner_drink * dinner_appetizer
				);

			// Load priority patterns from XML
			CVariationCoreXmlLoader loader;
			tstring errorString;
			HRESULT hr;
			if(FAILED(hr = loader.CreateXmlFileForRead(_T("VarCore.xml"), errorString)))
				WriteError(errorString.c_str());
			if(FAILED(hr = loader.LoadVariationGroupXml( this, _T("Baz"), errorString)))
				WriteError(errorString.c_str());

			// Write a new XML
			CVariationCoreXmlBuilder builder;
			for(size_t i = 0; i < GetNumPriorityProfiles(); ++i)
			{
				CPriorityProfile *priorityProfile = GetPriorityProfile(i);
				builder.AddPriorityProfile(this, _T("Baz"), priorityProfile);
			}
			builder.SaveXml(_T("Simple.xml"), errorString);
		}

	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}

protected:		
	MealInfo m_MealInfo;
};

//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp : public CWGFTestFramework
{
public:
	CMyTestApp() {}
	
	virtual TESTTYPE GetTestType() const { return TESTTYPE_API; }
	
	bool InitTestGroups()
		{
			// Register test class with the group manager.
			if( !ADD_TEST( _T( "foobar\\foo" ), CFooTest ) ) return false;
			if( !ADD_TEST( _T( "foobar\\bar" ), CBarTest ) ) return false;
			if( !ADD_TEST( _T( "baz" ), CBazTest ) ) return false;

			return true;
		}
		
	bool Setup()
		{
			return true;
		}
};

CMyTestApp  g_Application;
#include "Test_Include.fx"

int Test_Count = 103;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_0_inst_frc";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_0 : frc source reg combination v0 is allowed";
string VS_001 = 
	"vs_2_0 "
	"dcl_depth v0 "
	"frc r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_0 : frc source reg combination r0 is allowed";
string VS_002 = 
	"vs_2_0 "
	"mov r0, c0 "
	"frc r0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_0 : frc source reg combination c0 is allowed";
string VS_003 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_0 : frc source reg combination -c0 is allowed";
string VS_004 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_0 : frc source reg combination c0 is allowed";
string VS_005 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_0 : frc source reg combination c0.x is allowed";
string VS_006 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_0 : frc source reg combination c0.y is allowed";
string VS_007 = 
	"vs_2_0 "
	"def c0, 1, 1, 1, 1 "
	"frc r0, c0.y "
	"mov oPos, c0 ";