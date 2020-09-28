#include <d3d11.h>
#include <d3dx11.h>
#include <vector>
#include <tchar.h>

#include "FormatHelper.h"
#include "FormatDesc.hpp"
#include "D3DFormatConverter.h"
#include "SafeMacros.h"
#include "Strsafe.h"
#include <wincodec.h>

#define IFC(x) if (FAILED(hr = (x))){goto Cleanup;}

HMODULE CFormatHelper::s_hD3D = NULL;
ID3D11Device* CFormatHelper::s_pRefDevice = NULL;
bool CFormatHelper::s_bUseWarpAsReference = false;

static HMODULE s_hD3DX = NULL;

bool LoadD3DX()
{
	if (NULL == s_hD3DX)
	{
		s_hD3DX = LoadLibraryExW(L"D3DX11_TEST.DLL", NULL, 0);
		if( NULL == s_hD3DX )
			return false;
	}

	return true;
}

HRESULT WrappedD3DX11SaveTextureToMemory(
		ID3D11DeviceContext       *pContext,
		ID3D11Resource*            pSrcTexture,
		D3DX11_IMAGE_FILE_FORMAT   DestFormat,
		ID3D10Blob**               ppDestBuf,
		UINT                       Flags)
{
	if (!LoadD3DX())
		return E_FAIL;

	typedef HRESULT (WINAPI* FP_D3DX11SaveTextureToMemory)(
		ID3D11DeviceContext       *pContext,
		ID3D11Resource*            pSrcTexture,
		D3DX11_IMAGE_FILE_FORMAT   DestFormat,
		ID3D10Blob**               ppDestBuf,
		UINT                       Flags);

	FP_D3DX11SaveTextureToMemory D3DX11SaveTextureToMemory = (FP_D3DX11SaveTextureToMemory) GetProcAddress( s_hD3DX, "D3DX11SaveTextureToMemory");
	if (NULL == D3DX11SaveTextureToMemory)
		return E_FAIL;

	return D3DX11SaveTextureToMemory(pContext, pSrcTexture, DestFormat, ppDestBuf, Flags);
}

HRESULT WrappedD3DX11GetImageInfoFromMemory(
		LPCVOID                   pSrcData,
		SIZE_T                    SrcDataSize,
		ID3DX11ThreadPump*        pPump,
		D3DX11_IMAGE_INFO*        pSrcInfo,
		HRESULT*                  pHResult)
{
	if (!LoadD3DX())
		return E_FAIL;

	typedef HRESULT (WINAPI* FP_D3DX11GetImageInfoFromMemory)(
		LPCVOID                   pSrcData,
		SIZE_T                    SrcDataSize,
		ID3DX11ThreadPump*        pPump,
		D3DX11_IMAGE_INFO*        pSrcInfo,
		HRESULT*                  pHResult);

	FP_D3DX11GetImageInfoFromMemory D3DX11GetImageInfoFromMemory = (FP_D3DX11GetImageInfoFromMemory) GetProcAddress( s_hD3DX, "D3DX11GetImageInfoFromMemory");
	if (NULL == D3DX11GetImageInfoFromMemory)
		return E_FAIL;

	return D3DX11GetImageInfoFromMemory(pSrcData, SrcDataSize, pPump, pSrcInfo, pHResult);
}

HRESULT WrappedD3DX11CreateTextureFromMemory(
		ID3D11Device*             pDevice,
		LPCVOID                   pSrcData,
		SIZE_T                    SrcDataSize,
		D3DX11_IMAGE_LOAD_INFO*   pLoadInfo,    
		ID3DX11ThreadPump*        pPump,    
		ID3D11Resource**          ppTexture,
		HRESULT*                  pHResult)
{
	if (!LoadD3DX())
		return E_FAIL;

	typedef HRESULT (WINAPI* FP_D3DX11CreateTextureFromMemory)(
		ID3D11Device*             pDevice,
		LPCVOID                   pSrcData,
		SIZE_T                    SrcDataSize,
		D3DX11_IMAGE_LOAD_INFO*   pLoadInfo,    
		ID3DX11ThreadPump*        pPump,    
		ID3D11Resource**          ppTexture,
		HRESULT*                  pHResult);

	FP_D3DX11CreateTextureFromMemory D3DX11CreateTextureFromMemory = (FP_D3DX11CreateTextureFromMemory) GetProcAddress( s_hD3DX, "D3DX11CreateTextureFromMemory");
	if (NULL == D3DX11CreateTextureFromMemory)
		return E_FAIL;

	return D3DX11CreateTextureFromMemory(pDevice, pSrcData, SrcDataSize, pLoadInfo, pPump, ppTexture, pHResult);
}



/*****************************Private*Routine******************************\
* Clamp
*
* Converts a floating point number to a byte value clamping to range 0-255.
*
* History:
* Tue 01/02/2001 - StEstrop - Created
*
\**************************************************************************/
__inline BYTE CFormatHelper::Clamp(float clr)
{
    clr += 0.5f;

    if (clr < 0.0f)
    {
        return (BYTE)0;
    }

    if (clr > 255.0f)
    {
        return (BYTE)255;
    }

    return (BYTE)clr;
}


/*****************************Private*Routine******************************\
* ConvertYCrCbToRGB
*
* This equation was taken from Video Demystified (2nd Edition)
* by Keith Jack, page 43.
*
*
* History:
* Tue 01/02/2001 - StEstrop - Created
*
\**************************************************************************/
RGBQUAD
CFormatHelper::ConvertYCrCbToRGB(
    int y,
    int cr,
    int cb
    )
{
    RGBQUAD rgbq;

    float r = (1.1644f * (y-16)) + (1.5960f * (cr-128));
    float g = (1.1644f * (y-16)) - (0.8150f * (cr-128)) - (0.3912f * (cb-128));
    float b = (1.1644f * (y-16))                        + (2.0140f * (cb-128));


    rgbq.rgbBlue  = Clamp(b);
    rgbq.rgbGreen = Clamp(g);
    rgbq.rgbRed   = Clamp(r);
    rgbq.rgbReserved = 0; // Alpha

    return rgbq;
}

void Reformat420ToRGB32(
	DXGI_FORMAT resFormat, 
	FLOAT_COLOR *pDataFloat, 
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
        return NULL;
    }
#endif

    FLOAT_COLOR * lpDibBits = pDataFloat;

    for (UINT y = 0; y < uHeight; y += 2) {

        LPBYTE lpLineY1 = lpBitsY;                      
        LPBYTE lpLineY2 = lpBitsY + Pitch;
        LPBYTE lpLineCr = lpBitsCr;
        LPBYTE lpLineCb = lpBitsCb;

        FLOAT_COLOR * lpDibLine1 = lpDibBits;
        FLOAT_COLOR * lpDibLine2 = lpDibBits + uWidth;

        for (UINT x = 0; x < uWidth; x += 2) {

            int  y0 = (int)lpLineY1[0];
            int  y1 = (int)lpLineY1[1];
            int  y2 = (int)lpLineY2[0];
            int  y3 = (int)lpLineY2[1];
            int  cb = (int)lpLineCb[0];
            int  cr = (int)lpLineCr[0];

            lpDibLine1->comp[0] = (float)  y0 / 255.f ;
            lpDibLine1->comp[1] = (float)  cb / 255.f ;
            lpDibLine1->comp[2] = (float)  cr / 255.f ;
            lpDibLine1->comp[3] = (float)  1.f ; // Alpha


            (lpDibLine1+1)->comp[0] = (float) y1 / 255.f ;
            (lpDibLine1+1)->comp[1] = (float) cb / 255.f ;
            (lpDibLine1+1)->comp[2] = (float) cr / 255.f ;
            (lpDibLine1+1)->comp[3] = (float) 1.f ; // Alpha


            lpDibLine2->comp[0] = (float) y2 / 255.f ;
            lpDibLine2->comp[1] = (float) cb / 255.f ;
            lpDibLine2->comp[2] = (float) cr / 255.f ;
            lpDibLine2->comp[3] = (float) 1.f ; // Alpha

            (lpDibLine2+1)->comp[0] = (float) y3 / 255.f ;
            (lpDibLine2+1)->comp[1] = (float) cb / 255.f ;
            (lpDibLine2+1)->comp[2] = (float) cr / 255.f ;
            (lpDibLine2+1)->comp[3] = (float) 1.f ; // Alpha

            lpLineY1 += 2;
            lpLineY2 += 2;
            lpLineCr += iCbCrInc;
            lpLineCb += iCbCrInc;

            lpDibLine1 += 2;
            lpDibLine2 += 2;
        }

        lpDibBits += (2 * uWidth);
        lpBitsY   += Pitch*2;
        lpBitsCr  += lPitchCbCr;
        lpBitsCb  += lPitchCbCr;
    }
}


