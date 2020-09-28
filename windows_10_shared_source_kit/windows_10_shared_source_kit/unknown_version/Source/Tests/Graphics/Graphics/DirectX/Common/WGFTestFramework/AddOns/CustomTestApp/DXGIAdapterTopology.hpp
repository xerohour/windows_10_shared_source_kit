//////////////////////////////////////////////////////////////////////
// File:  DXGIAdapterTopology.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a helper object to keep track of the DXGI adapter
// and output topology.
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
// Framework headers
//

#include <DXGITest.h>
#include <D3DWrappers.h>

#define MAX_OUTPUTS     10

/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CDXGIAdapterTopology
//
// Purpose:
// Manages information about the DXGI adapters and
// outputs connected to a machine.
///////////////////////////////////////////////////////
class CDXGIAdapterTopology
{
	public:
		CDXGIAdapterTopology();

		//~CDXGIAdapterTopology()


	public:
		HRESULT EnumerateAdaptersAndOutputs
		(
			__inout DXGI_FACTORY_POINTER * pFactory,
			__in HMODULE hRasterizerDLL,
			IN D3D10_DRIVER_TYPE DriverType
		);


	public:
		// DXGI adapter information.
		UINT m_cAdapters;

		UINT m_AdapterOrdinalForAdapterSlot[ 10 ];
		UINT m_OutputsForAdapterSlot[ 10 ];

		DXGI_ADAPTER_DESC1 m_AdapterDescriptors[ 10 ];


		// DXGI output information.
		UINT m_cOutputs;

		UINT m_OutputOrdinalForOutputSlot[ MAX_OUTPUTS ];
		UINT m_AdapterSlotForOutputSlot[ 10 ];

		DXGI_OUTPUT_DESC m_OutputDescriptors[ 10 ];
};
