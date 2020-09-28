#include "D3DTestPCH.h"
#include "Framework.h"
#include "CTextures.h"
#include "CSwapChain.h"

#include <string>
#include <vector>

#include <ExceptionTrace.h>

#include <d3d9.h>
#include <ddraw.h>
#include <strsafe.h>


#if defined(_PREFAST_)
#pragma prefast(disable: 12110)
#pragma prefast(disable: 13502)
#pragma prefast(disable: 13503)
#endif

#define TERMINATE_NUMBER    0xFFFFFFFF

GETFUNC                 g_DCTGet=NULL;
COMPAREFUNC             g_DCTCompare=NULL;
HINSTANCE               g_hDCTDLL=NULL;


#if !defined(D3DERR_DEFERRED_DP2ERROR)
#define D3DERR_DEFERRED_DP2ERROR MAKE_D3DHRESULT(2158)
#endif


typedef basic_string< TCHAR > tstring;
class CD3DExceptionTrace
	:
	public CExceptionTrace
{
protected:
	std::vector< tstring > m_stackTrace;
	tstring m_exceptionTypeString;

	virtual void Init()
	{
		m_stackTrace.clear();
	}


	virtual int FilterProc( EXCEPTION_POINTERS *ep )
	{
		//
		// Validate parameters.
		//

		if ( ep == NULL )
		{
			return EXCEPTION_CONTINUE_SEARCH;
		};

		if ( ep->ExceptionRecord == NULL )
		{
			return EXCEPTION_CONTINUE_SEARCH;
		};


		//
		// Grab the exception code.
		//

		m_exceptionCode = ep->ExceptionRecord->ExceptionCode;


		//
		// Decide what to do with the exception.
		//

		switch ( m_exceptionCode )
		{
			//
			// Let the debugger handle these.
			//

			case EXCEPTION_BREAKPOINT:
			case EXCEPTION_SINGLE_STEP:
			{
				return EXCEPTION_CONTINUE_SEARCH;
			} break;


			//
			// These exceptions might represent a logic
			// error in the test or the component under
			// test.  We should log a failure, along with
			// as much information as possible as to the
			// state of the machine at the time of the
			// failure.
			//

			case EXCEPTION_ACCESS_VIOLATION:
			case EXCEPTION_FLT_DENORMAL_OPERAND:
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			case EXCEPTION_FLT_INEXACT_RESULT:
			case EXCEPTION_FLT_INVALID_OPERATION:
			case EXCEPTION_FLT_OVERFLOW:
			case EXCEPTION_FLT_STACK_CHECK:
			case EXCEPTION_FLT_UNDERFLOW:
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			case EXCEPTION_INT_OVERFLOW:
			{
				return EXCEPTION_EXECUTE_HANDLER;
			} break;


			//
			// We don't try to handle any other exceptions,
			// because they may require arbitrarily complex
			// cleanup and recovery, or may not even be
			// recoverable at all.  In these cases, the
			// test is in a completely indeterminate state,
			// and is very likely making matters worse than
			// just exiting.
			//

			default:
			{
				return EXCEPTION_CONTINUE_SEARCH;
			} break;
		};
	}


	virtual void StackTraceCallback( const tstring &traceString )
	{
		m_stackTrace.push_back( traceString );
	}


public:	
	size_t GetTraceDepth() const { return m_stackTrace.size(); }
	const tstring &GetTrace( size_t i ) const { return m_stackTrace[i]; }
};


// ======================================================================================
//
//  CD3DTest Static Data
//
// ======================================================================================

static CD3DExceptionTrace s_ExceptionTrace;


