////////////////////////////////////////////////////////////////////////////////
// MarkerValidation.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimingData.h"

////////////////////////////////////////////////////////////////////////////////
// CMarkerValidationTest
////////////////////////////////////////////////////////////////////////////////
class CMarkerValidationTest : public CTimingDataTest
{
public:
    CMarkerValidationTest(bool fEnableETW = true);
    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();

protected:
};

class CMarkerValidationTest_CommandList : public CMarkerValidationTest
{
public:
    TEST_RESULT Setup();
    TEST_RESULT ExecuteTestCase();
};

class CMarkerValidationTest_APISequence : public CMarkerValidationTest
{
public:
    TEST_RESULT SetupTestCase();
    TEST_RESULT ExecuteTestCase();
};

class CMarkerValidationTest_Optimized : public CMarkerValidationTest
{
public:
    TEST_RESULT ExecuteTestCase();
};

class CMarkerValidationTest_Flush : public CMarkerValidationTest
{
public:
    TEST_RESULT SetupTestCase();
    TEST_RESULT ExecuteTestCase();
};

class CMarkerValidationTest_CommandBufferLimit : public CMarkerValidationTest
{
public:
    TEST_RESULT ExecuteTestCase();
};

class CMarkerValidationTest_Range : public CMarkerValidationTest
{
public:
    TEST_RESULT ExecuteTestCase();
};