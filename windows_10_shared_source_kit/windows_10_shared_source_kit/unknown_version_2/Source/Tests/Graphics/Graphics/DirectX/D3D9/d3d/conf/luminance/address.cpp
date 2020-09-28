// FILE:        address.cpp
// DESC:        texture address mode tests
// AUTHOR:      Todd M. Frost

#include "luminance.h"

// NAME:        CLuminanceAddress()
// DESC:        luminance address class constructor
// INPUT:       none
// OUTPUT:      none

CLuminanceAddress::CLuminanceAddress()
{
    m_szCommandKey = "Address";
}

// NAME:        ~CLuminanceAddress()
// DESC:        luminance address class destructor
// INPUT:       none
// OUTPUT:      none

CLuminanceAddress::~CLuminanceAddress()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CLuminanceAddress::CommandLineHelp(VOID)
{
    CLuminance::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      D3DTESTINIT_ code

UINT CLuminanceAddress::TestInitialize(VOID)
{
    UINT uResult = CLuminance::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (m_dwCommonLuminanceFormats*NSTAGES*NMODES*NMODES));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CLuminance::ExecuteTests()...if test executed
//              false........................otherwise

bool CLuminanceAddress::ExecuteTest(UINT uTest)
{
    DWORD dwFormat, dwStages;
    UINT i, j, uTemp;

    for (uTemp = 1, dwStages = 1; dwStages <= NSTAGES; dwStages++)
        for (dwFormat = 0; dwFormat < m_dwCommonLuminanceFormats; dwFormat++)
            for (i = 0; i < NMODES; i++)
                for (j = 0; j < NMODES; j++, uTemp++)
                    if (uTemp == uTest)
                    {
                        if (((i == j) || m_Flags.Check(CF_INDEPENDENT)) &&
                            (m_dwTexAddressCaps & m_dwModeCapsVals[i][0]) &&
                            (m_dwTexAddressCaps & m_dwModeCapsVals[j][0]) &&
                            (dwStages <= m_dwStagesMax) &&
                            SetLuminanceTexture(dwFormat, TEXWIDTH, TEXHEIGHT))
                        {
                            m_dwStages = dwStages;
                            m_dwModes[0] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[i][1];
                            m_dwModes[1] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[j][1];
                            return CLuminance::ExecuteTest(uTest);
                        }
                        else
                        {
                            SkipTests((UINT) 1);
                            return false;
                        }
                    }

    return false;
}
