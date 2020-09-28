/******************************Module*Header*******************************
 * Module Name: billboard.cpp
 *
 * Implementation of the D3D sample "billboard", ported to the base video test infrastructure
 *
 * Author: Blake Pelton [blakep]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>
#include <provider.h>
#include <CrossProcSharing.h>

#include "billboard.h"
#include <time.h>
#include "threadtemplate.h"//for launchthread
#include "array.h"
#include "dx9addwindow.h"//for GetSwapChainEx

#include <Quadrant.h>


BOOL CALLBACK EnumWindowsCB(HWND hwnd, LPARAM lParam);


#define SAFE_DELETE_ARRAY(x) if(x) {delete [] x; x=NULL;}

int __cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 );

// Need global access to the eye direction used by the callback to sort trees
D3DXVECTOR3 g_vDir;

// Tree textures to use
TCHAR* g_strTreeTextures[NUMTREETEXTURES] =
{
    _T("Tree02S.dds"),
    _T("Tree35S.dds"),
    _T("Tree01S.dds"),
    _T("Tree02S1.dds"),
    _T("Tree35S1.dds"),
    _T("Tree01S1.dds"),
    _T("Tree02S2.dds"),
    _T("Tree35S2.dds"),
    _T("Tree01S2.dds"),
};

// Simple function to define "hilliness" for terrain
inline FLOAT HeightField( FLOAT x, FLOAT y )
{
    return 9*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

const DWORD TREEVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;


/*
  Name: SharedTexture
  Desc: Cross-process communication mechanism for communicating
  which textures have been created

  CBillBoard::m_SharedResources contains is an array of these
*/
struct SharedTexture
{
    wchar_t szName[256];//the name of the texture
    HANDLE hTexture;//global handle
    UINT nWidth;
    UINT nHeight;
    UINT nMipLevels;
    DWORD dwUsage;
    D3DFORMAT Format;
    D3DPOOL Pool;
};

/*
  Name: SharedTextureTable
  Desc: The entire table of shared textures that is used
  for x-proc shared resources
*/
const UINT SHARED_TEXTURE_COUNT = 100;
struct SharedTextureTable
{
    UINT nCount;
    SharedTexture TextureInfo[SHARED_TEXTURE_COUNT];
};


/*
  Name: LOCK_TABLE
  Desc: Locks m_SharedResources for reading or writing
  Unlocks when it goes out of scope
*/
#define LOCK_TABLE() CMutexLocker __mutex_locker = m_SharedMutex.Lock();

void SetCPUPriority(UINT nPriority, HANDLE hProcess, HANDLE hThread);

struct PriorityChangeData
{
    HANDLE hProcess;
    HANDLE hThread;
    UINT nFrequency;//in seconds
};


class CPriorityChanger
{
public:
    void operator()(const PriorityChangeData& Info)
    {
        //increase the priority of this thread, so that it doesn't get starved
        ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

        for(int i = 1; i < 32; i++)
        {
            SetCPUPriority(i, Info.hProcess, Info.hThread);
            ::Sleep(Info.nFrequency * 1000);
        }
    }
};




CBillBoard::CBillBoard() : 
    m_SharedResources(L"SCHBILLBOARDTABLE", sizeof(SharedTextureTable)), 
    m_SharedMutex(L"SCHBILLBOARDMUTEX"), 
    m_pWindowLoc(NULL)
{
    SetDescription(L"This is the D3D sample called billboard");

    AddParam<UINT>(L"Timeout", L"The number of seconds to run for");
    AddParam<bool>(L"UseXProc", L"Determines if corss-process resource sharing will be used");
    AddParam<UINT>(L"CPUPriorityChangeFreq", L"If non-0, then the cpu priority of this app increases 1 step every XXX seconds, where XXX is the value of this variable");
    AddParam<UINT>(L"SleepTime", L"The number of miliseconds to sleep between each frame, (0 and sleep will not be called)");
    AddParam<float>(L"TextureScale", L"The amount to scale the width and height of each texture by");
    AddParam<bool>(L"Foreground", L"If true, the rendering window is the foreground window");
    AddParam<bool>(L"Present", L"If false, then the rendering window will be minimized so that all present operations are clipped");
    AddParam<UINT>(L"FullScreenFreq", L"If non-0, then this specifies the number of seconds between full/screen windowed switches");
    AddParam<bool>(L"Windowed", L"Determines if the app starts in fullscreen or windowed mode");
    AddParam<bool>(L"GPUPriority", L"If true, then the GPU priority of this app will vary from -4 to 4 over the lifetime of this app");
    AddParam<bool>(L"VaryFrameLatency", L"If true, the maximum frame latency will be varied from 0 to 10");
    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    AddParam<UINT>(L"QuadrantLoc", L"Number either 1,2,3 or 4 - denotes window quadrant to use");

    SetParamTestDomainSingleValue<UINT>(L"Timeout", 10);
    SetParamTestDomainSingleValue<bool>(L"UseXProc", false);
    SetParamTestDomainSingleValue<UINT>(L"CPUPriorityChangeFreq", 0);
    SetParamTestDomainSingleValue<UINT>(L"SleepTime", 0);
    SetParamTestDomainSingleValue<float>(L"TextureScale", 1.0f);
    SetParamTestDomainSingleValue<bool>(L"Foreground", false);//by default, don't let the scheduler give a higher priority to 1 window
    SetParamTestDomainSingleValue<bool>(L"Present", true);
    SetParamTestDomainSingleValue<UINT>(L"FullScreenFreq", 0);
    SetParamTestDomainSingleValue<bool>(L"Windowed", true);
    SetParamTestDomainSingleValue<bool>(L"GPUPriority", false);
    SetParamTestDomainSingleValue<bool>(L"VaryFrameLatency", false);
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);
    SetParamTestDomainSingleValue<UINT>(L"QuadrantLoc", 0);

    if(!m_SharedResources.WasOpened())
    {
        //we created the shared table, initialize it
        LOCK_TABLE();
        ZeroMemory(m_SharedResources.GetPtr(), sizeof(SharedTextureTable));
    }

    m_pDevice = NULL;
    m_bUseCrossProc = false;
    m_nSleepTime = 0;
    m_fTextureScale = 1.0f;
    m_hRenderWnd = NULL;
}


