#include "dxfmt.h"

#if FAST_TEST
D3DFORMAT_RECORD DXGraphicsFormats[] =
{
    {FMT_A8R8G8B8,		"D3DFMT_A8R8G8B8"},
};

CONST UINT cuiNoOfFormats = (sizeof(DXGraphicsFormats) / sizeof(DXGraphicsFormats[0]));

FMT FormatSubsetSource[] =
{
    FMT_A8R8G8B8,
};

CONST UINT cuiNoOfSourceFormats = (sizeof(FormatSubsetSource) / sizeof(FormatSubsetSource[0]));

FMT FormatSubsetDest[] =
{
    FMT_A8R8G8B8,
};

CONST UINT cuiNoOfDestFormats = (sizeof(FormatSubsetDest) / sizeof(FormatSubsetDest[0]));

FMT FormatSubsetTexture[] =
{
    FMT_A8R8G8B8,
};

CONST UINT cuiNoOfTextureFormats = (sizeof(FormatSubsetTexture) / sizeof(FormatSubsetTexture[0]));

#else //FAST_TEST

D3DFORMAT_RECORD DXGraphicsFormats[] =
{
	{FMT_R8G8B8,		"D3DFMT_R8G8B8"},
	{FMT_A8R8G8B8,		"D3DFMT_A8R8G8B8"},
	{FMT_X8R8G8B8,		"D3DFMT_X8R8G8B8"},
	{FMT_R5G6B5,		"D3DFMT_R5G6B5"},
	{FMT_X1R5G5B5,		"D3DFMT_X1R5G5B5"},
	{FMT_A1R5G5B5,		"D3DFMT_A1R5G5B5"},
	{FMT_A4R4G4B4,		"D3DFMT_A4R4G4B4"},
	{FMT_R3G3B2,		"D3DFMT_R3G3B2"},
	{FMT_A8,			"D3DFMT_A8"},
	{FMT_A8R3G3B2,		"D3DFMT_A8R3G3B2"},
	{FMT_X4R4G4B4,		"D3DFMT_X4R4G4B4"},
	{FMT_A2B10G10R10,	"D3DFMT_A2B10G10R10"},
	{FMT_A8B8G8R8,		"D3DFMT_A8B8G8R8"},
	{FMT_X8B8G8R8,		"D3DFMT_X8B8G8R8"},
	{FMT_G16R16,		"D3DFMT_G16R16"},
	{FMT_A2R10G10B10,	"D3DFMT_A2R10G10B10"},
	{FMT_A16B16G16R16,	"D3DFMT_A16B16G16R16"},
	{FMT_A8P8,			"D3DFMT_A8P8"},
	{FMT_P8,			"D3DFMT_P8"},

	{FMT_L8,			"D3DFMT_L8"},
	{FMT_A8L8,		"D3DFMT_A8L8"},
	{FMT_A4L4,		"D3DFMT_A4L4"},
	{FMT_L16,			"D3DFMT_L16"},

	{FMT_V8U8,		"D3DFMT_V8U8"},
	{FMT_L6V5U5,		"D3DFMT_L6V5U5"},
	{FMT_X8L8V8U8,	"D3DFMT_X8L8V8U8"},
	{FMT_Q8W8V8U8,	"D3DFMT_Q8W8V8U8"},
	{FMT_V16U16,		"D3DFMT_V16U16"},
	{FMT_W11V11U10,	"D3DFMT_W11V11U10"},
	{FMT_W10V11U11,	"D3DFMT_W10V11U11"},
	{FMT_A2W10V10U10,	"D3DFMT_A2W10V10U10"},
	//{FMT_A8X8V8U8,	"D3DFMT_A8X8V8U8"},
	//{FMT_L8X8V8U8,	"D3DFMT_L8X8V8U8"},
	
	{FMT_Q16W16V16U16,"D3DFMT_Q16W16V16U16"},
    
	{FMT_UYVY,			"D3DFMT_UYVY"},
	//{FMT_R8G8_B8G8,		"D3DFMT_R8G8_B8G8"},
	{FMT_YUY2,			"D3DFMT_YUY2"},
	//{FMT_G8R8_G8B8,		"D3DFMT_G8R8_G8B8"},

	//{FMT_DXT1,			"D3DFMT_DXT1"},
	//{FMT_DXT2,			"D3DFMT_DXT2"},
	//{FMT_DXT3,			"D3DFMT_DXT3"},
	//{FMT_DXT4,			"D3DFMT_DXT4"},
	//{FMT_DXT5,			"D3DFMT_DXT5"},

	//{FMT_S1D15,			"D3DFMT_S1D15"},
	//{FMT_S8D24.			"D3DFMT_S8D24"},
	//{FMT_X8D24,			"D3DFMT_X8D24"},
	//{FMT_X4S4D24,		"D3DFMT_X4S4D24"},

	//{FMT_D16_LOCKABLE,"D3DFMT_D16_LOCKABLE"},
	//{FMT_D32,			"D3DFMT_D32"},
	//{FMT_D15S1,		"D3DFMT_D15S1"},
	//{FMT_D24S8,		"D3DFMT_D24S8"},
	//{FMT_D24X8,		"D3DFMT_D24X8"},
	//{FMT_D24X4S4,		"D3DFMT_D24X4S4"},
	//{FMT_D16,			"D3DFMT_D16"},
	//{FMT_D32F_LOCKABLE,"D3DFMT_D32F_LOCKABLE"},
	//{FMT_D24FS8,		"D3DFMT_D24FS8"},
	
	//{FMT_VERTEXDATA,	"D3DFMT_VERTEXDATA"},
	//{FMT_INDEX16,		"D3DFMT_INDEX16"},
	//{FMT_INDEX32,		"D3DFMT_INDEX32"}
    
	{FMT_MULTI2_ARGB8,	"D3DFMT_MULTI2_ARGB8"},

	{FMT_R16F,			"D3DFMT_R16F"},
	{FMT_G16R16F,		"D3DFMT_G16R16F"},
	{FMT_A16B16G16R16F,	"D3DFMT_A16B16G16R16F"},
	{FMT_R32F,			"D3DFMT_R32F"},
	{FMT_G32R32F,		"D3DFMT_G32R32F"},
	{FMT_A32B32G32R32F,	"D3DFMT_A32B32G32R32F"},
	
	{FMT_CxV8U8,		"D3DFMT_CxV8U8"},
    {(FMT) MAKEFOURCC('N', 'V', '1', '1'),		"NV11"},
    {(FMT) MAKEFOURCC('N', 'V', '1', '2'),		"NV12"},
    {(FMT) MAKEFOURCC('Y', '2', '1', '0'),		"Y210"},
    {(FMT) MAKEFOURCC('Y', '2', '1', '6'),		"Y216"},
    {(FMT) MAKEFOURCC('Y', '4', '1', '0'),		"Y410"},
    {(FMT) MAKEFOURCC('Y', '4', '1', '6'),		"Y416"},
    {(FMT) MAKEFOURCC('P', '2', '0', '8'),		"P208"},
    {(FMT) MAKEFOURCC('P', '0', '1', '0'),		"P010"},
    {(FMT) MAKEFOURCC('P', '0', '1', '6'),		"P016"}

};

