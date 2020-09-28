// FILE:        scorrupt.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil corruption due to z clear/write operations

#include "stencil.h"

const UINT CLEAR2_ENABLED  = 0;
const UINT ZWRITE_ENABLED  = 1;
const UINT CLEAR2_DISABLED = 2;
const UINT ZWRITE_DISABLED = 3;

const UINT NCASES          = 4;

// NAME:        CStencilSCorrupt()
// DESC:        stencil s corrupt class constructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorrupt::CStencilSCorrupt()
{
    m_szCommandKey = "SCorrupt";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilSCorrupt()
// DESC:        stencil s corrupt class destructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorrupt::~CStencilSCorrupt()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilSCorrupt::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilSCorrupt::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NCASES*NMASKS*m_nStipple*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilSCorrupt::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    CHAR szLabel[NCASES][19] =
    {
        "Clear2(),  enabled",
        "Z write,   enabled",
        "Clear2(), disabled",
        "Z write,  disabled"
    };
    CHAR szStipple[2][13] =
    {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwCase, dwClear, dwFlags;
    DWORD dwEnable[NCASES] =
    {
        (DWORD) TRUE,
        (DWORD) TRUE,
        (DWORD) FALSE,
        (DWORD) FALSE
    };
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; i < m_nStipple; i++)
        for (j = 0; j < NMASKS; j++)
            for (dwClear = (DWORD) 0; dwClear <= m_dwStencilBitMask; dwClear++)
                for (dwCase = (DWORD) 0; dwCase < NCASES; dwCase++, k++)
                    if (k == uTest)
                    {
                        wsprintf(m_szBuffer, "%s:  0x%08X, 0x%08X, %s", szLabel[dwCase], dwClear, dwMasks[j], szStipple[i]);
                        BeginTestCase(m_szBuffer, uTest);
                        Stipple(((i == 0) ? false : true));

                        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                        SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[j]);

                        SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                        SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                        SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                        dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

                        if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                        {
                            if (EndScene())
                            {
                                dwFlags = D3DCLEAR_TARGET;

                                if ((CLEAR2_ENABLED == dwCase) || (CLEAR2_DISABLED == dwCase))
                                    dwFlags |= D3DCLEAR_ZBUFFER;
                        
                                SetRenderState(D3DRS_STENCILENABLE, dwEnable[dwCase]);

                                if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                {
                                    if ((ZWRITE_ENABLED == dwCase) || (ZWRITE_DISABLED == dwCase))
                                        Flood((D3DVALUE) 0.0, m_dwFailColor);

                                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                                    SetRenderState(D3DRS_STENCILREF, dwClear);
                                    SetRenderState(D3DRS_STENCILMASK, dwMasks[j]);

                                    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_EQUAL);

                                    Flood((D3DVALUE) 0.0, m_dwPassColor);

                                    if (EndScene())
                                    {
                                        if (Compare())
                                            Pass();
                                        else
                                        {
                                            WriteToLog("Invalid pixel detected.\n");
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
                            else
                            {
                                WriteToLog("Unable to end scene, blocked.\n");
                                Fail();
                            }
                        }
                        else
                        {
                            WriteToLog("Unable to begin scene, blocked.\n");
                            Fail();
                        }

                        bSuccess = true;
                    }

    return bSuccess;
}
