//
// DESC: D3D context switch conformance test
// FILE: D3dContextSwitch.cpp
// COMMENTS:
// OWNER: Youssef Barakat (YoussefB)
//

#include "ContextSwitcher.h"
#include <fstream>

//
// Returns the size rounded up to a multiple of the page size
//
#define ROUND_TO_PAGES(Size, PageSize) \
	(((ULONG_PTR)(Size) + (ULONG_PTR)(PageSize) - 1) & ~((ULONG_PTR)(PageSize) - 1))

#define CREATE_MESSAGE(errorMessage) \
	FormatMessage( \
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, \
		NULL, \
		::GetLastError(), \
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
		(LPTSTR) &errorMessage, \
		0,  \
		NULL);

CD3DWindowFramework App;
CSwitcher Switcher;

//
// A structure for our custom vertex type
//
typedef struct _CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
} CUSTOMVERTEX, *PCUSTOMVERTEX;

//
// A structure for our boundary test vertex type
// No specular, diffuse, etc.
//
typedef struct _BTESTVERTEX
{
	FLOAT x, y, z, rhw;
} BTESTVERTEX, *PBTESTVERTEX;

//
// Our custom FVF, which describes our custom vertex structure
//
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

//
// This vertex is added to test a boundary condition where some cards try to read
// beyond the vertex buffer by assuming there's always a specular, etc. This vertex
// will be aligned at the end of the vertex buffer with a page guard after it.
//
#define D3DFVF_BTESTVERTEX (D3DFVF_XYZRHW)

//
// Define some triangles to use
//
CUSTOMVERTEX RedTriangle[] =
{
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0xff, 0x00, 0x00, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0xff, 0x00, 0x00, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0xff, 0x00, 0xff) }
};

CUSTOMVERTEX GreenTriangle[] =
{
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0xff, 0x00, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0xff, 0x00, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0x00, 0xff, 0xff) }
};

CUSTOMVERTEX BlueTriangle[] =
{
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0x00, 0xff, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0x00, 0x00, 0xff, 0xff) },
	{ 0.0f, 0.0f, 0.5f, 2.0f, RGBA_MAKE(0xff, 0x00, 0x00, 0xff) }
};

PBTESTVERTEX PageAlignedTriangle = NULL;

//
// Array of possible modes
//
MODELIST gDevModes[3];
MODELIST *pSwitchDevMode = NULL;

//
// Pointer to the current mode
//
PMODELIST gDevMode;

CSwitcher::CSwitcher()
{
	//
	// This is just the generic Framework startup code
	// we set the type to the enumerate CONF, and set the 
	// TestName and Command Key.
	//
	SetTestType(TESTTYPE_CONF);
	m_szCommandKey = "ContextSwitcher";

	//
	// Now set up our options from the commad line
	//

	//
	// We default to 2 DX versions (9, 8)
	//
	gNumberOfDXVersions = 2;

	m_Options.D3DOptions.dwIncVersions = (DXVERSION_9_OR_91 | DXVERSION_8_OR_81);

	if (KeySet("AddDx7") == true)
	{
		gNumberOfDXVersions++;
		m_Options.D3DOptions.dwIncVersions |= DXVERSION_7;
	}

	if (KeySet("AddDx6") == true)
	{
		gNumberOfDXVersions++;
		m_Options.D3DOptions.dwIncVersions |= DXVERSION_6;
	}

	if (KeySet("!focus") != true)
	{
		gFlags.Set(CF_CHANGE_FOCUS);
	}

	if (KeySet("!res") != true)
	{
		gFlags.Set(CF_CHANGE_RES);
	}

	if (KeySet("!validate") != true)
	{
		gFlags.Set(CF_VALIDATE);
	}

	if (KeySet("!append") != true)
	{
		gFlags.Set(CF_APPEND);
	}

	//
	// If the "alternate" option has been passed in
	// then we only want to run the alternate process
	// and this process should be marked as such,
	// otherwise this must be the primary process
	// NOTE: Setting 'alternate' and 'primary' are
	// mutually exclusive options!! If 'alternate' is set
	// no primary process is going to excute whatsoever.
	//
	if (KeySet("alternate"))
	{
		gFlags.Set(CF_ALTERNATE);

		m_szTestName = "ContextSwitcher -- Alternate";

		//
		// Initialize some global indicies
		//
		gTestChangeIndex = TEST_CHANGE_ALTERNATE_FRAME_INDEX;
		gFocusChangeIndex = FOCUS_CHANGE_ALTERNATE_FRAME_INDEX;
		gResChangeIndex = RES_CHANGE_ALTERNATE_FRAME_INDEX;

		//
		// Make sure we log to a new log file
		//
		LPTSTR pLogName = const_cast<LPTSTR>(m_szLogName);
		COPYSTRING(pLogName, ALT_LOGNAME);

		//
		// We will be switching the alternate process to FullScreen
		// unless the !fullscreen option is set
		//
		if (KeySet("!fullscreen") != true)
		{
			m_Options.D3DOptions.dwWinMode |= WINMODE_FULLSCREEN;
			m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_DISCARD;
		}
	}
	else
	{
		gFlags.Set(CF_PRIMARY);

		m_szTestName = "ContextSwitcher -- Primary";

		gTestChangeIndex = TEST_CHANGE_PRIMARY_FRAME_INDEX;
		gFocusChangeIndex = FOCUS_CHANGE_PRIMARY_FRAME_INDEX;
		gResChangeIndex = RES_CHANGE_PRIMARY_FRAME_INDEX;

		//
		// If the primary flag is set, then set
		// the PRIMARY_ONLY flag which means
		// don't spin off the alternate process
		//
		if (KeySet("primary"))
		{
			gFlags.Set(CF_PRIMARY_ONLY);
		}
		else
		{
			LPTSTR pLogName = const_cast<LPTSTR>(m_szLogName);
			COPYSTRING(pLogName, PRI_LOGNAME );
		}
	}

	m_Options.TestOptions.bSkipOnDeviceLost = true;
	gMutex = NULL;

	CSCreateMutex();
}


