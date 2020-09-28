//**********************************************************************
// File: test.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementations of overidden functions.
//
// History:
// 09/04/2007 Created by Jimmy Chen(@jianchen).
//**********************************************************************

#include "Test.hpp"

// Singleton application object.
CMyTestApp g_Application;
LPTSTR  CMyTest::ptszHelperAppD3D = _T("MemPressureReport.exe"); 

TestCaseDescriptor CMyTest::m_testCaseDesc[] = 
{
	{
        // case 0
        _T("[D3D9]TU-1/TC-01: No any video memory allocated"),           // name of the test case
		TRUE,                                                            // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_NoVideoMemoryAllocation,                           // expected test result 2 
    },
	{
        // case 1
        _T("[D3D9]TU-1/TC-02: No any video memory allocated"),           // name of the test case
		TRUE,                                                            // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_NoVideoMemoryAllocation,                           // expected test result 2 
    },
	{
        // case 2
        _T("[D3D9]TU-2/TC-01: Video memory allocated"),                  // name of the test case
		TRUE,                                                            // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_VideoMemoryAllocation,                             // expected test result 2
    },
	{
        // case 3
        _T("[D3D9]TU-2/TC-02: Video memory de-allocated"),               // name of the test case
		TRUE,                                                            // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_VideoMemoryDeallocation,                           // expected test result 2
    },
	{
        // case 4
        _T("[D3D9]TU-2/TC-03: Call Issue(Begin) twice"),                 // name of the test case
		TRUE,                                                            // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_NoVideoMemoryAllocation,                           // expected test result 2
    },
	{
        // case 5
        _T("[D3D9]TU-3/TC-01: Multiprocess"),                            // name of the test case
		FALSE,                                                           // the test case enabled 
        Verify_Result_InitialCondition,                                  // expected test result 1
        Verify_Result_VideoMemoryAllocation,                             // expected test result 2
    },
};

//**********************************************************************
// Individual Member functions for class CMyTest
//**********************************************************************
//----------------------------------------------------------------------------------
// Function: InitTestParameters()
// Purpose: Initializes the parameter axes along which the framework should iterate.
//----------------------------------------------------------------------------------
void 
CMyTest::InitTestParameters()
{
	// Set up a one-dimensional list of scenarios. No space allowed for parameter's name
	AddParameter(_T("CurrentTestCase"), 
                 &m_ushCurTestCase, 
                 RangeValueSet(0, (int)ARRAYSIZE(m_testCaseDesc)-1, 1));
};

//--------------------------------------------------------
// Function: SetupTestCase()
// Purpose: Per-test-case initialization.
//--------------------------------------------------------
TEST_RESULT
CMyTest::SetupTestCase()
{
	HRESULT         hrResult;
    TEST_RESULT     test_result = RESULT_PASS;  

	// Log the test case we're running.
	WriteToLog(_T("\nTest Case: %s."), m_testCaseDesc[m_ushCurTestCase].TestCaseName);

	// Skip inactive scenarios.
	if (!m_testCaseDesc[m_ushCurTestCase].bEnabled) 
	{
        WriteToLog(_T("Warning[SetupTestCase]: Test Case %u (%s) disabled. It'll be skipped."), 
                   m_ushCurTestCase, 
                   m_testCaseDesc[m_ushCurTestCase].TestCaseName);
		test_result = RESULT_SKIP;
		goto Exit;
	};

	hrResult = Helper_Initialization();
    if (FAILED(hrResult))
    {
		if (hrResult == E_TESTSKIPPED)
		{
	    	test_result = RESULT_SKIP;
			WriteToLog(_T("Warning[SetupTestCase]: Helper_Initialization() for Test Case %u (%s) skipped!"), 
				       m_ushCurTestCase, 
					   m_testCaseDesc[m_ushCurTestCase].TestCaseName);
		}
		else
		{
			WriteToLog(_T("Error[SetupTestCase]: Helper_Initialization() for Test Case %u (%s) failed!"), 
				       m_ushCurTestCase, 
					   m_testCaseDesc[m_ushCurTestCase].TestCaseName);
			test_result = RESULT_FAIL;
		}
		goto Exit;
    }

Exit:
	return test_result;
};

//----------------------------------------------------------------------------------
// Function: ExecuteTestCase
// Purpose: Executes a single test case.
//----------------------------------------------------------------------------------
TEST_RESULT 
CMyTest::ExecuteTestCase()
{
    HRESULT         hrResult = S_OK;
    TEST_RESULT     test_result = RESULT_PASS;  

	// Validate the test case number.
	if (m_ushCurTestCase < 0)
	{
        WriteToLog(_T("Error[ExecuteTestCase]: The test case number %d < 0, invalid."), m_ushCurTestCase);
	    test_result = RESULT_FAIL;
        goto Exit;
	};
	if (m_ushCurTestCase >= ARRAYSIZE(m_testCaseDesc))
	{
		WriteToLog(_T( "Invalid scenario number %d > %d, invalid."), m_ushCurTestCase, ARRAYSIZE(m_testCaseDesc));
	    test_result = RESULT_FAIL;
        goto Exit;
	};

	// Run the test case.
	switch (m_ushCurTestCase)
	{
		case 0:
            if (m_bImplementAllTests)
            {
                hrResult = TC_TU1_NoVideoMemoryAllocated_TC01();
            }
            break;

		case 1:
            if (m_bImplementAllTests)
            {
                hrResult = TC_TU1_NoVideoMemoryAllocated_TC02();
            }
            break;

		case 2:
            hrResult = TC_TU2_VideoMemoryAllocated_TC01();
            break;

		case 3:
            if (m_bImplementAllTests)
            {
                hrResult = TC_TU2_VideoMemoryDeallocated_TC02();
            }
            break;

		case 4:
            if (m_bImplementAllTests)
            {
                hrResult = TC_TU2_IssueBeginTwice_TC03();
            }
            break;

		case 5:
            if (m_bImplementAllTests)
            {
                hrResult = TC_TU3_Multiprocess_TC01();
            }
            break;

		default:
			WriteToLog(_T("Test case not implemented."));
		    break;
	};

    if (!SUCCEEDED(hrResult))
    {
        if (hrResult == E_TESTSKIPPED)
        {
		    test_result = RESULT_SKIP;
        }
        else
        {
		    test_result = RESULT_FAIL;
        }
    }

Exit:
    Sleep(2000);           // Wait for a while between each test
	return test_result;
}

