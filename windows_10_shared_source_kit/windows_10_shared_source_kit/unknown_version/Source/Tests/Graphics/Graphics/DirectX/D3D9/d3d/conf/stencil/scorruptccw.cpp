// FILE:        scorruptccw.cpp
// DESC:        stencil conformance tests (counter clockwise flood)
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil corruption due to z clear/write operations

#include "stencil.h"

const UINT CLEAR2_ENABLED  = 0;
const UINT ZWRITE_ENABLED  = 1;
const UINT CLEAR2_DISABLED = 2;
const UINT ZWRITE_DISABLED = 3;

const UINT NCASES          = 4;

// NAME:        CStencilSCorruptCCW()
// DESC:        stencil s corrupt ccw class constructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorruptCCW::CStencilSCorruptCCW()
{
    m_szCommandKey = "SCorruptCCW";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilSCorruptCCW()
// DESC:        stencil s corrupt ccw class destructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorruptCCW::~CStencilSCorruptCCW()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilSCorruptCCW::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        CapsCheck()
// DESC:        caps check
// INPUT:       none
// OUTPUT:      true.....if caps check is successful
//              false....otherwise

bool CStencilSCorruptCCW::CapsCheck(VOID)
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

UINT CStencilSCorruptCCW::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
    SetTestRange((UINT) 1, (UINT) (NCASES*NMASKS*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilSCorruptCCW::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    CHAR szLabel[NCASES][19] =
    {
        "Clear2(),  enabled",
        "Z write,   enabled",
        "Clear2(), disabled",
        "Z write,  disabled"
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
    UINT i, j;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (j = 1, i = 0; i < NMASKS; i++)
        for (dwClear = (DWORD) 0; dwClear <= m_dwStencilBitMask; dwClear++)
            for (dwCase = (DWORD) 0; dwCase < NCASES; dwCase++, j++)
                if (j == uTest)
                {
                    wsprintf(m_szBuffer, "%s:  0x%08X, 0x%08X, Non-Stippled", szLabel[dwCase], dwClear, dwMasks[i]);
                    BeginTestCase(m_szBuffer, uTest);

                    SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);

                    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                    SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
                    SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[i]);

                    SetRenderState(D3DRS_CCW_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_CCW_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRS_CCW_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                    dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

                    if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                    {
                        if (EndScene())
                        {
                            dwFlags = D3DCLEAR_TARGET;

                            if ((CLEAR2_ENABLED == dwCase) || (CLEAR2_DISABLED == dwCase))
                                dwFlags |= D3DCLEAR_ZBUFFER;
                        
                            SetRenderState(D3DRS_STENCILENABLE, dwEnable[dwCase]);
                            SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, dwEnable[dwCase]);

                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                if ((ZWRITE_ENABLED == dwCase) || (ZWRITE_DISABLED == dwCase))
                                    Flood((D3DVALUE) 0.0, m_dwFailColor, FLOOD_CCW);

                                SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                                SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, (DWORD) TRUE);
                                SetRenderState(D3DRS_STENCILREF, dwClear);
                                SetRenderState(D3DRS_STENCILMASK, dwMasks[i]);

                                SetRenderState(D3DRS_CCW_STENCILFUNC, (DWORD) D3DCMP_EQUAL);

                                Flood((D3DVALUE) 0.0, m_dwPassColor, FLOOD_CCW);

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
