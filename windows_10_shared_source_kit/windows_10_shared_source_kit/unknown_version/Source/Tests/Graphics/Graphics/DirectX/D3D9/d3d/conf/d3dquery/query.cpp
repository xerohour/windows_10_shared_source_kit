//
//  Query.cpp
//
//  IDirect3DQuery9 Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2002 Microsoft Corporation.
//
//
#include "resource.h"
#include "CD3DTest.h"
#include "CTestPtr.h"
#include "CQuery.h"
#include "CSurface.h"
#include "CBuffers.h"
#include "CTextures.h"
#include "Utils.h"

#include "d3d9.h"
#include "D3DX9.h"

#include <Powrprof.h>


/************************************************************
 * Declare the framework									*
 ************************************************************/
CD3DWindowFramework		App;


//-----------------------------------------------------------------------------
// local defs
//-----------------------------------------------------------------------------

typedef struct _D3DTEST_QUERY
{
    TCHAR      szName[MAX_PATH];
    QUERYTYPE  QueryType;
	PTR_TYPE   IdType;
	DWORD      dwIssueFlags;
    DWORD      dwGetDataFlags;
    PTR_TYPE   DataPointerType;
    DWORD      dwDataSize;
	UINT       nAlignmentOffset;
    RET_TYPE   ReturnType;
} D3DTEST_QUERY, *LPD3DTEST_QUERY;


typedef struct _QUERYTHREAD_DATA
{
    class CQueryConf* pTest;
	CQuery* pQuery;
	QUERYTYPE QueryType;
	DWORD   dwFlags;
	LPVOID  pData;
    DWORD   dwDataSize;
    DWORD   dwIssue;
} QUERYTHREAD_DATA;

// internal QueryTypes
#define QUERYTYPE_DDISTATS 7

#define QUERYTHREAD_OK        0
#define QUERYTHREAD_EXCEPTION QUERYTHREAD_OK+1
#define QUERYTHREAD_NOFLUSH   QUERYTHREAD_OK+2
#define QUERYTHREAD_FAILED    QUERYTHREAD_OK+3


template< class T >
struct ReleaseIt
{
    void operator()( T *pT ) const
    {
        if ( pT )
        {
            pT->Release();
        }
    }
};


//-----------------------------------------------------------------------------
// test case list
//-----------------------------------------------------------------------------

static D3DTEST_QUERY g_TestList[ ] =
{
	_T("Query (QUERYTYPE_RESOURCEMANAGER)"),    QUERYTYPE_RESOURCEMANAGER,      PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_RESOURCEMANAGER),         8, RET_VALID,
	_T("Query (QUERYTYPE_VERTEXSTATS)"),        QUERYTYPE_VERTEXSTATS,          PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_D3DVERTEXSTATS),          8, RET_VALID,
	_T("Query (QUERYTYPE_EVENT)"),              QUERYTYPE_EVENT,                PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(BOOL),                            8, RET_VALID,
	_T("Query (QUERYTYPE_OCCLUSION)"),          QUERYTYPE_OCCLUSION,            PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DWORD),                           8, RET_VALID,
	_T("Query (QUERYTYPE_VCACHE)"),             QUERYTYPE_VCACHE,               PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_VCACHE),                  8, RET_VALID,
	_T("Query (QUERYTYPE_TIMESTAMP)"),          QUERYTYPE_TIMESTAMP,            PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(UINT64),                          8, RET_VALID,
	_T("Query (QUERYTYPE_TIMESTAMPDISJOINT)"),  QUERYTYPE_TIMESTAMPDISJOINT,    PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(BOOL),                            8, RET_VALID,
	_T("Query (QUERYTYPE_TIMESTAMPFREQ)"),      QUERYTYPE_TIMESTAMPFREQ,        PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(UINT64),                          8, RET_VALID,
	_T("Query (QUERYTYPE_PIPELINETIMINGS)"),    QUERYTYPE_PIPELINETIMINGS,      PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9PIPELINETIMINGS),     8, RET_VALID,
	_T("Query (QUERYTYPE_INTERFACETIMINGS)"),   QUERYTYPE_INTERFACETIMINGS,     PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9INTERFACETIMINGS),    8, RET_VALID,
	_T("Query (QUERYTYPE_VERTEXTIMINGS)"),      QUERYTYPE_VERTEXTIMINGS,        PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9STAGETIMINGS),        8, RET_VALID,
	_T("Query (QUERYTYPE_PIXELTIMINGS)"),       QUERYTYPE_PIXELTIMINGS,         PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9STAGETIMINGS),        8, RET_VALID,
	_T("Query (QUERYTYPE_BANDWIDTHTIMINGS)"),   QUERYTYPE_BANDWIDTHTIMINGS,     PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9BANDWIDTHTIMINGS),    8, RET_VALID,
    _T("Query (QUERYTYPE_CACHEUTILIZATION)"),   QUERYTYPE_CACHEUTILIZATION,     PTR_OK,      ISSUE_BEGIN|ISSUE_END,   0,  PTR_OK,      sizeof(DEVINFO_D3D9CACHEUTILIZATION),    8, RET_VALID,
#ifndef _IA64_
	_T("Query (DWORD Align+1)"),             QUERYTYPE_RESOURCEMANAGER,     PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_RESOURCEMANAGER),   5, RET_VALID,
	_T("Query (DWORD Align+2)"),             QUERYTYPE_RESOURCEMANAGER,     PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_RESOURCEMANAGER),   6, RET_VALID,
	_T("Query (DWORD Align+3)"),             QUERYTYPE_RESOURCEMANAGER,     PTR_OK,      ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_RESOURCEMANAGER),   7, RET_VALID,
#endif // _IA64_
	_T("Query (pQuery = INVALID)"),          QUERYTYPE_VERTEXSTATS,         PTR_INVALID, ISSUE_END,               0,  PTR_OK,      sizeof(DEVINFO_D3DVERTEXSTATS),    8, RET_INVALID,
	_T("Query (Flags =-INVALID)"),           QUERYTYPE_VERTEXSTATS,         PTR_OK,      0x98273349,              0,  PTR_OK,      sizeof(DEVINFO_D3DVERTEXSTATS),    8, RET_INVALID,
	_T("Query (Flags = INVALID)"),           QUERYTYPE_VERTEXSTATS,         PTR_OK,      0x4,                     0,  PTR_OK,      sizeof(DEVINFO_D3DVERTEXSTATS),    8, RET_INVALID,
	_T("Query (pData = INVALID)"),           QUERYTYPE_RESOURCEMANAGER,     PTR_OK,      ISSUE_END,               0,  PTR_INVALID, sizeof(DEVINFO_RESOURCEMANAGER),   8, RET_INVALID,
	_T("Query (pData = NULL)"),              QUERYTYPE_RESOURCEMANAGER,     PTR_OK,      ISSUE_END,               0,  PTR_NULL,    sizeof(DEVINFO_RESOURCEMANAGER),   8, RET_INVALID,
};


//-----------------------------------------------------------------------------
// Definition of test classes
//-----------------------------------------------------------------------------

