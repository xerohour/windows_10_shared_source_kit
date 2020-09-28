#ifndef MY_RECTS_INCLUDED
#define MY_RECTS_INCLUDED

#include <windows.h>

// Each instance of the struct type defines a rectangle by specifying its left and right
// coordinates relative to surface width and its top and bottom coordinates relative to surface
// height.  Each coordinate is specified in terms of a numerator, denominator and constant.

typedef
struct _DEST_RECT_RECORD
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
DEST_RECT_RECORD;

extern DEST_RECT_RECORD DestRects[];
extern CONST UINT cuiNoOfDestRects;

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
*/

//extern SOURCE_LEFT_RECORD SourceLeft[];
//extern SOURCE_TOP_RECORD SourceTop[];
//extern SOURCE_WIDTH_RECORD SourceWidth[];
//extern SOURCE_HEIGHT_RECORD SourceHeight[];
//extern DEST_LEFT_RECORD DestLeft[];
//extern DEST_TOP_RECORD DestTop[];
//extern DEST_WIDTH_RECORD DestWidth[];
//extern DEST_HEIGHT_RECORD DestHeight[];

//extern CONST UINT cuiSourceLeftVals;
//extern CONST UINT cuiSourceTopVals;
//extern CONST UINT cuiSourceWidthVals;
//extern CONST UINT cuiSourceHeightVals;
//extern CONST UINT cuiDestLeftVals;
//extern CONST UINT cuiDestTopVals;
//extern CONST UINT cuiDestWidthVals;
//extern CONST UINT cuiDestHeightVals;

#endif
