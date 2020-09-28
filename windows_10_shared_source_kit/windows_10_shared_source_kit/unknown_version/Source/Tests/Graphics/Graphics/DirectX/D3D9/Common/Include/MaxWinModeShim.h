///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaxWinModeShim.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose: Contains interfaces for FPU state checking.
// 
// 02/09/2010  Created by @Jianchen.
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <float.h>
#include <intrin.h>

typedef void (WINAPI *PFNDirect3D9EnableMaximizedWindowedModeShim)(__in BOOL bAutoShimmed);

class CMaxWinModeShim
{
public:
    CMaxWinModeShim();
    virtual ~CMaxWinModeShim();

    HRESULT LoadMaxWinModeShimShimProc();
    void    Direct3D9EnableMaximizedWindowedModeShim(__in BOOL bAutoShimmed);

private:
    PFNDirect3D9EnableMaximizedWindowedModeShim  m_pfnDirect3D9EnableMaximizedWindowedModeShim;
    HMODULE      m_hD3D9DLL;
};