//----------------------------------------------------------------------------------
// Function: CleanupTestCase
// Purpose: Per-test-case cleanup.
//----------------------------------------------------------------------------------
void 
CMyTest::CleanupTestCase()
{
    Helper_Cleanup();
};

//*************************************************************************
// Test case functions for D3D: Testable Unit 1, 2 and 3
//*************************************************************************
//-------------------------------------------------------------------------------------------
// Function: TC_TU1_NoVideoMemoryAllocated_TC01()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU1_NoVideoMemoryAllocated_TC01()
{
    AppDescriptor       appDescriptor[] = {
        {
         (USHORT)Action_Wait,
         (DWORD)5000,                           
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,
         (void *)NULL,
        },
    };

    return Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));
}

//-------------------------------------------------------------------------------------------
// Function: TC_TU1_NoVideoMemoryAllocated_TC02()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU1_NoVideoMemoryAllocated_TC02()
{
    AppDescriptor       appDescriptor[] = {
        {
         (USHORT)Action_CreateTexture,
         (DWORD)D3DPOOL_SYSTEMMEM,                           
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,
         (void *)NULL,
        },
        {
         (USHORT)Action_CreateTexture,
         (DWORD)D3DPOOL_SCRATCH,
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,
         (void *)NULL,
        },
    };

    return Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));
}

//-------------------------------------------------------------------------------------------
// Function: TC_TU2_VideoMemoryAllocated_TC01()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU2_VideoMemoryAllocated_TC01()
{
    AppDescriptor       appDescriptor[] = {
        {
         (USHORT)Action_CreateTexture,
         (DWORD)D3DPOOL_MANAGED,                           
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,                           
         (void *)NULL,
        },
        {
         (USHORT)Action_CreateTexture,
         (DWORD)D3DPOOL_DEFAULT,                           
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,                           
         (void *)NULL,
        },
        {
         (USHORT)Action_RenderToTarget,
         (DWORD)0,                           
         (void *)NULL,

         (USHORT)Action_None,
         (DWORD)0,                           
         (void *)NULL,
        },
    };

    return Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));
}

//-------------------------------------------------------------------------------------------
// Function: TC_TU2_VideoMemoryDeallocated_TC02()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU2_VideoMemoryDeallocated_TC02()
{
    AppDescriptor       appDescriptor[] = {
        {
         (USHORT)Action_RenderToTarget,
         (DWORD)0,                           
         (void *)NULL,

         (USHORT)Action_ReleaseMemory,
         (DWORD)Mem_None,                           
         (void *)NULL,
        },
    };
    return Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));
}

//-------------------------------------------------------------------------------------------
// Function: TC_TU2_IssueBeginTwice_TC03()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU2_IssueBeginTwice_TC03()
{
    AppDescriptor       appDescriptor[] = {
        {                                         // call Issue(D3DISSUE_BEGIN) twice
         (USHORT)Action_RenderToTarget,
         (DWORD)0,                           
         (void *)NULL,

         (USHORT)Action_Issue_Begin,
         (DWORD)0,
         (void *)NULL,
        },
    };

    return Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));
}

//-------------------------------------------------------------------------------------------
// Function: TC_TU3_Multiprocess_TC01()
// Purpose:  
//-------------------------------------------------------------------------------------------
HRESULT 
CMyTest::TC_TU3_Multiprocess_TC01()
{
    HRESULT             hrResult = E_FAIL;    
    TCHAR               wszCommand[MAX_PATH];   
    STARTUPINFO         startInfo = {0};
    PROCESS_INFORMATION processInfo = {0};
    AppDescriptor       appDescriptor[] = {
        {
         (USHORT)Action_RenderToTarget,
         (DWORD)0,                           
         (void *)NULL,

         (USHORT)Action_Wait_Process,
         (DWORD)420000,
         (void *)&processInfo,
        },
    };

    if(!CreateProcess(ptszHelperAppD3D,                  // No module name (use command line)
                      NULL,                              // Command line
                      NULL,                              // Process handle not inheritable
                      NULL,                              // Thread handle not inheritable
                      FALSE,                             // Set handle inheritance to FALSE
                      CREATE_NEW_CONSOLE,                // Creation flags
                      NULL,                              // Use parent's environment block
                      NULL,                              // Use parent's starting directory 
                      &startInfo,                        // Pointer to STARTUPINFO structure
                      &processInfo))                     // Pointer to PROCESS_INFORMATION structure
    {
        Helper_DisplayLastError(_T("TC_TU3_Multiprocess_TC01()"), _T("CreateProcess()"));
        goto Exit;
    }
    hrResult = Helper_CommonTestFunction(&appDescriptor[0], ARRAYSIZE(appDescriptor));

Exit:
    return hrResult;
}