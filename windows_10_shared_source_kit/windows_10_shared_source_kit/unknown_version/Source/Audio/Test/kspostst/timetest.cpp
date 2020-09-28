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
#include <bestfit.h>
#include <math.h>


#define SLEEP_FOR   500
#define CPOINTS     200

// ------------------------------------------------------------------------------

ULONGLONG   g_CyclesPerMS;

#pragma optimize("", off)
#pragma warning(push)
#pragma warning(disable : 4748)

// since the timer tests are commented out from the test case table
// this code will also be disabled from compile

#if 0
// ------------------------------------------------------------------------------
// Mothra = timeGetTime
// Godzilla = QueryPerformanceCounter
// ------------------------------------------------------------------------------
DWORD
Test_MothraVsGodzilla
(
    void
)
{
    DWORD   dwRes = FNS_FAIL;
    PDPOINT argRes = NULL;
    ULONG   csEstTimeToFinish;
    int     i, j;

    typedef struct {
        int     nPriority;
        LPSTR   szRep;
    } PRIOR_REP;
    
    PRIOR_REP Priorities[] = 
    { 
        THREAD_PRIORITY_LOWEST,       "THREAD_PRIORITY_LOWEST",
        THREAD_PRIORITY_NORMAL,       "THREAD_PRIORITY_NORMAL",
        THREAD_PRIORITY_TIME_CRITICAL,"THREAD_PRIORITY_TIME_CRITICAL"
    };

    try
    {
        argRes = (PDPOINT)LocalAlloc(LPTR, CPOINTS * sizeof(DPOINT));
        if (!argRes)
            throw("Out of memory!");

        for (j = 0; j < 3; j++)
        {
            SLOG(eInfo1, "Setting Priority to %s", Priorities[j].szRep);
            if (!SetThreadPriority(GetCurrentThread(), Priorities[j].nPriority))
                SLOG(eError, "SetThreadPriority failed");

            csEstTimeToFinish = CPOINTS * SLEEP_FOR;
            SLOG(eInfo1, "This test will take ~ %d seconds.  Please stand by...", csEstTimeToFinish / 1000);
    
            Sleep(1);   // we need the next 2 calls to be in the same time slice

            argRes[0].x = tpTGT();
            argRes[0].y = tpQPC();

            for (i = 1; i < CPOINTS; i++)
            {
                Sleep(SLEEP_FOR);

                argRes[i].x = tpTGT() - argRes[0].x;
                argRes[i].y = tpQPC() - argRes[0].y;

#ifndef BUILD_TAEF
                if (g_IShell->RequestAbortWithPM())
                    throw("Aborting test...");
#endif

                csEstTimeToFinish -= SLEEP_FOR;
                if (csEstTimeToFinish % 10000 == 0)
                    SLOG(eInfo1, "%d more seconds...", csEstTimeToFinish / 1000);
            }

            argRes[0].x = 0.;
            argRes[0].y = 0.;

            SLOG(eInfo1, "        tGT         QPC        Delta");
            SLOG(eInfo1, "    ==========  ==========  ==========");

            for (i = 0; i < CPOINTS; i++)
            {
                double dDelta = argRes[i].x - argRes[i].y;
                SLOG(eInfo1, "    %10.3f, %10.3f, %.10f", argRes[i].x, argRes[i].y, dDelta);
            }

            double m = SLOPE(&argRes[1], CPOINTS-1);
            double b = INTERCEPT(&argRes[1], CPOINTS-1, m);

            SLOG(eInfo1, "Slope = %10.9f.  Intercept = %10.3f", m, b);
        }
    }

    // handle bad things
    catch(char* str)
    {
        SLOG(eError, str);
        dwRes = FNS_FAIL;
    }

    SafeLocalFree(argRes);

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    return dwRes;
}
#pragma warning(pop)
#pragma optimize("", on)
#endif //#ifndef _WIN64


// ------------------------------------------------------------------------------
// retrieves the system time, in milliseconds. The system time is the time elapsed since Windows was started.
// ------------------------------------------------------------------------------
double tpTGT (void)
{
#ifndef BUILD_FOR_MOBILE_CORE
    return (double)timeGetTime();
#else
    return (double)GetTickCount();
#endif
}

// ------------------------------------------------------------------------------
// returns millisecs
double tpQPC (void)
{
    static LARGE_INTEGER liFrequency = { 0 };
    static LARGE_INTEGER liStart = { 0 };
    static bool bInitialized = false;

    if (!bInitialized)
    {
        if (!QueryPerformanceFrequency(&liFrequency))
        {
            throw ("QueryPerformanceFrequency failed");
        }
        
        if (!QueryPerformanceCounter(&liStart))
        {
            throw ("QueryPerformanceCounter failed");
        }
        
        bInitialized = true;
    }

    LARGE_INTEGER liNow = { 0 };

    if (!QueryPerformanceCounter(&liNow))
    {
        throw ("QueryPerformanceCounter failed");
    }

    // milliseconds since test start
    return 1000.0 * (liNow.QuadPart - liStart.QuadPart) / liFrequency.QuadPart;
}


// ------------------------------------------------------------------------------
#if  defined(_X86_) && !defined(BUILD_TAEF)

#define rdtsc __asm _emit 0x0f __asm _emit 0x31
#pragma warning(disable:4035)

LONGLONG __inline ReadCycleCounter(VOID)
{
    __asm rdtsc;
}

#pragma warning(default:4035)

