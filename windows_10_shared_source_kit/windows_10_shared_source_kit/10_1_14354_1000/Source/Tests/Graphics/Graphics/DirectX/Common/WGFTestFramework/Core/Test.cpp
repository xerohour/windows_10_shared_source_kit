//////////////////////////////////////////////////////////////////////////
//  Test.cpp
//  created:	2004/08/30
//
//  purpose: Contains implementation of CTest class.
//////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "Test.h"
#include "Shell.h"
#include "Framework.h"

#include <strsafe.h>

BEGIN_NAMED_VALUES( RETRY_PHASE )
	NAMED_VALUE( _T( "Initial" ), RETRY_PHASE_INITIAL )
	NAMED_VALUE( _T( "Retry" ), RETRY_PHASE_RETRY )
	NAMED_VALUE( _T( "Reset and retry" ), RETRY_PHASE_RESET_AND_RETRY )
END_NAMED_VALUES( RETRY_PHASE )

//////////////////////////////////////////////////////////////////////////
// TEST_RESULT
//////////////////////////////////////////////////////////////////////////
bool TEST_RESULT::s_bDebugOnFail( false );

TEST_RESULT::TEST_RESULT( RESULT_CODE rc ) :
	m_rc( rc )
{
	if( s_bDebugOnFail && rc == RESULT_FAIL && ( !GetShell() || GetShell()->AllowDebugOnFail() ) )
	{
		DebugBreak();
	}
}

//////////////////////////////////////////////////////////////////////////
// CTest
//////////////////////////////////////////////////////////////////////////
CTest::CTest()  : m_bLockParameters( true )
{
}

CTest::~CTest()
{
}

TEST_RESULT CTest::Setup()
{
	return RESULT_PASS;
}

void CTest::LocalCleanup()
{
	Cleanup();
}

TEST_RESULT CTest::SetupTestCase()
{
	return RESULT_PASS;
}

TEST_RESULT CTest::ExecuteTestCase()
{
	return RESULT_PASS;
}

TEST_RESULT CTest::PostExecuteTestCase()
{
    return RESULT_PASS;
}

void CTest::CleanupTestCase()
{
}

tstring CTest::GenerateTestCaseName()
{
	return GetPath().ToString();
}

void CTest::BeginSection( const WCHAR * szSectionName )
{
	GetFramework()->BeginSection( szSectionName );
}

void CTest::EndSection( const bool bCommit /* = true */ )
{
	GetFramework()->EndSection( bCommit );
}

void CTest::LogAssertion
(
	const BOOL bCondition,
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR Assertion
)
{
	GetFramework()->LogAssertion
	(
		bCondition,
		FileName,
		FileLineNumber,
		Assertion
	);
}

void CTest::LogError
(
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR ErrorDescription,
	const DWORD ErrorCode,
	const ERRORCODE_TYPE ErrorType
)
{
	GetFramework()->LogError
	(
		FileName,
		FileLineNumber,
		ErrorDescription,
		ErrorCode,
		ErrorType
	);
}

void CTest::LogError
(
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR ErrorDescription,
	const DWORD ErrorCode,
	const ERRORCODE_TYPE ErrorType,
	const LPCWSTR FailureCategory
)
{
	GetFramework()->LogError
	(
		FileName,
		FileLineNumber,
		ErrorDescription,
		ErrorCode,
		ErrorType,
		FailureCategory
	);
}

void CTest::LogFile
(
	const LPCWSTR FileName
)
{
	GetFramework()->LogFile
	(
		FileName
	);
}

void CTest::LogImage
(
	const LPCWSTR FileName
)
{
	GetFramework()->LogImage
	(
		FileName
	);
}

void CTest::WriteToLog( const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteToLog( 1, szBuffer );
}

void CTest::WriteToLog( const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteToLog( 1, szBuffer );
}

void CTest::WriteMessage( const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteMessage( szBuffer );
}

void CTest::WriteMessage( const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteMessage( szBuffer );
}

void CTest::WriteToLog( UINT uLogLevel, const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteToLog( uLogLevel, szBuffer );
}

void CTest::WriteToLog( UINT uLogLevel, const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list arglist;
	va_start( arglist, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		arglist
	);
	va_end( arglist );

	GetFramework()->WriteToLog( uLogLevel, szBuffer );
}

