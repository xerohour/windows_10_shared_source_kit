#include "FillMode.h"

CLineFill::CLineFill()
{
    m_szTestName = _T("LemniSnake Fill Mode WireFrame");
    m_szCommandKey = _T("Wire");
}

UINT CLineFill::TestInitialize()
{
	SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
	return CFillMode::TestInitialize();
}