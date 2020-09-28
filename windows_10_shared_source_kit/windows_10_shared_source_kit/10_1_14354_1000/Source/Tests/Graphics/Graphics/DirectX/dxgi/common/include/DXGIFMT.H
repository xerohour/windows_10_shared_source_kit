#ifndef DXGIFMT_H_INCLUDED
#define DXGIFMT_H_INCLUDED

#include "dxgi.h"

typedef struct _DXGI_FORMAT_INFO
{
	DXGI_FORMAT dxgiformat;
	UINT uiEnumValue;
	UINT uiBPP;
}
DXGI_FORMAT_INFO;

extern DXGI_FORMAT_INFO DXGIFormats[];
extern CONST UINT cuiNoOfFormats;

extern CONST DXGI_FORMAT_INFO* FindFormatRecord(DXGI_FORMAT dxgiformatSearch);

extern DXGI_FORMAT DXGI_Formats_From_Source[];
extern const UINT cuiNoOfFormatsFromSource;

#endif
