//
// CImage.cpp - Implementation of CImage functions
//

#include "ImagePCH.h"
#include "CImage.h"

// --------------------------------------------------------------------------------------

CImage::CImage()
{
    m_dwWidth       = 0;
    m_dwHeight      = 0;
    m_pImageData    = NULL;
	m_pPackedImageData    = NULL;
	return;
}

// --------------------------------------------------------------------------------------

CImage::~CImage()
{
	ReleaseImageData();
	return;
}

// --------------------------------------------------------------------------------------

void CImage::ReleaseImageData()
{
    SAFEDELETEA(m_pImageData);
	SAFEDELETEA(m_pPackedImageData);

	m_dwWidth		= 0;
    m_dwHeight      = 0;
	return;
}

// --------------------------------------------------------------------------------------

bool CImage::Copy(CImage *pImage)
{
	return SetData(pImage->m_dwWidth, pImage->m_dwHeight, pImage->m_pImageData);
}

// --------------------------------------------------------------------------------------

bool CImage::Load( LPCTSTR Filename,
                   float fAlphaOverride)
{
	bool bLoaded = true;
	TCHAR *tExt;
	// Make sure we don't already have an image hangin out
	ReleaseImageData();
	tExt = _tcsstr(Filename, _T("."));
	if (NULL == tExt)
	{
		DPF(1, _T("CImage::Load Invalid file extension \n") );
	}
	else
	{
		//Assume that it is a resource bmp and try to load
		bLoaded = LoadBMP(Filename, fAlphaOverride);
		if (false == bLoaded)
		{
			DPF(1, _T("CImage::Load Invalid Filetype (%s)\n"), _T(Filename) );
		}		
	}	
	return bLoaded;
}

// --------------------------------------------------------------------------------------

bool CImage::SetData(DWORD dwWidth, DWORD dwHeight, FColor* pData)
{
    ReleaseImageData();

	m_pImageData = new FColor[dwWidth * dwHeight];
    if (NULL == m_pImageData)
        return false;

    memcpy(m_pImageData, pData, sizeof(FColor) * dwWidth * dwHeight);
    m_dwWidth = dwWidth;
    m_dwHeight = dwHeight;
    return true;
}

// --------------------------------------------------------------------------------------

bool CImage::GetData(DWORD &dwWidth, DWORD &dwHeight, FColor* pData)
{
	if ((dwWidth != m_dwWidth) || (dwHeight != m_dwHeight))
		return false;

	memcpy(pData, m_pImageData, sizeof(FColor) * m_dwWidth * m_dwHeight);	
	return true;
}

// --------------------------------------------------------------------------------------

bool CImage::ScaleData(DWORD dwNewWidth, DWORD dwNewHeight)
{
    FColor*	    pNewData;
    float       fInterpX = (float) m_dwWidth / (float)dwNewWidth;
    float       fInterpY = (float) m_dwHeight / (float)dwNewHeight;
    float       fX;
    float       fY;
    int         nX;
    int         nY;

    if (NULL == m_pImageData)
        return false;

    pNewData = new FColor[dwNewWidth * dwNewHeight];

    if (NULL == pNewData)
        return false;

    for (fY = 0, nY=0; nY < (float) dwNewHeight; fY += fInterpY, nY++)
    {
        FColor* pNewScanLine = &pNewData[nY * dwNewWidth];
        FColor* pOldScanLine = &m_pImageData[(int) fY * m_dwWidth];

        for (fX = 0, nX=0; nX < (float) dwNewWidth; fX += fInterpX, nX++)
        {
            if (fX >= (float) m_dwWidth)
                fX = (float) (m_dwWidth - 1);

            memcpy(pNewScanLine, pOldScanLine+(int)fX, sizeof(FColor));
            pNewScanLine++;
        }
    }

    ReleaseImageData();

    m_pImageData = pNewData;
    m_dwWidth = dwNewWidth;
    m_dwHeight = dwNewHeight;
    return true;
}

// --------------------------------------------------------------------------------------

bool CImage::ColorFill(FColor* pFillColor)
{
	UINT i = 1;
	UINT uCopy=0;
	UINT uSize = (m_dwWidth * m_dwHeight);

	if (NULL == m_pImageData)
        return false;
	
	memcpy(m_pImageData, pFillColor, sizeof(FColor));
	while ( i < uSize)
	{

		if(i * 2 > uSize)
			uCopy = uSize - i;
		else
			uCopy = i;

		memcpy(m_pImageData+i, m_pImageData, sizeof(FColor) * uCopy);
		i += i;
	}

    return true;
}

// --------------------------------------------------------------------------------------

