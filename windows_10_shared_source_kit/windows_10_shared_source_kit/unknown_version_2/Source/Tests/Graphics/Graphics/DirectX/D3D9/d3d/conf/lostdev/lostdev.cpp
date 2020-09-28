////////////////////////////////////////////////////////////////////////////////
// File:  LostDev.cpp
//
// Copyright (c) Microsoft Corporation
//
// Purpose:
// Test cases for Direct3D device loss.
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////

#include "LostDev.h"
#include "CTextures.h"
#include <windows.h>

#ifdef USE_FUS
#include "CFUS.h"
#endif


///////////////////////////////////////////////////////////////////////////
//
// Globals
//
///////////////////////////////////////////////////////////////////////////

//
// Declare the framework
//

CD3DWindowFramework	theApp;

#if FAST_TEST
CLostTexture				lostDevTexture;
#else //FAST_TEST
//
// Define the groups here
//
CLostDepthStencil			lostDevDepthStencil;
CLostRenderTarget			lostDevRenderTarget;

CLostImageSurface			lostDevImageSurface;
CLostOffscreenPlainSurface	lostDevOffscreenPlainSurface;

CLostVertexBuffer			lostDevVertexBuffer;

CLostTexture				lostDevTexture;
//CLostCubeTexture			lostDevCubeTexture;
//CLostVolumeTexture		lostDevVolTexture;

CLostStateBlock				lostStateBlock;
//CLostVertexShader			lostVertexShader;
//CLostPixelShader			lostPixelShader;
#endif//FAST_TEST


///////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Function:  ExceptionFilter
//
// Purpose:
// Determines which types of exceptions are eligible by handling in
// the application's __except block.  Currently the only exception
// that will be caught is an access violation.
//
// Called by:
// __except block of various pieces of code.
//
// Returns:
// EXCEPTION_EXECUTE_HANDLER     Go to the body of the __except block.
// EXCEPTION_CONTINUE_EXECUTION  Ignore the exception and continue.
// EXCEPTION_CONTINUE_SEARCH     Bubble the exception up the stack.
//////////////////////////////////////////////////////////////////////
int ExceptionFilter
(
	unsigned int ExceptionCode,
	_EXCEPTION_POINTERS * pExceptionDetails
)
{
	switch ( ExceptionCode )
	{
		case EXCEPTION_ACCESS_VIOLATION:
		{
			//OutputDebugString( "Caught access violation.\n" );
			return EXCEPTION_EXECUTE_HANDLER;
		};

		default:
		{
			//OutputDebugString( "Caught other exception.\n" );
			return EXCEPTION_CONTINUE_SEARCH;
		};
	};
};


///////////////////////////////////////////////////////////////////////////
//
// Implementation
//
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Function:  CLostDev
//
// Purpose:
// Constructor.
//////////////////////////////////////////////////////////////////////
CLostDev::CLostDev()
:
	m_bTestSystemMem(true),
	m_bTestManaged(true),
	m_bTestDefault(true),
	m_bTestScratch(true),
	m_bSkipGroup(false)
{ 
	m_szCommandKey = _T("LostDev");
	m_szLogName = _T("LostDev");
	m_szTestName = _T("Lost Device Conformance Test Cases");

	SetTestType(TESTTYPE_CONF);

	// Prevent a bunch of log spew about losing focus, since
	// losing focus is what this test is all about.
	m_Options.TestOptions.bLogAppActivation = false;

	// No need to disable DWM during the test initialization, this eliminates the ToggleDWM() call
	m_Options.TestOptions.bDisableDWM = false;

	m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;

	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.bRefEqualsSrc = true;
	//m_Options.D3DOptions.bReference = true;
	m_Options.TestOptions.bCompareSrcRef = false;
	m_Options.D3DOptions.dwRefDevType = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwVPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwPPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefVPEmuRegChange = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefPPEmuRegChange = VPEMUREGCHANGE_NONE;

	//Use these flags so that no backbuffer or DepthStencil surface are in D3DPOOL_DEFAULT for the reset
	m_Options.D3DOptions.bCachedBackBuffer = false;
	m_Options.D3DOptions.bZBuffer = false;

	m_Options.D3DOptions.dwIncVersions = DXVERSION_ALL;

	if(KeySet(_T("WHQL")))
	{
		// No need to test DX 8 interfaces for WHQL.
		m_Options.D3DOptions.dwIncVersions &= ~(DXVERSION_8 | DXVERSION_81 | DXVERSION_8_OR_81);
	}
	else
	{
		// No need to test DX 8.0 at all.
		m_Options.D3DOptions.dwIncVersions &= ~(DXVERSION_8);
	};

	m_pFramework->GetImageCmp()->SetPassRequired(0.85f);

	//Determine which tests to run from the command line params
	SetTestsToRun();
}


//////////////////////////////////////////////////////////////////////
// Function:  ~CLostDev
//
// Purpose:
// Destructor.
//////////////////////////////////////////////////////////////////////
CLostDev::~CLostDev()
{
}


// ----------------------------------------------------------------------------

bool CLostDev::CapsCheck()
{
	if (m_bSkipGroup)
	{
		return false;
	};

	return true;
}


