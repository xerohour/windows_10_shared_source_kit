#ifndef MY_AUXILIARY_INCLUDED

#define MY_AUXILIARY_INCLUDED

#include <windows.h>

extern char *EndOfString(const char *szString);
extern DWORD ExtractBitsRange(DWORD dwSourceBits, UINT uiLSB, UINT uiMSB);

#endif
