/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Entry point for LockConfirm test. The purpose of this test is to
* lock and unlock allocations changing contents and veryfying the contents
*
* Author: Marina Meleshenko [marmel]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/


#include <VidmmRequires.h>
#include "LockConfirm.h"

using namespace std;

extern void DBGPrint(CLogWriter* pLog, const LPWSTR pwszMessage, ...);

const wchar_t * DESC = L"lock allocations, change their content, verify content";


UINT WidthDim[] = {1, 2, 2048, 320};
UINT HeightDim[] = {1, 2, 2048, 280};

UINT VidMmFaults[] = 
{
    SwizzlingApertureFault,
    SwizzlingApertureFault | PagingPathLockSubRangeFault
};


UINT gTestCaseNumber = 0;

template<typename TestComponent>
std::wstring AddTestCase(CTestRunner& Runner,
                         const wchar_t* Name,
                         LONGLONG VidMemSize,
                         bool bBVT,
                         bool bIntegrated)
{
    std::wstring str = L"Device";
    str += ToString(gTestCaseNumber);

    Runner.AddNode< CI3DDeviceComponent >(str.c_str());
    
    SetupDeviceCommandLine(Runner, str.c_str());

    str += L"/";
    str += Name;

    Runner.AddNode< TestComponent>(str.c_str());

    Runner.SetParamTestDomainSingleValue<LONGLONG>(str.c_str(), L"VidMemSize", VidMemSize);
    Runner.SetParamTestDomainSingleValue<bool>(str.c_str(), L"BVT", bBVT);
    Runner.SetParamTestDomainSingleValue<bool>(str.c_str(), L"Integrated", bIntegrated);

    if(bBVT)
    {
        Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocWidth", 2048);
        Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocHeight", 2048);
    }
    else
    {
        UINT nWidthDims = sizeof(WidthDim) / sizeof(WidthDim[0]);
        Runner.SetParamTestDomain<UINT>(str.c_str(), L"AllocWidth", &WidthDim[0], &WidthDim[nWidthDims]);

        UINT nHeightDims = sizeof(HeightDim) / sizeof(HeightDim[0]);
        Runner.SetParamTestDomain<UINT>(str.c_str(), L"AllocHeight", &HeightDim[0], &HeightDim[nHeightDims]);
    }

    gTestCaseNumber++;

    return str;
}



/* Execute the test */
void  Go();

/* Entry point.
    This calls Go() and catches any exception thrown by it.
*/
void __cdecl main()
{
    try
    {
        Go();
    }catch(std::exception& e)
    {
        cout<<e.what();
    }catch(...)
    {
        cout<<"Caught exception at highest level";
    }
}//end of main


/* Sets up the test */

