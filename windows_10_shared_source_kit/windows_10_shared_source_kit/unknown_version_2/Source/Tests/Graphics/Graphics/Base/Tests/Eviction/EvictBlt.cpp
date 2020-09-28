#include <VidmmRequires.h>
#include "VidmmTestComponent.h"
#include "EvictBlt.h"

using namespace std;

#define MAX_MSG_SIZE 256


// This component test evictions in different conditions
// Create VB, 2D textures, Volume textures

extern bool I3DIsPow2(UINT nIn);
extern bool I3DForcePow2(UINT nIn);
extern void DBGPrint(CLogWriter* pLog, const LPWSTR pwszMessage, ...);
bool Is32BitAddressSpace();


CTestSlowEviction::CTestSlowEviction()
: m_size(0),
  m_fault(0),
  m_bBVT(false)
{
    SetDescription(L"This component tests eviction of large allocations.")  ;

    // Add parameters for the test
    AddParam<bool>(L"PagingPathLockSubRange", L"Causes eviction to happen in chunks");
    AddParam<bool>(L"PagingPathLockMinRange", L"Causes eviction to happen page by page");
    AddParam<LONGLONG>(L"VidMemSize", L"Total size of video memory (local + non-local)");
    AddParam<bool>(L"BVT", L"Whether to run in BVT mode (no allocation pattern checking)");
}


void CTestSlowEviction::RunTest()
{
    HRESULT hr = S_OK;

    m_bBVT = GetParam<bool>(L"BVT");

    // get fault information
    if(GetParam<bool>(L"PagingPathLockSubRange"))
    {
        m_fault |= PagingPathLockSubRangeFault;
    }
    if(GetParam<bool>(L"PagingPathLockMinRange"))
    {
        m_fault |= PagingPathLockMinRangeFault;
    }

    // get total size of video memory
    m_size = GetParam<LONGLONG>(L"VidMemSize");		

    //
    //	If we have 32-bit address space, cap m_size to 1.5 GB to avoid running out of
    //	virtual address space (since we create lockable allocations (on purpose), each 
    //	allocation will have a backing store in processes address space)
    //
    if(Is32BitAddressSpace())
    {
        LONGLONG UserModeVASpace = (LONGLONG)2 * 1024 * 1024 * 1024;	// 2GB
        LONGLONG MaxVAToUse = (LONGLONG)(UserModeVASpace * 0.75);
        if(m_size > MaxVAToUse)
        {
            m_size = MaxVAToUse;
        }
    }

    DBGPrint(m_pLog, L"Total video memory size = %I64d", m_size);

    hr = EvictVolTex();
    if(FAILED(hr) && ((hr != E_OUTOFMEMORY) && (hr != E_UNSUPPORTED)))
    {
        WCHAR err[MAX_MSG_SIZE] = L"";
        wsprintf(err, L"EvictVolTex failed, hr = 0x%X", hr);
        m_pLog->Fail(err);
    }

    //
    //	This test created a lot of allocations to cause evictions, 
    //	with deferred allocation destructions, these allocations might
    //	not have been destroyed. In order for them to be destroyed, we
    //	need to call Present() (that's when terminated allocation list is
    //	processed) to clean up memory for the next subtest
    //
    H_CHECK(m_pDevice->Present());

    //
    //	These tests are time consuming, don't run them all in BVT mode
    //
    if(!m_bBVT)
    {
        hr = EvictVB();
        if(FAILED(hr) && ((hr != E_OUTOFMEMORY) && (hr != E_UNSUPPORTED)))
        {
            WCHAR err[MAX_MSG_SIZE] = L"";
            wsprintf(err, L"EvictVB failed, hr = 0x%X", hr);
            m_pLog->Fail(err);
        }

        //
        //	This test created a lot of allocations to cause evictions, 
        //	with deferred allocation destructions, these allocations might
        //	not have been destroyed. In order for them to be destroyed, we
        //	need to call Present() (that's when terminated allocation list is
        //	processed) to clean up memory for the next subtest
        //
        H_CHECK(m_pDevice->Present());

            
        hr = EvictTex2();
        if(FAILED(hr) && (hr != E_OUTOFMEMORY))
        {
            WCHAR err[MAX_MSG_SIZE] = L"";
            wsprintf(err, L"EvictTex2 failed, hr = 0x%X", hr);
            m_pLog->Fail(err);
        }

        //
        //	This test created a lot of allocations to cause evictions, 
        //	with deferred allocation destructions, these allocations might
        //	not have been destroyed. In order for them to be destroyed, we
        //	need to call Present() (that's when terminated allocation list is
        //	processed) to clean up memory for the next subtest
        //
        H_CHECK(m_pDevice->Present());
    }
}