RENDERPRIMITIVEAPI CD3DTest::m_pRPDefaultAPIList[6] =
{
	RP_DP,                      // DrawPrimitive
	RP_DIP,                     // DrawIndexedPrimitive
	RP_DPS,                     // DrawPrimitiveStrided
	RP_DIPS,                    // DrawIndexedPrimitiveStrided
	RP_DPVB,                    // DrawPrimitiveVB
	RP_DIPVB                    // DrawIndexedPrimitiveVB
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullAPIList[16] =
{
	RP_BE,                      // Begin/Vertex/End
	RP_BIE,                     // BeginIndexed/Index/End
	RP_DP,                      // DrawPrimitive
	RP_DIP,                     // DrawIndexedPrimitive
	RP_DPS,                     // DrawPrimitiveStrided
	RP_DIPS,                    // DrawIndexedPrimitiveStrided
	RP_DPVB,                    // DrawPrimitiveVB
	RP_DIPVB,                   // DrawIndexedPrimitiveVB
	RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
	RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
	RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
	RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
	RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
	RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
	RP_DPVB_PVS,                // DrawPrimitiveVB after ProcessVerticesStrided
	RP_DIPVB_PVS                // DrawIndexedPrimitiveVB after ProcessVerticesStrided
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX5APIList[4] =
{
	RP_BE,                      // Begin/Vertex/End
	RP_BIE,                     // BeginIndexed/Index/End
	RP_DP,                      // DrawPrimitive
	RP_DIP,                     // DrawIndexedPrimitive
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX6APIList[14] =
{
	RP_BE,                      // Begin/Vertex/End
	RP_BIE,                     // BeginIndexed/Index/End
	RP_DP,                      // DrawPrimitive
	RP_DIP,                     // DrawIndexedPrimitive
	RP_DPS,                     // DrawPrimitiveStrided
	RP_DIPS,                    // DrawIndexedPrimitiveStrided
	RP_DPVB,                    // DrawPrimitiveVB
	RP_DIPVB,                   // DrawIndexedPrimitiveVB
	RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
	RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
	RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
	RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
	RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
	RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX7APIList[14] =
{
	RP_DP,                      // DrawPrimitive
	RP_DIP,                     // DrawIndexedPrimitive
	RP_DPS,                     // DrawPrimitiveStrided
	RP_DIPS,                    // DrawIndexedPrimitiveStrided
	RP_DPVB,                    // DrawPrimitiveVB
	RP_DIPVB,                   // DrawIndexedPrimitiveVB
	RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
	RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
	RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
	RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
	RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
	RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
	RP_DPVB_PVS,                // DrawPrimitiveVB after ProcessVerticesStrided
	RP_DIPVB_PVS                // DrawIndexedPrimitiveVB after ProcessVerticesStrided
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX8APIList[6] =
{
	RP_DP,                      // DrawPrimitiveUP
	RP_DIP,                     // DrawIndexedPrimitiveUP
	RP_DPVB,                    // DrawPrimitive (FVF shader)
	RP_DIPVB,                   // DrawIndexedPrimitive (FVF shader)
	RP_DPS,                     // DrawPrimitive (multistream shader)
	RP_DIPS,                    // DrawIndexedPrimitive (multistream shader)
};


DWORD CD3DTest::m_dwRPDefaultAPIListSize = 6;
DWORD CD3DTest::m_dwRPFullAPIListSize = 16;
DWORD CD3DTest::m_dwRPFullDX5APIListSize = 4;
DWORD CD3DTest::m_dwRPFullDX6APIListSize = 14;
DWORD CD3DTest::m_dwRPFullDX7APIListSize = 14;
DWORD CD3DTest::m_dwRPFullDX8APIListSize = 6;


// ======================================================================================
//
//  Helper Functions
//
// ======================================================================================

void SimulateAnException()
{
	//
	// Fake up some exception context information.
	//

	EXCEPTION_RECORD ExceptionRecord;
	ZeroMemory( &ExceptionRecord, sizeof( ExceptionRecord ) );
	ExceptionRecord.ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
	ExceptionRecord.ExceptionRecord = NULL;
	ExceptionRecord.ExceptionAddress = &SimulateAnException;
	ExceptionRecord.NumberParameters = 0;

	CONTEXT Context;
	ZeroMemory( &Context, sizeof( Context ) );

	EXCEPTION_POINTERS ExceptionPointers;
	ZeroMemory( &ExceptionPointers, sizeof( ExceptionPointers ) );
	ExceptionPointers.ExceptionRecord = &ExceptionRecord;
	ExceptionPointers.ContextRecord = &Context;


	//
	// Call our exception filter to simulate an exception.
	//

	ExceptionTraceFilter
	(
		&ExceptionPointers,
		&s_ExceptionTrace
	);
}


// ======================================================================================
//
//  CD3DTest Functions
//
// ======================================================================================

CD3DTest::CD3DTest(void)
{
	m_uDeviceList = 0;
	m_uCurrentMode = 0;
	m_uTotalModes = 0;
	m_pModeList = NULL;
	m_pCurrentMode = NULL;
	m_nTagHeight = 32;
	m_bShowCompareWindow = false;
	m_bCompareEnabled = false;
	m_bHandlePaintMsg = false;
	m_bException = true;
	m_pD3D = NULL;
	m_pCmpSwapChain = NULL;
	m_pCmpTexture = NULL;
	m_pSrcDevice = NULL;
	m_pRefDevice = NULL;
	m_szNewLogName = NULL;
	m_bCmdLineMode = false;
	m_bStretchPresent = true;
	m_bHaltOnFail = false;
	m_pCommonZBufferFormats = NULL;
	m_pCommonTextureFormats = NULL;
	m_pCommonCubeFormats = NULL;
	m_pCommonVolumeFormats = NULL;
	m_pCommonBackBufferFormats = NULL;
	m_pCommonMultiSampleFormats = NULL;
	m_pFramework = g_pFramework;
	m_pFramework->AddTest(this);
	m_bReverseMode = false;
	m_bGenGolden = false;
	m_fPrevPassRequired = 0.f;

	ZeroMemory( &m_Options, sizeof( m_Options ) );
	ZeroMemory( &m_pWindowMultiHead, MAX_DEVICES * sizeof( CWindow* ) );
}


CD3DTest::~CD3DTest(void)
{
	if (m_szNewLogName)
	{
		delete [] m_szNewLogName;
	}

	for( UINT uWindowMultiHead = 1; uWindowMultiHead < MAX_DEVICES; uWindowMultiHead++ )
	{
		if( NULL != m_pWindowMultiHead[uWindowMultiHead] )
		{
			m_pWindowMultiHead[uWindowMultiHead]->Close();
			delete m_pWindowMultiHead[uWindowMultiHead];
			m_pWindowMultiHead[uWindowMultiHead] = NULL;
		}
	}
}


bool CD3DTest::ParseCmdLine()
{
	TCHAR tcsBuffer[512] = _T( "" );
	TCHAR szSwDllName[256] = _T( "" );
	TCHAR szSwDllEntryPoint[64] = _T( "" );
	DWORD dwIncludeVersion = 0;
	DWORD dwExcludeVersion = 0;

	// Check for WinMode options
	if(ReadString(_T("WinMode"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		if(!_tcsicmp(_T("fullscreen"), tcsBuffer) || !_tcsicmp(_T("full"), tcsBuffer))
			m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
		if(!_tcsicmp(_T("window"), tcsBuffer) || !_tcsicmp(_T("win"), tcsBuffer))
			m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
		if(!_tcsicmp(_T("all"), tcsBuffer))
			m_Options.D3DOptions.dwWinMode = WINMODE_ALL;
	}
	// Check for backbuffer override
	if((m_Options.TestOptions.dwCaps & TESTCAPS_RESIZEMODE) && ReadString(_T("bb"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		if(!ParseResolution(tcsBuffer, &m_Options.D3DOptions.nBackBufferWidth, &m_Options.D3DOptions.nBackBufferHeight, 
			&m_Options.ModeOptions.BackBufferFmt))
		{
			DPF(1, _T("CD3DTest::ParseCmdLine:  Invalid display mode string.  Using defaults.\n"));
		}
	}

	// Check for DisplayMode override
	if(ReadString(_T("DisplayMode"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		if(!ParseResolution(tcsBuffer, (UINT*)&m_Options.D3DOptions.DisplayMode.dwWidth,(UINT*)&m_Options.D3DOptions.DisplayMode.dwHeight,
			(FMT*)&m_Options.D3DOptions.DisplayMode.Format.d3dfFormat))
		{
			DPF(1, _T("CD3DTest::ParseCmdLine:  Invalid display mode string.  Using defaults.\n"));
		}
		else
		{
			d3df2ddpf(m_Options.D3DOptions.DisplayMode.Format.d3dfFormat, &m_Options.D3DOptions.DisplayMode.Format.ddpfPixelFormat);
			m_Options.D3DOptions.DisplayMode.dwBPP = m_Options.D3DOptions.DisplayMode.Format.ddpfPixelFormat.dwRGBBitCount;
			m_bCmdLineMode = true;
		}
	}

	if(KeySet(_T("!Compare")))
		m_Options.TestOptions.bCompareSrcRef = false;

	if(KeySet(_T("SrcOnly")))
		m_Options.D3DOptions.bReference = false;

	if(KeySet(_T("FBCompare")))
		m_Options.TestOptions.bCompareFB = true;

	if(KeySet(_T("DebugHelper")))
		m_Options.D3DOptions.bD3DDebugHelper = true;

	if(KeySet(_T("!DisableIdle")))
		m_Options.TestOptions.bDisableIdleDetection = false;
	else
		m_Options.TestOptions.bDisableIdleDetection = true;

	if(KeySet(_T("!Stretch")) || m_Options.TestOptions.bCompareFB)
		m_bStretchPresent = false;

	if(KeySet(_T("!LogAppActivation")))
		m_Options.TestOptions.bLogAppActivation = false;
	else
		m_Options.TestOptions.bLogAppActivation = true;
	//
	// Check for "BreakOnCorruption".   BreakOnCorruption triggers a DebugBreak()
	// in response to WDDM Aperture Corruption detection.   This is primarily used 
	// during test automation to intentionally tie up the machine and prevent 
	// other tests from running on it.   Because aperture corruption is typically 
	// permanent, this prevents additional tests from running and failing.
	// 
	// Aperature corruption detection occurs as a post process (not real time).
	// Therefore "BreakOnCorruption" does not necessarily improve your ability to 
	// debug the cause of the corruption.
	// "BreakOnCorruption" is overridden by "DisableCorruption".
	//
	if(KeySet(_T("BreakOnCorruption")))
		m_Options.TestOptions.bBreakOnCorruptionDetection = true;
	// BreakOnCorruptionDectection is set to zero (false) by default
	// inside constructor.

	if(KeySet(_T("DisableCorruption")))
		m_Options.TestOptions.bDisableCorruptionDetection = true;
	else 
		m_Options.TestOptions.bDisableCorruptionDetection = false;

	if(KeySet(_T("WHQL")))
	{
		m_Options.TestOptions.dwCaps &= ~TESTCAPS_RESIZEWINDOW;
		if (!KeySet(_T("Mobile")))  // Don't force WHQL modes for mobile devices
		{
			m_Options.D3DOptions.DisplayMode.dwWidth = MODEOPT_WHQL;
			m_Options.D3DOptions.DisplayMode.dwHeight = MODEOPT_WHQL;
		}

		m_Options.TestOptions.bDisableCorruptionDetection = false;  // Always do corruption detection for WHQL
	}

	if(KeySet(_T("MultiHead")))
		m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_ADAPTERGROUP_DEVICE;
	if(KeySet(_T("MultiThreaded")))
		m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_MULTITHREADED;
	if(KeySet(_T("HaltOnFail")))
		m_bHaltOnFail = true;
	if(KeySet(_T("GenGolden")) && m_Options.D3DOptions.bGoldReference)
	{
		m_bGenGolden = true;
		m_Options.D3DOptions.bReference = false;
	}

	// Check for device overrides
	if(ReadString(_T("REF"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		// Convert to uppercase
		_tcsupr(tcsBuffer);

		// Get the device type
		if(_tcsstr(tcsBuffer, _T("HAL")))
			m_Options.D3DOptions.dwRefDevType = DEVICETYPE_HAL;
		else if(_tcsstr(tcsBuffer, _T("NULLREF")))
			m_Options.D3DOptions.dwRefDevType = DEVICETYPE_NULLREF;
		else if(_tcsstr(tcsBuffer, _T("REF")))
			m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		else if(_tcsstr(tcsBuffer, _T("SW")))
			m_Options.D3DOptions.dwRefDevType = DEVICETYPE_SW;


		if(_tcsstr(tcsBuffer, _T("SVP")))
			m_Options.D3DOptions.dwRefBehaviors	= DEVICECREATE_SWVP;
		else if(_tcsstr(tcsBuffer, _T("HVP")))
			m_Options.D3DOptions.dwRefBehaviors	= DEVICECREATE_HWVP;

		if(_tcsstr(tcsBuffer, _T("MX")))
			m_Options.D3DOptions.dwRefBehaviors	|= DEVICECREATE_MIXED;
		else if(_tcsstr(tcsBuffer, _T("PURE")))
			m_Options.D3DOptions.dwRefBehaviors	= DEVICECREATE_HWVP | DEVICECREATE_PURE;
		//else if(_tcsstr(tcsBuffer, _T("MAN")))
		//	m_Options.D3DOptions.dwRefBehaviors	|= DEVICECREATE_MANAGED;
		else // DEFAULT
			m_Options.D3DOptions.dwRefBehaviors	|= DEVICECREATE_STANDARD;
	}

	// Check software settings
	if(	ReadString(_T("SWDLL"),  szSwDllName, ARRAYSIZE(szSwDllName) ) && 
		ReadString(_T("SWDLLENTRY"), szSwDllEntryPoint, ARRAYSIZE(szSwDllEntryPoint) ))
	{
		m_pD3D->SetSWRast(szSwDllName, szSwDllEntryPoint);
	}

	// Check for runtime DLL switch
	if(ReadString(_T("SRCD3D"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		m_pD3D->SetRuntimeDLLName(tcsBuffer, NULL);
	}
	if(ReadString(_T("REFD3D"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		m_pD3D->SetRuntimeDLLName(NULL, tcsBuffer);
	}

	// Check for FPU flags
	if(!KeySet(_T("!FPUPRESERVE")) || KeySet(_T("WHQL")))
	{
		m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_FPU_PRESERVE;
		m_Options.D3DOptions.dwRefBehaviors |= DEVICECREATE_FPU_PRESERVE;
	}


	//
	// Work around tests that don't set dwIncVersions explicitly,
	// but rely instead on the min and/or max DX versions.
	//

	if ( m_Options.D3DOptions.dwIncVersions == 0 )
	{
		if
			(
			m_Options.D3DOptions.fMinDXVersion <= 6.0f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 6.0f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_6;
		};

		if
			(
			m_Options.D3DOptions.fMinDXVersion <= 7.0f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 7.0f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_7;
		};

		if
			(
			m_Options.D3DOptions.fMinDXVersion <= 8.1f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 8.1f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_8_OR_81;
		}
		else if
			(
			m_Options.D3DOptions.fMinDXVersion <= 8.0f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 8.0f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_8;
		};

		if
			(
			m_Options.D3DOptions.fMinDXVersion <= 9.1f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 9.1f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_9_OR_91;
		}
		else if
			(
			m_Options.D3DOptions.fMinDXVersion <= 9.0f &&
			( m_Options.D3DOptions.fMaxDXVersion >= 9.0f || m_Options.D3DOptions.fMaxDXVersion == 0.f )
			)
		{
			m_Options.D3DOptions.dwIncVersions |= DXVERSION_9;
		};
	};


	//
	// Process DX version inclusions from the command line.
	//

	if
	(
		KeySet(_T("latestdx")) ||
		KeySet(_T("dx9")) ||
		KeySet(_T("dx9.1")) ||
		KeySet(_T("dx9.0")) ||
		KeySet(_T("dx8")) ||
		KeySet(_T("dx8.1")) ||
		KeySet(_T("dx8.0")) ||
		KeySet(_T("dx7")) ||
		KeySet(_T("dx7.0")) ||
		KeySet(_T("dx6")) ||
		KeySet(_T("dx6.0"))
	)
	{
		dwIncludeVersion = 0;

		if(KeySet(_T("latestdx")))
			dwIncludeVersion |= DXVERSION_LATEST;

		if(KeySet(_T("dx9")))
			dwIncludeVersion |= DXVERSION_9_OR_91;
		if(KeySet(_T("dx9.1")))
			dwIncludeVersion |= DXVERSION_91;
		if(KeySet(_T("dx9.0")))
			dwIncludeVersion |= DXVERSION_9;

		if(KeySet(_T("dx8")))
			dwIncludeVersion |= DXVERSION_8_OR_81;
		if(KeySet(_T("dx8.1")))
			dwIncludeVersion |= DXVERSION_81;
		if(KeySet(_T("dx8.0")))
			dwIncludeVersion |= DXVERSION_8;

		if(KeySet(_T("dx7")) || KeySet(_T("dx7.0")))
			dwIncludeVersion |= DXVERSION_7;

		if(KeySet(_T("dx6")) || KeySet(_T("dx6.0")))
			dwIncludeVersion |= DXVERSION_6;
	};


	//
	// Process DX version exclusions from the command line.
	//

	if(KeySet(_T("!dx9")))
		dwExcludeVersion |= DXVERSION_9_OR_91 | DXVERSION_9 | DXVERSION_91;
	if(KeySet(_T("!dx9.1")))
		dwExcludeVersion |= DXVERSION_91;
	if(KeySet(_T("!dx9.0")))
		dwExcludeVersion |= DXVERSION_9;

	if(KeySet(_T("!dx8")))
		dwExcludeVersion |= DXVERSION_8_OR_81 | DXVERSION_8 | DXVERSION_81;
	if(KeySet(_T("!dx8.1")))
		dwExcludeVersion |= DXVERSION_81;
	if(KeySet(_T("!dx8.0")))
		dwExcludeVersion |= DXVERSION_8;

	if(KeySet(_T("!dx7")) || KeySet(_T("!dx7.0")))
		dwExcludeVersion |= DXVERSION_7;

	if(KeySet(_T("!dx6")) || KeySet(_T("!dx6.0")))
		dwExcludeVersion |= DXVERSION_6;


	// Decode the special test include version LATEST.
	float LatestDXVersion = min(m_pD3D->GetMaxVersion(), m_Options.D3DOptions.fMaxDXVersion);
	DWORD dwLatestDXIncVersion = GetDXVersionFlag(LatestDXVersion);
	if(m_Options.D3DOptions.dwIncVersions & DXVERSION_LATEST)
	{
		m_Options.D3DOptions.dwIncVersions |= dwLatestDXIncVersion;
		m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_LATEST;
	};


	// If there are command line DX version restrictions to process...
	if ( 0 != dwIncludeVersion )
	{
		// ...and the command line requests 9.0 or 9.1 (but not both)...
		if ( ( dwIncludeVersion & DXVERSION_9_OR_91 ) != 0 )
		{
			// ...and the latest available DX version is 9.0...
			if ( dwLatestDXIncVersion == DXVERSION_9 )
			{
				// ...then use 9.0...
				dwIncludeVersion |= DXVERSION_9;
				dwIncludeVersion &= ~DXVERSION_91;
				dwIncludeVersion &= ~DXVERSION_9_OR_91;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_9;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_91;
			}
			// ...otherwise, if the test requests 9.0 or 9.1 (but not both)...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91 ) != 0 )
			{
				// ...then life is good...
			}
			// ...otherwise, if the test requests 9.1 only...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_91 ) != 0 )
			{
				// ...then make sure that it runs 9.1 only...
				dwIncludeVersion |= DXVERSION_91;
				dwIncludeVersion &= ~DXVERSION_9_OR_91;
				dwIncludeVersion &= ~DXVERSION_9;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_91;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9;
			}
			// ...or, if the test requests 9.0 only...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9 ) != 0 )
			{
				// ...then make sure that it runs 9.0 only...
				dwIncludeVersion |= DXVERSION_9;
				dwIncludeVersion &= ~DXVERSION_9_OR_91;
				dwIncludeVersion &= ~DXVERSION_91;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_9;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_91;
			};
		}
		// ...otherwise, ...
		else
		{
			// ...if the command line requests 9.1...
			if ( ( dwIncludeVersion & DXVERSION_91 ) != 0 )
			{
				// ...and the test requests 9.0 or 9.1 (but not both)...
				if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91 ) != 0 )
				{
					// ...then make sure that it runs 9.1 only...
					dwIncludeVersion |= DXVERSION_91;
					dwIncludeVersion &= ~DXVERSION_9_OR_91;
					dwIncludeVersion &= ~DXVERSION_9;

					m_Options.D3DOptions.dwIncVersions |= DXVERSION_91;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9;
				};
			};

			// ...or if the command line requests 9.0...
			if ( ( dwIncludeVersion & DXVERSION_9 ) != 0 )
			{
				// ...and the test requests 9.0 or 9.1 (but not both)...
				if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91 ) != 0 )
				{
					// ...then make sure that it runs 9.0 only...
					dwIncludeVersion |= DXVERSION_9;
					dwIncludeVersion &= ~DXVERSION_9_OR_91;
					dwIncludeVersion &= ~DXVERSION_91;

					m_Options.D3DOptions.dwIncVersions |= DXVERSION_9;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_91;
				};
			};
		};
	};


	// If there are more command line DX version restrictions to process...
	if ( 0 != dwIncludeVersion )
	{
		// ...if the command line requests 8.0 or 8.1 (but not both)...
		if ( ( dwIncludeVersion & DXVERSION_8_OR_81 ) != 0 )
		{
			// ...and the latest available DX version is 8.0...
			if ( dwLatestDXIncVersion == DXVERSION_8 )
			{
				// ...then use 8.0...
				dwIncludeVersion |= DXVERSION_8;
				dwIncludeVersion &= ~DXVERSION_81;
				dwIncludeVersion &= ~DXVERSION_8_OR_81;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_8;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_81;
			}
			// ...otherwise, if the test requests 8.0 or 8.1 (but not both)...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81 ) != 0 )
			{
				// ...then life is good...
			}
			// ...otherwise, if the test requests 8.1 only...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_81 ) != 0 )
			{
				// ...then make sure that it runs 8.1 only...
				dwIncludeVersion |= DXVERSION_81;
				dwIncludeVersion &= ~DXVERSION_8_OR_81;
				dwIncludeVersion &= ~DXVERSION_8;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_81;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8;
			}
			// ...or, if the test requests 8.0 only...
			else if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8 ) != 0 )
			{
				// ...then make sure that it runs 8.0 only...
				dwIncludeVersion |= DXVERSION_8;
				dwIncludeVersion &= ~DXVERSION_8_OR_81;
				dwIncludeVersion &= ~DXVERSION_81;

				m_Options.D3DOptions.dwIncVersions |= DXVERSION_8;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_81;
			};
		}
		// ...otherwise, ...
		else
		{
			// ...if the command line requests 8.1...
			if ( ( dwIncludeVersion & DXVERSION_81 ) != 0 )
			{
				// ...and the test requests 8.0 or 8.1 (but not both)...
				if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81 ) != 0 )
				{
					// ...then make sure that it runs 8.1 only...
					dwIncludeVersion |= DXVERSION_81;
					dwIncludeVersion &= ~DXVERSION_8_OR_81;
					dwIncludeVersion &= ~DXVERSION_8;

					m_Options.D3DOptions.dwIncVersions |= DXVERSION_81;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8;
				};
			};

			// ...or if the command line requests 8.0...
			if ( ( dwIncludeVersion & DXVERSION_8 ) != 0 )
			{
				// ...and the test requests 8.0 or 8.1 (but not both)...
				if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81 ) != 0 )
				{
					// ...then make sure that it runs 8.0 only...
					dwIncludeVersion |= DXVERSION_8;
					dwIncludeVersion &= ~DXVERSION_8_OR_81;
					dwIncludeVersion &= ~DXVERSION_81;

					m_Options.D3DOptions.dwIncVersions |= DXVERSION_8;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
					m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_81;
				};
			};
		};
	};


	// If there are still command line DX version restrictions to process...
	if ( 0 != dwIncludeVersion )
	{
		// Restrict the test's requested versions by the command line's requested versions.
		m_Options.D3DOptions.dwIncVersions &= dwIncludeVersion;
	};


	// If the command line specifies DX version exclusions...
	if ( 0 != dwExcludeVersion )
	{
		// If the command line prohibits 9.1...
		if ( ( dwExcludeVersion & DXVERSION_91 ) != 0 )
		{
			// ...and the test requests 9.0 or 9.1 (but not both)...
			if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91 ) != 0 )
			{
				// ...and the command line doesn't also prohibit 9.0...
				if ( ( dwExcludeVersion & DXVERSION_9 ) == 0 )
				{
					// ...then run 9.0...
					m_Options.D3DOptions.dwIncVersions |= DXVERSION_9;
				};

				// ...and note that we've removed the ambiguity.
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
			};
		};

		// If the command line prohibits 9.0...
		if ( ( dwExcludeVersion & DXVERSION_9 ) != 0 )
		{
			// ...and the test requests 9.0 or 9.1 (but not both)...
			if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91 ) != 0 )
			{
				// ...and the command line doesn't also prohibit 9.1...
				if ( ( dwExcludeVersion & DXVERSION_91) == 0 )
				{
					// ...then run 9.1...
					m_Options.D3DOptions.dwIncVersions |= DXVERSION_91;
				};

				// ...and note that we've removed the ambiguity.
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
			};
		};

		// If the command line prohibits 8.1...
		if ( ( dwExcludeVersion & DXVERSION_81 ) != 0 )
		{
			// ...and the test requests 8.0 or 8.1 (but not both)...
			if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81 ) != 0 )
			{
				// ...and the command line doesn't also prohibit 8.0...
				if ( ( dwExcludeVersion & DXVERSION_8 ) == 0 )
				{
					// ...then run 8.0...
					m_Options.D3DOptions.dwIncVersions |= DXVERSION_8;
				};

				// ...and note that we've removed the ambiguity.
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
			};
		};

		// If the command line prohibits 8.0...
		if ( ( dwExcludeVersion & DXVERSION_8 ) != 0 )
		{
			// ...and the test requests 8.0 or 8.1 (but not both)...
			if ( ( m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81 ) != 0 )
			{
				// ...and the command line doesn't also prohibit 8.1...
				if ( ( dwExcludeVersion & DXVERSION_81) == 0 )
				{
					// ...then run 8.1...
					m_Options.D3DOptions.dwIncVersions |= DXVERSION_81;
				};

				// ...and note that we've removed the ambiguity.
				m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
			};
		};

		// Rule out versions prohibited on the command line from the test's requested versions.
		m_Options.D3DOptions.dwIncVersions &= ~dwExcludeVersion;
	};


	// Decode potentially ambiguous test DX version inclusion flags.
	if(m_Options.D3DOptions.dwIncVersions & DXVERSION_9_OR_91)
	{
		m_Options.D3DOptions.dwIncVersions |=  (min(m_pD3D->GetMaxVersion(), m_Options.D3DOptions.fMaxDXVersion) > 9.09f ? DXVERSION_91 : DXVERSION_9);
		m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_9_OR_91;
	};

	if(m_Options.D3DOptions.dwIncVersions & DXVERSION_8_OR_81)
	{
		m_Options.D3DOptions.dwIncVersions |=  (min(m_pD3D->GetMaxVersion(), m_Options.D3DOptions.fMaxDXVersion) > 8.0f ? DXVERSION_81 : DXVERSION_8);
		m_Options.D3DOptions.dwIncVersions &= ~DXVERSION_8_OR_81;
	};


	// Check for format overrides from the command line.
	if(ReadString(_T("TEX"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		StringToFmt(tcsBuffer, &m_Options.ModeOptions.TextureFmt);
	}

	if(ReadString(_T("ZBUF"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		StringToFmt(tcsBuffer, &m_Options.ModeOptions.ZBufferFmt);
	}

	if(ReadString(_T("CUBE"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		StringToFmt(tcsBuffer, &m_Options.ModeOptions.CubeFmt);
	}

	if(ReadString(_T("VOL"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		StringToFmt(tcsBuffer, &m_Options.ModeOptions.VolumeFmt);
	}

	if(ReadString(_T("MS"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		StringToFmt(tcsBuffer, &m_Options.ModeOptions.MultisampType);
	}


	// Check for fixed-function emulation overrides from the command line.
	if ( KeySet("FFEMU") )
	{
		m_Options.D3DOptions.dwVPEmuRegChange = VPEMUREGCHANGE_ENABLE;
		m_Options.D3DOptions.dwRefVPEmuRegChange = VPEMUREGCHANGE_DISABLE;
		m_Options.D3DOptions.dwPPEmuRegChange = PPEMUREGCHANGE_ENABLE;
		m_Options.D3DOptions.dwRefPPEmuRegChange = PPEMUREGCHANGE_DISABLE;
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		m_Options.TestOptions.bCompareSrcRef = true;
	}

	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DTest::ParseResolution(LPCTSTR pcsRes, __out UINT *pWidth, __out UINT *pHeight, __out FMT *pFmt)
{
	if(NULL == pcsRes)
	{
		DPF(1, _T("CD3DTest::ParseResolution:  pcsRes is NULL.\n"));
		return false;
	}

	TCHAR *sRes = NULL;
	TCHAR *sFmt = NULL;
	TCHAR *sWidth = NULL;
	TCHAR *sHeight = NULL;
	FMT fmt = (FMT)0;
	bool bRes = false;

	COPYSTRING(sRes, pcsRes);
	if(sRes == NULL)
		return false;

	// Get the BPP
	sFmt = _tcsrchr(sRes, _T('x'));
	if(sFmt == NULL)
	{
		goto Cleanup;
	}
	*sFmt = _T('\0'); sFmt++;

	// Get the Height
	sHeight = _tcsrchr(sRes, _T('x'));
	if(sHeight == NULL)
	{
		goto Cleanup;
	}
	*sHeight = _T('\0'); sHeight++;

	// Get the Width
	sWidth = sRes;

	// Covert the values
	if(pWidth && atoi(sWidth) != 0)
	{
		*pWidth = atoi(sWidth);
		bRes = true;
	}
	if(pHeight && atoi(sHeight) != 0)
	{
		*pHeight = atoi(sHeight);
		bRes = true;
	}
	StringToFmt(sFmt, &fmt);
	// If FMT == 0, then it's an unknown string
	if(pFmt && fmt != 0)
	{
		*pFmt = fmt;
		bRes = true;
	}


Cleanup:
	SAFEDELETEA(sRes);

	return bRes;
}


void CD3DTest::SetTestType(TESTTYPE Type)
{
	// Set the test options
	m_Options.TestOptions.Type              = Type;
	m_Options.TestOptions.dwCaps			= TESTCAPS_RESIZEWINDOW | TESTCAPS_RESIZEMODE;

	m_Options.D3DOptions.bD3DDebugHelper = false;

	// Set the default windowed mode properties
	m_Options.D3DOptions.dwWinMode          = WINMODE_WINDOWED;
	m_Options.D3DOptions.DisplayMode.dwWidth = MODEOPT_CURRENT;
	m_Options.D3DOptions.DisplayMode.dwHeight = MODEOPT_CURRENT;
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat =(D3DFORMAT)FMT_ANY;
	m_Options.D3DOptions.nBackBufferWidth  = MODEOPT_BACKBUFFERWIDTH;
	m_Options.D3DOptions.nBackBufferHeight  = MODEOPT_BACKBUFFERHEIGHT;

	// Set the default modes limits
	m_Options.ModeOptions.TextureFmt 		= FMT_ANY;
	m_Options.ModeOptions.ZBufferFmt 		= FMT_ANY;
	m_Options.ModeOptions.CubeFmt 			= FMT_ANY;
	m_Options.ModeOptions.VolumeFmt 		= FMT_ANY;
	m_Options.ModeOptions.BackBufferFmt		= FMT_ANY;
	m_Options.ModeOptions.MultisampType 	= MULTISAMPLE_ANY;
	m_Options.ModeOptions.uMaxDisplayModes 	= 1;

	// Set default texture filter
	m_Options.ModeOptions.dwTexExclude = PF_UNKNOWNFOURCC;
	m_Options.ModeOptions.dwCubeExclude = PF_UNKNOWNFOURCC;
	m_Options.ModeOptions.dwVolExclude = PF_UNKNOWNFOURCC;

	// Set the default back buffer cached mode
	m_Options.D3DOptions.bCachedBackBuffer	= true;
	m_Options.D3DOptions.bHintAntialias		= false;
	m_Options.D3DOptions.dwPresentFlags		= FMWK_DEFAULT;
	m_Options.D3DOptions.dwSwapEffect		= FMWK_DEFAULT;
	m_Options.D3DOptions.dwPresentInterval	= FMWK_DEFAULT;
	m_Options.D3DOptions.dwIncVersions		= KeySet(_T("WHQL")) ? DXVERSION_LATEST : 
		DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;

	// Set the default DXVersion ranges
	m_Options.D3DOptions.fMinDXVersion		= 6.f;
	m_Options.D3DOptions.fMaxDXVersion		= 9.1f;

	// Set the default PSGP Reg change
	m_Options.D3DOptions.dwPSGPRegChange    = PSGPREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_NONE;

	// Set the default fixed function emulation Reg changes
	m_Options.D3DOptions.dwVPEmuRegChange    = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefVPEmuRegChange = VPEMUREGCHANGE_DISABLE;
	m_Options.D3DOptions.dwPPEmuRegChange    = PPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefPPEmuRegChange = PPEMUREGCHANGE_DISABLE;

	// By default the SRC device can be different than the REF device
	m_Options.D3DOptions.bRefEqualsSrc		= false;
	m_Options.D3DOptions.bGoldReference		= false;

	// Set the test type specific variables
	switch(m_Options.TestOptions.Type)
	{
	case TESTTYPE_API:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.bReference = false;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP;
		m_Options.TestOptions.bCompareSrcRef = false;
		break;
	case TESTTYPE_CONF:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP;
		m_Options.TestOptions.bCompareSrcRef = true;
		break;
	case TESTTYPE_PSGP:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_ENABLE;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_DISABLE;
		m_Options.TestOptions.bCompareSrcRef = true;
		break;
	case TESTTYPE_FUNC:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.bEnumReference = false;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP;
		m_Options.TestOptions.bCompareSrcRef = true;
		break;
	case TESTTYPE_STRESS:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;
		m_Options.D3DOptions.bReference = false;
		m_Options.D3DOptions.bEnumReference = false;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED | WINMODE_FULLSCREEN;
		m_Options.ModeOptions.TextureFmt 		= FMT_ALL;
		m_Options.ModeOptions.ZBufferFmt 		= FMT_ALL;
		m_Options.ModeOptions.CubeFmt 			= FMT_ALL;
		m_Options.ModeOptions.VolumeFmt 		= FMT_ALL;
		m_Options.ModeOptions.MultisampType 	= MULTISAMPLE_ALL;
		m_Options.ModeOptions.uMaxDisplayModes 	= MODEOPT_MAXMODES;
		m_Options.TestOptions.bCompareSrcRef = false;
		break;
	default:
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.bReference = false;
		m_Options.D3DOptions.bEnumReference = false;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.TestOptions.bCompareSrcRef = false;
		DPF(1, _T("CD3DTest::SetTestType:  Unknown test type.  Using default configuration.\n"));
		break;
	};
}


void CD3DTest::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
	DWORD dwScenarios = 0;
	TCHAR tcsBuffer[MAX_PATH] = _T( "" );
	TCHAR tcsEXE[MAX_PATH] = _T( "" );

	if (dwTestScenarios & TESTSCENARIO_TNL)
	{
		if (KeySet("PSGP") && (dwTestScenarios & (TESTSCENARIO_PSGP|TESTSCENARIO_MSGP)))
		{
			dwScenarios = dwTestScenarios & TESTSCENARIO_PSGP;
			StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s-psgp"), GetExeName(true, tcsEXE));
			COPYSTRING(m_szNewLogName, tcsBuffer);
			m_szLogName = m_szNewLogName;
		}
		else if (KeySet("MSGP") && (dwTestScenarios & (TESTSCENARIO_PSGP|TESTSCENARIO_MSGP)))
		{
			dwScenarios = TESTSCENARIO_MSGP;
			StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s-msgp"), GetExeName(true, tcsEXE));
			COPYSTRING(m_szNewLogName, tcsBuffer);
			m_szLogName = m_szNewLogName;
		}
		else if (KeySet("RAST"))
			dwScenarios = TESTSCENARIO_RAST;
		else
			dwScenarios = (bPure) ? TESTSCENARIO_TNL|TESTSCENARIO_PURE : TESTSCENARIO_TNL;

	}
	else if (dwTestScenarios & (TESTSCENARIO_PSGP|TESTSCENARIO_MSGP))
	{
		if (KeySet("MSGP"))
			dwScenarios = dwTestScenarios & TESTSCENARIO_MSGP;
		else
			dwScenarios = dwTestScenarios & TESTSCENARIO_PSGP;
	}
	else
	{
		dwScenarios = dwTestScenarios;
	}	

	// Set scenario options

	// Window mode
	m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
	m_Options.D3DOptions.DisplayMode.dwWidth = MODEOPT_CURRENT;
	m_Options.D3DOptions.DisplayMode.dwHeight = MODEOPT_CURRENT;
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat =(D3DFORMAT)FMT_ANY;
	m_Options.D3DOptions.nBackBufferWidth  = MODEOPT_BACKBUFFERWIDTH;
	m_Options.D3DOptions.nBackBufferHeight  = MODEOPT_BACKBUFFERHEIGHT;

	// Default D3DOptions
	m_Options.D3DOptions.bRefEqualsSrc = false;
	m_Options.D3DOptions.bZBuffer = true;
	m_Options.D3DOptions.bCachedBackBuffer = true;
	m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
	m_Options.D3DOptions.fMinDXVersion = fMinVersion;
	m_Options.D3DOptions.fMaxDXVersion = 9.1f;
	m_Options.D3DOptions.dwPresentFlags		= FMWK_DEFAULT;
	m_Options.D3DOptions.dwSwapEffect		= FMWK_DEFAULT;
	m_Options.D3DOptions.dwPresentInterval	= FMWK_DEFAULT;
	m_Options.D3DOptions.bHintAntialias		= false;
	m_Options.D3DOptions.dwIncVersions		= KeySet(_T("WHQL")) ? DXVERSION_LATEST : 
		DXVERSION_9_OR_91 | DXVERSION_8_OR_81 | DXVERSION_7;

	// Default TestOptions
	m_Options.TestOptions.bCompareSrcRef = true;
	m_Options.TestOptions.dwCaps = TESTCAPS_RESIZEWINDOW | TESTCAPS_RESIZEMODE;;

	// Default mode options
	m_Options.ModeOptions.TextureFmt 		= FMT_ANY;
	m_Options.ModeOptions.ZBufferFmt 		= FMT_ANY;
	m_Options.ModeOptions.CubeFmt 			= FMT_ANY;
	m_Options.ModeOptions.VolumeFmt 		= FMT_ANY;
	m_Options.ModeOptions.BackBufferFmt		= FMT_ANY;
	m_Options.ModeOptions.MultisampType 	= MULTISAMPLE_ANY;
	m_Options.ModeOptions.uMaxDisplayModes 	= 1;

	// Set default texture filter
	m_Options.ModeOptions.dwTexExclude = PF_UNKNOWNFOURCC;
	m_Options.ModeOptions.dwCubeExclude = PF_UNKNOWNFOURCC;
	m_Options.ModeOptions.dwVolExclude = PF_UNKNOWNFOURCC;

	// Set the default back buffer cached mode
	m_Options.D3DOptions.bCachedBackBuffer	= true;
	m_Options.D3DOptions.bHintAntialias		= false;
	m_Options.D3DOptions.dwPresentFlags		= FMWK_DEFAULT;
	m_Options.D3DOptions.bGoldReference		= false;

	// Set the default fixed function emulation Reg changes
	m_Options.D3DOptions.dwVPEmuRegChange    = VPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefVPEmuRegChange = VPEMUREGCHANGE_DISABLE;
	m_Options.D3DOptions.dwPPEmuRegChange    = PPEMUREGCHANGE_NONE;
	m_Options.D3DOptions.dwRefPPEmuRegChange = PPEMUREGCHANGE_DISABLE;

	if (dwScenarios & TESTSCENARIO_RAST)
	{
		m_Options.TestOptions.Type = TESTTYPE_CONF;
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_NONE;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_NONE;
	}
	else if (dwScenarios & (TESTSCENARIO_TNL|TESTSCENARIO_PURE))
	{
		m_Options.TestOptions.Type = TESTTYPE_CONF;
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD;
		if (dwScenarios & TESTSCENARIO_PURE)			m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_HWVP_OR_PURE;
		else											m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_HWVP;
		m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_NONE;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_NONE;
	}
	else if (dwScenarios & (TESTSCENARIO_PSGP | TESTSCENARIO_MSGP))
	{
		m_Options.TestOptions.Type = TESTTYPE_PSGP;
		// ****** 7/26/2011:changing the dev device to REF + SWVP for PSGP|MSGP jobs
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwPSGPRegChange = 0;
		if (dwScenarios & TESTSCENARIO_PSGP)			m_Options.D3DOptions.dwPSGPRegChange |= PSGPREGCHANGE_ENABLE;
		if (dwScenarios & TESTSCENARIO_MSGP)			m_Options.D3DOptions.dwPSGPRegChange |= PSGPREGCHANGE_DISABLE;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bReference = true;
		// ****** 7/26/2011:changing the Ref device to REF + HWVP for PSGP|MSGP jobs
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		m_Options.D3DOptions.dwRefPSGPRegChange = (dwScenarios & TESTSCENARIO_PSGP) ? PSGPREGCHANGE_DISABLE : PSGPREGCHANGE_ENABLE;
	}
	else if (dwScenarios & (TESTSCENARIO_TNLREF|TESTSCENARIO_PUREREF))
	{
		m_Options.TestOptions.Type = TESTTYPE_CONF;
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD;
		if (dwScenarios & TESTSCENARIO_TNLREF)				m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_HWVP;
		if (dwScenarios & TESTSCENARIO_PUREREF)			m_Options.D3DOptions.dwBehaviors |= DEVICECREATE_PURE;
		m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_NONE;
		m_Options.D3DOptions.bEnumReference = true;
		m_Options.D3DOptions.bReference = true;
		m_Options.D3DOptions.dwRefDevType = DEVICETYPE_REF;
		m_Options.D3DOptions.dwRefBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_DISABLE;
	}
	else
	{
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
		m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
		m_Options.D3DOptions.bReference = false;
		m_Options.D3DOptions.bEnumReference = false;
		m_Options.D3DOptions.bZBuffer = true;
		m_Options.TestOptions.bCompareSrcRef = false;
		DPF(1, _T("CD3DTest::SetTestType:  Unknown test type.  Using default configuration.\n"));
	}
}


bool CD3DTest::Initialize(CD3D* pD3D, bool bExceptionHandling)
{
	if(pD3D == NULL)
	{
		DPF(1, _T("CD3DTest::Initialize:  pD3D is NULL.\n"));
		return false;
	}

	// Set the general test data
	m_pD3D = pD3D;

	pD3D->m_bD3DDebugHelper = m_Options.D3DOptions.bD3DDebugHelper;

	// Add Ref the D3D object and pass it to the test
	m_pD3D->AddRef();
	m_bException = bExceptionHandling;

	if(!ParseCmdLine())
	{
		DPF(1, _T("CD3DTest::Initialize:  ParseCmdLine failed.\n"));
		return false;
	}

	// Enum the reference if requested by the test
	if(m_Options.D3DOptions.bEnumReference)
		EnumReference();

	// Enforce golden image creation against PureREF
	if(m_Options.D3DOptions.bGoldReference)
	{
		if(m_Options.D3DOptions.dwDevTypes != DEVICETYPE_REF || 
			!((m_Options.D3DOptions.dwBehaviors & 0xFFFF) & (DEVICECREATE_PURE | DEVICECREATE_BESTAVAILABLE)))
		{		
			m_Options.D3DOptions.dwDevTypes = DEVICETYPE_REF;
			m_Options.D3DOptions.dwBehaviors = (m_Options.D3DOptions.dwBehaviors & 0xFFFF0000) | DEVICECREATE_PURE;
			DPF(1, _T("Golden image tests require PUREREF device.  Forcing src device to PUREREF.\n"));
		}
	}

	// Initialize API cycling
	InitializeAPICycling();

	return CTest::Initialize();
}


void CD3DTest::ReleaseDevMode()
{
	// Delete the mode list
	ReleaseModeList();

	// Delete the common format lists
	ReleaseCommonFormats();

	// Delete the common display modes
	ReleaseCommonDisplayModes();

	// Release all D3D objects(non-device)
	ReleaseObjects();

	// Release all objects
	ReleaseDevices();
}


void CD3DTest::ReleaseAll()
{
	ReleaseDevMode();

	RELEASE(m_pD3D);

	// Reset the EnumReference registry value
	if(m_Options.D3DOptions.bEnumReference)
		RemoveReference();

	if (g_hDCTDLL)
	{
		FreeLibrary(g_hDCTDLL);
		g_hDCTDLL = NULL;
	}
}


bool CD3DTest::RemoveReference()
{
	LONG lResult = 0;
	HKEY hKey = NULL;

	// Open the key

	lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Direct3D\\Drivers",
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if(lResult != ERROR_SUCCESS)
	{
		DPF(1, _T("CD3DTest::RemoveReference:  Unable to open Direct3D\\Drivers registry key.\n"));
		return false;
	}


	// Set the reg key to zero
	DWORD dwSize = 4;
	DWORD dwValue = 0;

	lResult = RegSetValueEx(hKey, "EnumReference", 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
	if(ERROR_SUCCESS != lResult)
	{
		DPF(1, _T("CD3DTest::RemoveReference:  Unable to restore EnumReference registry value.")); 
	}

	RegCloseKey(hKey);

	return true;
}


bool CD3DTest::EnumReference()
{
	LONG lResult = 0;
	HKEY hKey = NULL;

	// Open the key

	lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Direct3D\\Drivers",
		0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 
		NULL, &hKey, NULL);
	if(lResult != ERROR_SUCCESS)
	{
		DPF(1, _T("CD3DTest::EnumReference:  Unable to create Direct3D\\Drivers registry key.\n"));
		return false;
	}

	// Query for the value
	DWORD dwType, dwValue=1, dwSize = 4;

	lResult = RegQueryValueEx(hKey, "EnumReference", 0, &dwType, (BYTE *)&dwValue, &dwSize);
	if(ERROR_SUCCESS != lResult || dwValue == 0)
	{
		dwValue = 1;

		// Since this machine didn't have the value to begin with, we
		// must create one.

		lResult = RegSetValueEx(hKey, "EnumReference", 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult) 
		{
			DPF(1, _T("CD3DTest::EnumReference:  Unable to create EnumReference registry value."));
			RegCloseKey(hKey);
			return false;
		}
	} 
	else 
	{
		// We already had a key and it was set to 1, so we don't want to remove it at cleanup time
		m_Options.D3DOptions.bEnumReference = false;
	}

	RegCloseKey(hKey);

	return true;
}


bool CD3DTest::SetupTest(void)
{
	//
	// Initialize locals.
	//

	UINT uResult = 0;
	UINT i = 0;
	HRESULT hr = S_OK;
	bool bRes = true;
	LRESULT lrState = 0;
	static TCHAR sCaption[256] = {0};
	D3DTESTFINISH TestAbortCode = D3DTESTFINISH_SKIPREMAINDER;


	//
	// Initialize members.
	//

	m_bHandlePaintMsg = false;
	m_bTestTerminate = false;

	// Note test setup start time.
	GetSystemTime( &m_TestCaseStartTime );

	// Toggle the pass required based on compare type
	if(m_Options.D3DOptions.bGoldReference)
	{
		m_fPrevPassRequired = m_pFramework->GetImageCmp()->GetPassRequired();	// Store the existing pass percentage for restoring later
		m_pFramework->GetImageCmp()->SetPassRequired(.98f);	
	}

	// Verify that we have a valid mode list
	if(m_pModeList == NULL)
	{
		SetTestRange(1,1);

		DPF(1, _T("CD3DTest::SetupTest:  No test modes added.\n"));

		TestAbortCode = D3DTESTFINISH_SKIPREMAINDER;
		bRes = false;
		goto Exit;
	}

	// Set the local DX version for easy porting
	if(m_pD3D->GetVersion() >= 9.f && m_pD3D->GetVersion() < 10.f)
		m_dwVersion = 0x900;
	else if(m_pD3D->GetVersion() >= 8.f && m_pD3D->GetVersion() < 9.f)
		m_dwVersion = 0x800;
	else if(m_pD3D->GetVersion() == 7.f)
		m_dwVersion = 0x700;
	else if(m_pD3D->GetVersion() == 6.f)
		m_dwVersion = 0x600;

	// Initialize windows and controls
	HWND insertAfter = HWND_NOTOPMOST;

	if( m_Options.TestOptions.bCompareFB || (m_pFramework->m_dwStyleEx & WS_EX_TOPMOST))
		insertAfter = HWND_TOPMOST;

	SetWindowPos( m_pFramework->m_hWindow, insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	m_pFramework->GetWindow(WND_TESTNUM)->SetEnable((m_Options.TestOptions.dwCaps & TESTCAPS_SERIALEXECUTION) == 0);
	lrState = SendMessage(m_pFramework->GetWindow(WND_TOOLBAR)->m_hWindow, TB_GETSTATE, (WPARAM)IDM_EXECUTION_REPLAY, 0);
	lrState = (m_Options.TestOptions.dwCaps & TESTCAPS_SERIALEXECUTION) ? lrState & ~TBSTATE_ENABLED : lrState | TBSTATE_ENABLED;

	SendMessage(m_pFramework->GetWindow(WND_TOOLBAR)->m_hWindow, TB_SETSTATE, IDM_EXECUTION_REPLAY, lrState);

	// Construct the test caption and begin the group
	ConstructTestCaption(sCaption, ARRAYSIZE(sCaption));
	m_pFramework->m_DXGLog.BeginTestGroup((const PTSTR) sCaption);

	// Pre-setup the mode data. (In case the user wants to change the Z buffer
	// format before it's created, as an example).
	if (!PreModeSetup())
	{
		DPF(1, _T("CD3DTest::SetupTest:  PreModeSetup failed.\n"));

		TestAbortCode = D3DTESTFINISH_ABORT;
		bRes= false;
		goto Exit;
	}

	hr = SetupTestMode(m_pCurrentMode);

	if(hr != S_OK)
	{
		DPF(2, _T("CD3DTest::SetupTest:  Device intialization failed. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);

		if(hr == E_UNSUPPORTED)
		{
			SetTestRange(1,1);
			TestAbortCode = D3DTESTFINISH_SKIPREMAINDER;
		}
		else if(hr == S_FALSE)
		{
			TestAbortCode = D3DTESTFINISH_FAIL;
		}
		else
		{
			TestAbortCode = D3DTESTFINISH_ABORT;
		};

		bRes = false;
		goto Exit;
	}

	// Post mode setup, devices have been created
	if(!PostModeSetup())
	{
		DPF(1, _T("CD3DTest::SetupTest:  PostModeSetup failed.\n"));

		TestAbortCode = D3DTESTFINISH_ABORT;
		bRes = false;
		goto Exit;
	};

	// Set the window size appropriately
	SetupDisplay(m_pCurrentMode, m_pCurrentMode->bWindowed);

	// Disable system idle detection if needed.
	if(m_Options.TestOptions.bDisableIdleDetection == true)
	{
		if( NULL == SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED /*| ES_USER_PRESENT*/) )
		{
			LPVOID lpMsgBuf;
			DWORD dw = ::GetLastError(); 
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
			WriteToLog
				( 
				_T( "CD3DTest::SetupTest:  SetThreadExecutionState failed with error: %s\n" ),
				(LPCTSTR)lpMsgBuf
				);
			LocalFree(lpMsgBuf);
		}
	}


	// Initialize device API cycling
	for (i = 0; i < m_uDeviceList; i++)
		m_DeviceList[i]->SetAPI(m_dwRPCurrentAPI);

	// We're getting into user code.  Need to clean them up if initialization fails from here out.
	m_bTestTerminate = true;

	// Check the caps
	if(!CapsCheck())
	{
		WriteToLog
			(
			_T( "CD3DTest::SetupTest:  CapsCheck reports that device does not support requested capability.  Skipping this test group." )
			);

		SetTestRange(1,1);

		TestAbortCode = D3DTESTFINISH_SKIPREMAINDER;
		bRes = false;
		goto Exit;
	}

	// Test will setup D3D objects(Textures, stateblocks, etc.)
	if(!Setup())
	{
		DPF(1, _T("CD3DTest::SetupTest:  Setup failed.\n"));

		TestAbortCode = D3DTESTFINISH_ABORT;
		bRes = false;
		goto Exit;
	};

	// Initialize the test
	uResult = TestInitialize();

	// Check for intialization failure
	if (D3DTESTINIT_ABORT == uResult)
	{
		DPF(1, _T("CD3DTest::SetupTest:  TestInitialize returned D3DTESTINIT_ABORT.\n"));

		TestAbortCode = D3DTESTFINISH_ABORT;
		bRes = false;
		goto Exit;
	}
	else if (D3DTESTINIT_SKIPALL == uResult)
	{
		//DPF(1, _T("CD3DTest::SetupTest:  TestInitialize returned D3DTESTINIT_SKIPREMAINDER.\n"));
		TestAbortCode = D3DTESTFINISH_SKIPREMAINDER;
		bRes = false;
		goto Exit;
	}

	// Enable Paint msg handling 
	if(m_pCurrentMode->bWindowed)
		m_bHandlePaintMsg = true;

	if ( 0.0f != m_pFramework->m_fRunPerc )
	{
		DWORD dwTestsRun = (DWORD)(m_uTotalTests * m_pFramework->m_fRunPerc);

		if ( !dwTestsRun ) // Always run at least 1
			++dwTestsRun;

		m_fSkipsPerTest = (float)(( m_uTotalTests - dwTestsRun ) / dwTestsRun);
		m_fSkipCount = 0;
	}
	else
	{
		m_fSkipsPerTest = 0.0f;
	};

Exit:
	//
	// Write the result of SetupTest as a separate test case in the log.
	//

	if (m_pFramework->m_DXGLog.InTestCase())
	{
		m_pFramework->m_DXGLog.EndTestCase();
	};

	// Begin a test case.
	BeginTestCase
		(
		_T("SetupTest"),
		m_uTestNumber
		);


	ASSERT( m_pFramework->m_DXGLog.InTestCase() );
	if ( bRes == true )
	{
		//
		// Log a pass for the SetupTest test case.
		//

		m_pFramework->m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_PASS );
		m_fPassPercentage = 1.f;
	}
	else
	{
		switch ( TestAbortCode )
		{
		case D3DTESTFINISH_SKIPREMAINDER:
			{
				m_pFramework->m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_SKIPPED );
			} break;

		case D3DTESTFINISH_ABORT:
			{
				m_pFramework->m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_BLOCKED );
			} break;

		case D3DTESTFINISH_FAIL:
			{
				m_pFramework->m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_FAIL );
			} break;
		};
	};


	// End the test case.
	if (m_pFramework->m_DXGLog.InTestCase())
	{
		EndTestCase();
	};


	if ( bRes == false )
	{
		TestFinish( TestAbortCode );
	};


	return bRes;
}


bool CD3DTest::CapsCheck()
{
	// App hook to do caps/ddi checking.  Return true if the check succeeds(test will execute).
	return true;
}


bool CD3DTest::Setup()
{
	// App hook to create any D3D objects needed for the run
	// For backward compatibility, we're going to call the legacy hooks

	if (!SetDefaultViewport())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default viewport.\n"));
		return false;
	}

	if (!SetDefaultMaterials())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default materials.\n"));
		return false;
	}

	if (!SetDefaultLights())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default lights.\n"));
		return false;
	}

	if (!SetDefaultMatrices())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default matrices.\n"));
		return false;
	}

	if (!SetDefaultTextures())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default textures.\n"));
		return false;
	}

	if (!SetDefaultRenderStates())
	{
		DPF(1, _T("CD3DTest::Setup:  Failed to set default render states.\n"));
		return false;
	}

	return true;
}


void CD3DTest::Cleanup()
{
	// App hook to cleanup any D3D object currently allocated
}


bool CD3DTest::SetDefaultViewport()
{
	VIEWPORT Viewport;

	Viewport.X = 0;    
	Viewport.Y = 0;    
	Viewport.Width = m_DeviceList[0]->GetPresParams()->uWidth;
	Viewport.Height = m_DeviceList[0]->GetPresParams()->uHeight;
	Viewport.MinZ = 0.f; 
	Viewport.MaxZ = 1.f; 

	return SetViewport(&Viewport);
}


bool CD3DTest::SetDefaultMaterials()
{
	return true;
}


bool CD3DTest::SetDefaultTextures()
{
	return true;
}


bool CD3DTest::SetDefaultLights()
{
	return true;
}


bool CD3DTest::SetDefaultRenderStates()
{
	return true;
}


bool CD3DTest::SetDefaultMatrices()
{
	return true;
}


HRESULT CD3DTest::CreatePresentParams(TESTMODE *pMode, PRESENT_PARAMETERS **ppParams)
{
	HRESULT hr = S_OK;;
	PRESENT_PARAMETERS *pParams = NULL;
	DWORD dwParams = 1;
	UINT i;

	// Determine how many params we need(MultiHead requires one for each head, we'll create MAX)
	for(i=0; i<m_uDeviceList; i++)
	{
		if(m_DeviceList[i] == NULL)
			continue;
		dwParams = max(dwParams, m_DeviceList[i]->GetCaps()->NumberOfAdaptersInGroup);
	}

	for( UINT uWindowMultiHead = 1; uWindowMultiHead < MAX_DEVICES; uWindowMultiHead++ )
		if( NULL != m_pWindowMultiHead[uWindowMultiHead] )
		{
			m_pWindowMultiHead[uWindowMultiHead]->Close();
			delete m_pWindowMultiHead[uWindowMultiHead];
			m_pWindowMultiHead[uWindowMultiHead] = NULL;
		}
		m_pWindowMultiHead[0] = (CWindow*)m_pFramework;

		// Allocate the new params
		pParams = new PRESENT_PARAMETERS[dwParams];
		if(NULL == pParams)
		{
			DPF(1, _T("CD3DTest::CreatePresentParams:  Failed to allocate presentation parameters.\n"));
			hr = E_FAIL;
			goto Exit;
		}

		memset(pParams, 0, sizeof(PRESENT_PARAMETERS) * dwParams);

		// Update the PRESENT_PARAMETERS struct
		pParams[0].hDeviceWindow = pMode->bWindowed ? m_pFramework->m_pDisplayWindow->m_hWindow : m_pFramework->m_hWindow;
		pParams[0].uWidth = pMode->nBackBufferWidth;
		pParams[0].uHeight = pMode->nBackBufferHeight;
		pParams[0].uBackBufferCount = 1;
		pParams[0].bWindowed = pMode->bWindowed;
		pParams[0].MultiSample = (m_pCommonMultiSampleFormats) ? m_pCommonMultiSampleFormats[pMode->nMultisample] : MULTISAMPLE_NONE;
		pParams[0].MultiSampleQuality = pMode->nMltsmpQuality;
		pParams[0].dwFlags = m_Options.D3DOptions.dwPresentFlags;
		pParams[0].SwapEffect = m_Options.D3DOptions.dwSwapEffect;
		pParams[0].uPresentInterval = m_Options.D3DOptions.dwPresentInterval;
		pParams[0].bCachedBackBuffers = m_Options.D3DOptions.bCachedBackBuffer;
		if ( !pMode->bWindowed )
		{
			pParams[0].uFullscreenRefresh = pMode->pDisplay->dwRefreshRate;
		}

		if(m_pCommonBackBufferFormats)
			memcpy(&(pParams[0].BackBufferFormat), m_pCommonBackBufferFormats + pMode->nBackBufferFormat, sizeof(FORMAT));
		else
			memcpy(&(pParams[0].BackBufferFormat), &(pMode->pDisplay->Format), sizeof(FORMAT));

		memcpy(&(pParams[0].DesktopFormat), &(pMode->pDisplay->Format), sizeof(FORMAT));

		// Set the internal flags
		if(m_Options.D3DOptions.bCachedBackBuffer)
			pParams[0].dwInternalFlags |= PPINTERNAL_CACHEBACKBUFFER;

		if(m_Options.D3DOptions.bHintAntialias)
			pParams[0].dwInternalFlags |= PPINTERNAL_HINTANTIALIAS;

		if(m_Options.D3DOptions.bZBuffer)
		{
			if(m_uCommonZBufferFormats > 0)
			{
				pParams[0].bAutoDepthStencil = true;
				memcpy(&(pParams[0].AutoDepthStencilFormat), &(m_pCommonZBufferFormats[pMode->nZBufferFormat]), sizeof(FORMAT));
			}
			else
			{
				DPF(2, _T("CD3DTest::CreatePresentParams:  ZBuffer requested but there are no common formats available.  Disabling Z buffers.\n"));
				pParams[0].bAutoDepthStencil = false;
			}
		}

		// Make all the PPs the same
		for( i = 1; i < dwParams; i++ )
		{
			memcpy(&pParams[i], &pParams[0], sizeof(PRESENT_PARAMETERS));

			m_pWindowMultiHead[i] = new CWindow();
			if( NULL == m_pWindowMultiHead[i] ||
				!m_pWindowMultiHead[i]->InitializeWindow(
				0, 
				_T("FMWK-MAIN"), 
				_T("MultiHead"), 
				NULL, 
				LoadIcon( m_pFramework->m_hInstance,MAKEINTRESOURCE( IDI_D3DICON ) ),
				m_pFramework->m_dwWindowedStyle, 
				0, 
				0, 
				0, 
				0, 
				NULL 
				)
				)
			{
				hr = E_FAIL;
				break;
			}

			pParams[i].hDeviceWindow = m_pWindowMultiHead[i]->m_hWindow;
		}

Exit:

		if(FAILED(hr))
		{
			SAFEDELETEA(pParams);
		}
		else
			*ppParams = pParams;

		return hr;
}


HRESULT CD3DTest::SetupTestMode(TESTMODE *pMode)
{
	HRESULT hr = S_OK;
	PRESENT_PARAMETERS *pParams = NULL;

	if(NULL == pMode)
	{
		DPF(1, _T("CD3DTest::SetupTestMode:  pMode is NULL.\n"));
		return E_FAIL;
	}

	// Update local params
	m_bShowCompareWindow = m_pFramework->ShowCompareWnd();
	m_bCompareEnabled = m_Options.TestOptions.bCompareSrcRef;

	// Flush any lingering errors
	GetLastError();
	m_pD3D->SetDriverInternalError(false);
	m_pD3D->SetDeviceLost(false);

	// Create the present parameters
	hr = CreatePresentParams(pMode, &pParams);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3DTest::SetupTestMode:  CreatePresentParams failed.\n"));
		goto Exit;
	}

	// Create the actual D3D devices
	hr = UpdateDevices(pParams); 
	if(hr != S_OK)
	{
		DPF(1, _T("CD3DTest::SetupTestMode:  UpdateDevices failed.\n"));
		goto Exit;
	}

	// Create all D3D objects
	hr = CreateObjects();
	if(FAILED(hr))
	{
		DPF(1, _T("CD3DTest::SetupTestMode:  CreateObjects failed. hr =%s(%x)\n"), 
			m_pD3D->HResultToString(hr), hr);
		goto Exit;	
	}

	// Update the window based on new settings
	if(!m_pFramework->ToggleFullscreen(pMode->bWindowed) ||
		!m_pFramework->ToggleSizeable((m_Options.TestOptions.dwCaps & TESTCAPS_RESIZEWINDOW) && m_bStretchPresent))
	{
		DPF(1, _T("CD3DTest::SetupTestMode:  Failed to update framework settings.\n"));
		return E_FAIL;
	}

	// Set the LOGFONT
	m_LogFont.lfHeight = 12;
	m_LogFont.lfWidth = 0;
	m_LogFont.lfEscapement = 0;
	m_LogFont.lfOrientation = 0;
	m_LogFont.lfWeight = FW_NORMAL;
	m_LogFont.lfItalic = FALSE;
	m_LogFont.lfUnderline = FALSE;
	m_LogFont.lfStrikeOut = FALSE;
	m_LogFont.lfCharSet = ANSI_CHARSET;
	m_LogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_LogFont.lfQuality = DEFAULT_QUALITY;
	m_LogFont.lfPitchAndFamily = VARIABLE_PITCH;
	StringCchCopy( m_LogFont.lfFaceName, min( 6, LF_FACESIZE ), _T("arial\0") );

	SetFont( m_LogFont );


Exit:
	if ( FAILED( hr ) )
	{
		//
		// Check for a TDR in the event log.
		//

		DWORD TDREventIDs[] =
		{
			4097,
			4099,
			4101
		};

		DWORD MatchingEventID = 0;

		HRESULT hScanForTDREventResult = ScanEventLogForEvents
			(
			_T( "System" ),
			&m_TestCaseStartTime,
			ARRAYSIZE( TDREventIDs ),
			TDREventIDs,
			&MatchingEventID
			);

		if ( hScanForTDREventResult == S_OK )
		{
			DPF
				(
				1,
				_T( "CD3DTest::SetupTestMode:  A timeout detection and recovery (TDR) event (%u) was detected in the System event log during or just prior to device initialization.\n" ),
				MatchingEventID
				);
		};
	};


	SAFEDELETEA(pParams);

	return hr;
}


HRESULT CD3DTest::CreateImageCompare(CSwapChain **ppSwapChain)
{
	HRESULT hr;
	PRESENT_PARAMETERS *pParams = NULL;

	if(m_pCurrentMode->bWindowed)
	{
		// Allocate the new params
		pParams = new PRESENT_PARAMETERS;
		if(!pParams)
		{
			DPF(1, _T("CD3DTest::CreateImageCompare:  Failed to allocate presentation parameters.\n"));
			hr = E_FAIL;
			goto Exit;
		}

		memset(pParams, 0, sizeof(PRESENT_PARAMETERS));

		// Update the PRESENT_PARAMETERS struct

		pParams->hDeviceWindow = m_pCurrentMode->bWindowed ? m_pFramework->m_pDisplayWindow->m_hWindow : m_pFramework->m_hWindow;
		pParams->uWidth = m_DeviceList[0]->GetPresParams()->uWidth;
		pParams->uHeight = m_DeviceList[0]->GetPresParams()->uHeight;
		pParams->uBackBufferCount	= 1;
		pParams->bWindowed = m_pCurrentMode->bWindowed;
		pParams->MultiSample = MULTISAMPLE_NONE;
		pParams->dwFlags = PRESENTFLAG_LOCKABLE_BACKBUFFER;
		pParams->SwapEffect = SWAPEFFECT_COPY;
		pParams->bCachedBackBuffers = true;
		pParams->bAutoDepthStencil = false;
		if ( m_Options.TestOptions.bCompareFB )
			pParams->BackBufferFormat = FMT_A8R8G8B8;
		else if(m_pCommonBackBufferFormats)
			memcpy(&(pParams->BackBufferFormat), m_pCommonBackBufferFormats + m_pCurrentMode->nBackBufferFormat, sizeof(FORMAT));
		else
			memcpy(&(pParams->BackBufferFormat), &(m_pCurrentMode->pDisplay->Format), sizeof(FORMAT));

		// Set the internal flags
		if(m_Options.D3DOptions.bCachedBackBuffer)
			pParams->dwInternalFlags |= PPINTERNAL_CACHEBACKBUFFER;

		// Create the SwapChain

		hr = m_pRefDevice->CreateAdditionalSwapChain(pParams, ppSwapChain);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreateImageCompare:  CreateAdditionalSwapChain failed. hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}
	else
	{
		hr = m_pRefDevice->CreateTexture(m_pCurrentMode->nBackBufferWidth,m_pCurrentMode->nBackBufferHeight, 1, 0, m_pCommonBackBufferFormats[m_pCurrentMode->nBackBufferFormat],POOL_SYSTEMMEM, &m_pCmpTexture);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreateImageCompare:  CreateTexture failed. hr=%s(%x).\n"), 
				m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	m_bShowCompareWindow = m_pFramework->ShowCompareWnd();


Exit:

	SAFEDELETE(pParams);
	return hr;
}


bool CD3DTest::NextTestMode(void)
{
	if (!m_pModeList || !m_pCurrentMode)
		return true;


	// Go to the next mode.
	m_pCurrentMode = m_bReverseMode ? m_pCurrentMode->pPrevious : m_pCurrentMode->pNext;
	m_uCurrentMode +=  m_bReverseMode ? -1 : 1;


	// Reset the reverse flag.
	m_bReverseMode = false;

	return m_pCurrentMode == NULL;
}


bool CD3DTest::ReleaseTestMode()
{
	// Release all objects
	ReleaseObjects();

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(m_DeviceList[i])
			m_DeviceList[i]->ReleaseD3D();			// Release all alloc'd D3D object
	}

	// Older D3D versions require closer D3D->Device coupling, so we must release here.

	if(m_pD3D && m_pD3D->GetVersion() < 8.f)
		m_pD3D->ReleaseD3D();

	if(m_Options.D3DOptions.bGoldReference)
	{
		m_pFramework->GetImageCmp()->SetPassRequired(m_fPrevPassRequired);		// Restore default pass percentage
	}

	return true;
}


bool CD3DTest::CleanupTest(void)
{
	// Stop handling the paint msg
	m_bHandlePaintMsg = false;

	// Test app must release all
	if(m_bTestTerminate)
	{
		TestTerminate();
		m_bTestTerminate = false;
	}

	// Release this TESTMODE
	return ReleaseTestMode();
}


void CD3DTest::ReadString(LPCTSTR pKey, LPCTSTR pDefault, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove)
{
	m_pFramework->m_CmdLine.ReadString(pKey, pDefault, pBuffer, nBuffer, bRemove);
}


bool CD3DTest::ReadString(LPCTSTR pKey, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove)
{
	return m_pFramework->m_CmdLine.ReadString(pKey, pBuffer, nBuffer, bRemove) ? true:false;
}


void CD3DTest::ReadInteger(LPCTSTR pKey, int iDefault, __out int * pInt, bool bRemove)
{
	m_pFramework->m_CmdLine.ReadInteger(pKey,iDefault,pInt,bRemove);
}


bool CD3DTest::ReadInteger(LPCTSTR pKey, __out int * pInt, bool bRemove)
{
	return m_pFramework->m_CmdLine.ReadInteger(pKey,pInt,bRemove) ? true:false;
}


void CD3DTest::ReadFloat(LPCTSTR pKey, float fDefault, __out float * pFloat, bool bRemove)
{
	m_pFramework->m_CmdLine.ReadFloat(pKey,fDefault,pFloat,bRemove);
}


bool CD3DTest::ReadFloat(LPCTSTR pKey, __out float * pFloat, bool bRemove)
{
	return m_pFramework->m_CmdLine.ReadFloat(pKey,pFloat,bRemove) ? true:false;
}


bool CD3DTest::KeySet(LPCTSTR pKey, bool bRemove)
{
	return m_pFramework->m_CmdLine.KeySet(pKey,bRemove) ? true:false;
}


void CD3DTest::AddKeys(LPCTSTR pKey)
{
	m_pFramework->m_CmdLine.AddKeys(pKey);	
}


bool CD3DTest::IsWHQL()
{
	return m_pFramework->m_bWHQL;
}


void CD3DTest::UpdateDetails()
{
	UINT n1, n2, n3;
	FLOAT f1, f2, f3,f4;
	TCHAR szFormat[32], szBuffer[64];

	m_pFramework->GetImageCmp()->GetResultDetails(n1, n2, n3,f1, f2, f3, f4);

	if(IsCompareEnabled())
		WriteDetails(1, _T("$r@UImage Compare@U0:\n"
		"$w\tTotal Pixels Considered: $y%d\n"
		"$w\tPixels failing per channel test: $y%d\n"
		"$w\tPixels failing per pixel test: $y%d\n"
		"$w\tTotal difference in red channel: $y%.3f\n"
		"$w\tTotal difference in green channel: $y%.3f\n"
		"$w\tTotal difference in blue channel: $y%.3f\n"
		"$w\tTotal difference in alpha channel: $y%.3f\n"),
		n1,n2,n3,f1,f2,f3,f4);

	if (m_DeviceList[0])
	{
		WriteDetails(1, _T("$r@UMode Info@U0:\n"
			"$w\tRenderPrimitive mode: $y%s\n"
			"$w\tDefault texture loading mode: $y%s\n"),
			RPModeToString(), TexLoadToString(m_DeviceList[0]->GetTexLoad()));

		if(m_pCommonBackBufferFormats)
			FmtToString(FORMAT_TEXTURE, &m_pCommonBackBufferFormats[m_pCurrentMode->nBackBufferFormat], szFormat);
		else
			FmtToString(FORMAT_TEXTURE, &m_DeviceList[0]->GetAdapter()->DesktopMode.Format, szFormat);

		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault BackBuf: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}

	if(m_pCommonTextureFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault Texture: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}

	if(m_pCommonZBufferFormats)
	{
		FmtToString(FORMAT_ZBUFFER, &m_pCommonZBufferFormats[m_pCurrentMode->nZBufferFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault Z: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}

	if(m_pCommonCubeFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault Cube: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}

	if(m_pCommonVolumeFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonVolumeFormats[m_pCurrentMode->nBackBufferFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault Volume: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}

	if(m_pCommonMultiSampleFormats)
	{
		FmtToString(FORMAT_MULTISAMPLE, (FORMAT*)(&m_pCommonMultiSampleFormats[m_pCurrentMode->nMultisample]), szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("$w\tDefault MultiSamp: $y%s\n"), szFormat);
		WriteDetails(1, szBuffer);
	}
}


void CD3DTest::UpdateStatus()
{
	DWORD Result = m_pFramework->m_DXGLog.GetCurrentResult();
	TCHAR *sResStr = NULL;

	WriteStatus
	(
		_T("$wTest\t"),
		_T("$a%s"),
		m_pFramework->m_DXGLog.GetCurrentTest()
	);

	switch(Result)
	{
		case WTT_TESTCASE_RESULT_PASS:
			sResStr = _T("$a%s");
		break;

		case WTT_TESTCASE_RESULT_SKIPPED:
			sResStr = _T("$y%s");
		break;

		case WTT_TESTCASE_RESULT_WARN:
			sResStr = _T("$o%s");
		break;

		default:
			sResStr = _T("$r@b@i%s@b0@i0");
		break;
	}

	WriteStatus(_T("$wResult\t"), sResStr, m_pFramework->m_DXGLog.GetCurrentResultString());
	WriteStatus(_T("$wOverall Results"),_T("$aPass: $w%d$a, Fail: $w%d$a, Abort: $w%d$a, Skip: $w%d$a, Warn: $w%d$a"), GetPass(), GetFail(), GetAbort(), GetSkip(), GetWarn());
	if(IsCompareEnabled() && m_fPassPercentage != -1)
	{
		WriteStatus
			(
			_T("$wImage Compare"),
			_T("$aConforms: $w%0.2f%% $aRequired: $w%0.2f%%"),
			m_fPassPercentage * 100.0f,
			m_pFramework->GetImageCmp()->GetPassRequired() * 100.0f
			);
	};

	if(!m_pCurrentMode->bWindowed)
	{
		WriteStatus
			(
			_T("$wExecuting"),
			_T("$aTest(%d/%d), Mode(%d/%d), Group(%d/%d)"), 
			m_uTestNumber,
			m_uTotalTests,
			m_uCurrentMode,
			m_uTotalModes,
			m_pFramework->GetCurrentTestNum(),
			m_pFramework->GetNumTests()
			);
	};
}


///////////////////////////////////////////////////////
// Function:  ScanEventLogForEvents
//
// Purpose:
// Scans through the specified event log in search of
// an event with the specified event ID that occurs
// after the specified start time.
//
// Returns:
// S_OK     A matching event was found.
// S_FALSE  No matching event was found.
// E_xxxx   An error occurred.
///////////////////////////////////////////////////////
HRESULT CD3DTest::ScanEventLogForEvents
(
	__in_opt LPCTSTR pEventLogName,
	__in SYSTEMTIME * pScanStartSystemTime,
	UINT cEventIDs,
	__in_ecount( cEventIDs ) DWORD * pEventIDs,
	__out_opt DWORD * pMatchingEventID
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	FILETIME ScanStartFileTime;
	SYSTEMTIME ScanStartLocalTime;

	HANDLE hEventLog = NULL;
	HANDLE hEventLogChangeEvent = NULL;
	BYTE * pEventRecordBuffer = NULL;

	const UINT nEventRecordBuffer = 4 * 1024;


	//
	// Validate parameters.
	//

	if ( pScanStartSystemTime == NULL )
	{
		hOverallResult = E_INVALIDARG;

		goto Cleanup;
	};


	//
	// Convert the starting system time to a file time.
	//

	SystemTimeToFileTime
	(
		pScanStartSystemTime,
		&ScanStartFileTime
	);

	SystemTimeToTzSpecificLocalTime
	(
		NULL,
		pScanStartSystemTime,
		&ScanStartLocalTime
	);


	//#define FULL_DEBUG 1
#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CD3DTest::ScanEventLogForEvents:  Scanning for an event on or after %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u." ),
		ScanStartLocalTime.wMonth,
		ScanStartLocalTime.wDay,
		ScanStartLocalTime.wYear,
		ScanStartLocalTime.wHour,
		ScanStartLocalTime.wMinute,
		ScanStartLocalTime.wSecond
	);
#endif


	//
	// Create an event handle to use for signalling updates
	// to the event log.
	//

	hEventLogChangeEvent = CreateEvent
	(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	if
	(
		hEventLogChangeEvent == NULL ||
		hEventLogChangeEvent == INVALID_HANDLE_VALUE
	)
	{
		DWORD CreateEventLogChangeEventError = ::GetLastError();

		WriteToLog
		(
			_T( "CD3DTest::ScanEventLogForEvents:  CreateEvent failed with error %u." ),
			CreateEventLogChangeEventError
		);

		if ( CreateEventLogChangeEventError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( CreateEventLogChangeEventError );
		};

		goto Cleanup;
	};


	//
	// Allocate an event record buffer.
	//

	pEventRecordBuffer = new BYTE[ nEventRecordBuffer ];

	if ( pEventRecordBuffer == NULL )
	{
		hOverallResult = E_OUTOFMEMORY;

		goto Cleanup;
	};

	ZeroMemory( pEventRecordBuffer, nEventRecordBuffer );


	//
	// Open the event log.
	//

	hEventLog = OpenEventLog
	(
		NULL,
		pEventLogName
	);

	if ( hEventLog == NULL )
	{
		DWORD OpenEventLogError = ::GetLastError();

		DPF
		(
			1,
			_T( "CD3DTest::ScanEventLogForEvents:  OpenEventLog failed with error %u.\n" ),
			OpenEventLogError
		);

		if ( OpenEventLogError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( OpenEventLogError );
		};

		goto Cleanup;
	};


	//
	// Notify the event log that we want to be notified about
	// changes.
	//

	BOOL bNotifyChangeEventLogResult = NotifyChangeEventLog
	(
		hEventLog,
		hEventLogChangeEvent
	);

	if ( bNotifyChangeEventLogResult != TRUE )
	{
		DWORD NotifyChangeEventLogError = ::GetLastError();

		WriteToLog
		(
			_T( "CD3DTest::ScanEventLogForEvents:  NotifyChangeEventLog failed with error %u." ),
			NotifyChangeEventLogError
		);

		if ( NotifyChangeEventLogError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( NotifyChangeEventLogError );
		};

		goto Cleanup;
	};


	//
	// Wait around for changes to the event log.
	//
	// @N.B. [SEdmison]:
	// It may be the case that the target event hasn't been
	// logged yet.  So we'll vamp for a little while in order
	// to give the event log a chance to catch up.
	//

	DWORD WaitResult = WaitForSingleObject
	(
		hEventLogChangeEvent,
		5 * 1000
	);


	//
	// Determine how many events there are in the event log.
	//

	DWORD cEventLogRecords = 0;
	BOOL bGetEventCountResult = GetNumberOfEventLogRecords
	(
		hEventLog,
		&cEventLogRecords
	);

	if ( bGetEventCountResult != TRUE )
	{
		DWORD GetNumberOfEventLogRecordsError = ::GetLastError();

		WriteToLog
		(
			_T( "CD3DTest::ScanEventLogForEvents:  GetNumberOfEventLogRecords failed with error %u." )
		);

		if ( GetNumberOfEventLogRecordsError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetNumberOfEventLogRecordsError );
		};

		goto Cleanup;
	};

#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CD3DTest::ScanEventLogForEvents:  Found %u events in System event log." ),
		cEventLogRecords
	);
#endif


	//
	// Determine how which event record in the event log is the oldest.
	//

	DWORD iOldestEventLogRecord = 0;
	BOOL bGetOldestEventRecordResult = GetOldestEventLogRecord
	(
		hEventLog,
		&iOldestEventLogRecord
	);

	if ( bGetOldestEventRecordResult != TRUE )
	{
		DWORD GetOldestEventRecordError = ::GetLastError();

		WriteToLog
		(
			_T( "CD3DTest::ScanEventLogForEvents:  GetOldestEventLogRecord failed with error %u." )
		);

		if ( GetOldestEventRecordError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetOldestEventRecordError );
		};

		goto Cleanup;
	};

#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CD3DTest::ScanEventLogForEvents:  Oldest event record is %u." ),
		iOldestEventLogRecord
	);
#endif


	//
	// Starting from the last record in the log, scan backward
	// until we find an event that matches our target event ID,
	// until we pass the specified time stamp, or until we run
	// out of events to process (that is, we reached the
	// beginning of the log).
	//

	DWORD cBytesRead = 0;
	DWORD cBytesNeeded = 0;

	UINT cEventLogRecordsProcessed = 0;

	BOOL bContinue = TRUE;

	for
	(
		BOOL bReadEventResult = ReadEventLog
		(
			hEventLog,
			EVENTLOG_SEEK_READ |
			EVENTLOG_FORWARDS_READ,
			iOldestEventLogRecord,
			pEventRecordBuffer,
			nEventRecordBuffer,
			&cBytesRead,
			&cBytesNeeded
		);
		bContinue == TRUE;
		bReadEventResult = ReadEventLog
		(
			hEventLog,
			EVENTLOG_SEQUENTIAL_READ |
			EVENTLOG_FORWARDS_READ,
			0,
			pEventRecordBuffer,
			nEventRecordBuffer,
			&cBytesRead,
			&cBytesNeeded
		)
	)
	{
		if ( bReadEventResult == FALSE )
		{
			DWORD ReadEventLogError = ::GetLastError();

			if ( ReadEventLogError == ERROR_HANDLE_EOF )
			{
				// We've reached the end of the event log.

#if defined(FULL_DEBUG)
				WriteToLog
				(
					_T( "CD3DTest::ScanEventLogForEvents:  Reached end of event log after %u events." ),
					cEventLogRecordsProcessed
				);
#endif

				bContinue = FALSE;
				break;
			}
			else if ( ReadEventLogError == ERROR_INSUFFICIENT_BUFFER )
			{
				DPF
				(
					1,
					_T( "CD3DTest::ScanEventLogForEvents:  ReadEventLog failed with error ERROR_INSUFFICIENT_BUFFER.  Needed %u bytes.\n" ),
					cBytesNeeded
				);

				hOverallResult = HRESULT_FROM_WIN32( ReadEventLogError );
				goto Cleanup;
			}
			else
			{
				DPF
				(
					1,
					_T( "CD3DTest::ScanEventLogForEvents:  ReadEventLog failed with error %u.\n" ),
					ReadEventLogError
				);

				if ( ReadEventLogError != ERROR_SUCCESS )
				{
					hOverallResult = HRESULT_FROM_WIN32( ReadEventLogError );
				};

				goto Cleanup;
			};
		};


		if ( cBytesRead == 0 )
		{
			hOverallResult = S_FALSE;

			goto Cleanup;
		};


		for
		(
			EVENTLOGRECORD * pEventRecord = (EVENTLOGRECORD *) pEventRecordBuffer;
			pEventRecord != NULL &&
				(BYTE *) pEventRecord < ( pEventRecordBuffer + cBytesRead ) &&
				cEventLogRecordsProcessed <= cEventLogRecords;
			pEventRecord = (EVENTLOGRECORD *) ( (BYTE *) pEventRecord + pEventRecord->Length )
		)
		{
			//
			// Convert the event record's time offset to a FILETIME.
			//
			// @N.B. [SEdmison]:
			// Look a little strange?  Does to me, too.  This code
			// comes from MSDN.
			//
			// I sat down and thought about the math here, and I'm
			// not entirely sure how they're accounting for the
			// discontinuities in UTC (like the fact that it didn't
			// exist prior to 1972, much less all the way back to
			// 1601, or the addition and removal of leap-seconds).
			// In fact, I'm not sure how the rest of the OS deals
			// with these issues (if at all).
			//
			// For anyone who's read this far, my calculation for
			// the number of intervals between 1601 and 1970 is
			// as follows:
			//
			// (
			//   (
			//     369 years between 1970 and 1601
			//     * 365 days per year
			//   )
			//   + 92 leap-days
			//   - 3 nonexistent leap-days for 1700, 1800, and 1900
			// )
			// * 24 hours per day
			// * 60 minutes per hour
			// * 60 seconds per minute (ignoring leap-seconds)
			// * 1000 milliseconds per second
			// * 1000 microseconds per millisecond
			// * 10 100-nanosecond intervals per microsecond
			// for a grand total of
			//   116444736000000000.
			//

			FILETIME EventRecordFileTime;

			__int64 NumberOf100nsIntervalsFrom_1_1_1601_to_1_1_1970 = 116444736000000000;
			__int64 NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated = Int32x32To64
			(
				pEventRecord->TimeGenerated, // in seconds from 1/1/1970
				10000000 // 1000 milliseconds per second * 1000 microseconds per millisecond * 10 100-nanosecond intervals per microsecond
			) + NumberOf100nsIntervalsFrom_1_1_1601_to_1_1_1970;

			EventRecordFileTime.dwLowDateTime = (DWORD) NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated;
			EventRecordFileTime.dwHighDateTime = (DWORD) ( NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated >> 32 );

			SYSTEMTIME EventRecordSystemTime;
			SYSTEMTIME EventRecordLocalTime;

			FileTimeToSystemTime
			(
				&EventRecordFileTime,
				&EventRecordSystemTime
			);

			SystemTimeToTzSpecificLocalTime
			(
				NULL,
				&EventRecordSystemTime,
				&EventRecordLocalTime
			);


			//
			// Dump information about the record.
			//

#if defined(FULL_DEBUG)
			WriteToLog
			(
				_T( "CD3DTest::ScanEventLogForEvents:  Event record %u:  ID %u, generated at %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u." ),
				pEventRecord->RecordNumber,
				pEventRecord->EventID,
				EventRecordLocalTime.wMonth,
				EventRecordLocalTime.wDay,
				EventRecordLocalTime.wYear,
				EventRecordLocalTime.wHour,
				EventRecordLocalTime.wMinute,
				EventRecordLocalTime.wSecond
			);
#endif


			//
			// Check whether the event falls prior to the start
			// of our scan interval by more than 10 seconds.
			//

			if
			(
				EventRecordFileTime.dwHighDateTime < ScanStartFileTime.dwHighDateTime ||
				EventRecordFileTime.dwHighDateTime == ScanStartFileTime.dwHighDateTime &&
				EventRecordFileTime.dwLowDateTime < ScanStartFileTime.dwLowDateTime - 10 * 10000000
			)
			{
#if 0
#if defined(FULL_DEBUG)
				WriteToLog
				(
					_T( "CD3DTest::ScanEventLogForEvents:  Found an event earlier than our scan window after processing %u records." ),
					cEventLogRecordsProcessed
				);
#endif

				bContinue = FALSE;
				break;
#else
				++cEventLogRecordsProcessed;
				continue;
#endif
			};


			//
			// Check whether the event matches the set for
			// which we're scanning.
			//

			for
			(
				UINT iCurrentEventID = 0;
				iCurrentEventID < cEventIDs;
				++iCurrentEventID
			)
			{
				if ( pEventRecord->EventID == pEventIDs[ iCurrentEventID ] )
				{
					if ( pMatchingEventID != NULL )
					{
						*pMatchingEventID = pEventIDs[ iCurrentEventID ];
					};

					hOverallResult = S_OK;

					goto Cleanup;
				};
			};


			//
			// Keep a count of how many records we've scanned.
			//

			++cEventLogRecordsProcessed;
		};
	};


	hOverallResult = S_FALSE;


Cleanup:
	if ( pEventRecordBuffer != NULL )
	{
		delete [] pEventRecordBuffer;
		pEventRecordBuffer = NULL;
	};


	if
	(
		hEventLogChangeEvent != NULL &&
		hEventLogChangeEvent != INVALID_HANDLE_VALUE
	)
	{
		CloseHandle( hEventLogChangeEvent );
		hEventLogChangeEvent = NULL;
	};


	if ( hEventLog != NULL )
	{
		CloseEventLog( hEventLog );
		hEventLog = NULL;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  ProcessError
//
// Purpose:
// Handles certain Direct3D/DDraw error codes.
///////////////////////////////////////////////////////
void CD3DTest::ProcessError(HRESULT hr)
{
	// If we ever encounter DRIVERINTERNALERROR(will only be set for DX9 interfaces and above), we 
	// will abort the remaning test cases.  Driver is in a bad state and should never be
	// failing the flush.
	if ( m_pD3D->IsDriverInternalError() )
	{
		WriteToLog(_T("CD3DTest::ProcessError:  Encountered internal driver error.  Aborting remaining test cases."));
		Abort();
		TestFinish(D3DTESTFINISH_SKIPREMAINDER);
	}
	else if ( FAILED( hr ) )
	{
		WriteStatus
		(
			_T("$wError"),
			_T("\t$r%s"),
			m_pD3D->HResultToString( hr )
		);

		if
		(
			hr == D3DERR_DEFERRED_DP2ERROR ||
			hr == D3DERR_DRIVERINTERNALERROR
		)
		{
			WriteToLog(_T("CD3DTest::ProcessError:  Encountered error D3DERR_DRIVERINTERNALERROR.  Aborting remaining test cases."));
			Abort();
			TestFinish(D3DTESTFINISH_SKIPREMAINDER);
		}
		else if
		(
			hr == DDERR_SURFACELOST ||
			hr == D3DERR_DEVICELOST
		)
		{
			DPF
			(
				1,
				_T("CD3DTest::ProcessError:  Encountered device/surface loss error %x (%s).\n"),
				hr,
				m_pD3D->HResultToString(hr)
			);


			//
			// Check for a TDR in the event log.
			//

			DWORD TDREventIDs[] =
			{
				4097,
				4099,
				4101
			};

			DWORD MatchingEventID = 0;

			HRESULT hScanForTDREventResult = ScanEventLogForEvents
			(
				_T( "System" ),
				&m_TestCaseStartTime,
				ARRAYSIZE( TDREventIDs ),
				TDREventIDs,
				&MatchingEventID
			);

			if ( hScanForTDREventResult == S_OK )
			{
				DPF
				(
					1,
					_T( "CD3DTest::ProcessError:  A timeout detection and recovery (TDR) event (%u) was detected in the System event log during or just prior to this test case.\n" ),
					MatchingEventID
				);
			};


			//
			// Determine whether to fail or skip as a result of the device loss.
			//

			if ( m_Options.TestOptions.bSkipOnDeviceLost )
			{
				Skip();
			}
			else
			{
				Fail();
			}
		}
		else
		{
			WriteToLog
			(
				_T("CD3DTest::ProcessError:  Encountered error %x (%s)."),
				hr,
				m_pD3D->HResultToString(hr)
			);

			Fail();
		}
	}
}


HRESULT CD3DTest::SetLastError(HRESULT hr)
{ 
	return m_pD3D->SetLastError(hr); 
}


HRESULT CD3DTest::GetLastError()
{ 
	return m_pD3D->GetLastError();
}


void CD3DTest::ExecuteOneTest(void)
{
	//
	// Initialize locals.
	//

	HRESULT hr = 0;
	FPSTATE DCTStateBefore;
	FPSTATE DCTStateAfter;


	//
	// Initialize members and state.
	//

	// Initialize the conformance pass percentage.
	m_fPassPercentage = -1.f;

	// Note test case start time.
	GetSystemTime( &m_TestCaseStartTime );


	//
	// Validate members and state.
	//

	// Verify that we're on a valid test.
	if (TERMINATE_NUMBER == m_uTestNumber)
	{
		return;
	}

	// Check for device lost and handle it as appropriate.
	if (IsDeviceLost())
	{
		// If we fail to restore the lost device, exit out.
		if (!HandleDeviceLost())
			return;
	}

	// Get floating point state	
	if (NULL != g_DCTGet)
		g_DCTGet(&DCTStateBefore);


	//
	// Set up for the test case.
	//

	// Clear the render target.
	ClearFrame();

	// Set new API.
	SwitchAPI(m_uTestNumber);


	//
	// Run the test case.
	//

	// Execute by test number callback
	if(ExecuteTest(m_uTestNumber))
	{
		// Begin the test case if the test didn't
		if (!m_pFramework->m_DXGLog.InTestCase())
		{
			BeginTestCase( _T( "Unknown Test Case" ), m_uTestNumber );
		}


		// Render the scene - legacy
		SceneRefresh();


		// If we're generating a golden image, save off the src backbuffer
		if(m_Options.D3DOptions.bGoldReference)
		{
			hr = ProcessGoldenImage();
			if(FAILED(hr))
			{
				WriteToLog(_T("ProcessGoldenImage failed. hr=%s(%x).\n"), 
					m_pD3D->HResultToString(hr), hr);
				Fail();
			}
		}


		// Render validation (ie. image comparison)
		if(IsCompareEnabled())
		{
			if ( m_Options.TestOptions.bCompareFB )
			{
				DisplayFrame();
			}

			ProcessFrame();
		}
		else
		{
			Pass();
		}

		// Need to update before DisplayFrame since fullscreen DisplayFrame
		// needs this info.  This info can potentially be incorrect if something
		// fails after here(ex. Present fails).  There really is no way to 
		// avoid this since we need to render the status info frame, but
		// we want the log to be accurate in all cases.
		if(!m_pCurrentMode->bWindowed)
		{
			UpdateControls();

			// Update the onscreen stats for fullscreen
			for(UINT nDevices=0; nDevices < m_uDeviceList; nDevices++)
			{
				if(m_DeviceList[nDevices]->IsFullscreen())
				{
					DrawStats(m_pCurrentMode->pDisplay->dwWidth, m_pCurrentMode->pDisplay->dwHeight, m_DeviceList[nDevices]);
				}
			}
		}

		//Don't DisplayFrame() twice
		if( !m_Options.TestOptions.bCompareFB )
		{
			// Display the frame
			DisplayFrame();
		}
	}


	//
	// Process errors.
	//

	ProcessError(GetLastError());

	if (!m_Options.TestOptions.bIgnoreFPStateCheck)
	{
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
	}


	//
	// End the test case.
	//
	// @N.B.:
	// The test should call BeginTestCase within ExecuteTest.
	//

	EndTestCase();


	//
	// Update the status controls for windowed modes.
	//

	if(m_pCurrentMode->bWindowed)
	{
		UpdateControls();
	}


	//
	// Halt on failures.
	//

	if
	(
		m_bHaltOnFail &&
		m_pFramework->m_DXGLog.GetCurrentResult() != WTT_TESTCASE_RESULT_PASS &&
		m_pFramework->m_DXGLog.GetCurrentResult() != WTT_TESTCASE_RESULT_SKIPPED
	)
	{
		DPF( 1, _T("Halting due to failure.\n") );

		SendMessage(m_pFramework->m_hWindow, WM_COMMAND, IDM_EXECUTION_PAUSE, 0);

		if(!m_bShowCompareWindow)
		{
			ToggleCompareWindow();
		}
	}
}


void CD3DTest::UpdateControls()
{
	// Clear the controls
	m_pFramework->ClearControls();

	// Allow the test/fmwk to set current status
	UpdateStatus();

	// Update the test details
	UpdateDetails();

	// Update the windows with the new data
	m_pFramework->UpdateControls(this);
}


bool CD3DTest::HandleDeviceLost()
{
	HRESULT hr = S_OK;
	bool bRes = true;

	// Check for TS bug workaround. If we're lost due to TS client connection,
	// we need to recreate all devices.  Otherwise, we'll go through the reset
	// codepath.
	if(!m_pCurrentMode->bWindowed && GetSystemMetrics(SM_REMOTESESSION))
	{
		if(!ResetTSConnect())
		{
			// Couldn't restore - exit requested
			m_uTestNumber = TERMINATE_NUMBER;
			bRes = false;
		}

		// Flush any additional errors
		hr = GetLastError();
	}
	else
	{
		if(!ResetLostDevices())
		{
			// Couldn't restore - exit requested
			m_uTestNumber = TERMINATE_NUMBER;
			bRes = false;
		}

		// Flush any additional errors
		hr = GetLastError();
	}
	return bRes;
}


bool CD3DTest::ProcessFrame(void)
{
	bool bRes = true;
	HRESULT hr = S_OK;
	CSurface *pSrcSurf = NULL;
	CSurface *pRefSurf = NULL;
	CSurface *pCmpSurf = NULL;
	CSurface *pFBSurf = NULL;

	if (m_Options.D3DOptions.bReference && IsCompareEnabled())
	{
		// Special case modes with MS enabled

		if(m_pCurrentMode->nMultisample > 0 && !m_Options.TestOptions.bCompareFB)
		{
			DPF(1, _T("CD3DTest::ProcessFrame:  Compare requested for MultiSample backbuffer(support for front buffer only).\n"));
			bRes = false;
			goto Exit;
		}

		if ( m_Options.TestOptions.bCompareFB )
		{
			CSurface *pSurfaces[2] = {NULL, NULL};

			if ( m_pSrcDevice->IsFullscreen() )
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  Front buffer compare requested for fullscreen(support for windowed only).\n"));
				bRes = false;
				goto Exit;
			}

			hr = GetFrontBuffers( &pSurfaces[0], CD3D_SRC | CD3D_REF );
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  GetFrontBuffers failed. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				bRes = false;
				goto Exit;
			}

			pSrcSurf = pSurfaces[0];
			pRefSurf = pSurfaces[1];
		}
		else
		{
			// Get the src buffer
			hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);

			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  GetBackBufferCached failed on SRC. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				bRes = false;
				goto Exit;
			}

			// Get the ref buffer
			hr = m_pRefDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pRefSurf);

			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  GetBackBufferCached failed on REF. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				bRes = false;
				goto Exit;
			}
		}

		if(m_pCurrentMode->bWindowed)
		{
			hr = m_pCmpSwapChain->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pCmpSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  GetBackBufferCached failed.\n"));
				goto Exit;
			}
		}
		else
		{
			hr = m_pCmpTexture->GetSurfaceLevel(0, &pCmpSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::ProcessFrame:  GetSurfaceLevel failed.\n"));
				goto Exit;
			}
		}

		// Compare the buffers
		if(FAILED(m_pFramework->GetImageCmp()->CompareImages(pSrcSurf, pRefSurf, pCmpSurf)))
		{
			DPF(1, _T("CD3DTest::ProcessFrame:  CompareImages failed.\n"));
			Fail();
			goto Exit;
		}

		// Check the results
		m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();

		// If the device is now lost, it's possible that the compare results are invalid
		// because D3D will hand a dummy ptr back from Lock() with zero'd memory.
		// We don't want to fail the compare in that case, since this will be handled
		// by the device lost code later on and the test may choose to skip, not fail.
		if(m_pFramework->m_DXGLog.InTestCase() && !IsDeviceLost())
		{
			if(m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired())
			{
				WriteToLog
					(
					_T( "Image conformance of %2.2f%% was less than the required %2.2f%%." ),
					m_fPassPercentage * 100.f,
					m_pFramework->GetImageCmp()->GetPassRequired() * 100.f
					);

				Fail();
			}
			else
			{
				Pass();
			};
		}
	}

