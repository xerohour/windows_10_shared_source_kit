//////////////////////////////////////////////////////////////////////
// File:  D3D9CustomTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for custom D3D 9 test group object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////

//
// D3D9.dll API prototypes.
//

typedef IDirect3D9 * (WINAPI *PDIRECT3DCREATE9)
(
	UINT SDKVersion
);

typedef HRESULT (WINAPI *PDIRECT3DCREATE9EX)
(
	UINT SDKVersion,
	IDirect3D9Ex **
);


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D9CustomTest
//
// Purpose:
// Direct3D 9 custom test group object.
///////////////////////////////////////////////////////
template < typename AppClass >
class CD3D9CustomTest
:
	public CCustomTest< AppClass, CTest >
{
	//
	// Construction/destruction
	//

	public:
		CD3D9CustomTest
		(
			AppClass * pApplication
		);

		//virtual ~CD3D9CustomTest();


	//
	// Direct3D initialization and cleanup methods.
	//

	public:
		HRESULT LoadD3D9();

		HRESULT UnloadD3D9();

		HRESULT InitializeD3D9
		(
			bool bDebugHelper = false
		);

		HRESULT CleanupD3D9();


	//
	// Device management functions
	//

	public:
		// Frees Direct3D 9 resources.
		HRESULT CleanupResources();


	//
	// Miscellaneous helper methods
	//

	public:
		virtual __override LPCTSTR HResultToString
		(
			IN HRESULT hResult
		) const;


	//
	// Data members
	//

	public:
		//
		// Direct3D 9 members
		//

		BOOL m_bDoD3D9L;

		IDirect3D9 * m_pD3D9;
		IDirect3D9Ex * m_pD3D9Ex;

		// D3D9.dll handle and entry point.
		HMODULE m_hD3D9;
		PDIRECT3DCREATE9 m_pDirect3DCreate9;
		PDIRECT3DCREATE9EX m_pDirect3DCreate9Ex;

/*
		// Rasterizer DLL name and handle.
		TCHAR m_pD3D9RasterizerDLLName[ MAX_PATH ];
		HMODULE m_hD3D9RasterizerDLL;
*/
};


