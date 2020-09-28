#include "dxpool.h"

D3DPOOL_RECORD DXPoolTypes[] =
{
	{POOL_DEFAULT, "D3DPOOL_DEFAULT"},
	{POOL_MANAGED, "D3DPOOL_MANAGED"},
	{POOL_SYSTEMMEM, "D3DPOOL_SYSTEMMEM"},
	{POOL_SCRATCH, "D3DPOOL_SCRATCH"}
};

CONST UINT cuiNoOfPoolTypes = (sizeof(DXPoolTypes) / sizeof(DXPoolTypes[0]));

const D3DPOOL_RECORD* FindPoolRecord(DWORD dwPool)
{
	UINT uiPoolRecordIndex;

	for
	(
		uiPoolRecordIndex = 0;
		uiPoolRecordIndex < cuiNoOfPoolTypes;
		++uiPoolRecordIndex
	)
	{
			if (DXPoolTypes[uiPoolRecordIndex].dwFWPool == dwPool)
				return &(DXPoolTypes[uiPoolRecordIndex]);
	}

	return NULL;
}
