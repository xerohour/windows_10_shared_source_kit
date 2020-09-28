#pragma once
#include <windows.h>
#include "AbstractTest.h"

struct DisplayIdType
{
    UINT32 m_Value;

    DisplayIdType( UINT32 v = 0 );
};

tstring ToString( DisplayIdType const& id );
bool FromString( DisplayIdType* pId, tstring const& str );
bool operator ==( DisplayIdType const& lhs, DisplayIdType const& rhs );

struct MinimumResolution : public AbstractTest
{
    #define MAX_ADAPTERS 4
    #define MAX_SOURCES 16
    #define MAX_TARGETS 16
    #define MAX_PATHS 16

    #define MIN_WIDTH 1024
    #define MIN_HEIGHT 768

    MinimumResolution( LoggerWrapper* pLogger );

    struct AdapterTestSet
    {
        LUID m_AdapterLUID;
        tstring m_AdapterString;
        CTestCaseParameter<LUID>* m_pAdapterParam;
        CTestCaseParameter<tstring>* m_pAdapterStrParam;

        UINT32 m_nSourceCount;
        UINT32 m_nTargetCount;
        DisplayIdType m_pSourceId[MAX_SOURCES];
        DisplayIdType m_pTargetId[MAX_TARGETS];

        // A Path is defined as src[i] -> trg[i]
        CTestCaseParameter<DisplayIdType>* m_pSourceParams[MAX_SOURCES];
        CTestCaseParameter<DisplayIdType>* m_pTargetParams[MAX_TARGETS];
    };

    ccd::DisplayTopology m_DisplayTopology;
    UINT32 m_nAdapterCount;
    AdapterTestSet m_pAdapters[MAX_ADAPTERS];

	virtual void InitializeParameters();

	virtual TestResult::Type SetupTest();

	virtual TestResult::Type SetupTestCase();

	virtual TestResult::Type ExecuteTestCase();

	virtual void CleanupTestCase();

	virtual void CleanupTest();

    virtual tstring GetTestName();
};
