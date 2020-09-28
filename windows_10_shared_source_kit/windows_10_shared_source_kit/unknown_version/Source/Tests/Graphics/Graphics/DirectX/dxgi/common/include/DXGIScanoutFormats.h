#ifndef DXGI_COMMON_TEST_SCANOUT_FORMATS
#define DXGI_COMMON_TEST_SCANOUT_FORMATS

#include <windows.h>
#include <dxgi.h>

class clsScanOutFormats
{
public:
	static const DXGI_FORMAT DXGIScanOutFormats[];
	static const UINT cuiNoOfScanOutFormats;
	static bool CheckScanOutFormat(DXGI_FORMAT dxgiCheckFormat);
};

#endif
