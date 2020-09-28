// FILE:        address.cpp
// DESC:        texture address mode tests
// AUTHOR:      Todd M. Frost

#include "bump.h"

// NAME:        CBumpAddress()
// DESC:        bump address class constructor
// INPUT:       none
// OUTPUT:      none

CBumpAddress::CBumpAddress()
{
    m_szCommandKey = "Address";
}

// NAME:        ~CBumpAddress()
// DESC:        bump address class destructor
// INPUT:       none
// OUTPUT:      none

CBumpAddress::~CBumpAddress()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CBumpAddress::CommandLineHelp(VOID)
{
    CBump::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      CBump::CapsCheck()

bool CBumpAddress::CapsCheck(VOID)
{
    m_dwInclusion = (DWORD) PF_BUMPDUDV;
    return CBump::CapsCheck();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpAddress::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (m_dwCommonBumpFormats*NMODES*NMODES));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false....................otherwise

bool CBumpAddress::ExecuteTest(UINT uTest)
{
    DWORD dwFormat;
    UINT i, j, uTemp;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_dwCommonBumpFormats; dwFormat++)
        for (i = 0; i < NMODES; i++)
            for (j = 0; j < NMODES; j++, uTemp++)
                if (uTemp == uTest)
                {
                    if (((i == j) || m_Flags.Check(CF_INDEPENDENT)) &&
                        (m_dwTexAddressCaps & m_dwModeCapsVals[j][0]) &&
                        (m_dwTexAddressCaps & m_dwModeCapsVals[i][0]) &&
                        SetBumpTexture(dwFormat, TEXWIDTH, TEXHEIGHT))
                    {
                        m_dwModes[0] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[i][1];
                        m_dwModes[1] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[j][1];
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
