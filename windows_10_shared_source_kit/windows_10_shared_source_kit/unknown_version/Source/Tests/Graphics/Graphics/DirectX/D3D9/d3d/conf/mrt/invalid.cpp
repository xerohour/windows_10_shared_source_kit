#include <d3d9.h>
#include "MRT.h"

//************************************************************************
// Invalid Test functions

CMRTInvalid::CMRTInvalid()
{
	m_szTestName = "MRT Invalid";
	m_szCommandKey = "Invalid";

	// Tell framework not to compare with Ref 
	m_Options.TestOptions.bCompareSrcRef = false;

	// Tell parent how many render targets to use
	nNumRTs = 4;

	// Indicate that we are an invalid test case and want to test everything
    bColorWrite = true;
	bInvalid = true;
	bBlend = true;
	bFog = true;
}
