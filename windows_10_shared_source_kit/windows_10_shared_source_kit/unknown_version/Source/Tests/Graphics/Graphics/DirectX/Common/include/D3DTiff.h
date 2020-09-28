////////////////////////////////////////////////////////////////
// D3DTiff.h
//
// Utility for importing/exporting .tiff data to/from D3D resources.
////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11_2.h"

#include <wincodec.h>

class CD3DTiff
{
public:
	////////////////////////////////////////////////////////////////
	// WriteData()
	//
	// Write raw pixel data to .tiff file
	////////////////////////////////////////////////////////////////
	static HRESULT WriteData(
		_In_ LPCWSTR pFilename,
		_In_ void *pData, 
		_In_ WICPixelFormatGUID *pTiffFormat, 
		UINT width,
		UINT height,
		UINT rowPitch,
		bool useSRGB
		);

	////////////////////////////////////////////////////////////////
	// WriteSubresource()
	//
	// Write D3D subresource to .tiff file
	////////////////////////////////////////////////////////////////
	static HRESULT WriteSubresource(
		_In_ LPCWSTR pFilename,
		_In_ ID3D11Resource *pResource, 
		UINT width,
		UINT height,
		UINT subresource,
		DXGI_FORMAT format
		);

	////////////////////////////////////////////////////////////////
	// Read .tiff data from file
	////////////////////////////////////////////////////////////////
	static HRESULT Read(
		_In_ LPWSTR pFilename,
		_Out_ void *pData,
		UINT rowPitch
		);
		
};

