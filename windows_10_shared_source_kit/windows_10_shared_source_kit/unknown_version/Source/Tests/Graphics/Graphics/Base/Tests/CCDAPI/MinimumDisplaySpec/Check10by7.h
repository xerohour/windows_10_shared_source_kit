
#pragma once
#include <windows.h>
#include "AbstractTest.h"

class Check10by7 : public AbstractTest
{
    public:

        Check10by7( LoggerWrapper* pLogger );

    protected:

        virtual void InitializeParameters();

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();

    private:
        UINT32 m_MinimumDisplayPixelWidth;
        UINT32 m_MinimumDisplayPixelHeight;
        ccd::DisplayTopology m_DisplayTopology;
};
