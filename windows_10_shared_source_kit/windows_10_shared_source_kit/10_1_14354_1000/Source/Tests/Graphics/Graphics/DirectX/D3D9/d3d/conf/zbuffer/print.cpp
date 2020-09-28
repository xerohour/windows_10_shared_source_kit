#define D3D_OVERLOADS
#include <d3d.h>
#include "ZBuffer.h"

//************************************************************************
// Print Test functions

CZPrintTest::CZPrintTest()
{
	m_szTestName = "Print Z Buffer Formats";
	m_szCommandKey = "Print";

	m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
}

UINT CZPrintTest::TestInitialize(void)
{
	// Reset the Test range 
	SetTestRange(1, m_pSrcDevice->GetNumZFormats());

    int i = 1;
    TCHAR szBuffer[80];
    FORMAT *pFormat = m_pSrcDevice->GetZFormats();

    // Print out all source Z buffer formats
	while (NULL != pFormat)
	{
        FmtToString(FORMAT_ZBUFFER,pFormat,szBuffer);
        WriteToLog("Z Buffer Format %02d: %s\n",i++,szBuffer);

        pFormat = pFormat->pNext;
	}

	return D3DTESTINIT_SKIPALL;
}

bool CZPrintTest::ExecuteTest(UINT uTestNum)
{
    return true;
}
