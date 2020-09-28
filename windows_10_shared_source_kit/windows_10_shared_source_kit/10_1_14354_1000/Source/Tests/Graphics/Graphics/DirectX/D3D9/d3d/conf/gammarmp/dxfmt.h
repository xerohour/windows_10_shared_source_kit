#ifndef MY_DXFMT_INCLUDED

#define MY_DXFMT_INCLUDED

#ifdef BUILD_DX8
#include <d3d8.h>
#elif BUILD_DX9
#include <d3d9.h>
#endif

typedef
struct _D3DFORMAT_RECORD
{
	D3DFORMAT d3dformat;
	char *szName;
}
D3DFORMAT_RECORD;

extern D3DFORMAT_RECORD DXGraphicsFormats[];
extern const char *D3DFormatName(D3DFORMAT d3dfmt);

#endif
