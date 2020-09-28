// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  tests.cpp
//
// Abstract:
//
//  Implementation file for test cases
//




// -------------------------------------------------------------------------------

#include "PreComp.h"

#define SLEEP_FOR   500
#define CPOINTS     200

static LARGE_INTEGER liFreq = { 0, 0 };

// ------------------------------------------------------------------------------
// returns millisecs
double tpQPC (void)
{
    LARGE_INTEGER liCount;

    if ((liFreq.QuadPart == 0) && (!QueryPerformanceFrequency(&liFreq)))
        return (ULONG)-1;

    QueryPerformanceCounter(&liCount);

    // convert to ms
    liCount.QuadPart = liCount.QuadPart * 1000000 / liFreq.QuadPart;

    return (double)liCount.LowPart / 1000.;
}
