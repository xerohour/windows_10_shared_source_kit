#include "D3D10RefUtil.h"

HMODULE clsD3D10Ref::hmoduleRefRast = NULL;

#ifndef D3D_REF_DLLNAME
#ifdef FORCE_D3D10_REF
#define D3D_REF_DLLNAME "d3d10ref.dll"
#else
#define D3D_REF_DLLNAME "d3d11ref.dll"
#endif
#endif

clsD3D10Ref::~clsD3D10Ref()
{
}

HMODULE clsD3D10Ref::GetRefRastModulePointer()
{
	if (!hmoduleRefRast)
		if (!(hmoduleRefRast = LoadLibraryEx(D3D_REF_DLLNAME, NULL, 0)))
			GetFramework()->WriteToLog(_T ( "UpdateCurrentDeviceInfo:  Failed to load" D3D_REF_DLLNAME));

	return hmoduleRefRast;
}