bool CImage::ColorFill(DWORD dwWidth, DWORD dwHeight, D3DCOLOR FillColor)
{
	FColor imgdata;

	ReleaseImageData();
	m_pImageData = new FColor[dwWidth * dwHeight];
    if (NULL == m_pImageData)
        return false;
	m_dwWidth = dwWidth;
    m_dwHeight = dwHeight;

	imgdata.R = ((FLOAT)((FillColor & 0x0FF0000) >> 16)) / 255.F;
	imgdata.G = ((FLOAT)((FillColor & 0x0FF00) >> 8)) / 255.F;
	imgdata.B = ((FLOAT)((FillColor & 0x0FF))) / 255.F;
	imgdata.A = ((FLOAT)((FillColor & 0x0FF000000) >> 24)) / 255.F;
	return ColorFill(&imgdata);
}

// --------------------------------------------------------------------------------------

bool CImage::HasData()
{
	if (NULL == m_pImageData)
		return false;
	else
		return true;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadGradient(DWORD dwW, DWORD dwH, DWORD dwColors[4])
{
	int i;
	bool bRes = false;
	FColor* fColors = NULL;

	fColors = new FColor[4];
	if(NULL == fColors)
	{
		bRes = false;
		goto Exit;
	}

	for(i=0; i<4; i++)
	{
		fColors[i].R = CIL_MAKEPCT(CIL_RED(dwColors[i]));
		fColors[i].G = CIL_MAKEPCT(CIL_GREEN(dwColors[i]));
		fColors[i].B = CIL_MAKEPCT(CIL_BLUE(dwColors[i]));
		fColors[i].A = CIL_MAKEPCT(CIL_ALPHA(dwColors[i]));
	}

	bRes = LoadGradient(dwW, dwH, fColors);

Exit:
	SAFEDELETEA(fColors);
	return bRes;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadGradient(DWORD dwW,DWORD dwH, FColor pColors[4])
{
	// Color arrangement is:
	//
	// [0] +-----------+ [1]
	//     |           |
	//     |           |
	// [2] +-----------+ [3]

	ReleaseImageData();

	// Interpolants
	float fRedPct[4], fGreenPct[4], fBluePct[4], fAlphaPct[4];

	for(int i=0; i<4; i++)
	{
		fRedPct[i] = pColors[i].R;
		fGreenPct[i] = pColors[i].G;
		fBluePct[i] = pColors[i].B;
		fAlphaPct[i] = pColors[i].A;
	}

	FColor* Data = new FColor[dwW * dwH];
    if (!Data)
    {
        DPF(1, _T("CImage::LoadGradient - Out of memory.\n"));
        return NULL;
    }

	for(DWORD x = 0; x < dwW; x++)
	{
		for(DWORD y = 0; y < dwH; y++)
		{
            float fXPct, fYPct;
            if(dwW > 1)
                fXPct = (float)x/(float)(dwW - 1);
            else
                fXPct = 0;
            if(dwH > 1)
                fYPct = (float)y/(float)(dwH - 1);
            else
                fYPct = 0;
			
			Data[x + y*dwW].R = ((1.0f - fYPct) * (fXPct * fRedPct[1]   + (1.0f - fXPct) * fRedPct[0])) +
				   (fYPct          * (fXPct * fRedPct[3]   + (1.0f - fXPct) * fRedPct[2]));
			Data[x + y*dwW].G = ((1.0f - fYPct) * (fXPct * fGreenPct[1] + (1.0f - fXPct) * fGreenPct[0])) +
				   (fYPct            * (fXPct * fGreenPct[3] + (1.0f - fXPct) * fGreenPct[2]));				   
			Data[x + y*dwW].B = ((1.0f - fYPct) * (fXPct * fBluePct[1]  + (1.0f - fXPct) * fBluePct[0])) +
				   (fYPct           * (fXPct * fBluePct[3]  + (1.0f - fXPct) * fBluePct[2]));				   
			Data[x + y*dwW].A = ((1.0f - fYPct)* (fXPct * fAlphaPct[1] + (1.0f - fXPct) * fAlphaPct[0])) +
				   (fYPct           * (fXPct * fAlphaPct[3] + (1.0f - fXPct) * fAlphaPct[2]));
		}
	}

	if(false == SetData(dwW, dwH, Data))
	{
		DPF(1, _T("CImage::LoadGradient - Failed to set data in CImageData"));
		delete [] Data;
		return false;
	}
	
	delete [] Data;
	return true;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadStripes(DWORD dwW,DWORD dwH,DWORD dwStripes,DWORD dwColors[],bool bHorizontal, bool bDiagonal)
{
	UINT i;
	bool bRes = false;
	FColor* pColors = NULL;

	pColors = new FColor[dwStripes];
	if(NULL == pColors)
	{
		DPF(1, _T("CImage::LoadStripes - Out of memory.\n"));
		bRes = false;
		goto Exit;
	}

	for(i=0; i<dwStripes; i++)
	{
		pColors[i].R = CIL_MAKEPCT(CIL_RED(dwColors[i]));
		pColors[i].G = CIL_MAKEPCT(CIL_GREEN(dwColors[i]));
		pColors[i].B = CIL_MAKEPCT(CIL_BLUE(dwColors[i]));
		pColors[i].A = CIL_MAKEPCT(CIL_ALPHA(dwColors[i]));		
	}

	bRes = LoadStripes(dwW, dwH, dwStripes, pColors, bHorizontal, bDiagonal);

Exit:
	SAFEDELETEA(pColors);
	return bRes;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadStripes(DWORD dwW,DWORD dwH,DWORD dwStripes, FColor* pColors, bool bHorizontal, bool bDiagonal)
{
	DWORD dwStripeWidth;
	FColor *Data;

	// Release any old images we have hanging around
	ReleaseImageData();
	
	// Do some minor parameter validation
	if (dwW < 1)
		dwW = 1;

	if (dwH < 1)
		dwH = 1;

	if(true == bHorizontal)
		dwStripeWidth = dwW / dwStripes;
	else
		dwStripeWidth = dwH / dwStripes;

	if(dwStripeWidth < 1)
		dwStripeWidth = 1;

	// Allocate room for our data
	Data = new FColor[dwW * dwH];
    if (!Data)
    {
        DPF(1, _T("CImage::LoadStripes - Out of memory.\n"));
        return NULL;
    }

	for(DWORD x = 0; x < dwW; x++)
	{
		for(DWORD y = 0; y < dwH; y++)
		{
			// Determine which color we will be using.
			DWORD dwStripe;

			if(true == bHorizontal)
                if(true == bDiagonal)
                    dwStripe = (x + y) / dwStripeWidth;
                else
                    dwStripe = x / dwStripeWidth;
			else
                if(true == bDiagonal)
                    dwStripe = (y + x) / dwStripeWidth;
                else
                    dwStripe = y / dwStripeWidth;

			// Plug the color into our data
			memcpy(Data +(x + dwW * y), pColors + (dwStripe % dwStripes), sizeof(FColor));
		}
	}

	if(false == SetData(dwW, dwH, Data))
	{
		DPF(1, _T("CImage::LoadStripes - Failed to set data in CImageData.\n"));
		delete [] Data;
		return false;
	}
	
	delete [] Data;
	return true;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadAlgorithms(DWORD dwWidth, DWORD dwHeight, PCHANNELFUNCS pData)
{
    FLOAT u, v;
    FLOAT fDelta[2];
    UINT i, j, k;

    ReleaseImageData();
    FColor *Data = new FColor[dwWidth*dwHeight];

    if (!Data)
    {
        DPF(1, _T("CImage::LoadChannelData - Out of memory"));
        return NULL;
    }

    fDelta[0] = (FLOAT) 1.0 / (FLOAT) (dwWidth - 1);
    fDelta[1] = (FLOAT) 1.0 / (FLOAT) (dwHeight - 1); 
    
    for (v = 1.0f, k = 0, j = 0; j < dwHeight; j++, v -= fDelta[1])
	{
        for (u = 0.0f, i = 0; i < dwWidth; i++, u += fDelta[0])
        {
            Data[k].A = (pData->fAlpha) ? pData->fAlpha(u, v) : 0.0f;       
            Data[k].R = (pData->fRed)   ? pData->fRed(u, v) : 0.0f;       
            Data[k].G = (pData->fGreen) ? pData->fGreen(u, v) : 0.0f;       
            Data[k].B = (pData->fBlue)  ? pData->fBlue(u, v) : 0.0f;       

			k++;
        }
	}
    
	if (false == SetData(dwWidth, dwHeight, Data))
    {
        DPF(1, _T("CImage::LoadChannelData - SetData() failed"));
        delete [] Data;
        return false;
    }

    delete [] Data;
    return true;
}

// --------------------------------------------------------------------------------------

bool CImage::LoadBMP(LPCTSTR Filename, float fAlphaOverride)
{
	bool		bRes = true;
	HANDLE		Bitmap = NULL;
	SIZE		BitmapSize;
    BITMAP		bm;
	HDC			hDC = NULL;
	FColor		*pData = NULL;
	BITMAPINFO	*pBMInfo = NULL;
	DWORD		*pImageData = NULL;

	// Attempt to load from resource
	Bitmap = LoadImage(GetModuleHandle(NULL), Filename,IMAGE_BITMAP, 0, 0,LR_CREATEDIBSECTION);	
	if(NULL == Bitmap)
	{
		// Failed to load from resource, lets try a file
		Bitmap = LoadImage(NULL, Filename, IMAGE_BITMAP,0, 0,LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	}

	if(NULL == Bitmap)
	{
		// Completely failed to load.
		DPF(1, _T("CImage::LoadBMP - Unable to load BMP from either file or resource."));
		bRes = false;
		goto Exit;
	}

	// Get the bitmap size, so we can initialize the ImageData to the proper size
    // CLIFFO: Use GetObject() to get the size of the object
    if (sizeof(BITMAP) != GetObject(Bitmap,sizeof(BITMAP),&bm))
    {
		DPF(1, _T("CImage::LoadBMP - Failed to retrieve bitmap size."));
		bRes = false;
		goto Exit;
    }

    BitmapSize.cx = bm.bmWidth;
    BitmapSize.cy = bm.bmHeight;

	// Get the bits
	pData = new FColor[BitmapSize.cx * BitmapSize.cy];
	pImageData = new DWORD[BitmapSize.cx * BitmapSize.cy];
	// CLIFFO: Allocate required space. BITMAPINFO uses a BMINFOHEADER, which is smaller
    // than required. Fortunately, GetDIBits() would fail gracefully.
	pBMInfo = (BITMAPINFO *) new BYTE [sizeof(BITMAPV4HEADER) + sizeof(RGBQUAD) + 1024];
    if(NULL == pData || NULL == pImageData || NULL == pBMInfo)
    {
        DPF(1, _T("CImage::LoadBMP - Out of memory."));
        bRes = false;
		goto Exit;
    }

	ZeroMemory(pBMInfo, sizeof(BITMAPV4HEADER) + sizeof(RGBQUAD));
	BITMAPV4HEADER *Header = (BITMAPV4HEADER *)&(pBMInfo->bmiHeader);
	Header->bV4Size = sizeof(BITMAPV4HEADER);
	Header->bV4Width = BitmapSize.cx;
	Header->bV4Height = -BitmapSize.cy;
	Header->bV4Planes = 1;
	Header->bV4BitCount = 32;
	Header->bV4V4Compression = BI_BITFIELDS;
	Header->bV4SizeImage = 0;
	Header->bV4XPelsPerMeter = 0;
	Header->bV4YPelsPerMeter = 0;
	Header->bV4ClrUsed = 0;
	Header->bV4ClrImportant = 0;
	Header->bV4RedMask =   0x00FF0000;
	Header->bV4GreenMask = 0x0000FF00;
	Header->bV4BlueMask =  0x000000FF;
	Header->bV4AlphaMask = 0xFF000000;
	Header->bV4CSType = LCS_sRGB;

    // CLIFFO: Store the DC so it may be released.
	if(hDC = GetDC(NULL))
	{
		if(0 == GetDIBits(hDC,
						  (HBITMAP)Bitmap,
						  0,
						  BitmapSize.cy,
						  pImageData,
						  pBMInfo,
						  DIB_RGB_COLORS))
		{
			DPF(1, _T("CImage::LoadBMP - Failed to retrieve bitmap bits."));
			bRes = false;
			goto Exit;
		}
	}

    for(int x = 0; x < BitmapSize.cx; x++)
    for(int y = 0; y < BitmapSize.cy; y++)
	{
		pImageData[y * BitmapSize.cx + x] |= ((DWORD)(fAlphaOverride * 255.f)) << 24;
		pData[y * BitmapSize.cx + x] = pImageData[y * BitmapSize.cx + x];
	}

	if (false == SetData(BitmapSize.cx, BitmapSize.cy, pData))
	{
		DPF(1, _T("CImage::LoadBMP - SetData failed.\n"));
		bRes = false;
		goto Exit;
	}

Exit:

	SAFEDELETEA(pData);
	SAFEDELETEA(pImageData);
	SAFEDELETEA(pBMInfo);

	if(NULL != Bitmap)
		DeleteObject(Bitmap);
	if(NULL != hDC)
		ReleaseDC(NULL,hDC);

	return bRes;
}

// --------------------------------------------------------------------------------------

DWORD* CImage::GetPackedData()
{
	// Convert the float data to packed DWORD with 8 bits per channel
	if(NULL == m_pPackedImageData)
	{
		BYTE bR, bG, bB, bA;
		UINT uCount = m_dwWidth * m_dwHeight;
        
		m_pPackedImageData = new DWORD[uCount];
		if(NULL == m_pPackedImageData)
		{
			DPF(1, _T("CImage::GetPackedData - Out of memory.\n"));
			return NULL;
		}

		for(UINT i=0; i<uCount; i++)
		{
			bR = (BYTE)(m_pImageData[i].R * 255.f);
			bG = (BYTE)(m_pImageData[i].G * 255.f);
			bB = (BYTE)(m_pImageData[i].B * 255.f);
			bA = (BYTE)(m_pImageData[i].A * 255.f);
			
			m_pPackedImageData[i] = RGBA_MAKE(bR, bG, bB, bA);
		}
	}
	return m_pPackedImageData;
}