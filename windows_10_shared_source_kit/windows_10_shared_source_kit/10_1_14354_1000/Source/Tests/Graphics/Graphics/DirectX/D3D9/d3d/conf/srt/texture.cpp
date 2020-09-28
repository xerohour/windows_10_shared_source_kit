#define D3D_OVERLOADS
#include <d3d.h>
#include "Srt.h"

//************************************************************************
// Texture Test functions

CSRTTexture::CSRTTexture()
{
	m_szTestName = "SRT Texture";
	m_szCommandKey = "Texture";
}

//************************************************************************
// No ZBuffer Texture Test functions

CSRTNoZTexture::CSRTNoZTexture()
{
	m_szTestName = "SRT No ZBuffer Texture";
	m_szCommandKey = "NoZTexture";

    // Disable ZBuffers
    m_Options.D3DOptions.bZBuffer = false;
}
