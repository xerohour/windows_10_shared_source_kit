//////////////////////////////////////////////////////////////////////////
//  FrameworkCore.h
//  created:	2004/09/22
//
//  purpose: Declares the CWGFTestFramework class
//////////////////////////////////////////////////////////////////////////

#pragma once


//
// Project headers
//

#include "Shell.h"
#include "Logger.h"

#include "Test.h"
#include "StringUtils.h"
#include "VariationCore.h"
#include "TestClassFactory.h"
#include "CommandLine.h"
#include "sfi.h"
#include "FPUStateCheck.h"
#include "SettingProperties.h"
#include "ExceptionTrace.h"

//
// Test infrastructure headers
//

#include <DXGLogger.h>
#include <DXGLog.hpp>

#include <Ctfutb.h>


//////////////////////////////////////////////////////////////////////////
// CPlayList
//////////////////////////////////////////////////////////////////////////
class CPlayList
{
	private:
		struct CaseRange
		{
			CaseRange() {}
			CaseRange( const CaseRange &cr ) : nFirst( cr.nFirst ), nLast( cr.nLast ) {}
			CaseRange( size_t first, size_t last ) : nFirst( first ), nLast( last ) {}
			size_t nFirst;
			size_t nLast;
		};

		struct CaseRangeSet
		{
			void AddRange( size_t nFirst, size_t nLast );
			void RemoveRange( size_t nFirst, size_t nLast );
			void Clear() { Ranges.clear(); }
			size_t GetNumCaseRanges() const { return Ranges.size(); }
			size_t GetCaseRangeFirst( size_t nIndex ) const;
			size_t GetCaseRangeLast( size_t nIndex ) const;
			
			typedef std::vector< CaseRange > RangesType;
			RangesType Ranges;
		};
		
		struct TestPathCases
		{
			TestPathCases( CTestGroupLeafNode *node ) : pNode( node ) {}
			TestPathCases( const TestPathCases &pathCases ) : pNode( pathCases.pNode ), Cases( pathCases.Cases ) {}
			CTestGroupLeafNode *pNode;
			CaseRangeSet Cases;
		};

		typedef std::list< TestPathCases > TestCasesType;
		TestCasesType m_TestCases;
		
		typedef std::map< tcistring, TestCasesType::iterator > TestCasesMapType;
		TestCasesMapType m_TestCasesMap;

	public:
		static const size_t RANGE_END;

		struct Iterator
		{
			private:
				TestCasesType::const_iterator m_it;
				size_t m_nCaseRangeIndex;
				size_t m_nCaseIndex;

				Iterator( const TestCasesType::const_iterator &it, size_t nCaseRangeIndex, size_t nCaseIndex ) : 
					m_it( it ),
					m_nCaseRangeIndex( nCaseRangeIndex ),
					m_nCaseIndex( nCaseIndex ) {}

				size_t GetNumCaseRanges() const;
				size_t GetCaseRangeFirst( size_t nIndex ) const;
				size_t GetCaseRangeLast( size_t nIndex ) const;

			public:
				Iterator() {}
				Iterator( const Iterator &it ) : 
					m_it( it.m_it ),
					m_nCaseRangeIndex( it.m_nCaseRangeIndex ),
					m_nCaseIndex( it.m_nCaseIndex ) {}

				void BeginCases();
				void EndCases();
				void NullCase();

				void NextCase();		
				void PrevCase();

				void NextTest();		
				void PrevTest();

				// Reformat from single to multiple lines for easier C++ debugging
				size_t GetTestCaseNumber() const
				{
					return m_nCaseRangeIndex != size_t( -1 ) ? m_nCaseIndex + GetCaseRangeFirst( m_nCaseRangeIndex ) : 0;
				}
				
				//const CTestGroupPath &GetPath() const { return (*m_it).pNode->GetPath(); }
				CTestGroupLeafNode *GetNode() const
				{
					return (*m_it ).pNode;
				}
		
				bool operator==( const Iterator &it ) const { return m_it == it.m_it && m_nCaseRangeIndex == it.m_nCaseRangeIndex && m_nCaseIndex == it.m_nCaseIndex; }
				bool operator!=( const Iterator &it ) const { return m_it != it.m_it || m_nCaseRangeIndex != it.m_nCaseRangeIndex || m_nCaseIndex != it.m_nCaseIndex; }
				
				friend class CPlayList;
				friend class CWGFTestFramework;
		};

		Iterator Find( const CTestGroupPath &Path, size_t nCaseNumber ) const;
		Iterator Find( const tstring &strPath, size_t nCaseNumber ) const;

