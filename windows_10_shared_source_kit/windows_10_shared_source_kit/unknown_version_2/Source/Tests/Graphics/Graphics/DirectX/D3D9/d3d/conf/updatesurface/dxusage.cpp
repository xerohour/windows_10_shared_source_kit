#include <d3d8.h>
#include <stdio.h>
#include "dxusage.h"

D3DUSAGE_RECORD DXUsages[] =
{
	{USAGE_RENDERTARGET, "D3DUSAGE_RENDERTARGET"},
	{USAGE_AUTOGENMIPMAP, "D3DUSAGE_AUTOGENMIPMAP"}
	//{USAGE_HEAVYWEIGHT, "D3DUSAGE_HEAVYWEIGHT"}
	//{USAGE_DEPTHSTENCIL, "D3DUSAGE_DEPTHSTENCIL"}
	//{USAGE_TRANSFER, "D3DUSAGE_TRANSFER"}
};
CONST UINT cuiNoOfUsages = sizeof(DXUsages) / sizeof(DXUsages[0]);

// UpdateSurface-Specific:
// Valid usages for source texture surfaces (system memory only).
DWORD dwSourceUsageSets[] =
{
	0
};
CONST UINT cuiNoOfSourceUsageSets = (sizeof(dwSourceUsageSets) / sizeof(dwSourceUsageSets[0]));

// UpdateSurface-Specific:
// Valid usages for destination texture surfaces (video memory only).
DWORD dwDestUsageSets[] =
{
	0, USAGE_AUTOGENMIPMAP, USAGE_RENDERTARGET, (USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP)
};
CONST UINT cuiNoOfDestUsageSets = (sizeof(dwDestUsageSets) / sizeof(dwDestUsageSets[0]));

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
