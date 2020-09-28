// FILE:        fpstate.cpp
// DESC:        floating point state comparison utilities

#include <windows.h>
#include "fpstate.h"

// avoid IA64 warnings from WINAPI-export
#ifdef _X86_
#define __DLLEXPORT __declspec(dllexport)
#else
#define __DLLEXPORT
#endif

// NAME:        Init()
// DESC:        init floating point state
// INPUT:       none
// OUTPUT:      none

extern "C" __DLLEXPORT VOID WINAPI Init(VOID)
{
#ifdef _X86_
    _asm
    {
        FINIT
    }
#endif
}

// NAME:        Get()
// DESC:        get floating point state
// INPUT:       pState...address of FPSTATE data
// OUTPUT:      true.....if floating point state stored successfully (or run on alpha)
//              false....otherwise

extern "C" __DLLEXPORT bool WINAPI Get(PFPSTATE pState)
{
#ifdef _X86_
    if (!pState)
        return false;

    memset(pState, 0, sizeof(FPSTATE));

	_asm
	{
		MOV		EDI, pState
		FNSAVE	[EDI]
		FRSTOR	[EDI]
	}
#endif

	return true;
}

// NAME:        Set()
// DESC:        set floating point state, based on input FPSTATE structure
// INPUT:       pState...address of FPSTATE data
// OUTPUT:      true.....if floating point state stored successfully
//              false....otherwise

extern "C" __DLLEXPORT bool WINAPI Set(PFPSTATE pState)
{
#ifdef _X86_
    if (!pState)
        return false;

    _asm
    {
        MOV    EDI, pState
        FRSTOR [EDI]
    }
#endif

    return true;
}

// NAME:        Compare()
// DESC:        compare floating point states
// INPUT:       pBefore..address of FPSTATE data
//              pAfter...address of FPSTATE data
// OUTPUT:      true.....if floating point states compared successfully (or run on alpha)
//              false....otherwise
// COMMENTS:    GDI/display drivers must preserve:
//                  FPU control word
//                  FPU stack top pointer
//                  FPU stack top register
//                  FPU stack top register tag
                  
extern "C" __DLLEXPORT bool WINAPI Compare(PFPSTATE pBefore, PFPSTATE pAfter)
{
#ifdef _X86_
    CHAR pszData[64];

    if (!pBefore)
        return false; // invalid pointer

    if (!pAfter)
        return false; // invalid pointer

    if (pBefore->cw != pAfter->cw)
    {
        wsprintf(pszData, TEXT("FPU control word changed (0x%04X vs 0x%04X).\n"),
                 pBefore->cw, pAfter->cw); 
        OutputDebugString(pszData);
        return false;
    }

    if (pBefore->GetTop() != pAfter->GetTop())
    {
        wsprintf(pszData, TEXT("FPU stack top pointer changed (0x%08X vs 0x%08X).\n"),
                 pBefore->GetTop(), pAfter->GetTop());
        OutputDebugString(pszData);
        return false;
    }

/*
    if (pBefore->GetReg() != pAfter->GetReg())
    {
        wsprintf(pszData, TEXT("FPU stack top register changed (0x%08X vs 0x%08X).\n"),
                 pBefore->GetReg(), pAfter->GetReg));
        OutputDebugString(pszData);
        return false;
    }
*/

    if (pBefore->GetTag() != pAfter->GetTag())
    {
        wsprintf(pszData, TEXT("FPU stack top register tag changed (0x%08X, vs 0x%08X).\n"),
                 pBefore->GetTag(), pAfter->GetTag());
        OutputDebugString(pszData);
        return false;
    }
#endif
	return true;
}

// NAME:    DllMain()
// DESC:    dll main entry point
// INPUT:   hModule......module handle
//          dwReason.....reason for call
//          lpReserved...unused
// OUTPUT:  TRUE

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
