/******************************Module*Header*******************************
* Module Name: LockConfirm.cpp
*
* Implementation of the test component that will lock and unlock allocations 
* and then verify allocation content
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/

#include <VidmmRequires.h>
#include "LockConfirm.h"


extern bool I3DIsPow2(UINT nIn);
bool Is32BitAddressSpace();

//
// util functions to format debug strings
//
WCHAR* GetArgs(const WCHAR *pwszFmt, va_list argList)
{
    const int   nAllocSize = 256;
    UINT         nAlloc     = nAllocSize;
    bool        bSuccess   = false;
    WCHAR*      pwszOut = NULL;

    // preprocess the string with the values from the variable argument list.
    while(!bSuccess)
    {
        pwszOut = new WCHAR[nAlloc];

        if(!pwszOut)
            return L"Error formatting string. Out of memory.";

        int nChars = _vsnwprintf(pwszOut, nAlloc, pwszFmt, argList);

        if((nChars < 0) || (nChars == nAlloc)) // we need to alloc more memory
        {
            delete[] pwszOut;
            nAlloc += nAllocSize;
        }
        else
        {
            bSuccess = true;
            //add \n to the string
            StringCchCat(pwszOut, sizeof(pwszOut)/sizeof pwszOut[0], L"\n");
        }
    }

    return pwszOut;
}



void DBGPrintMsg(CLogWriter* pLog, const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    OutputDebugString(pwszOut);

    if(pLog)
    {
        pLog->Message(pwszOut);
    }

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}

void DBGPrintFail(CLogWriter* pLog, const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    OutputDebugString(pwszOut);

    if(pLog)
    {
        pLog->Fail(pwszOut);
    }

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}

void DBGPrintSkip(CLogWriter* pLog, const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    OutputDebugString(pwszOut);

    if(pLog)
    {
        pLog->Skip(pwszOut);
    }

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}


//#define TRIAGE_HELP_ON

#ifdef TRIAGE_HELP_ON
#define TRIAGE_HELP(_x_) DBGPrint _x_
#else
#define TRIAGE_HELP(_x_)
#endif


CLockConfirmTypeMap::CLockConfirmTypeMap()
{
    addType(ENUMARG(Texture));
    addType(ENUMARG(RenderTarget));
}

//////////////////////////////////////////////////////////////////////////////////

CLockConfirm::CLockConfirm()
{
    AddTypeMap(m_TypeMap);
    /*
        Describe the purpose of this component for the spec
    */
    SetDescription(L"lock allocations, change their content, verify content");

    AddParam<bool>(L"BVT", L"Whether to run in BVT mode (no allocation pattern checking)");
    AddParam<bool>(L"Integrated", L"Indicates that test is running on a system with integrated display chipset");
    AddParam<LONGLONG>(L"VidMemSize", L"Total size of local and non-local video memory on the system");
    AddParam<eALLOC_TYPE>(L"LockConfirmAllocType", L"Different types of allocations");
    AddParam<UINT>(L"AllocWidth", L"Width of the allocation");
    AddParam<UINT>(L"AllocHeight", L"Height of the allocation");
}


void CLockConfirm::Init()
{
    m_bBVT = GetParam<bool>(L"BVT");
    m_bIntegrated = GetParam<bool>(L"Integrated");
    m_Type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");
    m_Width = GetParam<UINT>(L"AllocWidth");
    m_Height = GetParam<UINT>(L"AllocHeight");

    std::wstring ComponentName = L"";
    GetComponentName(ComponentName);

    DBGPrintMsg(m_pLog, L"ComponentName = %s", ComponentName.c_str());
}


HRESULT CLockConfirm::LockSurface(CI3DSurface2Ptr pS, DWORD dwOldPattern, DWORD dwNewPattern, DWORD* pPitch, DWORD dwFlags)
{
    HRESULT hr = S_OK;
    UINT nWidth = 0, nHeight = 0, size = 0;
    D3DDDIFORMAT format;
    UINT nBytesPerPixel = 0;
    D3DLOCKED_RECT Rect;


    //
    //  Calculate size of the allocation
    //
    H_CHECK(pS->GetSize(&nWidth, &nHeight));
    format = pS->GetFormat();
    nBytesPerPixel = CPixel::BytesPerPixel(format);
    size = nWidth * nBytesPerPixel;
    DWORD FmtXBitsMask = GetXBitsMask(format);

    //
    //	Lock
    //
    hr = pS->Lock(dwFlags, NULL, &Rect);

    if(pPitch && *pPitch && (Rect.Pitch != *pPitch))
    {
        *pPitch = Rect.Pitch;
    }

    if(SUCCEEDED(hr))
    {
        BYTE* pStart = (BYTE*)Rect.pBits;

        if(pStart)
        {
            //
            //	Check old pattern
            //
            if(dwOldPattern)
            {
                for(UINT row = 0; row < nHeight; row++)
                {
                    if(FAILED(hr = ResourcePattern(size, pStart + row*Rect.Pitch, dwOldPattern, FmtXBitsMask, FALSE, TRUE)))
                    {
                        break;
                    }
                }
            }

            if(dwNewPattern)
            {
                //
                //	Fill surface with new pattern
                //
                for(UINT row = 0; row < nHeight; row++)
                {
                    if(FAILED(hr = ResourcePattern(size, pStart + row*Rect.Pitch, dwNewPattern, FmtXBitsMask, TRUE, FALSE)))
                    {
                        break;
                    }
                }
            }

            //
            //	If ResourcePattern failed, unlock this surface
            //
            if(FAILED(hr))
            {
                H_CHECK(pS->Unlock());
            }
        }
        else
        {
            DBGPrintFail(m_pLog, L"Lock succeeded, but pointer is NULL");
        }
    }
    else
    {
        DBGPrintMsg(m_pLog, L"Failed to lock surface, hr = 0x%X", hr);
    }

    return hr;
}

HRESULT CLockConfirm::UnlockSurface(CI3DSurface2Ptr pS)
{
    HRESULT hr = S_OK;

    hr = pS->Unlock();

    return hr;
}


