// FILE:        funcccw.cpp
// DESC:        stencil conformance tests (D3DCMP - counter clockwise flood)
// AUTHOR:      Todd M. Frost
// COMMENTS:

#include "stencil.h"

const UINT NARGS = 5;

DWORD gpdwFailArgsCCW[][NARGS] =
{
    { D3DCMP_NEVER,        D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESS,         D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESS,         D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_GREATER,      D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_GREATER,      D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_ZERO, 1, 0, 0 },
};

DWORD gpdwPassArgsCCW[][NARGS] =
{
    { D3DCMP_LESS,         D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_INCR, 0, 0, 1 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATER,      D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_ALWAYS,       D3DSTENCILOP_ZERO, 1, 1, 0 },
};

// NAME:        CStencilFuncCCW()
// DESC:        stencil func ccw class constructor
// INPUT:       none
// OUTPUT:      none

CStencilFuncCCW::CStencilFuncCCW()
{
    m_szCommandKey = "FuncCCW";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilFuncCCW()
// DESC:        stencil func ccw class destructor
// INPUT:       none
// OUTPUT:      none

CStencilFuncCCW::~CStencilFuncCCW()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilFuncCCW::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencilFuncCCW::CapsCheck(VOID)
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

UINT CStencilFuncCCW::TestInitialize(VOID)
{
    UINT uBase = NARGS*sizeof(DWORD);
    UINT uResult = CStencil::TestInitialize();

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
    SetTestRange((UINT) 1, (UINT) (NMASKS*(sizeof(gpdwFailArgsCCW)/uBase + 2*sizeof(gpdwPassArgsCCW)/uBase)));

    if (D3DTESTINIT_RUN == uResult)
    {
        if (!(m_dwZCmpCaps & D3DPCMPCAPS_ALWAYS))
        {
            WriteToLog("Device does NOT support D3DCMP_ALWAYS, blocked.\n");
            uResult = D3DTESTINIT_SKIPALL;
        }
        else if (!(m_dwZCmpCaps & D3DPCMPCAPS_NEVER))
        {
            WriteToLog("Device does NOT support D3DCMP_NEVER, blocked.\n");
            uResult = D3DTESTINIT_SKIPALL;
        }
    }

    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise
// COMMENTS:    gpdwFailArgsCCW, gpdwPassArgsCCW:
//                  0:  stencil compare function
//                  1:  stencil operation
//                  2:  stencil clear value (0 => 0, 1 => mask)
//                  3:  reference mask (pre)
//                  4:  reference mask (post)

bool CStencilFuncCCW::ExecuteTest(UINT uTest)
{
    ARGS Args;
    DWORD dwMasks[2];
    UINT i, j, n, uTests;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (j = 1, i = 0; i < NMASKS; i++)
    {
        uTests = sizeof(gpdwFailArgsCCW)/(NARGS*sizeof(DWORD));

        for (n = 0; n < uTests; n++, j++)
            if (j == uTest)
            {
                wsprintf(m_szBuffer, "%s, CCW_STENCILFAIL, Non-Stippled", GetFunc(gpdwFailArgsCCW[n][0]));
                BeginTestCase(m_szBuffer, uTest);

                SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

                SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
                SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                SetRenderState(D3DRS_STENCILMASK, dwMasks[i]);
                SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[i]);

                SetRenderState(D3DRS_CCW_STENCILFUNC, gpdwFailArgsCCW[n][0]);
                SetRenderState(D3DRS_CCW_STENCILFAIL, gpdwFailArgsCCW[n][1]);
                SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                Args.dwClearColor = m_dwFailColor;
                Args.dwStencil = gpdwFailArgsCCW[n][2];
                Args.dwReference[0] = gpdwFailArgsCCW[n][3];
                Args.dwReference[1] = gpdwFailArgsCCW[n][4];

                Test(&Args);
                return true;
            }

        uTests = sizeof(gpdwPassArgsCCW)/(NARGS*sizeof(DWORD));

        for (n = 0; n < uTests; n++, j++)
            if (j == uTest)
            {
                    wsprintf(m_szBuffer, "%s, CCW_STENCILZFAIL, Non-Stippled", GetFunc(gpdwPassArgsCCW[n][0]));
                    BeginTestCase(m_szBuffer, uTest);

                    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_NEVER);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRS_STENCILMASK, dwMasks[i]);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[i]);

                    SetRenderState(D3DRS_CCW_STENCILFUNC, gpdwPassArgsCCW[n][0]);
                    SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_CCW_STENCILZFAIL, gpdwPassArgsCCW[n][1]);
                    SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = gpdwPassArgsCCW[n][2];
                    Args.dwReference[0] = gpdwPassArgsCCW[n][3];
                    Args.dwReference[1] = gpdwPassArgsCCW[n][4];

                    Test(&Args);
                    return true;
                }

        uTests = sizeof(gpdwPassArgsCCW)/(NARGS*sizeof(DWORD));

        for (n = 0; n < uTests; n++, j++)
            if (j == uTest)
            {
                    wsprintf(m_szBuffer, "%s, CCW_STENCILPASS, Non-Stippled", GetFunc(gpdwPassArgsCCW[n][0]));
                    BeginTestCase(m_szBuffer, uTest);

                    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRS_STENCILMASK, dwMasks[i]);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[i]);

                    SetRenderState(D3DRS_CCW_STENCILFUNC, gpdwPassArgsCCW[n][0]);
                    SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_CCW_STENCILPASS, gpdwPassArgsCCW[n][1]);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = gpdwPassArgsCCW[n][2];
                    Args.dwReference[0] = gpdwPassArgsCCW[n][3];
                    Args.dwReference[1] = gpdwPassArgsCCW[n][4];

                    Test(&Args);
                    return true;
                }
        }

    return false;
}

