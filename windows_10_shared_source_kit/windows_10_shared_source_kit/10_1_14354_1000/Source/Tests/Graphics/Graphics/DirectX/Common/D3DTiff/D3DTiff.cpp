////////////////////////////////////////////////////////////////
// D3DTiff.cpp
//
// Utility for importing/exporting .tiff data to/from D3D resources.
////////////////////////////////////////////////////////////////

#include "D3DTiff.h"
#include "FormatHelper.h"
#include "D3DFormatConverter.h"

#include <Handle.h>

using namespace WindowsTest;

////////////////////////////////////////////////////////////////
// CD3DTiff::WriteData()
//
// Write raw pixel data to .tiff file
////////////////////////////////////////////////////////////////
HRESULT CD3DTiff::WriteData(
	_In_ LPCWSTR pFilename,
	_In_ void *pData, 
	_In_ WICPixelFormatGUID *pTiffFormat, 
	UINT width,
	UINT height,
	UINT rowPitch,
	bool useSRGB
	)
{
	HRESULT hr = S_OK;

	CHandle<IWICImagingFactory> pFactory = nullptr;
	CHandle<IWICBitmapEncoder> pEncoder = nullptr;
	CHandle<IWICBitmapFrameEncode> pBitmapFrame = nullptr;
	CHandle<IWICColorContext> pColorContext = nullptr;
	CHandle<IPropertyBag2> pPropertybag = nullptr;
	CHandle<IWICStream> pStream = nullptr;

	//
	// Initialize TIFF encoder
	//
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*) &pFactory
		);

	if (SUCCEEDED(hr))
	{
		hr = pFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		hr = pStream->InitializeFromFilename(pFilename, GENERIC_WRITE);
	}

	if (SUCCEEDED(hr))
	{
	   hr = pFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &pEncoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr))
	{
		hr = pEncoder->CreateNewFrame(&pBitmapFrame, &pPropertybag);
	}

	if(useSRGB)
	{
		if (SUCCEEDED(hr))
		{
			hr = pFactory->CreateColorContext(&pColorContext);
		}

		if (SUCCEEDED(hr))
		{
			// 1 indicates an sRGB color space
			hr = pColorContext->InitializeFromExifColorSpace(1);
		}
	}

	if (SUCCEEDED(hr))
	{        
		// Specify compression method
		PROPBAG2 option = { 0 };
		option.pstrName = L"TiffCompressionMethod";
		VARIANT varValue;    
		VariantInit(&varValue);
		varValue.vt = VT_UI1;
		varValue.bVal = WICTiffCompressionLZW;      
		hr = pPropertybag->Write(1, &option, &varValue);        
		if (SUCCEEDED(hr))
		{
			hr = pBitmapFrame->Initialize(pPropertybag);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = pBitmapFrame->SetSize(width, height);
	}

	if (SUCCEEDED(hr))
	{
		hr = pBitmapFrame->SetPixelFormat(pTiffFormat);
	}

	//
	// Write pixels
	//
	if (SUCCEEDED(hr))
	{
		UINT bufferSize = height * rowPitch;

		if (pData != NULL)
		{
			hr = pBitmapFrame->WritePixels(height, rowPitch, bufferSize, (BYTE *) pData);
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = pBitmapFrame->Commit();
	}    

	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Commit();
	}

	return hr;
}