Exit:
	RELEASE(pSrcSurf);
	RELEASE(pRefSurf);
	RELEASE(pCmpSurf);
	RELEASE(pFBSurf);

	return bRes;
}


bool CD3DTest::FilterResolution(PDISPLAYMODE pMode)
{
	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CD3DTest::FilterResolution:  pMode is NULL.\n" ) );
		return false;
	};


	//
	// Validate members.
	//

	// Make sure that we have a device.
	if (!m_DeviceList[0])
	{
		DPF(1, _T("CD3DTest::FilterResolution:  Invalid device list.\n"));
		return false;
	};


	// Make sure that we have an adapter.
	ADAPTER *pAdapter = m_DeviceList[0]->GetAdapter();

	if (!pAdapter)
	{
		DPF(1, _T("CD3DTest::FilterResolution:  Unable to get adapter.\n"));
		return false;
	};


	//
	// Establish the set of framework-acceptable modes.
	//

	DWORD FrameworkAcceptableResolutionsPreWin8[][3] =
	{
		// Landscape-orientation modes
		{  640,  480, 16 },
		{  640,  480, 32 },
		{  800,  600, 16 },
		{  800,  600, 32 },
		{ 1024,  768, 16 },
		{ 1024,  768, 32 },

		// Portrait-orientation modes
		{  480,  640, 16 },
		{  480,  640, 32 },
		{  600,  800, 16 },
		{  600,  800, 32 },
		{  768, 1024, 16 },
		{  768, 1024, 32 }
	};

	// In Windows 8 and later the 16-bit display modes are no longer available
	DWORD FrameworkAcceptableResolutionsPostWin8[][3] =
	{
		// Landscape-orientation modes
		{  640,  480, 32 },
		{  800,  600, 32 },
		{ 1024,  768, 32 },

		// Portrait-orientation modes
		{  480,  640, 32 },
		{  600,  800, 32 },
		{  768, 1024, 32 },

		// Used to indicate that we want the current desktop mode added.
		{ 0, 0, 0} 
	};

	// Get the Windows version
	OSVERSIONINFO windowsVersionInfo;
	windowsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&windowsVersionInfo);

	DWORD (*FrameworkAcceptableResolutions)[3];
	int cFrameworkAcceptableResolutions = 0;

	if(windowsVersionInfo.dwMajorVersion > 6 || (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion >= 2))
	{
		// Use resolutions for Windows 8 and beyond
		FrameworkAcceptableResolutions = FrameworkAcceptableResolutionsPostWin8;
		cFrameworkAcceptableResolutions = ARRAYSIZE( FrameworkAcceptableResolutionsPostWin8 );
	}
	else
	{
		// Use resolutions for Windows 7 and prior
		FrameworkAcceptableResolutions = FrameworkAcceptableResolutionsPreWin8;
		cFrameworkAcceptableResolutions = ARRAYSIZE( FrameworkAcceptableResolutionsPreWin8 );
	}

	// If we want to filter the list...
	if
	(
		(FMT) m_Options.D3DOptions.DisplayMode.Format.d3dfFormat == FMT_ALL ||
		KeySet(_T("whql"))
	)
	{
		// ...then drop down to the filtering logic below...
	}
	// ...or, if we just want to use the current resolution...
	else if
	(
		(FMT) m_Options.D3DOptions.DisplayMode.Format.d3dfFormat == FMT_ANY
	)
	{
		// ...then manipulate the list of acceptable modes so that
		// the current mode is the only mode in the list...
		FrameworkAcceptableResolutions[ 0 ][ 0 ] = pAdapter->DesktopMode.dwWidth;
		FrameworkAcceptableResolutions[ 0 ][ 1 ] = pAdapter->DesktopMode.dwHeight;
		FrameworkAcceptableResolutions[ 0 ][ 2 ] = pAdapter->DesktopMode.dwBPP;

		int cFrameworkAccepableResolutions = 1;
	}
	// ...otherwise, even if we don't want to constrain ourselves to the list above...
	else
	{
		// ...we still need to reject modes that are too small.
		if
		(
			pMode->dwWidth >= 640 &&
			pMode->dwHeight >= 480
		)
		{
			return true;
		};

		return false;
	};


	//
	// Walk the list of framework-acceptable modes in order to
	// determine whether to accept or reject the specified mode.
	//

	for
	(
		int i = 0;
		i < cFrameworkAcceptableResolutions;
		++i
	)
	{
		if
		(
			FrameworkAcceptableResolutions[ i ][ 0 ] == pMode->dwWidth && 
			FrameworkAcceptableResolutions[ i ][ 1 ] == pMode->dwHeight &&
			FrameworkAcceptableResolutions[ i ][ 2 ] == pMode->dwBPP
		)
		{
			return true;
		};
	};

	return false;
}