CSwitcher::~CSwitcher()
{
	MyCleanup();

	if(gMutex)
		CloseHandle(gMutex);

	if (!((gFlags.Check(CF_PRIMARY_ONLY)) || (gFlags.Check(CF_ALTERNATE))))
	{
		//
		// We spun off another thread, let's merge the logs
		//
		if (gFlags.Check(CF_APPEND))
		{
			Merge();
		}
	}
}


VOID CSwitcher::CommandLineHelp(VOID)
{
	WriteCommandLineHelp("$yprimary: $wprimary context only $c(default=primary and alternate created)");
	WriteCommandLineHelp("$yalternate: $walternate context only $c(default=primary and alternate created)");
	WriteCommandLineHelp("$ytests: $wnumber of tests $c(default=max=%d)", DEFAULT_NUMBER_OF_TESTS);
	WriteCommandLineHelp("$yAddDx7: $wadd Dx7 tests $c(default=disabled)");
	WriteCommandLineHelp("$yAddDx6: $wadd Dx6 tests $c(default=disabled)");
	WriteCommandLineHelp("$y!focus: $wdisable focus grab $c(default=attempted every %d frames)", gFocusChangeIndex);
	WriteCommandLineHelp("$y!res: $wdisable resolution change $c(default=attempted every %d frames)", gResChangeIndex);
	WriteCommandLineHelp("$y!fullscreen: $wdisable fullscreen alternate $c(default=enabled)");
	WriteCommandLineHelp("$y!validate: $wdisable device validation $c(default=enabled)");
	WriteCommandLineHelp("$y!append: $wdisable log merge $c(default=enabled)");
}


bool CSwitcher::CapsCheck(VOID)
{
	//
	// We will override the default behavior and always 
	// return true for a caps check
	//
	return true;
}


bool CSwitcher::Setup(VOID)
{
	//
	// This function setups up our device dependant settings if the device is lost 
	// we should get called in "CleanUp" and then called in "Setup" again.
	//
	return true;
}

