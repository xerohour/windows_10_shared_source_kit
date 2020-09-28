//////////////////////////////////////////////////////////////////////
// File:  D3D9ToGDIAdapterTopologyMap.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a helper object that creates a map between a
// Direct3D adapter topology and a GDI adapter and output topology.
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


//
// Project headers
//

#include "GDIAdapterTopology.hpp"
#include "D3D9AdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D9ToGDIAdapterTopologyMap
//
// Purpose:
// Builds a mapping from Direct3D adapters to GDI
// adapters and outputs.
///////////////////////////////////////////////////////
class CD3D9ToGDIAdapterTopologyMap
{
	public:
		CD3D9ToGDIAdapterTopologyMap();

		//~CD3D9ToGDIAdapterTopologyMap()


	public:
		HRESULT MapAdapterTopologies
		(
			CGDIAdapterTopology & GDITopology,
			CD3D9AdapterTopology & D3D9Topology
		);


	public:
		UINT m_GDIDeviceSlotForD3D9AdapterSlot[ 30 ];
		UINT m_D3D9AdapterSlotForGDIDeviceSlot[ 30 ];
};