////////////////////////////////////////////////////////////////
// CD3DTiff::WriteSubresource()
//
// Write D3D subresource to .tiff file
////////////////////////////////////////////////////////////////
HRESULT CD3DTiff::WriteSubresource(
	_In_ LPCWSTR pFilename,
	_In_ ID3D11Resource *pResource, 
	UINT width,
	UINT height,
	UINT subresource,
	DXGI_FORMAT format
	)
{
	HRESULT hr = S_OK;
	CHandle<ID3D11DeviceContext> pContext = nullptr;
	CHandle<ID3D11Texture2D> pStagingTexture = nullptr;
	CHandle<ID3D11Device> pDevice = nullptr;
	DXGI_FORMAT stagingFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	WICPixelFormatGUID tiffPixelFormat = GUID_WICPixelFormat32bppBGRA;
	bool useSRGB = false;
	
	pResource->GetDevice(&pDevice);

	if(0 == width || 0 == height)
	{
		// Need some dimensions
		return E_INVALIDARG;
	}

	// Make sure the format can be visualized in TIFF
	switch(format)
	{
	default:
		return E_NOTIMPL;
		break;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat64bppRGBA;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA1010102;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA1010102;
		//useSRGB = true;
		break;
	case DXGI_FORMAT_R16G16_UNORM:
		//stagingFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat64bppRGBA;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		//// This conversion is slightly lossy since 32-bit floats only have 23-bits of fractional precision
		//stagingFormat = DXGI_FORMAT_R32_FLOAT; 
		//tiffPixelFormat = GUID_WICPixelFormat32bppGrayFloat;
		break;
	case DXGI_FORMAT_R8G8_UNORM:
		//stagingFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		//stagingFormat = DXGI_FORMAT_R32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat32bppGrayFloat;
		break;
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_A8_UNORM:
		//stagingFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC7_UNORM:
		//stagingFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		//useSRGB = true;
		break;
	case DXGI_FORMAT_B5G6R5_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat16bppBGR565;
		break;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat16bppBGRA5551;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		//stagingFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA1010102XR;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		//useSRGB = true;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		//useSRGB = true;
		//stagingFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		//stagingFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		//tiffPixelFormat = GUID_WICPixelFormat128bppRGBAFloat;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		//tiffPixelFormat = GUID_WICPixelFormat96bppRGBFloat;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat128bppRGBAFloat;
		break;
	case DXGI_FORMAT_R32G32_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat128bppRGBAFloat;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		//stagingFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		//tiffPixelFormat = GUID_WICPixelFormat32bppRGBA;
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat96bppRGBFloat;
		break;
	case DXGI_FORMAT_R16G16_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat96bppRGBFloat;
		break;
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat32bppGrayFloat;
		break;
	case DXGI_FORMAT_R16_FLOAT:
		//stagingFormat = DXGI_FORMAT_R32_FLOAT;
		//tiffPixelFormat = GUID_WICPixelFormat32bppGrayFloat;
		break;
	}

	// Create the staging texture using the specified format
	CD3D11_TEXTURE2D_DESC stagingDesc(
		stagingFormat,
		width,
		height,
		1,
		0,
		0,
		D3D11_USAGE_STAGING,
		stagingFormat == format ? D3D11_CPU_ACCESS_READ : D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
		);

	hr = pDevice->CreateTexture2D(&stagingDesc, NULL, &pStagingTexture);
	if(FAILED(hr))
		return hr;

	pDevice->GetImmediateContext(&pContext);

	// If the source texture format matches the staging format
	// then just do a direct CopySubresourceRegion() directly to the staging texture and save
	// the cost of doing a pixel conversion
	if(stagingFormat == format)
	{
		// Copy the subresource directly to the formatted texture
		pContext->CopySubresourceRegion(pStagingTexture, 0, 0, 0, 0, pResource, subresource, nullptr);
	}
	else
	{
		D3D11_TEXTURE2D_DESC copyDesc;
		CHandle<ID3D11Texture2D> pStagedCopy;

		// Create a cpu-readable texture 2d to copy the texture subresource
		copyDesc.Format = format;
		copyDesc.ArraySize = 1;
		copyDesc.BindFlags = 0;
		copyDesc.MipLevels = 1;
		copyDesc.Usage = D3D11_USAGE_STAGING;
		copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		copyDesc.SampleDesc.Count = 1;
		copyDesc.SampleDesc.Quality = 0;
		copyDesc.MiscFlags = 0;
		copyDesc.Width = width;
		copyDesc.Height = height;

		hr = pDevice->CreateTexture2D(&copyDesc, nullptr, &pStagedCopy);
		if(FAILED(hr))
			return hr;

		// Copy the entire subresource to the staged texture2d
		pContext->CopySubresourceRegion(pStagedCopy, 0, 0, 0, 0, pResource, subresource, nullptr);

		D3D11_MAPPED_SUBRESOURCE mapDataSrc;
		D3D11_MAPPED_SUBRESOURCE mapDataDest;
		hr = pContext->Map(pStagedCopy, 0, D3D11_MAP_READ, 0, &mapDataSrc);
		if(FAILED(hr))
			return hr;
		hr = pContext->Map(pStagingTexture, 0, D3D11_MAP_WRITE, 0, &mapDataDest);
		if(FAILED(hr))
			return hr;

		// Copy converted pixels to pStagingTexture
		BYTE *pDestRow;
		UINT srcPixelSize = CFormatHelper::ElementBytes(copyDesc.Format);
		UINT destPixelSize = CFormatHelper::ElementBytes(stagingDesc.Format);
		pDestRow = reinterpret_cast<BYTE *>(mapDataDest.pData);
		UINT srcWidthInBytes = srcPixelSize * copyDesc.Width;
		UINT destWidthInBytes = destPixelSize * stagingDesc.Width;

		//
		// Convert the data to the target format
		//

		hr = CD3DFormatConverter::Convert2D(
			mapDataSrc.pData,
			mapDataDest.pData,
			format,
			stagingDesc.Format,
			0,
			0,
			copyDesc.Width,
			copyDesc.Height,
			0,
			0,
			mapDataSrc.RowPitch,
			mapDataDest.RowPitch
			);

		pContext->Unmap(pStagedCopy, 0);
		pContext->Unmap(pStagingTexture, 0);

		if(FAILED(hr))
		{
			return hr;
		}
	}

	// Now save the contents of the staging resource to a tiff file
	D3D11_MAPPED_SUBRESOURCE mapData;
	hr = pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mapData);
	if(FAILED(hr))
		return hr;

	hr = WriteData(pFilename, mapData.pData, &tiffPixelFormat, width, height, mapData.RowPitch, useSRGB);

	pContext->Unmap(pStagingTexture, 0);;

	return hr;
}