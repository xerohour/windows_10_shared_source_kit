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

#include "d3d9.h"
#include "d3dx9.h"
#include "time.h"
#include "ComPtr.h"
#include "dx9addwindow.h"//for GetSwapChainEx

#include <math.h>

#include "TestRunner.h"
#include "testcomponent.h"

#include "preempt.h"

#include "EventWatch.h"
#include "EventListening.h"

#include <etwLibEvents.h>
#include <CrossProcSharing.h>
#include <Quadrant.h>

// The following define forces the preempt class to rely on crossprocsharing. to run the 
// test standalone, remove this define.
#define NEED_CROSSPROC_SHARING

extern CProvider etwProvider;

void BuildPreemptBaseCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);


// This test framework is built off the idea that the commandline should provide
// key variable units and the code uses those units to vary the workload.
//
// From the command line:
//
// preempt: Run this test.
//
// Triangles: The number of triangles to use. These will be Equilateral triangles unless
//            we need to support other types of triangles (Right, Isosceles, Scalene or 
//            Obtuse).
// ppPrimitive: Pixels per primitive. If we need each triangle to be (about) 1000 
//              pixels in size, we'll draw a triangle that has about 1000 pixels. For
//              an Equilateral triangle, we'll follow the basic rule 
//              Area = (1/2)base*height where base=height. Thus, SQR(2*area)=base. 
//              Area=ppPrimitive.
// ppFrame: Primitives per frame. This number will represent how many triangles
//          we try to process per frame (every 16ms). (Currently not used)
// Time: Time, in seconds, for which the test will run.
// GPUPriority: Should be a value between -5 and 7. This value will be passed to SetGPUThreadPriority().
// PriorityClass: Should be a standard Priority Class value. It is directly related 
//                to the following constants:
//     0x8000 (32768) - ABOVE_NORMAL_PRIORITY_CLASS
//     0x4000 (16384) - BELOW_NORMAL_PRIORITY_CLASS
//      0x100 (256) - REALTIME_PRIORITY_CLASS
//       0x80 (128) - HIGH_PRIORITY_CLASS
//       0x40 (64) - IDLE_PRIORITY_CLASS
//       0x20 (32) - NORMAL_PRIORITY_CLASS
// ThreadPriority: The thread priority options have the following values:
// -15 - THREAD_PRIORITY_IDLE
// -1 - THREAD_PRIORITY_BELOW_NORMAL
// -2 - THREAD_PRIORITY_LOWEST
// 0 - THREAD_PRIORITY_NORMAL
// 1 - THREAD_PRIORITY_ABOVE_NORMAL
// 2 - THREAD_PRIORITY_HIGHEST
// 15 - THREAD_PRIORITY_TIME_CRITICAL
// Note that the Real Time class allows values from -7 to 6.
//
//
// A sample command line might look something like:
//
// SchBillboard Preempt Time:3 Triangles:1000 ppPrimitive:1000 GPUPriority:5 PriorityClass:0x80 ThreadPriority:15
//
// Note that this will run the preempt(ion) test for three seconds with 1000 triangles
// 
//
// Results: The end result is that when DWM is running, it will always look to preempt
// anyone else doing work. The work outlined by the commandline, if it's GPU intensive,
// will setup a situation where DWM will constantly ask for processing time.
//

// Only support EquilateralTriangle for proof of concept
// _PixelsPerTriangle = (1/2)b*b
CTriangle::CTriangle(TriangleType _Type,
                     UINT _iPixelsPerTriangle):
    m_Base(0.0),
    m_iBase(0),
    m_iHeight(0)
{
    int iPixels=0;
    m_Type = _Type;
    switch(m_Type)
    {
    case EquilateralTriangle:
        {
            // Determine the height and base
            m_Base=sqrt((double)(2*_iPixelsPerTriangle));
            m_iBase=(int)m_Base;
            m_iHeight=(int)m_Base;

            iPixels = (int)((m_iBase*m_iHeight)/2);
        }
        break;
    case RightTriangle:
    case IsoscelesTriangle:
    case ScaleneTriangle:
    case ObtuseTriangle:
    default:
        {
            return;
        }
        break;
    }

#ifdef DBG
    WCHAR wszBuf[128];
    StringCchPrintf(wszBuf,128,L"m_Base=%.2f, m_iBase=%d, m_iHeight=%d, effective pixels=%d\n\r",
                    m_Base,m_iBase,m_iHeight,iPixels);
    OutputDebugString(wszBuf);
#endif
}

CTriangle::~CTriangle()
{
}

int 
CTriangle::Base()
{
    return m_iBase;
}

int 
CTriangle::Height()
{
    return m_iHeight;
}

int
CTriangle::Pixels()
{
    return (int)((m_iBase*m_iHeight)/2);
}


CWorkLoad::CWorkLoad(IDirect3DDevice9Ex* _pDevice):
    m_pVertices(NULL)
{
    m_pDevice = _pDevice;
}