// NAME:        Test()
// DESC:        perform stencil function test (D3DRS_CCW_STENCILFUNC)
// INPUT:       pArgs........address of ARGS data
// OUTPUT:      none

VOID CStencilFuncCCW::Test(PARGS pArgs)
{
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    if (BeginScene(dvDepth, pArgs->dwStencil, pArgs->dwClearColor, dwFlags))
    {
        dvDepth = 0.5f;

        SetRenderState(D3DRS_STENCILREF, pArgs->dwReference[0]);

        Flood(dvDepth, m_dwFailColor, FLOOD_CCW);

        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILREF, pArgs->dwReference[1]);
        SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_EQUAL);

        Flood(dvDepth, m_dwPassColor, FLOOD_CCW);

        if (!EndScene())
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

    if (Compare())
        Pass();
    else
    {
        WriteToLog("Invalid pixel detected.\n");
        Fail();
    }
}

const UINT NFUNCS = 8;

// NAME:        GetFunc()
// DESC:        get function string
// INPUT:       dwFunc...function index
// OUTPUT:      szFuncs[dwFunc]
// COMMENTS:    dwFunc clamped to NFUNCS

LPSTR CStencilFuncCCW::GetFunc(DWORD dwFunc)
{
    static PTSTR szFuncs[] =
    {
        TEXT("D3DCMP_NEVER       "),
        TEXT("D3DCMP_LESS        "),
        TEXT("D3DCMP_EQUAL       "),
        TEXT("D3DCMP_LESSEQUAL   "),
        TEXT("D3DCMP_GREATER     "),
        TEXT("D3DCMP_NOTEQUAL    "),
        TEXT("D3DCMP_GREATEREQUAL"),
        TEXT("D3DCMP_ALWAYS      "),
        TEXT("invalid func       ")
    };

    dwFunc -= D3DCMP_NEVER;
    dwFunc = (DWORD) ((dwFunc < NFUNCS) ? dwFunc : NFUNCS);
    return((LPTSTR) szFuncs[dwFunc]);
}
