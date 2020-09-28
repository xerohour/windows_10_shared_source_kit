// FILE:        ignore.cpp
// DESC:        stencil conformance tests (ignore)
// AUTHOR:      Todd M. Frost
// COMMENTS:    verifies stencil tests aren't passing because stencil states are being ignored

#include "stencil.h"

// NAME:        CStencilIgnore()
// DESC:        stencil ignore class constructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnore::CStencilIgnore()
{
    m_szCommandKey = "Ignore";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilIgnore()
// DESC:        stencil ignore class destructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnore::~CStencilIgnore()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilIgnore::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilIgnore::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*2));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilIgnore::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwClear[2];
    DWORD dwMasks[2];
    UINT i, j, k, uTemp;

    CStencil::ExecuteTest(uTest);
    dwClear[0] = (DWORD) 0;
    dwClear[1] = m_dwStencilBitMask;
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = (DWORD) 0xffffffff;

    for (uTemp = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++)
            for (k = 0; (k < 2) && !bSuccess; k++, uTemp++)
                if (uTemp == uTest)
                {
                    Test(((i == 0) ? false : true), dwClear[k], dwMasks[j], uTest);
                    bSuccess = true;
                }

    return bSuccess;
}

// NAME:        Test()
// DESC:        perform stencil clear test variation
// INPUT:       bStipple.stipple flag
//              dwClear..stencil plane clear value
//              dwMask...stencil write mask
//              uTest....test number
// OUTPUT:      none

VOID CStencilIgnore::Test(bool bStipple, DWORD dwClear, DWORD dwMask, UINT uTest)
{
    CHAR szStipple[2][13] =
    {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    UINT i = bStipple ? 1 : 0;

    wsprintf(m_szBuffer, "Clear2():  0x%08X, 0x%08X, %s", dwClear, dwMask, szStipple[i]);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
    {
        Stipple(bStipple);

        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_STENCILREF, dwClear);
        SetRenderState(D3DRS_STENCILMASK, dwMask); 
        SetRenderState(D3DRS_STENCILWRITEMASK, dwMask);

        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NOTEQUAL);
        SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

        Flood((D3DVALUE) 0.5, m_dwFailColor);

        if (EndScene())
        {
            if (Compare())
                Pass();
            else
            {
                WriteToLog("Non pass color detected.\n");
                Fail();
            }

            DisplayFrame();
        }
        else
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
    }
}