double tpCCC (void)
{
    ULONGLONG ullTime = ReadCycleCounter();

    ullTime /= (ULONGLONG)g_CyclesPerMS;

    ULONG   ulTime = (ULONG)ullTime;

    return (double)ulTime;
}

// Some static variables used by GetCPUSpeed
static int s_milliseconds;
static __int64 s_ticks;

// ------------------------------------------------------------------------------
//  StartTimingCPU
// ------------------------------------------------------------------------------
#pragma warning(disable:4740)

int StartTimingCPU(void)
{
    //
    // detect ability to get info
    //
    __asm
    {
        pushfd                              ; push extended flags
        pop        eax                      ; store eflags into eax
        mov        ebx, eax                 ; save EBX for testing later
        xor        eax, (1<<21)             ; switch bit 21
        push    eax                         ; push eflags
        popfd                               ; pop them again
        pushfd                              ; push extended flags
        pop        eax                      ; store eflags into eax
        cmp        eax, ebx                 ; see if bit 21 has changed
        jz        no_cpuid                  ; make sure it's now on
    }

    //
    // start timing
    //
#ifndef BUILD_FOR_MOBILE_CORE
    s_milliseconds = -(int)timeGetTime();
#else
    s_milliseconds = -(int)GetTickCount();
#endif

    __asm
    {
        lea        ecx, s_ticks             ; get the offset
        mov        dword ptr [ecx], 0       ; zero the memory
        mov        dword ptr [ecx+4], 0     ;
//        rdtsc                             ; read time-stamp counter
        __emit 0fh 
        __emit 031h
        sub        [ecx], eax               ; store the negative
        sbb        [ecx+4], edx             ; in the variable
    }
    return 0;

no_cpuid:
    return -1;
}
#pragma warning(default:4740)

// ------------------------------------------------------------------------------
//  StopTimingCPU
// ------------------------------------------------------------------------------
VOID StopTimingCPU(void)
{
#ifndef BUILD_FOR_MOBILE_CORE
    s_milliseconds        += (int)timeGetTime();
#else
    s_milliseconds        += (int)GetTickCount();
#endif

    __asm
    {
        lea        ecx, s_ticks             ; get the offset
//        rdtsc                             ; read time-stamp counter
        __emit 0fh 
        __emit 031h

        add        [ecx], eax               ; add the tick count
        adc        [ecx+4], edx             ;
    }
    
    return;
}


// ------------------------------------------------------------------------------
//  CalcCPUSpeed
// ------------------------------------------------------------------------------
int CalcCPUSpeed(void)
{
    //
    // get the actual cpu speed in MHz, and
    // then find the one in the CPU speed list
    // that is closest
    //
    const struct tagCPUSPEEDS
    {
        float    fSpeed;
        int        iSpeed;
    } cpu_speeds[] =
    {
        //
        // valid CPU speeds that are not integrally divisible by
        // 16.67 MHz
        //
        {  60.00f,     60 },
        {  75.00f,     75 },
        {  90.00f,     90 },
        { 120.00f,    120 },
        { 180.00f,    180 },
    };

    //
    // find the closest one
    //
    float    fSpeed=((float)s_ticks)/((float)s_milliseconds*1000.0f);
    int        iSpeed=cpu_speeds[0].iSpeed;
    float    fDiff=(float)fabs(fSpeed-cpu_speeds[0].fSpeed);

    for (int i=1 ; i<sizeof(cpu_speeds)/sizeof(cpu_speeds[0]) ; i++)
    {
        float fTmpDiff = (float)fabs(fSpeed-cpu_speeds[i].fSpeed);

        if (fTmpDiff < fDiff)
        {
            iSpeed=cpu_speeds[i].iSpeed;
            fDiff=fTmpDiff;
        }
    }

    //
    // now, calculate the nearest multiple of fIncr
    // speed
    //


    //
    // now, if the closest one is not within one incr, calculate
    // the nearest multiple of fIncr speed and see if that's
    // closer
    //
    const float fIncr=16.66666666666666666666667f;
    const int iIncr=4267; // fIncr << 8

    //if (fDiff > fIncr)
    {
        //
        // get the number of fIncr quantums the speed is
        //
        int     iQuantums        = (int)((fSpeed / fIncr) + 0.5f);
        float   fQuantumSpeed    = (float)iQuantums * fIncr;
        float   fTmpDiff         = (float)fabs(fQuantumSpeed - fSpeed);

        if (fTmpDiff < fDiff)
        {
            iSpeed = (iQuantums * iIncr) >> 8;
            fDiff=fTmpDiff;
        }
    }

    return iSpeed;
}


// ------------------------------------------------------------------------------
//  GetCPUSpeed

int GetCPUSpeed(void)
{
    int speed1;
    int speed2;
    
    if (StartTimingCPU())
        return 0;
    Sleep(1000);
    StopTimingCPU();
    speed1 = CalcCPUSpeed();

    if (StartTimingCPU())
        return 0;
    Sleep(1000);
    StopTimingCPU();
    speed2 = CalcCPUSpeed();

    // 24339: Some laptop systems have a CPU speed that varies based on
    // current computing load.  If we think this may be the case, just
    // don't report a CPU speed.  A fancier option, on WinNT, would be
    // to use WMI to get the CPU speed.
    if (speed1 == speed2)
        return speed1;
    else
        return 0;
}


#endif  //#ifdef _X86_

OACR_WARNING_POP