HRESULT CLockConfirm::ResourcePattern(UINT size, BYTE* Start, DWORD dwFillPattern, DWORD Mask, BOOL bFill, BOOL bCheck)
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
                DWORD dwFillPatternWithoutXBits = dwFillPattern & Mask;
                DWORD Temp = *((DWORD*) Start) & Mask;

                if(Temp != dwFillPatternWithoutXBits)
                {
                    DBGPrintMsg(m_pLog, L"Pattern is wrong: 0x%X != 0x%X", Temp, dwFillPatternWithoutXBits);
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


HRESULT CLockConfirm::CreateRenderTarget(CI3DDevicePtr pDevice, RENDER_TARGET* pRT, DWORD dwWidth, DWORD dwHeight, bool bLockable)
{
    HRESULT hr = S_OK;
    DWORD Usage = I3DSURF_RENDERTARGET;

    if(!bLockable)
    {
        Usage |= I3DSURF_NOTLOCKABLE;
    }

    CI3DSurface2Ptr pCurrentRT;
    H_CHECK(pDevice->GetRenderTarget(0, &pCurrentRT.p));

    if(dwWidth == 0 || dwHeight == 0)
    {
        H_CHECK(hr = pCurrentRT->GetSize((UINT*)&dwWidth, (UINT*)&dwHeight));
    }

    hr = pDevice->Create2DNonMippedResource(&pRT->pRT.p,
                                             dwWidth,
                                             dwHeight,
                                             Usage,
                                             pCurrentRT->GetFormat(),
                                             D3DDDIMULTISAMPLE_NONE,
                                             0,
                                             D3DDDIPOOL_VIDEOMEMORY,
                                             NULL);

    if(SUCCEEDED(hr))
    {
        H_CHECK(pRT->pRT->GetSurface(&pRT->pSurface.p));
        
        //
        //	Use Color fill to use the surface
        //
        RECT rect;
        rect.left = 0;
        rect.top =0;
        rect.right=dwWidth;
        rect.bottom=dwHeight;

        
        H_CHECK(pDevice->ColorFill(pRT->pSurface, rect, D3DCOLOR_ARGB(255, 255, 0, 0)));

        if(bLockable)
        {
            //
            //	Lock and fill with pattern
            //

            pRT->dwFillPattern = rand();
            if(0 == pRT->dwFillPattern)
            {
                pRT->dwFillPattern++;
            }

            H_CHECK(LockSurface(pRT->pSurface, 0, pRT->dwFillPattern));

            H_CHECK(UnlockSurface(pRT->pSurface));
        }
    }

    return hr;
}


HRESULT CLockConfirm::CreatePrimary(PINNED_SURFACE* pS, UINT numS)
{
    HRESULT hr = E_FAIL;

    CI3DSwapChainPtr pSwapChain;
    CI3DSurface2Ptr pFrontBuffer;

    H_CHECK(m_pDevice->GetSwapChain(&pSwapChain.p));

    H_CHECK(pSwapChain->GetFrontBuffer(&pFrontBuffer.p));

    UINT Width = 0, Height = 0;
    pFrontBuffer->GetSize(&Width, &Height);


    if(SUCCEEDED(hr = m_pDevice->CreatePrimaryResource(&pS->pPrimary.p,
                                                        Width,
                                                        Height,
                                                        pFrontBuffer->GetOutputIndex(),
                                                        pFrontBuffer->GetRefreshRate(),
                                                        I3DSURF_PRIMARYSURFACE,
                                                        pFrontBuffer->GetFormat(),
                                                        pFrontBuffer->GetMultiSampleType(),
                                                        pFrontBuffer->GetMultiSampleQuality())))
    {

        H_CHECK(pS->pPrimary->GetSurface(&pS->pSurface.p));

        //
        //	Use this resource to have it be brought into memory
        //
        RECT rComplete;
        rComplete.left = 0;
        rComplete.right = Width;
        rComplete.top = 0;
        rComplete.bottom = Height;

        H_CHECK(m_pDevice->PresentCopyBlt(false,//vsync
                                          false,//src color key
                                          false,//dst color key
                                          0,//color
                                          pFrontBuffer.p,//src
                                          pS->pSurface.p,//dst
                                          rComplete,
                                          rComplete,
                                          1,
                                          &rComplete));  

        //        
        // now lock it and fill with initial pattern
        //
        pS->dwFillPattern = rand();
        if(0 == pS->dwFillPattern)
        {
            pS->dwFillPattern++;
        }

        H_CHECK(LockSurface(pS->pSurface, 0, pS->dwFillPattern));

        H_CHECK(UnlockSurface(pS->pSurface));
    }

    return hr;
}


HRESULT CLockConfirm::CreateDynamicTexture(DYNAMIC_TEXTURE* pT, DWORD dwWidth, DWORD dwHeight, DWORD dwMipCount)
{
    HRESULT hr = S_OK;
    CI3DTexturePtr pSysMemTexture; 
    D3DDDIFORMAT Format;
    DWORD Usage = I3DSURF_DYNAMIC|I3DSURF_TEXTURE;

    if(!dwWidth)
    {
        dwWidth = (DWORD)pow(2.0f, (rand() % 10) + 1);
    }

    if(!dwHeight)
    {
        dwHeight = (DWORD)pow(2.0f, (rand() % 10) + 1);
    }

    //
    //	Get format
    //
    H_CHECK(I3DXGetValidResourceFormat(m_pDevice, I3DFORMAT_ARGB, Usage, false, &Format));

    if(SUCCEEDED(hr = m_pDevice->CreateTexture(&pT->pVidMemTexture.p,
                                               dwWidth,
                                               dwHeight,
                                               dwMipCount,
                                               Usage,
                                               Format,
                                               D3DDDIPOOL_VIDEOMEMORY,
                                               false)))
    {
        
        H_CHECK(m_pDevice->CreateTexture(&pSysMemTexture.p,
                                          dwWidth,
                                          dwHeight,
                                          dwMipCount,
                                          I3DSURF_TEXTURE,
                                          Format,
                                          D3DDDIPOOL_SYSTEMMEM,
                                          false));

        //
        //	Fill with pattern
        //
        pT->dwFillPattern = rand();
        if(0 == pT->dwFillPattern)
        {
            pT->dwFillPattern++;
        }

        pSysMemTexture->GetSurfaceCount((UINT*)&dwMipCount);

        for(UINT i = 0; i < dwMipCount; i++)
        {
            CI3DSurface2Ptr pSurfaceSys, pSurfaceVid;

            H_CHECK(pSysMemTexture->GetSurface(i, &pSurfaceSys.p));
                
            H_CHECK(LockSurface(pSurfaceSys, 0, pT->dwFillPattern, NULL));
            H_CHECK(UnlockSurface(pSurfaceSys));

            //
            //	Initialize vid mem copy of the texture so that it is 
            //  actually paged into memory on use, rather than be spawned
            //
            H_CHECK(pT->pVidMemTexture->GetSurface(i, &pSurfaceVid.p));
            H_CHECK(LockSurface(pSurfaceVid, 0, pT->dwFillPattern, NULL));
            H_CHECK(UnlockSurface(pSurfaceVid));
        }


        //
        //	Use texture by blitting
        //
        RECT BltSrc;
        POINT BltDst;

        BltSrc.left = 0;
        BltSrc.right = dwWidth;
        BltSrc.top = 0;
        BltSrc.bottom = dwHeight;

        BltDst.x = 0;
        BltDst.y = 0;

        H_CHECK(m_pDevice->TexBlt(pSysMemTexture, 
                                  pT->pVidMemTexture, 
                                  0, 
                                  BltSrc, 
                                  BltDst));


        // keep pointer to level 0
        H_CHECK(pT->pVidMemTexture->GetSurface(0, &pT->pSurface.p));

        pT->dwMipCount = dwMipCount;
    }

    return hr;  
}


HRESULT CLockConfirm::CauseEvictions(CI3DDevicePtr pDevice)
{
    HRESULT hr = S_OK;

    {
    LONGLONG VidMemSize = GetParam<LONGLONG>(L"VidMemSize");
    RENDER_TARGET RT[1024];
    UINT i = 0;
    const UINT Dimensions = 1024;

    if(Is32BitAddressSpace())
    {
        LONGLONG MaxAllocatedSpace = (LONGLONG)(1.5 * 1024 * 1024 * 1024);

        if(VidMemSize > MaxAllocatedSpace)
        {
            VidMemSize = MaxAllocatedSpace;
        }
    }

    DBGPrintMsg(m_pLog, L"CauseEvictions() - memory size = %I64d", VidMemSize);
    
    while(VidMemSize > 0 && i < 1024)
    {
        hr = CreateRenderTarget(pDevice, &RT[i], Dimensions, Dimensions, false);

        if(FAILED(hr))
        {
            DBGPrintMsg(m_pLog, L"CauseEvictions() - Failed to create render target %d, hr = 0x%X", i, hr);
            break;
        }

        VidMemSize -= Dimensions * Dimensions * CPixel::BytesPerPixel(RT[i].pSurface->GetFormat());
        i++;
    }

    DBGPrintMsg(m_pLog, L"CauseEvictions() - created %d surfaces", i);
    }
    
    //
    //	A lot of allocations have been created to cause evictions, 
    //	with deferred allocation destructions, these allocations might
    //	not have been destroyed. In order for them to be destroyed, we
    //	need to call Present() (that's when terminated allocation list is
    //	processed) to clean up memory for the next iteration of this test
    //

    H_CHECK(pDevice->Present());

    return hr;
}


HRESULT CLockConfirm::CreateAllocations(SURFACE*** ppSurf, UINT Count)
{
    HRESULT hr = E_FAIL;

    SURFACE** pSurface = new SURFACE*[Count];
    if(!pSurface)
    {
        throw std::exception("Out of memory");
    }
    ZeroMemory(pSurface, sizeof(SURFACE*) * Count);

    if(!ppSurf)
    {
        throw std::exception("Test error - invalid parameter in CreateAllocations");
    }

    *ppSurf = pSurface;

    for(UINT i = 0; i < Count; i++)
    {
        if(m_Type == Texture)
        {
            pSurface[i] = (SURFACE*) new DYNAMIC_TEXTURE;
            if(!pSurface[i])
            {
                throw std::exception("Out of memory");
            }

            ZeroMemory(pSurface[i], sizeof(DYNAMIC_TEXTURE));

            if(FAILED(hr = CreateDynamicTexture((DYNAMIC_TEXTURE*)pSurface[i], m_Width, m_Height)))
            {
                break;
            }
        }
        else if(m_Type == RenderTarget)
        {
            pSurface[i] = (SURFACE*) new RENDER_TARGET;
            if(!pSurface[i])
            {
                throw std::exception("Out of memory");
            }
            
            ZeroMemory(pSurface[i], sizeof(RENDER_TARGET));

            if(FAILED(hr = CreateRenderTarget(m_pDevice, (RENDER_TARGET*)pSurface[i], m_Width, m_Height)))
            {
                break;
            }
        }
    }

    return hr;
}


void CLockConfirm::DestroyAllocations(SURFACE*** ppSurf, UINT Count)
{
    if(!ppSurf)
    {
        throw std::exception("Invalid parameter in DestroyAllocations");
    }

    SURFACE** pSurface = *ppSurf;

    for(UINT i = 0; i < Count; i++)
    {
        if(pSurface[i] != NULL)
        {
            if(m_Type == Texture)
            {
                delete (DYNAMIC_TEXTURE*)pSurface[i];
                pSurface[i] = NULL;
            }
            else if(m_Type == RenderTarget)
            {
                delete (RENDER_TARGET*)pSurface[i];
                pSurface[i] = NULL;
            }
        }
    }

    delete [] pSurface;
    *ppSurf = NULL;
}


bool CLockConfirm::CapsCheckForTextureSupport(UINT MipCount)
{

    //
    //	See if this HW requires square or pow2 textures
    //
    D3DCAPS9 Caps;
    H_CHECK(I3DGetD3D9Caps(m_pDevice, &Caps));

    UINT Width = 0, Height = 0;
    Width = GetParam<UINT>(L"AllocWidth");
    Height = GetParam<UINT>(L"AllocHeight");

    if(Caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
    {
        if(Width != Height)
        {
            return false;
        }
    }

    if(!I3DIsPow2(Width) || !I3DIsPow2(Height))
    {
        if(Caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        {
            if(!(Caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
            {
                return false;
            }
            else
            {
                if(MipCount != 1)
                {
                    return false;
                }
            }
        }
    }

    return true;
}



HRESULT CLockConfirm::SetVidMmFault(UINT Fault)
{
    HRESULT hr = S_OK;

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

    if(FAILED(hr = ddi->KmOpenAdapterFromHDC(&info)))
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

    if(Fault & PagingPathLockSubRangeFault)
    {
        faults.SetFault.PagingPathLockSubRange = 1;
    }

    if(Fault & PagingPathLockMinRangeFault)
    {
        faults.SetFault.PagingPathLockMinRange = 1;
    }

    if(Fault & SwizzlingApertureFault)
    {
        faults.SetFault.SwizzlingAperture = 1;
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

    return hr;
}


void CLockConfirm::RenderTexturedQuad(CI3DTexturePtr pTexture)
{
    struct MyVertex
    {
        float Pos[4];	
        float Tex[2];
    };
    const DWORD MYFVF = (D3DFVF_XYZRHW | D3DFVF_TEX1);
    const UINT  VERTEXCOUNT   = 6;
    const UINT  WIDTH		    = 320;
    const UINT  HEIGHT		    = 240;



    //create VB for drawing surface
    CI3DVertexBufferPtr pVertexBuffer;
    H_CHECK(m_pDevice->CreateVertexBuffer(&pVertexBuffer.p, 
                                        sizeof(MyVertex) * VERTEXCOUNT, 
                                        0,
                                        MYFVF, 
                                        D3DDDIPOOL_SYSTEMMEM, 
                                        NULL));

    MyVertex* pLockedVertices = NULL;
    H_CHECK(pVertexBuffer->Lock(0,0, (void**)&pLockedVertices, 0));

    pLockedVertices[0].Pos[0] = -0.5f;//x
    pLockedVertices[0].Pos[1] = -0.5f;//y
    pLockedVertices[0].Pos[2] = 0.5f;//z
    pLockedVertices[0].Pos[3] = 1.0f;//w
    pLockedVertices[0].Tex[0] = 0.0f;
    pLockedVertices[0].Tex[1] = 0.0f;


    pLockedVertices[1].Pos[0] = (float)WIDTH - 0.5f;//xbcz
    pLockedVertices[1].Pos[1] = -0.5f;//y
    pLockedVertices[1].Pos[2] = 0.5f;//z
    pLockedVertices[1].Pos[3] = 1.0f;//w
    pLockedVertices[1].Tex[0] = 1.0f;
    pLockedVertices[1].Tex[1] = 0.0f;

    pLockedVertices[2].Pos[0] = -0.5f;//x
    pLockedVertices[2].Pos[1] = (float)(HEIGHT) - 0.5f;//y
    pLockedVertices[2].Pos[2] = 0.5f;//z
    pLockedVertices[2].Pos[3] = 1.0f;//w
    pLockedVertices[2].Tex[0] = 0.0f;
    pLockedVertices[2].Tex[1] = 1.0f;

    pLockedVertices[3].Pos[0] = -0.5f;//x
    pLockedVertices[3].Pos[1] = (float)(HEIGHT) - 0.5f;//y
    pLockedVertices[3].Pos[2] = 0.5f;//z
    pLockedVertices[3].Pos[3] = 1.0f;//w
    pLockedVertices[3].Tex[0] = 0.0f;
    pLockedVertices[3].Tex[1] = 1.0f;

    pLockedVertices[4].Pos[0] = (float)(WIDTH) - 0.5f;//x
    pLockedVertices[4].Pos[1] = -0.5f;//y
    pLockedVertices[4].Pos[2] = 0.5f;//z
    pLockedVertices[4].Pos[3] = 1.0f;//w
    pLockedVertices[4].Tex[0] = 1.0f;
    pLockedVertices[4].Tex[1] = 0.0f;

    pLockedVertices[5].Pos[0] = (float)(WIDTH) - 0.5f;//x
    pLockedVertices[5].Pos[1] = (float)(HEIGHT) - 0.5f;//y
    pLockedVertices[5].Pos[2] = 0.5f;//z
    pLockedVertices[5].Pos[3] = 1.0f;//w
    pLockedVertices[5].Tex[0] = 1.0f;
    pLockedVertices[5].Tex[1] = 1.0f;

    H_CHECK(pVertexBuffer->Unlock());


    H_CHECK(m_pDevice->Clear(0x00FFFF00,I3DXGetClearFlags(m_pDevice) | D3DCLEAR_COMPUTERECTS,1,1,0,NULL));
    H_CHECK(m_pDevice->SetStreamSource(0,sizeof(MyVertex), pVertexBuffer));
    H_CHECK(I3DXSetDefaultVertexPipe(m_pDevice, MYFVF));
    H_CHECK(I3DXSet2DTexModDiffusePixelShader(m_pDevice));
    H_CHECK(m_pDevice->SetTexture(0, pTexture.p));

    H_CHECK(m_pDevice->BeginScene()); 
    H_CHECK(m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2));
    H_CHECK(m_pDevice->EndScene());   
    H_CHECK(m_pDevice->Present());

}


void CLockConfirm::UseSurface(CI3DDevicePtr pDevice, CI3DSurface2Ptr pSurface)
{
    RENDER_TARGET RT;
    UINT Width = 0, Height = 0;
    RECT Rect;

    H_CHECK(pSurface->GetSize(&Width, &Height));

    H_CHECK(CreateRenderTarget(pDevice, &RT, Width, Height, true));

    Rect.left = 0;
    Rect.top = 0;
    Rect.right = Width;
    Rect.bottom = Height;

    H_CHECK(m_pDevice->Blt(pSurface, RT.pSurface, Rect, Rect, I3DBLT_LINEAR, 0));
}

void CLockConfirm::CreateAdditionalDevice(CI3DDevice** ppDevice)
{
    //create a window
    CWindowThread Window;
    Window.Create(L"STATIC",
                  L"LockConfirmAdditionalDevice",
                  WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                  CW_USEDEFAULT,
                  CW_USEDEFAULT,
                  CW_USEDEFAULT,
                  CW_USEDEFAULT,
                  NULL,
                  NULL,
                  NULL);

    CI3DPtr p3D;
    DeviceCreateOptions Opts;

    H_CHECK(CreateI3D(&p3D.p, NULL));

    H_CHECK(p3D->GetValidDeviceCreateOptions(true,  // windowed
                                             Window.GetHWND(),
                                             256,	// width
                                             256,	// height
                                             1,		// back buffer count
                                             true,  // depth buffer
                                             false, 
                                             NULL,
                                             &Opts));

    H_CHECK(p3D->CreateDevice(&Opts, ppDevice));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CLockConfirmLockPinned::CLockConfirmLockPinned()
{
    SetDescription(L"LockPinned - locks/unlocks pinned allocations, verifies contents");
}

bool CLockConfirmLockPinned::CapsCheck()
{
    // Only run in full screen mode since we can't create primaries in windowed mode
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    if(!bWindowed)
    {
        // This test is not affected by the combination of dimentions/allocation type,
        // so, only run it once
        UINT Width = 0, Height = 0;
        Width = GetParam<UINT>(L"AllocWidth");
        Height = GetParam<UINT>(L"AllocHeight");

        if((Width == 1) &&
           (Height == 1))
        {
            return true;
        }
    }
    
    return false;
}

void CLockConfirmLockPinned::RunTest()
{
    Init();

    UINT i = 0;
    HRESULT hr = S_OK;
    PINNED_SURFACE Primaries[ALLOC_COUNT];

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        hr = CreatePrimary(&Primaries[i], i);

        if(FAILED(hr))
        {
            DBGPrintSkip(m_pLog, L"LockPinned - Failed to create primaries, hr = 0x%X, i = %d", hr, i);
            return;
        }
    }

    //
    //  Lock allocations
    //

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        DWORD newPattern = (0 != Primaries[i].dwFillPattern + 1) ? (Primaries[i].dwFillPattern + 1) : 1;
        if(FAILED(hr = LockSurface(Primaries[i].pSurface, 0, newPattern)))
        {
            DBGPrintFail(m_pLog, L"LockPinned - Failed to lock surface, hr = 0x%X, i = %d", hr, i);
            break;
        }  

        Primaries[i].dwFillPattern = newPattern;
        H_CHECK(UnlockSurface(Primaries[i].pSurface));
    }

    //
    //	Verify allocation content
    //
    if(SUCCEEDED(hr))
    {
        for(i = 0; i < ALLOC_COUNT; i++)
        {
            if(FAILED(hr = LockSurface(Primaries[i].pSurface, Primaries[i].dwFillPattern, 0)))
            {
                DBGPrintFail(m_pLog, L"LockPinned - checking surface failed, hr = 0x%X, i = %d", hr, i);
                break;
            }   

            H_CHECK(UnlockSurface(Primaries[i].pSurface));
            TRIAGE_HELP((L"\tLockPinned - unlocked allocation %d, hr = 0x%X", i, hr));
        }
    }
}

void CLockConfirmLockPinned::EndTest()
{
}



CLockConfirmLockNonPinned::CLockConfirmLockNonPinned()
{
    SetDescription(L"LockNonPinned - locks/unlocks non-pinned allocations, verifies contents");
}


bool CLockConfirmLockNonPinned::CapsCheck()
{
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        return CapsCheckForTextureSupport();
    }

    return true;
}

void CLockConfirmLockNonPinned::RunTest()
{
    Init();

    UINT i = 0;
    HRESULT hr = S_OK;
    SURFACE** pSurface = NULL;

    hr = CreateAllocations(&pSurface, ALLOC_COUNT);

    if(FAILED(hr))
    {
        DBGPrintSkip(m_pLog, L"LockUnpinned - Failed to create allocation, hr = 0x%X", hr);
        goto exit;
    }

    //
    //  Lock allocations
    //

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        DWORD newPattern = (0 != pSurface[i]->dwFillPattern + 1) ? (pSurface[i]->dwFillPattern + 1) : 1;
        if(FAILED(hr = LockSurface(pSurface[i]->pSurface, 0, newPattern, NULL)))
        {
            DBGPrintFail(m_pLog, L"LockUnpinned - LockSurface failed, hr = 0x%X, i = %d", hr, i);
            break;
        }     

        pSurface[i]->dwFillPattern = newPattern;
        H_CHECK(UnlockSurface(pSurface[i]->pSurface));
    }

    //
    //  Check allocation content
    //

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < ALLOC_COUNT; i++)
        {
            if(FAILED(hr = LockSurface(pSurface[i]->pSurface, pSurface[i]->dwFillPattern, 0, NULL)))
            {
                DBGPrintFail(m_pLog, L"LockUnpinned - checking surface failed, hr = 0x%X, i = %d", hr, i);
                break;
            }       

            H_CHECK(UnlockSurface(pSurface[i]->pSurface));
        }
    }

exit:

    DestroyAllocations(&pSurface, ALLOC_COUNT);
}

