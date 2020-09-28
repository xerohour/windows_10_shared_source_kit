#ifndef DIMS_INCLUDED
#define DIMS_INCLUDED

#include <windows.h>

extern CONST UINT cuiGlobalRTWidth;
extern CONST UINT cuiGlobalRTHeight;

typedef
struct _DIMENSION_RECORD
{
	UINT uiWidth;
	UINT uiHeight;
}
DIMENSION_RECORD;

extern DIMENSION_RECORD SurfaceDimensions[];
extern CONST UINT cuiNoOfDimensions;

#endif