		Iterator Begin() const; //{ return Iterator( m_TestCases.begin(), 0, 1 ); }
		Iterator End() const; //{ return Iterator( m_TestCases.end(), 0, 1 ); }

		void AddTestCases( CTestGroupLeafNode *pNode, size_t nFirst = 1, size_t nLast = RANGE_END );
		void AddTestCases( CTestGroupLeafNode *pNode, const tstring &strCases );
		void AddTestBranch( const CTestGroupPath &Branch );
		void AddTestBranch( const CTestGroupNode *pNode );

		void RemoveTestCases( const CTestGroupPath &Path, size_t nFirst = 1, size_t nLast = RANGE_END );
		void RemoveTestCases( const CTestGroupPath &Path, const tstring &strCases );
		void RemoveTestBranch( const CTestGroupPath &Branch );
		void RemoveTestBranch( const CTestGroupNode *pNode );

		void Clear();

	private:
		static void BuildCaseRangeSet( CaseRangeSet &set, const tstring &strCases );
		
		TestCasesType::iterator FindTestPath( const CTestGroupPath &Path );
		TestCasesType::iterator FindTestPath( const tstring &strPath );
		TestCasesType::const_iterator FindTestPath( const CTestGroupPath &Path ) const;
		TestCasesType::const_iterator FindTestPath( const tstring &strPath ) const;
		TestCasesType::iterator AddTest( CTestGroupLeafNode *pNode );
		TestCasesType::iterator AddTest( const tstring &strPath );
};


enum WGFTEST_STATUS
{
	WGFTEST_STATUS_NOT_FOUND,
	WGFTEST_STATUS_OK,
	WGFTEST_STATUS_SKIP_TEST,
	WGFTEST_STATUS_ABORT_TEST,
	WGFTEST_STATUS_SKIP_TEST_CASE,
	WGFTEST_STATUS_ABORT_TEST_USERINITIATED,
	WGFTEST_STATUS_EXCEPTION_CAUGHT
};


enum WGFTEST_MODE
{
	WGFTEST_MODE_UNDEFINED = 0,
	WGFTEST_MODE_BVT,
	WGFTEST_MODE_WHQL,
	WGFTEST_MODE_RUNTIME,
	WGFTEST_MODE_STRESS
};


bool FromString( WGFTEST_MODE *pMode, const tstring &str ); 


//////////////////////////////////////////////////////////////////////////
// CWGFTestFramework
//////////////////////////////////////////////////////////////////////////

const UINT DEFAULT_MAX_GROUP_PASSES = 0;
const UINT DEFAULT_MAX_TOTAL_PASSES = 0;
const UINT DEFAULT_MAX_GROUP_SKIPS = 2;
const UINT DEFAULT_MAX_TOTAL_SKIPS = 20;
const UINT DEFAULT_MAX_GROUP_FAILURES = 100;
const UINT DEFAULT_MAX_TOTAL_FAILURES = 1000;
const UINT DEFAULT_MAX_GROUP_ABORTS = 100;
const UINT DEFAULT_MAX_TOTAL_ABORTS = 200;
const UINT DEFAULT_MAX_GROUP_WARNINGS = 100;
const UINT DEFAULT_MAX_TOTAL_WARNINGS = 200;

const UINT OPTION_VALUE_HIDDEN = 0x00000001;	// Option doesn't enumerate when iterating through option set.
const UINT OPTION_VALUE_CONSTANT = 0x00000002;	// Option cannot be changed dynamically at execution time.


class CWGFTestFramework : public CTestGroupManager
{
public:
	CWGFTestFramework();
	~CWGFTestFramework();
			
	const OSVERSIONINFOEX *GetOSVersionInfo() const { return &m_osVersionInfo; }
			
	// Execute
	//
	// Call this within main() to execute the test framework.
	int Execute( CShellBase *pShell );

	CShellBase *GetShell() { return m_pShell; }
	CCommandLine *GetCommandLine() { return &m_CmdLine; }

	void BeginSection( const WCHAR * szSectionName );
	void EndSection( const bool bCommit = true );

	void LogAssertion
	(
		const BOOL bCondition,
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR Assertion
	);


	void LogError
	(
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR ErrorDescription,
		const DWORD ErrorCode,
		const ERRORCODE_TYPE ErrorType
	);

	void LogError
	(
		const LPCWSTR FileName,
		const UINT FileLineNumber,
		const LPCWSTR ErrorDescription,
		const DWORD ErrorCode,
		const ERRORCODE_TYPE ErrorType,
		const LPCWSTR FailureCategory
	);


