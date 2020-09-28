//////////////////////////////////////////////////////////////////////
// File:  D3D9CustomTest.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a custom Direct3D 9 test group object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D9CustomTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
template < typename AppClass >
CD3D9CustomTest< AppClass >::CD3D9CustomTest< AppClass >
(
	AppClass * pApplication
)
:
	CCustomTest< AppClass, CTest >
	(
		pApplication
	),
	m_bDoD3D9L( TRUE ),
	m_hD3D9( NULL ),
	m_pD3D9( NULL ),
	m_pD3D9Ex( NULL ),
	m_pDirect3DCreate9( NULL ),
	m_pDirect3DCreate9Ex( NULL )
/*
	m_hD3D9RasterizerDLL( NULL )
*/
{
/*
	ZeroMemory( m_pD3D9RasterizerDLLName, sizeof( m_pD3D9RasterizerDLLName ) );
*/
};


////////////////////////////////////////////////////////////
//
// Direct3D initialization/cleanup
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LoadD3D9
//
// Purpose:
// Loads D3D9.dll, and gets a pointer to the
// Direct3DCreate9Ex API.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D9CustomTest< AppClass >::LoadD3D9()
{
	HRESULT hOverallResult = E_FAIL;


	//
	// Load D3D9.dll.
	//

	m_hD3D9 = LoadLibraryExW( L"d3d9.dll", NULL, 0 );

	if ( m_hD3D9 == NULL )
	{
		hOverallResult = D3DERR_NOTAVAILABLE;
		goto Cleanup;
	};


	//
	// Get a pointer to our initializer API.
	//

	if ( m_bDoD3D9L == TRUE )
	{
		m_pDirect3DCreate9Ex = (PDIRECT3DCREATE9EX) GetProcAddress
		(
			m_hD3D9,
			"Direct3DCreate9Ex"
		);

		if ( m_pDirect3DCreate9Ex == NULL )
		{
			hOverallResult = D3DERR_NOTAVAILABLE;
			goto Cleanup;
		};
	}
	else
	{
		m_pDirect3DCreate9 = (PDIRECT3DCREATE9) GetProcAddress
		(
			m_hD3D9,
			"Direct3DCreate9"
		);

		if ( m_pDirect3DCreate9 == NULL )
		{
			hOverallResult = D3DERR_NOTAVAILABLE;
			goto Cleanup;
		};
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	if ( FAILED( hOverallResult ) )
	{
		//
		// Free D3D9.dll.
		//

		if ( m_hD3D9 != NULL )
		{
			m_pDirect3DCreate9 = NULL;
			m_pDirect3DCreate9Ex = NULL;

			FreeLibrary( m_hD3D9 );
			m_hD3D9 = NULL;
		};
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  UnloadD3D9
//
// Purpose:
// Unloads D3D9.dll.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D9CustomTest< AppClass >::UnloadD3D9()
{
	//
	// Unload D3D9.dll.
	//

	m_pDirect3DCreate9 = NULL;
	m_pDirect3DCreate9Ex = NULL;

	if ( m_hD3D9 != NULL )
	{
		FreeLibrary( m_hD3D9 );
		m_hD3D9 = NULL;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  InitializeD3D9
//
// Purpose:
// Initializes Direct3D and creates the IDirect3D9Ex
// enumerator object.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D9CustomTest< AppClass >::InitializeD3D9
(
	bool bDebugHelper
)
{
	//
	// Initialize Direct3D.
	//

	if ( m_bDoD3D9L == TRUE )
	{
		if ( m_pDirect3DCreate9Ex == NULL )
		{
			WriteToLog( _T( "InitializeD3D9:  Direct3DCreate9Ex API not found." ) );

			return E_FAIL;
		};


		HRESULT hCreateD3DExResult = (*m_pDirect3DCreate9Ex)
		(
			D3D_SDK_VERSION | ( bDebugHelper == true ? 0x80000000 : 0 ),
			&m_pD3D9Ex
		);

		if ( FAILED( hCreateD3DExResult ) )
		{
			if ( hCreateD3DExResult == D3DERR_NOTAVAILABLE )
			{
				WriteToLog
				(
					_T( "InitializeD3D9:  Direct3DCreate9Ex failed with error D3DERR_NOTAVAILABLE.  " )
					_T ("The graphics adapter probably does not have LDDM drivers installed." )
				);
			}
			else
			{
				WriteToLog
				(
					_T( "InitializeD3D9:  Direct3DCreate9Ex failed with error %u (0x%x)." ),
					hCreateD3DExResult,
					hCreateD3DExResult
				);
			};

			return hCreateD3DExResult;
		};

		if ( m_pD3D9Ex == NULL )
		{
			WriteToLog( _T( "InitializeD3D9:  m_pD3D9Ex == NULL." ) );

			return E_FAIL;
		};
	}
	else
	{
		if ( m_pDirect3DCreate9 == NULL )
		{
			WriteToLog( _T( "InitializeD3D9:  Direct3DCreate9 API not found." ) );

			return E_FAIL;
		};


		m_pD3D9 = (*m_pDirect3DCreate9)
		(
			D3D_SDK_VERSION | ( bDebugHelper == true ? 0x80000000 : 0 )
		);

		if ( m_pD3D9 == NULL )
		{
			WriteToLog
			(
				_T( "InitializeD3D9:  Direct3DCreate9 failed." )
			);

			return E_FAIL;
		};
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  CleanupD3D9
//
// Purpose:
// Releases the IDirect3D9Ex enumerator object.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D9CustomTest< AppClass >::CleanupD3D9()
{
	//
	// Release our enumerator object.
	//

	if ( m_pD3D9Ex != NULL )
	{
		m_pD3D9Ex->Release();
		m_pD3D9Ex = NULL;
	};

	if ( m_pD3D9 != NULL )
	{
		m_pD3D9->Release();
		m_pD3D9 = NULL;
	};

	return S_OK;
};


////////////////////////////////////////////////////////////
//
// Device management
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CleanupResources
//
// Purpose:
// Cleans up Direct3D 9 resources created by the
// custom test group layer.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D9CustomTest< AppClass >::CleanupResources()
{
/*
	//
	// Release the rasterizer DLL.
	//

	if ( m_hD3D9RasterizerDLL != NULL )
	{
		FreeLibrary( m_hD3D9RasterizerDLL );
		m_hD3D9RasterizerDLL = NULL;
	};
*/


	return S_OK;
};


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  HResultToString
//
// Purpose:
// Returns a stringized HRESULT.
///////////////////////////////////////////////////////
template < typename AppClass >
LPCTSTR CD3D9CustomTest< AppClass >::HResultToString
(
	IN HRESULT hResult
) const
{
	switch ( hResult )
	{
		//
		// Direct3D 9 status codes
		//

		HANDLE_HRESULT( D3DERR_WRONGTEXTUREFORMAT );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDCOLOROPERATION );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDCOLORARG );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDALPHAOPERATION );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDALPHAARG );
		HANDLE_HRESULT( D3DERR_TOOMANYOPERATIONS );
		HANDLE_HRESULT( D3DERR_CONFLICTINGTEXTUREFILTER );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDFACTORVALUE );
		HANDLE_HRESULT( D3DERR_CONFLICTINGRENDERSTATE );
		HANDLE_HRESULT( D3DERR_UNSUPPORTEDTEXTUREFILTER );
		HANDLE_HRESULT( D3DERR_CONFLICTINGTEXTUREPALETTE );
		HANDLE_HRESULT( D3DERR_DRIVERINTERNALERROR );

		HANDLE_HRESULT( D3DERR_NOTFOUND );
		HANDLE_HRESULT( D3DERR_MOREDATA );
		HANDLE_HRESULT( D3DERR_DEVICELOST );
		HANDLE_HRESULT( D3DERR_DEVICENOTRESET );
		HANDLE_HRESULT( D3DERR_NOTAVAILABLE );
		HANDLE_HRESULT( D3DERR_OUTOFVIDEOMEMORY );
		HANDLE_HRESULT( D3DERR_INVALIDDEVICE );
		HANDLE_HRESULT( D3DERR_INVALIDCALL );
		HANDLE_HRESULT( D3DERR_DRIVERINVALIDCALL );
		HANDLE_HRESULT( D3DERR_WASSTILLDRAWING );

		HANDLE_HRESULT( D3DERR_DEVICEREMOVED );
		HANDLE_HRESULT( D3DOK_NOAUTOGEN );
		HANDLE_HRESULT( S_NOT_RESIDENT );
		HANDLE_HRESULT( S_RESIDENT_IN_SHARED_MEMORY );
		HANDLE_HRESULT( S_PRESENT_MODE_CHANGED );
		HANDLE_HRESULT( S_PRESENT_OCCLUDED );
		HANDLE_HRESULT( D3DERR_DEVICEHUNG );


		//
		// D3D DDI status codes
		//
		// N.B. [SEdmison]:
		// The first few share values with the API error codes.
		//

		//HANDLE_HRESULT( D3DDDIERR_WASSTILLDRAWING );
		//HANDLE_HRESULT( D3DDDIERR_NOTAVAILABLE );
		//HANDLE_HRESULT( D3DDDIERR_DEVICEREMOVED );
		HANDLE_HRESULT( D3DDDIERR_PRIVILEGEDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDHANDLE );
		HANDLE_HRESULT( D3DDDIERR_CANTEVICTPINNEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_CANTRENDERLOCKEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDUSERBUFFER );
		HANDLE_HRESULT( D3DDDIERR_INCOMPATIBLEPRIVATEFORMAT );
		HANDLE_HRESULT( D3DDDIERR_UNSUPPORTEDOVERLAY );
		HANDLE_HRESULT( D3DDDIERR_UNSUPPORTEDOVERLAYFORMAT );


		//
		// All other status codes
		//

		default:
		{
			return CCustomTest::HResultToString( hResult );
		} break;
	};
};