void CLockConfirmLockNonPinned::EndTest()
{
}


CLockConfirmLockEvict::CLockConfirmLockEvict()
{
    SetDescription(L"LockEvict - locks allocations, changes its content, unlocks, then gets it evicted \
                    Verifies content after allocation is evicted");
}

bool CLockConfirmLockEvict::CapsCheck()
{
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        return CapsCheckForTextureSupport();
    }

    return true;
}

void CLockConfirmLockEvict::RunTest()
{
    Init();

    HRESULT hr = S_OK;
    SURFACE** pSurface = NULL;

    //
    //	Create test allocation, fill it with pattern
    //
    hr = CreateAllocations(&pSurface, 1);

    if(SUCCEEDED(hr))
    {
        DWORD newPattern = ( 0 != pSurface[0]->dwFillPattern + 1) ? (pSurface[0]->dwFillPattern + 1) : 1;

        hr = LockSurface(pSurface[0]->pSurface, 0, newPattern);

        if(SUCCEEDED(hr))
        {
            UnlockSurface(pSurface[0]->pSurface);

            pSurface[0]->dwFillPattern = newPattern;

            //
            //	Cause test allocation to be evicted
            //	
            hr = CauseEvictions(m_pDevice);

            if(SUCCEEDED(hr))
            {
                //
                //	Check test allocation pattern
                //
                hr = LockSurface(pSurface[0]->pSurface, pSurface[0]->dwFillPattern, 0);

                if(SUCCEEDED(hr))
                {
                    H_CHECK(UnlockSurface(pSurface[0]->pSurface));
                }
                else
                {
                    DBGPrintFail(m_pLog, L"LockEvict - checking surface failed, hr = 0x%X", hr);
                }
            }
            else
            {
                DBGPrintSkip(m_pLog, L"LockEvict - Failed to cause evictions, hr = 0x%X");
            }
        }
        else
        {
            DBGPrintFail(m_pLog, L"LockEvict - Failed to lock allocation, hr = 0x%X");
        }
    }
    else
    {
        DBGPrintSkip(m_pLog, L"LockEvict - failed to create allocation, hr = 0x%X");
    }

    DestroyAllocations(&pSurface, 1);
}

