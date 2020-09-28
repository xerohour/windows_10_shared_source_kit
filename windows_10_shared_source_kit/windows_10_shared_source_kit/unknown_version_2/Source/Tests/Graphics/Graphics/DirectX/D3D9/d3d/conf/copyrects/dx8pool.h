#ifndef DX8POOL_INCLUDED

#define DX8POOL_INCLUDED

#include <d3d8.h>

typedef
struct _D3DPOOL_RECORD
{
	D3DPOOL d3dpool;
	char *szName;
}
D3DPOOL_RECORD;

extern D3DPOOL_RECORD DX8PoolTypes[];
extern CONST UINT cuiNoOfPoolTypes;
extern const D3DPOOL_RECORD* FindPoolRecord(D3DPOOL d3dpool);

#endif