void CD3DTest::ReleaseCommonFormats()
{
	SAFEDELETEA(m_pCommonZBufferFormats);
	SAFEDELETEA(m_pCommonTextureFormats);
	SAFEDELETEA(m_pCommonCubeFormats);
	SAFEDELETEA(m_pCommonVolumeFormats);
	SAFEDELETEA(m_pCommonBackBufferFormats);
	SAFEDELETEA(m_pCommonMultiSampleFormats);
}


void CD3DTest::ReleaseModeList()
{
	TESTMODE *pMode = m_pModeList;

	m_uTotalModes = 0;
	m_uCurrentMode = 0;

	while (pMode)
	{
		TESTMODE *pTemp = pMode->pNext;
		SAFEDELETE(pMode->pDisplay);
		SAFEDELETE(pMode);
		pMode = pTemp;
	}

	m_pModeList = NULL;
	m_pCurrentMode = NULL;
}


bool CD3DTest::CopyFormatList(FORMAT *pSrcFmt, FORMAT **pFmtOut, UINT *uFmtOut)
{
	FORMAT CommonFmts[256];
	UINT uCommonFmts = 0;

	if ( pFmtOut == NULL )
	{
		DPF( 1, _T( "CD3DTest::CopyFormatList:  pFmtOut is NULL.\n" ) );
		return false;
	};

	if ( uFmtOut == NULL )
	{
		DPF(1, _T( "CD3DTest::CopyFormatsList:  uFmtOut is NULL.\n"));
		return false;
	}

	FORMAT *pTempFmt = pSrcFmt;
	while(pTempFmt)
	{
		memcpy(&(CommonFmts[uCommonFmts++]), pTempFmt, sizeof(FORMAT));
		pTempFmt = pTempFmt->pNext;
	}

	if(uCommonFmts)
	{
		*pFmtOut = new FORMAT[uCommonFmts];

		if(NULL == *pFmtOut)
		{
			DPF(1, _T("CD3DTest::CopyFormatList:  Failed to allocate format *pFmtOut.\n"));
			return false;
		}

		memcpy(*pFmtOut, CommonFmts, sizeof(FORMAT) * uCommonFmts);

		// Need to fix up the linked list ptrs

		for(UINT i=0; i<uCommonFmts-1; i++)
			(*pFmtOut)[i].pNext = &((*pFmtOut)[i+1]);

		(*pFmtOut)[uCommonFmts-1].pNext = NULL;

		*uFmtOut = uCommonFmts;
	}
	else
	{
		*pFmtOut = NULL;
		*uFmtOut = 0;
	}

	return true;
}