class CQueryConf : public CD3DTest
{
public:
    CQueryConf();
    template<class T>
    bool verifyEQ( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a == b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
	template<class T>
    bool verifyEQ_noFail( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a == b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList);
        return false;
    }
    template<class T>
    bool verifyNE( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a != b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyGT( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a > b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyGT_noFail( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a > b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList);
        return false;
    }
    template<class T>
    bool verifyGE( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a >= b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyGE_noFail( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a >= b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList);
        return false;
    }
    template<class T>
    bool verifyLT( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a < b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyLE( const T &a, const T &b, const TCHAR *ptcsFmt, ... )
    {
        if ( a <= b )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyIN( const T &a, const T &l, const T &h, const TCHAR *ptcsFmt, ... )
    {
        if ( l <= a && a <= h )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList, true);
        return false;
    }
    template<class T>
    bool verifyIN_noFail( const T &a, const T &l, const T &h, const TCHAR *ptcsFmt, ... )
    {
        if ( l <= a && a <= h )
            return true;

	    va_list	argList;
	    va_start(argList, ptcsFmt);
        writeToLog(ptcsFmt, argList);
        return false;
    }

    virtual UINT TestInitialize();
    HRESULT flush( CDevice *pDevice, bool bBackBuff = true, bool bClear = true, unsigned nRepeat = 1 );
protected:
    bool m_bIsDebug;
    D3DPOOL m_PoolManaged;
    static const QUERYTYPE QueryTypes[];
    static const char *QueryText[];
    static const unsigned nQueryTypeCount;
    static const float LowBoundPct;
    static const float HiBoundPct;

    float areaTriangle( void *pBuffer, unsigned stride, unsigned offset = 0 );
private:
    void writeToLog(const TCHAR *ptcsFmt, va_list &argList, bool bFail = false);
};

class CQueryUnknown : public CQueryConf
{
public:
	CQueryUnknown();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryGetData : public CQueryConf
{
public:
	CQueryGetData();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryIssue : public CQueryConf
{
public:
	CQueryIssue();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryEvent : public CQueryConf
{
public:
	CQueryEvent();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
};

class CQueryOcclusion : public CQueryConf
{
public:
	CQueryOcclusion();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
private:
    float areaTrapazoid( float a, float b, float h )
    {
        return 0.5f * ( a + b ) * h;
    }
    float projectedArea( float a, float w, float h )
    {
        return 0.25f * a * w * h;
    }
};

class CQueryVertexStats : public CQueryConf
{
public:
	CQueryVertexStats();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
};

class CQueryVCACHE : public CQueryConf
{
public:
	CQueryVCACHE();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
};

class CQueryResourceManager : public CQueryConf
{
public:
	CQueryResourceManager();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
};

class CQueryCreate : public CQueryConf
{
public:
	CQueryCreate();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryTest : public CQueryConf
{
protected:
	D3DTEST_QUERY* m_pTestList;
	virtual UINT CreateTestList();
	virtual void ReleaseTestList();
	virtual bool OnException(LPCTSTR szLabel, bool bWasInvalid);
public:
	CQueryTest();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
	virtual bool TestTerminate();
};

class CQueryCreateMany : public CQueryConf
{
public:
	CQueryCreateMany();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryTimeStamp : public CQueryConf
{
public:
	CQueryTimeStamp();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual	bool RunTimestampScenario(UINT uTestNumber);
	virtual bool CallProcess(LPSTR pszwCommand);
	virtual UINT TestInitialize();
};

class CQueryTimeStampFrequency : public CQueryConf
{
public:
	CQueryTimeStampFrequency();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryTimeStampDisjoint : public CQueryConf
{
public:
	CQueryTimeStampDisjoint();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryPipelineTimings : public CQueryConf
{
public:
	CQueryPipelineTimings();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryInterfaceTimings : public CQueryConf
{
public:
	CQueryInterfaceTimings();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryInterfaceTimingsFS : public CQueryConf
{
public:
	CQueryInterfaceTimingsFS();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryVertexTimings : public CQueryConf
{
public:
	CQueryVertexTimings();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryPixelTimings : public CQueryConf
{
public:
	CQueryPixelTimings();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryBandwidthTimings : public CQueryConf
{
public:
	CQueryBandwidthTimings();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

class CQueryCacheUtilization : public CQueryConf
{
public:
	CQueryCacheUtilization();
    virtual bool CapsCheck();
	virtual	bool ExecuteTest(UINT uTestNumber);
	virtual UINT TestInitialize();
};

//-----------------------------------------------------------------------------
// Static class members
//-----------------------------------------------------------------------------

const QUERYTYPE CQueryConf::QueryTypes[] =
{
	QUERYTYPE_RESOURCEMANAGER,
	QUERYTYPE_VERTEXSTATS,
	QUERYTYPE_EVENT,
    QUERYTYPE_OCCLUSION,
	QUERYTYPE_VCACHE,
    QUERYTYPE_TIMESTAMP,        
    QUERYTYPE_TIMESTAMPDISJOINT,
    QUERYTYPE_TIMESTAMPFREQ,    
    QUERYTYPE_PIPELINETIMINGS,  
    QUERYTYPE_INTERFACETIMINGS, 
    QUERYTYPE_VERTEXTIMINGS,    
    QUERYTYPE_PIXELTIMINGS,     
    QUERYTYPE_BANDWIDTHTIMINGS, 
    QUERYTYPE_CACHEUTILIZATION
};

const char *CQueryConf::QueryText[] =
{
	"QUERYTYPE_RESOURCEMANAGER",
	"QUERYTYPE_VERTEXSTATS",
	"QUERYTYPE_EVENT",
    "QUERYTYPE_OCCLUSION",
	"QUERYTYPE_VCACHE",
    "QUERYTYPE_TIMESTAMP",
    "QUERYTYPE_TIMESTAMPDISJOINT",
    "QUERYTYPE_TIMESTAMPFREQ",
    "QUERYTYPE_PIPELINETIMINGS",
    "QUERYTYPE_INTERFACETIMINGS",
    "QUERYTYPE_VERTEXTIMINGS",
    "QUERYTYPE_PIXELTIMINGS",
    "QUERYTYPE_BANDWIDTHTIMINGS",
    "QUERYTYPE_CACHEUTILIZATION"
};

const unsigned CQueryConf::nQueryTypeCount = sizeof QueryTypes / sizeof QueryTypes[0];
const float CQueryConf::LowBoundPct = 0.9f;
const float CQueryConf::HiBoundPct = 1.1f;

//-----------------------------------------------------------------------------
// Test objects
//-----------------------------------------------------------------------------
#if FAST_TEST
CQueryCreate                tstQueryCreate;
CQueryTest                  tstQueryStatus;
CQueryGetData               tstQueryGetData;
CQueryIssue                 tstQueryIssue;
CQueryOcclusion             tstQueryOcclusion;
CQueryEvent                 tstQueryEvent;
CQueryTimeStamp             tstQueryTimeStamp;
#else //FAST_TEST
CQueryCreate                tstQueryCreate;
CQueryTest                  tstQueryStatus;
CQueryGetData               tstQueryGetData;
CQueryIssue                 tstQueryIssue;
CQueryUnknown               tstQueryUnknown;
CQueryVertexStats           tstQueryVertexStats;
CQueryVCACHE                tstQueryVCACHE;
CQueryResourceManager       tstQueryResourceManager;
CQueryOcclusion             tstQueryOcclusion;
CQueryEvent                 tstQueryEvent;
CQueryCreateMany            tstQueryCreateMany;
CQueryTimeStamp             tstQueryTimeStamp;
CQueryPipelineTimings       tstQueryPipelineTimings;
CQueryInterfaceTimings      tstQueryInterfaceTimings;
CQueryInterfaceTimingsFS    tstQueryInterfaceTimings2;
CQueryVertexTimings         tstQueryVertexTimings;
#endif//FAST_TEST
//CQueryPixelTimings          tstQueryPixelTimings;
//CQueryBandwidthTimings      tstQueryBandwidthTimings;
//CQueryCacheUtilization      tstQueryCacheUtilization;
//CQueryTimeStampDisjoint     tstQueryTimeStampDisjoint;

// This test is invalid since the query can return any value for frequency (see Win7 718304)
//CQueryTimeStampFrequency    tstQueryTimeStampFrequency; 

//-----------------------------------------------------------------------------
// GetData
// Helper function(s) for time-out fetch of query data
//-----------------------------------------------------------------------------

static HRESULT GetData( CQuery *pQuery, 
					    void *pData, 
						DWORD dwSize, 
						DWORD dwFlags, 
						unsigned *pCount = 0,		// returns number times pQuery->GetData() was called
						double *pTime = 0,			// returns total time of loop iteration
						double dTimeOut = 1.0 )     // maximum looping time allowed
{
    HRESULT hr;
	double dTime = 0.0;
    unsigned count = 0;
	CPerformanceCounter timer;

	timer.start();

    do
    {
        count++;

        hr = pQuery->GetData( pData, dwSize, dwFlags );
    }
    while ( dwFlags == GETDATA_FLUSH && 
		    ( dTime = timer.finish() ) <= dTimeOut && 
			SUCCEEDED(hr) && 
			hr == S_FALSE );

    if ( pCount )
    {
        *pCount = count;
    }

	if ( pTime )
	{
		*pTime = dTime;
	}

    return hr;
}


//-----------------------------------------------------------------------------
// ThreadTimer
// Created as a seperate thread to "time out" if Query hangs while
// retrieving query.
//-----------------------------------------------------------------------------

DWORD WINAPI QueryThread(QUERYTHREAD_DATA* pData)
{
	HRESULT hr = S_OK;

	// the main test follows...
	try
    {
        if ( pData->pQuery->GetType() != pData->QueryType )
        {
            pData->pTest->WriteToLog(_T("GetType() returned %d, expected %d\n"), pData->pQuery->GetType(), pData->QueryType);
            return QUERYTHREAD_FAILED;
        }

        if ( pData->pQuery->GetDataSize() != pData->dwDataSize )
        {
            pData->pTest->WriteToLog(_T("GetDataSize() returned %d, expected %d\n"), pData->pQuery->GetDataSize(), pData->dwDataSize);
            return QUERYTHREAD_FAILED;
        }

        if ( FAILED( hr = pData->pQuery->Issue( pData->dwIssue ) ) )
        {
            return QUERYTHREAD_FAILED;
        }

		hr = pData->pQuery->GetData( pData->pData, pData->dwDataSize, pData->dwFlags );
	}
	// validate against an exception
	catch(...)
	{
		return QUERYTHREAD_EXCEPTION;
	}

    DEVINFO_RESOURCEMANAGER *pRM = (DEVINFO_RESOURCEMANAGER*)pData->pData;
    DEVINFO_D3DVERTEXSTATS *pVS = (DEVINFO_D3DVERTEXSTATS*)pData->pData;

	// couldn't get the query -- flush it out
	if( hr == D3DERR_NOTAVAILABLE )
	{
        CDevice *pDevice = 0;

		if(SUCCEEDED(hr = pData->pQuery->GetDevice(&pDevice)))
		{
            pData->pTest->flush( pDevice );

            RELEASE( pDevice );
		}

		try
		{
			hr = pData->pQuery->GetData( pData->pData, pData->dwDataSize, pData->dwFlags );
		}
		catch(...)
		{
			return QUERYTHREAD_EXCEPTION;
		}

		if( hr == D3DERR_NOTAVAILABLE )
			return QUERYTHREAD_NOFLUSH;
	}

	return (SUCCEEDED(hr) ? QUERYTHREAD_OK : QUERYTHREAD_FAILED);
}


//-----------------------------------------------------------------------------
// Implementation of test classes
//-----------------------------------------------------------------------------

CQueryUnknown::CQueryUnknown()
{
	m_szTestName = _T("QueryUnknown");
    m_szCommandKey = _T("QueryUnknown");
}

UINT CQueryUnknown::TestInitialize()
{
    SetTestRange(1,nQueryTypeCount * 3);

    return CQueryConf::TestInitialize();
}

bool CQueryUnknown::ExecuteTest(UINT nTest)
{
    const unsigned nCases = 3;
    unsigned iQuery = ( nTest - 1 ) / nCases;
    unsigned iCase  = ( nTest - 1 ) % nCases;

    char title[256];
    strcpy(title, "Query Unknown Interface: ");
    strcat(title, QueryText[ iQuery ]);

    BeginTestCase( title, nTest );

    CBoolException bResult;

    try
    {
        CAutoRelease< CQuery > pQuery;
        HRESULT hr = m_pSrcDevice->CreateQuery( QueryTypes[iQuery], &pQuery );
        if ( FAILED( hr ) )
        {
            WriteToLog("CreateQuery failed: %s\n", m_pD3D->HResultToString( hr ) );
            Skip();
            GetLastError();
            return false;
        }

        if ( iCase == 0 )
        {
            UINT count0 = getRefCount( (CQuery*) pQuery );
        
            UINT count1 = pQuery->AddRef();

            bResult = verifyEQ( count1, 
                                count0 + 1, 
                                TEXT("AddRef: Expecting (%d) Actual (%d)\n"), 
                                count0 + 1,
                                count1 );

            count1 = pQuery->Release();

            bResult = verifyEQ( count1, 
                                count0, 
                                TEXT("Release: Expecting (%d) Actual (%d)\n"), 
                                count0,
                                count1 );
        }
        else if ( iCase == 1 )
        {
            CAutoRelease< CDevice > pDevice;

            hr = pQuery->GetDevice( &pDevice );

            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("GetDevice: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            bResult = verifyEQ( (CDevice*)pDevice, 
                                m_pSrcDevice, 
                                TEXT("GetDevice: Expecting (0x%x) Actual (0x%x)\n"), 
                                m_pSrcDevice,
                                pDevice );
            if ( m_bIsDebug )
            {

            	hr = pQuery->GetDevice( NULL );

            	bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("GetDevice: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                m_pD3D->HResultToString( hr ) );

                hr = pQuery->GetDevice( (CDevice**) INVALID_POINTER );

                bResult = verifyNE( hr, 
                                    S_OK, 
                                    TEXT("GetDevice: Expecting (%s) Actual (%s)\n"), 
                                    m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                    m_pD3D->HResultToString( hr ) );
            }
        }
        else if ( iCase == 2 )
        {
            hr = pQuery->QueryInterface( GetD3DGUID(IID_QUERY, m_pSrcDevice->GetInterface()), NULL );

            bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("QueryInterface: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                m_pD3D->HResultToString( hr ) );

            if ( m_bIsDebug )
            {
                hr = pQuery->QueryInterface( GetD3DGUID(IID_QUERY, m_pSrcDevice->GetInterface()), (void**)INVALID_POINTER );

                bResult = verifyNE( hr, 
                                    S_OK, 
                                    TEXT("QueryInterface: Expecting (%s) Actual (%s)\n"), 
                                    m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                    m_pD3D->HResultToString( hr ) );
            }

            CAutoRelease< IUnknown > pIQuery;
            hr = pQuery->QueryInterface( GetD3DGUID(IID_QUERY, m_pSrcDevice->GetInterface()), (void**)&pIQuery );

            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("QueryInterface: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            CAutoRelease< IUnknown > pIUnknown;
            hr = pQuery->QueryInterface( IID_IUnknown, (void**)&pIUnknown );

            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("QueryInterface: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );
            
            CAutoRelease< IUnknown > pID3D;
            hr = pQuery->QueryInterface( GetD3DGUID(IID_D3D, m_pSrcDevice->GetInterface()), (void**)&pID3D );

            bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("QueryInterface: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );
        }
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryUnknown: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryGetData::CQueryGetData()
{
	m_szTestName = _T("QueryGetData");
    m_szCommandKey = _T("QueryGetData");
}

UINT CQueryGetData::TestInitialize()
{
    SetTestRange(1,nQueryTypeCount * 7);

    return CQueryConf::TestInitialize();
}

bool CQueryGetData::ExecuteTest(UINT nTest)
{
    bool bReturn = true;
    CBoolException bResult;

    static char *CaseText[] =
    {
        "GetData (no Issue)",
        "GetData (combinations No Flush)",
        "GetData (combinations Flush)",
        "GetData (lost device 1)",
        "GetData (lost device 2)",
        "GetData (timeout)",
        "GetData (flush)"
    };

    static DWORD dwValidIssueFlags[] =
    {
        ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
    };

    static DWORD dwSize[] = 
    {
        sizeof DEVINFO_RESOURCEMANAGER,
        sizeof DEVINFO_D3DVERTEXSTATS,
        sizeof BOOL,
        sizeof DWORD,
        sizeof DEVINFO_VCACHE,
        sizeof UINT64,                      
        sizeof BOOL,                        
        sizeof UINT64,                      
        sizeof DEVINFO_D3D9PIPELINETIMINGS, 
        sizeof DEVINFO_D3D9INTERFACETIMINGS,
        sizeof DEVINFO_D3D9STAGETIMINGS,    
        sizeof DEVINFO_D3D9STAGETIMINGS,    
        sizeof DEVINFO_D3D9BANDWIDTHTIMINGS,
        sizeof DEVINFO_D3D9CACHEUTILIZATION
    };

    const int nCases = sizeof( CaseText ) / sizeof( CaseText[0] );
    int iQuery = (nTest-1) / nCases;
    int iCase = (nTest-1) % nCases;

    char title[256];
    strcpy(title, QueryText[ iQuery ]);
    strcat(title, ": ");
    strcat(title, CaseText[ iCase ] );

    BeginTestCase( title, nTest );

    try
    {
        CAutoRelease< CQuery > pQuery;
        HRESULT hr = m_pSrcDevice->CreateQuery( QueryTypes[ iQuery ], &pQuery );
        if ( FAILED( hr ) )
        {
            WriteToLog("CreateQuery failed: %s\n", m_pD3D->HResultToString( hr ) );
            Skip();
            GetLastError();
            return false;
        }

        CAutoDelete<char> pData = new char[ dwSize[ iQuery ] ];
        zeroMem( pData.get(), dwSize[ iQuery ] );

        if ( iCase == 0 )
        {
            //"GetData (no Issue)",
            hr = pQuery->GetData( pData.get(), dwSize[ iQuery ], 0 );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("GetData (no Issue): Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            bResult = verifyNE( isZeroMem( pData.get(), dwSize[ iQuery ] ), 
                                true, 
                                TEXT("GetData (no Issue): Expecting garbage (non-NULL) data\n"));
        }
        else 
		{
			// determine if this is a synchronous or asynchronous query
			hr = pQuery->Issue( ISSUE_END );
			bResult = verifyEQ( hr, 
								S_OK,
								TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
								m_pD3D->HResultToString( S_OK ),
								m_pD3D->HResultToString( hr ) );

			bool synchronous = S_OK == pQuery->GetData( NULL, 0, 0 );

			if ( iCase == 1 )
			{
				//"GetData (combinations No Flush)",
				hr = pQuery->Issue( ISSUE_END );

				bResult = verifyGE( hr, 
									S_OK,
									TEXT("Issue: Expecting (S_OK or S_FALSE) Actual (%s)\n"), 
									m_pD3D->HResultToString( hr ) );

				hr = pQuery->GetData( NULL, 0, 0 );

				bResult = verifyGE( hr, 
									S_OK, 
									TEXT("GetData (NULL, zero) : Expecting (S_OK or S_FALSE) Actual (%s)\n"), 
									m_pD3D->HResultToString( hr ) );

				hr = pQuery->GetData( pData.get(), 0, 0 );

				bResult = verifyGE( hr, 
									S_OK, 
									TEXT("GetData (non-NULL, zero) : Expecting (S_OK or S_FALSE) Actual (%s)\n"), 
									m_pD3D->HResultToString( hr ) );

				// d3d9.L doesn't validate these parameters currently
				if ( m_pD3D->GetRuntimeVersion() < 9.1f )
				{
					hr = pQuery->GetData( NULL, dwSize[ iQuery ], 0 );
					bResult = verifyEQ( hr, 
										m_bIsDebug ? D3DERR_INVALIDCALL : S_FALSE, 
										TEXT("GetData (NULL, non-zero) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( m_bIsDebug ? D3DERR_INVALIDCALL : S_FALSE ),
										m_pD3D->HResultToString( hr ) );
				}

				hr = pQuery->GetData( pData.get(), dwSize[ iQuery ], 0 );

				bResult = verifyGE( hr, 
									S_OK, 
									TEXT("GetData (non-NULL, non-zero) : Expecting (S_OK or S_FALSE) Actual (%s)\n"), 
									m_pD3D->HResultToString( hr ) );

				// d3d9.L doesn't validate these parameters currently
				if ( m_pD3D->GetRuntimeVersion() < 9.1f )
				{
					hr = pQuery->GetData( pData.get(), dwSize[ iQuery ] + 1, 0 );
					bResult = verifyEQ( hr, 
										m_bIsDebug ? D3DERR_INVALIDCALL : S_FALSE, 
										TEXT("GetData (NULL, size mismatch) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( m_bIsDebug ? D3DERR_INVALIDCALL : S_FALSE ),
										m_pD3D->HResultToString( hr ) );
				}
			}
			else if ( iCase == 2 )
			{
				//"GetData (combinations Flush)",
				hr = pQuery->Issue( ISSUE_END );
				bResult = verifyEQ( hr, 
									S_OK,
									TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( S_OK ),
									m_pD3D->HResultToString( hr ) );

				if ( m_bIsDebug && m_pD3D->GetRuntimeVersion() < 9.1f )
				{
					hr = pQuery->GetData( NULL, dwSize[ iQuery ], GETDATA_FLUSH );
					bResult = verifyEQ( hr, 
										D3DERR_INVALIDCALL, 
										TEXT("GetData (NULL, non-zero) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
										m_pD3D->HResultToString( hr ) );

					hr = pQuery->GetData( pData.get(), dwSize[ iQuery ] + 1, GETDATA_FLUSH );
					bResult = verifyEQ( hr, 
										D3DERR_INVALIDCALL, 
										TEXT("GetData (NULL, size mismatch) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
										m_pD3D->HResultToString( hr ) );
				}
			}
			else if ( iCase == 3 )
			{
	            //"GetData (lost device 1)",
	            if ( DRIVERMODEL_LDDM != m_pD3D->GetDriverModel() )
	            {
					hr = pQuery->Issue( ISSUE_END );
					bResult = verifyEQ( hr, 
										S_OK, 
										TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( S_OK ),
										m_pD3D->HResultToString( hr ) );

					bResult = LoseTheDevice();

					hr = m_pSrcDevice->TestCooperativeLevel();
					bResult = verifyEQ( hr, 
										D3DERR_DEVICENOTRESET, 
										TEXT("TestCooperativeLevel: Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( D3DERR_DEVICENOTRESET ),
										m_pD3D->HResultToString( hr ) );


					hr = pQuery->GetData( pData.get(), dwSize[ iQuery ], GETDATA_FLUSH );
					bResult = verifyEQ( hr, 
										synchronous ? S_OK : D3DERR_DEVICELOST, 
										TEXT("GetData (lost device) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( synchronous ? S_OK : D3DERR_DEVICELOST ),
										m_pD3D->HResultToString( hr ) );

					pQuery.release();

					m_pSrcDevice->ReleaseWrappers();

					bResult = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );

					GetLastError();
					bReturn = false;
                }
				else
				{
					Skip();
				}

			}
			else if ( iCase == 4)
			{
	            //"GetData (lost device 2)",
	            if ( DRIVERMODEL_LDDM != m_pD3D->GetDriverModel() )
	            {
					if ( ISSUE_BEGIN & dwValidIssueFlags[iQuery] )
					{
						hr = pQuery->Issue( ISSUE_BEGIN );
						bResult = verifyEQ( hr, 
											S_OK, 
											TEXT("Issue(BEGIN): Expecting (%s) Actual (%s)\n"), 
											m_pD3D->HResultToString( S_OK ),
											m_pD3D->HResultToString( hr ) );
					}

					bResult = LoseTheDevice();

					hr = m_pSrcDevice->TestCooperativeLevel();
					bResult = verifyEQ( hr, 
										D3DERR_DEVICENOTRESET, 
										TEXT("TestCooperativeLevel: Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( D3DERR_DEVICENOTRESET ),
										m_pD3D->HResultToString( hr ) );

					hr = pQuery->Issue( ISSUE_END );
					bResult = verifyEQ( hr, 
										S_OK, 
										TEXT("Issue(END): Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( S_OK ),
										m_pD3D->HResultToString( hr ) );

					hr = pQuery->GetData( pData.get(), dwSize[ iQuery ], GETDATA_FLUSH );
					bResult = verifyEQ( hr, 
										synchronous ? S_OK : D3DERR_DEVICELOST, 
										TEXT("GetData (non-NULL, zero) : Expecting (%s) Actual (%s)\n"), 
										m_pD3D->HResultToString( synchronous ? S_OK : D3DERR_DEVICELOST ),
										m_pD3D->HResultToString( hr ) );

					pQuery.release();

					m_pSrcDevice->ReleaseWrappers();

					bResult = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );

					GetLastError();
					bReturn = false;
                }
				else
				{
					Skip();
				}
			}
			else if ( iCase == 5)
			{
				//"GetData (timeout)",
				HRESULT expectedSyncHR = synchronous ? S_OK : S_FALSE;
				hr = pQuery->Issue( ISSUE_END );
				bResult = verifyEQ( hr, 
									S_OK, 
									TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( S_OK ),
									m_pD3D->HResultToString( hr ) );

				unsigned count;
				hr = GetData( pQuery, pData.get(), dwSize[ iQuery ], 0, &count );
				if (hr == S_OK)
					expectedSyncHR = S_OK;
				bResult = verifyEQ( hr, 
									expectedSyncHR, 
									TEXT("GetData: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( expectedSyncHR ),
									m_pD3D->HResultToString( hr ) );

				bResult = verifyEQ( count, 
									1U,
									TEXT("GetData: Expecting count (%d ) Actual (%d)\n"), 
									count );
			}
			else if ( iCase == 6)
			{
				//"GetData (flush)"
				hr = pQuery->Issue( ISSUE_END );
				bResult = verifyEQ( hr, 
									S_OK, 
									TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( S_OK ),
									m_pD3D->HResultToString( hr ) );

				hr = flush( m_pSrcDevice, true, true, 2 );
				bResult = verifyEQ( hr, 
									S_OK, 
									TEXT("flush: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( S_OK ),
									m_pD3D->HResultToString( hr ) );

				hr = pQuery->GetData( pData.get(), dwSize[ iQuery ], 0 );
				bResult = verifyEQ( hr, 
									S_OK, 
									TEXT("GetData: Expecting (%s) Actual (%s)\n"), 
									m_pD3D->HResultToString( S_OK ),
									m_pD3D->HResultToString( hr ) );
			}
		}
    }
	catch(CBoolException )
	{
		Fail();
		GetLastError();
		WriteToLog( "CQueryGetData: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
		return false;
	}

    return bReturn;
}

//-----------------------------------------------------------------------------

CQueryIssue::CQueryIssue()
{
	m_szTestName = _T("QueryIssue");
    m_szCommandKey = _T("QueryIssue");
}

UINT CQueryIssue::TestInitialize()
{
    SetTestRange(1,22);

    return CQueryConf::TestInitialize();
}

bool CQueryIssue::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

	static QUERYTYPE QueryTypes[] =
	{
		QUERYTYPE_RESOURCEMANAGER,
		QUERYTYPE_VERTEXSTATS,
		QUERYTYPE_EVENT,
        QUERYTYPE_OCCLUSION,
        QUERYTYPE_OCCLUSION,
		QUERYTYPE_VCACHE,
        QUERYTYPE_TIMESTAMP,        
        QUERYTYPE_TIMESTAMPDISJOINT,
        QUERYTYPE_TIMESTAMPDISJOINT,
        QUERYTYPE_TIMESTAMPFREQ,    
        QUERYTYPE_PIPELINETIMINGS,
        QUERYTYPE_PIPELINETIMINGS,  
        QUERYTYPE_INTERFACETIMINGS,
        QUERYTYPE_INTERFACETIMINGS,
        QUERYTYPE_VERTEXTIMINGS,
        QUERYTYPE_VERTEXTIMINGS,
        QUERYTYPE_PIXELTIMINGS,
        QUERYTYPE_PIXELTIMINGS, 
        QUERYTYPE_BANDWIDTHTIMINGS,
        QUERYTYPE_BANDWIDTHTIMINGS, 
        QUERYTYPE_CACHEUTILIZATION,
        QUERYTYPE_CACHEUTILIZATION
	};

    static char *QueryText[] =
	{
		"QUERYTYPE_RESOURCEMANAGER",        
		"QUERYTYPE_VERTEXSTATS",
		"QUERYTYPE_EVENT",
        "QUERYTYPE_OCCLUSION",
        "QUERYTYPE_OCCLUSION",
		"QUERYTYPE_VCACHE",
        "QUERYTYPE_TIMESTAMP",
        "QUERYTYPE_TIMESTAMPDISJOINT",
        "QUERYTYPE_TIMESTAMPDISJOINT",
        "QUERYTYPE_TIMESTAMPFREQ",
        "QUERYTYPE_PIPELINETIMINGS",
        "QUERYTYPE_PIPELINETIMINGS",
        "QUERYTYPE_INTERFACETIMINGS",
        "QUERYTYPE_INTERFACETIMINGS",
        "QUERYTYPE_VERTEXTIMINGS",
        "QUERYTYPE_VERTEXTIMINGS",
        "QUERYTYPE_PIXELTIMINGS",
        "QUERYTYPE_PIXELTIMINGS",
        "QUERYTYPE_BANDWIDTHTIMINGS",
        "QUERYTYPE_BANDWIDTHTIMINGS",
        "QUERYTYPE_CACHEUTILIZATION",
        "QUERYTYPE_CACHEUTILIZATION"
	};

    DWORD dwValidIssueFlags[] =
    {
        ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
        ISSUE_BEGIN,
        ISSUE_END,
    };

    static char *IssueText[] =
	{
		"ISSUE_END",
		"ISSUE_END",
		"ISSUE_BEGIN",
        "ISSUE_BEGIN",
        "ISSUE_END",
		"ISSUE_END",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END",
        "ISSUE_BEGIN",
        "ISSUE_END"
	};

    static unsigned count = 0;

    try
    {
        HRESULT hr;
        int iQuery = nTest - 1;
        MEMORYSTATUS memory_before, memory_after;

        char title[256];
        strcpy(title, QueryText[ iQuery ]);
        strcat(title, ": ");
        strcat(title, IssueText[ iQuery ] );

        BeginTestCase( title, nTest );

	    GlobalMemoryStatus( &memory_before );

        CQuery *pQuery;
        hr = m_pSrcDevice->CreateQuery( QueryTypes[iQuery], &pQuery );
        if ( FAILED( hr ) )
        {
            WriteToLog("CreateQuery failed: %s\n", m_pD3D->HResultToString( hr ) );
            Skip();
        }
        else
        {
            hr = pQuery->Issue( dwValidIssueFlags[iQuery] );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            UINT count1 = getRefCount( pQuery );
            bResult = verifyNE( count1, 
                                UINT(0), 
                                TEXT("CreateQuery: Expecting non-zero reference count\n"));

            bResult = flush(m_pSrcDevice, true, true, 2);

            UINT count2 = pQuery->Release();
            bResult = verifyEQ( count2, 
                                UINT(0), 
                                TEXT("Issue Release: Expecting (%d) Actual (%d)\n"), 
                                0,
                                count2 );

            if (count++)
            {
                // ignore results of first allocation; otherwise they show up as leak
                GlobalMemoryStatus( &memory_after );
/*
                bResult = verifyEQ( memory_after.dwAvailVirtual, 
                                    memory_before.dwAvailVirtual , 
                                    TEXT("Issue Release: Expecting (%d) Actual (%d)\n"), 
                                    memory_before.dwAvailVirtual,
                                    memory_after.dwAvailVirtual );
*/
            }
        }
    }
    catch(CBoolException)
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryIssue: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

CQueryEvent::CQueryEvent()
{
	m_szTestName = _T("QueryEvent");
    m_szCommandKey = _T("QueryEvent");

#if FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
#else //FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
#endif

	m_Options.ModeOptions.ZBufferFmt = FMT_D24S8;
}

bool CQueryEvent::CapsCheck()
{
	HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_EVENT.\n") );
		return false;
    }
	return CQueryConf::CapsCheck();
}

UINT CQueryEvent::TestInitialize()
{
    UINT uResult = CQueryConf::TestInitialize();

    if( uResult == D3DTESTINIT_RUN )
    {
        SetTestRange(1,5);
    }

    return uResult;
}

struct UTLVertex
{
    UTLVertex( float x, float y, float z, float u, float v, DWORD c = 0xffffffff ) : x(x), y(y), z(z), c(c), u(u), v(v) {}
    float x;
    float y;
    float z;
    DWORD c;
    float u;
    float v;
};

bool CQueryEvent::ExecuteTest(UINT nTest)
{
	BOOL bData;
	double dTime;
	unsigned uCount;
	HRESULT hr;
    CBoolException bResult;
	CPerformanceCounter timer;

    BeginTestCase( _T("QueryEvent"), nTest );

    try
    {
        if ( nTest == 3 || nTest == 5 || nTest == 6 || nTest == 7 )
        {
			struct Vertex
			{
				Vertex( float x, float y, float z = 0.0f ) : x(x), y(y), z(z), rhw(1.0f) {}
				float x;
				float y;
				float z;
				float rhw;
			};

			struct TLVertex : public Vertex
			{
				TLVertex( float x, float y, float z, DWORD c, float u, float v ) : Vertex(x, y, z ), c(c), u(u), v(v) {}
				DWORD c;
				float u;
				float v;
			};

			PRESENT_PARAMETERS *m_pParams = m_pSrcDevice->GetPresParams();

			DEVICE_CREATION_PARAMETERS m_createParams;
			bResult = m_pSrcDevice->GetCreationParameters( &m_createParams );

			CAPS caps;
			bResult = m_pD3D->GetDeviceCaps( m_createParams.AdapterOrdinal, m_createParams.DeviceType, &caps );

			UINT width = caps.MaxTextureWidth / 2;
			UINT height = caps.MaxTextureHeight / 2;

			CAutoRelease< CSurface > pTmpRenderTarget, pTmpDepthStencil;

			// create the largest rendertarget and depthstencil buffers possible
			while ( width > 0 && height > 0 )
			{
				if ( SUCCEEDED( m_pSrcDevice->CreateRenderTarget( width,
																   height,
																   m_pParams->BackBufferFormat,
																   m_pParams->MultiSample,
																   m_pParams->MultiSampleQuality,
                                                                   nTest == 3 ? TRUE : FALSE,
																   &pTmpRenderTarget ) ) )
				{
					if ( SUCCEEDED( m_pSrcDevice->CreateDepthStencilSurface( width,
																			  height,
																			  m_pParams->AutoDepthStencilFormat,
																			  m_pParams->MultiSample,
																			  m_pParams->MultiSampleQuality,
																			  FALSE,
																			  &pTmpDepthStencil ) ) )
					{
						break;
					}
					else
					{
						pTmpRenderTarget.release();
					}
				}

				width -= 256;
				height -= 256;
			}

            bResult = width != 0 && height != 0;
            GetLastError();

			CAutoRelease< CSurface > pSrcRenderTarget, pSrcDepthStencil;

			bResult = m_pSrcDevice->GetRenderTarget( 0, &pSrcRenderTarget );

			bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pTmpRenderTarget );

			bResult = m_pSrcDevice->GetDepthStencilSurface( &pSrcDepthStencil );

			bResult = m_pSrcDevice->SetDepthStencilSurface( pTmpDepthStencil );

			D3DXIMAGE_INFO imageInfo;
			CAutoRelease< CTexture > pTexture, pTempTexture;

			// Create a temporary texture from our resource bitmap
			bResult = m_pSrcDevice->D3DXCreateTextureFromResourceEx( NULL,
																MAKEINTRESOURCE(IDB_BITMAP3),
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																(nTest == 7) ? D3DUSAGE_DYNAMIC : 0,
																D3DFMT_A8R8G8B8,
																(nTest == 7) ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM,
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																0,
																&imageInfo,
																NULL,
																&pTempTexture );

			const unsigned long bkColor = 0xff84b59c;   // greenish background color of texture

			// Walk the texture and make the backgroud transparent
			DWORD dwCount = pTempTexture->GetLevelCount();
			for( UINT uLevel = 0; uLevel < dwCount; uLevel++ )
			{
				SURFACEDESC desc;
				bResult = pTempTexture->GetLevelDesc(uLevel, &desc);

				if ( desc.Format.d3dfFormat == FMT_A8R8G8B8 )
				{
					LOCKED_RECT lockRect;

					bResult = pTempTexture->LockRect(uLevel, &lockRect, NULL, 0);

					for( int y = 0; y < desc.Height; y++ )
					{
						unsigned long *pRow = (unsigned long *) ((char *) lockRect.pBits + y * lockRect.iPitch);
						for( int x = 0; x < desc.Width; x++ )
						{
							unsigned long alpha = *pRow == bkColor ? 0x00 : 0xff;

							*pRow++ = (*pRow & 0x00ffffff) | ((alpha & 0xff) << 24);
						}
					}

					bResult = pTempTexture->UnlockRect(uLevel);
				}
			}

			if (nTest == 7)
			{
				// For the dynamic texture test, we can just use the temp texture
				pTexture = pTempTexture;
			}
			else
			{
				SURFACEDESC desc;
				bResult = pTempTexture->GetLevelDesc(0, &desc);

				// Create a video texture with the same size and format as the system texture
				bResult = m_pSrcDevice->CreateTexture(  desc.Width,
														desc.Height,
														pTempTexture->GetLevelCount(),
														0,
														desc.Format.d3dfFormat,
														D3DPOOL_DEFAULT,
														&pTexture );

				// Load the video texture with our altered system memory texture
				m_pSrcDevice->UpdateTexture(pTempTexture, pTexture);
			}

			// define the stencil contents ( a centered oval )
			int nVertices = 32;

			const float phi = 2.0f * 3.14159f / nVertices;

			CAutoRelease< CVertexBuffer > pVertexBuffer;
			bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof( Vertex ),
												USAGE_WRITEONLY,
												D3DFVF_XYZRHW,
												POOL_DEFAULT,
												&pVertexBuffer );

			bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof( Vertex ), 0 );

			bResult = m_pSrcDevice->SetFVF( D3DFVF_XYZRHW );

			Vertex *pV;
			bResult = pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

			float theta = 0.0f;
			for( int i=0; i < nVertices; i++ )
			{
				float x = width * 0.5f * ( sin( theta ) + 1.0f );
				float y = height - height * 0.5f * ( cos( theta ) + 1.0f );
				*pV++ = Vertex( x, y );
				theta += phi;
			}

			bResult = pVertexBuffer->Unlock();

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_STENCIL, 0, 1.0f, 0 );

			bResult = m_pSrcDevice->BeginScene();

			bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

			bResult = m_pSrcDevice->EndScene();

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL  );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR );

			bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, TEXF_LINEAR );

			pVertexBuffer.release();

			// create the vertices for the quad
			const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
			const size_t nSizeVertex = sizeof( TLVertex );
			TLVertex *pVertex;

			nVertices = 4;

			bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * nSizeVertex,
													USAGE_WRITEONLY,
													fvf,
													POOL_DEFAULT,
													&pVertexBuffer );

			bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, nSizeVertex, 0 );

			bResult = m_pSrcDevice->SetFVF( fvf );

			bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

			*pVertex++ = TLVertex( 0.0f, 0.0f, 0.0f, 0xffff0000, 0.0f, 0.0f );
			*pVertex++ = TLVertex( float(width), 0.0f, 0.0f, 0x7f00ff00, 1.0f, 0.0f );
			*pVertex++ = TLVertex( float(width), float(height), 0.0f, 0x5f0000ff, 1.0f, 1.0f );
			*pVertex++ = TLVertex( 0.0f, float(height), 0.0f, 0x00ffffff, 0.0f, 1.0f );

			bResult = pVertexBuffer->Unlock();

			int nStages = min( 2, caps.MaxTextureBlendStages );
			int nTextures = min( 1, caps.MaxSimultaneousTextures );

			// set the texture blending stages
			for( int i=0; i < nStages; i++ )
			{
				if ( i < nStages - nTextures )
				{
					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				}
				else
				{
					bResult = m_pSrcDevice->SetTexture( i, pTexture );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, 0 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_MODULATE2X );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
				}
			}

			DWORD numPasses;
			bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00ffffff, 1.0f, 0 );

            if ( nTest == 3 )
            {
#if FAST_TEST
                const int nTrials = 3;
#else//FAST_TEST
                const int nTrials = 100;
#endif//FAST_TEST
				std::vector< double > timings( nTrials );

                CAutoRelease< CQuery > pQuery1;
                bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery1 );
                int i;
                for( i=0; i < nTrials; i++ )
                {
                    bResult = pQuery1->Issue( ISSUE_END );
                    bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount );
                    bResult = m_pSrcDevice->BeginScene();
                    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );
					bResult = m_pSrcDevice->EndScene();
                    timer.start();
                    bResult = flush( m_pSrcDevice, false, false );
                    timings[i] = timer.finish();
                }

				CStats< double > stats1( timings, 10 );

                for( i=0; i < nTrials; i++ )
                {
                    bResult = pQuery1->Issue( ISSUE_END );
                    bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount );
                    bResult = m_pSrcDevice->BeginScene();
                    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );
					bResult = m_pSrcDevice->EndScene();
					bResult = pQuery1->Issue( ISSUE_END );
                    bResult = pQuery1->GetData( &bData, sizeof( bData ), GETDATA_FLUSH );	// single flush
                    timer.start();
                    bResult = flush( m_pSrcDevice, false, false );
                    timings[i] = timer.finish();
                }

				CStats< double > stats2( timings, 10 );

                for( i=0; i < nTrials; i++ )
                {
                    bResult = pQuery1->Issue( ISSUE_END );
                    bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount );
                    bResult = m_pSrcDevice->BeginScene();
                    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );
					bResult = m_pSrcDevice->EndScene();
                    bResult = pQuery1->Issue( ISSUE_END );
                    bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount );	//looping flush
                    timer.start();
                    bResult = flush( m_pSrcDevice, false, false );
                    timings[i] = timer.finish();
                }

				CStats< double > stats3( timings, 10 );

				bResult = m_pSrcDevice->SetDepthStencilSurface( pSrcDepthStencil );

				bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pSrcRenderTarget );

				bResult = verifyLT( stats3.m_ExpVal, 
					                stats1.m_ExpVal,
									TEXT("Issue: Expecting Time3 < Time1 (%f < %f)\n"),
									stats3.m_ExpVal, 
									stats1.m_ExpVal );

				double dMid = ( stats3.m_Mean + stats1.m_Mean ) * 0.5;

                bResult = verifyGT( stats2.m_Mean,
					                dMid - 2.0 * stats1.m_StdDev, 
                                    TEXT("Issue: Expecting Time2 > Mid-2*SD1 (%f > %f)\n"),
									stats2.m_Mean,
                                    dMid - 2.0 * stats1.m_StdDev );

                bResult = verifyLT( stats2.m_Mean, 
                                    stats1.m_Mean + 3.0 * stats1.m_StdDev, 
                                    TEXT("Issue: Expecting Time2 < Time1+3*SD1 (%f < %f)\n"), 
									stats2.m_Mean, 
									stats1.m_Mean + 3.0 * stats1.m_StdDev );
            }
            else if ( nTest == 5 )
            {
			    CAutoRelease< CQuery > m_pEventQuery;
			    bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &m_pEventQuery );

			    bResult = m_pSrcDevice->BeginScene();

			    bResult = m_pEventQuery->Issue( ISSUE_END );

			    bResult = GetData( m_pEventQuery, zeroMemory( &bData ), sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );

			    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

			    bResult = m_pEventQuery->Issue( ISSUE_END );

			    hr = GetData( m_pEventQuery, zeroMemory( &bData ), sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );

				bResult = m_pSrcDevice->SetDepthStencilSurface( pSrcDepthStencil );

				bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pSrcRenderTarget );

			    bResult = m_pSrcDevice->EndScene();

			    bResult = verifyNE( uCount == 1 || hr == S_FALSE, 
								    true, 
								    TEXT("GetData: Expecting uCount (>1) and bResult = (S_OK) Actual (%d) (%s)\n"), 
								    uCount,
								    m_pD3D->HResultToString( hr ) );
            }
            else if ( nTest == 6 )
            {
                double dTime1 = 0.0, dTime2 = 0.0;

			    bResult = m_pSrcDevice->BeginScene();

			    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

                bResult = m_pSrcDevice->EndScene();

                timer.start();
			    bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
                dTime1 = timer.finish();

			    bResult = pVertexBuffer->Unlock();

                timer.start();
			    bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
                dTime2 = timer.finish();

			    bResult = pVertexBuffer->Unlock();

				bResult = m_pSrcDevice->SetDepthStencilSurface( pSrcDepthStencil );

				bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pSrcRenderTarget );

                bResult = verifyEQ( dTime2 < dTime1, 
                                    true, 
                                    TEXT("Issue: Expecting (%f < %f)\n"), 
                                    dTime2, dTime1 );
            }
            else if ( nTest == 7 )
            {
                double dTime1 = 0.0, dTime2 = 0.0;

			    bResult = m_pSrcDevice->BeginScene();

			    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

                bResult = m_pSrcDevice->EndScene();

                LOCKED_RECT LockedRect;

                timer.start();
			    bResult = pTexture->LockRect(0, &LockedRect, NULL, 0 );
                dTime1 = timer.finish();

			    bResult = pTexture->UnlockRect(0);

                timer.start();
			    bResult = pTexture->LockRect(0, &LockedRect, NULL, 0 );
                dTime2 = timer.finish();

			    bResult = pTexture->UnlockRect(0);

				bResult = m_pSrcDevice->SetDepthStencilSurface( pSrcDepthStencil );

				bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pSrcRenderTarget );

                bResult = verifyEQ( dTime2 < dTime1, 
                                    true, 
                                    TEXT("Issue: Expecting (%f < %f)\n"), 
                                    dTime2, dTime1 );
            }

			bResult = m_pSrcDevice->StretchRect( pTmpRenderTarget, NULL, pSrcRenderTarget, NULL, D3DTEXF_NONE );

			bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
		}
		else if ( nTest == 4 )
        {
            static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

            const nK = 5;	// overlap count
            const nU = 1;	// number divisions in u
            const nV = 1;	// number divisions in v
            const float uStep = 2.0f / nU;
            const float vStep = 2.0f / nV;
            const float uTexStep = 1.0f / nU;
            const float vTexStep = 1.0f / nV;
            const int nVertices = 2 * nV * (nU + 1) * nK;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

            std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

            CAutoRelease< CTexture > pTexture1;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

            CAutoRelease< CTexture > pTexture2;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

            CAutoRelease< CQuery > pQuery1;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery1 );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            UTLVertex *pVertex;
            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            int iColor = 0;
            for( int k=0; k < nK; k++)
            {
                float y = 1.0f;
                float vTex = 0.0f;
                for( int j=0; j <= nV; j++ )
                {
                    pC->clear();

                    float x = -1.0f;
                    float uTex = 0.0f;
                    for( int i=0; i <= nU; i++ )
                    {
                        pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
                        x += uStep;
                        uTex += uTexStep;

                        if ( j )
                        {
                            *pVertex = pC->at( i );
                            pVertex++->c = colors[iColor++ % 3];

                            *pVertex = pL->at( i );
                            pVertex++->c = colors[iColor++ % 3];
                        }
                    }

                    y -= vStep;
                    vTex += vTexStep;
                    std::swap( pL, pC );
                }
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

            bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

			bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

			bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

			bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

			bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

			bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

			bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

			bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

			bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

            //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

			DWORD numPasses;
			bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery1->Issue( ISSUE_END );

            bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

            bResult = pQuery1->Issue( ISSUE_END );

            hr = GetData( pQuery1, zeroMemory( &bData ), sizeof( bData ), GETDATA_FLUSH, &uCount );

            bResult = m_pSrcDevice->EndScene();

            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("GetData: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            bResult = verifyGT( uCount, 
                                (unsigned)1, 
                                TEXT("GetData: Expecting uCount (>1) Actual (%d)\n"), 
                                uCount );

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

			bResult = m_pSrcDevice->BeginScene();

			bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

			bResult = m_pSrcDevice->EndScene();

            bResult = pQuery1->Issue( ISSUE_END );

			bResult = flush( m_pSrcDevice, true, true, 2 );

            hr = pQuery1->GetData( zeroMemory( &bData ), sizeof( bData ), 0 );

            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("GetData: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

			bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
        }
        else
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, float u, float v,
                        float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), u(u), v(v) {}
                float x;
                float y;
                float z;
                float nx;
                float ny;
                float nz;
                float u;
                float v;
            } *pVertex;

            const int nVertices = 4;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

            CAutoRelease< CQuery > pQuery1, pQuery2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery2 );

            CAutoRelease< CTexture > pTexture;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( -1.0f, 1.0f, 0.0f, 0.0f, 1.0f );
            *pVertex++ = Vertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f   );
            *pVertex++ = Vertex( 1.0f, 0.0f, 0.0f, 1.0f, 0.0f  );
            *pVertex++ = Vertex( -1.0f, -1.0f, 0.0f, 0.0f, 0.0f  );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            MATERIAL mtrl;
            zeroMemory( &mtrl  );
            mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
            mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
            mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
            bResult = m_pSrcDevice->SetMaterial( &mtrl );

            LIGHT light;
            zeroMemory( &light );
            light.Type = D3DLIGHT_DIRECTIONAL;
            light.Diffuse.r = 1.0f;
            light.Diffuse.g = 1.0f;
            light.Diffuse.b = 1.0f;
            light.Range = 1.0f;
            light.Direction.x = 0.0f;
            light.Direction.y = 0.0f;
            light.Direction.z = 1.0f;
            bResult = m_pSrcDevice->SetLight( 0, &light );

            bResult = m_pSrcDevice->LightEnable( 0, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture );

            bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            if ( nTest == 1 )
            {
                bResult = pQuery1->Issue( ISSUE_END );

                bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );
            }

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

            if ( nTest == 1 )
            {
                bResult = pQuery2->Issue( ISSUE_END );

                bResult = GetData( pQuery2, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );

				bResult = m_pSrcDevice->EndScene();

                bResult = verifyEQ( dTime < 0.1, 
                                    true, 
                                    TEXT("Issue: Expecting (%f < %f)\n"), 
                                    dTime, 0.1 );
            }

            if ( nTest == 2 )
            {
				bResult = m_pSrcDevice->EndScene();

                bResult = pQuery1->Issue( ISSUE_END );

                bResult = GetData( pQuery1, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );
            }

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            if ( nTest == 2 )
            {
                bResult = pQuery2->Issue( ISSUE_END );

                bResult = GetData( pQuery2, &bData, sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );

                bResult = verifyEQ( dTime < 0.1, 
                                    true, 
                                    TEXT("Issue: Expecting (%f < %f)\n"), 
                                    dTime, 0.1 );
            }
        }
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
		WriteToLog( "CQueryEvent: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------

CQueryOcclusion::CQueryOcclusion()
{
	m_Options.D3DOptions.bZBuffer = true;
	//m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
#if FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
#else //FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
#endif

	m_Options.ModeOptions.ZBufferFmt = FMT_D24S8;

	m_szTestName = _T("QueryOcclusion");
    m_szCommandKey = _T("QueryOcclusion");
}

bool CQueryOcclusion::CapsCheck()
{
	HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_OCCLUSION.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_EVENT.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryOcclusion::TestInitialize()
{
    UINT uResult = CQueryConf::TestInitialize();

    if( uResult == D3DTESTINIT_RUN )
    {
        SetTestRange(1,13);
    }

    return uResult;
}

bool CQueryOcclusion::ExecuteTest(UINT nTest)
{
	HRESULT hr;
    CBoolException bResult;

    BeginTestCase( _T("QueryOcclusion"), nTest );

    try
    {
        if ( nTest == 13 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z = 0.0f) : x(x), y(y), z(z), rhw(1.0f) {}
                float x;
                float y;
                float z;
                float rhw;
            } *pVertex;

            const int nStencilVertices = 3;
            const DWORD fvf = D3DFVF_XYZRHW;

			// define the stencil mask
			CAutoRelease< CVertexBuffer > pStencilVertexBuffer;
			bResult = m_pSrcDevice->CreateVertexBuffer( nStencilVertices * sizeof( Vertex ),
												        USAGE_WRITEONLY,
												        fvf,
												        D3DPOOL_DEFAULT,
												        &pStencilVertexBuffer );

			bResult = pStencilVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( 40.0f, 15.0f );
            *pVertex++ = Vertex( 50.0f, 10.0f );
            *pVertex++ = Vertex( 25.0f, 60.0f );

			bResult = pStencilVertexBuffer->Unlock();

            struct Vertex2 : public Vertex
            {
                Vertex2( float x, float y, float z, unsigned c) : Vertex( x, y, z ), c(c) {}
                unsigned c;
            } *pVertex2;

            const int nVertices = 15;
            const DWORD fvf2 = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

            // define the graphic content
            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex2),
                                                        USAGE_WRITEONLY,
                                                        fvf2,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex2, 0);

            *pVertex2++ = Vertex2( 10.0f, 10.0f, 0.5f, 0xffff0000 );
            *pVertex2++ = Vertex2( 100.0f, 10.0f, 0.5f, 0xff00ff00 );
            *pVertex2++ = Vertex2( 10.0f, 100.0f, 0.5f, 0xff0000ff );

            *pVertex2++ = Vertex2( 40.0f, 15.0f, 0.6f, 0xffff0000 );
            *pVertex2++ = Vertex2( 50.0f, 10.0f, 0.6f, 0xffff0000 );
            *pVertex2++ = Vertex2( 25.0f, 60.0f, 0.6f, 0xffff0000 );

            *pVertex2++ = Vertex2( 40.0f, 15.0f, 0.4f, 0xffff0000 );
            *pVertex2++ = Vertex2( 50.0f, 10.0f, 0.4f, 0xffff0000 );
            *pVertex2++ = Vertex2( 25.0f, 60.0f, 0.4f, 0xffff0000 );

            *pVertex2++ = Vertex2( 25.0f, 15.0f, 0.6f, 0xff00ff00 );
            *pVertex2++ = Vertex2( 35.0f, 10.0f, 0.6f, 0xff00ff00 );
            *pVertex2++ = Vertex2( 10.0f, 60.0f, 0.6f, 0xff00ff00 );

            *pVertex2++ = Vertex2( 25.0f, 15.0f, 0.4f, 0xff00ff00 );
            *pVertex2++ = Vertex2( 35.0f, 10.0f, 0.4f, 0xff00ff00 );
            *pVertex2++ = Vertex2( 10.0f, 60.0f, 0.4f, 0xff00ff00 );

            bResult = pVertexBuffer->Unlock();

            CAutoRelease< CQuery > pQuery1, pQuery2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery2 );

            // make sure normal cases work
            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex2) );

