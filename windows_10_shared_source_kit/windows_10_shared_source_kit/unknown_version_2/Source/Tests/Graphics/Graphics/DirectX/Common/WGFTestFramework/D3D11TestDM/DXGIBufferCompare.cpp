1e, 0xf43e, 0xf45e, 0xf47e, 0xf49e, 0xf4be, 0xf4de, 0xf4fe, 0xf51e, 0xf53e, 0xf55e, 0xf57e, 0xf59e, 0xf5be, 0xf5de, 0xf5fe, 
	0xf61e, 0xf63e, 0xf65e, 0xf67e, 0xf69e, 0xf6be, 0xf6de, 0xf6fe, 0xf71e, 0xf73e, 0xf75e, 0xf77e, 0xf79e, 0xf7be, 0xf7de, 0xf7fe, 
	0xf81f, 0xf83f, 0xf85f, 0xf87f, 0xf89f, 0xf8bf, 0xf8df, 0xf8ff, 0xf91f, 0xf93f, 0xf95f, 0xf97f, 0xf99f, 0xf9bf, 0xf9df, 0xf9ff, 
	0xfa1f, 0xfa3f, 0xfa5f, 0xfa7f, 0xfa9f, 0xfabf, 0xfadf, 0xfaff, 0xfb1f, 0xfb3f, 0xfb5f, 0xfb7f, 0xfb9f, 0xfbbf, 0xfbdf, 0xfbff, 
	0xfc1f, 0xfc3f, 0xfc5f, 0xfc7f, 0xfc9f, 0xfcbf, 0xfcdf, 0xfcff, 0xfd1f, 0xfd3f, 0xfd5f, 0xfd7f, 0xfd9f, 0xfdbf, 0xfddf, 0xfdff, 
	0xfe1f, 0xfe3f, 0xfe5f, 0xfe7f, 0xfe9f, 0xfebf, 0xfedf, 0xfeff, 0xff1f, 0xff3f, 0xff5f, 0xff7f, 0xff9f, 0xffbf, 0xffdf, 0xffff
};
// ======================================================================================
//
//  CDevice8.cpp
//
//  Copyright (C) 2000 Microsoft Corporation
//
// ======================================================================================

#include "DevicePCH.h"
#include "CDevice.h"
#include "D3DX8.h"

HRESULT CDevice::D3DXCreateMesh8(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD* pDeclaration, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMesh(NumFaces, NumVertices, Options, pDeclaration, m_pDevice8, ppMesh);
}

HRESULT CDevice::D3DXCreateMeshFVF8(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh)
{
	return ::D3DXCreateMeshFVF(NumFaces, NumVertices, Options, FVF, m_pDevice8, ppMesh);
}

HRESULT CDevice::GenerateTexture8(HRESULT hr, LPDIRECT3DTEXTURE8 pTexture, CTexture** ppTexture, CDevice* pDevice)
{
	if(FAILED(hr))
		return hr;

	CTexture* pct = new CTexture;
	if (pct == NULL)
		return E_FAIL;
	if(pct->Init(pTexture, pDevice) )
	{
		*ppTexture = pct;
	}
	else
	{
		delete pct;
		*ppTexture = NULL;
	}

	RELEASE(pTexture);
	return hr;
}

