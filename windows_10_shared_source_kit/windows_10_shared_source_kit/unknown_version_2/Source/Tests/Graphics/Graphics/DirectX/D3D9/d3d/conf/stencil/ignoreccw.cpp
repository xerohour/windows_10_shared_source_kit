// FILE:        ignoreccw.cpp
// DESC:        stencil conformance tests (ignore - counter clockwise flood)
// AUTHOR:      Todd M. Frost
// COMMENTS:    verifies stencil tests aren't passing because stencil states are being ignored

#include "stencil.h"

// NAME:        CStencilIgnoreCCW()
// DESC:        stencil ignore ccw class constructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnoreCCW::CStencilIgnoreCCW()
{
    m_szCommandKey = "IgnoreCCW";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilIgnoreCCW()
// DESC:        stencil ignore ccw class destructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnoreCCW::~CStencilIgnoreCCW()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilIgnoreCCW::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencilIgnoreCCW::CapsCheck(VOID)
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

UINT CStencilIgnoreCCW::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
    SetTestRange((UINT) 1, (UINT) (NMASKS*2));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilIgnoreCCW::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwClear[2];
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwClear[0] = (DWORD) 0;
    dwClear[1] = m_dwStencilBitMask;
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = (DWORD) 0xffffffff;

    for (k = 1, i = 0; (i < NMASKS) && !bSuccess; i++)
        for (j = 0; (j < 2) && !bSuccess; j++, k++)
            if (k == uTest)
            {
                Test(dwClear[j], dwMasks[i], uTest);
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

VOID CStencilIgnoreCCW::Test(DWORD dwClear, DWORD dwMask, UINT uTest)
{
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    wsprintf(m_szBuffer, "Clear2():  0x%08X, 0x%08X, Non-Stippled", dwClear, dwMask);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
    {
        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
        SetRenderState(D3DRS_STENCILREF, dwClear);
        SetRenderState(D3DRS_STENCILMASK, dwMask); 
        SetRenderState(D3DRS_STENCILWRITEMASK, dwMask);

        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_NOTEQUAL);
        SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CCW);

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
