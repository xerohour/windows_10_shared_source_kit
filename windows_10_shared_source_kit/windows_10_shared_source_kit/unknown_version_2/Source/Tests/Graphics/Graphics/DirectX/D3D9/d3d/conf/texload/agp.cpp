#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

//************************************************************************
// AGP Test functions

CAGPTest::CAGPTest()
{
	m_szTestName = "TexLoad AGP";
	m_szCommandKey = "AGP";

	// Inidicate that we are a AGP test
	bAGP = true;
}

CAGPTest::~CAGPTest()
{
}

//************************************************************************
// MipMap AGP Test functions

CMipMapAGPTest::CMipMapAGPTest()
{
	m_szTestName = "TexLoad MipMap_AGP";
	m_szCommandKey = "MipMap_AGP";

	// Inidicate that we are a MipMap & AGP test
	bAGP = true;
	bMipMap = true;
}

CMipMapAGPTest::~CMipMapAGPTest()
{
}
