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
extern UINT CandidateUsagesDX8Dest[];
extern CONST UINT cuiNoOfCandidateUsagesDX8Dest;
extern UINT CandidateUsagesDX9Source[];
extern CONST UINT cuiNoOfCandidateUsagesDX9Source;
extern UINT CandidateUsagesDX9Dest[];
extern CONST UINT cuiNoOfCandidateUsagesDX9Dest;
extern UINT MSUsagesDest[];
extern CONST UINT cuiNoOfMSUsagesDest;
extern UINT CandidateUsagesLDDMSource[];
extern CONST UINT cuiNoOfCandidateUsagesLDDMSource;
extern UINT CandidateUsagesLDDMDest[];
extern CONST UINT cuiNoOfCandidateUsagesLDDMDest;

extern void ListUsages(DWORD dwUsageSet, char *szDescription);

#endif