void CLockConfirmLockEvict::EndTest()
{
}


CLockConfirmEvictUnderLock::CLockConfirmEvictUnderLock()
{
    SetDescription(L"EvictUnderLock - locks allocation, changes its content, causes allocation to be \
                    evicted while under lock. Unlocks after eviction. Then verifies content");
}

bool CLockConfirmEvictUnderLock::CapsCheck()
{
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        return CapsCheckForTextureSupport();
    }

    return true;
}

void CLockConfirmEvictUnderLock::RunTest()
{
    Init();

    HRESULT hr = S_OK;
    SURFACE** pSurface = NULL;

    //
    //	Create test allocation, fill it with pattern
    //
    hr = CreateAllocations(&pSurface, 1);

    if(SUCCEEDED(hr))
    {
        DWORD newPattern = ( 0 != pSurface[0]->dwFillPattern + 1) ? (pSurface[0]->dwFillPattern + 1) : 1;

        hr = LockSurface(pSurface[0]->pSurface, 0, newPattern);

        if(SUCCEEDED(hr))
        {
            //
            //	Do not unlock, let it be evicted under lock
            //
            pSurface[0]->dwFillPattern = newPattern;

            //
            //	Cause test allocation to be evicted
            //	
            hr = CauseEvictions(m_pDevice);

            H_CHECK(UnlockSurface(pSurface[0]->pSurface));

            if(SUCCEEDED(hr))
            {
                //
                //	Check test allocation pattern
                //
                hr = LockSurface(pSurface[0]->pSurface, pSurface[0]->dwFillPattern, 0);

                if(SUCCEEDED(hr))
                {
                    UnlockSurface(pSurface[0]->pSurface);
                }
                else
                {
                    DBGPrintFail(m_pLog, L"EvictUnderLock - checking surface failed, hr = 0x%X", hr);
                }
            }
            else
            {
                DBGPrintSkip(m_pLog, L"EvictUnderLock - failed to cause evictions, hr = 0x%X", hr);
            }
        }
        else
        {
            DBGPrintFail(m_pLog, L"EvictUnderLock - failed to lock allocation, hr = 0x%X", hr);
        }
    }
    else
    {
        DBGPrintSkip(m_pLog, L"EvictUnderLock - Failed to create allocation, hr = 0x%X", hr);
    }


    DestroyAllocations(&pSurface, 1);
}


