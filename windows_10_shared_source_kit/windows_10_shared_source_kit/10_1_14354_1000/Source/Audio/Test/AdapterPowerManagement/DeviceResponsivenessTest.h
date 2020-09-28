//--------------------------------------------------------------------------
//
//  File: DeviceResponsivenessTest.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      5/1/2013    Rbeber       Splitting out to own file.
//
//--------------------------------------------------------------------------

class DeviceResponsivenessTest: public AdapterPowerManagementTest, public WEX::TestClass<DeviceResponsivenessTest>
{

    BEGIN_TEST_CLASS(DeviceResponsivenessTest)
    END_TEST_CLASS()

    TEST_CLASS_SETUP(SetupClass);
    TEST_CLASS_CLEANUP(CleanupClass);

    TEST_METHOD_SETUP(SetupTest);
    TEST_METHOD_CLEANUP(CleanupTest);

    BEGIN_TEST_METHOD(PowerChangeHCK_ScreenOn_ResponseFast)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"HCK" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = '*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(PowerChangeHCK_ScreenOff_ResponseResponsive)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"HCK" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = '*'" )
    END_TEST_METHOD()

private:
};
