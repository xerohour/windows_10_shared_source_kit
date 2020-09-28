////////////////////////////////////////////////////////////////////////////////
// MultipleEngine.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimingData.h"

#define ENGINE_RESULTS_FILE L"WGF11TimingData_engine.csv"

////////////////////////////////////////////////////////////////////////////////
//
// CMultipleEngineTest
//
////////////////////////////////////////////////////////////////////////////////

class CMultipleEngineTest : public CTimingDataTest
{
public:
    CMultipleEngineTest();

    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();

    TEST_RESULT ValidateEngine();

protected:
    CDxEtwHelper *m_pEngineHelper;
};

class CMultipleEngineTest_Draw : public CMultipleEngineTest
{
public:
    virtual TEST_RESULT ExecuteTestCase();

protected:
};

class CMultipleEngineTest_Copy : public CMultipleEngineTest
{
public:
    CMultipleEngineTest_Copy();

    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual void CleanupTestCase();

protected:
    ID3D11Texture2D *m_pDestTex;
};

class CMultipleEngineTest_Compute : public CMultipleEngineTest
{
public:
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT ExecuteTestCase();

protected:
};