void 
ReformatAYUVToRGB32(
								   FLOAT_COLOR *pDataFloat, 
								   UINT uHeight, 
								   UINT uWidth, 
								   UINT uDepth, 
								   void *pData,
                                   UINT Pitch)
{
	// Since uDepth is not yet used make sure nobody assumes it is
	assert( 1 == uDepth);

    LPBYTE lpBits = (LPBYTE)pData;
    LPBYTE lpLine;

    FLOAT_COLOR * lpDibLine = pDataFloat;
    FLOAT_COLOR * lpDib;

    for (UINT y = 0; y < uHeight; y++) {

        lpLine = lpBits;
        lpDib = lpDibLine;

        for (UINT x = 0; x < uWidth; x ++) {

            // read a 1x1 block of AYUV data
            const int  cr = lpLine[0];
            const int  cb = lpLine[1];
            const int  y  = lpLine[2];
            const BYTE a  = lpLine[3]; // Alpha

            // write a 1x1 block of RGB32 data
            lpDib->comp[0] = (float) y / 255.f;
            lpDib->comp[1] = (float) cb / 255.f;
            lpDib->comp[2] = (float) cr / 255.f;
            lpDib->comp[3] = (float) a / 255.f; // Alpha

            lpLine += 4;
            lpDib  += 1;
        }

        lpBits    += Pitch;
        lpDibLine += uWidth;
    }
}


void
ReformatYUY2ToRGB32(
								   FLOAT_COLOR *pDataFloat, 
								   UINT uHeight, 
								   UINT uWidth, 
								   UINT uDepth, 
								   void *pData,
                                   UINT Pitch)
{
    LPBYTE lpBits = (LPBYTE)pData;
    LPBYTE lpLine;

    for (UINT y = 0; y < uHeight; y++) {

        lpLine = lpBits;
        FLOAT_COLOR * lpDib = pDataFloat;

        for (UINT x = 0; x < uWidth; x += 2) {

            int  y0 = (int)lpLine[0];
            int  cb = (int)lpLine[1]; //U
            int  y1 = (int)lpLine[2];
            int  cr = (int)lpLine[3]; //V

            lpDib->comp[0] = (float) y0 / 255.f ;
            lpDib->comp[1] = (float) cb / 255.f ;
            lpDib->comp[2] = (float) cr / 255.f ;
            lpDib->comp[3] = (float) 1.f ; // Alpha


            (lpDib+1)->comp[0] = (float) y1 / 255.f ;
            (lpDib+1)->comp[1] = (float) cb / 255.f ;
            (lpDib+1)->comp[2] = (float) cr / 255.f ;
            (lpDib+1)->comp[3] = (float) 1.f ; // Alpha

            lpLine += 4;
            lpDib  += 2;
        }

        lpBits     += Pitch;
        pDataFloat += uWidth;
    }
}


/*****************************Private*Routine******************************\
* ConvertRGBToYCrCb
*
*
*
* History:
* 5/9/2003 - StEstrop - Created
*
\**************************************************************************/
typedef struct _AYUVsample2 {
    BYTE bCrValue;
    BYTE bCbValue;
    BYTE bY_Value;
    BYTE bSampleAlpha8;
}AYUVsample2;


__inline AYUVsample2
ConvertRGBToYCrCb(
    int r,
    int g,
    int b
    )
{
    AYUVsample2 ayuv;

    float y = 0.257F * r + 0.504F * g + 0.098F * b + 16.5F;
    float u = -.148F * r - 0.291F * g + 0.439F * b + 128.5F;
    float v = 0.439F * r - 0.368F * g - 0.071F * b + 128.5F;

    ayuv.bCrValue = (BYTE)(v);
    ayuv.bCbValue = (BYTE)(u);
    ayuv.bY_Value = (BYTE)(y);
    ayuv.bSampleAlpha8 = 0xFF;

    return ayuv;
}


