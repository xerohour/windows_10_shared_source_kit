//////////////////////////////////////////////////////////////////////
// File:  LockPointerAlias.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Performs a series of lock and write operations on vertex and
// iIndex buffers under the following conditions:
//     After initialization
//     After DrawPrimitive  (This case needs improvement.)
//     Device lost while locked
//     Lock after device lost
// A wide variety of USAGES, POOLS and lock flags are tested.
//
// History:
// 27 Oct 2005  SEdmison  Cleaned up.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "CBuffers.h"
#include "CVertexIndexBufferTest.h"
#include <d3dx8.h>
#include <types.h>


/////////////////////////////////////////////////////////////////
//
// Globals and statics
//
/////////////////////////////////////////////////////////////////

extern CD3DWindowFramework *g_pD3DFramework;

#if FAST_TEST
const DWORD LockPointerAlias::Usages[] =
{
    0,
    USAGE_WRITEONLY,
    USAGE_DONOTCLIP,
    USAGE_POINTS,
    USAGE_RTPATCHES,
    USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_WRITEONLY | USAGE_DYNAMIC,
    USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY,
    USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_POINTS,
    USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES,
    USAGE_SOFTWAREPROCESSING | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP | USAGE_DYNAMIC,
    USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};


const DWORD LockPointerAlias::Pools[] =
{
    //POOL_DEFAULT,
    POOL_MANAGED,
    POOL_SYSTEMMEM,
};


const DWORD LockPointerAlias::LockFlags[] =
{
    0,
    LOCK_DISCARD,
    LOCK_NOSYSLOCK,
    LOCK_DISCARD | LOCK_NOSYSLOCK,
};
#else //FAST_TEST

const DWORD LockPointerAlias::Usages[] =
{
	0,
	USAGE_WRITEONLY,
	USAGE_DONOTCLIP,
	USAGE_POINTS,
	USAGE_RTPATCHES,
	USAGE_NPATCHES,
	USAGE_DYNAMIC,
	USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_WRITEONLY | USAGE_DYNAMIC,
	USAGE_DONOTCLIP | USAGE_DYNAMIC,
	USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY,
	USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS,
	USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_RTPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_NPATCHES | USAGE_WRITEONLY | USAGE_DONOTCLIP,
	USAGE_SOFTWAREPROCESSING | USAGE_POINTS | USAGE_RTPATCHES | USAGE_NPATCHES,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_DONOTCLIP | USAGE_DYNAMIC,
	USAGE_SOFTWAREPROCESSING | USAGE_WRITEONLY | USAGE_DONOTCLIP | USAGE_DYNAMIC,
};


const DWORD LockPointerAlias::Pools[] =
{
	//POOL_DEFAULT,
	POOL_MANAGED,
	POOL_SYSTEMMEM,
};


const DWORD LockPointerAlias::LockFlags[] =
{
	0,
	LOCK_DISCARD,
	LOCK_NOSYSLOCK,
	LOCK_DISCARD | LOCK_NOSYSLOCK,
};

#endif //FAST_TEST

static LockPointerAliasTestCase s_LockPointerAliasTestCases
[
	COUNTOF( LockPointerAlias::Usages ) *
	COUNTOF( LockPointerAlias::Pools ) *
	COUNTOF( LockPointerAlias::LockFlags ) *
	2 // 2 possible values for DoIndex
];

#ifndef FAST_TEST //Too slow
// create instance of this class
LockPointerAlias tstLockPointerAlias;
#endif


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction / destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LockPointerAlias
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
LockPointerAlias::LockPointerAlias()
{
	m_szTestName = _T("VBIB Lock Pointer Alias");
	m_szCommandKey = _T("LockPointerAlias");

	SetTestType( TESTTYPE_CONF );
	m_Options.D3DOptions.bReference         = false;
	m_Options.TestOptions.bCompareSrcRef    = false;
	m_Options.D3DOptions.bZBuffer           = false;
	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
	m_Options.D3DOptions.dwBehaviors = DEVICECREATE_FPU_PRESERVE | DEVICECREATE_HWVP | DEVICECREATE_STANDARD | DEVICECREATE_MIXED | DEVICECREATE_PURE;
	m_Options.D3DOptions.bCachedBackBuffer    = false;
	m_Options.D3DOptions.fMinDXVersion = 8.0f;
	m_Options.D3DOptions.fMaxDXVersion = 9.0f;

	m_Options.D3DOptions.dwIncVersions = DXVERSION_8_OR_81 | DXVERSION_9;

	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_8_OR_81;


	m_dwSW = FALSE;
	m_dwBehaviorOverride = 0;
}