//////////////////////////////////////////////////////////////////////
// Function:  ValidateDevices
//
// Purpose:
// Validates device modes for this test.
// This function has also been overloaded to perform a check for a
// known GDI leak on Windows 9x.
//////////////////////////////////////////////////////////////////////
HRESULT	CLostDev::ValidateDevices(DEVICEMODE *pMode)
{
	HRESULT hRes = CD3DTest::ValidateDevices(pMode);
	if (hRes != S_OK)
		return hRes;

	if ( AreTherePossibleGDILeaks() )
	{
		DPF(1, "Because of a known GDI leak in win9x,\nthese applications must not run with this test:\n  -Internet Explorer\n  -Windows Explorer\n  -Active Desktop\nPlease close these application(s) and rerun the test.\n");
		return E_ABORT;
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Initializes the test group.
//////////////////////////////////////////////////////////////////////
UINT CLostDev::TestInitialize()
{
	//
	// Call the base class implementation.
	//

	UINT uResult = CD3DTest::TestInitialize();

	if(uResult != D3DTESTINIT_RUN)
	{
		return uResult;
	};


	//
	// Skip if we're on REF.
	//

	if ( m_pSrcDevice->GetDevType() == DEVICETYPE_REF )
	{
		WriteToLog
		(
			_T( "This test does not work on REF." )
		);

		SetTestRange(1,1);
		uResult = D3DTESTINIT_SKIPALL;
		goto Cleanup;
	};


	//
	// Set up our test factors.
	//

	CreateTestFactors();
	SetTestFactors();

	if (!m_rCasesManager->Valid())
	{
		SetTestRange(1,1);
		uResult = D3DTESTINIT_SKIPALL;
		goto Cleanup;
	}


#ifdef USE_FUS
	// Make sure that we're on Windows XP or above.
	OSVERSIONINFO VersionInfo;
	memset(&VersionInfo, 0, sizeof(OSVERSIONINFO));
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);
	if
	(
		VersionInfo.dwMajorVersion > 6 ||
		(VersionInfo.dwMajorVersion == 5) && (VersionInfo.dwMinorVersion >= 1)
	)
	{
		CFUS fus;
		if (fus.IsProfessional())
		{
			if (fus.IsDomainMember())
			{
				DPF(1, "You need to not be member of a Domain\n");
				return D3DTESTINIT_ABORT;
			}
		}
		else if (!fus.IsHomeEdition())
		{
			DPF(1, "FastUserSwitching is not enabled\n");
			uResult = D3DTESTINIT_ABORT;
			goto Cleanup;
		}

		if (!fus.IsFriendlyLogon())
		{
			DPF(1, "FriendlyLogon is not enabled\n");
			uResult = D3DTESTINIT_ABORT;
			goto Cleanup;
		}
		if (!fus.IsFastUserSwitchingEnabled())
		{
			DPF(1, "FastUserSwitching is not enabled\n");
			uResult = D3DTESTINIT_ABORT;
			goto Cleanup;
		}
	}
	else
	{
		//Disable FUS, we are not on XP
		m_uLostFlags &= ~LOST_FUS;
	}
#endif

	if (m_rCasesManager->Count() == 0)
	{
		SetTestRange(1,1);
		uResult = D3DTESTINIT_SKIPALL;
		goto Cleanup;
	};


	////Get the display settings
	//m_DisplayDevice.cb = sizeof(DISPLAY_DEVICE);
	//int iDevNum = 0; //Required, if 0 direct in EnumDisp.., it doesn't work
	//if ( !EnumDisplayDevices(NULL, iDevNum, &m_DisplayDevice, 0))
	//{
	//	SetTestRange(1,1);
	//	return D3DTESTINIT_SKIPALL;
	//}
	//
	//m_DevMode.dmSize = sizeof(DEVMODE);
	//if (!EnumDisplaySettings(m_DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &m_DevMode))
	//{
	//	SetTestRange(1,1);
	//	return D3DTESTINIT_SKIPALL;
	//}

	//Set the Test Range here
	SetTestRange(1, m_rCasesManager->Count());


Cleanup:
	return uResult;
}


//////////////////////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Executes the specified test case.
//////////////////////////////////////////////////////////////////////
bool CLostDev::ExecuteTest
(
	IN UINT uTestNumber
)
{
	//Don't let DWM paint messages interfere with the test
	m_bHandlePaintMsg = false;

	//Need to sleep every 600 cases so that the OS can recover from
	//memory leaks (or queueing up of refresh calls).
	if (0 == (uTestNumber % 600))
	{
		Sleep(5000);
	}

	m_rCasesManager->Set(uTestNumber - 1);

	// Set the Present Parameters
	memcpy(&m_PresParams, m_DeviceList[0]->GetPresParams(), sizeof(PRESENT_PARAMETERS));
	m_PresParams.SwapEffect = D3DSWAPEFFECT_COPY;  // Must be copy to compare the back buffers 
	m_PresParams.dwFlags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if ( !SetTestCase(uTestNumber) )
	{
		return false;
	}

	sprintf(m_szFullName, "G%d T%d %s", m_pFramework->GetCurrentTestNum(), uTestNumber, m_szSubTest);

	//Set the flags of the overloaded tests and add flags to m_szFullName
	SetFlags();

	OutputDebugString(m_szFullName);
	OutputDebugString("\n");

	// Begin the test case
	BeginTestCase(m_szFullName, uTestNumber);

	// Perform the test
	bool result = true;


	switch ( m_uTestType )
	{
		case TEST_CREATIONWHILELOST :
			result = CreationWhileLost();
			break;

		case TEST_CREATIONWHILELOSTFILLAFTER :
			result = CreationWhileLostFillAfter();
			break;

		case TEST_LOCKWHILELOST :
			result = LockWhileLost();
			break;

		case TEST_LOSTWHILELOCKED :
			result = LostWhileLocked();
			break;

		case TEST_LOSTWHILERENDERING :
			result = LostWhileRendering();
			break;

		case TEST_CREATEAANDBTHENLOSE :
			result = CreateAAndBThenLose();
			break;

		case TEST_CREATEALOSECREATEB :
			result = CreateAThenLoseThenCreateB();
			break;

		case TEST_LOSETHENCREATEAANDB :
			result = LoseThenCreateAAndB();
			break;

		case TEST_FUSCREATEFUSBACK :
			result = FUSCreateFUSBack();
			break;

		case TEST_CREATEFUSDELETEFUSBACK :
			result = CreateFUSDeleteFUSBack();
			break;

		case TEST_RELEASEAFTERRESETFAILED :
			result = ReleaseAfterResetFailed();
			break;

		case TEST_DEFAULTPOOLTEST :
			if(m_pD3D->GetVersion() >= 8.0)
				result = DefaultPoolTest();
			else
				result = true;//default pool resources don't fail this test pre-dx8
			break;

		case TEST_CREATEAANDBTHENSUSPENDSYSTEM:
			result = CreateAAndBThenSuspendSystem();
			break;

		default :
			DPF(1, "No Test Executed\n");
			result = false;
	}

	if (false == result)
	{
		FailTest();
	}

	return result;
}


//////////////////////////////////////////////////////////////////////
// Function:  TestTerminate
//
// Purpose:
// Cleans up after the test group.
//////////////////////////////////////////////////////////////////////
bool CLostDev::TestTerminate()
{
	ReleaseTest();

	////Restore the original screen resolution if the settings have changed
	//if (m_DisplayDevice.cb)
	//{
	//	LONG result = ChangeDisplaySettingsEx(m_DisplayDevice.DeviceName, &m_DevMode, NULL, CDS_RESET, NULL);
	//	if (result != DISP_CHANGE_SUCCESSFUL)
	//		return false;
	//}

	return true;
}


//////////////////////////////////////////////////////////////////////
// Function:  EndTestCase
//
// Purpose:
// Ends a test case.  Performs image comparison if needed.
//////////////////////////////////////////////////////////////////////
void CLostDev::EndTestCase()
{
	// Initialize locals.
	bool bRes = true;
	HRESULT hr = E_FAIL;
	CSurface * pSrcSurf = NULL;
	CSurface * pRefSurf = NULL;


	// No comparison is needed for DEFAULT pool tests.
	if (m_Pool == D3DPOOL_DEFAULT)
	{
		goto Exit;
	};


	// No comparison is needed for the ReleaseAfterResetFailed tests.
	if (m_uTestType == TEST_RELEASEAFTERRESETFAILED)
	{
		goto Exit;
	};


	// Sleep 2 seconds after rendered the frame to give the user a chance to see it.
	if ( m_bSeeResult == true )
	{
		Sleep(2000);
	};


	// Get the src buffer.
	hr = m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	if(FAILED(hr))
	{
		DPF
		(
			1,
			_T("EndTestCase:  GetBackBuffer on SRC failed with error 0x%x (%s).\n"),
			hr,
			m_pD3D->HResultToString(hr)
		);

		bRes = false;
		goto Exit;
	};
		

	// Get the ref buffer.
	hr = m_pRefDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &pRefSurf);
	if(FAILED(hr))
	{
		DPF
		(
			1,
			_T("EndTestCase:  GetBackBuffer on REF failed with error 0x%x (%s).\n"),
			hr,
			m_pD3D->HResultToString(hr)
		);

		bRes = false;
		goto Exit;
	};


	// Compare the buffers.
	if(FAILED(m_pFramework->GetImageCmp()->CompareImages(pSrcSurf, pRefSurf, NULL)))
	{
		DPF
		(
			1,
			_T("EndTestCase:  CompareImages failed with error 0x%x (%s).\n"),
			hr,
			m_pD3D->HResultToString(hr)
		);

		FailTest();
		goto Exit;
	};


	// Check the results.
	m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();
	if( m_fPassPercentage > m_pFramework->GetImageCmp()->GetPassRequired() )
	{
		Pass();
	}
	else
	{
		DPF
		(
			1, _T("EndTestCase:  Image comparison failed.  Expected:  %2.2f%%.  Actual:  %2.2f%%.\n"),
			m_pFramework->GetImageCmp()->GetPassRequired() * 100.f,
			m_fPassPercentage * 100.f
		);

		FailTest();
	};


	// Sleep 2 seconds after the failure to give the user a chance to see it.
	if ( m_bSeeFailures == true && m_bSeeResult == false )
	{
		//MessageBeep( MB_ICONEXCLAMATION );
		Sleep(2000);
	};


Exit:
	RELEASE(pSrcSurf);
	RELEASE(pRefSurf);

	CD3DTest::EndTestCase();
}

