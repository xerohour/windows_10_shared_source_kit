// FILE:        clearccw.cpp
// DESC:        stencil conformance tests (clear - counter clockwise flood)
// AUTHOR:      Todd M. Frost

#include "stencil.h"

// NAME:        CStencilClearCCW()
// DESC:        stencil clear ccw class constructor
// INPUT:       none
// OUTPUT:      none

CStencilClearCCW::CStencilClearCCW()
{
    m_szCommandKey = "ClearCCW";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilClearCCW()
// DESC:        stencil clear ccw class destructor
// INPUT:       none
// OUTPUT:      none

CStencilClearCCW::~CStencilClearCCW()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilClearCCW::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencilClearCCW::CapsCheck(VOID)
{
    if (m_pSrcDevice->GetCaps()->dwStencilCaps & D3DSTENCILCAPS_TWOSIDED)
        return CStencil::CapsCheck();
    else
        return false;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilClearCCW::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
    SetTestRange((UINT) 1, (UINT) (NMASKS*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilClearCCW::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwClear;
    DWORD dwMasks[2];
    UINT i, j;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (j = 1, i = 0; (i < NMASKS) && !bSuccess; i++)
        for (dwClear = 0; (dwClear <= m_dwStencilBitMask) && !bSuccess; dwClear++, j++)
            if (j == uTest)
            {
                Test(dwClear, dwMasks[i], uTest);
                bSuccess = true;
            }

    return bSuccess;
}

// NAME:        Test()
// DESC:        perform stencil clear test variation
// INPUT:       dwClear..stencil plane clear value
//              dwMask...stencil write mask
//              uTest....test number
// OUTPUT:      none

VOID CStencilClearCCW::Test(DWORD dwClear, DWORD dwMask, UINT uTest)
{
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    wsprintf(m_szBuffer, "Clear2():  0x%08X, 0x%08X, Non-Stippled", dwClear, dwMask);
    BeginTestCase(m_szBuffer, uTest);

    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
    SetRenderState(D3DRS_STENCILREF, dwClear);
    SetRenderState(D3DRS_STENCILMASK, dwMask);
    SetRenderState(D3DRS_STENCILWRITEMASK, dwMask);
    SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NOTEQUAL);

    SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
    SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

    if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
    {
        Flood((D3DVALUE) 0.5, m_dwPassColor, FLOOD_CCW);

        if (EndScene())
        {
            if (Compare())
                Pass();
            else
            {
                WriteToLog("Non pass color detected.\n");
                Fail();
            }
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