	void LogFile
	(
		const LPCWSTR FileName
	);


	void LogImage
	(
		const LPCWSTR FileName
	);


	void WriteMessage( const CHAR *szText );
	void WriteMessage( const WCHAR *szText );

	void WriteMessageF( const CHAR *szFormat, ... );
	void WriteMessageF( const WCHAR *szFormat, ... );
	
	void WriteToLog( UINT uLogLevel, const CHAR *szText );
	void WriteToLog( UINT uLogLevel, const WCHAR *szText );

	void WriteToLogF( UINT uLogLevel, const CHAR *szFormat, ... );
	void WriteToLogF( UINT uLogLevel, const WCHAR *szFormat, ... );
	
	void WriteToLog( const CHAR *szText );
	void WriteToLog( const WCHAR *szText );

	void WriteToLogF( const CHAR *szFormat, ... );
	void WriteToLogF( const WCHAR *szFormat, ... );

	// Set _NT_SYMBOL_PATH environment variable
	bool GetWTTDimensionValues( const tstring *pStrDimensionNameList, tstring *pStrDimensionValueList, UINT numDimensions);
	void SetSymbolPath();

	//
	// Overridable Methods
	//
	virtual TESTTYPE GetTestType() const = 0;
		// Returns the type of the test.
	
	virtual const TCHAR *GetName() const;
		// Returns a string containing the name of the application.
		// The default method returns the name of the executable file.
	
	virtual void InitOptionVariables() {}
		// Called by the framework prior to Setup() to allow
		// the developer to declare custom application options.
	
	virtual bool ProcessCommandLineArgument( const tstring &strArgument );
		// Called by the framework for each 
		// command line argument.  If possible use InitOptionVariables()
		// to provide automatic handling of command line options
		// by the framework.
		
	virtual bool InitTestGroups() = 0;
		// Called during framework initialization to allow 
		// the application to construct the test group tree.
	
	virtual bool Setup() { return true; }
		// Called during initialization by the framework to allow
		// any extra needed setup.
	
	virtual void Cleanup() {};
		// Called during application cleanup.
		
	virtual bool Update();
		// Called each "frame" to facilitate synchronous
		// execution of test cases and the shell.

	TEST_RESULT ExecuteTestCase();
		// Executes the current test using test case index
		// nIndex.  
		// Returns a TEST_RESULT indicating status of the
		// test case.
		
	void CleanupTestCase();
		// Calls m_pTest->Cleanup() and performs
		// post-cleanup tasks

	// CleanupTest()
	//  Deletes the current test object and performs additional test cleanup related tasks.
	//  Overridable.
	virtual void CleanupTest();

	virtual WGFTEST_STATUS OnBeyondLastTestCase();
		// Called when GoToNextTestCase() is called when the
		// current case is the last case in the test group tree.
		// Returns false if the application should exit.
		// Override this to start a new mode of cycling.
		
	virtual WGFTEST_STATUS OnBeyondFirstTestCase();
		// Called when GoToPrevTestCase() is called when the
		// current case is the first case in the test group tree.
		// Returns false if the application should exit.
		// Override this to start a new mode of cycling.

