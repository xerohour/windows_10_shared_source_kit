//////////////////////////////////////////////////////////////////////
// File:  HelperMacros.h
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// General-purpose macros for DirectImage tests.
//
// History:
// 10 Jun 2011  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// String manipulation macros
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Macro:  CONCATENATE
//
// Purpose:
// Concatenates two tokens into a single token.
///////////////////////////////////////////////////////
#if !defined(CONCATENATE2)
#define CONCATENATE2(x, y)  x ## y
#endif

#if !defined(CONCATENATE)
#define CONCATENATE(x, y)   CONCATENATE2(x, y)
#endif


///////////////////////////////////////////////////////
// Macro:  PASTER
//
// Purpose:
// Concatenates two tokens into a single token.
//
// Additional information:
// This is a legacy version of CONCATENATE which will
// eventually be removed.
///////////////////////////////////////////////////////
#if !defined(PASTER2)
#define PASTER2(x, y)  x ## y
#endif

#if !defined(PASTER)
#define PASTER(x, y)   PASTER2(x, y)
#endif


///////////////////////////////////////////////////////
// Macro:  TEXTIFY
//
// Purpose:
// Converts a sequence of characters into a string.
///////////////////////////////////////////////////////
#if !defined(TEXTIFY)
#define TEXTIFY(x) #x
#endif


///////////////////////////////////////////////////////
// Macro:  UNICODIFY
//
// Purpose:
// Converts a string into a wide-character string by
// prepending it with L.
//
// Example:
// UNICODIFY("Foo") produces L"Foo"
///////////////////////////////////////////////////////
#if !defined(UNICODIFY)
#define UNICODIFY(x) CONCATENATE(L, x)
#endif


////////////////////////////////////////////////////////////
//
// Logging macros
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Macro:  CHECK_CONDITION
//
// Purpose:
// Checks whether a condition is true, logging an error
// and throwing a TAEF verification exception if it is
// not.
//
// Dependencies:
// This macro assumes that there is a LOG_ERROR macro
// defined, which takes an error type, an error code,
// and an error description.
///////////////////////////////////////////////////////
#define CHECK_CONDITION(condition) \
{ \
    bool bTempResult = (bool) (condition); \
    if (bTempResult != true) \
    { \
        LOG_ERROR( \
            ERROR_TYPE_BOOL, \
            bTempResult, \
            L"Condition '" UNICODIFY(TEXTIFY(condition)) L"' was not true."); \
    \
        WEX::TestExecution::Private::MacroVerify::Fail(PRIVATE_VERIFY_ERROR_INFO); \
    } \
}


///////////////////////////////////////////////////////
// Macro:  CHECK_HRESULT
//
// Purpose:
// Checks whether an HRESULT indicates a failure,
// loging an error and throwing a TAEF verification
// exception if so.
//
// Dependencies:
// This macro assumes that there is a LOG_ERROR macro
// defined, which takes an error type, an error code,
// and an error description.
///////////////////////////////////////////////////////
#define CHECK_HRESULT(operation) \
{ \
    HRESULT hTempResult = (operation); \
    if (FAILED(hTempResult)) \
    { \
        LOG_ERROR( \
            ERROR_TYPE_HRESULT, \
            hTempResult, \
            UNICODIFY(TEXTIFY(operation)) L" failed."); \
    \
        WEX::TestExecution::Private::MacroVerify::Fail(PRIVATE_VERIFY_ERROR_INFO); \
    } \
}


////////////////////////////////////////////////////////////
//
// Resource pointer cleanup macros
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Macro:  SAFE_RELEASE
//
// Purpose:
// Releases a COM interface pointer.
///////////////////////////////////////////////////////
#define SAFE_RELEASE(pointer) \
    if ((pointer) != nullptr) \
    { \
        (pointer)->Release(); \
        (pointer) = nullptr; \
    }