CWorkLoad::~CWorkLoad()
{
    m_pVB.Release();
}

void CWorkLoad::Release()
{
    m_pVB.Release();
}

void CWorkLoad::Initialize(CTriangle* _pTriangle,
                           int _iTriangles,
                           bool _bcase)
{
    const int nVertices = _iTriangles * 3;
    const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

    // Create a vextex buffer that will hold the number of triangles that we want.
    H_CHECK(m_pDevice->CreateVertexBuffer(sizeof(Vertex) * nVertices,
                                          D3DUSAGE_WRITEONLY,
                                          FVF,
                                          D3DPOOL_DEFAULT,
                                          &m_pVB.p,
                                          NULL));
#ifdef DBG
    WCHAR wszBuf[128];
    StringCchPrintf(wszBuf,128,L"VertexBufferSize=%d\n\r",
                    sizeof(Vertex) * nVertices);
    OutputDebugString(wszBuf);
#endif
    //fill the VB
    H_CHECK(m_pVB->Lock(0, 0, (void**) &(m_pVertices), 0));
    
    for(int i = 0; i < _iTriangles; i++)
    {
        Vertex* pStart = &m_pVertices[i * 3];
        if( _bcase )
        {
            pStart[0].Pos = D3DXVECTOR4(0.0f, 0.0f, 0.5, 1.0f);
            pStart[1].Pos = D3DXVECTOR4(1.0f, 0.0f, 0.5, 1.0f);
            pStart[2].Pos = D3DXVECTOR4(0.0f, 1.0f, 0.5, 1.0f);
        }
        else
        {
            pStart[0].Pos = D3DXVECTOR4(0.0f, 1.0f, 0.5, 1.0f);
            pStart[1].Pos = D3DXVECTOR4(1.0f, 0.0f, 0.5, 1.0f);
            pStart[2].Pos = D3DXVECTOR4(1.0f, 1.0f, 0.5, 1.0f);        
        }
    }
    
    for(int i = 0; i < nVertices; i++)
    {
        m_pVertices[i].Color = 0xFFFFFFFF;
        m_pVertices[i].Pos.x *= (float)_pTriangle->Base();   //.Width;  // Use TrianglePrim.Base() 
        m_pVertices[i].Pos.y *= (float)_pTriangle->Height(); // ;m_RTDesc.Height; // Use TrianglePrim.Height()
        m_pVertices[i].Pos.z = 1.0f - (i / (float)_iTriangles);//back to front
        m_pVertices[i].Pos.w = 1.0f;
    }    

    H_CHECK(m_pVB->Unlock());

    H_CHECK(m_pDevice->SetFVF(FVF));
}

void
CWorkLoad::SetStreamSource()
{
    H_CHECK(m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Vertex)));
}

IDirect3DDevice9Ex* CWorkLoad::GetDevice()
{
    return m_pDevice;
}


// --------------------------- Preempt -----------------------------------------
//
// This handles the standard Preempt command line option. See usage options at the 
// top of this file.
//

void BuildPreemptCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    nodeStr += L"/preempt";
    _pRunner->AddNode<CPreempt>( nodeStr.c_str() );

    // Get standard options before adding our new ones.
    BuildPreemptBaseCommandOptions(_pRunner, nodeStr);
}


CPreempt::CPreempt()
{
}

void CPreempt::PreWalk(CLogWriter* pLog)
{
    HRESULT hr = S_OK;
    ReadCommandLine(pLog);

    // If we did not find the three parameters on the command line TotalTime, nTriangles or
    // nppPrimitive, we'll need to set the default values.
    if( MAX_UNSIGNED_INT == nTotalTime )
    {
        nTotalTime = 60;
    }
    if( MAX_UNSIGNED_INT == nTriangles )
    {
        nTriangles = 100;
    }
    if( MAX_UNSIGNED_INT == nppPrimitive )
    {
        nppPrimitive = 1000;
    }

    hr = DeterminePriorities();
    if( S_OK != hr )
    {
        return;
    }

    DumpParameters(1);

    pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");
    // Now establish our priorities...

    pDevice->SetGPUThreadPriority(iGPUPriority);

    // Now let's get to work.
    PerformWork();
}


void CPreempt::PostWalk(CLogWriter* pLog)
{
}
// --------------------------- Preempt -----------------------------------------

// ------------------------- AutoPreempt ---------------------------------------
//
// This test will grow the workload automatically until we get at least 10 preemptions
// per second. It's a virtual copy of Preempt above. It will error out at 60 seconds.
// The difference is that we use nTotalTime as the inner loop value and each time through
// the loop we look to increase our workload until we finally find a workload that causes
// enough preemptions.
//
// Command line must have the following items on it:
//
// SchBillboard.exe AutoPreempt listen Preemption:true
//
// The command line may contain any of the parameter outlined for CPrempt.
//
#define SIXTY_SECONDS 60
// 

void BuildAutoPreemptCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    nodeStr += L"/AutoPreempt";
    _pRunner->AddNode<CAutoPreempt>( nodeStr.c_str() );

    // Get standard options before adding our new ones.
    BuildPreemptBaseCommandOptions(_pRunner, nodeStr);
}



CAutoPreempt::CAutoPreempt()
{
}

void CAutoPreempt::PreWalk(CLogWriter* pLog)
{
    HRESULT hr = S_OK;
    ReadCommandLine(pLog);
    WCHAR wszBuf[128];

    // If we did not find the three parameters on the command line TotalTime, nTrianglesor
    // nppPrimitive, we'll need to set the default values.
    if( MAX_UNSIGNED_INT == nTotalTime )
    {
        nTotalTime = 5;
    }
    if( MAX_UNSIGNED_INT == nTriangles )
    {
        nTriangles = 1;
    }
    if( MAX_UNSIGNED_INT == nppPrimitive )
    {
        nppPrimitive = 1;
    }


    GETLISTEMSEMANTIC(COSPreemptionPerSecond,Preemption);

    hr = DeterminePriorities();
    if( S_OK != hr )
    {
        return;
    }

    DumpParameters(1);

    pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");

    // Now establish our priorities...

    pDevice->SetGPUThreadPriority(iGPUPriority);

    // Loop until we get ten preemptions per second generated by the core or fail if
    // we are generating DMA packets that take 1/2 second long to process.

    // To determine success for this test, we're going to look at the last full second
    // in our listener and see if we saw at least 10 preemption packets.
    ULONG ulCountForLastFullSecond = 0;


    time_t AutoStartTime = time(NULL);
    time_t AutoCurrentTime = AutoStartTime;

    EtwMessageTrace(L"Starting AutoPreempt test");

    // The main idea behind this loop is to grow our workload so that the DMA Packets
    // that we put down on the hardware take longer to process so we to increase our
    // probability of a preemption packet.

    do
    {
        StringCchPrintf(wszBuf,128,L"Triangles=%d, ppPrimitive=%d, ppFrame=%d\n\r",
                        nTriangles,nppPrimitive,nppFrame);
        EtwMessageTrace(wszBuf);
        pLog->Message(wszBuf);

        // Now let's get to work.
        PerformWork();

        // Now, check that the data gathered is within bounds...
        if( pPreemption )
        {
            // Get a pointer to our gathered data
            CPerSecond< CPreemption >* pPerSecond = pPreemption->GetSource();
            ulCountForLastFullSecond = pPerSecond->GetCountInLastFullSecond();
            if( ulCountForLastFullSecond >= 10 )
            {
                // We saw 10 preemption packets in the last full second.
                break;
            }
        }
        else
        {
            // we're not listening during this test. we'll just keep on going for the 
            // SIXTY_SECONDS and fail like we should.
        }

        // grow our primitives by ten.
        nTriangles *= 10;
        nppPrimitive *= 10;
        nppFrame *= 10;

        // Update our maxium time that we'll run.
        AutoCurrentTime = time(NULL);

    } while(((UINT)(AutoCurrentTime - AutoStartTime)) <= SIXTY_SECONDS);
    EtwMessageTrace(L"Ending AutoPreempt test");

    StringCchPrintf(wszBuf,128,L"Terminating: CountForLastFulSecond %d",ulCountForLastFullSecond);
    pLog->Message(wszBuf);

    // Did we exceed SIXTY_SECONDS worth of time and still not see preemption packets?
    if( (UINT)(AutoCurrentTime - AutoStartTime) > SIXTY_SECONDS ) 
    {
        // check the count of preemptions. If we did not get 5 in the last second,
        // we didn't hit enough preemptions
        if( ulCountForLastFullSecond < 10 )
        {
            pLog->Fail(L"Sixty Second timeout - Did not see enough preemptions on the GPU.");
        }
    }
    else
    {
        // check the count of preemptions. If we did not get 10 in the last second,
        // we didn't hit enough preemptions
        if( ulCountForLastFullSecond < 10 )
        {
            pLog->Fail(L"Did not see enough preemptions on the GPU.");
        }
    }
}


void CAutoPreempt::PostWalk(CLogWriter* pLog)
{
}
// ------------------------- AutoPreempt ---------------------------------------

//------------------------------------------------------------------------------
//
//  The command line for this test should look something like
//
// SchBillboard Preempter Time:30 PreemptionsPerSecond:150 SyncWith:ABC123 ProcCount:2 
//    GPUPriority:7 ThreadPriority:15 Triangles:1 ppPrimitive:1 PriorityClass:32768

void BuildPreempterCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    nodeStr += L"/preempter";
    _pRunner->AddNode<CPreempter>( nodeStr.c_str() );

    // Get standard options before adding our new ones.
    BuildPreemptBaseCommandOptions(_pRunner, nodeStr);

    // Now gather up our command line options.

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nPreemptionsPerSecond",
                               L"PreemptionsPerSecond",
                               L"The number of preemptions per second" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"SyncKeyString",  // Variable name
                               L"SyncWith",       // CommandLine string
                               L"Unique string to define global shared memory name" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"CountOfProcesses",  // Variable name
                               L"ProcCount",       // CommandLine string
                               L"Number of applications that will be monitored including manager" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nGrowTriangles",
                               L"GrowTriangles",
                               L"The number of triangles to grow every loop" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nGrowppPrimitive",
                               L"GrowppPrimitive",
                               L"number of pixels per Primitive to grow every loop" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nGrowppFrame",
                               L"GrowppFrame",
                               L"number of Primitives per frame to grow every loop" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"bAdd",
                               L"Add",
                               L"If true, add the grow by numbers, else multiply" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"bBackgroundOnly",
                               L"BackgroundOnly",
                               L"If true, work loop does not call present" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"bExpectTDR",
                               L"ExpectTDR",
                               L"If true, work loop will expect a TDR" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"QuadrantLoc",  // Variable name
                               L"Quadrant",       // CommandLine string
                               L"Number either 1,2,3 or 4 - denotes window quadrant to use" );

}


//
CPreempter::CPreempter() :
    m_pWork(NULL),
    m_pTrianglePrim(NULL)
{
    SetDescription(L"This component submits command buffers that take very little rendering time");

    AddParam<UINT>(L"nPreemptionsPerSecond", L"The number of preemptions per second(default 60)");
    AddParam<std::string>(L"SyncKeyString", L"Unique string to define global shared memory name");
    AddParam<UINT>(L"CountOfProcesses", L"Number of applications that will be monitored including manager");
    
    AddParam<UINT>(L"nGrowTriangles", L"The number of triangles to grow every loop");
    AddParam<UINT>(L"nGrowppPrimitive", L"number of pixels per Primitive to grow every loop");
    AddParam<UINT>(L"nGrowppFrame", L"number of Primitives per frame to grow every loop");
    AddParam<bool>(L"bAdd", L"If true, add the grow by numbers, else multiply");
    AddParam<bool>(L"bBackgroundOnly", L"If true, work loop does not call present");
    AddParam<bool>(L"bExpectTDR", L"If true, work loop will expect a TDR");
    AddParam<UINT>(L"QuadrantLoc", L"Number either 1,2,3 or 4 - denotes window quadrant to use");


    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<UINT>(L"nPreemptionsPerSecond", FAST_AS_POSSIBLE);
    SetParamTestDomainSingleValue<std::string>(L"SyncKeyString", "UnknownTest");
    SetParamTestDomainSingleValue<UINT>(L"CountOfProcesses", 0);

    SetParamTestDomainSingleValue<UINT>(L"nGrowTriangles", 0);
    SetParamTestDomainSingleValue<UINT>(L"nGrowppPrimitive", 0);
    SetParamTestDomainSingleValue<UINT>(L"nGrowppFrame", 0);
    SetParamTestDomainSingleValue<bool>(L"bAdd", false);
    SetParamTestDomainSingleValue<bool>(L"bBackgroundOnly", false);
    SetParamTestDomainSingleValue<bool>(L"bExpectTDR", false);
    SetParamTestDomainSingleValue<UINT>(L"QuadrantLoc", 0);
}

extern const char* PSCONST_SHADER;
const DWORD _FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);

//
// This method fills out m_pTrianglePrim and m_pWork
//
CWorkLoad* CPreempter::CreateWorkLoad(LPDIRECT3DDEVICE9EX _pDevice,
                                      UINT _nppPrimitive,
                                      UINT _nTriangles)
{
    // if we already have pointers, clean them up.
    if( m_pWork )
    {
        m_pWork->Release();
        delete m_pWork;
    }
    if( m_pTrianglePrim )
    {
        delete m_pTrianglePrim;
    }

    // Create our workload

    m_pTrianglePrim = new CTriangle(EquilateralTriangle,_nppPrimitive);
    if( m_pTrianglePrim )
    {
        m_pWork = new CWorkLoad(_pDevice);
        if( m_pWork )
        {
            // initialize with the triangle primitive we want to use and the number of triangles.
            m_pWork->Initialize(m_pTrianglePrim,_nTriangles,true);
        }
        else
        {
            PATH_TRAP();
            delete m_pTrianglePrim;
        }
    }

    return m_pWork;
}


