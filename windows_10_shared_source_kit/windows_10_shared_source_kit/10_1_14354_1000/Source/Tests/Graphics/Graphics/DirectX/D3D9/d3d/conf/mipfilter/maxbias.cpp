#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

//************************************************************************
// Point_MaxMipLevel Test functions

CPoint_MaxMipLevelTest::CPoint_MaxMipLevelTest()
{
	m_szTestName = "MipFilter Point_MaxMipLevel";
	m_szCommandKey = "Point_MaxMipLevel";

	// Inidicate that we are a Max test
	bMax = true;
}

//************************************************************************
// Linear_MaxMipLevel Test functions

CLinear_MaxMipLevelTest::CLinear_MaxMipLevelTest()
{
	m_szTestName = "MipFilter Linear_MaxMipLevel";
	m_szCommandKey = "Linear_MaxMipLevel";

	// Inidicate that we are a MipLinear & Max test
	bMax = true;
	bMipLinear = true;
}

//************************************************************************
// None_MaxMipLevel Test functions

CNone_MaxMipLevelTest::CNone_MaxMipLevelTest()
{
	m_szTestName = "MipFilter None_MaxMipLevel";
	m_szCommandKey = "None_MaxMipLevel";

	// Inidicate that we are a MipNone & Max test
	bMax = true;
	bMipNone = true;
}

//************************************************************************
// Point_LODBias Test functions

CPoint_LODBiasTest::CPoint_LODBiasTest()
{
	m_szTestName = "MipFilter Point_LODBias";
	m_szCommandKey = "Point_LODBias";

	// Inidicate that we are a Bias test
	bBias = true;
}

//************************************************************************
// Linear_LODBias Test functions

CLinear_LODBiasTest::CLinear_LODBiasTest()
{
	m_szTestName = "MipFilter Linear_LODBias";
	m_szCommandKey = "Linear_LODBias";

	// Inidicate that we are a MipLinear & Bias test
	bBias = true;
	bMipLinear = true;
}

//************************************************************************
// None_LODBias Test functions

CNone_LODBiasTest::CNone_LODBiasTest()
{
	m_szTestName = "MipFilter None_LODBias";
	m_szCommandKey = "None_LODBias";

	// Inidicate that we are a MipNone & Bias test
	bBias = true;
	bMipNone = true;
}
