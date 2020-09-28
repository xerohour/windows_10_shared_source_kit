// FILE:        scale.cpp
// DESC:        bump luminance scale tests
// AUTHOR:      Todd M. Frost

#include "bump.h"

// NAME:        CBumpScale()
// DESC:        bump scale class constructor
// INPUT:       none
// OUTPUT:      none

CBumpScale::CBumpScale()
{
    m_szCommandKey = "Scale";
}

// NAME:        ~CBumpScale()
// DESC:        bump scale class destructor
// INPUT:       none
// OUTPUT:      none

CBumpScale::~CBumpScale()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CBumpScale::CommandLineHelp(VOID)
{
    CBump::CommandLineHelp();
    WriteCommandLineHelp("$yvariations: $wvariations per test $c(default=8)");
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      CBump::CapsCheck()

bool CBumpScale::CapsCheck(VOID)
{
    m_dwInclusion = (DWORD) PF_BUMPLUMINANCE;
    return CBump::CapsCheck();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpScale::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (m_dwCommonBumpFormats*m_uVariations));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false.....otherwise
// COMMENTS:    D3DTADDRESS_MIRROR most visually pleasing, substitute for wrap if supported

bool CBumpScale::ExecuteTest(UINT uTest)
{
    DWORD dwFormat;
    UINT i, uTemp;

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_MIRROR)
        m_dwModes[1] = m_dwModes[0] = D3DTADDRESS_MIRROR;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_dwCommonBumpFormats; dwFormat++)
        for (i = 1; i <= m_uVariations; i++, uTemp++)
            if (uTemp == uTest)
            {
                if (SetBumpTexture(dwFormat, TEXWIDTH, TEXHEIGHT))
                {
                    m_fScale.f = Normalize(i, 1, m_uVariations);
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
