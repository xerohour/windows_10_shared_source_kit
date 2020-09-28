#ifndef DXGI_SURF_UTILS_INCLUDED
#define DXGI_SURF_UTILS_INCLUDED

#include "WGFTestCore.h"
#include "WGFTestUI.h"

#include <DXGI.h>


class CTestDXGISurfaceUtils
{
	public:
		CTestDXGISurfaceUtils();
		virtual ~CTestDXGISurfaceUtils();


	private:
		static DXGI_MAPPED_RECT dxgilr;
		static UINT uiBitsPerTexel;
		static UINT uiBytesPerTexel;

		static void InitializeSubrect(RECT rect, BYTE byteTwoBitVal, UINT uiModifiedBitsOffset);


	public:
		virtual bool InitializeLockableSurface(IDXGISurface *pInitSurface);
};

#endif
