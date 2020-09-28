#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

//************************************************************************
// Offscreen Test functions

CSRTOffscreen::CSRTOffscreen()
{
	m_szTestName = "SRT Offscreen";
	m_szCommandKey = "Offscreen";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    // Tell parent that we are an Offscreen case
    bOffscreen = true;
}

//************************************************************************
// No ZBuffer Offscreen Test functions

CSRTNoZOffscreen::CSRTNoZOffscreen()
{
	m_szTestName = "SRT No ZBuffer Offscreen";
	m_szCommandKey = "NoZOffscreen";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;

    // Tell parent that we are an Offscreen case
    bOffscreen = true;
}

