#include "AbstractTest.h"

tstring ToString( TestResult::Type const& t )
{
    switch( t )
    {
        case TestResult::Pass:
            return _T("Pass");
        case TestResult::Fail:
            return _T("Fail");
        case TestResult::Skip:
            return _T("Skip");
        case TestResult::Abort:
            return _T("Abort");
        default:
            return _T("Unknown");
    }
}

AbstractTest::AbstractTest( LoggerWrapper* pLogger )
    : m_pLogger(pLogger)
    , m_Argc(0)
    , m_ppArgv(nullptr)
{
}

AbstractTest::~AbstractTest()
{
}

void AbstractTest::LogParameterValues()
{
    tstringstream s;
    bool empty = true;
        
    std::for_each( m_ParameterArray.begin(), m_ParameterArray.end(), [&]( CTestCaseParameterBase* pParam )
    {
        if( (pParam->IsActive() || pParam->DefaultValueWasSet()) 
            && 0 != pParam->GetNumParameterFactors() )
        {
            if( !empty )
            {
                s << _T("\n");
            }
            else
            {
                empty = false;
            }
            s << pParam->GetName() << _T(" = ") << pParam->GetValueAsString();
        }
    } );

    LOG_MESSAGE( s.str().c_str() );
}

bool AbstractTest::InternalRunTest()
{
    TestResult::Type tr;

    TCHAR buf[256];
    _stprintf_s( buf, 256, _T("Setup") );
    tr = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::SetupTest, this ) );
    if( TestResult::Pass != tr )
    {
        if( TestResult::Fail == tr || TestResult::Abort == tr )
        {
            LOG_ERROR( E_FAIL, _T("Setup test result: %s due to previous errors."), ToString( tr ).c_str() );
            return false;
        }
        // skip
        m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_SKIPPED;
        return true;
    }
    
    _stprintf_s( buf, 256, _T("Parameter Initialization") );
    tr = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::InitializeParameters, this ) );
    if( m_pLogger->m_WTTResult != WTT_TESTCASE_RESULT_PASS
        || TestResult::Pass != tr )
    {
        LOG_ERROR( E_FAIL, _T("Failed to run test cases due to failure during %s."), buf );
        return false;
    }

    {
        auto runVariation = [&]() -> TestResult::Type
        {
            // log file needs the parameters
            LogParameterValues();

            tr = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::SetupTestCase, this ) );
            if( TestResult::Pass != tr )
            {
                // log finish
                if( TestResult::Skip != tr )
                {
                    LOG_ERROR( E_FAIL, _T("Failed during SetupTestCase.") );
                }
                return tr;
            }

            tr = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::ExecuteTestCase, this ) );

            if( TestResult::Abort != tr )
            {
                TestResult::Type res = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::CleanupTestCase, this ) );
                if( res == TestResult::Abort && TestResult::Pass == tr )
                {
                    LOG_ERROR( E_FAIL, _T("Failed during CleanupTestCase.") );
                    return res;
                }
            }

            return tr;
        };

        auto f = [&]() -> void
        {
            size_t const variations = GetNumVariations();

            bool executedNothing = true;
            for( size_t i = 0; i < variations; ++i )
            {
                // the generator is not zero based
                if( !GenerateVariation(i+1) ) 
                {
                    continue;
                }

                executedNothing = false;

                _stprintf_s( buf, 256, _T("TestCase: %08d"), i+1 );
                tr = ExecuteVariation( buf, runVariation );
                
                if( TestResult::Abort == tr )
                {
                    return;
                }
            }

            if( variations < 1 
                    || (1 == variations && executedNothing) )
            {
                LOG_ERROR( E_FAIL, _T("Failed to run any test cases due to zero variations being provided by the test.") );
                return;
            }
        };

        f();
    }

    _stprintf_s( buf, 256, _T("Cleanup") );
    tr = ExecuteAndCatchAllExceptions( std::bind( &AbstractTest::CleanupTest, this ) );
    if( TestResult::Pass != tr )
    {
        LOG_ERROR( E_FAIL, _T("Failed to properly cleanup test due to previous failures.") );
        return false;
    }
    
    return true;
}

bool AbstractTest::RunTest()
{
    TCHAR buf[256];
    _tcscpy_s( buf, 256, GetTestName().c_str() );
    START_VARIATION( buf );
    bool ret = InternalRunTest();
    END_VARIATION( buf );
    return ret;
}

void AbstractTest::StartVariation( TCHAR const* pName )
{
    START_VARIATION( const_cast<TCHAR*>(pName) );
}

void AbstractTest::EndVariation( TCHAR const* pName, TestResult::Type result )
{
    switch( result )
    {
        case TestResult::Pass:
            m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_PASS;
            break;
        case TestResult::Fail:
            m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_FAIL;
            break;
        case TestResult::Skip:
            m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_SKIPPED;
            break;
        case TestResult::Abort:
            m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_BLOCKED;
            break;
    }
    END_VARIATION( const_cast<TCHAR*>(pName) );
}

HEX32::HEX32( UINT32 v )
    : value(v)
{
}

HEX32::operator UINT32() const
{
    return value;
}

std::basic_string<TCHAR> ToString( HEX32 const& v )
{
    TCHAR buf[16];
    _stprintf_s( buf, _T("0x%X"), v );
    return buf;
}

bool FromString( HEX32* pV, std::basic_string<TCHAR> const& str )
{
    return 1 == _stscanf_s( str.c_str(), _T("%d"), pV );
}

bool operator ==( HEX32 const& v0, HEX32 const& v1 )
{
    return v0.value == v1.value;
}