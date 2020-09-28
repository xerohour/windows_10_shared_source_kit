#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

//************************************************************************
// Point_NonSquareW Test functions

CPoint_NonSquareWTest::CPoint_NonSquareWTest()
{
	m_szTestName = "MipFilter Point_NonSquareWidth";
	m_szCommandKey = "Point_NonSquareWidth";

	// Inidicate that we are a NonSquareW test
	bNonSquareW = true;
}

//************************************************************************
// Point_NonSquareH Test functions

CPoint_NonSquareHTest::CPoint_NonSquareHTest()
{
	m_szTestName = "MipFilter Point_NonSquareHeight";
	m_szCommandKey = "Point_NonSquareHeight";

	// Inidicate that we are a NonSquareH test
	bNonSquareH = true;
}

//************************************************************************
// Linear_NonSquareW Test functions

CLinear_NonSquareWTest::CLinear_NonSquareWTest()
{
	m_szTestName = "MipFilter Linear_NonSquareWidth";
	m_szCommandKey = "Linear_NonSquareWidth";

	// Inidicate that we are a MipLinear & NonSquareW test
	bMipLinear = true;
	bNonSquareW = true;
}

//************************************************************************
// Linear_NonSquareH Test functions

CLinear_NonSquareHTest::CLinear_NonSquareHTest()
{
	m_szTestName = "MipFilter Linear_NonSquareHeight";
	m_szCommandKey = "Linear_NonSquareHeight";

	// Inidicate that we are a MipLinear & NonSquareH test
	bMipLinear = true;
	bNonSquareH = true;
}
 
