#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

//************************************************************************
// UpdateTex Test functions

CUpdateTexTest::CUpdateTexTest()
{
	m_szTestName = "Compare Texture Formats (UpdateTex)";
	m_szCommandKey = "UpdateTex";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;
}

//************************************************************************
// CopyRects Test functions

CCopyRectsTest::CCopyRectsTest()
{
	m_szTestName = "Compare Texture Formats (CopyRects)";
	m_szCommandKey = "CopyRects";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;
}

//************************************************************************
// LockCopy Test functions

CLockCopyTest::CLockCopyTest()
{
	m_szTestName = "Compare Texture Formats (LockCopy)";
	m_szCommandKey = "LockCopy";

    dwLoadOption = TEXLOAD_LOCKCOPY;
}

//************************************************************************
// UpdateSurf Test functions

CUpdateSurfTest::CUpdateSurfTest()
{
	m_szTestName = "Compare Texture Formats (UpdateSurf)";
	m_szCommandKey = "UpdateSurf";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;
}

