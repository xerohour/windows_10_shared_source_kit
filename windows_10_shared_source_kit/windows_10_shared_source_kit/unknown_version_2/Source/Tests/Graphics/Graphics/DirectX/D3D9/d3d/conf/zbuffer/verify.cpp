#define D3D_OVERLOADS
#include <d3d.h>
#include "ZBuffer.h"
#include "CSurface.h"

//************************************************************************
// Verify Test functions

CZVerifyTest::CZVerifyTest()
{
	m_szTestName = "Verify Z Buffer Formats";
	m_szCommandKey = "Verify";

	// Only initialize the source
    m_Options.D3DOptions.bReference = false;
}

bool CZVerifyTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	errString[0] = '\0';
	msgString[0] = '\0';

	// Perform some Z buffer validation
	bValidFormat = VerifyFormat();

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CZVerifyTest::SceneRefresh(void)
{
}

bool CZVerifyTest::ProcessFrame(void)
{
    bValidFormat ? Pass() : Fail();

	// Print any errors to the log
	if (errString[0] != '\0')
		WriteToLog(errString);

    return bValidFormat;
}

bool CZVerifyTest::VerifyFormat(void)
{
    SURFACEDESC Desc;
    CSurface *pZBuffer = m_pSrcDevice->GetZBuffer();
    bool bError = false;

    // Get the ZBuffer surface description
	pZBuffer->GetDesc(&Desc);

    if (m_dwVersion >= 0x0800)
    {
   	    // Report the format
        switch (Desc.Format.d3dfFormat)
        {
            case FMT_D15S1:
                sprintf(msgString,"%sFormat: D3DFMT_D15S1, ",msgString);
                break;
            case FMT_D16:
                sprintf(msgString,"%sFormat: D3DFMT_D16, ",msgString);
                break;
            case FMT_D24X4S4:
                sprintf(msgString,"%sFormat: D3DFMT_D24X4S4, ",msgString);
                break;
            case FMT_D24X8:
                sprintf(msgString,"%sFormat: D3DFMT_D24X8, ",msgString);
                break;
            case FMT_D24S8:
                sprintf(msgString,"%sFormat: D3DFMT_D24S8, ",msgString);
                break;
            case FMT_D24FS8:
                sprintf(msgString,"%sFormat: D3DFMT_D24FS8, ",msgString);
                break;
            case FMT_D32:
                sprintf(msgString,"%sFormat: D3DFMT_D32, ",msgString);
                break;
            case FMT_D16_LOCKABLE:  
                sprintf(msgString,"%sFormat: D3DFMT_D16_LOCKABLE, ",msgString);
                break;
            case FMT_D32F_LOCKABLE:  
                sprintf(msgString,"%sFormat: D3DFMT_D32F_LOCKABLE, ",msgString);
                break;
            default: 
                sprintf(msgString,"%sFormat: %d, ",msgString,Desc.Format.d3dfFormat);
                break;
        };

        sprintf(msgString,"%sUsage: 0x%x",msgString,Desc.Usage);

	    // Check for invalid usage
        if (!(Desc.Usage & USAGE_DEPTHSTENCIL))
	    {
		    sprintf(errString,"%sError: D3DUSAGE_DEPTHSTENCIL not set on Z Buffer Usage!\n",errString);
		    bError = true;
	    }

	    // Check for invalid formats
        if ((Desc.Format.d3dfFormat != FMT_D16)			&&
            (Desc.Format.d3dfFormat != FMT_D15S1)		&&
            (Desc.Format.d3dfFormat != FMT_D24X4S4)		&&
            (Desc.Format.d3dfFormat != FMT_D24X8)		&&
            (Desc.Format.d3dfFormat != FMT_D24S8)		&&
            (Desc.Format.d3dfFormat != FMT_D24FS8)		&&
            (Desc.Format.d3dfFormat != FMT_D32)			&&
            (Desc.Format.d3dfFormat != FMT_D16_LOCKABLE)&&
            (Desc.Format.d3dfFormat != FMT_D32F_LOCKABLE))
	    {
		    sprintf(errString,"%sError: %d is an unknown Z Buffer Format!\n",errString,Desc.Format.d3dfFormat);
		    bError = true;
	    }
    }
    else
    {
        PIXELFORMAT *pDDPF      = &(Desc.Format.ddpfPixelFormat);

        DWORD dwFlags			= pDDPF->dwFlags;
        DWORD dwTotalBitDepth	= pDDPF->dwZBufferBitDepth;
        DWORD dwStencilBitDepth	= pDDPF->dwStencilBitDepth;
        DWORD dwZBitMask		= pDDPF->dwZBitMask;
        DWORD dwStencilBitMask	= pDDPF->dwStencilBitMask;
	    DWORD dwZBitCount		= 0;
	    DWORD dwStencilBitCount	= 0;
        DWORD mask;

        // Count number of bits for each component
        for (mask = pDDPF->dwZBitMask; mask; mask>>=1)
            dwZBitCount += (mask & 0x1);
        for (mask = pDDPF->dwStencilBitMask; mask; mask>>=1)
            dwStencilBitCount += (mask & 0x1);

   	    // Report the format
	    sprintf(msgString,"%s%d-bit Z Buffer, Z: %d-bit (0x%x)",msgString,dwTotalBitDepth,dwZBitCount,dwZBitMask);

        if (dwFlags & PF_STENCILBUFFER)
	    {
		    sprintf(msgString,"%s, Stencil: %d-bit (0x%x)",msgString,dwStencilBitCount,dwStencilBitMask);
	    }

	    // Check for invalid formats
        if (dwZBitCount != (dwTotalBitDepth - dwStencilBitDepth))
	    {
			// Ignore the inconsistant bit depth for the 32 bit,24/4 or 24/0 formats
			if (!((dwZBitCount == 24) && ((dwStencilBitCount == 4) || (dwStencilBitCount == 0))))
			{
				sprintf(errString,"%sError: ZBitMask inconsistant with bit depth value!\n",errString);
				bError = true;
			}
	    }
        if (dwStencilBitCount != dwStencilBitDepth)
	    {
		    sprintf(errString,"%sError: StencilBitMask inconsistant with bit depth value!\n",errString);
		    bError = true;
	    }
        if (!(dwFlags & PF_ZBUFFER))
	    {
		    sprintf(errString,"%sError: DDPF_ZBUFFER not set on enumerated Z buffer!\n",errString);
		    bError = true;
	    }
    }

	return (!bError);
}

