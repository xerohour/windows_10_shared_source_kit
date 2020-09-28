#ifndef MY_RECTS_INCLUDED
#define MY_RECTS_INCLUDED

#include <windows.h>

typedef
struct _RECT_RECORD
{
	INT iLeftNumerator;
	INT iLeftDenominator;
	INT iLeftConstant;

	INT iTopNumerator;
	INT iTopDenominator;
	INT iTopConstant;

	INT iRightNumerator;
	INT iRightDenominator;
	INT iRightConstant;

	INT iBottomNumerator;
	INT iBottomDenominator;
	INT iBottomConstant;
}
RECT_RECORD;

extern RECT_RECORD SourceRects[];
extern CONST UINT cuiNoOfSourceRects;
extern RECT_RECORD DestRects[];
extern CONST UINT cuiNoOfDestRects;

typedef
struct _RECT_RECORD_PAIR
{
	RECT_RECORD SourceRect;
	RECT_RECORD DestRect;
}
RECT_RECORD_PAIR;

extern RECT_RECORD_PAIR WHQL_Rect_Pairs[];
extern const UINT cuiNoOfWHQLRectPairs;

/*
typedef
struct _SOURCE_LEFT_RECORD
{
	INT iSurfaceWidthFactorNumerator;
	INT iSurfaceWidthFactorDenominator;
	INT iConstant;
}
SOURCE_LEFT_RECORD;

typedef
struct _SOURCE_TOP_RECORD
{
	INT iSurfaceHeightFactorNumerator;
	INT iSurfaceHeightFactorDenominator;
	INT iConstant;
}
SOURCE_TOP_RECORD;

typedef
struct _SOURCE_WIDTH_RECORD
{
	INT iSurfaceWidthFactorNumerator;
	INT iSurfaceWidthFactorDenominator;
	INT iConstant;
}
SOURCE_WIDTH_RECORD;

typedef
struct _SOURCE_HEIGHT_RECORD
{
	INT iSurfaceHeightFactorNumerator;
	INT iSurfaceHeightFactorDenominator;
	INT iConstant;
}
SOURCE_HEIGHT_RECORD;

typedef
struct _DEST_LEFT_RECORD
{
	INT iSurfaceWidthFactorNumerator;
	INT iSurfaceWidthFactorDenominator;
	INT iConstant;
}
DEST_LEFT_RECORD;

typedef
struct _DEST_TOP_RECORD
{
	INT iSurfaceHeightFactorNumerator;
	INT iSurfaceHeightFactorDenominator;
	INT iConstant;
}
DEST_TOP_RECORD;

typedef
struct _DEST_WIDTH_RECORD
{
	INT iSurfaceWidthFactorNumerator;
	INT iSurfaceWidthFactorDenominator;
	INT iConstant;
}
DEST_WIDTH_RECORD;

typedef
struct _DEST_HEIGHT_RECORD
{
	INT iSurfaceHeightFactorNumerator;
	INT iSurfaceHeightFactorDenominator;
	INT iConstant;
}
DEST_HEIGHT_RECORD;

extern SOURCE_LEFT_RECORD SourceLeft[];
extern SOURCE_TOP_RECORD SourceTop[];
extern SOURCE_WIDTH_RECORD SourceWidth[];
extern SOURCE_HEIGHT_RECORD SourceHeight[];
extern DEST_LEFT_RECORD DestLeft[];
extern DEST_TOP_RECORD DestTop[];
extern DEST_WIDTH_RECORD DestWidth[];
extern DEST_HEIGHT_RECORD DestHeight[];

extern CONST UINT cuiSourceLeftVals;
extern CONST UINT cuiSourceTopVals;
extern CONST UINT cuiSourceWidthVals;
extern CONST UINT cuiSourceHeightVals;
extern CONST UINT cuiDestLeftVals;
extern CONST UINT cuiDestTopVals;
extern CONST UINT cuiDestWidthVals;
extern CONST UINT cuiDestHeightVals;
*/

#endif
