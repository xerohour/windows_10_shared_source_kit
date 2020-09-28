#ifndef MY_DXUSAGE_INCLUDED

#define MY_DXUSAGE_INCLUDED

#include "FrameworkTypes.h"

typedef
struct _D3DUSAGE_RECORD
{
	DWORD dwUsage;
	char *szName;
}
D3DUSAGE_RECORD;

extern D3DUSAGE_RECORD DXUsages[];
extern CONST UINT cuiNoOfUsages;
extern DWORD dwSourceUsageSets[];
extern CONST UINT cuiNoOfSourceUsageSets;
extern DWORD dwDestUsageSets[];
extern CONST UINT cuiNoOfDestUsageSets;

extern void ListUsages(DWORD dwUsageSet, char *szDescription);

#endif
