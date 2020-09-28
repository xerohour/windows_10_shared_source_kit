#include "UtilFormats.h"

UINT GetBitsPerHorizontalCoordinate(FORMAT fmtFrameworkFormat)
{
	UINT uiBitsPerHorizontalCoordinate;

	switch ((FMT)(fmtFrameworkFormat.d3dfFormat))
	{
		// Current framework code (see the d3df2ddpf function in
		// testutils.cpp) does not set dwRGBBitCount for any of the
		// following formats.
	case FMT_UYVY:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	case FMT_R8G8_B8G8:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	case FMT_YUY2:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	case FMT_G8R8_G8B8:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	case FMT_DXT1:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	case FMT_DXT2:
		uiBitsPerHorizontalCoordinate = 32;
		break;
	case FMT_DXT3:
		uiBitsPerHorizontalCoordinate = 32;
		break;
	case FMT_DXT4:
		uiBitsPerHorizontalCoordinate = 32;
		break;
	case FMT_DXT5:
		uiBitsPerHorizontalCoordinate = 32;
		break;
	case FMT_V8U8:
		uiBitsPerHorizontalCoordinate = 16;
		break;
	default:
		uiBitsPerHorizontalCoordinate =
		(fmtFrameworkFormat.ddpfPixelFormat).dwRGBBitCount;
		break;
	}

	return uiBitsPerHorizontalCoordinate;
}

BOOL IsDXTnFormat(D3DFORMAT Format)
{
	return
	(
		((FMT)Format == FMT_DXT1) ||
		((FMT)Format == FMT_DXT2) ||
		((FMT)Format == FMT_DXT3) ||
		((FMT)Format == FMT_DXT4) ||
		((FMT)Format == FMT_DXT5)
	);
}

BOOL IsUYVYFormat(D3DFORMAT Format)
{
	return
	(
		((FMT)Format == FMT_UYVY) ||
		((FMT)Format == FMT_YUY2) ||
		((FMT)Format == FMT_R8G8_B8G8) ||
		((FMT)Format == FMT_G8R8_G8B8)
	);
}

RENDERCATEGORY GetFormatRenderCategory(D3DFORMAT Format)
{
	RENDERCATEGORY rcReturnVal;

	rcReturnVal = rcMipMap;

	switch ((FMT)Format)
	{
	case FMT_L8:
	case FMT_L16:
	case FMT_A8L8:
	case FMT_A4L4:
		rcReturnVal = rcLuminance;
		break;
	case FMT_V8U8:
	case FMT_Q8W8V8U8:
	case FMT_V16U16:
	case FMT_Q16W16V16U16:
		rcReturnVal = rcBumpMap;
		break;
	case FMT_L6V5U5:
	case FMT_X8L8V8U8:
		rcReturnVal = rcBumpLuminance;
		break;
	case FMT_MULTI2_ARGB8:
		rcReturnVal = rcMultiElement;
		break;
	}

	return rcReturnVal;
}
