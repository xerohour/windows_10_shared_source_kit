#pragma once
#include <windows.h>
#include "AbstractAdapterTest.h"

class MinimumMemoryRequired : public AbstractAdapterTest
{
    public:

        MinimumMemoryRequired( LoggerWrapper* pLogger );

    protected:

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();
};

