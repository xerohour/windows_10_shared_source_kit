#include <d3d8.h>
#include "dx8fmt.h"

D3DFORMAT_RECORD DX8GraphicsFormats[] =
{
	{D3DFMT_R8G8B8, "D3DFMT_R8G8B8", 3, 1},
	{D3DFMT_A8R8G8B8, "D3DFMT_A8R8G8B8", 4, 1},
	{D3DFMT_X8R8G8B8, "D3DFMT_X8R8G8B8", 4, 1},
	{D3DFMT_R5G6B5, "D3DFMT_R5G6B5", 2, 1},
	{D3DFMT_X1R5G5B5, "D3DFMT_X1R5G5B5", 2, 1},
	{D3DFMT_A1R5G5B5, "D3DFMT_A1R5G5B5", 2, 1},
	{D3DFMT_A4R4G4B4, "D3DFMT_A4R4G4B4", 2, 1},
	{D3DFMT_R3G3B2, "D3DFMT_R3G3B2", 1, 1},
	{D3DFMT_A8, "D3DFMT_A8", 1, 1},
	{D3DFMT_A8R3G3B2, "D3DFMT_A8R3G3B2", 2, 1},
	{D3DFMT_X4R4G4B4, "D3DFMT_X4R4G4B4", 2, 1},
	{D3DFMT_A8P8, "D3DFMT_A8P8", 2, 1},
	{D3DFMT_P8, "D3DFMT_P8", 1, 1},
	{D3DFMT_L8, "D3DFMT_L8", 1, 1},
	{D3DFMT_A8L8, "D3DFMT_A8L8", 2, 1},
	{D3DFMT_A4L4, "D3DFMT_A4L4", 1, 1},
	{D3DFMT_V8U8, "D3DFMT_V8U8", 2, 1},
	{D3DFMT_L6V5U5, "D3DFMT_L6V5U5", 2, 1},
	{D3DFMT_X8L8V8U8, "D3DFMT_X8L8V8U8", 4, 1},
	{D3DFMT_UYVY, "D3DFMT_UYVY", 2, 1},
	{D3DFMT_YUY2, "D3DFMT_YUY2", 2, 1},
	{D3DFMT_DXT1, "D3DFMT_DXT1", 8, 4},
	{D3DFMT_DXT2, "D3DFMT_DXT2", 16, 4},
	{D3DFMT_DXT3, "D3DFMT_DXT3", 16, 4},
	{D3DFMT_DXT4, "D3DFMT_DXT4", 16, 4},
	{D3DFMT_DXT5, "D3DFMT_DXT5", 16, 4},
	{D3DFMT_D16, "D3DFMT_D16", 0, 1},
	{D3DFMT_VERTEXDATA, "D3DFMT_VERTEXDATA", 0, 1},
	{D3DFMT_INDEX16, "D3DFMT_INDEX16", 0, 1},
	{D3DFMT_INDEX32, "D3DFMT_INDEX32", 0, 1}
};

CONST UINT cuiNoOfFormats = (sizeof(DX8GraphicsFormats) / sizeof(DX8GraphicsFormats[0]));

const D3DFORMAT_RECORD* FindFormatRecord(D3DFORMAT d3dfmt)
{
	UINT uiFormatRecordIndex;

	for
	(
		uiFormatRecordIndex = 0;
		uiFormatRecordIndex < (sizeof(DX8GraphicsFormats) / sizeof(DX8GraphicsFormats[0]));
		++uiFormatRecordIndex
	)
	{
			if (DX8GraphicsFormats[uiFormatRecordIndex].d3dformat == d3dfmt)
				return &(DX8GraphicsFormats[uiFormatRecordIndex]);
	}

	return NULL;
}