bool CD3DTest::FindCommonFormats
(
	FORMAT *pSrcFmt,
	FORMAT *pRefFmt,
	FORMAT **pFmtOut,
	UINT *uFmtOut
)
{
	FORMAT CommonFmts[256];
	UINT uCommonFmts = 0;

	if(m_pD3D == NULL)
	{
		DPF(1, _T("CD3DTest::FindCommonFormats:  m_pD3D is NULL.\n"));
		return false;
	}

	if(pSrcFmt == NULL || pRefFmt == NULL)
	{
		*pFmtOut = NULL;
		*uFmtOut = 0;
		return true;
	}

	FORMAT *pTempFmt = pSrcFmt;
	while(pTempFmt)
	{
		FORMAT *pTempRef = pRefFmt;
		while(pTempRef)
		{
			if(m_pD3D->CompareFormats(pTempFmt, pTempRef))
			{
				memcpy(&(CommonFmts[uCommonFmts++]), pTempFmt, sizeof(FORMAT));
				break;
			}

			pTempRef = pTempRef->pNext;
		}

		pTempFmt = pTempFmt->pNext;
	}

	if(uCommonFmts)
	{
		*pFmtOut = new FORMAT[uCommonFmts];

		if(NULL == *pFmtOut)
		{
			DPF(1, _T("CD3DTest::FindCommonFormats:  Failed to allocate format.\n"));
			return false;
		}

		memcpy(*pFmtOut, CommonFmts, sizeof(FORMAT) * uCommonFmts);

		// Need to fix up the linked list ptrs

		for(UINT i=0; i<uCommonFmts-1; i++)
			(*pFmtOut)[i].pNext = &((*pFmtOut)[i+1]);

		(*pFmtOut)[uCommonFmts-1].pNext = NULL;

		*uFmtOut = uCommonFmts;
	}

	return true;
}


bool CD3DTest::FindCommonFormats
(
	MULTISAMPLE_TYPE *pSrc,
	UINT uSrc,
	MULTISAMPLE_TYPE *pRef,
	UINT uRef,
	MULTISAMPLE_TYPE **pFmtOut,
	UINT *uFmtOut
)
{
	MULTISAMPLE_TYPE CommonFmts[256];
	UINT uCommonFmts = 0;

	if(m_pD3D == NULL)
	{
		DPF(1, _T("CD3DTest::FindCommonFormats:  m_pD3D is NULL.\n"));
		return false;
	}

	if(pSrc == NULL)
	{
		*pFmtOut = NULL;
		*uFmtOut = 0;
		return true;
	}

	if(pRef == NULL)
	{
		*uFmtOut = uSrc;
		*pFmtOut = new MULTISAMPLE_TYPE[*uFmtOut];
		if (*pFmtOut == NULL)
		{
			DPF(1, _T("CD3DTest::FindCommonFormats:  Failed to allocate multisample type *pFmtOut.\n"));
			*uFmtOut = 0;
			return false;
		}
		memcpy(*pFmtOut, pSrc, sizeof(MULTISAMPLE_TYPE) * (*uFmtOut));
		return true;
	}

	for(UINT i=0; i<uSrc; i++)
	{
		for(UINT j=0; j<uRef; j++)
		{
			if(pSrc[i] == pRef[j])
			{
#if defined(_PREFAST_)
				__analysis_assume( uCommonFmts < 256 );
#endif

				ASSERT(uCommonFmts < ARRAYSIZE(CommonFmts));
				memcpy(&(CommonFmts[uCommonFmts++]), &(pSrc[i]), sizeof(MULTISAMPLE_TYPE));
				break;
			}
		}
	}

	if(uCommonFmts)
	{
		*pFmtOut = new MULTISAMPLE_TYPE[uCommonFmts];
		if(*pFmtOut == NULL)
		{
			DPF(1, _T("CD3DTest::FindCommonFormats:  Failed to allocate multisample format *pFmtOut.\n"));
			return false;
		}
		memcpy(*pFmtOut, CommonFmts, sizeof(MULTISAMPLE_TYPE) * uCommonFmts);

		*uFmtOut = uCommonFmts;
	}

	return true;
}


bool CD3DTest::EnumCommonFormats(CDevice **pDevList, UINT uDevices)
{
	UINT nZFmts = 0;
	UINT nTexFmts = 0;
	UINT nCubeFmts = 0;
	UINT nVolFmts = 0;
	UINT nMSFmts = 0;
	UINT nBBFmts = 0;
	FORMAT *pZFmts = NULL;
	FORMAT *pTexFmts = NULL;
	FORMAT *pCubeFmts = NULL;
	FORMAT *pVolFmts = NULL;
	FORMAT *pBBFmts = NULL;
	MULTISAMPLE_TYPE *pMSFmts = NULL;
	FORMAT *pFmt = NULL;
	UINT 	nFmt = 0;


	//
	// Validate parameters.
	//

	if ( pDevList == NULL )
	{
		DPF( 1, _T( "CD3DTest::EnumCommonFormats:  pDevList is NULL.\n" ) );
		return false;
	};

	if ( pDevList[ 0 ] == NULL )
	{
		DPF( 1, _T( "CD3DTest::EnumCommonFormats:  pDevList[0] is NULL.\n" ) );
		return false;
	};

	if ( uDevices == 0 )
	{
		DPF(1, _T("CD3DTest::EnumCommonFormats:  uDevices is 0.\n"));
		return false;
	};


	// Release any existing formats
	ReleaseCommonFormats();

	// Set the master list to the list for the first device
	if(!CopyFormatList(pDevList[0]->GetZFormats(), &m_pCommonZBufferFormats, &m_uCommonZBufferFormats) ||
		!CopyFormatList(pDevList[0]->GetTexFormats(), &m_pCommonTextureFormats, &m_uCommonTextureFormats) ||
		!CopyFormatList(pDevList[0]->GetCubeFormats(), &m_pCommonCubeFormats, &m_uCommonCubeFormats) ||
		!CopyFormatList(pDevList[0]->GetVolumeFormats(), &m_pCommonVolumeFormats, &m_uCommonVolumeFormats) ||
		!CopyFormatList(pDevList[0]->GetBackBufferFormats(), &m_pCommonBackBufferFormats, &m_uCommonBackBufferFormats) ||
		!FindCommonFormats(pDevList[0]->GetMultiSampleFormats(), pDevList[0]->GetNumMultiSampleFormats(), NULL, 0, &m_pCommonMultiSampleFormats, &m_uCommonMultiSampleFormats))
	{
		DPF(1, _T("CD3DTest::EnumCommonFormats:  Failed to find common formats.\n"));
		return false;
	}

	for(UINT i=1; i<uDevices; i++)
	{	
		CDevice *pSrc = pDevList[i];
		pZFmts = pTexFmts = pCubeFmts = pVolFmts = NULL;
		nZFmts = nTexFmts = nCubeFmts = nVolFmts = 0;

		if(!FindCommonFormats(pSrc->GetZFormats(),	m_pCommonZBufferFormats, &pZFmts, &nZFmts) ||
			!FindCommonFormats(pSrc->GetTexFormats(), m_pCommonTextureFormats, &pTexFmts, &nTexFmts) ||
			!FindCommonFormats(pSrc->GetCubeFormats(), m_pCommonCubeFormats, &pCubeFmts, &nCubeFmts) ||
			!FindCommonFormats(pSrc->GetVolumeFormats(), m_pCommonVolumeFormats, &pVolFmts, &nVolFmts) ||
			!FindCommonFormats(pSrc->GetBackBufferFormats(), m_pCommonBackBufferFormats, &pBBFmts, &nBBFmts) ||
			!FindCommonFormats(pSrc->GetMultiSampleFormats(), pSrc->GetNumMultiSampleFormats(), m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &pMSFmts, &nMSFmts))
		{
			DPF(1, _T("CD3DTest::EnumCommonFormats:  Failed to find common formats.\n"));
			return false;
		}

		// Delete the old merged format lists
		SAFEDELETEA(m_pCommonZBufferFormats);
		SAFEDELETEA(m_pCommonTextureFormats);
		SAFEDELETEA(m_pCommonCubeFormats);
		SAFEDELETEA(m_pCommonVolumeFormats);
		SAFEDELETEA(m_pCommonMultiSampleFormats);
		SAFEDELETEA(m_pCommonBackBufferFormats);

		// Save the new format lists
		m_pCommonZBufferFormats = pZFmts;	m_uCommonZBufferFormats = nZFmts;
		m_pCommonTextureFormats = pTexFmts;	m_uCommonTextureFormats = nTexFmts;
		m_pCommonCubeFormats = pCubeFmts;	m_uCommonCubeFormats = nCubeFmts;
		m_pCommonVolumeFormats = pVolFmts;	m_uCommonVolumeFormats = nVolFmts;
		m_pCommonMultiSampleFormats = pMSFmts;	m_uCommonMultiSampleFormats = nMSFmts;
		m_pCommonBackBufferFormats = pBBFmts;	m_uCommonBackBufferFormats = nBBFmts;
	}

	return true;
}


bool CD3DTest::FilterFormats(FORMAT *pList, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *pCount)
{
	bool bRes = true;
	UINT uFmt=0;
	FORMAT *pIter = pList;
	FORMAT *pHead = NULL;
	FORMAT **pTail = &pHead;

	DebugOut.Write(4, _T("CD3DTest::FilterFormats:  Start.\n"));

	if(NULL == pList)
	{
		DPF(1, _T("CD3DTest::FilterFormats:  pList is NULL.\n"));
		return false;
	}

	// Walk the input list
	while(pIter)
	{
		// Check if this fmt is included
		if (((0 == dwInclFlags) ||  (pIter->ddpfPixelFormat.dwFlags & dwInclFlags)) &&
			((0 == dwExclFlags) || !(pIter->ddpfPixelFormat.dwFlags & dwExclFlags)))
		{
			*pTail = new FORMAT;

			if(NULL == *pTail)
			{
				DPF(1, _T("CD3DTest::FilterFormats:  Failed to allocate format *pTail.\n"));
				goto Exit;
			}

			// Copy the format
			memset(*pTail, 0, sizeof(FORMAT));
			memcpy(*pTail, pIter, sizeof(FORMAT) - sizeof(FORMAT*));

			// Advance the ptr
			pTail = &((*pTail)->pNext);

			// Increment the count
			uFmt++;
		}

		pIter = pIter->pNext;
	}

	if(pFmtOut)
		bRes = CopyFormatList(pHead, pFmtOut, &uFmt);

	if(pCount)
		*pCount = uFmt;

Exit:
	pIter = pHead;
	while(pIter)
	{
		FORMAT *pTemp = pIter->pNext;
		SAFEDELETE(pIter);
		pIter = pTemp;
	}

	return bRes;
}


bool CD3DTest::FilterCommonFormats()
{
	// Filter all texture formats
	if(m_pCommonTextureFormats && (m_Options.ModeOptions.dwTexInclude || m_Options.ModeOptions.dwTexExclude))
	{
		UINT uTmp = 0;
		FORMAT *pTmp;

		if(!FilterFormats(m_pCommonTextureFormats, m_Options.ModeOptions.dwTexInclude, 
			m_Options.ModeOptions.dwTexExclude, &pTmp, &uTmp))
		{
			DPF(1, _T("CD3DTest::FilterCommonFormats:  FilterFormats failed for textures.\n"));
			SAFEDELETEA(pTmp);
			return false;
		}

		SAFEDELETEA(m_pCommonTextureFormats);

		m_pCommonTextureFormats = pTmp;
		m_uCommonTextureFormats = uTmp;
	}

	// Filter all cube formats
	if(m_pCommonCubeFormats && (m_Options.ModeOptions.dwCubeInclude || m_Options.ModeOptions.dwCubeExclude))
	{
		UINT uTmp = 0;
		FORMAT *pTmp;

		if(!FilterFormats(m_pCommonCubeFormats, m_Options.ModeOptions.dwCubeInclude, 
			m_Options.ModeOptions.dwCubeExclude, &pTmp, &uTmp))
		{
			DPF(1, _T("CD3DTest::FilterCommonFormats:  FilterFormats failed for textures.\n"));
			SAFEDELETEA(pTmp);
			return false;
		}

		SAFEDELETEA(m_pCommonCubeFormats);

		m_pCommonCubeFormats = pTmp;
		m_uCommonCubeFormats = uTmp;
	}

	// Filter all volume formats
	if(m_pCommonVolumeFormats && (m_Options.ModeOptions.dwVolInclude || m_Options.ModeOptions.dwVolExclude))
	{
		UINT uTmp = 0;
		FORMAT *pTmp;

		if(!FilterFormats(m_pCommonVolumeFormats, m_Options.ModeOptions.dwVolInclude, 
			m_Options.ModeOptions.dwVolExclude, &pTmp, &uTmp))
		{
			DPF(1, _T("CD3DTest::FilterCommonFormats:  FilterFormats failed for textures.\n"));
			SAFEDELETEA(pTmp);
			return false;
		}

		SAFEDELETEA(m_pCommonVolumeFormats);

		m_pCommonVolumeFormats = pTmp;
		m_uCommonVolumeFormats = uTmp;
	}

	// Filter all ZBuffer formats
	if(m_pCommonZBufferFormats && (m_Options.ModeOptions.dwZInclude || m_Options.ModeOptions.dwZExclude))
	{
		UINT uTmp = 0;
		FORMAT *pTmp;

		if(!FilterFormats(m_pCommonZBufferFormats, m_Options.ModeOptions.dwZInclude, 
			m_Options.ModeOptions.dwZExclude, &pTmp, &uTmp))
		{
			DPF(1, _T("CD3DTest::FilterCommonFormats:  FilterFormats failed for textures.\n"));
			SAFEDELETEA(pTmp);
			return false;
		}

		SAFEDELETEA(m_pCommonZBufferFormats);

		m_pCommonZBufferFormats = pTmp;
		m_uCommonZBufferFormats = uTmp;
	}

	return true;
}


CDevice* CD3DTest::GetDevice(UINT uDevice)
{
	if(uDevice >= m_uDeviceList)
		return NULL;

	return m_DeviceList[uDevice];
}


bool CD3DTest::AddDeviceToList(CDevice *pDevice)
{
	if(m_uDeviceList < MAX_DEVICES)
	{
		m_DeviceList[m_uDeviceList++] = pDevice;
		return true;
	}
	else
	{
		DPF(1, _T("CD3DTest::AddDeviceToList:  Too many devices.\n"));
		return false;
	}
}


// --------------------------------------------------------------------------------------
// This is used to check for group-level problems, where a particular test group might
// not be able to run but the next one could.  Ex. Given two groups, one wants to run against
// DX6 and the other against DX6 and DX7.  The first group would be failed at this point
// when handed a DX7 DEVICEMODE.

bool CD3DTest::ModeSupported
(
	__in DEVICEMODE * pMode
)
{
	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CD3DTest::ModeSupported:  pMode == NULL.\n" ) );

		return false;
	};


	// Treat x_OR_y as x and y for purposes of matching a test to a
	// device mode.  We rely on CD3DWindowFramework::InitializeTests
	// do the differentiation of x_OR_y versus x_AND_y.
	DWORD dwIncVersions = m_Options.D3DOptions.dwIncVersions;
	if ( (dwIncVersions & DXVERSION_9_OR_91) != 0 )
	{
		dwIncVersions |= DXVERSION_9;
		dwIncVersions |= DXVERSION_91;
	};

	if ( (dwIncVersions & DXVERSION_8_OR_81) != 0 )
	{
		dwIncVersions |= DXVERSION_8;
		dwIncVersions |= DXVERSION_81;
	};

	// Check that this version was selected by the current test.
	if
	(
		pMode->fVersion > m_Options.D3DOptions.fMaxDXVersion ||
		pMode->fVersion < m_Options.D3DOptions.fMinDXVersion ||
		(dwIncVersions & GetDXVersionFlag(pMode->fVersion)) == 0
	)
	{
		DPF
		(
			1,
			_T( "CD3DTest::ModeSupported:  Test group \"%s\" is being skipped because it does not support Direct3D version %2.2f.\n" ),
			m_szTestName,
			pMode->fVersion
		);

		return false;
	};

	// Resolve ambiguous behavior flags.
	DWORD RequestedBehaviors = m_Options.D3DOptions.dwBehaviors;
	m_pD3D->ResolveBehaviors(pMode->nAdapter, pMode->dwDevType, RequestedBehaviors, &RequestedBehaviors);

	// Check that this device type was selected by the current test.
	if
	(
		m_pFramework->m_dwCmdDevTypes == 0 &&
		(pMode->dwDevType & m_Options.D3DOptions.dwDevTypes) == 0
	)
	{
		DPF
		(
			1,
			_T( "CD3DTest::ModeSupported:  Test group \"%s\" is being skipped because it does not support device type %u.\n" ),
			m_szTestName,
			pMode->dwDevType
		);

		return false;
	};

	// Check that the behaviors were selected by the current test.
	if
	(
		m_pFramework->m_dwCmdBehaviors == 0 &&
		(pMode->dwBehaviors & RequestedBehaviors) != (pMode->dwBehaviors)
	)
	{
		DPF
		(
			1,
			_T( "CD3DTest::ModeSupported:  Test group \"%s\" is being skipped because its behavior flags of 0x%x do not match the device mode's behavior flags of 0x%x.\n" ),
			m_szTestName,
			RequestedBehaviors,
			pMode->dwBehaviors
		);

		return false;
	};

	// Check that this format was selected by the current test.
	FMT fmt = (FMT) m_Options.D3DOptions.DisplayMode.Format.d3dfFormat;
	if
	(
		fmt != FMT_ANY && fmt != FMT_ALL &&
		fmt != (FMT) pMode->pDispMode->Format.d3dfFormat
	)
	{
		DPF
		(
			1,
			_T( "CD3DTest::ModeSupported:  Test group \"%s\" is being skipped because it does not support format 0x%x.\n" ),
			m_szTestName,
			pMode->pDispMode->Format.d3dfFormat
		);

		return false;
	};

	return true;
};


// --------------------------------------------------------------------------------------
// This is used to check for mode-level problems, where no test group will be able to run so 
// we abort/skip out without cycling through all groups.  Ex. A pure device mode would be
// failed at this point on adapters that don't support pure devices. 

HRESULT CD3DTest::ValidateDevices(DEVICEMODE *pMode)
{
	HRESULT hr = S_OK;
	PADAPTER pAdapter = NULL;


	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CD3DTest::ValidateDevices:  pMode is NULL.\n" ));
		return E_INVALIDARG;
	};


	//
	// Validate members.
	//

	ASSERT( m_pD3D != NULL );
	if ( m_pD3D == NULL )
	{
		DPF( 1, _T( "CD3DTest::ValidateDevices:  m_pD3D is NULL.\n" ));
		return E_ABORT;
	};


	//
	// Get the adapter.
	//

	pAdapter = m_pD3D->GetAdapter( pMode->nAdapter );
	if ( pAdapter == NULL )
	{
		DPF(1, _T("CD3DTest::ValidateDevices:  pMode->nAdapter is invalid.\n"));
		return E_ABORT;
	};


	//
	// Let the devices validate the mode.  pMode's data might be
	// updated in the process in cases where the test has requested
	// best fit(ex. FMT_ANY for display mode).
	//

	for ( UINT i = 0; i < m_uDeviceList; i++ )
	{
		ASSERT( m_DeviceList[i] != NULL );
		hr = m_DeviceList[i]->ValidateMode(pMode);

		if ( FAILED( hr ) )
		{
			DPF( 1, _T( "CD3DTest::ValidateDevices:  ValidateMode failed.\n" ) );
			return hr;
		};
	};


	//
	// Check for front-buffer comparison on old DX versions.
	//

	if ( pMode->fVersion < 8.0f && m_Options.TestOptions.bCompareFB )
	{
		DPF(1, _T("CD3DTest::ValidateDevices:  Front buffer comparison not supported for DirectX versions below 8.\n"));
		return E_ABORT;
	};


	//
	// Check for invalid desktop bit depth.
	//

	if(pMode->pDispMode->Format.ddpfPixelFormat.dwRGBBitCount < 16 || pMode->pDispMode->Format.ddpfPixelFormat.dwRGBBitCount == 24)
	{
		DPF
		(
			1,
			_T("CD3DTest::ValidateDevices:  %d-Bit desktop not supported.\n"),
			pMode->pDispMode->Format.ddpfPixelFormat.dwRGBBitCount
		);

		return E_ABORT;
	};

	return S_OK;
};


// --------------------------------------------------------------------------------------
// This should be ONLY device creation.  Any additional operations on the devices should
// be done at a higher level since tests will overload this to create different numbers/
// types of devices and we can't rely on them keeping up to date with the fmwk's version.

