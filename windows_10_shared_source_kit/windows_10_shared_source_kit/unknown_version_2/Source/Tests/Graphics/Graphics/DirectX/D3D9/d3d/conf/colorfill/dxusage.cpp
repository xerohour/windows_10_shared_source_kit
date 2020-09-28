#include <d3d8.h>
#include <stdio.h>
#include "dxusage.h"

D3DUSAGE_RECORD DXUsages[] =
{
	{USAGE_RENDERTARGET, "D3DUSAGE_RENDERTARGET"},
	{USAGE_DEPTHSTENCIL, "D3DUSAGE_DEPTHSTENCIL"}
	//{USAGE_TRANSFER, "D3DUSAGE_TRANSFER"}
};

CONST UINT cuiNoOfUsages = sizeof(DXUsages) / sizeof(DXUsages[0]);

DWORD dwUsageSets[] =
{
	//USAGE_TRANSFER
	//0, D3DUSAGE_DEPTHSTENCIL, D3DUSAGE_RENDERTARGET, (D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET)
	D3DUSAGE_RENDERTARGET
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
