#pragma once
#include <windows.h>
#include <tchar.h>
#include <functional>
#include <set>
#include "VariationCore.h"
#include "Logging.h"
#include "stringutils.h"
#include "helperfunctions.h"
#include "TestResult.h"

struct AbstractTest : protected CVariationGenerator
{
    LoggerWrapper* m_pLogger;
    int m_Argc;
    TCHAR *const* m_ppArgv;

    virtual tstring GetTestName() = 0;

    AbstractTest( LoggerWrapper* pLogger );
    virtual ~AbstractTest();

    bool RunTest();

    protected:
        virtual void InitializeParameters() = 0;
        virtual TestResult::Type SetupTest() = 0;
        virtual TestResult::Type SetupTestCase() = 0;
        virtual TestResult::Type ExecuteTestCase() = 0;
        virtual void CleanupTestCase() = 0;
        virtual void CleanupTest() = 0;

    private:
        void StartVariation( TCHAR const* pName );
        void EndVariation( TCHAR const* pName, TestResult::Type result );

        template<typename T>
            struct GetReturnType
            {
                typedef typename T::result_type return_type;
            };

        template<>
            struct GetReturnType<void (*)(void)>
            {
                typedef void return_type;
            };

        template<typename R>
            struct GetReturnType<R (*)(void)>
            {
                typedef R return_type;
            };
        
        template<typename T, bool B = std::is_void<GetReturnType<T>::return_type>::value>
            struct ExecuteAndReturn;

        template<typename T>
            struct ExecuteAndReturn<T,false>
            {
                static TestResult::Type Invoke( T& t )
                {
                    return t();
                }
            };

        template<typename T>
            struct ExecuteAndReturn<T,true>
            {
                static TestResult::Type Invoke( T& t )
                {
                    static_assert( std::is_void<decltype(t())>::value, "templating error for return type." );
                    t();
                    return TestResult::Pass;
                }
            };

        template<typename T>
            TestResult::Type ExecuteAndCatchStandardExceptions( T& func )
            {
                try
                {
                    return ExecuteAndReturn<T>::Invoke( func );
                }
                catch(...)
                {
                }
                LOG_ERROR( E_FAIL, _T("Unhandled standard exception occurred.  Details not implemented.") );
                return TestResult::Fail;
            }

        template<typename T>
            TestResult::Type ExecuteAndCatchAllExceptions( T& func )
            {
                __try
                {
                    return ExecuteAndCatchStandardExceptions( func );
                }
                __except(true)
                {
                }
                LOG_ERROR( E_FAIL, _T("Unhandled native exception occurred.  Details not implemented.") );
                return TestResult::Abort;
            }

        template<typename T>
            TestResult::Type ExecuteVariation( TCHAR const* pName, T& func )
            {
                StartVariation( pName );
                TestResult::Type ret = func();
                EndVariation( pName, ret );
                return ret;
            }

        template<typename T>
            TestResult::Type SafeExecuteVariation( TCHAR const* pName, T& func )
            {
                StartVariation( pName );
                TestResult::Type ret = ExecuteAndCatchAllExceptions( func );
                EndVariation( pName, ret );
                return ret;
            }

        bool InternalRunTest();
        void LogParameterValues();

};


struct HEX32
{
    UINT32 value;

    explicit HEX32( UINT32 v = 0 );

    operator UINT32() const;
};

std::basic_string<TCHAR> ToString( HEX32 const& );
bool FromString( HEX32*, std::basic_string<TCHAR> const& );
bool operator ==( HEX32 const&, HEX32 const& );

