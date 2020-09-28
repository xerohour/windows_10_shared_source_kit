#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// ColorWrite Test functions

CMRTColorWrite::CMRTColorWrite()
{
	m_szTestName = "MRT ColorWrite";
	m_szCommandKey = "ColorWrite";

    // Tell parent that we are a color write case
    bColorWrite = true;
}

//************************************************************************
// ColorWrite Test functions

CMETColorWrite::CMETColorWrite()
{
	m_szTestName = "MET ColorWrite";
	m_szCommandKey = "METColorWrite";

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_METN;

    // Tell parent that we are a color write case
    bColorWrite = true;

    // Tell parent that we are a METex case
	bMETex = true;
}