            bResult = m_pSrcDevice->SetFVF( fvf2 );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

            bResult = pQuery1->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 3, 1 );

			bResult = pQuery1->Issue( ISSUE_END );

            bResult = pQuery2->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 6, 1 );

			bResult = pQuery2->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            // No stencil - Z-fail
            DWORD dwCount;
            bResult = GetData( pQuery1, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = verifyEQ( dwCount, 
                                (DWORD) 0, 
                                TEXT("GetData: Expecting occlusion count = (%d) Actual (%d)\n"),
                                (DWORD) 0,
                                dwCount );

            // No stencil - Z-pass
            bResult = GetData( pQuery2, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex2, 0);
            float triangleArea = areaTriangle( pVertex2 + 6, sizeof(Vertex2) );
            bResult = pVertexBuffer->Unlock();

            bResult = verifyIN( (float) dwCount, 
                                triangleArea * LowBoundPct, 
                                triangleArea * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                triangleArea * LowBoundPct,
                                triangleArea * HiBoundPct,
                                dwCount );


            // render stencil buffer mask (area not to draw in)
			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->SetStreamSource( 0, pStencilVertexBuffer, sizeof( Vertex ), 0 );

			bResult = m_pSrcDevice->SetFVF( fvf );

			bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_STENCIL, 0, 1.0f, 0 );

			bResult = m_pSrcDevice->BeginScene();

			bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nStencilVertices / 3 );

			bResult = m_pSrcDevice->EndScene();

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL  );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );


            // verify stencil mask does affect occlusion results
            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex2) );

            bResult = m_pSrcDevice->SetFVF( fvf2 );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

            bResult = pQuery1->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 3, 1 );

			bResult = pQuery1->Issue( ISSUE_END );

            bResult = pQuery2->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 6, 1 );

			bResult = pQuery2->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            // Stencil-fail, Z-fail
            bResult = GetData( pQuery1, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = verifyEQ( dwCount, 
                                (DWORD) 0, 
                                TEXT("GetData: Expecting occlusion count = (%d) Actual (%d)\n"),
                                (DWORD) 0,
                                dwCount );

            // Stencil-fail, Z-pass
            bResult = GetData( pQuery2, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = verifyEQ( dwCount, 
                                (DWORD) 0, 
                                TEXT("GetData: Expecting occlusion count = (%d) Actual (%d)\n"),
                                (DWORD) 0,
                                dwCount );

            // do passing stencil case
            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery1->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 9, 1 );

			bResult = pQuery1->Issue( ISSUE_END );

            bResult = pQuery2->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 12, 1 );

			bResult = pQuery2->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            // Stencil-pass, Z-fail
            bResult = GetData( pQuery1, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = verifyEQ( dwCount, 
                                (DWORD) 0, 
                                TEXT("GetData: Expecting occlusion count = (%d) Actual (%d)\n"),
                                (DWORD) 0,
                                dwCount );

            // Stencil-pass, Z-pass
            bResult = GetData( pQuery2, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex2, 0);
            triangleArea = areaTriangle( pVertex2 + 12, sizeof(Vertex2) );
            bResult = pVertexBuffer->Unlock();

            bResult = verifyIN( (float) dwCount, 
                                triangleArea * LowBoundPct, 
                                triangleArea * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                triangleArea * LowBoundPct,
                                triangleArea * HiBoundPct,
                                dwCount );
        }
        else if ( nTest == 12 )
        {
			struct Vertex
			{
				Vertex( float x, float y, float z = 0.0f ) : x(x), y(y), z(z), rhw(1.0f) {}
				float x;
				float y;
				float z;
				float rhw;
			};

			struct TLVertex : public Vertex
			{
				TLVertex( float x, float y, float z, DWORD c, float u, float v ) : Vertex(x, y, z ), c(c), u(u), v(v) {}
				DWORD c;
				float u;
				float v;
			};

			PRESENT_PARAMETERS *m_pParams = m_pSrcDevice->GetPresParams();

			DEVICE_CREATION_PARAMETERS m_createParams;
			bResult = m_pSrcDevice->GetCreationParameters( &m_createParams );

			CAPS caps;
			bResult = m_pD3D->GetDeviceCaps( m_createParams.AdapterOrdinal, m_createParams.DeviceType, &caps );

			UINT width = caps.MaxTextureWidth / 2;
			UINT height = caps.MaxTextureHeight / 2;

			CAutoRelease< CSurface > pTmpRenderTarget, pTmpDepthStencil;

			// create the largest rendertarget and depthstencil buffers possible
			while ( width > 0 && height > 0 )
			{
				if ( SUCCEEDED( m_pSrcDevice->CreateRenderTarget( width,
																  height,
																  m_pParams->BackBufferFormat,
																  m_pParams->MultiSample,
																  m_pParams->MultiSampleQuality,
                                                                  nTest == 3 ? TRUE : FALSE,
																  &pTmpRenderTarget ) ) )
				{
					if ( SUCCEEDED( m_pSrcDevice->CreateDepthStencilSurface( width,
																			 height,
																			 m_pParams->AutoDepthStencilFormat,
																			 m_pParams->MultiSample,
																			 m_pParams->MultiSampleQuality,
																			 FALSE,
																			 &pTmpDepthStencil ) ) )
					{
						break;
					}
					else
					{
						pTmpRenderTarget.release();
					}
				}

				width -= 256;
				height -= 256;
			}

            bResult = width != 0 && height != 0;
            GetLastError();

			CAutoRelease< CSurface > pSrcRenderTarget, pSrcDepthStencil;

			bResult = m_pSrcDevice->GetRenderTarget( 0, &pSrcRenderTarget );

			bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pTmpRenderTarget );

			bResult = m_pSrcDevice->GetDepthStencilSurface( &pSrcDepthStencil );

			bResult = m_pSrcDevice->SetDepthStencilSurface( pTmpDepthStencil );

			D3DXIMAGE_INFO imageInfo;
			CAutoRelease< CTexture > pSysTexture, pTexture;

			// Create a system memory texture from our resource bitmap
			bResult = m_pSrcDevice->D3DXCreateTextureFromResourceEx( NULL,
																MAKEINTRESOURCE(IDB_BITMAP3),
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																0,
																D3DFMT_A8R8G8B8,
																D3DPOOL_SYSTEMMEM,
																D3DX_DEFAULT,
																D3DX_DEFAULT,
																0,
																&imageInfo,
																NULL,
																&pSysTexture );

			const unsigned long bkColor = 0xff84b59c;   // greenish background color of texture

			// Walk the texture and make the backgroud transparent
			DWORD dwCount = pSysTexture->GetLevelCount();
			for( UINT uLevel = 0; uLevel < dwCount; uLevel++ )
			{
				SURFACEDESC desc;
				bResult = pSysTexture->GetLevelDesc(uLevel, &desc);

				if ( desc.Format.d3dfFormat == FMT_A8R8G8B8 )
				{
					LOCKED_RECT lockRect;

					bResult = pSysTexture->LockRect(uLevel, &lockRect, NULL, 0);

					for( int y = 0; y < desc.Height; y++ )
					{
						unsigned long *pRow = (unsigned long *) ((char *) lockRect.pBits + y * lockRect.iPitch);
						for( int x = 0; x < desc.Width; x++ )
						{
							unsigned long alpha = *pRow == bkColor ? 0x00 : 0xff;

							*pRow++ = (*pRow & 0x00ffffff) | ((alpha & 0xff) << 24);
						}
					}

					bResult = pSysTexture->UnlockRect(uLevel);
				}
			}

			SURFACEDESC desc;
			bResult = pSysTexture->GetLevelDesc(0, &desc);

			// Create a video texture with the same size and format as the system texture
			bResult = m_pSrcDevice->CreateTexture(  desc.Width,
													desc.Height,
													pSysTexture->GetLevelCount(),
													0,
													desc.Format.d3dfFormat,
													D3DPOOL_DEFAULT,
													&pTexture );

			// Load the video texture with our altered system memory texture
			m_pSrcDevice->UpdateTexture(pSysTexture, pTexture);

			// define the stencil contents ( a centered oval )
			int nVertices = 32;

			const float phi = 2.0f * 3.14159f / nVertices;

			CAutoRelease< CVertexBuffer > pVertexBuffer;
			bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof( Vertex ),
												USAGE_WRITEONLY,
												D3DFVF_XYZRHW,
												POOL_DEFAULT,
												&pVertexBuffer );

			bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof( Vertex ), 0 );

			bResult = m_pSrcDevice->SetFVF( D3DFVF_XYZRHW );

			Vertex *pV;
			bResult = pVertexBuffer->Lock(0, 0, (void**)&pV, 0);

			float theta = 0.0f;
			for( int i=0; i < nVertices; i++ )
			{
				float x = width * 0.5f * ( sin( theta ) + 1.0f );
				float y = height - height * 0.5f * ( cos( theta ) + 1.0f );
				*pV++ = Vertex( x, y );
				theta += phi;
			}

			bResult = pVertexBuffer->Unlock();

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_REPLACE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_STENCIL, 0, 1.0f, 0 );

			bResult = m_pSrcDevice->BeginScene();

			bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

			bResult = m_pSrcDevice->EndScene();

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILREF, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILMASK, 0x00000001 );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_KEEP );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL  );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

			bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR );

			bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, TEXF_LINEAR );

			pVertexBuffer.release();

			// create the vertices for the quad
			const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
			const size_t nSizeVertex = sizeof( TLVertex );
			TLVertex *pVertex;

			nVertices = 4;

			bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * nSizeVertex,
													USAGE_WRITEONLY,
													fvf,
													POOL_DEFAULT,
													&pVertexBuffer );

			bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, nSizeVertex, 0 );

			bResult = m_pSrcDevice->SetFVF( fvf );

			bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

			*pVertex++ = TLVertex( 0.0f, 0.0f, 0.0f, 0xffff0000, 0.0f, 0.0f );
			*pVertex++ = TLVertex( float(width), 0.0f, 0.0f, 0x7f00ff00, 1.0f, 0.0f );
			*pVertex++ = TLVertex( float(width), float(height), 0.0f, 0x5f0000ff, 1.0f, 1.0f );
			*pVertex++ = TLVertex( 0.0f, float(height), 0.0f, 0x00ffffff, 0.0f, 1.0f );

			bResult = pVertexBuffer->Unlock();

			int nStages = min( 2, caps.MaxTextureBlendStages );
			int nTextures = min( 1, caps.MaxSimultaneousTextures );

			// set the texture blending stages
			for( int i=0; i < nStages; i++ )
			{
				if ( i < nStages - nTextures )
				{
					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
				}
				else
				{
					bResult = m_pSrcDevice->SetTexture( i, pTexture );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, 0 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_MODULATE2X );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
				}
			}

            unsigned uCount;
            double dTime;
			DWORD numPasses;
			bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00ffffff, 1.0f, 0 );

			CAutoRelease< CQuery > pEventQuery, pOcclusionQuery;
			bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pEventQuery );

			bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pOcclusionQuery );

			bResult = pEventQuery->Issue( ISSUE_END );

            BOOL bData;
			bResult = GetData( pEventQuery, zeroMemory( &bData ), sizeof( bData ), GETDATA_FLUSH, &uCount, &dTime );

			bResult = m_pSrcDevice->BeginScene();

            bResult = pOcclusionQuery->Issue( ISSUE_BEGIN );

			bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

			bResult = pOcclusionQuery->Issue( ISSUE_END );

            DWORD dwData;
			hr = GetData( pOcclusionQuery, zeroMemory( &dwData ), sizeof( dwData ), GETDATA_FLUSH, &uCount, &dTime );

			bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->StretchRect( pTmpRenderTarget, NULL, pSrcRenderTarget, NULL, D3DTEXF_NONE );

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

			bResult = m_pSrcDevice->SetDepthStencilSurface( pSrcDepthStencil );

			bResult = m_pSrcDevice->SetRenderTarget( (DWORD)0, pSrcRenderTarget );

			bResult = verifyNE( uCount == 1 || hr == S_FALSE, 
								true, 
								TEXT("GetData: Expecting uCount (>1) and bResult = (S_OK) Actual (%d) (%s)\n"), 
								uCount,
								m_pD3D->HResultToString( hr ) );
		}
        else if ( nTest == 11 )
        {
            CAutoRelease< CQuery > pQuery1, pQuery2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery2 );

            hr = pQuery1->Issue( ISSUE_BEGIN );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            hr = pQuery1->Issue( ISSUE_END );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            hr = pQuery2->Issue( ISSUE_BEGIN );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            DWORD dwCount;
            hr = GetData( pQuery1, &dwCount, sizeof( DWORD ), GETDATA_FLUSH );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            hr = pQuery2->Issue( ISSUE_END );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );
        }
        else if ( nTest == 10 && m_bIsDebug )
        {
            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery );

            hr = pQuery->Issue( ISSUE_BEGIN );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            DWORD dwData;
            hr = pQuery->GetData( &dwData, sizeof(DWORD), GETDATA_FLUSH );
            bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("GetData (NULL, zero) : Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                m_pD3D->HResultToString( hr ) );
        }
        else if ( nTest == 9 && m_bIsDebug )
        {
            CAutoRelease< CQuery > pQuery1, pQuery2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery2 );

            hr = pQuery1->Issue( ISSUE_BEGIN );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            hr = pQuery2->Issue( ISSUE_BEGIN );
            bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                m_pD3D->HResultToString( hr ) );
        }
        else if ( nTest == 8 && m_bIsDebug )
        {
            CAutoRelease< CQuery > pQuery1, pQuery2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery2 );

            hr = pQuery1->Issue( ISSUE_BEGIN );
            bResult = verifyEQ( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( S_OK ),
                                m_pD3D->HResultToString( hr ) );

            hr = pQuery2->Issue( ISSUE_END );
            bResult = verifyNE( hr, 
                                S_OK, 
                                TEXT("Issue: Expecting (%s) Actual (%s)\n"), 
                                m_pD3D->HResultToString( D3DERR_INVALIDCALL ),
                                m_pD3D->HResultToString( hr ) );
        }
        else if ( nTest == 7 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, unsigned c) : x(x), y(y), z(z), rhw(1.0f / z), c(c) {}
                float x;
                float y;
                float z;
                float rhw;
                unsigned c;
            } *pVertex;

            const int nVertices = 6;
            const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( 10.0f, 10.0f, 0.5f, 0xffff0000 );
            *pVertex++ = Vertex( 50.0f, 10.0f, 0.5f, 0xff00ff00 );
            *pVertex++ = Vertex( 25.0f, 60.0f, 0.5f, 0xff0000ff );
            *pVertex++ = Vertex( 10.0f, 10.0f, 0.4f, 0xffff0000 );
            *pVertex++ = Vertex( 70.0f, 20.0f, 0.4f, 0xff00ff00 );
            *pVertex++ = Vertex( 5.0f, 80.0f, 0.4f, 0xff0000ff );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 3, 1 );

            bResult = pQuery->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            DWORD dwCount;
            bResult = GetData( pQuery, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
            float triangleArea = areaTriangle( pVertex, sizeof(Vertex), 3 );
            bResult = pVertexBuffer->Unlock();

            bResult = verifyIN( (float) dwCount, 
                                triangleArea * LowBoundPct, 
                                triangleArea * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                triangleArea * LowBoundPct,
                                triangleArea * HiBoundPct,
                                dwCount );
        }
        else if ( nTest == 6 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, unsigned c) : x(x), y(y), z(z), rhw(1.0f / z), c(c) {}
                float x;
                float y;
                float z;
                float rhw;
                unsigned c;
            } *pVertex;

            const int nVertices = 6;
            const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( 10.0f, 10.0f, 0.4f, 0xffff0000 );
            *pVertex++ = Vertex( 50.0f, 10.0f, 0.4f, 0xff00ff00 );
            *pVertex++ = Vertex( 25.0f, 60.0f, 0.4f, 0xff0000ff );
            *pVertex++ = Vertex( 10.0f, 10.0f, 0.5f, 0xffff0000 );
            *pVertex++ = Vertex( 70.0f, 20.0f, 0.5f, 0xff00ff00 );
            *pVertex++ = Vertex( 5.0f, 80.0f, 0.5f, 0xff0000ff );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 3, 1 );

            bResult = pQuery->Issue( ISSUE_END );

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

            bResult = pQuery->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            DWORD dwCount;
            bResult = GetData( pQuery, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
            float triangleArea = areaTriangle( pVertex, sizeof(Vertex), 0 );
            bResult = pVertexBuffer->Unlock();

            bResult = verifyIN( (float) dwCount, 
                                triangleArea * LowBoundPct, 
                                triangleArea * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                triangleArea * LowBoundPct,
                                triangleArea * HiBoundPct,
                                dwCount );
        }
        else if ( nTest == 5 || nTest == 4 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z = 0.0f, 
                        float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}
                float x;
                float y;
                float z;
                float nx;
                float ny;
                float nz;
            } *pVertex;

            const int nVertices = 4;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL;
            const float fWidth = float(m_pCurrentMode->nBackBufferWidth);
            const float fHeight = float(m_pCurrentMode->nBackBufferHeight);

            CAutoRelease< CQuery > pQuery1, pQuery2, pQuery3, pQuery4, pQuery5;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery1 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery2 );
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery3 );

            if ( nTest == 5 )
            {
                HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery4 );
                if ( FAILED( hr ) )
                {
                    WriteToLog("CreateQuery(QUERYTYPE_EVENT) failed: %s\n", m_pD3D->HResultToString( hr ) );
                    Skip();
                    GetLastError();
                    return false;
                }
                bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, &pQuery5 );
            }

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( 0.0f, 0.0f );
            *pVertex++ = Vertex( 0.4f, 0.0f );
            *pVertex++ = Vertex( 0.4f, -0.4f );
            *pVertex++ = Vertex( 0.0f, -0.8f );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );
            bResult = m_pSrcDevice->SetFVF( fvf );

            MATERIAL mtrl;
            zeroMemory( &mtrl  );
            mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
            mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
            mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
            bResult = m_pSrcDevice->SetMaterial( &mtrl );

            LIGHT light;
            zeroMemory( &light );
            light.Type = D3DLIGHT_DIRECTIONAL;
            light.Diffuse.r = 1.0f;
            light.Diffuse.g = 1.0f;
            light.Diffuse.b = 1.0f;
            light.Range = 1.0f;
            light.Direction.x = 0.0f;
            light.Direction.y = 0.0f;
            light.Direction.z = 1.0f;
            bResult = m_pSrcDevice->SetLight( 0, &light );

            bResult = m_pSrcDevice->LightEnable( 0, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            D3DXMATRIX matrix1, matrix2, matrix3;

            hr = m_pSrcDevice->SetTransform(D3DTS_WORLD, D3DXMatrixTranslation( &matrix1, -1.0f, 1.0f, 0.0f ));

            bResult = pQuery1->Issue( ISSUE_BEGIN );

            BOOL bBool;
			unsigned uCount;
            double dTime1, dTime2, dTime3;

            if ( nTest == 5 )
            {
                bResult = pQuery4->Issue( ISSUE_END );

                bResult = GetData( pQuery4, &bBool, sizeof( BOOL), GETDATA_FLUSH );
            }

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

            if ( nTest == 5 )
            {
                bResult = pQuery5->Issue( ISSUE_END );

                bResult = GetData( pQuery5, &bBool, sizeof( BOOL), GETDATA_FLUSH, &uCount, &dTime1 );
            }

            bResult = pQuery1->Issue( ISSUE_END );

            hr = m_pSrcDevice->SetTransform(D3DTS_WORLD, D3DXMatrixMultiply( &matrix3,
                                                                             D3DXMatrixScaling( &matrix1, 2.0f, 2.0f, 1.0f ), 
                                                                             D3DXMatrixTranslation( &matrix2, 0.0f, 1.0f, 0.0f ) ) );

            bResult = pQuery2->Issue( ISSUE_BEGIN );

            if ( nTest == 5 )
            {
                bResult = pQuery4->Issue( ISSUE_END );

                bResult = GetData( pQuery4, &bBool, sizeof( BOOL), GETDATA_FLUSH );
            }

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

            if ( nTest == 5 )
            {
                bResult = pQuery5->Issue( ISSUE_END );

                bResult = GetData( pQuery5, &bBool, sizeof( BOOL), GETDATA_FLUSH, &uCount, &dTime2 );
            }

            bResult = pQuery2->Issue( ISSUE_END );

            hr = m_pSrcDevice->SetTransform(D3DTS_WORLD, D3DXMatrixMultiply( &matrix3,
                                                                             D3DXMatrixScaling( &matrix1, 0.5f, 0.5f, 1.0f ), 
                                                                             D3DXMatrixTranslation( &matrix2, -1.0f, 0.0f, 0.0f ) ) );

            bResult = pQuery3->Issue( ISSUE_BEGIN );

            if ( nTest == 5 )
            {
                bResult = pQuery4->Issue( ISSUE_END );

                bResult = GetData( pQuery4, &bBool, sizeof( BOOL), GETDATA_FLUSH );
            }

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

            if ( nTest == 5 )
            {
                bResult = pQuery5->Issue( ISSUE_END );

                bResult = GetData( pQuery5, &bBool, sizeof( BOOL), GETDATA_FLUSH, &uCount, &dTime3 );
            }

            bResult = pQuery3->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            // compute projected area of trapazoids (in pixels)
            float area1 = projectedArea( areaTrapazoid( 0.4f, 0.8f, 0.4f ), fWidth, fHeight );
            float area2 = projectedArea( areaTrapazoid( 0.8f, 1.6f, 0.8f ), fWidth, fHeight );
            float area3 = projectedArea( areaTrapazoid( 0.2f, 0.4f, 0.2f ), fWidth, fHeight );

            DWORD dwCount;
            unsigned dwLoop;

            bResult = GetData( pQuery3, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = verifyIN( (float) dwCount,    // half-size => 3% tolerance
                                area3 * 0.97f, 
                                area3 * 1.03f,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                area3 * 0.97f,
                                area3 * 1.03f,
                                dwCount );

            bResult = GetData( pQuery2, &dwCount, sizeof( dwCount), 0, &dwLoop );

            bResult = verifyIN( (float) dwCount,    // double-size => .75% tolerance
                                area2 * 0.9925f, 
                                area2 * 1.0075f,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                area2 * 0.9925f,
                                area2 * 1.0075f,
                                dwCount );

            bResult = GetData( pQuery1, &dwCount, sizeof( dwCount), 0, &dwLoop );

            bResult = verifyIN( (float) dwCount,    // regular-size => 1.5% tolerance
                                area1 * LowBoundPct, 
                                area1 * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                area1 * LowBoundPct,
                                area1 * HiBoundPct,
                                dwCount );
        }
        else if ( nTest == 3 || nTest == 2 || nTest == 1 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, unsigned c) : x(x), y(y), z(z), rhw(1.0f / z), c(c) {}
                float x;
                float y;
                float z;
                float rhw;
                unsigned c;
            } *pVertex;

            const int nVertices = 6;
            const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, &pQuery );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            *pVertex++ = Vertex( 10.0f, 10.0f, 0.5f, 0xffff0000 );
            *pVertex++ = Vertex( 50.0f, 10.0f, 0.5f, 0xff00ff00 );
            *pVertex++ = Vertex( 25.0f, 60.0f, 0.5f, 0xff0000ff );

            float z = nTest == 2 ? 0.6f : 0.4f;

            *pVertex++ = Vertex( 10.0f, 10.0f, z, 0xffff0000 );
            *pVertex++ = Vertex( 50.0f, 10.0f, z, 0xff00ff00 );
            *pVertex++ = Vertex( 25.0f, 60.0f, z, 0xff0000ff );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nTest == 1 ? 1 : nVertices / 3 );

			bResult = pQuery->Issue( ISSUE_END );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

			//bResult = pQuery->Issue( ISSUE_END );

            DWORD dwCount;
            bResult = GetData( pQuery, &dwCount, sizeof( dwCount), GETDATA_FLUSH );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);
            float triangleArea = areaTriangle( pVertex, sizeof(Vertex) );
            bResult = pVertexBuffer->Unlock();

            float pixelCount = nTest == 3 ? 2.0f * triangleArea : triangleArea;

            bResult = verifyIN( (float) dwCount, 
                                pixelCount * LowBoundPct, 
                                pixelCount * HiBoundPct,
                                TEXT("GetData: Expecting occlusion count between (%f - %f) Actual (%d)\n"), 
                                pixelCount * LowBoundPct,
                                pixelCount * HiBoundPct,
                                dwCount );
        }
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryOcclusion: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryVertexStats::CQueryVertexStats()
{
	m_szTestName = _T("QueryVertexStats");
    m_szCommandKey = _T("QueryVertexStats");
}