CONST UINT cuiNoOfFormats = (sizeof(DXGraphicsFormats) / sizeof(DXGraphicsFormats[0]));

FMT FormatSubsetSource[] =
{
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_R5G6B5,
    FMT_YUY2,
    FMT_UYVY,
    (FMT) MAKEFOURCC('N', 'V', '1', '1'),
    (FMT) MAKEFOURCC('Y', '2', '1', '0'),
    (FMT) MAKEFOURCC('P', '2', '0', '8')
    
    //
    // NV12 passes CheckDeviceFormat. But fails D3DXLoadSurfaceFromSurface.  Need to implement a 
    //LoadSurfaceFromSurface before adding this test 
    //

    //(FMT) MAKEFOURCC('N', 'V', '1', '2'),
    
};

CONST UINT cuiNoOfSourceFormats = (sizeof(FormatSubsetSource) / sizeof(FormatSubsetSource[0]));

FMT FormatSubsetDest[] =
{
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_R5G6B5
};

CONST UINT cuiNoOfDestFormats = (sizeof(FormatSubsetDest) / sizeof(FormatSubsetDest[0]));


FMT FormatSubsetTexture[] =
{
	FMT_A8R8G8B8,
	FMT_X8R8G8B8,
	FMT_R5G6B5
};

CONST UINT cuiNoOfTextureFormats = (sizeof(FormatSubsetTexture) / sizeof(FormatSubsetTexture[0]));

#endif //FAST_TEST

const D3DFORMAT_RECORD* FindFormatRecord(FMT fmt)
{
	UINT uiFormatRecordIndex;

	for
	(
		uiFormatRecordIndex = 0;
		uiFormatRecordIndex < cuiNoOfFormats;
		++uiFormatRecordIndex
	)
	{
			if (DXGraphicsFormats[uiFormatRecordIndex].fmtFW == fmt)
				return &(DXGraphicsFormats[uiFormatRecordIndex]);
	}

	return NULL;
}

bool FindFormatRecordIndex(FMT fmt, UINT *puiIndex)
{
	UINT uiFormatRecordIndex;

	if (!cuiNoOfFormats)
		return false;

	if (DXGraphicsFormats[(uiFormatRecordIndex = 0)].fmtFW != fmt)
		do
		{
			if (++uiFormatRecordIndex == cuiNoOfFormats)
				return false;
		}
		while (DXGraphicsFormats[uiFormatRecordIndex].fmtFW != fmt);

	*puiIndex = uiFormatRecordIndex;
	return true;
}
