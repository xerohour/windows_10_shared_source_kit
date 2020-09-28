#pragma once
#include <windows.h>
#include "AbstractTest.h"

class CheckColorDepth : public AbstractTest
{
    public:

        CheckColorDepth( LoggerWrapper* pLogger );

    protected:

		virtual void InitializeParameters();

        virtual TestResult::Type SetupTest();

        virtual TestResult::Type SetupTestCase();

        virtual TestResult::Type ExecuteTestCase();

        virtual void CleanupTestCase();

        virtual void CleanupTest();

        virtual tstring GetTestName();

	    ccd::DisplayTopology m_DisplayTopology;

		LUID   m_AdapterLUID;
		UINT32 m_nSourceId;

	private:
		struct Internal;
		Internal* m_pInternals;
};