////////////////////////////////////////////////////////////
//
// Framework method overrides
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  TestInitialize
//
// Purpose:
// Test group initialization.
///////////////////////////////////////////////////////
UINT LockPointerAlias::TestInitialize()
{
	//
	// Determine whether software vertex processing was specified
	// for device type mixed.
	//

	m_dwBehaviorOverride = m_pSrcDevice->GetBehaviors();
	if( m_dwBehaviorOverride & DEVICECREATE_MIXED )
	{
		m_dwSW = m_pSrcDevice->GetSoftwareVertexProcessing();
		//m_pSrcDevice->GetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, &m_dwSW );
	};


	//
	// Count the number of valid scenarios, and build our
	// scenario data list.
	//

	UINT cTestCases = 0;
	for ( UINT iCurrentUsage = 0; iCurrentUsage < COUNTOF( Usages ); ++iCurrentUsage )
	{
		for ( UINT iCurrentPool = 0; iCurrentPool < COUNTOF( Pools ); ++iCurrentPool )
		{
			for ( UINT iCurrentLockFlag = 0; iCurrentLockFlag < COUNTOF( LockFlags ); ++iCurrentLockFlag )
			{
				for ( UINT CurrentIndex = 0; CurrentIndex < 2; ++CurrentIndex )
				{
					// Skip invalid combinations of parameters:
					if
					(
						// Usage dynamic is not valid on managed buffers
						(
							( Usages[ iCurrentUsage ] & USAGE_DYNAMIC ) &&
							( Pools[ iCurrentPool ] == POOL_MANAGED )
						) ||

						// DISCARD and NOOVERWRITE are only valid for dynamic buffers
						(
							!( Usages[ iCurrentUsage ] & USAGE_DYNAMIC) &&
							(
								( LockFlags[ iCurrentLockFlag ] & LOCK_DISCARD ) ||
								( LockFlags[ iCurrentLockFlag ] & LOCK_NOOVERWRITE )
							)
						) ||

						// Software usage is not valid on hardware device
						(
							( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) &&
							( m_dwBehaviorOverride & DEVICECREATE_HWVP )
						) ||

						// Software usage is required on mixed device with SW
						// vertex processing when pool is not sysmem.
						(
							!( Usages[ iCurrentUsage ] & USAGE_SOFTWAREPROCESSING ) &&
							( m_dwBehaviorOverride & DEVICECREATE_MIXED ) &&
							m_dwSW &&
							Pools[ iCurrentPool ] != POOL_SYSTEMMEM
						)
					)
					{
						continue;
					};


					ASSERT( cTestCases < COUNTOF( s_LockPointerAliasTestCases ) );

					s_LockPointerAliasTestCases[ cTestCases ].nTest = cTestCases;

					s_LockPointerAliasTestCases[ cTestCases ].Usage = Usages[ iCurrentUsage ];
					s_LockPointerAliasTestCases[ cTestCases ].Pool = Pools [ iCurrentPool ];
					s_LockPointerAliasTestCases[ cTestCases ].LockFlags = LockFlags[ iCurrentLockFlag ];
					s_LockPointerAliasTestCases[ cTestCases ].DoIndex = CurrentIndex;

					++cTestCases;
				};
			};
		};
	};


	//
	// Set the test range.
	//

	SetTestRange
	(
		1,
		cTestCases
	);


	if( !(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) )
	{
		WriteToLog( TEXT("D3DDEVCAPS_HWTRANSFORMANDLIGHT not supported\n") );
		return D3DTESTINIT_SKIPALL;
	}


	m_DevType = m_pSrcDevice->GetDevType();
	m_dwAdapterID = m_pSrcDevice->GetAdapterID();

	ReleaseDevices();

	return D3DTESTINIT_RUN;
}