bool CQueryVertexStats::CapsCheck()
{
	HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_VERTEXSTATS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_VERTEXSTATS.\n") );
		return false;
    }

    DEVICE_CREATION_PARAMETERS Parameters;
    hr = m_pSrcDevice->GetCreationParameters( &Parameters );
    if ( FAILED( hr ) || ( Parameters.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
    {
		WriteToLog( 1, TEXT("Test requires SWVP.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryVertexStats::TestInitialize()
{
    UINT uResult = CQueryConf::TestInitialize();

    if( uResult == D3DTESTINIT_RUN )
    {
        SetTestRange(1,5);
    }

    return uResult;
}

bool CQueryVertexStats::ExecuteTest(UINT nTest)
{
    HRESULT hr;
    CBoolException bResult;

    BeginTestCase( _T("QueryVertexStats"), nTest );

    try
    {
		ClearFrame();

		bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

        if ( nTest == 1 || nTest == 2 || nTest == 5 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, 
                        float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}
                float x;
                float y;
                float z;
                float nx;
                float ny;
                float nz;
            } *pVertex;

            const int nVertices = 4;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_VERTEXSTATS, &pQuery );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        m_PoolManaged,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            float fOffset = nTest == 5 ? 10.0f : 0.0f;

            *pVertex++ = Vertex( fOffset+0.0f, fOffset+0.0f, 0.0f );
            *pVertex++ = Vertex( fOffset+0.4f, fOffset+0.0f, 0.0f );
            *pVertex++ = Vertex( fOffset+0.4f, fOffset-0.4f, 0.0f );
            *pVertex++ = Vertex( fOffset+0.0f, fOffset-0.8f, 0.0f );

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            MATERIAL mtrl;
            zeroMemory( &mtrl  );
            mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
            mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
            mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
            bResult = m_pSrcDevice->SetMaterial( &mtrl );

            LIGHT light;
            zeroMemory( &light );
            light.Type = D3DLIGHT_DIRECTIONAL;
            light.Diffuse.r = 1.0f;
            light.Diffuse.g = 1.0f;
            light.Diffuse.b = 1.0f;
            light.Range = 1.0f;
            light.Direction.x = 0.0f;
            light.Direction.y = 0.0f;
            light.Direction.z = 1.0f;
            bResult = m_pSrcDevice->SetLight( 0, &light );

            bResult = m_pSrcDevice->LightEnable( 0, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

            bResult = m_pSrcDevice->EndScene();

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3DVERTEXSTATS vertexStats;
            bResult = GetData( pQuery, zeroMemory( &vertexStats ), sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

            bResult = verifyEQ( vertexStats.NumRenderedTriangles,
                                (DWORD)(nTest == 5 ? 0 : 2), 
                                TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                (DWORD)(nTest == 5 ? 0 : 2),
                                vertexStats.NumRenderedTriangles );

            if ( nTest == 2 )
            {
                bResult = GetData( pQuery, zeroMemory( &vertexStats ), sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

                bResult = verifyEQ( vertexStats.NumRenderedTriangles,
                                    (DWORD)2, 
                                    TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                    (DWORD)2,
                                    vertexStats.NumRenderedTriangles );

				bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

                bResult = pQuery->Issue( ISSUE_END );

                bResult = GetData( pQuery, &vertexStats, sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

                bResult = verifyEQ( vertexStats.NumRenderedTriangles,
                                    (DWORD)0, 
                                    TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                    (DWORD)0,
                                    vertexStats.NumRenderedTriangles );
            }
        }
        else if ( nTest == 3 || nTest == 4 )
        {
            struct Vertex
            {
                Vertex( float x, float y, float z, unsigned c) : x(x), y(y), z(z), rhw(1.0f / z), c(c) {}
                float x;
                float y;
                float z;
                float rhw;
                unsigned c;
            } *pVertex;

            const int nVertices = 3;
            const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_VERTEXSTATS, &pQuery );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            CAPS caps;
            bResult = m_pSrcDevice->GetDeviceCaps( &caps );

            VIEWPORT viewport;
            bResult = m_pSrcDevice->GetViewport( &viewport );

            if ( caps.GuardBandTop != 0.0f && caps.GuardBandBottom != 0.0f )
            {
                *pVertex++ = Vertex( float(viewport.X) + float(viewport.Width) / 2.0f, caps.GuardBandTop - 2.0f, 0.5f, 0xffff0000 );
                *pVertex++ = Vertex( float(viewport.X) + float(viewport.Width), caps.GuardBandBottom + 2.0f, 0.5f, 0xff00ff00 );
                *pVertex++ = Vertex( float(viewport.X), float(caps.GuardBandBottom) + 2.0f, 0.5f, 0xff0000ff );
            }
            else
            {
                *pVertex++ = Vertex( float(viewport.X) + float(viewport.Width) / 2.0f, float(viewport.Y) - float(viewport.Height), 0.5f, 0xffff0000 );
                *pVertex++ = Vertex( float(viewport.X) + float(viewport.Width), float(viewport.Y) + 2.0f * float(viewport.Height), 0.5f, 0xff00ff00 );
                *pVertex++ = Vertex( float(viewport.X), float(viewport.Y) + 2.0f * float(viewport.Height), 0.5f, 0xff0000ff );
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices / 3 );

            bResult = m_pSrcDevice->EndScene();

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3DVERTEXSTATS vertexStats;
            bResult = GetData( pQuery, zeroMemory( &vertexStats ), sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

            bResult = verifyEQ( vertexStats.NumExtraClippingTriangles,
                                (DWORD)2, 
                                TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                (DWORD)2,
                                vertexStats.NumExtraClippingTriangles );

            if ( nTest == 4 )
            {
                bResult = GetData( pQuery, zeroMemory( &vertexStats ), sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

                bResult = verifyEQ( vertexStats.NumExtraClippingTriangles,
                                    (DWORD)2, 
                                    TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                    (DWORD)2,
                                    vertexStats.NumExtraClippingTriangles );

				bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

                bResult = pQuery->Issue( ISSUE_END );

                bResult = GetData( pQuery, &vertexStats, sizeof( DEVINFO_D3DVERTEXSTATS ), GETDATA_FLUSH );

                bResult = verifyEQ( vertexStats.NumExtraClippingTriangles,
                                    (DWORD)0, 
                                    TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                    (DWORD)0,
                                    vertexStats.NumExtraClippingTriangles );
            }
        }
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryVertexStats: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryVCACHE::CQueryVCACHE()
{
	m_szTestName = _T("QueryVCACHE");
    m_szCommandKey = _T("QueryVCACHE");
}

bool CQueryVCACHE::CapsCheck()
{
	HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_VCACHE, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_VCACHE.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryVCACHE::TestInitialize()
{
    UINT uResult = CQueryConf::TestInitialize();

    if( uResult == D3DTESTINIT_RUN )
    {
        SetTestRange(1,1);
    }
	
	return uResult;
}

bool CQueryVCACHE::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( _T("QueryVCACHE"), nTest );

    struct Vertex
    {
        Vertex( float x, float y, float z, 
                float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}
        float x;
        float y;
        float z;
        float nx;
        float ny;
        float nz;
    } *pVertex;

    const int nVertices = 4;
    const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL;

    try
    {
        HRESULT hr;

        CAutoRelease< CQuery > pQuery;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_VCACHE, &pQuery );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_VCACHE vcache1;
        bResult = GetData( pQuery, zeroMemory( &vcache1 ), sizeof( DEVINFO_VCACHE ), GETDATA_FLUSH );

        CAutoRelease< CVertexBuffer > pVertexBuffer;
        bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                    USAGE_WRITEONLY,
                                                    fvf,
                                                    m_PoolManaged,
                                                    &pVertexBuffer );

        bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

        *pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
        *pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
        *pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
        *pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

        bResult = pVertexBuffer->Unlock();

        bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

        bResult = m_pSrcDevice->SetFVF( fvf );

        MATERIAL mtrl;
        zeroMemory( &mtrl  );
        mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
        mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
        mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
        bResult = m_pSrcDevice->SetMaterial( &mtrl );

        LIGHT light;
        zeroMemory( &light );
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;
        light.Range = 1.0f;
        light.Direction.x = 0.0f;
        light.Direction.y = 0.0f;
        light.Direction.z = 1.0f;
        bResult = m_pSrcDevice->SetLight( 0, &light );

        bResult = m_pSrcDevice->LightEnable( 0, TRUE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

        bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

        bResult = m_pSrcDevice->BeginScene();

        bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

        bResult = m_pSrcDevice->EndScene();

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_VCACHE vcache;
        bResult = GetData( pQuery, zeroMemory( &vcache ), sizeof( DEVINFO_VCACHE ), GETDATA_FLUSH );

        bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryVCACHE: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryResourceManager::CQueryResourceManager()
{
    m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP | DEVICECREATE_DISABLE_DRIVER_MANAGEMENT;;
	m_szTestName = _T("QueryResourceManager");
    m_szCommandKey = _T("QueryResourceManager");
}

bool CQueryResourceManager::CapsCheck()
{
	HRESULT hr = m_pSrcDevice->CreateQuery( QUERYTYPE_RESOURCEMANAGER, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_RESOURCEMANAGER.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryResourceManager::TestInitialize()
{
    UINT uResult = CQueryConf::TestInitialize();

    if( uResult == D3DTESTINIT_RUN )
    {
		SetTestRange(1,2);
    }

    return uResult;
}

bool CQueryResourceManager::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( _T("QueryResourceManager"), nTest );

    struct Vertex
    {
        Vertex( float x, float y, float z, 
                float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}
        float x;
        float y;
        float z;
        float nx;
        float ny;
        float nz;
    } *pVertex;

    const int nVertices = 4;
    const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL;

    try
    {
        HRESULT hr;

        CAutoRelease< CQuery > pQuery;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_RESOURCEMANAGER, &pQuery );

        CAutoRelease< CVertexBuffer > pVertexBuffer;
        bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
                                                    USAGE_WRITEONLY,
                                                    fvf,
                                                    m_PoolManaged,
                                                    &pVertexBuffer );

        CAutoRelease< CTexture > pTexture;
        bResult = m_pSrcDevice->CreateTexture( 256, 256, 1, 0, FMT_A8R8G8B8, m_PoolManaged, &pTexture );
/*
        CAutoRelease< CCubeTexture > pSurface;
        bResult = m_pSrcDevice->CreateCubeTexture ( 256, 1, 0, FMT_R5G6B5, m_PoolManaged, &pSurface );
*/
        bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

        *pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
        *pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
        *pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
        *pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

        bResult = pVertexBuffer->Unlock();

        bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

        bResult = m_pSrcDevice->SetFVF( fvf );

        MATERIAL mtrl;
        zeroMemory( &mtrl  );
        mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
        mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
        mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
        bResult = m_pSrcDevice->SetMaterial( &mtrl );

        LIGHT light;
        zeroMemory( &light );
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;
        light.Range = 1.0f;
        light.Direction.x = 0.0f;
        light.Direction.y = 0.0f;
        light.Direction.z = 1.0f;
        bResult = m_pSrcDevice->SetLight( 0, &light );

        bResult = m_pSrcDevice->LightEnable( 0, TRUE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

        bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

        bResult = m_pSrcDevice->BeginScene();

        bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

        bResult = m_pSrcDevice->EndScene();

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_RESOURCEMANAGER resourceManager;
        bResult = GetData( pQuery, &resourceManager, sizeof( DEVINFO_RESOURCEMANAGER ), GETDATA_FLUSH );

        bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

        if ( nTest == 2 )
        {
            bResult = GetData( pQuery, zeroMemory( &resourceManager ), sizeof( DEVINFO_RESOURCEMANAGER ), GETDATA_FLUSH );
/*
            bResult = verifyEQ( vertexStats.NumRenderedTriangles,
                                (DWORD)2, 
                                TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                (DWORD)2,
                                vertexStats.NumRenderedTriangles );
*/
            bResult = pQuery->Issue( ISSUE_END );

            bResult = GetData( pQuery, &resourceManager, sizeof( DEVINFO_RESOURCEMANAGER ), GETDATA_FLUSH );
/*
            bResult = verifyEQ( vertexStats.NumRenderedTriangles,
                                (DWORD)0, 
                                TEXT("GetData: Expecting (%d) Actual (%d)\n"), 
                                (DWORD)0,
                                vertexStats.NumRenderedTriangles );
*/
        }
    }
	catch(CBoolException )
    {
		Fail();
		GetLastError();
        WriteToLog( "CQueryResourceManager: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------

CQueryCreate::CQueryCreate()
{
	m_szTestName = _T("QueryCreate");
    m_szCommandKey = _T("QueryCreate");
}

UINT CQueryCreate::TestInitialize()
{
    SetTestRange(1, nQueryTypeCount);
    return CQueryConf::TestInitialize();
}

bool CQueryCreate::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    char title[256];
    strcpy(title, "QueryCreate: ");
    strcat(title, QueryText[ nTest - 1 ]);

    BeginTestCase( title, nTest );

    try
    {
		if ( FAILED( m_pSrcDevice->CreateQuery( QueryTypes[ nTest - 1 ], NULL ) ) )
		{
			GetLastError();
		}
		else
		{
			CAutoRelease< CQuery > pQuery;
			bResult = m_pSrcDevice->CreateQuery( QueryTypes[ nTest - 1 ], &pQuery );
		}
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryCreate: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryCreateMany::CQueryCreateMany()
{
	m_szTestName = _T("QueryCreateMany");
    m_szCommandKey = _T("QueryCreateMany");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryCreateMany::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_EVENT.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_OCCLUSION, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_OCCLUSION.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}


UINT CQueryCreateMany::TestInitialize()
{
    SetTestRange(1, 4);
    return CQueryConf::TestInitialize();
}

bool CQueryCreateMany::ExecuteTest(UINT nTest)
{
    const unsigned numQueries = 4096;

    CBoolException bResult;

	static QUERYTYPE QueryTypes[] =
	{
		QUERYTYPE_EVENT,
        QUERYTYPE_OCCLUSION,
	};

    static char *QueryText[] =
	{
		"QUERYTYPE_EVENT",
        "QUERYTYPE_OCCLUSION",
	};

    char title[256];
    strcpy(title, "QueryCreateMany: ");
    strcat(title, QueryText[ nTest % 2 ]);

    BeginTestCase( title, nTest );

    CDevice *pDevice = nTest > 2 ? m_pRefDevice : m_pSrcDevice;

    QUERYTYPE type = QueryTypes[ nTest % 2 ];

    std::vector< CAutoRelease< CQuery > > Queries( numQueries );

    try
    {
        int i;
        for( i=0; i < numQueries; i++ )
        {
            bResult = pDevice->CreateQuery( type, &Queries[i] );
        }

        for( i=0; i < Queries.size(); i++ )
        {
            if ( QUERYTYPE_OCCLUSION == type )
            {
                bResult = Queries[i]->Issue( ISSUE_BEGIN );
            }

            bResult = Queries[i]->Issue( ISSUE_END );

            bResult = GetData( Queries[i], NULL, 0, GETDATA_FLUSH );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "QueryCreateMany: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------

CQueryTimeStamp::CQueryTimeStamp()
{
	m_szTestName = _T("QueryTimeStamp");
    m_szCommandKey = _T("QueryTimeStamp");
    //m_Options.D3DOptions.bReference = true;
    //m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;

	m_Options.ModeOptions.ZBufferFmt = FMT_D24S8;
}

bool CQueryTimeStamp::CapsCheck()
{
	HRESULT hr;

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_EVENT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_EVENT.\n") );
		return false;
    }
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMP.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMPDISJOINT.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMPFREQ.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryTimeStamp::TestInitialize()
{
    SetTestRange(1, 5);
    return CQueryConf::TestInitialize();
}

bool CQueryTimeStamp::CallProcess(LPSTR pszwCommand)
{
    HRESULT hr = S_OK;
    
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (FALSE == CreateProcess(0, 
                                pszwCommand,
                                0,
                                0,
                                FALSE,
                                0,
                                0,
                                NULL,
                                &si,
                                &pi))
    {
		WriteToLog( 1, TEXT("CreateProcess for xperf.exe failed. This test requires xperf.exe to be installed. hr = %s\n"),  m_pD3D->HResultToString(HRESULT_FROM_WIN32(GetLastError())));
        return false;
    }

    // Wait for xperf to exit so we know for sure that trace enabling is a success
    DWORD dwRet = WaitForSingleObject(pi.hProcess, 5000);
    if (WAIT_TIMEOUT == dwRet || WAIT_ABANDONED == dwRet)
    {
        WriteToLog( 1, TEXT("xperf.exe call timed out.\n") );
        return false;
    }

    //
    // Verify the exit code of xperf
    //
    dwRet=0;
    if(GetExitCodeProcess(pi.hProcess, &dwRet))
    {
		hr = HRESULT_FROM_WIN32(dwRet);
		if (FAILED(hr))
		{
			WriteToLog( 1, TEXT("xperf.exe failed, hr = %s.\n"), m_pD3D->HResultToString(hr));
			return false;
		}
    }
	else
	{
		WriteToLog( 1, TEXT("GetExitCodeProcess failed, hr = %s.\n"), m_pD3D->HResultToString(HRESULT_FROM_WIN32(GetLastError())));
        return false;
	}
    
    if (FALSE == CloseHandle(pi.hProcess))
    {
        WriteToLog( 1, TEXT("xperf.exe failed to close handle for process, hr = %s.\n"), m_pD3D->HResultToString(HRESULT_FROM_WIN32(GetLastError())));
        return false;
    }
    
    if (FALSE == CloseHandle(pi.hThread))
    {
        WriteToLog( 1, TEXT("xperf.exe failed to close handle for thread, hr = %s.\n"), m_pD3D->HResultToString(HRESULT_FROM_WIN32(GetLastError())));
        return false;
    }
    
    return true;
}

bool CQueryTimeStamp::ExecuteTest(UINT nTest)
{
	bool bPassedTest = false;

	bPassedTest = RunTimestampScenario(nTest);

	// In the case of the T40/8974, we may not have steady GPU counters
	// Therefore, if they fail the first iteration of this test, we will
	// hold the P-State steady and re-run the test one more time to give them
	// a chance to pass.

#if defined(_ARM_) || defined(_ARM64_) // temp. check for arm64
	if (!bPassedTest)
	{
		bool bCallProcessSucceeded = false;

		bCallProcessSucceeded = CallProcess("xperf.exe -start dxgklogger -on 802ec45a-1e99-4b83-9920-87c98277ba9d:0x1400:5 -BufferSize 1024 -MinBuffers 20 -MaxBuffers 160 -f dxgk.etl");
		if (!bCallProcessSucceeded)
		{
			// error will be logged in CallProcess
			bPassedTest = false;
			goto EndCase;
		}

		WriteToLog(TEXT("Re-running while holding p-state constant.\n"));
		bPassedTest = RunTimestampScenario(nTest);

		bCallProcessSucceeded = CallProcess("xperf.exe -stop dxgklogger");
		if (!bCallProcessSucceeded)
		{
			// error will be logged in CallProcess
			bPassedTest = false;
			goto EndCase;
		}
	}

EndCase:
#endif

	if (!bPassedTest)
	{
		Fail();
	}
	return bPassedTest;
}

bool CQueryTimeStamp::RunTimestampScenario(UINT nTest)
{
    CBoolException bResult;

    try
    {
	    double dTime;
	    unsigned uCount;

        CAutoRelease< CQuery > pDisjoint;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, &pDisjoint );

        CAutoRelease< CQuery > pTimeStamp1;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp1 );

        if ( 1 == nTest )
        {
            BeginTestCase( "One Timestamp verification", nTest );

            bResult = pDisjoint->Issue( ISSUE_BEGIN );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            UINT64 data1;
            bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            UINT64 data2;
            bResult = GetData( pTimeStamp1, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = pDisjoint->Issue( ISSUE_END );

            BOOL disjoint;
            bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = verifyEQ_noFail( disjoint,
                                (BOOL)FALSE, 
                                TEXT("GetData: Disjoint Query Expecting (%u) == (%u)\n"), 
                                (BOOL)FALSE,
                                disjoint );

            bResult = verifyGT_noFail( data2,
                                data1, 
                                TEXT("GetData: Timestamp Data Expecting (%I64u) > (%I64u)\n"), 
                                data2,
                                data1 );
        }
        else if ( 2 == nTest )
        {
            BeginTestCase( "Two Timestamp verification", nTest );

            CAutoRelease< CQuery > pTimeStamp2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp2 );

            bResult = pDisjoint->Issue( ISSUE_BEGIN );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            bResult = pTimeStamp1->GetData( NULL, 0, GETDATA_FLUSH );

            bResult = pTimeStamp2->Issue( ISSUE_END );

            bResult = pDisjoint->Issue( ISSUE_END );

            BOOL disjoint;
            bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = verifyEQ_noFail( disjoint,
                                (BOOL)FALSE, 
                                TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
                                (BOOL)FALSE,
                                disjoint );

            UINT64 data1;
            bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), GETDATA_FLUSH, &uCount, &dTime );

            UINT64 data2;
            bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = verifyGE_noFail( data2,
                                data1, 
                                TEXT("GetData: Timestamp Data Expecting (%I64u) >= (%I64u)\n"), 
                                data2,
                                data1 );
        }
        else if ( 3 == nTest )
        {
            const DWORD dwDelay = 500;
            const double DelayTime = dwDelay / 1000.0;

            BeginTestCase( "Verify QPC approximately the same as One TS", nTest );

            LARGE_INTEGER Frequency, Count1, Count2;
            QueryPerformanceFrequency( &Frequency );

            CAutoRelease< CQuery > pFrequency;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, &pFrequency );

            // flush with event query

            bResult = pDisjoint->Issue( ISSUE_BEGIN );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            UINT64 data1;
            bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), GETDATA_FLUSH, &uCount );

            QueryPerformanceCounter( &Count1 );

            ::Sleep( dwDelay );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            UINT64 data2;
            bResult = GetData( pTimeStamp1, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

            QueryPerformanceCounter( &Count2 );

            bResult = pFrequency->Issue( ISSUE_END );

            bResult = pDisjoint->Issue( ISSUE_END );

            BOOL disjoint;
            bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = verifyEQ_noFail( disjoint,
                                (BOOL)FALSE, 
                                TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
                                (BOOL)FALSE,
                                disjoint );

            double ElapsedQPC = double( Count2.QuadPart - Count1.QuadPart ) / double( Frequency.QuadPart );

            bResult = verifyIN_noFail( ElapsedQPC, 
                                DelayTime * 0.8, 
                                DelayTime * 1.20,
                                TEXT("GetData: Expecting Elapsed QPC between (%f - %f) Actual (%f)\n"), 
                                DelayTime * 0.8,
                                DelayTime * 1.20,
                                ElapsedQPC );

            UINT64 data0;
            bResult = GetData( pFrequency, &data0, sizeof( data0 ), GETDATA_FLUSH );

            double ElapsedTS = double( data2 - data1 ) / double( data0 );

            bResult = verifyIN_noFail( ElapsedTS, 
                                DelayTime * 0.8, 
                                DelayTime * 1.20,
                                TEXT("GetData: Expecting Elapsed TS between (%f - %f) Actual (%f)\n"), 
                                DelayTime * 0.8,
                                DelayTime * 1.20,
                                ElapsedTS );
        }
        else if ( 4 == nTest )
        {
            const DWORD dwDelay = 500;
            const double DelayTime = dwDelay / 1000.0;

            BeginTestCase( "Verify QPC approximately the same as Two TS", nTest );

            CAutoRelease< CQuery > pTimeStamp2;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp2 );

            CAutoRelease< CQuery > pFrequency;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, &pFrequency );

            LARGE_INTEGER Frequency, Count1, Count2;
            QueryPerformanceFrequency( &Frequency );

            bResult = pDisjoint->Issue( ISSUE_BEGIN );

            bResult = pTimeStamp1->Issue( ISSUE_END );

            bResult = pTimeStamp1->GetData( NULL, 0, GETDATA_FLUSH );

            QueryPerformanceCounter( &Count1 );

            ::Sleep( dwDelay );

            bResult = pTimeStamp2->Issue( ISSUE_END );

            QueryPerformanceCounter( &Count2 );

            bResult = pFrequency->Issue( ISSUE_END );

            bResult = pDisjoint->Issue( ISSUE_END );

            BOOL disjoint;
            bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

            bResult = verifyEQ_noFail( disjoint,
                                (BOOL)FALSE, 
                                TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
                                (BOOL)FALSE,
                                disjoint );

            double ElapsedQPC = double( Count2.QuadPart - Count1.QuadPart ) / double( Frequency.QuadPart );

            bResult = verifyIN_noFail( ElapsedQPC, 
                                DelayTime * 0.8, 
                                DelayTime * 1.20,
                                TEXT("GetData: Expecting Elapsed QPC between (%f - %f) Actual (%f)\n"), 
                                DelayTime * 0.8,
                                DelayTime * 1.20,
                                ElapsedQPC );

            UINT64 data0;
            bResult = GetData( pFrequency, &data0, sizeof( data0 ), GETDATA_FLUSH, &uCount );

            UINT64 data1;
            bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), GETDATA_FLUSH, &uCount );

            UINT64 data2;
            bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

            double ElapsedTS = double( data2 - data1 ) / double( data0 );

            bResult = verifyIN_noFail( ElapsedTS, 
                                DelayTime * 0.8, 
                                DelayTime * 1.20,
                                TEXT("GetData: Expecting Elapsed TS between (%f - %f) Actual (%f)\n"), 
                                DelayTime * 0.8,
                                DelayTime * 1.20,
                                ElapsedTS );
        }
        else if ( 5 == nTest && false ) // disabled due to unpredictable timing results, see Windows OS bug 2007769
        {
			// Using the -AllSame command line will produce three sets of timestamps that all use the 
			// exact same scene.  This is useful for debugging...

			if (KeySet("AllSame"))
			{
				BOOL disjoint;
				UINT64 data1, data2;

				CAutoRelease< CQuery > pTimeStamp2;
				bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp2 );

				BeginTestCase( "Verify Little Render Job Time < Big Render Job Time", nTest );

				// nNumJobs and uJobs array size should match
				int nNumJobs = 3;
				UINT64 uJobs[3];

				for (int i=0; i < nNumJobs; i++)
				{
					struct Vertex
					{
						Vertex( float x, float y, float z, DWORD c = 0xffff0000) : 
							x(x), y(y), z(z), c(c) {}
						float x;
						float y;
						float z;
						DWORD c;
					} *pVertex;

					const int nVertices = 6;
					const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

					CAutoRelease< CVertexBuffer > pVertexBuffer;
					bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
																USAGE_WRITEONLY,
																fvf,
																D3DPOOL_DEFAULT,
																&pVertexBuffer );

					bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

					bResult = pVertexBuffer->Unlock();

					bResult = m_pSrcDevice->SetFVF( fvf );

					bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

					bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

					// Render scene with No Queries to reduce startup noise...

					bResult = m_pSrcDevice->BeginScene();

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

					// Render scene with Queries...

					bResult = m_pSrcDevice->BeginScene();

					bResult = pDisjoint->Issue( ISSUE_BEGIN );

					bResult = pTimeStamp1->Issue( ISSUE_END );

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = pTimeStamp2->Issue( ISSUE_END );

					bResult = pDisjoint->Issue( ISSUE_END );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

					bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

					bResult = verifyEQ_noFail( disjoint,
										(BOOL)FALSE, 
										TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
										(BOOL)FALSE,
										disjoint );

					bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

					bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), 0, &uCount );

					uJobs[i] = data2 - data1;

					WriteToLog( "GetData: Job Size Timestamp: Job[%d] = (%I64u)\n", i, uJobs[i] );
				}

				// Force a failure to see the log timing messages
				bResult = false;
			}
			else
			{
				// Regular QueryTimeStamp execution path for Test #5

				BOOL disjoint;
				UINT64 data1, data2;

				CAutoRelease< CQuery > pTimeStamp2;
				bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp2 );

				BeginTestCase( "Verify Little Render Job Time < Big Render Job Time", nTest );

				// This first timestamp job is only here to reduce the amount of startup noise for the subsequent real tests and
				// the results for this job are ignored.
				{
					struct Vertex
					{
						Vertex( float x, float y, float z, DWORD c = 0xffff0000) : 
							x(x), y(y), z(z), c(c) {}
						float x;
						float y;
						float z;
						DWORD c;
					} *pVertex;

					const int nVertices = 6;
					const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

					CAutoRelease< CVertexBuffer > pVertexBuffer;
					bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
																USAGE_WRITEONLY,
																fvf,
																D3DPOOL_DEFAULT,
																&pVertexBuffer );

					bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

					bResult = pVertexBuffer->Unlock();

					bResult = m_pSrcDevice->SetFVF( fvf );

					bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

					bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

					// Render the scene without the queries...this will help flush out any startup costs

					bResult = m_pSrcDevice->BeginScene();

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

					// Render the scene with the queries...

					bResult = m_pSrcDevice->BeginScene();

					bResult = pDisjoint->Issue( ISSUE_BEGIN );

					bResult = pTimeStamp1->Issue( ISSUE_END );

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = pTimeStamp2->Issue( ISSUE_END );

					bResult = pDisjoint->Issue( ISSUE_END );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
				}

				bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

				bResult = verifyEQ_noFail( disjoint,
									(BOOL)FALSE, 
									TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
									(BOOL)FALSE,
									disjoint );

				bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

				bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), 0, &uCount );

				UINT64 uPrimerJob = data2 - data1;

/*
				// The following was the original code for the "small job" timestamp query.  This code
				// assumed XP style fixed function timing which is not true for Vista.  On Vista, lighting
				// is done through vertex shaders with the help of the fixed function mapper...unfortunately
				// this greatly increases the time to render the simple scene.
				{
					struct Vertex
					{
						Vertex( float x, float y, float z, float nx = 0.0f, float ny = 0.0f, float nz = -1.0f) : 
							x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}
						float x;
						float y;
						float z;
						float nx;
						float ny;
						float nz;
					} *pVertex;

					const int nVertices = 4;
					const DWORD fvf = D3DFVF_XYZ | D3DFVF_NORMAL;

					CAutoRelease< CVertexBuffer > pVertexBuffer;
					bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
																USAGE_WRITEONLY,
																fvf,
																D3DPOOL_DEFAULT,
																&pVertexBuffer );

					bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

					bResult = pVertexBuffer->Unlock();

					bResult = m_pSrcDevice->SetFVF( fvf );

					bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

					MATERIAL mtrl;
					zeroMemory( &mtrl  );
					mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
					mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
					mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
					bResult = m_pSrcDevice->SetMaterial( &mtrl );

					LIGHT light;
					zeroMemory( &light );
					light.Type = D3DLIGHT_DIRECTIONAL;
					light.Diffuse.r = 1.0f;
					light.Diffuse.g = 1.0f;
					light.Diffuse.b = 1.0f;
					light.Range = 1.0f;
					light.Direction.x = 0.0f;
					light.Direction.y = 0.0f;
					light.Direction.z = 1.0f;
					bResult = m_pSrcDevice->SetLight( 0, &light );

					bResult = m_pSrcDevice->LightEnable( 0, TRUE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

					bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

					bResult = m_pSrcDevice->BeginScene();

					bResult = pDisjoint->Issue( ISSUE_BEGIN );

					bResult = pTimeStamp1->Issue( ISSUE_END );

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, nVertices - 2 );

					bResult = pTimeStamp2->Issue( ISSUE_END );

					bResult = pDisjoint->Issue( ISSUE_END );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
				}
*/
				{
					struct Vertex
					{
						Vertex( float x, float y, float z, DWORD c = 0xffff0000) : 
							x(x), y(y), z(z), c(c) {}
						float x;
						float y;
						float z;
						DWORD c;
					} *pVertex;

					const int nVertices = 6;
					const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

					CAutoRelease< CVertexBuffer > pVertexBuffer;
					bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(Vertex),
																USAGE_WRITEONLY,
																fvf,
																D3DPOOL_DEFAULT,
																&pVertexBuffer );

					bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, 0.0f, 0.0f );
					*pVertex++ = Vertex( 0.4f, -0.4f, 0.0f );
					*pVertex++ = Vertex( 0.0f, -0.8f, 0.0f );

					bResult = pVertexBuffer->Unlock();

					bResult = m_pSrcDevice->SetFVF( fvf );

					bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(Vertex) );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

					bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );

					// Render the scene without the queries...this will help flush out any startup costs

					bResult = m_pSrcDevice->BeginScene();

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

					// Render the scene with the queries...

					bResult = m_pSrcDevice->BeginScene();

					bResult = pDisjoint->Issue( ISSUE_BEGIN );

					bResult = pTimeStamp1->Issue( ISSUE_END );

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertices );

					bResult = pTimeStamp2->Issue( ISSUE_END );

					bResult = pDisjoint->Issue( ISSUE_END );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
				}

				bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

				bResult = verifyEQ_noFail( disjoint,
									(BOOL)FALSE, 
									TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
									(BOOL)FALSE,
									disjoint );

				bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

				bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), 0, &uCount );

				UINT64 uSmallJob = data2 - data1;
				
				{
					static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

					const nK = 5;	// overlap count
					const nU = 1;	// number divisions in u
					const nV = 1;	// number divisions in v
					const float uStep = 2.0f / nU;
					const float vStep = 2.0f / nV;
					const float uTexStep = 1.0f / nU;
					const float vTexStep = 1.0f / nV;
					const int nVertices = 2 * nV * (nU + 1) * nK;
					const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

					std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

					CAutoRelease< CTexture > pTexture1;
					bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

					CAutoRelease< CTexture > pTexture2;
					bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

					CAutoRelease< CVertexBuffer > pVertexBuffer;
					bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
																USAGE_WRITEONLY,
																fvf,
																D3DPOOL_DEFAULT,
																&pVertexBuffer );

					UTLVertex *pVertex;
					bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

					int iColor = 0;
					for( int k=0; k < nK; k++)
					{
						float y = 1.0f;
						float vTex = 0.0f;
						for( int j=0; j <= nV; j++ )
						{
							pC->clear();

							float x = -1.0f;
							float uTex = 0.0f;
							for( int i=0; i <= nU; i++ )
							{
								pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
								x += uStep;
								uTex += uTexStep;

								if ( j )
								{
									*pVertex = pC->at( i );
									pVertex++->c = colors[iColor++ % 3];

									*pVertex = pL->at( i );
									pVertex++->c = colors[iColor++ % 3];
								}
							}

							y -= vStep;
							vTex += vTexStep;
							std::swap( pL, pC );
						}
					}

					bResult = pVertexBuffer->Unlock();

					bResult = m_pSrcDevice->SetFVF( fvf );

					bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

					bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

					bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

					bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

					bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

					bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

					bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

					bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

					bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

					bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

					bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

					bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

					bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

					bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

					DWORD numPasses;
					bResult = m_pSrcDevice->ValidateDevice( &numPasses );

					bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

					// Render the scene without the queries...this will help flush out any startup costs

					bResult = m_pSrcDevice->BeginScene();

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

					// Render the scene with the queries...

					bResult = m_pSrcDevice->BeginScene();

					bResult = pDisjoint->Issue( ISSUE_BEGIN );

					bResult = pTimeStamp1->Issue( ISSUE_END );

					bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

					bResult = pTimeStamp2->Issue( ISSUE_END );

					bResult = pDisjoint->Issue( ISSUE_END );

					bResult = m_pSrcDevice->EndScene();

					bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
				}

				bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount, &dTime );

				bResult = verifyEQ_noFail( disjoint,
									(BOOL)FALSE, 
									TEXT("GetData: Disjoint Query Data Expecting (%u) == (%u)\n"), 
									(BOOL)FALSE,
									disjoint );

				bResult = GetData( pTimeStamp2, &data2, sizeof( data2 ), GETDATA_FLUSH, &uCount );

				bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), 0, &uCount );

				UINT64 uBigJob = data2 - data1;

				// Used for debugging
				//
				// WriteToLog( "GetData: Job Size Timestamp uPrimerJob (%I64u), SmallJob (%I64u), BigJob (%I64u)\n", uPrimerJob, uSmallJob, uBigJob );
				// bResult = false;

				bResult = verifyGT_noFail( uBigJob,
									uSmallJob, 
									TEXT("GetData: Job Size Timestamp Difference Expecting BigJob (%I64u) > SmallJob (%I64u)\n"), 
									uBigJob,
									uSmallJob );
			}
		}
    }
	catch(CBoolException )
    {
        GetLastError();
        WriteToLog( "CQueryTimeStamp: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryTimeStampFrequency::CQueryTimeStampFrequency()
{
	m_szTestName = _T("QueryTimeStampFrequency");
    m_szCommandKey = _T("QueryTimeStampFrequency");
    //m_Options.D3DOptions.bReference = true;
}

bool CQueryTimeStampFrequency::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMPFREQ.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryTimeStampFrequency::TestInitialize()
{
    SetTestRange(1, 2);
    return CQueryConf::TestInitialize();
}

bool CQueryTimeStampFrequency::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Timestamp Frequency", nTest );

    try
    {
        UINT64 data0;
        unsigned uCount;

        CAutoRelease< CQuery > pDisjoint;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, &pDisjoint );

        CAutoRelease< CQuery > pFrequency;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, &pFrequency );

        bResult = pDisjoint->Issue( ISSUE_BEGIN );

        bResult = pFrequency->Issue( ISSUE_END );

        bResult = pDisjoint->Issue( ISSUE_END );

        BOOL disjoint;
        bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount );

        bResult = verifyEQ( disjoint,
                            (BOOL)FALSE, 
                            TEXT("GetData: Query Disjoint Data Expecting (%u) == (%u)\n"), 
                            (BOOL)FALSE,
                            disjoint );

        bResult = GetData( pFrequency, &data0, sizeof( data0 ), GETDATA_FLUSH, &uCount );

        if ( 1 == nTest )
        {
            bResult = verifyEQ( uCount,
                                unsigned(1), 
                                TEXT("GetData: Frequency Query Count Expecting (%u) == (%u)\n"), 
                                uCount,
                                unsigned(1) );

            bResult = verifyGT( data0,
                                UINT64( 10000000 ), 
                                TEXT("GetData: Frequency Query Data Expecting (%I64u) > (%I64u)\n"), 
                                data0,
                                UINT64( 10000000 ) );
        }
        else if ( 2 == nTest )
        {
            const int numQueries = 100;

            std::vector< CAutoRelease< CQuery > > Queries( numQueries );
            int i;

            for( i=0; i < numQueries; i++ )
            {
                bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, &Queries[i] );
            }

            CAutoRelease< CQuery > pDisjoint;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, &pDisjoint );

            bResult = pDisjoint->Issue( ISSUE_BEGIN );

            for( i=0; i < Queries.size(); i++ )
            {
                bResult = Queries[i]->Issue( ISSUE_END );

                UINT64 data;
                bResult = GetData( Queries[i], &data, sizeof( data ), GETDATA_FLUSH );

                bResult = verifyEQ( data0,
                                    data, 
                                    TEXT("GetData: Frequency Query Data Expecting (%I64u) == (%I64u)\n"), 
                                    data0,
                                    data );
            }

            bResult = pDisjoint->Issue( ISSUE_END );

            BOOL disjoint;
            bResult = GetData( pDisjoint, &disjoint, sizeof( disjoint ), GETDATA_FLUSH, &uCount );

            bResult = verifyEQ( disjoint,
                                (BOOL)FALSE, 
                                TEXT("GetData: Query Disjoint Data Expecting (%u) == (%u)\n"), 
                                (BOOL)FALSE,
                                disjoint );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryTimeStampFrequency: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryTimeStampDisjoint::CQueryTimeStampDisjoint()
{
	m_szTestName = _T("QueryTimeStampDisjoint");
    m_szCommandKey = _T("QueryTimeStampDisjoint");
    //m_Options.D3DOptions.bReference = true;
    //m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;

	m_Options.ModeOptions.ZBufferFmt = FMT_D24S8;
}