void CTestSlowEviction::EndTest()
{
}

HRESULT CTestSlowEviction::EvictVB()
{
    HRESULT hr = S_OK;

    D3DCAPS9 caps;
    H_CHECK(I3DGetD3D9Caps(m_pDevice, &caps));

    if(!I3DHWVBSupported(&caps))
    {
        m_pLog->Message(L"HW VB is not supported");
        return E_UNSUPPORTED;
    }

    // if we need to set any faults, do it now
    HANDLE hThread = NULL, hEvent = NULL;
    if(m_fault)
    {
        // Create an event that will be set when test is complete
        hEvent = CreateEvent(NULL, FALSE, FALSE, L"Test Complete Event");
        if(!hEvent)
            DBGPrint(m_pLog, L"Failed to create a test complete event");

        hThread = CreateThread(0, 0, ControlFault, &m_fault, 0, 0);
        if(!hThread)
            DBGPrint(m_pLog, L"Failed to create the fault setting thread");
    }

    // size of 1 VB, will use total of 20 VBs to fill memory
    UINT VBSize = (UINT)(m_size / 20);	
    const UINT nbAllocs = 20;

    // Create vertex buffers and use them, at which point eviction might take place
    CI3DVertexBufferPtr vbSrc[nbAllocs], vbDest[nbAllocs];
    const UINT BASE_PATTERN = 0x12345678;
    int i = 0;
    for(; i < nbAllocs; i++)
    {
        if(FAILED(hr = m_pDevice->CreateVertexBuffer(&vbDest[i].p, VBSize, I3DSURF_WRITEONLY, 0, D3DDDIPOOL_VIDEOMEMORY, NULL)))
        {
            WCHAR msg[MAX_MSG_SIZE] = L"";
            wsprintf(msg, L"Failed to create VB in video memory pool: 0x%X, iteration %d", hr, i);
            m_pLog->Message(msg);
            break;
        }
        try
        {
            H_CHECK(hr = m_pDevice->CreateVertexBuffer(&vbSrc[i].p, VBSize, 0, 0, D3DDDIPOOL_SYSTEMMEM, NULL));

            //
            //	Fill source with pattern
            //
            void* pV;
            H_CHECK(hr = vbSrc[i]->Lock(0, 0, &pV, 0));

            H_CHECK(hr = ResourcePattern(VBSize, (BYTE*)pV, BASE_PATTERN+i, 0, true, false));

            H_CHECK(hr = vbSrc[i]->Unlock());
        }
        catch(std::exception& e)
        {
            WCHAR err[MAX_MSG_SIZE] = L"";
            wsprintf(err, L"Exception is caught: %s\n", e.what());
            OutputDebugString(err);
        }
        if(vbSrc[i].p == NULL)
        {
            OutputDebugString(L"Out of memory\n");
            hr = E_OUTOFMEMORY;
            break;
        }
        
        // Use allocations
        D3DDDIRANGE range;
        range.Offset = 0;
        range.Size = VBSize;
        H_CHECK(m_pDevice->BufBlt(vbSrc[i], vbDest[i], range, 0));
        H_CHECK(m_pDevice->Flush());
        vbSrc[i].Release();

        //
        //	Make sure destination allocation now has the correct pattern
        //
        void* pV;
        H_CHECK(hr = vbDest[i]->Lock(0, 0, &pV, 0));

        H_CHECK(hr = ResourcePattern(VBSize, (BYTE*)pV, BASE_PATTERN+i, 0, false, true));

        H_CHECK(hr = vbDest[i]->Unlock());
    }

    // if we created fault thread, wait for it to exit
    if(hThread)
    {
        // If we have test complete event, set it now
        if(hEvent)
        {
            SetEvent(hEvent);
            CloseHandle(hEvent);
        }

        WaitForSingleObject(hThread, INFINITE);
        
        CloseHandle(hThread);
    }

    i--;
    while(i >= 0)
    {
        //
        //	At this point evictions must have happened, make sure pattern is still correct
        //
        void* pV;
        H_CHECK(hr = vbDest[i]->Lock(0, 0, &pV, 0));

        H_CHECK(hr = ResourcePattern(VBSize, (BYTE*)pV, BASE_PATTERN+i, 0, FALSE, TRUE));

        H_CHECK(hr = vbDest[i]->Unlock());

        vbDest[i].Release();
        i--;
    }

    m_pLog->Message(L"Done with VBs");
    return hr;
}

