#ifndef DXGI_TEST_BBDIMS_INCLUDED
#define DXGI_TEST_BBDIMS_INCLUDED

#include <windows.h>

typedef
struct _RELATIVE_BB_DIMS
{
	float fScaleFactorWidth;
	float fScaleFactorHeight;
    float x() const { return fScaleFactorWidth; }
    float y() const { return fScaleFactorHeight; }
} RELATIVE_BB_DIMS;

extern RELATIVE_BB_DIMS WindowRelativeBBDims[];
extern UINT uiNoOfWindowRelativeBBDims;

#endif