void CLockConfirmEvictUnderLock::EndTest()
{
}


CLockConfirmLockEachMipLevelNoSwizzlingRange::CLockConfirmLockEachMipLevelNoSwizzlingRange()
{
    SetDescription(L"LockEachMipLevelNoSwizzlingRange - will execute a scenario where several mip levels are being locked	\
                    but there is no available swizzling ranges for all levels, so allocation will need to be evicted.		\
                    Content of the allocation is verified");
}


bool CLockConfirmLockEachMipLevelNoSwizzlingRange::CapsCheck()
{
    // Only run in full screen mode since we can't create primaries in windowed mode
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    if(!bWindowed)
    {
        //
        //	This test only makes sense for textures
        //
        eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

        if(type == Texture)
        {
            // this test will create mipped textures
            return CapsCheckForTextureSupport(0);
        }
    }

    return false;
}


void CLockConfirmLockEachMipLevelNoSwizzlingRange::RunTest()
{
    Init();

    UINT i = 0, y = 0;
    HRESULT hr = S_OK;
    const UINT nPrim = ALLOC_COUNT / 2;
    DYNAMIC_TEXTURE TestTexture;

    //
    //	Create allocation
    //
    hr = CreateDynamicTexture(&TestTexture, m_Width, m_Height, 0);

    if(SUCCEEDED(hr))
    {
        //
        //	Lock each mip level without unlocking. At last mip level fail to acquire swizzling range.
        //  On HW that requires a separate swizzling range for each mip level this will result in VidMM
        //  evicting the whole allocation to its backing store
        //
        bool bFaultSet = false;

        for(y = 0; y < TestTexture.dwMipCount; y++)
        {
            CI3DSurface2Ptr pSurface;	
            H_CHECK(TestTexture.pVidMemTexture->GetSurface(y, &pSurface.p));

            if(y == (TestTexture.dwMipCount - 1))
            {
                SetVidMmFault(SwizzlingApertureFault);
                bFaultSet = true;
            }

            hr = LockSurface(pSurface, 0, TestTexture.dwFillPattern + y + 1);

            if(bFaultSet)
            {
                SetVidMmFault(0);
                bFaultSet = false;
            }

            if(FAILED(hr))
            {
                DBGPrintFail(m_pLog, L"LockEachMipLevelNoSwizzlingRange - Failed to lock %d level of texture, hr = 0x%X", y, hr);
                break;
            }
        }

        //
        //	Unlock all locked levels
        //
        while(y)
        {
            CI3DSurface2Ptr pSurface;	
            H_CHECK(TestTexture.pVidMemTexture->GetSurface(--y, &pSurface.p));

            H_CHECK(UnlockSurface(pSurface));
        }

        //
        //	Check patterns of all levels
        //
        if(SUCCEEDED(hr))
        {
            for(y = 0; y < TestTexture.dwMipCount; y++)
            {
                CI3DSurface2Ptr pSurface;	
                H_CHECK(TestTexture.pVidMemTexture->GetSurface(y, &pSurface.p));

                hr = LockSurface(pSurface, TestTexture.dwFillPattern + y + 1, 0);

                if(FAILED(hr))
                {
                    DBGPrintFail(m_pLog, L"LockEachMipLevelNoSwizzlingRange - checking %d level of texture failed, hr = 0x%X", y, hr);
                    break;
                }

                H_CHECK(UnlockSurface(pSurface));
            }
        }

    }
    else
    {
        DBGPrintSkip(m_pLog, L"LockEachMipLevelNoSwizzlingRange - Failed to create control allocation, hr = 0x%X", hr);
    }
}


void CLockConfirmLockEachMipLevelNoSwizzlingRange::EndTest()
{
}



CLockConfirmRunOutOfSwizzlingRanges_nonpinned::CLockConfirmRunOutOfSwizzlingRanges_nonpinned()
{
    SetDescription(L"This test locks non-pinned allocations without unlocking \
                    VidMM is expected to handle locking when running out of swizzling ranges");
}


bool CLockConfirmRunOutOfSwizzlingRanges_nonpinned::CapsCheck()
{
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        return CapsCheckForTextureSupport();
    }

    return true;
}


