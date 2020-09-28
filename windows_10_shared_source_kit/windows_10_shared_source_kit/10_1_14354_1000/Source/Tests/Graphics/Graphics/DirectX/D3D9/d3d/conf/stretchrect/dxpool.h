#ifndef MY_DXPOOL_INCLUDED
#define MY_DXPOOL_INCLUDED

#include "FrameworkTypes.h"
//#include "CD3DDef.h"

typedef
struct _D3DPOOL_RECORD
{
	DWORD dwFWPool;
	char *szName;
}
D3DPOOL_RECORD;

extern D3DPOOL_RECORD DXPoolTypes[];
extern CONST UINT cuiNoOfPoolTypes;
extern const D3DPOOL_RECORD* FindPoolRecord(DWORD dwPool);

#endif
