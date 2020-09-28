#include <d3d8.h>
#include "dx8pool.h"

D3DPOOL_RECORD DX8PoolTypes[] =
{
	{D3DPOOL_DEFAULT, "D3DPOOL_DEFAULT"},
	{D3DPOOL_MANAGED, "D3DPOOL_MANAGED"},
	{D3DPOOL_SYSTEMMEM, "D3DPOOL_SYSTEMMEM"}
	//{D3DPOOL_SCRATCH, "D3DPOOL_SCRATCH"}
};

CONST UINT cuiNoOfPoolTypes = (sizeof(DX8PoolTypes) / sizeof(DX8PoolTypes[0]));

const D3DPOOL_RECORD* FindPoolRecord(D3DPOOL d3dpool)
{
	UINT uiPoolRecordIndex;

	for
	(
		uiPoolRecordIndex = 0;
		uiPoolRecordIndex < (sizeof(DX8PoolTypes) / sizeof(DX8PoolTypes[0]));
		++uiPoolRecordIndex
	)
	{
			if (DX8PoolTypes[uiPoolRecordIndex].d3dpool == d3dpool)
				return &(DX8PoolTypes[uiPoolRecordIndex]);
	}

	return NULL;
}