void CLockConfirmRunOutOfSwizzlingRanges_nonpinned::RunTest()
{
    Init();

    UINT i = 0;
    HRESULT hr = S_OK;
    SURFACE** pSurface;

    
    hr = CreateAllocations(&pSurface, ALLOC_COUNT);
    if(FAILED(hr))
    {
        DBGPrintSkip(m_pLog, L"RunOutOfSwizzlingRanges_nonpinned - Failed to create allocations, hr = 0x%X", hr);
        goto exit;
    }

    //
    //  Lock allocations without unlocking
    //

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        DWORD newPattern = (0 != pSurface[i]->dwFillPattern + 1) ? (pSurface[i]->dwFillPattern + 1) : 1;
        if(FAILED(hr = LockSurface(pSurface[i]->pSurface, 0, newPattern, NULL)))
        {
            //
            //	On integrated it is acceptable to fail in this scenario since after running out of
            //	swizzling ranges, VidMM will not be able to evict allocation to system memory, so
            //	Lock will fail
            //
            if(m_bIntegrated)
            {
                DBGPrintFail(m_pLog, L"RunOutOfSwizzlingRanges_nonpinned - Failed to lock non-pinned allocation, iteration %d, hr = 0x%X	\
                                  Assuming this is due to running out of swizzling ranges on Integrated chipset", i, hr);

                hr = S_OK;
            }
            else
            {
                DBGPrintFail(m_pLog, L"RunOutOfSwizzlingRanges_nonpinned - Failed to lock non-pinned allocation, iteration %d, hr = 0x%X", i, hr);
            }
            break;
        }

        pSurface[i]->dwFillPattern = newPattern;
    }

    while(i)
    {
        H_CHECK(UnlockSurface(pSurface[--i]->pSurface));
    }


    //
    //  Check allocation content
    //

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < ALLOC_COUNT; i++)
        {
            if(FAILED(hr = LockSurface(pSurface[i]->pSurface, pSurface[i]->dwFillPattern, 0, NULL)))
            {
                DBGPrintFail(m_pLog, L"RunOutOfSwizzlingRanges_nonpinned - checking surface failed, hr = 0x%X, i = %d", hr, i);
                break;
            }       

            H_CHECK(UnlockSurface(pSurface[i]->pSurface));
        }
    }

exit:
    DestroyAllocations(&pSurface, ALLOC_COUNT);
}


void CLockConfirmRunOutOfSwizzlingRanges_nonpinned::EndTest()
{
}


CLockConfirmRunOutOfSwizzlingRanges_pinned::CLockConfirmRunOutOfSwizzlingRanges_pinned()
{
    SetDescription(L"This test locks pinned allocations without unlocking \
                     expects locks to fails when run out of swizzling ranges");
}

bool CLockConfirmRunOutOfSwizzlingRanges_pinned::CapsCheck()
{
    // Only run in full screen mode since we can't create primaries in windowed mode
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    if(!bWindowed)
    {
        // This test is not affected by the combination of dimentions/allocation type,
        // so, only run it once
        UINT Width = 0, Height = 0;
        Width = GetParam<UINT>(L"AllocWidth");
        Height = GetParam<UINT>(L"AllocHeight");

        if((Width == 1) &&
            (Height == 1))
        {
            return true;
        }
    }

    return false;
}


void CLockConfirmRunOutOfSwizzlingRanges_pinned::RunTest()
{
    Init();

    UINT i = 0;
    HRESULT hr = S_OK;
    PINNED_SURFACE Primaries[ALLOC_COUNT];

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        hr = CreatePrimary(&Primaries[i], i);

        if(FAILED(hr))
        {
            DBGPrintSkip(m_pLog, L"RunOutOfSwizzlingRanges_pinned - Failed to create primaries, hr = 0x%X, i = %d", hr, i);
            return;
        }
    }

    //
    //  Lock allocations without unlocking. Failure to lock is expected as when
    //	we run out of swizzling ranges there is nothing else we can do
    //

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        DWORD newPattern = (0 != Primaries[i].dwFillPattern + 1) ? (Primaries[i].dwFillPattern + 1) : 1;
        if(FAILED(hr = LockSurface(Primaries[i].pSurface, 0, newPattern)))
        {
            DBGPrintMsg(m_pLog, L"RunOutOfSwizzlingRanges_pinned - Locking pinned allocation failed. Assuming this is due to running out of swizzling ranges, \
                              iteration %d, hr = 0x%X", i, hr);

            hr = S_OK;
            break;
        }

        Primaries[i].dwFillPattern = newPattern;
    }

    while(i)
    {
        H_CHECK(UnlockSurface(Primaries[--i].pSurface));
    }       

    //
    //	Verify allocation content
    //
    if(SUCCEEDED(hr))
    {
        for(i = 0; i < ALLOC_COUNT; i++)
        {
            if(FAILED(hr = LockSurface(Primaries[i].pSurface, Primaries[i].dwFillPattern, 0)))
            {
                DBGPrintFail(m_pLog, L"RunOutOfSwizzlingRanges_pinned - checking surface failed, hr = 0x%X, i", hr, i);
                break;  
            }  
            H_CHECK(UnlockSurface(Primaries[i].pSurface));
        }
    }
}

void CLockConfirmRunOutOfSwizzlingRanges_pinned::EndTest()
{
}



CLockConfirmLockEachMipLevel::CLockConfirmLockEachMipLevel()
{
    SetDescription(L"locks and modifies each mip level of texture without unlocking it \
                     Unlocks levels and verifies contents");
}


bool CLockConfirmLockEachMipLevel::CapsCheck()
{
    //
    //	This test only makes sense for textures
    //
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        // this test will create mipped textures
        return CapsCheckForTextureSupport(0);
    }

    return false;
}


void CLockConfirmLockEachMipLevel::RunTest()
{
    Init();

    HRESULT hr = S_OK;
    UINT i = 0;
    DYNAMIC_TEXTURE TestTexture;

    //
    //	Create control allocation
    //
    hr = CreateDynamicTexture(&TestTexture, m_Width, m_Height, 0);

    //
    //	Lock each level and fill with unique fill pattern, don't unlock
    //
    if(SUCCEEDED(hr))
    {
        for(i = 0; i < TestTexture.dwMipCount; i++)
        {
            CI3DSurface2Ptr pSurface;	
            H_CHECK(TestTexture.pVidMemTexture->GetSurface(i, &pSurface.p));

            hr = LockSurface(pSurface, 0, TestTexture.dwFillPattern + i + 1);

            if(FAILED(hr))
            {
                DBGPrintFail(m_pLog, L"LockEachMipLevel - Failed to lock %d level of texture, hr = 0x%X", i, hr);
                break;
            }
        }

        //
        //	Unlock all locked levels
        //
        while(i)
        {
            CI3DSurface2Ptr pSurface;	
            H_CHECK(TestTexture.pVidMemTexture->GetSurface(--i, &pSurface.p));

            H_CHECK(UnlockSurface(pSurface));
        }

        //
        //	Check patterns of all levels
        //
        if(SUCCEEDED(hr))
        {
            for(i = 0; i < TestTexture.dwMipCount; i++)
            {
                CI3DSurface2Ptr pSurface;	
                H_CHECK(TestTexture.pVidMemTexture->GetSurface(i, &pSurface.p));

                hr = LockSurface(pSurface, TestTexture.dwFillPattern + i + 1, 0);

                if(FAILED(hr))
                {
                    DBGPrintFail(m_pLog, L"LockEachMipLevel - Failed checking level %d of texture, hr = 0x%X", i, hr);
                    break;
                }

                H_CHECK(UnlockSurface(pSurface));
            }
        }
    }
    else
    {
        DBGPrintSkip(m_pLog, L"LockEachMipLevel - failed to create dynamic texture, hr = 0x%X", hr);
    }

}


void CLockConfirmLockEachMipLevel::EndTest()
{
}


CLockConfirmLockWithFaults::CLockConfirmLockWithFaults()
{
    SetDescription(L"This test will enable different VidMM faults and lock allocations to force certain code paths");

    AddParam<UINT>(L"VidMmFaults", L"VidMm faults to set before locking");
}


bool CLockConfirmLockWithFaults::CapsCheck()
{
    eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

    if(type == Texture)
    {
        return CapsCheckForTextureSupport();
    }

    return true;
}


