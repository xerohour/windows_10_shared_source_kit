///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaxWinModeShim.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose: Provide a shared class to get the proc pointer of Direct3D9EnableMaximizedWindowedModeShim() 
//          from d3d9.dll and call this API 
// 
// 08/2010  Created by @Jianchen.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "MaxWinModeShim.h"

CMaxWinModeShim::CMaxWinModeShim()
: m_pfnDirect3D9EnableMaximizedWindowedModeShim(NULL),
  m_hD3D9DLL(NULL)
{
}

CMaxWinModeShim::~CMaxWinModeShim()
{
    if (m_hD3D9DLL != NULL)
    {
        FreeLibrary(m_hD3D9DLL);
    }
}

HRESULT 
CMaxWinModeShim::LoadMaxWinModeShimShimProc
(
)
{
    HRESULT     hrResult = S_OK;

    m_hD3D9DLL = LoadLibraryExW(L"d3d9.dll", NULL, 0);
    if (m_hD3D9DLL != NULL)
    {
        m_pfnDirect3D9EnableMaximizedWindowedModeShim = 
            (PFNDirect3D9EnableMaximizedWindowedModeShim) GetProcAddress (m_hD3D9DLL, "Direct3D9EnableMaximizedWindowedModeShim");

        if (m_pfnDirect3D9EnableMaximizedWindowedModeShim != NULL)
        {
            return hrResult;
        }
    }

   	hrResult = HRESULT_FROM_WIN32(GetLastError());
    return hrResult;
}

void 
CMaxWinModeShim::Direct3D9EnableMaximizedWindowedModeShim
(
    __in BOOL   bAutoShimmed
)
{
    assert (m_pfnDirect3D9EnableMaximizedWindowedModeShim != NULL);

    m_pfnDirect3D9EnableMaximizedWindowedModeShim(bAutoShimmed); 
}
