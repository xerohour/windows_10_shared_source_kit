////////////////////////////////////////////////////////////////
// D3DFormatConverter.cpp
////////////////////////////////////////////////////////////////

#include "D3DFormatConverter.h"
#include "FormatDesc.hpp"

#include <tchar.h>

/******************************Public*Routine******************************\
* DibConvert420ToRGB
*
* Converts a 420 planar dib to RGB32.  Supported pixel formats are:
* IYUV, YV12 and NV12.
*
* History:
* Fri 01/18/2008 - StEstrop - Created
* 8/22/2013 - Not StEstrop - Templatized support for RGBA_COLOR<> and renamed 'Stride' to 'Pitch'
\**************************************************************************/
template<typename _FloatType>
HRESULT
Convert420ToRGB(
									DXGI_FORMAT resFormat, 
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth, 
									void *pData,
									UINT Pitch)
{
	LPBYTE lpBitsY = (LPBYTE)pData;
	LPBYTE lpBitsCr;
	LPBYTE lpBitsCb;
	int    iCbCrInc;
	LONG   lPitchCbCr;
#if 0
	switch (resFormat) {
	case (DXGI_FORMAT) FOURCC_IYUV:
		lPitchCbCr = DibWidthBytes(pSrcDib) / 2;
		iCbCrInc = 1;
		lpBitsCb = lpBitsY  +  (DibHeight(pSrcDib) * DibWidthBytes(pSrcDib));
		lpBitsCr = lpBitsCb + ((DibHeight(pSrcDib) * DibWidthBytes(pSrcDib)) / 4);
		break;

	case FOURCC_YV12:
		lPitchCbCr = DibWidthBytes(pSrcDib) / 2;
		iCbCrInc = 1;
		lpBitsCr = lpBitsY  +  (DibHeight(pSrcDib) * DibWidthBytes(pSrcDib));
		lpBitsCb = lpBitsCr + ((DibHeight(pSrcDib) * DibWidthBytes(pSrcDib)) / 4);
		break;

	case DXGI_FORMAT_NV12:
#endif
		lPitchCbCr = Pitch;
		iCbCrInc = 2;
		lpBitsCb = lpBitsY  +  uHeight * Pitch;
		lpBitsCr = lpBitsCb + 1;
#if 0
		break;

	default:
		return E_INVALIDARG;
	}
#endif

	RGBA_COLOR<_FloatType> * lpDibBits = pDataFloat;

	for (UINT y = 0; y < uHeight; y += 2) {

		LPBYTE lpLineY1 = lpBitsY;                      
		LPBYTE lpLineY2 = lpBitsY + Pitch;
		LPBYTE lpLineCr = lpBitsCr;
		LPBYTE lpLineCb = lpBitsCb;

		RGBA_COLOR<_FloatType> * lpDibLine1 = lpDibBits;
		RGBA_COLOR<_FloatType> * lpDibLine2 = lpDibBits + uWidth;

		for (UINT x = 0; x < uWidth; x += 2) {

			int  y0 = (int)lpLineY1[0];
			int  y1 = (int)lpLineY1[1];
			int  y2 = (int)lpLineY2[0];
			int  y3 = (int)lpLineY2[1];
			int  cb = (int)lpLineCb[0];
			int  cr = (int)lpLineCr[0];

			RGBQUAD r = CFormatHelper::ConvertYCrCbToRGB(y0, cr, cb);
			lpDibLine1->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			lpDibLine1->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			lpDibLine1->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			lpDibLine1->comp[3] = (_FloatType) 0          / (_FloatType) 255.0 ; // Alpha


			r = CFormatHelper::ConvertYCrCbToRGB(y1, cr, cb);
			(lpDibLine1+1)->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			(lpDibLine1+1)->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			(lpDibLine1+1)->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			(lpDibLine1+1)->comp[3] = (_FloatType) 0          / (_FloatType) 255.0 ; // Alpha


			r = CFormatHelper::ConvertYCrCbToRGB(y2, cr, cb);
			lpDibLine2->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			lpDibLine2->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			lpDibLine2->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			lpDibLine2->comp[3] = (_FloatType) 0          / (_FloatType) 255.0 ; // Alpha

			r = CFormatHelper::ConvertYCrCbToRGB(y3, cr, cb);
			(lpDibLine2+1)->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			(lpDibLine2+1)->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			(lpDibLine2+1)->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			(lpDibLine2+1)->comp[3] = (_FloatType) 0          / (_FloatType) 255.0 ; // Alpha

			lpLineY1 += 2;
			lpLineY2 += 2;
			lpLineCr += iCbCrInc;
			lpLineCb += iCbCrInc;

			lpDibLine1 += 2;
			lpDibLine2 += 2;
		}

		lpDibBits += (2 * uWidth);
		lpBitsY   += (2 * Pitch);
		lpBitsCr  += lPitchCbCr;
		lpBitsCb  += lPitchCbCr;
	}

	return S_OK;
}

/******************************Public*Routine******************************\
* DibConvertAYUVToRGB
*
* History:
*   03/24/2008 - mmaitre - Created
*   8/22/2013 - Not mmaitre - Templatized support for RGBA_COLOR<> and renamed 'Stride' to 'Pitch'
*
\**************************************************************************/
template<typename _FloatType>
HRESULT ConvertAYUVToRGB(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth, // BUGBUG - Depth not yet implemented
									void *pData,
									UINT Pitch)
{
	// Since uDepth is not yet used make sure nobody assumes it is
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;

	LPBYTE lpBits = (LPBYTE)pData;
	LPBYTE lpLine;

	RGBA_COLOR<_FloatType> * lpDibLine = pDataFloat;
	RGBA_COLOR<_FloatType> * lpDib;

	for (UINT y = 0; y < uHeight; y++) {

		lpLine = lpBits;
		lpDib = lpDibLine;

		for (UINT x = 0; x < uWidth; x ++) {

			// read a 1x1 block of YUY2 data
			const int  cr = lpLine[0];
			const int  cb = lpLine[1];
			const int  y  = lpLine[2];
			const BYTE a  = lpLine[3]; // Alpha

			// write a 1x1 block of RGB32 data
			RGBQUAD r = CFormatHelper::ConvertYCrCbToRGB(y, cr, cb);
			lpDib->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			lpDib->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			lpDib->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			lpDib->comp[3] = (_FloatType) 1.0 ; // Alpha

			lpLine += 4;
			lpDib  += 1;
		}

		lpBits    += Pitch;
		lpDibLine += uWidth;
	}

	return S_OK;
}

template<typename _FloatType>
BYTE GetYFromRGB(_FloatType R, _FloatType G, _FloatType B)
{
	return CFormatHelper::Clamp((float)((R*(_FloatType)255*0.257) + (G*(_FloatType)255*0.504) + (B*(_FloatType)255*0.098) + (_FloatType)16));
}

template<typename _FloatType>
BYTE GetCbFromRGB(_FloatType R, _FloatType G, _FloatType B)
{
	return CFormatHelper::Clamp((float)((R*(_FloatType)255*-0.148) - (G*(_FloatType)255*0.291) + (B*(_FloatType)255*0.439) + (_FloatType)128));
}

template<typename _FloatType>
BYTE GetCrFromRGB(_FloatType R, _FloatType G, _FloatType B)
{
	return CFormatHelper::Clamp((float)((R*(_FloatType)255*0.439) - (G*(_FloatType)255*0.368) - (B*(_FloatType)255*0.071) + (_FloatType)128));
}


