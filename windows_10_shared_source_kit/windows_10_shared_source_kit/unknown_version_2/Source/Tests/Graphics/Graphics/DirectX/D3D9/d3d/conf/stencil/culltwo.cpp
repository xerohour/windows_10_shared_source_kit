// FILE:        culltwo.cpp
// DESC:        stencil conformance tests for cull modes (two sided stencil)
// AUTHOR:      Todd M. Frost
// COMMENTS:

#include "stencil.h"

// NAME:        CStencilCullTwo()
// DESC:        stencil cull two class constructor
// INPUT:       none
// OUTPUT:      none

CStencilCullTwo::CStencilCullTwo()
{
    m_szCommandKey = "CullTwo";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilCullTwo()
// DESC:        stencil cull two class destructor
// INPUT:       none
// OUTPUT:      none

CStencilCullTwo::~CStencilCullTwo()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilCullTwo::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencilCullTwo::CapsCheck(VOID)
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

UINT CStencilCullTwo::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    SetTestRange((UINT) 1, (UINT) NMASKS);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilCullTwo::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwMasks[2];
    UINT i, j;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = (DWORD) 0xffffffff;

    for (j = 1, i = 0; (i < NMASKS) && !bSuccess; i++, j++)
        if (j == uTest)
        {
            Test((DWORD) 0, dwMasks[i], uTest);
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

VOID CStencilCullTwo::Test(DWORD dwClear, DWORD dwMask, UINT uTest)
{
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    wsprintf(m_szBuffer, "D3DRS_CULLMODE (two sided):  0x%08X, 0x%08X, Non-Stippled", dwClear, dwMask);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
    {
        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
        SetRenderState(D3DRS_STENCILREF, dwClear);
        SetRenderState(D3DRS_STENCILMASK, dwMask);
        SetRenderState(D3DRS_STENCILWRITEMASK, dwMask);
        SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CCW);    // cull counter clockwise triangles (default)
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CW);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CCW);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CW);     // cull clockwise oriented triangles
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_NEVER);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CW);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CCW);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);   // do not cull clockwise or counter clockwise triangles
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_NEVER);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CW);
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