void CLockConfirmLockWithFaults::RunTest()
{
    Init();

    UINT VidMmFaults = GetParam<UINT>(L"VidMmFaults");
    UINT i = 0;
    HRESULT hr = S_OK;
    SURFACE** pSurface = NULL;

    hr = CreateAllocations(&pSurface, ALLOC_COUNT);

    if(FAILED(hr))
    {
        DBGPrintSkip(m_pLog, L"LockWithFaults - Failed to create allocations, hr = 0x%X", hr);
        goto exit;
    }

    //
    //	Set faults
    //
    if(SUCCEEDED(hr = SetVidMmFault(VidMmFaults)))
    {

        //
        //  Lock and check allocation content
        //

        if(SUCCEEDED(hr))
        {
            for(i = 0; i < ALLOC_COUNT; i++)
            {
                if(FAILED(hr = LockSurface(pSurface[i]->pSurface, pSurface[i]->dwFillPattern, 0, NULL)))
                {
                    DBGPrintFail(m_pLog, L"LockWithFaults - checking surface failed, hr = 0x%X, i = %d", hr, i);
                    break;
                }       

                H_CHECK(UnlockSurface(pSurface[i]->pSurface));
            }
        }


        if(SUCCEEDED(hr))
        {
            //
            //  Lock allocations again and fill with new pattern
            //

            for(i = 0; i < ALLOC_COUNT; i++)
            {
                DWORD newPattern = (0 != pSurface[i]->dwFillPattern + 1) ? (pSurface[i]->dwFillPattern + 1) : 1;
                if(FAILED(hr = LockSurface(pSurface[i]->pSurface, 0, newPattern, NULL)))
                {
                    DBGPrintFail(m_pLog, L"LockWithFaults - LockSurface failed, hr = 0x%X, i = %d", hr, i);
                    break;
                }     

                pSurface[i]->dwFillPattern = newPattern;
                H_CHECK(UnlockSurface(pSurface[i]->pSurface));
            }

            if(SUCCEEDED(hr))
            {
                //
                //	Reset VidMm faults
                //
                if(SUCCEEDED(hr = SetVidMmFault(0)))
                {

                    //
                    //  Check allocation content
                    //

                    if(SUCCEEDED(hr))
                    {
                        for(i = 0; i < ALLOC_COUNT; i++)
                        {
                            if(FAILED(hr = LockSurface(pSurface[i]->pSurface, pSurface[i]->dwFillPattern, 0, NULL)))
                            {
                                DBGPrintFail(m_pLog, L"LockWithFaults - checking surface after fault reset failed, hr = 0x%X, i = %d", hr, i);
                                break;
                            }       

                            H_CHECK(UnlockSurface(pSurface[i]->pSurface));
                        }
                    }
                }
                else
                {
                    DBGPrintFail(m_pLog, L"LockWithFaults - failed to reset VidMm fault, hr = 0x%X", hr);
                }
            }
        }
    }
    else
    {
        DBGPrintFail(m_pLog, L"LockWithFaults - failed to set VidMm fault, hr = 0x%X", hr);
    }

exit:

    DestroyAllocations(&pSurface, ALLOC_COUNT);

    SetVidMmFault(0);
}


void CLockConfirmLockWithFaults::EndTest()
{
}


CLockConfirmEvictWhileAccessed::CLockConfirmEvictWhileAccessed()
: m_bThreadStarted(false)
{
    SetDescription(L"This test will read and write to locked allocation while it is being evicted");
}

bool CLockConfirmEvictWhileAccessed::CapsCheck()
{
    //
    //	Run this test only once
    //
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    // run only in windowed mode to cut on execution
    if(bWindowed)
    {
        eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

        if(type == Texture)
        {
            UINT Width = 0, Height = 0;
            Width = GetParam<UINT>(L"AllocWidth");
            Height = GetParam<UINT>(L"AllocHeight");

            if((Width == 2048) && (Height == 2048))
            {
                return true;
            }
        }
    }

    return false;
}

void CLockConfirmEvictWhileAccessed::RunTest()
{
    Init();

    HRESULT hr = S_OK;
    UINT i = 0;
    DYNAMIC_TEXTURE TestTexture;
    HANDLE hThread = NULL;

    //
    //	Create lockable texture
    //
    hr = CreateDynamicTexture(&TestTexture, m_Width, m_Height, 1);

    if(SUCCEEDED(hr))
    {

        //
        //	Create a thread from which SuspendCPUAccess will be called causing eviction
        //  from the framebuffer of all locked surfaces (except primaries)
        //
        hThread = CreateThread(0, 0, &CLockConfirmEvictWhileAccessed::SuspendCPUAccessThreadProc, this, 0, 0);

        while(!m_bThreadStarted)
        {
            Sleep(100);
        }

        //
        //	While suspend thread is running, access allocation with CPU for reading and writing,
        //  verifying the pattern
        //

        UINT i = 0;

        while(WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT)
        {
            //
            //	Lock allocation for writing
            //
            hr = LockSurface(TestTexture.pSurface, 0, TestTexture.dwFillPattern + i);

            if(FAILED(hr))
            {
                DBGPrintFail(m_pLog, L"EvictWhileAccessed - Failed to lock texture for writing, hr = 0x%X, i = %d", hr, i);
                break;
            }
            
            H_CHECK(UnlockSurface(TestTexture.pSurface));


            //
            //	Allocation could have been evicted while we were writing to it. 
            //  Use it to have it paged back into memory
            //
            RenderTexturedQuad(TestTexture.pVidMemTexture);

            //
            //	Lock allocation for reading
            //
            hr = LockSurface(TestTexture.pSurface, TestTexture.dwFillPattern + i, 0);

            if(FAILED(hr))
            {
                DBGPrintFail(m_pLog, L"EvictWhileAccessed - Failed to lock texture for reading, i = %d, hr = 0x%X", i, hr);
                break;
            }

            H_CHECK(UnlockSurface(TestTexture.pSurface));

            //
            //	Allocation could have been evicted while we were reading from it. 
            //  Use it to have it paged back into memory
            //
            RenderTexturedQuad(TestTexture.pVidMemTexture);
            
            i++;
        }
    }
    else
    {
        DBGPrintSkip(m_pLog, L"EvictWhileAccessed - failed to create dynamic texture, hr = 0x%X", hr);
    }

    if(FAILED(hr))
    {
        if(hThread)
        {
            //
            //	Indicate to suspend thread to exit
            //
            m_bThreadStarted = false;

            WaitForSingleObject(hThread, INFINITE);
        }
    }
}

void CLockConfirmEvictWhileAccessed::EndTest()
{
}

DWORD CLockConfirmEvictWhileAccessed::SuspendCPUAccessThreadProc(void* context)
{
    CLockConfirmEvictWhileAccessed* pThis = (CLockConfirmEvictWhileAccessed*) context;

    D3DKMT_VIDMM_ESCAPE VidMMEscape;
    memset(&VidMMEscape, 0, sizeof(VidMMEscape));
    VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_SUSPEND_CPU_ACCESS_TEST;

    D3DKMT_ESCAPETYPE EscapeType = D3DKMT_ESCAPE_VIDMM;
    void* pPrivateDriverData = &VidMMEscape;
    UINT nPrivateDriverDataSize = sizeof(VidMMEscape);

    D3DDDI_ESCAPEFLAGS Flags;
    Flags.Value = 0;
    Flags.HardwareAccess = 1;

    pThis->m_bThreadStarted = true;

    for(UINT i = 0; i < 100; i++)
    {
        if(!pThis->m_bThreadStarted || !pThis->m_pDevice)
        {
            break;
        }

        pThis->m_pDevice->Escape(EscapeType,
                                 Flags,
                                 pPrivateDriverData,
                                 nPrivateDriverDataSize);

        Sleep(1000);
    }


    return 0;
}




CLockConfirmSwizzlingRangeContentionAndPaging::CLockConfirmSwizzlingRangeContentionAndPaging()
: m_bThreadRunning(false)
{
    SetDescription(L"This test will lock allocation without unlocking to create contention for swizzling ranges, \
                    at the same time some of the locked allocation will be evicted freeing the ranges");
}

bool CLockConfirmSwizzlingRangeContentionAndPaging::CapsCheck()
{
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    // run only in windowed mode to cut on execution
    if(bWindowed)
    {
        eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

        if(type == Texture)
        {
            return CapsCheckForTextureSupport();
        }

        return true;
    }

    return false;
}


