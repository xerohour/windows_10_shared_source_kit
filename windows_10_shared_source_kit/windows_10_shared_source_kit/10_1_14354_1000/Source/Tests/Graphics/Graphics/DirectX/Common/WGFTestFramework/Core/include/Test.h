//////////////////////////////////////////////////////////////////////////
//  Test.h
//  created:	2004/08/30
//
//  purpose: Contains declaration of CTest class.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "TestGroupManager.h"
#include "VariationGenerator.h"
#include "WindowsTestFailureCategoryStrings.h"
#include "Direct3DFailureCategoryStrings.h"

// Use the DECLARE_TEST_CLASS macro inside a class definition for a class
// derived from CTest that is intended to be created by the CTestFactory
// class.
#pragma deprecated( DECLARE_TEST_CLASS )
#define DECLARE_TEST_CLASS( classname ) /*\
	static CTest * WGFTESTAPI AllocateMe() { return new classname(); }*/
	
#pragma deprecated( REGISTER_TEST_CLASS )
#define REGISTER_TEST_CLASS( classname )

//////////////////////////////////////////////////////////////////////////
// RESULT_CODE
//
// Return value from CTest::ExecuteTestCase()
//////////////////////////////////////////////////////////////////////////
enum RESULT_CODE
{
	RESULT_NONE,		
	RESULT_PASS,		// Indicates passing result.
	RESULT_FAIL,		// Indicates failing result.
	RESULT_SKIP,		// Indicates test case skip.
	RESULT_ABORT,		// Indicates that the test group should be aborted
	RESULT_PENDING,		// Indicates the result is pending.  
						// Can be used for Multi-Frame (manual) tests or
						// may mean that the test result is determined
						// by a latter stage (Image compare?) in the test frame.
	RESULT_BLOCKED,		// Indicates that the individual test case could
						// not be completed.
	RESULT_RETRY,		// Indicates that the individual test case failed
						// but we will retry that test case. This is only used
						// by the framework, tests themselves should not set this.
};

enum RETRY_PHASE
{
	RETRY_PHASE_INITIAL,			// Not in a retry phase
	RETRY_PHASE_RETRY,				// During this retry phase, the framework will simply call the test's 
									// SetupTestCase(), ExecuteTestCase(), and CleanupTestCase().
	RETRY_PHASE_RESET_AND_RETRY,	// During this retry phase, the framework will reset the test (Example: For graphics, recycle the device(s) between retries
									// and call the test's Cleanup(), Setup(), SetupTestCase(), ExecuteTestCase(), and CleanupTestCase().
};

DECLARE_NAMED_VALUES( RETRY_PHASE );

enum SKIP_OP
{
	SKIP_EQUALS,
	SKIP_GREATER_THAN,
	SKIP_LESS_THAN,
	SKIP_GREATER_THAN_EQUALS,
	SKIP_LESS_THAN_EQUALS
};

class TEST_RESULT
{
public:
	TEST_RESULT() : m_rc( RESULT_NONE ) {}
	TEST_RESULT( RESULT_CODE rc );
	TEST_RESULT( const TEST_RESULT &tr ) : m_rc( tr.m_rc ) {}
	
	bool operator==( RESULT_CODE rc ) const { return m_rc == rc; }
	bool operator==( const TEST_RESULT &tr ) const { return m_rc == tr.m_rc; }
	
	operator RESULT_CODE() { return m_rc; }
	RESULT_CODE GetResultCode() const { return m_rc; }
	
private:
	RESULT_CODE m_rc;
	
	static bool s_bDebugOnFail;
	friend class CWGFTestFramework;
};
	
//////////////////////////////////////////////////////////////////////////
// CTest
//////////////////////////////////////////////////////////////////////////
class CTest : public CVariationGenerator
{
public:
	//
	// Construction
	//
    CTest();
	virtual ~CTest();
	
	//
	// Initialization
	//
	virtual void InitTestParameters() {}
		// Called once by the framework prior to Setup(). This
		// method is only called if the test has no parameters.  Override this
		// method to add parameters to the test.  The default implementation
		// adds a NULL parameter to indicate a single test case with no
		// named parameters.
	
	virtual void InitOutputParameters() {}

	virtual TEST_RESULT Setup();
		// Called by the framework each time the test becomes active.
		// Override this method to perform any necessary test-wide initialization.
		// Returns true if the test should continue to the execute loop.
		// NOTE: Do NOT use this method to add parameters to the test.
		// (see InitTestParameters()).
		
