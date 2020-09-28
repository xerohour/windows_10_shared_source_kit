#pragma once
#include <windows.h>
#include "EdidTestBase.h"

class CheckAdapterReportedModes : public EdidTestBase
{
    public:

        CheckAdapterReportedModes( LoggerWrapper* pLogger );

    protected:

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();

		
	private:
		struct Internals;
		Internals* m_pInternals;

	    TestResult::Type Verify( DISPLAY_DEVICE const* pAdapter, DISPLAY_DEVICE const* pDisplay );

};
