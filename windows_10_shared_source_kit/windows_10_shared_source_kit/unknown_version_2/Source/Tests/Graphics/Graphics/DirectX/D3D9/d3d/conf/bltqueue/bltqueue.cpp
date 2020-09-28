//
//  BltQueue.cpp
//
//  Blt Queuer Test.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2001 Microsoft Corporation.
//  Author:  Chuck Noble (chuckn)
//  Additional comments added by Olanh - April 2006
//

#include "BltQueue.h"
#include "CD3DTest.h"
#include "CBuffers.h"
#include "CSurface.h"
#include "CTextures.h"
#include <d3d8.h>
#include <d3dx8.h>

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
const window_width = (MODEOPT_BACKBUFFERWIDTH)<<1;
const window_height = (MODEOPT_BACKBUFFERHEIGHT)<<1;

// Declare the framework
CD3DWindowFramework App;

// Define the groups here
BltQueue classBltQueue;


//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

BltQueue::BltQueue()
{
	SetTestType( TESTTYPE_CONF );
	m_Options.D3DOptions.fMinDXVersion		= 8.f;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_SWVP;
	m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.bEnumReference = false;
	m_Options.D3DOptions.bZBuffer = false;
	m_Options.D3DOptions.nBackBufferWidth = window_width;
	m_Options.D3DOptions.nBackBufferHeight = window_height;

	m_szTestName = _T("BltQueue");
	m_szCommandKey = _T("BltQueue");

	m_bHadForceDriversOnKey = false;
	m_bHadForceDriversOffKey = false;
	m_dwOldForceDriversOnValue = 0;
	m_dwOldForceDriversOffValue = 0;

	m_pVB = 0L;
	m_pBackbuffer = 0L;
	m_pTexture = 0L;
}


//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

bool BltQueue::SetDefaultRenderStates()
{
	bool bResult = true;

	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_ZENABLE, (DWORD) FALSE ));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_SPECULARENABLE, (DWORD) FALSE ));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, (DWORD) FALSE ));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD) FALSE ));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_CLIPPING, (DWORD)TRUE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ));

	// Modulate color and alpha
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE));

	// Setup texture addressing and filtering
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP));
	bResult &= SUCCEEDED(m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE));
	
	return bResult;
}

//-----------------------------------------------------------------------------

bool BltQueue::PreModeSetup(void)
{
	if( KeySet(_T("FORCE") ) )
	{
		if( !SetDriverFlags( KNOWN_NOTAWINDOWEDBLTQUEUER, true ) )
			return false;
	}

	return CD3DTest::PreModeSetup();
}

//-----------------------------------------------------------------------------

