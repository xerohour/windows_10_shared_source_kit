#ifndef GETDEVICEINFO9_UTIL_INCLUDED
#define GETDEVICEINFO9_UTIL_INCLUDED

//
// Win32 headers
//

#include <windows.h>
#include <basetsd.h>
#include <tchar.h>


//
// Direct3D headers
//

#include <d3d9.h>
#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// Project headers
//

#include "WGFTestCore.h"
#include "DXGITest.h"


class clsGetDeviceInfo9
:
	public CWGFTestFramework
{
	public:
		clsGetDeviceInfo9();
		clsGetDeviceInfo9( D3D10_DRIVER_TYPE DriverType );

		virtual ~clsGetDeviceInfo9();

	public:
		virtual void InitOptionVariables();
		virtual bool UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo);

		D3D10_DRIVER_TYPE m_DriverType;
		bool m_bBVT;
};

#endif