	virtual bool UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo);
		// Called when beginning execution of a device group.
		// Returns false if the application should only log
		// system info.  Ovverride this and return true to log
		// device and system info.

	DXGLOG_DEVICEINFO GetCurrentDeviceInfo() { return m_CurrentDeviceInfo; }

	CSysInfo* GetSysInfo() { return m_DXGLog.GetSysInfo(); }

	ISimpleLogController * GetLogWrapper() { return m_pLogWrapper; }

	bool StartNewDeviceGroup();
		// This function starts a new device group in the log.
		// It should be called when you switch devices if you want
		// the log to reflect this, such as in OnBeyondLastTestCase()

	
	//
	// Methods
	//	
	WGFTEST_STATUS GoToFirstTestCase();
		// Sets the current test and case index to the
		// first unfiltered test case relative to the active
		// test group path.
		
	WGFTEST_STATUS GoToLastTestCase();
		// Sets the current test and case index to the
		// last unfiltered test case relative to the active
		// test group path.
		
	WGFTEST_STATUS GoToNextTestCase();
		// Sets the current test and case index to the
		// the next unfiltered test case relative to
		// the current test case in the active test group.
		
	WGFTEST_STATUS GoToPrevTestCase();
		// Sets the current test and case index to the
		// the previous unfiltered test case relative to
		// the current test case in the active test group.

	WGFTEST_STATUS GoToTestCase( CTestGroupLeafNode *pNode, size_t nNumber );
		// Sets the current test and case number to the
		// provided explicit number and path relative to
		// the active test group.
		//
		// If the test case number is invalid or references
		// a filtered test case then the return value
		// will be false.
		//
		// Changes the execution root to strPath.
		
	WGFTEST_STATUS ResetTestCase();
		// Resets the current test case.  If the current
		// test case is no longer valid (ie: the filter
		// changed) then the return value is false.
		
	WGFTEST_STATUS SkipToNextTestGroup();
		// Skips to the first unfiltered test case in the next test
		// group in the execution order.
		
	WGFTEST_STATUS SkipToPrevTestGroup();
		// Skips to the last unfiltered test case in the previous test
		// group in the execution order.
		
	CTest *GetCurrentTest() const { return m_pTest; }
		// Returns a pointer to the current test.  If no test
		// is currently active then the return value is NULL.
		
	size_t GetCurrentTestCase() const { return m_plit != m_pl.End() ? m_plit.GetTestCaseNumber() : 0; }
		// Returns the current test case number for the
		// current test.
		
	const CTestGroupNode *GetTestGroupNode( const CTestGroupPath &Path );
	
	// GetCurrentTestPath();
	//
	// Returns a CTestGroupPath object containing the path to the current
	// test group node.  An empty path implies that no test
	// group has yet been selected.
	const CTestGroupPath &GetCurrentTestPath() const
	{
		static CTestGroupPath nullPath( _T( "" ) );
		return m_pTest ? m_pTest->GetPath() : nullPath;
	}
		
	TEST_RESULT GetLastTestResult() const { return m_LastResult; }

	void		SetLastTestConformance( float fConformance ) { m_fLastTestConformance = fConformance; };

	WGFTEST_STATUS GetStatus() const { return m_Status; }

	bool		IsNewGroup() const { return m_bIsNewGroup; }

	void		SetStatus(WGFTEST_STATUS NewStatus)
	{
		if
		(
			(
				m_Status != WGFTEST_STATUS_ABORT_TEST_USERINITIATED &&
				m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT
			) ||
			NewStatus == WGFTEST_STATUS_EXCEPTION_CAUGHT
		)
		{
			m_Status = NewStatus;
		}
	};

	bool		IsStatusContinuable();

	bool		GetTestResultPending() const { return m_LastResult == RESULT_PENDING; }
	bool		GetTestResultRetry() const { return m_LastResult == RESULT_RETRY; }
	size_t		GetPassCount() const { return m_nPassCount; }
	size_t		GetFailCount() const { return m_nFailCount; }
	size_t		GetAbortCount() const { return m_nAbortCount; }
	size_t		GetSkipCount() const { return m_nSkipCount; }

	bool GetGroupNumber( const CTestGroupPath &Path, size_t &nNumber )
	{
		if( GetGroupIndex( Path, nNumber ) )
		{
				++nNumber;
			return true;
		}

		return false;
	}
		
	//void PlayListReset();
	void PlayListClear();
	void PlayListAddBranch( const CTestGroupPath &Path );
	void PlayListAddCaseRange( const CTestGroupPath &Path, size_t nFirst = 1, size_t nLast = CPlayList::RANGE_END );

	int BeginCSV();
	void WriteVariationCSV(size_t nNumber);
	void EndCSV();
		