bool CQueryTimeStampDisjoint::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMP.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMPDISJOINT.\n") );
		return false;
    }

    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_TIMESTAMPFREQ.\n") );
		return false;
    }
/*
    if ( !IsPwrSuspendAllowed() )
    {
		WriteToLog( 1, TEXT("Test requires support for Power Suspend.\n") );
		return false;
    }
*/
	return CQueryConf::CapsCheck();
}


UINT CQueryTimeStampDisjoint::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryTimeStampDisjoint::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    try
    {
        BeginTestCase( "Timestamp Disjoint", nTest );

        CAutoRelease< CQuery > pFrequency;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPFREQ, &pFrequency );

        CAutoRelease< CQuery > pDisjoint;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMPDISJOINT, &pDisjoint );

        CAutoRelease< CQuery > pTimeStamp1;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp1 );

        CAutoRelease< CQuery > pTimeStamp2;
        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_TIMESTAMP, &pTimeStamp2 );

        bResult = pDisjoint->Issue( ISSUE_BEGIN );

        bResult = pTimeStamp1->Issue( ISSUE_END );

        bResult = pTimeStamp1->GetData( NULL, 0, GETDATA_FLUSH );

        // Set a timer to wait for 30 seconds and resume in suspended power conservation mode
        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart=-300000000;

        HANDLE hTimer = CreateWaitableTimer( NULL, TRUE, "WaitableTimer" );
        bResult = 0 != hTimer;

        bResult = 0 != SetWaitableTimer( hTimer, &liDueTime, 0, NULL, NULL, TRUE );

        //bResult = 0 != SetSuspendState( FALSE, FALSE, FALSE );

        bResult = WAIT_OBJECT_0 == WaitForSingleObject( hTimer, INFINITE );

        CloseHandle( hTimer );

        bResult = pTimeStamp2->Issue( ISSUE_END );

        bResult = pFrequency->Issue( ISSUE_END );

        bResult = pDisjoint->Issue( ISSUE_END );

        BOOL data;
        HRESULT hr = GetData( pDisjoint, &data, sizeof( data ), GETDATA_FLUSH );

        if ( D3DERR_DEVICELOST == hr )
        {
			pTimeStamp2.release();
			pTimeStamp1.release();
            pDisjoint.release();
            pFrequency.release();

			m_pSrcDevice->ReleaseWrappers();

			while ( D3DERR_DEVICELOST == hr )
            {
                hr = m_pSrcDevice->TestCooperativeLevel();

                if ( D3DERR_DEVICENOTRESET == hr )
                {
                    hr = m_pSrcDevice->Reset( m_pSrcDevice->GetPresParams() );
                }
            }

            GetLastError();
        }
        else if ( S_OK == hr )
        {
            UINT64 data0;
            bResult = GetData( pFrequency, &data0, sizeof( data0 ), 0 );

            UINT64 data1;
            bResult = GetData( pTimeStamp1, &data1, sizeof( data1 ), 0 );

            UINT64 data2;
            bResult = GetData( pTimeStamp2, &data2, sizeof( data1 ), 0 );

            double elapsed = double( data2 - data1 ) / double( data0 );

            bResult = verifyIN( elapsed, 
                                30.0 * 0.85, 
                                30.0 * 1.15,
                                TEXT("GetData: Elapsed Timestamp Data Expecting between (%f - %f) Actual (%f)\n"), 
                                30.0 * 0.85,
                                30.0 * 1.15,
                                elapsed );
        }
        else
        {
            bResult = hr;
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryTimeStampDisjoint: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryPipelineTimings::CQueryPipelineTimings()
{
	m_szTestName = _T("QueryPipelineTimings");
    m_szCommandKey = _T("QueryPipelineTimings");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryPipelineTimings::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_PIPELINETIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_PIPELINETIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryPipelineTimings::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryPipelineTimings::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Pipeline Timings", nTest );

    try
    {
        unsigned count;
        CAutoRelease< CQuery > pQuery;

        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_PIPELINETIMINGS, &pQuery );

        bResult = pQuery->Issue( ISSUE_BEGIN );

        static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

        const nK = 5;	// overlap count
        const nU = 1;	// number divisions in u
        const nV = 1;	// number divisions in v
        const float uStep = 2.0f / nU;
        const float vStep = 2.0f / nV;
        const float uTexStep = 1.0f / nU;
        const float vTexStep = 1.0f / nV;
        const int nVertices = 2 * nV * (nU + 1) * nK;
        const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

        std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

        CAutoRelease< CTexture > pTexture1;
        bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

        CAutoRelease< CTexture > pTexture2;
        bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

        CAutoRelease< CVertexBuffer > pVertexBuffer;
        bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                    USAGE_WRITEONLY,
                                                    fvf,
                                                    D3DPOOL_DEFAULT,
                                                    &pVertexBuffer );

        UTLVertex *pVertex;
        bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

        int iColor = 0;
        for( int k=0; k < nK; k++)
        {
            float y = 1.0f;
            float vTex = 0.0f;
            for( int j=0; j <= nV; j++ )
            {
                pC->clear();

                float x = -1.0f;
                float uTex = 0.0f;
                for( int i=0; i <= nU; i++ )
                {
                    pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
                    x += uStep;
                    uTex += uTexStep;

                    if ( j )
                    {
                        *pVertex = pC->at( i );
                        pVertex++->c = colors[iColor++ % 3];

                        *pVertex = pL->at( i );
                        pVertex++->c = colors[iColor++ % 3];
                    }
                }

                y -= vStep;
                vTex += vTexStep;
                std::swap( pL, pC );
            }
        }

        bResult = pVertexBuffer->Unlock();

        bResult = m_pSrcDevice->SetFVF( fvf );

        bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

        bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

        bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

		bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

		bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

        bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

        bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

        bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

        //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		DWORD numPasses;
		bResult = m_pSrcDevice->ValidateDevice( &numPasses );

        bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

        bResult = m_pSrcDevice->BeginScene();

        bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

        bResult = m_pSrcDevice->EndScene();

        bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_D3D9PIPELINETIMINGS data;
        bResult = GetData( pQuery, zeroMemory( &data ), sizeof( data ), GETDATA_FLUSH, &count );

        bResult = verifyGT( count,
                            (unsigned) 1, 
                            TEXT("GetData: GetData iterations Expecting (%u) > (%u)\n"), 
                            count,
                            (unsigned) 1);

        if ( 1 == nTest )
        {
            bResult = verifyEQ( isZeroMemory( &data ),
                                false, 
                                TEXT("GetData: Nonzero Data Content Expecting (%u) Actual (%u)\n"), 
                                false,
                                true );

            float sum = data.VertexProcessingTimePercent + 
                        data.PixelProcessingTimePercent +
                        data.OtherGPUProcessingTimePercent +
                        data.GPUIdleTimePercent;

            bResult = verifyIN( sum, 
                                1.0f - 0.001f, 
                                1.0f + 0.001f,
                                TEXT("GetData: Expecting percentage sum between (%f - %f) Actual (%f)\n"), 
                                1.0f - 0.001f,
                                1.0f + 0.001f,
                                sum );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryPipelineTimings: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryInterfaceTimings::CQueryInterfaceTimings()
{
	m_szTestName = _T("QueryInterfaceTimings");
    m_szCommandKey = _T("QueryInterfaceTimings");
    //m_Options.D3DOptions.bReference = true;
    //m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
    //m_Options.D3DOptions.dwPresentInterval = PRESENT_INTERVAL_ONE;
}

bool CQueryInterfaceTimings::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_INTERFACETIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryInterfaceTimings::TestInitialize()
{
    SetTestRange(1, 2);
    return CQueryConf::TestInitialize();
}

bool CQueryInterfaceTimings::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    try
    {
        if ( false )
        {
            BeginTestCase( "Interface Timings - Waiting for GPU to Use Application Resource", nTest );

            unsigned count;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, &pQuery );

            static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

            const nK = 1;	// overlap count
            const nU = 2048;	// number divisions in u
            const nV = 2048;	// number divisions in v
            const float uStep = 2.0f / nU;
            const float vStep = 2.0f / nV;
            const float uTexStep = 1.0f / nU;
            const float vTexStep = 1.0f / nV;
            const int nVertices = 2 * nV * (nU + 1) * nK;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

            std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

            CAutoRelease< CTexture > pTexture1;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

            CAutoRelease< CTexture > pTexture2;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            UTLVertex *pVertex;
            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            int iColor = 0;
            for( int k=0; k < nK; k++)
            {
                float y = 1.0f;
                float vTex = 0.0f;
                for( int j=0; j <= nV; j++ )
                {
                    pC->clear();

                    float x = -1.0f;
                    float uTex = 0.0f;
                    for( int i=0; i <= nU; i++ )
                    {
                        float z = sin( uTex * 2.0f * 3.14159f ) + 1.0f;
                        pC->push_back( UTLVertex( x, y, z, uTex, vTex ) );
                        x += uStep;
                        uTex += uTexStep;

                        if ( j )
                        {
                            *pVertex = pC->at( i );
                            pVertex++->c = colors[iColor++ % 3];

                            *pVertex = pL->at( i );
                            pVertex++->c = colors[iColor++ % 3];
                        }
                    }

                    y -= vStep;
                    vTex += vTexStep;
                    std::swap( pL, pC );
                }
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

            bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

            //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		    //DWORD numPasses;
		    //bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery->Issue( ISSUE_BEGIN );

            bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );

		    CAutoRelease< CSurface > pSrcRenderTarget;
		    bResult = m_pSrcDevice->GetRenderTarget( 0, &pSrcRenderTarget );

            LOCKED_RECT lockRect;
            HRESULT hr = pSrcRenderTarget->LockRect( &lockRect, NULL, D3DLOCK_DONOTWAIT );

            if ( D3DERR_WASSTILLDRAWING == hr )
            {
                bResult = pSrcRenderTarget->LockRect( &lockRect, NULL, 0 );
            }
            else
            {
                // test problem
            }

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3D9INTERFACETIMINGS data;
            bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

            bResult = pSrcRenderTarget->UnlockRect();

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            bResult = verifyGT( data.WaitingForGPUToUseApplicationResourceTimePercent,
                                0.0f, 
                                TEXT("GetData: Expecting WaitingForGPUToAcceptMoreCommandsTimePercent > 0.0 Actual (%f)\n"), 
                                data.WaitingForGPUToUseApplicationResourceTimePercent );
        }
        else if ( 1 == nTest )
        {
            BeginTestCase( "Interface Timings - Waiting for GPU to accept more commands", nTest );

            unsigned count;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, &pQuery );

            static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

            const nK = 1;	    // overlap count
            const nU = 512;	// number divisions in u
            const nV = 512;	// number divisions in v
            const float uStep = 2.0f / nU;
            const float vStep = 2.0f / nV;
            const float uTexStep = 1.0f / nU;
            const float vTexStep = 1.0f / nV;
            const int nVertices = 2 * nV * (nU + 1) * nK;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

            std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

            CAutoRelease< CTexture > pTexture1;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

            CAutoRelease< CTexture > pTexture2;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        m_PoolManaged,
                                                        &pVertexBuffer );

            UTLVertex *pVertex;
            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            int iColor = 0;
            for( int k=0; k < nK; k++)
            {
                float y = 1.0f;
                float vTex = 0.0f;
                for( int j=0; j <= nV; j++ )
                {
                    pC->clear();

                    float x = -1.0f;
                    float uTex = 0.0f;
                    for( int i=0; i <= nU; i++ )
                    {
                        pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
                        x += uStep;
                        uTex += uTexStep;

                        if ( j )
                        {
                            *pVertex = pC->at( i );
                            pVertex++->c = colors[iColor++ % 3];

                            *pVertex = pL->at( i );
                            pVertex++->c = colors[iColor++ % 3];
                        }
                    }

                    y -= vStep;
                    vTex += vTexStep;
                    std::swap( pL, pC );
                }
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

            bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

            //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		    DWORD numPasses;
		    bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

            bResult = m_pSrcDevice->BeginScene();

            bResult = pQuery->Issue( ISSUE_BEGIN );

            for( int loop = 0; loop < 1000; loop++ )
            {
                bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );
            }

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3D9INTERFACETIMINGS data;
            bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH );

            bResult = m_pSrcDevice->EndScene();

            bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );

            bResult = verifyGT( data.WaitingForGPUToAcceptMoreCommandsTimePercent,
                                0.0f, 
                                TEXT("GetData: Expecting WaitingForGPUToAcceptMoreCommandsTimePercent > 0.0 Actual (%f)\n"), 
                                data.WaitingForGPUToAcceptMoreCommandsTimePercent );
        }
        else if ( 2 == nTest )
        {
            BeginTestCase( "Interface Timings - Wating for GPU to Stay within latency", nTest );

            unsigned count;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, &pQuery );

            static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

            const nK = 1;	// overlap count
            const nU = 512;	// number divisions in u
            const nV = 512;	// number divisions in v
            const float uStep = 2.0f / nU;
            const float vStep = 2.0f / nV;
            const float uTexStep = 1.0f / nU;
            const float vTexStep = 1.0f / nV;
            const int nVertices = 2 * nV * (nU + 1) * nK;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

            std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

            CAutoRelease< CTexture > pTexture1;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

            CAutoRelease< CTexture > pTexture2;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            UTLVertex *pVertex;
            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            int iColor = 0;
            for( int k=0; k < nK; k++)
            {
                float y = 1.0f;
                float vTex = 0.0f;
                for( int j=0; j <= nV; j++ )
                {
                    pC->clear();

                    float x = -1.0f;
                    float uTex = 0.0f;
                    for( int i=0; i <= nU; i++ )
                    {
                        pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
                        x += uStep;
                        uTex += uTexStep;

                        if ( j )
                        {
                            *pVertex = pC->at( i );
                            pVertex++->c = colors[iColor++ % 3];

                            *pVertex = pL->at( i );
                            pVertex++->c = colors[iColor++ % 3];
                        }
                    }

                    y -= vStep;
                    vTex += vTexStep;
                    std::swap( pL, pC );
                }
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

            bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

            //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		    DWORD numPasses;
		    bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

            bResult = pQuery->Issue( ISSUE_BEGIN );

            for ( int i=0; i<10000;i++ )
            {
                bResult = m_pSrcDevice->BeginScene();

                for( int j=0; j < 1; j++ )
                {
                    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );
                }

                bResult = m_pSrcDevice->EndScene();

                bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
            }

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3D9INTERFACETIMINGS data;
            bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

            bResult = verifyGT( data.WaitingForGPUToStayWithinLatencyTimePercent,
                                0.0f, 
                                TEXT("GetData: Expecting WaitingForGPUToStayWithinLatencyTimePercent > 0.0 Actual (%f)\n"), 
                                data.WaitingForGPUToStayWithinLatencyTimePercent );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryInterfaceTimings: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------

