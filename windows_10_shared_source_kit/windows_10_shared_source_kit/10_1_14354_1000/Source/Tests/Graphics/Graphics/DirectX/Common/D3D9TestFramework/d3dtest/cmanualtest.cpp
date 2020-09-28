#include "D3DTestPCH.h"

// --------------------------------------------------------------------------------------

void CManualTest::ExecuteOneTest(void)
{
	HRESULT hr = 0;
    FPSTATE DCTStateBefore, DCTStateAfter;
	DWORD	dwTime = timeGetTime();
	bool    bContinue = false, bHandledDevLost = true;

	// Reset state per test
	m_fPassPercentage = -1.f;

	// Verify we're on a valid test
    if (TERMINATE_NUMBER == m_uTestNumber)
        return;
    
    // Get floating point state	
	if (NULL != g_DCTGet)
		g_DCTGet(&DCTStateBefore);

	// Set new API
	SwitchAPI(m_uTestNumber);

	// Execute by test number callback
    if(ExecuteTest(m_uTestNumber))
	{
		// Begin the test case if the test didn't
        if (!m_pFramework->m_DXGLog.InTestCase())
			BeginTestCase(_T("Unknown Test Case"), m_uTestNumber);

        // Render validation (ie. image comparison)
        if(m_Options.TestOptions.bCompareSrcRef)
		{
			ProcessFrame();
		}
        else
        {
			Pass();
		}
		while(m_pFramework->IsTestPaused() && (bContinue = m_pFramework->ProcessMessages()))
		{
            // Check for device lost and handle it as appropriate
			if(IsDeviceLost())
			{
				bHandledDevLost = HandleDeviceLost();
				// Allow the manual test to setup this test case again
				ExecuteTest(m_uTestNumber);
			}

			if(bHandledDevLost)
			{
				OnPaint();
			}
		}

		if(bContinue)
		{
			// Check for result
			int nResult = m_pFramework->MsgBox(MB_YESNO | MB_ICONQUESTION, _T("Did the test pass?"));
	
			if(nResult == IDYES)
				Pass();
			else
			{
				Fail();
				DialogBox(m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_TESTFAILURE), m_pFramework->m_hWindow, (DLGPROC)TestFailureProc);
			}
		}
	}

	// Check if the floating point info was restored
	if (NULL != g_DCTGet)
		g_DCTGet(&DCTStateAfter);
	
	if (NULL != g_DCTCompare)
	{
		if (!g_DCTCompare(&DCTStateBefore,&DCTStateAfter))
		{
			WriteToLog("\nTest %d corrupted the floating point state\n",m_uTestNumber);
			Fail();
		}
	}

	// Always end the test case - test should call BeginTestCase within ExecuteTest
	EndTestCase();
}

// --------------------------------------------------------------------------------------

void CManualTest::SetTestType(TESTTYPE Type)
{
	CD3DTest::SetTestType(Type);

	m_Options.TestOptions.dwCaps |= TESTCAPS_RESIZEDEVICE;
}

// --------------------------------------------------------------------------------------

HRESULT CManualTest::ExecuteTests()
{
    while(!m_pFramework->IsTestFinished() && !IsTestFinished())
    {
		NextTest();
        m_pFramework->Pause();
		m_pFramework->UpdateToolbar();
		
		if (!m_bException)
			ExecuteOneTest();
		else
		{
			__try
			{
				ExecuteOneTest();
			}
			__except(1)
			{
				if(!m_pFramework->m_DXGLog.InTestCase())
					BeginTestCase(m_pFramework->m_DXGLog.GetCurrentTest()[0] ==  _T('\0') ? _T("Not within any test case") : m_pFramework->m_DXGLog.GetCurrentTest(), m_uTestNumber);

				WriteToLog(_T("***  Caught an exception calling ExecuteOneTest\n"));
				Fail();
				EndTestCase();
				
                m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
			}
		}
		
		if(!m_pFramework->ProcessMessages())
		{
			if(!m_pFramework->m_DXGLog.InTestCase())
				BeginTestCase(m_pFramework->m_DXGLog.GetCurrentTest()[0] ==  _T('\0')  ? _T("User aborted test.") : m_pFramework->m_DXGLog.GetCurrentTest(), m_uTestNumber);
			
			WriteToLog(_T("***  Test aborted by user.\n"));
			Abort();
			EndTestCase();

			m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
            m_pFramework->SuggestExitCode(EXITCODE_ABORT);
		}
	}

    return S_OK;
}

