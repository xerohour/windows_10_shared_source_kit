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
extern DWORD dwUsageSets[];
extern CONST UINT cuiNoOfUsageSets;
extern void ListUsages(DWORD dwUsageSet, char *szDescription);

#endif
