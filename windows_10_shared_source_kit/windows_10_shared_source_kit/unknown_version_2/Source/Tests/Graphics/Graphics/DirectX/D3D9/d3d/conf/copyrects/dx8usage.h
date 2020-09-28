#ifndef DX8USAGE_INCLUDED

#define DX8USAGE_INCLUDED

typedef
struct _D3DUSAGE_RECORD
{
	DWORD dwUsage;
	char *szName;
}
D3DUSAGE_RECORD;

extern D3DUSAGE_RECORD DX8Usages[];
extern DWORD dwUsageSets[];
extern CONST UINT cuiNoOfUsageSets;
extern void ListUsages(DWORD dwUsageSet, char *szDescription);

#endif
