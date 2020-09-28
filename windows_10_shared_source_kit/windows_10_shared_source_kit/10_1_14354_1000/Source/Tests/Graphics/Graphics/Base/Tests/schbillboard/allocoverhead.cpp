/******************************Module*Header*******************************
 * Module Name: allocoverhead.cpp
 *
 * Implementation of tests that measure cpu overhead
 * involved in locking, unlocking, creating, destroying, opening, and closing resources
 *
 * Author: Blake Pelton [blakep]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#include "allocoverhead.h"
#include "d3d9.h"
#include "d3dx9.h"
#include <fstream>
#include "comptr.h"

const size_t NOISE_ITERATIONS = 5;


/*
  Name: CDestroyOverhead
  Desc: A class that creates a resource, uses it, and then destroys it
*/
class CDestroyOverhead
{
public:
    /*
      bShared<IN>: If true, then the the create call will open a pre-existing resource
      and the destroy call will close that resource
    */
    CDestroyOverhead(IDirect3DDevice9* pDevice, bool bShared);
    ~CDestroyOverhead();
    
    /*
      Name: Create
      Desc: Creates a resource
    */
    void Create();

    /*
      Name: Use
      Desc: Uses a resource 

      n<IN>: The amount of GPU time that should be spent using the resource.  
      Greater numbers will yield a longer time spent in the GPU
    */
    void Use(UINT n);

    /*
      Name: Destroy
      Desc: Destroys the resource
    */
    void Destroy();

private:
    IDirect3DDevice9*  m_pDevice;
    IDirect3DSurface9* m_pSurf;
    IDirect3DSurface9* m_pMasterSurf;//if m_bShared = true, then this is the master copy
    HANDLE m_hSharedHandle;//if m_bShared = true, then this is the x-proc handle
    bool m_bShared;

    IDirect3DSurface9* m_pBltTarget;
};

/*
  Name: CDynamicLock
  Desc: A class that locks dynamic resources
*/
class CDynamicLock
{
public:
    CDynamicLock(IDirect3DDevice9* pDevice, DWORD dwLockFlags);
    ~CDynamicLock();

    void Use(UINT nQuads);
    void Lock();
    void Unlock();
    void BeginFrame();
    void EndFrame();

private:
    IDirect3DDevice9* m_pDevice;
    DWORD m_dwLockFlags;
    IDirect3DTexture9* m_pTex;
    IDirect3DTexture9* m_pDummyTex;
};


/*
  Name: CCreateOverhead
  Desc: A class that creates resources
*/
class CCreateOverhead
{
public:
    CCreateOverhead(IDirect3DDevice9* pDevice, bool bShared);
    ~CCreateOverhead();
    
    void Use(UINT nQuads);//note that this call to use doesn't actually use the resource created in Create()
    void Create();
    void Destroy();

private:
    IDirect3DSurface9* m_pMaster;//only used for shared allocations
    IDirect3DSurface9* m_pSurf;
    IDirect3DDevice9*  m_pDevice;
    bool m_bShared;
    HANDLE m_hShared;
};

/*
  Name: PerfCounter
  Desc: Returns the number of cycles that have elapsed since startup
*/
ULONGLONG PerfCounter();

/*
  Name: CCPUCycles
  Desc: A class that measures and averages the number of cpu cycles that an operation takes
*/
class CCPUCycles
{
public:
    /*
      nIterations = The number of times Start/End will be called between each call to Reset
    */
    CCPUCycles(size_t nIterations);
    
    /*
      Name: Start
      Desc: Signals the begining of another call
    */
    void Start();

    /*
      Name: End
      Desc: Signals the end of another call
    */
    void End();
    
    /*
      Name: Reset
      Desc: Signals the start of an entirely new test case
    */
    void Reset();

    /*
      Name: GetAverage
      Desc: Returns the average of all the CPU times that were 
      recorded in Start() End() pairs since the last call to Reset
    */
    ULONGLONG GetAverage();

