#include "rects.h"

// Arrays

#if FAST_TEST
RECT_RECORD SourceRects[] =
{
    // Entire source surface.
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
};

CONST UINT cuiNoOfSourceRects = (UINT)(sizeof(SourceRects) / sizeof(SourceRects[0]));

////////////////////////////////////////////////////////////////////////////////
// Destination Rectangles

RECT_RECORD DestRects[] =
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
};

CONST UINT cuiNoOfDestRects = (UINT)(sizeof(DestRects) / sizeof(DestRects[0]));


RECT_RECORD_PAIR WHQL_Rect_Pairs[] =
{
    {
        // Source Rectangle: Entire Surface
        {
            0, 1, 0,
            0, 1, 0,
            1, 1, 0,
            1, 1, 0
        },
        // Dest Rectangle: Middle
        {
            1, 4, 0,
            1, 4, 0,
            3, 4, 0,
            3, 4, 0
        }
    },
};

const UINT cuiNoOfWHQLRectPairs = (UINT)(sizeof(WHQL_Rect_Pairs) / sizeof(WHQL_Rect_Pairs[0]));

#else //FAST_TEST
////////////////////////////////////////////////////////////////////////////////
// Source Rectangles

RECT_RECORD SourceRects[] =
{
	// Entire source surface.
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
	}
	/*
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
	*/
};

CONST UINT cuiNoOfSourceRects = (UINT)(sizeof(SourceRects) / sizeof(SourceRects[0]));



////////////////////////////////////////////////////////////////////////////////
// Destination Rectangles

RECT_RECORD DestRects[] =
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
	}
	/*
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
	*/
};

CONST UINT cuiNoOfDestRects = (UINT)(sizeof(DestRects) / sizeof(DestRects[0]));


RECT_RECORD_PAIR WHQL_Rect_Pairs[] =
{
	{
		// Source Rectangle: Entire Surface
		{
			0, 1, 0,
			0, 1, 0,
			1, 1, 0,
			1, 1, 0
		},
		// Dest Rectangle: Middle
		{
			1, 4, 0,
			1, 4, 0,
			3, 4, 0,
			3, 4, 0
		}
	},

	{
		// Source Rectangle: Middle
		{
			1, 4, 0,
			1, 4, 0,
			3, 4, 0,
			3, 4, 0
		},
		// Dest Rectangle: Entire Surface
		{
			0, 1, 0,
			0, 1, 0,
			1, 1, 0,
			1, 1, 0
		}
	},

	{
		// Source Rectangle: Subrect, aligned with left edge
		{
			0, 1, 0,
			1, 4, 0,
			3, 4, 0,
			3, 4, 0
		},
		// Dest Rectangle: Subrect, aligned with lower edge
		{
			1, 4, 0,
			1, 4, 0,
			3, 4, 0,
			1, 1, 0
		}
	},

	{
		// Source Rectangle: Subrect, aligned with bottom edge
		{
			1, 4, 0,
			1, 4, 0,
			3, 4, 0,
			1, 1, 0
		},
		// Dest Rectangle: Subrect, aligned with right edge
		{
			1, 4, 0,
			1, 4, 0,
			1, 1, 0,
			3, 4, 0
		}
	}
};

/*

// AndrewLu (07/06/2006): Abandoning this set of rectangle pairs in favor of one that
// is a subset of the rectangle pairs in the non-WHQL test case set.

RECT_RECORD_PAIR WHQL_Rect_Pairs[] =
{
	{
		// Source Rectangle: Entire Surface
		{
			0, 1, 0,
			0, 1, 0,
			1, 1, 0,
			1, 1, 0
		},
		// Dest Rectangle: Compressed vertically and horizontally.
		{
			1, 8, 0,
			1, 4, 0,
			2, 8, 0,
			3, 4, 0
		}
	},

	{
		// Source Rectangle: Subrect
		{
			1, 8, 0,
			1, 4, 0,
			2, 8, 0,
			3, 4, 0
		},
		// Dest Rectangle: Entire Surface (Stretched vertically and horizontally)
		{
			0, 1, 0,
			0, 1, 0,
			1, 1, 0,
			1, 1, 0
		}
	},

	{
		// Source Rectangle: Subrect, aligned upper left
		{
			0, 1, 0,
			0, 1, 0,
			2, 3, 0,
			1, 2, 0
		},
		// Dest Rectangle: Subrect, aligned lower right; Compressed horizontally, stretched vertically
		{
			2, 3, 0,
			1, 4, 0,
			1, 1, 0,
			1, 1, 0
		}
	},

	{
		// Source Rectangle: Subrect, aligned lower right
		{
			2, 3, 0,
			1, 4, 0,
			1, 1, 0,
			1, 1, 0
		},
		// Dest Rectangle: Subrect, aligned upper left; Stretched horizontally, compressed vertically
		{
			0, 1, 0,
			0, 1, 0,
			2, 3, 0,
			1, 2, 0
		}
	}
};
*/

const UINT cuiNoOfWHQLRectPairs = (UINT)(sizeof(WHQL_Rect_Pairs) / sizeof(WHQL_Rect_Pairs[0]));
#endif //FAST_TEST
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