public:
	bool IsHelpOnly() const { return m_bHelp; }
	bool IsGenerateCSVOnly() { return m_bGenerateCSVOnly; }
	bool IsExportVCXml() { return !m_variationCoreXmlExportPath.empty(); }
	bool IsImportVCXml() { return !m_variationCoreXmlImportPath.empty(); }
	bool IsSession0() { return m_bSession0; }
	bool IsMobile()   { return GetShell()->IsMobile(); }

	int GetExecutionPriorityLevel() const { return m_priorityLevel; }
	const tstring &GetExecutionPriorityProfile() const { return m_priorityProfile; }

	//
	// Option Handling
	//
	//
	// Internal Option Handling
	//

	// COptionDesc
	template <typename _Type>
	class COptionDesc : public CSettingProperty<_Type>
	{
	public:
		typedef void (*OptionCallbackProc)( _In_ const _Type &value);

		COptionDesc( const tstring &name, _Type *pTarget, OptionCallbackProc pCallbackProc, const tstring &description, const tstring &categoryName, DWORD dwFlags, LPCTSTR szCmdLineUsage ) :
			CSettingProperty<_Type>(name, pTarget, SettingCallbackProc)
			{
			   tstring cmdLineUsage = szCmdLineUsage ? szCmdLineUsage : tstring( _T("-") ) + name;
			   SetAttribute(_T("Description"), description);
			   SetAttribute(_T("CmdLineUsage"), cmdLineUsage);
			   SetAttribute(_T("Category"), categoryName);
			   SetAttribute(_T("IsHidden"), dwFlags & OPTION_VALUE_HIDDEN ? _T("Yes") : _T("No"));
			   SetAttribute(_T("IsConstant"), dwFlags &OPTION_VALUE_CONSTANT ? _T("Yes") : _T("No"));

			   // Set the user data pointer to the static SettingCallbackProc method
			   SetUserData(pCallbackProc);
			}

		// Wrapper callback for invoking WGF-framework style option callback functions
		static void SettingCallbackProc(const _Type &value, void *pUserData)
		{
			OptionCallbackProc pOptionCallback = reinterpret_cast<OptionCallbackProc>(pUserData);
			if(pOptionCallback)
			{
				pOptionCallback(value);
			}
		}
	};

	// COptionDesc

	typedef std::vector< tstring > OptionCategoryArrayType;
	typedef std::map< tcistring, int > InvOptionCategoryMapType;

	int AddOptionCategory( LPCTSTR szName );
		// Call this method to add a custom option category.
		
	size_t GetNumOptionCategories() const { return m_OptionCategories.size(); }
		// Returns the number of option categories.
	
	const tstring &GetOptionCategoryName( size_t nIndex ) const { return m_OptionCategories[nIndex]; }
		// Returns a pointer to a string containing the name of the
		// option category at the given index.

	CPropertyManager *GetOptionsPropertyManager() { return &m_optionsManager; }
	
	template< class _ValueType >
	bool RegisterOptionVariable( LPCTSTR szName, LPCTSTR szDescription, _ValueType *pVariable, _ValueType DefaultValue, DWORD dwFlags = 0, int nCategoryID = 0, LPCTSTR szCmdLineUsage = NULL )
	{
		if( m_bLockOptionVariables )
		{
			_DPF( "Attempting to register option variable '%s' outside of appropriate InitOptionVariables() callback", szName );
			return false;
		}
				
		if(m_optionsManager.FindProperty( szName ))
		{
			_DPF( _T( "Option Variable '%s' already registered." ), szName );
			return false;
		}

		LPCTSTR pCategoryName = GetOptionCategoryName(nCategoryID).c_str();
				
		COptionDesc<_ValueType> *pOption = new COptionDesc<_ValueType>(szName, pVariable, nullptr, szDescription, pCategoryName, dwFlags, szCmdLineUsage);

		m_optionsManager.RegisterProperty(pOption);

		// Set the default value
		pOption->SetValue(DefaultValue);
			
		return true;
	}
	
	template< class _ValueType >
	bool RegisterOptionVariable( LPCTSTR szName, LPCTSTR szDescription, typename COptionDesc< _ValueType >::OptionCallbackProc pCallbackProc, _ValueType DefaultValue, DWORD dwFlags = 0, int nCategoryID = 0, LPCTSTR szCmdLineUsage = NULL )
	{
		if( m_bLockOptionVariables )
		{
			_DPF( "Attempting to register option variable '%s' outside of appropriate InitOptionVariables() callback", szName );
			return false;
		}
				
		if(m_optionsManager.FindProperty( szName ))
		{
			_DPF( _T( "Option Variable '%s' already registered." ), szName );
			return false;
		}
				
		LPCTSTR pCategoryName = GetOptionCategoryName(nCategoryID).c_str();
				
		COptionDesc<_ValueType> *pOption = new COptionDesc<_ValueType>(szName, nullptr, pCallbackProc, szDescription, pCategoryName, dwFlags, szCmdLineUsage);

		m_optionsManager.RegisterProperty(pOption);
		
		// Set the default value
		pOption->SetValue(DefaultValue);
			
		return true;
	}
		
	void UnRegisterOptionVariable( LPCTSTR szName );

	bool GetOptionValueAsString( const tstring &strName, tstring &strValue ) const;
		// Returns a tstring object containing the string representation of 
		// the current option value.  If the option doesn't exist the return value
		// is 'false', otherwise 'true'.

	bool SetOptionValueFromString( const tstring &strName, const tstring &strValue );
		// Sets the option value from the provided string representation.  If
		// the string cannot be converted or the option doesn't exist then the return
		// value is 'false', otherwise 'true'

	bool LogVerbose() const { return m_bLogVerbose; }
	bool LogAllFailures() const { return m_MaxGroupFailures == DXGLOG_MAX_TRACES; }

	WGFTEST_MODE GetTestMode() const { return m_TestMode; }

	// Static helper methods

	static void gOptionCallback_Retry( const tstring &Value );

	static void gOptionCallback_LogAll( const tstring &Value );

	static void gOptionCallback_LogAllSkips( const tstring &Value );
	static void gOptionCallback_LogSkips( const tstring &Value );
	static void gOptionCallback_MaxSkips( const tstring &Value );

	static void gOptionCallback_LogVerbose( const tstring &Value );
	static void gOptionCallback_LogAllFailures( const tstring &Value );
	static void gOptionCallback_MaxFailures( const tstring &Value );

	static void gOptionCallback_TestMode( const WGFTEST_MODE &mode);

	// These functions allow backwards capatibility to the framework "-WQHL" and "-BVT"
	// command line options.  The preferred method to accomplish this is to use 
	// "-testmode:WHQL" or "-testmode:BVT".
	static void gOptionCallback_SetTestModeWHQL( const tstring &Value );
	static void gOptionCallback_SetTestModeBVT( const tstring &Value );
	//

	static void gOptionCallback_SFI( const tstring &Value );

	static void gOptionCallback_ImportVCXML( const tstring &path );
	static void gOptionCallback_ExportVCXML( const tstring &path );

	static void gOptionCallback_ParamDefaultInit( const tstring &value );

	void OptionCallback_TestMode( const WGFTEST_MODE &mode) { m_TestMode = mode; };

	void OptionCallback_SetTestModeWHQL( const tstring &Value );
	void OptionCallback_SetTestModeBVT( const tstring &Value );

	void OptionCallback_SFI( const tstring &Value );

	void OptionCallback_Retry( const tstring &retry );

	void OptionCallback_LogAll( const tstring &Value );

	void OptionCallback_LogVerbose( const tstring &Value );
	void OptionCallback_LogAllFailures( const tstring &Value );
	void OptionCallback_MaxFailures( const tstring &Value );

	void OptionCallback_LogAllSkips( const tstring &Value );
	void OptionCallback_LogSkips( const tstring &Value );
	void OptionCallback_MaxSkips( const tstring &Value );

	bool SFIBegin() { 
#if !defined(_ARM_) && !defined(_ARM64_)
		return NO_ERROR == m_sfi.Begin();
#else   
		return true;
#endif
	}
	void SFIEnd() { 
#if !defined(_ARM_) && !defined(_ARM64_)
		m_sfi.End(); 
#endif
	}