UINT CSwitcher::TestInitialize(VOID)
{
	SYSTEM_INFO sysInfo;
	ULONG_PTR allocationSize;
	DWORD oldProtection;
	DWORD bTestVertexBufferSize;
	DWORD numberOfTests;
	DWORD dwIndex = 0, dwModeIndex = 0;
	DWORD dwWidth = 0, dwHeight = 0;

	//
	// Initialize the general vertices
	//
	RedTriangle[0].x = 2.0f;
	RedTriangle[0].y = 2.0f;

	RedTriangle[1].x = (D3DVALUE) (m_pCurrentMode->nBackBufferWidth - 2);
	RedTriangle[1].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);

	RedTriangle[2].x = 2.0f;
	RedTriangle[2].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);

	GreenTriangle[0].x = 2.0f;
	GreenTriangle[0].y = 2.0f;

	GreenTriangle[1].x = (D3DVALUE) (m_pCurrentMode->nBackBufferWidth - 2);
	GreenTriangle[1].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);

	GreenTriangle[2].x = 2.0f;
	GreenTriangle[2].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);

	BlueTriangle[0].x = 2.0f;
	BlueTriangle[0].y = 2.0f;

	BlueTriangle[1].x = (D3DVALUE) (m_pCurrentMode->nBackBufferWidth - 2);
	BlueTriangle[1].y = 2.0f;

	BlueTriangle[2].x = (D3DVALUE) (m_pCurrentMode->nBackBufferWidth - 2);
	BlueTriangle[2].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);

	//
	// Initialize the boundary test vertex buffer. We need to allocate memory that
	// is page-aligned, to do that we need to calculate how many bytes we need and
	// how many pages that will span then round up to the nearest number of pages. We
	// will then store our data at the end of that buffer (to be page-aligned) and change the
	// pointer to point to the start of our buffer. We will also allocate a Guard Page at the end
	// of the buffer to make sure no one overwrites that memory.
	//

	if (PageAlignedTriangle == NULL)
	{
		//
		// First, populate the system information structure to get the page size to be
		// 64-bit and LARGE_PAGE compliant.
		//
		GetSystemInfo(&sysInfo);

		//
		// We know we need atleast 3 vertices for a triangle, and we need to round
		// upwards to the nearest PageSize
		//
		bTestVertexBufferSize = (sizeof(BTESTVERTEX)*3);
		allocationSize = ROUND_TO_PAGES(bTestVertexBufferSize, sysInfo.dwPageSize);

		//
		// Now allocate all the memory including the guard page
		//
		PageAlignedTriangle = (PBTESTVERTEX)VirtualAlloc(
			NULL,
			allocationSize+sysInfo.dwPageSize,
			MEM_COMMIT,
			PAGE_READWRITE);

		if (PageAlignedTriangle == NULL)
		{
			LPVOID errorMessage;

			CREATE_MESSAGE(errorMessage);
			WriteToLog("VirtualAlloc() failed: last error <%s>\n", errorMessage);
			LocalFree(errorMessage);

			return D3DTESTINIT_ABORT;
		}
  
		//
		// Change the protection on the last page (guard page)
		//
		if (VirtualProtect(
			(PBYTE)PageAlignedTriangle+allocationSize,
			sysInfo.dwPageSize,
			PAGE_NOACCESS,
			&oldProtection) == 0)
		{
			LPVOID errorMessage;

			CREATE_MESSAGE(errorMessage);
			WriteToLog("VirtualProtect() failed: last error <%s>\n", errorMessage);
			LocalFree(errorMessage);

			return D3DTESTINIT_ABORT;
		}


		//
		// Move our pointer to point to the start of our aligned buffer
		//
		PageAlignedTriangle = (PBTESTVERTEX)((PBYTE)PageAlignedTriangle+(allocationSize -bTestVertexBufferSize));

		PageAlignedTriangle[0].x = 2.0f;
		PageAlignedTriangle[0].y = 2.0f;
		PageAlignedTriangle[0].z = 0.5f;
		PageAlignedTriangle[0].rhw = 2.0f;

		PageAlignedTriangle[1].x = (D3DVALUE) (m_pCurrentMode->nBackBufferWidth - 2);
		PageAlignedTriangle[1].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);
		PageAlignedTriangle[1].z = 0.5f;
		PageAlignedTriangle[1].rhw = 2.0f;

		PageAlignedTriangle[2].x = 2.0f;
		PageAlignedTriangle[2].y = (D3DVALUE) (m_pCurrentMode->nBackBufferHeight - 2);
		PageAlignedTriangle[2].z = 0.5f;
		PageAlignedTriangle[2].rhw = 2.0f;
	}
	
	dwModeIndex = 0;
	dwWidth = 1024;
	dwHeight = 768;
	pSwitchDevMode = gDevModes;	// Initialize the switch device Mode to the first one

	do{
		for( dwIndex = 0; dwIndex < m_uCommonDisplayModes; dwIndex++)
		{
			if( (m_pCommonDisplayModes[dwIndex].dwWidth == dwWidth) && 
				(m_pCommonDisplayModes[dwIndex].dwHeight == dwHeight))
			{
				gDevModes[dwModeIndex].DeviceMode.dmSize = sizeof(DEVMODE);
				gDevModes[dwModeIndex].DeviceMode.dmDriverExtra = 0;
				gDevModes[dwModeIndex].DeviceMode.dmPelsWidth = dwWidth;
				gDevModes[dwModeIndex].DeviceMode.dmPelsHeight = dwHeight;
				gDevModes[dwModeIndex].DeviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
				if( dwModeIndex < 2 )
					gDevModes[dwModeIndex].NextMode = &gDevModes[dwModeIndex + 1];
				if( dwWidth == 800 )	// If there is 800x600, let switch at that Mode
					pSwitchDevMode = &gDevModes[dwModeIndex];
				++ dwModeIndex;
				break;
			}
		}

		if( dwWidth == 1024)
		{
			dwWidth = 800;
			dwHeight = 600;
		}
		else if( dwWidth == 800)
		{
			dwWidth = 640;
			dwHeight = 480;
		}
		else
			dwWidth = 0;	// Done

	}while( dwWidth );

	if( !dwModeIndex )
		return D3DTESTINIT_ABORT;
	else
	{
		--dwModeIndex;
		gDevModes[dwModeIndex].NextMode = &gDevModes[0];
		gDevMode = &gDevModes[0];
	}

	//
	// Check to see if we need to spin off an alternate process
	// The condition to do that are: 1. we must be a primary process
	// 2. "Primary" key cannot be set.
	//
	if ((gFlags.Check(CF_PRIMARY)) && (gFlags.Check(CF_PRIMARY_ONLY) != true))
	{
		PCHAR commandLine = NULL;
		PCHAR newCommandLine = NULL;
		STARTUPINFO startup;

		if ((commandLine = GetCommandLine()) == NULL)
		{
			LPVOID errorMessage;

			CREATE_MESSAGE(errorMessage);
			WriteToLog("GetCommandLine() failed: last error <%s>\n", errorMessage);
			LocalFree(errorMessage);

			return D3DTESTINIT_ABORT;
		}

		startup.cb = sizeof(STARTUPINFO);
		GetStartupInfo(&startup);
    
		newCommandLine = (PCHAR) LocalAlloc(LPTR,(strlen(commandLine) + 12));
		sprintf(newCommandLine, "%s -alternate", commandLine);
  
		if (CreateProcess(NULL, newCommandLine, 0, 0, 0, 0, 0, 0, &startup, &gProcessInfo) != TRUE)
		{ 
			LPVOID errorMessage;

			CREATE_MESSAGE(errorMessage);
			WriteToLog("CreateProcess() failed: last error <%s>\n", errorMessage);
			LocalFree(errorMessage);

			//Delete our new Command Line
			if(newCommandLine != NULL)
			LocalFree(newCommandLine);
				  
			return D3DTESTINIT_ABORT;
		}

		//Delete our new Command Line
		if(newCommandLine != NULL)
		LocalFree(newCommandLine);
	}

	 //Create the "ContextSwitcher" Mutex
	ReadInteger("tests", DEFAULT_NUMBER_OF_TESTS, (PINT)&numberOfTests);

	//
	// Number of tests can never be negative since it's unsigned
	//
	numberOfTests = (numberOfTests > DEFAULT_NUMBER_OF_TESTS) ? DEFAULT_NUMBER_OF_TESTS : numberOfTests;

	if (gFlags.Check(CF_PRIMARY))
	{
		//
		// We need to run the test on the primary process about once
		// for every dx version we're testing against. For now, we're
		// testing against 2 versions (9, 8) so we need 2 times more 
		// runs while the other process runs.
		//
		numberOfTests = ADJUSTED_TESTS(numberOfTests);
		SetTestRange (1, numberOfTests);
	}
	else
	{
		SetTestRange (1, numberOfTests);

		//
		// Move the window so that the primary/alternate windows don't overlap
		//
		SetWindowPos(g_pFramework->m_hWindow, 0, 256, 256, 0, 0, (SWP_NOZORDER | SWP_NOSIZE));
	}

	if(!gMutex)
		return D3DTESTINIT_ABORT;

	return D3DTESTINIT_RUN;
}