// ----------------------------------------------------------------------------

void CLostDev::ReleaseTest()
{
	RELEASE(m_pBackBufferCopy);
	m_rCasesManager.Release();
	m_casesPools.Empty();
	m_casesLostWays.Empty();
	m_casesTestTypeDefault.Empty();
	m_casesTestType1Res.Empty();
	m_casesTestType2Res.Empty();
}

// ----------------------------------------------------------------------------

bool CLostDev::NormalResult()
{
	//Create the resource
	if ( !CreateResource(CD3D_REF) )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};

	//Lock the resource and fill it with data
	if ( !LockData(CD3D_REF) )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};

	if ( !FillAndUnlockData(CD3D_REF) )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Render the resource
	if ( !DrawResource(CD3D_REF) )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	return true;

Error:
	ReleaseResource();
	return false;
}


// ----------------------------------------------------------------------------
bool CLostDev::DefaultPoolTest()
{
	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lock the resource and fill it with data
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};


	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	// The reset should fail pre-9.L since the resource is in the default pool.
	MuteLog();

	bool bResetDeviceResult = ResetDevice();

	UnMuteLog();

	if
	(
		m_pD3D->GetVersion() >= 8.0f &&
		m_pD3D->GetVersion() < 9.1f &&
		bResetDeviceResult == true
	)
	{
		DPF
		(
			1,
			_T( "Reset succeeded when expected to fail due to resources in default pool." )
		);

		goto Error;
	}
	else if
	(
		m_pD3D->GetVersion() >= 9.1f &&
		bResetDeviceResult == false
	)
	{
		DPF
		(
			1,
			_T( "Reset failed due to resources in default pool when expected to succeed." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//The reset should now succeed in all cases.
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}


// ----------------------------------------------------------------------------
bool CLostDev::ReleaseAfterResetFailed()
{
	//Create a POOL_DEFAULT resource so that the reset fails
	LPTEXTURES pTex = NULL;
	if ( !CreateTexture(64, 64, 1, 0, FMT_X8R8G8B8, POOL_DEFAULT, &pTex) )
	{
		WriteToLog
		(
			_T( "Failed to create texture." )
		);

		goto Error;
	};


	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	// The reset should fail pre-9.L due to the texture in the default pool.
	MuteLog();

	bool bResetDeviceResult = ResetDevice();

	UnMuteLog();

	if
	(
		m_pD3D->GetVersion() >= 8.0f &&
		m_pD3D->GetVersion() < 9.1f &&
		bResetDeviceResult == true
	)
	{
		DPF
		(
			1,
			_T( "Reset succeeded when expected to fail due to resources in default pool." )
		);

		goto Error;
	}
	else if
	(
		m_pD3D->GetVersion() >= 9.1f &&
		bResetDeviceResult == false
	)
	{
		DPF
		(
			1,
			_T( "Reset failed due to resources in default pool when expected to succeed." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Release the pool default texture
	RELEASE(pTex);

	//The reset should now succeed in all cases.
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	RELEASE(pTex);

	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::CreationWhileLost()
{
	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lock the resource and fill it with data
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};

	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::CreationWhileLostFillAfter()
{
	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Lock the resource and fill it with data
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};

	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::LockWhileLost()
{
	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Lock the resource and fill it with data
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};

	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}


// ----------------------------------------------------------------------------

bool CLostDev::LockWhileLostLOOP()
{
	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	while(true)
	{
		__try
		{
			//Lose the device
			LoseDevice();

			//Lock the resource and fill it with data
			LockData();
			FillAndUnlockData();

			//Restore the device
			ResetDevice();
		}
		__except(1)
		{
			OutputDebugString("Caught an exception in the test");
		}
	}


Error:
	ReleaseResource();

	return true;
}

// ----------------------------------------------------------------------------

bool CLostDev::LostWhileLocked()
{
	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lock the resource
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Fill and Unlock the resource
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::LostWhileRendering()
{
	//Create the resource
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		);

		goto Error;
	};


	//Lock the resource
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		);

		goto Error;
	};


	//Fill and Unlock the resource
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Save the back buffer to compare the results
	if ( !SaveBackBuffer() )
	{
		WriteToLog
		(
			_T( "Failed to save back buffer." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Compare the back buffers
	if ( !CompareBackBuffers() )
	{
		WriteToLog
		(
			_T( "Failed to compare back buffers." )
		);

		goto Error;
	};

	
	//Release the back buffer copy
	RELEASE(m_pBackBufferCopy);

	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	RELEASE(m_pBackBufferCopy);

	ResetDevice();

	return false;
}


// ----------------------------------------------------------------------------
bool CLostDev::CreateAAndBThenLose()
{
	//Create the resource A
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource A." )
		);

		goto Error;
	};


	//Lock resource A
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource A." )
		);

		goto Error;
	};


	//Fill and Unlock resource A
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource A." )
		);

		goto Error;
	};


	//Create the resource B
	if ( !CreateResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to create resource B." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Copy the resource from Res A to Res B
	if ( !CopyResAToResB() )
	{
		WriteToLog
		(
			_T( "Failed to copy resource A to resource B." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}


//////////////////////////////////////////////////////////////////////
// Function:  CreateAAndBThenSuspendSystem
//
// Purpose:
// Creates resource A, locks it, fills it with data, then creates
// resource B, suspends the system to standby or hibernate, then
// copies A to B, renders B, renders A, and compares the results.
//
// The goal here is just to determine whether putting the system to
// sleep causes any unanticipated resource loss or locking problems.
//
// Note:
// LOST_STANDBY and LOST_HIBERNATE are both off by default.  They
// will only be included if asked for by name on the command line.
//////////////////////////////////////////////////////////////////////
bool CLostDev::CreateAAndBThenSuspendSystem()
{
	// Validate our pool type against our D3D version.
	if ( m_pD3D->GetVersion() < 9.1f && m_Pool == POOL_DEFAULT )
	{
		WriteToLog( _T( "CreateAAndBThenSuspendSystem:  Not supported for POOL_DEFAULT before DX 9.L.\n" ) );

		Skip();
		return false;
	};


	// Validate our device loss mechanism.
	if
	(
		m_uLostType != LOST_STANDBY &&
		m_uLostType != LOST_HIBERNATE
	)
	{
		WriteToLog( _T( "CreateAAndBThenSuspendSystem:  Invalid device loss mechanism.\n" ) );

		Skip();
		return false;
	};


	//Create the resource A
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource A." )
		);

		goto Error;
	};


	//Lock the resource A
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource A." )
		);

		goto Error;
	};


	//Fill and Unlock the resource A
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource A." )
		);

		goto Error;
	};


	//Create the resource B
	if ( !CreateResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to create resource B." )
		);

		goto Error;
	};


	//Suspend the system
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Copy the resource from Res A to Res B
	if ( !CopyResAToResB() )
	{
		WriteToLog
		(
			_T( "Failed to copy resource A to resource B." )
		);

		goto Error;
	};


	//Restore the device if necessary
	if ( m_pD3D->GetVersion() <= 9.0f )
	{
		if ( !ResetDevice() )
		{
			WriteToLog
			(
				_T( "Failed to reset device." )
			);

			goto Error;
		};

	};


	//Render the resource
	ClearFrame();
	if ( !DrawResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}


