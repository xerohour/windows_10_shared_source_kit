#ifndef DXGI_TEST_D3DREFUTIL_INCLUDED
#define DXGI_TEST_D3DREFUTIL_INCLUDED

#include <windows.h>

#include "WGFTestCore.h"


class clsD3D10Ref
{
	public:
		clsD3D10Ref() {}
		virtual ~clsD3D10Ref();


	public:
		HMODULE GetRefRastModulePointer();


	private:
		static HMODULE hmoduleRefRast;
};

#endif
