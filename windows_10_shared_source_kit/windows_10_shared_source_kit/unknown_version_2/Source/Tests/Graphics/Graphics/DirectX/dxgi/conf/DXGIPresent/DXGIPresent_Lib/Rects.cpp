#include "Rects.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions of the sets of dirty rects:

static RelativeRect DirtySet000[] =
{
	{0.125, 0.25, 0.5, 0.5},
	{0.25, 0.375, 0.75, 0.75},
	{0.5, 0.5, 0.625, 0.625}
};

static const UINT cuiNoOfRects000 = sizeof(DirtySet000) / sizeof(DirtySet000[0]);

DirtyRectsSet TestedDirtyRectSets[] =
{
	{NULL, 0},
	{DirtySet000, cuiNoOfRects000}
};

const UINT cuiNoOfTestedDirtyRectSets = sizeof(TestedDirtyRectSets) / sizeof(TestedDirtyRectSets[0]);

////////////////////////////////////////////////////////////////////////////////
// Definitions of the source rects:

RelativeRect TestedSourceRects[] =
{
	{0.0, 0.0, 1.0, 1.0},
	{0.0, 0.0, 0.875, 0.875},
	{0.125, 0.125, 1.0, 1.0}
};

const UINT cuiNoOfTestedSourceRects = sizeof(TestedSourceRects) / sizeof(TestedSourceRects[0]);

////////////////////////////////////////////////////////////////////////////////
// Definitions of the destination rects:

RelativeRect TestedDestRects[] =
{
	{0.0, 0.0, 1.0, 1.0},
	{0.375, 0.375, 0.625, 0.625}
};

const UINT cuiNoOfTestedDestRects = sizeof(TestedDestRects) / sizeof(TestedDestRects[0]);
