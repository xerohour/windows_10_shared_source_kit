#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

//************************************************************************
// Invalid Test functions

CSRTInvalid::CSRTInvalid()
{
	m_szTestName = "SRT Invalid";
	m_szCommandKey = "Invalid";

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;

    // Tell parent that we are the Invalid case
    bInvalid = true;
}
