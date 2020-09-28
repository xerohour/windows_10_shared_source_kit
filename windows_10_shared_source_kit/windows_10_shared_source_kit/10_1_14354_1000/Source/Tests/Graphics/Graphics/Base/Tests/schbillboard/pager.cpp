/******************************Module*Header*******************************
* Module Name: pager.cpp
*
* Implementation of a component that periodically pages in resources
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>
#include <provider.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>
#include <etwStatistics.h>
#include <DXGKernelStatistics.h>

#include "d3d9.h"
#include "d3dx9.h"
#include "time.h"
#include "testcomponent.h"
#include "ComPtr.h"

#include "TestRunner.h"
#include "testcomponent.h"

#include <math.h>

#include "pager.h"
#include <etwLibEvents.h>
#include <CrossProcSharing.h>

extern CProvider etwProvider;

//
// To run the pager functionality in SchBillboard, you'll want to use the following
// command lines:
//
// SchBillboard.exe Pager time:20 mb:150 period:20 width:2000 height:2000
//
// SchBillboard.exe PagerSync time:20 mb:150 period:20 width:2000 height:2000 SyncWith:XY123Z ProcCount:2 AddMB:20 
//

void BuildCommonOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    //match the time cmd line for the billboard mode too
    //just for convience
    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"TotalTime",
                               L"Time",
                               L"The number of seconds to run for" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"Period",
                               L"Period",
                               L"The number of seconds between each page-in operation" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"MB",
                               L"MB",
                               L"The number of megabytes of video memory to use" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"InitialPagein",
                               L"PageIn",
                               L"If true, then all surfaces will be initially paged-in" );
}

void BuildPagerCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    nodeStr += L"/pager";
    _pRunner->AddNode<CPager>( nodeStr.c_str() );

    BuildCommonOptions(_pRunner,nodeStr);
}

void BuildPagerSyncCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    nodeStr += L"/PagerSync";
    _pRunner->AddNode<CPagerSync>( nodeStr.c_str() );

    BuildCommonOptions(_pRunner, nodeStr);

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"AddMB",
                               L"AddMB",
                               L"The number of megabytes of video memory to grow by" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"SyncKeyString",  // Variable name
                               L"SyncWith",       // CommandLine string
                               L"Unique string to define global shared memory name" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"CountOfProcesses",  // Variable name
                               L"ProcCount",       // CommandLine string
                               L"Number of applications that will be monitored including manager" );
}


//------------------------------------------------------------------------------

CPager::CPager()
{
    SetDescription(L"This component creates many surfaces.  It then pages them all into video memory.  Next, this component uses 1 resource for a set amount of time, and then uses another (which will cause paging)");
    
    CommonSetupCommandLine();
}

void CPager::PreWalk(CLogWriter* pLog)
{
    const UINT nPeriod = GetParam<UINT>(L"Period");
    const UINT nTotalTime = GetParam<UINT>(L"TotalTime");
    const UINT nMegabytes = GetParam<UINT>(L"MB");
    const bool bPagein    = GetParam<bool>(L"InitialPagein");

    m_pDevice = CComPtr<IDirect3DDevice9Ex>( GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex") );    

    CreateSurfaces(nMegabytes);

    if(bPagein)
    {
        //use all surfaces once (to get them all paged in)
        for(size_t i = 0; i < m_Surfaces.size(); i++)
        {
            UseSurface(i);
        }
    }

    size_t nIndex = 0;

    time_t InitialTime = time(NULL);
    time_t CurrentTime = InitialTime;
    
    while(((UINT)(CurrentTime - InitialTime)) < nTotalTime)
    {
        //do this first so it starts at 1
        nIndex = (nIndex + 1) % m_Surfaces.size();

        time_t PeriodBegin = time(NULL);
        CurrentTime = PeriodBegin;
        
        while(((UINT)(CurrentTime - PeriodBegin)) < nPeriod)
        {
            UseSurface(0);

            CurrentTime = time(NULL);
        }
        UseSurface(nIndex);
    }
}

void CPager::PostWalk(CLogWriter* pLog)
{
    m_Surfaces.clear();
    m_pDevice = CComPtr< IDirect3DDevice9Ex >();
}

//------------------------------------------------------------------------------

CPagerSync::CPagerSync()
{
    SetDescription(L"This component creates many surfaces for a listening manager (CrossProcSharing). It then pages them all into video memory.  Next, this component uses 1 resource for a set amount of time, and then uses another (which will cause paging)");

    CommonSetupCommandLine();

    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    AddParam<UINT>(L"AddMB", L"The number of megabytes of video memory to grow by");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);
    SetParamTestDomainSingleValue<UINT>(L"AddMB", 0);
}

void CPagerSync::PreWalk(CLogWriter* pLog)
{
    const UINT nPeriod = GetParam<UINT>(L"Period");
    const UINT nTotalTime = GetParam<UINT>(L"TotalTime");
    UINT nMegabytes = GetParam<UINT>(L"MB");
    const bool bPagein    = GetParam<bool>(L"InitialPagein");
    const UINT nAddMB    = GetParam<UINT>(L"AddMB");


    std::string SyncKey = GetParam<std::string>(L"SyncKeyString");
    ULONG ulCount = GetParam<UINT>(L"CountOfProcesses");

    m_pDevice = CComPtr<IDirect3DDevice9Ex>( GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex") );    

    // Now setup for xProc Sharing if it's enabled

    // Create our client class
    CXProcSyncClient xProcSync((char*)SyncKey.c_str(),ulCount); 

    CreateSurfaces(nMegabytes);

    if(bPagein)
    {
        //use all surfaces once (to get them all paged in)
        for(size_t i = 0; i < m_Surfaces.size(); i++)
        {
            UseSurface(i);
        }
    }

    // Now we transition into ready and wait for the manager app to setup for
    // counting and timing.

    STATE_TYPE stRequest = xProcSync.TransitionTo(eStateReady);

    Sleep(100);

    CreateSurfaces(nMegabytes);

    if(bPagein)
    {
        //use all surfaces once (to get them all paged in)
        for(size_t i = 0; i < m_Surfaces.size(); i++)
        {
            UseSurface(i);
        }
    }
    // We're going to loop here as long as the manager doesn't ask us to terminate.
    // if
    while( stRequest != eStateTerminate )
    {
        size_t nIndex = 0;

        time_t InitialTime = time(NULL);
        time_t CurrentTime = InitialTime;
    
#ifdef DBG
        {
            WCHAR wszBuf[MAX_PATH];
            StringCchPrintf(wszBuf,MAX_PATH,L"Total Time for test is %d\n\r",nTotalTime);
            OutputDebugString(wszBuf);
            StringCchPrintf(wszBuf,MAX_PATH,L"Number of surfaces %d\n\r",m_Surfaces.size());
            OutputDebugString(wszBuf);
        }
#endif
        ULONG ulUseSurfaceZeroCounter=0;
        ULONG ulUseSurfaceCounter=0;
        while( (((UINT)(CurrentTime - InitialTime)) < nTotalTime) &&
               stRequest == eStateRun )
        {
            //do this first so it starts at 1
            nIndex = (nIndex + 1) % m_Surfaces.size();

            UseSurface(0);
            ulUseSurfaceZeroCounter++;

            ulUseSurfaceCounter++;
            UseSurface(nIndex);

            // Check to see if the manager wants us to finish
            stRequest = xProcSync.CheckForRequest();
        }
#ifdef DBG
        {
            WCHAR wszBuf[MAX_PATH];
            StringCchPrintf(wszBuf,MAX_PATH,L"Full Pass Done - Counts for SurfaceZero %d, Other=%d\n\r",ulUseSurfaceZeroCounter,ulUseSurfaceCounter);
            OutputDebugString(wszBuf);
        }
#endif
        // Note that if we time out in the above loop, our state will be eStateRun. Thus
        // we will not ask for a state change. Because of this, the manager timeout should
        // be shorter than the timeout here.
        if( stRequest != eStateRun )
        {
            // The Manager has asked us to transition into another state, let's do that.
            // If that request is to go into a ready state, we'll go there and loop until
            // the manager switches us back into a Running state. If we do get switched 
            // back into a running state, we're going to increase our workload and perform
            // our work again.
            stRequest = xProcSync.TransitionTo(stRequest);
            if( stRequest == eStateRun )
            {
                // For now, we're just going to run again not changing our workload!!!
                nMegabytes += nAddMB;
                CreateSurfaces(nMegabytes);
#ifdef DBG
                {
                    WCHAR wszBuf[MAX_PATH];
                    StringCchPrintf(wszBuf,MAX_PATH,L"Grew nMegabytes by %d to %d\n\r",nAddMB,nMegabytes);
                    OutputDebugString(wszBuf);
                }
#endif
            }
        }
    }

    // We are done, so we need to tell the manager
    xProcSync.TransitionTo(eStateTerminate);
}

void CPagerSync::PostWalk(CLogWriter* pLog)
{
    m_Surfaces.clear();
    m_pDevice = CComPtr< IDirect3DDevice9Ex >();
}


// -----------------------------------------------------------------------------
//
// Base functionality for pagers
//
void CPagerBase::CommonSetupCommandLine()
{
    AddParam<UINT>(L"Period", L"The number of seconds betewen each paging operation (after the initial paging)");
    AddParam<UINT>(L"TotalTime", L"The total number of seconds to run for");
    AddParam<UINT>(L"MB", L"The number of megabytes of video memory to fill");
    AddParam<bool>(L"InitialPagein", L"If true, then all surfaces will be used once at startup");

    SetParamTestDomainSingleValue<UINT>(L"Period", 5);
    SetParamTestDomainSingleValue<UINT>(L"TotalTime", 60);
    SetParamTestDomainSingleValue<UINT>(L"MB", 28);
    SetParamTestDomainSingleValue<bool>(L"InitialPagein", true);
}


void CPagerBase::CreateSurfaces(UINT nMegabytes)
{
    HRESULT hr;
    CComPtr< IDirect3DSurface9 > pBackBuffer;
 
    hr = m_pDevice->GetRenderTarget(0, &pBackBuffer.p);
    if(FAILED(hr))
    {
        throw std::exception("GetRenderTarget failed");
    }

    D3DSURFACE_DESC Desc;
    hr = pBackBuffer->GetDesc(&Desc);
    if(FAILED(hr))
    {
        throw std::exception("GetDesc failed");
    }


    //the R200 will allocate video memory if the source of a blt is larger
    //than the dst.  This messes up the results, so make sure they are the same size
    UINT nWidth = Desc.Width;
    UINT nHeight = Desc.Height;
    UINT nBPP = 4;
    
    UINT nMegabytesPerSurface = (nWidth * nHeight * nBPP) / (1024 * 1024);
    
    UINT nSurfaces = (nMegabytes / nMegabytesPerSurface) + 1;
    
    m_Surfaces.clear();
    
    m_Surfaces.resize(nSurfaces);
    
    for(size_t i = 0; i < nSurfaces; i++)
    {
        CComPtr< IDirect3DSurface9 > Surf;
        
        hr = m_pDevice->CreateRenderTarget(nWidth,
                                            nHeight,
                                            D3DFMT_X8R8G8B8,
                                            D3DMULTISAMPLE_NONE,
                                            0,
                                            FALSE,
                                            &Surf.p,
                                            NULL);
        
        if(FAILED(hr))
        {
            throw std::exception("Failed to create a render target");
        }
        
                
        m_Surfaces[i] = Surf;
    }
}

void CPagerBase::UseSurface(size_t nIndex)
{
    IDirect3DSurface9* pSurf = m_Surfaces[nIndex].p;
    
    HRESULT hr = m_pDevice->ColorFill(pSurf, 
                                      NULL,
                                      rand());
    
    if(FAILED(hr))
    {
        throw std::exception("ColorFill failed");
    }

    CComPtr< IDirect3DSurface9 > pRT;
    
    hr = m_pDevice->GetRenderTarget(0, &pRT.p);
    if(FAILED(hr))
    {
        throw std::exception("GetRenderTarget failed");
    }
    
    hr = m_pDevice->StretchRect(pSurf, NULL, pRT, NULL, D3DTEXF_NONE);
    if(FAILED(hr))
    {
        throw std::exception("StretchRect failed");
    }
    
    hr = m_pDevice->Present(0, 0, 0, 0);
    if(FAILED(hr))
    {
        throw std::exception("Present failed");
    }
}