void CPreempter::PreWalk(CLogWriter* pLog)
{
    CQuadrant* pWindowLoc = NULL;
    HRESULT hr;
    // Read our standard settings.
    ReadCommandLine(pLog);

    // Now read our speciality items
    nPreemptionsPerSecond = GetParam<UINT>(L"nPreemptionsPerSecond");
    std::string SyncKey = GetParam<std::string>(L"SyncKeyString");
    ULONG ulCount = GetParam<UINT>(L"CountOfProcesses");

    ULONG nGrowTriangles = GetParam<UINT>(L"nGrowTriangles");
    ULONG nGrowppPrimitive = GetParam<UINT>(L"nGrowppPrimitive");
    ULONG nGrowppFrame = GetParam<UINT>(L"nGrowppFrame");

    bool bAdd = GetParam<bool>(L"bAdd");
    bool bBackgroundOnly = GetParam<bool>(L"bBackgroundOnly");
    bool bExpectTDR = GetParam<bool>(L"bExpectTDR");

    UINT uiQuadrantLoc = GetParam<UINT>(L"QuadrantLoc");

    pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");

    //  Bring the rendering window to the foreground so that it is not clipped
    D3DDEVICE_CREATION_PARAMETERS DeviceParams;
    if(FAILED(pDevice->GetCreationParameters(&DeviceParams)))
    {
        throw std::exception("GetCreationParameters failed");
    }

    m_hRenderWnd = DeviceParams.hFocusWindow; 

    if( uiQuadrantLoc )
    {
        pWindowLoc = new CQuadrant(m_hRenderWnd,
                                   uiQuadrantLoc,
                                   SIDE_PIXELS);
    }

    //remember the device adapter ordinal and back buffer size
    m_nAdapterOrdinal = DeviceParams.AdapterOrdinal;

#ifdef NEED_CROSSPROC_SHARING
    // Now setup for xProc Sharing.
    CXProcSyncClient xProcSync((char*)SyncKey.c_str(),ulCount); 
#endif
    // Setup our priorities
    hr = DeterminePriorities();
    if( S_OK != hr )
    {
        return;
    }

    // Now establish our priorities...
    pDevice->SetGPUThreadPriority(iGPUPriority);

    UINT uiDelay=0;
    if( FAST_AS_POSSIBLE != nPreemptionsPerSecond )
    {
        // Calculate a reasonable delay for the nPreemptionsPerSecond request. This will 
        // just be a rough estimate. If they ask for 20 preemptions per second, it means
        // that we will delay 50 ms between requests. If they ask for 500, we will delay
        // 2. But, we'll always say that it takes at least 2 milliseconds to perform a render
        // call. Thus, we'll subtract the constant.
        UINT uiDelay = max((((int)(1000/nPreemptionsPerSecond))-(int)1),0);
    }

    // Now we transition into ready and wait for the manager app to setup for
    // counting and timing.

#ifdef NEED_CROSSPROC_SHARING
    STATE_TYPE stRequest = xProcSync.TransitionTo(eStateReady);
#else
    STATE_TYPE stRequest = eStateRun;
#endif
    // We're going to loop here as long as the manager doesn't ask us to terminate.
    UINT uiLoop=0;
    bool bDeviceLost=false;
    while( stRequest != eStateTerminate && !bDeviceLost)
    {
        //------------- Perform Work -----------
        EventWriteLogMsg(L"In Preempter test initializing triangles");

        // Create our workload and initialize with the triangle primitive we want to use 
        // and the number of triangles.
        m_pWork = CreateWorkLoad(pDevice,nppPrimitive,nTriangles);
        if( NULL == m_pWork )
        {
            PATH_TRAP();
            break;
            // fail gracefully if we can't get a pointer
        }

        IDirect3DDevice9Ex* pDevice = m_pWork->GetDevice();
        //new scope to make sure that pBackBuffer is release before Render is called (so that 
        //we can recover from a lost device)
        {
            CComPtr< IDirect3DSurface9> pBackBuffer;
            H_CHECK( pDevice->GetRenderTarget(0, &pBackBuffer.p ) );

            //this assumes the device starts out in windowed mode
            H_CHECK( pBackBuffer->GetDesc(&m_BackBufferDesc) );
        }

        // Log out our workload each time before we enter the work loop.
        DumpParameters(uiLoop++);

        time_t StartTime = time(NULL);
        time_t CurrentTime = StartTime;

        EventWriteLogMsg(L"In Preempter test at start of do loop");
        do
        {
            if(uiDelay)
            {
                // sleep for the amount of the delay.
                Sleep(uiDelay);
            }

            // For our high priority thread, let's try to interrupt every frame.
            if(bWaitForVBlank)
            {
                pDevice->WaitForVBlank(0);
            }
            
            EventWriteLogMsg(L"Start SetStreamSource");
            m_pWork->SetStreamSource();

            EventWriteLogMsg(L"Start Clear");
            H_CHECK(pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFF00FF, 1.0f, 0));
    
            EventWriteLogMsg(L"Start BeginScene");
            H_CHECK(pDevice->BeginScene());
            for(UINT ui=0;ui<nppFrame;ui++)
            {
                EventWriteLogMsg(L"Start DrawPrimitive");
                H_CHECK(pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                               0,
                                               nTriangles));
#ifdef NEED_CROSSPROC_SHARING
                if( 0 == (ui % 10) )
                {
                    // Check to see if the manager wants us to finish
                    stRequest = xProcSync.CheckForRequest();
                    if( eStateRun != stRequest )
                    {
                        break;
                    }
                }
#endif
            }

            EventWriteLogMsg(L"Start EndScene");
            H_CHECK(pDevice->EndScene());
            
            // If we're running in the background, we don't want to do a present.
            if( !bBackgroundOnly )
            {
                EventWriteLogMsg(L"Call Present");
                hr = pDevice->Present(0,0,0,0);

                if( bExpectTDR && 
                    ((D3DERR_DEVICELOST == hr) || (D3DERR_DEVICEHUNG == hr)) )
                {
                    // we may be expecting this, if so, we don't want to fail.
                    hr = S_OK;
                    bDeviceLost=true;
                }
                else
                {
                    // If we get a device lost and we're not expecting it, we fail (or any other error).
                    H_CHECK(hr);
                }

                EventWriteLogMsg(L"Done with Present");
            }

            CurrentTime = time(NULL);

#ifdef NEED_CROSSPROC_SHARING
            // Check to see if the manager wants us to finish
            stRequest = xProcSync.CheckForRequest();
#endif
            if( bDeviceLost )
            {
                // If we need to reset the device, we better do it before looping.
                ResetDevice(true);

                m_pWork->Initialize(m_pTrianglePrim,nTriangles,true);

                if( uiQuadrantLoc )
                {
                    delete pWindowLoc;
                    pWindowLoc = new CQuadrant(m_hRenderWnd,
                                               uiQuadrantLoc,
                                               SIDE_PIXELS);
                }
                bDeviceLost = false;
            }

        } while( (((UINT)(CurrentTime - StartTime)) <= nTotalTime) &&
                 stRequest == eStateRun);

        EventWriteLogMsg(L"In Preempter test at end of do loop");
        //------------- Perform Work -----------

