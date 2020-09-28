#include "CImageCompare.h"
#include "CSurface.h"

// --------------------------------------------------------------------------------------

CImageCompare::CImageCompare()
{
	m_fGain = -1;					// Adaptive gain
	m_fPassRequired = .85f;
	m_fPerPixelChannelTol = .15f;
	m_fPerPixelTol = .1f;
	m_fImgDeltaWght = 1.0f;
	m_uMinPixels = 1;
	m_uMaxPixels = 0xFFFFFFFF;
	m_bCompareInPlace = true;
	m_bFailOnly = true;
	m_bGenerateCompare = false;
	m_bIncludeClearColor = false;
	m_CmpImgMode = CMPIMGMODE_RGB;
	m_bCompareAlpha = true;

	m_pSrc = NULL;
	m_pRef = NULL;
	m_pCmp = NULL;

	ResetState();
}

// --------------------------------------------------------------------------------------

CImageCompare::~CImageCompare()
{
	SAFEDELETE(m_pSrc);
	SAFEDELETE(m_pRef);
	SAFEDELETE(m_pCmp);
}

// --------------------------------------------------------------------------------------

int CImageCompare::CompareSurfaceFormats(SURFACEDESC *pSurf1, SURFACEDESC *pSurf2)
{
	int iRetVal = S_OK;

	if( (pSurf1->dwFlags 			!= pSurf2->dwFlags)   ||
		(pSurf1->Format.d3dfFormat  != pSurf2->Format.d3dfFormat) )
	{
		iRetVal = S_DIMENSIONMATCH;
	}

	if( (pSurf1->dwHeight 			!= pSurf2->dwHeight) ||
		(pSurf1->dwWidth 			!= pSurf2->dwWidth) )
	{
		DPF(1, _T("CImageCompare::CompareSurfaces() - Surface compare failed because the surface descriptions of the targets are not identical.\n"));
		DPF(1, _T("CImageCompare::CompareSurfaces() - Width(%d / %d),\n"
				  "                                   Height(%d / %d),\n"
				  "                                   Format(%d / %d),\n"
				  "                                   Flags(%x / %x).\n"),
			pSurf1->dwWidth, pSurf2->dwWidth, pSurf1->dwHeight, pSurf2->dwHeight, 
			pSurf1->Format.d3dfFormat, pSurf2->Format.d3dfFormat, pSurf1->dwFlags, pSurf2->dwFlags);
		iRetVal = E_FAIL;
	}

	return iRetVal;
}

// --------------------------------------------------------------------------------------

