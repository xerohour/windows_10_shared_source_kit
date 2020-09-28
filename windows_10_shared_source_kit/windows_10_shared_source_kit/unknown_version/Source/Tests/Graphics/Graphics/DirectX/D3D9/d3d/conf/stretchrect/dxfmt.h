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

extern FMT FormatSubsetSource[];
extern CONST UINT cuiNoOfSourceFormats;
extern FMT FormatSubsetDest[];
extern CONST UINT cuiNoOfDestFormats;
extern FMT FormatSubsetTexture[];
extern CONST UINT cuiNoOfTextureFormats;

extern const D3DFORMAT_RECORD* FindFormatRecord(FMT fmt);
extern bool FindFormatRecordIndex(FMT fmt, UINT *puiIndex);

#endif
