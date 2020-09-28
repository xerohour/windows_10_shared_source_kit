#ifndef UTIL_FORMATS_INCLUDED
#define UTIL_FORMATS_INCLUDED

#include <windows.h>
#include "cd3ddef.h"

// Notwithstanding YUV and DXTn formats, a locked region of any surface can be updated
// using a simple row-wise update, if by "row" is meant "a set of texels represented by
// consecutive bytes in memory."  For most formats, this is a row of horizontally
// adjacent texels.  For DXTn formats, it is a row of horizontally adjacent 4x4 "tiles".

// The number of bytes in such a row is proportional to the number of
// "horizontal coordinates".  For most formats, the factor is just
// the number of bytes per texel.  For DXTn formats, the factor is
// a quarter of the number of bytes used to represent each 4x4 tile.

// This function returns that factor multiplied by 8.  Thus, multiplying the return
// value by the number of horizontal coordinates in a row yields the total number of bits
// in the row.

extern UINT GetBitsPerHorizontalCoordinate(FORMAT fmtFrameworkFormat);

extern BOOL IsDXTnFormat(D3DFORMAT Format);

extern BOOL IsUYVYFormat(D3DFORMAT Format);

typedef enum _RENDERCATEGORY{rcMipMap, rcLuminance, rcBumpMap, rcBumpLuminance, rcMultiElement} RENDERCATEGORY;
extern RENDERCATEGORY GetFormatRenderCategory(D3DFORMAT Format);

#endif
