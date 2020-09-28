#include "auxiliary.h"
#include <string.h>

char *EndOfString(const char *szString)
{
	return (char *)(szString + strlen(szString));
}

DWORD ExtractBitsRange(DWORD dwSourceBits, UINT uiLSB, UINT uiMSB)
{
	return (dwSourceBits >> uiLSB) & (DWORD)((UINT)((DWORD)1 << (uiMSB - uiLSB + 1)) - 1);
}
