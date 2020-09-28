#ifndef MY_DXMULTISAMPLE_INCLUDED

#define MY_DXMULTISAMPLE_INCLUDED

#include "FrameworkTypes.h"
//#include "CD3DDef.h"

typedef
struct _D3DMULTISAMPLE_TYPE_RECORD
{
	MULTISAMPLE_TYPE mstFW;
	char *szName;
}
D3DMULTISAMPLE_TYPE_RECORD;

extern D3DMULTISAMPLE_TYPE_RECORD DXMultiSampleTypes[];
extern CONST UINT cuiNoOfMultiSampleTypes;
extern const D3DMULTISAMPLE_TYPE_RECORD* FindMultiSampleRecord(MULTISAMPLE_TYPE mst);

#endif

