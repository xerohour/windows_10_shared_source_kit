#ifndef DXGI_BASIC_D3D10_INCLUDED
#define DXGI_BASIC_D3D10_INCLUDED

#include <d3d10_1.h>
#include <d3dx10.h>

#include "WGFTestCore.h"
#include "WGFTestUI.h"

typedef HRESULT (WGFTESTAPI *PFND3D10CREATEDEVICE)
(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT32 SDKVersion,
	ID3D10Device **ppDevice
);


class CTestBasicD3D10
{
	public:
		CTestBasicD3D10();
		virtual ~CTestBasicD3D10();


	public:
		bool Initialize_D3D10();


	private:
		static HMODULE hmoduleD3D10;


	protected:
		static const char * const szD3D10Path;
		static PFND3D10CREATEDEVICE pfnD3D10CreateDevice;
		static bool bD3D10Loaded;
};

#endif
