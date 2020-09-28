#pragma once
#include <windows.h>
#include "AbstractTest.h"

class AbstractAdapterTest : public AbstractTest
{
    public:

        LUID m_Adapter;
        tstring m_AdapterStr;
        ccd::DisplayTopology m_DisplayTopology;

        AbstractAdapterTest( LoggerWrapper* pLogger );

    protected:

        virtual void InitializeParameters();
};