HRESULT CTestSlowEviction::EvictTex2()
{
    HRESULT hr = S_OK;

    // Find a format
    D3DDDIFORMAT Fmt; 
    if(FAILED(hr = I3DXGetValidResourceFormat(m_pDevice, I3DFORMAT_ARGB, I3DSURF_VOLUME, false, &Fmt)))
        return hr;

    UINT FmtXBitsMask = GetXBitsMask(Fmt);

    // texture dimensions
    UINT tWidth = 1024, tHeight = 1024, pixelSize = 2;	//for I3DFORMAT_ARGB

    // See how many allocations we need to fill memory
    UINT nbAllocs = (UINT)(m_size / (tWidth * tHeight * pixelSize));
    const UINT maxAllocs = 1024;

    if(nbAllocs > maxAllocs)
    {
        nbAllocs = maxAllocs;
    }

    DBGPrint(m_pLog, L"Creating %d textures (%dx%dx%d)", nbAllocs, tWidth, tHeight, pixelSize);

    CI3DTexturePtr t2Src[maxAllocs], t2Dest[maxAllocs];


    // if we need to set any faults, do it now
    HANDLE hThread = NULL, hEvent = NULL;
    if(m_fault)
    {
        // Create an event that will be set when test is complete
        hEvent = CreateEvent(NULL, FALSE, FALSE, L"Test Complete Event");
        if(!hEvent)
            DBGPrint(m_pLog, L"Failed to create a test complete event");

        hThread = CreateThread(0, 0, ControlFault, &m_fault, 0, 0);
        if(!hThread)
            DBGPrint(m_pLog, L"Failed to create the fault setting thread");
    }


    // create textures
    const UINT BASE_PATTERN = 0x1111ffff;

    UINT i = 0;
    for(; i < nbAllocs; i++)
    {	
        if(FAILED(hr = m_pDevice->CreateTexture(&t2Dest[i].p, tWidth, tHeight, 1, 0, Fmt, D3DDDIPOOL_VIDEOMEMORY, NULL)))
        {
            WCHAR msg[MAX_MSG_SIZE] = L"";
            wsprintf(msg, L"Failed to create texture in video memory pool: 0x%X, iteration %d", hr, i);
            m_pLog->Message(msg);
            break;
        }
        try
        {
            H_CHECK(hr = m_pDevice->CreateTexture(&t2Src[i].p, tWidth, tHeight, 1, 0, Fmt, D3DDDIPOOL_SYSTEMMEM, NULL));

            // Fill source allocation with pattern
            CI3DSurface2Ptr pSurface;
            D3DLOCKED_RECT Rect;
            BYTE* pStart = NULL;

            H_CHECK(hr = t2Src[i]->GetSurface(0, &pSurface.p));
            H_CHECK(hr = pSurface->Lock(0, NULL, &Rect));

            pStart = (BYTE*)Rect.pBits;
            
            for(UINT row = 0; row < tHeight; row++)
            {
                H_CHECK(hr = ResourcePattern(tWidth * pixelSize, 
                                             pStart + row * Rect.Pitch,
                                             BASE_PATTERN + i,
                                             FmtXBitsMask,
                                             TRUE,
                                             FALSE));
            }

            H_CHECK(hr = pSurface->Unlock());
        }
        catch(...)
        {
            OutputDebugString(L"Caught an exception\n");
        }
        if(t2Src[i].p == NULL)
        {
            OutputDebugString(L"Out of memory\n");
            hr = E_OUTOFMEMORY;
            break;
        }

        // use allocations
        POINT DstPoint;
        DstPoint.x = 0;
        DstPoint.y = 0;
        
        RECT SrcRect;
        SrcRect.left = 0;
        SrcRect.top = 0;
        SrcRect.right = tWidth;
        SrcRect.bottom = tHeight;
        H_CHECK(m_pDevice->TexBlt(t2Src[i], t2Dest[i], 0, SrcRect, DstPoint));
        H_CHECK(m_pDevice->Flush());
        t2Src[i].Release();


        // Verify pattern of destination allocation
        CI3DSurface2Ptr pSurface;
        D3DLOCKED_RECT Rect;
        BYTE* pStart = NULL;

        H_CHECK(hr = t2Dest[i]->GetSurface(0, &pSurface.p));
        H_CHECK(hr = pSurface->Lock(0, NULL, &Rect));

        pStart = (BYTE*)Rect.pBits;

        for(UINT row = 0; row < tHeight; row++)
        {
            H_CHECK(hr = ResourcePattern(tWidth * pixelSize, 
                                        pStart + row * Rect.Pitch,
                                        BASE_PATTERN + i,
                                        FmtXBitsMask,
                                        FALSE,
                                        TRUE));
        }

        H_CHECK(hr = pSurface->Unlock());

    }

    // if we created fault thread, wait for it to exit
    if(hThread)
    {
        // If we have test complete event, set it now
        if(hEvent)
        {
            SetEvent(hEvent);
            CloseHandle(hEvent);
        }

        WaitForSingleObject(hThread, INFINITE);
        
        CloseHandle(hThread);
    }
    
    i--;
    while(i)
    {
        //
        //	At this point evictions must have happened, make sure pattern is still correct
        //
        CI3DSurface2Ptr pSurface;
        D3DLOCKED_RECT Rect;
        BYTE* pStart = NULL;

        H_CHECK(hr = t2Dest[i]->GetSurface(0, &pSurface.p));
        H_CHECK(hr = pSurface->Lock(0, NULL, &Rect));

        pStart = (BYTE*)Rect.pBits;

        for(UINT row = 0; row < tHeight; row++)
        {
            H_CHECK(hr = ResourcePattern(tWidth * pixelSize, 
                                        pStart + row * Rect.Pitch,
                                        BASE_PATTERN + i,
                                        FmtXBitsMask,
                                        FALSE,
                                        TRUE));
        }

        H_CHECK(hr = pSurface->Unlock());

        t2Dest[i].Release();
        i--;
    }

    m_pLog->Message(L"Done with 2d textures");

    return S_OK;
}

