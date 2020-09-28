// FILE:        cap.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil caps

#include "stencil.h"

// NAME:        CStencilCap()
// DESC:        stencil cap class constructor
// INPUT:       none
// OUTPUT:      none

CStencilCap::CStencilCap()
{
    m_szCommandKey = "Cap";
}

// NAME:        ~CStencilCap()
// DESC:        stencil cap class destructor
// INPUT:       none
// OUTPUT:      none

CStencilCap::~CStencilCap()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CStencilCap::CommandLineHelp(VOID)
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilCap::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    SetTestRange((UINT) 1, (UINT) 1);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest
// OUTPUT:      true.....if test executes successfully
//              false....otherwise

bool CStencilCap::ExecuteTest(UINT uTest)
{
    bool bSuccess = true;

    CStencil::ExecuteTest(uTest);

    BeginTestCase("Bit depth verification", uTest);

    if (m_dwStencilBitDepth == m_dwStencilBitDepthReported)
        Pass();
    else
    {
        WriteToLog("Bit depth verification failed:  %d reported, %d computed.\n",
                    m_dwStencilBitDepthReported, m_dwStencilBitDepth);
        Fail();
    }

    m_fPassPercentage = (bSuccess) ? 1.0f : 0.0f;
    return bSuccess;
}