protected:
	// SetRuntimeVersion
	//
	// This function is called to set what version of the runtime needs to be used
	// Called after InitOptionVariables but before InitTestGroups
	virtual void SetRuntimeVersion() {}

	// SetupTest()
	//
	// Called by the framework prior to calling CTest::Setup() when a test is changing.
	virtual TEST_RESULT SetupTest()
	{
		assert( GetCurrentTest() != NULL );
		return GetCurrentTest()->Setup();
	};

	void TrySetupTest(TEST_RESULT& tr);
	void TrySetupTestCase(CTest* pTest, TEST_RESULT& tr);
	
	//Implements sequential fault injection iteration
	TEST_RESULT IterateTestCase();

	// PreSetupTest()
	//
	// Called by the framework prior to setting up a test
	virtual void PreSetupTest() {}

	// PostCleanupTest()
	//
	// Called by the framework subsequent to calling CTest::Cleanup() when a test is changing.
	virtual void PostCleanupTest() {}

	// PreGenerateTestCase()
	// 
	// Called by the framework prior to calling GenerateTestCase().  This may be useful
	// if the developer wishes to cache information which may be destroyed by
	// parameter value generation.
	virtual void PreGenerateTestCase() {}
	
	// PreSetupTestCase()
	//
	// Called by the framework prior to calling CTest::SetupTestCase()
	virtual TEST_RESULT PreSetupTestCase() { return RESULT_PASS; }
	
	// PostCleanupTestCase()
	//
	// Called by the framework subsequent to calling CTest::CleanupTestCase()
	virtual void PostCleanupTestCase() {}

