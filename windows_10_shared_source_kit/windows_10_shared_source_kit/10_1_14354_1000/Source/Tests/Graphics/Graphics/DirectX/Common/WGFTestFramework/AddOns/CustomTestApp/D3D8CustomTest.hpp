//////////////////////////////////////////////////////////////////////
// File:  D3D8CustomTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for custom D3D 8 test group object.
//
// History:
// 25 Oct 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////

//
// D3D8.dll API prototypes.
//

typedef IDirect3D8 * (WINAPI *PDIRECT3DCREATE8)
(
	UINT SDKVersion
);


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D8CustomTest
//
// Purpose:
// Direct3D 8 custom test group object.
///////////////////////////////////////////////////////
template < typename AppClass >
class CD3D8CustomTest
:
	public CCustomTest< AppClass, CTest >
{
	//
	// Construction/destruction
	//

	public:
		CD3D8CustomTest
		(
			AppClass * pApplication
		);

		//virtual ~CD3D8CustomTest();


	//
	// Direct3D initialization and cleanup methods.
	//

	public:
		HRESULT LoadD3D8();

		HRESULT UnloadD3D8();

		HRESULT InitializeD3D8
		(
			bool bDebugHelper = false
		);

		HRESULT CleanupD3D8();


	//
	// Device management functions
	//

	public:
		// Frees Direct3D 8 resources.
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
		// Direct3D 8 members
		//

		IDirect3D8 * m_pD3D8;

		// D3D8.dll handle and entry point.
		HMODULE m_hD3D8;
		PDIRECT3DCREATE8 m_pDirect3DCreate8;
};


