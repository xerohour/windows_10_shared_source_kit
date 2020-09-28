#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

//************************************************************************
// Point_MinPoint Test functions

CPoint_MinPointTest::CPoint_MinPointTest()
{
	m_szTestName = "MipFilter Point_MinPoint";
	m_szCommandKey = "Point_MinPoint";
}

//************************************************************************
// Point_MinLinear Test functions

CPoint_MinLinearTest::CPoint_MinLinearTest()
{
	m_szTestName = "MipFilter Point_MinLinear";
	m_szCommandKey = "Point_MinLinear";

	// Inidicate that we are a MinLinear test
	bMinLinear = true;
}

//************************************************************************
// Linear_MinPoint Test functions

CLinear_MinPointTest::CLinear_MinPointTest()
{
	m_szTestName = "MipFilter Linear_MinPoint";
	m_szCommandKey = "Linear_MinPoint";

	// Inidicate that we are a MipLinear test
	bMipLinear = true;
}

//************************************************************************
// Linear_MinLinear Test functions

CLinear_MinLinearTest::CLinear_MinLinearTest()
{
	m_szTestName = "MipFilter Linear_MinLinear";
	m_szCommandKey = "Linear_MinLinear";

	// Inidicate that we are a MipLinear & MinLinear test
	bMipLinear = true;
	bMinLinear = true;
}