#ifdef NEED_CROSSPROC_SHARING
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
                // Each time the manager transitions us back to a ready state, we're going
                // to look and see if the user instructed us to grow any of our date items.

                if( nGrowTriangles )
                {
                    // User wants to grow the triangles
                    if(bAdd)
                    {
                        nTriangles += nGrowTriangles;
                    }
                    else
                    {
                        nTriangles *= nGrowTriangles;
                    }
                }
                if( nGrowppPrimitive )
                {
                    // User wants to grow the number of pixels per primitive
                    if(bAdd)
                    {
                        nppPrimitive += nGrowppPrimitive;
                    }
                    else
                    {
                        nppPrimitive *= nGrowppPrimitive;
                    }
                }
                if( nGrowppFrame )
                {
                    // User wants to grow the number of primitives per frame
                    if(bAdd)
                    {
                        nppFrame += nGrowppFrame;
                    }
                    else
                    {
                        nppFrame *= nGrowppFrame;
                    }
                }
            }
        }
#endif
    }

#ifdef NEED_CROSSPROC_SHARING
    // We are done, so we need to tell the manager
    xProcSync.TransitionTo(eStateTerminate);
#endif
    if( pWindowLoc )
    {
        delete pWindowLoc;
    }
}
void CPreempter::PostWalk(CLogWriter* pLog)
{
}


void CPreempter::ResetDevice(bool bWindowed)
{
    //destroy all vidmem resources

    if( m_pWork )
    {
        // Need to call Initialize() before this object is usable again.
        m_pWork->Release();
    }

    IDirect3DDevice9Ex* pDevice = m_pWork->GetDevice();

    HRESULT hr = WindowThread_TestCooperativeLevel(GetSemantic<CWindowThread*>(L"windowthread"),
                                                   pDevice);

    while(hr == D3DERR_DEVICELOST)
    {
        Sleep(100);

        hr = WindowThread_TestCooperativeLevel(GetSemantic<CWindowThread*>(L"windowthread"),
                                               pDevice);
    }

    if(FAILED(hr) && (hr != D3DERR_DEVICENOTRESET))
    {
        PATH_TRAP();
        throw std::exception( "TestCooperativeLevel failed" );
    }

    //get the old presentation parameters
    //we only try to restore to those
    D3DPRESENT_PARAMETERS PresentParams;
    D3DDISPLAYMODE AdapterMode;
    
    
    CComPtr<IDirect3DSwapChain9Ex> pSwapChain;
    H_CHECK( GetSwapChainEx( pDevice, 0, &pSwapChain.p ) );
    H_CHECK( pSwapChain->GetPresentParameters( &PresentParams ) );

    CComPtr<IDirect3D9> pD3D;
    H_CHECK( pDevice->GetDirect3D( &pD3D.p ) );

    int iCount=0;
    do
    {
        iCount++;
        hr = pD3D->GetAdapterDisplayMode( m_nAdapterOrdinal, &AdapterMode );

        // Note that error code 0x8876086a is D3DERR_NOTAVAILABLE (look at just the bottom 3 digits)
        Sleep(2000);
        if( iCount > 20 )
        {
            PATH_TRAP();
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


    H_CHECK( WindowThread_Reset(GetSemantic<CWindowThread*>(L"windowthread"), pDevice, &PresentParams) );

    SetWindowLong( m_hRenderWnd, GWL_STYLE, dwWindowStyle );
}





//------------------------------------------------------------------------------

void BuildPreemptBaseCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr)
{
    // Now gather up our command line options.

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"TotalTime",  // Variable name
                               L"Time",       // CommandLine string
                               L"The number of seconds to run for" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nTriangles",
                               L"Triangles",
                               L"The number of triangles" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nppPrimitive",
                               L"ppPrimitive",
                               L"number of pixels per Primitive" );

    _pRunner->LinkParamToCmdLine( nodeStr.c_str(),
                               L"nppFrame",
                               L"ppFrame",
                               L"number of Primitives per frame" );

    _pRunner->LinkParamToCmdLine(nodeStr.c_str(),
                              L"iGPUPriority",
                              L"GPUPriority",
                              L"Integer from -5 to 7");

    _pRunner->LinkParamToCmdLine(nodeStr.c_str(),
                              L"dwPriorityClass",
                              L"PriorityClass",
                              L"A *_PRIORITY_CLASS value");

    _pRunner->LinkParamToCmdLine(nodeStr.c_str(),
                              L"iThreadPriority",
                              L"ThreadPriority",
                              L"A THREAD_PRIORITY_* value");

    _pRunner->LinkParamToCmdLine(nodeStr.c_str(),
                              L"bWaitForVBlank",
                              L"WaitForVBlank",
                              L"If true, work will be performed after the vsync");
}


