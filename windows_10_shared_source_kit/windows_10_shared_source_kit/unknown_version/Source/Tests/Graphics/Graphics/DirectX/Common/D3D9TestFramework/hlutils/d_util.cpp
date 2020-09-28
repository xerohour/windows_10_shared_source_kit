// Debug utility functions:

#include "D_Util.h"
#include "Framework.h"


// This is defined here so non-CD3DTest member functions can join in the fun too!
void D_Out(const TCHAR *ptcsFmt, ...)
{
    TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);		// defined in TestUtils.h
	va_end(argList);
	
    if(g_pFramework)
		g_pFramework->DebugPrint(1, sTemp);

	SAFEDELETEA(sTemp);
}


bool D_MemError(const TCHAR* tcsLHS, const TCHAR* tcsRHS, const TCHAR* tcsFile, int iLine)
{
	D_Out(_T("%s:%d: Out of memory: %s = %s\n"), tcsFile, iLine, tcsLHS, tcsRHS);
	return false;
}

bool D_RefError(const TCHAR* tcsRef, const TCHAR* tcsFile, int iLine)
{
	D_Out(_T("%s:%d: Ref invalid, possible out of memory: %s\n"), tcsFile, iLine, tcsRef);
	return false;
}

bool D_HRError(HRESULT hr, const TCHAR* tcsRHS, const TCHAR* tcsFile, int iLine)
{
	D_Out(_T("%s:%d: Failed hr(%s): %s\n"), tcsFile, iLine, g_pFramework->GetD3D()->HResultToString(hr), tcsRHS);
	return false;
}

