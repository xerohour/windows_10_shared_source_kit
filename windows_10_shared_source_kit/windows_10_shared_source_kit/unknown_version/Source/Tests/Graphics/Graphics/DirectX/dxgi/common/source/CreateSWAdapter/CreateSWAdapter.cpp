//////////////////////////////////////////////////////////////////////
// File:  DXGICreateAdapter.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Class to create DXGI reference/software adapter.
//
// History:
// 02/01/2008 Jianchen  Created.
//////////////////////////////////////////////////////////////////////
#include "CreateSWAdapter.h"
#include <StrSafe.h>

////////////////////////////////////////////////////////////////////////////////
// clsDXGICreateSWAdapter Methods

clsDXGICreateSWAdapter::clsDXGICreateSWAdapter
(
)
: m_pSWAdapter(NULL)
{
}

clsDXGICreateSWAdapter::~clsDXGICreateSWAdapter
(
)
{
	if (m_pSWAdapter != NULL)
    {
        m_pSWAdapter->Release();
    }
}

//----------------------------------------------------------------------------------
// Function: clsDXGICreateSWAdapter::CreateRefSwAdapter()
// Purpose:  Create a software adapter.
//----------------------------------------------------------------------------------
HRESULT 
clsDXGICreateSWAdapter::CreateSWAdapter
(
    __in USHORT ushSWRasterizerVer
)
{
	HRESULT         hrResult = S_OK;
	IDXGIFactory    *pFactoryInterface = NULL;
    HMODULE         hRasterizer = NULL;

    if (m_pSWAdapter != NULL)
    {
        m_pSWAdapter->Release();
        m_pSWAdapter = NULL; 
    }

    hrResult = CreateDXGIFactory(IID_IDXGIFactory, (void **)&pFactoryInterface);
    if (FAILED(hrResult))
    {
	    DebugOut.Write("clsDXGICreateSWAdapter::CreateRefSwAdapter:  CreateDXGIFactory failed with error 0x%x.", hrResult);
	    goto Exit;
    };

    switch (ushSWRasterizerVer)
    {
        case WARP10_ADAPTER:
            hRasterizer = LoadLibraryEx(WARP10_RASTERIZER_NAME, NULL, 0);
            if (hRasterizer == NULL)
            {
                DebugOut.Write("clsDXGICreateSWAdapter::CreateRefSwAdapter: Failed to load the rasterizer %s.",
                               (LPCTSTR)WARP10_RASTERIZER_NAME);
	            goto Exit;
            };
            break;

        case D3D10_REF_ADAPTER:
            hRasterizer = LoadLibraryEx(D3D10_REF_RASTERIZER_NAME, NULL, 0);
            if (hRasterizer == NULL)
            {
                DebugOut.Write("clsDXGICreateSWAdapter::CreateRefSwAdapter: Failed to load the rasterizer %s.",
                               (LPCTSTR)D3D10_REF_RASTERIZER_NAME);
	            goto Exit;
            };
            break;

        default:
    	    DebugOut.Write("clsDXGICreateSWAdapter::CreateRefSwAdapter: unknown software rasterizer version %u.", ushSWRasterizerVer);
	        goto Exit;

    }

    // Create a software adapter.
    hrResult = pFactoryInterface->CreateSoftwareAdapter(hRasterizer, &m_pSWAdapter);
    if (FAILED(hrResult))
    {
	    DebugOut.Write("clsDXGICreateSWAdapter::CreateRefSwAdapter:  CreateSoftwareAdapter failed with error 0x%x.", hrResult);
    };
    FreeLibrary(hRasterizer);

Exit:
    if (pFactoryInterface != NULL)
    {
	    pFactoryInterface->Release();
    }
	return hrResult;
};