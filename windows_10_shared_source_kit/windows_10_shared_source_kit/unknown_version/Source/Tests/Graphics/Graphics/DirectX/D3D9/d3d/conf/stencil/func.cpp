// FILE:        func.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil funcs (D3DCMP)

#include "stencil.h"

const UINT NARGS = 5;

DWORD gpdwFailArgs[][NARGS] =
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

DWORD gpdwPassArgs[][NARGS] =
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

// NAME:        CStencilFunc()
// DESC:        stencil func class constructor
// INPUT:       none
// OUTPUT:      none

CStencilFunc::CStencilFunc()
{
    m_szCommandKey = "Func";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilFunc()
// DESC:        stencil func class destructor
// INPUT:       none
// OUTPUT:      none

CStencilFunc::~CStencilFunc()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilFunc::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilFunc::TestInitialize(VOID)
{
    UINT uBase = NARGS*sizeof(DWORD);
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*(sizeof(gpdwFailArgs)/uBase + 2*sizeof(gpdwPassArgs)/uBase)));

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
// COMMENTS:    gpdwFailArgs, gpdwPassArgs:
//                  0:  stencil compare function
//                  1:  stencil operation
//                  2:  stencil clear value (0 => 0, 1 => mask)
//                  3:  reference mask (pre)
//                  4:  reference mask (post)

bool CStencilFunc::ExecuteTest(UINT uTest)
{
    ARGS Args;
    CHAR szStipple[2][13] =
    {
        "Non-Stippled",
        "    Stippled"
    };
    DWORD dwMasks[2];
    UINT i, j, k, n, uTests;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; i < m_nStipple; i++)
        for (j = 0; j < NMASKS; j++)
        {
            uTests = sizeof(gpdwFailArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    wsprintf(m_szBuffer, "%s, STENCILFAIL, %s", GetFunc(gpdwFailArgs[n][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer, uTest);

                    Stipple(((i == 0) ? false : true));

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRS_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[j]);

                    SetRenderState(D3DRS_STENCILFUNC, gpdwFailArgs[n][0]);
                    SetRenderState(D3DRS_STENCILFAIL, gpdwFailArgs[n][1]);
                    SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = gpdwFailArgs[n][2];
                    Args.dwReference[0] = gpdwFailArgs[n][3];
                    Args.dwReference[1] = gpdwFailArgs[n][4];

                    Test(&Args);
                    return true;
                }

            uTests = sizeof(gpdwPassArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    wsprintf(m_szBuffer, "%s, STENCILZFAIL, %s", GetFunc(gpdwPassArgs[n][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer, uTest);

                    Stipple(((i == 0) ? false : true));

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_NEVER);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRS_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[j]);

                    SetRenderState(D3DRS_STENCILFUNC, gpdwPassArgs[n][0]);
                    SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_STENCILZFAIL, gpdwPassArgs[n][1]);
                    SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = gpdwPassArgs[n][2];
                    Args.dwReference[0] = gpdwPassArgs[n][3];
                    Args.dwReference[1] = gpdwPassArgs[n][4];

                    Test(&Args);
                    return true;
                }

            uTests = sizeof(gpdwPassArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    wsprintf(m_szBuffer, "%s, STENCILPASS, %s", GetFunc(gpdwPassArgs[n][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer, uTest);

                    Stipple(((i == 0) ? false : true));

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRS_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[j]);

                    SetRenderState(D3DRS_STENCILFUNC, gpdwPassArgs[n][0]);
                    SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_STENCILPASS, gpdwPassArgs[n][1]);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = gpdwPassArgs[n][2];
                    Args.dwReference[0] = gpdwPassArgs[n][3];
                    Args.dwReference[1] = gpdwPassArgs[n][4];

                    Test(&Args);
                    return true;
                }
        }

    return false;
}

// NAME:        Test()
// DESC:        perform stencil function test (D3DRS_STENCILFUNC)
// INPUT:       pArgs........address of ARGS data
// OUTPUT:      none

VOID CStencilFunc::Test(PARGS pArgs)
{
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    if (BeginScene(dvDepth, pArgs->dwStencil, pArgs->dwClearColor, dwFlags))
    {
        dvDepth = 0.5f;

        SetRenderState(D3DRS_STENCILREF, pArgs->dwReference[0]);

        Flood(dvDepth, m_dwFailColor);

        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
        SetRenderState(D3DRS_STENCILREF, pArgs->dwReference[1]);

        Flood(dvDepth, m_dwPassColor);

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

LPSTR CStencilFunc::GetFunc(DWORD dwFunc)
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
