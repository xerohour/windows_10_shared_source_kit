//////////////////////////////////////////////////////////////////////
// File:  D3D9AdapterTopology.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a helper object to keep track of the D3D 9 adapter
// topology.
//
// History:
// 07 Jan 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// C runtime headers
//

#include <assert.h>


//
// Win32 headers
//

#include <windows.h>
#include <d3d9.h>


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D9AdapterTopology
//
// Purpose:
// Manages information about the Direct3D 9 adapters
// connected to a machine.
///////////////////////////////////////////////////////
class CD3D9AdapterTopology
{
	public:
		CD3D9AdapterTopology();

		//~CD3D9AdapterTopology()


	public:
		HRESULT EnumerateAdaptersAndOutputs
		(
			IN IDirect3D9 * pEnumerator,
			D3DDEVTYPE DeviceType
		);


	public:
		UINT m_AdapterOrdinalForAdapterSlot[ 10 ];

		D3DADAPTER_IDENTIFIER9 m_AdapterDescriptors[ 10 ];
		D3DCAPS9 m_AdapterCaps[ 10 ];

		UINT m_cAdapters;
};
