// Copyright (C) Microsoft. All rights reserved.
#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <functional>
#include <powrprof.h>
#include <objbase.h>
#include <strsafe.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <wextestclass.h>
#include <BasicLog.h>
#include <BasicLogHelper.h>
#include <BasicPrintf.h>
#include <setupapi.h>
#include <devobj.h>
#include <ks.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>

#define LOG(log, ...) BasicLogPrintf( log, XMSG, 1, __VA_ARGS__ )
#define SKIP(log, ...) BasicLogPrintf( log, XSKIP, 1, __VA_ARGS__ )
#define WARN(log, ...) BasicLogPrintf( log, XWARN, 1, __VA_ARGS__ )
#define BLOCK(log, ...) BasicLogPrintf( log, XBLOCK, 1, __VA_ARGS__ )
#define ERR(log, ...) BasicLogPrintf( log, XFAIL, 1, __VA_ARGS__ )

class OnExit
{
public:
    OnExit( std::function< void (void) > _OnExit ) : m_FunctionPtr( _OnExit ) {}
    ~OnExit()
    {
        m_FunctionPtr();
    }

private:

    std::function< void (void) > m_FunctionPtr;
};
