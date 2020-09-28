//////////////////////////////////////////////////////////////////////
// File:  DXGIToGDIAdapterTopologyMap.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a helper object that creates a map between a DXGI
// adapter and output topology and a GDI adapter and output topology.
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
#include <basetsd.h>


//
// Direct3D headers
//

#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dx10.h>


//
// Project headers
//

#include "GDIAdapterTopology.hpp"
#include "DXGIAdapterTopology.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CDXGIToGDIAdapterTopologyMap
//
// Purpose:
// Builds a mapping from DXGI adapters and outputs to
// GDI adapters and outputs.
///////////////////////////////////////////////////////
class CDXGIToGDIAdapterTopologyMap
{
	public:
		CDXGIToGDIAdapterTopologyMap();

		//~CDXGIToGDIAdapterTopologyMap()


	public:
		HRESULT MapAdapterTopologies
		(
			CGDIAdapterTopology & GDITopology,
			CDXGIAdapterTopology & DXGITopology
		);


	public:
		UINT m_GDIDeviceSlotForDXGIAdapterSlot[ 10 ];
		UINT m_GDIDeviceSlotForDXGIOutputSlot[ 10 ];

		UINT m_DXGIAdapterSlotForGDIDeviceSlot[ 10 ];
		UINT m_DXGIOutputSlotForGDIDeviceSlot[ 10 ];
};
