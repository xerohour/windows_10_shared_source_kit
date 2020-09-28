// FILE:        caps.cpp
// DESC:        caps class methods for point sprite conformance tests
// AUTHOR:      Anthony W. Clarke
// COMMENTS:    tests for point sprite caps

#include "sprite.h"

// NAME:        CSpriteCap()
// DESC:        constructor for sprite cap class
// INPUT:       none
// OUTPUT:      none

CSpriteCap::CSpriteCap()
{
    m_szTestName = "Sprite caps";
    m_szCommandKey = "Caps";

    m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteCap()
// DESC:        destructor for sprite cap class
// INPUT:       none
// OUTPUT:      none

CSpriteCap::~CSpriteCap()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CSpriteCap::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) 1);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest
// OUTPUT:      true.....if test executes successfully
//              false....otherwise
// COMMENTS:    not a requirement for Dx8, however, drivers should cap max size => MIN_MAXPOINTSIZE

bool CSpriteCap::ExecuteTest(UINT uTest)
{
    if (1 == uTest)
    {
        BeginTestCase("Caps check", uTest);

        if (m_Sprite.PointSizeMax.f < MIN_MAXPOINTSIZE)
        {
            WriteToLog("Note:  dvMaxPointSize = %f (expected %f or greater).\n", m_Sprite.PointSizeMax.f, MIN_MAXPOINTSIZE);
            Fail();
        }
        else
            Pass();
    }

    return false; // never perform image comparison
}
