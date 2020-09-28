////////////////////////////////////////////////////////////////
// D3DFormatConverter.H
//
// Contains class CD3DFormatConverter used for converting between DXGI pixel data and raw
// floating point (32 or 64 bit)
////////////////////////////////////////////////////////////////

#pragma once

#include <d3d11.h>
#include <vector>

#include <FormatHelper.h>

////////////////////////////////////////////////////////////////
// class CD3DFormatConverter
//
// Methods used for converting pixels between DXGI pixel data and raw
// floating point (32 or 64 bit)
////////////////////////////////////////////////////////////////
class CD3DFormatConverter
{
public:
	////////////////////////////////////////////////////////////////
	// ConvertToFloat64()
	//
	// Converts resource data from DXGI pixel format to DOUBLE_COLOR.
	////////////////////////////////////////////////////////////////
	static HRESULT ConvertToFloat64(
		_Out_writes_(floatBufferElementCount) DOUBLE_COLOR *pFloatColorBuffer, 
		size_t floatBufferElementCount, // Number of FLOAT_COLOR elements pointed to by pFloatColorBuffer
		_In_ void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of source region
		UINT top,			// Y-coordinate of top row of source region
		UINT right,			// X-coordinate of one-past-the-right column of source region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of source region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in source image (must be greater-than-or-equal-to image width or 0 if source is 1D)
		UINT front,			// Z-coordinate of first slice of source region (for 3-dimensional resource)
		UINT back,			// Z-coordinate of one-past-the-last slice of source region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional source (can be 0 if source image is 1D or 2D)
		);

	////////////////////////////////////////////////////////////////
	// ConvertFromFloat64()
	//
	// Converts resource data from DOUBLE_COLOR to DXGI pixel format.
	////////////////////////////////////////////////////////////////
	static HRESULT ConvertFromFloat64(
		_In_ DOUBLE_COLOR *pFloatColorBuffer, 
		_Out_ void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of dest region
		UINT top,			// Y-coordinate of top row of dest region
		UINT right,			// X-coordinate of one-past-the-right column of dest region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of dest region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in dest image (must be greater-than-or-equal-to image width or 0 if dest is 1D)
		UINT front,			// Z-coordinate of first slice of dest region (for 3-dimensional redest)
		UINT back,			// Z-coordinate of one-past-the-last slice of dest region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional dest (can be 0 if dest image is 1D or 2D)
		);

	////////////////////////////////////////////////////////////////
	// ConvertToFloat32()
	//
	// Converts resource data from DXGI pixel format to FLOAT_COLOR.
	////////////////////////////////////////////////////////////////
	static HRESULT ConvertToFloat32(
		_Out_writes_(floatBufferElementCount) FLOAT_COLOR *pFloatColorBuffer, 
		size_t floatBufferElementCount, // Number of FLOAT_COLOR elements pointed to by pFloatColorBuffer
		_In_ void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of source region
		UINT top,			// Y-coordinate of top row of source region
		UINT right,			// X-coordinate of one-past-the-right column of source region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of source region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in source image (must be greater-than-or-equal-to image width or 0 if source is 1D)
		UINT front,			// Z-coordinate of first slice of source region (for 3-dimensional resource)
		UINT back,			// Z-coordinate of one-past-the-last slice of source region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional source (can be 0 if source image is 1D or 2D)
		);

	////////////////////////////////////////////////////////////////
	// ConvertFromFloat32()
	//
	// Converts resource data from FLOAT_COLOR to DXGI pixel format.
	////////////////////////////////////////////////////////////////
	static HRESULT ConvertFromFloat32(
		_In_ FLOAT_COLOR *pFloatColorBuffer, 
		_Out_ void *pData, 
		DXGI_FORMAT format,
		UINT left,			// X-coordinate of left column of dest region
		UINT top,			// Y-coordinate of top row of dest region
		UINT right,			// X-coordinate of one-past-the-right column of dest region (must not be larger than image width)
		UINT bottom,		// Y-coordinate of one-past-the-bottom row of dest region (must not be larger than image height)
		UINT rowPitch,		// Memory pitch in bytes between pixel rows in dest image (must be greater-than-or-equal-to image width or 0 if dest is 1D)
		UINT front,			// Z-coordinate of first slice of dest region (for 3-dimensional redest)
		UINT back,			// Z-coordinate of one-past-the-last slice of dest region
		UINT slicePitch		// Memory pitch between slices in 3-dimensional dest (can be 0 if dest image is 1D or 2D)
		);

	////////////////////////////////////////////////////////////////
	// Convert2D
	//
	// General 2D data conversion from one format to another
	////////////////////////////////////////////////////////////////
	static HRESULT Convert2D(
		_In_ void *pSrcData,
		_Out_ void *pDestData,
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
		);

	////////////////////////////////////////////////////////////////
	// GetNV12PixelYUV
	//
	// Get pixel YUV from a NV12 texture data
	////////////////////////////////////////////////////////////////
    static HRESULT GetNV12PixelYUV(
        BYTE *pData,
        UINT Width,
        UINT Height,
        UINT rowPitch,
        UINT xPos,
        UINT yPos,
        FLOAT_COLOR* oYUV);

};