    void PrintMinAndMax();

private:
    std::vector<ULONGLONG> m_Times;
    size_t m_nRecords;
    ULONGLONG m_nLastStart;
};

/*
  Name: RenderQuads
  Desc: Renders a specified number of quads that fill the render target
  This calls SetFVF, and DrawPrimtiveUP

  It does not call BeginScene, EndScene, Clear, Present
*/
void RenderQuads(IDirect3DDevice9* pDevice, UINT nQuads, float fSize = 1.0f);

/*
  Name: OutputResults
  Desc: Prints results to an output file with the format
  szTestCaseName:a,b,c,d
*/
void OutputResults(std::wofstream& OutputStream, const ResultVec& Data, const wchar_t* szTestCaseName);


CAllocOverhead::CAllocOverhead()
{
    SetDescription(L"This component measures cpu overhead associated with various allocation calls");
    
    AddParam<std::wstring>(L"OutputFile", L"The output filename");
    
    SetParamTestDomainSingleValue(L"OutputFile", std::wstring(L"allocoverhead.txt"));
}

void CAllocOverhead::PreWalk(CLogWriter* pLog)
{
    std::wstring sOutputFilename = GetParam<std::wstring>(L"OutputFile");
    
    IDirect3DDevice9Ex* pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");

    std::wofstream LogFile(sOutputFilename.c_str());

    ResultVec Results;

    
    //destroy allocation tests
    DestroyOverheadTest(false, pDevice, Results);
    OutputResults(LogFile, Results, L"DestroyAllocation");
    
    //close allocation tests
    DestroyOverheadTest(true, pDevice, Results);
    OutputResults(LogFile, Results, L"CloseAllocation");

    //lock-discard
    LockOverheadTest(pDevice,
                     D3DLOCK_DISCARD,
                     true,
                     Results);
    OutputResults(LogFile, Results, L"Lock-Discard");


    //lock-discard, no rendering
    LockOverheadTest(pDevice,
                     D3DLOCK_DISCARD,
                     false,
                     Results);
    OutputResults(LogFile, Results, L"Lock-Discard No Rendering");

    LockOverheadTest(pDevice,
                     0,
                     true,
                     Results);
    OutputResults(LogFile, Results, L"Plain Lock");


    LockOverheadTest(pDevice,
                     0,
                     false,
                     Results);
    OutputResults(LogFile, Results, L"Plain Lock no rendering");
    

    CreateOverheadTest(pDevice,
                       false,
                       Results);
    OutputResults(LogFile, Results, L"Create");
    
    CreateOverheadTest(pDevice,
                       true,
                       Results);
    OutputResults(LogFile, Results, L"Open");
}


void CAllocOverhead::PostWalk(CLogWriter* pLog)
{
    
}

void OutputResults(std::wofstream& OutputStream, const ResultVec& Data, const wchar_t* szTestCaseName)
{
    OutputStream << szTestCaseName << L":";

    for(size_t i = 0; i < Data.size(); i++)
    {
        OutputStream << Data[i];
        
        if(i != (Data.size() - 1))
        {
            OutputStream << L",";
        }
    }

    OutputStream << L"\n";
}


//////////////////////////////////////////////////
//Test cases
void CAllocOverhead::DestroyOverheadTest(bool bShared, IDirect3DDevice9* pDevice, ResultVec& Out)
{
    const UINT nIterations = 16;
    Out.resize(nIterations);

    //the number of iterations that each value of i will go through
    //this helps to reduce noise

    
    CDestroyOverhead Test(pDevice, bShared);
    CCPUCycles Timer(NOISE_ITERATIONS);
    

    for(UINT i = 0; i < nIterations; i++)
    {
        for(size_t j = 0; j < NOISE_ITERATIONS; j++)
        {
            Test.Create();
            Test.Use(i);
        
            Timer.Start();
            Test.Destroy();
            Timer.End();
        }

        Out[i] = Timer.GetAverage();
        Timer.Reset();
    }    
}