bool CFormatHelper::ReformatPixels(	CONVERSION_TYPE convType, 
									DXGI_FORMAT resFormat, 
									FLOAT_COLOR *pDataFloat, 
									UINT uHeight, 
									UINT uWidth, 
									UINT uDepth,
									void *pData,
                                    UINT Pitch )
{
    if ( convType != CONV_FROM_FORMAT )
        DebugBreak();
    switch ( resFormat )
    {
    case DXGI_FORMAT_YUY2:
        ReformatYUY2ToRGB32( pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
        return true;
    case DXGI_FORMAT_NV12:
        Reformat420ToRGB32( resFormat, pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
        return true;
    case DXGI_FORMAT_AYUV:
        ReformatAYUVToRGB32( pDataFloat, uHeight, uWidth, uDepth, pData, Pitch);
        return true;
    default:
        DebugBreak();
        return false;
    }
}

bool CFormatHelper::ConvertPixels( CONVERSION_TYPE convType, 
								   DXGI_FORMAT resFormat, 
								   FLOAT_COLOR *pDataFloat, 
								   UINT uHeight, 
								   UINT uWidth, 
								   void *pData,
								   UINT rowPitch)
{
	return ConvertPixels(convType, resFormat, pDataFloat, uHeight, uWidth, 1, pData, rowPitch, 0);
}

bool CFormatHelper::ConvertPixels( CONVERSION_TYPE convType, 
								   DXGI_FORMAT resFormat, 
								   FLOAT_COLOR *pDataFloat, 
								   UINT uHeight, 
								   UINT uWidth, 
								   UINT uDepth, 
								   void *pData,
								   UINT rowPitch,
								   UINT slicePitch)
{
	if( pDataFloat == NULL )
	{
		OutputDebugString( _T( "ConvertPixels() - pDataFloat parameter is NULL!\n" ) );
		return false;
	}

	if( pData == NULL )
	{
		OutputDebugString( _T( "ConvertPixels() - pData parameter is NULL!\n" ) );
		return false;	
	}

	UINT32 uNumElements = uHeight * uWidth * uDepth;
	if( uNumElements == 0 )
	{
		OutputDebugString( _T( "ConvertPixels() - uNumElements cannot be zero.\n" ) );
		return false;
	}

	HRESULT hr = S_OK;

	switch(convType)
	{
	case CONV_FROM_FORMAT:
		// Convert resource pixels to FLOAT_COLOR
		hr = CD3DFormatConverter::ConvertToFloat32(
			pDataFloat,
			uNumElements,
			pData, 
			resFormat,
			0,
			0,
			uWidth,
			uHeight,
			rowPitch,
			0,
			uDepth,
			slicePitch);

		if(FAILED(hr))
		{
			OutputDebugString( _T( "ConvertPixels() - CD3DFormatConverter::ConvertToFloat32() failed.\n" ) );
			return false;
		}
		break;

	case CONV_TO_FORMAT:
		// Convert resource pixels to FLOAT_COLOR
		hr = CD3DFormatConverter::ConvertFromFloat32(
			pDataFloat,
			pData, 
			resFormat,
			0,
			0,
			uWidth,
			uHeight,
			rowPitch,
			0,
			uDepth,
			slicePitch);
		
		if (FAILED(hr))
		{
			OutputDebugString(_T("ConvertPixels() - CD3DFormatConverter::ConvertFromFloat32() failed.\n"));
			return false;
		}
		break;	
	}

    // shatao 10/15/2014 
    // unfortunately, real return value hr is ignored.  If I change it, many tests might start to fail.
    // Add an assert here to alert whoever visits here.
    assert(SUCCEEDED(hr));

	return true;
}

bool CFormatHelper::EncodeCompressed( DXGI_FORMAT resFormat, 
									  const FLOAT_COLOR *pDataFloat, 
									  UINT uHeight, 
									  UINT uWidth, 
									  UINT uDepth, 
									  void *pData,
									  UINT rowPitch,
									  UINT slicePitch
									  )
{
	UINT32 uNumElements = uHeight * uWidth * uDepth;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pDataFloat;
	initData.SysMemPitch = uWidth * sizeof( float ) * 4;
	initData.SysMemSlicePitch = uHeight * initData.SysMemPitch;

	HRESULT hr;
	bool bResult = true;
	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pIC = NULL;
	ID3D11Resource *pTexture = NULL;
	ID3D10Blob *pImageFile = NULL;
	ID3D11Resource *pCompressedTex = NULL;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	
	if( !CreateDevice( &pDevice ) )
	{
		OutputDebugString( _T( "EncodeCompressed() - failed to create temporary D3D11.1 device.\n" ) );
		bResult = false;
		goto Done;
	}

	if (uDepth == 1)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.Width = uWidth;
		desc.Height = uHeight;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D *pTexture2D = NULL;
		if( FAILED( hr = pDevice->CreateTexture2D( &desc, &initData, &pTexture2D ) ) )
		{
			OutputDebugString( _T( "EncodeCompressed() - failed to create 32-bit float texture2D.\n" ) );
			bResult = false;
			goto Done;
		}
		pTexture = pTexture2D;
	}
	else
	{
		D3D11_TEXTURE3D_DESC desc;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.Width = uWidth;
		desc.Height = uHeight;
		desc.Depth = uDepth;
		ID3D11Texture3D *pTexture3D = NULL;
		if( FAILED( hr = pDevice->CreateTexture3D( &desc, &initData, &pTexture3D ) ) )
		{
			OutputDebugString( _T( "EncodeCompressed() - failed to create compressed texture3D.\n" ) );
			bResult = false;
			goto Done;
		}
		pTexture = pTexture3D;
	}

	pDevice->GetImmediateContext(&pIC);

	if( FAILED( hr = WrappedD3DX11SaveTextureToMemory( pIC, pTexture, D3DX11_IFF_DDS, &pImageFile, 0 ) ) )
	{
		OutputDebugString( _T( "EncodeCompressed() - D3DX11SaveTextureToMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	D3DX11_IMAGE_INFO imgInfo;
	if( FAILED( hr = WrappedD3DX11GetImageInfoFromMemory( pImageFile->GetBufferPointer(), pImageFile->GetBufferSize(), NULL, &imgInfo, NULL ) ) )
	{
		OutputDebugString( _T( "EncodeCompressed() - D3DX11GetImageInfoFromMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	loadInfo.BindFlags = 0;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	loadInfo.Depth = uDepth;
	loadInfo.Filter = D3DX11_FILTER_NONE;
	loadInfo.FirstMipLevel = 0;
	loadInfo.Format = resFormat;
	loadInfo.Height = uHeight;
	loadInfo.MipFilter = D3DX11_FILTER_NONE;
	loadInfo.MipLevels = 1;
	loadInfo.MiscFlags = 0;
	loadInfo.pSrcInfo = &imgInfo;
	loadInfo.Usage = D3D11_USAGE_STAGING;
	loadInfo.Width = uWidth;

	
	if( FAILED( hr = WrappedD3DX11CreateTextureFromMemory( pDevice, pImageFile->GetBufferPointer(), pImageFile->GetBufferSize(), &loadInfo, NULL, &pCompressedTex, NULL ) ) )
	{
		OutputDebugString( _T( "EncodeCompressed() - D3DX11CreateTextureFromMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	UINT uBlockPixelWidth = 4;
	UINT uBlockPitch = 16;
	UINT uBlockHeight = 4;
	GetBlockSize(resFormat, &uBlockPixelWidth, &uBlockHeight, &uBlockPitch);

	if (uDepth == 1)
	{
		ID3D11Texture2D *pTexture2D = NULL;
		if( FAILED( hr = pCompressedTex->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&pTexture2D)) ))
		{
			OutputDebugString( _T( "EncodeCompressed() - Failed to QI compressed texture2D.\n" ) );
			bResult = false;
			goto Done;
		}
		D3D11_MAPPED_SUBRESOURCE texMap;
		if( FAILED( pIC->Map( pTexture2D, 0, D3D11_MAP_READ, 0, &texMap ) ) )
		{
			OutputDebugString( _T( "EncodeCompressed() - Failed to Map compressed texture.\n" ) );
			bResult = false;
			goto Done;
		}

		// In order not to break code using earlier implementation of
		// EncodeCompressed() assume the default pitches of the destination
		// texture match the pitches of the compressed source
		// NOTE: This is not a good assumption since hardware is not required
		// to match pitch between two similarly sized compressed resources.
		// Using asserts to help correct this during debug test execution.
		assert(rowPitch != 0 || GetSizeInBlocks(uHeight, uBlockHeight) == 1);
		if(0 == rowPitch)
			rowPitch = texMap.RowPitch;

		for( UINT i = 0; i < GetSizeInBlocks(uHeight, uBlockHeight); ++i )
			memcpy( (BYTE*)pData + i * rowPitch, ((BYTE*)texMap.pData) + i * texMap.RowPitch, uBlockPitch * GetSizeInBlocks(uWidth, uBlockPixelWidth) );

		pIC->Unmap( pTexture2D, 0 );
		pTexture2D->Release();
	}
	else
	{
		ID3D11Texture3D *pTexture3D = NULL;
		if( FAILED( hr = pCompressedTex->QueryInterface(__uuidof(ID3D11Texture3D), (void**)(&pTexture3D)) ))
		{
			OutputDebugString( _T( "EncodeCompressed() - Failed to QI compressed texture3D.\n" ) );
			bResult = false;
			goto Done;
		}
		D3D11_MAPPED_SUBRESOURCE texMap;
		if( FAILED( hr = pIC->Map( pTexture3D, 0, D3D11_MAP_READ, 0, &texMap ) ) )
		{
			OutputDebugString( _T( "EncodeCompressed() - Failed to Map compressed texture3D.\n" ) );
			bResult = false;
			pTexture3D->Release();
			goto Done;
		}


		// In order not to break code using earlier implementation of
		// EncodeCompressed() assume the default pitches of the destination
		// texture match the pitches of the compressed source.
		// NOTE: This is not a good assumption since hardware is not required
		// to match pitch between two similarly sized compressed resources.
		// Using asserts to help correct this during debug test execution.
		assert(rowPitch != 0);
		assert(slicePitch != 0);
		if(0 == rowPitch)
			rowPitch = texMap.RowPitch;

		if(0 == slicePitch && uDepth > 1)
			slicePitch = texMap.DepthPitch;

		for( UINT d = 0; d < uDepth; ++d )
		{
			for( UINT i = 0; i < GetSizeInBlocks(uHeight, uBlockHeight); ++i )
			{
				memcpy( ((BYTE*)pData) + i * rowPitch + d * slicePitch, 
					((BYTE*)texMap.pData) + i * texMap.RowPitch + d * texMap.DepthPitch, 
					uBlockPitch * GetSizeInBlocks(uWidth, uBlockPixelWidth) );
			}
		}

		pIC->Unmap( pTexture3D, 0 );
		pTexture3D->Release();
	}

Done:
	SAFE_RELEASE( pCompressedTex );
	SAFE_RELEASE( pImageFile );
	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pDevice );
	SAFE_RELEASE( pIC );

	return bResult;
}

bool CFormatHelper::DecodeCompressed( DXGI_FORMAT resFormat, 
									  const FLOAT_COLOR *pDataFloat, 
									  UINT uHeight, 
									  UINT uWidth, 
									  UINT uDepth, 
									  void *pData,
									  UINT rowPitch,
									  UINT slicePitch
									  )
{
	UINT uNumElements = uHeight * uWidth * uDepth;

	UINT blockPixelWidth = 4;
	UINT blockPixelHeight = 4;
	UINT blockPitch = 16;
	GetBlockSize(resFormat, &blockPixelWidth, &blockPixelHeight, &blockPitch);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pData;
	initData.SysMemPitch = GetSizeInBlocks(uWidth, blockPixelWidth) * blockPitch;
	initData.SysMemSlicePitch = GetSizeInBlocks(uHeight, blockPixelHeight) * initData.SysMemPitch;

	if(0 == rowPitch)
		rowPitch = uWidth * initData.SysMemPitch;

	if(0 == slicePitch && uDepth > 1)
		slicePitch = initData.SysMemSlicePitch;

	HRESULT hr;
	bool bResult = true;
	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pIC = NULL;
	ID3D10Blob *pImageFile = NULL;
	ID3D11Resource *pTexture = NULL;
	ID3D11Resource *pFloatTex = NULL;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	
	if( !CreateDevice( &pDevice ) )
	{
		OutputDebugString( _T( "DecodeCompressed() - failed to create temporary D3D11.1 device.\n" ) );
		bResult = false;
		goto Done;
	}

	if (uDepth == 1)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		desc.Format = resFormat;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.Width = blockPixelWidth * GetSizeInBlocks(uWidth, blockPixelWidth);
		desc.Height = blockPixelHeight * GetSizeInBlocks(uHeight, blockPixelHeight);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D *pTexture2D = NULL;
		if( FAILED( hr = pDevice->CreateTexture2D( &desc, &initData, &pTexture2D ) ) )
		{
			OutputDebugString( _T( "DecodeCompressed() - failed to create compressed texture2D.\n" ) );
			bResult = false;
			goto Done;
		}
		pTexture = pTexture2D;
	}
	else
	{
		D3D11_TEXTURE3D_DESC desc;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		desc.Format = resFormat;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.Width = blockPixelWidth * GetSizeInBlocks(uWidth, blockPixelWidth);
		desc.Height = blockPixelHeight * GetSizeInBlocks(uHeight, blockPixelHeight);
		desc.Depth = uDepth;
		ID3D11Texture3D *pTexture3D = NULL;
		if( FAILED( hr = pDevice->CreateTexture3D( &desc, &initData, &pTexture3D ) ) )
		{
			OutputDebugString( _T( "DecodeCompressed() - failed to create compressed texture3D.\n" ) );
			bResult = false;
			goto Done;
		}
		pTexture = pTexture3D;
	}

	pDevice->GetImmediateContext(&pIC);

	if( FAILED( hr = WrappedD3DX11SaveTextureToMemory( pIC, pTexture, D3DX11_IFF_DDS, &pImageFile, 0 ) ) )
	{
		OutputDebugString( _T( "DecodeCompressed() - D3DX11SaveTextureToMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	D3DX11_IMAGE_INFO imgInfo;
	if( FAILED( hr = WrappedD3DX11GetImageInfoFromMemory( pImageFile->GetBufferPointer(), pImageFile->GetBufferSize(), NULL, &imgInfo, NULL ) ) )
	{
		OutputDebugString( _T( "DecodeCompressed() - D3DX11GetImageInfoFromMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	loadInfo.BindFlags = 0;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	loadInfo.Depth = uDepth;
	loadInfo.Filter = D3DX11_FILTER_NONE;
	loadInfo.FirstMipLevel = 0;
	loadInfo.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	loadInfo.Height = uHeight;
	loadInfo.MipFilter = D3DX11_FILTER_NONE;
	loadInfo.MipLevels = 1;
	loadInfo.MiscFlags = 0;
	loadInfo.pSrcInfo = &imgInfo;
	loadInfo.Usage = D3D11_USAGE_STAGING;
	loadInfo.Width = uWidth;

	if( FAILED( hr = WrappedD3DX11CreateTextureFromMemory( pDevice, pImageFile->GetBufferPointer(), pImageFile->GetBufferSize(), &loadInfo, NULL, &pFloatTex, NULL ) ) )
	{
		OutputDebugString( _T( "DecodeCompressed() - D3DX11CreateTextureFromMemory failed.\n" ) );
		bResult = false;
		goto Done;
	}

	if (uDepth == 1)
	{
		D3D11_MAPPED_SUBRESOURCE texData;
		ID3D11Texture2D *pTexture2D = NULL;
		if( FAILED( hr = pFloatTex->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&pTexture2D)) ))
		{
			OutputDebugString( _T( "DecodeCompressed() - Failed to QI decompressed texture2D.\n" ) );
			bResult = false;
			goto Done;
		}
		if( FAILED( hr = pIC->Map( pTexture2D, 0, D3D11_MAP_READ, 0, &texData ) ) )
		{
			OutputDebugString( _T( "DecodeCompressed() - Failed to Map decompressed texture2D.\n" ) );
			bResult = false;
			pTexture2D->Release();
			goto Done;
		}

		UINT uElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( DXGI_FORMAT_R32G32B32A32_FLOAT ) / 8;
		for( UINT i = 0; i < uHeight; ++i )
			memcpy( ((BYTE*)pDataFloat) + i * uWidth * uElementSize, ((BYTE*)texData.pData) + i * texData.RowPitch, uWidth * uElementSize );

		pIC->Unmap( pTexture2D, 0 );
		pTexture2D->Release();
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE texData;
		ID3D11Texture3D *pTexture3D = NULL;
		if( FAILED( hr = pFloatTex->QueryInterface(__uuidof(ID3D11Texture3D), (void**)(&pTexture3D)) ))
		{
			OutputDebugString( _T( "DecodeCompressed() - Failed to QI decompressed texture3D.\n" ) );
			bResult = false;
			goto Done;
		}
		if( FAILED( hr = pIC->Map( pTexture3D, 0, D3D11_MAP_READ, 0, &texData ) ) )
		{
			OutputDebugString( _T( "DecodeCompressed() - Failed to Map decompressed texture3D.\n" ) );
			bResult = false;
			pTexture3D->Release();
			goto Done;
		}

		UINT uElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( DXGI_FORMAT_R32G32B32A32_FLOAT ) / 8;
		for( UINT d = 0; d < uDepth; ++d )
		{
			for( UINT i = 0; i < uHeight; ++i )
			{
				memcpy( ((BYTE*)pDataFloat) + uWidth * uElementSize * (i + uHeight * d), 
					((BYTE*)texData.pData) + i * texData.RowPitch + d * texData.DepthPitch, 
					uWidth * uElementSize );
			}
		}

		pIC->Unmap( pTexture3D, 0 );
		pTexture3D->Release();
	}

Done:
	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pImageFile );
	SAFE_RELEASE( pFloatTex );
	SAFE_RELEASE( pDevice );
	SAFE_RELEASE( pIC );

	return bResult;
}

bool CFormatHelper::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CFormatHelper::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

bool CFormatHelper::IsYUV( DXGI_FORMAT format )
{
	return CD3D10FormatHelper_Test::YUV( format ) ? true : false;
}

bool CFormatHelper::IsPlanar( DXGI_FORMAT format )
{
	return CD3D10FormatHelper_Test::Planar( format ) ? true : false;
}

//---------------------------------------------------------------

bool CFormatHelper::CreateStripes( DXGI_FORMAT resFormat, 
								   UINT uHeight, 
								   UINT uWidth,
								   UINT uNumStripes,
								   const FLOAT_COLOR *pColors, 
								   UINT uNumColors,
								   void *pStripes, 
								   bool bVertical,
								   UINT rowPitch)
{
	if( ( bVertical && uNumStripes > uWidth ) ||
		( !bVertical && uNumStripes > uHeight ) ||
		uNumStripes == 0 ||
		pStripes == NULL || pColors == NULL )
	{
		OutputDebugString( _T( "CreateStripes() failed, invalid or null parameters passed.\n" ) );
		return false;
	}

	UINT uStripeSize;
	if( bVertical )
		uStripeSize = uWidth / uNumStripes;
	else
		uStripeSize = uHeight / uNumStripes;

	FLOAT_COLOR *pStripesFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			UINT uStep = bVertical ? uX : uY;
			pStripesFloat[ uY * uWidth + uX ] = pColors[ (uStep / uStripeSize) % uNumColors ];
		}
	}

	if( !ConvertPixels( CONV_TO_FORMAT, resFormat, pStripesFloat, uHeight, uWidth, pStripes, rowPitch ) )
	{
		delete [ ] pStripesFloat;
		return false;
	}

	delete [ ] pStripesFloat;

	return true;
}

//---------------------------------------------------------------
bool CFormatHelper::CreateGradient(	DXGI_FORMAT resFormat,
									const UINT uHeight, 
									const UINT uWidth,
									const FLOAT_COLOR pColors[4], 
									void* pGradient,
									UINT rowPitch
									)
{
	if ( NULL == pColors || 
		 NULL == pGradient )
	{
		OutputDebugString( _T( "CreateGradient() failed, null parameters passed.\n" ) );
		return false;
	}

	FLOAT_COLOR *pGradientFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	float dist[ 4 ];
	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			float uNormX = (float) uX / (float) uWidth;
			float uNormY = (float) uY / (float) uHeight;
			float uInvX = 1.f - uNormX;
			float uInvY = 1.f - uNormY;

			dist[ 0 ] = sqrt( (uNormX * uNormX) + (uNormY * uNormY) );
			dist[ 1 ] = sqrt( (uInvX * uInvX) + (uNormY * uNormY) );
			dist[ 2 ] = sqrt( (uNormX * uNormX) + (uInvY * uInvY) );
			dist[ 3 ] = sqrt( (uInvX * uInvX) + (uInvY * uInvY) );

			pGradientFloat[ uY * uWidth + uX ] = ( pColors[ 0 ] * dist[ 0 ] ) +
				( pColors[ 1 ] * dist[ 1 ] ) +
				( pColors[ 2 ] * dist[ 2 ] ) +
				( pColors[ 3 ] * dist[ 3 ] );
		}
	}

	if( !ConvertPixels( CONV_TO_FORMAT, resFormat, pGradientFloat, uHeight, uWidth, pGradient, rowPitch ) )
	{
		delete [ ] pGradientFloat;
		return false;
	}

	delete [ ] pGradientFloat;

	return true;
}

//---------------------------------------------------------------
bool CFormatHelper::CreateTiles( DXGI_FORMAT resFormat,
								 UINT uHeight, 
								 UINT uWidth,
								 UINT uEdgeWidth,
								 const FLOAT_COLOR* pColors, 
								 UINT uNumColors,
								 void *pTiles,
								 UINT rowPitch )
{
	if( uEdgeWidth > uHeight || uEdgeWidth > uWidth ||
		pColors == NULL || pTiles == NULL )
	{
		OutputDebugString( _T( "CreateTiles() failed, invalid or null parameters passed.\n" ) );
		return false;
	}

	FLOAT_COLOR *pTilesFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	UINT uNumTilesX = uWidth / uEdgeWidth;
	UINT uNumTilesY = uHeight / uEdgeWidth;
	if( uWidth % uEdgeWidth )
		uNumTilesX++;
	if( uHeight % uEdgeWidth )
		uNumTilesY++;

	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			UINT uTileX = uX / uEdgeWidth;
			UINT uTileY = uY / uEdgeWidth;
			UINT uTileNum = uTileY * uNumTilesX + uTileX;
			pTilesFloat[ uY * uWidth + uX ] = pColors[ uTileNum % uNumColors ];
		}
	}

	if( !ConvertPixels( CONV_TO_FORMAT, resFormat, pTilesFloat, uHeight, uWidth, pTiles, rowPitch) )
	{
		delete [ ] pTilesFloat;
		return false;
	}

	delete [ ] pTilesFloat;

	return true;
}

//---------------------------------------------------------------

void CFormatHelper::SetReferenceDeviceType( bool bUseWarpAsReference )
{
	s_bUseWarpAsReference = bUseWarpAsReference;
}

bool CFormatHelper::CreateDevice( ID3D11Device **ppDevice )
{
	HRESULT hr = S_OK;
	*ppDevice = NULL;
	if (s_pRefDevice == NULL)
	{
		typedef HRESULT (D3D11CreateDeviceProc)( 
			IDXGIAdapter *pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HANDLE hSoftware,
			UINT Flags,
			const D3D_FEATURE_LEVEL *pFeatureLevels,
			UINT FeatureLevels,
			UINT32 SDKVersion,
			ID3D11Device **ppDevice,
			D3D_FEATURE_LEVEL *pFeatureLevel,
			ID3D11DeviceContext **ppImmediateContext
		);

		if( s_hD3D == NULL )
			s_hD3D = LoadLibraryEx(D3D11DLLNAME, NULL, 0);
		if( s_hD3D == NULL )
			return false;

		D3D11CreateDeviceProc *pD3D11CreateDevice 
			= (D3D11CreateDeviceProc *) GetProcAddress( s_hD3D, "D3D11CreateDevice" );
		if( pD3D11CreateDevice == NULL )
		{
			return false;
		}

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_REFERENCE;

		if ( s_bUseWarpAsReference )
		{
			driverType = D3D_DRIVER_TYPE_WARP;
		}

		hr = pD3D11CreateDevice( NULL, driverType, NULL,
			0, &featureLevel,
			1, D3D11_SDK_VERSION, &s_pRefDevice, NULL, NULL);
		
	}

	if (SUCCEEDED(hr))
	{
		s_pRefDevice->AddRef();
		*ppDevice = s_pRefDevice;
	}

	return hr == S_OK;
}

void CFormatHelper::Cleanup()
{
	SAFE_RELEASE(s_pRefDevice);

	if( s_hD3D != NULL )
		FreeLibrary( s_hD3D );
	s_hD3D = NULL;

	if( s_hD3DX != NULL )
		FreeLibrary( s_hD3DX );
	s_hD3DX = NULL;
}

UINT CFormatHelper::GetSizeInBlocks(UINT pixelSize, UINT blockSize)
{
	return (pixelSize + blockSize - 1) / blockSize;
}

void CFormatHelper::GetBlockSize(	DXGI_FORMAT resFormat,
									UINT* pBlockTexelWidth,
									UINT* pBlockTexelHeight,
									UINT* pBlockByteSize )
{
	const UINT bitsPerElement = CD3D10FormatHelper_Test::GetBitsPerElement( resFormat );
	UINT blockWidth = 1;
	UINT blockHeight = 1;
	UINT blockBytes = bitsPerElement / 8;

	switch (resFormat)
	{
	default:
		assert(blockBytes > 0);
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:   
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		blockWidth = 2;
		blockHeight = 1;
		blockBytes = 4;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:      
	case DXGI_FORMAT_BC2_UNORM:      
	case DXGI_FORMAT_BC2_UNORM_SRGB: 
	case DXGI_FORMAT_BC3_TYPELESS:      
	case DXGI_FORMAT_BC3_UNORM:      
	case DXGI_FORMAT_BC3_UNORM_SRGB: 
	case DXGI_FORMAT_BC5_TYPELESS:      
	case DXGI_FORMAT_BC5_UNORM:      
	case DXGI_FORMAT_BC5_SNORM:     
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		blockWidth = 4;
		blockHeight = 4;
		blockBytes = 16;
		break;
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:     
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:     
	case DXGI_FORMAT_BC4_SNORM: 
		blockWidth = 4;
		blockHeight = 4;
		blockBytes = 8;
		break;
	}

	if (pBlockTexelWidth)
		*pBlockTexelWidth = blockWidth;
	if (pBlockTexelHeight)
		*pBlockTexelHeight = blockHeight;
	if (pBlockByteSize)
		*pBlockByteSize = blockBytes;
}

bool CFormatHelper::ResourceViewContainsBothLuminanceDataAndChrominanceData( const DXGI_FORMAT &resourceFormat )
{
	// Note: No need to determine if it is a video format, since it just returns false if it's not in the switch's list
	// These formats put both Luminance Data and Chrominance Data in the same view
	switch( resourceFormat )
	{
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_YUY2:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		return true;
	}

	return false;
}

//
// Returns a std::vector of valid shader resource view formats for a specified video format
// Some view formats separate luminance data (Y) from chrominance data (UV), others combine the two (YUV).
// When the two are combined into a single format, then the array lists are identical because
// both luminance data and chrominance data can be retrieved from the same view.
// The function returns true if an SRV can be created from the specified format.
//

bool CFormatHelper::GetSRViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData )
{
	// Local variables
	bool bSuccess = false;

	// Clear the lists
	luminanceData.clear();		// Y
	chrominanceData.clear();	// U, V

	// Ensure the format is a video format
	if( !CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// Not a video format
		goto done;
	}

	// Build the vector of valid view formats
	switch( resourceFormat )
	{
	default:
		// Format helper bug, all video formats should be accounted for
		goto done;
		break;

	case DXGI_FORMAT_AYUV:
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
		break;

	case DXGI_FORMAT_Y410:
		luminanceData.push_back( DXGI_FORMAT_R10G10B10A2_UINT );
		luminanceData.push_back( DXGI_FORMAT_R10G10B10A2_UNORM );
		break;

	case DXGI_FORMAT_NV11:
		// Fall-through
    case DXGI_FORMAT_NV12:
		luminanceData.push_back( DXGI_FORMAT_R8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UNORM );
		break;

    case DXGI_FORMAT_P010:
		// Fall-through
    case DXGI_FORMAT_P016:
		luminanceData.push_back( DXGI_FORMAT_R16_UINT );
		luminanceData.push_back( DXGI_FORMAT_R16_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UNORM );
		break;

    case DXGI_FORMAT_YUY2:
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
		luminanceData.push_back( DXGI_FORMAT_R8G8_B8G8_UNORM );
		break;

	case DXGI_FORMAT_Y416:
		// Fall-through
    case DXGI_FORMAT_Y210:
		// Fall-through
    case DXGI_FORMAT_Y216:
		luminanceData.push_back( DXGI_FORMAT_R16G16B16A16_UINT );
		luminanceData.push_back( DXGI_FORMAT_R16G16B16A16_UNORM );
		break;

	case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
		// Cannot create SRV of these
		goto done;
		break;
	}

	// Check if the array lists need to be duplicated
	if( ResourceViewContainsBothLuminanceDataAndChrominanceData( resourceFormat ) )
	{
		chrominanceData = luminanceData;
		assert( chrominanceData.size() == luminanceData.size() );
	}

	// The function completed successfully, and an SRV can be created
	bSuccess = true;

done:
	return bSuccess;
}

//
// Returns a std::vector of valid render target view formats for a specified video format
// Some view formats separate luminance data (Y) from chrominance data (UV), others combine the two (YUV).
// When the two are combined into a single format, then the array lists are identical because
// both luminance data and chrominance data can be retrieved from the same view.
// The function returns true if an RTV can be created from the specified format.
//

bool CFormatHelper::GetRTViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData )
{
	// Local variables
	bool bSuccess = false;

	// Clear the lists
	luminanceData.clear();		// Y
	chrominanceData.clear();	// U, V

	// Ensure the format is a video format
	if( !CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// Not a video format
		goto done;
	}

	// Build the vector of valid view formats
	switch( resourceFormat )
	{
	default:
		// Format helper bug, all video formats should be accounted for
		goto done;
		break;

	case DXGI_FORMAT_AYUV:
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
		break;

	case DXGI_FORMAT_NV11:
		// Fall-through
    case DXGI_FORMAT_NV12:
		luminanceData.push_back( DXGI_FORMAT_R8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UNORM );
		break;

    case DXGI_FORMAT_P010:
		// Fall-through
    case DXGI_FORMAT_P016:
		luminanceData.push_back( DXGI_FORMAT_R16_UINT );
		luminanceData.push_back( DXGI_FORMAT_R16_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UNORM );
		break;

	case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_YUY2:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
		// Cannot create RTV of these
		goto done;
		break;
	}

	// Check if the array lists need to be duplicated
	if( ResourceViewContainsBothLuminanceDataAndChrominanceData( resourceFormat ) )
	{
		chrominanceData = luminanceData;
		assert( chrominanceData.size() == luminanceData.size() );
	}

	// The function completed successfully, and an SRV can be created
	bSuccess = true;

done:
	return bSuccess;
}

//
// Returns a std::vector of valid unordered access view formats for a specified video format
// Some view formats separate luminance data (Y) from chrominance data (UV), others combine the two (YUV).
// When the two are combined into a single format, then the array lists are identical because
// both luminance data and chrominance data can be retrieved from the same view.
// The function returns true if an UAV can be created from the specified format.
//

bool CFormatHelper::GetUAViewFormatsForVideoResourceFormat( const DXGI_FORMAT &resourceFormat, std::vector<DXGI_FORMAT> &luminanceData, std::vector<DXGI_FORMAT> &chrominanceData )
{
	// Local variables
	bool bSuccess = false;

	// Clear the lists
	luminanceData.clear();		// Y
	chrominanceData.clear();	// U, V

	// Ensure the format is a video format
	if( !CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// Not a video format
		goto done;
	}

	// Build the vector of valid view formats
	switch( resourceFormat )
	{
	default:
		// Format helper bug, all video formats should be accounted for
		goto done;
		break;

	case DXGI_FORMAT_AYUV:
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
		luminanceData.push_back( DXGI_FORMAT_R32_UINT );
		break;

	case DXGI_FORMAT_Y410:
		luminanceData.push_back( DXGI_FORMAT_R10G10B10A2_UINT );
		luminanceData.push_back( DXGI_FORMAT_R10G10B10A2_UNORM );
		luminanceData.push_back( DXGI_FORMAT_R32_UINT );
		break;

	case DXGI_FORMAT_NV11:
		// Fall-through
    case DXGI_FORMAT_NV12:
		luminanceData.push_back( DXGI_FORMAT_R8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R8G8_UNORM );
		break;

    case DXGI_FORMAT_P010:
		// Fall-through
    case DXGI_FORMAT_P016:
		luminanceData.push_back( DXGI_FORMAT_R16_UINT );
		luminanceData.push_back( DXGI_FORMAT_R16_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UINT );
		chrominanceData.push_back( DXGI_FORMAT_R16G16_UNORM );
		chrominanceData.push_back( DXGI_FORMAT_R32_UINT );
		break;

    case DXGI_FORMAT_YUY2:
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UINT );
		luminanceData.push_back( DXGI_FORMAT_R8G8B8A8_UNORM );
		luminanceData.push_back( DXGI_FORMAT_R8G8_B8G8_UNORM );
		luminanceData.push_back( DXGI_FORMAT_R32_UINT );
		break;

	case DXGI_FORMAT_Y416:
		// Fall-through
    case DXGI_FORMAT_Y210:
		// Fall-through
    case DXGI_FORMAT_Y216:
		luminanceData.push_back( DXGI_FORMAT_R16G16B16A16_UINT );
		luminanceData.push_back( DXGI_FORMAT_R16G16B16A16_UNORM );
		break;

	case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
		// Cannot create UAV of these
		goto done;
		break;
	}

	// Check if the array lists need to be duplicated
	if( ResourceViewContainsBothLuminanceDataAndChrominanceData( resourceFormat ) )
	{
		chrominanceData = luminanceData;
		assert( chrominanceData.size() == luminanceData.size() );
	}

	// The function completed successfully, and an SRV can be created
	bSuccess = true;

done:
	return bSuccess;
}

