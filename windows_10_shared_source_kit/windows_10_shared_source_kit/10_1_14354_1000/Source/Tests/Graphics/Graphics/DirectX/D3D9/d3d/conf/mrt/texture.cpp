#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// One Texture Test functions

CMRTOneTexture::CMRTOneTexture()
{
	m_szTestName = "MRT One Texture";
	m_szCommandKey = "OneTex";

	// Tell parent how many render targets to use
	nNumRTs = 1;
}

//************************************************************************
// Two Texture Test functions

CMRTTwoTexture::CMRTTwoTexture()
{
	m_szTestName = "MRT Two Textures";
	m_szCommandKey = "TwoTex";

	// Tell parent how many render targets to use
	nNumRTs = 2;
}

//************************************************************************
// Three Texture Test functions

CMRTThreeTexture::CMRTThreeTexture()
{
	m_szTestName = "MRT Three Textures";
	m_szCommandKey = "ThreeTex";

	// Tell parent how many render targets to use
	nNumRTs = 3;
}

//************************************************************************
// Four Texture Test functions

CMRTFourTexture::CMRTFourTexture()
{
	m_szTestName = "MRT Four Textures";
	m_szCommandKey = "FourTex";

	// Tell parent how many render targets to use
	nNumRTs = 4;
}
