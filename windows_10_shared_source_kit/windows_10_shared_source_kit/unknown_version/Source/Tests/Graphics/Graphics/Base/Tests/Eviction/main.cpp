#include <VidmmRequires.h>
#include "EvictBlt.h"

using namespace std;

const wchar_t* DESC = L"This application tests eviction.";


/*
  Execute the Test
  This can throw an exception, but really shouldn't
*/
void Go();

/*
  Entry point.
  This calls Go() and catches any exception thrown by it
*/
void __cdecl main()
{    
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        cout << e.what();
    }
    catch(...)
    {
        cout << "Caught an exception at the highest level";
    }    
}


/*
  This sets up the test
*/
void Go()
{
    /*
        The test runner contains all the test components.
        It parses command line options, adds components, and executes them
        Using CTestRunnerBaseVid runner versus standard CTestRunner will
        provide aperture corruption check at the end of the test
    */
    CTestRunnerBaseVid Runner;


    /*
      First, tell the test runner the description for the test application
      (This will appear in the spec)
    */
    Runner.SetRootComponentDesc(DESC);

    Runner.AddNode<CI3DDeviceComponent>(L"Device");

    SetupDeviceCommandLine(Runner, L"Device");

    Runner.AddNode<CTestSlowEviction>(L"Device/SlowEviction");

    //only run in windowed mode
    Runner.SetParamTestDomainSingleValue<bool>(L"Device", L"Windowed", true);

    // pass total video memory parameter
    CSystemInfo sysInfo;
    LONGLONG VidMemSize = 0;

    if(FAILED(sysInfo.GetAvailableVideoMemory(NULL, (DWORDLONG*)&VidMemSize, NULL, NULL, NULL)))
    {
        VidMemSize = 512 * 1024 * 1024;
    }

    Runner.SetParamTestDomainSingleValue<LONGLONG>(L"Device/SlowEviction", L"VidMemSize", VidMemSize);


    Runner.DescribeCommandLineUsage(L"bvt", L"Runs the test in BVT mode (doesn't check allocation patterns)");
    Runner.DescribeCommandLineUsage(L"PagingPathLockSubRange", L"Forces allocation to be evicted in chunks");
    Runner.DescribeCommandLineUsage(L"PagingPathLockMinRange", L"Forces allocation to be evicted in pages");


    if (Runner.IsCmdLineSet(L"bvt") ||
        Runner.IsCmdLineSet(L"-bvt"))
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"Device/SlowEviction", L"BVT", true);
    }

    if(Runner.IsCmdLineSet(L"PagingPathLockSubRange"))
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"Device/SlowEviction", L"PagingPathLockSubRange", true);
    }
    else
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"Device/SlowEviction", L"PagingPathLockSubRange", false);
    }

    if(Runner.IsCmdLineSet(L"PagingPathLockMinRange"))
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"Device/SlowEviction", L"PagingPathLockMinRange", true);
    }
    else
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"Device/SlowEviction", L"PagingPathLockMinRange", false);
    }
    
    
    //  Now that we have setup our test, execute it
    Runner.Run();
}

// util functions to format debug strings
WCHAR* GetArgs(const WCHAR *pwszFmt, va_list argList)
{
    const int   nAllocSize = 128;
    UINT         nAlloc     = nAllocSize;
    bool        bSuccess   = false;
    WCHAR*      pwszOut = NULL;

    // preprocess the string with the values from the variable argument list.
    while(!bSuccess)
    {
        pwszOut = new WCHAR[nAlloc];

        if(!pwszOut)
            return L"Error formatting string. Out of memory.";

        int nChars = _vsnwprintf(pwszOut, nAlloc, pwszFmt, argList);

        if((nChars < 0) || (nChars == nAlloc)) // we need to alloc more memory
        {
            delete[] pwszOut;
            nAlloc += nAllocSize;
        }
        else
        {
            bSuccess = true;
            //add \n to the string
            StringCchCat(pwszOut, sizeof(pwszOut)/sizeof pwszOut[0], L"\n");
        }
    }

    return pwszOut;
}


#ifndef QUIET
void DBGPrint(CLogWriter* pLog, const LPWSTR pwszMessage, ...)
{
    va_list    argList;
    va_start(argList, pwszMessage);
    WCHAR *pwszOut = GetArgs(pwszMessage, argList);
    va_end(argList);

    pLog->Message(pwszOut);
    OutputDebugString(pwszOut);

    if( pwszOut )
    {
        delete[] pwszOut;
    }
}
#else
void DBGPrint(const LPWSTR pwszMessage, ...){};
#endif

