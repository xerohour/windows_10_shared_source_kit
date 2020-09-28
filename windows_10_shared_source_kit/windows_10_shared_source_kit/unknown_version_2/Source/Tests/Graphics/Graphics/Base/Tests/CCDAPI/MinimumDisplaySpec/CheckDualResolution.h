#pragma once
#include <windows.h>
#include "AbstractTest.h"

class CheckDualResolution : public AbstractTest
{
    public:

        CheckDualResolution( LoggerWrapper* pLogger );

    protected:

        virtual void InitializeParameters();

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();

    private:
        UINT m_ModeTest;
        ccd::DisplayTopology m_DisplayTopology;
};