private:		

	class CWGFExceptionTrace : public CExceptionTrace
	{
	public:	
		CWGFExceptionTrace()    {}
		virtual void    Init()    { m_stackTrace.clear(); }

		virtual void    StackTraceCallback( const tstring &traceString ) { m_stackTrace.push_back( traceString ); }

		virtual int     FilterProc( EXCEPTION_POINTERS *ep );

		size_t          GetTraceDepth() const { return m_stackTrace.size(); }
		const tstring & GetTrace( size_t i ) const { return m_stackTrace[i]; }

	private:
		CWGFExceptionTrace(const CWGFExceptionTrace&);          //disable copy&assignment
		CWGFExceptionTrace& operator=(const CWGFExceptionTrace&);   

		std::vector< tstring > m_stackTrace;
		tstring m_exceptionTypeString;
	};

	void LogException(	const CWGFExceptionTrace& exceptioin );

	//
	// Internal Methods
	//
	bool Initialize();
		// Called during application initialization.
	
	void Uninitialize();
		// Called during application shutdown.	
		
	TEST_RESULT LocalPreExecuteTestCase();


	//
	// Logger Methods
	//	
	bool OpenLogFile();
		// Opens the log file.
		
	void MuteLog() { m_bMuteLog = true; }
		// Mutes the log.
		
	void UnMuteLog() { m_bMuteLog = false; }
		// Un-mutes the log.
		
	bool IsLogMute() const { return m_bMuteLog; }
	bool IsCodeCoverage() const { return m_bCoverage; }
		
	HRESULT LogBeginTestGroup( const TCHAR *sName, unsigned int uGroup=0 ) { return m_bNoLog ? S_OK : m_DXGLog.BeginTestGroup( sName, uGroup ); }
	HRESULT LogEndTestGroup() { return m_bNoLog ? S_OK : m_DXGLog.EndTestGroup(); }
	
	HRESULT LogBeginTestCase( const TCHAR *sName, unsigned int uTest=0 ) { return m_bNoLog ? S_OK : m_DXGLog.BeginTestCase( sName, uTest ); }
	HRESULT LogEndTestCase() { return m_bNoLog ? S_OK : m_DXGLog.EndTestCase( m_fLastTestConformance ); }
	
	void LogTestCaseResult( );

	void LocalProcessCommandLineArgument( const tstring &strArg );
	
	void LocalInitOptionVariables();

	bool LogDeviceInfo();
	
	WGFTEST_STATUS LocalGoToTestCase( CTestGroupLeafNode *pNode, size_t nNumber );

	void RecordParamValues( RESULT_CODE result );
	void LogRecordedParamValues( );

	//
	// Filter related methods
	//
	void RefreshFilter( CTest *pTest );

	//
	// Static methods
	//
protected:
	float GetSDKBuild();
	RETRY_PHASE GetRetryPhase() const;

	
	//
	// Private Data
	//

protected:
	CDXGTestLogger m_DXGLog;
	CPropertyManager m_optionsManager;
	ISimpleLogController * m_pLogWrapper;

