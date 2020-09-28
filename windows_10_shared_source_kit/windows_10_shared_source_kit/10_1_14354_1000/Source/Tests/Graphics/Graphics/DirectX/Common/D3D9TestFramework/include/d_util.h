
#ifndef _D_UTIL_H_
#define _D_UTIL_H_

#include "TestUtils.h"

// ----------------------------------------------------------------------------
// The following are a couple of macros to aid in checking allocations:

// use to check allocation of memory.  ex: if(!CHECKNEW(pBlah, new Blah())) return FAILED;
#define CHECKNEW(lhs, rhs) (((lhs) = (rhs)) ? true : D_MemError(#lhs, #rhs, __FILE__, __LINE__))

// use after giving new ownership to ref, for example. (requires node allocation)
//	if(!CHECKREF(rBlah.Own(pBlah))) return FAILED;
#define CHECKREF(ref) ((ref).Valid() ? true : D_RefError(#ref, __FILE__, __LINE__))

#define CHECKHR(hr, rhs) (FAILED(hr = (rhs)) ? D_HRError(hr, #rhs, __FILE__, __LINE__) : true)


void D_Out(const TCHAR *ptcsFmt, ...);
bool D_MemError(const TCHAR* tcsLHS, const TCHAR* tcsRHS, const TCHAR* tcsFile, int iLine);
bool D_RefError(const TCHAR* tcsRef, const TCHAR* tcsFile, int iLine);
bool D_HRError(HRESULT hr, const TCHAR* tcsRHS, const TCHAR* tcsFile, int iLine);


#endif // _D_UTIL_H_