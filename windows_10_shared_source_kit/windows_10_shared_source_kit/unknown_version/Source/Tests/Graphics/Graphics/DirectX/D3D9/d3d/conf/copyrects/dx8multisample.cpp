#include "dx8multisample.h"

D3DMULTISAMPLE_TYPE_RECORD DX8MultiSampleTypes[] =
{
	{D3DMULTISAMPLE_NONE, "D3DMULTISAMPLE_NONE"},
	{D3DMULTISAMPLE_2_SAMPLES, "D3DMULTISAMPLE_2_SAMPLES"}
};

CONST UINT cuiNoOfMultiSampleTypes = sizeof(DX8MultiSampleTypes) / sizeof(DX8MultiSampleTypes[0]);

const D3DMULTISAMPLE_TYPE_RECORD* FindMultiSampleRecord(D3DMULTISAMPLE_TYPE d3dmst)
{
	UINT uiMSTRecordIndex;

	for
	(
		uiMSTRecordIndex = 0;
		uiMSTRecordIndex < cuiNoOfMultiSampleTypes;
		++uiMSTRecordIndex
	)
	{
			if (DX8MultiSampleTypes[uiMSTRecordIndex].d3dmst == d3dmst)
				return &(DX8MultiSampleTypes[uiMSTRecordIndex]);
	}

	return NULL;
}