// Reference: http://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
template<typename _FloatType>
HRESULT ConvertRGBToYUY2(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 ) return E_INVALIDARG; // Width must be even

	// YUY2 is 2:1 horizontally downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	BYTE *curPosOut = (BYTE*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			*curPosOut = GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2);
			*(curPosOut+1) = GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b);
			*(curPosOut+2) = GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2);   
			*(curPosOut+3) = GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
				   
			// Advance to next pixel (half width)
			curPosIn += 2;
			curPosOut += 4;
		}
		
		curPosOut += Pitch - (uWidth * 2); //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToNV12(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 || uHeight % 2 ) return E_INVALIDARG; // Width and height must be even

	// NV12 is 2:1 horizontally downsampled, and 2:1 vertically downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;

	BYTE *curPosOutY = (BYTE*)pData;
	BYTE *curPosOutUV = ((BYTE*)pData + (Pitch * uHeight)); 

	for (UINT i = 0 ; i < uHeight; i+=2)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			*curPosOutY = GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOutY+1) = GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b);
			*(curPosOutY+Pitch) = GetYFromRGB<_FloatType>((curPosIn+uWidth)->r, (curPosIn+uWidth)->g, (curPosIn+uWidth)->b);     
			*(curPosOutY+Pitch+1) = GetYFromRGB<_FloatType>((curPosIn+uWidth+1)->r, (curPosIn+uWidth+1)->g, (curPosIn+uWidth+1)->b);

			// Chroma is sampled from the average of the leftmost two elements (horizontally co-sited)
			*curPosOutUV = GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
													(curPosIn->g + (curPosIn+uWidth)->g) / 2,
													(curPosIn->b + (curPosIn+uWidth)->b) / 2);
			*(curPosOutUV+1) = GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
														(curPosIn->g + (curPosIn+uWidth)->g) / 2,
														(curPosIn->b + (curPosIn+uWidth)->b) / 2);
				   
			//Advance to next pixel (half width)
			curPosIn += 2;
			curPosOutY += 2;
			curPosOutUV += 2;
		}

		curPosIn += uWidth; // Skip one row (half height), next row was already sampled above
		curPosOutY += Pitch - uWidth + Pitch; //Fast forward remaining part of the stride, plus one more row (we've filled in two rows above)
		curPosOutUV += Pitch - uWidth; //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToNV11(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 4 ) return E_INVALIDARG; // Width must be multiple of 4

	// NV11 is 4:1 horizontally downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;

	BYTE *curPosOutY = (BYTE*)pData;
	BYTE *curPosOutUV = ((BYTE*)pData + (Pitch * uHeight)); 

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j+=4)
		{
			*curPosOutY = GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOutY+1) = GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b);
			*(curPosOutY+2) = GetYFromRGB<_FloatType>((curPosIn+2)->r, (curPosIn+2)->g, (curPosIn+2)->b);  
			*(curPosOutY+3) = GetYFromRGB<_FloatType>((curPosIn+3)->r, (curPosIn+3)->g, (curPosIn+3)->b);

			// Chroma is sampled from the leftmost element (co-sited)
			*curPosOutUV = GetCbFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOutUV+1) = GetCrFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
				   
			//Advance to next pixel (quarter width)
			curPosIn += 4;
			curPosOutY += 4;
			curPosOutUV += 2;
		}

		curPosOutY += Pitch - uWidth; //Fast forward remaining part of the stride
		if ( i % 2 )
		{
			curPosOutUV += Pitch - uWidth; //Fast forward remaining part of the stride
		}
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToAYUV(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;

	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	BYTE *curPosOut = (BYTE*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j++)
		{
			*curPosOut = GetCrFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOut+1) = GetCbFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOut+2) = GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b);
			*(curPosOut+3) = (BYTE)(curPosIn->a * (_FloatType)255);
				   
			//Advance to next pixel
			curPosIn++;
			curPosOut += 4;
		}

		curPosOut += Pitch - (uWidth * 4); //Fast forward remaining part of the stride
	}

	return S_OK;
}

// Reference: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578(v=vs.85).aspx
template<typename _FloatType>
HRESULT ConvertRGBToY410(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;

	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	DWORD *curPosOut = (DWORD*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j++)
		{		
			DWORD Cb, Y, Cr, A;

			Cb = (DWORD)GetCbFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 2;
			Y = (DWORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 12;
			Cr = (DWORD)GetCrFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 22;
			A = (DWORD)(curPosIn->a * (_FloatType)3) << 30;

			*curPosOut = A | Cr | Y | Cb;
				   
			//Advance to next pixel
			curPosIn++;
			curPosOut++;
		}

		curPosOut += (Pitch - (uWidth * 4)) / 4; //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToY416(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;

	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	WORD *curPosOut = (WORD*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j++)
		{
			*curPosOut = (WORD)GetCbFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
			*(curPosOut+1) = (WORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
			*(curPosOut+2) = (WORD)GetCrFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
			*(curPosOut+3) = (WORD)(curPosIn->a * (_FloatType)255) << 8;
				   
			//Advance to next pixel
			curPosIn++;
			curPosOut += 4;
		}

		curPosOut += (Pitch - (uWidth * 8)) / 2; //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToY216(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 ) return E_INVALIDARG; // Width must be even

	// Y216 is 2:1 horizontally downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	WORD *curPosOut = (WORD*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			// Chroma is sampled as an average of the two elements (centered)
			*curPosOut = (WORD)GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2) << 8;
			*(curPosOut+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b) << 8;
			*(curPosOut+2) = (WORD)GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2) << 8;
			*(curPosOut+3) = (WORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
				   
			// Advance to next pixel (half width)
			curPosIn += 2;
			curPosOut += 4;
		}

		curPosOut += (Pitch - (uWidth * 4)) / 2; //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToY210(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 ) return E_INVALIDARG; // Width must be even

	// Y210 is 2:1 horizontally downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;
	WORD *curPosOut = (WORD*)pData;          

	for (UINT i = 0 ; i < uHeight; i++)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			// Chroma is sampled as an average of the two elements (centered)
			*curPosOut = (WORD)GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2) << 8;
			*(curPosOut+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b) << 8;
			*(curPosOut+2) = (WORD)GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+1)->r) / 2, (curPosIn->g + (curPosIn+1)->g) / 2, (curPosIn->b + (curPosIn+1)->b) / 2) << 8;
			*(curPosOut+3) = (WORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
				   
			// Advance to next pixel (half width)
			curPosIn += 2;
			curPosOut += 4;
		}

		curPosOut += (Pitch - (uWidth * 4)) / 2; //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToP016(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 || uHeight % 2 ) return E_INVALIDARG; // Width and height must be even

	// P016 is 2:1 horizontally downsampled, and 2:1 vertically downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;

	WORD *curPosOutY = (WORD*)pData;
	WORD *curPosOutUV = (WORD*)((BYTE*)pData + (Pitch * uHeight)); 

	for (UINT i = 0 ; i < uHeight; i+=2)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			*curPosOutY = (WORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
			*(curPosOutY+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b) << 8;
			*(curPosOutY+Pitch) = (WORD)GetYFromRGB<_FloatType>((curPosIn+uWidth)->r, (curPosIn+uWidth)->g, (curPosIn+uWidth)->b) << 8;
			*(curPosOutY+Pitch+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+uWidth+1)->r, (curPosIn+uWidth+1)->g, (curPosIn+uWidth+1)->b) << 8;

			// Chroma is sampled from the average of the leftmost two elements (horizontally co-sited)
			*curPosOutUV = (WORD)GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
													(curPosIn->g + (curPosIn+uWidth)->g) / 2,
													(curPosIn->b + (curPosIn+uWidth)->b) / 2) << 8;
			*(curPosOutUV+1) = (WORD)GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
														(curPosIn->g + (curPosIn+uWidth)->g) / 2,
														(curPosIn->b + (curPosIn+uWidth)->b) / 2) << 8;
				   
			//Advance to next pixel (half width)
			curPosIn += 2;
			curPosOutY += 2;
			curPosOutUV += 2;
		}

		curPosIn += uWidth; // Skip one row (half height), next row was already sampled above
		curPosOutY += Pitch - (uWidth * 2) + Pitch; //Fast forward remaining part of the stride, plus one more row (we've filled in two rows above)
		curPosOutUV += Pitch - (uWidth * 2); //Fast forward remaining part of the stride
	}

	return S_OK;
}

