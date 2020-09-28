// FILE:        blend.cpp
// DESC:        bump luminance blend tests
// AUTHOR:      Todd M. Frost

#include "bump.h"

DWORD gpdwBlend[NBLENDS][2] = {
    { D3DTEXOPCAPS_MODULATE,   CF_MODULATE   },
    { D3DTEXOPCAPS_MODULATE2X, CF_MODULATE2X },
    { D3DTEXOPCAPS_MODULATE4X, CF_MODULATE4X },
};


// NAME:        CBumpBlend()
// DESC:        bump blend class constructor
// INPUT:       none
// OUTPUT:      none

CBumpBlend::CBumpBlend()
{
    m_szCommandKey = "Blend";
}

// NAME:        ~CBumpBlend()
// DESC:        bump blend class destructor
// INPUT:       none
// OUTPUT:      none

CBumpBlend::~CBumpBlend()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CBumpBlend::CommandLineHelp(VOID)
{
    CBump::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      CBump::CapsCheck()

bool CBumpBlend::CapsCheck(VOID)
{
    m_dwInclusion = (DWORD) PF_BUMPDUDV;
    return CBump::CapsCheck();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpBlend::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (NBLENDS*m_dwCommonBumpFormats));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false.....otherwise
// COMMENTS:    D3DTADDRESS_MIRROR most visually pleasing, substitute for wrap if supported

bool CBumpBlend::ExecuteTest(UINT uTest)
{
    DWORD dwFormat;
    UINT i, uTemp;

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_MIRROR)
        m_dwModes[1] = m_dwModes[0] = D3DTADDRESS_MIRROR;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_dwCommonBumpFormats; dwFormat++)
        for (i = 1; i <= NBLENDS; i++, uTemp++)
            if (uTemp == uTest)
            {
                if ((m_dwTexOpCaps & gpdwBlend[i-1][0]) &&
                    SetBumpTexture(dwFormat, TEXWIDTH, TEXHEIGHT))
                {
                    m_Flags.Clear(CF_BLEND);
                    m_Flags.Set(gpdwBlend[i-1][1]);
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
