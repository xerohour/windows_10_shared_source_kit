#include "rectsets.h"
#include <windows.h>

CONST UINT cuiMinSurfaceWidth = 32;
CONST UINT cuiMinSurfaceHeight = 32;

static RECT rectSingleWhole[] = {{0, 0, cuiMinSurfaceWidth, cuiMinSurfaceHeight}};

/*
static RECT rectSingleClipLowerRight[] =
{
	{
		cuiMinSurfaceWidth >> 1,
		cuiMinSurfaceHeight >> 1,
		(cuiMinSurfaceWidth >> 1) + cuiMinSurfaceWidth - 1,
		(cuiMinSurfaceHeight >> 1) + cuiMinSurfaceHeight - 1
	}
};
*/

static RECT rectTwoOverlap[] =
{
	{
		cuiMinSurfaceWidth >> 3,
		cuiMinSurfaceHeight >> 3, 
		((cuiMinSurfaceWidth * 5) >> 3)/* - 1*/,
		((cuiMinSurfaceHeight * 5) >> 3)/* - 1*/
	},
	{
		(cuiMinSurfaceWidth * 3) >> 3,
		(cuiMinSurfaceHeight * 3) >> 3, 
		((cuiMinSurfaceWidth * 7) >> 3)/* - 1*/,
		((cuiMinSurfaceHeight * 7) >> 3)/* - 1*/
	}
};

static RECT rectCentreToUpperLeft[] =
{
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2)/* - 1*/,
		((cuiMinSurfaceHeight * 3) >> 2)/* - 1*/
	}
};
static POINT pointCentreToUpperLeft[] = {{0, 0}};

static RECT rectCentreToTwoOverlap1[] =
{
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2),
		((cuiMinSurfaceHeight * 3) >> 2)
	},
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2),
		((cuiMinSurfaceHeight * 3) >> 2)
	}
};
static POINT pointCentreToTwoOverlap1[] =
{
	{cuiMinSurfaceWidth >> 3, cuiMinSurfaceHeight >> 3},
	{(cuiMinSurfaceWidth * 3) >> 3, (cuiMinSurfaceHeight * 3) >> 3}
};

static RECT rectCentreToTwoOverlap2[] =
{
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2),
		((cuiMinSurfaceHeight * 3) >> 2)
	},
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2),
		((cuiMinSurfaceHeight * 3) >> 2)
	}
};
static POINT pointCentreToTwoOverlap2[] =
{
	{(cuiMinSurfaceWidth * 3) >> 3, (cuiMinSurfaceHeight * 3) >> 3},
	{cuiMinSurfaceWidth >> 3, cuiMinSurfaceHeight >> 3}
};

/*
static RECT rectCentreToLowerRightClipped[] =
{
	{
		cuiMinSurfaceWidth >> 2,
		cuiMinSurfaceHeight >> 2,
		((cuiMinSurfaceWidth * 3) >> 2) - 1,
		((cuiMinSurfaceHeight * 3) >> 2) - 1
	}
};
static POINT pointCentreToLowerRightClipped[] =
{
	{(cuiMinSurfaceWidth * 3) >> 2, (cuiMinSurfaceHeight * 3) >> 2}
};
*/

RECT_SET RectangleSets[] =
{
	{(sizeof(rectSingleWhole)/sizeof(RECT)), rectSingleWhole, NULL, "top left to top left"},
	//{(sizeof(rectSingleClipLowerRight)/sizeof(RECT)), rectSingleClipLowerRight, NULL},
	{(sizeof(rectTwoOverlap)/sizeof(RECT)), rectTwoOverlap, NULL,
	"overlapping but distinct rectangles of the same size; no translation"},
	{(sizeof(rectCentreToUpperLeft)/sizeof(RECT)), rectCentreToUpperLeft, pointCentreToUpperLeft,
	"small, centered rectangle translated to upper left"},
	//{(sizeof(rectCentreToLowerRightClipped)/sizeof(RECT)), rectCentreToLowerRightClipped, pointCentreToLowerRightClipped}
	{(sizeof(rectCentreToTwoOverlap1)/sizeof(RECT)), rectCentreToTwoOverlap1, pointCentreToTwoOverlap1,
	"centred rectangle translated to two overlapping rectangles, upper left then lower right"},
	{(sizeof(rectCentreToTwoOverlap2)/sizeof(RECT)), rectCentreToTwoOverlap2, pointCentreToTwoOverlap2,
	"centred rectangle translated to two overlapping rectangles, lower right then upper left"}
};

CONST UINT cuiNoOfRectSets = sizeof(RectangleSets) / sizeof(RECT_SET);