void CLockConfirmSwizzlingRangeContentionAndPaging::RunTest()
{
    Init();

    UINT i = 0;
    HRESULT hr = S_OK;
    SURFACE** pSurface;
    HANDLE hThread = NULL;
    const UINT AllocationCount = 274;


    hr = CreateAllocations(&pSurface, AllocationCount);
    if(FAILED(hr))
    {
        DBGPrintSkip(m_pLog, L"SwizzlingRangeContentionAndPaging - Failed to create allocations, hr = 0x%X", hr);
        goto exit;
    }

    //
    //	Start eviction thread
    //
    hThread = CreateThread(0, 0, &CLockConfirmSwizzlingRangeContentionAndPaging::EvictionThreadProc, this, 0, 0);

    while(!m_bThreadRunning)
    {
        Sleep(100);
    }	

    for(UINT y = 0; y < 2; y++)
    {
        //
        //  Lock allocations without unlocking
        //

        for(i = 0; i < AllocationCount; i++)
        {
            //
            //	Allocation could have been paged, use it to bring it to video memory
            //
            UseSurface(m_pDevice, pSurface[i]->pSurface);

            DWORD newPattern = (0 != pSurface[i]->dwFillPattern + 1) ? (pSurface[i]->dwFillPattern + 1) : 1;
            if(FAILED(hr = LockSurface(pSurface[i]->pSurface, 0, newPattern, NULL)))
            {
                DBGPrintFail(m_pLog, L"SwizzlingRangeContentionAndPaging - Failed to lock non-pinned allocation, iteration %d, hr = 0x%X", i, hr);
                break;
            }

            pSurface[i]->dwFillPattern = newPattern;
        }

        while(i)
        {
            H_CHECK(UnlockSurface(pSurface[--i]->pSurface));
        }


        //
        //  Check allocation content
        //

        if(SUCCEEDED(hr))
        {
            for(i = 0; i < AllocationCount; i++)
            {
                if(FAILED(hr = LockSurface(pSurface[i]->pSurface, pSurface[i]->dwFillPattern, 0, NULL)))
                {
                    DBGPrintFail(m_pLog, L"SwizzlingRangeContentionAndPaging - checking surface failed, hr = 0x%X, i = %d", hr, i);
                    break;
                }       

                H_CHECK(UnlockSurface(pSurface[i]->pSurface));
            }
        }
    }

exit:

    if(hThread)
    {
        //
        //	Signal eviction thread to exit
        //
        m_bThreadRunning = false;
        WaitForSingleObject(hThread, INFINITE);
    }

    DestroyAllocations(&pSurface, AllocationCount);
}


void CLockConfirmSwizzlingRangeContentionAndPaging::EndTest()
{
}

DWORD CLockConfirmSwizzlingRangeContentionAndPaging::EvictionThreadProc(void* context)
{
    CLockConfirmSwizzlingRangeContentionAndPaging* pThis = (CLockConfirmSwizzlingRangeContentionAndPaging*)context;

    //
    // Create device
    //
    CI3DDevicePtr pDevice;
    pThis->CreateAdditionalDevice(&pDevice.p);


    pThis->m_bThreadRunning = true;

    while(pThis->m_bThreadRunning && pThis->m_pDevice)
    {
        try
        {
            pThis->CauseEvictions(pDevice);
        }
        catch(...)
        {
            continue;
        }

    }

    return 0;
}



CLockConfirmLockWhenOutOfMemory::CLockConfirmLockWhenOutOfMemory()
:m_numAllocations(0)
{	
    SetDescription(L"This test will lock allocation when process is out of virtual address space");

    ZeroMemory(m_Allocs, sizeof(void*) * m_cMaxAllocations);
}

bool CLockConfirmLockWhenOutOfMemory::CapsCheck()
{
    DeviceCreateOptions Opts;
    m_pDevice->GetCreationOptions(&Opts);
    bool bWindowed = Opts.bWindowed;

    // run only in windowed mode to cut on execution
    if(bWindowed)
    {
        // Run only on 32-bit as it is harder to get out of VA on 64-bit
        if(Is32BitAddressSpace())
        {
            eALLOC_TYPE type = GetParam<eALLOC_TYPE>(L"LockConfirmAllocType");

            if(type == Texture)
            {
                return CapsCheckForTextureSupport();
            }

            return false;
        }
    }

    return false;
}


void CLockConfirmLockWhenOutOfMemory::ConsumeVirtualMemory()
{
    while(1)
    {
        if(m_numAllocations >= m_cMaxAllocations)
        {
            //
            //	We shouldn't get here, if we did, something is wrong
            //
            H_CHECK(E_FAIL);
            break;
        }

        m_Allocs[m_numAllocations] = VirtualAlloc(NULL, 64*1024, MEM_RESERVE, PAGE_READWRITE);

        if(!m_Allocs[m_numAllocations])
        {
            break;
        }

        m_numAllocations++;
    }
}


void CLockConfirmLockWhenOutOfMemory::FreeVirtualMemory()
{
    while(m_numAllocations)
    {
        m_numAllocations--;
        if(!VirtualFree(m_Allocs[m_numAllocations], 0, MEM_RELEASE))  
        {
            DWORD dwError = GetLastError();
            DBGPrintSkip(m_pLog, L"Failed to free allocated memory, error = %d", dwError);
        }
        m_Allocs[m_numAllocations] = NULL;
    }
}

void CLockConfirmLockWhenOutOfMemory::RunTest()
{
    Init();

    HRESULT hr = S_OK;
    DYNAMIC_TEXTURE VidMemTexture; 
    D3DDDIFORMAT Format;
    DWORD dwPattern = 0xabcdabcd;

    //
    //	Get format
    //
    H_CHECK(I3DXGetValidResourceFormat(m_pDevice, I3DFORMAT_ARGB, I3DSURF_DYNAMIC|I3DSURF_TEXTURE, false, &Format));

    if(SUCCEEDED(hr = m_pDevice->CreateTexture(&VidMemTexture.pVidMemTexture.p,
                                                GetParam<UINT>(L"AllocWidth"),
                                                GetParam<UINT>(L"AllocHeight"),
                                                1,
                                                I3DSURF_DYNAMIC|I3DSURF_TEXTURE,
                                                Format,
                                                D3DDDIPOOL_VIDEOMEMORY,
                                                false)))
    {

        H_CHECK(VidMemTexture.pVidMemTexture->GetSurface(0, &VidMemTexture.pSurface.p));

        //
        //	Consume all virtual memory
        //
        ConsumeVirtualMemory();

        //
        //	Now that we don't have a lot of VA left, lock allocation. Locking will result in locking with alternate VA on
        //	some IHVs, at which point VidMM will try to allocate alternate VA backing store. But because there is not
        //	enough virtual memory is left that might fail. Driver should try very hard then to succeed the lock
        //

        hr = LockSurface(VidMemTexture.pSurface, 0, dwPattern);

        if(SUCCEEDED(hr))
        {
            H_CHECK(UnlockSurface(VidMemTexture.pSurface));

            //
            //	To make sure we got valid pointer from previous lock, release all consumed memory,
            //	lock again and read allocation content
            //
            FreeVirtualMemory();

            hr = LockSurface(VidMemTexture.pSurface, dwPattern, 0);
            if(FAILED(hr))
            {
                DBGPrintFail(m_pLog, L"Failed to lock allocation, hr = 0x%X", hr);
            }
            else
            {
                H_CHECK(UnlockSurface(VidMemTexture.pSurface));
            }
        }
    }
    else
    {
        DBGPrintSkip(m_pLog, L"CLockConfirmLockWhenOutOfMemory - failed to create allocation, hr = 0x%X", hr);
    }

    FreeVirtualMemory();
}

void CLockConfirmLockWhenOutOfMemory::EndTest()
{
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