void Go()
{
    /*
        The test runner contains all the test components.
        It parses command line options, adds components, and executes them
        Using CTestRunnerBaseVid runner versus standard CTestRunner will
        provide aperture corruption check at the end of the test
    */
    CTestRunnerBaseVid Runner;
    bool bBVT = false;
    bool bIntegrated = false;
    std::vector< std::wstring > strTest;

    Runner.DescribeCommandLineUsage(L"bvt", L"Runs the test in BVT mode (doesn't check allocation patterns)");
    if (Runner.IsCmdLineSet(L"bvt") ||
        Runner.IsCmdLineSet(L"-bvt"))
    {
        bBVT = true;
    }

    Runner.DescribeCommandLineUsage(L"integrated", L"Indicates that test is running on a system with integrated display chipset");
    if (Runner.IsCmdLineSet(L"integrated") ||
        Runner.IsCmdLineSet(L"-integrated"))
    {
        bIntegrated = true;
    }

    Runner.DescribeCommandLineUsage(L"test", L"Specify specific test case to run");
    if(Runner.IsCmdLineSet(L"test")	||
       Runner.IsCmdLineSet(L"-test"))
    {
        strTest = Runner.GetCmdLineValues(L"test");

        if(!strTest.size())
        {
            cout << "Please specify which test to run. \"LockConfirm.exe [test:test1,test2,...]\"" << endl;
            cout << "Where test1,test2,... are any combination of following tests" << endl;
            cout << "SwizzlingRangeContentionAndPaging" << endl;
            cout << "EvictWhileAccessed" << endl;
            cout << "LockPinned" << endl;
            cout << "LockNonPinned" << endl;
            cout << "EvictUnderLock" << endl;
            cout << "LockEvict" << endl;
            cout << "LockEachMipLevelNoSwizzlingRange" << endl;
            cout << "RunOutOfSwizzlingRanges_nonpinned" << endl;
            cout << "RunOutOfSwizzlingRanges_pinned" << endl;
            cout << "LockEachMipLevel" << endl;
            cout << "LockWithFaults" << endl;
            cout << "LockWhenOutOfMemory" << endl;
        }
    }
    else
    {
        //
        //	Run all tests
        //
        strTest.push_back(L"SwizzlingRangeContentionAndPaging");
        strTest.push_back(L"EvictWhileAccessed");
        strTest.push_back(L"LockPinned");
        strTest.push_back(L"LockNonPinned");
        strTest.push_back(L"EvictUnderLock");
        strTest.push_back(L"LockEvict");
        strTest.push_back(L"LockEachMipLevelNoSwizzlingRange");
        strTest.push_back(L"RunOutOfSwizzlingRanges_nonpinned");
        strTest.push_back(L"RunOutOfSwizzlingRanges_pinned");
        strTest.push_back(L"LockEachMipLevel");
        strTest.push_back(L"LockWithFaults");
        strTest.push_back(L"LockWhenOutOfMemory");
    }

    /*
        First, tell the test runner the description for the test application (This will appear in spec)
    */
    Runner.SetRootComponentDesc(DESC);

    /*
        Next we add our hierarchy of components
    */

    // pass size of video memory
    CSystemInfo sysInfo;
    LONGLONG VidMemSize = 0;

    if(FAILED(sysInfo.GetAvailableVideoMemory(NULL, (DWORDLONG*)&VidMemSize, NULL, NULL, NULL)))
    {
        VidMemSize = 512 * 1024 * 1024;
    }


    for(UINT i = 0; i < strTest.size(); i++)
    {
        if((ToLowerCase(strTest[i]) == ToLowerCase(L"SwizzlingRangeContentionAndPaging")) && !bBVT)
        {
            std::wstring str;

            str = AddTestCase<CLockConfirmSwizzlingRangeContentionAndPaging>(Runner, 
                                                                             L"SwizzlingRangeContentionAndPaging", 
                                                                             VidMemSize, 
                                                                             bBVT, 
                                                                             bIntegrated);

            // Run this test once for RenderTarget and Texture
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocWidth", 512);
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocHeight", 512);
        }
        else if((ToLowerCase(strTest[i]) == ToLowerCase(L"LockWhenOutOfMemory")) && !bBVT)
        {
            std::wstring str;	

            str = AddTestCase<CLockConfirmLockWhenOutOfMemory>(Runner, L"LockWhenOutOfMemory", VidMemSize, bBVT, bIntegrated);

            // Run this test only once
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocWidth", 2048);
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocHeight", 2048);
            Runner.SetParamTestDomainSingleValue<eALLOC_TYPE>(str.c_str(), L"LockConfirmAllocType", Texture);
        }
        else if((ToLowerCase(strTest[i]) == ToLowerCase(L"EvictWhileAccessed")) && !bBVT)
        {
            std::wstring str;	
            
            str = AddTestCase<CLockConfirmEvictWhileAccessed>(Runner, L"EvictWhileAccessed", VidMemSize, bBVT, bIntegrated);

            // Run this test only once
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocWidth", 2048);
            Runner.SetParamTestDomainSingleValue<UINT>(str.c_str(), L"AllocHeight", 2048);
            Runner.SetParamTestDomainSingleValue<eALLOC_TYPE>(str.c_str(), L"LockConfirmAllocType", Texture);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockPinned"))
        {
            AddTestCase<CLockConfirmLockPinned>(Runner, L"LockPinned", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockNonPinned"))
        {
            AddTestCase<CLockConfirmLockNonPinned>(Runner, L"LockNonPinned", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"EvictUnderLock"))
        {
            AddTestCase<CLockConfirmEvictUnderLock>(Runner, L"EvictUnderLock", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockEvict"))
        {
            AddTestCase<CLockConfirmLockEvict>(Runner, L"LockEvict", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockEachMipLevelNoSwizzlingRange"))
        {
            AddTestCase<CLockConfirmLockEachMipLevelNoSwizzlingRange>(Runner, L"LockEachMipLevelNoSwizzlingRange", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"RunOutOfSwizzlingRanges_nonpinned"))
        {
            AddTestCase<CLockConfirmRunOutOfSwizzlingRanges_nonpinned>(Runner, L"RunOutOfSwizzlingRanges_nonpinned", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"RunOutOfSwizzlingRanges_pinned"))
        {
            AddTestCase<CLockConfirmRunOutOfSwizzlingRanges_pinned>(Runner, L"RunOutOfSwizzlingRanges_pinned", VidMemSize, bBVT, bIntegrated);
        }
        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockEachMipLevel"))
        {
            AddTestCase<CLockConfirmLockEachMipLevel>(Runner, L"LockEachMipLevel", VidMemSize, bBVT, bIntegrated);
        }

        else if(ToLowerCase(strTest[i]) == ToLowerCase(L"LockWithFaults"))
        {
            //
            //	For LockWithFault tests we'll add additional parameters (faults)
            //
            std::wstring str = AddTestCase<CLockConfirmLockWithFaults>(Runner, L"LockWithFaults", VidMemSize, bBVT, bIntegrated);
            
            UINT FaultCount = sizeof(VidMmFaults) / sizeof(VidMmFaults[0]);

            if(bBVT)
            {
                // for BVT don't run any of the slow path faults
                FaultCount = 1;
            }

            Runner.SetParamTestDomain<UINT>(str.c_str(), L"VidMmFaults", &VidMmFaults[0], &VidMmFaults[FaultCount]);
        }
    }

    /*
    Now that we have setup our test, execute it
    */
    Runner.Run();

}

