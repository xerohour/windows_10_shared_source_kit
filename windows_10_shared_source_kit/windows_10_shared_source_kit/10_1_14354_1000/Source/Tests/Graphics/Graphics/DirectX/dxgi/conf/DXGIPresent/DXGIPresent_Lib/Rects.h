#ifndef DXGI_PRESENT_TEST_RECTS_INCLUDED
#define DXGI_PRESENT_TEST_RECTS_INCLUDED

#include <windows.h>

typedef
struct _RelativeRect
{
	float left;
	float top;
	float right;
	float bottom;
}
RelativeRect;

typedef
struct _DirtyRectsSet
{
	const RelativeRect * const pRectArray;
	const UINT uiNoOfRects;
}
DirtyRectsSet;

extern DirtyRectsSet TestedDirtyRectSets[];
extern const UINT cuiNoOfTestedDirtyRectSets;
extern RelativeRect TestedSourceRects[];
extern const UINT cuiNoOfTestedSourceRects;
extern RelativeRect TestedDestRects[];
extern const UINT cuiNoOfTestedDestRects;

#endif