void CAllocOverhead::LockOverheadTest(IDirect3DDevice9* pDevice,
                                      DWORD dwLockFlags,
                                      bool bRender,
                                      ResultVec& Out)
{
    CDynamicLock Test(pDevice, dwLockFlags);

    const UINT LOCKS_PER_FRAME = 64;
    const UINT MAX_QUADS = 200;//this must be big enough so that when the renaming limit is reached, we are waiting on the GPU
    
    Out.resize(MAX_QUADS);
    CCPUCycles Timer(LOCKS_PER_FRAME);

    //warmup
    for(UINT x = 0; x < 100; x++)
    {
        Test.BeginFrame();
        Test.Lock();
        Test.Unlock();

        if(bRender)
        {
            Test.Use(1);
        }
        
        Test.EndFrame();
    }
    

    for(UINT nQuads = 0; nQuads < MAX_QUADS; nQuads++)
    {
        //sleep so that we aren't affected by the previous iteration (the GPU becomes idle)
        Sleep(30);


        Test.BeginFrame();

        for(UINT i = 0; i < LOCKS_PER_FRAME; i++)
        {
            Timer.Start();
            Test.Lock();
            Timer.End();
        
            Test.Unlock();
        
            if(bRender)
            {
                Test.Use(nQuads + 1);
            }
        }

        Test.EndFrame();


        Out[nQuads] = Timer.GetAverage();

        //Timer.PrintMinAndMax();
        Timer.Reset();
    }
}

void CAllocOverhead::CreateOverheadTest(IDirect3DDevice9* pDevice,
                                        bool bShared,
                                        ResultVec& Out)
{
    CCreateOverhead Test(pDevice, bShared);
    
    const UINT MAX_QUADS = 100;
    const UINT ITERATIONS = 6;

    Out.resize(MAX_QUADS);
    CCPUCycles Timer(ITERATIONS);
    
    //warmup
    for(UINT i = 0; i < 100; i++)
    {
        Test.Use(1);
        Test.Create();
        Test.Destroy();
    }

    for(UINT i = 0; i < MAX_QUADS; i++)
    {
        for(UINT j = 0; j < ITERATIONS; j++)
        {
            Test.Use(i);
            
            Timer.Start();
            Test.Create();
            Timer.End();
            
            Test.Destroy();
        }

        Out[i] = Timer.GetAverage();
        Timer.Reset();
    }
}




//////////////////////////////////////////////////
//globals for shared resource info
const UINT RESOURCE_WIDTH = 2048;
const UINT RESOURCE_HEIGHT = 2048;
const D3DFORMAT RESOURCE_FMT = D3DFMT_X8R8G8B8;
const D3DMULTISAMPLE_TYPE RESOURCE_MS_TYPE = D3DMULTISAMPLE_NONE;
const UINT RESOURCE_MS_QUALITY = 0;
const BOOL RESOURCE_LOCKABLE = TRUE;



CDestroyOverhead::CDestroyOverhead(IDirect3DDevice9* pDevice, bool bShared)
{
    assert(NULL != pDevice);
    
    m_pDevice = pDevice;
    m_pDevice->AddRef();

    m_pSurf = NULL;
    m_pMasterSurf = NULL;
    m_hSharedHandle = NULL;
    m_bShared = bShared;
    m_pBltTarget = NULL;
    

    //create the blt target
    if(FAILED(m_pDevice->CreateRenderTarget(RESOURCE_WIDTH,
                                            RESOURCE_HEIGHT,
                                            RESOURCE_FMT,
                                            RESOURCE_MS_TYPE,
                                            RESOURCE_MS_QUALITY,
                                            RESOURCE_LOCKABLE,
                                            &m_pBltTarget,
                                            NULL)))
    {
        throw std::exception("Failed to create blt target");
    }
    
    
    if(bShared)
    {
        //create the master surface
        HRESULT hr = m_pDevice->CreateRenderTarget(RESOURCE_WIDTH,
                                                   RESOURCE_HEIGHT,
                                                   RESOURCE_FMT,
                                                   RESOURCE_MS_TYPE,
                                                   RESOURCE_MS_QUALITY,
                                                   RESOURCE_LOCKABLE,
                                                   &m_pMasterSurf,
                                                   &m_hSharedHandle);
    
        if(FAILED(hr))
        {
            throw std::exception("Failed to create a master render target");
        }
    }
}