bool CSwitcher::ExecuteTest(UINT uTest)
{
	bool status = true;
	HRESULT result;
	LPVOID vertices = NULL;
	DWORD FVF = D3DFVF_CUSTOMVERTEX;

	ClearFrame();

	gFlags.Clear(CF_SKIP);
	bProcessFrameFlag = true;

	BeginTestCase(m_szTestName, uTest);

	switch(uTest % gTestChangeIndex)
	{
		case 0:
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);
			vertices = RedTriangle;
		break;

		case 1:
			vertices = GreenTriangle;
		break;

		case 2:
			vertices = BlueTriangle;
		break;

		//
		// The next are for the ALTERNATE process only:
		//
		case 3:
			vertices = GreenTriangle;
		break;

		case 4:
			vertices = BlueTriangle;
		break;

		case 5:
			//
			// Use the special page-guarded vertex buffer
			//
			vertices = PageAlignedTriangle;
			FVF = D3DFVF_BTESTVERTEX;
		break;

		case 6:
		{
			LPSURFACES pNewRenderTarget = NULL;
			LPSURFACES pOldRenderTarget = NULL;
			DWORD renderTargetIndex=0;

			//
			// First set the vertex buffer
			//
			vertices = GreenTriangle;

			//
			// Change our render state from the primary context
			//
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD) FALSE);

			//
			// If the next mode is 800x600 mode (we must be in 640x480), invalidate the
			// surface and do a context switch
			//
			if ((pSwitchDevMode == gDevMode) &&
			(m_pD3D->GetVersion() >= 9.f))
			{
				// Its not valid to Compare Image in this mode.
				bProcessFrameFlag = false;

				if (GetRenderTarget(&pOldRenderTarget, 1) == TRUE)
				{
					//
					// Create a temp surface for rendering
					//
					status = CreateRenderTarget(
						m_pCurrentMode->nBackBufferWidth, 
						m_pCurrentMode->nBackBufferHeight,
						FMT_A8R8G8B8,
						MULTISAMPLE_NONE,
						true,
						&pNewRenderTarget,
						1);

					if (status == TRUE)
					{
						//
						// Set our render target to the new one, and destroy it
						//
						if (SetRenderTarget(renderTargetIndex, pNewRenderTarget, 1) == TRUE)
						{
							pNewRenderTarget->Release();

							//
							// Wait for the other process to do some rendering
							//
							Sleep(2000);

							SetRenderTarget(renderTargetIndex, pOldRenderTarget, 1);
						}
						else
						{
							pNewRenderTarget->Release();
						}
					}
					else
					{
						result = GetLastError();
					}

					RELEASE(pOldRenderTarget);
				}
			}

			break;
		}

		case 7:
			vertices = RedTriangle;
		break;

		default:
		break;
	}

	if( Validate())
	{
		if (BeginScene() == true)
		{
			if (RenderPrimitive(D3DPT_TRIANGLELIST, FVF, vertices, 3, NULL, 0, NULL) == true)
			{
				if (EndScene() == true)
				{
					//
					// Make this window the foreground and focus window every gFocusChangeIndex frames
					//

					if(gMutex)
					{
						DWORD dwWaitState = WaitForSingleObject( gMutex, 5000L);
						if((dwWaitState == WAIT_OBJECT_0) || (dwWaitState == WAIT_ABANDONED))
						{
							__try 
							{ 
								if ((gFlags.Check(CF_CHANGE_FOCUS)) && ((uTest % gFocusChangeIndex) == 0))
								{
									bProcessFrameFlag = false;
									SetForegroundWindow(m_pFramework->m_pDisplayWindow->m_hWindow);
									Sleep(500);
								}

								if ((gFlags.Check(CF_CHANGE_RES)) && ((uTest % gResChangeIndex) == 0) && (m_pCurrentMode->bWindowed))
								{
									bProcessFrameFlag = false;
									ChangeDisplaySettings(&gDevMode->DeviceMode, 0);
									gDevMode = gDevMode->NextMode;
									Sleep(500);
								}
							} 
							__finally 
							{
								// Release ownership of the mutex object.
								if (! ReleaseMutex(gMutex)) 
								{ 
									DPF(1, "CSwitcher::ProcessFrame ReleaseMutex failed\n");
								}
							}
						}
					}
				}
				else
				{
					result = GetLastError();

					if (!IsLost(result))
					{
						WriteToLog("EndScene() failed: (%s)\n", GLE(result));
						Fail();
					}
					else
						SkipTests(1);

					status = false;
				}
			}
			else
			{
				result = GetLastError();

				if (!IsLost(result))
				{
					WriteToLog("RenderPrimitive() failed: (%s)\n", GLE(result));
					Fail();
				}
				else
					SkipTests(1);

				EndScene();
				status = false;
			}
		}
		else
		{
			result = GetLastError();

			if (!IsLost(result))
			{
				WriteToLog("BeginScene() failed: (%s)\n", GLE(result));
				Fail();
			}
			else
				SkipTests(1);

			status = false;
		}
	}
	else
	{
		SkipTests(1);
		status = false;
	}

	return status;
}


