#include "dxusage.h"

CONST DWORD dwSourceUsages[] =
{
	0
};
CONST UINT cuiNoOfSourceUsages = sizeof(dwSourceUsages) / sizeof(dwSourceUsages[0]);

CONST DWORD dwDestUsages[] =
{
	0
	//USAGE_RENDERTARGET
	/*
	USAGE_AUTOGENMIPMAP,
	USAGE_RENDERTARGET | USAGE_AUTOGENMIPMAP
	*/
};
CONST UINT cuiNoOfDestUsages = sizeof(dwDestUsages) / sizeof(dwDestUsages[0]);
