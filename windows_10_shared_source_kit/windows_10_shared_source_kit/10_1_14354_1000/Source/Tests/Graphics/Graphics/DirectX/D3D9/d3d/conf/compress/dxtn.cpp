#define D3D_OVERLOADS
#include <d3d.h>
#include "Compress.h"

//************************************************************************
// DXT1 Test functions

CDXT1Test::CDXT1Test()
{
	m_szTestName = "DXT1 Compressed Texture";
	m_szCommandKey = "DXT1";

	// Tell parent our FourCC code
	FourCC = FMT_DXT1;
}

CDXT1Test::~CDXT1Test()
{
}
   
//************************************************************************
// DXT2 Test functions

CDXT2Test::CDXT2Test()
{
	m_szTestName = "DXT2 Compressed Texture";
	m_szCommandKey = "DXT2";

	// Tell parent our FourCC code
	FourCC = FMT_DXT2;
}

CDXT2Test::~CDXT2Test()
{
}
   
//************************************************************************
// DXT3 Test functions

CDXT3Test::CDXT3Test()
{
	m_szTestName = "DXT3 Compressed Texture";
	m_szCommandKey = "DXT3";

	// Tell parent our FourCC code
	FourCC = FMT_DXT3;
}

CDXT3Test::~CDXT3Test()
{
}
   
//************************************************************************
// DXT4 Test functions

CDXT4Test::CDXT4Test()
{
	m_szTestName = "DXT4 Compressed Texture";
	m_szCommandKey = "DXT4";

	// Tell parent our FourCC code
	FourCC = FMT_DXT4;
}

CDXT4Test::~CDXT4Test()
{
}
   
//************************************************************************
// DXT5 Test functions

CDXT5Test::CDXT5Test()
{
	m_szTestName = "DXT5 Compressed Texture";
	m_szCommandKey = "DXT5";

	// Tell parent our FourCC code
	FourCC = FMT_DXT5;
}

CDXT5Test::~CDXT5Test()
{
}

