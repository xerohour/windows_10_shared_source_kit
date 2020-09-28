/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <hitframework.h>
#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "install.h"
#include "uaatest.h"
#include "HdRegTest.h"
#include "HdHitLog.h"
#include "HdHitPlugin.h"
#include "hdaudiomemoryspace.h"
#include "controller.h"
#include "hdhitlog.h"
#include "log.h"

HdHitLog::HdHitLog() 
{

}

bool HdHitLog::Initialize () 
//Initializes the Log object and readies it for subsequent calls   
{
	return TRUE;						
}

bool HdHitLog::Uninitialize () 
//Releases any resources of the Log object.    
{
	return TRUE;
}

void HdHitLog::AppendNewLine (__inout_bcount(MAX_CHARS) PSTR Buffer)
{
    HRESULT hr;
    size_t StringLength =0;

    hr = StringCchLengthA (Buffer, MAX_CHARS, &StringLength);
    if (FAILED(hr)) { return; }

    if (StringLength == MAX_CHARS - 1) {
        Buffer[StringLength-1] = '\n';
        Buffer[StringLength] = '\0';				
    } 
    else {
        Buffer[StringLength] = '\n';
        Buffer[StringLength+1] = '\0';				
    }

}

void HdHitLog::Note
(
    __in __format_string const PCSTR VerboseInfoMessage, 
    IN ...
 )
//Logs an informational diagnostics message
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer [MAX_CHARS];

    va_start (argList, VerboseInfoMessage);
    hr = StringCchVPrintfA (TempBuffer, MAX_CHARS, VerboseInfoMessage, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);
    AppendNewLine (TempBuffer);
    SLOG (eInfo3, "%s", TempBuffer);
}

	
void HdHitLog::Info
(
    __in __format_string const PCSTR InfoMessage, 
    IN ...
)
//Logs information about the progress of a test.
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer [MAX_CHARS];

    va_start (argList, InfoMessage);
    hr = StringCchVPrintfA (TempBuffer, MAX_CHARS, InfoMessage, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);

    AppendNewLine (TempBuffer);
    SLOG (eInfo2, "%s", TempBuffer);
    
}

void HdHitLog::Warn
(
    __in __format_string const PCSTR WarningMessage, 
    IN ...
)
//Logs information about a situation which possibly needs attention but is not critical and ignoring the message does not negatively affect the run    
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer [MAX_CHARS];

    va_start (argList, WarningMessage);
    hr = StringCchVPrintfA (TempBuffer, MAX_CHARS, WarningMessage, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);
	
    AppendNewLine (TempBuffer);
    XLOG(XWARN, eWarn1, "%s", TempBuffer);

}

void HdHitLog::Pass
(
    __in __format_string const PCSTR PassMessage, 
    IN ...
)
//Logs a successful result of a test case.    
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer [MAX_CHARS];

    va_start (argList, PassMessage);
    hr = StringCchVPrintfA (TempBuffer, MAX_CHARS, PassMessage, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);
	
    AppendNewLine (TempBuffer);
    SLOG (eInfo1, "%s", TempBuffer);

}

void HdHitLog::Fail
(
    __in __format_string const PCSTR FailureMessage, 
    IN ...
)
//Logs an unsuccessful result of a test case.    
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer [MAX_CHARS];

    va_start (argList, FailureMessage);
    hr = StringCchVPrintfA (TempBuffer, MAX_CHARS, FailureMessage, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);

    AppendNewLine (TempBuffer);
    SLOG (eError, "%s", TempBuffer);
}

void HdHitLog::Diag
(
    __in const PCSTR FileName, 
    __in ULONG LineNumber, 
    __in __format_string const PCSTR DiagnosticsMessage, 
    IN ...
)
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer1 [MAX_CHARS];	
    CHAR TempBuffer2 [MAX_CHARS];	

    hr = StringCchPrintfA (TempBuffer1, MAX_CHARS, "DIAGS: %s (%s, %d)", DiagnosticsMessage, FileName, LineNumber);
    if (FAILED(hr)) { return; }

    va_start (argList, DiagnosticsMessage);
    hr = StringCchVPrintfA (TempBuffer2, MAX_CHARS, TempBuffer1, argList);
    if (FAILED(hr)) { return; }
    va_end (argList);

    AppendNewLine (TempBuffer2);
    SLOG (eInfo2, "%s", TempBuffer2);

}

void HdHitLog::Error
(
    __in const PCSTR FileName, 
    __in ULONG LineNumber, 
    __in __format_string const PCSTR ErrorMessage, 
    IN ...
)
//Logs a non-fatal error situation inside HIT or the host application    
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer1 [MAX_CHARS];	
    CHAR TempBuffer2 [MAX_CHARS];	

    hr = StringCchPrintfA (TempBuffer1, MAX_CHARS, "ERROR: %s (%s, %d)", ErrorMessage, FileName, LineNumber);
    if (FAILED(hr)) { return; }

    va_start (argList, ErrorMessage);
    hr = StringCchVPrintfA (TempBuffer2, MAX_CHARS, TempBuffer1, argList);		
    if (FAILED(hr)) { return; }
    va_end (argList);

    AppendNewLine (TempBuffer2);
    SLOG (eError, "%s", TempBuffer2);

}

void HdHitLog::Fatal
(
    __in const PCSTR FileName, 
    __in ULONG LineNumber, 
    __in __format_string const PCSTR FatalErrorMessage, 
    IN ...
)
//Logs a fatal error situation.     
{
    HRESULT hr;
    va_list argList;
    CHAR TempBuffer1 [MAX_CHARS];	
    CHAR TempBuffer2 [MAX_CHARS];	

    hr = StringCchPrintfA (TempBuffer1, MAX_CHARS, "FATAL: %s (%s, %d)", FatalErrorMessage, FileName, LineNumber);
    if (FAILED(hr)) { return; }

    va_start (argList, FatalErrorMessage);
    hr = StringCchVPrintfA (TempBuffer2, MAX_CHARS, TempBuffer1, argList);		
    if (FAILED(hr)) { return; }
    va_end (argList);

    AppendNewLine (TempBuffer2);
    XLOG (XFAIL, eFatalError, "%s", TempBuffer2);

}

// no-ops			
void HdHitLog::SaveMask()
{
    SLOG (eInfo1, "HdHitLog::SaveMask is a no-op");
}

void HdHitLog::RestoreMask()
{
    SLOG (eInfo1, "HdHitLog::RestoreMask is a no-op");
}

void HdHitLog::SetConsoleLogging(
    __in bool Value
)
{
    SLOG (eInfo1, "HdHitLog::SetConsoleLogging(%u) is a no-op", Value);
}

void HdHitLog::SetDebuggerLogging(
    __in bool Value
)
{
    SLOG (eInfo1, "HdHitLog::SetDebuggerLogging(%u) is a no-op", Value);
}