HRESULT CImageCompare::CompareImages(CSurface *pSourceSurface, CSurface *pRefSurface, CSurface *pResultSurface, DWORD dwFlags)
{
	HRESULT hr = S_OK;
	SURFACEDESC  Desc1;
	SURFACEDESC  Desc2;
	SURFACEDESC  DescResult;
	bool bForceConvert = false;
		
	if (NULL == pSourceSurface || NULL == pRefSurface)
	{
		DPF(1, _T("CImageCompare::CompareImages() - Invalid params.\n"));
		hr = E_FAIL;
		goto Exit;
	}

	// Get the surface descriptions
	hr = pSourceSurface->GetDesc(&Desc1);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::CompareImages() - GetDescription failed surface 1. hr = %s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	hr = pRefSurface->GetDesc(&Desc2);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::CompareImages() - GetDescription failed surface 2. hr = %s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	// Make sure image descriptions are exactly the same.
	switch ( CompareSurfaceFormats(&Desc1, &Desc2) )
	{
	case S_DIMENSIONMATCH:
		DPF(2, _T("CImageCompare::CompareImages() - Source and Ref surfaces are not identical formats.\n"
			"CompareConverted will be called.\n"));
		bForceConvert = true;
		break;
	case E_FAIL:
		DPF(1, _T("CImageCompare::CompareImages() - Surfaces are not identical dimensions.\n"));
		hr = E_FAIL;
		goto Exit;
	}

	// Validate the cmp surface if requested
	if (NULL != pResultSurface)
	{
		hr = pResultSurface->GetDesc(&DescResult);
		if(FAILED(hr))
		{
			DPF(1, _T("CImageCompare::CompareImages() - GetDescription failed on result surface. hr = %s(%x).\n"),
				pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
			goto Exit;
		}

		switch ( CompareSurfaceFormats(&Desc1, &DescResult) )
		{
		case S_DIMENSIONMATCH:
			DPF(2, _T("CImageCompare::CompareImages() - Source and result surfaces are not identical formats.\n"
				"CompareConverted will be called.\n"));
			bForceConvert = true;
			break;
		case E_FAIL:
			DPF(1, _T("CImageCompare::CompareImages() - Surfaces are not identical dimensions.\n"));
			hr = E_FAIL;
			goto Exit;
		}
	}

	// Reset the state
	ResetState();

	// Do the compare
	if( ( Desc1.Format.ddpfPixelFormat.dwFlags & PF_FLOATTEX ) || ( Desc1.Format.ddpfPixelFormat.dwRGBBitCount > 32 ) || !m_bCompareInPlace || bForceConvert )
	{
		hr = CompareConverted(pSourceSurface, pRefSurface, pResultSurface);
	}
	else
	{
		hr = CompareInPlace(pSourceSurface, pRefSurface, pResultSurface);
		
		if ( (dwFlags & COMPIMG_OPT_FORCEFSURF) && (NULL != pResultSurface) && SUCCEEDED( hr ) )
		{
			if( (NULL == m_pCmp) ||
				(Desc1.Width > m_pCmp->GetWidth() || Desc1.Height > m_pCmp->GetHeight()))
			{
				SAFEDELETE(m_pCmp);
				m_pCmp = new FSurface();

				if( NULL == m_pCmp )
				{
					DPF(1, _T("CImageCompare::CompareImages() - Out of memory.\n"));
					hr = E_OUTOFMEMORY;
					goto Exit;
				}
			}

			hr = m_pCmp->ConvertFrom( pResultSurface );
		}
	}

Exit:	
	return hr;
}

// --------------------------------------------------------------------------------------

void CImageCompare::ResetState()
{
	m_nTotalPixels = 0;
	m_nPerChannelFail = 0;
	m_nPerPixelFail = 0;
	m_fSumDeltaR = 0.f;
	m_fSumDeltaG = 0.f;
	m_fSumDeltaB = 0.f;
	m_fSumDeltaA = 0.f;
}

// --------------------------------------------------------------------------------------

void CImageCompare::NextCmpImgMode()
{
	m_CmpImgMode = (CMPIMGMODE)((UINT)m_CmpImgMode +1);

	if(m_CmpImgMode == CMPIMGMODE_END)
		m_CmpImgMode = (CMPIMGMODE)0;
}

// --------------------------------------------------------------------------------------

TCHAR* CImageCompare::GetCmpImgModeStr()
{
	switch(m_CmpImgMode)
	{
	case CMPIMGMODE_RGB:
		return _T("RGB Channels");
	case CMPIMGMODE_R:
		return _T("Red Channel");
	case CMPIMGMODE_G:
		return _T("Green Channel");
	case CMPIMGMODE_B:
		return _T("Blue Channel");
	case CMPIMGMODE_A:
		return _T("Alpha Channel");
	default:
		return _T("Unknown Mode");
	}
}

// --------------------------------------------------------------------------------------

HRESULT CImageCompare::CompareConverted(CSurface *pSourceSurface, CSurface *pRefSurface, CSurface *pResultSurface)
{
	UINT i, j;
	bool bFailed = false;
	HRESULT hr = S_OK;
	SURFACEDESC  Desc;
	FColor *fCurSrc = NULL, *fCurRef = NULL, *fCurCmp = NULL, TempColor;
	FLOAT fDeltaR, fDeltaG, fDeltaB, fDeltaA, fGain;

	// Get the surface descriptions.  At this point, we're sure that
	// all surfaces have identical dimensions.
	hr = pSourceSurface->GetDesc(&Desc);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::FloatCompare() - GetDescription failed surface 1. hr = %s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	// Allocate the FSurfaces if needed
	if( (NULL == m_pSrc || NULL == m_pRef || (NULL != pResultSurface && NULL == m_pCmp)) ||
		(Desc.Width > m_pSrc->GetWidth() || Desc.Height > m_pSrc->GetHeight()))
	{
		SAFEDELETE(m_pSrc);
		SAFEDELETE(m_pRef);
		SAFEDELETE(m_pCmp);

		m_pSrc = new FSurface();
		m_pRef = new FSurface();
		m_pCmp = new FSurface();

		if(NULL == m_pSrc || NULL == m_pRef || NULL == m_pCmp)
		{
			DPF(1, _T("CImageCompare::FloatCompare() - Out of memory.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}
	}

	// Convert all of the surfaces into FLOAT space
	if(FAILED(m_pSrc->ConvertFrom(pSourceSurface)) ||
	   FAILED(m_pRef->ConvertFrom(pRefSurface)) ||
	   FAILED(m_pCmp->SetSize(Desc.Width, Desc.Height)))
	{
		DPF(1, _T("CImageCompare::FloatCompare() - Failed to convert surfaces to FLOAT space.\n"));
		goto Exit;
	}

	if( m_pCmp && m_bGenerateCompare )
	{
		// Clear the compare surface
		memset(m_pCmp->GetData(), 0, sizeof(FColor) * Desc.Width * Desc.Height);
	}

	// Save off the clear color to possibly be modified later
	TempColor = m_fClearColor;

	// Do the pixel by pixel compare
	for(i=0; i<Desc.Height; i++)
	for(j=0; j<Desc.Width; j++)
	{
		fCurSrc = m_pSrc->GetData() + (i * Desc.Width + j);
		fCurRef = m_pRef->GetData() + (i * Desc.Width + j);
		if(NULL != m_pCmp)
			fCurCmp = m_pCmp->GetData() + (i * Desc.Width + j);

		if(!m_bCompareAlpha)
		{
			fCurSrc->A = fCurRef->A = 0.f;
			TempColor.A = 0.f;
		}

		if (*fCurSrc == *fCurRef && *fCurSrc != TempColor)
		{
				m_nTotalPixels++;
		}
		else
		{
			bFailed = false;
			m_nTotalPixels++;

			// Calc the deltas
			fDeltaR = fabs(fCurSrc->R - fCurRef->R);
			fDeltaG = fabs(fCurSrc->G - fCurRef->G);
			fDeltaB = fabs(fCurSrc->B - fCurRef->B);
			fDeltaA = fabs(fCurSrc->A - fCurRef->A);
			
			// A fixed tolerance only works for specific ranges of
			// floating point numbers since floating point precision
			// varies across the full spectrum of floating point values.
			// For instance 1.15 - 1.0 is relatively accurately calculated
			// in 32-bit floating point but 999999.15 - 999999.0 is not
			// (it actually resolves to 0.125).
			// Rather than just subtract the two floating point values and
			// compare the result to a fixed value, the delta should be
			// scaled by 2^exponent.
			// NOTE: For values < 1.0 leave the tolerance scale at 1.

			unsigned long r = *( (unsigned long *) &fCurRef->R ); // Get the floating point bits as a long unsigned integer
			r &= 0x7f800000; // Keep the exponent, discard sign bit and leave mantissa as (1.)0000...
			float fTolScaleR = *( (float *) &r );
			if( fTolScaleR < 1 )
				fTolScaleR = 1;
			float fMaxDeltaR = fTolScaleR * m_fPerPixelChannelTol; // scale by tolerance value
						
			unsigned long g = *( (unsigned long *) &fCurRef->G ); // Get the floating point bits as a long unsigned integer
			g &= 0x7f800000; // Keep the exponent, discard sign bit and leave mantissa as (1.)0000...
			float fTolScaleG = *( (float *) &g );
			if( fTolScaleG < 1 )
				fTolScaleG = 1;
			float fMaxDeltaG = fTolScaleG * m_fPerPixelChannelTol; // scale by tolerance value
						
			unsigned long b = *( (unsigned long *) &fCurRef->B ); // Get the floating point bits as a long unsigned integer
			b &= 0x7f800000; // Keep the exponent, discard sign bit and leave mantissa as (1.)0000...
			float fTolScaleB = *( (float *) &b );
			if( fTolScaleB < 1 )
				fTolScaleB = 1;
			float fMaxDeltaB = fTolScaleB * m_fPerPixelChannelTol; // scale by tolerance value
						
			unsigned long a = *( (unsigned long *) &fCurRef->A ); // Get the floating point bits as a long unsigned integer
			a &= 0x7f800000; // Keep the exponent, discard sign bit and leave mantissa as (1.)0000...
			float fTolScaleA = *( (float *) &a );
			if( fTolScaleA < 1 )
				fTolScaleA = 1;
			float fMaxDeltaA = fTolScaleA * m_fPerPixelChannelTol; // scale by tolerance value
			
			float fTolScaleMax = __max( fTolScaleR, __max( fTolScaleG, m_bCompareAlpha ? __max( fTolScaleB, fTolScaleA	) : fTolScaleB ) );				

			// Check for per-channel failure
			if(fDeltaR > fMaxDeltaR || fDeltaG > fMaxDeltaG ||
			   fDeltaB > fMaxDeltaB || fDeltaA > fMaxDeltaA)
			{
				m_nPerChannelFail++;
				bFailed = true;
			}
			// Check for overall pixel quality
			else if( (fDeltaR + fDeltaG + fDeltaB + fDeltaA) / (m_bCompareAlpha ? 4.f : 3.f) > fTolScaleMax * m_fPerPixelTol)
			{
				m_nPerPixelFail++;
				bFailed = true;
			}

			// Sum the deltas for overall image fidelity
			fDeltaR /= fTolScaleR;
			fDeltaG /= fTolScaleG;
			fDeltaB /= fTolScaleB;
			fDeltaA /= fTolScaleA;
			
			m_fSumDeltaR += fDeltaR;
			m_fSumDeltaG += fDeltaG;
			m_fSumDeltaB += fDeltaB;
			m_fSumDeltaA += fDeltaA;

			// Generate the results surface
			if (m_bGenerateCompare)
			{
				if((!m_bFailOnly || bFailed) && NULL != fCurCmp)
				{
					// Set the appropriate compare img mode
					switch(m_CmpImgMode)
					{
					case CMPIMGMODE_R:
						fDeltaG = fDeltaB = 0;
						break;
					case CMPIMGMODE_G:
						fDeltaR = fDeltaB = 0;
						break;
					case CMPIMGMODE_B:
						fDeltaR = fDeltaG = 0;
						break;
					case CMPIMGMODE_A:
						fDeltaG = fDeltaB = fDeltaR = fDeltaA;
						break;
					case CMPIMGMODE_RGB:
					default:
						break;
					}

					// Calc the gain.  If the user has requested a specific gain, use it.
					// Otherwise, maximize the pixel color while maintaining the color fidelity.
					if(m_fGain < 0.f)
					{
						fGain = max(fDeltaR, max(fDeltaG, fDeltaB));
						*fCurCmp = FColor((1.f - (fGain - fDeltaR) / fGain), 
										(1.f - (fGain - fDeltaG) / fGain),
										(1.f - (fGain - fDeltaB) / fGain), 0.f);
					}
					else
					{
						*fCurCmp = FColor(fDeltaR * m_fGain, fDeltaG * m_fGain, fDeltaB * m_fGain, 0.f);
					}
					fCurCmp->Clamp();
				}
			}
		}
	}

	// Generate the output surface
	if(m_bGenerateCompare && NULL != m_pCmp)
	{
		// Convert back to the surface format
		hr = m_pCmp->ConvertTo(pResultSurface);
		if(FAILED(hr))
		{
			DPF(1, _T("CImageCompare::FloatCompare() - Failed to convert result surface from FLOAT space. hr = %s(%x).\n"),
					pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	return hr;
}

// --------------------------------------------------------------------------------------

HRESULT CImageCompare::CompareInPlace(CSurface *pSourceSurface, CSurface *pRefSurface, CSurface *pResultSurface)
{
	HRESULT			hr;
	SURFACEDESC		Desc;
	LOCKED_RECT		lrSrc, lrRef, lrResult;
	MASK			Mask;
	DWORD			dwY=0, dwX=0, dwClearColor=0, dwBytesPerPixel;
	FLOAT			fDeltaR=0.f,fDeltaG=0.f,fDeltaB=0.f,fDeltaA=0.f;
	int				iR1=0, iG1=0, iB1=0, iA1=0;
	int				iR2=0, iG2=0, iB2=0, iA2=0;
	PBYTE			pSrc1, pSrc2, pRes;
	float			fGain=0.f, fNormalize=0.f;
	bool			bFailed;
	FColor			TempColor;



	BYTE*           pSrcBits = NULL;
	BYTE*           pRefBits = NULL;

	UINT            nBytesPerLine = 0;


	if (NULL == pSourceSurface || NULL == pRefSurface)
	{
		DPF(1, _T("CImageCompare::CompareInPlace() - Invalid params.\n"));
		return E_FAIL;
	}

	// Zero the rect data
	memset(&lrSrc, 0, sizeof(LOCKED_RECT));
	memset(&lrRef, 0, sizeof(LOCKED_RECT));
	memset(&lrResult, 0, sizeof(LOCKED_RECT));

	// Get the surface descriptions
	hr = pSourceSurface->GetDesc(&Desc);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::CompareInPlace() - GetDescription failed surface 1. hr = %s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	TempColor = m_fClearColor;
	if(!m_bCompareAlpha)
		TempColor.A = 0.f;
	TempColor.ConvertTo((FMT)Desc.Format.d3dfFormat, .5f, (char*)&dwClearColor, FALSE, ELEMENTINDEX_0);
	dwBytesPerPixel = (Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8);
	
	// Compute masks, ceilings, and shift values.
	FormatToMask(&Desc.Format, &Mask);

	// Lock the surfaces
	hr = pSourceSurface->LockRect(&lrSrc, NULL, LOCK_READONLY | LOCK_NOSYSLOCK);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::CompareInPlace() - LockRect failed on SRC. hr=%s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		goto Exit;
	}

	hr = pRefSurface->LockRect(&lrRef, NULL, LOCK_READONLY | LOCK_NOSYSLOCK);
	if (FAILED(hr))
	{
		DPF(1, _T("CImageCompare::CompareInPlace() - LockRect failed on REF. hr=%s(%x).\n"),
			pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
		pSourceSurface->UnlockRect();
		goto Exit;
	}

	if (NULL != pResultSurface)
	{
		hr = pResultSurface->LockRect(&lrResult, NULL, LOCK_READONLY | LOCK_NOSYSLOCK);
		if (FAILED(hr))
		{
			DPF(1, _T("CImageCompare::CompareInPlace() - LockRect failed on RESULT surface. hr=%s(%x).\n"),
				pSourceSurface->GetCDevice()->GetD3D()->HResultToString(hr), hr);
			pSourceSurface->UnlockRect();
			pRefSurface->UnlockRect();
			goto Exit;
		}
	}


	nBytesPerLine = Desc.dwWidth * dwBytesPerPixel;

	pSrcBits = new BYTE[nBytesPerLine];
	pRefBits = new BYTE[nBytesPerLine];


	if((NULL == pSrcBits) || (NULL == pRefBits))
	{
		DPF(1, _T("CImageCompare::CompareInPlace() - Out of memory"));
		hr = E_FAIL;
		goto Exit;
	}

	for(dwY = 0; dwY < Desc.Height; dwY++)
	{
		//A huge bottlenck in the image compare is reading the pixels out of video memory(lrSrc.pBits).  So each horizontal line is memcpy'd to speed things up.  
		//This optimization was present a year ago, but someone removed it.  Please think long and hard before erasing this memcpy.  ChuckN knows about this.
		memcpy(pSrcBits, ((BYTE*)lrSrc.pBits) + (dwY * lrSrc.iPitch), nBytesPerLine);
		memcpy(pRefBits, ((BYTE*)lrRef.pBits) + (dwY * lrRef.iPitch), nBytesPerLine);

		pSrc1= ((BYTE*)pSrcBits);
		pSrc2= ((BYTE*)pRefBits);
		pRes = ((BYTE*)lrResult.pBits) + (dwY * lrResult.iPitch);

		for(dwX = 0; dwX < Desc.Width; dwX++)
		{
			DWORD dwPixel1 = dwBytesPerPixel == 2 ? *((WORD*)pSrc1) : *((DWORD*)pSrc1);
			DWORD dwPixel2 = dwBytesPerPixel == 2 ? *((WORD*)pSrc2) : *((DWORD*)pSrc2);

			// Protect against drivers ignoring the masks
			dwPixel1 &= (Mask.dwRedMask << Mask.nRedShift) | 
						(Mask.dwGreenMask << Mask.nGreenShift) | 
						(Mask.dwBlueMask << Mask.nBlueShift) |
						(m_bCompareAlpha ? (Mask.dwAlphaMask << Mask.nAlphaShift) : 0);
			dwPixel2 &= (Mask.dwRedMask << Mask.nRedShift) | 
						(Mask.dwGreenMask << Mask.nGreenShift) | 
						(Mask.dwBlueMask << Mask.nBlueShift) |
						(m_bCompareAlpha ? (Mask.dwAlphaMask << Mask.nAlphaShift) : 0);

			if (dwPixel1 == dwPixel2)
			{
				// Early out for 0 difference.
				if (m_bIncludeClearColor || dwClearColor != dwPixel1)
					m_nTotalPixels++;

				if (m_bGenerateCompare && NULL != pResultSurface)
				{
					memset(pRes, 0, dwBytesPerPixel);
					pRes += dwBytesPerPixel;
				}
			}
			else
			{
				fNormalize = 0.f;
				bFailed = false;

				if(Mask.nRedBits > 0)
				{
					iR1 = (dwPixel1 >> Mask.nRedShift) & Mask.dwRedMask;
					iR2 = (dwPixel2 >> Mask.nRedShift) & Mask.dwRedMask;
					m_fSumDeltaR += fDeltaR = (float)abs(iR1-iR2) / (float)Mask.dwRedMask;
					fNormalize += 1.f;
				}
				if(Mask.nGreenBits > 0)
				{
					iG1 = (dwPixel1 >> Mask.nGreenShift) & Mask.dwGreenMask;
					iG2 = (dwPixel2 >> Mask.nGreenShift) & Mask.dwGreenMask;
					m_fSumDeltaG += fDeltaG = (float)abs(iG1-iG2) / (float)Mask.dwGreenMask;
					fNormalize += 1.f;
				}
				if(Mask.nBlueBits > 0)
				{
					iB1 = (dwPixel1 >> Mask.nBlueShift) & Mask.dwBlueMask;
					iB2 = (dwPixel2 >> Mask.nBlueShift) & Mask.dwBlueMask;
					m_fSumDeltaB += fDeltaB = (float)abs(iB1-iB2) / (float)Mask.dwBlueMask;
					fNormalize += 1.f;
				}
				if(m_bCompareAlpha && Mask.nAlphaBits > 0)
				{
					iA1 = (dwPixel1 >> Mask.nAlphaShift) & Mask.dwAlphaMask;
					iA2 = (dwPixel2 >> Mask.nAlphaShift) & Mask.dwAlphaMask;
					m_fSumDeltaA += fDeltaA = (float)abs(iA1-iA2) / (float)Mask.dwAlphaMask;
					fNormalize += 1.f;
				}			

				// Check for per-channel failure
				if(fDeltaR > m_fPerPixelChannelTol || fDeltaG > m_fPerPixelChannelTol ||
				   fDeltaB > m_fPerPixelChannelTol || fDeltaA > m_fPerPixelChannelTol)
				{
					m_nPerChannelFail++;
					bFailed = true;
				}
				// Check for overall pixel quality
				else if( (fDeltaR + fDeltaG + fDeltaB + fDeltaA) / fNormalize > m_fPerPixelTol)
				{
					m_nPerPixelFail++;
					bFailed = true;
				}

				m_nTotalPixels++;

				// Generate the results surface
				if (m_bGenerateCompare && NULL != pResultSurface)
				{
					if(!m_bFailOnly || bFailed)
					{
						// Set the appropriate compare img mode
						switch(m_CmpImgMode)
						{
						case CMPIMGMODE_R:
							fDeltaG = fDeltaB = 0;
							break;
						case CMPIMGMODE_G:
							fDeltaR = fDeltaB = 0;
							break;
						case CMPIMGMODE_B:
							fDeltaR = fDeltaG = 0;
							break;
						case CMPIMGMODE_A:
							fDeltaG = fDeltaB = fDeltaR = fDeltaA;
							break;
						case CMPIMGMODE_RGB:
						default:
							break;
						}
						// Calc the gain.  If the user has requested a specific gain, use it.
						// Otherwise, maximize the pixel color while maintaining the color fidelity.
						if(m_fGain < 0.f)
						{
							fGain = max(fDeltaR, max(fDeltaG, fDeltaB));
							TempColor = FColor((1.f - (fGain - fDeltaR) / fGain), 
											(1.f - (fGain - fDeltaG) / fGain),
											(1.f - (fGain - fDeltaB) / fGain), 0.f);
						}
						else
						{
							TempColor = FColor(fDeltaR * m_fGain, fDeltaG * m_fGain, fDeltaB * m_fGain, 0.f);
						}
						TempColor.ConvertTo((FMT)Desc.Format.d3dfFormat, .5f, (char*)pRes, FALSE, ELEMENTINDEX_0);
					}
					else
					{
						memset(pRes, 0, dwBytesPerPixel);
					}

					pRes += dwBytesPerPixel;
				}
			}
			pSrc1 += dwBytesPerPixel;
			pSrc2 += dwBytesPerPixel;
		}
	}

	// Unlock the surfaces    
	pSourceSurface->UnlockRect();
	pRefSurface->UnlockRect();
	if(pResultSurface)
		pResultSurface->UnlockRect();

Exit:
	SAFEDELETEA(pSrcBits);
	SAFEDELETEA(pRefBits);


	return hr;
}

// --------------------------------------------------------------------------------------

FLOAT CImageCompare::GetCompareResult()
{
	UINT nTotalFail = m_nPerChannelFail + m_nPerPixelFail;
	float fResult = 0.f;

	// A specific number of pixels MUST have been rendered.
	if
	(
		m_uMinPixels > m_nTotalPixels
	)
	{
		DPF
		(
			1,
			_T("CImageCompare::GetCompareResult() - too few pixels rendered (%u).\n"),
			m_nTotalPixels
		);

		return 0.0f;
	};

	if
	(
		m_uMaxPixels < m_nTotalPixels
	)
	{
		DPF
		(
			1,
			_T("CImageCompare::GetCompareResult() - too many pixels rendered (%u).\n"),
			m_nTotalPixels
		);

		return 0.0f;
	};

	// Check for zero rendered pixels
	if (0 == m_nTotalPixels)
	{
		DPF(1, _T("CImageCompare::GetCompareResult() - Zero pixels rendered.\n"));
		return nTotalFail == 0 ? 1.0f : 0.f;
	};

	// We take the per-pixel percentages and substract off the total delta per channel.  If any
	// channel is off by >= PerChannelTol, then we will trigger a failure.
	fResult = (1.0f - ((float)nTotalFail / (float)m_nTotalPixels));
	fResult -= ( (m_fSumDeltaR + m_fSumDeltaG + m_fSumDeltaB) / ((float)m_nTotalPixels * 3.f) ) * m_fImgDeltaWght;

	return max(0, min(1, fResult));
}