CDestroyOverhead::~CDestroyOverhead()
{
    if(m_pDevice)
    {
        m_pDevice->Release();
    }

    if(m_pSurf)
    {
        m_pSurf->Release();
    }

    if(m_pMasterSurf)
    {
        m_pMasterSurf->Release();
    }

    if(m_pBltTarget)
    {
        m_pBltTarget->Release();
    }
    
}

void CDestroyOverhead::Create()
{
    assert(NULL == m_pSurf);
    assert(NULL != m_pDevice);

    //if we are in x-proc mode, then open rather than create
    HANDLE* pSharedHandle;
    if(m_bShared)
    {
        pSharedHandle = &m_hSharedHandle;
    }
    else
    {
        pSharedHandle = NULL;
    }
    
    
    //create a render target
    HRESULT hr = m_pDevice->CreateRenderTarget(RESOURCE_WIDTH,
                                               RESOURCE_HEIGHT,
                                               RESOURCE_FMT,
                                               RESOURCE_MS_TYPE,
                                               RESOURCE_MS_QUALITY,
                                               RESOURCE_LOCKABLE,
                                               &m_pSurf,
                                               pSharedHandle);
    
    if(FAILED(hr))
    {
        throw std::exception("Failed to create a render target");
    }
}

void CDestroyOverhead::Use(UINT n)
{
    for(UINT i = 0; i < (n * 3); i++)
    {
        m_pDevice->StretchRect(m_pSurf, NULL, m_pBltTarget, NULL, D3DTEXF_NONE);
    }
}

void CDestroyOverhead::Destroy()
{
    assert(NULL != m_pSurf);
    
    m_pSurf->Release();
    m_pSurf = NULL;
}


//////////////////////////////////////////////////
CDynamicLock::CDynamicLock(IDirect3DDevice9* pDevice, DWORD dwLockFlags)
{
    m_pDevice = NULL;
    m_dwLockFlags = 0;
    m_pTex = NULL;
    m_pDummyTex = NULL;
   
    assert(NULL != pDevice);

    m_pDevice = pDevice;
    m_pDevice->AddRef();

    m_dwLockFlags = dwLockFlags;

    if(FAILED(m_pDevice->CreateTexture(RESOURCE_WIDTH/64,
                                       RESOURCE_HEIGHT/64,
                                       1,
                                       D3DUSAGE_DYNAMIC,
                                       RESOURCE_FMT,
                                       D3DPOOL_DEFAULT,
                                       &m_pTex,
                                       NULL)))
    {
        throw std::exception("Failed to create a dynamic texture");
    }

    if(FAILED(m_pDevice->CreateTexture(1,
                                       1,
                                       1,
                                       0,
                                       RESOURCE_FMT,
                                       D3DPOOL_DEFAULT,
                                       &m_pDummyTex,
                                       NULL)))
    {
        throw std::exception("Failed to create dummy texture");
    }
}

CDynamicLock::~CDynamicLock()
{
    if(m_pDevice)
    {
        m_pDevice->Release();
    }

    if(m_pTex)
    {
        m_pTex->Release();
    }

    if(m_pDummyTex)
    {
        m_pDummyTex->Release();
    }
}