bool BltQueue::CapsCheck()
{
	if(0 == (m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
	{
		WriteToLog(_T("D3DPMISCCAPS_CLIPTLVERTS not supported.  Skipping test.") );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

UINT BltQueue::TestInitialize()
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
		return D3DTESTINIT_SKIPALL;
	};


	//
	// Set the test range.
	//

	SetTestRange(1, 1);


	try
	{
		HRESULT hr;

		// create the texture
		if( FAILED(hr = m_pSrcDevice->D3DXCreateTextureFromResourceEx(	NULL,
																		_T("dx.bmp"),
																		256,
																		256,
																		1,
																		0,
																		D3DFMT_UNKNOWN,
																		D3DPOOL_DEFAULT,
																		D3DX_FILTER_TRIANGLE,
																		D3DX_FILTER_TRIANGLE,
																		(D3DCOLOR)0,
																		NULL,
																		NULL,
																		&m_pTexture
																		) ) )
		{
			WriteToLog(_T("D3DXCreateTextureFromResourceEx() returned %s (0x%x) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
		// create the vertex buffer
		if( FAILED(hr = m_pSrcDevice->CreateVertexBuffer(	4 * sizeof(BLTQUEUE_VERTEX),
															D3DUSAGE_SOFTWAREPROCESSING|D3DUSAGE_WRITEONLY,
															BLTQUEUE_FVF,
															POOL_SYSTEMMEM,
															&m_pVB)))
		{
			WriteToLog(_T("CreateVertexBuffer() returned %s (0x%s) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
		// we need the backbuffer as a destination for the blt operations.
		if( FAILED(hr = m_pSrcDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackbuffer ) ) )
		{
			WriteToLog(_T("GetBackBuffer() returned %s (0x%s) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
		// state setup
		if( FAILED(hr = m_pSrcDevice->SetStreamSource(0,m_pVB,sizeof(BLTQUEUE_VERTEX))))
		{
			WriteToLog(_T("SetStreamSource() returned %s (0x%s) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
		if( FAILED(hr = m_pSrcDevice->SetVertexShader(BLTQUEUE_FVF)))
		{
			WriteToLog(_T("SetVertexShader() returned %s (0x%s) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
		if( FAILED(hr = m_pSrcDevice->SetTexture(0,m_pTexture)))
		{
			WriteToLog(_T("SetTexture() returned %s (0x%s) - ABORTING\n"), m_pD3D->HResultToString(hr), hr);
			throw hr;
		}
	}
	catch(HRESULT hr)
	{
		RELEASE(m_pBackbuffer);
		RELEASE(m_pVB);
		RELEASE(m_pTexture);
		return D3DTESTINIT_ABORT;
	}

	// boost the test priority
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass( hProcess, REALTIME_PRIORITY_CLASS );

	// boost the thread priority and run the test
	HANDLE hThread = GetCurrentThread();
	SetThreadPriority( hThread, THREAD_PRIORITY_TIME_CRITICAL );


	return uResult;
}

//-----------------------------------------------------------------------------

bool BltQueue::TestTerminate()
{
	// return test priority to normal
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass( hProcess, NORMAL_PRIORITY_CLASS );

	RELEASE(m_pBackbuffer);
	RELEASE(m_pVB);
	RELEASE(m_pTexture);

	if( KeySet(_T("FORCE") ) )
		ResetDriverFlags( );

	return CD3DTest::TestTerminate( );
}

//-----------------------------------------------------------------------------

bool BltQueue::ExecuteTest(UINT nTest)
{
	bool bResult = true;

	BeginTestCase( TEXT("BltQueue"), nTest );
	ClearFrame();

	// n[i] = time it took for i blts
	// nD[i] = time it took minus the time a LockRect w/zero preceeding blts.
	// nDMax = maximum allowable time for the blt queue to flush... Assumes it should
	// not exceed the time it took 3 blts, plus half the time it took one blt.

	// olanh -Above comments appear to be out of date.  This code is simply recording 
	// when the longest lock occurred and when the second longest lock occurred.
	// These values should be roughly equivalent.  I'm guessing the second longest 
	// time is recorded so we can throw out the worse case.
	// Once the blt queue is full, all the times should be about the same.
	// Some will be slightly longer or shorter, but we can still use this value
	// as a rough indicator of when we've filled the queue.
	UINT n[MAX_BLTS], nDMaxAct,nDMaxAct_II,nD;
	UINT i;
	
	bool bDumpValues = KeySet(_T("DUMP"));

	nDMaxAct = 0;
	nDMaxAct_II = 0;
	// Test w/i number of blts
	for( i=0; i<MAX_BLTS; i++)
	{
		n[i]= TestBltQueue(i);
		if( n[i] >= nDMaxAct )
		{
			nDMaxAct_II = nDMaxAct;
			nDMaxAct = n[i];
		}
		
		if(bDumpValues)
			WriteToLog( _T("Time[%d]=%d\n"), i, n[i] );
	}

	// nix the test if LockRect didn't eat up at least a little time by
	// our timer.   This would only occur if QPC precision was too low.
	if( n[0] < 8 )
	{
			WriteToLog(0, _T("Skipping Test!\n") );
			WriteToLog(0, _T("QueryPerformanceCounter resolution is too low!  Time for a Lock() call was\n") );
			WriteToLog(0, _T("only %d QPC cycles. (8 is the minimum needed to run test.)  This is a known\n"), n[0] );
			WriteToLog(0, _T("problem on Win9x with high-end systems.\n") );

			Skip();

			return true;
	}

	// if we essentially had a flat line don't use nDMaxAct or results would be unpredictable
	//
	// olanh - I believe this code was tweaked based on experimental 
	// observations that will hopefully remain consistent over time.   The 
	// test multiplies the max lock time by .875 and hopes that the first 
	// lock time that exceeds this value represents when we hit the queue 
	// limit.  This could be overly generous, but at least we won't trigger false
	// failures.
	// The test also makes sure that cost of queueing is relatively high compared
	// to other factors (such as lock overhead).   If the max lock time is not 
	// significatly higher that the lock time for a single frame, the test uses 
	// the single frame time, this effectively causes the test to always pass.
	// I wonder if might be better to log a skip in this case.
	
	FLOAT fDMax = 0.875f * (FLOAT)(nDMaxAct_II > n[1]*2 ? nDMaxAct_II: n[1]);

	UINT nBltsQueued;
	nBltsQueued = 0;
	for( i=0; i<MAX_BLTS; i++)
	{
		if( (FLOAT) n[i] > fDMax )
		{
			nBltsQueued = i;
			break;
		}
	}

	if( nBltsQueued > BLT_MAX )
	{
		WriteToLog( _T("See DDBLT_PRESENTATION comments in DDRAW.H:\n") );
		WriteToLog( _T("Thus the true rule is that the driver\n") );
		WriteToLog( _T("must not schedule a DDBLT_PRESENTATION\n") );
		WriteToLog( _T("blt if there are 3 or more\n") );
		WriteToLog( _T("DDBLT_PRESENTLAST blts in the hardware pipe.\n") );

		WriteToLog( _T("\nFailure: ") );
		bResult = false;
	}
	else
		if( nBltsQueued > BLT_WARN )
			WriteToLog( _T("Warning: ") );

	WriteToLog( nBltsQueued >= MAX_BLTS-3 ? _T("Blts Queued >= %d.\n") : _T("Blts Queued = %d.\n"), nBltsQueued );


	if( false == bResult )
		Fail();
	else
	{
		if( bDumpValues )
		{
			WriteToLog(_T("Test Passed.  Calling Fail() to flush raw times to log.\n"));
			Fail();
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

//
// Repeatedly test the Blts, and report the minimum time in the series of tests
//
UINT BltQueue::TestBltQueue(UINT nBlts)
{
	UINT nThisTime;
	UINT nMinTime = (UINT)0;
	bool bMinTimeInitialized = false;;
	
	for( int i=0; i<64; i++)
	{
		// Make sure we flush anything that's pending
		LOCKED_RECT lrc;
		if( FAILED( m_pBackbuffer->LockRect( &lrc, NULL, (DWORD)0 ) ) )
		{
			return (UINT)0;
		}
		if( FAILED( m_pBackbuffer->UnlockRect( ) ) )
		{
			return (UINT)0;
		}
		
		// call our test
		nThisTime = RunBltQueueSequence(nBlts);

		if( false == bMinTimeInitialized )
		{
			nMinTime = nThisTime;
			bMinTimeInitialized = true;
		}

		if( nThisTime< nMinTime)
			nMinTime = nThisTime;
	}

	return nMinTime;
}

//-----------------------------------------------------------------------------

//
// Perform a series of Blts and report the time the subsequent LockRect() blocked for
//
UINT BltQueue::RunBltQueueSequence(UINT nBlts)
{
	LARGE_INTEGER t1, t2;
	if( !QueryPerformanceCounter( &t1 ) )
		return 0;
	
	ZeroMemory( &t1, sizeof( t1 ) );
	ZeroMemory( &t2, sizeof( t2 ) );

	for( int i=0; i<nBlts; i++)
	{
		static FLOAT fRot=0.0f;
		m_pSrcDevice->Clear( 0, NULL, CLEARF_TARGET, (D3DCOLOR)0, 1.0f, 0);
		m_pSrcDevice->BeginScene();
		for(int j=0;j<20; j++)
						DrawSprite( 0.5f * (float)window_width, 0.5f * (float)window_height, (float)window_width, (float)window_height, fRot + (float)j*0.1f);
		m_pSrcDevice->EndScene();
		m_pSrcDevice->Present( 0L, 0L, 0L, 0L );

		fRot+=0.01f;
	}

	
	LOCKED_RECT lrc;
	// time the LockRect(), then of course unlock it.
	QueryPerformanceCounter( &t1 );
	m_pBackbuffer->LockRect( &lrc, 0L, (DWORD)0 );
	QueryPerformanceCounter( &t2 );
	m_pBackbuffer->UnlockRect( );

	return(UINT)(t2.LowPart - t1.LowPart);
}

//-----------------------------------------------------------------------------

bool BltQueue::SetDriverFlags(DWORD dwFlags, bool bValue)
{
	LONG    lResult;
	HKEY    hKey;

	// Open the key

	lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Direct3D"),
							0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 
							NULL, &hKey, NULL);
	if(lResult != ERROR_SUCCESS)
		return false;

	// Query for the value
   
	DWORD dwType, dwValue, dwSize = 4;

	// make sure the ForceDriverFlagsOn key is ON if requested.
	lResult = RegQueryValueEx(hKey, _T("ForceDriverFlagsOn"), 0, &dwType, (BYTE *)&dwValue, &dwSize);
	if(ERROR_SUCCESS != lResult)
	{
		dwValue = (bValue == true) ? dwFlags | 0 : 0 & (~dwFlags);
		m_bHadForceDriversOnKey = false;
		
		// Since this machine didn't have the value to begin with, we
		// must create one.

		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult) 
		{
			RegCloseKey(hKey);
			return false;
		}
	} 
	else 
	{
		// We already had a value, record it and change it
		m_bHadForceDriversOnKey = true;
		m_dwOldForceDriversOnValue = dwValue;
		dwValue = (bValue == true) ? dwFlags | dwValue : dwValue & (~dwFlags);
		
		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult)
		{
			RegCloseKey(hKey);
			return false;
		}
	}

	// make sure the ForceDriverFlagsOff key is OFF if requested.
	lResult = RegQueryValueEx(hKey, _T("ForceDriverFlagsOff"), 0, &dwType, (BYTE *)&dwValue, &dwSize);
	if(ERROR_SUCCESS != lResult)
	{
		dwValue = (bValue == true) ?  0 & (~dwFlags) : 0 | dwFlags ;
		m_bHadForceDriversOffKey = false;
		
		// Since this machine didn't have the value to begin with, we
		// must create one.

		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult) 
		{
			RegCloseKey(hKey);
			return false;
		}
	} 
	else 
	{
		// We already had a value, record it and change it
		m_bHadForceDriversOffKey = true;
		m_dwOldForceDriversOffValue = dwValue;
		dwValue = (bValue == true) ? dwValue & (~dwFlags) : dwValue | dwFlags;
		
		lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize);
		if(ERROR_SUCCESS != lResult)
		{
			RegCloseKey(hKey);
			return false;
		}
	}

	RegCloseKey(hKey);

	return true;
}

//-----------------------------------------------------------------------------

bool BltQueue::ResetDriverFlags()
{
	LONG    lResult;
	HKEY    hKey;
	bool	bRet = true;

	// Open the key

	lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Direct3D"),
							0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 
							NULL, &hKey, NULL);
	if(lResult != ERROR_SUCCESS)
		return false;

	// set the value
	DWORD dwType, dwValue, dwSize = 4;

	if( m_bHadForceDriversOnKey )
	{
		dwValue = m_dwOldForceDriversOnValue;
		bRet &= (lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOn"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize) != ERROR_SUCCESS);
	}
	else
	{
		bRet &= (lResult = RegDeleteValue( hKey, _T("ForceDriverFlagsOn") ) != ERROR_SUCCESS);
	}

	if( m_bHadForceDriversOffKey )
	{
		dwValue = m_dwOldForceDriversOffValue;
		bRet &= (lResult = RegSetValueEx(hKey, _T("ForceDriverFlagsOff"), 0, REG_DWORD, (BYTE *)&dwValue, dwSize) != ERROR_SUCCESS);
	}
	else
	{
		bRet &= (lResult = RegDeleteValue( hKey, _T("ForceDriverFlagsOff") ) != ERROR_SUCCESS);
	}

	RegCloseKey(hKey);

	return true;
}

//-----------------------------------------------------------------------------

bool BltQueue::DrawSprite(float center_x, float center_y, float width, float height, float theta)
{
	// rotate 45 degrees - easy transform for diagonals
	theta += D3DX_PI/4;
	
	// determine the distance from square center to any far corner
	float l = 0.5f * sqrt( width * width + height * height );

	// get the sin, cos for the transform
	float _cos = cos(theta);
	float _sin = sin(theta);
	
	// lock, populate, unlock, draw
	BLTQUEUE_VERTEX* pVerts = 0L;
	try
	{
		HRESULT hr;
		if(FAILED(hr = m_pVB->Lock(0,0,(BYTE**)&pVerts,0)))
			throw hr;
		memset(pVerts, 0, 4 * sizeof BLTQUEUE_VERTEX);
		pVerts[0].w = pVerts[1].w = pVerts[2].w = pVerts[3].w = 1.0f;
		pVerts[0].z = pVerts[1].z = pVerts[2].z = pVerts[3].z = 0.0f;
		pVerts[0].diffuse = pVerts[1].diffuse = pVerts[2].diffuse = pVerts[3].diffuse = 0xffffffff;
		// Setup a 2-prim tri-fan for dp.
		// It's harder for the HW to group fan DP calls
		// and in this case, we want to busy up the card
		// top vert
		pVerts[0].x = center_x - l * _sin;
		pVerts[0].y = center_y - l * _cos;
		pVerts[0].u = 0.f;
		pVerts[0].v = 0.f;
		// right vert
		pVerts[1].x = center_x + l * _cos;
		pVerts[1].y = center_y - l * _sin;
		pVerts[1].u = 1.f;
		pVerts[1].v = 0.f;
		// bottom vert
		pVerts[2].x = center_x + l * _sin;
		pVerts[2].y = center_y + l * _cos;
		pVerts[2].u = 1.f;
		pVerts[2].v = 1.f;
		// left vert
		pVerts[3].x = center_x - l * _cos;
		pVerts[3].y = center_y + l * _sin;
		pVerts[3].u = 0.f;
		pVerts[3].v = 1.f;
		if(FAILED(hr = m_pVB->Unlock()))
			throw hr;
		else
			pVerts = 0L;
		if(FAILED(hr = m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2)))
			throw hr;
	}
	catch(HRESULT hr)
	{
		if(pVerts)
		{
			m_pVB->Unlock();
			pVerts = 0L;
		}
		return false;
	}
	
	return true;		
}	

//-----------------------------------------------------------------------------

LRESULT	BltQueue::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	SetCursor( NULL );
	return TRUE; // prevent Windows from setting cursor to window class cursor
}
