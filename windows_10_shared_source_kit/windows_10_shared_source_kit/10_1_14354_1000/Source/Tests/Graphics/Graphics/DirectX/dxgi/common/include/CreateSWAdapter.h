#ifndef DXGI_TEST_DXGI_ENUMMODES
#define DXGI_TEST_DXGI_ENUMMODES

#include <windows.h>
#include <dxgi.h>
#include <d3d9.h>
#include <d3d10_1.h>
#include <d3dx10.h>
#include <map>

#include "WGFTestCore.h"
#include "DXGITestFramework.h"

#define  WARP10_ADAPTER                      3
#define  D3D10_REF_ADAPTER                   103

#define  WARP10_RASTERIZER_NAME              "d3d10warp.dll"
#define  D3D10_REF_RASTERIZER_NAME           "d3d10ref.dll"

class clsDXGICreateSWAdapter
{
	public:
		clsDXGICreateSWAdapter();
		~clsDXGICreateSWAdapter();
        
        HRESULT      CreateSWAdapter(__in USHORT ushSWRasterizerVer);

	private:
        IDXGIAdapter    *m_pSWAdapter;
};
#endif