//
// Returns the index to the first format that supports the requested D3D11_FORMAT_SUPPORT(s), if found.
//
HRESULT CFormatHelper::GetFirstFormatWithSupport ( ID3D11Device *pDevice, const std::vector<DXGI_FORMAT> &resourceFormats, UINT FormatSupport, UINT &Index )
{
    HRESULT hr = E_FAIL;

    for ( auto it = resourceFormats.begin(); it != resourceFormats.end(); ++it )
    {
        UINT fmtFlags = 0;
        hr = pDevice->CheckFormatSupport( *it, &fmtFlags );

        if (SUCCEEDED(hr) && ((fmtFlags & FormatSupport) == FormatSupport))
        {
            Index = static_cast<UINT>(it - resourceFormats.begin());
            break;
        }
    }

    return hr;
}

//
// Returns the maximum error that is allowed by the spec for decoding of a particular block compressed format
// returns 0.0 for all other formats
//
float CFormatHelper::GetBCFormatMaxTolerance(DXGI_FORMAT format)
{
	double absolute_error = 0;
	double max_relative_error = 0;

	switch(format)
	{
	default:
		break;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		absolute_error = 1.0/255.0;
		max_relative_error = 1.0;
		break;
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC5_UNORM:
		absolute_error = 1.0/65535.0;
		max_relative_error = 1.0;
		break;
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_SNORM:
		absolute_error = 1.0/32767.0;
		max_relative_error = 2.0;
		break;
	}

	return (float)(absolute_error + 0.03 * max_relative_error);
}