bool CSwitcher::DisplayFrame(VOID)
{
	bool status = true;


	if (Validate())
	{
		status = CD3DTest::DisplayFrame();
		if (status != true)
		{
			HRESULT result = GetLastError();

			//
			// If the device is lost, don't log a failure
			//
			if (IsLost(result))
			{
				SkipTests(1);
				gFlags.Set(CF_SKIP);
			}

			DPF(1, "CSwitcher::DisplayFrame() failed: %s\n", GLE(result));
			SetLastError(result);
		}
	}
	else
	{
		gFlags.Set(CF_SKIP);
		SkipTests(1);
	}

   return status;
}

bool CSwitcher::TestTerminate(VOID)
{
	bool status = true;

	//
	// If this test was run as PRIMARY ONLY or ALTERNATE
	// this means we didn't spin off any new processes
	// and we can just get out. Otherwise, the 'not' of the entire
	// line means we DID spin off another process
	//
	if (!((gFlags.Check(CF_PRIMARY_ONLY)) || (gFlags.Check(CF_ALTERNATE))))
	{
		//
		// We spun off another thread, let's just wait until it's done please
		//
		if (gProcessInfo.hProcess != NULL)
		{
			WaitForSingleObject(gProcessInfo.hProcess, INFINITE);
		}
	}

	return status;
}


