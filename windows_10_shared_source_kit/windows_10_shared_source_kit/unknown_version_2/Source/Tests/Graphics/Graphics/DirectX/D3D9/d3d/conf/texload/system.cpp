#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

//************************************************************************
// System Test functions

CSystemTest::CSystemTest()
{
	m_szTestName = "TexLoad System";
	m_szCommandKey = "System";

	// Inidicate that we are a System test
	bSystem = true;
}

CSystemTest::~CSystemTest()
{
}

//************************************************************************
// MipMap System Test functions

CMipMapSystemTest::CMipMapSystemTest()
{
	m_szTestName = "TexLoad MipMap_System";
	m_szCommandKey = "MipMap_System";

	// Inidicate that we are a MipMap & System test
	bSystem = true;
	bMipMap = true;
}

CMipMapSystemTest::~CMipMapSystemTest()
{
}