HRESULT CFormatHelper::SaveColorArray(
    __in_ecount(width * height) FLOAT_COLOR* colors,
    __in UINT width,
    __in UINT height,
    __in LPWSTR target_file_name_prefix)
{
    return SaveColorArrayAsPNG(colors, width, height, target_file_name_prefix);
}

HRESULT CFormatHelper::SaveColorArrayAsPNG(
    __in_ecount(width * height) FLOAT_COLOR* colors,
    __in UINT width,
    __in UINT height,
    __in LPWSTR target_file_name_prefix)
{
    HRESULT                         hr = S_OK;
    const float                     DEFAULT_DPI = 96.0f;
    const int                       CONVERTED_COLORS_BYTE_COUNT = width * height * sizeof(DWORD);
    WCHAR                           tszFilename[MAX_PATH];
    LPBYTE                          pbConvertedColors = NULL;

    IWICImagingFactory*             pWICFactory = NULL;
    IWICBitmapEncoder*              pIBitmapEncoder = NULL;
    IWICStream*                     pIStream = NULL;
    IWICBitmapFrameEncode*          pIEncoderFrame = NULL;
    IWICBitmap*                     pIWICBitmap = NULL;      
    IWICBitmapLock*                 pLock = NULL;
    UINT                            cbBufferSize = 0;
    BYTE*                           pbBuffer = NULL;
    WICRect                         rcLock = { 0, 0, width, height };

    //
    // Now, convert the FLOAT_COLOR array to a DXGI_FORMAT_R8G8B8A8_UNORM array:
    //
    pbConvertedColors = new BYTE[CONVERTED_COLORS_BYTE_COUNT];
    if(NULL == pbConvertedColors)
    {
        OutputDebugString(_T("CFormatHelper::SaveColorArrayAsPNG(): failed to allocate for a temporary buffer"));

        goto Cleanup;
    }
    
    if(!CFormatHelper::ConvertPixels(
                                     CONV_TO_FORMAT,
                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                     colors /* pDataFloat */,
                                     height /* uHeight */,
                                     width /* uWidth */,
                                     pbConvertedColors /* pData */
                                    )
      )
    {
        OutputDebugString(_T("CFormatHelper::SaveColorArrayAsPNG(): Convert Pixels failed."));
        goto Cleanup;
    }
    
    IFC(CoInitializeEx(NULL, COINIT_MULTITHREADED));

    IFC(CoCreateInstance(
                         CLSID_WICImagingFactory,
                         NULL,
                         CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&pWICFactory)
                        )
       );

    IFC(pWICFactory->CreateStream(&pIStream));

    _snwprintf_s(tszFilename, sizeof(tszFilename), _TRUNCATE, L"%s.png", target_file_name_prefix);

    IFC(pIStream->InitializeFromFilename(tszFilename, GENERIC_WRITE));

    IFC(pWICFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pIBitmapEncoder));

    IFC(pIBitmapEncoder->Initialize(pIStream, WICBitmapEncoderNoCache));

    IFC(pIBitmapEncoder->CreateNewFrame(&pIEncoderFrame, NULL));

    IFC(pIEncoderFrame->Initialize(NULL));
    
    IFC(pWICFactory->CreateBitmap(width,
                                  height,
                                  GUID_WICPixelFormat32bppRGBA,
                                  WICBitmapCacheOnDemand,
                                  &pIWICBitmap
                                 )
       );

    IFC(pIWICBitmap->Lock(&rcLock, WICBitmapLockWrite, &pLock));
    IFC(pLock->GetDataPointer(&cbBufferSize, &pbBuffer));

    memcpy(pbBuffer, pbConvertedColors, cbBufferSize);    
    SAFE_RELEASE(pLock);
    
    IFC(pIWICBitmap->SetResolution(DEFAULT_DPI, DEFAULT_DPI));
    IFC(pIEncoderFrame->WriteSource(pIWICBitmap, &rcLock));
    
    IFC(pIEncoderFrame->Commit());
    IFC(pIBitmapEncoder->Commit());

    OutputDebugString(_T("CFormatHelper::SaveColorArrayAsPNG(): Png saved."));