void CSwitcher::MyCleanup()
{
	DWORD bTestVertexBufferSize;
	DWORD allocationSize;
	SYSTEM_INFO sysInfo;

	GetSystemInfo(&sysInfo);

	bTestVertexBufferSize = (sizeof(BTESTVERTEX)*3);
	allocationSize = (DWORD)ROUND_TO_PAGES(bTestVertexBufferSize, sysInfo.dwPageSize);


	VirtualFree(PageAlignedTriangle, 0, MEM_RELEASE); // Size must be 0 if MEM_RELEAS
	PageAlignedTriangle = NULL;
}


bool CSwitcher::IsLost(HRESULT Result)
{
	bool status = false;

	if ((Result == D3DERR_DEVICELOST) || (Result == DDERR_SURFACELOST) || (Result == DDERR_SURFACEBUSY))
	{
//      WriteToLog("CSwitcher::IsLost(): Lost device scenario detected\n");
		status = true;
	}

	return status;
}


bool CSwitcher::Validate(VOID)
{
	bool status = true;
	DWORD passes;
	HRESULT result;

	if (gFlags.Check(CF_SKIP))
	{
		gFlags.Clear(CF_SKIP);
		return true;
	}

	if (gFlags.Check(CF_VALIDATE))
	{
		if (((ValidateDevice(&passes, CD3D_SRC) != true) || (passes > 1)) ||
		((ValidateDevice(&passes, CD3D_REF) != true) || (passes > 1)))
		{
			status = false;
		}
	}

	return status;
}

bool CSwitcher::ProcessFrame(void)
{
	bool bStatus = true;
	DWORD dwWaitState = 0;
	// Check for device lost and handle it as appropriate

	if(gMutex)
	{
		dwWaitState = WaitForSingleObject( gMutex, 5000L);
		if((dwWaitState == WAIT_OBJECT_0) || (dwWaitState == WAIT_ABANDONED))
		{
			__try 
			{ 
				// If the device is lost, There is no reason to compare images
				gFlags.Set(CF_VALIDATE);
				if (bProcessFrameFlag && !IsDeviceLost() && Validate())
				{
					bStatus = CD3DTest::ProcessFrame();
				}
			} 
			__finally 
			{ 
				// Release ownership of the mutex object.
				if (! ReleaseMutex(gMutex)) 
				{ 
					DPF(1, "%s CSwitcher::ProcessFrame ReleaseMutex failed\n", m_szTestName);
				}
			}
		}
	}

	return bStatus;
}


/*
	Create the mutex to keep the test frame Work from changing the display mode, 
	and while processing the frame data.
*/
void CSwitcher::CSCreateMutex(void)
{
	gMutex = CreateMutex(NULL, 0, CONTEXT_SWITCHER_MUTEX_NAME );

	if( !gMutex )
	{
			LPVOID errorMessage;       
			CREATE_MESSAGE(errorMessage);
			DPF(1, "CreateMutex failed (Error == %s)\n", errorMessage );
			LocalFree(errorMessage);
	}
}


void CSwitcher::ProcessError(HRESULT hr)
{
	if( (hr == D3DERR_DEVICELOST) || (hr == D3DERR_DEVICENOTRESET) || 
		(hr == DDERR_SURFACELOST) || (hr == DDERR_SURFACEBUSY) )
	{
		gFlags.Set(CF_SKIP);
		SkipTests(1);
		CD3DTest::ProcessError(D3DERR_DEVICELOST);
	}
	else if( hr==E_FAIL)
	{
		gFlags.Set(CF_VALIDATE);
		if (!IsDeviceLost() && Validate())
		{
			// Set the E_FAIL error back.
			SetLastError( E_FAIL );
			CD3DTest::ProcessError(hr);
		}
		else
		{
			gFlags.Set(CF_SKIP);
			SkipTests(1);
		}
	}
	else
	{
		CD3DTest::ProcessError(hr);
	}
}