///////////////////////////////////////////////////////
// Function:  ExecuteTest
//
// Purpose:
// Runs a test case.
///////////////////////////////////////////////////////
bool LockPointerAlias::ExecuteTest(UINT nTest)
{
	//
	// Validate the test case ordinal.
	//

	if ( nTest > COUNTOF( s_LockPointerAliasTestCases ) )
	{
		WriteToLog
		(
			_T( "Invalid test ordinal %u." ),
			nTest
		);

		return false;
	};


	//
	// Look up test case data.
	//

	const UINT Size = 4096+4096+4096+256;
	const DWORD FVF = D3DFVF_VERTEX;
	const FMT IBFormat = FMT_INDEX16;

	UINT Usage = s_LockPointerAliasTestCases[ nTest - 1 ].Usage;
	DWORD Pool = s_LockPointerAliasTestCases[ nTest - 1 ].Pool;
	DWORD Flags = s_LockPointerAliasTestCases[ nTest - 1 ].LockFlags;
	BOOL DoIndex = ( s_LockPointerAliasTestCases[ nTest - 1 ].DoIndex == 1) ? TRUE : FALSE;

	TCHAR szDeviceName[32] = _T( "" );
	int iIndex;


	//
	// Build the test case name.
	//

	TCHAR sz[ 256 ] = _T( "" );
	wsprintf
	(
		sz,
		TEXT("Usage = 0x%x, Pool = %d, LockFlags = 0x%x, Index Buffer = %s"),
		Usage,
		Pool,
		Flags,
		DoIndex ? TEXT("true") : TEXT("false")
	);


	//
	// Begin the test case.
	//

	BeginTestCase( sz, nTest );


	//
	// Clear the frame in preparation for rendering.
	//

	ClearFrame();


	HRESULT hr = S_OK;
	PRESENT_PARAMETERS Param;
	DISPLAYMODE Mode;
	LPDEVICE pDevice = NULL;
	LPVERTEXBUFFER pVB = NULL;
	LPINDEXBUFFER  pIB = NULL;
	LPBYTE pbVBData = NULL;
	LPBYTE pbIBData = NULL;

	LONG lRet;
	DEVMODE dmcur;
	DEVMODE dmnew;

	memset( &dmcur, 0, sizeof( dmcur ) );
	dmcur.dmSize = sizeof(dmcur);

	hr = m_pD3D->GetAdapterDisplayMode( m_dwAdapterID, &Mode );
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("GetAdapterDisplayMode() returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);
		Abort();
		return(false);
	}

	memset( &Param, 0, sizeof(PRESENT_PARAMETERS) );

	Param.uWidth = GetCurrentMode()->pDisplay->dwWidth;
	Param.uHeight = GetCurrentMode()->pDisplay->dwHeight;
	Param.BackBufferFormat = GetCurrentMode()->bWindowed == false ? GetCurrentMode()->pDisplay->Format : Mode.Format;
	Param.DesktopFormat = Mode.Format;
	Param.uBackBufferCount = 1;
	Param.MultiSample = MULTISAMPLE_NONE;
	Param.SwapEffect = D3DSWAPEFFECT_COPY;
	Param.hDeviceWindow = g_pD3DFramework->m_hWindow;
	Param.bWindowed = GetCurrentMode()->bWindowed;
	Param.bAutoDepthStencil = m_Options.D3DOptions.bZBuffer;
	Param.AutoDepthStencilFormat = m_Options.D3DOptions.bZBuffer == false ? FMT_UNKNOWN : m_pCommonZBufferFormats[GetCurrentMode()->nZBufferFormat];
	Param.uFullscreenRefresh = GetCurrentMode()->bWindowed == false ?  GetCurrentMode()->pDisplay->dwRefreshRate : 0;
	Param.uFullscreenPresentInterval = 0;
	Param.dwFlags = (Param.MultiSample == MULTISAMPLE_NONE) ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;

	hr = m_pD3D->CreateDevice(  m_dwAdapterID,
								m_DevType,
								Param.hDeviceWindow,
								m_dwBehaviorOverride,
								&Param,
								&pDevice
								);
	if( FAILED(hr) )
	{
		WriteToLog(
			TEXT("CreateDevice() returned %s (0x%x)\n"),
			m_pD3D->HResultToString(hr),
			hr
		);
		Skip();
		return(false);
	}


	if( m_dwBehaviorOverride & DEVICECREATE_MIXED )
	{
		if ( FAILED( hr = pDevice->SetSoftwareVertexProcessing( m_dwSW ) ) )
		//if ( FAILED(hr = pDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, m_dwSW) ) )
		{
			WriteToLog(
				TEXT("SetRenderState() returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			Abort();
			return(false);
		}
	}

	do
	{
		do
		{
			if( !GetDisplayString( m_dwAdapterID, szDeviceName, 32 ) )
			{
				WriteToLog( TEXT("GetDisplayString failed.\n") );
				hr = E_FAIL;
				continue;
			}

			lRet = EnumDisplaySettings( szDeviceName, ENUM_CURRENT_SETTINGS, &dmcur );
			if( !lRet )
			{
				WriteToLog(
					TEXT("EnumDisplaySettings() returned %d\n"),
					lRet
				);
				hr = E_FAIL;
				continue;
			}

			DWORD dw = 0;
			do
			{
				memset( &dmnew, 0, sizeof( dmnew ) );
				dmnew.dmSize = sizeof(dmnew);
        
				lRet = EnumDisplaySettings(
					szDeviceName,
					dw++,
					&dmnew
				);

				if( !lRet )
				{
					WriteToLog(
						TEXT("EnumDisplaySettings() returned %d\n"),
						lRet
					);
					hr = E_FAIL;
					continue;
				}
			}
			while( (dmnew.dmPelsWidth == dmcur.dmPelsWidth &&
					dmnew.dmPelsHeight == dmcur.dmPelsHeight && 
					dmnew.dmBitsPerPel == dmcur.dmBitsPerPel) || 
					(dmnew.dmBitsPerPel == 4) 
				  );

			dmnew.dmSize = sizeof(dmnew);
			dmnew.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

			hr = pDevice->CreateVertexBuffer(
				Size,
				Usage,
				FVF,
				Pool,
				&pVB
			);
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("CreateVertexBuffer(%d,0x%x,0x%x,%d,) failed, returned %s (0x%x)\n"),
					Size,
					Usage,
					FVF,
					Pool,
					m_pD3D->HResultToString(hr),
					hr
				);
				continue;
			}

			hr = pDevice->SetStreamSource( 0, pVB, sizeof(D3DVERTEX) );
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("SetStreamSource() failed, returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				continue;
			}

			if( DoIndex )
			{
				hr = pDevice->CreateIndexBuffer(
					Size,
					Usage,
					IBFormat,
					Pool,
					&pIB
				);
				if( FAILED(hr) )
				{
					WriteToLog(
						TEXT("CreateIndexBuffer(%d,0x%x,%d,%d,) failed, returned %s (0x%x)\n"),
						Size,
						Usage,
						IBFormat,
						Pool,
						m_pD3D->HResultToString(hr),
						hr
					);
					continue;
				}

				hr = pDevice->SetIndices( pIB, 0 );
				if( FAILED(hr) )
				{
					WriteToLog(
						TEXT("SetIndices() failed, returned %s (0x%x)\n"),
						m_pD3D->HResultToString(hr),
						hr
					);
					continue;
				}
			}

			pDevice->SetRenderState( D3DRS_CLIPPING, (Usage & USAGE_DONOTCLIP) ? FALSE : TRUE );

			hr = pDevice->SetVertexShader( D3DFVF_VERTEX );
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("SetVertexShader() failed, returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				continue;
			}

			hr = pDevice->BeginScene();
			if( FAILED(hr) )
			{
				WriteToLog(
					TEXT("BeginScene() failed, returned %s (0x%x)\n"),
					m_pD3D->HResultToString(hr),
					hr
				);
				continue;
			}
		}
		while(0);

		if( FAILED(hr) )
		{
			Abort();
			continue;
		}


		do
		{
			try
			{
				WriteToLog( TEXT("VB::Lock(0,0,,0x%x)\n"), Flags );
				hr = pVB->Lock( 0, 0, &pbVBData, Flags );
				if( FAILED(hr) )
				{
					WriteToLog(
						TEXT("...returned %s (0x%x)\n"),
						m_pD3D->HResultToString(hr),
						hr
					);
				}

				WriteToLog( TEXT("writing to pointer (%x) from VB::Lock()\n"), pbVBData );
				for( iIndex = 0; iIndex < Size; iIndex++ )
				{
					pbVBData[iIndex] = 0;
				}

				WriteToLog( TEXT("VB::Unlock()\n") );
				pVB->Unlock();

				if( DoIndex )
				{
					WriteToLog( TEXT("IB::Lock(0,0,,0x%x)\n"), Flags );
					hr = pIB->Lock( 0, 0, &pbIBData, Flags );
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}

					WriteToLog( TEXT("writing to pointer from IB::Lock()\n") );
					for(iIndex = 0; iIndex < Size; iIndex++ )
					{
						pbIBData[iIndex] = 0x0;
					}

					WriteToLog( TEXT("IB::Unlock()\n") );
					pIB->Unlock();
				}

				// draw object, to cause DMA on buffer to start (hopefully)
				if( !DoIndex )
				{
					WriteToLog( TEXT("DrawPrimitive()\n") );
					hr = pDevice->DrawPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}
				else
				{
					WriteToLog( TEXT("DrawIndexedPrimitive()\n") );
					hr = pDevice->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}

				WriteToLog( TEXT("VB::Lock(0,0,,0x%x)\n"), Flags );
				hr = pVB->Lock( 0, 0, &pbVBData, Flags );
				if( FAILED(hr) )
				{
					WriteToLog(
						TEXT("...returned %s (0x%x)\n"),
						m_pD3D->HResultToString(hr),
						hr
					);
				}

				WriteToLog( TEXT("writing to pointer (0x%x) from VB::Lock()\n"), pbVBData );
				for(iIndex = 0; iIndex < Size; iIndex++ )
				{
					pbVBData[iIndex] = 0x49;
				}

				if( DoIndex )
				{
					WriteToLog( TEXT("IB::Lock(0,0,,0x%x)\n"), Flags );
					hr = pIB->Lock( 0, 0, &pbIBData, Flags );
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}

					WriteToLog( TEXT("writing to pointer from IB::Lock()\n") );
					for(iIndex = 0; iIndex < Size; iIndex++ )
					{
						pbIBData[iIndex] = 0x49;
					}
				}

				// cause device loss condition by changing modes
				WriteToLog( TEXT("Calling ChangeDisplayMode.\n") );

				if ( false == LoseTheDevice( D3DLOSEDEV_BYOTHERPROCESS ) )
				{
					WriteToLog( TEXT("LoseTheDevice failed.\n") );
					Abort();
					continue;
				};

				// check if device lost
				WriteToLog( TEXT("TestCooperativeLevel()\n") );
				hr = pDevice->TestCooperativeLevel();
				if(
					(hr != D3DERR_DEVICELOST) &&
					(hr != D3DERR_DEVICENOTRESET)
				)
				{
					WriteToLog( TEXT("Device is not lost; cannot continue.\n") );
					Abort();
					continue;
				}
				GetLastError();

				WriteToLog( TEXT("writing to pointer (0x%x) from VB::Lock() after device lost\n"), pbVBData );
				for( iIndex = 0; iIndex < Size; iIndex++ )
				{
					pbVBData[iIndex] = 0;
				}

				WriteToLog( TEXT("VB::Unlock()\n") );
				pVB->Unlock();

				if( DoIndex )
				{
					WriteToLog( TEXT("writing to pointer from IB::Lock() after device lost\n") );
					for(iIndex = 0; iIndex < Size; iIndex++ )
					{
						pbIBData[iIndex] = 0;
					}

					WriteToLog( TEXT("IB::Unlock()\n") );
					pIB->Unlock();
				}

				if( !DoIndex )
				{
					WriteToLog( TEXT("DrawPrimitive()\n") );
					hr = pDevice->DrawPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}
				else
				{
					WriteToLog( TEXT("DrawIndexedPrimitive()\n") );
					hr = pDevice->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}

				WriteToLog( TEXT("VB::Lock(0,0,,0x%x)\n"), Flags );
				hr = pVB->Lock( 0, 0, &pbIBData, Flags );
				if( FAILED(hr) )
				{
					WriteToLog(
						TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
						hr
					);
				}

				WriteToLog( TEXT("writing to pointer from VB::Lock()\n") );
				for( iIndex = 0; iIndex < Size; iIndex++ )
				{
					pbIBData[iIndex] = 0;
				}

				WriteToLog( TEXT("VB::Unlock()\n") );
				pVB->Unlock();

				if( DoIndex )
				{
					WriteToLog( TEXT("IB::Lock(0,0,,0x%x)\n"), Flags );
					hr = pIB->Lock( 0, 0, &pbIBData, Flags );
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}

					WriteToLog( TEXT("writing to pointer from IB::Lock()\n") );
					for(iIndex = 0; iIndex < Size; iIndex++ )
					{
						pbIBData[iIndex] = 0;
					}

					WriteToLog( TEXT("IB::Unlock()\n") );
					pIB->Unlock();
				}

				if( !DoIndex )
				{
					WriteToLog( TEXT("DrawPrimitive()\n") );
					hr = pDevice->DrawPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}
				else
				{
					WriteToLog( TEXT("DrawIndexedPrimitive()\n") );
					hr = pDevice->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						1,
						0,
						1
					);
					if( FAILED(hr) )
					{
						WriteToLog(
							TEXT("...returned %s (0x%x)\n"),
							m_pD3D->HResultToString(hr),
							hr
						);
					}
				}
			}
			catch(...)
			{
				WriteToLog(TEXT("...caused unhandled exception\n"));
//                WriteToLog(TEXT("Index: %d/%d\n"), iIndex, Size);

				Fail();
			}
		}
		while(0);

		hr = pDevice->EndScene();
		if( FAILED(hr) )
		{
			WriteToLog(
				TEXT("EndScene() failed, returned %s (0x%x)\n"),
				m_pD3D->HResultToString(hr),
				hr
			);
			Fail();
			continue;
		}
	}
	while(0);


	RELEASE(pVB);
	RELEASE(pIB);
	if( pDevice )
		pDevice->ReleaseD3D();
	RELEASE(pDevice);


	return false;
};
