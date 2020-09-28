#include "FillMode.h"

CSolidFill::CSolidFill()
{
    m_szTestName = _T("LemniSnake Fill Mode Solid");
    m_szCommandKey = _T("Solid");
}

UINT CSolidFill::TestInitialize()
{
	SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
	return CFillMode::TestInitialize();
}