Cleanup:    
    SAFE_RELEASE(pLock);
    SAFE_RELEASE(pIBitmapEncoder);
    SAFE_RELEASE(pIStream);
    SAFE_RELEASE(pIEncoderFrame);
    SAFE_RELEASE(pIWICBitmap);
    SAFE_RELEASE(pWICFactory);

    CoUninitialize();

    return hr;
}

HRESULT CFormatHelper::SaveColorArray(
    void * pData,
    DXGI_FORMAT resFormat,
    __in UINT width,
    __in UINT height,
    __in LPWSTR target_file_name_prefix)
{
    // First need to convert to Float array
    FLOAT_COLOR * pFloatColors = NULL;
    size_t cColors = width * height;
    HRESULT hr = S_OK;
    pFloatColors = new FLOAT_COLOR[cColors];
    if (!pFloatColors)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    if (!ConvertPixels( 
        CONV_FROM_FORMAT, //CONVERSION_TYPE convType, 
        resFormat, //DXGI_FORMAT resFormat, 
        pFloatColors, //FLOAT_COLOR *pDataFloat, 
        height, //UINT uHeight, 
        width, //UINT uWidth, 
        pData)) //void *pData );
    {
        hr = E_FAIL;
        goto Cleanup;
    }

    // Then save
    hr = SaveColorArray(
        pFloatColors, 
        width, 
        height, 
        target_file_name_prefix);

Cleanup:
    if (pFloatColors)
    {
        delete[] pFloatColors;
    }
    return hr;
}

