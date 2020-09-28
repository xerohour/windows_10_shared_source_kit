#ifndef DIMS_INCLUDED
#define DIMS_INCLUDED

#include <windows.h>

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