bool CSwitcher::Merge(void)
{
	HANDLE hMerFile;
	TCHAR *sAltPath = new TCHAR[MAX_PATH];
	TCHAR *sPriPath = new TCHAR[MAX_PATH];
	TCHAR *sMerPath = new TCHAR[MAX_PATH];
	TCHAR *sTmpPath = new TCHAR[MAX_PATH];
	TCHAR *sWinPath = new TCHAR[MAX_PATH];

	//
	// This merge creates another file for 
	// dumping the contents of the primary
	// and the alternate logs. The reason I chose
	// to create another file instead of just appending
	// the alternate to the primary is because we
	// can't get WRITE privileges to the primary before
	// we exit the app.
	//

	// primary file
	if( !GetWindowsDirectory( sWinPath, MAX_PATH) )
		return false;

	_stprintf(sPriPath, _T("%s%s%s%s"), sWinPath, _T("\\dxlogs\\"), 
		PRI_LOGNAME, _T(".wtl"));

	  
	// Alternate file
	_stprintf(sAltPath, _T("%s%s%s%s"), sWinPath, _T("\\dxlogs\\"), 
		ALT_LOGNAME, _T(".wtl"));

	// Merge file
	_stprintf(sMerPath, _T("%s%s%s%s"), sWinPath, _T("\\dxlogs\\"), 
		CS_LOGNAME, _T(".wtl"));


	hMerFile = CreateFile(
		sMerPath,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hMerFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMerFile);
		// Temp file
		_stprintf(sTmpPath, _T("%s%s%s%s"), sWinPath, _T("\\dxlogs\\"), 
			TMP_LOGNAME, _T(".wtl"));
			
		MoveFile(sMerPath, sTmpPath);
		MergeHead(sMerPath, sTmpPath);
		MergeMiddle(sMerPath, sPriPath);
		DeleteFile(sTmpPath);
		MergeTail(sMerPath, sAltPath);
	}
	else
	{
		MergeHead(sMerPath, sPriPath);
		MergeTail(sMerPath, sAltPath);
	}

	DeleteFile(sAltPath);
	DeleteFile(sPriPath);

	delete []sAltPath;
	delete []sPriPath;
	delete []sMerPath;
	delete []sTmpPath;
	delete []sWinPath;

	return true;
}

bool CSwitcher::FindString(PCHAR pBuff, PCHAR tszString)
{
	int i, Len = strlen( tszString );
	
	for(i = 0; i < Len; i++ )
	{
		if( pBuff[i] != tszString[i] )
			break;
	}

	if( i < Len )
		return false;
	else
		return true;
}


