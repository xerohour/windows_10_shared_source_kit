#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// Multiple Element Texture Test functions

CMETexture::CMETexture()
{
	m_szTestName = "Multiple Element Texture";
	m_szCommandKey = "MET";

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_METN;

    // Tell parent that we are a METex case
	bMETex = true;
}

//************************************************************************
// Multiple Element Texture Blend Test functions

CMETBlend::CMETBlend()
{
	m_szTestName = "MET Blend";
	m_szCommandKey = "METBlend";

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_METN;

    // Tell parent that we are a METex case
	bMETex = true;

    // Tell parent that we are a blend case
    bBlend = true;
}
