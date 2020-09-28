#include "colors.h"

#ifdef FAST_TEST
DWORD dwColors[] =
{
	0x00ffffff, // White, with highest order byte blank
	0xffffffff, // White, with all bits of highest order byte set
	0x0000ffff, // Cyan
	0x00ff00ff, // Magenta
};
#else
DWORD dwColors[] =
{
	0x00ffffff, // White, with highest order byte blank
	0xffffffff, // White, with all bits of highest order byte set
	0x00ff0000, // Red
	0x0000ff00, // Green
	0x000000ff, // Blue
	0x0000ffff, // Cyan
	0x00ff00ff, // Magenta
	0x00ffff00, // Yellow
	0x009752DA  // "Random"
};
#endif


CONST UINT cuiNoOfColors = (UINT)(sizeof(dwColors) / sizeof(dwColors[0]));