CPreemptBase::CPreemptBase()
{
    SetDescription(L"This component submits command buffers that take a large amount of rendering time");

    AddParam<UINT>(L"TotalTime", L"The total number of seconds to run for");
    AddParam<UINT>(L"nTriangles", L"The number of triangles");
    AddParam<UINT>(L"nppPrimitive", L"The number of pixels per primitive");
    AddParam<UINT>(L"nppFrame", L"The number of primitives per frame");
    AddParam<int>(L"iGPUPriority", L"integer from -5 to 7");
    AddParam<DWORD>(L"dwPriorityClass", L"A *_PRIORITY_CLASS value");
    AddParam<int>(L"iThreadPriority", L"A THREAD_PRIORITY_* value");
    AddParam<bool>(L"bWaitForVBlank", L"if true, work will be performed after the vsync");
    
    SetParamTestDomainSingleValue<UINT>(L"TotalTime", MAX_UNSIGNED_INT); //5
    SetParamTestDomainSingleValue<UINT>(L"nTriangles", MAX_UNSIGNED_INT); //1
    SetParamTestDomainSingleValue<UINT>(L"nppPrimitive", MAX_UNSIGNED_INT); //1
    SetParamTestDomainSingleValue<UINT>(L"nppFrame", 1);
    SetParamTestDomainSingleValue<int>(L"iGPUPriority", 0);
    SetParamTestDomainSingleValue<DWORD>(L"dwPriorityClass", NORMAL_PRIORITY_CLASS);
    SetParamTestDomainSingleValue<int>(L"iThreadPriority", THREAD_PRIORITY_NORMAL);
    SetParamTestDomainSingleValue<bool>(L"bWaitForVBlank", false);
}

void CPreemptBase::ReadCommandLine(CLogWriter* _pLog)
{
    pLog = _pLog;
    nTotalTime = GetParam<UINT>(L"TotalTime");
    nTriangles = GetParam<UINT>(L"nTriangles");
    nppPrimitive = GetParam<UINT>(L"nppPrimitive");
    nppFrame = GetParam<UINT>(L"nppFrame");
    iGPUPriority = GetParam<int>(L"iGPUPriority");
    dwPriorityClass = GetParam<DWORD>(L"dwPriorityClass");
    iThreadPriority = GetParam<int>(L"iThreadPriority");
    bWaitForVBlank = GetParam<bool>(L"bWaitForVBlank");
}

void CPreemptBase::DumpParameters(UINT _uiCounter)
{
    WCHAR wszBuf[128];

    // The following outputdebugstring is for dumping the test parameters.
    StringCchPrintf(wszBuf,128,L"%d, TotalTime=%d, Triangles=%d, ppPrimitive=%d, ppFrame=%d",
                    _uiCounter,nTotalTime,nTriangles,nppPrimitive,nppFrame);
    pLog->Message(wszBuf);
#ifdef DBG
    OutputDebugString(wszBuf);
    OutputDebugString(L"\n\r");
#endif

    StringCchPrintf(wszBuf,128,L"%d iGPUPriority=%d, dwPriorityClass=0x%08x, iThreadPriority=%d",
                    _uiCounter,iGPUPriority,dwPriorityClass,iThreadPriority);
    pLog->Message(wszBuf);
#ifdef DBG
    OutputDebugString(wszBuf);
    OutputDebugString(L"\n\r");
#endif
}



