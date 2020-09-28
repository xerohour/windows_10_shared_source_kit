#include <d3d8.h>
#include <stdio.h>
#include "dx8usage.h"

D3DUSAGE_RECORD DX8Usages[] =
{
	{D3DUSAGE_DEPTHSTENCIL, "D3DUSAGE_DEPTHSTENCIL"},
	{D3DUSAGE_RENDERTARGET, "D3DUSAGE_RENDERTARGET"}
};

DWORD dwUsageSets[] =
{
	0, D3DUSAGE_DEPTHSTENCIL, D3DUSAGE_RENDERTARGET, (D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET)
};

CONST UINT cuiNoOfUsageSets = (sizeof(dwUsageSets) / sizeof(dwUsageSets[0]));

void ListUsages(DWORD dwUsageSet, char *szDescription)
{
	UINT uiUsageRecordIndex;
	bool bElementAdded = false;

	if (dwUsageSet)
	{
		for
		(
			uiUsageRecordIndex = 0;
			uiUsageRecordIndex < (sizeof(DX8Usages) / sizeof(DX8Usages[0]));
			++uiUsageRecordIndex
		)
		{
			if (dwUsageSet & (DX8Usages[uiUsageRecordIndex]).dwUsage)
			{
				if (bElementAdded)
				{
					strcpy(szDescription, ", ");
					szDescription += 2;
				}
				else
					bElementAdded = true;

				strcpy(szDescription, (DX8Usages[uiUsageRecordIndex]).szName);
				do {} while (*(++szDescription));
			}
		}
	}
	else
		strcpy(szDescription, "(None)");

}