HRESULT CBillBoard::CreateTexture(const wchar_t* szFileName,
                                  IDirect3DTexture9** ppOut)
{
    if(m_bUseCrossProc)
    {
        LOCK_TABLE();
        
        SharedTextureTable* pTable = (SharedTextureTable*)m_SharedResources.GetPtr();
        
        //see if it has already been created
        for(UINT i = 0; i < pTable->nCount; i++)
        {
            if(0 == _wcsicmp(pTable->TextureInfo[i].szName, szFileName))
            {
                //it has already been created, just open it
                const SharedTexture& Info = pTable->TextureInfo[i];
                HANDLE hGlobal = Info.hTexture;
                
                return m_pDevice->CreateTexture(Info.nWidth,
                                                Info.nHeight,
                                                Info.nMipLevels,
                                                Info.dwUsage,
                                                Info.Format,
                                                Info.Pool,
                                                ppOut,
                                                &hGlobal);
            }
        }

        

        //it has not already been created, create it
        CComPtr<IDirect3DTexture9> pTex;

        HANDLE hShared = NULL;
        HRESULT hr = CreateATexture(szFileName,
                                    &hShared,
                                    &pTex.p);
        
        if(FAILED(hr))
        {
            return hr;
        }

        D3DSURFACE_DESC Desc;
        if(FAILED(pTex->GetLevelDesc(0, &Desc)))
        {
            return E_FAIL;
        }
        
        
        if(pTable->nCount < (SHARED_TEXTURE_COUNT-1))
        {
            UINT nIndex = pTable->nCount;
            SharedTexture& Info = pTable->TextureInfo[nIndex];
            wcscpy(Info.szName, szFileName);
            Info.hTexture = hShared;
            Info.nWidth = Desc.Width;
            Info.nHeight = Desc.Height;
            Info.nMipLevels = pTex->GetLevelCount(),
            Info.dwUsage = Desc.Usage;
            Info.Format = Desc.Format;
            Info.Pool = Desc.Pool;

            pTable->nCount++;
        }

        *ppOut = pTex.Detach();
        return S_OK;
    }

    //if we get here then m_bUseCrossProc is false
    return CreateATexture(szFileName,
                          NULL,
                          ppOut);
    
}


HRESULT CBillBoard::CreateATexture(const wchar_t* szFileName,
                                   HANDLE* pSharedOut,
                                   IDirect3DTexture9** ppOut)
{
    HRESULT hr;
    
    D3DXIMAGE_INFO ImageInfo;
    hr = D3DXGetImageInfoFromResource(NULL, szFileName, &ImageInfo);
    if(FAILED(hr))
    {
        return hr;
    }
    
    
    

    //create a system memory version (needed because D3DXCreateTextureFromFile doesn't allow x-proc
    CComPtr<IDirect3DTexture9> pSysMem;
    hr = D3DXCreateTextureFromResourceEx(m_pDevice,
                                        NULL,
                                        szFileName,
                                        (UINT) (ImageInfo.Width * m_fTextureScale),//width
                                        (UINT) (ImageInfo.Height * m_fTextureScale),//height
                                        D3DX_DEFAULT,//mip levels
                                        0,//usage
                                        D3DFMT_A8R8G8B8,//format,
                                        D3DPOOL_SYSTEMMEM,
                                        D3DX_DEFAULT,//filter
                                        D3DX_DEFAULT,//mip filter
                                        0,//colorkey
                                        NULL,//image info
                                        NULL,//palette
                                        &pSysMem.p);
    if(FAILED(hr))
    {
        return hr;
    }                                             
                                             

    //create a video memory version
    D3DSURFACE_DESC Desc;
    if(FAILED(pSysMem->GetLevelDesc(0, &Desc)))
    {
        return E_FAIL;
    }


    if(pSharedOut)
    {
        *pSharedOut = NULL;//this means it will be filled with the global handle
    }

    CComPtr<IDirect3DTexture9> pVidMem;
    hr = m_pDevice->CreateTexture(Desc.Width,
                                  Desc.Height,
                                  pSysMem->GetLevelCount(),
                                  Desc.Usage,
                                  Desc.Format,
                                  D3DPOOL_DEFAULT,
                                  &pVidMem.p,
                                  pSharedOut);
    if(FAILED(hr))
    {
        return hr;
    }                       
    

    //copy from system to video
    hr = m_pDevice->UpdateTexture(pSysMem,
                                  pVidMem);
    if(FAILED(hr))
    {
        return hr;
    }
    
    *ppOut = pVidMem.Detach();
    
    return S_OK;
}


