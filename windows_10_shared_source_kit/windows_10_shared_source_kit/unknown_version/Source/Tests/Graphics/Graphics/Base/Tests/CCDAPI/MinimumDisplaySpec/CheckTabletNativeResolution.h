
#pragma once
#include <windows.h>
#include "AbstractTest.h"

class CheckTabletNativeResolution : public AbstractTest
{
    public:

        CheckTabletNativeResolution( LoggerWrapper* pLogger );

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
