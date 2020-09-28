// FILE:        cullone.cpp
// DESC:        stencil conformance tests for cull modes (one sided stencil)
// AUTHOR:      Todd M. Frost
// COMMENTS:

#include "stencil.h"

// NAME:        CStencilCullOne()
// DESC:        stencil cull one class constructor
// INPUT:       none
// OUTPUT:      none

CStencilCullOne::CStencilCullOne()
{
    m_szCommandKey = "CullOne";
    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilCullOne()
// DESC:        stencil cull one class destructor
// INPUT:       none
// OUTPUT:      none

CStencilCullOne::~CStencilCullOne()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilCullOne::CommandLineHelp(VOID)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilCullOne::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilCullOne::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = (DWORD) 0xffffffff;

    for (k = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++, k++)
            if (k == uTest)
            {
                Test(((i == 0) ? false : true), (DWORD) 0, dwMasks[j], uTest);
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

VOID CStencilCullOne::Test(bool bStipple, DWORD dwClear, DWORD dwMask, UINT uTest)
{
    CHAR szStipple[2][13] =
    {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    UINT i = bStipple ? 1 : 0;

    wsprintf(m_szBuffer, "D3DRS_CULLMODE (one sided):  0x%08X, 0x%08X, %s", dwClear, dwMask, szStipple[i]);
    BeginTestCase(m_szBuffer, uTest);

    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
    {
        Stipple(bStipple);

        SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_ALWAYS);

        SetRenderState(D3DRS_STENCILENABLE, (DWORD) TRUE);
        SetRenderState(D3DRS_STENCILREF, dwClear);
        SetRenderState(D3DRS_STENCILMASK, dwMask);
        SetRenderState(D3DRS_STENCILWRITEMASK, dwMask);
        SetRenderState(D3DRS_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CCW);    // cull counter clockwise triangles (default)
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CCW);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_CW);     // cull clockwise oriented triangles
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
        Flood((D3DVALUE) 0.5, m_dwFailColor, FLOOD_CW);

        SetRenderState(D3DRS_CULLMODE, (DWORD) D3DCULL_NONE);   // do not cull clockwise or counter clockwise triangles
        SetRenderState(D3DRS_STENCILFUNC, (DWORD) D3DCMP_NEVER);
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
