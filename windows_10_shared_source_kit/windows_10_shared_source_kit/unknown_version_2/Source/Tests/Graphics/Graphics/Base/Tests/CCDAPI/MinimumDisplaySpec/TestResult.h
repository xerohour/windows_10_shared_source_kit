#pragma once
#include <windows.h>
#include <tchar.h>
#include <stringutils.h>


namespace TestResult
{
    enum Type : char
    {
        Pass,
        Fail,
        Skip,
        Abort
    };
}

tstring ToString( TestResult::Type const& t );