CFilterHandle CTest::SkipMatching( CFilterHandle filter )
{
	m_skipRoot = filter;
	tstring filterExpr = filter->ToString();
	WriteToLog( 0, _T( "SKIPPING CASES MATCHING: %s" ), filterExpr.c_str() );
	return m_skipRoot;
}

void CTest::BeginTestParameters()
{
	m_bLockParameters = false;
}

void CTest::EndTestParameters()
{
	m_bLockParameters = true;
	if( !m_RootFactor.Valid() )
	{
		// User never specified test factor expression so attempt
		// to create a default expression using nested cycling.
		if( m_ParameterArray.size() != 0 )
		{
			for( size_t i = m_ParameterArray.size() - 1; i != (size_t) -1; --i )
			{
				CTestCaseParameterBase *pParam = m_ParameterArray[i];
				if( pParam )
				{
					for( size_t j = pParam->GetNumParameterFactors() - 1; j != (size_t) -1; --j )
					{
						CTestCaseParameterFactorValuesBase *pParamValues = pParam->GetParameterFactorValues( j );
						testfactor::RFactor rf = pParamValues->GetTestFactor();
						if( rf.Valid() )
						{
							if( m_RootFactor.Valid() )
								m_RootFactor = rf * m_RootFactor;
							else
								m_RootFactor = rf;
							m_RootFactor->Set( 0 ); // Initialize factors
						}
					}
				}
			}
		}
	}

	// If the root factor is still invalid then simply
	// create a single-state test factor object at the root
	// so there will always be at least one test case.
	if( !m_RootFactor.Valid() )
	{
		m_RootFactor = testfactor::NewFactor( 1 );
		m_RootFactor->Set( 0 );
	}
    
	// Set the priority profile
	SetActivePriorityProfile( GetFramework()->GetExecutionPriorityProfile() );
}

void CTest::AddParameterBase( CTestCaseParameterBase *pParameter )
{
    assert( !m_bLockParameters && _T( "AddParameter() should only be after BeginTestParameters()" ) );

    if( !m_bLockParameters && pParameter )
    {
    CVariationGenerator::AddParameterBase(pParameter);    
    }
}

void CTest::SetRootTestFactor(	testfactor::RFactor RootFactor )
{
    assert( !m_bLockParameters && _T( "SetRootTestFactor() should only be called after BeginTestParameters()" ) );

    if( !m_bLockParameters )
    {
        CVariationGenerator::SetRootTestFactor(RootFactor); 
    }
}

testfactor::RFactor CTest::GetRootTestFactor()
{
    assert( !m_bLockParameters && _T( "GetRootTestFactor() should only be called after BeginTestParameters()" ) );
    return CVariationGenerator::GetRootTestFactor();
}

void CTest::ErrorOut( const TCHAR *szFormat, ... )
{
	TCHAR szBuffer[ 4096 ] = _T("");

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintf
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );
	
	// Write the string to the debug output
	OutputDebugString( szBuffer );

	// Write the error to the shell (or to stderr if no shell is present).
	CShellBase *pShell = GetShell();

	if( pShell && (GetFramework()->IsMobile() == FALSE))
		pShell->ErrorText( szBuffer );
	else
		_ftprintf( stderr, _T("%s\n"), szBuffer ); // Write the error to stderr
	
	// Send error to log
	GetFramework()->WriteToLog( 1, szBuffer );
}

void CTest::ThrowFailure(HRESULT hr, const TCHAR * Message)
{
    if (FAILED(hr))
    {
        WriteToLog(Message);
        WriteToLog(" HRESULT == %08x\n", hr);
        throw RESULT_FAIL;
    }
}

void CTest::ThrowSuccess(HRESULT hr, const TCHAR * Message)
{
    if (SUCCEEDED(hr))
    {
        WriteToLog(Message);
        WriteToLog("\n");
        throw RESULT_FAIL;
    }
}

void CTest::ThrowFailureWin32(UINT err, const TCHAR * Message)
{
    if (ERROR_SUCCESS != err)
    {
        WriteToLog(Message);
        WriteToLog(" WIN32 Error Code == %d\n", err);
        throw RESULT_FAIL;
    }
}
