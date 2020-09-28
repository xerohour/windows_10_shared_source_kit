#ifndef DX8MULTISAMPLE_INCLUDED

#define DX8MULTISAMPLE_INCLUDED

#include <d3d8.h>

typedef
struct _D3DMULTISAMPLE_TYPE_RECORD
{
	D3DMULTISAMPLE_TYPE d3dmst;
	char *szName;
}
D3DMULTISAMPLE_TYPE_RECORD;

extern D3DMULTISAMPLE_TYPE_RECORD DX8MultiSampleTypes[];
extern CONST UINT cuiNoOfMultiSampleTypes;
extern const D3DMULTISAMPLE_TYPE_RECORD* FindMultiSampleRecord(D3DMULTISAMPLE_TYPE d3dmst);

#endif