void CDynamicLock::Use(UINT nQuads)
{
    m_pDevice->SetTexture(0, m_pTex);

    RenderQuads(m_pDevice, nQuads, 0.2f);//the 0.2f has to be just right so that you don't drastically make the gpu work too hard, this will vary from gpu to gpu
    
    //we need to set the dummy texture here to work around an R200 bug where the driver
    //would reference the texture even when it was not used
    m_pDevice->SetTexture(0, m_pDummyTex);
    RenderQuads(m_pDevice, 1, 0.1f);
}

void CDynamicLock::Lock()
{
    assert(NULL != m_pTex);
    
    D3DLOCKED_RECT Info;
    
    if(FAILED(m_pTex->LockRect(0,
                               &Info,
                               NULL,
                               m_dwLockFlags)))
    {
        throw std::exception("Failed to lock a dynamic texture");
    }
}

void CDynamicLock::Unlock()
{
    assert(NULL != m_pTex);
    
    if(FAILED(m_pTex->UnlockRect(0)))
    {
        throw std::exception("Failed to unlock a dynamic texture");
    }
}


void CDynamicLock::BeginFrame()
{
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF0000FF, 1.0f, 0);
    
    m_pDevice->BeginScene();
}


void CDynamicLock::EndFrame()
{
    m_pDevice->EndScene();
    
    //we must call present so that we get a flush
    m_pDevice->Present(0,0,0,0);
}


//////////////////////////////////////////////////
CCreateOverhead::CCreateOverhead(IDirect3DDevice9* pDevice, bool bShared)
{
    m_pMaster = NULL;
    m_pSurf   = NULL;
    m_pDevice = pDevice;
    m_bShared = bShared;
    m_hShared = NULL;
    
    if(m_bShared)
    {
        HRESULT hr = m_pDevice->CreateRenderTarget(RESOURCE_WIDTH,
                                                   RESOURCE_HEIGHT,
                                                   RESOURCE_FMT,
                                                   RESOURCE_MS_TYPE,
                                                   RESOURCE_MS_QUALITY,
                                                   RESOURCE_LOCKABLE,
                                                   &m_pMaster,
                                                   &m_hShared);
    
        if(FAILED(hr))
        {
            throw std::exception("Failed to create a master render target");
        }
    }
    
}

CCreateOverhead::~CCreateOverhead()
{
    if(m_pMaster)
    {
        m_pMaster->Release();
    }

    if(m_pSurf)
    {
        m_pSurf->Release();   
    }
}


void CCreateOverhead::Use(UINT nQuads)
{
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF0000FF, 1.0f, 0);
    
    m_pDevice->BeginScene();

    RenderQuads(m_pDevice, nQuads);

    m_pDevice->EndScene();

    m_pDevice->Present(0,0,0,0);
}

void CCreateOverhead::Create()
{
    if(m_pSurf)
    {
        m_pSurf->Release();
        m_pSurf = NULL;
    }

    HANDLE* pSharedHandle = NULL;
    
    if(m_bShared)
    {
        pSharedHandle = &m_hShared;
    }

    HRESULT hr = m_pDevice->CreateRenderTarget(RESOURCE_WIDTH,
                                               RESOURCE_HEIGHT,
                                               RESOURCE_FMT,
                                               RESOURCE_MS_TYPE,
                                               RESOURCE_MS_QUALITY,
                                               RESOURCE_LOCKABLE,
                                               &m_pSurf,
                                               pSharedHandle);
    
    if(FAILED(hr))
    {
        throw std::exception("Failed to create a render target");
    }
}

void CCreateOverhead::Destroy()
{
    assert(NULL != m_pSurf);
    
    m_pSurf->Release();
    m_pSurf = NULL;
}





//////////////////////////////////////////////////
//utilities
ULONGLONG PerfCounter()
{
#if defined(_X86_)
    _asm
    {
        rdtsc//this fills edx:eax with the time stamp counter
        }
#else
    LARGE_INTEGER Ret;
    QueryPerformanceCounter(&Ret);
    return Ret.QuadPart;
#endif
}