template<typename _FloatType>
HRESULT ConvertRGBToP010(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
									UINT Pitch
									)
{
	assert( 1 == uDepth);
	if ( 1 != uDepth ) return E_NOTIMPL;
	if ( uWidth % 2 || uHeight % 2 ) return E_INVALIDARG; // Width and height must be even

	// P010 is 2:1 horizontally downsampled, and 2:1 vertically downsampled
	RGBA_COLOR<_FloatType> *curPosIn = pDataFloat;

	WORD *curPosOutY = (WORD*)pData;
	WORD *curPosOutUV = (WORD*)((BYTE*)pData + (Pitch * uHeight)); 

	for (UINT i = 0 ; i < uHeight; i+=2)
	{
		for (UINT j = 0; j < uWidth; j+=2)
		{
			*curPosOutY = (WORD)GetYFromRGB<_FloatType>(curPosIn->r, curPosIn->g, curPosIn->b) << 8;
			*(curPosOutY+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+1)->r, (curPosIn+1)->g, (curPosIn+1)->b) << 8;
			*(curPosOutY+Pitch) = (WORD)GetYFromRGB<_FloatType>((curPosIn+uWidth)->r, (curPosIn+uWidth)->g, (curPosIn+uWidth)->b) << 8;
			*(curPosOutY+Pitch+1) = (WORD)GetYFromRGB<_FloatType>((curPosIn+uWidth+1)->r, (curPosIn+uWidth+1)->g, (curPosIn+uWidth+1)->b) << 8;

			// Chroma is sampled from the average of the leftmost two elements (horizontally co-sited)
			*curPosOutUV = (WORD)GetCbFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
													(curPosIn->g + (curPosIn+uWidth)->g) / 2,
													(curPosIn->b + (curPosIn+uWidth)->b) / 2) << 8;
			*(curPosOutUV+1) = (WORD)GetCrFromRGB<_FloatType>((curPosIn->r + (curPosIn+uWidth)->r) / 2, 
														(curPosIn->g + (curPosIn+uWidth)->g) / 2,
														(curPosIn->b + (curPosIn+uWidth)->b) / 2) << 8;
				   
			//Advance to next pixel (half width)
			curPosIn += 2;
			curPosOutY += 2;
			curPosOutUV += 2;
		}

		curPosIn += uWidth; // Skip one row (half height), next row was already sampled above
		curPosOutY += Pitch - (uWidth * 2) + Pitch; //Fast forward remaining part of the stride, plus one more row (we've filled in two rows above)
		curPosOutUV += Pitch - (uWidth * 2); //Fast forward remaining part of the stride
	}

	return S_OK;
}


/*****************************Private*Routine******************************\
* ConvertYUY2ToRGB
*
*
*
* History:
* Fri 12/07/2007 - StEstrop - Created
* 8/22/2013 - Not StEstrop - Templatized support for RGBA_COLOR<> and renamed 'Stride' to 'Pitch'
*
\**************************************************************************/
template<typename _FloatType>
HRESULT ConvertYUY2ToRGB(
									RGBA_COLOR<_FloatType> *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth, // BUGBUG - Depth not yet implemented
									void *pData,
									UINT Pitch
									)
{
	// Since uDepth is not yet used make sure nobody assumes it is
	assert( 1 == uDepth);
	if ( 1 != uDepth )
	{
		return E_NOTIMPL;
	}

	LPBYTE lpBits = (LPBYTE)pData;
	LPBYTE lpLine = nullptr;

	for (UINT y = 0; y < uHeight; y++) {

		lpLine = lpBits;
		RGBA_COLOR<_FloatType> * lpDib = pDataFloat;

		for (UINT x = 0; x < uWidth; x += 2) {

			int  y0 = (int)lpLine[0];
			int  cb = (int)lpLine[1];
			int  y1 = (int)lpLine[2];
			int  cr = (int)lpLine[3];

			RGBQUAD r = CFormatHelper::ConvertYCrCbToRGB(y0, cr, cb);
			lpDib->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			lpDib->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			lpDib->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			lpDib->comp[3] = (_FloatType) 1.0 ; // Alpha


			r = CFormatHelper::ConvertYCrCbToRGB(y1, cr, cb);
			(lpDib+1)->comp[0] = (_FloatType) r.rgbRed   / (_FloatType) 255.0 ;
			(lpDib+1)->comp[1] = (_FloatType) r.rgbGreen / (_FloatType) 255.0 ;
			(lpDib+1)->comp[2] = (_FloatType) r.rgbBlue  / (_FloatType) 255.0 ;
			(lpDib+1)->comp[3] = (_FloatType) 1.0 ; // Alpha

			lpLine += 4;
			lpDib  += 2;
		}

		lpBits     += Pitch;
		pDataFloat += uWidth;
	}

	return S_OK;
}