private:
	CShellBase *m_pShell;
	CCommandLine m_CmdLine;
	DXGLOG_INFO m_LogInfo;	
	DXGLOG_DEVICEINFO m_CurrentDeviceInfo;
	tstring m_LogPath;
	tstring m_LogFileName;
	bool m_bMuteLog;
	bool m_bNoLog;
	bool m_bCoverage;
	bool m_bShowDesktopAtStartup;
	bool m_bAllowGhosting;
	tstring m_SFIOptions;
	bool m_bAllowIdleDetection;
	bool m_bHelp;
	bool m_bGenerateCSVOnly;
	bool m_bLogClean;
	bool m_bTextLog;
	bool m_bLogMessages;
	bool m_bLogToWindow;
	bool m_bLogToWindowCached;
	bool m_bLogToDebugger;
	bool m_bNoCache;
	bool m_bDBForceLog;
	bool m_bDBKeepLocalLog;
	bool m_bLogVerbose;
	size_t m_LastTestCount;
	float m_RandPoolTemp;
	int m_priorityLevel;
	tstring m_priorityProfile;
	bool m_bSession0;
	UINT m_Retries;
	UINT m_RetryCurrent;
	bool m_bInsideRetrySection;
	OSVERSIONINFOEX m_osVersionInfo;

	FILE *m_pCSVFile;
	CVariationCoreXmlBuilder *m_variationCoreXmlBuilder;
	tstring m_variationCoreXmlExportPath;
	tstring m_variationCoreXmlImportPath;

	WGFTEST_MODE m_TestMode;
	CFPUStateCheck m_fpuStateCheck;

	bool m_bDoNotCatchExceptions;
	size_t m_breakOnAllocID;

	UINT m_MaxGroupPasses;
	UINT m_MaxTotalPasses;
	UINT m_MaxGroupSkips;
	UINT m_MaxTotalSkips;
	UINT m_MaxGroupFailures;
	UINT m_MaxTotalFailures;
	UINT m_MaxGroupAborts;
	UINT m_MaxTotalAborts;
	UINT m_MaxGroupWarnings;
	UINT m_MaxTotalWarnings;

	// random percentage parameters - percent and seed
	float m_fRandPercent;
	UINT m_uSeed;

	// pool of random numbers 0 to 1
	std::vector<float> m_vRandPool;

	int m_nFailureSummaryLevel;

	ParameterNameToValueToResultToCountMap m_AllParamValues;
	ParameterNameToValueToResultToCountMap m_PairAllParamValues;


	class DPFLevel
	{
		public:
			DPFLevel() { iLevel = 1; }

			DPFLevel( int i )
			{
				iLevel = i;
				_DPFSetLevel( i );

				if (GetFramework() != NULL)
				{
					GetFramework()->m_DXGLog.SetDebugOutLevel(iLevel);
				}
			}

			DPFLevel( const DPFLevel &level )
			{
				iLevel = level.iLevel;
				_DPFSetLevel( iLevel );

				if (GetFramework() != NULL)
				{
					GetFramework()->m_DXGLog.SetDebugOutLevel(iLevel);
				}
			}

//			operator=( int i ) { iLevel = i; _DPFSetLevel( i ); }
			int iLevel;
	};
	
	DPFLevel m_dpfLevel;
	OptionCategoryArrayType m_OptionCategories;
	InvOptionCategoryMapType m_InvOptionCategoryMap;

	bool m_bLockOptionVariables;
	
	friend class CShellBase;

	std::vector< tstring >	m_filterStrings;
	CTest				*m_pTest;
	bool				m_needTestSetup;

	TEST_RESULT			m_LastResult;

	float				m_fLastTestConformance;

	WGFTEST_STATUS		m_Status;
	bool				m_bIsNewGroup;
	bool				m_bTestCaseInitialized;
	size_t				m_nPassCount;
	size_t				m_nFailCount;
	size_t				m_nAbortCount;
	size_t				m_nSkipCount;

	// Code Coverage
	HINSTANCE			m_hCovDLL;


	typedef DWORD (*COVERAGEINITIALIZE)(DWORD options);
	typedef DWORD (*COVERAGERESET)(const TCHAR* machine);
	typedef DWORD (*COVERAGESAVE)(const TCHAR* traceName, const TCHAR* machine);
	typedef DWORD (*COVERAGEUNINITIALIZE)();

	COVERAGEINITIALIZE		CoverageInitialize;
	COVERAGERESET			CoverageReset;
	COVERAGESAVE			CoverageSave;
	COVERAGEUNINITIALIZE	CoverageUninitialize;

	CPlayList	m_pl;
	CPlayList::Iterator m_plit;
	tstring m_testCaseName;

	// ITfLangBarMgr
	HINSTANCE			m_hMSCTF;

	typedef HRESULT (WINAPI *PTF_CREATELANGBARMGR)(ITfLangBarMgr**);

	DWORD	m_dwLangBarState;

#if !defined(_ARM_) && !defined(_ARM64_)
	SFI::CSFI                m_sfi;
#endif //_ARM_

	static CWGFExceptionTrace s_exceptionTrace;

private:
	// BUGBUG - If the framework ever needs to be built
	// as a DLL this method will no longer work.  
	static CWGFTestFramework *m_pThis;
	static CWGFTestFramework * WGFTESTAPI Instance() { return m_pThis; }
	friend CWGFTestFramework * WGFTESTAPI GetFramework();

	friend tstring ToString( const CWGFTestFramework::DPFLevel &level );
	friend bool FromString( CWGFTestFramework::DPFLevel *plevel, const tstring &str );
};

extern tstring ToString( const CWGFTestFramework::DPFLevel &level );
extern bool FromString( CWGFTestFramework::DPFLevel *plevel, const tstring &str );

// Global helper functions.
extern CWGFTestFramework * WGFTESTAPI GetFramework();
inline CShellBase * WGFTESTAPI GetShell() { return GetFramework()->GetShell(); }

inline int AddOptionCategory( LPCTSTR szName )
{
	return GetFramework()->AddOptionCategory( szName );
}

template< class _ValueType >
bool RegisterOptionVariable( LPCTSTR szName, LPCTSTR szDescription, _ValueType *pVariable, _ValueType DefaultValue, DWORD dwFlags = 0, int nCategoryID = 0, LPCTSTR szCmdLineUsage = NULL )
{
	return GetFramework()->RegisterOptionVariable( szName, szDescription, pVariable, DefaultValue, dwFlags, nCategoryID, szCmdLineUsage );
}
