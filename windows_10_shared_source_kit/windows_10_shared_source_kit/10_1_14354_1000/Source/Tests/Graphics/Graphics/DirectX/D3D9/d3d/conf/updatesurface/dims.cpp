#include "dims.h"

CONST UINT cuiGlobalRTWidth = 64;
CONST UINT cuiGlobalRTHeight = 64;

DIMENSION_RECORD SurfaceDimensions[] =
{
	//{32, 32},
	{64, 64}
};

CONST UINT cuiNoOfDimensions = (sizeof(SurfaceDimensions) / sizeof(SurfaceDimensions[0]));