HRESULT CTestSlowEviction::EvictVolTex()
{
    HRESULT hr = S_OK;

    // Find a format
    D3DDDIFORMAT Fmt; 
    if(FAILED(hr = I3DXGetValidResourceFormat(m_pDevice, I3DFORMAT_ARGB, I3DSURF_VOLUME, false, &Fmt)))
        return hr;

    // texture dimensions
    UINT tWidth = 1024, tHeight = 1024, tDepth = 4, pixelSize = 2;	//for I3DFORMAT_ARGB

    // See how many allocations we need to fill memory
    UINT nbAllocs = (UINT)(m_size / (tWidth * tHeight * tDepth * pixelSize));

    const UINT maxAllocs = 1024;

    if(nbAllocs > maxAllocs)
    {
        nbAllocs = maxAllocs;
    }

    DBGPrint(m_pLog, L"Creating %d volume textures (%dx%dx%dx%d)", nbAllocs, tWidth, tHeight, tDepth, pixelSize);

    CI3DVolumeTexturePtr t3Src[maxAllocs], t3Dest[maxAllocs];

    // if we need to set any faults, do it now
    HANDLE hThread = NULL, hEvent = NULL;
    if(m_fault)
    {
        // Create an event that will be set when test is complete
        hEvent = CreateEvent(NULL, FALSE, FALSE, L"Test Complete Event");
        if(!hEvent)
            DBGPrint(m_pLog, L"Failed to create a test complete event");

        hThread = CreateThread(0, 0, ControlFault, &m_fault, 0, 0);
        if(!hThread)
            DBGPrint(m_pLog, L"Failed to create the fault setting thread");
    }


    // create textures 

    UINT i = 0;
    for(; i < nbAllocs; i++)
    {
        if(FAILED(hr = m_pDevice->CreateVolumeTexture(&t3Dest[i].p, tWidth, tHeight, tDepth, 
                                                      1, 0, Fmt, D3DDDIPOOL_VIDEOMEMORY, NULL)))
        {
            WCHAR msg[MAX_MSG_SIZE] = L"";
            wsprintf(msg, L"Failed to create volume texture in video memory pool: 0x%X, iteration %d", hr, i);
            m_pLog->Message(msg);
            break;
        }
        try
        {
            H_CHECK(m_pDevice->CreateVolumeTexture(&t3Src[i].p, tWidth, tHeight, tDepth,
                                               1, 0, Fmt, D3DDDIPOOL_SYSTEMMEM, NULL));
        }
        catch(...)
        {
            OutputDebugString(L"Caught an exception\n");
        }
        if(t3Src[i].p == NULL)
        {
            OutputDebugString(L"Out of memory\n");
            hr = E_OUTOFMEMORY;
            break;
        }


        // use allocations
        D3DDDIBOX srcBox;
        srcBox.Left = 0;
        srcBox.Right = tWidth;
        srcBox.Top = 0;
        srcBox.Bottom = tHeight;
        srcBox.Front = 0;
        srcBox.Back = tDepth;
        H_CHECK(m_pDevice->VolBlt(t3Src[i], t3Dest[i], srcBox, 0, 0, 0));
        H_CHECK(m_pDevice->Flush());
        t3Src[i].Release();
    }

    // if we created fault thread, wait for it to exit
    if(hThread)
    {
        // If we have test complete event, set it now
        if(hEvent)
        {
            SetEvent(hEvent);
            CloseHandle(hEvent);
        }

        WaitForSingleObject(hThread, INFINITE);
        
        CloseHandle(hThread);
    }

    while(i)
    {
        t3Dest[i-1].Release();
        i--;
    }

    return S_OK;
}



