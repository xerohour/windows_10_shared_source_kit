/******************************Module*Header*******************************
* Module Name: overcommit.cpp
* 
* Implementation of a test that overcommits to video memory
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "d3d9.h"
#include "overcommit.h"
#include "ComPtr.h"

#include <time.h>

COverCommit::COverCommit()
{
    SetDescription(L"This component gradually over-commits to video memory");
    
    AddParam<UINT>(L"Time", L"The number of seconds to run for");
    AddParam<UINT>(L"Size", L"The number of megabytes to allocate and gradually use");
    AddParam<bool>(L"Random", L"If true, then the surfaces will be accessed in a random order, otherwise they will be accessed in a round-robin order");
    AddParam<bool>(L"Increase", L"If true, then the working set increases from 0 to Size over time, otherwise it stays constat at Size");
    
    SetParamTestDomainSingleValue<UINT>(L"Time", 60);
    SetParamTestDomainSingleValue<UINT>(L"Size", 512);
    SetParamTestDomainSingleValue<bool>(L"Random", false);
    SetParamTestDomainSingleValue<bool>(L"Increase", true);
}

void COverCommit::PreWalk(CLogWriter* pLog)
{
    Cleanup();

    const UINT nSize     = GetParam<UINT>(L"Size");
    const UINT nTime     = GetParam<UINT>(L"Time");
    const bool bRandom   = GetParam<bool>(L"Random");
    const bool bIncrease = GetParam<bool>(L"Increase");

    //1 surface = 1 MB, which simplifies the code
    const UINT nSurfaces = nSize;
    

    IDirect3DDevice9Ex* pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");

    AllocateSurfaces(pDevice,
                     nSize);
    

    UINT nSurfIndex = 0;
    time_t StartTime = time(NULL);
    time_t CurrentTime = StartTime;

    while((CurrentTime - StartTime) < (time_t)nTime)
    {
        //calculate the working set size
        UINT nWorkingSetSize;

        if(bIncrease)
        {
            nWorkingSetSize = (nSurfaces * ((UINT)(CurrentTime - StartTime))) /  nTime;
            nWorkingSetSize = max(nWorkingSetSize, 1);
            nWorkingSetSize = min(nWorkingSetSize, nSurfaces);
        }
        else
        {
            nWorkingSetSize = nSurfaces;
        }
        
        

        RenderFrame(pDevice, nSurfIndex % nWorkingSetSize);

        if(bRandom)
        {
            nSurfIndex = rand();
        }
        else
        {
            nSurfIndex++;
        }
        
        CurrentTime = time(NULL);
    }
}

void COverCommit::PostWalk(CLogWriter* pLog)
{    
    Cleanup();
}

void COverCommit::Cleanup()
{
    for(size_t i = 0; i < m_Surfaces.size(); i++)
    {
        if(m_Surfaces[i])
        {
            m_Surfaces[i]->Release();
            m_Surfaces[i] = NULL;
        }
    }

    m_Surfaces.clear();   
}

void COverCommit::AllocateSurfaces(IDirect3DDevice9* pDevice,
                                   UINT nSize)
{
    assert(0 == m_Surfaces.size());
    assert(NULL != pDevice);
    
    m_Surfaces.resize(nSize);
    
    //NULL all surfaces in case an error happens here
    //we don't want to crash in Cleanup()
    for(size_t i = 0; i < m_Surfaces.size(); i++)
    {
        m_Surfaces[i] = NULL;
    }
    
    for(size_t i = 0; i < m_Surfaces.size(); i++)
    {
        IDirect3DSurface9* pSurf = NULL;

        //Each surface is 1 MB, which means number of surfaces = number of megabytes
        if(FAILED(pDevice->CreateRenderTarget(512,
                                              512,
                                              D3DFMT_X8R8G8B8,
                                              D3DMULTISAMPLE_NONE,
                                              0,
                                              FALSE,
                                              &pSurf,
                                              NULL)))
        {
            throw std::exception("Failed to create a render target");
        }

        m_Surfaces[i] = pSurf;
    }
}


void COverCommit::RenderFrame(IDirect3DDevice9* pDevice,
                              UINT nSurface)
{
    assert(nSurface < m_Surfaces.size());
    assert(NULL != pDevice);
    
    IDirect3DSurface9* pSurf = m_Surfaces[nSurface];
    assert(NULL != pSurf);

    CComPtr<IDirect3DSurface9> pBackBuffer;
    if(FAILED(pDevice->GetRenderTarget(0, &pBackBuffer.p)))
    {
        throw std::exception("Failed to get the back buffer");
    }
    
    
    //clear with the surface color
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, nSurface, 1.0f, 0);

    //colorfill the surface index
    pDevice->ColorFill(pSurf, NULL, nSurface);
    
    pDevice->Present(0,0,0,0);
}



