#ifndef MY_DXFILTER_INCLUDED
#define MY_DXFILTER_INCLUDED

#include "FrameworkTypes.h"
#include <d3dx9.h>
#include <d3d9caps.h>

typedef
struct _D3DTEXTUREFILTERTYPE_RECORD
{
	DWORD dwFWFilter;
	const char *szName;
	DWORD StretchRectCapsBits;
	UINT uiNoOfD3DXFilterTypes;
	CONST DWORD *D3DXFilterTypesArray;
}
D3DTEXTUREFILTERTYPE_RECORD;

extern D3DTEXTUREFILTERTYPE_RECORD DXFilterTypes[];
extern CONST UINT cuiNoOfFilterTypes;

#endif
