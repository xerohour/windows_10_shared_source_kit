#pragma once
#include <windows.h>
#include "AbstractTest.h"

class CheckPostAdapterBIOSColorDepth : public AbstractTest
{
    public:

        CheckPostAdapterBIOSColorDepth( LoggerWrapper* pLogger );

    protected:

		virtual void InitializeParameters();

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();

		UINT32 m_Value;
		
};