HRESULT CD3DTest::CreateDevices(DEVICEMODE *pMode)
{
	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CD3DTest::CreateDevices:  pMode is NULL.\n" ) );
		return E_INVALIDARG;
	};


	//
	// Validate members.
	//

	if ( m_pD3D == NULL )
	{
		DPF( 1, _T( "CD3DTest::CreateDevices:  m_pD3D is NULL.\n" ) );
		return E_INVALIDARG;
	};


	DWORD dwBeh = pMode->dwBehaviors | (0xFFFF0000 & m_Options.D3DOptions.dwBehaviors);
	/*
	There's no need to resolve behavior flags here, since all the
	behavior flags that might need resolution are masked out above.
	if( !m_pD3D->ResolveBehaviors( pMode->nAdapter, m_Options.D3DOptions.dwDevTypes, m_Options.D3DOptions.dwBehaviors, &dwBeh ) )
	{
	DPF(1,"CD3DTest::CreateDevices:  ResolveBehaviors failed.");
	}
	*/

	// We do need to resolve ambiguous behaviors here, though, since
	// we're taking the whole thing directly from the test options.
	DWORD dwRefBeh = m_Options.D3DOptions.dwRefBehaviors;
	if( !m_pD3D->ResolveBehaviors( pMode->nAdapter, m_Options.D3DOptions.dwRefDevType, m_Options.D3DOptions.dwRefBehaviors, &dwRefBeh ) )
	{
		DPF(1,"CD3DTest::CreateDevices:  ResolveBehaviors failed.");
	};


	// Verify that no ambiguous device creation flags are left
	// after resolution.
	if ( ( dwRefBeh & DEVICECREATE_SW_OR_HW_VP ) != 0 )
	{
		DPF(1,"CD3DTest::CreateDevices:  Unresolved ambiguous device creation flag for reference device:  SW_OR_HW_VP.");
		return E_INVALIDARG;
	};

	if ( ( dwRefBeh & DEVICECREATE_HWVP_OR_PURE ) != 0 )
	{
		DPF(1,"CD3DTest::CreateDevices:  Unresolved ambiguous device creation flag for reference device:  HWVP_OR_PURE.");
		return E_INVALIDARG;
	};

	if ( ( dwRefBeh & DEVICECREATE_BESTAVAILABLE ) != 0 )
	{
		DPF(1,"CD3DTest::CreateDevices:  Unresolved ambiguous device creation flag for reference device:  BESTAVAILABLE.");
		return E_INVALIDARG;
	};


	// Verify that no combinations of conflicting device creation
	// flags are left after resolution.
	if
		(
		( dwRefBeh & DEVICECREATE_STANDARD ) != 0 &&
		( dwRefBeh & DEVICECREATE_PURE ) != 0
		)
	{
		DPF(1,"CD3DTest::CreateDevices:  Invalid combination of flags specified for reference device:  STANDARD and PURE.");
		return E_INVALIDARG;
	};

	if
		(
		( dwRefBeh & DEVICECREATE_SWVP ) != 0 &&
		( dwRefBeh & DEVICECREATE_HWVP ) != 0
		)
	{
		DPF(1, "CD3DTest::CreateDevices:  Invalid combination of flags specified for reference device:  HWVP and SWVP.");
		return E_INVALIDARG;
	};


	bool bRestore = false;
	HRESULT hr = S_OK;


	// Create the SRC device
	hr = m_pD3D->CreateDevice(pMode->nAdapter, pMode->dwDevType, m_pFramework->m_hWindow,
		dwBeh, NULL, &m_pSrcDevice);
	if(FAILED(hr))
	{
		DPF(2, _T("CD3DTest::CreateDevices:  Failed to create source device. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	else
	{
		AddDeviceToList(m_pSrcDevice);
	}


	// Create the REF device if the test uses one
	if(m_Options.D3DOptions.bReference)
	{
		if(m_Options.D3DOptions.bRefEqualsSrc)
			hr = m_pD3D->CreateDevice(pMode->nAdapter, pMode->dwDevType, m_pFramework->m_hWindow,dwBeh, NULL, &m_pRefDevice);
		else
			hr = m_pD3D->CreateDevice(pMode->nAdapter, m_Options.D3DOptions.dwRefDevType, m_pFramework->m_hWindow, dwRefBeh, NULL, &m_pRefDevice);

		if(FAILED(hr))
		{
			DPF(2, _T("CD3DTest::CreateDevices:  Failed to create reference device. hr=%s(%x)\n"),
				m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		else
		{
			AddDeviceToList(m_pRefDevice);
		}
	}


Exit:
	if(FAILED(hr))
	{
		ReleaseDevices();
	}

	return hr;
}


bool CD3DTest::IsFullscreenAvailable(int nAdapter)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if((m_DeviceList[i]->GetAdapterID() == nAdapter) &&
			m_DeviceList[i]->IsFullscreen())
			return false;
	}

	return true;
}


HRESULT CD3DTest::UpdateDevices(PRESENT_PARAMETERS *pParams)
{
	HRESULT hr = S_OK;
	UINT nParams = 0;

	if(pParams == NULL)
	{
		DPF(1, _T("CD3DTest::UpdateDevices:  pParams is NULL.\n"));
		return E_FAIL;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(m_DeviceList[i] == NULL)
		{
			DPF(1, _T("CD3DTest::UpdateDevices:  Invalid device pointer in device list.\n"));
			return E_FAIL;
		}

		// Check for multiple devices, fullscreen for the same adapter
		// If we find this case, we make the all secondary devices windowed
		if(!pParams->bWindowed && !IsFullscreenAvailable(m_DeviceList[i]->GetAdapterID()))
		{
			nParams = max(1, m_DeviceList[i]->GetCaps()->NumberOfAdaptersInGroup);

			for(UINT nParam=0; nParam < nParams; nParam++)
			{
				pParams[nParam].bWindowed = true;
				// D3DSWAPEFFECT 0x40000000 (MS internal flag)
				pParams[nParam].SwapEffect = 0x40000000;
				pParams[nParam].uFullscreenRefresh = 0;
				pParams[nParam].MultiSample = MULTISAMPLE_NONE;
				pParams[nParam].MultiSampleQuality = 0;
			}
		}

		// Set flag to indicate what device we're creating
		if( i == 0)
			pParams->dwInternalFlags |= PPINTERNAL_DEVICE0;
		else
			pParams->dwInternalFlags &= ~PPINTERNAL_DEVICE0;

		hr = m_DeviceList[i]->UpdateDevice(pParams);
		if(hr != S_OK)
		{
			DPF(hr == E_UNSUPPORTED ? 2 : 1, _T("CD3DTest::UpdateDevices:  UpdateDevice failed on device #%d.\n"), i);
			return hr;
		}
	}
	return S_OK;
}


bool CD3DTest::FilterDisplayModes(DISPLAYMODE *pModes, UINT uModes, DISPLAYMODE **pOut, UINT *uOut)
{
	DISPLAYMODE *pNew = NULL;
	UINT uNew = 0;

	//
	// Validate parameters.
	//

	if ( pModes == NULL )
	{
		DPF( 1, _T( "CD3DTest::FilterDisplayModes:  pModes is NULL.\n" ) );
		return false;
	};

	if ( pOut == NULL )
	{
		DPF( 1, _T( "CD3DTest::FilterDisplayModes:  pOut is NULL.\n" ) );
		return false;
	};

	if ( uOut == NULL )
	{
		DPF( 1, _T( "CD3DTest::FilterDisplayModes:  uOut is NULL.\n" ) );
		return false;
	};


	// Allocate the new mode list of max size
	pNew = new DISPLAYMODE[uModes];

	if(pNew == NULL)
	{
		DPF(1, _T("CD3DTest::FilterDisplayModes:  Failed to allocate display mode pNew.\n"));
		return false;
	}

	// Create the list of filtered modes
	for(UINT i=0; i<uModes; i++)
	{
		if(FilterResolution(&(pModes[i])))
			memcpy(&(pNew[uNew++]), &(pModes[i]), sizeof(DISPLAYMODE));
	}

	// If no matching modes were found, let's consider adding the current desktop mode.
	if (uNew == 0)
	{
		// Does WxHxBPP of 0x0x0 exist on the filter list?
		// If so, we'll add the current desktop mode.
		// This is a safety check to only add the current desktop mode when no other modes are available, 
		// and only for tests that opt into the default FilterResolution, or claim to support 0x0x0
		// in their overloaded version of FilterResolution.
		DISPLAYMODE displaymodeZero;
		ZeroMemory(&displaymodeZero, sizeof(DISPLAYMODE));
		if (FilterResolution(&displaymodeZero)) // Is there a 0,0,0 entry in the filter list?
		{
            
			// Add current desktop mode             
			memcpy(&(pNew[uNew++]), &(m_DeviceList[0]->GetAdapter()->DesktopMode), sizeof(DISPLAYMODE));
		}
	}

	if(uNew)
	{
		// Create the out list and copy the modes

		*pOut = new DISPLAYMODE[uNew];

		if(NULL == *pOut)
		{
			DPF(1, _T("CD3DTest::FilterDisplayModes:  Failed to allocate display mode *pOut.\n"));
			*uOut = 0;
			goto Error;
		}

		memcpy(*pOut, pNew, sizeof(DISPLAYMODE) * uNew);
	}
	*uOut = uNew;

	if (pNew)
	{
		delete [] pNew;
		pNew = NULL;
	}

	return true;

Error:
	if (pNew)
	{
		delete [] pNew;
		pNew = NULL;
	}

	return false;	
}


bool CD3DTest::FindCommonDisplayModes
(
	DISPLAYMODE *pMode0,
	UINT uMode0,
	DISPLAYMODE *pMode1,
	UINT uMode1,
	DISPLAYMODE **pOut,
	UINT *uOut
)
{
	int nMax = max(uMode0, uMode1);
	DISPLAYMODE *pNew = NULL;
	UINT uNew = 0;


	//
	// Validate parameters.
	//

	if ( pMode0 == NULL )
	{
		DPF( 1, _T( "CD3DTest::FindCommonDisplayModes:  pMode0 is NULL.\n" ) );
		return false;
	};

	if ( pMode1 == NULL )
	{
		DPF( 1, _T( "CD3DTest::FindCommonDisplayModes:  pMode1 is NULL.\n" ) );
		return false;
	};

	if ( pOut == NULL )
	{
		DPF( 1, _T( "CD3DTest::FindCommonDisplayModes:  pOut is NULL.\n" ) );
		return false;
	};

	if ( uOut == NULL )
	{
		DPF( 1, _T( "CD3DTest::FindCommonDisplayModes:  uOut is NULL.\n" ) );
		return false;
	};


	// Allocate the new mode list of max size
	pNew = new DISPLAYMODE[nMax];

	if(pNew == NULL)
	{
		DPF(1, _T("CD3DTest::FindCommonDisplayModes:  Failed to allocate display mode pNew.\n"));
		return false;
	}

	for(UINT i=0; i< uMode0; i++)
	{
		for(UINT j=0; j< uMode1; j++)
		{
			if( (pMode0[i].dwWidth == pMode1[j].dwWidth) &&
				(pMode0[i].dwHeight == pMode1[j].dwHeight) &&
				(pMode0[i].dwBPP == pMode1[j].dwBPP) &&
				m_pD3D->CompareFormats(&(pMode0[i].Format), &(pMode1[j].Format)))
			{
				memcpy(&(pNew[uNew++]), &(pMode0[i]), sizeof(DISPLAYMODE));
				break;
			}
		}
	}

	if(uNew)
	{
		// Create the out list and copy the modes

		*pOut = new DISPLAYMODE[uNew];

		if(NULL == *pOut)
		{
			DPF(1, _T("CD3DTest::FindCommonDisplayModes:  Failed to allocated display mode pOut.\n"));
			*uOut = 0;

			if (NULL != pNew)
			{
				delete [] pNew;
				pNew = NULL;
			}

			return false;
		}

		memcpy(*pOut, pNew, sizeof(DISPLAYMODE) * uNew);
	}

	*uOut = uNew;

	if (pNew)
	{
		delete [] pNew;
		pNew = NULL;
	}

	return true;
}


void CD3DTest::ReleaseCommonDisplayModes()
{
	SAFEDELETEA(m_pCommonDisplayModes);
	m_uCommonDisplayModes = 0;
}


bool CD3DTest::EnumCommonDisplayModes(CDevice** pDevList, UINT uDevices)
{
	DISPLAYMODE *pMode = NULL;
	DISPLAYMODE *pOut = NULL;
	UINT uModes = 0;
	UINT uOut = 0;


	//
	// Validate parameters.
	//

	if ( pDevList == NULL )
	{
		DPF( 1, _T( "CD3DTest::EnumCommonDisplayModes:  pDevList is NULL.\n" ) );
		return false;
	};

	if ( pDevList[ 0 ] == NULL )
	{
		DPF( 1, _T( "CD3DTest::EnumCommonDisplayModes:  pDevList[0] is NULL.\n" ) );
		return false;
	};

	if ( uDevices == 0 )
	{
		DPF( 1, _T( "CD3DTest::EnumCommonDisplayModes:  uDevices is 0.\n" ) );
		return false;
	};


	// Release any existing formats
	ReleaseCommonDisplayModes();

	// Filter out uninteresting display modes
	if(!FilterDisplayModes(pDevList[0]->GetDisplayModes(), pDevList[0]->GetNumDisplayModes(), &m_pCommonDisplayModes, &m_uCommonDisplayModes))
	{
		DPF(1, _T("CD3DTest::EnumCommonDisplayModes:  FilterDisplayModes failed.\n"));
		return false;
	}

	for(UINT i=1; i<uDevices; i++)
	{	
		CDevice *pSrc = pDevList[i];
		pMode= NULL;
		uModes = 0;

		// Filter out these modes too
		if(!FilterDisplayModes(pSrc->GetDisplayModes(), pSrc->GetNumDisplayModes(), &pMode, &uModes))
		{
			DPF(1, _T("CD3DTest::EnumCommonDisplayModes:  FilterDisplayModes failed.\n"));
			return false;
		}


		if(!FindCommonDisplayModes(pMode, uModes, m_pCommonDisplayModes, m_uCommonDisplayModes, &pOut, &uOut))
		{
			DPF(1, _T("CD3DTest::EnumCommonDisplayModes:  Failed to find common display modes.\n"));
			return false;
		}

		// Delete the old lists
		SAFEDELETE(m_pCommonDisplayModes);
		SAFEDELETE(pMode);

		// Save the new format lists
		m_pCommonDisplayModes = pOut;
		m_uCommonDisplayModes = uOut;
	}

	return true;
}


HRESULT CD3DTest::SetupDeviceMode(DEVICEMODE *pMode)
{
	//
	// Validate parameters.
	//

	if ( pMode == NULL )
	{
		DPF( 1, _T( "CD3DTest::SetupDeviceMode:  pMode is NULL.\n" ) );
		return E_INVALIDARG;
	};


	HRESULT hr = S_OK;

	// Release any existing D3D object/modes
	ReleaseDevMode();

	// Verify that our adapter is valid
	if (m_pD3D->GetAdapter(pMode->nAdapter) == NULL)	
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  GetAdapter failed.\n"));
		return E_FAIL;
	}

	// Set the requested display mode for this device
	memcpy(&m_pD3D->GetAdapter(pMode->nAdapter)->DesktopMode, pMode->pDispMode, sizeof(DISPLAYMODE));

	// Create the devices for this mode
	hr = CreateDevices(pMode);

	if(FAILED(hr))
	{
		DPF(2, _T("CD3DTest::SetupDeviceMode  CreateDevices failed.\n"));
		return hr;
	}

	// FMT_ANY gets decoded in CreateDevices() and stored in the DesktopMode fmt.  At this point,
	// we need to update the pMode->pDisplayMode so that the test knows the actual display mode
	// we're going to run in.

	if((FMT)pMode->pDispMode->Format.d3dfFormat == FMT_ANY)
	{
		memcpy(&pMode->pDispMode->Format, &m_pD3D->GetAdapter(pMode->nAdapter)->DesktopMode.Format, sizeof(FORMAT));
	}

	// Set additional device data

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		m_DeviceList[i]->SetPSGPRegChange(m_DeviceList[i] != m_pRefDevice ? pMode->dwPSGPRegChange :
			m_Options.D3DOptions.dwRefPSGPRegChange);
	m_DeviceList[i]->SetVPEmuRegChange(m_DeviceList[i] != m_pRefDevice ? m_Options.D3DOptions.dwVPEmuRegChange :
		m_Options.D3DOptions.dwRefVPEmuRegChange);
	m_DeviceList[i]->SetPPEmuRegChange(m_DeviceList[i] != m_pRefDevice ? m_Options.D3DOptions.dwPPEmuRegChange :
		m_Options.D3DOptions.dwRefPPEmuRegChange);
	}

	// Validate this device mode against the caps
	hr = ValidateDevices(pMode);

	if(FAILED(hr))
	{
		DPF(2, _T("CD3DTest::SetupDeviceMode:  ValidateDevices failed.\n"));
		return hr;		
	}

	// Generate the list of common formats

	if(!EnumCommonFormats(m_DeviceList, m_uDeviceList))
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  EnumCommonFormats failed.\n"));
		return E_FAIL;
	}

	// Filter those modes if requested
	if(!FilterCommonFormats())
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  FilterCommonFormats failed.\n"));
		return E_FAIL;
	}

	// Generate the list of common formats
	if(!EnumCommonDisplayModes(m_DeviceList, m_uDeviceList))
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  EnumCommonDisplayModes failed.\n"));
		return E_FAIL;
	}

	// Construct the test's mode list
	if(!ConstructModeList(pMode))
	{
		DPF(2, _T("CD3DTest::SetupDeviceMode:  ConstructModeList failed.\n"));
		return E_UNSUPPORTED;
	}

	// Initialize the devices specifc vars
	if(!m_StateBlockHandles.Setup(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  Failed to setup state block handles.\n"));
		return E_FAIL;
	}

	if(!m_VertexShaderHandles.Setup(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  Failed to setup vertex shader handles.\n"));
		return E_FAIL;
	}

	if(!m_PixelShaderHandles.Setup(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest::SetupDeviceMode:  Failed to setup pixel shader handles.\n"));
		return E_FAIL;
	}		

	return S_OK;
}


bool CD3DTest::AddMode(DISPLAYMODE *pDisplay, bool bWindowed, int nTexture, int nZBuffer, int nCube, int nVolume, int nMS, int nMSQ, int nBB)
{
	TESTMODE **pMode = &m_pModeList, *pPrevious = NULL;
	PDISPLAYMODE pNewDisplay = NULL;

	// Find the tail
	while(*pMode)
	{
		pPrevious = *pMode;
		pMode = &((*pMode)->pNext);
	}

	// Alloc the new test mode
	*pMode = new TESTMODE;
	pNewDisplay = new DISPLAYMODE;
	if(*pMode == NULL || NULL == pNewDisplay)
	{
		SAFEDELETE( (*pMode) );
		SAFEDELETE(pNewDisplay);
		DPF(1, _T("CD3DTest::AddTestMode:  Out of memory.\n"));
		return false;
	}
	memset(*pMode, 0, sizeof(TESTMODE));
	memcpy(pNewDisplay, pDisplay, sizeof(DISPLAYMODE));
	d3df2ddpf(pNewDisplay->Format.d3dfFormat, &pNewDisplay->Format.ddpfPixelFormat);

	(*pMode)->pDisplay			= pNewDisplay;
	(*pMode)->nBackBufferWidth	= bWindowed ? m_Options.D3DOptions.nBackBufferWidth : pNewDisplay->dwWidth;
	(*pMode)->nBackBufferHeight	= bWindowed ? m_Options.D3DOptions.nBackBufferHeight : pNewDisplay->dwHeight;
	(*pMode)->bWindowed 		= bWindowed;
	(*pMode)->nTextureFormat 	= nTexture;
	(*pMode)->nZBufferFormat 	= nZBuffer;
	(*pMode)->nCubeFormat 		= nCube;
	(*pMode)->nVolumeFormat 	= nVolume;
	(*pMode)->nMultisample 		= nMS;
	(*pMode)->nMltsmpQuality	= nMSQ;
	(*pMode)->nBackBufferFormat	= nBB;
	(*pMode)->pPrevious			= pPrevious;

	m_uTotalModes++;

	return true;
}


bool CD3DTest::GetFormatIndex(FMT fmt, FORMAT *pList, UINT *pIndex)
{
	FORMAT Format(fmt);
	FORMAT *pIter = pList;
	UINT uCount = 0;

	if(NULL == pIndex)
	{
		DPF(1, _T("CD3DTest::GetFormatIndex:  pIndex is NULL.\n"));
		return false;
	}

	if(NULL == pList || fmt == FMT_ANY || fmt == FMT_ALL)
	{
		*pIndex = 0;
		return true;
	}

	// Walk the input list
	while(pIter)
	{
		// If we have D3DFORMATS(DX8 and up), use them to compare.  Otherwise look at the bits.
		if(0 != Format.d3dfFormat && 0 != pIter->d3dfFormat)
		{
			if(Format.d3dfFormat == pIter->d3dfFormat)
			{
				*pIndex = uCount;
				return true;
			}
		}
		// else check the pixel format for a match
		else if(memcmp(&Format.ddpfPixelFormat, &(pIter->ddpfPixelFormat), sizeof(PIXELFORMAT)) == 0)
		{
			*pIndex = uCount;
			return true;
		}

		uCount++;
		pIter = pIter->pNext;
	}

	return false;
}


bool CD3DTest::GetZBufferFormatIndex(FMT fmt, FORMAT *pList, FORMAT *pRenderTarget, UINT *pIndex)
{
	FORMAT Format(fmt);
	FORMAT *pIter = pList;
	UINT uCount = 0;


	//
	// Validate parameters.
	//

	if ( pIndex == NULL )
	{
		DPF( 1, _T( "CD3DTest::GetZBufferFormatIndex:  pIndex is NULL.\n" ) );
		return false;
	};

	if ( pRenderTarget == NULL )
	{
		DPF( 1, _T("CD3DTest::GetZBufferFormatIndex:  pRenderTarget is NULL.\n" ) );
		return false;
	};


	if(NULL == pList)
	{
		*pIndex = 0;
		return true;
	}

	// Walk the input list
	while(pIter)
	{
		// if FMT_ANY, we want to match the RenderTarget bit count to the Z if possible
		if(fmt == FMT_ANY || fmt == FMT_ALL)
		{
			if(pIter->ddpfPixelFormat.dwZBufferBitDepth == pRenderTarget->ddpfPixelFormat.dwRGBBitCount)
			{
				*pIndex = uCount;
				return true;
			}
		}
		// If we have D3DFORMATS(DX8 and up), use them to compare.  Otherwise look at the bits.
		else if(0 != Format.d3dfFormat && 0 != pIter->d3dfFormat)
		{
			if(Format.d3dfFormat == pIter->d3dfFormat)
			{
				*pIndex = uCount;
				return true;
			}
		}
		// We want to match Z15S1 to S1Z15 as well as to each other
		else if((Format.ddpfPixelFormat.dwZBufferBitDepth - Format.ddpfPixelFormat.dwStencilBitDepth) == 
			(pIter->ddpfPixelFormat.dwZBufferBitDepth - pIter->ddpfPixelFormat.dwStencilBitDepth) &&
			(Format.ddpfPixelFormat.dwStencilBitDepth == pIter->ddpfPixelFormat.dwStencilBitDepth) &&
			(Format.ddpfPixelFormat.dwFlags == pIter->ddpfPixelFormat.dwFlags) &&
			pRenderTarget)
		{
			*pIndex = uCount;
			return true;
		}

		uCount++;
		pIter = pIter->pNext;
	}

	// If we failed to find a perfect match for FMT_ANT, try the first one anyway
	if(fmt == FMT_ANY || fmt == FMT_ALL)
	{
		*pIndex = 0;
		return true;
	}

	return false;
}


bool CD3DTest::GetBackBufferFormatIndex(FMT fmt, FORMAT *pList, FORMAT *pRenderTarget, UINT *pIndex)
{
	FORMAT Format(fmt);
	FORMAT *pIter = pList;
	UINT uCount = 0, uClosest = 0;


	//
	// Validate parameters.
	//

	if ( NULL == pIndex )
	{
		DPF( 1, _T( "CD3DTest::GetBackBufferFormatIndex:  pIndex is NULL.\n" ) );
		return false;
	};

	if(NULL == pList)
	{
		*pIndex = 0;
		return true;
	};


	// Walk the input list
	while(pIter)
	{
		// if FMT_ANY, we want to match the RenderTarget bit count to the Z if possible
		if(fmt == FMT_ANY || fmt == FMT_ALL)
		{
			if(NULL == pRenderTarget)
			{
				DPF(1, _T("CD3DTest::GetBackBufferFormatIndex:  pRenderTarget is NULL.\n"));
				return false;
			}

			if(pIter->d3dfFormat == pRenderTarget->d3dfFormat)
			{
				*pIndex = uCount;
				return true;
			}
			// If we can't find an exact match, at least get one that's the same bitdepth
			if(pIter->ddpfPixelFormat.dwRGBBitCount == 
				pRenderTarget->ddpfPixelFormat.dwRGBBitCount)
			{
				uClosest = uCount;
			}
		}
		else
		{
			if(Format.d3dfFormat == pIter->d3dfFormat)
			{
				*pIndex = uCount;
				return true;
			}
		}

		uCount++;
		pIter = pIter->pNext;
	}

	// If we failed to find a perfect match for FMT_ANY, try the first one anyway
	if(fmt == FMT_ANY || fmt == FMT_ALL)
	{
		*pIndex = uClosest;
		return true;
	}

	return false;
}


bool CD3DTest::GetFormatIndex(MULTISAMPLE_TYPE MultiSamp, MULTISAMPLE_TYPE *pList, UINT uMS, UINT *pIndex)
{
	//
	// Validate parameters.
	//

	if(NULL == pIndex)
	{
		DPF(1, _T("CD3DTest::GetFormatIndex:  pIndex is NULL.\n"));
		return false;
	}

	if(NULL == pList || MultiSamp == MULTISAMPLE_ALL || MultiSamp == MULTISAMPLE_ANY)
	{
		*pIndex = 0;
		return true;
	}

	// Walk the input list

	for(UINT i=0; i<uMS; i++)
	{
		// Check if this fmt is a match

		if(MultiSamp == pList[i])
		{
			*pIndex = i;
			return true;
		}
	}

	return false;
}


bool CD3DTest::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
	UINT uTexture = 0;
	UINT uZBuffer = 0;
	UINT uCube = 0;
	UINT uVolume = 0;
	UINT uMS = 0;
	UINT uBB = 0;

	// Convert the formats into indexes into the common lists
	if(	!GetFormatIndex(m_Options.ModeOptions.TextureFmt,	m_pCommonTextureFormats, &uTexture) ||
		!GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
		!GetFormatIndex(m_Options.ModeOptions.CubeFmt,		m_pCommonCubeFormats,	&uCube) ||
		!GetFormatIndex(m_Options.ModeOptions.VolumeFmt,	m_pCommonVolumeFormats, &uVolume) ||
		!GetFormatIndex(m_Options.ModeOptions.MultisampType,m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
		!GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt,m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
		// We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
		(m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
		(FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
		FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))) )
	{
		DPF(2, _T("Requested fmt not supported by device.\n"));
		return false;
	}

	// If only one mode was requested, add it and return
	if( m_Options.ModeOptions.TextureFmt != FMT_ALL && 
		m_Options.ModeOptions.ZBufferFmt != FMT_ALL && 
		m_Options.ModeOptions.CubeFmt != FMT_ALL && 
		m_Options.ModeOptions.VolumeFmt != FMT_ALL && 
		m_Options.ModeOptions.BackBufferFmt != FMT_ALL && 
		m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
	{
		return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
	}

	if(m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
	{
		for(UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
		{
			// Filter out unsupported BB modes based on ModeOptions
			if( m_pD3D->GetVersion() < 8.f)
			{
				if ( pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat) 
					continue;
			}
			else if (FAILED (m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, 
				m_pCommonBackBufferFormats[nBB], bWindowed)))
				continue;

			if(!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
			{
				DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
				return false;
			}
		}
	}

	if(m_Options.ModeOptions.TextureFmt == FMT_ALL)
	{
		for(UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
		{
			if(!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
			{
				DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
				return false;
			}
		}
	}

	if(m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
	{
		for(UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
		{
			if(!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
			{
				DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
				return false;
			}
		}
	}

	if(m_Options.ModeOptions.CubeFmt == FMT_ALL)
	{
		for(UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
		{
			if(!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
			{
				DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
				return false;
			}
		}
	}

	if(m_Options.ModeOptions.VolumeFmt == FMT_ALL)
	{
		for(UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
		{
			if(!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
			{
				DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
				return false;
			}
		}
	}

	if(m_Options.ModeOptions.MultisampType == FMT_ALL)
	{
		DWORD dwMSQuality = 0;

		for(UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
		{
			int nValidBB = -1, nValidZBuffer = -1;

			// For each MS type, find a BB and ZBuffer that support that type
			for(UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
				for(UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
				{
					// If specific modes are requested, only allow those modes to be matched.
					if(m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
						nBB != uBB)
						continue;
					if(m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
						nZBuffer != uZBuffer)
						continue;

					// Filter out unsupported BB modes based on ModeOptions
					if(SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
						SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
						SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
					{
						nValidBB = nBB;
						nValidZBuffer = nZBuffer;
						break;
					}
				}

				if(nValidBB == -1 || nValidZBuffer == -1)
					continue;

				for(UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
				{
					if(!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
					{
						DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
						return false;
					}
				}
		}
	}

	return true;
}


//We override this function, to make sure that the CheckDeviceType calls are called after 
//the devices are created, and before the mode list is created.
bool CD3DTest::ConstructModeList(DEVICEMODE *pMode)
{
	bool bRes = true;
	DISPLAYMODE Mode;

	if(NULL == pMode)
	{
		DPF(1, _T("CD3DTest::ConstructModeList:  pMode is NULL.\n"));
		return false;
	}

	if(m_pModeList)
	{
		DPF(1, _T("CD3DTest::ConstructModeList:  Deleting pre-existing modelist.\n"));
		ReleaseModeList();
	}

	// Fill in the defaults
	memcpy(&Mode, pMode->pDispMode, sizeof(DISPLAYMODE));
	Mode.dwBPP = pMode->pDispMode->Format.ddpfPixelFormat.dwRGBBitCount;

	// Do all the windowed modes
	if(m_Options.D3DOptions.dwWinMode & WINMODE_WINDOWED)
	{
		bRes = AddFormatModes(&Mode, true);	
		if(!bRes)
		{
			DPF(2, _T("CD3DTest::ConstructModeList:  AddFormatModes failed (%dx%dx%d Windowed).\n"),
				Mode.dwWidth, Mode.dwHeight, Mode.dwBPP);
			return false;
		}
	}

	// Do all the fullscreen modes
	if(m_Options.D3DOptions.dwWinMode & WINMODE_FULLSCREEN)
	{
		// If a mode was requested from the cmdline, execute only that mode
		if(m_bCmdLineMode)
		{
			// If mode fmt not given, default to desktop
			if(FMT_ANY == (FMT)Mode.Format.d3dfFormat || FMT_ALL == (FMT)Mode.Format.d3dfFormat)
			{
				memcpy(&Mode.Format, &m_pSrcDevice->GetAdapter()->DesktopMode.Format, sizeof(FORMAT));
				Mode.dwBPP = Mode.Format.ddpfPixelFormat.dwRGBBitCount;
			}

			if(!AddFormatModes(&Mode, false))
			{			
				DPF(2, _T("CD3DTest::ConstructModeList:  AddFormatModes failed (%dx%dx%d Fullscreen).\n"),
					Mode.dwWidth, Mode.dwHeight, Mode.dwBPP);
				return false;
			}
		}
		else
		{
			// Run through all modes requested by the test
			for(UINT nDisplayModes = 0; nDisplayModes < m_Options.ModeOptions.uMaxDisplayModes; nDisplayModes++)
			{
				if(nDisplayModes >= m_uCommonDisplayModes)
					break;

				if(!(bRes = AddFormatModes(&(m_pCommonDisplayModes[nDisplayModes]), false)))
				{
					DPF(2, _T("CD3DTest::ConstructModeList:  AddFormatModes failed (%dx%dx%d Fullscreen).\n"),
						m_pCommonDisplayModes[nDisplayModes].dwWidth, m_pCommonDisplayModes[nDisplayModes].dwHeight,
						m_pCommonDisplayModes[nDisplayModes].dwBPP);
					return false;
				}
			}
		}
	}

	if(m_pModeList == NULL)
	{
		DPF(1, _T("Device does not support the requested behaviors\\capabilities.\n"));
		return false;
	}
	else
	{
		m_pCurrentMode = m_pModeList;
		m_uCurrentMode = 1;
		return true;
	}
}


void CD3DTest::SceneRefresh(void)
{
}


bool CD3DTest::PreModeSetup(void)
{
	return true;
}


bool CD3DTest::PostModeSetup(void)
{
	return true;
}


bool CD3DTest::ExecuteTest(UINT uTestNumber)
{
	return true;
}


void CD3DTest::WriteStatus(const TCHAR *ptcsKey, const TCHAR *ptcsFmt,...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	m_pFramework->m_pStatWindow->WriteStatusWhole(1, ptcsKey, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteStatusBar(int nField, const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	m_pFramework->WriteStatusBar(nField, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteStatus(int nLevel, const TCHAR *ptcsKey, const TCHAR *ptcsFmt,...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	m_pFramework->m_pStatWindow->WriteStatusWhole(nLevel, ptcsKey, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteToLog(UINT uLogLevel, const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	// write to the framework
	m_pFramework->WriteStringToLog(uLogLevel, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteMessage(UINT uLogLevel, const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	// write to the framework
	m_pFramework->WriteMessage(uLogLevel, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteDetails(UINT uLogLevel, const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	// write to the framework
	m_pFramework->WriteDetails(uLogLevel, sTemp);

	SAFEDELETEA(sTemp);
}


void CD3DTest::WriteToLog(const TCHAR *ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	// write to the framework
	m_pFramework->WriteStringToLog(1, sTemp);

	SAFEDELETEA(sTemp);
}


bool CD3DTest::ProcessMessages(void)
{
	return m_pFramework->ProcessMessages();
}


void CD3DTest::Pass(void)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_PASS);
	m_pFramework->SuggestExitCode(EXITCODE_SUCCESS);
}


UINT CD3DTest::GetPass()
{
	return m_pFramework->m_DXGLog.GetDevicePass();
}


void CD3DTest::Fail(void)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_FAIL);

	// We no longer set the EXITCODE for failures because we run lots 
	// of modes, and the exitcode gets applied to all of in the DCT.
}


UINT CD3DTest::GetFail()
{
	return m_pFramework->m_DXGLog.GetDeviceFail();
}


void CD3DTest::Abort(void)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_BLOCKED);

	// We no longer set the EXITCODE for failures because we run lots 
	// of modes, and the exitcode gets applied to all of in the DCT.
}


UINT CD3DTest::GetAbort()
{
	return m_pFramework->m_DXGLog.GetDeviceAbort();
}


void CD3DTest::Skip(void)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_SKIPPED);
	m_pFramework->SuggestExitCode(EXITCODE_SUCCESS);
}


UINT CD3DTest::GetSkip()
{
	return m_pFramework->m_DXGLog.GetDeviceSkip();
}


void CD3DTest::Warn(void)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_WARN);
	m_pFramework->SuggestExitCode(EXITCODE_SUCCESS);
}


UINT CD3DTest::GetWarn()
{
	return m_pFramework->m_DXGLog.GetDeviceWarn();
}


void CD3DTest::SetResult(DWORD Result)
{
	m_pFramework->m_DXGLog.SetTestCaseResult(Result);
}


void CD3DTest::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
	m_pFramework->m_DXGLog.BeginTestCase(szTestCase, uTestNumber);
}


void CD3DTest::EndTestCase()
{
	// Log test case results

	if ( m_pFramework->m_DXGLog.InTestCase() )
	{
		DWORD TestCaseResult = m_pFramework->m_DXGLog.GetCurrentResult();

		switch ( TestCaseResult )
		{
			case WTT_TESTCASE_RESULT_PASS:
			case WTT_TESTCASE_RESULT_FAIL:
			{
				m_pFramework->m_DXGLog.EndTestCase( m_fPassPercentage );
			} break;

			default:
			{
				m_pFramework->m_DXGLog.EndTestCase();
			} break;
		}
	}
}


void CD3DTest::ReleaseObjects()
{
	// Release the swapchain
	RELEASE(m_pCmpSwapChain);
	RELEASE(m_pCmpTexture);
}


HRESULT CD3DTest::CreateObjects()
{
	HRESULT hr = S_OK;

	// Create the ImageCompare if needed	
	if(m_Options.TestOptions.bCompareSrcRef && m_Options.D3DOptions.bReference)
		hr = CreateImageCompare(&m_pCmpSwapChain);

	return hr;
}


void CD3DTest::ReleaseDevices()
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(m_DeviceList[i])
		{
			m_DeviceList[i]->ReleaseD3D();			// Release all alloc'd D3D object
			RELEASE(m_DeviceList[i]);				// Release the wrapper
		}
	}

	m_uDeviceList = 0;
	m_pSrcDevice = NULL;
	m_pRefDevice = NULL;

	// Older D3D versions require closer D3D->Device coupling, so we must release here.
	if(m_pD3D && m_pD3D->GetVersion() < 8.f)
		m_pD3D->ReleaseD3D();
}


bool CD3DTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, 0x00000032, 1.0f, 0);
}


void CD3DTest::ConstructTestCaption(__out_ecount(nText) LPTSTR szText, int nText)
{
	TCHAR       szFormat[MAX_PATH];
	TCHAR       szBuffer[MAX_PATH];
	TESTMODE *  pMode = m_pCurrentMode;


	// Start the test group name.
	StringCchCopy(szText, nText, m_szTestName);


	// Tack on the resolution.
	if(m_Options.ModeOptions.uMaxDisplayModes > 1)
	{
		StringCchPrintf
		(
			szBuffer,
			ARRAYSIZE(szBuffer),
			_T(", %dx%dx%d@%dHz"),
			pMode->pDisplay->dwWidth,
			pMode->pDisplay->dwHeight,
			pMode->pDisplay->dwBPP,
			pMode->pDisplay->dwRefreshRate
		);

		StringCchCat(szText, nText, szBuffer);
	};


	// Tack on the back buffer format.
	if(m_Options.ModeOptions.BackBufferFmt != FMT_ANY && m_uCommonBackBufferFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonBackBufferFormats[pMode->nBackBufferFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", BackBuf: %s"), szFormat);

		StringCchCat(szText, nText, szBuffer);
	}


	// Tack on the texture format.
	if(m_Options.ModeOptions.TextureFmt != FMT_ANY && m_uCommonTextureFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonTextureFormats[pMode->nTextureFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", Texture: %s"), szFormat);

		StringCchCat(szText, nText, szBuffer);
	}


	// Tack on the z buffer format.
	if(m_Options.ModeOptions.ZBufferFmt != FMT_ANY && m_uCommonZBufferFormats)
	{
		FmtToString(FORMAT_ZBUFFER, &m_pCommonZBufferFormats[pMode->nZBufferFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", Z: %s"), szFormat);

		StringCchCat(szText, nText, szBuffer);
	}


	// Tack on the cube format.
	if(m_Options.ModeOptions.CubeFmt != FMT_ANY && m_uCommonCubeFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonCubeFormats[pMode->nCubeFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", Cube: %s"), szFormat);

		StringCchCat(szText, nText, szBuffer);
	}


	// Tack on the volume format.
	if(m_Options.ModeOptions.VolumeFmt != FMT_ANY && m_uCommonVolumeFormats)
	{
		FmtToString(FORMAT_TEXTURE, &m_pCommonVolumeFormats[pMode->nVolumeFormat], szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", Volume: %s"), szFormat);

		StringCchCat(szText, nText, szBuffer);
	}


	// Tack on the multisample type.
	if(m_Options.ModeOptions.MultisampType != MULTISAMPLE_ANY && m_uCommonMultiSampleFormats)
	{
		FmtToString(FORMAT_MULTISAMPLE, (FORMAT*)(&m_pCommonMultiSampleFormats[pMode->nMultisample]), szFormat);
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T(", MultiSamp: %s (Qual = %d)"), szFormat, pMode->nMltsmpQuality);

		StringCchCat(szText, nText, szBuffer);
	}
}


bool CD3DTest::DisplayFrame()
{
	HRESULT hr;
	bool    bRes = true;

	if(m_pCurrentMode == NULL)
		return false;

	if(0 == m_uDeviceList)
		return true;

	// Full screen drawing

	if(!m_pCurrentMode->bWindowed)
	{		
		// Present all fullscreen devices

		for(UINT nDevices=0; nDevices < m_uDeviceList; nDevices++)
		{
			if(m_DeviceList[nDevices]->IsFullscreen())
			{
				hr = m_DeviceList[nDevices]->Present(NULL, NULL, NULL, NULL);	

				// On DX 9.L allow Present to return out of video
				// memory without failing the test.  This is
				// because in WDDM we don't commit all of the
				// memory for resources used in the scene
				// until Present, at which point we can discover
				// that there isn't enough video memory to do the
				// Present.  We can't fail a driver for this.
				if (m_pD3D->GetVersion() >= 9.1f && hr == D3DERR_OUTOFVIDEOMEMORY)
				{
					// Clear out the error and issue a skip
					GetLastError();
					hr = S_OK;
					Skip();
				}
				else if(FAILED(hr))
				{
					DPF(1, _T("CD3DTest::DisplayFrame:  Present failed (fullscreen). hr=%s(%x).\n"), 
						m_pD3D->HResultToString(hr), hr);
					bRes = false;
				}
			}
		}
	}
	else
	{
		int nDevTextHeight = 17;
		int nDevWidth = m_DeviceList[0]->GetPresParams() == NULL ? 0 : m_DeviceList[0]->GetPresParams()->uWidth;
		int nDevHeight = m_DeviceList[0]->GetPresParams() == NULL ? 0 : m_DeviceList[0]->GetPresParams()->uHeight;
		UINT nWindowWidth = m_pFramework->m_pDisplayWindow->m_nWidth/(m_uDeviceList + ((m_bShowCompareWindow && m_pCmpSwapChain) ? 1:0));
		UINT nWindowHeight = m_pFramework->m_pDisplayWindow->m_nHeight - m_nTagHeight;

		RECT rRect;

		// Cap the min at zero

		if(nDevWidth < 0)
			nDevWidth = 0;

		if(nDevHeight < 0)
			nDevHeight = 0;

		HDC hDC = GetDC(m_pFramework->m_pDisplayWindow->m_hWindow);
		HGDIOBJ hGDIObj = NULL;

		if(hDC == NULL)
		{
			DPF(1, _T("CD3DTest::DisplayFrame:  GetDC failed.\n"));
			return false;
		}

		InvalidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, NULL, true);

		for(UINT nDevices=0; nDevices < m_uDeviceList; nDevices++)
		{
			TCHAR tcsBuffer[256] = "\0";
			int nOffset = nWindowWidth*nDevices;
			UINT nBlue = 128;
			UINT nGreen = (UINT) (128.0f * ((float)nDevices / (float)m_uDeviceList));

			switch(m_DeviceList[nDevices]->GetDevType())
			{
				case DEVICETYPE_HAL:
					StringCchPrintf
					(
						tcsBuffer,
						ARRAYSIZE(tcsBuffer),
						_T("%s(%s)"),
						m_DeviceList[nDevices]->GetAdapter()->sAdapterName,
						m_DeviceList[nDevices]->GetAdapter()->sVersion);
				break;

				case DEVICETYPE_REF:
					StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("Microsoft Reference Driver"));
				break;

				case DEVICETYPE_NULLREF:
					StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("Microsoft Null Reference Driver"));
				break;

				default:
				break;
			}

			// Draw the device tag

			rRect.left = nOffset; rRect.top = 0; rRect.right = nOffset + nWindowWidth; rRect.bottom = nDevTextHeight;
			hGDIObj = SelectObject(hDC, m_pFramework->GetLargeFont());
			SetBkColor(hDC,RGB(0,nGreen,nBlue));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
			ExtTextOut(hDC,10 + nOffset,1,ETO_OPAQUE | ETO_CLIPPED,&rRect,m_DeviceList[nDevices]->GetDescription(),_tcslen(m_DeviceList[nDevices]->GetDescription()),NULL);
			ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rRect);

			//Make sure we deselect the font so that it won't leak resources on win9x

			SelectObject(hDC, hGDIObj);

			// Draw the adapter tag

			int nDriverHeight = m_nTagHeight - nDevTextHeight;

			rRect.left = nOffset; rRect.top = nDevTextHeight; rRect.right = nOffset + nWindowWidth; rRect.bottom = nDriverHeight + nDevTextHeight;
			hGDIObj = SelectObject(hDC, m_pFramework->GetSmallFont());
			SetBkColor(hDC,RGB(0,nGreen,nBlue));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
			ExtTextOut(hDC,10 + nOffset,nDevTextHeight+1,ETO_OPAQUE |ETO_CLIPPED, &rRect,tcsBuffer,_tcslen(tcsBuffer),NULL);
			ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rRect);

			//Make sure we deselect the font so that it won't leak resources on win9x

			SelectObject(hDC, hGDIObj);

			// Present the devices - if the src is smaller than the minimum DST rect, we'll do a stretch

			RECT rSrc = {0, 0, nDevWidth, nDevHeight};
			RECT rDst = {   nOffset,
				m_nTagHeight, 
				nOffset + (m_bStretchPresent  ? nWindowWidth : __min(m_Options.D3DOptions.nBackBufferWidth, nWindowWidth)) , 
				m_nTagHeight + (m_bStretchPresent ? nWindowHeight : __min(m_Options.D3DOptions.nBackBufferHeight, nWindowHeight))};

			if(rDst.right - rDst.left > 0 && rDst.bottom - rDst.top > 0)
			{
				ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rDst);

				hr = m_DeviceList[nDevices]->Present(&rSrc, &rDst, NULL, NULL);

				// On DX 9.L allow Present to return out of video
				// memory without failing the test.  This is
				// because in WDDM we don't commit all of the
				// memory for resources used in the scene
				// until Present, at which point we can discover
				// that there isn't enough video memory to do the
				// Present.  We can't fail a driver for this.
				if (m_pD3D->GetVersion() >= 9.1f && hr == D3DERR_OUTOFVIDEOMEMORY)
				{
					// Clear out the error and issue a skip
					GetLastError();
					hr = S_OK;
					Skip();
				}
				// Under WHQL, allow Present to return surface loss
				// without failing the test.  This is because the
				// DWM can be doing things in the background that
				// cause surface loss, and there is no way for the
				// test to prevent this or even to know when to
				// expect it.  See Windows OS Bugs bug 1962606 for
				// details.
				else if (hr == DDERR_SURFACELOST && KeySet(_T("whql")))
				{
					// Reset devices.
					ResetLostDevices();

					// Clear out the error and issue a skip
					GetLastError();
					hr = S_OK;
					Skip();
				}
				else if(FAILED(hr))
				{
					DPF(1, _T("CD3DTest::DisplayFrame:  Present failed (windowed). hr=%s(%x).\n"), 
						m_pD3D->HResultToString(hr), hr);
					bRes = false;
				}
			}
		}

		// Show the Compare Window
		if (m_bShowCompareWindow && m_pCmpSwapChain)
		{
			TCHAR * pcsDesc = _T("Image Compare\0");
			int     nOffset = nWindowWidth * m_uDeviceList;

			rRect.left = nOffset; rRect.top = 0; rRect.right = nOffset + nWindowWidth; rRect.bottom = nDevTextHeight;
			hGDIObj = SelectObject(hDC, m_pFramework->GetLargeFont());
			SetBkColor(hDC,RGB(128,0,0));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
			ExtTextOut(hDC,10 + nOffset,1,ETO_OPAQUE | ETO_CLIPPED,&rRect,pcsDesc,_tcslen(pcsDesc),NULL);
			ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rRect);

			//Make sure we deselect the font so that it won't leak resources on win9x

			SelectObject(hDC, hGDIObj);

			pcsDesc = m_pFramework->GetImageCmp()->GetCmpImgModeStr();

			int nDriverHeight = m_nTagHeight - nDevTextHeight;

			rRect.left = nOffset; rRect.top = nDevTextHeight; rRect.right = nOffset + nWindowWidth; rRect.bottom = nDriverHeight + nDevTextHeight;
			hGDIObj = SelectObject(hDC, m_pFramework->GetSmallFont());
			SetBkColor(hDC,RGB(128,0,0));
			SetTextColor(hDC,RGB(255,255,0));

#pragma prefast(suppress: 528, "SHExtTextOut does not appear in publics.")
			ExtTextOut(hDC,10 + nOffset,nDevTextHeight+1,ETO_OPAQUE |ETO_CLIPPED, &rRect,pcsDesc,_tcslen(pcsDesc),NULL);
			ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rRect);

			//Make sure we deselect the font so that it won't leak resources on win9x

			SelectObject(hDC, hGDIObj);

			// Present the compare
			RECT rSrc = {0, 0, nDevWidth, nDevHeight};
			RECT rDst = {nOffset, m_nTagHeight, 
				nOffset + (m_bStretchPresent ? nWindowWidth : nDevWidth) , 
				m_nTagHeight + (m_bStretchPresent ? nWindowHeight : nDevHeight)};

			if(rDst.right - rDst.left > 0 && rDst.bottom - rDst.top > 0)
			{
				ValidateRect(m_pFramework->m_pDisplayWindow->m_hWindow, &rDst);

				hr = m_pCmpSwapChain->Present(&rSrc, &rDst, NULL, NULL);
				if(FAILED(hr))
				{
					DPF(1, _T("CD3DTest::DisplayFrame:  ImgCmp Present failed. hr=%s(%x).\n"), 
						m_pD3D->HResultToString(hr), hr);
					bRes = false;
				}
			}
		}
		// Release the DC
		if(hDC)
			ReleaseDC(m_pFramework->m_pDisplayWindow->m_hWindow,hDC);
	}

	return bRes;
}


bool CD3DTest::ToggleCompareWindow()
{
	if(m_Options.TestOptions.bCompareSrcRef && m_Options.D3DOptions.bReference && 
		m_pCurrentMode->bWindowed && m_pCmpSwapChain)
	{
		m_bShowCompareWindow = !m_bShowCompareWindow;
		SetupDisplay(m_pCurrentMode, m_pCurrentMode->bWindowed);

		m_pFramework->GetImageCmp()->SetGenerateCompare(m_bShowCompareWindow);
		SendMessage(m_pFramework->m_hWindow, WM_COMMAND, (WPARAM)IDM_PROCESSFRAME, 0L);
	}

	return true;
}


void CD3DTest::SetCompareSrcRef(bool bEnable)
{
	if(IsCompareEnabled() != bEnable)
	{
		m_bCompareEnabled = bEnable;
		// Close the compare window if needed
		if(!bEnable && m_bShowCompareWindow)
		{
			m_bShowCompareWindow = bEnable;
			SetupDisplay(m_pCurrentMode, m_pCurrentMode->bWindowed);
		}
	}
}


void CD3DTest::SetupDisplay(TESTMODE *pMode, bool bWindowed)
{
	// Resize the window
	if(bWindowed)
	{
		int nWidth=0;
		int nDevWidth = max(MODEOPT_BACKBUFFERWIDTH, pMode->nBackBufferWidth);
		int nDevHeight = max(MODEOPT_BACKBUFFERHEIGHT, pMode->nBackBufferHeight);

		// Calculate the client size
		for(UINT i=0; i<m_uDeviceList; i++)
			nWidth += nDevWidth;
		if(m_bShowCompareWindow)
			nWidth += nDevWidth;

		m_pFramework->SetDisplaySize(nWidth, nDevHeight + m_nTagHeight, true);
	}
	else
		m_pFramework->SetDisplaySize(pMode->pDisplay->dwWidth, pMode->pDisplay->dwHeight, false);

	// Move to correct adapter
	m_pFramework->SetAdapter(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetAdapter());
}


HRESULT CD3DTest::Resize(UINT uWidth, UINT uHeight)
{
	HRESULT hr = S_OK;
	UINT uDevHeight, uDevWidth;

	// Only handle when we have actual devices, caps supported
	if(0 == m_uDeviceList || !(m_Options.TestOptions.dwCaps & TESTCAPS_RESIZEDEVICE))
		return S_OK;

	uDevHeight = uHeight;
	uDevWidth = uWidth / (m_uDeviceList + (m_bShowCompareWindow ? 1 : 0));

	Cleanup();
	ReleaseObjects();

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		hr = m_DeviceList[i]->Resize(uDevWidth, uDevHeight);

		if(FAILED(hr))
			goto Exit;
	}

	CreateObjects();

	if(!Setup())
	{
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	return hr;
}


bool CD3DTest::SkipTestRange(UINT uStart, UINT uStop)
{
	UINT uCount = (uStop - uStart) + 1;

	if(m_pFramework->m_DXGLog.InTestCase())
	{
		if(uCount > 1)
			DPF(2, _T("CD3DTest::SkipTestRange - Do not skip more than 1 test inside BeginTestCase. Undefined result codes occur.\n"));

		m_pFramework->m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_SKIPPED);
	}
	else
		m_pFramework->m_DXGLog.SkipTest(uCount);

	WriteToLog(1, _T("Skipping test ranges %d through %d\n"), uStart, uStop);

	return CTest::SkipTestRange(uStart,uStop);
}


