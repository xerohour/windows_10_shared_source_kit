#include "rects.h"

#ifdef FAST_TEST
DEST_RECT_RECORD DestRects[] =
{
	// Entire destination surface.
	{
		0, 1, 0,
		0, 1, 0,
		1, 1, 0,
		1, 1, 0
	},
	// Middle
	{
		1, 4, 0,
		1, 4, 0,
		3, 4, 0,
		3, 4, 0
	},
	// Left Edge
	{
		0, 1, 0,
		1, 4, 0,
		3, 4, 0,
		3, 4, 0
	},
	// Horizontal Line
	{
		0, 1, 0,
		1, 2, 0,
		1, 1, 0,
		1, 2, 1
	},
	// Vertical Line
	{
		1, 2, 0,
		0, 1, 0,
		1, 2, 1,
		1, 1, 0
	}
};
#else
DEST_RECT_RECORD DestRects[] =
{
	// Entire destination surface.
	{
		0, 1, 0,
		0, 1, 0,
		1, 1, 0,
		1, 1, 0
	},
	// Middle
	{
		1, 4, 0,
		1, 4, 0,
		3, 4, 0,
		3, 4, 0
	},
	// Left Edge
	{
		0, 1, 0,
		1, 4, 0,
		3, 4, 0,
		3, 4, 0
	},
	// Top Edge
	{
		1, 4, 0,
		0, 1, 0,
		3, 4, 0,
		3, 4, 0
	},
	// Right Edge
	{
		1, 4, 0,
		1, 4, 0,
		1, 1, 0,
		3, 4, 0
	},
	// Bottom Edge
	{
		1, 4, 0,
		1, 4, 0,
		3, 4, 0,
		1, 1, 0
	},
	// Horizontal Line
	{
		0, 1, 0,
		1, 2, 0,
		1, 1, 0,
		1, 2, 1
	},
	// Vertical Line
	{
		1, 2, 0,
		0, 1, 0,
		1, 2, 1,
		1, 1, 0
	}
};
#endif

CONST UINT cuiNoOfDestRects = (UINT)(sizeof(DestRects) / sizeof(DestRects[0]));

/*
SOURCE_LEFT_RECORD SourceLeft[] =
{
//	{-1, 2, 0},
	{0, 1, 0},
	{0, 1, 1},
	{0, 1, 2},
	{0, 1, 3},
	{1, 2, 0}
//	{1, 1, 0}
};
CONST UINT cuiSourceLeftVals = sizeof(SourceLeft) / sizeof(SourceLeft[0]);

SOURCE_TOP_RECORD SourceTop[] =
{
//	{-1, 2, 0},
	{0, 1, 0},
	{1, 2, 0},
//	{1, 1, 0}
};
CONST UINT cuiSourceTopVals = sizeof(SourceTop) / sizeof(SourceTop[0]);

SOURCE_WIDTH_RECORD SourceWidth[] =
{
//	{-1, 2, 0},
//	{0, 1, 0},
//	{0, 1, 1},
	{1, 2, 0}
//	{1, 1, 0},
//	{2, 1, 0}
};
CONST UINT cuiSourceWidthVals = sizeof(SourceWidth) / sizeof(SourceWidth[0]);

SOURCE_HEIGHT_RECORD SourceHeight[] =
{
//	{-1, 2, 0},
//	{0, 1, 0},
//	{0, 1, 1},
	{1, 2, 0}
//	{1, 1, 0},
//	{2, 1, 0}
};
CONST UINT cuiSourceHeightVals = sizeof(SourceHeight) / sizeof(SourceHeight[0]);

DEST_LEFT_RECORD DestLeft[] =
{
//	{-1, 2, 0},
	{0, 1, 0},
	{0, 1, 1},
	{0, 1, 2},
	{0, 1, 3},
	{1, 2, 0}
//	{1, 1, 0}
};
CONST UINT cuiDestLeftVals = sizeof(DestLeft) / sizeof(DestLeft[0]);

DEST_TOP_RECORD DestTop[] =
{
//	{-1, 2, 0},
	{0, 1, 0},
	{1, 2, 0}
//	{1, 1, 0}
};
CONST UINT cuiDestTopVals = sizeof(DestTop) / sizeof(DestTop[0]);

DEST_WIDTH_RECORD DestWidth[] =
{
//	{-1, 2, 0},
//	{0, 1, 0},
//	{0, 1, 1},
//	{1, 3, 0},
	{1, 2, 0},
	{1, 1, 0}
	//{2, 1, 0},
	//{3, 1, 0}
};
CONST UINT cuiDestWidthVals = sizeof(DestWidth) / sizeof(DestWidth[0]);

DEST_HEIGHT_RECORD DestHeight[] =
{
//	{-1, 2, 0},
//	{0, 1, 0},
//	{0, 1, 1},
//	{1, 3, 0},
	{1, 2, 0},
	{1, 1, 0}
	//{2, 1, 0},
	//{3, 1, 0}
};
CONST UINT cuiDestHeightVals = sizeof(DestHeight) / sizeof(DestHeight[0]);
*/
