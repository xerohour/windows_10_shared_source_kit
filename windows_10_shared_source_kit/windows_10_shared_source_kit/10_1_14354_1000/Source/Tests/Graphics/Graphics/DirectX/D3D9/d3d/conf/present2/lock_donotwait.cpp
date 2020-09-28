//
//  LockDoNotWait.cpp
//
//  D3DLOCK_DONOTWAIT Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2002 Microsoft Corporation.
//
//  Original Author :  ChrisLav
//

#include "Lock_DoNotWait.h"

// -------------------------------------------------------------------------------------
LockDoNotWait::LockDoNotWait()
{
	m_szTestName = _T("LockDoNotWait");
	m_szCommandKey = _T("LockDoNotWait");

	SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91;

	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwPresentInterval = D3DPRESENT_INTERVAL_ONE;
	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
	m_Options.D3DOptions.dwSwapEffect = D3DSWAPEFFECT_FLIP;
	m_Options.D3DOptions.dwPresentFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
}


// -------------------------------------------------------------------------------------
bool LockDoNotWait::CapsCheck()
{
	// this test cannot be run on REF;
	bool bNonREF = ( D3DDEVTYPE_HAL == m_pSrcDevice->GetDevType() );

	return bNonREF;
}


// -------------------------------------------------------------------------------------
UINT LockDoNotWait::TestInitialize()
{
    UINT uResult = CD3DTest::TestInitialize();
	if(uResult != D3DTESTINIT_RUN)
		return uResult;


//	m_AdapterFormat = m_DeviceList[0]->GetAdapter()->DesktopMode.Format;
//	m_fDXVersion = m_DeviceList[0]->GetInterface();


	// Set the test range to 1 - one pass through
	SetTestRange(1, 1);

	return uResult;    
}

// -------------------------------------------------------------------------------------
bool LockDoNotWait::ExecuteTest(UINT uTestNumber)
{
	HRESULT hr;
	LPSURFACE pBB = NULL;
	LOCKED_RECT lockedRect;

	BeginTestCase("Repeat 100x : Clear, Present, Lock(DONOTWAIT)", uTestNumber);

	//
	// D3DLOCK_DONOTWAIT test
	//

	hr = m_pSrcDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB);
	if (FAILED(hr))
	{
		WriteToLog (TEXT("GetBackBuffer failed with %s\n"), m_pD3D->HResultToString(hr));
		Fail();
		return false;
	}

	for (int i=0; i<100; i++)
	{
		hr = m_pSrcDevice->Clear(0, NULL, FMWK_DEFAULT, 0x00000032, 1.f, 0);
		if (FAILED(hr))
		{
			WriteToLog (TEXT("Clear failed with %s\n"), m_pD3D->HResultToString(hr));
			Fail();
		}

		hr = m_pSrcDevice->Present(NULL, NULL, NULL, NULL);
		if (FAILED(hr))
		{
			WriteToLog (TEXT("Present failed with %s\n"), m_pD3D->HResultToString(hr));
			Fail();
		}

		try
		{
			hr=pBB->LockRect(
				&lockedRect,
				NULL,
				D3DLOCK_DONOTWAIT
				);
			if (hr == D3DERR_WASSTILLDRAWING)
			{
				//The test is successful
				goto Exit;
			}
			else if (FAILED(hr))
			{
				WriteToLog (TEXT("LockRect failed with %s\n"), m_pD3D->HResultToString(hr));
				Fail();
			}

			hr=pBB->UnlockRect();
			if (FAILED(hr))
			{
				WriteToLog (TEXT("Unlock failed with %s\n"), m_pD3D->HResultToString(hr));
				Fail();
			}
		}
		catch (...)
		{
			WriteToLog (TEXT("Unhandled exception in LockRect\n"));
			Fail();
		}
	}

	WriteToLog (TEXT("The driver doesn't return D3DERR_WASSTILLDRAWING when D3DLOCK_DONOTWAIT is used\n"));
	Fail();

	//Make sure the test doesn't fail because of the previous errors
	GetLastError();

Exit:

	RELEASE(pBB);

	//return false so that the framework doesn't present
	return false;
}

