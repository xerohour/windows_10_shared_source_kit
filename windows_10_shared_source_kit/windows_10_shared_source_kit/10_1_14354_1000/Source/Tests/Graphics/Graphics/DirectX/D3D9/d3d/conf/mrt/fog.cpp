#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// Fog Test functions

CMRTFog::CMRTFog()
{
	m_szTestName = "MRT Fog";
	m_szCommandKey = "Fog";

    // Tell parent that we are a fog case
	bFog = true;
}

//************************************************************************
// MET Fog Test functions

CMETFog::CMETFog()
{
	m_szTestName = "MET Fog";
	m_szCommandKey = "METFog";

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_METN;

    // Tell parent that we are a fog case
	bFog = true;

    // Tell parent that we are a METex case
	bMETex = true;
}