	virtual void Cleanup() {}
		// Called by the framework each time a test is about to become
		// inactive.  Override this method to perform any necessary
		// cleanup.	
	
	//
	// Test case execution
	//
	virtual TEST_RESULT SetupTestCase();
		// Called by the framework prior to test case
		// execution.  Override this method to initialize
		// the state of the test case before execution.
		
	virtual TEST_RESULT ExecuteTestCase();
		// Called by the framework to execute the current
		// test case.
		// Returns a test result indicating the status
		// of the test case.

    virtual TEST_RESULT PostExecuteTestCase();
        // Called by the framework immediately after executing
        // the current test case, before cleanup.
        // Use to verify state that is common between tests.
		
	virtual void CleanupTestCase();
		// Called by the framework after test case 
		// execution.  Override this method to perform any 
		// required cleanup.
		
	// GenerateTestCaseName()
	// Called by the framework after generating a test case but before test
	// case is setup or executed.  The default return value is a string
	// representing the test group path containing the test case.
	virtual tstring GenerateTestCaseName();

    //
	// Filter methods
	//
	CFilterHandle GetRootFilter() const { return m_filterRoot; }
	CFilterHandle GetSkipMatchingFilter() const { return m_skipRoot; }
	CFilterHandle SkipMatching( CFilterHandle skipFilter );
	
    //
    // VariationGenerator overrides
    //
    virtual void SetRootTestFactor(	testfactor::RFactor RootFactor );
    virtual testfactor::RFactor GetRootTestFactor();
    virtual void ErrorOut( const TCHAR *szFormat, ... );

	//
	// Logger methods.
	//
	static void BeginSection( const WCHAR * szSectionName );
	static void EndSection( const bool bCommit = true );


	static void LogAssertion
	(
		const BOOL bCondition,
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR Assertion
	);


	static void LogError
	(
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR ErrorDescription,
		const DWORD ErrorCode,
		const ERRORCODE_TYPE ErrorType
	);

	static void LogError
	(
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR ErrorDescription,
		const DWORD ErrorCode,
		const ERRORCODE_TYPE ErrorType,
		const LPCWSTR FailureCategory
	);


	static void LogFile
	(
		const LPCWSTR FileName
	);


	static void LogImage
	(
		const LPCWSTR FileName
	);


	static void WriteToLog( const CHAR *szFormat, ... );
	static void WriteToLog( const WCHAR *szFormat, ... );

	static void WriteToLog( UINT uLogLevel, const CHAR *szFormat, ... );
	static void WriteToLog( UINT uLogLevel, const WCHAR *szFormat, ... );
		
    //
    // Helpers for try-throw-catch style tests:
    //
    static void ThrowFailure(HRESULT hr, const TCHAR * Message);
    static void ThrowSuccess(HRESULT hr, const TCHAR * Message);
    static void ThrowFailureWin32(UINT err, const TCHAR * Message);

    //
	// Message methods.
	//
	void WriteMessage( const CHAR *szFormat, ... );
	void WriteMessage( const WCHAR *szFormat, ... );
		
	//
	// Accessors
	//
	const CTestGroupPath &GetPath() const { return m_pNode->GetPath(); }
	const tstring &GetTestName() const { return m_pNode->GetName(); }
	const tstring &GetTestDesc() const { return m_pNode->GetTestDesc(); }


private:
	bool m_bLockParameters;	
	void SetRootFilter( CFilterHandle filter ) { m_filterRoot = filter; }

    //
    // VariationGenerator overrides
    //
    virtual void BeginTestParameters();
    virtual void EndTestParameters();
    virtual void AddParameterBase( CTestCaseParameterBase *pParameter );

	void LocalCleanup();
		// Called by the framework to perform test cleanup.
	
public:
	// Declare the CTest::AllocProc type.
	typedef CTest *(WGFTESTAPI *AllocProc)();

protected:
	CTestGroupLeafNode *m_pNode;
	CFilterHandle 		m_filterRoot;
	CFilterHandle		m_skipRoot;
	friend class CTestGroupLeafNode; // Needed so that the leaf node can set the m_Path member.
	friend class CWGFTestFramework;

};

template< class _TestClass >
CTest * WGFTESTAPI AllocTest() { return new _TestClass(); }


