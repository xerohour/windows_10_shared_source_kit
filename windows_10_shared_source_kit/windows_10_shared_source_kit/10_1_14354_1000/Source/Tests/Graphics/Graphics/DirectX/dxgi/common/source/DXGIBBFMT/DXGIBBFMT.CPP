#include "DXGIBBFMT.h"

DXGI_FORMAT clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[] =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
};

UINT clsDXGIRenderTargetFormats::uiNoOfBBFormats =
sizeof(clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats) /
sizeof(clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[0]);


//
// Flip Model requires a separate list of back buffer formats.
// Even this list varies based on Feature Level.
//
DXGI_FORMAT clsDXGIFlipModelRenderTargetFormats::DXGI_Back_Buffer_Formats[] =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT, // Not supported via FL9.
	DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,     
    DXGI_FORMAT_B8G8R8X8_UNORM,
};

UINT clsDXGIFlipModelRenderTargetFormats::uiNoOfBBFormats =
sizeof(clsDXGIFlipModelRenderTargetFormats::DXGI_Back_Buffer_Formats) /
sizeof(clsDXGIFlipModelRenderTargetFormats::DXGI_Back_Buffer_Formats[0]);
