#include "badapp.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "time.h"
#include "ComPtr.h"




CBadApp::CBadApp()
{
    SetDescription(L"This component submitts command buffers that take a large amount of rendering time");

    AddParam<UINT>(L"TotalTime", L"The total number of seconds to run for");
    AddParam<UINT>(L"FillRate", L"The number of pixels to fill with each DrawPrimitive call (in millons)");
    AddParam<bool>(L"Increase", L"If true, then the number of pixels rendered per draw primitive call will increase from 0 to the maximum as time progresses");
    
    SetParamTestDomainSingleValue<UINT>(L"TotalTime", 60);
    SetParamTestDomainSingleValue<UINT>(L"FillRate", 100);
    SetParamTestDomainSingleValue<bool>(L"Increase", false);
}

void CBadApp::PreWalk(CLogWriter* pLog)
{
    const UINT nTotalTime = GetParam<UINT>(L"TotalTime");
    const ULONGLONG nPixelsPerCall = ((ULONGLONG)GetParam<UINT>(L"FillRate")) * 1000 * 1000;
    const bool bIncrease = GetParam<bool>(L"Increase");

    IDirect3DDevice9Ex* pDevice = GetSemantic<LPDIRECT3DDEVICE9EX>(L"d3ddeviceex");
    
    //get the current render target, so that it can be restored
    CComPtr< IDirect3DSurface9 > pRenderTarget;
    H_CHECK(pDevice->GetRenderTarget(0, &pRenderTarget.p));
        
    D3DSURFACE_DESC RTDesc;
    H_CHECK(pRenderTarget->GetDesc(&RTDesc));
    
    const UINT nPixelsPerQuad = RTDesc.Width * RTDesc.Height;
    
    const UINT nQuadsPerCall = (UINT) ((nPixelsPerCall / nPixelsPerQuad) + 1);//round up

    const UINT nVertices = nQuadsPerCall * 2 * 3;
    
    struct Vertex
    {
        D3DXVECTOR4 Pos;//x y z rhw
        DWORD Color;
    };
    const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

    CComPtr< IDirect3DVertexBuffer9 > pVB;
    H_CHECK(pDevice->CreateVertexBuffer(sizeof(Vertex) * nVertices,
                                          D3DUSAGE_WRITEONLY,
                                          FVF,
                                          D3DPOOL_DEFAULT,
                                          &pVB.p,
                                          NULL));
    
    //fill the VB
    Vertex* pVertices = NULL;
    H_CHECK(pVB->Lock(0, 0, (void**) &(pVertices), 0));
    
    for(UINT i = 0; i < nQuadsPerCall; i++)
    {
        Vertex* pStart = &pVertices[i * 3 * 2];
        pStart[0].Pos = D3DXVECTOR4(0.0f, 0.0f, 0.5, 1.0f);
        pStart[1].Pos = D3DXVECTOR4(1.0f, 0.0f, 0.5, 1.0f);
        pStart[2].Pos = D3DXVECTOR4(0.0f, 1.0f, 0.5, 1.0f);
        pStart[3].Pos = D3DXVECTOR4(0.0f, 1.0f, 0.5, 1.0f);
        pStart[4].Pos = D3DXVECTOR4(1.0f, 0.0f, 0.5, 1.0f);
        pStart[5].Pos = D3DXVECTOR4(1.0f, 1.0f, 0.5, 1.0f);        
    }
    
    for(UINT i = 0; i < nVertices; i++)
    {
        pVertices[i].Color = 0xFFFFFFFF;
        pVertices[i].Pos.x *= (float)RTDesc.Width;
        pVertices[i].Pos.y *= (float)RTDesc.Height;
        pVertices[i].Pos.z = 1.0f - (i / (float)nVertices);//back to front
        pVertices[i].Pos.w = 1.0f;
    }    

    H_CHECK(pVB->Unlock());

    H_CHECK(pDevice->SetFVF(FVF));
    H_CHECK(pDevice->SetStreamSource(0, pVB, 0, sizeof(Vertex)));

    time_t StartTime = time(NULL);
    time_t CurrentTime = StartTime;

    UINT nPrimitives;

    if(bIncrease)
    {
        nPrimitives = 1;
        
    }
    else
    {
        nPrimitives = nQuadsPerCall * 2;
    }
    
    

    do
    {
        H_CHECK(pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFF00FF, 1.0f, 0));

        H_CHECK(pDevice->BeginScene());

        H_CHECK(pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                       0,
                                       nPrimitives));
        
        H_CHECK(pDevice->EndScene());

        H_CHECK(pDevice->Present(0,0,0,0));

        CurrentTime = time(NULL);

        if(bIncrease)
        {
            float fTime = (CurrentTime - StartTime) / (float)nTotalTime;
            
            nPrimitives = (UINT) (fTime * nQuadsPerCall * 2);
            if(0 == nPrimitives)
            {
                nPrimitives = 1;
            }
            if(nPrimitives > (nQuadsPerCall * 2))
            {
                nPrimitives = nQuadsPerCall * 2;
            }
        }
        
    } while(((UINT)(CurrentTime - StartTime)) <= nTotalTime);
}


void CBadApp::PostWalk(CLogWriter* pLog)
{
}