HRESULT CDevice::D3DXCreateTexture8(UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTexture(m_pDevice8, Width, Height, MipLevels, Usage, Format, Pool, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileA8(LPCSTR pSrcFile, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileA(m_pDevice8, pSrcFile, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileExA8(LPCSTR pSrcFile, UINT Size, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileExA(m_pDevice8, pSrcFile, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemory8(LPCVOID pSrcData, UINT SrcData, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemory(m_pDevice8, pSrcData, SrcData, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromFileInMemoryEx8(LPCVOID pSrcData, UINT SrcData, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromFileInMemoryEx(m_pDevice8, pSrcData, SrcData, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromResourceA8(HMODULE hSrcModule, LPCSTR pSrcResource, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceA(m_pDevice8, hSrcModule, pSrcResource, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT CDevice::D3DXCreateTextureFromResourceExA8(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, CTexture** ppTexture)
{
	LPDIRECT3DTEXTURE8 pTexture = NULL;
	HRESULT hr = ::D3DXCreateTextureFromResourceExA(m_pDevice8, hSrcModule, pSrcResource, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
	return GenerateTexture8(hr, pTexture, ppTexture, this);
}

HRESULT	CDevice::D3DXSaveSurfaceToFileA8(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect)
{
	return ::D3DXSaveSurfaceToFileA( pDestFile, DestFormat, pSrcSurface == NULL ? NULL : pSrcSurface->GetD3DSurface8(), pSrcPalette, pSrcRect );
}

HRESULT CDevice::D3DXLoadSurfaceFromSurface8(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
    return ::D3DXLoadSurfaceFromSurface(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcSurface->GetD3DSurface8(), pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXLoadSurfaceFromFileInMemory8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcData, UINT SrcData, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFileInMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcData, SrcData, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXLoadSurfaceFromMemory8( CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcPitch, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
	return ::D3DXLoadSurfaceFromMemory(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, pSrcMemory, SrcFormat, SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT CDevice::D3DXLoadSurfaceFromFile8(CSurface* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPCSTR sFilename, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
	return ::D3DXLoadSurfaceFromFile(pDestSurface == NULL ? NULL : pDestSurface->GetD3DSurface8(), pDestPalette, pDestRect, sFilename, pSrcRect, Filter, ColorKey, pSrcInfo);
}

HRESULT CDevice::D3DXSaveTextureToFileA8(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat, LPBASETEXTURE pSrcTexture, CONST PALETTEENTRY* pSrcPalette)
{
	return ::D3DXSaveTextureToFileA(pDestFile, DestFormat, pSrcTexture == NULL ? NULL : pSrcTexture->GetD3DTexture8(), pSrcPalette);
}

HRESULT CDevice::D3DXLoadVolumeFromVolume8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,CVolume* pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromVolume(pDestVolume == NULL ? NULL : pDestVolume->GetD3DVolume8(),pDestPalette,(D3DBOX*)pDestBox,pSrcVolume == NULL ? NULL : pSrcVolume->GetD3DVolume8(),pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT CDevice::D3DXLoadVolumeFromMemory8(CVolume* pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST BOX* pDestBox,LPCVOID  pSrcMemory,FORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST BOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
{
	return ::D3DXLoadVolumeFromMemory(NULL == pDestVolume ? NULL : pDestVolume->GetD3DVolume8(),pDestPalette,(D3DBOX*)pDestBox,pSrcMemory,SrcFormat.d3dfFormat,SrcRowPitch,SrcSlicePitch,pSrcPalette,(D3DBOX*)pSrcBox,Filter,ColorKey);
}

HRESULT CDevice::D3DXFillTexture8( LPTEXTURE pTexture, void* pFunction, LPVOID pData)
{
	return ::D3DXFillTexture( pTexture == NULL ? NULL : (LPDIRECT3DTEXTURE8)pTexture->GetD3DTexture8(), (LPD3DXFILL2D)pFunction, pData );
}

HRESULT CDevice::D3DXFilterTexture8(CBaseTexture* pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD MipFilter)
{
	return ::D3DXFilterTexture( pBaseTexture->GetD3DTexture8(), pPalette, SrcLevel, MipFilter );
}#include <d3d8.h>
#include <d3dx8.h>
#include "MVertex.h"

bool MVertex::SetDefaultMatrices(void) {
    D3DXMATRIX			Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DXVECTOR3			from(0.0f, 0.0f, 1.0f); 
    D3DXVECTOR3	        at(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3	        up(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = -2.f;
	float               fZf = 2.f;

    
	// Projection matrix
    D3DXMatrixOrthoLH(&Matrix, 2.f, 2.f, fZn, fZf);
	if (!SetTransform(D3DTS_PROJECTION, (LPD3DMATRIX)&Matrix))
		return false;

	D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, (LPD3DMATRIX)&Matrix))
        return false;

    if (!SetTransform(D3DTS_VIEW, (LPD3DMATRIX)&Matrix))
        return false;

    return true;
}

bool MVertex::SetDefaultMaterials(void) {
    MATERIAL		Material;

    ZeroMemory(&Material,sizeof(MATERIAL));
    Material.diffuse.r  = 1.0f;
    Material.diffuse.g  = 1.0f;
    Material.diffuse.b  = 1.0f;
    Material.diffuse.a  = 1.0f;
    Material.ambient.r  = 1.0f;
    Material.ambient.g  = 1.0f;
    Material.ambient.b  = 1.0f;
    Material.ambient.a  = 1.0f;
    Material.specular.r = 1.0f;
    Material.specular.g = 1.0f;
    Material.specular.b = 1.0f;
    Material.specular.a = 1.0f;
    Material.emissive.r = 0.0f;
    Material.emissive.g = 0.0f;
    Material.emissive.b = 0.0f;
    Material.emissive.a = 0.0f;
    Material.power      = 20.0f;

    if (!SetMaterial(&Material))
        return false;

    return true;
}


bool MVertex::SetDefaultLights(void) {
    LIGHT		Light;
	D3DXVECTOR3	vLightPos = D3DXVECTOR3(0.f, 0.f, -500.f);
	D3DXVECTOR3	vLightDir = D3DXVECTOR3(0.f, 0.f, 1.f);

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.dltType		= D3DLIGHT_SPOT;
	Light.Diffuse.r		= 1.0f;
	Light.Diffuse.g		= 1.0f;
	Light.Diffuse.b		= 1.0f;
	Light.Diffuse.a		= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range           = 10e10f;
	Light.Attenuation0    = 1.f;
	Light.Attenuation1    = 0.f;
	Light.Attenuation2    = 0.f;
    Light.Theta			= pi/50;
    Light.Phi				= pi/20;

	if (!SetLight(0,&Light))
		return false;
	if (!LightEnable(0, TRUE))
		return false;
	return true;
}


bool MVertex::SetDefaultRenderStates(void) {
	SetRenderState(D3DRS_AMBIENT,RGB(200,200,200));
	SetRenderState(D3DRS_LOCALVIEWER,(DWORD)TRUE);
	SetRenderState(D3DRS_CLIPPING, (DWORD)TRUE);
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
	SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)TEXF_POINT);
	SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
	SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
	return true;
}
// DXGIBufferCompare.cpp
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

#include "pch.h"

#include "DXGIBufferCompare.h"
#include "DXGIFloatTypes.h"

extern const UINT8 expand_2_to_8[];
extern const UINT8 expand_3_to_8[];
extern const UINT8 expand_4_to_8[];
extern const UINT8 expand_5_to_8[];
extern const UINT8 expand_6_to_8[];
extern const UINT16 expand_2_to_16[];
extern const UINT16 expand_10_to_16[];
extern const UINT16 expand_11_to_16[];

static void ExpandBits( DWORD in, int nInBitSize, UINT8 *out )
{
	switch( nInBitSize )
	{
		case 1:
			*out = in ? 0 : 255;
			break;
		case 2:
			*out = expand_2_to_8[in];
			break;
		case 3:
			*out = expand_3_to_8[in];
			break;
		case 4:
			*out = expand_4_to_8[in];
			break;
		case 5:
			*out = expand_5_to_8[in];
			break;
		case 6:
			*out = expand_6_to_8[in];
			break;
		case 7:
			*out = static_cast<UINT8>( ( in << 1 ) | ( in >> 6 ) );
			break;
		default:
			// BUGBUG - Spit out some warning here
			break;
	}
}

static void ExpandBits( DWORD in, int nInBitSize, UINT16 *out )
{
	switch( nInBitSize )
	{
		case 1:
			*out = in ? 0 : 0xffff;
			break;
		case 2:
			*out = expand_2_to_16[in];
			break;
		default:
		{
			int nShift = 16 - nInBitSize;
			*out = static_cast<UINT16>( ( in << nShift ) | ( in >> ( nInBitSize - nShift ) ) );
			break;
		}
	}
}

static void ExpandBits( DWORD in, int nInBitSize, UINT32 *out )
{
	int nShift = 32 - nInBitSize;
	*out = ( in << nShift ) | ( in >> ( nInBitSize - nShift ) );
}

static void ExpandBits( DWORD in, int nInBitSize, INT8 *out )
{
	switch( nInBitSize )
	{
		case 1:
			*out = static_cast<INT8>( in ? 0 : 255 );
			break;
		case 2:
			*out = static_cast<INT8>( expand_2_to_8[in] );
			break;
		case 3:
			*out = static_cast<INT8>( expand_3_to_8[in] );
			break;
		case 4:
			*out = static_cast<INT8>( expand_4_to_8[in] );
			break;
		case 5:
			*out = static_cast<INT8>( expand_5_to_8[in] );
			break;
		case 6:
			*out = static_cast<INT8>( expand_6_to_8[in] );
			break;
		case 7:
			*out = static_cast<INT8>( ( in << 1 ) | ( in >> 6 ) );
			break;
		default:
			// BUGBUG - Spit out some warning here
			break;
	}
}

static void ExpandBits( DWORD in, int nInBitSize, INT16 *out )
{
	switch( nInBitSize )
	{
		case 1:
			*out = static_cast<INT16>( in ? 0 : 0xffff );
			break;
		case 2:
			*out = static_cast<INT16>( expand_2_to_16[in] );
			break;
		default:
		{
			int nShift = 16 - nInBitSize;
			*out = static_cast<INT16>( ( in << nShift ) | ( in >> ( nInBitSize - nShift ) ) );
			break;
		}
	}
}

static void ExpandBits( DWORD in, int nInBitSize, INT32 *out )
{
	int nShift = 32 - nInBitSize;
	*out = static_cast<INT32>( ( in << nShift ) | ( in >> ( nInBitSize - nShift ) ) );
}

static void ExpandBits( DWORD in, int nInBitSize, float *out )
{
	switch( nInBitSize )
	{
		case 16:
			*out = *( (CDXGIFloat16 *) &in );
			break;
			
		case 11:
			*out = *( (CDXGIFloat11 *) &in );
			break;
			
		case 10:
			*out = *( (CDXGIFloat10 *) &in );
			break;
	};
}


// _Type should be an unsigned integer type of size pDesc->uChunkBitSize.
// _S should be the type that the components get expanded to.
template< typename _Type, typename _S > void CacheCompactElements( _Type *pElement, _S *pCache, CBufferCompare::ElementDesc *pDesc, UINT uNumElements, UINT uElementStride )
{
	_Type mask;
	DWORD dwBits;
	int nNumBits;
	
	// Write only to even indexes in cache since cached data should be interleaved	

	switch( pDesc->uNumComponents )
	{
		case 1:
			for( int i = uNumElements; i > 0; --i )
			{
				nNumBits = pDesc->uCompBitSize[0];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[0]] >> pDesc->uCompShift[0] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[0], &pCache[0] );
				
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 2;
			}
			break;
		case 2:
			for( int i = uNumElements; i > 0; --i )
			{
				nNumBits = pDesc->uCompBitSize[0];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[0]] >> pDesc->uCompShift[0] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[0], &pCache[0] );
				
				nNumBits = pDesc->uCompBitSize[1];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[1]] >> pDesc->uCompShift[1] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[1], &pCache[2] );
				
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 4;
			}
			break;
		case 3:
			for( int i = uNumElements; i > 0; --i )
			{
				nNumBits = pDesc->uCompBitSize[0];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[0]] >> pDesc->uCompShift[0] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[0], &pCache[0] );
				
				nNumBits = pDesc->uCompBitSize[1];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[1]] >> pDesc->uCompShift[1] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[1], &pCache[2] );
				
				nNumBits = pDesc->uCompBitSize[2];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[2]] >> pDesc->uCompShift[2] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[2], &pCache[4] );
				
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 6;
			}
			break;
		case 4:
			for( int i = uNumElements; i > 0; --i )
			{
				nNumBits = pDesc->uCompBitSize[0];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[0]] >> pDesc->uCompShift[0] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[0], &pCache[0] );
				
				nNumBits = pDesc->uCompBitSize[1];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[1]] >> pDesc->uCompShift[1] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[1], &pCache[2] );
				
				nNumBits = pDesc->uCompBitSize[2];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[2]] >> pDesc->uCompShift[2] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[2], &pCache[4] );
				
				nNumBits = pDesc->uCompBitSize[3];
				mask = nNumBits == 32 ? 0xffffffff : ( 1 << nNumBits ) - 1;
				dwBits = ( pElement[pDesc->uCompChunkIndex[3]] >> pDesc->uCompShift[3] ) & mask;
				ExpandBits( dwBits, pDesc->uCompBitSize[3], &pCache[6] );
				
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 8;
			}
			break;
	}
}

// _Type should be the type of the components of the elements.
template< typename _Type > void CacheSimpleElements( _Type *pElement, _Type *pCache, CBufferCompare::ElementDesc *pDesc, UINT uNumElements, UINT uElementStride )
{
	// Write only to even indexes in cache since cached data should be interleaved	
	switch( pDesc->uNumComponents )
	{
		case 1:
			for( int i = uNumElements; i > 0; --i )
			{
				pCache[0] = pElement[pDesc->uCompChunkIndex[0]];
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 2;
			}
			break;
		case 2:
			for( int i = uNumElements; i > 0; --i )
			{
				pCache[0] = pElement[pDesc->uCompChunkIndex[0]];
				pCache[2] = pElement[pDesc->uCompChunkIndex[1]];
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 4;
			}
			break;
		case 3:
			for( int i = uNumElements; i > 0; --i )
			{
				pCache[0] = pElement[pDesc->uCompChunkIndex[0]];
				pCache[2] = pElement[pDesc->uCompChunkIndex[1]];
				pCache[4] = pElement[pDesc->uCompChunkIndex[2]];
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 6;
			}
			break;
		case 4:
			for( int i = uNumElements; i > 0; --i )
			{
				pCache[0] = pElement[pDesc->uCompChunkIndex[0]];
				pCache[2] = pElement[pDesc->uCompChunkIndex[1]];
				pCache[4] = pElement[pDesc->uCompChunkIndex[2]];
				pCache[6] = pElement[pDesc->uCompChunkIndex[3]];
				pElement = (_Type *) ( ( (char *) pElement ) + uElementStride );
				pCache += 8;
			}
			break;
		default:
			// BUGBUG - TODO: Assert here or something.
			break;
	}
}

