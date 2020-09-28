// FILE:        mix.cpp
// DESC:        texture mix mode tests
// AUTHOR:      Todd M. Frost

#include "bump.h"

// NAME:        CBumpMix()
// DESC:        bump mix class constructor
// INPUT:       none
// OUTPUT:      none

CBumpMix::CBumpMix()
{
    m_szCommandKey = "Mix";
}

// NAME:        ~CBumpMix()
// DESC:        bump mix class destructor
// INPUT:       none
// OUTPUT:      none

CBumpMix::~CBumpMix()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CBumpMix::CommandLineHelp(VOID)
{
    CBump::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      CBump::CapsCheck()

bool CBumpMix::CapsCheck(VOID)
{
    bool bResult;

    m_dwInclusion = (DWORD) PF_BUMPDUDV;

    // CBump::CapsCheck() clears CF_SYSTEM_CREATED

    bResult = CBump::CapsCheck();
    m_Flags.Set(CF_SYSTEM_CREATED);
    return bResult;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpMix::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) m_dwCommonBumpFormats);
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false....................otherwise

bool CBumpMix::ExecuteTest(UINT uTest)
{
    DWORD dwFormat;
    UINT uTemp;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_dwCommonBumpFormats; uTemp++, dwFormat++)
        if (uTemp == uTest)
        {
            if (SetBumpTexture(dwFormat, TEXWIDTH, TEXHEIGHT))
            {
                m_dwModes[0] = D3DTADDRESS_WRAP;
                m_dwModes[1] = D3DTADDRESS_WRAP;
                return CBump::ExecuteTest(uTest);
            }
            else
            {
                SkipTests((UINT) 1);
                return false;
            }
        }

    return false;
}

// NAME:        ProcessFrame()
// DESC:        process result
// INPUT:       none
// OUTPUT:      pass or fail

bool CBumpMix::ProcessFrame(void)
{
    // Set compare function to require 0%, doing our own validation
	m_pFramework->GetImageCmp()->SetPassRequired(0.0f);

	// Calculate image compare
	CD3DTest::ProcessFrame();

    // Reset compare function to require 85%
	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	// If device can render system memory textures, use compare results
	if (m_Flags.Check(CF_SYSTEM_ALLOWED))
		return true;

	// Allow the default black texture render
	if (m_fPassPercentage == 0.0)
	{
		m_fPassPercentage = 1.0;
		Pass();
	}
	// Should not have gotten to this point, device needs to render black or fail ValidateDevice for system memory textures
	else
	{
        WriteToLog("System memory texture used, D3DDEVCAPS_TEXTURESYSTEMMEMORY not set.\n");
        WriteToLog("ValidateDevice() must fail or default (all black) render is allowed.\n");
		Fail();
	}

	return true;
}
