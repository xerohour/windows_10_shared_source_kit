#include "testcomponentspch.h"
#include "SharedSurfaceLock.h"
#include "pixel.h"
#include <time.h>

/*
 * Implimentation of the SharedSurfaceLock Class as described in SharedSurfaceLock.h
 */

CSharedSurfaceLock::CSharedSurfaceLock()
{
    SetDescription(L"Randomly selects an output head, locks the shared primary surface, and performs read/write testing.");
    
	AddParam<int>(L"Delay", L"Number of Milliseconds after read/write testing has completed before returning");
    
    SetParamTestDomainSingleValue<int>(L"Delay", 3000);
}

void CSharedSurfaceLock::RunTest()
{
    CI3DSwapChainPtr pSwapChain;
    H_CHECK(m_pDevice->GetSwapChain(&pSwapChain.p));

    CI3DSurface2Ptr pRenderSurface;
    if(FAILED(pSwapChain->GetFrontBuffer(&pRenderSurface.p)))
    {
        //this can fail if a mode-switch occurs during the call to GetFrontBuffer
        //So put a failure in the log.
        m_pLog->Skip(L"A mode-switch occured during a call to GetFrontBuffer");
        return;
    }

    UINT SurfaceWidth = 0;
    UINT SurfaceHeight = 0;

    H_CHECK(pRenderSurface->GetSize(&SurfaceWidth, &SurfaceHeight));

    UINT PixelSize = CPixel::BytesPerPixel(pRenderSurface->GetFormat());

    D3DLOCKED_RECT d3dLockedRect;
    H_CHECK(pRenderSurface->Lock(0, NULL, &d3dLockedRect));

    if( (PixelSize == 4) || (PixelSize == 2) )
    {
        //only touch memory for known surface formats
        ReadWriteMemory(d3dLockedRect, PixelSize, SurfaceWidth, SurfaceHeight);
    }

    H_CHECK(pRenderSurface->Unlock());
    int Delay = GetParam<int>(L"Delay");
    Sleep(Delay);
}


void CSharedSurfaceLock::EndTest()
{
}
void CSharedSurfaceLock::ReadWriteMemory(D3DLOCKED_RECT d3dLocked_Rect, UINT PixelSize, UINT SurfaceWidth, UINT SurfaceHeight)
{
    int SurfaceSize = d3dLocked_Rect.Pitch * SurfaceHeight;

    std::vector<BYTE> ScreenCopy;
    ScreenCopy.resize(SurfaceSize);
    CopyMemory(&ScreenCopy[0], d3dLocked_Rect.pBits, SurfaceSize);

    BYTE* pBits = (BYTE*) d3dLocked_Rect.pBits;
    UINT BitsOffset = 0;
    UINT ScreenCopyOffset = 0;

    for(int y1 = 0; y1 < (int)SurfaceHeight; y1++)
    {
        for(int x1 = 0; x1 < (int)SurfaceWidth; x1++)
        {
            int x2 = SurfaceWidth - x1 - 1;
            int y2 = SurfaceHeight - y1 - 1;

            BitsOffset = (x1 * PixelSize) + (y1 * d3dLocked_Rect.Pitch);
            ScreenCopyOffset = (x2 * PixelSize) + (y2 * d3dLocked_Rect.Pitch); 
            CopyMemory(pBits + BitsOffset, &ScreenCopy[ScreenCopyOffset], PixelSize);
        }
    }
}