#define DEFAULT_FLOAT32_TOL_REL (0.0000002384185791015625f) // 2 ^ -23
#define DEFAULT_FLOAT32_TOL_ABS (0.0000002384185791015625f) // 2 ^ -23
#define DEFAULT_FLOAT16_TOL_REL (0.001953125f) // 2 ^ -9
#define DEFAULT_FLOAT16_TOL_ABS (0.001953125f) // 2 ^ -9
#define DEFAULT_FLOAT10_TOL_REL (0.0625f) // 2 ^ -4
#define DEFAULT_FLOAT10_TOL_ABS (0.0625f) // 2 ^ -4
#define DEFAULT_INT32_TOL_PCT (.10f)
#define DEFAULT_INT32_TOL_ABS 255
#define DEFAULT_INT16_TOL_PCT (.10f)
#define DEFAULT_INT16_TOL_ABS 15
#define DEFAULT_INT8_TOL_PCT (.10f)
#define DEFAULT_INT8_TOL_ABS 25
#define DEFAULT_PER_ELEMENT_TOL_ADJUSTMENT (1.0f)

CBufferCompare::CBufferCompare() :
	m_fFloat32TolRel( DEFAULT_FLOAT32_TOL_REL ),
	m_fFloat32TolAbs( DEFAULT_FLOAT32_TOL_ABS ),
	m_fFloat16TolRel( DEFAULT_FLOAT16_TOL_REL ),
	m_fFloat16TolAbs( DEFAULT_FLOAT16_TOL_ABS ),
	m_fFloat10TolRel( DEFAULT_FLOAT10_TOL_REL ),
	m_fFloat10TolAbs( DEFAULT_FLOAT10_TOL_ABS ),
	m_uFloat32TolULP( 0 ),
	m_uFloat16TolULP( 0 ),
	m_uFloat10TolULP( 0 ),
	m_fInt32TolPct( DEFAULT_INT32_TOL_PCT ),
	m_uInt32TolAbs( DEFAULT_INT32_TOL_ABS ),
	m_fInt16TolPct( DEFAULT_INT16_TOL_PCT ),
	m_uInt16TolAbs( DEFAULT_INT16_TOL_ABS ),
	m_fInt8TolPct( DEFAULT_INT8_TOL_PCT ),
	m_uInt8TolAbs( DEFAULT_INT8_TOL_ABS ),
	m_fPerElementTolAdjustment( DEFAULT_PER_ELEMENT_TOL_ADJUSTMENT )
{
	m_pCache = new INT8[COMPARE_CACHE_SIZE]();
}


CBufferCompare::~CBufferCompare()
{
	delete [] m_pCache;
	m_pCache = NULL;
}


bool CBufferCompare::DXGIFormatToElementDesc( DXGI_FORMAT Format, ElementDesc *desc )
{
	// Initialize default values
	memset( desc, 0, sizeof( ElementDesc ) );
	desc->uCompChunkIndex[0] = 0;
	desc->uCompChunkIndex[1] = 1;
	desc->uCompChunkIndex[2] = 2;
	desc->uCompChunkIndex[3] = 3;
	desc->Format = Format;
	
	switch( Format )
	{
		//
		// Native component type formats
		//
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			desc->uElementSize = 16;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 32;
			break;
			
		case DXGI_FORMAT_R32G32B32A32_SINT:
			desc->uElementSize = 16;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_INT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 32;
			break;
			
		case DXGI_FORMAT_R32G32B32A32_UINT:
			desc->uElementSize = 16;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 32;
			break;
			
		case DXGI_FORMAT_R32G32B32_FLOAT:
			desc->uElementSize = 12;
			desc->uNumComponents = 3;
			desc->uNumChunks = 3;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = 32;
			break;
			
		case DXGI_FORMAT_R32G32B32_SINT:
			desc->uElementSize = 12;
			desc->uNumComponents = 3;
			desc->uNumChunks = 3;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_INT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = 32;
			break;
			
		case DXGI_FORMAT_R32G32B32_UINT:
			desc->uElementSize = 12;
			desc->uNumComponents = 3;
			desc->uNumChunks = 3;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = 32;
			break;
			
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
			desc->uElementSize = 8;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_UINT16;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 16;
			break;
			
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
			desc->uElementSize = 8;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_INT16;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 16;
			break;
		
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			desc->uElementSize = 8;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 16;
			break;
			
		case DXGI_FORMAT_R32G32_FLOAT:
			desc->uElementSize = 8;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	32;
			break;
			
		case DXGI_FORMAT_R32G32_SINT:
			desc->uElementSize = 8;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_INT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	32;
			break;
			
		case DXGI_FORMAT_R32G32_UINT:
			desc->uElementSize = 8;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT32;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	32;
			break;
			
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 8;
			break;
			
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 4;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_INT8;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	desc->uCompBitSize[2] = desc->uCompBitSize[3] = 8;
			break;
		
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_UINT16;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	16;
			break;
		
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_INT16;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	16;
			break;
		
		case DXGI_FORMAT_R16G16_FLOAT:
			desc->uElementSize = 4;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	16;
			break;
			
		case DXGI_FORMAT_R32_SINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_INT16;
			desc->uCompBitSize[0] =	32;
			break;
			
		case DXGI_FORMAT_R32_UINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT16;
			desc->uCompBitSize[0] =	32;
			break;
			
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_D32_FLOAT:
			desc->uElementSize = 4;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	32;
			break;
			
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
			desc->uElementSize = 2;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	8;
			break;
			
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
			desc->uElementSize = 2;
			desc->uNumComponents = 2;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_INT8;
			desc->uCompBitSize[0] =	desc->uCompBitSize[1] =	8;
			break;
			
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_D16_UNORM:
			desc->uElementSize = 2;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_UINT16;
			desc->uCompBitSize[0] =	16;
			break;
			
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
			desc->uElementSize = 2;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_INT16;
			desc->uCompBitSize[0] =	16;
			break;
			
		case DXGI_FORMAT_R16_FLOAT:
			desc->uElementSize = 2;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 16;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	16;
			break;
			
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
			desc->uElementSize = 1;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[0] =	8;
			break;
		
		case DXGI_FORMAT_A8_UNORM:
			desc->uElementSize = 1;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[3] = 8;
			break;
			
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
			desc->uElementSize = 1;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_INT8;
			desc->uCompBitSize[0] =	8;
			break;
			
		//
		// Compacted formats
		//
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 4;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT16;
			desc->uCompBitSize[0] =	10;
			desc->uCompBitSize[1] =	10;
			desc->uCompBitSize[2] = 10;
			desc->uCompBitSize[3] = 2;
			desc->uCompShift[0] = 22;
			desc->uCompShift[1] = 12;
			desc->uCompShift[2] = 2;
			desc->uCompShift[3] = 0;
			desc->uCompChunkIndex[0] = desc->uCompChunkIndex[1] = desc->uCompChunkIndex[2] = desc->uCompChunkIndex[3] = 0;
			break;
			
		case DXGI_FORMAT_R11G11B10_FLOAT:
			desc->uElementSize = 4;
			desc->uNumComponents = 3;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	11;
			desc->uCompBitSize[1] =	11;
			desc->uCompBitSize[2] = 10;
			desc->uCompShift[0] = 21;
			desc->uCompShift[1] = 10;
			desc->uCompShift[2] = 0;
			desc->uCompChunkIndex[0] = desc->uCompChunkIndex[1] = desc->uCompChunkIndex[2] = desc->uCompChunkIndex[3] = 0;
			break;
			
		//
		// Partial formats
		//
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			desc->uElementSize = 8;
			desc->uNumComponents = 1;
			desc->uNumChunks = 8;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[1] =	8;
			desc->uCompChunkIndex[1] = 4;
			break;
			
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			desc->uElementSize = 8;
			desc->uNumComponents = 1;
			desc->uNumChunks = 2;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_FLOAT;
			desc->uCompBitSize[0] =	32;
			break;
			
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			desc->uElementSize = 32;
			desc->uNumComponents = 1;
			desc->uNumChunks = 1;
			desc->uChunkBitSize = 32;
			desc->compareType = TYPE_UINT32;
			desc->uCompBitSize[0] = 24;
			desc->uCompShift[0] = 8;
			break;
			
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			desc->uElementSize = 4;
			desc->uNumComponents = 1;
			desc->uNumChunks = 4;
			desc->uChunkBitSize = 8;
			desc->compareType = TYPE_UINT8;
			desc->uCompBitSize[1] =	8;
			desc->uCompChunkIndex[1] = 4;
			break;
			
		//
		// Special case formats			
		//
		case DXGI_FORMAT_R1_UNORM:
		
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			return false;
			break;	

		//
		// Mixed formats are rejected for comparison for now.
		//	
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return false;
			break;
	}
	
	return true;
}

