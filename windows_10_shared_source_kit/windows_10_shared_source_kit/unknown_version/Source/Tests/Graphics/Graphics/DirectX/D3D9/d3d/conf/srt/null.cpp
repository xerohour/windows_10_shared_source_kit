#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

//************************************************************************
// Null Test functions

CSRTNull::CSRTNull()
{
	m_szTestName = "SRT Null";
	m_szCommandKey = "Null";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    // Tell parent that we are a Null case
    bNull = true;
}

//************************************************************************
// No ZBuffer Null Test functions

CSRTNoZNull::CSRTNoZNull()
{
	m_szTestName = "SRT No ZBuffer Null";
	m_szCommandKey = "NoZNull";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;

    // Tell parent that we are a Null case
    bNull = true;
}

