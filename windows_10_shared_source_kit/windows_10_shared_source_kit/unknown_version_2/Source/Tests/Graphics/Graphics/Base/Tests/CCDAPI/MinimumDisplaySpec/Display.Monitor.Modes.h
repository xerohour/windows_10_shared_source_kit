#pragma once

#include <windows.h>
#include "EdidTestBase.h"

class DisplayMonitorModes : public EdidTestBase
{
    public:

        DisplayMonitorModes( LoggerWrapper* pLogger );

    protected:

        virtual TestResult::Type ExecuteTestCase();

        virtual tstring GetTestName();
};
