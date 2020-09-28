#ifndef MY_DXFMT_INCLUDED

#define MY_DXFMT_INCLUDED

#include "FrameworkTypes.h"
//#include "CD3DDef.h"

typedef
struct _D3DFORMAT_RECORD
{
	FMT fmtFW;
	char *szName;
}
D3DFORMAT_RECORD;

extern D3DFORMAT_RECORD DXGraphicsFormats[];
extern CONST UINT cuiNoOfFormats;
extern const D3DFORMAT_RECORD* FindFormatRecord(FMT fmt);

#endif