bool CD3DTest::SkipTests(UINT uCount)
{
	if(0 == uCount)
		return false;

	return SkipTestRange(m_uTestNumber,m_uTestNumber+uCount-1);
}


bool CD3DTest::DrawStats(int nWidth, int nHeight, CDevice *pDevice)
{
	TCHAR tcsBuffer[MAX_PATH] = {0}, tcsFmt[64] = {0};
	int nTextHeight = 12;
	int nBottom = nHeight;
	RECT rc = { 0, nBottom-nTextHeight, nWidth, nBottom };

	if(FAILED(pDevice->BeginScene()))
	{
		DPF(1, _T("CD3DTest::DrawStats:  BeginScene failed.\n"));
		return false;
	}

	for(int i=m_pFramework->m_pStatWindow->GetNumLines()-1; i>= 0; i--)
	{
		// Get the log line
		m_pFramework->m_pStatWindow->GetLogLine(i, tcsBuffer, MAX_PATH);

		// Check for tabs.  We will manually insert tab breaks
		TCHAR *sTemp = _tcschr(tcsBuffer, _T('\t'));
		if(NULL != sTemp)
		{
			*sTemp = _T('\0');
			while(*(++sTemp) == _T('\t'));
			rc.left = nWidth / 2;
			pDevice->Draw2DText(0xFF00FFFF, sTemp, _tcslen(sTemp), &rc, DT_LEFT);
		}

		rc.left = 0;
		pDevice->Draw2DText(0xFFFFFFFF, tcsBuffer, _tcslen(tcsBuffer), &rc, DT_LEFT);

		rc.bottom -= nTextHeight;
		rc.top -= nTextHeight;
	}

	// Draw the adapter/test/device info
	ConstructTestCaption(tcsBuffer, ARRAYSIZE(tcsBuffer));
	rc.left = 0; rc.top = 0; rc.bottom = nTextHeight;
	pDevice->Draw2DText(0xFFFFFF00, tcsBuffer, _tcslen(tcsBuffer), &rc, DT_CENTER);

	FmtToString(FORMAT_TEXTURE, &m_pCurrentMode->pDisplay->Format, tcsFmt);
	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s at %dx%dx%s @ %dhz"), (LPTSTR) pDevice->GetDescription(), 
		m_pCurrentMode->pDisplay->dwWidth, 
		m_pCurrentMode->pDisplay->dwHeight, 
		tcsFmt,
		m_pCurrentMode->pDisplay->dwRefreshRate);
	rc.top += nTextHeight; rc.bottom += nTextHeight;
	pDevice->Draw2DText(0xFFFFFF00, tcsBuffer, _tcslen(tcsBuffer), &rc, DT_CENTER);

	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s(%s)"), (LPTSTR) pDevice->GetAdapter()->sAdapterName, 
		(LPTSTR) pDevice->GetAdapter()->sVersion);
	rc.top += nTextHeight; rc.bottom += nTextHeight;
	pDevice->Draw2DText(0xFFFFFF00, tcsBuffer, _tcslen(tcsBuffer), &rc, DT_CENTER);

	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("Result: %s"), m_pFramework->m_DXGLog.GetCurrentResultString());
	rc.top += nTextHeight; rc.bottom += nTextHeight;
	pDevice->Draw2DText(0xFFFFbb00, tcsBuffer, _tcslen(tcsBuffer), &rc, DT_CENTER);

	if(FAILED(pDevice->EndScene()))
	{
		DPF(1, _T("CD3DTest::DrawStats:  EndScene failed.\n"));
		return false;
	}

	return true;
}


void CD3DTest::OnPaint()
{
	if(m_bHandlePaintMsg)
		DisplayFrame();
}


LRESULT CD3DTest::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDM_PROCESSFRAME:
		if(IsCompareEnabled() && m_bHandlePaintMsg)
		{
			ProcessFrame();
			DisplayFrame();
			UpdateControls();
		}
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


UINT CD3DTest::TestInitialize(void)
{
	return D3DTESTINIT_RUN;
}


bool CD3DTest::TestTerminate(void)
{
	// Cleanup any d3d objects

	Cleanup();

	return true;
}


void CD3DTest::TestFinish(UINT uWhy)
{
	switch(uWhy)
	{
		case D3DTESTFINISH_SKIPREMAINDER:
		{
			UINT uCount = GetRemainingTests();
			if(m_pFramework->m_DXGLog.InTestCase())
			{
				EndTestCase();
			}

			WriteToLog(1, _T("Skipping test ranges %u through %u\n") ,m_uTestNumber, m_uTotalTests);
			m_pFramework->m_DXGLog.SkipTest(uCount);
		}
		break;


		case D3DTESTFINISH_ABORT:
		{
			BeginTestCase(_T("Test Initialization"), m_uTestNumber);
			Abort();

			m_fPassPercentage = -1.f;
			EndTestCase();

			WriteToLog(1, _T("\nTest aborted during test %u. (HCT FAIL)\n\n"), m_uTestNumber);
		}
		break;


		case D3DTESTFINISH_FAIL:
		{
			BeginTestCase(_T("Test Initialization"), m_uTestNumber);

			Fail();

			m_fPassPercentage = -1.f;
			EndTestCase();

			WriteToLog(1, _T("\nTest failed during test %u. (HCT FAIL)\n\n"), m_uTestNumber);
		}
		break;
	}

	m_uTestNumber = TERMINATE_NUMBER;
}


void CD3DTest::WriteCommandLineHelp(LPCTSTR ptcsFormat,...)
{
	va_list list;
	TCHAR *sTemp = NULL;
	TCHAR tcsBuffer[MAX_PATH];

	va_start(list, ptcsFormat);
	sTemp = GetVarArgString(ptcsFormat, list);
	va_end(list);

	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("\t\t%s\n"), sTemp);

	SAFEDELETEA(sTemp);

	m_pFramework->WriteCommandLineHelp(tcsBuffer);
}


void CD3DTest::CommandLineHelp()
{
	WriteCommandLineHelp(_T("No help available"));
}


bool CD3DTest::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;
	UINT i;

	for(i=0; i<uDevice; i++)
		dwMask <<= 1;

	// Check the flags, and check that the device requested is valid

	if( ((dwMask & dwFlags) == 0) || m_DeviceList[i] == NULL)
		return true;

	return false;
}