CCPUCycles::CCPUCycles(size_t nIterations)
{
    assert(0 != nIterations);
    
    m_Times.resize(nIterations);

    m_nRecords = 0;

    m_nLastStart = 0;
}

void CCPUCycles::Start()
{
    assert(m_nRecords < m_Times.size());
    
    m_nLastStart = PerfCounter();
}

void CCPUCycles::End()
{
    const ULONGLONG Current = PerfCounter();
    const ULONGLONG Diff = Current - m_nLastStart;
    m_Times[m_nRecords] = Diff;
    m_nRecords = (m_nRecords + 1);
}

void CCPUCycles::Reset()
{
    m_nRecords = 0;
}

ULONGLONG CCPUCycles::GetAverage()
{
    if(0 == m_nRecords)
    {
        return 0;
    }
    else
    {
        ULONGLONG Sum = 0;
        for(size_t i = 0; i < m_nRecords; i++)
        {
            Sum += m_Times[i];
        }

        return Sum / m_nRecords;
    }
}

void CCPUCycles::PrintMinAndMax()
{
    ULONGLONG Min = 0xFFFFFFFFFFFFFFFF;
    ULONGLONG Max = 0;

    for(size_t i = 0; i < m_nRecords; i++)
    {
        if(m_Times[i] < Min)
        {
            Min = m_Times[i];
        }
        if(m_Times[i] > Max)
        {
            Max = m_Times[i];
        }
    }

    char msg[256];
    sprintf(msg, "Min: %i, Max: %i\n", (int)Min, (int)Max);
    OutputDebugStringA(msg);
}

//////////////////////////////////////////////////
struct RHWVertex
{
    D3DXVECTOR4 vPos;
    DWORD dwColor;
};
const DWORD RHW_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

void RenderQuads(IDirect3DDevice9* pDevice, UINT nQuads, float fSize)
{
    if(0 == nQuads)
    {
        return;
    }
    
    CComPtr<IDirect3DSurface9> pRT;
    if(FAILED(pDevice->GetRenderTarget(0, &pRT.p)))
    {
        throw std::exception("Failed to get the current render target");
    }

    D3DSURFACE_DESC Desc;
    if(FAILED(pRT->GetDesc(&Desc)))
    {
        throw std::exception("Failed to get render target description");
    }

    std::vector<RHWVertex> Vertices(nQuads * 6);
    RHWVertex* pVerts = &(Vertices[0]);
    
    
    for(UINT nQuad = 0; nQuad < nQuads; nQuad++)
    {
        RHWVertex* pStart = &(pVerts[nQuad * 6]);

        //upper left
        pStart[0].vPos.x = 0.0f;
        pStart[0].vPos.y = 0.0f;

        //upper right
        pStart[1].vPos.x = ((float)Desc.Width) * fSize;
        pStart[1].vPos.y = 0.0f;

        //lower left
        pStart[2].vPos.x = 0.0f;
        pStart[2].vPos.y = ((float)Desc.Height) * fSize;

        //lower left
        pStart[3].vPos.x = 0.0f;
        pStart[3].vPos.y = ((float)Desc.Height) * fSize;

        //upper right
        pStart[4].vPos.x = ((float)Desc.Width) * fSize;
        pStart[4].vPos.y = 0.0f;

        //lower right
        pStart[5].vPos.x = ((float)Desc.Width) * fSize;
        pStart[5].vPos.y = ((float)Desc.Height) * fSize;
        
        //fill in all data that is constant over a quad
        for(UINT i = 0; i < 6; i++)
        {
            pStart[i].dwColor = 0xFFFF0000;//red
            pStart[i].vPos.z = 1.0f - (nQuad / (float)nQuads);//back to front rendering
            pStart[i].vPos.w = 1.0f;
        }
    }

    
    pDevice->SetFVF(RHW_FVF);
    
    //draw red
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                             nQuads * 2,
                             pVerts,
                             sizeof(RHWVertex));
}
