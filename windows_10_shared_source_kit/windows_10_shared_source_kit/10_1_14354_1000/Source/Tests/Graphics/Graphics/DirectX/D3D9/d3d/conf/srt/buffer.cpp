#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

bool CSRT::CreateBuffers(bool bCreateRT, bool bCreateZ)
{
    bool bResult;
    SURFACEDESC Desc;
    CSurface *pTemp;

    // Create the Render Target
    if (bCreateRT)
    {
        // Get target dimensions
        pOriginalRT->GetSurface(&pTemp,1);
        pTemp->GetDesc(&Desc);
        RELEASE(pTemp);

        // Create an offscreen surface for rendering
        bResult = CreateRenderTarget(Desc.dwWidth, 
                                     Desc.dwHeight,
                                     m_pCommonTextureFormats[nTexFormat],
                                     MULTISAMPLE_NONE,
                                     true,
                                     &pOffScreenRT); 
        if (!bResult)
        {
	        WriteToLog("CreateRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		    RELEASE(pOffScreenRT);
		    return false;
	    }
    }

    // Create the depth buffer
    if (bCreateZ)
    {
        // Get Z buffer dimensions
        if (bCreateRT || bNull)
        {
            pOriginalZ->GetSurface(&pTemp,1);
            pTemp->GetDesc(&Desc);
            RELEASE(pTemp);
        }

        // Create an offscreen Z buffer
        bResult = CreateDepthStencilSurface((bCreateRT || bNull) ? Desc.dwWidth : nWidthRT, 
                                            (bCreateRT || bNull) ? Desc.dwHeight : nHeightRT,
                                            m_pCommonZBufferFormats[nZFormat],
                                            MULTISAMPLE_NONE,
                                            &pTempZ); 
        if (!bResult)
        {
	        WriteToLog("CreateDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		    RELEASE(pTempZ);
		    return false;
	    }
    }

    return true;
}
