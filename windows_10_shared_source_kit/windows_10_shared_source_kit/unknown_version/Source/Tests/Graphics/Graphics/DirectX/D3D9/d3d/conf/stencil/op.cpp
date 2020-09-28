// FILE:        op.cpp
// DESC:        stencil plane op conformance tests (D3DSTENCILOP)
// AUTHOR:      Todd M. Frost
// COMMENTS:

#include "stencil.h"

const UINT NSTENCILSTATES = (D3DRS_STENCILPASS - D3DRS_STENCILFAIL + 1);
const UINT NSTENCILOPS    = (D3DSTENCILOP_DECR - D3DSTENCILOP_KEEP + 1);

// NAME:        CStencilOp()
// DESC:        stencil op class constructor
// INPUT:       none
// OUTPUT:      none

CStencilOp::CStencilOp()
{
    m_szCommandKey = "Op";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilOp()
// DESC:        stencil op class destructor
// INPUT:       none
// OUTPUT:      none

CStencilOp::~CStencilOp()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilOp::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilOp::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*NSTENCILSTATES*NSTENCILOPS));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilOp::ExecuteTest(UINT uTest)
{
    bool bFail = false, bSuccess = false, bWrap;
    CHAR szStipple[2][13] =
    {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwOp, dwState;
    DWORD dwMasks[2];
    UINT i, j, k, n;
    DWORD dwClear, dwIterations;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    DWORD dwReference[2], dwStates[5];

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++)
            for (dwState = D3DRS_STENCILFAIL; dwState <= D3DRS_STENCILPASS; dwState++)
                for (dwOp = D3DSTENCILOP_KEEP; dwOp <= D3DSTENCILOP_DECR; dwOp++, k++)
                    if (k == uTest)
                    {
                        Stipple(((i == 0) ? false : true));

                        switch (dwState)
                        {
                            case D3DRS_STENCILFAIL:
                                dwStates[0] = D3DCMP_ALWAYS;
                                dwStates[1] = D3DCMP_NEVER;
                                dwStates[2] = dwOp;
                                dwStates[3] = D3DSTENCILOP_KEEP;
                                dwStates[4] = D3DSTENCILOP_KEEP;
                            break;

                            case D3DRS_STENCILZFAIL:
                                dwStates[0] = D3DCMP_NEVER;
                                dwStates[1] = D3DCMP_ALWAYS;
                                dwStates[2] = D3DSTENCILOP_KEEP;
                                dwStates[3] = dwOp;
                                dwStates[4] = D3DSTENCILOP_KEEP;
                            break;

                            case D3DRS_STENCILPASS:
                                dwStates[0] = D3DCMP_ALWAYS;
                                dwStates[1] = D3DCMP_ALWAYS;
                                dwStates[2] = D3DSTENCILOP_KEEP;
                                dwStates[3] = D3DSTENCILOP_KEEP;
                                dwStates[4] = dwOp;
                            break;

                            default:
                                WriteToLog("Unknown render state, blocked.\n");
                                Fail();
                                return false;
                            break;
                        }

                        switch (dwOp)
                        {
                            case D3DSTENCILOP_KEEP:
                                dwClear = m_dwStencilBitMask & 0x55555555;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = dwClear;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_ZERO:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_REPLACE:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INCRSAT:
                                dwClear = (DWORD) 0;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = m_dwStencilBitMask;
                                dwReference[1] = m_dwStencilBitMask;
                            break;

                            case D3DSTENCILOP_DECRSAT:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INVERT:
                                dwClear = m_dwStencilBitMask & 0x55555555;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = m_dwStencilBitMask & 0xaaaaaaaa;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INCR:
                                dwClear = (DWORD) 0;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = m_dwStencilBitMask;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_DECR:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = m_dwStencilBitMask;
                            break;

                            default:
                                WriteToLog("Unknown stencil op, blocked.\n");
                                Fail();
                                return false;
                            break;         
                        }

                        wsprintf(m_szBuffer, "%s, %s, %s", GetOp(dwOp), GetState(dwState), szStipple[i]);
                        BeginTestCase(m_szBuffer, uTest);

                        SetRenderState(D3DRS_ZFUNC, dwStates[0]);

                        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
                        SetRenderState(D3DRS_STENCILREF, dwReference[0]);
                        SetRenderState(D3DRS_STENCILMASK, dwMasks[j]);
                        SetRenderState(D3DRS_STENCILWRITEMASK, dwMasks[j]);

                        SetRenderState(D3DRS_STENCILFUNC, dwStates[1]);
                        SetRenderState(D3DRS_STENCILFAIL, dwStates[2]);
                        SetRenderState(D3DRS_STENCILZFAIL, dwStates[3]);
                        SetRenderState(D3DRS_STENCILPASS, dwStates[4]);

                        for (n = 0; n < dwIterations; n++)
                        {
                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                Flood((D3DVALUE) 0.5, m_dwFailColor);

                                if (!EndScene())
                                {
                                    WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
                                    Fail();
                                    bFail = true;
                                    break;
                                }
                            }
                            else
                            {
                                WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
                                Fail();
                                bFail = true;
                                break;
                            }

                            dwFlags = (DWORD) 0;
                        }

                        if (!bFail)
                        {
                            SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                            SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
                            SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                            SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                            SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                Flood((D3DVALUE) 0.5, m_dwPassColor);
                    
                                if (EndScene())
                                {
                                    m_fPassPercentage = 1.0f;

                                    if (!SupportedOperation(dwOp) || Compare())
                                        Pass();
                                    else
                                    {
                                        WriteToLog("Invalid pixel detected.\n");
                                        Fail();
                                        bFail = true;
                                    }
                                }
                                else
                                {
                                    WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
                                    Fail();
                                    bFail = true;
                                }
                            }
                            else
                            {
                                WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
                                Fail();
                                bFail = true;
                            }
                        }

                        if (bWrap && !bFail)
                        {
                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                Flood((D3DVALUE) 0.5, m_dwFailColor);

                                if (EndScene())
                                {
                                    SetRenderState(D3DRS_ZFUNC, dwStates[0]);

                                    SetRenderState(D3DRS_STENCILFUNC, dwStates[1]);
                                    SetRenderState(D3DRS_STENCILFAIL, dwStates[2]);
                                    SetRenderState(D3DRS_STENCILZFAIL, dwStates[3]);
                                    SetRenderState(D3DRS_STENCILPASS, dwStates[4]);

                                    if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                    {
                                        Flood((D3DVALUE) 0.5, m_dwFailColor);

                                        if (EndScene())
                                        {
                                            SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

                                            SetRenderState(D3DRS_STENCILREF, dwReference[1]);

                                            SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
                                            SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                                            SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                                            SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                            {
                                                Flood((D3DVALUE) 0.5, m_dwPassColor);

                                                if (EndScene())
                                                {
                                                    m_fPassPercentage = 1.0f;

                                                    if (!SupportedOperation(dwOp) || Compare())
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

                        bSuccess = true;
                    }

    return bSuccess;
}

const UINT NOPS = 8;

// NAME:        GetOp()
// DESC:        get operation string
// INPUT:       dwOp.....operation index
// OUTPUT:      szOps[dwOp]
// COMMENTS:    dwOp clamped to NOPS

LPSTR CStencilOp::GetOp(DWORD dwOp)
{
    static PTSTR szOps[] =
    {
        TEXT("D3DSTENCILOP_KEEP   "),
        TEXT("D3DSTENCILOP_ZERO   "),
        TEXT("D3DSTENCILOP_REPLACE"),
        TEXT("D3DSTENCILOP_INCRSAT"),
        TEXT("D3DSTENCILOP_DECRSAT"),
        TEXT("D3DSTENCILOP_INVERT "),
        TEXT("D3DSTENCILOP_INCR   "),
        TEXT("D3DSTENCILOP_DECR   "),
        TEXT("invalid op          ")
    };

    dwOp -= D3DSTENCILOP_KEEP;
    dwOp = (DWORD) ((dwOp < NOPS) ? dwOp : NOPS);
    return((LPTSTR) szOps[dwOp]);
}

const UINT NSTATES = 3;

// NAME:        GetState()
// DESC:        get state string
// INPUT:       dwState..state index
// OUTPUT:      szStates[dwState]
// COMMENTS:    dwState clamped to NSTATES

LPSTR CStencilOp::GetState(DWORD dwState)
{
    static PTSTR szStates[] =
    {
        TEXT("D3DRS_STENCILFAIL "),
        TEXT("D3DRS_STENCILZFAIL"),
        TEXT("D3DRS_STENCILPASS "),
        TEXT("invalid state     ")
    };

    dwState -= D3DRS_STENCILFAIL;
    dwState = (DWORD) ((dwState < NSTATES) ? dwState : NSTATES);
    return((LPTSTR) szStates[dwState]);
}
