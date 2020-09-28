/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#ifndef __HDHITLOG_H__
#define __HDHITLOG_H__

#ifndef __HIT_DEFINE_FRAMEWORK_H__
#error hdhitlog.h requires hitframework.h
#endif

#define MAX_CHARS 			MAX_MESSAGE_LENGTH

class HdHitLog : public Hit::ILog {
	
   public:		
      HdHitLog();
      bool Initialize();
      bool Uninitialize();
      void Note(
        __in __format_string const PCSTR VerboseInfoMessage, 
        IN ...);
      void Info(
        __in __format_string const PCSTR InfoMessage, 
        IN ...);
      void Warn(
        __in __format_string const PCSTR WarningMessage, 
        IN ...);
      void Pass(
        __in __format_string const PCSTR PassMessage, 
        IN ...);
      void Fail (
        __in __format_string const PCSTR FailureMessage, 
        IN ...);
      void Diag (
        __in const PCSTR FileName, 
        __in ULONG LineNumber, 
        __in __format_string const PCSTR DiagnosticsMessage, 
        IN ...);
      void Error (
        __in const PCSTR FileName, 
        __in ULONG LineNumber, 
        __in __format_string const PCSTR ErrorMessage, 
        IN ...);
      void Fatal (
        __in const PCSTR FileName, 
        __in ULONG LineNumber, 
        __in __format_string const PCSTR FatalErrorMessage, 
        IN ...);
      void AppendNewLine (__inout_bcount(MAX_CHARS) PSTR Buffer);
      void SaveMask();
      void RestoreMask();
      void SetConsoleLogging(
          __in bool Value
          );
      void SetDebuggerLogging(
          __in bool Value
          );
};

#endif // __HDHITLOG_H__
