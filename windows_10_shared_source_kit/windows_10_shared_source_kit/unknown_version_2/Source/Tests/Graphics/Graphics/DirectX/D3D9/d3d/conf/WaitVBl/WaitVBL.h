#ifndef WAITVBL_INCLUDED
#define WAITVBL_INCLUDED

#include <windows.h>
#include "CD3DTest.h"

#include <d3d9.h>

#define MAX_PERCENT_DIFFERENCE 0.33f
#define NO_OF_TESTED_INTERVALS 100
#define MAX_FAILING_INTERVALS 33

#define MAX_MODE_DESC_LEN 255

class clsWaitVBl: public CD3DTest
{
private:
	UINT uiTestedAdapterOrdinal;
	D3DDEVTYPE d3ddevtypeTestedType;
	DISPLAYMODE d3ddisplaymodeAdapter;
	char szModeDesc[MAX_MODE_DESC_LEN];
	LARGE_INTEGER liFrequency;
    float m_fMeasuredIntervals[NO_OF_TESTED_INTERVALS];

    HRESULT InterlacedMode(__in DWORD dwAdapter, __in bool &bInterlaced);

public:

	clsWaitVBl();
	~clsWaitVBl();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT uiTestCaseId);
	virtual bool TestTerminate(void);

	virtual bool DisplayFrame();
};

//QPCStateVals InitializeQPCVars();

#endif
