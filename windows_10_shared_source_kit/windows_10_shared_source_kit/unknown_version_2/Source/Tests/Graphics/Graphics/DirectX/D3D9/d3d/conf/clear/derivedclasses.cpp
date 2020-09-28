#define D3D_OVERLOADS
#include <d3d.h>
#include "clear.h"

/******* Clear Texture as render target. *******/

CTextureClearTest::CTextureClearTest()
{
    TCHAR   szString[MAX_PATH * 2];

	m_szTestName = "Clear_Test Texture";
	m_szCommandKey = "Texture";

	m_bTestRangeSpecified = false;
    if(ReadString(m_szCommandKey, szString, ARRAYSIZE(szString)))
    {
		m_bTestRangeSpecified = true;
	}
}

CTextureClearTest::~CTextureClearTest()
{
}

/******* Clear Offscreen as render target. *******/

COffscreenClearTest::COffscreenClearTest()
{
    TCHAR   szString[MAX_PATH * 2];

	m_szTestName = "Clear_Test Offscreen";
	m_szCommandKey = "Offscreen";

	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

	m_bTestRangeSpecified = false;
    if(ReadString(m_szCommandKey, szString, ARRAYSIZE(szString)))
    {
		m_bTestRangeSpecified = true;
	}

    m_bOffscreen = true;
}

COffscreenClearTest::~COffscreenClearTest()
{
}