HRESULT CTestSlowEviction::ResourcePattern(UINT size, BYTE* Start, DWORD dwFillPattern, DWORD Mask, BOOL bFill, BOOL bCheck)
{
    HRESULT hr = S_OK;

    if(m_bBVT)
    {
        //	Don't do any allocation pattern verification in BVT mode
        return hr;
    }

    if(size)
    {
        if(bFill)
        {
            //
            //  Fill allocation with pattern
            //
            while(size >= sizeof(DWORD))
            {
                memcpy(Start, &dwFillPattern, sizeof(DWORD));
                Start += sizeof(DWORD);
                size -= sizeof(DWORD);
                dwFillPattern++;
            }
        }

        if(bCheck)
        {
            //
            //	 if Mask is provided, we need to ignore the bits set in the mask,
            //	 because driver is not required to maintain them
            //
            Mask = ~Mask;

            while(size >= sizeof(DWORD))
            {
                DWORD Temp = *((DWORD*) Start) & Mask;
                DWORD dwFillPatternWithoutXBits = dwFillPattern & Mask;
                
                if(Temp != dwFillPatternWithoutXBits)
                {
                    DBGPrint(m_pLog, L"Pattern is wrong: 0x%X != 0x%X", Temp, dwFillPatternWithoutXBits);
                    hr = E_FAIL;
                    break;
                }

                Start += sizeof(DWORD);
                size -= sizeof(DWORD);
                dwFillPattern++;
            }
        }
    }

    return hr;
}


