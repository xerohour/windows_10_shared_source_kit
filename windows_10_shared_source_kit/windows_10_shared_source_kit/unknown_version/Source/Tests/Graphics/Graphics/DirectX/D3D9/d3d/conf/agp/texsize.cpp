#define D3D_OVERLOADS
#include <d3d.h>
#include "AGP.h"

//************************************************************************
// Texture 8x8 Test functions

CTexture8x8Test::CTexture8x8Test()
{
	m_szTestName = "AGP Texture 8x8";
	m_szCommandKey = "Texture8x8";

	// Tell the parent our texture size
	nTextureSize = 8;
}

CTexture8x8Test::~CTexture8x8Test()
{
}

//************************************************************************
// Texture 16x16 Test functions

CTexture16x16Test::CTexture16x16Test()
{
	m_szTestName = "AGP Texture 16x16";
	m_szCommandKey = "Texture16x16";

	// Tell the parent our texture size
	nTextureSize = 16;
}

CTexture16x16Test::~CTexture16x16Test()
{
}

//************************************************************************
// Texture 64x64 Test functions

CTexture64x64Test::CTexture64x64Test()
{
	m_szTestName = "AGP Texture 64x64";
	m_szCommandKey = "Texture64x64";

	// Tell the parent our texture size
	nTextureSize = 64;
}

CTexture64x64Test::~CTexture64x64Test()
{
}

//************************************************************************
// Texture 256x256 Test functions

CTexture256x256Test::CTexture256x256Test()
{
	m_szTestName = "AGP Texture 256x256";
	m_szCommandKey = "Texture256x256";

	// Tell the parent our texture size
	nTextureSize = 256;
}

CTexture256x256Test::~CTexture256x256Test()
{
}

//************************************************************************
// Texture 1024x1024 Test functions

CTexture1024x1024Test::CTexture1024x1024Test()
{
	m_szTestName = "AGP Texture 1024x1024";
	m_szCommandKey = "Texture1024x1024";

	// Tell the parent our texture size
	nTextureSize = 1024;
}

CTexture1024x1024Test::~CTexture1024x1024Test()
{
}