// ----------------------------------------------------------------------------
bool CLostDev::CreateAThenLoseThenCreateB()
{
	//Create the resource A
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource A." )
		);

		goto Error;
	};


	//Lock the resource A
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource A." )
		);

		goto Error;
	};


	//Fill and Unlock the resource A
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource A." )
		);

		goto Error;
	};


	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Create the resource B
	if ( !CreateResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to create resource B." )
		);

		goto Error;
	};


	//Copy the resource from Res A to Res B
	if ( !CopyResAToResB() )
	{
		WriteToLog
		(
			_T( "Failed to copy resource A to resource B." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::LoseThenCreateAAndB()
{
	//Lose the device
	if ( !LoseDevice() )
	{
		WriteToLog
		(
			_T( "Failed to lose device." )
		);

		if ( m_uLostType == LOST_CDSBYOTHERPROCESS )
		{
			WriteToLog
			(
				_T( "This may indicate progressive system failure due to a resource leak in OS components.  Aborting." )
			);

			Abort();
		};

		goto Error;
	};


	//Create the resource A
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource A." )
		);

		goto Error;
	};


	//Lock the resource A
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource A." )
		);

		goto Error;
	};


	//Fill and Unlock the resource A
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to create or unlock resource A." )
		);

		goto Error;
	};


	//Create the resource B
	if ( !CreateResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to create resource B." )
		);

		goto Error;
	};


	//Copy the resource from Res A to Res B
	if ( !CopyResAToResB() )
	{
		WriteToLog
		(
			_T( "Failed to copy resource A to resource B." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		);

		goto Error;
	};


	//Render the resource
	ClearFrame();
	if ( !DrawResourceB() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		);

		goto Error;
	};


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::FUSCreateFUSBack()
{
#ifdef USE_FUS
	CFUS fus;

	//FUS away
	if ( !fus.DisconnectUser() )
	{
		DPF(1, _T("FUSCreateFUSBack() could not Disconnect user\n"));
		goto Error;
	}
	Sleep(5000);

/*	m_bHandlePaintMsg = false;
	MessageBox(m_DeviceList[0]->GetPresParams()->hDeviceWindow, "Click OK, then you need to switch user and wait 20 sec before switching back", "go", MB_OK | MB_APPLMODAL);
	Sleep(20000);
*/	try
	{
		//Create the resource A
		if ( !CreateResource() )
		{
			WriteToLog
			(
				_T( "Failed to create resource." )
			};

			goto Error;
		};
	}
	catch(char* str)
	{
		DPF(1, _T("FUSCreateFUSBack() CreateResource FAULTED\n"));
		//FUS back
		if ( !fus.ReconnectUser() )
		{
			DPF(1, _T("FUSCreateFUSBack() could not Reconnect user\n"));
			goto Error;
		}
		Sleep(5000);
		goto Error;
	}

	//MessageBox(m_DeviceList[0]->GetPresParams()->hDeviceWindow, "This will call Reset and UpdateTexture", "go", MB_OK | MB_APPLMODAL);

	//FUS back
	if ( !fus.ReconnectUser() )
	{
		DPF(1, _T("FUSCreateFUSBack() could not Reconnect user\n"));
		goto Error;
	}
	Sleep(5000);

	//Restore the device
	DPF(1, "RESET\n");
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		};

		goto Error;
	};


	//Lock the resource
	DPF(1, "LOCK\n");
	if ( !LockData() )
	{
		WriteToLog
		(
			_T( "Failed to lock resource." )
		};

		goto Error;
	};


	//Fill and Unlock the resource
	DPF(1, "FILL\n");
	if ( !FillAndUnlockData() )
	{
		WriteToLog
		(
			_T( "Failed to fill or unlock resource." )
		};

		goto Error;
	};


	//Render the resource
	DPF(1, "DRAW\n");
	ClearFrame();
	if ( !DrawResource() )
	{
		WriteToLog
		(
			_T( "Failed to draw." )
		};

		goto Error;
	};


	//Release the resource
	DPF(1, "RELEASE\n");
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Compare with normal result
	DPF(1, "COMPARE\n");
	if ( !NormalResult() )
	{
		WriteToLog
		(
			_T( "Failed to render normal result." )
		);

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();
#endif

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::CreateFUSDeleteFUSBack()
{
#ifdef USE_FUS

	CFUS fus;

	//Create the resource A
	if ( !CreateResource() )
	{
		WriteToLog
		(
			_T( "Failed to create resource." )
		};

		goto Error;
	};


	//FUS away
	if ( !fus.DisconnectUser() )
	{
		DPF(1, _T("FUSCreateFUSBack() could not Disconnect user\n"));
		goto Error;
	}
	Sleep(5000);


	//FUS back
	if ( !fus.ReconnectUser() )
	{
		DPF(1, _T("FUSCreateFUSBack() could not Reconnect user\n"));
		goto Error;
	}
	Sleep(5000);


	//Release the resource
	if ( !ReleaseResource() )
	{
		WriteToLog
		(
			_T( "Failed to release resource." )
		);

		goto Error;
	};


	//Restore the device
	if ( !ResetDevice() )
	{
		WriteToLog
		(
			_T( "Failed to reset device." )
		};

		goto Error;
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();


	return true;

Error:
	ReleaseResource();

	ResetDevice();
#endif

	return false;
}

// ----------------------------------------------------------------------------

bool CLostDev::SaveBackBuffer()
{
	LPSURFACES pSrcSurf = NULL;
	LPSURFACE pSurfA = NULL;
	LPSURFACE pSurfB = NULL;
	LOCKED_RECT lockRectA;
	LOCKED_RECT lockRectB;
	SURFACEDESC	Desc;

	bool bRes = false;
	
	if( !GetBackBuffer(0, BACKBUFFER_TYPE_MONO  , &pSrcSurf, CD3D_SRC) )
	{
		DPF( 1,  _T("SaveBackBuffer() - GetBackBuffer failed\n"));
		goto Finalize;
	}

	if (! CreateImageSurface(m_PresParams.uWidth, m_PresParams.uHeight, m_PresParams.BackBufferFormat, &m_pBackBufferCopy, CD3D_SRC) || !m_pBackBufferCopy )
	{
		DPF( 1, _T("SaveBackBuffer() - CreateTexture failed\n"));
		goto Finalize;
	}

	//if (! CopyRects(pSrcSurf, NULL, 0, m_pBackBufferCopy, NULL, CD3D_SRC) )
	//{
	//	DPF( 1, _T("SaveBackBuffer() - CopyRects failed\n"));
	//	RELEASE(pSrcSurf);
	//	return false;
	//}
	//Copy the backbuffer (lots of stuff to do since we can't copyrects to SCRATCH pool)
	if( !pSrcSurf->GetSurface( &pSurfA, CD3D_SRC -1 ) ) 
	{
		DPF( 1, _T("SaveBackBuffer() - GetSurface A failed\n"));
		goto Finalize;
	}
	if( !m_pBackBufferCopy->GetSurface( &pSurfB, CD3D_SRC -1 ) ) 
	{
		DPF( 1, _T("SaveBackBuffer() - GetSurface A failed\n"));
		goto Finalize;
	}

	if ( FAILED( pSurfA->GetDesc(&Desc) ) )
	{
		DPF( 1, _T("SaveBackBuffer() - GetLevelDesc failed\n"));
		goto Finalize;
	}

	//Lock the 2 surfaces
	if ( FAILED( pSurfA->LockRect(&lockRectA, NULL, D3DLOCK_READONLY) ) )
	{
		DPF( 1, _T("SaveBackBuffer() - LockRect A failed\n"));
		goto Finalize;
	}

	if ( FAILED( pSurfB->LockRect(&lockRectB, NULL, D3DLOCK_READONLY) ) )
	{
		DPF( 1, _T("SaveBackBuffer() - LockRect B failed\n"));
		goto Finalize;
	}

	//Copy A to B
	if ( !CopyFromAToB(&lockRectA, &lockRectB, &Desc) )
	{
		DPF( 1, _T("SaveBackBuffer() - memcpy failed\n"));
		goto Finalize;
	}

	//Unlock the 2 surfaces
	if ( FAILED(pSurfA->UnlockRect() ) )
	{
		DPF( 1, _T("SaveBackBuffer() - UnLockRect A failed\n"));
		goto Finalize;
	}

	if ( FAILED(pSurfB->UnlockRect() ) )
	{
		DPF( 1, _T("SaveBackBuffer() - UnLockRect B failed\n"));
		goto Finalize;
	}

	bRes = true;

Finalize:
	RELEASE(pSurfA);
	RELEASE(pSurfB);

	RELEASE(pSrcSurf);

	return bRes;
}

// ----------------------------------------------------------------------------

bool CLostDev::CompareBackBuffers()
{
	bool bRes = true;
	LPSURFACE pSrcSurf = NULL;
	LPSURFACE pCopy = NULL;

	DPF(4, _T("CLostDev::CompareBackBuffers\n"));

	// Get the src buffer.
	HRESULT hr = m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	if(FAILED(hr))
	{
		DPF
		(
			1,
			_T("CompareBackBuffers:  GetBackBuffer on SRC failed with error 0x%x (%s).\n"),
			hr,
			m_pD3D->HResultToString(hr)
		);

		bRes = false;
		goto Error;
	};
		

	// Get the saved back buffer.
	if( !m_pBackBufferCopy->GetSurface( &pCopy, CD3D_SRC - 1 ) ) 
	{
		DPF( 1, _T("CompareBackBuffers:  GetSurface on copy failed.\n"));

		bRes = false;
		goto Error;
	};


	// Compare the buffers.
	if(FAILED(m_pFramework->GetImageCmp()->CompareImages(pSrcSurf, pCopy, NULL)))
	{
		DPF( 1, _T("CompareBackBuffers:  CompareImages failed.\n"));

		bRes = false;
		goto Error;
	};


	// Check the results.
	m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();
	if ( m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired() )
	{
		DPF
		(
			1,
			_T("CompareBackBuffers:  Image comparison fails.  Expected:  %2.2f.  Actual:  %2.2f.\n"),
			m_pFramework->GetImageCmp()->GetPassRequired() * 100.f,
			m_fPassPercentage * 100.f
		);

		FailTest();

		bRes = true;
		goto Error;
	};


Error:
	RELEASE(pSrcSurf);
	RELEASE(pCopy);

	return bRes;
}

// ----------------------------------------------------------------------------

bool CLostDev::LoseDevice()
{
	bool bDeviceLost = true;
	switch(m_uLostType)
	{
		case LOST_CHANGEDISPLAY:
		{
			// Change and restore display mode to lose the device.
			bDeviceLost = LoseTheDevice(D3DLOSEDEV_DEFAULT);
			if (!bDeviceLost)
			{
				DPF( 0, _T("CLostDev::LoseDevice:  LoseTheDevice(D3DLOSEDEV_DEFAULT) failed.\n"));
			}
		} break;


		case LOST_CDSBYOTHERPROCESS:
		{
			// Call another process that will change and restore display mode to lose the device.
			bDeviceLost = LoseTheDevice(D3DLOSEDEV_BYOTHERPROCESS);
			if (!bDeviceLost)
			{
				DPF( 0, _T("CLostDev::LoseDevice:  LoseTheDevice(D3DLOSEDEV_BYOTHERPROCESS) failed.\n"));
			}
		} break;


		case LOST_FUS:
		{
			// Change and restore user to lose the device.
			bDeviceLost = LoseTheDevice(D3DLOSEDEV_BYFUS);
			if (!bDeviceLost)
			{
				DPF( 0, _T("CLostDev::LoseDevice:  LoseTheDevice(D3DLOSEDEV_BYFUS) failed.\n"));
			}
		} break;


		case LOST_STANDBY:
		{
			// Suspend the system to stand-by.
			bDeviceLost = LoseTheDevice(D3DLOSEDEV_BYSTANDBY);
			if (!bDeviceLost)
			{
				DPF( 0, _T("CLostDev::LoseDevice:  LoseTheDevice(D3DLOSEDEV_BYSTANDBY) failed.\n"));
			}
		} break;


		case LOST_HIBERNATE:
		{
			// Suspend the system to hibernate.
			bDeviceLost = LoseTheDevice(D3DLOSEDEV_BYHIBERNATE);
			if (!bDeviceLost)
			{
				DPF( 0, _T("CLostDev::LoseDevice:  LoseTheDevice(D3DLOSEDEV_BYHIBERNATE) failed.\n"));
			}
		} break;


		default:
		{
			// Huh?!
			DPF( 1, _T("LostDev Bug: invalid m_uLostType : %d\n"), m_uLostType);
		} break;
	}


	//
	// Verify that the device was lost if we expected it to be.
	//
	// When do we / don't we expect it to be lost?
	// - We don't expect it to be lost via any of these mechanisms on 9.L.
	// - On 9.0c and previous versions, we expect the device to be lost
	//   due to a mode change by another app and by desktop switch.
	// - Standby and hibernate might or might not cause device
	//   loss, depending on how the machine is configured for power management,
	//   and since we don't know, we won't check.
	//

	if
	(
		bDeviceLost == true &&
		m_pD3D->GetVersion() < 9.1f &&
		m_uLostType != LOST_STANDBY &&
		m_uLostType != LOST_HIBERNATE
	)
	{
		MuteLog();
		DWORD LostDevices = GetLostDevices();
		UnMuteLog();

		if ( LostDevices == 0 )
		{
			DPF
			(
				1,
				_T("CLostDev::LoseDevice:  The device was NOT lost for DX %2.1f device.\n"),
				m_pD3D->GetVersion()
			);

			return false;
		};
	};


	// This is a fix to make sure that ProcessLastError won't
	// fail the test if the device has been lost.
	GetLastError();

	return bDeviceLost;
}

// ----------------------------------------------------------------------------

bool CLostDev::ResetDevice(ULONG lDev)
{
	__try
	{
		if((m_pD3D->GetVersion() >= 8.0f))
		{
			//We need to release the D3DXFont otherwise Reset can fail
			m_DeviceList[0]->D3DXFontOnLostDevice();

			//Reset the swap chain
			if (! Reset(&m_PresParams) )
			{
				HRESULT hLastError = GetLastError();
				if
				(
					hLastError == D3DERR_OUTOFVIDEOMEMORY
				)
				{
					DPF( 1, _T("CLostDev::ResetDevice:  Reset failed with error D3DERR_OUTOFVIDEOMEMORY.\n"));

					m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
					m_pFramework->SuggestExitCode(EXITCODE_ABORT);

					Abort();
					return false;
				}
				else if
				(
					hLastError == E_OUTOFMEMORY
				)
				{
					DPF( 1, _T("CLostDev::ResetDevice:  Reset failed with error E_OUTOFMEMORY.\n"));

					m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
					m_pFramework->SuggestExitCode(EXITCODE_ABORT);

					Abort();
					return false;
				};


				if ( !m_pFramework->IsLogMute() )
				{
					if (! Reset(&m_PresParams) )
					{
						DPF( 1, _T("CLostDev::ResetDevice:  Reset failed.\n"));

						Abort();
						return false;
					}
				}
				else
				{
					DPF( 1, _T("CLostDev::ResetDevice:  Reset failed.\n"));
					return false;
				}
			}

			//We need to reset the D3DXFont
			m_DeviceList[0]->D3DXFontOnResetDevice();
		}
		else
		{
			if (!ResetLostDevices())
			{
				DPF( 1, _T("CLostDev::ResetDevice:  ResetLostDevices failed.\n"));
				return false;
			}
		}
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF( 1, _T("CLostDev::ResetDevice:  Exception caught while resetting device.\n"));
		return false;
	};


	//
	// Verify that our devices are no longer lost.
	//

	MuteLog();
	DWORD LostDevices = GetLostDevices();
	UnMuteLog();

	if ( ( LostDevices & lDev ) != 0 )
	{
		DPF( 1, _T( "CLostDev::ResetDevice:  Device still lost after attempted reset.\n" ));
		return false;
	};

	return true;
}


FMT CLostDev::GetValidFormat(RESOURCETYPE rtype)
{
	const dwTextureFormatListSize = 32;
	FMT TextureFormatList[dwTextureFormatListSize] = 
	{
		FMT_A8R8G8B8,
		FMT_X8R8G8B8,
		FMT_R5G6B5,
//		FMT_X1R5G5B5, // Commenting out most formats to speed up test execution.
//		FMT_A1R5G5B5,
//		FMT_A4R4G4B4,
//		FMT_R3G3B2,
//		FMT_A8,
//		FMT_A8R3G3B2,
//		FMT_X4R4G4B4,
//		FMT_R8G8B8,
//		FMT_A2B10G10R10,
	// MPL:Tex
	//    FMT_A8B8G8R8,
	//    FMT_X8B8G8R8,
//		FMT_G16R16,

//		FMT_A8P8,
//		FMT_P8,

//		FMT_L8,
//		FMT_A8L8,
//		FMT_A4L4,

//		FMT_V8U8,
//		FMT_L6V5U5,
//		FMT_X8L8V8U8,
//		FMT_Q8W8V8U8,
//		FMT_V16U16,
//		FMT_W11V11U10,
	// MPL:Tex
	//    FMT_W10V11U11,
//		FMT_A2W10V10U10,
	// MPL:Tex
	//    FMT_A8X8V8U8,
	//    FMT_L8X8V8U8,
		
//		FMT_UYVY,
//		FMT_YUY2,
		FMT_DXT1,
//		FMT_DXT2,
//		FMT_DXT3,
//		FMT_DXT4,
//		FMT_DXT5
	};

	if(m_pD3D->GetVersion() >= 8.0f)
	{
		for (int i=0; i<dwTextureFormatListSize; i++)
		{
			HRESULT hr = m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(),
													m_pSrcDevice->GetDevType(),
													m_pCurrentMode->pDisplay->Format,
													0,
													rtype,
													TextureFormatList[i]);
			if (SUCCEEDED(hr))
			{
				return TextureFormatList[i];
			}
		}

		return FMT_ANY;
	}
	else
	{
	   UINT     uTextureFormatCount;
	   FORMAT*  pFormats;

	   switch(rtype)
	   {
	   case RTYPE_TEXTURE:
		   uTextureFormatCount  = m_uCommonTextureFormats;
		   pFormats             = m_pCommonTextureFormats;
		   break;

	   case RTYPE_CUBETEXTURE:
		   uTextureFormatCount  = m_uCommonCubeFormats;
		   pFormats             = m_pCommonCubeFormats;
		   break;

	   case RTYPE_VOLUMETEXTURE:
		   uTextureFormatCount  = m_uCommonVolumeFormats;
		   pFormats             = m_pCommonVolumeFormats;
		   break;

	   default:
		   uTextureFormatCount  = 0;
		   pFormats             = NULL;
	   }

	   if(0 != uTextureFormatCount)
	   {
		   return (FMT)(pFormats[0].d3dfFormat);
	   }
	}

	return FMT_ANY;
}

void CLostDev::DumpTestDetails(void)
{
	//DPF(1, "	PixelFormat :  %s\n", m_szSurfaceFormat);
	DPF(1, "    Pool :     %s\n", m_szPoolType);
	DPF(1, "    Lost by :  %s\n", m_szLostType);
}

void CLostDev::FailTest()
{
	//For dx6, we don't care about the results, so we always succeed
	if (m_pD3D->GetVersion() < 7.0f)
		return;

	DumpTestDetails();
	Fail();
}