void SetFault(UINT mask)
{
    CDDI* ddi = NULL;
    D3DKMT_HANDLE adapter = NULL; 

    // get needed values
    CreateDDI(&ddi);
    
    //get a GDI HDC for the primary adapter
    CGDIDeviceContext DC;
    
    //open an adapter
    D3DKMT_OPENADAPTERFROMHDC info;
    memset(&info, 0, sizeof(info));
    
    info.hDc = DC.GetDC();  

    if(!NT_SUCCESS(ddi->KmOpenAdapterFromHDC(&info)))
    {
        goto exit;
    }
    
    adapter = info.hAdapter;

    // set fault based on mask
    D3DKMT_ESCAPE esc_info;
    memset(&esc_info, 0, sizeof(esc_info));

    D3DKMT_VIDMM_ESCAPE faults;
    memset(&faults, 0, sizeof(faults));
    
    esc_info.hAdapter = adapter;
    esc_info.Type = D3DKMT_ESCAPE_VIDMM;
    esc_info.pPrivateDriverData = &faults;
    esc_info.PrivateDriverDataSize = sizeof(faults);
    esc_info.Flags.HardwareAccess = FALSE;
    
    faults.Type = D3DKMT_VIDMMESCAPETYPE_SETFAULT;

    if(mask & PagingPathLockSubRangeFault)
    {
        faults.SetFault.PagingPathLockSubRange = 1;
    }

    if(mask & PagingPathLockMinRangeFault)
    {
        faults.SetFault.PagingPathLockMinRange = 1;
    }

    NTSTATUS status = ddi->KmEscape(&esc_info);

exit:
    if(NULL != adapter)
    {
        D3DKMT_CLOSEADAPTER ca_info;
        ca_info.hAdapter = adapter;
        
        ddi->KmCloseAdapter(&ca_info);

        adapter = NULL;
    }

    if(NULL != ddi)
    {
        ddi->Release();
        ddi = NULL;
    }

}

DWORD WINAPI ControlFault(LPVOID mask)
{
    // if params is not NULL, set the fault, wait for 1 min
    // or for the test to complete and then reset it 
    if(mask)
    {
        UINT* pu = (UINT*)mask;
        SetFault(*pu);

        // wait on handle for 1 minute
        HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, L"Test Complete Event");

        if(!hEvent)
        {
            Sleep(60000);
        }
        else
        {
            WaitForSingleObject(hEvent, 60000);
            CloseHandle(hEvent);
        }

        SetFault(0);
    }
    // if mask is NULL, reset fault and return right away
    else
    {
        SetFault(0);
    }

    return 0;
}


#ifndef _WIN64
typedef BOOL (*IsWow64ProcessFunc)(HANDLE hProcess, PBOOL Wow64Process);
#endif

bool Is32BitAddressSpace()
{
#ifndef _WIN64
    BOOL bIsWow = FALSE;
    IsWow64ProcessFunc IsWow64ProcFunc = NULL;
    HMODULE hKernel32 = GetModuleHandle(_T("kernel32"));
    if(NULL != hKernel32)
    {
        IsWow64ProcFunc = (IsWow64ProcessFunc)GetProcAddress(hKernel32, "IsWow64Process");
    }
    if (NULL != IsWow64ProcFunc)
    {
        if ((IsWow64ProcFunc)(GetCurrentProcess(), &bIsWow) == 0)
        {
            // error
            return false;
        }
        if (bIsWow)
        {
            //
            //	We are 32-bit app running on 64-bit OS
            //
            return false;
        }
    }
    //
    //	We are 32-bit app running on 32-bit OS
    //
    return true;
#else
    //
    //	We are 64-bit application, so we are running on 64-bit OS
    //
    return false;
#endif	
}