// Generic compare method
#define ABS_VALUE( a ) ( a < 0 ? -a : a )

// Return value <= 1.0 indicates matching comparison.
template< typename _Type >
float Compare( _Type src, _Type ref, _Type max_error )
{
	float diff = (float) ( src - ref );
	diff = fabs( diff );
	return diff / max_error;
}


template< typename _Type> 
int CacheCompare( _Type *pCache, int nComponentsPerElement, int uNumElements, float fTolPct, UINT32 uTolAbs, float m_fPerElementTolAdjustment )
{
	int nFailCount = 0;
	float *pErrorScale;
	float fElementErrorMax = m_fPerElementTolAdjustment * nComponentsPerElement;
	
	for( int i = uNumElements; i > 0; --i )
	{
		bool bPass = true;
		float fErrorAcc = 0;
		for( int j = nComponentsPerElement; j > 0; --j )
		{
			// Calculate max_error
			_Type maxError = max( (_Type) uTolAbs, (_Type) ( fTolPct * pCache[1] ) );
			float fError = Compare< _Type >( pCache[0], pCache[1], maxError );
			
			if( fError > 1.f )
				bPass = false;
			
			fErrorAcc += fError;
			
			pCache += 2;
		}
		
		if( fErrorAcc > fElementErrorMax )
			bPass = false;
		
		if( !bPass )
			nFailCount++;
	}
	
	return nFailCount;
}

int CacheCompareFloat( float *pCache, int nComponentsPerElement, int uNumElements, float fTolRel[4], float fTolAbs[4], float m_fPerElementTolAdjustment )
{
	int nFailCount = 0;
	float fElementErrorMax = m_fPerElementTolAdjustment * nComponentsPerElement;
	
	for( int i = uNumElements; i > 0; --i )
	{
		bool bPass = true;
		float fErrorAcc = 0;
		
		for( int j = nComponentsPerElement; j > 0; --j )
		{
			// Calculate max_error
			UINT32 uRef = *( (UINT32 *) &pCache[1] );
			uRef &= 0x7f800000;
			float maxError = max( fTolAbs[j], fTolRel[j] * *( (float *) &uRef ) );
			
			float fError = Compare< float >( pCache[0], pCache[1], maxError );
			
			if( fError > 1.f )
				bPass = false;
			
			fErrorAcc += fError;
			
			pCache += 2;
		}
		
		if( fErrorAcc > fElementErrorMax )
			bPass = false;
		
		if( !bPass )
			nFailCount++;
	}
		
	return nFailCount;
}

int CacheCompareFloatULP( float *pCache, int nComponentsPerElement, int uNumElements, UINT32 uTolULP[4], float m_fPerElementTolAdjustment )
{
	int nFailCount = 0;
	
	for( int i = uNumElements; i > 0; --i )
	{
		bool bPass = true;
		float fErrorAcc = 0;
		
		for( int j = nComponentsPerElement; j > 0; --j )
		{
			// Calculate max_error
			UINT32 uMax = *( ( UINT32 *) &pCache[0] ) + uTolULP[j];
			float maxError = *( (float *) &uMax ) - pCache[0];
			
			float fError = Compare< float >( pCache[0], pCache[1], maxError );
			
			if( fError > 1.f )
				bPass = false;
			
			fErrorAcc += fError;
			
			pCache += 2;
		}
		
		if( fErrorAcc > m_fPerElementTolAdjustment * nComponentsPerElement )
			bPass = false;
		
		if( !bPass )
			nFailCount++;
	}
		
	return nFailCount;
}