// --------------------------------------------------------------------------------------

bool CManualTest::PostModeSetup(void)
{
	if(!SetupArcBall())
		return false;

    return CD3DTest::PostModeSetup();
}

// --------------------------------------------------------------------------------------

HRESULT CManualTest::Render(void)
{
	return S_OK;
}

// --------------------------------------------------------------------------------------

LRESULT CManualTest::OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_ArcBall.HandleMouseMessages(uMsg, wParam, lParam);

	return CD3DTest::OnMouse(uMsg, wParam, lParam);
}

// --------------------------------------------------------------------------------------

HRESULT CManualTest::Resize(UINT uWidth, UINT uHeight)
{
	HRESULT hr = S_OK;

	if(0 == m_uDeviceList || !(m_Options.TestOptions.dwCaps & TESTCAPS_RESIZEDEVICE))
		return S_OK;
	
	hr = CD3DTest::Resize(uWidth, uHeight);
    
	SetupArcBall();

	// Allow the manual test to setup this test case again
	if(m_uTestNumber > 0 && m_uTestNumber < TERMINATE_NUMBER)
		ExecuteTest(m_uTestNumber);

	return hr;
}

void CManualTest::OnPaint()
{
	HRESULT hr = S_OK;
	DWORD dwTime = 0;

	if(m_bHandlePaintMsg)
	{
        m_pFramework->ClearControls();
	            
		if(m_bRenderOnPaint)
		{
			// Clear the render target
			ClearFrame();
		    
			// Render the frame
			Render();

			// Display the frame
			DisplayFrame();

			// Process any errors caused by flush
			hr = GetLastError();
			if(FAILED(hr))
			{
				ProcessError(hr);
			}
		}

		// Update every 10 milliseconds
		if(timeGetTime() - dwTime > 10)
		{
			dwTime = timeGetTime();
			// Update the controls
			UpdateControls();
		}
	}
}

// --------------------------------------------------------------------------------------

void CManualTest::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
	// Since we need to call ExecuteTest to recreate objects on Resize(), we
	// need to prevent multiple calls to BeginTestCase().

	if (!m_pFramework->m_DXGLog.InTestCase())
		m_pFramework->m_DXGLog.BeginTestCase((const PTSTR)szTestCase, uTestNumber == 0xFFFFFFFF ? m_pFramework->m_DXGLog.GetTestNumberForDevice() : uTestNumber);
}

// --------------------------------------------------------------------------------------

bool CManualTest::SetupArcBall()
{
	if(0 == m_uDeviceList)
		return false;

    // Setup the deafult arcball
	m_ArcBall.SetWindow(m_DeviceList[0]->GetPresParams()->uWidth, m_DeviceList[0]->GetPresParams()->uHeight, 0.85f);

	return true;
}

// --------------------------------------------------------------------------------------

INT_PTR CALLBACK CManualTest::TestFailureProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    const UINT nBuffer = 255;
	TCHAR tcsBuffer[nBuffer];

	switch (nMsg) 
    { 
	case WM_INITDIALOG:
		{
            return TRUE; 
		}
    case WM_COMMAND: 
		{
			switch (LOWORD(wParam)) 
            { 
			case IDOK: 
				GetDlgItemText(hWnd, IDC_FAILUREDESC, tcsBuffer, 254);
				_tcscat(tcsBuffer, _T("\n"));
				DPF(1, tcsBuffer);
                EndDialog(hWnd, TRUE);
                return TRUE; 
			default:
				break;
			}
		}
	default:
		break;
 
    } 
    return FALSE; 
}