void CBillBoard::PreWalk(CLogWriter* pLog)
{    
    m_pDevice = CComPtr<IDirect3DDevice9Ex>( GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex") );    

    m_fTextureScale = GetParam<float>(L"TextureScale");

    m_bUseCrossProc = GetParam<bool>(L"UseXProc");

    bool bForeground = GetParam<bool>(L"Foreground");

    bool bPresent = GetParam<bool>(L"Present");

    bool bGPUPriority = GetParam<bool>(L"GPUPriority");

    if(!CheckCaps())
    {
        pLog->Skip(L"Device not supported");
        return;
    }
    
    LARGE_INTEGER Freq;
    if(!QueryPerformanceFrequency(&Freq))
    {
        pLog->Skip(L"QueryPerformanceCounter not supported");
        return;
    }

    m_QPCFreq = Freq.QuadPart;
    
    PumpMessages();
    
    LoadResources();

    const UINT nSeconds = GetParam<UINT>(L"Timeout");

    const UINT nCPUPriorityFreq = GetParam<UINT>(L"CPUPriorityChangeFreq");
    
    m_nSleepTime = GetParam<UINT>(L"SleepTime");

    const UINT nFullScreenFreq = GetParam<UINT>(L"FullScreenFreq");
    
    const bool bWindowed = GetParam<bool>(L"Windowed");

    const bool bVaryLatency = GetParam<bool>(L"VaryFrameLatency");

    // If CountOfProcesses is zero, we're not going to turn on the CrossProcSharing functionality.

    std::string SyncKey = GetParam<std::string>(L"SyncKeyString");
    ULONG ulCount = GetParam<UINT>(L"CountOfProcesses");
    CXProcSyncClient* pxProcSync=NULL; //((char*)SyncKey.c_str(),ulCount); 

    if( ulCount )
    {
        pxProcSync = (CXProcSyncClient*) new CXProcSyncClient((char*)SyncKey.c_str(),ulCount);
    }

    UINT uiQuadrantLoc = GetParam<UINT>(L"QuadrantLoc");

    /*
      Bring the rendering window to the foreground so that it is not clipped
    */
    D3DDEVICE_CREATION_PARAMETERS DeviceParams;
    if(FAILED(m_pDevice->GetCreationParameters(&DeviceParams)))
    {
        throw std::exception("GetCreationParameters failed");
    }

    if( uiQuadrantLoc )
    {
        m_pWindowLoc = new CQuadrant(DeviceParams.hFocusWindow,
                                     uiQuadrantLoc,
                                     SIDE_PIXELS);
    }

    //remember the device adapter ordinal and back buffer size
    m_nAdapterOrdinal = DeviceParams.AdapterOrdinal;

    //new scope to make sure that pBackBuffer is release before Render is called (so that 
    //we can recover from a lost device)
    {
        CComPtr< IDirect3DSurface9> pBackBuffer;
        H_CHECK( m_pDevice->GetRenderTarget(0, &pBackBuffer.p ) );
    
        //this assumes the device starts out in windowed mode
        H_CHECK( pBackBuffer->GetDesc(&m_BackBufferDesc) );
    }
    

    //determine if the device was created windowed or fullscreen
    D3DPRESENT_PARAMETERS PresentParams;
    
    CComPtr<IDirect3DSwapChain9Ex> pSwapChain;
    H_CHECK( GetSwapChainEx(m_pDevice, 0, &pSwapChain.p ) );
    
    H_CHECK( pSwapChain->GetPresentParameters( &PresentParams ) );

    m_bWindowed = (TRUE == PresentParams.Windowed);

    if(bWindowed != m_bWindowed)
    {
        //the user requested the opposite of what we have
        ResetDevice( !m_bWindowed );
        m_bWindowed = !m_bWindowed;
    }
    

    
    //if we are to vary cpu priority, we do it from another, high priority thread
    //so that the act of change cpu priority doesn't get starved
    if(0 != nCPUPriorityFreq)
    {
        PriorityChangeData Info;
        Info.hProcess = ::GetCurrentProcess();
        Info.hThread = ::GetCurrentThread();
        Info.nFrequency = nCPUPriorityFreq;
        
        LaunchThread(Info, CPriorityChanger());
    }
    

    

    HWND hForeground = bForeground ? DeviceParams.hFocusWindow : GetForegroundWindow();

    //::SetWindowPos(DeviceParams.hFocusWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    if(NULL != hForeground)
    {
        if(!::SetForegroundWindow(hForeground))
        {
            throw std::exception("SetForegroundWindow failed");
        }
    }


    m_hRenderWnd = DeviceParams.hFocusWindow; 

    if(!bPresent)
    {
        ::ShowWindow(m_hRenderWnd, SW_MINIMIZE);
    }

    time_t StartTime = time(NULL);
    time_t Diff;
    
    time_t LastFullScreenChangeTime = StartTime;

    //set bottom GPU priority offset
    //we can't go all the way down to -7
    //because that causes priority = 0
    //which causes starvation
    const INT MinGPUPriority = -5;
    const INT MaxGPUPriority = 7;

    if(bGPUPriority)
    {
        if(FAILED(m_pDevice->SetGPUThreadPriority(MinGPUPriority)))
        {
            throw std::exception("Failed to set minimum GPU priority");
        }
    }

    if(bVaryLatency)
    {
        if(FAILED(m_pDevice->SetMaximumFrameLatency(0)))
        {
            throw std::exception("Failed to set initial maximum frame latency");
        }
    }

    if(bPresent)
    {
        ArrangeWindows();
    }

    if( pxProcSync )
    {
        // We're synchronizing cross process, we'll only want to work when the manager tells
        // us too.
        STATE_TYPE stRequest = pxProcSync->TransitionTo(eStateReady);

        // We're going to loop here as long as the manager doesn't ask us to terminate.
        while( stRequest != eStateTerminate )
        {
            bool bExit=false;
            //------------------------------------------------------------------
            do
            {
                // if we have a problem with our device, we want to gracefully shutdown our
                // crossprocsharing stuff.
                try
                {
                    for(int i = 0; i < 10; i++)
                    {
                        PumpMessages();
                        Render(true);
                    }

                    time_t CurrentTime = time(NULL);
                    Diff = CurrentTime - StartTime;

                    if(0 != nFullScreenFreq)
                    {
                        if( (CurrentTime != LastFullScreenChangeTime) && ( ((UINT)(CurrentTime - LastFullScreenChangeTime) >= nFullScreenFreq ) ) )
                        {
                            ResetDevice( !m_bWindowed );
                            m_bWindowed = !m_bWindowed;
                            LastFullScreenChangeTime = CurrentTime;
                        }
                    }

                    //vary gpu thread priority
                    if(bGPUPriority)
                    {
                        float fPriority = (float)Diff / (float)nSeconds;
                        INT nPriority = (INT) ((MaxGPUPriority - MinGPUPriority) * fPriority);
                        nPriority += MinGPUPriority;

                        nPriority = max(MinGPUPriority, nPriority);
                        nPriority = min(MaxGPUPriority, nPriority);

                        if(FAILED(m_pDevice->SetGPUThreadPriority(nPriority)))
                        {
                            throw std::exception("Failed to set GPU thread priority");
                        }
                    }

                    if(bVaryLatency)
                    {
                        UINT Latency = (UINT) (((float)Diff / (float)nSeconds) * 10);
                        Latency = min(10, Latency);

                        if(FAILED(m_pDevice->SetMaximumFrameLatency(Latency)))
                        {
                            throw std::exception("SetMaximumFrameLatency failed");
                        }
                    }

                    // Check to see if the manager wants us to finish
                    stRequest = pxProcSync->CheckForRequest();
                }
                catch(std::exception& e)
                {
                    cout << e.what();
                    bExit=true;
                }
                catch(...)
                {
                    cout << "Caught an exception in render loop\n";
                    bExit=true;
                }    

            } while(((UINT)Diff < nSeconds) &&
                    (stRequest == eStateRun) && 
                    !bExit);
            //------------------------------------------------------------------

            // Note that if we time out in the above loop, our state will be eStateRun. Thus
            // we will not ask for a state change. Because of this, the manager timeout should
            // be shorter than the timeout here.
            if( stRequest != eStateRun )
            {
                // The Manager has asked us to transition into another state, let's do that.
                // If that request is to go into a ready state, we'll go there and loop until
                // the manager switches us back into a Running state. If we do get switched 
                // back into a running state, we're going to perform the same workload that we
                // did originally.
                stRequest = pxProcSync->TransitionTo(eStateReady);
            }
            // if we error out through the try/catch block, bExit will be true thus
            // we'll want to shutdown our crossprocsharing stuff.
            if( stRequest == eStateRun && bExit )
            {
                pxProcSync->TransitionTo(eStateTerminate);
            }
        }

        if( pxProcSync )
        {
            delete pxProcSync;
        }
    }
    else
    {
        // Perform the standard behavior.
        //----------------------------------------------------------------------
        do
        {
            for(int i = 0; i < 10; i++)
            {
                PumpMessages();
                Render(false);
            }

            time_t CurrentTime = time(NULL);
            Diff = CurrentTime - StartTime;

            if(0 != nFullScreenFreq)
            {
                if( (CurrentTime != LastFullScreenChangeTime) && ( ((UINT)(CurrentTime - LastFullScreenChangeTime) >= nFullScreenFreq ) ) )
                {
                    ResetDevice( !m_bWindowed );
                    m_bWindowed = !m_bWindowed;
                    LastFullScreenChangeTime = CurrentTime;
                }
            }

            //vary gpu thread priority
            if(bGPUPriority)
            {
                float fPriority = (float)Diff / (float)nSeconds;
                INT nPriority = (INT) ((MaxGPUPriority - MinGPUPriority) * fPriority);
                nPriority += MinGPUPriority;

                nPriority = max(MinGPUPriority, nPriority);
                nPriority = min(MaxGPUPriority, nPriority);

                if(FAILED(m_pDevice->SetGPUThreadPriority(nPriority)))
                {
                    throw std::exception("Failed to set GPU thread priority");
                }
            }

            if(bVaryLatency)
            {
                UINT Latency = (UINT) (((float)Diff / (float)nSeconds) * 10);
                Latency = min(10, Latency);

                if(FAILED(m_pDevice->SetMaximumFrameLatency(Latency)))
                {
                    throw std::exception("SetMaximumFrameLatency failed");
                }
            }

        } while((UINT)Diff < nSeconds);
        //----------------------------------------------------------------------

    }

    if( m_pWindowLoc )
    {
        delete m_pWindowLoc;
    }
}


void CBillBoard::PostWalk(CLogWriter* pLog)
{    
    CleanupResources();

    m_pDevice.Release();

}


void CBillBoard::PumpMessages()
{
    MSG Message;
    while(PeekMessage(&Message,
                      NULL,
                      0,
                      0,
                      PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
}


void CBillBoard::ArrangeWindows()
{
    if(!m_bWindowed)
    {
        //no point if we are already windowed
        return;
    }

    if( m_pWindowLoc )
    {
        m_pWindowLoc->UseQuadrant();
        return;
    }
    
    m_Siblings.resize(0);
    ::EnumWindows(EnumWindowsCB, (LPARAM)this);
    
    //m_Siblings now includes all rendering windows (including m_hRenderWnd)
    

    if(!::TileWindows(NULL,
                      MDITILE_HORIZONTAL | MDITILE_VERTICAL,
                      NULL,
                      m_Siblings.size(),
                      VectorToPtr(m_Siblings)))
    {
        //this is expected to fail if any app has exclusive access
        DWORD dwErr = GetLastError();
        OutputDebugString(ToString<DWORD>(dwErr).c_str());
        OutputDebugString(L"\nSiblingSize: ");

        OutputDebugString(ToString<DWORD>(m_Siblings.size()).c_str());
        OutputDebugString(L"\n");
    }
}

BOOL CALLBACK EnumWindowsCB(HWND hWnd, LPARAM lParam)
{
    CBillBoard* pBillboard = (CBillBoard*)lParam;
    pBillboard->OnEnumWindow(hWnd);

    return TRUE;
}


void CBillBoard::OnEnumWindow(HWND hWnd)
{
    const UINT CHARS = 1024;
    
    wchar_t szRenderWindow[CHARS];
    wchar_t szTargetWindow[CHARS];
    
    if(GetWindowText(m_hRenderWnd, szRenderWindow, CHARS))
    {
        if(GetWindowText(hWnd, szTargetWindow, CHARS))
        {
            if(0 == wcscmp(szRenderWindow, szTargetWindow))
            {
                m_Siblings.push_back(hWnd);
            }
        }
    }
}


void CBillBoard::CleanupResources()
{
    {
        //remove everything from the shared texture table (even if we didn't create it)
        //this would be better if only the resources created by this process were cleared
        LOCK_TABLE();
        ZeroMemory(m_SharedResources.GetPtr(), sizeof(SharedTextureTable));
    }
    
    m_pTerrain = std::auto_ptr<CD3DMesh>();
    m_pSkyBox = std::auto_ptr<CD3DMesh>();

    for(UINT i = 0; i < NUMTREETEXTURES; i++)
    {
        m_pTreeTextures[i].Release();
    }

    m_pTreeVB.Release();
}

void CBillBoard::ResetDevice(bool bWindowed)
{
    //destroy all vidmem resources
    CleanupResources();

    HRESULT hr = WindowThread_TestCooperativeLevel(GetSemantic<CWindowThread*>(L"windowthread"),
                                                   m_pDevice);

    while(hr == D3DERR_DEVICELOST)
    {
        Sleep(100);

        hr = WindowThread_TestCooperativeLevel(GetSemantic<CWindowThread*>(L"windowthread"),
                                               m_pDevice);
    }

    if(FAILED(hr) && (hr != D3DERR_DEVICENOTRESET))
    {
        throw std::exception( "TestCooperativeLevel failed" );
    }

    //get the old presentation parameters
    //we only try to restore to those
    D3DPRESENT_PARAMETERS PresentParams;
    D3DDISPLAYMODE AdapterMode;
    
    
    CComPtr<IDirect3DSwapChain9Ex> pSwapChain;
    H_CHECK( GetSwapChainEx( m_pDevice, 0, &pSwapChain.p ) );
    H_CHECK( pSwapChain->GetPresentParameters( &PresentParams ) );

    CComPtr<IDirect3D9> pD3D;
    H_CHECK( m_pDevice->GetDirect3D( &pD3D.p ) );

    int iCount=0;
    do
    {
        iCount++;
        hr = pD3D->GetAdapterDisplayMode( m_nAdapterOrdinal, &AdapterMode );

        // Note that error code 0x8876086a is D3DERR_NOTAVAILABLE (look at just the bottom 3 digits)
        Sleep(2000);
        if( iCount > 20 )
        {
            throw std::exception( "GetAdapterDisplayMode fails to return success" );
        }
    }
    while(FAILED(hr));

    //in windowed mode, we want the window to have
    //a title bar etc,
    //in full-screen mode, we don't
    DWORD dwWindowStyle;
    
    if(bWindowed)
    {
        PresentParams.BackBufferWidth = m_BackBufferDesc.Width;
        PresentParams.BackBufferHeight = m_BackBufferDesc.Height;
        PresentParams.BackBufferFormat = m_BackBufferDesc.Format;
        PresentParams.Windowed = TRUE;
        PresentParams.FullScreen_RefreshRateInHz = 0;
        dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
    }
    else
    {
        //full-screen devices can't have alpha in the back buffer
        D3DFORMAT Fmt = m_BackBufferDesc.Format;
        if(D3DFMT_A8R8G8B8 == Fmt)
        {
            Fmt = D3DFMT_X8R8G8B8;
        }

        
        PresentParams.BackBufferWidth = AdapterMode.Width;
        PresentParams.BackBufferHeight = AdapterMode.Height;
        PresentParams.BackBufferFormat = Fmt;
        PresentParams.Windowed = FALSE;
        PresentParams.hDeviceWindow = m_hRenderWnd;
        PresentParams.FullScreen_RefreshRateInHz = AdapterMode.RefreshRate;
        dwWindowStyle = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
    }


    H_CHECK( WindowThread_Reset(GetSemantic<CWindowThread*>(L"windowthread"), m_pDevice, &PresentParams) );

    SetWindowLong( m_hRenderWnd, GWL_STYLE, dwWindowStyle );

    if(bWindowed)
    {
        //When going to fullscreen, d3d makes the appwindow a top-most window
        //this brings it back down
        SetWindowPos(m_hRenderWnd, 
                     HWND_NOTOPMOST,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    
                     
    
    //re-load all vidmem resources
    LoadResources();

    
    
}



bool CBillBoard::CheckCaps()
{
    DWORD dwBehavior;
    D3DDEVICE_CREATION_PARAMETERS CreationParams;
    D3DDISPLAYMODE DisplayMode;
    D3DSURFACE_DESC BBDesc;
    CComPtr<IDirect3D9> pD3D;
    CComPtr<IDirect3DSurface9> pBB;
    

    H_CHECK(m_pDevice->GetDeviceCaps(&m_Caps));
    H_CHECK(m_pDevice->GetCreationParameters(&CreationParams));
    H_CHECK(m_pDevice->GetDirect3D(&pD3D.p));
    H_CHECK(pD3D->GetAdapterDisplayMode(CreationParams.AdapterOrdinal, &DisplayMode));
    H_CHECK(m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB.p));
    H_CHECK(pBB->GetDesc(&BBDesc));

    dwBehavior = CreationParams.BehaviorFlags;
    
    if( dwBehavior & D3DCREATE_PUREDEVICE )
    {//OS bug 1075565
        //return false; // GetTransform doesn't work on PUREDEVICE
    }
    

    // Need to support post-pixel processing (for alpha blending)
    if( FAILED( pD3D->CheckDeviceFormat( m_Caps.AdapterOrdinal, 
                                         m_Caps.DeviceType,
                                         DisplayMode.Format,
                                         D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                                         D3DRTYPE_SURFACE, 
                                         BBDesc.Format ) ) )
    {
        return false;
    }

    // This sample uses alpha textures and/or straight alpha. Make sure the
    // device supports them
    if( m_Caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
        return true;
    if( m_Caps.TextureCaps & D3DPTEXTURECAPS_ALPHA )
        return true;

    return false;
}


void CBillBoard::LoadResources()
{
    m_pTerrain = std::auto_ptr<CD3DMesh>( new CD3DMesh() );
    if(NULL == m_pTerrain.get())
    {
        throw std::bad_alloc();
    }
    
    m_pSkyBox = std::auto_ptr<CD3DMesh>( new CD3DMesh() );
    if(NULL == m_pSkyBox.get())
    {
        throw std::bad_alloc();
    }

    
    // Create the tree textures
    for( DWORD i=0; i<NUMTREETEXTURES; i++ )
    {
        m_pTreeTextures[i].Release();
        
        // Create the texture using D3DX
        if( FAILED( this->CreateTexture(g_strTreeTextures[i],
                                        &m_pTreeTextures[i].p ) ) )
        {
            std::wstring sError(L"Failed to load texture: ");
            sError += g_strTreeTextures[i];
            throw wexception(sError.c_str());
        }
    }

    InitTrees();

    // Create a quad for rendering each tree
    m_pTreeVB.Release();
    H_CHECK(m_pDevice->CreateVertexBuffer( NUM_TREES*4*sizeof(TREEVERTEX),
                                           D3DUSAGE_WRITEONLY, 
                                           TREEVERTEX::FVF,
                                           D3DPOOL_DEFAULT, 
                                           &m_pTreeVB.p, 
                                           NULL));
    

    // Copy tree mesh data into vertexbuffer
    TREEVERTEX* v;
    H_CHECK(m_pTreeVB->Lock( 0, 0, (void**)&v, 0 ));

    INT iTree;
    DWORD dwOffset = 0;
    for( iTree = 0; iTree < NUM_TREES; iTree++ )
    {
        memcpy( &v[dwOffset], m_Trees[iTree].v, 4*sizeof(TREEVERTEX) );
        m_Trees[iTree].dwOffset = dwOffset;
        dwOffset += 4;
    }

    H_CHECK(m_pTreeVB->Unlock());

    // Load the skybox
    if( FAILED( m_pSkyBox->Create( m_pDevice, "SkyBox2.x", this ) ) )
    {
        throw std::exception("Failed to load SkyBox2.x");
    }

    // Load the terrain
    if( FAILED( m_pTerrain->Create( m_pDevice, "SeaFloor.x", this ) ) )
    {
        throw std::exception("Failed to load Seafloor.x");
    }
    
    

    // Add some "hilliness" to the terrain
    CComPtr<IDirect3DVertexBuffer9> pVB;
    if( SUCCEEDED( m_pTerrain->GetSysMemMesh()->GetVertexBuffer( &pVB.p ) ) )
    {
        struct VERTEX { FLOAT x,y,z,tu,tv; };
        VERTEX* pVertices;
        DWORD   dwNumVertices = m_pTerrain->GetSysMemMesh()->GetNumVertices();

        H_CHECK(pVB->Lock( 0, 0, (void**)&pVertices, 0 ));

        for( DWORD i=0; i<dwNumVertices; i++ )
            pVertices[i].y = HeightField( pVertices[i].x, pVertices[i].z );

        H_CHECK(pVB->Unlock());
    }


    m_pTerrain->RestoreDeviceObjects( m_pDevice );
    m_pSkyBox->RestoreDeviceObjects( m_pDevice );


    
    CComPtr<IDirect3DSurface9> pBB;
    H_CHECK(m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB.p));

    D3DSURFACE_DESC Desc;
    H_CHECK(pBB->GetDesc(&Desc));
    
    

    // Set the transform matrices (view and world are updated per frame)
    D3DXMATRIXA16 matProj;
    FLOAT fAspect = Desc.Width / (FLOAT)Desc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    

    m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up the default texture states
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

    m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );
}


void CBillBoard::Render(bool bExpectingDeviceLost)
{
    LARGE_INTEGER QPC;
    QueryPerformanceCounter(&QPC);
    
    m_fTime = (float) ((double)QPC.QuadPart / (double)m_QPCFreq);

    FrameMove();

    m_pDevice->Clear(0 , NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pDevice->BeginScene() ) )
    {
        // Render the Skybox
        {
            // Center view matrix for skybox and disable zbuffer
            D3DXMATRIXA16 matView, matViewSave;
            m_pDevice->GetTransform( D3DTS_VIEW, &matViewSave );
            matView = matViewSave;
            matView._41 = 0.0f; matView._42 = -3.0f; matView._43 = 0.0f;
            m_pDevice->SetTransform( D3DTS_VIEW,      &matView );
            m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
            // Some cards do not disable writing to Z when 
            // D3DRS_ZENABLE is FALSE. So do it explicitly
            m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

            // Render the skybox
            m_pSkyBox->Render( m_pDevice );

            // Restore the render states
            m_pDevice->SetTransform( D3DTS_VIEW,      &matViewSave );
            m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
        }

        // Draw the terrain
        m_pTerrain->Render( m_pDevice );

        // Draw the trees
        DrawTrees();

        // End the scene.
        m_pDevice->EndScene();
    }

    HRESULT hr = m_pDevice->Present(0, 0, 0, 0);
    if(FAILED(hr))
    {
        if(bExpectingDeviceLost)
        {
            ResetDevice(m_bWindowed);
        }
        else
        {
            std::wstring sError( L"Present Failed " );
            sError += ToString<HRESULT>(hr);
            
            throw wexception( sError.c_str() );
        }
    }
    else
    {
        if(0 != m_nSleepTime)
        {
            Sleep(m_nSleepTime);
        }
    }
}

void CBillBoard::DrawTrees()
{
    // Set diffuse blending for alpha set in vertices.
    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Enable alpha testing (skips pixels with less than a certain alpha.)
    if( m_Caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
    {
        m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        m_pDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
        m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
    }

    // Loop through and render all trees
    m_pDevice->SetStreamSource( 0, m_pTreeVB, 0, sizeof(TREEVERTEX) );
    m_pDevice->SetFVF( TREEVERTEX::FVF );
    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Quick culling for trees behind the camera
        // This calculates the tree position relative to the camera, and
        // projects that vector against the camera's direction vector. A
        // negative dot product indicates a non-visible tree.
        if( 0 > ( m_Trees[i].vPos.x - m_vEyePt.x ) * g_vDir.x + 
            ( m_Trees[i].vPos.z - m_vEyePt.z ) * g_vDir.z )
        {
            break;
        }
        
        // Set the tree texture
        m_pDevice->SetTexture( 0, m_pTreeTextures[m_Trees[i].dwTreeTexture] );

        // Translate the billboard into place
        m_matBillboardMatrix._41 = m_Trees[i].vPos.x;
        m_matBillboardMatrix._42 = m_Trees[i].vPos.y;
        m_matBillboardMatrix._43 = m_Trees[i].vPos.z;
        m_pDevice->SetTransform( D3DTS_WORLD, &m_matBillboardMatrix );

        // Render the billboard
        m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_Trees[i].dwOffset, 2 );
    }

    // Restore state
    D3DXMATRIXA16  matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
}

void CBillBoard::FrameMove()
{
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;

    vEyePt.x = 30.0f*cosf( 0.8f * ( m_fTime ) );
    vEyePt.z = 30.0f*sinf( 0.8f * ( m_fTime ) );
    vEyePt.y = 4 + HeightField( vEyePt.x, vEyePt.z );

    vLookatPt.x = 30.0f*cosf( 0.8f * ( m_fTime + 0.5f ) );
    vLookatPt.z = 30.0f*sinf( 0.8f * ( m_fTime + 0.5f ) );
    vLookatPt.y = vEyePt.y - 1.0f;

    // Set the app view matrix for normal viewing
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXVECTOR3 vDir = vLookatPt - vEyePt;
    if( vDir.x > 0.0f )
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    else
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
    g_vDir   = vDir;

    // Sort trees in back-to-front order
    qsort( m_Trees, NUM_TREES, sizeof(Tree), TreeSortCB );

    // Store vectors used elsewhere
    m_vEyePt = vEyePt;
}



//-----------------------------------------------------------------------------
// Name: IsTreePositionValid
// Desc: Verifies that the tree at the given index is sufficiently spaced
//       from the other trees. If trees are placed too closely, one tree
//       can quickly pop in front of the other as the camera angle changes.
//-----------------------------------------------------------------------------
bool CBillBoard::IsTreePositionValid(DWORD nTree)
{
    FLOAT x = m_Trees[nTree].vPos.x;
    FLOAT z = m_Trees[nTree].vPos.z;

    for(DWORD i=0; i < nTree; i++)
    {
        double fDeltaX = fabs( x - m_Trees[i].vPos.x );
        double fDeltaZ = fabs( z - m_Trees[i].vPos.z );

        if( 3.0 > pow( fDeltaX, 2 ) + pow( fDeltaZ, 2 ) )
            return FALSE;
    }

    return TRUE;
}


void CBillBoard::InitTrees()
{
    // Initialize the tree data
    for(UINT i=0; i<NUM_TREES; i++)
    {
        do
        {
            // Position the trees randomly
            FLOAT fTheta  = 2.0f*D3DX_PI*(FLOAT)rand()/RAND_MAX;
            FLOAT fRadius = 25.0f + 55.0f * (FLOAT)rand()/RAND_MAX;
            m_Trees[i].vPos.x  = fRadius * sinf(fTheta);
            m_Trees[i].vPos.z  = fRadius * cosf(fTheta);
            m_Trees[i].vPos.y  = HeightField( m_Trees[i].vPos.x, m_Trees[i].vPos.z );
        }
        while( !IsTreePositionValid( i ) );

        // Size the trees randomly
        FLOAT fWidth  = 1.0f + 0.2f * (FLOAT)(rand()-rand())/RAND_MAX;
        FLOAT fHeight = 1.4f + 0.4f * (FLOAT)(rand()-rand())/RAND_MAX;

        // Each tree is a random color between red and green
        DWORD r = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD g = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD b = 0;
        DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);

        m_Trees[i].v[0].p     = D3DXVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_Trees[i].v[0].color = dwColor;
        m_Trees[i].v[0].tu    = 0.0f;   m_Trees[i].v[0].tv = 1.0f;
        m_Trees[i].v[1].p     = D3DXVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[1].color = dwColor;
        m_Trees[i].v[1].tu    = 0.0f;   m_Trees[i].v[1].tv = 0.0f;
        m_Trees[i].v[2].p     = D3DXVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_Trees[i].v[2].color = dwColor;
        m_Trees[i].v[2].tu    = 1.0f;   m_Trees[i].v[2].tv = 1.0f;
        m_Trees[i].v[3].p     = D3DXVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[3].color = dwColor;
        m_Trees[i].v[3].tu    = 1.0f;   m_Trees[i].v[3].tv = 0.0f;

        // Pick a random texture for the tree
        m_Trees[i].dwTreeTexture = (DWORD) (rand() % NUMTREETEXTURES);
    }
}