#define NO CFormatHelper::COMPONENT_ENCODING_NONE  
#define FP CFormatHelper::COMPONENT_ENCODING_FP    
#define SI CFormatHelper::COMPONENT_ENCODING_SINT  
#define UI CFormatHelper::COMPONENT_ENCODING_UINT  
#define XR CFormatHelper::COMPONENT_ENCODING_XRBIAS
#define X  CFormatHelper::COMPONENT_ENCODING_X     
#define BC CFormatHelper::COMPONENT_ENCODING_BC    
#define P8 CFormatHelper::COMPONENT_ENCODING_P8    
#define S5 CFormatHelper::COMPONENT_ENCODING_SHARED5BITEXP    
#define EX CFormatHelper::COMPONENT_ENCODING_EXPONENT



struct
{
    DXGI_FORMAT                         SanityCheck;
    struct
    {
        CFormatHelper::COMPONENT_ENCODING   Encoding;
        UINT                                Bits;
    } Data[4];
} Supplemental[] = 
{
    //      DXGI_FORMAT                           BitsPerComponent[4]
    {DXGI_FORMAT_UNKNOWN                     ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R32G32B32A32_TYPELESS       ,FP,32,FP,32,FP,32,FP,32   },
    {    DXGI_FORMAT_R32G32B32A32_FLOAT      ,FP,32,FP,32,FP,32,FP,32   },
    {    DXGI_FORMAT_R32G32B32A32_UINT       ,UI,32,UI,32,UI,32,UI,32   },
    {    DXGI_FORMAT_R32G32B32A32_SINT       ,SI,32,SI,32,SI,32,SI,32   },
    {DXGI_FORMAT_R32G32B32_TYPELESS          ,NO,32,NO,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32B32_FLOAT         ,NO,32,NO,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32B32_UINT          ,NO,32,NO,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32B32_SINT          ,NO,32,NO,32,NO,0 ,NO,0    },
    {DXGI_FORMAT_R16G16B16A16_TYPELESS       ,NO,16,NO,16,NO,16,NO,16   },
    {   DXGI_FORMAT_R16G16B16A16_FLOAT       ,FP,16,FP,16,FP,16,FP,16   },
    {    DXGI_FORMAT_R16G16B16A16_UNORM      ,UI,16,UI,16,UI,16,UI,16   },
    {    DXGI_FORMAT_R16G16B16A16_UINT       ,UI,16,UI,16,UI,16,UI,16   },
    {    DXGI_FORMAT_R16G16B16A16_SNORM      ,SI,16,SI,16,SI,16,SI,16   },
    {    DXGI_FORMAT_R16G16B16A16_SINT       ,SI,16,SI,16,SI,16,SI,16   },
    {DXGI_FORMAT_R32G32_TYPELESS             ,NO,32,NO,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32_FLOAT            ,FP,32,FP,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32_UINT             ,UI,32,UI,32,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32G32_SINT             ,SI,32,SI,32,NO,0 ,NO,0    },
    {DXGI_FORMAT_R32G8X24_TYPELESS           ,NO,32,NO,8 ,NO,24,NO,0    },
    {    DXGI_FORMAT_D32_FLOAT_S8X24_UINT    ,FP,32,UI,8 ,X ,24,NO,0    },
    {    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,UI,32,X ,8 ,X ,24,NO,0    },
    {    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT ,X ,32,UI,8 ,X ,24,NO,0    },
    {DXGI_FORMAT_R10G10B10A2_TYPELESS        ,NO,10,NO,10,NO,10,NO,2    },
    {    DXGI_FORMAT_R10G10B10A2_UNORM       ,UI,10,UI,10,UI,10,UI,2    },
    {    DXGI_FORMAT_R10G10B10A2_UINT        ,UI,10,UI,10,UI,10,UI,2    },
    {DXGI_FORMAT_R11G11B10_FLOAT             ,FP,11,FP,11,FP,10,NO,0    },
    {DXGI_FORMAT_R8G8B8A8_TYPELESS           ,NO,8 ,NO,8 ,NO,8 ,NO,8    },
    {    DXGI_FORMAT_R8G8B8A8_UNORM          ,UI,8 ,UI,8 ,UI,8 ,UI,8    },
    {    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     ,UI,8 ,UI,8 ,UI,8 ,UI,8    },
    {    DXGI_FORMAT_R8G8B8A8_UINT           ,UI,8 ,UI,8 ,UI,8 ,UI,8    },
    {    DXGI_FORMAT_R8G8B8A8_SNORM          ,SI,8 ,SI,8 ,SI,8 ,SI,8    },
    {    DXGI_FORMAT_R8G8B8A8_SINT           ,SI,8 ,SI,8 ,SI,8 ,SI,8    },
    {DXGI_FORMAT_R16G16_TYPELESS             ,NO,16,NO,16,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16G16_FLOAT            ,FP,16,FP,16,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16G16_UNORM            ,UI,16,UI,16,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16G16_UINT             ,UI,16,UI,16,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16G16_SNORM            ,SI,16,SI,16,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16G16_SINT             ,SI,16,SI,16,NO,0 ,NO,0    },
    {DXGI_FORMAT_R32_TYPELESS                ,NO,32,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_D32_FLOAT               ,FP,32,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32_FLOAT               ,FP,32,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32_UINT                ,UI,32,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R32_SINT                ,SI,32,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R24G8_TYPELESS              ,NO,24,NO,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_D24_UNORM_S8_UINT       ,UI,24,UI,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R24_UNORM_X8_TYPELESS   ,UI,24,X ,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_X24_TYPELESS_G8_UINT    ,X ,24,UI,8 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R8G8_TYPELESS               ,NO,8 ,NO,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8G8_UNORM              ,UI,8 ,UI,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8G8_UINT               ,UI,8 ,UI,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8G8_SNORM              ,SI,8 ,SI,8 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8G8_SINT               ,SI,8 ,SI,8 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R16_TYPELESS                ,NO,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16_FLOAT               ,FP,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_D16_UNORM               ,UI,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16_UNORM               ,UI,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16_UINT                ,UI,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16_SNORM               ,SI,16,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R16_SINT                ,SI,16,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R8_TYPELESS                 ,NO,8 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8_UNORM                ,UI,8 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8_UINT                 ,UI,8 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8_SNORM                ,SI,8 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_R8_SINT                 ,SI,8 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_A8_UNORM                    ,UI,8 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R1_UNORM                    ,UI,1 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R9G9B9E5_SHAREDEXP          ,S5,9 ,S5,9 ,S5,9 ,EX,5    },
    {DXGI_FORMAT_R8G8_B8G8_UNORM             ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_G8R8_G8B8_UNORM             ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC1_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC1_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC1_UNORM_SRGB          ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC2_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC2_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC2_UNORM_SRGB          ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC3_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC3_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC3_UNORM_SRGB          ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC4_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC4_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC4_SNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC5_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC5_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC5_SNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B5G6R5_UNORM                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B5G5R5A1_UNORM              ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B8G8R8A8_UNORM              ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B8G8R8X8_UNORM              ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B8G8R8A8_TYPELESS           ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB     ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_B8G8R8X8_TYPELESS           ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB     ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC6H_TYPELESS               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC6H_UF16               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC6H_SF16               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {DXGI_FORMAT_BC7_TYPELESS                ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC7_UNORM               ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    {    DXGI_FORMAT_BC7_UNORM_SRGB          ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_AYUV                       ,UI,8 ,UI,8 ,UI,8 ,UI,8    },
    { DXGI_FORMAT_Y410                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_Y416                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_NV12                       ,UI,8 ,UI,8 ,UI,8 ,NO,0    },
    { DXGI_FORMAT_P010                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_P016                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_420_OPAQUE                 ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_YUY2                       ,UI,8 ,UI,8 ,UI,8 ,NO,0    },
    { DXGI_FORMAT_Y210                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_Y216                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_NV11                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_AI44                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_IA44                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_P8                         ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_A8P8                       ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
    { DXGI_FORMAT_B4G4R4A4_UNORM             ,NO,0 ,NO,0 ,NO,0 ,NO,0    },
};

bool CFormatHelper::GetMipDimensions(UINT mipSlice, _Inout_ UINT *pWidth, _Inout_opt_ UINT *pHeight, _Inout_opt_ UINT *pDepth)
{
	UINT denominator = (1 << mipSlice); // 2 ^ subresource
	UINT mipWidth = *pWidth / denominator;
	UINT mipHeight = pHeight ? *pHeight / denominator : 1;
	UINT mipDepth = pDepth ? *pDepth / denominator : 1;

	if(mipWidth == 0 && mipHeight == 0 && mipDepth == 0)
		return false; // No such mip slice

	// Adjust dimensions for degenerate mips
	if(mipHeight == 0 )
		mipHeight = 1;
	if(mipWidth == 0 )
		mipWidth = 1;
	if(mipDepth == 0)
		mipDepth = 1;

	*pWidth = mipWidth;
	if(pHeight) *pHeight = mipHeight;
	if(pDepth) *pDepth = mipDepth;

	return true;
}

UINT CFormatHelper::ElementBits(DXGI_FORMAT f)
{
	return CD3D10FormatHelper_Test::GetBitsPerElement(f);
}

UINT CFormatHelper::ElementBytes(DXGI_FORMAT f)
{
	// Add seven to guarantee the number of bytes is large enough to contain all bits
	return (CD3D10FormatHelper_Test::GetBitsPerElement(f) + 7) / 8;
}

CFormatHelper::COMPONENT_ENCODING   CFormatHelper::ComponentEncoding( DXGI_FORMAT f, UINT Channel )
{
    if ( f >= ARRAYSIZE( Supplemental ) )
    {
        DebugBreak();
        return CFormatHelper::COMPONENT_ENCODING_NONE;
    }
    if ( Supplemental[f].SanityCheck != f )
        DebugBreak();
    return Supplemental[f].Data[Channel].Encoding;
}

UINT CFormatHelper::ComponentBits( DXGI_FORMAT f, UINT Channel )
{
    if ( f >= ARRAYSIZE( Supplemental ) )
    {
        DebugBreak();
        return 0;
    }
    if ( Supplemental[f].SanityCheck != f )
        DebugBreak();
    return Supplemental[f].Data[Channel].Bits;
}

bool CFormatHelper::IsComponentPresent( DXGI_FORMAT f, UINT Channel )
{
    if ( f >= ARRAYSIZE( Supplemental ) )
    {
        DebugBreak();
        return false;
    }
    if ( Supplemental[f].SanityCheck != f )
        DebugBreak();
    return Supplemental[f].Data[Channel].Encoding != NO;
}

float CFormatHelper::MachineEpsilon( COMPONENT_ENCODING e, UINT Bits )
{
    switch ( e )
    {
    case COMPONENT_ENCODING_NONE         :
    case COMPONENT_ENCODING_X            :
    case COMPONENT_ENCODING_EXPONENT     :
    case COMPONENT_ENCODING_BC           :
    case COMPONENT_ENCODING_P8           :
    case COMPONENT_ENCODING_SHARED5BITEXP:
        return 0.0f;
    case COMPONENT_ENCODING_FP           :
        switch( Bits )
        {
        case 32: return powf( 2.f, -24.f );
        case 16: return powf( 2.f, -11.f );;
        case 11: return powf( 2.f, -6.f );;
        case 10: return powf( 2.f, -6.f );;
        }
        break;
    case COMPONENT_ENCODING_SINT         :
    case COMPONENT_ENCODING_UINT         :
        return 1.f / (float) ( (1<<(Bits-1)) - 1 ); 
    case COMPONENT_ENCODING_XRBIAS       :
        assert( Bits == 10 );
        return 1.f;
    }
    return 0.f;
}

float CFormatHelper::MaxRepresentable( COMPONENT_ENCODING e, UINT Bits )
{
    switch ( e )
    {
    case COMPONENT_ENCODING_NONE         :
    case COMPONENT_ENCODING_X            :
    case COMPONENT_ENCODING_EXPONENT     :
    case COMPONENT_ENCODING_BC           :
    case COMPONENT_ENCODING_P8           :
        return 0.f;
    case COMPONENT_ENCODING_SHARED5BITEXP:
        return (float)(1<<Bits) * (2<<5);
    case COMPONENT_ENCODING_FP           :
        switch( Bits )
        {
        case 32: return FLT_MAX;
        case 16: return MAX_FLOAT16;
        case 11: return MAX_FLOAT11;
        case 10: return MAX_FLOAT10;
        }
        break;
    case COMPONENT_ENCODING_SINT         :
        return 1.f;
    case COMPONENT_ENCODING_UINT         :
        return 1.f;
    case COMPONENT_ENCODING_XRBIAS       :
        assert( Bits == 10 );
        return 1.f;
    }
    return 0.f;
}

float CFormatHelper::MinRepresentable( COMPONENT_ENCODING e, UINT Bits )
{
    switch ( e )
    {
    case COMPONENT_ENCODING_SINT         :
        return -1.f;
    case COMPONENT_ENCODING_UINT         :
        return 0.f;
    case COMPONENT_ENCODING_XRBIAS       :
        assert( Bits == 10 );
        return 1.f;
    case COMPONENT_ENCODING_FP           :
        switch( Bits )
        {
        case 32: return FLT_MIN;
        case 16: return MIN_FLOAT16;
        case 11: return MIN_FLOAT11;
        case 10: return MIN_FLOAT10;
        }
        break;
    }
    return -1.f * MaxRepresentable(e, Bits );
}

