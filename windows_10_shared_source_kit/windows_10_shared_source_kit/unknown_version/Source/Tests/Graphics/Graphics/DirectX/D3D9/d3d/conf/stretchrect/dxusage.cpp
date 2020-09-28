#include <d3d8.h>
#include <stdio.h>
#include "dxusage.h"

D3DUSAGE_RECORD DXUsages[] =
{
	{USAGE_RENDERTARGET, "D3DUSAGE_RENDERTARGET"},
	{USAGE_DEPTHSTENCIL, "D3DUSAGE_DEPTHSTENCIL"},
	{USAGE_AUTOGENMIPMAP, "D3DUSAGE_AUTOGENMIPMAP"}
	//{USAGE_TRANSFER, "D3DUSAGE_TRANSFER"}
};

CONST UINT cuiNoOfUsages = sizeof(DXUsages) / sizeof(DXUsages[0]);

// Note: The arrays below reference the elements of this array by index.  Take care
// when modifying the initializer for this array.
DWORD dwUsageSets[] =
{
	0, USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)
};
CONST UINT cuiNoOfUsageSets = (sizeof(dwUsageSets) / sizeof(dwUsageSets[0]));

// DX8 drivers (and earlier):

// Possible usages (indexing dwUsageSets) for destination textures:
UINT CandidateUsagesDX8Dest[] = {1, 2}; // {USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)}
CONST UINT cuiNoOfCandidateUsagesDX8Dest = sizeof(CandidateUsagesDX8Dest) / sizeof(CandidateUsagesDX8Dest[0]);

// DX9 drivers (and later):

// Possible usages (indexing dwUsageSets) for source textures:
UINT CandidateUsagesDX9Source[] = {0, 1, 2}; // {0, USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)}
CONST UINT cuiNoOfCandidateUsagesDX9Source = sizeof(CandidateUsagesDX9Source) / sizeof(CandidateUsagesDX9Source[0]);

// Possible usages (indexing dwUsageSets) for destination textures:
UINT CandidateUsagesDX9Dest[] = {1, 2}; // {USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)}
CONST UINT cuiNoOfCandidateUsagesDX9Dest = sizeof(CandidateUsagesDX9Dest) / sizeof(CandidateUsagesDX9Dest[0]);

// Possible usages (indexing dwUsageSets) for destination textures in "multisampled StretchRects".
UINT MSUsagesDest[] = {1};
CONST UINT cuiNoOfMSUsagesDest = sizeof(MSUsagesDest) / sizeof(MSUsagesDest[0]);

// LDDM drivers:

// Possible usages (indexing dwUsageSets) for source textures:
UINT CandidateUsagesLDDMSource[] = {0, 1, 2}; // {0, USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)}
CONST UINT cuiNoOfCandidateUsagesLDDMSource = sizeof(CandidateUsagesDX9Source) / sizeof(CandidateUsagesDX9Source[0]);

// Possible usages (indexing dwUsageSets) for destination textures:
UINT CandidateUsagesLDDMDest[] = {0, 1, 2}; // {USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)}
CONST UINT cuiNoOfCandidateUsagesLDDMDest = sizeof(CandidateUsagesDX9Dest) / sizeof(CandidateUsagesDX9Dest[0]);

// "Utility" functions:

void ListUsages(DWORD dwUsageSet, char *szDescription)
{
	UINT uiUsageRecordIndex;
	bool bElementAdded = false;

	if (dwUsageSet)
	{
		for
		(
			uiUsageRecordIndex = 0;
			uiUsageRecordIndex < cuiNoOfUsages;
			++uiUsageRecordIndex
		)
		{
			if (dwUsageSet & (DXUsages[uiUsageRecordIndex]).dwUsage)
			{
				if (bElementAdded)
				{
					strcpy(szDescription, ", ");
					szDescription += 2;
				}
				else
					bElementAdded = true;

				strcpy(szDescription, (DXUsages[uiUsageRecordIndex]).szName);
				do {} while (*(++szDescription));
			}
		}
	}
	else
		strcpy(szDescription, "(None)");

}