void CD3DTest::InitializeAPICycling()
{
	TCHAR pszAPICommandLine[10] = _T( "" );

	m_dwRPAPICyclingFrequency = 3;
	m_dwRPCurrentAPI = RP_DIP;
	m_pRPAPIList = m_pRPDefaultAPIList;
	m_dwRPAPIListSize = m_dwRPDefaultAPIListSize;
	m_bRPCommandLineOverwrite = false;
	m_dwRPAPICyclingIndex = 0;

	// Process -api command line switch. It needs to be done after device setup but before TestInitialize();
	//
	if (ReadString("API", pszAPICommandLine, ARRAYSIZE(pszAPICommandLine)))
	{
		if      (!strcmp(pszAPICommandLine,"be"))       { SetAPI(RP_BE);            m_bRPCommandLineOverwrite = true; } 
		else if (!strcmp(pszAPICommandLine,"bie"))      { SetAPI(RP_BIE);           m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dp"))       { SetAPI(RP_DP);            m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dip"))      { SetAPI(RP_DIP);           m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dps"))      { SetAPI(RP_DPS);           m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dips"))     { SetAPI(RP_DIPS);          m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dpvb"))     { SetAPI(RP_DPVB);          m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dipvb"))    { SetAPI(RP_DIPVB);         m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dpvbpv"))   { SetAPI(RP_DPVB_PV);       m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dipvbpv"))  { SetAPI(RP_DIPVB_PV);      m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dpvbo"))    { SetAPI(RP_DPVB_OPT);      m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dipvbo"))   { SetAPI(RP_DIPVB_OPT);     m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dpvbopv"))  { SetAPI(RP_DPVB_OPT_PV);   m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dipvbopv")) { SetAPI(RP_DIPVB_OPT_PV);  m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dpvbpvs"))  { SetAPI(RP_DPVB_PVS);      m_bRPCommandLineOverwrite = true; }
		else if (!strcmp(pszAPICommandLine,"dipvbpvs")) { SetAPI(RP_DIPVB_PVS);     m_bRPCommandLineOverwrite = true; }
	}
}


void CD3DTest::SetAPI(RENDERPRIMITIVEAPI dwAPI)
{
	// Do not change API if it was set with command line
	if (m_bRPCommandLineOverwrite)
		return;

	// Set new API and disable cycling
	m_dwRPCurrentAPI = dwAPI;
	m_pRPAPIList = NULL;
	m_dwRPAPIListSize = 0;
	m_dwRPAPICyclingFrequency = 0;
	m_dwRPAPICyclingIndex = 0;
	for (UINT i = 0; i < m_uDeviceList; i++)
		m_DeviceList[i]->SetAPI(m_dwRPCurrentAPI);
}


void CD3DTest::SetAPI(PRENDERPRIMITIVEAPI pAPIList, DWORD dwAPIListSize, DWORD dwAPICyclingFrequency)
{
	// Do not change API if it was set with command line
	if (m_bRPCommandLineOverwrite)
		return;

	if (NULL == pAPIList || 0 == dwAPIListSize)
	{
		// Disbale API switching
		m_pRPAPIList = NULL;
		m_dwRPAPIListSize = 0;
		m_dwRPAPICyclingFrequency = 0;
		m_dwRPAPICyclingIndex = 0;
	}
	else
	{
		// Set new API list
		m_pRPAPIList = pAPIList;
		m_dwRPAPIListSize = dwAPIListSize;
		m_dwRPAPICyclingFrequency = dwAPICyclingFrequency;

		// Reset current API settings
		m_dwRPAPICyclingIndex = 0;
		m_dwRPCurrentAPI = m_pRPAPIList[m_dwRPAPICyclingIndex];
		for (UINT i = 0; i < m_uDeviceList; i++)
			m_DeviceList[i]->SetAPI(m_dwRPCurrentAPI);
	}
}


void CD3DTest::SwitchAPI(DWORD dwTestNumber)
{
	// Don't do anything if API switching is disabled
	if (0 == m_dwRPAPICyclingFrequency || 
		NULL == m_pRPAPIList ||
		0 == m_dwRPAPIListSize ||
		true == m_bRPCommandLineOverwrite)
	{
		return;
	}

	// Check if we are ready to switch
	DWORD dwTest = dwTestNumber - 1; // zero-based test number
	m_dwRPAPICyclingIndex = (dwTest / m_dwRPAPICyclingFrequency) % m_dwRPAPIListSize;
	m_dwRPCurrentAPI = m_pRPAPIList[m_dwRPAPICyclingIndex];
	for (UINT i = 0; i < m_uDeviceList; i++)
		m_DeviceList[i]->SetAPI(m_dwRPCurrentAPI);
}


RENDERPRIMITIVEAPI CD3DTest::GetAPI(void)
{
	return m_dwRPCurrentAPI;
}


TCHAR* CD3DTest::TexLoadToString(DWORD dwTexLoad)
{
	switch(dwTexLoad)
	{
		case TEXLOAD_DEFAULT:		return _T("Default");
		case TEXLOAD_UPDATETEXTURE:	return _T("UpdateTexture");
		case TEXLOAD_COPYRECTS:		return _T("CopyRects");
		case TEXLOAD_LOCKCOPY:		return _T("Lock & Copy");
		case TEXLOAD_UPDATESURFACE:	return _T("UpdateSurface");
		default:					return _T("Unknown");
	}
}


TCHAR* CD3DTest::RPModeToString(void)
{
	switch(m_dwRPCurrentAPI)
	{
		case RP_BE:			  return _T("Begin/Vertex/End");
		case RP_BIE:          return _T("BeginIndexed/Index/End");
		case RP_DP:           return _T("DrawPrimitiveUP (DrawPrimitive)");
		case RP_DIP:          return _T("DrawIndexedPrimitiveUP (DrawIndexedPrimitive)");
		case RP_DPS:          return _T("DrawPrimitive multiple streams (DrawPrimitiveStrided)");
		case RP_DIPS:         return _T("DrawIndexedPrimitive multiple streams (DrawIndexedPrimitiveStrided)");
		case RP_DPVB:         return _T("DrawPrimitive single stream (DrawPrimitiveVB)");
		case RP_DIPVB:        return _T("DrawIndexedPrimitive single stream (DrawIndexedPrimitiveVB)");
		case RP_DPVB_PV:      return _T("DrawPrimitiveVB after ProcessVertices)");
		case RP_DIPVB_PV:     return _T("DrawIndexedPrimitiveVB after ProcessVertices)");
		case RP_DPVB_OPT:     return _T("DrawPrimitiveVB with optimized vertex buffer)");
		case RP_DIPVB_OPT:    return _T("DrawIndexedPrimitiveVB with optimized vertex buffer)");
		case RP_DPVB_OPT_PV:  return _T("DrawPrimitiveVB after ProcessVertices with optimized vertex buffer)");
		case RP_DIPVB_OPT_PV: return _T("DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer)");
		case RP_DPVB_PVS:     return _T("DrawPrimitiveVB after ProcessVerticesStrided)");
		case RP_DIPVB_PVS:    return _T("DrawIndexedPrimitiveVB after ProcessVerticesStrided)");
		default:			  return _T("Unknown");
	}
}


void CD3DTest::SetDestinationFVF(DWORD dwDestinationFVF)
{
	m_dwDestinationFVF = dwDestinationFVF;
	for (UINT i = 0; i < m_uDeviceList; i++)
		m_DeviceList[i]->SetDestinationFVF(m_dwDestinationFVF);
}


DWORD CD3DTest::GetDestinationFVF(void)
{
	return m_dwDestinationFVF;
}


void CD3DTest::MuteLog()
{
	m_pFramework->MuteLog();
}


void CD3DTest::UnMuteLog()
{
	m_pFramework->UnMuteLog();
}


bool CD3DTest::LoseTheDevice(DWORD dwLostType)
{
	switch (dwLostType)
	{
		case D3DLOSEDEV_DEFAULT:
			return m_pFramework->LoseDeviceDefault(m_pFramework->m_pCurrentDevMode->nAdapter);

		case D3DLOSEDEV_BYOTHERPROCESS:
			return m_pFramework->LoseDeviceByOtherProcess(m_pFramework->m_pCurrentDevMode->nAdapter);

		case D3DLOSEDEV_BYFUS:
			return m_pFramework->LoseDeviceByFUS();

		case D3DLOSEDEV_BYTERMINALSERVER:
			return m_pFramework->LoseDeviceByTerminalServer();

		case D3DLOSEDEV_BYTDR:
			return m_pFramework->LoseDeviceByTDR(m_pFramework->m_pCurrentDevMode->nAdapter);

		case D3DLOSEDEV_BYSTANDBY:
			return m_pFramework->SuspendSystem(false);

		case D3DLOSEDEV_BYHIBERNATE:
			return m_pFramework->SuspendSystem(true);

		default:
			WriteToLog(_T("CD3DTest::LoseTheDevice:  Invalid dwLostType\n"));
			return false;
	}

	return false;
}


void CD3DTest::OnDeviceLost()
{
	// Default to calling the app's d3d object cleanup
	Cleanup();
}


void CD3DTest::OnReset()
{
	// Default to reset all Default test objects
	Setup();
}


bool CD3DTest::PreviousGroup(void)
{
	if(m_pCurrentMode == NULL || NULL == m_pCurrentMode->pPrevious)
		return false;

	m_bReverseMode = true;
	return true;
}


HRESULT CD3DTest::ExecuteTests()
{
	while(!m_pFramework->IsTestFinished() && !IsTestFinished())
	{
		// Execute the test if it's not paused
		if(!m_pFramework->IsTestPaused())
		{
			// Prevent screensavers from kicking in on our test.
			if ( m_Options.TestOptions.bDisableIdleDetection == true )
			{
				PulseScreensaverTimeout();
			};

			// If we're replaying the same test, pause execution and execute the current 
			// test again.  Otherwise, get the next test and continue
			if(!m_pFramework->IsTestReplaying())
			{
				NextTest();

				if ( m_fSkipsPerTest != 0.0f && TERMINATE_NUMBER != m_uTestNumber )
				{
					if ( 1.0f > m_fSkipCount )
					{
						m_fSkipCount += m_fSkipsPerTest;
					}
					else
					{					
						if ( m_fSkipCount >= m_uTotalTests - m_uTestNumber )
						{
							TestFinish( D3DTESTFINISH_SKIPREMAINDER );
							m_fSkipCount = 0.0f;
							continue;
						}
						else
						{
							SkipTests( (UINT)m_fSkipCount );
							m_fSkipCount = 0.0f;
							continue;
						}
					}				
				}
			}
			else 
				m_pFramework->Pause();

			if ( 0 != m_uTestNumber )
			{
				// If we're not doing exception handling for the test...
				if (!m_bException)
				{
					// ...then just run it...
					ExecuteOneTest();
				}
				// ...otherwise...
				else
				{
					// ..setup a try block...
					__try
					{
						// ...and run the test...
						ExecuteOneTest();
					}
					// ...and if we catch an exception, then run it through our filter...
					__except( ExceptionTraceFilter( GetExceptionInformation(), &s_ExceptionTrace ) )
					{
						// ...and if the filter passes it through, then log the details.
						if(!m_pFramework->m_DXGLog.InTestCase())
						{
							BeginTestCase(_T("Not within any test case"), m_uTestNumber);
						};

						WriteToLog(_T("***  Caught an exception calling ExecuteOneTest:  "));
						WriteToLog( "### %s", s_ExceptionTrace.GetExceptionDescString().c_str() );

						for( size_t i = 0; i < s_ExceptionTrace.GetTraceDepth(); ++i )
						{
							WriteToLog( "### %s", s_ExceptionTrace.GetTrace( i ).c_str() );
						};

						Fail();
						EndTestCase();

						m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
					};
				};
			};
		};


		if ( m_pFramework->IsTestPaused() )
		{
			WaitMessage();
		};


		if ( !m_pFramework->ProcessMessages() )
		{
			if ( !m_pFramework->m_DXGLog.InTestCase() )
			{
				BeginTestCase( "User aborted test.", 0xFFFFFFFF );
			};

			WriteToLog(_T("***  Test aborted by user.\n"));

			Abort();
			EndTestCase();

			m_pFramework->m_dwExecuteState = EXECUTE_TERMINATE;
			m_pFramework->SuggestExitCode(EXITCODE_ABORT);
		};
	};


	return S_OK;
}


HRESULT	CD3DTest::LoadSaveGoldenImage(CSurface *pSurface, bool bLoadorSave)
{
	HRESULT hr = S_OK;
	TCHAR sFileName[MAX_PATH];
	TCHAR sDir[MAX_PATH];
	TCHAR *sTemp = NULL;
	CDevice *pDevice = NULL;

	if(NULL == pSurface)
		return E_INVALIDARG;

	pSurface->GetDevice(&pDevice);
	if(NULL == pDevice)
		return E_INVALIDARG;

	// Clear the buffer
	memset(sFileName, 0, sizeof(TCHAR) * MAX_PATH);

	// Create the filename
	StringCchPrintf(sDir, ARRAYSIZE(sDir), _T("\\\\dxdlab\\d3dtests%s\\goldenimages\\%s\\"), bLoadorSave ? _T("") : _T("$"), m_pFramework->m_LogInfo.sTestName);
	StringCchPrintf(sFileName, ARRAYSIZE(sFileName), _T("%s%s(%d).bmp"), sDir, m_pFramework->m_DXGLog.GetCurrentGroup(), m_uTestNumber);

	// Cleanup the filename
	sTemp = sFileName;
	while(*sTemp != _T('\0'))
	{
		switch(*sTemp)
		{
			case _T(':'):
				if(*(sTemp+1) != _T('\\'))  // Remove ':' except when ":\" except when use as drive spec(ex. c:\winnt)
					*sTemp = _T('-');
			break;

			default:
			break;
		}
		sTemp++;
	}

	if(bLoadorSave)
	{
		hr = pDevice->D3DXLoadSurfaceFromFile(pSurface, NULL, NULL, sFileName, NULL, 1, 0, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::LoadGoldenImage:  D3DXLoadSurfaceFromFile failed for %s.\n"),sFileName);
			DPF(1, _T("CD3DTest::LoadGoldenImage:  hr = %s(%x).\n"), m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}
	else
	{
		CreateDirectory(sDir, NULL);

		hr = pDevice->D3DXSaveSurfaceToFileA(sFileName, (D3DXIMAGE_FILEFORMAT)0/* D3DXIFF_BMP */, pSurface, NULL, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::LoadGoldenImage:  D3DXSaveSurfaceToFile failed for %s.\n")
				_T("CD3DTest::LoadGoldenImage:  hr = %s(%x).\n"), sFileName, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	RELEASE(pDevice);
	return hr;
}


HRESULT CD3DTest::DeviceCapture(DWORD dwFlags)
{
	CSurface *pSurf = NULL;
	HRESULT hr = S_OK;
	TCHAR *sTemp = NULL, *sGroupName = NULL;
	TCHAR sFileName[MAX_PATH], sDir[MAX_PATH];
	TCHAR tcsBuffer[MAX_PATH], tcsShortPath[MAX_PATH];
	UINT uiIndex = 0;

	CSurface * pSurfacesFB[32];

	if(m_pD3D->GetVersion() <= 7.01f)
	{
		hr = E_UNSUPPORTED;
		DPF(1, _T("Cannot save front/back buffers to disk on DX7 / DX6.\n"));
		goto Exit;
	}

	memset( pSurfacesFB, 0, sizeof(CSurface*) * 32 );

	if (g_pFramework->m_DXGLog.GetSysInfo()->IsWin9x()) {
		GetExpandedPath(_T("%windir%\\dxlogs\\"), sDir);
	}
	else {
		GetShortPathName(
			GetExpandedPath(_T("%userprofile%\\dxlogs\\"),tcsBuffer), tcsShortPath, MAX_PATH);

		HRESULT hFormatLogPathResult = StringCchPrintf(sDir, ARRAYSIZE( sDir ),_T("%s"),tcsShortPath);
		if ( FAILED( hFormatLogPathResult ) )
		{
			return hFormatLogPathResult;
		};
	}


	// Make a local copy of the group name and remove invalid filename chars
	COPYSTRING(sGroupName, m_pFramework->m_DXGLog.GetCurrentGroup());
	if(NULL == sGroupName)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	sTemp = sGroupName;
	while(*sTemp != _T('\0'))
	{
		switch(*sTemp)
		{
		case _T('/'):
		case _T('\\'):
		case _T(':'):
		case _T('<'):
		case _T('>'):
		case _T('|'):
			*sTemp = _T('-');
			break;
		default:
			break;
		}

		sTemp++;
	}

	//Do this once to improve performance for front buffer comparisons
	if ( m_Options.TestOptions.bCompareFB )
	{
		hr = GetFrontBuffers( pSurfacesFB, dwFlags );
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeviceCapture:  GetFrontBuffers failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr));
			goto Exit;
		}
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		StringCchPrintf(sFileName, ARRAYSIZE(sFileName), _T("%sDevice #%d - %s(%d).bmp"), sDir, i, sGroupName, m_uTestNumber);

		if ( m_Options.TestOptions.bCompareFB )
		{
			//Must use another counter, since the array is compacted, ignoring devices that have been skipped.
			pSurf = pSurfacesFB[uiIndex];
			pSurfacesFB[uiIndex] = NULL;
			uiIndex++;
		}
		else
		{
			hr = m_DeviceList[i]->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::DeviceCapture:  GetBackBufferCached failed on device %d. hr=%s(%x).\n"),
					i, m_pD3D->HResultToString(hr));
				goto Exit;
			}
		}

		hr = m_DeviceList[i]->D3DXSaveSurfaceToFileA(sFileName, (D3DXIMAGE_FILEFORMAT)0/* D3DXIFF_BMP */, pSurf, NULL, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeviceCapture:  D3DXLoadSurfaceFromFile failed for %s.\n"),sFileName);
			DPF(1, _T("CD3DTest::DeviceCapture:  hr = %s(%x).\n"), m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		RELEASE( pSurf );
	}

	if(dwFlags & CD3D_IMAGECMP && (NULL != m_pCmpSwapChain || NULL != m_pCmpTexture))
	{
		// If we're not showing the image compare window, we won't have a valid
		// image compare result in the swapchain so we need to force an update.
		if(!m_bShowCompareWindow)
		{
			m_pFramework->GetImageCmp()->SetGenerateCompare(true);

			if(!m_pCurrentMode->bWindowed)
			{
				// Need to replay the test to generate a Compare result
				// without the onscreen stats
				SendMessage(m_pFramework->m_hWindow, WM_COMMAND, IDM_EXECUTION_REPLAY, 0);
				ProcessMessages();
			}
			else
				ProcessFrame();

			m_pFramework->GetImageCmp()->SetGenerateCompare(false);
		}

		StringCchPrintf(sFileName, ARRAYSIZE(sFileName), _T("%sImage Compare - %s(%d).bmp"), sDir, sGroupName, m_uTestNumber);

		if(m_pCurrentMode->bWindowed)
		{
			ASSERT(m_pCmpSwapChain != NULL);
			hr = m_pCmpSwapChain->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::DeviceCapture:  GetBackBufferCached failed.\n"));
				goto Exit;
			}
		}
		else
		{
			hr = m_pCmpTexture->GetSurfaceLevel(0, &pSurf);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::DeviceCapture:  GetSurfaceLevel failed.\n"));
				goto Exit;
			}
		}

		hr = m_pRefDevice->D3DXSaveSurfaceToFileA(sFileName, (D3DXIMAGE_FILEFORMAT)1/* D3DXIFF_BMP */, pSurf, NULL, NULL);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeviceCapture:  D3DXSaveSurfaceToFileA failed for %s.\n"),sFileName);
			DPF(1, _T("CD3DTest::DeviceCapture:  hr = %s(%x).\n"), m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	RELEASE(pSurf);

	if ( m_Options.TestOptions.bCompareFB )
	{
		for ( UINT i = 0; i < 32; i++ )
			RELEASE( pSurfacesFB[i] );
	}

	SAFEDELETEA(sGroupName);

	return hr;
}


HRESULT CD3DTest::ProcessGoldenImage()
{
	HRESULT hr = S_OK;
	CSurface *pSurf = NULL;

	// Generate the golden image from the src
	if(m_bGenGolden)
	{
		hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ProcessGoldenImage:  GetBackBufferCached failed on SRC. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr));
			goto Exit;
		}

		hr = LoadSaveGoldenImage(pSurf, false);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ProcessGoldenImage:  LoadSaveGoldenImage failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr));
			goto Exit;
		}
	}
	RELEASE(pSurf);

	// Now load the golden image into the REF buffer
	if(NULL != m_pRefDevice)
	{
		hr = m_pRefDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ProcessGoldenImage:  GetBackBufferCached failed on SRC. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr));
			goto Exit;
		}

		hr = LoadSaveGoldenImage(pSurf, true);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ProcessGoldenImage:  LoadSaveGoldenImage failed. hr=%s(%x).\n"),
				m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	RELEASE(pSurf);
	return hr;
}


HRESULT CD3DTest::GetFrontBuffers( CSurface ** pSurface, DWORD dwFlags  )
{
	//Takes a pointer to an array of surfaces equal in size the the number of devices requested by dwFlags

	DWORD dwSurfaces = 0;

	UINT		nWidth, nHeight;
	UINT		nOffset;
	HRESULT		hr = S_OK;
	BYTE		*pFB, *pDest;
	RECT		SrcRect;
	POINT		CornerPoint = {0,0};
	LOCKED_RECT	FBLockRect, DestLockRect;
	CSurface *pFBSurf = NULL;

	RECT PrevRect;
	RECT MouseTrap;

	GetWindowRect( m_pFramework->m_pRebar->m_hWindow, &MouseTrap );

	GetClipCursor( &PrevRect );

	nOffset = m_pFramework->m_pDisplayWindow->m_nWidth/(m_uDeviceList + ((m_bShowCompareWindow && m_pCmpSwapChain) ? 1:0));

	if ( m_bStretchPresent )
	{
		nWidth = m_pFramework->m_pDisplayWindow->m_nWidth/(m_uDeviceList + ((m_bShowCompareWindow && m_pCmpSwapChain) ? 1:0));
		nHeight = m_pFramework->m_pDisplayWindow->m_nHeight - m_nTagHeight;;
	}
	else
	{
		nWidth = m_pCurrentMode->nBackBufferWidth;
		nHeight = m_pCurrentMode->nBackBufferHeight;
	}

	CornerPoint.y = m_nTagHeight;
	if ( !ClientToScreen( m_pFramework->m_pDisplayWindow->m_hWindow, &CornerPoint ) )
	{
		DPF(1, _T("CD3DTest::GetFrontBuffers:  ClientToScreen failed." ));
		goto Fail;
	}

	memcpy( &SrcRect, &(m_pSrcDevice->GetAdapter()->rMonitorRect), sizeof(RECT) );

	//Get the front buffer
	hr = m_pSrcDevice->CreateImageSurface( SrcRect.right - SrcRect.left, SrcRect.bottom - SrcRect.top,
		FMT_A8R8G8B8, &pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CD3DTest::GetFrontBuffers:  CreateImageSurface failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		goto Fail;
	}

	ClipCursor( &MouseTrap );
	hr = m_pSrcDevice->GetFrontBuffer( pFBSurf );
	if(FAILED(hr))
	{
		DPF(1, _T("CD3DTest::GetFrontBuffers:  GetFrontBuffer failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		ClipCursor( &PrevRect );
		goto Fail;
	}
	ClipCursor( &PrevRect );

	//First determine the number of surfaces we're working with
	for ( UINT j = 0, i = 1, k = 0; i < ( 1 << m_uDeviceList ); i <<= 1, j++ )
	{
		if ( i & dwFlags )
		{
			//We have a match!
			hr = m_pSrcDevice->CreateImageSurface( nWidth, nHeight, FMT_A8R8G8B8, &pSurface[k] );
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::GetFrontBuffers:  CreateImageSurface failed on SRC. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				goto Fail;
			}
			//Keep this going so we know how many to release if we bomb out
			dwSurfaces++;

			SrcRect.top = CornerPoint.y - m_pSrcDevice->GetAdapter()->rMonitorRect.top;
			SrcRect.left = CornerPoint.x - m_pSrcDevice->GetAdapter()->rMonitorRect.left;
			SrcRect.bottom = SrcRect.top + nHeight;
			SrcRect.right = SrcRect.left + nWidth;

			SrcRect.left += ( nOffset * j );
			SrcRect.right += ( nOffset * j );

			// Get dest
			hr = pFBSurf->LockRect( &FBLockRect, &SrcRect, 0 );
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::GetFrontBuffers:  LockRect failed on source. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				goto Fail;
			}
			hr = pSurface[k]->LockRect( &DestLockRect, NULL, 0 );
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::GetFrontBuffers:  LockRect failed on dest. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
				goto Fail;
			}

			//Do the copy
			pFB = (BYTE*) FBLockRect.pBits;
			pDest = (BYTE*) DestLockRect.pBits;
			for ( UINT n = 0; n < nHeight; n++ )
			{
				memcpy( pDest, pFB, DestLockRect.iPitch );

				pFB += FBLockRect.iPitch;
				pDest += DestLockRect.iPitch;
			}

			//Now unlock both surfaces
			hr = pFBSurf->UnlockRect();
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::GetFrontBuffers:  UnlockRect failed on source. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
			}
			hr = pSurface[k]->UnlockRect();
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::GetFrontBuffers:  UnlockRect failed on dest. hr=%s(%x).\n"),
					m_pD3D->HResultToString(hr));
			}

			k++;
		}
	}

	RELEASE( pFBSurf );

	return hr;
Fail:
	RELEASE( pFBSurf );

	for ( UINT i = 0; i < dwSurfaces; i++ )
		RELEASE( pSurface[i] );

	return hr;
}


HRESULT CD3DTest::PulseScreensaverTimeout()
{
	//
	// Get the screensaver timeout.
	//

	UINT ScreensaverTimeout = 0;
	BOOL bGetScreensaverTimeoutResult = SystemParametersInfo
	(
		SPI_GETSCREENSAVETIMEOUT,
		0,
		&ScreensaverTimeout,
		0
	);

	if ( bGetScreensaverTimeoutResult != TRUE )
	{
		DWORD GetScreensaverTimeoutError = GetLastError();

		if ( GetScreensaverTimeoutError == ERROR_SUCCESS )
		{
			return E_FAIL;
		};

		return HRESULT_FROM_WIN32( GetScreensaverTimeoutError );
	};


	//
	// Set a default if needed.
	//

	if ( ScreensaverTimeout == 0 )
	{
		// Default to 10 minutes.
		ScreensaverTimeout = 10 * 60;
	};


	//
	// Set the screensaver timeout to the same value.
	// This has the effect of resetting the timer.
	//

	BOOL bSetScreensaverTimeoutResult = SystemParametersInfo
	(
		SPI_SETSCREENSAVETIMEOUT,
		ScreensaverTimeout,
		NULL,
		0
	);

	if ( bSetScreensaverTimeoutResult == TRUE )
	{
		return S_OK;
	};


	return HRESULT_FROM_WIN32( ::GetLastError() );
};


DWORD GetDXVersionFlag(float fVersion)
{
	if(fVersion >= 9.1f)
		return DXVERSION_91;
	else if(fVersion >= 9.f && fVersion < 9.1f)
		return DXVERSION_9;
	else if(fVersion >= 8.1f && fVersion < 9.0f)
		return DXVERSION_81;
	else if(fVersion >= 8.f && fVersion < 8.1f)
		return DXVERSION_8;
	else if(FLOAT_EQUAL(fVersion, 7.f))
		return DXVERSION_7;
	else if(FLOAT_EQUAL(fVersion, 6.f))
		return DXVERSION_6;
	else
		return 0;
}
