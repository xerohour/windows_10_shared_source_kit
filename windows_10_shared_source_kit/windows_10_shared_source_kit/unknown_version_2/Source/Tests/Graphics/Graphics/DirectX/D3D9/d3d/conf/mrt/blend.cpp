#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// One Blend Test functions

CMRTOneBlend::CMRTOneBlend()
{
	m_szTestName = "MRT One Texture Blend";
	m_szCommandKey = "OneBlend";

	// Tell parent how many render targets to use
	nNumRTs = 1;

    // Tell parent that we are a blend case
    bBlend = true;
}

//************************************************************************
// Two Blend Test functions

CMRTTwoBlend::CMRTTwoBlend()
{
	m_szTestName = "MRT Two Texture Blend";
	m_szCommandKey = "TwoBlend";

	// Tell parent how many render targets to use
	nNumRTs = 2;

    // Tell parent that we are a blend case
    bBlend = true;
}

//************************************************************************
// Three Blend Test functions

CMRTThreeBlend::CMRTThreeBlend()
{
	m_szTestName = "MRT Three Texture Blend";
	m_szCommandKey = "ThreeBlend";

	// Tell parent how many render targets to use
	nNumRTs = 3;

    // Tell parent that we are a blend case
    bBlend = true;
}

//************************************************************************
// Four Blend Test functions

CMRTFourBlend::CMRTFourBlend()
{
	m_szTestName = "MRT Four Texture Blend";
	m_szCommandKey = "FourBlend";

	// Tell parent how many render targets to use
	nNumRTs = 4;

    // Tell parent that we are a blend case
    bBlend = true;
}