HRESULT CPreemptBase::DeterminePriorities()
{
    // The GPU Priority can be a value from -5 to 7. Thus, if the user sends in a value
    // less then or greater then these values, we'll correct it here.
    if(iGPUPriority<-5)
    {
        iGPUPriority=-5;
    }
    if(iGPUPriority>7)
    {
        iGPUPriority=7;
    }

    // The Process Priority Classes have the following values: 
    // 0x8000 (32768) - ABOVE_NORMAL_PRIORITY_CLASS
    // 0x4000 (16384) - BELOW_NORMAL_PRIORITY_CLASS
    //  0x100 (256) - REALTIME_PRIORITY_CLASS
    //   0x80 (128) - HIGH_PRIORITY_CLASS
    //   0x40 (64) - IDLE_PRIORITY_CLASS
    //   0x20 (32) - NORMAL_PRIORITY_CLASS
    if( dwPriorityClass <= 0x20 )
    {
        dwPriorityClass = NORMAL_PRIORITY_CLASS;
    }
    else if (dwPriorityClass <= 0x40 && dwPriorityClass >0x20)
    {
        dwPriorityClass = IDLE_PRIORITY_CLASS;
    }
    else if (dwPriorityClass <= 0x80 && dwPriorityClass >0x40)
    {
        dwPriorityClass = HIGH_PRIORITY_CLASS;
    }
    else if (dwPriorityClass <= 0x100 && dwPriorityClass >0x80)
    {
        dwPriorityClass = REALTIME_PRIORITY_CLASS;
    }
    else if (dwPriorityClass <= 0x4000 && dwPriorityClass >0x100)
    {
        dwPriorityClass = BELOW_NORMAL_PRIORITY_CLASS;
    }
    else if (dwPriorityClass <= 0x8000 && dwPriorityClass >0x4000)
    {
        dwPriorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
    }

    // The thread priority options have the following values:
    // -15 - THREAD_PRIORITY_IDLE
    // -1 - THREAD_PRIORITY_BELOW_NORMAL
    // -2 - THREAD_PRIORITY_LOWEST
    // 0 - THREAD_PRIORITY_NORMAL
    // 1 - THREAD_PRIORITY_ABOVE_NORMAL
    // 2 - THREAD_PRIORITY_HIGHEST
    // 15 - THREAD_PRIORITY_TIME_CRITICAL
    //
    // The Real Time class allows values from -7 to 6.
    // 
    // The following code will correct the bounds to our lowest and highest settings.

    if( dwPriorityClass == REALTIME_PRIORITY_CLASS )
    {
        if( iThreadPriority < -7 )
        {
            iThreadPriority = -7;
        }
        if( iThreadPriority > 6 )
        {
            iThreadPriority = 6;
        }
    }
    else
    {
        if( iThreadPriority < THREAD_PRIORITY_BELOW_NORMAL )
        {
            iThreadPriority = THREAD_PRIORITY_IDLE;
        }
        if( iThreadPriority > THREAD_PRIORITY_HIGHEST )
        {
            iThreadPriority = THREAD_PRIORITY_TIME_CRITICAL;
        }
    }

    HANDLE hProcess = NULL;
    hProcess = GetCurrentProcess();

    if( !SetPriorityClass(hProcess,dwPriorityClass) )
    {
        pLog->Skip(L"SetPriorityClass failed");
        return S_FALSE;
    }

    HANDLE hThread = NULL;
    hThread = GetCurrentThread();

    if( !SetThreadPriority(hThread,iThreadPriority) )
    {
        pLog->Skip(L"SetThreadPriority failed");
        return S_FALSE;
    }
    return S_OK;
}

void CPreemptBase::PerformWork()
{
    CTriangle TrianglePrim(EquilateralTriangle,nppPrimitive);

    // Create our workload
    CWorkLoad WorkLoad(pDevice);

    // initialize with the triangle primitive we want to use and the number of triangles.
    WorkLoad.Initialize(&TrianglePrim,nTriangles,true);

    time_t StartTime = time(NULL);
    time_t CurrentTime = StartTime;

    EtwMessageTrace(L"In Preempt test at start of do loop");
    do
    {
        // For our high priority thread, let's try to interrupt every frame.
        if(bWaitForVBlank)
        {
            pDevice->WaitForVBlank(0);
        }
        EtwMessageTrace(L"Start SetStreamSource");
        WorkLoad.SetStreamSource();
        EtwMessageTrace(L"Start Clear");
        H_CHECK(pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFF00FF, 1.0f, 0));
        EtwMessageTrace(L"Start BeginScene");
        H_CHECK(pDevice->BeginScene());
        EtwMessageTrace(L"Start DrawPrimitive");
        H_CHECK(pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                       0,
                                       nTriangles));
        EtwMessageTrace(L"Start EndScene");
        H_CHECK(pDevice->EndScene());
        EtwMessageTrace(L"Call Present");
        H_CHECK(pDevice->Present(0,0,0,0));
        EtwMessageTrace(L"Done with Present");

        CurrentTime = time(NULL);

    } while(((UINT)(CurrentTime - StartTime)) <= nTotalTime);
    EtwMessageTrace(L"In Preempt test at end of do loop");
}





