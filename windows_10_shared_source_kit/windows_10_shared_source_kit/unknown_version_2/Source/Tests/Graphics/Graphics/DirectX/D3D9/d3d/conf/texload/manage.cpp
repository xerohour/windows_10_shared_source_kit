#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

//************************************************************************
// Manage Test functions

CManageTest::CManageTest()
{
	m_szTestName = "TexLoad Manage";
	m_szCommandKey = "Manage";

	// Inidicate that we are a Manage test
	bManage = true;
}

CManageTest::~CManageTest()
{
}

//************************************************************************
// MipMap Manage Test functions

CMipMapManageTest::CMipMapManageTest()
{
	m_szTestName = "TexLoad MipMap_Manage";
	m_szCommandKey = "MipMap_Manage";

	// Inidicate that we are a MipMap & Manage test
	bManage = true;
	bMipMap = true;
}

CMipMapManageTest::~CMipMapManageTest()
{
}
