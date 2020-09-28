#define D3D_OVERLOADS
#include <d3d8.h>
#include "types.h"
#include "AlphaBld.h"

//************************************************************************
// SwapEffect Test functions

CSwapEffectTest::CSwapEffectTest()
{
    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;

	// Tell parent class what caps to look for
	dwSrcCap = D3DPBLENDCAPS_DESTALPHA;
	dwDestCap = D3DPBLENDCAPS_INVDESTALPHA;

	// Tell parent class what render states to set
	dwSrcBlend = D3DBLEND_DESTALPHA;
	dwDestBlend = D3DBLEND_INVDESTALPHA;

	// Give parent a printable name for the caps
	szSrcName = "DestAlpha";
	szDestName = "InvDestAlpha";

	// Tell parent that we want the checker background
	bChecker = true;

	// Indicate that this state is available for separate alpha
	bSeparateAlpha = true;
}

bool CSwapEffectTest::FilterResolution(PDISPLAYMODE pMode)
{
    if (!m_DeviceList[0])
    {
        DPF(1, _T("CSwapEffectCopyTest::FilterResolution() - invalid device list.\n"));
        return false;
    }

    ADAPTER *pAdapter = m_DeviceList[0]->GetAdapter();

    if (!pAdapter)
    {
        DPF(1, _T("CSwapEffectCopyTest::FilterResolution() - unable to get adapter.\n"));
        return false;
    }

	DWORD Resolution[3] = {640,480,(DWORD)pAdapter->DesktopMode.Format.d3dfFormat};

    if(Resolution[0] == pMode->dwWidth && 
       Resolution[1] == pMode->dwHeight &&
       (D3DFORMAT)Resolution[2] == pMode->Format.d3dfFormat)
        return true;

	return false;
}

//************************************************************************
// SwapEffectCopy Test functions

CSwapEffectCopyTest::CSwapEffectCopyTest()
{
	m_szTestName = "Swap Effect Copy";
	m_szCommandKey = "SwapEffectCopy";

    // Init Framework options
	m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_COPY;
}

//************************************************************************
// SwapEffectFlip Test functions

CSwapEffectFlipTest::CSwapEffectFlipTest()
{
	m_szTestName = "Swap Effect Flip";
	m_szCommandKey = "SwapEffectFlip";

    // Init Framework options
	m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_FLIP;

	// Tell parent that we need to check the swap effect cap
	bSwapEffectCap = true;
}

//************************************************************************
// SwapEffectDiscard Test functions

CSwapEffectDiscardTest::CSwapEffectDiscardTest()
{
	m_szTestName = "Swap Effect Discard";
	m_szCommandKey = "SwapEffectDiscard";

    // Init Framework options
	m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_DISCARD;

	// Tell parent that we need to check the swap effect cap
	bSwapEffectCap = true;
}
