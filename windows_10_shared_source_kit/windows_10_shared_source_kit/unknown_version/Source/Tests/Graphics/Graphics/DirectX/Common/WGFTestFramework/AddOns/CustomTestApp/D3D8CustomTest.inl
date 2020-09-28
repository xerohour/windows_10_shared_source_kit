//////////////////////////////////////////////////////////////////////
// File:  D3D8CustomTest.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a custom Direct3D 8 test group object.
//
// History:
// 25 Oct 2007  SEdmison  Created.
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
// Function:  CD3D8CustomTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
template < typename AppClass >
CD3D8CustomTest< AppClass >::CD3D8CustomTest< AppClass >
(
	AppClass * pApplication
)
:
	CCustomTest< AppClass, CTest >
	(
		pApplication
	),
	m_hD3D8( NULL ),
	m_pD3D8( NULL ),
	m_pDirect3DCreate8( NULL )
{
};


////////////////////////////////////////////////////////////
//
// Direct3D initialization/cleanup
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LoadD3D8
//
// Purpose:
// Loads D3D8.dll, and gets a pointer to the
// Direct3DCreate8 API.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D8CustomTest< AppClass >::LoadD3D8()
{
	HRESULT hOverallResult = E_FAIL;


	//
	// Load D3D8.dll.
	//

	m_hD3D8 = LoadLibraryA( "d3d8.dll" );

	if ( m_hD3D8 == NULL )
	{
		hOverallResult = D3DERR_NOTAVAILABLE;
		goto Cleanup;
	};


	//
	// Get a pointer to our initializer API.
	//

	m_pDirect3DCreate8 = (PDIRECT3DCREATE8) GetProcAddress
	(
		m_hD3D8,
		"Direct3DCreate8"
	);

	if ( m_pDirect3DCreate8 == NULL )
	{
		hOverallResult = D3DERR_NOTAVAILABLE;
		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	if ( FAILED( hOverallResult ) )
	{
		//
		// Free D3D8.dll.
		//

		if ( m_hD3D8 != NULL )
		{
			m_pDirect3DCreate8 = NULL;

			FreeLibrary( m_hD3D8 );
			m_hD3D8 = NULL;
		};
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  UnloadD3D8
//
// Purpose:
// Unloads D3D8.dll.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D8CustomTest< AppClass >::UnloadD3D8()
{
	//
	// Unload D3D8.dll.
	//

	m_pDirect3DCreate8 = NULL;

	if ( m_hD3D8 != NULL )
	{
		FreeLibrary( m_hD3D8 );
		m_hD3D8 = NULL;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  InitializeD3D8
//
// Purpose:
// Initializes Direct3D and creates the IDirect3D8
// enumerator object.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D8CustomTest< AppClass >::InitializeD3D8
(
	bool bDebugHelper
)
{
	//
	// Initialize Direct3D.
	//

	if ( m_pDirect3DCreate8 == NULL )
	{
		WriteToLog( _T( "InitializeD3D8:  Direct3DCreate8 API not found." ) );

		return E_FAIL;
	};


	m_pD3D8 = (*m_pDirect3DCreate8)
	(
		D3D_SDK_VERSION | ( bDebugHelper == true ? 0x80000000 : 0 )
	);

	if ( m_pD3D8 == NULL )
	{
		WriteToLog
		(
			_T( "InitializeD3D8:  Direct3DCreate8 failed." )
		);

		return E_FAIL;
	};


	return S_OK;
};


///////////////////////////////////////////////////////
// Function:  CleanupD3D8
//
// Purpose:
// Releases the IDirect3D8 enumerator object.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D8CustomTest< AppClass >::CleanupD3D8()
{
	//
	// Release our enumerator object.
	//

	if ( m_pD3D8 != NULL )
	{
		m_pD3D8->Release();
		m_pD3D8 = NULL;
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
// Cleans up Direct3D 8 resources created by the
// custom test group layer.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CD3D8CustomTest< AppClass >::CleanupResources()
{
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
LPCTSTR CD3D8CustomTest< AppClass >::HResultToString
(
	IN HRESULT hResult
) const
{
	switch ( hResult )
	{
		//
		// Direct3D 8 status codes
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


		//
		// All other status codes
		//

		default:
		{
			return CCustomTest::HResultToString( hResult );
		} break;
	};
};