CQueryInterfaceTimingsFS::CQueryInterfaceTimingsFS()
{
	m_szTestName = _T("QueryInterfaceTimings2");
    m_szCommandKey = _T("QueryInterfaceTimings2");
#if FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
#else //FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
#endif
}

bool CQueryInterfaceTimingsFS::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_INTERFACETIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryInterfaceTimingsFS::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryInterfaceTimingsFS::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    try
    {
        if ( 1 == nTest )
        {
            BeginTestCase( "Interface Timings Fullscreen - Wating for GPU to Stay within latency", nTest );

            unsigned count;

            CAutoRelease< CQuery > pQuery;
            bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_INTERFACETIMINGS, &pQuery );

            static DWORD colors[] = { 0x8fff7f7f, 0x9f7fff7f, 0xaf7f7fff };

            const nK = 1;	// overlap count
            const nU = 512;	// number divisions in u
            const nV = 512;	// number divisions in v
            const float uStep = 2.0f / nU;
            const float vStep = 2.0f / nV;
            const float uTexStep = 1.0f / nU;
            const float vTexStep = 1.0f / nV;
            const int nVertices = 2 * nV * (nU + 1) * nK;
            const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

            std::vector< UTLVertex > v1, v2, *pL = &v2, *pC = &v1;

            CAutoRelease< CTexture > pTexture1;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), &pTexture1 );

            CAutoRelease< CTexture > pTexture2;
            bResult = m_pSrcDevice->D3DXCreateTextureFromResource( NULL, MAKEINTRESOURCE(IDB_BITMAP2), &pTexture2 );

            CAutoRelease< CVertexBuffer > pVertexBuffer;
            bResult = m_pSrcDevice->CreateVertexBuffer( nVertices * sizeof(UTLVertex),
                                                        USAGE_WRITEONLY,
                                                        fvf,
                                                        D3DPOOL_DEFAULT,
                                                        &pVertexBuffer );

            UTLVertex *pVertex;
            bResult = pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

            int iColor = 0;
            for( int k=0; k < nK; k++)
            {
                float y = 1.0f;
                float vTex = 0.0f;
                for( int j=0; j <= nV; j++ )
                {
                    pC->clear();

                    float x = -1.0f;
                    float uTex = 0.0f;
                    for( int i=0; i <= nU; i++ )
                    {
                        pC->push_back( UTLVertex( x, y, 0.5f, uTex, vTex ) );
                        x += uStep;
                        uTex += uTexStep;

                        if ( j )
                        {
                            *pVertex = pC->at( i );
                            pVertex++->c = colors[iColor++ % 3];

                            *pVertex = pL->at( i );
                            pVertex++->c = colors[iColor++ % 3];
                        }
                    }

                    y -= vStep;
                    vTex += vTexStep;
                    std::swap( pL, pC );
                }
            }

            bResult = pVertexBuffer->Unlock();

            bResult = m_pSrcDevice->SetFVF( fvf );

            bResult = m_pSrcDevice->SetStreamSource( 0, pVertexBuffer, sizeof(UTLVertex) );

            bResult = m_pSrcDevice->SetTexture( 0, pTexture1 );

            bResult = m_pSrcDevice->SetTexture( 1, pTexture2 );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		    bResult = m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );

		    bResult = m_pSrcDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_LINEAR );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

            bResult = m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

            //bResult = m_pSrcDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		    DWORD numPasses;
		    bResult = m_pSrcDevice->ValidateDevice( &numPasses );

            bResult = m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, 0xffffffff, 1.0f, 0 );

            bResult = pQuery->Issue( ISSUE_BEGIN );

            for ( int i=0; i<10000;i++ )
            {
                bResult = m_pSrcDevice->BeginScene();

                for( int j=0; j < 1; j++ )
                {
                    bResult = m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nVertices - 2 );
                }

                bResult = m_pSrcDevice->EndScene();

                bResult = m_pSrcDevice->Present( NULL, NULL, NULL, NULL );
            }

            bResult = pQuery->Issue( ISSUE_END );

            DEVINFO_D3D9INTERFACETIMINGS data;
            bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

            bResult = verifyGT( data.WaitingForGPUToStayWithinLatencyTimePercent,
                                0.0f, 
                                TEXT("GetData: Expecting WaitingForGPUToStayWithinLatencyTimePercent > 0.0 Actual (%f)\n"), 
                                data.WaitingForGPUToStayWithinLatencyTimePercent );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryInterfaceTimingsFS: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryVertexTimings::CQueryVertexTimings()
{
	m_szTestName = _T("QueryVertexTimings");
    m_szCommandKey = _T("QueryVertexTimings");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryVertexTimings::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_VERTEXTIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_VERTEXTIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryVertexTimings::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryVertexTimings::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Vertex Timings", nTest );

    try
    {
        unsigned count;
        CAutoRelease< CQuery > pQuery;

        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_VERTEXTIMINGS, &pQuery );

        bResult = pQuery->Issue( ISSUE_BEGIN );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_D3D9STAGETIMINGS data;
        bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

        if ( 1 == nTest )
        {
            bResult = verifyEQ( isZeroMemory( &data ),
                                false, 
                                TEXT("GetData: Nonzero Data Content Expecting (%u) Actual (%u)\n"), 
                                false,
                                true );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryVertexTimings: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryPixelTimings::CQueryPixelTimings()
{
	m_szTestName = _T("QueryPixelTimings");
    m_szCommandKey = _T("QueryPixelTimings");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryPixelTimings::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_PIXELTIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_PIXELTIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryPixelTimings::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryPixelTimings::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Pixel Timings", nTest );

    try
    {
        unsigned count;
        CAutoRelease< CQuery > pQuery;

        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_PIXELTIMINGS, &pQuery );

        bResult = pQuery->Issue( ISSUE_BEGIN );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_D3D9STAGETIMINGS data;
        bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

        if ( 1 == nTest )
        {
            bResult = verifyEQ( isZeroMemory( &data ),
                                false, 
                                TEXT("GetData: Nonzero Data Content Expecting (%u) Actual (%u)\n"), 
                                false,
                                true );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryPixelTimings: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryBandwidthTimings::CQueryBandwidthTimings()
{
	m_szTestName = _T("QueryBandwidthTimings");
    m_szCommandKey = _T("QueryBandwidthTimings");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryBandwidthTimings::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_BANDWIDTHTIMINGS, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_BANDWIDTHTIMINGS.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryBandwidthTimings::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryBandwidthTimings::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Bandwidth Timings", nTest );

    try
    {
        unsigned count;
        CAutoRelease< CQuery > pQuery;

        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_BANDWIDTHTIMINGS, &pQuery );

        bResult = pQuery->Issue( ISSUE_BEGIN );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_D3D9BANDWIDTHTIMINGS data;
        bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

        if ( 1 == nTest )
        {
            bResult = verifyEQ( isZeroMemory( &data ),
                                false, 
                                TEXT("GetData: Nonzero Data Content Expecting (%u) Actual (%u)\n"), 
                                false,
                                true );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryBandwidthTimings: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

CQueryCacheUtilization::CQueryCacheUtilization()
{
	m_szTestName = _T("QueryCacheUtilization");
    m_szCommandKey = _T("QueryCacheUtilization");
    m_Options.D3DOptions.bReference = true;
}

bool CQueryCacheUtilization::CapsCheck()
{
	HRESULT hr;
    
    hr = m_pSrcDevice->CreateQuery( QUERYTYPE_CACHEUTILIZATION, NULL );

    if ( FAILED( hr ) )
    {
		WriteToLog( 1, TEXT("Test requires support for QUERYTYPE_CACHEUTILIZATION.\n") );
		return false;
    }

	return CQueryConf::CapsCheck();
}

UINT CQueryCacheUtilization::TestInitialize()
{
    SetTestRange(1, 1);
    return CQueryConf::TestInitialize();
}

bool CQueryCacheUtilization::ExecuteTest(UINT nTest)
{
    CBoolException bResult;

    BeginTestCase( "Cache Utilization", nTest );

    try
    {
        unsigned count;
        CAutoRelease< CQuery > pQuery;

        bResult = m_pSrcDevice->CreateQuery( QUERYTYPE_CACHEUTILIZATION, &pQuery );

        bResult = pQuery->Issue( ISSUE_BEGIN );

        bResult = pQuery->Issue( ISSUE_END );

        DEVINFO_D3D9CACHEUTILIZATION data;
        bResult = GetData( pQuery, zeroMemory(&data), sizeof( data ), GETDATA_FLUSH, &count );

        if ( 1 == nTest )
        {
            bResult = verifyEQ( isZeroMemory( &data ),
                                false, 
                                TEXT("GetData: Nonzero Data Content Expecting (%u) Actual (%u)\n"), 
                                false,
                                true );
        }
    }
	catch(CBoolException )
    {
        Fail();
        GetLastError();
        WriteToLog( "CQueryCacheUtilization: exception count = %d, hr=%s(%#x)\n", bResult.getCount(), m_pD3D->HResultToString( (HRESULT)bResult), (HRESULT)bResult );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

CQueryTest::CQueryTest()
{
    m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_MULTITHREADED;
	m_szTestName = _T("Query");
    m_szCommandKey = _T("Query");

	m_pTestList = 0L;
}


//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT CQueryTest::TestInitialize()
{
	// Create the test list
	UINT n = CreateTestList();

	// Set the test range
    SetTestRange(1,n);

    return CQueryConf::TestInitialize();
}

//-----------------------------------------------------------------------------

bool CQueryTest::TestTerminate()
{
	ReleaseTestList();

	return true;
}

//-----------------------------------------------------------------------------

UINT CQueryTest::CreateTestList()
{
	D3DTEST_QUERY test;

    static DWORD dwValidIssueFlags[] =
    {
        ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END,
        ISSUE_BEGIN|ISSUE_END
    };

    static DWORD dwIssueFlags[] =
    {
        ISSUE_BEGIN,
        ISSUE_END
    };

	static DWORD dwGetDataFlags[] =
	{
		0,
		GETDATA_FLUSH,
	};

	UINT nIssueFlagsCount = sizeof dwIssueFlags / sizeof dwIssueFlags[0];
	UINT nGetDataFlagsCount = sizeof dwGetDataFlags / sizeof dwGetDataFlags[0];
	UINT nListCount    = sizeof g_TestList / sizeof g_TestList[0];
	UINT n = nListCount + nQueryTypeCount * nIssueFlagsCount * nGetDataFlagsCount;

	D3DTEST_QUERY* pTestList = m_pTestList = new D3DTEST_QUERY[n];

	// add the list items
#pragma prefast( suppress:412 "size is adequate for copy" )
	memcpy(pTestList, g_TestList, sizeof g_TestList);
	pTestList += nListCount;

	// spin through each query type
	for( unsigned int query_type = 0; query_type< nQueryTypeCount; query_type++ )
	{
		// spin through each issue flag combo
		for( unsigned int issue_index = 0; issue_index < nIssueFlagsCount; issue_index++ )
		{
		    // spin through each getdata flag combo
		    for( unsigned int flags_index = 0; flags_index < nGetDataFlagsCount; flags_index++ )
		    {
			    _stprintf(test.szName, _T("Query (query type = %d, issue flags = 0x%x, getdata flags = 0x%x)"), 
                    QueryTypes[query_type], dwIssueFlags[issue_index], dwGetDataFlags[flags_index]);
			    test.QueryType = QueryTypes[query_type];
			    test.IdType = PTR_OK;
                test.dwIssueFlags = dwIssueFlags[issue_index];
			    test.dwGetDataFlags = dwGetDataFlags[flags_index];
			    test.DataPointerType = PTR_OK;
			    test.nAlignmentOffset = 8;
                test.ReturnType = dwIssueFlags[issue_index] & dwValidIssueFlags[query_type] ? RET_VALID : RET_INVALID;

                // special class for queries with BEGIN
                if ( test.dwIssueFlags == ISSUE_BEGIN )
                {
                    if ( test.QueryType == QUERYTYPE_OCCLUSION ||
                         test.QueryType == QUERYTYPE_TIMESTAMPDISJOINT  ||
                         test.QueryType == QUERYTYPE_PIPELINETIMINGS  ||
                         test.QueryType == QUERYTYPE_INTERFACETIMINGS  ||
                         test.QueryType == QUERYTYPE_VERTEXTIMINGS  ||
                         test.QueryType == QUERYTYPE_PIXELTIMINGS ||
                         test.QueryType == QUERYTYPE_BANDWIDTHTIMINGS ||
                         test.QueryType == QUERYTYPE_CACHEUTILIZATION )
                    {
			            test.ReturnType = RET_INVALID;
                    }
                }

	            switch(test.QueryType)
	            {
                case QUERYTYPE_VCACHE:
		            test.dwDataSize = sizeof DEVINFO_VCACHE;
		            break;
	            case QUERYTYPE_RESOURCEMANAGER:
		            test.dwDataSize = sizeof DEVINFO_RESOURCEMANAGER;
		            break;
	            case QUERYTYPE_VERTEXSTATS:
		            test.dwDataSize = sizeof DEVINFO_D3DVERTEXSTATS;
		            break;
	            case QUERYTYPE_EVENT:
		            test.dwDataSize = sizeof BOOL;
		            break;
	            case QUERYTYPE_OCCLUSION:
		            test.dwDataSize = sizeof DWORD;
		            break;
                case QUERYTYPE_TIMESTAMP:
		            test.dwDataSize = sizeof UINT64;
		            break;
	            case QUERYTYPE_TIMESTAMPDISJOINT:
		            test.dwDataSize = sizeof BOOL;
		            break;
	            case QUERYTYPE_TIMESTAMPFREQ:
		            test.dwDataSize = sizeof UINT64;
		            break;
	            case QUERYTYPE_PIPELINETIMINGS:
		            test.dwDataSize = sizeof DEVINFO_D3D9PIPELINETIMINGS;
		            break;
	            case QUERYTYPE_INTERFACETIMINGS:
		            test.dwDataSize = sizeof DEVINFO_D3D9INTERFACETIMINGS;
		            break;
	            case QUERYTYPE_VERTEXTIMINGS:
		            test.dwDataSize = sizeof DEVINFO_D3D9STAGETIMINGS;
		            break;
	            case QUERYTYPE_PIXELTIMINGS:
		            test.dwDataSize = sizeof DEVINFO_D3D9STAGETIMINGS;
		            break;
	            case QUERYTYPE_BANDWIDTHTIMINGS:
		            test.dwDataSize = sizeof DEVINFO_D3D9BANDWIDTHTIMINGS;
		            break;
	            case QUERYTYPE_CACHEUTILIZATION:
		            test.dwDataSize = sizeof DEVINFO_D3D9CACHEUTILIZATION;
		            break;
	            default:
		            test.dwDataSize = 0;
		            break;
	            }

			    memcpy( pTestList++, &test, sizeof test );
		    }
        }
	}

	return n;
}

//-----------------------------------------------------------------------------

void CQueryTest::ReleaseTestList()
{
	if( m_pTestList )
	{
		delete[] m_pTestList;
		m_pTestList = 0;
	}
}

//-----------------------------------------------------------------------------

bool CQueryTest::OnException(LPCTSTR szLabel, bool bWasInvalid)
{
	if( m_bIsDebug || !bWasInvalid)
	{
		WriteToLog(	_T("%s() caused unhandled exception\n"), szLabel);
		Fail();
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CQueryTest::ExecuteTest(UINT nTest)
{
	D3DTEST_QUERY* pThisTest = m_pTestList + (nTest-1);
 	MEMORYSTATUS memory_before, memory_after;
	HRESULT hr;
	bool bResult = true;
    
	ClearFrame();

	// tell the framework let's go
	BeginTestCase( pThisTest->szName, nTest );
    
	// Create a buffered display mode structure	
	CTestPtr data(pThisTest->dwDataSize, pThisTest->nAlignmentOffset, 8);
	if( !data.IsValid() )
	{
		Abort();
		return true;
	}

	// translate the mode pointer type into a pointer
	LPVOID pData = 0L;
	if( PTR_OK == pThisTest->DataPointerType )
	{
		pData = data.GetPtr();
		memset( pData, 0x01, pThisTest->dwDataSize);
	}
	else if (PTR_NULL == pThisTest->DataPointerType )
		pData = 0L;
	else if (PTR_INVALID == pThisTest->DataPointerType )
		pData = (void*)-1L;
	else
	{
		WriteToLog(_T("Invalid Data Pointer Type\n"));
		Abort();
		return true;
	}
	
	// verify the query is supported
	if(FAILED(m_pSrcDevice->CreateQuery( pThisTest->QueryType, 0L )))
	{
		// query type not supported
		WriteToLog(_T("Query not supported.\n"));
		Skip();
		GetLastError();
		return true;
	}

    if ( pThisTest->IdType == PTR_INVALID && !m_bIsDebug )
    {
        Skip();
        return true;
    }

	// initialize the memory check
	//GlobalMemoryStatus( &memory_before );

	CAutoRelease< CQuery > pQuery;
    if(FAILED(m_pSrcDevice->CreateQuery( pThisTest->QueryType, pThisTest->IdType == PTR_INVALID ? (CQuery**) -1L : &pQuery)))
	{
        if ( RET_VALID == pThisTest->ReturnType )
        {
		    // CreateQuery failed for a valid query!
		    WriteToLog(_T("CreateQuery failed.\n"));
		    Abort();
        }

        GetLastError();
		return true;
	}

	QUERYTHREAD_DATA thread_data;
	DWORD dwThreadId = 0;
	HANDLE hThread = 0L;
	memset( &thread_data, 0, sizeof thread_data );
    thread_data.pTest = this;
	thread_data.pQuery = pQuery;
	thread_data.QueryType = pThisTest->QueryType;
	thread_data.dwFlags = pThisTest->dwGetDataFlags;
	thread_data.pData = pData;
    thread_data.dwDataSize = pThisTest->dwDataSize;
    thread_data.dwIssue = pThisTest->dwIssueFlags;

	// run the Query test in a seperate thread so we can timeout
	if( 0L == (hThread = CreateThread(0L, 0, (LPTHREAD_START_ROUTINE)&QueryThread, &thread_data, 0, &dwThreadId)) )
	{
		// CreateQuery failed for a valid query!
		WriteToLog(_T("CreateThread failed.\n"));
		Abort();
		return true;
	}

	// give GetData up to 5 seconds before timing out
	if( WAIT_OBJECT_0 != WaitForSingleObject( hThread, 5000 ) )
	{
		CloseHandle(hThread);
		WriteToLog(_T("Query timed out.\n"));
		Fail();
		return true;
	}
	
	DWORD dwExitCode = 0;
	if( !GetExitCodeThread( hThread, &dwExitCode ) )
	{
		// CreateQuery failed for a valid query!
		WriteToLog(_T("GetThreadExitCode failed.\n"));
		Fail();
		return true;
	}

	
	if( dwExitCode == QUERYTHREAD_OK )
	{
		hr = S_OK;
	}
	else if( dwExitCode == QUERYTHREAD_FAILED )
	{
		hr = E_FAIL;
	}
	else if( dwExitCode == QUERYTHREAD_NOFLUSH )
	{
		WriteToLog(_T("Could not flush Query To Completion.\n"));
		Fail();
		return true;
	}
	else if( dwExitCode == QUERYTHREAD_EXCEPTION )
	{
		return OnException(_T("Query"), RET_INVALID == pThisTest->ReturnType && PTR_NULL != pThisTest->DataPointerType);
	}
	else
	{
		WriteToLog(_T("GetThreadExitCode returned unrecognized result.\n"));
		Fail();
		return true;
	}

	// go ahead and retrieve the query if the test case didn't already
	if( FAILED(hr) )
	{
		char * pDataDump = new char [ pThisTest->dwDataSize ];
		if( pDataDump )
		{
			GetData( pQuery, pDataDump, pThisTest->dwDataSize, GETDATA_FLUSH );
			delete [] pDataDump;
		}
		else
		{
			WriteToLog(_T("Out of memory\n"));
			bResult = false;
		}
	}

	// Don't bother validating the result for invalid parameters against the retail runtime.
	if( !m_bIsDebug && RET_INVALID == pThisTest->ReturnType )
	{
		GetLastError();
		return true;
	}

	// record the memory loss if any
	GlobalMemoryStatus( &memory_after );

	// validate the return value
	if( FAILED(hr) && RET_VALID == pThisTest->ReturnType )
	{
		WriteToLog(	_T("Query returned %s (0x%x), expected SUCCESS\n"), m_pD3D->HResultToString(hr), hr );
		bResult = false;
	}
	if( SUCCEEDED(hr) && RET_INVALID == pThisTest->ReturnType )
	{
		WriteToLog(	_T("Query returned %s (0x%x), expected FAILURE\n"), m_pD3D->HResultToString(hr), hr );
		bResult = false;
	}

	// validate no buffer overrun occurred
	if( !data.IsValid() )
	{
		WriteToLog(	_T("Query=%s overran buffer\n"), m_pD3D->HResultToString(hr) );
		bResult = false;
	}

	// validate no memory leak
/*
	if( memory_after.dwAvailVirtual < memory_before.dwAvailVirtual )
    {
        WriteToLog( _T("%d bytes leaked\n"), memory_before.dwAvailVirtual - memory_after.dwAvailVirtual );
        bResult = false;
    }
*/
	// if the test failed, notify the framework
	if( false == bResult )
		Fail();
	
	// don't let the framework fail the test case based on errors generated here
	GetLastError();

	return true;
}

//-----------------------------------------------------------------------------

CQueryConf::CQueryConf() :
    m_bIsDebug( false )
{
    SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.fMinDXVersion = 9.f;
    m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_DISABLE_DRIVER_MANAGEMENT;
	m_Options.D3DOptions.bReference = false;
    m_Options.D3DOptions.bEnumReference = false;
	m_Options.D3DOptions.bZBuffer = true;

	m_Options.TestOptions.bCompareSrcRef = false;
}

UINT CQueryConf::TestInitialize()
{
	// Query test is not valid for the reference device
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_REF)
	{
		WriteToLog("Reference device not supported.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Is this the debug runtime?
	m_bIsDebug = m_pD3D->GetDebug();

    m_PoolManaged = (D3DPOOL)POOL_MANAGED;
    if ( FLOAT_EQUAL( m_pD3D->GetVersion(), 9.1f ) )
        m_PoolManaged = (D3DPOOL)POOL_MANAGED_INTERNAL;

    return CD3DTest::TestInitialize();
}

HRESULT CQueryConf::flush( CDevice *pDevice, bool bBackBuff, bool bClear, unsigned nRepeat )
{
    HRESULT hr = S_OK;
    
    if( pDevice && nRepeat )
    {
        if ( bClear )
        {
            D3DRECT rect = { 0, 0, 1, 1 };
            hr = pDevice->Clear( 1, &rect, CLEARF_TARGET | CLEARF_ZBUFFER, 0x00000032, 1.0f, 0 );
        }

        CAutoRelease< CSurface > pSurface;
        if ( bBackBuff )
        {
            hr = pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurface );
        }
        else
        {
            hr = pDevice->GetRenderTarget( 0, &pSurface );
        }

	    if ( SUCCEEDED( hr ) )
	    {
            LOCKED_RECT locked_rect;

            if ( SUCCEEDED( hr = pSurface->LockRect( zeroMemory( &locked_rect ), 0L, 0 ) ) )
		    {
			    hr = pSurface->UnlockRect();
		    }
	    }

        return FAILED( hr ) ? hr : flush( pDevice, bBackBuff, bClear, nRepeat - 1 );
    }
    return hr;
}

float CQueryConf::areaTriangle( void *pBuffer, unsigned stride, unsigned offset )
{
    union
    {
        float *pF;
        char *pC;
        void *pB;
    };

    D3DXVECTOR3 v1( (pB = pBuffer, pC += offset * stride, pF) ), v2( (pC += stride, pF) ), v3( (pC += stride, pF) ), a, b, c;
    return 0.5f * D3DXVec3Length( D3DXVec3Cross(&c, D3DXVec3Subtract( &a, &v2, &v1 ), D3DXVec3Subtract( &b, &v3, &v1 ) ) );
}

void CQueryConf::writeToLog(const TCHAR *ptcsFmt, va_list &argList, bool bFail)
{
    TCHAR *sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);
    m_pFramework->WriteToLog(1, sTemp);
	SAFEDELETEA(sTemp);

    if ( bFail )
    {
        Fail();
    }
}