UINT CBufferCompare::Compare(
		void *pSrcData, 
		void *pRefData, 
		DXGI_FORMAT Format, 
		UINT uElementOffset, 
		UINT uElementStride,
		UINT uWidth,
		UINT uHeight,
		UINT uSrcRowPitch,
		UINT uRefRowPitch,
		UINT uDepth, 
		UINT uSrcDepthPitch,
		UINT uRefDepthPitch
		)
{
	UINT i, j, k;

	// The following variables are cached in case repeated sequential
	// calls to CompareElements use the same format.  This will speed up
	// comparison of two dimensional data such as texture surfaces.
	static DXGI_FORMAT lastFormat = DXGI_FORMAT_UNKNOWN;
	static ElementDesc desc;
	static float fFloatTolRel[4];
	static float fFloatTolAbs[4];
	static UINT32 uFloatTolULP[4];
	static UINT uElementSizeInCache = 0;
	static bool bSimple = false;

	UINT uNumFailures = 0;
	
	if( lastFormat != Format )
	{
		lastFormat = Format;
			
		if( !DXGIFormatToElementDesc( Format, &desc ) )
			return false;	// No comparison method for this format (yet)
			
		switch( desc.compareType )
		{
			case TYPE_INT32:
			case TYPE_UINT32:
				uElementSizeInCache = desc.uNumComponents * 4;
				break;
			case TYPE_INT16:
			case TYPE_UINT16:
				uElementSizeInCache = desc.uNumComponents * 2;
				break;
			case TYPE_INT8:
			case TYPE_UINT8:
				uElementSizeInCache = desc.uNumComponents * 1;
				break;
			case TYPE_FLOAT:
				uElementSizeInCache = desc.uNumComponents * sizeof( float );
				break;
		}
		
		//
		// Determine tolerance values
		//	
		for( i = 0; i < desc.uNumComponents; ++i )
		{
			// Set scale[i] to range of values
			if( desc.compareType == TYPE_FLOAT )
			{
				// Calculate tolerance based on number of bits
				if( desc.uCompBitSize[i] >= 32 )
				{
					fFloatTolRel[i] = m_fFloat32TolRel;
					fFloatTolAbs[i] = m_fFloat32TolAbs;
					uFloatTolULP[i] = m_uFloat32TolULP;
				}
				else
				if( desc.uCompBitSize[i] >= 16 )
				{
					fFloatTolRel[i] = m_fFloat16TolRel;
					fFloatTolAbs[i] = m_fFloat16TolAbs;
					uFloatTolULP[i] = m_uFloat16TolULP;
				}
				else
				if( desc.uCompBitSize[i] >= 10 )
				{
					fFloatTolRel[i] = m_fFloat10TolRel;
					fFloatTolAbs[i] = m_fFloat10TolAbs;
					uFloatTolULP[i] = m_uFloat10TolULP;
				}
			}
		}
		
		// Determine if elements are simple (no compact data)
		bSimple = true;
		
		for( i = 0; i < desc.uNumComponents; ++i )
		{
			if( ( desc.uCompBitSize[i] > 0 && desc.uCompBitSize[i] != desc.uChunkBitSize ) || ( desc.compareType == TYPE_FLOAT && desc.uCompBitSize[i] != 32 ) )
			{
				bSimple = false;
				break;
			}
		}
	}
	
	INT8 *pCache = m_pCache;
	
	INT8 *pSrcSlice = ( (INT8 *) pSrcData ) + uElementOffset;
	INT8 *pRefSlice = ( (INT8 *) pRefData ) + uElementOffset;
	
	UINT uMaxElements = COMPARE_CACHE_SIZE / ( uElementSizeInCache * 2 );
	UINT uNumCachedElements = 0;

	// Copy elements into cache buffer
	for( k = uDepth; k > 0; --k )
	{
		INT8 *pSrcRow = pSrcSlice;
		INT8 *pRefRow = pRefSlice;
		
		for( j = uHeight; j > 0; --j )
		{
			INT8 *pSrcElement = pSrcRow;
			INT8 *pRefElement = pRefRow;	
			
			for( i = uWidth; ; )
			{
				UINT uCacheCountAvailable = min( uMaxElements, i );
				
				if( uCacheCountAvailable == 0 )
				{
					// Compare cached elements then flush the cache.
					switch( desc.compareType )
					{
						case TYPE_FLOAT:
							if( uFloatTolULP > 0 )
								uNumFailures += CacheCompareFloatULP( (float *) m_pCache, desc.uNumComponents, uNumCachedElements, uFloatTolULP, m_fPerElementTolAdjustment );
							else
								uNumFailures += CacheCompareFloat( (float *) m_pCache, desc.uNumComponents, uNumCachedElements, fFloatTolRel, fFloatTolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_INT32:
							uNumFailures += CacheCompare<INT32>( (INT32 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt32TolPct, m_uInt32TolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_UINT32:
							uNumFailures += CacheCompare<UINT32>( (UINT32 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt32TolPct, m_uInt32TolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_INT16:
							uNumFailures += CacheCompare<INT16>( (INT16 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt16TolPct, m_uInt16TolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_UINT16:
							uNumFailures += CacheCompare<UINT16>( (UINT16 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt16TolPct, m_uInt16TolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_INT8:
							uNumFailures += CacheCompare<INT8>( (INT8 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt8TolPct, m_uInt8TolAbs, m_fPerElementTolAdjustment );
							break;
							
						case TYPE_UINT8:
							uNumFailures += CacheCompare<UINT8>( (UINT8 *) m_pCache, desc.uNumComponents, uNumCachedElements, m_fInt8TolPct, m_uInt8TolAbs, m_fPerElementTolAdjustment );
							break;
					}
					
					pCache = m_pCache;
					uNumCachedElements = 0;
					uMaxElements = COMPARE_CACHE_SIZE / ( uElementSizeInCache * 2 );
					if( i == 0 )
						break;
						
					continue;
				}
				
				i -= uCacheCountAvailable;
				uMaxElements -= uCacheCountAvailable;
				uNumCachedElements += uCacheCountAvailable;
					
				switch( desc.compareType )
				{
					case TYPE_FLOAT:
						if( bSimple )
						{
							CacheSimpleElements< float >( (float *) pSrcElement, ( (float *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< float >( (float *) pRefElement, ( (float *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, float >( (UINT8 *) pSrcElement, ( (float *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, float >( (UINT8 *) pRefElement, ( (float *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, float >( (UINT16 *) pSrcElement, ( (float *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, float >( (UINT16 *) pRefElement, ( (float *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, float >( (UINT32 *) pSrcElement, ( (float *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, float >( (UINT32 *) pRefElement, ( (float *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_INT32:
						if( bSimple )
						{
							CacheSimpleElements< INT32 >( (INT32 *) pSrcElement, ( (INT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< INT32 >( (INT32 *) pRefElement, ( (INT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, INT32 >( (UINT8 *) pSrcElement, ( (INT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, INT32 >( (UINT8 *) pRefElement, ( (INT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, INT32 >( (UINT16 *) pSrcElement, ( (INT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, INT32 >( (UINT16 *) pRefElement, ( (INT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, INT32 >( (UINT32 *) pSrcElement, ( (INT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, INT32 >( (UINT32 *) pRefElement, ( (INT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_UINT32:
						if( bSimple )
						{
							CacheSimpleElements< UINT32 >( (UINT32 *) pSrcElement, ( (UINT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< UINT32 >( (UINT32 *) pRefElement, ( (UINT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, UINT32 >( (UINT8 *) pSrcElement, ( (UINT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, UINT32 >( (UINT8 *) pRefElement, ( (UINT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, UINT32 >( (UINT16 *) pSrcElement, ( (UINT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, UINT32 >( (UINT16 *) pRefElement, ( (UINT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, UINT32 >( (UINT32 *) pSrcElement, ( (UINT32 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, UINT32 >( (UINT32 *) pRefElement, ( (UINT32 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_INT16:
						if( bSimple )
						{
							CacheSimpleElements< INT16 >( (INT16 *) pSrcElement, ( (INT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< INT16 >( (INT16 *) pRefElement, ( (INT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, INT16 >( (UINT8 *) pSrcElement, ( (INT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, INT16 >( (UINT8 *) pRefElement, ( (INT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, INT16 >( (UINT16 *) pSrcElement, ( (INT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, INT16 >( (UINT16 *) pRefElement, ( (INT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, INT16 >( (UINT32 *) pSrcElement, ( (INT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, INT16 >( (UINT32 *) pRefElement, ( (INT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_UINT16:
						if( bSimple )
						{
							CacheSimpleElements< UINT16 >( (UINT16 *) pSrcElement, ( (UINT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< UINT16 >( (UINT16 *) pRefElement, ( (UINT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, UINT16 >( (UINT8 *) pSrcElement, ( (UINT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, UINT16 >( (UINT8 *) pRefElement, ( (UINT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, UINT16 >( (UINT16 *) pSrcElement, ( (UINT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, UINT16 >( (UINT16 *) pRefElement, ( (UINT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, UINT16 >( (UINT32 *) pSrcElement, ( (UINT16 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, UINT16 >( (UINT32 *) pRefElement, ( (UINT16 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_INT8:
						if( bSimple )
						{
							CacheSimpleElements< INT8 >( (INT8 *) pSrcElement, ( (INT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< INT8 >( (INT8 *) pRefElement, ( (INT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, INT8 >( (UINT8 *) pSrcElement, ( (INT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, INT8 >( (UINT8 *) pRefElement, ( (INT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, INT8 >( (UINT16 *) pSrcElement, ( (INT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, INT8 >( (UINT16 *) pRefElement, ( (INT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, INT8 >( (UINT32 *) pSrcElement, ( (INT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, INT8 >( (UINT32 *) pRefElement, ( (INT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
					case TYPE_UINT8:
						if( bSimple )
						{
							CacheSimpleElements< UINT8 >( (UINT8 *) pSrcElement, ( (UINT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
							CacheSimpleElements< UINT8 >( (UINT8 *) pRefElement, ( (UINT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
						}
						else
						{
							switch( desc.uChunkBitSize )
							{
								case 8:
									CacheCompactElements< UINT8, UINT8 >( (UINT8 *) pSrcElement, ( (UINT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT8, UINT8 >( (UINT8 *) pRefElement, ( (UINT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
								case 16:
									CacheCompactElements< UINT16, UINT8 >( (UINT16 *) pSrcElement, ( (UINT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT16, UINT8 >( (UINT16 *) pRefElement, ( (UINT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;									
								case 32:									
									CacheCompactElements< UINT32, UINT8 >( (UINT32 *) pSrcElement, ( (UINT8 *) pCache ) + 0, &desc, uCacheCountAvailable, uElementStride );
									CacheCompactElements< UINT32, UINT8 >( (UINT32 *) pRefElement, ( (UINT8 *) pCache ) + 1, &desc, uCacheCountAvailable, uElementStride );
									break;
							}
						}
						break;
				}
				
				int increment = uCacheCountAvailable * uElementSizeInCache;
				pSrcElement += increment;
				pRefElement += increment;
				pCache += increment * 2;
			}
			
			pSrcRow += uSrcRowPitch;
			pRefRow += uRefRowPitch;
		}
		
		pSrcSlice += uSrcDepthPitch;
		pRefSlice += uRefDepthPitch;
	}	
	
	// Return the number of elements that failed the comparison
	return uNumFailures;
}

//
// Expansion Tables
//
const UINT8 expand_2_to_8[] =
{
	0x00, 0x55, 0xaa, 0xff
};

const UINT8 expand_3_to_8[] =
{
	0x00, 0x24, 0x49, 0x6d, 0x92, 0xb6, 0xdb, 0xff
};

const UINT8 expand_4_to_8[] =
{
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

const UINT8 expand_5_to_8[] =
{
	0x00, 0x08, 0x10, 0x18, 0x21, 0x29, 0x31, 0x39, 0x42, 0x4a, 0x52, 0x5a, 0x63, 0x6b, 0x73, 0x7b, 
	0x84, 0x8c, 0x94, 0x9c, 0xa5, 0xad, 0xb5, 0xbd, 0xc6, 0xce, 0xd6, 0xde, 0xe7, 0xef, 0xf7, 0xff
};

const UINT8 expand_6_to_8[] =
{
	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 
	0x41, 0x45, 0x49, 0x4d, 0x51, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d, 
	0x82, 0x86, 0x8a, 0x8e, 0x92, 0x96, 0x9a, 0x9e, 0xa2, 0xa6, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe, 
	0xc3, 0xc7, 0xcb, 0xcf, 0xd3, 0xd7, 0xdb, 0xdf, 0xe3, 0xe7, 0xeb, 0xef, 0xf3, 0xf7, 0xfb, 0xff
};

const UINT16 expand_2_to_16[] =
{
	0x0000, 0x5555, 0xaaaa, 0xffff
};

const UINT16 expand_10_to_16[] =
{
	0x0000, 0x0040, 0x0080, 0x00c0, 0x0100, 0x0140, 0x0180, 0x01c0, 0x0200, 0x0240, 0x0280, 0x02c0, 0x0300, 0x0340, 0x0380, 0x03c0, 
	0x0401, 0x0441, 0x0481, 0x04c1, 0x0501, 0x0541, 0x0581, 0x05c1, 0x0601, 0x0641, 0x0681, 0x06c1, 0x0701, 0x0741, 0x0781, 0x07c1, 
	0x0802, 0x0842, 0x0882, 0x08c2, 0x0902, 0x0942, 0x0982, 0x09c2, 0x0a02, 0x0a42, 0x0a82, 0x0ac2, 0x0b02, 0x0b42, 0x0b82, 0x0bc2, 
	0x0c03, 0x0c43, 0x0c83, 0x0cc3, 0x0d03, 0x0d43, 0x0d83, 0x0dc3, 0x0e03, 0x0e43, 0x0e83, 0x0ec3, 0x0f03, 0x0f43, 0x0f83, 0x0fc3, 
	0x1004, 0x1044, 0x1084, 0x10c4, 0x1104, 0x1144, 0x1184, 0x11c4, 0x1204, 0x1244, 0x1284, 0x12c4, 0x1304, 0x1344, 0x1384, 0x13c4, 
	0x1405, 0x1445, 0x1485, 0x14c5, 0x1505, 0x1545, 0x1585, 0x15c5, 0x1605, 0x1645, 0x1685, 0x16c5, 0x1705, 0x1745, 0x1785, 0x17c5, 
	0x1806, 0x1846, 0x1886, 0x18c6, 0x1906, 0x1946, 0x1986, 0x19c6, 0x1a06, 0x1a46, 0x1a86, 0x1ac6, 0x1b06, 0x1b46, 0x1b86, 0x1bc6, 
	0x1c07, 0x1c47, 0x1c87, 0x1cc7, 0x1d07, 0x1d47, 0x1d87, 0x1dc7, 0x1e07, 0x1e47, 0x1e87, 0x1ec7, 0x1f07, 0x1f47, 0x1f87, 0x1fc7, 
	0x2008, 0x2048, 0x2088, 0x20c8, 0x2108, 0x2148, 0x2188, 0x21c8, 0x2208, 0x2248, 0x2288, 0x22c8, 0x2308, 0x2348, 0x2388, 0x23c8, 
	0x2409, 0x2449, 0x2489, 0x24c9, 0x2509, 0x2549, 0x2589, 0x25c9, 0x2609, 0x2649, 0x2689, 0x26c9, 0x2709, 0x2749, 0x2789, 0x27c9, 
	0x280a, 0x284a, 0x288a, 0x28ca, 0x290a, 0x294a, 0x298a, 0x29ca, 0x2a0a, 0x2a4a, 0x2a8a, 0x2aca, 0x2b0a, 0x2b4a, 0x2b8a, 0x2bca, 
	0x2c0b, 0x2c4b, 0x2c8b, 0x2ccb, 0x2d0b, 0x2d4b, 0x2d8b, 0x2dcb, 0x2e0b, 0x2e4b, 0x2e8b, 0x2ecb, 0x2f0b, 0x2f4b, 0x2f8b, 0x2fcb, 
	0x300c, 0x304c, 0x308c, 0x30cc, 0x310c, 0x314c, 0x318c, 0x31cc, 0x320c, 0x324c, 0x328c, 0x32cc, 0x330c, 0x334c, 0x338c, 0x33cc, 
	0x340d, 0x344d, 0x348d, 0x34cd, 0x350d, 0x354d, 0x358d, 0x35cd, 0x360d, 0x364d, 0x368d, 0x36cd, 0x370d, 0x374d, 0x378d, 0x37cd, 
	0x380e, 0x384e, 0x388e, 0x38ce, 0x390e, 0x394e, 0x398e, 0x39ce, 0x3a0e, 0x3a4e, 0x3a8e, 0x3ace, 0x3b0e, 0x3b4e, 0x3b8e, 0x3bce, 
	0x3c0f, 0x3c4f, 0x3c8f, 0x3ccf, 0x3d0f, 0x3d4f, 0x3d8f, 0x3dcf, 0x3e0f, 0x3e4f, 0x3e8f, 0x3ecf, 0x3f0f, 0x3f4f, 0x3f8f, 0x3fcf, 
	0x4010, 0x4050, 0x4090, 0x40d0, 0x4110, 0x4150, 0x4190, 0x41d0, 0x4210, 0x4250, 0x4290, 0x42d0, 0x4310, 0x4350, 0x4390, 0x43d0, 
	0x4411, 0x4451, 0x4491, 0x44d1, 0x4511, 0x4551, 0x4591, 0x45d1, 0x4611, 0x4651, 0x4691, 0x46d1, 0x4711, 0x4751, 0x4791, 0x47d1, 
	0x4812, 0x4852, 0x4892, 0x48d2, 0x4912, 0x4952, 0x4992, 0x49d2, 0x4a12, 0x4a52, 0x4a92, 0x4ad2, 0x4b12, 0x4b52, 0x4b92, 0x4bd2, 
	0x4c13, 0x4c53, 0x4c93, 0x4cd3, 0x4d13, 0x4d53, 0x4d93, 0x4dd3, 0x4e13, 0x4e53, 0x4e93, 0x4ed3, 0x4f13, 0x4f53, 0x4f93, 0x4fd3, 
	0x5014, 0x5054, 0x5094, 0x50d4, 0x5114, 0x5154, 0x5194, 0x51d4, 0x5214, 0x5254, 0x5294, 0x52d4, 0x5314, 0x5354, 0x5394, 0x53d4, 
	0x5415, 0x5455, 0x5495, 0x54d5, 0x5515, 0x5555, 0x5595, 0x55d5, 0x5615, 0x5655, 0x5695, 0x56d5, 0x5715, 0x5755, 0x5795, 0x57d5, 
	0x5816, 0x5856, 0x5896, 0x58d6, 0x5916, 0x5956, 0x5996, 0x59d6, 0x5a16, 0x5a56, 0x5a96, 0x5ad6, 0x5b16, 0x5b56, 0x5b96, 0x5bd6, 
	0x5c17, 0x5c57, 0x5c97, 0x5cd7, 0x5d17, 0x5d57, 0x5d97, 0x5dd7, 0x5e17, 0x5e57, 0x5e97, 0x5ed7, 0x5f17, 0x5f57, 0x5f97, 0x5fd7, 
	0x6018, 0x6058, 0x6098, 0x60d8, 0x6118, 0x6158, 0x6198, 0x61d8, 0x6218, 0x6258, 0x6298, 0x62d8, 0x6318, 0x6358, 0x6398, 0x63d8, 
	0x6419, 0x6459, 0x6499, 0x64d9, 0x6519, 0x6559, 0x6599, 0x65d9, 0x6619, 0x6659, 0x6699, 0x66d9, 0x6719, 0x6759, 0x6799, 0x67d9, 
	0x681a, 0x685a, 0x689a, 0x68da, 0x691a, 0x695a, 0x699a, 0x69da, 0x6a1a, 0x6a5a, 0x6a9a, 0x6ada, 0x6b1a, 0x6b5a, 0x6b9a, 0x6bda, 
	0x6c1b, 0x6c5b, 0x6c9b, 0x6cdb, 0x6d1b, 0x6d5b, 0x6d9b, 0x6ddb, 0x6e1b, 0x6e5b, 0x6e9b, 0x6edb, 0x6f1b, 0x6f5b, 0x6f9b, 0x6fdb, 
	0x701c, 0x705c, 0x709c, 0x70dc, 0x711c, 0x715c, 0x719c, 0x71dc, 0x721c, 0x725c, 0x729c, 0x72dc, 0x731c, 0x735c, 0x739c, 0x73dc, 
	0x741d, 0x745d, 0x749d, 0x74dd, 0x751d, 0x755d, 0x759d, 0x75dd, 0x761d, 0x765d, 0x769d, 0x76dd, 0x771d, 0x775d, 0x779d, 0x77dd, 
	0x781e, 0x785e, 0x789e, 0x78de, 0x791e, 0x795e, 0x799e, 0x79de, 0x7a1e, 0x7a5e, 0x7a9e, 0x7ade, 0x7b1e, 0x7b5e, 0x7b9e, 0x7bde, 
	0x7c1f, 0x7c5f, 0x7c9f, 0x7cdf, 0x7d1f, 0x7d5f, 0x7d9f, 0x7ddf, 0x7e1f, 0x7e5f, 0x7e9f, 0x7edf, 0x7f1f, 0x7f5f, 0x7f9f, 0x7fdf, 
	0x8020, 0x8060, 0x80a0, 0x80e0, 0x8120, 0x8160, 0x81a0, 0x81e0, 0x8220, 0x8260, 0x82a0, 0x82e0, 0x8320, 0x8360, 0x83a0, 0x83e0, 
	0x8421, 0x8461, 0x84a1, 0x84e1, 0x8521, 0x8561, 0x85a1, 0x85e1, 0x8621, 0x8661, 0x86a1, 0x86e1, 0x8721, 0x8761, 0x87a1, 0x87e1, 
	0x8822, 0x8862, 0x88a2, 0x88e2, 0x8922, 0x8962, 0x89a2, 0x89e2, 0x8a22, 0x8a62, 0x8aa2, 0x8ae2, 0x8b22, 0x8b62, 0x8ba2, 0x8be2, 
	0x8c23, 0x8c63, 0x8ca3, 0x8ce3, 0x8d23, 0x8d63, 0x8da3, 0x8de3, 0x8e23, 0x8e63, 0x8ea3, 0x8ee3, 0x8f23, 0x8f63, 0x8fa3, 0x8fe3, 
	0x9024, 0x9064, 0x90a4, 0x90e4, 0x9124, 0x9164, 0x91a4, 0x91e4, 0x9224, 0x9264, 0x92a4, 0x92e4, 0x9324, 0x9364, 0x93a4, 0x93e4, 
	0x9425, 0x9465, 0x94a5, 0x94e5, 0x9525, 0x9565, 0x95a5, 0x95e5, 0x9625, 0x9665, 0x96a5, 0x96e5, 0x9725, 0x9765, 0x97a5, 0x97e5, 
	0x9826, 0x9866, 0x98a6, 0x98e6, 0x9926, 0x9966, 0x99a6, 0x99e6, 0x9a26, 0x9a66, 0x9aa6, 0x9ae6, 0x9b26, 0x9b66, 0x9ba6, 0x9be6, 
	0x9c27, 0x9c67, 0x9ca7, 0x9ce7, 0x9d27, 0x9d67, 0x9da7, 0x9de7, 0x9e27, 0x9e67, 0x9ea7, 0x9ee7, 0x9f27, 0x9f67, 0x9fa7, 0x9fe7, 
	0xa028, 0xa068, 0xa0a8, 0xa0e8, 0xa128, 0xa168, 0xa1a8, 0xa1e8, 0xa228, 0xa268, 0xa2a8, 0xa2e8, 0xa328, 0xa368, 0xa3a8, 0xa3e8, 
	0xa429, 0xa469, 0xa4a9, 0xa4e9, 0xa529, 0xa569, 0xa5a9, 0xa5e9, 0xa629, 0xa669, 0xa6a9, 0xa6e9, 0xa729, 0xa769, 0xa7a9, 0xa7e9, 
	0xa82a, 0xa86a, 0xa8aa, 0xa8ea, 0xa92a, 0xa96a, 0xa9aa, 0xa9ea, 0xaa2a, 0xaa6a, 0xaaaa, 0xaaea, 0xab2a, 0xab6a, 0xabaa, 0xabea, 
	0xac2b, 0xac6b, 0xacab, 0xaceb, 0xad2b, 0xad6b, 0xadab, 0xadeb, 0xae2b, 0xae6b, 0xaeab, 0xaeeb, 0xaf2b, 0xaf6b, 0xafab, 0xafeb, 
	0xb02c, 0xb06c, 0xb0ac, 0xb0ec, 0xb12c, 0xb16c, 0xb1ac, 0xb1ec, 0xb22c, 0xb26c, 0xb2ac, 0xb2ec, 0xb32c, 0xb36c, 0xb3ac, 0xb3ec, 
	0xb42d, 0xb46d, 0xb4ad, 0xb4ed, 0xb52d, 0xb56d, 0xb5ad, 0xb5ed, 0xb62d, 0xb66d, 0xb6ad, 0xb6ed, 0xb72d, 0xb76d, 0xb7ad, 0xb7ed, 
	0xb82e, 0xb86e, 0xb8ae, 0xb8ee, 0xb92e, 0xb96e, 0xb9ae, 0xb9ee, 0xba2e, 0xba6e, 0xbaae, 0xbaee, 0xbb2e, 0xbb6e, 0xbbae, 0xbbee, 
	0xbc2f, 0xbc6f, 0xbcaf, 0xbcef, 0xbd2f, 0xbd6f, 0xbdaf, 0xbdef, 0xbe2f, 0xbe6f, 0xbeaf, 0xbeef, 0xbf2f, 0xbf6f, 0xbfaf, 0xbfef, 
	0xc030, 0xc070, 0xc0b0, 0xc0f0, 0xc130, 0xc170, 0xc1b0, 0xc1f0, 0xc230, 0xc270, 0xc2b0, 0xc2f0, 0xc330, 0xc370, 0xc3b0, 0xc3f0, 
	0xc431, 0xc471, 0xc4b1, 0xc4f1, 0xc531, 0xc571, 0xc5b1, 0xc5f1, 0xc631, 0xc671, 0xc6b1, 0xc6f1, 0xc731, 0xc771, 0xc7b1, 0xc7f1, 
	0xc832, 0xc872, 0xc8b2, 0xc8f2, 0xc932, 0xc972, 0xc9b2, 0xc9f2, 0xca32, 0xca72, 0xcab2, 0xcaf2, 0xcb32, 0xcb72, 0xcbb2, 0xcbf2, 
	0xcc33, 0xcc73, 0xccb3, 0xccf3, 0xcd33, 0xcd73, 0xcdb3, 0xcdf3, 0xce33, 0xce73, 0xceb3, 0xcef3, 0xcf33, 0xcf73, 0xcfb3, 0xcff3, 
	0xd034, 0xd074, 0xd0b4, 0xd0f4, 0xd134, 0xd174, 0xd1b4, 0xd1f4, 0xd234, 0xd274, 0xd2b4, 0xd2f4, 0xd334, 0xd374, 0xd3b4, 0xd3f4, 
	0xd435, 0xd475, 0xd4b5, 0xd4f5, 0xd535, 0xd575, 0xd5b5, 0xd5f5, 0xd635, 0xd675, 0xd6b5, 0xd6f5, 0xd735, 0xd775, 0xd7b5, 0xd7f5, 
	0xd836, 0xd876, 0xd8b6, 0xd8f6, 0xd936, 0xd976, 0xd9b6, 0xd9f6, 0xda36, 0xda76, 0xdab6, 0xdaf6, 0xdb36, 0xdb76, 0xdbb6, 0xdbf6, 
	0xdc37, 0xdc77, 0xdcb7, 0xdcf7, 0xdd37, 0xdd77, 0xddb7, 0xddf7, 0xde37, 0xde77, 0xdeb7, 0xdef7, 0xdf37, 0xdf77, 0xdfb7, 0xdff7, 
	0xe038, 0xe078, 0xe0b8, 0xe0f8, 0xe138, 0xe178, 0xe1b8, 0xe1f8, 0xe238, 0xe278, 0xe2b8, 0xe2f8, 0xe338, 0xe378, 0xe3b8, 0xe3f8, 
	0xe439, 0xe479, 0xe4b9, 0xe4f9, 0xe539, 0xe579, 0xe5b9, 0xe5f9, 0xe639, 0xe679, 0xe6b9, 0xe6f9, 0xe739, 0xe779, 0xe7b9, 0xe7f9, 
	0xe83a, 0xe87a, 0xe8ba, 0xe8fa, 0xe93a, 0xe97a, 0xe9ba, 0xe9fa, 0xea3a, 0xea7a, 0xeaba, 0xeafa, 0xeb3a, 0xeb7a, 0xebba, 0xebfa, 
	0xec3b, 0xec7b, 0xecbb, 0xecfb, 0xed3b, 0xed7b, 0xedbb, 0xedfb, 0xee3b, 0xee7b, 0xeebb, 0xeefb, 0xef3b, 0xef7b, 0xefbb, 0xeffb, 
	0xf03c, 0xf07c, 0xf0bc, 0xf0fc, 0xf13c, 0xf17c, 0xf1bc, 0xf1fc, 0xf23c, 0xf27c, 0xf2bc, 0xf2fc, 0xf33c, 0xf37c, 0xf3bc, 0xf3fc, 
	0xf43d, 0xf47d, 0xf4bd, 0xf4fd, 0xf53d, 0xf57d, 0xf5bd, 0xf5fd, 0xf63d, 0xf67d, 0xf6bd, 0xf6fd, 0xf73d, 0xf77d, 0xf7bd, 0xf7fd, 
	0xf83e, 0xf87e, 0xf8be, 0xf8fe, 0xf93e, 0xf97e, 0xf9be, 0xf9fe, 0xfa3e, 0xfa7e, 0xfabe, 0xfafe, 0xfb3e, 0xfb7e, 0xfbbe, 0xfbfe, 
	0xfc3f, 0xfc7f, 0xfcbf, 0xfcff, 0xfd3f, 0xfd7f, 0xfdbf, 0xfdff, 0xfe3f, 0xfe7f, 0xfebf, 0xfeff, 0xff3f, 0xff7f, 0xffbf, 0xffff
};


const UINT16 expand_11_to_16[] =
{
	0x0000, 0x0020, 0x0040, 0x0060, 0x0080, 0x00a0, 0x00c0, 0x00e0, 0x0100, 0x0120, 0x0140, 0x0160, 0x0180, 0x01a0, 0x01c0, 0x01e0, 
	0x0200, 0x0220, 0x0240, 0x0260, 0x0280, 0x02a0, 0x02c0, 0x02e0, 0x0300, 0x0320, 0x0340, 0x0360, 0x0380, 0x03a0, 0x03c0, 0x03e0, 
	0x0400, 0x0420, 0x0440, 0x0460, 0x0480, 0x04a0, 0x04c0, 0x04e0, 0x0500, 0x0520, 0x0540, 0x0560, 0x0580, 0x05a0, 0x05c0, 0x05e0, 
	0x0600, 0x0620, 0x0640, 0x0660, 0x0680, 0x06a0, 0x06c0, 0x06e0, 0x0700, 0x0720, 0x0740, 0x0760, 0x0780, 0x07a0, 0x07c0, 0x07e0, 
	0x0801, 0x0821, 0x0841, 0x0861, 0x0881, 0x08a1, 0x08c1, 0x08e1, 0x0901, 0x0921, 0x0941, 0x0961, 0x0981, 0x09a1, 0x09c1, 0x09e1, 
	0x0a01, 0x0a21, 0x0a41, 0x0a61, 0x0a81, 0x0aa1, 0x0ac1, 0x0ae1, 0x0b01, 0x0b21, 0x0b41, 0x0b61, 0x0b81, 0x0ba1, 0x0bc1, 0x0be1, 
	0x0c01, 0x0c21, 0x0c41, 0x0c61, 0x0c81, 0x0ca1, 0x0cc1, 0x0ce1, 0x0d01, 0x0d21, 0x0d41, 0x0d61, 0x0d81, 0x0da1, 0x0dc1, 0x0de1, 
	0x0e01, 0x0e21, 0x0e41, 0x0e61, 0x0e81, 0x0ea1, 0x0ec1, 0x0ee1, 0x0f01, 0x0f21, 0x0f41, 0x0f61, 0x0f81, 0x0fa1, 0x0fc1, 0x0fe1, 
	0x1002, 0x1022, 0x1042, 0x1062, 0x1082, 0x10a2, 0x10c2, 0x10e2, 0x1102, 0x1122, 0x1142, 0x1162, 0x1182, 0x11a2, 0x11c2, 0x11e2, 
	0x1202, 0x1222, 0x1242, 0x1262, 0x1282, 0x12a2, 0x12c2, 0x12e2, 0x1302, 0x1322, 0x1342, 0x1362, 0x1382, 0x13a2, 0x13c2, 0x13e2, 
	0x1402, 0x1422, 0x1442, 0x1462, 0x1482, 0x14a2, 0x14c2, 0x14e2, 0x1502, 0x1522, 0x1542, 0x1562, 0x1582, 0x15a2, 0x15c2, 0x15e2, 
	0x1602, 0x1622, 0x1642, 0x1662, 0x1682, 0x16a2, 0x16c2, 0x16e2, 0x1702, 0x1722, 0x1742, 0x1762, 0x1782, 0x17a2, 0x17c2, 0x17e2, 
	0x1803, 0x1823, 0x1843, 0x1863, 0x1883, 0x18a3, 0x18c3, 0x18e3, 0x1903, 0x1923, 0x1943, 0x1963, 0x1983, 0x19a3, 0x19c3, 0x19e3, 
	0x1a03, 0x1a23, 0x1a43, 0x1a63, 0x1a83, 0x1aa3, 0x1ac3, 0x1ae3, 0x1b03, 0x1b23, 0x1b43, 0x1b63, 0x1b83, 0x1ba3, 0x1bc3, 0x1be3, 
	0x1c03, 0x1c23, 0x1c43, 0x1c63, 0x1c83, 0x1ca3, 0x1cc3, 0x1ce3, 0x1d03, 0x1d23, 0x1d43, 0x1d63, 0x1d83, 0x1da3, 0x1dc3, 0x1de3, 
	0x1e03, 0x1e23, 0x1e43, 0x1e63, 0x1e83, 0x1ea3, 0x1ec3, 0x1ee3, 0x1f03, 0x1f23, 0x1f43, 0x1f63, 0x1f83, 0x1fa3, 0x1fc3, 0x1fe3, 
	0x2004, 0x2024, 0x2044, 0x2064, 0x2084, 0x20a4, 0x20c4, 0x20e4, 0x2104, 0x2124, 0x2144, 0x2164, 0x2184, 0x21a4, 0x21c4, 0x21e4, 
	0x2204, 0x2224, 0x2244, 0x2264, 0x2284, 0x22a4, 0x22c4, 0x22e4, 0x2304, 0x2324, 0x2344, 0x2364, 0x2384, 0x23a4, 0x23c4, 0x23e4, 
	0x2404, 0x2424, 0x2444, 0x2464, 0x2484, 0x24a4, 0x24c4, 0x24e4, 0x2504, 0x2524, 0x2544, 0x2564, 0x2584, 0x25a4, 0x25c4, 0x25e4, 
	0x2604, 0x2624, 0x2644, 0x2664, 0x2684, 0x26a4, 0x26c4, 0x26e4, 0x2704, 0x2724, 0x2744, 0x2764, 0x2784, 0x27a4, 0x27c4, 0x27e4, 
	0x2805, 0x2825, 0x2845, 0x2865, 0x2885, 0x28a5, 0x28c5, 0x28e5, 0x2905, 0x2925, 0x2945, 0x2965, 0x2985, 0x29a5, 0x29c5, 0x29e5, 
	0x2a05, 0x2a25, 0x2a45, 0x2a65, 0x2a85, 0x2aa5, 0x2ac5, 0x2ae5, 0x2b05, 0x2b25, 0x2b45, 0x2b65, 0x2b85, 0x2ba5, 0x2bc5, 0x2be5, 
	0x2c05, 0x2c25, 0x2c45, 0x2c65, 0x2c85, 0x2ca5, 0x2cc5, 0x2ce5, 0x2d05, 0x2d25, 0x2d45, 0x2d65, 0x2d85, 0x2da5, 0x2dc5, 0x2de5, 
	0x2e05, 0x2e25, 0x2e45, 0x2e65, 0x2e85, 0x2ea5, 0x2ec5, 0x2ee5, 0x2f05, 0x2f25, 0x2f45, 0x2f65, 0x2f85, 0x2fa5, 0x2fc5, 0x2fe5, 
	0x3006, 0x3026, 0x3046, 0x3066, 0x3086, 0x30a6, 0x30c6, 0x30e6, 0x3106, 0x3126, 0x3146, 0x3166, 0x3186, 0x31a6, 0x31c6, 0x31e6, 
	0x3206, 0x3226, 0x3246, 0x3266, 0x3286, 0x32a6, 0x32c6, 0x32e6, 0x3306, 0x3326, 0x3346, 0x3366, 0x3386, 0x33a6, 0x33c6, 0x33e6, 
	0x3406, 0x3426, 0x3446, 0x3466, 0x3486, 0x34a6, 0x34c6, 0x34e6, 0x3506, 0x3526, 0x3546, 0x3566, 0x3586, 0x35a6, 0x35c6, 0x35e6, 
	0x3606, 0x3626, 0x3646, 0x3666, 0x3686, 0x36a6, 0x36c6, 0x36e6, 0x3706, 0x3726, 0x3746, 0x3766, 0x3786, 0x37a6, 0x37c6, 0x37e6, 
	0x3807, 0x3827, 0x3847, 0x3867, 0x3887, 0x38a7, 0x38c7, 0x38e7, 0x3907, 0x3927, 0x3947, 0x3967, 0x3987, 0x39a7, 0x39c7, 0x39e7, 
	0x3a07, 0x3a27, 0x3a47, 0x3a67, 0x3a87, 0x3aa7, 0x3ac7, 0x3ae7, 0x3b07, 0x3b27, 0x3b47, 0x3b67, 0x3b87, 0x3ba7, 0x3bc7, 0x3be7, 
	0x3c07, 0x3c27, 0x3c47, 0x3c67, 0x3c87, 0x3ca7, 0x3cc7, 0x3ce7, 0x3d07, 0x3d27, 0x3d47, 0x3d67, 0x3d87, 0x3da7, 0x3dc7, 0x3de7, 
	0x3e07, 0x3e27, 0x3e47, 0x3e67, 0x3e87, 0x3ea7, 0x3ec7, 0x3ee7, 0x3f07, 0x3f27, 0x3f47, 0x3f67, 0x3f87, 0x3fa7, 0x3fc7, 0x3fe7, 
	0x4008, 0x4028, 0x4048, 0x4068, 0x4088, 0x40a8, 0x40c8, 0x40e8, 0x4108, 0x4128, 0x4148, 0x4168, 0x4188, 0x41a8, 0x41c8, 0x41e8, 
	0x4208, 0x4228, 0x4248, 0x4268, 0x4288, 0x42a8, 0x42c8, 0x42e8, 0x4308, 0x4328, 0x4348, 0x4368, 0x4388, 0x43a8, 0x43c8, 0x43e8, 
	0x4408, 0x4428, 0x4448, 0x4468, 0x4488, 0x44a8, 0x44c8, 0x44e8, 0x4508, 0x4528, 0x4548, 0x4568, 0x4588, 0x45a8, 0x45c8, 0x45e8, 
	0x4608, 0x4628, 0x4648, 0x4668, 0x4688, 0x46a8, 0x46c8, 0x46e8, 0x4708, 0x4728, 0x4748, 0x4768, 0x4788, 0x47a8, 0x47c8, 0x47e8, 
	0x4809, 0x4829, 0x4849, 0x4869, 0x4889, 0x48a9, 0x48c9, 0x48e9, 0x4909, 0x4929, 0x4949, 0x4969, 0x4989, 0x49a9, 0x49c9, 0x49e9, 
	0x4a09, 0x4a29, 0x4a49, 0x4a69, 0x4a89, 0x4aa9, 0x4ac9, 0x4ae9, 0x4b09, 0x4b29, 0x4b49, 0x4b69, 0x4b89, 0x4ba9, 0x4bc9, 0x4be9, 
	0x4c09, 0x4c29, 0x4c49