HRESULT ConvertYUVPixels( CONVERSION_TYPE convType, 
									DXGI_FORMAT resFormat, 
									FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth, 
									void *pData,
									UINT Pitch )
{
	if ( convType == CONV_FROM_FORMAT )
	{
		switch ( resFormat )
		{
		case DXGI_FORMAT_YUY2:
			return ConvertYUY2ToRGB( pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
		case DXGI_FORMAT_NV12:
			return Convert420ToRGB( resFormat, pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
		case DXGI_FORMAT_AYUV:
			return ConvertAYUVToRGB( pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
		default:
			assert( false && "Unhandled YUV format." );
			return E_NOTIMPL;
		}
	}
	else
	{
		assert( false && "Unhandled conversion type." );
		return E_NOTIMPL;
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////
// class CImageDataHelperParams
//
// Provides useful information about resource structure based on pixel format
////////////////////////////////////////////////////////////////
class CImageDataHelperParams
{
public:
	UINT components[4];
	UINT componentTypes[4];
	UINT componentBits[4];
	UINT numComponents;
	UINT numComponentsIncludingFiller; // includes X channel for some formats such as BGRX
	UINT bytesPerElement;
	UINT firstComponent;
	UINT lastComponent;
	bool compressed;
	bool hasAlpha;
	bool packed;
	bool planar;
	UINT widthInterval;
	UINT heightInterval;

public:
	CImageDataHelperParams(DXGI_FORMAT format)
	{
		try
		{
			numComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( format );
		}
		catch( HRESULT e )
		{
			OutputDebugString( _T( "CD3DFormatConverter::Init() - Format not available in CD3D10FormatHelper." ) );
			throw; // Rethrow
		}

		numComponentsIncludingFiller = 0;
		bool bSeenFirstComponent = false;
		firstComponent = 4; // initialize to bad value so default loop does nothing
		lastComponent = 0;
		for( UINT i = 0; i < 4; ++i ) 
		{
			components[ i ] = (UINT) CD3D10FormatHelper_Test::GetComponentName( format, i );
			componentBits[ i ] = (UINT) CD3D10FormatHelper_Test::GetBitsPerComponent( format, i );
			if( componentBits[ i ] && (components[ i ] != D3D10FCN_X) )
			{
				if( !bSeenFirstComponent )
				{
					bSeenFirstComponent = true;
					firstComponent = i;
				}
				lastComponent = i;
			}
			if( componentBits[ i ] )
			{
				numComponentsIncludingFiller = i+1;
			}
			componentTypes[ i ] = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( format, i );
			switch( componentTypes[ i ] )
			{
			case D3D10FCI_FLOAT:
			case D3D10FCI_SNORM:
			case D3D10FCI_UNORM:
			case D3D10FCI_TYPELESS:
			case D3D10FCI_UNORM_SRGB:
			case D3D10FCI_SINT:
			case D3D10FCI_UINT:
			case D3D10FCI_BIASED_FIXED_2_8:
				break;
			default:
				assert( false && "Unsupported component data type." );
				throw(E_NOTIMPL);
			}
		}
	
		bytesPerElement = CD3D10FormatHelper_Test::GetBitsPerElement( format ) / 8;

		compressed = false;
		switch( format )
		{
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:   
		case DXGI_FORMAT_G8R8_G8B8_UNORM:   
		case DXGI_FORMAT_BC1_TYPELESS:      
		case DXGI_FORMAT_BC1_UNORM:      
		case DXGI_FORMAT_BC1_UNORM_SRGB: 
		case DXGI_FORMAT_BC2_TYPELESS:      
		case DXGI_FORMAT_BC2_UNORM:      
		case DXGI_FORMAT_BC2_UNORM_SRGB: 
		case DXGI_FORMAT_BC3_TYPELESS:      
		case DXGI_FORMAT_BC3_UNORM:      
		case DXGI_FORMAT_BC3_UNORM_SRGB: 
		case DXGI_FORMAT_BC4_TYPELESS:      
		case DXGI_FORMAT_BC4_UNORM:      
		case DXGI_FORMAT_BC4_SNORM:      
		case DXGI_FORMAT_BC5_TYPELESS:      
		case DXGI_FORMAT_BC5_UNORM:      
		case DXGI_FORMAT_BC5_SNORM:     
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			compressed = true;
		}

		planar = false;
		switch( format )
		{
		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case DXGI_FORMAT_NV11:
			planar = true;
		}

		packed = false;
		switch( format )
		{
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			packed = true;
		}


		hasAlpha = false;
		for( UINT i = 0; i < numComponents; ++i )
		{
			switch(components[i])
			{
			case D3D10FCN_R:
			case D3D10FCN_G:
			case D3D10FCN_B:
			case D3D10FCN_S:
			case D3D10FCN_D:
				break;
			case D3D10FCN_A:
			case D3D10FCN_X:
				hasAlpha = true;
				break;
			default:
				assert( false && "Unsupported component/channel." );
				throw(E_NOTIMPL);
			}
		}


		widthInterval = 1;
		heightInterval = 1;
		switch( format )
		{
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			widthInterval = 2;
			break;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case DXGI_FORMAT_NV11:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			widthInterval = 2;
			heightInterval = 2;
			break;

		case DXGI_FORMAT_YUY2:
			widthInterval = 2;
			break;
		}
	}
};

////////////////////////////////////////////////////////////////
// DecodeCompressed()
//
// Decodes a compressed resource to FLOAT_COLOR format
////////////////////////////////////////////////////////////////
static HRESULT DecodeCompressed( 
	DXGI_FORMAT resFormat, 
	const FLOAT_COLOR *pDataFloat, 
	UINT uHeight, 
	UINT uWidth, 
	UINT uDepth, 
	void *pData,
	UINT rowPitch,
	UINT slicePitch
	)
{
	return CFormatHelper::DecodeCompressed( resFormat, pDataFloat, uHeight, uWidth, uDepth, pData, rowPitch, slicePitch ) ? S_OK : E_FAIL;
}

////////////////////////////////////////////////////////////////
// DecodeCompressed()
//
// Decodes a compressed resource to DOUBLE_COLOR format
////////////////////////////////////////////////////////////////
static HRESULT DecodeCompressed( 
	DXGI_FORMAT resFormat, 
	const DOUBLE_COLOR *pDataFloat, 
	UINT uHeight, 
	UINT uWidth, 
	UINT uDepth, 
	void *pData,
	UINT rowPitch,
	UINT slicePitch
	)
{
	assert( false && !"Cannot decode block compressed data to double-precision color data.");

	// Not implemented for double-precision floating point data
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////
// EncodeCompressed()
//
// Encodes a compressed resource from FLOAT_COLOR format
////////////////////////////////////////////////////////////////
static HRESULT EncodeCompressed( 
	DXGI_FORMAT resFormat, 
	const FLOAT_COLOR *pDataFloat, 
	UINT uHeight, 
	UINT uWidth, 
	UINT uDepth, 
	void *pData,
	UINT rowPitch,
	UINT slicePitch
	)
{
	return CFormatHelper::EncodeCompressed( resFormat, pDataFloat, uHeight, uWidth, uDepth, pData, rowPitch, slicePitch ) ? S_OK : E_FAIL;
}

////////////////////////////////////////////////////////////////
// EncodeCompressed()
//
// Encodes a compressed resource from DOUBLE_COLOR format
////////////////////////////////////////////////////////////////
static HRESULT EncodeCompressed( 
	DXGI_FORMAT resFormat, 
	const DOUBLE_COLOR *pDataFloat, 
	UINT uHeight, 
	UINT uWidth, 
	UINT uDepth, 
	void *pData,
	UINT rowPitch,
	UINT slicePitch
	)
{
	assert( false && !"Cannot encode block compressed data from double-precision color data." );

	// Not implemented for double-precision floating point data
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////
// ConvertPixelToFloat()
//
// Templatized method used by CD3DFormatConverter class to convert a single pixel from
// a DXGI pixel format to either FLOAT_COLOR or DOUBLE_COLOR format.
////////////////////////////////////////////////////////////////
template<typename _FloatType>
void ConvertPixelToFloat(
		RGBA_COLOR<_FloatType> *pFloatColor, 
		void *pData,
		UINT numComponentsIncludingFiller,
		UINT bytesPerElement,
		UINT firstComponent,
		UINT lastComponent,
		D3D10_FORMAT_COMPONENT_NAME *components,
		D3D10_FORMAT_COMPONENT_INTERPRETATION *componentTypes,
		UINT *componentBits
		)
{
	UINT uCurrentBit = 0;
	UINT32 *pCurrentDWORD = (UINT32*) pData;
	UINT32 zero = 0;
	if( bytesPerElement < 4 )
	{
		pCurrentDWORD = &zero;
		memcpy( pCurrentDWORD, pData, bytesPerElement );
	}

	// Init components to default values
	pFloatColor->r = (_FloatType) 0.0;
	pFloatColor->g = (_FloatType) 0.0;
	pFloatColor->b = (_FloatType) 0.0;
	pFloatColor->a = (_FloatType) 1.0;

	for( UINT iCmp = firstComponent; iCmp <= lastComponent; ++iCmp )
	{
		if( uCurrentBit >= 32 )
		{
			pCurrentDWORD++;
			uCurrentBit = uCurrentBit % 32;
		}

		_FloatType fCompValue = (_FloatType) 0.0;

		// create mask for current component...
		UINT bits = componentBits[ iCmp ];
		UINT32 uCompMask = ((1LL << bits) - 1);

		// ...and grab the value
		UINT32 uCompData = (*pCurrentDWORD >> uCurrentBit) & uCompMask;
		D3D10_FORMAT_COMPONENT_INTERPRETATION componentType = (D3D10_FORMAT_COMPONENT_INTERPRETATION) componentTypes[iCmp];
		switch( componentType )
		{
		case D3D10FCI_FLOAT:
			if( bits == 16 )
			{
				float16 val;
				val.v = (UINT16) uCompData;
				fCompValue = (_FloatType) (float) val;
			}
			else if( bits == 11 )
			{
				float11 val;
				val.v = (UINT16) uCompData;
				fCompValue = (_FloatType) (float) val;
			}
			else if( bits == 10 )
			{
				float10 val;
				val.v = (UINT16) uCompData;
				fCompValue = (_FloatType) (float) val;
			}
			else if(bits == 32)
			{
				fCompValue = (_FloatType) *( (float*) &uCompData );
			}
			else
			{
				throw(std::exception("Invalid floating point bit count"));
			}
			break;
		case D3D10FCI_SNORM:
			if ( uCompData == (1LL << (bits-1)) )
			{
				// Force the most negative value to -1.0
				fCompValue = -1.0;
			}
			else
			{
				int val = 0;

				if( (uCompData & ((UINT64)1 << (bits-1))) )
					val = -(int)( ( uCompData ^ ((1LL << bits) - 1) ) + 1 );
				else
					val = uCompData;
				fCompValue = (float) val * ( 1.f / (float)((1 << (bits-1)) - 1) );
			}
			break;
		case D3D10FCI_UNORM:
		case D3D10FCI_TYPELESS:
			fCompValue = (_FloatType) uCompData;
			fCompValue = fCompValue / ((1LL << (bits)) - 1 );
			break;
		case D3D10FCI_UNORM_SRGB:
			fCompValue = (_FloatType) uCompData / ((1LL << (bits)) - 1 );
			if( components[ iCmp ] != D3D10FCN_A )
			{
				// Convert to linear space
				if( fCompValue <= 0.04045 )
					fCompValue /= (_FloatType) 12.92;
				else
					fCompValue = pow( ( ( fCompValue + (_FloatType) 0.055 ) / (_FloatType) 1.055 ), (_FloatType) 2.4 );
			}
			break;
		case D3D10FCI_SINT:
			if( (uCompData & (1LL << (bits-1))) )
				fCompValue = (_FloatType) -(int)( ( uCompData ^ ((1LL << (bits)) - 1) ) + 1 );
			else
				fCompValue = (_FloatType) uCompData;
			break;
		case D3D10FCI_UINT:
			fCompValue = (_FloatType) uCompData;
			break;
		case D3D10FCI_BIASED_FIXED_2_8:
			fCompValue = (_FloatType)( uCompData - 0x100 ) / (_FloatType) 255.0;
			break;
		}

		switch( components[ iCmp ] )
		{
		case D3D10FCN_R:
		case D3D10FCN_D:
			pFloatColor->r = fCompValue;
			break;
		case D3D10FCN_G:
		case D3D10FCN_S:
			pFloatColor->g = fCompValue;
			break;
		case D3D10FCN_B:
			pFloatColor->b = fCompValue;
			break;
		case D3D10FCN_A:
		case D3D10FCN_X:
			pFloatColor->a = fCompValue;
			break;
		}

		uCurrentBit += componentBits[ iCmp ];
	}
}

////////////////////////////////////////////////////////////////
// ConvertFloatToPixel()
//
// Templatized method used by CD3DFormatConverter class to convert a single pixel from
// either FLOAT_COLOR or DOUBLE_COLOR format to DXGI pixel format.
////////////////////////////////////////////////////////////////
template<typename _FloatType>
void ConvertFloatToPixel(
		RGBA_COLOR<_FloatType> *pFloatColor, 
		void *pData,
		UINT numComponentsIncludingFiller,
		UINT bytesPerElement,
		D3D10_FORMAT_COMPONENT_NAME *components,
		D3D10_FORMAT_COMPONENT_INTERPRETATION *componentTypes,
		UINT *componentBits
		)
{
	UINT32 pValue[ 4 ] = { 0 };
	UINT bitsUsed = 0;

	for( UINT iCmp = 0; iCmp < numComponentsIncludingFiller; ++iCmp )
	{
		UINT32 cmpVal = 0;
		_FloatType cmpFloatType = 0.0;
		float cmpFloat32 = 0.0;
		UINT32 uMax = 0xffffffff;

		D3D10_FORMAT_COMPONENT_NAME component = components[iCmp];

		switch( component )
		{
		case D3D10FCN_R:
		case D3D10FCN_D:
			cmpFloatType = pFloatColor->r;
			break;
		case D3D10FCN_G:
		case D3D10FCN_S:
			cmpFloatType = pFloatColor->g;
			break;
		case D3D10FCN_B:
			cmpFloatType = pFloatColor->b;
			break;
		case D3D10FCN_A:
			cmpFloatType = pFloatColor->a;
			break;
		case D3D10FCN_X:
			cmpFloatType = 1.0;
			break;
		}

		cmpFloat32 = (float) cmpFloatType;
		bool isNaN = CFormatHelper::IsNaN( cmpFloat32 );

		UINT32 floatBits = *( (UINT32*) &cmpFloat32 );
		int exp = 0;

		UINT bits = componentBits[ iCmp ];
		D3D10_FORMAT_COMPONENT_INTERPRETATION componentType = componentTypes[iCmp];
		switch( componentType )
		{
		// Floating point formats convert from cmpFloat32 rather than cmpFloatType
		case D3D10FCI_FLOAT:
			if( bits == 16 )
			{
				if( cmpFloat32 > MAX_FLOAT16 )
					cmpFloat32 = MAX_FLOAT16;
				else if( cmpFloat32 < MIN_FLOAT16 )
					cmpFloat32 = MIN_FLOAT16;
				cmpVal = float16::make( &cmpFloat32 ).GetBits( );

				if( floatBits == INFP_FLOAT32 )
					cmpVal = INFP_FLOAT16;
				else if( floatBits == INFN_FLOAT32 )
					cmpVal = INFN_FLOAT16;
				else if( CFormatHelper::IsDenorm( cmpFloat32 ) )
					cmpVal = DENORM_FLOAT16;
				else if( CFormatHelper::IsNaN( cmpFloat32 ) )
					cmpVal = NAN_FLOAT16;
			}
			else if( bits == 11 )
			{					
				if( cmpFloat32 > MAX_FLOAT11 )
					cmpFloat32 = MAX_FLOAT11;
				else if( cmpFloat32 < MIN_FLOAT11 )
					cmpFloat32 = MIN_FLOAT11;
				cmpVal = float11::make( &cmpFloat32 ).GetBits( );

				if( floatBits == INFP_FLOAT32 )
					cmpVal = INFP_FLOAT11;
				else if( floatBits == INFN_FLOAT32 )
					cmpVal = INFN_FLOAT11;
				else if( CFormatHelper::IsDenorm( cmpFloat32 ) )
					cmpVal = DENORM_FLOAT11;
				else if( CFormatHelper::IsNaN( cmpFloat32 ) )
					cmpVal = NAN_FLOAT11;
			}
			else if( bits == 10 )
			{
				if( cmpFloat32 > MAX_FLOAT10 )
					cmpFloat32 = MAX_FLOAT10;
				else if( cmpFloat32 < MIN_FLOAT10 )
					cmpFloat32 = MIN_FLOAT10;
				cmpVal = float10::make( &cmpFloat32 ).GetBits( );

				if( floatBits == INFP_FLOAT32 )
					cmpVal = INFP_FLOAT10;
				else if( floatBits == INFN_FLOAT32 )
					cmpVal = INFN_FLOAT10;
				else if( CFormatHelper::IsDenorm( cmpFloat32 ) )
					cmpVal = DENORM_FLOAT10;
				else if( CFormatHelper::IsNaN( cmpFloat32 ) )
					cmpVal = NAN_FLOAT10;
			}
			else
				cmpVal = *( (UINT32*) &cmpFloat32 );
			break;
		case D3D10FCI_SNORM:
			if(!isNaN)
			{
				// Clamp value between -1.0 and 1.0...
				cmpFloatType = min(max((_FloatType) -1.0, cmpFloatType), (_FloatType) 1.0);
				// ...then convert to integer scale...
				cmpFloatType = cmpFloatType * ( ( 1 << (bits - 1) ) - 1 );
				// ...then add (subtract if negative) 0.5 for rounding...
				cmpFloatType = (cmpFloatType >= (_FloatType) 0.0) ? cmpFloatType + (_FloatType) 0.5 : cmpFloatType - (_FloatType) 0.5;
				// ...and convert to a signed integer
				cmpVal = (UINT32) (int)cmpFloatType;
				// ...and finally mask out the excess bits
				cmpVal &= (1LL<<bits)-1;
			}
			break;
		case D3D10FCI_UNORM:
		case D3D10FCI_TYPELESS:
			if(!isNaN)
			{
				// Clamp value between 0.0 and 1.0...
				cmpFloatType = min(max((_FloatType) 0.0, cmpFloatType), (_FloatType) 1.0);
				// ...convert to double and multiply by our shifted number of bits...
				double bigFloat = cmpFloatType;
				bigFloat = bigFloat * ( ( 1LL << bits ) - 1 );
				// ...then add 0.5 for rounding...
				bigFloat += 0.5;
				// ...and convert to integer
				cmpVal = (UINT32) bigFloat;
			}
			break;
		case D3D10FCI_UNORM_SRGB:
			if(!isNaN)
			{
				// Clamp value between 0.0 and 1.0...
				cmpFloatType = min(max((_FloatType) 0.0, cmpFloatType), (_FloatType) 1.0);
				// ...then convert to SRGB space...
				if( component != D3D10FCN_A )
				{
					if( cmpFloatType <= 0.0031308 )
						cmpFloatType *= (_FloatType) 12.92;
					else
						cmpFloatType = (_FloatType) 1.055 * pow( cmpFloatType, (_FloatType) 1.0 / (_FloatType) 2.4 ) - (_FloatType) 0.055;
				}
				// ...convert to double and multiply by our shifted number of bits...
				double bigFloat = cmpFloatType;
				bigFloat = bigFloat * ( ( 1LL << bits ) - 1 );
				// ...add 0.5 for rounding
				bigFloat += 0.5;
				// ...and convert to integer
				cmpVal = (UINT32) bigFloat;
			}
			break;
		case D3D10FCI_SINT:
			if(!isNaN)
			{
				int minInt = -((1<<(bits-1)));
				_FloatType minFloat = (_FloatType) minInt;
				if( cmpFloatType < minFloat)
				{
					cmpVal = minInt;
				}
				else 
				{
					int maxInt = ((1<<(bits-1))-1);
					_FloatType maxFloat = (_FloatType) maxInt;

					if( cmpFloatType > maxFloat )
					{
						cmpVal = maxInt;
					}
					else
					{
						cmpVal = (INT32) cmpFloatType;
					}
				}

				// Keep low <bits> bits, zero others.
				cmpVal &= ((UINT64)1<<bits)-1;
			}
			break;
		case D3D10FCI_UINT:
			if(!isNaN)
			{
				if( cmpFloatType < 0.f )
				{
					cmpVal = 0;
				}
				else
				{
					UINT maxInt = ((1LL<<bits)-1);
					_FloatType maxFloat = (_FloatType) maxInt;

					if( cmpFloatType > maxFloat)
					{
						cmpVal = maxInt;
					}
					else
					{
						cmpVal = (UINT32) cmpFloatType;
					}
				}
			}

			break;
		case D3D10FCI_BIASED_FIXED_2_8:
			if(!isNaN)
			{
				cmpFloat32 = (cmpFloat32 * 255.f) + 256.f;
				exp = ( ( *( (UINT32*) &cmpFloat32 ) & MASK_EXP_FLOAT32 ) >> SHIFT_EXP_FLOAT32 ) - 127;
				cmpVal = ( *( (UINT32*) &cmpFloat32 ) & 0x007fe000 ); // 10 most-significant-bits are the result
			}
			break;
		}

		UINT bitsUsedWithin32 = bitsUsed % 32;
		pValue[ bitsUsed / 32 ] |= ( cmpVal << bitsUsedWithin32 );	
		bitsUsed += bits;
	}

	memcpy( pData, pValue, bytesPerElement );
}

////////////////////////////////////////////////////////////////
// ConvertToFloat()
//
// Templatized method used by CD3DFormatConverter class to convert resource data from
// a DXGI pixel format to either FLOAT_COLOR or DOUBLE_COLOR format.
////////////////////////////////////////////////////////////////
template<typename _FloatType> 
static HRESULT ConvertToFloat(
		RGBA_COLOR<_FloatType> *pFloatColorBuffer, 
		size_t floatBufferElementCount, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of source region
		UINT top,			// Y-coordinate of top row of source region
		UINT right,			// X-coordinate of one-past-the-right column of source region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of source region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in source image (must be greater-than-or-equal-to image width or 0 if source is 1D)
		UINT front,			// Z-coordinate of first slice of source region (for 3-dimensional resource)
		UINT back,			// Z-coordinate of one-past-the-last slice of source region
		UINT slicePitch 	// Memory pitch between slices in 3-dimensional source (can be 0 if source image is 1D or 2D)
	)
{
	UINT convertWidth = right - left;
	UINT convertHeight = bottom - top;
	UINT convertDepth = back - front;
	UINT32 uNumElements = convertHeight * convertWidth * convertDepth;
	if( uNumElements == 0 )
	{
		assert( false && "Region to be converted cannot have height, width, or depth of zero." );
		return E_INVALIDARG;
	}

	CImageDataHelperParams helperParams(format);

	if( helperParams.compressed )
	{
		return DecodeCompressed( format, pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch, slicePitch );
	}

	if(0 == rowPitch)
		rowPitch = convertWidth * helperParams.bytesPerElement;

	if(0 == slicePitch)
		slicePitch = rowPitch * convertHeight;

	assert(rowPitch >= convertWidth * helperParams.bytesPerElement);
	assert(slicePitch >= rowPitch * convertHeight);


	//
	// Handle specific packed and planar YUV formats.
	//

	switch ( format )
	{
	case DXGI_FORMAT_YUY2:
		return ConvertYUY2ToRGB( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_NV12:
		return Convert420ToRGB( format, pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_AYUV:
		return ConvertAYUVToRGB( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_420_OPAQUE:
		assert( false && "Unhandled format." );
		return E_NOTIMPL;
	}


	//
	// Note that other packed and planar formats are unsupported for now.
	//

	if ( helperParams.packed == true || helperParams.planar == true )
	{
		return E_NOTIMPL;
	}


	int sign = 0;
	int val = 0;
	size_t iElem = 0;

	// For each slice...
	for(UINT slice = front; slice < back; ++slice)
	{
		BYTE *pSrcSlice = ((BYTE *) pData) + slice * slicePitch;

		// For each row...
		for(UINT row = top; row < bottom; ++row)
		{
			BYTE *pSrcRow = pSrcSlice + row * rowPitch;

			if(iElem >= floatBufferElementCount)
			{
				assert( false && "Cannot write beyond bounds of buffer" );
				return E_BOUNDS;
			}

			// For each column...
			for(UINT col = left; col < right; ++col)
			{
				BYTE *pCurrentElem = pSrcRow + col * helperParams.bytesPerElement;

				ConvertPixelToFloat(
					&pFloatColorBuffer[ iElem ],
					pCurrentElem,
					helperParams.numComponentsIncludingFiller,
					helperParams.bytesPerElement,
					helperParams.firstComponent,
					helperParams.lastComponent,
					(D3D10_FORMAT_COMPONENT_NAME *) helperParams.components,
					(D3D10_FORMAT_COMPONENT_INTERPRETATION *) helperParams.componentTypes,
					helperParams.componentBits
					);

				iElem++;
			}
		}
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////
// ConvertFromFloat()
//
// Templatized method used by CD3DFormatConverter class to convert resource data from
// either FLOAT_COLOR or DOUBLE_COLOR format to DXGI pixel format.
////////////////////////////////////////////////////////////////
template <typename _FloatType>
static HRESULT ConvertFromFloat(
		RGBA_COLOR<_FloatType> *pFloatColorBuffer, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of dest region
		UINT top,			// Y-coordinate of top row of dest region
		UINT right,			// X-coordinate of one-past-the-right column of dest region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of dest region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in dest image (must be greater-than-or-equal-to image width or 0 if dest is 1D)
		UINT front,			// Z-coordinate of first slice of dest region (for 3-dimensional redest)
		UINT back,			// Z-coordinate of one-past-the-last slice of dest region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional dest (can be 0 if dest image is 1D or 2D)
		)
{
	UINT convertWidth = right - left;
	UINT convertHeight = bottom - top;
	UINT convertDepth = back - front;
	UINT32 uNumElements = convertHeight * convertWidth * convertDepth;
	if( uNumElements == 0 )
	{
		assert( false && "Region to be converted cannot have height, width, or depth of zero." );
		return E_INVALIDARG;
	}

	CImageDataHelperParams helperParams(format);


	//
	// Handle block-compressed formats.
	//

	if( helperParams.compressed )
	{
		return EncodeCompressed( format, pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch, slicePitch );
	}

	//
	// Fix row and slice pitch if not specified (note, EncodeCompressed() above handles its own case of correcting a zero pitch)
	//
	if(0 == rowPitch)
		rowPitch = convertWidth * helperParams.bytesPerElement;

	if(0 == slicePitch)
		slicePitch = rowPitch * convertHeight;

	assert(rowPitch >= convertWidth * helperParams.bytesPerElement);
	assert(slicePitch >= rowPitch * convertHeight);

	//
	// Handle specific packed and planar YUV formats.
	//

	switch (format)
	{
	case DXGI_FORMAT_YUY2:
		return ConvertRGBToYUY2( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_NV12:
		return ConvertRGBToNV12( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_NV11:
		return ConvertRGBToNV11( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_AYUV:
		return ConvertRGBToAYUV( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_Y410:
		return ConvertRGBToY410( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_Y416:
		return ConvertRGBToY416( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_Y216:
		return ConvertRGBToY216( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_Y210:
		return ConvertRGBToY210( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_P016:
		return ConvertRGBToP016( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );

	case DXGI_FORMAT_P010:
		return ConvertRGBToP010( pFloatColorBuffer, convertHeight, convertWidth, convertDepth, pData, rowPitch );
	}


	//
	// Note that other packed and planar formats are not currently supported.
	//

	if( helperParams.packed == true || helperParams.planar == true )
	{
		return E_NOTIMPL;
	}

	//
	// The 420_OPAQUE format can't be mapped since its exact bit layout is unspecified.
	//

	if( format == DXGI_FORMAT_420_OPAQUE )
	{
		return E_NOTIMPL;
	}

	size_t iElem = 0;

	// For each slice...
	for(UINT slice = front; slice < back; ++slice)
	{
		BYTE *pDestSlice = ((BYTE *) pData) + slice * slicePitch;

		// For each row...
		for(UINT row = top; row < bottom; ++row)
		{
			BYTE *pDestRow = pDestSlice + row * rowPitch;

			// For each column...
			for(UINT col = left; col < right; ++col)
			{
				ConvertFloatToPixel(
					pFloatColorBuffer + iElem,
					((BYTE*)pDestRow) + col * helperParams.bytesPerElement,
					helperParams.numComponentsIncludingFiller,
					helperParams.bytesPerElement,
					(D3D10_FORMAT_COMPONENT_NAME *) helperParams.components,
					(D3D10_FORMAT_COMPONENT_INTERPRETATION *) helperParams.componentTypes,
					helperParams.componentBits
					);

				iElem++;
			}
		}
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////
// CD3DFormatConverter::ConvertToFloat32()
//
// Converts resource data from DXGI pixel format to FLOAT_COLOR.
////////////////////////////////////////////////////////////////
HRESULT CD3DFormatConverter::ConvertToFloat32(
		FLOAT_COLOR *pFloatColorBuffer, 
		size_t floatBufferElementCount, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of source region
		UINT top,			// Y-coordinate of top row of source region
		UINT right,			// X-coordinate of one-past-the-right column of source region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of source region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in source image (must be greater-than-or-equal-to image width or 0 if source is 1D)
		UINT front,			// Z-coordinate of first slice of source region (for 3-dimensional resource)
		UINT back,			// Z-coordinate of one-past-the-last slice of source region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional source (can be 0 if source image is 1D or 2D)
		)
{
	switch(format)
	{
	// The following formats cannot be losslessly converted to 32-bit float so use 64-bit converter instead
	case DXGI_FORMAT_R32G32B32A32_SINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32B32A32_SINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32B32A32_UINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32G32B32_SINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32B32_SINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32G32B32_UINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32B32_UINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32G32_SINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32_SINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32G32_UINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32G32_UINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32_SINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32_SINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R32_UINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R32_UINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_D24_UNORM_S8_UINT cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		OutputDebugString( _T( "ConvertPixels() - Format DXGI_FORMAT_R24_UNORM_X8_TYPELESS cannot be losslessly converted to 32-bit float per component.  Consider using CD3DFormatConverter::ConvertToFloat64().\n" ) );
		break;
	}

	return ConvertToFloat<float>(
		pFloatColorBuffer,
		floatBufferElementCount,
		pData,
		format,
		left,
		top,
		right,
		bottom,
		rowPitch,
		front,
		back,
		slicePitch
		);
}

////////////////////////////////////////////////////////////////
// CD3DFormatConverter::ConvertToFloat64()
//
// Converts resource data from DXGI pixel format to DOUBLE_COLOR.
////////////////////////////////////////////////////////////////
HRESULT CD3DFormatConverter::ConvertToFloat64(
		DOUBLE_COLOR *pFloatColorBuffer, 
		size_t floatBufferElementCount, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of source region
		UINT top,			// Y-coordinate of top row of source region
		UINT right,			// X-coordinate of one-past-the-right column of source region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of source region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in source image (must be greater-than-or-equal-to image width or 0 if source is 1D)
		UINT front,			// Z-coordinate of first slice of source region (for 3-dimensional resource)
		UINT back,			// Z-coordinate of one-past-the-last slice of source region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional source (can be 0 if source image is 1D or 2D)
		)
{
	return ConvertToFloat<double>(
		pFloatColorBuffer,
		floatBufferElementCount,
		pData,
		format,
		left,
		top,
		right,
		bottom,
		rowPitch,
		front,
		back,
		slicePitch
		);
}

////////////////////////////////////////////////////////////////
// CD3DFormatConverter::ConvertFromFloat64()
//
// Converts resource data from DOUBLE_COLOR to DXGI pixel format.
////////////////////////////////////////////////////////////////
HRESULT CD3DFormatConverter::ConvertFromFloat64(
		DOUBLE_COLOR *pFloatColorBuffer, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of dest region
		UINT top,			// Y-coordinate of top row of dest region
		UINT right,			// X-coordinate of one-past-the-right column of dest region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of dest region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in dest image (must be greater-than-or-equal-to image width or 0 if dest is 1D)
		UINT front,			// Z-coordinate of first slice of dest region (for 3-dimensional redest)
		UINT back,			// Z-coordinate of one-past-the-last slice of dest region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional dest (can be 0 if dest image is 1D or 2D)
		)
{
	return ConvertFromFloat<double>(
		pFloatColorBuffer,
		pData,
		format,
		left,
		top,
		right,
		bottom,
		rowPitch,
		front,
		back,
		slicePitch
		);
}

////////////////////////////////////////////////////////////////
// CD3DFormatConverter::ConvertFromFloat32()
//
// Converts resource data from FLOAT_COLOR to DXGI pixel format.
////////////////////////////////////////////////////////////////
HRESULT CD3DFormatConverter::ConvertFromFloat32(
		FLOAT_COLOR *pFloatColorBuffer, 
		void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of dest region
		UINT top,			// Y-coordinate of top row of dest region
		UINT right,			// X-coordinate of one-past-the-right column of dest region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of dest region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in dest image (must be greater-than-or-equal-to image width or 0 if dest is 1D)
		UINT front,			// Z-coordinate of first slice of dest region (for 3-dimensional redest)
		UINT back,			// Z-coordinate of one-past-the-last slice of dest region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional dest (can be 0 if dest image is 1D or 2D)
		)
{
	return ConvertFromFloat<float>(
		pFloatColorBuffer,
		pData,
		format,
		left,
		top,
		right,
		bottom,
		rowPitch,
		front,
		back,
		slicePitch
		);
}

////////////////////////////////////////////////////////////////
// Convert2D
//
// General resource data conversion from one format to another
////////////////////////////////////////////////////////////////
HRESULT CD3DFormatConverter::Convert2D(
	void *pSrcData,
	void *pDestData,
	DXGI_FORMAT srcFormat,
	DXGI_FORMAT destFormat,
	UINT srcLeft,
	UINT srcTop,
	UINT srcRight,
	UINT srcBottom,
	UINT destLeft,
	UINT destTop,
	UINT srcRowPitch,
	UINT destRowPitch
	)
{
	HRESULT hr = S_OK;

	UINT srcPixelSize = CFormatHelper::ElementBytes(srcFormat);
	UINT destPixelSize = CFormatHelper::ElementBytes(destFormat);
	BYTE *pDestRow = reinterpret_cast<BYTE *>(pDestData);
	UINT width = (srcRight - srcLeft);
	UINT height = (srcBottom - srcTop);
	UINT srcWidthInBytes = srcPixelSize * width;
	UINT destWidthInBytes = destPixelSize * width;

	//
	// Convert the data to the target format
	//
	switch(srcFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		{
			// Use double-precision floating point color

			// Allocate buffer...
			// In the interest of not wasting a lot of memory the image conversion is done one row at a time.  This allows
			// us to allocate only width * sizeof(FLOAT_COLOR) bytes for our intermediate buffer.  This also helps to avoid
			// page faults improving the conversion performance
			std::vector<DOUBLE_COLOR> colorBuffer(width);

			// For each row...
			for(UINT row = 0; row < height; ++row)
			{
				// Convert resource pixels to FLOAT_COLOR
				hr = CD3DFormatConverter::ConvertToFloat64(
					colorBuffer.data(),
					width,
					pSrcData, 
					srcFormat,
					0,
					row,
					width,
					row + 1,
					srcRowPitch,
					0,
					1,
					0);

				if(FAILED(hr))
				{
					return hr;
				}

				// Convert FLOAT_COLOR to destination format
				hr = CD3DFormatConverter::ConvertFromFloat64(
					colorBuffer.data(),
					pDestData, 
					destFormat,
					0,
					row,
					width,
					row + 1,
					destRowPitch,
					0,
					1,
					0);
			}
		}
		break;

	default:
		{
			// Use single-precision floating-point color

			// Allocate buffer...
			// In the interest of not wasting a lot of memory the image conversion is done one row at a time.  This allows
			// us to allocate only width * sizeof(FLOAT_COLOR) bytes for our intermediate buffer.  This also helps to avoid
			// page faults, improving the conversion performance
			std::vector<FLOAT_COLOR> colorBuffer(width);

			// For each row...
			for(UINT row = 0; row < height; ++row)
			{
				// Convert resource pixels to FLOAT_COLOR
				hr = CD3DFormatConverter::ConvertToFloat32(
					colorBuffer.data(),
					width,
					pSrcData, 
					srcFormat,
					0,
					row,
					width,
					row + 1,
					srcRowPitch,
					0,
					1,
					0);

				if(FAILED(hr))
				{
					return hr;
				}

				// Convert FLOAT_COLOR to destination format
				hr = CD3DFormatConverter::ConvertFromFloat32(
					colorBuffer.data(),
					pDestData, 
					destFormat,
					0,
					row,
					width,
					row + 1,
					destRowPitch,
					0,
					1,
					0);
			}
		}
	}


	return hr;
}

/************************************************************\
* GetNV12PixelYUV
*
* Get pixel YUV from a NV12 texture data. 
*
* History:
* 10/8/2014 - Shaobin Tao - Created
\**************************************************************************/
HRESULT CD3DFormatConverter::GetNV12PixelYUV(
    BYTE *NV12Data,
    UINT Width,
    UINT Height,
    UINT rowPitch,
    UINT xPos,
    UINT yPos,
    FLOAT_COLOR* oYUV)
{
    if( NV12Data == NULL || 
        oYUV == NULL || 
        xPos < 0 || xPos > (Width - 1) ||
        yPos < 0 || yPos > (Height - 1) )
    {
        return E_INVALIDARG;
    }

    int uvstart = rowPitch * Height;
    int uvIndex = uvstart + (yPos / 2) * rowPitch + (xPos/2) * 2;
    float uu = NV12Data[uvIndex];
    float vv = NV12Data[uvIndex+1];

    int lumaIndex = yPos * rowPitch + xPos;
    float yy = NV12Data[lumaIndex];

    oYUV->r = yy / 255.0f;
    oYUV->g = uu / 255.0f;
    oYUV->b = vv / 255.0f;
    oYUV->a = 0.0f;

	return S_OK;
}