//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------
int __cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 )
{
    Tree* p1 = (Tree*)arg1;
    Tree* p2 = (Tree*)arg2;
    
    FLOAT d1 = p1->vPos.x * g_vDir.x + p1->vPos.z * g_vDir.z;
    FLOAT d2 = p2->vPos.x * g_vDir.x + p2->vPos.z * g_vDir.z;
    if (d1 < d2)
        return +1;

    return -1;
}



//////////////////////////////////////////////////
//CD3DMesh, taken straight from the SDX sample framework
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DMesh::CD3DMesh( LPCTSTR strName )
{
    _tcsncpy( m_strName, strName, sizeof(m_strName) / sizeof(TCHAR) );
    m_strName[sizeof(m_strName) / sizeof(TCHAR) - 1] = _T('\0');
    m_pSysMemMesh        = NULL;
    m_pLocalMesh         = NULL;
    m_dwNumMaterials     = 0L;
    m_pMaterials         = NULL;
    m_pTextures          = NULL;
    m_bUseMaterials      = TRUE;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DMesh::~CD3DMesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::Create( LPDIRECT3DDEVICE9 pd3dDevice, 
                          LPCSTR strFilename,
                          CTextureCreator* pTexCreator)
{
    TCHAR        strPath[MAX_PATH];
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT      hr = S_OK;


    // Load the mesh
    if( FAILED( hr = D3DXLoadMeshFromXResource( NULL,
                                                strFilename, 
                                                "X",
                                                D3DXMESH_SYSTEMMEM, 
                                                pd3dDevice, 
                                                &pAdjacencyBuffer, 
                                                &pMtrlBuffer, 
                                                NULL,
                                                &m_dwNumMaterials,
                                                &m_pSysMemMesh ) ) )
    {
        return hr;
    }

    // Optimize the mesh for performance
    if( FAILED( hr = m_pSysMemMesh->OptimizeInplace(
                                                    D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                                                    (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
    {
        SAFE_RELEASE( pAdjacencyBuffer );
        SAFE_RELEASE( pMtrlBuffer );
        return hr;
    }

    // Get material info for the mesh
    // Get the array of materials out of the buffer
    if( pMtrlBuffer && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
        if( m_pMaterials == NULL )
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            m_pTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
            if( m_pTextures == NULL )
            {
                hr = E_OUTOFMEMORY;
            }
            else
            {
                // Copy each material and create its texture
                for( DWORD i=0; i<m_dwNumMaterials; i++ )
                {
                    // Copy the material
                    m_pMaterials[i]         = d3dxMtrls[i].MatD3D;
                    m_pTextures[i]          = NULL;

                    // Create a texture
                    if( d3dxMtrls[i].pTextureFilename )
                    {
                        std::wstring sWideFileName;
                        ShortToWide(d3dxMtrls[i].pTextureFilename, sWideFileName);
                
                        if( FAILED( pTexCreator->CreateTexture(sWideFileName.c_str(),
                                                               &m_pTextures[i] ) ) )
                        {
                            hr = E_FAIL;
                            m_pTextures[i] = NULL;
                        }
                    }
                }
            }
        }
    }

    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pMtrlBuffer );

    return hr;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::SetFVF( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwFVF )
{
    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

    if( m_pSysMemMesh )
    {
        if( FAILED( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_SYSTEMMEM, dwFVF,
                                                 pd3dDevice, &pTempSysMemMesh ) ) )
            return E_FAIL;
    }
    if( m_pLocalMesh )
    {
        if( FAILED( m_pLocalMesh->CloneMeshFVF( 0L, dwFVF, pd3dDevice,
                                                &pTempLocalMesh ) ) )
        {
            SAFE_RELEASE( pTempSysMemMesh );
            return E_FAIL;
        }
    }

    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );

    if( pTempSysMemMesh ) m_pSysMemMesh = pTempSysMemMesh;
    if( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

    // Compute normals in case the meshes have them
    if( m_pSysMemMesh )
        D3DXComputeNormals( m_pSysMemMesh, NULL );
    if( m_pLocalMesh )
        D3DXComputeNormals( m_pLocalMesh, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CD3DMesh::SetVertexDecl
// Desc: Convert the mesh to the format specified by the given vertex
//       declarations.
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::SetVertexDecl( LPDIRECT3DDEVICE9 pd3dDevice, D3DVERTEXELEMENT9 *pDecl )
{
    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

    if( m_pSysMemMesh )
    {
        if( FAILED( m_pSysMemMesh->CloneMesh( D3DXMESH_SYSTEMMEM, pDecl,
                                              pd3dDevice, &pTempSysMemMesh ) ) )
            return E_FAIL;
    }

    if( m_pLocalMesh )
    {
        if( FAILED( m_pLocalMesh->CloneMesh( 0L, pDecl, pd3dDevice,
                                             &pTempLocalMesh ) ) )
        {
            SAFE_RELEASE( pTempSysMemMesh );
            return E_FAIL;
        }
    }

    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );

    if( pTempSysMemMesh )
    {
        m_pSysMemMesh = pTempSysMemMesh;
        // Compute normals in case the meshes have them
        D3DXComputeNormals( m_pSysMemMesh, NULL );
    }

    if( pTempLocalMesh )
    {
        m_pLocalMesh = pTempLocalMesh;
        // Compute normals in case the meshes have them
        D3DXComputeNormals( m_pLocalMesh, NULL );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( NULL == m_pSysMemMesh )
        return E_FAIL;

    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    if( FAILED( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_MANAGED, m_pSysMemMesh->GetFVF(),
                                             pd3dDevice, &m_pLocalMesh ) ) )
        return E_FAIL;

    return S_OK;

}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pLocalMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::Destroy()
{
    InvalidateDeviceObjects();
    for( UINT i=0; i<m_dwNumMaterials; i++ )
        SAFE_RELEASE( m_pTextures[i] );
    SAFE_DELETE_ARRAY( m_pTextures );
    SAFE_DELETE_ARRAY( m_pMaterials );

    SAFE_RELEASE( m_pSysMemMesh );

    m_dwNumMaterials = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DMesh::Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bDrawOpaqueSubsets,
                          bool bDrawAlphaSubsets )
{
    if( NULL == m_pLocalMesh )
        return E_FAIL;

    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_bUseMaterials )
            {
                if( m_pMaterials[i].Diffuse.a < 1.0f )
                    continue;
                pd3dDevice->SetMaterial( &m_pMaterials[i] );
                pd3dDevice->SetTexture( 0, m_pTextures[i] );
            }
            m_pLocalMesh->DrawSubset( i );
        }
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a == 1.0f )
                continue;

            // Set the material and texture
            pd3dDevice->SetMaterial( &m_pMaterials[i] );
            pd3dDevice->SetTexture( 0, m_pTextures[i] );
            m_pLocalMesh->DrawSubset( i );
        }
    }

    return S_OK;
}






