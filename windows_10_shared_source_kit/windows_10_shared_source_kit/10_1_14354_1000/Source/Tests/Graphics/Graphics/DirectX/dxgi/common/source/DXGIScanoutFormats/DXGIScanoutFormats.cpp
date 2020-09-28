#include "DXGIScanoutFormats.h"

const DXGI_FORMAT clsScanOutFormats::DXGIScanOutFormats[] =
{
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
};

const UINT clsScanOutFormats::cuiNoOfScanOutFormats =
sizeof(clsScanOutFormats::DXGIScanOutFormats) /
sizeof(clsScanOutFormats::DXGIScanOutFormats[0]);

bool clsScanOutFormats::CheckScanOutFormat(DXGI_FORMAT dxgiCheckFormat)
{
	if (cuiNoOfScanOutFormats)
	{
		UINT uiFormatIndex = 0;

		do
		{
			if (dxgiCheckFormat == DXGIScanOutFormats[uiFormatIndex])
				return true;
		}
		while (++uiFormatIndex < cuiNoOfScanOutFormats);
	}

	return false;
}
