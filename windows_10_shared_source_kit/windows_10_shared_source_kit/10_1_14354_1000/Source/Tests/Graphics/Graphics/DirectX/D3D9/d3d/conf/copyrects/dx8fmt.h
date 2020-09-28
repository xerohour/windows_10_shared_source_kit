#ifndef DX8FMT_INCLUDED

#define DX8FMT_INCLUDED

#include <d3d8.h>

typedef
struct _D3DFORMAT_RECORD
{
	D3DFORMAT d3dformat;
	char *szName;
	UINT uiBytesPerPixel;
	UINT uiCoordinatesPerBlockEdge;
}
D3DFORMAT_RECORD;

extern D3DFORMAT_RECORD DX8GraphicsFormats[];
extern CONST UINT cuiNoOfFormats;
extern const D3DFORMAT_RECORD* FindFormatRecord(D3DFORMAT d3dfmt);

#endif
