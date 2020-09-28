#pragma once

#include <windows.h>
#include "AbstractTest.h"

class EdidTestBase : public AbstractTest
{
    public:

        EdidTestBase( LoggerWrapper* pLogger );

    protected:

        virtual void InitializeParameters();

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        // Call this from SetupTestCase
        TestResult::Type SetTestCaseTopology();

    protected:
        LUID m_AdapterLuid;
        HEX32 m_TargetId;

        // Not valid until SetTestCaseTopology() is called
        DISPLAY_DEVICE m_GDISource;
        DISPLAY_DEVICE m_GDITarget;
        DISPLAYCONFIG_PATH_INFO m_TestCasePath;

        // cached QDC all results
        ccd::DisplaySettings m_AllPaths;

        ccd::DisplayTopology m_DisplayTopology;
};