void SetCPUPriority(UINT nPriority, HANDLE hProcess, HANDLE hThread)
{
    //threads have priorities, and processes do to
    //total priority is a combination of both
    //map nPriority to values for the process and thread priorities
    //this mapping is in the MSDN page called Scheduling Priorities
    DWORD dwProcessClass;//process priority class
    int   nThreadLevel;//thread priority level

    
    switch(nPriority)
    {
    case 1:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_IDLE;
        break;
        
    case 2:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_LOWEST;
        break;
        
    case 3:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case 4:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_NORMAL;
        break;

    case 5:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case 6:
        dwProcessClass = IDLE_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_HIGHEST;
        break;

    case 7:
        dwProcessClass = NORMAL_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_NORMAL;
        break;

    case 8:
        dwProcessClass = NORMAL_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case 9:
        dwProcessClass = NORMAL_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_HIGHEST;
        break;

    case 10:
        dwProcessClass = NORMAL_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case 11:
        dwProcessClass = HIGH_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_LOWEST;
        break;

    case 12:
        dwProcessClass = HIGH_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case 13:
        dwProcessClass = HIGH_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_NORMAL;
        break;

    case 14:
        dwProcessClass = HIGH_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case 15:
        dwProcessClass = HIGH_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_HIGHEST;
        break;

    case 16:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_IDLE;
        break;

    case 17:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = -7;
        break;

    case 18:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = -6;
        break;

    case 19:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = -5;
        break;

    case 20:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = -4;
        break;

    case 21:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = -3;
        break;

    case 22:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_LOWEST;
        break;

    case 23:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case 24:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_NORMAL;
        break;

    case 25:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case 26:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_HIGHEST;
        break;

    case 27:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = 3;
        break;

    case 28:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = 4;
        break;

    case 29:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = 5;
        break;

    case 30:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = 6;
        break;

        
    case 31:
        dwProcessClass = REALTIME_PRIORITY_CLASS;
        nThreadLevel = THREAD_PRIORITY_TIME_CRITICAL;
        break;

    default:
        throw std::exception("Invalid priority level");
    }

    if(!::SetThreadPriority(hThread, nThreadLevel))
    {
        throw std::exception("SetThreadPriority failed");
    }

    if(!::SetPriorityClass(hProcess, dwProcessClass))
    {
        throw std::exception("SetPriorityClass failed");
    }
}
