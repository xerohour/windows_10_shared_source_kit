#pragma once

#ifdef UNICODE
#define _WIDEN2(x) L ## x  
#define _WIDEN(x) _WIDEN2(x)  
#define __TFILE__ _WIDEN(__FILE__)  
#define __TFUNCTION__ _WIDEN(__FUNCTION__)  
#else
#define __TFILE__ __FILE__
#define __TFUNCTION__ __FUNCTION__
#endif

#include <wttlogger.h>

// Simple logging wrapper used by the DisplayTopology library.
struct LoggerWrapper
{
    CWTTLogger* m_pLogger;
    LONG m_Handle;
    // failing result will be written here and can be
    // checked by the calling application if necessary.
	DWORD m_WTTResult;
    bool m_bEnableDebugBreakOnFailure;
};

#define LOG_ERROR(c, ...) \
	{\
		HRESULT _hr_;\
        m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_FAIL;\
        if( m_pLogger->m_bEnableDebugBreakOnFailure )\
        {\
            __debugbreak();\
        }\
		if( FAILED(_hr_= m_pLogger->m_pLogger->Trace( WTT_LVL_ERR, m_pLogger->m_Handle, c, WTT_ERROR_TYPE_NTSTATUS, __WFILE__, __LINE__, __VA_ARGS__ ) ) )\
		{\
			_ftprintf_s( stderr, _T("%s:%d The logger failed to log err hr=0x%08X\n"), __TFILE__, __LINE__, _hr_ );\
			_ftprintf_s( stderr, __VA_ARGS__ ); \
		}\
	}

#define LOG_ERROR_HR(c, ...) \
	{\
		HRESULT _hr_;\
        m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_FAIL;\
        if( m_pLogger->m_bEnableDebugBreakOnFailure )\
        {\
            __debugbreak();\
        }\
		if( FAILED(_hr_= m_pLogger->m_pLogger->Trace( WTT_LVL_ERR, m_pLogger->m_Handle, c, WTT_ERROR_TYPE_HRESULT, __WFILE__, __LINE__, __VA_ARGS__ ) ) )\
		{\
			_ftprintf_s( stderr, _T("%s:%d The logger failed to log err hr=0x%08X\n"), __TFILE__, __LINE__, _hr_ );\
			_ftprintf_s( stderr, __VA_ARGS__ ); \
		}\
	}

#define LOG_MESSAGE(...) \
	{\
		HRESULT _hr_;\
		if( FAILED(_hr_= m_pLogger->m_pLogger->Trace( WTT_LVL_MSG, m_pLogger->m_Handle, __VA_ARGS__ ) ) )\
		{\
			_ftprintf_s( stderr, _T("%s:%d The logger failed to log msg hr=0x%08X\n"), __TFILE__, __LINE__, _hr_ );\
			_ftprintf_s( stderr, __VA_ARGS__ ); \
		}\
	}

#define LOG_WARN(...) \
	{\
		HRESULT _hr_;\
		if( FAILED(_hr_= m_pLogger->m_pLogger->Trace( WTT_LVL_WARN, m_pLogger->m_Handle, __WFILE__, __LINE__, __VA_ARGS__ ) ) )\
		{\
			_ftprintf_s( stderr, _T("%s:%d The logger failed to log warn hr=0x%08X\n"), __TFILE__, __LINE__, _hr_ );\
			_ftprintf_s( stderr, __VA_ARGS__ ); \
		}\
	}

#define START_VARIATION(x) \
	{\
		m_pLogger->m_WTTResult = WTT_TESTCASE_RESULT_PASS;\
		HRESULT _hr_;\
		if( FAILED(_hr_ = m_pLogger->m_pLogger->StartTest(x,m_pLogger->m_Handle)) )\
		{\
			_ftprintf( stderr, _T("%s:%d The logger failed to start test case: %s.  hr=0x%08X\n"), __TFILE__, __LINE__, x, _hr_ );\
		}\
	}

#define END_VARIATION(x) \
	{\
		HRESULT _hr_;\
		if( FAILED(_hr_ = m_pLogger->m_pLogger->EndTest(x,m_pLogger->m_WTTResult,nullptr,m_pLogger->m_Handle)) )\
		{\
			_ftprintf( stderr, _T("%s:%d The logger failed to end test case: %s. result=0x%08X  hr=0x%08X\n"), __TFILE__, __LINE__, x, m_pLogger->m_WTTResult, _hr_ );\
		}\
	}

void LogMessage( wchar_t const*, ... );
void LogWarning( wchar_t const*, UINT, wchar_t const*, ... );
void LogError( 
    DWORD error, 
    wchar_t const* file, 
    wchar_t const* function, 
    UINT line, 
    wchar_t const* msg, 
    ... );

#include "DisplayTopology.h"

