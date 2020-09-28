#include "dxmultisample.h"

D3DMULTISAMPLE_TYPE_RECORD DXMultiSampleTypes[] =
{
	{MULTISAMPLE_NONE, "D3DMULTISAMPLE_NONE"},
	//{MULTISAMPLE_2_SAMPLES, "D3DMULTISAMPLE_2_SAMPLES"}	multisampling not supported by the API
};

CONST UINT cuiNoOfMultiSampleTypes = sizeof(DXMultiSampleTypes) / sizeof(DXMultiSampleTypes[0]);

const D3DMULTISAMPLE_TYPE_RECORD* FindMultiSampleRecord(MULTISAMPLE_TYPE mst)
{
	UINT uiMSTRecordIndex;

	for
	(
		uiMSTRecordIndex = 0;
		uiMSTRecordIndex < cuiNoOfMultiSampleTypes;
		++uiMSTRecordIndex
	)
	{
			if (DXMultiSampleTypes[uiMSTRecordIndex].mstFW == mst)
				return &(DXMultiSampleTypes[uiMSTRecordIndex]);
	}

	return NULL;
}