bool CSwitcher::MergeHead(TCHAR *FileMerge, TCHAR *FileName)
{
	HANDLE hPriFile;
	HANDLE hMerFile;
	DWORD NumberOfBytesRead;
	DWORD NumberOfBytesWritten;
	DWORD NumberOfBytesInFile;
	PCHAR mBuffer = NULL;
	PCHAR tBuffer = NULL;
	int	maxSearch;
	int StartPos;
	bool bAppendFlags = false;

		
	hMerFile = CreateFile( FileMerge,
		GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

 	if (hMerFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	hPriFile = CreateFile(
		FileName,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (hPriFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMerFile);
		return false;
	}

	//
	// Read the primary process' log
	//
	NumberOfBytesInFile = GetFileSize(hPriFile, NULL);

	tBuffer = (PCHAR) malloc (NumberOfBytesInFile + sizeof(WTT_LOGGER_TAIL));

	if (tBuffer == NULL)
	{
		CloseHandle(hMerFile);
		CloseHandle(hPriFile);
		return false;
	}

	memset(tBuffer, '\0', NumberOfBytesInFile);

	ReadFile(
		hPriFile,
		tBuffer,
		NumberOfBytesInFile,
		&NumberOfBytesRead,
		NULL);

	// Find the WTT_LOGGER_TAIL
	maxSearch = NumberOfBytesRead;
	StartPos = 0;
	for( int i =0; i < maxSearch; i++ )
	{
	   if( FindString( &tBuffer[i], WTT_LOGGER_TAIL) )
		{
			NumberOfBytesRead = i - (StartPos + EOL_LENGTH);
			break;
		}
	}
	//
	// Write to the merged file
	//
	WriteFile(
		hMerFile,
		&tBuffer[StartPos],
		NumberOfBytesRead,
		&NumberOfBytesWritten,
		NULL);

	//
	// We're done with the primary now
	//
	CloseHandle(hPriFile);
	CloseHandle(hMerFile);

	if( tBuffer) free(tBuffer);

	return true;
}


bool CSwitcher::MergeTail(TCHAR *FileMerge, TCHAR *FileName)
{
	HANDLE hPriFile;
	HANDLE hMerFile;
	DWORD NumberOfBytesRead;
	DWORD NumberOfBytesWritten;
	DWORD NumberOfBytesInFile;
	PCHAR buffer = NULL;
	int	maxSearch;
	int StartPos;
	bool bAppendFlags = false;


	hMerFile = CreateFile(
		FileMerge,
		GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

 	if (hMerFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	LARGE_INTEGER lNumber;
	lNumber.QuadPart = 0;
	SetFilePointerEx(hMerFile, lNumber, &lNumber, FILE_END);

	hPriFile = CreateFile(
		FileName,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (hPriFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMerFile);
		return false;
	}

	//
	// Read the primary process' log
	//
	NumberOfBytesInFile = GetFileSize(hPriFile, NULL);
	buffer = NULL;
	buffer = (PCHAR) malloc (NumberOfBytesInFile + sizeof(WTT_LOGGER_TAIL));

	if (buffer == NULL)
	{
		CloseHandle(hMerFile);
		CloseHandle(hPriFile);
		return false;
	}

	memset(buffer, '\0', NumberOfBytesInFile);

	ReadFile(
		hPriFile,
		buffer,
		NumberOfBytesInFile,
		&NumberOfBytesRead,
		NULL);

	maxSearch = NumberOfBytesRead - sizeof(WTT_LOGGER_HEAD);
	StartPos = 0;
	for( int i =0; i < maxSearch; i++ )
	{
		if( FindString( &buffer[i], WTT_LOGGER_HEAD) )
		{
			StartPos = i + sizeof(WTT_LOGGER_HEAD);
			NumberOfBytesRead -= (StartPos +1);
			break;
		}
	}

	//
	// Write to the merged file
	//
	WriteFile(
		hMerFile,
		&buffer[StartPos],
		NumberOfBytesRead,
		&NumberOfBytesWritten,
		NULL);

	//
	// We're done with the primary now
	//
	CloseHandle(hPriFile);
	CloseHandle(hMerFile);

	if( buffer) free(buffer);

	return true;
}

bool CSwitcher::MergeMiddle(TCHAR *FileMerge, TCHAR *FileName)
{
	HANDLE hPriFile;
	HANDLE hMerFile;
	DWORD NumberOfBytesRead;
	DWORD NumberOfBytesWritten;
	DWORD NumberOfBytesInFile;
	PCHAR buffer = NULL;
	int	maxSearch;
	int StartPos;
	bool bMiddleFlags = false;


	hMerFile = CreateFile(
		FileMerge,
		GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

 	if (hMerFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	NumberOfBytesInFile = GetFileSize(hMerFile, NULL);
	if(NumberOfBytesInFile)
		bMiddleFlags = true;

	LARGE_INTEGER lNumber;
	lNumber.QuadPart = 0;
	SetFilePointerEx(hMerFile, lNumber, &lNumber, FILE_END);

	hPriFile = CreateFile(
		FileName,
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (hPriFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//
	// Read the primary process' log
	//
	NumberOfBytesInFile = GetFileSize(hPriFile, NULL);
	buffer = NULL;
	buffer = (PCHAR) malloc (NumberOfBytesInFile + sizeof(WTT_LOGGER_TAIL));

	if (buffer == NULL)
	{
		return false;
	}

	memset(buffer, '\0', NumberOfBytesInFile);

	ReadFile(
		hPriFile,
		buffer,
		NumberOfBytesInFile,
		&NumberOfBytesRead,
		NULL);

	maxSearch = NumberOfBytesRead;
	StartPos = 0;

	for( int i =0; i < maxSearch; i++ )
	{
		if( FindString( &buffer[i], WTT_LOGGER_HEAD) )
		{
			StartPos = i + sizeof(WTT_LOGGER_HEAD);
		}
		if( FindString( &buffer[i], WTT_LOGGER_TAIL) )
		{
			NumberOfBytesRead = i - StartPos;
			break;
		}
	}

	//
	// Write to the merged file
	//
	WriteFile(
		hMerFile,
		&buffer[StartPos],
		NumberOfBytesRead,
		&NumberOfBytesWritten,
		NULL);

	//
	// We're done with the primary now
	//
	CloseHandle(hPriFile);
	CloseHandle(hMerFile);

	if( buffer) free(buffer);

	return true;
}
