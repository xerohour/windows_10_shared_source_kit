#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

//************************************************************************
// Print Test functions

CPrintTest::CPrintTest()
{
	m_szTestName = "Print Texture Formats";
	m_szCommandKey = "Print";
}

UINT CPrintTest::TestInitialize(void)
{
	// Setup the Test range 
	SetTestRange(1, m_pSrcDevice->GetNumTexFormats());

    int i = 1;
    FORMAT *pFormat = m_pSrcDevice->GetTexFormats();

	// Print out all source texture formats
	while (NULL != pFormat)
	{
        TCHAR szBuffer[80];
		szBuffer[0] = '\0';

        FmtToString(FORMAT_TEXTURE,pFormat,szBuffer);
		WriteToLog("Texture Format %02d: %s, BitCount: %d\n",i++,szBuffer, pFormat->ddpfPixelFormat.dwRGBBitCount);

        pFormat = pFormat->pNext;
	}

	return D3DTESTINIT_SKIPALL;
}
