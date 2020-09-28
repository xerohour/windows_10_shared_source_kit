#ifndef RECTSETS_INCLUDED
#define RECTSETS_INCLUDED

#include <windows.h>

extern CONST UINT cuiMinSurfaceWidth;
extern CONST UINT cuiMinSurfaceHeight;
extern CONST UINT cuiNoOfRectSets;

typedef
struct _RECT_SET
{
	int iNoOfRects;
	RECT *rects;
	POINT *points;
	char *szDescription;
}
RECT_SET;

extern RECT_SET RectangleSets[];

#endif
