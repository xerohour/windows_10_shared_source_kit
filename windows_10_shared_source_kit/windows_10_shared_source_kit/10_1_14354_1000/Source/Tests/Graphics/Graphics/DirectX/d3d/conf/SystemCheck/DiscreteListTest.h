#pragma once

// Includes
#include "SystemCheck.h"

////////////////////////

class CDiscreteListTest : public CD3D11Test
{
public:
	CDiscreteListTest(void);
	virtual ~CDiscreteListTest(void);

	void InitTestParameters();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

    static HRESULT APIENTRY EscapeCB(HANDLE hAdapter, CONST D3DDDICB_ESCAPE *pEscapeCB);

private:
	HMODULE m_hDList;
	HMODULE m_hGDI32;
    PFND3DKMT_CLOSEADAPTER m_pfnCloseAdapter;
    D3DKMT_HANDLE m_hDiscreteAdapter;
	bool m_bFoundHybridDiscreteAdapter;
	UINT m_hybridDiscreteAdapterIndex;
};