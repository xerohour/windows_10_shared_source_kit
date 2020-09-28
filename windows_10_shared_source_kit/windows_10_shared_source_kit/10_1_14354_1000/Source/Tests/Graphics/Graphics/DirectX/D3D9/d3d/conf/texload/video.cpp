#define D3D_OVERLOADS
#include <d3d.h>
#include "TexLoad.h"

//************************************************************************
// Video Test functions

CVideoTest::CVideoTest()
{
	m_szTestName = "TexLoad Video";
	m_szCommandKey = "Video";

	// Inidicate that we are a Video test
	bVideo = true;
}

CVideoTest::~CVideoTest()
{
}

//************************************************************************
// MipMap Video Test functions

CMipMapVideoTest::CMipMapVideoTest()
{
	m_szTestName = "TexLoad MipMap_Video";
	m_szCommandKey = "MipMap_Video";

	// Inidicate that we are a MipMap & Video test
	bVideo = true;
	bMipMap = true;
}

CMipMapVideoTest::~CMipMapVideoTest()
{
}

